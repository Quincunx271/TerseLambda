#include <tl/tl.hpp>

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <cassert>
#include <type_traits>
#include <utility>

namespace {
    template <typename... Ls>
    struct overload : Ls...
    {
        using Ls::operator()...;
    };

    template <typename... Ls>
    overload(Ls...) -> overload<Ls...>;
}

int main()
{
    // TL(...)
    {
        // basic
        assert([] TL(_1)(2) == 2);
        assert([] TL(_1 + _2)(1, 2) == 3);
        assert([] TL(_2)(3, 4) == 4);

        // Forwarding
        int a = 0;
        int const b = 0;
        constexpr auto fn = [] TL(TL_FWD(_1));
        static_assert(std::is_same_v<decltype(fn(a)), int&>);
        static_assert(std::is_same_v<decltype(fn(std::move(a))), int&&>);
        static_assert(std::is_same_v<decltype(fn(b)), int const&>);
        static_assert(std::is_same_v<decltype(fn(std::move(b))), int const&&>);

        assert(&fn(a) == &a);
        assert(&fn(b) == &b);

        // SFINAE-friendly
        struct foo
        {
            int value;
        };

        static_assert(overload {
                          [] TL(_1.value + _2.value),
                          [](auto&&...) { return -10; },
                      }(foo {42}, foo {2})
            == 44);

        static_assert(overload {
                          [] TL(_1.value + _2.thing),
                          [](auto&&...) { return -10; },
                      }(foo {42}, foo {2})
            == -10);

        // noexcept-friendly
        struct bar
        {
            bool yes() const noexcept
            {
                return true;
            }
            bool no() const
            {
                return false;
            }
        };
        struct baz
        {
            bool yes() const noexcept(false)
            {
                return true;
            }
        };
        static_assert(noexcept([] TL(_1.yes())(bar {})));
        static_assert(!noexcept([] TL(_1.no())(bar {})));
        static_assert(noexcept([] TL((_args.yes() && ...))(bar {})));
        static_assert(!noexcept([] TL((_args.no() && ...))(bar {})));
        static_assert(noexcept([] TL((_args.no() && ...))()));
        static_assert(!noexcept([] TL((_args.yes() && ...))(bar {}, baz {})));
    }

    // TLN(...)
    {
        // non-capturing
        // basic

        // ill-formed prior to C++20; just use []{ return 2; }();
        // Also, not implemented for after C++20; it would expand incorrectly
        // assert([] TLN() { return 2; }() == 2);
        assert([] TLN(a) { return a; }(2) == 2);
        assert([] TLN(a, b) { return a + b; }(1, 2) == 3);
        assert([] TLN(a, b) {
            (void)a;
            return b;
        }(3, 4)
            == 4);

        // Forwarding (tests TL_RET(...))
        int a = 0;
        int const b = 0;
        constexpr auto fn = [] TLN(a) TL_RET(a);
        static_assert(std::is_same_v<decltype(fn(a)), int&>);
        static_assert(std::is_same_v<decltype(fn(std::move(a))), int&&>);
        static_assert(std::is_same_v<decltype(fn(b)), int const&>);
        static_assert(std::is_same_v<decltype(fn(std::move(b))), int const&&>);
        static_assert(noexcept(fn(a)));

        assert(&fn(a) == &a);
        assert(&fn(b) == &b);

        // Variadic args
        assert([] TLN(... args) { return (args + ...); }(1, 2, 3, 4, 5, 6, 7, 8,
                   9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20)
            == 20 * (20 + 1) / 2);
    }
    {
        // capturing works
        int i = 42;
        assert([i] TLN(a) { return i + a; }(3) == i + 3);
    }

    // TLV
    {
        // non-capturing
        auto sum = [] TLV((_args + ...));
        assert(sum(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                   18, 19, 20)
            == 20 * (20 + 1) / 2);

        static_assert(noexcept(sum(1, 2, 3)));

        // SFINAE-friendly
        struct foo
        {
            int value;
        };

        static_assert(overload {
                          [] TLV((_args.thing + ...)),
                          [](auto&& it) { return it.value; },
                      }(foo {42})
            == 42);
    }
    {
        // capturing works
        int i = 42;
        auto sum = [i] TLV(i + (_args + ...));
        assert(sum(1) == 43);
    }

    // TLG
    {
        // non-capturing
        // basic
        assert([] TLG(0, 2)() == 2);
        assert([] TLG(1, 2)(4) == 2);
        assert([] TLG(1, std::move(_1))(2) == 2);
        assert([] TLG(2, _1 + _2)(1, 2) == 3);
        assert([] TLG(2, std::move(_2))(3, 4) == 4);

        // Forwarding
        int a = 0;
        int const b = 0;
        constexpr auto fn = [] TLG(1, TL_FWD(_1));
        static_assert(std::is_same_v<decltype(fn(a)), int&>);
        static_assert(std::is_same_v<decltype(fn(std::move(a))), int&&>);
        static_assert(std::is_same_v<decltype(fn(b)), int const&>);
        static_assert(std::is_same_v<decltype(fn(std::move(b))), int const&&>);

        assert(&fn(a) == &a);
        assert(&fn(b) == &b);

        // SFINAE-friendly
        struct foo
        {
            int value;
        };

        static_assert(overload {
                          [] TLG(2, _1.value + _2.thing),
                          [](auto&& it, auto&&) { return it.value; },
                      }(foo {42}, foo {2})
            == 42);
    }
    {
        // capturing works
        int i = 42;
        assert([i] TLG(1, i + _1)(3) == i + 3);
    }
}
