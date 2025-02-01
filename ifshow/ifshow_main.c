#include "ifshow.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage(const char *progname) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s -a\n", progname);
    fprintf(stderr, "  %s -i <interface>\n", progname);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
         usage(argv[0]);
         return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-a") == 0) {
         /* List all interfaces */
         return show_all_interfaces(stdout);
    } else if (strcmp(argv[1], "-i") == 0) {
         if (argc < 3) {
              usage(argv[0]);
              return EXIT_FAILURE;
         }
         return show_interface_by_name(argv[2], stdout);
    } else {
         usage(argv[0]);
         return EXIT_FAILURE;
    }
}
