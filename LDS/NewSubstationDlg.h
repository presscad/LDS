#pragma once
#include "SubstationGeneralPage.h"
#include "SubstationBeamPage.h"
#include "SubstationPostPage.h"
#include "SubstationSquareBeamPage.h"
#include "SubstationSegmentPage.h"
#include "LinkLabel.h"
#include "Block.h"
#include "afxwin.h"

// CNewSubstationDlg �Ի���

class CNewSubstationDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNewSubstationDlg)
	CSubstationGeneralPage pageGeneral;
	CSubstationBeamPage pageBeam;
	CSubstationSquareBeamPage pageSquareBeam;	
	CSubstationPostPage pagePost;
	CSubstationBeamSegmentPage pageSegment;
	CBrush			m_Brush;			// background brush
	BOOL FillSummarizeParam();
	BOOL FillPostParam();
	BOOL FillBeamParam();
public:
	CNewSubstationDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNewSubstationDlg();
	BOOL m_bWizardDialog;	//TRUE:���򵼶Ի���ģʽ����
	// modal processing
	virtual INT_PTR DoModal();
// �Ի�������
	enum { IDD = IDD_SUBSTATION_DLG };
public:
	CLinkLabel m_labelGeneral;
	CLinkLabel m_labelPost;
	CLinkLabel m_labelBeam;
	CLinkLabel m_labelSegment;
	int m_iActivePage;
	CParaBlockModel* m_pModel;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bDesignPost;
	BOOL m_bDesignBeam;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClickedLabelPost();
	afx_msg void OnClickedLabelBeam();
	afx_msg void OnClickedLabelGeneral();
	afx_msg void OnClickedLabelSegment();
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnPaint();
};
