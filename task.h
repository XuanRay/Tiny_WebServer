/*   file    : task.h
 *   author  : rayss
 *   date    : 2022.08.10
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : 封装的任务，即解析客户请求（HTTP）                   
 */

#ifndef _TASK_H_
#define _TASK_H_

#include <string>
using namespace std;


#define BUFFER_SIZE 1024    //管理的socket个数 最大值

void removefd( int epollfd, int fd );

/* 封装任务执行的类 */
class Task {
private:
    int accp_fd;             /* 存储accept的返回值，即已连接的描述符 */
    int epoll_fd;            /* epoll文件描述符 */

public:
    Task() {}

    Task(int fd, int epoll) : accp_fd(fd), epoll_fd(epoll) {}

    ~Task() { removefd( epoll_fd, accp_fd ); }

    /* 执行任务 */
    void doit();

    /* 发送文件 */
    int send_file( const string & filename, const char *type, 
                   int start, const int num = 200, const char *info = "OK" );

    /* 处理get请求 */
    void deal_get( const string & uri, int start = 0 );
    
    /* 处理post请求 */
    void deal_post( const string & uri, char *buf );
    
    /* 获取文件大小 */
    int get_size( const string & filename );
};

#endif
