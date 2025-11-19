#include "main.h"

#include <shlobj_core.h>

#include "PluginAPI.h"
#include "f4se_common/f4se_version.h"

std::string mName = "NG_test";
UInt32 mVer = 1;

PluginHandle			    g_pluginHandle = kPluginHandle_Invalid;
extern "C" {
	__declspec(dllexport) F4SEPluginVersionData F4SEPlugin_Version =
	{
		F4SEPluginVersionData::kVersion,
	
		1,
		"NG_test",
		"Neanka",

		0,	// not version independent
		0,	// not version independent (extended field)
		{ RUNTIME_VERSION_1_10_984, 0 },	// compatible with 1.10.984

		0,	// works with any version of the script extender. you probably do not need to put anything here
	};

	__declspec(dllexport) bool F4SEPlugin_Load(const F4SEInterface* f4se)
	{
		char logPath[MAX_PATH];
		sprintf_s(logPath, sizeof(logPath), "\\My Games\\Fallout4\\F4SE\\%s.log", "NG_test");
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, logPath);
		_MESSAGE("load");
		g_pluginHandle = f4se->GetPluginHandle();
		return true;
	}
};
