#pragma once

#include <filesystem>
#include <meojson/json.hpp>

#include "Conf/Conf.h"

MAA_DEBUG_NS_BEGIN

struct Source
{
    std::filesystem::path file;
    json::location::position pos;
};

struct Task
{
    struct Trace
    {
        json::value value;
        std::optional<Source> source;
    };

    std::vector<Trace> trace;
};

struct Json
{
    std::filesystem::path file;
    json::location::location_info<std::string> location;
};

MAA_DEBUG_NS_END
