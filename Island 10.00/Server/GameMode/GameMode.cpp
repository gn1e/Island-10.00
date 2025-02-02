#include "GameMode.h"
#include "..\Other\logger.h"

static bool (*ReadyToStartMatchOG)(AFortGameModeAthena*);
bool ReadyToStartMatch(AFortGameModeAthena* GameMode) {
    static bool bPlaylist = false;
    static bool bListening = false;

    auto GameState = static_cast<AFortGameStateAthena*>(GameMode->GameState);
    if (!GameState || !GameState->MapInfo) 
        return false;

    if (!bPlaylist) {
        LogDebug("Parsing playlist!");
        auto Playlist = UObject::FindObject<UFortPlaylistAthena>("FortPlaylistAthena Playlist_DefaultSolo.Playlist_DefaultSolo");
        if (!Playlist) {
            LogError("Failed to parse playlist!");
            return false;
        }

        GameState->CurrentPlaylistInfo.BasePlaylist = Playlist;
        GameState->CurrentPlaylistInfo.PlaylistReplicationKey++;
        GameState->CurrentPlaylistInfo.MarkArrayDirty();
        GameState->OnRep_CurrentPlaylistInfo();

        GameMode->CurrentPlaylistId = Playlist->PlaylistId;
        GameMode->CurrentPlaylistName = Playlist->PlaylistName;
    }

    if (!bListening) {
        LogDebug("Creating NetDriver!");
        SDK::UNetDriver* (*CreateNetDriver)(UEngine * Engine, UWorld * InWorld, FName NetDriverDefinition) = decltype(CreateNetDriver)(uintptr_t(GetModuleHandle(0)) + 0x5285050);
        FName NetDriverDefinition = UKismetStringLibrary::Conv_StringToName(L"GameNetDriver");

        auto Driver = CreateNetDriver(UEngine::GetEngine(), UWorld::GetWorld(), NetDriverDefinition); // i THINK this works!! not sure tho!!
        if (!Driver) {
            LogError("Failed to create NetDriver!");
            return false;
        }

    /*    UWorld::GetWorld()->NetDriver = Driver;
        UWorld::GetWorld()->NetDriver->NetDriverName = NetDriverDefinition;
        UWorld::GetWorld()->NetDriver->World = UWorld::GetWorld();

        FString Error;
        FURL URL;
        URL.Port = 7777;


        LogDebug("Trying to listen!");
        InitListen(UWorld::GetWorld(), URL, false, Error);
        LogDebug("Setting world!");
        SetWorld(UWorld::GetWorld()); */

        for (auto& Collection : UWorld::GetWorld()->LevelCollections) {
            Collection.NetDriver = UWorld::GetWorld()->NetDriver;
        }
        SetConsoleTitleA("Island 10.00 || Listening");
        GameMode->bWorldIsReady = true;
    }


    // chapter 2 support/?? i thinkkkk
    if (GameMode->bDelayedStart)
        return false;

    FName WaitingToStartName = UKismetStringLibrary::Conv_StringToName(L"WaitingToStart");
    if (GameMode->MatchState == WaitingToStartName)
    {
        if (GameMode->NumPlayers + GameMode->NumBots > 0)
        {
            return true;
        }
    }
    return false;
}