// AngleBoltsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "AngleBoltsDlg.h"
#include "afxdialogex.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "ModifyJgLsDlg.h"
#include "btc.h"
//////////////////////////////////////////////////////////////////////////
//回调函数及临时函数处理
static double GetLsParaDatumG(CDesignLsPara *pLsPara)
{
	if(pLsPara==NULL)
		return 0;
	double fDatumG=0;
	if(pLsPara->des_base_pos.datumPoint.datum_pos_style==2)
		fDatumG=pLsPara->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist;
	else if(pLsPara->des_base_pos.datumPoint.datum_pos_style==3)
		fDatumG=pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1;
	else //if(pLsPara->des_base_pos.datumPoint.datum_pos_style==1)
		fDatumG=pLsPara->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist;
	return fDatumG;
}
static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem,CString& sTextValue)
{
	CDesignLsPara *pDesignLsPara=(CDesignLsPara*)pSelItem->m_idProp;
	if(pDesignLsPara==NULL)
		return FALSE;
	CAngleBoltsDlg *pLayAngleBoltDlg=(CAngleBoltsDlg*)pListCtrl->GetParent();
	if(iSubItem==1)
	{
		if(sTextValue.CompareNoCase("M12")==0)
			pDesignLsPara->d =12;
		else if(sTextValue.CompareNoCase("M16")==0)
			pDesignLsPara->d =16;
		else if(sTextValue.CompareNoCase("M20")==0)
			pDesignLsPara->d =20;
		else if(sTextValue.CompareNoCase("M24")==0)
			pDesignLsPara->d =24;
		else if(sTextValue.CompareNoCase("M27")==0)
			pDesignLsPara->d =27;
		else 
			pDesignLsPara->d =atoi(sTextValue);
		pLayAngleBoltDlg->PreviewBolt(pDesignLsPara);
	}
	else if(iSubItem==3)
	{
		pDesignLsPara->des_base_pos.len_offset_dist = atof(sTextValue);
		pLayAngleBoltDlg->PreviewBolt(pDesignLsPara);
	}
	else if(iSubItem==4)
	{
		pDesignLsPara->des_base_pos.wing_offset_dist = atof(sTextValue)-GetLsParaDatumG(pDesignLsPara);
		pLayAngleBoltDlg->PreviewBolt(pDesignLsPara);
	}
	else if(iSubItem==5)
	{
		pDesignLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;
		if(atoi(sTextValue)!=pLayAngleBoltDlg->m_nCrossAngleG)
			pDesignLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=4;
		pDesignLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2=atoi(sTextValue);
		pLayAngleBoltDlg->PreviewBolt(pDesignLsPara);
	}
	return TRUE;
}
static BOOL FireDeleteItem(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem)
{
	CListCtrl* m_pLsSetList=pListCtrl;//为了少改动代码 特意定义临时变量
	CAngleBoltsDlg *pLayAngleBoltDlg=(CAngleBoltsDlg*)pListCtrl->GetParent();
	CDesignLsPara *pTmLsPara=(CDesignLsPara*)pSelItem->m_idProp;
	if(pTmLsPara==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Share the bolt with other parts.It cannot be deleted!");
#else 
		AfxMessageBox("与其余构件间的共用螺栓，不可删除!");
#endif
		return FALSE;
	}
	for(CDesignLsPara *pLsPara=pLayAngleBoltDlg->m_pLsList->GetFirst();pLsPara;pLsPara=pLayAngleBoltDlg->m_pLsList->GetNext())
	{
		if(pLsPara==pTmLsPara)
		{
			pLayAngleBoltDlg->m_pLsList->DeleteCursor();
			//删除对应的螺栓实体
			if(pLsPara->pMapLs&&pLayAngleBoltDlg->m_xHashPrevViewBolts.GetValue(pLsPara->pMapLs->handle))
			{
				pLsPara->pMapLs->feature=0;	//设置当前螺栓为空闲状态
				g_pSolidDraw->DelEnt(pLsPara->pMapLs->handle);
				pLsPara->pMapLs=NULL;
			}
			break;
		}
	}
	return TRUE;	 
}
//////////////////////////////////////////////////////////////////////////
// CAngleBoltsDlg 对话框
IMPLEMENT_DYNAMIC(CAngleBoltsDlg, CRememberLocationDlg)
CAngleBoltsDlg::CAngleBoltsDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CAngleBoltsDlg::IDD, pParent)
{
	m_pBaseRod=NULL;
	m_pCrossRod=NULL;
	m_iOffsetWing=0;
	m_iRayDirection=0;
	m_iLsLayStyle=0;
	m_iBoltNorm=0;
	m_nG = 0;
	m_nG1 = 0;
	m_nG2 = 0;
	m_nCrossAngleG=0;
	m_bProject=TRUE;
	m_bPlaneOffset=FALSE;
	m_fHoleD=0;
	m_nLsNum=0;
	m_nLsSpace=0;
	m_nOffset0=0;
	m_iBoltRows=0;
	m_bMirLayLs=FALSE;
	m_iHoleFuncType=0;
	m_bPunchHole=TRUE;
	m_bDrillHole=!m_bPunchHole;
	face_norm_offset.Set(0,0,1);
}

CAngleBoltsDlg::~CAngleBoltsDlg()
{
}

void CAngleBoltsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_S_BASE_ANGLE, m_sBaseAngleSize);
	DDX_Text(pDX, IDC_S_CROSS_ANGLE, m_sRayAngleSize);
	DDX_Text(pDX, IDC_S_DATUM_POINT, m_sDatumPtText);
	DDX_Text(pDX, IDC_S_DATUM_POINT2, m_sDatumPtText2);
	DDX_Text(pDX, IDC_E_G, m_nG);
	DDX_Text(pDX, IDC_E_G1, m_nG1);
	DDX_Text(pDX, IDC_E_G2, m_nG2);
	DDX_Text(pDX, IDC_E_CROSS_ANGLE_G, m_nCrossAngleG);
	DDX_Check(pDX, IDC_CHK_PROJECT, m_bProject);
	DDX_Check(pDX, IDC_CHK_PLANE_OFFSET, m_bPlaneOffset);
	DDX_CBString(pDX, IDC_CMB_BOLT_D, m_sBoltSize);
	DDX_Text(pDX, IDC_E_HOLE_D, m_fHoleD);
	DDX_Text(pDX, IDC_E_SPACE, m_nLsSpace);
	DDX_Text(pDX, IDC_E_END_SPACE, m_nOffset0);
	DDX_Text(pDX, IDC_E_NUM, m_nLsNum);
	DDX_CBIndex(pDX, IDC_CMB_LS_ROWS, m_iBoltRows);
	DDX_CBIndex(pDX, IDC_CMB_HOLE_TYPE, m_iHoleFuncType);
	DDX_Check(pDX, IDC_CHK_PUNCH, m_bPunchHole);
	DDX_Check(pDX, IDC_CHK_DRILL, m_bDrillHole);
	DDX_Control(pDX, IDC_LIST_LS_SET, m_xListCtrlLsSet);
}


BEGIN_MESSAGE_MAP(CAngleBoltsDlg, CRememberLocationDlg)
	ON_BN_CLICKED(IDC_CHK_PLANE_OFFSET, OnBnClickedChkPlaneOffset)
	ON_BN_CLICKED(IDC_CHK_PROJECT, OnBnClickedChkProject)
	ON_BN_CLICKED(IDC_BN_TWO_EDGE_LAYOUT, OnBnTwoEdgeLayBolts)
	ON_BN_CLICKED(IDC_BN_SINGLE_LAYOUT,OnBnSingleLayBolts)
	ON_BN_CLICKED(IDC_BN_SWITCH_G, OnBnSwitchG)
	ON_CBN_SELCHANGE(IDC_CMB_BOLT_D, OnChangeCmbBoltSize)
	ON_CBN_SELCHANGE(IDC_CMB_LS_ROWS, OnSelchangeCmbLsRows)
	ON_EN_CHANGE(IDC_E_NUM, OnChangeELsNum)
	ON_BN_CLICKED(IDC_CHK_PUNCH, OnClickedChkPunchHole)
	ON_BN_CLICKED(IDC_CHK_DRILL, OnClickedChkDrillHole)
	ON_NOTIFY(NM_CLICK, IDC_LIST_LS_SET, OnClickListLsSet)
END_MESSAGE_MAP()


// CAngleBoltsDlg 消息处理程序
BOOL CAngleBoltsDlg::OnInitDialog()
{
	if(m_pBaseRod==NULL)
		return FALSE;
	CDialog::OnInitDialog();
	//
	CXhChar16 sSpec;
	m_pBaseRod->GetSpecification(sSpec,FALSE);
	m_sBaseAngleSize.Format("编号:%s %s",(char*)m_pBaseRod->GetPartNo(),(char*)sSpec);
	m_nG = m_nG1 = m_nG2 = 0;
	f3dPoint work_norm;
	if(m_pBaseRod->GetClassTypeId()==CLS_LINEANGLE)
	{
		CLDSLineAngle* pAngle=(CLDSLineAngle*)m_pBaseRod;
		char cWorkWing=m_iOffsetWing==0?'X':'Y';
		JGZJ jgzj=pAngle->GetZhunJu(cWorkWing);
		if(section.pSectNode!=NULL)
		{
			if(m_iOffsetWing==0)
				jgzj.g=(int)pAngle->GetNodeWingXG(section.pSectNode);
			else
				jgzj.g=(int)pAngle->GetNodeWingYG(section.pSectNode);
		}
		m_nG=jgzj.g;
		m_nG1=jgzj.g1;
		m_nG2=jgzj.g2;
		if(pAngle->desStartPos.IsFaceOffset())
			face_norm_offset=pAngle->desStartPos.face_offset_norm;
		else if(pAngle->desEndPos.IsFaceOffset())
			face_norm_offset=pAngle->desEndPos.face_offset_norm;
		//根据当前视图法线智能判断螺栓朝向
		work_norm=(m_iOffsetWing==0)?pAngle->vxWingNorm:pAngle->vyWingNorm;
		double dd=work_norm*viewNorm;
		m_iBoltNorm=(dd>0)?0:1;
		if(m_iBoltNorm==0)
			m_tBoltNormOffset.AddThick(-m_pBaseRod->handle,TRUE);
	}
	else if(m_pBaseRod->GetClassTypeId()==CLS_LINEFLAT)
	{
		CLDSLineFlat* pFlat=(CLDSLineFlat*)m_pBaseRod;
		work_norm=pFlat->WorkPlaneNorm();
		double dd=work_norm*viewNorm;
		m_iBoltNorm=(dd>0)?0:1;
		if(m_iBoltNorm==0)
			m_tBoltNormOffset.AddThick(-m_pBaseRod->handle,TRUE);
	}
	else if(m_pBaseRod->GetClassTypeId()==CLS_LINESLOT)
	{
		CLDSLineSlot* pSlot=(CLDSLineSlot*)m_pBaseRod;
		work_norm=pSlot->WorkPlaneNorm();
		double dd=work_norm*viewNorm;
		m_iBoltNorm=(dd>0)?0:1;
		if(m_iBoltNorm==0||m_iOffsetWing>0)
			m_tBoltNormOffset.AddThick(-m_pBaseRod->handle,TRUE);
		if(m_iOffsetWing>0)
			m_nG=ftoi(pSlot->GetWidth()*0.5);
	}	
	//
	if(section.pSectNode!=NULL)
	{	//根据选中节点，初始化螺栓设计信息
		m_iDatumPtStyle=1;		//节点在心线投影
		m_sRayAngleSize.Format("节点：0X%X",section.pSectNode->handle);
	}
	else if(m_pCrossRod)
	{	//根据选中交叉角钢，初始化螺栓设计信息
		m_iDatumPtStyle=3;		//角钢心线交点
		m_pCrossRod->GetSpecification(sSpec,FALSE);
		m_sRayAngleSize.Format("编号:%s %s",(char*)m_pCrossRod->GetPartNo(),(char*)sSpec);
		m_sDatumPtText.Format("基点位于心线交点");
		JGZJ jgzj;
		getjgzj(jgzj,m_pCrossRod->GetWidth());
		m_nCrossAngleG=jgzj.g;
	}
	else
	{
		m_iDatumPtStyle = 0;
		m_sRayAngleSize.Format("螺栓基点定位:");
		if(section.ciSectType == 1)
			m_sDatumPtText2.Format("基点位于角钢楞点(始端)");
		else //if(section.ciSectType == 2)
			m_sDatumPtText2.Format("基点位于角钢楞点(终端)");
	}
	//
	if(section.ciSectType==2||(section.pSectNode&&m_pBaseRod->pEnd==section.pSectNode))
	{
		m_iRayDirection=1;	//终－>始
		m_nLsNum = (section.ciSectType == 2)?1:m_pBaseRod->wnEndConnBoltN;
		m_sBoltSize.Format("%d",m_pBaseRod->connectEnd.d);
		m_iBoltRows=m_pBaseRod->connectEnd.rows-1;
	}
	else if(section.ciSectType==1||(section.pSectNode&&m_pBaseRod->pStart==section.pSectNode))
	{
		m_iRayDirection=0;	//始－>终
		m_nLsNum = (section.ciSectType == 1)?1:m_pBaseRod->wnStartConnBoltN;
		m_sBoltSize.Format("%d",m_pBaseRod->connectStart.d);
		m_iBoltRows=m_pBaseRod->connectStart.rows-1;
	}
	else 
	{	
		m_iRayDirection=0;	//始－>终
		m_nLsNum=1;
		m_sBoltSize.Format("%d",SelBoltDByWingWide(m_pBaseRod->GetWidth()));
	}
	if(m_nLsNum>=5&&m_nG1>0)	//螺栓数大于5时需要布置双排螺栓
		m_iBoltRows=1;	
	m_fHoleD=atoi(m_sBoltSize)+1.5;
	UpdateLsSpaceData();
	//初始化ListCtrl
	m_xListCtrlLsSet.EmptyColumnHeader();
	m_xListCtrlLsSet.AddColumnHeader("",20);
#ifdef AFX_TARG_ENU_ENGLISH
	m_xListCtrlLsSet.AddColumnHeader("Specs",60);
	m_xListCtrlLsSet.AddColumnHeader("Datum Point Locate Style",108);
#else 
	m_xListCtrlLsSet.AddColumnHeader("规格",60);
	m_xListCtrlLsSet.AddColumnHeader("基点定位方式",108);
#endif
	m_xListCtrlLsSet.AddColumnHeader("ΔL",60);
	m_xListCtrlLsSet.AddColumnHeader("g",60);
	if(m_iDatumPtStyle==3)
		m_xListCtrlLsSet.AddColumnHeader("g'",60);
	m_xListCtrlLsSet.SetModifyValueFunc(FireValueModify);
	m_xListCtrlLsSet.SetDeleteItemFunc(FireDeleteItem);
	m_xListCtrlLsSet.InitListCtrl(NULL,FALSE);
	//
	UpdateContrlStatus();
	UpdateData(FALSE);
	//显示螺栓基准点的工作面
	WorkPlaneSketch();
	//移动对话框到左上角
	MoveWndToLocation();
	if(m_pBaseRod->IsAngle())
		SetWindowText("布置角钢螺栓");
	else if (m_pBaseRod->GetClassTypeId()==CLS_LINEFLAT)
		SetWindowText("布置扁铁螺栓");
	else
		SetWindowText("布置槽钢螺栓");
	return TRUE;
}
//判断是否启用交换心距
BOOL CAngleBoltsDlg::IsEnableSwitchG(int* g1/*=NULL*/,int* g2/*=NULL*/)
{
	if(m_xHashPrevViewBolts.GetNodeNum()<2)
		return FALSE;
	CHashList<int> hashBoltGs;
	if(m_xListCtrlLsSet.GetSelectedCount()==0)
	{
		int nCount=m_xListCtrlLsSet.GetCount();
		for(int i=0;i<nCount;i++)
		{
			CSuperGridCtrl::CTreeItem *pCurItem=m_xListCtrlLsSet.GetTreeItem(i);
			CDesignLsPara *pLsPara=(CDesignLsPara*)pCurItem->m_idProp;
			if(pLsPara==NULL)
				continue;
			CString gstr=m_xListCtrlLsSet.GetItemText(i,4);
			int g=atoi(gstr);
			hashBoltGs.SetValue(g,g);
		}
	}
	else
	{
		POSITION pos=m_xListCtrlLsSet.GetFirstSelectedItemPosition();
		while(pos!=NULL)
		{
			int iCurSel=m_xListCtrlLsSet.GetNextSelectedItem(pos);
			CSuperGridCtrl::CTreeItem *pCurItem=m_xListCtrlLsSet.GetTreeItem(iCurSel);
			CDesignLsPara *pLsPara=(CDesignLsPara*)pCurItem->m_idProp;
			if(pLsPara==NULL)
				continue;
			CString gstr=m_xListCtrlLsSet.GetItemText(iCurSel,4);
			int g=atoi(gstr);
			hashBoltGs.SetValue(g,g);
		}
	}
	if(hashBoltGs.GetNodeNum()!=2)
		return FALSE;
	if(g1 && g2)
	{
		*g1=*(hashBoltGs.GetFirst());
		*g2=*(hashBoltGs.GetTail());
	}
	return TRUE;
}
void CAngleBoltsDlg::UpdateContrlStatus()
{
	GetDlgItem(IDC_E_G1)->EnableWindow(m_iBoltRows==1);
	GetDlgItem(IDC_E_G2)->EnableWindow(m_iBoltRows==1);
	GetDlgItem(IDC_BN_SWITCH_G)->EnableWindow(IsEnableSwitchG());
	GetDlgItem(IDC_CHK_PROJECT)->ShowWindow(section.pSectNode!=NULL);
	GetDlgItem(IDC_CHK_PLANE_OFFSET)->ShowWindow(section.pSectNode!=NULL);
	GetDlgItem(IDC_E_CROSS_ANGLE_G)->ShowWindow(m_pCrossRod!=NULL);
	GetDlgItem(IDC_S_DATUM_POINT)->ShowWindow(m_pCrossRod!=NULL);
	GetDlgItem(IDC_S_DATUM_POINT2)->ShowWindow(m_iDatumPtStyle==0);
}
void CAngleBoltsDlg::UpdateLsSpaceData()
{
	LSSPACE_STRU LsSpace;
	long ls_d=atoi(m_sBoltSize);
	if(GetLsSpace(LsSpace,ls_d))
	{
		if(m_iBoltRows==0)
			m_nLsSpace = LsSpace.SingleRowSpace;
		else
			m_nLsSpace = LsSpace.doubleRowSpace;
	}
}
void CAngleBoltsDlg::InitDesignLsPara(CDesignLsPara& ls_para)
{
	ls_para.d=atoi(m_sBoltSize);
	ls_para.iSeg=m_pBaseRod->iSeg;
	//螺栓法线设定
	if(m_pBaseRod->GetClassTypeId()==CLS_LINEANGLE)
	{
		ls_para.des_work_norm.norm_style=1;				//角钢肢法向
		ls_para.des_work_norm.norm_wing=m_iOffsetWing;	//所在肢
		ls_para.des_work_norm.hVicePart=m_pBaseRod->handle;
		ls_para.des_work_norm.direction=m_iBoltNorm;
	}
	else if(m_pBaseRod->GetClassTypeId()==CLS_LINEFLAT)
	{	
		ls_para.des_work_norm.norm_style=3;
		ls_para.des_work_norm.norm_wing=m_iOffsetWing;
		ls_para.des_work_norm.hVicePart=m_pBaseRod->handle;
		ls_para.des_work_norm.direction=m_iBoltNorm;
	}
	else
	{
		ls_para.des_work_norm.norm_style=5;
		ls_para.des_work_norm.hVicePart=m_pBaseRod->handle;
		if(m_iOffsetWing==0)
		{
			if(m_iBoltNorm==0)	//朝外
				ls_para.des_work_norm.direction=4;
			else
				ls_para.des_work_norm.direction=5;
		}
		else if(m_iOffsetWing==1)
			ls_para.des_work_norm.direction=2;
		else if(m_iOffsetWing==2)
			ls_para.des_work_norm.direction=3;
	}
	//螺栓位置设定
	ls_para.des_base_pos.hPart=m_pBaseRod->handle;
	ls_para.des_base_pos.direction=m_iRayDirection;
	ls_para.des_base_pos.offset_wing=m_iOffsetWing;
	ls_para.des_base_pos.norm_offset=m_tBoltNormOffset;
	if(section.pSectNode!=NULL)
	{	//定位方式默认为节点向角钢心线投影
		ls_para.des_base_pos.datumPoint.datum_pos_style=2;
		ls_para.des_base_pos.datumPoint.des_para.RODNODE.direction=m_iRayDirection;
		ls_para.des_base_pos.datumPoint.des_para.RODNODE.hRod =m_pBaseRod->handle;
		ls_para.des_base_pos.datumPoint.des_para.RODNODE.hNode=section.pSectNode->handle;
		ls_para.des_base_pos.datumPoint.des_para.RODNODE.offset_wing=m_iOffsetWing;
		if(m_iDatumPtStyle==1)
			ls_para.des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset=FALSE;
		else
		{
			ls_para.des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset=TRUE;
			ls_para.des_base_pos.datumPoint.des_para.RODNODE.vPlaneNormal=face_norm_offset;
		}
		ls_para.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style=4;
		ls_para.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist =m_nG;
	}
	else if(m_pCrossRod)
	{	//角钢心线交点为定位基点
		ls_para.des_base_pos.datumPoint.datum_pos_style=3;
		ls_para.des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1=m_pBaseRod->handle;
		ls_para.des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2=m_pCrossRod->handle;
		ls_para.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = m_nG;
		ls_para.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2 = m_nCrossAngleG;
		ls_para.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
		ls_para.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=4;
	}
	else if (section.ciSectType == 1 || section.ciSectType == 2)
	{
		ls_para.des_base_pos.datumPoint.datum_pos_style=1;
		ls_para.des_base_pos.datumPoint.des_para.RODEND.direction=section.ciSectType-1;//m_iRayDirection;
		ls_para.des_base_pos.datumPoint.des_para.RODEND.hRod =m_pBaseRod->handle;
		ls_para.des_base_pos.datumPoint.des_para.RODEND.offset_wing=m_iOffsetWing;
		ls_para.des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect = true;
		ls_para.des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=4;
		ls_para.des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist =m_nG;
	}
}
void CAngleBoltsDlg::LayoutBolts()
{
	try{
		CDesignLsPara ls_stru;
		InitDesignLsPara(ls_stru);
		//根据用户输入螺栓信息进行螺栓布置
		for(int i=0;i<m_nLsNum;i++)
		{
			if(m_bMirLayLs)		//两侧对称布孔
			{	//纵向偏移
				if(m_iBoltRows==0)	//单排两侧对称布孔
				{
					if(i==0)
						ls_stru.des_base_pos.len_offset_dist=-(m_nLsNum/2)*m_nLsSpace+(m_nLsNum+1)%2*m_nLsSpace*0.5;
					else
						ls_stru.des_base_pos.len_offset_dist+=m_nLsSpace;
				}
				else				//双排两侧对称布孔
				{
					if(i==0)
						ls_stru.des_base_pos.len_offset_dist=-m_nLsSpace*(m_nLsNum-1)/4;
					else
						ls_stru.des_base_pos.len_offset_dist+=0.5*m_nLsSpace;
				}
				//肢向偏移
				if(m_iBoltRows==0)	//单排两侧对称布孔
					ls_stru.des_base_pos.wing_offset_dist=0;
				else				//双排两侧对称布孔
				{
					if(i%2==0)
						ls_stru.des_base_pos.wing_offset_dist=(m_iLsLayStyle==0)?(m_nG1-m_nG):(m_nG2-m_nG);
					else
						ls_stru.des_base_pos.wing_offset_dist=(m_iLsLayStyle==0)?(m_nG2-m_nG):(m_nG1-m_nG);
				}
			}
			else	//单侧布孔
			{	
				//纵向偏移
				if(i==0)
					ls_stru.des_base_pos.len_offset_dist=m_nOffset0;
				else if(m_iBoltRows==0)	//单排单向布孔
					ls_stru.des_base_pos.len_offset_dist+=m_nLsSpace;
				else					//双排单向布孔
					ls_stru.des_base_pos.len_offset_dist+=0.5*m_nLsSpace;
				//肢向偏移
				if(m_iBoltRows==0)	//单排
					ls_stru.des_base_pos.wing_offset_dist=0;
				else				//双排
				{
					if(i%2==0)
						ls_stru.des_base_pos.wing_offset_dist=(m_iLsLayStyle==0)?(m_nG1-m_nG):(m_nG2-m_nG);
					else
						ls_stru.des_base_pos.wing_offset_dist=(m_iLsLayStyle==0)?(m_nG2-m_nG):(m_nG1-m_nG);
				}
			}
			//角钢心线交点布置螺栓或存在端连接时对称布置螺栓应保证一个螺栓位于基点位置
			if(m_bMirLayLs)
			{
				int iFirstOutterBoltIndex=-1,iFirstOutterBoltIndex2=1;
				iFirstOutterBoltIndex=m_nLsNum/2-1;
				if(m_nLsNum%2>0)
					iFirstOutterBoltIndex2=m_nLsNum/2+1;
				else
					iFirstOutterBoltIndex2=m_nLsNum/2;
				if(i>iFirstOutterBoltIndex&&i<iFirstOutterBoltIndex2&&m_iDatumPtStyle==3)
					ls_stru.des_base_pos.len_offset_dist=m_nOffset0;
			}
			m_pLsList->append(ls_stru);
		}
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
}
void CAngleBoltsDlg::WorkPlaneSketch()
{
	DelWorkPlaneSketch();
	GEPOINT base_pos,vFrontDirection;
	BYTE direction=0;
	if(m_xHashPrevViewBolts.GetNodeNum()==0)
	{
		CDesignLsPara ls_para;
		InitDesignLsPara(ls_para);
		ls_para.des_base_pos.datumPoint.UpdatePos(m_pBaseRod->BelongModel());
		base_pos=ls_para.des_base_pos.datumPoint.Position();
		ls_para.des_work_norm.UpdateVector(m_pBaseRod->BelongModel());
		vFrontDirection=ls_para.des_work_norm.vector;
		direction=ls_para.des_base_pos.direction;
	}
	else
	{
		CLDSBolt* pBolt=m_xHashPrevViewBolts.GetFirst();
		vFrontDirection=g_pSolidOper->TransVFromGL(GEPOINT(0,0,1));
		base_pos=pBolt->ucs.origin;
		if(vFrontDirection*pBolt->ucs.axis_z<0)
			vFrontDirection=-pBolt->ucs.axis_z;
		else
			vFrontDirection=pBolt->ucs.axis_z;
		base_pos+=vFrontDirection*pBolt->get_L();
		direction=pBolt->des_base_pos.direction;
	}
	m_xLineVec=m_pBaseRod->End()-m_pBaseRod->Start();
	if(direction==1)
		m_xLineVec=m_pBaseRod->Start()-m_pBaseRod->End();
	normalize(m_xLineVec);
	if(m_pBaseRod->GetClassTypeId()==CLS_LINEANGLE)
	{
		CLDSLineAngle* pAngle=(CLDSLineAngle*)m_pBaseRod;
		m_xWingOffsetVec=(m_iOffsetWing==0)?pAngle->GetWingVecX():pAngle->GetWingVecY();
	}
	else if(m_pBaseRod->GetClassTypeId()==CLS_LINESLOT)
	{
		CLDSLineSlot* pSlot=(CLDSLineSlot*)m_pBaseRod;
		if(m_iOffsetWing>0)
			m_xWingOffsetVec=m_pBaseRod->ucs.axis_y;
		else
			m_xWingOffsetVec=m_pBaseRod->ucs.axis_x;
	}
	else if(m_pBaseRod->GetClassTypeId()==CLS_LINEFLAT)
		m_xWingOffsetVec=m_pBaseRod->ucs.axis_y;
	btc::SKETCH_PLANE sketch(base_pos,m_xLineVec,vFrontDirection);
	g_pSolidDraw->NewWorkPlaneSketch(1,RGB(152,152,152),sketch.pVertexArr,sketch.VertexCount,sketch.normal,"ΔL");
	g_pSolidDraw->NewWorkPlaneSketch(2,RGB(152,152,152),&base_pos,1,m_xWingOffsetVec,"ΔG",75);
	g_pSolidDraw->Draw();
}
void CAngleBoltsDlg::DelWorkPlaneSketch()
{
	if(!m_xLineVec.IsZero())
		g_pSolidDraw->DelWorkPlaneSketch(1);
	if(!m_xWingOffsetVec.IsZero())
		g_pSolidDraw->DelWorkPlaneSketch(2);
	g_pSolidDraw->Draw();
}
void CAngleBoltsDlg::PreviewBolt(CDesignLsPara *pLsPara)
{
	if(pLsPara->pMapLs==NULL)
	{	//设置plsPara对应的关联螺栓
		for(CLDSBolt *pBolt=m_xHashPrevViewBolts.GetFirst();pBolt;pBolt=m_xHashPrevViewBolts.GetNext())
		{
			if(pBolt->feature==0)
			{
				pLsPara->pMapLs=pBolt;
				pBolt->feature=(long)pLsPara;
				break;
			}
		}
		if(pLsPara->pMapLs==NULL)
		{
			pLsPara->pMapLs=m_xHashPrevViewBolts.Add(0);
			pLsPara->pMapLs->handle=m_xHashPrevViewBolts.GetCursorKey();
			pLsPara->pMapLs->feature=(long)pLsPara;
		}
	}
	if(pLsPara->pMapLs&&m_xHashPrevViewBolts.GetValue(pLsPara->pMapLs->handle))
	{
		pLsPara->DesignLs(pLsPara->pMapLs);
		pLsPara->pMapLs->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pLsPara->pMapLs->GetSolidPartObject());
		g_pSolidDraw->Draw();
	}
}
void CAngleBoltsDlg::AppendRecord(CString*str_arr,void* pLsPara/*=NULL*/)
{
	CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
	BOOL bEidt=(pLsPara==NULL)?TRUE:FALSE;
	lpInfo->SetSubItemText(0,"",TRUE);
	lpInfo->SetSubItemText(1,str_arr[0],bEidt);
	lpInfo->SetControlType(1,GCT_CMB_EDIT);
	lpInfo->SetListItemsStr(1,"M12|M16|M20|M24|M27");
	lpInfo->SetSubItemText(2,str_arr[1],TRUE);
	lpInfo->SetSubItemText(3,str_arr[2],bEidt);
	lpInfo->SetControlType(3,GCT_EDIT);
	lpInfo->SetSubItemText(4,str_arr[3],bEidt);
	lpInfo->SetControlType(4,GCT_EDIT);
	if(strlen(str_arr[4])>0)
		lpInfo->SetSubItemText(5,str_arr[4],bEidt);
	CSuperGridCtrl::CTreeItem* pItem = m_xListCtrlLsSet.InsertRootItem(lpInfo);
	if(pLsPara)
		pItem->m_idProp = (DWORD)pLsPara;
}
void CAngleBoltsDlg::RefreshLsListCtrl()
{
	m_xListCtrlLsSet.DeleteAllItems();
	CString str_arr[5];
	for(CDesignLsPara* pLsPara=m_pLsList->GetFirst();pLsPara;pLsPara=m_pLsList->GetNext())
	{
		str_arr[0].Format("M%d",pLsPara->d);
		if(pLsPara->des_base_pos.datumPoint.datum_pos_style==3)
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[1] = "Center Line Intersection Point";
#else 
			str_arr[1] = "心线交点";
#endif
		else if(pLsPara->des_base_pos.datumPoint.datum_pos_style==2)
		{
			if(!pLsPara->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset)
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[1] = "Node project to angle's center line";
			else
				str_arr[1] = "Node offset to angle's center line";
#else 
				str_arr[1] = "节点向心线投影";
			else
				str_arr[1] = "节点向心线平推";
#endif
		}
		str_arr[2].Format("%f",pLsPara->des_base_pos.len_offset_dist);
		SimplifiedNumString(str_arr[2]);
		str_arr[3].Format("%f",pLsPara->des_base_pos.wing_offset_dist+GetLsParaDatumG(pLsPara));
		SimplifiedNumString(str_arr[3]);
		if(m_iDatumPtStyle==3)
			str_arr[4].Format("%d",pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2);
		AppendRecord(str_arr,(void *)pLsPara);
		PreviewBolt(pLsPara);
	}
}
//
void CAngleBoltsDlg::OnBnSingleLayBolts()
{
	UpdateData();
	m_bMirLayLs=FALSE;	//非对称布置
	LayoutBolts();
	RefreshLsListCtrl();
	UpdateContrlStatus();
	WorkPlaneSketch();
	UpdateData(FALSE);
}
void CAngleBoltsDlg::OnBnTwoEdgeLayBolts()
{
	UpdateData();
	m_bMirLayLs=TRUE;	//对称布置
	LayoutBolts();
	RefreshLsListCtrl();
	UpdateContrlStatus();
	WorkPlaneSketch();
	UpdateData(FALSE);
}
//螺栓多排布置时，支持交换心距
void CAngleBoltsDlg::OnBnSwitchG()
{
	UpdateData();
	int g1=0,g2=0;
	if(!IsEnableSwitchG(&g1,&g2))
		return;
	if(m_xListCtrlLsSet.GetSelectedCount()==0)
	{
		for(int i=0;i<m_xListCtrlLsSet.GetItemCount();i++)
		{
			CSuperGridCtrl::CTreeItem *pCurItem=m_xListCtrlLsSet.GetTreeItem(i);
			CDesignLsPara *pLsPara=(CDesignLsPara*)pCurItem->m_idProp;
			if(pLsPara==NULL)
				continue;
			CString gstr=m_xListCtrlLsSet.GetItemText(i,4);
			int g=atoi(gstr);
			if(g==g1)
				pLsPara->des_base_pos.wing_offset_dist=g2-GetLsParaDatumG(pLsPara);
			else if(g==g2)
				pLsPara->des_base_pos.wing_offset_dist=g1-GetLsParaDatumG(pLsPara);
		}
	}
	else
	{
		POSITION pos=m_xListCtrlLsSet.GetFirstSelectedItemPosition();
		while(pos!=NULL)
		{
			int iCurSel=m_xListCtrlLsSet.GetNextSelectedItem(pos);
			CSuperGridCtrl::CTreeItem *pCurItem=m_xListCtrlLsSet.GetTreeItem(iCurSel);
			CDesignLsPara *pLsPara=(CDesignLsPara*)pCurItem->m_idProp;
			if(pLsPara==NULL)
				continue;
			CString gstr=m_xListCtrlLsSet.GetItemText(iCurSel,4);
			int g=atoi(gstr);
			if(g==g1)
				pLsPara->des_base_pos.wing_offset_dist=g2-GetLsParaDatumG(pLsPara);
			else if(g==g2)
				pLsPara->des_base_pos.wing_offset_dist=g1-GetLsParaDatumG(pLsPara);
		}
	}
	//更新ListCtrl数据显示
	for(int i=0;i<m_xListCtrlLsSet.GetItemCount();i++)
	{
		CSuperGridCtrl::CTreeItem *pCurItem=m_xListCtrlLsSet.GetTreeItem(i);
		CDesignLsPara *pLsPara=(CDesignLsPara*)pCurItem->m_idProp;
		if(pLsPara==NULL)
			continue;
		m_xListCtrlLsSet.SetItemText(i,4,CXhChar16("%.f",pLsPara->des_base_pos.wing_offset_dist+GetLsParaDatumG(pLsPara)));
		PreviewBolt(pLsPara);
	}
	UpdateData(FALSE);
}
void CAngleBoltsDlg::OnChangeCmbBoltSize() 
{
	UpdateData();
	CString sText;
	CComboBox *pCmb = (CComboBox*)GetDlgItem(IDC_CMB_BOLT_D);
	int iCur = pCmb->GetCurSel();
	pCmb->GetLBText(iCur,m_sBoltSize);
	int ls_d = atoi(m_sBoltSize);
	m_fHoleD=ls_d+1.5;
	UpdateLsSpaceData();
	UpdateData(FALSE);
}
void CAngleBoltsDlg::OnSelchangeCmbLsRows()
{
	UpdateData();
	UpdateLsSpaceData();
	UpdateContrlStatus();
	UpdateData(FALSE);
}
void CAngleBoltsDlg::OnChangeELsNum()
{
#ifdef __SMART_DRAW_INC_
	UpdateData();
	if(m_nLsNum>=5)
		m_iBoltRows=1;
	else
		m_iBoltRows=0;
	UpdateLsSpaceData();
	UpdateContrlStatus();
	UpdateData(FALSE);
#endif
}
void CAngleBoltsDlg::OnBnClickedChkProject()
{
	m_bProject=TRUE;
	m_bPlaneOffset=!m_bProject;
	m_iDatumPtStyle=1;	//节点在心线投影
	UpdateData(FALSE);
}
void CAngleBoltsDlg::OnBnClickedChkPlaneOffset()
{
	m_bPlaneOffset=TRUE;
	m_bProject=!m_bPlaneOffset;
	m_iDatumPtStyle=2;	//节点在心线平推
	UpdateData(FALSE);
}
void CAngleBoltsDlg::OnClickedChkPunchHole()
{
	UpdateData();
	m_bDrillHole=!m_bPunchHole;
	UpdateData(FALSE);
}
void CAngleBoltsDlg::OnClickedChkDrillHole()
{
	UpdateData();
	m_bPunchHole=!m_bDrillHole;
	UpdateData(FALSE);
}
void CAngleBoltsDlg::OnClickListLsSet(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	//
	g_pSolidDraw->ReleaseSnapStatus();
	POSITION pos=m_xListCtrlLsSet.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel=m_xListCtrlLsSet.GetNextSelectedItem(pos);
		CSuperGridCtrl::CTreeItem *pCurItem=m_xListCtrlLsSet.GetTreeItem(iCurSel);
		CDesignLsPara *pLsPara=(CDesignLsPara*)pCurItem->m_idProp;
		if(pLsPara&&pLsPara->pMapLs)
			g_pSolidDraw->SetEntSnapStatus(pLsPara->pMapLs->handle);
	}
	UpdateContrlStatus();
	*pResult = 0;
}
BOOL UnifyAngleBoltParamG(DESIGN_LS_POS &designLsPos);
void CAngleBoltsDlg::OnOK()
{
	if(m_pLsList&&m_pLsList->GetNodeNum()==0&&m_nLsNum>0)
	{
		LayoutBolts(); 
		WorkPlaneSketch();
	}
	for(CDesignLsPara* pLsPara=m_pLsList->GetFirst();pLsPara;pLsPara=m_pLsList->GetNext())
		UnifyAngleBoltParamG(pLsPara->des_base_pos);	//统一角钢螺栓准据参数
	//删除用于预显示的螺栓
	for(CLDSBolt *pBolt=m_xHashPrevViewBolts.GetFirst();pBolt;pBolt=m_xHashPrevViewBolts.GetNext())
		g_pSolidDraw->DelEnt(pBolt->handle);
	DelWorkPlaneSketch();
	CDialog::OnOK();
}
void CAngleBoltsDlg::OnCancel()
{
	for(CLDSBolt *pBolt=m_xHashPrevViewBolts.GetFirst();pBolt;pBolt=m_xHashPrevViewBolts.GetNext())
		g_pSolidDraw->DelEnt(pBolt->handle);
	DelWorkPlaneSketch();
	CDialog::OnCancel();
}
