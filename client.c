#include <stdio.h>
#include <stdlib.h>
#include <tls.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include "chat.h"

#define BUFFER 1024 //dimensione del buffer

int open_connection_client(const char *hostname, int port);

int run_client(const char *hostname, int port) {
    //printf("Sono il client\n");

    int server_socket;

    server_socket = open_connection_client(hostname, port);

    struct tls *c_tls = tls_client();
    struct tls_config *config = tls_config_new();

    tls_config_insecure_noverifycert(config);   //disabilitano parti di verifica del certificato
    tls_config_insecure_noverifyname(config);

    uint32_t protocols = 0;
    tls_config_parse_protocols(&protocols, "secure");
    tls_config_set_protocols(config, protocols);
    tls_config_set_ciphers(config, "secure");
//    tls_config_set_key_file(config, "../Docs/mycert.pem");
//    tls_config_set_cert_file(config, "../Docs/mycert.pem");

    if (tls_configure(c_tls, config) != 0) {
        perror("tls configure");
        tls_error(c_tls);
        exit(EXIT_FAILURE);
    }

    if (tls_connect_socket(c_tls, server_socket, hostname) != 0) {//crea un nuovo socket
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    //read_chat(c_tls);
    //write_chat(c_tls);
//    pid_t pid = fork();
//    if (pid == 0)
//        write_chat(c_tls);
//    else
//        read_chat(c_tls);
//    kill(0, SIGKILL);

    pthread_t read_thread, write_thread;
    printf("Before client Thread\n");
    pthread_create(&read_thread, NULL, (void *(*)(void *)) read_chat, c_tls);
    pthread_create(&write_thread, NULL, (void *(*)(void *)) write_chat, c_tls);
    pthread_join(read_thread, NULL);
    pthread_join(write_thread, NULL);
    printf("After client Thread\n");

    close(server_socket);
    tls_close(c_tls);
    tls_free(c_tls);
    tls_config_free(config);
    return EXIT_SUCCESS;
}

int open_connection_client(const char *hostname, int port) {
    int sock;
//    struct hostent *host;
    struct sockaddr_in server_addr;
//    if ((host = gethostbyname(hostname)) == NULL) {
//        perror(hostname);
//        abort();
//    }

    //PF_INET= inposto il formato dell'indirizzo (ipv4) SOCK_STREAM trasmette i dati come un flusso
    sock = socket(PF_INET, SOCK_STREAM, 0); // imposto la connessione
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;   //identifica il formato dell'indirizzo
    server_addr.sin_port = htons(port); //numero di porta
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//*(long *) (host->h_addr); //contiene l'indirizzoIPv4

    //apre la connessione
    if (connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
        close(sock);
        perror(hostname);
        abort();
    }
    return sock;
}