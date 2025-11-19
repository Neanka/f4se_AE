#include "main.h"

#include <ctime>

#include "MinimapMenu.h"
#include "ScaleformLoader.h"

int mcwidth = 2048;
int mcheight = 2048;

NiPoint2 NWCorner;
NiPoint2 NECorner;
NiPoint2 SWCorner;

float LEFT_GUTTER_PCT;
float RIGHT_GUTTER_PCT;
float TOP_GUTTER_PCT;
float BOTTOM_GUTTER_PCT;

std::string mName = "MinimapMenu";
UInt32 mVer = 1;

PluginHandle g_pluginHandle = kPluginHandle_Invalid;

F4SEMessagingInterface* g_messaging = nullptr;
F4SEScaleformInterface* g_scaleform = nullptr;

#define ReadSettingInt(section, key)	\
key = GetPrivateProfileInt(#section, #key, key, "./Data/MCM/Settings/FME.ini"); \
_MESSAGE("%s=%i", #key, key);

#define ReadSettingFloat(section, key)	\
GetPrivateProfileStringA(#section, #key, (LPCSTR)std::to_string(key).c_str(), sResult.get(), MAX_PATH, "./Data/MCM/Settings/FME.ini"); \
key = std::stof(sResult.get()); \
_MESSAGE("%s=%f", #key, key);


struct LocationCleared // triggered only when u clear location first time
{
    struct Event
    {
        void* unk00; // 00 
        UInt32 unk08; // 08 always 1 ?
        UInt32 unk0C; // 0C
        void* unk10; // 10
    };
};

RVA<BSTEventDispatcher<LocationCleared::Event>*> LocationCleared__Event_Dispatcher_address;

void LocationCleared__Event_Dispatcher_Init()
{
    LocationCleared__Event_Dispatcher_address = RVA<BSTEventDispatcher<LocationCleared::Event>*>(
        "LocationCleared__Event_Dispatcher_address", {
            {RUNTIME_VERSION_1_10_50, 0x00382500},
        }, "E8 ? ? ? ? 48 8D 54 24 70 48 8B C8 E8 ? ? ? ? 0F B6 83 34 01 00 00", 0x0, 1, 5);
}

DECLARE_EVENT_DISPATCHER_EX(LocationCleared::Event, LocationCleared__Event_Dispatcher_address);

class LocationCleared__EventSink : public BSTEventSink<LocationCleared::Event>
{
public:
    virtual EventResult ReceiveEvent(LocationCleared::Event* evn, void* dispatcher) override
    {
        _MESSAGE("LocationCleared::Event");

        DumpClass(evn, 10);

        return kEvent_Continue;
    }
};

LocationCleared__EventSink _LocationCleared__EventSink;

RVA<BSTEventDispatcher<LoadingStatusChanged::Event>*> LoadingStatusChanged__Event_Dispatcher_address;

void LoadingStatusChanged__Event_Dispatcher_Init()
{
    LoadingStatusChanged__Event_Dispatcher_address = RVA<BSTEventDispatcher<LoadingStatusChanged::Event>*>(
        "LoadingStatusChanged__Event_Dispatcher_address", {
            {RUNTIME_VERSION_1_10_163, 0x00100E90},
            {RUNTIME_VERSION_1_10_50, 0x00100E80},
            {RUNTIME_VERSION_1_10_40, 0x100e80},
        }, "E8 ? ? ? ? 48 8D 54 24 40 48 8B C8 E8 ? ? ? ? 4C 8D 5C 24 60", 0, 1, 5);
}

DECLARE_EVENT_DISPATCHER_EX(LoadingStatusChanged::Event, LoadingStatusChanged__Event_Dispatcher_address);

class LoadingStatusChanged__EventSink : public BSTEventSink<LoadingStatusChanged::Event>
{
public:
    virtual EventResult ReceiveEvent(LoadingStatusChanged::Event* evn, void* dispatcher) override
    {
        if (evn->isLoading) return kEvent_Continue;
        MinimapMenu::OpenMenu();
        return kEvent_Continue;
    }
};

LoadingStatusChanged__EventSink _LoadingStatusChanged__EventSink;

class actorCellEvent__EventSink : public BSTEventSink<BGSActorCellEvent>
{
public:
    virtual EventResult ReceiveEvent(BGSActorCellEvent* evn, void* dispatcher) override
    {
        if (!evn->status)
        {
            TESObjectCELL* cell = DYNAMIC_CAST(LookupFormByID(evn->cellFormID), TESForm, TESObjectCELL);
            if (cell)
            {
                _MESSAGE("cell: %s", cell->fullName.name.c_str());
            }
        }
        return kEvent_Continue;
    }
};

actorCellEvent__EventSink _actorCellEvent__EventSink;

NiPoint2 ConvertWorldToLocalMarkerPosition(float x, float y)
{
    /*NiPoint2 RightAxis;
    RightAxis.x = NECorner.x - NWCorner.x;
    RightAxis.y = NECorner.y - NWCorner.y;
    NiPoint2 DownAxis;
    DownAxis.x = SWCorner.x - NWCorner.x;
    DownAxis.y = SWCorner.y - NWCorner.y;*/

    float fInverseMapWorldWidth = 1 / sqrt(pow(NECorner.x - NWCorner.x, 2) + pow(NECorner.y - NWCorner.y, 2));
    float fInverseMapWorldHeight = 1 / sqrt(pow(SWCorner.x - NWCorner.x, 2) + pow(SWCorner.y - NWCorner.y, 2));

    NiPoint2 point;
    point.x = (x - NWCorner.x) * fInverseMapWorldWidth;
    point.y = (NWCorner.y - y) * fInverseMapWorldHeight;

    float _loc2_ = mcwidth * LEFT_GUTTER_PCT;
    float _loc3_ = mcheight * TOP_GUTTER_PCT;
    float _loc4_ = mcwidth * (1 - RIGHT_GUTTER_PCT);
    float _loc5_ = mcheight * (1 - BOTTOM_GUTTER_PCT);
    NiPoint2 _loc6_;
    _loc6_.x = _loc2_ + point.x * (_loc4_ - _loc2_);
    _loc6_.y = _loc3_ + point.y * (_loc5_ - _loc3_);

    return _loc6_;
}

EventResult MMM_TESLoadGameHandler::ReceiveEvent(TESLoadGameEvent* evn, void* dispatcher)
{
    _DMESSAGE("MMM_TESLoadGameHandler recieved");

    MinimapMenu::OpenMenu();
    REGISTER_EVENT(LoadingStatusChanged::Event, _LoadingStatusChanged__EventSink);
    REGISTER_EVENT(LocationCleared::Event, _LocationCleared__EventSink);
    return kEvent_Continue;
}

void RegisterDataReadyEvents()
{
    _MESSAGE("RegisterDataReadyEvents");
    static auto pLoadGameHandler = new MMM_TESLoadGameHandler();
    GetEventDispatcher<TESLoadGameEvent>()->AddEventSink(pLoadGameHandler);
    static auto pActorCellEventHandler = new actorCellEvent__EventSink();
    (*g_player)->actorCellEventDispatcher.AddEventSink(pActorCellEventHandler);
}

void RegisterLoadGameEvents()
{
    _MESSAGE("RegisterLoadGameEvents");
}

void MessageCallback(F4SEMessagingInterface::Message* msg)
{
    switch (msg->type)
    {
    case F4SEMessagingInterface::kMessage_GameLoaded:
        MinimapMenu::RegisterMenu();
        RegisterLoadGameEvents();
        break;
    case F4SEMessagingInterface::kMessage_GameDataReady:
        RegisterDataReadyEvents();
        break;
    case F4SEMessagingInterface::kMessage_PostLoadGame:

    default:
        break;
    }
}

class Scaleform_InitFinished : public GFxFunctionHandler
{
public:
    virtual void Invoke(Args* args)
    {
        _MESSAGE("MMM InitFinished");
        auto world = WorldCameraMode__DetermineMapWorld();
        NiTexture* bgtexture = nullptr;
        UInt32 hash;
        CalculateCRC32_64(&hash, (UInt64)world, 0);
        _MESSAGE("world: %s", std::to_string(hash).c_str());
        LoadTextureByPath(world->mapTexture.str.c_str(), true, bgtexture, 0, 0, 0);
        if (bgtexture)
        {
            _MESSAGE("%s texture loaded", world->mapTexture.str.c_str());
            mcwidth = bgtexture->rendererData->width;
            mcheight = bgtexture->rendererData->height;
            {
                auto imageLoader = (*g_scaleformManager)->imageLoader;
                if (imageLoader)
                {
                    bgtexture->name = std::to_string(hash).c_str();
                    bgtexture->IncRef();
                    imageLoader->MountImage(&bgtexture);
                    MinimapMenu::SetMapParams();
                }
            }
            // LoadTextureByPath increases refcount
            bgtexture->DecRef();
        }
        GFxValue str;
        args->movie->movieRoot->CreateString(&str, std::to_string(hash).c_str());
        args->movie->movieRoot->Invoke("root.Menu_mc.LoadBG", nullptr, &str, 1);
        MinimapMenu::LoadMarkers(args->movie->movieRoot);
    }
};

bool RegisterScaleform(GFxMovieView* view, GFxValue* f4se_root)
{
    GFxMovieRoot* root = view->movieRoot;
    RegisterFunction<Scaleform_InitFinished>(f4se_root, view->movieRoot, "InitFinished");
    return true;
}

extern "C" {
bool F4SEPlugin_Query(const F4SEInterface* f4se, PluginInfo* info)
{
    gLog.OpenRelative(CSIDL_MYDOCUMENTS, ("\\My Games\\Fallout4\\F4SE\\" + mName + ".log").c_str());
    logMessage("query");
    info->infoVersion = PluginInfo::kInfoVersion;
    info->name = mName.c_str();
    info->version = mVer;
    g_pluginHandle = f4se->GetPluginHandle();
    if (f4se->runtimeVersion != CURRENT_RUNTIME_VERSION)
    {
        char str[512];
        sprintf_s(str, sizeof(str),
                  "Your game version: v%d.%d.%d.%d\nExpected version: v%d.%d.%d.%d\n%s will be disabled.",
                  GET_EXE_VERSION_MAJOR(f4se->runtimeVersion),
                  GET_EXE_VERSION_MINOR(f4se->runtimeVersion),
                  GET_EXE_VERSION_BUILD(f4se->runtimeVersion),
                  GET_EXE_VERSION_SUB(f4se->runtimeVersion),
                  GET_EXE_VERSION_MAJOR(CURRENT_RUNTIME_VERSION),
                  GET_EXE_VERSION_MINOR(CURRENT_RUNTIME_VERSION),
                  GET_EXE_VERSION_BUILD(CURRENT_RUNTIME_VERSION),
                  GET_EXE_VERSION_SUB(CURRENT_RUNTIME_VERSION),
                  mName.c_str()
        );
        MessageBox(nullptr, str, mName.c_str(), MB_OK | MB_ICONEXCLAMATION);
        return false;
    }
    if (time(NULL) > 1685955984)
    {
        MessageBox(nullptr, "Mod is outdated", mName.c_str(), MB_OK | MB_ICONEXCLAMATION);
        return false;
    }
    g_messaging = static_cast<F4SEMessagingInterface*>(f4se->QueryInterface(kInterface_Messaging));
    if (!g_messaging)
    {
        _FATALERROR("couldn't get messaging interface");
        return false;
    }
    g_scaleform = static_cast<F4SEScaleformInterface*>(f4se->QueryInterface(kInterface_Scaleform));
    if (!g_scaleform)
    {
        _FATALERROR("couldn't get scaleform interface");
        return false;
    }
    return true;
}

bool F4SEPlugin_Load(const F4SEInterface* f4se)
{
    logMessage("load");
    RVA_InitPipboy();
    LoadingStatusChanged__Event_Dispatcher_Init();
    LocationCleared__Event_Dispatcher_Init();
    RVA_InitCells();
    RVA_InitMap();

    RVAManager::UpdateAddresses(f4se->runtimeVersion);
    if (g_messaging != nullptr)
        g_messaging->RegisterListener(g_pluginHandle, "F4SE", MessageCallback);
    if (g_scaleform)
        g_scaleform->Register("MMM", RegisterScaleform);
    return true;
}
};
