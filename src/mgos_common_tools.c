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

const char* tools_get_device_id()
{
    return mgos_sys_config_get_device_id();
}

const char* tools_get_device_ip()
{
    struct mgos_net_ip_info ip_info;
    memset(&ip_info, 0, sizeof(struct mgos_net_ip_info));
    memset(sta_dev_ip, 0, STA_IP_LEN);
    if (mgos_net_get_ip_info(MGOS_NET_IF_TYPE_WIFI, MGOS_NET_IF_WIFI_STA, &ip_info)) {
        mgos_net_ip_to_str(&ip_info.ip, sta_dev_ip);
    } else if (mgos_net_get_ip_info(MGOS_NET_IF_TYPE_WIFI, MGOS_NET_IF_WIFI_AP, &ip_info)) {
        mgos_net_ip_to_str(&ip_info.ip, sta_dev_ip);
    }
    return sta_dev_ip;
}

char* tools_get_mac_addr_fmt(uint8_t sep)
{
    const char* macRaw = mgos_sys_ro_vars_get_mac_address();
    int i, out = 0;

    for (i = 1; i <= 12; i++) {
        macAddr[out] = macRaw[i - 1];
        out++;
        if ((i % 2) == 0) {
            macAddr[out] = (char)sep;
            out++;
        }
    }
    macAddr[17] = 0;

    LOG(LL_DEBUG, ("getMacAddress: MAC address found: <%s>", macAddr));
    return macAddr;
}

bool tools_file_exists(char* file)
{
    FILE* test = fopen(file, "rb");
    bool isExisting = (test != NULL);

    if (isExisting) {
        fclose(test);
    }

    return isExisting;
}

size_t tools_create_filepath(struct mbuf* res, char* path, char* file)
{
    uint8_t end = 0;

    mbuf_init(res, strlen(path) + strlen(file) + 2);
    if (strlen(path) > 0) {
        mbuf_append(res, path, strlen(path));
        mbuf_append(res, "/", 1);
    }
    mbuf_append(res, file, strlen(file));
    mbuf_append(res, &end, 1);
    mbuf_trim(res);
    return res->len;
}

bool tools_file_move(char* file, char* source, char* target)
{
    struct mbuf src;
    struct mbuf tgt;
    size_t pos = 0;
    uint8_t ch;

    tools_create_filepath(&src, source, file);
    tools_create_filepath(&tgt, target, file);

    if (strcmp(src.buf, tgt.buf) == 0) {
        return true;
    }

    FILE* in = fopen(src.buf, "rb");
    FILE* out = fopen(tgt.buf, "wb");
    if (in == NULL || out == NULL) {
        LOG(LL_ERROR, ("tools_file_move: error opening files! <%s> - <%s>", src.buf, tgt.buf));
        in = out = 0;
        return false;
    }

    fseek(in, 0L, SEEK_END); // file pointer at end of file
    pos = ftell(in);
    fseek(in, 0L, SEEK_SET); // file pointer set at start
    while (pos--) {
        ch = fgetc(in); // copying file character by character
        fputc(ch, out);
    }

    fclose(out);
    fclose(in);

    if (remove(src.buf) < 0) {
        LOG(LL_ERROR, ("tools_file_move: error deleting file! <%s>", src.buf));
        return false;
    }

    mbuf_free(&src);
    mbuf_free(&tgt);

    return true;
}

void tools_to_upper_case(char* txt)
{
    // Convert to upper case
    char* s = txt;
    while (*s) {
        *s = toupper((unsigned char)*s);
        s++;
    }
}

void tools_to_lower_case(char* txt)
{
    // Convert to upper case
    char* s = txt;
    while (*s) {
        *s = tolower((unsigned char)*s);
        s++;
    }
}

bool tools_to_hex(int num, int len, char* out)
{
    // Convert to HEX
    //char *out = json_asprintf("%0*x", len, num);
    sprintf(out, "%0*x", len, num);
    tools_to_upper_case(out);
    LOG(LL_DEBUG, ("Number <%d> as HEX: <%s>", num, out));
    return true;
}

char* tools_get_fs_info(const char* path)
{
    (void)path;
    return "{}";
    size_t ram_size = mgos_get_heap_size();
    size_t ram_free = mgos_get_free_heap_size();
    size_t ram_min_free = mgos_get_min_free_heap_size();
    size_t fs_memory_usage = mgos_get_fs_memory_usage();
    size_t fs_size = mgos_get_fs_size();
    size_t fs_free_size = mgos_get_free_fs_size();
    uint32_t cpu_freq = mgos_get_cpu_freq();
    fsInfo = json_asprintf("{ ram_size: %u, ram_free: %u, ram_min_free: %u, root_fs_memory: %u, root_fs_size: %u, root_fs_free: %u, CPU_freq: %u }",
        (unsigned int)ram_size,
        (unsigned int)ram_free,
        (unsigned int)ram_min_free,
        (unsigned int)fs_memory_usage,
        (unsigned int)fs_size,
        (unsigned int)fs_free_size,
        (unsigned int)cpu_freq);
    return fsInfo;
}

void tools_hex_dump(void* addr, int len, int log_type, char* out, int out_len, bool show_ascii)
{
    int i;
    char line[18];
    unsigned char* pc = (unsigned char*)addr;
    struct mbuf out_buf;
    mbuf_init(&out_buf, 0);

    // Traverse the data byte wise
    for (i = 0; i < len; i++) {
        // we show 16 bytes per line in hex
        if ((i % 16) == 0) {
            if (i > 0) {
                // add a linebreak, but not for the first (empty) line
                mbuf_append(&out_buf, "\n", 1);
            }
            // add the data offset in hex
            mbuf_append(&out_buf, "  ", 2);
            sprintf(line, "%04X", i);
            mbuf_append(&out_buf, line, 4);
            mbuf_append(&out_buf, " ", 1);
        }

        // add the hex code of the current byte
        sprintf(line, " 0x%02X", pc[i]);
        mbuf_append(&out_buf, line, 5);

        // if wished, we add an ASCII character if printable, else a dot "."
        if (show_ascii) {
            mbuf_append(&out_buf, " ", 1);
            if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
                mbuf_append(&out_buf, ".", 1);
            } else {
                mbuf_append(&out_buf, &pc[i], 1);
            }
        }
    }

    // padding the last line so itPad out last line if not exactly 16 characters.
    /*
    while ((i++ % 16) != 0) {
        mbuf_append(&out_buf, " ", 1);
    }
    */
    // the last linebreak
    mbuf_append(&out_buf, "\n", 1);

    // if log output is wished (log_type is > -1)
    if (log_type > -1) {
        LOG(log_type, ("\n%.*s", out_buf.len, out_buf.buf));
    }

    // if needed, we copy the data into a given buffer
    if (out != NULL && out_len > 0) {
        memcpy(out, out_buf.buf, min(out_buf.len, out_len));
    }

    mbuf_free(&out_buf);
}

int min(int a, int b)
{
    return (((a) < (b)) ? (a) : (b));
}

int max(int a, int b)
{
    return (((a) > (b)) ? (a) : (b));
}

double minval(double a, double b, double c)
{
    double first = (((a) < (b)) ? (a) : (b));
    return (((first) < (c)) ? (first) : (c));
}

double maxval(double a, double b, double c)
{
    double first = (((a) > (b)) ? (a) : (b));
    return (((first) > (c)) ? (first) : (c));
}

bool mgos_common_tools_init(void)
{

    if (!mgos_sys_config_get_common_tools_enable()) {
        return true;
    }

    srand(time(NULL)); // Initialization, should only be called once.
    macAddr = malloc(64);
    memset(macAddr, 0, 64);
    fsInfo = malloc(256);
    memset(fsInfo, 0, 256);
    sta_dev_ip = malloc(STA_IP_LEN);
    memset(sta_dev_ip, 0, STA_IP_LEN);

    return true;
}

int tools_str_split(const char* str, char c, char*** arr)
{
    int count = 1;
    int token_len = 1;
    int i = 0;
    char* p;
    char* t;

    if (str == NULL || strlen(str) == 0) {
        return 0;
    }

    p = (char*)str;
    while (*p != '\0') {
        if (*p == c)
            count++;
        p++;
    }

    *arr = (char**)malloc(sizeof(char*) * count);
    if (*arr == NULL)
        exit(1);
    memset(*arr, 0, sizeof(char*) * count);

    p = (char*)str;
    while (*p != '\0') {
        if (*p == c) {
            (*arr)[i] = (char*)malloc(token_len + 1);
            if ((*arr)[i] == NULL)
                exit(1);
            memset((*arr)[i], 0, token_len + 1);
            token_len = 0;
            i++;
        }
        p++;
        token_len++;
    }
    (*arr)[i] = (char*)malloc(token_len + 1);
    if ((*arr)[i] == NULL)
        exit(1);
    memset((*arr)[i], 0, token_len + 1);

    i = 0;
    p = (char*)str;
    t = ((*arr)[i]);
    while (*p != '\0') {
        if (*p != c && *p != '\0') {
            *t = *p;
            t++;
        } else {
            *t = '\0';
            i++;
            t = ((*arr)[i]);
        }
        p++;
    }

    return count;
}

bool tools_str_split_free(char** arr, size_t len)
{
    size_t i;
    if (arr) {
        for (i = 0; i < len; i++) {
            free(arr[i]);
        }
        free(arr);
        return true;
    }
    return false;
}

char* tools_config_get_dyn(const char* fmt, const char* key, bool do_lower)
{
    const struct mgos_conf_entry* schema = mgos_config_schema();
    struct mg_str dynKey;
    struct mg_str dynVal;
    char* out_key;
    char* lower_key;

    if (do_lower) {
        mg_asprintf(&lower_key, 0, "%s", key);
        tools_to_lower_case(lower_key);
        key = lower_key;
    }

    mg_asprintf(&out_key, 0, fmt, key);
    dynKey = mg_mk_str(out_key);
    mgos_config_get(dynKey, &dynVal, &mgos_sys_config, schema);
    free(out_key);
    if (do_lower && lower_key != NULL) {
        free(lower_key);
    }
    return (char*)dynVal.p;
}

uint32_t tools_config_get_dyn_number(const char* fmt, const char* key)
{
    char* str_result = tools_config_get_dyn(fmt, key, false);
    uint32_t result = 0;
    if (str_result != NULL) {
        result = (uint32_t)atoi(str_result);
    }
    return result;
}

char** tools_config_get_dyn_arr(const char* fmt, const char* key, size_t* elems)
{
    char* subs = tools_config_get_dyn(fmt, key, false);
    char** values = NULL;
    elems[0] = tools_str_split(subs, ',', &values);
    free(subs);
    if (elems[0] == 0) {
        return NULL;
    }
    return values;
}

char** tools_config_get_string_arr(const char* data, const char separator, uint32_t* result_count)
{
    char** str_results;
    char** results;
    *result_count = tools_str_split(data, separator, &str_results);
    results = *result_count > 0 ? calloc(*result_count, sizeof(char*)) : NULL;
    for (uint32_t i = 0; i < *result_count; i++) {
        mg_asprintf(&results[i], 0, "%s", str_results[i]);
    }
    tools_str_split_free(str_results, *result_count);
    return results;
}

void tools_free_string_arr(char** data, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++) {
        free(data[i]);
    }
    free(data);
}

tools_char_array* tools_create_char_array(uint32_t size)
{
    tools_char_array* result = calloc(1, sizeof(tools_char_array));
    result->data = calloc(size, sizeof(char*));
    result->len = size;
    return result;
}

void tools_free_char_array(tools_char_array* arr)
{
    for (uint32_t i = 0; i < arr->len; i++) {
        if (arr->data[i] != NULL) {
            free(arr->data[i]);
            arr->data[i] = NULL;
        }
    }
    free(arr->data);
    arr->data = NULL;
    arr->len = 0;
    free(arr);
}

tools_num_array* tools_create_num_array(uint32_t size)
{
    tools_num_array* result = calloc(1, sizeof(tools_num_array));
    result->data = calloc(size, sizeof(uint32_t));
    result->len = size;
    return result;
}

void tools_free_num_arr(tools_num_array* arr)
{
    free(arr->data);
    free(arr);
}

bool tools_is_in_num_arr(tools_num_array* arr, uint32_t test)
{
    for (size_t i = 0; i < arr->len; i++) {
        if (arr->data[i] == test) {
            return true;
        }
    }
    return false;
}

tools_num_tree* tools_create_num_tree(uint32_t size)
{
    tools_num_tree* result = calloc(1, sizeof(tools_num_tree));
    result->data = calloc(size, sizeof(tools_num_array*));
    result->len = size;
    return result;
}

tools_char_array* tools_config_get_char_arr(const char* data, const char separator)
{
    char** str_results;
    uint32_t size = tools_str_split(data, separator, &str_results);
    tools_char_array* results = tools_create_char_array(size);
    for (uint32_t i = 0; i < results->len; i++) {
        size_t dat_len = strlen(str_results[i]);
        results->data[i] = calloc(1, dat_len + 1);
        strncpy(results->data[i], str_results[i], dat_len);
        results->data[i][dat_len] = 0;
    }
    tools_str_split_free(str_results, results->len);
    return results;
}

tools_num_array* tools_config_get_number_arr(const char* data, const char separator)
{
    char** str_results;
    uint32_t size = tools_str_split(data, separator, &str_results);
    tools_num_array* results = tools_create_num_array(size);
    for (uint32_t i = 0; i < results->len; i++) {
        results->data[i] = (uint32_t)atoi(str_results[i]);
    }
    tools_str_split_free(str_results, results->len);
    return results;
}

tools_num_tree* tools_config_get_number_tree(const char* data, const char separator)
{
    char** str_groups;
    uint32_t size = tools_str_split(data, separator, &str_groups);
    tools_num_tree* results = tools_create_num_tree(size);
    for (uint32_t i = 0; i < results->len; i++) {
        results->data[i] = tools_config_get_number_arr(str_groups[i], ',');
    }
    tools_str_split_free(str_groups, results->len);
    return results;
}

void tools_text_dump_array(void* array, const char* type, const char* name, enum cs_log_level level)
{
    if (strcmp(type, "tools_num_array") == 0) {
        tools_num_array* num_array = (tools_num_array*)array;
        for (uint32_t i = 0; i < num_array->len; i++) {
            LOG(level, ("Content of '%s (type: %s)' - Len: %lu, Data: %lu", name, type, (unsigned long)num_array->len, (unsigned long)num_array->data[i]));
        }
        // } else if (strcmp(type, "tools_rgb_array") == 0) {
        //     tools_rgb_array* rgb_array = (tools_rgb_array*)array;
        //     for (int i = 0; i < rgb_array->len; i++) {
        //         LOG(level, ("Content of '%s (type: %s)' - Len: %d, Data: [R:0x%0X, G:0x%0X, B:0x%0X]", name, type, rgb_array->len, rgb_array->data[i].r, rgb_array->data[i].g, rgb_array->data[i].b));
        //     }
    } else if (strcmp(type, "tools_char_array") == 0) {
        tools_char_array* char_array = (tools_char_array*)array;
        for (uint32_t i = 0; i < char_array->len; i++) {
            LOG(level, ("Content of '%s (type: %s)' - Len: %lu, Data: %s", name, type, (unsigned long)char_array->len, char_array->data[i]));
        }
    } else if (strcmp(type, "tools_num_tree") == 0) {
        tools_num_tree* num_tree = (tools_num_tree*)array;
        for (uint32_t i = 0; i < num_tree->len; i++) {
            tools_num_array* curr_array = num_tree->data[i];
            for (uint32_t j = 0; j < curr_array->len; j++) {
                LOG(level, ("Content of '%s (type: %s)' - Len: %lu, Data: %lu", name, type, (unsigned long)curr_array->len, (unsigned long)curr_array->data[j]));
            }
        }
    }
}

void tools_scan_array(const char* str, int len, void* user_data)
{
    struct json_token t;
    uint8_t* array = (uint8_t*)user_data;
    int i;
    LOG(LL_DEBUG, ("Parsing array: %.*s", len, str));
    for (i = 0; json_scanf_array_elem(str, len, "", i, &t) > 0; i++) {
        array[i] = ((uint8_t)*t.ptr) - '0';
        LOG(LL_DEBUG, ("Index %d, token [%.*s]", i, t.len, t.ptr));
    }
}

int tools_get_random(int start, int end)
{
    float rand = mgos_rand_range(start, end);
    return (int)round(rand);
}