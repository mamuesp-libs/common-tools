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

bool tools_to_hex(int num, int len, char *out)
{
  // Convert to HEX
  //char *s = json_asprintf("%0*x", len, num);
  sprintf(out, "%0*x", len, num);
  tools_to_upper_case(out);
  LOG(LL_DEBUG, ("Number <%d> as HEX: <%s>", num, out));
  return true;
}

char *tools_get_fs_info(const char *path)
{
  return "{}";
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
}

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
