/*
Copyright Justin Bassett 2018-2021
Distributed under the MIT License
(See the accompanying LICENSE file or https://opensource.org/licenses/MIT)
*/

#pragma once

// Implementations must ignore attributes if they don't do anything with them.
// Unfortunately, implementations still emit warnings, and MSVC's __forceinline is not an attribute.
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
// This returns by value, like the default lambda without `-> decltype(auto)`.
// Use this unless you know your type isn't cheap to copy and the expression yields a reference.
#define TL(...) TLR(::tl_detail::copy(__VA_ARGS__))

// Creates a decltype(auto) terse lambda with the given expression.
// [] TLR(_1.do_something())
// Use this to avoid a copy if your expression returns a reference.
#define TLR(...)                                                                                   \
    <typename... TL_DETAIL_ID(TlArgs)>(TL_DETAIL_ID(TlArgs) && ... _args) noexcept(                \
        decltype([](TL_DETAIL_ID(TlArgs) && ... _args) {                                           \
            [[maybe_unused]] auto&& _1 = ::tl_detail::nth<0>(TL_FWD(_args)...);                    \
            [[maybe_unused]] auto&& _2 = ::tl_detail::nth<1>(TL_FWD(_args)...);                    \
            [[maybe_unused]] auto&& _3 = ::tl_detail::nth<2>(TL_FWD(_args)...);                    \
            [[maybe_unused]] auto&& _4 = ::tl_detail::nth<3>(TL_FWD(_args)...);                    \
            return ::tl_detail::bool_t<noexcept(__VA_ARGS__)> {};                                  \
        }(TL_FWD(_args)...))::value)                                                               \
        ->decltype(auto) requires TL_DETAIL_REQUIRES(, __VA_ARGS__)                                \
    {                                                                                              \
        [[maybe_unused]] auto&& _1 = ::tl_detail::nth<0>(TL_FWD(_args)...);                        \
        [[maybe_unused]] auto&& _2 = ::tl_detail::nth<1>(TL_FWD(_args)...);                        \
        [[maybe_unused]] auto&& _3 = ::tl_detail::nth<2>(TL_FWD(_args)...);                        \
        [[maybe_unused]] auto&& _4 = ::tl_detail::nth<3>(TL_FWD(_args)...);                        \
                                                                                                   \
        return __VA_ARGS__;                                                                        \
    }

#define TL_DETAIL_REQUIRES(NOEXCEPT, ...)                                                          \
    requires(decltype(::tl_detail::nth<0>(TL_FWD(_args)...)) _1,                                   \
        decltype(::tl_detail::nth<1>(TL_FWD(_args)...)) _2,                                        \
        decltype(::tl_detail::nth<2>(TL_FWD(_args)...)) _3,                                        \
        decltype(::tl_detail::nth<3>(TL_FWD(_args)...)) _4)                                        \
    {                                                                                              \
        {__VA_ARGS__} NOEXCEPT;                                                                    \
    }

#define TL_DETAIL_ID(x) TL_DETAIL_CAT(x, __LINE__)
#define TL_DETAIL_CAT(a, b) TL_DETAIL_CAT2(a, b)
#define TL_DETAIL_CAT2(a, b) a##b

namespace tl_detail {
    template <typename T>
    constexpr auto decay_fn(T&& t) noexcept
    {
        return TL_FWD(t);
    }

    template <bool B>
    struct bool_t
    {
        static constexpr bool value = B;
    };

    template <typename T>
    extern T declval() noexcept;

    template <typename T>
    using decay_t = decltype(tl_detail::decay_fn(tl_detail::declval<T>()));

    template <typename T>
    constexpr bool is_noexcept_constructible_v = noexcept(decay_t<T>(tl_detail::declval<T>()));

    struct not_a_parameter
    { };

    // Functions marked always inline for better -O0 code-gen; don't have to
    // call N functions

    template <typename T>
    TL_ALWAYS_INLINE constexpr auto copy(T&& t) noexcept(is_noexcept_constructible_v<T&&>)
    {
        return TL_FWD(t);
    }

    // Note: these functions don't need to be sfinae or noexcept friendly;
    // they're only used in contexts where it won't affect anything.

    template <int N, typename T, typename... Ts>
    TL_ALWAYS_INLINE constexpr decltype(auto) nth_impl(T&& t, Ts&&... ts)
    {
        if constexpr (N == 0) {
            return TL_FWD(t);
        } else {
            return tl_detail::nth_impl<N - 1>(TL_FWD(ts)...);
        }
    }

    template <int N, typename... Ts>
    TL_ALWAYS_INLINE constexpr decltype(auto) nth(Ts&&... ts)
    {
        if constexpr (N < sizeof...(Ts)) {
            return tl_detail::nth_impl<N>(TL_FWD(ts)...);
        } else {
            return not_a_parameter {};
        }
    }
}
