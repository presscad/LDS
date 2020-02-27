#pragma once
class CLDSPart;
class UI{
public:
	static bool blEnableIntermediateUpdateUI;
	static bool blDisableIntermediateMessageBox;
	//blUpdateScr=true,表示刷新屏幕显示
	static void UpdatePartSolid(CLDSPart* pPart,bool blUpdateScr=true);
};
