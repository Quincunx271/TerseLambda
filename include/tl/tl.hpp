/*
Copyright Justin Bassett 2018-2021
Distributed under the MIT License
(See the accompanying LICENSE file or https://opensource.org/licenses/MIT)
*/

#pragma once

// Implementations must ignore attributes if they don't do anything with them.
// Unfortunately, MSVC doesn't obey this rule, and MSVC's __forceinline is not
// an attribute.
#ifdef _MSC_VER
#define TL_ALWAYS_INLINE __forceinline
#else
#define TL_ALWAYS_INLINE [[gnu::always_inline]]
#endif

// Not using std::forward, but rolling our own. This enables running this header
// through the preprocessor while still getting meaningful output

#define TL_FWD(...) static_cast<decltype(__VA_ARGS__)&&>(__VA_ARGS__)

// Creates a terse lambda with the given expression.
// [] TL(_1.do_something())
// @note Not `noexcept`-friendly or sfinae-friendly. Use TLV or TLG if those
// attributes are necessary
#define TL(...)                                                                \
    <typename... Args>(Args && ... _args) noexcept(                            \
        decltype([]([[maybe_unused]] auto&& _1, [[maybe_unused]] auto&& _2,    \
                     [[maybe_unused]] auto&& _3, [[maybe_unused]] auto&& _4) { \
            return ::tl::detail::bool_t<noexcept(__VA_ARGS__)> {};             \
        }(::tl::detail::nth<0>(TL_FWD(_args)...),                              \
                     ::tl::detail::nth<1>(TL_FWD(_args)...),                   \
                     ::tl::detail::nth<2>(TL_FWD(_args)...),                   \
                     ::tl::detail::nth<3>(TL_FWD(_args)...)))::value)          \
        ->decltype(auto) requires requires(                                    \
            ::tl::detail::nth_type<0, Args&&...> _1,                           \
            ::tl::detail::nth_type<1, Args&&...> _2,                           \
            ::tl::detail::nth_type<2, Args&&...> _3,                           \
            ::tl::detail::nth_type<3, Args&&...> _4, Args && ... _args)        \
    {                                                                          \
        __VA_ARGS__;                                                           \
    }                                                                          \
    {                                                                          \
        [[maybe_unused]] auto&& _1 = ::tl::detail::nth<0>(TL_FWD(_args)...);   \
        [[maybe_unused]] auto&& _2 = ::tl::detail::nth<1>(TL_FWD(_args)...);   \
        [[maybe_unused]] auto&& _3 = ::tl::detail::nth<2>(TL_FWD(_args)...);   \
        [[maybe_unused]] auto&& _4 = ::tl::detail::nth<3>(TL_FWD(_args)...);   \
                                                                               \
        return __VA_ARGS__;                                                    \
    }

namespace tl::detail {
    struct not_a_parameter
    { };

    // Functions marked always inline for better -O0 code-gen; don't have to
    // call N functions

    // Dev: these functions don't need to be sfinae or noexcept friendly;
    // they're only called by the macro which is not sfinae or noexcept
    // friendly.

    template <int N, typename T, typename... Ts>
    TL_ALWAYS_INLINE constexpr decltype(auto) nth_impl(T&& t, Ts&&... ts)
    {
        if constexpr (N == 0) {
            return TL_FWD(t);
        } else {
            return tl::detail::nth_impl<N - 1>(TL_FWD(ts)...);
        }
    }

    template <int N, typename... Ts>
    TL_ALWAYS_INLINE constexpr decltype(auto) nth(Ts&&... ts)
    {
        if constexpr (N < sizeof...(Ts)) {
            return tl::detail::nth_impl<N>(TL_FWD(ts)...);
        } else {
            return not_a_parameter {};
        }
    }

    template <int I, typename... Args>
    extern not_a_parameter nth_ref;

    template <typename T0, typename... Args>
    extern T0 nth_ref<0, T0, Args...>;

    template <typename T0, typename T1, typename... Args>
    extern T1 nth_ref<1, T0, T1, Args...>;

    template <typename T0, typename T1, typename T2, typename... Args>
    extern T2 nth_ref<2, T0, T1, T2, Args...>;

    template <typename T0, typename T1, typename T2, typename T3,
        typename... Args>
    extern T3 nth_ref<3, T0, T1, T2, T3, Args...>;

    template <int I, typename... Args>
    using nth_type = decltype(nth_ref<I, Args...>);

    template <bool V>
    struct bool_t
    {
        static constexpr bool value = V;
    };
}
