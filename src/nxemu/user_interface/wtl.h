#pragma once

#include <windows.h>
#pragma warning(push)
#pragma warning(disable : 4091) // warning C4091: 'typedef ': ignored on left of 'tagGPFIDL_FLAGS' when no variable is declared
#pragma warning(disable : 4201) // warning C4201: nonstandard extension used: nameless struct/union
#pragma warning(disable : 4302) // warning C4302: 'type cast': truncation from 'LPCTSTR' to 'WORD'
#pragma warning(disable : 4458) // warning C4458: declaration of 'dwCommonButtons' hides class member
#pragma warning(disable : 4838) // warning C4838: conversion from 'int' to 'UINT' requires a narrowing conversion
#pragma warning(disable : 4996) // warning C4996: 'GetVersionExA': was declared deprecated
#define _ATL_DISABLE_NOTHROW_NEW
#include <shellapi.h>
#include <atlbase.h>

#include <wtl/atlapp.h>
#include <wtl/atldlgs.h>
#include <wtl/atlframe.h>
#include <wtl/atlctrls.h>

#define _WTL_NO_CSTRING

#include <atlwin.h>
#include <wtl/atlmisc.h>
#include <wtl/atlcrack.h>

#pragma warning(pop)

class WTLModule :
    public CAppModule
{
public:
    WTLModule(void)
    {
        Init(nullptr, GetModuleHandle(nullptr));
    }
    virtual ~WTLModule(void)
    {
        Term();
    }
};

extern WTLModule _Module;
