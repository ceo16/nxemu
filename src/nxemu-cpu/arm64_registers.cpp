#include "arm64_registers.h"
#include "dynarmic/interface/A64/a64.h"

extern IModuleNotification * g_notify;

A64Registers::A64Registers() :
    m_tpidr_el0(0),
    m_tpidrro_el0(0),
    m_jit(nullptr)
{
}

uint32_t A64Registers::Get32(IArm64Reg::Reg reg)
{
    if (reg >= IArm64Reg::Reg::W0 && reg <= IArm64Reg::Reg::W30)
    {
        return (uint32_t)m_jit->GetRegister((uint32_t)reg - (uint32_t)IArm64Reg::Reg::W0);
    }
    if (reg == IArm64Reg::Reg::WZR)
    {
        return 0;
    }
    if (reg >= IArm64Reg::Reg::PSTATE)
    {
        return m_jit->GetPstate();
    }
    g_notify->BreakPoint(__FILE__, __LINE__);
    return 0;
}

uint64_t A64Registers::Get64(IArm64Reg::Reg reg)
{
    if (reg >= IArm64Reg::Reg::X0 && reg <= IArm64Reg::Reg::X30)
    {
        return m_jit->GetRegister((uint32_t)reg - (uint32_t)IArm64Reg::Reg::X0);
    }
    if (reg == IArm64Reg::Reg::SP)
    {
        return m_jit->GetSP();
    }
    if (reg == IArm64Reg::Reg::XZR)
    {
        return 0;
    }
    if (reg == IArm64Reg::Reg::PC)
    {
        return m_jit->GetPC();
    }
    if (reg == IArm64Reg::Reg::TPIDR_EL0)
    {
        return m_tpidr_el0;
    }
    if (reg == IArm64Reg::Reg::TPIDRRO_EL0)
    {
        return m_tpidrro_el0;
    }
    g_notify->BreakPoint(__FILE__, __LINE__);
    return 0;
}

void A64Registers::Get128(IArm64Reg::Reg reg, uint64_t & hiValue, uint64_t & loValue)
{
    if (reg >= IArm64Reg::Reg::Q0 && reg <= IArm64Reg::Reg::Q31)
    {
        Dynarmic::A64::Vector v = m_jit->GetVector(((uint32_t)reg - (uint32_t)IArm64Reg::Reg::Q0));
        hiValue = v[0];
        loValue = v[1];
    }
    else
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
    }
}

void A64Registers::Set32(IArm64Reg::Reg reg, uint32_t value)
{
    if (reg >= IArm64Reg::Reg::W0 && reg <= IArm64Reg::Reg::W30)
    {
        m_jit->SetRegister((uint32_t)reg - (uint32_t)IArm64Reg::Reg::W0, value);
    }
    else if (reg == IArm64Reg::Reg::PSTATE)
    {
        m_jit->SetPstate(value);
    }
    else
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
    }
}

void A64Registers::Set64(IArm64Reg::Reg reg, uint64_t value)
{
    if (reg >= IArm64Reg::Reg::X0 && reg <= IArm64Reg::Reg::X30)
    {
        m_jit->SetRegister((uint32_t)reg - (uint32_t)IArm64Reg::Reg::X0, value);
    }
    else if (reg == IArm64Reg::Reg::SP)
    {
        m_jit->SetSP(value);
    }
    else if (reg == IArm64Reg::Reg::PC)
    {
        m_jit->SetPC(value);
    }
    else if (reg == IArm64Reg::Reg::TPIDRRO_EL0)
    {
        m_tpidrro_el0 = value;
    }
    else if (reg == IArm64Reg::Reg::TPIDR_EL0)
    {
        m_tpidr_el0 = value;
    }
    else
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
    }
}

void A64Registers::Set128(IArm64Reg::Reg reg, uint64_t hiValue, uint64_t loValue)
{
    if (reg >= IArm64Reg::Reg::Q0 && reg <= IArm64Reg::Reg::Q31)
    {
        m_jit->SetVector(((uint32_t)reg - (uint32_t)IArm64Reg::Reg::Q0), {hiValue, loValue});
    }
    else if (reg >= IArm64Reg::Reg::V0 && reg <= IArm64Reg::Reg::V31)
    {
        m_jit->SetVector(((uint32_t)reg - (uint32_t)IArm64Reg::Reg::V0), {hiValue, loValue});
    }
    else
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
    }
}

void A64Registers::SetFPCR(uint32_t value)
{
    m_jit->SetFpcr(value);
}

void A64Registers::SetFPSR(uint32_t value)
{
    m_jit->SetFpsr(value);
}

uint32_t A64Registers::GetFPCR() const
{
    return m_jit->GetFpcr();
}

uint32_t A64Registers::GetFPSR() const
{
    return m_jit->GetFpsr();
}

void A64Registers::SetJit(Dynarmic::A64::Jit * jit)
{
    m_jit = jit;
}
