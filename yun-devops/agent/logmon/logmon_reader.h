/**
 * copyright (c) 2015 Baidu.com, Inc. All Rights Reserved
 **/

/*
 * @file logmon_reader.h
 * @author zhaoxin08(zhaoxin08@baidu.com)
 * @date  2015-08-03
 * @brief
 *
 */

#ifndef LOGMON_LOGMON_READER_H
#define LOGMON_LOGMON_READER_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string>

enum RET
{
    OK = 0,
    ERR = -1,
};

#define pinfo(fmt, ...)     \
    printf("\033[1;34;40m%s-%s-%d: "fmt"\033[0m\n", ((char*)__FILE__), \
            (char*)__func__, (int)__LINE__, ##__VA_ARGS__)

#define perr(fmt, ...)     \
    fprintf(stderr, "\033[1;31;40m%s-%s-%d: error: "fmt"\033[0m\n", \
            (char*)__FILE__, (char*)__func__, (int)__LINE__, ##__VA_ARGS__)

#define LM_NOTICE_LOG pinfo
#define LM_DEBUG_LOG pinfo
#define LM_WARNING_LOG perr

enum
{
    PATH_LENGTH = 1024
};

/**
 * @brief日志读取的状态文件
 */
struct read_status_t
{
    off_t file_pos;   // !<读取文件位置
    ino_t file_ino;   // !<日志文件的inode
};

/**
 * @brief 判断文件或者路径是否存在
 * @param [in] path
 * @return bool
 *        - true : 存在
 *        - false：不存在
 */
bool is_path_exist(const char* path);

/**
 * @brief 模块日志读取类
 *      读取模块的日志到内存中
 */
class LmReader
{
public:
    /**
     * @brief 初始化日志读取相关信息
     * @param [in] status_file
     * @param [in] module_name
     * @param [in] log_path
     * @param [in] log_file
     * @param [in] new_log_file
     * @param [in] item_cycle
     *  仅仅用于status文件名称的一部分
     */
    int init(const char* status_file,
             const char* module_name,
             const char* log_path,
             const char* log_file,
             const char* new_log_file,
             int item_cycle);
    /**
     * @brief 读取日志文件,根据文件大小限制、读取速度进行限制
     * @param [in] speed : int 读取速度,单位(bytes/s)
     * @param [in] dur_tm : int 单次读取的时间
     * @param [in/out] log_str : std::string 读取到的log内容字符串
     * @return int
     *        - OK : 成功
     *        - ERR： 失败
     *
     **/
    int read_log(int speed, int dur_tm, std::string& log_str);
    /**
     * @brief读取状态文件
     * @return int
     *        - OK : 成功
     *        - ERR： 失败
     */
    int read_status();
    /**
     * @brief日志读取结束后更新状态文件
     * @return int
     *        - OK : 成功
     *        - ERR： 失败
     */
    int update_status();
    /**
     * @brief 获取单实例
     */
    static LmReader* get_instance();
    /**
     * @brief 判断读取的日志文件是否到达文件尾
     * @return true :到达文件尾
     *         false:没有到文件尾
     */
    bool is_eof();

    /**
     * @brief 根据状态文件是否存在来判断程序是否初次启动
     * @return true : 是
     *         false: 否
     */
    bool is_first_launch();

    /**
     * @brief 清理
     */
    void clear_up();

private:
    /**
     * @brief 构造函数
     */
    LmReader();
    ~LmReader();
    /**
     * @brief 判断日志文件是否被切割
     * @param st: struct stat* 文件stat
     * @return bool
     *        - true : 切割
     *        - false：  没有被切割
     */
    bool is_split(struct stat* st);

    /**
     * @brief 按照给定的读取速度读取文件
     * @param fp : FILE * 读取文件
     * @param speed : int 每秒读取字节数
     * @param dur_tm : int 以每秒读日志的持续时间
     * @param str : std::string& 读取的字符串
     * @return int : 返回读取状态
     */
    int speed_read(FILE * fp, int speed, int dur_tm, std::string& str);

    /**
     * @brief 通过inode号查取对应的文件名
     *
     * @return
     *         OK : 成功
     *         ERR: 失败
     */
    int find_last_log();

    /**
     * @brief 通过inode号来查找被切割前的日志文件名
     *
     * @param fpath : 遍历目录中的文件或者路径
     * @param sb : fpath的stat状态
     * @param typeflag : 类型标志
     *
     * @return
     */
    static int last_log_fun(const char *fpath,
                            const struct stat *sb,
                            int typeflag);
    ///定义查找的函数指针
    typedef int (*last_log_fp)(const char *fpath,
                            const struct stat *sb,
                            int typeflag);
private:
    static read_status_t _s_read_status; //!< 上个周期log文件的读取状态
    FILE*  _status_file; //!< 读取状态文件
    FILE*  _log_file;    //!< 日志文件
    FILE*  _last_log_file;   //!< 被切割日志的上一个日志文件
    FILE*  _cur_log_file;    //!< 当前读取的log文件
    static LmReader * _s_lm_reader; //!<单实例对象
    off_t  _cur_pos;    //!<当前日志文件读取为置
    bool   _issplit;    //!<日志文件是否被切割
    size_t _log_size;   //!<单读取的日志文件大小
    size_t _readlog_size; //!<当前周期需要读取日志的大小
    char _log_path[PATH_LENGTH];    //!<日志文件路径
    bool _st_file_exist;           //!<日志装态文件是否存在
    static char _s_last_log_fn[PATH_LENGTH]; //!<被切割日志文件名
    size_t _read_size;                //!< 读取日志文件的大小
    size_t _speed_read_size;          //!< speedread读取的日志文件大小
};

#endif

