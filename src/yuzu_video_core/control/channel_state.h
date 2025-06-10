// SPDX-FileCopyrightText: 2022 yuzu Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <memory>

#include "yuzu_common/common_types.h"
#include <nxemu-module-spec/video.h>

namespace Core {
class System;
}

namespace VideoCore {
class RasterizerInterface;
}

namespace Tegra {

class GPU;
class MemoryManagerRegistry;

namespace Engines {
class Puller;
class Fermi2D;
class Maxwell3D;
class MaxwellDMA;
class KeplerCompute;
class KeplerMemory;
} // namespace Engines

class MemoryManager;
class DmaPusher;

namespace Control {

struct ChannelState {
    explicit ChannelState(s32 bind_id);
    ChannelState(const ChannelState& state) = delete;
    ChannelState& operator=(const ChannelState&) = delete;
    ChannelState(ChannelState&& other) noexcept = default;
    ChannelState& operator=(ChannelState&& other) noexcept = default;

    void Init(GPU& gpu, u64 program_id);

    void BindRasterizer(VideoCore::RasterizerInterface* rasterizer);

    s32 bind_id = -1;
    u64 program_id = 0;
    /// 3D engine
    std::unique_ptr<Engines::Maxwell3D> maxwell_3d;
    /// 2D engine
    std::unique_ptr<Engines::Fermi2D> fermi_2d;
    /// Compute engine
    std::unique_ptr<Engines::KeplerCompute> kepler_compute;
    /// DMA engine
    std::unique_ptr<Engines::MaxwellDMA> maxwell_dma;
    /// Inline memory engine
    std::unique_ptr<Engines::KeplerMemory> kepler_memory;

    std::shared_ptr<MemoryManager> memory_manager;

    std::unique_ptr<DmaPusher> dma_pusher;

    bool initialized{};
};

} // namespace Control

} // namespace Tegra

class VideoManager;

class IChannelStatePtr :
    public IChannelState
{
public:
    IChannelStatePtr(Tegra::GPU & gpu, Tegra::MemoryManagerRegistry & registry);
    IChannelStatePtr(Tegra::GPU & gpu, Tegra::MemoryManagerRegistry & registry, std::shared_ptr<Tegra::Control::ChannelState> && channelState);
    ~IChannelStatePtr();

    // IChannelState
    bool Initialized() const override;
    int32_t BindId() const override;
    void Init(uint64_t programId) override;
    void SetMemoryManager(uint32_t id) override;
    void Release() override;

private:
    IChannelStatePtr();
    IChannelStatePtr(const IChannelStatePtr&) = delete;
    IChannelStatePtr& operator=(const IChannelStatePtr&) = delete;

    Tegra::GPU & m_gpu;
    Tegra::MemoryManagerRegistry & m_registry;
    std::shared_ptr<Tegra::Control::ChannelState> m_channelState;
};
