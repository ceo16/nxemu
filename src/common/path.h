#pragma once
#include <string>

class Path
{
public:
    enum DIR_CURRENT_DIRECTORY { CURRENT_DIRECTORY = 1 };

    Path();
    Path(const char * path);
    Path(const char * path, const char * fileName);
    Path(const std::string & path);
    Path(const std::string & path, const char * fileName);
    Path(const std::string & path, const std::string & fileName);

    Path(DIR_CURRENT_DIRECTORY sdt, const char * nameExten = nullptr);

    operator const char *() const;
    operator const std::string& ();

    void GetDriveDirectory(std::string & directory) const;
    std::string GetDriveDirectory(void) const;
	void GetComponents(std::string * drive = nullptr, std::string * directory = nullptr, std::string * name = nullptr, std::string * extension = nullptr) const;

    void SetDriveDirectory(const char * driveDirectory);
    void SetNameExtension(const char * nameExtension);
	void SetComponents(const char * drive, const char * directory, const char * name, const char * extension);

    bool FileDelete(bool evenIfReadOnly = true) const;
    bool FileExists() const;

    bool DirectoryChange() const;

private:
    void SetToCurrentDirectory();

    void CleanPath(std::string & path) const;

    void EnsureTrailingBackslash(std::string & directory) const;

    std::string	m_path;
};