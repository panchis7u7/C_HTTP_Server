#include "../headers/Cache.h"
#include "../headers/File.h"
#include "../headers/Mime.h"
#include "../headers/Net.h"
#include "../headers/Mysql.h"
#include "../headers/Api.h"
#include "../headers/Queue.h"
#include "../headers/Utils.h"
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
#include <stdarg.h>

#define PUERTO "3490"
#define ARCHIVOS_SERVIDOR "./serverfiles"
#define ROOT_SERVIDOR "./serverroot"
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
void obtener_archivo(int, struct args*);
void handleHttpRequest(int fd, struct args*);
void* threadFunc();

int listenfd = 0;
char* CORS = "Access-Control-Allow-Headers: *\r\nAccess-Control-Allow-Methods: GET, POST, OPTIONS, PUT, PATCH, DELETE\r\n";
pthread_t threadPool[THREADPOOL_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t conditioVariable = PTHREAD_COND_INITIALIZER;

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

void obtener_archivo(int fd, struct args* args){
    char ruta_abs[4096];
    char* tipo_mime; 
    file_data* datos_archivo;
    struct cache_entry* cacheent;
    //Obtener Ruta.
    snprintf(ruta_abs, sizeof ruta_abs, "%s%s", ROOT_SERVIDOR, args->request_info->request);
    //Checar si archivo esta en cache.
    cacheent = get_cache(args->cache, ruta_abs);
    if(cacheent){

        printf("%s\nStratus WebServer: Got connection from %s.\n%sRequest: %s %s %s => %sCache.\n", 
                KMAG, args->request_info->ip_addr, KWHT, 
                args->request_info->protocol,
                args->request_info->request_type, 
                args->request_info->request,KYEL);

        sendResponse(fd, "HTTP/1.1 200 OK", cacheent->content_type, cacheent->content, cacheent->content_len, "");
        return;
    } else {

        printf("%s\nStratus WebServer: Got connection from %s.\n%sRequest: %s %s %s => %sFile.\n", 
                KMAG, args->request_info->ip_addr, KWHT, 
                args->request_info->protocol,
                args->request_info->request_type, 
                args->request_info->request, KBLU);

        datos_archivo = cargar_archivo(ruta_abs);
        if (datos_archivo == NULL){
            if(strcmp(ruta_abs, "./serverroot/") == 0){
                // Guachar
                args->request_info->request = "/index.html";
                obtener_archivo(fd, args);
                return;
            } else {
                resp_404(fd);
                return;
            }
        }
        tipo_mime = obtener_tipo_mime(ruta_abs);
        sendResponse(fd, "HTTP/1.1 200 OK", tipo_mime, datos_archivo->data, datos_archivo->tamano, "");
        //printf("\nruta-abs: %s\n", ruta_abs);
        put_cache(args->cache, ruta_abs, tipo_mime, datos_archivo->data, datos_archivo->tamano);
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

 void* threadFunc(void* args){
     struct args* data = (struct args*)args;
     int* clientSocket = NULL;
     while (1)
     {   pthread_mutex_lock(&mutex);
         pthread_cond_wait(&conditioVariable, &mutex);
         clientSocket = removeQueue();
         pthread_mutex_unlock(&mutex);
         if(clientSocket != NULL){
            handleHttpRequest(*clientSocket, data);
            close(*clientSocket);
            free(clientSocket);
         }
     }
 }

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 //Encargarse de la solicitud HTTP y mandar respuesta.
 void handleHttpRequest(int fd, struct args* args){
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

     //Obtener el tipo de solicitud y la ruta .
     sscanf(solicitud, "%s %s %s", tipo_solicitud, ruta_solicitud, protocolo_solicitud);

     args->request_info->request = cleanHttp(ruta_solicitud);
     args->request_info->request_type = tipo_solicitud;
     args->request_info->protocol = protocolo_solicitud;
     //printf("%sRequest: %s %s %s", KWHT, tipo_solicitud, ruta_solicitud, protocolo_solicitud);  
    
    if(strcmp(tipo_solicitud, "GET") == 0){
        if(strcmp(obtener_tipo_mime(ruta_solicitud), "application/octet-stream") == 0) {
            printf("%s\nStratus WebServer: Got connection from %s.\n%sRequest: %s %s %s => %sAPI.\n", 
                KMAG, args->request_info->ip_addr, KWHT, 
                args->request_info->protocol,
                args->request_info->request_type, 
                args->request_info->request, KYEL);

            handleGetApi(fd, ruta_solicitud, args, sendResponse);
        } else
            obtener_archivo(fd, args);
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

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 int main(void){
     int newfd; //Escucha en sock_fd, nueva coneccion en newfd.
     struct sockaddr_storage info_addr;
     char s[INET6_ADDRSTRLEN];
     int i;

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

    Cache* cache = create_cache(20, 0);

     MYSQL* conn;

     MYSQL_CONN conn_data = {
         .host = "localhost",
         .user = "panchis",
         .password = "pass",
         .dbname = "Estudiantes",
         .sock = NULL,
         .port = 3306,
         .flag = 0
     };

     conn = mysql_connect(&conn_data);
     struct args* args = (struct args*)malloc(sizeof(struct args));
     args->cache = cache;
     args->conn = conn;
     args->request_info = (request_info*)malloc(sizeof(request_info));

     printf("%sStratus WebServer: Waiting for new connections on port %s...\n", KBLU, PUERTO);

     for(i = 0; i < THREADPOOL_SIZE; ++i){
         if(pthread_create(&threadPool[i], NULL, threadFunc, args) != 0){
             perror("Failed to create thread\n");
         }
     }

     while(1) {
        socklen_t tamano_sin = sizeof info_addr;

        newfd = accept(listenfd, (struct sockaddr*)&info_addr, &tamano_sin);
        if(newfd == -1){
            perror("Accept.");
            continue;
        }

        //Imprime un mensaje de que obtuvimos una coneccion.
        inet_ntop(info_addr.ss_family, get_in_addr((struct sockaddr*)&info_addr), s, sizeof s);
        args->request_info->ip_addr = s;
        //printf("%s\nStratus WebServer: Got connection from %s.\n", KMAG , s);

        int* clientSocket = (int*)malloc(sizeof(int));
        *clientSocket = newfd;

        pthread_mutex_lock(&mutex);
        insertQueue(clientSocket);
        pthread_cond_signal(&conditioVariable);
        pthread_mutex_unlock(&mutex);
    }

    for(int i = 0; i < THREADPOOL_SIZE; ++i){
         if(pthread_join(threadPool[i], NULL) != 0){
             perror("Failed to join thread\n");
         }
     }

    return 0;
 }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
