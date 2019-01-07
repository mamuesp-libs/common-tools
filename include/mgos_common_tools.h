/**
 *
 * Copyright (c) 2018 Manfred Mueller-Spaeth <fms1961@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 * A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **/
#ifndef __TOOLS_COMMON_TOOLS_H
#define __TOOLS_COMMON_TOOLS_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "frozen.h"
#include "mgos.h"
#include "mgos_app.h"
#include "mgos_config.h"
#include "mgos_ro_vars.h"
#include "mgos_rpc.h"

typedef struct {
    char* keys;
} tools_cb_data;

#define STA_IP_LEN 17

char* sta_dev_ip;
char* macAddr;
char* fsInfo;

const char* tools_get_device_id();

const char* tools_get_device_ip();

char* tools_get_mac_addr_fmt(uint8_t sep);

bool tools_file_exists(char* file);

size_t tools_create_filepath(struct mbuf* res, char* path, char* file);

bool tools_file_move(char* file, char* source, char* target);

void tools_to_upper_case(char* txt);

void tools_to_lower_case(char* txt);

bool tools_to_hex(int num, int len, char* out);

char* tools_get_fs_info(const char* path);

void tools_hex_dump(void* addr, int len, int log_type, char* out, int out_len, bool show_ascii);

int min(int a, int b);
int max(int a, int b);

bool tools_common_tools_init(void);

#endif // __TOOLS_COMMON_TOOLS_H
