#include "switch_system.h"
#include "file_format/nacp.h"
#include "file_format/nro.h"
#include "settings/core_settings.h"
#include "settings/identifiers.h"
#include "settings/settings.h"

std::unique_ptr<SwitchSystem> SwitchSystem::m_instance;

bool SwitchSystem::Create(IRenderWindow & window)
{
    ShutDown();
    m_instance.reset(new SwitchSystem());
    if (m_instance == nullptr)
    {
        return false;
    }
    return m_instance->Initialize(window);
}

void SwitchSystem::ShutDown(void)
{
    if (m_instance)
    {
        m_instance.reset();
    }
}

SwitchSystem * SwitchSystem::GetInstance()
{
    return m_instance.get();
}

SwitchSystem::SwitchSystem() :
    m_emulationRunning(false)
{
}

SwitchSystem::~SwitchSystem()
{
    StopEmulation();
}

bool SwitchSystem::Initialize(IRenderWindow & window)
{
    if (!m_modules.Initialize(window, *this))
    {
        return false;
    }
    return true;
}

void SwitchSystem::StartEmulation(void)
{
    m_emulationRunning = true;
    m_modules.StartEmulation();
}

void SwitchSystem::StopEmulation(void)
{
    if (!m_emulationRunning)
    {
        return;
    }
    m_emulationRunning = false;
    m_modules.StopEmulation();
}

IVideo & SwitchSystem::Video(void)
{
    return *m_modules.Video();
}

IOperatingSystem & SwitchSystem::OperatingSystem()
{
    return *m_modules.OperatingSystem();
}

ICpu & SwitchSystem::Cpu(void)
{
    return *m_modules.Cpu();
}

bool SwitchSystem::LoadRom(const char * romFile)
{
    bool res = false;
    if (Nro::IsNroFile(romFile))
    {
        res = LoadNRO(romFile);
    }

    if (res)
    {
        Settings::GetInstance().SetString(NXCoreSetting::GameFile, romFile);
    }
    return res;
}

bool SwitchSystem::LoadNRO(const char * nroFile)
{
    Path filePath(nroFile);
    m_nro = std::make_unique<Nro>(filePath);
    if (!m_nro->Valid())
    {
        return false;
    }

    IOperatingSystem * operatingSystem = m_modules.OperatingSystem();
    const IProgramMetadata & metaData = m_nro->MetaData();
    uint64_t baseAddress = 0;
    if (!operatingSystem->CreateApplicationProcess(m_nro->CodeSize(), metaData, baseAddress))
    {
        return false;
    }

    if (!operatingSystem->LoadModule(*m_nro, baseAddress))
    {
        return false;
    }
    const NACP * Nacp = m_nro->Nacp();
    if (Nacp == nullptr)
    {
        return false;
    }
    Settings::GetInstance().SetString(NXCoreSetting::GameName, Nacp->GetApplicationName().c_str());
    operatingSystem->StartApplicationProcess(baseAddress, metaData.GetMainThreadPriority(), metaData.GetMainThreadStackSize());
    return true;
}

