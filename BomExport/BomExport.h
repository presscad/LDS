// BomExport.h : BomExport DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CBomExportApp
// �йش���ʵ�ֵ���Ϣ������� BomExport.cpp
//

class CBomExportApp : public CWinApp
{
public:
	CBomExportApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
