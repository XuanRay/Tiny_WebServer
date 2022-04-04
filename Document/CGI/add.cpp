#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main( int argc, char **argv ) {
    int a, b, accp_fd;
    sscanf( argv[0], "%d+%d,%d", &a, &b, &accp_fd );

    int t = a+b;
    string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=utf-8\r\n\r\n";
    string body = "<html><head><title>rayss's CGI</title></head>";
    body += "<body><p>The result is " + to_string(a) + "+" + to_string(b) + " = " + to_string(t);
    body += "</p></body></html>";
    response += body;
    //dup2(accp_fd, STDOUT_FILENO); /* dup2 复制文件描述符 STDOUT_FILENO 向屏幕输出 */
    // cout << response.c_str();
    send( accp_fd, response.c_str(), response.size(), 0 );
    return 0;
}