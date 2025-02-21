// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <array>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <boost/container/flat_map.hpp>
#include "yuzu_common/common_types.h"
#include "core/file_sys/vfs/vfs.h"

namespace FileSys {
class CNMT;

enum class ContentRecordType : u8;
enum class TitleType : u8;

struct ContentRecord;
struct CNMTHeader;
struct MetaRecord;
class RegisteredCache;

using VfsCopyFunction = std::function<bool(const VirtualFile&, const VirtualFile&, size_t)>;

enum class InstallResult {
    Success,
    OverwriteExisting,
    ErrorAlreadyExists,
    ErrorCopyFailed,
    ErrorMetaFailed,
    ErrorBaseInstall,
};

struct ContentProviderEntry {
    u64 title_id;
    ContentRecordType type;

    std::string DebugInfo() const;
};

constexpr u64 GetUpdateTitleID(u64 base_title_id) {
    return base_title_id | 0x800;
}

// boost flat_map requires operator< for O(log(n)) lookups.
bool operator<(const ContentProviderEntry& lhs, const ContentProviderEntry& rhs);

// std unique requires operator== to identify duplicates.
bool operator==(const ContentProviderEntry& lhs, const ContentProviderEntry& rhs);
bool operator!=(const ContentProviderEntry& lhs, const ContentProviderEntry& rhs);

class ContentProvider {
public:
    virtual ~ContentProvider();

    virtual void Refresh() = 0;

    virtual bool HasEntry(u64 title_id, ContentRecordType type) const = 0;
    bool HasEntry(ContentProviderEntry entry) const;

    virtual std::optional<u32> GetEntryVersion(u64 title_id) const = 0;

    virtual VirtualFile GetEntryUnparsed(u64 title_id, ContentRecordType type) const = 0;
    VirtualFile GetEntryUnparsed(ContentProviderEntry entry) const;

    virtual VirtualFile GetEntryRaw(u64 title_id, ContentRecordType type) const = 0;
    VirtualFile GetEntryRaw(ContentProviderEntry entry) const;

    virtual std::vector<ContentProviderEntry> ListEntries() const;

    // If a parameter is not std::nullopt, it will be filtered for from all entries.
    virtual std::vector<ContentProviderEntry> ListEntriesFilter(
        std::optional<TitleType> title_type = {}, std::optional<ContentRecordType> record_type = {},
        std::optional<u64> title_id = {}) const = 0;
};

class PlaceholderCache {
public:
    explicit PlaceholderCache(VirtualDir dir);

private:
    VirtualDir dir;
};

enum class ContentProviderUnionSlot {
    SysNAND,        ///< System NAND
    UserNAND,       ///< User NAND
    SDMC,           ///< SD Card
    FrontendManual, ///< Frontend-defined game list or similar
};

// Combines multiple ContentProvider(s) (i.e. SysNAND, UserNAND, SDMC) into one interface.
class ContentProviderUnion : public ContentProvider {
public:
    ~ContentProviderUnion() override;

    void SetSlot(ContentProviderUnionSlot slot, ContentProvider* provider);
    void ClearSlot(ContentProviderUnionSlot slot);

    void Refresh() override;
    bool HasEntry(u64 title_id, ContentRecordType type) const override;
    std::optional<u32> GetEntryVersion(u64 title_id) const override;
    VirtualFile GetEntryUnparsed(u64 title_id, ContentRecordType type) const override;
    VirtualFile GetEntryRaw(u64 title_id, ContentRecordType type) const override;
    std::vector<ContentProviderEntry> ListEntriesFilter(
        std::optional<TitleType> title_type, std::optional<ContentRecordType> record_type,
        std::optional<u64> title_id) const override;

    std::vector<std::pair<ContentProviderUnionSlot, ContentProviderEntry>> ListEntriesFilterOrigin(
        std::optional<ContentProviderUnionSlot> origin = {},
        std::optional<TitleType> title_type = {}, std::optional<ContentRecordType> record_type = {},
        std::optional<u64> title_id = {}) const;

    std::optional<ContentProviderUnionSlot> GetSlotForEntry(u64 title_id,
                                                            ContentRecordType type) const;

private:
    std::map<ContentProviderUnionSlot, ContentProvider*> providers;
};

class ManualContentProvider : public ContentProvider {
public:
    ~ManualContentProvider() override;

    void AddEntry(TitleType title_type, ContentRecordType content_type, u64 title_id,
                  VirtualFile file);
    void ClearAllEntries();

    void Refresh() override;
    bool HasEntry(u64 title_id, ContentRecordType type) const override;
    std::optional<u32> GetEntryVersion(u64 title_id) const override;
    VirtualFile GetEntryUnparsed(u64 title_id, ContentRecordType type) const override;
    VirtualFile GetEntryRaw(u64 title_id, ContentRecordType type) const override;
    std::vector<ContentProviderEntry> ListEntriesFilter(
        std::optional<TitleType> title_type, std::optional<ContentRecordType> record_type,
        std::optional<u64> title_id) const override;

private:
    std::map<std::tuple<TitleType, ContentRecordType, u64>, VirtualFile> entries;
};

} // namespace FileSys
