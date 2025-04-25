#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

#define VERSION_MAJOR               0
#define VERSION_MINOR               1
#define VERSION_REVISION            0
#define VERSION_BUILD               85
#define VERSION_PREFIX              "Dev-"

#define GIT_REVISION                "cb0c01345a8067df971b809f0c8bf49f36c6ade0"
#define GIT_REVISION_SHORT          "cb0c013"
#define GIT_DIRTY                   "Dirty"
#define GIT_VERSION                 "cb0c013-Dirty"

#define VER_FILE_DESCRIPTION_STR    "NXEmu-Loader"
#define VER_FILE_VERSION            VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_BUILD
#define VER_FILE_VERSION_STR        VERSION_PREFIX STRINGIZE(VERSION_MAJOR)        \
                                    "." STRINGIZE(VERSION_MINOR)    \
                                    "." STRINGIZE(VERSION_REVISION) \
                                    "." STRINGIZE(VERSION_BUILD)    \
                                    "-" GIT_VERSION

#define VER_PRODUCTNAME_STR         "NXEmu-Loader"
#define VER_PRODUCT_VERSION         VER_FILE_VERSION
#define VER_PRODUCT_VERSION_STR     VER_FILE_VERSION_STR
#define VER_ORIGINAL_FILENAME_STR   VER_PRODUCTNAME_STR ".dll"
#define VER_INTERNAL_NAME_STR       VER_PRODUCTNAME_STR
#define VER_COPYRIGHT_STR           "Copyright (C) 2025"

#ifdef _DEBUG
#define VER_VER_DEBUG             VS_FF_DEBUG
#else
#define VER_VER_DEBUG             0
#endif

#define VER_FILEOS                  VOS_NT_WINDOWS32
#define VER_FILEFLAGS               VER_VER_DEBUG
#define VER_FILETYPE                VFT_DLL

