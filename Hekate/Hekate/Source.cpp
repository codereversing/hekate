#ifdef _M_IX86
#ifdef _DEBUG
#pragma comment(lib, "protobuf/lib/libprotobufd_x86.lib")
#else
#pragma comment(lib, "protobuf/lib/libprotobuf_x86.lib")
#endif
#elif defined _M_AMD64
#ifdef _DEBUG
#pragma comment(lib, "protobuf/lib/libprotobufd_x64.lib")
#else
#pragma comment(lib, "protobuf/lib/libprotobuf_x64.lib")
#endif
#else
#error "Unsupported platform"
#endif

#include <Windows.h>

#include "App/HekateMain.h"
#include "Capstone/CapstoneDll.h"
#include "Hook/InlineHook.h"
#include "Hook/HookEngine.h"
#include "Protobuf/Message/HekateClientMessageBuilder.h"
#include "Protobuf/Proto/Server/HekateServerProto.pb.h"

#include "plog/Log.h"

void Initialize()
{
#ifdef VERBOSE
    plog::init(plog::verbose, "C:/Temp/log.txt");
#elif _DEBUG
    plog::init(plog::debug, "C:/Temp/log.txt");
#else
    plog::init(plog::error, "log.txt");
#endif

    Hekate::Capstone::ResolveCapstoneImports();
}

#ifdef DLL_BUILD

static std::unique_ptr<Hekate::App::HekateMain> pHekate;
static HANDLE hThread = INVALID_HANDLE_VALUE;

DWORD WINAPI HekateThreadEntry(LPVOID lpParameter)
{
    Initialize();
    pHekate = std::unique_ptr<Hekate::App::HekateMain>(new Hekate::App::HekateMain);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    switch(dwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hModule);

        DWORD dwThreadId = 0;
        hThread = CreateThread(nullptr, 0, HekateThreadEntry, nullptr, 0, &dwThreadId);

        break;
    }
    case DLL_PROCESS_DETACH:

        if (pHekate.get() != nullptr)
        {
            pHekate.release();
        }
        if (hThread != INVALID_HANDLE_VALUE)
        {
            (void)CloseHandle(hThread);
        }

        google::protobuf::ShutdownProtobufLibrary();

        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;
    }

    return TRUE;
}

#endif
