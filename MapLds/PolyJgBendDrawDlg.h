#pragma once


// CPolyJgBendDrawDlg �Ի���
#include "resource.h"
#include "lds_co_part.h"
#include "RxTools.h"
#include "XhListCtrl.h"

class CPolyJgBendDrawDlg : public CDialog
{
	DECLARE_DYNAMIC(CPolyJgBendDrawDlg)
private:
	POLYPARTLIST polyPartList;
	CStringKeyHashList<DWORD> m_polyHashByPartNo;
public:
	CPolyJgBendDrawDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPolyJgBendDrawDlg();
	//
	PARTSET partSet;
	void RefreshListCtr();
	void ConfirmCreate(bool bOnlySelecteItems);
// �Ի�������
	enum { IDD = IDD_POLY_JG_SET_DLG };
	CXhListCtrl m_xListCtrl;
	CString	m_sSegStr;
	CString	m_sPartNo;
	double	m_fDrawScale;
	double	m_fMinDist;
	CString m_sCreate;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedConfirm();
	afx_msg void OnBnRebuildPartSet();
	afx_msg void OnClickListCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
};
