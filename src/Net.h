#ifndef _NET_C_
#define _NET_C_

struct sockaddr;
void* get_in_addr(struct sockaddr* sa);
int getListeningSocket(char* puerto);

#endif //!_NET_C_