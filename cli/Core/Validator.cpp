#include "Core/Validator.h"

#include <MaaPP/MaaPP.hpp>
#include <format>
#include <set>

namespace maa::cli
{

constexpr std::string_view win32_method[] = { "Find", "Search", "Cursor", "Desktop", "Foreground" };

template <typename Obj>
static inline bool check_duplicate_name(const std::vector<Obj>& value, std::string& dup_key)
{
    std::set<std::string> names;
    for (const auto& val : value) {
        if (names.contains(val.name)) {
            dup_key = val.name;
            return false;
        }
        names.insert(val.name);
    }
    return true;
}

bool validate(Project& proj, std::string& err)
{
    std::string key;

    if (!check_duplicate_name(proj.resource, key)) {
        err = std::format("duplicated resource name {}", key);
        return false;
    }
    for (const auto& [name, opt] : proj.option) {
        std::set<std::string> case_names;
        for (const auto& cs : opt.cases) {
            if (case_names.contains(cs.name)) {
                err = std::format("duplicated case name {} in option {}", cs.name, name);
                return false;
            }
            case_names.insert(cs.name);
        }
        if (!case_names.contains(opt.default_name)) {
            err =
                std::format("default case name {} in option {} not exists", opt.default_name, name);
            return false;
        }
    }
    if (!check_duplicate_name(proj.task, key)) {
        err = std::format("duplicated task name {}", key);
        return false;
    }
    for (const auto& tsk : proj.task) {
        for (const auto& opt : tsk.related_option) {
            if (!proj.option.contains(opt)) {
                err = std::format("related option {} in task {} not exists", opt, tsk.name);
                return false;
            }
        }
    }
    return true;
}

template <typename Type>
static inline bool check_controller_type_helper(const Type& known, const std::string& value)
{
    for (const auto& [name, _] : known) {
        if (name == value) {
            return true;
        }
    }
    return false;
}

template <typename Type, typename Obj>
static inline bool check_controller_type(const Obj& value, std::string& err)
{
    if (!value.touch.empty() && !check_controller_type_helper(Type::touch_index, value.touch)) {
        err = std::format("unknown touch type {}", value.touch);
        return false;
    }
    if (!value.key.empty() && !check_controller_type_helper(Type::key_index, value.key)) {
        err = std::format("unknown key type {}", value.key);
        return false;
    }
    if (!value.screencap.empty()
        && !check_controller_type_helper(Type::screencap_index, value.screencap)) {
        err = std::format("unknown screencap type {}", value.screencap);
        return false;
    }
    return true;
}

bool validate(const Project& proj, ProjectConfig& cfg, std::string& err)
{
    std::string sub_err;

    if (cfg.controller.type != "adb" && cfg.controller.type != "win32") {
        err = std::format(
            "unknown controller type {} in cfg {} project {}",
            cfg.controller.type,
            cfg.name,
            proj.name);
        return false;
    }
    if (!check_controller_type<AdbType>(cfg.controller.adb, sub_err)) {
        err = std::format("{} in cfg {} project {}", sub_err, cfg.name, proj.name);
        return false;
    }
    if (!check_controller_type<Win32Type>(cfg.controller.win32, sub_err)) {
        err = std::format("{} in cfg {} project {}", sub_err, cfg.name, proj.name);
        return false;
    }

    if (!cfg.controller.win32.method.empty()) {
        if (std::find(std::begin(win32_method), std::end(win32_method), cfg.controller.win32.method)
            == std::end(win32_method)) {
            err = std::format(
                "unknown win32 method {} in cfg {} project {}",
                cfg.controller.win32.method,
                cfg.name,
                proj.name);
            return false;
        }
    }

    if (std::find_if(
            proj.resource.begin(),
            proj.resource.end(),
            [&cfg](const auto& res) { return res.name == cfg.resource.name; })
        == proj.resource.end()) {
        err = std::format(
            "resource {} in cfg {} project {} not exist",
            cfg.resource.name,
            cfg.name,
            proj.name);
        return false;
    }

    for (const auto& tsk : cfg.task) {
        if (std::find_if(
                proj.task.begin(),
                proj.task.end(),
                [&tsk](const auto& t) { return t.name == tsk.name; })
            == proj.task.end()) {
            err = std::format(
                "task {} in cfg {} project {} not exist",
                tsk.name,
                cfg.name,
                proj.name);
            return false;
        }
        for (const auto& opt : tsk.option) {
            if (!proj.option.contains(opt.name)) {
                err = std::format(
                    "option {} in task {} cfg {} project {} not exist",
                    opt.name,
                    tsk.name,
                    cfg.name,
                    proj.name);
                return false;
            }
        }
    }

    return true;
}

}
