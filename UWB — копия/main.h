#pragma once
#include "Shared.h"
#include "f4seee/f4seeeEvents.h"

class UWB_TESContainerChangedEventSink : public BSTEventSink<TESContainerChangedEvent>
{
public:
	virtual	EventResult	ReceiveEvent(TESContainerChangedEvent* evn, void* dispatcher) override;
};

void RegisterLoadGameEvents();

void RegisterDataReadyEvents();
