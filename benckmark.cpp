#include <thread_db.h>
#include <signal.h>

#include "benckmark.h"
#include "thread/thread_pool.h"
#include "timerfd/timer_fd.h"
Option gOption;
using namespace std::placeholders;

bool g_is_loop = false;
void timerHandle(int sig)
{
    if(sig == SIGALRM )
    {
        g_is_loop = false;
    }
}

void addsig(int sig)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = timerHandle;
    sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);
    sigaction(sig, &sa, NULL);
}

BenchMark::BenchMark()
            :testResult_(new TestResult[gOption.thread_num_]){}

BenchMark::~BenchMark(){
    if(gOption.concurrent_num_ > 1)
        delete []testResult_;
    else
        delete(testResult_);
}

void BenchMark::run()
{
    g_is_loop = true;
    //设置测试时间
    addsig(SIGALRM);
    addsig(SIGTERM);
    alarm(gOption.duration_);
 
    ThreadPool threadPool(gOption.thread_num_ - 1);
   
    for (int i = 1; i < gOption.thread_num_; ++i) {
        threadPool.appendTask(std::bind(&BenchMark::benchmark, this, i));
    }
    threadPool.run();
    benchmark(0);
    //threadPool.join();
    //sleep(1);
    //统计测试结果

    /*
     *  QPS = req/sec = 请求数/秒
     *  QPS = 总请求数 / ( 进程总数 *   请求时间 )
     *  QPS: 单个进程每秒请求服务器的成功次数
     *
     */
    uint64_t total_request_num = 0;
    uint64_t total_response_num = 0;
    uint64_t total_time = 0;
    for (int i = 0; i < gOption.thread_num_; ++i) {
        total_request_num += testResult_[i].request_num_;
        total_response_num += testResult_[i].response_num_;
        total_time += testResult_[i].use_timer_;
    }

    for(int i = 0;i < gOption.thread_num_;++i)
    {
        std::cout << "this is thread " << i << " :\n";
        std::cout << "\tconnect  time : " << gOption.concurrent_num_ << " use : " << testResult_[i].connect_time_ << " us\n";
        std::cout << "\tuse      time : " << testResult_[i].use_timer_ << " \n";
        std::cout << "\trequest  num  : " << testResult_[i].request_num_ << " \n";
        std::cout << "\tresponse num  : " << testResult_[i].response_num_ << " \n";
    }

    std::cout<<"total result : \n";
    std::cout << "\ttotal request  num  is : " << total_request_num << "\n";
    std::cout << "\ttotal response num  is : " << total_response_num << "\n";
    std::cout << "\ttotal use     time  is : " << total_time << "\n";
    std::cout << "\tQPS                 is : " << (total_response_num*(1000 * 1000) /total_time)*gOption.thread_num_   << "\n";
}
void BenchMark::benchmark(int thread_id)
{

    Epoll epoll;
    TestResult* testResult = testResult_ + thread_id;
    uint64_t start_time = nowTime();
    //向主机发起连接
    for (int i = 0; i < gOption.concurrent_num_; ++i) {
        int conn_fd;
        if ((conn_fd = net::TcpSocket::create_and_bind()) < 0 )
            printErrorMsg("creat socket");
        if ( net::TcpSocket::noblockingConnect(conn_fd, gOption.server_ip_, gOption.server_port_,
                gOption.connect_timeout_ms_) < 0 ) {
            printErrorMsg("connect");
        }
        if ( !setFdNonBlocking(conn_fd))
            printErrorMsg("setFdNonBlocking");
        //printf("connect time is %d \n", i);
        net::TcpSocket::setTcpNoDelay(conn_fd);
        epoll.addNewFd(conn_fd);
        //usleep(1000);
    }
    uint64_t connect_end_time = nowTime();
    testResult->connect_time_ = connect_end_time - start_time;

    epoll.setReadcb(std::bind(&BenchMark::handleRead, this, testResult, _1, _2));

    epoll.setWritecb([testResult,this](int fd){
      testResult->request_num_++;
      this->writeCallBack_(fd);
    });
    while (g_is_loop)
        epoll.epollWait();
    uint64_t end_time = nowTime();

    testResult->use_timer_ = end_time - connect_end_time;

}

void Option::printDefautOption()
{

    printf("Defaut Option : \n");
    printf("\thost               = 127.0.0.1\n"
            "\ttimeout_ms         = 3000\n"
            "\tserver_port        = 9000\n"
            "\tconcurrent_num     = 1024\n"
            "\tis_keep_alive      = true\n"
            "\tduration           = 0\n"
            "\tthread_num         = 1\n");
    printf("Usage: %s \n"
            "\t  [-h host] \n"
            "\t  [-p source_port] \n"
            "\t  [-c concurrent_num] \n"
            "\t  [-t thread_num] \n"
            "\t  [-a is_keep_alive] \n"
            //测试持续时间
            "\t  [-d duration] \n"
            "\t  [-w timeout] \n", "benchmark");
}
void Option::printCurrentOption()
{
    printf("Current Option : \n");
    std::cout << "\thost               = " << server_ip_ << "\n" << "\tserver_port        = " << server_port_ << "\n"
              << "\ttimeout_ms         = " << connect_timeout_ms_ << "\n" << "\tconcurrent_num     = "
              << concurrent_num_ << "\n" << "\tis_keep_alive      = " << is_keep_alive_ << "\n"
              << "\tthread_num         = " << thread_num_ << "\n" << "\tduration           = " << duration_ << "\n"
              << "\tinterval_time_s    = " << interval_time_s_ << "\n";
}
int Option::process_parameters(int argc, char** argv)
{
    int c;
    while ((c = getopt(argc, argv, "h:p:t:c:aw:d:")) != -1) {
        switch (c) {
        case 'a':gOption.is_keep_alive_ = true;
            break;
        case 'h':gOption.server_ip_ = optarg;
            break;
        case 't':gOption.thread_num_ = (size_t) atoi(optarg);
            break;
        case 'p':gOption.server_port_ = (uint16_t) atoi(optarg);
            break;
        case 'c':gOption.concurrent_num_ = (uint16_t) atoi(optarg);
            break;
        case 'd':gOption.duration_ = (uint32_t) atoi(optarg);
            break;
        case 'w':gOption.connect_timeout_ms_ = (uint32_t) atoi(optarg);
            break;
        default:Option::printDefautOption();
            return 0;
        }
    }

    return 0;
}
