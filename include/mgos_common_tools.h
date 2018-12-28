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
#ifndef __tools_COMMON_TOOLS_H
#define __tools_COMMON_TOOLS_H

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
   char *keys;   
} tools_cb_data;

#define STA_IP_LEN 17

char *sta_dev_ip;
char *macAddr;
char *fsInfo;
//char *result_keys;
//char *result_object;

/*
typedef struct {
   char *key;
   int key_len;
   char *val;
   int val_len;
   enum json_token_type type;
   void *next;
} json_token_listitem;
*/

const char *tools_get_device_id();

const char *tools_get_device_ip();

char *tools_get_mac_addr_fmt(uint8_t sep);

bool tools_file_exists(char *file);

size_t tools_create_filepath(struct mbuf *res, char *path, char *file);

bool tools_file_move(char *file, char *source, char* target);

void tools_to_upper_case(char *txt);

void tools_to_lower_case(char *txt);

char *tools_to_hex(int num, int len);

char *tools_get_fs_info(const char *path);

/*
json_token_listitem *tool_json_tokenlist_put(json_token_listitem *list, struct json_token *key, struct json_token *val, bool skip);

json_token_listitem *tool_json_tokenlist_new(void);

void tool_json_tokenlist_free(json_token_listitem *list);

char *tool_json_get_keys(char *str_obj, int len);

char *tool_json_merge(char *obj_A, int len_A, char *obj_B, int len_B);

void tool_json_mergebuffer_free();

char *tool_json_tokenlist_convert(json_token_listitem *list);

char *tool_json_prepare_result(char *buf, int len);

void tool_json_free_result();
*/

bool tools_common_tools_init(void);

#endif // __tools_COMMON_TOOLS_H
