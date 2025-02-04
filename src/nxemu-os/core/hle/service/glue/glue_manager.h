// SPDX-FileCopyrightText: Copyright 2019 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <map>
#include <vector>
#include "common/common_types.h"
#include "core/hle/result.h"

namespace Service::Glue {

// A class to manage state related to the arp:w and arp:r services, specifically the registration
// and unregistration of launch and control properties.
class ARPManager {
public:
    ARPManager();
    ~ARPManager();

    // Returns a vector of the raw bytes of NACP data (necessarily 0x4000 in size) corresponding to
    // the provided title ID if it was previously registered, otherwise ResultProcessIdNotRegistered
    // if it was never registered or ResultInvalidProcessId if the title ID is 0.
    Result GetControlProperty(std::vector<u8>* out_control_property, u64 title_id) const;

    // Removes the registration for the provided title ID from the database, returning
    // ResultProcessIdNotRegistered if it doesn't exist in the database and ResultInvalidProcessId
    // if the title ID is 0.
    Result Unregister(u64 title_id);

    // Removes all entries from the database, always succeeds. Should only be used when resetting
    // system state.
    void ResetAll();

private:
    struct MapEntry;
    std::map<u64, MapEntry> entries;
};

} // namespace Service::Glue
