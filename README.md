# utf8-shim 0.9
Minimal C++ library for easy cross-platform utf8-everywhere

## Introduction

This library started as something I was copying to my C++ projects because I wanted minimal utf-8 support on Windows. With modern C++ it is increasingly easy to use utf-8 in every scenario and ignore wide char and wide streams except in the most rare circumstances. This library is primarily for me and therefore people might find it too opinionated, but if other people can get some use out of it, then I am happy. I put this up so I could more easily track its use in my own projects and keep any bugfixes centralized.

## What this library does not include

The UTF-16 <-> UTF-8 conversions are entirely Windows specific, because this library assumes (rightly) that pretty much any other OS can handle utf-8 natively.

There is also currently no additional functionality for dealing with utf-8 representations (utf8 aware strlen, character level iteration, etc.). This is because I don't generally need this functionality for most of my own projects.

If you have a use-case that is not covered that seems like it would fall in the scope of a minimal library such as this, I am open to pull-requests subject to code review.

## Getting Started

Using the library is easy. In the simplest form, just include `utf8_shim.hpp` anywhere you would use iostream, and use `u8s::cout`, `u8s::cout`, `u8s::cin`, `u8s::cerr`, and `u8s::clog` wherever you would normally use the `std` versions.

I have also included addtional routines for getting utf-8 encoded args from Windows without hassle and some extra logic for setting the output mode of the console.

You can use standard stream operators, but I have also included one called `u8s::newline` for convenience which is the same as `std::endl` but does not flush the stream.

For simplicity you can optionally include utf8_shim.cpp in your project, which includes a definition of main that does everything for you and calls a new user-defined entry-point `int utf8_main(std::vector<std::string> args)`

## Hello World

```
#include "utf8_shim.hpp"

int utf8_main(std::vector<std::string> args)
{
	u8s::cout << u8"Hello Wörld" << u8s::newline;
	return 0;
}

```

Or alternatively if you don't want to use the provided implementation of main, it doesn't really do anything magic.


```
int main(int argc, char* argv[]) noexcept
{
    try {
        return utf8_main(u8s::get_utf8_args(argc, argv));
    }
    catch (std::exception& exc) {
        u8s::cerr << exc.what() << u8s::newline;
        return EXIT_FAILURE;
    }
}
```

If you don't want/require the minimal crash pad, you can call `std::vector<std::string> get_utf8_args(int, char* [])` directly. On Windows it does the console inititalization and UTF-16 to UTF-8 conversion of the arguments. On other platforms it just copies the arguments into a `vector` of `string` for easy/safe usage without dealing explicitly with zero-terminated strings.	