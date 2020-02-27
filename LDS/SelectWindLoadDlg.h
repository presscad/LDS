#if !defined(__SELECT_WIND_LOAD_DLG_)
#define __SELECT_WIND_LOAD_DLG_

#pragma once

#include "Tower.h"
#include "afxwin.h"
#include "PropertyList.h"
// CSelectWindLoadDlg 对话框

class CSelectWindLoadDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectWindLoadDlg)
public:
	CXhPtrSet<CWindSegment> m_selectList;
	CHashStrList<BOOL>	m_checkList;
	CPropertyList m_propList;
	CWindSegment m_xDefaultWindSeg;
public:
	CSelectWindLoadDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSelectWindLoadDlg();
	//
	void DisplayWindLoadProperty();
// 对话框数据
	enum { IDD = IDD_SELECT_WIND_LOAD_DLG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
};
#endif