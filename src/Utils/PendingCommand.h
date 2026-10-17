#pragma once

#include <atomic>

// Hands a command from one task to another without sharing any other state.
// The producer (e.g. a network callback) calls Set(), the consumer (the main
// loop) calls Take(). If several commands arrive before the consumer runs,
// only the latest one is kept.
template <typename T>
class PendingCommand
{
public:
    // Stores a new command, replacing any command not yet taken
    void Set(T value)
    {
        mValue.store(value);
        mPending.store(true);
    }

    // Returns true and the latest command if one is pending, false otherwise
    bool Take(T &value)
    {
        if (!mPending.exchange(false))
        {
            return false;
        }
        value = mValue.load();
        return true;
    }

private:
    std::atomic<T> mValue{};
    std::atomic<bool> mPending{false};
};
