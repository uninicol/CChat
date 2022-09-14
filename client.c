#include <stdio.h>
#include <stdlib.h>
#include <tls.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "host-address.h"

#define PORT 4445

int socket_connect_server();

int run_client() {
    printf("Sono il client\n");
    if (tls_init() < 0) {
        perror("tls_init error\n");
        exit(EXIT_FAILURE);
    }
    struct tls *c_tls = tls_client();
    struct tls_config *config = tls_config_new();

    tls_config_insecure_noverifycert(config);   //disabilitano parti di verifica del certificato
    tls_config_insecure_noverifyname(config);

    uint32_t protocols = 0;
    tls_config_parse_protocols(&protocols, "secure");
//    tls_config_set_protocols(config, protocols);
//    tls_config_set_ciphers(config, "secure");
//    tls_config_set_key_file(config, "cert.pem");
//    tls_config_set_cert_file(config, "host.cert");
    if (tls_configure(c_tls, config) != 0) {
        perror("tls configure");
        tls_error(c_tls);
        exit(EXIT_FAILURE);
    }


    //fare socket per mandare c_tls e ricevere indirizzo
    int socket = socket_connect_server();

    char *server_ip;
    //scanf("%s", server_ip);
    if (tls_connect(c_tls, server_ip, atoi(PORT)) != 0) {//crea un nuovo socket
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

//    if (tls_connect_socket(c_tls, c_socket, "localhost") < 0) {
//        perror("tls_connect error\n");
//        printf("%s\n", tls_error(c_tls));
//        exit(EXIT_FAILURE);
//    }

    char *msg = "ciao server";
    tls_write(c_tls, msg, strlen(msg));

    //INPUT chat
//    char msg[255];
//    while (!strcmp(msg, "/EXIT")) {// la chat viene chiusa con il comando /EXIT
//        fgets(msg, sizeof(msg), stdin);
//        tls_write(c_tls, msg, sizeof(msg));
//    }

//    struct tls_config *config = tls_config_new();
//    struct c_tls *c_tls = c_tls(); //viene creata la connessione TLS chiamata context
//    tls_configure(c_tls, config);
//
//    int socket = tls_connect(c_tls, "host:4444", "4444");

    tls_close(c_tls);
    tls_free(c_tls);
    tls_config_free(config);
    return EXIT_SUCCESS;
}

int socket_connect_server() {
    //inet il formato dell'indirizzo è nome internet
    //stream trasmette dati come un flusso
    int c_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (c_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    setsockopt(c_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

    struct sockaddr_in servaddr, cli;
    memset(&servaddr, 0, sizeof(servaddr));

    //Assegnamento ip e porta
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);  //htons converte il valore per sin_port
    servaddr.sin_addr.s_addr = NULL;//TODO sostituire con indirizzo pubblico

    if (connect(c_socket, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
    return c_socket;
}