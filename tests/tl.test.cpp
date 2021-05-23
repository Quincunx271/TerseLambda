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
        constexpr auto fn = [] TLR(TL_FWD(_1));
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

        {
            auto l = overload {
                [] TL(_1.value + _2.value),
                [](auto&&...) { return -10; },
            };
            static_assert(l(foo {42}, foo {2}) == 44);
        }

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

    // TLV
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
                          [](auto&& it) { return it.value; },
                      }(foo {42})
            == 42);
    }
    {
        // capturing works
        int i = 42;
        auto sum = [i] TL(i + (_args + ...));
        assert(sum(1) == 43);
    }
}
