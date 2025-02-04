#include "cpu_module.h"
#include "common/assert.h"

namespace Core
{

ArmCpuModule::ArmCpuModule(Core::System & system, bool is64Bit, bool usesWallClock, Kernel::KProcess * process, uint32_t coreIndex) :
    ArmInterface{usesWallClock}
{
}

ArmCpuModule::~ArmCpuModule()
{
}

Architecture ArmCpuModule::GetArchitecture() const
{
    UNIMPLEMENTED();
    return Architecture::AArch64;
}

HaltReason ArmCpuModule::RunThread(Kernel::KThread * thread)
{
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
