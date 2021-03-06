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

#include "dal_services.h"

#include "monitor_tables.h"

#define DELAY_100MS 100
#define DELAY_120MS 120
#define DELAY_150MS 150
#define DELAY_250MS 250
#define DELAY_500MS 500
#define DELAY_1250MS 1250
#define DELAY_50MS 50

#define MAX_PIXEL_CLOCK_245MHZ 245

#define RETRY_TIMEOUT_2000MS 2000

#define LINK_RATE_LOW 0x06

static const struct monitor_patch_info monitor_patch_table[] = {
	{ MONITOR_MANUFACTURER_ID_1, MONITOR_PRODUCT_ID_1,
		MONITOR_PATCH_TYPE_DO_NOT_USE_DETAILED_TIMING, 0 },
	{ MONITOR_MANUFACTURER_ID_27, MONITOR_PRODUCT_ID_60,
		MONITOR_PATCH_TYPE_DO_NOT_USE_RANGE_LIMITATION, 0 },
	{ MONITOR_MANUFACTURER_ID_4,
		MONITOR_PRODUCT_ID_4,
		MONITOR_PATCH_TYPE_RANDOM_CRT, 0 },
	{ MONITOR_MANUFACTURER_ID_24, MONITOR_PRODUCT_ID_57,
		MONITOR_PATCH_TYPE_VENDOR_1, 0 },
	{ MONITOR_MANUFACTURER_ID_5,
		MONITOR_PRODUCT_ID_13,
		MONITOR_PATCH_TYPE_VENDOR_0, 0 },
	{ MONITOR_MANUFACTURER_ID_10, MONITOR_PRODUCT_ID_22,
			MONITOR_PATCH_TYPE_DO_NOT_USE_EDID_MAX_PIX_CLK,
			MAX_PIXEL_CLOCK_245MHZ },
	{ MONITOR_MANUFACTURER_ID_2, MONITOR_PRODUCT_ID_2,
		MONITOR_PATCH_TYPE_RESTRICT_VESA_MODE_TIMING, 0 },
	{ MONITOR_MANUFACTURER_ID_14,
		MONITOR_PRODUCT_ID_33,
		MONITOR_PATCH_TYPE_HDTV_WITH_PURE_DFP_EDID, 0 },
	{ MONITOR_MANUFACTURER_ID_11, MONITOR_PRODUCT_ID_23,
		MONITOR_PATCH_TYPE_TURN_OFF_DISPLAY_BEFORE_MODE_CHANGE, 0 },
	{ MONITOR_MANUFACTURER_ID_12, MONITOR_PRODUCT_ID_24,
		MONITOR_PATCH_TYPE_TURN_OFF_DISPLAY_BEFORE_MODE_CHANGE, 0 },
	{ MONITOR_MANUFACTURER_ID_4,
		MONITOR_PRODUCT_ID_9,
		MONITOR_PATCH_TYPE_TURN_OFF_DISPLAY_BEFORE_MODE_CHANGE, 0 },
	{ MONITOR_MANUFACTURER_ID_13, MONITOR_PRODUCT_ID_28,
		MONITOR_PATCH_TYPE_TURN_OFF_DISPLAY_BEFORE_MODE_CHANGE,
		DELAY_120MS },
	{ MONITOR_MANUFACTURER_ID_13, MONITOR_PRODUCT_ID_29,
		MONITOR_PATCH_TYPE_TURN_OFF_DISPLAY_BEFORE_MODE_CHANGE,
		DELAY_120MS },
	{ MONITOR_MANUFACTURER_ID_13, MONITOR_PRODUCT_ID_30,
		MONITOR_PATCH_TYPE_TURN_OFF_DISPLAY_BEFORE_MODE_CHANGE,
		DELAY_120MS },
	{ MONITOR_MANUFACTURER_ID_13, MONITOR_PRODUCT_ID_31,
		MONITOR_PATCH_TYPE_TURN_OFF_DISPLAY_BEFORE_MODE_CHANGE,
		DELAY_120MS },
	{ MONITOR_MANUFACTURER_ID_28, MONITOR_PRODUCT_ID_69,
		MONITOR_PATCH_TYPE_TURN_OFF_DISPLAY_BEFORE_MODE_CHANGE,
		DELAY_120MS },
	{ MONITOR_MANUFACTURER_ID_21, MONITOR_PRODUCT_ID_43,
		MONITOR_PATCH_TYPE_TURN_OFF_DISPLAY_BEFORE_MODE_CHANGE, 0 },
	{ MONITOR_MANUFACTURER_ID_21, MONITOR_PRODUCT_ID_44,
		MONITOR_PATCH_TYPE_TURN_OFF_DISPLAY_BEFORE_MODE_CHANGE, 0 },
	{ MONITOR_MANUFACTURER_ID_21, MONITOR_PRODUCT_ID_46,
		MONITOR_PATCH_TYPE_INCREASE_DEFER_WRITE_RETRY_I2C_OVER_AUX, 0 },
	{ MONITOR_MANUFACTURER_ID_23, MONITOR_PRODUCT_ID_56,
		MONITOR_PATCH_TYPE_EDID_EXTENTION_ERROR_CHECK_SUM, 0 },
	{ MONITOR_MANUFACTURER_ID_6, MONITOR_PRODUCT_ID_17,
		MONITOR_PATCH_TYPE_EDID_EXTENTION_ERROR_CHECK_SUM, 0 },
	{ MONITOR_MANUFACTURER_ID_25,
		MONITOR_PRODUCT_ID_59,
		MONITOR_PATCH_TYPE_ERROR_CHECKSUM, 0 },
	{ MONITOR_MANUFACTURER_ID_26, MONITOR_PRODUCT_ID_58,
		MONITOR_PATCH_TYPE_ERROR_CHECKSUM, 0 },
	{ MONITOR_MANUFACTURER_ID_4,
		MONITOR_PRODUCT_ID_5,
		MONITOR_PATCH_TYPE_ERROR_CHECKSUM, 0 },
	{ MONITOR_MANUFACTURER_ID_4, MONITOR_PRODUCT_ID_6,
		MONITOR_PATCH_TYPE_ERROR_CHECKSUM, 0 },
	{ MONITOR_MANUFACTURER_ID_31, MONITOR_PRODUCT_ID_72,
		MONITOR_PATCH_TYPE_LIMIT_PANEL_SUPPORT_RGB_ONLY, 0 },
	{ MONITOR_MANUFACTURER_ID_32, MONITOR_PRODUCT_ID_75,
		MONITOR_PATCH_TYPE_DUAL_EDID_PANEL, 0 },
	{ MONITOR_MANUFACTURER_ID_32, MONITOR_PRODUCT_ID_77,
		MONITOR_PATCH_TYPE_DUAL_EDID_PANEL, 0 },
	{ MONITOR_MANUFACTURER_ID_32, MONITOR_PRODUCT_ID_78,
		MONITOR_PATCH_TYPE_DUAL_EDID_PANEL, 0 },
	{ MONITOR_MANUFACTURER_ID_32, MONITOR_PRODUCT_ID_79,
		MONITOR_PATCH_TYPE_DUAL_EDID_PANEL, 0 },
	{ MONITOR_MANUFACTURER_ID_32, MONITOR_PRODUCT_ID_80,
		MONITOR_PATCH_TYPE_DUAL_EDID_PANEL, 0 },
	{ MONITOR_MANUFACTURER_ID_32, MONITOR_PRODUCT_ID_81,
		MONITOR_PATCH_TYPE_DUAL_EDID_PANEL, 0 },
	{ MONITOR_MANUFACTURER_ID_32, MONITOR_PRODUCT_ID_82,
		MONITOR_PATCH_TYPE_DUAL_EDID_PANEL, 0 },
	{ MONITOR_MANUFACTURER_ID_32, MONITOR_PRODUCT_ID_83,
		MONITOR_PATCH_TYPE_DUAL_EDID_PANEL, 0 },
	{ MONITOR_MANUFACTURER_ID_32, MONITOR_PRODUCT_ID_84,
		MONITOR_PATCH_TYPE_DUAL_EDID_PANEL, 0 },
	{ MONITOR_MANUFACTURER_ID_32, MONITOR_PRODUCT_ID_85,
		MONITOR_PATCH_TYPE_DUAL_EDID_PANEL, 0 },
	{ MONITOR_MANUFACTURER_ID_32, MONITOR_PRODUCT_ID_86,
		MONITOR_PATCH_TYPE_FORCE_LINK_RATE, LINK_RATE_LOW },
	{ MONITOR_MANUFACTURER_ID_32, MONITOR_PRODUCT_ID_89,
		MONITOR_PATCH_TYPE_FORCE_LINK_RATE, LINK_RATE_LOW },
	{ MONITOR_MANUFACTURER_ID_5, MONITOR_PRODUCT_ID_14,
		MONITOR_PATCH_TYPE_IGNORE_19X12_STD_TIMING, 0 },
	{ MONITOR_MANUFACTURER_ID_14, MONITOR_PRODUCT_ID_32,
		MONITOR_PATCH_TYPE_RESET_TX_ON_DISPLAY_POWER_ON, 0 },
	{ MONITOR_MANUFACTURER_ID_22, MONITOR_PRODUCT_ID_62,
		MONITOR_PATCH_TYPE_RESET_TX_ON_DISPLAY_POWER_ON, 0 },
	{ MONITOR_MANUFACTURER_ID_15,
		MONITOR_MANUFACTURER_ID_16,
		MONITOR_PATCH_TYPE_RESET_TX_ON_DISPLAY_POWER_ON, 0 },
	{ MONITOR_MANUFACTURER_ID_17, MONITOR_PRODUCT_ID_34,
		MONITOR_PATCH_TYPE_RESET_TX_ON_DISPLAY_POWER_ON, 0 },
	{ MONITOR_MANUFACTURER_ID_18, MONITOR_PRODUCT_ID_35,
		MONITOR_PATCH_TYPE_RESET_TX_ON_DISPLAY_POWER_ON, 0 },
	{ MONITOR_MANUFACTURER_ID_31, MONITOR_PRODUCT_ID_74,
		MONITOR_PATCH_TYPE_RESET_TX_ON_DISPLAY_POWER_ON, 0 },
	{ MONITOR_MANUFACTURER_ID_22, MONITOR_PRODUCT_ID_64,
		MONITOR_PATCH_TYPE_RESET_TX_ON_DISPLAY_POWER_ON, 0 },
	{ MONITOR_MANUFACTURER_ID_36, MONITOR_PRODUCT_ID_93,
		MONITOR_PATCH_TYPE_RESET_TX_ON_DISPLAY_POWER_ON, 0 },
	{ MONITOR_MANUFACTURER_ID_32, MONITOR_PRODUCT_ID_0,
		MONITOR_PATCH_TYPE_VENDOR_2, 0 },
	{ MONITOR_MANUFACTURER_ID_33, MONITOR_PRODUCT_ID_0,
		MONITOR_PATCH_TYPE_VENDOR_2, 0 },
	{ MONITOR_MANUFACTURER_ID_32, MONITOR_PRODUCT_ID_76,
		MONITOR_PATCH_TYPE_RESET_TX_ON_DISPLAY_POWER_ON, 0 },
	{ MONITOR_MANUFACTURER_ID_35, MONITOR_PRODUCT_ID_90,
		MONITOR_PATCH_TYPE_LIMIT_PANEL_SUPPORT_RGB_ONLY, 0 },
	{ MONITOR_MANUFACTURER_ID_35, MONITOR_PRODUCT_ID_91,
		MONITOR_PATCH_TYPE_INCREASE_DEFER_WRITE_RETRY_I2C_OVER_AUX, 0 },
	{ MONITOR_MANUFACTURER_ID_35, MONITOR_PRODUCT_ID_92,
		MONITOR_PATCH_TYPE_INCREASE_DEFER_WRITE_RETRY_I2C_OVER_AUX, 0 },
	{ MONITOR_MANUFACTURER_ID_4,
		MONITOR_PRODUCT_ID_11,
		MONITOR_PATCH_TYPE_RESTRICT_PROT_DUAL_LINK_DVI, 0 },
	{ MONITOR_MANUFACTURER_ID_4,
		MONITOR_PRODUCT_ID_12,
		MONITOR_PATCH_TYPE_RESTRICT_PROT_DUAL_LINK_DVI, 0 },
	{ MONITOR_MANUFACTURER_ID_10, MONITOR_PRODUCT_ID_21,
		MONITOR_PATCH_TYPE_RESTRICT_PROT_DUAL_LINK_DVI, 0 },
	{ MONITOR_MANUFACTURER_ID_12, MONITOR_PRODUCT_ID_25,
		MONITOR_PATCH_TYPE_DELAY_AFTER_DP_RECEIVER_POWER_UP,
		DELAY_150MS },
	{ MONITOR_MANUFACTURER_ID_32, MONITOR_PRODUCT_ID_88,
		MONITOR_PATCH_TYPE_DELAY_BEFORE_READ_EDID, 25 },
	{ MONITOR_MANUFACTURER_ID_12, MONITOR_PRODUCT_ID_26,
		MONITOR_PATCH_TYPE_DELAY_AFTER_DP_RECEIVER_POWER_UP,
		DELAY_150MS },
	{ MONITOR_MANUFACTURER_ID_32, MONITOR_PRODUCT_ID_87,
		MONITOR_PATCH_TYPE_NO_DEFAULT_TIMINGS, 0 },
	{ MONITOR_MANUFACTURER_ID_22, MONITOR_PRODUCT_ID_65,
		MONITOR_PATCH_TYPE_ADD_CEA861_DETAILED_TIMING_VIC31, 0 },
	{ MONITOR_MANUFACTURER_ID_22, MONITOR_PRODUCT_ID_66,
		MONITOR_PATCH_TYPE_ADD_CEA861_DETAILED_TIMING_VIC16, 0 },
	{ MONITOR_MANUFACTURER_ID_22, MONITOR_PRODUCT_ID_66,
		MONITOR_PATCH_TYPE_ADD_CEA861_DETAILED_TIMING_VIC31, 0 },
	{ MONITOR_MANUFACTURER_ID_22, MONITOR_PRODUCT_ID_67,
		MONITOR_PATCH_TYPE_ADD_CEA861_DETAILED_TIMING_VIC16, 0 },
	{ MONITOR_MANUFACTURER_ID_22, MONITOR_PRODUCT_ID_67,
		MONITOR_PATCH_TYPE_ADD_CEA861_DETAILED_TIMING_VIC31, 0 },
	{ MONITOR_MANUFACTURER_ID_21, MONITOR_PRODUCT_ID_0,
		MONITOR_PATCH_TYPE_DELAY_BEFORE_UNMUTE, DELAY_150MS },
	{ MONITOR_MANUFACTURER_ID_21, MONITOR_PRODUCT_ID_47,
		MONITOR_PATCH_TYPE_RETRY_LINK_TRAINING_ON_FAILURE,
		RETRY_TIMEOUT_2000MS },
	{ MONITOR_MANUFACTURER_ID_22, MONITOR_PRODUCT_ID_67,
		MONITOR_PATCH_TYPE_ALLOW_AUX_WHEN_HPD_LOW, 0 },
	{ MONITOR_MANUFACTURER_ID_22, MONITOR_PRODUCT_ID_66,
		MONITOR_PATCH_TYPE_ALLOW_AUX_WHEN_HPD_LOW, 0 },
	{ MONITOR_MANUFACTURER_ID_22, MONITOR_PRODUCT_ID_68,
		MONITOR_PATCH_TYPE_ALLOW_AUX_WHEN_HPD_LOW, 0 },
	{ MONITOR_MANUFACTURER_ID_17, MONITOR_PRODUCT_ID_36,
		MONITOR_PATCH_TYPE_TILED_DISPLAY, 0 },
	{ MONITOR_MANUFACTURER_ID_14, MONITOR_PRODUCT_ID_38,
		MONITOR_PATCH_TYPE_TILED_DISPLAY, 0 },
	{ MONITOR_MANUFACTURER_ID_14, MONITOR_PRODUCT_ID_37,
		MONITOR_PATCH_TYPE_TILED_DISPLAY, 0 },
	{ MONITOR_MANUFACTURER_ID_21, MONITOR_PRODUCT_ID_39,
		MONITOR_PATCH_TYPE_TILED_DISPLAY, 0 },
	{ MONITOR_MANUFACTURER_ID_21, MONITOR_PRODUCT_ID_40,
		MONITOR_PATCH_TYPE_TILED_DISPLAY, 0 },
	{ MONITOR_MANUFACTURER_ID_21, MONITOR_PRODUCT_ID_41,
		MONITOR_PATCH_TYPE_TILED_DISPLAY, 0 },
	{ MONITOR_MANUFACTURER_ID_20, MONITOR_PRODUCT_ID_42,
		MONITOR_PATCH_TYPE_TILED_DISPLAY, 0 },
	{ MONITOR_MANUFACTURER_ID_2, MONITOR_PRODUCT_ID_2,
		MONITOR_PATCH_TYPE_LARGE_PANEL, 0 },
	{ MONITOR_MANUFACTURER_ID_4,
		MONITOR_PRODUCT_ID_7,
		MONITOR_PATCH_TYPE_LARGE_PANEL, 0 },
	{ MONITOR_MANUFACTURER_ID_4,
		MONITOR_PRODUCT_ID_7_2,
		MONITOR_PATCH_TYPE_LARGE_PANEL, 0 },
	{ MONITOR_MANUFACTURER_ID_7, MONITOR_PRODUCT_ID_18,
		MONITOR_PATCH_TYPE_LARGE_PANEL, 0 },
	{ MONITOR_MANUFACTURER_ID_9, MONITOR_PRODUCT_ID_20,
		MONITOR_PATCH_TYPE_LARGE_PANEL, 0 },
	{ MONITOR_MANUFACTURER_ID_21, MONITOR_PRODUCT_ID_48,
		MONITOR_PATCH_TYPE_VID_STREAM_DIFFER_TO_SYNC, 0 },
	{ MONITOR_MANUFACTURER_ID_21, MONITOR_PRODUCT_ID_49,
		MONITOR_PATCH_TYPE_VID_STREAM_DIFFER_TO_SYNC, 0 },
	{ MONITOR_MANUFACTURER_ID_5, MONITOR_PRODUCT_ID_15,
		MONITOR_PATCH_TYPE_DELAY_AFTER_DP_RECEIVER_POWER_UP,
		DELAY_250MS },

	{ MONITOR_MANUFACTURER_ID_21, MONITOR_PRODUCT_ID_50,
		MONITOR_PATCH_TYPE_EXTRA_DELAY_ON_DISCONNECT, DELAY_500MS },
	{ MONITOR_MANUFACTURER_ID_21, MONITOR_PRODUCT_ID_51,
		MONITOR_PATCH_TYPE_EXTRA_DELAY_ON_DISCONNECT, DELAY_500MS },
	{ MONITOR_MANUFACTURER_ID_37, MONITOR_PRODUCT_ID_94,
		MONITOR_PATCH_TYPE_EXTRA_DELAY_ON_DISCONNECT, DELAY_1250MS },
	{ MONITOR_MANUFACTURER_ID_21, MONITOR_PRODUCT_ID_46_HDMI,
		MONITOR_PATCH_TYPE_EXTRA_DELAY_ON_DISCONNECT, DELAY_250MS },
	{ MONITOR_MANUFACTURER_ID_21, MONITOR_PRODUCT_ID_53,
		MONITOR_PATCH_TYPE_EXTRA_DELAY_ON_DISCONNECT, DELAY_1250MS },
	{ MONITOR_MANUFACTURER_ID_21, MONITOR_PRODUCT_ID_54,
			MONITOR_PATCH_TYPE_DELAY_BEFORE_READ_EDID, DELAY_50MS },
	{ MONITOR_MANUFACTURER_ID_21, MONITOR_PRODUCT_ID_55,
		MONITOR_PATCH_TYPE_EXTRA_DELAY_ON_DISCONNECT, DELAY_1250MS },
	{ MONITOR_MANUFACTURER_ID_12, MONITOR_PRODUCT_ID_27,
			MONITOR_PATCH_TYPE_SINGLE_MODE_PACKED_PIXEL, 0 },
	{ MONITOR_MANUFACTURER_ID_22, MONITOR_PRODUCT_ID_0,
		MONITOR_PATCH_TYPE_DELAY_AFTER_PIXEL_FORMAT_CHANGE,
		DELAY_150MS },
};

uint32_t dal_monitor_tables_get_count(void)
{
	return ARRAY_SIZE(monitor_patch_table);
}

const struct monitor_patch_info *dal_monitor_tables_get_entry_at(uint32_t i)
{
	if (i >= dal_monitor_tables_get_count()) {
		dal_error("%s: incorrect index %d\n", __func__, i);
		return NULL;
	}
	return &monitor_patch_table[i];
}

const struct monitor_patch_info *dal_monitor_tables_find_entry(
	enum monitor_manufacturer_id manufacturer_id,
	enum monitor_product_id product_id,
	enum monitor_patch_type patch_type)
{
	uint32_t i;

	for (i = 0; i < dal_monitor_tables_get_count(); ++i) {
		const struct monitor_patch_info *entry =
			&monitor_patch_table[i];
		if (entry->manufacturer_id == manufacturer_id &&
			entry->product_id == product_id &&
			entry->type == patch_type)
			return entry;
	}
	return NULL;
}
