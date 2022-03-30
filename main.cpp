/*   file    : main.cpp
 *   author  : rayss
 *   date    : 2022.01.01
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : 主函数                   
 */

/** 
 *  采用 epoll+线程池 实现，含有GET、POST， 可以发送html、picture、MP3、js、css、ico
 *  服务器可以稳定运行
 */

#include <iostream>

#include "webServer.h"


int main( int argc, char **argv ) {
    if( argc != 2 ) {
        std::cout << "Usage : ./server + port\n";
        return -1;
    }

    int port = atoi( argv[1] );
    WebServer webServer( port );
    webServer.runs();

    return 0;
}