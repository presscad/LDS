#pragma once
#include "ArrayList.h"
#include "list.h"
#include "LinkLabel.h"
#include "RememberLocationDlg.h"
// CLabelOperDlg 对话框

class CLabelOperDlg : public CRememberLocationDlg
{
	DECLARE_DYNAMIC(CLabelOperDlg)

	SmartPartPtr m_pCurrPart;
	CLDSPart* m_pMirXPart;
	CLDSPart* m_pMirYPart;
	CLDSPart* m_pMirZPart;
	CLDSPart* m_pMirRotPart;
	CLDSPart* m_pMirXnRotPart;
	CLDSPart* m_pMirYnRotPart;
	CLDSPart* m_pMirZnRotPart;
protected:
	char m_ciErrorMsgMode;
	CLinkLabel m_ctrlErrorMsg,m_ctrlAnchorWingStyle;
	CHashList<bool>hashProcessedParts;
	ARRAY_LIST<CLDSPartPtr> xPartArr;
	int m_indexCurrOperPart;
	void _InternalSetCurrPart(CLDSPart* pPart);
	bool UpdateCtrlStates();
	bool UpdatePrcoessStates();
	bool UpdateCurGroupPartInfo();
	void UpdateAngleGBySizeSpec(double width);
public:
	CLabelOperDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLabelOperDlg();
	void SetPartSet(IXhSet<CLDSPartPtr>* pPartSet);
// 对话框数据
	enum { IDD = IDD_LABEL_OPER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnNextLabelGroup();
	afx_msg void OnChangeEOriginPartNo();
	afx_msg void OnChangeEMirXOriginPartNo();
	afx_msg void OnChangeAngleWorkWingG();
	afx_msg void OnCbnSelchangeCmbSizeSpec();
	afx_msg void OnBnClickedChkSynRotation();
	afx_msg void OnEnChangeMirRotPartNo();
	afx_msg void OnEnChangeMirXnRotPartNo();
	//
	CString m_sSizeSpec;
	char m_cQualityLevel;
	int m_iMaterial;
	CString m_sOriginPartNo;
	CString m_sMirXOriginPartNo;
	CString m_sMirYOriginPartNo;
	CString m_sMirZOriginPartNo;
	CString m_sMirRotPartNo;
	CString m_sMirXnRotPartNo;
	CString m_sMirYnRotPartNo;
	CString m_sMirZnRotPartNo;
	CString m_sSegNo;
	BOOL m_bSyncPartSizeMat;
	BOOL m_bSyncRotationParts;
	int m_nWorkWingG;
	int m_nIdleWingG;
private:
	char m_ciWorkWing,m_ciIdleWing;	//'X','Y','A',0
};
