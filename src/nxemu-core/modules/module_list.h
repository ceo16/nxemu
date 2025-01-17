#pragma once
#include <common/path.h>
#include <memory>
#include <nxemu-core/modules/module_base.h>
#include <string>
#include <vector>

class ModuleList
{
public:
    typedef struct
    {
        MODULE_INFO info;
        Path path;
        std::string file;
    } MODULE;

    ModuleList(bool autoFill = true);
    ~ModuleList() = default;

    void LoadList(void);
    uint32_t GetModuleCount(void) const;
    const MODULE * GetModuleInfo(uint32_t index) const;

private:
    ModuleList(const ModuleList &) = delete;
    ModuleList & operator=(const ModuleList &) = delete;

    typedef std::vector<std::unique_ptr<MODULE>> Modules;

    void AddModuleFromDir(const Path & dir);

    Path m_moduleDir;
    Modules m_modules;
};