# Copyright (c) 2010 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.


CC ?= gcc
CCFLAGS ?= -Wall -Werror
PKG_CONFIG ?= pkg-config

INCLUDE_DIRS = $(shell $(PKG_CONFIG) --cflags x11 xrandr)
LIB_DIRS = $(shell $(PKG_CONFIG) --libs x11 xrandr xext alsa libcras)

MON=ch7036_monitor
BUG=ch7036_debug
OBJECTS=ch7036_access.o GenTableCH7036.o xrr_utils.o audio_utils.o

all: $(MON) $(BUG)

.c.o:
	$(CC) $(CCFLAGS) $(INCLUDE_DIRS) -c $< -o $@

$(MON): $(OBJECTS) $(MON).o
	$(CC) $(CCFLAGS) $(INCLUDE_DIRS) $(LIB_DIRS) $^ $(LIBS) $(LDFLAGS) \
                -o $@

$(BUG): $(OBJECTS) $(BUG).o
	$(CC) $(CCFLAGS) $(INCLUDE_DIRS) $(LIB_DIRS) $^ $(LIBS) $(LDFLAGS) \
                -o $@

clean:
	@rm -f $(MON) $(BUG) $(OBJECTS)
