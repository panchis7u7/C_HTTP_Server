#include "Cache.h"
#include "File.h"
#include "Mime.h"
#include "Net.h"
#include "Mysql.h"
#include "Api.h"
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

#define PUERTO "3490"
#define ARCHIVOS_SERVIDOR "./serverfiles"
#define ROOT_SERVIDOR "./serverroot"

//Prototipo de funciones.
// int enviar_respuesta(int, char*, char*, void*, int);
// void resp_404(int);
// void get_d20(int);
// void get_fecha(int);
// void post_guardado(int, char*);
// //int obtener_archivo_o_cache(int, cache*, char*);
// void obtener_archivo(int, struct cache*, char*);
// void handle_solicitud_http(int, struct cache*); 

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Send an HTTP response
 *
 * header:       "HTTP/1.1 404 NOT FOUND" or "HTTP/1.1 200 OK", etc.
 * content_type: "text/plain", etc.
 * body:         the data to send.
 *
 * Return the value from the send() function.
 */

int enviar_respuesta(int fd, char* cabeza, char* tipo_contenido, void* cuerpo, unsigned long long tamano_contenido){
    const int tamano_respuesta_maxima = 65536 + tamano_contenido;
    //const int tamano_respuesta_maxima = 262144;
    char* respuesta = (char*)malloc(tamano_respuesta_maxima*sizeof(char));
    //char respuesta[tamano_respuesta_maxima];
    char buffer[100];

    time_t tiempo;
    struct tm* info;
    time(&tiempo);
    info = localtime(&tiempo);
    strftime(buffer, 100, "%a %b %d %T %Z %Y", info);

    int tamano_respuesta = snprintf(respuesta, tamano_respuesta_maxima,
                                    "%s\n"
                                    "Access-Control-Allow-Origin: *\n"
                                    "Date: %s\n"
                                    "Connection: close\n"
                                    "Content-Length: %llu\n"
                                    "Content-Type: %s\n"
                                    "\n",
                                    cabeza, buffer, tamano_contenido, tipo_contenido);
    memcpy(respuesta + tamano_respuesta, cuerpo, tamano_contenido);
    tamano_respuesta += tamano_contenido;
    //Mandalo todo!. Guachar
    int rv = send(fd, respuesta, tamano_respuesta, 0);
    if(rv < 0){
        perror("send");
    }
    free(respuesta);
    return rv;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Mandar una respuesta de punto final /d20.
void get_d20(int fd){
        srand(time(NULL) + getpid());
        char str[8];
        int random = (rand() % (20 -1 + 1) + 1);
        int tamano = sprintf(str, "%d", random);
        enviar_respuesta(fd, "HTTP/1.1 200 OK", "text/plain", str, tamano);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 void get_fecha(int fd) {
    time_t gmt_format;
    time(&gmt_format);
    char current[26]; // gmtime documentation stated that a user-supplied buffer.
                      // should have at least 26 bytes.
    int length = sprintf(current, "%s", asctime(gmtime(&gmt_format)));
    enviar_respuesta(fd, "HTTP/1.1 200 OK", "text/plain", current, length);
 }

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void obtener_archivo(int fd, struct cache* cache, char* ruta_archivo){
    char ruta_abs[4096];
    char* tipo_mime; 
    file_data* datos_archivo;
    struct entrada_cache* cacheent;
    //Obtener Ruta.
    snprintf(ruta_abs, sizeof ruta_abs, "%s%s", ROOT_SERVIDOR, ruta_archivo);
    //Checar si archivo esta en cache.
    cacheent = get_cache(cache, ruta_abs);
    if(cacheent){
        printf(" => Cache.\n");
        enviar_respuesta(fd, "HTTP/1.1 200 OK", cacheent->tipo_contenido, cacheent->contenido, cacheent->tamano_contenido);
        return;
    } else {
        datos_archivo = cargar_archivo(ruta_abs);
        if (datos_archivo == NULL){
            if(strcmp(ruta_abs, "./serverroot/") == 0){
                // Guachar
                obtener_archivo(fd, cache, "/index.html");
                return;
            } else {
                resp_404(fd);
                return;
            }
        }
        printf(" => Archivo.\n");
        tipo_mime = obtener_tipo_mime(ruta_abs);
        enviar_respuesta(fd, "HTTP/1.1 200 OK", tipo_mime, datos_archivo->data, datos_archivo->tamano);
        //printf("\nruta-abs: %s\n", ruta_abs);
        put_cache(cache, ruta_abs, tipo_mime, datos_archivo->data, datos_archivo->tamano);
        liberar_archivo(datos_archivo);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 //Buscar el comienzo del cuerpo del paquete HTTP.
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

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 //Encargarse de la solicitud HTTP y mandar respuesta.
 void handle_solicitud_http(int fd, struct cache* cache, MYSQL* conn){
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

     //printf("\nPaquete: %s\n", solicitud);

     //Obtener el tipo de solicitud y la ruta .
     sscanf(solicitud, "%s %s %s", tipo_solicitud, ruta_solicitud, protocolo_solicitud);
     printf("Solicitud: %s %s %s\n", tipo_solicitud, ruta_solicitud, protocolo_solicitud);

     //Lamar los manejadores de funciones apropiados, con los datos recibidos.
     /* if (strcmp(tipo_solicitud, "GET") == 0) {
         if (strcmp(ruta_solicitud, "/d20") == 0){
             get_d20(fd);
         } else  {
             if(strcmp(obtener_tipo_mime(ruta_solicitud), "application/octet-stream") == 0)
                printf("\n\nsdsd\n\n");
             obtener_archivo(fd, cache, ruta_solicitud);
         }
     } else if (strcmp(tipo_solicitud, "POST") == 0) {
        if (strcmp(ruta_solicitud, "/save") == 0) {
            post_guardado(fd, cuerpo);
        } else {
            resp_404(fd);
        }
     } else {
         fprintf(stderr, "%sTipo de solicitud desconocida \"%s\"\n", KRED, tipo_solicitud);
         return;
    }  */  

    if(strcmp(tipo_solicitud, "GET") == 0){
        if(strcmp(obtener_tipo_mime(ruta_solicitud), "application/octet-stream") == 0)
            handleApi(fd, ruta_solicitud, conn, enviar_respuesta);
        else 
            obtener_archivo(fd, cache, ruta_solicitud);
    }
    return;
 }

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//char* get_in_addr(const struct sockaddr* sa, char* s, size_t longitud_maxima);

 int main(void){
     int newfd; //Escucha en sock_fd, nueva coneccion en newfd.
     struct sockaddr_storage info_addr;
     char s[INET6_ADDRSTRLEN];

    struct cache* cache = crear_cache(20, 0);

     //Obtener un socket oyente.
     int listenfd = obtener_socket_oyente(PUERTO);

     if (listenfd < 0) {
         fprintf(stderr, "\n%sServidor Web: Error fatal al obtener socket oyente.\n", KRED);
         exit(1);
     }

     MYSQL* conn;
     MYSQL_RES* result;
     MYSQL_ROW* row;

     MYSQL_CONN conn_data = {
         .host = "localhost",
         .user = "root",
         .password = "password",
         .dbname = "Estudiantes",
         .sock = NULL,
         .port = 3306,
         .flag = 0
     };

     conn = mysql_connect(&conn_data);
     
     
     printf("%sServidor Web: Esperando conecciones en el puerto %s...\n", KBLU, PUERTO);

     //Bucle principal que acepta conecciones entrantes.

     while(1) {
        socklen_t tamano_sin = sizeof info_addr;

        newfd = accept(listenfd, (struct sockaddr*)&info_addr, &tamano_sin);
        if(newfd == -1){
            perror("Accept.");
            continue;
        }

        //Imprime un mensaje de que obtuvimos una coneccion.
        inet_ntop(info_addr.ss_family, get_in_addr((struct sockaddr *)&info_addr), s, sizeof s);
        printf("%sServidor Web: Se obtuvo conexion de %s.\n", KMAG , s);

        handle_solicitud_http(newfd, cache, conn);
        close(newfd);
    }
 return 0;
 }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////