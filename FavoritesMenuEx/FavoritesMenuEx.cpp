#include "FavoritesMenuEx.h"
#include "f4se/ScaleformLoader.h"
#include <regex>

#include "FavoritesMenuExInput.h"

FavoritesMenuEx::FavoritesMenuEx() :GameMenuBase()
{
    flags = kFlag_UsesMenuContext | kFlag_UsesCursor | kFlag_UsesMovementToDirection;
    depth = 0x6;
    if (CALL_MEMBER_FN((*g_scaleformManager), LoadMovie)(this, this->movie, "FavoritesMenuEx", "root1.Menu_mc", 1))
    {
        _DMESSAGE("LoadMovie done");
        CreateBaseShaderTarget(this->filterHolder, this->stage);

        if (iUseCustomColors)
        {
            this->filterHolder->HUDColorType = kHUDColorTypes_CustomColor;
            NiColor color;
            color.r = float(iColorR & 0xFF) / 255.0f;
            color.g = float(iColorG & 0xFF) / 255.0f;
            color.b = float(iColorB & 0xFF) / 255.0f;
            this->filterHolder->EnableColorMultipliers(&color, 1.0f);
        }
        else
        {
            this->filterHolder->SetFilterColor(false);
        }
        
        (*g_colorUpdateDispatcher)->eventDispatcher.AddEventSink(this->filterHolder);

        this->shaderFXObjects.Push(this->filterHolder);

        if ((*g_inputDeviceMgr)->IsGamepadEnabled())
        {
            flags &= ~kFlag_UsesCursor;
            this->movie->movieRoot->SetVariable("root.Menu_mc.exitbutton_mc.visible", &GFxValue(false));
        }
    }
}
        
void FavoritesMenuEx::Invoke(Args* args)
{
    switch (args->optionID)
    {
    case 0:
        {
            CloseMenu();
        }
        break;
    case 1:
        {
            UseQuickKey(GetSelection());
        }
        break;
    case 2:
        {
            auto root = args->movie->movieRoot;
            GFxValue arr;
            root->CreateObject(args->result);
            root->CreateArray(&arr);
            for (size_t i = 0; i < (*g_PipboyDataManager)->inventoryData.inventoryObjects.count; i++)
            {
                auto itemobject = (*g_PipboyDataManager)->inventoryData.inventoryObjects.entries[i];
                auto favorite = static_cast<PipboyPrimitiveValue<SInt32>*>(itemobject->table.Find(
                        &BSFixedString("favorite"))->
                    value);
                auto text = static_cast<PipboyPrimitiveValue<BSFixedString>*>(itemobject->table.Find(
                    &BSFixedString("text"))->value);
                
                auto FavIconType = static_cast<PipboyPrimitiveValue<UInt32>*>(itemobject->table.Find(
                        &BSFixedString("FavIconType"))
                    ->value);
                auto count = static_cast<PipboyPrimitiveValue<UInt32>*>(itemobject->table.Find(&BSFixedString("count"))
                    ->value);
                auto HandleID = static_cast<PipboyPrimitiveValue<UInt32>*>(itemobject->table.Find(
                        &BSFixedString("HandleID"))->
                    value);
                auto isLegendary = static_cast<PipboyPrimitiveValue<bool>*>(itemobject->table.Find(
                        &BSFixedString("isLegendary"))->
                    value);
                if (favorite->value > -1)
                {                    
                    GFxValue entry, _favorite, _text, _FavIconType, _count, _isLegendary;
                    root->CreateObject(&entry);
                    std::string __text = text->value;
                    if (iMode == 3)
                    {
                        auto _it = autoTags.find(__text);
                        if (_it != autoTags.end())
                        {
                            __text = std::string("[").append(_it->second).append("] ").append(__text);
                            //_MESSAGE("autotag found: %s", __text.c_str());
                        }      
                    }
                    std::regex re("^[\\[\\|\\(\\{](.*?)[\\}\\)\\|\\]] ?");
                    std::cmatch m;
                    if (std::regex_search(__text.c_str(), m, re))
                    {
                        auto it = icons.find(m[0].str().substr(1, m[0].str().length()-3).c_str());
                        if (it != icons.end())
                        {
                            //_MESSAGE("icon found: %s", it->second.c_str());
                            GFxValue tag;
                            root->CreateString(&tag, it->second.c_str());
                            entry.SetMember("tag", &tag);
                        }                        
                        __text = __text.substr(m[0].str().length(), __text.length()-m[0].str().length());
                    }                    
                    _favorite.SetInt(favorite->value);
                    root->CreateString(&_text, __text.c_str());
                    _FavIconType.SetUInt(FavIconType->value);
                    _count.SetInt(count->value);
                    _isLegendary.SetBool(isLegendary->value);
                    entry.SetMember("favorite", &_favorite);
                    entry.SetMember("text", &_text);
                    entry.SetMember("FavIconType", &_FavIconType);
                    entry.SetMember("count", &_count);
                    entry.SetMember("isLegendary", &_isLegendary);
                    auto item = getInventoryItemByHandleID_int(HandleID->value);
                    if (item->form->formType == kFormType_WEAP)
                    {
                        auto weap = static_cast<TESObjectWEAP*>(item->form);
                        TESObjectWEAP::InstanceData* weapData = &weap->weapData;
                        BGSInventoryItem::Stack* stack = item->stack;
                        auto stackArray = static_cast<PipboyArray*>(itemobject->table.Find(&BSFixedString("StackID"))->
                            value);
                        auto StackID = static_cast<PipboyPrimitiveValue<UInt32>*>(stackArray->value[0]);
                        for (int i = 0; i < StackID->value; i++)
                        {
                            stack = stack->next;
                        }
                        ExtraDataList* stackDataList = stack->extraData;
                        auto eid = DYNAMIC_CAST(stackDataList->GetByType(kExtraData_InstanceData),
                                                BSExtraData, ExtraInstanceData);
                        if (eid)
                        {
                            weapData = static_cast<TESObjectWEAP::InstanceData*>(eid->instanceData);
                        }
                        auto ammo = weapData->ammo;

                        if (ammo != nullptr)
                        {
                            GFxValue _ammoName, _ammoCount;
                            root->CreateString(&_ammoName, ammo->fullName.name.c_str());
                            _ammoCount.SetUInt(GetItemCount(*g_player, ammo));
                            entry.SetMember("ammo", &_ammoName);
                            entry.SetMember("ammoCount", &_ammoCount);
                        }
                    }
                    arr.PushBack(&entry);
                }
            }
            if (iMode != 1)
            {
                GFxValue loader, urlRequest, target;
                root->GetVariable(&target, "root");
                root->CreateObject(&loader, "flash.display.Loader");
                GFxValue contentLoaderInfo;
                loader.GetMember("contentLoaderInfo", &contentLoaderInfo);
                GFxValue listener[2];
                root->CreateString(&listener[0], "complete");
                GFxValue menu;
                target.GetMember("Menu_mc", &menu);
                menu.GetMember("icons_loaded", &listener[1]);
                contentLoaderInfo.Invoke("addEventListener", nullptr, listener, 2);
                GFxValue loadArgs[2];
                GFxValue filePath;
                if (iMode == 3) root->CreateString(&filePath, "FallUI_IconLib.swf");
                else root->CreateString(&filePath, "iconlibs2.swf");
                root->CreateObject(&loadArgs[0], "flash.net.URLRequest", &filePath, 1);
                loadArgs[1].SetNull();
                loader.Invoke("load", nullptr, loadArgs, 2);
            }
            GFxValue Mode; Mode.SetInt(iMode);
            GFxValue gamepadEnabled; gamepadEnabled.SetBool((*g_inputDeviceMgr)->IsGamepadEnabled());
            GFxValue MenuStyle; MenuStyle.SetInt(iMenuStyle);
            GFxValue _fx; _fx.SetNumber(fx);
            GFxValue _fy; _fy.SetNumber(fy);
            GFxValue _fs; _fs.SetNumber(fs);
            args->result->SetMember("iMode", &Mode);
            args->result->SetMember("bGamepadEnabled", &gamepadEnabled);
            args->result->SetMember("iMenuStyle", &MenuStyle);
            args->result->SetMember("fx", &_fx);
            args->result->SetMember("fy", &_fy);
            args->result->SetMember("fs", &_fs);
            args->result->SetMember("arr", &arr);
        }
        break;
    case 3:
        {
            if (args->args[0].GetType() != GFxValue::kType_String) break;
            PlayUISound(args->args[0].GetString());
        }
        break;
    case 4:
        {
            static BSFixedString menuName("FavoritesMenuEx");
            CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Close);
        }
    case 5:
        {
            CloseMenuDelayed();
        }
    default:
        break;
    }
}

void FavoritesMenuEx::RegisterFunctions()
{
    this->RegisterNativeFunction("Close", 0);
    this->RegisterNativeFunction("useQuickkey", 1);
    this->RegisterNativeFunction("requestData", 2);
    this->RegisterNativeFunction("PlaySound", 3);
    this->RegisterNativeFunction("Exit", 4);
    this->RegisterNativeFunction("CloseDelayed", 5);
}

UInt32 FavoritesMenuEx::ProcessMessage(UIMessage* msg)
{
    switch (msg->type)
    {
    case kMessage_Open:
        {
            FavoritesMenuExInput::GetInstance().RegisterForInput(true);
            IMenu* menu = (*g_ui)->GetMenu(BSFixedString("HUDMenu"));
            menu->movie->movieRoot->SetVariable("root.BottomCenterGroup_mc.visible", &GFxValue(false));
            menu->movie->movieRoot->SetVariable("root.TopCenterGroup_mc.visible", &GFxValue(false));
            menu->movie->movieRoot->SetVariable("root.CenterGroup_mc.visible", &GFxValue(false));
            break;
        }
    case kMessage_Close:
        {
            FavoritesMenuExInput::GetInstance().RegisterForInput(false);
            IMenu* menu = (*g_ui)->GetMenu(BSFixedString("HUDMenu"));
            menu->movie->movieRoot->SetVariable("root.BottomCenterGroup_mc.visible", &GFxValue(true));
            menu->movie->movieRoot->SetVariable("root.TopCenterGroup_mc.visible", &GFxValue(true));
            menu->movie->movieRoot->SetVariable("root.CenterGroup_mc.visible", &GFxValue(true));
            break;
        }
    case kMessage_CloseDelayed:
        {
            IMenu* menu = (*g_ui)->GetMenu(BSFixedString("FavoritesMenuEx"));
            menu->movie->movieRoot->Invoke("root.Menu_mc.closeMenuDelayed", nullptr, nullptr, 0);
            break;
        }
    default:
        break;
    }
    return this->GameMenuBase::ProcessMessage(msg);
}

IMenu* FavoritesMenuEx::CreateFavoritesMenuEx()
{
    return new FavoritesMenuEx();
}

void FavoritesMenuEx::OpenMenu()
{
    _MESSAGE("Open menu slowtime factor: %f", *(float*)((uintptr_t)(*g_VATS)+0x84));

    if (*(float*)((uintptr_t)(*g_VATS)+0x84) == 0.0f)
    {
        bTimeSlowed = true;
        if (iSlowTime)
        {
            SpellItem* spell = DYNAMIC_CAST(LookupFormByID(0x1D4AB4), TESForm, SpellItem);
            Spell_Cast((*g_gameVM)->m_virtualMachine,0,spell,*g_player,nullptr);
        }
        if (iImad)
        {
            TESImageSpaceModifier* imad = DYNAMIC_CAST(LookupFormByID(0x2170E9), TESForm, TESImageSpaceModifier);
            ImageSpaceModifier_Apply((*g_gameVM)->m_virtualMachine,0,imad,1.0);
        }
    }
    
    static BSFixedString menuName("FavoritesMenuEx");
    CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Open);
}

void FavoritesMenuEx::CloseMenu()
{
    _MESSAGE("Close menu");
    if (bTimeSlowed)
    {
        bTimeSlowed = false;
        if (iSlowTime)
        {
            SpellItem* spell = DYNAMIC_CAST(LookupFormByID(0x1D4AB4), TESForm, SpellItem);
            Actor_DispellSpell((*g_gameVM)->m_virtualMachine,0,*g_player,spell);
        }
        if (iImad)
        {
            TESImageSpaceModifier* imad = DYNAMIC_CAST(LookupFormByID(0x2170E9), TESForm, TESImageSpaceModifier);
            ImageSpaceModifier_Remove((*g_gameVM)->m_virtualMachine,0,imad);
        }
    }
    
    static BSFixedString menuName("FavoritesMenuEx");
    CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Close);
}

void FavoritesMenuEx::CloseMenuDelayed()
{
    _MESSAGE("Close menu");
    if (bTimeSlowed)
    {
        bTimeSlowed = false;
        if (iSlowTime)
        {
            SpellItem* spell = DYNAMIC_CAST(LookupFormByID(0x1D4AB4), TESForm, SpellItem);
            Actor_DispellSpell((*g_gameVM)->m_virtualMachine,0,*g_player,spell);
        }
        if (iImad)
        {
            TESImageSpaceModifier* imad = DYNAMIC_CAST(LookupFormByID(0x2170E9), TESForm, TESImageSpaceModifier);
            ImageSpaceModifier_Remove((*g_gameVM)->m_virtualMachine,0,imad);
        }
    }
    
    static BSFixedString menuName("FavoritesMenuEx");
    CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_CloseDelayed);
}

void FavoritesMenuEx::RegisterMenu()
{
    static BSFixedString menuName("FavoritesMenuEx");
    if ((*g_ui) != nullptr && !(*g_ui)->IsMenuRegistered(menuName))
    {
        (*g_ui)->Register("FavoritesMenuEx", CreateFavoritesMenuEx);
    }
    _DMESSAGE("RegisterMenu %s", (*g_ui)->IsMenuRegistered(menuName) ? "registered" : "not registered");
}

void FavoritesMenuEx::SelectionChange(int val)
{
    IMenu* menu = (*g_ui)->GetMenu(BSFixedString("FavoritesMenuEx"));
    GFxValue sector;
    sector.SetInt(val);
    menu->movie->movieRoot->Invoke("root.Menu_mc.SetSelection", nullptr, &sector, 1);
}

void FavoritesMenuEx::DownPressed()
{
    IMenu* menu = (*g_ui)->GetMenu(BSFixedString("FavoritesMenuEx"));
    menu->movie->movieRoot->Invoke("root.Menu_mc.DownPressed", nullptr, nullptr, 0);
}

void FavoritesMenuEx::UpPressed()
{
    IMenu* menu = (*g_ui)->GetMenu(BSFixedString("FavoritesMenuEx"));
    menu->movie->movieRoot->Invoke("root.Menu_mc.UpPressed", nullptr, nullptr, 0);
}

void FavoritesMenuEx::LeftPressed()
{
    IMenu* menu = (*g_ui)->GetMenu(BSFixedString("FavoritesMenuEx"));
    menu->movie->movieRoot->Invoke("root.Menu_mc.LeftPressed", nullptr, nullptr, 0);
}

void FavoritesMenuEx::RightPressed()
{
    IMenu* menu = (*g_ui)->GetMenu(BSFixedString("FavoritesMenuEx"));
    menu->movie->movieRoot->Invoke("root.Menu_mc.RightPressed", nullptr, nullptr, 0);
}

int FavoritesMenuEx::GetSelection()
{
    IMenu* menu = (*g_ui)->GetMenu(BSFixedString("FavoritesMenuEx"));
    GFxValue result;
    menu->movie->movieRoot->Invoke("root.Menu_mc.GetSelection", &result, nullptr, 0);
    return result.GetInt();
}

int FavoritesMenuEx::GetSelectedSector()
{
    IMenu* menu = (*g_ui)->GetMenu(BSFixedString("FavoritesMenuEx"));    
    GFxValue result;
    menu->movie->movieRoot->Invoke("root.Menu_mc.GetSelectedSector", &result, nullptr, 0);
    return result.GetInt();
}