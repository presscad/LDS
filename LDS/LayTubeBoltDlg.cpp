// LayTubeBoltDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "LayTubeBoltDlg.h"
#include ".\laytubeboltdlg.h"
#include "GlobalFunc.h"
#include "ScrTools.h"
#include "env_def.h"
#include "ModifyJgLsDlg.h"
#include "LmaDlg.h"

#ifdef __COMPLEX_PART_INC_
// CLayTubeBoltDlg 对话框
IMPLEMENT_DYNAMIC(CLayTubeBoltDlg, CDialog)
CLayTubeBoltDlg::CLayTubeBoltDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CLayTubeBoltDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLayTubeBoltDlg)
	m_nLsEndSpace = 0;
	m_nLsNum = 0;
	m_nLsSpace = 0;
	m_iRayDirection = 0;
	m_hBaseNode = _T("");
	m_sRayTubeSize = _T("");
	m_iDatumPointStyle = 0;
	m_sBoltSize = _T("");
	m_sSegNo = _T("");
	m_iType = 0;
	m_bVirtualPart=FALSE;
	m_fOffsetAngle=0.0;
	//}}AFX_DATA_INIT
	m_hPlate=0;
	m_pTube=NULL;
	m_pNode=NULL;
	m_bIncPlateProfilePara=FALSE;
	face_norm_offset.Set(0,0,1);
	m_bRayEndFaceOffset=FALSE;
	m_pLegacyLsList=NULL;
	m_iStartEdge=0;
	m_iLayoutBoltStyle=0;
	m_fLenOffsetDist=0.0;
}

CLayTubeBoltDlg::~CLayTubeBoltDlg()
{
}

void CLayTubeBoltDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLayTubeBoltDlg)
	DDX_Text(pDX, IDC_S_RAY_TUBE_SIZE, m_sRayTubeSize);
	DDX_Text(pDX, IDC_E_LS_NUM, m_nLsNum);
	DDX_Text(pDX, IDC_E_LS_END_SPACE, m_nLsEndSpace);
	DDX_Text(pDX, IDC_E_LS_SPACE, m_nLsSpace);
	DDX_Radio(pDX, IDC_RDO_RAY_DIRECTION, m_iRayDirection);
	DDX_Text(pDX, IDC_S_BASE_NODE, m_hBaseNode);
	DDX_CBIndex(pDX, IDC_CMB_DATUM_POINT_STYLE, m_iDatumPointStyle);
	DDX_CBString(pDX, IDC_CMB_BOLT_SIZE, m_sBoltSize);
	DDX_Text(pDX, IDC_E_SEG_NO, m_sSegNo);
	DDX_CBIndex(pDX, IDC_CMB_TYPE, m_iType);
	DDX_Check(pDX, IDC_CHK_VERTUAL_PART, m_bVirtualPart);
	DDX_Text(pDX, IDC_E_OFFSET_ANGLE, m_fOffsetAngle);
	DDX_Control(pDX, IDC_LIST_LS_SET, m_xBoltListCtrl);
	DDX_CBIndex(pDX, IDC_CMB_START_EDGE, m_iStartEdge);
	DDX_CBIndex(pDX, IDC_CMB_LAYOUT_BOLT_STYLE, m_iLayoutBoltStyle);
	DDX_Text(pDX, IDC_E_LEN_OFFSET_DIST, m_fLenOffsetDist);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLayTubeBoltDlg, CDialog)
	//{{AFX_MSG_MAP(CLayTubeBoltDlg)
	ON_CBN_SELCHANGE(IDC_CMB_BOLT_SIZE, OnChangeCmbBoltSize)
	ON_CBN_SELCHANGE(IDC_CMB_DATUM_POINT_STYLE, OnSelchangeCmbDatumPointStyle)
	ON_CBN_SELCHANGE(IDC_CMB_TYPE, OnSelchangeCmbType)
	ON_BN_CLICKED(IDC_BN_LAYOUT_BOLTS, OnBnLayoutBolts)
	ON_BN_CLICKED(IDC_BN_BOLT_NORM_OFFSET, OnBnBoltNormOffset)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_LS_SET, OnLvnKeydownListLsSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CLayTubeBoltDlg::OnInitDialog() 
{
	if(m_pTube==NULL||m_pNode==NULL)
		return FALSE;
	if(!m_bInernalStart)
	{
		if(strlen(m_tBoltNormOffset.key_str)==0)
		{	//没有赋螺栓法向偏移默认值
			m_tBoltNormOffset.AddThick((long)m_pTube->GetDiameter()/2);
			m_tBoltNormOffset.AddThick(-m_pTube->handle,TRUE);
		}
		//工作法向偏移
		CString sThickStr;
		sThickStr.Format("%d",m_tBoltNormOffset.Thick(m_pTube->BelongModel()));
		GetDlgItem(IDC_BN_BOLT_NORM_OFFSET)->SetWindowText(sThickStr);
		if(m_sRayTubeSize.GetLength()==0)
		{	//没有赋规格字符串默认值时才重新计算此字符串
			char size_header[20];
			sprintf(size_header,"Ф%f",m_pTube->GetWidth());
			SimplifiedNumString(size_header);
			m_sRayTubeSize.Format("%sX%f",size_header,m_pTube->GetThick());
			SimplifiedNumString(m_sRayTubeSize);
		}
		m_hBaseNode.Format("0X%X",m_pNode->handle);
		//段号未初始化时使用钢管段号初始化螺栓段号,节点板命令使用钢板段号初始化 wht 11-01-07
		if(strlen(m_sSegNo)==0)
			m_sSegNo.Format("%s",(char*)m_pTube->iSeg.ToString());
		if(m_pTube->pEnd==m_pNode)
		{
			m_iRayDirection=1;	//终－>始
			m_nLsNum=m_pTube->connectEnd.wnConnBoltN;
			m_sBoltSize.Format("%d",m_pTube->connectEnd.d);
		}
		else if(m_pTube->pStart==m_pNode)
		{
			m_iRayDirection=0;	//始－>终
			m_nLsNum=m_pTube->connectStart.wnConnBoltN;
			m_sBoltSize.Format("%d",m_pTube->connectStart.d);
		}
		else 
		{	//基准钢管螺栓数=MAX(射线钢管螺栓数)*1.5;  //wht 10-11-23
			//在使用节点板命令时已经计算并保存在connectMid中
			m_iRayDirection=0;	//始－>终
			m_nLsNum=m_pTube->connectMid.wnConnBoltN;
			m_sBoltSize.Format("%d",m_pTube->connectMid.d);
		}
		//根据螺栓尺寸调整端距
		int bolt_d=atoi(m_sBoltSize);
		LSSPACE_STRU LsSpace;
		if(GetLsSpace(LsSpace,atoi(m_sBoltSize)))
		{
			m_nLsSpace = LsSpace.SingleRowSpace;
			//如果外面没有给端距赋值，才需要赋值
			if(m_nLsEndSpace == 0)
				m_nLsEndSpace = LsSpace.EndSpace;
		}
		if(m_pTube->pEnd==m_pNode||m_pTube->pStart==m_pNode)
		{
			m_iDatumPointStyle=0;	//归属节点为钢管端节点时默认以钢管端点为定位基点
			m_fLenOffsetDist = LsSpace.EndSpace;
		}	
		else if(m_iDatumPointStyle==0)
		{	//螺栓定位方式可能在外部已经指定 默认为端点定位时需要调整定位方式 wht 10-01-14
			//m_bStdProfile=TRUE;		//归属节点为中间节点时默认为标准轮廓 wht 09-08-05
			if(fabs(m_pTube->End().z-m_pTube->Start().z)<1)	//临时防错判断 wjh-2011.8.16
				m_bRayEndFaceOffset=FALSE;
			if(m_bRayEndFaceOffset)
			{	//归属节点为中间节点时默认以节点向钢管心线平推为基点定位方式
				m_iDatumPointStyle=2;
				//平推面法线应默认与螺栓定位基准钢管平推面法线一致
				if(m_pTube->desStartPos.datum_pos_style>2)
					face_norm_offset=m_pTube->desStartPos.face_offset_norm;
				else if(m_pTube->desEndPos.datum_pos_style>2)
					face_norm_offset=m_pTube->desEndPos.face_offset_norm;
			}
			else
				m_iDatumPointStyle=1;	//归属节点为中间节点时默认以节点向钢管心线投影为基点定位方式
		}
	}

	CDialog::OnInitDialog();
	m_xBoltListCtrl.AddColumnHeader("规格");
	m_xBoltListCtrl.AddColumnHeader("基点定位方式");
		m_xBoltListCtrl.AddColumnHeader("偏移角/纵向偏移");
	m_xBoltListCtrl.AddColumnHeader("法向偏移");
	long col_wide_arr[4]={40,130,110,65};
	m_xBoltListCtrl.InitListCtrl(col_wide_arr);
	CStringArray str_arr;
	str_arr.SetSize(4);
	if(m_pLegacyLsList!=NULL)
	{
		int i=0;
		double piece_angle;
		if (m_pLsList->GetNodeNum()>0)
			piece_angle = 360.0/m_pLsList->GetNodeNum();
		for(CLDSBolt* pBolt=m_pLegacyLsList->GetFirst();pBolt;pBolt=m_pLegacyLsList->GetNext())
		{
			str_arr[0].Format("M%d",pBolt->get_d());
			if(pBolt->des_base_pos.datumPoint.datum_pos_style==1)
				str_arr[1] = "钢管端点";
			else if(pBolt->des_base_pos.datumPoint.datum_pos_style==2)
				str_arr[1] = "节点向钢管心线投影";
			if (m_iLayoutBoltStyle==0)
				str_arr[2].Format("%f",m_fOffsetAngle+piece_angle*i);
			else
				str_arr[2].Format("%f",pBolt->des_base_pos.wing_offset_dist);
			SimplifiedNumString(str_arr[2]);
			str_arr[3].Format("%d",pBolt->des_base_pos.norm_offset.Thick(m_pTube->BelongModel()));
			int iItem=m_xBoltListCtrl.InsertItemRecord(-1,str_arr);
			//m_pLsSetList->SetItemData(iItem,(DWORD)pLsPara);
			i++;
		}
	}
	int i=0;
	double piece_angle;
	if (m_pLsList->GetNodeNum()>0)
		piece_angle = 360.0/m_pLsList->GetNodeNum();
	for(CDesignLsPara *pLsPara=m_pLsList->GetFirst();pLsPara;pLsPara=m_pLsList->GetNext())
	{
		str_arr[0].Format("M%d",pLsPara->d);
		if(pLsPara->des_base_pos.datumPoint.datum_pos_style==1)
			str_arr[1] = "钢管端点";
		else if(pLsPara->des_base_pos.datumPoint.datum_pos_style==2)
			str_arr[1] = "节点向钢管心线投影";
		if (m_iLayoutBoltStyle==0)
			str_arr[2].Format("%f",m_fOffsetAngle+piece_angle*i);
		else
			str_arr[2].Format("%f",pLsPara->des_base_pos.wing_offset_dist);
		SimplifiedNumString(str_arr[2]);
		str_arr[3].Format("%d",pLsPara->des_base_pos.norm_offset.Thick(m_pTube->BelongModel()));
		int iItem=m_xBoltListCtrl.InsertItemRecord(-1,str_arr);
		m_xBoltListCtrl.SetItemData(iItem,(DWORD)pLsPara);
		i++;
	}
	OnSelchangeCmbDatumPointStyle();
	//重启对话框后不用更新偏移量，否则在使用心线交点布置螺栓时手动输入的偏移值保存不上
	double x=fabs(face_norm_offset.x);
	double y=fabs(face_norm_offset.y);
	double z=fabs(face_norm_offset.z);
	if(x>EPS&&y<EPS&&z<EPS)
		m_iType=0;
	else if(y>EPS&&x<EPS&&z<EPS)
		m_iType=1;
	else if(z>EPS&&y<EPS&&x<EPS)
		m_iType=2;
	else
		m_iType=3;
	UpdateData(FALSE);
	CRect rect;
	GetWindowRect(rect);
	rect.right = rect.Width();
	rect.left = 0;
	rect.bottom = rect.Height();
	rect.top = 0;
	MoveWindow(rect,TRUE);
	return TRUE;
}

// CLayTubeBoltDlg 消息处理程序

void CLayTubeBoltDlg::OnChangeCmbBoltSize()
{
	CString sText;
	UpdateData();
	int ls_d = atoi(m_sBoltSize);
	LSSPACE_STRU LsSpace;
	if(GetLsSpace(LsSpace,ls_d))
	{
		m_nLsSpace = LsSpace.SingleRowSpace;
		UpdateData(FALSE);
	}
}

void CLayTubeBoltDlg::OnSelchangeCmbDatumPointStyle()
{
	UpdateData();
	if(m_iDatumPointStyle==2)
	{
		GetDlgItem(IDC_S_OFFSET_FACE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CMB_TYPE)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_S_OFFSET_FACE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CMB_TYPE)->ShowWindow(SW_HIDE);
	}
	GetDlgItem(IDC_S_OFFSET_FACE)->EnableWindow(m_iDatumPointStyle==2);
	GetDlgItem(IDC_CMB_TYPE)->EnableWindow(m_iDatumPointStyle==2);
	if(m_iDatumPointStyle==2)	//平推
	{
		//平推面法线应默认与螺栓定位基准钢管平推面法线一致
		if (m_pTube->desStartPos.datum_pos_style>2)
			face_norm_offset=m_pTube->desStartPos.face_offset_norm;
		else if(m_pTube->desEndPos.datum_pos_style>2)
			face_norm_offset=m_pTube->desEndPos.face_offset_norm;
	}
	UpdateData(FALSE);
}

void CLayTubeBoltDlg::OnSelchangeCmbType()
{
	UpdateData();
	if(m_iType==3)
	{
		CReportVectorDlg dlg;
		dlg.m_sCaption="输入平推面法线";
		dlg.m_fVectorX = face_norm_offset.x;
		dlg.m_fVectorY = face_norm_offset.y;
		dlg.m_fVectorZ = face_norm_offset.z;
		if(dlg.DoModal()==IDOK)
		{
			face_norm_offset.x = dlg.m_fVectorX;
			face_norm_offset.y = dlg.m_fVectorY;
			face_norm_offset.z = dlg.m_fVectorZ;
		}
	}
	else if(m_iType==0)
	{
		face_norm_offset.x = 1;
		face_norm_offset.y = 0;
		face_norm_offset.z = 0;
	}
	else if(m_iType==1)
	{
		face_norm_offset.x = 0;
		face_norm_offset.y = 1;
		face_norm_offset.z = 0;
	}
	else
	{
		face_norm_offset.x = 0;
		face_norm_offset.y = 0;
		face_norm_offset.z = 1;
	}
}

void CLayTubeBoltDlg::OnBnLayoutBolts()
{
	//按下布置螺栓按钮需要更新楞线侧边距，肢翼侧边距
	LayoutBolts(TRUE);
}

void CLayTubeBoltDlg::DisplayItemInfo(int iCurSel)
{
	CString ss;
	char tem_str[100];
	if(iCurSel<0)
		return;
	CDesignLsPara *pLsPara=(CDesignLsPara*)m_xBoltListCtrl.GetItemData(iCurSel);
	//CDesignLsPara *pLsPara=m_pLsList->GetByIndex(iCurSel);
	if(pLsPara==NULL)
	{
		AfxMessageBox("与其余构件间的共用螺栓，不可修改!");
		return;
	}
	if(pLsPara->des_base_pos.datumPoint.datum_pos_style==2&&
		pLsPara->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset)
		face_norm_offset=pLsPara->des_base_pos.datumPoint.des_para.RODNODE.vPlaneNormal;
	m_xBoltListCtrl.GetItemText(iCurSel,0,tem_str,100);
	m_sBoltSize.Format("%s",tem_str+1);
	m_xBoltListCtrl.GetItemText(iCurSel,1,tem_str,100);
	ss.Format("%s",tem_str);
	if(ss.CompareNoCase("钢管端点")==0)
		m_iDatumPointStyle = 0;
	else if(ss.CompareNoCase("节点向钢管心线投影")==0)
		m_iDatumPointStyle = 1;	//2
	if(pLsPara->des_base_pos.datumPoint.datum_pos_style==2)
	{
		if(pLsPara->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset==FALSE)
			m_iDatumPointStyle = 1;	//节点向钢管线投影为定位基点
		else
			m_iDatumPointStyle = 2;	//节点在钢管心线平推为定位基点
	}
	m_iRayDirection=pLsPara->des_base_pos.direction;
	m_xBoltListCtrl.GetItemText(iCurSel,2,tem_str,100);
	GetDlgItem(IDC_BN_BOLT_NORM_OFFSET)->SetWindowText(ss);
	UpdateData(FALSE);
}

void CLayTubeBoltDlg::OnBnBoltNormOffset()
{
	CString sThickStr=m_tBoltNormOffset.key_str;
	Input("输入法向偏移字符串",&sThickStr,'s');
	_snprintf(m_tBoltNormOffset.key_str,49,"%s",sThickStr);
	sThickStr.Format("%d",m_tBoltNormOffset.Thick(m_pTube->BelongModel()));
	GetDlgItem(IDC_BN_BOLT_NORM_OFFSET)->SetWindowText(sThickStr);
}

void CLayTubeBoltDlg::DeleteItem(int iCurSel)
{
	POSITION pos = m_xBoltListCtrl.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel = m_xBoltListCtrl.GetNextSelectedItem(pos);
		CDesignLsPara *pTmLsPara=(CDesignLsPara*)m_xBoltListCtrl.GetItemData(iCurSel);
		if(pTmLsPara==NULL)
		{
			AfxMessageBox("与其余构件间的共用螺栓，不可删除!");
			continue;
		}
		for(CDesignLsPara *pLsPara=m_pLsList->GetFirst();pLsPara;pLsPara=m_pLsList->GetNext())
		{
			if(pLsPara==pTmLsPara)
			{
				m_pLsList->DeleteCursor();
				break;
			}
		}
		m_xBoltListCtrl.DeleteItem(iCurSel);
		pos = m_xBoltListCtrl.GetFirstSelectedItemPosition();
	}
}

void CLayTubeBoltDlg::OnLvnKeydownListLsSet(NMHDR *pNMHDR, LRESULT *pResult)
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	POSITION pos = m_xBoltListCtrl.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_xBoltListCtrl.GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_F3)
		{
			if(iCurSel>=0)
				DisplayItemInfo(iCurSel);
		}
		else if(pLVKeyDow->wVKey==VK_DELETE)
		{
			if(iCurSel>=0)
				DeleteItem(iCurSel);
		}
	}

	*pResult = 0;
	//LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
}

void CLayTubeBoltDlg::LayoutBolts(BOOL bUpdateData/*=TRUE*/)
{
	CDesignLsPara ls_stru;
	try{
		UpdateData();
		ls_stru.d=atoi(m_sBoltSize);
		ls_stru.iSeg=SEGI(m_sSegNo.GetBuffer());
		if (m_bVirtualPart)
			ls_stru.m_bVirtualPart=true;
		else
			ls_stru.m_bVirtualPart=false;;
		//螺栓法线设定
		ls_stru.des_work_norm.hVicePart = m_pTube->handle;
		ls_stru.des_base_pos.norm_offset=m_tBoltNormOffset;
		ls_stru.des_work_norm.norm_style=5;
		if (m_iStartEdge==0)	//起始边为基准边
			ls_stru.des_work_norm.direction=2;
		else					//起始边为焊道边
			ls_stru.des_work_norm.direction=3;
		ls_stru.des_work_norm.rot_ang=m_fOffsetAngle;
		ls_stru.des_base_pos.direction=m_iRayDirection;
		//螺栓位置设定
		if(m_iDatumPointStyle==0)	//钢管端点为定位基点
		{
			ls_stru.des_base_pos.datumPoint.datum_pos_style=1;
			ls_stru.des_base_pos.hPart=ls_stru.des_base_pos.datumPoint.des_para.RODEND.hRod=m_pTube->handle;
			ls_stru.des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
			ls_stru.des_base_pos.datumPoint.des_para.RODEND.direction=m_iRayDirection;
			ls_stru.des_base_pos.datumPoint.des_para.RODEND.len_offset_dist=m_fLenOffsetDist;
			ls_stru.des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist=0;
			//if (m_pTube->IsAngle())
				//ls_stru.des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=4;
		}
		else if(m_iDatumPointStyle==1||m_iDatumPointStyle==2)	//节点向钢管心线投影/平推为定位基点
		{
			ls_stru.des_base_pos.datumPoint.datum_pos_style=2;
			ls_stru.des_base_pos.datumPoint.des_para.RODNODE.direction=m_iRayDirection;
			ls_stru.des_base_pos.datumPoint.des_para.RODNODE.len_offset_dist=m_fLenOffsetDist;
			if(m_iDatumPointStyle==1)
				ls_stru.des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset=FALSE;
			else
			{
				ls_stru.des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset=TRUE;
				ls_stru.des_base_pos.datumPoint.des_para.RODNODE.vPlaneNormal=face_norm_offset;
			}
			ls_stru.des_base_pos.hPart=ls_stru.des_base_pos.datumPoint.des_para.RODNODE.hRod=m_pTube->handle;
			ls_stru.des_base_pos.datumPoint.des_para.RODNODE.hNode=m_pNode->handle;
		}
		CDesignLsPara *pLsPara=NULL;
		ATOM_LIST<CDesignLsPara> lslist;
		int i;
		for(i=0;i<m_nLsNum;i++)
		{
			if (m_iLayoutBoltStyle==0)
			{
				if (i==0)
					ls_stru.des_work_norm.rot_ang=m_fOffsetAngle;
				else
				{
					double piece_angle=360.0/m_nLsNum;
					ls_stru.des_work_norm.rot_ang=m_fOffsetAngle+piece_angle*i;
				}
			}
			else
			{
				if(i==0)
					ls_stru.des_base_pos.wing_offset_dist=m_fLenOffsetDist;
				else
					ls_stru.des_base_pos.wing_offset_dist+=m_nLsSpace;
			}
			pLsPara=lslist.append(ls_stru);
		}
		CStringArray str_arr;
		str_arr.SetSize(4);
		i=0;
		double piece_angle;
		if (lslist.GetNodeNum()>0)
			piece_angle = 360.0/lslist.GetNodeNum();
		for(pLsPara=lslist.GetFirst();pLsPara;pLsPara=lslist.GetNext())
		{
			pLsPara=m_pLsList->append(*pLsPara);	//必须有此行，因为螺栓记录需要与对话框同生命周期，而不是与此函数同寿命
			str_arr[0].Format("M%d",pLsPara->d);
			if(pLsPara->des_base_pos.datumPoint.datum_pos_style==1)
				str_arr[1] = "钢管端点";
			else if(pLsPara->des_base_pos.datumPoint.datum_pos_style==2)
			{
				if(!pLsPara->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset)
					str_arr[1] = "节点向钢管心线投影";
				else
					str_arr[1] = "节点向钢管心线平推";
			}
			if (m_iLayoutBoltStyle==0)
				str_arr[2].Format("%f",m_fOffsetAngle+piece_angle*i);
			else
				str_arr[2].Format("%f",pLsPara->des_base_pos.wing_offset_dist);
			SimplifiedNumString(str_arr[2]);
			str_arr[3].Format("%d",pLsPara->des_base_pos.norm_offset.Thick(m_pTube->BelongModel()));
			int iItem=m_xBoltListCtrl.InsertItemRecord(-1,str_arr);
			m_xBoltListCtrl.SetItemData(iItem,(DWORD)pLsPara);
			i++;
		}
		UpdateData(FALSE);
		CWnd *pButtonOK=GetDlgItem(IDOK);
		pButtonOK->SetFocus();
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
}

void CLayTubeBoltDlg::OnOK() 
{
	//需要布螺栓但还没有布过时就确认,此时系统会自动布置螺栓
	//未按下布置螺栓按钮不更新楞线侧边距、肢翼侧边距，否则修改过的数据将被冲掉
	if(m_pLsList&&m_pLsList->GetNodeNum()==0&&m_nLsNum>0)
		LayoutBolts(FALSE); 
	CDialog::OnOK();
}

#endif