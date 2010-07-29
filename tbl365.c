////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 Chrontel International Ltd. All rights reserved.
// Use of this source code is governed by the license that can be found in the LICENSE file.
/////////////////////////////////////////////////////////////////////////////////
/*
 * @file  lbl365.c
 * @version 1.0
 * @author Roger Yu, 05/28/10
 * @revision
**/
//

#include <stdio.h>
#include "GenTableCH7036.h"


struct timing_ch7036 invmode =
{	76000,		
	PIXEL_FMT_18BIT,
	0,			0,		1,
    1560,	1366,	32,		64,		806,	768,	6,		12,
	0
};

static struct timing_ch7036 OUTPUT_MODE[] = {
	{	
		25200,			
		MK_PIXEL_HDMI(0, 1,	1),
		0,		0,		1,
		800,	640,	16,		96,		525,	480,	10,		2	,
		MK_SCALE(0, 0),
	},
	{	
		27027,			
		MK_PIXEL_HDMI(0, 2,	1),
		0,		0,		1,
		858,	720,	16,		62,		525,	480,	9,		6	,
		MK_SCALE(0, 0),
	},
	{	
		74250,			
		MK_PIXEL_HDMI(0, 4, 2),
		1,		1,		1,
		1650,	1280,	110,	40,		750,	720,	5,		5	,
		MK_SCALE(0, 0),
	},
	{	
		74250,			
		MK_PIXEL_HDMI(0, 5, 2),  // dvi, fmt, aspect
		1,		1,		1,       // hpo, vpo, depo
		2200,	1920,	88,		44,		1125,	1080,	4,		5	,
		MK_SCALE(0, 0),
	},
	{	
		148500,			
		MK_PIXEL_HDMI(0, 16, 2),  // dvi, fmt, aspect
		1,		1,		1,       // hpo, vpo, depo
		2200,	1920,	88,		44,		1125,	1080,	4,		5,
		MK_SCALE(0, 0),
	},
	{	
		27000,			
		MK_PIXEL_HDMI(0, 17, 1),
		0,		0,		1,
		864,	720,	12,		64,		625,	576,	5,		5,
		MK_SCALE(0, 0),
	},
	{	
		74250,			
		MK_PIXEL_HDMI(0, 19, 2),
		1,		1,		1,
		1980,	1280,	440,	40,		750,	720,	5,		5,
		MK_SCALE(0, 0),
	},
	{	
		74250,			
		MK_PIXEL_HDMI(0, 20, 2),  // dvi, fmt, aspect
		1,		1,		1,       // hpo, vpo, depo
		2640,	1920,	528,	44,		1125,	1080,	4,		5,
		MK_SCALE(0, 0),
	},
	{	
		148500,			
		MK_PIXEL_HDMI(0, 31, 2),  // dvi, fmt, aspect
		1,		1,		1,       // hpo, vpo, depo
		2640,	1920,	528,	44,		1125,	1080,	4,		5,
		MK_SCALE(0, 0),
	},

	//////  DVI modes
	{	
		40000,			
		MK_PIXEL_HDMI(1, 0, 0),
		1,		1,		1,
		1056,	800,	40,		128,	628,	600,	1,		4,
		MK_SCALE(0, 0),
	},
	{	
		65000,			
		MK_PIXEL_HDMI(1, 0, 0),
		1,		1,		1,
		1344,	1024,	24,		136,	806,	768,	3,		6,
		MK_SCALE(0, 0),
	},
	{	
		108000,			
		MK_PIXEL_HDMI(1, 0, 0),
		1,		1,		1,
		1688,	1280,	48,		112,	1066,	1024,	1,		3,
		MK_SCALE(0, 0),
	},
	{	
		88750,			
		MK_PIXEL_HDMI(1, 0, 0),
		1,		1,		1,
		1600,	1440,	48,		32,		926,	900,	3,		6,
		MK_SCALE(0, 0),
	},
	{	
		119000,			
		MK_PIXEL_HDMI(1, 0, 0),
		1,		1,		1,
		1840,	1680,	48,		32,		1080,	1050,	3,		6,
		MK_SCALE(0, 0),
	},
	{	
		162000,			
		MK_PIXEL_HDMI(1, 0, 0),
		1,		1,		1,
		2160,	1600,	64,		192,	1250,	1200,	1,		3,
		MK_SCALE(0, 0),
	},
	//=====
	{	
		130250,			
		MK_PIXEL_HDMI(1, 0, 0),
		1,		1,		1,
		1760,  1600, 48, 32,   1235,1200,  3,    4 ,
		MK_SCALE(0, 0),
	},
	{	
		161000,			
		MK_PIXEL_HDMI(1, 0, 0),
		1,		1,		1,
		2160,  1600,112, 168,   1245,1200,  3,    4  ,
		MK_SCALE(0, 0),
	},


};
#define OUTPUT_MODE_NUMBER (sizeof(OUTPUT_MODE)/sizeof(struct timing_ch7036))


int g_verbose = 0;

int g_allmodes = 0;
int g_underscan = 0;


unsigned char I2CRead_impl(unsigned char index)
{
	return 0;
}

void I2CWrite_impl(unsigned char index, unsigned char value)
{
	printf("Impl Write reg[%02x]=%02x\n", index, value);
}

int main(int argc, char* argv[])
{
	uint32 err;
	struct reg_ch7036*  pRegTable;
	int sm, em;
	int i;

	if(argc >= 2)
	{
		i = 1;
		//parsing command:
		while(argv[i])
		{
			if(strcmp(argv[i], "-a") == 0) {
				printf("\n***Run through all the output modes\n");
				g_allmodes = 1;
			}
			else if(strcmp(argv[i], "-us") == 0) {
				printf("\n***Run underscan (-10%%) for the output modes\n");
				g_underscan = 1;
			}
			i++;
		}
	}

	// default 720p60 mode
	sm = 2;
	em = sm+1;
	if (g_allmodes) {
		sm = 0;
		em = OUTPUT_MODE_NUMBER;
	}
	
	for (i=sm; i<em; i++) {
		uint32 t;
		t = (OUTPUT_MODE[i].clk_khz/OUTPUT_MODE[i].ht)*1000/OUTPUT_MODE[i].vt;

		printf("**** Start to prepare mode[%d] %dx%d clk=%d (kHz) frame=%d****\n", i, OUTPUT_MODE[i].ha, OUTPUT_MODE[i].va, OUTPUT_MODE[i].clk_khz, t);

		if (g_underscan) OUTPUT_MODE[i].scale = MK_SCALE(10, 10);
		err = GenTableCH7036( &invmode, &OUTPUT_MODE[i], &pRegTable);
		if ((err==ERR_NO_ERROR) && (pRegTable!=0))
		{
			uint32 len;

			len = SendRegisterTable(pRegTable);

			printf("==== Register table has %d items\n", len);
			err = CalculateINCs();
//			Display();
			printf("---- Done for set up ----.\n\n");
		}
		else {
			printf("!!!! Failed for setup (Error=%d) !!!!\n\n", err); 
		}

		if ( (i+1) < em) {
			printf("Press <Enter> key to test next mode ('q'<Enter> to exit)...\n\n");
			if (getchar() == 'q')	break;
		}

	}
	return 0;
}

