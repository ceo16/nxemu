// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>
#include <map>
#include <mutex>
#include "yuzu_common/common_types.h"
#include "core/hle/result.h"

enum class LoaderContentRecordType : u8;

namespace Core {
class System;
}

namespace FileSys {

enum class BisPartitionId : u32;
enum class SaveDataSpaceId : u8;
enum class SaveDataType : u8;
enum class StorageId : u8;

struct SaveDataAttribute;
struct SaveDataSize;
} // namespace FileSys

namespace Service {

namespace SM {
class ServiceManager;
} // namespace SM

namespace FileSystem {

class RomFsController;
class SaveDataController;

enum class ContentStorageId : u32 {
    System,
    User,
    SdCard,
};

enum class ImageDirectoryId : u32 {
    NAND,
    SdCard,
};

using ProcessId = u64;
using ProgramId = u64;

void LoopProcess(Core::System& system);

} // namespace FileSystem
} // namespace Service
