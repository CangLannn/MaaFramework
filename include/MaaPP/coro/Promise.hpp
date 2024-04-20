#pragma once

#include <coroutine>
#include <functional>
#include <optional>
#include <utility>
#include <vector>

namespace maa::coro
{

template <typename T>
struct __promise_traits
{
    using then_t = void(const T&);

    using result_t = T;

    template <typename F>
    using then_ret_t = std::invoke_result_t<F, const T&>;

    template <typename R>
    using then_holder_t = R(const T&);
};

template <>
struct __promise_traits<void>
{
    using then_t = void();

    using result_t = std::monostate;

    template <typename F>
    using then_ret_t = std::invoke_result_t<F>;

    template <typename R>
    using then_holder_t = R();
};

template <typename T>
struct __promise_type;

template <typename T>
struct Promise
{
    using then_t = typename __promise_traits<T>::then_t;

    using result_t = typename __promise_traits<T>::result_t;

    template <typename F>
    using then_ret_t = typename __promise_traits<T>::then_ret_t<F>;

    template <typename R>
    using then_holder_t = typename __promise_traits<T>::then_holder_t<R>;

    struct State
    {
        std::optional<result_t> result_;
        std::vector<std::function<then_t>> then_;

        std::optional<std::coroutine_handle<>> task_;

        State() {}

        State(const State&) = delete;
        State& operator=(const State&) = delete;

        ~State()
        {
            if (task_.has_value()) {
                task_.value().destroy();
            }
        }
    };

    std::shared_ptr<State> state_;

    Promise()
        : state_(std::make_shared<State>())
    {
    }

    template <typename t = T>
    requires std::is_same_v<void, t>
    static Promise<t> resolve_now()
    {
        Promise<T> pro;
        pro.resolve();
        return pro;
    }

    template <typename t = T>
    requires !std::is_same_v<void, t>
             static Promise<t> resolve_now(t value)
    {
        Promise<T> pro;
        pro.resolve(std::move(value));
        return pro;
    }

    bool resolved() const { return state_->result_.has_value(); }

    template <typename F>
    auto then(F f) const -> Promise<then_ret_t<F>>
    {
        using R = then_ret_t<F>;
        std::function<then_holder_t<R>> func(f);
        if (resolved()) {
            if constexpr (std::is_same_v<void, R>) {
                if constexpr (std::is_same_v<void, T>) {
                    func();
                }
                else {
                    func(state_->result_.value());
                }
                return Promise<R>::resolve_now();
            }
            else {
                if constexpr (std::is_same_v<void, T>) {
                    return Promise<R>::resolve_now(func());
                }
                else {
                    return Promise<R>::resolve_now(func(state_->result_.value()));
                }
            }
        }
        else {
            Promise<R> pro;
            if constexpr (std::is_same_v<void, T>) {
                state_->then_.push_back([p = pro, f = std::move(func)]() mutable {
                    if constexpr (std::is_same_v<void, R>) {
                        f();
                        p.resolve();
                    }
                    else {
                        p.resolve(f());
                    }
                });
            }
            else {
                state_->then_.push_back([p = pro, f = std::move(func)](const T& val) mutable {
                    if constexpr (std::is_same_v<void, R>) {
                        f(val);
                        p.resolve();
                    }
                    else {
                        p.resolve(f(val));
                    }
                });
            }
            return pro;
        }
    }

    template <typename t = T>
    requires std::is_same_v<void, t>
    void resolve() const
    {
        if (resolved()) {
            return;
        }
        state_->result_ = std::monostate {};
        std::vector<std::function<then_t>> thens;
        thens.swap(state_->then_);
        for (const auto& f : thens) {
            f();
        }
    }

    template <typename t = T>
    requires !std::is_same_v<void, t>
             void resolve(t value) const
    {
        if (resolved()) {
            return;
        }
        state_->result_ = std::move(value);
        std::vector<std::function<then_t>> thens;
        thens.swap(state_->then_);
        for (const auto& f : thens) {
            f(state_->result_.value());
        }
    }

    using promise_type = __promise_type<T>;

    bool await_ready() const { return resolved(); }

    void await_suspend(std::coroutine_handle<> handle) const
    {
        if constexpr (std::is_same_v<void, T>) {
            then([handle]() { handle.resume(); });
        }
        else {
            then([handle](const T&) { handle.resume(); });
        }
    }

    T await_resume() const
    {
        if constexpr (std::is_same_v<void, T>) {
            ;
        }
        else {
            return state_->result_.value();
        }
    }
};

struct __promise_type_base
{
    std::suspend_never initial_suspend() noexcept { return {}; }

    std::suspend_always final_suspend() noexcept { return {}; }

    void unhandled_exception() { std::rethrow_exception(std::current_exception()); }
};

template <typename T>
struct __promise_type : public __promise_type_base
{
    Promise<T> promise_;

    Promise<T> get_return_object()
    {
        promise_.state_->task_ = std::coroutine_handle<__promise_type>::from_promise(*this);
        return promise_;
    }

    void return_value(T val) { promise_.resolve(std::move(val)); }
};

template <>
struct __promise_type<void> : public __promise_type_base
{
    Promise<void> promise_;

    Promise<void> get_return_object()
    {
        promise_.state_->task_ = std::coroutine_handle<__promise_type>::from_promise(*this);
        return promise_;
    }

    void return_void() { promise_.resolve(); }
};

}
