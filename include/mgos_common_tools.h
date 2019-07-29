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
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} tools_rgb_data;

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

void tools_rgb_to_hsv(tools_rgb_data in, double* h, double* s, double* v);
tools_rgb_data tools_hsv_to_rgb(double h, double s, double v);

int tools_get_random(int start, int end);
tools_rgb_data tools_get_random_color(tools_rgb_data start, tools_rgb_data* test, uint16_t count, double min_dist);
tools_rgb_data tools_get_random_color_fade(tools_rgb_data start, tools_rgb_data* test, uint16_t count, double min_dist, double s_new, double v_new);
double tools_check_color_distance(tools_rgb_data* start, uint16_t count, double h_test);
void tools_config_get_color(char* fmt, char* key, tools_rgb_data* color);
tools_rgb_data tools_color_wheel(double wheel_pos, double base);
tools_rgb_data tools_fade_color(tools_rgb_data start, double fade);

char* tools_config_get_dyn(const char* fmt, const char* key, bool do_lower);
char** tools_config_get_dyn_arr(const char* fmt, const char* key, size_t* elems);
uint32_t *tools_config_get_number_arr(const char* data, const char separator, uint32_t *result_count);
tools_rgb_data *tools_config_get_color_arr(const char* data, const char separator, uint32_t *result_count);
uint32_t tools_config_get_dyn_number(const char* fmt, const char* key);

void tools_scan_array(const char *str, int len, void *user_data);

void tools_set_color(tools_rgb_data* color, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
tools_rgb_data tools_hexcolor_to_rgb(uint32_t hex_val);
tools_rgb_data tools_hexcolor_str_to_rgb(char* hex_val);

int min(int a, int b);
int max(int a, int b);
double minval(double a, double b, double c);
double maxval(double a, double b, double c);

bool tools_common_tools_init(void);

#endif // __TOOLS_COMMON_TOOLS_H
