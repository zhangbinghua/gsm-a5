# SPDX-License-Identifier: GPL-2.0
CC = gcc

# Architecture
ARCH := $(shell getconf LONG_BIT)

CFLAGS = -I./

SRC := print_bin.c bitmap.c convert_hex.c

# Config

ifeq ($(ARCH),64)
CFLAGS += -DCONFIG_64BIT
endif

all: convert_hex

convert_hex: $(SRC)
	$(CC) $(SRC) $(CFLAGS) -o $@

clean:
	@rm -rf *.o convert_hex > /dev/null
