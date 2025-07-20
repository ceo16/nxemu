// SPDX-FileCopyrightText: 2014 Citra Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <vector>
#include <glad/glad.h>
#include "yuzu_common/common_types.h"
#include "yuzu_common/math_util.h"

#include "yuzu_video_core/renderer_base.h"
#include "yuzu_video_core/renderer_opengl/gl_device.h"
#include "yuzu_video_core/renderer_opengl/gl_rasterizer.h"
#include "yuzu_video_core/renderer_opengl/gl_resource_manager.h"
#include "yuzu_video_core/renderer_opengl/gl_shader_manager.h"
#include "yuzu_video_core/renderer_opengl/gl_state_tracker.h"

namespace Core {
class System;
} // namespace Core

namespace Core::Frontend {
class EmuWindow;
}

namespace Tegra {
class GPU;
}

namespace OpenGL {

class BlitScreen;

class RendererOpenGL final : public VideoCore::RendererBase {
public:
    explicit RendererOpenGL(Core::Frontend::EmuWindow& emu_window_,
                            Tegra::MaxwellDeviceMemoryManager& device_memory_, Tegra::GPU& gpu_,
                            std::unique_ptr<Core::Frontend::GraphicsContext> context_);
    ~RendererOpenGL() override;

    void Composite(std::span<const Tegra::FramebufferConfig> framebuffers) override;

    std::vector<u8> GetAppletCaptureBuffer() override;

    VideoCore::RasterizerInterface* ReadRasterizer() override {
        return &rasterizer;
    }

    [[nodiscard]] std::string GetDeviceVendor() const override {
        return device.GetVendorName();
    }

private:
    void AddTelemetryFields();

    void InitializeWatermark();
    void RenderWatermark();
    void CleanupWatermark();
    void RenderToBuffer(std::span<const Tegra::FramebufferConfig> framebuffers,
                        const Layout::FramebufferLayout& layout, void* dst);
    void RenderScreenshot(std::span<const Tegra::FramebufferConfig> framebuffers);
    void RenderAppletCaptureLayer(std::span<const Tegra::FramebufferConfig> framebuffers);

    Core::Frontend::EmuWindow& emu_window;
    Tegra::MaxwellDeviceMemoryManager& device_memory;
    Tegra::GPU& gpu;

    Device device;
    StateTracker state_tracker;
    ProgramManager program_manager;
    RasterizerOpenGL rasterizer;
    OGLFramebuffer screenshot_framebuffer;
    OGLFramebuffer capture_framebuffer;
    OGLRenderbuffer capture_renderbuffer;
    GLuint watermark_texture;
    GLuint watermark_vao;
    GLuint watermark_vbo;
    GLuint watermark_shader;
    int watermark_width;
    int watermark_height;
    std::chrono::steady_clock::time_point watermark_start_time;
    bool watermark_timer_started;
    bool watermark_show;

    std::unique_ptr<BlitScreen> blit_screen;
    std::unique_ptr<BlitScreen> blit_applet;
};

} // namespace OpenGL
