#pragma once
#include "ArrayList.h"
#include "list.h"
#include "LinkLabel.h"
#include "CmdLineMiniPanel.h"


// CEmbedLabelInputPanel 对话框

class CEmbedLabelInputPanel : public CCmdLineMiniPanel
{
	DECLARE_DYNAMIC(CEmbedLabelInputPanel)

	SmartPartPtr m_pCurrPart;
	CLDSPart* m_pMirXPart;
	CLDSPart* m_pMirYPart;
	CLDSPart* m_pMirZPart;
	CLDSPart* m_pMirRotPart;
	CLDSPart* m_pMirXnRotPart;
	CLDSPart* m_pMirYnRotPart;
	CLDSPart* m_pMirZnRotPart;
	struct PARTINFO{
		CXhChar16 szSizeSpec;
		BYTE ciMaterial;
		WORD wnWorkWingG;
		WORD wnIdleWingG;
		CXhChar16 szAnchorWing;
	public:
		char ciWorkWing,ciIdleWing;	//'X','Y','A',0
	};
	PARTINFO GetPartInfo(CLDSPart* pPart);
	void UpdatePartInfoToUI(PARTINFO* pPartInfo,bool blFrontTrueSideFalse=true);
	SmartPartPtr get_pCurrMirRotPart();
	__declspec(property(get=get_pCurrMirRotPart)) SmartPartPtr pCurrMirRotPart;
protected:
	char m_ciErrorMsgMode;
	CLinkLabel m_ctrlErrorMsg,m_ctrlAnchorWingStyle,m_ctrlSideAnchorWingStyle;
	CHashList<bool>hashProcessedParts;
	ARRAY_LIST<CLDSPartPtr> xPartArr;
	int m_indexCurrOperPart;
	void _InternalSetCurrPart(CLDSPart* pPart);
	bool UpdateCtrlStates();
	bool UpdatePrcoessStates();
	bool UpdateCurGroupPartInfo();
	void UpdateAngleGBySizeSpec(double width);
	void UpdateSlaveSideAngleGBySizeSpec(double width);
	void NotifyMasterPartSizeSpecChanged(double dfSizeWidth,double dfSizeThick);
	void NotifyMasterPartMaterialChanged(int iMatIndex);
public:
	CEmbedLabelInputPanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEmbedLabelInputPanel();
	void SetPartSet(IXhSet<CLDSPartPtr>* pPartSet);
	// 对话框数据
	enum { IDD = IDD_LABEL_OPER_PAGE };
	virtual UINT DlgResourceID(){return IDD_LABEL_OPER_PAGE;}
	void FillPanelTitle(const char* sName);
	bool ConfirmAndNextLabelGroup();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	void UpdateMirRotPartInfoByLabel();
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual int DispatchCmdLineMsg(const char* szCmdMessage);
	afx_msg void OnChangeEOriginPartNo();
	afx_msg void OnChangeEMirXOriginPartNo();
	afx_msg void OnChangeAngleWorkWingG();
	afx_msg void OnChangeSlaveSideAngleWorkWingG();
	afx_msg void OnCbnSelchangeCmbSizeSpec();
	afx_msg void OnCbnSelchangeSlaveCmbSizeSpec();
	afx_msg void OnCbnSelchangeCmbMaterial();
	afx_msg void OnBnClickedChkSynRotation();
	afx_msg void OnEnChangeMirRotPartNo();
	afx_msg void OnEnChangeMirXnRotPartNo();
	//
	CString m_sSizeSpec,m_sMirRotPartSizeSpec;
	int m_iMaterial,m_iMirRotPartMaterial;
	char m_cQualityLevel;
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
	int m_nWorkWingG,m_nMirRotWorkWingG;
	int m_nIdleWingG,m_nMirRotIdleWingG;
private:
	char m_ciWorkWing,m_ciIdleWing;	//'X','Y','A',0
	char m_ciMirRotWorkWing,m_ciMirRotIdleWing;	//'X','Y','A',0
};
