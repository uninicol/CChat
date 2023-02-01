#include <stdio.h>
#include <stdlib.h>
#include <tls.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "Headers/chat.h"

#define BUFFER 1024 //dimensione del buffer

int open_connection_client(const char *hostname, int port);

int run_client(const char *hostname, int port) {
    int serversock;
    struct tls *c_tls = tls_client();
    struct tls_config *config = tls_config_new();

    tls_config_insecure_noverifycert(config);   //disabilitano parti di verifica del certificato
    tls_config_insecure_noverifyname(config);

    uint32_t protocols = 0;
    tls_config_parse_protocols(&protocols, "secure");
    tls_config_set_protocols(config, protocols);
    tls_config_set_ciphers(config, "secure");

    if (tls_configure(c_tls, config) != 0) {
        perror("tls configure");
        tls_error(c_tls);
        exit(EXIT_FAILURE);
    }
    tls_config_free(config);
    serversock = open_connection_client(hostname, port);

    if (tls_connect_socket(c_tls, serversock, hostname) != 0) {//crea un nuovo socket
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    printf("Connessione stabilita\n");
    start_chat(c_tls);

    close(serversock);
    tls_close(c_tls);
    tls_free(c_tls);
    return EXIT_SUCCESS;
}

int open_connection_client(const char *hostname, int port) {
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0); // imposto la connessione
    if (sock == -1) {
        perror("creazione socket fallita");
        abort();
    }
    bzero(&server_addr, sizeof(server_addr));

    //Assegno ip e porta del server dove connettersi
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(hostname);

    //apre la connessione
    if (connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
        close(sock);
        perror(hostname);
        abort();
    }
    return sock;
}