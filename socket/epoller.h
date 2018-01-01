//
// Created by yangning on 17-10-18.
//

#ifndef EPOLL_EPOLL_TEST_H
#define EPOLL_EPOLL_TEST_H
#include "../common.h"
#include "socket_buf.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <vector>
#include <iostream>
#include <map>
#include <functional>
#include <cstring>


#define MAYBE_TIME_OUT 1000

#define EPOLL_FLAGS 0


class Epoll{
public:
    using EpollEventList=std::vector<struct epoll_event>;
    using EventCallBack=std::function<void()>;
    using WriteCallBack=std::function<void(int fd)>;
    using ReadCallBack=std::function<void(net::SocketBuf*,int)>;

    static const int kInitSize= 1024;
    static const int kNonWriting=~EPOLLOUT;
    Epoll(size_t init_size=kInitSize):epollEventList_(init_size),epoll_fd_(epoll_create1(EPOLL_FLAGS)){
        if (epoll_fd_ < 0)
            printErrorMsg("epoll create");
    }

    void addNewFd(int fd);

    void removeFd(int fd);

    void epollWait();

    void setReadcb(const ReadCallBack& readcb);

    void setWritecb(const WriteCallBack& writecb)
    {
        writecb_ = writecb;
    }
    int getEpollFd()const
    {
        return epoll_fd_;
    }

private:
    void handleEvent(size_t ready_num);

    void handleRead(int fd);

    int epoll_fd_;
    ReadCallBack readcb_;
    WriteCallBack writecb_;
    net::SocketBuf socketBuf_;
    EpollEventList epollEventList_;
};

#endif //EPOLL_EPOLL_TEST_H
