#pragma once
#include "GameMenus.h"
#include "main.h"

extern std::map<UInt32, UInt32> markersMap;


extern PipboyObject * player;
extern PipboyPrimitiveThrottledValue<float>* X;
extern PipboyPrimitiveThrottledValue<float>* Rotation;
extern PipboyPrimitiveThrottledValue<float>* Y;

extern float _x;
extern float _y;
extern float _rotation;

class MinimapMenu : public GameMenuBase
{
public:

    MinimapMenu();
    void Invoke(Args * args) final;
    void AdvanceMovie(float unk0, void* unk1);
    void RegisterFunctions() final;
    UInt32 ProcessMessage(UIMessage * msg) final;
    static IMenu * CreateMinimapMenu();
    static void OpenMenu();
    static void CloseMenu();
    static void RegisterMenu();
    static void SetMapParams();
    static void LoadMarkers(GFxMovieRoot* root = nullptr);
};
