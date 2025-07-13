// SPDX-FileCopyrightText: Copyright 2024 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <nxemu-module-spec/system_loader.h>
#include "core/file_sys/filesystem_interfaces.h"
#include "core/file_sys/fs_save_data_types.h"

namespace Service::FileSystem {

class SaveDataController {
public:
    explicit SaveDataController(Core::System& system, SaveDataFactoryPtr && factory_);
    ~SaveDataController();

    Result OpenSaveData(IVirtualDirectoryPtr & out_save_data, SaveDataSpaceId space, const SaveDataAttribute & attribute);
    void SetAutoCreate(bool state);

private:
    Core::System& system;
    SaveDataFactoryPtr factory;
};

} // namespace Service::FileSystem
