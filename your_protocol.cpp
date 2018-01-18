//
// Created by yangning on 18-1-17.
//
// Descriprion :
//
// Copyright (c) yangning All rights reserved.
//

#include "your_protocol.h"

const char* hello="\"set yn 32 1023434 4\\r\\n yn\\r\\n\"";

void readCallBack(net::SocketBuf* buf, int fd)
{
    char recv_buf[MAX_BUF_SIZE] = {'\0'};
    size_t readable = buf->readableBytes();
    buf->read(recv_buf, readable);
    // As soon as any data is received, write it back
    ::send(fd, recv_buf, readable, MSG_NOSIGNAL);
}

void writeCallBack(int fd)
{

    ::send(fd, hello, strlen(hello), MSG_NOSIGNAL);
}
