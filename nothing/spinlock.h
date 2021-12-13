#include <atomic>
#include <thread>

/*
 * Probably not a good idea to use spinlocks in user mode.
 */
class spinlock {
    std::atomic_flag flag;

    spinlock() noexcept = default;

    void lock() noexcept
    {
        while (true) {
            for (int i = 0; i < 100000; i++) {
                if (flag.test_and_set())
                    return;
            }

            std::this_thread::yield();
        }
    }

    bool try_lock() noexcept { return !flag.test_and_set(); }
    void unlock() noexcept { flag.clear(); }
};
