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

enum class LoaderContentRecordType : u8;
enum class LoaderResultStatus : uint16_t;

namespace FileSys {

class PartitionFilesystem;

} // namespace FileSys
