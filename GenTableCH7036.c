////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 Chrontel International Ltd. All rights reserved.
// Use of this source code is governed by the license that can be found in the LICENSE file.
/////////////////////////////////////////////////////////////////////////////////
/*
 * @file  GenTableCH7036.c
 * @version 1.0
 * @revision
**/

#include "GenTableCH7036.h"



typedef struct{
	uint16 ht;
	uint16 ha;
	uint16 ho;
	uint16 hw;
	uint16 vt;
	uint16 va;
	uint16 vo;
	uint16 vw;
}TIMING, *PTIMING;


#define POL_INVERT			1
#define POL_NO_INV			0

#define CONST_0001			1
#define CONST_0002			0

typedef struct{
	
	TIMING timing;
	uint32 sfld_0001;
	uint8  pixel_fmt;	
	uint8  hs_pol;		
	uint8  vs_pol;
	uint8  de_pol;
	uint8  sfld_0006;	
	uint8  sfld_0007;
	
	uint8  sfld_0008;	
	uint8  sfld_0009;
	uint8  i2s_len;
	uint8  sfld_0011;
}INPUT_INFO, *PINPUT_INFO;




#define CONST_0003			0
#define CONST_0004			1L
#define CONST_0034			2L
#define CONST_0005			3L
#define CONST_0006			4L


#define CONST_0026		(CONST_0003 << 0) | (CONST_0004 << 4) | (CONST_0034 << 8) | (CONST_0005 << 12) | (CONST_0006 << 16)
#define CONST_0027		(CONST_0004 << 0) | (CONST_0034 << 4) | (CONST_0005 << 8) | (CONST_0006 << 12) | (CONST_0003 << 16)
#define CONST_0028		(CONST_0034 << 0) | (CONST_0005 << 4) | (CONST_0006 << 8) | (CONST_0003 << 12) | (CONST_0004 << 16)
#define CONST_0029		(CONST_0005 << 0) | (CONST_0006 << 4) | (CONST_0003 << 8) | (CONST_0004 << 12) | (CONST_0034 << 16)
#define CONST_0030		(CONST_0006 << 0) | (CONST_0003 << 4) | (CONST_0004 << 8) | (CONST_0034 << 12) | (CONST_0005 << 16)
#define LVDS_CHANNAL_SWAP_OP5		(CONST_0005 << 0) | (CONST_0006 << 4) | (CONST_0034 << 8) | (CONST_0004 << 12) | (CONST_0003 << 16)

typedef struct{
	uint32 sfld_0012;	
	uint32 sfld_0013;		
	uint32 pixel_fmt;		
}IOT1_DFMT, *PIOT1_DFMT;

#define CONST_0007		1
#define CONST_0008		2

typedef struct{
	uint8 sfld_0014;		
	uint8 sfld_0015;		
	uint8 sfld_0016;		
	uint8 sfld_0012;		
	uint8 sfld_0017;	
	uint8 hs_pol;			
	uint8 vs_pol;			
	uint8 sfld_0018;	
}IOT2_DFMT, *PIOT2_DFMT;

typedef struct{
	uint8 sfld_0012;		
	uint8 hs_pol;			
	uint8 vs_pol;			
	uint8 de_pol;			
}IOT3_DFMT, *PIOT3_DFMT;

#define CONST_0009		(1 << 0)
#define CONST_0010		(1 << 1)
#define CONST_0011			(1 << 2)

#define CONST_0012			0
#define CONST_0013			1
#define CONST_0014			2
#define CONST_0015			3

typedef struct{
	uint32 channel;			
	IOT1_DFMT sfld_iot01;		
	IOT2_DFMT sfld_iot02;		
	IOT3_DFMT sfld_iot03;		
	TIMING timing;			
	uint32 sfld_0019;		
	uint8  sfld_0020;	
	uint8  sfld_0021;	
	uint8  sfld_0050;			
	uint8  sfld_0022;			
	uint8  sfld_0023;			
}OUTPUT_INFO, *POUTPUT_INFO;


#define CONST_0035			0
#define CONST_0016			1
#define CONST_0017			3
#define CONST_0018			7

#define CONST_0019				0
#define CONST_0020				1

#define CONST_0021			166000	
#define CONST_0022					3000	

#define CONST_0023			200		
#define CONST_0024			250		
#define CONST_0025			300		

typedef struct{
	uint32 sfld_0024;		
	
	uint8 sfld_0025;		
	uint8 sfld_0026;		
	uint8 sfld_0027;			
	uint8 sfld_0028;			
	
	uint8 sfld_0029;	
	uint8 sfld_0030;	
	
	uint8 sfld_0031;		
	uint8 sfld_0032;		
	
	uint8 sfld_0033;		
}PREFER_INFO, *PPREFER_INFO;


typedef struct{
	uint32 sfld_0034;
	INPUT_INFO* pInfo_T_0001;
	OUTPUT_INFO* pInfo_T_0002;
	PREFER_INFO* pInfo_T_0003;
}DEV_CONTEXT, *PDEV_CONTEXT;



#ifndef _DEF_REG_MAP_H
#define _DEF_REG_MAP_H





#define CONST_0032		5
#define CONST_0033	0x80

static uint8 global_arr_001[5][0x80] = {
	
	{
	0x56, 
	0xF0, 
	0xF3, 
	0x00, 
	0x36, 
	0x58, 
	0xAC, 
	0xDD, 
	0x0F, 
	0x1F, 
	0xB4, 
	0x1A, 
	0x80, 
	0x20, 
	0x00, 
	0x10, 
	0x60, 
	0x11, 
	0xE0, 
	0x0D, 
	0x00, 
	0x0A, 
	0x02, 
	0x00, 
	0x00, 
	0xF8, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x1A, 
	0x80, 
	0x20, 
	0x00, 
	0x10, 
	0x60, 
	0x11, 
	0xE0, 
	0x0D, 
	0x00, 
	0x0A, 
	0x02, 
	0x08, 
	0x00, 
	0x00, 
	0x3C, 
	0x00, 
	0x01, 
	0x01, 
	0xC0, 
	0x01, 
	0x01, 
	0x80, 
	0x40, 
	0x40, 
	0x47, 
	0x88, 
	0x00, 
	0x00, 
	0x00, 
	0x86, 
	0x00, 
	0x11, 
	0x0E, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x40, 
	0x40, 
	0x80, 
	0x00, 
	0x00, 
	0x00, 
	0x1F, 
	0xFF, 
	0x00, 
	0x80, 
	0x10, 
	0x60, 
	0x00, 
	0x0A, 
	0x02, 
	0x08, 
	0x00, 
	0x00, 
	0x00, 
	0x40, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x01, 
	0x2D, 
	0x90, 
	0x20, 
	0x22, 
	0x44, 
	0x24, 
	0x40, 
	0x00, 
	0x10, 
	0x00, 
	0xA0, 
	0x4B, 
	0x18, 
	0x01, 
	0x00, 
	0x00, 
	0x20, 
	0x80, 
	0x18, 
	0x00, 
	0x00, 
	0x01, 
	0x00, 
	0x00, 
	0x00, 
	0xFF, 
	0x0F, 
	0x00, 
	},

	
	{
	0x56, 
	0xF0, 
	0xF3, 
	0x01, 
	0x36, 
	0x58, 
	0xAC, 
	0x20, 
	0x00, 
	0x4B, 
	0x00, 
	0x6D, 
	0x6A, 
	0x51, 
	0x93, 
	0x1C, 
	0x00, 
	0x08, 
	0xC5, 
	0xA8, 
	0x91, 
	0x68, 
	0x29, 
	0x0E, 
	0xC8, 
	0x42, 
	0x6C, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x10, 
	0x07, 
	0xFF, 
	0xB6, 
	0x10, 
	0x00, 
	0x00, 
	0x15, 
	0x18, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x0B, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x08, 
	0x60, 
	0x14, 
	0x20, 
	0x00, 
	0x00, 
	0x20, 
	0x00, 
	0x49, 
	0x10, 
	0xFF, 
	0xFF, 
	0xFF, 
	0x00, 
	0x08, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0xA0, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x7A, 
	0x5E, 
	0x6E, 
	0x1F, 
	0x1F, 
	0x00, 
	0x00, 
	0x00, 
	0x20, 
	0x40, 
	0x40, 
	0x40, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0x50, 
	0x00, 
	0x00, 
	0x09, 
	0x00, 
	0x00, 
	0x70, 
	0x00, 
	0x50, 
	0x00, 
	0x98, 
	0x00, 
	0x98, 
	0xFF, 
	0x00, 
	0x00, 
	0x00, 
	},

	
	{
	0x56, 
	0xF0, 
	0xF3, 
	0xFF, 
	0x36, 
	0x58, 
	0xAC, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	},

	
	{
	0x56, 
	0xF0, 
	0xF3, 
	0x03, 
	0x36, 
	0x58, 
	0xAC, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0xFF, 
	0x00, 
	0x03, 
	0x19, 
	0x40, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x11, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0x09, 
	0x1D, 
	0x0F, 
	0x00, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0x00, 
	0x00, 
	0xFF, 
	0xF8, 
	0xFF, 
	0x00, 
	0x02, 
	0xFF, 
	0x00, 
	0x00, 
	0x00, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	},

	
	{
	0x56, 
	0xF0, 
	0xF3, 
	0x04, 
	0x36, 
	0x58, 
	0xAC, 
	0xFF, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0xC0, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0x00, 
	0x00, 
	0x4F, 
	0x07, 
	0x4F, 
	0x07, 
	0x3B, 
	0x07, 
	0x3B, 
	0x07, 
	0x50, 
	0x00, 
	0x50, 
	0x00, 
	0x10, 
	0x00, 
	0x00, 
	0x10, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0x00, 
	0x03, 
	0x00, 
	0x00, 
	0x00, 
	0x24, 
	0x00, 
	0x40, 
	0xFF, 
	0xFF, 
	0x7F, 
	0x56, 
	0x80, 
	0x3F, 
	0x00, 
	0x84, 
	0x60, 
	0xAF, 
	0x00, 
	0x00, 
	0x80, 
	0x94, 
	0x00, 
	0x80, 
	0x00, 
	0x05, 
	0x38, 
	0x01, 
	0x83, 
	0x11, 
	0x01, 
	0x80, 
	0x90, 
	0x00, 
	0x40, 
	0x4A, 
	0xE4, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x89, 
	0x10, 
	0xAC, 
	0x00, 
	0xE0, 
	0x80, 
	0x80, 
	0x19, 
	0x7D, 
	0x00, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0xFF, 
	0x00, 
	0x0C, 
	},
};

#endif                                       




#ifndef _REG_TABLE_H
#define _REG_TABLE_H



typedef enum{
	FLD0000 = 0,

	
	FLD0001,	FLD0002,	FLD0003,	FLD0004,	FLD0005,	FLD0006,	FLD0007,	FLD0008,	FLD0009, 
	FLD0010,		FLD0011,		FLD0012,		FLD0013,
	FLD0014,	FLD0015,	FLD0016,	FLD0017,	FLD0018,

	
	FLD0019,	FLD0020,	FLD0021,	FLD0022,	FLD0023,
	FLD0024,				FLD0025,				
	FLD0026,	FLD0027,	FLD0028,	FLD0029,	FLD0030,		

	
	FLD0031,	FLD0032,	FLD0033,	FLD0034,	FLD0035,		FLD0036,		FLD0037,	FLD0038,	
	FLD0039,	FLD0040,	FLD0041,	FLD0042,	FLD0043,			FLD0044,	

	
	FLD0045,		FLD0046,			FLD0047,		FLD0048,		FLD0049, 
	FLD0050,			FLD0051,			HDMIIN_FLD0051,		HDMIIN_FLD0050,		FLD0054,
	FLD0055,			FLD0056,			FLD0057,
	FLD0058,			FLD0059,		FLD0033_SCL,		FLD0034_SCL,

	
	FLD0062,			FLD0063,		FLD0064,		FLD0065,		
	FLD0066,			FLD0067,

	
	FLD0066SEC,		FLD0069,			FLD0070,			FLD0080,			FLD0081,		FLD0082,			FLD0083, 
	FLD0084,			FLD00844,		FLD00843,		FLD0087,			FLD0088,		FLD0089,
	FLD0090,				FLD0091,				FLD0092,		FLD0093,		FLD0094, FLD0066OD_SEL,

	
	FLD0096,			FLD0097,		FLD0098,			FLD0099,		FLD0100,	
	FLD0101,			FLD0102,			FLD0103,
	FLD0104,		FLD0105,		FLD0106,			FLD0107,			FLD0108,
	FLD0109,		FLD0110,			FLD0111,			FLD0112,			FLD0113,		FLD0114,
	FLD0115,			FLD0116,			FLD0117,			FLD0118,			FLD0119,		FLD0120,
	FLD0121,			FLD0122,			FLD0123,
	FLD0124,			FLD0125,		FLD0126,						
	
	
	FLD0127,		FLD0128,			FLD0129,		FLD0130,			FLD0130_SER,
	FLD0130_PLL,		FLD0130DRI,		FLD0134,			FLD0135,	
	FLD0136,		FLD0137,			FLD0138,		FLD0139,		FLD0140,	FLD0141,	
	FLD0142,			FLD0143,	
	FLD0144,			FLD0145,			FLD0146,			FLD0147,		FLD0148,		FLD0149,		FLD0150,
	FLD0151,			FLD0152,			FLD0153,

	
	FLD0154,	FLD0155,		FLD0156,			FLD0157,		FLD0158,	

	
	FLD0159,			FLD0160,		FLD0161,

	
	FLD0162,
	
	MUL_ID_END,
	
}MULTI_REG_ID;

typedef struct{
	MULTI_REG_ID sfld_0034;
	uint8 sfld_0035;
	uint8 sfld_0036;
	uint8 sfld_0037;
	uint8 sfld_0038;
	uint8 sfld_0039;
	uint8 sfld_0040;
	uint8 sfld_0041;
	uint8 sfld_0042;
	uint8 sfld_0043;
	uint8 sfld_0044;
	uint8 sfld_0046;
	uint8 sfld_0047;
	uint8 sfld_0048;
}MULTI_REG, *PMULTI_REG;

extern MULTI_REG global_arr_002[];

#endif



MULTI_REG global_arr_002[MUL_ID_END] = 
{
	{FLD0000,       -1,  0, 0,	-1,  0, 0,	-1,  3, 6,	-1,   0, 7,		0},
	
	{FLD0001,		      -1,  0, 0,	-1,  0, 0,	0x0B,3, 6,	0x0D, 0, 7,		0},
	{FLD0002,		      -1,  0, 0,	-1,  0, 0,	0x11,3, 5,	0x13, 0, 7,		0},
	{FLD0003,		      -1,  0, 0,	-1,  0, 0,	0x0B,0, 2,	0x0C, 0, 7,		0},
	{FLD0004,		      -1,  0, 0,	-1,  0, 0,	0x11,0, 2,	0x12, 0, 7,		0},
	{FLD0005,		      -1,  0, 0,	-1,  0, 0,	0x0E,0, 2,	0x0F, 0, 7,		0},
	{FLD0006,		      -1,  0, 0,	-1,  0, 0,	0x14,0, 2,	0x15, 0, 7,		0},
	{FLD0007,		      -1,  0, 0,	-1,  0, 0,	0x0E,3, 5,	0x10, 0, 7,		0},
	{FLD0008,		      -1,  0, 0,	-1,  0, 0,	0x14,3, 5,	0x16, 0, 7,		0},
	{FLD0009,            -1,  0, 0,	0x0F,0, 1,  0x14,0, 7,  0x15, 0, 7,     4},
	{FLD0010, -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x57, 7, 7,		4},	
	{FLD0011, -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x57, 6, 6,		4},
	{FLD0012,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x41, 6, 6,		1},
	{FLD0013,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x41, 5, 5,		1},
	{FLD0014,     -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x58, 4, 4,		4},	
	{FLD0015,     -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x58, 3, 3,		4},	
	{FLD0016,     -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x58, 2, 2,		4},	
	{FLD0017,     -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x58, 1, 1,		4},	
	{FLD0018,	  -1,  0, 0,	-1,	 0, 0,  -1,  0, 0,  0x5F, 0, 0,		4},
	
	{FLD0019,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x5E, 5, 7,		4},	
	{FLD0020,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x5E, 2, 4,		4},	
	{FLD0021,	  -1,  0, 0,	-1,  0, 0,	-1,	 0, 0,  0x5F, 7, 7,		4},	
	{FLD0022,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x5F, 4, 6,		4},	
	{FLD0023, -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x5F, 1, 3,		4},	
	{FLD0024,-1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x59, 5, 6,		4},	
	{FLD0025,-1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x61, 1, 1,		4},
	{FLD0026,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x59, 4, 4,		4},	
    {FLD0027,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x59, 3, 3,		4},	
	{FLD0028,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x59, 2, 2,		4},	
	{FLD0029,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x59, 1, 1,		4},	
	{FLD0030, -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x59, 0, 0,		4},	
	
	{FLD0031,			  -1,  0, 0,	-1,  0, 0,	0x1F,3, 6,	0x21, 0, 7,		0},
	{FLD0032,			  -1,  0, 0,	-1,  0, 0,	0x25,3, 5,	0x27, 0, 7,		0},
	{FLD0033,			  -1,  0, 0,	-1,  0, 0,	0x1F,0, 2,	0x20, 0, 7,		0},
	{FLD0034,			  -1,  0, 0,	-1,  0, 0,	0x25,0, 2,	0x26, 0, 7,		0},
	{FLD0035,	      -1,  0, 0,	-1,  0, 0,	0x54,0, 2,	0x55, 0, 7,		0},
	{FLD0036,	      -1,  0, 0,	-1,  0, 0,	0x57,0, 2,	0x58, 0, 7,		0},
	{FLD0037,	      -1,  0, 0,	-1,  0, 0,	0x54,3, 5,	0x56, 0, 7,		0},
	{FLD0038,	      -1,  0, 0,	-1,  0, 0,	0x57,3, 5,	0x59, 0, 7,		0},
	{FLD0039,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x19, 5, 5,		0},
	{FLD0040,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x19, 4, 4,		0},
	{FLD0041,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x19, 3, 3,		0},
	{FLD0042,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x2E, 4, 4,		0},
	{FLD0043,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x2E, 3, 3,		0},
	{FLD0044,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x2E, 5, 5,		0},
	
	{FLD0045,	      -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x0A, 2, 2,		0},
	{FLD0046,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x1E, 5, 5,		0},
	{FLD0047,       -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x4C, 5, 5,		0},
	{FLD0048,       -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x4C, 0, 4,		0},
	{FLD0049,   -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x7E, 7, 7,		0},
	{FLD0050,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x24, 5, 5,		1},
	{FLD0051,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x24, 4, 4,		1},
	{HDMIIN_FLD0051,	  -1,  0, 0,    -1,  0, 0,	-1,  0, 0,	0x61, 6, 6,		4},
	{HDMIIN_FLD0050,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x61, 5, 5,		4},
	{FLD0054,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x61, 4, 4,		4},
	{FLD0055,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x3D, 2, 3,		0},
	{FLD0056,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x3D, 0, 1,		0},
	{FLD0057,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x40, 0, 5,		0},
	{FLD0058,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x4B, 6, 6,		0},
	{FLD0059,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x6C, 0, 0,		4},
	{FLD0033_SCL,		  -1,  0, 0,	-1,  0, 0,	0x6A,0, 7,	0x6B, 5, 7,		4},
	{FLD0034_SCL,		  -1,  0, 0,	-1,  0, 0,	0x6B,0, 4,	0x6C, 2, 7,		4},
	
	{FLD0062,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x1E, 7, 7,		0},
	{FLD0063,      -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x1E, 6, 6,		0},
	{FLD0064,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x1E, 2, 3,		0},
	{FLD0065,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x1E, 0, 1,		0},
	{FLD0066,            -1,  0, 0,	0x0F,4, 5,  0x10,0, 7,  0x11, 0, 7 ,    4},
	
	{FLD0067,		  -1,  0, 0,	-1,  0, 0,	0x28,  0, 7,0x29, 0, 7,		1},	
	
	{FLD0066SEC,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x12, 4, 4,		1},
	{FLD0069,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x55, 3, 5,		4},	
	{FLD0070,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x55, 0, 2,		4},	
	{FLD0080,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x56, 5, 7,		4},	
	{FLD0081,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x13, 1, 2,		1},
	{FLD0082,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x5E, 0, 7,		0},
	{FLD0083,            -1,  0, 0,	0x0F,2, 3,  0x12,0, 7,  0x13, 0, 7,     4},
	{FLD0084,		  -1,  0, 0,	-1,  0, 0,  0x12,0, 1,	0x13, 7, 7,		1},
	{FLD00844,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x6E, 2, 2,		4},
	{FLD00843,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x6E, 1, 1,		4},
	{FLD0087,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x69, 5, 6,		4},
	{FLD0088,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x69, 3, 4,		4},
	{FLD0089,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x6E, 3, 7,		4},
	{FLD0090,			  0x5A,0, 7,	0x5B,0, 7,	0x5C,0, 7,	0x5D, 0, 7,		0},
	{FLD0091,		      -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x60, 0, 7,		4},
	{FLD0092,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x67, 7, 7,		4},
	{FLD0093,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x0C, 3, 4,		1},
	{FLD0094,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x0C, 1, 2,		1},
	{FLD0066OD_SEL,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x61, 7, 7,		4},
	
	{FLD0096,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x68, 1, 2,		0},
	{FLD0097,	      -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x74, 7, 7,		0},
	{FLD0098,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x74, 3, 3,		0},
	{FLD0099,	      -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x74, 4, 4,		0},
	{FLD0100,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x2D, 6, 7,		0},
	{FLD0101,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x2D, 4, 5,		0},
	{FLD0102,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x2E, 7, 7,		0},
	{FLD0103,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x2E, 6, 6,		0},
	{FLD0104,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x25, 6, 6,		0},
	{FLD0105,	      -1,  0, 0,	-1,  0, 0,	0x60,0, 7,	0x61, 0, 2,		0},
	{FLD0106,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x32, 7, 7,		0},
	{FLD0107,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x32, 6, 6,		0},
	{FLD0108,		      -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x64, 0, 6,		0},
	{FLD0109,		  -1,  0, 0,	-1,  0, 0,	0x6B,0, 7,	0x6C, 0, 3,		0},
	{FLD0110,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x6A, 0, 0,		0},
	{FLD0111,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x6A, 2, 2,		0},
	{FLD0112,			  -1,  0, 0,	-1,  0, 0,	0x6D,0, 7,	0x6E, 0, 2,		0},
	{FLD0113,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x6E, 3, 3,		0},
	{FLD0114,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x2F, 7, 7,		0},
	{FLD0115,			  -1,  0, 0,	-1,  0, 0,	0x2A,0, 7,	0x2B, 0, 2,		4},
	{FLD0116,			  -1,  0, 0,	-1,  0, 0,	0x2C,0, 7,	0x2D, 0, 2,		4},
	{FLD0117,			  -1,  0, 0,	-1,  0, 0,	0x2E,0, 7,	0x2F, 0, 2,		4},
	{FLD0118,			  -1,  0, 0,	-1,  0, 0,	0x30,0, 7,	0x31, 0, 2,		4},
	{FLD0119,		  -1,  0, 0,	-1,  0, 0,	0x32,0, 7,	0x33, 0, 2,		4},
	{FLD0120,		  -1,  0, 0,	-1,  0, 0,	0x34,0, 7,	0x35, 0, 2,		4},
	{FLD0121,			  -1,  0, 0,	0x36,0, 4,	0x37,0, 7,	0x38, 0, 7,		4},
	{FLD0122,			  -1,  0, 0,	0x39,0, 7,	0x3A,0, 7,  0x3B, 0, 7,		4},
	{FLD0123,		      -1,  0, 0,	0x3C,0, 7,	0x3D,0, 7,	0x3E, 0, 7,		4},
	{FLD0124,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x70, 6, 7,		0},
	{FLD0125,	      -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x0A, 7, 7,		0},
	{FLD0126,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x0A, 4, 4,		0},
	
	{FLD0127,	      -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x09, 0, 0,		0},
	{FLD0128,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x07, 6, 6,		0},
	{FLD0129,	      -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x07, 0, 0,		0},
	{FLD0130,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x07, 3, 3,		0},
	{FLD0130_SER,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x16, 0, 0,		1},
	{FLD0130_PLL,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x16, 3, 3,		1}, 
	{FLD0130DRI,	      -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x08, 4, 7,		0},
	{FLD0134,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x07, 4, 4,		0},
	{FLD0135,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x0E, 7, 7,		1},
	{FLD0136,	      -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x0A, 3, 3,		0},
	{FLD0137,		      -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x63, 4, 7,		4},
	{FLD0138,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x64, 0, 0,		4},
    {FLD0139,		  -1,  0, 0,    -1,  0, 0,	-1,  0, 0,	0x66, 1, 1,		4},
	{FLD0140,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x11, 4, 4,		1},
	{FLD0141,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x1A, 4, 4,		1}, 
	{FLD0142,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x09, 2, 2,		0},
	{FLD0143,		      -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x08, 1, 3,		0},
	{FLD0144,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x09, 5, 5,		0},
	{FLD0145,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x07, 1, 1,		0},
	{FLD0146,	      -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x54, 4, 4,		4},
	{FLD0147,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x0F, 7, 7,		1},
	{FLD0148,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x07, 2, 2,		0},
	{FLD0149,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x07, 5, 5,		0},
	{FLD0150,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x07, 7, 7,		0},
	{FLD0151,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x09, 4, 4,		0},
	{FLD0152,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x09, 3, 3,		0},
	{FLD0153,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x0A, 5, 5,		0},
	{FLD0154,	  -1,  0, 0,	-1,  0, 0,0x1C,	 0,	7,  0x1D, 0, 7,		0},
	{FLD0155, 	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x07, 2, 2,		1},
	{FLD0156,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x21, 7, 7,		1},
	{FLD0157,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x58, 5, 5,		4},
	{FLD0158,	  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x4E, 0, 0,		1},
	{FLD0159,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x19, 6, 6,		0},
	{FLD0160,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x07, 1, 1,		1},
	{FLD0161,		  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x08, 0, 0,		1},
	{FLD0162,			  -1,  0, 0,	-1,  0, 0,	-1,  0, 0,	0x2E, 0, 2,		0},
	
};






static uint8 g_nCurPage = 0;
static uint8 global_arr_003[CONST_0032][CONST_0033];

static uint8 global_arr_004[CONST_0032][CONST_0033];
static uint8 g_dirtyreg = 0;
static uint8 g_mcurst = 1;

static struct reg_ch7036  global_arr_005[CONST_0032*CONST_0033];
static uint32 global_arr_005_length =0;


static uint32 append_reg(uint8 index, uint8 value)
{
	global_arr_005_length++;
	global_arr_005[global_arr_005_length].index = index;
	global_arr_005[global_arr_005_length].value = value;
	global_arr_005[0].value = global_arr_005_length & 0xFF;
	global_arr_005[0].index = 0xF0+ (0xF & (global_arr_005_length >>8));
	return global_arr_005_length;

}




static uint8 in_data(uint8 index)
{
	return global_arr_003[g_nCurPage][index];
}

void out_data(uint8 index, uint8 value)
{
	if(index == 0x03)
	{
		g_nCurPage = value;
	}
	else
	{
		global_arr_003[g_nCurPage][index] = value;

		global_arr_004[g_nCurPage][index] = 1;
	}
}

static void iic_reset()
{
	uint8 page, index;
	for(page=0; page<CONST_0032; ++page)
	{
		for(index=0; index<CONST_0033; ++index)
		{
			global_arr_003[page][index] = global_arr_001[page][index];
			global_arr_004[page][index]=0;
		}
	}
	g_nCurPage = 0x00; 
	global_arr_005_length = 0;
}

static uint32 in_data_ex(MULTI_REG_ID reg_id)
{
	uint8 oldpage;
	uint32 n0, n1, n2, n3, mask, i, value;

	
	n0 = n1 = n2 = n3 = value = mask = 0;

	
	i = (uint32)reg_id;

	
	oldpage = in_data(0x03);
	out_data(0x03, global_arr_002[i].sfld_0048);

	if(-1 != global_arr_002[i].sfld_0035)
		n0 = in_data(global_arr_002[i].sfld_0035);

	if(-1 != global_arr_002[i].sfld_0038) 
		n1 = in_data(global_arr_002[i].sfld_0038);

	if(-1 != global_arr_002[i].sfld_0041)
		n2 = in_data(global_arr_002[i].sfld_0041);

	n3 = in_data(global_arr_002[i].sfld_0044);

	out_data(0x03, oldpage);

	if(0 != n0)
	{
		mask = 1;
		mask <<= global_arr_002[i].sfld_0037 - global_arr_002[i].sfld_0036 + 1;
		mask -= 1;
		mask <<= global_arr_002[i].sfld_0036;
		n0 &= mask;
		n0 >>= global_arr_002[i].sfld_0036;
		n0 <<= (global_arr_002[i].sfld_0040 - global_arr_002[i].sfld_0039 + 1)+(global_arr_002[i].sfld_0043 - global_arr_002[i].sfld_0042 + 1) + (global_arr_002[i].sfld_0047 - global_arr_002[i].sfld_0046 + 1);
	}
	if(0 != n1)
	{
		mask = 1;
		mask <<= global_arr_002[i].sfld_0040 - global_arr_002[i].sfld_0039 + 1;
		mask -= 1;
		mask <<= global_arr_002[i].sfld_0039;
		n1 &= mask;
		n1 >>= global_arr_002[i].sfld_0039;
		n1 <<= (global_arr_002[i].sfld_0043 - global_arr_002[i].sfld_0042 + 1) + (global_arr_002[i].sfld_0047 - global_arr_002[i].sfld_0046 + 1);
	}

	if(0 != n2)
	{
		mask = 1;
		mask <<= global_arr_002[i].sfld_0043 - global_arr_002[i].sfld_0042 + 1;
		mask -= 1;
		mask <<= global_arr_002[i].sfld_0042;
		n2 &= mask;
		n2 >>= global_arr_002[i].sfld_0042;	
		n2 <<= global_arr_002[i].sfld_0047 - global_arr_002[i].sfld_0046 + 1;
	}

	if(0 != n3)
	{
		mask = 1;
		mask <<= global_arr_002[i].sfld_0047 - global_arr_002[i].sfld_0046 + 1;
		mask -= 1;
		mask <<= global_arr_002[i].sfld_0046;
		n3 &= mask;
		n3 >>= global_arr_002[i].sfld_0046;
	}

	value = n0 | n1 | n2 | n3;
	return value;
}

static void out_data_ex(MULTI_REG_ID reg_id, uint32 value)
{
	uint8 oldpage;
	uint32 n1, n2, n3, n4, mask, nOrigin, i, value_local;

	
	n1 = n2 = n3 = n4 = mask = nOrigin = i = 0;
	value_local = value;

	
	i = (uint32)reg_id;

	
	oldpage = in_data(0x03);
	out_data(0x03, global_arr_002[i].sfld_0048);

	mask = 1;
	mask <<= global_arr_002[i].sfld_0047 - global_arr_002[i].sfld_0046 + 1;
	mask -= 1;
	n1 = value_local & mask;
	n1 <<= global_arr_002[i].sfld_0046;
	nOrigin = in_data(global_arr_002[i].sfld_0044);
	mask <<= global_arr_002[i].sfld_0046;
	nOrigin &= ~mask;
	n1 |= nOrigin;
	out_data(global_arr_002[i].sfld_0044, n1);
	value_local >>= global_arr_002[i].sfld_0047 - global_arr_002[i].sfld_0046 + 1;

	if(-1 != global_arr_002[i].sfld_0041)
	{
		mask = 1;
		mask <<= global_arr_002[i].sfld_0043 - global_arr_002[i].sfld_0042 + 1;
		mask -= 1;
		n2 = value_local & mask;
		n2 <<= global_arr_002[i].sfld_0042;
		nOrigin = in_data(global_arr_002[i].sfld_0041);
		mask <<= global_arr_002[i].sfld_0042;
		nOrigin &= ~mask;
		n2 |= nOrigin;
		out_data(global_arr_002[i].sfld_0041, n2);
		value_local >>= global_arr_002[i].sfld_0043 - global_arr_002[i].sfld_0042 + 1;

		if(-1 != global_arr_002[i].sfld_0038)
		{
			mask = 1;
			mask <<= global_arr_002[i].sfld_0040 - global_arr_002[i].sfld_0039 + 1;
			mask -= 1;
			n3 = value_local & mask;
			n3 <<= global_arr_002[i].sfld_0039; 
			nOrigin = in_data(global_arr_002[i].sfld_0038);
			mask <<= global_arr_002[i].sfld_0039;
			nOrigin &= ~mask;
			n3 |= nOrigin;
			out_data(global_arr_002[i].sfld_0038, n3);
			value_local >>= global_arr_002[i].sfld_0040 - global_arr_002[i].sfld_0039 + 1;

			if(-1 != global_arr_002[i].sfld_0035)
			{
				mask = 1;
				mask <<= global_arr_002[i].sfld_0037 - global_arr_002[i].sfld_0036 +1;
				mask -= 1;
				n4 = value_local & mask;
				n4 <<= global_arr_002[i].sfld_0036;
				nOrigin = in_data(global_arr_002[i].sfld_0035);
				mask <<= global_arr_002[i].sfld_0036;
				nOrigin &= ~mask;
				n4 |= nOrigin;
				out_data(global_arr_002[i].sfld_0035,n4);
			}
		}		
	}

	out_data(0x03, oldpage);
}


static void func_gen_0001()
{

    append_reg(0x03, 0x04);
	
    append_reg(0x52, 0x2A);  
    append_reg(0x52, 0x2F);
}



static void func_gen_0002()
{

	append_reg(0x03, 0x04);

	append_reg(0x5A, 0x06 );
	append_reg(0x5A, 0x04);

	append_reg(0x52, 0x29);   
	append_reg(0x52, 0x2B );

	append_reg(0x5A, 0x04);
}



static void func_gen_0003()
{
			uint8 page, index;
			uint16 k =0;       
			uint16 rc[5];

	for(page=0; page<CONST_0032; ++page)
	{
		rc[page]=0;
		if(page == 0x02) continue; 


		append_reg(0x03, page);

		for(index=0x07; index<CONST_0033; ++index)
		{
			if (g_dirtyreg) {
				if(global_arr_004[page][index])
				{   
					
					k++;  
					rc[page]++;

					append_reg(index, global_arr_003[page][index]);
				}
			} else {
				if(global_arr_003[page][index] != global_arr_001[page][index])
				{   
					
					k++;  
					rc[page]++;

					append_reg(index, global_arr_003[page][index]);
				}
			}
		}
	}
	

	

	

		
	
	
}







   
   
   

#undef POST_RUN_VIRTUAL_MODE




static uint32 g_nLastError;






static ch_bool func_gen_0004(ch_bool funpar_0001, uint8* funpar_0002, uint8* div_addr); 
static ch_bool func_gen_0005(ch_bool funpar_0003, uint8* funpar_0004, uint8* div_addr);
static ch_bool func_gen_0006(ch_bool funpar_0005, uint8* funpar_0006, uint8* div_addr);
static ch_bool func_gen_0007(ch_bool funpar_0007, uint8* funpar_0008, uint8* div_addr);
static ch_bool func_gen_0008(ch_bool funpar_0009, uint8* funpar_0010, uint8* div_addr);
static ch_bool func_gen_0009(ch_bool funpar_0011, uint8* funpar_0012, uint8* div_addr);
static ch_bool func_gen_0010(ch_bool funpar_0013, uint8* funpar_0014, uint8* div_addr);
static ch_bool func_gen_0011(ch_bool funpar_0015, uint8* funpar_0016, uint8* div_addr);


static ch_bool func_gen_0012(INPUT_INFO* pInfo_T_0001);


static ch_bool func_gen_0013(OUTPUT_INFO* pInfo_T_0002);


static ch_bool func_gen_0014(PREFER_INFO* pInfo_T_0003);


static ch_bool func_gen_0015(DEV_CONTEXT* pDevContext);


static ch_bool func_gen_0016(DEV_CONTEXT* pDevContext);


static ch_bool func_gen_0017(DEV_CONTEXT* pDevContext);

#ifdef POST_RUN_VIRTUAL_MODE

static ch_bool func_gen_0018();
#endif

#ifdef __GNUC__

static uint32 mul32div32(uint32 a, uint32 b, uint32 c)
{
	return (uint32)((((uint64)a)*b)/c);
}    

#else
#if _MSC_VER<=800
extern uint32 dos_mul32div32(uint32 a, uint32 b, uint32 c);

uint32 mul32div32(uint32 a, uint32 b, uint32 c)
{
	uint32 t;
	t = dos_mul32div32( a, b, c);
	return t;
}

#else

static uint32 mul32div32(uint32 a, uint32 b, uint32 c)
{
	return (uint32)((((uint64)a)*b)/c);
}    
#endif
#endif


ch_bool func_gen_0019(DEV_CONTEXT* pDevContext)
{
	uint32 C;
	uint32 locvar_0001, locvar_0002, locvar_0003, val_t;
	uint32 locvar_0004;
	uint32 locvar_0005;
	uint32 locvar_0006;
	uint32 locvar_0007;
	uint32 locvar_0008;
	uint32 locvar_0009;
	uint32 locvar_0010;

	INPUT_INFO* pInfo_T_0001 = pDevContext->pInfo_T_0001;
	OUTPUT_INFO* pInfo_T_0002 = pDevContext->pInfo_T_0002;
	PREFER_INFO* pInfo_T_0003 = pDevContext->pInfo_T_0003;

	
	g_nLastError = 0;

	
	out_data(0x03, 0x04);
	pDevContext->sfld_0034 = in_data(0x50);
	if(pDevContext->sfld_0034 != 0x56)
	{
		g_nLastError = 105;
		return ch_false;
	}

	
	
	if(pInfo_T_0001->timing.ha % 2)
	{
		pInfo_T_0001->timing.ha--;	
	}
	if(pInfo_T_0001->timing.va % 2)
	{
		pInfo_T_0001->timing.va--;	
	}

	
	
	if(pInfo_T_0003->sfld_0026)
	{
		C = 100;
	}
	else if(pInfo_T_0003->sfld_0027)
	{
		C = 100;
	}
	else
	{
		C = (pInfo_T_0003->sfld_0028) ? 75 : 50;
	}
	if(pInfo_T_0002->sfld_0020)
	{
		locvar_0001 = pInfo_T_0002->timing.ha * (100L - pInfo_T_0002->sfld_0020) / 100L;
	}
	else
	{
		locvar_0001 = pInfo_T_0002->timing.ha;
	}
	if(pInfo_T_0002->sfld_0021)
	{
		locvar_0002 = pInfo_T_0002->timing.va * (100L - pInfo_T_0002->sfld_0021) / 100L;
	}
	else
	{
		locvar_0002 = pInfo_T_0002->timing.va;
	}
	locvar_0001 = locvar_0001 + (locvar_0001 % 2);
	locvar_0002 = locvar_0002 + (locvar_0002 % 2);
	locvar_0004 = pInfo_T_0001->timing.ha;
	if(locvar_0004 > locvar_0001 && pInfo_T_0002->sfld_0050 != CONST_0013 && pInfo_T_0002->sfld_0050 != CONST_0015)
	{
		locvar_0004 = locvar_0001;
	}

	if(pInfo_T_0002->sfld_0050 == CONST_0013 || pInfo_T_0002->sfld_0050 == CONST_0015)
	{
		val_t = 720;
	}
	else
	{
		if(pInfo_T_0003->sfld_0026)
		{
			val_t = 720;
		}
		else
		{
			val_t = pInfo_T_0003->sfld_0027 ? 720 : 1440;
		}
	}
	if(locvar_0004 > val_t && (pInfo_T_0002->sfld_0050 != CONST_0012 || pInfo_T_0002->sfld_0022 || pInfo_T_0002->sfld_0023))
	{
		locvar_0004 = val_t;
	}

	if(pInfo_T_0002->channel & CONST_0010)
	{
		
		switch(pInfo_T_0002->sfld_iot02.sfld_0015)
		{
		case  5:
		case  6:
		case  7:
		case 10:
		case 11:
		case 20:
		case 21:
		case 22:
		case 25:
		case 26:
			locvar_0007 = 1;
			break;
		default:
			locvar_0007 = 0;
			break;		
		}
	}
	val_t = locvar_0007 ? (locvar_0002 / 2) : locvar_0002;
	if(pInfo_T_0002->sfld_0050 == CONST_0013 || pInfo_T_0002->sfld_0050 == CONST_0015)
	{
		if(locvar_0004 <= val_t)
			locvar_0006 = 3;
		else
			locvar_0006 = 1;
	}
	else
	{
		if(pInfo_T_0001->timing.va <= val_t)
			locvar_0006 = 3;
		else
			locvar_0006 = 1;
	}
	locvar_0010 = (pInfo_T_0001->sfld_0001 + pInfo_T_0002->sfld_0019 * pInfo_T_0001->timing.ha / locvar_0001 * (4 - locvar_0006)) / 85 * C;

	if(locvar_0010 > CONST_0021)
	{
		if(locvar_0006 >= 2)
		{
			g_nLastError = 103;
			return ch_false;
		}
		
		locvar_0006++;
		locvar_0010 = (pInfo_T_0001->sfld_0001 + pInfo_T_0002->sfld_0019 * pInfo_T_0001->timing.ha / locvar_0001 * (4 - locvar_0006)) / 85 * C;

		if(locvar_0010 > CONST_0021)
		{
			g_nLastError = 103;
			return ch_false;
		}
		pInfo_T_0003->sfld_0033 = 1;
	}

	
	if(pInfo_T_0002->sfld_0050 == CONST_0013 || pInfo_T_0002->sfld_0050 == CONST_0015)
	{
		val_t = 100;
	}
	else
	{
		if(pInfo_T_0003->sfld_0026)
		{
			val_t = 100;
		}
		else if(pInfo_T_0003->sfld_0027)
		{
			val_t = 100;
		}
		else
		{
			val_t = pInfo_T_0003->sfld_0028 ? 75 : 50;
		}
	}
	if(val_t == 75)
	{
		locvar_0005 = (locvar_0004 / 4) * 3 + (locvar_0004 % 4);
	}
	else
	{
		locvar_0005 = locvar_0004 * val_t / 100;
	}
	if(pInfo_T_0002->sfld_0050 != CONST_0012 || pInfo_T_0002->sfld_0022 || pInfo_T_0002->sfld_0023)
	{
		locvar_0008 = 45;
	}
	else if(locvar_0005 <= 720 && pInfo_T_0001->timing.va <= 720)
	{
		locvar_0008 = 45;
	}
	else
	{
		locvar_0008 = locvar_0005 / 16;
		locvar_0008 = (locvar_0005 % 16) ? (locvar_0008 + 1) : locvar_0008;
	}

	
	if((pInfo_T_0002->sfld_0050 == CONST_0013 || pInfo_T_0002->sfld_0050 == CONST_0015) && pInfo_T_0003->sfld_0027 == 0 && pInfo_T_0003->sfld_0028 == 0 && pInfo_T_0003->sfld_0026 == 0)
	{
		locvar_0003 = pInfo_T_0001->timing.va / 2;
	}
	else
	{
		locvar_0003 = pInfo_T_0001->timing.va;
	}
	val_t = (locvar_0003 % 16) ? (locvar_0003 / 16 + 1) : (locvar_0003 / 16);
	locvar_0009 = locvar_0008 * val_t;

	
	if(pInfo_T_0003->sfld_0026)
	{
		if(pInfo_T_0003->sfld_0027)
		{
			g_nLastError = 106;
			return ch_false;
		}
		if(pInfo_T_0003->sfld_0028)
		{
			g_nLastError = 107;
			return ch_false;
		}
		if(pInfo_T_0002->sfld_0050 == CONST_0012 && pInfo_T_0002->sfld_0022 == 0 && pInfo_T_0002->sfld_0023 == 0)
		{
			if(locvar_0009 > CONST_0022)
			{
				g_nLastError = 104;
				return ch_false;
			}
		}
		else if(pInfo_T_0002->sfld_0050 != CONST_0012)	
		{
			if(pInfo_T_0001->timing.va > 720)
			{
				g_nLastError = 108;
				return ch_false;
			}
		}
		else 
		{
			if(pInfo_T_0001->timing.va > 720)
			{
				g_nLastError = 109;
				return ch_false;
			}
		}
	}

	
	if(pInfo_T_0003->sfld_0026 == 0)
	{
		if(pInfo_T_0002->sfld_0050 == CONST_0012 && pInfo_T_0002->sfld_0022 == 0 && pInfo_T_0002->sfld_0023 == 0)
		{	
			if(locvar_0009 > CONST_0022)	
			{
				if(pInfo_T_0003->sfld_0027)
				{
					g_nLastError = 106;
					return ch_false;
				}
				if(pInfo_T_0003->sfld_0028)
				{
					g_nLastError = 107;
					return ch_false;
				}
				g_nLastError = 104;
				return ch_false;
			}
		}
		if(pInfo_T_0002->sfld_0050 == CONST_0014)
		{
			if(pInfo_T_0003->sfld_0028)
			{
				g_nLastError = 107;
				return ch_false;
			}
			if(pInfo_T_0003->sfld_0027)
			{
				if(pInfo_T_0001->timing.va > 720)
				{
					g_nLastError = 106;
					return ch_false;
				}
			}
			else
			{
				if(pInfo_T_0001->timing.va > 720)
				{
					g_nLastError = 108;
					return ch_false;
				}
			}
		}
		if(pInfo_T_0002->sfld_0022 || pInfo_T_0002->sfld_0023)
		{
			if(pInfo_T_0003->sfld_0028)
			{
				g_nLastError = 107;
				return ch_false;
			}
			if(pInfo_T_0001->timing.va > 720)
			{
				g_nLastError = 109;
				return ch_false;
			}
		}
		if(pInfo_T_0002->sfld_0050 == CONST_0013 || pInfo_T_0002->sfld_0050 == CONST_0015)
		{
			if(pInfo_T_0003->sfld_0028)
			{
				g_nLastError = 107;
				return ch_false;
			}
			if(pInfo_T_0003->sfld_0027)
			{
				if(pInfo_T_0001->timing.va > 720)
				{
					g_nLastError = 106;
					return ch_false;
				}
			}
			else
			{
				if(pInfo_T_0001->timing.va > 1440)
				{
					g_nLastError = 108;
					return ch_false;
				}
			}
		}
	}

	
	
	if(pInfo_T_0003->sfld_0027 || pInfo_T_0003->sfld_0028)
	{
		pInfo_T_0003->sfld_0031 = 0;
	}

	

	
	iic_reset();

	
	if(pInfo_T_0002->channel & CONST_0010 || pInfo_T_0002->channel & CONST_0011) 
	{
		
		out_data(0x03, 0x01);
		out_data(0x07, 0x7E);
		out_data(0x09, 0x0B);
		out_data(0x03, 0x00);

	}

	return ch_true;
}

ch_bool func_gen_0020(DEV_CONTEXT* pDevContext)
{
	if(!func_gen_0012(pDevContext->pInfo_T_0001))
		return ch_false;

	if(!func_gen_0013(pDevContext->pInfo_T_0002))
		return ch_false;

	if(!func_gen_0014(pDevContext->pInfo_T_0003))
		return ch_false;

	if(!func_gen_0015(pDevContext))
		return ch_false;

	if(!func_gen_0016(pDevContext))
		return ch_false;

	if(!func_gen_0017(pDevContext))
		return ch_false;

	
	if(pDevContext->pInfo_T_0002->channel & CONST_0010) {

	}

	return ch_true;
}


ch_bool func_gen_0021(DEV_CONTEXT* pDevContext)
{

   
		
		func_gen_0002();

		
		func_gen_0003();

#ifdef POST_RUN_VIRTUAL_MODE
	
		{
 			if(!func_gen_0018())
				return ch_false;
		}	
	if(pInfo_T_0002->channel & CONST_0010 || pInfo_T_0002->channel & CONST_0011) 
	{
		uint8 val_t;
		
		out_data(0x03, 0x00);
		val_t = in_data(0x0A);
		out_data(0x0A, val_t | 0x80);
		out_data(0x0A, val_t & 0x7F);
		


			out_data(0x03, 0x04);
			val_t = in_data(0x52);
			out_data(0x52, val_t & 0xFE);
		

			if (g_mcurst) val_t |= 0x04;    
			out_data(0x52, val_t | 0x01);
			
				
		out_data(0x03, 0x00);
		val_t = in_data(0x0A);
		out_data(0x0A, val_t & 0xEF);

			val_t = in_data(0x52);
			val_t = val_t & 0xFB;
			out_data(0x52, val_t);
			out_data(0x52, val_t | 0x04);
	}
#endif
	return ch_true;
}


uint32 GetLastErrorMessage()
{
	return g_nLastError;
}



ch_bool func_gen_0012(INPUT_INFO* pInfo_T_0001)
{
	
	out_data_ex(FLD0001, pInfo_T_0001->timing.ht);
	out_data_ex(FLD0003, pInfo_T_0001->timing.ha);
	out_data_ex(FLD0005, pInfo_T_0001->timing.ho);
	out_data_ex(FLD0007, pInfo_T_0001->timing.hw);
	out_data_ex(FLD0002, pInfo_T_0001->timing.vt);
	out_data_ex(FLD0004, pInfo_T_0001->timing.va);
	out_data_ex(FLD0006, pInfo_T_0001->timing.vo);
	out_data_ex(FLD0008, pInfo_T_0001->timing.vw);
	

	
	out_data_ex(FLD0010, pInfo_T_0001->pixel_fmt);
	
	
	out_data_ex(FLD0012, pInfo_T_0001->hs_pol);
	out_data_ex(FLD0013, pInfo_T_0001->vs_pol);
	
	out_data_ex(FLD0039, pInfo_T_0001->hs_pol);
	out_data_ex(FLD0040, pInfo_T_0001->vs_pol);
	out_data_ex(FLD0041, pInfo_T_0001->de_pol);
	
	out_data_ex(FLD0014, (pInfo_T_0001->sfld_0006 & ( ((uint32)1) <<  0)) ? 1 : 0);
	out_data_ex(FLD0015, (pInfo_T_0001->sfld_0006 & ( ((uint32)1) <<  1)) ? 1 : 0);
	out_data_ex(FLD0016, (pInfo_T_0001->sfld_0006 & ( ((uint32)1) <<  2)) ? 1 : 0);
	out_data_ex(FLD0017, (pInfo_T_0001->sfld_0006 & ( ((uint32)1) <<  3)) ? 1 : 0);
	
	out_data_ex(FLD0018, pInfo_T_0001->sfld_0007);
	
	out_data_ex(FLD0063, pInfo_T_0001->sfld_0008);
	out_data_ex(FLD0062, pInfo_T_0001->sfld_0009);
	out_data_ex(FLD0064, pInfo_T_0001->i2s_len);
	out_data_ex(FLD0065, pInfo_T_0001->sfld_0011);

	return ch_true;
}

ch_bool func_gen_0013(OUTPUT_INFO* pInfo_T_0002)
{
	uint8 locvar_0011, locvar_0012, locvar_0013, locvar_0014, locvar_0015;
	uint8 locvar_0016, locvar_0017, locvar_0018, locvar_0019, locvar_0020;

	uint8 locvar_0021, locvar_0007, copy, locvar_0022, locvar_0023, locvar_0024, hsp, vsp, m1m0, c1c0, vic; 
	uint32 locvar_0025, locvar_0026;

	
	IOT1_DFMT* locvar_0027 = &pInfo_T_0002->sfld_iot01;
	IOT2_DFMT* locvar_0028 = &pInfo_T_0002->sfld_iot02;


	
	if(pInfo_T_0002->channel & CONST_0010 || pInfo_T_0002->channel & CONST_0011)
	{
		out_data_ex(FLD0031, pInfo_T_0002->timing.ht);
		out_data_ex(FLD0033, pInfo_T_0002->timing.ha);
		out_data_ex(FLD0035, pInfo_T_0002->timing.ho);
		out_data_ex(FLD0037, pInfo_T_0002->timing.hw);
		out_data_ex(FLD0032, pInfo_T_0002->timing.vt);
		out_data_ex(FLD0034, pInfo_T_0002->timing.va);
		out_data_ex(FLD0036, pInfo_T_0002->timing.vo);
		out_data_ex(FLD0038, pInfo_T_0002->timing.vw);

		out_data_ex(FLD0101, pInfo_T_0002->sfld_0050);
		out_data_ex(FLD0102, pInfo_T_0002->sfld_0022);
		out_data_ex(FLD0103, pInfo_T_0002->sfld_0023);
		
		locvar_0025 = pInfo_T_0002->timing.ha * (100L - pInfo_T_0002->sfld_0020) / 100L;
		locvar_0025 = locvar_0025 + (locvar_0025 % 2);
		locvar_0026 = pInfo_T_0002->timing.va * (100L - pInfo_T_0002->sfld_0021) / 100L;
		locvar_0026 = locvar_0026 + (locvar_0026 % 2);
		if(pInfo_T_0002->sfld_0020 || pInfo_T_0002->sfld_0021)
		{
			out_data_ex(FLD0059, 1);
			out_data_ex(FLD0033_SCL, locvar_0025);
			out_data_ex(FLD0034_SCL, locvar_0026);
		}
		
		out_data_ex(FLD0101, pInfo_T_0002->sfld_0050);
		out_data_ex(FLD0102, pInfo_T_0002->sfld_0022);
		out_data_ex(FLD0103, pInfo_T_0002->sfld_0023);
	}
	
	if(pInfo_T_0002->channel & CONST_0009)
	{
		
		locvar_0011 = (locvar_0027->sfld_0012 >>  0) & 0xF;
		locvar_0012 = (locvar_0027->sfld_0012 >>  4) & 0xF;
		locvar_0013 = (locvar_0027->sfld_0012 >>  8) & 0xF;
		locvar_0014 = (locvar_0027->sfld_0012 >> 12) & 0xF;
		locvar_0015 = (locvar_0027->sfld_0012 >>  16) & 0xF;
		out_data_ex(FLD0019, locvar_0011);
		out_data_ex(FLD0020, locvar_0012);
		out_data_ex(FLD0021, locvar_0013);
		out_data_ex(FLD0022, locvar_0014);
		out_data_ex(FLD0023, locvar_0015);
		
		locvar_0016 = (locvar_0027->sfld_0013 >> 0) & 0x1;
		locvar_0017 = (locvar_0027->sfld_0013 >> 1) & 0x1;
		locvar_0018 = (locvar_0027->sfld_0013 >> 2) & 0x1;
		locvar_0019 = (locvar_0027->sfld_0013 >> 3) & 0x1;
		locvar_0020 = (locvar_0027->sfld_0013 >> 4) & 0x1;
		out_data_ex(FLD0026, locvar_0016);
		out_data_ex(FLD0027, locvar_0017);
		out_data_ex(FLD0028, locvar_0018);
		out_data_ex(FLD0029, locvar_0019);
		out_data_ex(FLD0030, locvar_0020);
		
		out_data_ex(FLD0011, locvar_0027->pixel_fmt);
	}
	
	if(pInfo_T_0002->channel & CONST_0011)
	{

		out_data_ex(FLD0042, 1);
		out_data_ex(FLD0043, 0);
		out_data_ex(FLD0044, 0);

	}

	{
		
			

		out_data_ex(HDMIIN_FLD0051, 0);
		out_data_ex(HDMIIN_FLD0050, 1);
		out_data_ex(FLD0054, 1);

	}

	
	if(pInfo_T_0002->channel & CONST_0010)
	{
		
		locvar_0021 = locvar_0028->sfld_0014 ? 0 : 1;
		out_data_ex(FLD0045, locvar_0021);
		
		switch(locvar_0028->sfld_0015)
		{
		case  5:
		case  6:
		case  7:
		case 10:
		case 11:
		case 20:
		case 21:
		case 22:
		case 25:
		case 26:
			locvar_0007 = 1;
			break;
		default:
			locvar_0007 = 0;
			break;		
		}
		out_data_ex(FLD0046, locvar_0007);
		
		copy = 1;
		out_data_ex(FLD0058, copy);
		
		locvar_0022 = locvar_0028->sfld_0017;
		out_data_ex(FLD0047, locvar_0022);
		
		locvar_0023 = locvar_0028->sfld_0012;
		out_data_ex(FLD0048, locvar_0023);
		
		locvar_0024 = 1;
		out_data_ex(FLD0049, locvar_0024);
		
		hsp = locvar_0028->hs_pol;
		vsp = locvar_0028->vs_pol;
		out_data_ex(FLD0051, hsp);
		out_data_ex(FLD0050, vsp);
		
		
		switch(locvar_0028->sfld_0015)
		{
		case 1:
			{
				if(locvar_0028->sfld_0016 == CONST_0008) 
				{
					g_nLastError = 201;
					return ch_false;
				}
			}
			break;
		case 4:
		case 5:
		case 16:
		case 19:
		case 20:
		case 31:
		case 32:
		case 33:
		case 34:
			{
				if(locvar_0028->sfld_0016 == CONST_0007)	
				{
					g_nLastError = 201;
					return ch_false;
				}
			}
			break;
		default:
			break;
		}
		m1m0 = locvar_0028->sfld_0016;
		out_data_ex(FLD0055, m1m0);
		
		c1c0 = 2;
		out_data_ex(FLD0056, c1c0);
		
		vic = locvar_0028->sfld_0015;
		out_data_ex(FLD0057, vic);
	}

	
	out_data( 0x3,  0x1);
	out_data( 0x23, 0x63);
	out_data( 0x3,  0x0);

	return ch_true;
}

ch_bool func_gen_0014(PREFER_INFO* pInfo_T_0003)
{
	uint8 hsync_cnt_th, prbs_set_sel;
	uint8 dbp; 

	
	

	
	out_data_ex(FLD0066OD_SEL, pInfo_T_0003->sfld_0025);
	out_data_ex(FLD0097, pInfo_T_0003->sfld_0026);
	out_data_ex(FLD0098, pInfo_T_0003->sfld_0027);
	out_data_ex(FLD0099, pInfo_T_0003->sfld_0028);
	
	hsync_cnt_th = pInfo_T_0003->sfld_0029;
	out_data_ex(FLD0024, hsync_cnt_th);
	prbs_set_sel = pInfo_T_0003->sfld_0030;
	out_data_ex(FLD0025, prbs_set_sel);
	
	dbp = pInfo_T_0003->sfld_0031 ? 0 : 1;
	out_data_ex(FLD0159, dbp);
	
	out_data_ex(FLD0162, 5);

	return ch_true;
}


ch_bool func_gen_0015(DEV_CONTEXT* pDevContext)
{
	uint8 locvar_0029, locvar_0030;
	uint8 locvar_0031, locvar_0032;
	uint8 locvar_0033, locvar_0034;
	uint8 locvar_0035, locvar_0036;
	uint8 locvar_0037, locvar_0038;
	uint8 locvar_0039, locvar_0040;
	uint8 locvar_0041, locvar_0042;
	uint8 locvar_0043, locvar_0044;
	
	uint32 locvar_0045;
	uint32 locvar_0046;
	uint32 locvar_0047;
	uint32 locvar_0048;
	uint8 locvar_0049;
	uint8 locvar_0050;
	uint8 locvar_0051;
	
	uint32 val_t;
	uint32 val_t1, val_t2;

	
	INPUT_INFO* pInfo_T_0001 = pDevContext->pInfo_T_0001;
	OUTPUT_INFO* pInfo_T_0002 = pDevContext->pInfo_T_0002;
	PREFER_INFO* pInfo_T_0003 = pDevContext->pInfo_T_0003;

	
	locvar_0030 = 4;
	locvar_0032 = 8;
	if(!func_gen_0005(ch_false, &locvar_0029, &locvar_0030))
	{
		g_nLastError = 202;
		return ch_false;
	}
	if(!func_gen_0006(ch_false, &locvar_0031, &locvar_0032))
	{
		g_nLastError = 203;
		return ch_false;
	}
	out_data_ex(FLD0070, locvar_0029);
	out_data_ex(FLD0080, locvar_0031);

	
	for(locvar_0034=1; locvar_0034<=64; locvar_0034<<=1)
	{
		val_t = pInfo_T_0001->sfld_0001 / locvar_0034;
		if(val_t >= 2300 && val_t <= 4600)
			break;
	}
	if(locvar_0034 > 64)
	{
		g_nLastError = 204;
		return ch_false;
	}
	if(!func_gen_0004(ch_false, &locvar_0033, &locvar_0034))
	{
		g_nLastError = 204;
		return ch_false;
	}
	out_data_ex(FLD0069, locvar_0033);

	
	for(locvar_0036=1; locvar_0036<=8; locvar_0036<<=1)
	{
		val_t = pInfo_T_0003->sfld_0024 * locvar_0036 / 64;
		if(val_t >= 2300 && val_t <= 2600)
			break;
	}
	if(locvar_0036 > 8)
	{
		g_nLastError = 205;
		return ch_false;
	}
	if(!func_gen_0010(ch_false, &locvar_0035, &locvar_0036))
	{
		g_nLastError = 205;
		return ch_false;
	}
	out_data_ex(FLD0081, locvar_0035);

	
	val_t1 = (pInfo_T_0001->sfld_0001 << 12);
	val_t2 = (locvar_0034 * locvar_0036 * pInfo_T_0003->sfld_0024);
	locvar_0045 = (val_t1 % val_t2) ? (val_t1 / val_t2 + 1) : (val_t1 / val_t2); 
	out_data_ex(FLD0082, locvar_0045);

	
	if(pInfo_T_0001->sfld_0008) 
	{
		locvar_0049 = pInfo_T_0002->sfld_0019 / 10;
	}
	else 
	{
		locvar_0048 = (pInfo_T_0001->sfld_0001 * 2 * locvar_0030 * locvar_0032) / (locvar_0034 * 10);
	}
	out_data_ex(FLD0067, locvar_0048);

	
	
	if((pInfo_T_0002->channel & CONST_0010) && (pInfo_T_0002->sfld_0019 == 54000	||
												  pInfo_T_0002->sfld_0019 == 72000	||
												  pInfo_T_0002->sfld_0019 == 74250	||
												  pInfo_T_0002->sfld_0019 == 108000	||
												  pInfo_T_0002->sfld_0019 == 148500  ))
	{
		locvar_0049 = 1;
	}
	else
	{
		locvar_0049 = 0;
	}
	out_data_ex(FLD0066SEC, locvar_0049);

	
	if(locvar_0049 == 0)
	{
		locvar_0040 = 8;
		locvar_0042 = 8;
		for(locvar_0038=1; locvar_0038<=8; ++locvar_0038)
		{
			val_t = pInfo_T_0002->sfld_0019 * locvar_0038 / 64;
			if(val_t >= 2300 && val_t <= 4600)
				break;
		}
		if(locvar_0038 > 8)
		{
			g_nLastError = 206;
			return ch_false;
		}
		locvar_0044 = 1; 
		if(pInfo_T_0003->sfld_0025 == 1)
		{

			locvar_0046 = mul32div32(pInfo_T_0002->sfld_0019* locvar_0034 * locvar_0038, ( ((uint32)1) <<  20), pInfo_T_0001->sfld_0001); 
			out_data_ex(FLD0090, locvar_0046);
		}
		else
		{
			locvar_0047 = pInfo_T_0001->sfld_0001 * ( ((uint32)1) <<  12) / (pInfo_T_0002->sfld_0019 * locvar_0034 * locvar_0038);
			out_data_ex(FLD0091, locvar_0047);
		}
	}
	else 
	{
		switch(pInfo_T_0002->sfld_0019)
		{
		case 54000:
			locvar_0044 = 6;
			locvar_0042 = 8;
			locvar_0040 = 6;
			locvar_0038 = 4;
			break;
		case 72000:
			locvar_0044 = 6;
			locvar_0042 = 8;
			locvar_0040 = 6;
			locvar_0038 = 3;
			break;
		case 74250:
			locvar_0044 = 5;
			locvar_0042 = 11;
			locvar_0040 = 5;
			locvar_0038 = 4;
			break;
		case 108000:
			locvar_0044 = 6;
			locvar_0042 = 8;
			locvar_0040 = 6;
			locvar_0038 = 2;
			break;
		case 148500:
			locvar_0044 = 5;
			locvar_0042 = 11;
			locvar_0040 = 5;
			locvar_0038 = 2;
			break;
		default:
			return ch_false;
		}
	}
	if(!func_gen_0007(ch_false, &locvar_0037, &locvar_0038))
	{
		g_nLastError = 206;
		return ch_false;
	}
	out_data_ex(FLD0084, locvar_0037 & 0x07);
	out_data_ex(FLD00843, locvar_0037 >> 3 & 0x01);
	if(!func_gen_0008(ch_false, &locvar_0039, &locvar_0040))
	{
		g_nLastError = 207;
		return ch_false;
	}
	out_data_ex(FLD0087, locvar_0039);
	if(!func_gen_0009(ch_false, &locvar_0041, &locvar_0042))
	{
		g_nLastError = 208;
		return ch_false;
	}
	out_data_ex(FLD0088, locvar_0041);
	if(!func_gen_0011(ch_false, &locvar_0043, &locvar_0044))
	{
		g_nLastError = 209;
		return ch_false;
	}
	out_data_ex(FLD0089, locvar_0043);

	
	if(pInfo_T_0001->sfld_0001 > 40000)
	{
		out_data_ex(FLD0092, 1);
	}

	
	if(pInfo_T_0002->channel & CONST_0010)
	{
		val_t = pInfo_T_0002->sfld_0019;
		if(val_t < 25000 || val_t > 165000)
		{
			g_nLastError = 210;
			return ch_false;
		}
		if(val_t <= 40000)
		{
			locvar_0050 = 0;
			locvar_0051 = 0;
		}
		else if(val_t < 80000)
		{
			locvar_0050 = 1;
			locvar_0051 = 1;
		}
		else
		{
			locvar_0050 = 2;
			locvar_0051 = 2;
		}
		out_data_ex(FLD0093, locvar_0050);
		out_data_ex(FLD0094, locvar_0051);
	}

	
	out_data_ex(FLD0160, 1);
	
	return ch_true;
}

ch_bool func_gen_0016(DEV_CONTEXT* pDevContext)
{
	uint8 locvar_0052;
	uint32 locvar_0092, locvar_0053, locvar_0054;
	uint32 locvar_0004, locvar_0005;
	uint8 locvar_0055, locvar_0056;
	uint8 locvar_0057;
	uint32 locvar_0058;
	uint8 locvar_0008_reg;
	uint32 locvar_0009_reg;
	uint8 locvar_0059;
	uint8 locvar_0060;
	uint32 locvar_0061;
	uint8 locvar_0006_reg;
	uint8 locvar_0062;
	uint8 locvar_0063;
	uint8 locvar_0064;

	uint32 val_t;
	uint32 locvar_0001;
	uint32 locvar_0002;
	uint32 locvar_0003;

	
	INPUT_INFO* pInfo_T_0001 = pDevContext->pInfo_T_0001;
	OUTPUT_INFO* pInfo_T_0002 = pDevContext->pInfo_T_0002;
	PREFER_INFO* pInfo_T_0003 = pDevContext->pInfo_T_0003;

	
	locvar_0052 = 0;
	out_data_ex(FLD0100, locvar_0052);

	
	locvar_0092 = ((pInfo_T_0001->sfld_0001 * 10000L) / pInfo_T_0001->timing.ht) * 10L / pInfo_T_0001->timing.vt;
	locvar_0053 = ((pInfo_T_0002->sfld_0019 * 10000L) / pInfo_T_0002->timing.ht) * 10L / pInfo_T_0002->timing.vt;
	locvar_0054 = in_data_ex(FLD0046) ? (locvar_0053 * 2) : (locvar_0053);

	
	locvar_0004 = pInfo_T_0001->timing.ha;
	locvar_0058 = 0;
	locvar_0055 = 0;
	locvar_0056 = 0;
	if(pInfo_T_0002->sfld_0020)
	{
		locvar_0001 = pInfo_T_0002->timing.ha * (100L - pInfo_T_0002->sfld_0020) / 100L;
	}
	else
	{
		locvar_0001 = pInfo_T_0002->timing.ha;
	}
	locvar_0001 = locvar_0001 + (locvar_0001 % 2);
	if(locvar_0004 > locvar_0001)
	{
		locvar_0057 = 1;
		locvar_0058 = locvar_0001;
		locvar_0004 = locvar_0058;
		locvar_0055 = 1;
		locvar_0056 = 1;
	}
	else
	{
		locvar_0057 = 0;
		locvar_0055 = 1;
		locvar_0056 = 1;
	}
	if(pInfo_T_0002->sfld_0050 == CONST_0013 || pInfo_T_0002->sfld_0050 == CONST_0015)
	{
		val_t = 720;
	}
	else
	{
		if(pInfo_T_0003->sfld_0026)
		{
			val_t = 720;
		}
		else
		{
			val_t = pInfo_T_0003->sfld_0027 ? 720 : 1440;
		}
	}
	if(locvar_0004 > val_t && (pInfo_T_0002->sfld_0050 != CONST_0012 || pInfo_T_0002->sfld_0022 || pInfo_T_0002->sfld_0023))
	{
		locvar_0057 = 1;
		locvar_0058 = val_t;
		locvar_0004 = locvar_0058;
		locvar_0055 = 1;
		locvar_0056 = 1;
	}
	out_data_ex(FLD0104, locvar_0057);
	out_data_ex(FLD0105, locvar_0058);
	
	if(pInfo_T_0001->timing.ha * 100L / locvar_0001 > pInfo_T_0003->sfld_0032)
	{
		locvar_0056 = 0;
	}
	out_data_ex(FLD0106, locvar_0055);
	out_data_ex(FLD0107, locvar_0056);

	
	if(pInfo_T_0002->sfld_0050 == CONST_0013 || pInfo_T_0002->sfld_0050 == CONST_0015)
	{
		val_t = 100;
	}
	else
	{
		if(pInfo_T_0003->sfld_0026)
		{
			val_t = 100;
		}
		else if(pInfo_T_0003->sfld_0027)
		{
			val_t = 100;
		}
		else
		{
			val_t = pInfo_T_0003->sfld_0028 ? 75 : 50;
		}
	}
	if(val_t == 75)
	{
		locvar_0005 = (locvar_0004 / 4) * 3 + (locvar_0004 % 4);
	}
	else
	{
		locvar_0005 = locvar_0004 * val_t / 100;
	}
	if(pInfo_T_0002->sfld_0050 != CONST_0012 || pInfo_T_0002->sfld_0022 || pInfo_T_0002->sfld_0023)
	{
		locvar_0008_reg = 45;
	}
	else if(locvar_0005 <= 720 && pInfo_T_0001->timing.va <= 720)
	{
		locvar_0008_reg = 45;
	}
	else
	{
		locvar_0008_reg = locvar_0005 / 16;
		locvar_0008_reg = (locvar_0005 % 16) ? (locvar_0008_reg + 1) : locvar_0008_reg;
	}
	out_data_ex(FLD0108, locvar_0008_reg);

	
	if((pInfo_T_0002->sfld_0050 == CONST_0013 || pInfo_T_0002->sfld_0050 == CONST_0015) && pInfo_T_0003->sfld_0027 == 0 && pInfo_T_0003->sfld_0028 == 0 && pInfo_T_0003->sfld_0026 == 0)
	{
		locvar_0003 = pInfo_T_0001->timing.va / 2;
	}
	else
	{
		locvar_0003 = pInfo_T_0001->timing.va;
	}
	val_t = (locvar_0003 % 16) ? (locvar_0003 / 16 + 1) : (locvar_0003 / 16);
	locvar_0009_reg = locvar_0008_reg * val_t;
	if(locvar_0009_reg > 2048)
	{
		locvar_0059 = 1;
		locvar_0060 = 1;
		val_t = (locvar_0092 * 100) / locvar_0054;
		if(val_t > 100)
		{
			locvar_0061 = pInfo_T_0001->timing.vt * (10000L - 10000L / val_t) / 10000L + CONST_0023;
		}
		else
		{
			locvar_0061 = pInfo_T_0001->timing.vt * (100 - val_t) / 100 + CONST_0023;
		}
	}
	else
	{
		locvar_0059 = 0;
		locvar_0060 = 0;
		locvar_0061 = 0;
	}
	out_data_ex(FLD0109, locvar_0009_reg);
	out_data_ex(FLD0110, locvar_0059);
	out_data_ex(FLD0111, locvar_0060);
	out_data_ex(FLD0112, locvar_0061);

	
	if(pInfo_T_0002->sfld_0021)
	{
		locvar_0002 = pInfo_T_0002->timing.va * (100L - pInfo_T_0002->sfld_0021) / 100L;
	}
	else
	{
		locvar_0002 = pInfo_T_0002->timing.va;
	}
	locvar_0002 = locvar_0002 + (locvar_0002 % 2);
	val_t = in_data_ex(FLD0046) ? (locvar_0002 / 2) : locvar_0002;
	if(pInfo_T_0002->sfld_0050 == CONST_0013 || pInfo_T_0002->sfld_0050 == CONST_0015)
	{
		if(locvar_0004 <= val_t)
			locvar_0006_reg = 3;
		else
			locvar_0006_reg = 1;
	}
	else
	{
		if(pInfo_T_0001->timing.va <= val_t)
			locvar_0006_reg = 3;
		else
			locvar_0006_reg = 1;
	}
	if(pInfo_T_0003->sfld_0033)
	{
		locvar_0006_reg++;
	}
	out_data_ex(FLD0096, locvar_0006_reg);

	
	if(locvar_0092 >= locvar_0054)
		locvar_0062 = 1;
	else
		locvar_0062 = 0;
	out_data_ex(FLD0113, locvar_0062);

	
	locvar_0063 = (	(pInfo_T_0003->sfld_0026 == 0)		&&
					(pInfo_T_0003->sfld_0027 == 0)			&&
					(pInfo_T_0003->sfld_0028 == 0)		&&
					(	((pInfo_T_0002->sfld_0050 == 0)	&&
						(pInfo_T_0002->sfld_0022 == 1))	||
						((pInfo_T_0002->sfld_0050 == 1)	&&
						(pInfo_T_0002->sfld_0022 == 0))	||
						((pInfo_T_0002->sfld_0050 == 3) &&
						(pInfo_T_0002->sfld_0022 == 1)) ||
						((pInfo_T_0002->sfld_0050 == 2) &&
						(pInfo_T_0002->sfld_0022 == 0))  )   ) ? 1 : 0;
	out_data_ex(FLD0114, locvar_0063);

	
	locvar_0064 = 2;
	out_data_ex(FLD0124, locvar_0064);

	return ch_true;	
}

#ifdef POST_RUN_VIRTUAL_MODE
ch_bool func_gen_0018()
{
	uint32 hdinc_reg, hdinca_reg, hdincb_reg;
	uint32 hinc_reg, hinca_reg, hincb_reg;
	uint32 vinc_reg, vinca_reg, vincb_reg;

	uint32 val_t;

	
	out_data(0x03, 0x04);
	
	val_t = in_data(0x2A);
	hinca_reg = (val_t << 3) | (in_data(0x2B) & 0x07);
	
	val_t = in_data(0x2C);
	hincb_reg = (val_t << 3) | (in_data(0x2D) & 0x07);
	
	val_t = in_data(0x2E);
	vinca_reg = (val_t << 3) | (in_data(0x2F) & 0x07);
	
	val_t = in_data(0x30);
	vincb_reg = (val_t << 3) | (in_data(0x31) & 0x07);
	
	val_t = in_data(0x32); 
	hdinca_reg = (val_t << 3) | (in_data(0x33) & 0x07);
	
	val_t = in_data(0x34);
	hdincb_reg = (val_t << 3) | (in_data(0x35) & 0x07);

	
	out_data(0x03, 0x04);

	
	if(in_data_ex(FLD0104))
	{
		if(hdincb_reg == 0)
		{
			g_nLastError = 301;
			return ch_false;
		}

		hdinc_reg = mul32div32(hdinca_reg, ( ((uint32)1) <<  20), hdincb_reg);
		out_data(0x3C, (hdinc_reg >> 16) & 0xFF);
		out_data(0x3D, (hdinc_reg >>  8) & 0xFF);
		out_data(0x3E, (hdinc_reg >>  0) & 0xFF);
	}
	if(hincb_reg == 0 || vincb_reg == 0)
	{
		g_nLastError = 301;
		return ch_false;
	}
	
	if(hinca_reg > hincb_reg)
	{
		g_nLastError = 302;
		return ch_false;
	}


	hinc_reg = mul32div32(hinca_reg, ( ((uint32)1) <<  20), hincb_reg);

	vinc_reg = mul32div32(vinca_reg, ( ((uint32)1) <<  20), vincb_reg);

	
	out_data(0x36, (hinc_reg >> 16) & 0xFF);
	out_data(0x37, (hinc_reg >>  8) & 0xFF);
	out_data(0x38, (hinc_reg >>  0) & 0xFF);

	out_data(0x39, (vinc_reg >> 16) & 0xFF);
	out_data(0x3A, (vinc_reg >>  8) & 0xFF);
	out_data(0x3B, (vinc_reg >>  0) & 0xFF);

	return ch_true;
}
#endif

ch_bool func_gen_0017(DEV_CONTEXT* pDevContext)
{
	uint8 locvar_0065 = 1;
	uint8 locvar_0066 = 1;
	uint8 locvar_0067 = 1;
	uint8 locvar_0068 = 1;
	uint8 locvar_0068_ser = 1;
	uint8 locvar_0068_pll = 1;
	uint8 locvar_0068dri = 7;


	uint8 locvar_0072 = 0;


	uint8 locvar_0073 = 0;   
	
	uint8 locvar_0074 = 1;
	uint8 locvar_0075 = 63;
	uint8 locvar_0076 = 1;
	uint8 locvar_0077 = 1;
	uint8 locvar_0078 = 1;
	uint8 locvar_0079 = 1;
	uint8 locvar_0080 = 1;
	uint8 locvar_0081 = 7;
	uint8 locvar_0082 = 1;
	uint8 locvar_0083 = 1;


	uint8 locvar_0084 = 0;  

	uint8 locvar_0085 = 1;
	uint8 locvar_0086 = 1;
	uint8 locvar_0087 = 1;
	
	uint8 locvar_0088 = 1;
	uint8 locvar_0089 = 1;
	uint8 locvar_0090 = 1;
	
	uint8 locvar_0091 = 0;

	
	INPUT_INFO* pInfo_T_0001 = pDevContext->pInfo_T_0001;
	OUTPUT_INFO* pInfo_T_0002 = pDevContext->pInfo_T_0002;

	
	
	
	
	




	out_data_ex(FLD0155, 1);
	out_data_ex(FLD0156, 0);
	out_data_ex(FLD0157, 1);
	out_data_ex(FLD0158, 0);
	
	if(pInfo_T_0002->channel & CONST_0009)
	{
		locvar_0074 = 0;
		locvar_0075 = 0;
		locvar_0076 = 0;
		locvar_0077 = 0;
		locvar_0078 = 0;
		locvar_0079 = 0;
		locvar_0086 = 0;
	}
	if(pInfo_T_0002->channel & CONST_0010)
	{
		locvar_0065 = 0;
		if(pInfo_T_0001->sfld_0008 == 1)
			locvar_0066 = 0;
		else
			locvar_0067 = 0;
		locvar_0068 = 0;
		locvar_0068_ser = 0;
		locvar_0068_pll = 0;
		locvar_0068dri = 0;
		locvar_0073 = 0;
		locvar_0075 = 0;
		locvar_0076 = 0;
		locvar_0080 = 0;
		locvar_0083 = 0;
		locvar_0082 = 0;
		locvar_0085 = 0;
		locvar_0086 = 0;
		locvar_0087 = 0;
		locvar_0088 = 0;
		locvar_0089 = 0;
		locvar_0090 = 0;
	}
	if(pInfo_T_0002->channel & CONST_0011)
	{
		locvar_0075 = 0;
		locvar_0076 = 0;
		locvar_0080 = 0;
		locvar_0081 = 0;
		locvar_0083 = 0;
		locvar_0082 = 0;
		locvar_0085 = 0;
		locvar_0086 = 0;
		locvar_0088 = 0;
		locvar_0089 = 0;
		locvar_0090 = 0;
		locvar_0091 = 1;
	}

	
	out_data_ex(FLD0127,		locvar_0065);
	out_data_ex(FLD0128,		locvar_0066);
	out_data_ex(FLD0129,		locvar_0067);
	out_data_ex(FLD0130,		locvar_0068);
	out_data_ex(FLD0130_SER,	locvar_0068_ser);
	out_data_ex(FLD0130_PLL,	locvar_0068_pll);
	out_data_ex(FLD0130DRI,		locvar_0068dri);
	out_data_ex(FLD0134,		locvar_0072);
	out_data_ex(FLD0135,		locvar_0073);
	out_data_ex(FLD0136,		locvar_0074);
	out_data_ex(FLD0137,			locvar_0075);
	out_data_ex(FLD0138,		locvar_0076);
	out_data_ex(FLD0139,		locvar_0077);
	out_data_ex(FLD0140,		locvar_0078);
	out_data_ex(FLD0141,		locvar_0079);
	out_data_ex(FLD0142,		locvar_0080);
	out_data_ex(FLD0143,			locvar_0081);
	out_data_ex(FLD0145,		locvar_0082);
	out_data_ex(FLD0146,		locvar_0083);
	out_data_ex(FLD0147,		locvar_0084);
	out_data_ex(FLD0148,			locvar_0085);
	out_data_ex(FLD0149,			locvar_0086);
	out_data_ex(FLD0150,		locvar_0087);
	out_data_ex(FLD0151,			locvar_0088);
	out_data_ex(FLD0152,			locvar_0089);
	out_data_ex(FLD0153,			locvar_0090);
	out_data_ex(FLD0161,		locvar_0091);

	
	
	
	{
		uint8 val_t;
		out_data(0x03, 0x01);
		
		val_t = in_data(0x16) & 0xF7;
		out_data(0x16, val_t);
		
		val_t = in_data(0x24) & 0xFD;
		out_data(0x24, val_t);
		
		val_t = in_data(0x14) | 0x40;
		out_data(0x14, val_t);
		out_data(0x03, 0x04);
		val_t = in_data(0x54) | 0x40;
		out_data(0x54, val_t);
	}
	
	

	return ch_true;
}

ch_bool func_gen_0004(ch_bool funpar_0001, uint8* funpar_0002, uint8* div_addr)
{
	uint8 val_t;

	if(funpar_0001)	
	{
		if((*funpar_0002) <= 5)
		{
			(*div_addr)= 1<<(*funpar_0002);
		}
		else if((*funpar_0002) == 6 || (*funpar_0002) == 7) 
		{
			(*div_addr) = 64;
		}
		else
		{
			return ch_false;
		}
	}
	else 
	{
		for(val_t=1; val_t<=6; ++val_t)
		{
			if(*div_addr == ( ((uint32)1) <<  val_t))
			{
				*funpar_0002 = val_t;
				return ch_true;
			}
		}
		return ch_false;
	}
	return ch_true;
}

ch_bool func_gen_0005(ch_bool funpar_0003, uint8* funpar_0004, uint8* div_addr)
{
	uint8 val_t;

	if(funpar_0003)	
	{
		if((*funpar_0004) <= 3)
		{
			*div_addr = 1 << (*funpar_0004);
		}
		else if((4 == (*funpar_0004)) || (6 == (*funpar_0004)))
		{
			(*div_addr) = 16;
		}
		else if((5 == (*funpar_0004)) || (7 == (*funpar_0004)))
		{
			(*div_addr) = 32;
		} 
		else 
		{
			return ch_false;
		}
	}
	else	
	{
		for(val_t=0; val_t<=5; ++val_t)
		{
			if(*div_addr == ( ((uint32)1) <<  val_t))
			{
				*funpar_0004 = val_t;
				return ch_true;
			}
		}
		return ch_false;
	}
	return ch_true;
}

ch_bool func_gen_0006(ch_bool funpar_0005, uint8* funpar_0006, uint8* div_addr)
{
	uint8 val_t;
	
	if(funpar_0005)	
	{
		if((*funpar_0006) <= 3)
		{
			*div_addr = 1 << (*funpar_0006);
		}
		else if((4 == (*funpar_0006)) || (6 == (*funpar_0006)))
		{
			(*div_addr) = 16;
		}
		else if((5 == (*funpar_0006)) || (7 == (*funpar_0006)))
		{
			(*div_addr) = 32;
		} 
		else 
		{
			return ch_false;
		}
	}
	else	
	{
		for(val_t=0; val_t<=5; ++val_t)
		{
			if(*div_addr == ( ((uint32)1) <<  val_t))
			{
				*funpar_0006 = val_t;
				return ch_true;
			}
		}
		return ch_false;
	}
	return ch_true;
}

ch_bool func_gen_0007(ch_bool funpar_0007, uint8* funpar_0008, uint8* div_addr)
{
	if(funpar_0007)	
	{
		switch(*funpar_0008)
		{
		case 0:		
			*div_addr = 1;	
			break;
		case 4:		
			*div_addr = 2;	
			break;
		case 1:    
			*div_addr = 3;
			break;
		case 8:    
			*div_addr = 4;
			break;
		case 2:    
			*div_addr = 5;
			break;
		case 5:    
			*div_addr = 6;
			break;
		case 3:    
			*div_addr = 7;
			break;
		case 12:
			*div_addr = 8;
			break;
		default :
			return ch_false;
		}
	}
	else	
	{
		switch(*div_addr)
		{
		case 1:    
			*funpar_0008 = 0;
			break;
		case 2:    
			*funpar_0008 = 4;
			break;
		case 3:    
			*funpar_0008 = 1;
			break;
		case 4:    
			*funpar_0008 = 8;
			break;
		case 5:    
			*funpar_0008 = 2;
			break;
		case 6:    
			*funpar_0008 = 5;
			break;
		case 7:    
			*funpar_0008 = 3;
			break;
		case 8:    
			*funpar_0008 = 0x0C;
			break;
		default : 
			return ch_false;
		}
	}
	return ch_true;
}

ch_bool func_gen_0008(ch_bool funpar_0009, uint8* funpar_0010, uint8* div_addr)
{
	if(funpar_0009)
	{
		if(*funpar_0010 <= 3)
		{
			(*div_addr)= (*funpar_0010) + 5;
			return ch_true;
		}
		return ch_false;
	}
	else
	{
		if(*div_addr >= 5 && *div_addr <= 8)
		{
			*funpar_0010 = (*div_addr) - 5;
			return ch_true;
		}
		return ch_false;
	}
	return ch_true;
}

ch_bool func_gen_0009(ch_bool funpar_0011, uint8* funpar_0012, uint8* div_addr)
{
	if(funpar_0011)
	{
		if(*funpar_0012 <= 3)
		{
			(*div_addr)= (*funpar_0012) + 8;
			return ch_true;
		}
		return ch_false;
	}
	else
	{
		if(*div_addr >= 8 && *div_addr <= 11)
		{
			*funpar_0012 = (*div_addr) - 8;
			return ch_true;
		}
		return ch_false;
	}
	return ch_true;
}

ch_bool func_gen_0010(ch_bool funpar_0013, uint8* funpar_0014, uint8* div_addr)
{
	uint8 val_t;

	if(funpar_0013)	
	{
		if(*funpar_0014 <= 3)
		{
			*div_addr= 1 << (*funpar_0014);
			return ch_true;
		}
		return ch_false;
	}
	else	
	{
		for(val_t=0; val_t<=3; ++val_t)
		{
			if(*div_addr == ( ((uint32)1) <<  val_t))
			{
				*funpar_0014 = val_t;
				return ch_true;
			}
		}
        return ch_false;
	}

	return ch_true;
}

ch_bool func_gen_0011(ch_bool funpar_0015, uint8* funpar_0016, uint8* div_addr)
{
	if(funpar_0015)	
	{
		switch(*funpar_0016)
		{
		case 0x00:  
			*div_addr = 1;
			break;
		case 0x04: 
			*div_addr = 2;
			break;
		case 0x08: 
			*div_addr = 3;
			break;
		case 0x0C:  
			*div_addr = 4;
			break;
		case 0x10: 
			*div_addr = 5;
			break;
		case 0x11: 
			*div_addr = 6;
			break;
		case 0x12: 
			*div_addr = 7;
			break;
		case 0x13: 
			*div_addr = 8;
			break;
		case 0x01: 
			*div_addr = 9;
			break;
		case 0x14: 
			*div_addr = 10;
			break;
		case 0x02: 
			*div_addr = 11;
			break;
		case 0x15: 
			*div_addr = 12;
			break;
		case 0x03: 
			*div_addr = 13;
			break;
		case 0x16: 
			*div_addr = 14;
			break;
		case 0x18: 
			*div_addr = 15;
			break;
		case 0x17: 
			*div_addr = 16;
			break;
		default:  
			return ch_false;
		}
	}
	else	
	{
		switch(*div_addr)
		{
		case 1:  
			*funpar_0016 = 0x00;
			break;
		case 2: 
			*funpar_0016 = 0x04;
			break;
		case 3: 
			*funpar_0016 = 0x08;
			break;
		case 4: 
			*funpar_0016 = 0x0C;
			break;
		case 5:
			*funpar_0016 = 0x10;
			break;
		case  6: 
			*funpar_0016 = 0x11;
			break;
		case 7: 
			*funpar_0016 = 0x12;
			break;
		case 8: 
			*funpar_0016 = 0x13;
			break;
		case 9: 
			*funpar_0016 = 0x01;
			break;
		case 10: 
			*funpar_0016 = 0x14;
			break;
		case 11: 
			*funpar_0016 = 0x02;
			break;
		case 12: 
			*funpar_0016 = 0x15;
			break;
		case 13: 
			*funpar_0016 = 0x03;
			break;
		case 14: 
			*funpar_0016 = 0x16;
			break;
		case 15:
			*funpar_0016 = 0x18;
			break;
		case 16: 
			*funpar_0016 = 0x17;
			break;
		default: 
			return ch_false;
		}
	}
	return ch_true;
}




static void func_gen_0022(INPUT_INFO* pInfo_T_0001, struct timing_ch7036* pLvdsInput)
{
	pInfo_T_0001->timing.ht = pLvdsInput->ht;
	pInfo_T_0001->timing.ha = pLvdsInput->ha;
	pInfo_T_0001->timing.ho = pLvdsInput->ho;
	pInfo_T_0001->timing.hw = pLvdsInput->hw;
	pInfo_T_0001->timing.vt = pLvdsInput->vt;
	pInfo_T_0001->timing.va = pLvdsInput->va;
	pInfo_T_0001->timing.vo = pLvdsInput->vo;
	pInfo_T_0001->timing.vw = pLvdsInput->vw;
	pInfo_T_0001->sfld_0001 = pLvdsInput->clk_khz; 

	pInfo_T_0001->pixel_fmt = pLvdsInput->pixel_fmt;
	pInfo_T_0001->hs_pol = pLvdsInput->hs_pol;
	pInfo_T_0001->vs_pol = pLvdsInput->vs_pol;
	pInfo_T_0001->de_pol = pLvdsInput->de_pol;

	pInfo_T_0001->sfld_0006 = POL_NO_INV;
	pInfo_T_0001->sfld_0007 = POL_NO_INV;

	pInfo_T_0001->sfld_0008 = CONST_0002;
	pInfo_T_0001->sfld_0009 = 0;
	pInfo_T_0001->i2s_len = 0;
	pInfo_T_0001->sfld_0011 = 0;
}

static void func_gen_0023(OUTPUT_INFO* pInfo_T_0002, struct timing_ch7036* pHdmiOutput)
{
	pInfo_T_0002->sfld_iot02.sfld_0014 = (PIXEL_HDMI_ENCODE_DVI & pHdmiOutput->pixel_fmt) ? 1 : 0;
	pInfo_T_0002->sfld_iot02.sfld_0015 = PIXEL_HDMI_FOTMAT(pHdmiOutput->pixel_fmt); 
	pInfo_T_0002->sfld_iot02.sfld_0016 = PIXEL_HDMI_ASPECT(pHdmiOutput->pixel_fmt);
	pInfo_T_0002->timing.ht = pHdmiOutput->ht;
	pInfo_T_0002->timing.ha = pHdmiOutput->ha;
	pInfo_T_0002->timing.ho = pHdmiOutput->ho;
	pInfo_T_0002->timing.hw = pHdmiOutput->hw;
	pInfo_T_0002->timing.vt = pHdmiOutput->vt;
	pInfo_T_0002->timing.va = pHdmiOutput->va;
	pInfo_T_0002->timing.vo = pHdmiOutput->vo;
	pInfo_T_0002->timing.vw = pHdmiOutput->vw;
	pInfo_T_0002->sfld_0019 = pHdmiOutput->clk_khz;

	pInfo_T_0002->channel = CONST_0009 | CONST_0010 | CONST_0011;
	
	pInfo_T_0002->sfld_iot01.sfld_0012 = CONST_0026;
	pInfo_T_0002->sfld_iot01.sfld_0013 = (POL_LOW << 4) | (POL_LOW << 3) | (POL_LOW << 2) | (POL_LOW << 1) | (POL_LOW << 0);
	pInfo_T_0002->sfld_iot01.pixel_fmt = PIXEL_FMT_18BIT;

	pInfo_T_0002->sfld_iot02.sfld_0012 = 0;
	pInfo_T_0002->sfld_iot02.sfld_0017 = POL_NO_INV;
    
	pInfo_T_0002->sfld_iot02.hs_pol = pHdmiOutput->hs_pol;
	pInfo_T_0002->sfld_iot02.vs_pol = pHdmiOutput->vs_pol;
	pInfo_T_0002->sfld_iot02.sfld_0018 = 0;


	pInfo_T_0002->sfld_0020 = SCALE_HDOWN(pHdmiOutput->scale); 
	pInfo_T_0002->sfld_0021 = SCALE_VDOWN(pHdmiOutput->scale); 

	pInfo_T_0002->sfld_0050 = 0;
	pInfo_T_0002->sfld_0022 = 0;
	pInfo_T_0002->sfld_0023 = 0;
}

static void func_gen_0024(PREFER_INFO* pInfo_T_0003)
{
	pInfo_T_0003->sfld_0024 = 166000; 
	pInfo_T_0003->sfld_0025 = 1;
	pInfo_T_0003->sfld_0026 = 0;
	pInfo_T_0003->sfld_0027 = 0;
	pInfo_T_0003->sfld_0028 = 0;
	pInfo_T_0003->sfld_0029 = CONST_0035;
	pInfo_T_0003->sfld_0030 = CONST_0020;
	pInfo_T_0003->sfld_0031 = 1;
	pInfo_T_0003->sfld_0033 = 0;
	pInfo_T_0003->sfld_0032 = 130;	
}


uint32 GenTableCH7036( struct timing_ch7036* pLvdsInput, 
					   struct timing_ch7036* pHdmiOutput,
					   struct reg_ch7036**   ppRegTable
					   )
{
	INPUT_INFO input_info;
	OUTPUT_INFO output_info;
	PREFER_INFO prefer_info;
	DEV_CONTEXT DevContext;
	uint32 last_error;

	if (ppRegTable) *ppRegTable =0;
	
	
		func_gen_0022(&input_info, pLvdsInput);
		func_gen_0023(&output_info, pHdmiOutput);
		func_gen_0024(&prefer_info);
		DevContext.pInfo_T_0001 = &input_info;
		DevContext.pInfo_T_0002 = &output_info;
		DevContext.pInfo_T_0003 = &prefer_info;
		
		
		
		
		iic_reset();
		if(!func_gen_0019(&DevContext))
		{
			last_error = GetLastErrorMessage(); 
			if(last_error == 106)
			{
				
				prefer_info.sfld_0027 = 0;
				prefer_info.sfld_0028 = 1; 
				if(!func_gen_0019(&DevContext))
				{
					if(GetLastErrorMessage() == 107)
					{
						
						prefer_info.sfld_0028 = 0; 
					}
				} 
			}
			if(last_error == 107)
			{
				
				prefer_info.sfld_0028 = 0;
			}


			if(!func_gen_0019(&DevContext))
			{
				
				last_error= GetLastErrorMessage();
				return last_error;
			}
		}
		
		

		
		
		if(!func_gen_0020(&DevContext))
		{
			
			last_error= GetLastErrorMessage();
			return last_error;
		}


		
		if(!func_gen_0021(&DevContext))
		{
			
			last_error= GetLastErrorMessage();
			return last_error;
		}

		{
			uint8 val_t;

			
			out_data(0x03, 0x00);
			append_reg(0x03, 0x00);
			val_t = in_data(0x0A);
			val_t |= 0x80;
			append_reg(0x0A, val_t);
			val_t &= 0x7F;
			append_reg(0x0A, val_t);
			val_t &= 0xEF;
			append_reg(0x0A, val_t);
		}
        
		func_gen_0001();
		
		if (ppRegTable) *ppRegTable = global_arr_005;

		return 0;
}
