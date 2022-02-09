/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_SPINLOCK_H_
#define NOTHING_SPINLOCK_H_

#include <atomic>
#include <thread>

namespace nothing {

class spinlock {
  public:
    spinlock() noexcept = default;

    void lock() noexcept
    {
        while (true) {
            for (int i = 0; i < 100000; i++) {
                if (_flag.test_and_set())
                    return;
            }

            std::this_thread::yield();
        }
    }

    bool try_lock() noexcept { return !_flag.test_and_set(); }
    void unlock() noexcept { _flag.clear(); }

  private:
    std::atomic_flag _flag;
};

} // namespace nothing

#endif
