#include "Shared.h"
#include "main.h"

#include "f4se\GameMenus.h"

std::string mName = "cc_cleaner";
UInt32 mVer = 1;

F4SEScaleformInterface		*g_scaleform = NULL;

PluginHandle			    g_pluginHandle = kPluginHandle_Invalid;


std::string CCGetConfigOption(const char * section, const char * key)
{
	std::string	result;

	const std::string & configPath = "./Data/MCM/Settings/cc_cleaner.ini";
	if(!configPath.empty())
	{
		char	resultBuf[256];
		resultBuf[0] = 0;

		UInt32	resultLen = GetPrivateProfileString(section, key, NULL, resultBuf, sizeof(resultBuf), configPath.c_str());

		result = resultBuf;
	}

	return result;
}

class Scaleform_inject : public GFxFunctionHandler
{
public:
	Scaleform_inject() { }
	
	virtual void	Invoke(Args * args){

		GFxMovieRoot	*movieRoot = args->movie->movieRoot;
		//_MESSAGE("enter frame handler");
		GFxValue listarr;
		movieRoot->GetVariable(&listarr, "root.Menu_mc.MainPanel_mc.List_mc.entryList");
		for (size_t i = 0; i < listarr.GetArraySize(); i++)
		{
			GFxValue item;
			listarr.GetElement(i, &item);
			GFxValue value;
			item.GetMember("text", &value);
			if (!to_bool(CCGetConfigOption("Main","bCCEntryEnabled")))
			{
				if (_stricmp(value.GetString(), "$ADD-ONS") == 0 || _stricmp(value.GetString(), "$CREATION CLUB") == 0 || _stricmp(value.GetString(), "$CREATIONSNEW") == 0 || _stricmp(value.GetString(), "$INSTALLED CONTENT") == 0)
				{
					item.SetMember("filterFlag", &GFxValue(0));
				}
			}
		}

		GFxValue root;
		movieRoot->GetVariable(&root, "root");
		GFxValue menu;
		movieRoot->GetVariable(&menu, "root.Menu_mc");
		
		GFxValue listener[2];
		movieRoot->CreateString(&listener[0], "enterFrame");
		root.GetMember("frameaddedlistener", &listener[1]);
		menu.Invoke("removeEventListener", nullptr, listener, 2);
	}
};

bool RegisterScaleform(GFxMovieView * view, GFxValue * f4se_root)
{
	GFxMovieRoot	*movieRoot = view->movieRoot;
	GFxValue currentSWFPath;
	std::string currentSWFPathString = "";
	if (movieRoot->GetVariable(&currentSWFPath, "root.loaderInfo.url")) {
		currentSWFPathString = currentSWFPath.GetString();
	} else {
		_MESSAGE("WARNING: Scaleform registration failed.");
	}
	if (currentSWFPathString.find("MainMenu.swf") != std::string::npos) {
		
		//_MESSAGE("Hooking MainMenu menu.");
		GFxValue root;
		movieRoot->GetVariable(&root, "root");
		GFxValue menu;
		movieRoot->GetVariable(&menu, "root.Menu_mc");
		auto fnc = new Scaleform_inject();

		GFxValue listener[2];
		movieRoot->CreateString(&listener[0], "enterFrame");
		movieRoot->CreateFunction(&listener[1], fnc);
		root.SetMember("frameaddedlistener", &listener[1]);
		InterlockedDecrement(&fnc->refCount);
		menu.Invoke("addEventListener", nullptr, listener, 2);

		if (!to_bool(CCGetConfigOption("Main","bMOTDEnabled")))
		{
			GFxValue MOTD;
			movieRoot->GetVariable(&MOTD, "root.Menu_mc.MessageOfTheDay_mc");
			MOTD.SetMember("y", &GFxValue(10000.0f));
		}

		GFxValue mpanel;
		movieRoot->GetVariable(&mpanel, "root.Menu_mc.MainPanel_mc");
		mpanel.SetMember("y", &GFxValue(432.0f)); // 412 orig
		
		GFxValue bg1, bra1;
		movieRoot->Invoke("root.Menu_mc.MainPanel_mc.getChildAt",&bg1,&GFxValue((UInt32)0),1); // bg
		movieRoot->Invoke("root.Menu_mc.MainPanel_mc.getChildAt",&bra1,&GFxValue((UInt32)1),1); // brackets
		bg1.SetMember("height", &GFxValue(308.0f)); // 327 orig
		bra1.SetMember("height", &GFxValue(308.0f));		
	} 
	return true;
}

extern "C" {
	__declspec(dllexport) F4SEPluginVersionData F4SEPlugin_Version =
	{
		F4SEPluginVersionData::kVersion,
	
		1,
		"cc_cleaner",
		"Neanka",

		0,	// not version independent
		0,	// not version independent (extended field)
		{ CURRENT_RELEASE_RUNTIME, 0 },

		0,	// works with any version of the script extender. you probably do not need to put anything here
	};

	__declspec(dllexport) bool F4SEPlugin_Load(const F4SEInterface* f4se)
	{
		char logPath[MAX_PATH];
		sprintf_s(logPath, sizeof(logPath), "\\My Games\\Fallout4\\F4SE\\%s.log", "CCCleaner");
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, logPath);
		_MESSAGE("load");
		g_pluginHandle = f4se->GetPluginHandle();
		g_scaleform = (F4SEScaleformInterface *)f4se->QueryInterface(kInterface_Scaleform);
		if(!g_scaleform)
		{
			errorMessage("couldn't get scaleform interface");
			return false;
		}
		g_scaleform->Register("cc_cleaner", RegisterScaleform);
		logMessage("Scaleform Register Succeeded");
		return true;
	}
};