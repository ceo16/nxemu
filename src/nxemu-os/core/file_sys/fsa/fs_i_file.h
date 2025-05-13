// SPDX-FileCopyrightText: Copyright 2024 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "yuzu_common/overflow.h"
#include "core/file_sys/fs_file.h"
#include "core/file_sys/fs_operate_range.h"
#include "core/hle/result.h"

namespace FileSys::Fsa {

class IFile {
public:
    Result Flush() {
        R_RETURN(this->DoFlush());
    }

protected:

private:
    Result DoFlush() {
        // Exists for SDK compatibiltity -- No need to flush file.
        R_SUCCEED();
    }
};

} // namespace FileSys::Fsa
