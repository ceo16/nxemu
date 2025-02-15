// SPDX-FileCopyrightText: Copyright 2021 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "yuzu_common/yuzu_assert.h"
#include "yuzu_common/common_funcs.h"
#include "yuzu_common/logging/backend.h"

#include "yuzu_common/settings.h"

void assert_fail_impl() {
    if (Settings::values.use_debug_asserts) {
        Common::Log::Stop();
        Crash();
    }
}

[[noreturn]] void unreachable_impl() {
    Common::Log::Stop();
    Crash();
    throw std::runtime_error("Unreachable code");
}
