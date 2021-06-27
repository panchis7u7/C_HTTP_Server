#ifndef _MYSQL_CONN_H_
#define _MYSQL_CONN_H_

#include <mysql/mysql.h>

typedef struct MYSQL_CONN
{
    char* host;
    char* user;
    char* password;
    char* dbname;
    char* sock;
    int port;
    int flag;
} MYSQL_CONN;

extern MYSQL* mysql_connect(MYSQL_CONN*);
#endif //_MYSQL_CONN_H_