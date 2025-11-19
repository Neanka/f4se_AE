#include "main.h"

#include <regex>
#include <shlobj_core.h>

#include "GameData.h"
#include "GameMenus.h"
#include "GameRTTI.h"
#include "PapyrusNativeFunctions.h"
#include "PluginAPI.h"
#include "f4se_common/f4se_version.h"

std::string mName = "FOLON_UIMapSearchArea";
UInt32 mVer = 1;

PluginHandle g_pluginHandle = kPluginHandle_Invalid;

F4SEPapyrusInterface* g_papyrus = nullptr;
F4SEScaleformInterface* g_scaleform = nullptr;

class Area
{
public:
	Area() : _x(0), _y(0), _radius(0) {_quest = nullptr;}
	Area(float x, float y, float radius, TESForm* quest) : _x(x), _y(y), _radius(radius) {_quest = DYNAMIC_CAST(quest, TESForm, TESQuest);}
	
	float _x;
	float _y;
	float _radius;
	TESQuest* _quest;
};

std::map<UInt32, Area> Areas;

bool pap_add(StaticFunctionTag*, UInt32 id, float x, float y, float radius, TESForm* quest)
{
	auto it = Areas.find(id);
	if (it == Areas.end())
	{
		Areas[id] = Area(x, y, radius, quest);
	}
	return true;
}

bool pap_remove(StaticFunctionTag*, UInt32 id)
{
	auto it = Areas.find(id);
	if (it != Areas.end())
	{
		Areas.erase(it);
	}
	return true;
}

bool RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction5 <StaticFunctionTag, bool, UInt32, float, float, float, TESForm*>("AddArea", "FOLON:FOLON_UIMapSearchArea", pap_add, vm));
	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, bool, UInt32>("RemoveArea", "FOLON:FOLON_UIMapSearchArea", pap_remove, vm));
	return true;
}

class Scaleform_GetAreas : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args) {
		_MESSAGE("Scaleform_GetAreas");
		args->movie->movieRoot->CreateArray(args->result);
		for (const auto& kv : Areas) {
			if (kv.second._quest != nullptr && (kv.second._quest->flags >> 11 & 1) != 1)
			{
				_MESSAGE("%s quest inactive", kv.second._quest->fullName.name.c_str());
				continue;
			}
			GFxValue entry, x, y, radius;
			args->movie->movieRoot->CreateObject(&entry);
			x.SetNumber(kv.second._x);
			y.SetNumber(kv.second._y);
			radius.SetNumber(kv.second._radius);
			entry.SetMember("x", &x);
			entry.SetMember("y", &y);
			entry.SetMember("radius", &radius);
			args->result->PushBack(&entry);
		}
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
		if (currentSWFPathString.find("PipboyMenu.swf") != std::string::npos)
		{
			_DMESSAGE("hooking pipboymenu");
			RegisterFunction<Scaleform_GetAreas>(f4se_root, view->movieRoot, "GetAreas");
			GFxValue loader, urlRequest, root;
			movieRoot->GetVariable(&root, "root");
			movieRoot->CreateObject(&loader, "flash.display.Loader");
			movieRoot->CreateObject(&urlRequest, "flash.net.URLRequest", &GFxValue("FOLON_UIMapSearchArea.swf"), 1);
			root.SetMember("FOLON_UIMapSearchArea_loader", &loader);
			movieRoot->Invoke("root.FOLON_UIMapSearchArea_loader.load", nullptr, &urlRequest, 1);
			movieRoot->Invoke("root.addChild", nullptr, &loader, 1);
		}		
	}
	return true;
}
/*
extern "C"
{

	bool F4SEPlugin_Query(const F4SEInterface * f4se, PluginInfo * info)
	{
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, (const char*)("\\My Games\\Fallout4\\F4SE\\"+ mName +".log").c_str());
		_MESSAGE("query");
		// populate info structure
		info->infoVersion =	PluginInfo::kInfoVersion;
		info->name =		mName.c_str();
		info->version =		mVer;

		// store plugin handle so we can identify ourselves later
		g_pluginHandle = f4se->GetPluginHandle();

		// Check game version
		if (f4se->runtimeVersion != CURRENT_RUNTIME_VERSION) {
			char str[512];
			sprintf_s(str, sizeof(str), "Your game version: v%d.%d.%d.%d\nExpected version: v%d.%d.%d.%d\n%s will be disabled.",
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

			MessageBox(NULL, str, mName.c_str(), MB_OK | MB_ICONEXCLAMATION);
			return false;
		}
		g_papyrus = (F4SEPapyrusInterface*)f4se->QueryInterface(kInterface_Papyrus);
		if (!g_papyrus)
		{
			_FATALERROR("couldn't get papyrus interface");
			return false;
		}
		g_scaleform = (F4SEScaleformInterface*)f4se->QueryInterface(kInterface_Scaleform);
		if (!g_scaleform)
		{
			_FATALERROR("couldn't get scaleform interface");
			return false;
		}
		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface *f4se)
	{
		_MESSAGE("load");
		if (g_scaleform)
			g_scaleform->Register(mName.c_str(), ScaleformCallback);
		if (g_papyrus)
			g_papyrus->Register(RegisterFuncs);
		return true;
	}
};
*/
extern "C" {
	__declspec(dllexport) F4SEPluginVersionData F4SEPlugin_Version =
	{
		F4SEPluginVersionData::kVersion,
	
		1,
		"FOLON_UIMapSearchArea",
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
			_FATALERROR("couldn't get papyrus interface");
			return false;
		}
		g_scaleform = (F4SEScaleformInterface*)f4se->QueryInterface(kInterface_Scaleform);
		if (!g_scaleform)
		{
			_FATALERROR("couldn't get scaleform interface");
			return false;
		}
		_MESSAGE("load");
		if (g_scaleform)
			g_scaleform->Register(mName.c_str(), ScaleformCallback);
		if (g_papyrus)
			g_papyrus->Register(RegisterFuncs);
		return true;
	}
};