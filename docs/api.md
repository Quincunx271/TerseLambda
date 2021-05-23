
# Overview

This library provides the following macros for writing terse lambdas:

* [`TL(<expr>)`][doc-TL]. Terse Lambda. `[] TL(_1.name)`.
* [`TLR(<expr>)`][doc-TLR]. Terse Lambda with reference return. `[] TLR(_1.name())`.

As well as a utility macro:

* [`TL_FWD(<expr>)`][doc-TL_FWD]. Forwards the argument as by `std::forward`.
  `do_something(TL_FWD(_1))`.

# TL - Terse Lambda

```c++
[] TL(_1.name)
```

This macro defines a lambda whose parameters can be accessed with `_n`, where
`n` is a number. Alternatively, the parameters can be accessed as a parameter
pack via the `_args` parameter: `[] TL(do_something(_args...))`.
Of course, lambda captures can be specified as normal: `[i] TL(_1 + i)`.
`TL` is `noexcept` and SFINAE\* friendly.

`TL` returns by value, meaning that if your expression returns a reference, that
reference will be copied rather than returned directly. To return by reference
(i.e. a lambda returning `decltype(auto)`), use [`TLR`][doc-TLR].

Limitations:

* `_1`, `_2`, `_3`, `_4`. There's no `_5` or above. You likely should not be
  using a terse lambda if you need numbered arguments that high.
* Although `TL` is mostly SFINAE-friendly, there is a rough edge when trying to
  overload it with a function that takes a fixed number of unconstrained
  arguments; it will call the overload rather than the terse lambda.

# TLR - Terse Lambda Reference

```c++
[] TLR(_1.name)
```

The same as [`TL`][doc-TL], but returns "by reference." In short, `TL` defines
a lambda that returns by value: `[]() { return <expr>; }`. `TLR` defines a
lambda that returns with `decltype(auto)`:
`[]() -> decltype(auto) { return <expr>; }`.

Use this when you know that copying/moving the return value is expensive.
This does have more rough edges, so prefer `TL` by default.

# TL_FWD

```c++
TL_FWD(x)
```

Forwards the parameter as if by `std::forward<decltype(x)>(x)`.

  [doc-overview]: #overview
  [doc-TL]: #tl---terse-lambda
  [doc-TLR]: #tlr---terse-lambda-reference
  [doc-TL_FWD]: #tl_fwd
