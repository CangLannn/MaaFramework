#pragma once

#include <format>
#include <iostream>
#include <string_view>
#include <tuple>

namespace maa::cli::ui
{

void enter_raw();
void leave_raw();

namespace cursor
{

inline void move(size_t row, size_t col)
{
    std::cout << std::format("\x1b[{};{}H", row + 1, col + 1) << std::flush;
}

inline void up(size_t row)
{
    std::cout << std::format("\x1b[{}A", row) << std::flush;
}

inline void down(size_t row)
{
    std::cout << std::format("\x1b[{}B", row) << std::flush;
}

inline void right(size_t col)
{
    std::cout << std::format("\x1b[{}C", col) << std::flush;
}

inline void left(size_t col)
{
    std::cout << std::format("\x1b[{}D", col) << std::flush;
}

inline void down_left(size_t row)
{
    std::cout << std::format("\x1b[{}E", row) << std::flush;
}

inline void up_left(size_t row)
{
    std::cout << std::format("\x1b[{}F", row) << std::flush;
}

inline void to_col(size_t col)
{
    std::cout << std::format("\x1b[{}G", col + 1) << std::flush;
}

static inline std::vector<std::tuple<size_t, size_t>> pos;

inline std::tuple<size_t, size_t> query();

inline void save()
{
    pos.push_back(query());
}

inline void restore()
{
    auto p = pos.back();
    pos.pop_back();
    move(std::get<0>(p), std::get<1>(p));
}

inline void hide()
{
    std::cout << "\x1b[?25l" << std::flush;
}

inline void show()
{
    std::cout << "\x1b[?25h" << std::flush;
}

}

namespace erase
{

inline void after()
{
    std::cout << "\x1b[0J" << std::flush;
}

inline void before()
{
    std::cout << "\x1b[1J" << std::flush;
}

inline void screen()
{
    std::cout << "\x1b[2J" << std::flush;
}

inline void saved()
{
    std::cout << "\x1b[3J" << std::flush;
}

inline void line_after()
{
    std::cout << "\x1b[0K" << std::flush;
}

inline void line_before()
{
    std::cout << "\x1b[1K" << std::flush;
}

inline void line()
{
    std::cout << "\x1b[2K" << std::flush;
}

}

template <void (*enter)(), void (*leave)()>
struct helper
{
    bool released_ = false;

    helper() { enter(); }

    ~helper() { release(); }

    void release()
    {
        if (!released_) {
            released_ = true;
            leave();
        }
    }
};

using raw_helper = helper<enter_raw, leave_raw>;
using cursor_save_helper = helper<cursor::save, cursor::restore>;
using cursor_hide_helper = helper<cursor::hide, cursor::show>;

namespace cursor
{

inline std::tuple<size_t, size_t> query()
{
    raw_helper __raw;
    std::cout << "\x1b[6n" << std::flush;
    char ch;
    std::cin >> ch;
    std::cin >> ch;
    size_t row, col;
    std::cin >> row;
    std::cin >> ch;
    std::cin >> col;
    std::cin >> ch;
    return std::make_tuple(row - 1, col - 1);
}

}

}
