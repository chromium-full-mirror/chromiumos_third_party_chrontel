// Copyright (c) 2010 The Chromium OS Authors. All rights reserved.
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


#include "ch7036.h"
#include "edid_utils.h"
#include "xrr_utils.h"

#define MAX_EDID_EXT 4

#define DEF_DEV "/dev/i2c-2"
#define DEF_FW "/lib/firmware/chrontel/fw7036.bin"


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
	  "Usage: %s {-v|-p][-d<i2c device>|-n][-f<firmware>][-Ew,a]\n", prog);
  fprintf(stderr, "-d<filename>: set i2c device to use (default %s)\n", 
	  DEF_DEV);
  fprintf(stderr, "-f<filename>: set firmware to use (default %s)\n", DEF_FW);
  fprintf(stderr, "-n: Use dummy i2c device\n");
  fprintf(stderr, "-v: Verbose, print every I2C access\n");
  fprintf(stderr, "-p: Probe: check I2C access and Chrontel ID and exit.\n");
  fprintf(stderr, "-E<width>,<aspect>: Force aspet and max width\n");
  
  exit(1);
}



int get_native(unsigned char *data, struct timing_ch7036* prefer, int* aspect,
               int verbose)
{
  int seen_pref = 0;
  int interlace = 0;
  int i;

  for(i = 0; i < 4; i++) {
    int pelclk = data[54 + 18*i] + (data[55 + 18*i]<<8);
    
    if ((pelclk != 0)) {
      int hres = data[56 + 18*i] + ((data[58 + 18*i] & 0xf0)<<4);
      int hbl = data[57 + 18*i] + ((data[58 + 18*i] & 0x0f)<<8);
      int vres = data[59 + 18*i] + ((data[61 + 18*i] & 0xf0)<<4);
      int vbl = data[60 + 18*i] + ((data[61 + 18*i] & 0x0f)<<8);
      int hso = data[62 + 18*i] + ((data[65 + 18*i] & 0xc0)<<2);
      int hsw = data[63 + 18*i] + ((data[65 + 18*i] & 0x30)<<4);
      int vso = (data[64 + 18*i] >> 4) + ((data[65 + 18*i] & 0x0c)<<2);
      int vsw = (data[64 + 18*i] & 0xf)+ ((data[65 + 18*i] & 0x03)<<4);
      int hsiz = data[66 + 18*i] + ((data[68 + 18*i] & 0xf0)<<4);
      int vsiz = data[67 + 18*i] + ((data[68 + 18*i] & 0x0f)<<8);
      // int hbdr = data[69 + 18*i];
      // int vbdr = data[70 + 18*i];
      int mdflg = data[71 + 18*i];

      //int refr = (pelclk * 10000)/((hres+hbl)*(vres+vbl));
      //int refm = (pelclk * 10000)%((hres+hbl)*(vres+vbl));
      //int refd = (refm*100)/((hres+hbl)*(vres+vbl));

      /* Seen some devices that list interlaced mode first */
      /* If they also have non-interlaced then use that    */
      if ((!seen_pref) || 
	  (seen_pref && interlace && ((mdflg & 0x80) == 0))) {
	prefer->clk_khz = pelclk * 10;
	/* FIXME mdhayter need to look for extension for DVI/HDMI */
	prefer->pixel_fmt = PIXEL_HDMI_ENCODE_DVI;
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
            printf("Didn't find aspect from %dx%d, trying size %dx%d",
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
	  }
	}
	/* First encountered is the preferred and native */
	seen_pref = 1;
      }
    }
  }
  return seen_pref;
}

int get_standard(unsigned char *data, 
		 struct timing_ch7036 **mon, int *found_aspect,
		 int force_maxw, int force_aspect, int verbose)
{
  int curbest_aspect = -1;
  struct timing_ch7036 *curbest = NULL;
  int i, md;
  int native_aspect;

  if (force_maxw > 0)
    native_aspect = force_aspect;
  else
    native_aspect = find_aspect_fromisize(data);

  for(i=0; i < 11; i++) {
    int hinfo = 0x01;
    int vinfo = 0x01;
    int hres, vres;

    if (i < 8) { hinfo = data[38+2*i]; vinfo = data[39+2*i];}
    /* 8-10 check established timing 60Hz entries */ 
    if ((i == 8)  && (data[35] & 0x20)) { hinfo = (640-248)/8;  vinfo = 0x40; }
    if ((i == 9)  && (data[35] & 0x01)) { hinfo = (800-248)/8;  vinfo = 0x40; }
    if ((i == 10) && (data[36] & 0x08)) { hinfo = (1024-248)/8; vinfo = 0x40; }

    /* 01 01 is pad by spec, but 00 00 and 20 20 are see in wild */
    if (((hinfo == 0x01) && (vinfo == 0x01)) ||
	((hinfo == 0x00) && (vinfo == 0x00)) ||
	((hinfo == 0x20) && (vinfo == 0x20)))
      continue;
    /* FIXME only doing 60Hz */
    if ((vinfo & 0x3f) != 0) continue;
    
    hres = (hinfo * 8) + 248;
    if (verbose) printf("Found %d wide %s\n", hres, aspect_to_str[vinfo>>6]);
    if ((force_maxw > 0) && (hres > force_maxw)) continue;

    if (verbose) printf("Try %d wide %s\n", hres, aspect_to_str[vinfo>>6]);

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
    }

    for(md = 0; md < N_KNOWN_MODES; md++) {
      if ((known_modes[md].ha == hres) && (known_modes[md].va == vres)) break;
    }
    if (md < N_KNOWN_MODES) {
      /* Found a mode that we know and monitor can do */
      if ((curbest == NULL) ||
	  (((vinfo >> 6) == native_aspect) && 
	   ((curbest_aspect != native_aspect) || (hres > curbest->ha)))) {
	/* We had nothing, or we found first or bigger native aspect */
	if (verbose) printf("Better mode width %d, aspect %s\n",
                            hres, aspect_to_str[vinfo>>6]);
	curbest = known_modes + md;
	curbest_aspect = (vinfo >> 6);
      } else if (curbest_aspect != native_aspect) {
	
	if ((native_aspect == ASPECT_16_10) && 
	    ((vinfo >> 6) == ASPECT_16_9) &&
	    ((curbest_aspect != ASPECT_16_9) || (hres > curbest->ha))) {
	  /* Looking for 16:10, found first 16:9 or bigger 16:9 */
	  if (verbose) printf("Want 16:10 use 16:9 width %d, aspect %s\n",
                              hres, aspect_to_str[vinfo>>6]);
	  curbest = known_modes + md;
	  curbest_aspect = (vinfo >> 6);
	} else if (hres > curbest->ha) {
	  if (verbose) printf("Better based on size: width %d, aspect %s\n",
                              hres, aspect_to_str[vinfo>>6]);
	  curbest = known_modes + md;
	  curbest_aspect = (vinfo >> 6);
	}	  
      }
    }
  }
  if (curbest != NULL) {
    *mon = curbest;
    *found_aspect = curbest_aspect;
    return 1;
  }
  return 0;
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



int main(int argc, char *argv[])
{
  int i2cdev;
  char *i2cfile = DEF_DEV;
  char *fwfile = DEF_FW;
  int arg;
  int err;
  Display *dpy;
  Window root;
  int screen;
  Window bwin;
  XRRModeInfo *xmode;
  int forcen_x = 0;
  int forcen_a = 0;
  int probe_only = 0;
  int dummy_i2c = 0;
  int verbose = 0;
  int *width_fix;
  unsigned char edid[256];
  int aspect;
  int hdmiOn, seenOff;
  int needReload = 1;   /* Force firmware to load on the first loop */
  int newFirmware = 1;

  struct reg_ch7036 *reglist;

  struct timing_ch7036 lcd_timing;
  struct timing_ch7036 prefer;
  struct timing_ch7036 *mon_timing;

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
      {
	char *endn;

	forcen_x = strtol(argv[arg] + 2, &endn, 10);
	if (endn[0] != ',') {
	  fprintf(stderr, "Expecting comma (-E<max_width>,<aspect>) in %s\n",
		  argv[arg]);
	  usage(argv[0]);
	  forcen_x = 0;
	} else
	  forcen_a = strtol(endn + 1, &endn, 10);
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

  dpy = XOpenDisplay(":0.0");
  if (dpy == NULL) {
    fprintf(stderr, "%s: Can't open display :0.0\n", argv[0]);
    exit(1);
  }
  screen = DefaultScreen (dpy);
  root = RootWindow (dpy, screen);
  xmode = get_x_mode_info(dpy, root);
  if (verbose) showmode(xmode);

  if ((xmode->width == 1366) && (xmode->height == 768)) {
    width_fix = width_fix_1366x768;
  } else if ((xmode->width == 1280) && (xmode->height == 800)) {
    width_fix = width_fix_1280x800;
  } else
    width_fix = NULL;

  lcd_timing.clk_khz = xmode->dotClock/1000;
  lcd_timing.pixel_fmt = PIXEL_FMT_18BIT;
  lcd_timing.hs_pol = (xmode->modeFlags & RR_HSyncPositive) ? 1 : 0;
  lcd_timing.vs_pol = (xmode->modeFlags & RR_VSyncPositive) ? 1 : 0;
  lcd_timing.de_pol = 1;

  lcd_timing.ht = xmode->hTotal;
  lcd_timing.ha = xmode->width;
  lcd_timing.ho = xmode->hSyncStart - xmode->width;
  lcd_timing.hw = xmode->hSyncEnd - xmode->hSyncStart;
  lcd_timing.vt = xmode->vTotal;
  lcd_timing.va = xmode->height;
  lcd_timing.vo = xmode->vSyncStart - xmode->height;
  lcd_timing.vw = xmode->vSyncEnd - xmode->vSyncStart;
  lcd_timing.scale = MK_SCALE(0, 0);

  while (1) {
    int new_width;

    if (needReload) {
      int mcutry = 0;
      /* This also intializes the changed vector */
      ch_reset_todefault(i2cdev);

      err = ch_load_firmware(i2cdev, fwfile);
      if (err < 0) {
        fprintf(stderr, "%s: Could not load firmware %s\n", argv[0], fwfile);
        exit(1);
      }
      newFirmware = 1;
      needReload = 0;
      do {
        if (ch_mcu_version(i2cdev, 1, verbose) > 0) break;
        sleep(1);
        mcutry++;
        if (mcutry >= 5) needReload = 1;
      } while (mcutry < 5);
      /* HDMI detection seems a bit bouncy after firmware load */
      /* If there was a sleep added here it would delay detection */
      /* at startup time, so don't do that */
      /* Using the longer ch_mcu_version timeout makes it less liekly */
      /* Turning the monitor on here seems to make the problem go away */
      /* FIXME: but it causes LCD flicker */
      ch_monitor_on(i2cdev);
      ch_monitor_off_keep_ddc(i2cdev);
    }

    seenOff = 0;
    while (!needReload && !ch_hdmi_detected(i2cdev)) {
      sleep(5);
      /* Check the Chrontel firmware is still alive */
      /* Originally only did this when DPMS said screen came on */
      /* But that misses the case where lid close takes system to s3 */
      if (ch_mcu_version(i2cdev, 0, 0) < 0) {
        if (verbose) printf("Lost the Chrontel firmware\n");
        needReload = 1;
      }
    }
    /* Recycle the outer while loop to hit the fw reload code */
    if (needReload) continue;

    err = ch_get_edid(i2cdev, 0, &edid[0]);
    mon_timing = NULL;
    if (err < 0) {
      if (ch_mcu_version(i2cdev, 0, verbose) < 0) {
        if (verbose) printf("EDID error because Chrontel firmware lost\n");
        needReload = 1;
        continue;
      }
      /* EDID timed out, but things seem alive */
      if (newFirmware) {
        if (verbose)
          printf("Ignore HPD because EDID failed just after firmware load\n");
        sleep(5);
        /* Loop back to recheck HPD after sleep */
        continue;
      }
      fprintf(stderr, "%s: Get EDID Failed (initial block) using 640x480@60\n",
              argv[0]);
    } else if (!edid_valid(&edid[0])) {
      if (newFirmware) {
        if (verbose)
          printf("Ignore HPD since EDID not valid just after firmware load\n");
        sleep(5);
        /* Loop back to recheck HPD after sleep */
        continue;
      }
      fprintf(stderr, "%s: EDID is not valid, using 640x480@60\n",
              argv[0]);
    } else {
      if (edid[126] > 0)
        ch_get_edid(i2cdev, 1, &edid[128]);

      if (verbose) {
        show_edid_data(&edid[0], 128, 0);
        if (edid[126] > 0)
          show_edid_data(&edid[128], 128, 128);
        show_edid(&edid[0], (edid[126] > 0) ? 1:0);
      }
      if ((forcen_x == 0) && get_native(&edid[0], &prefer, &aspect, verbose))
	mon_timing = &prefer;
      else 
	get_standard(&edid[0], &mon_timing, &aspect, forcen_x, forcen_a, 
                     verbose);
    }

    newFirmware = 0;

    do {
      if (mon_timing == NULL) {
        if (verbose)
          printf("%s: Fall back to default output 640x480@60\n", argv[0]);
	/* First in table is default 640x480@60 */
	mon_timing = known_modes;
	aspect = ASPECT_4_3;
      }
      if (verbose) {
	show_ch_timing(&lcd_timing, "input (laptop lcd)");
	show_ch_timing(mon_timing, "output (hdmi port)");
      }
      if (width_fix != NULL)
	new_width = width_fix[aspect];
      else
	new_width = xmode->width;

      lcd_timing.ha = new_width;
      lcd_timing.ho = xmode->hSyncStart - new_width;

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

    if (new_width != xmode->width)
      bwin = x_start_blank(dpy, screen, root, xmode->width, xmode->height);

    if (verbose) printf("Restore defaults:\n");
    ch_restore_notin_seq(i2cdev, reglist);
    if (verbose) printf("Program registers:\n");
    ch_write_reg_sequence(i2cdev, reglist);
    /* Chrontel code seems to set the AVI infoframe for TV (overscan) */
    /* this causes some monitors to crop the edges and resize         */
    /* Fix here to use Computer (underscan) mode                      */
    ch_write_reg(i2cdev, CH7036_AVIINFO3, 0x12);
    ch_calculate_incs(i2cdev);
    ch_monitor_on(i2cdev);
    hdmiOn = 1;

    if (new_width != xmode->width)
      set_x_size(dpy, root, new_width, xmode->height, 1, bwin);
    if (verbose) printf("Monitor should be on!\n");
    do {
      CARD16 state;
      BOOL onoff;

      /* Check the Chrontel firmware is still alive */
      /* Moved to outer loop because lid switch yanks power and kills fw */
      /* without any chance for powerd to do it nicely */
      if (ch_mcu_version(i2cdev, 0, 0) < 0) {
        if (verbose) printf("Lost the Chrontel firmware\n");
        needReload = 1;
      } else {
        DPMSInfo(dpy, &state, &onoff);
        if ((state != DPMSModeOn) && hdmiOn) {
          if (verbose) printf("DPMS is not on, turn off hdmi\n");
          ch_monitor_off_keep_ddc(i2cdev);
          hdmiOn = 0;
        } else if ((state == DPMSModeOn) && !hdmiOn) {
          if (verbose) printf("DPMS is on, turn on hdmi\n");
          ch_monitor_on(i2cdev);
          if (verbose) printf("Monitor back on with DPMS On\n");
          hdmiOn = 1;
        }
      }
      /* If we need a reload, don't wait! */
      if (!needReload) sleep(5);
    } while (!needReload && ch_hdmi_detected(i2cdev));
    if (verbose) printf("Exit displaying needReload = %d, hdmiOn = %d\n", 
                        needReload, hdmiOn);
    if (new_width != xmode->width)
      set_x_size(dpy, root, xmode->width, xmode->height, 0, bwin);
    if (hdmiOn && !needReload) {
      ch_monitor_off_keep_ddc(i2cdev);
      hdmiOn = 0;
      if (verbose) printf("Monitor should be off!\n");
      sleep(2);
    }
  }
  close(i2cdev);
  return 0;
}

    
