// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <cstring>

#include "yuzu_common/yuzu_assert.h"
#include "core/core.h"
#include "core/hle/kernel/k_event.h"
#include "core/hle/service/am/am.h"
#include "core/hle/service/am/applet_data_broker.h"
#include "core/hle/service/am/applet_manager.h"
#include "core/hle/service/am/frontend/applet_cabinet.h"
#include "core/hle/service/am/frontend/applet_controller.h"
#include "core/hle/service/am/frontend/applet_error.h"
#include "core/hle/service/am/frontend/applet_general.h"
#include "core/hle/service/am/frontend/applet_profile_select.h"
#include "core/hle/service/am/frontend/applet_web_browser.h"
#include "core/hle/service/am/frontend/applets.h"
#include "core/hle/service/am/service/storage.h"
#include "core/hle/service/sm/sm.h"

namespace Service::AM::Frontend {

FrontendApplet::FrontendApplet(Core::System& system_, std::shared_ptr<Applet> applet_,
                               LibraryAppletMode applet_mode_)
    : system{system_}, applet{std::move(applet_)}, applet_mode{applet_mode_} {}

FrontendApplet::~FrontendApplet() = default;

void FrontendApplet::Initialize() {
    std::shared_ptr<IStorage> common = PopInData();
    ASSERT(common != nullptr);
    const auto common_data = common->GetData();

    ASSERT(common_data.size() >= sizeof(CommonArguments));
    std::memcpy(&common_args, common_data.data(), sizeof(CommonArguments));

    initialized = true;
}

std::shared_ptr<IStorage> FrontendApplet::PopInData() {
    std::shared_ptr<IStorage> ret;
    applet.lock()->caller_applet_broker->GetInData().Pop(&ret);
    return ret;
}

std::shared_ptr<IStorage> FrontendApplet::PopInteractiveInData() {
    std::shared_ptr<IStorage> ret;
    applet.lock()->caller_applet_broker->GetInteractiveInData().Pop(&ret);
    return ret;
}

void FrontendApplet::PushOutData(std::shared_ptr<IStorage> storage) {
    applet.lock()->caller_applet_broker->GetOutData().Push(storage);
}

void FrontendApplet::PushInteractiveOutData(std::shared_ptr<IStorage> storage) {
    applet.lock()->caller_applet_broker->GetInteractiveOutData().Push(storage);
}

void FrontendApplet::Exit() {
    applet.lock()->caller_applet_broker->SignalCompletion();
}

FrontendAppletSet::FrontendAppletSet() = default;

FrontendAppletSet::~FrontendAppletSet() = default;

FrontendAppletSet::FrontendAppletSet(FrontendAppletSet&&) noexcept = default;

FrontendAppletSet& FrontendAppletSet::operator=(FrontendAppletSet&&) noexcept = default;

FrontendAppletHolder::FrontendAppletHolder(Core::System& system_) : system{system_} {}

FrontendAppletHolder::~FrontendAppletHolder() = default;

const FrontendAppletSet& FrontendAppletHolder::GetFrontendAppletSet() const {
    return frontend;
}

NFP::CabinetMode FrontendAppletHolder::GetCabinetMode() const {
    return cabinet_mode;
}

AppletId FrontendAppletHolder::GetCurrentAppletId() const {
    return current_applet_id;
}

void FrontendAppletHolder::SetFrontendAppletSet(FrontendAppletSet set) {
    UNIMPLEMENTED();
}

void FrontendAppletHolder::SetCabinetMode(NFP::CabinetMode mode) {
    cabinet_mode = mode;
}

void FrontendAppletHolder::SetCurrentAppletId(AppletId applet_id) {
    current_applet_id = applet_id;
}

void FrontendAppletHolder::SetDefaultAppletsIfMissing() {
}

void FrontendAppletHolder::ClearAll() {
    frontend = {};
}

std::shared_ptr<FrontendApplet> FrontendAppletHolder::GetApplet(std::shared_ptr<Applet> applet,
                                                                AppletId id,
                                                                LibraryAppletMode mode) const {
    UNIMPLEMENTED_MSG(
        "No backend implementation exists for applet_id={:02X}! Falling back to stub applet.",
        static_cast<u8>(id));
    return std::make_shared<StubApplet>(system, applet, id, mode);
}

} // namespace Service::AM::Frontend
