/**
 * FILE     : redis-dump-key.c
 * DESC     : output a key of the database
 * AUTHOR   : v1.0 written by Alex Stocks
 * DATE     : on April 5, 2015
 * LICENCE  : GPL 2.0
 * MOD      :
 **/

#include "tool.h"

int main(int argc, char** argv)
{
    if (argc < 6) {
        FATAL("Right usage: dbprint ip port password db_id key");
        return -1;
    }

    char* ip = argv[1];
    unsigned short port = (unsigned short)strtoul(argv[2], (char**)NULL, 10);
    char* password = argv[3];
    unsigned db=  (unsigned)strtoul(argv[4], (char**)NULL, 10);
    char* key = argv[5];

    redis_instance inst = {
        .port = port,
        .db = db,
        .cxt = NULL,
    };
    strncpy(inst.ip, ip, sizeof(inst.ip));
    strncpy(inst.password, password, sizeof(inst.password));
    if (connect_instance(&inst)) {
        FATAL("failed to connect instance{%s-%u}", inst.ip, inst.port);
        return -1;
    }

    long long num = get_key_num(&inst);
    if (num <= 0) {
        redisFree(inst.cxt);
        FATAL("get_key_num() = %lld", num);
        return -1;
    }

    print_key(&inst, key, strlen(key));

    disconnect_instance(&inst);

    return 0;
}

