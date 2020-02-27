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
	m_sPrevCommandName="�ظ���˨�Ϸ��Լ��";
#endif
	FILE *fp;
	fp = fopen("LsCheck.txt","wt");
	char sError[201];
	int nInvalidHits = 0;
	if(dlg.DoModal()!=IDOK)
		return;
	//�ֽ�κ��ַ���
	CHashList<SEGI>segTbl;
	GetSegNoHashTblBySegStr(dlg.m_sSegStr,segTbl);
	//�Ǹ���˨�Ϸ��Լ��
	for(CLDSLineAngle *pJg=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pJg;
		pJg=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
	{
		if(pJg->m_bVirtualPart)
			continue;
		else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pJg->dwPermission))
			continue;	//����Эͬʱ���Դ˽Ǹ�û��ӵ���޸�Ȩ��
		else if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pJg->iSeg.iSeg)==NULL)
			continue;	//�ж��Ƿ���ָ�����Ҷκŷ�Χ��
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
	//������˨�߾���	
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
				continue;	//����ְ岻����У��
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPlank->dwPermission))
				continue;	//����Эͬʱ���Դ˸ְ�û��ӵ���޸�Ȩ��
			else if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pPart->iSeg.iSeg)==NULL)
				continue;	//�ж��Ƿ���ָ�����Ҷκŷ�Χ��
			if(!pPlank->ValidateProfile(dlg.m_fScope))	//���Ϸ������Ӱ�߾�
			{
#ifdef AFX_TARG_ENU_ENGLISH
				fprintf(fp,"%4d#  0X%X connecting plate {%s} bolt's margin isn't satisfied��\n",nInvalidHits+nHits+1,pPlank->handle,(char*)pPlank->GetPartNo());
#else 
				fprintf(fp,"%4d#  0X%X���Ӱ�{%s}��˨�߾಻���㣡\n",nInvalidHits+nHits+1,pPlank->handle,(char*)pPlank->GetPartNo());
#endif
				g_pSolidDraw->SetEntSnapStatus(pPlank->handle);
				nHits++;
			}
			pProcDlg->Refresh(iOrder*100/nPlates);
		}
	}
	if(nHits>0)
#ifdef AFX_TARG_ENU_ENGLISH
		fprintf(fp,"\nTotal��%d plate don't meet the requirement of bolt margin.\n\n\n",nHits);
#else 
		fprintf(fp,"\n�ܼƣ�%d��岻������˨�߾�Ҫ��\n\n\n",nHits);
#endif
	nInvalidHits+=nHits;

	//��˨ͨ���Լ���˨���Ϸ��Լ��
	iOrder=1;
	nHits=0;
	int nBolts=Ta.Parts.GetNodeNum(CLS_BOLT);
	for(CLDSBolt *pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
	{
		iOrder++;
		if(pBolt->m_bVirtualPart)
			continue;
		if(pBolt->get_d()>27)
			continue;	//��ʱֻ���27���µ���˨ 27���ϵ���˨���ⲻ����
		else if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pBolt->iSeg.iSeg)==NULL)
			continue;	//�ж��Ƿ���ָ�����Ҷκŷ�Χ��
		LS_XING_HAO *pLsXingHao=CLsLibrary::FindLsGuiGeByValidLen(pBolt->get_d(),pBolt->get_L(),pBolt->m_hFamily);
		if(pLsXingHao)
		{
			if(dlg.m_bBoltL0&&!pBolt->IsFootNail())
			{	//�����˨ͨ��
				if(pBolt->get_L0()<pLsXingHao->down_lim)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					fprintf(fp,"%4d#  0X%X bolt's L0 is too small��please check it��\n",nInvalidHits+nHits+1,pBolt->handle);
#else 
					fprintf(fp,"%4d#  0X%X��˨ͨ���С�����ʵ��\n",nInvalidHits+nHits+1,pBolt->handle);
#endif
					g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
					nHits++;
				}
				else if(pBolt->get_L0()>pLsXingHao->up_lim)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					fprintf(fp,"%4d#  0X%X bolt's L0 is too large��please check it��\n",nInvalidHits+nHits+1,pBolt->handle);
#else 
					fprintf(fp,"%4d#  0X%X��˨ͨ��������ʵ��\n",nInvalidHits+nHits+1,pBolt->handle);
#endif
					g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
					nHits++;
				}
			}
		}
		else
		{
			if(dlg.m_bBoltGuiGe)
			{	//�����˨���
#ifdef AFX_TARG_ENU_ENGLISH
				fprintf(fp,"%4d#  0X%X The library of bolt specification without the specification��please check it��\n",nInvalidHits+nHits+1,pBolt->handle);
#else
				fprintf(fp,"%4d#  0X%X��˨�������޴˹����˨�����ʵ��\n",nInvalidHits+nHits+1,pBolt->handle);
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
		MessageBox("ͨ����˨�Ϸ��Լ��!");
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
			continue;	//����Эͬʱ���Դ˽Ǹ�û��ӵ���޸�Ȩ��
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
		MessageBox("ͨ���Ǹ���˨�Ϸ��Լ��!");
#endif
}

void CLDSView::OnBoltValidCheck() 
{
	m_nPrevCommandID=(ID_BOLT_VALID_CHECK);
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat checking bolt";
#else 
	m_sPrevCommandName="�ظ��ص���˨У��";
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
			continue;	//����Эͬʱ���Դ˽Ǹ�û��ӵ���޸�Ȩ��
		pBolt->feature=0;	//��ʱfeature���Ա�ʾ��˨�������õĴ������ڴ�������
		BOOL pushed=Ta.Parts.push_stack();
		for(CLDSBolt *pOtherBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT);pOtherBolt;pOtherBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
		{
			if(!pOtherBolt->cfgword.And(pBolt->cfgword))
				continue;	//��ĺ�û�н���������Ҫ���
			if(DISTANCE(pBolt->ucs.origin,pOtherBolt->ucs.origin)<pBolt->get_d())
			{
				g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
				g_pSolidDraw->SetEntSnapStatus(pOtherBolt->handle);
#ifdef AFX_TARG_ENU_ENGLISH
				fprintf(fp,"%4d# 0X%X and 0X%X bolt's position is too close,it has conflicts�� \n",nInvalidHits+1,pBolt->handle,pOtherBolt->handle);
#else 
				fprintf(fp,"%4d#0X%X��0X%X��˨λ�ù�����������ͻ�� \n",nInvalidHits+1,pBolt->handle,pOtherBolt->handle);
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
			(*pLsRef)->feature++;	//ͳ��ÿ����˨����˨���е����ô���
	}
	i=0;
	for(pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
	{
		i++;
		DisplayProcess(85+(i*15)/part_n);
		if(pBolt->m_bVirtualPart)
			continue;
		else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBolt->dwPermission))
			continue;	//����Эͬʱ���Դ˽Ǹ�û��ӵ���޸�Ȩ��
		else if(pBolt->feature==0)
		{
			g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
#ifdef AFX_TARG_ENU_ENGLISH
			fprintf(fp,"%4d#0X%X bolt can't connect any part,it may be the excess bolt��\n",nInvalidHits+1,pBolt->handle);
#else 
			fprintf(fp,"%4d#0X%X��˨δ�����κι���������Ϊ������˨�� \n",nInvalidHits+1,pBolt->handle);
#endif
			nInvalidHits++;
		}
		else if(pBolt->feature==1&&!pBolt->IsFootNail())
		{
			g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
#ifdef AFX_TARG_ENU_ENGLISH
			fprintf(fp,"%4d#0X%X bolt connects only one part ,it may lack of bolt's holes�� \n",nInvalidHits+1,pBolt->handle);
#else 
			fprintf(fp,"%4d#0X%X��˨ֻ������һ������������ȱ����˨�ף� \n",nInvalidHits+1,pBolt->handle);
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
		MessageBox("ͨ��������˨У��!");
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
		m_sPrevCommandName="��ʾ���п�";
#endif
		g_sysPara.bDisplayAllHole=FALSE;
		g_sysPara.display.bSolidLs = TRUE;
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_sPrevCommandName="Hide All Bolts";
#else 
		m_sPrevCommandName="�������п�";
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
	{	//δѡ���κι���ʱ����Ⱦ���й�����ʵ�� wjh-2015.4.15
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
				break;	//�����ͬ
			}
			else
				continue;
		}
		if(!bProcess)
			guige_list.append(guige);
	}
	fprintf(fp,"�Ǹֹ���б�:\n");
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
				continue;	//���Ǵ��ֹ��
			if(nHits!=0)
				fprintf(fp,",");
			nHits++;
			if(pJg->sub_type&TA_JG_EXPLODE)
				fprintf(fp,"0X%X(�����)",pJg->handle);
			else if(pJg->sub_type==TA_JG_VIRTUAL)
				fprintf(fp,"0X%X(�����Ǹ�)",pJg->handle);
			else if(pJg->m_bVirtualPart)
				fprintf(fp,"0X%X(��Ǹ�)",pJg->handle);
			else
				fprintf(fp,"0X%X",pJg->handle);
		}
		fprintf(fp,";\n");
		fprintf(fp,"     С��:%d��.\n",nHits);
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
				break;	//�����ͬ
			}
			else
				continue;
		}
		if(!bProcess)
			tube_guige_list.append(tubeguige);
	}
	fprintf(fp,"\n�ֹܹ���б�:\n");
	int j=0;
	for(pTubeGuige=tube_guige_list.GetFirst();pTubeGuige;pTubeGuige=tube_guige_list.GetNext())
	{
		j++;
		if(toupper(pTubeGuige->cMaterial)==CSteelMatLibrary::Q235BriefMark());//'S')
			fprintf(fp,"%4d#Q235��%3.0fX%2.0f :",j,pTubeGuige->d,pTubeGuige->t);
		else if(toupper(pTubeGuige->cMaterial)==CSteelMatLibrary::Q345BriefMark());//'H')
			fprintf(fp,"%4d#Q345��%3.0fX%2.0f :",j,pTubeGuige->d,pTubeGuige->t);
		else if(toupper(pTubeGuige->cMaterial)==CSteelMatLibrary::Q390BriefMark());//'G')
			fprintf(fp,"%4d#Q390��%3.0fX%2.0f :",j,pTubeGuige->d,pTubeGuige->t);
		int nHits=0;
		for(CLDSLineTube *pLineTube=(CLDSLineTube*)Ta.Parts.GetFirst(CLS_LINETUBE);pLineTube;pLineTube=(CLDSLineTube*)Ta.Parts.GetNext(CLS_LINETUBE))
		{
			if(pLineTube->cMaterial!=pTubeGuige->cMaterial
				||fabs(pLineTube->d-pTubeGuige->d)>EPS
				||fabs(pLineTube->t-pTubeGuige->t)>EPS)
				continue;	//���Ǵ��ֹ��
			if(nHits!=0)
				fprintf(fp,",");
			nHits++;
			if(!pLineTube->is_visible)
				fprintf(fp,"0X%X(�����ֹ�)",pLineTube->handle);
			else if(pLineTube->m_bVirtualPart)
				fprintf(fp,"0X%X(��ֹ�)",pLineTube->handle);
			else
				fprintf(fp,"0X%X",pLineTube->handle);
		}
		fprintf(fp,";\n");
		fprintf(fp,"     С��:%d��.\n",nHits);
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
	Input("Input the range of error margin��",&epsilon,'f');
#else 
	Input("����߾���Χ��",&epsilon,'f');
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
			continue;	//����ְ岻���б߾���
		if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPlank->dwPermission))
			continue;	//����Эͬʱ���Դ˸ְ�û��ӵ���޸�Ȩ��
		//if(pPlank->sub_type&TA_PLANK_DIANBAN)
		//	continue;
		if(!pPlank->ValidateProfile(epsilon))	//���Ϸ������Ӱ�߾�
		{
#ifdef AFX_TARG_ENU_ENGLISH
			fprintf(fp,"0X%X connect plate{%s} margin of bolt isn't satisfied��\n",pPlank->handle,(char*)pPlank->GetPartNo());
#else 
			fprintf(fp,"0X%X���Ӱ�{%s}��˨�߾಻���㣡\n",pPlank->handle,(char*)pPlank->GetPartNo());
#endif
			g_pSolidDraw->SetEntSnapStatus(pPlank->GetBaseNode()->handle);
			nHits++;
		}
		pProcDlg->Refresh(iOrder*100/nPlates);
	}
	if(nHits>0)
#ifdef AFX_TARG_ENU_ENGLISH
		fprintf(fp,"\nTotal��%d plate does not meet the requirements of bolt margin��",nHits);
#else 
		fprintf(fp,"\n�ܼƣ�%d��岻������˨�߾�Ҫ��",nHits);
#endif
	delete pProcDlg;
	fclose(fp);
	if(nHits>0)
		WinExec("notepad.exe ljbls_chk.txt",SW_SHOW);
	else
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("The margin of connect plates are checked successfully!");
#else 
		MessageBox("˳��ͨ�����Ӱ�߾�У��!");
#endif
}
#endif