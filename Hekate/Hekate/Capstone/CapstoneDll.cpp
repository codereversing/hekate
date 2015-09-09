#include "Capstone/CapstoneDll.h"

#include "plog/Log.h"

#include <Windows.h>

namespace Hekate
{
namespace Capstone
{

extern cs_openFnc cs_open = nullptr;
extern cs_disasmFnc cs_disasm = nullptr;
extern cs_freeFnc cs_free = nullptr;
extern cs_closeFnc cs_close = nullptr;

void ResolveCapstoneImports()
{
#ifdef _M_IX86
    const HMODULE module = LoadLibrary(L"capstone_x86.dll");
#elif defined _M_AMD64
    const HMODULE module = LoadLibrary(L"capstone_x64.dll");
#else
#error "Unsupported architecture"
#endif
    if (module == nullptr)
    {
        LOG_FATAL << "Could not load Capstone Engine. "
            << "Error = " << std::hex << GetLastError();
        exit(-1);
    }

    cs_open = (cs_openFnc)GetProcAddress(module, "cs_open");
    if (cs_open == nullptr)
    {
        CapstoneImportError("cs_open.");
    }

    cs_disasm = (cs_disasmFnc)GetProcAddress(module, "cs_disasm");
    if (cs_disasm == nullptr)
    {
        CapstoneImportError("cs_disasm.");
    }

    cs_free = (cs_freeFnc)GetProcAddress(module, "cs_free");
    if (cs_free == nullptr)
    {
        CapstoneImportError("cs_free.");
    }

    cs_close = (cs_closeFnc)GetProcAddress(module, "cs_close");
    if (cs_close == nullptr)
    {
        CapstoneImportError("cs_close");
    }
}

void CapstoneImportError(const std::string error)
{
    LOG_FATAL << "Could not resolve " << error.c_str() << " "
        << "Error = " << std::hex << GetLastError();
    exit(-1);
}

}
}