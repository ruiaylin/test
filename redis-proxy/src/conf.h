/**
 * FILE     : conf.h
 * DESC     : [string]key-[string]value dictonary
 * AUTHOR   : v0.3.0 written by Alex Stocks
 * DATE     : on Oct 25, 2012
 * LICENCE  : GPL 2.0
 * MOD      :
 **/

#ifndef __CONF_H__
#define __CONF_H__

#include <stddef.h>

/**
 * description    : read a configure file @file and get the dictionary <br/><br/>
 *
 * in@file        : file name
 * in-out@dict    : dictionary
 *
 * out@ret        : the return value is 0 if both params are not nil
 *                  and the file @file is valid configure file;
 *                  otherwise -1 if params are illegal or -2 if @file
 *                  does not exist or it does not have any configure key-value.
 **/
int dict_init(void** dict, char* file);
/**
 * description    : free the dictionary @dict <br/><br/>
 *
 * in@dict        : config dictionary
 *
 * out@ret        : return is void
 **/
void dict_uninit(void** dict);
/**
 * description    : get value of the @key0+@key1 from the dictionary @dict <br/><br/>
 *
 * in@dict        : dictionary. pay attention that parameter type is "void*".
 * in@key0        : first key
 * in@key1        : second key
 *
 * out@ret        : if the dictionary @dict is complete and there is value of @key,
 *                  the return is its value, otherwise nil instead.
 **/
const char* dict_get_value(void* dict, const char* key0, const char* key1);
/**
 * description    : get the @index-th element of the @key0+@key1 from the dictionary
 *                  @dict <br/><br/>
 *
 * in@dict        : dictionary
 * in@key0        : first key
 * in@key1        : second key
 * in@index       : element index. the start index is 0.
 *
 * out@ret        : if the value of @key in@dict is a array and the @index
 *                  is not greater than the maximum element number of its
 *                  related array, the return value is what you wanna, otherwise
 *                  it is nil.
 **/
const char* dict_get_array_value(void* dict, const char* key0, const char* key1, int index);
/**
 * description    : get the size of @key0+@key1's value array. <br/><br/>
 *
 * in@dict        : dictionary
 * in@key0        : first key
 * in@key1        : second key

 * in-out@szie    : array size
 *
 * out@ret        : if successful, ret is 0; otherwise -1 if some parameters
 *                  are illegal.
 **/
int dict_get_array_size(void* dict, const char* key0, const char* key1, size_t* size);

#define DICT_VALUE(dict, key0, key1) ({                             \
    const char* __conf_ret__ = nil_str;                             \
    __conf_ret__ = dict_get_value(                                  \
                                  (void*)(dict),                    \
                                  (const char*)key0,                \
                                  (const char*)key1                 \
                                 );                                 \
    (char*)__conf_ret__;                                            \
})

#define DICT_VALUE_U8(dict, key0, key1)                             \
    str2u8(DICT_VALUE(dict, key0, key1))

#define DICT_ARRAY_VALUE(dict, key0, key1, idx) ({                  \
    const char* __conf_ret__ = nil_str;                             \
    __conf_ret__ = dict_get_array_value(                            \
                                        (void*)(dict),              \
                                        (const char*)key0,          \
                                        (const char*)key1,          \
                                        idx                         \
                                        );                          \
    (char*)__conf_ret__;                                            \
})

#define DICT_ARRAY_VALUE_U8(dict, key0, key1, idx)                  \
    str2u8((char*)DICT_ARRAY_VALUE(dict, key0, key1, idx))

#define DICT_ARRAY_SIZE(dict, key0, key1) ({                        \
    size_t __conf_size__ = 0;                                       \
    dict_get_array_size(                                            \
                        (void*)(dict),                              \
                        (const char*)(key0),                        \
                        (const char*)(key1),                        \
                        &__conf_size__                              \
                       );                                           \
})

#endif

