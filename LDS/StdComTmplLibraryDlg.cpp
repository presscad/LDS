// StdComTmplLibraryDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "afxdialogex.h"
#include "env_def.h"
#include "StdComTmplLibraryDlg.h"

///////////////////////////////////////////////
// CStdComTmplLibraryDlg 对话框

IMPLEMENT_DYNAMIC(CStdComTmplLibraryDlg, CDialogEx)

CStdComTmplLibraryDlg::CStdComTmplLibraryDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStdComTmplLibraryDlg::IDD, pParent)
{
	m_iCurrStdTmplGroup=0;
}

CStdComTmplLibraryDlg::~CStdComTmplLibraryDlg()
{
}

void CStdComTmplLibraryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_COM_TMPL_GROUP, m_xStdTmplGroup);
}


BEGIN_MESSAGE_MAP(CStdComTmplLibraryDlg, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_COM_TMPL_GROUP, &OnSelchangeTabComTmplGroup)
	ON_BN_CLICKED(IDOK, &CStdComTmplLibraryDlg::OnBnOk)
	ON_BN_CLICKED(IDCANCEL, &CStdComTmplLibraryDlg::OnBnCancel)
END_MESSAGE_MAP()


// CStdComTmplLibraryDlg 消息处理程序

BOOL CStdComTmplLibraryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CStdComTempl::szImgRootPath.Printf("%simages\\stdtmpl_images\\",theApp.execute_path);
	// TODO:  在此添加额外的初始化
	m_xStdTmplGroup.DeleteAllItems();
	bool blAppendStruPage=false,blAppendNodePage=false,blAppendHeadArmFramePage=false;
	if(globalStdComTmplLibrary.xStruCatlog.MoveSonItemFirst()!=NULL)
	{
		m_xStdTmplGroup.InsertItem(PANEL_STRUCTURE,"结构部件");
		strupage.Create(IDD_STD_COM_TMPL_STRUCTURE_DLG,GetDlgItem(IDC_TAB_COM_TMPL_GROUP));
		blAppendStruPage=true;
	}
	if(globalStdComTmplLibrary.xNodeCatlog.MoveSonItemFirst()!=NULL)
	{
		m_xStdTmplGroup.InsertItem(PANEL_CONN_NODE,"节点构造");
		nodepage.Create(IDD_STD_COM_TMPL_CONN_NODE_DLG,GetDlgItem(IDC_TAB_COM_TMPL_GROUP));
		blAppendHeadArmFramePage=true;
	}
#ifdef _DEBUG
	/*if(PRODUCT_FUNC::IsHasInternalTest())
	{
		m_xStdTmplGroup.InsertItem(PANEL_FRAMELINE,"塔头横担");
		strupage.Create(IDD_STD_COM_TMPL_STRUCTURE_DLG,GetDlgItem(IDC_TAB_COM_TMPL_GROUP));
		blAppendStruPage=true;
	}*/
#endif
	CalclayoutPropPages();
	if(blAppendStruPage)
		strupage.ShowWindow(SW_SHOW);	//默认显示结构组件模板页
	if(blAppendNodePage&&!blAppendStruPage)
		nodepage.ShowWindow(SW_SHOW);	//默认显示节点组件模板页
	else
		nodepage.ShowWindow(SW_HIDE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
UINT CStdComTmplLibraryDlg::GetCurrSelStdComTmplId()
{
	if(m_iCurrStdTmplGroup==PANEL_STRUCTURE)
		return strupage.GetCurrSelStdComTmplId();
	else if(m_iCurrStdTmplGroup==PANEL_CONN_NODE)
		return nodepage.GetCurrSelStdComTmplId();
	else
		return 0;
}

void CStdComTmplLibraryDlg::CalclayoutPropPages()
{
	RECT rcPanel,rcHeaderBtn;
	m_xStdTmplGroup.GetItemRect(0,&rcHeaderBtn);
	m_xStdTmplGroup.GetClientRect(&rcPanel);//获取picture  control控件的区域
	//rcPanel.left=0;
	rcPanel.top=rcHeaderBtn.bottom+2;	//下移开标签按钮
	if(strupage.GetSafeHwnd())
		strupage.MoveWindow(&rcPanel);
	if(nodepage.GetSafeHwnd())
		nodepage.MoveWindow(&rcPanel);
}

void CStdComTmplLibraryDlg::OnSelchangeTabComTmplGroup(NMHDR *pNMHDR, LRESULT *pResult)
{
	this->m_iCurrStdTmplGroup = this->m_xStdTmplGroup.GetCurSel();
	if(m_iCurrStdTmplGroup==CStdComTmplLibraryDlg::PANEL_CONN_NODE)
	{
		if(strupage.GetSafeHwnd())
			strupage.ShowWindow(SW_HIDE);
		if(nodepage.GetSafeHwnd())
			nodepage.ShowWindow(SW_SHOW);
	}
	else if(m_iCurrStdTmplGroup==CStdComTmplLibraryDlg::PANEL_STRUCTURE)
	{
		if(strupage.GetSafeHwnd())
			strupage.ShowWindow(SW_SHOW);
		if(nodepage.GetSafeHwnd())
			nodepage.ShowWindow(SW_HIDE);
	}
	//CPaintDC dc(this);
	//m_propList.m_iPropGroup=m_iTemplateGroup;
	//m_propList.Redraw();
	//m_uIconWidth=m_dwIconWidth;
	//m_uIconHeight=m_dwIconHeight;
	//bitmap.ReInitDC(dc.GetSafeHdc(),m_uIconWidth,m_uIconHeight);
	//UpdateFaceSet();
	*pResult = 0;
}


void CStdComTmplLibraryDlg::OnOK(){
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}
void CStdComTmplLibraryDlg::OnCancel(){
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}
void CStdComTmplLibraryDlg::OnBnOk()
{
	OnOK();
}


void CStdComTmplLibraryDlg::OnBnCancel()
{
	OnCancel();
}
