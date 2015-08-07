// Copyright 2015 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __DRM_UTILS_H
#define __DRM_UTILS_H

int
drm_init(void);

void
drm_done(void);

int
drm_get_mode(drmModeModeInfoPtr mode, int *dpms);

void
drm_show_mode(drmModeModeInfoPtr mode);


#endif /* __DRM_UTILS_H */
