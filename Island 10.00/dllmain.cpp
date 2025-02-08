#include "framework.h"
#include "Server/GameMode/GameMode.h"
#include "Other/logger.cpp"

DWORD WINAPI Main(LPVOID)
{
    BaseAddressHolder BaseAddress;
    debug = true; // Turn this into true/false if you want debug logging.

    AllocConsole();
    SetConsoleTitleA(("Island 10.00 || Initializing..."));
    FILE* File = nullptr;
    freopen_s(&File, "CONOUT$", "w", stdout);
    stopfnlogs();

    MH_Initialize();

    LogDebug(std::format("ImageBase: {}", reinterpret_cast<uintptr_t>(GetModuleHandle(0))));

    Log("Intializing hooks");
    auto* GameModeObj = AFortGameModeAthena::GetDefaultObj();
    if (GameModeObj)
    {
        void** VTable = *reinterpret_cast<void***>(GameModeObj);
        Utils::VirtualHook(VTable, 0x74, GameMode::ReadyToStartMatch);
    }
    else
    {
        LogError("Failed to get AFortGameModeAthena default object.");
    }

    //SDK::UFunction* FN = UObject::FindObject<UFunction>("Function ReadyToStartMatch");

    UKismetSystemLibrary::GetDefaultObj()->ExecuteConsoleCommand(UWorld::GetWorld(), L"open Athena_Terrain", nullptr);

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

