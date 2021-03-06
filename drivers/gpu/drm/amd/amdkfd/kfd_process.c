/*
 * Copyright 2014 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <linux/mutex.h>
#include <linux/log2.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/amd-iommu.h>
#include <linux/notifier.h>
#include <linux/compat.h>
#include <linux/mm.h>

struct mm_struct;

#include "kfd_priv.h"
#include "kfd_dbgmgr.h"

/*
 * Initial size for the array of queues.
 * The allocated size is doubled each time
 * it is exceeded up to MAX_PROCESS_QUEUES.
 */
#define INITIAL_QUEUE_ARRAY_SIZE 16

/*
 * List of struct kfd_process (field kfd_process).
 * Unique/indexed by mm_struct*
 */
#define KFD_PROCESS_TABLE_SIZE 5 /* bits: 32 entries */
static DEFINE_HASHTABLE(kfd_processes_table, KFD_PROCESS_TABLE_SIZE);
static DEFINE_MUTEX(kfd_processes_mutex);

DEFINE_STATIC_SRCU(kfd_processes_srcu);

static struct workqueue_struct *kfd_process_wq;

struct kfd_process_release_work {
	struct work_struct kfd_work;
	struct kfd_process *p;
};

#define MIN_IDR_ID 1
#define MAX_IDR_ID 0 /*0 - for unlimited*/

static struct kfd_process *find_process(const struct task_struct *thread);
static struct kfd_process *create_process(const struct task_struct *thread);

void kfd_process_create_wq(void)
{
	if (!kfd_process_wq)
		kfd_process_wq = create_workqueue("kfd_process_wq");
}

void kfd_process_destroy_wq(void)
{
	if (kfd_process_wq) {
		flush_workqueue(kfd_process_wq);
		destroy_workqueue(kfd_process_wq);
		kfd_process_wq = NULL;
	}
}

struct kfd_process *kfd_create_process(const struct task_struct *thread)
{
	struct kfd_process *process;

	BUG_ON(!kfd_process_wq);

	if (thread->mm == NULL)
		return ERR_PTR(-EINVAL);

	/* Only the pthreads threading model is supported. */
	if (thread->group_leader->mm != thread->mm)
		return ERR_PTR(-EINVAL);

	/* Take mmap_sem because we call __mmu_notifier_register inside */
	down_write(&thread->mm->mmap_sem);

	/*
	 * take kfd processes mutex before starting of process creation
	 * so there won't be a case where two threads of the same process
	 * create two kfd_process structures
	 */
	mutex_lock(&kfd_processes_mutex);

	/* A prior open of /dev/kfd could have already created the process. */
	process = find_process(thread);
	if (process)
		pr_debug("kfd: process already found\n");

	if (!process)
		process = create_process(thread);

	mutex_unlock(&kfd_processes_mutex);

	up_write(&thread->mm->mmap_sem);

	return process;
}

struct kfd_process *kfd_get_process(const struct task_struct *thread)
{
	struct kfd_process *process;

	if (thread->mm == NULL)
		return ERR_PTR(-EINVAL);

	/* Only the pthreads threading model is supported. */
	if (thread->group_leader->mm != thread->mm)
		return ERR_PTR(-EINVAL);

	process = find_process(thread);

	return process;
}

static struct kfd_process *find_process_by_mm(const struct mm_struct *mm)
{
	struct kfd_process *process;

	hash_for_each_possible_rcu(kfd_processes_table, process,
					kfd_processes, (uintptr_t)mm)
		if (process->mm == mm)
			return process;

	return NULL;
}

static struct kfd_process *find_process(const struct task_struct *thread)
{
	struct kfd_process *p;
	int idx;

	idx = srcu_read_lock(&kfd_processes_srcu);
	p = find_process_by_mm(thread->mm);
	srcu_read_unlock(&kfd_processes_srcu, idx);

	return p;
}

static void kfd_process_wq_release(struct work_struct *work)
{
	struct kfd_process_release_work *my_work;
	struct kfd_process_device *pdd, *temp, *peer_pdd;
	struct kfd_process *p;
	void *mem;
	int id;

	my_work = (struct kfd_process_release_work *) work;

	p = my_work->p;

	pr_debug("Releasing process (pasid %d) in workqueue\n",
			p->pasid);

	mutex_lock(&p->mutex);

	list_for_each_entry_safe(pdd, temp, &p->per_device_data,
							per_device_list) {
		pr_debug("Releasing pdd (topology id %d) for process (pasid %d) in workqueue\n",
				pdd->dev->id, p->pasid);

		if (pdd->dev->device_info->is_need_iommu_device)
			amd_iommu_unbind_pasid(pdd->dev->pdev, p->pasid);

		/*
		 * Remove all handles from idr and release appropriate
		 * local memory object
		 */
		idr_for_each_entry(&pdd->alloc_idr, mem, id) {
			idr_remove(&pdd->alloc_idr, id);
			list_for_each_entry(peer_pdd,
				&p->per_device_data, per_device_list) {
					pdd->dev->kfd2kgd->unmap_memory_to_gpu(
						peer_pdd->dev->kgd,
						mem, peer_pdd->vm);
			}
			pdd->dev->kfd2kgd->free_memory_of_gpu(
							pdd->dev->kgd, mem);
		}
	}

	list_for_each_entry_safe(pdd, temp, &p->per_device_data,
							per_device_list) {
		radeon_flush_tlb(pdd->dev, p->pasid);
		/* Destroy the GPUVM VM context */
		if (pdd->vm)
			pdd->dev->kfd2kgd->destroy_process_vm(
				pdd->dev->kgd, pdd->vm);
		list_del(&pdd->per_device_list);
		kfree(pdd);
	}

	kfd_event_free_process(p);

	kfd_pasid_free(p->pasid);

	mutex_unlock(&p->mutex);

	mutex_destroy(&p->mutex);

	kfree(p->queues);

	kfree(p);

	kfree((void *)work);
}

static void kfd_process_destroy_delayed(struct rcu_head *rcu)
{
	struct kfd_process_release_work *work;
	struct kfd_process *p;

	BUG_ON(!kfd_process_wq);

	p = container_of(rcu, struct kfd_process, rcu);
	BUG_ON(atomic_read(&p->mm->mm_count) <= 0);

	mmdrop(p->mm);

	work = kmalloc(sizeof(struct kfd_process_release_work), GFP_ATOMIC);

	if (work) {
		INIT_WORK((struct work_struct *) work, kfd_process_wq_release);
		work->p = p;
		queue_work(kfd_process_wq, (struct work_struct *) work);
	}
}

static void kfd_process_notifier_release(struct mmu_notifier *mn,
					struct mm_struct *mm)
{
	struct kfd_process *p;
	struct kfd_process_device *pdd = NULL;

	/*
	 * The kfd_process structure can not be free because the
	 * mmu_notifier srcu is read locked
	 */
	p = container_of(mn, struct kfd_process, mmu_notifier);
	BUG_ON(p->mm != mm);

	mutex_lock(&kfd_processes_mutex);
	hash_del_rcu(&p->kfd_processes);
	mutex_unlock(&kfd_processes_mutex);
	synchronize_srcu(&kfd_processes_srcu);

	mutex_lock(&p->mutex);

	/* In case our notifier is called before IOMMU notifier */
	pqm_uninit(&p->pqm);

	/* Iterate over all process device data structure and check
	 * if we should reset all wavefronts */
	list_for_each_entry(pdd, &p->per_device_data, per_device_list)
		if (pdd->reset_wavefronts) {
			pr_warn("amdkfd: Resetting all wave fronts\n");
			dbgdev_wave_reset_wavefronts(pdd->dev, p);
			pdd->reset_wavefronts = false;
		}

	mutex_unlock(&p->mutex);

	/*
	 * Because we drop mm_count inside kfd_process_destroy_delayed
	 * and because the mmu_notifier_unregister function also drop
	 * mm_count we need to take an extra count here.
	 */
	atomic_inc(&p->mm->mm_count);
	mmu_notifier_unregister_no_release(&p->mmu_notifier, p->mm);
	mmu_notifier_call_srcu(&p->rcu, &kfd_process_destroy_delayed);
}

static const struct mmu_notifier_ops kfd_process_mmu_notifier_ops = {
	.release = kfd_process_notifier_release,
};

static struct kfd_process *create_process(const struct task_struct *thread)
{
	struct kfd_process *process;
	int err = -ENOMEM;

	process = kzalloc(sizeof(*process), GFP_KERNEL);

	if (!process)
		goto err_alloc_process;

	process->queues = kmalloc_array(INITIAL_QUEUE_ARRAY_SIZE,
					sizeof(process->queues[0]), GFP_KERNEL);
	if (!process->queues)
		goto err_alloc_queues;

	process->pasid = kfd_pasid_alloc();
	if (process->pasid == 0)
		goto err_alloc_pasid;

	mutex_init(&process->mutex);

	process->mm = thread->mm;

	/* register notifier */
	process->mmu_notifier.ops = &kfd_process_mmu_notifier_ops;
	err = __mmu_notifier_register(&process->mmu_notifier, process->mm);
	if (err)
		goto err_mmu_notifier;

	hash_add_rcu(kfd_processes_table, &process->kfd_processes,
			(uintptr_t)process->mm);

	process->lead_thread = thread->group_leader;

	process->queue_array_size = INITIAL_QUEUE_ARRAY_SIZE;

	INIT_LIST_HEAD(&process->per_device_data);

	kfd_event_init_process(process);

	err = pqm_init(&process->pqm, process);
	if (err != 0)
		goto err_process_pqm_init;

	/* init process apertures*/
	process->is_32bit_user_mode = is_compat_task();
	if (kfd_init_apertures(process) != 0)
		goto err_init_apretures;

	return process;

err_init_apretures:
	pqm_uninit(&process->pqm);
err_process_pqm_init:
	hash_del_rcu(&process->kfd_processes);
	synchronize_rcu();
	mmu_notifier_unregister_no_release(&process->mmu_notifier, process->mm);
err_mmu_notifier:
	kfd_pasid_free(process->pasid);
err_alloc_pasid:
	kfree(process->queues);
err_alloc_queues:
	kfree(process);
err_alloc_process:
	return ERR_PTR(err);
}

struct kfd_process_device *kfd_get_process_device_data(struct kfd_dev *dev,
							struct kfd_process *p)
{
	struct kfd_process_device *pdd = NULL;

	list_for_each_entry(pdd, &p->per_device_data, per_device_list)
		if (pdd->dev == dev)
			break;

	return pdd;
}

struct kfd_process_device *kfd_create_process_device_data(struct kfd_dev *dev,
							struct kfd_process *p)
{
	struct kfd_process_device *pdd = NULL;

	pdd = kzalloc(sizeof(*pdd), GFP_KERNEL);
	if (pdd != NULL) {
		pdd->dev = dev;
		INIT_LIST_HEAD(&pdd->qpd.queues_list);
		INIT_LIST_HEAD(&pdd->qpd.priv_queue_list);
		pdd->qpd.dqm = dev->dqm;
		pdd->qpd.pqm = &p->pqm;
		pdd->qpd.evicted = 0;
		pdd->reset_wavefronts = false;
		list_add(&pdd->per_device_list, &p->per_device_data);

		/* Init idr used for memory handle translation */
		idr_init(&pdd->alloc_idr);

		/* Create the GPUVM context for this specific device */
		if (dev->kfd2kgd->create_process_vm(dev->kgd, &pdd->vm)) {
			pr_err("Failed to create process VM object\n");
			list_del(&pdd->per_device_list);
			kfree(pdd);
			pdd = NULL;
		}
	}

	return pdd;
}

/*
 * Direct the IOMMU to bind the process (specifically the pasid->mm)
 * to the device.
 * Unbinding occurs when the process dies or the device is removed.
 *
 * Assumes that the process lock is held.
 */
struct kfd_process_device *kfd_bind_process_to_device(struct kfd_dev *dev,
							struct kfd_process *p)
{
	struct kfd_process_device *pdd;
	int err;

	pdd = kfd_get_process_device_data(dev, p);
	if (!pdd) {
		pr_err("Process device data doesn't exist\n");
		return ERR_PTR(-ENOMEM);
	}

	if (pdd->bound)
		return pdd;

	if (dev->device_info->is_need_iommu_device) {
		err = amd_iommu_bind_pasid(dev->pdev, p->pasid, p->lead_thread);
		if (err < 0)
			return ERR_PTR(err);
	}

	pdd->bound = true;

	return pdd;
}

void kfd_unbind_process_from_device(struct kfd_dev *dev, unsigned int pasid)
{
	struct kfd_process *p;
	struct kfd_process_device *pdd;
	int idx, i;
	long status = -EFAULT;

	BUG_ON(dev == NULL);

	idx = srcu_read_lock(&kfd_processes_srcu);

	hash_for_each_rcu(kfd_processes_table, i, p, kfd_processes)
		if (p->pasid == pasid)
			break;

	srcu_read_unlock(&kfd_processes_srcu, idx);

	BUG_ON(p->pasid != pasid);

	mutex_lock(&p->mutex);

	mutex_lock(get_dbgmgr_mutex());

	if ((dev->dbgmgr) && (dev->dbgmgr->pasid == p->pasid)) {

		status = kfd_dbgmgr_unregister(dev->dbgmgr, p);
		if (status == 0) {
			kfd_dbgmgr_destroy(dev->dbgmgr);
			dev->dbgmgr = NULL;
		}
	}

	mutex_unlock(get_dbgmgr_mutex());

	pqm_uninit(&p->pqm);

	pdd = kfd_get_process_device_data(dev, p);
	if (pdd->reset_wavefronts) {
		dbgdev_wave_reset_wavefronts(pdd->dev, p);
		pdd->reset_wavefronts = false;
	}

	/*
	 * Just mark pdd as unbound, because we still need it to call
	 * amd_iommu_unbind_pasid() in when the process exits.
	 * We don't call amd_iommu_unbind_pasid() here
	 * because the IOMMU called us.
	 */
	if (pdd)
		pdd->bound = false;

	mutex_unlock(&p->mutex);
}

struct kfd_process_device *kfd_get_first_process_device_data(struct kfd_process *p)
{
	return list_first_entry(&p->per_device_data,
				struct kfd_process_device,
				per_device_list);
}

struct kfd_process_device *kfd_get_next_process_device_data(struct kfd_process *p,
						struct kfd_process_device *pdd)
{
	if (list_is_last(&pdd->per_device_list, &p->per_device_data))
		return NULL;
	return list_next_entry(pdd, per_device_list);
}

bool kfd_has_process_device_data(struct kfd_process *p)
{
	return !(list_empty(&p->per_device_data));
}

/* Create specific handle mapped to mem from process local memory idr
 * Assumes that the process lock is held. */
int kfd_process_device_create_obj_handle(struct kfd_process_device *pdd, void *mem)
{
	int handle;

	BUG_ON(pdd == NULL);
	BUG_ON(mem == NULL);

	idr_preload(GFP_KERNEL);

	handle = idr_alloc(&pdd->alloc_idr, mem, MIN_IDR_ID, MAX_IDR_ID, GFP_NOWAIT);

	idr_preload_end();

	return handle;
}

/* Translate specific handle from process local memory idr
 * Assumes that the process lock is held. */
void *kfd_process_device_translate_handle(struct kfd_process_device *pdd, int handle)
{
	BUG_ON(pdd == NULL);

	if (handle < 0)
		return NULL;

	return idr_find(&pdd->alloc_idr, handle);
}

/* Remove specific handle from process local memory idr
 * Assumes that the process lock is held. */
void kfd_process_device_remove_obj_handle(struct kfd_process_device *pdd, int handle)
{
	BUG_ON(pdd == NULL);

	if (handle < 0)
		return;

	idr_remove(&pdd->alloc_idr, handle);
}

/* This returns with process->mutex locked. */
struct kfd_process *kfd_lookup_process_by_pasid(unsigned int pasid)
{
	struct kfd_process *p;
	unsigned int temp;

	int idx = srcu_read_lock(&kfd_processes_srcu);

	hash_for_each_rcu(kfd_processes_table, temp, p, kfd_processes) {
		if (p->pasid == pasid) {
			mutex_lock(&p->mutex);
			break;
		}
	}

	srcu_read_unlock(&kfd_processes_srcu, idx);

	return p;
}

int kfd_reserved_mem_mmap(struct kfd_process *process, struct vm_area_struct *vma)
{
	unsigned long pfn, i;
	int ret = 0;
	struct kfd_dev *dev = kfd_device_by_id(vma->vm_pgoff);

	if (dev == NULL)
		return -EINVAL;
	if ((vma->vm_start & (PAGE_SIZE - 1)) ||
		(vma->vm_end & (PAGE_SIZE - 1))) {
		pr_err("KFD only support page aligned memory map.\n");
		return -EINVAL;
	}

	pr_debug("kfd reserved mem mmap been called.\n");
	/* We supported  two reserved memory mmap in the future .
	    1. Trap handler code and parameter (TBA and TMA , 2 pages total)
	    2. Relaunch stack (control  block, 1 page for Carrizo)
	 */

	for (i = 0; i < ((vma->vm_end - vma->vm_start) >> PAGE_SHIFT); ++i) {
		pfn = page_to_pfn(&dev->cwsr_pages[i]);
		vma->vm_flags |= VM_IO | VM_DONTCOPY | VM_DONTEXPAND
			| VM_NORESERVE | VM_DONTDUMP | VM_PFNMAP;
		/* mapping the page to user process */
		ret = remap_pfn_range(vma, vma->vm_start + (i << PAGE_SHIFT),
				pfn, PAGE_SIZE, vma->vm_page_prot);
		if (ret)
			break;
	}
	return ret;
}

