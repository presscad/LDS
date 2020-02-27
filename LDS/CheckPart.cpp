// LDSView.cpp : implementation of the CLDSView class
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "GUID.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "MainFrm.h"
#include "Tower.h"
#include "PromptDlg.h"
#include "CalInt3dllDlg.h"
#include "UserDefMsg.h"
#include "database.h"
#include "dlg.h"
#include "LmaDlg.h"
#include "InputAnValDlg.h"
#include "LsCheckParaDlg.h"
#include "direct.h"
#include "ScrTools.h"
#include "PartLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __TSA_
void CLDSView::OnCheckBoltValid(){;}
void CLDSView::OnCheckJgLsValid(){;}
void CLDSView::OnBoltValidCheck(){;}
void CLDSView::OnDisplayOrHideAllHole(){;}
void CLDSView::OnCheckPlankLsValid(){;}
#else
CLsCheckParaDlg dlg;
void CLDSView::OnCheckBoltValid() 
{
	m_nPrevCommandID=(ID_CHECK_BOLT_VALID);
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat testing bolt's legality";
#else 
	m_sPrevCommandName="重复螺栓合法性检测";
#endif
	FILE *fp;
	fp = fopen("LsCheck.txt","wt");
	char sError[201];
	int nInvalidHits = 0;
	if(dlg.DoModal()!=IDOK)
		return;
	//分解段号字符串
	CHashList<SEGI>segTbl;
	GetSegNoHashTblBySegStr(dlg.m_sSegStr,segTbl);
	//角钢螺栓合法性检测
	for(CLDSLineAngle *pJg=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pJg;
		pJg=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
	{
		if(pJg->m_bVirtualPart)
			continue;
		else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pJg->dwPermission))
			continue;	//网络协同时，对此角钢没有拥有修改权限
		else if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pJg->iSeg.iSeg)==NULL)
			continue;	//判断是否在指定查找段号范围内
		if(pJg->ValidLsList(sError,dlg.m_bRollingSpace,FALSE,FALSE,dlg.m_fScope)!=0)
		{
			g_pSolidDraw->SetEntSnapStatus(pJg->handle);
			fprintf(fp,"%4d#  ",nInvalidHits+1);
			fprintf(fp,sError);
			fprintf(fp,";\n");
			nInvalidHits++;
		}
		if(pJg->ValidLsList(sError,FALSE,dlg.m_bCutAngleDist,FALSE,dlg.m_fScope)!=0)
		{
			g_pSolidDraw->SetEntSnapStatus(pJg->handle);
			fprintf(fp,"%4d#  ",nInvalidHits+1);
			fprintf(fp,sError);
			fprintf(fp,";\n");
			nInvalidHits++;
		}
		if(pJg->ValidLsList(sError,FALSE,FALSE,dlg.m_bLsEndSpace,dlg.m_fScope)!=0)
		{
			g_pSolidDraw->SetEntSnapStatus(pJg->handle);
			fprintf(fp,"%4d#  ",nInvalidHits+1);
			fprintf(fp,sError);
			fprintf(fp,";\n");
			nInvalidHits++;
		}
		if(!pJg->ValidLsPushFlat(sError,dlg.m_fScope))
		{
			g_pSolidDraw->SetEntSnapStatus(pJg->handle);
			fprintf(fp,"%4d#  ",nInvalidHits+1);
			fprintf(fp,sError);
			fprintf(fp,";\n");
			nInvalidHits++;
		}
	}
	//连板螺栓边距检测	
	CProcBarDlg *pProcDlg = NULL;
	pProcDlg = new CProcBarDlg(AfxGetApp()->m_pMainWnd);
	pProcDlg->Create();
	int iOrder=1;
	int nHits=0;
	int nPlates = Ta.Parts.GetNodeNum(CLS_PLATE);
	if(dlg.m_bLsPlateEdgeSpace)
	{
		for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(pPart->GetClassTypeId()!=CLS_PLATE&&pPart->GetClassTypeId()!=CLS_PARAMPLATE)
				continue;
			CLDSGeneralPlate *pPlank = (CLDSGeneralPlate*)pPart;
			iOrder++;
			if(pPlank->m_bVirtualPart)
				continue;	//虚拟钢板不进行校核
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPlank->dwPermission))
				continue;	//网络协同时，对此钢板没有拥有修改权限
			else if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pPart->iSeg.iSeg)==NULL)
				continue;	//判断是否在指定查找段号范围内
			if(!pPlank->ValidateProfile(dlg.m_fScope))	//不合法的连接板边距
			{
#ifdef AFX_TARG_ENU_ENGLISH
				fprintf(fp,"%4d#  0X%X connecting plate {%s} bolt's margin isn't satisfied！\n",nInvalidHits+nHits+1,pPlank->handle,(char*)pPlank->GetPartNo());
#else 
				fprintf(fp,"%4d#  0X%X连接板{%s}螺栓边距不满足！\n",nInvalidHits+nHits+1,pPlank->handle,(char*)pPlank->GetPartNo());
#endif
				g_pSolidDraw->SetEntSnapStatus(pPlank->handle);
				nHits++;
			}
			pProcDlg->Refresh(iOrder*100/nPlates);
		}
	}
	if(nHits>0)
#ifdef AFX_TARG_ENU_ENGLISH
		fprintf(fp,"\nTotal：%d plate don't meet the requirement of bolt margin.\n\n\n",nHits);
#else 
		fprintf(fp,"\n总计：%d块板不满足螺栓边距要求。\n\n\n",nHits);
#endif
	nInvalidHits+=nHits;

	//螺栓通厚以及螺栓规格合法性检测
	iOrder=1;
	nHits=0;
	int nBolts=Ta.Parts.GetNodeNum(CLS_BOLT);
	for(CLDSBolt *pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
	{
		iOrder++;
		if(pBolt->m_bVirtualPart)
			continue;
		if(pBolt->get_d()>27)
			continue;	//暂时只检测27以下的螺栓 27以上的螺栓规格库不完整
		else if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pBolt->iSeg.iSeg)==NULL)
			continue;	//判断是否在指定查找段号范围内
		LS_XING_HAO *pLsXingHao=CLsLibrary::FindLsGuiGeByValidLen(pBolt->get_d(),pBolt->get_L(),pBolt->m_hFamily);
		if(pLsXingHao)
		{
			if(dlg.m_bBoltL0&&!pBolt->IsFootNail())
			{	//检测螺栓通厚
				if(pBolt->get_L0()<pLsXingHao->down_lim)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					fprintf(fp,"%4d#  0X%X bolt's L0 is too small，please check it！\n",nInvalidHits+nHits+1,pBolt->handle);
#else 
					fprintf(fp,"%4d#  0X%X螺栓通厚过小，请核实！\n",nInvalidHits+nHits+1,pBolt->handle);
#endif
					g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
					nHits++;
				}
				else if(pBolt->get_L0()>pLsXingHao->up_lim)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					fprintf(fp,"%4d#  0X%X bolt's L0 is too large，please check it！\n",nInvalidHits+nHits+1,pBolt->handle);
#else 
					fprintf(fp,"%4d#  0X%X螺栓通厚过大，请核实！\n",nInvalidHits+nHits+1,pBolt->handle);
#endif
					g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
					nHits++;
				}
			}
		}
		else
		{
			if(dlg.m_bBoltGuiGe)
			{	//检测螺栓规格
#ifdef AFX_TARG_ENU_ENGLISH
				fprintf(fp,"%4d#  0X%X The library of bolt specification without the specification，please check it！\n",nInvalidHits+nHits+1,pBolt->handle);
#else
				fprintf(fp,"%4d#  0X%X螺栓规格库中无此规格螺栓，请核实！\n",nInvalidHits+nHits+1,pBolt->handle);
#endif
				g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
				nHits++;
			}
		}
	}
	delete pProcDlg;
	nInvalidHits+=nHits;
	fclose(fp);
	if(nInvalidHits>0)
		WinExec("notepad.exe LsCheck.txt",SW_SHOW);
	else
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("Bolt's legality is checked successfully!");
#else 
		MessageBox("通过螺栓合法性检测!");
#endif
}

void CLDSView::OnCheckJgLsValid() 
{
	FILE *fp;
	fp = fopen("LsCheck.txt","wt");
	char sError[201];
	int nInvalidHits = 0;
	dlg.DoModal();
	for(CLDSLineAngle *pJg=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pJg;
		pJg=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
	{
		if(pJg->m_bVirtualPart)
			continue;
		else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pJg->dwPermission))
			continue;	//网络协同时，对此角钢没有拥有修改权限
		if(pJg->ValidLsList(sError,dlg.m_bRollingSpace,FALSE,FALSE,dlg.m_fScope)!=0)
		{
			g_pSolidDraw->SetEntSnapStatus(pJg->handle);
			fprintf(fp,"%4d#  ",nInvalidHits+1);
			fprintf(fp,sError);
			fprintf(fp,";\n");
			nInvalidHits++;
		}
		if(pJg->ValidLsList(sError,FALSE,dlg.m_bCutAngleDist,FALSE,dlg.m_fScope)!=0)
		{
			g_pSolidDraw->SetEntSnapStatus(pJg->handle);
			fprintf(fp,"%4d#  ",nInvalidHits+1);
			fprintf(fp,sError);
			fprintf(fp,";\n");
			nInvalidHits++;
		}
		if(pJg->ValidLsList(sError,FALSE,FALSE,dlg.m_bLsEndSpace,dlg.m_fScope)!=0)
		{
			g_pSolidDraw->SetEntSnapStatus(pJg->handle);
			fprintf(fp,"%4d#  ",nInvalidHits+1);
			fprintf(fp,sError);
			fprintf(fp,";\n");
			nInvalidHits++;
		}
	}
	fclose(fp);
	if(nInvalidHits>0)
		WinExec("notepad.exe LsCheck.txt",SW_SHOW);
	else
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("Bolt's legality is checked successfully!");
#else 
		MessageBox("通过角钢螺栓合法性检测!");
#endif
}

void CLDSView::OnBoltValidCheck() 
{
	m_nPrevCommandID=(ID_BOLT_VALID_CHECK);
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat checking bolt";
#else 
	m_sPrevCommandName="重复重叠螺栓校核";
#endif
	FILE *fp;
	fp = fopen("boltvalid.txt","wt");
	int i=0,nInvalidHits = 0;
	CLDSBolt *pBolt=NULL;
	int part_n=Ta.Parts.GetNodeNum(),bolt_n=Ta.Parts.GetNodeNum(CLS_BOLT);
	DisplayProcess(0);
	ULONG nn=bolt_n*(bolt_n-1)/2;
	for(pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
	{
		i++;
		ULONG process=(2*bolt_n-1-i)*i/2;
		DisplayProcess((process*70)/nn);
		if(pBolt->m_bVirtualPart)
			continue;
		else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBolt->dwPermission))
			continue;	//网络协同时，对此角钢没有拥有修改权限
		pBolt->feature=0;	//此时feature属性表示螺栓被孔引用的次数，在此先清零
		BOOL pushed=Ta.Parts.push_stack();
		for(CLDSBolt *pOtherBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT);pOtherBolt;pOtherBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
		{
			if(!pOtherBolt->cfgword.And(pBolt->cfgword))
				continue;	//配材号没有交集，不需要检查
			if(DISTANCE(pBolt->ucs.origin,pOtherBolt->ucs.origin)<pBolt->get_d())
			{
				g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
				g_pSolidDraw->SetEntSnapStatus(pOtherBolt->handle);
#ifdef AFX_TARG_ENU_ENGLISH
				fprintf(fp,"%4d# 0X%X and 0X%X bolt's position is too close,it has conflicts！ \n",nInvalidHits+1,pBolt->handle,pOtherBolt->handle);
#else 
				fprintf(fp,"%4d#0X%X与0X%X螺栓位置过近，产生冲突！ \n",nInvalidHits+1,pBolt->handle,pOtherBolt->handle);
#endif
				nInvalidHits++;
			}
		}
		Ta.Parts.pop_stack(pushed);
	}
	CSuperSmartPtr<CLDSPart>pSuperPart;
	i=0;
	for(pSuperPart=Ta.Parts.GetFirst();pSuperPart.IsHasPtr();pSuperPart=Ta.Parts.GetNext())
	{
		i++;
		DisplayProcess(70+(i*15)/part_n);
		CLsRefList *pLsRefList=pSuperPart->GetLsRefList();
		if(pLsRefList==NULL)
			continue;
		for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
			(*pLsRef)->feature++;	//统计每个螺栓在螺栓孔中的引用次数
	}
	i=0;
	for(pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
	{
		i++;
		DisplayProcess(85+(i*15)/part_n);
		if(pBolt->m_bVirtualPart)
			continue;
		else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBolt->dwPermission))
			continue;	//网络协同时，对此角钢没有拥有修改权限
		else if(pBolt->feature==0)
		{
			g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
#ifdef AFX_TARG_ENU_ENGLISH
			fprintf(fp,"%4d#0X%X bolt can't connect any part,it may be the excess bolt！\n",nInvalidHits+1,pBolt->handle);
#else 
			fprintf(fp,"%4d#0X%X螺栓未连接任何构件，可能为多余螺栓！ \n",nInvalidHits+1,pBolt->handle);
#endif
			nInvalidHits++;
		}
		else if(pBolt->feature==1&&!pBolt->IsFootNail())
		{
			g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
#ifdef AFX_TARG_ENU_ENGLISH
			fprintf(fp,"%4d#0X%X bolt connects only one part ,it may lack of bolt's holes！ \n",nInvalidHits+1,pBolt->handle);
#else 
			fprintf(fp,"%4d#0X%X螺栓只连接了一个构件，可能缺少螺栓孔！ \n",nInvalidHits+1,pBolt->handle);
#endif
			nInvalidHits++;
		}
	}
	DisplayProcess(100);
	fclose(fp);
	if(nInvalidHits>0)
		WinExec("notepad.exe boltvalid.txt",SW_SHOW);
	else
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("The bolt's redundancy is checked successfully!");
#else 
		MessageBox("通过冗余螺栓校核!");
#endif
}

void CLDSView::OnDisplayOrHideAllHole() 
{
	m_nPrevCommandID=ID_DISPLAY_OR_HIDE_ALL_HOLE;
	if(g_sysPara.bDisplayAllHole)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_sPrevCommandName="Display All Holes";
#else 
		m_sPrevCommandName="显示所有孔";
#endif
		g_sysPara.bDisplayAllHole=FALSE;
		g_sysPara.display.bSolidLs = TRUE;
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_sPrevCommandName="Hide All Bolts";
#else 
		m_sPrevCommandName="隐藏所有孔";
#endif
		g_sysPara.bDisplayAllHole=TRUE;
		g_sysPara.display.bSolidLs = FALSE;
	}
	long *id_arr=NULL;
	int n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	PARTSET partset;
	for(int i=0;i<n;i++)
	{
		CLDSPart* pPart=Ta.Parts.FromHandle(id_arr[i]);
		if(pPart==NULL)
			continue;
		if(pPart->GetLsRefList()!=NULL)
		{
			partset.append(pPart);
			pPart->SetSolidModified();
			pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole);
			pPart->SetSolidModified();
			g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
		}
		else if(pPart->GetClassTypeId()==CLS_BOLT&&g_sysPara.bDisplayAllHole)
			g_pSolidDraw->DelEnt(pPart->handle);
	}
	if(partset.GetNodeNum()>0)
	{
		if(g_sysPara.bDisplayAllHole)
			g_pSolidDraw->Draw();
		else
			RedrawAll();
		return;
	}
	else
	{	//未选中任何构件时，渲染所有构件的实体 wjh-2015.4.15
		for(CLDSLineAngle *pJg=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pJg;pJg=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
			pJg->SetSolidModified();
		for(CLDSLineSlot *pSlot=(CLDSLineSlot*)Ta.Parts.GetFirst(CLS_LINESLOT);pSlot;pSlot=(CLDSLineSlot*)Ta.Parts.GetNext(CLS_LINESLOT))
			pSlot->SetSolidModified();
		for(CLDSLineFlat *pFlat=(CLDSLineFlat*)Ta.Parts.GetFirst(CLS_LINEFLAT);pFlat;pFlat=(CLDSLineFlat*)Ta.Parts.GetNext(CLS_LINEFLAT))
			pFlat->SetSolidModified();
		for(CLDSParamPlate *pParamPlate=(CLDSParamPlate*)Ta.Parts.GetFirst(CLS_PARAMPLATE);pParamPlate;pParamPlate=(CLDSParamPlate*)Ta.Parts.GetNext(CLS_PARAMPLATE))
			pParamPlate->SetSolidModified();
		/*for(CLDSPolyAngle *pHuoQuJg=(CLDSPolyAngle*)Ta.Parts.GetFirst(CLS_POLYANGLE);pHuoQuJg;
		pHuoQuJg=(CLDSPolyAngle*)Ta.Parts.GetNext(CLS_POLYANGLE))
		pHuoQuJg->SetModified();*/
		for(CLDSPlate *pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
			pPlate->SetSolidModified();
		RedrawAll();
	}
}

/*void CLDSView::OnExportJgGuigeList() 
{
	FILE *fp;
	fp = fopen("PoleGuige.txt","wt");
	ATOM_LIST<JGGUIGE>guige_list;
	JGGUIGE *pGuige;
	for(CLDSLineAngle *pJg=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pJg;pJg=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
	{
		JGGUIGE guige;
		guige.cMaterial=pJg->cMaterial;
		guige.wing_wide=pJg->GetWidth();
		guige.wing_thick=pJg->GetThick();
		int ii=0;
		BOOL bProcess=FALSE;
		for(pGuige=guige_list.GetFirst();pGuige;pGuige=guige_list.GetNext())
		{
			if(guige.cMaterial<pGuige->cMaterial)
			{
				guige_list.insert(guige);
				bProcess=TRUE;
				break;
			}
			else if(guige.wing_wide-pGuige->wing_wide<-EPS)
			{
				guige_list.insert(guige);
				bProcess=TRUE;
				break;
			}
			else if(guige.wing_thick-pGuige->wing_thick<-EPS)
			{
				guige_list.insert(guige);
				bProcess=TRUE;
				break;
			}
			else if(guige.cMaterial==pGuige->cMaterial
				&&fabs(guige.wing_wide-pGuige->wing_wide)<EPS
				&&fabs(guige.wing_thick-pGuige->wing_thick)<EPS)
			{
				bProcess=TRUE;
				break;	//规格相同
			}
			else
				continue;
		}
		if(!bProcess)
			guige_list.append(guige);
	}
	fprintf(fp,"角钢规格列表:\n");
	int i=0;
	for(pGuige=guige_list.GetFirst();pGuige;pGuige=guige_list.GetNext())
	{
		i++;
		if(toupper(pGuige->cMaterial)==CSteelMatLibrary::Q235BriefMark());//'S')
			fprintf(fp,"%4d#Q235L%3.0fX%2.0f :",i,pGuige->wing_wide,pGuige->wing_thick);
		else if(toupper(pGuige->cMaterial)==CSteelMatLibrary::Q345BriefMark());//'H')
			fprintf(fp,"%4d#Q345L%3.0fX%2.0f :",i,pGuige->wing_wide,pGuige->wing_thick);
		else if(toupper(pGuige->cMaterial)==CSteelMatLibrary::Q390BriefMark());//'G')
			fprintf(fp,"%4d#Q390L%3.0fX%2.0f :",i,pGuige->wing_wide,pGuige->wing_thick);
		int nHits=0;
		for(pJg=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pJg;pJg=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
		{
			if(pJg->cMaterial!=pGuige->cMaterial
				||fabs(pJg->GetWidth()-pGuige->wing_wide)>EPS
				||fabs(pJg->GetThick()-pGuige->wing_thick)>EPS)
				continue;	//不是此种规格
			if(nHits!=0)
				fprintf(fp,",");
			nHits++;
			if(pJg->sub_type&TA_JG_EXPLODE)
				fprintf(fp,"0X%X(被拆分)",pJg->handle);
			else if(pJg->sub_type==TA_JG_VIRTUAL)
				fprintf(fp,"0X%X(隐含角钢)",pJg->handle);
			else if(pJg->m_bVirtualPart)
				fprintf(fp,"0X%X(虚角钢)",pJg->handle);
			else
				fprintf(fp,"0X%X",pJg->handle);
		}
		fprintf(fp,";\n");
		fprintf(fp,"     小计:%d根.\n",nHits);
	}
	ATOM_LIST<TUBEGUIGE>tube_guige_list;
	TUBEGUIGE *pTubeGuige;
	for(CLDSLineTube *pLineTube=(CLDSLineTube*)Ta.Parts.GetFirst(CLS_LINETUBE);pLineTube;pLineTube=(CLDSLineTube*)Ta.Parts.GetNext(CLS_LINETUBE))
	{
		TUBEGUIGE tubeguige;
		tubeguige.cMaterial=pLineTube->cMaterial;
		tubeguige.d=pLineTube->d;
		tubeguige.t=pLineTube->t;
		int ii=0;
		BOOL bProcess=FALSE;
		for(pTubeGuige=tube_guige_list.GetFirst();pTubeGuige;pTubeGuige=tube_guige_list.GetNext())
		{
			if(tubeguige.cMaterial<pTubeGuige->cMaterial)
			{
				tube_guige_list.insert(tubeguige);
				bProcess=TRUE;
				break;
			}
			else if(tubeguige.d-pTubeGuige->d<-EPS)
			{
				tube_guige_list.insert(tubeguige);
				bProcess=TRUE;
				break;
			}
			else if(tubeguige.t-pTubeGuige->t<-EPS)
			{
				tube_guige_list.insert(tubeguige);
				bProcess=TRUE;
				break;
			}
			else if(tubeguige.cMaterial==pTubeGuige->cMaterial
				&&fabs(tubeguige.d-pTubeGuige->d)<EPS
				&&fabs(tubeguige.t-pTubeGuige->t)<EPS)
			{
				bProcess=TRUE;
				break;	//规格相同
			}
			else
				continue;
		}
		if(!bProcess)
			tube_guige_list.append(tubeguige);
	}
	fprintf(fp,"\n钢管规格列表:\n");
	int j=0;
	for(pTubeGuige=tube_guige_list.GetFirst();pTubeGuige;pTubeGuige=tube_guige_list.GetNext())
	{
		j++;
		if(toupper(pTubeGuige->cMaterial)==CSteelMatLibrary::Q235BriefMark());//'S')
			fprintf(fp,"%4d#Q235Φ%3.0fX%2.0f :",j,pTubeGuige->d,pTubeGuige->t);
		else if(toupper(pTubeGuige->cMaterial)==CSteelMatLibrary::Q345BriefMark());//'H')
			fprintf(fp,"%4d#Q345Φ%3.0fX%2.0f :",j,pTubeGuige->d,pTubeGuige->t);
		else if(toupper(pTubeGuige->cMaterial)==CSteelMatLibrary::Q390BriefMark());//'G')
			fprintf(fp,"%4d#Q390Φ%3.0fX%2.0f :",j,pTubeGuige->d,pTubeGuige->t);
		int nHits=0;
		for(CLDSLineTube *pLineTube=(CLDSLineTube*)Ta.Parts.GetFirst(CLS_LINETUBE);pLineTube;pLineTube=(CLDSLineTube*)Ta.Parts.GetNext(CLS_LINETUBE))
		{
			if(pLineTube->cMaterial!=pTubeGuige->cMaterial
				||fabs(pLineTube->d-pTubeGuige->d)>EPS
				||fabs(pLineTube->t-pTubeGuige->t)>EPS)
				continue;	//不是此种规格
			if(nHits!=0)
				fprintf(fp,",");
			nHits++;
			if(!pLineTube->is_visible)
				fprintf(fp,"0X%X(隐含钢管)",pLineTube->handle);
			else if(pLineTube->m_bVirtualPart)
				fprintf(fp,"0X%X(虚钢管)",pLineTube->handle);
			else
				fprintf(fp,"0X%X",pLineTube->handle);
		}
		fprintf(fp,";\n");
		fprintf(fp,"     小计:%d根.\n",nHits);
	}
	fclose(fp);
	WinExec("notepad.exe PoleGuige.txt",SW_SHOW);
}*/

static double epsilon=0.5;
void CLDSView::OnCheckPlankLsValid() 
{
	FILE *fp=fopen("ljbls_chk.txt","wt");
	int nRecN=0;
	CProcBarDlg *pProcDlg = NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	Input("Input the range of error margin：",&epsilon,'f');
#else 
	Input("输入边距误差范围：",&epsilon,'f');
#endif
	pProcDlg = new CProcBarDlg(AfxGetApp()->m_pMainWnd);
	pProcDlg->Create();
	int iOrder=1;
	int nHits=0;
	int nPlates = Ta.Parts.GetNodeNum(CLS_PLATE);
	for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_PLATE&&pPart->GetClassTypeId()!=CLS_PARAMPLATE)
			continue;
		CLDSGeneralPlate *pPlank = (CLDSGeneralPlate*)pPart;
		iOrder++;
		if(pPlank->m_bVirtualPart)
			continue;	//虚拟钢板不进行边距检测
		if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPlank->dwPermission))
			continue;	//网络协同时，对此钢板没有拥有修改权限
		//if(pPlank->sub_type&TA_PLANK_DIANBAN)
		//	continue;
		if(!pPlank->ValidateProfile(epsilon))	//不合法的连接板边距
		{
#ifdef AFX_TARG_ENU_ENGLISH
			fprintf(fp,"0X%X connect plate{%s} margin of bolt isn't satisfied！\n",pPlank->handle,(char*)pPlank->GetPartNo());
#else 
			fprintf(fp,"0X%X连接板{%s}螺栓边距不满足！\n",pPlank->handle,(char*)pPlank->GetPartNo());
#endif
			g_pSolidDraw->SetEntSnapStatus(pPlank->GetBaseNode()->handle);
			nHits++;
		}
		pProcDlg->Refresh(iOrder*100/nPlates);
	}
	if(nHits>0)
#ifdef AFX_TARG_ENU_ENGLISH
		fprintf(fp,"\nTotal：%d plate does not meet the requirements of bolt margin。",nHits);
#else 
		fprintf(fp,"\n总计：%d块板不满足螺栓边距要求。",nHits);
#endif
	delete pProcDlg;
	fclose(fp);
	if(nHits>0)
		WinExec("notepad.exe ljbls_chk.txt",SW_SHOW);
	else
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("The margin of connect plates are checked successfully!");
#else 
		MessageBox("顺利通过连接板边距校核!");
#endif
}
#endif