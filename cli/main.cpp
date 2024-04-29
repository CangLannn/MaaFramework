#include <MaaPP/MaaPP.hpp>
#include <iostream>
#include <memory>

#include "Core/Context.h"
#include "Core/Interactor.h"
#include "UI/Menu.h"
#include "Utils/Misc.h"

using namespace maa;

coro::Promise<int> async_main()
{
    set_stdout_level(MaaLoggingLevel_Error);

    init(cli::program_directory().string());

    // co_await cli::ui::menu(
    //     "hello",
    //     {
    //         { "choice1",
    //           [](auto) {
    //               std::cout << "Choose 1" << std::endl;
    //           } },
    //         { "choice2",
    //           [](auto) {
    //               std::cout << "Choose 2" << std::endl;
    //           } },
    //     });

    auto context = std::make_shared<cli::Context>();
    context->load_config();
    context->load_projects();

    auto interactor = std::make_shared<cli::Interactor>(context);

    co_await interactor->main();

    co_return 0;
}

int main()
{
    coro::EventLoop ev;

    ev.stop_after(async_main());

    return ev.exec();
}
