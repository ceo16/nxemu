#include <nxemu-core/settings/identifiers.h>
#include "core/cpu_manager.h"
#include "core/hle/kernel/k_process.h"
#include "core/hle/service/am/applet_manager.h"
#include "yuzu_common/logging/backend.h"
#include "os_manager.h"

extern IModuleSettings * g_settings;

OSManager::OSManager(ISwitchSystem & switchSystem) :
    m_switchSystem(switchSystem),
    m_coreSystem(switchSystem),
    m_process(nullptr)
{
}

OSManager::~OSManager()
{
    if (m_process != nullptr)
    {
        m_process->Close();
        m_process = nullptr;
    }
    Common::Log::Stop();
}

void OSManager::EmulationStarting()
{
    m_coreSystem.GetCpuManager().OnGpuReady();
}

bool OSManager::Initialize(void)
{
    Common::Log::Initialize();
    Common::Log::Start();
    Common::Log::SetColorConsoleBackendEnabled(g_settings->GetBool(NXCoreSetting::ShowConsole));
    m_coreSystem.Initialize();
    return true;
}

bool OSManager::CreateApplicationProcess(uint64_t codeSize, const IProgramMetadata & metaData, uint64_t & baseAddress)
{
    if (m_process != nullptr)
    {
       return false;
    }
    m_coreSystem.Run();
    m_coreSystem.InitializeKernel();
    Kernel::KernelCore & kernel = m_coreSystem.Kernel();
    m_process = Kernel::KProcess::Create(kernel);
    if (m_process == nullptr)
    {
        return false;
    }
    Kernel::KProcess::Register(kernel, m_process);
    kernel.AppendNewProcess(m_process);
    kernel.MakeApplicationProcess(m_process);

    if (m_process->LoadFromMetadata(metaData, codeSize, 0, false).IsError())
    {
        return false;
    }
    
    auto params = Service::AM::FrontendAppletParameters{
        .applet_id = Service::AM::AppletId::Application,
        .applet_type = Service::AM::AppletType::Application,
    };
    params.program_id = metaData.GetTitleID();
    m_coreSystem.GetAppletManager().CreateAndInsertByFrontendAppletParameters(m_process->GetProcessId(), params);

    baseAddress = GetInteger(m_process->GetEntryPoint());
    return true;
}

void OSManager::StartApplicationProcess(uint64_t /*baseAddress*/, int32_t priority, int64_t stackSize)
{
    m_process->Run(priority, stackSize);
}

bool OSManager::LoadModule(const IModuleInfo & module, uint64_t baseAddress)
{
    if (m_process == nullptr)
    {
        return false;
    }
    m_process->LoadModule(module, baseAddress);
    return true;
}

