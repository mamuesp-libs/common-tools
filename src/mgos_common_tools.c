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

#include "mgos_common_tools.h"

const char *tools_get_device_id()
{
  return mgos_sys_config_get_device_id();
}

const char *tools_get_device_ip()
{
  struct mgos_net_ip_info ip_info;
  memset(&ip_info, 0, sizeof(ip_info));
  memset(sta_dev_ip, 0, STA_IP_LEN);
  if (mgos_net_get_ip_info(MGOS_NET_IF_TYPE_WIFI, MGOS_NET_IF_WIFI_STA, &ip_info))
  {
    mgos_net_ip_to_str(&ip_info.ip, sta_dev_ip);
  }
  else if (mgos_net_get_ip_info(MGOS_NET_IF_TYPE_WIFI, MGOS_NET_IF_WIFI_AP, &ip_info))
  {
    mgos_net_ip_to_str(&ip_info.ip, sta_dev_ip);
  }
  return sta_dev_ip;
}

char *tools_get_mac_addr_fmt(uint8_t sep)
{
  const char *macRaw = mgos_sys_ro_vars_get_mac_address();
  int i, out = 0;

  for (i = 1; i <= 12; i++)
  {
    macAddr[out] = macRaw[i - 1];
    out++;
    if ((i % 2) == 0)
    {
      macAddr[out] = (char)sep;
      out++;
    }
  }
  macAddr[17] = 0;

  LOG(LL_DEBUG, ("getMacAddress: MAC address found: <%s>", macAddr));
  return macAddr;
}

bool tools_file_exists(char *file)
{
  FILE *test = fopen(file, "rb");
  bool isExisting = (test != NULL);

  if (isExisting)
  {
    fclose(test);
  }

  return isExisting;
}

size_t tools_create_filepath(struct mbuf *res, char *path, char *file)
{
  uint8_t end = 0;

  mbuf_init(res, strlen(path) + strlen(file) + 2);
  if (strlen(path) > 0)
  {
    mbuf_append(res, path, strlen(path));
    mbuf_append(res, "/", 1);
  }
  mbuf_append(res, file, strlen(file));
  mbuf_append(res, &end, 1);
  mbuf_trim(res);
  return res->len;
}

bool tools_file_move(char *file, char *source, char *target)
{
  struct mbuf src;
  struct mbuf tgt;
  size_t pos = 0;
  uint8_t ch;

  tools_create_filepath(&src, source, file);
  tools_create_filepath(&tgt, target, file);

  if (strcmp(src.buf, tgt.buf) == 0)
  {
    return true;
  }

  FILE *in = fopen(src.buf, "rb");
  FILE *out = fopen(tgt.buf, "wb");
  if (in == NULL || out == NULL)
  {
    LOG(LL_ERROR, ("tools_file_move: error opening files! <%s> - <%s>", src.buf, tgt.buf));
    in = out = 0;
    return false;
  }

  fseek(in, 0L, SEEK_END); // file pointer at end of file
  pos = ftell(in);
  fseek(in, 0L, SEEK_SET); // file pointer set at start
  while (pos--)
  {
    ch = fgetc(in); // copying file character by character
    fputc(ch, out);
  }

  fclose(out);
  fclose(in);

  if (remove(src.buf) < 0)
  {
    LOG(LL_ERROR, ("tools_file_move: error deleting file! <%s>", src.buf));
    return false;
  }

  mbuf_free(&src);
  mbuf_free(&tgt);

  return true;
}

void tools_to_upper_case(char *txt)
{
  // Convert to upper case
  char *s = txt;
  while (*s)
  {
    *s = toupper((unsigned char)*s);
    s++;
  }
}

void tools_to_lower_case(char *txt)
{
  // Convert to upper case
  char *s = txt;
  while (*s)
  {
    *s = tolower((unsigned char)*s);
    s++;
  }
}

char *tools_to_hex(int num, int len)
{
  // Convert to HEX
  char *s = json_asprintf("%0*x", len, num);
  tools_to_upper_case(s);
  LOG(LL_DEBUG, ("Number <%d> as HEX: <%s>", num, s));
  return s;
}

char *tools_get_fs_info(const char *path)
{
  return "{}";
  /*
  size_t ram_size = mgos_get_heap_size();
  size_t ram_free = mgos_get_free_heap_size();
  size_t ram_min_free = mgos_get_min_free_heap_size();
  size_t fs_memory_usage = mgos_get_fs_memory_usage();
  size_t fs_size = mgos_get_fs_size();
  size_t fs_free_size = mgos_get_free_fs_size();
  uint32_t cpu_freq =  mgos_get_cpu_freq();
  fsInfo = json_asprintf("{ ram_size: %u, ram_free: %u, ram_min_free: %u, root_fs_memory: %u, root_fs_size: %u, root_fs_free: %u, CPU_freq: %u }",
                        (unsigned int) ram_size,
                        (unsigned int) ram_free,
                        (unsigned int) ram_min_free,
                        (unsigned int) fs_memory_usage,
                        (unsigned int) fs_size,
                        (unsigned int) fs_free_size,
                        (unsigned int) cpu_freq);
  return fsInfo;
  */
}

/*
static char *get_token_type_name(enum json_token_type type) {
  char *result = "INACTIVE";
#ifdef TOOLS_DEBUG_ENUM  
  switch(type) {
    case JSON_TYPE_STRING:
      result = "JSON_TYPE_STRING";
      break;
    case JSON_TYPE_NUMBER:
      result = "JSON_TYPE_NUMBER";
      break;
    case JSON_TYPE_TRUE:
      result = "JSON_TYPE_TRUE";
      break;
    case JSON_TYPE_FALSE:
      result = "JSON_TYPE_FALSE";
      break;
    case JSON_TYPE_NULL:
      result = "JSON_TYPE_NULL";
      break;
    case JSON_TYPE_OBJECT_START:
      result = "JSON_TYPE_OBJECT_START";
      break;
    case JSON_TYPE_OBJECT_END:
      result = "JSON_TYPE_OBJECT_END";
      break;
    case JSON_TYPE_ARRAY_START:
      result = "JSON_TYPE_ARRAY_START";
      break;
    case JSON_TYPE_ARRAY_END:
      result = "JSON_TYPE_ARRAY_END";
      break;
    default:
      result = "JSON_TYPE_INVALID";
      break;
    }
#endif    
    return result;
}

json_token_listitem *tool_json_get_token(json_token_listitem *curr, struct json_token *key, struct json_token *val, json_token_listitem *next){
  if (curr->key != NULL) {
    free(curr->key);
  }
  curr->key = malloc(key->len);
  curr->key_len = key->len;
  memcpy(curr->key, key->ptr, key->len);
  
  if (curr->val != NULL) {
    free(curr->val);
  }
  curr->val = malloc(val->len);
  curr->val_len = val->len;
  memcpy(curr->val, val->ptr, val->len);
  curr->type = val->type;
  
  curr->next = next;
  
  return curr;
}

json_token_listitem *tool_json_tokenlist_put(json_token_listitem *list, struct json_token *key, struct json_token *val, bool skip) {
  json_token_listitem *curr = list;
  bool do_scan = true;

  while (do_scan) {
    if (curr->key == NULL) {
      // no entries yet ... we just take the item in
      tool_json_get_token(curr, key, val, NULL);
      // stop walking through
      do_scan = false;
    } else if (strncmp(curr->key, key->ptr, key->len) == 0) {
      if (skip == false) {
        // an item with the same key has been found, we overwrite it
        tool_json_get_token(curr, key, val, curr->next);
      }
      // stop walking through
      do_scan = false;
    } else if (curr->next != NULL) {
      // nothing to do, we move on in the list ...
      curr = (json_token_listitem *) curr->next;
    } else {
      // we are at the end of the list ... we add the item here
      curr->next = tool_json_get_token(tool_json_tokenlist_new(), key, val, NULL);
      curr = (json_token_listitem *) curr->next;
      // stop walking through
      do_scan = false;
    }
  }
  return curr;
}

json_token_listitem *tool_json_tokenlist_new(void) {
  json_token_listitem *result = malloc(sizeof(json_token_listitem));
  memset(result, 0, sizeof(json_token_listitem));
  return result;
}

void tool_json_tokenlist_free(json_token_listitem *list) {
  json_token_listitem *curr_item;
  while (list != NULL) {
    // store the current item ...
    curr_item = list;
    // ... and set the list pointer to the next item
    list = list->next;
    // if set, free the current token
    if (curr_item->key != NULL) {
      memset(curr_item->key, 0, curr_item->key_len);
      free(curr_item->key);
      curr_item->key_len = 0;
    }
    if (curr_item->val != NULL) {
      memset(curr_item->val, 0, curr_item->val_len);
      free(curr_item->val);
      curr_item->val_len = 0;
    }
    // now free the current item itself
    memset(curr_item, 0, sizeof(json_token_listitem));
    free(curr_item);
  }
}

char *tool_json_get_keys(char *str_obj, int len) {
  struct mbuf key_buff;
  struct mbuf *mjs_keys = &key_buff;
  void *h = NULL;
  struct json_token key, val;
  char *result;
  mbuf_init(mjs_keys, 0);
  while ((h = json_next_key(str_obj, len, h, "", &key, &val)) != NULL) {
    if (mjs_keys->len > 0) {
      mbuf_append(mjs_keys, ",", 1);
    };
    mbuf_append(mjs_keys, key.ptr, key.len);
    LOG(LL_DEBUG, ("tool_json_get_keys: [%.*s] -> [%.*s]", key.len, key.ptr, val.len, val.ptr));
  }
  result = tool_json_prepare_result(mjs_keys->buf, mjs_keys->len);
  mbuf_free(mjs_keys);
  memset(mjs_keys, 0, sizeof(struct mbuf));
  return result;
}

char *tool_json_merge(char *obj_A, int len_A, char *obj_B, int len_B) {
  void *h = NULL;
  json_token_listitem *token_list = tool_json_tokenlist_new();
  struct json_token *key = malloc(sizeof(struct json_token));
  struct json_token *val = malloc(sizeof(struct json_token));
  while ((h = json_next_key(obj_B, len_B, h, "", key, val)) != NULL) {
    json_token_listitem *curr = tool_json_tokenlist_put(token_list, key, val, false);
    LOG(LL_DEBUG, ("tool_json_merge: B -> [%.*s] -> [%.*s]", curr->key_len, curr->key, curr->val_len, curr->val));
    // get new key and val items
    key = malloc(sizeof(struct json_token));
    val = malloc(sizeof(struct json_token));
  }
  while ((h = json_next_key(obj_A, len_A, h, "", key, val)) != NULL) {
    json_token_listitem *curr = tool_json_tokenlist_put(token_list, key, val, true);
    LOG(LL_DEBUG, ("tool_json_merge: B -> [%.*s] -> [%.*s]", curr->key_len, curr->key, curr->val_len, curr->val));
    // get new key and val items
    key = malloc(sizeof(struct json_token));
    val = malloc(sizeof(struct json_token));
  }
  return tool_json_tokenlist_convert(token_list);
}

char *tool_json_tokenlist_convert(json_token_listitem *list) {
  struct mbuf res_buf;
  mbuf_init(&res_buf, 0);
  mbuf_append(&res_buf, "{", 1);
  json_token_listitem *curr = list;
  while (curr != NULL) {
    LOG(LL_DEBUG, ("tool_json_tokenlist_convert: key -> [%.*s], val -> [%.*s] - type [%s]", curr->key_len, curr->key, curr->val_len, curr->val, get_token_type_name(curr->type)));
    mbuf_append(&res_buf, "\"", 1);
    mbuf_append(&res_buf, curr->key, curr->key_len);
    mbuf_append(&res_buf, "\":", 2);
    if (curr->type == JSON_TYPE_STRING) {
      mbuf_append(&res_buf, "\"", 1);
    }
    mbuf_append(&res_buf, curr->val, curr->val_len);
    if (curr->type == JSON_TYPE_STRING) {
      mbuf_append(&res_buf, "\"", 1);
    }
    curr = curr->next;
    if (curr != NULL) {
      mbuf_append(&res_buf, ",", 1);
    }
  }
  mbuf_append(&res_buf, "}", 1);
  tool_json_mergebuffer_free();
  result_object = malloc(res_buf.len + 1);
  sprintf(result_object, "%.*s", res_buf.len, res_buf.buf);
  LOG(LL_DEBUG, ("tool_json_tokenlist_convert: result -> [%s]", result_object));
  mbuf_free(&res_buf);
  return result_object;
}

void tool_json_mergebuffer_free(){
  if (result_object != NULL)
  {
    free(result_object);
    result_object = NULL;
  }
}

char *tool_json_prepare_result(char *buf, int len)
{
  tool_json_free_result();
  result_keys = malloc(len + 1);
  memset(result_keys, 0, len + 1);
  strncpy(result_keys, buf, len);
  LOG(LL_DEBUG, ("tools_mjs_prepare_result: Result <%s> - len <%d>", result_keys, len));
  return result_keys;
}

  void tool_json_free_result() {
    if (result_keys != NULL) {
      free(result_keys);
      result_keys = NULL;
    }
  }
*/
bool mgos_common_tools_init(void)
{

  if (!mgos_sys_config_get_common_tools_enable())
  {
    return true;
  }

  macAddr = malloc(64);
  memset(macAddr, 0, 64);
  fsInfo = malloc(256);
  memset(fsInfo, 0, 256);
  sta_dev_ip = malloc(STA_IP_LEN);
  memset(sta_dev_ip, 0, STA_IP_LEN);

  return true;
}
