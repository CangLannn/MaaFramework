#pragma once

#include "meojson/reflection/jsonization.hpp"
#include <MaaPP/MaaPP.hpp>
#include <unordered_map>

#include <meojson/json.hpp>

namespace maa::cli
{

// represent a project, like predefined options and entries.
struct Project
{
    struct Resource
    {
        std::string name;
        std::vector<std::string> path;

        MEO_JSONIZATION(name, path);
    };

    struct CaseOption
    {
        struct Case
        {
            std::string name;
            json::object param;

            MEO_JSONIZATION(name, param);
        };

        std::vector<Case> cases;
        std::string default_name;

        MEO_JSONIZATION(cases, default_name);
    };

    // struct InjectOptionEntry
    // {
    //     enum Type
    //     {
    //         Number,
    //         String
    //     };

    //     std::vector<std::vector<std::string>> path;
    // };

    struct Task
    {
        std::string name;
        std::string task_entry;
        json::object task_param;
        std::vector<std::string> related_option;

        MEO_JSONIZATION(name, task_entry, MEO_OPT task_param, MEO_OPT related_option);
    };

    struct Executor
    {
        std::string exec_path;
        std::vector<std::string> exec_param;

        MEO_JSONIZATION(exec_path, exec_param);
    };

    std::string name;
    std::vector<Resource> resource;
    std::vector<Task> task;
    std::unordered_map<std::string, CaseOption> option;
    std::unordered_map<std::string, Executor> recognizer;
    std::unordered_map<std::string, Executor> action;

    MEO_JSONIZATION(name, resource, task, MEO_OPT option, MEO_OPT recognizer, MEO_OPT action);
};

// represent config for a project, like option values and entry chains.
struct ProjectConfig
{
    struct Controller
    {
        struct Adb
        {
            std::string adb;
            std::string address;

            std::string touch;
            std::string key;
            std::string screencap;

            json::object config;

            MEO_JSONIZATION(
                adb,
                address,
                MEO_OPT touch,
                MEO_OPT key,
                MEO_OPT screencap,
                MEO_OPT config)
        };

        struct Win32
        {
            std::string method;
            std::string class_name;
            std::string window_name;

            std::string touch;
            std::string key;
            std::string screencap;

            MEO_JSONIZATION(
                method,
                MEO_OPT class_name,
                MEO_OPT window_name,
                MEO_OPT touch,
                MEO_OPT key,
                MEO_OPT screencap)
        };

        std::string name;
        std::string type;

        Adb adb;
        Win32 win32;

        MEO_JSONIZATION(name, type, MEO_OPT adb, MEO_OPT win32)
    };

    struct Resource
    {
        std::string name;

        MEO_JSONIZATION(name)
    };

    struct Option
    {
        std::string name;
        std::string value;

        MEO_JSONIZATION(name, value)
    };

    struct Task
    {
        std::string name;
        std::vector<Option> option;

        MEO_JSONIZATION(name, MEO_OPT option)
    };

    std::string name;
    Controller controller;
    Resource resource;
    std::vector<Task> task;

    MEO_JSONIZATION(name, controller, resource, task)
};

// represent a instance of a project. only runtime.
struct ProjectInstance
{
    ControllerHandle controller;
    ResourceHandle resource;
    InstanceHandle instance;
};

// represent cli config
struct Config
{
    bool placeholder;

    MEO_JSONIZATION(MEO_OPT placeholder)
};

}
