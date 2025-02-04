// SPDX-FileCopyrightText: 2014 Citra Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <array>
#include <atomic>

#include "common/microprofile.h"
#include "core/core.h"
#include "core/core_timing.h"
#include "core/cpu_manager.h"
#include "core/debugger/debugger.h"
#include "core/gpu_dirty_memory_manager.h"
#include "core/hle/kernel/k_memory_manager.h"
#include "core/hle/kernel/k_process.h"
#include "core/hle/kernel/k_resource_limit.h"
#include "core/hle/kernel/k_scheduler.h"
#include "core/hle/kernel/kernel.h"
#include "core/hle/kernel/physical_core.h"
#include "core/hle/service/am/applet_manager.h"
#include "core/hle/service/am/frontend/applets.h"
#include "core/hle/service/apm/apm_controller.h"
#include "core/hle/service/services.h"

MICROPROFILE_DEFINE(ARM_CPU0, "ARM", "CPU 0", MP_RGB(255, 64, 64));
MICROPROFILE_DEFINE(ARM_CPU1, "ARM", "CPU 1", MP_RGB(255, 64, 64));
MICROPROFILE_DEFINE(ARM_CPU2, "ARM", "CPU 2", MP_RGB(255, 64, 64));
MICROPROFILE_DEFINE(ARM_CPU3, "ARM", "CPU 3", MP_RGB(255, 64, 64));

namespace Core {


struct System::Impl {
    explicit Impl(System & system, ISwitchSystem & switchSystem) 
        : kernel{system}, cpu_manager{system}, applet_manager{system}, frontend_applets{system}, switchSystem(switchSystem) {}

    void Initialize(System& system) {
        device_memory = std::make_unique<Core::DeviceMemory>();

        is_multicore = true; // Settings::values.use_multi_core.GetValue();

        core_timing.SetMulticore(is_multicore);
        core_timing.Initialize([&system]() { system.RegisterHostThread(); });

        // Create default implementations of applets if one is not provided.
        frontend_applets.SetDefaultAppletsIfMissing();

        is_async_gpu = false; // Settings::values.use_asynchronous_gpu_emulation.GetValue();

        kernel.SetMulticore(is_multicore);
        cpu_manager.SetMulticore(is_multicore);
        cpu_manager.SetAsyncGpu(is_async_gpu);
    }

    void ReinitializeIfNecessary(System& system) {
        const bool must_reinitialize = false;
        //    is_multicore != Settings::values.use_multi_core.GetValue() ||
        //    extended_memory_layout != (Settings::values.memory_layout_mode.GetValue() !=
        //                               Settings::MemoryLayout::Memory_4Gb);

        if (!must_reinitialize) {
            return;
        }

        LOG_DEBUG(Kernel, "Re-initializing");

        //is_multicore = Settings::values.use_multi_core.GetValue();
        //extended_memory_layout =
        //    Settings::values.memory_layout_mode.GetValue() != Settings::MemoryLayout::Memory_4Gb;

        Initialize(system);
    }

    void SetNVDECActive(bool is_nvdec_active) {
        nvdec_active = is_nvdec_active;
    }

    bool GetNVDECActive() {
        return nvdec_active;
    }

    void InitializeKernel(System& system) {
        LOG_DEBUG(Core, "initialized OK");

        // Setting changes may require a full system reinitialization (e.g., disabling multicore).
        ReinitializeIfNecessary(system);

        kernel.Initialize();
        cpu_manager.Initialize();

        service_manager = std::make_shared<Service::SM::ServiceManager>(kernel);
        services =
            std::make_unique<Service::Services>(service_manager, system, stop_event.get_token());

        is_powered_on = true;
        exit_locked = false;
        exit_requested = false;

        microprofile_cpu[0] = MICROPROFILE_TOKEN(ARM_CPU0);
        microprofile_cpu[1] = MICROPROFILE_TOKEN(ARM_CPU1);
        microprofile_cpu[2] = MICROPROFILE_TOKEN(ARM_CPU2);
        microprofile_cpu[3] = MICROPROFILE_TOKEN(ARM_CPU3);
        LOG_DEBUG(Core, "Initialized OK");
    }


    bool IsShuttingDown() const {
        return is_shutting_down;
    }

    void SetShuttingDown(bool shutting_down) {
        is_shutting_down = shutting_down;
    }

    std::atomic<bool> is_shutting_down{};

    Timing::CoreTiming core_timing;
    Kernel::KernelCore kernel;
    std::unique_ptr<Core::DeviceMemory> device_memory;

    CpuManager cpu_manager;
    std::atomic_bool is_powered_on{};
    bool exit_locked = false;
    bool exit_requested = false;

    bool nvdec_active{};

    std::array<u8, 0x20> build_id{};

    ISwitchSystem & switchSystem;

    /// Applets
    Service::AM::AppletManager applet_manager;
    Service::AM::Frontend::FrontendAppletHolder frontend_applets;

    /// APM (Performance) services
    Service::APM::Controller apm_controller{core_timing};

    /// Service manager
    std::shared_ptr<Service::SM::ServiceManager> service_manager;

    /// Services
    std::unique_ptr<Service::Services> services;

    bool is_multicore{};
    bool is_async_gpu{};

    ExecuteProgramCallback execute_program_callback;
    std::stop_source stop_event;

    std::array<u64, Core::Hardware::NUM_CPU_CORES> dynarmic_ticks{};
    std::array<MicroProfileToken, Core::Hardware::NUM_CPU_CORES> microprofile_cpu{};

    std::array<Core::GPUDirtyMemoryManager, Core::Hardware::NUM_CPU_CORES>
        gpu_dirty_memory_managers;

    std::deque<std::vector<u8>> user_channel;
};

System::System(ISwitchSystem & switchSystem) : impl{std::make_unique<Impl>(*this, switchSystem)} {}

System::~System() = default;

CpuManager& System::GetCpuManager() {
    return impl->cpu_manager;
}

const CpuManager& System::GetCpuManager() const {
    return impl->cpu_manager;
}

void System::InitializeKernel()
{
    impl->InitializeKernel(*this);
}

void System::Initialize()
{
    impl->Initialize(*this);
}

bool System::IsShuttingDown() const {
    return impl->IsShuttingDown();
}

void System::SetShuttingDown(bool shutting_down) {
    impl->SetShuttingDown(shutting_down);
}

bool System::IsPoweredOn() const {
    return impl->is_powered_on.load(std::memory_order::relaxed);
}

std::span<GPUDirtyMemoryManager> System::GetGPUDirtyMemoryManager() {
    return impl->gpu_dirty_memory_managers;
}

ISwitchSystem & System::GetSwitchSystem()
{
    return impl->switchSystem;
}

Kernel::PhysicalCore& System::CurrentPhysicalCore() {
    return impl->kernel.CurrentPhysicalCore();
}

const Kernel::PhysicalCore& System::CurrentPhysicalCore() const {
    return impl->kernel.CurrentPhysicalCore();
}

/// Gets the global scheduler
Kernel::GlobalSchedulerContext& System::GlobalSchedulerContext() {
    return impl->kernel.GlobalSchedulerContext();
}

/// Gets the global scheduler
const Kernel::GlobalSchedulerContext& System::GlobalSchedulerContext() const {
    return impl->kernel.GlobalSchedulerContext();
}

Kernel::KProcess* System::ApplicationProcess() {
    return impl->kernel.ApplicationProcess();
}

Core::DeviceMemory& System::DeviceMemory() {
    return *impl->device_memory;
}

const Core::DeviceMemory& System::DeviceMemory() const {
    return *impl->device_memory;
}

Kernel::KernelCore& System::Kernel() {
    return impl->kernel;
}

const Kernel::KernelCore& System::Kernel() const {
    return impl->kernel;
}

Timing::CoreTiming& System::CoreTiming() {
    return impl->core_timing;
}

const Timing::CoreTiming& System::CoreTiming() const {
    return impl->core_timing;
}

u64 System::GetApplicationProcessProgramID() const {
    return impl->kernel.ApplicationProcess()->GetProgramId();
}

void System::SetFrontendAppletSet(Service::AM::Frontend::FrontendAppletSet&& set) {
    impl->frontend_applets.SetFrontendAppletSet(std::move(set));
}

Service::AM::Frontend::FrontendAppletHolder& System::GetFrontendAppletHolder() {
    return impl->frontend_applets;
}

const Service::AM::Frontend::FrontendAppletHolder& System::GetFrontendAppletHolder() const {
    return impl->frontend_applets;
}

Service::AM::AppletManager& System::GetAppletManager() {
    return impl->applet_manager;
}

Service::APM::Controller& System::GetAPMController() {
    return impl->apm_controller;
}

const Service::APM::Controller& System::GetAPMController() const {
    return impl->apm_controller;
}

void System::SetExitLocked(bool locked) {
    impl->exit_locked = locked;
}

bool System::GetExitLocked() const {
    return impl->exit_locked;
}

void System::SetExitRequested(bool requested) {
    impl->exit_requested = requested;
}

bool System::GetExitRequested() const {
    return impl->exit_requested;
}

void System::SetApplicationProcessBuildID(const CurrentBuildProcessID& id) {
    impl->build_id = id;
}

const System::CurrentBuildProcessID& System::GetApplicationProcessBuildID() const {
    return impl->build_id;
}

Service::SM::ServiceManager& System::ServiceManager() {
    return *impl->service_manager;
}

const Service::SM::ServiceManager& System::ServiceManager() const {
    return *impl->service_manager;
}

void System::RegisterCoreThread(std::size_t id) {
    impl->kernel.RegisterCoreThread(id);
}

void System::RegisterHostThread() {
    impl->kernel.RegisterHostThread();
}

void System::EnterCPUProfile() {
    std::size_t core = impl->kernel.GetCurrentHostThreadID();
    impl->dynarmic_ticks[core] = MicroProfileEnter(impl->microprofile_cpu[core]);
}

void System::ExitCPUProfile() {
    std::size_t core = impl->kernel.GetCurrentHostThreadID();
    MicroProfileLeave(impl->microprofile_cpu[core], impl->dynarmic_ticks[core]);
}

bool System::IsMulticore() const {
    return impl->is_multicore;
}

bool System::DebuggerEnabled() const {
    //return Settings::values.use_gdbstub.GetValue();
    return false;
}

Core::Debugger& System::GetDebugger() {
    UNIMPLEMENTED();
    static Core::Debugger * ptr = nullptr;
    return *ptr;
}

const Core::Debugger& System::GetDebugger() const {
    UNIMPLEMENTED();
    static Core::Debugger * ptr = nullptr;
    return *ptr;
}

void System::RunServer(std::unique_ptr<Service::ServerManager>&& server_manager) {
    return impl->kernel.RunServer(std::move(server_manager));
}

void System::RegisterExecuteProgramCallback(ExecuteProgramCallback&& callback) {
    impl->execute_program_callback = std::move(callback);
}

void System::ExecuteProgram(std::size_t program_index) {
    if (impl->execute_program_callback) {
        impl->execute_program_callback(program_index);
    } else {
        LOG_CRITICAL(Core, "execute_program_callback must be initialized by the frontend");
    }
}

std::deque<std::vector<u8>>& System::GetUserChannel() {
    return impl->user_channel;
}


void System::Exit() {
    UNIMPLEMENTED();
}

} // namespace Core
