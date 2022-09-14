#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "server.h"
#include "client.h"
#include "host-address.h"

void printIP() {
    printf("IP pubblico:\t");
    FILE *ip = popen("curl -s https://api.ipify.org", "r");
    char c;
    c = fgetc(ip);
    while (c != EOF) {
        printf("%c", c);
        c = fgetc(ip);
    }
    fclose(ip);
    printf("\nHost address:\t%s\n", hostAddress());

}

int main(int argc, char *argv[]) {// definiamo come argomenti ip e porta (oppure lo facciamo automatico)
    printIP();
//    pid_t pid = fork();
//    if (pid == 0)
//        run_server();
//    else
//        run_client();
    run_server();
    return EXIT_SUCCESS;
}
