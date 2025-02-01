#include "ifshow.h"
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

/* 
 * get_prefix_length:
 *   Implementation for both IPv4 and IPv6.
 */
int get_prefix_length(struct sockaddr *netmask) {
    int prefix = 0;
    if (netmask == NULL)
        return prefix;
    if (netmask->sa_family == AF_INET) {
        struct sockaddr_in *mask = (struct sockaddr_in *) netmask;
        unsigned char *p = (unsigned char *) &mask->sin_addr;
        for (int i = 0; i < 4; i++) {
            unsigned char byte = p[i];
            for (int j = 0; j < 8; j++) {
                if (byte & 0x80)
                    prefix++;
                else
                    break;  // assumes a contiguous mask
                byte <<= 1;
            }
        }
    } else if (netmask->sa_family == AF_INET6) {
        struct sockaddr_in6 *mask6 = (struct sockaddr_in6 *) netmask;
        unsigned char *p = mask6->sin6_addr.s6_addr;
        for (int i = 0; i < 16; i++) {
            unsigned char byte = p[i];
            for (int j = 0; j < 8; j++) {
                if (byte & 0x80)
                    prefix++;
                else
                    break;
                byte <<= 1;
            }
        }
    }
    return prefix;
}

/*
 * show_all_interfaces:
 *   Retrieve the list of interfaces using getifaddrs() and print for each unique interface
 *   its name followed by its IPv4/IPv6 addresses (in prefix notation) to the provided stream.
 */
int show_all_interfaces(FILE *stream) {
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1) {
        fprintf(stream, "Error retrieving interface information.\n");
        return -1;
    }
    
    #define MAX_INTERFACES 128
    char *iface_names[MAX_INTERFACES];
    int iface_count = 0;
    
    /* Collect unique interface names that have valid IP addresses */
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;
        if (ifa->ifa_addr->sa_family != AF_INET && ifa->ifa_addr->sa_family != AF_INET6)
            continue;
        int exists = 0;
        for (int i = 0; i < iface_count; i++) {
            if (strcmp(iface_names[i], ifa->ifa_name) == 0) {
                exists = 1;
                break;
            }
        }
        if (!exists) {
            iface_names[iface_count] = strdup(ifa->ifa_name);
            if (iface_names[iface_count] == NULL) {
                fprintf(stream, "Memory allocation error.\n");
                freeifaddrs(ifaddr);
                return -1;
            }
            iface_count++;
            if (iface_count >= MAX_INTERFACES)
                break;
        }
    }
    
    /* Print each interface and its addresses */
    for (int i = 0; i < iface_count; i++) {
        fprintf(stream, "%s:\n", iface_names[i]);
        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == NULL)
                continue;
            if (strcmp(iface_names[i], ifa->ifa_name) != 0)
                continue;
            if (ifa->ifa_addr->sa_family == AF_INET || ifa->ifa_addr->sa_family == AF_INET6) {
                char addr_str[INET6_ADDRSTRLEN];
                if (ifa->ifa_addr->sa_family == AF_INET) {
                    struct sockaddr_in *addr_in = (struct sockaddr_in *)ifa->ifa_addr;
                    if (inet_ntop(AF_INET, &addr_in->sin_addr, addr_str, sizeof(addr_str)) == NULL)
                        continue;
                } else {
                    struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *)ifa->ifa_addr;
                    if (inet_ntop(AF_INET6, &addr_in6->sin6_addr, addr_str, sizeof(addr_str)) == NULL)
                        continue;
                }
                int prefix = 0;
                if (ifa->ifa_netmask != NULL)
                    prefix = get_prefix_length(ifa->ifa_netmask);
                fprintf(stream, "  %s/%d\n", addr_str, prefix);
            }
        }
        free(iface_names[i]);
    }
    
    freeifaddrs(ifaddr);
    return 0;
}

/*
 * show_interface_by_name:
 *   Retrieve the list of interfaces and print only those addresses associated with the
 *   interface whose name is given by ifname.
 */
int show_interface_by_name(const char *ifname, FILE *stream) {
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1) {
        fprintf(stream, "Error retrieving interface information.\n");
        return -1;
    }
    
    int found = 0;
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;
        if (strcmp(ifa->ifa_name, ifname) != 0)
            continue;
        if (ifa->ifa_addr->sa_family == AF_INET || ifa->ifa_addr->sa_family == AF_INET6) {
            found = 1;
            char addr_str[INET6_ADDRSTRLEN];
            if (ifa->ifa_addr->sa_family == AF_INET) {
                struct sockaddr_in *addr_in = (struct sockaddr_in *)ifa->ifa_addr;
                if (inet_ntop(AF_INET, &addr_in->sin_addr, addr_str, sizeof(addr_str)) == NULL)
                    continue;
            } else {
                struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *)ifa->ifa_addr;
                if (inet_ntop(AF_INET6, &addr_in6->sin6_addr, addr_str, sizeof(addr_str)) == NULL)
                    continue;
            }
            int prefix = 0;
            if (ifa->ifa_netmask != NULL)
                prefix = get_prefix_length(ifa->ifa_netmask);
            fprintf(stream, "%s/%d\n", addr_str, prefix);
        }
    }
    
    if (!found) {
        fprintf(stream, "Interface '%s' not found or has no IP addresses.\n", ifname);
    }
    
    freeifaddrs(ifaddr);
    return 0;
}
