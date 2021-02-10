#include "Api.h"
#include <string.h>

void handleApi(int fd, char* api,  MYSQL* conn, int(*f)(int fd, char* cabeza, char* tipo_contenido, void* cuerpo, unsigned long long tamano_contenido)) {
    if(strcmp(api, "/prueba") == 0) {
        MYSQL_FIELD* field;
        MYSQL_RES* res;
        MYSQL_ROW row;
        unsigned int num_fields;
        char body[700];
        int length = 0;

        mysql_query(conn, "SELECT * FROM Alumnos;");
        res = mysql_store_result(conn);
        num_fields = mysql_num_fields(res);
        char* headers[num_fields];

        for(unsigned int i = 0; (field = mysql_fetch_field(res)); i++) {
            headers[i] = field->name;
        }
        printf("number of fields: %d\n", num_fields);
        
        length += snprintf(body+length, sizeof(body), "[");
        while(row = mysql_fetch_row(res)){
            length += snprintf(body+length, sizeof(body), "{");
            for(int i = 0; i < num_fields; i++){
                length += snprintf(body+length, sizeof(body),"\"%s\": %s, ", headers[i], row[i]);
            }
            length += snprintf(body+length, sizeof(body), "}, ");
        }
        length += snprintf(body+length, sizeof(body), "]");

        (*f)(fd, "HTTP/1.1 200 OK", "application/json", body, length);
        mysql_free_result(res);
    }
}

