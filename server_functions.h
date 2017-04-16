#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

//AUTH: Keller Hood
struct serv {
	int tcp_fd;
	int udp_fd;
};
struct serv *init_serv(const char ip[14], int port);
void close_serv(struct serv *server);

//AUTH: Jorge Macias
void domorethanstuff(int);

#endif