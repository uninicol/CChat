#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "server.h"
#include "client.h"

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3)
        perror("numero parametri non corretto");
    else if (argc == 2) {
        run_server(atoi(argv[1]));
    } else {
        run_client(argv[1], atoi(argv[2]));
    }
    return EXIT_SUCCESS;
}
