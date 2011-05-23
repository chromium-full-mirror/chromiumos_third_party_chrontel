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

static Display *dpy;
static Window root;
static int screen;


void usage(char *prog)
{
  fprintf(stderr, "Usage: %s [-d<i2c device>|-n] [i|d|h|m|o] ...\n", prog);
  fprintf(stderr, "-d: set i2c device to use (default %s)\n", DEF_DEV);
  fprintf(stderr, "-g: set gpio device to use (default none)\n");
  fprintf(stderr, "-n: Use dummy i2c device\n");
  fprintf(stderr, "-v: Verbose, print every I2C access\n");
  fprintf(stderr, "c - calculate and write in INC values\n");
  fprintf(stderr, "m - monitor on\n");
  fprintf(stderr, "o - monitor off (keep ddc)\n");
  fprintf(stderr, "O - monitor off (power down ddc)\n");
  fprintf(stderr, "r - reset and release datapath\n");
  fprintf(stderr, "R - reset and release chip, regs go to defaults\n");
  fprintf(stderr, "l <file> - Load firmware from file\n");
  fprintf(stderr, "v - show MCU firmware version\n");
  fprintf(stderr, "e - read and dump EDID\n");
  fprintf(stderr, "E - dump test EDIDs\n");
  fprintf(stderr, "a - wait for gpio to signal attatch\n");
  fprintf(stderr, "d - wait for gpio to signal dettatch\n");
  fprintf(stderr, "W<page>,<reg>,<value> - Write CH7036 register\n");
  fprintf(stderr, "G<page>,<reg> - Get CH7036 register\n");

  exit(1);
}

int main(int argc, char *argv[])
{
  int i2cdev;
  int gpiodev = 0;
  char *i2cfile = DEF_DEV;
  char *gpiofile = NULL;
  int arg;
  int err;
  XRRModeInfo *xmode;

  int dummy_i2c = 0;


  for(arg=1; arg < argc; arg++) {
    if ((argv[arg][0]) != '-') break;
    switch (argv[arg][1]) {

    default:
      fprintf(stderr, "Unknown option: %s\n", argv[arg]);
      usage(argv[0]);

    case 'n':
      dummy_i2c = 1;
      break;

    case 'v':
      ch7036_debugi2c = 1;
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
    }
  }

  if (dummy_i2c)
    i2cdev = -1;
  else {
    i2cdev = open(i2cfile, O_RDWR);

    if (i2cdev < 0) {
      fprintf(stderr, "Could not open %s: %s\n", i2cfile, strerror(errno));
      exit(1);
    }
  }

  if (gpiofile != NULL) {
    gpiodev = open(gpiofile, O_RDONLY);
    if (gpiodev < 0) {
      fprintf(stderr, "Could not open %s: %s\n", gpiofile, strerror(errno));
      exit(1);
    }
  }

  /* CH7036 only has one slave address option */
  if (dummy_i2c)
    err = 0;
  else
    err = ioctl(i2cdev, I2C_SLAVE, CH7036_I2C_ADDR);

  if (err < 0) {
    fprintf(stderr, "Could not set slave address 0x%x: %s\n",
            CH7036_I2C_ADDR, strerror(errno));
    exit(1);
  }

  /* Read the information */
  err = ch_read_reg(i2cdev, CH7036_DEVID);
  fprintf(stderr, "Found device ID 0x%02x\n", err);

  err = ch_read_reg(i2cdev, CH7036_HDMI_ST);

  fprintf(stderr, "HDMI status 0x%02x: %s\n", err,
          (err & 0x10) ? "connected (or firmware not running)":"not detected");

  dpy = XOpenDisplay(":0.0");
  if (dpy == NULL) {
    fprintf(stderr, "Can't open display :0.0\n");
    exit(1);
  }
  screen = DefaultScreen (dpy);
  root = RootWindow (dpy, screen);
  xmode = get_x_mode_info(dpy, root);
  showmode(xmode);

  for(;arg < argc; arg++) {
    switch(argv[arg][0]) {

    default:
      fprintf(stderr,"Unknown command letter %s\n", argv[arg]);
      usage(argv[0]);
      break;

    case 'c':
      printf("Calc INCs\n");
      ch_calculate_incs(i2cdev);
      break;

    case 'm':
      printf("MONITOR on\n");
      ch_monitor_on(i2cdev, 1, 1);
      break;

    case 'O':
      printf("MONITOR off (and power down ddc)\n");
      ch_monitor_off(i2cdev);
      break;

    case 'o':
      printf("MONITOR off (keep ddc alive)\n");
      ch_monitor_off_keep_ddc(i2cdev);
      break;

    case 'W':
      {
        int page, reg, value;
        char *endn;

        page = strtol(argv[arg] + 1, &endn, 0);
        if (endn[0] != ',') {
          fprintf(stderr, "Expecting 1st comma (W<page>,<reg>,<value>) in %s\n",
                  argv[arg]);
          break;
        }
        reg = strtol(endn + 1, &endn, 0);
        if (endn[0] != ',') {
          fprintf(stderr, "Expecting 2nd comma (W<page>,<reg>,<value>) in %s\n",
                  argv[arg]);
          break;
        }
        value = strtol(endn + 1, &endn, 0);
        if (endn != argv[arg]) argv[arg] = endn - 1;
        printf("Write 0x%x, 0x%x, 0x%x\n", page, reg, value);
        ch_write_reg(i2cdev, page, reg, value);
        break;
      }

    case 'G':
      {
        int page, reg, value;
        char *endn;

        page = strtol(argv[arg] + 1, &endn, 0);
        if (endn[0] != ',') {
          fprintf(stderr, "Expecting 1st comma (G<page>,<reg>) in %s\n",
                  argv[arg]);
          break;
        }
        reg = strtol(endn + 1, &endn, 0);
        if (endn != argv[arg]) argv[arg] = endn - 1;
        value = ch_read_reg(i2cdev, page, reg);
        printf("Read 0x%x, 0x%x --> 0x%x\n", page, reg, value);
        break;
      }

    case 'r':
      printf("RESET datapath\n");
      ch_reset_datapath(i2cdev);
      break;

    case 'R':
      printf("RESET to defaults\n");
      ch_reset_todefault(i2cdev);
      break;

    case 'l':
      if (arg > (argc-1)) {
        fprintf(stderr, "No firmware filename found\n");
        usage(argv[0]);
        break;
      }
      printf("LOAD firmware %s\n", argv[arg+1]);
      ch_load_firmware(i2cdev, argv[arg+1]);
      arg++;
      /* Make sure continuation check fails below */
      if (argv[arg][0])
        argv[arg][1] = 0;
      else
        argv[arg]--;
      break;

    case 'v':
      printf("Get MCU firmware version\n");
      ch_mcu_version(i2cdev, 1, 1);
      break;

    case 'E':
      {
        int i = 1;

        while (show_test_edid(stdout, i) == 0) i++;
        break;
    }
    case 'e':
      {
        int ext, i;
        unsigned char edid[MAX_EDID_EXT*128];

        printf("Get EDID\n");
        err = ch_get_edid(i2cdev, 0, &edid[0]);
        if (err < 0) {
          fprintf(stderr, "Get EDID Failed (initial block)\n");
          exit(1);
        }
        show_edid_data(stdout, &edid[0], 128, 0);
        ext = edid[126]; // Number of extension blocks
        if (ext > MAX_EDID_EXT) {
          fprintf(stderr, "Too many EDID extensions (%d), using %d\n",
                  ext, MAX_EDID_EXT);
          ext = MAX_EDID_EXT;
        }
        for(i = 1; i <= ext; i++) {
          err = ch_get_edid(i2cdev, i, &edid[i*128]);
          if (err < 0) {
            fprintf(stderr, "Get EDID Failed (extension block %d)\n", i);
            exit(1);
          }
          show_edid_data(stdout, &edid[i*128], 128, i * 128);
        }
        show_edid(stdout, &edid[0], ext);
      }
      break;
    case 'a':
      {
        if (!gpiodev)
          fprintf(stderr, "No gpio device to detect attach\n");
        else {
          while (ch_hdmi_gpio_detected(gpiodev, 1) == 0)
            sleep(YIELD_GPIO_DETECT_SECS);
        }
        break;
      }
    case 'd':
      {
        if (!gpiodev)
          fprintf(stderr, "No gpio device to detect detatch\n");
        else {
          while (ch_hdmi_gpio_detected(gpiodev, 1) == 1)
            sleep(YIELD_GPIO_DETECT_SECS);
        }
        break;
      }
    }
    if (argv[arg][1] != 0) {
      argv[arg]++;
      arg--;
    }
  }

  close(i2cdev);
  if (gpiodev) close(gpiodev);
  return 0;
}
