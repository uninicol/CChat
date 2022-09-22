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
    struct tls_config *config;


    //vengono fatte tutte le configurazioni su s_tls
    configure_tls(config, &s_tls);
    //tls_config_free(config);  //Segmentation fault non so perchè

    int server_socket = open_connection(port);
    int client_socket = establish_connection(server_socket);

    if (tls_accept_socket(s_tls, &c_tls, client_socket) != 0) {
        perror("server tls_accept_socket error\n");
        abort();
    }

    pid_t pid = fork();
    if (pid == 0)
        write_chat(c_tls);
    else
        read_chat(c_tls);

    close(server_socket);
    tls_close(s_tls);
    tls_free(s_tls);
    return 0;
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


    if (tls_config_set_key_file(config, "../Docs/mycert.pem") != 0) {
        perror("server tls_config_set_key_file error\n"); //TODO possiamo chiedere di generarlo automaticamente
        abort();
    }

    if (tls_config_set_cert_file(config, "../Docs/mycert.pem") != 0) {
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
    int sock;
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        abort();
    }
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY;

    //assegno l'indirizzo ip e la porta
    if (bind(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
        perror("errore bind");
        abort();
    }

    //ascolto un solo client
    if (listen(sock, 1) != 0) {
        perror("Can't configure listening port");
        abort();
    }
    return sock;
}

int establish_connection(int server_socket) {
    struct sockaddr_in server_addr;
    socklen_t len = sizeof(server_addr);

    listen(server_socket, 5);
    int client_socket = accept(server_socket, (struct sockaddr *) &server_addr, &len); //accetta la connessione
    printf("Connection: %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
    return client_socket;
}