//<LOCALE_TRANSLATE Confirm by hxr/>
// PartGroupAngularDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "PartGroupAngularDlg.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "env_def.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CPartGroupAngularDlg dialog


CPartGroupAngularDlg::CPartGroupAngularDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPartGroupAngularDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartGroupAngularDlg)
	m_bStartLineIsWeldRoad = FALSE;
	m_fDimR = 0.0;
	m_sEndPartHandle = _T("0X0");
	m_sStartPartHandle = _T("0X0");
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("starting parts");
	m_arrHeaderLabel.Add("ending parts");
	m_arrHeaderLabel.Add("dimension radius");
	m_arrHeaderLabel.Add("starting edge");
#else
	m_arrHeaderLabel.Add("起始构件");
	m_arrHeaderLabel.Add("终止构件");
	m_arrHeaderLabel.Add("标注半径");
	m_arrHeaderLabel.Add("起始边");
#endif
	m_pWorkView = NULL;
	m_pWorkPartGroup = NULL;
	m_bPauseBreakExit = FALSE;
	m_bHasWeldRoad=FALSE;
}


void CPartGroupAngularDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartGroupAngularDlg)
	DDX_Check(pDX, IDC_CHK_START_LINE_IS_WELD_ROAD, m_bStartLineIsWeldRoad);
	DDX_Text(pDX, IDC_E_DIM_R, m_fDimR);
	DDX_Text(pDX, IDC_E_END_PART, m_sEndPartHandle);
	DDX_Text(pDX, IDC_E_START_PART, m_sStartPartHandle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPartGroupAngularDlg, CDialog)
	//{{AFX_MSG_MAP(CPartGroupAngularDlg)
	ON_BN_CLICKED(IDC_BTN_ADD_ANGULAR, OnAddAngular)
	ON_BN_CLICKED(IDC_BTN_MODIFY_ANGULAR, OnModifyAngular)
	ON_BN_CLICKED(IDC_BTN_SELECT_PART, OnSelectPart)
	ON_NOTIFY(NM_CLICK, IDC_LIST_ANGULAR, OnClickListAngular)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ANGULAR, OnDblclkListAngular)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_ANGULAR, OnKeydownListAngular)
	ON_EN_KILLFOCUS(IDC_E_START_PART, OnKillfocusStartPart)
	ON_EN_KILLFOCUS(IDC_E_END_PART, OnKillfocusEndPart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartGroupAngularDlg message handlers

BOOL CPartGroupAngularDlg::OnInitDialog() 
{
	m_bPauseBreakExit=FALSE;
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_ANGULAR);
	if(m_pListCtrl)
		m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	if(m_pWorkView==NULL||m_pWorkPartGroup==NULL)
		return FALSE;
	for(CAngularDimension *pAngular=m_pWorkView->Angulars.GetFirst();pAngular;pAngular=m_pWorkView->Angulars.GetNext())
	{
		CLDSPart *pStartPart = console.FromPartHandle(pAngular->m_hStartPart);
		CLDSPart *pEndPart = console.FromPartHandle(pAngular->m_hEndPart);
		CString tempStr;
		CStringArray str_arr;
		if(pStartPart==NULL||pEndPart==NULL)
			continue;
		tempStr.Format("0X%X",pAngular->m_hStartPart);
		str_arr.Add(tempStr);
		tempStr.Format("0X%X",pAngular->m_hEndPart);
		str_arr.Add(tempStr);
		tempStr.Format("%f",pAngular->m_fDimR);
		SimplifiedNumString(tempStr);
		str_arr.Add(tempStr);
		if(pAngular->m_bStartLineIsDatumLine)
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr.Add("Datum Edge");
		else 
			str_arr.Add("Weld Line");
#else
			str_arr.Add("基准边");
		else 
			str_arr.Add("焊道线");
#endif
		AppendRecord(-1,str_arr);
	}
	//存在基准钢管，且基准钢管有焊道线
	GetDlgItem(IDC_CHK_START_LINE_IS_WELD_ROAD)->EnableWindow(m_bHasWeldRoad);
	CDialog::OnInitDialog();
	return TRUE;
}

void CPartGroupAngularDlg::RefreshListViewHeader()
{
	LV_COLUMN lvc;
	int i, NUM_COLUMNS, nColumnCount;
	CHeaderCtrl *pHeader = m_pListCtrl->GetHeaderCtrl();
	nColumnCount = pHeader->GetItemCount();

	// Delete all of the columns.
	for (i=0;i < nColumnCount;i++)
	{
	   m_pListCtrl->DeleteColumn(0);
	}
	if(m_arrHeaderLabel.GetSize()>0)
		NUM_COLUMNS = m_arrHeaderLabel.GetSize();
	else
		NUM_COLUMNS = 0;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for(i = 0; i<NUM_COLUMNS; i++)
	{
		lvc.iSubItem = i;
		lvc.pszText = m_arrHeaderLabel[i].GetBuffer(1);//_gszColumnLabel[i];
		lvc.cx = 78;
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}

int CPartGroupAngularDlg::AppendRecord(int iItem, CStringArray &str_arr)
{
	//insert items
	LV_ITEM lvi;
	if(iItem==-1)
		iItem=m_pListCtrl->GetItemCount();

	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.pszText = str_arr[0].GetBuffer(1);//_gszItem[i][0];
	iItem = m_pListCtrl->InsertItem(&lvi);
	//set item text for additional columns
	int nCount = m_arrHeaderLabel.GetSize();
	nCount = __min(nCount,str_arr.GetSize());
	for(int j = 1; j<nCount; j++)
	{
		str_arr[j].TrimLeft();
		str_arr[j].TrimRight();
		m_pListCtrl->SetItemText(iItem,j,str_arr[j]);
	}
	m_pListCtrl->RedrawItems(iItem,iItem);
	return iItem;
}

void CPartGroupAngularDlg::OnAddAngular() 
{
	UpdateData();
	long hStart=0, hEnd=0;
	CString tempStr;
	CStringArray str_arr;
	sscanf(m_sStartPartHandle,"%X",&hStart);
	sscanf(m_sEndPartHandle,"%X",&hEnd);
	CLDSPart *pStartPart = console.FromPartHandle(hStart);
	CLDSPart *pEndPart = console.FromPartHandle(hEnd);
	if(pStartPart==NULL&&pEndPart==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("handle input is wrong,please input again and add!");
#else
		AfxMessageBox("输入了错误的句柄，请重新输入后再添加!");
#endif
		return;
	}
	if(pStartPart==NULL&&pEndPart)
	{	//确保起始构件存在
		pStartPart=pEndPart;
		pEndPart=NULL;
	}
	int nCount=m_pListCtrl->GetItemCount();
	for(int i=0;i<nCount;i++)
	{
		char sText[50];
		CString sStartPart, sEndPart;
		m_pListCtrl->GetItemText(i,0,sText,50);
		sStartPart.Format("%s",sText);
		m_pListCtrl->GetItemText(i,1,sText,50);
		sEndPart.Format("%s",sText);
		if(m_sStartPartHandle.CompareNoCase(sStartPart)==0&&
			m_sEndPartHandle.CompareNoCase(sEndPart)==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("the same mark line has existed, no permission to repeat adding!");
#else
			AfxMessageBox("已存在相同标注线，不能重复添加!");
#endif
			return;
		}
	}
	tempStr.Format("0X%X",pStartPart->handle);
	str_arr.Add(tempStr);
	if(pEndPart)
		tempStr.Format("0X%X",pEndPart->handle);
	else 
		tempStr.Format("0X%X",0);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fDimR);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	if(m_bStartLineIsWeldRoad)
#ifdef AFX_TARG_ENU_ENGLISH
		str_arr.Add("Weld Line");
	else 
		str_arr.Add("Datum Edge");
#else
		str_arr.Add("焊道线");
	else 
		str_arr.Add("基准边");
#endif
	AppendRecord(-1,str_arr);
}

void CPartGroupAngularDlg::OnModifyAngular() 
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		UpdateData();
		if(iCurSel>=0)
		{
			int nCount=m_pListCtrl->GetItemCount();
			for(int i=0;i<nCount;i++)
			{
				char sText[50];
				CString sStartPart, sEndPart;
				if(i==iCurSel)
					continue;
				m_pListCtrl->GetItemText(i,0,sText,50);
				sStartPart.Format("%s",sText);
				m_pListCtrl->GetItemText(i,1,sText,50);
				sEndPart.Format("%s",sText);
				if(m_sStartPartHandle.CompareNoCase(sStartPart)==0&&
					m_sEndPartHandle.CompareNoCase(sEndPart)==0)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("the same mark line has existed, no permission to repeat adding!");
#else
					AfxMessageBox("已存在相同标注线，不能重复添加!");
#endif
					return;
				}
			}
			//更新定位点列表
			CString ss;
			m_pListCtrl->SetItemText(iCurSel,0,m_sStartPartHandle);
			m_pListCtrl->SetItemText(iCurSel,1,m_sEndPartHandle);
			ss.Format("%f",m_fDimR);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,2,ss);
			if(m_bStartLineIsWeldRoad)
#ifdef AFX_TARG_ENU_ENGLISH
				ss="Weld Line";
			else
				ss="Datum Edge";
#else
				ss="焊道线";
			else
				ss="基准边";
#endif
			m_pListCtrl->SetItemText(iCurSel,3,ss);
		}
	}
}

void CPartGroupAngularDlg::OnSelectPart() 
{
	m_bPauseBreakExit=TRUE;
	OnOK(); 
}

static void DrawArcLineArrow(f3dArcLine arcLine)
{
	f3dLine line;
	f3dPoint norm,arrow_vec,temp_vec;
	f3dPoint startPt=arcLine.Start();
	f3dPoint endPt=arcLine.End();
	f3dPoint center=arcLine.Center();
	arrow_vec=startPt-center;
	temp_vec=endPt-center;
	normalize(arrow_vec);
	normalize(temp_vec);
	norm=arrow_vec^temp_vec;
	normalize(norm);	
	//设置不显示直线方向标志
	g_pSolidSet->SetDisplayLineVecMark(FALSE);
	temp_vec=endPt-center;
	normalize(temp_vec);
	arrow_vec.Set(-temp_vec.y,temp_vec.x);
	temp_vec=endPt-startPt;
	normalize(temp_vec);
	if(temp_vec*arrow_vec<0)
		arrow_vec*=-1.0;
	line.pen.width=arcLine.pen.width;
	line.pen.crColor=arcLine.pen.crColor;
	line.endPt=endPt;
	line.startPt=endPt-arrow_vec*5;
	rotate_point_around_axis(line.startPt,Pi*0.25,endPt,endPt+norm*100);
	g_pSolidDraw->NewLine(line);
	rotate_point_around_axis(line.startPt,-Pi*0.5,endPt,endPt+norm*100);
	g_pSolidDraw->NewLine(line);
	temp_vec=startPt-center;
	normalize(temp_vec);
	arrow_vec.Set(-temp_vec.y,temp_vec.x);
	temp_vec=startPt-endPt;
	normalize(temp_vec);
	if(temp_vec*arrow_vec<0)
		arrow_vec*=-1.0;
	line.endPt=startPt;
	line.startPt=startPt-arrow_vec*5;
	rotate_point_around_axis(line.startPt,Pi*0.25,startPt,startPt+norm*100);
	g_pSolidDraw->NewLine(line);
	rotate_point_around_axis(line.startPt,-Pi*0.5,startPt,startPt+norm*100);
	g_pSolidDraw->NewLine(line);
}

void CPartGroupAngularDlg::SelectPart(int nCurSel) 
{
	CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	char tem_str[100];
	long hStartPart=0, hEndPart=0;
	double fRadius=0;
	BOOL bStartLineIsDatumLine=TRUE;
	CLDSPart *pStartPart=NULL, *pEndPart=NULL;
	CLDSPlate *pStartPlate=NULL, *pEndPlate=NULL;
	CLDSLineTube *pStartTube=NULL, *pEndTube=NULL;
	CLDSLineSlot *pStartSlot=NULL, *pEndSlot=NULL;
	m_pListCtrl->GetItemText(nCurSel,0,tem_str,100);
	sscanf(tem_str,"%X",&hStartPart);
	m_pListCtrl->GetItemText(nCurSel,1,tem_str,100);
	sscanf(tem_str,"%X",&hEndPart);
	m_pListCtrl->GetItemText(nCurSel,2,tem_str,100);
	fRadius=atof(tem_str);
	m_pListCtrl->GetItemText(nCurSel,3,tem_str,100);
#ifdef AFX_TARG_ENU_ENGLISH
	if(strcmp(tem_str,"Datum Edge")==0)
		bStartLineIsDatumLine=TRUE;
	else if(strcmp(tem_str,"Weld Line")==0)
#else
	if(strcmp(tem_str,"基准边")==0)
		bStartLineIsDatumLine=TRUE;
	else if(strcmp(tem_str,"焊道线")==0)
#endif
		bStartLineIsDatumLine=FALSE;
	//选中当前构件
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(hStartPart);
	g_pSolidDraw->SetEntSnapStatus(hEndPart);
	pLDSView->UpdatePropertyPage();
	//显示角度标注线
	pStartPart=console.FromPartHandle(hStartPart);
	pEndPart=console.FromPartHandle(hEndPart);
	if(pStartPart==NULL&&pEndPart==NULL)
		return;
	if(pStartPart==NULL&&pEndPart) 
	{	//仅有一个基准构件时保证基准构件保存在pStartPart中
		pStartPart=pEndPart;
		pEndPart=NULL;
	}
	CLDSLineTube *pDatumLineTube=(CLDSLineTube*)console.FromPartHandle(pStartPart->m_hPartWeldParent,CLS_LINETUBE);
	if(pDatumLineTube==NULL)
		return;	//暂时仅支持显示焊接父构件为钢管的构件之间的夹角
	f3dPoint datum_line_vec=pDatumLineTube->End()-pDatumLineTube->Start();
	normalize(datum_line_vec);
	if(pStartPart&&pStartPart->GetClassTypeId()==CLS_PLATE)
		pStartPlate=(CLDSPlate*)pStartPart;
	else if(pStartPart&&pStartPart->GetClassTypeId()==CLS_LINETUBE)
		pStartTube=(CLDSLineTube*)pStartPart;
	else if(pStartPart&&pStartPart->GetClassTypeId()==CLS_LINESLOT)
		pStartSlot=(CLDSLineSlot*)pStartPart;
	if(pEndPart&&pEndPart->GetClassTypeId()==CLS_PLATE)
		pEndPlate=(CLDSPlate*)pEndPart;
	else if(pEndPart&&pEndPart->GetClassTypeId()==CLS_LINETUBE)
		pEndTube=(CLDSLineTube*)pEndPart;
	else if(pEndPart&&pEndPart->GetClassTypeId()==CLS_LINESLOT)
		pEndSlot=(CLDSLineSlot*)pEndPart;
	f3dPoint vertice,verify_vec,axis_x1,axis_x2,datum_edge_vec;
	f3dArcLine part_angle_line,datum_angle_line;	//构件之间的角度线 构件与基准边之间的角度线
	double weldRoadAngle=pDatumLineTube->CalWeldRoadAngle();
	f3dPoint weld_road_vec(cos(weldRoadAngle),sin(weldRoadAngle));
	vector_trans(weld_road_vec,pDatumLineTube->ucs,TRUE);	//由基准钢管相对坐标系转为全塔坐标系
	if(bStartLineIsDatumLine)//以基准边为基准
		datum_edge_vec=pDatumLineTube->ucs.axis_x;	
	else	//以焊道线为基准		
		datum_edge_vec=weld_road_vec;
	//标注钢板与钢板或钢板与基准边之间的角度线
	if(pStartPlate||pEndPlate)
	{	
		PROFILE_VERTEX *pVertex=NULL;
		if(pStartPlate)
		{	
			if(fabs(pStartPlate->ucs.axis_z*datum_line_vec)>EPS)
				return;	//基准钢板不是钢管轴向板
			if(pStartPlate->vertex_list.GetNodeNum()<=0)
				return;	//非法钢板
			axis_x1=pStartPlate->ucs.axis_z^datum_line_vec;
			normalize(axis_x1);
			//计算板重心位置
			for(pVertex=pStartPlate->vertex_list.GetFirst();pVertex;pVertex=pStartPlate->vertex_list.GetNext())
			{
				vertice=pVertex->vertex;
				coord_trans(vertice,pStartPlate->ucs,TRUE);
				verify_vec+=vertice;
			}
			//根据板的重心点计算板的延伸方向
			verify_vec/=pStartPlate->vertex_list.GetNodeNum();
			project_point(verify_vec,pStartPlate->ucs.origin,pStartPlate->ucs.axis_z);
			verify_vec-=pStartPlate->ucs.origin;
			normalize(verify_vec);
			//板延伸方向
			if(axis_x1*verify_vec<0)
				axis_x1*=-1.0;
		}
		if(pEndPlate)
		{	
			if(fabs(pEndPlate->ucs.axis_z*datum_line_vec)>EPS)
				return;	//基准钢板不是钢管轴向板
			if(pEndPlate->vertex_list.GetNodeNum()<=0)
				return;	//非法钢板
			axis_x2=pEndPlate->ucs.axis_z^datum_line_vec;
			normalize(axis_x2);
			verify_vec.Set();//初始化
			//计算板重心位置
			for(pVertex=pEndPlate->vertex_list.GetFirst();pVertex;pVertex=pEndPlate->vertex_list.GetNext())
			{
				vertice=pVertex->vertex;
				coord_trans(vertice,pEndPlate->ucs,TRUE);
				verify_vec+=vertice;
			}
			//根据板的重心点计算板的延伸方向
			verify_vec/=pEndPlate->vertex_list.GetNodeNum();
			project_point(verify_vec,pEndPlate->ucs.origin,pEndPlate->ucs.axis_z);
			verify_vec-=pEndPlate->ucs.origin;
			normalize(verify_vec);
			//板延伸方向
			if(axis_x2*verify_vec<0)
				axis_x2*=-1.0;
		}
		if(pStartPlate&&pEndPlate)
		{
			//标注两钢板之间的夹角
			part_angle_line.SetCenter(pStartPlate->ucs.origin);
			part_angle_line.SetStart(pStartPlate->ucs.origin+axis_x1*fRadius);
			part_angle_line.SetEnd(pStartPlate->ucs.origin+axis_x2*fRadius);
			part_angle_line.pen.crColor=RGB(255,0,0);	//红色
			part_angle_line.pen.width=2;	//线宽
			if(fabs(datum_edge_vec*axis_x1)<EPS_COS2&&fabs(datum_edge_vec*axis_x2)<EPS_COS2)
			{	//两钢板均不在基准边上,标注离基准边较近的钢板与基准边之间的角度
				datum_angle_line.SetCenter(pStartPlate->ucs.origin);
				datum_angle_line.SetStart(pStartPlate->ucs.origin+datum_edge_vec*fRadius);
				//终止边标记点
				verify_vec=axis_x1;
				double dd1=datum_edge_vec*axis_x1;
				double dd2=datum_edge_vec*axis_x2;
				//同号取余弦值小的 异号取余弦值为正的
				if((dd1*dd2>0&&dd1>dd2)||(dd1*dd2<0&&dd2>0))
					verify_vec=axis_x2;
				datum_angle_line.SetEnd(pStartPlate->ucs.origin+verify_vec*fRadius);
				datum_angle_line.pen.crColor=RGB(255,0,0);
				datum_angle_line.pen.width=2;
			}
		}
		else if(pStartPlate&&pEndPlate==NULL)
		{
			if(fabs(datum_edge_vec*axis_x1)<EPS_COS2)
			{	//钢板不在基准边上,标注钢板与基准边之间的角度
				datum_angle_line.SetCenter(pStartPlate->ucs.origin);
				datum_angle_line.SetStart(pStartPlate->ucs.origin+datum_edge_vec*fRadius);
				datum_angle_line.SetEnd(pStartPlate->ucs.origin+axis_x1*fRadius);
				datum_angle_line.pen.crColor=RGB(255,0,0);
				datum_angle_line.pen.width=2;
			}
		}
	}
	//标准两脚钉或者脚钉和基准边之间的夹角
	else if(pStartSlot||pEndSlot)
	{
		//脚钉法线方向
		f3dPoint dim_origin;
		CLDSBolt *pStartBolt=NULL,*pEndBolt=NULL;
		if(pStartSlot)
		{	
			pStartSlot->BuildUCS();
			if(pStartSlot->GetLsCount()!=1)
				return;
			pStartBolt=pStartSlot->GetFirstLsRef()->GetLsPtr();
			if(pStartBolt==NULL||(pStartBolt&&!pStartBolt->IsFootNail()))
				return;
			pStartBolt->GetUCS();
			axis_x1=pStartBolt->get_norm();
			normalize(axis_x1);
		}
		if(pEndSlot)
		{	
			pEndSlot->BuildUCS();
			if(pEndSlot->GetLsCount()!=1)
				return;
			pEndBolt=pEndSlot->GetFirstLsRef()->GetLsPtr();
			if(pEndBolt==NULL||(pEndBolt&&!pEndBolt->IsFootNail()))
				return;
			pEndBolt->GetUCS();
			axis_x2=pEndBolt->get_norm();
			normalize(axis_x2);
		}
		if(pStartBolt)
			SnapPerp(&dim_origin,pDatumLineTube->Start(),pDatumLineTube->End(),pStartBolt->ucs.origin);
		if(pStartBolt&&pEndBolt)
		{
			//标注两脚钉之间的夹角
			part_angle_line.SetCenter(dim_origin);
			part_angle_line.SetStart(dim_origin+axis_x1*fRadius);
			part_angle_line.SetEnd(dim_origin+axis_x2*fRadius);
			part_angle_line.pen.crColor=RGB(255,0,0);	//红色
			part_angle_line.pen.width=2;	//线宽
			if(fabs(datum_edge_vec*axis_x1)<EPS_COS2&&fabs(datum_edge_vec*axis_x2)<EPS_COS2)
			{	//两钢板均不在基准边上,标注离基准边较近的脚钉与基准边之间的角度
				datum_angle_line.SetCenter(dim_origin);
				datum_angle_line.SetStart(dim_origin+datum_edge_vec*fRadius);
				//终止边标记点
				verify_vec=axis_x1;
				double dd1=datum_edge_vec*axis_x1;
				double dd2=datum_edge_vec*axis_x2;
				//同号取余弦值小的 异号取余弦值为正的
				if((dd1*dd2>0&&dd1>dd2)||(dd1*dd2<0&&dd2>0))
					verify_vec=axis_x2;
				datum_angle_line.SetEnd(dim_origin+verify_vec*fRadius);
				datum_angle_line.pen.crColor=RGB(255,0,0);
				datum_angle_line.pen.width=2;
			}
		}
		else if(pStartBolt&&pEndBolt==NULL)
		{
			if(fabs(datum_edge_vec*axis_x1)<EPS_COS2)
			{	//脚钉不在基准边上,标注脚钉与基准边之间的角度
				datum_angle_line.SetCenter(dim_origin);
				datum_angle_line.SetStart(dim_origin+datum_edge_vec*fRadius);
				datum_angle_line.SetEnd(dim_origin+axis_x1*fRadius);
				datum_angle_line.pen.crColor=RGB(255,0,0);
				datum_angle_line.pen.width=2;
			}
		}
	}
	else if(pStartTube&&pEndTube)
	{
		//计算钢管延伸方向
		f3dPoint dim_origin;
		pStartTube->BuildUCS();
		axis_x1=pStartTube->End()-pStartTube->Start();
		normalize(axis_x1);
		pEndTube->BuildUCS();
		axis_x2=pEndTube->End()-pEndTube->Start();
		normalize(axis_x2);
		datum_line_vec=axis_x1^axis_x2;
		normalize(datum_line_vec);
		int nRet=Int3dll(pStartTube->Start(),pStartTube->End(),pEndTube->Start(),pEndTube->End(),dim_origin);
		if(nRet<=0)
			return;
		part_angle_line.SetCenter(dim_origin);
		part_angle_line.SetStart(dim_origin+axis_x1*fRadius);
		part_angle_line.SetEnd(dim_origin+axis_x2*fRadius);
		part_angle_line.pen.crColor=RGB(255,0,0);	//红色
		part_angle_line.pen.width=2;	//线宽
	}
	int sign=1;
	double sector_angle=0;
	if(part_angle_line.pen.width==2)
	{	//绘制圆弧
		f3dPoint startPt,endPt,center;
		startPt=part_angle_line.Start();
		endPt=part_angle_line.End();
		center=part_angle_line.Center();
		double half_len = 0.5*DISTANCE(startPt,endPt);
		if(fRadius<0)
			sign=-1;
		if((fabs(fRadius)-half_len)<EPS)
			sector_angle = Pi;
		else if(fabs(half_len/fRadius)<1)
			sector_angle = 2*asin(half_len/fabs(fRadius));
		//part_angle_line.CreateMethod2(startPt,endPt,f3dPoint(0,0,sign),sector_angle);
		part_angle_line.CreateMethod3(startPt,endPt,datum_line_vec,fRadius,center);
		g_pSolidDraw->NewArcLine(part_angle_line);
		//绘制箭头
		DrawArcLineArrow(part_angle_line);		
	}
	if(datum_angle_line.pen.width==2)
	{	//绘制圆弧
		f3dPoint startPt,endPt,center;
		startPt=datum_angle_line.Start();
		endPt=datum_angle_line.End();
		center=datum_angle_line.Center();
		double half_len = 0.5*DISTANCE(startPt,endPt);
		if(fRadius<0)
			sign=-1;
		if((fabs(fRadius)-half_len)<EPS)
			sector_angle = Pi;
		else if(fabs(half_len/fRadius)<1)
			sector_angle = 2*asin(half_len/fabs(fRadius));
		//datum_angle_line.CreateMethod2(startPt,endPt,f3dPoint(0,0,sign),sector_angle);
		datum_angle_line.CreateMethod3(startPt,endPt,datum_line_vec,fRadius,center);
		g_pSolidDraw->NewArcLine(datum_angle_line);
		//绘制箭头
		DrawArcLineArrow(datum_angle_line);		
	}
	g_pSolidDraw->Draw();
}

void CPartGroupAngularDlg::OnClickListAngular(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(iCurSel>=0)
			SelectPart(iCurSel);
	}
	*pResult = 0;
}

void CPartGroupAngularDlg::OnDblclkListAngular(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(iCurSel>=0)
			ModifyItem(iCurSel);
	}
	*pResult = 0;
}

void CPartGroupAngularDlg::OnKeydownListAngular(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_F3)
		{
			if(iCurSel>=0)
				ModifyItem(iCurSel);
		}
		else if(pLVKeyDow->wVKey==VK_DELETE)
		{
			while(iCurSel>=0)
			{
				m_pListCtrl->DeleteItem(iCurSel);
				pos=m_pListCtrl->GetFirstSelectedItemPosition();
				if(pos==NULL)
					break;
				iCurSel=m_pListCtrl->GetNextSelectedItem(pos);
			}
		}
	}
	*pResult = 0;
}

void CPartGroupAngularDlg::ModifyItem(int iCurSel)
{
	GetDlgItem(IDC_CHK_START_LINE_IS_WELD_ROAD)->EnableWindow(TRUE);
	char sText[50];
	m_pListCtrl->GetItemText(iCurSel,0,sText,50);
	m_sStartPartHandle.Format("%s",sText);
	m_pListCtrl->GetItemText(iCurSel,1,sText,50);
	m_sEndPartHandle.Format("%s",sText);
	m_pListCtrl->GetItemText(iCurSel,2,sText,50);
	m_fDimR=atof(sText);
	m_pListCtrl->GetItemText(iCurSel,5,sText,50);
	CString ss=sText;
#ifdef AFX_TARG_ENU_ENGLISH
	if(ss.CompareNoCase("Datum Edge")==0)
		m_bStartLineIsWeldRoad=FALSE;
	else if(ss.CompareNoCase("Weld Line")==0)
#else
	if(ss.CompareNoCase("基准边")==0)
		m_bStartLineIsWeldRoad=FALSE;
	else if(ss.CompareNoCase("焊道线")==0)
#endif
		m_bStartLineIsWeldRoad=TRUE;
	UpdateData(FALSE);
}

void CPartGroupAngularDlg::OnOK() 
{
	if(m_pWorkView && m_pWorkView->Angulars.GetNodeNum()>0)
		m_pWorkView->Angulars.Empty();
	int nCount = m_pListCtrl->GetItemCount();
	for(int i=0;i<nCount;i++)
	{
		char sText[100]="";
		long hStartPart=0, hEndPart=0;
		CAngularDimension *pAngular=NULL;
		m_pListCtrl->GetItemText(i,0,sText,100);
		sscanf(sText,"%X",&hStartPart);
		m_pListCtrl->GetItemText(i,1,sText,100);
		sscanf(sText,"%X",&hEndPart);
		CLDSPart *pStartPart=console.FromPartHandle(hStartPart);
		CLDSPart *pEndPart=console.FromPartHandle(hEndPart);
		if(pStartPart==NULL||pEndPart==NULL)
			continue;
		pAngular= m_pWorkView->Angulars.append();
		pAngular->m_hStartPart = pStartPart->handle;
		pAngular->m_hEndPart = pEndPart->handle;
		m_pListCtrl->GetItemText(i,2,sText,100);
		pAngular->m_fDimR=atof(sText);
		m_pListCtrl->GetItemText(i,3,sText,100);
		CString ss=sText;
#ifdef AFX_TARG_ENU_ENGLISH
		if(ss.CompareNoCase("Weld Line")==0)
			pAngular->m_bStartLineIsDatumLine=FALSE;
		else if(ss.CompareNoCase("Datum Edge")==0)
#else
		if(ss.CompareNoCase("焊道线")==0)
			pAngular->m_bStartLineIsDatumLine=FALSE;
		else if(ss.CompareNoCase("基准边")==0)
#endif
			pAngular->m_bStartLineIsDatumLine=TRUE;
	}
	CDialog::OnOK();
}

void CPartGroupAngularDlg::OnKillfocusStartPart() 
{
	UpdateData();
	char sText[100]="";
	long hStartPart=0, hEndPart=0;
	sscanf(m_sStartPartHandle,"%X",&hStartPart);
	sscanf(m_sEndPartHandle,"%X",&hEndPart);
	CLDSPart *pStartPart=console.FromPartHandle(hStartPart);
	CLDSPart *pEndPart=console.FromPartHandle(hEndPart);
	if(pStartPart==NULL||pEndPart==NULL)
	{
		m_bHasWeldRoad=FALSE;
		GetDlgItem(IDC_CHK_START_LINE_IS_WELD_ROAD)->EnableWindow(m_bHasWeldRoad);
		UpdateData(FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("starting parts and ending parts share different weld parent parts!");
#else
		AfxMessageBox("起始构件和终止构件焊接父构件不相同！");
#endif
		return;
	}
	CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(pStartPart->m_hPartWeldParent,CLS_LINETUBE);
	if(pLineTube&&pStartPart->m_hPartWeldParent==pEndPart->m_hPartWeldParent)
		m_bHasWeldRoad=pLineTube->m_bHasWeldRoad;
	else
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("starting parts and ending parts share different weld parent parts!");
#else
		AfxMessageBox("起始构件和终止构件焊接父构件不相同！");
#endif
	GetDlgItem(IDC_CHK_START_LINE_IS_WELD_ROAD)->EnableWindow(m_bHasWeldRoad);
	UpdateData(FALSE);
}

void CPartGroupAngularDlg::OnKillfocusEndPart() 
{
	UpdateData();
	char sText[100]="";
	long hStartPart=0, hEndPart=0;
	sscanf(m_sStartPartHandle,"%X",&hStartPart);
	sscanf(m_sEndPartHandle,"%X",&hEndPart);
	CLDSPart *pStartPart=console.FromPartHandle(hStartPart);
	CLDSPart *pEndPart=console.FromPartHandle(hEndPart);
	if(pStartPart==NULL||pEndPart==NULL)
	{
		m_bHasWeldRoad=FALSE;
		GetDlgItem(IDC_CHK_START_LINE_IS_WELD_ROAD)->EnableWindow(m_bHasWeldRoad);
		UpdateData(FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("starting parts and ending parts share different weld parent parts!");
#else
		AfxMessageBox("起始构件和终止构件焊接父构件不相同！");
#endif
		return;
	}
	CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(pStartPart->m_hPartWeldParent,CLS_LINETUBE);
	if(pLineTube&&pStartPart->m_hPartWeldParent==pEndPart->m_hPartWeldParent)
		m_bHasWeldRoad=pLineTube->m_bHasWeldRoad;
	else
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("starting parts and ending parts share different weld parent parts!");
#else
		AfxMessageBox("起始构件和终止构件焊接父构件不相同！");
#endif
	GetDlgItem(IDC_CHK_START_LINE_IS_WELD_ROAD)->EnableWindow(m_bHasWeldRoad);
	UpdateData(FALSE);
}
#endif