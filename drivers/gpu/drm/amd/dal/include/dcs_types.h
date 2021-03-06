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

#ifndef __DAL_DCS_TYPES_H__
#define __DAL_DCS_TYPES_H__

#include "timing_service_types.h"
#include "signal_types.h"

#define NUM_OF_BYTE_EDID_COLOR_CHARACTERISTICS 10
#define MAX_NUM_OF_HDMI_VSDB_3D_EXTENDED_SUPPORT 21
#define MAX_NUM_OF_HDMI_VSDB_VICS 7
#define MAX_NUM_OF_HDMI_VSDB_3D_MULTI_SUPPORT 16
#define TILED_DISPLAY_HORIZONTAL 1920
#define TILED_DISPLAY_VERTICAL 2160

struct drr_config {
	/* minimum frame per second for dynamic
	 * refresh rate feature; 0 if drr support not found*/
	uint32_t min_fps_in_microhz;
	bool force_lock_on_event;
	bool lock_to_master_vsync;

	struct {
		uint8_t FORCED_BY_REGKEY_OR_ESCAPE:1;
		uint8_t FORCED_BY_VBIOS:1;
		uint8_t SUPPORTED_BY_EDID:1;
	} support_method;
};

struct timing_limits {
	uint32_t min_pixel_clock_in_khz;
	uint32_t max_pixel_clock_in_khz;
};

struct vendor_product_id_info {
	uint32_t manufacturer_id;
	uint32_t product_id;
	uint32_t serial_id;
	uint32_t manufacture_week;
	uint32_t manufacture_year;
};

struct display_range_limits {
	uint32_t min_v_rate_hz;
	uint32_t max_v_rate_hz;
	uint32_t min_h_rate_khz;
	uint32_t max_h_rateIn_khz;
	uint32_t max_pix_clk_khz;
	bool use_override;
};

struct monitor_user_select_limits {
	bool use_ati_override;
	uint32_t max_h_res;
	uint32_t max_v_res;
	uint32_t max_refresh_rate;
};

enum edid_screen_aspect_ratio {
	EDID_SCREEN_AR_UNKNOWN = 0,
	EDID_SCREEN_AR_PROJECTOR,
	EDID_SCREEN_AR_16X9,
	EDID_SCREEN_AR_16X10,
	EDID_SCREEN_AR_4X3,
	EDID_SCREEN_AR_5X4,
	EDID_SCREEN_AR_9X16,
	EDID_SCREEN_AR_10X16,
	EDID_SCREEN_AR_3X4,
	EDID_SCREEN_AR_4X5
};

struct edid_screen_info {
	enum edid_screen_aspect_ratio aspect_ratio;
	uint32_t width;
	uint32_t height;
};

struct display_characteristics {
	uint8_t gamma;
	uint8_t color_characteristics[NUM_OF_BYTE_EDID_COLOR_CHARACTERISTICS];
};

union cv_smart_dongle_modes {
	uint8_t all;
	struct cv_smart_dongle_switches {
		uint8_t MODE_1080I:1;
		uint8_t MODE_720P:1;
		uint8_t MODE_540P:1;
		uint8_t MODE_480P:1;
		uint8_t MODE_480I:1;
		uint8_t MODE_16_9:1;
	} switches;
};

struct cea_audio_mode {
	uint8_t format_code; /* ucData[0] [6:3]*/
	uint8_t channel_count; /* ucData[0] [2:0]*/
	uint8_t sample_rate; /* ucData[1]*/
	union {
		uint8_t sample_size; /* for LPCM*/
		/*  for Audio Formats 2-8 (Max bit rate divided by 8 kHz)*/
		uint8_t max_bit_rate;
		uint8_t audio_codec_vendor_specific; /* for Audio Formats 9-15*/
	};
};

union cea_speaker_allocation_data_block {
	struct {
		uint32_t FL_FR:1;
		uint32_t LFE:1;
		uint32_t FC:1;
		uint32_t RL_RR:1;
		uint32_t RC:1;
		uint32_t FLC_FRC:1;
		uint32_t RLC_RRC:1;
	} bits;
	uint32_t raw;
};

struct cea_colorimetry_data_block {
	struct {
		uint32_t XV_YCC601:1;
		uint32_t XV_YCC709:1;
		uint32_t S_YCC601:1;
		uint32_t ADOBE_YCC601:1;
		uint32_t ADOBE_RGB:1;

	} flag;
	struct {
		uint32_t MD0:1;
		uint32_t MD1:1;
		uint32_t MD2:1;
		uint32_t MD3:1;
	} metadata_flag;
};

union cea_video_capability_data_block {
	struct {
		uint8_t S_CE0:1;
		uint8_t S_CE1:1;
		uint8_t S_IT0:1;
		uint8_t S_IT1:1;
		uint8_t S_PT0:1;
		uint8_t S_PT1:1;
		uint8_t QS:1;
		uint8_t QY:1;
	} bits;
	uint8_t raw;
};

enum stereo_3d_multi_presence {
	STEREO_3D_MULTI_NOT_PRESENT = 0,
	STEREO_3D_MULTI_ALL_FORMATS,
	STEREO_3D_MULTI_MASKED_FORMATS,
	STEREO_3D_MULTI_RESERVED
};

enum cea_hdmi_vic {
	CEA_HDMI_VIC_RESERVED = 0,
	CEA_HDMI_VIC_4KX2K_30,
	CEA_HDMI_VIC_4KX2K_25,
	CEA_HDMI_VIC_4KX2K_24,
	CEA_HDMI_VIC_4KX2K_24_SMPTE
};

struct cea_hdmi_vsdb_extended_caps {
	uint32_t reserved;
	uint32_t image_size;
	enum stereo_3d_multi_presence stereo_3d_multi_present;
	bool stereo_3d_present;
	uint32_t hdmi_3d_len;
	uint32_t hdmi_vic_len;
};

struct cea_vendor_specific_data_block {

	uint32_t ieee_id;

	struct commonent_phy {
		uint32_t PHY_ADDR_A:4;
		uint32_t PHY_ADDR_B:4;
		uint32_t PHY_ADDR_C:4;
		uint32_t PHY_ADDR_D:4;
	} commonent_phy_addr;

	struct byte6 {
		uint32_t SUPPORTS_AI:1;
		uint32_t DC_48BIT:1;
		uint32_t DC_36BIT:1;
		uint32_t DC_30BIT:1;
		uint32_t DC_Y444:1;
		uint32_t DVI_DUAL:1;
		uint32_t RESERVED:2;
	} byte6;/* link capabilities*/
	bool byte6_valid;

	uint32_t max_tmds_clk_mhz;

	struct byte8 {
		uint32_t LATENCY_FIELDS_PRESENT:1;
		uint32_t ILATENCY_FIELDS_PRESENT:1;
		uint32_t HDMI_VIDEO_PRESENT:1;
		uint32_t RESERVED:1;
		uint32_t CNC3_GAME:1;
		uint32_t CNC2_CINEMA:1;
		uint32_t CNC1_PHOTO:1;
		uint32_t CNC0_GRAPHICS:1;
	} byte8;
	/*bit 6-7: latency flags to indicate valid latency fields*/
	/*bit 5: support of additional video format capabilities*/
	/* bit 0-3: flags indicating which content type is supported*/
	uint32_t video_latency;
	uint32_t audio_latency;
	uint32_t i_video_latency;
	uint32_t i_audio_latency;

	struct cea_hdmi_vsdb_extended_caps hdmi_vsdb_extended_caps;

	enum stereo_3d_multi_presence stereo_3d_multi_present;

	struct {
		bool FRAME_PACKING:1;
		bool SIDE_BY_SIDE_HALF:1;
		bool TOP_AND_BOTTOM:1;
	} stereo_3d_all_support;
	uint16_t stereo_3d_mask;

	enum cea_hdmi_vic hdmi_vic[MAX_NUM_OF_HDMI_VSDB_VICS];
	struct stereo_3d_extended_support {
		struct {
			bool FRAME_PACKING:1;
			bool SIDE_BY_SIDE_HALF:1;
			bool TOP_AND_BOTTOM:1;
		} format;
		uint32_t vic_index;
		uint32_t value;
		uint32_t size;
	} stereo_3d_extended_support[MAX_NUM_OF_HDMI_VSDB_3D_EXTENDED_SUPPORT];
};

struct cea861_support {

	uint32_t revision;
	union {
		struct {
			uint32_t NATIVE_COUNT:4;
			uint32_t BASE_AUDIO:1;
			uint32_t YCRCB444:1;
			uint32_t YCRCB422:1;
			uint32_t UNDER_SCAN:1;
		} features;
		uint8_t raw_features;
	};
};

struct dcs_customized_mode {
	struct {
		uint32_t READ_ONLY:1;
		uint32_t ADD_BY_DRIVER:1;
		uint32_t INTERLACED:1;
		uint32_t BASE_MODE:1;
	} flags;
	struct mode_info base_mode_info;
	struct mode_info customized_mode_info;
};

struct dcs_override_mode_timing {
	/* possible timing standards, bit vector of TimingStandard*/
	uint32_t possible_timing_standards;
	/* indicate driver default timing is used , no override*/
	bool use_driver_default_timing;
	struct mode_timing mode_timing;
};

struct dcs_override_mode_timing_list {
	uint32_t max_num_overrides;
	uint32_t num_overrides;
	struct dcs_override_mode_timing mode_timings[1];
};

/* "interface type" is different from Signal Type because
 * an "interface type" can be driven by more than one Signal Type.
 * For example, INTERFACE_TYPE_DVI can be driven by
 * Single or Dual link DVI signal. */
enum dcs_interface_type {
	INTERFACE_TYPE_VGA = 0,
	INTERFACE_TYPE_DVI,
	INTERFACE_TYPE_CV,
	INTERFACE_TYPE_TV,
	INTERFACE_TYPE_LVDS,
	INTERFACE_TYPE_DP,
	INTERFACE_TYPE_WIRELESS,
	INTERFACE_TYPE_CF,
	INTERFACE_TYPE_EDP
};

enum dcs_edid_connector_type {
	EDID_CONNECTOR_UNKNOWN = 0,
	EDID_CONNECTOR_ANALOG = 1,
	EDID_CONNECTOR_DIGITAL = 10,
	EDID_CONNECTOR_DVI = 11,
	EDID_CONNECTOR_HDMIA = 12,
	EDID_CONNECTOR_MDDI = 14,
	EDID_CONNECTOR_DISPLAYPORT = 15
};

union panel_misc_info {
	struct {
		uint32_t H_CUT_OFF:1;
		uint32_t H_SYNC_POLARITY:1;/*0=Active High, 1=Active Low*/
		uint32_t V_SYNC_POLARITY:1; /*0=Active High, 1=Active Low*/
		uint32_t V_CUT_OFF:1;
		uint32_t H_REPLICATION_BY_2:1;
		uint32_t V_REPLICATION_BY_2:1;
		uint32_t COMPOSITE_SYNC:1;
		uint32_t INTERLACE:1;
		uint32_t DOUBLE_CLOCK:1;
		uint32_t RGB888:1;
		uint32_t GREY_LEVEL:2;
		uint32_t SPATIAL:1;
		uint32_t TEMPORAL:1;
		uint32_t API_ENABLED:1;
	} bits;
	uint32_t raw;
};

union hdtv_mode_support {
	struct {
		uint32_t HDTV_SUPPORT_480I:1;
		uint32_t HDTV_SUPPORT_480P:1;
		uint32_t HDTV_SUPPORT_576I25:1;
		uint32_t HDTV_SUPPORT_576P50:1;
		uint32_t HDTV_SUPPORT_720P:1;
		uint32_t HDTV_SUPPORT_720P50:1;
		uint32_t HDTV_SUPPORT_1080I:1;
		uint32_t HDTV_SUPPORT_1080I25:1;
		uint32_t HDTV_SUPPORT_1080P:1;
		uint32_t HDTV_SUPPORT_1080P50:1;
		uint32_t HDTV_SUPPORT_1080P24:1;
		uint32_t HDTV_SUPPORT_1080P25:1;
		uint32_t HDTV_SUPPORT_1080P30:1;
		uint32_t HDTV_SUPPORT_16X9:1;
	} bits;
	uint32_t raw;
};

enum edid_retrieve_status {
	EDID_RETRIEVE_SUCCESS = 0,
	EDID_RETRIEVE_FAIL,
	EDID_RETRIEVE_SAME_EDID,
	EDID_RETRIEVE_FAIL_WITH_PREVIOUS_SUCCESS
};

#define DCS_DECODE_EDID_RETRIEVE_STATUS(status) \
	(status == EDID_RETRIEVE_SUCCESS) ? "EDID_RETRIEVE_SUCCESS" : \
	(status == EDID_RETRIEVE_FAIL) ? "EDID_RETRIEVE_FAIL" : \
	(status == EDID_RETRIEVE_SAME_EDID) ? "EDID_RETRIEVE_SAME_EDID" : \
	(status == EDID_RETRIEVE_FAIL_WITH_PREVIOUS_SUCCESS) ? \
		"EDID_RETRIEVE_FAIL_WITH_PREVIOUS_SUCCESS" : "Unknown"


#ifndef TV_SIGNALFORMAT_DEFINED
#define TV_SIGNALFORMAT_DEFINED
enum tv_signal_format {
	TV_SIGNAL_FORMAT_UNKNOWN,
	TV_SIGNAL_FORMAT_NTSC,
	TV_SIGNAL_FORMAT_NTSC_J,
	TV_SIGNAL_FORMAT_PAL,
	TV_SIGNAL_FORMAT_PAL_M,
	TV_SIGNAL_FORMAT_PAL_CN,
	TV_SIGNAL_FORMAT_SECAM
};
#endif

enum tv_signal_format_result {
	TV_SIGNAL_FORMAT_RESULT_OK,
	TV_SIGNAL_FORMAT_SET_MODE_REQ,
	TV_SIGNAL_FORMAT_REBOOT_REQ,
	TV_SIGNAL_FORMAT_ERROR
};

enum pixel_encoding_mask {
	PIXEL_ENCODING_MASK_YCBCR444 = 0x01,
	PIXEL_ENCODING_MASK_YCBCR422 = 0x02,
	PIXEL_ENCODING_MASK_RGB = 0x04,
};

struct display_pixel_encoding_support {
	uint32_t mask;
};

enum color_depth_index {
	COLOR_DEPTH_INDEX_UNKNOWN,
	COLOR_DEPTH_INDEX_666 = 0x01,
	COLOR_DEPTH_INDEX_888 = 0x02,
	COLOR_DEPTH_INDEX_101010 = 0x04,
	COLOR_DEPTH_INDEX_121212 = 0x08,
	COLOR_DEPTH_INDEX_141414 = 0x10,
	COLOR_DEPTH_INDEX_161616 = 0x20,
	COLOR_DEPTH_INDEX_LAST = 0x40,
};

struct display_color_depth_support {
	uint32_t mask;
	bool deep_color_native_res_only;
};

struct display_color_and_pixel_support {
	struct display_color_depth_support color_depth_support;
	struct display_pixel_encoding_support pixel_encoding_support;
	bool deep_color_y444_support;
};

enum dcs_packed_pixel_format {
	DCS_PACKED_PIXEL_FORMAT_NOT_PACKED = 0,
	DCS_PACKED_PIXEL_FORMAT_SPLIT_G70_B54_R70_B10 = 1,
	DCS_PACKED_PIXEL_FORMAT_R70_G76 = 2,
	DCS_PACKED_PIXEL_FORMAT_SPLIT_B70_G10_R70_G76 = 3,
	DCS_PACKED_PIXEL_FORMAT_G70_B54_R70_B10 = 4,
	DCS_PACKED_PIXEL_FORMAT_G70_B54 = 5,
	DCS_PACKED_PIXEL_FORMAT_B70_R30_G70_R74 = 6,
	DCS_PACKED_PIXEL_FORMAT_B70_G70_R70 = 7,
	DCS_PACKED_PIXEL_FORMAT_B70_R32_G70_R76 = 8,
};

enum monitor_manufacturer_id {
	MONITOR_MANUFACTURER_ID_0 = 0x0000,
	MONITOR_MANUFACTURER_ID_1 = 0x3834,
	MONITOR_MANUFACTURER_ID_2 = 0x4d24,
	MONITOR_MANUFACTURER_ID_3 = 0x293E,
	MONITOR_MANUFACTURER_ID_4 = 0x635a,
	MONITOR_MANUFACTURER_ID_5 = 0x1006,
	MONITOR_MANUFACTURER_ID_6 = 0xc32a,
	MONITOR_MANUFACTURER_ID_7 = 0x4d24,
	MONITOR_MANUFACTURER_ID_8 = 0x110e,
	MONITOR_MANUFACTURER_ID_9 = 0xaf0d,
	MONITOR_MANUFACTURER_ID_10 = 0x6D1E,
	MONITOR_MANUFACTURER_ID_11 = 0xA338,
	MONITOR_MANUFACTURER_ID_12 = 0xC315,
	MONITOR_MANUFACTURER_ID_13 = 0xD94D,
	MONITOR_MANUFACTURER_ID_14 = 0x104D,
	MONITOR_MANUFACTURER_ID_15 = 0x855C,
	MONITOR_MANUFACTURER_ID_16 = 0x4251,
	MONITOR_MANUFACTURER_ID_17 = 0xA934,
	MONITOR_MANUFACTURER_ID_18 = 0x0C41,
	/* TODO: Update when EDID is available */
	MONITOR_MANUFACTURER_ID_19 = 0xDEAD,
	MONITOR_MANUFACTURER_ID_20 = 0x6904,
	MONITOR_MANUFACTURER_ID_21 = 0xAC10,
	MONITOR_MANUFACTURER_ID_22 = 0x2D4C,
	MONITOR_MANUFACTURER_ID_23 = 0x144E,
	MONITOR_MANUFACTURER_ID_24 = 0x6c50,
	MONITOR_MANUFACTURER_ID_26 = 0x0c41,
	MONITOR_MANUFACTURER_ID_27 = 0x143E,
	MONITOR_MANUFACTURER_ID_25 = 0xffff,
	MONITOR_MANUFACTURER_ID_28 = 0x3421,
	MONITOR_MANUFACTURER_ID_29 = 0x2D19,
	MONITOR_MANUFACTURER_ID_30 = 0x8B52,
	MONITOR_MANUFACTURER_ID_31 = 0x7204,
	MONITOR_MANUFACTURER_ID_32 = 0xF022,
	MONITOR_MANUFACTURER_ID_33 = 0x0E11,
	MONITOR_MANUFACTURER_ID_34 = 0xD241,
	MONITOR_MANUFACTURER_ID_35 = 0xAE30,
	MONITOR_MANUFACTURER_ID_36 = 0xF91E,
	MONITOR_MANUFACTURER_ID_37 = 0xAB4C,
};

enum monitor_product_id {
	MONITOR_PRODUCT_ID_0 = 0x0000,
	MONITOR_PRODUCT_ID_1 = 0x0BCC,
	MONITOR_PRODUCT_ID_2 = 0x251F,
	MONITOR_PRODUCT_ID_3 = 0x5241,
	MONITOR_PRODUCT_ID_4 = 0x6919,
	MONITOR_PRODUCT_ID_5 = 0xee18,
	MONITOR_PRODUCT_ID_6 = 0xf008,
	MONITOR_PRODUCT_ID_7 = 0x2f0c,
	MONITOR_PRODUCT_ID_7_2 = 0x3411,
	MONITOR_PRODUCT_ID_9 = 0x4208,
	MONITOR_PRODUCT_ID_10 = 0xE51D,
	MONITOR_PRODUCT_ID_11 = 0x7E22,
	MONITOR_PRODUCT_ID_12 = 0x0E23,
	MONITOR_PRODUCT_ID_13 = 0x9d08,
	MONITOR_PRODUCT_ID_14 = 0x9236,
	MONITOR_PRODUCT_ID_15 = 0x9227,
	MONITOR_PRODUCT_ID_16 = 0x0220,
	MONITOR_PRODUCT_ID_17 = 0x4920,
	MONITOR_PRODUCT_ID_18 = 0x251f,
	MONITOR_PRODUCT_ID_19 = 0x1395,
	MONITOR_PRODUCT_ID_20 = 0xc04e,
	MONITOR_PRODUCT_ID_21 = 0x5787,
	MONITOR_PRODUCT_ID_22 = 0x5A71,
	MONITOR_PRODUCT_ID_23 = 0x6622,
	MONITOR_PRODUCT_ID_24 = 0x20C1,
	MONITOR_PRODUCT_ID_25 = 0x2110,
	MONITOR_PRODUCT_ID_26 = 0x2006,
	MONITOR_PRODUCT_ID_27 = 0x1827,
	MONITOR_PRODUCT_ID_28 = 0x0EA0,
	MONITOR_PRODUCT_ID_29 = 0x03D0,
	MONITOR_PRODUCT_ID_30 = 0x01D2,
	MONITOR_PRODUCT_ID_31 = 0x2801,
	MONITOR_PRODUCT_ID_32 = 0x0FB3,
	MONITOR_PRODUCT_ID_33 = 0x0FB1,
	MONITOR_PRODUCT_ID_34 = 0xA045,
	MONITOR_PRODUCT_ID_35 = 0x0001,
	MONITOR_PRODUCT_ID_36 = 0xA296,
	MONITOR_PRODUCT_ID_38 = 0x21DC,
	MONITOR_PRODUCT_ID_37 = 0x21EA,
	MONITOR_PRODUCT_ID_39 = 0x4093,
	MONITOR_PRODUCT_ID_40 = 0x4094,
	MONITOR_PRODUCT_ID_41 = 0x4094,
	MONITOR_PRODUCT_ID_42 = 0x32A2,
	MONITOR_PRODUCT_ID_43 = 0xE009,
	MONITOR_PRODUCT_ID_44 = 0xA010,
	MONITOR_PRODUCT_ID_45 = 0x405C,
	MONITOR_PRODUCT_ID_46 = 0xF017,
	MONITOR_PRODUCT_ID_47 = 0xD026,
	MONITOR_PRODUCT_ID_48 = 0x4036,
	MONITOR_PRODUCT_ID_49 = 0x4065,
	MONITOR_PRODUCT_ID_50 = 0xA02A,
	MONITOR_PRODUCT_ID_51 = 0xA02C,
	MONITOR_PRODUCT_ID_46_HDMI = 0xF016,
	MONITOR_PRODUCT_ID_53 = 0xF048,
	MONITOR_PRODUCT_ID_54 = 0xA0A2,
	MONITOR_PRODUCT_ID_55 = 0x4083,
	MONITOR_PRODUCT_ID_56 = 0x0E74,
	MONITOR_PRODUCT_ID_57 = 0x2771,
	MONITOR_PRODUCT_ID_58 = 0x0814,
	MONITOR_PRODUCT_ID_59 = 0xffff,
	MONITOR_PRODUCT_ID_60 = 0x3339,
	MONITOR_PRODUCT_ID_61 = 0x01F5,
	MONITOR_PRODUCT_ID_62 = 0x02A5,
	MONITOR_PRODUCT_ID_63 = 0x06AC,
	MONITOR_PRODUCT_ID_64 = 0x04D5,
	MONITOR_PRODUCT_ID_65 = 0x079D,
	MONITOR_PRODUCT_ID_66 = 0x079F,
	MONITOR_PRODUCT_ID_67 = 0x0797,
	MONITOR_PRODUCT_ID_68 = 0x0B80,
	MONITOR_PRODUCT_ID_69 = 0x7D06,
	MONITOR_PRODUCT_ID_70 = 0x0131,
	MONITOR_PRODUCT_ID_71 = 0x8545,
	MONITOR_PRODUCT_ID_72 = 0x0002,
	MONITOR_PRODUCT_ID_73 = 0x0125,
	MONITOR_PRODUCT_ID_74 = 0x00D0,
	MONITOR_PRODUCT_ID_75 = 0x26F7,
	MONITOR_PRODUCT_ID_76 = 0x26F9,
	MONITOR_PRODUCT_ID_77 = 0x2807,
	MONITOR_PRODUCT_ID_78 = 0x26F3,
	MONITOR_PRODUCT_ID_79 = 0x2676,
	MONITOR_PRODUCT_ID_80 = 0x0A72,
	MONITOR_PRODUCT_ID_81 = 0x2693,
	MONITOR_PRODUCT_ID_82 = 0x2615,
	MONITOR_PRODUCT_ID_83 = 0x2613,
	MONITOR_PRODUCT_ID_84 = 0x262D,
	MONITOR_PRODUCT_ID_85 = 0x264B,
	MONITOR_PRODUCT_ID_86 = 0x2869,
	MONITOR_PRODUCT_ID_87 = 0x286C,
	MONITOR_PRODUCT_ID_88 = 0x288F,
	MONITOR_PRODUCT_ID_89 = 0x2954,
	MONITOR_PRODUCT_ID_90 = 0x6522,
	MONITOR_PRODUCT_ID_91 = 0x0FAE,
	MONITOR_PRODUCT_ID_92 = 0x0A0C,
	MONITOR_PRODUCT_ID_93 = 0x00BF,
	MONITOR_PRODUCT_ID_94 = 0x0,
};

enum monitor_patch_type {
	MONITOR_PATCH_TYPE_NONE,
	MONITOR_PATCH_TYPE_ERROR_CHECKSUM,
	MONITOR_PATCH_TYPE_HDTV_WITH_PURE_DFP_EDID,
	MONITOR_PATCH_TYPE_DO_NOT_USE_DETAILED_TIMING,
	MONITOR_PATCH_TYPE_DO_NOT_USE_RANGE_LIMITATION,
	MONITOR_PATCH_TYPE_EDID_EXTENTION_ERROR_CHECK_SUM,
	MONITOR_PATCH_TYPE_TURN_OFF_DISPLAY_BEFORE_MODE_CHANGE,
	MONITOR_PATCH_TYPE_RESTRICT_VESA_MODE_TIMING,
	MONITOR_PATCH_TYPE_DO_NOT_USE_EDID_MAX_PIX_CLK,
	MONITOR_PATCH_TYPE_VENDOR_0,
	MONITOR_PATCH_TYPE_RANDOM_CRT,
	MONITOR_PATCH_TYPE_VENDOR_1,
	MONITOR_PATCH_TYPE_LIMIT_PANEL_SUPPORT_RGB_ONLY,
	MONITOR_PATCH_TYPE_PACKED_PIXEL_FORMAT,
	MONITOR_PATCH_TYPE_LARGE_PANEL,
	MONITOR_PATCH_TYPE_STEREO_SUPPORT,
	/* 0 (default) - mean patch will not be applied, however it can be
	 * explicitly set to 1
	 */
	MONITOR_PATCH_TYPE_DUAL_EDID_PANEL,
	MONITOR_PATCH_TYPE_IGNORE_19X12_STD_TIMING,
	MONITOR_PATCH_TYPE_MULTIPLE_PACKED_TYPE,
	MONITOR_PATCH_TYPE_RESET_TX_ON_DISPLAY_POWER_ON,
	MONITOR_PATCH_TYPE_VENDOR_2,
	MONITOR_PATCH_TYPE_RESTRICT_PROT_DUAL_LINK_DVI,
	MONITOR_PATCH_TYPE_FORCE_LINK_RATE,
	MONITOR_PATCH_TYPE_DELAY_AFTER_DP_RECEIVER_POWER_UP,
	MONITOR_PATCH_TYPE_KEEP_DP_RECEIVER_POWERED,
	MONITOR_PATCH_TYPE_DELAY_BEFORE_READ_EDID,
	MONITOR_PATCH_TYPE_DELAY_AFTER_PIXEL_FORMAT_CHANGE,
	MONITOR_PATCH_TYPE_INCREASE_DEFER_WRITE_RETRY_I2C_OVER_AUX,
	MONITOR_PATCH_TYPE_NO_DEFAULT_TIMINGS,
	MONITOR_PATCH_TYPE_ADD_CEA861_DETAILED_TIMING_VIC16,
	MONITOR_PATCH_TYPE_ADD_CEA861_DETAILED_TIMING_VIC31,
	MONITOR_PATCH_TYPE_DELAY_BEFORE_UNMUTE,
	MONITOR_PATCH_TYPE_RETRY_LINK_TRAINING_ON_FAILURE,
	MONITOR_PATCH_TYPE_ALLOW_AUX_WHEN_HPD_LOW,
	MONITOR_PATCH_TYPE_TILED_DISPLAY,
	MONITOR_PATCH_TYPE_DISABLE_PSR_ENTRY_ABORT,
	MONITOR_PATCH_TYPE_EDID_EXTENTION_ERROR_CHECKSUM,
	MONITOR_PATCH_TYPE_ALLOW_ONLY_CE_MODE,
	MONITOR_PATCH_TYPE_VID_STREAM_DIFFER_TO_SYNC,
	MONITOR_PATCH_TYPE_EXTRA_DELAY_ON_DISCONNECT,
	MONITOR_PATCH_TYPE_DELAY_AFTER_DISABLE_BACKLIGHT_DFS_BYPASS,
	MONITOR_PATCH_TYPE_SINGLE_MODE_PACKED_PIXEL
};

/* Single entry in the monitor table */
struct monitor_patch_info {
	enum monitor_manufacturer_id manufacturer_id;
	enum monitor_product_id product_id;
	enum monitor_patch_type type;
	uint32_t param;
};

union dcs_monitor_patch_flags {
	struct {
		bool ERROR_CHECKSUM:1;
		bool HDTV_WITH_PURE_DFP_EDID:1;
		bool DO_NOT_USE_DETAILED_TIMING:1;
		bool DO_NOT_USE_RANGE_LIMITATION:1;
		bool EDID_EXTENTION_ERROR_CHECKSUM:1;
		bool TURN_OFF_DISPLAY_BEFORE_MODE_CHANGE:1;
		bool RESTRICT_VESA_MODE_TIMING:1;
		bool DO_NOT_USE_EDID_MAX_PIX_CLK:1;
		bool VENDOR_0:1;
		bool RANDOM_CRT:1;/* 10 bits used including this one-*/
		bool VENDOR_1:1;
		bool LIMIT_PANEL_SUPPORT_RGB_ONLY:1;
		bool PACKED_PIXEL_FORMAT:1;
		bool LARGE_PANEL:1;
		bool STEREO_SUPPORT:1;
		bool DUAL_EDID_PANEL:1;
		bool IGNORE_19X12_STD_TIMING:1;
		bool MULTIPLE_PACKED_TYPE:1;
		bool RESET_TX_ON_DISPLAY_POWER_ON:1;
		bool ALLOW_ONLY_CE_MODE:1;/* 20 bits used including this one*/
		bool RESTRICT_PROT_DUAL_LINK_DVI:1;
		bool FORCE_LINK_RATE:1;
		bool DELAY_AFTER_DP_RECEIVER_POWER_UP:1;
		bool KEEP_DP_RECEIVER_POWERED:1;
		bool DELAY_BEFORE_READ_EDID:1;
		bool DELAY_AFTER_PIXEL_FORMAT_CHANGE:1;
		bool INCREASE_DEFER_WRITE_RETRY_I2C_OVER_AUX:1;
		bool NO_DEFAULT_TIMINGS:1;
		bool ADD_CEA861_DETAILED_TIMING_VIC16:1;
		bool ADD_CEA861_DETAILED_TIMING_VIC31:1; /* 30 bits*/
		bool DELAY_BEFORE_UNMUTE:1;
		bool RETRY_LINK_TRAINING_ON_FAILURE:1;
		bool ALLOW_AUX_WHEN_HPD_LOW:1;
		bool TILED_DISPLAY:1;
		bool DISABLE_PSR_ENTRY_ABORT:1;
		bool INTERMITTENT_EDID_ERROR:1;/* 36 bits total*/
		bool VID_STREAM_DIFFER_TO_SYNC:1;/* 37 bits total*/
		bool EXTRA_DELAY_ON_DISCONNECT:1;/* 38 bits total*/
		bool DELAY_AFTER_DISABLE_BACKLIGHT_DFS_BYPASS:1;/* 39 bits total*/
	} flags;
	uint64_t raw;
};

struct dcs_edid_supported_max_bw {
	uint32_t pix_clock_khz;
	uint32_t bits_per_pixel;
};

struct dcs_stereo_3d_features {
	struct {
/* 3D Format supported by monitor (implies supported by driver)*/
		uint32_t SUPPORTED:1;
/* 3D Format supported on all timings
(no need to check every timing for 3D support)*/
		uint32_t ALL_TIMINGS:1;
/* 3D Format supported in clone mode*/
		uint32_t CLONE_MODE:1;
/* Scaling allowed when driving 3D Format*/
		uint32_t SCALING:1;
/* Left and right images packed by SW within single frame*/
		uint32_t SINGLE_FRAME_SW_PACKED:1;
	} flags;
};

struct dcs_container_id {
	/*128bit GUID in binary form*/
	uint8_t guid[16];
	/* 8 byte port ID -> ELD.PortID*/
	uint32_t port_id[2];
	/* 2 byte manufacturer name -> ELD.ManufacturerName*/
	uint16_t manufacturer_name;
	/* 2 byte product code -> ELD.ProductCode*/
	uint16_t product_code;
};

struct dcs_display_tile {
/*unique Id of Tiled Display. 0 - means display is not part in Tiled Display*/
	uint64_t id;
	uint32_t rows;/* size of Tiled Display in tiles*/
	uint32_t cols;/* size of Tiled Display in tiles*/
	uint32_t width;/* size of current Tile in pixels*/
	uint32_t height;/* size of current Tile in pixels*/
	uint32_t row;/* location of current Tile*/
	uint32_t col;/* location of current Tile*/
	struct {
		/*in pixels*/
		uint32_t left;
		uint32_t right;
		uint32_t top;
		uint32_t bottom;
	} bezel;/* bezel information of current tile*/

	struct {
		uint32_t SEPARATE_ENCLOSURE:1;
		uint32_t BEZEL_INFO_PRESENT:1;
		uint32_t CAN_SCALE:1;
	} flags;

	struct {
		uint32_t manufacturer_id;
		uint32_t product_id;
		uint32_t serial_id;
	} topology_id;
};

#endif /* __DAL_DCS_TYPES_H__ */

