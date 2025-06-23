#include "system_config.h"
#include "system_config_audio.h"
#include <sciter_ui.h>
#include <sciter_element.h>
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
        if (pageNav.IsValid())
        {
            void * interfacePtr = nullptr;
            if (m_sciterUI.GetElementInterface(pageNav, IID_IPAGENAV, &interfacePtr))
            {
                m_pageNav = std::shared_ptr<IPageNav>(reinterpret_cast<IPageNav*>(interfacePtr), [](IPageNav*) {});
                m_pageNav->AddSink(this);
            }
        }
    }
}

bool SystemConfig::PageNavChangeFrom(const std::string& PageName, SCITER_ELEMENT PageNav)
{
    return true;
}

bool SystemConfig::PageNavChangeTo(const std::string& PageName, SCITER_ELEMENT PageNav)
{
    return true;
}

void SystemConfig::PageNavCreatedPage(const std::string& PageName, SCITER_ELEMENT Page)
{
    if (PageName == "Audio")
    {
        m_systemConfigAudio.reset(new SystemConfigAudio(m_sciterUI, m_window->GetHandle(), Page));
    }
}

void SystemConfig::PageNavPageChanged(const std::string& PageName, SCITER_ELEMENT PageNav)
{
}

