#pragma once
#include "f4se/GameInput.h"

class FavoritesMenuExInput : public BSInputEventUser
{
public:
    static FavoritesMenuExInput& GetInstance() {
        static FavoritesMenuExInput instance;
        return instance;
    }

    FavoritesMenuExInput(FavoritesMenuExInput const&)		= delete;
    void operator=(FavoritesMenuExInput const&) = delete;

    void RegisterForInput(bool bRegister);
    bool ShouldHandleEvent(InputEvent * inputEvent);

    // Input Handlers
    virtual void OnButtonEvent(ButtonEvent * inputEvent);
    virtual void OnThumbstickEvent(ThumbstickEvent * inputEvent);
private:
    FavoritesMenuExInput() : BSInputEventUser(true) { }
};
