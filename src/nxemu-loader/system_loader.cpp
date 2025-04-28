#include "system_loader.h"
#include "file_format/nro.h"
#include "file_format/nacp.h"
#include <common/path.h>
#include <nxemu-core/settings/identifiers.h>

extern IModuleSettings * g_settings;

Systemloader::Systemloader(ISwitchSystem & system) :
    m_system(system),
    m_TitleID(0)
{
}

Systemloader::~Systemloader()
{
}

bool Systemloader::SelectAndLoad(void * parentWindow)
{
    Path fileToOpen;
    std::string fileName;
    const char * filter = "Switch Files (*.nro)\0*.nro\0All files (*.*)\0*.*\0";
    if (fileToOpen.FileSelect(parentWindow, Path(Path::MODULE_DIRECTORY), filter, true))
    {
        fileName = (const std::string &)fileToOpen;
    }
    if (fileName.length() == 0)
    {
        return false;
    }
    return LoadRom(fileName.c_str());
}

bool Systemloader::LoadRom(const char * romFile)
{
    bool res = false;
    g_settings->SetBool(NXCoreSetting::RomLoading, true);
    if (Nro::IsNroFile(romFile))
    {
        res = LoadNRO(romFile);
    }

    g_settings->SetBool(NXCoreSetting::RomLoading, false);
    if (res)
    {
        g_settings->SetString(NXCoreSetting::GameFile, romFile);
        m_system.StartEmulation();
    }
    return res;
}

bool Systemloader::LoadNRO(const char * nroFile)
{
    Path filePath(nroFile);
    m_nro = std::make_unique<Nro>(filePath);
    if (!m_nro->Valid())
    {
        return false;
    }

    IOperatingSystem & operatingSystem = m_system.OperatingSystem();
    const IProgramMetadata & metaData = m_nro->MetaData();
    uint64_t baseAddress = 0;
    if (!operatingSystem.CreateApplicationProcess(m_nro->CodeSize(), metaData, baseAddress))
    {
        return false;
    }

    if (!operatingSystem.LoadModule(*m_nro, baseAddress))
    {
        return false;
    }
    const NACP * Nacp = m_nro->Nacp();
    if (Nacp == nullptr)
    {
        return false;
    }

    m_TitleID = Nacp->GetTitleId();
    operatingSystem.RegisterProcessVirtualFile(m_TitleID, /*file,*/ false);
    g_settings->SetString(NXCoreSetting::GameName, Nacp->GetApplicationName().c_str());
    operatingSystem.StartApplicationProcess(baseAddress, metaData.GetMainThreadPriority(), metaData.GetMainThreadStackSize());
    return true;
}
