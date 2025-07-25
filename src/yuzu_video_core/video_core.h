// SPDX-FileCopyrightText: 2014 Citra Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>

namespace Core {
class System;
}

namespace Core::Frontend {
class EmuWindow;
}

namespace Tegra {
class GPU;
}

namespace VideoCore {

class RendererBase;

/// Creates an emulated GPU instance using the given system context.
std::unique_ptr<Tegra::GPU> CreateGPU(ISwitchSystem & system, Core::Frontend::EmuWindow & emu_window, Tegra::Host1x::Host1x & host1x);

} // namespace VideoCore
