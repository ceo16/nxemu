#pragma once
#include <stdint.h>
#include <string>

class File;

class NACP
{
    struct LanguageEntry
    {
        uint8_t ApplicationName[0x200];
        uint8_t DeveloperName[0x100];
    };

#pragma pack(push, 1)
    struct NACPInfo
    {
        LanguageEntry LanguageEntries[0x10];
        uint8_t Isbn[0x25];
        uint8_t StartupUserAccount;
        uint8_t UserAccountSwitchLock;
        uint8_t AddOnContentRegistrationType;
        uint32_t AttributeFlag;
        uint32_t SupportedLanguageFlag;
        uint32_t ParentalControlFlag;
        bool Screenshot;
        uint8_t VideoCapture;
        bool DataLossConfirmation;
        uint8_t PlayLogPolicy;
        uint64_t PresenceGroupId;
        uint8_t RatingAge[0x20];
        uint8_t DisplayVersion[0x10];
        uint64_t AddOnContentBaseId;
        uint64_t SaveDataOwnerId;
        uint64_t UserAccountSaveDataSize;
        uint64_t UserAccountSaveDataJournalSize;
        uint64_t DeviceSaveDataSize;
        uint64_t DeviceSaveDataJournalSize;
        uint64_t BcatDeliveryCacheStorageSize;
        uint64_t ApplicationErrorCodeCategory;
        uint64_t LocalCommunicationId[0x8];
        uint8_t LogoType;
        uint8_t LogoHandling;
        uint8_t RuntimeAddOnContentInstall;
        uint8_t RuntimeParameterDelivery;
        uint8_t AppropriateAgeForChina;
        uint8_t Reserved;
        uint8_t CrashReport;
        uint8_t Hdcp;
        uint64_t SeedForPseudoDeviceId;
        uint8_t BcatPassphrase[0x41];
        uint8_t StartupUserAccountOption;
        uint8_t ReservedForUserAccountSaveDataOperation[0x6];
        uint64_t UserAccountSaveDataSizeMax;
        uint64_t UserAccountSaveDataJournalSizeMax;
        uint64_t DeviceSaveDataSizeMax;
        uint64_t DeviceSaveDataJournalSizeMax;
        uint64_t TemporaryStorageSize;
        uint64_t CacheStorageSize;
        uint64_t CacheStorageJournalSize;
        uint64_t CacheStorageDataAndJournalSizeMax;
        uint8_t CacheStorageIndexMax[2];
        uint8_t Reserved1;
        uint8_t RuntimeUpgrade;
        uint32_t SupportingLimitedLicenses;
        uint8_t PlayLogQueryableApplicationId[8][0x10];
        uint8_t PlayLogQueryCapability;
        uint8_t RepairFlag;
        uint8_t ProgramIndex;
        uint8_t RequiredNetworkServiceLicenseOnLaunchFlag;
        uint32_t Reserved2;
        uint8_t NeighborDetectionClientConfiguration[0x198];
        uint8_t JitConfiguration[0x10];
        uint8_t RequiredAddOnContentsSetBinaryDescriptor[2][0x20];
        uint8_t PlayReportPermission;
        uint8_t CrashScreenshotForProd;
        uint8_t CrashScreenshotForDev;
        uint8_t ContentsAvailabilityTransitionPolicy;
        uint32_t Reserved3;
        uint8_t AccessibleLaunchRequiredVersion[8][8];
        uint8_t Reserved4[0xBB8];
#pragma pack(pop)
    };
    static_assert(sizeof(NACPInfo) == 0x4000, "NACPInfo has incorrect size.");

public:
    enum Language : uint8_t
    {
        AmericanEnglish = 0,
        BritishEnglish = 1,
        Japanese = 2,
        French = 3,
        German = 4,
        LatinAmericanSpanish = 5,
        Spanish = 6,
        Italian = 7,
        Dutch = 8,
        CanadianFrench = 9,
        Portugese = 10,
        Russian = 11,
        Korean = 12,
        Taiwanese = 13,
        Chinese = 14,

        Default = 255,
    };

    NACP();
    ~NACP() = default;

    bool Load(File & file, uint64_t offset, uint64_t fileOffset, uint64_t fileSize);
    std::string GetApplicationName(Language language = Language::Default) const;
    uint64_t GetTitleId(void) const;

private:
    NACP(const NACP &) = delete;
    NACP & operator=(const NACP &) = delete;

    const LanguageEntry & GetLanguageEntry(Language language) const;

    NACPInfo m_info;
};