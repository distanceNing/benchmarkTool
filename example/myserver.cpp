//
// Created by yangning on 17-12-2.
//
// Descriprion :
//
// Copyright (c) yangning All rights reserved.
//

#include "benckmark.h"
#define   HEAD_SIZE   7

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

    //构造一个心跳包
    char buffer[MAX_BUF_SIZE] = {'\0'};
    const char* cmdType = "HBT";
    const char* text = "HBT";
    const int CMD_SIZE = 3;
    size_t text_len = 3;
    int net_len = ntohl((uint32_t) text_len);
    memcpy(buffer, cmdType, CMD_SIZE);
    memcpy(buffer + CMD_SIZE, &net_len, sizeof(int));
    memcpy(buffer + HEAD_SIZE, text, text_len);
    size_t pktSize = (size_t) (HEAD_SIZE + text_len);

    benchMark.setReadCallBack([buffer,pktSize](net::SocketBuf* buf, int fd) {
      char recv_buf[MAX_BUF_SIZE] = {'\0'};
      size_t readable = buf->readableBytes();
      buf->read(recv_buf, readable);
      printf("%s\n", recv_buf);
      ::send(fd, buffer, pktSize, MSG_NOSIGNAL);
      if(gOption.is_keep_alive_)
      {}
    });


    benchMark.setWriteCallBack([buffer, pktSize](int fd) {
      ::send(fd, buffer, pktSize, MSG_NOSIGNAL);
    });

    benchMark.run();
    return 0;
}


