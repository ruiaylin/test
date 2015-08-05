/*
 *
 * Copyright (c) 2015 Baidu.com, Inc. All Rights Reserved
 *
 */

/*
 * @file logmon.cc
 * @author zhaoxin08(zhaoxin08@baidu.com)
 * @date  2015-8-3
 * @brief
 *      读取日志文件,可以处理分割的日志,然后进行日志过滤
 */

#include <stdlib.h>
#include <sys/time.h>
#include "logmon_reader.h"
#include <vector>

typedef struct log_item_tag   {
    char*   str;
    size_t  time;
} log_item_t, *log_item_p, log_item_a[1];

typedef std::vector<log_item_p> log_item_vector;

size_t calc(char* haystack, char* needle);
void statictic(std::string& log_str, log_item_vector& log_item_array);
void print_log_item_array(log_item_vector& array);

int main(int argc, char** argv)
{
    if (argc < 9) {
        printf("{}");
        return;
    }

    // 给参数赋值
    char* module_name = argv[1];
    char* status_file_path = argv[2];
    char* status_file_name = argv[3];
    char* log_path = argv[4];
    char* log_name = argv[5];
    char* new_log_file_name = log_path;
    int cycle = (int)strtol(argv[6], (char**)NULL, 10);
    int read_speed = (int)strtol(argv[7], (char**)NULL, 10);

    log_item_p item;
    log_item_vector log_item_array;
    int idx = 0;
    for (idx = 8; idx < argc; idx++) {
        item = new log_item_t;
        item->str = argv[idx];
        item->time = 0;
        log_item_array.push_back(item);
    }

    //日志读取初始化
    LmReader* reader = LmReader::get_instance();
    if (NULL == reader) {
        return -1;
    }

    char status_file[PATH_LENGTH];
    snprintf(
             status_file,
             PATH_LENGTH,
             "%s/%s",
             status_file_path,
             status_file_name
            );

    //如果日志不存在的话，则默认输出各个监控项的值为0
    char log_file_name[PATH_LENGTH];
    snprintf(log_file_name, PATH_LENGTH, "%s/%s", log_path, log_name);
    if (!is_path_exist(log_file_name)) {
        return -1;
    }

    int ret = reader->init(
                           status_file,
                           module_name,
                           log_path,
                           log_file_name,
                           new_log_file_name,
                           cycle
                          );
    if (OK != ret) {
        LM_WARNING_LOG("Init read_log failed");
        return -1;
    }

    //第一次运行的话则退出
    if (reader->is_first_launch()) { //如果是第一次启动，则采用默认的输出方式
        //更新读取日志的状态文件
        if (OK != reader->update_status()) {
            return -1;
        }
    }

    int dur_read_tm = 100; //持续读取的时间,单位ms
    idx = 0;
    int top = (cycle * 1000) / dur_read_tm;
    struct timeval start_tm;
    struct timeval end_tm;
    struct timeval used_tm;
    struct timespec sleep_tm = {0, 0};
    size_t last_line = 0;
    std::string log_str; // 读取的日志字符串,用于内容和日志滚动匹配
    //开始日志读取和匹配
    while (idx++ < top) {
        gettimeofday(&start_tm, NULL);

        //读取日志
        reader->read_log(read_speed, dur_read_tm, log_str);
        LM_DEBUG_LOG("[read_log][size:%zu]", log_str.size());
        //匹配操作
        last_line = log_str.rfind('\n');
        if (std::string::npos != last_line) {
            statictic(log_str, log_item_array);
            log_str.clear();
        }
        gettimeofday(&end_tm, NULL);
        timersub(&end_tm, &start_tm, &used_tm);
        sleep_tm.tv_nsec = 1000000 * (dur_read_tm
                - (used_tm.tv_sec * 1000 + used_tm.tv_usec / 1000));
        if (sleep_tm.tv_nsec < 0) {
            sleep_tm.tv_nsec = 0;
        }
        nanosleep(&sleep_tm, NULL);
    }
    reader->update_status();
    reader->clear_up();
    print_log_item_array(log_item_array);

    return 0;
}

size_t calc(char* haystack, char* needle)
{
    size_t time = 0;
    size_t len = strlen(needle);
    if (len) {
        while (haystack = strstr(haystack, needle)) {
            haystack += len;
            time++;
        }
    }

    return time;
}

void statictic(std::string& log_str, log_item_vector& log_item_array)
{
    if (!log_str.size()) {
        return;
    }

    char* str = (char*)log_str.c_str();
    log_item_vector::iterator it = log_item_array.begin();
    while (it != log_item_array.end()) {
        (*it)->time += calc(str, (*it)->str);
    }
}

void print_log_item_array(log_item_vector& array)
{
    char tmp[1024];
    std::string buf;

    buf += "{";
    log_item_vector::iterator it = array.begin();
    snprintf(tmp, sizeof(tmp), "\"%s\":\"%zu\"", (*it)->str, (*it)->time);
    buf += tmp;
    it++;

    for (; it != array.end(); it++) {
        buf += ", ";
        snprintf(tmp, sizeof(tmp), "\"%s\":\"%zu\"", (*it)->str, (*it)->time);
        buf += tmp;
    }

    buf += "}";
    printf("%s", buf.c_str());
}

