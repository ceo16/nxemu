// SPDX-FileCopyrightText: Copyright 2024 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <nxemu-module-spec/system_loader.h>
#include "core/file_sys/filesystem_interfaces.h"

namespace Service::FileSystem {

class SaveDataController {
public:
    explicit SaveDataController(Core::System& system, SaveDataFactoryPtr && factory_);
    ~SaveDataController();
    void SetAutoCreate(bool state);

private:
    Core::System& system;
    SaveDataFactoryPtr factory;
};

} // namespace Service::FileSystem
