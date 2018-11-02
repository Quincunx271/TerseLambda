# Terse Lambda

Terse lambdas for C++. `[] TL(_1.name)` == `[] (auto&& it) { return it.name; }`

[Try it online](https://godbolt.org/z/k4qNfW)

## Why? C++ already has lambdas. Aren't they good enough?

Not really. See vector-of-bool's blog post [*Now I Am Become Perl*][vob-perl].

C++11's lambdas are much better than no lambdas, but the lambda syntax gets in
the way of just understanding what it does. I have to push the previous mental
state onto the mental stack to first understand what the lambda does, after
which I can pop and finally make sense of the full expression. Abbreviated
lambdas make this transition unnecessary in many cases.

Consider the following:

```c++
std::sort(my_vector.begin(), my_vector.end(),
    [](std::string const& lhs, std::string const& rhs) {
        return lhs.size() < rhs.size();
    });
```

When I read this expression, here's my mental state:

* `std::sort(my_vector.begin(), my_vector.end(),`. We are sorting a full vector
  by some custom order.
* `[](std::string const& lhs, std::string const& rhs)`. Lambda incoming. Do
  these parameters make sense? \*forgets that we are sorting\*. Yeah, it makes
  sense to take the parameters by `const&`.
* `return lhs.size() < rhs.size()`. Return that the left side's size is less
  than the right side's size.... Oh, so this lambda compares two `std::string`s
  by size.
* Where were we again? \*glances up\*. Oh yeah, we were sorting a vector.
* So this sorts a vector by size.

In contrast, abbreviated lambdas don't require this transition:

```c++
std::sort(my_vector.begin(), my_vector.end(),
    [][&1.size() < &2.size()]);
```

* `std::sort(my_vector.begin(), my_vector.end(),`. We are sorting a full vector
  by some custom order.
* `[][&1.size() < &2.size()]` we compare the first size to the second size. Or,
  in other words, we are sorting by size.

## What does this library do to help this?

vector-of-bool proposes roughly `[][&1.size() < &2.size()]`; roughly the
hyper-abbreviated syntax of [P0573][P0573], which also proposes the more
conservative `[](lhs, rhs) => lhs.size() < rhs.size()`. Neither syntax is in C++
today.

This library gives very similar syntax to both of the proposals:

```c++
[] TL(_1.size() < _2.size())
[] TLN(lhs, rhs) TL_RET(lhs.size() < rhs.size())
// Or more explicitly:
[] TLN(lhs, rhs) { return lhs.size() < rhs.size(); }
```

## Should I use this?

No. Seriously. You really should not use this in a production codebase. It's
okay to try it out for fun, but you should not be introducing changes to the
very syntax of C++ in your company's code.

If you really do feel the need to use this, you've been warned. Don't blame me
when your coworkers complain that they can't read your code.

## Overview

This library provides a few macros for writing terse lambdas:

* [`TL(...)`][doc-TL]. Terse Lambda. `[] TL(_1.name)`.
* [`TLN(...)`][doc-TLN]. Terse Lambda, Named. `[] TLN(a) { return a.name; }`.
* [`TLG(N, ...)`][doc-TLG]. Terse Lambda, Generic. `[] TLG(1, _1.name)`.
* [`TLV(...)`][doc-TLV]. Terse Lambda, Variadic.
  `[] TLV(do_something(_args...))`.

Of course, lambda captures can be specified as normal: `[i] TL(_1 + i)`.

Some utility macros:

* [`TL_FWD(...)`][doc-TL_FWD]. Forwards the argument as by `std::forward`.
  `do_something(TL_FWD(_1))`
* [`TL_RET(...)`][doc-TL_RET]. Returns the expression, respecting `noexcept` and
  SFINAE.

## TL - Terse Lambda

```c++
[] TL(_1.name)
```

This macro defines a lambda whose parameters can be accessed with `_n`, where
`n` is a number. Alternatively, the parameters can be accessed as a parameter
pack via the `_args` parameter: `[] TL(do_something(_args...))`.

Limitations:

* `_1`, `_2`, `_3`, `_4`. There's no `_5` or above. If you really need that many
  parameters, you shouldn't be using numbered parameters. Use `TLN` or a
  non-terse lambda.
* Not `noexcept` aware or SFINAE friendly. Use `TLN`, `TLG`, or `TLV` instead.

[Overview][doc-overview]

## TLN - Terse Lambda, Named

```c++
[] TLN(a) { return a.name; }
```

The most versatile of all terse lambdas, `TLN` defines a macro argument list
with the names you give it. This does not give you an automatic lambda body;
you have to define that yourself. Can be combined with `TL_RET` for a terse
body:

```c++
[] TLN(a) TL_RET(a.name)
```

Variadic lambdas can be declared by prepending a name with `...`:

```c++
[] TLN(...args) TL_RET(do_something(args...))
```

Limitations:

* Does not work with no arguments. Just use a regular lambda:
  `[]() TL_RET(a.name)`

[Overview][doc-overview]

## TLG - Terse Lambda, Generic

```c++
[] TLG(1, _1.name)
```

This macro works around the limitations of `TL` by requiring you to specify the
number of function parameters as the first argument. Doing so allows `TLG` to be
`noexcept` aware and SFINAE friendly, but at the cost of parameter packs.

Limitations:

* Cannot access parameters as a parameter pack. Use `TLV` instead.

[Overview][doc-overview]

## TLV - Terse Lambda, Variadic-only

```c++
[] TLV(do_something(_args...))
```

Variadic-only, `noexcept` aware, and SFINAE friendly terse lambda.

Limitations:

* Cannot access arguments with `_n` syntax.

[Overview][doc-overview]

## TL_FWD

```c++
TL_FWD(x)
```

Forwards the parameter as if by `std::forward<decltype(x)>(x)`.

[Overview][doc-overview]

## TL_RET

```c++
TL_RET(<expr>)
```

Returns the expression in a `noexcept` aware and SFINAE friendly manner.
Designed to be used directly after the closing `)` of a function parameter list.

Expands to:

```c++
noexcept(noexcept(<expr>))
    -> decltype(<expr>)
{
    return <expr>;
}
```

Limitations:

* Inhibits the use of C++20 constraints, which appear *after* the `noexcept`
  specification.

[Overview][doc-overview]


  [vob-perl]: https://vector-of-bool.github.io/2018/10/31/become-perl.html
  [P0573]: https://wg21.link/P0573

  [doc-overview]: #overview
  [doc-TL]: #tl---terse-lambda
  [doc-TLN]: #tln---terse-lambda-named
  [doc-TLG]: #tlg---terse-lambda-generic
  [doc-TLV]: #tlv---terse-lambda-variadic-only
  [doc-TL_FWD]: #tl_fwd
  [doc-TL_RET]: #tl_ret
