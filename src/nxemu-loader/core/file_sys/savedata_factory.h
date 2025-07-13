// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>
#include <string>
#include "yuzu_common/common_funcs.h"
#include "yuzu_common/common_types.h"
#include "core/file_sys/fs_save_data_types.h"
#include "core/file_sys/vfs/vfs.h"

class Systemloader;

namespace FileSys {

constexpr const char* GetSaveDataSizeFileName() {
    return ".yuzu_save_size";
}

using ProgramId = u64;

/// File system interface to the SaveData archive
class SaveDataFactory {
public:
    explicit SaveDataFactory(Systemloader & loader_, ProgramId program_id_,
                             VirtualDir save_directory_);
    ~SaveDataFactory();

    VirtualDir Create(SaveDataSpaceId space, const SaveDataAttribute& meta) const;
    VirtualDir Open(SaveDataSpaceId space, const SaveDataAttribute& meta) const;

    VirtualDir GetSaveDataSpaceDirectory(SaveDataSpaceId space) const;

    static std::string GetSaveDataSpaceIdPath(SaveDataSpaceId space);
    static std::string GetFullPath(ProgramId program_id, VirtualDir dir, SaveDataSpaceId space,
                                   SaveDataType type, u64 title_id, u128 user_id, u64 save_id);
    static std::string GetUserGameSaveDataRoot(u128 user_id, bool future);

    SaveDataSize ReadSaveDataSize(SaveDataType type, u64 title_id, u128 user_id) const;
    void WriteSaveDataSize(SaveDataType type, u64 title_id, u128 user_id,
                           SaveDataSize new_value) const;

    void SetAutoCreate(bool state);

private:
    Systemloader & loader;
    ProgramId program_id;
    VirtualDir dir;
    bool auto_create{true};
};

} // namespace FileSys

class SaveDataFactoryPtr :
    public ISaveDataFactory
{
public:
    SaveDataFactoryPtr();
    SaveDataFactoryPtr(std::shared_ptr<FileSys::SaveDataFactory> saveDataFactory);
    SaveDataFactoryPtr(SaveDataFactoryPtr && other) noexcept;
    ~SaveDataFactoryPtr();

    SaveDataFactoryPtr & operator=(SaveDataFactoryPtr && other) noexcept;

    FileSys::SaveDataFactory * operator->() const;
    FileSys::SaveDataFactory & operator*() const;
    operator bool() const;

    // ISaveDataFactory
    IVirtualDirectory * Open(SaveDataSpaceId space, const SaveDataAttribute & meta) const override;
    void Release() override;

private:
    SaveDataFactoryPtr(const SaveDataFactoryPtr &) = delete;
    SaveDataFactoryPtr & operator=(const SaveDataFactoryPtr &) = delete;

    std::shared_ptr<FileSys::SaveDataFactory> m_saveDataFactory;
};
