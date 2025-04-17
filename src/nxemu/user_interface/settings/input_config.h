#pragma once

__interface ISciterUI;
__interface ISciterWindow;

class InputConfig
{
public:
    InputConfig(ISciterUI & SciterUI);
    ~InputConfig();

    void Display(void * parentWindow);

private:
    InputConfig() = delete;
    InputConfig(const InputConfig &) = delete;
    InputConfig & operator=(const InputConfig &) = delete;

    ISciterUI & m_sciterUI;
    ISciterWindow * m_window;
};
