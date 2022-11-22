#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "server.h"
#include "client.h"
#include "host-address.h"

int main(int argc, char *argv[]) {
    printf("Host address:\t%s\n", hostAddress());
    if (argc < 2 || argc > 3)
        perror("numero parametri non corretto");
    if (argc == 2) {
        run_server(atoi(argv[1]));
    } else {
        run_client(argv[1], atoi(argv[2]));
    }
    return EXIT_SUCCESS;
}
