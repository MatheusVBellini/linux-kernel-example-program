#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"

void initialize_socket(int *sockfd, struct sockaddr_in *server_addr, const char *ip, int port) {
    // create socket for through the internet using TCP
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd < 0) {
        perror("failed to create socket");
        exit(1);
    }

    // set server address
    memset(server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port);

    // convert IPv4 and IPv6 addresses to binary form
    if(inet_pton(AF_INET, ip, &server_addr->sin_addr) < 0) {
        perror("invalid IP address");
        exit(2);
    }

    // connect to the server
    if (connect(*sockfd, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) {
        perror("connection failed");
        exit(3);
    }
}

void usage_warning(const char *program_name) {
    printf("\nUsage: \n\t%s <IP address> <PORT> [-o <output file>]\n\n", program_name);
    printf("Options:\n");
    printf("\t-o \t\t\t Outputs to a specific destination file\n");
    exit(1);
}