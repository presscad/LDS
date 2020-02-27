#include "supergridctrl.h"
#include "XhListCtrl.h"
#pragma once


// CRetrieveSegIDlg �Ի���

class CRetrieveSegIDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRetrieveSegIDlg)
public:
	 CXhListCtrl m_xListCtrl;
     PARTSET m_partSet;
	 int     m_Segi;
public:
	CRetrieveSegIDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRetrieveSegIDlg();
	CHashSet<CLDSPart*>m_hashDatumPartByHandle ;
// �Ի�������
	enum { IDD = IDD_RETRIEVE_SEG_I_DLG };
public:
	 virtual BOOL OnInitDialog();
	 void AdjustInsertPartOrFLSegI(CLDSParamPlate* pParamPlate,SEGI segI);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnAdjustSegI();
};
