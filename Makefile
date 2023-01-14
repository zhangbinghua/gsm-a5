# SPDX-License-Identifier: GPL-2.0
CC=gcc

# Architecture
ARCH := $(shell getconf LONG_BIT)

CFLAGS = -I./

SRC := print_bin.c bitmap.c gsm-a5.c

# Config

ifeq ($(ARCH),64)
CFLAGS += -DCONFIG_64BIT
endif

all: gsm-a5

gsm-a5: $(SRC)
	$(CC) $(SRC) $(CFLAGS) -o $@

clean:
	@rm -rf *.o gsm-a5 > /dev/null
