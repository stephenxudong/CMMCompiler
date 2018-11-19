//
//  Thread.cpp
//  CMM
//
//  Created by Stephen Liao on 2018/10/31.
//  Copyright © 2018 Stephen. All rights reserved.
//

#include "Thread.h"

void* Thread::run1()
{
    threadStatus = THREAD_STATUS_RUNNING;
    tid = pthread_self();
    run();
    threadStatus = THREAD_STATUS_EXIT;
    tid = 0;
    pthread_exit(NULL);
}

Thread::Thread()
{
    tid = 0;
    threadStatus = THREAD_STATUS_NEW;
}

bool Thread::start()
{
    //int iRet = 0;
    return pthread_create(&tid, NULL, thread_proxy_func, this) == 0;
}

pthread_t Thread::getThreadID()
{
    return tid;
}

int Thread::getState()
{
    return threadStatus;
}

void Thread::join()
{
    if (tid > pthread_t(0))
    {
        pthread_join(tid, NULL);
    }
}
void * Thread::thread_proxy_func(void * args)
{
    Thread * pThread = static_cast<Thread *>(args);
    
    pThread->run();
    
    return NULL;
}

void Thread::join(unsigned long millisTime)
{
    if (tid == 0)
    {
        return;
    }
    if (millisTime == 0)
    {
        join();
    }else
    {
        unsigned long k = 0;
        while (threadStatus != THREAD_STATUS_EXIT && k <= millisTime)
        {
            sleep(100);
            k++;
        }
    }
}
