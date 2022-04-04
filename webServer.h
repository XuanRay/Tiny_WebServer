/*   file    : webServer.h
 *   author  : rayss
 *   date    : 2022.11.03
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : webServer接口                   
 */


#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_


#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <string>

#include "task.h"

using namespace std;

#define MAX_EVENT_NUM 100  /* epoll内核 可传出的最大事件个数 */

class WebServer {
private:
    int port;                           /* 端口 */
    int sock_fd;                        /* server 管理连接的 file descriptor */
    int epoll_fd;                       /* epoll file descriptor */
    struct sockaddr_in server_addr;     /* 网络地址 ip + port */

public:
    WebServer( int p ) : sock_fd(0), port(p) { 
        memset( &server_addr, 0, sizeof( server_addr ) ); 
    }

    ~WebServer() { close( sock_fd ); }

    /* run method */
    int runs();
};


#endif /* _WEBSERVER_H_ */