#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "server.h"
#include "client.h"
#include "host-address.h"

int main(int argc, char *argv[]) {// definiamo come argomenti ip e porta (oppure lo facciamo automatico)
    printf("Host address:\t%s\n", hostAddress());//TODO da eliminare e sostituire controllando ip con ifconfig
    //TODO sia client che server possono scrivere
    //TODO 1 arg= porta host
    //TODO 2 arg= ip posta server

    if (argc < 2 || argc > 3)
        perror("numero parametri non corretto");
    if (argc == 2) {
        //server
        run_server(atoi(argv[1]));
    } else {
        //client
        run_client(argv[1], atoi(argv[2]));
    }

//    int local_port;
//    printf("Inserire porta del server locale:\t");
//    scanf("%d", &local_port);
//
//    pid_t pid = fork();
//    if (pid == 0)
//        run_server(local_port);
//    else {
//        char *ip = (char *) malloc(15);
//        int port;
//
//        printf("Inserire ip server host:\t");
//        scanf("%s", ip);
//        printf("Inserire porta del server host:\t");
//        scanf("%d", &port);
//        run_client(ip, port);
//        free(ip);
//    }
    return EXIT_SUCCESS;
}
