#include <nxemu-core/settings/identifiers.h>
#include "core/cpu_manager.h"
#include "core/file_sys/romfs_factory.h"
#include "core/hle/kernel/k_process.h"
#include "core/hle/service/am/applet_manager.h"
#include "core/hle/service/filesystem/filesystem.h"
#include "yuzu_common/logging/backend.h"
#include "yuzu_common/settings.h"
#include "yuzu_common/settings_input.h"
#include "yuzu_hid_core/hid_core.h"
#include "yuzu_input_common/main.h"
#include "yuzu_input_common/drivers/keyboard.h"
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

    auto & player = Settings::values.players.GetValue()[0];
    player.connected = true;
    InputSettings::ButtonsRaw & buttons = player.buttons;
    buttons[0x00000000] = "engine:keyboard,code:67,toggle:0";
    buttons[0x00000001] = "engine:keyboard,code:88,toggle:0";
    buttons[0x00000002] = "engine:keyboard,code:86,toggle:0";
    buttons[0x00000003] = "engine:keyboard,code:90,toggle:0";
    buttons[0x00000004] = "engine:keyboard,code:70,toggle:0";
    buttons[0x00000005] = "engine:keyboard,code:71,toggle:0";
    buttons[0x00000006] = "engine:keyboard,code:81,toggle:0";
    buttons[0x00000007] = "engine:keyboard,code:69,toggle:0";
    buttons[0x00000008] = "engine:keyboard,code:82,toggle:0";
    buttons[0x00000009] = "engine:keyboard,code:84,toggle:0";
    buttons[0x0000000a] = "engine:keyboard,code:77,toggle:0";
    buttons[0x0000000b] = "engine:keyboard,code:78,toggle:0";
    buttons[InputSettings::NativeButton::DLeft] = "engine:keyboard,code:37,toggle:0";
    buttons[InputSettings::NativeButton::DUp] = "engine:keyboard,code:38,toggle:0";
    buttons[InputSettings::NativeButton::DRight] = "engine:keyboard,code:39,toggle:0";
    buttons[InputSettings::NativeButton::DDown] = "engine:keyboard,code:40,toggle:0";
    buttons[0x00000010] = "engine:keyboard,code:81,toggle:0";
    buttons[0x00000011] = "engine:keyboard,code:69,toggle:0";
    buttons[0x00000012] = "engine:keyboard,code:0,toggle:0";
    buttons[0x00000013] = "engine:keyboard,code:0,toggle:0";
    buttons[0x00000014] = "engine:keyboard,code:81,toggle:0";
    buttons[0x00000015] = "engine:keyboard,code:69,toggle:0";

    m_coreSystem.Initialize();
    m_coreSystem.HIDCore().ReloadInputDevices();
    return true;
}

bool OSManager::CreateApplicationProcess(uint64_t codeSize, const IProgramMetadata & metaData, uint64_t & baseAddress)
{
    if (m_process != nullptr)
    {
       return false;
    }
    m_coreSystem.Run();
    m_coreSystem.InitializeKernel(metaData.GetTitleID());
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

void OSManager::RegisterProcessVirtualFile(uint64_t programId, /*IVirtualFile & file,*/ bool updatable)
{
    FileSys::VirtualFile file;
    m_coreSystem.GetFileSystemController().RegisterProcess(m_process->GetProcessId(), programId, std::make_unique<FileSys::RomFSFactory>(file, updatable, m_coreSystem.GetContentProvider(), m_coreSystem.GetFileSystemController()));
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

IDeviceMemory & OSManager::DeviceMemory(void)
{
    return m_coreSystem.DeviceMemory();
}

void OSManager::KeyboardKeyPress(int modifier, int keyIndex, int keyCode)
{
    std::shared_ptr<InputCommon::InputSubsystem> & input_subsystem = m_coreSystem.InputSubsystem();
    input_subsystem->GetKeyboard()->SetKeyboardModifiers(modifier);
    input_subsystem->GetKeyboard()->PressKeyboardKey(keyIndex);
    input_subsystem->GetKeyboard()->PressKey(keyCode);
    input_subsystem->PumpEvents();
}

void OSManager::KeyboardKeyRelease(int modifier, int keyIndex, int keyCode)
{
    std::shared_ptr<InputCommon::InputSubsystem> & input_subsystem = m_coreSystem.InputSubsystem();
    input_subsystem->GetKeyboard()->SetKeyboardModifiers(modifier);
    input_subsystem->GetKeyboard()->ReleaseKeyboardKey(keyIndex);
    input_subsystem->GetKeyboard()->ReleaseKey(keyCode);
    input_subsystem->PumpEvents();
}
