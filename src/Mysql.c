#include "Mysql.h"
#include <stdio.h>

MYSQL* mysql_connect(MYSQL_CONN* connection){
    MYSQL* conn;
    conn = mysql_init(NULL);
    if(!(mysql_real_connect(
        conn,
        connection->host,
        connection->user,
        connection->password,
        connection->dbname,
        connection->port,
        connection->sock,
        connection->flag)))
    {
        fprintf(stderr, "\n\x1B[31mError: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
        exit(1);
    }

    printf("\x1B[32mSuccessfully connected with MySQL!\n");
    return conn;
}