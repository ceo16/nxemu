#include "system_loader.h"
#include "file_format/nro.h"
#include "file_format/nacp.h"
#include <fmt/core.h>
#include <common/path.h>
#include <nxemu-core/settings/identifiers.h>
#include "core/file_sys/registered_cache.h"
#include "core/file_sys/filesystem.h"
#include "core/file_sys/romfs_factory.h"
#include "core/file_sys/vfs/vfs_real.h"
#include "core/file_sys/vfs/vfs_types.h"
#include "core/file_sys/system_archive/system_archive.h"

extern IModuleSettings * g_settings;

struct Systemloader::Impl {
    explicit Impl(Systemloader & loader, ISwitchSystem& system) :
        m_loader(loader),
        m_system(system),
        m_fsController(loader),
        m_TitleID(0)
    {
    }

    bool LoadNRO(const char* nroFile);

    Systemloader & m_loader;
    ISwitchSystem & m_system;
    /// RealVfsFilesystem instance
    FileSys::VirtualFilesystem m_virtualFilesystem;
    /// ContentProviderUnion instance
    std::unique_ptr<FileSys::ContentProviderUnion> m_contentProvider;
    FileSys::FileSystemController m_fsController;
    std::unique_ptr<Nro> m_nro;
    uint64_t m_TitleID;
};

Systemloader::Systemloader(ISwitchSystem & system) :
    impl(std::make_unique<Impl>(*this, system))
{
}

Systemloader::~Systemloader()
{
}

bool Systemloader::Initialize(void)
{
    if (impl->m_virtualFilesystem == nullptr) {
        impl->m_virtualFilesystem = std::make_shared<FileSys::RealVfsFilesystem>();
    }
    if (impl->m_contentProvider == nullptr) {
        impl->m_contentProvider = std::make_unique<FileSys::ContentProviderUnion>();
    }
    GetFileSystemController().CreateFactories(*GetFilesystem(), false);
    return true;
}

bool Systemloader::SelectAndLoad(void * parentWindow)
{
    Path fileToOpen;
    std::string fileName;
    const char * filter = "Switch Files (*.nro)\0*.nro\0All files (*.*)\0*.*\0";
    if (fileToOpen.FileSelect(parentWindow, Path(Path::MODULE_DIRECTORY), filter, true))
    {
        fileName = (const std::string &)fileToOpen;
    }
    if (fileName.length() == 0)
    {
        return false;
    }
    return LoadRom(fileName.c_str());
}

bool Systemloader::LoadRom(const char * romFile)
{
    bool res = false;
    g_settings->SetBool(NXCoreSetting::RomLoading, true);
    if (Nro::IsNroFile(romFile))
    {
        res = impl->LoadNRO(romFile);
    }

    g_settings->SetBool(NXCoreSetting::RomLoading, false);
    if (res)
    {
        g_settings->SetString(NXCoreSetting::GameFile, romFile);
        impl->m_system.StartEmulation();
    }
    return res;
}

ISwitchSystem & Systemloader::GetSystem() {
    return impl->m_system;
}

FileSys::VirtualFilesystem Systemloader::GetFilesystem() {
    return impl->m_virtualFilesystem;
}

FileSys::FileSystemController & Systemloader::GetFileSystemController() {
    return impl->m_fsController;
}

void Systemloader::RegisterContentProvider(FileSys::ContentProviderUnionSlot slot, FileSys::ContentProvider* provider) 
{
    impl->m_contentProvider->SetSlot(slot, provider);
}

bool Systemloader::Impl::LoadNRO(const char* nroFile)
{
    Path filePath(nroFile);
    m_nro = std::make_unique<Nro>(filePath);
    if (!m_nro->Valid())
    {
        return false;
    }

    IOperatingSystem & operatingSystem = m_system.OperatingSystem();
    const IProgramMetadata & metaData = m_nro->MetaData();
    uint64_t baseAddress = 0;
    uint64_t processID = 0;
    if (!operatingSystem.CreateApplicationProcess(m_nro->CodeSize(), metaData, baseAddress, processID, false))
    {
        return false;
    }

    if (!operatingSystem.LoadModule(*m_nro, baseAddress))
    {
        return false;
    }
    const NACP * Nacp = m_nro->Nacp();
    if (Nacp == nullptr)
    {
        return false;
    }

    m_TitleID = Nacp->GetTitleId();
    m_fsController.RegisterProcess(processID, m_TitleID, std::make_unique<FileSys::RomFSFactory>(nullptr, false, *m_contentProvider, m_fsController));
    g_settings->SetString(NXCoreSetting::GameName, Nacp->GetApplicationName().c_str());
    operatingSystem.StartApplicationProcess(baseAddress, metaData.GetMainThreadPriority(), metaData.GetMainThreadStackSize());
    return true;
}

IFileSystemController & Systemloader::FileSystemController()
{
    return impl->m_fsController;
}

IVirtualFile * Systemloader::SynthesizeSystemArchive(const uint64_t title_id)
{
    FileSys::VirtualFile file = FileSys::SystemArchive::SynthesizeSystemArchive(title_id);
    return std::make_unique<VirtualFilePtr>(file).release();
}

uint32_t Systemloader::GetContentProviderEntriesCount(bool useTitleType, LoaderTitleType titleType, bool useContentRecordType, LoaderContentRecordType contentRecordType, bool useTitleId, unsigned long long titleId)
{
    const FileSys::ContentProviderUnion & rcu = *impl->m_contentProvider;
    std::optional<LoaderTitleType> title = useTitleType ? std::optional<LoaderTitleType>((LoaderTitleType)titleType) : std::nullopt;
    std::optional<LoaderContentRecordType> record = useContentRecordType ? std::optional<LoaderContentRecordType>((LoaderContentRecordType)contentRecordType) : std::nullopt;
    std::optional<u64> id = useTitleId ? std::optional<u64>(titleId) : std::nullopt;
    const std::vector<FileSys::ContentProviderEntry> list = rcu.ListEntriesFilter(title, record, id);
    return (uint32_t)list.size();
}
