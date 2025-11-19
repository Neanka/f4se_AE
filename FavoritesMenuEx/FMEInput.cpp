#include "FMEInput.h"

#include "FavoritesMenuEx.h"
#include "main.h"
#include "Shared.h"

UInt8 iHoldKey = 0;

void FMEInput::RegisterForInput(bool bRegister)
{
    tArray<BSInputEventUser*>* inputEvents = &(*g_menuControls)->inputEvents;
    BSInputEventUser* inputHandler = this;
    int idx = inputEvents->GetItemIndex(inputHandler);
    if (idx > -1)
    {
        if (!bRegister)
        {
            inputEvents->Remove(idx);
            _MESSAGE("FMEInput: Unregistered for input events.");
        }
    }
    else
    {
        if (bRegister)
        {
            inputEvents->Push(inputHandler);
            _MESSAGE("FMEInput: Registered for input events.");
        }
    }
}

bool FMEInput::ShouldHandleEvent(InputEvent* inputEvent)
{
    return FavoritesManager__inputEventUser__ShouldHandleEvent(&(*g_favoritesManager)->inputEventUser, inputEvent);
}




uintptr_t NVAddress() {
    static uintptr_t chwBaseAddr = (uintptr_t)GetModuleHandle("F4NV_Core.dll");
    return chwBaseAddr;
}

uintptr_t NVAddress(uintptr_t offset) {
    return NVAddress() + offset;
}

template <typename T>
class RelocAddrNV
{
public:
    RelocAddrNV(uintptr_t offset)
        :m_offset(reinterpret_cast <BlockConversionType *>(offset + NVAddress()))
    {
        //
    }

    operator T()
    {
        return reinterpret_cast <T>(m_offset);
    }

    uintptr_t GetUIntPtr() const
    {
        return reinterpret_cast <uintptr_t>(m_offset);
    }

private:
    struct BlockConversionType { };
    BlockConversionType * m_offset;

    // hide
    RelocAddrNV();
    RelocAddrNV(RelocAddrNV & rhs);
    RelocAddrNV & operator=(RelocAddrNV & rhs);
};

void SwitchAmmo()
{
    if (!NVAddress(0)) {
        _MESSAGE("Cannot find F4NV_Core.dll!");
        return ;
    }

    _MESSAGE("NV BaseAddr: %016I64X", NVAddress());
    typedef void*(*_switchAmmo)();
    RelocAddrNV <_switchAmmo> switchAmmo(0x33CB90);
    switchAmmo();
}

void FMEInput::OnButtonEvent(ButtonEvent* inputEvent)
{
    if (bSettingsChanged)
    {
        ReadSettings();
        bSettingsChanged = false;
    }
    float timer = inputEvent->timer;
    bool isDown = inputEvent->isDown == 1.0f && timer == 0.0f;
    bool isUp = inputEvent->isDown == 0.0f && timer != 0.0f;
    //if (isDown) _MESSAGE("control: %s", inputEvent->controlID.c_str());
    if (isDown)
    {
        auto index = FavoritesManager_getQuickKeyIndex(*g_favoritesManager, inputEvent->GetControlID());
        if (index != 12)
        {
            UseQuickKey(index);
            return;
        }
        if ((*g_inputDeviceMgr)->IsGamepadEnabled())
        {
            if (strcmp(inputEvent->controlID.c_str(), "QuickkeyUp") == 0)
            {
                switch (iDpadUpAction)
                {
                case 1:
                    {
                        FavoritesMenuEx::OpenMenu();
                        break;
                    }
                case 2:
                    {
                        _MESSAGE("switch ammo");
                        SwitchAmmo();
                        break;
                    }
                default:
                    break;
                }
            }
            else if (strcmp(inputEvent->controlID.c_str(), "QuickkeyDown") == 0)
            {
                switch (iDpadDownAction)
                {
                case 1:
                    {
                        FavoritesMenuEx::OpenMenu();
                        break;
                    }
                case 2:
                    {
                        _MESSAGE("switch ammo");
                        SwitchAmmo();
                        break;
                    }
                default:
                    break;
                }
            }
            else if (strcmp(inputEvent->controlID.c_str(), "QuickkeyLeft") == 0)
            {
                switch (iDpadLeftAction)
                {
                case 1:
                    {
                        FavoritesMenuEx::OpenMenu();
                        break;
                    }
                case 2:
                    {
                        _MESSAGE("switch ammo");
                        SwitchAmmo();
                        break;
                    }
                default:
                    break;
                }
            }
            else if (strcmp(inputEvent->controlID.c_str(), "QuickkeyRight") == 0)
            {
                switch (iDpadRightAction)
                {
                case 1:
                    {
                        FavoritesMenuEx::OpenMenu();
                        break;
                    }
                case 2:
                    {
                        _MESSAGE("switch ammo");
                        SwitchAmmo();
                        break;
                    }
                default:
                    break;
                }
            }
        }        
        else if (strcmp(inputEvent->controlID.c_str(), "Quickkeys") == 0) FavoritesMenuEx::OpenMenu();
    }

}
