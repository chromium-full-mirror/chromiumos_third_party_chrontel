////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 Chrontel International Ltd. All rights reserved.  
// Use of this source code is governed by the license that can be found in the LICENSE file.
///////////////////////////////////////////////////////////////////////////////////////////
/*
 * @file  GenTableCH7036.c
 * @version 1.1
 * @revision
**/

/*
 Changes:
    07/02/2010: 
		1) Add GenTableCH7036_outcfg() so the output can be configured as 
		   LVDS and HDMI seperatly or both. The resgiter table will be much shorter when
		   LVDS only.
		2) AVI Infoframe Scan inofrmation default to underscan. (Note: many TV
		   actaully will not underscane even AVI info frame specifies. You may need
		   use MK_SCALE to make sure underscan on all TV).
		3) Tune the power control bits for different outcfg mode to save power.   
				
*/

#ifndef _GENTABLE_CH7036_H
#define _GENTABLE_CH7036_H

//type re-define:

typedef unsigned char			uint8;
typedef unsigned short			uint16;
#ifdef __GNUC__
typedef unsigned int			uint32;
#else
#if _MSC_VER > 800
typedef unsigned int			uint32;
#else
typedef unsigned long			uint32;
#endif
#endif
#ifdef __GNUC__
typedef unsigned long long		uint64;
#else
#if _MSC_VER > 800
typedef unsigned __int64		uint64;
#endif
#endif

typedef signed char				int8;
typedef signed short			int16;
#ifdef __GNUC__
typedef signed int				int32;
#else
#if _MSC_VER > 800
typedef signed int				int32;
#else
typedef signed long				int32;
#endif
#endif
#ifdef __GNUC__
typedef signed long long		int64;
#else
#if _MSC_VER > 800
typedef signed __int64			int64;
#endif
#endif
typedef uint32					ch_bool;
#define ch_true					1
#define ch_false				0

// pixel format for LVDS input
#define PIXEL_FMT_18BIT		0
#define PIXEL_FMT_24BIT		1
// pixel format definition for HDMI/DVI output
//  bit[15:0] indicates HDMI pixel format/mode, bit[16] indicate is DVI(1) or HDMI(0)
//  bit[17] indicates force interlace

#define PIXEL_HDMI_ENCODE_DVI	0x10000
#define PIXEL_HDMI_FOTMAT(x)	(0xFF & (x))
#define MK_PIXEL_HDMI_FORMAT(x) (0xFF & (x))

#define PIXEL_HDMI_ASPECT(x)	 ((0xFF00 & (x))>>8)
#define MK_PIXEL_HDMI_ASPECT(x) (((x) & 0xFF)<<8)

#define PIXEL_HDMI_ENCODE_INTERLACE	0x20000

#define MK_PIXEL_HDMI(dvi,fmt,aspect)  ( (((dvi) & 0x01)<<16)+(((aspect) & 0xFF)<<8)+ ((fmt) & 0xFF) ) 
#define MK_PIXEL_HDMI_INT(dvi,fmt,aspect)  ( PIXEL_HDMI_ENCODE_INTERLACE +(((dvi) & 0x01L)<<16)+(((aspect) & 0xFF)<<8)+ ((fmt) & 0xFF) ) 

// polarity
#define POL_HIGH			1
#define POL_LOW				0

// scale definition
#define SCALE_HDOWN(x)  (0xFF & (x))
#define MK_SCALE_HDOWN(x)  (0xFF & (x))

#define SCALE_VDOWN(x)  ((0xFF00 & (x))>>8)
#define MK_SCALE_VDOWN(x)  ((0xFF & (x))<<8)

#define MK_SCALE(hdown, vdown)  ((0xFF & (hdown))+ ((0xFF & (vdown))<<8))

typedef struct timing_ch7036 {
	uint32 clk_khz;
	uint32  pixel_fmt;	// 0: 18bits - 1: 24bits
	uint8  hs_pol;		// 0:
	uint8  vs_pol;
	uint8  de_pol;

	uint16 ht;
	uint16 ha;
	uint16 ho;
	uint16 hw;
	uint16 vt;
	uint16 va;
	uint16 vo;
	uint16 vw;
	uint32 scale;
} TIMING_CH7036, *PTIMING_CH7036;

// global configuration for CH7036 control
typedef struct config_ch7036 {
	uint8 size;
	uint8 deviceid;     // ch7036 device id
	uint8 revisionid;   // ch7036 revision id
	// lvds transmit control fields
	uint8 comv_off;		// common voltage off (0 or 1)
	uint8 drv_strength; // (0 to 15) 
	//  lvds receiver
	uint8 refdly;       // (0 to 7)
	uint8 fbdly;        // (0 to 7)
	// inout lvds type
	uint8 lvds_inout;   // LVDS input and output type

} CONFIG_CH7036, *PCONFIG_CH7036;

// comv_off default is ON, for certain LVDS panel it can be OFF to save power
#define CFG7036_COMV_OFF_YES	1
#define CFG7036_COMV_OFF_NO		0
#define CFG7036_COMV_OFF_DEFAULT	CFG7036_COMV_OFF_NO

#define CFG7036_DRV_STRENGTH_DEFAULT	0x00

#define CFG7036_REFDLY_DEFAULT	0x0
#define CFG7036_FBDLY_DEFAULT	0x1

#define CFG7036_LVDS_INOUT_18BIT_IN_18BIT_OUT	0x00
#define CFG7036_LVDS_INOUT_18BIT_IN_24BIT_OUT	0x40
#define CFG7036_LVDS_INOUT_24BIT_IN_18BIT_OUT	0x80
#define CFG7036_LVDS_INOUT_24BIT_IN_24BIT_OUT	0xC0

#define CFG7036_LVDS_INOUT_DEFAULT CFG7036_LVDS_INOUT_18BIT_IN_18BIT_OUT

typedef struct reg_ch7036 {
	uint8 index;
	uint8 value;
} REG_CH7036, PREG_CH7036;  

#define IDX_SIZE 0xF0
#define IDX_PAGE 0xE0

#define IDX_END		0xff
#define IDX_CLR		0xfe
#define IDX_SET		0xfd
#define IDX_READ	0xfc


//////////////////////////////////////////////////////////////////////////
//Error type define: easy for return error message to user:
#define ERR_NO_ERROR						0


// output display config
#define OUTCFG_LVDS	0x01
#define OUTCFG_HDMI	0x02

#ifdef __cplusplus
extern "C" {
#endif

// functon implemented in GenTableCH7036.c

void SetGlobalConfigCH7036(struct config_ch7036* pCfg);

// return ERR_xxx 
uint32 GenTableCH7036( struct timing_ch7036* pLvdsInput, 
					   struct timing_ch7036* pHdmiOutput,
					   struct reg_ch7036**   ppRegTable
					   );
					   
uint32 GenTableCH7036_outcfg( struct timing_ch7036* pLvdsInput, 
					   struct timing_ch7036* pHdmiOutput,
					   struct reg_ch7036**   ppRegTable,
                       unsigned int outcfg
					   );

// functions in postgen7036.c or tbl365.c

//	write the registers in the pRegTable returned by GenTableCH7036 
//  to CH7036 by using i2c read/write implementation				   
uint32 SendRegisterTable(struct reg_ch7036*  pRegTable);
					   
uint32 CalculateINCs();

//void Display();

/*
 * The below is the code sequence to generate the register table, then
 * do the post-processing and finally turn on display
 * 
 {
	uint32 err;
	struct reg_ch7036*  pRegTable;

	err = GenTableCH7036( &invmode, &vmode720p, &pRegTable);
	if ((err==ERR_NO_ERROR) && (pRegTable!=0))
	{
		SendRegisterTable(pRegTable);
		CalculateINCs();
		Display();
	}
}
 *  
 */

//-------------------------------------------------------
// real implement of registers read/write functions
unsigned char I2CRead_impl(unsigned char index);
void I2CWrite_impl(unsigned char index, unsigned char value);

#ifdef __cplusplus
}
#endif



#endif  // _GENTABLE_CH7036_H
