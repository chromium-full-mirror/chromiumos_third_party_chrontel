// Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/Xproto.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/dpms.h>

#if RANDR_MAJOR > 1 || (RANDR_MAJOR == 1 && RANDR_MINOR >= 2)
#define HAS_RANDR_1_2 1
#else
#error Only tested with xrandr 1.2
#endif

#include "audio_utils.h"
#include "ch7036.h"
#include "edid_utils.h"
#include "xrr_utils.h"

#define MAX_EDID_EXT 4

#define DEF_DEV "/dev/i2c-2"
#define DEF_FW "/lib/firmware/chrontel/fw7036.bin"
#define EDID_LOG "/var/log/hdmi_edid.log"

/* Test flags used for experiments */
int test_flags = 0;

#define CHTEST_FORCEDETECT 0x01

/* These are defined for convenience during development */
/* to allow experiments to be patched in and tested across monitors */
/* They should not be used in final code pushed to mainline tree */
#define CHTEST_EXPERIMENT1 0x100
#define CHTEST_EXPERIMENT2 0x200


/* Internal error codes */
#define ERR_FAILED -1
#define ERR_NEED_RELOAD -2
#define ERR_NEWFW_RETRY -3
#define ERR_READ_FAILED -4
#define ERR_EXTRD_FAILED -5

/* Width fixups for supported displays */
int width_fix_1366x768[4] = {1366, 1024, 960, 1366};
int width_fix_1280x800[4] = {1280, 1066, 1000, 1280};

/* Known timings (from tbl7036.c) */
static struct timing_ch7036 known_modes[] = {
  {
    /* Put this first in table and use as default */
    25200,
    MK_PIXEL_HDMI(0, 1, 1),
    0, 0, 1,
    800, 640, 16,  96,  525, 480, 10,  2 ,
    MK_SCALE(0, 0),
  },
  {
    27027,
    MK_PIXEL_HDMI(0, 2, 1),
    0,  0,  1,
    858, 720, 16,  62,  525, 480, 9,  6 ,
    MK_SCALE(0, 0),
  },
  {
    74250,
    MK_PIXEL_HDMI(0, 4, 2),
    1,  1,  1,
    1650, 1280, 110, 40,  750, 720, 5,  5 ,
    MK_SCALE(0, 0),
  },
  {
    148500,
    MK_PIXEL_HDMI(0, 16, 2),  // dvi, fmt, aspect
    1,  1,  1,       // hpo, vpo, depo
    2200, 1920, 88,  44,  1125, 1080, 4,  5,
    MK_SCALE(0, 0),
  },
  {
    // Interlaced, put second so it won't be picked on res alone
    74250,
    MK_PIXEL_HDMI(0, 5, 2),  // dvi, fmt, aspect
    1,  1,  1,       // hpo, vpo, depo
    2200, 1920, 88,  44,  1125, 1080, 4,  5 ,
    MK_SCALE(0, 0),
  },
  {
    27000,
    MK_PIXEL_HDMI(0, 17, 1),
    0,  0,  1,
    864, 720, 12,  64,  625, 576, 5,  5,
    MK_SCALE(0, 0),
  },
  {
    74250,
    MK_PIXEL_HDMI(0, 19, 2),
    1,  1,  1,
    1980, 1280, 440, 40,  750, 720, 5,  5,
    MK_SCALE(0, 0),
  },
  {
    148500,
    MK_PIXEL_HDMI(0, 31, 2),  // dvi, fmt, aspect
    1,  1,  1,       // hpo, vpo, depo
    2640, 1920, 528, 44,  1125, 1080, 4,  5,
    MK_SCALE(0, 0),
  },
  {
    // Interlaced, put second
    74250,
    MK_PIXEL_HDMI(0, 20, 2),  // dvi, fmt, aspect
    1,  1,  1,       // hpo, vpo, depo
    2640, 1920, 528, 44,  1125, 1080, 4,  5,
    MK_SCALE(0, 0),
  },

  //////  DVI modes
  {
    25200,
    MK_PIXEL_HDMI(1, 0, 0),
    1, 1, 1,
    800, 640, 16,  96,  525, 480, 10,  2 ,
    MK_SCALE(0, 0),
  },
  {
    27027,
    MK_PIXEL_HDMI(1, 0, 0),
    1,  1,  1,
    858, 720, 16,  62,  525, 480, 9,  6 ,
    MK_SCALE(0, 0),
  },
  {
    27000,
    MK_PIXEL_HDMI(1, 0, 0),
    1,  1,  1,
    864, 720, 12,  64,  625, 576, 5,  5,
    MK_SCALE(0, 0),
  },
  {
    40000,
    MK_PIXEL_HDMI(1, 0, 0),
    1,  1,  1,
    1056, 800, 40,  128, 628, 600, 1,  4,
    MK_SCALE(0, 0),
  },
  {
    65000,
    MK_PIXEL_HDMI(1, 0, 0),
    1,  1,  1,
    1344, 1024, 24,  136, 806, 768, 3,  6,
    MK_SCALE(0, 0),
  },
  {
    74250,
    MK_PIXEL_HDMI(1, 0, 0),
    1,  1,  1,
    1650, 1280, 110, 40,  750, 720, 5,  5 ,
    MK_SCALE(0, 0),
  },
  {
    108000,
    MK_PIXEL_HDMI(1, 0, 0),
    1,  1,  1,
    1688, 1280, 48,  112, 1066, 1024, 1,  3,
    MK_SCALE(0, 0),
  },
  {
    88750,
    MK_PIXEL_HDMI(1, 0, 0),
    1,  1,  1,
    1600, 1440, 48,  32,  926, 900, 3,  6,
    MK_SCALE(0, 0),
  },
  {
    119000,
    MK_PIXEL_HDMI(1, 0, 0),
    1,  1,  1,
    1840, 1680, 48,  32,  1080, 1050, 3,  6,
    MK_SCALE(0, 0),
  },
  {
    148500,
    MK_PIXEL_HDMI(1, 0, 0),  // dvi, fmt, aspect
    1,  1,  1,       // hpo, vpo, depo
    2200, 1920, 88,  44,  1125, 1080, 4,  5,
    MK_SCALE(0, 0),
  },
  {
    162000,
    MK_PIXEL_HDMI(1, 0, 0),
    1,  1,  1,
    2160, 1600, 64,  192, 1250, 1200, 1,  3,
    MK_SCALE(0, 0),
  },
  //=====
  {
    130250,
    MK_PIXEL_HDMI(1, 0, 0),
    1,  1,  1,
    1760,  1600, 48, 32,   1235,1200,  3,    4 ,
    MK_SCALE(0, 0),
  },
  {
    161000,
    MK_PIXEL_HDMI(1, 0, 0),
    1,  1,  1,
    2160,  1600,112, 168,   1245,1200,  3,    4  ,
    MK_SCALE(0, 0),
  },
  // CVT 1920x1200 reduced blanking modeline (not from chrontel)
  // (As well as CVT this matches Dell monitor DTD)
  {
    154000,
    MK_PIXEL_HDMI(1, 0, 0),
    1,  0,  1,
    2080,  1920, 48, 32,   1235,1200,  3,    6 ,
    MK_SCALE(0, 0),
  },


};
#define N_KNOWN_MODES (sizeof(known_modes)/sizeof(struct timing_ch7036))


struct timeval mkwin_tout;


Window x_start_blank(Display *dpy, int screen, Window root, int lw, int lh)
{
  Window black_window;

  XSetWindowAttributes attr;
  XEvent expev;
  struct timeval tv;
  const struct timeval tout = {10, 500000};
  const struct timeval animate_delay = {0, 300000};

  int seen_expose;
  attr.event_mask = ExposureMask;
  attr.background_pixel = BlackPixel(dpy, screen);
  attr.border_pixel = BlackPixel(dpy, screen);

  black_window = XCreateWindow(dpy, root,
                               0, 0, lw, lh, /* Full screen size */
                               0, 0, /* No border */
                               InputOutput, (Visual *)CopyFromParent,
                               (CWBackPixel | CWBorderPixel | CWEventMask),
                               &attr);
  XMapWindow(dpy, black_window);

  /* Need for the window to be created before we resize the root!    */
  /* This is more tricky than it should be                           */
  /* The initial Expose event is for the whole window                */
  /* even though it is still being animated on to the actual screen  */
  /* So Im going to wait animate_delay seconds after                 */
  /* Same timer mechanism for 10 sec total timeout                   */
    gettimeofday(&tv, NULL);
    timeradd(&tv, &tout, &mkwin_tout);
    do {
      seen_expose = XCheckWindowEvent(dpy, black_window,
                                      ExposureMask,
                                      (XEvent *)&expev);
      gettimeofday(&tv, NULL);
      if (seen_expose) {
        timeradd(&tv, &animate_delay, &mkwin_tout);
        break;
      }
    } while (timercmp(&tv, &mkwin_tout, <));
    return black_window;
}

void set_x_size(Display *dpy, Window root, int w, int h,
                int use_black, Window black_window)
{
  XConfigureEvent ev;
  XEvent expev;
  struct timeval tv;

  /* Wait round if the timeout didn't expire */
  if (use_black) do {
      /* Do the check to let X flush things */
      XCheckWindowEvent(dpy, black_window, ExposureMask, &expev);
      gettimeofday(&tv, NULL);
    } while (timercmp(&tv, &mkwin_tout, <));

  ev.type = ConfigureNotify;
  ev.display = dpy;
  ev.serial = 0;
  ev.send_event = True;
  ev.event = root;
  ev.window = root;
  ev.x = 0;
  ev.y = 0;
  ev.width = w;
  ev.height = h;
  ev.border_width = 0;
  ev.above = None;
  ev.override_redirect = 0;
  XSendEvent(dpy, root, False,
             StructureNotifyMask|SubstructureNotifyMask, (XEvent *)&ev);
  XFlush(dpy);
  if (use_black) {
    XDestroyWindow(dpy, black_window);
    XFlush(dpy);
  }
}


void usage(char *prog)
{
  fprintf(stderr,
          "Usage: %s [options]\n", prog);
  fprintf(stderr, "-d<filename>: set i2c device to use (default %s)\n",
          DEF_DEV);
  fprintf(stderr, "-g<filename>: enable and set gpio device for HDMI detect\n");
  fprintf(stderr, "-f<filename>: set firmware to use (default %s)\n", DEF_FW);
  fprintf(stderr, "-n: Use dummy i2c device\n");
  fprintf(stderr, "-v: Verbose, print every I2C access\n");
  fprintf(stderr, "-p: Probe: check I2C access and Chrontel ID and exit.\n");
  fprintf(stderr, "-W<width>,<aspect>: Force aspet and max width\n");
  fprintf(stderr, "-E<number>: Force use of test EDID number\n");
  fprintf(stderr, "-T<number>: Enable test flags based on number\n");
  fprintf(stderr, "            0x%03x - Force detection of monitor\n",
          CHTEST_FORCEDETECT);
  fprintf(stderr, "            0x%03x - Use experiment 1 (if there is one)\n",
          CHTEST_EXPERIMENT1);
  fprintf(stderr, "            0x%03x - Use experiment 2 (if there is one)\n",
          CHTEST_EXPERIMENT2);
  fprintf(stderr, "-m: list known modes\n");
  fprintf(stderr, "-M<number>: Force use of mode number\n");
  fprintf(stderr, "-x: Fixup X server width\n");
  fprintf(stderr, "-X: Fixup X server width and exit\n");

  exit(1);
}



int get_native(unsigned char *data, struct timing_ch7036* prefer, int* aspect,
               int forcen_a, int verbose)
{
  int seen_pref = 0;
  int interlace = 0;
  int i;
  int n_dtds = EDID_N_DTDS;
  int has_hdmi = edid_has_hdmi_info(data, data[EDID_EXT_FLAG] ? 1:0);

  /* In HDMI case know start of extra DTDs and max available space */
  /* This gives a max, some could be in the 00 padding area */
  if (has_hdmi)
    n_dtds += (CEA_LAST_PAD - data[EDID_SIZE+CEA_DTD_OFFSET])/DTD_SIZE;

  for(i = 0; i < n_dtds; i++) {
    int pelclk;
    unsigned char* base;

    /* First DTDs are in the main EDID, any more are in the extension */
    if (i < EDID_N_DTDS)
      base = data + EDID_DTD_BASE + DTD_SIZE*i;
    else
      base = (data + EDID_SIZE + data[EDID_SIZE+CEA_DTD_OFFSET] +
              DTD_SIZE*(i-EDID_N_DTDS));

    /* In extension block this will be 0 when we hit padding */
    /* in main block 0 indicates something other than DTD    */
    pelclk = base[DTD_PCLK_LO] + (base[DTD_PCLK_HI]<<8);

    if (pelclk != 0) {
      int hres = base[DTD_HA_LO] + ((base[DTD_HABL_HI] & 0xf0)<<4);
      int hbl = base[DTD_HBL_LO] + ((base[DTD_HABL_HI] & 0x0f)<<8);
      int vres = base[DTD_VA_LO] + ((base[DTD_VABL_HI] & 0xf0)<<4);
      int vbl = base[DTD_VBL_LO] + ((base[DTD_VABL_HI] & 0x0f)<<8);
      int hso = base[DTD_HSO_LO] + ((base[DTD_HVSX_HI] & 0xc0)<<2);
      int hsw = base[DTD_HSW_LO] + ((base[DTD_HVSX_HI] & 0x30)<<4);
      int vso = (base[DTD_VSX_LO] >> 4) + ((base[DTD_HVSX_HI] & 0x0c)<<2);
      int vsw = (base[DTD_VSX_LO] & 0xf) + ((base[DTD_HVSX_HI] & 0x03)<<4);
      int hsiz = base[DTD_HSIZE_LO] + ((base[DTD_HVSIZE_HI] & 0xf0)<<4);
      int vsiz = base[DTD_VSIZE_LO] + ((base[DTD_HVSIZE_HI] & 0x0f)<<8);
      //int hbdr = base[DTD_HBORDER];
      //int vbdr = base[DTD_VBORDER];
      int mdflg = base[DTD_FLAGS];

      //int refr = (pelclk * 10000)/((hres+hbl)*(vres+vbl));
      //int refm = (pelclk * 10000)%((hres+hbl)*(vres+vbl));
      //int refd = (refm*100)/((hres+hbl)*(vres+vbl));

      if (hres > CH_MAX_HRES) {
        if (verbose) printf("Ignore %dx%d mode since too wide for scaler\n",
                            hres, vres);
        continue;
      }
      /* First encountered is the preferred and native */
      /* BUT: Seen some devices that list interlaced mode first */
      /* If they also have non-interlaced then use that    */
      /* HDMI monitors don't seem to obey this, there may be a better */
      /* mode in the extension block, look for something wider */
      if ((!seen_pref) ||
          (seen_pref && interlace && ((mdflg & 0x80) == 0)) ||
          (seen_pref && has_hdmi &&
           ((mdflg & 0x80) == 0) && (hres > prefer->ha))) {

        prefer->clk_khz = pelclk * 10;
        prefer->pixel_fmt = (has_hdmi) ? 0 : PIXEL_HDMI_ENCODE_DVI;
        if (mdflg & 0x80) prefer->pixel_fmt |= PIXEL_HDMI_ENCODE_INTERLACE;
        prefer->hs_pol = (mdflg & 2) ? POL_HIGH : POL_LOW;
        prefer->vs_pol = (mdflg & 4) ? POL_HIGH : POL_LOW;
        prefer->de_pol = POL_HIGH;

        prefer->ht = hres + hbl;
        prefer->ha = hres;
        prefer->ho = hso;
        prefer->hw = hsw;
        prefer->vt = vres + vbl;
        prefer->va = vres;
        prefer->vo = vso;
        prefer->vw = vsw;
        prefer->scale = MK_SCALE(0, 0);

        *aspect = find_aspect(hres, vres);
        interlace = (mdflg & 0x80);

        if (*aspect < 0) {
          if (verbose)
            printf("Didn't find aspect from %dx%d, trying size %dx%d\n",
                   hres, vres, hsiz, vsiz);
          *aspect = find_aspect(hsiz, vsiz);
          if ((*aspect < 0) && interlace) {
            /* This is really about 1920x540i, seen in the wild */
            if (verbose) printf("No aspect for interlaced, try double v\n");
            *aspect = find_aspect(hres, vres*2);
          }
          if (*aspect < 0) {
            if (verbose) printf("Still couldn't find aspect, use 4x3\n");
            *aspect = ASPECT_4_3;
          } else
            if (verbose) printf("Found aspect %s\n", aspect_to_str[*aspect]);
        }
        if ((forcen_a < 0) || (*aspect == forcen_a)) seen_pref = 1;
      }
    }
  }
  return seen_pref;
}

/* Using 'fake' standard timing entries to cover some established timings */
#define EST_TIME_640x480x60 (EDID_N_STDTIME+0)
#define EST_TIME_800x600x60 (EDID_N_STDTIME+1)
#define EST_TIME_1024x768x60 (EDID_N_STDTIME+2)
#define KNOWN_EST_TIMES 3

int get_standard(unsigned char *data,
                 struct timing_ch7036 **mon, int *found_aspect,
                 int force_maxw, int force_aspect, int verbose)
{
  int curbest_aspect = -1;
  struct timing_ch7036 *curbest = NULL;
  int i, md;
  int found_md;
  int native_aspect;

  if (force_maxw > 0)
    native_aspect = force_aspect;
  else
    native_aspect = find_aspect_fromisize(data);
  if (verbose) printf("Check standard modes (monitor native aspect %s)\n",
                      aspect_to_str[native_aspect]);

  for(i=0; i < (EDID_N_STDTIME + KNOWN_EST_TIMES); i++) {
    int hinfo = 0x01;
    int vinfo = 0x01;
    int hres, vres;

    if (i < EDID_N_STDTIME) {
      hinfo = data[EDID_STDTIMEH + STDTIME_SIZE*i];
      vinfo = data[EDID_STDTIMEV + STDTIME_SIZE*i];
    }
    /* 8-10 check established timing 60Hz entries */
    if ((i == EST_TIME_640x480x60) && (data[EDID_ESTTIME1] & 0x20)) {
      hinfo = (640-STDTIME_HBASE)/STDTIME_HMULT;
      vinfo = ASPECT_4_3 << STDTIME_VASPECT_SHIFT;
    }
    if ((i == EST_TIME_800x600x60) && (data[EDID_ESTTIME1] & 0x01)) {
      hinfo = (800-STDTIME_HBASE)/STDTIME_HMULT;
      vinfo = ASPECT_4_3 << STDTIME_VASPECT_SHIFT;
    }
    if ((i == EST_TIME_1024x768x60) && (data[EDID_ESTTIME2] & 0x08)) {
      hinfo = (1024-STDTIME_HBASE)/STDTIME_HMULT;
      vinfo = ASPECT_4_3 << STDTIME_VASPECT_SHIFT;
    }

    /* 01 01 is pad by spec, but 00 00 and 20 20 are see in wild */
    if (((hinfo == 0x01) && (vinfo == 0x01)) ||
        ((hinfo == 0x00) && (vinfo == 0x00)) ||
        ((hinfo == 0x20) && (vinfo == 0x20)))
      continue;
    /* Not doing above 60Hz */
    if ((vinfo & STDTIME_VREFMINUS60_MASK) != 0) continue;

    hres = (hinfo * STDTIME_HMULT) + STDTIME_HBASE;
    if (verbose) printf("Found %d wide %s\n", hres,
                        aspect_to_str[vinfo>>STDTIME_VASPECT_SHIFT]);
    if ((force_maxw > 0) && (hres > force_maxw)) {
      if (verbose) printf("-- ignore because greater than forced max width\n");
      continue;
    }
    if (hres > CH_MAX_HRES) {
      if (verbose) printf("-- too wide for scaler\n");
      continue;
    }

    switch (vinfo >> STDTIME_VASPECT_SHIFT) {
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
    }
    found_md = -1;
    for(md = 0; md < N_KNOWN_MODES; md++) {
      if ((known_modes[md].ha == hres) && (known_modes[md].va == vres)) {
        // Preference is for a DVI mode, so done when we find the first
        if (known_modes[md].pixel_fmt & PIXEL_HDMI_ENCODE_DVI) {
          found_md = md;
          break;
        }
        // If no DVI modes then use the first matching entry
        if (found_md < 0) found_md = md;
      }
    }
    if (found_md >= 0) {
      /* Found a mode that we know and monitor can do */
      if ((curbest == NULL) ||
          (((vinfo >> STDTIME_VASPECT_SHIFT) == native_aspect) &&
           ((curbest_aspect != native_aspect) || (hres > curbest->ha)))) {
        /* We had nothing, or we found first or bigger native aspect */
        if (verbose) printf("Better mode width %d, aspect %s\n",
                            hres, aspect_to_str[vinfo>>STDTIME_VASPECT_SHIFT]);
        curbest = known_modes + found_md;
        curbest_aspect = (vinfo >> STDTIME_VASPECT_SHIFT);
      } else if (curbest_aspect != native_aspect) {
        if ((native_aspect == ASPECT_16_10) &&
            ((vinfo >> STDTIME_VASPECT_SHIFT) == ASPECT_16_9) &&
            ((curbest_aspect != ASPECT_16_9) || (hres > curbest->ha))) {
          /* Looking for 16:10, found first 16:9 or bigger 16:9 */
          if (verbose) printf("Want 16:10 use 16:9 width %d, aspect %s\n",
                              hres,
                              aspect_to_str[vinfo>>STDTIME_VASPECT_SHIFT]);
          curbest = known_modes + found_md;
          curbest_aspect = (vinfo >> STDTIME_VASPECT_SHIFT);
        } else if (hres > curbest->ha) {
          if (verbose) printf("Better based on size: width %d, aspect %s\n",
                              hres,
                              aspect_to_str[vinfo>>STDTIME_VASPECT_SHIFT]);
          curbest = known_modes + found_md;
          curbest_aspect = (vinfo >> STDTIME_VASPECT_SHIFT);
        }
      }
    } else if (verbose) printf("-- no match in known_modes table\n");

  }
  if (curbest != NULL) {
    *mon = curbest;
    *found_aspect = curbest_aspect;
    return 1;
  }
  return 0;
}

/* Function added by Chrontel */
/* FIXME: Should this move to ch7036_access.c */
void set_ch7036_config(int i2cdev)
{
  struct config_ch7036 mycfg;
  uint8 page;

  mycfg.size = sizeof(struct config_ch7036);
  //assuming page 1, the read function will return right page
  page = ch_read_reg(i2cdev, 0x1, 0x03);
  ch_write_reg(i2cdev, page, 0x03, 0x04);
  mycfg.deviceid = ch_read_reg(i2cdev,0x4,0x50);

  mycfg.revisionid = 0x0F & ch_read_reg(i2cdev,0x4,0x51);
  ch_write_reg(i2cdev,0x4, 0x03, page); // restore orignal page

  mycfg.comv_off = CFG7036_COMV_OFF_YES; // only work for certain LVDS panel!!!
  mycfg.comv_off = CFG7036_COMV_OFF_DEFAULT;

  mycfg.drv_strength = CFG7036_DRV_STRENGTH_DEFAULT;
  mycfg.fbdly = CFG7036_FBDLY_DEFAULT;
  mycfg.refdly = CFG7036_REFDLY_DEFAULT;
  mycfg.lvds_inout = CFG7036_LVDS_INOUT_DEFAULT;

  SetGlobalConfigCH7036( &mycfg);
  return;
}

void show_ch_timing(struct timing_ch7036* timing, char* what)
{
  printf("GenTableCH7036 %s timing set to:\n", what);
  printf("    %d,\n", timing->clk_khz);
  printf("    0x%05x,\n", timing->pixel_fmt);
  printf("    %d, %d, %d,\n",
         timing->hs_pol, timing->vs_pol, timing->de_pol);
  printf("    %d, %d, %d, %d, %d, %d, %d, %d,\n",
         timing->ht, timing->ha, timing->ho, timing->hw,
         timing->vt, timing->va, timing->vo, timing->vw);
  printf("    0x%04x,\n", timing->scale);
}

int reloadfw(int i2cdev, char *fwfile, char *progname,
               int gpiodev, int need_reset, int verbose)
{
  int mcutry = 0;
  /* This also intializes the changed vector */
  if (need_reset) ch_reset_todefault(i2cdev);

  if (ch_load_firmware(i2cdev, fwfile) < 0) {
    fprintf(stderr, "%s: Could not load firmware %s\n", progname, fwfile);
    return ERR_FAILED;
  }
  do {
    if (ch_mcu_version(i2cdev, 1, verbose) > 0) break;
    sleep(YIELD_FIRMWARE_START_SECS);
    mcutry++;
    if (mcutry >= MCU_ATTEMPTS_AFTER_FWLOAD) return ERR_NEED_RELOAD;
  } while (mcutry < MCU_ATTEMPTS_AFTER_FWLOAD);
  /* HDMI detection seems a bit bouncy after firmware load */
  /* If there was a sleep added here it would delay detection */
  /* at startup time, so don't do that */
  /* Using the longer ch_mcu_version timeout makes it less liekly */
  /* Turning the monitor on here seems to make the problem go away */
  /* but it causes LCD flicker so avoid when using gpio detection */
  if (gpiodev) {
    if (verbose) printf("GPIO detection, don't bounce the display\n");
  } else {
    /* Don't use verbose: hdmi setting is don't care for workaround */
    ch_monitor_on(i2cdev, 0, 0);
    ch_monitor_off_keep_ddc(i2cdev);
  }
  return 0;
}

int wait_monitor_attach(int i2cdev, int gpiodev, int verbose)
{
  int needReload = 0;

  if (gpiodev) {
    int chkfw = 0;
    /* Force detect acts as if HPD is always set, so no need for loop */
    if (!(test_flags & CHTEST_FORCEDETECT)) {
      /* Don't pass verbose flag here as 2nd arg, it is too verbose */
      while (!needReload && (ch_hdmi_gpio_detected(gpiodev, 0) == 0)) {
        if (++chkfw > CHECK_FW_INTERVAL_IN_GPIOLOOPS) {
          chkfw = 0;
          if (ch_mcu_version(i2cdev, 0, 0) < 0) {
            if (verbose) printf("Lost Chrontel firmware waiting for gpio\n");
            needReload = 1;
            continue;
          }
        }
        sleep(YIELD_GPIO_DETECT_SECS);
      }
    }
    /* Need to enable DDC to allow EDID read */
    ch_write_reg(i2cdev, CH7036_MAGIC2_0E, 0x13); // MAGIC
    if (!needReload && (ch_mcu_version(i2cdev, 0, 0) < 0)) {
      if (verbose) printf("GPIO detected but lost the Chrontel firmware\n");
      needReload = 1;
    }
  } else
    while (!needReload &&
           !(test_flags & CHTEST_FORCEDETECT) && !ch_hdmi_detected(i2cdev)) {
      sleep(YIELD_CHRONTEL_DETECT_SECS);
      /* Check the Chrontel firmware is still alive */
      /* Originally only did this when DPMS said screen came on */
      /* But that misses the case where lid close takes system to s3 */
      if (ch_mcu_version(i2cdev, 0, 0) < 0) {
        if (verbose) printf("Lost the Chrontel firmware\n");
        needReload = 1;
      }
    }
  return needReload;
}

int get_edid(int i2cdev, unsigned char *edid, int newFirmware, int verbose)
{
  int err = ch_get_edid(i2cdev, 0, &edid[0]);

  if (err < 0) {
    if (ch_mcu_version(i2cdev, 0, verbose) < 0) {
      if (verbose) printf("EDID error because Chrontel firmware lost\n");
      return ERR_NEED_RELOAD;
    }
    /* EDID timed out, but things seem alive */
    if (newFirmware) {
      if (verbose)
        printf("Ignore HPD because EDID failed just after firmware load\n");
      sleep(YIELD_CHRONTEL_DETECT_SECS);
      /* Loop back to recheck HPD after sleep */
      return ERR_NEWFW_RETRY;
    }
    return ERR_READ_FAILED;
  } else if (!edid_valid(&edid[0])) {
    if (newFirmware) {
      if (verbose)
        printf("Ignore HPD since EDID not valid just after firmware load\n");
      sleep(YIELD_CHRONTEL_DETECT_SECS);
      /* The EDID was read, so next time it is not new fw */
      /* Loop back to recheck HPD after sleep */
      return ERR_NEWFW_RETRY;
    }
    /* Returns ok because EDID was read. It won't be useful... */
    return 0;
  } else if (edid[EDID_EXT_FLAG] > 0)
    if (ch_get_edid(i2cdev, 1, edid+EDID_SIZE) < 0) return ERR_EXTRD_FAILED;
  return 0;
}

int init_lcd_timing(struct timing_ch7036 *lcd_timing, int *width_fix,
                    XRRModeInfo *xmode)
{
  if ((xmode->width == 1366) && (xmode->height == 768)) {
    width_fix = width_fix_1366x768;
  } else if ((xmode->width == 1280) && (xmode->height == 800)) {
    width_fix = width_fix_1280x800;
  } else
    width_fix = NULL;

  lcd_timing->clk_khz = xmode->dotClock/1000;
  lcd_timing->pixel_fmt = PIXEL_FMT_18BIT;
  lcd_timing->hs_pol = (xmode->modeFlags & RR_HSyncPositive) ? 1 : 0;
  lcd_timing->vs_pol = (xmode->modeFlags & RR_VSyncPositive) ? 1 : 0;
  lcd_timing->de_pol = 1;

  lcd_timing->ht = xmode->hTotal;
  lcd_timing->ha = xmode->width;
  lcd_timing->ho = xmode->hSyncStart - xmode->width;
  lcd_timing->hw = xmode->hSyncEnd - xmode->hSyncStart;
  lcd_timing->vt = xmode->vTotal;
  lcd_timing->va = xmode->height;
  lcd_timing->vo = xmode->vSyncStart - xmode->height;
  lcd_timing->vw = xmode->vSyncEnd - xmode->vSyncStart;
  lcd_timing->scale = MK_SCALE(0, 0);
}


int main(int argc, char *argv[])
{
  int i2cdev;
  char *i2cfile = DEF_DEV;
  int gpiodev;
  char *gpiofile = NULL;
  char *fwfile = DEF_FW;
  int arg;
  int err;
  Display *dpy;
  Window root;
  int screen;
  Window bwin;
  XRRModeInfo *xmode;
  int forcen_x = 0;
  int forcen_a = -1;
  int force_edid = 0;
  int probe_only = 0;
  int dummy_i2c = 0;
  int verbose = 0;
  int *width_fix;
  unsigned char edid[256];
  unsigned char newedid[256];
  int use_new_edid = 0;
  int aspect;
  int hdmiOn;
  int mondetect;
  int expectres;
  int needReload = 1;   /* Force firmware to load on the first loop */
  int newFirmware = 1;
  int force_mode = -1;
  int fixupx = 0;
  int only_fixupx = 0;

  struct reg_ch7036 *reglist;

  struct timing_ch7036 lcd_timing;
  struct timing_ch7036 prefer;
  struct timing_ch7036 *mon_timing;
  /* First entry in known modes table is 640x480 @ 60Hz */
  struct timing_ch7036 *mode_640x480x60 = known_modes;

  /* Set stdout for easy logs during debug */
  setlinebuf(stdout);
  printf("%s: starts\n", argv[0]);
  for(arg=1; arg < argc; arg++) {
    if ((argv[arg][0]) != '-') break;
    switch (argv[arg][1]) {

    default:
      fprintf(stderr, "Unknown option: %s\n", argv[arg]);
      usage(argv[0]);

    case 'p':
      probe_only = 1;
      break;

    case 'n':
      dummy_i2c = 1;
      break;

    case 'X':
      only_fixupx = 1;
      /* Fall through */
    case 'x':
      fixupx = 1;
      break;

    case 'V':
      ch7036_debugi2c = 1;
      /* Fall through */
    case 'v':
      verbose = 1;
      break;

    case 'd':
      if (argv[arg][2] != 0)
        i2cfile = argv[arg] + 2;
      else {
        if (++arg < argc)
          i2cfile = argv[arg];
        else {
          fprintf(stderr, "-d needs filename: %s\n", argv[arg]);
          usage(argv[0]);
        }
      }
      break;

    case 'g':
      if (argv[arg][2] != 0)
        gpiofile = argv[arg] + 2;
      else {
        if (++arg < argc)
          gpiofile = argv[arg];
        else {
          fprintf(stderr, "-g needs filename: %s\n", argv[arg]);
          usage(argv[0]);
        }
      }
      break;

    case 'f':
      if (argv[arg][2] != 0)
        fwfile = argv[arg] + 2;
      else {
        if (++arg < argc)
          fwfile = argv[arg];
        else {
          fprintf(stderr, "-f needs filename: %s\n", argv[arg]);
          usage(argv[0]);
        }
      }
      break;

    case 'E':
      force_edid = strtol(argv[arg] + 2, NULL, 10);
      if ((force_edid < 1) || (force_edid > N_TEST_EDIDS)) {
          fprintf(stderr, "-E%d out of range, must be 1-%d\n",
                  force_edid, N_TEST_EDIDS);
          usage(argv[0]);
      }
      break;

    case 'T':
      test_flags = strtol(argv[arg] + 2, NULL, 0);
      if (test_flags)
        fprintf(stderr, "%s: WARNING enabling tests with flags 0x%x\n",
                argv[0], test_flags);
      break;

    case 'W':
      {
        char *endn;

        forcen_x = strtol(argv[arg] + 2, &endn, 10);
        if (endn[0] != ',') {
          fprintf(stderr, "Expecting comma (-W<max_width>,<aspect>) in %s\n",
                  argv[arg]);
          usage(argv[0]);
          forcen_x = 0;
        } else
          forcen_a = strtol(endn + 1, &endn, 10);
        if (endn != argv[arg]) argv[arg] = endn - 1;
      }
      break;

    case 'm':
      {
        int md;

        for(md = 0;
            md < sizeof(known_modes)/sizeof(struct timing_ch7036); md++)
          printf("%2d - %s %dx%d\n", md,
                 ((known_modes[md].pixel_fmt & PIXEL_HDMI_ENCODE_DVI) ?
                  "DVI" : "HDMI"),
                 known_modes[md].ha, known_modes[md].va);
        exit(0);
      }
    case 'M':
      {
        char *endn;

        force_mode = strtol(argv[arg] + 2, &endn, 10);
        if (endn != argv[arg]) argv[arg] = endn - 1;
      }
    }
  }

  if (dummy_i2c)
    i2cdev = -1;
  else {
    int tryi2c = probe_only ? 20 : 1;

    /* udev runs fairly late. */
    /* To allow early probe tollerate device not being there for a while */

    while (tryi2c > 0) {
      i2cdev = open(i2cfile, O_RDWR);
      if (i2cdev > 0) break;

      if (verbose || !probe_only)
        fprintf(stderr, "%s: Could not open %s: %s\n",
                argv[0], i2cfile, strerror(errno));
      tryi2c--;

      if (tryi2c < 1)
        exit(1);
      sleep(1);
    }
  }
  /* CH7036 only has one slave address option */
  if (dummy_i2c)
    err = 0;
  else
    err = ioctl(i2cdev, I2C_SLAVE, CH7036_I2C_ADDR);

  if (err < 0) {
      if (verbose || !probe_only)
        fprintf(stderr, "%s: Could not set slave address 0x%x: %s\n",
                argv[0], CH7036_I2C_ADDR, strerror(errno));
    exit(1);
  }

  /* Read the information */
  err = ch_read_reg(i2cdev, CH7036_DEVID);

  if (verbose)
    fprintf(stderr, "Found device ID 0x%02x\n", err);

  if (err != EXPECTED_CH7036_DEVID) {
    if (verbose || !probe_only)
      fprintf(stderr,
              "%s: Fatal: Device ID 0x%02x not the expected 0x%02x\n",
              argv[0], err, EXPECTED_CH7036_DEVID);
    exit(1);
  }

  if (probe_only)
    exit(0);

  if (verbose)
    fprintf(stderr, "Found revision ID 0x%02x\n",
            ch_read_reg(i2cdev, CH7036_REVID));

  if (gpiofile != NULL) {
    gpiodev = open(gpiofile, O_RDONLY);
    if (gpiodev < 0) {
      fprintf(stderr, "Could not open %s: %s\n", gpiofile, strerror(errno));
      exit(1);
    }
  } else
    gpiodev = 0;

  dpy = XOpenDisplay(":0.0");
  if (dpy == NULL) {
    fprintf(stderr, "%s: Can't open display :0.0\n", argv[0]);
    exit(1);
  }
  screen = DefaultScreen (dpy);
  root = RootWindow (dpy, screen);
  xmode = get_x_mode_info(dpy, root);
  if (xmode != NULL) {
    if (verbose) showmode(xmode);

    if (fixupx) {
      set_x_size(dpy, root, xmode->width, xmode->height, 0, 0);
      if (only_fixupx) exit(0);
    }
    init_lcd_timing(&lcd_timing, width_fix, xmode);
  }
  audio_init(verbose);

  while (1) {
    int new_width;
    int new_height;
    FILE* edidlog;
    char *why_res = "nothing";
    char *why_aud;
    int enable_audio = 0;
    int output_hdmi = 0;
    int use_black_window;
    int ares;
    if ((edidlog = fopen(EDID_LOG, "w")) != NULL) {
      fprintf(edidlog, "No HDMI device detected\n");
      fclose(edidlog);
    }

    if (needReload)
      needReload = reloadfw(i2cdev, fwfile, argv[0], gpiodev, 1, verbose);
    if (needReload == ERR_FAILED) exit(1);
    if (needReload) continue;

    needReload = wait_monitor_attach(i2cdev, gpiodev, verbose);
    /* Recycle the outer while loop to hit the fw reload code */
    if (needReload) continue;

    if (force_edid) {
      /* Should not fail because force_edid was checked above */
      err = get_test_edid(force_edid, &edid[0]);
    } else if (use_new_edid) {
      int i;
      for(i=0; i < (EDID_SIZE + EEXT_SIZE); i++) edid[i] = newedid[i];
      err = 0;
      use_new_edid = 0;
    } else {
      err = get_edid(i2cdev, &edid[0], newFirmware, verbose);
      newFirmware = 0;
      /* If the error happened with new firmware we try again */
      if (err == ERR_NEWFW_RETRY) continue;
    }

    mon_timing = NULL;
    enable_audio = 0;
    why_aud = "LPCM Audio support not found in EDID";

    if (force_mode >= 0) {
      /* Mode forced on command line: ignore EDID */
      mon_timing = &known_modes[force_mode];
      aspect = find_aspect(mon_timing->ha, mon_timing->va);
      // Select audio if it is an HDMI mode
      if (mon_timing->pixel_fmt & PIXEL_HDMI_ENCODE_DVI) {
        enable_audio = 0;
        why_res = "DVI mode forced on command line";
        why_aud = "audio forced off";
      } else {
        enable_audio = 1;
        why_res = "HDMI mode forced on command line";
        why_aud = "audio forced on";
      }
      if (verbose) printf("Force mode %d with aspect %d\n",
                          force_mode, aspect);
    } else if (err) {
      /* Error reading the EDID */
      fprintf(stderr, "%s: Get EDID Failed (%s block) using 640x480@60\n",
              argv[0], (err == ERR_READ_FAILED) ? "initial" : "extension");
      why_res = "default resolution because EDID could not be read";
      mon_timing = mode_640x480x60;
      aspect = ASPECT_4_3;
    } else if (!edid_valid(&edid[0])) {
      /* EDID is not valid */
      fprintf(stderr, "%s: EDID is not valid, using 640x480@60\n",
              argv[0]);
      why_res = "default resolution because EDID is not valid";
      mon_timing = mode_640x480x60;
      aspect = ASPECT_4_3;
    } else {
      /* Got an EDID, use it to figure out the timing */
      if (verbose) {
        show_edid_data(stdout, &edid[0], EDID_SIZE, 0);
        if (edid[EDID_EXT_FLAG] > 0)
          show_edid_data(stdout, &edid[EDID_SIZE], EEXT_SIZE, EDID_SIZE);
        show_edid(stdout, &edid[0], (edid[EDID_EXT_FLAG] > 0) ? 1:0);
      }
      if ((forcen_x == 0) &&
          get_native(&edid[0], &prefer, &aspect, forcen_a, verbose)){
        mon_timing = &prefer;
        why_res = "preferred/native resolution requested in EDID";

        /* HDMI TVs tend to assume overscan (i.e. junk round the real frame)*/
        /* even when we tell them otherwise with the AVI INFO frame         */
        /* even if they set the 'underscan' flag that says they should      */
        /* If the EDID has an HDMI extension then assume this badness       */
        /* unless it is not 16:9 because most monitors are well behaved     */
        if (edid_has_hdmi_info(&edid[0], (edid[EDID_EXT_FLAG] > 0) ? 1:0)) {
          if (aspect == ASPECT_16_9) {
            prefer.scale = MK_SCALE(6, 5);
            why_res = "preferred resolution in EDID scaled for overscan";
          }
          if (edid_lpcm_support(&edid[0], (edid[EDID_EXT_FLAG] > 0) ? 1:0)) {
            enable_audio = 1;
            why_aud = "EDID indicates LPCM audio support";
          }
        }
      } else {
        get_standard(&edid[0], &mon_timing, &aspect, forcen_x, forcen_a,
                     verbose);
        why_res = "best standard resolution supported in EDID";
      }
    }

    /* Could loop here 3 times: preferred, standard and default */
    do {
      while (xmode == NULL) {
        if (verbose) printf("Querying xmode info\n");
        xmode = get_x_mode_info(dpy, root);
        if (xmode == NULL)
          sleep(YIELD_CHRONTEL_DETECT_SECS);
      }
      init_lcd_timing(&lcd_timing, width_fix, xmode);
      /* Catch error cases either from the code above or second pass of loop */
      if (mon_timing == NULL) {
        if (verbose)
          printf("%s: Fall back to default output 640x480@60\n", argv[0]);
        why_res = "default resolution because nothing better was identified";
        mon_timing = mode_640x480x60;
        aspect = ASPECT_4_3;
      }
      if (verbose) {
        show_ch_timing(&lcd_timing, "input (laptop lcd)");
        show_ch_timing(mon_timing, "output (hdmi port)");
      }
      if ((width_fix != NULL) && (aspect >= 0))
        new_width = width_fix[aspect];
      else
        new_width = xmode->width;
      new_height = xmode->height;

      /* Special case for 1280 width to avoid shrinking by a small amount */
      /* (the 5:4 aspect 1280x1024 will have reduced new_width above) */
      if ((new_width == 1366) && (mon_timing->ha == 1280)) {
        new_width = 1280;
        /* Mostly for 1280x720 which is popular for TVs, avoid y scale too */
        if (mon_timing->va < xmode->height) new_height = mon_timing->va;
      }

      lcd_timing.ha = new_width;
      lcd_timing.ho = xmode->hSyncStart - new_width;
      lcd_timing.va = new_height;
      lcd_timing.vo = xmode->vSyncStart - new_height;

      set_ch7036_config(i2cdev);
      err = GenTableCH7036(&lcd_timing, mon_timing, &reglist);

      if (err != 0) {
        fprintf(stderr, "%s: GenTableCH7036 gave error %d\n",
                argv[0], err);
        if (mon_timing != &prefer) {
          /* Force the default to be used */
          mon_timing = NULL;
        } else {
          /* Prefered timing didn't help us, try standard with no force */
          mon_timing = NULL;
          get_standard(&edid[0], &mon_timing, &aspect, 0, 0, verbose);
        }
      }
    } while (err != 0);

    use_black_window = 0;

    /* The use flag bogus_screen_resizes gives this define to both */
    /* this driver and the chromeos window manager. When set the   */
    /* wm will clear the unused screen areas, so we don't have to  */
    /* This allows the login/unlock screen to clear correctly      */
    /* Aura does not do this but the flag is set to allow non-aura */
#if defined(BOGUS_SCREEN_RESIZES) && !defined(USE_AURA)
    if (verbose) printf("Use collusion with window manager to clear screen\n");
#else
    if (verbose) printf("Try black window to clear screen\n");
    if (new_width != xmode->width) {
      bwin = x_start_blank(dpy, screen, root, xmode->width, xmode->height);
      use_black_window = 1;
    }
#endif

    if (verbose) printf("Restore defaults:\n");
    ch_restore_notin_seq(i2cdev, reglist, verbose);

    if (mon_timing->scale != MK_SCALE(0,0)) {
      int res = ch_change_reg(reglist, CH7036_AVIINFO3, 0xFC, 0x1);
      if (verbose) printf("Change AVIINFO3 from 0x%2x to 0x%2x\n",
                          res, (res & 0xFC)|0x1);
    }
    /* Note: SDTIME1 code that was here has moved to ch_monitor_on() */

    /* Sequence provided has 2 resets each 2 writes which cause flicker */
    /* Only seem to need the reset that is in ch_monitor_on below */
    ares = ch_remove_last_reg(reglist, CH7036_RESET);
    if (verbose) printf("Removed CH7036_RESET write of 0x%2x\n", ares);
    ares = ch_remove_last_reg(reglist, CH7036_RESET);
    if (verbose) printf("Removed CH7036_RESET write of 0x%2x\n", ares);
    ares = ch_remove_last_reg(reglist, CH7036_RESET);
    if (verbose) printf("Removed CH7036_RESET write of 0x%2x\n", ares);
    ares = ch_remove_last_reg(reglist, CH7036_RESET);
    if (verbose) printf("Removed CH7036_RESET write of 0x%2x\n", ares);
    /* Select HDMI output if mode is not DVI or audio is enabled */
    output_hdmi = (((mon_timing->pixel_fmt & PIXEL_HDMI_ENCODE_DVI) == 0) ||
                   enable_audio);
    if (verbose) printf("Program registers:\n");
    ch_write_reg_sequence(i2cdev, reglist);
    ch_calculate_incs(i2cdev);
    ch_monitor_on(i2cdev, output_hdmi, verbose);
    hdmiOn = 1;

    if (new_width != xmode->width) {
      if (verbose) printf("Change X active to %dx%d\n", new_width, new_height);
      set_x_size(dpy, root, new_width, new_height, use_black_window, bwin);
    }
    audio_to_hdmi(enable_audio);
    if (verbose) printf("Monitor should be on!\n");
    if ((edidlog = fopen(EDID_LOG, "w")) != NULL) {
      fprintf(edidlog, "%sI output %dx%d%s. Use %dx%d on local LCD.\n",
              (mon_timing->pixel_fmt & PIXEL_HDMI_ENCODE_DVI) ? "DV" : "HDM",
              mon_timing->ha, mon_timing->va,
              (mon_timing->scale != MK_SCALE(0,0)) ? "(scaled)":"",
              new_width, new_height);
      fprintf(edidlog, "Use %s\n", why_res);
      fprintf(edidlog, "%s audio pass through because %s\n\n",
              enable_audio ? "Enable" : "No", why_aud);
      fprintf(edidlog, "EDID Data read from HDMI:\n");
      show_edid_data(edidlog, &edid[0], EDID_SIZE, 0);
      if (edid[EDID_EXT_FLAG] > 0)
        show_edid_data(edidlog, &edid[EDID_SIZE], EEXT_SIZE, EDID_SIZE);
      show_edid(edidlog, &edid[0], (edid[EDID_EXT_FLAG] > 0) ? 1:0);
      fclose(edidlog);
    }
    /* Suspend the firmware because it has been seen to cause screen flicker */
    /* Can't do this if relying on firmware to report disconnect event */
    if (gpiodev)
      expectres = ch_suspend_firmware(i2cdev, verbose);
    else
      expectres = 0x2f;

    mondetect = 1;
    do {
      CARD16 state;
      BOOL onoff;
      int resreg;

      /* Check for unexpected change in Chrontel */
      /* most likely happens because the machine suspended */
      if ((resreg = ch_read_reg(i2cdev, CH7036_RESET)) != expectres) {
        int i;
        if (verbose)
          printf("Unexpected change in CH7036, RESET 0x%x expecting 0x%x\n",
                 resreg, expectres);
        /* Reload registers assuming part got to default state */
        ch_write_reg_sequence(i2cdev, reglist);
        ch_calculate_incs(i2cdev);
        ch_monitor_on(i2cdev, output_hdmi, verbose);
        audio_to_hdmi(enable_audio);
        hdmiOn = 1;
        /* Machine was suspended, so the firmware was lost */
        needReload = reloadfw(i2cdev, fwfile, argv[0], gpiodev, 0, verbose);
        if (needReload) continue;
        /* Need to enable DDC to allow EDID read */
        if (gpiodev) ch_write_reg(i2cdev, CH7036_MAGIC2_0E, 0x13); // MAGIC
        err = get_edid(i2cdev, &newedid[0], 1, verbose);
        if (err) {
          if (verbose) printf("Could not get new EDID\n");
          needReload = 1;
          continue;
        }
        for(i=0; i<EDID_SIZE; i++) if (edid[i] != newedid[i]) break;
        if (i < EDID_SIZE) {
          if (verbose) printf("New EDID is different, monitor changed!\n");
          /* Clear hdmiOn to prevent the display being disabled below */
          hdmiOn = 0;
          use_new_edid = 1;
          break;
        }
        if (verbose) printf("New EDID is the same, using existing settings\n");
        if (gpiodev) ch_suspend_firmware(i2cdev, verbose);
      } else {
        DPMSInfo(dpy, &state, &onoff);
        xmode = get_x_mode_info(dpy, root);
        if (((state != DPMSModeOn) && hdmiOn) || (xmode == NULL)) {
          if (verbose) printf("DPMS is %son, output is %s, turn off hdmi\n",
                              (state != DPMSModeOn) ? "not " : "",
                              (xmode == NULL) ? "disabled":"enabled");
          ch_monitor_off_keep_ddc(i2cdev);
          hdmiOn = 0;
        } else if ((state == DPMSModeOn) && !hdmiOn) {
          if (verbose) printf("DPMS is on, turn on hdmi\n");
          ch_monitor_on(i2cdev, output_hdmi, verbose);
          if (verbose) printf("Monitor back on with DPMS On\n");
          hdmiOn = 1;
        }
      }
      audio_to_hdmi(enable_audio);
      sleep(gpiodev ? YIELD_GPIO_DETECT_SECS : YIELD_CHRONTEL_DETECT_SECS);
      if (test_flags & CHTEST_FORCEDETECT)
        mondetect = 1;
      else if (gpiodev)
        mondetect = ch_hdmi_gpio_detected(gpiodev, 0);
      else
        mondetect = ch_hdmi_detected(i2cdev);
    } while (!needReload && mondetect);
    if (verbose) printf("Exit displaying needReload = %d, hdmiOn = %d\n",
                        needReload, hdmiOn);

    if (!needReload && gpiodev && hdmiOn)
      ch_release_firmware(i2cdev, verbose);

    if (new_width != xmode->width) {
      if (verbose) printf("Change X active to %dx%d\n",
                          xmode->width, xmode->height);
      set_x_size(dpy, root, xmode->width, xmode->height, 0, bwin);
    }
    if (hdmiOn && !needReload) {
      ch_monitor_off_keep_ddc(i2cdev);
      audio_to_hdmi(0);
      hdmiOn = 0;
      if (verbose) printf("Monitor should be off!\n");
      sleep(YIELD_CHRONTEL_TURN_OFF_SECS);
    }
  }
  close(i2cdev);
  return 0;
}
