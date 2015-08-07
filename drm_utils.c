// Copyright 2015 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <xf86drm.h>
#include <xf86drmMode.h>

#include "drm_utils.h"

static int fd = -1;
static drmModeResPtr res = NULL;

int
drm_init(void)
{
	int ret;

	ret = drmOpen("i915", NULL);
	if (ret < 0)
		return ret;
	fd = ret;
	/* give up drm master in case we are first */
	drmDropMaster(fd);
	res = drmModeGetResources(fd);
	if (!res) {
		drmClose(fd);
		fd = -1;
		return -1;
	}
	return 0;
}

void
drm_done(void)
{
	if (res) {
		drmModeFreeResources(res);
		res = NULL;
	}

	if (fd >= 0) {
		drmClose(fd);
		fd = -1;
	}
}

static drmModeConnectorPtr
find_connector(drmModeCrtcPtr crtc)
{
	int c;

	for (c = 0; c < res->count_connectors; c++) {
		drmModeConnectorPtr connector;
		drmModeEncoderPtr encoder;
		connector = drmModeGetConnector(fd, res->connectors[c]);
		if (!connector)
			continue;
		encoder = drmModeGetEncoder(fd, connector->encoder_id);
		if (!encoder) {
			drmModeFreeConnector(connector);
			continue;
		}
		if (encoder->crtc_id == crtc->crtc_id) {
			/* got it */
			drmModeFreeEncoder(encoder);
			return connector;
		}
		drmModeFreeEncoder(encoder);
		drmModeFreeConnector(connector);
	}
	return NULL;
}

static int get_dpms(drmModeConnectorPtr connector, int *dpms)
{
	int p;

	if (!connector) {
		*dpms = DRM_MODE_DPMS_OFF;
		return 0;
	}

	for (p = 0; p < connector->count_props; p++) {
		drmModePropertyPtr prop;
		prop = drmModeGetProperty(fd, connector->props[p]);

		if (prop) {
			if (!strcmp(prop->name, "DPMS")) {
				*dpms = (int)connector->prop_values[p];
				drmModeFreeProperty(prop);
				return 0;
			}
			drmModeFreeProperty(prop);
		}
	}

	/* no DPMS property, assume connector is on */
	*dpms = DRM_MODE_DPMS_ON;
	return 0;
}

int
drm_get_mode(drmModeModeInfoPtr mode, int *dpms)
{
	int c;
	int found = 0;

	if (fd < 0)
		return -ENODEV;

	/* find a crtc with a valid mode and return the info */
	for (c = 0; c < res->count_crtcs && !found; c++) {
		drmModeCrtcPtr crtc;
		crtc = drmModeGetCrtc(fd, res->crtcs[c]);
		if (crtc) {
			if (crtc->buffer_id && crtc->mode_valid) {
				drmModeConnectorPtr connector;
				*mode = crtc->mode;
				found = 1;

				connector = find_connector(crtc);
				get_dpms(connector, dpms);
				if (connector) {
					drmModeFreeConnector(connector);
				}
			}
			drmModeFreeCrtc(crtc);
		}
	}

	if (!found) {
		mode->htotal = mode->vtotal = 0;
		*dpms = DRM_MODE_DPMS_OFF;
	}
	return 0;
}

static char modechar(int flags, int pos, int neg) {
  return (flags & pos) ? '+' : ((flags & neg) ? '-' : '?');
}

void
drm_show_mode(drmModeModeInfoPtr mode)
{
	printf("Mode %s\n", mode->name);
	printf("%dx%d, dot clock %ld KHz  %cHsync %cVsync\n",
	       mode->hdisplay, mode->vdisplay, mode->clock,
	       modechar(mode->flags, DRM_MODE_FLAG_PHSYNC, DRM_MODE_FLAG_NHSYNC),
	       modechar(mode->flags, DRM_MODE_FLAG_PVSYNC, DRM_MODE_FLAG_NVSYNC));
	printf("H: start %d, end %d, skew %d, total %d\n",
	       mode->hsync_start, mode->hsync_end, mode->hskew, mode->htotal);
	printf("V: start %d, end %d, total %d\n",
	       mode->vsync_start, mode->vsync_end, mode->vtotal);
}
