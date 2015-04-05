/**
 * FILE     : tool.c
 * DESC     : common struct & varialbe
 * AUTHOR   : v0.1 written by Alex Stocks
 * DATE     : on April 5, 2015
 * LICENCE  : GPL 2.0
 * MOD      :
 **/

#include "tool.h"

const char* type_name[] = {"string", "list", "set", "hash", "zset", "none"};

int connect_instance(redis_instance* inst)
{
    struct timeval timeout = {1, 0};
    redisContext* context = redisConnectWithTimeout(inst->ip, inst->port, timeout);
    if (context == NULL) {
        FATAL("connect to old redis failed");
        return -1;
    }
    if (context->err) {
        FATAL("connect to old redis failed: %s", context->errstr);
        return - 1;
    }

    redisReply* reply = (redisReply*)redisCommand(context, "auth %s", inst->password);
    if (reply == NULL) {
        PRINT_REPLY_ERROR(reply);
        return -1;
    }
    freeReplyObject(reply);

    inst->cxt = context;

    return 0;
}

void disconnect_instance(redis_instance* inst)
{
    if (NULL != inst->cxt) {
        redisFree(inst->cxt);
        inst->cxt = NULL;
    }
}

long long get_key_num(redis_instance* inst)
{
    long long num = 0;

    redisReply* reply = (redisReply*)redisCommand(inst->cxt, "select %u", inst->db);
    if (reply == NULL || !(reply->len) || strcmp(reply->str, "OK")) {
        PRINT_REPLY_ERROR(reply);
        return -1;
    }
    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(inst->cxt, "dbsize");
    assert(reply != NULL);
    assert(REDIS_REPLY_INTEGER == reply->type);
    num = reply->integer;
    freeReplyObject(reply);

    return num;
}

long long get_lastsave(redis_instance* inst)
{
    long long time = 0;

    redisReply* reply = (redisReply*)redisCommand(inst->cxt, "lastsave");
    assert(reply != NULL);
    assert(REDIS_REPLY_INTEGER == reply->type);
    time = reply->integer;
    freeReplyObject(reply);

    return time;
}

int get_key_type(redis_instance* inst, char* key, size_t len)
{
    assert(NULL != inst);
    assert(NULL != key);
    assert(0 != len);

    redisReply* reply = (redisReply*)redisCommand(
                inst->cxt, "type %b", key, len);
    assert(reply != NULL);
    assert(reply->type == REDIS_REPLY_STATUS);
    assert(0 != reply->len);
    assert(NULL != reply->str);

    int flag = 0;
    for (flag = 0; flag < KUNKOWN; flag++) {
        if (0 == strcmp(type_name[flag], reply->str)) {
            break;
        }
    }

    freeReplyObject(reply);

    return flag;
}

long long get_key_pttl(redis_instance* inst, char* key, size_t len)
{
    assert(NULL != inst);
    assert(NULL != key);
    assert(0 != len);

    redisReply* reply = (redisReply*)redisCommand(
                inst->cxt, "pttl %b", key, len);
    assert(reply != NULL);
    assert(reply->type == REDIS_REPLY_INTEGER);

    long long pttl = reply->integer;

    freeReplyObject(reply);

    return pttl;
}

long long get_list_size(redis_instance* inst, char* list, size_t len)
{
    long long num = 0;
    redisReply* reply = (redisReply*)redisCommand(inst->cxt, "llen %b", list, len);
    assert(reply != NULL);
    assert(reply->type == REDIS_REPLY_INTEGER);

    num = reply->integer;

    freeReplyObject(reply);

    return num;
}

long long get_set_size(redis_instance* inst, char* set, size_t len)
{
    long long num = 0;
    redisReply* reply = (redisReply*)redisCommand(inst->cxt, "scard %b", set, len);
    assert(reply != NULL);
    assert(reply->type == REDIS_REPLY_INTEGER);

    num = reply->integer;

    freeReplyObject(reply);

    return num;
}

long long get_sset_size(redis_instance* inst, char* sset, size_t len)
{
    long long num = 0;
    redisReply* reply = (redisReply*)redisCommand(inst->cxt, "zcard %b", sset, len);
    assert(reply != NULL);
    assert(reply->type == REDIS_REPLY_INTEGER);

    num = reply->integer;

    freeReplyObject(reply);

    return num;
}

long long get_hashtable_size(redis_instance* inst, char* hash, size_t len)
{
    long long num = 0;
    redisReply* reply = (redisReply*)redisCommand(inst->cxt, "hlen %b", hash, len);
    assert(reply != NULL);
    assert(reply->type == REDIS_REPLY_INTEGER);

    num = reply->integer;

    freeReplyObject(reply);

    return num;
}

void print_list(redis_instance* inst, char* list, size_t len)
{
    long index = 0;
    do {
        redisReply* reply = (redisReply*)redisCommand(
                    inst->cxt, "lrange %b %lld %lld",
                    list, len, index, index + SCAN_INC - 1);
        assert(reply != NULL);
        assert(reply->type == REDIS_REPLY_ARRAY);
        if (0 == reply->elements) {
            break;
        }

        redisReply* reply1;
        for (size_t i = 0; i < reply->elements; i++)  {
            reply1 = reply->element[i];
            assert(reply1 != NULL);
            assert(reply1->len != 0);

            pline("    [cursor:%ld, key:%s],", index++, reply1->str);
        }

        if (reply->elements < SCAN_INC) {
            freeReplyObject(reply);
            break;
        }

        freeReplyObject(reply);
    } while(1);
}

void print_set(redis_instance* inst, char* set, size_t len)
{
    long index = 0;
    long long cursor = 0;
    redisReply* keys;
    do {
        redisReply* reply = (redisReply*)redisCommand(
                    inst->cxt, "sscan %b %lld count %lld",
                    set, len, cursor, SCAN_INC);
        assert(reply != NULL);
        assert(reply->type == REDIS_REPLY_ARRAY);
        assert(2 == reply->elements);
        assert(REDIS_REPLY_STRING == reply->element[0]->type);
        assert(REDIS_REPLY_ARRAY == reply->element[1]->type);

        char* key;
        redisReply *keys = reply->element[1];
        for (size_t i = 0; i < keys->elements; i++)  {
            key = keys->element[i]->str;
            pline("    [cursor:%ld, key:%s],", index++, key);
        }

        cursor = strtoll(reply->element[0]->str, (char**)NULL, 10);
        freeReplyObject(reply);
    } while(cursor != 0);
}

void print_sset(redis_instance* inst, char* sset, size_t len)
{
    /*
    kv = (redisReply*)redisCommand(inst->cxt, "zrange %s 0 -1 withscores", key);
    assert (kv != NULL);
    assert(kv->type == REDIS_REPLY_ARRAY);
    assert(kv->elements != 0);

    pline("{idx:%lu, sset:%s, member-score num:%lu,", index++, key, kv->elements);
    for (unsigned j = 0; j < kv->elements; j += 2) {
    pline("    {idx:%u, member:%s, score:%s},", j, kv->element[i]->str, kv->element[i + 1]->str);
    }
    pline("}\n");
    freeReplyObject(kv);
    */
    long index = 0;
    long long cursor = 0;
    redisReply* keys;
    do {
        redisReply* reply = (redisReply*)redisCommand(
                    inst->cxt, "zscan %b %lld count %lld",
                    sset, len, cursor, SCAN_INC);
        assert(reply != NULL);
        assert(reply->type == REDIS_REPLY_ARRAY);
        assert(2 == reply->elements);
        assert(REDIS_REPLY_STRING == reply->element[0]->type);
        assert(REDIS_REPLY_ARRAY == reply->element[1]->type);

        redisReply *keys = reply->element[1];
        for (size_t i = 0; i < keys->elements; i+=2)  {
            pline("    [cursor:%u, key:%s, value:%s],",
                  index++, keys->element[i]->str, keys->element[i + 1]->str);
        }

        cursor = strtoll(reply->element[0]->str, (char**)NULL, 10);
        freeReplyObject(reply);
    } while(cursor != 0);
}

void print_hashtable(redis_instance* inst, char* hash, long long len)
{
    /*
       kv = (redisReply*)redisCommand(inst->cxt, "hgetall %s", key);
       assert (kv != NULL);
       assert(kv->type == REDIS_REPLY_ARRAY);
       assert(kv->elements != 0);

       pline("{idx:%lu, hashtable:%s, kv num:%lu,", idx, key, kv->elements);
       for (unsigned j = 0; j < kv->elements; j += 2) {
       pline("    {idx:%u, key:%s, value:%s},", j, kv->element[i]->str, kv->element[i + 1]->str);
       }
       pline("}\n");
       freeReplyObject(kv);

     */
    long index = 0;
    long long cursor = 0;
    redisReply* keys;
    do {
        redisReply* reply = (redisReply*)redisCommand(
                    inst->cxt, "hscan %b %lld count %lld",
                    hash, (size_t)(len), cursor, SCAN_INC);
        assert(reply != NULL);
        assert(reply->type == REDIS_REPLY_ARRAY);
        assert(2 == reply->elements);
        assert(REDIS_REPLY_STRING == reply->element[0]->type);
        assert(REDIS_REPLY_ARRAY == reply->element[1]->type);

        redisReply *keys = reply->element[1];
        for (size_t i = 0; i < keys->elements; i+=2)  {
            pline("    [cursor:%ld, key:%s, value:%s],",
                        index++, keys->element[i]->str, keys->element[i+1]->str);
        }

        cursor = strtoll(reply->element[0]->str, (char**)NULL, 10);
        freeReplyObject(reply);
    } while(cursor != 0);
}

void print_key(redis_instance* inst, char* key, size_t len)
{
    long long size = 0;
    int flag = get_key_type(inst, key, len);
    long long pttl = get_key_pttl(inst, key, len);
    switch (flag) {
        case KSTRING: {
            redisReply* kv = (redisReply*)redisCommand(
                        inst->cxt, "get %b", key, len);
            assert (kv != NULL);
            assert(kv->type == REDIS_REPLY_STRING);
            assert(kv->len != 0);

            pline("{key:%s, pttl:%lldms, type:string, db:%u, value:%s}",
                   key, pttl, inst->db, kv->str);
            freeReplyObject(kv);
        }
        break;

        case KHASH: {
            size = get_hashtable_size(inst, key, len);
            pline("{key:%s, pttl:%lldms, type:hashtable, size:%lld, db:%lu,\n  kvs:[",
                  key, pttl, size, inst->db);
            if (size) {
                print_hashtable(inst, key, len);
            }
            pline("  ]\n}");
        }
        break;

        case KLIST: {
            size = get_list_size(inst, key, len);
            pline("{key:%s, pttl:%lldms, type:list, size:%lld, db:%lu,\n  keys:[",
                  key, pttl, size, inst->db);
            if (size) {
                print_list(inst, key, len);
            }
            pline("  ]\n}");
        }
        break;

        case KSET: {
            size = get_set_size(inst, key, len);
            pline("{key:%s, pttl:%lldms, type:set, size:%lld, db:%lu,\n  keys:[",
                  key, pttl, size, inst->db);
            if (size) {
                print_set(inst, key, len);
            }
            pline("  ]\n}");
        }
        break;

        case KSSET: {
            size = get_sset_size(inst, key, len);
            pline("{key:%s, pttl:%lldms, type:sset, size:%lu, db:%lu,\n  kvs:[",
                  key, pttl, size, inst->db);
            if (size) {
                print_sset(inst, key, len);
            }
            pline("  ]\n}");
        }
        break;

        case KNONE: {
            FATAL("none type of key:%s", key);
        }
        break;

        case KUNKOWN: {
            FATAL("unknown type of key:%s", key);
        }
    }
}

