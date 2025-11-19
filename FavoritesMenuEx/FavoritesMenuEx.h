#pragma once
#include "main.h"

class FavoritesMenuEx : public GameMenuBase
{
public:
    enum
    {
        kMessage_CloseDelayed = 0x10
    };
    FavoritesMenuEx();
    void Invoke(Args * args) final;
    void RegisterFunctions() final;
    UInt32 ProcessMessage(UIMessage * msg) final;
    static IMenu * CreateFavoritesMenuEx();
    static void OpenMenu();
    static void CloseMenu();
    static void CloseMenuDelayed();
    static void RegisterMenu();
    static void SelectionChange(int val);
    static void DownPressed();
    static void UpPressed();
    static void LeftPressed();
    static void RightPressed();
    static int GetSelection();
    static int GetSelectedSector();
};
