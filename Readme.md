# Terse Lambda

Terse lambdas for C++. `[] TL(_1.name)` == `[] (auto&& it) { return it.name; }`

[Try it online](https://godbolt.org/z/aP3nEYn6b)

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

This library gives very similar syntax to the former proposal:

```c++
[] TL(_1.size() < _2.size())
```

## Should I use this?

No. Seriously. You really should not use this in a production codebase. It's
okay to try it out for fun, but you should not be introducing changes to the
very syntax of C++ in your company's code.

If you really do feel the need to use this, you've been warned. Don't blame me
when your coworkers complain that they can't read your code.

## Documentation

See [the API documentation](docs/api.md).

For consuming TerseLambda using package managers, see [the package manager documentation](docs/package-management.md).


  [vob-perl]: https://vector-of-bool.github.io/2018/10/31/become-perl.html
  [P0573]: https://wg21.link/P0573
