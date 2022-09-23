//
// Created by nicol on 9/21/22.
//

#include <strings.h>
#include <stdio.h>
#include <tls.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "chat.h"

#define BUFFER 1024

volatile bool closed = 0;

void start_chat(struct tls *c_tls) {
    pthread_t read_thread, write_thread;
    pthread_create(&read_thread, NULL, (void *(*)(void *)) read_chat, c_tls);
    pthread_create(&write_thread, NULL, (void *(*)(void *)) write_chat, c_tls);
    pthread_join(write_thread, NULL);
    pthread_join(read_thread, NULL);
}

void write_chat(struct tls *c_tls) {
    char buf[BUFFER] = {};
    while (buf[0] != ':' && buf[1] != 'q') {
        bzero(buf, strlen(buf));
        fgets(buf, BUFFER, stdin);
        if (closed)
            break;
        tls_write(c_tls, buf, strlen(buf));
    }
}

void read_chat(struct tls *c_tls) {
    char buf[BUFFER] = {};
    ssize_t bytes;
    while (!closed) {
        bzero(buf, strlen(buf));
        bytes = tls_read(c_tls, buf, sizeof(buf));
        if (bytes > 0) {
            if (buf[0] == ':' && buf[1] == 'q') {
                printf("Chat terminata");//non stampa al ricevente(?)
                tls_write(c_tls, ":q", 2);//chiudo il lettore dell'altro
                closed = 1;
                break;
            }
            printf("Server: %s", buf);
        }
    }
}