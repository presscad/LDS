//<LOCALE_TRANSLATE Confirm by hxr/>
// ParamPlateParamListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "ParamPlateParamListDlg.h"
#include "MainFrm.h"
#include "GlobalFunc.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CParamPlateParamListDlg dialog

CParamPlateParamListDlg::CParamPlateParamListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamPlateParamListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CParamPlateParamListDlg)
	//}}AFX_DATA_INIT
	m_nSplitterWidth = 5;
	m_pParamPlate = NULL;
}


void CParamPlateParamListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParamPlateParamListDlg)
	DDX_Control(pDX, IDC_LIST_BOX, m_paramList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParamPlateParamListDlg, CDialog)
	//{{AFX_MSG_MAP(CParamPlateParamListDlg)
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBtnClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParamPlateParamListDlg message handlers

BOOL CParamPlateParamListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_pParamPlate==NULL)
		return FALSE;
	if(m_pParamPlate->GetClassTypeId()!=CLS_PARAMPLATE)
		return FALSE;
	RECT rc,rcTop,rcBottom;
	GetClientRect(&rc);
	m_rcClient = rc;
	m_rcClient.left = rc.left+365;
	m_rcClient.right = rc.right-12;
	CWnd *pTopWnd = GetDlgItem(IDC_LIST_BOX);
	CWnd *pBtmWnd = GetDlgItem(IDC_E_PROP_HELP_STR);
	if(pTopWnd)
		pTopWnd->GetWindowRect(&rcTop);
	if(pBtmWnd)
		pBtmWnd->GetWindowRect(&rcBottom);
	m_paramList.m_hPromptWnd = pBtmWnd->GetSafeHwnd();
	ScreenToClient(&rcTop);
	ScreenToClient(&rcBottom);
	int btmHeight = rcBottom.bottom - rcBottom.top;
	rcTop.left = rcBottom.left = rc.left+365;
	rcTop.right = rcBottom.right = rc.right-12;
	rcTop.top = 12;
	rcBottom.bottom = rc.bottom+12;
	rcTop.bottom=rc.bottom-btmHeight-m_nSplitterWidth-1;
	rcBottom.top=rcTop.bottom+m_nSplitterWidth+1;
	m_nOldHorzY = rcBottom.top-m_nSplitterWidth/2;
	if(pTopWnd)
		pTopWnd->MoveWindow(&rcTop);
	if(pBtmWnd)
		pBtmWnd->MoveWindow(&rcBottom);
	
	m_bTracking = FALSE;
	m_hCursorSize = AfxGetApp()->LoadStandardCursor(IDC_SIZENS);
	m_hCursorArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	//根据所选参数板的类型显示合适的图片
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_PLATE_TYPE);
	m_CurPlateBmp.Detach();
	if(m_pParamPlate->m_iParamType==TYPE_FL||m_pParamPlate->m_iParamType==TYPE_FLG||
		m_pParamPlate->m_iParamType==TYPE_FLR)
		m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_FL2);
	else if(m_pParamPlate->m_iParamType==TYPE_UEND)
		m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_UEND_BOLT);
	else if(m_pParamPlate->m_iParamType==TYPE_ROLLEND)
		m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_ROLLEND_BOLT);
	else if(m_pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		m_CurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_COVERPLATE);
	else if(m_pParamPlate->m_iParamType==TYPE_ELBOWPLATE)
		m_CurPlateBmp.LoadBitmap(IDB_BMP_ELBOW_PLATE);
	else if(m_pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
	{	//根据肋板外形显示不同示意图 wht 10-10-26
		long B=0,C=0,O=0;
		m_pParamPlate->GetDesignItemValue('B',&B);	//第一块基准钢板句柄
		m_pParamPlate->GetDesignItemValue('C',&C);	//第二块基准钢板句柄
		m_pParamPlate->GetDesignItemValue('O',&O);	//顶部是否为圆弧连接
		CLDSPlate *pDatumPlate1=(CLDSPlate*)console.FromPartHandle(B,CLS_PLATE);
		CLDSPlate *pDatumPlate2=(CLDSPlate*)console.FromPartHandle(C,CLS_PLATE);
		if(O==1)
		{	//顶部圆弧连接
			if(pDatumPlate1&&pDatumPlate2)	//两块基准钢板的顶部圆弧连接
				m_CurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE2);
			else if(pDatumPlate1&&pDatumPlate2==NULL)	//一块基准钢板的顶部圆弧连接
				m_CurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE4);
			else 
				m_CurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE);
		}
		else //不同环向肋板
			m_CurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE);
	}
	else if(m_pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
		m_CurPlateBmp.LoadBitmap(IDB_BMP_TUBE_RIB_PLATE);
	else if(m_pParamPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
	{
		long I=0;
		m_pParamPlate->GetDesignItemValue(KEY2C("I"),&I);	//外形类别 0.内侧肋板|1.外侧肋板
		if (I == 0)
		{
			long C=0;
			m_pParamPlate->GetDesignItemValue(KEY2C("C"),&C);	//内侧肋板斜边类型 0.直线|1.圆弧
			if(C == 0)
				m_CurPlateBmp.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE);
			else
				m_CurPlateBmp.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE1);
		}
		else if(I==1)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE2);
		else if(I==2)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE3);
	}
	else if(m_pParamPlate->m_iParamType==TYPE_XEND)
		m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_XEND_BOLT);
	else if(m_pParamPlate->m_iParamType==TYPE_FLD)
		m_CurPlateBmp.LoadBitmap(IDB_BMP_STD_PART_FLD);
	else if(m_pParamPlate->m_iParamType==TYPE_FLP)
		m_CurPlateBmp.LoadBitmap(IDB_BMP_STD_PART_FLP);
	else
		m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_NONE);
	pPic->SetBitmap(m_CurPlateBmp);
	//显示参数板相关设计参数
	DisplayParamPlateDesignParam();
	//移动对话框到左上角
	CRect rect;
	GetWindowRect(rect);
	rect.right = rect.Width();
	rect.left = 0;
	rect.bottom = rect.Height();
	rect.top = 0;
	MoveWindow(rect,TRUE);
	return TRUE;
}

void CParamPlateParamListDlg::OnKillFocus(CWnd* pNewWnd) 
{
	CDialog::OnKillFocus(pNewWnd);
}

void CParamPlateParamListDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
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
	CDialog::OnLButtonDown(nFlags, point);
}

void CParamPlateParamListDlg::OnMouseMove(UINT nFlags, CPoint point) 
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
	CDialog::OnMouseMove(nFlags, point);
}

void CParamPlateParamListDlg::OnLButtonUp(UINT nFlags, CPoint point) 
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
		CWnd *pBtmWnd = GetDlgItem(IDC_E_PROP_HELP_STR);
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

void CParamPlateParamListDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	RECT rcFull,rcHelpStr,rcSplitter;
	GetClientRect(&rcFull);
	CWnd *pWnd = GetDlgItem(IDC_E_PROP_HELP_STR);
	pWnd->GetWindowRect(&rcHelpStr);
	ScreenToClient(&rcHelpStr);
	rcSplitter.left = rcHelpStr.left;
	int width = 4;
	rcSplitter.top = rcHelpStr.top-width;
	rcSplitter.right = rcHelpStr.right;
	rcSplitter.bottom = rcHelpStr.top-1;
	CPen psPen(PS_SOLID, 1, RGB(120,120,120));
	CPen* pOldPen = dc.SelectObject(&psPen);
	dc.MoveTo(rcSplitter.left,rcSplitter.top);
	dc.LineTo(rcSplitter.right,rcSplitter.top);
	dc.MoveTo(rcSplitter.left,rcSplitter.bottom);
	dc.LineTo(rcSplitter.right,rcSplitter.bottom);
	dc.SelectObject(pOldPen);
	psPen.DeleteObject();
}

void CParamPlateParamListDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	m_rcClient.bottom = cy;
	m_rcClient.right = cx;
	RECT rcTop,rcBottom;
	CWnd *pTopWnd = GetDlgItem(IDC_LIST_BOX);
	CWnd *pBtmWnd = GetDlgItem(IDC_E_PROP_HELP_STR);
	if(pTopWnd)
		pTopWnd->GetWindowRect(&rcTop);
	if(pBtmWnd)
		pBtmWnd->GetWindowRect(&rcBottom);
	ScreenToClient(&rcTop);
	ScreenToClient(&rcBottom);
	int btmHeight = rcBottom.bottom - rcBottom.top;
	rcTop.left = rcBottom.left = cy+500;
	rcTop.right = rcBottom.right = cx;
	rcTop.top = 0;
	rcBottom.bottom = cy;
	rcTop.bottom=cy-btmHeight-m_nSplitterWidth-1;
	rcBottom.top=rcTop.bottom+m_nSplitterWidth+1;
	m_nOldHorzY = rcBottom.top-m_nSplitterWidth/2;
	if(pTopWnd)
		pTopWnd->MoveWindow(&rcTop);
	if(pBtmWnd)
		pBtmWnd->MoveWindow(&rcBottom);	
}

void CParamPlateParamListDlg::InvertLine(CDC* pDC,CPoint ptFrom,CPoint ptTo)
{
	int nOldMode = pDC->SetROP2(R2_NOT);
	
	pDC->MoveTo(ptFrom);
	pDC->LineTo(ptTo);

	pDC->SetROP2(nOldMode);
}

BOOL CParamPlateParamListDlg::IsHasFocus()
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

//判断当前选中项的指定项的参数值是否相同
BOOL IsMutiObjsParam(CLDSParamPlate *pParamPlate, DESIGN_PLATE_ITEM *pItem)
{
	CLDSView *pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	if(pView->selectObjs.GetNodeNum()<=1)
		return FALSE;
	for(CLDSParamPlate *pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;
		pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
	{
		double fValue=-10000;
		long handle=-10000;
		if(pPlate->GetClassTypeId()!=CLS_PARAMPLATE)
			continue;
		if(pPlate==pParamPlate)
			continue;
		if(pItem->dataType==0)
		{
			pPlate->GetDesignItemValue(pItem->key,&fValue);
			if(fValue!=pItem->value.fValue)
				return TRUE;
		}
		else if(pItem->dataType==1)
		{
			pPlate->GetDesignItemValue(pItem->key,&handle);
			if(handle!=pItem->value.hPart)
				return TRUE;
		}
		else if(pItem->dataType==2)
		{
			pPlate->GetDesignItemValue(pItem->key,&handle);
			if(handle!=pItem->value.iType)
				return TRUE;
		}
	}
	return FALSE;
}

//修改指定项的参数值
BOOL ModifyItemParamValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pFirstParamPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	char tem_str[100];

	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	sprintf(tem_str, "%s",valueStr);
	DESIGN_PLATE_ITEM *pDesignItem=pFirstParamPlate->GetDesignItem((WORD)pItem->m_idProp);
	if(pDesignItem==NULL)
		return FALSE;
	for(CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
	{
		if(pParamPlate->GetClassTypeId()!=CLS_PARAMPLATE)
			continue;
		char sNotes[200]="";
		strcpy(sNotes,pDesignItem->sNotes);
		if(pDesignItem->dataType==1)
		{
			long handle=0;
			sscanf(tem_str,"%X",&handle);
			pParamPlate->SetDesignItemValue(pDesignItem->key,handle,sNotes,pDesignItem->paramType,1);
		}
		else
			pParamPlate->SetDesignItemValue(pDesignItem->key,atof(tem_str),sNotes,pDesignItem->paramType,pDesignItem->dataType);
	}
	return TRUE;
}

void CParamPlateParamListDlg::DisplayParamPlateDesignParam()
{
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL, *pRoot=m_paramList.GetRootItem();
	m_paramList.m_nObjClassTypeId = CLS_PARAMPLATE;
	CItemInfo* lpInfo=NULL;
	DESIGN_PLATE_ITEM *pItem=NULL;
	m_paramList.CleanTree();
	//设置属性栏回调函数
	m_paramList.SetModifyValueFunc(ModifyItemParamValue);
	CString ss;
	m_paramList.m_iPropGroup=0;		//不分组的时候需默认显示第一分组信息
	if(m_pParamPlate->m_bStdPart)	//标准化参数板
	{
		CLDSStdPartModel stdPartModel;
		if(!m_pParamPlate->GetStdPartModel(&stdPartModel))
			return;
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "position para";
		lpInfo->m_strPropHelp = "positioning parameters";
#else
		lpInfo->m_strPropName = "定位参数";
		lpInfo->m_strPropHelp = "定位参数";
#endif
		pGroupItem = m_paramList.InsertItem(pRoot,lpInfo, -1);
		
		//D与标准法兰相关参数同名,用X表示旋转角度  暂时如此处理 wht 10-01-21
		double X=0;
		m_pParamPlate->GetDesignItemValue('X',&X);
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_cmbType=CDT_EDIT;
		lpInfo->m_cmbItems="0|90|180|270|";
		lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="work plane's rotation degree relative to tube's datum plane";
#else
		lpInfo->m_strPropHelp="工作面相对钢管基准面旋转角度";
#endif
		ss.Format("%f", X);
		SimplifiedNumString(ss);
		lpInfo->m_strPropValue=ss;
		pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp=(int)'X';
		//J
		long J=0;
		m_pParamPlate->GetDesignItemValue('J',&J);
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="J";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="when ring layout bolts:1 means that welding road line is the datum, 0 means the datum edge is the datum.";
#else
		lpInfo->m_strPropHelp="环向布置螺栓时:1表示以焊道线为基准，0表示以基准边为基准。";
#endif
		ss.Format("%d", J);
		lpInfo->m_strPropValue=ss;
		pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp=(int)'J';
		//钢管端与法兰底面的距离
		if(m_pParamPlate->m_iParamType==TYPE_FLP)
		{	//H1保存在参数'O'中	wht 10-04-09
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="H1";	
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="distance from tube end and flange bottom";
#else
			lpInfo->m_strPropHelp="钢管端与法兰底面的距离";
#endif
			long O=0;
			m_pParamPlate->GetDesignItemValue('O',&O);
			ss.Format("%d", O);
			lpInfo->m_strPropValue=ss;
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp=(int)'O';
		}
		else if(m_pParamPlate->m_iParamType==TYPE_FLR||
			m_pParamPlate->m_iParamType==TYPE_FLG)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="H";	
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="distance from tube end and flange bottom";
#else
			lpInfo->m_strPropHelp="钢管端与法兰底面的距离";
#endif
			double H=0;
			m_pParamPlate->GetDesignItemValue('H',&H);
			lpInfo->m_strPropValue.Format("%.f",H);
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp=(int)'H';
			pPropItem->SetReadOnly();
		}
		/*
		long J=0;
		m_pParamPlate->GetDesignItemValue('J',&J);
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_strPropName="J";
		lpInfo->m_cmbItems = "0.基准边|1.焊道线|";
		lpInfo->m_strPropHelp="环向布置螺栓时:1表示以焊道线为基准，0表示以基准边为基准。";
		if(J!=0)
			lpInfo->m_strPropValue.Format("%s","1.焊道线");
		else
			lpInfo->m_strPropValue.Format("%s","0.基准边");
		pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp=(int)'J';
		*/

		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "appearance para";
		lpInfo->m_strPropHelp = "appearance parameters";
#else
		lpInfo->m_strPropName = "外形参数";
		lpInfo->m_strPropHelp = "外形参数";
#endif
		pGroupItem = m_paramList.InsertItem(pRoot,lpInfo, -1);
		if(m_pParamPlate->m_iParamType==TYPE_FLG||m_pParamPlate->m_iParamType==TYPE_FLR)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="R";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="flange inside diameter";
#else
			lpInfo->m_strPropHelp="法兰内径";
#endif
			lpInfo->m_strPropValue.Format("%f", stdPartModel.param.fl_param2.fInnerD*0.5);
			SimplifiedNumString(lpInfo->m_strPropValue);
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->SetReadOnly();
			
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="T";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="flange thickness";
#else
			lpInfo->m_strPropHelp="法兰厚度";
#endif
			lpInfo->m_strPropValue.Format("%f", stdPartModel.param.fl_param2.fThick);
			SimplifiedNumString(lpInfo->m_strPropValue);
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->SetReadOnly();
			
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="W";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="flange outside diameter";
#else
			lpInfo->m_strPropHelp="法兰外径";
#endif
			lpInfo->m_strPropValue.Format("%f", stdPartModel.param.fl_param2.fOuterD);
			SimplifiedNumString(lpInfo->m_strPropValue);
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->SetReadOnly();
			
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="K";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="bolt's hole positioning circle diameter";
#else
			lpInfo->m_strPropHelp="螺栓孔定位圆直径";
#endif
			lpInfo->m_strPropValue.Format("%f", stdPartModel.param.fl_param2.fBoltCirD);
			SimplifiedNumString(lpInfo->m_strPropValue);
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->SetReadOnly();
		}
		else if(m_pParamPlate->m_iParamType==TYPE_FLP||m_pParamPlate->m_iParamType==TYPE_FLD)
		{
			if(m_pParamPlate->m_iParamType==TYPE_FLD)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="A";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="welding end outside diameter";
#else
				lpInfo->m_strPropHelp="焊端外径";
#endif
				ss.Format("%f", stdPartModel.param.fl_param.A);
				SimplifiedNumString(ss);
				lpInfo->m_strPropValue=ss;
				pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
				pPropItem->SetReadOnly();
			}
			else if(m_pParamPlate->m_iParamType==TYPE_FLP)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="B";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="flange inside diameter";
#else
				lpInfo->m_strPropHelp="法兰内径";
#endif
				ss.Format("%f", stdPartModel.param.fl_param.B);
				SimplifiedNumString(ss);
				lpInfo->m_strPropValue=ss;
				pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
				pPropItem->SetReadOnly();
			}
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="C";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="flange thickness";
#else
			lpInfo->m_strPropHelp="法兰厚度";
#endif
			ss.Format("%f", stdPartModel.param.fl_param.C);
			SimplifiedNumString(ss);
			lpInfo->m_strPropValue=ss;
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->SetReadOnly();

			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="D";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="flange outside diameter";
#else
			lpInfo->m_strPropHelp="法兰外径";
#endif
			ss.Format("%f", stdPartModel.param.fl_param.D);
			SimplifiedNumString(ss);
			lpInfo->m_strPropValue=ss;
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->SetReadOnly();

			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="H";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="flange height";
#else
			lpInfo->m_strPropHelp="法兰高度";
#endif
			ss.Format("%f", stdPartModel.param.fl_param.H);
			SimplifiedNumString(ss);
			lpInfo->m_strPropValue=ss;
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->SetReadOnly();
			
			if(m_pParamPlate->m_iParamType==TYPE_FLD)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="H1";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="top height of flange";	
#else
				lpInfo->m_strPropHelp="法兰顶部高度";	
#endif
				ss.Format("%f", stdPartModel.param.fl_param.H1);
				SimplifiedNumString(ss);
				lpInfo->m_strPropValue=ss;
				pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
				pPropItem->SetReadOnly();
			}

			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="K";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="bolt's hole positioning circle diameter";
#else
			lpInfo->m_strPropHelp="螺栓孔定位圆直径";
#endif
			ss.Format("%f", stdPartModel.param.fl_param.K);
			SimplifiedNumString(ss);
			lpInfo->m_strPropValue=ss;
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->SetReadOnly();

			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="N";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="flange root diameter，including R";
#else
			lpInfo->m_strPropHelp="法兰根部直径，包括尺寸R";
#endif
			ss.Format("%f", stdPartModel.param.fl_param.N);
			SimplifiedNumString(ss);
			lpInfo->m_strPropValue=ss;
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->SetReadOnly();

			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="R";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="flange neck chamfering radius";
#else
			lpInfo->m_strPropHelp="法兰颈部倒角半径";
#endif
			ss.Format("%f", stdPartModel.param.fl_param.R);
			SimplifiedNumString(ss);
			lpInfo->m_strPropValue=ss;
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->SetReadOnly();

			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="S";
			if(m_pParamPlate->m_iParamType==TYPE_FLD)
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="S=flange top thickness-main tube thickness difference valve ";
			else 
				lpInfo->m_strPropHelp="flange top thickness";
#else
				lpInfo->m_strPropHelp="S=法兰顶部厚度-主管厚度的差值";
			else 
				lpInfo->m_strPropHelp="法兰顶部厚度";
#endif
			ss.Format("%f", stdPartModel.param.fl_param.S);
			SimplifiedNumString(ss);
			lpInfo->m_strPropValue=ss;
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->SetReadOnly();
		}
		else if(m_pParamPlate->m_iParamType==TYPE_UEND||m_pParamPlate->m_iParamType==TYPE_ROLLEND||m_pParamPlate->m_iParamType==TYPE_XEND)
		{
			if(m_pParamPlate->m_iParamType==TYPE_XEND)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="A";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="cross plate's additional plate positioning parameter A";
#else
				lpInfo->m_strPropHelp="十字插板附加板定位参数A";
#endif
				ss.Format("%f", stdPartModel.param.insert_plate_param.A);
				SimplifiedNumString(ss);
				lpInfo->m_strPropValue=ss;
				pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
				pPropItem->SetReadOnly();
				
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="R";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="cross plate's additional plate positioning parameter R";
#else
				lpInfo->m_strPropHelp="十字插板附加板定位参数R";
#endif
				ss.Format("%f", stdPartModel.param.insert_plate_param.R);
				SimplifiedNumString(ss);
				lpInfo->m_strPropValue=ss;
				pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
				pPropItem->SetReadOnly();
			}
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="H";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="width";
#else
			lpInfo->m_strPropHelp="宽度";
#endif
			ss.Format("%f", stdPartModel.param.insert_plate_param.H);
			SimplifiedNumString(ss);
			lpInfo->m_strPropValue=ss;
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->SetReadOnly();

			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="L";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="length";
#else
			lpInfo->m_strPropHelp="长度";
#endif
			ss.Format("%f", stdPartModel.param.insert_plate_param.L);
			SimplifiedNumString(ss);
			lpInfo->m_strPropValue=ss;
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->SetReadOnly();

			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="M";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="length of the part inserted in tube";
#else
			lpInfo->m_strPropHelp="插入钢管部分的长度";
#endif
			ss.Format("%f", stdPartModel.param.insert_plate_param.M);
			SimplifiedNumString(ss);
			lpInfo->m_strPropValue=ss;
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->SetReadOnly();

			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="N";
			if(m_pParamPlate->m_iParamType==TYPE_XEND)
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="width of cross plate's additional plate";
			else 
				lpInfo->m_strPropHelp="length of cut angle";
#else
				lpInfo->m_strPropHelp="十字插板附加板宽度";
			else 
				lpInfo->m_strPropHelp="切角长度";
#endif
			ss.Format("%f", stdPartModel.param.insert_plate_param.N);
			SimplifiedNumString(ss);
			lpInfo->m_strPropValue=ss;
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->SetReadOnly();

			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="S";
			if(m_pParamPlate->m_iParamType==TYPE_XEND)
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="length of cross plate's additional plate";
			else 
				lpInfo->m_strPropHelp="width of cut angle";
#else
				lpInfo->m_strPropHelp="十字插板附加板长度";
			else 
				lpInfo->m_strPropHelp="切角宽度";
#endif
			ss.Format("%f", stdPartModel.param.insert_plate_param.S);
			SimplifiedNumString(ss);
			lpInfo->m_strPropValue=ss;
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->SetReadOnly();

			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="T";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="thickness";
#else
			lpInfo->m_strPropHelp="厚度";
#endif
			ss.Format("%d", stdPartModel.param.insert_plate_param.Thick);
			SimplifiedNumString(ss);
			lpInfo->m_strPropValue=ss;
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->SetReadOnly();

			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="W";
			if(m_pParamPlate->m_iParamType==TYPE_XEND)
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="thickness of cross plate's additional plate";
			else if(m_pParamPlate->m_iParamType==TYPE_ROLLEND)
				lpInfo->m_strPropHelp="height of rolled edge";
			else if(m_pParamPlate->m_iParamType==TYPE_UEND)
				lpInfo->m_strPropHelp="distance between top and down plane of U-plate";
#else
				lpInfo->m_strPropHelp="十字插板附加板厚度";
			else if(m_pParamPlate->m_iParamType==TYPE_ROLLEND)
				lpInfo->m_strPropHelp="卷边高度";
			else if(m_pParamPlate->m_iParamType==TYPE_UEND)
				lpInfo->m_strPropHelp="U型板上下两面间的距离";
#endif
			ss.Format("%f", stdPartModel.param.insert_plate_param.W);
			SimplifiedNumString(ss);
			lpInfo->m_strPropValue=ss;
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->SetReadOnly();
		}
	}
	else	//普通参数板
	{
		//定位参数
		BOOL bHasPosParam=FALSE, bHasShapeParam=FALSE;
		for(pItem=m_pParamPlate->params.GetFirst();pItem;pItem=m_pParamPlate->params.GetNext())
		{
			if(pItem->dataType==1)
			{
				if(!bHasPosParam)
				{	//插入第一个定位参数前插入该项
					bHasPosParam=TRUE;
					lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "position para";
					lpInfo->m_strPropHelp = "positioning parameters";
#else
					lpInfo->m_strPropName = "定位参数";
					lpInfo->m_strPropHelp = "定位参数";
#endif
					pGroupItem = m_paramList.InsertItem(pRoot,lpInfo, -1);
				}
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName=pItem->key.Label();
				lpInfo->m_strPropHelp=pItem->sNotes;
				lpInfo->m_bMutiObjsProp=IsMutiObjsParam(m_pParamPlate,pItem);
				if(pItem->value.hPart<0)
					lpInfo->m_strPropValue.Format("-0X%X",abs(pItem->value.hPart));
				else 
					lpInfo->m_strPropValue.Format("0X%X",pItem->value.hPart);
				pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
				pPropItem->m_idProp=(WORD)pItem->key;
			}
			else
			{
				if(pItem->paramType!=0)
					continue;
				if(m_pParamPlate->m_iParamType==TYPE_XEND&&pItem->key=='D')
					continue;	//十字插板的旋转角度项改到直接从Ｚ轴方向设定
				if(!bHasPosParam)
				{	//插入第一个定位参数前插入该项
					bHasPosParam=TRUE;
					lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "position para";
					lpInfo->m_strPropHelp = "positioning parameters";
#else
					lpInfo->m_strPropName = "定位参数";
					lpInfo->m_strPropHelp = "定位参数";
#endif
					pGroupItem = m_paramList.InsertItem(pRoot,lpInfo, -1);
				}
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				if(pItem->key=='D')
				{	//旋转角度
					lpInfo->m_controlType=PIT_COMBO;
					lpInfo->m_cmbType=CDT_EDIT;
					lpInfo->m_cmbItems="0|90|180|270|";
				}
				lpInfo->m_strPropName=pItem->key.Label();//pItem->cItem;
				lpInfo->m_strPropHelp=pItem->sNotes;
				lpInfo->m_bMutiObjsProp=IsMutiObjsParam(m_pParamPlate,pItem);
				ss.Format("%g", pItem->ToDouble());
				lpInfo->m_strPropValue=ss;
				pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
				pPropItem->m_idProp=pItem->key;
			}
		}
		//外形参数
		BOOL bBetaPara=FALSE;
		if(m_pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
		{	//根据肋板外形显示D参数名称 wht 10-10-28
			long B=0,C=0,O=0;
			m_pParamPlate->GetDesignItemValue('B',&B);	//第一块基准钢板句柄
			m_pParamPlate->GetDesignItemValue('C',&C);	//第二块基准钢板句柄
			m_pParamPlate->GetDesignItemValue('O',&O);	//顶部是否为圆弧连接
			CLDSPlate *pDatumPlate1=(CLDSPlate*)console.FromPartHandle(B,CLS_PLATE);
			CLDSPlate *pDatumPlate2=(CLDSPlate*)console.FromPartHandle(C,CLS_PLATE);
			if(O==1&&pDatumPlate1&&pDatumPlate2==NULL)
				bBetaPara=TRUE;	//D参数名显示为β
		}
		for(pItem=m_pParamPlate->params.GetFirst();pItem;pItem=m_pParamPlate->params.GetNext())
		{
			if(pItem->dataType==1)
				continue;	//数据类型为句柄都为定位参数
			if(pItem->paramType!=1)
				continue;	//不为外形参数的跳过
			if(!bHasShapeParam)
			{	//插入第一个外形参数前插入该项
				bHasShapeParam=TRUE;
				lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "appearance para";
				lpInfo->m_strPropHelp = "appearance parameters";
#else
				lpInfo->m_strPropName = "外形参数";
				lpInfo->m_strPropHelp = "外形参数";
#endif
				pGroupItem = m_paramList.InsertItem(pRoot,lpInfo, -1);
			}
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			if(pItem->key=='D'&&bBetaPara)
				lpInfo->m_strPropName="β";
			else
				lpInfo->m_strPropName=pItem->key.Label();
			lpInfo->m_strPropHelp=pItem->sNotes;
			lpInfo->m_bMutiObjsProp=IsMutiObjsParam(m_pParamPlate,pItem);
			if(pItem->dataType==0)	//浮点数
				ss.Format("%f", pItem->value.fValue);
			else if(pItem->dataType==1)	//句柄值
				ss.Format("0X%X",pItem->value.hPart);
			else//if(pItem->dataType==2)//整数值
				ss.Format("%d",pItem->value.iType);
			SimplifiedNumString(ss);
			lpInfo->m_strPropValue=ss;
			pPropItem=m_paramList.InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp=pItem->key;
		}
	}
	m_paramList.Redraw();
}

void CParamPlateParamListDlg::OnOK() 
{
	//确认输入
}

void CParamPlateParamListDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CParamPlateParamListDlg::OnClose() 
{
	CUndoOperObject undo(&Ta,true);
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	try{
	for(CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
	{	
		if(pParamPlate->GetClassTypeId()!=CLS_PARAMPLATE)
			continue;
		//重新设计参数化板
		CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(pParamPlate->m_hPartWeldParent,CLS_LINETUBE);
		if(pLineTube)
		{
			if(pLineTube->m_tJointEnd.hLinkObj==pParamPlate->handle)
				pLineTube->DesignEndJointUCS(pParamPlate);
			else if(pLineTube->m_tJointStart.hLinkObj==pParamPlate->handle)
				pLineTube->DesignStartJointUCS(pParamPlate);
		}
		pParamPlate->SetModified();
		pParamPlate->DesignStdPartUcs();
		pParamPlate->DesignPlate();
		pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pParamPlate->GetSolidPartObject());
		//重新设计参数板时将其附带的螺栓重新设计 wht 11-05-27
		for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->CalGuigeAuto();
			pBolt->SetModified();
			pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
		//重新设计U型板和卷边板时将其附带的封头板和加劲板一起重新设计 wht 11-05-27
		if(pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND||
			pParamPlate->m_iParamType==TYPE_XEND)
		{
			for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
			{
				CLDSPart *pPart=console.FromPartHandle(pItem->h,CLS_PLATE,CLS_PARAMPLATE);
				if(pPart==NULL)
					continue;
				if(pPart->GetClassTypeId()==CLS_PLATE)
				{
					CLDSPlate *pPlate=(CLDSPlate*)pPart;
					pPlate->DesignSetupUcs();
					pPlate->DesignPlate();
				}
				else 
					((CLDSParamPlate*)pPart)->DesignPlate();
				pPart->SetModified();
				pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
			}
		}
		g_pSolidDraw->Draw();
	}
	}
	catch(char* error)
	{
		AfxMessageBox(error);
	}
	catch(...)
	{
		AfxMessageBox("unexpected error");
	}
	CDialog::OnClose();
}

void CParamPlateParamListDlg::OnBtnClose() 
{
	OnClose();
	CDialog::OnOK();
}
#endif