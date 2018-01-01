#include "benckmark.h"
#include "thread/thread_pool.h"

Option gOption;

void BenchMark::run()
{
   isLoop_= true;
    if ( gOption.thread_num_ == 1 ) {
        benchmark();
    }
    else {
        ThreadPool threadPool(std::bind(&BenchMark::benchmark,this), gOption.thread_num_);
        threadPool.run();
        threadPool.join();
    }
}
void BenchMark::benchmark()
{
    Epoll epoll;
    //if ( option_.interval_time_s_ != 0 )
    //{
    //    timer_ = new net::TimerFd();
    //    timer_->setTime(option_.interval_time_s_,option_.interval_time_s_);
    //}


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
        printf("connect time is %d \n", i);
        net::TcpSocket::setTcpNoDelay(conn_fd);
        epoll.addNewFd(conn_fd);
        usleep(1000);
    }

    epoll.setReadcb(readCallBack_);
    epoll.setWritecb(writeCallBack_);
    while (isLoop_)
        epoll.epollWait();
}

void Option::printDefautOption()
{

    printf("Defaut Option : \n");
    printf( "\thost               = 127.0.0.1\n"
            "\ttimeout_ms         = 3000\n"
            "\tserver_port        = 9000\n"
            "\tconcurrent_num     = 1024\n"
            "\tis_keep_alive      = true\n"
            "\tthread_num         = 1\n");
    printf("Usage: %s \n"
            "\t  [-h host] \n"
            "\t  [-p source_port] \n"
            "\t  [-c concurrent_num] \n"
            "\t  [-t thread_num] \n"
            "\t  [-a is_keep_alive] \n"
            "\t  [-w timeout] \n", "benchmark");
}
void Option::printCurrentOption()
{
    printf("Current Option : \n");
    std::cout<<"\thost               = "<<server_ip_<<"\n"
             <<"\tserver_port        = "<<server_port_<<"\n"
             <<"\ttimeout_ms         = "<<connect_timeout_ms_<<"\n"
             <<"\tconcurrent_num     = "<<concurrent_num_<<"\n"
             <<"\tis_keep_alive      = "<<is_keep_alive_<<"\n"
             <<"\tthread_num         = "<<thread_num_<<"\n"
             <<"\tinterval_time_s    = "<<interval_time_s_<<"\n";
}
int Option::process_parameters(int argc, char** argv)
{
    int c;
    while ((c = getopt(argc, argv, "h:p:t:c:a")) != -1) {
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
        default:Option::printDefautOption();
            return 0;
        }
    }

    return 0;
}
