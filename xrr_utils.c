// Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/Xproto.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>

#include "xrr_utils.h"

#if RANDR_MAJOR > 1 || (RANDR_MAJOR == 1 && RANDR_MINOR >= 2)
#define HAS_RANDR_1_2 1
#else
#error Only tested with xrandr 1.2
#endif


static char modechar(int flags, int pos, int neg) {
  return (flags & pos) ? '+' : ((flags & neg) ? '-' : '?');
}

void showmode(XRRModeInfo *mode)
{
  printf("Mode %s (%x)\n", mode->name, (unsigned int)mode->id);
  printf("%dx%d, dot clock %ld  %cHsync %cVsync\n",
         mode->width, mode->height, mode->dotClock,
         modechar(mode->modeFlags, RR_HSyncPositive, RR_HSyncNegative),
         modechar(mode->modeFlags, RR_VSyncPositive, RR_VSyncNegative));
  printf("H: start %d, end %d, skew %d, total %d\n",
         mode->hSyncStart, mode->hSyncEnd, mode->hSkew, mode->hTotal);
  printf("V: start %d, end %d, total %d\n",
         mode->vSyncStart, mode->vSyncEnd, mode->vTotal);
}

XRRModeInfo *get_x_mode_info(Display *dpy, Window root)
{
  XRRScreenResources *resources;
  int out, md;
  RRCrtc activecrtc = 0;
  XRRCrtcInfo *cinfo;
  RRMode activemode;
  int major, minor;


  if (!XRRQueryVersion (dpy, &major, &minor)) {
    fprintf (stderr, "RandR extension missing\n");
    exit (1);
  }

  resources = XRRGetScreenResources (dpy, root);

  /* Assume device only uses LVDS output, so find it by what is enabled */
  /* It points to a crtc which will be in a mode */
  for(out = 0; out < resources->noutput; out++) {
    XRROutputInfo *oinfo = XRRGetOutputInfo(dpy, resources,
                                            resources->outputs[out]);
    if (oinfo->connection == RR_Connected) {
      activecrtc = oinfo->crtc;
      break;
    }
  }
  if (!activecrtc)
    return NULL;

  if (out >= resources->noutput) {
    fprintf(stderr, "Could not find active output\n");
    exit(1);
  }
  cinfo = XRRGetCrtcInfo(dpy, resources, activecrtc);
  /* Get the XID of the mode */
  activemode = cinfo->mode;
  for(md = 0; md < resources->nmode; md++) {
    if (resources->modes[md].id == activemode) break;
  }
  if (md >= resources->nmode) {
    fprintf(stderr, "Could not find mode ID %x\n", (unsigned int)activemode);
    exit(1);
  }
  return &(resources->modes[md]);
}
