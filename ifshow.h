#ifndef IFSHOW_H
#define IFSHOW_H

#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>

/* 
 * get_prefix_length:
 *   Given a pointer to a sockaddr representing a netmask, 
 *   compute the prefix length (number of leading 1 bits).
 */
int get_prefix_length(struct sockaddr *netmask);

/*
 * show_all_interfaces:
 *   Retrieve the list of local network interfaces and, for each interface,
 *   print its name and associated IPv4/IPv6 addresses (with prefix) to the given stream.
 *
 *   Parameters:
 *     stream : an open FILE pointer (for example stdout or a socket stream via fdopen())
 *
 *   Returns 0 on success, -1 on error.
 */
int show_all_interfaces(FILE *stream);

/*
 * show_interface_by_name:
 *   Retrieve and print (to the given stream) the IPv4/IPv6 addresses and their prefixes
 *   for the network interface whose name is provided.
 *
 *   Parameters:
 *     ifname : the interface name (e.g., "eth0")
 *     stream : an open FILE pointer to print the results.
 *
 *   Returns 0 on success, -1 on error.
 */
int show_interface_by_name(const char *ifname, FILE *stream);

#endif /* IFSHOW_H */
