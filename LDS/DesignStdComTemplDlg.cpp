#include "stdafx.h"
#include "Lds.h"
#include "GlobalFunc.h"
#include "PropertyListOper.h"
#include "DesignStdComTemplDlg.h"
#include "btc.h"

#ifdef __COMMON_PART_INC_
//////////////////////////////////////////////////////////////////////////
//回调函数处理
//////////////////////////////////////////////////////////////////////////
// CDesWireNodeDlg 对话框
IMPLEMENT_DYNAMIC(CDesignStdComTemplDlg, CDialogEx)
int CDesignStdComTemplDlg::nScrLocationX=0;
int CDesignStdComTemplDlg::nScrLocationY=0;
CDesignStdComTemplDlg::CDesignStdComTemplDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDesignStdComTemplDlg::IDD, pParent)
{
	m_idStdComTmpl=0;
}

CDesignStdComTemplDlg::~CDesignStdComTemplDlg()
{
}

void CDesignStdComTemplDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX_SUB,m_propSubList);
}


BEGIN_MESSAGE_MAP(CDesignStdComTemplDlg, CDialog)
	ON_WM_MOVE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CDesWireNodeDlg 消息处理程序
BOOL CDesignStdComTemplDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_MAP);
	pMainPropList->SetSketchImgPanel(pPic);
	pMainPropList->SetSketchBitmap(&m_xComSketchBmp);
	pMainPropList->SetBelongModel(&Ta);
	pMainPropList->InitializeUI(m_bInernalStart==TRUE,m_idStdComTmpl);
	CXhChar50 title("创建[%s]",pMainPropList->pStdTempl->GetName());
	SetWindowText(title);
	if(m_bInernalStart)
	{	//内部启动
		_InternalGetMainPropList()->pStdTempl->EnableIntelliRecogRodNodes(true);
		pMainPropList->FinishSelectObjOper(m_hPickObj,GetEventPropId());
		pMainPropList->DisplayPropertyList();
		CPropTreeItem *pItem=pMainPropList->FindItemByPropId(m_idEventProp,NULL);
		pMainPropList->SetFocus();	//属性栏获得焦点
		if(pItem)
			pMainPropList->SetCurSel(pItem->m_iIndex);	//设置选中项
	}
	else
	{
		pMainPropList->pStdTempl->ClearTmplParams();
		pMainPropList->AttachSubPropList(&m_propSubList);
		pMainPropList->SetDividerScale(0.5);
		m_propSubList.SetDividerScale(0.5);
		pMainPropList->m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
		m_propSubList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
		pMainPropList->DisplayPropertyList();
	}
	m_propSubList.DisplayPropertyList();
	//移动对话框到左上角
	CRect rect;
	GetWindowRect(rect);
	int width=rect.Width();
	int height=rect.Height();
	rect.left = nScrLocationX;
	rect.top = nScrLocationY;
	rect.right = rect.left+width;
	rect.bottom = rect.top+height;
	MoveWindow(rect,TRUE);
	m_bEnableWindowMoveListen=TRUE;
	UpdateData(FALSE);
	return TRUE;
}
int CDesignStdComTemplDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bEnableWindowMoveListen=FALSE;
	if (CCallBackDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
void CDesignStdComTemplDlg::OnMove(int x, int y)
{
	CCallBackDialog::OnMove(x, y);
	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}

void CDesignStdComTemplDlg::OnOK()
{
	if(!this->pMainPropList->CreateComTemplInstance())
		return;
	pMainPropList->ConfirmOK();
	return CDialog::OnOK();
}
void CDesignStdComTemplDlg::OnCancel()
{
	pMainPropList->CancelUI();
	return CDialog::OnCancel();
}
////////////////////////////////////////////////////////////////////////////////////////////////
//CDesignWireStruTemplDlgEx 走道模板实例类定义
CDesignWireStruTemplDlgEx::CDesignWireStruTemplDlgEx()
{
	m_xTemplUI.SetBelongModel(&Ta);
}
BOOL CDesignWireStruTemplDlgEx::OnInitDialog()
{
	BOOL status=CDesignStdComTemplDlg::OnInitDialog();
	//TODO:添加定制代码
	return status;
}

void CDesignWireStruTemplDlgEx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX, m_xTemplUI);
	DDX_Control(pDX, IDC_LIST_BOX_SUB,m_propSubList);
}
void CDesignWireStruTemplDlgEx::OnOK()
{
	CWaitCursor wait_cursor;
	if(!m_xTemplUI.CreateComTemplInstance())
		return;
	this->m_xTemplUI.ConfirmOK();
	return CDialog::OnOK();
}
void CDesignWireStruTemplDlgEx::OnCancel()
{
	m_xTemplUI.CancelUI();
	return CDialog::OnCancel();
}
#endif
#ifdef __COMPLEX_PART_INC_
////////////////////////////////////////////////////////////////////////////////////////////////
//CDesignTubeWalkWayDlgEx 走道模板实例类定义
CDesignTubeWalkWayDlgEx::CDesignTubeWalkWayDlgEx()
{
	m_xTemplUI.SetBelongModel(&Ta);
}
CLDSNode* CDesignTubeWalkWayDlgEx::set_m_pBaseNode(CLDSNode* value)
{
	if (value)
		m_xTemplUI.m_hBaseNode=value->handle;
	return m_xTemplUI.params.m_pBaseNode=value;
}
CLDSLineTube* CDesignTubeWalkWayDlgEx::set_m_pBaseTube(CLDSLineTube* value)
{
	if (value)
		m_xTemplUI.m_hBaseTube=value->handle;
	return m_xTemplUI.params.m_pBaseTube=value;
}

BOOL CDesignTubeWalkWayDlgEx::OnInitDialog()
{
	BOOL status=CDesignStdComTemplDlg::OnInitDialog();
	SetWindowText("创建钢管走道");
	//TODO:添加定制代码
	return status;
}

void CDesignTubeWalkWayDlgEx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX, m_xTemplUI);
	DDX_Control(pDX, IDC_LIST_BOX_SUB,m_propSubList);
}

void CDesignTubeWalkWayDlgEx::OnOK()
{
	if(!m_xTemplUI.CreateComTemplInstance())
		return;
	this->m_xTemplUI.ConfirmOK();
	return CDialog::OnOK();
}
void CDesignTubeWalkWayDlgEx::OnCancel()
{
	m_xTemplUI.CancelUI();
	return CDialog::OnCancel();
}
//////////////////////////////////////////////////////////////////////////
//CDesignSafeHangStruTemplDlgEx
CDesignTubeSafeHolderDlgEx::CDesignTubeSafeHolderDlgEx()
{
	m_xTemplUI.SetBelongModel(&Ta);
}
void CDesignTubeSafeHolderDlgEx::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_LIST_BOX, m_xTemplUI);
	DDX_Control(pDX, IDC_LIST_BOX_SUB,m_propSubList);
}
BOOL CDesignTubeSafeHolderDlgEx::OnInitDialog()
{
	BOOL status=CDesignStdComTemplDlg::OnInitDialog();
	SetWindowText("创建安全挂点支架");
	//TODO:添加定制代码
	return status;
}
void CDesignTubeSafeHolderDlgEx::OnOK()
{
	if(!m_xTemplUI.CreateComTemplInstance())
		return;
	this->m_xTemplUI.ConfirmOK();
	return CDialog::OnOK();
}
void CDesignTubeSafeHolderDlgEx::OnCancel()
{
	m_xTemplUI.CancelUI();
	return CDialog::OnCancel();
}
#endif