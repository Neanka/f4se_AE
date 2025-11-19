#include "main.h"

std::string mName = "repairmenu";
UInt32 mVer = 1;

PluginHandle			    g_pluginHandle = kPluginHandle_Invalid;

#include "SharedClasses_Pipboy.h"

F4SEPapyrusInterface		*g_papyrus = NULL;

// typedef void (*func)(int, int);
//using func = void (*) (int, int);

//typedef void(*_sub_140C0F860)(void* manager, void* param2);

#include "PapyrusNativeFunctions.h"


typedef UInt8(*_uf1)(void* arg1, void* arg2, void** arg3);
RelocAddr <_uf1> uf1_HookTarget(0x2D4BF88);
_uf1 uf1_Original;

UInt8 uf1_Hook(void* arg1, void* arg2, void** arg3)
{
	_MESSAGE("uf1_Hook");
	DumpClass(arg1, 2);
	DumpClass(arg2, 1);
	DumpClass(*arg3, 10); 

	//UInt8 result = uf1_Original(arg1, arg2, arg3);
	//_MESSAGE("res: %u", result);
	return 1;
}

typedef UInt8(*_tf1)(void* thissink, BSAnimationGraphEvent* evnstruct, void** dispatcher);
RelocAddr <_tf1> tf1_HookTarget(0x2D44250);
_tf1 tf1_Original;

class BSAnimationGraphEvent
{
public:
	TESForm* unk00; 
	BSFixedString	eventName;
	//...etc
};
UInt8 tf1_Hook(void* arg1, BSAnimationGraphEvent* arg2, void** arg3)
{
	//_MESSAGE("tf1_Hook");

	//DumpClass(arg1, 1);
	//DumpClass(arg2, 10);
	//DumpClass((void*)((uintptr_t)arg3-0x68), 0xC0/8);
	//_MESSAGE("%s", arg2->eventName.c_str());
	return tf1_Original(arg1, arg2, arg3);
}

EventResult DC_Function(PlayerDifficultySettingChanged::Event * evn)
{
	_DMESSAGE("PlayerDifficultySettingChanged::Event recieved");
	return kEvent_Continue;
};


_CollectDamageInfo CollectDamageInfo_Original;

void CollectDamageInfo_Hook(void* param1, void* param2, void* param3) {
	_MESSAGE("CollectDamageInfo_Hook");
	CollectDamageInfo_Original(param1, param2, param3);
}


_ProcessMessage ProcessMessage_Original;

void ProcessMessage_Hook(x_Message* param1, void* param2, void* param3) 
{
	if (param1->messageType == 0xE)
	{
		_MESSAGE("ProcessMessage_Hook %08X", param1->messageType);
		DumpClass(param1,10);
	}
	if (param1->messageType == 0x9)
	{
		_MESSAGE("ProcessMessage_Hook %08X", param1->messageType);
		DumpClass(param1, 10);
	}
	ProcessMessage_Original(param1, param2, param3);
	
}


void ProcessDamageFrame_Hook(Actor* pObj, DamageFrame* pDamageFrame)
{
	_MESSAGE("ProcessDamageFrame_Hook");
	ProcessDamageFrame(pObj, pDamageFrame);
}


using _unkDamageFunction = void(*)(void*, void*);
RelocAddr<_unkDamageFunction> unkDamageFunction (0xFCC540);

_unkDamageFunction unkDamageFunction_Original;

#include "f4se/GameMessages.h"
bool RMtestfunk(StaticFunctionTag *base) {
	_MESSAGE("repairmenu testfunk");
	DumpClass(*g_messageQueue, 800);
//	DumpClass(*g_PipboyDataManager, 800);
	//tracePipboyObject((PipboyObject*)&(*g_PipboyDataManager)->mainObject);

	//REGISTER_EVENT_SIMPLE(PlayerDifficultySettingChanged::Event, DC_Function);
	/*
	PipboyObject* xPO = CreatePipboyObject(nullptr);

	PipboyArray* xPA = CreatePipboyArray(xPO);
	CALL_MEMBER_FN(xPO, SetMember)(&BSFixedString("some array"), xPA);

	PipboyObject* xPO1 = CreatePipboyObject(xPA);
	CALL_MEMBER_FN(xPO1, SetMember_BSFixedStringCS)(&BSFixedString("some string var"), &BSFixedString("text"));
	xPA->Insert(xPO1, 0);
	PipboyObject* xPO2 = CreatePipboyObject(xPA);
	xPA->Insert(xPO2, 0);
	tracePipboyObject(xPO);
	*/
	
	return true;
}

bool RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction0 <StaticFunctionTag, bool>("RMtestfunk", "MyDebug:debugS", RMtestfunk, vm));
	return true;
}

#include "f4se_common//BranchTrampoline.h"
#include "f4se_common/SafeWrite.h"
#include "xbyak/xbyak.h"

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
		// get the papyrus interface and query its version
		g_papyrus = (F4SEPapyrusInterface *)f4se->QueryInterface(kInterface_Papyrus);
		if (!g_papyrus)
		{
			_MESSAGE("couldn't get papyrus interface");
			return false;
		}
		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface *f4se)
	{
		logMessage("load");
		if (!g_branchTrampoline.Create(1024 * 64))
		{
			_ERROR("couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
			return false;
		}

		if (!g_localTrampoline.Create(1024 * 64, nullptr))
		{
			_ERROR("couldn't create codegen buffer. this is fatal. skipping remainder of init process.");
			return false;
		}
		if (g_papyrus)
		{
			g_papyrus->Register(RegisterFuncs);
			_MESSAGE("Papyrus Register Succeeded");
		}
		sig_scan_timer tmr; tmr.start();
		plugin_info.plugin_name = "def_shared";
		SharedClasses_PipboyInit();
		Shared2_Init();
		Shared2_InitEvents();
		Shared2_RepairMenuInit();
		//InitReloc(mName.c_str());
		_MESSAGE("Sigscan elapsed: %llu ms.", tmr.stop());


		/*{
			struct CollectDamageInfo_Code : Xbyak::CodeGenerator {
				CollectDamageInfo_Code(void* buf) : Xbyak::CodeGenerator(4096, buf)
				{
					Xbyak::Label retnLabel;

					mov(rax, rsp);
					mov(ptr[rax + 0x10], rdx);
					jmp(ptr[rip + retnLabel]);

					L(retnLabel);
					dq(CollectDamageInfo.GetUIntPtr() + 7);
				}
			};

			void* codeBuf = g_localTrampoline.StartAlloc();
			CollectDamageInfo_Code code(codeBuf);
			g_localTrampoline.EndAlloc(code.getCurr());

			CollectDamageInfo_Original = (_CollectDamageInfo)codeBuf;

			g_branchTrampoline.Write6Branch(CollectDamageInfo.GetUIntPtr(), (uintptr_t)CollectDamageInfo_Hook);
		}

		g_branchTrampoline.Write5Call(ProcessDamageFrameHookTarget.GetUIntPtr(), (uintptr_t)ProcessDamageFrame_Hook);


		{
			struct ProcessMessage_Code : Xbyak::CodeGenerator {
				ProcessMessage_Code(void* buf) : Xbyak::CodeGenerator(4096, buf)
				{
					Xbyak::Label retnLabel;

					mov(r11, rsp);
					push(rbp);
					push(rsi);
					jmp(ptr[rip + retnLabel]);

					L(retnLabel);
					dq(ProcessMessage.GetUIntPtr() + 5);
				}
			};

			void* codeBuf = g_localTrampoline.StartAlloc();
			ProcessMessage_Code code(codeBuf);
			g_localTrampoline.EndAlloc(code.getCurr());

			ProcessMessage_Original = (_ProcessMessage)codeBuf;

			g_branchTrampoline.Write5Branch(ProcessMessage.GetUIntPtr(), (uintptr_t)ProcessMessage_Hook);
		}*/

		//uf1_Original = HookUtil::SafeWrite64(uf1_HookTarget.GetUIntPtr(), &uf1_Hook);
		//unsigned char data[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
		//SafeWriteBuf(RelocAddr<uintptr_t>(0x000E038B2).GetUIntPtr(), &data, sizeof(data));
		//unsigned char data2[] = { 0x90, 0xE9, 0xE8, 0x02, 0x00 };
		//SafeWriteBuf(RelocAddr<uintptr_t>(0x00E037E7).GetUIntPtr(), &data2, sizeof(data2));
		unsigned char data3[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
		SafeWriteBuf(RelocAddr<uintptr_t>(0x000E03893).GetUIntPtr(), &data3, sizeof(data3));
		tf1_Original = HookUtil::SafeWrite64(tf1_HookTarget.GetUIntPtr(), &tf1_Hook);
		return true;
	}

};
