/*   file    : locker.h
 *   author  : rayss
 *   date    : 2021.07.01
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : 封装互斥锁和条件变量                   
 */


#ifndef _LOCKER_H_
#define _LOCKER_H_

#include <iostream>
#include <exception>
#include <pthread.h>
using namespace std;

/* 线程锁 */
class MutexLocker {
private:
    pthread_mutex_t m_mutex;

public:
    MutexLocker() { 
        if( pthread_mutex_init( &m_mutex, NULL ) ) {  //success return 0
            cout << "mutex init error __ 1\n";
            throw exception();
        }
    }

    ~MutexLocker() {
        pthread_mutex_destroy( &m_mutex );
    }

    /* lock */
    bool mutex_lock() {
        return pthread_mutex_lock( &m_mutex ) == 0;  //加锁成功返回0
    }

    /* unlock */
    bool mutex_unlock() {
        return pthread_mutex_unlock( &m_mutex );
    }
};


/* 条件变量 */
class Cond {
private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
    
public:
    Cond() {
        if( pthread_mutex_init( &m_mutex, NULL ) ) {
            throw exception();
        }
        if( pthread_cond_init( &m_cond, NULL ) ) {
            pthread_cond_destroy( &m_cond );
            throw exception();
        }
    }

    ~Cond() {
        pthread_mutex_destroy( &m_mutex );
        pthread_cond_destroy( &m_cond );
    }

    // 等待条件变量，cond与mutex搭配使用，避免造成共享数据的混乱
    bool wait() {
        pthread_mutex_lock( &m_mutex );
        int ret = pthread_cond_wait( &m_cond, &m_mutex );
        pthread_mutex_unlock( &m_mutex );
        return ret == 0;
    }

    // 唤醒等待该条件变量的某个线程，如果有多个线程，选择优先级最高的
    bool signal() {
        return pthread_cond_signal( &m_cond ) == 0;
    }

    // 唤醒所有等待该条件变量的线程
    bool broadcast() {
        return pthread_cond_broadcast( &m_cond ) == 0;
    }
};

#endif  //_LOCKER_H_