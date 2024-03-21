#include "DebugContext.h"

MAA_DEBUG_NS_BEGIN

DebugContext::DebugContext(TaskNS::PipelineTask* task)
    : task_(task)
{
}

std::vector<std::string> DebugContext::get_task_list() const
{
    return task_->data_mgr_.get_task_list();
}

MAA_DEBUG_NS_END
