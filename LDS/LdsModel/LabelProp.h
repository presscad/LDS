#pragma once
#include <WinDef.h>
struct LABEL_PROP{
	static const DWORD ROD_ODDMENT_START = 0x00000001;	//始端正负头
	static const DWORD ROD_ODDMENT_END	 = 0x00000002;	//终端正负头
	static const DWORD ANGLE_CUT_START	 = 0x00000004;	//始端切角（肢）
	static const DWORD ANGLE_CUT_END	 = 0x00000008;	//终端切角（肢）
};
