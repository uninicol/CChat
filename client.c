#include <stdio.h>
#include <stdlib.h>
#include <tls.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "host-address.h"

#define BUFFER 1024 /*buffer for reading messages*/

int open_connection_client(const char *hostname, const char *port);

int run_client(const char *hostname, const char *port) {
    printf("Sono il client\n");

    int sock;
    char buf[BUFFER];
    char input[BUFFER];
    int bytes;
    //char *hostname, *portnum;

    pid_t cpid; /* fork variable*/

//    hostname = strings[1];
//
//    portnum = strings[2];

    sock = open_connection_client(hostname, port); /*converting ascii port to interger */

    struct tls *c_tls = tls_client();
    struct tls_config *config = tls_config_new();

    //tls_config_insecure_noverifycert(config);   //disabilitano parti di verifica del certificato
    //tls_config_insecure_noverifyname(config);

    uint32_t protocols = 0;
    tls_config_parse_protocols(&protocols, "secure");
    tls_config_set_protocols(config, protocols);
    tls_config_set_ciphers(config, "secure");
    tls_config_set_key_file(config, "../Docs/mycert.pem");
    tls_config_set_cert_file(config, "../Docs/mycert.pem");
    if (tls_configure(c_tls, config) != 0) {
        perror("tls configure");
        tls_error(c_tls);
        exit(EXIT_FAILURE);
    }

    if (tls_connect_socket(c_tls, sock, hostname) != 0) {//crea un nuovo socket
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    char *msg = "ciao server";
    tls_write(c_tls, msg, strlen(msg));


    //INPUT chat
//    char msg[255];
//    while (!strcmp(msg, "/EXIT")) {// la chat viene chiusa con il comando /EXIT
//        fgets(msg, sizeof(msg), stdin);
//        tls_write(c_tls, msg, sizeof(msg));
//    }

    close(sock);
    tls_close(c_tls);
    tls_free(c_tls);
    tls_config_free(config);
    return EXIT_SUCCESS;
}

int open_connection_client(const char *hostname, const char *port) {
    int sock;
    struct hostent *host;
    struct sockaddr_in addr; /*creating the sockets*/
    if ((host = gethostbyname(hostname)) == NULL) {
        perror(hostname);
        abort();
    }

    sock = socket(AF_INET, SOCK_STREAM, 0); /* setting the connection as tcp it creates endpoint for connection */
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));
    addr.sin_addr.s_addr = *(long *) (host->h_addr);

    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) != 0) {/*initiate a connection on a socket*/
        close(sock);
        perror(hostname);
        abort();
    }
}