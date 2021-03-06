/*
 * Copyright 2012-15 Advanced Micro Devices, Inc.
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

#ifndef __DAL_CLOCK_SOURCE_TYPES_H__
#define __DAL_CLOCK_SOURCE_TYPES_H__

#include "include/signal_types.h"
#include "include/grph_object_ctrl_defs.h"

/* Deep Color Depth affect pixel clock. */
enum deep_color_depth {
	DEEP_COLOR_DEPTH_24 = 0,	/* 8  bits */
	DEEP_COLOR_DEPTH_30,	/* 10 bits */
	DEEP_COLOR_DEPTH_36,	/* 12 bits */
	DEEP_COLOR_DEPTH_48	/* 16 bits */
};

/**
 *  ClockSharingLevel
 *  Enumeration for clock sharing support.
 *  Level <x> means sharing supported on all levels below and including <x>
 */
enum clock_sharing_level {
	CLOCK_SHARING_LEVEL_NOT_SHAREABLE = 0,
	CLOCK_SHARING_LEVEL_DP_MST_SHAREABLE,
	CLOCK_SHARING_LEVEL_DISPLAY_PORT_SHAREABLE
};

/**
 *  Display Port HW De spread of Reference Clock related Parameters structure
 *  Store it once at boot for later usage
  */
struct csdp_ref_clk_ds_params {
	bool hw_dso_n_dp_ref_clk;
/* Flag for HW De Spread enabled (if enabled SS on DP Reference Clock)*/
	uint32_t avg_dp_ref_clk_khz;
/* Average DP Reference clock (in KHz)*/
	uint32_t ss_percentage_on_dp_ref_clk;
/* DP Reference clock SS percentage
 * (not to be mixed with DP IDCLK SS from PLL Settings)*/
	uint32_t ss_percentage_divider;
/* DP Reference clock SS percentage divider */
};

/**
 *  Pixel Clock Parameters structure
 *  These parameters are required as input
 *  when calculating Pixel Clock Dividers for requested Pixel Clock
 */
struct pixel_clk_flags {
	uint32_t ENABLE_SS:1;
	uint32_t DISPLAY_BLANKED:1;
	uint32_t PROGRAM_PIXEL_CLOCK:1;
	uint32_t PROGRAM_ID_CLOCK:1;
};

struct pixel_clk_params {
	uint32_t requested_pix_clk; /* in KHz */
/*> Requested Pixel Clock
 * (based on Video Timing standard used for requested mode)*/
	uint32_t requested_sym_clk; /* in KHz */
/*> Requested Sym Clock (relevant only for display port)*/
	uint32_t dp_ref_clk; /* in KHz */
/*> DP reference clock - calculated only for DP signal for specific cases*/
	struct graphics_object_id encoder_object_id;
/*> Encoder object Id - needed by VBIOS Exec table*/
	enum signal_type signal_type;
/*> signalType -> Encoder Mode - needed by VBIOS Exec table*/
	enum controller_id controller_id;
/*> ControllerId - which controller using this PLL*/
	enum deep_color_depth color_depth;
	struct csdp_ref_clk_ds_params de_spread_params;
/*> de-spread info, relevant only for on-the-fly tune-up pixel rate*/

	uint32_t dvo_cfg;
/*> If DVO, need passing link rate
 * and output 12bit low or 24bit to VBIOS Exec table*/

	enum disp_pll_config disp_pll_cfg;
	struct pixel_clk_flags flags;
};

/**
 *  Pixel Clock Dividers structure with desired Pixel Clock
 *  (adjusted after VBIOS exec table),
 *  with actually calculated Clock and reference Crystal frequency
 */
struct pll_settings {
	uint32_t actual_pix_clk;
	uint32_t adjusted_pix_clk;
	uint32_t calculated_pix_clk;
	uint32_t vco_freq;
	uint32_t reference_freq;
	uint32_t reference_divider;
	uint32_t feedback_divider;
	uint32_t fract_feedback_divider;
	uint32_t pix_clk_post_divider;
	uint32_t ss_percentage;
	bool use_external_clk;
};

#define MAX_PLL_CALC_ERROR 0xFFFFFFFF

#endif
