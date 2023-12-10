/*

Copyright 2023 Tim Seguine

This file is subject to the terms of the MIT License.
For details see the top-level LICENSE file.

*/
#include "utf8_shim.hpp"


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


