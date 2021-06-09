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
#include <pthread.h>

#define PUERTO "3490"
#define ARCHIVOS_SERVIDOR "../serverfiles"
#define ROOT_SERVIDOR "../serverroot"
#define THREADPOOL_SIZE 20

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

//Prototipo de funciones.
int sendResponse(int, char*, char*, void*, unsigned long long, char*);
void resp_404(int);
void get_d20(int);
void get_fecha(int);
void post_guardado(int, char*);
void obtener_archivo(int, struct cache*, char*);
void handle_solicitud_http(int, struct cache*, MYSQL*); 
void* threadFunc();

int listenfd = 0;
char* CORS = "Access-Control-Allow-Headers: *\r\nAccess-Control-Allow-Methods: GET, POST, OPTIONS, PUT, PATCH, DELETE\r\n";
pthread_t thread_pool[THREADPOOL_SIZE];

char* cleanHttp(char* str){
    char* index;
    if((index = strstr(str, "%20")) != NULL){
        index[0] = ' ';
        strncpy(index+1, index+3, strlen(index));
    }
    if((index = strstr(str, "'")) != NULL){
        strncpy(index, index+1, strlen(index));
    }
   return str;
}

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

int sendResponse(int fd, char* cabeza, char* contentType, void* body, unsigned long long contentLen, char* flags){
    const int maxResponseLen = 65536 + contentLen;
    //const int tamano_respuesta_maxima = 262144;
    char* response = (char*)malloc(maxResponseLen*sizeof(char));
    //char respuesta[tamano_respuesta_maxima];
    char buffer[100];

    time_t currentTime;
    struct tm* info;
    time(&currentTime);
    info = localtime(&currentTime);
    strftime(buffer, 100, "%a %b %d %T %Z %Y", info);

    int responseLen = snprintf(response, maxResponseLen,
                                    "%s\n"
                                    "Access-Control-Allow-Origin: *\n"
                                    "%s"
                                    "Date: %s\n"
                                    "Connection: close\n"
                                    "Content-Length: %llu\n"
                                    "Content-Type: %s\n"
                                    "\n",
                                    cabeza, flags, buffer, contentLen, contentType);
    memcpy(response + responseLen, body, contentLen);
    responseLen += contentLen;
    //Mandalo todo!. Guachar
    int rv = send(fd, response, responseLen, 0);
    if(rv < 0){
        perror("send");
        return -1;
    }
    free(response);
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
    sendResponse(fd, "HTTP/1.1 404 NOT FOUND", tipo_mime, datos_archivo->data, datos_archivo->tamano, "");
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
        sendResponse(fd, "HTTP/1.1 200 OK", "text/plain", str, tamano, "");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 void get_fecha(int fd) {
    time_t gmt_format;
    time(&gmt_format);
    char current[26]; // gmtime documentation stated that a user-supplied buffer.
                      // should have at least 26 bytes.
    int length = sprintf(current, "%s", asctime(gmtime(&gmt_format)));
    sendResponse(fd, "HTTP/1.1 200 OK", "text/plain", current, length, "");
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
     sendResponse(fd, "HTTP/1.1 200 OK", "application/json", cuerpo_respuesta, tamano, "");
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
        printf(" %s=> Cache.", KBLU);
        sendResponse(fd, "HTTP/1.1 200 OK", cacheent->tipo_contenido, cacheent->contenido, cacheent->tamano_contenido, "");
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
        printf(" %s=> Archivo.", KYEL);
        tipo_mime = obtener_tipo_mime(ruta_abs);
        sendResponse(fd, "HTTP/1.1 200 OK", tipo_mime, datos_archivo->data, datos_archivo->tamano, "");
        //printf("\nruta-abs: %s\n", ruta_abs);
        put_cache(cache, ruta_abs, tipo_mime, datos_archivo->data, datos_archivo->tamano);
        liberar_archivo(datos_archivo);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 //Buscar el comienzo del cuerpo del paquete HTTP.
 char* encontrar_inicio_cuerpo(char* header){
     char* begin;
     if((begin = strstr(header, "\r\n\r\n")) != NULL) {
         return begin + 2;
     } else if ((begin = strstr(header, "\n\n")) != NULL) {
         return begin + 2;
     } else if((begin = strstr(header, "\r\r")) != NULL){
         return begin + 2;
     } else {
         return begin; 
     } 
 }

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 //Encargarse de la solicitud HTTP y mandar respuesta.
 void handleHttpRequest(int fd, struct cache* cache, MYSQL* conn){
     const int tamano_buffer_solicitud = 65536;
     char solicitud[tamano_buffer_solicitud];
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

     //printf("\nPaquete: %s\n", solicitud);

     //Obtener el tipo de solicitud y la ruta .
     sscanf(solicitud, "%s %s %s", tipo_solicitud, ruta_solicitud, protocolo_solicitud);
     printf("%sSolicitud: %s %s %s", KWHT, tipo_solicitud, ruta_solicitud, protocolo_solicitud);  
    
    if(strcmp(tipo_solicitud, "GET") == 0){
        if(strcmp(obtener_tipo_mime(ruta_solicitud), "application/octet-stream") == 0)
            handleGetApi(fd, ruta_solicitud, conn, sendResponse);
        else
            obtener_archivo(fd, cache, cleanHttp(ruta_solicitud));
    } else if(strcmp(tipo_solicitud, "OPTIONS") == 0){
        sendResponse(fd, "HTTP/1.1 200 OK", "application/json", "", 0, CORS);
    }
    return;
 }

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 //Gracefully quit the program.
 void signalHandler(){
     write(STDOUT_FILENO, "\n\x1B[31mQuitting...\n\x1B[37m", 23);
     close(listenfd);
     exit(0);
 }

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


 void* threadFunc(){
     return NULL;
 }

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 int main(void){
     int newfd; //Escucha en sock_fd, nueva coneccion en newfd.
     struct sockaddr_storage info_addr;
     char s[INET6_ADDRSTRLEN];

     //Obtener un socket oyente.
     listenfd = getListeningSocket(PUERTO);

     if (listenfd < 0) {
         fprintf(stderr, "\n%sStratus WebServer: Fatal error at opening listening socket.\n", KRED);
         exit(1);
     }

     printf("%s-----------------------------------------------------------\n", KGRN);
     printf("%s   | Created by: Carlos Sebastian Madrigal Rodriguez. |\n", KGRN);
     printf("%s-----------------------------------------------------------\n\n", KGRN);

     struct sigaction sa;
     sa.sa_handler = signalHandler;

     //handle ^C quitting and Seg faults.
     sigaction(SIGINT, &sa,NULL);
     sigaction(SIGSEGV, &sa, NULL);

    for(int i = 0; i < THREADPOOL_SIZE; i++){
        pthread_create(&(thread_pool[i]), NULL, threadFunc, NULL);
    }

    struct cache* cache = crear_cache(20, 0);

     MYSQL* conn;

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
     
     printf("%sStratus WebServer: Waiting for new conections on port %s...\n", KBLU, PUERTO);

     //Bucle principal que acepta conecciones entrantes.

     while(1) {
        socklen_t tamano_sin = sizeof info_addr;

        newfd = accept(listenfd, (struct sockaddr*)&info_addr, &tamano_sin);
        if(newfd == -1){
            perror("Accept.");
            continue;
        }

        //Imprime un mensaje de que obtuvimos una coneccion.
        inet_ntop(info_addr.ss_family, get_in_addr((struct sockaddr*)&info_addr), s, sizeof s);
        printf("%s\nStratus WebServer: Got connection from %s.\n", KMAG , s);

        handleHttpRequest(newfd, cache, conn);
        close(newfd);
    }
 return 0;
 }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////