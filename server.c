//
// Created by nicki on 30/06/2022.
//

#include "server.h"
#include <stdio.h>
#include <tls.h>
#include <openssl/aes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 4444

int run_server() {
    printf("Sono il server\n");
    struct tls *s_tls = NULL;
    struct tls *c_tls = NULL;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server, client;
    struct tls_config *config = tls_config_new();
    if (config == NULL) {
        perror("tls_config_new error\n");
        exit(EXIT_FAILURE);
    }

    uint32_t protocols = 0;
    if (tls_config_parse_protocols(&protocols, "secure") != 0) {
        perror("server tls_config_parse_protocols error\n");
        exit(EXIT_FAILURE);
    }

    tls_config_set_protocols(config, protocols);

    if (tls_config_set_ciphers(config, "secure") != 0) {
        perror("server tls_config_set_ciphers error\n");
        exit(EXIT_FAILURE);
    }

    //openssl genrsa -aes256 -out privateKey.key 2048
    if (tls_config_set_key_file(config, "privateKey.key") != 0) {
        perror("server tls_config_set_key_file error\n"); //TODO possiamo chiedere di generarlo automaticamente
        exit(EXIT_FAILURE);
    }

    //chmod 400 privateKey.key
    //openssl req -new -x509 -nodes -sha256 -days 365 -key privateKey.key -out server.crt
    if (tls_config_set_cert_file(config, "server.crt") != 0) {
        perror("server tls_config_set_cert_file error\n"); //TODO possiamo chiedere di generarlo automaticamente
        exit(EXIT_FAILURE);
    }

    s_tls = tls_server(); //viene creata la connessione TLS (context)
    if (s_tls == NULL) {
        perror("server tls_server error\n");
        exit(EXIT_FAILURE);
    }

    if (tls_configure(s_tls, config) != 0) {
        char *string = "server tls_configure error: %s\n\n";
        strncat(string, tls_error(s_tls), 8);
        perror(string);
        exit(EXIT_FAILURE);
    }

    bzero(&server, sizeof(server));
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT);
    server.sin_family = AF_INET;

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, 4);
    int b = bind(sock, (struct sockaddr *) &server, sizeof(server));
    if (b < 0) {
        perror("server bind error\n");
        exit(EXIT_FAILURE);
    }
    listen(sock, 1);

    socklen_t client_size = sizeof(client);
    int sc = accept(sock, (struct sockaddr *) &client, &client_size);

    if (tls_accept_socket(s_tls, &c_tls, sc) < 0) {
        perror("server tls_accept_socket error\n");
        exit(EXIT_FAILURE);
    }

    char bufs[255];
    tls_read(c_tls, bufs, sizeof bufs);
    printf("%s\n", bufs);

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
//        exit(EXIT_FAILURE);
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
//        exit(EXIT_FAILURE);
//    }
//    return c_socket;
//}
//
