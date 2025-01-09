#include "module_list.h"
#include "settings/core_settings.h"
#include <common/dynamic_library.h>
#include <common/std_string.h>

ModuleList::ModuleList(bool autoFill) :
    m_moduleDir(coreSettings.moduleDir)
{
    if (autoFill)
    {
        LoadList();
    }
}

void ModuleList::LoadList()
{
    m_modules.clear();
    AddModuleFromDir(m_moduleDir);
}

void ModuleList::AddModuleFromDir(const Path & dir)
{
    Path searchDir(dir.GetDriveDirectory().c_str(), "*");
    if (searchDir.FindFirst(Path::FIND_ATTRIBUTE_SUBDIR))
    {
        do
        {
            AddModuleFromDir(searchDir);
        } while (searchDir.FindNext());
        searchDir.DirectoryUp();
    }

    searchDir.SetNameExtension("*.dll");
    if (searchDir.FindFirst())
    {
        DynLibHandle hLib = nullptr;
        do
        {
            if (hLib != nullptr)
            {
                DynamicLibraryClose(hLib);
                hLib = nullptr;
            }

            hLib = DynamicLibraryOpen(searchDir);
            if (hLib == nullptr)
            {
                continue;
            }

            ModuleBase::tyGetModuleInfo getModuleInfo = (ModuleBase::tyGetModuleInfo)DynamicLibraryGetProc(hLib, "GetModuleInfo");
            if (getModuleInfo == nullptr)
            {
                continue;
            }

            std::unique_ptr<MODULE> module = std::make_unique<MODULE>();
            getModuleInfo(&module->info);
            if (!ModuleBase::ValidVersion(module->info))
            {
                continue;
            }
            module->path = searchDir;
            module->file = stdstr((const char *)searchDir).substr(strlen(m_moduleDir));
            m_modules.push_back(std::move(module));
        } while (searchDir.FindNext());

        if (hLib != nullptr)
        {
            DynamicLibraryClose(hLib);
            hLib = nullptr;
        }
    }
}

uint32_t ModuleList::GetModuleCount() const
{
    return (uint32_t)m_modules.size();
}

const ModuleList::MODULE * ModuleList::GetModuleInfo(uint32_t indx) const
{
    return indx < m_modules.size() ? m_modules[indx].get() : nullptr;
}