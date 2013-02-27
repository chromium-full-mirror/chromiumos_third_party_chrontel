// Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <assert.h>
#include <dirent.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cras_client.h>

#include "audio_utils.h"

static const size_t MAX_IONODES = 20;
static int aud_verbose = 0;
static int hdmi_audio_available;

static struct cras_client *client;
static enum {
  CLIENT_INIT,
  CLIENT_CREATED,
  CLIENT_CONNECTED,
  CLIENT_THREAD_RUN,
  CLIENT_READY,
} client_status;

static int connect_to_server()
{
  int err = 0;
  switch (client_status) {
  case CLIENT_INIT:
    err = cras_client_create(&client);
    if (err) {
      printf("Audio: failed to create client\n");
      return err;
    }
    client_status = CLIENT_CREATED;
    // fallthrough
  case CLIENT_CREATED:
    err = cras_client_connect(client);
    if (err) {
      printf("Audio: failed to connect to server\n");
      return err;
    }
    client_status = CLIENT_CONNECTED;
    // fallthrough
  case CLIENT_CONNECTED:
    err = cras_client_run_thread(client);
    if (err) {
      printf("Audio: failed to run client thread\n");
      return err;
    }
    client_status = CLIENT_THREAD_RUN;
    // fallthrough
  case CLIENT_THREAD_RUN:
    err = cras_client_connected_wait(client);
    if (err) {
      printf(stderr, "Audio: failed to wait connected.\n");
      return err;
    }
    client_status = CLIENT_READY;
    printf("Audio: client ready\n");
    // fallthrough
  case CLIENT_READY:
  default:
    return 0;
  }
}

int audio_init(int verbose)
{
  int err = 0;
  aud_verbose = verbose;
  if (aud_verbose) printf("Audio: init\n");
  connect_to_server();
  return 0;
}

void audio_to_hdmi(int available)
{
  size_t num_devs = 0;
  size_t num_nodes = MAX_IONODES;
  struct cras_ionode_info nodes[MAX_IONODES];
  int rc, i;

  if (available == hdmi_audio_available)
    return;
  if (aud_verbose) printf("Audio: Set HDMI audio output to %s\n",
                          available ? "Available" : "Unavailable");
  hdmi_audio_available = available;
  if (connect_to_server() != 0)
    return;

  rc = cras_client_get_output_devices(client, NULL, nodes, &num_devs,
                                      &num_nodes);
  if (rc < 0) {
    printf("Audio: cannot get output nodes\n");
    return;
  }

  for (i = 0; i < num_nodes; i++) {
    if (!strcmp(nodes[i].name, "IEC958")) {
      cras_client_set_node_attr(client, nodes[i].iodev_idx,
                                nodes[i].ionode_idx, IONODE_ATTR_PLUGGED,
                                available);
      return;
    }
  }

  printf("Audio: IEC959 node is not found");
}
