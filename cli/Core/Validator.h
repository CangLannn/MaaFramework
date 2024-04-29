#pragma once

#include "Core/Type.h"

namespace maa::cli
{

bool validate(Project& proj, std::string& err);
bool validate(const Project& proj, ProjectConfig& cfg, std::string& err);

}
