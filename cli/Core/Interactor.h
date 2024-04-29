#pragma once

#include <MaaPP/MaaPP.hpp>
#include <memory>

#include "Core/Context.h"
#include "Core/Type.h"

namespace maa::cli
{

class Interactor : public std::enable_shared_from_this<Interactor>
{
public:
    Interactor(std::shared_ptr<Context> context)
        : context_(context)
    {
    }

    coro::Promise<void> main();
    coro::Promise<void> on_project(Context::ProjectInfo& info);
    coro::Promise<void> on_project_create_config(Context::ProjectInfo& info);
    coro::Promise<void> on_project_config(Context::ProjectInfo& info, ProjectConfig& config);

private:
    std::shared_ptr<Context> context_;
};

}
