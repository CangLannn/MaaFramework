#pragma once

#include <functional>
#include <string>
#include <variant>
#include <vector>

#include <MaaPP/MaaPP.hpp>

namespace maa::cli::ui
{

struct MenuEntry
{
    std::string text;
    std::function<coro::Promise<>(size_t entry)> action;
};

coro::Promise<size_t> menu(std::string title, std::vector<MenuEntry> entries);

}
