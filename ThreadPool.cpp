#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

template <typename T>
class Job {
    T* mJobHandlerClass;
    JobFunction mJob;
    public:
    typedef void (T::*JobFunction)(void);
    Job(T* clazz, JobHandler job): mJobHandlerClass(clazz), mJob(job)
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
        (mJobHandlerClass->*mJob)()
    }
}

class ThreadPool
{
    int mSize;
    bool mCanceled;
    std::vector<std::thread*> mThreadPool;
    std::queue<Job> mjobQueue;
    std::condition_variable mCondition;
    std::mutex mMutex;
    void ThreadFunction()
    {
        while(!mCanceled)
        {
            std::unique_lock<std::mutex> lock(mMutex);
            mCondition.wait(lock);
            Job job = mJobQueue.pop();
            job.run();
        }
    }
    public :
        ThreadPool(int size) : mSize(size)
        {
            mCanceled = false;
            mThreadPool->resize(size);
            for(auto itr = mThreadPool->being(), itr != mThreadPool->end(); itr++)
            {
                *itr = new std::thread(ThreadPool::ThreadFunction, this);
            }
        }

        ThreadPool(const ThreadPool& other) = delete;
        ThreadPool& operator=(const ThreadPool& rhs) = delete;

        ~ThreadPool()
        {
            for(auto itr = mThreadPool->being(), itr != mThreadPool->end(); itr++)
            {
                *itr->join();
            }
        }


};

#endif
