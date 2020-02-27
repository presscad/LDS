#if !defined(AFX_KEYBOARDMAPDLG_H__DC06BA92_169C_4EF4_8DC4_094AA004AC89__INCLUDED_)
#define AFX_KEYBOARDMAPDLG_H__DC06BA92_169C_4EF4_8DC4_094AA004AC89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KeyboardMapDlg.h : header file
//

#include "XhListCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CKeyboardMapDlg dialog
struct SYS_CMD_ITEM
{
	CXhChar50 m_sCmdName;	//名称
	CXhChar50 m_sShortCut;	//快捷方式
	CXhChar50 m_sCmdStr;	//命令行
	CXhChar50 m_sDescript;	//描述
	//
	SYS_CMD_ITEM(){}
	SYS_CMD_ITEM(const char* name,const char* short_cut,const char* cmd_str,const char* descript)
	{
		m_sCmdName.Copy(name);
		m_sShortCut.Copy(short_cut);
		m_sCmdStr.Copy(cmd_str);
		m_sDescript.Copy(descript);
	}
};

class CKeyboardMapDlg : public CDialog
{
// Construction
	CHashStrList<SYS_CMD_ITEM> m_xCmdMapHash;
public:
	CKeyboardMapDlg(CWnd* pParent = NULL);   // standard constructor
	//
	void InitCmdMapItems();
// Dialog Data
	//{{AFX_DATA(CKeyboardMapDlg)
	enum { IDD = IDD_KEYBOARD_MAP_DLG };
	CXhListCtrl	m_listKeyboardMap;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyboardMapDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CKeyboardMapDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYBOARDMAPDLG_H__DC06BA92_169C_4EF4_8DC4_094AA004AC89__INCLUDED_)
