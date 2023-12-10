/*

Copyright 2023 Tim Seguine

This file is subject to the terms of the MIT License.
For details see the top-level LICENSE file.

*/
#pragma once


#include <iostream>
#include <string>
#include <vector>
#if __cpp_lib_string_view
#include <string_view>
#endif

// If using utf8_shim.cpp this is the entry-point of your program
extern int utf8_main(std::vector<std::string> args);

namespace u8s {

    inline std::ostream& newline(std::ostream& os) {
        return os.put('\n');
    }

    inline std::wostream& newline(std::wostream& os) {
        return os.put(L'\n');
    }

}

#if _WIN32

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <shellapi.h>
#include <io.h>
#include <fcntl.h>

#include <cassert>


namespace u8s {

    inline int utf8bytesRequired(wchar_t const* wstr, std::size_t size) noexcept {
        return WideCharToMultiByte(CP_UTF8, 0, wstr, static_cast<int>(size), nullptr, 0, nullptr, nullptr);
    }

    inline int utf16wcharsRequired(char const* str, std::size_t size) noexcept {
        return MultiByteToWideChar(CP_UTF8, 0, str, static_cast<int>(size), nullptr, 0);
    }

    inline std::string narrow(wchar_t const* wstr, std::size_t size) {
        auto const bytes_required = utf8bytesRequired(wstr, size);
        std::vector<char> str_data(static_cast<std::size_t>(bytes_required), 0);
        auto const bytes_written = WideCharToMultiByte(CP_UTF8, 0, wstr, static_cast<int>(size),
            str_data.data(), bytes_required, nullptr, nullptr);
        assert(bytes_written == bytes_required);
        return std::string(begin(str_data), end(str_data));
    }

    inline std::wstring widen(char const* str, std::size_t size) {
        auto const wchars_required = utf16wcharsRequired(str, size);
        std::vector<wchar_t> wstr_data(static_cast<std::size_t>(wchars_required), 0);
        auto const wchars_written = MultiByteToWideChar(CP_UTF8, 0, str, static_cast<int>(size),
            wstr_data.data(), wchars_required);
        assert(wchars_written == wchars_required);
        return std::wstring(begin(wstr_data), end(wstr_data));
    }

    inline void setmode_w(int fileno) noexcept {
        int setmode_value = _setmode(fileno, _O_WTEXT);
        assert(setmode_value != -1);
    }

    inline void set_utf8_console() noexcept {
        setmode_w(_fileno(stdin));
        setmode_w(_fileno(stdout));
        setmode_w(_fileno(stderr));
        std::ios_base::sync_with_stdio(false);
    }

    inline std::vector<std::string> get_utf8_args(int, char* []) {
        set_utf8_console();
        int argcW = 0;
        auto** argvW = CommandLineToArgvW(GetCommandLineW(), &argcW);

        std::vector<std::string> args;
        args.reserve(argcW);
        for (int i = 0; i < argcW; ++i) {
            args.emplace_back(narrow(argvW[i], wcslen(argvW[i])));
        }
        LocalFree(argvW);
        return args;
    }


    inline std::string narrow(std::wstring const& wstr) {
        return narrow(wstr.data(), wstr.size());
    }

    inline std::wstring widen(std::string const& str) {
        return widen(str.data(), str.size());
    }

}

inline std::wostream& operator<<(std::wostream& ws, std::string const& s) {
    return ws << u8s::widen(s);
}

#if __cpp_lib_string_view
inline std::wostream& operator<<(std::wostream& ws, std::string_view sv) {
    return ws << u8s::widen(sv.data(), sv.size());
}
#endif

inline std::wostream& operator<<(std::wostream& ws, char const* sz) {
    return ws << std::string(sz);
}

inline std::wostream& operator<<(std::wostream& ws, char8_t const* sz) {
    return ws << std::string(reinterpret_cast<char const*>(sz));
}


inline std::wistream& operator>>(std::wistream& ws, std::string& s) {
    std::wstring widestring;
    ws >> widestring;
    s = u8s::narrow(widestring);
    return ws;
}




#ifdef _MSC_VER
// The standard guarantees that these are initialized already by including <iostream> so this warning is a false positive
#pragma warning(disable: 26427)
#endif

namespace u8s {

    static auto& cout = std::wcout;
    static auto& cin = std::wcin;
    static auto& cerr = std::wcerr;
    static auto& clog = std::wclog;

}

#ifdef _MSC_VER
#pragma warning(default: 26427)
#endif

#else

namespace u8s {

    inline void set_utf8_console() noexcept {
        std::ios_base::sync_with_stdio(false);
    }

    inline std::vector<std::string> get_utf8_args(int argc, char* argv[]) {
        set_utf8_console();
        return std::vector<std::string>(argv, argv + argc);
    }

    static auto& cout = std::cout;
    static auto& cin = std::cin;
    static auto& cerr = std::cerr;
    static auto& clog = std::clog;

}

#endif
