#include "GameMode.h"
#include "..\Other\logger.h"
#include "..\Other\globals.cpp"

static bool (*OGReadyToStartMatch)(AFortGameModeAthena*);

inline UNetDriver* (*CreateNetDriver)(UEngine*, UWorld*, FName) = decltype(CreateNetDriver)(BaseAddress + 0x33F0490);
inline bool (*InitListen)(UNetDriver*, void*, FURL&, bool, FString&) = decltype(InitListen)(BaseAddress + 0x6D9070);
inline void* (*SetWorld)(UNetDriver*, UWorld*) = decltype(SetWorld)(BaseAddress + 0x315EF10);
inline void (*ServerReplicateActors)(UReplicationDriver*) = decltype(ServerReplicateActors)(BaseAddress + 0xA0DAC0);

bool ReadyToStartMatch(AFortGameModeAthena* GameMode) {
    static bool bPlaylist = false;
    static bool bNetDriver = false;

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
        UFortPlaylistAthena* Playlist = nullptr;

        if (bSolos) {
            Playlist = UObject::FindObject<UFortPlaylistAthena>("FortPlaylistAthena Playlist_DefaultSolo.Playlist_DefaultSolo");
        }
        else if (bDuos) {
            Playlist = UObject::FindObject<UFortPlaylistAthena>("FortPlaylistAthena Playlist_DefaultDuo.Playlist_DefaultDuo");
        }

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

    if (!bNetDriver) {
        LogDebug("Initializing NetDriver...");

        auto World = UWorld::GetWorld();
        UEngine* Engine = UEngine::GetEngine();
        if (!Engine || !World) {
            LogError("Failed to retrieve Engine or World!");
            return false;
        }

        World->NetDriver = CreateNetDriver(Engine, World, FName(L"GameNetDriver"));
        if (!World->NetDriver) {
            LogError("Failed to create NetDriver!");
            return false;
        }

        World->NetDriver->World = World;
        World->NetDriver->NetDriverName = FName(L"GameNetDriver");

        if (GameMode->GameSession) {
            GameMode->GameSession->MaxPlayers = GameState->CurrentPlaylistInfo.BasePlaylist->MaxPlayers;
        }
        else {
            LogError("GameSession is null!");
        }

        for (auto& Collection : World->LevelCollections) {
            Collection.NetDriver = World->NetDriver;
        }

        GameState->AirCraftBehavior = GameState->CurrentPlaylistInfo.BasePlaylist->AirCraftBehavior;
        GameState->CachedSafeZoneStartUp = GameState->CurrentPlaylistInfo.BasePlaylist->SafeZoneStartUp;

        FString Error;
        if (!InitListen(World->NetDriver, World, World->PersistentLevel->URL, false, Error)) {
            LogError("Failed to listen on port " + std::to_string(Port) + "! Error: " + Error.ToString());
            return false;
        }

        SetWorld(World->NetDriver, World);
        Log("Listening on Port " + std::to_string(Port) + "!");
        SetConsoleTitleA("Island 10.00 || Listening");

        GameMode->bWorldIsReady = true;
        bNetDriver = true;
    }

    return OGReadyToStartMatch(GameMode);
}
