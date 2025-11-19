#include "main.h"

#include <regex>
#include <shlobj_core.h>
#include "GameMenus.h"
#include "PluginAPI.h"
#include "f4se_common/f4se_version.h"
#include "ScaleformLoader.h"

std::string mName = "FOLON_UICompasSZ";
UInt32 mVer = 1;

PluginHandle g_pluginHandle = kPluginHandle_Invalid;

F4SEMessagingInterface* g_messaging = nullptr;
F4SEPapyrusInterface* g_papyrus = NULL;

class FOLON_UICompasSZ : public GameMenuBase
{
public:
	FOLON_UICompasSZ() : GameMenuBase()
	{
		flags = kFlag_AllowSaving | kFlag_DontHideCursorWhenTopmost | kFlag_CompanionAppAllowed | kFlag_CustomRendering;// | kFlag_AlwaysOpen;
		depth = 0x6;
		if (CALL_MEMBER_FN((*g_scaleformManager), LoadMovie)(this, this->movie, "FOLON_UICompasSZ", "root1.CompasSZ", 2))
		{

			_DMESSAGE("LoadMovie FOLON_UICompasSZ done");

			CreateBaseShaderTarget(this->filterHolder, this->stage);

			this->filterHolder->SetFilterColor(false);
			(*g_colorUpdateDispatcher)->eventDispatcher.AddEventSink(this->filterHolder);

			this->shaderFXObjects.Push(this->filterHolder);
		}
	}

	static IMenu* CreateFOLON_UICompasSZ()
	{
		return new FOLON_UICompasSZ();
	}

	static void OpenMenu()
	{
		static BSFixedString menuName("FOLON_UICompasSZ");
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Open);
	}

	static void CloseMenu()
	{
		static BSFixedString menuName("FOLON_UICompasSZ");
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Close);
	}

	static void RegisterMenu()
	{
		static BSFixedString menuName("FOLON_UICompasSZ");
		if ((*g_ui) != nullptr && !(*g_ui)->IsMenuRegistered(menuName))
		{
			(*g_ui)->Register("FOLON_UICompasSZ", CreateFOLON_UICompasSZ);
		}
		_DMESSAGE("RegisterMenu %s", (*g_ui)->IsMenuRegistered(menuName) ? "registered" : "not registered");
	}
};

void MessageCallback(F4SEMessagingInterface::Message* msg)
{
	switch (msg->type)
	{
	case F4SEMessagingInterface::kMessage_GameLoaded:
		FOLON_UICompasSZ::RegisterMenu();
		break;
	default:
		break;
	}
}

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

bool pap_start(StaticFunctionTag*)
{
	FOLON_UICompasSZ::OpenMenu();
	return true;
}

bool pap_stop(StaticFunctionTag*)
{
	FOLON_UICompasSZ::CloseMenu();
	return true;
}

bool RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction0 <StaticFunctionTag, bool>("Start", "folon:FOLON_UICompasSZ", pap_start, vm));
	vm->RegisterFunction(
		new NativeFunction0 <StaticFunctionTag, bool>("Stop", "folon:FOLON_UICompasSZ", pap_stop, vm));
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
		"FOLON_UICompasSZ",
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
			_MESSAGE("got papyrus interface");
		}

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