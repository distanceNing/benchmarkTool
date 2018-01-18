cmake_minimum_required(VERSION 3.7)
project(benckmark)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        common.h
        socket/epoller.cpp socket/epoller.h
        socket/tcp_socket.h
        socket/tcp_socket.cpp
        socket/socket_buf.cpp
        socket/socket_buf.h
        main.cpp

        thread/Thread.h thread/Thread.cpp
        thread/thread_pool.cpp thread/thread_pool.h
        thread/lock.h thread/lock.cpp
        timerfd/timer_fd.h timerfd/timer_fd.cpp
        benckmark.cpp benckmark.h
        your_protocol.cpp your_protocol.h)
link_libraries(pthread)
add_executable(benckmark ${SOURCE_FILES})