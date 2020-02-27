#if !defined(AFX_TRANSFORMWORKSTATUSDLG_H__D91AFB53_6C2C_412F_80A5_4AF7710D75DF__INCLUDED_)
#define AFX_TRANSFORMWORKSTATUSDLG_H__D91AFB53_6C2C_412F_80A5_4AF7710D75DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransformWorkStatusDlg.h : header file
//
#include "PropertyList.h"
#include "supergridctrl.h"
#include "Tower.h"
#include "Markup.h"
#include "CallBackDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CTransformWorkStatusDlg dialog

#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
class CTransformWorkStatusDlg : public CCallBackDialog
{
// Construction
public:
	CTower *m_pTower;
	CWireLayout *m_pWireLayout;
	int m_iTabGroup;
	CTransformWorkStatusDlg(CWnd* pParent = NULL);   // standard constructor
	//
	BOOL ImportXMLToGroupWorkStatus(CMarkup xml,CWorkStatusGroupInfo &groupWorkStatus,CString sElemName,
									CStringKeyHashTable<long> *pWorkEnvHashtable=NULL,long *pWorkStatusKey=NULL);
	BOOL ExportGroupWorkStatusToXML(CMarkup &xml,CWorkStatusGroupInfo &groupWorkStatus,
									CString sElemName,const void *pObj=NULL);
	BOOL GroupWorkStatusXMLSerialize(CString sFileName,BOOL bWrite,CWorkStatusGroupInfo &groupWorkStatus);
// Dialog Data
	//{{AFX_DATA(CTransformWorkStatusDlg)
	enum { IDD = IDD_TRANSFORM_WORKSTATUS_LOAD_DLG };
	CTabCtrl	m_ctrlPropGroup;
	CSuperGridCtrl	m_listCtrl;
	CString	m_sModelLimStr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransformWorkStatusDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTransformWorkStatusDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnImportTemplateXml();
	virtual void OnOK();
	afx_msg void OnEditItemPropItem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void RefreshListCtrl(int iTabGroup);
	void FinishSelectObjOper();		//完成选择对象的后续操作
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSFORMWORKSTATUSDLG_H__D91AFB53_6C2C_412F_80A5_4AF7710D75DF__INCLUDED_)
