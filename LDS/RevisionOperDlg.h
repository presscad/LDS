#pragma once
#include "afxcmn.h"
#include "PropertyList.h"
#include "supergridctrl.h"
#include "PostProcessReport.h"
// CRevisionOperDlg 对话框

#if defined(__LDS_)||defined(__TSA_)
class CRevisionOperDlg : public CDialog
{
	bool m_bLockedLocateElemFunc;	//保证LocateElem不被重复循环调用
	bool m_bLockedUpdateElemFunc;	//保证UpdateElem不被重复循环调用
	CPropertyList m_propList;
	CHashList<bool> hashSortOrderType;;
	CString m_cmbRsltItems;
	CString m_sActiveRsltItemKeyStr;
	CBitmap m_curBitmap1,m_curBitmap2,m_curBitmap3;
private:
	CString GetPropValueStr(int idProp);
	void DisplayOneGroupSetProperty(int group,CPropTreeItem *pGroupItem);
	void DisplaySetPropList();
public:
	CTabCtrl m_tabCtrlListGroup;
	CSuperGridCtrl m_listElems;
	CRevisionOperDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRevisionOperDlg();
	void SetColumnItemState(int idProp,bool bDisplay);
	DECLARE_DYNCREATE(CRevisionOperDlg)
	static void InitPropItemHashtable();	//初始肘板型肋板各参数的基础信息(数据键值、类型、属性名称及帮助提示）

// 对话框数据
	enum { IDD = IDD_REVISION_OPER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTcnSelchangeTabCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnColumnclickOutputInfoList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedReadRsltItem();
	afx_msg void OnBnClickedSaveNewItem();
	afx_msg void OnBnClickedDelRsltItem();
	afx_msg void OnBnClickedExportRsltRptFile();
	bool IsLockedLoacateElemFunc(){return m_bLockedLocateElemFunc;}
	void UpdateRevision(CPostProcessReport &report,int iOrderCol=0,bool bAscending=true);
	void ReleaseAllItemSelMark();
	void LocateElem(long iStartPointI,long iEndPointI);
	void UpdateElem(long iStartPointI,long iEndPointI);
	void UpdateRsltNames();
};
#endif
