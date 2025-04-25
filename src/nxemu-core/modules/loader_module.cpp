#include "loader_module.h"

LoaderModule::LoaderModule() :
    m_createSystemLoader(dummyCreateSystemLoader),
    m_destroySystemLoader(dummyDestroySystemLoader)
{
}

ISystemloader * LoaderModule::CreateSystemLoader(ISwitchSystem & system) const
{
    return m_createSystemLoader(system);
}

void LoaderModule::DestroySystemLoader(ISystemloader * loader) const
{
    m_destroySystemLoader(loader);
}

void LoaderModule::UnloadModule(void)
{
    m_createSystemLoader = dummyCreateSystemLoader;
    m_destroySystemLoader = dummyDestroySystemLoader;
}

bool LoaderModule::LoadFunctions(void)
{
    m_createSystemLoader = (tyCreateSystemLoader)DynamicLibraryGetProc(m_lib, "CreateSystemLoader");
    m_destroySystemLoader = (tyDestroySystemLoader)DynamicLibraryGetProc(m_lib, "DestroySystemLoader");

    bool res = true;
    if (m_createSystemLoader == nullptr)
    {
        m_createSystemLoader = dummyCreateSystemLoader;
        res = false;
    }
    if (m_destroySystemLoader == nullptr)
    {
        m_destroySystemLoader = dummyDestroySystemLoader;
        res = false;
    }
    return res;
}

MODULE_TYPE LoaderModule::ModuleType() const
{
    return MODULE_TYPE_LOADER;
}

ISystemloader * LoaderModule::dummyCreateSystemLoader(ISwitchSystem & /*System*/)
{
    return nullptr;
}

void LoaderModule::dummyDestroySystemLoader(ISystemloader * /*loader*/)
{
}
