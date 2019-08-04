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
#include <math.h>
#include "frozen.h"
#include "mgos.h"
#include "mgos_wifi.h"
#include "mgos_app.h"
#include "mgos_config.h"
#include "mgos_ro_vars.h"

typedef struct {
    void* data;
    uint32_t len;
} tools_array;

typedef struct {
    char** data;
    uint32_t len;
} tools_char_array;

typedef struct {
    uint32_t* data;
    uint32_t len;
} tools_num_array;

typedef struct {
    tools_num_array** data;
    uint32_t len;
} tools_num_tree;

typedef struct {
    char* keys;
} tools_cb_data;

#define STA_IP_LEN 17

char* sta_dev_ip;
char* macAddr;
char* fsInfo;

bool tools_str_split_free(char** arr, size_t len);

int tools_str_split(const char* str, char c, char*** arr);

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

int tools_get_random(int start, int end);

char* tools_config_get_dyn(const char* fmt, const char* key, bool do_lower);
char** tools_config_get_dyn_arr(const char* fmt, const char* key, size_t* elems);
uint32_t tools_config_get_dyn_number(const char* fmt, const char* key);

tools_char_array *tools_create_char_array(uint32_t size);
void tools_free_char_array(tools_char_array* arr);
tools_char_array *tools_config_get_char_arr(const char* data, const char separator);

tools_num_array *tools_create_num_array(uint32_t size);
void tools_free_num_arr(tools_num_array* arr);
bool tools_is_in_num_arr(tools_num_array* arr, uint32_t test);
tools_num_array *tools_config_get_number_arr(const char* data, const char separator);

tools_num_tree *tools_create_num_tree(uint32_t size);
tools_num_tree *tools_config_get_number_tree(const char* data, const char separator);

void tools_text_dump_array(void* array, const char* type, const char* name, enum cs_log_level level);

void tools_scan_array(const char *str, int len, void *user_data);

int min(int a, int b);
int max(int a, int b);
double minval(double a, double b, double c);
double maxval(double a, double b, double c);

bool tools_common_tools_init(void);

#endif // __TOOLS_COMMON_TOOLS_H
