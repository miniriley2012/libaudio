//
// Created by Riley Quinn on 8/17/20.
//

#ifndef LIBAUDIO_LIBAUDIO_DETAIL_GUARD_HPP
#define LIBAUDIO_LIBAUDIO_DETAIL_GUARD_HPP

#include <mutex>

#define _DEFER_CONCAT(x, y) x ## y
#define _DEFER_CONCAT2(x, y) _DEFER_CONCAT(x, y)
#define DEFER(f, ...) Guard _DEFER_CONCAT2(_guard_, __COUNTER__) (f __VA_OPT__(,) __VA_ARGS__)

template<typename Function, typename ...Args>
constexpr bool is_valid_guard_function = std::is_invocable_v<Function, Args...> &&
                                         (std::is_void_v<std::invoke_result_t<Function, Args...>> ||
                                          std::is_default_constructible_v<std::invoke_result_t<Function, Args...>>);

template<typename Function, typename ...Args> requires is_valid_guard_function<Function, Args...>
class Guard {
    using result_type = std::invoke_result_t<Function, Args...>;
    std::function<result_type()> f;
    std::once_flag flag;
public:
    explicit Guard(Function func, Args ...args) : f{[func, ...args = std::forward<Args>(args)]() mutable {
        return func(std::forward<Args>(args)...);
    }} {}

    result_type fire() {
        try {
            if constexpr (std::is_void_v<result_type>) {
                std::call_once(flag, f);
                return;
            } else {
                result_type t;
                std::call_once(flag, [&t, &f = this->f] { t = f(); });
                return t;
            }
        } catch (...) {
            if (std::uncaught_exceptions() == 0) {
                throw;
            }
        }
        return {};
    }

    void cancel() { std::call_once(flag, [] {}); }

    ~Guard() { fire(); }
};


#endif //LIBAUDIO_LIBAUDIO_DETAIL_GUARD_HPP
