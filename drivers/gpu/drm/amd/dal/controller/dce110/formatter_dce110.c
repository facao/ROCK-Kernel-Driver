/*
 * Copyright 2012-15 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
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

#include "dal_services.h"

#include "dce/dce_11_0_d.h"
#include "dce/dce_11_0_sh_mask.h"

#include "formatter_dce110.h"

#define regs_for_formatter(id) \
[CONTROLLER_ID_D ## id - 1] = {\
[IDX_FMT_BIT_DEPTH_CONTROL] = mmFMT ## id ## _FMT_BIT_DEPTH_CONTROL,\
[IDX_FMT_DITHER_RAND_R_SEED] = mmFMT ## id ## _FMT_DITHER_RAND_R_SEED,\
[IDX_FMT_DITHER_RAND_G_SEED] = mmFMT ## id ## _FMT_DITHER_RAND_G_SEED,\
[IDX_FMT_DITHER_RAND_B_SEED] = mmFMT ## id ## _FMT_DITHER_RAND_B_SEED,\
[IDX_FMT_TEMPORAL_DITHER_PATTERN_CONTROL] =\
	mmFMT ## id ## _FMT_TEMPORAL_DITHER_PATTERN_CONTROL,\
[IDX_FMT_TEMPORAL_DITHER_PATTERN_S_MATRIX] =\
	mmFMT ## id ## _FMT_TEMPORAL_DITHER_PROGRAMMABLE_PATTERN_S_MATRIX,\
[IDX_FMT_TEMPORAL_DITHER_PATTERN_T_MATRIX] =\
	mmFMT ## id ## _FMT_TEMPORAL_DITHER_PROGRAMMABLE_PATTERN_T_MATRIX,\
[IDX_FMT_CLAMP_CNTL] = mmFMT ## id ## _FMT_CLAMP_CNTL,\
[IDX_FMT_CONTROL] = mmFMT ## id ## _FMT_CONTROL,\
[IDX_FMT_DYNAMIC_EXP_CNTL] = mmFMT ## id ## _FMT_DYNAMIC_EXP_CNTL,\
[IDX_FMT_CLAMP_COMPONENT_R] = mmFMT ## id ## _FMT_CLAMP_COMPONENT_R,\
[IDX_FMT_CLAMP_COMPONENT_G] = mmFMT ## id ## _FMT_CLAMP_COMPONENT_G,\
[IDX_FMT_CLAMP_COMPONENT_B] = mmFMT ## id ## _FMT_CLAMP_COMPONENT_B,\
}

enum fmt_regs_idx {
	IDX_FMT_BIT_DEPTH_CONTROL,
	IDX_FMT_DITHER_RAND_R_SEED,
	IDX_FMT_DITHER_RAND_G_SEED,
	IDX_FMT_DITHER_RAND_B_SEED,
	IDX_FMT_TEMPORAL_DITHER_PATTERN_CONTROL,
	IDX_FMT_TEMPORAL_DITHER_PATTERN_S_MATRIX,
	IDX_FMT_TEMPORAL_DITHER_PATTERN_T_MATRIX,
	IDX_FMT_CLAMP_CNTL,
	IDX_FMT_CONTROL,
	IDX_FMT_DYNAMIC_EXP_CNTL,
	IDX_FMT_CLAMP_COMPONENT_R,
	IDX_FMT_CLAMP_COMPONENT_G,
	IDX_FMT_CLAMP_COMPONENT_B,
	FMT_REGS_IDX_SIZE
};

static const uint32_t fmt_regs[][FMT_REGS_IDX_SIZE] = {
	regs_for_formatter(0),
	regs_for_formatter(1),
	regs_for_formatter(2)
};

static bool formatter_dce110_construct(
	struct formatter *fmt,
	struct formatter_init_data *init_data);

struct formatter *dal_formatter_dce110_create(
	struct formatter_init_data *init_data)
{
	struct formatter *fmt = dal_alloc(sizeof(struct formatter));

	if (!fmt)
		return NULL;

	if (formatter_dce110_construct(fmt, init_data))
		return fmt;

	dal_free(fmt);
	BREAK_TO_DEBUGGER();
	return NULL;
}

static void destroy(struct formatter **fmt)
{
	dal_free(*fmt);
	*fmt = NULL;
}

static void set_dyn_expansion(
	struct formatter *fmt,
	enum color_space color_sp,
	enum color_depth color_dpth,
	enum signal_type signal)
{
	uint32_t value;
	bool enable_dyn_exp = false;

	value = dal_read_reg(fmt->ctx,
			fmt->regs[IDX_FMT_DYNAMIC_EXP_CNTL]);

	set_reg_field_value(value, 0,
		FMT_DYNAMIC_EXP_CNTL, FMT_DYNAMIC_EXP_EN);
	set_reg_field_value(value, 0,
		FMT_DYNAMIC_EXP_CNTL, FMT_DYNAMIC_EXP_MODE);



	/* From HW programming guide:
		FMT_DYNAMIC_EXP_EN = 0 for limited RGB or YCbCr output
		FMT_DYNAMIC_EXP_EN = 1 for RGB full range only*/
	if (color_sp == COLOR_SPACE_SRGB_FULL_RANGE)
		enable_dyn_exp = true;

	/*00 - 10-bit -> 12-bit dynamic expansion*/
	/*01 - 8-bit  -> 12-bit dynamic expansion*/
	if (signal == SIGNAL_TYPE_HDMI_TYPE_A) {
		switch (color_dpth) {
		case COLOR_DEPTH_24:
			set_reg_field_value(value, enable_dyn_exp ? 1:0,
				FMT_DYNAMIC_EXP_CNTL, FMT_DYNAMIC_EXP_EN);
			set_reg_field_value(value, 1,
				FMT_DYNAMIC_EXP_CNTL, FMT_DYNAMIC_EXP_MODE);
			break;
		case COLOR_DEPTH_30:
			set_reg_field_value(value, enable_dyn_exp ? 1:0,
				FMT_DYNAMIC_EXP_CNTL, FMT_DYNAMIC_EXP_EN);
			set_reg_field_value(value, 0,
				FMT_DYNAMIC_EXP_CNTL, FMT_DYNAMIC_EXP_MODE);
			break;
		case COLOR_DEPTH_36:
			break;
		default:
			break;
		}
	}

	dal_write_reg(fmt->ctx,
			fmt->regs[IDX_FMT_DYNAMIC_EXP_CNTL],
			value);
}

/**
 *	set_truncation
 *	1) set truncation depth: 0 for 18 bpp or 1 for 24 bpp
 *	2) enable truncation
 *	3) HW remove 12bit FMT support for DCE11 power saving reason.
 */
static void set_truncation(
		struct formatter *fmt,
		const struct bit_depth_reduction_params *params)
{
	uint32_t value = 0;

	/*Disable truncation*/
	value = dal_read_reg(fmt->ctx,
		fmt->regs[IDX_FMT_BIT_DEPTH_CONTROL]);
	set_reg_field_value(value, 0,
		FMT_BIT_DEPTH_CONTROL, FMT_TRUNCATE_EN);
	set_reg_field_value(value, 0,
		FMT_BIT_DEPTH_CONTROL, FMT_TRUNCATE_DEPTH);
	set_reg_field_value(value, 0,
		FMT_BIT_DEPTH_CONTROL, FMT_TRUNCATE_MODE);

	dal_write_reg(fmt->ctx,
		fmt->regs[IDX_FMT_BIT_DEPTH_CONTROL], value);

	/* no 10bpc trunc on DCE11*/
	if (params->flags.TRUNCATE_ENABLED == 0 ||
		params->flags.TRUNCATE_DEPTH == 2)
		return;

	/*Set truncation depth and Enable truncation*/
	set_reg_field_value(value, 1,
		FMT_BIT_DEPTH_CONTROL, FMT_TRUNCATE_EN);
	set_reg_field_value(value, params->flags.TRUNCATE_MODE,
		FMT_BIT_DEPTH_CONTROL, FMT_TRUNCATE_MODE);
	set_reg_field_value(value, params->flags.TRUNCATE_DEPTH,
		FMT_BIT_DEPTH_CONTROL, FMT_TRUNCATE_DEPTH);

	dal_write_reg(fmt->ctx,
		fmt->regs[IDX_FMT_BIT_DEPTH_CONTROL], value);

}

/**
 *	set_spatial_dither
 *	1) set spatial dithering mode: pattern of seed
 *	2) set spatical dithering depth: 0 for 18bpp or 1 for 24bpp
 *	3) set random seed
 *	4) set random mode
 *		lfsr is reset every frame or not reset
 *		RGB dithering method
 *		0: RGB data are all dithered with x^28+x^3+1
 *		1: R data is dithered with x^28+x^3+1
 *		G data is dithered with x^28+X^9+1
 *		B data is dithered with x^28+x^13+1
 *		enable high pass filter or not
 *	5) enable spatical dithering
 */
static void set_spatial_dither(
	struct formatter *fmt,
	const struct bit_depth_reduction_params *params)
{
	uint32_t depth_cntl_value = 0;
	uint32_t fmt_cntl_value = 0;
	uint32_t dither_r_value = 0;
	uint32_t dither_g_value = 0;
	uint32_t dither_b_value = 0;

	/*Disable spatial (random) dithering*/
	depth_cntl_value = dal_read_reg(fmt->ctx,
		fmt->regs[IDX_FMT_BIT_DEPTH_CONTROL]);

	fmt_cntl_value = dal_read_reg(fmt->ctx,
		fmt->regs[IDX_FMT_CONTROL]);
	set_reg_field_value(depth_cntl_value, 0,
		FMT_BIT_DEPTH_CONTROL, FMT_SPATIAL_DITHER_EN);
	set_reg_field_value(depth_cntl_value, 0,
		FMT_BIT_DEPTH_CONTROL, FMT_SPATIAL_DITHER_MODE);
	set_reg_field_value(depth_cntl_value, 0,
		FMT_BIT_DEPTH_CONTROL, FMT_SPATIAL_DITHER_DEPTH);
	set_reg_field_value(depth_cntl_value, 0,
		FMT_BIT_DEPTH_CONTROL, FMT_TEMPORAL_DITHER_EN);
	set_reg_field_value(depth_cntl_value, 0,
		FMT_BIT_DEPTH_CONTROL, FMT_HIGHPASS_RANDOM_ENABLE);
	set_reg_field_value(depth_cntl_value, 0,
		FMT_BIT_DEPTH_CONTROL, FMT_FRAME_RANDOM_ENABLE);
	set_reg_field_value(depth_cntl_value, 0,
		FMT_BIT_DEPTH_CONTROL, FMT_RGB_RANDOM_ENABLE);

	dal_write_reg(fmt->ctx,
		fmt->regs[IDX_FMT_BIT_DEPTH_CONTROL], depth_cntl_value);

	/* no 10bpc on DCE11*/
	if (params->flags.SPATIAL_DITHER_ENABLED == 0 ||
		params->flags.SPATIAL_DITHER_DEPTH == 2)
		return;

	/* only use FRAME_COUNTER_MAX if frameRandom == 1*/
	if (params->flags.FRAME_RANDOM == 1) {
		if (params->flags.SPATIAL_DITHER_DEPTH == 0 ||
		params->flags.SPATIAL_DITHER_DEPTH == 1) {
			set_reg_field_value(fmt_cntl_value, 15,
				FMT_CONTROL,
				FMT_SPATIAL_DITHER_FRAME_COUNTER_MAX);
			set_reg_field_value(fmt_cntl_value, 2,
				FMT_CONTROL,
				FMT_SPATIAL_DITHER_FRAME_COUNTER_BIT_SWAP);
		} else if (params->flags.SPATIAL_DITHER_DEPTH == 2) {
			set_reg_field_value(fmt_cntl_value, 3,
				FMT_CONTROL,
				FMT_SPATIAL_DITHER_FRAME_COUNTER_MAX);
			set_reg_field_value(fmt_cntl_value, 1,
				FMT_CONTROL,
				FMT_SPATIAL_DITHER_FRAME_COUNTER_BIT_SWAP);
		} else
			return;
	} else {
		set_reg_field_value(fmt_cntl_value, 0,
			FMT_CONTROL,
			FMT_SPATIAL_DITHER_FRAME_COUNTER_MAX);
		set_reg_field_value(fmt_cntl_value, 0,
			FMT_CONTROL,
			FMT_SPATIAL_DITHER_FRAME_COUNTER_BIT_SWAP);
	}

	dal_write_reg(fmt->ctx, fmt->regs[IDX_FMT_CONTROL],
			fmt_cntl_value);

	/*Set seed for random values for
	 * spatial dithering for R,G,B channels*/
	set_reg_field_value(dither_r_value, params->r_seed_value,
		FMT_DITHER_RAND_R_SEED,
		FMT_RAND_R_SEED);

	dal_write_reg(fmt->ctx,
		fmt->regs[IDX_FMT_DITHER_RAND_R_SEED],
		dither_r_value);

	set_reg_field_value(dither_g_value,
		params->g_seed_value,
		FMT_DITHER_RAND_G_SEED,
		FMT_RAND_G_SEED);

	dal_write_reg(fmt->ctx,
		fmt->regs[IDX_FMT_DITHER_RAND_G_SEED],
		dither_g_value);

	set_reg_field_value(dither_b_value, params->b_seed_value,
		FMT_DITHER_RAND_B_SEED,
		FMT_RAND_B_SEED);

	dal_write_reg(fmt->ctx,
		fmt->regs[IDX_FMT_DITHER_RAND_B_SEED],
		dither_b_value);

	/* FMT_OFFSET_R_Cr  31:16 0x0 Setting the zero
	 * offset for the R/Cr channel, lower 4LSB
	 * is forced to zeros. Typically set to 0
	 * RGB and 0x80000 YCbCr.
	 */
	/* FMT_OFFSET_G_Y   31:16 0x0 Setting the zero
	 * offset for the G/Y  channel, lower 4LSB is
	 * forced to zeros. Typically set to 0 RGB
	 * and 0x80000 YCbCr.
	 */
	/* FMT_OFFSET_B_Cb  31:16 0x0 Setting the zero
	 * offset for the B/Cb channel, lower 4LSB is
	 * forced to zeros. Typically set to 0 RGB and
	 * 0x80000 YCbCr.
	 */

	/*Set spatial dithering bit depth*/
	set_reg_field_value(depth_cntl_value,
		params->flags.SPATIAL_DITHER_DEPTH,
		FMT_BIT_DEPTH_CONTROL,
		FMT_SPATIAL_DITHER_DEPTH);

	/* Set spatial dithering mode
	 * (default is Seed patterrn AAAA...)
	 */
	set_reg_field_value(depth_cntl_value,
		params->flags.SPATIAL_DITHER_MODE,
		FMT_BIT_DEPTH_CONTROL,
		FMT_SPATIAL_DITHER_MODE);

	/*Reset only at startup*/
	set_reg_field_value(depth_cntl_value,
		params->flags.FRAME_RANDOM,
		FMT_BIT_DEPTH_CONTROL,
		FMT_RGB_RANDOM_ENABLE);

	/*Set RGB data dithered with x^28+x^3+1*/
	set_reg_field_value(depth_cntl_value,
		params->flags.RGB_RANDOM,
		FMT_BIT_DEPTH_CONTROL,
		FMT_RGB_RANDOM_ENABLE);

	/*Disable High pass filter*/
	set_reg_field_value(depth_cntl_value,
		params->flags.HIGHPASS_RANDOM,
		FMT_BIT_DEPTH_CONTROL,
		FMT_HIGHPASS_RANDOM_ENABLE);

	/*Enable spatial dithering*/
	set_reg_field_value(depth_cntl_value,
		1,
		FMT_BIT_DEPTH_CONTROL,
		FMT_SPATIAL_DITHER_EN);

	dal_write_reg(fmt->ctx,
		fmt->regs[IDX_FMT_BIT_DEPTH_CONTROL],
		depth_cntl_value);

}

/**
 *	SetTemporalDither (Frame Modulation)
 *	1) set temporal dither depth
 *	2) select pattern: from hard-coded pattern or programmable pattern
 *	3) select optimized strips for BGR or RGB LCD sub-pixel
 *	4) set s matrix
 *	5) set t matrix
 *	6) set grey level for 0.25, 0.5, 0.75
 *	7) enable temporal dithering
 */
static void set_temporal_dither(
	struct formatter *fmt,
	const struct bit_depth_reduction_params *params)
{
	uint32_t value;

	/*Disable temporal (frame modulation) dithering first*/
	value = dal_read_reg(fmt->ctx,
			fmt->regs[IDX_FMT_BIT_DEPTH_CONTROL]);

	set_reg_field_value(value,
		0,
		FMT_BIT_DEPTH_CONTROL,
		FMT_TEMPORAL_DITHER_EN);

	set_reg_field_value(value,
		0,
		FMT_BIT_DEPTH_CONTROL,
		FMT_TEMPORAL_DITHER_RESET);
	set_reg_field_value(value,
		0,
		FMT_BIT_DEPTH_CONTROL,
		FMT_TEMPORAL_DITHER_OFFSET);
	set_reg_field_value(value,
		0,
		FMT_BIT_DEPTH_CONTROL,
		FMT_TEMPORAL_DITHER_DEPTH);
	set_reg_field_value(value,
		0,
		FMT_BIT_DEPTH_CONTROL,
		FMT_TEMPORAL_LEVEL);
	set_reg_field_value(value,
		0,
		FMT_BIT_DEPTH_CONTROL,
		FMT_25FRC_SEL);

	set_reg_field_value(value,
		0,
		FMT_BIT_DEPTH_CONTROL,
		FMT_50FRC_SEL);

	set_reg_field_value(value,
		0,
		FMT_BIT_DEPTH_CONTROL,
		FMT_75FRC_SEL);

	dal_write_reg(fmt->ctx,
		fmt->regs[IDX_FMT_BIT_DEPTH_CONTROL], value);

	/* no 10bpc dither on DCE11*/
	if (params->flags.FRAME_MODULATION_ENABLED == 0 ||
		params->flags.FRAME_MODULATION_DEPTH == 2)
		return;

	/* Set temporal dithering depth*/
	set_reg_field_value(value,
		params->flags.FRAME_MODULATION_DEPTH,
		FMT_BIT_DEPTH_CONTROL,
		FMT_TEMPORAL_DITHER_DEPTH);

	set_reg_field_value(value,
		0,
		FMT_BIT_DEPTH_CONTROL,
		FMT_TEMPORAL_DITHER_RESET);

	set_reg_field_value(value,
		0,
		FMT_BIT_DEPTH_CONTROL,
		FMT_TEMPORAL_DITHER_OFFSET);

	/*Select legacy pattern based on FRC and Temporal level*/
	dal_write_reg(fmt->ctx,
		fmt->regs[IDX_FMT_TEMPORAL_DITHER_PATTERN_CONTROL], 0);
	/*Set s matrix*/
	dal_write_reg(fmt->ctx,
		fmt->regs[IDX_FMT_TEMPORAL_DITHER_PATTERN_S_MATRIX], 0);
	/*Set t matrix*/
	dal_write_reg(fmt->ctx,
		fmt->regs[IDX_FMT_TEMPORAL_DITHER_PATTERN_T_MATRIX], 0);

	/*Select patterns for 0.25, 0.5 and 0.75 grey level*/
	set_reg_field_value(value,
		params->flags.TEMPORAL_LEVEL,
		FMT_BIT_DEPTH_CONTROL,
		FMT_TEMPORAL_LEVEL);

	set_reg_field_value(value,
		params->flags.FRC25,
		FMT_BIT_DEPTH_CONTROL,
		FMT_25FRC_SEL);

	set_reg_field_value(value,
		params->flags.FRC50,
		FMT_BIT_DEPTH_CONTROL,
		FMT_50FRC_SEL);

	set_reg_field_value(value,
		params->flags.FRC75,
		FMT_BIT_DEPTH_CONTROL,
		FMT_75FRC_SEL);

	/*Enable bit reduction by temporal (frame modulation) dithering*/
	set_reg_field_value(value,
		1,
		FMT_BIT_DEPTH_CONTROL,
		FMT_TEMPORAL_DITHER_EN);

	dal_write_reg(fmt->ctx,
		fmt->regs[IDX_FMT_BIT_DEPTH_CONTROL],
		value);

}

/**
 *	Set Clamping
 *	1) Set clamping format based on bpc - 0 for 6bpc (No clamping)
 *		1 for 8 bpc
 *		2 for 10 bpc
 *		3 for 12 bpc
 *		7 for programable
 *	2) Enable clamp if Limited range requested
 */
static void set_clamping(
	struct formatter *fmt,
	const struct clamping_and_pixel_encoding_params *params)
{
	uint32_t clamp_cntl_value = 0;
	uint32_t red_clamp_value = 0;
	uint32_t green_clamp_value = 0;
	uint32_t blue_clamp_value = 0;

	clamp_cntl_value = dal_read_reg(fmt->ctx,
			fmt->regs[IDX_FMT_CLAMP_CNTL]);

	set_reg_field_value(clamp_cntl_value,
		0,
		FMT_CLAMP_CNTL,
		FMT_CLAMP_DATA_EN);

	set_reg_field_value(clamp_cntl_value,
		0,
		FMT_CLAMP_CNTL,
		FMT_CLAMP_COLOR_FORMAT);

	switch (params->clamping_level) {
	case CLAMPING_FULL_RANGE:
		break;

	case CLAMPING_LIMITED_RANGE_8BPC:
		set_reg_field_value(clamp_cntl_value,
			1,
			FMT_CLAMP_CNTL,
			FMT_CLAMP_DATA_EN);

		set_reg_field_value(clamp_cntl_value,
			1,
			FMT_CLAMP_CNTL,
			FMT_CLAMP_COLOR_FORMAT);

		break;

	case CLAMPING_LIMITED_RANGE_10BPC:
		set_reg_field_value(clamp_cntl_value,
			1,
			FMT_CLAMP_CNTL,
			FMT_CLAMP_DATA_EN);

		set_reg_field_value(clamp_cntl_value,
			2,
			FMT_CLAMP_CNTL,
			FMT_CLAMP_COLOR_FORMAT);

		break;
	case CLAMPING_LIMITED_RANGE_12BPC:
		set_reg_field_value(clamp_cntl_value,
			1,
			FMT_CLAMP_CNTL,
			FMT_CLAMP_DATA_EN);

		set_reg_field_value(clamp_cntl_value,
			3,
			FMT_CLAMP_CNTL,
			FMT_CLAMP_COLOR_FORMAT);

		break;
	case CLAMPING_LIMITED_RANGE_PROGRAMMABLE:
		set_reg_field_value(clamp_cntl_value,
			1,
			FMT_CLAMP_CNTL,
			FMT_CLAMP_DATA_EN);

		set_reg_field_value(clamp_cntl_value,
			7,
			FMT_CLAMP_CNTL,
			FMT_CLAMP_COLOR_FORMAT);

		/*set the defaults*/
		set_reg_field_value(red_clamp_value,
			0x10,
			FMT_CLAMP_COMPONENT_R,
			FMT_CLAMP_LOWER_R);

		set_reg_field_value(red_clamp_value,
			0xFEF,
			FMT_CLAMP_COMPONENT_R,
			FMT_CLAMP_UPPER_R);

		dal_write_reg(fmt->ctx,
			fmt->regs[IDX_FMT_CLAMP_COMPONENT_R],
			red_clamp_value);

		set_reg_field_value(green_clamp_value,
			0x10,
			FMT_CLAMP_COMPONENT_G,
			FMT_CLAMP_LOWER_G);

		set_reg_field_value(green_clamp_value,
			0xFEF,
			FMT_CLAMP_COMPONENT_G,
			FMT_CLAMP_UPPER_G);

		dal_write_reg(fmt->ctx,
			fmt->regs[IDX_FMT_CLAMP_COMPONENT_G],
			green_clamp_value);

		set_reg_field_value(blue_clamp_value,
			0x10,
			FMT_CLAMP_COMPONENT_B,
			FMT_CLAMP_LOWER_B);

		set_reg_field_value(blue_clamp_value,
			0xFEF,
			FMT_CLAMP_COMPONENT_B,
			FMT_CLAMP_UPPER_B);

		dal_write_reg(fmt->ctx,
			fmt->regs[IDX_FMT_CLAMP_COMPONENT_B],
			blue_clamp_value);

		break;

	default:
		break;
	}

	/*Set clamp control*/
	dal_write_reg(fmt->ctx,
		fmt->regs[IDX_FMT_CLAMP_CNTL],
		clamp_cntl_value);

}

/**
 *	set_pixel_encoding
 *
 *	Set Pixel Encoding
 *		0: RGB 4:4:4 or YCbCr 4:4:4 or YOnly
 *		1: YCbCr 4:2:2
 */

static void set_pixel_encoding(
	struct formatter *fmt,
	const struct clamping_and_pixel_encoding_params *params)
{
	uint32_t fmt_cntl_value;

	/*RGB 4:4:4 or YCbCr 4:4:4 - 0; YCbCr 4:2:2 -1.*/
	fmt_cntl_value = dal_read_reg(fmt->ctx,
			fmt->regs[IDX_FMT_CONTROL]);

	set_reg_field_value(fmt_cntl_value,
		0,
		FMT_CONTROL,
		FMT_PIXEL_ENCODING);

	if (params->pixel_encoding == CNTL_PIXEL_ENCODING_YCBCR422) {
		set_reg_field_value(fmt_cntl_value,
			1,
			FMT_CONTROL,
			FMT_PIXEL_ENCODING);

		/*00 - Pixels drop mode ,01 - Pixels average mode*/
		set_reg_field_value(fmt_cntl_value,
			0,
			FMT_CONTROL,
			FMT_SUBSAMPLING_MODE);

		/*00 - Cb before Cr ,01 - Cr before Cb*/
		set_reg_field_value(fmt_cntl_value,
			0,
			FMT_CONTROL,
			FMT_SUBSAMPLING_ORDER);
	}
	dal_write_reg(fmt->ctx, fmt->regs[IDX_FMT_CONTROL], fmt_cntl_value);

}

static void program_bit_depth_reduction(
	struct formatter *fmt,
	const struct bit_depth_reduction_params *params)
{
	set_truncation(fmt, params);
	set_spatial_dither(fmt, params);
	set_temporal_dither(fmt, params);
}

static void program_clamping_and_pixel_encoding(
	struct formatter *fmt,
	const struct clamping_and_pixel_encoding_params *params)
{
	set_clamping(fmt, params);
	set_pixel_encoding(fmt, params);
}

/**
 *	setup_stereo_polarity: Programs fields relating
 *	to the FMT Block
 *	This function is only called by
 *	DisplayController::EnableStereo
 */
static void setup_stereo_polarity(
	struct formatter *fmt,
	enum fmt_stereo_action action,
	bool right_eye_polarity)
{
	uint32_t fmt_cntl_value;

	fmt_cntl_value = dal_read_reg(fmt->ctx,
			fmt->regs[IDX_FMT_CONTROL]);

	switch (action) {
	case FMT_STEREO_ACTION_ENABLE:
		set_reg_field_value(fmt_cntl_value,
			1,
			FMT_CONTROL,
			FMT_STEREOSYNC_OVERRIDE);

		set_reg_field_value(fmt_cntl_value,
			right_eye_polarity ? 1:0,
			FMT_CONTROL,
			FMT_STEREOSYNC_OVR_POL);
		break;

	case FMT_STEREO_ACTION_DISABLE:
		set_reg_field_value(fmt_cntl_value,
			0,
			FMT_CONTROL,
			FMT_STEREOSYNC_OVERRIDE);
		break;

	case FMT_STEREO_ACTION_UPDATE_POLARITY:
		set_reg_field_value(fmt_cntl_value,
			right_eye_polarity ? 1:0,
			FMT_CONTROL,
			FMT_STEREOSYNC_OVR_POL);
		break;

	default:
		break;
	}

	dal_write_reg(fmt->ctx,
		fmt->regs[IDX_FMT_CONTROL],
		fmt_cntl_value);

}

static const struct formatter_funcs formatter_dce110_funcs = {
	.set_dyn_expansion =
		set_dyn_expansion,
	.program_bit_depth_reduction =
		program_bit_depth_reduction,
	.program_clamping_and_pixel_encoding =
		program_clamping_and_pixel_encoding,
	.setup_stereo_polarity = setup_stereo_polarity,
	.destroy = destroy,
};

static bool formatter_dce110_construct(
	struct formatter *fmt,
	struct formatter_init_data *init_data)
{
	if (!dal_formatter_construct(fmt, init_data))
		return false;

	fmt->regs = fmt_regs[init_data->id - 1];
	fmt->funcs = &formatter_dce110_funcs;
	return true;
}

