// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "core/file_sys/filesystem.h"
#include "core/file_sys/vfs/vfs.h"
#include "core/file_sys/savedata_factory.h"
#include "core/file_sys/registered_cache.h"
#include "core/file_sys/bis_factory.h"
#include "core/file_sys/sdmc_factory.h"
#include "core/file_sys/romfs_factory.h"

#include "system_loader.h"
#include <yuzu_common/logging/log.h>
#include <yuzu_common/fs/path_util.h>

namespace FileSys {

FileSystemController::FileSystemController(Systemloader & loader_) : loader{ loader_ } {}

FileSystemController::~FileSystemController() = default;

bool FileSystemController::RegisterProcess(
    ProcessId process_id, ProgramId program_id,
    std::shared_ptr<FileSys::RomFSFactory>&& romfs_factory) {
    std::scoped_lock lk{ registration_lock };

    registrations.emplace(process_id, Registration{
                                          .program_id = program_id,
                                          .romfs_factory = std::move(romfs_factory),
                                          .save_data_factory = CreateSaveDataFactory(program_id),
        });

    LOG_DEBUG(Service_FS, "Registered for process {}", process_id);
    return true;
}

IFileSysRegisteredCache * FileSystemController::GetSystemNANDContents() const
{
    return SystemNANDContents();
}

ISaveDataController * FileSystemController::OpenSaveDataController() const
{
    UNIMPLEMENTED();
    return nullptr;
}

uint64_t FileSystemController::GetFreeSpaceSize(StorageId id) const
{
    UNIMPLEMENTED();
    return 0;
}

uint64_t FileSystemController::GetTotalSpaceSize(StorageId id) const
{
    UNIMPLEMENTED();
    return 0;
}

bool FileSystemController::OpenProcess(uint64_t* programId, ISaveDataFactory ** saveDataFactory, IRomFsController ** romFsController, uint64_t processId)
{
    std::scoped_lock lk{ registration_lock };

    const auto it = registrations.find(processId);
    if (it == registrations.end())
    {
        UNIMPLEMENTED();
        return false;
    }

    *programId = it->second.program_id;
    *saveDataFactory = std::make_unique<SaveDataFactoryPtr>(it->second.save_data_factory).release();
    *romFsController = std::make_unique<RomFSFactoryPtr>(it->second.romfs_factory).release();
    return true;
}

bool FileSystemController::OpenSDMC(IVirtualDirectory ** out_sdmc) const
{
    LOG_TRACE(Service_FS, "Opening SDMC");

    if (sdmc_factory == nullptr)
    {
        return false;
    }

    VirtualDir sdmc = sdmc_factory->Open();
    if (sdmc == nullptr) 
    {
        return false;
    }

    *out_sdmc = std::make_unique<VirtualDirectoryPtr>(sdmc).release();
    return true;
}

FileSys::RegisteredCache * FileSystemController::SystemNANDContents() const
{
    LOG_TRACE(Service_FS, "Opening System NAND Contents");

    if (bis_factory == nullptr)
    {
        return nullptr;
    }
    return bis_factory->GetSystemNANDContents();
}

void FileSystemController::CreateFactories(FileSys::VfsFilesystem & vfs, bool overwrite) {
    if (overwrite) {
        bis_factory = nullptr;
        sdmc_factory = nullptr;
    }

    using YuzuPath = Common::FS::YuzuPath;
    const auto sdmc_dir_path = Common::FS::GetYuzuPath(YuzuPath::SDMCDir);
    const auto sdmc_load_dir_path = sdmc_dir_path / "atmosphere/contents";
    const auto rw_mode = VirtualFileOpenMode::ReadWrite;

    auto nand_directory =
        vfs.OpenDirectory(Common::FS::GetYuzuPathString(YuzuPath::NANDDir), rw_mode);
    auto sd_directory = vfs.OpenDirectory(Common::FS::PathToUTF8String(sdmc_dir_path), rw_mode);
    auto load_directory = vfs.OpenDirectory(Common::FS::GetYuzuPathString(YuzuPath::LoadDir),
        VirtualFileOpenMode::Read);
    auto sd_load_directory = vfs.OpenDirectory(Common::FS::PathToUTF8String(sdmc_load_dir_path),
        VirtualFileOpenMode::Read);
    auto dump_directory =
        vfs.OpenDirectory(Common::FS::GetYuzuPathString(YuzuPath::DumpDir), rw_mode);

    if (bis_factory == nullptr) {
        bis_factory = std::make_unique<FileSys::BISFactory>(
            nand_directory, std::move(load_directory), std::move(dump_directory));
        loader.RegisterContentProvider(FileSys::ContentProviderUnionSlot::SysNAND,
            bis_factory->GetSystemNANDContents());
        loader.RegisterContentProvider(FileSys::ContentProviderUnionSlot::UserNAND,
            bis_factory->GetUserNANDContents());
    }

    if (sdmc_factory == nullptr) {
        sdmc_factory = std::make_unique<FileSys::SDMCFactory>(std::move(sd_directory),
            std::move(sd_load_directory));
        loader.RegisterContentProvider(FileSys::ContentProviderUnionSlot::SDMC,
            sdmc_factory->GetSDMCContents());
    }
}

std::shared_ptr<FileSys::SaveDataFactory> FileSystemController::CreateSaveDataFactory(
    ProgramId program_id) {
    using YuzuPath = Common::FS::YuzuPath;
    const auto rw_mode = VirtualFileOpenMode::ReadWrite;

    auto vfs = loader.GetFilesystem();
    const auto nand_directory =
        vfs->OpenDirectory(Common::FS::GetYuzuPathString(YuzuPath::NANDDir), rw_mode);
    return std::make_shared<FileSys::SaveDataFactory>(loader, program_id,
        std::move(nand_directory));
}

}