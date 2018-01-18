//
// Created by yangning on 18-1-17.
//
// Descriprion :
//
// Copyright (c) yangning All rights reserved.
//

#ifndef BENCKMARK_YOUR_PROTOCOL_H
#define BENCKMARK_YOUR_PROTOCOL_H

#include <cstdio>
#include <sys/socket.h>
#include "common.h"
#include "socket/socket_buf.h"




void readCallBack(net::SocketBuf* buf, int fd);


void writeCallBack(int fd);

#endif //BENCKMARK_YOUR_PROTOCOL_H
