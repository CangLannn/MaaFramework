#include <MaaPP/MaaPP.hpp>
#include <iostream>

#include "UI/Menu.h"
#include "Utils/Misc.h"

using namespace maa;

coro::Promise<int> async_main()
{
    set_stdout_level(MaaLoggingLevel_Error);

    init(cli::program_directory().string());

    co_await cli::ui::menu(
        "hello",
        {
            { "choice1",
              [](auto) {
                  std::cout << "Choose 1" << std::endl;
              } },
            { "choice2",
              [](auto) {
                  std::cout << "Choose 2" << std::endl;
              } },
        });

    co_return 0;
}

int main()
{
    coro::EventLoop ev;

    ev.stop_after(async_main());

    return ev.exec();
}
