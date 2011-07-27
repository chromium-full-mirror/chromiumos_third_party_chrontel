// Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __AUDIO_UTILS_H__
#define __AUDIO_UTILS_H__

#include <strings.h>
#include <errno.h>

#define INPUT_DIR "/dev/input"
#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define IS_BIT_SET(bit, array)  ((array[LONG(bit)] >> OFF(bit)) & 1)

#define aud_error(...) do {                                             \
  fprintf(stderr, "Audio: Error: %s:%u :: ", __FILE__, __LINE__);       \
  fprintf(stderr, __VA_ARGS__); } while (0)

#define aud_errno(...) do {                                             \
  fprintf(stderr, "Audio: Error: %s:%u :: ", __FILE__, __LINE__);       \
  fprintf(stderr, __VA_ARGS__);                                         \
  fprintf(stderr, ", err:%s\n", strerror(errno));} while (0)

enum aud_cmd {
  get=0,
  set=1,
};

enum jack_status {
  unplugged=0,
  plugged=1,
  unknown=2,
};

struct aud_jack_event {
  int fd;
  int etype;
  enum jack_status status;
};

int audio_init(int verbose);
void audio_to_hdmi(int enable);
void audio_check_jack(int enable);

#endif
