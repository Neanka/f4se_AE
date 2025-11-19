#include "main.h"

#include <regex>
#include <shlobj_core.h>
#include "GameData.h"
#include "HookUtil.h"
#include "PluginAPI.h"
#include "f4se_common/f4se_version.h"
#include "f4se_common/SafeWrite.h"
std::string mName = "FOLON_UILoadingMenu_DisableInput";
UInt32 mVer = 1;

PluginHandle g_pluginHandle = kPluginHandle_Invalid;

RelocAddr<uintptr_t> hook_target_rot (0x0FE0C88);
RelocAddr<uintptr_t> hook_target_mouse_rot (0x0FE0E07);
RelocAddr<uintptr_t> hook_target_mouse_zoom (0x0FE0E3D);
RelocAddr<uintptr_t> hook_target_kb1 (0x0FE0EBC);
RelocAddr<uintptr_t> hook_target_kb2 (0x0FE0F24);
RelocAddr<uintptr_t> hook_target_kb3 (0x0FE0F88);
RelocAddr<uintptr_t> hook_target_kb4 (0x0FE0FF5);

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
		HookUtil:HookUtil::SafeWrite64(shouldhanleevent_native.GetUIntPtr(), (uintptr_t)shouldhanleevent_hook);
	}

};
*/
extern "C" {
	__declspec(dllexport) F4SEPluginVersionData F4SEPlugin_Version =
	{
		F4SEPluginVersionData::kVersion,
	
		1,
		"FOLON_UILoadingMenu_DisableInput",
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
		unsigned char data[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
		SafeWriteBuf(hook_target_rot.GetUIntPtr(), &data, sizeof(data));
		SafeWriteBuf(hook_target_mouse_rot.GetUIntPtr(), &data, sizeof(data));
		SafeWriteBuf(hook_target_mouse_zoom.GetUIntPtr(), &data, sizeof(data));
		SafeWriteBuf(hook_target_kb1.GetUIntPtr(), &data, sizeof(data));
		SafeWriteBuf(hook_target_kb2.GetUIntPtr(), &data, sizeof(data));
		SafeWriteBuf(hook_target_kb3.GetUIntPtr(), &data, sizeof(data));
		SafeWriteBuf(hook_target_kb4.GetUIntPtr(), &data, sizeof(data));
		return true;
	}
};