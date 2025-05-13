// SPDX-FileCopyrightText: Copyright 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "yuzu_common/common_funcs.h"
#include "core/file_sys/fsa/fs_i_filesystem.h"
#include "core/hle/service/cmif_types.h"
#include "core/hle/service/filesystem/filesystem.h"
#include "core/hle/service/filesystem/fsp/fsp_types.h"
#include "core/hle/service/service.h"

namespace FileSys::Sf {
struct Path;
}

namespace Service::FileSystem {

class IFile;
class IDirectory;

class IFileSystem final : public ServiceFramework<IFileSystem> {
public:
    Result Commit();

private:
    std::unique_ptr<FileSys::Fsa::IFileSystem> backend;
};

} // namespace Service::FileSystem
