#include "server.h"
#include "chat.h"
#include <stdio.h>
#include <tls.h>
#include <openssl/aes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER 1024

void configure_tls(struct tls_config *config, struct tls **s_tls);

int open_connection(int port);

int establish_connection(int server_socket);

int run_server(int port) {
    printf("Sono il server porta: %d\n", port);
    struct tls *s_tls = tls_server();
    struct tls *c_tls = NULL;
    struct tls_config *config = NULL;

    //vengono fatte tutte le configurazioni su s_tls
    configure_tls(config, &s_tls);
    tls_config_free(config);

    int server_socket = open_connection(port);
    int client_socket = establish_connection(server_socket);

    if (tls_accept_socket(s_tls, &c_tls, client_socket) != 0) {
        perror("server tls_accept_socket error\n");
        abort();
    }

    printf("Connessione stabilita\n");
    start_chat(c_tls);

    close(server_socket);
    close(client_socket);
    tls_close(s_tls);
    tls_free(s_tls);
    return EXIT_SUCCESS;
}

void configure_tls(struct tls_config *config, struct tls **s_tls) {
    config = tls_config_new();
    if (config == NULL) {
        perror("tls_config_new error\n");
        abort();
    }

    uint32_t protocols = 0;
    if (tls_config_parse_protocols(&protocols, "secure") != 0) {
        perror("server tls_config_parse_protocols error\n");
        abort();
    }

    tls_config_set_protocols(config, protocols);

    char *ciphers = "AES256";
    if (tls_config_set_ciphers(config, ciphers) != 0) {
        perror("server tls_config_set_ciphers error\n");
        abort();
    }

    if (tls_config_set_key_file(config, "Docs/mycert.pem") != 0) {
        perror("server tls_config_set_key_file error\n"); //TODO possiamo chiedere di generarlo automaticamente
        abort();
    }

    if (tls_config_set_cert_file(config, "Docs/mycert.pem") != 0) {
        perror("server tls_config_set_cert_file error\n"); //TODO possiamo chiedere di generarlo automaticamente
        abort();
    }

    if (s_tls == NULL) {
        perror("server tls_server error\n");
        abort();
    }

    if (tls_configure(*s_tls, config) != 0) {
        printf("server tls_configure error: ");
        perror(tls_error(*s_tls));
        abort();
    }
}

int open_connection(int port) {
    struct sockaddr_in server_addr;
    int serversock;
    int opt = 1;

    //creo e verifico il socket
    serversock = socket(AF_INET, SOCK_STREAM, 0);
    if (serversock < 0) {
        perror("socket");
        abort();
    }
    //posso riutilizzare la stessa porta
    setsockopt(serversock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    bzero(&server_addr, sizeof(server_addr));

    //assegno ip e porta
    server_addr.sin_family = AF_INET;   //specifica la famiglia di protocolli da usare, in questo caso IPv4
    server_addr.sin_port = htons(port); //la porta viene memorizzata nell'ordine di rete
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //permette di far connettere al server ogni indirizzo, ordinato in ordine di rete


    //associo il socket all'ip
    if (bind(serversock, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
        perror("errore bind");
        abort();
    }

    //ascolto un solo client
    if (listen(serversock, 1) != 0) {
        perror("Can't configure listening port");
        abort();
    }

    return serversock;
}

int establish_connection(int server_socket) {
    struct sockaddr_in cli;
    socklen_t len = sizeof(cli);
    int connfd = accept(server_socket, (struct sockaddr *) &cli, &len);
    if (connfd < 0) {
        perror("server accept failed");
        abort();
    }
    return connfd;
}