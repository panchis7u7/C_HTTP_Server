#include "Cache.h"
#include "File.h"
#include "Mime.h"
#include "Net.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define PORT "6969"
#define ARCHIVOS_SERVIDOR "/.serverfiles"
#define ROOT_SERVIDOR "./serveroot"

//Prototipo de funciones.
int Send_response(int, char*, char*, void*, int);
void Resp_404(int);
void Get_d20(int);
void Get_fecha(int);
void Post_guardado(int, char*);
int Obtener_archivo_o_cache(int, cache*, char*);
void Obtener_archivo(int, cache*, char*);

/**
 * Send an HTTP response
 *
 * header:       "HTTP/1.1 404 NOT FOUND" or "HTTP/1.1 200 OK", etc.
 * content_type: "text/plain", etc.
 * body:         the data to send.
 *
 * Return the value from the send() function.
 */

int Send_response(int fd, char* cabeza, char* tipo_contenido, void* cuerpo, int tamano_contenido){
    const int tamano_respuesta_maxima = 65536;
    char respuesta[tamano_respuesta_maxima];
    time_t tiempo;
    struct tm* info;
    info = localtime(&tiempo);

    int tamano_respuesta = sprintf(respuesta, 
                                    "%s\n"
                                    "Fecha: %s"
                                    "Conexion: cerrada\n"
                                    "Tamano-contenido: %d\n"
                                    "Tipo_contenido: %s\n"
                                    "\n",
                                    cabeza, asctime(info), tamano_contenido, tipo_contenido);
    memcpy(respuesta + tamano_respuesta, cuerpo, tamano_contenido);
    //Mandalo todo!.
    int rv;
    if(rv = send(fd, respuesta, tamano_respuesta + tamano_contenido, 0) < 0){
        perror("send");
    }
    return rv;
}

//Manda una respuesta 404: No encontrado.
void Resp_404(int fd){
    char ruta_archivo[4096];
    file_data* datos_archivo;
    char* tipo_mime;

    //Buscar el archivo "404.html".
    snprintf(ruta_archivo, sizeof(ruta_archivo), "%s/404.html", ARCHIVOS_SERVIDOR);
    datos_archivo = Cargar_archivo(ruta_archivo);
    if(datos_archivo == NULL){
        fprintf(stderr, "No se pudo encontrar el archivo %s!.\n", ruta_archivo);
        exit(3);
    }

    tipo_mime = Obtener_tipo_mime(ruta_archivo);

    Send_response(fd, "HTTP/1.1 404 NOT FOUND", tipo_mime, datos_archivo->data, datos_archivo->tamano);
    Liberar_archivo(datos_archivo);
}

//Mandar una respuesta de punto final /d20.
void Get_d20(int fd){
        srand(time(NULL) + getpid());
        char str[8];
        int random = rand() % 20 + 1;
        int tamano = sprintf(str, "%d/n", random);
        Send_response(fd, "HTTP/1.1 200 OK", "text/plain", str, tamano);
}

 void Get_fecha(int fd) {
    time_t gmt_format;
    time(&gmt_format);
    char current[26]; // gmtime documentation stated that a user-supplied buffer.
                      // should have at least 26 bytes.
    int length = sprintf(current, "%s", asctime(gmtime(&gmt_format)));
    Send_response(fd, "HTTP/1.1 200 OK", "text/plain", current, length);
 }

 void Post_guardado(int fd, char* cuerpo){
     char* status;
     int archivo = open("data.txt", O_CREAT | O_WRONLY | O_APPEND, 0644);
     if(archivo < 0){
         status = "Fallido";
     } else {
         flock(archivo, LOCK_EX);
         write(archivo, cuerpo, strlen(cuerpo));
         flock(archivo, LOCK_UN);
         close(archivo);
         status = "ok";
     }

     char cuerpo_respuesta[128];
     int tamano = sprintf(cuerpo_respuesta, "{\"status\": \"%s\"}\n", status);
     Send_response(fd, "HTTP/1.1 200 OK", "application/json", cuerpo_respuesta, tamano);
    //Guardar el cuerpo y enviar respuesta.
 }

 int Obtener_archivo_o_cache(int fd, cache* cache, char* ruta_archivo){
     file_data* datos_archivo;
     entrada_cache* cacheent;
     char* tipo_mime;

     cacheent = Get_cache(cache, ruta_archivo);

     if(cacheent != NULL){
         Send_response(fd, "HTTP/1.1 200 OK", cacheent->tipo_contenido, cacheent->contenido, cacheent->tamano_contenido);
     } else {
         datos_archivo = Cargar_archivo(ruta_archivo);
         if(ruta_archivo == NULL) {
                return -1;
         }
         tipo_mime = Obtener_tipo_mime(ruta_archivo);
         Send_response(fd, "HTTP/1.1 200 OK", tipo_mime, datos_archivo->data, datos_archivo->tamano);
         Put_cache(cache, ruta_archivo, tipo_mime, datos_archivo->data, datos_archivo->tamano);
         Liberar_archivo(datos_archivo);
     }
     return 0;
 }

 void Obtener_archivo(int fd, cache* cache, char* ruta_solicitud){
     char ruta_archivo[65536];
     file_data* datos_archivo;
     char* tipo_mime;

     //Tratar de encontrar el archivo.
     snprintf(ruta_archivo, sizeof(ruta_archivo), "%s%s", ROOT_SERVIDOR, ruta_solicitud);
     datos_archivo = Cargar_archivo(ruta_archivo);
     if(datos_archivo == NULL){
         snprintf(ruta_archivo, sizeof(ruta_archivo), "%s%s/index.html", ROOT_SERVIDOR, ruta_solicitud);
         datos_archivo = Cargar_archivo(ruta_archivo);
         if(datos_archivo == NULL){
             Resp_404(fd);
             return;
         }
     }
     tipo_mime = Obtener_tipo_mime(ruta_archivo);
     Send_response(fd, "HTTP/1.1 200 OK", tipo_mime, datos_archivo->data, datos_archivo->tamano);
     Liberar_archivo(datos_archivo);
 }