#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void initialize_socket(int *sockfd, struct sockaddr_in *server_addr, const char *ip, int port);
void usage_warning(const char *program_name);

#endif