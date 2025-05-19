// SPDX-FileCopyrightText: Copyright 2024 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include "core/file_sys/filesystem_interfaces.h"

namespace Service::FileSystem {

class RomFsController {
public:
    explicit RomFsController(RomFsControllerPtr && factory_, uint64_t program_id_);
    ~RomFsController();

    IVirtualFilePtr OpenRomFSCurrentProcess();

private:
    const RomFsControllerPtr factory;
    const uint64_t program_id;
};

} // namespace Service::FileSystem
