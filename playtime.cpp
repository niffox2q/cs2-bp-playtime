#include "playtime.h"
#include <cstdio>
#include <algorithm>

#include "ahh.h"

Playtime g_Playtime;
PLUGIN_EXPOSE(Playtime, g_Playtime);


IVEngineServer2* engine = nullptr;
CGlobalVars* gpGlobals = nullptr;
CGameEntitySystem* g_pGameEntitySystem = nullptr;
CEntitySystem* g_pEntitySystem = nullptr;

IUtilsApi* utils = nullptr;
IMenusApi* menus_api;
IPlayersApi* players_api;
IBattlePassApi* bp_api;

std::string sServerMod;

#define MAX_PLAYERS 64

void LoadConfig() {
    KeyValues* config = new KeyValues("Config");
    const char* path = "addons/configs/Battlepass/core.ini";
    if (!config->LoadFromFile(g_pFullFileSystem, path)) {
        utils->ErrorLog("%s Failed to load: %s",g_PLAPI->GetLogTag(), path);
        delete config;
        return;
        
    }

    sServerMod = config->GetString("mod","");


    delete config;
}

CGameEntitySystem* GameEntitySystem() {
    return utils ? utils->GetCGameEntitySystem() : nullptr;
}

void StartupServer() {
    g_pGameEntitySystem = GameEntitySystem();
    g_pEntitySystem = utils->GetCEntitySystem();
    gpGlobals = utils->GetCGlobalVars();

}

bool Playtime::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late) {
    PLUGIN_SAVEVARS();

    GET_V_IFACE_CURRENT(GetEngineFactory, g_pCVar, ICvar, CVAR_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetEngineFactory, g_pSchemaSystem, ISchemaSystem, SCHEMASYSTEM_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetFileSystemFactory, g_pFullFileSystem, IFileSystem, FILESYSTEM_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer2, SOURCE2ENGINETOSERVER_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetServerFactory, g_pSource2Server, ISource2Server, SOURCE2SERVER_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetServerFactory, g_pSource2GameClients, IServerGameClients, SOURCE2GAMECLIENTS_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetServerFactory, g_pSource2GameEntities, ISource2GameEntities, SOURCE2GAMEENTITIES_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pNetworkSystem, INetworkSystem, NETWORKSYSTEM_INTERFACE_VERSION);


    ConVar_Register(FCVAR_SERVER_CAN_EXECUTE | FCVAR_GAMEDLL);
    g_SMAPI->AddListener(this, this);

    

    return true;
}

void Playtime::AllPluginsLoaded() {
    int ret;
    utils = (IUtilsApi*)g_SMAPI->MetaFactory(Utils_INTERFACE, &ret, nullptr);
    if (ret == META_IFACE_FAILED) {
        META_CONPRINTF("%s | Missing UTILS plugin.\n", g_PLAPI->GetLogTag());
        engine->ServerCommand(("meta unload " + std::to_string(g_PLID)).c_str());
        return;
    }

    menus_api = (IMenusApi*)g_SMAPI->MetaFactory(Menus_INTERFACE, &ret, nullptr);
    if (ret == META_IFACE_FAILED) {
        META_CONPRINTF("%s | Missing UTILS plugin.",g_PLAPI->GetLogTag());
        engine->ServerCommand(("meta unload " + std::to_string(g_PLID)).c_str());
        return;
    }

    bp_api =  (IBattlePassApi*)g_SMAPI->MetaFactory(BP_INTERFACE, &ret, nullptr);
    if (ret == META_IFACE_FAILED) {
        META_CONPRINTF("%s | Missing Battlepass Core plugin.",g_PLAPI->GetLogTag());
        engine->ServerCommand(("meta unload " + std::to_string(g_PLID)).c_str());
        return;
    }

    utils->CreateTimer(60.0f,[](){
        for (int iSlot = 0; iSlot <= MAX_PLAYERS;iSlot++) {
            auto pController = CCSPlayerController::FromSlot(iSlot);
            if (!pController) continue;
            auto pPawn = pController->GetPlayerPawn();
            if (!pPawn) continue;
            if (pPawn->IsBot()) continue;

            int iTeam = pPawn->GetTeam();
            if (iTeam < 1 || iTeam > 3) continue;

            bp_api->SendCustomEvent(iSlot, "playtime", {{"mod",sServerMod}}, {{"team", iTeam}});
        }
        
        return 60.0f;
    });

}

bool Playtime::Unload(char* error, size_t maxlen) {

    utils->ClearAllHooks(g_PLID);
    ConVar_Unregister();
    return true;
}

const char* Playtime::GetAuthor() { return "niffox"; }
const char* Playtime::GetDate() { return __DATE__; }
const char* Playtime::GetDescription() { return "[BP] Playtime"; }
const char* Playtime::GetLicense() { return "GPL"; }
const char* Playtime::GetLogTag() { return "[BP] Playtime"; }
const char* Playtime::GetName() { return "[BP] Playtime"; }
const char* Playtime::GetURL() { return "https://t.me/niffox_2q"; }
const char* Playtime::GetVersion() { return "1.0.0"; }