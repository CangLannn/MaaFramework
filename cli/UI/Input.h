#pragma once

#include <string>

#include <MaaPP/MaaPP.hpp>

namespace maa::cli::ui
{

coro::Promise<int> input_raw();
coro::Promise<long> input_number();
coro::Promise<std::string> input_string();

}
