#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "server.h"
#include "client.h"
#include "host-address.h"

int main(int argc, char *argv[]) {// definiamo come argomenti ip e porta (oppure lo facciamo automatico)
    printf("Host address:\t%s\n", hostAddress());
    int local_port;
    printf("Inserire porta del server locale:\t");
    scanf("%d", &local_port);

    pid_t pid = fork();
    if (pid == 0)
        run_server(local_port);
    else {
        char *ip;
        int port;

        printf("Inserire ip server:\t");
        scanf("%s", ip);
        printf("Inserire porta del server:\t");
        scanf("%d", &port);
        run_client(ip, port);
    }
    return EXIT_SUCCESS;
}
