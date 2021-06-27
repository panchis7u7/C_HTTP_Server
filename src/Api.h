#ifndef _API_H_
#define _API_H_

#include "Utils.h"
#include "Mysql.h"

void handleGetApi(int fd, char* api, struct args* args, int(*f)(int fd, char* cabeza, char* tipo_contenido, void* cuerpo, unsigned long long tamano_contenido, char* flags)); 

#endif //_API_H_