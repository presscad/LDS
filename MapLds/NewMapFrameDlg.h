#pragma once

#include "resource.h"

// CNewMapFrameDlg 对话框
#include "SysPara.h"

class CNewMapFrameDlg : public CDialog
{
	CComboBox *m_pMapFrmNameCmbBox;
public:
	CNewMapFrameDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNewMapFrameDlg();
	enum { IDD = IDD_NEW_MAP_FRAME_DLG };
// 对话框数据

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	void RefreshMapFrmData();

	DECLARE_MESSAGE_MAP()
public:
	CString m_sMapFrmName;			//图框名称（新建图框）
	CString m_sCmbMapFrmName;		//图框名称（插入图框）
	int m_nLength;					//图框长度
	int m_nWidth;					//图框宽度
	int m_nLeftMargin;				//左侧页边间隙
	int m_nOthersMargin;			//其它侧页边间隙
	int m_nBottomRightFixLen;		//图框右下侧固定长度
	int m_nBottomRightFixWidth;		//图框右下侧固定宽度

	MAP_FRAME_SIZE m_xMapFrm;
	BOOL m_bNewMapFrm;				//TRUE:	新建图框；FALSE:插入图框；
	afx_msg void OnCbnSelchangeCmbMapFrameName();
};
