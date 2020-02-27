#pragma once
#include "HashTable.h"
#include "MainFrm.h"
#include "XhListCtrl.h"
#include "PropertyList.h"

// CUserDefCmdGroupDlg 对话框

class CUserDefCmdGroupDlg : public CDialog
{
	DECLARE_DYNAMIC(CUserDefCmdGroupDlg)
	CListCtrl *m_pCmdListCtrl;
	void ContextMenu(CWnd *pWnd, CPoint point);
	void SetCheckState(BOOL bCheck);
	void UpdateCmdProperties(CMDCFG_ITEM *pCmdItem);
public:
	CMD_GROUP *m_pCmdGroup;
	CHashList<CMDCFG_ITEM>* m_pCmdItems;
	CUserDefCmdGroupDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUserDefCmdGroupDlg();
	//对命令组中所有命令都有效的临时属性
	CString m_sSegStr;		//段号范围
	BOOL m_bSelectScope;
	SCOPE_STRU m_scope;		//碰撞区域
//对话框数据
	//{{AFX_DATA(CUpdateUserDataDlg)
	enum { IDD = IDD_USERDEF_CMD_GROUP_DLG };
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUpdateUserDataDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CUpdateUserDataDlg)
	afx_msg void OnBnMoveToBottom();
	afx_msg void OnBnMoveDown();
	afx_msg void OnBnMoveToTop();
	afx_msg void OnBnMoveUp();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkPartnoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickPartnoList(NMHDR* pNMHDR, LRESULT* pResult);
	void OnCheckItems();
	void OnUncheckItems();
	//}}AFX_MSG
private:
	int AppendRecord(CListCtrl *pListCtrl,CString sItem,DWORD iParam,BOOL bCheck);
	int InsertRecord(CListCtrl* pListCtrl, CString sItem, DWORD iParam,int iInserPos,BOOL bCheck);
	int GetSelectItemIndex();
	void SelectItem(int iItem);

	DECLARE_MESSAGE_MAP()
public:
	int m_nMaxWarnings;
	BOOL m_bOutputToNotepad;
	afx_msg void OnLvnItemchangedListCmdItem(NMHDR *pNMHDR, LRESULT *pResult);
public:
	static CStringKeyHashTable<long>propHashtable;	//属性字符串哈希表
	static void InitCmdGroupPropHashtable();
	static long GetPropID(char* propStr);			//得到属性ID
private:
	CPropertyList m_propList;
};
