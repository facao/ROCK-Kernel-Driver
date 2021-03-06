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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: AMD
 *
 */

#ifndef __DAL_GRPH_OBJECT_CTRL_DEFS_H__
#define __DAL_GRPH_OBJECT_CTRL_DEFS_H__

#include "grph_object_defs.h"

/*
 * #####################################################
 * #####################################################
 *
 * These defines shared between asic_control/bios_parser and other
 * DAL components
 *
 * #####################################################
 * #####################################################
 */

enum tv_standard {
	TV_STANDARD_UNKNOWN = 0, /* direct HW (mmBIOS_SCRATCH_2) translation! */
	TV_STANDARD_NTSC,
	TV_STANDARD_NTSCJ,
	TV_STANDARD_PAL,
	TV_STANDARD_PALM,
	TV_STANDARD_PALCN,
	TV_STANDARD_PALN,
	TV_STANDARD_PAL60,
	TV_STANDARD_SECAM
};

enum cv_standard {
	CV_STANDARD_UNKNOWN = 0x0000,
	CV_STANDARD_HD_MASK = 0x0800,		/* Flag mask HDTV output */
	CV_STANDARD_SD_NTSC_MASK = 0x1000,	/* Flag mask NTSC output */
	CV_STANDARD_SD_NTSC_M,		/* NTSC (North America) output 1001 */
	CV_STANDARD_SD_NTSC_J,		/* NTSC (Japan) output 1002 */
	CV_STANDARD_SD_480I,		/* SDTV 480i output 1003 */
	CV_STANDARD_SD_480P,		/* SDTV 480p output 1004 */
	CV_STANDARD_HD_720_60P = 0x1800,/* HDTV 720/60p output 1800 */
	CV_STANDARD_HD_1080_60I,	/* HDTV 1080/60i output 1801 */
	CV_STANDARD_SD_PAL_MASK = 0x2000,/* Flag mask PAL output */
	CV_STANDARD_SD_PAL_B,			/* PAL B output 2001 */
	CV_STANDARD_SD_PAL_D,			/* PAL D output 2002 */
	CV_STANDARD_SD_PAL_G,			/* PAL G output 2003 */
	CV_STANDARD_SD_PAL_H,			/* PAL H output 2004 */
	CV_STANDARD_SD_PAL_I,			/* PAL I output 2005 */
	CV_STANDARD_SD_PAL_M,			/* PAL M output 2006 */
	CV_STANDARD_SD_PAL_N,			/* PAL N output 2007 */
	CV_STANDARD_SD_PAL_N_COMB,	/* PAL Combination N output 2008 */
	CV_STANDARD_SD_PAL_60,		/* PAL 60 output (test mode) 2009 */
	CV_STANDARD_SD_576I,		/* SDTV 576i output 2010 */
	CV_STANDARD_SD_576P,		/* SDTV 576p output 2011 */
	CV_STANDARD_HD_720_50P = 0x2800,/* HDTV 720/50p output 2800 */
	CV_STANDARD_HD_1080_50I,	/* HDTV 1080/50i output 2801 */
	CV_STANDARD_SD_SECAM_MASK = 0x4000, /* Flag mask SECAM output */
	CV_STANDARD_SD_SECAM_B,		/* SECAM B output 4001 */
	CV_STANDARD_SD_SECAM_D,		/* SECAM D output 4002 */
	CV_STANDARD_SD_SECAM_G,		/* SECAM G output 4003 */
	CV_STANDARD_SD_SECAM_H,		/* SECAM H output 4004 */
	CV_STANDARD_SD_SECAM_K,		/* SECAM K output 4005 */
	CV_STANDARD_SD_SECAM_K1,	/* SECAM K1 output 4006 */
	CV_STANDARD_SD_SECAM_L,		/* SECAM L output 4007 */
	CV_STANDARD_SD_SECAM_L1		/* SECAM L1 output 4009 */
};

enum disp_pll_config {
	DISP_PLL_CONFIG_UNKNOWN = 0,
	DISP_PLL_CONFIG_DVO_DDR_MODE_LOW_12BIT,
	DISP_PLL_CONFIG_DVO_DDR_MODE_UPPER_12BIT,
	DISP_PLL_CONFIG_DVO_DDR_MODE_24BIT
};

enum transmitter_color_depth {
	TRANSMITTER_COLOR_DEPTH_24 = 0,	/* 8 bits */
	TRANSMITTER_COLOR_DEPTH_30,	/* 10 bits */
	TRANSMITTER_COLOR_DEPTH_36,	/* 12 bits */
	TRANSMITTER_COLOR_DEPTH_48	/* 16 bits */
};

enum display_output_bit_depth {
	PANEL_UNDEFINE = 0,
	PANEL_6BIT_COLOR = 1,
	PANEL_8BIT_COLOR = 2,
	PANEL_10BIT_COLOR = 3,
	PANEL_12BIT_COLOR = 4,
	PANEL_16BIT_COLOR = 5,
};

/* Color depth of DP transmitter */
enum dp_transmitter_color_depth {
	DP_TRANSMITTER_COLOR_DEPTH_UNKNOWN = 0,
	DP_TRANSMITTER_COLOR_DEPTH_666,
	DP_TRANSMITTER_COLOR_DEPTH_888,
	DP_TRANSMITTER_COLOR_DEPTH_101010,
	DP_TRANSMITTER_COLOR_DEPTH_121212,
	DP_TRANSMITTER_COLOR_DEPTH_141414,
	DP_TRANSMITTER_COLOR_DEPTH_161616
};

enum lcd_scale {
	/* No request to turn on LCD SCALER (Centering or Replication) */
	LCD_SCALE_NONE = 0,
	/* Request LCD SCALER in full panel mode */
	LCD_SCALE_FULLPANEL,
	/* Request LCD SCALER in aspect-ratio mode */
	LCD_SCALE_ASPECTRATIO,

	LCD_SCALE_UNKNOWN = (-1L),
};

/* Device type as abstracted by ATOM BIOS */
enum dal_device_type {
	DEVICE_TYPE_UNKNOWN = 0,
	DEVICE_TYPE_LCD,
	DEVICE_TYPE_CRT,
	DEVICE_TYPE_DFP,
	DEVICE_TYPE_CV,
	DEVICE_TYPE_TV,
	DEVICE_TYPE_CF,
	DEVICE_TYPE_WIRELESS
};

/* Device ID as abstracted by ATOM BIOS */
struct device_id {
	enum dal_device_type device_type:16;
	uint32_t enum_id:16;	/* 1 based enum */
};

struct graphics_object_i2c_info {
	struct gpio_info {
		uint32_t clk_mask_register_index;
		uint32_t clk_en_register_index;
		uint32_t clk_y_register_index;
		uint32_t clk_a_register_index;
		uint32_t data_mask_register_index;
		uint32_t data_en_register_index;
		uint32_t data_y_register_index;
		uint32_t data_a_register_index;

		uint32_t clk_mask_shift;
		uint32_t clk_en_shift;
		uint32_t clk_y_shift;
		uint32_t clk_a_shift;
		uint32_t data_mask_shift;
		uint32_t data_en_shift;
		uint32_t data_y_shift;
		uint32_t data_a_shift;
	} gpio_info;

	bool i2c_hw_assist;
	uint32_t i2c_line;
	uint32_t i2c_engine_id;
	uint32_t i2c_slave_address;
};


struct graphics_object_hpd_info {
	uint8_t hpd_int_gpio_uid;
	uint8_t hpd_active;
};

struct connector_device_tag_info {
	uint32_t acpi_device;
	struct device_id dev_id;
};

struct device_timing {
	struct misc_info {
		uint32_t HORIZONTAL_CUT_OFF:1;
		/* 0=Active High, 1=Active Low */
		uint32_t H_SYNC_POLARITY:1;
		/* 0=Active High, 1=Active Low */
		uint32_t V_SYNC_POLARITY:1;
		uint32_t VERTICAL_CUT_OFF:1;
		uint32_t H_REPLICATION_BY2:1;
		uint32_t V_REPLICATION_BY2:1;
		uint32_t COMPOSITE_SYNC:1;
		uint32_t INTERLACE:1;
		uint32_t DOUBLE_CLOCK:1;
		uint32_t RGB888:1;
		uint32_t GREY_LEVEL:2;
		uint32_t SPATIAL:1;
		uint32_t TEMPORAL:1;
		uint32_t API_ENABLED:1;
	} misc_info;

	uint32_t pixel_clk; /* in KHz */
	uint32_t horizontal_addressable;
	uint32_t horizontal_blanking_time;
	uint32_t vertical_addressable;
	uint32_t vertical_blanking_time;
	uint32_t horizontal_sync_offset;
	uint32_t horizontal_sync_width;
	uint32_t vertical_sync_offset;
	uint32_t vertical_sync_width;
	uint32_t horizontal_border;
	uint32_t vertical_border;
};

struct supported_refresh_rate {
	uint32_t REFRESH_RATE_30HZ:1;
	uint32_t REFRESH_RATE_40HZ:1;
	uint32_t REFRESH_RATE_48HZ:1;
	uint32_t REFRESH_RATE_50HZ:1;
	uint32_t REFRESH_RATE_60HZ:1;
};

struct embedded_panel_info {
	struct device_timing lcd_timing;
	uint32_t ss_id;
	struct supported_refresh_rate supported_rr;
	uint32_t drr_enabled;
	uint32_t min_drr_refresh_rate;
};

struct embedded_panel_patch_mode {
	uint32_t width;
	uint32_t height;
};

struct firmware_info {
	struct pll_info {
		uint32_t crystal_frequency; /* in KHz */
		uint32_t min_input_pxl_clk_pll_frequency; /* in KHz */
		uint32_t max_input_pxl_clk_pll_frequency; /* in KHz */
		uint32_t min_output_pxl_clk_pll_frequency; /* in KHz */
		uint32_t max_output_pxl_clk_pll_frequency; /* in KHz */
	} pll_info;

	struct firmware_feature {
		uint32_t memory_clk_ss_percentage;
		uint32_t engine_clk_ss_percentage;
	} feature;

	uint32_t default_display_engine_pll_frequency; /* in KHz */
	uint32_t external_clock_source_frequency_for_dp; /* in KHz */
	uint32_t smu_gpu_pll_output_freq; /* in KHz */
	uint8_t min_allowed_bl_level;
	uint8_t remote_display_config;
};

/* direct HW (mmBIOS_SCRATCH_2) translation! */
union tv_standard_support {
	uint8_t u_all;
	struct {
		bool TV_SUPPORT_NTSC:1;
		bool TV_SUPPORT_NTSCJ:1;

		bool TV_SUPPORT_PAL:1;
		bool TV_SUPPORT_PALM:1;
		bool TV_SUPPORT_PALCN:1;
		bool TV_SUPPORT_PALN:1;
		bool TV_SUPPORT_PAL60:1;

		bool TV_SUPPORT_SECAM:1;
	} bits;
};

struct analog_tv_info {
	union tv_standard_support tv_suppported;
	union tv_standard_support tv_boot_up_default;
};

struct spread_spectrum_info {
	struct spread_spectrum_type {
		bool CENTER_MODE:1;
		bool EXTERNAL:1;
		bool STEP_AND_DELAY_INFO:1;
	} type;

	/* in unit of 0.01% (spreadPercentageDivider = 100),
	otherwise in 0.001% units (spreadPercentageDivider = 1000); */
	uint32_t spread_spectrum_percentage;
	uint32_t spread_percentage_divider; /* 100 or 1000 */
	uint32_t spread_spectrum_range; /* modulation freq (HZ)*/

	union {
		struct step_and_delay_info {
			uint32_t step;
			uint32_t delay;
			uint32_t recommended_ref_div;
		} step_and_delay_info;
		/* For mem/engine/uvd, Clock Out frequence (VCO ),
		in unit of kHz. For TMDS/HDMI/LVDS, it is pixel clock,
		for DP, it is link clock ( 270000 or 162000 ) */
		uint32_t target_clock_range; /* in KHz */
	};

};

struct graphics_object_encoder_cap_info {
	uint32_t dp_hbr2_cap:1;
	uint32_t dp_hbr2_validated:1;
	uint32_t reserved:15;
};

struct din_connector_info {
	uint32_t gpio_id;
	bool gpio_tv_active_state;
};

/* Invalid channel mapping */
enum { INVALID_DDI_CHANNEL_MAPPING = 0x0 };

/**
 * DDI PHY channel mapping reflecting XBAR setting
 */
union ddi_channel_mapping {
	struct mapping {
		uint8_t lane0:2;	/* Mapping for lane 0 */
		uint8_t lane1:2;	/* Mapping for lane 1 */
		uint8_t lane2:2;	/* Mapping for lane 2 */
		uint8_t lane3:2;	/* Mapping for lane 3 */
	} mapping;
	uint8_t raw;
};

/**
* Transmitter output configuration description
*/
struct transmitter_configuration_info {
	/* DDI PHY ID for the transmitter */
	enum transmitter transmitter_phy_id;
	/* DDI PHY channel mapping reflecting crossbar setting */
	union ddi_channel_mapping output_channel_mapping;
};

struct transmitter_configuration {
	/* Configuration for the primary transmitter */
	struct transmitter_configuration_info primary_transmitter_config;
	/* Secondary transmitter configuration for Dual-link DVI */
	struct transmitter_configuration_info secondary_transmitter_config;
};


/* These size should be sufficient to store info coming from BIOS */
#define NUMBER_OF_UCHAR_FOR_GUID 16
#define MAX_NUMBER_OF_EXT_DISPLAY_PATH 7
#define NUMBER_OF_CSR_M3_ARB 10
#define NUMBER_OF_DISP_CLK_VOLTAGE 4
#define NUMBER_OF_AVAILABLE_SCLK 5

/* V6 */
struct integrated_info {
	struct clock_voltage_caps {
		/* The Voltage Index indicated by FUSE, same voltage index
		shared with SCLK DPM fuse table */
		uint32_t voltage_index;
		/* Maximum clock supported with specified voltage index */
		uint32_t max_supported_clk; /* in KHz */
	} disp_clk_voltage[NUMBER_OF_DISP_CLK_VOLTAGE];

	struct display_connection_info {
		struct external_display_path {
			/* A bit vector to show what devices are supported */
			uint32_t device_tag;
			/* 16bit device ACPI id. */
			uint32_t device_acpi_enum;
			/* A physical connector for displays to plug in,
			using object connector definitions */
			struct graphics_object_id device_connector_id;
			/* An index into external AUX/DDC channel LUT */
			uint8_t ext_aux_ddc_lut_index;
			/* An index into external HPD pin LUT */
			uint8_t ext_hpd_pin_lut_index;
			/* external encoder object id */
			struct graphics_object_id ext_encoder_obj_id;
			/* XBAR mapping of the PHY channels */
			union ddi_channel_mapping channel_mapping;
		} path[MAX_NUMBER_OF_EXT_DISPLAY_PATH];

		uint8_t gu_id[NUMBER_OF_UCHAR_FOR_GUID];
		uint8_t checksum;
	} ext_disp_conn_info; /* exiting long long time */

	struct available_s_clk_list {
		/* Maximum clock supported with specified voltage index */
		uint32_t supported_s_clk; /* in KHz */
		/* The Voltage Index indicated by FUSE for specified SCLK */
		uint32_t voltage_index;
		/* The Voltage ID indicated by FUSE for specified SCLK */
		uint32_t voltage_id;
	} avail_s_clk[NUMBER_OF_AVAILABLE_SCLK];

	uint8_t memory_type;
	uint8_t ma_channel_number;
	uint32_t boot_up_engine_clock; /* in KHz */
	uint32_t dentist_vco_freq; /* in KHz */
	uint32_t boot_up_uma_clock; /* in KHz */
	uint32_t boot_up_req_display_vector;
	uint32_t other_display_misc;
	uint32_t gpu_cap_info;
	uint32_t sb_mmio_base_addr;
	uint32_t system_config;
	uint32_t cpu_cap_info;
	uint32_t max_nb_voltage;
	uint32_t min_nb_voltage;
	uint32_t boot_up_nb_voltage;
	uint32_t ext_disp_conn_info_offset;
	uint32_t csr_m3_arb_cntl_default[NUMBER_OF_CSR_M3_ARB];
	uint32_t csr_m3_arb_cntl_uvd[NUMBER_OF_CSR_M3_ARB];
	uint32_t csr_m3_arb_cntl_fs3d[NUMBER_OF_CSR_M3_ARB];
	uint32_t gmc_restore_reset_time;
	uint32_t minimum_n_clk;
	uint32_t idle_n_clk;
	uint32_t ddr_dll_power_up_time;
	uint32_t ddr_pll_power_up_time;
	/* start for V6 */
	uint32_t pcie_clk_ss_type;
	uint32_t lvds_ss_percentage;
	uint32_t lvds_sspread_rate_in_10hz;
	uint32_t hdmi_ss_percentage;
	uint32_t hdmi_sspread_rate_in_10hz;
	uint32_t dvi_ss_percentage;
	uint32_t dvi_sspread_rate_in_10_hz;
	uint32_t sclk_dpm_boost_margin;
	uint32_t sclk_dpm_throttle_margin;
	uint32_t sclk_dpm_tdp_limit_pg;
	uint32_t sclk_dpm_tdp_limit_boost;
	uint32_t boost_engine_clock;
	uint32_t boost_vid_2bit;
	uint32_t enable_boost;
	uint32_t gnb_tdp_limit;
	/* Start from V7 */
	uint32_t max_lvds_pclk_freq_in_single_link;
	uint32_t lvds_misc;
	uint32_t lvds_pwr_on_seq_dig_on_to_de_in_4ms;
	uint32_t lvds_pwr_on_seq_de_to_vary_bl_in_4ms;
	uint32_t lvds_pwr_off_seq_vary_bl_to_de_in4ms;
	uint32_t lvds_pwr_off_seq_de_to_dig_on_in4ms;
	uint32_t lvds_off_to_on_delay_in_4ms;
	uint32_t lvds_pwr_on_seq_vary_bl_to_blon_in_4ms;
	uint32_t lvds_pwr_off_seq_blon_to_vary_bl_in_4ms;
	uint32_t lvds_reserved1;
	uint32_t lvds_bit_depth_control_val;
};

/**
* Power source ids.
*/
enum power_source {
	POWER_SOURCE_AC = 0,
	POWER_SOURCE_DC,
	POWER_SOURCE_LIMITED_POWER,
	POWER_SOURCE_LIMITED_POWER_2,
	POWER_SOURCE_MAX
};

struct bios_event_info {
	uint32_t thermal_state;
	uint32_t backlight_level;
	enum power_source powerSource;
	bool has_thermal_state_changed;
	bool has_power_source_changed;
	bool has_forced_mode_changed;
	bool forced_mode;
	bool backlight_changed;
};

union stereo_3d_support {
	struct {
		/* HW can alter left and right image sequentially */
		uint32_t FRAME_ALTERNATE:1;
		/* Frame Alternate + HW can integrate stereosync
		signal into TMDS stream */
		uint32_t DVI_FRAME_ALT:1;
		/* Frame Alternate + HW can integrate stereosync
		signal into DP stream */
		uint32_t DISPLAY_PORT_FRAME_ALT:1;
		/* Frame Alternate + HW can drive stereosync signal
		on separate line */
		uint32_t SIDEBAND_FRAME_ALT:1;
		/* SW allowed to pack left and right image into single frame.
		Used for HDMI only, DP has it's own flags. */
		uint32_t SW_FRAME_PACK:1;
		/* HW can pack left and right image into single HDMI frame */
		uint32_t PROGRESSIVE_FRAME_PACK:1;
		/* HW can pack left and right interlaced images into
		single HDMI frame */
		uint32_t INTERLACE_FRAME_PACK:1;
		/* HW can pack left and right images into single DP frame */
		uint32_t DISPLAY_PORT_FRAME_PACK:1;
		/* SW can pack left and right images into single DP frame */
		uint32_t DISPLAY_PORT_SW_FRAME_PACK:1;
		/* HW can mix left and right images into single image */
		uint32_t INTERLEAVE:1;
		/* HW can mix left and right interlaced images
		into single image */
		uint32_t INTERLACE_INTERLEAVE:1;
		/* Allow display-based formats (whatever supported)
		in WS stereo mode */
		uint32_t DISPLAY_3DIN_WS_MODE:1;
		/* Side-by-side, packed by application/driver into 2D frame */
		uint32_t SIDE_BY_SIDE_SW_PACKED:1;
		/* Top-and-bottom, packed by application/driver into 2D frame */
		uint32_t TOP_AND_BOTTOM_SW_PACKED:1;
	} bits;
	uint32_t u_all;
};

/* Bitvector and bitfields of possible optimizations
 #IMPORTANT# Keep bitfields match bitvector! */
enum optimization_feature {
	/* Don't do HW programming on panels that were enabled by VBIOS */
	OF_SKIP_HW_PROGRAMMING_ON_ENABLED_EMBEDDED_DISPLAY = 0x1,
	OF_SKIP_RESET_OF_ALL_HW_ON_S3RESUME = 0x2,
	OF_SKIP_HW_RESET_OF_EMBEDDED_DISPLAY_ON_S3RESUME = 0x4,
	OF_SKIP_POWER_UP_VBIOS_ENABLED_ENCODER = 0x8,
	/* Do not turn off VCC while powering down on boot or resume */
	OF_KEEP_VCC_DURING_POWER_DOWN_ON_BOOT_OR_RESUME = 0x10,
	/* Do not turn off VCC while performing SetMode */
	OF_KEEP_VCC_DURING_SET_MODE = 0x20,
	OF_DO_NOT_WAIT_FOR_HPD_LOW = 0x40,
	OF_SKIP_POWER_DOWN_INACTIVE_ENCODER = 0x80
};

union optimization_flags {
	struct {
		/* Don't do HW programming if panels were enabled by VBIOS */
		uint32_t SKIP_HW_PROGRAMMING_ON_ENABLED_EMBEDDED_DISPLAY:1;
		uint32_t SKIP_RESET_OF_ALL_HW_ON_S3RESUME:1;
		uint32_t SKIP_HW_RESET_OF_EMBEDDED_DISPLAY_ON_S3RESUME:1;
		uint32_t SKIP_POWER_UP_VBIOS_ENABLED_ENCODER:1;
		/* Do not turn off VCC while powering down on boot or resume */
		uint32_t KEEP_VCC_DURING_POWER_DOWN_ON_BOOT_OR_RESUME:1;
		/* Do not turn off VCC while performing SetMode */
		uint32_t KEEP_VCC_DURING_SET_MODE:1;
		uint32_t DO_NOT_WAIT_FOR_HPD_LOW:1;
	} bits;
	uint32_t u_all;
};

/* Bitvector and bitfields of performance measurements
 #IMPORTANT# Keep bitfields match bitvector! */
enum perf_measure {
	PERF_MEASURE_ADAPTER_POWER_STATE = 0x1,
	PERF_MEASURE_DISPLAY_POWER_STATE = 0x2,
	PERF_MEASURE_SET_MODE_SEQ = 0x4,
	PERF_MEASURE_DETECT_AT_RESUME = 0x8,
	PERF_MEASURE_MEMORY_READ_CONTROL = 0x10,
};

union perf_measure_flags {
	struct {
		uint32_t ADAPTER_POWER_STATE:1;
		uint32_t DISPLAY_POWER_STATE:1;
		uint32_t SET_MODE_SEQ:1;
		uint32_t DETECT_AT_RESUME:1;
		uint32_t MEMORY_READ_CONTROL:1;

	} bits;
	uint32_t u_all;
};

enum {
	PERF_MEASURE_POWERCODE_OFFSET = 0x0,
	PERF_MEASURE_POWER_CODE_MASK = 0xFF,
	PERF_MEASURE_POWER_STATE_OFFSET = 8,
	PERF_MEASURE_POWER_STATE_MASK = 0x000FF00,
	PERF_MEASURE_PREV_POWER_STATE_OFFSET = 16,
	PERF_MEASURE_PREV_POWER_STATE_MASK = 0x00FF0000,
	PERF_MEASURE_DISPLAY_INDEX_OFFSET = 24,
	PERF_MEASURE_DISPLAY_INDEX_MASK = 0xFF000000
};

enum {
	HDMI_PIXEL_CLOCK_IN_KHZ_297 = 297000,
	TMDS_PIXEL_CLOCK_IN_KHZ_165 = 165000
};

/*
 * DFS-bypass flag
 */
/* Copy of SYS_INFO_GPUCAPS__ENABEL_DFS_BYPASS from atombios.h */
enum {
	DFS_BYPASS_ENABLE = 0x10
};

enum {
	INVALID_BACKLIGHT = -1
};

struct panel_backlight_boundaries {
	uint32_t min_signal_level;
	uint32_t max_signal_level;
};

struct panel_backlight_default_levels {
	uint32_t ac_level_percentage;
	uint32_t dc_level_percentage;
};

#endif
