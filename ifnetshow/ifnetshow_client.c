/*
 * ifnetshow_client.c
 *
 * This is the client program that connects to a remote machine (specified by its IP address)
 * and requests network interface information from the persistent agent.
 *
 * Usage:
 *   ifnetshow -n <addr> -i <ifname>   (to list the IPv4/IPv6 prefixes for the specified interface)
 *   ifnetshow -n <addr> -a             (to list all network interfaces and their IPv4/IPv6 prefixes)
 *
 * Compile with:
 *     gcc ifnetshow_client.c -o ifnetshow
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

/* usage:
 * Prints the correct command-line usage and exits.
 */
void usage(const char *progname) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s -n <addr> -i <ifname>\n", progname);
    fprintf(stderr, "  %s -n <addr> -a\n", progname);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    char *remote_addr = NULL;
    char *ifname = NULL;
    int list_all = 0;

    // The expected command-line options are:
    //   -n <addr> (remote agent IP address)
    //   -i <ifname> or -a
    if (argc < 4) {
        usage(argv[0]);
    }

    // Parse command-line arguments.
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0) {
            if (i + 1 < argc) {
                remote_addr = argv[i + 1];
                i++;
            } else {
                usage(argv[0]);
            }
        } else if (strcmp(argv[i], "-i") == 0) {
            if (i + 1 < argc) {
                ifname = argv[i + 1];
                i++;
            } else {
                usage(argv[0]);
            }
        } else if (strcmp(argv[i], "-a") == 0) {
            list_all = 1;
        } else {
            usage(argv[0]);
        }
    }

    // Ensure that a remote address is provided.
    if (remote_addr == NULL) {
        usage(argv[0]);
    }
    // Must specify either -i or -a (but not both).
    if (list_all == 0 && ifname == NULL) {
        usage(argv[0]);
    }
    if (list_all == 1 && ifname != NULL) {
        usage(argv[0]);
    }

    // Create a TCP socket.
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up the remote server address structure.
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, remote_addr, &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    // Connect to the remote agent.
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // Build the command string to send to the agent.
    char command[256];
    memset(command, 0, sizeof(command));
    if (list_all) {
        strcpy(command, "ALL");
    } else {
        snprintf(command, sizeof(command), "IFNAME %s", ifname);
    }

    // Send the command to the agent.
    if (write(sockfd, command, strlen(command)) < 0) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    // Read the response from the agent and display it.
    char buffer[BUFFER_SIZE];
    int n;
    while ((n = read(sockfd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[n] = '\0';
        printf("%s", buffer);
    }
    if (n < 0) {
        perror("read");
    }

    close(sockfd);
    return 0;
}
