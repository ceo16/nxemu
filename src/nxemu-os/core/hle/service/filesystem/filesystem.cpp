// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <utility>

#include "yuzu_common/yuzu_assert.h"
#include "yuzu_common/fs/fs.h"
#include "yuzu_common/fs/path_util.h"
#include "yuzu_common/settings.h"
#include "core/core.h"
#include "core/hle/service/filesystem/filesystem.h"
#include "core/hle/service/filesystem/fsp/fsp_ldr.h"
#include "core/hle/service/filesystem/fsp/fsp_pr.h"
#include "core/hle/service/filesystem/fsp/fsp_srv.h"
#include "core/hle/service/filesystem/romfs_controller.h"
#include "core/hle/service/filesystem/save_data_controller.h"
#include "core/hle/service/server_manager.h"

namespace Service::FileSystem {

void LoopProcess(Core::System& system) {
    auto server_manager = std::make_unique<ServerManager>(system);

    const auto FileSystemProxyFactory = [&] { return std::make_shared<FSP_SRV>(system); };

    server_manager->RegisterNamedService("fsp-ldr", std::make_shared<FSP_LDR>(system));
    server_manager->RegisterNamedService("fsp:pr", std::make_shared<FSP_PR>(system));
    server_manager->RegisterNamedService("fsp-srv", std::move(FileSystemProxyFactory));
    ServerManager::RunServer(std::move(server_manager));
}

} // namespace Service::FileSystem
