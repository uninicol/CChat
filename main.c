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
    //    pid_t pid = fork();
//    if (pid == 0)
//        run_server();
//    else
    run_client("192.168.1.223", "6001");
    return EXIT_SUCCESS;
}
