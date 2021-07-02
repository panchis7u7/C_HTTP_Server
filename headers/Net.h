#ifndef _NET_C_
#define _NET_C_

struct sockaddr;
extern void* get_in_addr(struct sockaddr* sa);
extern int get_listening_socket(char* puerto);

#endif //!_NET_C_