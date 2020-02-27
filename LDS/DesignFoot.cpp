//<LOCALE_TRANSLATE Confirm by hxr/>
#include "stdafx.h"
#include "LDS.h"
#include "XhCharString.h"
#include "Tower.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "CmdLineDlg.h"
#include "FootDesignTemplDlg.h"
#include "SnapTypeVerify.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
#ifdef __ANGLE_PART_INC_

int CLDSView::DesignFoot()
{
	g_pSolidDraw->ReleaseSnapStatus();
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//如果默认当前为开窗选择状态时进行DesignFoot将会由于3D绘图引擎为绘图状态，导致不能点选构件 wjh-2016.9.9
	g_pSolidSet->SetOperType(OPER_OTHER);
	//1.切换到单线图状态便于选择节点
	g_pSolidSet->SetDisplayType(DISP_LINE);
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select dependent nodes on base plate:","");
#else
	pCmdLine->FillCmdLine("请选择底座板的依赖节点:","");
#endif
	CLDSNode *pBaseNode=NULL;
	while(1)
	{
		f3dPoint* point;
		int ret = g_pSolidSnap->SnapPoint(point);
		if(ret<0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		else if(ret>0)
		{
			pBaseNode=console.FromNodeHandle(point->ID);
			if(pBaseNode)
			{
				g_pSolidDraw->SetEntSnapStatus(point->ID);
				break;
			}
		}
	}
	CString cmdStr;
	//2.选择确认塔脚所连接的主材角钢
	CSuperSmartPtr<CLDSLinePart> pAngle,pBaseAngle,pDownBaseAngle;
	for(pAngle=Ta.Parts.GetFirstLinePart();pAngle.IsHasPtr();pAngle=Ta.Parts.GetNextLinePart())
	{
		if(  pAngle->IsAngle()&&(pAngle->layer(0)=='S'||pAngle->layer(0)=='L')&&pAngle->layer(1)=='Z'&&
		   ((pAngle->pStart&&pAngle->pStart==pBaseNode)||(pAngle->pEnd&&pAngle->pEnd==pBaseNode)))
		{
			if(pAngle->GetClassTypeId()==CLS_LINEANGLE&&pAngle.LineAnglePointer()->group_father_jg_h>0)
				continue;	//跳过组合角钢的子角钢
			else if(pAngle->m_bVirtualPart)
				continue;	//跳过虚拟辅助角钢
			if(pBaseAngle.IsNULL())
				pBaseAngle=pAngle;
			else if(pDownBaseAngle.IsNULL())
				pDownBaseAngle=pAngle;
			if(pBaseAngle.IsHasPtr()&&pDownBaseAngle.IsHasPtr())
				break;
		}
	}
	g_pSolidDraw->ReleaseSnapStatus();
	if(pBaseAngle.IsHasPtr())
		g_pSolidDraw->SetEntSnapStatus(pBaseAngle->handle);
	if(pDownBaseAngle.IsHasPtr())
		g_pSolidDraw->SetEntSnapStatus(pDownBaseAngle->handle);
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("select and confirm the leg angle connected with tower foot:","");
#else
	pCmdLine->FillCmdLine("所选择确认塔脚所连接的主材角钢:","");
#endif
	long* id_arr=NULL;
	do{
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
			return FALSE;
		if(g_pSolidSnap->GetLastSelectEnts(id_arr)>0)
			pBaseAngle=Ta.FromRodHandle(id_arr[0]);
		else if(cmdStr.GetLength()>0)
			pBaseAngle=Ta.FromRodHandle(HexStrToLong(cmdStr));
		if(pBaseAngle.IsNULL()||(pBaseAngle.IsHasPtr()&&!pBaseAngle->IsAngle()))
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("the currently selected is not angle type!");
		if(pBaseAngle.IsHasPtr())
			break;
		pCmdLine->FillCmdLine("select and confirm the leg angle connected with tower foot:","");
#else
			AfxMessageBox("当前选中对象不是角钢类型!");
		if(pBaseAngle.IsHasPtr())
			break;
		pCmdLine->FillCmdLine("所选择确认塔脚所连接的主材角钢:","");
#endif
	}while(pBaseAngle.IsNULL());
	f3dPoint lenDownVec,lenVec=pBaseAngle->Start()-pBaseAngle->End();
	if(pBaseAngle->pStart==pBaseNode)
		lenVec*=-1.0;
	if(pDownBaseAngle.IsHasPtr()&&lenVec.z>0)	//存在上下主材的牛蹄板
	{
		pAngle=pBaseAngle;
		pBaseAngle=pDownBaseAngle;
		pDownBaseAngle=pAngle;
		lenDownVec=lenVec;
		lenVec=pBaseAngle->Start()-pBaseAngle->End();
		if(pBaseAngle->pStart==pBaseNode)
			lenVec*=-1.0;
		if(lenVec.z>0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("two dependent legs of hoof plate collide in direction!");
#else
			logerr.Log("牛蹄板的两根连接依赖主材方向有冲突!");
#endif
			return FALSE;
		}
	}
	//3.确认是否导入已有塔脚设计模板
	static char cImportTempl;
	if(cImportTempl!='Y'&&cImportTempl!='N')
		cImportTempl='P';
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine((char*)CXhChar100("Whether to import exist template file[specified file(Y)/the lastest file(P)/None(N)]<%C>:",cImportTempl),"");
#else
	pCmdLine->FillCmdLine((char*)CXhChar100("是否导入已有塔脚模板文件快速生成塔脚结构[导入指定模板(Y)/导入上一次模板(P)/不导入(N)]<%C>:",cImportTempl),"");
#endif
	if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"Y|P|N"))
		return FALSE;
	if(cmdStr.GetLength()>0)
		cImportTempl=toupper(cmdStr[0]);
	CString templfile;
	if(cImportTempl=='Y')
	{
		CFileDialog dlg(TRUE,"ft","foottempl.ft",
#ifdef AFX_TARG_ENU_ENGLISH
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"tower design template file(*.ft)|*.ft|all files(*.*)|*.*||");
#else
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"塔脚设计模板文件(*.ft)|*.ft|所有文件(*.*)|*.*||");
#endif
		if(dlg.DoModal()==IDOK)
			templfile=dlg.GetPathName();
	}
	else if(cImportTempl=='P')
		templfile.Format("%slatest.ft",theApp.work_path);
	//4.初始化设计参数
  	CFootDesignTemplDlg dlg;
	dlg.foottempl.m_hBaseNode=pBaseNode->handle;	//定位节点
	dlg.foottempl.primaryAngle.pAngle=pBaseAngle.LineAnglePointer();	//基准角钢
	if(pDownBaseAngle.IsHasPtr())
		dlg.foottempl.primaryAngleDown.pAngle=pDownBaseAngle.LineAnglePointer();	//牛蹄板下侧基准角钢
	if(templfile.GetLength()==0||!dlg.foottempl.FromXmlTemplFile(templfile))
	{	//未导入模版,手动输入各项塔脚设计参数
		dlg.foottempl.primaryAngle.UpdateBoltInfo(pBaseNode);
		if(pDownBaseAngle.IsHasPtr())
			dlg.foottempl.primaryAngleDown.UpdateBoltInfo(pBaseNode);
	}
	if(dlg.DoModal()!=IDOK)
		return FALSE;
	//5、开始逐步设计塔脚
	CLogErrorLife loglife;
	CUndoOperObject undo(&Ta,true);
	if(dlg.foottempl.DesignFoot()==TRUE)
	{
		if(dlg.foottempl.base.pPlate)
		{
			dlg.foottempl.base.pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(dlg.foottempl.base.pPlate->GetSolidPartObject());
		}
		for(CFootTempl::SHOEPLATE* pShoePlate=dlg.foottempl.listShoePlates.GetFirst();pShoePlate;
			pShoePlate=dlg.foottempl.listShoePlates.GetNext())
		{
			if(pShoePlate->pPlate)
			{
				pShoePlate->pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pShoePlate->pPlate->GetSolidPartObject());
				for(CLsRef* pLsRef=pShoePlate->pPlate->GetFirstLsRef();pLsRef;pLsRef=pShoePlate->pPlate->GetNextLsRef())
				{
					CLDSBolt* pBolt=pLsRef->GetLsPtr();
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}
		}
		for(CFootTempl::RIBPLATE* pRibPlate=dlg.foottempl.listRibPlates.GetFirst();pRibPlate;
			pRibPlate=dlg.foottempl.listRibPlates.GetNext())
		{
			if(pRibPlate->pPlate)
			{
				pRibPlate->pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pRibPlate->pPlate->GetSolidPartObject());
			}
		}
		g_pSolidDraw->Draw();
		//
		if(!dlg.m_bSaveData)
		{	//将塔脚板设计数据保存到默认路径下
			templfile.Format("%slatest.ft",theApp.work_path);
			dlg.foottempl.ToXmlTemplFile(templfile);
		}
		else
		{	//另存到用户指定文件下
			CFileDialog file_dlg(FALSE,"ft","foottempl.ft",
#ifdef AFX_TARG_ENU_ENGLISH
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"tower design template file(*.ft)|*.ft|all files(*.*)|*.*||");
#else
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"塔脚设计模板文件(*.ft)|*.ft|所有文件(*.*)|*.*||");
#endif
			if(file_dlg.DoModal()!=IDOK)
				return FALSE;
			templfile = file_dlg.GetPathName();
			dlg.foottempl.ToXmlTemplFile(templfile);
		}
		return TRUE;
	}
	else
		return FALSE;
}
static BOOL RetrieveFootParts(PARTSET& part_set)
{
	long *id_arr=0;
	long nSel=g_pSolidSnap->GetLastSelectEnts(id_arr);
	for(long i=0;i<nSel;i++)
	{
		CLDSPart* pPart=console.FromPartHandle(id_arr[i]);
		if(pPart==NULL||pPart->GetClassTypeId()!=CLS_PARAMPLATE)
			continue;
		CLDSParamPlate* pParaPlate=(CLDSParamPlate*)pPart;
		//if(pParaPlate->m_cFuncType!=CLDSPart::FUNC_FOOTPLATE)	//过滤非塔脚板
			//continue;
		if(pParaPlate->m_iParamType==TYPE_BASEPLATE)
			part_set.append(pParaPlate);
		else if(pParaPlate->m_iParamType==TYPE_SHOEPLATE)
			part_set.append(pParaPlate);
		else if(pParaPlate->m_iParamType==TYPE_RIBPLATE)
			part_set.append(pParaPlate);
	}
	if(part_set.GetNodeNum()>3)
		return TRUE;
	return FALSE;
}
static bool IsWeldDiZuo(CFootTempl& foottempl)
{
	UCS_STRU ucs;
	foottempl.GetUCS(ucs,false);
	f3dPoint datumPlaneOrg=ucs.origin;
	datumPlaneOrg+=ucs.axis_z*(foottempl.base.nNormOffset+foottempl.base.thick);
	double odd_dist=0,dd=0;
	f3dPoint line_vec,vertice,ber_pos;
	if(foottempl.primaryAngle.pAngle->pStart->handle==foottempl.m_hBaseNode)
	{
		odd_dist=foottempl.primaryAngle.pAngle->startOdd();
		//
		line_vec=foottempl.primaryAngle.pAngle->Start()-foottempl.primaryAngle.pAngle->End();
		normalize(line_vec);
		ber_pos=foottempl.primaryAngle.pAngle->Start();
		Int3dlf(vertice,ber_pos,line_vec,ucs.origin,ucs.axis_z);
		dd=DISTANCE(vertice,ber_pos);
		if((datumPlaneOrg-ber_pos)*line_vec<0)
			dd*=-1.0;
	}
	else
	{
		odd_dist=foottempl.primaryAngle.pAngle->endOdd();
		//
		line_vec=foottempl.primaryAngle.pAngle->End()-foottempl.primaryAngle.pAngle->Start();
		normalize(line_vec);
		f3dPoint ber_pos = foottempl.primaryAngle.pAngle->End();
		Int3dlf(vertice,ber_pos,line_vec,ucs.origin,ucs.axis_z);
		dd=DISTANCE(vertice,ber_pos);
		if((datumPlaneOrg-ber_pos)*line_vec<0)
			dd*=-1.0;
	}
	if(odd_dist==dd)
		return true;
	return false;
}
void InitLineAngleBoltInfo(CFootTempl::LINKANGLE& xLinkAngle,CLDSParamPlate* pShoePlate)
{
	if(xLinkAngle.pAngle==NULL || pShoePlate->GetLsRefList()<=0)
		return;
	CLDSLineAngle* pBaseAngle=xLinkAngle.pAngle;
	if(pBaseAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		CLDSLineAngle* pBaseGroupAngle=(CLDSGroupLineAngle*)pBaseAngle;
		pBaseAngle=(CLDSLineAngle*)console.FromPartHandle(pBaseGroupAngle->son_jg_h[0]);
	}
	int nBoltNum=0,bolt_d=0;
	CHashList<int> hashBoltGs;
	for(CLsRef* pLsRef=pShoePlate->GetFirstLsRef();pLsRef;pLsRef=pShoePlate->GetNextLsRef())
	{
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		if(pBaseAngle->FindLsByHandle(pBolt->handle)==NULL)
			continue;
		bolt_d=pBolt->get_d();
		nBoltNum++;
		int g=(int)(pBolt->des_base_pos.wing_offset_dist+pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist);
		hashBoltGs.SetValue(g,g);
	}
	if(nBoltNum>1)
	{
		xLinkAngle.bolt.d=bolt_d;
		xLinkAngle.bolt.num=nBoltNum;
		xLinkAngle.bolt.rows=(hashBoltGs.GetNodeNum()==1)?0:1;
		xLinkAngle.bolt.layout=0;
	}
}
int CLDSView::ExportFootTemplate()
{
	if(!PRODUCT_FUNC::IsHasInternalTest())
		return 0;
	try{
		//捕捉角钢塔脚板
		CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
		CString cmdStr;
		PARTSET part_set;
		CCmdLockObject cmdlock(this);
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("FootTemplate please choose angle foot's param-plates:","");
#else
		pCmdLine->FillCmdLine("FootTemplate 请选择角钢塔脚板:","");
#endif
		g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_PARAMPLATE));
		while(1)
		{
			part_set.Empty();
			if(pCmdLine->GetStrFromCmdLine(cmdStr)==FALSE)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			if(RetrieveFootParts(part_set))
				break;
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("FootTemplate please choose angle foot's param-plates:","");
#else
			pCmdLine->FillCmdLine("FootTemplate 没有选中合适的塔脚板,请重新选择角钢塔脚板:","");
#endif	
			g_pSolidDraw->ReleaseSnapStatus();
		}
		pCmdLine->FinishCmdLine("FootTemplate 角钢塔脚板选择完毕!");
		//提取塔脚板模板的设计参数
		CUndoOperObject undo(&Ta,true);
		ATOM_LIST<CLDSParamPlate*> shoe_plate_list,rib_plate_list;
		CLDSParamPlate *pFootPlate=NULL,*pBasePlate=NULL;
		for(pFootPlate=(CLDSParamPlate*)part_set.GetFirst();pFootPlate;pFootPlate=(CLDSParamPlate*)part_set.GetNext())
		{
			if(pFootPlate->m_iParamType==TYPE_BASEPLATE)
				pBasePlate=pFootPlate;
			else if(pFootPlate->m_iParamType==TYPE_SHOEPLATE)
				shoe_plate_list.append(pFootPlate);
			else if(pFootPlate->m_iParamType==TYPE_RIBPLATE)
				rib_plate_list.append(pFootPlate);
		}
		if(pBasePlate==NULL||shoe_plate_list.GetNodeNum()<3)
		{
			AfxMessageBox("选中的塔脚板不支持导出为模板文件!");
			return FALSE;
		}
		double fValue=0;
		long handle=0,nValue=0;
		CLDSLineAngle *pDatumJg=NULL,*pDownDatumJg=NULL;
		CLDSNode* pBaseNode=pBasePlate->GetBaseNode();
		pBasePlate->GetDesignItemValue(KEY2C("L"),&handle);
		pDatumJg=(CLDSLineAngle*)console.FromPartHandle(handle,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pBaseNode==NULL||pDatumJg==NULL)
			return FALSE;
		CLDSParamPlate *pUpShoePlate=NULL,*pDownShoePlate=NULL;
		for(CLDSParamPlate** ppShoePlate=shoe_plate_list.GetFirst();ppShoePlate;ppShoePlate=shoe_plate_list.GetNext())
		{
			(*ppShoePlate)->GetDesignItemValue('D',&handle);	//靴板的依赖角钢
			CLDSLineAngle* pAngle=(CLDSLineAngle*)console.FromPartHandle(handle,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pAngle==NULL)
				continue;
			if(pUpShoePlate==NULL&&pAngle->handle==pDatumJg->handle)
				pUpShoePlate=(*ppShoePlate);
			if(pDownShoePlate==NULL&&pAngle->handle!=pDatumJg->handle)
			{
				pDownShoePlate=(*ppShoePlate);
				pDownDatumJg=pAngle;
			}
		}
		if(pUpShoePlate==NULL)
		{
			AfxMessageBox("选中的塔脚板不支持导出为模板文件!");
			return FALSE;
		}
		//塔脚板连接角钢信息
		CFootTempl foot_templ;
		foot_templ.C=0;
		foot_templ.m_hBaseNode=pBaseNode->handle;
		foot_templ.primaryAngle.pAngle=pDatumJg;
		foot_templ.primaryAngle.bolt.d=SelBoltDByWingWide(pDatumJg->GetWidth());
		foot_templ.UpdateLinkAngleZhunJu(foot_templ.primaryAngle);
		foot_templ.UpdateBoltEndSpace(foot_templ.primaryAngle);
		if(pDownDatumJg)
		{
			foot_templ.primaryAngleDown.pAngle=pDownDatumJg;
			foot_templ.primaryAngleDown.bolt.d=SelBoltDByWingWide(pDownDatumJg->GetWidth());
			foot_templ.UpdateLinkAngleZhunJu(foot_templ.primaryAngleDown);
			foot_templ.UpdateBoltEndSpace(foot_templ.primaryAngleDown);
		}
		//连接靴板信息
		if(pUpShoePlate->GetDesignItemValue('A',&nValue))
			foot_templ.upLinkSP.A=(short)nValue;
		if(pUpShoePlate->GetDesignItemValue('B',&nValue))
			foot_templ.upLinkSP.B=(short)nValue;
		if(pUpShoePlate->GetDesignItemValue(KEY2C("IA"),&nValue))
			foot_templ.upLinkSP.C=(short)nValue;
		if(pUpShoePlate->GetDesignItemValue(KEY2C("IB"),&nValue))
			foot_templ.upLinkSP.D=(short)nValue;
		if(pUpShoePlate->GetDesignItemValue(KEY2C("IC"),&nValue))
			foot_templ.upLinkSP.E=(short)nValue;
		if(pUpShoePlate->GetDesignItemValue(KEY2C("ID"),&nValue))
			foot_templ.upLinkSP.F=(short)nValue;
		if(pUpShoePlate->GetDesignItemValue('M',&nValue))
			foot_templ.upLinkSP.M=(short)nValue;
		if(pUpShoePlate->GetDesignItemValue(KEY2C("AL"),&nValue))
			foot_templ.upLinkSP.AL=(short)nValue;
		if(pUpShoePlate->GetDesignItemValue(KEY2C("CM"),&nValue))
			foot_templ.upLinkSP.CM=(short)nValue;
		if(pUpShoePlate->GetDesignItemValue(KEY2C("CS"),&nValue))
			foot_templ.upLinkSP.style_bolt0_weld1=(BYTE)nValue;
		if(foot_templ.upLinkSP.style_bolt0_weld1==1)
		{
			foot_templ.upLinkSP.m_bBreakXueBan0C=TRUE;
			foot_templ.upLinkSP.m_bBreakXueBan90C=TRUE;
		}
		else
		{
			long profile_side=0,work_wing=0;
			pUpShoePlate->GetDesignItemValue('F',&profile_side);//外形类别
			pUpShoePlate->GetDesignItemValue('W',&work_wing);	//靴板所在方向
			if((profile_side==0&&work_wing==0)||(profile_side!=0&&work_wing==1))
				foot_templ.upLinkSP.m_bBreakXueBan0C=FALSE;
			else
				foot_templ.upLinkSP.m_bBreakXueBan0C=TRUE;
			foot_templ.upLinkSP.m_bBreakXueBan90C=!foot_templ.upLinkSP.m_bBreakXueBan0C;
		}
		if(pDownShoePlate)
		{
			if(pDownShoePlate->GetDesignItemValue('A',&nValue))
				foot_templ.downLinkSP.A=(short)nValue;
			if(pDownShoePlate->GetDesignItemValue('B',&nValue))
				foot_templ.downLinkSP.B=(short)nValue;
			if(pDownShoePlate->GetDesignItemValue(KEY2C("IA"),&nValue))
				foot_templ.downLinkSP.C=(short)nValue;
			if(pDownShoePlate->GetDesignItemValue(KEY2C("IB"),&nValue))
				foot_templ.downLinkSP.D=(short)nValue;
			if(pDownShoePlate->GetDesignItemValue(KEY2C("IC"),&nValue))
				foot_templ.downLinkSP.E=(short)nValue;
			if(pDownShoePlate->GetDesignItemValue(KEY2C("ID"),&nValue))
				foot_templ.downLinkSP.F=(short)nValue;
			if(pDownShoePlate->GetDesignItemValue('M',&nValue))
				foot_templ.downLinkSP.M=(short)nValue;
			if(pDownShoePlate->GetDesignItemValue(KEY2C("AL"),&nValue))
				foot_templ.downLinkSP.AL=(short)nValue;
			if(pDownShoePlate->GetDesignItemValue(KEY2C("CM"),&nValue))
				foot_templ.downLinkSP.CM=(short)nValue;
			if(pDownShoePlate->GetDesignItemValue(KEY2C("CS"),&nValue))
				foot_templ.downLinkSP.style_bolt0_weld1=(BYTE)nValue;
			if(foot_templ.downLinkSP.style_bolt0_weld1==1)
			{
				foot_templ.downLinkSP.m_bBreakXueBan0C=TRUE;
				foot_templ.downLinkSP.m_bBreakXueBan90C=TRUE;
			}
			else
			{
				long profile_side=0,work_wing=0;
				pDownShoePlate->GetDesignItemValue('F',&profile_side);	//外形类别
				pDownShoePlate->GetDesignItemValue('W',&work_wing);		//靴板所在方向
				if((profile_side==0&&work_wing==0)||(profile_side!=0&&work_wing==1))
					foot_templ.downLinkSP.m_bBreakXueBan0C=FALSE;
				else
					foot_templ.downLinkSP.m_bBreakXueBan0C=TRUE;
				foot_templ.downLinkSP.m_bBreakXueBan90C=!foot_templ.downLinkSP.m_bBreakXueBan0C;
			}
		}
		if(foot_templ.upLinkSP.style_bolt0_weld1==0)
			InitLineAngleBoltInfo(foot_templ.primaryAngle,pUpShoePlate);
		if(foot_templ.downLinkSP.style_bolt0_weld1==0)
			InitLineAngleBoltInfo(foot_templ.primaryAngleDown,pDownShoePlate);
		//底板设计参数
		foot_templ.base.thick=(short)pBasePlate->GetThick();
		foot_templ.base.cMaterial=pBasePlate->cMaterial;
		foot_templ.base.sPartNo.Copy(pBasePlate->GetPartNo());
		foot_templ.base.cNormOffsetType=2;	//指定偏移量
		if(pBasePlate->GetDesignItemValue('N',&fValue))
			foot_templ.base.nNormOffset=(short)fValue;
		if(foot_templ.base.nNormOffset==0)
			foot_templ.base.cNormOffsetType=1; //下平面
		foot_templ.m_bWeldDiZuo=IsWeldDiZuo(foot_templ);	//是否焊接底座
		if(pBasePlate->GetDesignItemValue('S',&nValue))
			foot_templ.base.shape=(BYTE)nValue;
		if(foot_templ.base.shape==0)
		{
			pBasePlate->GetDesignItemValue(KEY2C("XA"),&foot_templ.base.XA);
			pBasePlate->GetDesignItemValue(KEY2C("XB"),&foot_templ.base.XB);
			pBasePlate->GetDesignItemValue(KEY2C("YA"),&foot_templ.base.YA);
			pBasePlate->GetDesignItemValue(KEY2C("YB"),&foot_templ.base.YB);
			for(int i=0;i<4;i++)
			{
				pBasePlate->GetDesignItemValue(KEY2C(CXhChar16("X%d",i)),&foot_templ.base.XC[i]);
				pBasePlate->GetDesignItemValue(KEY2C(CXhChar16("Y%d",i)),&foot_templ.base.YC[i]);
			}
		}
		else
			pBasePlate->GetDesignItemValue(KEY2C("R"),&foot_templ.base.fR);	//半径
		//底板孔
		for(CLsRef* pLsRef=pBasePlate->GetFirstLsRef();pLsRef;pLsRef=pBasePlate->GetNextLsRef())
		{
			CLDSBolt* pBolt=pLsRef->GetLsPtr();
			if(pBolt->m_cFuncType!=CLDSBolt::FUNC_SETUPHOLE)
				continue;
			CFootTempl::BASEPLATE::HOLE  ls_hole;
			ls_hole.d=pBolt->get_d();
			ls_hole.hole_d=short(pBolt->get_d()+pBolt->hole_d_increment);
			ls_hole.x=short(pBolt->des_base_pos.datumPoint.datum_pos.x);
			ls_hole.y=short(pBolt->des_base_pos.datumPoint.datum_pos.y);
			foot_templ.base.listHoles.AttachNode(ls_hole);
		}
		//靴板设计参数
		foot_templ.RetrieveShoePlates();
		for(CLDSParamPlate** ppShoePlate=shoe_plate_list.GetFirst();ppShoePlate;ppShoePlate=shoe_plate_list.GetNext())
		{
			char label[4]="UXB";
			(*ppShoePlate)->GetDesignItemValue('D',&handle);
			if(handle!=pDatumJg->handle)
				label[0]='D';
			long profile_side=0,work_wing=0;
			(*ppShoePlate)->GetDesignItemValue('W',&work_wing);
			if(work_wing==1)
				label[1]='Y';
			(*ppShoePlate)->GetDesignItemValue('F',&profile_side);
			if(profile_side==2)
				label[2]='F';
			//
			CFootTempl::SHOEPLATE* pShoePlate=foot_templ.listShoePlates.GetValue(KEY4C(label));
			if(pShoePlate)
			{
				pShoePlate->thick=(short)(*ppShoePlate)->GetThick();
				pShoePlate->cMaterial=(*ppShoePlate)->cMaterial;
				pShoePlate->sPartNo.Copy((*ppShoePlate)->GetPartNo());
				if((*ppShoePlate)->GetDesignItemValue('L',&nValue))
					pShoePlate->fCutAngleWidth=nValue;
				if((*ppShoePlate)->GetDesignItemValue(KEY2C("NL"),&nValue))
					pShoePlate->norm_offset=(short)nValue;
				foot_templ.RetrieveLinkAngles(pShoePlate);
			}
		}
		//肋板设计参数
		for(CLDSParamPlate** ppRibPlate=rib_plate_list.GetFirst();ppRibPlate;ppRibPlate=rib_plate_list.GetNext())
		{
			CFootTempl::RIBPLATE* pRibPlate=foot_templ.listRibPlates.Add(0);
			pRibPlate->thick=(*ppRibPlate)->thick;
			pRibPlate->cMaterial=(*ppRibPlate)->cMaterial;
			pRibPlate->sPartNo.Copy((*ppRibPlate)->GetPartNo());
			pRibPlate->upper=true;
			if((*ppRibPlate)->GetDesignItemValue('A',&handle)&&handle<0)
				pRibPlate->upper=false;
			if((*ppRibPlate)->GetDesignItemValue('E',&nValue))
				pRibPlate->A=nValue;
			if((*ppRibPlate)->GetDesignItemValue('F',&nValue))
				pRibPlate->C=nValue;
			if((*ppRibPlate)->GetDesignItemValue('G',&nValue))
				pRibPlate->B=nValue;
			if((*ppRibPlate)->GetDesignItemValue('H',&nValue))
				pRibPlate->D=nValue;
			if((*ppRibPlate)->GetDesignItemValue('e',&nValue))
				pRibPlate->iCutMode=nValue;
			if((*ppRibPlate)->GetDesignItemValue('I',&nValue))
				pRibPlate->fCutAngleWidth=nValue;
			if((*ppRibPlate)->GetDesignItemValue(KEY2C("MF"),&nValue))
				pRibPlate->MF=nValue;
			if((*ppRibPlate)->GetDesignItemValue(KEY2C("MG"),&nValue))
				pRibPlate->MG=nValue;
			if((*ppRibPlate)->GetDesignItemValue(KEY2C("MH"),&nValue))
				pRibPlate->MH=nValue;
			if((*ppRibPlate)->GetDesignItem(KEY2C("V")))
			{
				pRibPlate->axisXType=5;
				(*ppRibPlate)->GetDesignItemValue(KEY2C("Ra"),&pRibPlate->pos_para.PARA3.angle);
				(*ppRibPlate)->GetDesignItemValue(KEY2C("Rd"),&pRibPlate->pos_para.PARA3.fRayDist);
			}
			else if((*ppRibPlate)->GetDesignItem(KEY2C("Ra")))
			{
				pRibPlate->axisXType=0;
				(*ppRibPlate)->GetDesignItemValue(KEY2C("Ra"),&pRibPlate->pos_para.PARA2.angle);
			}
			else if((*ppRibPlate)->GetDesignItem(KEY2C("Ox")))
			{
				(*ppRibPlate)->GetDesignItemValue(KEY2C("Ox"),&pRibPlate->pos_para.PARA1.OX);
				(*ppRibPlate)->GetDesignItemValue(KEY2C("Oy"),&pRibPlate->pos_para.PARA1.OY);
				(*ppRibPlate)->GetDesignItemValue(KEY2C("Xx"),&pRibPlate->pos_para.PARA1.axisX_x);
				(*ppRibPlate)->GetDesignItemValue(KEY2C("Xy"),&pRibPlate->pos_para.PARA1.axisX_y);
				(*ppRibPlate)->GetDesignItemValue(KEY2C("Ga"),&pRibPlate->fGradientAngle);
				//
				if(pRibPlate->pos_para.PARA1.axisX_x==1&&pRibPlate->pos_para.PARA1.axisX_y==0)
					pRibPlate->axisXType=1;
				else if(pRibPlate->pos_para.PARA1.axisX_x==-1&&pRibPlate->pos_para.PARA1.axisX_y==0)
					pRibPlate->axisXType=2;
				else if(pRibPlate->pos_para.PARA1.axisX_x==0&&pRibPlate->pos_para.PARA1.axisX_y==1)
					pRibPlate->axisXType=3;
				else if(pRibPlate->pos_para.PARA1.axisX_x==0&&pRibPlate->pos_para.PARA1.axisX_y==-1)
					pRibPlate->axisXType=4;
			}
		}
		//生成模板文件
		CFileDialog file_dlg(FALSE,"ft","foottempl.ft",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"塔脚设计模板文件(*.ft)|*.ft|所有文件(*.*)|*.*||");
		if(file_dlg.DoModal()==IDOK)
			foot_templ.ToXmlTemplFile(file_dlg.GetPathName());
		//
		g_pSolidDraw->ReleaseSnapStatus();	
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Command:","");
#else 
		pCmdLine->FillCmdLine("命令:","");
#endif
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	return 1;
}
#endif