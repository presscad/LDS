//<LOCALE_TRANSLATE Confirm by hxr/>
// TowerPropertyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "TowerPropertyDlg.h"
#include "LDSView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTowerPropertyDlg dialog
IMPLEMENT_DYNCREATE(CTowerPropertyDlg, CDialog)

CTowerPropertyDlg::CTowerPropertyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTowerPropertyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTowerPropertyDlg)
	//}}AFX_DATA_INIT
	m_nSplitterWidth = 5;
	m_pAtom = NULL;
	m_bShowTopCMB=FALSE;
	m_nOldBtmHeight = 0;
	m_curClsTypeId=LDSCLASS_TYPEID(0);	//默认显示选中的所有构件的属性
}


void CTowerPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTowerPropertyDlg)
	DDX_Control(pDX, IDC_CMB_SELECT_OBJECT_INFO, m_cmbSelectObjInfo);
	DDX_Control(pDX, IDC_TAB_GROUP, m_ctrlPropGroup);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_S_PROP_HELP_PIC, m_propHelpWnd);
}


BEGIN_MESSAGE_MAP(CTowerPropertyDlg, CDialog)
	//{{AFX_MSG_MAP(CTowerPropertyDlg)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_GROUP, OnSelchangeTabGroup)
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_CMB_SELECT_OBJECT_INFO, OnSelchangeSelectObjectInfo)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTowerPropertyDlg message handlers
bool FireContextHelpPropList(CWnd* pWnd,CHashSet<PROPLIST_ITEM*>* pHashPropItemById,CPropTreeItem* pPropItem)
{
	CXhChar200 helpStr;
	PROPLIST_ITEM* pPropHelpItem=pHashPropItemById->GetValue(pPropItem->m_idProp);
	if(pPropHelpItem!=NULL)
		sprintf(helpStr,"%s::/%s",theApp.m_pszHelpFilePath,(char*)pPropHelpItem->Url);
	else
	{
		//sprintf(helpStr,"%s::/命令参考/属性栏.htm",theApp.m_pszHelpFilePath);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Lacking of the help top of this property,please contact the software vendor directly!");
#else
		AfxMessageBox("暂缺少该属性的帮助主题，请直接与软件供应商联系！");
#endif
	}
	pWnd->HtmlHelp((DWORD_PTR)(char*)helpStr,HH_DISPLAY_TOPIC);
	return true;
}
BOOL CTowerPropertyDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	CPropTreeItem* pPropItem=m_propList.GetSelectedItem();
	switch(m_propList.m_nObjClassTypeId)
	{
		case CLS_NODE:
			FireContextHelpPropList(this,&CLDSNode::hashPropItemById,pPropItem);
			break;
		case CLS_LINEPART:
			FireContextHelpPropList(this,&CLDSLinePart::hashPropItemById,pPropItem);
			break;
		case CLS_LINEANGLE:
			FireContextHelpPropList(this,&CLDSLineAngle::hashPropItemById,pPropItem);
			break;
		case CLS_PLATE:
			FireContextHelpPropList(this,&CLDSPlate::hashPropItemById,pPropItem);
			break;
		case CLS_SPHERE:
			FireContextHelpPropList(this,&CLDSSphere::hashPropItemById,pPropItem);
			break;
		case CLS_PARAMPLATE:
			FireContextHelpPropList(this,&CLDSParamPlate::hashPropItemById,pPropItem);
			break;
		case CLS_BOLT:
			FireContextHelpPropList(this,&CLDSBolt::hashPropItemById,pPropItem);
			break;
		case CLS_LINESLOT:
			FireContextHelpPropList(this,&CLDSLineSlot::hashPropItemById,pPropItem);
			break;
		case CLS_LINEFLAT:
			FireContextHelpPropList(this,&CLDSLineFlat::hashPropItemById,pPropItem);
			break;
		case CLS_FEMELEMENT:
			FireContextHelpPropList(this,&CElemInfo::hashPropItemById,pPropItem);
			break;
		case CLS_ARCANGLE:
			FireContextHelpPropList(this,&CLDSArcAngle::hashPropItemById,pPropItem);
			break;
		case CLS_ARCSLOT:
			FireContextHelpPropList(this,&CLDSArcSlot::hashPropItemById,pPropItem);
			break;
		case CLS_ARCFLAT:
			FireContextHelpPropList(this,&CLDSArcFlat::hashPropItemById,pPropItem);
			break;
		case CLS_POLYJOINT:
			FireContextHelpPropList(this,&CLDSPolyJoint::hashPropItemById,pPropItem);
			break;
		case CLS_BLOCK:
			FireContextHelpPropList(this,&CBlockModel::hashPropItemById,pPropItem);
			break;
		case CLS_BLOCKREF:
			FireContextHelpPropList(this,&CBlockReference::hashPropItemById,pPropItem);
			break;
		case CLS_PROFILEVERTEX:
			FireContextHelpPropList(this,&PROFILE_VERTEX::hashPropItemById,pPropItem);
			break;
	    default:
			FireContextHelpPropList(this,&CLDSApp::hashPropItemById,pPropItem);
			break;
	}
	return TRUE;	
}

BOOL CTowerPropertyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
#ifdef __INTERNAL_VERSION
	m_bShowTopCMB=TRUE;
	m_cmbSelectObjInfo.SetCurSel(0);
#endif
	RECT rc,rcHeader,rcTop,rcBottom,rcCMB;
	GetClientRect(&rc);
	m_rcClient = rc;
	CWnd *pTopWnd = GetDlgItem(IDC_LIST_BOX);
	CWnd *pBtmWnd = GetDlgItem(IDC_S_PROP_HELP_PIC);	//IDC_E_PROP_HELP_STR
	CWnd *pCmbWnd = GetDlgItem(IDC_CMB_SELECT_OBJECT_INFO);
	if(pTopWnd)
		pTopWnd->GetWindowRect(&rcTop);
	if(pBtmWnd)
		pBtmWnd->GetWindowRect(&rcBottom);
	if(pCmbWnd)
		pCmbWnd->GetWindowRect(&rcCMB);
	if(!m_bShowTopCMB)
	{	//不显示顶部下拉框
		if(pCmbWnd)
			pCmbWnd->ShowWindow(SW_HIDE);
		rcCMB.top=rcCMB.bottom=rcCMB.left=rcCMB.right=0;
	}
	else 
	{
		if(pCmbWnd)
			pCmbWnd->ShowWindow(SW_SHOW);
		/*int cmbHeight = rcCMB.bottom - rcCMB.top;
		int cmbWidth  = rcCMB.right - rcCMB.left;
		rcCMB.left=rcCMB.top=0;
		rcCMB.right=cmbWidth;
		rcCMB.bottom=cmbHeight;
		rcCMB.bottom+=4;
		if(pCmbWnd)
		pCmbWnd->MoveWindow(&rcCMB);*/
		rcCMB.bottom+=4;
	}
	m_propList.m_hPromptWnd = pBtmWnd->GetSafeHwnd();
	ScreenToClient(&rcTop);
	ScreenToClient(&rcBottom);
	if(m_bShowTopCMB)
		ScreenToClient(&rcCMB);
	int btmHeight = rcBottom.bottom - rcBottom.top;
	rcHeader.left = rcTop.left = rcBottom.left = 0;
	rcHeader.right = rcTop.right = rcBottom.right = rc.right;
	rcHeader.top=rcCMB.bottom;
	rcBottom.bottom = rc.bottom;
	//根据分组数调整窗口位置
	if(m_arrPropGroupLabel.GetSize()<=0)
		rcHeader.bottom=rcTop.top=rcCMB.bottom;
	else
		rcHeader.bottom = rcTop.top = rcCMB.bottom+21;

	rcTop.bottom=rc.bottom-btmHeight-m_nSplitterWidth-1;
	rcBottom.top=rcTop.bottom+m_nSplitterWidth+1;
	m_nOldHorzY = rcBottom.top-m_nSplitterWidth/2;

	RefreshTabCtrl(0);
	m_ctrlPropGroup.MoveWindow(&rcHeader);
	if(pTopWnd)
		pTopWnd->MoveWindow(&rcTop);
	if(pBtmWnd)
		pBtmWnd->MoveWindow(&rcBottom);
	
	m_bTracking = FALSE;
	m_hCursorSize = AfxGetApp()->LoadStandardCursor(IDC_SIZENS);
	m_hCursorArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTowerPropertyDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	m_rcClient.bottom = cy;
	m_rcClient.right = cx;
	RECT rcHeader,rcTop,rcBottom,rcCMB;
	CWnd *pTopWnd = GetDlgItem(IDC_LIST_BOX);
	CWnd *pBtmWnd = GetDlgItem(IDC_S_PROP_HELP_PIC);	//IDC_E_PROP_HELP_STR
	CWnd *pCmbWnd = GetDlgItem(IDC_CMB_SELECT_OBJECT_INFO);
	if(pTopWnd)
		pTopWnd->GetWindowRect(&rcTop);
	if(pBtmWnd)
		pBtmWnd->GetWindowRect(&rcBottom);
	if(pCmbWnd)
		pCmbWnd->GetWindowRect(&rcCMB);
	if(!m_bShowTopCMB)
	{	//不显示顶部下拉框
		if(pCmbWnd)
			pCmbWnd->ShowWindow(SW_HIDE);
		rcCMB.top=rcCMB.bottom=rcCMB.left=rcCMB.right=0;
	}
	else 
	{
		if(pCmbWnd)
			pCmbWnd->ShowWindow(SW_SHOW);
		/*int cmbHeight = rcCMB.bottom - rcCMB.top;
		int cmbWidth  = rcCMB.right - rcCMB.left;
		rcCMB.left=rcCMB.top=0;
		rcCMB.right=cmbWidth;
		rcCMB.bottom=cmbHeight;
		rcCMB.bottom+=4;
		if(pCmbWnd)
			pCmbWnd->MoveWindow(&rcCMB);*/
		rcCMB.bottom+=4;
	}
	ScreenToClient(&rcTop);
	ScreenToClient(&rcBottom);
	if(m_bShowTopCMB)
		ScreenToClient(&rcCMB);
	int btmHeight = rcBottom.bottom - rcBottom.top;
	rcHeader.left = rcTop.left = rcBottom.left = 0;
	rcHeader.right = rcTop.right = rcBottom.right = cx;
	rcHeader.top=rcCMB.bottom;
	rcBottom.bottom = cy;
	//根据分组数调整窗口位置
	if(m_arrPropGroupLabel.GetSize()<=0)
		rcHeader.bottom=rcTop.top=rcCMB.bottom;
	else
		rcHeader.bottom = rcTop.top = rcCMB.bottom+21;

	rcTop.bottom=cy-btmHeight-m_nSplitterWidth-1;
	rcBottom.top=rcTop.bottom+m_nSplitterWidth+1;
	m_nOldHorzY = rcBottom.top-m_nSplitterWidth/2;
	if(m_ctrlPropGroup.GetSafeHwnd())
		m_ctrlPropGroup.MoveWindow(&rcHeader);
	if(pTopWnd)
		pTopWnd->MoveWindow(&rcTop);
	if(pBtmWnd)
		pBtmWnd->MoveWindow(&rcBottom);
}

void CTowerPropertyDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	RECT rcFull,rcHelpStr,rcSplitter;
	GetClientRect(&rcFull);
	CWnd *pWnd = GetDlgItem(IDC_S_PROP_HELP_PIC);	//IDC_E_PROP_HELP_STR
	pWnd->GetWindowRect(&rcHelpStr);
	ScreenToClient(&rcHelpStr);
	rcSplitter.left = rcFull.left;
	int width = 4;
	rcSplitter.top = rcHelpStr.top-width;
	rcSplitter.right = rcFull.right;
	rcSplitter.bottom = rcHelpStr.top-1;
	CPen psPen(PS_SOLID, 1, RGB(120,120,120));
	CPen* pOldPen = dc.SelectObject(&psPen);
	dc.MoveTo(rcSplitter.left,rcSplitter.top);
	dc.LineTo(rcSplitter.right,rcSplitter.top);
	dc.MoveTo(rcSplitter.left,rcSplitter.bottom);
	dc.LineTo(rcSplitter.right,rcSplitter.bottom);
	dc.SelectObject(pOldPen);
	psPen.DeleteObject();
	// TODO: Add your message handler code here
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CTowerPropertyDlg::InvertLine(CDC* pDC,CPoint ptFrom,CPoint ptTo)
{
	int nOldMode = pDC->SetROP2(R2_NOT);
	
	pDC->MoveTo(ptFrom);
	pDC->LineTo(ptTo);

	pDC->SetROP2(nOldMode);
}

void CTowerPropertyDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if ((point.y>=m_nOldHorzY-5) && (point.y<=m_nOldHorzY+5))
	{
		//if mouse clicked on divider line, then start resizing

		::SetCursor(m_hCursorSize);

		CRect windowRect;
		GetWindowRect(windowRect);
		windowRect.left += 10; windowRect.right -= 10;
		//do not let mouse leave the dialog boundary
		::ClipCursor(windowRect);
		m_nOldHorzY = point.y;

		CClientDC dc(this);
		InvertLine(&dc,CPoint(m_rcClient.left,m_nOldHorzY),CPoint(m_rcClient.right,m_nOldHorzY));

		//capture the mouse
		SetCapture();
		m_bTracking = TRUE;
	}
	else
	{
		m_bTracking = FALSE;
		CDialog::OnLButtonDown(nFlags, point);
	}
}

void CTowerPropertyDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bTracking)
	{
		m_bTracking = FALSE;
		//if mouse was captured then release it
		if (GetCapture()==this)
			::ReleaseCapture();

		::ClipCursor(NULL);

		CClientDC dc(this);
		InvertLine(&dc,CPoint(m_rcClient.left,point.y),CPoint(m_rcClient.right,point.y));
		//set the divider position to the new value
		m_nOldHorzY = point.y;

		RECT rcTop,rcBottom;
		CWnd *pTopWnd = GetDlgItem(IDC_LIST_BOX);
		CWnd *pBtmWnd = GetDlgItem(IDC_S_PROP_HELP_PIC);	//IDC_E_PROP_HELP_STR
		if(pTopWnd)
			pTopWnd->GetWindowRect(&rcTop);
		if(pBtmWnd)
			pBtmWnd->GetWindowRect(&rcBottom);
		ScreenToClient(&rcTop);
		ScreenToClient(&rcBottom);
		rcBottom.top = m_nOldHorzY+m_nSplitterWidth/2;
		rcTop.bottom = rcBottom.top-m_nSplitterWidth-1;
		pTopWnd->MoveWindow(&rcTop);
		pBtmWnd->MoveWindow(&rcBottom);
		//redraw
		Invalidate();
	}
	else
		CDialog::OnLButtonUp(nFlags, point);
}

void CTowerPropertyDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bTracking)
	{
		//move divider line to the mouse pos. if columns are
		//currently being resized
		CClientDC dc(this);
		//remove old divider line
		InvertLine(&dc,CPoint(m_rcClient.left,m_nOldHorzY),CPoint(m_rcClient.right,m_nOldHorzY));
		//draw new divider line
		InvertLine(&dc,CPoint(m_rcClient.left,point.y),CPoint(m_rcClient.right,point.y));
		m_nOldHorzY = point.y;
	}
	else if ((point.y >= m_nOldHorzY-5) && (point.y <= m_nOldHorzY+5))
		//set the cursor to a sizing cursor if the cursor is over the row divider
		::SetCursor(m_hCursorSize);
	else
		CDialog::OnMouseMove(nFlags, point);
}

void CTowerPropertyDlg::OnOK() 
{
	//确认输入
}

void CTowerPropertyDlg::OnCancel() 
{
}

BOOL CTowerPropertyDlg::PreTranslateMessage(MSG* pMsg) 
{
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CTowerPropertyDlg::IsHasFocus()
{
	CWnd *pWnd = GetFocus();
	CWnd *pParentWnd = NULL;
	HWND hParent = NULL;
	if(pWnd)
	{
		pParentWnd = pWnd->GetParent();
		if(pParentWnd)
			hParent = pParentWnd->GetSafeHwnd();
	}
	if(pWnd->GetSafeHwnd()==GetParent()->GetSafeHwnd()||pWnd->GetSafeHwnd()==GetSafeHwnd()||hParent==GetSafeHwnd())
		return TRUE;
	else
		return FALSE;
}


void CTowerPropertyDlg::OnKillFocus(CWnd* pNewWnd) 
{
	CDialog::OnKillFocus(pNewWnd);
/*	CPropertyList *pList = (CPropertyList*)GetDlgItem(IDC_LIST_BOX);
	pList->OnKillfocusCmbBox();
	pList->OnKillfocusEditBox();
	*/
}

void CTowerPropertyDlg::SetCurSelPropGroup(int iCurSel)
{
	switch(m_propList.m_nObjClassTypeId)
	{
	case CLS_NODE:
		CLDSNode::m_iCurDisplayPropGroup=iCurSel;
		break;
	case CLS_LINEPART:
		CLDSLinePart::m_iCurDisplayPropGroup=iCurSel;
		break;
#ifdef __PART_DESIGN_INC_
	case CLS_FEMELEMENT:
		CElemInfo::m_iCurDisplayPropGroup=iCurSel;
		break;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	case CLS_LINEANGLE:
		CLDSLineAngle::m_iCurDisplayPropGroup=iCurSel;
		break;
	case CLS_LINETUBE:
		CLDSLineTube::m_iCurDisplayPropGroup=iCurSel;
		break;
	case CLS_LINEFLAT:
		CLDSLineFlat::m_iCurDisplayPropGroup=iCurSel;
		break;
	case CLS_LINESLOT:
		CLDSLineSlot::m_iCurDisplayPropGroup=iCurSel;
		break;
	case CLS_GROUPLINEANGLE:
		CLDSGroupLineAngle::m_iCurDisplayPropGroup=iCurSel;
		break;
	case CLS_ARCANGLE:
		CLDSArcAngle::m_iCurDisplayPropGroup=iCurSel;
		break;
	case CLS_ARCFLAT:
		CLDSArcFlat::m_iCurDisplayPropGroup=iCurSel;
		break;
	case CLS_ARCSLOT:
		CLDSArcSlot::m_iCurDisplayPropGroup=iCurSel;
		break;
	case CLS_PARAMPLATE:
		CLDSParamPlate::m_iCurDisplayPropGroup=iCurSel;
		break;
	case CLS_PLATE:
		CLDSPlate::m_iCurDisplayPropGroup=iCurSel;
		break;
	case CLS_BOLT:
		CLDSBolt::m_iCurDisplayPropGroup=iCurSel;
		break;
	case CLS_SPHERE:
		CLDSSphere::m_iCurDisplayPropGroup=iCurSel;
		break;
#endif
#ifdef __COMPLEX_PART_INC_
	case CLS_BLOCK:
		CBlockModel::m_iCurDisplayPropGroup=iCurSel;
		break;
	case CLS_BLOCKREF:
		CBlockReference::m_iCurDisplayPropGroup=iCurSel;
		break;
#endif
	default:
		CLDSApp::m_iCurDisplayPropGroup=iCurSel;
		break;
	}
}

void CTowerPropertyDlg::OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_propList.m_iPropGroup = m_ctrlPropGroup.GetCurSel();
	SetCurSelPropGroup(m_propList.m_iPropGroup);
	m_propList.Redraw();
	*pResult = 0;
}

void CTowerPropertyDlg::RefreshTabCtrl(int iCurSel)
{
	m_ctrlPropGroup.DeleteAllItems();
	for(int i=0;i<m_arrPropGroupLabel.GetSize();i++)
		m_ctrlPropGroup.InsertItem(i,m_arrPropGroupLabel[i]);
	if(m_arrPropGroupLabel.GetSize()>0)
	{
		m_ctrlPropGroup.SetCurSel(iCurSel);
		m_propList.m_iPropGroup=iCurSel;
	}
	else //不需要分组显示
		m_propList.m_iPropGroup=0;
	
	//根据分组数调整窗口位置
	RECT rcPropWnd,rcHeader,rcTop,rcBottom,rcCMB;
	GetClientRect(&rcPropWnd);
	CWnd *pTopWnd = GetDlgItem(IDC_LIST_BOX);
	CWnd *pBtmWnd = GetDlgItem(IDC_S_PROP_HELP_PIC);	//IDC_E_PROP_HELP_STR
	CWnd *pCmbWnd = GetDlgItem(IDC_CMB_SELECT_OBJECT_INFO);
	if(pTopWnd)
		pTopWnd->GetWindowRect(&rcTop);
	if(pBtmWnd)
		pBtmWnd->GetWindowRect(&rcBottom);
	if(pCmbWnd)
		pCmbWnd->GetWindowRect(&rcCMB);
	if(!m_bShowTopCMB)
	{	//不显示顶部下拉框
		if(pCmbWnd)
			pCmbWnd->ShowWindow(SW_HIDE);
		rcCMB.top=rcCMB.bottom=rcCMB.left=rcCMB.right=0;
	}
	else 
	{
		if(pCmbWnd)
			pCmbWnd->ShowWindow(SW_SHOW);
		/*int cmbHeight = rcCMB.bottom - rcCMB.top;
		int cmbWidth  = rcCMB.right - rcCMB.left;
		rcCMB.left=rcCMB.top=0;
		rcCMB.right=cmbWidth;
		rcCMB.bottom=cmbHeight;
		rcCMB.bottom+=4;
		if(pCmbWnd)
			pCmbWnd->MoveWindow(&rcCMB);*/
		rcCMB.bottom+=4;
	}
	ScreenToClient(&rcTop);
	ScreenToClient(&rcBottom);
	if(m_bShowTopCMB)
		ScreenToClient(&rcCMB);
	int btmHeight = rcBottom.bottom - rcBottom.top;
	rcHeader.left = rcTop.left = rcBottom.left = 0;
	rcHeader.right = rcTop.right = rcBottom.right = rcPropWnd.right;
	rcHeader.top=rcCMB.bottom;
	rcBottom.bottom = rcPropWnd.bottom;
	if(m_arrPropGroupLabel.GetSize()<=0)
		rcHeader.bottom=rcTop.top=rcCMB.bottom;
	else
		rcHeader.bottom = rcTop.top = rcCMB.bottom+20;
	
	rcTop.bottom=rcPropWnd.bottom-btmHeight-m_nSplitterWidth-1;
	rcBottom.top=rcTop.bottom+m_nSplitterWidth+1;
	m_nOldHorzY = rcBottom.top-m_nSplitterWidth/2;
	if(m_ctrlPropGroup.GetSafeHwnd())
		m_ctrlPropGroup.MoveWindow(&rcHeader);
	if(pTopWnd)
		pTopWnd->MoveWindow(&rcTop);
	if(pBtmWnd)
		pBtmWnd->MoveWindow(&rcBottom);
}

void CTowerPropertyDlg::UpdateSelectFilter(HASHOBJSET& selectObjs)
{
	typedef struct tagPART_NUM
	{
		int nCount;
		char sName[16];
		LDSCLASS_TYPEID cls_id;
		tagPART_NUM() {memset(this,0,sizeof(tagPART_NUM));}
	}PART_NUM;
	
	// 0.nNode 1.nPart 2.nLinePart 3.nLineAngle 4.nLineTube 5. nLineFlat 6.nLineSlot 
	// 7.nArcPart 8.nArcFlat 9.nArcSlot 10.nArcAngle 11.nBolt 12.nPlate 13.nCommonPlate 14.nParamPlate
	PART_NUM partNumArr[15];
	int nSumObj=selectObjs.GetNodeNum();
	for(CLDSObject *pObj=selectObjs.GetFirst();pObj;pObj=selectObjs.GetNext())
	{
		if(!pObj->IsDbObject())
			continue;
		CLDSDbObject* pDbObj=(CLDSDbObject*)pObj;
		if(pDbObj->GetClassTypeId()==CLS_NODE)
		{
			partNumArr[0].nCount++;
			if(partNumArr[0].nCount==1)
			{	//节点个数
				partNumArr[0].cls_id=CLS_NODE;
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(partNumArr[0].sName,"Node");
#else
				strcpy(partNumArr[0].sName,"节点");
#endif
			}
		}
		else if(pDbObj->IsPart())
		{
			partNumArr[1].nCount++;
			if(partNumArr[1].nCount==1)
			{	//构件个数
				partNumArr[1].cls_id=CLS_PART;
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(partNumArr[1].sName,"Part");
#else
				strcpy(partNumArr[1].sName,"构件");
#endif
			}
			CLDSPart *pPart=(CLDSPart*)pObj;
			if(pPart->IsLinePart())
			{
				partNumArr[2].nCount++;
				if(partNumArr[2].nCount==1)
				{	//杆件个数
					partNumArr[2].cls_id=CLS_LINEPART;
#ifdef AFX_TARG_ENU_ENGLISH
					strcpy(partNumArr[2].sName,"Rod");
#else
					strcpy(partNumArr[2].sName,"杆件");
#endif
				}
				if(pPart->GetClassTypeId()==CLS_LINEANGLE)
				{
					partNumArr[3].nCount++;
					if(partNumArr[3].nCount==1)
					{	//角钢个数
						partNumArr[3].cls_id=CLS_LINEANGLE;
#ifdef AFX_TARG_ENU_ENGLISH
						strcpy(partNumArr[3].sName,"Angle");
#else
						strcpy(partNumArr[3].sName,"角钢");
#endif
					}
				}
				else if(pPart->GetClassTypeId()==CLS_LINETUBE)
				{
					partNumArr[4].nCount++;
					if(partNumArr[4].nCount==1)
					{	//钢管个数
						partNumArr[4].cls_id=CLS_LINETUBE;
#ifdef AFX_TARG_ENU_ENGLISH
						strcpy(partNumArr[4].sName,"Tube");
#else
						strcpy(partNumArr[4].sName,"钢管");
#endif
					}
				}
				else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
				{
					partNumArr[5].nCount++;
					if(partNumArr[5].nCount==1)
					{	//扁钢个数
						partNumArr[5].cls_id=CLS_LINEFLAT;
#ifdef AFX_TARG_ENU_ENGLISH
						strcpy(partNumArr[5].sName,"Flat");
#else
						strcpy(partNumArr[5].sName,"扁钢");
#endif
					}
				}
				else if(pPart->GetClassTypeId()==CLS_LINESLOT)
				{
					partNumArr[6].nCount++;
					if(partNumArr[6].nCount==1)
					{	//槽钢个数
						partNumArr[6].cls_id=CLS_LINESLOT;
#ifdef AFX_TARG_ENU_ENGLISH
						strcpy(partNumArr[6].sName,"Slot");
#else
						strcpy(partNumArr[6].sName,"槽钢");
#endif
					}
				}
			}
			else if(pPart->IsArcPart())
			{
				partNumArr[7].nCount++;
				if(partNumArr[7].nCount==1)
				{	//环形杆件个数
					partNumArr[7].cls_id=CLS_ARCPART;
#ifdef AFX_TARG_ENU_ENGLISH
					strcpy(partNumArr[7].sName,"Arc Rod");
#else
					strcpy(partNumArr[7].sName,"环形杆件");
#endif
				}
				if(pPart->GetClassTypeId()==CLS_ARCANGLE)
				{
					partNumArr[8].nCount++;
					if(partNumArr[8].nCount==1)
					{	//环形角钢个数
						partNumArr[8].cls_id=CLS_ARCANGLE;
#ifdef AFX_TARG_ENU_ENGLISH
						strcpy(partNumArr[8].sName,"Arc Angle");
#else
						strcpy(partNumArr[8].sName,"环形角钢");
#endif
					}
				}
				else if(pPart->GetClassTypeId()==CLS_ARCFLAT)
				{
					partNumArr[9].nCount++;
					if(partNumArr[9].nCount==1)
					{	//环形扁钢个数
						partNumArr[9].cls_id=CLS_ARCFLAT;
#ifdef AFX_TARG_ENU_ENGLISH
						strcpy(partNumArr[9].sName,"Arc Flat");
#else
						strcpy(partNumArr[9].sName,"环形扁钢");
#endif
					}
				}
				else if(pPart->GetClassTypeId()==CLS_ARCSLOT)
				{
					partNumArr[10].nCount++;
					if(partNumArr[10].nCount==1)
					{	//环形槽钢个数
						partNumArr[10].cls_id=CLS_ARCFLAT;
#ifdef AFX_TARG_ENU_ENGLISH
						strcpy(partNumArr[10].sName,"Arc Slot");
#else
						strcpy(partNumArr[10].sName,"环形槽钢");
#endif
					}
				}
			}
			else if(pPart->GetClassTypeId()==CLS_BOLT)
			{
				partNumArr[11].nCount++;
				if(partNumArr[11].nCount==1)
				{	//螺栓个数
					partNumArr[11].cls_id=CLS_BOLT;
#ifdef AFX_TARG_ENU_ENGLISH
					strcpy(partNumArr[11].sName,"Bolt");
#else
					strcpy(partNumArr[11].sName,"螺栓");
#endif
				}
			}
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE||pPart->GetClassTypeId()==CLS_PLATE)
			{
				partNumArr[12].nCount++;
				if(partNumArr[12].nCount==1)
				{	//钢板个数
					partNumArr[12].cls_id=LDSCLASS_TYPEID(-1);	//用-1表示钢板
#ifdef AFX_TARG_ENU_ENGLISH
					strcpy(partNumArr[12].sName,"Plate");
#else
					strcpy(partNumArr[12].sName,"钢板");
#endif
				}	
				if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
				{
					partNumArr[13].nCount++;
					if(partNumArr[13].nCount==1)
					{	//参数化板个数
						partNumArr[13].cls_id=CLS_PARAMPLATE;
#ifdef AFX_TARG_ENU_ENGLISH
						strcpy(partNumArr[13].sName,"Para Plate");
#else
						strcpy(partNumArr[13].sName,"参数化板");
#endif
					}
				}
				else if(pPart->GetClassTypeId()==CLS_PLATE)
				{
					partNumArr[14].nCount++;
					if(partNumArr[14].nCount==1)
					{	//普通钢板个数
						partNumArr[14].cls_id=CLS_PLATE;
#ifdef AFX_TARG_ENU_ENGLISH
						strcpy(partNumArr[14].sName,"Ordinary Plate");
#else
						strcpy(partNumArr[14].sName,"普通钢板");
#endif
					}
				}
			}
		}
	}
	while(m_cmbSelectObjInfo.GetCount()>0)
		m_cmbSelectObjInfo.DeleteString(0);
	int iItem=0;
	CString ss;
	if(nSumObj==0)
#ifdef AFX_TARG_ENU_ENGLISH
		m_cmbSelectObjInfo.AddString("no select");
	else
	{
		ss.Format("all(%d)",nSumObj);
#else
		m_cmbSelectObjInfo.AddString("无选择");
	else
	{
		ss.Format("全部(%d)",nSumObj);
#endif
		iItem=m_cmbSelectObjInfo.AddString(ss);
		m_cmbSelectObjInfo.SetItemData(iItem,DWORD(0));
	}
	for(int i=0;i<15;i++)
	{
		int nCount=partNumArr[i].nCount;
		LDSCLASS_TYPEID cls_id=partNumArr[i].cls_id;
		if(nCount>0)
		{
			if(((i==1&&cls_id==CLS_PART)||(i==2&&cls_id==CLS_LINEPART)
				||(i==7&&cls_id==CLS_ARCPART))&&partNumArr[i].nCount==1)
				continue;	
			else if(i==1&&nCount==nSumObj)
				continue; //构件数与对象总数相同时不用显示构件数
			else if(i==1&&(partNumArr[2].nCount==nCount||partNumArr[7].nCount==nCount))
				continue; //构件数与杆件数(或环形构件数)相同时不显示构件数
			else if(i==2&&(partNumArr[3].nCount==nCount||partNumArr[4].nCount==nCount
				||partNumArr[5].nCount==nCount||partNumArr[6].nCount==nCount))
				continue; //杆件数与角钢(钢管或扁钢或槽钢)数相同时无需显示杆件数
			else if(i==7&&(partNumArr[8].nCount==nCount||partNumArr[9].nCount==nCount
				||partNumArr[10].nCount==nCount))
				continue; //环形构件数与环形扁钢(槽钢或角钢)数相同时无需显示环向构件数
			else if(i==12&&(partNumArr[13].nCount==nCount||partNumArr[14].nCount==nCount))
				continue; //钢板数等于普通钢板或参数化钢板个数时无需显示钢板个数
			ss.Format("%s(%d)",partNumArr[i].sName,partNumArr[i].nCount);
			iItem=m_cmbSelectObjInfo.AddString(ss);
			m_cmbSelectObjInfo.SetItemData(iItem,DWORD(partNumArr[i].cls_id));
		}
	}
	if(m_cmbSelectObjInfo.GetCurSel()<0)
		m_cmbSelectObjInfo.SetCurSel(0);
	UpdateData(FALSE);
}

void CTowerPropertyDlg::OnSelchangeSelectObjectInfo() 
{
	UpdateData();
	CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	if(pLDSView==NULL)
		return;
	int iCurSel=m_cmbSelectObjInfo.GetCurSel();
	if(iCurSel<0)
		iCurSel=0;
	m_curClsTypeId=(LDSCLASS_TYPEID)m_cmbSelectObjInfo.GetItemData(iCurSel);
	pLDSView->UpdatePropertyPage(NULL,TRUE);
	UpdateData(FALSE);
}
void CTowerPropertyDlg::SetPropHelpToTextMode()
{
	if(m_propHelpWnd.GetStyle()&SS_BITMAP)
		SetPropHelpBmpID(NULL);	//设置为文本帮助模式
}
void CTowerPropertyDlg::SetPropHelpBmpID(UINT nID)
{	
	CRect rcBottom;
	m_propHelpWnd.GetWindowRect(&rcBottom);
	ScreenToClient(&rcBottom);
	//
	m_xCurHelpBmp.Detach();
	BOOL bRetCode=FALSE;
	if(nID!=0)
		bRetCode=m_xCurHelpBmp.LoadBitmap(nID);
	int nBtmHeight=0;
	if(bRetCode)
	{	//显示帮助图片
		m_propHelpWnd.ModifyStyle(0,SS_BITMAP|SS_CENTERIMAGE|SS_REALSIZEIMAGE);
		m_propHelpWnd.SetBitmap(m_xCurHelpBmp);
		//根据图片高度调整m_propHelpWnd高度
		nBtmHeight=rcBottom.bottom-rcBottom.top;
		if(m_nOldBtmHeight==0)
			m_nOldBtmHeight=nBtmHeight;
		BITMAP bmp;
		m_xCurHelpBmp.GetBitmap(&bmp);
		if(nBtmHeight<bmp.bmHeight)	//确保m_propHelpWnd高度大于图片高度
			nBtmHeight=bmp.bmHeight;
	}
	else
	{
		m_propHelpWnd.ModifyStyle(SS_BITMAP,0);
		m_propHelpWnd.ModifyStyle(SS_CENTERIMAGE,0);
		m_propHelpWnd.ModifyStyle(SS_REALSIZEIMAGE,0);
		m_propHelpWnd.SetFont(m_propList.GetFont());
		//恢复m_propHelpWnd原高度
		nBtmHeight=m_nOldBtmHeight;
	}
	if(nBtmHeight>0&&nBtmHeight!=(rcBottom.bottom-rcBottom.top))
	{	//调整帮助字符串窗口尺寸
		CRect rcTop;
		m_propList.GetWindowRect(&rcTop);
		ScreenToClient(&rcTop);
		rcTop.bottom=m_rcClient.bottom-nBtmHeight-m_nSplitterWidth-1;
		rcBottom.top=rcTop.bottom+m_nSplitterWidth+1;
		m_nOldHorzY = rcBottom.top-m_nSplitterWidth/2;
		m_propList.MoveWindow(&rcTop);
		m_propHelpWnd.MoveWindow(&rcBottom);
		m_propHelpWnd.Invalidate();
	}
}