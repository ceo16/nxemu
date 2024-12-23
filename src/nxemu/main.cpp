#include "user_interface/main_window.h"
#include <common/std_string.h>
#include <nxemu-core/Version.h>
#include <memory>
#include <windows.h>

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpszArgs*/, int /*nWinMode*/)
{
    std::unique_ptr<MainWindow> mainWindow = std::make_unique<MainWindow>(stdstr_f("NXEmu %s", VER_FILE_VERSION_STR).ToUTF16().c_str());
    mainWindow->ShowWindow(true);
    mainWindow->ProcessAllMessages();
    mainWindow.reset(nullptr);
    return 0;
}