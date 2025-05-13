// SPDX-FileCopyrightText: Copyright 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "yuzu_common/common_funcs.h"
#include "yuzu_common/common_types.h"

namespace FileSys {

enum class OpenMode : u32 {
    Read = (1 << 0),
    Write = (1 << 1),
    AllowAppend = (1 << 2),

    ReadWrite = (Read | Write),
    All = (ReadWrite | AllowAppend),
};
DECLARE_ENUM_FLAG_OPERATORS(OpenMode)
} // namespace FileSys
