// SPDX-FileCopyrightText: Copyright 2024 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "core/core.h"
#include "core/hle/service/sm/sm.h"
#include "core/hle/service/vi/container.h"
#include "core/hle/service/vi/vi_results.h"

namespace Service::VI {

Container::Container(Core::System& system) {
    m_displays.CreateDisplay(DisplayName{"Default"});
    m_displays.CreateDisplay(DisplayName{"External"});
    m_displays.CreateDisplay(DisplayName{"Edid"});
    m_displays.CreateDisplay(DisplayName{"Internal"});
    m_displays.CreateDisplay(DisplayName{"Null"});


    m_conductor.emplace(system, *this, m_displays);
}

Container::~Container() {
    this->OnTerminate();
}

void Container::OnTerminate() {
    std::scoped_lock lk{m_lock};

    m_is_shut_down = true;

    m_layers.ForEachLayer([&](auto& layer) { this->DestroyLayerLocked(layer.GetId()); });

    UNIMPLEMENTED();
}

SharedBufferManager* Container::GetSharedBufferManager() {
    return std::addressof(*m_shared_buffer_manager);
}

Result Container::GetBinderDriver(
    std::shared_ptr<Nvnflinger::IHOSBinderDriver>* out_binder_driver) {
    *out_binder_driver = m_binder_driver;
    R_SUCCEED();
}

Result Container::GetLayerProducerHandle(
    std::shared_ptr<android::BufferQueueProducer>* out_producer, u64 layer_id) {
    UNIMPLEMENTED();
    R_SUCCEED();
}

Result Container::OpenDisplay(u64* out_display_id, const DisplayName& display_name) {
    auto* const display = m_displays.GetDisplayByName(display_name);
    R_UNLESS(display != nullptr, VI::ResultNotFound);

    *out_display_id = display->GetId();
    R_SUCCEED();
}

Result Container::CloseDisplay(u64 display_id) {
    R_SUCCEED();
}

Result Container::CreateManagedLayer(u64* out_layer_id, u64 display_id, u64 owner_aruid) {
    std::scoped_lock lk{m_lock};
    R_RETURN(this->CreateLayerLocked(out_layer_id, display_id, owner_aruid));
}

Result Container::DestroyManagedLayer(u64 layer_id) {
    std::scoped_lock lk{m_lock};

    // Try to close, if open, but don't fail if not.
    this->CloseLayerLocked(layer_id);

    R_RETURN(this->DestroyLayerLocked(layer_id));
}

Result Container::OpenLayer(s32* out_producer_binder_id, u64 layer_id, u64 aruid) {
    std::scoped_lock lk{m_lock};
    R_RETURN(this->OpenLayerLocked(out_producer_binder_id, layer_id, aruid));
}

Result Container::CloseLayer(u64 layer_id) {
    std::scoped_lock lk{m_lock};
    R_RETURN(this->CloseLayerLocked(layer_id));
}

Result Container::SetLayerVisibility(u64 layer_id, bool visible) {
    std::scoped_lock lk{m_lock};

    auto* const layer = m_layers.GetLayerById(layer_id);
    R_UNLESS(layer != nullptr, VI::ResultNotFound);

    UNIMPLEMENTED();
    R_SUCCEED();
}

Result Container::SetLayerBlending(u64 layer_id, bool enabled) {
    UNIMPLEMENTED();
    R_SUCCEED();
}

void Container::LinkVsyncEvent(u64 display_id, Event* event) {
    std::scoped_lock lk{m_lock};
    m_conductor->LinkVsyncEvent(display_id, event);
}

void Container::UnlinkVsyncEvent(u64 display_id, Event* event) {
    std::scoped_lock lk{m_lock};
    m_conductor->UnlinkVsyncEvent(display_id, event);
}

Result Container::CreateStrayLayer(s32* out_producer_binder_id, u64* out_layer_id, u64 display_id) {
    std::scoped_lock lk{m_lock};
    R_TRY(this->CreateLayerLocked(out_layer_id, display_id, {}));
    R_RETURN(this->OpenLayerLocked(out_producer_binder_id, *out_layer_id, {}));
}

Result Container::DestroyStrayLayer(u64 layer_id) {
    std::scoped_lock lk{m_lock};
    R_TRY(this->CloseLayerLocked(layer_id));
    R_RETURN(this->DestroyLayerLocked(layer_id));
}

Result Container::CreateLayerLocked(u64* out_layer_id, u64 display_id, u64 owner_aruid) {
    UNIMPLEMENTED();
    R_SUCCEED();
}

Result Container::DestroyLayerLocked(u64 layer_id) {
    UNIMPLEMENTED();
    R_SUCCEED();
}

Result Container::OpenLayerLocked(s32* out_producer_binder_id, u64 layer_id, u64 aruid) {
    UNIMPLEMENTED();
    R_SUCCEED();
}

Result Container::CloseLayerLocked(u64 layer_id) {
    UNIMPLEMENTED();
    R_SUCCEED();
}

bool Container::ComposeOnDisplay(s32* out_swap_interval, f32* out_compose_speed_scale,
                                 u64 display_id) {
    UNIMPLEMENTED();
    return false;
}

} // namespace Service::VI
