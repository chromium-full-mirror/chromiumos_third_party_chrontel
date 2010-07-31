////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 Chrontel International Ltd. All rights reserved.
// Use of this source code is governed by the license that can be found in the LICENSE file.
/////////////////////////////////////////////////////////////////////////////////
/*
 * @file  GenTableCH7036.h
 * @version 1.0
 * @author Roger Yu, 05/28/10
 * @revision
**/

/*
 Changes:
    07/02/2010:   Roger Yu.
		1) Add GenTableCH7036_outcfg() so the output can be configured as 
		   LVDS and HDMI seperatly or both. The resgiter table will be much shorter when
		   LVDS only.
		2) AVI Infoframe Scan inofrmation default to underscan. (Note: many TV
		   actaully will not underscane even AVI info frame specifies. You may need
		   use MK_SCALE to make sure underscan on all TV).
		3) add MK_PIXEL_HDMI_INT so the output can be force to be interlace for special purpose   
		4) Tune the power control bits for different outcfg mode to save power.   
				
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

// return default value of register
// Note page is specified using the value written to register 3
unsigned char GenDefault(int page, int reg);

#ifdef USE_TBL365
// functions in tbl365.c

//	write the registers in the pRegTable returned by GenTableCH7036 
//  to CH7036 by using i2c read/write implementation				   
uint32 SendRegisterTable(struct reg_ch7036*  pRegTable);
					   
uint32 CalculateINCs();

  // Note clashes with X base type
void Display();

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
#endif

#ifdef __cplusplus
}
#endif



#endif  // _GENTABLE_CH7036_H
