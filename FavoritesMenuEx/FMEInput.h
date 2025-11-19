#pragma once
#include "f4se/GameInput.h"

extern UInt8 iHoldKey;

/*
class ThumbstickEventEx : public IDEvent, public InputEvent
{
public:
    UInt64 stick;
    float x;
    float y;
    UInt32 unk40; // direction with deadzones 1 up 2 right 3 down 4 left
    UInt32 unk44; // previous direction
};
STATIC_ASSERT(sizeof(ThumbstickEventEx) == 0x048);
*/
class FMEInput : public BSInputEventUser
{
public:
    static FMEInput& GetInstance() {
        static FMEInput instance;
        return instance;
    }

    FMEInput(FMEInput const&)		= delete;
    void operator=(FMEInput const&) = delete;

    void RegisterForInput(bool bRegister);
    bool ShouldHandleEvent(InputEvent * inputEvent);

    // Input Handlers
    virtual void OnButtonEvent(ButtonEvent * inputEvent);
private:
    FMEInput() : BSInputEventUser(true) { }
};
