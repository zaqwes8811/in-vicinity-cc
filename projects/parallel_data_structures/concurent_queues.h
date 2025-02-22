#ifndef AP_DS_H_
#define AP_DS_H_

#include <condition_variable>
#include <list>
#include <mutex>

namespace fix_extern_concurent {
// Существующие интерфейсы
// Boost - lookfree ds
// TBB - best but...
// С++11 - потокобезопасные контейнеры - таких там нет
//   http://stackoverflow.com/questions/15278343/c11-thread-safe-queue

// Intel TBB
template <typename T>
class concurent_queue_
//: public boost::noncopyable
{
public:
    concurent_queue_() {}

    void push(const T &source);
    bool try_pop(T &destination);

    bool empty() const;  // may not be precist cos pending
};

template <typename T>
class concurent_bounded_queue
//: public boost::noncopyable
{
public:
    concurent_bounded_queue() {}

    void push(const T &source);
    bool try_pop(T &destination);

    bool empty() const;
};

//
// TODO: очередь от Шена
// http://channel9.msdn.com/Events/GoingNative/2013/Cpp-Seasoning
//
// В вопросах кто-то не лестно отозвался о реализации, но что он сказал?
template <typename T>
class concurent_queue {
    std::mutex mutex_;
    std::list<T> q_;

public:
    void enqueue(T x) {
        // allocation here
        std::list<T> tmp;
        tmp.push_back(x);  // move(x));

        // threading
        {
            std::lock_guard<std::mutex> lock(mutex_);
            // вроде бы константное время
            q_.splice(end(q_), tmp);

            // для вектора может неожиданно потреб. реаллокация
        }
    }

    // ...
};

/** Pro
   http://stackoverflow.com/questions/20488854/implementation-of-a-bounded-blocking-queue-using-c

 https://www.threadingbuildingblocks.org/docs/help/reference/containers_overview/concurrent_bounded_queue_cls.htm
 Assume all includes are there

  http://www.boost.org/doc/libs/1_35_0/doc/html/thread/synchronization.html#thread.synchronization.condvar_ref.condition_variable_any
*/
struct BoundedBlockingQueueTerminateException : virtual std::exception
//, virtual boost::exception
{};

// http://stackoverflow.com/questions/5018783/c-pthread-blocking-queue-deadlock-i-think
/**
  \param T - assign operator, copy ctor

  \fixme exception safty!!
*/
template <typename T>
class concurent_bounded_try_queue
//: public boost::noncopyable
{
public:
    // types
    typedef T value_type;
    // typedef A allocator_type;
    typedef T &reference;
    typedef const T &const_reference;

    explicit concurent_bounded_try_queue(int size)
        : m_current_size(0),
          m_nblocked_pop(0),
          m_nblocked_push(0),
          m_stopped(false),
          cm_size(size),
          m_with_blocked(false) {
        if (cm_size < 1) {
            // BOOST_THROW_EXCEPTION
        }
    }

    ~concurent_bounded_try_queue() {
        // stop(true);
    }

    bool empty() {
        // http://stackoverflow.com/questions/22676871/boost-scoped-lock-replacement-in-c11
        std::unique_lock<std::mutex>
            // mutex::scoped_lock
            lock(m_mtx);
        return m_q.empty();
    }

    std::size_t size() {
        std::unique_lock<std::mutex> lock(m_mtx);
        return m_current_size;
        // return m_q.size();  // O(n) for list
    }

    bool try_push(const T &x) {
        std::list<T> tmp;
        tmp.push_back(x);

        {
            std::unique_lock<std::mutex> lock(m_mtx);
            // if (m_q.size() == cm_size)//size)
            // if (size() == cm_size)//size)  // self deadlock
            if (m_current_size == cm_size)  // size)
                return false;

            // m_q.push(x);  // bad
            // m_q.splice(boost::end(m_q), tmp);
            m_q.splice(m_q.end(), tmp);
            m_current_size++;
        }
        // if (m_with_blocked) m_pop_cv.notify_one();
        return true;
    }

    bool try_pop(T &popped) {
        {
            std::unique_lock<std::mutex> lock(m_mtx);
            if (m_q.empty()) return false;

            // FIXME: is
            popped = m_q.front();
            m_q.pop_front();  // pop()
            m_current_size--;
        }
        // if (m_with_blocked) m_push_cv.notify_one();
        return true;
    }

private:
    void stop(bool wait) {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_stopped = true;
        m_pop_cv.notify_all();
        m_push_cv.notify_all();

        if (wait) {
            while (m_nblocked_pop) m_pop_cv.wait(lock);
            while (m_nblocked_push) m_push_cv.wait(lock);
        }
    }

    /**
      \brief blocked call
    */
    void wait_and_pop(T &popped) {
        std::unique_lock<std::mutex> lock(m_mtx);

        ++m_nblocked_pop;

        while (!m_stopped && m_q.empty()) m_pop_cv.wait(lock);

        --m_nblocked_pop;

        if (m_stopped) {
            m_pop_cv.notify_all();
            // BOOST_THROW_EXCEPTION(
            throw BoundedBlockingQueueTerminateException();  //);
        }

        popped = m_q.front();
        m_q.pop_front();  // pop();

        m_push_cv.notify_one();  // strange
    }

    void wait_and_push(const T &item) {
        {
            std::unique_lock<std::mutex> lock(m_mtx);

            ++m_nblocked_push;
            while (!m_stopped && m_q.size() == size()) m_push_cv.wait(lock);
            --m_nblocked_push;

            if (m_stopped) {
                m_push_cv.notify_all();
                // BOOST_THROW_EXCEPTION(
                throw BoundedBlockingQueueTerminateException();  //);
            }

            m_q.push(item);
        }
        m_pop_cv.notify_one();
    }

private:
    int m_current_size;  // for std::list
    std::
        // queue  // FIXME: back to deque
        list<T>
            m_q;  // FIXME: to list
    std::mutex m_mtx;
    std::condition_variable_any m_pop_cv;   // q.empty() condition
    std::condition_variable_any m_push_cv;  // q.size() == size condition
    int m_nblocked_pop;
    int m_nblocked_push;
    bool m_stopped;
    const int cm_size;
    const bool m_with_blocked;
};
}  // namespace fix_extern_concurent
#endif
