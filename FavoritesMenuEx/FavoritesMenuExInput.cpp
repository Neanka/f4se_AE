#include "FavoritesMenuExInput.h"

#include "FavoritesMenuEx.h"
#include "FMEInput.h"
#include "f4se/GameMenus.h"
#include "main.h"

void FavoritesMenuExInput::RegisterForInput(bool bRegister)
{
    tArray<BSInputEventUser*>* inputEvents = &(*g_menuControls)->inputEvents;
    BSInputEventUser* inputHandler = this;
    int idx = inputEvents->GetItemIndex(inputHandler);
    if (idx > -1)
    {
        if (!bRegister)
        {
            inputEvents->Remove(idx);
            _MESSAGE("FavoritesMenuExInput: Unregistered for input events.");
        }
    }
    else
    {
        if (bRegister)
        {
            inputEvents->Push(inputHandler);
            _MESSAGE("FavoritesMenuExInput: Registered for input events.");
        }
    }
}

bool FavoritesMenuExInput::ShouldHandleEvent(InputEvent* inputEvent)
{
    return true;
}

void FavoritesMenuExInput::OnButtonEvent(ButtonEvent* inputEvent)
{
    float timer = inputEvent->timer;
    bool isDown = inputEvent->isDown == 1.0f && timer == 0.0f;
    bool isUp = inputEvent->isDown == 0.0f && timer != 0.0f;
    //if (isDown) _MESSAGE("control: %s", inputEvent->controlID.c_str());
    if (strcmp(inputEvent->controlID.c_str(), "Cancel") == 0 && iHoldKey == 0) FavoritesMenuEx::CloseMenu();
    else if ((*g_inputDeviceMgr)->IsGamepadEnabled())
    {
        if (isDown && iHoldKey == 0)
        {
            if (strcmp(inputEvent->controlID.c_str(), "Accept") == 0)
            {
                FavoritesMenuEx::CloseMenuDelayed();
                //UseQuickKey(FavoritesMenuEx::GetSelection());
            }
            else if (strcmp(inputEvent->controlID.c_str(), "Down") == 0) FavoritesMenuEx::DownPressed();
            else if (strcmp(inputEvent->controlID.c_str(), "Up") == 0) FavoritesMenuEx::UpPressed();
            else if (strcmp(inputEvent->controlID.c_str(), "Left") == 0) FavoritesMenuEx::LeftPressed();
            else if (strcmp(inputEvent->controlID.c_str(), "Right") == 0) FavoritesMenuEx::RightPressed();
        }
        else if (isUp && iHoldKey == 1)
        {
            if (strcmp(inputEvent->controlID.c_str(), "Down") == 0
                || strcmp(inputEvent->controlID.c_str(), "Up") == 0
                || strcmp(inputEvent->controlID.c_str(), "Left") == 0
                || strcmp(inputEvent->controlID.c_str(), "Right") == 0)
            {
                FavoritesMenuEx::CloseMenuDelayed();
                //UseQuickKey(FavoritesMenuEx::GetSelection());
            }
        }
    }
    else // no gamepad
    {
        if (isDown && iHoldKey == 0)
        {
            if (strcmp(inputEvent->controlID.c_str(), "Quickkeys") == 0)
            {
                FavoritesMenuEx::CloseMenu();
            }
            else if (strcmp(inputEvent->controlID.c_str(), "Activate") == 0)
            {
                FavoritesMenuEx::CloseMenuDelayed();
                //UseQuickKey(FavoritesMenuEx::GetSelection());
            }
        }
        else if (isUp && strcmp(inputEvent->controlID.c_str(), "Quickkeys") == 0 && iHoldKey == 1)
        {
            FavoritesMenuEx::CloseMenuDelayed();
            //UseQuickKey(FavoritesMenuEx::GetSelection());
        }
    }
}

void FavoritesMenuExInput::OnThumbstickEvent(ThumbstickEvent* inputEvent)
{
    static BSFixedString menuName("FavoritesMenuEx");
    if (!(*g_ui)->IsMenuOpen(menuName)) return;
    if (inputEvent->stick == iUseRightStick && inputEvent->unk40 != 0 && inputEvent->unk44 != 0)
    {
        if (inputEvent->y == 0 && inputEvent->y == 0) return;
        int ang = round(450 - std::atan2(inputEvent->y, inputEvent->x) * 180 / 3.14159265358979323846);
        ang = (ang % 360) / 30;
        if (ang != FavoritesMenuEx::GetSelectedSector())
        {
            FavoritesMenuEx::SelectionChange(ang);
        }
    }
}
