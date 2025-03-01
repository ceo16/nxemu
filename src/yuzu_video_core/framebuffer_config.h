// SPDX-FileCopyrightText: Copyright 2020 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "yuzu_common/common_types.h"
#include "yuzu_common/math_util.h"
#include "yuzu_video_core/service/nvnflinger/buffer_transform_flags.h"
#include "yuzu_video_core/service/nvnflinger/pixel_format.h"
#include "yuzu_video_core/service/nvnflinger/ui/fence.h"

namespace Tegra {

enum class BlendMode {
    Opaque,
    Premultiplied,
    Coverage,
};

/**
 * Struct describing framebuffer configuration
 */
struct FramebufferConfig {
    DAddr address{};
    u32 offset{};
    u32 width{};
    u32 height{};
    u32 stride{};
    Service::android::PixelFormat pixel_format{};
    Service::android::BufferTransformFlags transform_flags{};
    Common::Rectangle<int> crop_rect{};
    BlendMode blending{};
};

Common::Rectangle<f32> NormalizeCrop(const FramebufferConfig& framebuffer, u32 texture_width,
                                     u32 texture_height);

} // namespace Tegra
