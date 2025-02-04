// SPDX-FileCopyrightText: Copyright 2019 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <memory>
#include "core/core.h"
#include "core/hle/service/glue/bgtc.h"
#include "core/hle/service/glue/ectx.h"
#include "core/hle/service/glue/glue.h"
#include "core/hle/service/glue/notif.h"
#include "core/hle/service/server_manager.h"

namespace Service::Glue {

void LoopProcess(Core::System& system) {
    auto server_manager = std::make_unique<ServerManager>(system);

    // BackGround Task Controller
    server_manager->RegisterNamedService("bgtc:t", std::make_shared<BGTC_T>(system));
    server_manager->RegisterNamedService("bgtc:sc", std::make_shared<BGTC_SC>(system));

    // Error Context
    server_manager->RegisterNamedService("ectx:aw", std::make_shared<ECTX_AW>(system));

    // Notification Services
    server_manager->RegisterNamedService(
        "notif:a", std::make_shared<INotificationServicesForApplication>(system));
    server_manager->RegisterNamedService("notif:s",
                                         std::make_shared<INotificationServices>(system));

    ServerManager::RunServer(std::move(server_manager));
}

} // namespace Service::Glue
