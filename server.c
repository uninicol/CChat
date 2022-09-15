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

#define BUFFER 1024

void configure_tls(struct tls_config *config, struct tls **s_tls);

int open_connection(const char *port);

void tls_connection(int server_socket, struct tls *s_tls, struct tls **c_tls);

int run_server(const char *port) {
    printf("Sono il server\n");
    struct tls *s_tls = tls_server();
    struct tls *c_tls = NULL;
    struct tls_config *config;

    //vengono fatte tutte le configurazioni su s_tls
    configure_tls(config, &s_tls);
    //tls_config_free(config);  //Segmentation fault non so perchè

    int server_socket = open_connection(port);

    tls_connection(server_socket, s_tls, &c_tls);

/*    char buf[1024];
    ssize_t sd, bytes = 0;
    char input[BUFFER];

    while (buf[0] != ':' && buf[1] != 'q') {
        bytes = tls_read(c_tls, buf, sizeof(buf)); *//* get request and read message from server*//*
        if (bytes > 0) {
            buf[bytes] = 0;
            printf("nMESSAGE FROM SERVER:%s\n", buf);
        }
    }*/

    char input[BUFFER];
    bzero(input, BUFFER);
    while (input[0] != ':' && input[1] != 'q') {
        printf("\nMESSAGE TO SERVER: ");
        fgets(input, BUFFER, stdin);
        tls_write(s_tls, input, strlen(input));
    }

//
//
//    char bufs[BUFFER], bufc[BUFFER];
//    struct pollfd pfd[2];

//    char *msg = "Ciao client sono il server";
//    tls_write(c_tls, msg, strlen(msg));
//    ssize_t byte = tls_read(c_tls, bufc, strlen(bufc));

//
//    ssize_t outlen = 0;
//    pfd[0].fd = 0;
//    pfd[0].events = POLLIN;
//    pfd[1].fd = server_socket;
//    pfd[1].events = POLLIN;
//
//    while (bufc[0] != ':' && bufc[1] != 'q') {
//
//        poll(pfd, 2, -1);
//
//        bzero(bufs, BUFFER);
//        bzero(bufc, BUFFER);
//
//        if (pfd[0].revents & POLLIN) {
//            int q = read(0, bufc, BUFFER);
//            tls_write(c_tls, bufc, q);
//        }
//
//        if (pfd[1].revents & POLLIN) {
//            if ((outlen = tls_read(c_tls, bufs, BUFFER)) <= 0) break;
//            printf("Mensagem (%lu): %s\n", outlen, bufs);
//        }
//    }


//    int bytes;
//    if (cpid == 0) {
//        while (1) {
//            bytes = tls_read(c_tls, bufc, sizeof(bufc)); /* get request and read message from server*/
//            if (bytes > 0) {
//                bufc[bytes] = 0;
//                printf("nMESSAGE FROM SERVER:%sn", buf);
//            } else
//                ERR_print_errors_fp(stderr);
//        }
//    } else {
//        while (1) {
//            printf("nMESSAGE TO CLIENT:");
//            fgets(input, BUFFER, stdin); /* get request and reply to client*/
//            SSL_write(ssl, input, strlen(input));
//        }
//    }
//}

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

int open_connection(const char *port) {
    struct sockaddr_in server;
    int sock;
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        abort();
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(port));
    server.sin_addr.s_addr = htonl(INADDR_ANY);    //inet_addr(hostAddress());

    if (bind(sock, (struct sockaddr *) &server, sizeof(server)) != 0) /* assiging the ip address and port*/
    {
        perror("can't bind port"); /* reporting error using errno.h library */
        abort(); /*if error will be there then abort the process */
    }

    if (listen(sock, 1) != 0) /*for listening to max of 1 clients in the queue*/
    {
        perror("Can't configure listening port"); /* reporting error using errno.h library */
        abort(); /*if erroor will be there then abort the process */
    }

    return sock;
}

void tls_connection(int server_socket, struct tls *s_tls, struct tls **c_tls) {
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr; /*socket for server*/
    socklen_t len = sizeof(client_addr);

    listen(server_socket, 1);
    int client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &len); /* accept connection as usual */
    printf("Connection: %s:%d\n", inet_ntoa(server_addr.sin_addr),
           ntohs(server_addr.sin_port)); /*printing connected client information*/

    if (tls_accept_socket(s_tls, c_tls, client_socket) != 0) {
        perror("server tls_accept_socket error\n");
        abort();
    }
}