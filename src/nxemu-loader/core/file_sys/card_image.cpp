// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <array>
#include <string>

#include <fmt/ostream.h>

#include "yuzu_common/yuzu_assert.h"
#include "yuzu_common/logging/log.h"
#include "core/file_sys/card_image.h"
#include "core/file_sys/content_archive.h"
#include "core/file_sys/nca_metadata.h"
#include "core/file_sys/partition_filesystem.h"
#include "core/file_sys/submission_package.h"
#include "core/file_sys/vfs/vfs_offset.h"
#include "core/file_sys/vfs/vfs_vector.h"
#include "core/loader/loader.h"

namespace FileSys {

constexpr u64 GAMECARD_CERTIFICATE_OFFSET = 0x7000;
constexpr std::array partition_names{
    "update",
    "normal",
    "secure",
    "logo",
};

XCI::XCI(VirtualFile file_, u64 program_id, size_t program_index)
    : file(std::move(file_)), program_nca_status{LoaderResultStatus::ErrorXCIMissingProgramNCA},
      partitions(partition_names.size()),
      partitions_raw(partition_names.size()) {
    const auto header_status = TryReadHeader();
    if (header_status != LoaderResultStatus::Success) {
        status = header_status;
        return;
    }

    PartitionFilesystem main_hfs(std::make_shared<OffsetVfsFile>(
        file, file->GetSize() - header.hfs_offset, header.hfs_offset));

    update_normal_partition_end = main_hfs.GetFileOffsets()["secure"];

    if (main_hfs.GetStatus() != LoaderResultStatus::Success) {
        status = main_hfs.GetStatus();
        return;
    }

    for (XCIPartition partition :
         {XCIPartition::Update, XCIPartition::Normal, XCIPartition::Secure, XCIPartition::Logo}) {
        const auto partition_idx = static_cast<std::size_t>(partition);
        auto raw = main_hfs.GetFile(partition_names[partition_idx]);

        partitions_raw[static_cast<std::size_t>(partition)] = std::move(raw);
    }

    UNIMPLEMENTED();
    status = LoaderResultStatus::ErrorNotImplemented;
}

XCI::~XCI() = default;

LoaderResultStatus XCI::GetStatus() const {
    return status;
}

LoaderResultStatus XCI::GetProgramNCAStatus() const {
    return program_nca_status;
}

VirtualDir XCI::GetPartition(XCIPartition partition) {
    const auto id = static_cast<std::size_t>(partition);
    if (partitions[id] == nullptr && partitions_raw[id] != nullptr) {
        partitions[id] = std::make_shared<PartitionFilesystem>(partitions_raw[id]);
    }

    return partitions[static_cast<std::size_t>(partition)];
}

std::vector<VirtualDir> XCI::GetPartitions() {
    std::vector<VirtualDir> out;
    for (const auto& id :
         {XCIPartition::Update, XCIPartition::Normal, XCIPartition::Secure, XCIPartition::Logo}) {
        const auto part = GetPartition(id);
        if (part != nullptr) {
            out.push_back(part);
        }
    }
    return out;
}

VirtualDir XCI::GetSecurePartition() {
    return GetPartition(XCIPartition::Secure);
}

VirtualDir XCI::GetNormalPartition() {
    return GetPartition(XCIPartition::Normal);
}

VirtualDir XCI::GetUpdatePartition() {
    return GetPartition(XCIPartition::Update);
}

VirtualDir XCI::GetLogoPartition() {
    return GetPartition(XCIPartition::Logo);
}

VirtualFile XCI::GetPartitionRaw(XCIPartition partition) const {
    return partitions_raw[static_cast<std::size_t>(partition)];
}

VirtualFile XCI::GetSecurePartitionRaw() const {
    return GetPartitionRaw(XCIPartition::Secure);
}

VirtualFile XCI::GetStoragePartition0() const {
    return std::make_shared<OffsetVfsFile>(file, update_normal_partition_end, 0, "partition0");
}

VirtualFile XCI::GetStoragePartition1() const {
    return std::make_shared<OffsetVfsFile>(file, file->GetSize() - update_normal_partition_end,
                                           update_normal_partition_end, "partition1");
}

VirtualFile XCI::GetNormalPartitionRaw() const {
    return GetPartitionRaw(XCIPartition::Normal);
}

VirtualFile XCI::GetUpdatePartitionRaw() const {
    return GetPartitionRaw(XCIPartition::Update);
}

VirtualFile XCI::GetLogoPartitionRaw() const {
    return GetPartitionRaw(XCIPartition::Logo);
}

u64 XCI::GetProgramTitleID() const {
    UNIMPLEMENTED();
    return 0;
}

std::vector<u64> XCI::GetProgramTitleIDs() const {
    UNIMPLEMENTED();
    return {};
}

u32 XCI::GetSystemUpdateVersion() {
    const auto update = GetPartition(XCIPartition::Update);
    if (update == nullptr) {
        return 0;
    }

    for (const auto& update_file : update->GetFiles()) {
        NCA nca{update_file};

        if (nca.GetStatus() != LoaderResultStatus::Success || nca.GetSubdirectories().empty()) {
            continue;
        }

        if (nca.GetType() == NCAContentType::Meta && nca.GetTitleId() == 0x0100000000000816) {
            const auto dir = nca.GetSubdirectories()[0];
            const auto cnmt = dir->GetFile("SystemUpdate_0100000000000816.cnmt");
            if (cnmt == nullptr) {
                continue;
            }

            CNMT cnmt_data{cnmt};

            const auto metas = cnmt_data.GetMetaRecords();
            if (metas.empty()) {
                continue;
            }

            return metas[0].title_version;
        }
    }

    return 0;
}

u64 XCI::GetSystemUpdateTitleID() const {
    return 0x0100000000000816;
}

std::vector<VirtualFile> XCI::GetFiles() const {
    return {};
}

std::vector<VirtualDir> XCI::GetSubdirectories() const {
    return {};
}

std::string XCI::GetName() const {
    return file->GetName();
}

VirtualDir XCI::GetParentDirectory() const {
    return file->GetContainingDirectory();
}

VirtualDir XCI::ConcatenatedPseudoDirectory() {
    const auto out = std::make_shared<VectorVfsDirectory>();
    for (const auto& part_id : {XCIPartition::Normal, XCIPartition::Logo, XCIPartition::Secure}) {
        const auto& part = GetPartition(part_id);
        if (part == nullptr)
            continue;

        for (const auto& part_file : part->GetFiles())
            out->AddFile(part_file);
    }

    return out;
}

LoaderResultStatus XCI::TryReadHeader() {
    constexpr size_t CardInitialDataRegionSize = 0x1000;

    // Define the function we'll use to determine if we read a valid header.
    const auto ReadCardHeader = [&]() {
        // Ensure we can read the entire header. If we can't, we can't read the card image.
        if (file->ReadObject(&header) != sizeof(GamecardHeader)) {
            return LoaderResultStatus::ErrorBadXCIHeader;
        }

        // Ensure the header magic matches. If it doesn't, this isn't a card image header.
        if (header.magic != Common::MakeMagic('H', 'E', 'A', 'D')) {
            return LoaderResultStatus::ErrorBadXCIHeader;
        }

        // We read a card image header.
        return LoaderResultStatus::Success;
    };

    // Try to read the header directly.
    if (ReadCardHeader() == LoaderResultStatus::Success) {
        return LoaderResultStatus::Success;
    }

    // Get the size of the file.
    const size_t card_image_size = file->GetSize();

    // If we are large enough to have a key area, offset past the key area and retry.
    if (card_image_size >= CardInitialDataRegionSize) {
        file = std::make_shared<OffsetVfsFile>(file, card_image_size - CardInitialDataRegionSize,
                                               CardInitialDataRegionSize);
        return ReadCardHeader();
    }

    // We had no header and aren't large enough to have a key area, so this can't be parsed.
    return LoaderResultStatus::ErrorBadXCIHeader;
}

u8 XCI::GetFormatVersion() {
    return GetLogoPartition() == nullptr ? 0x1 : 0x2;
}
} // namespace FileSys
