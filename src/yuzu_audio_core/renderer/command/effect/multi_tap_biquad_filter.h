// SPDX-FileCopyrightText: Copyright 2022 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <array>
#include <string>

#include "yuzu_audio_core/renderer/command/icommand.h"
#include "yuzu_audio_core/renderer/voice/voice_info.h"
#include "yuzu_common/common_types.h"

namespace AudioCore::ADSP::AudioRenderer {
class CommandListProcessor;
}

namespace AudioCore::Renderer {

/**
 * AudioRenderer command for applying multiple biquads at once.
 */
struct MultiTapBiquadFilterCommand : ICommand {
    /**
     * Print this command's information to a string.
     *
     * @param processor - The CommandListProcessor processing this command.
     * @param string - The string to print into.
     */
    void Dump(const AudioRenderer::CommandListProcessor& processor, std::string& string) override;

    /**
     * Process this command.
     *
     * @param processor - The CommandListProcessor processing this command.
     */
    void Process(const AudioRenderer::CommandListProcessor& processor) override;

    /**
     * Verify this command's data is valid.
     *
     * @param processor - The CommandListProcessor processing this command.
     * @return True if the command is valid, otherwise false.
     */
    bool Verify(const AudioRenderer::CommandListProcessor& processor) override;

    /// Input mix buffer index
    s16 input;
    /// Output mix buffer index
    s16 output;
    /// Biquad parameters
    std::array<VoiceInfo::BiquadFilterParameter, MaxBiquadFilters> biquads;
    /// Biquad states, updated each call
    std::array<CpuAddr, MaxBiquadFilters> states;
    /// If each biquad needs initialisation
    std::array<bool, MaxBiquadFilters> needs_init;
    /// Number of active biquads
    u8 filter_tap_count;
};

} // namespace AudioCore::Renderer
