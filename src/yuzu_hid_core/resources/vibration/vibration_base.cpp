// SPDX-FileCopyrightText: Copyright 2024 yuzu Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include "hid_result.h"
#include "resources/npad/npad_types.h"
#include "resources/npad/npad_vibration.h"
#include "resources/vibration/vibration_base.h"

namespace Service::HID {

NpadVibrationBase::NpadVibrationBase() {}

Result NpadVibrationBase::Activate() {
    ref_counter++;
    return ResultSuccess;
}

Result NpadVibrationBase::Deactivate() {
    if (ref_counter > 0) {
        ref_counter--;
    }

    return ResultSuccess;
}

bool NpadVibrationBase::IsActive() const {
    return ref_counter > 0;
}

bool NpadVibrationBase::IsVibrationMounted() const {
    return is_mounted;
}

} // namespace Service::HID
