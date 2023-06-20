#include <stdio.h>
#include <stdlib.h>
#include "client.h"

// receive desired IP address and PORT from command line as well as optional output file
int main(int argc, char const *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;
    char filename[1024];

    // command usage check
    if (argc == 3) {
        strcpy(filename, "log/log.txt");
    } else if ((strcmp(argv[3], "-o") == 0) && (argc == 5)) {
        strcpy(filename, argv[4]);
    } else {
        usage_warning(argv[0]);
        return 1;
    }

    // initialize socket
    initialize_socket(&sockfd, &server_addr, argv[1], atoi(argv[2]));

    // write received information to file
    char buffer[1024];
    FILE *fp = fopen(filename, "w");
    while (1) {
        if (read(sockfd, buffer, sizeof(buffer)) < 0) {
            perror("read failed");
            exit(4);
        }
        fprintf(fp, "%s", buffer);
        fflush(fp);
    }

    close(sockfd);
    fclose(fp);

    return 0;
}