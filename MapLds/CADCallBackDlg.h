#if !defined(AFX_CADCALLBACKDLG_H__F07AFCF4_8860_4C20_A03A_E723AB08E4B6__INCLUDED_)
#define AFX_CADCALLBACKDLG_H__F07AFCF4_8860_4C20_A03A_E723AB08E4B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CADCallBackDlg.h : header file
//

#include "XhCharString.h"
#include "Tower.h"

/////////////////////////////////////////////////////////////////////////////
// CCADCallBackDlg dialog
typedef struct tagCAD_SCREEN_ENT
{
	CLDSObject *m_pObj;			//����ѡ������Ӧ����������
	AcDbObjectId m_idEnt;		//����ѡ���ʵ��
	int m_iTagParam;			//���ӵ�ѡ��������
	GEPOINT m_ptPick;			//ʰȡ������
	tagCAD_SCREEN_ENT(){memset(this,0,sizeof(tagCAD_SCREEN_ENT));}
}CAD_SCREEN_ENT;
class CCADCallBackDlg : public CDialog
{
// Construction
protected:
	BOOL (*FireCallBackFunc)(CDialog *pDlg);
public:
	CCADCallBackDlg(UINT nIDTemplate,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCADCallBackDlg)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCADCallBackDlg)
	public:
#if defined(_WIN64)
	virtual INT_PTR DoModal();
#else
	virtual int DoModal();
#endif
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCADCallBackDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	long m_iBreakType;			//�ж����� 1.ʰȡ�� 2.ʰȡֱ�� 3.ִ������
	BOOL m_bInernalStart;		//�ڲ����������Ի���
	BOOL m_bPauseBreakExit;		//�ڲ�ʰȡ����ʱ��ʱ�ж��˳��Ի���
public:
	BOOL m_bFireListItemChange;	//�Ƿ���ӦListCtrl��ItemChange�¼�,һ����OnInitDialog������Insert�����ĸ��¼�����Ӧ
	int m_nPickEntType;			//0.�� 1.���ݵ��Ӧ�Ľڵ� 2.�˼����� 3.����ʵ�� wht 11-06-29
	int m_nResultEnt;			//��Ҫѡ���ʵ�����
	f3dPoint m_ptPick;			//����ʰȡ������
	ATOM_LIST<CAD_SCREEN_ENT> resultList;	//ѡ��
	CStringArray m_arrCmdPickPrompt;//ʰȡ����ʱ�����е���ʾ�ַ���
	void SetCallBackFunc(BOOL (*func)(CDialog *pDlg)){FireCallBackFunc = func;}
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CADCALLBACKDLG_H__F07AFCF4_8860_4C20_A03A_E723AB08E4B6__INCLUDED_)
