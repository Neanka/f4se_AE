#include "main.h"

#include <filesystem>
#include <fstream>

#include <rapidxml/rapidxml.hpp>

#include "FavoritesMenuEx.h"
#include "FMEInput.h"
#include "xbyak/xbyak.h"

#include "Shared.h"

std::map<std::string, std::string> icons;
std::map<std::string, std::string> autoTags;
UInt8 iMode = 0;
UInt8 iUseRightStick = 0;

UInt8 iDpadUpAction = 1;
UInt8 iDpadDownAction = 1;
UInt8 iDpadLeftAction = 1;
UInt8 iDpadRightAction = 1;

UInt8 iSlowTime = 0;
UInt8 iImad = 0;

UInt8 iUseCustomColors = 0;
UInt8 iColorR = 0xC7;
UInt8 iColorG = 0xA5;
UInt8 iColorB = 0x30;

UInt8 iMenuStyle = 0;

float fx = 0;
float fy = 0;
float fs = 1;

bool bTimeSlowed = false;

bool bDEF_UIexist = false;
bool bFallUIexist = false;
bool bSettingsChanged = true;

std::string mName = "FavoritesMenuEx";
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

bool CheckFileAccess( PCTSTR filename )
{
    bool state = true;
    DWORD attrib = GetFileAttributes( filename );
    if( attrib == INVALID_FILE_ATTRIBUTES )
    {
        state = false;      // not accessible
    }
    else if( attrib & FILE_ATTRIBUTE_DIRECTORY )
    {
        state = false;      // it is a directory
    }
    return state;
}

void UseQuickKey(SInt32 index)
{
    if (index > -1) FavoritesManager_useQuickkey(*g_favoritesManager, index);
    else _DMESSAGE("UseQuickKey failed. quickey index = -2");
}

void checkLibs()
{
    if (CheckFileAccess("./Data/Interface/DEF_CONF/DEF_INV_TAGS.xml")
        && CheckFileAccess("./Data/Interface/iconlibs2.swf"))
    {
        _MESSAGE("DEF_INV files exists");
        bDEF_UIexist = true;

    }
    Setting	* setting = GetINISetting("sLanguage:General");
    std::string filename = "./Data/Interface/ItemSorter/FIS/AutoTags/IndexedTags_";
    filename.append(setting->data.s).append(".txt");
    if (CheckFileAccess("./Data/Interface/ItemSorter/FIS (FallUI Item Sorter).xml")
        && CheckFileAccess("./Data/Interface/FallUI_IconLib.swf")
            && CheckFileAccess(filename.c_str()))
    {
        _MESSAGE("FIS files exists");
        bFallUIexist = true;
    }
}

void ReadSettings()
{
    _MESSAGE("read settings");
    checkLibs();
    ReadSettingInt(Main, iHoldKey)
    ReadSettingInt(Main, iMode)
    ReadSettingInt(Gamepad, iUseRightStick)
    ReadSettingInt(Gamepad, iDpadUpAction)
    ReadSettingInt(Gamepad, iDpadDownAction)
    ReadSettingInt(Gamepad, iDpadLeftAction)
    ReadSettingInt(Gamepad, iDpadRightAction)
    ReadSettingInt(Effects, iSlowTime)
    ReadSettingInt(Effects, iImad)
    ReadSettingInt(Colors, iUseCustomColors)
    ReadSettingInt(Colors, iColorR)
    ReadSettingInt(Colors, iColorG)
    ReadSettingInt(Colors, iColorB)
    ReadSettingInt(Style, iMenuStyle)
    std::unique_ptr<char[]> sResult(new char[MAX_PATH]);
    ReadSettingFloat(Style, fx)
    ReadSettingFloat(Style, fy)
    ReadSettingFloat(Style, fs)
    iUseRightStick += 0xB;
    _MESSAGE("Stick: %i" , iUseRightStick);
    icons.clear();
    autoTags.clear();
    switch (iMode)
    {
    case 0:
        {
            if (bFallUIexist)
            {
                iMode = 3;
                readXML("./Data/Interface/ItemSorter/FIS (FallUI Item Sorter).xml", 2);
            }
            else if (bDEF_UIexist)
            {
                iMode = 2;
                readXML("./Data/Interface/DEF_CONF/DEF_INV_TAGS.xml", 1);
            }
            else iMode = 1;
        }
        break;
    case 1:
        {
            
        }
        break;
    case 2:
        {
            if (bDEF_UIexist) readXML("./Data/Interface/DEF_CONF/DEF_INV_TAGS.xml", 1);
            else iMode = 1;
        }
        break;
    case 3:
        {
            if (bFallUIexist) readXML("./Data/Interface/ItemSorter/FIS (FallUI Item Sorter).xml", 2);
            else iMode = 1;
        }
        break;
    default:
        break;
    }
    _MESSAGE("iMode final: %i", iMode);
}

void readXML(std::string file, int type)
{
    using namespace rapidxml;
    xml_document<> doc;
    using namespace std;
    ifstream ifs = ifstream(file, ios_base::in);
    if (ifs.is_open() == true)
    {
        char c; int i = 0; char arr[100000];
        while (ifs.get(c)) {
            arr[i] = c;
            i++;
        }
        ifs.close();
        doc.parse<0>(arr);

        xml_node<> *node;
        if (type == 1) node = doc.first_node("tags");
        else node = doc.first_node("TagConfiguration")->first_node("tags");

        for (xml_node<> *nd = node->first_node(); nd; nd = nd->next_sibling("tag"))
            icons[nd->first_attribute("keyword")->value()] = nd->first_attribute("icon")->value();
        //for (const auto& kv : icons) _MESSAGE("%s - %s", kv.first.c_str(), kv.second.c_str());
    }
    if (type == 2)
    {
        Setting	* setting = GetINISetting("sLanguage:General");
        string filename = "./Data/Interface/ItemSorter/FIS/AutoTags/IndexedTags_";
        filename.append(setting->data.s).append(".txt");
        ifstream autoTagsFile = ifstream(filename, ios_base::in);
        if (autoTagsFile.is_open() == true)
        {
            do
            {
                string temp, tag, key;
                getline(autoTagsFile, temp);
                if (temp.substr(0,1) == "[")
                {                    
                    tag = temp.substr(1, temp.find("]")-1);
                    int pos = temp.find("");
                    temp = temp.substr(pos+1, temp.length()-pos-1);
                    
                    do {
                        pos = temp.find("");
                        key = temp.substr(0,pos);
                        temp = temp.substr(pos+1, temp.length()-pos-1);
                        autoTags[key] = tag;
                    } while (pos != string::npos);
                }
            }
            while (!autoTagsFile.eof());
            autoTagsFile.close();
            //for (const auto& kv : autoTags) _MESSAGE("%s - %s", kv.first.c_str(), kv.second.c_str());
        }
    }
}

void MessageCallback(F4SEMessagingInterface::Message* msg)
{
    switch (msg->type)
    {
    case F4SEMessagingInterface::kMessage_GameLoaded:
        //MenuOpenCloseHandler::Register();
        FavoritesMenuEx::RegisterMenu();
        FMEInput::GetInstance().RegisterForInput(true);
        (*g_favoritesManager)->inputEventUser.enabled = false;
       
        break;
    case F4SEMessagingInterface::kMessage_GameDataReady:
        //static auto pMenuOpenCloseHandler = new MenuOpenCloseHandler();
        //(*g_ui)->menuOpenCloseEventSource.AddEventSink(pMenuOpenCloseHandler);
        ReadSettings();
        bSettingsChanged = false;
        break;
    default:
        break;
    }
}

class Scaleform_UpdateSettings : public GFxFunctionHandler
{
public:
    void Invoke(Args* args) override
    {
        _MESSAGE("Scaleform_UpdateSettings");
        bSettingsChanged = true;
    }
};

bool RegisterScaleform(GFxMovieView* view, GFxValue* f4se_root)
{
    GFxMovieRoot* root = view->movieRoot;
    RegisterFunction<Scaleform_UpdateSettings>(f4se_root, view->movieRoot, "UpdateSettings");
    return true;
}
/*
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
    RVA_InitInventory();
    RVA_InitPipboy();
    RVA_InitFavorites();
    RVA_InitFavoritesMenuExAddresses();
    RVA_InitSpells();
    RVA_InitVATS();
    RVAManager::UpdateAddresses(f4se->runtimeVersion);
    if (g_messaging != nullptr)
        g_messaging->RegisterListener(g_pluginHandle, "F4SE", MessageCallback);
    if (g_scaleform)
        g_scaleform->Register("FME", RegisterScaleform);
    return true;
}
};
*/
extern "C" {
    __declspec(dllexport) F4SEPluginVersionData F4SEPlugin_Version =
    {
        F4SEPluginVersionData::kVersion,
	
        1,
        "FavoritesMenuEx",
        "Neanka",

        0,	// not version independent
        0,	// not version independent (extended field)
        { CURRENT_RELEASE_RUNTIME, 0 },	// compatible with 1.10.984

        0,	// works with any version of the script extender. you probably do not need to put anything here
    };

    __declspec(dllexport) bool F4SEPlugin_Load(const F4SEInterface* f4se)
    {
        char logPath[MAX_PATH];
        sprintf_s(logPath, sizeof(logPath), "\\My Games\\Fallout4\\F4SE\\%s.log", "FavoritesMenuEx");
        gLog.OpenRelative(CSIDL_MYDOCUMENTS, logPath);
        _MESSAGE("load");
        g_pluginHandle = f4se->GetPluginHandle();
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
        logMessage("load");
        RVA_InitInventory();
        RVA_InitPipboy();
        RVA_InitFavorites();
        RVA_InitFavoritesMenuExAddresses();
        RVA_InitSpells();
        RVA_InitVATS();
        RVAManager::UpdateAddresses(f4se->runtimeVersion);
        if (g_messaging != nullptr)
            g_messaging->RegisterListener(g_pluginHandle, "F4SE", MessageCallback);
        if (g_scaleform)
            g_scaleform->Register("FME", RegisterScaleform);
        return true;
    }
};