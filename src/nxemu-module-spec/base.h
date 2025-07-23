#pragma once
#include <stdint.h>
#include <string>

#if !defined(EXPORT) && defined(__cplusplus)
#define EXPORT extern "C" __declspec(dllexport)
#elif !defined(EXPORT)
#define EXPORT __declspec(dllexport)
#endif

#ifndef CALL
#define CALL __cdecl
#else
#define CALL
#endif

enum
{
    MODULE_LOADER_SPECS_VERSION = 0x0108,
    MODULE_VIDEO_SPECS_VERSION = 0x010B,
    MODULE_CPU_SPECS_VERSION = 0x0104,
    MODULE_OPERATING_SYSTEM_SPECS_VERSION = 0x0109,
};

enum MODULE_TYPE : uint16_t
{
    MODULE_TYPE_LOADER = 1,
    MODULE_TYPE_VIDEO = 2,
    MODULE_TYPE_CPU = 3,
    MODULE_TYPE_OPERATING_SYSTEM = 4,
};

__interface IModuleNotification
{
    void DisplayError(const char * message) = 0;
    void BreakPoint(const char * fileName, uint32_t lineNumber) = 0;
};

typedef void (*SettingChangeCallback)(const char * setting, void * userData);

__interface IModuleSettings
{
    const char * GetString(const char * setting) const = 0;
    bool GetBool(const char * setting) const = 0;
    int32_t GetInt(const char * setting) const = 0;
    void SetString(const char * setting, const char * value) = 0;
    void SetBool(const char * setting, bool value) = 0;
    void SetInt(const char * setting, int32_t value) = 0;

    void SetDefaultBool(const char * setting, bool value) = 0;
    void SetDefaultInt(const char * setting, int value) = 0;
    void SetDefaultString(const char * setting, const char * value) = 0;

    const char * GetSectionSettings(const char * section) const = 0;
    void SetSectionSettings(const char * section, const std::string & json) = 0;

    void RegisterCallback(const char * setting, SettingChangeCallback callback, void * userData) = 0;
    void UnregisterCallback(const char * setting, SettingChangeCallback callback, void * userData) = 0;
};

typedef struct
{
    IModuleNotification * notification;
    IModuleSettings * settings;
} ModuleInterfaces;

typedef struct
{
    uint16_t version; // Should be set module spec version eg MODULE_VIDEO_SPECS_VERSION
    uint16_t type;    // Set to the module type, eg MODULE_TYPE_VIDEO
    char name[200];   // Name of the DLL
} MODULE_INFO;

__interface IRenderWindow
{
    void * RenderSurface(void) const;
    float PixelRatio(void) const;
};

__interface IOperatingSystem;
__interface IVideo;
__interface ICpu;
__interface ISystemloader;

__interface ISwitchSystem
{
    void StartEmulation() = 0;

    ISystemloader & Systemloader() = 0;
    IOperatingSystem & OperatingSystem() = 0;
    IVideo & Video() = 0;
    ICpu & Cpu() = 0;
};

/*
Function: GetModuleInfo
Purpose: Fills the MODULE_INFO structure with information about the DLL.
Input: A pointer to a MODULE_INFO structure to be populated.
Output: none
*/
EXPORT void CALL GetModuleInfo(MODULE_INFO * info);

/*
Function: ModuleInitialize
Purpose: Initializes the module for global use.
Input: None
Output: Returns 0 on success
*/
EXPORT int CALL ModuleInitialize(ModuleInterfaces & interfaces);

/*
Function: ModuleCleanup
Purpose: Cleans up global resources used by the module.
Input: None
Output: None
*/
EXPORT void CALL ModuleCleanup();

/*
Function: EmulationStarting
Purpose: Called when emulation is starting
Input: None.
Output: None.
*/
EXPORT void CALL EmulationStarting();

/*
Function: EmulationStopping
Purpose: Called when emulation is stopping
Input: None
Output: None
*/
EXPORT void CALL EmulationStopping();

/*
Function: FlushSettings
Purpose: Called when emulation is saving settings
Input: None
Output: None
*/
EXPORT void CALL FlushSettings();