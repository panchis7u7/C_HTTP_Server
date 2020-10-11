#include "Mime.h"
#include <ctype.h>
#include <string.h>

#define TIPO_MIME_PREDETERMINADO "application/octet-stream"

//Prototipo de funciones.
//char* string_minusculas(char*);

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
  if (strcmp(ext, "css") == 0) {
    return "text/css";
  }
  if (strcmp(ext, "jpeg") == 0 || strcmp(ext, "jpg") == 0 || strcmp(ext, "jfif") == 0) {
    return "image/jpeg";
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
  if (strcmp(ext, "svg") == 0) {
    return "image/svg+xml";
  }
  if (strcmp(ext, "ttf") == 0) {
    return "font/ttf";
  }
  if (strcmp(ext, "woff") == 0) {
    return "font/woff";
  }
  if (strcmp(ext, "woff2") == 0) {
    return "font/woff2";
  }
  if (strcmp(ext, "eot") == 0) {
    return "application/vnd.ms-fontobject";
  }
  if (strcmp(ext, "csv") == 0) {
    return "text/csv";
  }

  return TIPO_MIME_PREDETERMINADO;
}