#pragma once
class CLDSPart;
class UI{
public:
	static bool blEnableIntermediateUpdateUI;
	static bool blDisableIntermediateMessageBox;
	//blUpdateScr=true,��ʾˢ����Ļ��ʾ
	static void UpdatePartSolid(CLDSPart* pPart,bool blUpdateScr=true);
};
