/*
 * neighborshow.c
 *
 * This command-line tool sends a UDP broadcast discovery request to find
 * neighboring machines. It waits for responses from agents running on other machines
 * and then prints the unique hostnames.
 *
 * Usage:
 *    neighborshow         (defaults to 1 hop)
 *    neighborshow -hop n  (n > 1 for multi‐hop discovery)
 *
 * Compile with:
 *     gcc -o neighborshow neighborshow.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>
#include <sys/select.h>
#include "neighborshow.h"

#define RESPONSE_TIMEOUT 3   /* seconds to wait for responses */
#define MAX_HOSTNAMES 100

typedef struct {
    char hostname[256];
} hostname_entry;

int main(int argc, char *argv[]) {
    int hop = 1;  /* default hop count is 1 */
    
    /* Parse optional command-line argument: -hop n */
    if (argc == 3) {
        if (strcmp(argv[1], "-hop") == 0) {
            hop = atoi(argv[2]);
            if (hop < 1)
                hop = 1;
        } else {
            fprintf(stderr, "Usage: %s [-hop n]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    } else if (argc != 1) {
        fprintf(stderr, "Usage: %s [-hop n]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Enable broadcast */
    int broadcastEnable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        perror("setsockopt (SO_BROADCAST)");
        exit(EXIT_FAILURE);
    }

    /* Bind the socket to an ephemeral port so we can receive responses */
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = 0;  /* system assigns port */
    if (bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /* Generate a random request id */
    srand(time(NULL) ^ getpid());
    int req_id = rand() % 1000000;

    /* Prepare the request message:
     *   "NEIGHBOR_REQUEST <req_id> <hop>"
     */
    char request[MAX_BUFFER];
    snprintf(request, sizeof(request), "%s %d %d", REQUEST_PREFIX, req_id, hop);

    /* Set up the broadcast address */
    struct sockaddr_in broadcast_addr;
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(NEIGHBOR_PORT);
    broadcast_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

    /* Send the broadcast discovery request */
    if (sendto(sockfd, request, strlen(request), 0,
               (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) < 0) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }

    /* Wait for responses for a fixed timeout */
    fd_set read_fds;
    struct timeval timeout;
    timeout.tv_sec = RESPONSE_TIMEOUT;
    timeout.tv_usec = 0;

    hostname_entry hostnames[MAX_HOSTNAMES];
    int hostname_count = 0;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        int ret = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);
        if (ret < 0) {
            perror("select");
            break;
        } else if (ret == 0) {
            /* Timeout expired */
            break;
        }

        char buffer[MAX_BUFFER];
        struct sockaddr_in sender_addr;
        socklen_t sender_len = sizeof(sender_addr);
        int n = recvfrom(sockfd, buffer, MAX_BUFFER - 1, 0,
                         (struct sockaddr *)&sender_addr, &sender_len);
        if (n < 0) {
            perror("recvfrom");
            break;
        }
        buffer[n] = '\0';

        /* Expected response format:
         *   "NEIGHBOR_RESPONSE <req_id> <hostname>"
         */
        char prefix[32];
        int resp_id;
        char resp_hostname[256];
        if (sscanf(buffer, "%31s %d %255s", prefix, &resp_id, resp_hostname) != 3)
            continue;
        if (strcmp(prefix, RESPONSE_PREFIX) != 0 || resp_id != req_id)
            continue;

        /* Check if this hostname is already in our list */
        int exists = 0;
        for (int i = 0; i < hostname_count; i++) {
            if (strcmp(hostnames[i].hostname, resp_hostname) == 0) {
                exists = 1;
                break;
            }
        }
        if (!exists && hostname_count < MAX_HOSTNAMES) {
            strcpy(hostnames[hostname_count].hostname, resp_hostname);
            hostname_count++;
        }
    }

    /* Print the list of unique neighboring hostnames */
    printf("Neighboring machines (hop = %d):\n", hop);
    for (int i = 0; i < hostname_count; i++) {
        printf("  %s\n", hostnames[i].hostname);
    }

    close(sockfd);
    return 0;
}
