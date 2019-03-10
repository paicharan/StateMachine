#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include<iostream>
#include<mutex>
#include<queue>
#include<thread>
#include<condition_variable>

template <typename T>
class Job {
    public:
    typedef int (T::*JobFunction)(void);
    Job(T* clazz, JobFunction job): mJobHandlerClass(clazz), mJob(job)
    {
    }
    Job(const Job& other)
    {
        this->mJobHandlerClass = other.mJobHandlerClass;
        this->mJob = other.mJob;
    }
    Job& operator=(const Job& other)
    {
        this->mJobHandlerClass = other.mJobHandlerClass;
        this->mJob = other.mJob;
    }

    void run()
    {
        (mJobHandlerClass->*mJob)();
    }
    private:
    T* mJobHandlerClass;
    JobFunction mJob;
};

template <typename T>
class ThreadPool
{
    int mSize;
    bool mCanceled;
    std::vector<std::thread*> mThreadPool;
    std::queue<Job<T> > mJobQueue;
    std::condition_variable mCondition;
    std::mutex mMutex;
    void ThreadFunction()
    {
        while(!mCanceled)
        {
            {
                std::unique_lock<std::mutex> lock(mMutex);
                mCondition.wait(lock, [=] { return !mJobQueue.empty(); });
            }
            Job<T> job = mJobQueue.front();
            mJobQueue.pop();
            job.run();
        }
    }
    public :
        ThreadPool(int size) : mSize(size)
        {
            mCanceled = false;
            mThreadPool.resize(size);
            for(auto itr = mThreadPool.begin(); itr != mThreadPool.end(); itr++)
            {
                *itr = new std::thread(&ThreadPool::ThreadFunction, this);
            }
        }

        ThreadPool(const ThreadPool& other) = delete;
        ThreadPool& operator=(const ThreadPool& rhs) = delete;

        ~ThreadPool()
        {
            mCanceled = true;
            mCondition.notify_all();
            for(auto itr = mThreadPool.begin(); itr != mThreadPool.end(); itr++)
            {
                *itr->join();
            }
        }

        void addToPool(Job<T>& job)
        {
            mJobQueue.push(job);
            mCondition.notify_one();
        }

};

#endif
