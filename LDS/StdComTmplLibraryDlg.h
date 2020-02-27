#pragma once
#include "afxcmn.h"
#include ".\StdComTmplStruPage.h"
#include ".\StdComTmplConnNodePage.h"
#include ".\StdComTempl\IStdComTempl.h"
#include "objptr_list.h"
#include "ComplexId.h"

class CStdComTmplLibraryDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStdComTmplLibraryDlg)

public:
	CStdComTmplLibraryDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStdComTmplLibraryDlg();
	static const int PANEL_STRUCTURE = 0;
	static const int PANEL_CONN_NODE = 1;
	static const int PANEL_FRAMELINE = 2;	//塔头横担模板
	int m_iCurrStdTmplGroup;
	CStdComTmplStruPage strupage;
	CStdComTmplConnNodePage nodepage;
public:
	UINT GetCurrSelStdComTmplId();
	__declspec(property(get=GetCurrSelStdComTmplId)) UINT StdComTmplId;
// 对话框数据
	enum { IDD = IDD_STD_COM_TMPL_LIBRARY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	CTabCtrl m_xStdTmplGroup;
	void CalclayoutPropPages();
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeTabComTmplGroup(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnOk();
	afx_msg void OnBnCancel();
};
