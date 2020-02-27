//<LOCALE_TRANSLATE Confirm by hxr/>
// DesignLjParaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "DesignLjParaDlg.h"
#include "DatumPointDlg.h"
#include "DesignVectorPropDlg.h"
//#include "DesignLsParaDlg.h"
//#include "DesignPadPlankParaDlg.h"
#include "DesignLjPartPropDlg.h"
#include "DesignLjLinePartParaDlg.h"
#include "DesignVertexParaDlg.h"
#include "ScrTools.h"
#include "env_def.h"
#include "Query.h"
#include "EditPlateDatumFaceDlg.h"
#include "DefHuoquFaceDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CDesignLjParaDlg dialog

CDesignLjParaDlg::CDesignLjParaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignLjParaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignLjParaDlg)
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_xListVertexes.AddColumnHeader("Serial",50);
	m_xListVertexes.AddColumnHeader("P.x",50);
	m_xListVertexes.AddColumnHeader("P.y",50);
	m_xListVertexes.AddColumnHeader("Rod",60);
	m_xListVertexes.AddColumnHeader("Location",80);
	m_xListVertexes.AddColumnHeader("State",40);
#else 
	m_xListVertexes.AddColumnHeader("序号",50);
	m_xListVertexes.AddColumnHeader("P.x",50);
	m_xListVertexes.AddColumnHeader("P.y",50);
	m_xListVertexes.AddColumnHeader("杆件",60);
	m_xListVertexes.AddColumnHeader("方位",80);
	m_xListVertexes.AddColumnHeader("状态",40);
#endif
	m_pPlate=NULL;
}


void CDesignLjParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignLjParaDlg)
	DDX_Control(pDX, IDC_LIST_VERTEXES, m_xListVertexes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignLjParaDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignLjParaDlg)
	ON_NOTIFY(NM_CLICK, IDC_LIST_VERTEXES, OnClickListVertex)
	ON_BN_CLICKED(IDC_BN_ENABLE, &CDesignLjParaDlg::OnBnClickedBnEnable)
	ON_BN_CLICKED(IDC_BN_DISABLE, &CDesignLjParaDlg::OnBnClickedBnDisable)
	ON_BN_CLICKED(IDC_BN_MOVE_UP, &CDesignLjParaDlg::OnBnClickedBnMoveUp)
	ON_BN_CLICKED(IDC_BN_MOVE_DOWN, &CDesignLjParaDlg::OnBnClickedBnMoveDwon)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignLjParaDlg message handlers

void InitLjParaListCtrlValue(CStringArray& str_arr,CDesignLjPartPara *pLinePartPara)
{
	if(str_arr.GetSize()!=6)
		str_arr.SetSize(6);
	CLDSPart *pPart=console.FromPartHandle(pLinePartPara->hPart);
	str_arr[0].Format("0X%X",pLinePartPara->hPart);
	if(pPart&&(pPart->GetClassTypeId()==CLS_LINETUBE||pPart->GetClassTypeId()==CLS_LINEANGLE||
		pPart->GetClassTypeId()==CLS_LINEFLAT||pPart->GetClassTypeId()==CLS_LINESLOT))
	{
		str_arr[1]=pPart->GetPartNo();
		str_arr[2].Format("%d",pLinePartPara->iFaceNo);
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[3].Format("%C leg",'X'+pLinePartPara->angle.cur_wing_x0_y1);
#else
			str_arr[3].Format("%C肢",'X'+pLinePartPara->angle.cur_wing_x0_y1);
#endif
		else
			str_arr[3]="";
		if(pLinePartPara->start0_end1==0)
#ifdef AFX_TARG_ENU_ENGLISH			
                        str_arr[4]="starting";
		else if(pLinePartPara->start0_end1==1)
			str_arr[4]="ending";
		else
			str_arr[4]="middle";
#else
			str_arr[4]="始端";
		else if(pLinePartPara->start0_end1==1)
			str_arr[4]="终端";
		else
			str_arr[4]="中间";
#endif
		str_arr[5]=pPart->GetPartTypeName();
	}
	else if(pPart==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH

		str_arr[1]="Invalid handle";
#else
		str_arr[1]="句柄无效";
#endif
	}
	else
	{
		if(pPart&&pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
#ifdef AFX_TARG_ENU_ENGLISH

			str_arr[1]="combined angle";
#else
			str_arr[1]="组合角钢";
#endif
			str_arr[2]=str_arr[3]=str_arr[4]=str_arr[5]="";
		}
		str_arr[1]=pPart->GetPartNo();
		str_arr[2].Format("%d",pLinePartPara->iFaceNo);
#ifdef AFX_TARG_ENU_ENGLISH
		str_arr[3].Format("%C leg",'X'+pLinePartPara->angle.cur_wing_x0_y1);
		if(pLinePartPara->start0_end1==0)
                        str_arr[4]="starting";
		else if(pLinePartPara->start0_end1==1)
			str_arr[4]="ending";
		else
			str_arr[4]="middle";
#else
		str_arr[3].Format("%C肢",'X'+pLinePartPara->angle.cur_wing_x0_y1);
		if(pLinePartPara->start0_end1==0)
			str_arr[4]="始端";
		else if(pLinePartPara->start0_end1==1)
			str_arr[4]="终端";
		else
			str_arr[4]="中间";
#endif
		str_arr[5]=pPart->GetPartTypeName();
	}
}

void InitVertexListCtrlValue(CStringArray& str_arr,CLDSPlate* pPlate,int serial,PROFILE_VERTEX* pVertex)
{
	if(str_arr.GetSize()!=6)
		str_arr.SetSize(6);
	str_arr[0].Format("%C",serial+'@');
	str_arr[1].Format("%.1f",pVertex->vertex.x);
	str_arr[2].Format("%.1f",pVertex->vertex.y);
	if(pVertex->m_hRod>0x20)
	{
		CDesignLjPartPara* pLjRod=NULL;
		for(pLjRod=pPlate->designInfo.partList.GetFirst();pLjRod;pLjRod=pPlate->designInfo.partList.GetNext())
		{
			if(pLjRod->hPart==pVertex->m_hRod)
				break;
		}
		str_arr[3].Format("0X%X",pVertex->m_hRod);
		str_arr[4]="用户定义";
		if(pLjRod&&pLjRod->start0_end1<=1&&pVertex->m_cPosDesignType<=PROFILE_VERTEX::DT_WING2)
		{
			if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER)
				str_arr[4]="楞线外侧";
			else if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING)
				str_arr[4]="肢翼外侧";
			else if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER2)
				str_arr[4]="楞线内侧";
			else if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING2)
				str_arr[4]="肢翼内侧";
		}
		else if(pVertex->m_cPosDesignType<=PROFILE_VERTEX::DT_WING2)
		{
			if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER)
				str_arr[4]="楞线终端";
			else if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING)
				str_arr[4]="肢翼终端";
			else if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER2)
				str_arr[4]="楞线始端";
			else if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING2)
				str_arr[4]="肢翼始端";
		}
	}
	str_arr[5]="保留";	//排除
}
BOOL CDesignLjParaDlg::OnInitDialog() 
{
	if(m_pPlate==NULL)
		return FALSE;
	m_pPlate->designInfo.CloneTo(m_xOldLjPara);	//备份设计参数用来判断设计参数是否修改过
	CDialog::OnInitDialog();
	m_xListVertexes.InitListCtrl();
	CStringArray str_arr;
	int serial=1;
	str_arr.SetSize(6);
	for(PROFILE_VERTEX* pVertex=m_pPlate->vertex_list.GetFirst(TRUE);pVertex;pVertex=m_pPlate->vertex_list.GetNext(TRUE))
	{
		InitVertexListCtrlValue(str_arr,m_pPlate,serial,pVertex);
		if(!m_pPlate->vertex_list.IsCursorDeleted())
		{
			str_arr[5]="保留";
			serial++;
		}
		else
		{
			str_arr[0]="";
			str_arr[5]="去除";
		}
		int index=m_xListVertexes.InsertItemRecord(-1,str_arr);
		m_xListVertexes.SetItemData(index,(DWORD)pVertex);
	}
	return TRUE;
}

void CDesignLjParaDlg::DesignPoint(CLDSPoint &point, char *sDescription)
{
	CDatumPointDlg des_point_dlg;
#ifdef AFX_TARG_ENU_ENGLISH
	des_point_dlg.m_shDatumPoint.Format("handle:no");
#else
	des_point_dlg.m_shDatumPoint.Format("句柄:无");
#endif
	des_point_dlg.m_sPointDescription =sDescription;
	des_point_dlg.m_pPoint=&point;
	des_point_dlg.DoModal();
}

void CDesignLjParaDlg::SelectPart(long hCurPart) 
{
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(hCurPart);
}

void CDesignLjParaDlg::OnClickListVertex(NMHDR* pNMHDR, LRESULT* pResult) 
{
	g_pSolidDraw->DestroyAllMark();
	POSITION pos = this->m_xListVertexes.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_xListVertexes.GetNextSelectedItem(pos);
		if(iCurSel>=0)
		{
			char tem_str[101]={0};
			m_xListVertexes.GetItemText(iCurSel,3,tem_str,100);
			SelectPart(HexStrToLong(tem_str));
			PROFILE_VERTEX* pVertex=(PROFILE_VERTEX*)m_xListVertexes.GetItemData(iCurSel);
			f3dPoint vertice=m_pPlate->GetRealVertex(pVertex->vertex);
			coord_trans(vertice,m_pPlate->ucs,TRUE);
			g_pSolidDraw->NewMark(vertice,10,AtomType::mkX);
		}
	}
	*pResult = 0;
}

void CDesignLjParaDlg::OnOK() 
{
	CXhChar100 itemText;
	BOOL modified=IsModifyDesignPara();
	PROFILE_VERTEX* pVertex;
	int i,count=m_xListVertexes.GetItemCount();
	DYN_ARRAY<PROFILE_VERTEX> vertexarr(count);
	for(i=0;i<count;i++)
	{
		m_xListVertexes.GetItemText(i,5,itemText,100);
		pVertex=(PROFILE_VERTEX*)m_xListVertexes.GetItemData(i);
		vertexarr[i]=*pVertex;
		if(!itemText.Equal("保留"))
			vertexarr[i].vertex.feature=-abs(vertexarr[i].vertex.feature);
	}
	m_pPlate->vertex_list.Empty();
	for(i=0;i<count;i++)
	{
		pVertex=m_pPlate->vertex_list.append(vertexarr[i]);
		if(pVertex->vertex.feature<0)
		{
			pVertex->vertex.feature*=-1;
			m_pPlate->vertex_list.DeleteCursor(FALSE);
		}
	}
	if(modified)//自动转换为仅更新坐标以保留外形计算结果 wjh-2015.8.26
		m_pPlate->designInfo.iProfileStyle0123=3;
	g_pSolidDraw->NewSolidPart(m_pPlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
	CDialog::OnOK();
}

BOOL CDesignLjParaDlg::IsModifyDesignPara()
{
	int i,count=m_xListVertexes.GetItemCount();
	CXhChar100 itemText;
	DYN_ARRAY<PROFILE_VERTEX> vertexarr(count);
	PROFILE_VERTEX* pVertex,*pLegacyVertex=m_pPlate->vertex_list.GetFirst(TRUE);
	for(i=0;i<count;i++)
	{
		pVertex=(PROFILE_VERTEX*)m_xListVertexes.GetItemData(i);
		m_xListVertexes.GetItemText(i,5,itemText,100);
		if(pVertex!=pLegacyVertex)
			return TRUE;
		if(itemText.Equal("保留")&&m_pPlate->vertex_list.IsCursorDeleted())
			return TRUE;
		else if(!itemText.Equal("保留")&&!m_pPlate->vertex_list.IsCursorDeleted())
			return TRUE;
		pLegacyVertex=m_pPlate->vertex_list.GetNext(TRUE);
		return TRUE;
	}
	return FALSE;
}

void CDesignLjParaDlg::OnBnClickedBnMoveUp()
{
	int iCurSel=-1,count=m_xListVertexes.GetItemCount();
	POSITION pos = this->m_xListVertexes.GetFirstSelectedItemPosition();
	if(pos!=NULL)
		iCurSel = m_xListVertexes.GetNextSelectedItem(pos);
	CLDSPlate plate;
	plate.SetBelongModel(m_pPlate->BelongModel());
	m_pPlate->CloneTo(plate);
	plate.handle=m_pPlate->handle;
	plate.vertex_list.Empty();
	CXhChar100 itemText;
	PROFILE_VERTEX* pVertex;
	ARRAY_LIST<PROFILE_VERTEX> vertexes(count);
	for(int i=0;i<count;i++)
	{
		PROFILE_VERTEX* pSwapVertex=(PROFILE_VERTEX*)m_xListVertexes.GetItemData(i);
		if(iCurSel!=-1&&i==iCurSel-1)
		{
			m_xListVertexes.GetItemText(i-1,5,itemText,100);
			pVertex=(PROFILE_VERTEX*)m_xListVertexes.GetItemData(i-1);
		}
		else
		{
			m_xListVertexes.GetItemText(i,5,itemText,100);
			pVertex=(PROFILE_VERTEX*)m_xListVertexes.GetItemData(i);
		}
		if(pSwapVertex!=pVertex)
		{
			for(int j=0;j<6;j++)
			{
				char text[101]={0};
				m_xListVertexes.GetItemText(i,j,text,100);
				m_xListVertexes.SetItemText(i,j,m_xListVertexes.GetItemText(i+1,j));
				m_xListVertexes.SetItemText(i+1,j,text);
			}
			DWORD dwData=m_xListVertexes.GetItemData(i);
			m_xListVertexes.SetItemData(i,m_xListVertexes.GetItemData(i+1));
			m_xListVertexes.SetItemData(i+1,dwData);
		}
		if(!itemText.Equal("保留"))
			continue;//pVertex->vertex.feature*=-1;
		plate.vertex_list.append(*pVertex);
	}
	plate.Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(plate.GetSolidPartObject());
	g_pSolidDraw->Draw();
}

void CDesignLjParaDlg::OnBnClickedBnMoveDwon()
{
	int iCurSel=-1,count=m_xListVertexes.GetItemCount();
	POSITION pos = this->m_xListVertexes.GetFirstSelectedItemPosition();
	if(pos!=NULL)
		iCurSel = m_xListVertexes.GetNextSelectedItem(pos);
	CLDSPlate plate;
	plate.SetBelongModel(m_pPlate->BelongModel());
	m_pPlate->CloneTo(plate);
	plate.handle=m_pPlate->handle;
	plate.vertex_list.Empty();
	CXhChar100 itemText;
	PROFILE_VERTEX* pVertex;
	ARRAY_LIST<PROFILE_VERTEX> vertexes(count);
	for(int i=0;i<count;i++)
	{
		PROFILE_VERTEX* pSwapVertex=(PROFILE_VERTEX*)m_xListVertexes.GetItemData(i);
		if(iCurSel!=-1&&i==iCurSel)
		{
			m_xListVertexes.GetItemText(i+1,5,itemText,100);
			pVertex=(PROFILE_VERTEX*)m_xListVertexes.GetItemData(i+1);
		}
		else
		{
			pVertex=(PROFILE_VERTEX*)m_xListVertexes.GetItemText(i,5,itemText,100);
			pVertex=(PROFILE_VERTEX*)m_xListVertexes.GetItemData(i);
		}
		if(pSwapVertex!=pVertex)
		{
			for(int j=0;j<6;j++)
			{
				char text[101]={0};
				m_xListVertexes.GetItemText(i,j,text,100);
				m_xListVertexes.SetItemText(i,j,m_xListVertexes.GetItemText(i+1,j));
				m_xListVertexes.SetItemText(i+1,j,text);
			}
			DWORD dwData=m_xListVertexes.GetItemData(i);
			m_xListVertexes.SetItemData(i,m_xListVertexes.GetItemData(i+1));
			m_xListVertexes.SetItemData(i+1,dwData);
		}
		if(!itemText.Equal("保留"))
			continue;//pVertex->vertex.feature*=-1;
		plate.vertex_list.append(*pVertex);
	}
	plate.Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(plate.GetSolidPartObject());
	g_pSolidDraw->Draw();
}

void CDesignLjParaDlg::OnBnClickedBnEnable()
{
	POSITION pos = this->m_xListVertexes.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_xListVertexes.GetNextSelectedItem(pos);
		if(iCurSel>=0)
			m_xListVertexes.SetItemText(iCurSel,5,"保留");
	}
	CLDSPlate plate;
	plate.SetBelongModel(m_pPlate->BelongModel());
	m_pPlate->CloneTo(plate);
	plate.handle=m_pPlate->handle;
	plate.vertex_list.Empty();
	CXhChar100 itemText;
	PROFILE_VERTEX* pVertex;
	int i,count=m_xListVertexes.GetItemCount();
	for(i=0;i<count;i++)
	{
		m_xListVertexes.GetItemText(i,5,itemText,100);
		pVertex=(PROFILE_VERTEX*)m_xListVertexes.GetItemData(i);
		if(!itemText.Equal("保留"))
			continue;//pVertex->vertex.feature*=-1;
		plate.vertex_list.append(*pVertex);
	}
	plate.Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(plate.GetSolidPartObject());
	g_pSolidDraw->Draw();
}

void CDesignLjParaDlg::OnBnClickedBnDisable()
{
	POSITION pos = this->m_xListVertexes.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_xListVertexes.GetNextSelectedItem(pos);
		if(iCurSel>=0)
			m_xListVertexes.SetItemText(iCurSel,5,"去除");
	}
	CLDSPlate plate;
	plate.SetBelongModel(m_pPlate->BelongModel());
	m_pPlate->CloneTo(plate);
	plate.handle=m_pPlate->handle;
	plate.vertex_list.Empty();
	CXhChar100 itemText;
	PROFILE_VERTEX* pVertex;
	int i,count=m_xListVertexes.GetItemCount();
	for(i=0;i<count;i++)
	{
		m_xListVertexes.GetItemText(i,5,itemText,100);
		pVertex=(PROFILE_VERTEX*)m_xListVertexes.GetItemData(i);
		if(!itemText.Equal("保留"))
			continue;//pVertex->vertex.feature*=-1;
		plate.vertex_list.append(*pVertex);
	}
	plate.Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(plate.GetSolidPartObject());
	g_pSolidDraw->Draw();
}
#endif
