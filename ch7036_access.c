// Copyright (c) 2010 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h>

#include "ch7036.h"

/* Fast load of firmware does not work yet */
/* #define FASTLOAD */
int ch7036_debugi2c = 0;
int ch7036_cur_page = -1;
static int ch_page_map[5] = {-1, 0, 1, 3, 4};
static int ch_page_revmap[8] = {1, 2, -1, 3, 4, -1, -1, -1};

/* Firmware data area moved in firmware 1.2 */
#ifdef FIRMWARE_0_86
#define MCU_DATA_PAGE 0x04
static unsigned char es_map[16] = {
  0x08,0x09,0x0F,0x10,0x11,0x12,0x13,0x14,
  0x15,0x16,0x17,0x28,0x29,0x47,0x48,0x49
}; //for A version 8/15/10 DT
#else
#define MCU_DATA_PAGE 0x01
static unsigned char es_map[16] = {
  0x26,0x27,0x42,0x43,0x44,0x45,0x46,0x47,
  0x6A,0x51,0x52,0x53,0x57,0x58,0x59,0x5A
};//for D version 8/15/10 DT
#endif

static unsigned char changed[5][256];

typedef struct {
  unsigned char size; // total size should be less than 16 bytes
  unsigned char ver_major;
  unsigned char ver_minor;
  unsigned char did;
  // CH7036 device id
  unsigned char rid;
  // CH7036 revision id
  unsigned char capbility; // FW capability;
  // 0x01: EDID_Reading
  unsigned char reserved;
} FW7036_CFG;


static uint8 g_DefRegMap[5][0x80] = {
  //Page 0 :
  {
    0x56, // Index = 0x00 ( page 0 )
    0xF0, // Index = 0x01 ( page 0 )
    0xF3, // Index = 0x02 ( page 0 )
    0x00, // Index = 0x03 ( page 0 )
    0x36, // Index = 0x04 ( page 0 )
    0x58, // Index = 0x05 ( page 0 )
    0xAC, // Index = 0x06 ( page 0 )
    0xDD, // Index = 0x07 ( page 0 )
    0x0F, // Index = 0x08 ( page 0 )
    0x1F, // Index = 0x09 ( page 0 )
    0xB4, // Index = 0x0A ( page 0 )
    0x1A, // Index = 0x0B ( page 0 )
    0x80, // Index = 0x0C ( page 0 )
    0x20, // Index = 0x0D ( page 0 )
    0x00, // Index = 0x0E ( page 0 )
    0x10, // Index = 0x0F ( page 0 )
    0x60, // Index = 0x10 ( page 0 )
    0x11, // Index = 0x11 ( page 0 )
    0xE0, // Index = 0x12 ( page 0 )
    0x0D, // Index = 0x13 ( page 0 )
    0x00, // Index = 0x14 ( page 0 )
    0x0A, // Index = 0x15 ( page 0 )
    0x02, // Index = 0x16 ( page 0 )
    0x00, // Index = 0x17 ( page 0 )
    0x00, // Index = 0x18 ( page 0 )
    0xF8, // Index = 0x19 ( page 0 )
    0x00, // Index = 0x1A ( page 0 )
    0x00, // Index = 0x1B ( page 0 )
    0x00, // Index = 0x1C ( page 0 )
    0x00, // Index = 0x1D ( page 0 )
    0x00, // Index = 0x1E ( page 0 )
    0x1A, // Index = 0x1F ( page 0 )
    0x80, // Index = 0x20 ( page 0 )
    0x20, // Index = 0x21 ( page 0 )
    0x00, // Index = 0x22 ( page 0 )
    0x10, // Index = 0x23 ( page 0 )
    0x60, // Index = 0x24 ( page 0 )
    0x11, // Index = 0x25 ( page 0 )
    0xE0, // Index = 0x26 ( page 0 )
    0x0D, // Index = 0x27 ( page 0 )
    0x00, // Index = 0x28 ( page 0 )
    0x0A, // Index = 0x29 ( page 0 )
    0x02, // Index = 0x2A ( page 0 )
    0x08, // Index = 0x2B ( page 0 )
    0x00, // Index = 0x2C ( page 0 )
    0x00, // Index = 0x2D ( page 0 )
    0x3C, // Index = 0x2E ( page 0 )
    0x00, // Index = 0x2F ( page 0 )
    0x01, // Index = 0x30 ( page 0 )
    0x01, // Index = 0x31 ( page 0 )
    0xC0, // Index = 0x32 ( page 0 )
    0x01, // Index = 0x33 ( page 0 )
    0x01, // Index = 0x34 ( page 0 )
    0x80, // Index = 0x35 ( page 0 )
    0x40, // Index = 0x36 ( page 0 )
    0x40, // Index = 0x37 ( page 0 )
    0x47, // Index = 0x38 ( page 0 )
    0x88, // Index = 0x39 ( page 0 )
    0x00, // Index = 0x3A ( page 0 )
    0x00, // Index = 0x3B ( page 0 )
    0x00, // Index = 0x3C ( page 0 )
    0x86, // Index = 0x3D ( page 0 )
    0x00, // Index = 0x3E ( page 0 )
    0x11, // Index = 0x3F ( page 0 )
    0x0E, // Index = 0x40 ( page 0 )
    0x00, // Index = 0x41 ( page 0 )
    0x00, // Index = 0x42 ( page 0 )
    0x00, // Index = 0x43 ( page 0 )
    0x00, // Index = 0x44 ( page 0 )
    0x00, // Index = 0x45 ( page 0 )
    0x00, // Index = 0x46 ( page 0 )
    0x00, // Index = 0x47 ( page 0 )
    0x00, // Index = 0x48 ( page 0 )
    0x00, // Index = 0x49 ( page 0 )
    0x00, // Index = 0x4A ( page 0 )
    0x40, // Index = 0x4B ( page 0 )
    0x40, // Index = 0x4C ( page 0 )
    0x80, // Index = 0x4D ( page 0 )
    0x00, // Index = 0x4E ( page 0 )
    0x00, // Index = 0x4F ( page 0 )
    0x00, // Index = 0x50 ( page 0 )
    0x1F, // Index = 0x51 ( page 0 )
    0xFF, // Index = 0x52 ( page 0 )
    0x00, // Index = 0x53 ( page 0 )
    0x80, // Index = 0x54 ( page 0 )
    0x10, // Index = 0x55 ( page 0 )
    0x60, // Index = 0x56 ( page 0 )
    0x00, // Index = 0x57 ( page 0 )
    0x0A, // Index = 0x58 ( page 0 )
    0x02, // Index = 0x59 ( page 0 )
    0x08, // Index = 0x5A ( page 0 )
    0x00, // Index = 0x5B ( page 0 )
    0x00, // Index = 0x5C ( page 0 )
    0x00, // Index = 0x5D ( page 0 )
    0x40, // Index = 0x5E ( page 0 )
    0x00, // Index = 0x5F ( page 0 )
    0x00, // Index = 0x60 ( page 0 )
    0x00, // Index = 0x61 ( page 0 )
    0x00, // Index = 0x62 ( page 0 )
    0x01, // Index = 0x63 ( page 0 )
    0x2D, // Index = 0x64 ( page 0 )
    0x90, // Index = 0x65 ( page 0 )
    0x20, // Index = 0x66 ( page 0 )
    0x22, // Index = 0x67 ( page 0 )
    0x44, // Index = 0x68 ( page 0 )
    0x24, // Index = 0x69 ( page 0 )
    0x40, // Index = 0x6A ( page 0 )
    0x00, // Index = 0x6B ( page 0 )
    0x10, // Index = 0x6C ( page 0 )
    0x00, // Index = 0x6D ( page 0 )
    0xA0, // Index = 0x6E ( page 0 )
    0x4B, // Index = 0x6F ( page 0 )
    0x18, // Index = 0x70 ( page 0 )
    0x01, // Index = 0x71 ( page 0 )
    0x00, // Index = 0x72 ( page 0 )
    0x00, // Index = 0x73 ( page 0 )
    0x20, // Index = 0x74 ( page 0 )
    0x80, // Index = 0x75 ( page 0 )
    0x18, // Index = 0x76 ( page 0 )
    0x00, // Index = 0x77 ( page 0 )
    0x00, // Index = 0x78 ( page 0 )
    0x01, // Index = 0x79 ( page 0 )
    0x00, // Index = 0x7A ( page 0 )
    0x00, // Index = 0x7B ( page 0 )
    0x00, // Index = 0x7C ( page 0 )
    0xFF, // Index = 0x7D ( page 0 )
    0x0F, // Index = 0x7E ( page 0 )
    0x00, // Index = 0x7F ( page 0 )
  },

  //Page 1:
  {
    0x56, // Index = 0x00 ( page 1 )
    0xF0, // Index = 0x01 ( page 1 )
    0xF3, // Index = 0x02 ( page 1 )
    0x01, // Index = 0x03 ( page 1 )
    0x36, // Index = 0x04 ( page 1 )
    0x58, // Index = 0x05 ( page 1 )
    0xAC, // Index = 0x06 ( page 1 )
    0x20, // Index = 0x07 ( page 1 )
    0x00, // Index = 0x08 ( page 1 )
    0x4B, // Index = 0x09 ( page 1 )
    0x00, // Index = 0x0A ( page 1 )
    0x6D, // Index = 0x0B ( page 1 )
    0x6A, // Index = 0x0C ( page 1 )
    0x51, // Index = 0x0D ( page 1 )
    0x93, // Index = 0x0E ( page 1 )
    0x1C, // Index = 0x0F ( page 1 )
    0x00, // Index = 0x10 ( page 1 )
    0x08, // Index = 0x11 ( page 1 )
    0xC5, // Index = 0x12 ( page 1 )
    0xA8, // Index = 0x13 ( page 1 )
    0x91, // Index = 0x14 ( page 1 )
    0x68, // Index = 0x15 ( page 1 )
    0x29, // Index = 0x16 ( page 1 )
    0x0E, // Index = 0x17 ( page 1 )
    0xC8, // Index = 0x18 ( page 1 )
    0x42, // Index = 0x19 ( page 1 )
    0x6C, // Index = 0x1A ( page 1 )
    0x00, // Index = 0x1B ( page 1 )
    0x00, // Index = 0x1C ( page 1 )
    0x00, // Index = 0x1D ( page 1 )
    0x00, // Index = 0x1E ( page 1 )
    0x00, // Index = 0x1F ( page 1 )
    0x00, // Index = 0x20 ( page 1 )
    0x10, // Index = 0x21 ( page 1 )
    0x07, // Index = 0x22 ( page 1 )
    0xFF, // Index = 0x23 ( page 1 )
    0xB6, // Index = 0x24 ( page 1 )
    0x10, // Index = 0x25 ( page 1 )
    0x00, // Index = 0x26 ( page 1 )
    0x00, // Index = 0x27 ( page 1 )
    0x15, // Index = 0x28 ( page 1 )
    0x18, // Index = 0x29 ( page 1 )
    0x00, // Index = 0x2A ( page 1 )
    0x00, // Index = 0x2B ( page 1 )
    0x00, // Index = 0x2C ( page 1 )
    0x00, // Index = 0x2D ( page 1 )
    0x00, // Index = 0x2E ( page 1 )
    0x00, // Index = 0x2F ( page 1 )
    0x00, // Index = 0x30 ( page 1 )
    0x00, // Index = 0x31 ( page 1 )
    0x00, // Index = 0x32 ( page 1 )
    0x00, // Index = 0x33 ( page 1 )
    0x00, // Index = 0x34 ( page 1 )
    0x00, // Index = 0x35 ( page 1 )
    0x00, // Index = 0x36 ( page 1 )
    0x00, // Index = 0x37 ( page 1 )
    0x00, // Index = 0x38 ( page 1 )
    0x00, // Index = 0x39 ( page 1 )
    0x00, // Index = 0x3A ( page 1 )
    0x0B, // Index = 0x3B ( page 1 )
    0x00, // Index = 0x3C ( page 1 )
    0x00, // Index = 0x3D ( page 1 )
    0x00, // Index = 0x3E ( page 1 )
    0x00, // Index = 0x3F ( page 1 )
    0x08, // Index = 0x40 ( page 1 )
    0x60, // Index = 0x41 ( page 1 )
    0x14, // Index = 0x42 ( page 1 )
    0x20, // Index = 0x43 ( page 1 )
    0x00, // Index = 0x44 ( page 1 )
    0x00, // Index = 0x45 ( page 1 )
    0x20, // Index = 0x46 ( page 1 )
    0x00, // Index = 0x47 ( page 1 )
    0x49, // Index = 0x48 ( page 1 )
    0x10, // Index = 0x49 ( page 1 )
    0xFF, // Index = 0x4A ( page 1 )
    0xFF, // Index = 0x4B ( page 1 )
    0xFF, // Index = 0x4C ( page 1 )
    0x00, // Index = 0x4D ( page 1 )
    0x08, // Index = 0x4E ( page 1 )
    0x00, // Index = 0x4F ( page 1 )
    0x00, // Index = 0x50 ( page 1 )
    0x00, // Index = 0x51 ( page 1 )
    0x00, // Index = 0x52 ( page 1 )
    0x00, // Index = 0x53 ( page 1 )
    0x00, // Index = 0x54 ( page 1 )
    0xA0, // Index = 0x55 ( page 1 )
    0x00, // Index = 0x56 ( page 1 )
    0x00, // Index = 0x57 ( page 1 )
    0x00, // Index = 0x58 ( page 1 )
    0x00, // Index = 0x59 ( page 1 )
    0x00, // Index = 0x5A ( page 1 )
    0x7A, // Index = 0x5B ( page 1 )
    0x5E, // Index = 0x5C ( page 1 )
    0x6E, // Index = 0x5D ( page 1 )
    0x1F, // Index = 0x5E ( page 1 )
    0x1F, // Index = 0x5F ( page 1 )
    0x00, // Index = 0x60 ( page 1 )
    0x00, // Index = 0x61 ( page 1 )
    0x00, // Index = 0x62 ( page 1 )
    0x20, // Index = 0x63 ( page 1 )
    0x40, // Index = 0x64 ( page 1 )
    0x40, // Index = 0x65 ( page 1 )
    0x40, // Index = 0x66 ( page 1 )
    0x00, // Index = 0x67 ( page 1 )
    0x00, // Index = 0x68 ( page 1 )
    0x00, // Index = 0x69 ( page 1 )
    0x00, // Index = 0x6A ( page 1 )
    0xFF, // Index = 0x6B ( page 1 )
    0xFF, // Index = 0x6C ( page 1 )
    0xFF, // Index = 0x6D ( page 1 )
    0xFF, // Index = 0x6E ( page 1 )
    0x50, // Index = 0x6F ( page 1 )
    0x00, // Index = 0x70 ( page 1 )
    0x00, // Index = 0x71 ( page 1 )
    0x09, // Index = 0x72 ( page 1 )
    0x00, // Index = 0x73 ( page 1 )
    0x00, // Index = 0x74 ( page 1 )
    0x70, // Index = 0x75 ( page 1 )
    0x00, // Index = 0x76 ( page 1 )
    0x50, // Index = 0x77 ( page 1 )
    0x00, // Index = 0x78 ( page 1 )
    0x98, // Index = 0x79 ( page 1 )
    0x00, // Index = 0x7A ( page 1 )
    0x98, // Index = 0x7B ( page 1 )
    0xFF, // Index = 0x7C ( page 1 )
    0x00, // Index = 0x7D ( page 1 )
    0x00, // Index = 0x7E ( page 1 )
    0x00, // Index = 0x7F ( page 1 )
  },

  //Page 2:
  {
    0x56, // Index = 0x00 ( page 2 )
    0xF0, // Index = 0x01 ( page 2 )
    0xF3, // Index = 0x02 ( page 2 )
    0xFF, // Index = 0x03 ( page 2 )
    0x36, // Index = 0x04 ( page 2 )
    0x58, // Index = 0x05 ( page 2 )
    0xAC, // Index = 0x06 ( page 2 )
    0xFF, // Index = 0x07 ( page 2 )
    0xFF, // Index = 0x08 ( page 2 )
    0xFF, // Index = 0x09 ( page 2 )
    0xFF, // Index = 0x0A ( page 2 )
    0xFF, // Index = 0x0B ( page 2 )
    0xFF, // Index = 0x0C ( page 2 )
    0xFF, // Index = 0x0D ( page 2 )
    0xFF, // Index = 0x0E ( page 2 )
    0xFF, // Index = 0x0F ( page 2 )
    0xFF, // Index = 0x10 ( page 2 )
    0xFF, // Index = 0x11 ( page 2 )
    0xFF, // Index = 0x12 ( page 2 )
    0xFF, // Index = 0x13 ( page 2 )
    0xFF, // Index = 0x14 ( page 2 )
    0xFF, // Index = 0x15 ( page 2 )
    0xFF, // Index = 0x16 ( page 2 )
    0xFF, // Index = 0x17 ( page 2 )
    0xFF, // Index = 0x18 ( page 2 )
    0xFF, // Index = 0x19 ( page 2 )
    0xFF, // Index = 0x1A ( page 2 )
    0xFF, // Index = 0x1B ( page 2 )
    0xFF, // Index = 0x1C ( page 2 )
    0xFF, // Index = 0x1D ( page 2 )
    0xFF, // Index = 0x1E ( page 2 )
    0xFF, // Index = 0x1F ( page 2 )
    0xFF, // Index = 0x20 ( page 2 )
    0xFF, // Index = 0x21 ( page 2 )
    0xFF, // Index = 0x22 ( page 2 )
    0xFF, // Index = 0x23 ( page 2 )
    0xFF, // Index = 0x24 ( page 2 )
    0xFF, // Index = 0x25 ( page 2 )
    0xFF, // Index = 0x26 ( page 2 )
    0xFF, // Index = 0x27 ( page 2 )
    0xFF, // Index = 0x28 ( page 2 )
    0xFF, // Index = 0x29 ( page 2 )
    0xFF, // Index = 0x2A ( page 2 )
    0xFF, // Index = 0x2B ( page 2 )
    0xFF, // Index = 0x2C ( page 2 )
    0xFF, // Index = 0x2D ( page 2 )
    0xFF, // Index = 0x2E ( page 2 )
    0xFF, // Index = 0x2F ( page 2 )
    0xFF, // Index = 0x30 ( page 2 )
    0xFF, // Index = 0x31 ( page 2 )
    0xFF, // Index = 0x32 ( page 2 )
    0xFF, // Index = 0x33 ( page 2 )
    0xFF, // Index = 0x34 ( page 2 )
    0xFF, // Index = 0x35 ( page 2 )
    0xFF, // Index = 0x36 ( page 2 )
    0xFF, // Index = 0x37 ( page 2 )
    0xFF, // Index = 0x38 ( page 2 )
    0xFF, // Index = 0x39 ( page 2 )
    0xFF, // Index = 0x3A ( page 2 )
    0xFF, // Index = 0x3B ( page 2 )
    0xFF, // Index = 0x3C ( page 2 )
    0xFF, // Index = 0x3D ( page 2 )
    0xFF, // Index = 0x3E ( page 2 )
    0xFF, // Index = 0x3F ( page 2 )
    0xFF, // Index = 0x40 ( page 2 )
    0xFF, // Index = 0x41 ( page 2 )
    0xFF, // Index = 0x42 ( page 2 )
    0xFF, // Index = 0x43 ( page 2 )
    0xFF, // Index = 0x44 ( page 2 )
    0xFF, // Index = 0x45 ( page 2 )
    0xFF, // Index = 0x46 ( page 2 )
    0xFF, // Index = 0x47 ( page 2 )
    0xFF, // Index = 0x48 ( page 2 )
    0xFF, // Index = 0x49 ( page 2 )
    0xFF, // Index = 0x4A ( page 2 )
    0xFF, // Index = 0x4B ( page 2 )
    0xFF, // Index = 0x4C ( page 2 )
    0xFF, // Index = 0x4D ( page 2 )
    0xFF, // Index = 0x4E ( page 2 )
    0xFF, // Index = 0x4F ( page 2 )
    0xFF, // Index = 0x50 ( page 2 )
    0xFF, // Index = 0x51 ( page 2 )
    0xFF, // Index = 0x52 ( page 2 )
    0xFF, // Index = 0x53 ( page 2 )
    0xFF, // Index = 0x54 ( page 2 )
    0xFF, // Index = 0x55 ( page 2 )
    0xFF, // Index = 0x56 ( page 2 )
    0xFF, // Index = 0x57 ( page 2 )
    0xFF, // Index = 0x58 ( page 2 )
    0xFF, // Index = 0x59 ( page 2 )
    0xFF, // Index = 0x5A ( page 2 )
    0xFF, // Index = 0x5B ( page 2 )
    0xFF, // Index = 0x5C ( page 2 )
    0xFF, // Index = 0x5D ( page 2 )
    0xFF, // Index = 0x5E ( page 2 )
    0xFF, // Index = 0x5F ( page 2 )
    0xFF, // Index = 0x60 ( page 2 )
    0xFF, // Index = 0x61 ( page 2 )
    0xFF, // Index = 0x62 ( page 2 )
    0xFF, // Index = 0x63 ( page 2 )
    0xFF, // Index = 0x64 ( page 2 )
    0xFF, // Index = 0x65 ( page 2 )
    0xFF, // Index = 0x66 ( page 2 )
    0xFF, // Index = 0x67 ( page 2 )
    0xFF, // Index = 0x68 ( page 2 )
    0xFF, // Index = 0x69 ( page 2 )
    0xFF, // Index = 0x6A ( page 2 )
    0xFF, // Index = 0x6B ( page 2 )
    0xFF, // Index = 0x6C ( page 2 )
    0xFF, // Index = 0x6D ( page 2 )
    0xFF, // Index = 0x6E ( page 2 )
    0xFF, // Index = 0x6F ( page 2 )
    0xFF, // Index = 0x70 ( page 2 )
    0xFF, // Index = 0x71 ( page 2 )
    0xFF, // Index = 0x72 ( page 2 )
    0xFF, // Index = 0x73 ( page 2 )
    0xFF, // Index = 0x74 ( page 2 )
    0xFF, // Index = 0x75 ( page 2 )
    0xFF, // Index = 0x76 ( page 2 )
    0xFF, // Index = 0x77 ( page 2 )
    0xFF, // Index = 0x78 ( page 2 )
    0xFF, // Index = 0x79 ( page 2 )
    0xFF, // Index = 0x7A ( page 2 )
    0xFF, // Index = 0x7B ( page 2 )
    0xFF, // Index = 0x7C ( page 2 )
    0xFF, // Index = 0x7D ( page 2 )
    0xFF, // Index = 0x7E ( page 2 )
    0xFF, // Index = 0x7F ( page 2 )
  },

  //Page 3:
  {
    0x56, // Index = 0x00 ( page 3 )
    0xF0, // Index = 0x01 ( page 3 )
    0xF3, // Index = 0x02 ( page 3 )
    0x03, // Index = 0x03 ( page 3 )
    0x36, // Index = 0x04 ( page 3 )
    0x58, // Index = 0x05 ( page 3 )
    0xAC, // Index = 0x06 ( page 3 )
    0x00, // Index = 0x07 ( page 3 )
    0x00, // Index = 0x08 ( page 3 )
    0x00, // Index = 0x09 ( page 3 )
    0x00, // Index = 0x0A ( page 3 )
    0x00, // Index = 0x0B ( page 3 )
    0xFF, // Index = 0x0C ( page 3 )
    0x00, // Index = 0x0D ( page 3 )
    0x03, // Index = 0x0E ( page 3 )
    0x19, // Index = 0x0F ( page 3 )
    0x40, // Index = 0x10 ( page 3 )
    0x00, // Index = 0x11 ( page 3 )
    0x00, // Index = 0x12 ( page 3 )
    0x00, // Index = 0x13 ( page 3 )
    0x00, // Index = 0x14 ( page 3 )
    0x00, // Index = 0x15 ( page 3 )
    0x00, // Index = 0x16 ( page 3 )
    0x00, // Index = 0x17 ( page 3 )
    0x00, // Index = 0x18 ( page 3 )
    0x00, // Index = 0x19 ( page 3 )
    0x00, // Index = 0x1A ( page 3 )
    0x00, // Index = 0x1B ( page 3 )
    0x00, // Index = 0x1C ( page 3 )
    0x00, // Index = 0x1D ( page 3 )
    0x00, // Index = 0x1E ( page 3 )
    0x00, // Index = 0x1F ( page 3 )
    0x00, // Index = 0x20 ( page 3 )
    0x00, // Index = 0x21 ( page 3 )
    0x00, // Index = 0x22 ( page 3 )
    0x00, // Index = 0x23 ( page 3 )
    0x00, // Index = 0x24 ( page 3 )
    0x11, // Index = 0x25 ( page 3 )
    0xFF, // Index = 0x26 ( page 3 )
    0xFF, // Index = 0x27 ( page 3 )
    0xFF, // Index = 0x28 ( page 3 )
    0xFF, // Index = 0x29 ( page 3 )
    0xFF, // Index = 0x2A ( page 3 )
    0xFF, // Index = 0x2B ( page 3 )
    0xFF, // Index = 0x2C ( page 3 )
    0xFF, // Index = 0x2D ( page 3 )
    0xFF, // Index = 0x2E ( page 3 )
    0xFF, // Index = 0x2F ( page 3 )
    0xFF, // Index = 0x30 ( page 3 )
    0xFF, // Index = 0x31 ( page 3 )
    0xFF, // Index = 0x32 ( page 3 )
    0xFF, // Index = 0x33 ( page 3 )
    0xFF, // Index = 0x34 ( page 3 )
    0xFF, // Index = 0x35 ( page 3 )
    0xFF, // Index = 0x36 ( page 3 )
    0xFF, // Index = 0x37 ( page 3 )
    0xFF, // Index = 0x38 ( page 3 )
    0xFF, // Index = 0x39 ( page 3 )
    0xFF, // Index = 0x3A ( page 3 )
    0xFF, // Index = 0x3B ( page 3 )
    0xFF, // Index = 0x3C ( page 3 )
    0xFF, // Index = 0x3D ( page 3 )
    0xFF, // Index = 0x3E ( page 3 )
    0xFF, // Index = 0x3F ( page 3 )
    0xFF, // Index = 0x40 ( page 3 )
    0x09, // Index = 0x41 ( page 3 )
    0x1D, // Index = 0x42 ( page 3 )
    0x0F, // Index = 0x43 ( page 3 )
    0x00, // Index = 0x44 ( page 3 )
    0xFF, // Index = 0x45 ( page 3 )
    0xFF, // Index = 0x46 ( page 3 )
    0xFF, // Index = 0x47 ( page 3 )
    0xFF, // Index = 0x48 ( page 3 )
    0xFF, // Index = 0x49 ( page 3 )
    0xFF, // Index = 0x4A ( page 3 )
    0xFF, // Index = 0x4B ( page 3 )
    0xFF, // Index = 0x4C ( page 3 )
    0xFF, // Index = 0x4D ( page 3 )
    0xFF, // Index = 0x4E ( page 3 )
    0xFF, // Index = 0x4F ( page 3 )
    0xFF, // Index = 0x50 ( page 3 )
    0xFF, // Index = 0x51 ( page 3 )
    0xFF, // Index = 0x52 ( page 3 )
    0xFF, // Index = 0x53 ( page 3 )
    0xFF, // Index = 0x54 ( page 3 )
    0xFF, // Index = 0x55 ( page 3 )
    0xFF, // Index = 0x56 ( page 3 )
    0xFF, // Index = 0x57 ( page 3 )
    0xFF, // Index = 0x58 ( page 3 )
    0xFF, // Index = 0x59 ( page 3 )
    0xFF, // Index = 0x5A ( page 3 )
    0xFF, // Index = 0x5B ( page 3 )
    0xFF, // Index = 0x5C ( page 3 )
    0xFF, // Index = 0x5D ( page 3 )
    0xFF, // Index = 0x5E ( page 3 )
    0xFF, // Index = 0x5F ( page 3 )
    0xFF, // Index = 0x60 ( page 3 )
    0xFF, // Index = 0x61 ( page 3 )
    0xFF, // Index = 0x62 ( page 3 )
    0xFF, // Index = 0x63 ( page 3 )
    0xFF, // Index = 0x64 ( page 3 )
    0xFF, // Index = 0x65 ( page 3 )
    0xFF, // Index = 0x66 ( page 3 )
    0xFF, // Index = 0x67 ( page 3 )
    0xFF, // Index = 0x68 ( page 3 )
    0xFF, // Index = 0x69 ( page 3 )
    0xFF, // Index = 0x6A ( page 3 )
    0xFF, // Index = 0x6B ( page 3 )
    0xFF, // Index = 0x6C ( page 3 )
    0xFF, // Index = 0x6D ( page 3 )
    0x00, // Index = 0x6E ( page 3 )
    0x00, // Index = 0x6F ( page 3 )
    0xFF, // Index = 0x70 ( page 3 )
    0xF8, // Index = 0x71 ( page 3 )
    0xFF, // Index = 0x72 ( page 3 )
    0x00, // Index = 0x73 ( page 3 )
    0x02, // Index = 0x74 ( page 3 )
    0xFF, // Index = 0x75 ( page 3 )
    0x00, // Index = 0x76 ( page 3 )
    0x00, // Index = 0x77 ( page 3 )
    0x00, // Index = 0x78 ( page 3 )
    0xFF, // Index = 0x79 ( page 3 )
    0xFF, // Index = 0x7A ( page 3 )
    0xFF, // Index = 0x7B ( page 3 )
    0xFF, // Index = 0x7C ( page 3 )
    0xFF, // Index = 0x7D ( page 3 )
    0xFF, // Index = 0x7E ( page 3 )
    0xFF, // Index = 0x7F ( page 3 )
  },

  //Page 4:
  {
    0x56, // Index = 0x00 ( page 4 )
    0xF0, // Index = 0x01 ( page 4 )
    0xF3, // Index = 0x02 ( page 4 )
    0x04, // Index = 0x03 ( page 4 )
    0x36, // Index = 0x04 ( page 4 )
    0x58, // Index = 0x05 ( page 4 )
    0xAC, // Index = 0x06 ( page 4 )
    0xFF, // Index = 0x07 ( page 4 )
    0x00, // Index = 0x08 ( page 4 )
    0x00, // Index = 0x09 ( page 4 )
    0x00, // Index = 0x0A ( page 4 )
    0x00, // Index = 0x0B ( page 4 )
    0x00, // Index = 0x0C ( page 4 )
    0x00, // Index = 0x0D ( page 4 )
    0xC0, // Index = 0x0E ( page 4 )
    0x00, // Index = 0x0F ( page 4 )
    0x00, // Index = 0x10 ( page 4 )
    0x00, // Index = 0x11 ( page 4 )
    0x00, // Index = 0x12 ( page 4 )
    0x00, // Index = 0x13 ( page 4 )
    0x00, // Index = 0x14 ( page 4 )
    0x00, // Index = 0x15 ( page 4 )
    0x00, // Index = 0x16 ( page 4 )
    0x00, // Index = 0x17 ( page 4 )
    0xFF, // Index = 0x18 ( page 4 )
    0xFF, // Index = 0x19 ( page 4 )
    0xFF, // Index = 0x1A ( page 4 )
    0xFF, // Index = 0x1B ( page 4 )
    0xFF, // Index = 0x1C ( page 4 )
    0xFF, // Index = 0x1D ( page 4 )
    0xFF, // Index = 0x1E ( page 4 )
    0xFF, // Index = 0x1F ( page 4 )
    0xFF, // Index = 0x20 ( page 4 )
    0xFF, // Index = 0x21 ( page 4 )
    0xFF, // Index = 0x22 ( page 4 )
    0xFF, // Index = 0x23 ( page 4 )
    0xFF, // Index = 0x24 ( page 4 )
    0xFF, // Index = 0x25 ( page 4 )
    0xFF, // Index = 0x26 ( page 4 )
    0xFF, // Index = 0x27 ( page 4 )
    0x00, // Index = 0x28 ( page 4 )
    0x00, // Index = 0x29 ( page 4 )
    0x4F, // Index = 0x2A ( page 4 )
    0x07, // Index = 0x2B ( page 4 )
    0x4F, // Index = 0x2C ( page 4 )
    0x07, // Index = 0x2D ( page 4 )
    0x3B, // Index = 0x2E ( page 4 )
    0x07, // Index = 0x2F ( page 4 )
    0x3B, // Index = 0x30 ( page 4 )
    0x07, // Index = 0x31 ( page 4 )
    0x50, // Index = 0x32 ( page 4 )
    0x00, // Index = 0x33 ( page 4 )
    0x50, // Index = 0x34 ( page 4 )
    0x00, // Index = 0x35 ( page 4 )
    0x10, // Index = 0x36 ( page 4 )
    0x00, // Index = 0x37 ( page 4 )
    0x00, // Index = 0x38 ( page 4 )
    0x10, // Index = 0x39 ( page 4 )
    0x00, // Index = 0x3A ( page 4 )
    0x00, // Index = 0x3B ( page 4 )
    0x00, // Index = 0x3C ( page 4 )
    0x00, // Index = 0x3D ( page 4 )
    0x00, // Index = 0x3E ( page 4 )
    0x00, // Index = 0x3F ( page 4 )
    0x00, // Index = 0x40 ( page 4 )
    0xFF, // Index = 0x41 ( page 4 )
    0xFF, // Index = 0x42 ( page 4 )
    0xFF, // Index = 0x43 ( page 4 )
    0xFF, // Index = 0x44 ( page 4 )
    0x00, // Index = 0x45 ( page 4 )
    0x03, // Index = 0x46 ( page 4 )
    0x00, // Index = 0x47 ( page 4 )
    0x00, // Index = 0x48 ( page 4 )
    0x00, // Index = 0x49 ( page 4 )
    0x24, // Index = 0x4A ( page 4 )
    0x00, // Index = 0x4B ( page 4 )
    0x40, // Index = 0x4C ( page 4 )
    0xFF, // Index = 0x4D ( page 4 )
    0xFF, // Index = 0x4E ( page 4 )
    0x7F, // Index = 0x4F ( page 4 )
    0x56, // Index = 0x50 ( page 4 )
    0x80, // Index = 0x51 ( page 4 )
    0x3F, // Index = 0x52 ( page 4 )
    0x00, // Index = 0x53 ( page 4 )
    0x84, // Index = 0x54 ( page 4 )
    0x60, // Index = 0x55 ( page 4 )
    0xAF, // Index = 0x56 ( page 4 )
    0x00, // Index = 0x57 ( page 4 )
    0x00, // Index = 0x58 ( page 4 )
    0x80, // Index = 0x59 ( page 4 )
    0x94, // Index = 0x5A ( page 4 )
    0x00, // Index = 0x5B ( page 4 )
    0x80, // Index = 0x5C ( page 4 )
    0x00, // Index = 0x5D ( page 4 )
    0x05, // Index = 0x5E ( page 4 )
    0x38, // Index = 0x5F ( page 4 )
    0x01, // Index = 0x60 ( page 4 )
    0x83, // Index = 0x61 ( page 4 )
    0x11, // Index = 0x62 ( page 4 )
    0x01, // Index = 0x63 ( page 4 )
    0x80, // Index = 0x64 ( page 4 )
    0x90, // Index = 0x65 ( page 4 )
    0x00, // Index = 0x66 ( page 4 )
    0x40, // Index = 0x67 ( page 4 )
    0x4A, // Index = 0x68 ( page 4 )
    0xE4, // Index = 0x69 ( page 4 )
    0x00, // Index = 0x6A ( page 4 )
    0x00, // Index = 0x6B ( page 4 )
    0x00, // Index = 0x6C ( page 4 )
    0x00, // Index = 0x6D ( page 4 )
    0x89, // Index = 0x6E ( page 4 )
    0x10, // Index = 0x6F ( page 4 )
    0xAC, // Index = 0x70 ( page 4 )
    0x00, // Index = 0x71 ( page 4 )
    0xE0, // Index = 0x72 ( page 4 )
    0x80, // Index = 0x73 ( page 4 )
    0x80, // Index = 0x74 ( page 4 )
    0x19, // Index = 0x75 ( page 4 )
    0x7D, // Index = 0x76 ( page 4 )
    0x00, // Index = 0x77 ( page 4 )
    0xFF, // Index = 0x78 ( page 4 )
    0xFF, // Index = 0x79 ( page 4 )
    0xFF, // Index = 0x7A ( page 4 )
    0xFF, // Index = 0x7B ( page 4 )
    0xFF, // Index = 0x7C ( page 4 )
    0xFF, // Index = 0x7D ( page 4 )
    0x00, // Index = 0x7E ( page 4 )
    0x0C, // Index = 0x7F ( page 4 )
  },
};

static unsigned char GenDefault(int p, int r)
{
  if ((p < 5) && (r < 128))
    return g_DefRegMap[p][r];
  return 0;
}


static int smbus_write_byte_data(int dev, int reg, int data)
{
  struct i2c_smbus_ioctl_data iodata;
  union i2c_smbus_data value;
  int res;

  if (dev < 0) {
    printf("WR_%d {0x%02x, 0x%02x} -> %d\n", ch7036_cur_page, reg, data, 0);
    return 0;
  }

  value.byte = data;
  iodata.read_write = I2C_SMBUS_WRITE;
  iodata.command = reg;
  iodata.size = I2C_SMBUS_BYTE_DATA;
  iodata.data = &value;
  res = ioctl(dev, I2C_SMBUS, &iodata);
  if (res < 0)
    fprintf(stderr, "Write byte fail (%d, %02x, %02x): %s",
	    dev, reg, data, strerror(errno));
  if (ch7036_debugi2c)
    printf("WR_%d {0x%02x, 0x%02x} -> %d\n", ch7036_cur_page, reg, data, res);
  return res;
}

static int smbus_write_word_data(int dev, int reg, int data)
{
  struct i2c_smbus_ioctl_data iodata;
  union i2c_smbus_data value;
  int res;

  if (dev < 0) {
    printf("WR_%d {0x%02x, 0x%04x} -> %d\n", ch7036_cur_page, reg, data, 0);
    return 0;
  }

  value.word = data;
  iodata.read_write = I2C_SMBUS_WRITE;
  iodata.command = reg;
  iodata.size = I2C_SMBUS_WORD_DATA;
  iodata.data = &value;
  res = ioctl(dev, I2C_SMBUS, &iodata);
  if (res < 0)
    fprintf(stderr, "Write word fail (%d, %04x, %02x): %s",
	    dev, reg, data, strerror(errno));
  if (ch7036_debugi2c)
    printf("WR_%d {0x%02x, 0x%04x} -> %d\n", ch7036_cur_page, reg, data, res);
  return res;
}

#ifdef FASTLOAD
static int smbus_write_reg_bytes(int dev, int reg, int size, 
				 unsigned char *data)
{
  struct i2c_smbus_ioctl_data iodata;
  union i2c_smbus_data value;
  int res;

  if (size > I2C_SMBUS_BLOCK_MAX) {
    fprintf(stderr, "Bad call to smbus_write_reg_bytes with size %d\n", size);
    return -1;
  }

  if (dev < 0) {
    printf("WR_%d {0x%02x, 0x%02x,...[%d]} -> %d\n", 
	   ch7036_cur_page, reg, data[0], size, 0);
    return 0;
  }

  memcpy(&value.block[1], data, size);
  value.block[0] = size;
  iodata.read_write = I2C_SMBUS_WRITE;
  iodata.command = reg;
  iodata.size = I2C_SMBUS_I2C_BLOCK_DATA;
  iodata.data = &value;
  res = ioctl(dev, I2C_SMBUS, &iodata);
  if (res < 0)
    fprintf(stderr, "Write bytes fail (%d, %02x, %02x,...[%d]): %s\n",
	    dev, reg, data[0], size, strerror(errno));
  if (ch7036_debugi2c)
    printf("WR_%d {0x%02x, 0x%02x,...[%d]} -> %d\n", 
	   ch7036_cur_page, reg, data[0], size, res);
  return res;
}
#endif

static int smbus_read_byte_data(int dev, int reg)
{
  struct i2c_smbus_ioctl_data iodata;
  union i2c_smbus_data value;

  if (dev < 0) {
    printf("RD_%d {0x%02x} -> 0x%02x\n", ch7036_cur_page, reg, 0);
    return 0;
  }

  iodata.read_write = I2C_SMBUS_READ;
  iodata.command = reg;
  iodata.size = I2C_SMBUS_BYTE_DATA;
  iodata.data = &value;
  if (ioctl(dev, I2C_SMBUS, &iodata) < 0) {
    fprintf(stderr, "Read byte fail (%d, %02x): %s",
	    dev, reg, strerror(errno));
    return -1;
  }
  if (ch7036_debugi2c)
    printf("RD_%d {0x%02x} -> 0x%02x\n", ch7036_cur_page, reg, value.byte);
  return value.byte;
}


int ch_write_reg(int dev, int page, int reg, int data)
{
  int res;

  if (((page < 1) || (page > 4)) && (reg != CH7036_PAGE))
    return -1;

  if ((page != ch7036_cur_page) && (reg != CH7036_PAGE)) {
    if (ch_write_reg(dev, ch7036_cur_page, CH7036_PAGE, ch_page_map[page]) < 0)
      return -1;
  }

  res = smbus_write_byte_data(dev, reg, data);

  if (res < 0)
    return -1;

  if (reg == CH7036_PAGE) {
    ch7036_cur_page = ch_page_revmap[data & 7];
  }
  return 0;
}

int ch_read_reg(int dev, int page, int reg)
{
  int res;

  if ((page < 1) || (page > 4))
    return -1;

  if (page != ch7036_cur_page) {
    if (ch_write_reg(dev, ch7036_cur_page, CH7036_PAGE, ch_page_map[page]) < 0)
      return -1;
  }
  res =  smbus_read_byte_data(dev, reg);

  return res;
}

int ch_write_reg_sequence (int dev, struct reg_ch7036* seq)
{
  int i, res;
  int n_regs = N_REGS_IN_SEQ(seq);

  for (i=1; i <= n_regs; i++) {
    res = smbus_write_byte_data(dev, seq[i].index, seq[i].value);
    changed[ch_page_map[ch7036_cur_page]][seq[i].index] = 1;

    if (res < 0)
      return -1;
    if (seq[i].index == CH7036_PAGE) 
      ch7036_cur_page = ch_page_revmap[seq[i].value & 7];
  }
  return 0;
}

int ch_restore_notin_seq (int dev, struct reg_ch7036* seq, int verbose)
{
  int i, j;
  int n_regs = N_REGS_IN_SEQ(seq);
  int loc_cp = ch7036_cur_page;
  int err;
  int nwritten = 0;

  /* Clear the changed vector for anything we are about to write */
  for (i=1; i <= n_regs; i++) {
    changed[ch_page_map[loc_cp]][seq[i].index] = 0;
    if (seq[i].index == CH7036_PAGE) 
      loc_cp = ch_page_revmap[seq[i].value & 7];
  }
  
  for(j=0; j < 5; j++)
    for(i=0; i < 128; i++) if (changed[j][i]) {
	if (i == CH7036_PAGE) continue;
	err = ch_write_reg(dev, ch_page_revmap[j], i, GenDefault(j, i));
	if (err != 0) return err;
        nwritten++;
      }
  if (verbose)
    printf("... Restored %d registers\n", nwritten);
  return 0;
}

int ch_change_reg(struct reg_ch7036* seq, int page, int reg, 
                  int andv, int orv)
{
  int i;
  int n_regs = N_REGS_IN_SEQ(seq);
  int cp = -1;
  int last_found = -1;

  /* Some registers are in the list multiple times */
  /* Need to modify the last one to have an effect */
  /* But must parse forward to follow page changes */
  for (i=1; i <= n_regs; i++) {
    if ((cp == page) && (seq[i].index == reg))
      last_found = i;
    if (seq[i].index == CH7036_PAGE) 
      cp = ch_page_revmap[seq[i].value & 7];
  }
  if (last_found > 0) {
      int ov = seq[last_found].value;
      seq[last_found].value = (ov & andv) | orv;
      return ov;
  }
  return -1;
}


void ch_reset_datapath (int dev)
{
  /* Reset the datapath then remove reset */
  ch_write_reg(dev, CH7036_RESET, 0x2e);
  ch_write_reg(dev, CH7036_RESET, 0x2f);
}

/* Email from Chrontel 5/20 to reset to defaults */
/* Note: will need to redo step1 after this! */
void ch_reset_todefault (int dev)
{
  int i;
  /* Reset registers then remove reset */
  ch_write_reg(dev, CH7036_RESET, 0x2d);
  ch_write_reg(dev, CH7036_RESET, 0x2f);

  for(i=0; i < 256; i++) 
    changed[0][i] = changed[1][i] = changed[2][i] = 
      changed[3][i] = changed[4][i] = 0;

}

/* Turn the monitor on */
void ch_monitor_on(int dev)
{
  ch_write_reg(dev, CH7036_PWRST2, 0xD0); // HDMI driver, SDRAM. SPDIF on
  ch_write_reg(dev, CH7036_PWRST3, 0x0F); // VGA dac powered down
  ch_write_reg(dev, CH7036_PWRST4, 0x02); // SDRAM, HDMI power on, RGB off

  ch_write_reg(dev, CH7036_MAGIC2_0E, 0x13); // MAGIC
  ch_write_reg(dev, CH7036_SDTIME1, 0x20); // HDMI driver, PLL power on
  
  ch_write_reg(dev, CH7036_PWRSTATE1, 0x84); // Normal (pll on)
  ch_reset_datapath(dev);
}

/* Turn the monitor off */
void ch_monitor_off(int dev)
{
  ch_write_reg(dev, CH7036_PWRST2, 0xDF); // HDMI driver, SDRAM. SPDIF off
  ch_write_reg(dev, CH7036_PWRST3, 0xFF); // VGA dac powered down
  ch_write_reg(dev, CH7036_PWRST4, 0x7F); // Stop hdmi,scaler, pwr down

  ch_write_reg(dev, CH7036_MAGIC2_0E, 0x93); // MAGIC
  ch_write_reg(dev, CH7036_SDTIME1, 0x29); // HDMI driver, PLL power off
  
  ch_write_reg(dev, CH7036_PWRSTATE1, 0x94); // pll power down
}

/* Turn the monitor off but keep DDC running so EDID can still be read */
void ch_monitor_off_keep_ddc(int dev)
{
  ch_write_reg(dev, CH7036_PWRST2, 0xDF); // HDMI driver, SDRAM. SPDIF off
  ch_write_reg(dev, CH7036_PWRST3, 0xFF); // VGA dac powered down
  ch_write_reg(dev, CH7036_PWRST4, 0x7F); // Stop hdmi,scaler, pwr down

  /* Chrontel email response 2010-06-17 to Off preventing EDID reads */
  /* Please remove both  0xE and 0x16 registers */
  /* 0xE will power down DDC */
  
  ch_write_reg(dev, CH7036_PWRSTATE1, 0x94); // pll power down
}

int ch_hdmi_is_on(int dev)
{
  int res;
  res = ch_read_reg(dev, CH7036_PWRST2);
  if (res < 0) {
    fprintf(stderr, "Failed to read powerstate 2 to check hdmi on\n");
    return 0;
  }
  /* Check if HDMI driver is powered down (bit set) */
  return (res & 0x8) ? 0 : 1;
}

/* Calculate INC registers from values the chip provides */

void ch_calculate_incs(int dev)
{
  unsigned int hinca, hincb, hinc;
  unsigned int vinca, vincb, vinc;
  unsigned int hdinca, hdincb, hdinc;

  //first read out parameters:

  hinca = ((ch_read_reg(dev, CH7036_HINCA1) << 3) | 
	   (ch_read_reg(dev, CH7036_HINCA2) & 0x7));
  hincb = ((ch_read_reg(dev, CH7036_HINCB1) << 3) | 
	   (ch_read_reg(dev, CH7036_HINCB2) & 0x7));
  vinca = ((ch_read_reg(dev, CH7036_VINCA1) << 3) | 
	   (ch_read_reg(dev, CH7036_VINCA2) & 0x7));
  vincb = ((ch_read_reg(dev, CH7036_VINCB1) << 3) | 
	   (ch_read_reg(dev, CH7036_VINCB2) & 0x7));
  hdinca = ((ch_read_reg(dev, CH7036_HDINCA1) << 3) | 
	    (ch_read_reg(dev, CH7036_HDINCA2) & 0x7));
  hdincb = ((ch_read_reg(dev, CH7036_HDINCB1) << 3) | 
	    (ch_read_reg(dev, CH7036_HDINCB2) & 0x7));

  /* calculate them */
  /* NOTE: this code works because the values are 11-bit */
  /* so the shift in a 32-bit uint will not drop any bits from the top */
  if (hinca == 0 || hincb == 0) {
    hinc = 0;
  } else {
    hinc = (1 << 20) * hinca / hincb;
  }

  if (vinca == 0 || vincb == 0) {
    vinc = 0;
  } else {
    vinc = (1 << 20) * vinca / vincb;
  }

  if (hdinca == 0 || hdincb == 0) {
    hdinc = 0;
  } else {
    hdinc = (1 << 20) * hdinca / hdincb;
  }

  ch_write_reg(dev, CH7036_HINCC1,(hinc>>16) & 0xFF);
  ch_write_reg(dev, CH7036_HINCC2,(hinc>>8)  & 0xFF);
  ch_write_reg(dev, CH7036_HINCC3, hinc      & 0xFF);

  ch_write_reg(dev, CH7036_VINCC1,(vinc>>16) & 0xFF);
  ch_write_reg(dev, CH7036_VINCC2,(vinc>>8)  & 0xFF);
  ch_write_reg(dev, CH7036_VINCC3, vinc      & 0xFF);

  ch_write_reg(dev, CH7036_HDINCC1,(hdinc>>16) & 0xFF);
  ch_write_reg(dev, CH7036_HDINCC2,(hdinc>>8)  & 0xFF);
  ch_write_reg(dev, CH7036_HDINCC3, hdinc      & 0xFF);
}



/* Load Firmware */

int ch_load_firmware(int dev, char* fname)
{
  unsigned char buf[10240];
  FILE* fp;
  long fs, fs1;
  int res = -1;

  do {

    fp = fopen(fname, "rb");
    if (fp==NULL) {
      fprintf(stderr, "Could not open %s to read firmware\n", fname);
      break;
    }
    fseek( fp, 0, SEEK_END);
    fs = ftell(fp);
    fseek( fp, 0, SEEK_SET);

    fs1 = fread( buf, 1, fs, fp);
    if (fs1 != fs) {
      printf("File %s reading error!\n", fname);
      break;
    }

    printf("Firmware size = %d bytes.\n", (int)fs);
    /* Initialize the firmware download */
    ch_write_reg(dev, 4, 0x52, 0x2b);
    ch_write_reg(dev, 4, 0x5b, 0x9e);
    ch_write_reg(dev, 4, 0x5b, 0xb3);
    smbus_write_byte_data(dev, 0x03, 0x07);
    /*  Download the firmware */
    /*  "The downloading can be byte by byte, or using I2C block write" */
    /* FIXME mdhayter: Did not get the block write to work with count > 1 */
    /* The SMBus hangs completely using I2C_SMBUS_I2C_BLOCK which is the  */
    /* version that does not send the length required by Chrontel (unless */
    /* the length is set to 1). The transfers complete ok using the */
    /* I2C_SMBUS_BLOCK version, but the firmware is no good (as expected) */
    /* Could it be the kernel driver? */
    /* Using word seems ok and halves the number of transactions */
#ifdef SLOWLOAD
    for (fs1=0; fs1<fs; fs1++) 
      smbus_write_byte_data(dev, 0x07, buf[fs1]);
#endif

#ifndef FASTLOAD
    for(fs1 = 0; (fs1+1) < fs; fs1+=2)
      smbus_write_word_data(dev, 0x07, buf[fs1] | (buf[fs1+1]<<8));
    if (fs1 < fs)
      smbus_write_byte_data(dev, 0x07, buf[fs1]);
#else
#define TXSIZE 32
    for(fs1 = 0; (fs1+TXSIZE) < fs; fs1+=TXSIZE)
      smbus_write_reg_bytes(dev, 0x07, TXSIZE, buf+fs1);
    if (fs1 < fs)
      smbus_write_reg_bytes(dev, 0x07, fs-fs1, buf+fs1);
#endif

    /*  Finishing the download and run the firmware */
    smbus_write_byte_data(dev, 0x03, 0x04);
    smbus_write_byte_data(dev, 0x52, 0x2F);
    smbus_write_byte_data(dev, 0x03, 0x00);
    ch7036_cur_page = -1;
    res = 0;

  } while(0);

  if (fp!=NULL) fclose(fp);
  return res;
}


/* Note -- this only works if firmware is loaded */
/* will time out if the firmware is not running  */
int ch_mcu_version(int dev, int newFirmware, int verbose)
{
  unsigned char data[16];
  FW7036_CFG *config = (FW7036_CFG *) &data[0];
  struct timeval tv;
  /* This timeout needs to be tuned. 500,000 works but is a bit */
  /* long when trying to detect the firmware going away         */
  /* 100,000 seems to sometimes give false timeouts             */
  /* 200,000 is good except when the firmware is restarting     */
  struct timeval tout = {0, 200000};
  if (newFirmware) tout.tv_usec = 500000;

  ch7036_cur_page = -1;

  // Send Host Request
  smbus_write_byte_data(dev, 0x03, 0x00);
  smbus_write_byte_data(dev, 0x4F, 0x5F);
  // Wait the MCU response
  gettimeofday(&tv, NULL);
  timeradd(&tv, &tout, &tout);

  while (1) {
    smbus_write_byte_data(dev, 0x03, 0x00);
    if (((0x7F & smbus_read_byte_data(dev, 0x4F))==0x1F) || (dev < 0)) {
      int i;
      smbus_write_byte_data(dev, 0x03, MCU_DATA_PAGE);
      for (i=0; i<sizeof(FW7036_CFG); i++) {
        // data[16] holds the return parameter block from MCU
        data[i] = smbus_read_byte_data(dev, es_map[i]);
      }
      smbus_write_byte_data(dev, 0x03, 0x00);
      break;
    }
    gettimeofday(&tv, NULL);
    if (timercmp(&tv, &tout, >)) {
      fprintf(stderr, "Timeout waiting for MCU version\n");
      return -1;
    }
  }
  if (verbose)
    printf("CH7036 MCU ver %d.%d, Device %02x rev %d, capability 0x%x\n",
	   config->ver_major, config->ver_minor,
	   config->did, config->rid, config->capbility);
  return (config->ver_major<<8) | config->ver_minor;
}

/* Note -- this only works if firmware is loaded */
/* Always says hdmi detected if firmware is not loaded */
int ch_hdmi_detected(int dev)
{
  int res;
  res = ch_read_reg(dev, CH7036_HDMI_ST);
  if (res < 0) return res;

  res = (res & 0x10) ? 1 : 0;
  return res;
}

/* Note -- this only works if firmware is loaded */
/* will time out if the firmware is not running  */
int ch_get_edid(int dev, int edid_block, unsigned char* edid)
{
  int j;
  int res;
  struct timeval tv;
  struct timeval tout = {0, 500000};
  ch7036_cur_page = -1;

  // edid_block
  for (j=0; j<8; j++) {
    // Send Host Request
    res = smbus_write_byte_data(dev, 0x03, 0x00);
    if (res < 0) return res;
    // one edid block contains 128 bytes, so we need read 8 blocks
    res = smbus_write_byte_data(dev, 0x50, ((edid_block*8)+j));
    if (res < 0) return res;
    res = smbus_write_byte_data(dev, 0x4F, 0x41);
    if (res < 0) return res;
    // Wait the MCU response
    gettimeofday(&tv, NULL);
    timeradd(&tv, &tout, &tout);
    while (1) {
      res = smbus_write_byte_data(dev, 0x03, 0x00);
      if (res < 0) return res;
      // Order in dummy case should do one read...
      if (((0x7F & smbus_read_byte_data(dev, 0x4F))==0x01) || (dev < 0)) {
        int i;
        smbus_write_byte_data(dev, 0x03, MCU_DATA_PAGE);
        for (i=0; i<16; i++) {
          edid[j*16 + i] = smbus_read_byte_data(dev, es_map[i] );
        }
        smbus_write_byte_data(dev, 0x03, 0x00);
        break;
      }
      gettimeofday(&tv, NULL);
      if (timercmp(&tv, &tout, >)) {
	fprintf(stderr, "Timeout waiting for MCU to return EDID\n");
	return -1;
      }
    }
  }
  return 0;
}


