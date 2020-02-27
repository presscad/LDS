#pragma once
#include ".\Resource.h"
#include "CallBackDialog.h"
// CInputWirePointDlg �Ի���
class CLDSLineAngle;
class CMechanicsPlateCheckDlg : public CCallBackDialog
{
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX, nScrLocationY;
public:
	CMechanicsPlateCheckDlg(CWnd* pParent = nullptr);   // ��׼���캯��
	virtual ~CMechanicsPlateCheckDlg();
	//
// �Ի�������
	enum { IDD = IDD_MECHANICS_CHECK_DLG };
public:
	//{{AFX_VIRTUAL(CLinkTubeDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();    // DDX/DDV support
	//}}AFX_VIRTUAL
	DECLARE_MESSAGE_MAP()
protected:
	afx_msg void OnMove(int x, int y);
	afx_msg void OnBnRdoAxisForceCalmode();
public:
	struct {
		CLDSLineAngle* pOperAngle;
		double dfAxisNByBolts;	//��λkN
	}operangle;
	BOOL m_bConnStrengthCheck;
	BOOL m_bBoltBlockShearCheck;
	BOOL m_bEdgeStableCheck;
	double m_fRodAxisForce;
	int m_iAxisForceMode;
	CString m_sRayAngleLabel;
};
