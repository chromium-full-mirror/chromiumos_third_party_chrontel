// Copyright (c) 2010 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#ifndef __CH7036_H__
#define __CH7036_H__

#include "GenTableCH7036.h"

#define CH7036_I2C_ADDR 0x76

#define EXPECTED_CH7036_DEVID 0x56

#define CH7036_PAGE 0x03

#define CH7036_PWRST2 1,0x07
#define CH7036_PWRST3 1,0x08
#define CH7036_PWRST4 1,0x09

#define CH7036_INTIME1_PAGE 1
#define CH7036_INTIME1_REG 0x0b
#define CH7036_INTIME2_PAGE 1
#define CH7036_INTIME2_REG 0x0c

#define CH7036_INTIME4_PAGE 1
#define CH7036_INTIME4_REG 0x0e
#define CH7036_INTIME5_PAGE 1
#define CH7036_INTIME5_REG 0x0f

#define CH7036_AVIINFO1 1, 0x3D
#define CH7036_AVIINFO2 1, 0x3E
#define CH7036_AVIINFO3 1, 0x3F
#define CH7036_AVIINFO4 1, 0x40

#define CH7036_MAGIC2_0E 2, 0x0e
#define CH7036_SDTIME1 2, 0x16

#define CH7036_HDMI_ST 3, 0x25

#define CH7036_HINCA1 4,0x2A
#define CH7036_HINCA2 4,0x2B
#define CH7036_HINCB1 4,0x2C
#define CH7036_HINCB2 4,0x2D
#define CH7036_VINCA1 4,0x2E
#define CH7036_VINCA2 4,0x2F
#define CH7036_VINCB1 4,0x30
#define CH7036_VINCB2 4,0x31

#define CH7036_HDINCA1 4,0x32
#define CH7036_HDINCA2 4,0x33
#define CH7036_HDINCB1 4,0x34
#define CH7036_HDINCB2 4,0x35

#define CH7036_HINCC1 4,0x36
#define CH7036_HINCC2 4,0x37
#define CH7036_HINCC3 4,0x38

#define CH7036_VINCC1 4,0x39
#define CH7036_VINCC2 4,0x3A
#define CH7036_VINCC3 4,0x3B

#define CH7036_HDINCC1 4,0x3C
#define CH7036_HDINCC2 4,0x3D
#define CH7036_HDINCC3 4,0x3E

#define CH7036_DEVID 4,0x50
#define CH7036_RESET 4,0x52
#define CH7036_PWRSTATE1 4,0x54

/* Routines/globals provided by ch7036_access.c */
extern int ch7036_debugi2c; /* Set to cause debug printing */
extern int ch7036_cur_page; /* Current register page being accessed */

int ch_write_reg(int dev, int page, int reg, int data);
int ch_read_reg(int dev, int page, int reg);
int ch_write_reg_sequence (int dev, struct reg_ch7036* seq);
int ch_restore_notin_seq (int dev, struct reg_ch7036* seq);
void ch_reset_datapath (int dev);
void ch_reset_todefault (int dev);
void ch_monitor_on(int dev);
void ch_monitor_off(int dev);
void ch_monitor_off_keep_ddc(int dev);
int ch_hdmi_is_on(int dev);
void ch_calculate_incs(int dev);
int ch_load_firmware(int dev, char* fname);
int ch_mcu_version(int dev, int verbose);
int ch_hdmi_detected(int dev);
int ch_get_edid(int dev, int edid_block, unsigned char* edid);

#endif /* def __CH7036_H__ */
