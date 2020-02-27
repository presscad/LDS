#pragma once

#include "resource.h"
#include "lds_part.h"
#include "RememberLocationDlg.h"
//////////////////////////////////////////////////////////////////////////
// COpenSlotDlg �Ի���
class COpenSlotDlg : public CRememberLocationDlg
{
	DECLARE_DYNAMIC(COpenSlotDlg)
public:
	COpenSlotDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~COpenSlotDlg();

// �Ի�������
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
	
};
