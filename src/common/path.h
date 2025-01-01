#pragma once
#include <string>

class Path
{
public:
    enum DIR_CURRENT_DIRECTORY
    {
        CURRENT_DIRECTORY = 1
    };
    enum DIR_MODULE_DIRECTORY
    {
        MODULE_DIRECTORY = 2
    };


    Path();
    Path(const Path & rPath);
    Path(const char * path);
    Path(const char * path, const char * fileName);
    Path(const std::string & path);
    Path(const std::string & path, const char * fileName);
    Path(const std::string & path, const std::string & fileName);

    Path(DIR_CURRENT_DIRECTORY sdt, const char * nameExten = nullptr);
    Path(DIR_MODULE_DIRECTORY sdt, const char * nameExten = nullptr);

    operator const char *() const;
    operator const std::string &();

    void GetDriveDirectory(std::string & directory) const;
    std::string GetDriveDirectory(void) const;
    void GetDirectory(std::string & directory) const;
    std::string GetDirectory(void) const;
    void GetComponents(std::string * drive = nullptr, std::string * directory = nullptr, std::string * name = nullptr, std::string * extension = nullptr) const;
    bool IsRelative() const;

    void SetDriveDirectory(const char * driveDirectory);
    void SetDirectory(const char * directory, bool ensureAbsolute = false);
    void SetNameExtension(const char * nameExtension);
    void SetComponents(const char * drive, const char * directory, const char * name, const char * extension);
    void AppendDirectory(const char * subDirectory);

    bool FileDelete(bool evenIfReadOnly = true) const;
    bool FileExists() const;
    bool FileSelect(void * hwndOwner, const char * initialDir, const char * fileFilter, bool fileMustExist);

    bool DirectoryCreate(bool createIntermediates = true);
    bool DirectoryChange() const;
    bool DirectoryExists() const;
    void DirectoryNormalize(Path baseDir);
    void DirectoryUp(std::string * lastDir = nullptr);

private:
    void SetToCurrentDirectory();
    void SetToModuleDirectory();

    void CleanPath(std::string & path) const;
    void EnsureLeadingBackslash(std::string & directory) const;
    void EnsureTrailingBackslash(std::string & directory) const;
    void StripLeadingBackslash(std::string & path) const;
    void StripTrailingBackslash(std::string & rDirectory) const;

    static void * m_hInst;
    std::string m_path;
};