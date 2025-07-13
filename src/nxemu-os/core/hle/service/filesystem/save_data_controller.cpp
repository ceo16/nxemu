// SPDX-FileCopyrightText: Copyright 2024 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "core/core.h"
#include "core/hle/result.h"
#include "core/hle/service/filesystem/save_data_controller.h"
#include "core/file_sys/errors.h"
#include "yuzu_common/yuzu_assert.h"
#include <utility>

namespace Service::FileSystem {

namespace {

// A default size for normal/journal save data size if application control metadata cannot be found.
// This should be large enough to satisfy even the most extreme requirements (~4.2GB)
constexpr u64 SufficientSaveDataSize = 0xF0000000;

} // namespace

SaveDataController::SaveDataController(Core::System& system_, SaveDataFactoryPtr && factory_)
    : system{ system_ }, factory{ std::move(factory_) }
{
}

SaveDataController::~SaveDataController() = default;

Result SaveDataController::OpenSaveData(IVirtualDirectoryPtr & out_save_data, SaveDataSpaceId space, const SaveDataAttribute& attribute)
{
    out_save_data = factory->Open(space, attribute);
    if (!out_save_data) 
    {
        return FileSys::ResultTargetNotFound;
    }
    return ResultSuccess;
}

void SaveDataController::SetAutoCreate(bool state) {
    UNIMPLEMENTED();
}

} // namespace Service::FileSystem
