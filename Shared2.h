#pragma once

#define CURRENT_RUNTIME_VERSION CURRENT_RELEASE_RUNTIME

#define DECLARE_EVENT_DISPATCHER_EX(Event, address) \
template<> inline BSTEventDispatcher<Event> * GetEventDispatcher() \
{ \
	typedef BSTEventDispatcher<Event> * (*_GetEventDispatcher)(); \
	RelocAddr<_GetEventDispatcher> GetDispatcher(address.GetUIntPtr()-RelocationManager::s_baseAddr); \
	return GetDispatcher(); \
}

#define REGISTER_EVENT(_event, _sink) GetEventDispatcher<_event>()->AddEventSink(&_sink);

#define REGISTER_EVENT_SIMPLE(_event, _functionName) \
class _functionName##_class : public BSTEventSink<_event> \
{ \
public: \
	virtual ~##_functionName##_class() { }; \
	virtual	EventResult	ReceiveEvent(_event * evn, void * dispatcher) override \
	{ \
		return _functionName(evn); \
	}; \
}; \
 \
static _functionName##_class _functionName##_sink; \
 \
GetEventDispatcher<_event>()->AddEventSink(&##_functionName##_sink);

namespace HookUtil
{
	uintptr_t SafeWrite64(uintptr_t addr, uintptr_t data);

	template <class Ty, class TRet, class... TArg>
	inline auto SafeWrite64(uintptr_t jumpSrc, TRet(Ty::* fn)(TArg...)) -> decltype(fn)
	{
		typedef decltype(fn) Fn;
		union
		{
			uintptr_t	u64;
			Fn			fn;
		} data;

		data.fn = fn;

		data.u64 = SafeWrite64(jumpSrc, data.u64);
		return data.fn;
	}

	template <class TRet, class... TArg>
	inline auto SafeWrite64(uintptr_t jumpSrc, TRet(*fn)(TArg...)) -> decltype(fn)
	{
		typedef decltype(fn) Fn;
		return (Fn)SafeWrite64(jumpSrc, (uintptr_t)fn);
	}
}

#include "f4se/PluginAPI.h"
#include "f4se/GameAPI.h"
#include "f4se/GameData.h"
#include <shlobj.h>
#include <string>

#include "GameReferences.h"

#include "Relocation/RVA.h"
#include "f4se_common/Relocation.h"

#include "ScaleformValue.h"

#include "SharedClasses_EventStruct.h"

extern IDebugLog	gLog;

void logMessage(std::string aString);

void InitReloc();

class InventoryInterface
{
public:
	struct FavoriteChangedEvent
	{

	};
	struct CountChangedEvent
	{

	};
	void*												unk00;
	BSTEventDispatcher<CountChangedEvent>				countChangedEventDispatcher;		// 08
	BSTEventDispatcher<FavoriteChangedEvent>			favoriteChangedEventDispatcher;		// 60
	struct Entry
	{
	public:
		UInt32			handleID; // 00
		UInt32			ownerHandle; // 04
		UInt16			itemPosition; // 08 position in inventoryList
		UInt16			unk0A;
	};
	STATIC_ASSERT(sizeof(Entry) == 0xC);
	tArray<Entry>										inventoryItems;						// B8
};
STATIC_ASSERT(sizeof(InventoryInterface) == 0xD0);

extern RelocPtr <InventoryInterface*> g_inventoryInterface;

class ActorEquipManager
{
public:
	void*												unk00;
	BSTEventDispatcher<ActorEquipManagerEvent::Event>	actorEquipManagerEventDispatcher;	// 08
};
STATIC_ASSERT(sizeof(ActorEquipManager) == 0x60);

extern RelocPtr <ActorEquipManager*> g_actorEquipManager;

void Shared2_Init();

extern RelocPtr <void*> g_holotapeStateChangedEventDispatcherAddress;
DECLARE_EVENT_DISPATCHER_EX(HolotapeStateChanged::Event, g_holotapeStateChangedEventDispatcherAddress)

extern RelocPtr <void*> g_bGSInventoryItemEventDispatcherAddress;
DECLARE_EVENT_DISPATCHER_EX(BGSInventoryItemEvent::Event, g_bGSInventoryItemEventDispatcherAddress)

extern RelocPtr <void*> g_playerDifficultySettingChangedDispatcherAddress;
DECLARE_EVENT_DISPATCHER_EX(PlayerDifficultySettingChanged::Event, g_playerDifficultySettingChangedDispatcherAddress)

typedef void(*_CollectDamageInfo)(void* param1, void* param2, void* param3);
extern RelocAddr <_CollectDamageInfo> CollectDamageInfo;

class BGSAttackData;

struct DamageFrame
{
	NiPoint3					hitLocation;					// 00
	UInt32						pad0C;							// 0C
	float						unk10[8];						// 10
	bhkNPCollisionObject* collisionObj;					// 30
	UInt64						unk38;							// 38
	UInt32						attackerHandle;					// 40
	UInt32						victimHandle;					// 44
	UInt64						unk48[(0x50 - 0x48) >> 3];		// 48
	BGSAttackData * attackData;					// 50 occur when use melee weapon.
	TESForm * damageSourceForm;				// 58
	TBO_InstanceData * instanceData;					// 60
	UInt64						unk68[(0x80 - 0x68) >> 3];		// 68
	TESAmmo * ammo;							// 80 occur when use gun.
	void* unk88;						// 88
	float						damage2;						// 90 game uses this value to calc final damage.
	float						unk94;							// 94
	float						damage;							// 98
};
STATIC_ASSERT(sizeof(DamageFrame) == 0xA0);

using _ProcessDamageFrame = void(*)(void*, DamageFrame*);
extern RelocAddr<_ProcessDamageFrame> ProcessDamageFrame;
extern RelocAddr <_ProcessDamageFrame> ProcessDamageFrameHookTarget;

class x_Message
{
public:
	UInt32		messageType;
	// ...
};

using _ProcessMessage = void(*)(x_Message*, void*, void*);
extern RelocAddr<_ProcessMessage> ProcessMessage;

void Shared2_RepairMenuInit();

void Shared2_InitEvents();

/*

*/
