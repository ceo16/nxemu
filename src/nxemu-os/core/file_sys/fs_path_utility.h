// SPDX-FileCopyrightText: Copyright 2024 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "yuzu_common/yuzu_assert.h"
#include "yuzu_common/common_funcs.h"
#include "yuzu_common/common_types.h"
#include "yuzu_common/scope_exit.h"
#include "core/file_sys/fs_memory_management.h"
#include "core/file_sys/fs_string_util.h"
#include "core/hle/result.h"

namespace FileSys {

constexpr inline size_t MountNameLengthMax = 15;

namespace StringTraits {

constexpr inline char DirectorySeparator = '/';
constexpr inline char DriveSeparator = ':';
constexpr inline char Dot = '.';
constexpr inline char NullTerminator = '\x00';

constexpr inline char AlternateDirectorySeparator = '\\';

constexpr inline const char InvalidCharacters[6] = {':', '*', '?', '<', '>', '|'};
constexpr inline const char InvalidCharactersForHostName[6] = {':', '*', '<', '>', '|', '$'};
constexpr inline const char InvalidCharactersForMountName[5] = {'*', '?', '<', '>', '|'};

namespace impl {

template <const char* InvalidCharacterSet, size_t NumInvalidCharacters>
consteval u64 MakeInvalidCharacterMask(size_t n) {
    u64 mask = 0;
    for (size_t i = 0; i < NumInvalidCharacters; ++i) {
        if ((static_cast<u64>(InvalidCharacterSet[i]) >> 6) == n) {
            mask |= static_cast<u64>(1) << (static_cast<u64>(InvalidCharacterSet[i]) & 0x3F);
        }
    }
    return mask;
}

template <const char* InvalidCharacterSet, size_t NumInvalidCharacters>
constexpr bool IsInvalidCharacterImpl(char c) {
    constexpr u64 Masks[4] = {
        MakeInvalidCharacterMask<InvalidCharacterSet, NumInvalidCharacters>(0),
        MakeInvalidCharacterMask<InvalidCharacterSet, NumInvalidCharacters>(1),
        MakeInvalidCharacterMask<InvalidCharacterSet, NumInvalidCharacters>(2),
        MakeInvalidCharacterMask<InvalidCharacterSet, NumInvalidCharacters>(3)};

    return (Masks[static_cast<u64>(c) >> 6] &
            (static_cast<u64>(1) << (static_cast<u64>(c) & 0x3F))) != 0;
}

} // namespace impl

constexpr bool IsInvalidCharacter(char c) {
    return impl::IsInvalidCharacterImpl<InvalidCharacters, Common::Size(InvalidCharacters)>(c);
}
constexpr bool IsInvalidCharacterForHostName(char c) {
    return impl::IsInvalidCharacterImpl<InvalidCharactersForHostName,
                                        Common::Size(InvalidCharactersForHostName)>(c);
}
constexpr bool IsInvalidCharacterForMountName(char c) {
    return impl::IsInvalidCharacterImpl<InvalidCharactersForMountName,
                                        Common::Size(InvalidCharactersForMountName)>(c);
}

} // namespace StringTraits

constexpr inline size_t WindowsDriveLength = 2;
constexpr inline size_t UncPathPrefixLength = 2;
constexpr inline size_t DosDevicePathPrefixLength = 4;

class PathFlags {
private:
    static constexpr u32 WindowsPathFlag = (1 << 0);
    static constexpr u32 RelativePathFlag = (1 << 1);
    static constexpr u32 EmptyPathFlag = (1 << 2);
    static constexpr u32 MountNameFlag = (1 << 3);
    static constexpr u32 BackslashFlag = (1 << 4);
    static constexpr u32 AllCharactersFlag = (1 << 5);

private:
    u32 m_value;

public:
    constexpr PathFlags() : m_value(0) { /* ... */
    }

#define DECLARE_PATH_FLAG_HANDLER(__WHICH__)                                                       \
    constexpr bool Is##__WHICH__##Allowed() const { return (m_value & __WHICH__##Flag) != 0; }     \
    constexpr void Allow##__WHICH__() { m_value |= __WHICH__##Flag; }

    DECLARE_PATH_FLAG_HANDLER(WindowsPath)
    DECLARE_PATH_FLAG_HANDLER(RelativePath)
    DECLARE_PATH_FLAG_HANDLER(EmptyPath)
    DECLARE_PATH_FLAG_HANDLER(MountName)
    DECLARE_PATH_FLAG_HANDLER(Backslash)
    DECLARE_PATH_FLAG_HANDLER(AllCharacters)

#undef DECLARE_PATH_FLAG_HANDLER
};

template <typename T>
    requires(std::same_as<T, char> || std::same_as<T, wchar_t>)
constexpr inline bool IsDosDevicePath(const T* path) {
    ASSERT(path != nullptr);

    using namespace StringTraits;

    return path[0] == AlternateDirectorySeparator && path[1] == AlternateDirectorySeparator &&
           (path[2] == Dot || path[2] == '?') &&
           (path[3] == DirectorySeparator || path[3] == AlternateDirectorySeparator);
}

template <typename T>
    requires(std::same_as<T, char> || std::same_as<T, wchar_t>)
constexpr inline bool IsUncPath(const T* path, bool allow_forward_slash = true,
                                bool allow_back_slash = true) {
    ASSERT(path != nullptr);

    using namespace StringTraits;

    return (allow_forward_slash && path[0] == DirectorySeparator &&
            path[1] == DirectorySeparator) ||
           (allow_back_slash && path[0] == AlternateDirectorySeparator &&
            path[1] == AlternateDirectorySeparator);
}

constexpr inline bool IsWindowsDrive(const char* path) {
    ASSERT(path != nullptr);

    return (('a' <= path[0] && path[0] <= 'z') || ('A' <= path[0] && path[0] <= 'Z')) &&
           path[1] == StringTraits::DriveSeparator;
}

constexpr inline bool IsWindowsPath(const char* path, bool allow_forward_slash_unc) {
    return IsWindowsDrive(path) || IsDosDevicePath(path) ||
           IsUncPath(path, allow_forward_slash_unc, true);
}

constexpr inline int GetWindowsSkipLength(const char* path) {
    if (IsDosDevicePath(path)) {
        return DosDevicePathPrefixLength;
    } else if (IsWindowsDrive(path)) {
        return WindowsDriveLength;
    } else if (IsUncPath(path)) {
        return UncPathPrefixLength;
    } else {
        return 0;
    }
}

constexpr inline bool IsPathAbsolute(const char* path) {
    return IsWindowsPath(path, false) || path[0] == StringTraits::DirectorySeparator;
}

constexpr inline bool IsPathRelative(const char* path) {
    return path[0] && !IsPathAbsolute(path);
}

constexpr inline bool IsCurrentDirectory(const char* path) {
    return path[0] == StringTraits::Dot &&
           (path[1] == StringTraits::NullTerminator || path[1] == StringTraits::DirectorySeparator);
}

constexpr inline bool IsParentDirectory(const char* path) {
    return path[0] == StringTraits::Dot && path[1] == StringTraits::Dot &&
           (path[2] == StringTraits::NullTerminator || path[2] == StringTraits::DirectorySeparator);
}

constexpr inline bool IsPathStartWithCurrentDirectory(const char* path) {
    return IsCurrentDirectory(path) || IsParentDirectory(path);
}

constexpr inline bool IsSubPath(const char* lhs, const char* rhs) {
    // Check pre-conditions
    ASSERT(lhs != nullptr);
    ASSERT(rhs != nullptr);

    // Import StringTraits names for current scope
    using namespace StringTraits;

    // Special case certain paths
    if (IsUncPath(lhs) && !IsUncPath(rhs)) {
        return false;
    }
    if (!IsUncPath(lhs) && IsUncPath(rhs)) {
        return false;
    }

    if (lhs[0] == DirectorySeparator && lhs[1] == NullTerminator && rhs[0] == DirectorySeparator &&
        rhs[1] != NullTerminator) {
        return true;
    }
    if (rhs[0] == DirectorySeparator && rhs[1] == NullTerminator && lhs[0] == DirectorySeparator &&
        lhs[1] != NullTerminator) {
        return true;
    }

    // Check subpath
    for (size_t i = 0; /* ... */; ++i) {
        if (lhs[i] == NullTerminator) {
            return rhs[i] == DirectorySeparator;
        } else if (rhs[i] == NullTerminator) {
            return lhs[i] == DirectorySeparator;
        } else if (lhs[i] != rhs[i]) {
            return false;
        }
    }
}

// Path utilities
constexpr inline void Replace(char* dst, size_t dst_size, char old_char, char new_char) {
    ASSERT(dst != nullptr);
    for (char* cur = dst; cur < dst + dst_size && *cur; ++cur) {
        if (*cur == old_char) {
            *cur = new_char;
        }
    }
}

inline Result CheckUtf8(const char* s) {
    UNIMPLEMENTED();
    R_SUCCEED();
}

// Path formatting
class PathNormalizer {
private:
    enum class PathState {
        Start,
        Normal,
        FirstSeparator,
        Separator,
        CurrentDir,
        ParentDir,
    };

private:
    static constexpr void ReplaceParentDirectoryPath(char* dst, const char* src) {
        // Use StringTraits names for remainder of scope
        using namespace StringTraits;

        // Start with a dir-separator
        dst[0] = DirectorySeparator;

        auto i = 1;
        while (src[i] != NullTerminator) {
            if ((src[i - 1] == DirectorySeparator || src[i - 1] == AlternateDirectorySeparator) &&
                src[i + 0] == Dot && src[i + 1] == Dot &&
                (src[i + 2] == DirectorySeparator || src[i + 2] == AlternateDirectorySeparator)) {
                dst[i - 1] = DirectorySeparator;
                dst[i + 0] = Dot;
                dst[i + 1] = Dot;
                dst[i + 2] = DirectorySeparator;
                i += 3;
            } else {
                if (src[i - 1] == AlternateDirectorySeparator && src[i + 0] == Dot &&
                    src[i + 1] == Dot && src[i + 2] == NullTerminator) {
                    dst[i - 1] = DirectorySeparator;
                    dst[i + 0] = Dot;
                    dst[i + 1] = Dot;
                    i += 2;
                    break;
                }

                dst[i] = src[i];
                ++i;
            }
        }

        dst[i] = StringTraits::NullTerminator;
    }

public:
    static constexpr bool IsParentDirectoryPathReplacementNeeded(const char* path) {
        // Use StringTraits names for remainder of scope
        using namespace StringTraits;

        if (path[0] != DirectorySeparator && path[0] != AlternateDirectorySeparator) {
            return false;
        }

        // Check to find a parent reference using alternate separators
        if (path[0] != NullTerminator && path[1] != NullTerminator && path[2] != NullTerminator) {
            size_t i;
            for (i = 0; path[i + 3] != NullTerminator; ++path) {
                if (path[i + 1] != Dot || path[i + 2] != Dot) {
                    continue;
                }

                const char c0 = path[i + 0];
                const char c3 = path[i + 3];

                if (c0 == AlternateDirectorySeparator &&
                    (c3 == DirectorySeparator || c3 == AlternateDirectorySeparator ||
                     c3 == NullTerminator)) {
                    return true;
                }

                if (c3 == AlternateDirectorySeparator &&
                    (c0 == DirectorySeparator || c0 == AlternateDirectorySeparator)) {
                    return true;
                }
            }

            if (path[i + 0] == AlternateDirectorySeparator && path[i + 1] == Dot &&
                path[i + 2] == Dot /* && path[i + 3] == NullTerminator */) {
                return true;
            }
        }

        return false;
    }

    static Result IsNormalized(bool* out, size_t* out_len, const char* path,
                                         bool allow_all_characters = false) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    static Result Normalize(char* dst, size_t* out_len, const char* path,
                                      size_t max_out_size, bool is_windows_path,
                                      bool is_drive_relative_path,
                                      bool allow_all_characters = false) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }
};

class PathFormatter {
private:
    static Result CheckSharedName(const char* name, size_t len) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    static Result CheckHostName(const char* name, size_t len) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    static Result CheckInvalidBackslash(bool* out_contains_backslash, const char* path,
                                                  bool allow_backslash) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

public:
    static Result CheckPathFormat(const char* path, const PathFlags& flags) {
        bool normalized;
        size_t len;
        R_RETURN(IsNormalized(std::addressof(normalized), std::addressof(len), path, flags));
    }

    static Result SkipMountName(const char** out, size_t* out_len, const char* path) {
        R_RETURN(ParseMountName(out, out_len, nullptr, 0, path));
    }

    static Result ParseMountName(const char** out, size_t* out_len, char* out_mount_name,
                                           size_t out_mount_name_buffer_size, const char* path) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    static Result SkipRelativeDotPath(const char** out, size_t* out_len,
                                                const char* path) {
        R_RETURN(ParseRelativeDotPath(out, out_len, nullptr, 0, path));
    }

    static Result ParseRelativeDotPath(const char** out, size_t* out_len,
                                                 char* out_relative,
                                                 size_t out_relative_buffer_size,
                                                 const char* path) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    static Result SkipWindowsPath(const char** out, size_t* out_len, bool* out_normalized,
                                            const char* path, bool has_mount_name) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    static Result ParseWindowsPath(const char** out, size_t* out_len, char* out_win,
                                             size_t out_win_buffer_size, const char* path,
                                             bool has_mount_name) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    static Result IsNormalized(bool* out, size_t* out_len, const char* path,
                                         const PathFlags& flags = {}) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }

    static Result Normalize(char* dst, size_t dst_size, const char* path, size_t path_len,
                                      const PathFlags& flags) {
        UNIMPLEMENTED();
        R_SUCCEED();
    }
};

} // namespace FileSys
