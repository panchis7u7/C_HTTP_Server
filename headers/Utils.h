#ifndef _UTILS_H_
#define _UTILS_H_

#include <mysql/mysql.h>

typedef struct request_info {
    char* ip_addr;
    char* request_type;
    char* protocol;
    char* request;
} request_info;

struct args {
    struct cache* cache;
    MYSQL* conn;
    request_info* request_info;
};

#endif //_UTILS_H_