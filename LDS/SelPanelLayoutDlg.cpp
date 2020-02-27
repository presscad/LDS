//<LOCALE_TRANSLATE Confirm by hxr/>
// SelPanelLayoutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "SelPanelLayoutDlg.h"
#include "StdFacePanel.h"
#include "comdef.h"
#include "Expression.h"
//#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelPanelLayoutDlg dialog


CSelPanelLayoutDlg::CSelPanelLayoutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelPanelLayoutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelPanelLayoutDlg)
	m_bShankPanel = FALSE;
	m_bKPanel = FALSE;
	//}}AFX_DATA_INIT
	m_iPanelType=0;	//默认为塔身标准面
	m_bFirstPopup=TRUE;
	m_uIconWidth=m_uIconHeight=m_dwIconWidth=m_dwIconHeight=100;
}


void CSelPanelLayoutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelPanelLayoutDlg)
	DDX_Check(pDX, IDC_CHK_SHANK_PANEL, m_bShankPanel);
	DDX_Check(pDX, IDC_CHK_KFACE_PANEL, m_bKPanel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelPanelLayoutDlg, CDialog)
	//{{AFX_MSG_MAP(CSelPanelLayoutDlg)
	ON_BN_CLICKED(IDC_CHK_SHANK_PANEL, OnChkShankPanel)
	ON_BN_CLICKED(IDC_CHK_KFACE_PANEL, OnChkKfacePanel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CTRL_STD_FACE, OnDblclkListCtrlStdFace)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelPanelLayoutDlg message handlers

BOOL CSelPanelLayoutDlg::OnInitDialog() 
{
	if(m_iPanelType==1)
	{
		m_bShankPanel=TRUE;
		m_bKPanel=FALSE;
	}
	else
		m_bShankPanel=m_bKPanel=FALSE;
	if(m_iPanelType==6)
		m_bLFrontPanel=TRUE;
	CDialog::OnInitDialog();
	
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_CTRL_STD_FACE);
	m_pListCtrl->SetExtendedStyle( LVS_ICON);// | LVS_EX_REGIONAL );
	m_pListCtrl->SetBkColor(RGB(255,255,255));
	CPaintDC dc(this);
	m_uIconWidth=m_dwIconWidth;
	m_uIconHeight=m_dwIconHeight;
	if(m_bFirstPopup)
	{
		bitmap.InitDC(dc.GetSafeHdc(),m_uIconWidth,m_uIconHeight);
		imageList.Create(m_uIconWidth, m_uIconHeight, ILC_COLOR32,   2, 4);
	}
	else
	{
		bitmap.ReInitDC(dc.GetSafeHdc(),m_uIconWidth,m_uIconHeight);
		imageList.DeleteImageList();
		imageList.Create(m_uIconWidth, m_uIconHeight, ILC_COLOR32,   2, 4);
	}
	m_pListCtrl->SetImageList(&imageList,LVSIL_NORMAL);
	if(m_bShankPanel||m_bKPanel)
	{
		GetDlgItem(IDC_CHK_SHANK_PANEL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHK_KFACE_PANEL)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_CHK_SHANK_PANEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHK_KFACE_PANEL)->ShowWindow(SW_HIDE);
	}
	//初始化窗口标准面列表控件大小
	/*CRect rc;
	int scr_cy,increY;
	GetWindowRect(rc);
	ScreenToClient(rc);
	scr_cy=GetSystemMetrics(SM_CYMAXIMIZED);
	increY=scr_cy-rc.Height()-10;
	rc.bottom+=increY;
	rc.top+=5;
	MoveWindow(rc);
	m_pListCtrl->GetWindowRect(rc);
	ScreenToClient(rc);
	rc.bottom+=increY;
	m_pListCtrl->MoveWindow(rc);*/

	UpdateFaceSet();
	for(int i=0;i<m_pListCtrl->GetItemCount();i++)
	{
		CString sLabel=m_pListCtrl->GetItemText(i,0);
		if(sLabel.CompareNoCase(m_sFacePanel)!=0)
			continue;
		m_pListCtrl->SetItemState(i,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
		break;
	}
	m_bFirstPopup=FALSE;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CSelPanelLayoutDlg::AddFacePanel(CFacePanel &panel)
{
	CBitmap bmap;
	CPaintDC dc(this);
	CPen pen;
	DWORD dwCount=m_uIconWidth*m_uIconHeight*4;
	BYTE *data =new BYTE[dwCount];
	pen.CreatePen(PS_SOLID,1,RGB(0,255,0));
	bmap.CreateCompatibleBitmap(&dc,m_uIconWidth,m_uIconHeight);
	//if(m_iTemplateGroup>0)
	//	panel.m_iFaceType=m_iTemplateGroup-1;
	SCOPE_STRU scope;
	for(CFacePanelNode *pNode=panel.node_list.GetFirst();pNode;pNode=panel.node_list.GetNext())
	{
		if(panel.m_iFaceType!=1&&pNode->m_bInternodeRepeat)
			continue;
		scope.VerifyVertex(f3dPoint(pNode->pos.x,pNode->pos.y));
	}
	CPen *pOldPen=bitmap.GetDC()->SelectObject(&pen);
	bitmap.Clear();
	double scale=0.9*min(m_uIconWidth/scope.wide(),m_uIconHeight/scope.high());
	UCS_2dSTRU ucs;
	ucs.origin.Set((scope.fMinX+0.5*scope.wide()-0.5*m_dwIconWidth/scale),(scope.fMinY+0.5*scope.high()+0.5*m_dwIconHeight/scale));
	ucs.axis_x.Set(1,0);
	ucs.axis_y.Set(0,-1);
	CExpression expression;
	EXPRESSION_VAR *pVar=expression.varList.Append();
	strcpy(pVar->variableStr,"N");
	if(panel.m_iFaceType!=1)
		pVar->fValue=panel.m_nInternodeNum;
	else
		pVar->fValue=1;
	pVar=expression.varList.Append();
	strcpy(pVar->variableStr,"i");
	int i,repeatNum=1;
	if(panel.m_iFaceType!=1)
		repeatNum=panel.m_nInternodeNum-panel.m_nRepeatLessNum;
	for(i=0;i<repeatNum;i++)
	{
		for(CFacePanelPole *pPole=panel.pole_list.GetFirst();pPole;pPole=panel.pole_list.GetNext())
		{
			if(i>0&&!pPole->m_bInternodeRepeat)
				continue;
			CFacePanelNode *pStart=&panel.node_list[pPole->start_i];
			CFacePanelNode *pEnd=&panel.node_list[pPole->end_i];
			if(pStart->m_bInternodeRepeat)
			{
				expression.VarAt(1)->fValue=i;//expression.varList[1].fValue=i;
				double scale=expression.SolveExpression(pStart->scale_expression);
				CFacePanelNode *pScaleStart=&panel.node_list[pStart->attach_node_index[0]];
				CFacePanelNode *pScaleEnd=&panel.node_list[pStart->attach_node_index[1]];
				pStart->pos=pScaleStart->pos+(pScaleEnd->pos-pScaleStart->pos)*scale;
			}
			f2dPoint start_pos=pStart->pos;
			if(pEnd->m_bInternodeRepeat)
			{
				expression.VarAt(1)->fValue=i;//expression.varList[1].fValue=i;
				double scale=expression.SolveExpression(pEnd->scale_expression);
				CFacePanelNode *pScaleStart=&panel.node_list[pEnd->attach_node_index[0]];
				CFacePanelNode *pScaleEnd=&panel.node_list[pEnd->attach_node_index[1]];
				pEnd->pos=pScaleStart->pos+(pScaleEnd->pos-pScaleStart->pos)*scale;
			}
			f2dPoint end_pos=pEnd->pos;
			coord_trans(start_pos,ucs,FALSE);
			coord_trans(end_pos,ucs,FALSE);
			start_pos.x*=scale;
			start_pos.y*=scale;
			end_pos.x*=scale;
			end_pos.y*=scale;
			start_pos.y+=1;	//加1是为了在简图被选中时阴影显示时不全部被屏蔽掉
			end_pos.y+=1;	//加1是为了在简图被选中时阴影显示时不全部被屏蔽掉
			bitmap.GetDC()->MoveTo(ftoi(start_pos.x),ftoi(start_pos.y));
			bitmap.GetDC()->LineTo(ftoi(end_pos.x),ftoi(end_pos.y));
		}
	}
	BITMAP bm;
	bitmap.GetBitmap(&bm);
	bitmap.GetBitmapBits(dwCount,data);
	bmap.SetBitmapBits(dwCount,data);
	int face_i=imageList.Add(&bmap,RGB(0,0,0));
	bitmap.GetDC()->SelectObject(pOldPen);	//选回原画笔
	m_pListCtrl->InsertItem(m_pListCtrl->GetItemCount(),panel.sLabel,face_i);
	delete []data;
}
void CSelPanelLayoutDlg::UpdateFaceSet()
{
	CString Part;
	m_pListCtrl->DeleteAllItems();	//清空显示列表
	if(m_iPanelType==1)		//1.塔身标准面
#ifdef AFX_TARG_ENU_ENGLISH
		Part="tower body standard plane";
	else if(m_iPanelType==2)	//3:on behalf of horizon isolation plane;
		Part="horizon isolation standard plane";
	else if(m_iPanelType==4)	//4:leg K plane
		Part="K-type standard plane";
	else if(m_iPanelType==5)	//5:V plane
		Part="V-type standard plane";
#else
		Part="塔身标准面";
	else if(m_iPanelType==2)	//3:表示横隔面;
		Part="横隔标准面";
	else if(m_iPanelType==4)	//4:腿部K面
		Part="K型标准面";
	else if(m_iPanelType==5)	//5:V面
		Part="V型标准面";
	else if(m_iPanelType==6)	//6:V面
		Part="接腿正侧面";
#endif
	else
		return;
	IStorage *pRootStg;
	DWORD grfMode = STGM_READ | STGM_SHARE_EXCLUSIVE;
	char file_path[MAX_PATH];
	strcpy(file_path,theApp.execute_path);
	//GetSysPath(file_path);
	strcat(file_path,"facepanel.pns");
	HRESULT hr = ::StgOpenStorage(_bstr_t(file_path), NULL,grfMode,NULL, 0, &pRootStg);
	if(pRootStg==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Fail to open default standard plane file!");
#else
		AfxMessageBox("打不开默认标准面文件！");
#endif
		return;
	}
	IStream *pStream;
	IStorage *pStg;
	m_uIconWidth=m_dwIconWidth;
	m_uIconHeight=m_dwIconHeight;
	pRootStg->OpenStorage(_bstr_t(Part),NULL,grfMode,NULL,0,&pStg);
	if(pStg==NULL)
	{
		pRootStg->Release();
		return;
		//pRootStg->CreateStorage(_bstr_t(Part),grfMode|STGM_CREATE,0,0,&pStg);
	}

	imageList.DeleteImageList();
	imageList.Create(m_uIconWidth, m_uIconHeight, ILC_COLOR32,2,4);
	m_pListCtrl->SetImageList(&imageList,LVSIL_NORMAL);
	IEnumSTATSTG *pFaceEnum;
	STATSTG face_statstg;
	pStg->EnumElements(0,NULL,0,&pFaceEnum);
	while(pFaceEnum->Next(1, &face_statstg, NULL) == S_OK)
	{
		if(face_statstg.type==STGTY_STREAM)
		{
			CString face_title=(char*)_bstr_t(face_statstg.pwcsName);
			HRESULT hr=pStg->OpenStream(face_statstg.pwcsName,NULL,grfMode,0,&pStream);
			CFacePanel panel;
			sprintf(panel.sLabel,"%s",face_title);
			panel.FromStream(pStream);
			AddFacePanel(panel);
			pStream->Release();
		}
	}
	pFaceEnum->Release();
	pStg->Release();
	pRootStg->Release();
	m_pListCtrl->Arrange(LVA_ALIGNLEFT);
}

void CSelPanelLayoutDlg::OnChkShankPanel() 
{
	UpdateData();
	m_bKPanel=!m_bShankPanel;
	UpdateData(FALSE);
	if(m_bKPanel)
		m_iPanelType=4;
	else
		m_iPanelType=1;
	UpdateFaceSet();
}

void CSelPanelLayoutDlg::OnChkKfacePanel() 
{
	UpdateData();
	m_bShankPanel=!m_bKPanel;
	UpdateData(FALSE);
	if(m_bKPanel)
		m_iPanelType=4;
	else
		m_iPanelType=1;
	UpdateFaceSet();
}

void CSelPanelLayoutDlg::OnDblclkListCtrlStdFace(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	*pResult = 0;
}

void CSelPanelLayoutDlg::OnOK() 
{
	int iSel=-1;
	POSITION pos=m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos)
		iSel=m_pListCtrl->GetNextSelectedItem(pos);
	if(iSel>=0)
		m_sFacePanel=m_pListCtrl->GetItemText(iSel,0);
	else
		m_sFacePanel="";
	
	CDialog::OnOK();
}
