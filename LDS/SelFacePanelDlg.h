#if !defined(AFX_SELFACEPANELDLG_H__FF311897_D9DE_42CF_8D9D_541EBFDE555B__INCLUDED_)
#define AFX_SELFACEPANELDLG_H__FF311897_D9DE_42CF_8D9D_541EBFDE555B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelFacePanelDlg.h : header file
//
#include "StdFacePanel.h"
#include "BitmapGraph.h"
#include "PropertyList.h"
#include "RememberLocationDlg.h"
//#include "DrawEngine.h"
/////////////////////////////////////////////////////////////////////////////
// CSelFacePanelDlg dialog

class CSelFacePanelDlg : public CRememberLocationDlg
{
	static int m_iSelItemIndex;
	BOOL m_bFirstPopup;
	UINT m_uIconWidth,m_uIconHeight;	//��ǰ��ʾģ��ʾ��ͼ��Ĵ�С
	UINT m_dwIconWidth,m_dwIconHeight;
	BOOL m_bTracking;
	CListCtrl* m_pListCtrl;
	CImageList imageList;
	CBitmapGraph bitmap;
	CString	m_sKeyNodeOrder;
	//CDrawEngine *m_pDraw;
// ��ͼ������
	UCS_STRU rgn_ucs;	//�ɻ�ͼ��ת�����Ի���ͻ�������
	CRect m_rectInitDrawRgn;	//��ǰ�Ի���ĳ�ʼ���Σ���Ļ���꣩
	CRect m_rectInitWndRgn;		//��ǰ�Ի���ĳ�ʼ���Σ���Ļ���꣩
	double m_fScaleScreenToUser;
	HCURSOR m_hOldCursor,m_hCursor;
	CPen solid_pen,green_solid_pen,green_dash_pen,red_solid_pen,wide_solid_pen,dash_pen,dash_dot_pen;
private:
	CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	void InitPropHashtable();
	PROPLIST_ITEM* AddPropItem(const char* propKey,PROPLIST_ITEM item);//������������º���
public:
	long GetPropID(char* propStr);	//�õ�����ID
	int GetPropValueStr(int idProp,char* valueStr);
private:
	void UpdateFaceDetailGraph();//��ͼ������ʾ��ͼ
	void DisplayPropertyList();
	void AddHeadTemplate(CParaTemplate &headTemplate);
	void AddFacePanel(CFacePanel &panel);
	void UpdateFaceSet();
	BOOL ReadCursorHeadTemplate(CParaTemplate& headTemplate,BOOL bPromptError);
	BOOL ReadCursorFacePanel(CFacePanel& panel,BOOL bPromptError);
	void RefreshTabCtrl(int iCurSel);
// Construction
public:
	long m_nInternodeNum;
	int m_iTemplateGroup;
	CFacePanel m_xFacePanel;
	CParaTemplate m_xHeadTemplate;
	GEPOINT m_vAcsAxisX;
	char m_cThirdLayer;
	long	m_nKeyNodeNum;
	int		m_iKeyNodeMir;		//0.�޶Գƣ�1.X��Գƣ�2.Y��Գƣ�3.XYZ�Գ�
	int		m_iStdFaceMir;		//0.�޶Գƣ�1.X��Գƣ�2.Y��Գƣ�3.Z��Գƣ�4.����5.Z�Գ�&����6.�ǶԳ�
	CSelFacePanelDlg(CWnd* pParent = NULL);   // standard constructor
	void SetCurrWorkView(CDisplayView *pView);
	void ChangeKeyNodeNum();
// Dialog Data
	//{{AFX_DATA(CSelFacePanelDlg)
	enum { IDD = IDD_SEL_FACE_PANEL_DLG };
	CTabCtrl	m_ctrlPropGroup;
	CPropertyList	m_propList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelFacePanelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelFacePanelDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnItemchangedListCtrlStdFace(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELFACEPANELDLG_H__FF311897_D9DE_42CF_8D9D_541EBFDE555B__INCLUDED_)
