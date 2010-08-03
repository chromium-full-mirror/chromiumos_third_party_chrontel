// Copyright (c) 2010 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __EDID_UTILS_H__
#define __EDID_UTILS_H__

/* These match the EDID encoding for Standard Timing block */
#define ASPECT_16_10 0
#define ASPECT_4_3   1
#define ASPECT_5_4   2
#define ASPECT_16_9  3
#define N_ASPECTS    4

int edid_valid(unsigned char *edid_data);
int edid_has_hdmi_info(unsigned char *edid_data, int ext);
void show_edid_data(FILE *outfile, unsigned char *edid_data, 
                    int items, int base);
void show_edid(FILE *outfile, unsigned char *edid_data, int ext);
int find_aspect(int h, int v);
int find_aspect_fromisize(unsigned char *edid_data);
extern char *aspect_to_str[];

#endif
