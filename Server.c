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

#define PUERTO "6969"
#define ARCHIVOS_SERVIDOR "/.serverfiles"
#define ROOT_SERVIDOR "./serveroot"

//Prototipo de funciones.
int enviar_respuesta(int, char*, char*, void*, int);
void resp_404(int);
void get_d20(int);
void get_fecha(int);
void post_guardado(int, char*);
int obtener_archivo_o_cache(int, cache*, char*);
void obtener_archivo(int, cache*, char*);

/**
 * Send an HTTP response
 *
 * header:       "HTTP/1.1 404 NOT FOUND" or "HTTP/1.1 200 OK", etc.
 * content_type: "text/plain", etc.
 * body:         the data to send.
 *
 * Return the value from the send() function.
 */

int enviar_respuesta(int fd, char* cabeza, char* tipo_contenido, void* cuerpo, int tamano_contenido){
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
void resp_404(int fd){
    char ruta_archivo[4096];
    file_data* datos_archivo;
    char* tipo_mime;

    //Buscar el archivo "404.html".
    snprintf(ruta_archivo, sizeof(ruta_archivo), "%s/404.html", ARCHIVOS_SERVIDOR);
    datos_archivo = cargar_archivo(ruta_archivo);
    if(datos_archivo == NULL){
        fprintf(stderr, "No se pudo encontrar el archivo %s!.\n", ruta_archivo);
        exit(3);
    }

    tipo_mime = obtener_tipo_mime(ruta_archivo);
    enviar_respuesta(fd, "HTTP/1.1 404 NOT FOUND", tipo_mime, datos_archivo->data, datos_archivo->tamano);
    liberar_archivo(datos_archivo);
}

//Mandar una respuesta de punto final /d20.
void get_d20(int fd){
        srand(time(NULL) + getpid());
        char str[8];
        int random = rand() % 20 + 1;
        int tamano = sprintf(str, "%d/n", random);
        enviar_respuesta(fd, "HTTP/1.1 200 OK", "text/plain", str, tamano);
}

 void get_fecha(int fd) {
    time_t gmt_format;
    time(&gmt_format);
    char current[26]; // gmtime documentation stated that a user-supplied buffer.
                      // should have at least 26 bytes.
    int length = sprintf(current, "%s", asctime(gmtime(&gmt_format)));
    enviar_respuesta(fd, "HTTP/1.1 200 OK", "text/plain", current, length);
 }

 void post_guardado(int fd, char* cuerpo){
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
     enviar_respuesta(fd, "HTTP/1.1 200 OK", "application/json", cuerpo_respuesta, tamano);
    //Guardar el cuerpo y enviar respuesta.
 }

 int obtener_archivo_o_cache(int fd, cache* cache, char* ruta_archivo){
     file_data* datos_archivo;
     entrada_cache* cacheent;
     char* tipo_mime;

     cacheent = get_cache(cache, ruta_archivo);

     if(cacheent != NULL){
         enviar_respuesta(fd, "HTTP/1.1 200 OK", cacheent->tipo_contenido, cacheent->contenido, cacheent->tamano_contenido);
     } else {
         datos_archivo = cargar_archivo(ruta_archivo);
         if(ruta_archivo == NULL) {
                return -1;
         }
         tipo_mime = obtener_tipo_mime(ruta_archivo);
         enviar_respuesta(fd, "HTTP/1.1 200 OK", tipo_mime, datos_archivo->data, datos_archivo->tamano);
         put_cache(cache, ruta_archivo, tipo_mime, datos_archivo->data, datos_archivo->tamano);
         liberar_archivo(datos_archivo);
     }
     return 0;
 }

 void obtener_archivo(int fd, cache* cache, char* ruta_solicitud){
     char ruta_archivo[65536];
     file_data* datos_archivo;
     char* tipo_mime;

     //Tratar de encontrar el archivo.
     snprintf(ruta_archivo, sizeof(ruta_archivo), "%s%s", ROOT_SERVIDOR, ruta_solicitud);
     datos_archivo = cargar_archivo(ruta_archivo);
     if(datos_archivo == NULL){
         snprintf(ruta_archivo, sizeof(ruta_archivo), "%s%s/index.html", ROOT_SERVIDOR, ruta_solicitud);
         datos_archivo = cargar_archivo(ruta_archivo);
         if(datos_archivo == NULL){
             resp_404(fd);
             return;
         }
     }
     tipo_mime = obtener_tipo_mime(ruta_archivo);
     enviar_respuesta(fd, "HTTP/1.1 200 OK", tipo_mime, datos_archivo->data, datos_archivo->tamano);
     liberar_archivo(datos_archivo);
 }

 //Buscar el comienzo del archivo.
 char* encontrar_inicio_cuerpo(char* cabezilla){
     char* inicio;
     if((inicio = strstr(cabezilla, "\r\n\r\n")) != NULL) {
         return inicio + 2;
     } else if ((inicio = strstr(cabezilla, "\n\n")) != NULL) {
         return inicio + 2;
     } else if((inicio = strstr(cabezilla, "\r\r")) != NULL){
         return inicio + 2;
     } else {
         return inicio; 
     }
 }

 //Encargarse de la solicitud HTTP y mandar respuesta.
 void handle_solicitud_http(int fd, struct cache* cache){
     const int tamano_buffer_solicitud = 65536;
     char solicitud[tamano_buffer_solicitud];
     char* p;
     char tipo_solicitud[8]; //Get o Post.
     char ruta_solicitud[1024];  //info etc.
     char protocolo_solicitud[128]; //HTTP/1.1.

     //Leer solicitud;
     int bytes_rcvd = recv(fd, solicitud, tamano_buffer_solicitud - 1, 0);
     if (bytes_rcvd < 0){
         perror("recv.");
         return;
     }

     //Poner un terminador de cadena al final de la solicitud.
     solicitud[bytes_rcvd] = '\0';

     p = encontrar_inicio_cuerpo(solicitud);

     char* cuerpo = p + 1;

     //Obtener el tipo de solicitud y la ruta .
     sscanf(solicitud, "%s %s %s", tipo_solicitud, ruta_solicitud, protocolo_solicitud);
     printf("Solicitud: %s %s %s\n", tipo_solicitud, ruta_solicitud, protocolo_solicitud);

     //Lamar los manejadores de funciones apropiados, con los datos recibidos.
     if (strcmp(tipo_solicitud, "GET") == 0) {
         if (strcmp(ruta_solicitud, "/d20") == 0){
             get_d20(fd);
         } else  {
             obtener_archivo(fd, cache, ruta_solicitud);
         }
     } else if (strcmp(tipo_solicitud, "POST") == 0) {
        if (strcmp(ruta_solicitud, "/save") == 0) {
            post_guardado(fd, cuerpo);
        } else {
            resp_404(fd);
        }
     } else {
         fprintf(stderr, "Tipo de solicitud desconocida \"%s\"\n", tipo_solicitud);
         return;
    }    
 }

 int main(void){
     int newfd; //Escucha en sock_fd, nueva coneccion en newfd.
     struct sockaddr_storage info_addr;
     char s[INET6_ADDRSTRLEN];

     struct cache* cache = crear_cache(10, 0);

     //Obtener un socket oyente.
     int listenfd = obtener_socket_oyente(PUERTO);

     if (listenfd < 0) {
         fprintf(stderr, "Servidor Web: Error fatal al obtener socket oyente.\n");
         exit(1);
     }

     printf("Servidor Web: Esperando conecciones en el puerto %s...\n", PUERTO);

     //Bucle principal que acepta conecciones entrantes.

     while(1){
         socklen_t tamano_sin = sizeof info_addr;

         newfd = accept(listenfd, (struct sockaddr*)&info_addr, &tamano_sin);
         if(newfd == -1){
             perror("Accept.");
             continue;
         }

         //Imprime un mensaje de que obtuvimos una coneccion.
         get_in_addr(((struct sockaddr*)&info_addr), s, sizeof(s));
         printf("Servidor Web: Se obtuvo coneccion de %s\n.", s);

        handle_solicitud_http(newfd, cache);
        close(newfd);
 }

 return 0;
 }
