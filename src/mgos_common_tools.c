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

typedef struct {
    double r; // a fraction between 0 and 1
    double g; // a fraction between 0 and 1
    double b; // a fraction between 0 and 1
} rgb;

typedef struct {
    double h; // angle in degrees
    double s; // a fraction between 0 and 1
    double v; // a fraction between 0 and 1
} hsv;

static hsv rgb2hsv(rgb in);
static rgb hsv2rgb(hsv in);

static hsv rgb2hsv(rgb in)
{
 /*
    hsv out;

    double max = maxval(in.r, in.g, in.b);
    double min = minval(in.r, in.g, in.b);

    out.h = max;
    out.s = max;
    out.v = max;

    double d = max - min;
    out.s = (max == 0.0) ? 0.0 : (d / max);
    if (max == min) {
        out.h = 0; // achromatic
    } else {
        if (max == in.r) {
            out.h = (in.g - in.b) / (d + (in.g < in.b ? 6.0 : 0.0));
        } else if (max == in.g) {
            out.h = (in.b - in.r) / (d + 2.0);
        } else if (max == in.b) {
            out.h = (in.r - in.g) / (d + 4.0);
        }
        out.h = out.h / 6.0;
    }

    return out;
*/
    hsv out;
    double min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min < in.b ? min : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max > in.b ? max : in.b;

    out.v = max; // v
    delta = max - min;
    if (delta < 0.00001) {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max); // s
    } else {
        // if max is 0, then r = g = b = 0
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN; // its now undefined
        return out;
    }
    if (in.r >= max) // > is bogus, just keeps compilor happy
        out.h = (in.g - in.b) / delta; // between yellow & magenta
    else if (in.g >= max)
        out.h = 2.0 + (in.b - in.r) / delta; // between cyan & yellow
    else
        out.h = 4.0 + (in.r - in.g) / delta; // between magenta & cyan

    out.h *= 60.0; // degrees

    if (out.h < 0.0)
        out.h += 360.0;

    return out;

}

static rgb hsv2rgb(hsv in)
{
    rgb out;

    // range of 0.0 .. 1.0 needed ...
    in.h /= 360.0;

    int i = (int)floor(in.h * 6.0);
    double f = in.h * 6.0 - i;
    double p = in.v * (1.0 - in.s);
    double q = in.v * (1.0 - f * in.s);
    double t = in.v * (1.0 - (1.0 - f) * in.s);

    switch (i % 6) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;
    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }

    return out;
}

void tools_rgb_to_hsv(tools_rgb_data in, double* h, double* s, double* v)
{
    rgb d_in;
    hsv out;

    d_in.r = in.r / 255.0;
    d_in.g = in.g / 255.0;
    d_in.b = in.b / 255.0;

    out = rgb2hsv(d_in);

    *h = out.h;
    *s = out.s;
    *v = out.v;
}

tools_rgb_data tools_hsv_to_rgb(double h, double s, double v)
{
    tools_rgb_data out;
    rgb d_out;
    hsv in;

    in.h = h;
    in.s = s;
    in.v = v;

    d_out = hsv2rgb(in);

    out.r = (uint8_t)round(d_out.r * 255.0);
    out.g = (uint8_t)round(d_out.g * 255.0);
    out.b = (uint8_t)round(d_out.b * 255.0);

    return out;
}

tools_rgb_data tools_color_wheel(double wheel_pos, double base)
{
    tools_rgb_data color;
    rgb out_rgb;
    hsv in_hsv;

    in_hsv.h = (wheel_pos / base) * 360.0;
    in_hsv.s = 1.0;
    in_hsv.v = 1.0;
    out_rgb = hsv2rgb(in_hsv);

    color.r = (uint8_t)round(out_rgb.r * 255.0);
    color.g = (uint8_t)round(out_rgb.g * 255.0);
    color.b = (uint8_t)round(out_rgb.b * 255.0);

    return color;
}

int tools_get_random(int start, int end)
{
    float rand = mgos_rand_range(start, end);
    return (int)round(rand);
}

tools_rgb_data tools_fade_color(tools_rgb_data start, double fade)
{
    tools_rgb_data color;
    double h, s, v;
    tools_rgb_to_hsv(start, &h, &s, &v);
    color = tools_hsv_to_rgb(h, s, v * fade);
    return color;
}

double tools_check_color_distance(tools_rgb_data* start, uint16_t count, double h_test)
{
    tools_rgb_data* col;
    double h, s, v, dist, min = 9999.0;

    for (int i = 0; i < count; i++) {
        col = start + (i * sizeof(tools_rgb_data));
        tools_rgb_to_hsv(*col, &h, &s, &v);
        dist = fabs(h - h_test);
        min = (dist < min) ? dist : min;
    }

    return min;
}

tools_rgb_data tools_get_random_color(tools_rgb_data start, tools_rgb_data* test, uint16_t count, double min_dist)
{
    return tools_get_random_color_fade(start, test, count, min_dist, 1.0, 0.4);
}

tools_rgb_data tools_get_random_color_fade(tools_rgb_data start, tools_rgb_data* test, uint16_t count, double min_dist, double s_new, double v_new)
{
    double h, s, v, new_h;
    tools_rgb_to_hsv(start, &h, &s, &v);

    if ((start.r + start.g + start.b) < 10.0) {
        h = (double)tools_get_random(0, 360);
    } else {
        tools_rgb_to_hsv(start, &h, &s, &v);
    }

    do {
        tools_get_random(0, 360);
        new_h = (double)tools_get_random(0, 360);
    } while (tools_check_color_distance(test, count, new_h) < 30.0);

    LOG(LL_DEBUG, ("Generate random color: Diff %03.02f, Old H %03.02f, New H %03.02f", fabs(h - new_h), h, new_h));
    LOG(LL_DEBUG, ("Generate random color from R: 0x%02X, G: 0x%02X, B: 0x%02X", start.r, start.g, start.b));

    h = new_h;
    s = s_new;
    v = v_new;
    LOG(LL_DEBUG, ("Generate random color with H: %.02f, S: %.02f, V: %.02f", h, s, v));
    tools_rgb_data result = tools_hsv_to_rgb(h, s, v);
    LOG(LL_DEBUG, ("Generated random color with R: 0x%02X, G: 0x%02X, B: 0x%02X", result.r, result.g, result.b));

    return result;
}

void tools_set_color(tools_rgb_data* color, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = a;
}

void tools_config_get_color(char* fmt, char* key, tools_rgb_data* color)
{
    size_t elems;
    char** conf_col = tools_config_get_dyn_arr(fmt, key, &elems);
    if (conf_col) {
        if (elems == 3) {
            color->r = atoi(conf_col[0]);
            color->g = atoi(conf_col[1]);
            color->b = atoi(conf_col[2]);
        }
        if (elems == 4) {
            color->a = atoi(conf_col[3]);
        }
        tools_str_split_free(conf_col, elems);
    }
}

tools_rgb_data tools_hexcolor_str_to_rgb(char *hex_val)
{
    uint32_t number = (int)strtol(hex_val, NULL, 0);
    return tools_hexcolor_to_rgb(number);
}

tools_rgb_data tools_hexcolor_to_rgb(uint32_t hex_val)
{
        tools_rgb_data color;

        if (hex_val > 0xFFFFFF) {
            color.r = ((hex_val & 0xFF000000) >> 24) & 0xFF;
            color.g = ((hex_val & 0x00FF0000) >> 16) & 0xFF;
            color.b = ((hex_val & 0x0000FF00) >> 8) & 0xFF;
            color.a = ((hex_val & 0x000000FF)) & 0xFF;
        } else {
            color.r = ((hex_val & 0xFF0000) >> 16) & 0xFF;
            color.g = ((hex_val & 0x00FF00) >> 8) & 0xFF;
            color.b = ((hex_val & 0x0000FF)) & 0xFF;
            color.a = 255;
        }

        return color;
    }
