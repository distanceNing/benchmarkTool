//
// Created by yangning on 18-1-1.
//
// Descriprion :
//
// Copyright (c) yangning All rights reserved.
//
#include "benckmark.h"

int main(int argc, char* argv[])
{
    if ( argc < 2 ) {
        Option::printDefautOption();
    }
    else
    {
        Option::process_parameters(argc, argv);
        gOption.printCurrentOption();
    }

    BenchMark benchMark;
    const char* hello="hello world";
    benchMark.setWriteCallBack([hello](int fd) {
      //send hello to server
      ::send(fd, hello, strlen(hello), MSG_NOSIGNAL);
    });

    benchMark.setReadCallBack([](net::SocketBuf* buf, int fd) {
      char recv_buf[MAX_BUF_SIZE] = {'\0'};
      size_t readable = buf->readableBytes();
      buf->read(recv_buf, readable);
      // As soon as any data is received, write it back
      ::send(fd, recv_buf, readable, MSG_NOSIGNAL);
    });

    benchMark.run();
    return 0;
}
