// Copyright (c) 2010 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "edid_utils.h"

/* Dump out an EDID block in a simple format */
void show_edid_data(unsigned char *edid_data, int items, int base)
{
  int item = 0;

  while (item < items) {
    int i;
    printf(" 0x%04x:  ", item + base);
    for(i=0; i < 16; i++) {
      printf("%02x ", edid_data[item++]);
      if (item >= items) break;
    }
    printf("\n");
  }
}


unsigned char test_edid1[128] = {
  0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,
  0x06,0xaf,0x5c,0x20,0x00,0x00,0x00,0x00,
  0x01,0x12,0x01,0x03,0x80,0x1a,0x0e,0x78,
  0x0a,0x99,0x85,0x95,0x55,0x56,0x92,0x28,
  0x22,0x50,0x54,0x00,0x00,0x00,0x01,0x01,
  0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
  0x01,0x01,0x01,0x01,0x01,0x01,0x96,0x19,
  0x56,0x28,0x50,0x00,0x08,0x30,0x18,0x10,
  0x24,0x00,0x00,0x90,0x10,0x00,0x00,0x18,
  0x00,0x00,0x00,0x0f,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x20,0x00,0x00,0x00,0xfe,0x00,0x41,
  0x55,0x4f,0x0a,0x20,0x20,0x20,0x20,0x20,
  0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xfe,
  0x00,0x42,0x31,0x31,0x36,0x58,0x57,0x30,
  0x32,0x20,0x56,0x30,0x20,0x0a,0x00,0xf8
};

unsigned char test_edid2[128] = {
  0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,
  0x30,0xe4,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x14,0x01,0x03,0x80,0x1a,0x0e,0x78,
  0x0a,0xbf,0x45,0x95,0x58,0x52,0x8a,0x28,
  0x25,0x50,0x54,0x00,0x00,0x00,0x01,0x01,
  0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
  0x01,0x01,0x01,0x01,0x01,0x01,0x84,0x1c,
  0x56,0xa8,0x50,0x00,0x19,0x30,0x30,0x20,
  0x35,0x00,0x00,0x90,0x10,0x00,0x00,0x1b,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0xfe,0x00,0x4c,
  0x47,0x20,0x44,0x69,0x73,0x70,0x6c,0x61,
  0x79,0x0a,0x20,0x20,0x00,0x00,0x00,0xfc,
  0x00,0x4c,0x50,0x31,0x31,0x36,0x57,0x48,
  0x31,0x2d,0x54,0x4c,0x4e,0x31,0x00,0x4e
};

/* Print out an EDID */
void show_edid(unsigned char *edid_data, int ext)
{
  int i;
  int edidver = edid_data[18];
  int edidrev = edid_data[19];
  if (!((edid_data[0]==0x00) && (edid_data[1]==0xff) && (edid_data[2]==0xff) && 
	(edid_data[3]==0xff) && (edid_data[4]==0xff) && (edid_data[5]==0xff) && 
	(edid_data[6]==0xff) && (edid_data[7]==0x00))) {
    printf("Block does not contain EDID header\n");
    return;
  }
  /* unsigned edid_data so the right shifts pull in zeros */
  printf("Manufacturer ID %c%c%c, product ID 0x%x\n", 
	 '@' + (edid_data[8]>>2), 
	 '@' + ((edid_data[8] & 3)<<3) + (edid_data[9]>>5),
	 '@' + (edid_data[9] & 0x1f),
	 edid_data[10] + (edid_data[11]<<8));
  printf("Manufactured wk %d of %d. Edid version %d.%d\n",
	 edid_data[16], 1990+edid_data[17], edidver, edidrev);
  printf("Input: %s, vid level %d, %s, %s %s %s %s sync, %dx%dcm, Gamma %f\n",
	 (edid_data[20] & 0x80) ? "digital" : "analog",
	 (edid_data[20]>>5) & 3,
	 (edid_data[20] * 0x10) ? "Blank to black" : "",
	 (edid_data[20] * 0x08) ? "Separate" : "",
	 (edid_data[20] * 0x04) ? "Composite" : "",
	 (edid_data[20] * 0x02) ? "On-green" : "",
	 (edid_data[20] * 0x01) ? "Serration V" : "",
	 edid_data[21], edid_data[22], 1.0+((float)edid_data[23]/100.0));
  printf("Features: %s %s %s %s %s %s %s\n",
	 (edid_data[24] & 0x80) ? "standby" : "",
	 (edid_data[24] & 0x40) ? "suspend" : "",
	 (edid_data[24] & 0x20) ? "active-off" : "",
	 (edid_data[24] & 0x18) ? "colour" : "monochrome",
	 (edid_data[24] & 0x04) ? "std-cspace" : "non-std-cspace",
	 (edid_data[24] & 0x02) ? "preferred-timing" : "",
	 (edid_data[24] & 0x02) ? "default-GTF" : "");
  
  printf("Established Timing:\n");
  if (edid_data[35] & 0x80) printf("720x400@70\n");
  if (edid_data[35] & 0x40) printf("720x400@88\n");
  if (edid_data[35] & 0x20) printf("640x480@60\n");
  if (edid_data[35] & 0x10) printf("640x480@67\n");
  if (edid_data[35] & 0x08) printf("640x480@72\n");
  if (edid_data[35] & 0x04) printf("640x480@75\n");
  if (edid_data[35] & 0x02) printf("800x600@56\n");
  if (edid_data[35] & 0x01) printf("800x600@60\n");
  if (edid_data[36] & 0x80) printf("800x600@72\n");
  if (edid_data[36] & 0x40) printf("800x600@75\n");
  if (edid_data[36] & 0x20) printf("832x624@75\n");
  if (edid_data[36] & 0x10) printf("1024x768i@87\n");
  if (edid_data[36] & 0x08) printf("1024x768@60\n");
  if (edid_data[36] & 0x04) printf("1024x768@70\n");
  if (edid_data[36] & 0x02) printf("1024x768@75\n");
  if (edid_data[36] & 0x01) printf("1280x1024@75\n");
  if (edid_data[36] & 0x80) printf("1152x870@75\n");
  
  printf("Standard timing:\n");
  for(i=0; i < 8; i++) {
    int hinfo = edid_data[38+2*i];
    int vinfo = edid_data[39+2*i];
    int hres, vres;

    /* 01 01 is pad by spec, but 00 00 and 20 20 are see in wild */
    if (((hinfo == 0x01) && (vinfo == 0x01)) ||
	((hinfo == 0x00) && (vinfo == 0x00)) ||
	((hinfo == 0x20) && (vinfo == 0x20)))
      continue;
    hres = (hinfo * 8) + 248;
    switch (vinfo >> 6) {
    case ASPECT_16_10:
      vres = (hres * 10)/16;
      break;
    case ASPECT_4_3:
      vres = (hres * 3)/4;
      break;
    case ASPECT_5_4:
      vres = (hres * 4)/5;
      break;
    case ASPECT_16_9:
      vres = (hres * 9)/16;
      break;
      /* Default will only be hit if compiler broken */
    default:
      vres = 0;
    }
    printf("%d: %dx%d@%d\n", i, hres, vres, 60 + (vinfo & 0x3f));
  }
  
  printf("Descriptor blocks:\n");
  for(i = 0; i < 4; i++) {
    int pelclk = edid_data[54 + 18*i] + (edid_data[55 + 18*i]<<8);
    
    if (pelclk != 0) {
      int hres = edid_data[56 + 18*i] + ((edid_data[58 + 18*i] & 0xf0)<<4);
      int hbl = edid_data[57 + 18*i] + ((edid_data[58 + 18*i] & 0x0f)<<8);
      int vres = edid_data[59 + 18*i] + ((edid_data[61 + 18*i] & 0xf0)<<4);
      int vbl = edid_data[60 + 18*i] + ((edid_data[61 + 18*i] & 0x0f)<<8);
      int hso = edid_data[62 + 18*i] + ((edid_data[65 + 18*i] & 0xc0)<<2);
      int hsw = edid_data[63 + 18*i] + ((edid_data[65 + 18*i] & 0x30)<<4);
      int vso = (edid_data[64 + 18*i] >> 4) + 
	        ((edid_data[65 + 18*i] & 0x0c)<<2);
      int vsw = (edid_data[64 + 18*i] & 0xf) + 
                ((edid_data[65 + 18*i] & 0x03)<<4);
      int hsiz = edid_data[66 + 18*i] + ((edid_data[68 + 18*i] & 0xf0)<<4);
      int vsiz = edid_data[67 + 18*i] + ((edid_data[68 + 18*i] & 0x0f)<<8);
      int hbdr = edid_data[69 + 18*i];
      int vbdr = edid_data[70 + 18*i];
      int mdflg = edid_data[71 + 18*i];

      int refr = (pelclk * 10000)/((hres+hbl)*(vres+vbl));
      int refm = (pelclk * 10000)%((hres+hbl)*(vres+vbl));
      int refd = (refm*100)/((hres+hbl)*(vres+vbl));

      printf("%dx%d%c@%d.%02d, dot clock %d  %cHsync %cVsync\n", 
	     hres, vres, (mdflg & 0x80) ? 'i':'p', 
	     refr, refd,
	     pelclk * 10000,
	     (mdflg & 0x2) ? '+':'-',
	     (mdflg & 0x4) ? '+':'-');
      printf("H: start %d, end %d, total %d\n",
	     hres+hso, hres+hso+hsw, hres+hbl);
      printf("V: start %d, end %d, total %d\n",
	     vres+vso, vres+vso+vsw, vres+vbl);
      printf("Size %dx%dmm, Border %dx%d pixels\n", hsiz, vsiz, hbdr, vbdr);
    } else { 
      char monstr[18];
      int stp;
      switch (edid_data[57 + 18*i]) {
	
      case 0xff:
      case 0xfe:
      case 0xfc:
	strncpy(monstr, (char *)edid_data+59+18*i, 13);
	for(stp = 0; stp < 13; stp++) if (monstr[stp] == 0x0a) monstr[stp] = 0;
	monstr[stp] = 0;
	if (edid_data[57 + 18*i] != 0xfe)
	  printf("%s: %s\n", 
		 (edid_data[57 + 18*i] == 0xfc) ? "Name" : "Serial", monstr);
	else
	  printf("%s\n", monstr);
	break;

      case 0xfd:
	printf("Range V %d - %d Hz, H %d - %d kHz, Pel <= %d MHz\n",
	       edid_data[59 + 18*i], edid_data[60 + 18*i], 
	       edid_data[61 + 18*i], edid_data[62 + 18*i], 
	       edid_data[63 + 18*i]*10);
	break;

      default:
	printf("Undecoded descriptor block type 0x%x\n", edid_data[57 + 18*i]);
	break;
      }
    }
  }
}


/* Pixel counts normally round to 8 */
#define CLOSE_ENOUGH(a, b) (abs((a)-(b)) < 16)

char *aspect_to_str[]={"16:10","4:3","5:4","16:9"};

int find_aspect(int h, int v) 
{
  if (CLOSE_ENOUGH((h * 3), (v * 4)))
    return ASPECT_4_3;
  if (CLOSE_ENOUGH((h * 4), (v * 5)))
    return ASPECT_5_4;
  if (CLOSE_ENOUGH((h * 9), (v * 16)))
    return ASPECT_16_9;
  if (CLOSE_ENOUGH((h * 10), (v * 16)))
    return ASPECT_16_10;

  return -1;
}

int find_aspect_fromisize(unsigned char *edid_data)
{
  int hsiz = edid_data[21];
  int vsiz = edid_data[22];
  int res;

  /* Zero size for projector */
  /* Only use this code if there was no preferred resolution */
  /* So assume it is an older 4:3 projector not a video one  */
  if ((hsiz == 0) && (vsiz == 0))
    return ASPECT_4_3;

  res = find_aspect(hsiz, vsiz);

  /* If things didn't work out, assume the old 4:3 case */
  if (res < 0)
    return ASPECT_4_3;
  else
    return res;
}

