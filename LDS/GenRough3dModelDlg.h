#pragma once
#include "Resource.h"

// CGenRough3dModelDlg

class CGenRough3dModelDlg : public CDialog
{
	DECLARE_DYNAMIC(CGenRough3dModelDlg)

public:
	CGenRough3dModelDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CGenRough3dModelDlg();

// �Ի�������
	enum { IDD = IDD_GENERATE_ROUGH_3D_MODEL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bIntelliSetRodPosition;
	BOOL m_bIntelliDesignConnection;
	BOOL m_bMechanicPersist;
	BOOL m_bCombineColinearRods;
	BOOL m_bUsePerfectAngleG;
	double m_fTolerance;
	double m_fToleranceTangent;
	UINT m_nMinKeyLength;
};
