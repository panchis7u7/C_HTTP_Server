#include "Net.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <features.h>

#define BACKLOG 10  //Numero de conecciones pendientes que la cola almacenara.

//Esto obtiene una direccion de internet, ya sea IPV4 o IPV6.
char* get_in_addr(const struct sockaddr* sa, char* s, size_t longitud_maxima){
    switch(sa->sa_family){
        case AF_INET:
            inet_ntop(AF_INET, &(((struct sockaddr_in*)sa)->sin_addr), s, longitud_maxima);
            break;
        case AF_INET6:
            inet_ntop(AF_INET6, &(((struct sockaddr_in6*)sa)->sin6_addr), s, longitud_maxima);
            break;
        default:
            strncpy(s, "Unknown AF", longitud_maxima);
            return NULL;
    }
    return s;
}

//Regresar el socket oyente (listening) principal.
int obtener_socket_oyente(char* puerto){
    int sockfd; 
    struct addrinfo hints, *servinfo, *p;
    int true = 1;
    int rv;

    //Este bloque mira a las interfaces de area local e
    //intenta encontrar algunas que concuerden con nuestros requerimientos
    //ya sea IPV4 o IPV6 (AF_UNSPEC) y TCP (SOCK_STREAM) y usar cualquier IP en
    //esta maquina (AI_PASSIVE).

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;  //usar mi ip.

    if((rv = getaddrinfo(NULL, puerto, &hints, &servinfo)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }

    //Una vez que tengamos una lista de potenciales interfaces, recorrerlas
    //y tratar de preparar un socket en cada una. Parar de recorrerlasla primera vez
    //que se haya tenido exito.

    for(p = servinfo; p != NULL; p = p->ai_next){
        //Trata de hacer un socket basado en la interfaz candidata.
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            continue;
        }
        //SO_REUSEADDR previene errores del tipo: "La direccion ya esta ocupada"
        //que comunmente se presentan al testear servidores.
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int)) == -1){
            perror("setsockopt");
            close(sockfd);
            freeaddrinfo(servinfo); //Todo terminado con esta estructura.
            return -2;
        }
        //A ver si podemos enlazar (bind) este socket con esta direccion IP local. Esto
        //asocia el descriptor de archivo (sockfd) en el que deberemos
        //leer y escribir con una direccion IP especifica,
        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            close(sockfd);
            continue;
        }
        //Si se llego hasta aqui, tenemos un socket enlazado y estamos listos!.
        break;
    }

    //Todo terminado con esta estructura.
    freeaddrinfo(servinfo);
    
    //Si p es NULL, significa que no rompimos fuera del bucle y 
    //no tenemos un buen socket.
    if(p == NULL){
        fprintf(stderr, "Servidor: Error al encontrar una direccion local.");
        return -3;
    }

    //Empieza a escuchar. Esto es lo que permitira a computadoras remotas
    //conectarse a este socket/IP.
    if(listen(sockfd, BACKLOG) == -1){
        close(sockfd);
        return -4;
    }
    return sockfd;
}