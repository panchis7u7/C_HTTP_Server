#include "Mime.h"
#include <ctype.h>
#include <string.h>

#define DEFAULT_MIME "application/octet-stream"

//Prototipo de funciones.
//char* string_minusculas(char*);

//Convierte a minusculas toda una cadena de caracteres.
char* string_minusculas(char* s){
  char* p;
  for(p = s; *p != '\0'; p++)
  {
    *p = tolower(*p);
  }
  return s;
}

char* cleanText(char* str){
  int version = 0;
  for (int i = 0, j; str[i] != '\0'; ++i) {
      // enter the loop if the character is not an alphabet
      // and not the null character.
      if(str[i] == "?")
        version = 1;
      while (!(str[i] >= 'a' && str[i] <= 'z') && !(str[i] >= 'A' && str[i] <= 'Z') && !((str[i] >= '0' && str[i] <= '9') && version != 0) && !(str[i] == '\0')) {
         for (j = i; str[j] != '\0'; ++j) {
            // if jth element of line is not an alphabet,
            // assign the value of (j+1)th element to the jth element.
            str[j] = str[j + 1];
         }
         str[j] = '\0';
      }
   }
}

//Regresa un tipo MIME para un nombre de archivo dado.
char *obtener_tipo_mime(char *filename) {
  char *ext = strrchr(filename, '.');

  if (ext == NULL) {
    return DEFAULT_MIME;
  }
  ext++;
  string_minusculas(ext);
  cleanText(ext);
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

  return DEFAULT_MIME;
}