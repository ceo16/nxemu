// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <utility>

#include "yuzu_common/yuzu_assert.h"
#include "yuzu_common/fs/fs.h"
#include "yuzu_common/fs/path_util.h"
#include "yuzu_common/settings.h"
#include "core/core.h"
#include "core/file_sys/control_metadata.h"
#include "core/file_sys/registered_cache.h"
#include "core/file_sys/vfs/vfs.h"
#include "core/hle/service/filesystem/filesystem.h"
#include "core/hle/service/filesystem/fsp/fsp_ldr.h"
#include "core/hle/service/filesystem/fsp/fsp_pr.h"
#include "core/hle/service/filesystem/fsp/fsp_srv.h"
#include "core/hle/service/server_manager.h"
#include "core/loader/loader.h"

namespace Service::FileSystem {

static FileSys::VirtualDir GetDirectoryRelativeWrapped(FileSys::VirtualDir base,
                                                       std::string_view dir_name_) {
    std::string dir_name(Common::FS::SanitizePath(dir_name_));
    if (dir_name.empty() || dir_name == "." || dir_name == "/" || dir_name == "\\")
        return base;

    return base->GetDirectoryRelative(dir_name);
}

VfsDirectoryServiceWrapper::VfsDirectoryServiceWrapper(FileSys::VirtualDir backing_)
    : backing(std::move(backing_)) {}

VfsDirectoryServiceWrapper::~VfsDirectoryServiceWrapper() = default;

std::string VfsDirectoryServiceWrapper::GetName() const {
    return backing->GetName();
}

Result VfsDirectoryServiceWrapper::CreateFile(const std::string& path_, u64 size) const {
    UNIMPLEMENTED();
    return ResultSuccess;
}

Result VfsDirectoryServiceWrapper::DeleteFile(const std::string& path_) const {
    UNIMPLEMENTED();
    return ResultSuccess;
}

Result VfsDirectoryServiceWrapper::CreateDirectory(const std::string& path_) const {
    UNIMPLEMENTED();
    return ResultSuccess;
}

Result VfsDirectoryServiceWrapper::DeleteDirectory(const std::string& path_) const {
    UNIMPLEMENTED();
    return ResultSuccess;
}

Result VfsDirectoryServiceWrapper::DeleteDirectoryRecursively(const std::string& path_) const {
    UNIMPLEMENTED();
    return ResultSuccess;
}

Result VfsDirectoryServiceWrapper::CleanDirectoryRecursively(const std::string& path) const {
    UNIMPLEMENTED();
    return ResultSuccess;
}

Result VfsDirectoryServiceWrapper::RenameFile(const std::string& src_path_,
                                              const std::string& dest_path_) const {
    UNIMPLEMENTED();
    return ResultSuccess;
}

Result VfsDirectoryServiceWrapper::RenameDirectory(const std::string& src_path_,
                                                   const std::string& dest_path_) const {
    UNIMPLEMENTED();
    return ResultUnknown;
}

Result VfsDirectoryServiceWrapper::OpenFile(FileSys::VirtualFile* out_file,
                                            const std::string& path_,
                                            FileSys::OpenMode mode) const {
    UNIMPLEMENTED();
    return ResultSuccess;
}

Result VfsDirectoryServiceWrapper::OpenDirectory(FileSys::VirtualDir* out_directory,
                                                 const std::string& path_) {
    UNIMPLEMENTED();
    return ResultSuccess;
}

Result VfsDirectoryServiceWrapper::GetEntryType(FileSys::DirectoryEntryType* out_entry_type,
                                                const std::string& path_) const {
    UNIMPLEMENTED();
    return ResultSuccess;
}

Result VfsDirectoryServiceWrapper::GetFileTimeStampRaw(
    FileSys::FileTimeStampRaw* out_file_time_stamp_raw, const std::string& path) const {
    UNIMPLEMENTED();
    return ResultSuccess;
}

FileSystemController::FileSystemController(Core::System& system_) : system{system_} {}

FileSystemController::~FileSystemController() = default;

u64 FileSystemController::GetFreeSpaceSize(FileSys::StorageId id) const {
    UNIMPLEMENTED();
    return 0;
}

u64 FileSystemController::GetTotalSpaceSize(FileSys::StorageId id) const {
    UNIMPLEMENTED();
    return 0;
}

void FileSystemController::SetGameCard(FileSys::VirtualFile file) {
    UNIMPLEMENTED();
}


void FileSystemController::CreateFactories(FileSys::VfsFilesystem& vfs, bool overwrite) {
    UNIMPLEMENTED();
}

void FileSystemController::Reset() {
    UNIMPLEMENTED();
}

void LoopProcess(Core::System& system) {
    auto server_manager = std::make_unique<ServerManager>(system);

    const auto FileSystemProxyFactory = [&] { return std::make_shared<FSP_SRV>(system); };

    server_manager->RegisterNamedService("fsp-ldr", std::make_shared<FSP_LDR>(system));
    server_manager->RegisterNamedService("fsp:pr", std::make_shared<FSP_PR>(system));
    server_manager->RegisterNamedService("fsp-srv", std::move(FileSystemProxyFactory));
    ServerManager::RunServer(std::move(server_manager));
}

} // namespace Service::FileSystem
