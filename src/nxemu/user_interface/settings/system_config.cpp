#include "system_config.h"
#include "system_config_audio.h"
#include <nxemu-core/machine/switch_system.h>
#include <sciter_ui.h>
#include <sciter_element.h>
#include <sciter_handler.h>
#include <widgets/page_nav.h>

SystemConfig::SystemConfig(ISciterUI & SciterUI) :
    m_sciterUI(SciterUI),
    m_window(nullptr)
{
}

SystemConfig::~SystemConfig()
{
}

void SystemConfig::Display(void * parentWindow)
{
    enum
    {
        WINDOW_HEIGHT = 300,
        WINDOW_WIDTH = 300,
    };

    if (!m_sciterUI.WindowCreate(parentWindow, "system_config.html", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SUIW_CHILD, m_window))
    {
        return;
    }
    SciterElement root(m_window->GetRootElement());
    if (root.IsValid())
    {
        SciterElement pageNav = root.GetElementByID("MainTabNav");
        std::shared_ptr<void> interfacePtr = pageNav.IsValid() ? m_sciterUI.GetElementInterface(pageNav, IID_IPAGENAV) : nullptr;
        if (interfacePtr)
        {
            m_pageNav = std::static_pointer_cast<IPageNav>(interfacePtr);
            m_pageNav->AddSink(this);
        }

        SciterElement okButton = root.FindFirst("button[role=\"window-ok\"]");
        m_sciterUI.AttachHandler(okButton, IID_ICLICKSINK, (IClickSink*)this);

    }
}

bool SystemConfig::PageNavChangeFrom(const std::string & /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
    return true;
}

bool SystemConfig::PageNavChangeTo(const std::string & /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
    return true;
}

void SystemConfig::PageNavCreatedPage(const std::string & pageName, SCITER_ELEMENT page)
{
    if (pageName == "Audio")
    {
        m_systemConfigAudio.reset(new SystemConfigAudio(m_sciterUI, m_window->GetHandle(), page));
    }
}

void SystemConfig::PageNavPageChanged(const std::string & /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
}

bool SystemConfig::OnClick(SCITER_ELEMENT element, SCITER_ELEMENT /*source*/, uint32_t /*reason*/)
{
    SciterElement clickElem(element);
    if (clickElem.GetAttribute("role") == "window-ok")
    {
        if (m_systemConfigAudio)
        {
            m_systemConfigAudio->SaveSetting();
        }
        SwitchSystem * system = SwitchSystem::GetInstance();
        if (system != nullptr)
        {
            system->FlushSettings();
        }
        m_window->Destroy();
    }
    return false;
}


