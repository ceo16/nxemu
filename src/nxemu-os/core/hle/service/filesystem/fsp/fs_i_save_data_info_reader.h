// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <vector>
#include "yuzu_common/common_types.h"
#include "core/hle/service/cmif_types.h"
#include "core/hle/service/service.h"
#include "core/file_sys/fs_save_data_types.h"

namespace Service::FileSystem {

class SaveDataController;

class ISaveDataInfoReader final : public ServiceFramework<ISaveDataInfoReader> {
public:
    explicit ISaveDataInfoReader(Core::System& system_,
                                 std::shared_ptr<SaveDataController> save_data_controller_,
                                 SaveDataSpaceId space);
    ~ISaveDataInfoReader() override;

    struct SaveDataInfo {
        u64_le save_id_unknown;
        SaveDataSpaceId space;
        SaveDataType type;
        INSERT_PADDING_BYTES(0x6);
        std::array<u8, 0x10> user_id;
        u64_le save_id;
        u64_le title_id;
        u64_le save_image_size;
        u16_le index;
        FileSys::SaveDataRank rank;
        INSERT_PADDING_BYTES(0x25);
    };
    static_assert(sizeof(SaveDataInfo) == 0x60, "SaveDataInfo has incorrect size.");

    Result ReadSaveDataInfo(Out<u64> out_count,
                            OutArray<SaveDataInfo, BufferAttr_HipcMapAlias> out_entries);

private:
    void FindAllSaves(SaveDataSpaceId space);

    std::shared_ptr<SaveDataController> save_data_controller;
    std::vector<SaveDataInfo> info;
    u64 next_entry_index = 0;
};

} // namespace Service::FileSystem
