/*
 * Copyright 2012-14 Advanced Micro Devices, Inc.
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
 *
 * Authors: AMD
 *
 */

#ifndef __DAL_HDTV_DCO_H__
#define __DAL_HDTV_DCO_H__

struct hdtv_dco;
struct timing_service;
struct adapter_service;
struct dcs;

struct hdtv_dco *dal_hdtv_dco_create_dvi(
	struct timing_service *ts,
	struct adapter_service *as,
	struct dcs *dcs);

struct hdtv_dco *dal_hdtv_dco_create_vga(
	struct timing_service *ts,
	struct adapter_service *as,
	struct dcs *dcs);

struct hdtv_dco *dal_hdtv_dco_create_cv(
	struct timing_service *ts,
	struct adapter_service *as,
	struct dcs *dcs);

#endif /* __DAL_HDTV_DCO_H__ */
