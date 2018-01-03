//
// Created by yangning on 17-12-30.
//
// Descriprion :
//
// Copyright (c) yangning All rights reserved.
//

#ifndef TTCP_BENCKMARK_H
#define TTCP_BENCKMARK_H
#include "socket/epoller.h"
#include "socket/tcp_socket.h"

#include <memory>
#include <cstdio>
extern struct Option gOption;
namespace  net{
class TimerFd;
}

struct Option {
  const char* server_ip_;
  uint16_t server_port_;
  bool is_keep_alive_;
  size_t thread_num_;
  size_t concurrent_num_;
  size_t connect_timeout_ms_;
  uint32_t interval_time_s_;
  uint32_t duration_;
  Option()
  {
      server_ip_ = "127.0.0.1";
      connect_timeout_ms_ = 3000;
      server_port_ = 9000;
      concurrent_num_ = 1024;
      is_keep_alive_ = false;
      thread_num_ = 1;
      interval_time_s_ = 0;
      duration_ = 0;
  }
  static int
  process_parameters (int argc, char **argv);
  void printCurrentOption();

  static void printDefautOption();
};

class BenchMark {
public:
    typedef struct {
      uint64_t connect_time_;
      uint64_t request_num_;
      uint64_t response_num_;
      uint64_t use_timer_;
    }TestResult;

    BenchMark()
            :isLoop_(false),testResult_(new TestResult[gOption.thread_num_])
    {
    }
    //当套接字可读时,用户制定的回调函数
    void setReadCallBack(const Epoll::ReadCallBack& cb)
    {
        readCallBack_ = cb;
    }
    //可写时
    void setWriteCallBack(const Epoll::WriteCallBack& cb)
    {
        writeCallBack_ = cb;
    }

    //调用run使得测试运行起来
    void run();

    virtual ~BenchMark(){
        if(gOption.concurrent_num_ > 1)
            delete []testResult_;
        else
            delete(testResult_);
    }
private:

    void handleRead(TestResult* testResult,net::SocketBuf* buf,int fd)
    {
        testResult->response_num_++;
        readCallBack_(buf,fd);
    }

    void handleWrite(TestResult* testResult,int fd)
    {
        testResult->request_num_++;
        writeCallBack_(fd);
    }

    void benchmark(int thread_id);


    TestResult* testResult_;


    void handleWrite(){}
    //std::unique_ptr<net::TimerFd> timer_;
    bool isLoop_;
    Epoll::ReadCallBack readCallBack_;
    Epoll::WriteCallBack writeCallBack_;
};

#endif //TTCP_BENCKMARK_H
