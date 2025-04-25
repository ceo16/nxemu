#pragma once
#include "module_base.h"
#include <nxemu-module-spec/system_loader.h>

class LoaderModule :
    public ModuleBase
{
public:
    typedef ISystemloader *(CALL * tyCreateSystemLoader)(ISwitchSystem & System);
    typedef void(CALL * tyDestroySystemLoader)(ISystemloader * loader);

    LoaderModule();
    ~LoaderModule() = default;

    ISystemloader * CreateSystemLoader(ISwitchSystem & system) const;
    void DestroySystemLoader(ISystemloader * loader) const;

protected:
    void UnloadModule(void);
    bool LoadFunctions(void);

    MODULE_TYPE ModuleType() const;

private:
    LoaderModule(const LoaderModule &) = delete;
    LoaderModule & operator=(const LoaderModule &) = delete;

    static ISystemloader * CALL dummyCreateSystemLoader(ISwitchSystem & System);
    static void CALL dummyDestroySystemLoader(ISystemloader * loader);

    tyCreateSystemLoader m_createSystemLoader;
    tyDestroySystemLoader m_destroySystemLoader;
};