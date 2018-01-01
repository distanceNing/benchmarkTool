//
// Created by yangning on 17-10-18.
//

#include "epoller.h"
#include "../common.h"

const char* hello= "hello world";
void Epoll::handleEvent(size_t ready_num)
{
    //处理文件描述符上发生的事件
    for (size_t i = 0; i < ready_num; ++i) {
        int fd=epollEventList_[i].data.fd;
        struct epoll_event event =epollEventList_[i];
        if ( event.events & EPOLLOUT )
        {
            //写入一些数据后,关闭对EPOLLOUT事件的关注
            if(writecb_)
                writecb_(fd);
            struct epoll_event ev;
            ev.data.fd = fd;
            ev.events = EPOLLIN;
            epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev);
        }
        else if(epollEventList_[i].events & EPOLLIN){
            handleRead(fd);
            //fd_map_.find(fd)->second();
        }
    }
}

void Epoll::handleRead(int fd)
{
    ssize_t read_size = socketBuf_.readFromFd(fd);
    if ( read_size > 0 )
        readcb_(&socketBuf_, fd);
    else if ( read_size == 0 )
    {
        removeFd(fd);
        close(fd);
        std::cout << "client fd " << fd << " close --\n";
    }
    else
        printErrorMsg("readFromFd");
}


void Epoll::addNewFd(int fd)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLOUT;
    int flag = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event);
    if (flag < 0)
        printErrorMsg("epoll_ctl");
}
void Epoll::epollWait()
{
    int ready_num = epoll_wait(epoll_fd_, epollEventList_.data(), static_cast<int>(epollEventList_.size()),
            MAYBE_TIME_OUT);
    if (ready_num < 0) {
        printErrorMsg("epoll_wait");
    }
    else if (ready_num == 0) {
        std::cout << "No Event Happened --- \n";
    }
    else
        handleEvent((size_t)ready_num);

    //当返回的准备好的事件和pollEventList的长度相同时,进行扩充.
    if (epollEventList_.size() == static_cast<unsigned long>(ready_num))
    {
        epollEventList_.resize(epollEventList_.size() * 2);
    }
}
void Epoll::removeFd(int fd)
{
    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, NULL);
    //fd_map_.erase(fd);
}
void Epoll::setReadcb(const Epoll::ReadCallBack& readcb)
{
    readcb_ = readcb;
}


