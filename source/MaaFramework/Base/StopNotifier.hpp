#pragma once

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <mutex>

#include "Conf/Conf.h"
#include "Utils/Logger.h"
#include "Utils/NonCopyable.hpp"

MAA_NS_BEGIN

class StopNotifier : public NonCopyable
{
public:
    StopNotifier();
    ~StopNotifier();

    void set_is_running(std::function<bool()> is_running) { is_running_ = is_running; }

    void set_on_stop(std::function<void()> on_stop) { on_stop_ = on_stop; }

    bool sleep(unsigned ms);

    void stop();
    bool idle();

    void chain(StopNotifier& notifier);
    void unchain(StopNotifier& notifier);

    bool needs_stop() const { return needs_stop_; }

private:
    bool needs_stop_ = false;
    std::mutex notifier_mtx_;
    std::function<bool()> is_running_;
    std::function<void()> on_stop_;

    std::mutex mtx_;
    std::condition_variable cond_;
    std::vector<StopNotifier*> chain_notifiers_;
    std::vector<StopNotifier*> parent_notifiers_;
};

inline StopNotifier::StopNotifier()
{
}

inline StopNotifier::~StopNotifier()
{
    std::unique_lock<std::mutex> lock(notifier_mtx_);

    for (auto parent : parent_notifiers_) {
        parent->unchain(*this);
    }
}

inline bool StopNotifier::sleep(unsigned ms)
{
    if (needs_stop_) {
        return false;
    }

    auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(ms);

    std::unique_lock<std::mutex> cond_lock(mtx_);

    if (cond_.wait_until(cond_lock, timeout, [this]() {
            return this->needs_stop_; //
        })) {
        return false;
    }
    return true;
}

inline void StopNotifier::stop()
{
    std::unique_lock<std::mutex> cond_lock(mtx_);

    needs_stop_ = true;

    on_stop_();

    cond_.notify_all();

    std::unique_lock<std::mutex> lock(notifier_mtx_);
    for (auto sub : chain_notifiers_) {
        sub->stop();
    }
}

inline bool StopNotifier::idle()
{
    if (!needs_stop_) {
        return true;
    }

    if (is_running_()) {
        return false;
    }

    std::unique_lock<std::mutex> lock(notifier_mtx_);
    for (auto sub : chain_notifiers_) {
        if (!sub->idle()) {
            return false;
        }
    }

    needs_stop_ = false;

    return true;
}

inline void StopNotifier::chain(StopNotifier& notifier)
{
    if (needs_stop_) {
        LogWarn << "chain notifier during stopping, ignore.";
        return;
    }

    {
        std::unique_lock<std::mutex> lock(notifier_mtx_);

        chain_notifiers_.push_back(&notifier);
    }

    {
        std::unique_lock<std::mutex> child_lock(notifier.notifier_mtx_);

        notifier.parent_notifiers_.push_back(this);
    }
}

inline void StopNotifier::unchain(StopNotifier& notifier)
{
    if (needs_stop_) {
        LogWarn << "unchain notifier during stopping, ignore.";
        return;
    }

    std::unique_lock<std::mutex> lock(notifier_mtx_);

    auto it = std::find(chain_notifiers_.begin(), chain_notifiers_.end(), &notifier);
    if (it != chain_notifiers_.end()) {
        chain_notifiers_.erase(it);
    }
}

MAA_NS_END
