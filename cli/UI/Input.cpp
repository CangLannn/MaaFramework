#include "UI/Input.h"

#include <iostream>

namespace maa::cli::ui
{

coro::Promise<long> input_number()
{
    return coro::EventLoop::current()->eval([]() {
        char ch;
        std::cin >> ch;
        std::cin.putback(ch);

        std::string row;
        std::getline(std::cin, row);

        try {
            return std::stol(row);
        }
        catch (...) {
            return -1l;
        }
    });
}

coro::Promise<std::string> input_string()
{
    return coro::EventLoop::current()->eval([]() {
        char ch;
        std::cin >> ch;
        std::cin.putback(ch);

        std::string row;
        std::getline(std::cin, row);

        if (row.length() > 0 && row.back() == '\n') {
            row.pop_back();
        }
        return row;
    });
}

}
