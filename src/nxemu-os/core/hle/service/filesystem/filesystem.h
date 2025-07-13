// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>
#include <map>
#include <mutex>
#include "yuzu_common/common_types.h"
#include "core/hle/result.h"
#include "core/file_sys/filesystem_interfaces.h"
#include <core/file_sys/fs_filesystem.h>
#include <nxemu-module-spec/system_loader.h>

enum class LoaderContentRecordType : u8;

namespace Core {
class System;
}

namespace FileSys {

enum class BisPartitionId : u32;
enum class StorageId : u8;

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

// A class that wraps a VfsDirectory with methods that return ResultVal and Result instead of
// pointers and booleans. This makes using a VfsDirectory with switch services much easier and
// avoids repetitive code.
class VfsDirectoryServiceWrapper {
public:
    explicit VfsDirectoryServiceWrapper(IVirtualDirectoryPtr && backing);
    ~VfsDirectoryServiceWrapper();

    /**
     * Get a descriptive name for the archive (e.g. "RomFS", "SaveData", etc.)
     */
    std::string GetName() const;

    /**
     * Create a file specified by its path
     * @param path Path relative to the Archive
     * @param size The size of the new file, filled with zeroes
     * @return Result of the operation
     */
    Result CreateFile(const std::string& path, u64 size) const;

    /**
     * Delete a file specified by its path
     * @param path Path relative to the archive
     * @return Result of the operation
     */
    Result DeleteFile(const std::string& path) const;

    /**
     * Create a directory specified by its path
     * @param path Path relative to the archive
     * @return Result of the operation
     */
    Result CreateDirectory(const std::string& path) const;

    /**
     * Delete a directory specified by its path
     * @param path Path relative to the archive
     * @return Result of the operation
     */
    Result DeleteDirectory(const std::string& path) const;

    /**
     * Delete a directory specified by its path and anything under it
     * @param path Path relative to the archive
     * @return Result of the operation
     */
    Result DeleteDirectoryRecursively(const std::string& path) const;

    /**
     * Cleans the specified directory. This is similar to DeleteDirectoryRecursively,
     * in that it deletes all the contents of the specified directory, however, this
     * function does *not* delete the directory itself. It only deletes everything
     * within it.
     *
     * @param path Path relative to the archive.
     *
     * @return Result of the operation.
     */
    Result CleanDirectoryRecursively(const std::string& path) const;

    /**
     * Rename a File specified by its path
     * @param src_path Source path relative to the archive
     * @param dest_path Destination path relative to the archive
     * @return Result of the operation
     */
    Result RenameFile(const std::string& src_path, const std::string& dest_path) const;

    /**
     * Rename a Directory specified by its path
     * @param src_path Source path relative to the archive
     * @param dest_path Destination path relative to the archive
     * @return Result of the operation
     */
    Result RenameDirectory(const std::string& src_path, const std::string& dest_path) const;

    /**
     * Open a file specified by its path, using the specified mode
     * @param path Path relative to the archive
     * @param mode Mode to open the file with
     * @return Opened file, or error code
     */
    Result OpenFile(IVirtualFile ** out_file, const std::string & path, VirtualFileOpenMode mode) const;

    /**
     * Get the type of the specified path
     * @return The type of the specified path or error code
     */
    Result GetEntryType(FileSys::DirectoryEntryType* out_entry_type, const std::string& path) const;

private:
    IVirtualDirectoryPtr backing;
};

} // namespace FileSystem
} // namespace Service
