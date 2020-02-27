#pragma once
#ifdef __PART_DESIGN_INC_

#include "Tower.h"
#include "afxwin.h"
#include "PropertyList.h"
// CSelectWorkCaseDlg �Ի���
//////////////////////////////////////////
#if defined(__LDS_)||defined(__TSA_)
class CSelectWorkCaseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectWorkCaseDlg)
public:
	CXhPtrSet<CWorkStatus> m_selectList;
	CHashStrList<BOOL>	m_checkList;
	CPropertyList m_propList;
	CWorkStatus m_xDefaultWork;
public:
	CSelectWorkCaseDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSelectWorkCaseDlg();
	//
	void DisplayWorkCaseProperty();
	void SelchangeCmbMainLoadStyle(CPropTreeItem* pPropItem,bool initDefaultParams=true);
// �Ի�������
	enum { IDD = IDD_WORK_CASE_DLG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
};
#endif
#endif
