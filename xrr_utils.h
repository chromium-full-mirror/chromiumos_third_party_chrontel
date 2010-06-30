// Copyright (c) 2010 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __XRR_UTILS_H__
#define __XRR_UTILS_H__

void showmode(XRRModeInfo *mode);
XRRModeInfo *get_x_mode_info(Display *dpy, Window root);

#endif
