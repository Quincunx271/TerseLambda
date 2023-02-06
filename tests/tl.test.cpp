#include <tl/tl.hpp>

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <cassert>
#include <string>
#include <type_traits>
#include <utility>

namespace {
    // For testing how [] TL(...) behaves when it is overloaded. This is particularly relevant to
    // SFINAE friendliness.
    template <typename... Ls>
    struct overload : Ls...
    {
        using Ls::operator()...;
    };

    template <typename... Ls>
    explicit overload(Ls...) -> overload<Ls...>;

    struct stat_counts
    {
        int num_copy_ctor = 0;
        int num_move_ctor = 0;
        int num_copy_assign = 0;
        int num_move_assign = 0;
    };

    // Counts copy/move operations, for verifying that we do not have unexpected copies or moves.
    struct count_stats
    {
        stat_counts* counts;

        // clang-format off
        count_stats(stat_counts* counts) : counts(counts) {}
        count_stats(const count_stats& other) : counts(other.counts) { ++counts->num_copy_ctor; }
        count_stats(count_stats&& other) : counts(other.counts) { ++counts->num_move_ctor; }
        count_stats& operator=(const count_stats& other) { counts = other.counts; ++counts->num_copy_assign; return *this; }
        count_stats& operator=(count_stats&& other) { counts = other.counts; ++counts->num_move_assign; return *this; }
        // clang-format on
    };
}

// Rather than using a testing framework, we test directly in main() to avoid dependencies.
int main()
{
    // Straightforward usage examples.
    assert([] TL(_1)(2) == 2);
    assert([] TL(_1 + _2)(1, 2) == 3);
    assert([] TL(_2)(3, 4) == 4);
    // Capturing works.
    {
        int i = 42;
        assert([i] TL(i + _1)(-42) == 0);
        auto sum = [i] TL(i + (_args + ...));
        assert(sum(1) == 43);
        assert(sum(-42, 1, 2) == 3);
    }
    // Possibly counterintuitive examples.
    // This is okay, because you should generally use `[] TL(...)` in a narrow
    // scope where it's clear what the lambda is being called with (e.g. as an
    // argument to a std algorithm).
    assert([] TL(_1)(1, 2, 3) == 1); // You can pass more arguments than are used.
    assert([] TL(42)(3) == 42); // Including if you don't use any of the arguments.

    // Variadic usage examples.
    {
        // non-capturing
        auto sum = [] TL((_args + ...));
        assert(sum(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20)
            == 20 * (20 + 1) / 2);

        static_assert(noexcept(sum(1, 2, 3)));

        // SFINAE-friendly
        struct foo
        {
            int value;
        };
        static_assert(overload {
                          [] TL((_args.thing + ...)),
                          [](auto&&... it) { return (it.value + ...); },
                      }(foo {42})
            == 42);
    }

    // SFINAE-friendly
    {
        struct foo
        {
            int value;
        };
        // implicitly convertible from foo.
        struct bar
        {
            bar(foo) { }
        };
        {
            constexpr auto l = overload {
                [] TL(_1.value + _2.value),
                [](auto&&...) { return -10; },
            };
            static_assert(l(foo {42}, foo {2}) == 44);
        }
        static_assert(overload {
                          [] TL(_1.value + _2.thing), // .thing doesn't exist.
                          [](auto&&...) { return -10; },
                      }(foo {42}, foo {2})
            == -10);
        // Counterintuitively, SFINAE will prefer ordinary lambdas with a fixed
        // number of arguments over terse lambdas:
        static_assert(overload {
                          // We might expect the overloads to be ambiguous, but
                          // the ordinary lambda is selected instead. This is
                          // an artifact of how overload resolution interacts
                          // with variadic arguments: variadic arguments are
                          // ordered after fixed numbers of arguments.
                          [] TL(_1.value + _2.value),
                          [](foo, foo) { return -10; },
                      }(foo {42}, foo {2})
            == -10);
        static_assert(overload {
                          // In this case, the ordinary lambda requires
                          // conversions, so it is properly ordered after the
                          // terse lambda despite this.
                          [] TL(_1.value + _2.value),
                          [](bar, bar) { return -10; },
                      }(foo {42}, foo {2})
            == 44);
    }

    // Value category tests.
    {
        // prvalues are respected.
        constexpr auto fn = [] TLR(42);
        static_assert(std::is_same_v<decltype(fn()), int>);
    }
    {
        // Forwarding works.
        int a = 0;
        int const b = 0;
        constexpr auto fn = [] TLR(TL_FWD(_1));
        static_assert(std::is_same_v<decltype(fn(a)), int&>);
        static_assert(std::is_same_v<decltype(fn(std::move(a))), int&&>);
        static_assert(std::is_same_v<decltype(fn(b)), int const&>);
        static_assert(std::is_same_v<decltype(fn(std::move(b))), int const&&>);

        assert(&fn(a) == &a);
        assert(&fn(b) == &b);
    }

    // Value category tests when accessing a data member.
    // decltype(foo.str) is `std::string` regardless of the value category of
    // `foo`. We want uses of it in the lambda to inherit its category from the
    // value category of `foo`.
    struct member_type_access
    {
        std::string str;
        const std::string& str_ref;
    };
    {
        constexpr auto fn = [] TLR(TL_FWD(_1).str);
        static_assert(
            std::is_same_v<decltype(fn(std::declval<member_type_access&&>())), std::string&&>);
        static_assert(std::is_same_v<decltype(fn(std::declval<const member_type_access&>())),
            const std::string&>);
    }
    {
        constexpr auto fn = [] TLR(_1.str);
        static_assert(std::is_same_v<decltype(fn(std::declval<const member_type_access&>())),
            const std::string&>);
        // This is a gotcha case, but it's the same as ordinary C++ functions.
        // If you don't move the function argument `_1`, it will be an lvalue
        // reference.
        static_assert(
            std::is_same_v<decltype(fn(std::declval<member_type_access&&>())), std::string&>);
    }
    {
        constexpr auto fn = [] TLR(_1.str_ref);
        // The value category can't override the data member.
        static_assert(
            std::is_same_v<decltype(fn(std::declval<member_type_access&&>())), const std::string&>);
    }
    {
        // If we _really_ want the by-value behavior, ask for it by using `[] TL(...)` instead of
        // `[] TLR(...)`.
        constexpr auto fn = [] TL(TL_FWD(_1).str);
        static_assert(
            std::is_same_v<decltype(fn(std::declval<member_type_access&&>())), std::string>);
    }
    {
        constexpr auto fn = [] TL(_1.str); // If we _really_ want the by-value behavior, ask for it.
        static_assert(
            std::is_same_v<decltype(fn(std::declval<const member_type_access&>())), std::string>);
    }
    // Ensure there's no extra copies of members. This tests the same thing as
    // the prior typed tests, but in a different way.
    struct member_count_access
    {
        count_stats stats;
    };
    stat_counts counts;
    member_count_access counter {&counts};
    [] TLR(_1.stats)(counter);
    assert(counts.num_copy_ctor == 0);
    assert(counts.num_move_ctor == 0);
    assert(counts.num_copy_assign == 0);
    assert(counts.num_move_assign == 0);

    // noexcept-friendly
    struct bar
    {
        bool yes() const noexcept { return true; }
        bool no() const { return false; }
    };
    struct baz
    {
        // Same name as bar::yes, but not actually noexcept (for variadic verification).
        bool yes() const noexcept(false) { return true; }
    };
    {
        auto l = [] TL(_1.yes());
        static_assert(noexcept(l(bar {})));
    }
    {
        auto l = [] TL(_1.no());
        static_assert(!noexcept(l(bar {})));
    }
    {
        auto l = [] TL((_args.yes() && ...));
        static_assert(noexcept(l(bar {})));
    }
    {
        auto l = [] TL((_args.no() && ...));
        static_assert(!noexcept(l(bar {})));
    }
    {
        auto l = [] TL((_args.no() && ...));
        static_assert(noexcept(l()));
    }
    {
        auto l = [] TL((_args.yes() && ...));
        static_assert(!noexcept(l(bar {}, baz {})));
    }
}
