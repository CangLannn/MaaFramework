#pragma once

#include <vector>

#include "Core/Type.h"

namespace maa::cli
{

class Context
{
public:
    struct ProjectInfo
    {
        Project project;
        std::vector<ProjectConfig> configs;
    };

    bool load_config();
    bool load_projects();

private:
    Config config_;
    std::vector<ProjectInfo> projects_;
};

}
