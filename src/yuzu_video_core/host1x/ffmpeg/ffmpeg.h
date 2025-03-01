// SPDX-FileCopyrightText: Copyright 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>
#include <optional>
#include <span>
#include <vector>
#include <queue>

#include "yuzu_common/common_funcs.h"
#include "yuzu_common/common_types.h"
#include "yuzu_video_core/host1x/nvdec_common.h"

extern "C" {
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
}

namespace FFmpeg {

class DecodeApi {
public:
    YUZU_NON_COPYABLE(DecodeApi);
    YUZU_NON_MOVEABLE(DecodeApi);

    DecodeApi() = default;
    ~DecodeApi() = default;

    bool Initialize(Tegra::Host1x::NvdecCommon::VideoCodec codec);
    void Reset();

    bool SendPacket(std::span<const u8> packet_data, size_t configuration_size);
};

} // namespace FFmpeg
