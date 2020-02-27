//<LOCALE_TRANSLATE Confirm by hxr/>
// BendPlateByAngleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "BendPlateByAngleDlg.h"
#include "Query.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////////
//DESIGN_GUAXIAN_HOLE
DESIGN_GUAXIAN_HOLE::GUAXIANHOLE_BASE_INFO::GUAXIANHOLE_BASE_INFO()
{
	serial=0;
	hole_d=40;		
	fRadius=60;
	lengthPosToEdge=0;
	bDrillHole=FALSE;
}
DESIGN_GUAXIAN_HOLE::DESIGN_GUAXIAN_HOLE()
{
	m_iEdgeShapeType=0;
	pStartVertex=NULL;
	pEndVertex=NULL;
}
/////////////////////////////////////////////////////////////////////////////
// CBendPlateByAngleDlg dialog
CBendPlateByAngleDlg::CBendPlateByAngleDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CBendPlateByAngleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBendPlateByAngleDlg)
	m_fBendAngle = 5.0;
	m_fBendClearSpace = 10.0;
	m_iDatumEdge = 0;
	m_iBendLineStyle=0;
	//
	m_fOriginX=0;
	m_fOriginY=0;
	m_iIndex=0;
	m_iHoleD=0;
	m_fRadius=0;
	m_fPosX=0;
	m_fPosY=0;
	m_sCirPlatePartNo=_T("");
	m_fOffsetDist=0;
	m_sTubePartNo=_T("");
	m_iPlateMaterial=0;
	m_iTubeGuiGe=3;
	m_iTubeMaterial=0;
	m_bDrillHole=FALSE;
	m_bHaveTube=FALSE;
	m_bHaveCirPlate=FALSE;
	m_iEdgeShapeType=1;
	//
	//m_listCtrlRayAngles.AddColumnHeader();
	//m_listCtrlRayAngles.AddColumnHeader();
	//
#ifdef AFX_TARG_ENU_ENGLISH
	m_listCtrlGuaXianHoles.AddColumnHeader("number",45);
	m_listCtrlGuaXianHoles.AddColumnHeader("diameter",50);
	m_listCtrlGuaXianHoles.AddColumnHeader("outer radius",65);
	m_listCtrlGuaXianHoles.AddColumnHeader("X",40);
	m_listCtrlGuaXianHoles.AddColumnHeader("Y",40);
	m_listCtrlGuaXianHoles.AddColumnHeader("drill",50);
#else
	m_listCtrlGuaXianHoles.AddColumnHeader("序号",45);
	m_listCtrlGuaXianHoles.AddColumnHeader("孔径",50);
	m_listCtrlGuaXianHoles.AddColumnHeader("外缘半径",65);
	m_listCtrlGuaXianHoles.AddColumnHeader("X",40);
	m_listCtrlGuaXianHoles.AddColumnHeader("Y",40);
	m_listCtrlGuaXianHoles.AddColumnHeader("钻孔",50);
#endif
	//}}AFX_DATA_INIT
}


void CBendPlateByAngleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBendPlateByAngleDlg)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST_BOX, m_xPropList);
	DDX_Control(pDX, IDC_LIST_RAY_ANGLES, m_listCtrlRayAngles);
	DDX_Control(pDX, IDC_LIST_GUAXIAN_HOLES, m_listCtrlGuaXianHoles);
	DDX_Text(pDX, IDC_E_ORIGIN_X, m_fOriginX);
	DDX_Text(pDX, IDC_E_ORIGIN_Y, m_fOriginY);
	DDX_Text(pDX, IDC_E_INDEX, m_iIndex);
	DDX_Text(pDX, IDC_E_HOLE_D, m_iHoleD);
	DDX_Text(pDX, IDC_E_RADIUS, m_fRadius);
	DDX_Text(pDX, IDC_E_POS_X, m_fPosX);
	DDX_Text(pDX, IDC_E_POS_Y, m_fPosY);
	DDX_Text(pDX, IDC_E_TUBE_PARTNO, m_sTubePartNo);
	DDX_Text(pDX, IDC_E_CIRPALTE_PARTNO, m_sCirPlatePartNo);
	DDX_Text(pDX, IDC_E_OFFSET_DIST, m_fOffsetDist);
	DDX_CBIndex(pDX, IDC_CMB_TUBE_MATERIAL, m_iTubeMaterial);
	DDX_CBIndex(pDX, IDC_CMB_TUBE_GUIGE, m_iTubeGuiGe);
	DDX_CBIndex(pDX, IDC_CMB_CIRPLATE_MATERIAL, m_iPlateMaterial);
	DDX_Check(pDX, IDC_CHK_DRILL_HOLE, m_bDrillHole);
	DDX_Check(pDX, IDC_CHK_HAVE_TUBE, m_bHaveTube);
	DDX_Check(pDX, IDC_CHK_HAVE_CIRPLATE, m_bHaveCirPlate);
	DDX_Radio(pDX, IDC_RADIO_WAITU, m_iEdgeShapeType);
}


BEGIN_MESSAGE_MAP(CBendPlateByAngleDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CBendPlateByAngleDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BN_ADD_GUAXIAN_HOLE, &CBendPlateByAngleDlg::OnBnClickedBnAddGuaxianHole)
	ON_BN_CLICKED(IDC_CHK_HAVE_TUBE, &CBendPlateByAngleDlg::OnBnClickedChkHaveTube)
	ON_BN_CLICKED(IDC_CHK_HAVE_CIRPLATE, &CBendPlateByAngleDlg::OnBnClickedChkHaveCirplate)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GUAXIAN_HOLES, &CBendPlateByAngleDlg::OnNMDblclkListGuaXianHoles)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_GUAXIAN_HOLES, &CBendPlateByAngleDlg::OnNMRClickListGuaXianHoles)
	ON_COMMAND(ID_DELETE_ITEM,&CBendPlateByAngleDlg::OnDeleteItem)
	ON_BN_CLICKED(IDC_BN_SEL_POS_BOLT, &CBendPlateByAngleDlg::OnBnClickedBnSelPosBolt)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBendPlateByAngleDlg message handlers

CHashStrList<PROPLIST_ITEM> CBendPlateByAngleDlg::propHashtable;	//属性字符串哈希表
void CBendPlateByAngleDlg::InitPropHashtable()
{
	propHashtable.SetHashTableGrowSize(50);
	long id=1;
#ifdef AFX_TARG_ENU_ENGLISH
	AddPropItem("BendLineStyle",PROPLIST_ITEM(id++,"curved line locate mode","plate curved line Location and direction locate mode","angle edge|manual specify"));
	AddPropItem("hBendLineAngle",PROPLIST_ITEM(id++,"curved lines depend on angle","plate curved line locate angle handle"));
	AddPropItem("m_iDatumEdge",PROPLIST_ITEM(id++,"locate","Which side of the depend on angle is plate curved line located in","ridge line side|leg side"));
	AddPropItem("m_fBendClearSpace",PROPLIST_ITEM(id++,"bended line clearance","clearance value of the bended line and angle edge"));
	AddPropItem("m_fBendAngle",PROPLIST_ITEM(id++,"curved angle","the angle of curved plane around curved line clockwise,°"));
#else
	AddPropItem("BendLineStyle",PROPLIST_ITEM(id++,"制弯线定位方式","钢板制弯线位置及走向的定位方式","角钢边缘|手动指定"));
	AddPropItem("hBendLineAngle",PROPLIST_ITEM(id++,"制弯线依赖角钢","钢板制弯线的定位角钢句柄"));
	AddPropItem("m_iDatumEdge",PROPLIST_ITEM(id++,"位于","钢板制弯线位于定位依赖角钢的哪一侧","楞线侧|肢翼侧"));
	AddPropItem("m_fBendClearSpace",PROPLIST_ITEM(id++,"火曲线间隙","火曲线与角钢边缘的间隙值"));
	AddPropItem("m_fBendAngle",PROPLIST_ITEM(id++,"火曲角度","火曲面绕火曲线的逆时针旋转角度,°"));
#endif
}
PROPLIST_ITEM* CBendPlateByAngleDlg::AddPropItem(char* keyStr,PROPLIST_ITEM item)
{
	PROPLIST_ITEM* pItem=propHashtable.GetValue(keyStr);
	if(pItem!=NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("attribute primary key string %s duplicate",keyStr);
#else
		logerr.Log("属性主键字符串%s出现重复",keyStr);
#endif
		return pItem;
	}
	else
		pItem=propHashtable.SetValue(keyStr,item);
	pItem->propKeyStr.Copy(keyStr);
	return pItem;
}
long CBendPlateByAngleDlg::GetPropID(char* propStr)	//得到属性ID
{
	PROPLIST_ITEM* pItem=propHashtable.GetValue(propStr);
	if(pItem)
		return pItem->id;
	else
		return 0;
}
int CBendPlateByAngleDlg::GetPropValueStr(long id, char *valueStr)	//根据属性ID得到属性值
{
	CXhChar50 sText;
	if(GetPropID("BendLineStyle")==id)
	{	//制弯线定位方式
		if(m_iBendLineStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Angle Edge");
		else
			sText.Copy("1.Manual Specify");
#else
			sText.Copy("0.角钢边缘");
		else
			sText.Copy("1.手动指定");
#endif
	}
	else if(GetPropID("hBendLineAngle")==id)	
	{	//制弯线依赖角钢
		sText.Printf("0X%X",m_hBendLineAngle);
	}
	else if(GetPropID("m_iDatumEdge")==id)
	{	//位于
		if(m_iDatumEdge==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Ridge Line Side");
		else
			sText.Copy("1.Leg Side");
#else
			sText.Copy("0.楞线侧");
		else
			sText.Copy("1.肢翼侧");
#endif
	}
	else if(GetPropID("m_fBendClearSpace")==id)
	{	//火曲线间隙
		sText.Printf("%g",m_fBendClearSpace);
	}
	else if(GetPropID("m_fBendAngle")==id)
	{	//火曲角度
		sText.Printf("%g",m_fBendAngle);
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return sText.GetLength();
}

BOOL CBendPlateByAngleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitPropHashtable();
	DisplayPropertyList();
	RefreshCtrlStatue();
	m_listCtrlRayAngles.InitListCtrl();		//初始化射线杆件列表
	m_listCtrlGuaXianHoles.InitListCtrl();	//初始化挂线孔列表
	//套管材质
	CComboBox* pCMB = (CComboBox*)GetDlgItem(IDC_CMB_TUBE_MATERIAL);
	AddSteelMatRecord(pCMB);
	pCMB->SetCurSel(m_iTubeMaterial);
	//套管规格
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_TUBE_GUIGE);
	AddTubeRecord(pCMB);
	pCMB->SetCurSel(m_iTubeGuiGe);
	//环板材质
	pCMB = (CComboBox*)GetDlgItem(IDC_CMB_CIRPLATE_MATERIAL);
	AddSteelMatRecord(pCMB);
	pCMB->SetCurSel(m_iPlateMaterial);
	MoveWndToLocation();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
CItemInfo* CBendPlateByAngleDlg::AppendPropItemInfo(const char* propKey,int controlType/*=0*/,long* pPropId/*=NULL*/,BOOL bUpdateValueText/*=TRUE*/)
{
	PROPLIST_ITEM* pPropItem=propHashtable.GetValue(propKey);
	if(pPropItem==NULL)
		return NULL;
	CItemInfo* lpInfo = new CItemInfo();
	lpInfo->m_strPropName=pPropItem->name;
	lpInfo->m_strPropHelp=pPropItem->propHelpStr;
	lpInfo->m_controlType=controlType;
	if(pPropId!=NULL)
		*pPropId=pPropItem->id;
	CXhChar50 sText;
	if(bUpdateValueText&&GetPropValueStr(pPropItem->id,sText)>0)
		lpInfo->m_strPropValue = sText;
	return lpInfo;
}
CPropTreeItem* CBendPlateByAngleDlg::InsertItem(CPropTreeItem *pParent, const char* propKey,
	int controlType/*=0*/,BOOL bUpdateValueText/*=TRUE*/, int iInsertIndex/*-1*/, BOOL bUpdate/*=0*/)
{
	long propId=0;
	CItemInfo* lpInfo=AppendPropItemInfo(propKey,controlType,&propId,bUpdateValueText);
	if(lpInfo==NULL)
		return NULL;
	CPropTreeItem* pItem=m_xPropList.InsertItem(pParent,lpInfo,iInsertIndex,bUpdate);
	pItem->m_idProp=propId;
	pItem->m_dwPropGroup=pParent->m_dwPropGroup;
	return pItem;
}
//defined in DeignTubeEndLjDlg.cpp
extern CPropTreeItem* InsertOrUpdateItem(CPropertyList* pPropList,PROPLIST_ITEM* pPropItem,CPropTreeItem *pParent, int controlType/*=0*/,const char* valueText/*=NULL*/, int iInsertIndex/*-1*/, BOOL bUpdate/*=0*/);
CPropTreeItem* CBendPlateByAngleDlg::InsertOrUpdateItem(CPropTreeItem *pParent, const char* propKey,
	int controlType/*=0*/,BOOL bUpdateValueText/*=TRUE*/, int iInsertIndex/*-1*/, BOOL bUpdate/*=0*/)
{
	PROPLIST_ITEM* pPropItem=propHashtable.GetValue(propKey);
	if(pPropItem==NULL)
		return NULL;
	CXhChar50 sText;
	if(bUpdateValueText&&GetPropValueStr(pPropItem->id,sText)>0)
		return ::InsertOrUpdateItem(&m_xPropList,pPropItem,pParent,controlType,sText,iInsertIndex,bUpdate);
	else
		return ::InsertOrUpdateItem(&m_xPropList,pPropItem,pParent,controlType,NULL,iInsertIndex,bUpdate);
}
BOOL ModifyBendPlateByAngleProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CBendPlateByAngleDlg* pBendDlg=(CBendPlateByAngleDlg*)pPropList->GetParent();
	CXhChar100 tem_str;
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pItem->m_idProp==CBendPlateByAngleDlg::GetPropID("m_iDatumEdge"))
	{
		pBendDlg->m_iDatumEdge=tem_str[0]-'0';
	}
	else if(pItem->m_idProp==CBendPlateByAngleDlg::GetPropID("BendLineStyle"))
	{
		pBendDlg->m_iBendLineStyle=tem_str[0]-'0';
	}
	else if(pItem->m_idProp==CBendPlateByAngleDlg::GetPropID("hBendLineAngle"))
	{
		pBendDlg->m_hBendLineAngle=HexStrToLong(tem_str);
	}
	else if(pItem->m_idProp==CBendPlateByAngleDlg::GetPropID("m_fBendClearSpace"))
	{
		pBendDlg->m_fBendClearSpace=atof(tem_str);
	}
	else if(pItem->m_idProp==CBendPlateByAngleDlg::GetPropID("m_fBendAngle"))
	{
		pBendDlg->m_fBendAngle=atof(tem_str);
	}
	else
		return FALSE;
	return TRUE;
}
void CBendPlateByAngleDlg::DisplayPropertyList()
{
	m_xPropList.CleanTree();
	m_xPropList.SetModifyValueFunc(ModifyBendPlateByAngleProperty);
	m_xPropList.SetButtonClickFunc(NULL);
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL,*pLeafItem=NULL,*pRootItem=m_xPropList.GetRootItem();
	//制弯线定位方式
	pGroupItem = InsertItem(pRootItem,"BendLineStyle",CItemInfo::TYPE_COMBO,TRUE);
#ifdef AFX_TARG_ENU_ENGLISH
	pGroupItem->m_lpNodeInfo->m_cmbItems="0.Angle Edge|1.Manual Specify";
#else
	pGroupItem->m_lpNodeInfo->m_cmbItems="0.角钢边缘|1.手动指定";
#endif
	//制弯线依赖角钢
	pPropItem=InsertItem(pGroupItem,"hBendLineAngle",CItemInfo::TYPE_BUTTON,TRUE);
	//钢板制弯线位于定位依赖角钢的哪一侧
	pPropItem=InsertItem(pGroupItem,"m_iDatumEdge",CItemInfo::TYPE_COMBO,TRUE);
#ifdef AFX_TARG_ENU_ENGLISH
	pPropItem->m_lpNodeInfo->m_cmbItems="0.Ridge Line Side|1.Leg Side";
#else
	pPropItem->m_lpNodeInfo->m_cmbItems="0.楞线侧|1.肢翼侧";
#endif
	//火曲线间隙
	pPropItem = InsertItem(pGroupItem,"m_fBendClearSpace",CItemInfo::TYPE_EDIT,TRUE);
	//火曲角度
	pGroupItem = InsertItem(pRootItem,"m_fBendAngle",CItemInfo::TYPE_EDIT,TRUE);
	m_xPropList.Redraw();
}
void CBendPlateByAngleDlg::RefreshCtrlStatue()
{
	((CEdit*)GetDlgItem(IDC_E_INDEX))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_E_TUBE_PARTNO))->SetReadOnly(!m_bHaveTube);
	((CEdit*)GetDlgItem(IDC_E_CIRPALTE_PARTNO))->SetReadOnly(!m_bHaveCirPlate);
	((CEdit*)GetDlgItem(IDC_E_OFFSET_DIST))->SetReadOnly(!m_bHaveCirPlate);
}
//////////////////////////////////////////////////////////////////////////
//事件处理函数
void CBendPlateByAngleDlg::OnBnClickedChkHaveTube()
{
	UpdateData();
	RefreshCtrlStatue();
	UpdateData(FALSE);
}
void CBendPlateByAngleDlg::OnBnClickedChkHaveCirplate()
{
	UpdateData();
	RefreshCtrlStatue();
	UpdateData(FALSE);
}
void CBendPlateByAngleDlg::OnBnClickedBnAddGuaxianHole()
{
	UpdateData();
	DESIGN_GUAXIAN_HOLE::GUAXIANHOLE_BASE_INFO* pGuaXianHole=NULL;
	pGuaXianHole=design_guaxian_hole.guaXianHoleHashList.Add(0);
	pGuaXianHole->ls_pos.Set(m_fOriginX,m_fOriginY,0);
	pGuaXianHole->fDistX=m_fPosX;
	pGuaXianHole->fDistY=m_fPosY;
	pGuaXianHole->hole_d=m_iHoleD;
	pGuaXianHole->fRadius=m_fRadius;
	pGuaXianHole->bDrillHole=m_bDrillHole;
	pGuaXianHole->bHasTube=m_bHaveTube;
	CComboBox* pCMB=NULL;
	CString sValue;
	if(pGuaXianHole->bHasTube)
	{
		strcpy(pGuaXianHole->tube.sPartNo,m_sTubePartNo);
		pCMB=(CComboBox*)GetDlgItem(IDC_CMB_TUBE_GUIGE);
		pCMB->GetLBText(m_iTubeGuiGe,sValue);
		strcpy(pGuaXianHole->tube.guige,sValue.GetBuffer());
		pCMB=(CComboBox*)GetDlgItem(IDC_CMB_TUBE_MATERIAL);
		pCMB->GetLBText(m_iTubeMaterial,sValue);
		pGuaXianHole->tube.cMaterial=QuerySteelBriefMatMark(sValue.GetBuffer());
	}
	pGuaXianHole->bWeldCirClePank=m_bHaveCirPlate;
	if(pGuaXianHole)
	{
		strcpy(pGuaXianHole->cir_plate.sPartNo,m_sCirPlatePartNo);
		pGuaXianHole->cir_plate.fOffsetDist=m_fOffsetDist;
		pCMB=(CComboBox*)GetDlgItem(IDC_CMB_CIRPLATE_MATERIAL);
		pCMB->GetLBText(m_iPlateMaterial,sValue);
		pGuaXianHole->cir_plate.cMaterial=QuerySteelBriefMatMark(sValue.GetBuffer());
	}
	//更新m_listCtrlGuaXianHoles记录
	CStringArray str_arr;
	str_arr.SetSize(6);
	str_arr[0].Format("%d",pGuaXianHole->Serial());
	str_arr[1].Format("%d",pGuaXianHole->hole_d); 
	str_arr[2].Format("%g",pGuaXianHole->fRadius);
	str_arr[3].Format("%g",pGuaXianHole->ls_pos.x+pGuaXianHole->fDistX);
	str_arr[4].Format("%g",pGuaXianHole->ls_pos.y+pGuaXianHole->fDistY);
	if(pGuaXianHole->bDrillHole)
#ifdef AFX_TARG_ENU_ENGLISH
		str_arr[5]="Yes";
	else
		str_arr[5]="No";
#else
		str_arr[5]="是";
	else
		str_arr[5]="否";
#endif
	m_listCtrlGuaXianHoles.InsertItemRecord(-1,str_arr);    //插入记录
}
void CBendPlateByAngleDlg::OnNMDblclkListGuaXianHoles(NMHDR *pNMHDR, LRESULT *pResult)
{ 
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int iCurSel = m_listCtrlGuaXianHoles.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if(iCurSel<0)
		return;
	char key[4]="";
	m_listCtrlGuaXianHoles.GetItemText(iCurSel,0,key,4);
	DESIGN_GUAXIAN_HOLE::GUAXIANHOLE_BASE_INFO* pGuaXianHole=NULL;
	pGuaXianHole=design_guaxian_hole.guaXianHoleHashList.GetValue(atoi(key));
	if(pGuaXianHole==NULL)
		return;
	m_fOriginX=pGuaXianHole->ls_pos.x;
	m_fOriginY=pGuaXianHole->ls_pos.y;
	m_iIndex=pGuaXianHole->Serial();
	m_iHoleD=pGuaXianHole->hole_d;
	m_fRadius=pGuaXianHole->fRadius;
	m_bDrillHole=pGuaXianHole->bDrillHole;
	m_bHaveTube=pGuaXianHole->bHasTube;
	CComboBox* pCMB=NULL;
	CString sValue;
	if(m_bHaveTube)
	{
		m_sTubePartNo.Format("%s",pGuaXianHole->tube.sPartNo);
		pCMB=(CComboBox*)GetDlgItem(IDC_CMB_TUBE_GUIGE);
		m_iTubeGuiGe=pCMB->FindString(-1,pGuaXianHole->tube.guige);
		m_iTubeMaterial=QuerySteelMatIndex(pGuaXianHole->tube.cMaterial);
	}
	m_bHaveCirPlate=pGuaXianHole->bWeldCirClePank;
	if(m_bHaveCirPlate)
	{
		m_sCirPlatePartNo.Format("%s",pGuaXianHole->cir_plate.sPartNo);
		m_fOffsetDist=pGuaXianHole->cir_plate.fOffsetDist;
		m_iPlateMaterial=QuerySteelMatIndex(pGuaXianHole->cir_plate.cMaterial);
	}
	RefreshCtrlStatue();
	UpdateData(FALSE);
	*pResult = 0;
}
void CBendPlateByAngleDlg::OnNMRClickListGuaXianHoles(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	//1.判断是否存在选中项
	int iCurSel = m_listCtrlGuaXianHoles.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED); 
	if(iCurSel<0)
		return;
	//2.获取当前鼠标位置
	CPoint point;
	GetCursorPos(&point);
	//3.获取ListCtrl对应的矩形区域
	CRect rect;
	m_listCtrlGuaXianHoles.GetWindowRect(&rect);
	ScreenToClient(&rect);
	//4.在ListCtrl范围内点击右键时显示右键菜单
	if(rect.PtInRect(point))
		ContextMenu(this,point);
	*pResult = 0;
}
void CBendPlateByAngleDlg::ContextMenu(CWnd *pWnd, CPoint point)
{
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP); //加载菜单项
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
#ifdef AFX_TARG_ENU_ENGLISH
	pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete");
#else
	pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除");
#endif
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point .x, point .y,this);
}
void CBendPlateByAngleDlg::OnDeleteItem()
{ 
	int iCurSel = m_listCtrlGuaXianHoles.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED); 
	if(iCurSel<0)
		return;
	char key[4]="";
	m_listCtrlGuaXianHoles.GetItemText(iCurSel,0,key,4);
	DESIGN_GUAXIAN_HOLE::GUAXIANHOLE_BASE_INFO* pGuaXianHole=NULL;
	pGuaXianHole=design_guaxian_hole.guaXianHoleHashList.GetValue(atoi(key));
	if(pGuaXianHole)
		design_guaxian_hole.guaXianHoleHashList.DeleteNode(pGuaXianHole->Serial());
	m_listCtrlGuaXianHoles.DeleteItem(iCurSel);	//删除选中项 
}
void CBendPlateByAngleDlg::OnBnClickedBnSelPosBolt()
{
	
}
