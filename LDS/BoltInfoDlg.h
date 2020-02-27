#pragma once
#include "afxcmn.h"
#include "LDSView.h"
#include "XhListCtrl.h"


// CBoltInfoDlg 对话框

#ifndef AFX_TARG_ENU_ENGLISH
class CBoltInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBoltInfoDlg)
public:
	CBoltInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBoltInfoDlg();
// 对话框数据
	enum { IDD = IDD_BOLT_INFO_DLG };
	struct BOLTINFO
	{
		long hBolt;
		CLDSBolt* pBolt; 
		THANDLE   hAngle; //归属角钢
		int       iOffsetWing;	//0.X肢；1.Y肢
		double    fWingOffsetDist;
		BOLTINFO(CLDSBolt* pBolt=NULL);
		void SetKey(DWORD key){hBolt=(long)key;}
	};
	struct ANGLEINFO
	{
		long hAngle;
		bool m_bDoubleRowX,m_bDoubleRowY;
		short m_siXG1,m_siYG1;
		short m_nBoltInX,m_nBoltInY;
	public:
		ANGLEINFO();
		void SetKey(DWORD key){hAngle=(long)key;}
		void VerifyResideBoltInfo(BOLTINFO* pBoltInfo);
	};
public:
	BOOL m_bGroupSelect;
	CXhListCtrl m_boltInfoListCtrl;
	CHashListEx<BOLTINFO> hashBoltInfoByHandle;
public:
	void UpdateBoltInfoListCtrl(long hDefaultAngle,int iDefaultWing);
	void SelectBoltInfos();
	void SelectDefaultBolts();
	void ActivateDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
	void GetSelectBoltFrom3D();
	virtual void OnCancel();
public:
	afx_msg void OnBnSwapG();
	afx_msg void OnClickBoltInfoList(NMHDR* pNMHDR, LRESULT* pResult);
};
extern CBoltInfoDlg *g_pBoltInfoDlg;
#endif