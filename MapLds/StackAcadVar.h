#pragma once

class CStackAcadVar {
	bool m_bVarPushed;
	long m_liAcedVarValue;
	char m_szVarName[32];
public:
	CStackAcadVar(const char* szVarName,long liVarValue);
	bool PopupAcadVar();
	~CStackAcadVar();
};
