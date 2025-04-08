#include "settings/ui_settings.h"
#include "user_interface/main_window.h"
#include "user_interface/notification.h"
#include "user_interface/sciter_main_window.h"
#include <widgets/menubar.h>
#include <common/std_string.h>
#include <memory>
#include <nxemu-core/app_init.h>
#include <nxemu-core/version.h>
#include <sciter_ui.h>
#include <windows.h>

void RegisterWidgets(ISciterUI & sciterUI)
{
    WidgetMenuBar::Register(sciterUI);
}

int WINAPI WinMain(_In_ HINSTANCE /*hInstance*/, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR /*lpszArgs*/, _In_ int /*nWinMode*/)
{
    bool Res = AppInit(&Notification::GetInstance());

    if (uiSettings.sciterUI && Res)
    {
        ISciterUI * sciterUI = nullptr;
        if (Res && !SciterUIInit(uiSettings.languageDir, uiSettings.languageBase.c_str(), uiSettings.languageCurrent.c_str(), uiSettings.sciterConsole, sciterUI))
        {
            Res = false;
        }
        if (Res)
        {
            RegisterWidgets(*sciterUI);
            SciterMainWindow window(*sciterUI, stdstr_f("NXEmu %s", VER_FILE_VERSION_STR).c_str());
            window.Show();
            sciterUI->Run();
        }
        if (sciterUI != nullptr)
        {
            sciterUI->Shutdown();
        }
    }
    else if (Res)
    {
        std::unique_ptr<MainWindow> mainWindow = std::make_unique<MainWindow>(stdstr_f("NXEmu %s", VER_FILE_VERSION_STR).ToUTF16().c_str());
        mainWindow->ShowWindow(true);
        mainWindow->ProcessAllMessages();
        mainWindow.reset(nullptr);
    }
    AppCleanup();
    Notification::CleanUp();
    return Res ? 0 : 1;
}