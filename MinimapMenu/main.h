#pragma once
#include "Shared.h"

#define MAP_TEXTURE "Textures\\Interface\\Pip-Boy\\WorldMap_d.dds"

extern int mcwidth;
extern int mcheight;
extern NiPoint2 NWCorner;
extern NiPoint2 NECorner;
extern NiPoint2 SWCorner;

extern float LEFT_GUTTER_PCT;
extern float RIGHT_GUTTER_PCT;
extern float TOP_GUTTER_PCT;
extern float BOTTOM_GUTTER_PCT;

class MMM_TESLoadGameHandler : public BSTEventSink<TESLoadGameEvent>
{
public:
    virtual ~MMM_TESLoadGameHandler() { };
    virtual	EventResult	ReceiveEvent(TESLoadGameEvent * evn, void * dispatcher) override;
};

 NiPoint2 ConvertWorldToLocalMarkerPosition(float x, float y);