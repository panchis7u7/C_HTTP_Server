#include "Api.h"
#include <string.h>

void handleApi(int fd, char* api,  MYSQL* conn, int(*f)(int fd, char* cabeza, char* tipo_contenido, void* cuerpo, unsigned long long tamano_contenido)) {
    if(strcmp(api, "/prueba") == 0) {
        MYSQL_FIELD* field;
        MYSQL_RES* res;
        MYSQL_ROW row;
        unsigned int num_fields;
        char body[1200];
        int length = 0;

        mysql_query(conn, "SELECT * FROM Alumnos;");
        res = mysql_store_result(conn);
        num_fields = mysql_num_fields(res);
        MYSQL_FIELD* fields[num_fields];

        for(unsigned int i = 0; (field = mysql_fetch_field(res)); i++) {
            fields[i] = field;
        }
        
        length += snprintf(body+length, sizeof(body), "[");
        while(row = mysql_fetch_row(res)){
            length += snprintf(body+length, sizeof(body), "{");
            for(int i = 0; i < num_fields-1; i++){
                if(IS_NUM(fields[i]->type))
                    length += snprintf(body+length, sizeof(body),"\"%s\":%s,", fields[i]->name, row[i]);
                else
                    length += snprintf(body+length, sizeof(body),"\"%s\":\"%s\",", fields[i]->name, row[i]);
            }
            length += snprintf(body+length-1, sizeof(body), "},")-1;
        }
        length += snprintf(body+length-1, sizeof(body), "]")-1;

        (*f)(fd, "HTTP/1.1 200 OK", "application/json", body, length);
        length = 0;
        mysql_free_result(res);
    }
}

