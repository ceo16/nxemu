#include "user_interface/main_window.h"
#include <memory>
#include <windows.h>

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpszArgs*/, int /*nWinMode*/)
{
    std::unique_ptr<MainWindow> mainWindow = std::make_unique<MainWindow>(L"NxEmu");
    mainWindow->ShowWindow(true);
    mainWindow->ProcessAllMessages();
    mainWindow.reset(nullptr);
    return 0;
}