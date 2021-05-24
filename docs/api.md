# Overview

This library provides the following macros for writing terse lambdas:

* [`TL(<expr>)`][doc-TL]. Terse Lambda. `[] TL(_1.name)`.
* [`TLR(<expr>)`][doc-TLR]. Terse Lambda with reference return. `[] TLR(_1.name())`.

It also provides the following utility macro:

* [`TL_FWD(<expr>)`][doc-TL_FWD]. Forwards the argument as by `std::forward`.
  `do_something(TL_FWD(_1))`.

# TL - Terse Lambda

```c++
[] TL(<expr>)
```

Defines a lambda with parameters:
* `_n` where `n` is a number (`_1`, `_2`, ...). These are positional parameters.
* `_args`. This gives you a parameter pack of all the arguments.

Lambda captures can be specified as normal: `[i] TL(_1 + i)`.

`TL` returns _by value_. If your expression returns a reference and you want the
lambda to return by reference (i.e. a lambda returning `decltype(auto)`), use [`TLR`][doc-TLR].

Metaprogramming properties:
* `TL` is `noexcept` and SFINAE\* friendly.

Limitations:
* `_1`, `_2`, `_3`, `_4`. There's no `_5` or above. You likely should not be
  using a terse lambda if you need numbered arguments that high.
* Although `TL` is mostly SFINAE-friendly, there is a rough edge when trying to
  overload it with a function that takes a fixed number of unconstrained
  arguments; it will call the overload rather than the terse lambda.

Examples:

```c++
sort(people, [] TL(_1.name < _2.name));
transform(people, ages.begin(), [] TL(_1.age()));
reduce(elapsed_times, [] TL(_1 + _2));
copy_if(documents, back_inserter(images),
  [&image_exts] TL(image_exts.contains(_1.extension())));

auto hypotenuse = [] TL(std::sqrt(_1 * _1 + _2 * _2));
auto second = [] TL(_2);
auto add_n = [n] TL(n + _2);
```

# TLR - Terse Lambda Reference

```c++
[] TLR(_1.name)
```

The same as [`TL`][doc-TL], but `TLR` returns _by `decltype(auto)`_. This style
has more edge cases and is trickier to use correctly. By default, for safety,
use [`TL`][doc-TL].

Summary:

```c++
[] TL(<expr>) == []() { return <expr>; }
[] TLR(<expr>) == []() -> decltype(auto) { return <expr>; }
```

Examples:

```c++
sort_by(people, [] TLR(_1.name)); // using [] TL(_1.name) would copy the name to compare
sort_by(documents, [&] TLR(history.access_time(_1)));
auto second = [] TLR(TL_FWD(_2));
```

# TL_FWD

```c++
TL_FWD(<expr>)
```

Forwards the parameter as if by `std::forward<decltype(<expr>)>(<expr>)`.

  [doc-overview]: #overview
  [doc-TL]: #tl---terse-lambda
  [doc-TLR]: #tlr---terse-lambda-reference
  [doc-TL_FWD]: #tl_fwd
