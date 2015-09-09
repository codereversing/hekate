#include "Common/SafeObject.h"

namespace Hekate
{
namespace Common
{
namespace SafeObjectCleanupFnc
{

bool ClnCloseHandle(const HANDLE &handle)
{
    return BOOLIFY(CloseHandle(handle));
};

bool ClnFreeLibrary(const HMODULE &handle)
{
    return BOOLIFY(FreeLibrary(handle));
};

bool ClnLocalFree(const HLOCAL &handle)
{
    return (LocalFree(handle) == nullptr);
};

bool ClnGlobalFree(const HGLOBAL &handle)
{
    return (GlobalFree(handle) == nullptr);
};

bool ClnUnmapViewOfFile(const PVOID &handle)
{
    return BOOLIFY(UnmapViewOfFile(handle));
};

bool ClnCloseDesktop(const HDESK &handle)
{
    return BOOLIFY(CloseDesktop(handle));
};

bool ClnCloseWindowStation(const HWINSTA &handle)
{
    return BOOLIFY(CloseWindowStation(handle));
};

bool ClnCloseServiceHandle(const SC_HANDLE &handle)
{
    return BOOLIFY(CloseServiceHandle(handle));
};

bool ClnVirtualFree(const PVOID &handle)
{
    return BOOLIFY(VirtualFree(handle, 0, MEM_RELEASE));
};

}
}
}
