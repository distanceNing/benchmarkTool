
### benchmark:
    A simple tool to test tcp server concurrency .



#### example:
This example show how to test echoserver .

```cpp
your_protocol.cpp:

#include "your_protocol.h"
// this callback when the socket is readabale.
void readCallBack(net::SocketBuf* buf, int fd)
{
    char recv_buf[MAX_BUF_SIZE] = {'\0'};
    size_t readable = buf->readableBytes();
    buf->read(recv_buf, readable);
    // As soon as any data is received, write it back
    ::send(fd, recv_buf, readable, MSG_NOSIGNAL);
}
//when the socket is writeable

void writeCallBack(int fd)
{
    const char* hello="hello world";
    ::send(fd, hello, strlen(hello), MSG_NOSIGNAL);
}

```


#### build：
using cmke to build this project：
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
