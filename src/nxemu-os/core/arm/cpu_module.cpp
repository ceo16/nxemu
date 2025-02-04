#include "cpu_module.h"
#include "core/hle/kernel/k_process.h"
#include <nxemu-module-spec/cpu.h>

namespace Core
{
class CpuModuleCallback : public ICpuInfo
{
public:
    explicit CpuModuleCallback(IArm64Executor *& arm64Executor, Core::System & system, Kernel::KProcess * process) :
        m_arm64Executor(arm64Executor),
        m_system(system),
        m_process(process),
        m_memory(process->GetMemory()),
        m_svn(0)
    {
    }


    IArm64Executor *& m_arm64Executor;
    Kernel::KProcess * m_process{};
    Core::System & m_system;
    Core::Memory::Memory & m_memory;
    uint32_t m_svn;
};

ArmCpuModule::ArmCpuModule(Core::System & system, bool is64Bit, bool usesWallClock, Kernel::KProcess * process, uint32_t coreIndex) :
    ArmInterface{usesWallClock},
    m_system(system),
    m_cb(std::make_unique<CpuModuleCallback>(m_arm64Executor, system, process)),
    m_arm64Executor(nullptr)
{
    if (is64Bit)
    {
        m_arm64Executor = system.GetSwitchSystem().Cpu().CreateArm64Executor(process->GetExclusiveMonitor(), *m_cb, coreIndex);    
    }
}

ArmCpuModule::~ArmCpuModule()
{
    if (m_arm64Executor != nullptr)
    {
        m_system.GetSwitchSystem().Cpu().DestroyArm64Executor(m_arm64Executor);
        m_arm64Executor = nullptr;
    }
}

Architecture ArmCpuModule::GetArchitecture() const
{
    UNIMPLEMENTED();
    return Architecture::AArch64;
}

HaltReason ArmCpuModule::RunThread(Kernel::KThread * thread)
{
    if (m_arm64Executor != nullptr)
    {
        IArm64Executor::HaltReason reason = m_arm64Executor->Execute();
        switch (reason)
        {
        case IArm64Executor::HaltReason::SupervisorCall: return HaltReason::SupervisorCall;
        }
    }
    UNIMPLEMENTED();
    return HaltReason::DataAbort;
}

HaltReason ArmCpuModule::StepThread(Kernel::KThread * thread)
{
    UNIMPLEMENTED();
    return HaltReason::DataAbort;
}

void ArmCpuModule::GetContext(Kernel::Svc::ThreadContext & ctx) const
{
    UNIMPLEMENTED();
}

void ArmCpuModule::SetContext(const Kernel::Svc::ThreadContext & ctx)
{
    UNIMPLEMENTED();
}

void ArmCpuModule::SetTpidrroEl0(u64 value)
{
    UNIMPLEMENTED();
}

void ArmCpuModule::GetSvcArguments(std::span<uint64_t, 8> args) const
{
    UNIMPLEMENTED();
}

void ArmCpuModule::SetSvcArguments(std::span<const uint64_t, 8> args)
{
    UNIMPLEMENTED();
}

u32 ArmCpuModule::GetSvcNumber() const
{
    UNIMPLEMENTED();
    return 0;
}

void ArmCpuModule::SignalInterrupt(Kernel::KThread* thread)
{
    UNIMPLEMENTED();
}

void ArmCpuModule::ClearInstructionCache()
{
    UNIMPLEMENTED();
}

void ArmCpuModule::InvalidateCacheRange(u64 addr, std::size_t size)
{
    UNIMPLEMENTED();
}

const Kernel::DebugWatchpoint * ArmCpuModule::HaltedWatchpoint() const
{
    UNIMPLEMENTED();
    return nullptr;
}

void ArmCpuModule::RewindBreakpointInstruction()
{
    UNIMPLEMENTED();
}

}
