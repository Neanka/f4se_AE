#include "main.h"

std::string mName = "SubtitlesEx";
UInt32 mVer = 1;

PluginHandle			    g_pluginHandle = kPluginHandle_Invalid;

UInt8 gender = 0;
bool genderset = false;

#define HUDSubtitleText__UpdateComponent_ADDRESS 0x2D19B08// 48 8B C4 55 48 8D 68 88 48 81 EC 70 01 00 00 48 89 58 08 48 89 70 10 48 89 78 18
typedef UInt32(*_HUDSubtitleText__UpdateComponent)(HUDSubtitleText* hUDSubtitleText);
RelocAddr <_HUDSubtitleText__UpdateComponent> HUDSubtitleText__UpdateComponent(HUDSubtitleText__UpdateComponent_ADDRESS);
_HUDSubtitleText__UpdateComponent HUDSubtitleText__UpdateComponent_Original;

UInt32 HUDSubtitleText__UpdateComponent_Hook(HUDSubtitleText* hUDSubtitleText)
{
	if (hUDSubtitleText->hUDSubtitleDisplayEventReceived)
	{
		if (!genderset)
		{
			TESNPC * actorBase = DYNAMIC_CAST((*g_player)->baseForm, TESForm, TESNPC);			
			if(actorBase) {
				gender = CALL_MEMBER_FN(actorBase, GetSex)();
				_MESSAGE("player sex = %s", gender?"female":"male");
			}
			genderset = true;
		}
		std::string str = hUDSubtitleText->ev.SubtitleText.c_str();
		auto itm = str.find("<playersex::male>");
		auto itf = str.find("<playersex::female>");
		_MESSAGE("%i %i", itm, itf);
		if (itm!= std::string::npos && itf!= std::string::npos)
		{
			std::string newstr;
			if (gender == 0)
			{
				newstr = str.substr(17, itf-17);
			}
			else
			{
				newstr = str.substr(itf+19);
			}
			_MESSAGE("%s", newstr.c_str());
			hUDSubtitleText->ev.SubtitleText = BSFixedString(newstr.c_str());
		}
	}
	return HUDSubtitleText__UpdateComponent_Original(hUDSubtitleText);
}

extern "C"
{
	bool F4SEPlugin_Query(const F4SEInterface * f4se, PluginInfo * info)
	{
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, (const char*)("\\My Games\\Fallout4\\F4SE\\"+ mName +".log").c_str());
		logMessage("query");
		
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
		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface *f4se)
	{
		logMessage("load");
		HUDSubtitleText__UpdateComponent_Original = HookUtil::SafeWrite64(HUDSubtitleText__UpdateComponent.GetUIntPtr(), &HUDSubtitleText__UpdateComponent_Hook);
		return true;
	}
}