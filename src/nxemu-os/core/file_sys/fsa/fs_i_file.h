// SPDX-FileCopyrightText: Copyright 2024 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "yuzu_common/overflow.h"
#include "core/file_sys/errors.h"
#include "core/file_sys/filesystem_interfaces.h"
#include "core/file_sys/fs_file.h"
#include "core/file_sys/fs_operate_range.h"
#include "core/hle/result.h"
#include <nxemu-module-spec/system_loader.h>

namespace FileSys::Fsa {

class IFile {
public:
    explicit IFile(IVirtualFilePtr && backend_) : backend(std::move(backend_)) {}
    virtual ~IFile() {}

    Result Read(size_t* out, s64 offset, void* buffer, size_t size, const ReadOption& option) {
        // Check that we have an output pointer
        R_UNLESS(out != nullptr, ResultNullptrArgument);

        // If we have nothing to read, just succeed
        if (size == 0) {
            *out = 0;
            R_SUCCEED();
        }

        // Check that the read is valid
        R_UNLESS(buffer != nullptr, ResultNullptrArgument);
        R_UNLESS(offset >= 0, ResultOutOfRange);
        R_UNLESS(Common::CanAddWithoutOverflow<s64>(offset, size), ResultOutOfRange);

        // Do the read
        R_RETURN(this->DoRead(out, offset, buffer, size, option));
    }

    Result Read(size_t* out, s64 offset, void* buffer, size_t size) {
        R_RETURN(this->Read(out, offset, buffer, size, ReadOption::None));
    }
    Result Flush() {
        R_RETURN(this->DoFlush());
    }

    Result Write(s64 offset, const void* buffer, size_t size, const WriteOption& option) {
        // Handle the zero-size case
        if (size == 0) {
            if (option.HasFlushFlag()) {
                R_TRY(this->Flush());
            }
            R_SUCCEED();
        }

        // Check the write is valid
        R_UNLESS(buffer != nullptr, ResultNullptrArgument);
        R_UNLESS(offset >= 0, ResultOutOfRange);
        R_UNLESS(Common::CanAddWithoutOverflow<s64>(offset, size), ResultOutOfRange);

        R_RETURN(this->DoWrite(offset, buffer, size, option));
    }
protected:

private:
    Result DoRead(size_t* out, s64 offset, void* buffer, size_t size, const ReadOption& option) {
        const auto read_size = backend->ReadBytes(static_cast<u8*>(buffer), size, offset);
        *out = read_size;

        R_SUCCEED();
    }

    Result DoGetSize(s64* out) {
        *out = backend->GetSize();
        R_SUCCEED();
    }

    Result DoFlush() {
        // Exists for SDK compatibiltity -- No need to flush file.
        R_SUCCEED();
    }

    Result DoWrite(s64 offset, const void* buffer, size_t size, const WriteOption& option) {
        const std::size_t written = backend->WriteBytes(static_cast<const u8*>(buffer), size, offset);

        ASSERT_MSG(written == size,
                   "Could not write all bytes to file (requested={:016X}, actual={:016X}).", size,
                   written);

        R_SUCCEED();
    }

    IVirtualFilePtr backend;
};

} // namespace FileSys::Fsa
