#include "main.h"

#include <regex>
#include <shlobj_core.h>
#include "GameMenus.h"
#include "PluginAPI.h"
#include "f4se_common/f4se_version.h"
#include "ScaleformLoader.h"

std::string mName = "FOLON_UITimer";
UInt32 mVer = 1;

UInt32 QuestTimer;

PluginHandle g_pluginHandle = kPluginHandle_Invalid;

F4SEMessagingInterface* g_messaging = nullptr;
F4SEPapyrusInterface* g_papyrus = NULL;

class FOLON_UITimer : public GameMenuBase
{
public:
	FOLON_UITimer() : GameMenuBase()
	{
		flags = kFlag_AllowSaving | kFlag_DontHideCursorWhenTopmost | kFlag_CompanionAppAllowed | kFlag_CustomRendering;// | kFlag_AlwaysOpen;
		depth = 0x6;
		if (CALL_MEMBER_FN((*g_scaleformManager), LoadMovie)(this, this->movie, "FOLON_UITimer", "root1.UITimer", 2))
		{

			_DMESSAGE("LoadMovie FOLON_UITimer done");

			CreateBaseShaderTarget(this->filterHolder, this->stage);

			this->filterHolder->SetFilterColor(false);
			(*g_colorUpdateDispatcher)->eventDispatcher.AddEventSink(this->filterHolder);

			this->shaderFXObjects.Push(this->filterHolder);
		}
	}

	static IMenu* CreateFOLON_UITimer()
	{
		return new FOLON_UITimer();
	}

	static void OpenMenu()
	{
		static BSFixedString menuName("FOLON_UITimer");
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Open);
	}

	static void CloseMenu()
	{
		static BSFixedString menuName("FOLON_UITimer");
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Close);
	}

	static void RegisterMenu()
	{
		static BSFixedString menuName("FOLON_UITimer");
		if ((*g_ui) != nullptr && !(*g_ui)->IsMenuRegistered(menuName))
		{
			(*g_ui)->Register("FOLON_UITimer", CreateFOLON_UITimer);
		}
		_DMESSAGE("RegisterMenu %s", (*g_ui)->IsMenuRegistered(menuName) ? "registered" : "not registered");
	}
};

void MessageCallback(F4SEMessagingInterface::Message* msg)
{
	switch (msg->type)
	{
	case F4SEMessagingInterface::kMessage_GameLoaded:
		FOLON_UITimer::RegisterMenu();
		break;
	default:
		break;
	}
}

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

bool pap_start(StaticFunctionTag*)
{
	FOLON_UITimer::OpenMenu();
	return true;
}

bool pap_stop(StaticFunctionTag*)
{
	FOLON_UITimer::CloseMenu();
	return true;
}

bool Update(UInt32 time)
{
	IMenu* menu = (*g_ui)->GetMenu(BSFixedString("FOLON_UITimer"));
	if (!menu) return false;
	GFxMovieRoot* movieRoot = menu->movie->movieRoot;
	GFxValue StringValue;
	int mins, secs;
	mins = time / 60;
	secs = time % 60;
	char str1[256];
	sprintf_s(str1, sizeof(str1), "%02i:%02i", mins, secs);
	movieRoot->CreateString(&StringValue, str1);
	movieRoot->Invoke("root.UITimer.Timer_Text.set_text", nullptr, &StringValue, 1);
	return true;
}

bool pap_update(StaticFunctionTag*, UInt32 time)
{
	Update(time);
	return true;
}

bool RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction0 <StaticFunctionTag, bool>("Start", "FOLON:FOLON_UITimer", pap_start, vm));
	vm->RegisterFunction(
		new NativeFunction0 <StaticFunctionTag, bool>("Stop", "FOLON:FOLON_UITimer", pap_stop, vm));
	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, bool, UInt32>("UpdateValues", "FOLON:FOLON_UITimer", pap_update, vm));
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
			_MESSAGE("couldn't get papyrus interface");
			return false;
		}
		else {
			_MESSAGE("got papyrus interface");
		}

		g_messaging = (F4SEMessagingInterface*)f4se->QueryInterface(kInterface_Messaging);
		if (!g_messaging)
		{
			_FATALERROR("couldn't get messaging interface");
			return false;
		}
		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface *f4se)
	{
		_MESSAGE("load");
		if (g_messaging != nullptr)
			g_messaging->RegisterListener(g_pluginHandle, "F4SE", MessageCallback);
		if (g_papyrus)
		{
			g_papyrus->Register(RegisterFuncs);
			_MESSAGE("Papyrus Register Succeeded");
		}
		return true;
	}
};
*/
	extern "C" {
		__declspec(dllexport) F4SEPluginVersionData F4SEPlugin_Version =
		{
			F4SEPluginVersionData::kVersion,
	
			1,
			"FOLON_UITimer",
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
			else {
				_MESSAGE("got papyrus interface");			}

			g_messaging = (F4SEMessagingInterface*)f4se->QueryInterface(kInterface_Messaging);
			if (!g_messaging)
			{
				_FATALERROR("couldn't get messaging interface");
				return false;
			}
			_MESSAGE("load");
			if (g_messaging != nullptr)
				g_messaging->RegisterListener(g_pluginHandle, "F4SE", MessageCallback);
			if (g_papyrus)
			{
				g_papyrus->Register(RegisterFuncs);
				_MESSAGE("Papyrus Register Succeeded");
			}
			return true;
		}
	};