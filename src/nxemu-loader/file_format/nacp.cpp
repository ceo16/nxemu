#include "nacp.h"
#include <common/file.h>

NACP::NACP(void) :
    m_info({0})
{
}

bool NACP::Load(File & file, uint64_t offset, uint64_t fileOffset, uint64_t fileSize)
{
    if (!file.IsOpen() || fileSize < sizeof(m_info))
    {
        return false;
    }
    if (file.Seek(offset + fileOffset, IFile::SeekPosition::begin) < 0)
    {
        return false;
    }
    if (file.Read(&m_info, sizeof(m_info)) != sizeof(m_info))
    {
        return false;
    }
    return true;
}

std::string NACP::GetApplicationName(Language language) const
{
    const LanguageEntry & entry = GetLanguageEntry(language);
    uint32_t len = 0, maxLen = sizeof(LanguageEntry::ApplicationName) / sizeof(LanguageEntry::ApplicationName[0]);
    while (len < maxLen)
    {
        if (entry.ApplicationName[len] == '\0')
        {
            break;
        }
        len += 1;
    }
    return std::string((const char *)&entry.ApplicationName[0], len);
}

uint64_t NACP::GetTitleId(void) const
{
    return m_info.PresenceGroupId;
}

const NACP::LanguageEntry & NACP::GetLanguageEntry(Language language) const
{
    if (language != Language::Default)
    {
        return m_info.LanguageEntries[language];
    }

    for (uint32_t i = 0, n = sizeof(NACPInfo::LanguageEntries) / sizeof(NACPInfo::LanguageEntries[0]); i < n; i++)
    {
        const LanguageEntry & entry = m_info.LanguageEntries[i];
        if (entry.ApplicationName[0] != '\0')
        {
            return entry;
        }
    }

    return GetLanguageEntry(Language::AmericanEnglish);
}
