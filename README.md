
### benchmark:
    A simple tool to test tcp server concurrency .



#### example:
This example show how to test echoserver .

```
main.cpp:

#include "benckmark.h"

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        Option::printDefautOption();
    }
    else
        Option::process_parameters(argc,argv);

    BenchMark benchMark;

    /*  overwrite
     *  read call back call when the sock readable
     *  write call back call when the sock readable
     */
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

    //call benchmark run
    benchMark.run();
    return 0;
}
```


#### build：
using cmke：
```
cmake CMakeLists.txt

make
```

```
Defaut Option :
    host               = 127.0.0.1
    timeout_ms         = 3000
    server_port        = 9000
    concurrent_num     = 1024
    is_keep_alive      = true
    thread_num         = 1

```

```
Usage: benchmark
      [-h host]
      [-p source_port]
      [-c concurrent_num]
      [-t thread_num]
      [-a is_keep_alive]
      [-w timeout]
```
