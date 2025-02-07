#include "GameMode.h"
#include "..\Other\logger.h"

static bool (*ReadyToStartMatchOG)(AFortGameModeAthena*);

bool ReadyToStartMatch(AFortGameModeAthena* GameMode) {
    static bool bPlaylist = false;
    static bool bListening = false;

    if (!GameMode || !GameMode->GameState) {
        LogError("Invalid GameMode or GameState!");
        return false;
    }

    auto GameState = static_cast<AFortGameStateAthena*>(GameMode->GameState);
    if (!GameState->MapInfo) {
        LogError("GameState has no MapInfo!");
        return false;
    }

    if (!bPlaylist) {
        LogDebug("Initializing playlist...");
        auto Playlist = UObject::FindObject<UFortPlaylistAthena>("FortPlaylistAthena Playlist_DefaultSolo.Playlist_DefaultSolo");
        if (!Playlist) {
            LogError("Failed to find playlist!");
            return false;
        }

        GameState->CurrentPlaylistInfo.BasePlaylist = Playlist;
        GameState->CurrentPlaylistInfo.PlaylistReplicationKey++;
        GameState->CurrentPlaylistInfo.MarkArrayDirty();
        GameState->OnRep_CurrentPlaylistInfo();

        GameMode->CurrentPlaylistId = Playlist->PlaylistId;
        GameMode->CurrentPlaylistName = Playlist->PlaylistName;
        bPlaylist = true;
    }

    if (!bListening) {
        LogDebug("Initializing NetDriver...");
        auto CreateNetDriver = reinterpret_cast<SDK::UNetDriver * (*)(UEngine*, UWorld*, FName)>(uintptr_t(GetModuleHandle(0)) + 0x5285050);
        FName NetDriverDefinition = UKismetStringLibrary::Conv_StringToName(L"GameNetDriver");

        auto World = UWorld::GetWorld();
        auto Driver = CreateNetDriver(UEngine::GetEngine(), World, NetDriverDefinition);
        if (!Driver) {
            LogError("Failed to create NetDriver!");
            return false;
        }

        World->NetDriver = Driver;
        World->NetDriver->NetDriverName = NetDriverDefinition;
        World->NetDriver->World = World;

        FString Error;
        FURL URL;
        URL.Port = 7777;

        LogDebug("Starting to listen on port %d...", URL.Port);
        if (!InitListen(World, URL, false, Error)) {
            LogError("Failed to initialize listening: %s", *Error);
            return false;
        }
        if (InitListen) {
            Log("Starting to listen on port %d...", URL.Port);
        }


        SetWorld(World);
        for (auto& Collection : World->LevelCollections) {
            Collection.NetDriver = World->NetDriver;
        }

        SetConsoleTitleA("Island 10.00 || Listening");
        GameMode->bWorldIsReady = true;
        bListenting = true;
    }

    return ReadyToStartMatchOG(GameMode);
}
