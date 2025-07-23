#pragma once
#include "base.h"

__interface IArm64Reg
{
    // clang-format off
    enum class Reg
    {
        INVALID,
        W0, W1, W2, W3, W4, W5, W6, W7, W8, W9, W10, W11, W12, W13, W14, W15, W16, W17, W18, W19, W20, W21, W22, W23, W24, W25, W26, W27, W28, W29, W30, WZR,
        X0, X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, XZR,
        V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12, V13, V14, V15, V16, V17, V18, V19, V20, V21, V22, V23, V24, V25, V26, V27, V28, V29, V30, V31,
        B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, B11, B12, B13, B14, B15, B16, B17, B18, B19, B20, B21, B22, B23, B24, B25, B26, B27, B28, B29, B30, B31,
        H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15, H16, H17, H18, H19, H20, H21, H22, H23, H24, H25, H26, H27, H28, H29, H30, H31,
        S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16, S17, S18, S19, S20, S21, S22, S23, S24, S25, S26, S27, S28, S29, S30, S31,
        D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13, D14, D15, D16, D17, D18, D19, D20, D21, D22, D23, D24, D25, D26, D27, D28, D29, D30, D31,
        Q0, Q1, Q2, Q3, Q4, Q5, Q6, Q7, Q8, Q9, Q10, Q11, Q12, Q13, Q14, Q15, Q16, Q17, Q18, Q19, Q20, Q21, Q22, Q23, Q24, Q25, Q26, Q27, Q28, Q29, Q30, Q31,
        WSP, SP, PC, PSTATE, TPIDR_EL0, TPIDRRO_EL0,

        //Alias
        FP = X29,
        LR = X30,
    };
    // clang-format on

    uint32_t Get32(Reg reg) = 0;
    uint64_t Get64(Reg reg) = 0;
    void Get128(Reg reg, uint64_t & hiValue, uint64_t & loValue) = 0;
    uint32_t GetFPCR() const = 0;
    uint32_t GetFPSR() const = 0;

    void Set32(Reg reg, uint32_t value) = 0;
    void Set64(Reg reg, uint64_t value) = 0;
    void Set128(Reg reg, uint64_t hiValue, uint64_t loValue) = 0;

    void SetFPCR(uint32_t value) = 0;
    void SetFPSR(uint32_t value) = 0;
};

__interface IArm64Executor
{
    enum class HaltReason
    {
        Stopped,
        SupervisorCall,
        SupervisorCallBreakLoop,
        BreakLoop,
    };

    IArm64Reg & Reg(void) = 0;
    HaltReason Execute(void) = 0;
    void InvalidateCacheRange(uint64_t addr, uint64_t size) = 0;
    void HaltExecution(HaltReason hr) = 0;
};

__interface IMemory
{
    void RasterizerMarkRegionCached(uint64_t vaddr, uint64_t size, bool cached) = 0;
    uint8_t * GetPointerSilent(uint64_t vaddr) = 0;

    uint8_t Read8(uint64_t addr) = 0;
    uint16_t Read16(uint64_t addr) = 0;
    uint32_t Read32(uint64_t addr) = 0;
    uint64_t Read64(uint64_t addr) = 0;

    bool WriteExclusive8(uint64_t addr, uint8_t data, uint8_t expected) = 0;
    bool WriteExclusive16(uint64_t addr, uint16_t data, uint16_t expected) = 0;
    bool WriteExclusive32(uint64_t addr, uint32_t data, uint32_t expected) = 0;
    bool WriteExclusive64(uint64_t addr, uint64_t data, uint64_t expected) = 0;
};

__interface ICpuInfo
{
    uint64_t CpuTicks() = 0;
    void ServiceCall(uint32_t index) = 0;
    bool ReadMemory(uint64_t addr, uint8_t * buffer, uint32_t len) = 0;
    bool WriteMemory(uint64_t addr, const uint8_t * buffer, uint32_t len) = 0;
};

__interface IExclusiveMonitor
{
    uint8_t ExclusiveRead8(uint32_t coreIndex, uint64_t addr) = 0;
    uint16_t ExclusiveRead16(uint32_t coreIndex, uint64_t addr) = 0;
    uint32_t ExclusiveRead32(uint32_t coreIndex, uint64_t addr) = 0;
    uint64_t ExclusiveRead64(uint32_t coreIndex, uint64_t addr) = 0;
    void ClearExclusive(uint32_t coreIndex) = 0;

    bool ExclusiveWrite8(uint32_t coreIndex, uint64_t addr, uint8_t value) = 0;
    bool ExclusiveWrite16(uint32_t coreIndex, uint64_t addr, uint16_t value) = 0;
    bool ExclusiveWrite32(uint32_t coreIndex, uint64_t addr, uint32_t value) = 0;
    bool ExclusiveWrite64(uint32_t coreIndex, uint64_t addr, uint64_t value) = 0;
};

__interface ICpu
{
    bool Initialize(void) = 0;

    IExclusiveMonitor * CreateExclusiveMonitor(IMemory & memory, uint32_t processorCount) = 0;
    void DestroyExclusiveMonitor(IExclusiveMonitor * monitor) = 0;

    IArm64Executor * CreateArm64Executor(IExclusiveMonitor * monitor, ICpuInfo & info, uint32_t coreIndex) = 0;
    void DestroyArm64Executor(IArm64Executor * executor) = 0;
};

EXPORT ICpu * CALL CreateCpu(ISwitchSystem & System);
EXPORT void CALL DestroyCpu(ICpu * Cpu);
