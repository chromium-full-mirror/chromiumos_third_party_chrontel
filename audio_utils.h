// Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __AUDIO_UTILS_H__
#define __AUDIO_UTILS_H__

#include <strings.h>
#include <errno.h>

#define aud_error(...) do {                                             \
  fprintf(stderr, "Audio: Error: %s:%u :: ", __FILE__, __LINE__);       \
  fprintf(stderr, __VA_ARGS__); } while (0)

#define aud_errno(...) do {                                             \
  fprintf(stderr, "Audio: Error: %s:%u :: ", __FILE__, __LINE__);       \
  fprintf(stderr, __VA_ARGS__);                                         \
  fprintf(stderr, ", err:%s\n", strerror(errno));} while (0)

int audio_init(int verbose);
void audio_to_hdmi(int enable);

#endif
