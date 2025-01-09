#pragma once
#include <stdint.h>

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
    MODULE_VIDEO_SPECS_VERSION = 0x0100,
    MODULE_CPU_SPECS_VERSION = 0x0100,
    MODULE_OPERATING_SYSTEM_SPECS_VERSION = 0x0100,
};

enum MODULE_TYPE : uint16_t
{
    MODULE_TYPE_VIDEO = 1,
    MODULE_TYPE_CPU = 2,
    MODULE_TYPE_OPERATING_SYSTEM = 3,
};

typedef struct
{
    uint16_t version; // Should be set module spec version eg MODULE_VIDEO_SPECS_VERSION
    uint16_t type;    // Set to the module type, eg MODULE_TYPE_VIDEO
    char name[200];   // Name of the DLL
} MODULE_INFO;

/*
Function: GetModuleInfo
Purpose: Fills the MODULE_INFO structure with information about the DLL.
Input: A pointer to a MODULE_INFO structure to be populated.    
Output: none
*/
EXPORT void CALL GetModuleInfo(MODULE_INFO * info);
