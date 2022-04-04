/*   file    : server.cpp
 *   author  : rayss
 *   date    : 2021.11.11
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
#include <string>
#include <unistd.h>

#include "webServer.h"


int main( int argc, char **argv ) {
    if( argc != 3 ) {
        std::cout << "Usage : ./server + port + workingDirector\n";
        return -1;
    }

    int port = atoi( argv[1] );
    
    int ret = chdir((const char *)argv[2]);

    if (ret == -1) {
        perror("change dir error");
        exit(1);
    }

    WebServer webServer( port );
    webServer.runs();

    return 0;
}