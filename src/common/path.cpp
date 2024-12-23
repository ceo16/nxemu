#include "path.h"
#include "std_string.h"
#include <Windows.h>

const char DRIVE_DELIMITER = ':';
const char * const DIR_DOUBLEDELIM = "\\\\";
const char DIRECTORY_DELIMITER = '\\';
const char DIRECTORY_DELIMITER2 = '/';

Path::Path()
{
}

Path::Path(const char * path)
{
    m_path = path ? path : "";
    CleanPath(m_path);
}

Path::Path(const char * path, const char * fileName)
{
    SetDriveDirectory(path);
    SetNameExtension(fileName);
}

Path::Path(const std::string & path)
{
    m_path = path;
    CleanPath(m_path);
}

Path::Path(const std::string & path, const char * fileName)
{
    SetDriveDirectory(path.c_str());
    SetNameExtension(fileName);
}

Path::Path(const std::string & path, const std::string & fileName)
{
    SetDriveDirectory(path.c_str());
    SetNameExtension(fileName.c_str());
}

Path::Path(DIR_CURRENT_DIRECTORY /*sdt*/, const char * nameExten)
{
    // Application's current directory
    SetToCurrentDirectory();
    if (nameExten)
    {
        SetNameExtension(nameExten);
    }
}

Path::operator const char* () const
{
    return (const char*)m_path.c_str();
}

Path::operator const std::string& ()
{ 
    return m_path; 
}

void Path::GetDriveDirectory(std::string & driveDirectory) const
{
    std::string drive, directory;
    GetComponents(&drive, &directory);
    driveDirectory = drive;
    if (!drive.empty())
    {
        driveDirectory += DRIVE_DELIMITER;
    }
    driveDirectory += directory;
}

std::string Path::GetDriveDirectory(void) const
{
    std::string driveDirectory;
    GetDriveDirectory(driveDirectory);
    return driveDirectory;
}

void Path::GetComponents(std::string * drive, std::string * directory, std::string * name, std::string * extension) const
{
    char driveBuff[_MAX_DRIVE + 1] = { 0 }, dirBuff[_MAX_DIR + 1] = { 0 }, nameBuff[_MAX_FNAME + 1] = { 0 }, extBuff[_MAX_EXT + 1] = { 0 };

    const char * basePath = m_path.c_str();
    const char * driveDir = strrchr(basePath, DRIVE_DELIMITER);
    if (driveDir != nullptr)
    {
        size_t len = sizeof(dirBuff) < (driveDir - basePath) ? sizeof(driveBuff) : driveDir - basePath;
        strncpy(driveBuff, basePath, len);
        basePath += len + 1;
    }

    const char * last = strrchr(basePath, DIRECTORY_DELIMITER);
    if (last != nullptr)
    {
        size_t len = sizeof(dirBuff) < (last - basePath) ? sizeof(dirBuff) : last - basePath;
        if (len > 0)
        {
            strncpy(dirBuff, basePath, len);
        }
        else
        {
            dirBuff[0] = DIRECTORY_DELIMITER;
            dirBuff[1] = '\0';
        }
        strncpy(nameBuff, last + 1, sizeof(nameBuff));
    }
    else
    {
        strncpy(dirBuff, basePath, sizeof(dirBuff));
    }
    char * ext = strrchr(nameBuff, '.');
    if (ext != nullptr)
    {
        strncpy(extBuff, ext + 1, sizeof(extBuff));
        *ext = '\0';
    }

    if (drive)
    {
        *drive = driveBuff;
    }
    if (directory)
    {
        *directory = dirBuff;
    }
    if (name)
    {
        *name = nameBuff;
    }
    if (extension)
    {
        *extension = extBuff;
    }
}

void Path::SetDriveDirectory(const char * driveDirectory)
{
    std::string driveDirectoryStr = driveDirectory;
    if (driveDirectoryStr.length() > 0)
    {
        EnsureTrailingBackslash(driveDirectoryStr);
        CleanPath(driveDirectoryStr);
    }

    std::string name, extension;
    GetComponents(nullptr, nullptr, &name, &extension);
    SetComponents(nullptr, driveDirectoryStr.c_str(), name.c_str(), extension.c_str());
}

void Path::SetNameExtension(const char * nameExtension)
{
    std::string directory, drive;
    GetComponents(&drive, &directory);
    SetComponents(drive.c_str(), directory.c_str(), nameExtension, nullptr);
}

void Path::SetComponents(const char * drive, const char * directory, const char * name, const char * extension)
{
    char fullname[MAX_PATH];

    memset(fullname, 0, sizeof(fullname));
    if (directory == nullptr || strlen(directory) == 0)
    {
        static char emptyDir[] = { DIRECTORY_DELIMITER, '\0' };
        directory = emptyDir;
    }

    _makepath(fullname, drive, directory, name, extension);
    m_path = fullname;
}

bool Path::FileDelete(bool evenIfReadOnly) const
{
    std::wstring filePath = stdstr(m_path).ToUTF16();
    uint32_t attributes = ::GetFileAttributes(filePath.c_str());
    if (attributes == (uint32_t)-1)
    {
        return false;
    }

    if (((attributes & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY) && !evenIfReadOnly)
    {
        return false;
    }

    SetFileAttributes(filePath.c_str(), FILE_ATTRIBUTE_NORMAL);
    return DeleteFile(filePath.c_str()) != 0;
}

bool Path::FileExists() const
{
    WIN32_FIND_DATA FindData;
    HANDLE hFindFile = FindFirstFile(stdstr(m_path).ToUTF16().c_str(), &FindData);
    bool bSuccess = (hFindFile != INVALID_HANDLE_VALUE);

    if (hFindFile != nullptr) // Make sure we close the search
    {
        FindClose(hFindFile);
    }

    return bSuccess;
}

bool Path::DirectoryChange() const
{
    std::string driveDirectory;
    GetDriveDirectory(driveDirectory);

    return SetCurrentDirectory(stdstr(driveDirectory).ToUTF16().c_str()) != 0;
}

void Path::SetToCurrentDirectory()
{
    wchar_t path[260];
    memset(path, 0, sizeof(path));
    ::GetCurrentDirectory(sizeof(path), path);
    SetDriveDirectory(stdstr().FromUTF16(path).c_str());
}

void Path::CleanPath(std::string& path) const
{
    std::string::size_type pos = path.find(DIRECTORY_DELIMITER2);
    while (pos != std::string::npos)
    {
        path.replace(pos, 1, &DIRECTORY_DELIMITER);
        pos = path.find(DIRECTORY_DELIMITER2, pos + 1);
    }

    bool appendEnd = !_strnicmp(path.c_str(), DIR_DOUBLEDELIM, 2);
    pos = path.find(DIR_DOUBLEDELIM);
    while (pos != std::string::npos)
    {
        path.replace(pos, 2, &DIRECTORY_DELIMITER);
        pos = path.find(DIR_DOUBLEDELIM, pos + 1);
    }
    if (appendEnd)
    {
        path.insert(0, stdstr_f("%c", DIRECTORY_DELIMITER).c_str());
    }
}

void Path::EnsureTrailingBackslash(std::string & directory) const
{
    std::string::size_type length = directory.length();

    if (directory.empty() || (directory[length - 1] != DIRECTORY_DELIMITER))
    {
        directory += DIRECTORY_DELIMITER;
    }
}
