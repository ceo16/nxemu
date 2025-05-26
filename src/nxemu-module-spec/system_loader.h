#pragma once
#include "base.h"

enum class LoaderTitleType : uint8_t {
    SystemProgram = 0x01,
    SystemDataArchive = 0x02,
    SystemUpdate = 0x03,
    FirmwarePackageA = 0x04,
    FirmwarePackageB = 0x05,
    Application = 0x80,
    Update = 0x81,
    AOC = 0x82,
    DeltaTitle = 0x83,
};

enum class LoaderContentRecordType : uint8_t {
    Meta = 0,
    Program = 1,
    Data = 2,
    Control = 3,
    HtmlDocument = 4,
    LegalInformation = 5,
    DeltaFragment = 6,
};

enum class LoaderResultStatus : uint16_t {
    Success,
    ErrorAlreadyLoaded,
    ErrorNotImplemented,
    ErrorNotInitialized,
    ErrorBadNPDMHeader,
    ErrorBadACIDHeader,
    ErrorBadACIHeader,
    ErrorBadFileAccessControl,
    ErrorBadFileAccessHeader,
    ErrorBadKernelCapabilityDescriptors,
    ErrorBadPFSHeader,
    ErrorIncorrectPFSFileSize,
    ErrorBadNCAHeader,
    ErrorMissingProductionKeyFile,
    ErrorMissingHeaderKey,
    ErrorIncorrectHeaderKey,
    ErrorNCA2,
    ErrorNCA0,
    ErrorMissingTitlekey,
    ErrorMissingTitlekek,
    ErrorInvalidRightsID,
    ErrorMissingKeyAreaKey,
    ErrorIncorrectKeyAreaKey,
    ErrorIncorrectTitlekeyOrTitlekek,
    ErrorXCIMissingProgramNCA,
    ErrorNCANotProgram,
    ErrorNoExeFS,
    ErrorBadXCIHeader,
    ErrorXCIMissingPartition,
    ErrorNullFile,
    ErrorMissingNPDM,
    Error32BitISA,
    ErrorUnableToParseKernelMetadata,
    ErrorNoRomFS,
    ErrorIncorrectELFFileSize,
    ErrorLoadingNRO,
    ErrorLoadingNSO,
    ErrorNoIcon,
    ErrorNoControl,
    ErrorBadNAXHeader,
    ErrorIncorrectNAXFileSize,
    ErrorNAXKeyHMACFailed,
    ErrorNAXValidationHMACFailed,
    ErrorNAXKeyDerivationFailed,
    ErrorNAXInconvertibleToNCA,
    ErrorBadNAXFilePath,
    ErrorMissingSDSeed,
    ErrorMissingSDKEKSource,
    ErrorMissingAESKEKGenerationSource,
    ErrorMissingAESKeyGenerationSource,
    ErrorMissingSDSaveKeySource,
    ErrorMissingSDNCAKeySource,
    ErrorNSPMissingProgramNCA,
    ErrorBadBKTRHeader,
    ErrorBKTRSubsectionNotAfterRelocation,
    ErrorBKTRSubsectionNotAtEnd,
    ErrorBadRelocationBlock,
    ErrorBadSubsectionBlock,
    ErrorBadRelocationBuckets,
    ErrorBadSubsectionBuckets,
    ErrorMissingBKTRBaseRomFS,
    ErrorNoPackedUpdate,
    ErrorBadKIPHeader,
    ErrorBLZDecompressionFailed,
    ErrorBadINIHeader,
    ErrorINITooManyKIPs,
    ErrorIntegrityVerificationNotImplemented,
    ErrorIntegrityVerificationFailed,
};

struct ContentProviderEntry
{
    uint64_t titleID;
    LoaderContentRecordType type;
};

__interface IVirtualFile;

__interface IVirtualDirectory
{
    IVirtualFile * GetFile(const char * name) const = 0;
    IVirtualFile * GetFileRelative(const char * relative_path) const = 0;
    void Release() = 0;
};

__interface IVirtualFile
{
    uint64_t GetSize() const = 0;
    uint64_t ReadBytes(uint8_t * data, uint64_t datalen, uint64_t offset) = 0;
    IVirtualDirectory * ExtractRomFS() = 0;
    IVirtualFile * Duplicate() = 0;
    void Release() = 0;
};

__interface ISaveDataFactory
{
    void Release() = 0;
};

__interface IRomFsController
{
    IVirtualFile * OpenCurrentProcess(uint64_t currentProcessTitleId) const = 0;
    void Release() = 0;
};

__interface IFileSysNACP
{
    uint32_t GetParentalControlFlag() const = 0;
    bool GetRatingAge(uint8_t * buffer, uint32_t bufferSize) const = 0;
    void Release() = 0;
};

__interface IFileSysNCA
{
    LoaderResultStatus GetStatus() const = 0;
    IVirtualFile * GetRomFS() = 0;
    void Release() = 0;
};

__interface IFileSysRegisteredCache
{
    IFileSysNCA * GetEntry(uint64_t title_id, LoaderContentRecordType type) const = 0;
};

__interface IFileSystemController
{
    IFileSysRegisteredCache * GetSystemNANDContents() const = 0;
    bool OpenProcess(uint64_t * programId, ISaveDataFactory ** saveDataFactory, IRomFsController ** romFsController, uint64_t processId) = 0;
    bool OpenSDMC(IVirtualDirectory** out_sdmc) const = 0;
};

__interface ISystemloader
{
    bool Initialize() = 0;
    bool SelectAndLoad(void * parentWindow) = 0;
    bool LoadRom(const char * romFile) = 0;

    IFileSystemController & FileSystemController() = 0;
    IVirtualFile * SynthesizeSystemArchive(const uint64_t title_id) = 0;
    uint32_t GetContentProviderEntriesCount(bool useTitleType, LoaderTitleType titleType, bool useContentRecordType, LoaderContentRecordType contentRecordType, bool useTitleId, unsigned long long titleId) = 0;
    uint32_t GetContentProviderEntries(bool useTitleType, LoaderTitleType titleType, bool useContentRecordType, LoaderContentRecordType contentRecordType, bool useTitleId, unsigned long long titleId, ContentProviderEntry* entries, uint32_t entryCount) = 0;
    IFileSysNCA * GetContentProviderEntry(uint64_t title_id, LoaderContentRecordType type) = 0;
    IFileSysNACP * GetPMControlMetadata(uint64_t programID) = 0;
};

EXPORT ISystemloader * CALL CreateSystemLoader(ISwitchSystem & system);
EXPORT void CALL DestroySystemLoader(ISystemloader * systemloader);
