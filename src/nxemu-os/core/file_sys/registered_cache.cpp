// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <algorithm>
#include <random>
#include <regex>
#include "yuzu_common/yuzu_assert.h"
#include "yuzu_common/fs/path_util.h"
#include "yuzu_common/hex_util.h"
#include "yuzu_common/logging/log.h"
#include "yuzu_common/scope_exit.h"
#include "core/file_sys/card_image.h"
#include "core/file_sys/common_funcs.h"
#include "core/file_sys/nca_metadata.h"
#include "core/file_sys/registered_cache.h"
#include "core/file_sys/submission_package.h"
#include "core/file_sys/vfs/vfs_concat.h"
#include "core/loader/loader.h"

namespace FileSys {

// The size of blocks to use when vfs raw copying into nand.
constexpr size_t VFS_RC_LARGE_COPY_BLOCK = 0x400000;

std::string ContentProviderEntry::DebugInfo() const {
    return fmt::format("title_id={:016X}, content_type={:02X}", title_id, static_cast<u8>(type));
}

bool operator<(const ContentProviderEntry& lhs, const ContentProviderEntry& rhs) {
    return (lhs.title_id < rhs.title_id) || (lhs.title_id == rhs.title_id && lhs.type < rhs.type);
}

bool operator==(const ContentProviderEntry& lhs, const ContentProviderEntry& rhs) {
    return std::tie(lhs.title_id, lhs.type) == std::tie(rhs.title_id, rhs.type);
}

bool operator!=(const ContentProviderEntry& lhs, const ContentProviderEntry& rhs) {
    return !operator==(lhs, rhs);
}

static bool FollowsTwoDigitDirFormat(std::string_view name) {
    static const std::regex two_digit_regex("000000[0-9A-F]{2}", std::regex_constants::ECMAScript |
                                                                     std::regex_constants::icase);
    return std::regex_match(name.begin(), name.end(), two_digit_regex);
}

static bool FollowsNcaIdFormat(std::string_view name) {
    static const std::regex nca_id_regex("[0-9A-F]{32}\\.nca", std::regex_constants::ECMAScript |
                                                                   std::regex_constants::icase);
    static const std::regex nca_id_cnmt_regex(
        "[0-9A-F]{32}\\.cnmt.nca", std::regex_constants::ECMAScript | std::regex_constants::icase);
    return (name.size() == 36 && std::regex_match(name.begin(), name.end(), nca_id_regex)) ||
           (name.size() == 41 && std::regex_match(name.begin(), name.end(), nca_id_cnmt_regex));
}

static std::string GetRelativePathFromNcaID(const std::array<u8, 16>& nca_id, bool second_hex_upper,
                                            bool within_two_digit, bool cnmt_suffix) {
    UNIMPLEMENTED();
    return "";
}

static std::string GetCNMTName(TitleType type, u64 title_id) {
    static constexpr std::array<const char*, 9> TITLE_TYPE_NAMES{
        "SystemProgram",
        "SystemData",
        "SystemUpdate",
        "BootImagePackage",
        "BootImagePackageSafe",
        "Application",
        "Patch",
        "AddOnContent",
        "" ///< Currently unknown 'DeltaTitle'
    };

    auto index = static_cast<std::size_t>(type);
    // If the index is after the jump in TitleType, subtract it out.
    if (index >= static_cast<std::size_t>(TitleType::Application)) {
        index -= static_cast<std::size_t>(TitleType::Application) -
                 static_cast<std::size_t>(TitleType::FirmwarePackageB);
    }
    return fmt::format("{}_{:016x}.cnmt", TITLE_TYPE_NAMES[index], title_id);
}

ContentProvider::~ContentProvider() = default;

bool ContentProvider::HasEntry(ContentProviderEntry entry) const {
    return HasEntry(entry.title_id, entry.type);
}

VirtualFile ContentProvider::GetEntryUnparsed(ContentProviderEntry entry) const {
    return GetEntryUnparsed(entry.title_id, entry.type);
}

VirtualFile ContentProvider::GetEntryRaw(ContentProviderEntry entry) const {
    return GetEntryRaw(entry.title_id, entry.type);
}

std::vector<ContentProviderEntry> ContentProvider::ListEntries() const {
    return ListEntriesFilter(std::nullopt, std::nullopt, std::nullopt);
}

PlaceholderCache::PlaceholderCache(VirtualDir dir_) : dir(std::move(dir_)) {}

ContentProviderUnion::~ContentProviderUnion() = default;

void ContentProviderUnion::SetSlot(ContentProviderUnionSlot slot, ContentProvider* provider) {
    providers[slot] = provider;
}

void ContentProviderUnion::ClearSlot(ContentProviderUnionSlot slot) {
    providers[slot] = nullptr;
}

void ContentProviderUnion::Refresh() {
    for (auto& provider : providers) {
        if (provider.second == nullptr)
            continue;

        provider.second->Refresh();
    }
}

bool ContentProviderUnion::HasEntry(u64 title_id, ContentRecordType type) const {
    for (const auto& provider : providers) {
        if (provider.second == nullptr)
            continue;

        if (provider.second->HasEntry(title_id, type))
            return true;
    }

    return false;
}

std::optional<u32> ContentProviderUnion::GetEntryVersion(u64 title_id) const {
    for (const auto& provider : providers) {
        if (provider.second == nullptr)
            continue;

        const auto res = provider.second->GetEntryVersion(title_id);
        if (res != std::nullopt)
            return res;
    }

    return std::nullopt;
}

VirtualFile ContentProviderUnion::GetEntryUnparsed(u64 title_id, ContentRecordType type) const {
    for (const auto& provider : providers) {
        if (provider.second == nullptr)
            continue;

        const auto res = provider.second->GetEntryUnparsed(title_id, type);
        if (res != nullptr)
            return res;
    }

    return nullptr;
}

VirtualFile ContentProviderUnion::GetEntryRaw(u64 title_id, ContentRecordType type) const {
    for (const auto& provider : providers) {
        if (provider.second == nullptr)
            continue;

        const auto res = provider.second->GetEntryRaw(title_id, type);
        if (res != nullptr)
            return res;
    }

    return nullptr;
}

std::vector<ContentProviderEntry> ContentProviderUnion::ListEntriesFilter(
    std::optional<TitleType> title_type, std::optional<ContentRecordType> record_type,
    std::optional<u64> title_id) const {
    std::vector<ContentProviderEntry> out;

    for (const auto& provider : providers) {
        if (provider.second == nullptr)
            continue;

        const auto vec = provider.second->ListEntriesFilter(title_type, record_type, title_id);
        std::copy(vec.begin(), vec.end(), std::back_inserter(out));
    }

    std::sort(out.begin(), out.end());
    out.erase(std::unique(out.begin(), out.end()), out.end());
    return out;
}

std::vector<std::pair<ContentProviderUnionSlot, ContentProviderEntry>>
ContentProviderUnion::ListEntriesFilterOrigin(std::optional<ContentProviderUnionSlot> origin,
                                              std::optional<TitleType> title_type,
                                              std::optional<ContentRecordType> record_type,
                                              std::optional<u64> title_id) const {
    std::vector<std::pair<ContentProviderUnionSlot, ContentProviderEntry>> out;

    for (const auto& provider : providers) {
        if (provider.second == nullptr)
            continue;

        if (origin.has_value() && *origin != provider.first)
            continue;

        const auto vec = provider.second->ListEntriesFilter(title_type, record_type, title_id);
        std::transform(vec.begin(), vec.end(), std::back_inserter(out),
                       [&provider](const ContentProviderEntry& entry) {
                           return std::make_pair(provider.first, entry);
                       });
    }

    std::sort(out.begin(), out.end());
    out.erase(std::unique(out.begin(), out.end()), out.end());
    return out;
}

std::optional<ContentProviderUnionSlot> ContentProviderUnion::GetSlotForEntry(
    u64 title_id, ContentRecordType type) const {
    const auto iter =
        std::find_if(providers.begin(), providers.end(), [title_id, type](const auto& provider) {
            return provider.second != nullptr && provider.second->HasEntry(title_id, type);
        });

    if (iter == providers.end()) {
        return std::nullopt;
    }

    return iter->first;
}

ManualContentProvider::~ManualContentProvider() = default;

void ManualContentProvider::AddEntry(TitleType title_type, ContentRecordType content_type,
                                     u64 title_id, VirtualFile file) {
    entries.insert_or_assign({title_type, content_type, title_id}, file);
}

void ManualContentProvider::ClearAllEntries() {
    entries.clear();
}

void ManualContentProvider::Refresh() {}

bool ManualContentProvider::HasEntry(u64 title_id, ContentRecordType type) const {
    return GetEntryRaw(title_id, type) != nullptr;
}

std::optional<u32> ManualContentProvider::GetEntryVersion(u64 title_id) const {
    return std::nullopt;
}

VirtualFile ManualContentProvider::GetEntryUnparsed(u64 title_id, ContentRecordType type) const {
    return GetEntryRaw(title_id, type);
}

VirtualFile ManualContentProvider::GetEntryRaw(u64 title_id, ContentRecordType type) const {
    const auto iter =
        std::find_if(entries.begin(), entries.end(), [title_id, type](const auto& entry) {
            const auto content_type = std::get<1>(entry.first);
            const auto e_title_id = std::get<2>(entry.first);
            return content_type == type && e_title_id == title_id;
        });
    if (iter == entries.end())
        return nullptr;
    return iter->second;
}

std::vector<ContentProviderEntry> ManualContentProvider::ListEntriesFilter(
    std::optional<TitleType> title_type, std::optional<ContentRecordType> record_type,
    std::optional<u64> title_id) const {
    std::vector<ContentProviderEntry> out;

    for (const auto& entry : entries) {
        const auto [e_title_type, e_content_type, e_title_id] = entry.first;
        if ((title_type == std::nullopt || e_title_type == *title_type) &&
            (record_type == std::nullopt || e_content_type == *record_type) &&
            (title_id == std::nullopt || e_title_id == *title_id)) {
            out.emplace_back(ContentProviderEntry{e_title_id, e_content_type});
        }
    }

    std::sort(out.begin(), out.end());
    out.erase(std::unique(out.begin(), out.end()), out.end());
    return out;
}
} // namespace FileSys
