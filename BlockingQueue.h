#ifndef CPPWHAREHOUSE_BASE_BLOCKINGQUEUE_H
#define CPPWHAREHOUSE_BASE_BLOCKINGQUEUE_H

#include <condition_variable>
#include <mutex>

#include <deque>
#include <assert.h>

namespace CppWhareHouse
{

template<typename T>
class BlockingQueue
{
 public:
  BlockingQueue()
    : mutex_(),
      queue_()
  {
  }

  void put(const T& x)
  {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push_back(x);
    notEmpty_.notify_one(); // wait morphing saves us
    // http://www.domaigne.com/blog/computing/condvars-signal-with-mutex-locked-or-not/
  }

  void put(T&& x)
  {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push_back(std::move(x));
    notEmpty_.notify_one();
  }

  T take()
  {
    std::unique_lock<std::mutex> lock(mutex_);
    // always use a while-loop, due to spurious wakeup
    while (queue_.empty())
    {
      notEmpty_.wait(lock);
    }
    assert(!queue_.empty());
    T front(std::move(queue_.front()));
    queue_.pop_front();
    return std::move(front);
  }

  size_t size() const
  {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
  }

 private:
  mutable std::mutex mutex_;
  std::condition_variable         notEmpty_;
  std::deque<T>     queue_;
};

}  

#endif  // 
