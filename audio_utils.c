// Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <assert.h>
#include <alsa/asoundlib.h>
#include <dirent.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "audio_utils.h"

#ifdef USE_ALSA_CONTROL
static int aud_verbose = 0;
static struct aud_jack_event hp_jack;
static char card[64] = "default";

static int aud_switch(enum aud_cmd cmd, const char *name, int *value)
{
  int err = 0;
  snd_mixer_t *handle;
  snd_mixer_selem_id_t *sid;
  snd_mixer_elem_t *elem;
  snd_mixer_selem_id_alloca(&sid);

  if ((err = snd_mixer_open(&handle, 0)) < 0) {
    aud_errno("mixer %s open", card);
    return err;
  }
  if ((err = snd_mixer_attach(handle, card)) < 0) {
    aud_errno("mixer attach %s", card);
    snd_mixer_close(handle);
    return err;
  }
  if ((err = snd_mixer_selem_register(handle, NULL, NULL)) < 0) {
    aud_errno("mixer register");
    snd_mixer_close(handle);
    return err;
  }
  if ((err = snd_mixer_load(handle)) < 0) {
    aud_errno("mixer %s load", card);
    snd_mixer_close(handle);
    return err;
  }
  for (elem = snd_mixer_first_elem(handle); elem;
       elem = snd_mixer_elem_next(elem)) {
    snd_mixer_selem_get_id(elem, sid);
    if (!(strcmp(snd_mixer_selem_id_get_name(sid), name))) {
      if (aud_verbose)
        printf("Audio: found %s audio switch\n", name);
      if (snd_mixer_selem_has_playback_switch(elem)) {
        switch (cmd) {
          case get:
            err = snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_MONO,
                                                      value);
            if (err < 0)
              aud_errno("getting %s", name);
            break;
          case set:
            err = snd_mixer_selem_set_playback_switch_all(elem, *value);
            if (err < 0)
              aud_errno("setting %s -> %d", name, *value);
            break;
          default:
            aud_error("unknown switch cmd %d for %s\n", cmd, name);
            err = -1;
            break;
        }
      }
    }
  }
  snd_mixer_close(handle);
  return err;
}

static int set_speaker_switch(int value)
{
  int *pval = &value;
  return aud_switch(set, "Speaker", pval);
}

static int set_spdif_switch(int value)
{
  int *pval = &value;
  return aud_switch(set, "IEC958", pval);
}

static int open_jack_event(int etype)
{
  DIR *ev_dir;
  struct dirent *ev_entry;
  int fd = -1;

  assert(etype & SW_HEADPHONE_INSERT);

  if((ev_dir = opendir(INPUT_DIR)) == NULL) {
    aud_errno("opening dir %s", INPUT_DIR);
    return -1;
  }
  if (chdir(INPUT_DIR)) {
    aud_errno("chdir to %s", INPUT_DIR);
    return -1;
  }

  while ((ev_entry = readdir(ev_dir))) {
    if ((ev_entry->d_type == DT_CHR) &&
        !(strncmp(ev_entry->d_name, "event", 5))) {

      if (access(ev_entry->d_name, R_OK) < 0) {
        aud_errno("read access %s", ev_entry->d_name);
        return -1;
      }

      if ((fd = open(ev_entry->d_name, O_RDONLY)) < 0) {
        aud_errno("opening event %s", ev_entry->d_name);
        return -1;
      }

      unsigned long events[NBITS(EV_MAX)];
      memset(events, 0, sizeof(events));
      if (ioctl(fd, EVIOCGBIT(0, EV_MAX), events) < 0) {
        aud_errno("ioctl EVIOCGBIT for events");
        return -1;
      }

      if (IS_BIT_SET(EV_SW, events)) {
        unsigned long sw_events[NBITS(SW_MAX)];
        memset(sw_events, 0, sizeof(sw_events));

        if (ioctl(fd, EVIOCGBIT(EV_SW, SW_MAX), sw_events) < 0) {
          aud_errno("ioctl EVIOCGBIT for SW events");
          return -1;
        }
        if (IS_BIT_SET(etype, sw_events)) {
          break;
        }
      }
    }
  }
  return fd;
}

static int open_jack_hp_event(struct aud_jack_event *jack) {
  jack->etype = SW_HEADPHONE_INSERT;
  jack->fd = open_jack_event(SW_HEADPHONE_INSERT);
  jack->status = unknown;
  return jack->fd;
}

static void get_jack_status(struct aud_jack_event *jack) {
  unsigned long events[NBITS(SW_MAX)];
  assert(jack->fd > 0);
  memset(events, 0, sizeof(events));
  ioctl(jack->fd, EVIOCGSW(sizeof(events)), events);
  jack->status = IS_BIT_SET(hp_jack.etype, events);
}

static void close_jack_event(struct aud_jack_event *jack)
{
  close(jack->fd);
}

#define FORCE_MUTE_FILE "/var/run/chrontel/forced_mute"

static int get_force_mute()
{
  return (access(FORCE_MUTE_FILE, R_OK) == 0);
}

static void enable_hdmi_audio()
{
  int fd;
  do {
    fd = creat(FORCE_MUTE_FILE, S_IRUSR | S_IWUSR);
  } while ((fd == -1) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)));
  if (fd == -1) {
    aud_errno("creating %s", FORCE_MUTE_FILE);
  } else {
    close(fd);
  }
  set_speaker_switch(0);
  if (aud_verbose) printf("Audio: muted Speaker\n");
  set_spdif_switch(1);
  if (aud_verbose) printf("Audio: enabled SPDIF\n");
}

static void disable_hdmi_audio()
{
  int err;
  do {
    err = remove(FORCE_MUTE_FILE);
  } while ((err == -1 ) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)));
  if (err == -1)
    aud_errno("removing %s", FORCE_MUTE_FILE);
  set_speaker_switch(1);
  if (aud_verbose) printf("Audio: unmuted Speaker\n");
  set_spdif_switch(0);
  if (aud_verbose) printf("Audio: disabled SPDIF\n");
}

#endif /* USE_ALSA_CONTROL */

int audio_init(int verbose)
{
  int err = 0;
#ifdef USE_ALSA_CONTROL

  aud_verbose = verbose;
  if ((err = open_jack_hp_event(&hp_jack)) < 0) {
    aud_error("opening hp jack event\n");
  } else {
    get_jack_status(&hp_jack);
  }

#endif
  return err;
}

void audio_to_hdmi(int enable)
{
#ifdef USE_ALSA_CONTROL

  if (aud_verbose) printf("Audio: Set HDMI audio output to %s\n",
                          enable ? "Enable" : "Disable");

  if (enable) {
    get_jack_status(&hp_jack);
    if (aud_verbose) printf("Audio: headphone is %s\n",
                            hp_jack.status ? "plugged-in" : "unplugged");
    if (hp_jack.status == unplugged)
      enable_hdmi_audio();
    else if (get_force_mute())
      disable_hdmi_audio();
  } else {
    if (get_force_mute())
      disable_hdmi_audio();
  }

#endif
}

void audio_check_jack(int enable)
{
#ifdef USE_ALSA_CONTROL
  if (enable == 0)
    return;

  enum jack_status cur_status = hp_jack.status;
  get_jack_status(&hp_jack);
  if (hp_jack.status != cur_status)
    audio_to_hdmi(enable);

#endif
}
