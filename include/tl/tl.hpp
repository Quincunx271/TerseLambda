/*
Copyright Justin Bassett 2018
Distributed under the MIT License
(See the accompanying LICENSE file or https://opensource.org/licenses/MIT)
*/

#pragma once

#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/repetition/enum_shifted_params.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/transform.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

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

#define TL_RET(...)                                                            \
    noexcept(noexcept(__VA_ARGS__))->decltype(__VA_ARGS__)                     \
    {                                                                          \
        return __VA_ARGS__;                                                    \
    }

// Creates a terse lambda with the given expression.
// [] TL(_1.do_something())
// @note Not `noexcept`-friendly or sfinae-friendly. Use TLV or TLG if those
// attributes are necessary
#define TL(...)                                                                \
    (auto&&... _args)                                                          \
        ->decltype(auto) requires requires(                                    \
            ::tl::detail::nth_type<0, decltype(_args)&&...> _1,                \
            ::tl::detail::nth_type<1, decltype(_args)&&...> _2,                \
            ::tl::detail::nth_type<2, decltype(_args)&&...> _3,                \
            ::tl::detail::nth_type<3, decltype(_args)&&...> _4)                \
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

// Creates a variadic-only terse lambda.
// [] TLV(call_something(_args...))
// @note `noexcept`-friendly and sfinae-friendly.
#define TLV(...)                                                               \
    (auto&&... _args) noexcept(noexcept(__VA_ARGS__))->decltype(__VA_ARGS__)   \
    {                                                                          \
        return __VA_ARGS__;                                                    \
    }

// Dev: Produces a Boost.PP sequence from 1 to N inclusive: (1)(2)(...)(N)
#define TL_DETAIL_NUM_SEQ(N)                                                   \
    BOOST_PP_VARIADIC_TO_SEQ(BOOST_PP_ENUM_SHIFTED_PARAMS(BOOST_PP_INC(N), ))

// Dev: creates `[[maybe_unused]] auto&&` function parameters.
// [](
//     [[maybe_unused]] auto&& _1,
//     [[maybe_unused]] auto&& _2,
//     ...
// )
#define TL_DETAIL_CREATE_ARG_N(s, data, elem)                                  \
    [[maybe_unused]] auto&& BOOST_PP_CAT(_, elem)

#define TL_DETAIL_CREATE_ARGS(N)                                               \
    BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(                                  \
        TL_DETAIL_CREATE_ARG_N, , TL_DETAIL_NUM_SEQ(N)))

// Creates a generic terse lambda
// [] TLG(1, _1.do_something())
// @argument1 The number of parameters on the lambda. Must be a preprocessor
// number (either a #define or a literal)
// @note `noexcept`-friendly and sfinae-friendly.
// @note Not variadic.
#define TLG(N, ...)                                                            \
    BOOST_PP_IF(N, (TL_DETAIL_CREATE_ARGS(N)), ())                             \
    noexcept(noexcept(__VA_ARGS__))->decltype(__VA_ARGS__)                     \
    {                                                                          \
        return __VA_ARGS__;                                                    \
    }

#define TL_DETAIL_CREATE_NAMED_ARG(s, data, elem) auto&& elem

// Creates a terse lambda with argument names
// [] TLN(a, b) { return a + b; }
// [] TLN(a, b) TL_RET(a + b);
// The most versatile of all terse lambdas.
// @note As `noexcept`-friendly and sfinae-friendly as you write it
#define TLN(...)                                                               \
    (BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(                                 \
        TL_DETAIL_CREATE_NAMED_ARG, , BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))))

namespace tl::detail {
    struct not_a_parameter
    {};

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
            return not_a_parameter{};
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
}
