//<LOCALE_TRANSLATE Confirm by hxr/>
#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "MainFrm.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "PromptDlg.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "GUID.h"
#include "dlg.h"
#include "BoltPropDlg.h"
#include "Query.h"
#include "CmdLineDlg.h"

BOOL CLDSView::HidePart()
{
	long n,*id_arr=NULL;
	CLDSDbObject *pObj;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	pCmdLine->FinishCmdLine();
	n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(g_sysPara.display.bPartSketch) 
	{	//删除ID为0的实体 wht 10-09-15
		BOOL bRet=TRUE;
		while(bRet)
			bRet=g_pSolidDraw->DelEnt(0);
	}
	for(int i=0;i<n;i++)
	{
		pObj = (CLDSDbObject*)console.FromHandle(id_arr[i],TRUE);
		if(pObj)
			pObj->is_visible = FALSE;
		g_pSolidDraw->DelEnt(id_arr[i]);
	}
	if(n<=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("No selected parts need to be hidden","");
		pCmdLine->FinishCmdLine();
	}
	pCmdLine->FillCmdLine("command:","");
#else
		pCmdLine->FillCmdLine("没有选中需要被隐去的构件","");
		pCmdLine->FinishCmdLine();
	}
	pCmdLine->FillCmdLine("命令:","");
#endif
	return TRUE;
}

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
BOOL CLDSView::ReplaceBoltDatumPoint()
{
	CString cmdStr;
	f3dPoint inter_pos;
	CLDSBolt *pBolt=NULL,*pDatumBolt=NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	PARTSET boltSet;
	long *id_arr=NULL;
	long nn=g_pSolidSnap->GetLastSelectEnts(id_arr);
	long hBasePart=0;
	BYTE offset_wing;
	if(nn==0)
		return FALSE;
	for(int i=0;i<nn;i++)
	{
		CLDSBolt *pBolt=(CLDSBolt*)console.FromPartHandle(id_arr[i]);
		if(pBolt)
			boltSet.append(pBolt);
		if(hBasePart==0)
		{
			hBasePart=pBolt->des_base_pos.hPart;
			offset_wing=pBolt->des_base_pos.offset_wing;
		}
		else if(hBasePart!=pBolt->des_base_pos.hPart||offset_wing!=pBolt->des_base_pos.offset_wing)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("the position datum rod or offset leg of current selected bolt is different,replace group bolt datum point cannot be done");
			pCmdLine->FillCmdLine("命令:","");
#else
			AfxMessageBox("当前选中螺栓的位置基准杆件或偏移肢不同，不能进行成组螺栓基点替换！");
			pCmdLine->FillCmdLine("命令:","");
#endif
			return FALSE;
		}
	}
	g_pSolidDraw->ReleaseSnapStatus();
	//选择基准角钢
	GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECTINDEX_BOLT),CLS_BOLT);
	scr_part_para.disp_type=DISP_SOLID;
	while(1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		scr_part_para.cmdStr="ReplaceBoltDP please select the target datum point reference bolt on the same datum part:";
		scr_part_para.cmdErrorFeedbackStr="No proper part is selected, please select the target datum point reference bolt on the same datum part again:";
#else
		scr_part_para.cmdStr="ReplaceBoltDP 请选择同一基准构件上的目标基点参照螺栓:";
		scr_part_para.cmdErrorFeedbackStr="没有选中合适的构件，请重新选择同一基准构件上的目标基点参照螺栓:";
#endif
		if(!GetPartsFromScr(scr_part_para))
			return FALSE; 
		CLDSDbObject *pObj=scr_part_para.pResultObjsArr[0];
		if(pObj&&pObj->GetClassTypeId()==CLS_BOLT)
			pDatumBolt=(CLDSBolt*)pObj;
		else
			return FALSE;
		//if(hBasePart==pDatumBolt->des_base_pos.hPart&&offset_wing==pDatumBolt->des_base_pos.offset_wing)
			break;
		//else
		//	AfxMessageBox("当前选中基点参照螺栓的位置基准杆件或偏移肢不同与之前待选择螺栓不同，请重新选择基点参照螺栓！");
	}
	Ta.BeginUndoListen();
	g_pSolidDraw->ReleaseSnapStatus();
	CSuperSmartPtr<CLDSPart> pDatumPart=console.FromPartHandle(hBasePart);
	f3dPoint len_vec,wing_vec;
	if(pDatumPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		len_vec=pDatumPart.LineAnglePointer()->End()-pDatumPart.LineAnglePointer()->Start();
		normalize(len_vec);
		if(offset_wing==0)
			wing_vec=pDatumPart.LineAnglePointer()->GetWingVecX();
		else
			wing_vec=pDatumPart.LineAnglePointer()->GetWingVecY();
	}
	else
	{
		len_vec=pDatumPart->ucs.axis_x;
		wing_vec=pDatumPart->ucs.axis_y;
	}
	for(pBolt=(CLDSBolt*)boltSet.GetFirst();pBolt;pBolt=(CLDSBolt*)boltSet.GetNext())
	{
		//存在基点坐标未更新情况 wjh-2018.1.12
		pBolt->des_base_pos.datumPoint.UpdatePos(pBolt->BelongModel());
		pDatumBolt->des_base_pos.datumPoint.UpdatePos(pBolt->BelongModel());
		f3dPoint oldPos=pBolt->des_base_pos.datumPoint.Position();
		pBolt->des_base_pos.datumPoint=pDatumBolt->des_base_pos.datumPoint;
		f3dPoint newPos=pBolt->des_base_pos.datumPoint.Position();
		project_point(oldPos,newPos,pBolt->ucs.axis_z);
		f3dPoint offset_vec=oldPos-newPos;
		double wing_offset_dist_incre=offset_vec*wing_vec;	//肢向偏移量增量
		double len_offset_dist_incre=offset_vec*len_vec;	//纵向偏移量增量
		if(pBolt->des_base_pos.direction==1)
			len_offset_dist_incre*=-1;
		pBolt->des_base_pos.datumPoint=pDatumBolt->des_base_pos.datumPoint;
		pBolt->des_base_pos.len_offset_dist+=len_offset_dist_incre;
		pBolt->des_base_pos.wing_offset_dist+=wing_offset_dist_incre;
	}
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
	return TRUE;
}
BOOL CLDSView::AlignBoltsAlongLenVec()
{
	CString cmdStr;
	f3dPoint inter_pos;
	CLDSBolt *pBolt=NULL,*pDatumBolt=NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	long *id_arr=NULL;
	long nn=g_pSolidSnap->GetLastSelectEnts(id_arr);
	long hBasePart=0;
	BYTE offset_wing;
	if(nn!=1)
		return FALSE;
	if((pBolt=(CLDSBolt*)console.FromPartHandle(id_arr[0]))==NULL)
		return FALSE;
	hBasePart=pBolt->des_base_pos.hPart;
	offset_wing=pBolt->des_base_pos.offset_wing;

	g_pSolidDraw->ReleaseSnapStatus();
	//选择基准螺栓
	GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECTINDEX_BOLT),CLS_BOLT);
	scr_part_para.disp_type=DISP_SOLID;
	while(1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		scr_part_para.cmdStr="AlignBolts please select the reference align bolt in the same connect plane:";
		scr_part_para.cmdErrorFeedbackStr="No proper part is selected, please select the reference align bolt in the same connect plane again:";
#else
		scr_part_para.cmdStr="AlignBolts 请选择同一连接平面上的纵向对齐参照螺栓:";
		scr_part_para.cmdErrorFeedbackStr="没有选中合适的构件，请重新选择同一连接平面上的纵向对齐参照螺栓:";
#endif
		if(!GetPartsFromScr(scr_part_para))
			return FALSE; 
		CLDSDbObject *pObj=scr_part_para.pResultObjsArr[0];
		if(pObj&&pObj->GetClassTypeId()==CLS_BOLT)
			pDatumBolt=(CLDSBolt*)pObj;
		else
			return FALSE;
		//if(hBasePart==pDatumBolt->des_base_pos.hPart&&offset_wing==pDatumBolt->des_base_pos.offset_wing)
		if(fabs(pDatumBolt->ucs.axis_z*pBolt->ucs.axis_z)>EPS_COS2)
			break;
		//else
		//	AfxMessageBox("当前选中基点参照螺栓的位置基准杆件或偏移肢不同与之前待选择螺栓不同，请重新选择基点参照螺栓！");
	}
	Ta.BeginUndoListen();
	g_pSolidDraw->ReleaseSnapStatus();
	CSuperSmartPtr<CLDSPart> pDatumPart=console.FromPartHandle(hBasePart);
	f3dPoint len_vec,wing_vec;
	if(pDatumPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		len_vec=pDatumPart.LineAnglePointer()->End()-pDatumPart.LineAnglePointer()->Start();
		normalize(len_vec);
		if(offset_wing==0)
			wing_vec=pDatumPart.LineAnglePointer()->GetWingVecX();
		else
			wing_vec=pDatumPart.LineAnglePointer()->GetWingVecY();
	}
	else
	{
		len_vec=pDatumPart->ucs.axis_x;
		wing_vec=pDatumPart->ucs.axis_y;
	}
	GECS cs=pBolt->ucs;
	GEPOINT lenVecOffset=pDatumBolt->ucs.origin-pBolt->ucs.origin;
	double len_offset=len_vec*lenVecOffset;
	if(pBolt->des_base_pos.direction==0)
		pBolt->des_base_pos.len_offset_dist+=ftoi(len_offset*10)*0.1;
	else
		pBolt->des_base_pos.len_offset_dist-=ftoi(len_offset*10)*0.1;
	pBolt->correct_pos();
	pBolt->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	g_pSolidDraw->Draw();
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
	return TRUE;
}

BOOL CLDSView::DisplayPartWorkUcs()
{
	CLDSDbObject *pDbObj=NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//选择需要显示工作坐标系的构件
	long *id_arr;
	GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECT_ALL));
	scr_part_para.disp_type=DISP_SOLID;
	if(g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
		pDbObj=(CLDSDbObject*)console.FromHandle(id_arr[0],TRUE);
	if(pDbObj==NULL||(!pDbObj->IsPart()&&pDbObj->GetClassTypeId()!=CLS_BLOCKREF))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		scr_part_para.cmdStr="DisplayUcs please specify part need to show work coordinate:";
		scr_part_para.cmdErrorFeedbackStr="select wrong part or select more than one part,please select again:";
#else
		scr_part_para.cmdStr="DisplayUcs 请指定要显示工作坐标系的构件:";
		scr_part_para.cmdErrorFeedbackStr="选择了错误的构件或选择了多个构件，请重新选择:";
#endif
		if(!GetPartsFromScr(scr_part_para))
			return FALSE; 
		pDbObj=scr_part_para.pResultObjsArr[0];
		if(pDbObj==NULL||(!pDbObj->IsPart()&&pDbObj->GetClassTypeId()!=CLS_BLOCKREF))
			return FALSE;
	}
	UCS_STRU part_ucs;
	if(pDbObj->IsPart())
		part_ucs=((CLDSPart*)pDbObj)->ucs;
	else if(pDbObj->GetClassTypeId()==CLS_BLOCKREF)
		part_ucs=((CBlockReference*)pDbObj)->GetACS();
	else
		return FALSE;
	UCS_STRU object_ucs,ucs;
	ucs = part_ucs;
	g_pSolidSet->GetObjectUcs(object_ucs);
	coord_trans(ucs.origin,object_ucs,TRUE);
	vector_trans(ucs.axis_x,object_ucs,TRUE);
	vector_trans(ucs.axis_y,object_ucs,TRUE);
	vector_trans(ucs.axis_z,object_ucs,TRUE);
	g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,ucs);
	//g_pSolidSet->SetDispWorkUcs();
	g_pSolidSet->SetZoomStyle(ROT_CENTER);
	g_pSolidSet->SetRotOrg(part_ucs.origin);
	g_sysPara.m_bDispWorkUcs = TRUE;	//更新系统设置对话框
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
	return TRUE;
}

BOOL DelSpecClsTypePart(SCOPE_STRU *pScope/*=NULL*/)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("DelSpecClsPart input part type need to be deleted[Para Plate(1)/Ordinary Plate(2)/Bolt(3)/Tube(4)/Angle(5)]<1>:","");
#else
	pCmdLine->FillCmdLine("DelSpecClsPart 输入删除构件的类型[参数板(1)/普通钢板(2)/螺栓(3)/钢管(4)/角钢(5)]<1>:","");
#endif
	int cls_id=0,type_id=0;
	CString cmdStr;
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
			return FALSE;
		if(cmdStr.GetLength()==0||cmdStr.CompareNoCase("1")==0)
		{
			cls_id=CLS_PARAMPLATE;
			break;
		}
		else if(cmdStr.CompareNoCase("2")==0)
		{
			cls_id=CLS_PLATE;
			break;
		}
		else if(cmdStr.CompareNoCase("3")==0)
		{
			cls_id=CLS_BOLT;
			break;
		}
		else if(cmdStr.CompareNoCase("4")==0)
		{
			cls_id=CLS_LINETUBE; 
			break;
		}
		else if(cmdStr.CompareNoCase("5")==0)
		{
			cls_id=CLS_LINEANGLE;
			break;
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("need option keywords","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("DelSpecClsPart input part type need to be deleted[parametric plate(1)/Ordinary Plate(2)/Bolt(3)/Tube(4)/Angle(5)]<1>:","");
#else
			pCmdLine->FillCmdLine("需要选项关键字","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("DelSpecClsPart 输入删除构件的类型[参数板(1)/普通钢板(2)/螺栓(3)/钢管(4)/角钢(5)]<1>:","");
#endif
			continue;
		}
	}
	if(cls_id==CLS_PARAMPLATE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("input parametric plate type need to be deleted[all(0)/vertical reinforce plate(1)/elbow plate(2)/square end sealing plate(3)/ring sealing plate(4)/circular(5)/U-shape plate(6)/rolled edge plate(7)/flange(8)]<0>:","");
#else
		pCmdLine->FillCmdLine("输入删除参数板的类型[所有(0)/垂向加劲板(1)/肘板(2)/方形封头板(3)/环向封板(4)/环向肋板(5)/U型板(6)/卷边板(7)/法兰(8)]<0>:","");
#endif
		while(1)
		{
			if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				return FALSE;
			if(cmdStr.GetLength()==0||cmdStr.CompareNoCase("0")==0)
			{
				type_id=0;
				break;
			}
			else if(cmdStr.CompareNoCase("1")==0)
			{
				type_id=TYPE_VERTPLATE;
				break;
			}
			else if(cmdStr.CompareNoCase("2")==0)
			{
				type_id=TYPE_ELBOWPLATE;
				break;
			}
			else if(cmdStr.CompareNoCase("3")==0)
			{
				type_id=TYPE_ENDPLATE;
				break;
			}
			else if(cmdStr.CompareNoCase("4")==0)
			{
				type_id=TYPE_CIRCOVERPLATE; 
				break;
			}
			else if(cmdStr.CompareNoCase("5")==0)
			{
				type_id=TYPE_CIRRIBPLATE;
				break;
			}
			else if(cmdStr.CompareNoCase("6")==0)
			{
				type_id=TYPE_UEND;
				break;
			}
			else if(cmdStr.CompareNoCase("7")==0)
			{
				type_id=TYPE_ROLLEND; 
				break;
			}
			else if(cmdStr.CompareNoCase("8")==0)
			{
				type_id=TYPE_FL;
				break;
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
pCmdLine->FillCmdLine("need option keywords","");
				pCmdLine->FinishCmdLine();
				pCmdLine->FillCmdLine("input parametric plate type need to be deleted[all(0)/vertical reinforce plate(1)/elbow plate(2)/square end sealing plate(3)/ring sealing plate(4)/circular(5)/U-shape plate(6)/rolled edge plate(7)/flange(8)]<1>:","");
#else
				pCmdLine->FillCmdLine("需要选项关键字","");
				pCmdLine->FinishCmdLine();
				pCmdLine->FillCmdLine("输入删除参数板的类型[垂向加劲板(1)/肘板(2)/方形封头板(3)/环向封板(4)/环向肋板(5)/U型板(6)/卷边板(7)/法兰(8)]<1>:","");
#endif
				continue;
			}
		}
	}
	CLDSPart *pPart=NULL;
	for(pPart=Ta.Parts.GetFirst(cls_id);pPart;pPart=Ta.Parts.GetNext(cls_id))
	{
		if(!pPart)
			continue;
		if(CLS_PARAMPLATE==cls_id&&pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			if(((CLDSParamPlate*)pPart)->m_iParamType==type_id||type_id==0)
			{
				if(pScope && !pScope->IsIncludePoint(pPart->ucs.origin))
					continue;	//基点不在指定空间区域内的板
				console.DispPartSet.DeleteNode(pPart->handle);
				console.DeletePart(pPart->handle);
			}
			else
				continue;
		}
		else
		{
			if(pPart->IsLinePart())
			{
				if(pScope && !pScope->IsIntersWithLine(((CLDSLinePart*)pPart)->Start(),((CLDSLinePart*)pPart)->End()))
					continue;	//两端点不在指定空间区域内的杆件
			}
			else
			{
				if(pScope && !pScope->IsIncludePoint(pPart->ucs.origin))
					continue;	//基点不在指定空间区域内的板
			}
			console.DispPartSet.DeleteNode(pPart->handle);
			if(cls_id==CLS_BOLT)
				Ta.DelBoltAndRelativeBoltRef(pPart->handle);
			else
				console.DeletePart(pPart->handle);
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Operation has been completed!","");
	pCmdLine->FinishCmdLine();
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("操作已完成!","");
	pCmdLine->FinishCmdLine();
	pCmdLine->FillCmdLine("命令:","");
#endif
	return TRUE;
}
BOOL CLDSView::DelSpecClsPart()
{
	return DelSpecClsTypePart(NULL);
}
#endif

//from outputinfo.cpp
SCOPE_STRU GetPartScope(CLDSPart* pPart,SCOPE_STRU *pScope=NULL);
#include "SearchPartDlg.h"
BOOL CLDSView::FindPart()
{
	g_pSearchPartDlg->InitSearchPartDlg();
	return TRUE;
}
static BOOL FindPartByPartNo(const char* sPartNo,bool bPopErrMsg=true,DBOBJSET* pFindedObjSet=NULL)
{
	ATOM_LIST<long> handleList;
	CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->m_bVirtualPart)
			continue;
		if(stricmp(pPart->GetPartNo(),sPartNo)!=0)
			continue;
		if(pFindedObjSet)
			pFindedObjSet->append(pPart);
		handleList.append(pPart->handle);
	}
	if(handleList.GetNodeNum()<=0)
	{
		if(bPopErrMsg)
		{
			if(g_pSearchPartDlg->GetSafeHwnd()!=0)
#ifdef AFX_TARG_ENU_ENGLISH
				g_pSearchPartDlg->MessageBox("Fail to find the part of specified part number!");
			else
				AfxMessageBox("Fail to find the rod of specified part number!");
#else
				g_pSearchPartDlg->MessageBox("未找到指定件号的构件!");
			else
				AfxMessageBox("未找到指定件号的构件!");
#endif
		}
		return FALSE;
	}
	for(long* pHandle=handleList.GetFirst();pHandle;pHandle=handleList.GetNext())
	{
		g_pSolidDraw->SetEntSnapStatus(*pHandle);
		if(handleList.GetCurrentIndex()==0)
		{
			CLDSPart *pPart=console.FromPartHandle(*pHandle);
			SCOPE_STRU scope=GetPartScope(pPart);
			g_pSolidOper->FocusTo(scope,g_sysPara.m_fMaxScaleScr2User);
			f3dPoint center;
			g_pSolidSet->SetOperType(OPER_ROTATE);
			scope.centre(&center.x,&center.y,&center.z);
			g_pSolidSet->SetRotOrg(center);
			g_pSolidSet->SetZoomStyle(ROT_CENTER);
		}
	}
	return TRUE;
}
BOOL CLDSView::QuickFindPart(const char* sSearchText,DBOBJSET* pFindedObjSet/*=NULL*/)
{	//智能综合查找
	if(strlen(sSearchText)<=0)
	{
		if(pFindedObjSet!=NULL)
			return FALSE;	//作为CSearchPartDlg类的子函数调用不需要提示错误信息 wjh-2016.6.26
		if(g_pSearchPartDlg->GetSafeHwnd()!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			g_pSearchPartDlg->MessageBox("the inputed search content is empty!");
		else
			MessageBox("the inputed search content is empty!");
#else
			g_pSearchPartDlg->MessageBox("输入的查找内容为空!");
		else
			MessageBox("输入的查找内容为空!");
#endif
		return FALSE;
	}
	if(sSearchText[0]=='\'')	//字符串以单引号开始，则通过件号查找
	{
		CXhChar16 sPartNo,sText(sSearchText);
		sPartNo=strtok(sText,"' ");
		return FindPartByPartNo(sPartNo,pFindedObjSet==NULL,pFindedObjSet); 
	}
	BOOL bReadPointI=false;
#ifdef __TSA_
	bReadPointI=true;
#elif defined(__LDS_)
	if(theApp.m_bAnalysisWorkEnv)
		bReadPointI=true;
#endif
	long h=0,pointI=0;
	sscanf(sSearchText,"%X",&h);
	
	if(sSearchText[0]=='0'&&toupper(sSearchText[1])=='X'||!bReadPointI)
		bReadPointI=false;	//读的是构件句柄
	if(bReadPointI)
	{
		bool hasNonDigitLetter=false;	//含有非数字字母
		CXhChar50 text(sSearchText);
		char* iter=(char*)text;
		while(iter!=0)
		{
			if(*iter==0)
				break;
			else if(*iter<'0'||*iter>'9')
			{
				hasNonDigitLetter=true;
				break;
			}
			iter++;
		}
		if(!hasNonDigitLetter)
			sscanf(sSearchText,"%d",&pointI);
	}

	CLDSObject *pObj = NULL;
#if defined(__TSA_)||defined(__LDS_)
	if(bReadPointI&&pointI>0)
	{
		for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			if(pNode->point_i==pointI)
				break;
		}
		pObj=pNode;
		if(pNode)
			h=pNode->handle;
		else
			h=HexStrToLong(sSearchText);
	}
#endif
	if(pObj==NULL)
		pObj=(CLDSObject*)Ta.FromHandle(h,TRUE);
	if(pObj==NULL && FindPartByPartNo(sSearchText,false,pFindedObjSet))
		return TRUE; 
	if(pObj==NULL)
	{
		if(pFindedObjSet!=NULL)
			return FALSE;	//作为CSearchPartDlg类的子函数调用不需要提示错误信息 wjh-2016.6.26
		if(g_pSearchPartDlg->GetSafeHwnd()!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			g_pSearchPartDlg->MessageBox("Fail to find the object of specified handle!");
		else
			MessageBox("Fail to find the object of specified handle!");
#else
			g_pSearchPartDlg->MessageBox("未发现你所指定句柄的对象!");
		else
			MessageBox("未发现你所指定句柄的对象!");
#endif
		return FALSE;
	}
	if(!pObj->IsDbObject())
	{
		if(pFindedObjSet!=NULL)
			return FALSE;	//作为CSearchPartDlg类的子函数调用不需要提示错误信息 wjh-2016.6.26
		if(g_pSearchPartDlg->GetSafeHwnd()!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			g_pSearchPartDlg->MessageBox("geometric drawing elements object has been selected,showing object properties does not be supported at present!");
		else
			MessageBox("Geometric drawing elements object has been selected,object properties display does not be supported at present!");
#else
			g_pSearchPartDlg->MessageBox("选中了几何图元对象，暂不支持对象属性显示!");
		else
			MessageBox("选中了几何图元对象，暂不支持对象属性显示!");
#endif
		return FALSE;
	}
	g_pSolidDraw->ReleaseSnapStatus();
	if(pFindedObjSet)
		pFindedObjSet->append((CLDSDbObject*)pObj);
	UpdatePropertyPage(pObj);
	if(g_pSolidDraw->SetEntSnapStatus(h)&&m_curAppType==TA_ASSEMBLE_ENV)
	{
		if(((CLDSDbObject*)pObj)->IsPart())
		{	//找到构件后将旋转中心设置到构件所在位置方便找到构件  wht 09-12-03
			CLDSPart *pPart=(CLDSPart*)pObj;
			SCOPE_STRU scope=GetPartScope(pPart);
			g_pSolidOper->FocusTo(scope,g_sysPara.m_fMaxScaleScr2User);
			f3dPoint center;
			g_pSolidSet->SetOperType(OPER_ROTATE);	//设置操作类型为旋转
			scope.centre(&center.x,&center.y,&center.z);
			g_pSolidSet->SetRotOrg(center);
			g_pSolidSet->SetZoomStyle(ROT_CENTER);	//设置为指定旋转中心类型
			Invalidate(FALSE);
		}
		else if(pObj->GetClassTypeId()==CLS_NODE)
		{
			g_pSolidSet->SetOperType(OPER_ROTATE);	//设置操作类型为旋转
			f3dPoint org=((CLDSNode*)pObj)->ActivePosition();
			SCOPE_STRU scope;
			scope.VerifyVertex(org);
			scope.fMinX-=500;
			scope.fMaxX+=500;
			scope.fMinY-=500;
			scope.fMaxY+=500;
			scope.fMinZ-=500;
			scope.fMaxZ+=500;
			g_pSolidOper->FocusTo(scope,g_sysPara.m_fMaxScaleScr2User);
			g_pSolidSet->SetRotOrg(org);
			g_pSolidSet->SetZoomStyle(ROT_CENTER);	//设置为指定旋转中心类型
			Invalidate(FALSE);
		}
	}
	else if(m_curAppType==TA_ASSEMBLE_ENV&&pFindedObjSet==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("The object of specified handle is hidden currently!");
#else
		MessageBox("指定句柄的对象当前处于非显示状态！");
#endif
	return TRUE;
}
BOOL CLDSView::DeletePart()
{
	Ta.BeginUndoListen();
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("the handle of specified object:","");
#else
	pCmdLine->FillCmdLine("指定对象句柄:","");
#endif
	while(1)
	{
		CString cmdStr;
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
			return FALSE;
		long h=0;
		sscanf(cmdStr.GetBuffer(10),"%X",&h);
		CLDSObject *pObj = console.FromHandle(h);
		if(pObj==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Fail to find the object of specified handle","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("the handle of specified object:","");
#else
			pCmdLine->FillCmdLine("未发现你所指定句柄的对象","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("指定对象句柄:","");
#endif
			continue;
		}
		else if(pObj->IsDbObject())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("command:","");
#else
			pCmdLine->FillCmdLine("命令:","");
#endif
			DelSpecPart(h);
		}
		break;
	}
	Ta.EndUndoListen();
	return TRUE;
}

void CLDSView::OnDelSpecifyPart() 
{	//删除指定构件
	m_nPrevCommandID=ID_DEL_SPECIFY_PART;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat to delete specified part";
#else
	m_sPrevCommandName="重复删除指定构件";
#endif
	DeletePart();
}
