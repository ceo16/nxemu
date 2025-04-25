#pragma once
#include <common/padding.h>
#include <memory>
#include <nxemu-module-spec/operating_system.h>
#include <stdint.h>
#include <vector>

class NACP;

__interface IProgramMetadata;

class Nro :
    public IModuleInfo
{
    typedef struct
    {
        PADDING_BYTES(0x4);
        uint32_t ModuleOffset;
        PADDING_BYTES(0x8);
        uint8_t Signature[4];
        uint32_t Version;
        uint32_t Size;
        uint32_t Flags;
        uint32_t TextMemoryOffset;
        uint32_t TextSize;
        uint32_t RoMemoryOffset;
        uint32_t RoSize;
        uint32_t DataMemoryOffset;
        uint32_t DataSize;
        uint32_t BssSize;
        uint32_t Reserved;
        uint8_t ModuleId[0x20];
        uint32_t DsoHandleOffset;
        uint32_t Reserved1;
        uint32_t EmbeddedOffset;
        uint32_t EmbeddedSize;
        uint32_t DynStrOffset;
        uint32_t DynStrSize;
        uint32_t DynSymOffset;
        uint32_t DynSymSize;
    } NRO_HEADER;
    static_assert(sizeof(NRO_HEADER) == 0x80, "NRO_HEADER has incorrect size.");

    typedef struct
    {
        uint8_t Signature[4];
        uint32_t DynamicOffset;
        uint32_t BssStartOffset;
        uint32_t BssEndOffset;
        uint32_t ExceptionInfoStartOffset;
        uint32_t ExceptionInfoEndOffset;
        uint32_t ModuleOffset;
    } NRO_MODULE_HEADER;
    static_assert(sizeof(NRO_MODULE_HEADER) == 0x1c, "NRO_MODULE_HEADER has incorrect size.");

public:
    Nro(const char * filePath);
    ~Nro() = default;

    //IModuleInfo
    const uint8_t * Data(void) const;
    uint32_t DataSize(void) const;
    uint64_t CodeSegmentAddr(void) const;
    uint64_t CodeSegmentOffset(void) const;
    uint64_t CodeSegmentSize(void) const;
    uint64_t RODataSegmentAddr(void) const;
    uint64_t RODataSegmentOffset(void) const;
    uint64_t RODataSegmentSize(void) const;
    uint64_t DataSegmentAddr(void) const;
    uint64_t DataSegmentOffset(void) const;
    uint64_t DataSegmentSize(void) const;

    NACP * Nacp() const;
    uint32_t CodeSize();
    const IProgramMetadata & MetaData() const;
    bool Valid() const;

    static bool IsNroFile(const char * filePath);

private:
    Nro(void) = delete;
    Nro(const Nro &) = delete;
    Nro & operator=(const Nro &) = delete;

    bool HasModHeader() const;

    static constexpr uint32_t PageAlignSize(uint32_t size);

    std::vector<uint8_t> m_programImage;
    NRO_HEADER m_header;
    NRO_MODULE_HEADER m_moduleHeader;
    std::unique_ptr<NACP> m_nacp;
    uint32_t m_bssSize;
    bool m_valid;
};
