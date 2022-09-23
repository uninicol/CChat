//
// Created by nicol on 9/21/22.
//

#include <strings.h>
#include <stdio.h>
#include <tls.h>
#include <string.h>
#include "chat.h"

#define BUFFER 1024

void write_chat(struct tls *c_tls) {
    char buf[BUFFER] = {};
    while (buf[0] != ':' && buf[1] != 'q') {
        bzero(buf, strlen(buf));
        printf("MESSAGE TO SERVER:");
        fgets(buf, BUFFER, stdin);
        tls_write(c_tls, buf, strlen(buf)); // cifra e scrive il messaggio
    }
}

void read_chat(struct tls *c_tls) {
    char buf[BUFFER] = {};
    ssize_t bytes;
    while (1) {
        bzero(buf, strlen(buf));
        printf("leggo\n");
        bytes = tls_read(c_tls, buf, sizeof(buf)); /* get request and read message from server*/
        if (bytes > 0) {
            if (buf[0] == ':' && buf[1] == 'q') break; //riceve il comando di terminazione e esce dal ciclo
            //buf[bytes] = 0;
            printf("Server: %s", buf);
        }
    }
}