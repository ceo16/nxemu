// SPDX-FileCopyrightText: Copyright 2024 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "yuzu_common/alignment.h"
#include "yuzu_common/common_funcs.h"
#include "core/file_sys/fs_memory_management.h"
#include "core/file_sys/fs_path_utility.h"
#include "core/file_sys/fs_string_util.h"
#include "core/hle/result.h"

namespace FileSys {
class DirectoryPathParser;

class Path {
    YUZU_NON_COPYABLE(Path);
    YUZU_NON_MOVEABLE(Path);

private:
    static constexpr const char* EmptyPath = "";
    static constexpr size_t WriteBufferAlignmentLength = 8;

private:
    friend class DirectoryPathParser;

public:
    class WriteBuffer {
        YUZU_NON_COPYABLE(WriteBuffer);

    private:
        char* m_buffer;
        size_t m_length_and_is_normalized;

    public:
        constexpr WriteBuffer() : m_buffer(nullptr), m_length_and_is_normalized(0) {}

        constexpr ~WriteBuffer() {
            if (m_buffer != nullptr) {
                Deallocate(m_buffer, this->GetLength());
                this->ResetBuffer();
            }
        }

        constexpr WriteBuffer(WriteBuffer&& rhs)
            : m_buffer(rhs.m_buffer), m_length_and_is_normalized(rhs.m_length_and_is_normalized) {
            rhs.ResetBuffer();
        }

        constexpr WriteBuffer& operator=(WriteBuffer&& rhs) {
            if (m_buffer != nullptr) {
                Deallocate(m_buffer, this->GetLength());
            }

            m_buffer = rhs.m_buffer;
            m_length_and_is_normalized = rhs.m_length_and_is_normalized;

            rhs.ResetBuffer();

            return *this;
        }

        constexpr void ResetBuffer() {
            m_buffer = nullptr;
            this->SetLength(0);
        }

        constexpr char* Get() const {
            return m_buffer;
        }

        constexpr size_t GetLength() const {
            return m_length_and_is_normalized >> 1;
        }

        constexpr bool IsNormalized() const {
            return static_cast<bool>(m_length_and_is_normalized & 1);
        }

        constexpr void SetNormalized() {
            m_length_and_is_normalized |= static_cast<size_t>(1);
        }

        constexpr void SetNotNormalized() {
            m_length_and_is_normalized &= ~static_cast<size_t>(1);
        }

    private:
        constexpr WriteBuffer(char* buffer, size_t length)
            : m_buffer(buffer), m_length_and_is_normalized(0) {
            this->SetLength(length);
        }

    public:
        static WriteBuffer Make(size_t length) {
            if (void* alloc = Allocate(length); alloc != nullptr) {
                return WriteBuffer(static_cast<char*>(alloc), length);
            } else {
                return WriteBuffer();
            }
        }

    private:
        constexpr void SetLength(size_t size) {
            m_length_and_is_normalized = (m_length_and_is_normalized & 1) | (size << 1);
        }
    };

private:
    const char* m_str;
    WriteBuffer m_write_buffer;

public:
    constexpr Path() : m_str(EmptyPath), m_write_buffer() {}

    constexpr Path(const char* s) : m_str(s), m_write_buffer() {
        m_write_buffer.SetNormalized();
    }

    constexpr ~Path() = default;

    Result SetShallowBuffer(const char* buffer) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    constexpr const char* GetString() const {
        // Check pre-conditions
        ASSERT(this->IsNormalized());

        return m_str;
    }

    constexpr size_t GetLength() const {
        if (std::is_constant_evaluated()) {
            return Strlen(this->GetString());
        } else {
            return std::strlen(this->GetString());
        }
    }

    constexpr bool IsEmpty() const {
        return *m_str == '\x00';
    }

    constexpr bool IsMatchHead(const char* p, size_t len) const {
        return Strncmp(this->GetString(), p, len) == 0;
    }

    Result Initialize(const Path& rhs) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result Initialize(const char* path, size_t len) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result Initialize(const char* path) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result InitializeWithReplaceBackslash(const char* path) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result InitializeWithReplaceForwardSlashes(const char* path) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result InitializeWithNormalization(const char* path, size_t size) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result InitializeWithNormalization(const char* path) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result InitializeAsEmpty() {
        // Clear our buffer
        this->ClearBuffer();

        // Set normalized
        this->SetNormalized();

        R_SUCCEED();
    }

    Result AppendChild(const char* child) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result AppendChild(const Path& rhs) {
        R_RETURN(this->AppendChild(rhs.GetString()));
    }

    Result Combine(const Path& parent, const Path& child) {
        // Get the lengths
        const auto p_len = parent.GetLength();
        const auto c_len = child.GetLength();

        // Allocate our buffer
        R_TRY(this->Preallocate(p_len + c_len + 1));

        // Initialize as parent
        R_TRY(this->Initialize(parent));

        // If we're empty, we can just initialize as child
        if (this->IsEmpty()) {
            R_TRY(this->Initialize(child));
        } else {
            // Otherwise, we should append the child
            R_TRY(this->AppendChild(child));
        }

        R_SUCCEED();
    }

    Result RemoveChild() {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result Normalize(const PathFlags& flags) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

private:
    void ClearBuffer() {
        m_write_buffer.ResetBuffer();
        m_str = EmptyPath;
    }

    void SetModifiableBuffer(WriteBuffer&& buffer) {
        // Check pre-conditions
        ASSERT(buffer.Get() != nullptr);
        ASSERT(buffer.GetLength() > 0);
        ASSERT(Common::IsAligned(buffer.GetLength(), WriteBufferAlignmentLength));

        // Get whether we're normalized
        if (m_write_buffer.IsNormalized()) {
            buffer.SetNormalized();
        } else {
            buffer.SetNotNormalized();
        }

        // Set write buffer
        m_write_buffer = std::move(buffer);
        m_str = m_write_buffer.Get();
    }

    constexpr void SetReadOnlyBuffer(const char* buffer) {
        m_str = buffer;
        m_write_buffer.ResetBuffer();
    }

    Result Preallocate(size_t length) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    Result InitializeImpl(const char* path, size_t size) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    constexpr char* GetWriteBuffer() {
        ASSERT(m_write_buffer.Get() != nullptr);
        return m_write_buffer.Get();
    }

    constexpr size_t GetWriteBufferLength() const {
        return m_write_buffer.GetLength();
    }

    constexpr bool IsNormalized() const {
        return m_write_buffer.IsNormalized();
    }

    constexpr void SetNormalized() {
        m_write_buffer.SetNormalized();
    }

    constexpr void SetNotNormalized() {
        m_write_buffer.SetNotNormalized();
    }

public:
    bool operator==(const FileSys::Path& rhs) const {
        return std::strcmp(this->GetString(), rhs.GetString()) == 0;
    }
    bool operator!=(const FileSys::Path& rhs) const {
        return !(*this == rhs);
    }
    bool operator==(const char* p) const {
        return std::strcmp(this->GetString(), p) == 0;
    }
    bool operator!=(const char* p) const {
        return !(*this == p);
    }
};

inline Result SetUpFixedPath(FileSys::Path* out, const char* s) {
    UNIMPLEMENTED();
    R_SUCCEED();
}

constexpr inline bool IsWindowsDriveRootPath(const FileSys::Path& path) {
    const char* const str = path.GetString();
    return IsWindowsDrive(str) &&
           (str[2] == StringTraits::DirectorySeparator ||
            str[2] == StringTraits::AlternateDirectorySeparator) &&
           str[3] == StringTraits::NullTerminator;
}

} // namespace FileSys
