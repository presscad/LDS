//<LOCALE_TRANSLATE Confirm by hxr/>
// TubeGuiGeLibDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "PartGuiGeLibDlg.h"
#include "GlobalFunc.h"
#include "database.h"
#include "XHcharString.h"
#include "env_def.h"
#include "SortFunc.h"
#include "./StdPartSolid/Fitting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartGuiGeLibDlg dialog
void CalParaValue(double D,double T,double& area,double& theroy_weight,double& I,double &r,double &W)
{
	double radius = D/2*0.1;
	area = Pi*radius*radius-Pi*(radius-T*0.1)*(radius-T*0.1);
	theroy_weight = CalSteelPlankWei(area*1000*100);
	double inner_d=(D-2*T)*0.1;
	I = (D*D*D*D*0.0001-inner_d*inner_d*inner_d*inner_d)*Pi/64;	//I=Pi(D^4-d^4),薄壁简化计算公式I=Pi*R^3*t,cm^4
	r = sqrt(I/area);
	W = I/D*2*10;
}

static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CPartGuiGeLibDlg *pTubeGuiGeLibDlg=(CPartGuiGeLibDlg*)pListCtrl->GetParent();
	if(pTubeGuiGeLibDlg==NULL)
		return FALSE;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	int nCurRow = pListCtrl->GetSelectedItem();
	int nCurCol = pListCtrl->GetCurSubItem();
	CRect rectItem;
	pListCtrl->GetItemRect(nCurRow,rectItem,LVIR_BOUNDS);
	if(!rectItem.PtInRect(point))
		return FALSE;	//鼠标不在当前选中项上
	if (nCurCol>=0)
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_PASTE_ITEM,"paste");
	if (nCurCol==0)
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"delete the specification");
	if (pTubeGuiGeLibDlg->m_iPartType==0&&nCurCol>=2)	//tube
		pMenu->AppendMenu(MF_STRING,ID_DAOWEI_SELECT_PART,"modify current column's attributes");
#else
		pMenu->AppendMenu(MF_STRING,ID_PASTE_ITEM,"粘贴");
	if (nCurCol==0)
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除此规格");
	if (pTubeGuiGeLibDlg->m_iPartType==0&&nCurCol>=2)	//钢管
		pMenu->AppendMenu(MF_STRING,ID_DAOWEI_SELECT_PART,"修正当前列属性");
#endif
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pTubeGuiGeLibDlg);
	return TRUE;
}

static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,
							int iSubItem,CString& sTextValue)
{
	CPartGuiGeLibDlg *pTubeGuiGeLibDlg=(CPartGuiGeLibDlg*)pListCtrl->GetParent();
	pTubeGuiGeLibDlg->m_bModified=TRUE;
	if(sTextValue=="0"&&iSubItem<2)
	{
		sTextValue = "";
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox(NULL,"data cannot be zero","ERROR",MB_OK);
#else
		MessageBox(NULL,"数据不能为0","ERROR",MB_OK);
#endif
		pListCtrl->SetSubItemText(pSelItem,iSubItem,sTextValue);
		return FALSE;
	}
	CString oldText = pSelItem->m_lpNodeInfo->GetSubItemText(iSubItem);
	if (oldText.CompareNoCase(sTextValue)==0)
		return FALSE;
	if (pTubeGuiGeLibDlg->m_iPartType == 6&&iSubItem==1)
	{
		//螺栓直径必须在螺栓规格表范围内（当前规格是12—48）
		if (atoi(sTextValue)<12||atoi(sTextValue)>48)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox(NULL,"bolt's diameter is not in the specified range!","ERROR",MB_OK);
#else
			MessageBox(NULL,"螺栓直径不在规定范围内!","ERROR",MB_OK);
#endif
			return FALSE;
		}
	}
	if(iSubItem>1&&pTubeGuiGeLibDlg->m_iPartType<3)
	{
		CString sSelectD = pSelItem->m_lpNodeInfo->GetSubItemText(0);
		CString sSelectT = pSelItem->m_lpNodeInfo->GetSubItemText(1);
		if (sSelectD == "" || sSelectT=="")
		{
			if (pTubeGuiGeLibDlg->m_iPartType==0)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"please input diameter and wall thickness firstly","ERROR",MB_OK);
			if (pTubeGuiGeLibDlg->m_iPartType==1)
				MessageBox(NULL,"please input leg width and leg thickness firstly","ERROR",MB_OK);
			if (pTubeGuiGeLibDlg->m_iPartType==2)
				MessageBox(NULL,"please input width and thickness firstly","ERROR",MB_OK);
#else
				MessageBox(NULL,"请先填写直径和壁厚","ERROR",MB_OK);
			if (pTubeGuiGeLibDlg->m_iPartType==1)
				MessageBox(NULL,"请先填写肢宽和肢厚","ERROR",MB_OK);
			if (pTubeGuiGeLibDlg->m_iPartType==2)
				MessageBox(NULL,"请先填写宽度和厚度","ERROR",MB_OK);
#endif
			return FALSE;
		}
	}
	int num = pListCtrl->GetItemCount();
		pListCtrl->SetSubItemText(pSelItem,iSubItem,sTextValue);
	//检验是否已经存在
	if (iSubItem==0 || iSubItem==1)
	{
		CString sSelectD = pSelItem->m_lpNodeInfo->GetSubItemText(0);
		CString sSelectT = pSelItem->m_lpNodeInfo->GetSubItemText(1);
		CString sSelectL = pSelItem->m_lpNodeInfo->GetSubItemText(2);
		if (sSelectD != "" && sSelectT!="")
		{
			for (int i=0;i<num-1;i++)//不和自己比较也不和最后的空数据比较
			{
				if (i==pSelItem->m_idProp-1)
					continue;
				CXhChar100 sD; 
				CXhChar100 sT;
				CXhChar100 sL;
				pListCtrl->GetItemText(i,0,sD,100);
				pListCtrl->GetItemText(i,1,sT,100);
				pListCtrl->GetItemText(i,2,sL,100);
				if(sSelectD.CompareNoCase(sD)==0&&sSelectT.CompareNoCase(sT)==0&&
					(pTubeGuiGeLibDlg->m_iPartType!=4||sSelectL.CompareNoCase(sL)==0))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"the specification already existed","ERROR",MB_OK);
#else
					MessageBox(NULL,"此规格已经存在","ERROR",MB_OK);
#endif
					CString sSelectD = "";
					pListCtrl->SetSubItemText(pSelItem,iSubItem,sSelectD);
					return FALSE;
				}
			}
			if (pTubeGuiGeLibDlg->m_iPartType==0)
			{
				//自动计算填写数据
				double D = atof(sSelectD);
				double T = atof(sSelectT);
				double area,theroy_weight,I,r,W;
				CalParaValue(D,T,area,theroy_weight,I,r,W);

				//area = ftoi(area*1000)/1000.0;
				oldText.Format("%.3f",area);
				SimplifiedNumString(oldText);
				pListCtrl->SetSubItemText(pSelItem,2,oldText);

				theroy_weight = ftoi(theroy_weight*100)/100.0;
				oldText.Format("%f",theroy_weight);
				SimplifiedNumString(oldText);
				pListCtrl->SetSubItemText(pSelItem,3,oldText);

				//I = ftoi(I*100)/100.0;	//对于大数转为整数时可能会溢出
				oldText.Format("%.2f",I);
				SimplifiedNumString(oldText);
				pListCtrl->SetSubItemText(pSelItem,4,oldText);
				
				r = ftoi(r*1000)/1000.0;
				oldText.Format("%f",r);
				SimplifiedNumString(oldText);
				pListCtrl->SetSubItemText(pSelItem,5,oldText);
				
				//W = ftoi(W*100)/100.0;
				oldText.Format("%.2f",W);
				SimplifiedNumString(oldText);
				pListCtrl->SetSubItemText(pSelItem,6,oldText);
			}
			if (pTubeGuiGeLibDlg->m_iPartType==2)
			{
				double D = atof(sSelectD);
				double T = atof(sSelectT);
				double area = D*T*0.01;
				double theroy_weight = CalSteelPlankWei(area*1000*100);
				theroy_weight = ftoi(theroy_weight*100)/100.0;
				oldText.Format("%f",theroy_weight);
				SimplifiedNumString(oldText);
				pListCtrl->SetSubItemText(pSelItem,2,oldText);
			}
		}
	}
	long nIndex = pSelItem->m_idProp-1;
	if (pSelItem->m_idProp==num && sTextValue!="")
	{
		//if (pTubeGuiGeLibDlg->m_iPartType==5)
		//	return TRUE;
		CSuperGridCtrl::CTreeItem* pItemArr;
		CString itemText = "";
		CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
		lpRoot->AddSubItemText(itemText,TRUE);
		CSuperGridCtrl::CTreeItem* pLastItem = pListCtrl->GetTreeItem(num-1);
		pItemArr = pListCtrl->InsertRootItem(lpRoot);
		pItemArr->m_idProp=num+1;
		int nItemNum = 0;
		if (pTubeGuiGeLibDlg->m_iPartType==0)
			nItemNum=7;
		else if (pTubeGuiGeLibDlg->m_iPartType==1)
			nItemNum = 11;
		else if (pTubeGuiGeLibDlg->m_iPartType==2)
			nItemNum = 3;
		else if (pTubeGuiGeLibDlg->m_iPartType==3)
			nItemNum = 14;
		else if (pTubeGuiGeLibDlg->m_iPartType==4)
			nItemNum = 7;
		else if (pTubeGuiGeLibDlg->m_iPartType==5)
			nItemNum = 8;
		for (int j=0;j<nItemNum;j++)
			pListCtrl->SetSubItemText(pItemArr,j,itemText);
		pItemArr->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
	}
	return TRUE;
}

void CPartGuiGeLibDlg::OnPasteData()
{
	m_bModified=TRUE;
	m_TubeLibList.PasteDataFromClipboard(7,NULL,TRUE);
}

void CPartGuiGeLibDlg::OnDelData()
{
	m_bModified=TRUE;
	int iCurItem = m_TubeLibList.GetSelectedItem();
	int nSumItem = m_TubeLibList.GetItemCount();
	if(nSumItem<2||nSumItem==iCurItem+1)
		return;
	for (int i = iCurItem;i<nSumItem;i++)
	{
		CSuperGridCtrl::CTreeItem* pItem = m_TubeLibList.GetTreeItem(i);
		pItem->m_idProp-=1;
	}
	m_TubeLibList.DeleteItem(iCurItem);
	m_TubeLibList.UpdateData();
}

CPartGuiGeLibDlg::CPartGuiGeLibDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPartGuiGeLibDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartGuiGeLibDlg)
	//}}AFX_DATA_INIT
	m_pLsFamily=NULL;
}


void CPartGuiGeLibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartGuiGeLibDlg)
	DDX_Control(pDX, IDC_CHECK, m_CheckBtn);
	DDX_Control(pDX, IDC_LIST_TUBE_LIB, m_TubeLibList);
	DDX_Control(pDX, IDC_BN_IMPORT, m_ImportBtn);
	DDX_Control(pDX, IDC_BN_EXPORT, m_ExportBtn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPartGuiGeLibDlg, CDialog)
	//{{AFX_MSG_MAP(CPartGuiGeLibDlg)
	ON_COMMAND(ID_PASTE_ITEM, OnPasteData)
	ON_COMMAND(ID_DELETE_ITEM, OnDelData)
	ON_BN_CLICKED(IDC_BN_IMPORT, OnBnImport)
	ON_BN_CLICKED(IDC_BN_EXPORT, OnBnExport)
	ON_BN_CLICKED(IDC_CANNEL, OnCannel)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_BN_CLICKED(ID_DAOWEI_SELECT_PART, OnRevisionTubePara)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartGuiGeLibDlg message handlers

BOOL CPartGuiGeLibDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_bModified=FALSE;
	if (m_iPartType==0)//钢管
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_TubeLibList.AddColumnHeader("diameter(mm)");
		m_TubeLibList.AddColumnHeader("wall thickness(mm)");
		m_TubeLibList.AddColumnHeader("Section Area(cm2)");	
		m_TubeLibList.AddColumnHeader("ρ(kg/m)");
#else
		m_TubeLibList.AddColumnHeader("直径(mm)");
		m_TubeLibList.AddColumnHeader("壁厚(mm)");
		m_TubeLibList.AddColumnHeader("截面积(cm2)");
		m_TubeLibList.AddColumnHeader("ρ(kg/m)");
#endif
#if defined(__TSA_)||defined(__LDS_)
#ifdef AFX_TARG_ENU_ENGLISH
		m_TubeLibList.AddColumnHeader("moment of inertia(cm4)");
		m_TubeLibList.AddColumnHeader("gyre radius(cm)");
		m_TubeLibList.AddColumnHeader("anti-bending section modulus(cm3)");
#else	
		m_TubeLibList.AddColumnHeader("惯性距(cm4)");
		m_TubeLibList.AddColumnHeader("回转半径(cm)");
		m_TubeLibList.AddColumnHeader("抗弯截面模量(cm3)");
#endif
#endif
	}
	else if (m_iPartType==1)//角钢
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_TubeLibList.AddColumnHeader("leg width(mm)");
		m_TubeLibList.AddColumnHeader("leg thickness(mm)");
		m_TubeLibList.AddColumnHeader("Section Area(cm2)");
		m_TubeLibList.AddColumnHeader("inside circle R(mm)");
		m_TubeLibList.AddColumnHeader("ρ(kg/m)");
#else
		m_TubeLibList.AddColumnHeader("肢宽(mm)");
		m_TubeLibList.AddColumnHeader("肢厚(mm)");
		m_TubeLibList.AddColumnHeader("截面积(cm2)");
		m_TubeLibList.AddColumnHeader("内圆R(mm)");
		m_TubeLibList.AddColumnHeader("ρ(kg/m)");
#endif
#if defined(__TSA_)||defined(__LDS_)
#ifdef AFX_TARG_ENU_ENGLISH
		m_TubeLibList.AddColumnHeader("type");
#else
		m_TubeLibList.AddColumnHeader("类型");
#endif
		m_TubeLibList.AddColumnHeader("Rx(cm)");
		m_TubeLibList.AddColumnHeader("Ryo(cm)");
		m_TubeLibList.AddColumnHeader("Ix(cm4)");
		m_TubeLibList.AddColumnHeader("Iyo(cm4)");
		m_TubeLibList.AddColumnHeader("Wx(cm3)");
		m_TubeLibList.AddColumnHeader("Wyo(cm3)");
#endif
	}
	else if (m_iPartType==2)//扁铁
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_TubeLibList.AddColumnHeader("width(mm)");
		m_TubeLibList.AddColumnHeader("thickness(mm)");
		m_TubeLibList.AddColumnHeader("ρ(kg/m)");
#else
		m_TubeLibList.AddColumnHeader("宽度(mm)");
		m_TubeLibList.AddColumnHeader("厚度(mm)");
		m_TubeLibList.AddColumnHeader("ρ(kg/m)");	
#endif
	}
	else if (m_iPartType==3)//槽钢
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_TubeLibList.AddColumnHeader("spec");
		m_TubeLibList.AddColumnHeader("section height(mm)");
		m_TubeLibList.AddColumnHeader("width(mm)");
		m_TubeLibList.AddColumnHeader("thick(mm)");
		m_TubeLibList.AddColumnHeader("insider circle R(mm)");
		m_TubeLibList.AddColumnHeader("section area(cm2)");
		m_TubeLibList.AddColumnHeader("anti-bending section modulus of the min axis(cm3)");
#else
		m_TubeLibList.AddColumnHeader("规格");
		m_TubeLibList.AddColumnHeader("截面高(mm)");
		m_TubeLibList.AddColumnHeader("宽度(mm)");
		m_TubeLibList.AddColumnHeader("厚度(mm)");
		m_TubeLibList.AddColumnHeader("内圆半径(mm)");
		m_TubeLibList.AddColumnHeader("截面积(cm2)");
		m_TubeLibList.AddColumnHeader("理论重量(kg)");
#endif
#if defined(__TSA_)||defined(__LDS_)
#ifdef AFX_TARG_ENU_ENGLISH
		m_TubeLibList.AddColumnHeader("Theoretical weight(kg)");
		m_TubeLibList.AddColumnHeader("Moment of inertia(cm4)");
		m_TubeLibList.AddColumnHeader("gyre radius(cm)");
		m_TubeLibList.AddColumnHeader("anti-bending section modulus(cm3)");
		m_TubeLibList.AddColumnHeader("Moment of inertia of the min axis(cm4)");
		m_TubeLibList.AddColumnHeader("gyre radius of the min axis(cm)");
#else
		m_TubeLibList.AddColumnHeader("惯性矩(cm4)");
		m_TubeLibList.AddColumnHeader("回转半径(cm)");
		m_TubeLibList.AddColumnHeader("抗弯截面模量(cm3)");
		m_TubeLibList.AddColumnHeader("最小轴惯性矩(cm4)");
		m_TubeLibList.AddColumnHeader("最小轴回转半径(cm)");
		m_TubeLibList.AddColumnHeader("最小轴抗弯截面模量(cm3)");
#endif
		m_TubeLibList.AddColumnHeader("Z0(cm)");
#endif
	}
	else if (m_iPartType==4)//螺栓
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_TubeLibList.AddColumnHeader("level");
		m_TubeLibList.AddColumnHeader("diameter(mm)");
		m_TubeLibList.AddColumnHeader("valid length(mm)");
		m_TubeLibList.AddColumnHeader("non-buckle longth(mm)");
		m_TubeLibList.AddColumnHeader("lower limit of LO(mm)");
		m_TubeLibList.AddColumnHeader("upper limit of LO(mm)");
		//m_TubeLibList.AddColumnHeader("thickness of washer(mm)");
		m_TubeLibList.AddColumnHeader("Theoretical weight(kg)");
#else
		m_TubeLibList.AddColumnHeader("级别");
		m_TubeLibList.AddColumnHeader("直径(mm)");
		m_TubeLibList.AddColumnHeader("有效长(mm)");
		m_TubeLibList.AddColumnHeader("无扣长(mm)");
		m_TubeLibList.AddColumnHeader("通厚下限(mm)");
		m_TubeLibList.AddColumnHeader("通厚上限(mm)");
		//m_TubeLibList.AddColumnHeader("垫圈厚度(mm)");
		m_TubeLibList.AddColumnHeader("理论重量(kg)");
#endif
		m_nLsXingHaoCount=m_pLsFamily->GetCount();
		for(int i=0;i<m_nLsXingHaoCount;i++)
			org_lsguige_table[i]=*m_pLsFamily->RecordAt(i);	
	}
	else if (m_iPartType==5)//螺栓间距库
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_TubeLibList.AddColumnHeader("bolt's diameter(mm)");
		m_TubeLibList.AddColumnHeader("single row space(mm)");
		m_TubeLibList.AddColumnHeader("double row space(mm)");
		m_TubeLibList.AddColumnHeader("end distance(mm)");
		m_TubeLibList.AddColumnHeader("connecting board margin(mm)");
		m_TubeLibList.AddColumnHeader("rolling margin≥(mm)");
		m_TubeLibList.AddColumnHeader("cut angle margin≥(mm)");
		m_TubeLibList.AddColumnHeader("washer space(mm)");
#else
		m_TubeLibList.AddColumnHeader("螺栓直径(mm)");
		m_TubeLibList.AddColumnHeader("单排间距(mm)");
		m_TubeLibList.AddColumnHeader("双排间距(mm)");
		m_TubeLibList.AddColumnHeader("端距(mm)");
		m_TubeLibList.AddColumnHeader("连板边距(mm)");
		m_TubeLibList.AddColumnHeader("轧制边距≥(mm)");
		m_TubeLibList.AddColumnHeader("切角边距≥(mm)");
		m_TubeLibList.AddColumnHeader("垫板边距(mm)");
#endif
	}
	else if(m_iPartType==6)//法兰螺栓选材表
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_TubeLibList.AddColumnHeader("scale");
		m_TubeLibList.AddColumnHeader("bolt's diameter(mm)");
		m_TubeLibList.AddColumnHeader("distance to tube wall≥(mm)");
		m_TubeLibList.AddColumnHeader("distance to flange out edge≥(mm)");
#else
		m_TubeLibList.AddColumnHeader("级别");
		m_TubeLibList.AddColumnHeader("螺栓直径(mm)");
		m_TubeLibList.AddColumnHeader("距管壁距离≥(mm)");
		m_TubeLibList.AddColumnHeader("距法兰外缘距离≥(mm)");
#endif
	}
	else if(m_iPartType==7)//角钢接头库
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_TubeLibList.AddColumnHeader("width of datum angle");
		m_TubeLibList.AddColumnHeader("thickness of datum angle");
		m_TubeLibList.AddColumnHeader("width of inner angle");
		m_TubeLibList.AddColumnHeader("thickness of inner angle");
		m_TubeLibList.AddColumnHeader("width of outer plate");
		m_TubeLibList.AddColumnHeader("thickness of outer plate");
		m_TubeLibList.AddColumnHeader("width of outer angle");
		m_TubeLibList.AddColumnHeader("thickness of outer angle");
#else
		m_TubeLibList.AddColumnHeader("基准角钢宽度");
		m_TubeLibList.AddColumnHeader("基准角钢厚度");
		m_TubeLibList.AddColumnHeader("内包角钢宽度");
		m_TubeLibList.AddColumnHeader("内包角钢厚度");
		m_TubeLibList.AddColumnHeader("外包钢板宽度");
		m_TubeLibList.AddColumnHeader("外包钢板厚度");
		m_TubeLibList.AddColumnHeader("外包角钢宽度");
		m_TubeLibList.AddColumnHeader("外包角钢厚度");
#endif
	}
	else if(m_iPartType==8)//H型钢
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_TubeLibList.AddColumnHeader("section height(mm)");
		m_TubeLibList.AddColumnHeader("width(mm)");
		m_TubeLibList.AddColumnHeader("thick(mm)");
		m_TubeLibList.AddColumnHeader("thick2(mm)");
		m_TubeLibList.AddColumnHeader("insider circle R(mm)");
		m_TubeLibList.AddColumnHeader("section area(cm2)");
		m_TubeLibList.AddColumnHeader("ρ(kg/m)");
#else
		m_TubeLibList.AddColumnHeader("高(mm)");
		m_TubeLibList.AddColumnHeader("宽(mm)");
		m_TubeLibList.AddColumnHeader("腹板厚度(mm)");
		m_TubeLibList.AddColumnHeader("翼缘厚度(mm)");
		m_TubeLibList.AddColumnHeader("内圆弧半径(mm)");
		m_TubeLibList.AddColumnHeader("截面面积(cm2)");
		m_TubeLibList.AddColumnHeader("ρ(kg/m)");
#endif	
#if defined(__TSA_)||defined(__LDS_)
		m_TubeLibList.AddColumnHeader("Ix(cm4)");
		m_TubeLibList.AddColumnHeader("Iyo(cm4)");
		m_TubeLibList.AddColumnHeader("Rx(cm)");
		m_TubeLibList.AddColumnHeader("Ryo(cm)");
		m_TubeLibList.AddColumnHeader("Wx(cm3)");
		m_TubeLibList.AddColumnHeader("Wyo(cm3)");
#endif
	}
	else if(m_iPartType==9)//地脚螺栓
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_TubeLibList.AddColumnHeader("d");
		m_TubeLibList.AddColumnHeader("hole d");
		m_TubeLibList.AddColumnHeader("nut high");
		m_TubeLibList.AddColumnHeader("nut end taper");
		m_TubeLibList.AddColumnHeader("nut outer diameter");
		m_TubeLibList.AddColumnHeader("nut inner diameter");
		m_TubeLibList.AddColumnHeader("bolt exposed length");
		m_TubeLibList.AddColumnHeader("gasket hole d increment");
		m_TubeLibList.AddColumnHeader("gasket length");
		m_TubeLibList.AddColumnHeader("gasket thick");
#else
		m_TubeLibList.AddColumnHeader("直径");
		m_TubeLibList.AddColumnHeader("底脚板孔径");
		m_TubeLibList.AddColumnHeader("螺母高度");
		m_TubeLibList.AddColumnHeader("螺母过渡面坡度");
		m_TubeLibList.AddColumnHeader("螺母外接圆直径");
		m_TubeLibList.AddColumnHeader("螺母内切圆直径");
		m_TubeLibList.AddColumnHeader("螺栓出露长");
		m_TubeLibList.AddColumnHeader("垫块孔径增大值");
		m_TubeLibList.AddColumnHeader("垫块宽度");
		m_TubeLibList.AddColumnHeader("垫块厚度");
#endif	
	}
	else if(m_iPartType==10)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_iPartSubType==0)
		{	//EB挂板
			m_TubeLibList.AddColumnHeader("size");
			m_TubeLibList.AddColumnHeader("b");
			m_TubeLibList.AddColumnHeader("S");
			m_TubeLibList.AddColumnHeader("M");
			m_TubeLibList.AddColumnHeader("Ф");
			m_TubeLibList.AddColumnHeader("L");
			m_TubeLibList.AddColumnHeader("H");
			m_TubeLibList.AddColumnHeader("R");
		}
		else if(m_iPartSubType==1)
		{	//GD挂板
			m_TubeLibList.AddColumnHeader("size");
			m_TubeLibList.AddColumnHeader("b");
			m_TubeLibList.AddColumnHeader("S");
			m_TubeLibList.AddColumnHeader("M");
			m_TubeLibList.AddColumnHeader("Ф");
			m_TubeLibList.AddColumnHeader("H");
			m_TubeLibList.AddColumnHeader("H1");
			m_TubeLibList.AddColumnHeader("H2");
			m_TubeLibList.AddColumnHeader("L");
			m_TubeLibList.AddColumnHeader("L1");
		}
#else
		if(m_iPartSubType==0)
		{	//EB挂板
			m_TubeLibList.AddColumnHeader("型号代码");
			m_TubeLibList.AddColumnHeader("挂板厚度");
			m_TubeLibList.AddColumnHeader("预留通厚");
			m_TubeLibList.AddColumnHeader("耳轴直径");
			m_TubeLibList.AddColumnHeader("挂孔直径");
			m_TubeLibList.AddColumnHeader("挂板宽度");
			m_TubeLibList.AddColumnHeader("挂孔到轴垂距");
			m_TubeLibList.AddColumnHeader("圆弧半径");
		}
		else if(m_iPartSubType==1)
		{	//GD挂板
			m_TubeLibList.AddColumnHeader("型号代码");
			m_TubeLibList.AddColumnHeader("挂板厚度");
			m_TubeLibList.AddColumnHeader("预留厚度");
			m_TubeLibList.AddColumnHeader("耳轴直径");
			m_TubeLibList.AddColumnHeader("挂孔直径");
			m_TubeLibList.AddColumnHeader("下圆弧到孔垂距");
			m_TubeLibList.AddColumnHeader("上圆弧到轴垂距");
			m_TubeLibList.AddColumnHeader("挂孔到轴垂距");
			m_TubeLibList.AddColumnHeader("耳轴总长");
			m_TubeLibList.AddColumnHeader("挂板宽度");
		}
#endif	
	}
	if (m_iPartType == 0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Tube Specification Library");
#else
		SetWindowText("钢管规格库");
#endif
#if defined(__TSA_)||defined(__LDS_)
		long col_wide_arr[12]={70,80,100,120,120,115,140};
#else
		long col_wide_arr[4]={70,80,100,120};
#endif
		m_TubeLibList.InitListCtrl(col_wide_arr);
		GetDlgItem(IDC_CHECK)->ShowWindow(SW_NORMAL);
	}
	else if (m_iPartType == 1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Angle Specification Library");
#else
		SetWindowText("角钢规格库");
#endif

#if defined(__TSA_)||defined(__LDS_)
#ifdef AFX_TARG_ENU_ENGLISH
		long col_wide_arr[12]={108,135,135,50,80,70,70,70,70,70,70,70};
#else
		long col_wide_arr[12]={70,70,95,80,50,80,70,70,70,70,70,70};
#endif
#else
#ifdef AFX_TARG_ENU_ENGLISH
		long col_wide_arr[5]={70,70,95,95,80};
#else 
		long col_wide_arr[5]={70,70,95,95,80};
#endif
#endif
		m_TubeLibList.InitListCtrl(col_wide_arr);
		m_CheckBtn.ShowWindow(HIDE_WINDOW);
		m_CheckBtn.HideCaret();
	}
	else if (m_iPartType == 2)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Flat Specification Library");
#else
		SetWindowText("扁铁规格库");
#endif
#if defined(__TSA_)||defined(__LDS_)
		long col_wide_arr[3]={70,80,100};
#else
		long col_wide_arr[3]={70,80,100};
#endif
		m_TubeLibList.InitListCtrl(col_wide_arr);
	}
	else if (m_iPartType == 3)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Slot Specification Library");
#else
		SetWindowText("槽钢规格库");
#endif
#if defined(__TSA_)||defined(__LDS_)
		long col_wide_arr[14]={70,80,100,120,120,115,140,120,120,120,120,120,120,120};
#else
		long col_wide_arr[7]={70,80,100,120,120,115,140};
#endif
		m_TubeLibList.InitListCtrl(col_wide_arr);
		m_CheckBtn.ShowWindow(HIDE_WINDOW);
		m_CheckBtn.HideCaret();
	}
	else if (m_iPartType == 4)
	{
		CString title;
#ifdef AFX_TARG_ENU_ENGLISH
		title.Format("%s Specification Library",m_pLsFamily->name);
#else
		title.Format("%s规格库",m_pLsFamily->name);
#endif
		SetWindowText(title);
#ifdef AFX_TARG_ENU_ENGLISH
		long col_wide_arr[7]={50,100,130,163,163,163,170};
#else 
		long col_wide_arr[7]={50,70,80,80,100,100,100};
#endif
		m_TubeLibList.InitListCtrl(col_wide_arr);
		m_CheckBtn.ShowWindow(HIDE_WINDOW);
		m_CheckBtn.HideCaret();
	}
	else if (m_iPartType == 5)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Bolt Distance Library");
#else
		SetWindowText("螺栓间距库");
#endif
#ifdef AFX_TARG_ENU_ENGLISH
		long col_wide_arr[8]={150,155,155,140,210,160,170,100};
#else 
		long col_wide_arr[8]={100,100,100,70,100,115,115,100};
#endif
		m_TubeLibList.InitListCtrl(col_wide_arr);
		m_CheckBtn.ShowWindow(HIDE_WINDOW);
		m_CheckBtn.HideCaret();
	}
	else if (m_iPartType == 6)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Flange Bolt Material List");
#else
		SetWindowText("法兰螺栓选材表");
#endif
		long col_wide_arr[4]={100,100,145,145};
		m_TubeLibList.InitListCtrl(col_wide_arr);
		m_CheckBtn.ShowWindow(HIDE_WINDOW);
		m_ImportBtn.ShowWindow(HIDE_WINDOW);
		m_ExportBtn.ShowWindow(HIDE_WINDOW);
		m_CheckBtn.HideCaret();
	}
	else if (m_iPartType == 7)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Angle Joint Library");
#else
		SetWindowText("角钢接头库");
#endif
		long col_wide_arr[8]={93,93,93,93,93,93,93,93};
		m_TubeLibList.InitListCtrl(col_wide_arr);
		m_CheckBtn.ShowWindow(HIDE_WINDOW);
		m_CheckBtn.HideCaret();
	}
	else if (m_iPartType == 8)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("H Specification Library");
#else
		SetWindowText("H型钢规格库");
#endif
#if defined(__TSA_)||defined(__LDS_)
		long col_wide_arr[13]={70,80,100,110,110,115,70,70,70,70,70,70,70};
#else
		long col_wide_arr[7]={70,80,100,110,110,115,70};
#endif
		m_TubeLibList.InitListCtrl(col_wide_arr);
		m_CheckBtn.ShowWindow(HIDE_WINDOW);
		m_CheckBtn.HideCaret();
	}
	else if (m_iPartType == 9)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Anchor Bolt Library");
#else
		SetWindowText("地脚螺栓库");
#endif
		long col_wide_arr[10]={70,90,85,110,110,110,90,110,85,85};
		m_TubeLibList.InitListCtrl(col_wide_arr);
		m_CheckBtn.ShowWindow(HIDE_WINDOW);
		m_CheckBtn.HideCaret();
	}
	else if(m_iPartType==10)
	{
		if(m_iPartSubType==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			SetWindowText("EB Fitting Library");
#else
			SetWindowText("EB挂板库");
#endif
			long col_wide_arr[8]={150,80,80,80,80,80,90,80};
			m_TubeLibList.InitListCtrl(col_wide_arr);
		}
		else if(m_iPartSubType==1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			SetWindowText("GD Fitting Library");
#else
			SetWindowText("GD挂板库");
#endif
			long col_wide_arr[10]={90,70,70,70,70,95,95,90,70,70};
			m_TubeLibList.InitListCtrl(col_wide_arr);
		}
		m_CheckBtn.ShowWindow(HIDE_WINDOW);
		m_CheckBtn.HideCaret();
	}
	m_TubeLibList.SetContextMenuFunc(FireContextMenu);
	m_TubeLibList.SetModifyValueFunc(FireValueModify);
	Refresh();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPartGuiGeLibDlg::OnBnImport() 
{
	CString sFileName;
	m_bModified=TRUE;
	if (m_iPartType==0)//钢管
	{
		CFileDialog dlg(TRUE,"pgt","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"tube specification file(*.pgt)|*.pgt||");
#else
			"钢管规格文件(*.pgt)|*.pgt||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			TubeGuiGeSerialize(sFileName,FALSE);	
		}
	}
	else if (m_iPartType==1)//角钢
	{
		CFileDialog dlg(TRUE,"jgt","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"angle specification file(*.jgt)|*.jgt||");
#else
			"角钢规格文件(*.jgt)|*.jgt||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			JgGuiGeSerialize(sFileName,FALSE);
		}
	}
	else if (m_iPartType==2)//扁铁
	{
		CFileDialog dlg(TRUE,"bia","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"angle specification file(*.bia)|*.bia||");
#else
			"角钢规格文件(*.bia)|*.bia||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			FlatGuiGeSerialize(sFileName,FALSE);	
		}
	}
	else if (m_iPartType==3)//槽钢
	{
		CFileDialog dlg(TRUE,"cgt","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"slot specification file(*.cgt)|*.cgt||");
#else
			"槽钢规格文件(*.cgt)|*.cgt||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			SlotGuiGeSerialize(sFileName,FALSE);	
		}
	}
	else if (m_iPartType==4)//螺栓规格
	{
		CFileDialog dlg(TRUE,"lst","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"bolt specification file(*.lst)|*.lst||");
#else
		"螺栓规格文件(*.lst)|*.lst||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			LsGuiGeSerialize(m_pLsFamily,sFileName,FALSE);	
		}
	}
	else if (m_iPartType==5)//螺栓间距
	{
		CFileDialog dlg(TRUE,"lss","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"bolt distance margin file(*.lss)|*.lss||");
#else
		"螺栓间距边距文件(*.lss)|*.lss||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			LsSpaceSerialize(sFileName,FALSE);	
		}
	}
	else if (m_iPartType==7)//角钢接头库
	{
		CFileDialog dlg(TRUE,"jjp","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"angle joint library file(*.jjp)|*.jjp||");
#else
			"角钢接头库文件(*.jjp)|*.jjp||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			JgJointParaSerialize(sFileName,FALSE);	
		}
	}
#ifdef __H_SHAPE_STEEL_
	else if (m_iPartType==8)//H型钢
	{
		if(m_iPartSubType==0)
		{
			CFileDialog dlg(TRUE,"hwgt","Tower",
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
				"HW specification file(*.hwgt)|*.hwgt||");
#else
				"HW型钢规格文件(*.hwgt)|*.hwgt||");
#endif
			if(dlg.DoModal()==IDOK)
			{
				sFileName = dlg.GetPathName();
				HShapeGuiGeSerialize(sFileName,m_iPartSubType,FALSE);	
			}
		}
		if(m_iPartSubType==1)
		{
			CFileDialog dlg(TRUE,"hmgt","Tower",
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
				"HM specification file(*.hmgt)|*.hmgt||");
#else
				"HM型钢规格文件(*.hmgt)|*.hmgt||");
#endif
			if(dlg.DoModal()==IDOK)
			{
				sFileName = dlg.GetPathName();
				HShapeGuiGeSerialize(sFileName,m_iPartSubType,FALSE);	
			}
		}
		if(m_iPartSubType==2)
		{
			CFileDialog dlg(TRUE,"hngt","Tower",
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
				"HN specification file(*.hngt)|*.hngt||");
#else
				"HN型钢规格文件(*.hngt)|*.hngt||");
#endif
			if(dlg.DoModal()==IDOK)
			{
				sFileName = dlg.GetPathName();
				HShapeGuiGeSerialize(sFileName,m_iPartSubType,FALSE);	
			}
		}
		if(m_iPartSubType==3)
		{
			CFileDialog dlg(TRUE,"hpgt","Tower",
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
				"HP specification file(*.hpgt)|*.hpgt||");
#else
				"HP型钢规格文件(*.hpgt)|*.hpgt||");
#endif
			if(dlg.DoModal()==IDOK)
			{
				sFileName = dlg.GetPathName();
				HShapeGuiGeSerialize(sFileName,m_iPartSubType,FALSE);	
			}
		}
	}
#endif
	else if (m_iPartType==9)//地脚螺栓库
	{
		CFileDialog dlg(TRUE,"abl","地脚螺栓库",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"anchor bolt library file(*.abl)|*.abl||");
#else
			"地脚螺栓库文件(*.abl)|*.abl||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			AnchorBoltParaSerialize(sFileName,FALSE);	
		}
	}
	else if(m_iPartType==10)	//连接金具库
	{
		if(m_iPartSubType==0)
		{	//EB挂板
			CFileDialog dlg(TRUE,"ebgt","Fitting",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"EB挂板规格文件(*.ebgt)|*.ebgt||");
			if(dlg.DoModal()==IDOK)
			{
				sFileName = dlg.GetPathName();
				EBFittingSerialize(sFileName,FALSE);
			}
		}
		else if(m_iPartSubType==1)
		{	//GD挂板
			CFileDialog dlg(TRUE,"gdgt","Fitting",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"GD挂板规格文件(*.gdgt)|*.gdgt||");
			if(dlg.DoModal()==IDOK)
			{
				sFileName = dlg.GetPathName();
				GDFittingSerialize(sFileName,FALSE);
			}
		}
	}
	Refresh();
}

void CPartGuiGeLibDlg::OnBnExport() 
{
	WriteDataBase();
	CString sFileName;
	if (m_iPartType==0)//钢管
	{
		CFileDialog dlg(FALSE,"pgt","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"tube specification file(*.pgt)|*.pgt||");
#else
			"钢管规格文件(*.pgt)|*.pgt||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			TubeGuiGeSerialize(sFileName,TRUE);	
		}
	}
	else if (m_iPartType==1)//角钢
	{
		CFileDialog dlg(FALSE,"jgt","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"angle specification file(*.jgt)|*.jgt||");
#else
			"角钢规格文件(*.jgt)|*.jgt||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			JgGuiGeSerialize(sFileName,TRUE);
		}
	}
	else if (m_iPartType==2)//扁铁
	{
		CFileDialog dlg(FALSE,"bia","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"angle specification file(*.bia)|*.bia||");
#else
			"角钢规格文件(*.bia)|*.bia||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			FlatGuiGeSerialize(sFileName,TRUE);	
		}
	}
	else if (m_iPartType==3)//槽钢
	{
		CFileDialog dlg(FALSE,"cgt","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"slot specification file(*.cgt)|*.cgt||");
#else
			"槽钢规格文件(*.cgt)|*.cgt||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			SlotGuiGeSerialize(sFileName,TRUE);	
		}
	}
	else if (m_iPartType==4)//螺栓规格
	{
		CFileDialog dlg(FALSE,"lst","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"bolt specification file(*.lst)|*.lst||");
#else
			"螺栓规格文件(*.lst)|*.lst||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			LsGuiGeSerialize(m_pLsFamily,sFileName,TRUE);	
		}
	}
	else if (m_iPartType==5)//螺栓间距
	{
		CFileDialog dlg(FALSE,"lss","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"bolt space margin specification file(*.lss)|*.lss||");
#else
			"螺栓间距边距文件(*.lss)|*.lss||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			LsSpaceSerialize(sFileName,TRUE);	
		}
	}
	else if (m_iPartType==7)//角钢接头库
	{
		CFileDialog dlg(FALSE,"jjp","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"angle joint library file(*.jjp)|*.jjp||");
#else
			"角钢接头库文件(*.jjp)|*.jjp||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			JgJointParaSerialize(sFileName,TRUE);	
		}
	}
#ifdef __H_SHAPE_STEEL_
	else if (m_iPartType==8)//H型钢
	{
		if(m_iPartSubType==0)
		{
			CFileDialog dlg(FALSE,"hwgt","Tower",
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
				"HW specification file(*.hwgt)|*.hwgt||");
#else
				"HW型钢规格文件(*.hwgt)|*.hwgt||");
#endif
			if(dlg.DoModal()==IDOK)
			{

				sFileName = dlg.GetPathName();
				HShapeGuiGeSerialize(sFileName,m_iPartSubType,TRUE);	
			}
		}
		if(m_iPartSubType==1)
		{
			CFileDialog dlg(FALSE,"hmgt","Tower",
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
				"HM specification file(*.hmgt)|*.hmgt||");
#else
				"HM型钢规格文件(*.hmgt)|*.hmgt||");
#endif
			if(dlg.DoModal()==IDOK)
			{

				sFileName = dlg.GetPathName();
				HShapeGuiGeSerialize(sFileName,m_iPartSubType,TRUE);	
			}
		}
		if(m_iPartSubType==2)
		{
			CFileDialog dlg(FALSE,"hngt","Tower",
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
				"HN specification file(*.hngt)|*.hngt||");
#else
				"HN型钢规格文件(*.hngt)|*.hngt||");
#endif
			if(dlg.DoModal()==IDOK)
			{

				sFileName = dlg.GetPathName();
				HShapeGuiGeSerialize(sFileName,m_iPartSubType,TRUE);	
			}
		}
		if(m_iPartSubType==3)
		{
			CFileDialog dlg(FALSE,"hpgt","Tower",
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
				"HP specification file(*.hpgt)|*.hpgt||");
#else
				"HP型钢规格文件(*.hpgt)|*.hpgt||");
#endif
			if(dlg.DoModal()==IDOK)
			{

				sFileName = dlg.GetPathName();
				HShapeGuiGeSerialize(sFileName,m_iPartSubType,TRUE);	
			}
		}
	}
#endif
	else if (m_iPartType==9)//地脚螺栓库
	{
		CFileDialog dlg(FALSE,"abl","地脚螺栓库",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"anchor bolt library file(*.abl)|*.abl||");
#else
			"地脚螺栓库文件(*.abl)|*.abl||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			AnchorBoltParaSerialize(sFileName,TRUE);	
		}
	}
	else if(m_iPartType==10)	//连接金具库
	{
		if(m_iPartSubType==0)
		{	//EB挂板
			CFileDialog dlg(FALSE,"ebgt","Fitting",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"EB挂板规格文件(*.ebgt)|*.ebgt||");
			if(dlg.DoModal()==IDOK)
			{
				sFileName = dlg.GetPathName();
				EBFittingSerialize(sFileName,TRUE);
			}
		}
		else if(m_iPartSubType==1)
		{	//GD挂板
			CFileDialog dlg(FALSE,"gdgt","Fitting",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"GD挂板库文件(*.gdgt)|*.gdgt||");
			if(dlg.DoModal()==IDOK)
			{
				sFileName = dlg.GetPathName();
				GDFittingSerialize(sFileName,TRUE);	
			}
		}
	}
}

BOOL CPartGuiGeLibDlg::Refresh()
{
	CString sText,sError;
	try
	{
		if (m_TubeLibList.GetItemCount()>0)
			m_TubeLibList.DeleteAllItems();
		IFittingLibrary<FITTING_EB>& gxarrEBLib=*FittingLibrary<FITTING_EB>();
		IFittingLibrary<FITTING_GD>& gxarrGDLib=*FittingLibrary<FITTING_GD>();
		IFittingLibrary<FITTING_UR>& gxarrURLib=*FittingLibrary<FITTING_UR>();
		//CSuperGridCtrl::CTreeItem* pItemArr[100]={NULL};
		CSuperGridCtrl::CTreeItem* pItem=NULL;
		int i=0;
		CString itemText;
		int nNum = 0;
		if (m_iPartType==0)
			nNum = tubeguige_N;
		else if (m_iPartType==1)
			nNum = jgguige_N;
		else if (m_iPartType==2)
			nNum = bianguige_N;
		else if (m_iPartType==3)
			nNum = slotguige_N;
		else if (m_iPartType==4)
			nNum = m_pLsFamily->GetCount();
		else if (m_iPartType==5)
			nNum = lsspace_N;
		else if (m_iPartType==6)
			nNum = g_sysPara.flSelectLsTlb_N;
		else if(m_iPartType==7)
			nNum = g_nJgJointRecordNum;
#ifdef __H_SHAPE_STEEL_
		else if(m_iPartType==8)
		{
			if(m_iPartSubType==0)
				nNum = hw_guige_N;
			else if(m_iPartSubType==1)
				nNum = hm_guige_N;
			else if(m_iPartSubType==2)
				nNum = hn_guige_N;
			else if(m_iPartSubType==3)
				nNum = hp_guige_N;
		}
#endif
		else if(m_iPartType==9)
			nNum=ANCHOR_BOLT::GetCount();
		else if(m_iPartType==10)
		{
			if(m_iPartSubType==0)
				nNum=gxarrEBLib.Count;
			else if(m_iPartSubType==1)
				nNum=gxarrGDLib.Count;
		}
		for (i=0;i<nNum;i++)
		{
#ifndef __PART_DESIGN_INC_
			if (theApp.m_bDrawingWorkEnvOnly&&m_iPartType==1&&jgguige_table[i].cType!='L')
				continue;	//角钢或绘图状态时不需要显示组合角钢信息　wjh-2015.2.26
#endif
			CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
			lpRoot->SetSubItemColor(0,RGB(192,192,192));
			lpRoot->SetSubItemColor(1,RGB(192,192,192));
			if (m_iPartType==0)
			{	
				//直径
				itemText.Format("%f",tubeguige_table[i].D);
				SimplifiedNumString(itemText);
				lpRoot->AddSubItemText(itemText,TRUE);
				pItem = m_TubeLibList.InsertRootItem(lpRoot);
				pItem->m_idProp=i+1;
				//壁厚
				itemText.Format("%f",tubeguige_table[i].thick);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,1,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//截面积
				itemText.Format("%f",tubeguige_table[i].area);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,2,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//理论重量
				itemText.Format("%f",tubeguige_table[i].theroy_weight);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,3,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
#if defined(__TSA_)||defined(__LDS_)
				//惯性距
				itemText.Format("%f",tubeguige_table[i].I);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,4,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//回转半径
				itemText.Format("%f",tubeguige_table[i].r);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,5,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//抗弯截面模量
				itemText.Format("%f",tubeguige_table[i].W);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,6,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
#endif
			}
			else if (m_iPartType==1)
			{
				//肢宽
				itemText.Format("%f",jgguige_table[i].wing_wide);
				SimplifiedNumString(itemText);
				lpRoot->AddSubItemText(itemText,FALSE);
				pItem = m_TubeLibList.InsertRootItem(lpRoot);
				pItem->m_idProp=i+1;
				//肢厚
				itemText.Format("%f",jgguige_table[i].wing_thick);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,1,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//截面面积(cm2)
				itemText.Format("%f",jgguige_table[i].area);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,2,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//内圆弧半径
				itemText.Format("%f",jgguige_table[i].r);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,3,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//理论重量
				itemText.Format("%f",jgguige_table[i].theroy_weight);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,4,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
#if defined(__TSA_)||defined(__LDS_)
				//类型
				itemText.Format("%c",jgguige_table[i].cType);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,5,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//平行轴回转半径
				itemText.Format("%f",jgguige_table[i].Rx);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,6,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//最小轴回转半径
				itemText.Format("%f",jgguige_table[i].Ry0);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,7,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//平行轴惯性距
				itemText.Format("%f",jgguige_table[i].Ix);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,8,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//最小轴惯性矩
				itemText.Format("%f",jgguige_table[i].Iy0);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,9,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//平行轴抗弯截面模量
				itemText.Format("%f",jgguige_table[i].Wx);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,10,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//最小轴抗弯截面模量
				itemText.Format("%f",jgguige_table[i].Wy0);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,11,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
#endif
			}
			else if (m_iPartType==2)
			{
				//宽度
				itemText.Format("%f",bianguige_table[i].wide);
				SimplifiedNumString(itemText);
				lpRoot->AddSubItemText(itemText,TRUE);
				pItem = m_TubeLibList.InsertRootItem(lpRoot);
				pItem->m_idProp=i+1;
				//壁厚
				itemText.Format("%f",bianguige_table[i].thick);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,1,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//理论重量
				itemText.Format("%f",bianguige_table[i].theroy_weight);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,2,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
			}
			else if (m_iPartType==3)
			{
				lpRoot->SetSubItemColor(2,RGB(192,192,192));
				lpRoot->SetSubItemColor(3,RGB(192,192,192));
				//槽钢规格
				itemText.Format("%s",slotguige_table[i].guige);
				SimplifiedNumString(itemText);
				lpRoot->AddSubItemText(itemText,TRUE);
				pItem = m_TubeLibList.InsertRootItem(lpRoot);
				pItem->m_idProp=i+1;
				//高
				itemText.Format("%f",slotguige_table[i].h);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,1,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//宽
				itemText.Format("%f",slotguige_table[i].b);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,2,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//厚
				itemText.Format("%f",slotguige_table[i].d);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,3,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//内圆弧半径
				itemText.Format("%f",slotguige_table[i].r);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,4,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//截面面积(cm2)
				itemText.Format("%f",slotguige_table[i].area);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,5,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//理论重量
				itemText.Format("%f",slotguige_table[i].theroy_weight);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,6,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
#if defined(__TSA_)||defined(__LDS_)	
				//水平惯性距
				itemText.Format("%f",slotguige_table[i].Ix);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,7,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//平行轴回转半径
				itemText.Format("%f",slotguige_table[i].Rx);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,8,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//平行轴抗弯截面模量
				itemText.Format("%f",slotguige_table[i].Wx);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,9,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//最小轴惯性矩
				itemText.Format("%f",slotguige_table[i].Iy);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,10,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//最小轴回转半径
				itemText.Format("%f",slotguige_table[i].Ry);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,11,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//最小轴抗弯截面模量
				itemText.Format("%f",slotguige_table[i].Wy);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,12,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//Z0
				itemText.Format("%f",slotguige_table[i].Z0);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,13,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
#endif
			}
			else if (m_iPartType==4)
			{
				lpRoot->SetSubItemColor(2,RGB(192,192,192));
				LS_XING_HAO *pXingHao=m_pLsFamily->RecordAt(i);
				itemText.Format("%f",pXingHao->grade);
				SimplifiedNumString(itemText);
				lpRoot->AddSubItemText(itemText,TRUE);
				pItem = m_TubeLibList.InsertRootItem(lpRoot);
				pItem->m_idProp=i+1;
		
				itemText.Format("%d",pXingHao->d);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,1,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				
				itemText.Format("%d",pXingHao->valid_len);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,2,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
			
				itemText.Format("%d",pXingHao->no_thread_len);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,3,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				
				itemText.Format("%d",pXingHao->down_lim);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,4,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				
				itemText.Format("%d",pXingHao->up_lim);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,5,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				
				/*itemText.Format("%d",pXingHao->washer_thick);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,6,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);*/
				
				itemText.Format("%f",pXingHao->weight);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,6,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
			}
			else if (m_iPartType==5)
			{
				LSSPACEEX boltSpaceEx;
				IBoltLibrary::GetLsSpaceEx(LSSPACE_TABLE[i].M_Diameter,&boltSpaceEx);
				lpRoot->SetSubItemColor(1,lpRoot->GetSubItemColor(5));
				itemText.Format("%d",LSSPACE_TABLE[i].M_Diameter);
				SimplifiedNumString(itemText);
				lpRoot->AddSubItemText(itemText,TRUE);
				pItem = m_TubeLibList.InsertRootItem(lpRoot);
				pItem->m_idProp=i+1;
				
				itemText.Format("%d",LSSPACE_TABLE[i].SingleRowSpace);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,1,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				
				itemText.Format("%d",LSSPACE_TABLE[i].doubleRowSpace);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,2,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				
				itemText.Format("%d",LSSPACE_TABLE[i].EndSpace);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,3,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				
				itemText.Format("%d",LSSPACE_TABLE[i].PlateEdgeSpace);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,4,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				
				itemText.Format("%d",LSSPACE_TABLE[i].RollingEdgeSpace);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,5,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				
				itemText.Format("%d",LSSPACE_TABLE[i].CutAngleSpace);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,6,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);	

				itemText.Format("%d",boltSpaceEx.WasherEdgeSpace);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,7,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);	
			}
			else if(m_iPartType==6)
			{
				itemText.Format("%f",g_sysPara.flSelectLs_table[i].grade);
				SimplifiedNumString(itemText);
				lpRoot->AddSubItemText(itemText,TRUE);
				lpRoot->SetControlType(0,GCT_CMB_LIST);
				lpRoot->SetListItemsStr(0, "4.8|6.8|8.8|10.9");
				pItem = m_TubeLibList.InsertRootItem(lpRoot);
				pItem->m_idProp=i+1;
				
				itemText.Format("%f",g_sysPara.flSelectLs_table[i].d);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,1,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				
				itemText.Format("%f",g_sysPara.flSelectLs_table[i].disTube);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,2,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				
				itemText.Format("%f",g_sysPara.flSelectLs_table[i].disFL);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,3,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
			}
			else if(m_iPartType==7)
			{
				//基准角钢宽度
				itemText.Format("%f",jg_joint_table[i].fBaseJgWidth);
				SimplifiedNumString(itemText);
				lpRoot->AddSubItemText(itemText,TRUE);
				pItem = m_TubeLibList.InsertRootItem(lpRoot);
				pItem->m_idProp=i+1;
				//基准角钢厚度
				itemText.Format("%f",jg_joint_table[i].fBaseJgThick);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,1,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//内包角钢宽度
				itemText.Format("%f",jg_joint_table[i].fInnerJgWidth);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,2,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//内包角钢厚度
				itemText.Format("%f",jg_joint_table[i].fInnerJgThick);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,3,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//外包钢板宽度
				itemText.Format("%f",jg_joint_table[i].fOuterPlateWidth);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,4,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//外包钢板厚度
				itemText.Format("%f",jg_joint_table[i].fOuterPlateThick);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,5,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//外包角钢宽度
				itemText.Format("%f",jg_joint_table[i].fOuterJgWidth);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,6,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//外包角钢厚度
				itemText.Format("%f",jg_joint_table[i].fOuterJgThick);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,7,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
			}
#ifdef __H_SHAPE_STEEL_
			else if (m_iPartType==8)
			{
				lpRoot->SetSubItemColor(2,RGB(192,192,192));
				lpRoot->SetSubItemColor(3,RGB(192,192,192));
				//高
				if(m_iPartSubType==0)
					itemText.Format("%f",hw_guige_table[i].h);
				else if(m_iPartSubType==1)
					itemText.Format("%f",hm_guige_table[i].h);
				else if(m_iPartSubType==2)
					itemText.Format("%f",hn_guige_table[i].h);
				else if(m_iPartSubType==3)
					itemText.Format("%f",hp_guige_table[i].h);
				SimplifiedNumString(itemText);
				lpRoot->AddSubItemText(itemText,TRUE);
				pItem = m_TubeLibList.InsertRootItem(lpRoot);
				pItem->m_idProp=i+1;
				//宽
				if(m_iPartSubType==0)
					itemText.Format("%f",hw_guige_table[i].b);
				else if(m_iPartSubType==1)
					itemText.Format("%f",hm_guige_table[i].b);
				else if(m_iPartSubType==2)
					itemText.Format("%f",hn_guige_table[i].b);
				else if(m_iPartSubType==3)
					itemText.Format("%f",hp_guige_table[i].b);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,1,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//腹板厚度
				if(m_iPartSubType==0)
					itemText.Format("%f",hw_guige_table[i].t1);
				else if(m_iPartSubType==1)
					itemText.Format("%f",hm_guige_table[i].t1);
				else if(m_iPartSubType==2)
					itemText.Format("%f",hn_guige_table[i].t1);
				else if(m_iPartSubType==3)
					itemText.Format("%f",hp_guige_table[i].t1);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,2,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//翼缘厚度

				if(m_iPartSubType==0)
					itemText.Format("%f",hw_guige_table[i].t2);
				else if(m_iPartSubType==1)
					itemText.Format("%f",hm_guige_table[i].t2);
				else if(m_iPartSubType==2)
					itemText.Format("%f",hn_guige_table[i].t2);
				else if(m_iPartSubType==3)
					itemText.Format("%f",hp_guige_table[i].t2);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,3,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//内圆弧半径
				if(m_iPartSubType==0)
					itemText.Format("%f",hw_guige_table[i].r);
				else if(m_iPartSubType==1)
					itemText.Format("%f",hm_guige_table[i].r);
				else if(m_iPartSubType==2)
					itemText.Format("%f",hn_guige_table[i].r);
				else if(m_iPartSubType==3)
					itemText.Format("%f",hp_guige_table[i].r);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,4,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//截面面积(cm2)
				if(m_iPartSubType==0)
					itemText.Format("%f",hw_guige_table[i].area);
				else if(m_iPartSubType==1)
					itemText.Format("%f",hm_guige_table[i].area);
				else if(m_iPartSubType==2)
					itemText.Format("%f",hn_guige_table[i].area);
				else if(m_iPartSubType==3)
					itemText.Format("%f",hp_guige_table[i].area);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,5,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//理论重量
				if(m_iPartSubType==0)
					itemText.Format("%f",hw_guige_table[i].theroy_weight);
				else if(m_iPartSubType==1)
					itemText.Format("%f",hm_guige_table[i].theroy_weight);
				else if(m_iPartSubType==2)
					itemText.Format("%f",hn_guige_table[i].theroy_weight);
				else if(m_iPartSubType==3)
					itemText.Format("%f",hp_guige_table[i].theroy_weight);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,6,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
#if defined(__TSA_)||defined(__LDS_)
				//平行轴惯性矩
				if(m_iPartSubType==0)
					itemText.Format("%f",hw_guige_table[i].Ix);
				else if(m_iPartSubType==1)
					itemText.Format("%f",hm_guige_table[i].Ix);
				else if(m_iPartSubType==2)
					itemText.Format("%f",hn_guige_table[i].Ix);
				else if(m_iPartSubType==3)
					itemText.Format("%f",hp_guige_table[i].Ix);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,7,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//最小轴惯性矩
				if(m_iPartSubType==0)
					itemText.Format("%f",hw_guige_table[i].Iy);
				else if(m_iPartSubType==1)
					itemText.Format("%f",hm_guige_table[i].Iy);
				else if(m_iPartSubType==2)
					itemText.Format("%f",hn_guige_table[i].Iy);
				else if(m_iPartSubType==3)
					itemText.Format("%f",hp_guige_table[i].Iy);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,10,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//平行轴回转半径
				if(m_iPartSubType==0)
					itemText.Format("%f",hw_guige_table[i].Rx);
				else if(m_iPartSubType==1)
					itemText.Format("%f",hm_guige_table[i].Rx);
				else if(m_iPartSubType==2)
					itemText.Format("%f",hn_guige_table[i].Rx);
				else if(m_iPartSubType==3)
					itemText.Format("%f",hp_guige_table[i].Rx);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,8,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//最小轴回转半径
				if(m_iPartSubType==0)
					itemText.Format("%f",hw_guige_table[i].Ry);
				else if(m_iPartSubType==1)
					itemText.Format("%f",hm_guige_table[i].Ry);
				else if(m_iPartSubType==2)
					itemText.Format("%f",hn_guige_table[i].Ry);
				else if(m_iPartSubType==3)
					itemText.Format("%f",hp_guige_table[i].Ry);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,11,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//平行轴抗弯截面模量
				if(m_iPartSubType==0)
					itemText.Format("%f",hw_guige_table[i].Wx);
				else if(m_iPartSubType==1)
					itemText.Format("%f",hm_guige_table[i].Wx);
				else if(m_iPartSubType==2)
					itemText.Format("%f",hn_guige_table[i].Wx);
				else if(m_iPartSubType==3)
					itemText.Format("%f",hp_guige_table[i].Wx);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,9,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//最小轴抗弯截面模量
				if(m_iPartSubType==0)
					itemText.Format("%f",hw_guige_table[i].Wy);
				else if(m_iPartSubType==1)
					itemText.Format("%f",hm_guige_table[i].Wy);
				else if(m_iPartSubType==2)
					itemText.Format("%f",hn_guige_table[i].Wy);
				else if(m_iPartSubType==3)
					itemText.Format("%f",hp_guige_table[i].Wy);
				SimplifiedNumString(itemText);
				m_TubeLibList.SetSubItemText(pItem,12,itemText);
				pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
#endif
			}
#endif
			else if(m_iPartType==9)
			{
				const ANCHOR_BOLT* pAnchorBolt=ANCHOR_BOLT::RecordAt(i);
				lpRoot->AddSubItemText(CXhChar50("%d",pAnchorBolt->d));
				lpRoot->AddSubItemText(CXhChar50("%d",pAnchorBolt->wiBasePlateHoleD));
				lpRoot->AddSubItemText(CXhChar50("%.2f",pAnchorBolt->m_fNutH));
				lpRoot->AddSubItemText(CXhChar50("%.2f",pAnchorBolt->m_fNutTaper));
				lpRoot->AddSubItemText(CXhChar50("%.2f",pAnchorBolt->m_fNutDe));
				lpRoot->AddSubItemText(CXhChar50("%.2f",pAnchorBolt->m_fNutDs));
				lpRoot->AddSubItemText(CXhChar50("%.1f",pAnchorBolt->m_fLe));
				lpRoot->AddSubItemText(CXhChar50("%d",pAnchorBolt->wiHoleIncrement));
				lpRoot->AddSubItemText(CXhChar50("%d",pAnchorBolt->wiWidth));
				lpRoot->AddSubItemText(CXhChar50("%d",pAnchorBolt->wiThick));
				pItem = m_TubeLibList.InsertRootItem(lpRoot);
				pItem->m_idProp=i+1;
			}
			else if(m_iPartType==10&&m_iPartSubType==0)
			{
				lpRoot->AddSubItemText(gxarrEBLib[i].szSizeCode);
				lpRoot->AddSubItemText(CXhChar50("%d",gxarrEBLib[i].siThick));
				lpRoot->AddSubItemText(CXhChar50("%d",gxarrEBLib[i].siThroughThick,TRUE));
				lpRoot->AddSubItemText(CXhChar50("%.1f",gxarrEBLib[i].dfTrunnionD));
				lpRoot->AddSubItemText(CXhChar50("%.1f",gxarrEBLib[i].dfHoleD));
				lpRoot->AddSubItemText(CXhChar50("%.1f",gxarrEBLib[i].dfBoardL));
				lpRoot->AddSubItemText(CXhChar50("%.1f",gxarrEBLib[i].dfBoardH));
				lpRoot->AddSubItemText(CXhChar50("%.1f",gxarrEBLib[i].dfChinR));
				//
				pItem=m_TubeLibList.InsertRootItem(lpRoot);
				pItem->m_idProp=i+1;
			}
			else if(m_iPartType==10&&m_iPartSubType==1)
			{
				lpRoot->AddSubItemText(gxarrGDLib[i].szSizeCode);
				lpRoot->AddSubItemText(CXhChar50("%d",gxarrGDLib[i].siThick));
				lpRoot->AddSubItemText(CXhChar50("%d",gxarrGDLib[i].siThroughThick,TRUE));
				lpRoot->AddSubItemText(CXhChar50("%.1f",gxarrGDLib[i].dfTrunnionD));
				lpRoot->AddSubItemText(CXhChar50("%.1f",gxarrGDLib[i].dfHoleD));
				lpRoot->AddSubItemText(CXhChar50("%.1f",gxarrGDLib[i].dfBoardH));
				lpRoot->AddSubItemText(CXhChar50("%.1f",gxarrGDLib[i].dfBoardH1));
				lpRoot->AddSubItemText(CXhChar50("%.1f",gxarrGDLib[i].dfBoardH2));
				lpRoot->AddSubItemText(CXhChar50("%.1f",gxarrGDLib[i].dfTrunnionL));
				lpRoot->AddSubItemText(CXhChar50("%.1f",gxarrGDLib[i].dfBoardL));
				//
				pItem=m_TubeLibList.InsertRootItem(lpRoot);
				pItem->m_idProp=i+1;
			}
		}
		//if (m_iPartType==5)
		//	return TRUE;
		itemText = "";
		CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
		lpRoot->AddSubItemText(itemText,TRUE);
		pItem = m_TubeLibList.InsertRootItem(lpRoot);
		pItem->m_idProp=i+1;
		int nNumItem = 0;
		if (m_iPartType==0)
			nNumItem = 7;
		else if (m_iPartType==1)
			nNumItem = 11;
		else if (m_iPartType==2)
			nNumItem = 3;
		else if (m_iPartType==3)
			nNumItem = 14;
		else if (m_iPartType==4)
			nNumItem = 7;
		else if (m_iPartType==5)
			nNumItem = 7;
		else if (m_iPartType==6)
			nNumItem = 4;
		else if (m_iPartType==7)
			nNumItem = 8 ;
		else if (m_iPartType==8)
			nNumItem = 13;
		else if(m_iPartType==9)
			nNumItem = 10;
		else if(m_iPartType==10&&m_iPartSubType==0)
			nNumItem = 8;
		else if(m_iPartType==10&&m_iPartSubType==1)
			nNumItem = 10;
		for (int j=0;j<nNumItem;j++)
		{
			if (m_iPartType==6)
			{
				lpRoot->SetControlType(0,GCT_CMB_LIST);
				lpRoot->SetListItemsStr(0, "4.8|6.8|8.8|10.9");
			}
			m_TubeLibList.SetSubItemText(pItem,j,itemText);
		}
		pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
		return FALSE;
	}
	return TRUE;
}

void CPartGuiGeLibDlg::CheckValue()
{
	int nIndex;
	BOOL bFindError = FALSE;
	COLORREF color = m_TubeLibList.GetTreeItem(m_TubeLibList.GetItemCount()-1)->m_lpNodeInfo->GetSubItemColor(0);
	for (nIndex=0;nIndex<m_TubeLibList.GetItemCount()-1;nIndex++)
	{
		CSuperGridCtrl::CTreeItem *pItem=m_TubeLibList.GetTreeItem(nIndex);
		CString sSelectD = pItem->m_lpNodeInfo->GetSubItemText(0);
		CString sSelectT = pItem->m_lpNodeInfo->GetSubItemText(1);
		
		if (m_iPartType==0)
		{	//钢管
			double D = atof(sSelectD);
			double T = atof(sSelectT);
			double area,theroy_weight,I,r,W;
			double area2,theroy_weight2,I2,r2,W2;
			CString realyValueText;
			CalParaValue(D,T,area,theroy_weight,I,r,W);
			area = ftoi(area*1000)/1000.0;
			realyValueText.Format("%f",area);
			SimplifiedNumString(realyValueText);
			CString sText = pItem->m_lpNodeInfo->GetSubItemText(2);
			area2=atof(sText);
	
			if (fabs(area-area2)>=0.001)//strcmp(sText,realyValueText) != 0)
			{
				pItem->m_lpNodeInfo->SetSubItemColor(2,RGB(255,255,0));
				bFindError = TRUE;
			}
			else if (pItem->m_lpNodeInfo->GetSubItemColor(2) != color)
				pItem->m_lpNodeInfo->SetSubItemColor(2,color);			
			theroy_weight = ftoi(theroy_weight*1000)/1000.0;
			realyValueText.Format("%f",theroy_weight);
			SimplifiedNumString(realyValueText);
			sText = pItem->m_lpNodeInfo->GetSubItemText(3);
			theroy_weight2=atof(sText);
			if (fabs(theroy_weight-theroy_weight2)>=0.001)//strcmp(sText,realyValueText) != 0)
			{
				pItem->m_lpNodeInfo->SetSubItemColor(3,RGB(255,255,0));
				bFindError = TRUE;
			}
			else if (pItem->m_lpNodeInfo->GetSubItemColor(3) != color)
				pItem->m_lpNodeInfo->SetSubItemColor(3,color);
			I = ftoi(I*1000)/1000.0;
			realyValueText.Format("%f",I);
			SimplifiedNumString(realyValueText);
			sText = pItem->m_lpNodeInfo->GetSubItemText(4);
			I2=atof(sText);
			if (fabs(I-I2)>=0.001)//strcmp(sText,realyValueText) != 0)
			{
				pItem->m_lpNodeInfo->SetSubItemColor(4,RGB(255,255,0));
				bFindError = TRUE;
			}
			else if (pItem->m_lpNodeInfo->GetSubItemColor(4) != color)
				pItem->m_lpNodeInfo->SetSubItemColor(4,color);
			r = ftoi(r*1000)/1000.0;
			realyValueText.Format("%f",r);
			SimplifiedNumString(realyValueText);
			sText = pItem->m_lpNodeInfo->GetSubItemText(5);
			r2=atof(sText);
			if (fabs(r-r2)>=0.001)//strcmp(sText,realyValueText) != 0)
			{
				bFindError = TRUE;
				pItem->m_lpNodeInfo->SetSubItemColor(5,RGB(255,255,0));
			}
			else if (pItem->m_lpNodeInfo->GetSubItemColor(5) != color)
				pItem->m_lpNodeInfo->SetSubItemColor(5,color);
			W = ftoi(W*1000)/1000.0;
			realyValueText.Format("%f",W);
			SimplifiedNumString(realyValueText);
			sText = pItem->m_lpNodeInfo->GetSubItemText(6);
			W2=atof(sText);
			if (fabs(W-W2)>=0.001)//strcmp(sText,realyValueText) != 0)
			{
				bFindError = TRUE;
				pItem->m_lpNodeInfo->SetSubItemColor(6,RGB(255,255,0));
			}
			else if (pItem->m_lpNodeInfo->GetSubItemColor(6) != color)
				pItem->m_lpNodeInfo->SetSubItemColor(6,color);
		}
		else if (m_iPartType==2)
		{	//扁钢
			double D = atof(sSelectD);
			double T = atof(sSelectT);
			double area = D*T*0.01;
			double theroy_weight = CalSteelPlankWei(area*1000*100);
			theroy_weight = ftoi(theroy_weight*100)/100.0;
			CString realyValueText;
			realyValueText.Format("%f",theroy_weight);
			SimplifiedNumString(realyValueText);
			CString sText = pItem->m_lpNodeInfo->GetSubItemText(2);
			if (strcmp(sText,realyValueText) != 0)
			{
				bFindError = TRUE;
				pItem->m_lpNodeInfo->SetSubItemColor(2,RGB(255,255,0));	
			}
			else if (pItem->m_lpNodeInfo->GetSubItemColor(2) != color)
				pItem->m_lpNodeInfo->SetSubItemColor(2,color);	
		}
		/*else if (m_iPartType==4)
		{	//螺栓规格表
			int d=0;
			CString sText=pItem->m_lpNodeInfo->GetSubItemText(1);
			d=atoi(sText);
			sText=pItem->m_lpNodeInfo->GetSubItemText(2);
			double fValidL=atof(sText);
			double D=0,H=0,H2=0;
			FindLsD_H(d,&D,&H,&H2);
			double area1=6*0.5*(0.5*D)*(sqrt(3)*D*0.5); //6*1/2*底*高
			double area2=Pi*(0.5*d)*(0.5*d);
			double weight=CalSteelPlankWei(area1*(H+H2)+area2*fValidL);
			sText=pItem->m_lpNodeInfo->GetSubItemText(6);
			double cur_weight=atof(sText);
			if(fabs(cur_weight-weight)>EPS)
			{
				bFindError = TRUE;
				pItem->m_lpNodeInfo->SetSubItemColor(6,RGB(255,0,0));
			}
		}*/
		m_TubeLibList.Redraw();
	}
	if(!bFindError)
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("check pass,no error information","check results",MB_OK);
#else
		MessageBox("校验通过,无错误信息","校验结果",MB_OK);
#endif
}

static int compareTubeSpecFunc(const TUBE_PARA_TYPE& item1,const TUBE_PARA_TYPE& item2)
{
	if(item1.D>item2.D)
		return 1;
	else if(item1.D<item2.D)
		return -1;
	else if(item1.thick>item2.thick)
		return 1;
	else if(item1.thick<item2.thick)
		return -1;
	return 0;
}

static int compareAngleSpecFunc(const JG_PARA_TYPE& item1,const JG_PARA_TYPE& item2)
{
	//对于类型为'L'的角钢一律放在前面，其他类型的角钢类字母大小排序
	if(item1.cType!='L'&&item2.cType=='L')
		return 1;
	else if(item1.cType=='L'&&item2.cType!='L')
		return -1;
	else if(item1.cType>item2.cType)
		return 1;
	else if(item1.cType<item2.cType)
		return -1;
	else if(item1.wing_wide>item2.wing_wide)
		return 1;
	else if(item1.wing_wide<item2.wing_wide)
		return -1;
	else if(item1.wing_thick>item2.wing_thick)
		return 1;
	else if(item1.wing_thick<item2.wing_thick)
		return -1;
	return 0;
}
static int compareFlatSpecFunc(const BIAN_PARA_TYPE& item1,const BIAN_PARA_TYPE& item2)
{
	if(item1.wide>item2.wide)
		return 1;
	else if(item1.wide<item2.wide)
		return -1;
	else if(item1.thick>item2.thick)
		return 1;
	else if(item1.thick<item2.thick)
		return -1;
	return 0;
}
void CPartGuiGeLibDlg::WriteDataBase()
{
	int nNum = m_TubeLibList.GetItemCount();
	if(m_iPartType==0)
	{
		tubeguige_N = nNum-1;
		for (int i=0;i<tubeguige_N;i++)
		{
			CXhChar100 sText;
			m_TubeLibList.GetItemText(i,0,sText,100);
			tubeguige_table[i].D = atof(sText);
			m_TubeLibList.GetItemText(i,1,sText,100);
			tubeguige_table[i].thick = atof(sText);
			m_TubeLibList.GetItemText(i,2,sText,100);
			tubeguige_table[i].area = atof(sText);
			m_TubeLibList.GetItemText(i,3,sText,100);
			tubeguige_table[i].theroy_weight = atof(sText);
			m_TubeLibList.GetItemText(i,4,sText,100);
			tubeguige_table[i].I = atof(sText);
			m_TubeLibList.GetItemText(i,5,sText,100);
			tubeguige_table[i].r = atof(sText);
			m_TubeLibList.GetItemText(i,6,sText,100);
			tubeguige_table[i].W = atof(sText);
		}
		CQuickSort<TUBE_PARA_TYPE>::QuickSort(tubeguige_table,tubeguige_N,compareTubeSpecFunc);
	}
	else if (m_iPartType==1)
	{
		jgguige_N = nNum-1;
		for (int i=0;i<jgguige_N;i++)
		{
			CXhChar100 sText;
			m_TubeLibList.GetItemText(i,0,sText,100);
			jgguige_table[i].wing_wide = atof(sText);
			m_TubeLibList.GetItemText(i,1,sText,100);
			jgguige_table[i].wing_thick = atof(sText);
			m_TubeLibList.GetItemText(i,2,sText,100);
			jgguige_table[i].area = atof(sText);
			m_TubeLibList.GetItemText(i,3,sText,100);
			jgguige_table[i].r = atof(sText);
			m_TubeLibList.GetItemText(i,4,sText,100);
			jgguige_table[i].theroy_weight = atof(sText);
#if defined(__TSA_)||defined(__LDS_)
			m_TubeLibList.GetItemText(i,5,sText,100);
			jgguige_table[i].cType = sText[0];
			m_TubeLibList.GetItemText(i,6,sText,100);
			jgguige_table[i].Rx = atof(sText);
			m_TubeLibList.GetItemText(i,7,sText,100);
			jgguige_table[i].Ry0 = atof(sText);
			m_TubeLibList.GetItemText(i,8,sText,100);
			jgguige_table[i].Ix = atof(sText);
			m_TubeLibList.GetItemText(i,9,sText,100);
			jgguige_table[i].Iy0 = atof(sText);
			m_TubeLibList.GetItemText(i,10,sText,100);
			jgguige_table[i].Wx = atof(sText);
			m_TubeLibList.GetItemText(i,11,sText,100);
			jgguige_table[i].Wy0 = atof(sText);
#else
			m_TubeLibList.GetItemText(i,5,sText,100);
			jgguige_table[i].Rx = atof(sText);
			m_TubeLibList.GetItemText(i,6,sText,100);
			jgguige_table[i].Ry0 = atof(sText);
			m_TubeLibList.GetItemText(i,7,sText,100);
			jgguige_table[i].Ix = atof(sText);
			m_TubeLibList.GetItemText(i,8,sText,100);
			jgguige_table[i].Iy0 = atof(sText);
			m_TubeLibList.GetItemText(i,9,sText,100);
			jgguige_table[i].Wx = atof(sText);
			m_TubeLibList.GetItemText(i,10,sText,100);
			jgguige_table[i].Wy0 = atof(sText);
#endif
		}
		CQuickSort<JG_PARA_TYPE>::QuickSort(jgguige_table,jgguige_N,compareAngleSpecFunc);
	}
	else if (m_iPartType==2)
	{
		bianguige_N = nNum-1;
		for (int i=0;i<bianguige_N;i++)
		{
			CXhChar100 sText;
			m_TubeLibList.GetItemText(i,0,sText,100);
			bianguige_table[i].wide = atof(sText);
			m_TubeLibList.GetItemText(i,1,sText,100);
			bianguige_table[i].thick = atof(sText);
			m_TubeLibList.GetItemText(i,2,sText,100);
			bianguige_table[i].theroy_weight = atof(sText);
		}
		CQuickSort<BIAN_PARA_TYPE>::QuickSort(bianguige_table,bianguige_N,compareFlatSpecFunc);
	}
	else if (m_iPartType==3)
	{
		slotguige_N = nNum-1;
		for (int i=0;i<slotguige_N;i++)
		{
			CXhChar100 sText;
			m_TubeLibList.GetItemText(i,0,sText,100);
			sprintf(slotguige_table[i].guige,"%s",(char*)sText);
			m_TubeLibList.GetItemText(i,1,sText,100);
			slotguige_table[i].h = atof(sText);
			m_TubeLibList.GetItemText(i,2,sText,100);
			slotguige_table[i].b = atof(sText);
			m_TubeLibList.GetItemText(i,3,sText,100);
			slotguige_table[i].d = atof(sText);
			m_TubeLibList.GetItemText(i,4,sText,100);
			slotguige_table[i].r = atof(sText);
			m_TubeLibList.GetItemText(i,5,sText,100);
			slotguige_table[i].area = atof(sText);
			m_TubeLibList.GetItemText(i,6,sText,100);
			slotguige_table[i].theroy_weight = atof(sText);
			m_TubeLibList.GetItemText(i,7,sText,100);
			slotguige_table[i].Ix = atof(sText);
			m_TubeLibList.GetItemText(i,8,sText,100);
			slotguige_table[i].Rx = atof(sText);
			m_TubeLibList.GetItemText(i,9,sText,100);
			slotguige_table[i].Wx = atof(sText);
			m_TubeLibList.GetItemText(i,10,sText,100);
			slotguige_table[i].Iy = atof(sText);
			m_TubeLibList.GetItemText(i,11,sText,100);
			slotguige_table[i].Ry = atof(sText);
			m_TubeLibList.GetItemText(i,12,sText,100);
			slotguige_table[i].Wy = atof(sText);
			m_TubeLibList.GetItemText(i,13,sText,100);
			slotguige_table[i].Z0 = atof(sText);
		}
	}
	else if (m_iPartType==4)
	{
		if(nNum-1<=0)
			return;
		m_pLsFamily->CreateFamily(nNum-1);
		for (int i=0;i<nNum-1;i++)
		{
			LS_XING_HAO *pXingHao=m_pLsFamily->RecordAt(i);
			CXhChar100 sText;
			m_TubeLibList.GetItemText(i,0,sText,100);
			pXingHao->grade=atof(sText);
			m_TubeLibList.GetItemText(i,1,sText,100);
			pXingHao->d = atoi(sText);
			m_TubeLibList.GetItemText(i,2,sText,100);
			pXingHao->valid_len = atoi(sText);
			m_TubeLibList.GetItemText(i,3,sText,100);
			pXingHao->no_thread_len = atoi(sText);
			m_TubeLibList.GetItemText(i,4,sText,100);
			pXingHao->down_lim = atoi(sText);
			m_TubeLibList.GetItemText(i,5,sText,100);
			pXingHao->up_lim = atoi(sText);
			/*m_TubeLibList.GetItemText(i,6,sText,100);
			pXingHao->washer_thick = atoi(sText);*/
			m_TubeLibList.GetItemText(i,6,sText,100);
			pXingHao->weight = atof(sText);
			sprintf(pXingHao->guige,"%.1fM%dX%d",pXingHao->grade,pXingHao->d,pXingHao->valid_len);
		}
	}
	else if (m_iPartType==5)
	{
		lsspace_N = nNum-1;
		for (int i=0;i<lsspace_N;i++)
		{
			CXhChar100 sText;
			m_TubeLibList.GetItemText(i,0,sText,100);
			LSSPACE_TABLE[i].M_Diameter = atoi(sText);
			m_TubeLibList.GetItemText(i,1,sText,100);
			LSSPACE_TABLE[i].SingleRowSpace = atoi(sText);
			m_TubeLibList.GetItemText(i,2,sText,100);
			LSSPACE_TABLE[i].doubleRowSpace = atoi(sText);
			m_TubeLibList.GetItemText(i,3,sText,100);
			LSSPACE_TABLE[i].EndSpace = atoi(sText);
			m_TubeLibList.GetItemText(i,4,sText,100);
			LSSPACE_TABLE[i].PlateEdgeSpace = atoi(sText);
			m_TubeLibList.GetItemText(i,5,sText,100);
			LSSPACE_TABLE[i].RollingEdgeSpace = atoi(sText);
			m_TubeLibList.GetItemText(i,6,sText,100);
			LSSPACE_TABLE[i].CutAngleSpace = atoi(sText);
			LSSPACEEX boltSpaceEx(LSSPACE_TABLE[i]);
			m_TubeLibList.GetItemText(i,7,sText,100);
			boltSpaceEx.WasherEdgeSpace = atoi(sText);
			IBoltLibrary::SetLsSpaceEx(boltSpaceEx.M_Diameter,boltSpaceEx);
		}
	}
	else if (m_iPartType==6)
	{
		g_sysPara.flSelectLsTlb_N = nNum-1;
		for (int i=0;i<g_sysPara.flSelectLsTlb_N;i++)
		{
			CXhChar100 sText;
			m_TubeLibList.GetItemText(i,0,sText,100);
			g_sysPara.flSelectLs_table[i].grade=atof(sText);
			m_TubeLibList.GetItemText(i,1,sText,100);
			g_sysPara.flSelectLs_table[i].d = atoi(sText);
			m_TubeLibList.GetItemText(i,2,sText,100);
			g_sysPara.flSelectLs_table[i].disTube = atoi(sText);
			m_TubeLibList.GetItemText(i,3,sText,100);
			g_sysPara.flSelectLs_table[i].disFL = atoi(sText);
		}
	}
	else if (m_iPartType==7)
	{
		g_nJgJointRecordNum = nNum-1;
		for (int i=0;i<g_nJgJointRecordNum;i++)
		{
			CXhChar100 sText;
			m_TubeLibList.GetItemText(i,0,sText,100);
			jg_joint_table[i].fBaseJgWidth = atof(sText);
			m_TubeLibList.GetItemText(i,1,sText,100);
			jg_joint_table[i].fBaseJgThick = atof(sText);
			m_TubeLibList.GetItemText(i,2,sText,100);
			jg_joint_table[i].fInnerJgWidth = atof(sText);
			m_TubeLibList.GetItemText(i,3,sText,100);
			jg_joint_table[i].fInnerJgThick = atof(sText);
			m_TubeLibList.GetItemText(i,4,sText,100);
			jg_joint_table[i].fOuterPlateWidth = atof(sText);
			m_TubeLibList.GetItemText(i,5,sText,100);
			jg_joint_table[i].fOuterPlateThick = atof(sText);
			m_TubeLibList.GetItemText(i,6,sText,100);
			jg_joint_table[i].fOuterJgWidth = atof(sText);
			m_TubeLibList.GetItemText(i,7,sText,100);
			jg_joint_table[i].fOuterJgThick = atof(sText);
		}
	}
#ifdef __H_SHAPE_STEEL_
	else if (m_iPartType==8)
	{
		if(m_iPartSubType==0)
		{
			hw_guige_N = nNum-1;
			for (int i=0;i<hw_guige_N;i++)
			{
				CXhChar100 sText;
				m_TubeLibList.GetItemText(i,0,sText,100);
				hw_guige_table[i].h = atof(sText);
				m_TubeLibList.GetItemText(i,1,sText,100);
				hw_guige_table[i].b = atof(sText);
				m_TubeLibList.GetItemText(i,2,sText,100);
				hw_guige_table[i].t1 = atof(sText);
				m_TubeLibList.GetItemText(i,3,sText,100);
				hw_guige_table[i].t2 = atof(sText);
				m_TubeLibList.GetItemText(i,4,sText,100);
				hw_guige_table[i].r = atof(sText);
				m_TubeLibList.GetItemText(i,5,sText,100);
				hw_guige_table[i].area = atof(sText);
				m_TubeLibList.GetItemText(i,6,sText,100);
				hw_guige_table[i].theroy_weight = atof(sText);
				m_TubeLibList.GetItemText(i,7,sText,100);
				hw_guige_table[i].Ix = atof(sText);
				m_TubeLibList.GetItemText(i,8,sText,100);
				hw_guige_table[i].Iy = atof(sText);
				m_TubeLibList.GetItemText(i,9,sText,100);
				hw_guige_table[i].Rx = atof(sText);
				m_TubeLibList.GetItemText(i,10,sText,100);
				hw_guige_table[i].Ry = atof(sText);
				m_TubeLibList.GetItemText(i,11,sText,100);
				hw_guige_table[i].Wx = atof(sText);
				m_TubeLibList.GetItemText(i,12,sText,100);
				hw_guige_table[i].Wy = atof(sText);
			}
		}
		else if(m_iPartSubType==1)
		{
			hm_guige_N = nNum-1;
			for (int i=0;i<hm_guige_N;i++)
			{
				CXhChar100 sText;
				m_TubeLibList.GetItemText(i,0,sText,100);
				hm_guige_table[i].h = atof(sText);
				m_TubeLibList.GetItemText(i,1,sText,100);
				hm_guige_table[i].b = atof(sText);
				m_TubeLibList.GetItemText(i,2,sText,100);
				hm_guige_table[i].t1 = atof(sText);
				m_TubeLibList.GetItemText(i,3,sText,100);
				hm_guige_table[i].t2 = atof(sText);
				m_TubeLibList.GetItemText(i,4,sText,100);
				hm_guige_table[i].r = atof(sText);
				m_TubeLibList.GetItemText(i,5,sText,100);
				hm_guige_table[i].area = atof(sText);
				m_TubeLibList.GetItemText(i,6,sText,100);
				hm_guige_table[i].theroy_weight = atof(sText);
				m_TubeLibList.GetItemText(i,7,sText,100);
				hm_guige_table[i].Ix = atof(sText);
				m_TubeLibList.GetItemText(i,8,sText,100);
				hm_guige_table[i].Iy = atof(sText);
				m_TubeLibList.GetItemText(i,9,sText,100);
				hm_guige_table[i].Rx = atof(sText);
				m_TubeLibList.GetItemText(i,10,sText,100);
				hm_guige_table[i].Ry = atof(sText);
				m_TubeLibList.GetItemText(i,11,sText,100);
				hm_guige_table[i].Wx = atof(sText);
				m_TubeLibList.GetItemText(i,12,sText,100);
				hm_guige_table[i].Wy = atof(sText);
			}
		}
		else if(m_iPartSubType==2)
		{
			hn_guige_N = nNum-1;
			for (int i=0;i<hn_guige_N;i++)
			{
				CXhChar100 sText;
				m_TubeLibList.GetItemText(i,0,sText,100);
				hn_guige_table[i].h = atof(sText);
				m_TubeLibList.GetItemText(i,1,sText,100);
				hn_guige_table[i].b = atof(sText);
				m_TubeLibList.GetItemText(i,2,sText,100);
				hn_guige_table[i].t1 = atof(sText);
				m_TubeLibList.GetItemText(i,3,sText,100);
				hn_guige_table[i].t2 = atof(sText);
				m_TubeLibList.GetItemText(i,4,sText,100);
				hn_guige_table[i].r = atof(sText);
				m_TubeLibList.GetItemText(i,5,sText,100);
				hn_guige_table[i].area = atof(sText);
				m_TubeLibList.GetItemText(i,6,sText,100);
				hn_guige_table[i].theroy_weight = atof(sText);
				m_TubeLibList.GetItemText(i,7,sText,100);
				hn_guige_table[i].Ix = atof(sText);
				m_TubeLibList.GetItemText(i,8,sText,100);
				hn_guige_table[i].Iy = atof(sText);
				m_TubeLibList.GetItemText(i,9,sText,100);
				hn_guige_table[i].Rx = atof(sText);
				m_TubeLibList.GetItemText(i,10,sText,100);
				hn_guige_table[i].Ry = atof(sText);
				m_TubeLibList.GetItemText(i,11,sText,100);
				hn_guige_table[i].Wx = atof(sText);
				m_TubeLibList.GetItemText(i,12,sText,100);
				hn_guige_table[i].Wy = atof(sText);
			}
		}
		if(m_iPartSubType==3)
		{
			hp_guige_N = nNum-1;
			for (int i=0;i<hp_guige_N;i++)
			{
				CXhChar100 sText;
				m_TubeLibList.GetItemText(i,0,sText,100);
				hp_guige_table[i].h = atof(sText);
				m_TubeLibList.GetItemText(i,1,sText,100);
				hp_guige_table[i].b = atof(sText);
				m_TubeLibList.GetItemText(i,2,sText,100);
				hp_guige_table[i].t1 = atof(sText);
				m_TubeLibList.GetItemText(i,3,sText,100);
				hp_guige_table[i].t2 = atof(sText);
				m_TubeLibList.GetItemText(i,4,sText,100);
				hp_guige_table[i].r = atof(sText);
				m_TubeLibList.GetItemText(i,5,sText,100);
				hp_guige_table[i].area = atof(sText);
				m_TubeLibList.GetItemText(i,6,sText,100);
				hp_guige_table[i].theroy_weight = atof(sText);
				m_TubeLibList.GetItemText(i,7,sText,100);
				hp_guige_table[i].Ix = atof(sText);
				m_TubeLibList.GetItemText(i,8,sText,100);
				hp_guige_table[i].Iy = atof(sText);
				m_TubeLibList.GetItemText(i,9,sText,100);
				hp_guige_table[i].Rx = atof(sText);
				m_TubeLibList.GetItemText(i,10,sText,100);
				hp_guige_table[i].Ry = atof(sText);
				m_TubeLibList.GetItemText(i,11,sText,100);
				hp_guige_table[i].Wx = atof(sText);
				m_TubeLibList.GetItemText(i,12,sText,100);
				hp_guige_table[i].Wy = atof(sText);
			}
		}
	}
#endif
	else if (m_iPartType==9)
	{
		int nCount=max(nNum-1,100);
		ANCHOR_BOLT::SetCount(nCount);
		ANCHOR_BOLT anchorBolt;
		for (int i=0;i<nCount;i++)
		{
			CXhChar100 sText;
			m_TubeLibList.GetItemText(i,0,sText,100);
			anchorBolt.d = atoi(sText);
			m_TubeLibList.GetItemText(i,1,sText,100);
			anchorBolt.wiBasePlateHoleD = atoi(sText);
			m_TubeLibList.GetItemText(i,2,sText,100);
			anchorBolt.m_fNutH = (float)atof(sText);
			m_TubeLibList.GetItemText(i,3,sText,100);
			anchorBolt.m_fNutTaper = (float)atof(sText);
			m_TubeLibList.GetItemText(i,4,sText,100);
			anchorBolt.m_fNutDe = (float)atof(sText);
			m_TubeLibList.GetItemText(i,5,sText,100);
			anchorBolt.m_fNutDs = (float)atof(sText);
			m_TubeLibList.GetItemText(i,6,sText,100);
			anchorBolt.m_fLe = (float)atof(sText);
			m_TubeLibList.GetItemText(i,7,sText,100);
			anchorBolt.wiHoleIncrement = atoi(sText);
			m_TubeLibList.GetItemText(i,8,sText,100);
			anchorBolt.wiWidth = atoi(sText);
			m_TubeLibList.GetItemText(i,9,sText,100);
			anchorBolt.wiThick = atoi(sText);
			ANCHOR_BOLT::SetAnchorBolt(i,anchorBolt);
		}
	}
	else if(m_iPartType==10)
	{
		IFittingLibrary<FITTING_EB>& gxarrEBLib=*FittingLibrary<FITTING_EB>();
		IFittingLibrary<FITTING_GD>& gxarrGDLib=*FittingLibrary<FITTING_GD>();
		IFittingLibrary<FITTING_UR>& gxarrURLib=*FittingLibrary<FITTING_UR>();
		if(m_iPartSubType==0)
		{
			gxarrEBLib.SetRecCount(nNum-1);
			for(int i=0;i<gxarrEBLib.Count;i++)
			{
				CXhChar100 sText;
				m_TubeLibList.GetItemText(i,0,sText,100);
				strcpy(gxarrEBLib[i].szSizeCode,sText);
				m_TubeLibList.GetItemText(i,1,sText,100);
				gxarrEBLib[i].siThick = atoi(sText);
				m_TubeLibList.GetItemText(i,2,sText,100);
				gxarrEBLib[i].siThroughThick = atoi(sText);
				m_TubeLibList.GetItemText(i,3,sText,100);
				gxarrEBLib[i].dfTrunnionD = atof(sText);
				m_TubeLibList.GetItemText(i,4,sText,100);
				gxarrEBLib[i].dfHoleD = atof(sText);
				m_TubeLibList.GetItemText(i,5,sText,100);
				gxarrEBLib[i].dfBoardL = atof(sText);
				m_TubeLibList.GetItemText(i,6,sText,100);
				gxarrEBLib[i].dfBoardH = atof(sText);
				m_TubeLibList.GetItemText(i,7,sText,100);
				gxarrEBLib[i].dfChinR = atof(sText);
			}
		}
		else if(m_iPartSubType==1)
		{
			gxarrGDLib.SetRecCount(nNum-1);
			for(int i=0;i<gxarrGDLib.Count;i++)
			{
				CXhChar100 sText;
				m_TubeLibList.GetItemText(i,0,sText,100);
				strcpy(gxarrGDLib[i].szSizeCode,sText);
				m_TubeLibList.GetItemText(i,1,sText,100);
				gxarrGDLib[i].siThick = atoi(sText);
				m_TubeLibList.GetItemText(i,2,sText,100);
				gxarrGDLib[i].siThroughThick = atoi(sText);
				m_TubeLibList.GetItemText(i,3,sText,100);
				gxarrGDLib[i].dfTrunnionD = atof(sText);
				m_TubeLibList.GetItemText(i,4,sText,100);
				gxarrGDLib[i].dfHoleD = atof(sText);
				m_TubeLibList.GetItemText(i,5,sText,100);
				gxarrGDLib[i].dfBoardH = atof(sText);
				m_TubeLibList.GetItemText(i,6,sText,100);
				gxarrGDLib[i].dfBoardH1 = atof(sText);
				m_TubeLibList.GetItemText(i,7,sText,100);
				gxarrGDLib[i].dfBoardH2 = atof(sText);
				m_TubeLibList.GetItemText(i,8,sText,100);
				gxarrGDLib[i].dfTrunnionL = atof(sText);
				m_TubeLibList.GetItemText(i,9,sText,100);
				gxarrGDLib[i].dfBoardL = atof(sText);
			}
		}
	}
}

void CPartGuiGeLibDlg::OnOK() 
{
	WriteDataBase();
	CDialog::OnOK();
}

void CPartGuiGeLibDlg::OnCannel() 
{
	if(m_bModified&&m_iPartType==4)
	{
		m_pLsFamily->CreateFamily(m_nLsXingHaoCount);
		for(int i=0;i<m_nLsXingHaoCount;i++)
			*m_pLsFamily->RecordAt(i)=org_lsguige_table[i];	
	}
	CDialog::OnCancel();
}
void CPartGuiGeLibDlg::OnRevisionTubePara()
{
	POSITION pos=m_TubeLibList.GetFirstSelectedItemPosition();
	int iCol=m_TubeLibList.GetCurSubItem();
	while(pos)
	{
		int iRow=m_TubeLibList.GetNextSelectedItem(pos);
		CXhChar16 diaStr,thickStr,resultStr;
		double result=0;
		m_TubeLibList.GetItemText(iRow,0,diaStr,17);
		m_TubeLibList.GetItemText(iRow,1,thickStr,17);
		double D=atof(diaStr);
		double T=atof(thickStr);
		double area,theroy_weight,I,r,W;
		CalParaValue(D,T,area,theroy_weight,I,r,W);
		if(iCol==2)//截面积
			resultStr.Printf("%.3f",area);
		else if(iCol==3)	//密度kg/m
			resultStr.Printf("%.3f",theroy_weight);
		else if(iCol==4)	//惯性矩,cm4
			resultStr.Printf("%.3f",I);
		else if(iCol==5)	//回转半径, cm
			resultStr.Printf("%.3f",r);
		else if(iCol==6)	//抗弯截面模量,cm3
			resultStr.Printf("%.3f",W);
		CSuperGridCtrl::CTreeItem *pItem=m_TubeLibList.GetTreeItem(iRow);
		m_TubeLibList.SetSubItemText(pItem,iCol,(char*)resultStr);
		//m_TubeLibList.SetItemText(iRow,iCol,(char*)resultStr);如果这样赋值只是更新了界面未更新实际规格库数据 wjh-2013.6.5
	}
}
void CPartGuiGeLibDlg::OnCheck() 
{
	CheckValue();
}
void CPartGuiGeLibDlg::OnSize(UINT nType, int cx, int cy)
{		
	RECT rect;
	int wide,high,x,y;
	CWnd* pListWnd=CWnd::GetDlgItem(IDC_LIST_TUBE_LIB);
	if(pListWnd->GetSafeHwnd()!=NULL)
	{
		pListWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		x=cx-rect.right;
		y=cy-rect.bottom-40;
		rect.right=cx;
		rect.bottom=cy-40;
		pListWnd->MoveWindow(&rect);	
	}
	CWnd* pStaWnd=CWnd::GetDlgItem(IDC_S_LIB_TYPE);
	if(pStaWnd->GetSafeHwnd()!=NULL)
	{
		pStaWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		wide=rect.right-rect.left;
		high=rect.bottom-rect.top;
		rect.right+=x;
		rect.left=rect.right-wide;
		if(rect.bottom+y>=40)
		{
			rect.bottom+=y;
			rect.top=rect.bottom-high;
		}
		if(rect.left<rect.right&&rect.top<rect.bottom)
			pStaWnd->MoveWindow(&rect);	
	}
	CWnd* pCmbBoltWnd=CWnd::GetDlgItem(IDC_CMB_BOLT_LIB_TYPE);
	if(pCmbBoltWnd->GetSafeHwnd()!=NULL)
	{
		pCmbBoltWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		wide=rect.right-rect.left;
		high=rect.bottom-rect.top;
		rect.right+=x;
		rect.left=rect.right-wide;
		if(rect.bottom+y>=40)
		{
			rect.bottom+=y;
			rect.top=rect.bottom-high;
		}
		if(rect.left<rect.right&&rect.top<rect.bottom)
			pCmbBoltWnd->MoveWindow(&rect);	
	}
	CWnd* pCmbWnd=CWnd::GetDlgItem(IDC_CHECK);
	if(pCmbWnd->GetSafeHwnd()!=NULL)
	{
		pCmbWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		wide=rect.right-rect.left;
		high=rect.bottom-rect.top;
		rect.right+=x;
		rect.left=rect.right-wide;
		if(rect.bottom+y>=40)
		{
			rect.bottom+=y;
			rect.top=rect.bottom-high;
		}
		if(rect.left<rect.right&&rect.top<rect.bottom)
			pCmbWnd->MoveWindow(&rect);	
	}
	CWnd* pBnExportWnd=CWnd::GetDlgItem(IDC_BN_EXPORT);
	if(pBnExportWnd->GetSafeHwnd()!=NULL)
	{
		pBnExportWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		wide=rect.right-rect.left;
		high=rect.bottom-rect.top;
		rect.right+=x;
		rect.left=rect.right-wide;
		if(rect.bottom+y>=40)
		{
			rect.bottom+=y;
			rect.top=rect.bottom-high;
		}
		if(rect.left<rect.right&&rect.top<rect.bottom)
			pBnExportWnd->MoveWindow(&rect);	
	}
	CWnd* pBnImportWnd=CWnd::GetDlgItem(IDC_BN_IMPORT);
	if(pBnImportWnd->GetSafeHwnd()!=NULL)
	{
		pBnImportWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		wide=rect.right-rect.left;
		high=rect.bottom-rect.top;
		rect.right+=x;
		rect.left=rect.right-wide;
		if(rect.bottom+y>=40)
		{
			rect.bottom+=y;
			rect.top=rect.bottom-high;
		}
		if(rect.left<rect.right&&rect.top<rect.bottom)
			pBnImportWnd->MoveWindow(&rect);	
	}
	CWnd* pBnOkWnd=CWnd::GetDlgItem(IDOK);
	if(pBnOkWnd->GetSafeHwnd()!=NULL)
	{
		pBnOkWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		wide=rect.right-rect.left;
		high=rect.bottom-rect.top;
		rect.right+=x;
		rect.left=rect.right-wide;
		if(rect.bottom+y>=40)
		{
			rect.bottom+=y;
			rect.top=rect.bottom-high;
		}
		if(rect.left<rect.right&&rect.top<rect.bottom)
			pBnOkWnd->MoveWindow(&rect);	
	}
	CWnd* pBnCannelWnd=CWnd::GetDlgItem(IDC_CANNEL);
	if(pBnCannelWnd->GetSafeHwnd()!=NULL)
	{
		pBnCannelWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		wide=rect.right-rect.left;
		high=rect.bottom-rect.top;
		rect.right+=x;
		rect.left=rect.right-wide;
		if(rect.bottom+y>=40)
		{
			rect.bottom+=y;
			rect.top=rect.bottom-high;
		}
		if(rect.left<rect.right&&rect.top<rect.bottom)
			pBnCannelWnd->MoveWindow(&rect);	
	}
	CDialog::OnSize(nType, cx, cy);
}