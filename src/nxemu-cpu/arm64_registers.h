#pragma once
#include <nxemu-module-spec/cpu.h>


class ArmDynarmic64;

namespace Dynarmic::A64
{
class Jit;
}

class A64Registers :
    public IArm64Reg
{
    friend ArmDynarmic64;

public:
    A64Registers();

    uint32_t Get32(IArm64Reg::Reg reg);
    uint64_t Get64(IArm64Reg::Reg reg);
    void Get128(IArm64Reg::Reg reg, uint64_t & hiValue, uint64_t & loValue);
    uint32_t GetFPCR() const;
    uint32_t GetFPSR() const;

    void Set32(IArm64Reg::Reg reg, uint32_t value);
    void Set64(IArm64Reg::Reg reg, uint64_t value);
    void Set128(IArm64Reg::Reg reg, uint64_t hiValue, uint64_t loValue);
    void SetFPCR(uint32_t value);
    void SetFPSR(uint32_t value);

    void SetJit(Dynarmic::A64::Jit * jit);

private:
    A64Registers(const A64Registers&) = delete;
    A64Registers& operator=(const A64Registers&) = delete;

    Dynarmic::A64::Jit * m_jit;

    //CP15
    uint64_t m_tpidr_el0; /* User RW Thread register.  */
    uint64_t m_tpidrro_el0; /* User RO Thread register.  */
};
