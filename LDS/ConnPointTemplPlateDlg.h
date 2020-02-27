#pragma once
#include "afxcmn.h"
#include "resource.h"
#include ".\StdComTempl\IStdComTempl.h"
#include "PropertyList.h"

// CConnPointTemplPlateDlg 对话框
class CConnPointTemplPlateDlg : public CDialogEx
{
	int m_dwIconWidth,m_dwIconHeight;
	CImageList imageList;
	CStdComTemplConnPoint* m_pStdConnPoint;
	CBitmap m_xSketchImg;	//存储当前选中模板的示意图
	CRect m_rectInitDrawRgn;
	CRect m_rectInitWndRgn;
public:
	UINT m_idTemplPlate;
	RECT m_rcClient;
	int m_nOldHorzY;
	HICON m_hCursorArrow;
	HICON m_hCursorSize;
	BOOL m_bTracking;
	int m_nSplitterWidth;
	CStringArray m_sTabLabelArr;
public:
	CConnPointTemplPlateDlg(CWnd* pParent = NULL);
	virtual ~CConnPointTemplPlateDlg();
	//
	void RefreshImageLictCtr();
	void SetCurrConnPoint(CStdComTemplConnPoint* pStdConnPoint);
	void DisplayParaProperty();
	void InvertLine(CDC* pDC,CPoint ptFrom,CPoint ptTo);
	CPropertyList *GetPropertyList(){return &m_xPropList;}
	void RelayoutWnd();
// 对话框数据
	enum { IDD = IDD_CONN_POINT_TEMPL_PLATE_DLG };
	CPropertyList m_xPropList;
	CListCtrl m_xIconListCtrl;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnByGeneralPolicy();
	afx_msg void OnLvnItemchangedListCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnClose();
};
