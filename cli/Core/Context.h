#pragma once

#include <vector>

#include "Core/Type.h"

namespace maa::cli
{

class Context
{
    friend class Interactor;

public:
    struct ProjectInfo
    {
        std::string name; // name of json
        Project project;
        std::vector<ProjectConfig> configs;
    };

    bool load_config();
    bool load_projects();
    bool refresh_project_config(ProjectInfo& info);

private:
    Config config_;
    std::vector<ProjectInfo> projects_;
};

}
