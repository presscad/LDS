#pragma once
#include "afxwin.h"
#include "ServerTowerType.h"

// CFileGroupDlg 对话框

class CFileGroupDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileGroupDlg)
	CServerTowerFile *m_pCurFile;
public:
	CFileGroupDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFileGroupDlg();

// 对话框数据
	enum { IDD = IDD_FILEGROUP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	//{{AFX_MSG(CFileGroupDlg)
	afx_msg void OnSelchangeCmbCurFile();
	afx_msg void OnChangeSegStr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BYTE m_ciStartMode;	//0.新建编辑模式;1.关联映射模式
	static const BYTE EDIT_FILEGROUP = 0;
	static const BYTE EDIT_TOWERTYPE = 1;
	char m_ciObjectType;//0.新建塔型文件分组;1.新建塔型 2.新建文件
	static const BYTE OBJ_NEW_FILEGROUP	= 0;
	static const BYTE OBJ_NEW_TOWERTYPE	= 1;
	static const BYTE OBJ_NEW_TOWERFILE	= 2;
	CServerTowerFileGroup *m_pFileGroup;
	CString m_sSegStr;
	CString m_sName;
	CComboBox m_cmbCurFile;
	CServerTowerFile *GetFileGroupCurFile();
	CString m_sOriginalFileName;
	CString m_sCurFileNotes;
	CString m_sTowerTypeName;
};