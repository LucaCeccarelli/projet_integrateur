/*
 * ifnetshow_agent.c
 *
 * Persistent agent (server) for ifnetshow.
 * It listens for connections on a fixed port and returns network interface
 * information from the local machine.
 *
 * Supported commands (sent as plain text):
 *   - "ALL"
 *         => List all network interfaces with their IPv4/IPv6 addresses.
 *   - "IFNAME <ifname>"
 *         => List the addresses (with prefix) for the specified interface.
 *
 * The agent reuses the code from ifshow by including "ifshow.h".
 *
 * Compile with:
 *    gcc -o ifnetshow_agent ifnetshow_agent.c ifshow.c
 */

#include "ifshow.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

/*
 * process_request:
 *   Processes the command received from the client.
 *   The command can be:
 *       "ALL"            -> list all interfaces.
 *       "IFNAME <name>"  -> list the addresses for a specific interface.
 *
 *   This function opens a FILE stream on the client socket descriptor and then
 *   calls the appropriate ifshow function.
 */
void process_request(int client_fd, const char *request) {
    char mode[16];
    char ifname[128];
    int is_all = 0;

    memset(mode, 0, sizeof(mode));
    memset(ifname, 0, sizeof(ifname));

    if (strncmp(request, "ALL", 3) == 0) {
        is_all = 1;
    } else if (strncmp(request, "IFNAME", 6) == 0) {
        // Expected format: "IFNAME <ifname>"
        if (sscanf(request, "%s %s", mode, ifname) != 2) {
            dprintf(client_fd, "Invalid command format.\n");
            return;
        }
    } else {
        dprintf(client_fd, "Unknown command.\n");
        return;
    }

    /* Convert the client socket descriptor into a FILE stream
     * so that we can use our ifshow functions which print to a stream.
     */
    FILE *stream = fdopen(client_fd, "w");
    if (stream == NULL) {
        perror("fdopen");
        return;
    }

    if (is_all) {
        show_all_interfaces(stream);
    } else {
        show_interface_by_name(ifname, stream);
    }

    fflush(stream);
    /* Note: Do not fclose(stream) here if you do not want to close client_fd automatically.
     * In this example, we will let the main loop close the socket.
     */
}

int main() {
    int sockfd, client_fd;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    /* Create a TCP socket. */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Allow immediate reuse of the port. */
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    /* Bind the socket to all local interfaces on SERVER_PORT. */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /* Listen for incoming connections. */
    if (listen(sockfd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Agent server listening on port %d...\n", SERVER_PORT);

    /* Main loop: accept and process client connections. */
    while (1) {
        client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        /* Optionally, print the client's IP address. */
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("Accepted connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        /* Read the client's request. */
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        int n = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (n < 0) {
            perror("read");
            close(client_fd);
            continue;
        }

        process_request(client_fd, buffer);

        close(client_fd);
    }

    close(sockfd);
    return 0;
}
