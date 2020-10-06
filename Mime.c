#include "Mime.h"
#include <ctype.h>
#include <string.h>

#define TIPO_MIME_PREDETERMINADO "application/octet-stream"

//Prototipo de funciones.
char* string_minusculas(char*);

//Convierte a minusculas toda una cadena de caracteres.
char* string_minusculas(char* s){
    char* p;
    for (p = s; *p != '\0'; p++)
    {
        *p = tolower(*p);
    }
    return s;
}

//Regresa un tipo MIME para un nombre de archivo dado.
char *obtener_tipo_mime(char *filename) {
  char *ext = strrchr(filename, '.');

  if (ext == NULL) {
    return TIPO_MIME_PREDETERMINADO;
  }
  ext++;
  string_minusculas(ext);
  // Esto es O(n) y deberia de ser O(1).

  if (strcmp(ext, "html") == 0 || strcmp(ext, "htm") == 0) {
    return "text/html";
  }
  if (strcmp(ext, "jpeg") == 0 || strcmp(ext, "jpg") == 0) {
    return "image/jpeg";
  }
  if (strcmp(ext, "css") == 0) {
    return "text/css";
  }
  if (strcmp(ext, "js") == 0) {
    return "application/javascript";
  }
  if (strcmp(ext, "json") == 0) {
    return "application/json";
  }
  if (strcmp(ext, "txt") == 0) {
    return "text/plan";
  }
  if (strcmp(ext, "gif") == 0) {
    return "image/gif";
  }
  if (strcmp(ext, "png") == 0) {
    return "image/png";
  }

  return TIPO_MIME_PREDETERMINADO;
}