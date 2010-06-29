////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 Chrontel International Ltd. All rights reserved.
// Use of this source code is governed by the license that can be found in the LICENSE file.
/////////////////////////////////////////////////////////////////////////////////
/*
 * @file  postgen7036.c
 * @version 1.0
 * @author Roger Yu, 05/28/10
 * @revision
**/

#include "GenTableCH7036.h"


//=================================================
uint32 SendRegisterTable(struct reg_ch7036*  pRegTable)
{
		uint32 len;
		uint32 i;

		len = 0xF & pRegTable[0].index;
		len = (len<<8) + pRegTable[0].value;
		for (i=1; i<= len; i++) {
			I2CWrite_impl(pRegTable[i].index, pRegTable[i].value);
		}
		return len;
}

uint32 CalculateINCs()
{
	uint32 hdinc_reg, hdinca_reg, hdincb_reg;
	uint32 hinc_reg, hinca_reg, hincb_reg;
	uint32 vinc_reg, vinca_reg, vincb_reg;

	uint32 val_t;

	I2CWrite_impl(0x03, 0x04);
	val_t = I2CRead_impl(0x2A);
	hinca_reg = (val_t << 3) | (I2CRead_impl(0x2B) & 0x07);
	val_t = I2CRead_impl(0x2C);
	hincb_reg = (val_t << 3) | (I2CRead_impl(0x2D) & 0x07);
	val_t = I2CRead_impl(0x2E);
	vinca_reg = (val_t << 3) | (I2CRead_impl(0x2F) & 0x07);
	val_t = I2CRead_impl(0x30);
	vincb_reg = (val_t << 3) | (I2CRead_impl(0x31) & 0x07);
	val_t = I2CRead_impl(0x32); 
	hdinca_reg = (val_t << 3) | (I2CRead_impl(0x33) & 0x07);
	val_t = I2CRead_impl(0x34);
	hdincb_reg = (val_t << 3) | (I2CRead_impl(0x35) & 0x07);

	if(hdincb_reg == 0)
	{
		return -1;
	}
	hdinc_reg = (uint32)((hdinca_reg << 20) / hdincb_reg);

	I2CWrite_impl(0x3C, (hdinc_reg >> 16) & 0xFF);
	I2CWrite_impl(0x3D, (hdinc_reg >>  8) & 0xFF);
	I2CWrite_impl(0x3E, (hdinc_reg >>  0) & 0xFF);
	
	if(hincb_reg == 0 || vincb_reg == 0)
	{
		return -1;
	}
	
	if(hinca_reg > hincb_reg)
	{
		return -1;
	}

	hinc_reg = (uint32)((hinca_reg <<  20) / hincb_reg);
	vinc_reg = (uint32)((vinca_reg <<  20) / vincb_reg);

	I2CWrite_impl(0x36, (hinc_reg >> 16) & 0xFF);
	I2CWrite_impl(0x37, (hinc_reg >>  8) & 0xFF);
	I2CWrite_impl(0x38, (hinc_reg >>  0) & 0xFF);

	I2CWrite_impl(0x39, (vinc_reg >> 16) & 0xFF);
	I2CWrite_impl(0x3A, (vinc_reg >>  8) & 0xFF);
	I2CWrite_impl(0x3B, (vinc_reg >>  0) & 0xFF);

	return ERR_NO_ERROR;
}

void Display()
{

}

