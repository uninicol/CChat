#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Headers/server.h"
#include "Headers/client.h"

void get_arguments();

void ask_client();

void ask_server();

int main(int argc, char *argv[]) {
    if (argc == 1) {
        get_arguments();
    } else if (argc == 2) {
        run_server(atoi(argv[1]));
    } else if (argc == 3) {
        run_client(argv[1], atoi(argv[2]));
    } else {
        printf("Numero parametri non corretto");
        get_arguments();
    }
    return EXIT_SUCCESS;
}

void get_arguments() {
    char buf;
    printf("Come far partire il programma?\n1-client\n2-server\n3-esci\n");
    scanf("%c", &buf);
    switch (atoi(&buf)) {
        case 1:
            ask_client();
            break;
        case 2:
            ask_server();
            break;
        default:
            break;
    }
}

void ask_client() {
    char *ip = NULL;
    printf("Inserisci ip del server:\t");
    scanf("%s", ip);
    int port;
    printf("Inserisci porta del server:\t");
    scanf("%i", &port);
    run_client(ip, port);
}

void ask_server() {
    int port;
    printf("Inserisci porta del server:\t");
    scanf("%i", &port);
    run_server(port);
}
