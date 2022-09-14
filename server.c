//
// Created by nicki on 30/06/2022.
//

#include "server.h"
#include "host-address.h"
#include <stdio.h>
#include <tls.h>
#include <openssl/aes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>

#define PORT 4446

void configure_tls(struct tls_config *config, struct tls *s_tls);

void open_connection(struct sockaddr_in *server, int sock);

int run_server() {
    printf("Sono il server\n");
    struct tls *s_tls;
    struct tls *c_tls;
    struct tls_config *config;
    int sock = 0;
    struct sockaddr_in server, client;

    //vengono fatte tutte le configurazioni su s_tls
    configure_tls(config, s_tls);

    open_connection(&server, &sock);

    socklen_t client_size = sizeof(client);
    int sc = accept(sock, (struct sockaddr *) &client, &client_size);

    if (tls_accept_socket(s_tls, &c_tls, sc) < 0) {
        perror("server tls_accept_socket error\n");
        abort();
    }

    char *msg = "Ciao client";
    tls_write(c_tls, msg, strlen(msg));

    struct pollfd pfd[2];
    pfd[0].fd = 0;
    pfd[0].events = POLLIN;
    pfd[1].fd = sc;
    pfd[1].events = POLLIN;

    char bufs[255];
    ssize_t outlen = 0;
    while (bufs[0] != ':' && bufs[1] != 'q') {
        poll(pfd, 2, -1);
        bzero(bufs, 1000);
        bzero(bufs, 1000);
        if (pfd[0].revents & POLLIN) {
            int q = read(0, bufs, 1000);
            tls_write(c_tls, bufs, q);
        }
        if (pfd[1].revents & POLLIN) {
            if ((outlen = tls_read(c_tls, bufs, 1000)) <= 0) break;
            printf("Mensagem (%lu): %s\n", outlen, bufs);
        }
    }


//    char bufs[255];
//    tls_read(c_tls, bufs, sizeof bufs);
//    printf("%s\n", bufs);

    //if(tls_accept_socket(s_tls, )<0)

    //EVP_CIPHER_CTX *cipherCtx = EVP_CIPHER_CTX_new();
    //AES_encrypt("fds", "dio", AES_KEY *key);

    tls_close(s_tls);
    tls_free(s_tls);
    tls_config_free(config);
    return 0;
}

//int socket_connect_server() {
//    //inet il formato dell'indirizzo è nome internet
//    //stream trasmette dati come un flusso
//    int c_socket = socket(AF_INET, SOCK_STREAM, 0);
//    if (c_socket != 0) {
//        perror("socket");
//        abort();
//    }
//
//    struct sockaddr_in servaddr, cli;
//    bzero(&servaddr, sizeof(servaddr));
//
//    //Assegnamento ip e porta
//    servaddr.sin_family = AF_INET;
//    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");//TODO sostituire con indirizzo pubblico
//    servaddr.sin_port = htons(PORT);  //htons converte il valore per sin_port
//
//    if (connect(c_socket, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
//        perror("socket error\n");
//        abort();
//    }
//    return c_socket;
//}
//

void configure_tls(struct tls_config *config, struct tls *s_tls) {
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

    char *ciphers = "ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384";
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

    s_tls = tls_server(); //viene creata la connessione TLS (context)
    if (s_tls == NULL) {
        perror("server tls_server error\n");
        abort();
    }

    if (tls_configure(s_tls, config) != 0) {
        printf("server tls_configure error: ");
        perror(tls_error(s_tls));
        abort();
    }
}

void open_connection(struct sockaddr_in *server,
                     int sock) {//TODO posso dichiarare sock all'interno se non serve successivamente
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        abort();
    }

    bzero(&server, sizeof(server));
    server->sin_family = AF_INET;
    server->sin_port = htons(PORT);
    server->sin_addr.s_addr = htonl(INADDR_ANY);    //inet_addr(hostAddress());

    if (connect(sock, (struct sockaddr *) &server, sizeof(server)) != 0) {
        close(sock);
        perror("socket error\n");
        abort();
    }
    //    if (bind(sock, (struct sockaddr *) &server, sizeof(server)) != 0) {
//        perror("server bind error");
//        abort();
//    }
//    if (listen(sock, 1) != 0) {
//        perror("server listen error");
//        abort();
//    }
}