#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "server.h"
#include "client.h"
#include "host-address.h"

int main(int argc, char *argv[]) {// definiamo come argomenti ip e porta (oppure lo facciamo automatico)
    printf("Host address:\t%s\n", hostAddress());
    //TODO sia client che server possono scrivere
    //TODO 1 arg= porta dell'host
    //TODO 2 arg= ip porta server

    if (argc < 2 || argc > 3)
        perror("numero parametri non corretto");
    if (argc == 2) {
        //server
        run_server(atoi(argv[1]));
    } else {
        //client
        run_client(argv[1], atoi(argv[2]));
    }
    return EXIT_SUCCESS;
}
