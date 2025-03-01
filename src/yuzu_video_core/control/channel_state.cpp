// SPDX-FileCopyrightText: 2022 yuzu Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include "yuzu_common/yuzu_assert.h"
#include "yuzu_video_core/control/channel_state.h"
#include "yuzu_video_core/dma_pusher.h"
#include "yuzu_video_core/engines/fermi_2d.h"
#include "yuzu_video_core/engines/kepler_compute.h"
#include "yuzu_video_core/engines/kepler_memory.h"
#include "yuzu_video_core/engines/maxwell_3d.h"
#include "yuzu_video_core/engines/maxwell_dma.h"
#include "yuzu_video_core/engines/puller.h"
#include "yuzu_video_core/memory_manager.h"

namespace Tegra::Control {

ChannelState::ChannelState(s32 bind_id_) : bind_id{bind_id_}, initialized{} {}

void ChannelState::Init(GPU& gpu, u64 program_id_) {
    ASSERT(memory_manager);
    program_id = program_id_;
    dma_pusher = std::make_unique<Tegra::DmaPusher>(gpu, *memory_manager, *this);
    maxwell_3d = std::make_unique<Engines::Maxwell3D>(*memory_manager);
    fermi_2d = std::make_unique<Engines::Fermi2D>(*memory_manager);
    kepler_compute = std::make_unique<Engines::KeplerCompute>(*memory_manager);
    maxwell_dma = std::make_unique<Engines::MaxwellDMA>(*memory_manager);
    kepler_memory = std::make_unique<Engines::KeplerMemory>(*memory_manager);
    initialized = true;
}

void ChannelState::BindRasterizer(VideoCore::RasterizerInterface* rasterizer) {
    dma_pusher->BindRasterizer(rasterizer);
    memory_manager->BindRasterizer(rasterizer);
    maxwell_3d->BindRasterizer(rasterizer);
    fermi_2d->BindRasterizer(rasterizer);
    kepler_memory->BindRasterizer(rasterizer);
    kepler_compute->BindRasterizer(rasterizer);
    maxwell_dma->BindRasterizer(rasterizer);
}

} // namespace Tegra::Control
