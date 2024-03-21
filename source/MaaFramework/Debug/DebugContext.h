#pragma once

#include "API/MaaTypes.h"
#include "Conf/Conf.h"
#include "Instance/InstanceInternalAPI.hpp"
#include "Task/PipelineTask.h"

MAA_DEBUG_NS_BEGIN

class DebugContext : public MaaDebugContextAPI
{
public:
    DebugContext(TaskNS::PipelineTask* task);

    virtual std::vector<std::string> get_task_list() const override;
    virtual const MAA_DEBUG_NS::Task* get_task_debug_info(std::string_view task) const override;

private:
    TaskNS::PipelineTask* task_;
};

MAA_DEBUG_NS_END
