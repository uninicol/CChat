#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "server.h"
#include "client.h"
#include "host-address.h"

int main(int argc, char *argv[]) {// definiamo come argomenti ip e porta (oppure lo facciamo automatico)
    printf("\nHost address:\t%s\n", hostAddress());

    char *ip;
    int local_port;
    int port;

    printf("Inserire ip server:\t");
    fgets(ip, 15, stdin);
    printf("Inserire porta del server:\t");
    scanf("%d", &port);
    printf("Inserire porta del server locale:\t");
    scanf("%d", &local_port);


    pid_t pid = fork();
    if (pid == 0)
        run_server(local_port);
    else
        run_client(ip, port);
    return EXIT_SUCCESS;
}
