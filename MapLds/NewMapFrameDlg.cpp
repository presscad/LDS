//<LOCALE_TRANSLATE/>
// NewMapFrameDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "NewMapFrameDlg.h"
#include "XhCharString.h"


// CNewMapFrameDlg 对话框

CNewMapFrameDlg::CNewMapFrameDlg(CWnd* pParent /*=NULL*/)
: CDialog(CNewMapFrameDlg::IDD, pParent)
{
	m_sCmbMapFrmName = _T("");
	m_pMapFrmNameCmbBox=NULL;
	m_sMapFrmName = _T("");
	m_nLength=0;
	m_nWidth=0;
	m_nLeftMargin=25;
	m_nOthersMargin=10;
	m_nBottomRightFixLen=0;
	m_nBottomRightFixWidth=0;
	m_xMapFrm.style=10;
	m_bNewMapFrm=TRUE;
}

CNewMapFrameDlg::~CNewMapFrameDlg()
{
}

void CNewMapFrameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_CMB_MAP_FRAME_NAME, m_sCmbMapFrmName);
	DDX_Text(pDX, IDC_E_MAP_FRAME_NAME, m_sMapFrmName);
	DDX_Text(pDX, IDC_E_MAP_FRAME_LENGTH, m_nLength);
	DDX_Text(pDX, IDC_E_MAP_FRAME_WIDTH, m_nWidth);
	DDX_Text(pDX, IDC_E_MAP_FRAME_LEFT_MARGIN, m_nLeftMargin);
	DDX_Text(pDX, IDC_E_MAP_FRAME_OTHERS_MARGIN, m_nOthersMargin);
	DDX_Text(pDX, IDC_E_MAP_FRAME_BOTTOM_RIGHT_FIX_LEN, m_nBottomRightFixLen);
	DDX_Text(pDX, IDC_E_MAP_FRAME_BOTTOM_RIGHT_FIX_WIDTH, m_nBottomRightFixWidth);
}

void CNewMapFrameDlg::RefreshMapFrmData()
{
	UpdateData();
	CXhChar100 sKey;
	sKey.Printf("%s",m_sCmbMapFrmName);
	MAP_FRAME_SIZE* pMapFrm=sys.m_hashMapFrameList.GetValue((char*)sKey);
	if (pMapFrm!=NULL)
	{
		m_nLength=pMapFrm->nLength;
		m_nWidth=pMapFrm->nWidth;
		m_nLeftMargin=pMapFrm->nLeftMargin;
		m_nOthersMargin=pMapFrm->nRightMargin;
		m_nBottomRightFixLen=pMapFrm->nBottomRightFixLen;
		m_nBottomRightFixWidth=pMapFrm->nBottomRightFixWidth;
	}
	UpdateData(FALSE);
}
BOOL CNewMapFrameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_pMapFrmNameCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_MAP_FRAME_NAME);
	ASSERT(m_pMapFrmNameCmbBox);
	if (!m_bNewMapFrm)
	{
		GetDlgItem(IDC_E_MAP_FRAME_NAME)->ShowWindow(SW_HIDE);
		//图框具体信息设为只读
		((CEdit*)GetDlgItem(IDC_E_MAP_FRAME_LENGTH))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_E_MAP_FRAME_WIDTH))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_E_MAP_FRAME_LEFT_MARGIN))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_E_MAP_FRAME_OTHERS_MARGIN))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_E_MAP_FRAME_BOTTOM_RIGHT_FIX_LEN))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_E_MAP_FRAME_BOTTOM_RIGHT_FIX_WIDTH))->SetReadOnly(TRUE);
		for(MAP_FRAME_SIZE* pMapFrame=sys.m_hashMapFrameList.GetFirst();pMapFrame;pMapFrame=sys.m_hashMapFrameList.GetNext())
			m_pMapFrmNameCmbBox->AddString((char*)pMapFrame->sMapFrameName);
		m_pMapFrmNameCmbBox->SetCurSel(0);
		RefreshMapFrmData();
	}
	else
		GetDlgItem(IDC_CMB_MAP_FRAME_NAME)->ShowWindow(SW_HIDE);

	if (m_bNewMapFrm)
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("new map frame");
	else
		SetWindowText("insert map frame");
#else
		SetWindowText("新建图框");
	else
		SetWindowText("插入图框");
#endif
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BEGIN_MESSAGE_MAP(CNewMapFrameDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_CMB_MAP_FRAME_NAME, OnCbnSelchangeCmbMapFrameName)
END_MESSAGE_MAP()

// CNewMapFrameDlg 消息处理程序

void CNewMapFrameDlg::OnOK()
{
	UpdateData();
	if (m_bNewMapFrm)
	{
		if (m_nLength<=0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("map frame length must be larger than 0!");
#else
			AfxMessageBox("图框长度必须大于0！");
#endif
			return;
		}
		if (m_nLength<=0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("map frame width must be larger than 0!");
#else
			AfxMessageBox("图框宽度必须大于0！");
#endif
			return;
		}
		if (m_sMapFrmName.CompareNoCase("")==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("map frame name cannot be empty!");
#else
			AfxMessageBox("图框名称不能为空！");
#endif
			return;
		}
		if (m_sMapFrmName.GetLength()>100)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("the number of map frame name characters have be more than 100!");
#else
			AfxMessageBox("图框名称超过100！");
#endif
			return;
		}
		CXhChar100 sKey;
		sKey.Printf("%s",m_sMapFrmName);
		if (sys.m_hashMapFrameList.GetValue((char*)sKey)!=NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("frame name already exists!");
#else
			AfxMessageBox("图框名称已存在！");
#endif
			return;
		}
		m_xMapFrm.sMapFrameName.Printf("%s",m_sMapFrmName);
		m_xMapFrm.nLength=m_nLength;
		m_xMapFrm.nWidth=m_nWidth;
		m_xMapFrm.nLeftMargin=m_nLeftMargin;
		m_xMapFrm.nRightMargin
			=m_xMapFrm.nTopMargin
			=m_xMapFrm.nBottomMargin=m_nOthersMargin;
		m_xMapFrm.nBottomRightFixLen=m_nBottomRightFixLen;
		m_xMapFrm.nBottomRightFixWidth=m_nBottomRightFixWidth;
	}
	CDialog::OnOK();
}

void CNewMapFrameDlg::OnCbnSelchangeCmbMapFrameName()
{
	RefreshMapFrmData();
}
