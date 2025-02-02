#include "framework.h"
#include "Server/GameMode/GameMode.h"
#include "Other/logger.h"

DWORD WINAPI Main(LPVOID)
{
    debug = true; // Turn this into true/false if you want debug logging.

    AllocConsole();
    SetConsoleTitleA(("Island 10.00 || Initializing..."));
    FILE* File = nullptr;
    freopen_s(&File, "CONOUT$", "w", stdout);
    stopfnlogs();

    MH_Initialize();

    LogDebug(std::format("ImageBase: {}", reinterpret_cast<uintptr_t>(GetModuleHandle(0))));

    Log("Intializing hooks");
    Utils::VirtualHook(&(AFortGameModeAthena::GetDefaultObj()->VTable), 0x42, GameMode::ReadyToStartMatch);

    //SDK::UFunction* FN = UObject::FindObject<UFunction>("Function ReadyToStartMatch");

    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"open Athena_Terrian", nullptr);

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == 1)
    {
        CreateThread(nullptr, 0, Main, nullptr, 0, nullptr);
    }
    return TRUE;
}

