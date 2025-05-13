// SPDX-FileCopyrightText: Copyright 2024 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "core/file_sys/fs_path.h"
#include "core/hle/result.h"
#include "core/hle/service/filesystem/filesystem.h"

namespace FileSys::Fsa {

class IFile;
class IDirectory;

enum class QueryId : u32 {
    SetConcatenationFileAttribute = 0,
    UpdateMac = 1,
    IsSignedSystemPartitionOnSdCardValid = 2,
    QueryUnpreparedFileInformation = 3,
};

class IFileSystem {
public:
    virtual ~IFileSystem() {}

    Result DeleteFile(const Path& path) {
        R_RETURN(this->DoDeleteFile(path));
    }

    Result CreateDirectory(const Path& path) {
        R_RETURN(this->DoCreateDirectory(path));
    }

    Result DeleteDirectory(const Path& path) {
        R_RETURN(this->DoDeleteDirectory(path));
    }

    Result DeleteDirectoryRecursively(const Path& path) {
        R_RETURN(this->DoDeleteDirectoryRecursively(path));
    }

    Result RenameFile(const Path& old_path, const Path& new_path) {
        R_RETURN(this->DoRenameFile(old_path, new_path));
    }

    Result RenameDirectory(const Path& old_path, const Path& new_path) {
        R_RETURN(this->DoRenameDirectory(old_path, new_path));
    }

    Result Commit() {
        R_RETURN(this->DoCommit());
    }

    Result GetFreeSpaceSize(s64* out, const Path& path) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result GetTotalSpaceSize(s64* out, const Path& path) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result CleanDirectoryRecursively(const Path& path) {
        R_RETURN(this->DoCleanDirectoryRecursively(path));
    }

    Result QueryEntry(char* dst, size_t dst_size, const char* src, size_t src_size, QueryId query,
                      const Path& path) {
        R_RETURN(this->DoQueryEntry(dst, dst_size, src, src_size, query, path));
    }

    // These aren't accessible as commands
    Result CommitProvisionally(s64 counter) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result Rollback() {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result Flush() {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

private:
    Result DoCreateFile(const Path& path, s64 size, int flags) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result DoDeleteFile(const Path& path) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result DoCreateDirectory(const Path& path) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result DoDeleteDirectory(const Path& path) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result DoDeleteDirectoryRecursively(const Path& path) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result DoRenameFile(const Path& old_path, const Path& new_path) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result DoRenameDirectory(const Path& old_path, const Path& new_path) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result DoCommit() {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result DoGetFreeSpaceSize(s64* out, const Path& path) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result DoGetTotalSpaceSize(s64* out, const Path& path) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result DoCleanDirectoryRecursively(const Path& path) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result DoQueryEntry(char* dst, size_t dst_size, const char* src, size_t src_size, QueryId query,
                        const Path& path) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }
};

} // namespace FileSys::Fsa
