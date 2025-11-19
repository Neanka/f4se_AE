#pragma once
#include "Shared.h"

extern std::map<std::string, std::string> icons;
extern std::map<std::string, std::string> autoTags;
extern UInt8 iMode;
extern UInt8 iUseRightStick;

extern UInt8 iDpadUpAction;
extern UInt8 iDpadDownAction;
extern UInt8 iDpadLeftAction;
extern UInt8 iDpadRightAction;

extern UInt8 iSlowTime;
extern UInt8 iImad;

extern UInt8 iUseCustomColors;
extern UInt8 iColorR;
extern UInt8 iColorG;
extern UInt8 iColorB;

extern UInt8 iMenuStyle;

extern float fx;
extern float fy;
extern float fs;

extern bool bTimeSlowed;

extern bool bDEF_UIexist;
extern bool bFallUIexist;
extern bool bSettingsChanged;

void readXML(std::string file, int type);

void ReadSettings();

void UseQuickKey(SInt32 index);
#pragma 


