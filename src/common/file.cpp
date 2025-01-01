#include "file.h"
#include <Windows.h>

File::File() :
    m_file(INVALID_HANDLE_VALUE),
    m_closeOnDestroy(false)
{
}

File::File(void * fileHandle) :
    m_file(fileHandle),
    m_closeOnDestroy(true)
{
}

File::File(const char * fileName, uint32_t openFlags) :
    m_file(INVALID_HANDLE_VALUE),
    m_closeOnDestroy(true)
{
    Open(fileName, openFlags);
}

File::~File()
{
    if (m_file != INVALID_HANDLE_VALUE && m_closeOnDestroy)
    {
        Close();
    }
}

bool File::Open(const char * fileName, uint32_t openFlags)
{
    if (!Close())
    {
        return false;
    }

    if (fileName == nullptr || strlen(fileName) == 0)
    {
        return false;
    }

    m_file = INVALID_HANDLE_VALUE;

    ULONG dwAccess = 0;
    switch (openFlags & 3)
    {
    case modeRead:
        dwAccess = GENERIC_READ;
        break;
    case modeWrite:
        dwAccess = GENERIC_WRITE;
        break;
    case modeReadWrite:
        dwAccess = GENERIC_READ | GENERIC_WRITE;
        break;
    default:
        return false;
    }

    ULONG shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    if ((openFlags & shareDenyWrite) == shareDenyWrite)
    {
        shareMode &= ~FILE_SHARE_WRITE;
    }
    if ((openFlags & shareDenyRead) == shareDenyRead)
    {
        shareMode &= ~FILE_SHARE_READ;
    }
    if ((openFlags & shareExclusive) == shareExclusive)
    {
        shareMode = 0;
    }

    // map modeNoInherit flag
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = nullptr;
    sa.bInheritHandle = (openFlags & modeNoInherit) == 0;

    // map creation flags
    ULONG createFlag = OPEN_EXISTING;
    if (openFlags & modeCreate)
    {
        createFlag = ((openFlags & modeNoTruncate) != 0) ? OPEN_ALWAYS : CREATE_ALWAYS;
    }

    HANDLE hFile = ::CreateFileA(fileName, dwAccess, shareMode, &sa, createFlag, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    m_file = hFile;
    m_closeOnDestroy = true;
    return true;
}

bool File::Close()
{
    bool bError = true;
    if (m_file != INVALID_HANDLE_VALUE)
    {
        bError = !::CloseHandle(m_file);
    }
    m_file = INVALID_HANDLE_VALUE;
    m_closeOnDestroy = false;
    return bError;
}

void File::SeekToBegin(void)
{
    Seek(0, SeekPosition::begin);
}

uint64_t File::Seek(int64_t offset, SeekPosition from)
{
    LONG distanceToMove = (LONG)(offset & 0xFFFFFFFF);
    LONG distanceToMoveHigh = (LONG)(offset >> 32);
    ULONG dwNew = ::SetFilePointer(m_file, distanceToMove, &distanceToMoveHigh, (ULONG)from);
    if (dwNew == (ULONG)-1)
    {
        return (uint64_t)-1;
    }
    return ((uint64_t)distanceToMoveHigh << 32) | dwNew;
}

bool File::SetLength(uint64_t dwNewLen)
{
    Seek(dwNewLen, SeekPosition::begin);
    return SetEndOfFile();
}

uint64_t File::GetLength() const
{
    DWORD hiWord = 0;
    DWORD loWord = GetFileSize(m_file, &hiWord);
    return ((uint64_t)hiWord << 32) | (uint64_t)loWord;
}

uint32_t File::Read(void * lpBuf, uint32_t nCount)
{
    if (nCount == 0)
    {
        return 0;
    }

    DWORD read = 0;
    if (!::ReadFile(m_file, lpBuf, nCount, &read, nullptr))
    {
        return 0;
    }
    return (uint32_t)read;
}

bool File::Write(const void * buffer, uint32_t bufferSize)
{
    if (bufferSize == 0)
    {
        return true;
    }

    ULONG written = 0;
    if (!::WriteFile(m_file, buffer, bufferSize, &written, nullptr))
    {
        return false;
    }

    if (written != bufferSize)
    {
        return false;
    }
    return true;
}

bool File::IsOpen(void) const
{
    return m_file != INVALID_HANDLE_VALUE;
}

bool File::SetEndOfFile()
{
    return ::SetEndOfFile(m_file) != 0;
}
