#include "main.h"

#include "ScaleformLoader.h"
#include "TBSTranslator.h"
#include "f4se/PapyrusNativeFunctions.h"

std::string mName = "TBS";
UInt32 mVer = 1;

PluginHandle			    g_pluginHandle = kPluginHandle_Invalid;

F4SEMessagingInterface* g_messaging = nullptr;
F4SEPapyrusInterface* g_papyrus = NULL;
F4SEScaleformInterface* g_scaleform = nullptr;

bool loader_loaded = false;

void MessageCallback(F4SEMessagingInterface::Message* msg)
{
    switch (msg->type)
    {
    case F4SEMessagingInterface::kMessage_GameLoaded:
        _MESSAGE("kMessage_GameLoaded");
        // Inject translations
        BSScaleformTranslator * translator = (BSScaleformTranslator*)(*g_scaleformManager)->stateBag->GetStateAddRef(GFxState::kInterface_Translator);
        if (translator) {
            TBSTranslator::LoadTranslations(translator);
        }
        break;
    }
}

bool pap_update(StaticFunctionTag*, UInt32 val)
{
    if (loader_loaded)
    {
        auto const menu = (*g_ui)->GetMenu(BSFixedString("HUDMenu"));
        if (menu)
        {
            GFxValue root, loader;
            menu->movie->movieRoot->GetVariable(&root, "root");
            root.GetMember("TBS_loader", &loader);

            GFxValue content;
            loader.GetMember("content", &content);

            GFxValue menu_mc, bar;
            content.GetMember("Menu_mc", &menu_mc);
            menu_mc.GetMember("bar", &bar);

            GFxValue arrargs[1];
            arrargs[0].SetInt(val);
            bar.Invoke("set_value", nullptr, arrargs, 1 );
        }
    }
    return true;
}

bool RegisterFuncs(VirtualMachine* vm)
{
    vm->RegisterFunction(
        new NativeFunction1 <StaticFunctionTag, bool, UInt32>("UpdateValue", "TBS:TBS", pap_update, vm));
    return true;
}

class F4SEScaleform_OnLoadCompleted : public GFxFunctionHandler
{
public:
    F4SEScaleform_OnLoadCompleted() { }

    virtual void Invoke(Args * args)
    {
        _MESSAGE("loader load completed");
        loader_loaded = true;
    }
};

bool ScaleformCallback(GFxMovieView* view, GFxValue* f4se_root)
{
    GFxMovieRoot* movieRoot = view->movieRoot;
    GFxValue currentSWFPath;
    std::string currentSWFPathString = "";
    if (movieRoot->GetVariable(&currentSWFPath, "root.loaderInfo.url"))
    {
        currentSWFPathString = currentSWFPath.GetString();
        if (currentSWFPathString.find("HUDMenu.swf") != std::string::npos)
        {
            _DMESSAGE("hooking HUDMenu");
            loader_loaded = false;

            GFxValue loader, urlRequest, root;
            movieRoot->GetVariable(&root, "root");
            movieRoot->CreateObject(&loader, "flash.display.Loader");
            movieRoot->CreateObject(&urlRequest, "flash.net.URLRequest", &GFxValue("TBS.swf"), 1);
            root.SetMember("TBS_loader", &loader);
            
            GFxValue contentLoaderInfo;
            loader.GetMember("contentLoaderInfo", &contentLoaderInfo);
            GFxValue listener[2];
            F4SEScaleform_OnLoadCompleted * onCompleted = new F4SEScaleform_OnLoadCompleted();    
            movieRoot->CreateString(&listener[0], "complete");
            movieRoot->CreateFunction(&listener[1], onCompleted);
            contentLoaderInfo.Invoke("addEventListener", nullptr, listener, 2);
            InterlockedDecrement(&onCompleted->refCount);
            
            movieRoot->Invoke("root.TBS_loader.load", nullptr, &urlRequest, 1);
            movieRoot->Invoke("root.LeftMeters_mc.HPMeter_mc.Bracket_mc.addChild", nullptr, &loader, 1);
        }		
    }
    return true;
}

extern "C" {
    __declspec(dllexport) F4SEPluginVersionData F4SEPlugin_Version =
    {
        F4SEPluginVersionData::kVersion,
	
        1,
        "TBS",
        "Neanka",

        0,	// not version independent
        0,	// not version independent (extended field)
        { RUNTIME_VERSION_1_10_984, 0 },	// compatible with 1.10.984

        0,	// works with any version of the script extender. you probably do not need to put anything here
    };

__declspec(dllexport) bool F4SEPlugin_Load(const F4SEInterface* f4se)
{
	gLog.OpenRelative(CSIDL_MYDOCUMENTS, (const char*)("\\My Games\\Fallout4\\F4SE\\" + mName + ".log").c_str());
	_MESSAGE("query");
	g_pluginHandle = f4se->GetPluginHandle();
    g_papyrus = (F4SEPapyrusInterface*)f4se->QueryInterface(kInterface_Papyrus);
    if (!g_papyrus)
    {
        _MESSAGE("couldn't get papyrus interface");
        return false;
    }

    g_messaging = (F4SEMessagingInterface*)f4se->QueryInterface(kInterface_Messaging);
    if (!g_messaging)
    {
        _FATALERROR("couldn't get messaging interface");
        return false;
    }
    g_scaleform = (F4SEScaleformInterface*)f4se->QueryInterface(kInterface_Scaleform);
    if (!g_scaleform)
    {
        _FATALERROR("couldn't get scaleform interface");
        return false;
    }
    _MESSAGE("load");
    if (g_messaging)
        g_messaging->RegisterListener(g_pluginHandle, "F4SE", MessageCallback);
    if (g_papyrus)
        g_papyrus->Register(RegisterFuncs);
    if (g_scaleform)
        g_scaleform->Register(mName.c_str(), ScaleformCallback);
    return true;
}
};