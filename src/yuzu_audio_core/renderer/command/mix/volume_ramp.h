// SPDX-FileCopyrightText: Copyright 2022 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

#include "yuzu_audio_core/renderer/command/icommand.h"
#include "yuzu_common/common_types.h"

namespace AudioCore::ADSP::AudioRenderer {
class CommandListProcessor;
}

namespace AudioCore::Renderer {

/**
 * AudioRenderer command for applying volume to a mix buffer, with ramping for the volume to smooth
 * out the transition.
 */
struct VolumeRampCommand : ICommand {
    /**
     * Print this command's information to a string.
     *
     * @param processor - The CommandListProcessor processing this command.
     * @param string    - The string to print into.
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

    /// Fixed point precision
    u8 precision;
    /// Input mix buffer index
    s16 input_index;
    /// Output mix buffer index
    s16 output_index;
    /// Previous mix volume applied to the input
    f32 prev_volume;
    /// Current mix volume applied to the input
    f32 volume;
};

} // namespace AudioCore::Renderer
