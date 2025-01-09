#pragma once
#include <string>

typedef void * DynLibHandle;

DynLibHandle DynamicLibraryOpen(const char * path, bool showErrors = true);
void DynamicLibraryClose(DynLibHandle libHandle);
void * DynamicLibraryGetProc(DynLibHandle libHandle, const char * name);
