/**
 * FILE     : conf.c
 * DESC     : [string]key-[string]value dictonary
 * AUTHOR   : v0.3.0 written by Alex Stocks
 * DATE     : on Oct 25, 2012
 * LICENCE  : GPL 2.0
 * MOD      :
 **/

#include "redis.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "log.h"
#include "dict.h"
#include "sds.h"

//////////////////////////////////////////
//get_opt
//////////////////////////////////////////

#define PTR_OFFSET(ptr0, ptr1) ({                  \
    (((char*)(ptr0) < (char*)(ptr1)) ?             \
    ((char*)(ptr1) - (char*)(ptr0)) :              \
    ((char*)(ptr0) - (char*)(ptr1)));              \
})

#define CMP(dst, src, len) ({                    \
    int __cmp_flag__ = -2;                       \
    if ((unsigned)(len))) {                      \
        __cmp_flag__ = memcmp(                   \
                            (void*)(dst),        \
                            (void*)(src),        \
                            (size_t)(len)        \
                            );                   \
    }                                            \
    __cmp_flag__;                                \
})

static int get_line_key(char* line, char** key, size_t* size);
static int get_line_value(char* line, char** value, size_t* size);
static int is_comment(const char* buf, size_t buf_len);
static int parse_line(dict* dt, sds line);
static int parse_array(dict* dt, sds key, sds value);

#define COMMENT_CHAR0       '#'
#define KEY_CHAR1           '['
#define KEY_CHAR2           ']'
#define CR_CHAR             '\n'
#define EQUAL_CHAR0         '='
#define EQUAL_CHAR1         ':'
#define ARRAY_DELIM         ','
#define DEFAULT_FIRST_KEY   "default"

static const char* suffix = "__lisk_array_size_suffix__";

int get_line_key(char* line, char** key, size_t* size)
{
    char  equal0;
    char  equal1;
    char* ret;

    equal0 = EQUAL_CHAR0;
    equal1 = EQUAL_CHAR1;

    ret = strchr(line, equal0);
    if (!ret) {
        ret = strchr(line, equal1);
        if (!ret) {
            perr("strchr(line = %s, equal1 = %c) = %p", line, equal1, ret);
            return -1;
        }
    }

    *key = line;
    *size = (size_t)PTR_OFFSET(ret, line);

    return 0;
}

int get_line_value(char* line, char** value, size_t* value_size)
{
    char        equal0;
    char        equal1;
    char*       ret;
    unsigned    size;

    size = strlen(line);
    if (!size) {
        return -1;
    }

    equal0 = EQUAL_CHAR0;
    equal1 = EQUAL_CHAR1;

    ret = strchr(line, equal0);
    if (!ret) {
        ret = strchr(line, equal1);
        if (!ret) {
            perr("strchr(line = %p, equal1 = %c) = %p", line, equal1, ret);
            return -2;
        }
    }

    *value = ret + sizeof(char);
    *value_size = (size_t)PTR_OFFSET(line + size, *value);

    return 0;
}

int is_comment(const char* buf, size_t buf_len)
{
    int ret = 0;

    ret = -2;
    if (0 < buf_len && buf[0] == COMMENT_CHAR0) {
        ret = 0;
    }

    return ret;
}

int is_first_key(const char* buf, unsigned buf_len)
{
    int ret = 0;

    ret = -2;
    if (buf[0] == KEY_CHAR1 && buf[buf_len-1] == KEY_CHAR2) {
        ret = 0;
    }

    return ret;
}

int parse_line(dict* dt, sds line)
{
    static char first_key[1024] = {DEFAULT_FIRST_KEY};
    do {
        if (!sdslen(line)) {
            break;
        }

        int ret = is_comment((char*)line, sdslen(line));
        if (!ret) {
            break;
        }

        sdstrimspace(line);
        if (!sdslen(line)) {
            break;
        }

        // get first key
        ret = is_first_key((char*)(line), sdslen(line));
        if (!ret) {
            ((char*)line)[sdslen(line)-1] = '\0';
            snprintf(first_key, sizeof(first_key), "%s", (char*)(line) + 1);
            continue;
        }

        // key
        sds key = sdsnewlen(first_key, strlen(first_key));
        char* key_str;
        size_t length;
        ret = get_line_key((char*)(line), &key_str, &length);
        if (ret || !length) {
            break;
        }
        sdscatlen(key, (void*)(key_str), length);

        // value
        char* value_str;
        ret = get_line_value((char*)(line), &value_str, &length);
        if (ret || !length) {
            sdsfree(key);
            continue;
        }
        sds value = sdsnewlen(value_str, length);

        dictAdd(dt, (void*)(key), (void*)(value));
        parse_array(dt, key, value);
    } while(0);

    return 0;
}

int parse_array(dict* dt, sds key, sds value)
{
    char* key_str = key;
    char* value_str = value;
    char line_key[strlen(key_str) + 32];

    int idx = 0;
    char limit = ARRAY_DELIM;
    char* start = value_str;
    char* end = value_str + strlen(value_str);
    sds key_;
    sds value_;
    for (;;) {
        if (start >= end) {
            break;
        }
        char* pos = strchr(start, limit);
        if (!pos) {
            break;
        }
        if (pos <= start) {
            start += sizeof(char);
            continue;
        }
        sprintf(line_key, "%s%d", key_str, idx);
        key_ = sdsnew(line_key);
        value_ = sdsnewlen((void*)(start), PTR_OFFSET(start, pos));
        dictAdd(dt, key_, value_);

        start = pos + sizeof(char);
        idx++;
    }

    if (!idx) {
        if (start < end) {
            sprintf(line_key, "%s%d", key_str, idx);
            key_ = sdsnew(line_key);
            value_ = sdsnewlen(start, PTR_OFFSET(start, end));
            dictAdd(dt, key, value);
            idx++;
        }

        sprintf(line_key, "%s%s", key_str, suffix);
        key_ = sdsnew(line_key);
        char size_str[64];
        sprintf(size_str, "%d", idx);
        value_ = sdsnew(size_str);
        dictAdd(dt, key_, value_);
    }

    return 0;
}

int file_get_line(int fd, sds line)
{
    off_t cursor = lseek(fd, 0, SEEK_CUR);
    if (cursor < 0) {
        cursor = 0;
    }
    struct stat    fst;
    fstat(fd, &fst);
    size_t file_len = fst.st_size;
    if ((size_t)(cursor + 1) >= file_len) {
        return -1;
    }

    int flag = 0;
    char new_line = '\n';
    char same_line = '\\';
    size_t sub_str_len = 0;
    char buf[1024];
    char* new_line_buf;
    ssize_t offset = 0;
    while (0 < (offset = pread(fd, buf, sizeof(buf)-1, cursor))) {
        buf[offset] = '\0';
        // check '\n' or '\\'
        new_line_buf = strchr(buf, new_line);
        if (new_line_buf) {
            new_line_buf[0] = '\0';
            sub_str_len = strlen(buf);
            flag = 0;
            if (sub_str_len && buf[sub_str_len-1] == same_line) {
                flag = 1;
                sub_str_len --;
            }
            sdscatlen(line, buf, sub_str_len);
            cursor += (sub_str_len + 1 + flag);
            lseek(fd, cursor, SEEK_SET);
            if (!flag) {
                break;
            }
        } else {
            sdscatlen(line, buf, strlen(buf));
            cursor += strlen(buf);
            lseek(fd, cursor, SEEK_SET);
        }
    }

    if (offset <= 0) {
        return -2;
    }

    return 0;
}

int dict_init(void** dict_pptr, char* file)
{
    if (!(file) && !(dict_pptr)) {
        perr("@file = %p, @dict = %p", (void*)file, (void*)dict_pptr);
        return -1;
    }

    dict* dt = dictCreate(&dbDictType,NULL);
    int fd = open(file, O_RDONLY);
    sds line = sdsnewlen(NULL, 96);
    do {
        sdsclear(line);
        int ret = file_get_line(fd, line);
        if (ret) {
            break;
        }

        parse_line(dt, line);
    } while (1);
    int ret = 0;
    if (!dictSize(dt)) {
        dt = NULL;
        ret = -1;
    }
    sdsfree(line);
    close(fd);

    *dict_pptr = dt;

    return ret;
}

void dict_uninit(void** dict_pptr)
{
    if (dict_pptr && *dict_pptr) {
        dictEntry* de;
        dictIterator* it = dictGetIterator(*dict_pptr);
        while ((de = dictNext(it))) { 
            sdsfree(dictGetKey(de));
            sdsfree(dictGetVal(de));
        }
        dictReleaseIterator(it); 
        dictRelease((dict*)(*dict_pptr));
        *dict_pptr = NULL;
    }
}

const char* dict_get_value(void* dict, const char* key_str0, const char* key_str1)
{
    if (!dict || !key_str0 || !key_str1) {
        perr("@dict = %p, @key_str0 = %p, @key_str1 = %p", dict, key_str0, key_str1);
        return NULL;
    }

    if (!(strlen(key_str1))) {
        key_str1 = key_str0;
        key_str0 = DEFAULT_FIRST_KEY;
    }

    char key_string[strlen(key_str0) + strlen(key_str1) + 32];
    sprintf(key_string, "%s%s", key_str0, key_str1);
    sds key = sdsnew(key_string);
    dictEntry* de = dictFind(dict, key);
    if (!de) {
        perr("dictFind(dict = %p, key = %s) = nil", dict, key_string);
        return NULL;
    }

    return dictGetVal(de);
}

const char* dict_get_array_value(void* dict, const char* key_str0, const char* key_str1, int idx)
{
    if (!(dict) || !(key_str0) || !(key_str1)) {
        perr("@dict = %p, @key0 = %p, @key1 = %p", dict, key_str0, key_str1);
        return NULL;
    }

    if (!strlen(key_str1)) {
        key_str1 = key_str0;
        key_str0 = DEFAULT_FIRST_KEY;
    }

    char key_string[strlen(key_str0) + strlen(key_str1) + 32];
    sprintf(key_string, "%s%s%d", key_str0, key_str1, idx);
    sds key = sdsnew(key_string);
    dictEntry* de = dictFind(dict, key);
    sdsfree(key);
    if (!de) {
        perr("dictFind(dict = %p, key = %s) = nil", dict, key_string);
        return NULL;
    }

    return dictGetVal(de);
}

int dict_get_array_size(void* dict, const char* key_str0, const char* key_str1, size_t* size)
{
    if (!(dict) || !(key_str0) || !(key_str1) ||!(size)) {
        perr("@dict = %p, @key0 = %p, @key1 = %p, @size = %p",
                dict, key_str0, key_str1, (void*)(size));
        return -1;
    }

    if (!strlen(key_str1)) {
        key_str1 = key_str0;
        key_str0 = DEFAULT_FIRST_KEY;
    }

    char line_key[strlen(key_str0) + strlen(key_str1) + 32];
    sprintf(line_key, "%s%s%s", key_str0, key_str1, suffix);
    sds key = sdsnew(line_key);
    dictEntry* de = dictFind(dict, key);
    sdsfree(key);
    if (!de) {
        perr("dictFind(dict = %p, key = %s) = nil", (void*)(dict), line_key);
        return -2;
    }

    *size = strtoul(dictGetVal(de), (char**)NULL, 10);

    return 0;
}

