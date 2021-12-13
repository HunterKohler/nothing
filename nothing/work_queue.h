#include <functional>
#include <list>
#include <queue>
#include <future>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <memory>

namespace nothing
{

class worker {
    using task_type = std::package_task<void(void)>;

    work_queue *_owner;
    std::thread _thread;
    std::packaged_task<void(void)> _task;

    worker(work_queue *owner) : _owner{ owner }, _thread(&worker::loop, this) {}
    ~worker() { _thread.join(); }

    void loop()
    {
        while (true) {
            std::unique_lock lock(_owner->_mutex);

            _owner->_work_cond.wait(lock, []() {
                return _owner->state != work_queue::state::working ||
                       _owner->_work.size();
            });
        }
    }
};

class work_queue {
    static size_type default_max_workers()
    {
        static size_type ret = std::min(std::thread::hardware_concurrency(), 4);
        return ret;
    }

    using size_type = std::size_t;
    using task_type = std::packaged_task<void(void)>;
    using duration_type = std::chrono::nanoseconds;

    enum class state {
        working,
        waiting,
        paused,
        paused_for,
        paused_until,
        stopping,
    };

    enum class launch_policy {
        start,
        paused,
        defered,
    };

    state _state;
    duration_type _linger;
    size_type _min_workers;
    size_type _max_workers;
    size_type _idle;
    std::mutex _mutex;
    std::condition_variable _work_cond;
    std::queue<task_type> _work;
    std::list<worker> _workers;

    work_queue(size_type max_workers) : work_queue(0, max_workers) {}
    work_queue(size_type min_workers = 0, size_type max_workers = 0,
               duration_type linger = 0,
               launch_policy policy = launch_policy::start)
        : _min_workers{ min_workers },
          _max_workers{ std::max(
              max_workers ? max_workers : default_max_workers(), min_workers) },
          _linger{ linger }
    {
        std::lock_guard lock(_mutex);

        switch (policy) {
        case start:
            _state = state::waiting;
            for (int i = 0; i < _min_workers; i++)
                _launch_worker();
            break;
        case defered:
            _state = state::waiting;
            break;
        case pause:
            _state = state::paused;
            break;
        }
    }

    ~work_queue()
    {
        std::unique_lock lock(_mutex);

        _state = state::stopping;
        _work_cond.notify_all();

        lock.unlock();
        for (worker &w: workers)
            w._thread.join();
    }

    bool pause_for();
    bool pause_until();
    bool pause();

    bool resume()
    {
        switch (_state) {
        case paused:
            /* ... */
            break;

        case paused_for:
        case paused_until:
            /* ... */
            break;

        case working:
        case waiting:
        case stopping:
            return false;
        }
    }

    size_type max_workers() const { return _max_workers; }
    size_type max_workers(size_type n);

    size_type min_workers() const { return _min_workers; }
    size_type min_workers(size_type n);

    duration_type linger() const { return _linger; }
    duration_type linger(duration_type t);

    state get_state() const { return _state; }

    size_type idle() const { return _idle; }
    size_type active() const { return _workers.size() - _idle; }
};

} // namespace nothing
