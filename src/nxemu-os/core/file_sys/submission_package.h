// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <map>
#include <memory>
#include <set>
#include <vector>
#include "yuzu_common/common_types.h"
#include "core/file_sys/nca_metadata.h"
#include "core/file_sys/vfs/vfs.h"

namespace Core::Crypto {
class KeyManager;
}

namespace Loader {
enum class ResultStatus : u16;
}

namespace FileSys {

class PartitionFilesystem;

enum class ContentRecordType : u8;

} // namespace FileSys
