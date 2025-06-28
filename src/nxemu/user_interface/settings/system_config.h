#pragma once
#include <sciter_handler.h>
#include <widgets/page_nav.h>

__interface ISciterUI;
__interface ISciterWindow;
class SystemConfigAudio;

class SystemConfig :
    public IPagesSink,
    public IClickSink
{
public:
    SystemConfig(ISciterUI& SciterUI);
    ~SystemConfig();

    void Display(void* parentWindow);

    // IPagesSink
    bool PageNavChangeFrom(const std::string & pageName, SCITER_ELEMENT pageNav) override;
    bool PageNavChangeTo(const std::string & pageName, SCITER_ELEMENT pageNav) override;
    void PageNavCreatedPage(const std::string & pageName, SCITER_ELEMENT page) override;
    void PageNavPageChanged(const std::string & pageName, SCITER_ELEMENT pageNav) override;

    //IClickSink
    bool OnClick(SCITER_ELEMENT element, SCITER_ELEMENT source, uint32_t reason) override;

private:
    SystemConfig() = delete;
    SystemConfig(const SystemConfig &) = delete;
    SystemConfig & operator=(const SystemConfig &) = delete;

    ISciterUI & m_sciterUI;
    ISciterWindow * m_window;
    std::shared_ptr<IPageNav> m_pageNav;
    std::unique_ptr<SystemConfigAudio> m_systemConfigAudio;
};
