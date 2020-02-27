#pragma once
#include "propertylist.h"
#include "xhlistctrl.h"
#include "HashTable.h"
#include "CallBackDialog.h"
#include "NativePropertyList.h"
#ifdef __COMMON_PART_INC_

// CSectionViewPropertyDlg �Ի���

class CSectionViewPropertyDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CSectionViewPropertyDlg)
	CHashList<CSectionView::PART_ITEM*> m_xPartItemHashHandle;
	CXhListCtrl m_xPartList;
	CSectionViewPropertyList m_xPropList;
public:
	CLDSPartGroup *m_pWorkPartGroup;
	CSectionView *m_pWorkView, *m_pSrcView;
	int m_iCurDesParaType;	//0.X 1.Y 2.Z 3.ԭ��
public:
	CSectionViewPropertyDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSectionViewPropertyDlg();

	void DisplaySectionViewProperty();
	void SelectPart(int nCurSel);
	void ContextMenu(CWnd* pWnd, CPoint point);
	void OnDrawBySelPart();
// �Ի�������
	enum { IDD = IDD_SECTION_VIEW_PROPERTY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnConfirm();
	afx_msg void OnNMClickListPart(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownListPart(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickListPart(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListPart(NMHDR *pNMHDR, LRESULT *pResult);
};
#endif