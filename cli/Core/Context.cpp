#include "Core/Context.h"

#include <meojson/json.hpp>

#include "Core/Path.h"
#include "Core/Validator.h"

namespace maa::cli
{

bool Context::load_config()
{
    auto value = json::open(config_path());
    if (!value.has_value()) {
        return false;
    }
    if (!config_.from_json(value.value())) {
        return false;
    }
    return true;
}

bool Context::load_projects()
{
    std::string err;

    for (const auto& proj : scan_project()) {
        ProjectInfo info;
        info.name = proj;
        auto value = json::open(project_path(proj));
        if (!value.has_value()) {
            std::cout << std::format("open project {} failed", proj) << std::endl;
            return false;
        }
        if (!info.project.check_json(value.value(), err)
            || !info.project.from_json(value.value())) {
            std::cout << std::format("parse project {} failed, error {}", proj, err) << std::endl;
            return false;
        }
        if (!validate(info.project, err)) {
            std::cout << err << std::endl;
            return false;
        }
        if (!refresh_project_config(info)) {
            return false;
        }
        projects_.push_back(std::move(info));
    }
    return true;
}

bool Context::refresh_project_config(ProjectInfo& info)
{
    std::string err;
    info.configs.clear();
    for (const auto& cfg : scan_project_config(info.name)) {
        auto cfg_value = json::open(project_config_path(info.name, cfg));
        if (!cfg_value.has_value()) {
            std::cout << std::format("open config {} of project {} failed", cfg, info.name)
                      << std::endl;
            return false;
        }
        ProjectConfig proj_cfg;
        if (!proj_cfg.check_json(cfg_value.value(), err)
            || !proj_cfg.from_json(cfg_value.value())) {
            std::cout << std::format(
                "parse config {} of project {} failed, error {}",
                cfg,
                info.name,
                err) << std::endl;
            return false;
        }
        if (!validate(info.project, proj_cfg, err)) {
            std::cout << err << std::endl;
            return false;
        }
        info.configs.push_back(std::move(proj_cfg));
    }
    return true;
}

}
