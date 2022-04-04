/*   file    : task.cpp
 *   author  : rayss
 *   date    : 2022.08.10
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : 封装的任务，即解析客户请求（HTTP）的实现。                   
 */


#include <sstream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/wait.h>
#include <errno.h>
#include <iostream>

#include "task.h"

using namespace std;


/** removefd  remove fd from epollfd  close(fd)
 *  @param epollfd  epoll文件描述符
 *  @param fd       待移除的fd
 */ 
void removefd( int epollfd, int fd ) {
    epoll_ctl( epollfd, EPOLL_CTL_DEL, fd, 0 );
    close( fd );
}

/** reset_oneshot  reset fd
 *  @param epollfd  epoll文件描述符
 *  @param fd       待修改的socket fd
 */ 
void reset_oneshot( int epoll_fd, int fd ) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    epoll_ctl( epoll_fd, EPOLL_CTL_MOD, fd, &event ); /* EPOLL_CTL_MOD 修改已经注册的fd的监听事件 */
}


/* 执行任务 */
void Task::doit() {
    char buf[ BUFFER_SIZE ] = {0};
    int r;

    while( r = recv( accp_fd, buf, 1024, 0 ) ) {
        if( !r ) { /* 对端正确关闭 */
            cout << " browser exit.\n";
            break;
        } 
        
        // else if( r < 0 ) {  /* 如果接收出错则继续接收数据 */
        //     continue;
        // }

        else if (r < 0) {       //非阻塞方式，ET 循环接受立即按届
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                reset_oneshot(epoll_fd, accp_fd);
                printf("EAGAIN read later\n");
                break;
            }
        }

        int start = 0;
        char method[5], uri[100], version[10];
        sscanf( buf, "%s %s %s", method, uri, version );

        if( char *tmp = strstr( buf, "Range:" ) ) { /* Range: bytes=firstbytepos-lastbytepos */
            tmp += 13;
            sscanf( tmp, "%d", &start );
        }

        if( !strcmp( method, "GET" ) || !strcmp(method, "get") ) {  // 为GET
            deal_get( uri, start );
        } else if( !strcmp( method, "POST" ) || !strcmp(method, "post")) {  // 为POST
            deal_post( uri, buf );
        } else {
            const char *header = "HTTP/1.1 501 Not Implemented\r\nContent-Type: text/plain;charset=utf-8\r\n\r\n";
            send( accp_fd, header, strlen(header), 0 );
        }
        break;  /* 只要处理完就退出循环，避免浏览器一直处于pending状态 */ // undo
    }
    // close( accp_fd ); 
}


/** deal_get
 *  @param uri    get请求的uri
 *  @param start  Range字段的firstbytepos
 */ 
void Task::deal_get( const string & uri, int start ) {
    string filename = uri.substr(1);

    if( uri == "/" || uri == "/index.html" ) {

        send_file( "index.html", "text/html", start, 200, "OK" );

    } else if ( uri == "/submit.html" ) {

        send_file( "submit.html", "text/html", start, 200, "OK" );

    } else if( uri.find( ".jpg" ) != string::npos || uri.find( ".png" ) != string::npos ) {
        
        send_file( filename, "image/jpg", start , 200, "OK" );
    
    } else if( uri.find( ".html" ) != string::npos ) {
        
        send_file( filename, "text/html", start , 200, "OK" );
    
    } else if( uri.find( ".ico" ) != string::npos ) {
        
        send_file( filename, "image/x-icon", start , 200, "OK" );
    
    } else if( uri.find( ".js" ) != string::npos ) {
        
        send_file( filename, "yexy/javascript", start , 200, "OK" );
    
    } else if( uri.find( ".css" ) != string::npos ) {
        
        send_file( filename, "text/css", start , 200, "OK" );
    
    } else if( uri.find( ".mp3" ) != string::npos ) {
        
        send_file( filename, "audio/mp3", start , 200, "OK" );
    
    } else if( uri.find( ".mp4" ) != string::npos ) {
        
        send_file( filename, "audio/mp4", start , 200, "OK" );
    
    } else {
        
        send_file( filename, "text/plain", start , 200, "OK" );
    
    }
}


/** deal_post
 *  @param uri  请求首部的uri
 *  @param buf  post请求头部
 */ 
void Task::deal_post( const string & uri, char *buf ) {
    string filename = uri.substr(1);
    if( uri.find( "adder" ) != string::npos ) {  //使用CGI服务器，进行加法运算
        char *tmp = buf;
        int len, a, b;
        char *l = strstr( tmp, "Content-Length:" );  // 获取请求报文主体大小
        sscanf( l, "Content-Length: %d", &len );
        len = strlen( tmp ) - len;
        tmp += len;
        sscanf( tmp, "a=%d&b=%d", &a, &b );
        sprintf(tmp, "%d+%d,%d", a, b, accp_fd);  // tmp存储发送到CGI服务器的参数

        /* fork产生子进程，执行CGI服务器进行计算（webServer一眼只进行解析、发送数据，不进行相关计算）*/
        if( fork() == 0 ) {
            // dup2( accp_fd, STDOUT_FILENO );
            execl( filename.c_str(), tmp, NULL );
        }
        wait( NULL );  // 等待子进程结束
    } else {
        send_file( "html/404.html", "text/html", 0, 404, "Not Found" );
    }
}


/** send_file
 *  @param filename  请求首部的uri中的filename
 *  @param type      type对应response的Content-Type
 *  @param start     Range:
 *  @param num       状态码
 *  @param info      状态原语
 */ 
int Task::send_file( const string & filename, const char *type, 
                     int start, const int num, const char *info ) {
    struct stat filestat;
    int ret = stat( filename.c_str(), &filestat );
    if( ret < 0 || !S_ISREG( filestat.st_mode ) ) {  // 打开文件出错或没有该文件
        // cout << "file not found : " << filename << endl;
        send_file( "html/404.html", "text/html", 0, 404, "Not Found" );
        return -1;
    }

    char header[200];
    sprintf( header, "HTTP/1.1 %d %s\r\nServer: Rayss\r\nContent-Length: %d\r\nContent-Type:\
                     %s;charset:utf-8\r\n\r\n", num, info, int(filestat.st_size - start), type );   //undo

    // send第二个参数只能是c类型字符串，不能使用string
    send( accp_fd, header, strlen(header), 0 );

    int fd = open( filename.c_str(), O_RDONLY );
    int sum = start;

    while( sum < filestat.st_size ) {
        off_t t = sum;

        int r = sendfile( accp_fd, fd, &t, filestat.st_size );

        if( r < 0 ) {
            printf("errno = %d, r = %d\n", errno, r);
            // perror("sendfile : ");
            if( errno == EAGAIN ) {
                printf("errno is EAGAIN\n");
                reset_oneshot( epoll_fd, accp_fd );
                continue;
            } else {
                perror( "sendfile " );
                close( fd );
                break;
            }
        } else {
            sum += r;
        }
    }
    close( fd );
    // printf( "sendfile finish, %d\n", accp_fd );
    return 0;
}


/** get_size  获取文件的大小
 *  @param filename  请求首部的uri中的filename
 */ 
int Task::get_size( const string & filename ) {
    struct stat filestat;
    int ret = stat( filename.c_str(), &filestat );
    if( ret < 0 ) {
        cout << "file not found : " << filename << endl;
        return 0;
    }
    return filestat.st_size;
}
