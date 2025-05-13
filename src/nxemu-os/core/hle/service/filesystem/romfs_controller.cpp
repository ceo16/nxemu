// SPDX-FileCopyrightText: Copyright 2024 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "core/hle/service/filesystem/romfs_controller.h"

namespace Service::FileSystem {

RomFsController::RomFsController(RomFsControllerPtr&& factory_, uint64_t program_id_) :
    factory(std::move(factory_)),
    program_id(program_id_)
{
}

RomFsController::~RomFsController()
{
}

} // namespace Service::FileSystem
