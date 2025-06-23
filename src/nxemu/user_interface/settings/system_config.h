#pragma once
#include <widgets/page_nav.h>

__interface ISciterUI;
__interface ISciterWindow;
class SystemConfigAudio;

class SystemConfig :
    public IPagesSink
{
public:
    SystemConfig(ISciterUI& SciterUI);
    ~SystemConfig();

    void Display(void* parentWindow);

    // IPagesSink
    bool PageNavChangeFrom(const std::string& PageName, SCITER_ELEMENT PageNav) override;
    bool PageNavChangeTo(const std::string& PageName, SCITER_ELEMENT PageNav) override;
    void PageNavCreatedPage(const std::string& PageName, SCITER_ELEMENT Page) override;
    void PageNavPageChanged(const std::string& PageName, SCITER_ELEMENT PageNav) override;

private:
    SystemConfig() = delete;
    SystemConfig(const SystemConfig &) = delete;
    SystemConfig & operator=(const SystemConfig &) = delete;

    ISciterUI & m_sciterUI;
    ISciterWindow * m_window;
    std::shared_ptr<IPageNav> m_pageNav;
    std::unique_ptr<SystemConfigAudio> m_systemConfigAudio;
};
