#include "settings/ui_settings.h"
#include "user_interface/notification.h"
#include "user_interface/main_window.h"
#include <common/std_string.h>
#include <memory>
#include <nxemu-core/app_init.h>
#include <nxemu-core/version.h>
#include <windows.h>

int WINAPI WinMain(_In_ HINSTANCE /*hInstance*/, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR /*lpszArgs*/, _In_ int /*nWinMode*/)
{
    if (!AppInit(&Notification::GetInstance()))
    {
        return 1;
    }

    std::unique_ptr<MainWindow> mainWindow = std::make_unique<MainWindow>(stdstr_f("NXEmu %s", VER_FILE_VERSION_STR).ToUTF16().c_str());
    mainWindow->ShowWindow(true);
    mainWindow->ProcessAllMessages();
    mainWindow.reset(nullptr);
    AppCleanup();
    Notification::CleanUp();
    return 0;
}