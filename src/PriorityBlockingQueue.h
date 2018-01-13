#ifndef PRIORITYBLOCKINGQUEUE_H
#define PRIORITYBLOCKINGQUEUE_H

#include <queue>
#include <muduo/base/Condition.h>
#include <muduo/base/Mutex.h>

namespace RTMP{

using muduo::MutexLock;
using muduo::MutexLockGuard;
using muduo::Condition;

template <typename T,typename Comepare>
class PriorityBlockingQueue
{
public:
    PriorityBlockingQueue(int maxsz = -1) 
        : _maxSize(maxsz), 
          _mutex(),
          _notEmpty(_mutex),
          _notFull(_mutex) {}
    
    void push(const T& x)
    {
        MutexLockGuard lock(_mutex);
        while (_queue.size() == _maxSize)
        {
            _notFull.wait();
        }
        assert(!(_queue.size() == _maxSize));
        _queue.push(x);
        _notEmpty.notify();
    }

    const T &top()
    {
        MutexLockGuard lock(_mutex);
        while (_queue.empty())
        {
            _notEmpty.wait();
        }
        assert(!_queue.empty());
        return _queue.top();
    }

    void pop()
    {
        MutexLockGuard lock(_mutex);
        while (_queue.empty())
        {
            _notEmpty.wait();
        }
        assert(!_queue.empty());
        _queue.pop();
        _notFull.notify();
    }


    bool empty() const
    {
        MutexLockGuard lock(_mutex);
        return _queue.empty();
    }

    bool full() const
    {
        MutexLockGuard lock(_mutex);
        return _queue.size() == _maxSize;
    }

    size_t size() const
    {
        MutexLockGuard lock(_mutex);
        return _queue.size();
    }

    size_t capacity() const
    {
        MutexLockGuard lock(_mutex);
        return _queue._maxSize;
    }

    bool unlimited()
    {
        return capacity() == -1;
    }
private:
    mutable MutexLock                           _mutex;
    Condition                                   _notEmpty;
    Condition                                   _notFull;
    std::priority_queue<T,std::vector<T>,Comepare>   _queue;
    int _maxSize;
};






}
#endif

