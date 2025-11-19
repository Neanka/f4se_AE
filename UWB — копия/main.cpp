#include "main.h"
#include "json\json.h"
#include "RVA.h"


std::string mName = "UWB";
UInt32 mVer = 1;

PluginHandle			    g_pluginHandle = kPluginHandle_Invalid;

F4SEScaleformInterface* g_scaleform = nullptr;
F4SEMessagingInterface* g_messaging = nullptr;
F4SEPapyrusInterface* g_papyrus = NULL;


#include <fstream>
#include <sstream>

bool ReadJson()
{
	std::ifstream file;
	std::string filePath = "Data\\UWB\\test_1.json";
	file.open(filePath, std::ifstream::binary);
	if (file.is_open())
	{
		_DMESSAGE("file opened");

		Json::Value root;
		file >> root;

	}
	else
	{
		_DMESSAGE("file not opened");
	}

	return true;
}

#include "ScaleformLoader.h"

class UWB_Menu : public GameMenuBase
{
public:

	enum
	{
		kMessage_msg0 = 0x10,
		kMessage_msg1
	};

	UWB_Menu() : GameMenuBase()
	{
		//default menu depth is 6.
		flags = kFlag_DoNotPreventGameSave | kFlag_DisableInteractive | kFlag_Unk800000 | kFlag_ApplyDropDownFilter | kFlag_DoNotDeleteOnClose;
		depth = 0x6;
		if (CALL_MEMBER_FN((*g_scaleformManager), LoadMovie)(this, this->movie, "UWB_Menu", "root1.Menu_mc", 2))
		{

			_DMESSAGE("LoadMovie UWB_Menu done");

			CreateBaseShaderTarget(this->shaderTarget, this->stage);

			this->shaderTarget->SetFilterColor(false);
			(*g_colorUpdateDispatcher)->eventDispatcher.AddEventSink(this->shaderTarget);

			this->subcomponents.Push(this->shaderTarget);
		}
		//IMenu* menu = (*g_ui)->GetMenu(BSFixedString("HUDMenu"));
		//menu->depth = 0x15;
	}

	virtual void	Invoke(Args* args) final
	{
		switch (args->optionID)
		{
		case 0:
		{
			_DMESSAGE("command 0");
		}
		break;
		case 1:
		{
			_DMESSAGE("command 1");
		}
		break;
		case 2:
		{
			_DMESSAGE("command 2");
		}
		break;
		default:
			break;
		}
	}

	virtual void	RegisterFunctions() final
	{
		_DMESSAGE("RegisterFunctions");
		//this->RegisterNativeFunction("command0", 0);
		//this->RegisterNativeFunction("command1", 1);
		//this->RegisterNativeFunction("command2", 2);
	}

	virtual UInt32	ProcessMessage(UIMessage* msg) final
	{
		GFxMovieRoot* root = movie->movieRoot;
		switch (msg->type)
		{
		case kMessage_msg0:
		{
			//GFxValue val, menu_mc;
			//root->Invoke("root.Menu_mc.filling_test", &val, nullptr, 0);
			//traceGFxValue(&val);
			//root->GetVariable(&menu_mc, "root.Menu_mc");
			//traceGFxValue(&menu_mc);
			break;
		}
		case kMessage_msg1:
		{
			GFxValue ValueToSet, StringValue;
			GFxValue new_widget, new_bar, new_text, menu_mc;
			root->CreateObject(&new_widget, "widget_template");
			new_widget.SetMember("name", &GFxValue("wt1"));
			root->CreateObject(&new_bar, "def_bar");
			new_bar.SetMember("name", &GFxValue("bar1"));
			root->GetVariable(&menu_mc, "root.Menu_mc");
			new_widget.SetMember("bar1", &new_bar);
			menu_mc.SetMember("wt1", &new_widget);


			// -------------- text
			root->CreateObject(&new_text, "def_text");
			new_text.SetMember("name", &GFxValue("tf1"));
			root->CreateString(&StringValue, "$MOD_CONFIG");			
			new_widget.SetMember("tf1", &new_text);


			root->Invoke("root.Menu_mc.addChild", nullptr, &new_widget, 1);
			root->Invoke("root.Menu_mc.wt1.addChild", nullptr, &new_bar, 1);
			root->Invoke("root.Menu_mc.wt1.addChild", nullptr, &new_text, 1);


			root->Invoke("root.Menu_mc.wt1.tf1.set_text", nullptr, &StringValue, 1);
			
			ValueToSet.SetInt(0);
			root->SetVariable("root.Menu_mc.wt1.bar1.min_value", &ValueToSet);
			ValueToSet.SetInt(100);
			root->SetVariable("root.Menu_mc.wt1.bar1.max_value", &ValueToSet);
			ValueToSet.SetInt(75);
			root->Invoke("root.Menu_mc.wt1.bar1.set_value", nullptr, &ValueToSet, 1);

			ValueToSet.SetInt(75);
			root->SetVariable("root.Menu_mc.wt1.x", &ValueToSet);
			root->SetVariable("root.Menu_mc.wt1.y", &ValueToSet);

			root->Invoke("root.Menu_mc.wt1.bar1.traceValues", nullptr, nullptr, 0);
			traceGFxValue(&menu_mc);
			break;
		}
		default:
			break;
		}
		return this->GameMenuBase::ProcessMessage(msg);
	};

	virtual void	DrawNextFrame(float unk0, void* unk1) final
	{
		//_DMESSAGE("DrawNextFrame");
		return this->GameMenuBase::DrawNextFrame(unk0, unk1);
	};

	static IMenu* CreateUWB_Menu()
	{
		return new UWB_Menu();
	}

	static void invoke_msg0()
	{
		static BSFixedString menuName("UWB_Menu");
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_msg0);
	}

	static void invoke_msg1()
	{
		static BSFixedString menuName("UWB_Menu");
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_msg1);
	}

	static void OpenMenu()
	{
		static BSFixedString menuName("UWB_Menu");
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Open);
	}

	static void CloseMenu()
	{
		static BSFixedString menuName("UWB_Menu");
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Close);
	}

	static void RegisterMenu()
	{
		static BSFixedString menuName("UWB_Menu");
		if ((*g_ui) != nullptr && !(*g_ui)->IsMenuRegistered(menuName))
		{
			(*g_ui)->Register("UWB_Menu", CreateUWB_Menu);
		}
		_DMESSAGE("RegisterMenu %s", (*g_ui)->IsMenuRegistered(menuName) ? "registered" : "not registered");
	}
};

void MessageCallback(F4SEMessagingInterface::Message* msg)
{
	switch (msg->type)
	{
	case F4SEMessagingInterface::kMessage_GameLoaded:
		RegisterLoadGameEvents();
		UWB_Menu::RegisterMenu();
		break;
	case F4SEMessagingInterface::kMessage_GameDataReady:
		RegisterDataReadyEvents();
		break;
	default:
		break;
	}
}

bool ScaleformCallback(GFxMovieView* view, GFxValue* value)
{
	GFxMovieRoot* movieRoot = view->movieRoot;

	GFxValue currentSWFPath;
	std::string currentSWFPathString = "";
	if (movieRoot->GetVariable(&currentSWFPath, "root.loaderInfo.url")) {
		currentSWFPathString = currentSWFPath.GetString();
		//_MESSAGE("hooking %s", currentSWFPathString.c_str());
	}
	return true;
}

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

bool UWB_test(StaticFunctionTag*)
{
	UWB_Menu::OpenMenu();
	return true;
}

bool UWB_test1(StaticFunctionTag*)
{
	//UWB_Menu::invoke_msg0();
	ReadJson();
	return true;
}

bool UWB_test2(StaticFunctionTag*)
{
	UWB_Menu::invoke_msg1();
	return true;
}

bool RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction0 <StaticFunctionTag, bool>("UWB_test", "MyDebug:debugS", UWB_test, vm));
	vm->RegisterFunction(
		new NativeFunction0 <StaticFunctionTag, bool>("UWB_test1", "MyDebug:debugS", UWB_test1, vm));
	vm->RegisterFunction(
		new NativeFunction0 <StaticFunctionTag, bool>("UWB_test2", "MyDebug:debugS", UWB_test2, vm));
	return true;
}

EventResult	UWB_TESContainerChangedEventSink::ReceiveEvent(TESContainerChangedEvent* evn, void* dispatcher)
{
	_DMESSAGE("TESContainerChangedEvent recieved:");
	DumpClass(evn, 20);

	return kEvent_Continue;
}

UWB_TESContainerChangedEventSink g_UWB_TESContainerChangedEventSink;

void RegisterLoadGameEvents()
{
	_DMESSAGE("RegisterLoadGameEvents");
}

void RegisterDataReadyEvents()
{
	_DMESSAGE("RegisterDataReadyEvents");
	REGISTER_EVENT(TESContainerChangedEvent, g_UWB_TESContainerChangedEventSink);
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
		g_papyrus = (F4SEPapyrusInterface*)f4se->QueryInterface(kInterface_Papyrus);
		if (!g_papyrus)
		{
			_MESSAGE("couldn't get papyrus interface");
			return false;
		}
		else {
			_MESSAGE("got papyrus interface");
		}
		g_scaleform = (F4SEScaleformInterface*)f4se->QueryInterface(kInterface_Scaleform);
		if (!g_scaleform)
		{
			_FATALERROR("couldn't get scaleform interface");
			return false;
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
		logMessage("load");
		if (g_scaleform)
			g_scaleform->Register("UWB", ScaleformCallback);

		if (g_messaging != nullptr)
			g_messaging->RegisterListener(g_pluginHandle, "F4SE", MessageCallback);
		if (g_papyrus)
		{
			g_papyrus->Register(RegisterFuncs);
			_MESSAGE("Papyrus Register Succeeded");
		}
		TESContainerChangedEvent_Dispatcher_Init();
		RVAManager::UpdateAddresses(f4se->runtimeVersion);
		return true;
	}

};
