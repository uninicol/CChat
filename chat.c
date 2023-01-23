//
// Created by nicol on 9/21/22.
//

#include <strings.h>
#include <stdio.h>
#include <tls.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "Headers/chat.h"

#define BUFFER 1024

volatile bool closed = false;

void start_chat(struct tls *cs_tls) {
    pthread_t read_thread, write_thread;
    pthread_create(&read_thread, NULL, (void *(*)(void *)) read_chat, cs_tls);
    pthread_create(&write_thread, NULL, (void *(*)(void *)) write_chat, cs_tls);
    pthread_join(write_thread, NULL);
    pthread_join(read_thread, NULL);
}

void write_chat(struct tls *cs_tls) {
    char buf[BUFFER] = {};
    while (buf[0] != ':' && buf[1] != 'q') {
        bzero(buf, strlen(buf));
        fgets(buf, BUFFER, stdin);
        if (closed)
            break;
        tls_write(cs_tls, buf, strlen(buf));
    }
}

void read_chat(struct tls *cs_tls) {
    char buf[BUFFER] = {};
    ssize_t bytes;
    while (!closed) {
        bzero(buf, strlen(buf));
        bytes = tls_read(cs_tls, buf, sizeof(buf));
        if (bytes == 1 && buf[0] == '\n') //evita di avere un messaggio vuoto
            continue;
        if (buf[0] == ':' && buf[1] == 'q') {
            printf("Chat terminata\n");
            tls_write(cs_tls, ":q", 2);//chiudo il lettore dell'altro
            closed = true;
            break;
        }
        printf("Server: %s", buf);
    }
}
