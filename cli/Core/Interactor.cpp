#include "Core/Interactor.h"

#include <filesystem>

#include <MaaPP/MaaPP.hpp>
#include <meojson/json.hpp>

#include "Core/Path.h"
#include "Core/Type.h"
#include "UI/Input.h"
#include "UI/Menu.h"
#include "UI/Term.h"

namespace maa::cli
{

coro::Promise<void> Interactor::main()
{
    std::vector<ui::MenuEntry> project_entries;
    for (auto& proj : context_->projects_) {
        project_entries.push_back(
            { proj.project.name, [&proj, self = shared_from_this()](auto) -> coro::Promise<> {
                 std::cout << "select project " << proj.name << std::endl;
                 co_await self->on_project(proj);
             } });
    }
    if (project_entries.empty()) {
        std::cout << "no project found!" << std::endl;
        co_return;
    }
    ui::cursor::move(0, 0);
    co_await ui::menu("select project", project_entries);
}

coro::Promise<void> Interactor::on_project(Context::ProjectInfo& info)
{
    ui::cursor_save_helper __save;
    std::vector<ui::MenuEntry> project_config_entries;
    for (auto& cfg : info.configs) {
        project_config_entries.push_back(
            { cfg.name, [&info, &cfg, self = shared_from_this()](auto) -> coro::Promise<> {
                 std::cout << "select config " << cfg.name << std::endl;
                 co_await self->on_project_config(info, cfg);
             } });
    }
    if (project_config_entries.empty()) {
        std::cout << "no config found!" << std::endl;
        co_await on_project_create_config(info);
        context_->refresh_project_config(info);
        __save.release();
        co_await on_project(info);
        co_return;
    }
    co_await ui::menu("select config", project_config_entries);
    co_return;
}

coro::Promise<void> Interactor::on_project_create_config(Context::ProjectInfo& info)
{
    auto config = std::make_shared<ProjectConfig>();
    std::cout << "create new config, name: " << std::flush;
    auto name = co_await ui::input_string();
    auto path = project_config_path(info.name, name);
    if (std::filesystem::exists(path)) {
        std::cout << std::format("config {} exists", name) << std::endl;
        co_await on_project_create_config(info);
        co_return;
    }

    config->name = name;

    auto controller_type_choice = co_await ui::menu(
        "choose controller type",
        {
            { "Adb", nullptr },
            { "Win32", nullptr },
        });
    if (controller_type_choice == 0) {
        config->controller.type = "adb";
    }
    else {
        config->controller.type = "win32";
    }

    std::vector<ui::MenuEntry> resource_entries;
    for (const auto& res : info.project.resource) {
        resource_entries.push_back({ res.name, nullptr });
    }
    auto resource_choice = co_await ui::menu("select resource", resource_entries);
    config->resource.name = info.project.resource[resource_choice].name;

    if (!std::filesystem::exists(path.parent_path())) {
        std::filesystem::create_directory(path.parent_path());
    }
    std::ofstream cfg_file(path);
    cfg_file << config->to_json().format() << std::endl;
    co_return;
}

coro::Promise<void> Interactor::on_project_config(Context::ProjectInfo& info, ProjectConfig& config)
{
    std::ignore = config;
    std::cout << std::format("run config {} of project {}", config.name, info.project.name)
              << std::endl;
    co_return;
}

}
