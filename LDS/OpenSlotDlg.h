#pragma once

#include "resource.h"
#include "lds_part.h"
#include "RememberLocationDlg.h"
//////////////////////////////////////////////////////////////////////////
// COpenSlotDlg 对话框
class COpenSlotDlg : public CRememberLocationDlg
{
	DECLARE_DYNAMIC(COpenSlotDlg)
public:
	COpenSlotDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COpenSlotDlg();

// 对话框数据
	enum { IDD = IDD_OPEN_SLOT_DLG };
	double m_fSlotH;
	double m_fSlotW;
	double m_fOffLen;
	double m_fDeltaS;
	double m_fDeltaE;
	int m_iCutWing;
public:
	CLDSLineAngle* m_pCurJg;
	//
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
	
};
