#pragma once

#include <filesystem>
#include <format>
#include <vector>

#include "Utils/Misc.h"

namespace maa::cli
{

inline std::filesystem::path config_path()
{
    return project_directory() / "config.json";
}

inline std::filesystem::path project_path(std::string project)
{
    return project_directory() / "project" / std::format("{}.json", project);
}

inline std::filesystem::path project_config_path(std::string project, std::string config)
{
    return project_directory() / "project" / project / std::format("{}.json", config);
}

inline std::vector<std::string> scan_project()
{
    std::vector<std::string> result;
    if (!std::filesystem::is_directory(project_directory() / "project")) {
        return {};
    }
    for (const auto& entry : std::filesystem::directory_iterator(project_directory() / "project")) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            result.push_back(entry.path().stem().string());
        }
    }
    return result;
}

inline std::vector<std::string> scan_project_config(std::string project)
{
    std::vector<std::string> result;
    if (!std::filesystem::is_directory(project_directory() / "project" / project)) {
        return {};
    }
    for (const auto& entry :
         std::filesystem::directory_iterator(project_directory() / "project" / project)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            result.push_back(entry.path().stem().string());
        }
    }
    return result;
}

}
