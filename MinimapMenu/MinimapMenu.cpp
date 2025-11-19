#include "MinimapMenu.h"

#include "ScaleformLoader.h"

std::map<UInt32, UInt32> markersMap;

PipboyObject * player;
PipboyPrimitiveThrottledValue<float>* X = nullptr;
PipboyPrimitiveThrottledValue<float>* Rotation = nullptr;
PipboyPrimitiveThrottledValue<float>* Y = nullptr;
float _x = 0;
float _y = 0;
float _rotation = 0;


void MinimapMenu::LoadMarkers(GFxMovieRoot* root)
{
    _MESSAGE("LoadMarkers");
    markersMap.clear();
    EnterCriticalSection(&(*g_PipboyDataManager)->mapData.lpcs);
    auto mapDataObject = (*g_PipboyDataManager)->mapData.mapDataObject;
    auto worldObject = static_cast<PipboyObject*>(mapDataObject->table.Find(&BSFixedString("World"))->value);
    auto locationsArray = static_cast<PipboyArray*>(worldObject->table.Find(&BSFixedString("Locations"))->value);
    tracePipboyArray(locationsArray);
    GFxValue _arr, _obj, _type, _mX, _mY, _Discovered, _LocationMarkerFormId, _Name, _LocationFormId;
    GFxValue _ClearedStatus, _Visible, _WorkshopOwned, _WorkshopPopulation, _WorkshopHappinessPct;
    root->CreateArray(&_arr);
    for (size_t i = 0; i < locationsArray->value.count; i++)
    {
        root->CreateObject(&_obj);
        const auto it = static_cast<PipboyObject*>(locationsArray->value[i]);
        auto type = static_cast<PipboyPrimitiveValue<UInt32>*>(it->table.Find(&BSFixedString("type"))->value);
        _type.SetUInt(type->value);
        _obj.SetMember("type", &_type);
        auto mX = static_cast<PipboyPrimitiveValue<float>*>(it->table.Find(&BSFixedString("X"))->value);
        auto mY = static_cast<PipboyPrimitiveValue<float>*>(it->table.Find(&BSFixedString("Y"))->value);
        NiPoint2 pos = ConvertWorldToLocalMarkerPosition(mX->value, mY->value);
        _mX.SetNumber(pos.x);
        _obj.SetMember("X", &_mX);
        _mY.SetNumber(pos.y);
        _obj.SetMember("Y", &_mY);
        auto Discovered = static_cast<PipboyPrimitiveValue<bool>*>(it->table.Find(&BSFixedString("Discovered"))->value);
        _Discovered.SetBool(Discovered->value);
        _obj.SetMember("Discovered", &_Discovered);
        auto LocationFormId = static_cast<PipboyPrimitiveValue<UInt32>*>(it->table.Find(&BSFixedString("LocationFormId"))->value);
        _LocationFormId.SetUInt(LocationFormId->value);
        _obj.SetMember("LocationFormId", &_LocationFormId);
        auto LocationMarkerFormId = static_cast<PipboyPrimitiveValue<UInt32>*>(it->table.Find(&BSFixedString("LocationMarkerFormId"))->value);
        _LocationMarkerFormId.SetUInt(LocationMarkerFormId->value);
        _obj.SetMember("LocationMarkerFormId", &_LocationMarkerFormId);
        auto Name = static_cast<PipboyPrimitiveValue<BSFixedString>*>(it->table.Find(&BSFixedString("Name"))->value);
        root->CreateString(&_Name, Name->value);
        _obj.SetMember("Name", &_Name);

        auto Visible = static_cast<PipboyPrimitiveValue<bool>*>(it->table.Find(&BSFixedString("Visible"))->value);
        _Visible.SetBool(Visible->value);
        _obj.SetMember("Visible", &_Visible);

        HUDMarkerData__GetFlashSymbolNameForMapMarker_struct str1;
        str1.type = type->value;
        _MESSAGE("icon: %s", HUDMarkerData__GetFlashSymbolNameForMapMarker(str1)->c_str());
        GFxValue _classname;
        root->CreateString(&_classname, HUDMarkerData__GetFlashSymbolNameForMapMarker(str1)->c_str());
        _obj.SetMember("classname", &_classname);
        
        _arr.PushBack(&_obj);

    }
    traceGFxValue(&_arr);
    root->Invoke("root.Menu_mc.PopArray", nullptr, &_arr, 1);



    LeaveCriticalSection(&(*g_PipboyDataManager)->mapData.lpcs);


}

void MinimapMenu::SetMapParams()
{
    _MESSAGE("SetMapParams");
    _x = 0;
    _y = 0;
    _rotation = 0;
    _MESSAGE("Texture: width: %i, height: %i", mcwidth, mcheight);
    const auto worldmapextentsObject = (*g_PipboyDataManager)->mapData.worldMapExtents;
    NWCorner.x = static_cast<PipboyPrimitiveValue<float>*>(worldmapextentsObject->table.Find(&BSFixedString("NWX"))->value)->value;
    NWCorner.y = static_cast<PipboyPrimitiveValue<float>*>(worldmapextentsObject->table.Find(&BSFixedString("NWY"))->value)->value;
    NECorner.x = static_cast<PipboyPrimitiveValue<float>*>(worldmapextentsObject->table.Find(&BSFixedString("NEX"))->value)->value;
    NECorner.y = static_cast<PipboyPrimitiveValue<float>*>(worldmapextentsObject->table.Find(&BSFixedString("NEY"))->value)->value;
    SWCorner.x = static_cast<PipboyPrimitiveValue<float>*>(worldmapextentsObject->table.Find(&BSFixedString("SWX"))->value)->value;
    SWCorner.y = static_cast<PipboyPrimitiveValue<float>*>(worldmapextentsObject->table.Find(&BSFixedString("SWY"))->value)->value;
    _MESSAGE("corners: %f %f %f %f", NECorner.x, NECorner.y, SWCorner.x, SWCorner.y);
    
    auto str = std::string(TESWorldSpace__GetWorldMapTextureName(WorldCameraMode__DetermineMapWorld()));
    if (str.find("WorldMap_d") != std::string::npos)
    {
        LEFT_GUTTER_PCT = 0.02685546875;
        RIGHT_GUTTER_PCT = 0.0283203125;
        TOP_GUTTER_PCT = 0.02685546875;
        BOTTOM_GUTTER_PCT = 0.0283203125;
    }
    else if (str.find("MapFarHarbor") != std::string::npos)
    {
        LEFT_GUTTER_PCT = 0.00920067;
        RIGHT_GUTTER_PCT = 0.03374173;
        TOP_GUTTER_PCT = 0.02515624;
        BOTTOM_GUTTER_PCT = 0.00958931;
    }
    else if (str.find("NukaWorldMap") != std::string::npos)
    {
        LEFT_GUTTER_PCT = -0.045;
        RIGHT_GUTTER_PCT = -0.095;
        TOP_GUTTER_PCT = 0.0005;
        BOTTOM_GUTTER_PCT = -0.155;
    }
    else
    {
        LEFT_GUTTER_PCT = 0.02685546875;
        RIGHT_GUTTER_PCT = 0.0283203125;
        TOP_GUTTER_PCT = 0.02685546875;
        BOTTOM_GUTTER_PCT = 0.0283203125;
    }
    _MESSAGE("gutters: %f %f %f %f", LEFT_GUTTER_PCT, RIGHT_GUTTER_PCT, TOP_GUTTER_PCT, BOTTOM_GUTTER_PCT);
}

MinimapMenu::MinimapMenu() :GameMenuBase()
{
    flags = kFlag_AllowSaving | kFlag_DontHideCursorWhenTopmost | kFlag_CustomRendering | kFlag_CompanionAppAllowed; //  
    depth = 0x6;
    if (CALL_MEMBER_FN((*g_scaleformManager), LoadMovie)(this, this->movie, "MinimapMenu", "root1.Menu_mc", 1))
    {
        _DMESSAGE("LoadMovie done");
        CreateBaseShaderTarget(this->filterHolder, this->stage);
        this->filterHolder->SetFilterColor(false);
        
        (*g_colorUpdateDispatcher)->eventDispatcher.AddEventSink(this->filterHolder);

        this->shaderFXObjects.Push(this->filterHolder);
        auto mapDataObject = (*g_PipboyDataManager)->mapData.mapDataObject;
        auto worldObject = static_cast<PipboyObject*>(mapDataObject->table.Find(&BSFixedString("World"))->value);
        auto playerObject = static_cast<PipboyObject*>(worldObject->table.Find(&BSFixedString("Player"))->value);
        X = static_cast<PipboyPrimitiveThrottledValue<float>*>(playerObject->table.Find(&BSFixedString("X"))->value);
        Rotation = static_cast<PipboyPrimitiveThrottledValue<float>*>(playerObject->table.Find(&BSFixedString("Rotation"))->value);
        Y = static_cast<PipboyPrimitiveThrottledValue<float>*>(playerObject->table.Find(&BSFixedString("Y"))->value);
        /*



            LoadTextureByPath(_texturename.c_str(), true, bgtexture, 0, 0, 0);
            if (bgtexture)
            {
                _MESSAGE("%s texture loaded", _texturename.c_str());
                {
                    auto imageLoader = (*g_scaleformManager)->imageLoader;
                    if (imageLoader)
                    {                    
                        bgtexture->name = "worldmap";
                        bgtexture->IncRef();
                        imageLoader->MountImage(&bgtexture);
                        SetMapParams();
                    }
                }
                // LoadTextureByPath increases refcount
                bgtexture->DecRef();
            }
        this->movie->movieRoot->Invoke("root.Menu_mc.LoadBG", nullptr, nullptr, 0);
        */
        this->movie->movieRoot->Invoke("root.Menu_mc.Init", nullptr, nullptr, 0);
    }
}

void MinimapMenu::Invoke(Args* args)
{
    switch (args->optionID)
    {
    case 0:
        {

        }
        break;
    case 1:
        {

        }
        break;
    default:
        break;
    }
}

void MinimapMenu::AdvanceMovie(float unk0, void* unk1)
{
    //_DMESSAGE("MinimapMenu::AdvanceMovie");
    if (_rotation != Rotation->value)
    {
        _rotation = Rotation->value;
        this->movie->movieRoot->SetVariable("root.Menu_mc.mapinst_mc.map_submc.rotation", &GFxValue(-_rotation));
        this->movie->movieRoot->Invoke("root.Menu_mc.mapinst_mc.map_submc.MarkersHolder_mc.SetRotation",nullptr,&GFxValue(_rotation), 1);
    }
    if (_x != X->value || _y != Y->value)
    {
        _x = X->value;
        _y = Y->value;
        NiPoint2 pos = ConvertWorldToLocalMarkerPosition(_x, _y);
        this->movie->movieRoot->SetVariable("root.Menu_mc.mapinst_mc.map_submc.TextureHolder_mc.x", &GFxValue(-pos.x));
        this->movie->movieRoot->SetVariable("root.Menu_mc.mapinst_mc.map_submc.TextureHolder_mc.y", &GFxValue(-pos.y));
        this->movie->movieRoot->SetVariable("root.Menu_mc.mapinst_mc.map_submc.MarkersHolder_mc.x", &GFxValue(-pos.x));
        this->movie->movieRoot->SetVariable("root.Menu_mc.mapinst_mc.map_submc.MarkersHolder_mc.y", &GFxValue(-pos.y));
    }
    return this->GameMenuBase::AdvanceMovie(unk0, unk1);
};

void MinimapMenu::RegisterFunctions()
{
    this->RegisterNativeFunction("f0", 0);
    this->RegisterNativeFunction("f1", 1);
}

UInt32 MinimapMenu::ProcessMessage(UIMessage* msg)
{
    switch (msg->type)
    {
    case kMessage_Open:
        {
            _DMESSAGE("MMM Opened");
            break;
        }
    case kMessage_Close:
        {
            _DMESSAGE("MMM Closed");
            break;
        }
    default:
        break;
    }
    return this->GameMenuBase::ProcessMessage(msg);
}

IMenu* MinimapMenu::CreateMinimapMenu()
{
    return new MinimapMenu();
}

void MinimapMenu::OpenMenu()
{
    _MESSAGE("Open menu");
    static BSFixedString menuName("MinimapMenu");
    CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Open);
}

void MinimapMenu::CloseMenu()
{
    _MESSAGE("Close menu");
    static BSFixedString menuName("MinimapMenu");
    CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Close);
}

void MinimapMenu::RegisterMenu()
{
    static BSFixedString menuName("MinimapMenu");
    if ((*g_ui) != nullptr && !(*g_ui)->IsMenuRegistered(menuName))
    {
        (*g_ui)->Register("MinimapMenu", CreateMinimapMenu);
    }
    _DMESSAGE("RegisterMenu %s", (*g_ui)->IsMenuRegistered(menuName) ? "registered" : "not registered");
}