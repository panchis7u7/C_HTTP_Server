#ifndef _API_H_
#define _API_H_

#include "Mysql.h"

extern void handleApi(int fd, char* api, MYSQL* conn, int(*f)(int fd, char* cabeza, char* tipo_contenido, void* cuerpo, unsigned long long tamano_contenido)); 

#endif //_API_H_