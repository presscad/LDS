#pragma once
#include "afxwin.h"
#include "ServerTowerType.h"

// CFileGroupDlg �Ի���

class CFileGroupDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileGroupDlg)
	CServerTowerFile *m_pCurFile;
public:
	CFileGroupDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFileGroupDlg();

// �Ի�������
	enum { IDD = IDD_FILEGROUP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	//{{AFX_MSG(CFileGroupDlg)
	afx_msg void OnSelchangeCmbCurFile();
	afx_msg void OnChangeSegStr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BYTE m_ciStartMode;	//0.�½��༭ģʽ;1.����ӳ��ģʽ
	static const BYTE EDIT_FILEGROUP = 0;
	static const BYTE EDIT_TOWERTYPE = 1;
	char m_ciObjectType;//0.�½������ļ�����;1.�½����� 2.�½��ļ�
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