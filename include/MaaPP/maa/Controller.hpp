#pragma once

#include <iostream>
#include <string_view>
#include <utility>

#include <MaaFramework/MaaAPI.h>
#include <MaaFramework/MaaMsg.h>
#include <meojson/json.hpp>

#include "MaaPP/coro/EventLoop.hpp"
#include "MaaPP/coro/Promise.hpp"
#include "MaaPP/maa/AdbDevice.hpp"
#include "MaaPP/maa/details/ActionHelper.hpp"
#include "MaaPP/maa/details/String.hpp"

namespace maa
{

class Controller;

class ControllerAction : public details::ActionBase<ControllerAction, Controller>
{
    friend class Controller;

public:
    using ActionBase::ActionBase;

    MaaStatus status();

    coro::Promise<MaaStatus> wait() { return status_; }

private:
    coro::Promise<MaaStatus> status_;
};

class Controller : public details::ActionHelper<Controller, ControllerAction, MaaControllerHandle>
{
    friend class ControllerAction;
    friend class Instance;

public:
    struct adb_controller_tag
    {
    };

    Controller(
        [[maybe_unused]] adb_controller_tag tag,
        const std::string& adb_path,
        const std::string& address,
        MaaAdbControllerType type,
        const std::string& config,
        const std::string& agent_path)
        : ActionHelper(MaaAdbControllerCreateV2(
            adb_path.c_str(),
            address.c_str(),
            type,
            config.c_str(),
            agent_path.c_str(),
            &Controller::_callback,
            this))
    {
    }

    Controller(
        [[maybe_unused]] adb_controller_tag tag,
        const AdbDevice& device,
        const std::string& agent_path)
        : Controller(tag, device.adb_path, device.address, device.type, device.config, agent_path)
    {
    }

    ~Controller() { MaaControllerDestroy(inst_); }

    bool set_long_side(int width)
    {
        return MaaControllerSetOption(
            inst_,
            MaaCtrlOption_ScreenshotTargetLongSide,
            &width,
            sizeof(width));
    }

    bool set_short_side(int width)
    {
        return MaaControllerSetOption(
            inst_,
            MaaCtrlOption_ScreenshotTargetShortSide,
            &width,
            sizeof(width));
    }

    bool set_start_entry(std::string_view entry)
    {
        return MaaControllerSetOption(
            inst_,
            MaaCtrlOption_DefaultAppPackageEntry,
            const_cast<char*>(entry.data()),
            entry.size());
    }

    bool set_stop_entry(std::string_view entry)
    {
        return MaaControllerSetOption(
            inst_,
            MaaCtrlOption_DefaultAppPackage,
            const_cast<char*>(entry.data()),
            entry.size());
    }

    bool set_recording(bool enable)
    {
        return MaaControllerSetOption(inst_, MaaCtrlOption_Recording, &enable, sizeof(enable));
    }

    std::shared_ptr<ControllerAction> post_connect()
    {
        return put_action(MaaControllerPostConnection(inst_));
    }

    bool connected() { return MaaControllerConnected(inst_); }

    std::optional<std::string> uuid()
    {
        details::String buf;
        if (MaaControllerGetUUID(inst_, buf.handle())) {
            return buf;
        }
        else {
            return std::nullopt;
        }
    }

private:
    static void _callback(MaaStringView msg, MaaStringView details, MaaTransparentArg arg)
    {
        auto detail_opt = json::parse(details);
        if (!detail_opt.has_value() || !detail_opt.value().is_object()) {
            return;
        }
        // prevent destroy
        auto self = reinterpret_cast<Controller*>(arg)->shared_from_this();

        coro::EventLoop::current()->defer(
            [self, msg_str = std::string(msg), detail_val = std::move(detail_opt.value())]() {
                const auto& detail_obj = detail_val.as_object();
                if (!detail_obj.contains("id")) {
                    return;
                }
                MaaCtrlId id = detail_obj.at("id").as_unsigned_long_long();
                if (!self->actions_.contains(id)) {
                    std::cout << "cannot find id " << id << std::endl;
                    return;
                }
                auto ptr = self->actions_.at(id).lock();
                if (!ptr) {
                    std::cout << "action id " << id << " expired" << std::endl;
                    return;
                }
                if (msg_str == MaaMsg_Controller_Action_Completed) {
                    ptr->status_.resolve(MaaStatus_Success);
                }
                else if (msg_str == MaaMsg_Controller_Action_Failed) {
                    ptr->status_.resolve(MaaStatus_Failed);
                }
            });
    }
};

inline MaaStatus ControllerAction::status()
{
    return MaaControllerStatus(inst_->inst_, id_);
}

}