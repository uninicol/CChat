//
// Created by nicol on 9/21/22.
//

#ifndef CMAKECCHAT_CHAT_H
#define CMAKECCHAT_CHAT_H

void start_chat(struct tls *c_tls);

void write_chat(struct tls *c_tls);

void read_chat(struct tls *c_tls);

#endif //CMAKECCHAT_CHAT_H
