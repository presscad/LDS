#include "stdafx.h"
#include "LDS.h"
#include "stdio.h"
#include "math.h"
#include "Tower.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "LDSDoc.h"
#include "MainFrm.h"
#include "CmdLineDlg.h"
//#include "MsgPromptDlg.h"
#include "excel9.h"
#include "database.h"
#include "Query.h"
#include "SortFunc.h"
#include "XhCharString.h"
#if defined(__LDS_)||defined(__TSA_)
#include "PreProcessor.h"
#include "PostProcessor.h"
#include "PostProcess.h"
#include "PostProcessReport.h"
#include "EarthQuake.h"
#include "LibraryQuery.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if defined(__LDS_)||defined(__TSA_)
//������
/*
int compare_func4(const POLE_PTR &pPole1,const POLE_PTR &pPole2)
{
	long No1[2],No2[2];
	No1[0]=min(pPole1->pStart->point_i,pPole1->pEnd->point_i);
	No1[1]=max(pPole1->pStart->point_i,pPole1->pEnd->point_i);
	No2[0]=min(pPole2->pStart->point_i,pPole2->pEnd->point_i);
	No2[1]=max(pPole2->pStart->point_i,pPole2->pEnd->point_i);
	if(No1[0]<No2[0])
		return -1;
	else if(No1[0]==No2[0])
	{
		if(No1[1]<No2[1])
			return -1;
		else if(No1[1]==No2[1])
			return 0;
		else
			return 1;
	}
	else
		return 1;
}*/

int compare_func3(const REACTION_PTR &pReact1,const REACTION_PTR &pReact2)
{
	if(pReact1->pNode&&pReact2->pNode&& pReact1->pNode->point_i<pReact2->pNode->point_i)
		return -1;
	else if(pReact1->pNode&&pReact2->pNode&&pReact1->pNode->point_i==pReact2->pNode->point_i)
	{
		if(pReact1->pModule&&pReact2->pModule&& pReact1->pModule->iNo<pReact2->pModule->iNo)
			return -1;
		else if(pReact1->pModule&&pReact2->pModule&& pReact1->pModule->iNo==pReact2->pModule->iNo)
		{
			if(pReact1->pStatus&&pReact2->pStatus&& pReact1->pStatus->iNo<pReact2->pStatus->iNo)
				return -1;
			else if(pReact1->pStatus&&pReact2->pStatus&& pReact1->pStatus->iNo==pReact2->pStatus->iNo)
			{
				long just=0;
				long key_arr[4]={1000,100,10,1};
				for(int i=0;i<4;i++)
				{
					if(pReact1->leg_no[i]>pReact2->leg_no[i])
						just+=key_arr[i];
					else if(pReact1->leg_no[i]<pReact2->leg_no[i])
						just-=key_arr[i];
				}
				if(just>0)
					return 1;
				else if(just<0)
					return -1;
				else
					return 0;
			}
			else
				return 1;
		}
		else
			return 1;
	}
	else
		return 1;
}

//from AnalyzeProcessor.cpp
int compare_func1(const NODE_PTR& pNode1,const NODE_PTR& pNode2);
int compare_func2(const ELEM_PTR &pElem1,const ELEM_PTR &pElem2);
void CreateDetailRptTxtFileByEN(char *file_name,CFGWORD cfgwordBody,CFGWORD cfgwordLeg,int rpt_type)
{
	CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	
	char file_path[MAX_PATH],ext[5],filter[MAX_PATH];
	strcpy(file_path,file_name);
	if(rpt_type==1)
		strcpy(ext,".db2");
	else if(rpt_type==2)
		strcpy(ext,".eqk");
	else
		strcpy(ext,".rpt");
	strcat(file_path,ext);
	CFileFind fileFind;
	if(fileFind.FindFile(file_path))
	{
		sprintf(filter,"������(*%s)|*%s||",ext,ext);
		CFileDialog savedlg(FALSE,ext,file_path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filter);
		if(savedlg.DoModal()!=IDOK)
			return;
		sprintf(file_path,"%s",savedlg.GetFileName());
	}
	FILE *fp=fopen(file_path,"wt");
	if(fp==NULL)
	{
		pCmdLine->PromptStrInCmdLine("Report�ļ�����ʧ�ܣ�");
		return;
	}
	fprintf(fp,"\n");
	fprintf(fp,"***********************************************************************************************\n");
	fprintf(fp,"*                                                                                             *\n");
	fprintf(fp,"*            TOWER ANALYSIS AND DESIGN  - (C) 2002-2006 Xerofox Software,Ltd.                 *\n");
	fprintf(fp,"*                                                                                             *\n");
	fprintf(fp,"***********************************************************************************************\n");
	fprintf(fp,"Project Code : %s\n",Ta.m_sPrjCode);
	fprintf(fp,"Project Name : %s\n",Ta.m_sPrjName);
	fprintf(fp,"Designed by  : %s\n",Ta.m_sOperator);
	CTime t=CTime::GetCurrentTime();
	fprintf(fp,"Date run     : %s\n",t.Format("%H:%M:%S %B %d, %Y"));
	CString product_version;
	theApp.GetProductVersion(product_version);
	fprintf(fp,"by           : TSA Version %s\n",product_version);
	fprintf(fp,"Licensed to  : %s\n",Ta.m_sCompanyName);
	fprintf(fp,"\n");
	if(g_sysPara.AnalysisPara.m_iNonlinearStyle==0)
		fprintf(fp,"Successfully performed linear analysis!\n");
	else
		fprintf(fp,"Successfully performed nonlinear analysis!\n");
	fprintf(fp,"\n");
	fprintf(fp,"===============================================================================================\n");
	fprintf(fp,"Part ��:Design Code\n");
	if(Ta.m_iMemberSizeCheckCode==0)
		fprintf(fp,"Member check option:DL/T 5092-1999 and DL/T 5154-2002, China.\n");
	else
		fprintf(fp,"Member check option:ASCE 10-97, America.\n");
	fprintf(fp,"\n");
	fprintf(fp,"==================================================================================================\n");
	fprintf(fp,"Part ��: Steel Material Properties\n\n");
	
	fprintf(fp,"When diameter or thickness ��16 mm, it belongs to \"Group 1\".\n");
	fprintf(fp,"When diameter or thickness ��16~40 mm for Q235, it belongs to \"Group 2\".\n");
	fprintf(fp,"When diameter or thickness ��16~35 mm for Q345 and Q390, it belongs to \"Group 2\".\n");
	fprintf(fp,"When diameter or thickness ��40~60 mm for Q235, it belongs to \"Group 3\".\n");
	fprintf(fp,"When diameter or thickness ��35~50 mm for Q345 and Q390, it belongs to \"Group 3\".\n");
	fprintf(fp,"\n");
	fprintf(fp,"   Steel|  Brief|     Modulus|     Design|     Design|     Design|  Shearing|  Shearing|  Shearing\n");
	fprintf(fp,"Material|       |          of|   Strength|   Strength|   Strength|  Strength|  Strength|  Strength\n");
	fprintf(fp,"   Label|   Mark|  Elasticity|    Group 1|    Group 2|    Group 3|   Group 1|   Group 2|   Group 3\n");
	fprintf(fp,"        |       |       (MPa)|      (MPa)|      (MPa)|      (MPa)|     (MPa)|     (MPa)|     (MPa)\n");
	fprintf(fp,"--------------------------------------------------------------------------------------------------\n");
	for(int i=0;i<CSteelMatLibrary::GetCount();i++)
	{
		STEELMAT mat=CSteelMatLibrary::RecordAt(i);
		fprintf(fp,"    %s     '%C'   %10.2f  %10.2f  %10.2f  %10.2f %10.2f %10.2f %10.2f\n",
			mat.mark,mat.cBriefMark,mat.E,mat.intensity[4],mat.intensity[5],
			mat.intensity[6],mat.intensity[8],mat.intensity[9],mat.intensity[10]);
	}
	fprintf(fp,"\n");
	fprintf(fp,"================================\n");
	fprintf(fp,"Part ��: Bolt Properties\n");
	fprintf(fp,"\n");
	fprintf(fp," Bolt|Shearing| Tension| Bearing\n");
	fprintf(fp,"Grade|Strength|Strength|Strength\n");
	fprintf(fp,"     |   (MPa)|   (MPa)|   (MPa)\n");
	fprintf(fp,"--------------------------------\n");
	for(i=0;i<LsTypeCount;i++)
	{
		if(i<LsTypeCount)
			fprintf(fp,"  %s %8.2f %8.2f %8.2f\n",ls_table[i].grade,ls_table[i].fv,ls_table[i].f,ls_table[i].fc);
	}
	fprintf(fp,"\n");
	fprintf(fp,"===============================================================================================\n");
	fprintf(fp,"Part ��: Nodes Geometry\n");
	fprintf(fp,"\n");
	fprintf(fp," Node|   Symmetry| X Coord.| Y Coord.| Z Coord.|X Disp.|Y Disp.|Z Disp.|X Rot .|Y Rot .|Z Rot .|Module No.\n");
	fprintf(fp,"Label|       Code|      (m)|      (m)|      (m)|  Rest.|  Rest.|  Rest.|  Rest.|  Rest.|  Rest.|      List\n");
	fprintf(fp,"-------------------------------------------------------------------------------------------------------------\n");
	long nNodeNum=Ta.Node.GetNodeNum();
	NODE_PTR *node_arr=new NODE_PTR[nNodeNum];
	i=0;
	for(CLDSNode *pNode=preprocessor.result.allnodeset.GetFirst();pNode;pNode=preprocessor.result.allnodeset.GetNext())
	{
		node_arr[i]=pNode;
		i++;
	}
	CQuickSort<NODE_PTR>::QuickSort(node_arr,nNodeNum,compare_func1);
	for(i=0;i<nNodeNum;i++)
	{
		char symmetry[20]="";
		char sDispRest[100]="";
		if(node_arr[i]->IsFatherObject())	//���ڵ�
		{
			if(node_arr[i]->GetRelativeMirObj(MIRMSG(1))>0x20)
				strcat(symmetry,"   X");
			else
				strcat(symmetry,"    ");
			if(node_arr[i]->GetRelativeMirObj(MIRMSG(2))>0x20)
				strcat(symmetry,"Y");
			else
				strcat(symmetry," ");
			if(node_arr[i]->GetRelativeMirObj(MIRMSG(3))>0x20)
				strcat(symmetry,"Z");
			else
				strcat(symmetry," ");
			if(stricmp(symmetry,"     ")==0)
				strcpy(symmetry,"      None ");
			else //if(strlen(symmetry)>0)
				strcat(symmetry,"-Symm");
		}
		else
		{
			RELATIVE_OBJECT relaObj;
			if(node_arr[i]->GetFatherMirObj(&relaObj))
			{
				if(relaObj.mirInfo.axis_flag==1)
					strcpy(symmetry,"     X-Gen ");
				else if(relaObj.mirInfo.axis_flag==2)
					strcpy(symmetry,"     Y-Gen ");
				else if(relaObj.mirInfo.axis_flag==4)
					strcpy(symmetry,"     Z-Gen ");
				else
					strcpy(symmetry,"      None ");
			}
			else
				strcpy(symmetry,"      None ");
		}
		CLDSModule *pPrevModule=NULL;
		BOOL bFirstLine=TRUE;
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			char module_no[6];
			if(!pModule->IsSonPart(node_arr[i],NULL,false))
				continue;	//�ýڵ㲻�Ǵ�ģ���еĽڵ�
			if(pPrevModule==NULL)
			{
				if(node_arr[i]->IsHasRestrict(0,X_RESTRICT))
					strcat(sDispRest,"  FIXED");
				else
					strcat(sDispRest,"   FREE");
				if(node_arr[i]->IsHasRestrict(0,Y_RESTRICT))
					strcat(sDispRest,"   FIXED");
				else
					strcat(sDispRest,"    FREE");
				if(node_arr[i]->IsHasRestrict(0,Z_RESTRICT))
					strcat(sDispRest,"   FIXED");
				else
					strcat(sDispRest,"    FREE");
				if(node_arr[i]->IsHasRestrict(0,X_ROT_RESTRICT))
					strcat(sDispRest,"   FIXED");
				else
					strcat(sDispRest,"    FREE");
				if(node_arr[i]->IsHasRestrict(0,Y_ROT_RESTRICT))
					strcat(sDispRest,"   FIXED");
				else
					strcat(sDispRest,"    FREE");
				if(node_arr[i]->IsHasRestrict(0,Z_ROT_RESTRICT))
					strcat(sDispRest,"   FIXED");
				else
					strcat(sDispRest,"    FREE");
				sprintf(module_no,"  %d",pModule->iNo);
				strcat(sDispRest,module_no);
			}
			else if(node_arr[i]->GetRestrictWord(pPrevModule->handle)!=node_arr[i]->GetRestrictWord(pModule->handle))
			{	//ǰ������ģ�ͽڵ�Լ��״����ͬ
				if(bFirstLine)
				{
					fprintf(fp,"%5d %s  %8.3f  %8.3f  %8.3f %s\n",node_arr[i]->point_i,symmetry,node_arr[i]->Position(false).x/1000,node_arr[i]->Position(false).y/1000,node_arr[i]->Position(false).z/1000,sDispRest);
					bFirstLine=FALSE;
				}
				else
					fprintf(fp,"                                                %s\n",sDispRest);
				strcpy(sDispRest,"");
				if(node_arr[i]->IsHasRestrict(0,X_RESTRICT))
					strcat(sDispRest,"  FIXED");
				else
					strcat(sDispRest,"   FREE");
				if(node_arr[i]->IsHasRestrict(0,Y_RESTRICT))
					strcat(sDispRest,"   FIXED");
				else
					strcat(sDispRest,"    FREE");
				if(node_arr[i]->IsHasRestrict(0,Z_RESTRICT))
					strcat(sDispRest,"   FIXED");
				else
					strcat(sDispRest,"    FREE");
				if(node_arr[i]->IsHasRestrict(0,X_ROT_RESTRICT))
					strcat(sDispRest,"   FIXED");
				else
					strcat(sDispRest,"    FREE");
				if(node_arr[i]->IsHasRestrict(0,Y_ROT_RESTRICT))
					strcat(sDispRest,"   FIXED");
				else
					strcat(sDispRest,"    FREE");
				if(node_arr[i]->IsHasRestrict(0,Z_ROT_RESTRICT))
					strcat(sDispRest,"   FIXED");
				else
					strcat(sDispRest,"    FREE");
				char module_no[6];
				sprintf(module_no,"  %d",pModule->iNo);
				strcat(sDispRest,module_no);
			}
			else
			{	//ǰ������ģ�ͽڵ�Լ��״����ͬ
				char module_no[6];
				sprintf(module_no,",%d",pModule->iNo);
				strcat(sDispRest,module_no);
			}
			pPrevModule=pModule;
		}
		if(bFirstLine)
		{
			fprintf(fp,"%5d %s  %8.3f  %8.3f  %8.3f %s\n",node_arr[i]->point_i,symmetry,
				node_arr[i]->Position(false).x/1000,node_arr[i]->Position(false).y/1000,node_arr[i]->Position(false).z/1000,sDispRest);
			bFirstLine=FALSE;
		}
		else
			fprintf(fp,"                                                %s\n",sDispRest);
		
	}
	delete []node_arr;
	fprintf(fp,"\n");
	//Part ��: �����б�
	fprintf(fp,"======================================================\n");
	fprintf(fp,"Part ��: Weight Bill of Tower(Cg, Inc.)\n");
	fprintf(fp,"\n");
	fprintf(fp," Working Module|Total Weight|Total Gravity\n");
	fprintf(fp,"     (Body/Leg)|        (kg)|         (kN)\n");
	fprintf(fp,"------------------------------------------------------\n");
	i=0;
	for(TOWERWEIGHT *pWeight=postprocessor.TowerWeight.GetFirst();pWeight;pWeight=postprocessor.TowerWeight.GetNext())
	{
		char csMark[20]="";
		CXhChar16 cfgStr;
		if(pWeight->iBody0Leg1==0)	//����
		{
			cfgStr.Printf("%d",pWeight->cfg_no);
			cfgStr.ResizeLength(3,' ',true);
			sprintf(csMark," %s           ",(char*)cfgStr,pWeight->cfg_no);
		}
		else						//����
		{
			CFGWORD cfgword(pWeight->cfg_no);
			cfgword.GetLegScopeStr(cfgStr,17,true);
			cfgStr.ResizeLength(4,' ',true);
			sprintf(csMark," %s          ",(char*)cfgStr);//(pWeight->cfg_no-1)/8+1,(pWeight->cfg_no-1)%8+'A');
		}
		int str_len=strlen(csMark);
		if(str_len<15)
		{
			for(int j=15;j>=0;j--)
			{
				if(j-15+str_len<0)
					csMark[j]=' ';
				else
					csMark[j]=csMark[j-15+str_len];
			}
		}
		fprintf(fp,"%s    %8.2f    %8.3f\n",csMark,pWeight->netWeight,pWeight->netWeight*0.0098);
	}
	fprintf(fp,"\n");
	//Part ��: ����������
	fprintf(fp,"===========================================================\n");
	fprintf(fp,"Part ��: Foundation Node Reaction\n");
	fprintf(fp,"\n");
	fprintf(fp," Node|    Working Phase|     Rx(kN)|     Ry(kN)|     Rz(kN)\n");
	fprintf(fp,"Label|(Case,Module,Leg)\n");
	fprintf(fp,"----------------------------------------------------------------------------------------------\n");
	if(g_sysPara.reactionExportType==1)
	{
		int nReaction=postprocessor.Reaction.GetNodeNum();
		REACTION_PTR *react_arr=new REACTION_PTR[nReaction];
		i=0;
		for(BASENODE_REACTION *pReact=postprocessor.Reaction.GetFirst();pReact;pReact=postprocessor.Reaction.GetNext())
		{
			react_arr[i]=pReact;
			i++;
		}
		//CQuickSort<REACTION_PTR>::QuickSort(react_arr,nReaction,compare_func3);
		//�˴����ÿ�������������������ֻ�ʱ����ݹ���ȳ���ת���������򣬽�������Ч�ʼ����½����������� wjh-2013-5-1
		CHeapSort<REACTION_PTR>::HeapSort(react_arr,nReaction,compare_func3);
		long prev_point_i=0,nCount=0;;
		double Rzmax=0,Rzmin=0;
		for(i=0;i<nReaction;i++)
		{
			/*if(g_sysPara.bCalStdFoundationForce)
			{
				if(react_arr[i]->reaction_force_std.z>=0)		//�ϲ���
					react_arr[i]->status = 1;
				else //if(react_arr[i]->reaction_force_std.z<0)	//��ѹ��
					react_arr[i]->status =-1;
			}
			else*/
			{
				if(react_arr[i]->reaction_force_design.z>=0)		//�ϲ���
					react_arr[i]->status = 1;
				else //if(react_arr[i]->reaction_force_design.z<0)	//��ѹ��
					react_arr[i]->status =-1;
			}
		}
		for(i=0;i<nReaction;i++)
		{
			for(int j=i+1;j<nReaction;j++)
			{
				if(react_arr[i]->pNode->point_i!=react_arr[j]->pNode->point_i)
					continue;	//����ͬһ�����ڵ�ʱ������ƥ�����ɸѡ
				if(react_arr[i]->pModule->iNo!=react_arr[j]->pModule->iNo || 
					react_arr[i]->pStatus->iNo!=react_arr[j]->pStatus->iNo)
					continue;	//����ͬһģ��ͬһ�����Ļ���������������ƥ�����ɸѡ
				/*if(g_sysPara.bCalStdFoundationForce)
				{
					if(react_arr[i]->status>0&&react_arr[j]->status>=0)
					{	//ͬ���ϲ���
						if(react_arr[i]->reaction_force_std.z<react_arr[j]->reaction_force_std.z)
						{
							react_arr[i]->status=0;	//��������ϲ���,Ϊ��ͨ�����ڵ�
							break;
						}
						else
							react_arr[j]->status = 2;	//�˽ڵ㲻���ϲ�����ֵ�����ڵ�
					}
					else if(react_arr[i]->status<0&&react_arr[j]->status<0)
					{	//ͬ����ѹ��
						if(react_arr[i]->reaction_force_std.z>react_arr[j]->reaction_force_std.z)
						{
							react_arr[i]->status=0;		//���������ѹ��,Ϊ��ͨ�����ڵ�
							break;
						}
						else
							react_arr[j]->status = -2;	//�˽ڵ㲻����ѹ����ֵ�����ڵ�
					}
				}
				else*/
				{
					if(react_arr[i]->status>0&&react_arr[j]->status>=0)
					{	//ͬ���ϲ���
						if(react_arr[i]->reaction_force_design.z<react_arr[j]->reaction_force_design.z)
						{
							react_arr[i]->status=0;	//��������ϲ���,Ϊ��ͨ�����ڵ�
							break;
						}
						else
							react_arr[j]->status = 2;	//�˽ڵ㲻���ϲ�����ֵ�����ڵ�
					}
					else if(react_arr[i]->status<0&&react_arr[j]->status<0)
					{	//ͬ����ѹ��
						if(react_arr[i]->reaction_force_design.z>react_arr[j]->reaction_force_design.z)
						{
							react_arr[i]->status=0;		//���������ѹ��,Ϊ��ͨ�����ڵ�
							break;
						}
						else
							react_arr[j]->status = -2;	//�˽ڵ㲻����ѹ����ֵ�����ڵ�
					}
				}
			}
		}
		for(i=0;i<nReaction;i++)
		{
			if(react_arr[i]->status!=1&&react_arr[i]->status!=-1)
				continue;	//���Ǽ�ֵ�����ڵ�
			char csMark[20]="";
			sprintf(csMark,"%2d,%2d,%2d-%2d-%2d-%2d",react_arr[i]->pStatus->iNo,react_arr[i]->pModule->iNo,
				react_arr[i]->leg_no[0],react_arr[i]->leg_no[1],react_arr[i]->leg_no[2],react_arr[i]->leg_no[3]);
			int str_len=strlen(csMark);
			if(str_len<15)
			{
				for(int j=15;j>=0;j--)
				{
					if(j-15+str_len<0)
						csMark[j]=' ';
					else
						csMark[j]=csMark[j-15+str_len];
				}
			}
			if(prev_point_i==0)
			{
				nCount++;
				prev_point_i=react_arr[i]->pNode->point_i;
				/*if(g_sysPara.bCalStdFoundationForce)
				{
					if(react_arr[i]->reaction_force_std.z>0)
						Rzmax=react_arr[i]->reaction_force_std.z;
					else
						Rzmin=react_arr[i]->reaction_force_std.z;
					fprintf(fp,"%5d %s    %8.3f    %8.3f    %8.3f    %8.3f    %8.3f    %8.3f\n",react_arr[i]->pNode->point_i,csMark,
						react_arr[i]->reaction_force_design.x/1000,react_arr[i]->reaction_force_design.y/1000,react_arr[i]->reaction_force_design.z/1000,
						react_arr[i]->reaction_force_std.x/1000,react_arr[i]->reaction_force_std.y/1000,react_arr[i]->reaction_force_std.z/1000);
				}
				else*/
				{
					if(react_arr[i]->reaction_force_design.z>0)
						Rzmax=react_arr[i]->reaction_force_design.z;
					else
						Rzmin=react_arr[i]->reaction_force_design.z;
					fprintf(fp,"%5d %s    %8.3f    %8.3f    %8.3f\n",react_arr[i]->pNode->point_i,csMark,
						react_arr[i]->reaction_force_design.x/1000,react_arr[i]->reaction_force_design.y/1000,react_arr[i]->reaction_force_design.z/1000);
				}
			}
			else if(prev_point_i==react_arr[i]->pNode->point_i)
			{
				nCount++;
				//prev_point_i=react_arr[i]->pNode->point_i;
				/*if(g_sysPara.bCalStdFoundationForce)
				{
					if(react_arr[i]->reaction_force_std.z>Rzmax)
						Rzmax=react_arr[i]->reaction_force_std.z;
					else if(react_arr[i]->reaction_force_std.z<Rzmin)
						Rzmin=react_arr[i]->reaction_force_std.z;
					fprintf(fp,"%5d %s    %8.3f    %8.3f    %8.3f    %8.3f    %8.3f    %8.3f\n",react_arr[i]->pNode->point_i,csMark,
						react_arr[i]->reaction_force_design.x/1000,react_arr[i]->reaction_force_design.y/1000,react_arr[i]->reaction_force_design.z/1000,
						react_arr[i]->reaction_force_std.x/1000,react_arr[i]->reaction_force_std.y/1000,react_arr[i]->reaction_force_std.z/1000);
				}
				else*/
				{
					if(react_arr[i]->reaction_force_design.z>Rzmax)
						Rzmax=react_arr[i]->reaction_force_design.z;
					else if(react_arr[i]->reaction_force_design.z<Rzmin)
						Rzmin=react_arr[i]->reaction_force_design.z;
					fprintf(fp,"%5d %s    %8.3f    %8.3f    %8.3f\n",react_arr[i]->pNode->point_i,csMark,
						react_arr[i]->reaction_force_design.x/1000,react_arr[i]->reaction_force_design.y/1000,react_arr[i]->reaction_force_design.z/1000);
				}
				if(i==nReaction-1&&nCount>2)	//���һ��������������
					fprintf(fp,"                    +Rzmax=%8.3f(kN),-Rzmax=%8.3f(kN)\n",Rzmax/1000,Rzmin/1000);
				continue;
			}
			else	//ת���ڵ�
			{
				if(nCount>2)
					fprintf(fp,"                    +Rzmax=%8.3f(kN),-Rzmax=%8.3f(kN)\n",Rzmax/1000,Rzmin/1000);
				Rzmax=Rzmin=0;
				nCount=1;
				prev_point_i=react_arr[i]->pNode->point_i;
				/*if(g_sysPara.bCalStdFoundationForce)
				{
					if(react_arr[i]->reaction_force_std.z>0)
						Rzmax=react_arr[i]->reaction_force_std.z;
					else
						Rzmin=react_arr[i]->reaction_force_std.z;
					fprintf(fp,"\n");
					fprintf(fp,"%5d %s    %8.3f    %8.3f    %8.3f    %8.3f    %8.3f    %8.3f\n",react_arr[i]->pNode->point_i,csMark,
						react_arr[i]->reaction_force_design.x/1000,react_arr[i]->reaction_force_design.y/1000,react_arr[i]->reaction_force_design.z/1000,
						react_arr[i]->reaction_force_std.x/1000,react_arr[i]->reaction_force_std.y/1000,react_arr[i]->reaction_force_std.z/1000);
				}
				else*/
				{
					if(react_arr[i]->reaction_force_design.z>0)
						Rzmax=react_arr[i]->reaction_force_design.z;
					else
						Rzmin=react_arr[i]->reaction_force_design.z;
					fprintf(fp,"\n");
					fprintf(fp,"%5d %s    %8.3f    %8.3f    %8.3f\n",react_arr[i]->pNode->point_i,csMark,
						react_arr[i]->reaction_force_design.x/1000,react_arr[i]->reaction_force_design.y/1000,react_arr[i]->reaction_force_design.z/1000);
				}
			}
		}
		delete []react_arr;
	}
	else
	{
		int nReaction=postprocessor.Reaction.GetNodeNum();
		REACTION_PTR *react_arr=new REACTION_PTR[nReaction];
		i=0;
		for(BASENODE_REACTION *pReact=postprocessor.Reaction.GetFirst();pReact;pReact=postprocessor.Reaction.GetNext())
		{
			react_arr[i]=pReact;
			i++;
		}
		CHeapSort<REACTION_PTR>::HeapSort(react_arr,nReaction,compare_func3);
		long prev_point_i=0,nCount=0;;
		double Rzmax=0,Rzmin=0;
		for(i=0;i<nReaction;i++)
		{
			char csMark[20]="";
			sprintf(csMark,"%2d,%2d,%2d-%2d-%2d-%2d",react_arr[i]->pStatus->iNo,react_arr[i]->pModule->iNo,
				react_arr[i]->leg_no[0],react_arr[i]->leg_no[1],react_arr[i]->leg_no[2],react_arr[i]->leg_no[3]);
			int str_len=strlen(csMark);
			if(str_len<15)
			{
				for(int j=15;j>=0;j--)
				{
					if(j-15+str_len<0)
						csMark[j]=' ';
					else
						csMark[j]=csMark[j-15+str_len];
				}
			}
			if(prev_point_i==0)
			{
				nCount++;
				/*if(g_sysPara.bCalStdFoundationForce)
				{
					if(react_arr[i]->reaction_force_std.z>0)
						Rzmax=react_arr[i]->reaction_force_std.z;
					else
						Rzmin=react_arr[i]->reaction_force_std.z;
					prev_point_i=react_arr[i]->pNode->point_i;
					fprintf(fp,"%5d %s    %8.3f    %8.3f    %8.3f    %8.3f    %8.3f    %8.3f\n",react_arr[i]->pNode->point_i,csMark,
						react_arr[i]->reaction_force_design.x/1000,react_arr[i]->reaction_force_design.y/1000,react_arr[i]->reaction_force_design.z/1000,
						react_arr[i]->reaction_force_std.x/1000,react_arr[i]->reaction_force_std.y/1000,react_arr[i]->reaction_force_std.z/1000);
				}
				else*/
				{
					if(react_arr[i]->reaction_force_design.z>0)
						Rzmax=react_arr[i]->reaction_force_design.z;
					else
						Rzmin=react_arr[i]->reaction_force_design.z;
					prev_point_i=react_arr[i]->pNode->point_i;
					fprintf(fp,"%5d %s    %8.3f    %8.3f    %8.3f\n",react_arr[i]->pNode->point_i,csMark,
						react_arr[i]->reaction_force_design.x/1000,react_arr[i]->reaction_force_design.y/1000,react_arr[i]->reaction_force_design.z/1000);
				}
			}
			else if(prev_point_i==react_arr[i]->pNode->point_i)
			{
				nCount++;
				/*if(g_sysPara.bCalStdFoundationForce)
				{
					if(react_arr[i]->reaction_force_std.z>Rzmax)
						Rzmax=react_arr[i]->reaction_force_std.z;
					else if(react_arr[i]->reaction_force_std.z<Rzmin)
						Rzmin=react_arr[i]->reaction_force_std.z;
					prev_point_i=react_arr[i]->pNode->point_i;
					fprintf(fp,"%5d %s    %8.3f    %8.3f    %8.3f    %8.3f    %8.3f    %8.3f\n",react_arr[i]->pNode->point_i,csMark,
						react_arr[i]->reaction_force_design.x/1000,react_arr[i]->reaction_force_design.y/1000,react_arr[i]->reaction_force_design.z/1000,
						react_arr[i]->reaction_force_std.x/1000,react_arr[i]->reaction_force_std.y/1000,react_arr[i]->reaction_force_std.z/1000);
				}
				else*/
				{
					if(react_arr[i]->reaction_force_design.z>Rzmax)
						Rzmax=react_arr[i]->reaction_force_design.z;
					else if(react_arr[i]->reaction_force_design.z<Rzmin)
						Rzmin=react_arr[i]->reaction_force_design.z;
					prev_point_i=react_arr[i]->pNode->point_i;
					fprintf(fp,"%5d %s    %8.3f    %8.3f    %8.3f\n",react_arr[i]->pNode->point_i,csMark,
						react_arr[i]->reaction_force_design.x/1000,react_arr[i]->reaction_force_design.y/1000,react_arr[i]->reaction_force_design.z/1000);
				}
				if(i==nReaction-1&&nCount>2)	//���һ��������������
					fprintf(fp,"                    +Rzmax=%8.3f(kN),-Rzmax=%8.3f(kN)\n",Rzmax/1000,Rzmin/1000);
				continue;
			}
			else	//ת���ڵ�
			{
				if(nCount>2)
					fprintf(fp,"                    +Rzmax=%8.3f(kN),-Rzmax=%8.3f(kN)\n",Rzmax/1000,Rzmin/1000);
				Rzmax=Rzmin=0;
				nCount=1;
				/*if(g_sysPara.bCalStdFoundationForce)
				{
					if(react_arr[i]->reaction_force_std.z>0)
						Rzmax=react_arr[i]->reaction_force_std.z;
					else
						Rzmin=react_arr[i]->reaction_force_std.z;
					prev_point_i=react_arr[i]->pNode->point_i;
					fprintf(fp,"\n");
					fprintf(fp,"%5d %s    %8.3f    %8.3f    %8.3f    %8.3f    %8.3f    %8.3f\n",react_arr[i]->pNode->point_i,csMark,
						react_arr[i]->reaction_force_design.x/1000,react_arr[i]->reaction_force_design.y/1000,react_arr[i]->reaction_force_design.z/1000,
						react_arr[i]->reaction_force_std.x/1000,react_arr[i]->reaction_force_std.y/1000,react_arr[i]->reaction_force_std.z/1000);
				}
				else*/
				{
					if(react_arr[i]->reaction_force_design.z>0)
						Rzmax=react_arr[i]->reaction_force_design.z;
					else
						Rzmin=react_arr[i]->reaction_force_design.z;
					prev_point_i=react_arr[i]->pNode->point_i;
					fprintf(fp,"\n");
					fprintf(fp,"%5d %s    %8.3f    %8.3f    %8.3f\n",react_arr[i]->pNode->point_i,csMark,
						react_arr[i]->reaction_force_design.x/1000,react_arr[i]->reaction_force_design.y/1000,react_arr[i]->reaction_force_design.z/1000);
				}
			}
		}
		delete []react_arr;
	}
	//Part �� & Part ��
	fprintf(fp,"\n");
	typedef CElement* ELEM_PTR;
	int nElemNum=postprocessor.hashElems.GetNodeNum();
	ELEM_PTR *elem_arr=new ELEM_PTR[nElemNum];
	i=0;
	for(CElement *pElem=postprocessor.hashElems.GetFirst();pElem;pElem=postprocessor.hashElems.GetNext())
	{
		elem_arr[i]=pElem;
		i++;
	}
	CQuickSort<ELEM_PTR>::QuickSort(elem_arr,nElemNum,compare_func2);
	//Part : �˼�������Ϣ
	fprintf(fp,"====================================================================================================\n");
	fprintf(fp,"Part ��: Member Basic Properties\n");
	fprintf(fp,"\n");
	fprintf(fp,"   Member|Member| Member|Material|  Unit|  Total| Gross| Coef.of|Radius of|Radius of|Section|   Bolt|  Bear\n");
	fprintf(fp,"    Label|  Type|   Size|  & Size|Weight| Length|  Area|Hole_Out| Gyration| Gyration|Modulus|  Info.|  Load\n");
	fprintf(fp,"         |      |   (mm)|    Code|(kg/m)|    (m)| (cm2)|        |   Rx(cm)|  Ry0(cm)|Wx(cm3)|       |  Coef\n");
	fprintf(fp,"---------------------------------------------------------------------------------------------------------\n");
	
	for(i=0;i<nElemNum;i++)
	{
		long No[2];
		No[0]=min(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		No[1]=max(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		
		if(elem_arr[i]->pOrgPole->size_idClassType==0)
		{
			fprintf(fp,"%4d-%4d      U  %3.0fX%2.0f        %8.3f%8.3f %6.2f\n",No[0],No[1],
			elem_arr[i]->pOrgPole->GetWidth(),elem_arr[i]->pOrgPole->GetThick(),
			elem_arr[i]->pOrgPole->userdef.unit_weight,elem_arr[i]->Length()/1000,
			elem_arr[i]->pOrgPole->userdef.area/100);
		}
		else
		{
			CXhChar16 ss_ls;
			if(elem_arr[i]->pStart==elem_arr[i]->pOrgPole->pStart)	//ʼ�˵�Ԫ
				ss_ls.Printf("%sM%dX%d %5.3f",elem_arr[i]->pOrgPole->connectStart.grade,elem_arr[i]->pOrgPole->connectStart.d,elem_arr[i]->pOrgPole->connectStart.wnSummBoltN,elem_arr[i]->pOrgPole->connectStart.bearloadcoef);
			else if(elem_arr[i]->pEnd==elem_arr[i]->pOrgPole->pEnd)	//ĩ�˵�Ԫ
				ss_ls.Printf("%sM%dX%d %5.3f",elem_arr[i]->pOrgPole->connectEnd.grade,elem_arr[i]->pOrgPole->connectEnd.d,elem_arr[i]->pOrgPole->connectEnd.wnSummBoltN,elem_arr[i]->pOrgPole->connectEnd.bearloadcoef);
			else
				ss_ls.Printf("Middle Elem.");
			double Rx=elem_arr[i]->pOrgPole->GetWorkR(0)/10;
			double Ry0=elem_arr[i]->pOrgPole->GetWorkR(1)/10;
			double Wx=elem_arr[i]->pOrgPole->GetWorkW(0)/1000;
			double area=elem_arr[i]->pOrgPole->GetArea()/100;
			double length=elem_arr[i]->Length()/1000;	// ��λm
			double theroy_weight=globalLibrary.GetWeightOf1Meter(
				elem_arr[i]->pOrgPole->size_wide,elem_arr[i]->pOrgPole->size_thick,0,
				elem_arr[i]->pOrgPole->size_idClassType,elem_arr[i]->pOrgPole->size_cSubClassType);
			if(elem_arr[i]->pOrgPole->size_idClassType==0)
			{
				fprintf(fp,"%4d-%4d      %s %3.0fX%2.0f\n",No[0],No[1],
					elem_arr[i]->pOrgPole->GetBriefPartTypeMark(),elem_arr[i]->pOrgPole->GetWidth(),elem_arr[i]->pOrgPole->GetThick());
			}
			else if(strlen(elem_arr[i]->pOrgPole->GetBriefPartTypeMark())==1)
			{
				fprintf(fp,"%4d-%4d      %s %3.0fX%2.0f%C   %4d %8.3f%8.3f %6.2f   %6.2f  %8.3f  %8.3f%8.3f %s\n",No[0],No[1],
					elem_arr[i]->pOrgPole->GetBriefPartTypeMark(),elem_arr[i]->pOrgPole->GetWidth(),elem_arr[i]->pOrgPole->GetThick(),
					elem_arr[i]->pOrgPole->cMaterial,elem_arr[i]->pOrgPole->m_uStatMatNo,
					theroy_weight,length,area,elem_arr[i]->design_info.hole_out,Rx,Ry0,Wx,(char*)ss_ls);
			}
			else
			{
				fprintf(fp,"%4d-%4d     %s %3.0fX%2.0f%C   %4d %8.3f%8.3f %6.2f   %6.2f  %8.3f  %8.3f%8.3f %s\n",No[0],No[1],
					elem_arr[i]->pOrgPole->GetBriefPartTypeMark(),elem_arr[i]->pOrgPole->GetWidth(),elem_arr[i]->pOrgPole->GetThick(),
					elem_arr[i]->pOrgPole->cMaterial,elem_arr[i]->pOrgPole->m_uStatMatNo,
					theroy_weight,length,area,elem_arr[i]->design_info.hole_out,Rx,Ry0,Wx,(char*)ss_ls);
			}
		}
	}
	fprintf(fp,"\n");
	fprintf(fp,"===================================================================================================================================================\n");
	fprintf(fp,"Part ��: Member Design Information\n");
	fprintf(fp,"\n");
	fprintf(fp,"   Member| Control|   Max.|    Working Phase|    Max.|    Working Phase|Coef. of| Coef. of| Coef.|  L/r|K*Lo/r|Ulti-| Coef.of|  Begin|    End|  Begin|    End\n");
	fprintf(fp,"    Label| Criter-|  Tens.|(Case,Module,Leg)|   Comp.|(Case,Module,Leg)|Strength|Stability|   of | Sel.|      | mate|   Bear |   Rot.|   Rot.|  Force|  Force\n");
	fprintf(fp,"         |     ion|  (Mpa)|                 |   (Mpa)|                 |Decrease|     (��)|   mN |Style|      | Lo/r|   Load |  Rest.|  Rest.|   Type|   Type\n");
	fprintf(fp,"---------------------------------------------------------------------------------------------------------------------------------------------------\n");
	for(i=0;i<nElemNum;i++)
	{
		long No[2];
		No[0]=min(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		No[1]=max(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		CXhChar50 ss;
		if(elem_arr[i]->m_iCtrlRule==1)
			ss.Copy("  K*Lo/r");
		else if(elem_arr[i]->m_iCtrlRule==2)
			ss.Copy("   TENS.");
		else if(elem_arr[i]->m_iCtrlRule==3)
			ss.Copy("   COMP.");
		else if(elem_arr[i]->m_iCtrlRule==4)
			ss.Copy("   STAB.");
		else if(elem_arr[i]->m_iCtrlRule==5)
			ss.Copy("M&S.Code");
		else if(elem_arr[i]->m_iCtrlRule==6)
			ss.Copy("    BOLT");
		else //if(elem_arr[i]->m_iCtrlRule==0)
			ss.Copy("    NONE");
		if(rpt_type!=0&&elem_arr[i]->m_iCtrlRule>0
			&&elem_arr[i]->m_iCtrlRule!=5)
			ss[0]='>';
		CString sRx_Ry0,s1,s2,s3,s4,s5,s6;
		CLDSModule *pModule=Ta.Module.FromHandle(elem_arr[i]->MaxTension.hModule);
		CWorkStatus *pStatus=Ta.WorkStatus.FromHandle(elem_arr[i]->MaxTension.hWorkStatus.Id());
		if(pModule&&pStatus)
		{
			s1.Format("%2d,%2d,%2d-%2d-%2d-%2d",pStatus->iNo,pModule->iNo,
				elem_arr[i]->MaxTension.leg_no[0],elem_arr[i]->MaxTension.leg_no[1],
				elem_arr[i]->MaxTension.leg_no[2],elem_arr[i]->MaxTension.leg_no[3]);
		}
		else
			s1.Format(" 0, 0, 0- 0- 0- 0");
		pModule=Ta.Module.FromHandle(elem_arr[i]->MaxCompression.hModule);
		pStatus=Ta.WorkStatus.FromHandle(elem_arr[i]->MaxCompression.hWorkStatus.Id());
		if(pModule&&pStatus)
		{
			s2.Format("%2d,%2d,%2d-%2d-%2d-%2d",pStatus->iNo,pModule->iNo,
				elem_arr[i]->MaxCompression.leg_no[0],elem_arr[i]->MaxCompression.leg_no[1],
				elem_arr[i]->MaxCompression.leg_no[2],elem_arr[i]->MaxCompression.leg_no[3]);
		}
		else
			s2.Format(" 0, 0, 0- 0- 0- 0");
		if(elem_arr[i]->LamdaRType()==0)
			sRx_Ry0="Rx ";
		else
			sRx_Ry0="Ryo";
		if(elem_arr[i]->design_info.bStartRotRest)
			s3="PARTIAL";
		else
			s3="   FREE";
		if(elem_arr[i]->design_info.bEndRotRest)
			s4="PARTIAL";
		else
			s4="   FREE";
		if(elem_arr[i]->start_force_type==CENTRIC_FORCE)
			s5="CENTRIC";
		else
			s5=" ECCEN.";
		if(elem_arr[i]->end_force_type==CENTRIC_FORCE)
			s6="CENTRIC";
		else
			s6=" ECCEN.";
		double An=elem_arr[i]->pOrgPole->GetArea()-elem_arr[i]->design_info.hole_out*elem_arr[i]->design_info.hole_d*elem_arr[i]->pOrgPole->GetThick();
		char caserule[3];
		if(elem_arr[i]->design_info.iDangerousRule==1)
			strcpy(caserule,"��");
		else if(elem_arr[i]->design_info.iDangerousRule==2)
			strcpy(caserule," T");
		else if(elem_arr[i]->design_info.iDangerousRule==3)
			strcpy(caserule," C");
		else if(elem_arr[i]->design_info.iDangerousRule==4)
			strcpy(caserule," S");
		else
			strcpy(caserule,"  ");
		double coefStrength=1.0;
		if(pElem->design_info.iDangerousLoadCase==2)		//T
			coefStrength=pElem->design_info.fCoefTensStrengthDecrease;
		else if(pElem->design_info.iDangerousLoadCase==3)	//C
			coefStrength=pElem->design_info.fCoefCompStrengthDecrease;
		else //if(pElem->design_info.iDangerousLoadCase==4)	//S
			coefStrength=pElem->design_info.mN;
		if(No[0]==elem_arr[i]->pStart->point_i)
			fprintf(fp,"%4d-%4d %s%8.3f %s %8.3f %s %4.2f/%4.2f %8.3f %6.2f   %s %5.0f%7.0f %5.3f@%s %s %s %s %s\n",No[0],No[1],(char*)ss,
			elem_arr[i]->MaxTension.N/An,s1,elem_arr[i]->MaxCompression.N/An,s2,
			elem_arr[i]->design_info.fCoefTensStrengthDecrease,elem_arr[i]->design_info.fCoefCompStrengthDecrease,
			elem_arr[i]->design_info.fCoefStablity,elem_arr[i]->design_info.mN,sRx_Ry0,
			elem_arr[i]->Lamda(),elem_arr[i]->design_info.UltimateLo_r,elem_arr[i]->design_info.fCoefEffic,caserule,s3,s4,s5,s6);
		else
			fprintf(fp,"%4d-%4d %s%8.3f %s %8.3f %s %4.2f/%4.2f %8.3f %6.2f   %s %5.0f%7.0f %5.3f@%s %s %s %s %s\n",No[0],No[1],(char*)ss,
			elem_arr[i]->MaxTension.N/An,s1,elem_arr[i]->MaxCompression.N/An,s2,
			elem_arr[i]->design_info.fCoefTensStrengthDecrease,elem_arr[i]->design_info.fCoefCompStrengthDecrease,
			elem_arr[i]->design_info.fCoefStablity,elem_arr[i]->design_info.mN,sRx_Ry0,
			elem_arr[i]->Lamda(),elem_arr[i]->design_info.UltimateLo_r,elem_arr[i]->design_info.fCoefEffic,caserule,s4,s3,s6,s5);
	}
	fclose(fp);
	delete []elem_arr;
	
	pCmdLine->PromptStrInCmdLine("Ӣ�İ���ϸ�����ļ������ɣ�");
}
void CreateDetailRptTxtFileFromReportByCN(char *file_name)
{
	CFileDialog dlg(FALSE,"rpt",file_name,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"��ѡ�������ļ�(*.rpt)|*.rpt|����������ļ�(*.db2)|*.db2||");
	if(dlg.DoModal()!=IDOK)
		return;
	int i,rpt_type;
	if(dlg.GetFileExt().CompareNoCase("db2")==0)
		rpt_type=1;
	else
		rpt_type=2;
	//char file_path[MAX_PATH];
	//sprintf(file_path,"%s",dlg.GetPathName());
	FILE *fp=fopen(dlg.GetPathName(),"wt");
	if(fp==NULL)
		return;
	CLDSModule *pModule;
	CWorkStatus *pStatus;
	CHashTable<long>hashModuleNo,hashStatusNo;
	hashModuleNo.CreateHashTable(Ta.Module.GetNodeNum());
	hashStatusNo.CreateHashTable(Ta.WorkStatus.GetNodeNum());
	for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		hashModuleNo.SetValueAt(pModule->handle,pModule->iNo);
	for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
		hashStatusNo.SetValueAt(pStatus->handle,pStatus->iNo);
	fprintf(fp,"\n");
	fprintf(fp,"***********************************************************************************************\n");
	fprintf(fp,"*                                                                                             *\n");
	fprintf(fp,"*            �����������������  - (C) 2002-2016 �����ź��������Ƽ����޹�˾                 *\n");
	fprintf(fp,"*                                                                                             *\n");
	fprintf(fp,"***********************************************************************************************\n");
	fprintf(fp,"���̴��� : %s\n",Ta.m_sPrjCode);
	fprintf(fp,"�������� : %s\n",Ta.m_sPrjName);
	fprintf(fp,"�����Ա : %s\n",Ta.m_sOperator);
	CTime t=CTime::GetCurrentTime();
	fprintf(fp,"����ʱ�� : %s\n",t.Format("%H:%M:%S %B %d, %Y"));
	CString product_version;
	theApp.GetProductVersion(product_version);
#ifdef __LDS_
	fprintf(fp,"����汾 : LDS Version %s\n",product_version);
#else
	fprintf(fp,"����汾 : TSA Version %s\n",product_version);
#endif
	fprintf(fp,"��Ȩ�û� : %s\n",Ta.m_sCompanyName);
	fprintf(fp,"\n");
	if(g_sysPara.AnalysisPara.m_iNonlinearStyle==0)
		fprintf(fp,"�ɹ����������Է���!\n");
	else
		fprintf(fp,"�ɹ������˷����Է���!\n");
	fprintf(fp,"\n");
	fprintf(fp,"===============================================================================================\n");
	fprintf(fp,"�ڢ񲿷�:��ƹ�̣��涨��\n");
	IMemberDesignCode* pCode=IMemberDesignCode::FromCodeSerial(postreport.m_iMemberSizeCheckCode);
	fprintf(fp,"��ִ�е���ƹ�̣��涨��Ϊ:%s��\n",pCode->CodeName());
	fprintf(fp,"\n");

	fprintf(fp,"==================================================================================================\n");
	fprintf(fp,"�ڢ򲿷�: �ֲĻ�е����\n\n");
	
	fprintf(fp,"ֱ������ ��16 mm, ѡ�� \"��1��\"��\n");
	fprintf(fp,"ֱ������ ��16~40 mm ����Q235, ѡ�� \"�ڶ���\"��\n");
	fprintf(fp,"ֱ������ ��16~35 mm ����Q345��Q390, ѡ�� \"�ڶ���\"��\n");
	fprintf(fp,"ֱ������ ��40~60 mm ����Q235, ѡ�� \"������\"��\n");
	fprintf(fp,"ֱ������ ��35~50 mm ����Q345��Q390, ѡ�� \"������\"��\n");
	fprintf(fp,"\n");
	fprintf(fp,"    �ֲ�|   �ֲ�|        ����|       ���|       ���|      ���|      ����|       ����|      ����\n");
	fprintf(fp,"        |   ��|            |       ǿ��|       ǿ��|      ǿ��|      ǿ��|       ǿ��|      ǿ��\n");
	fprintf(fp,"    �ƺ�|   �ƺ�|        ģ��|      ��1��|      ��2��|     ��3��|     ��1��|      ��2��|     ��3��\n");
	fprintf(fp,"        |       |       (MPa)|      (MPa)|      (MPa)|      (MPa)|     (MPa)|     (MPa)|     (MPa)\n");
	fprintf(fp,"--------------------------------------------------------------------------------------------------\n");
	for(i=0;i<CSteelMatLibrary::GetCount();i++)
	{
		STEELMAT mat = CSteelMatLibrary::RecordAt(i);
		fprintf(fp,"    %s     '%C'   %10.2f  %10.2f  %10.2f  %10.2f %10.2f %10.2f %10.2f\n",
			mat.mark,mat.cBriefMark,mat.E,mat.intensity[4],mat.intensity[5],mat.intensity[6],
			mat.intensity[8],mat.intensity[9],mat.intensity[10]);
	}
	fprintf(fp,"\n");
	fprintf(fp,"===============================================================================================\n");
	fprintf(fp,"�ڢ󲿷�: ��������б�\n\n");
	bool bHasAngle=false,bHasTube=false;
	for(CElement *pElem=postreport.hashElems.GetFirst();pElem;pElem=postreport.hashElems.GetNext())
	{
		if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE||pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
			bHasAngle=true;
		else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
			bHasTube=true;
		if(bHasAngle&&bHasTube)
			break;
	}
	fprintf(fp,"���| ��  �� | �� �� |��С��|ƽ����|�� Բ| ��  λ�����| ��  �� | �� �� |��С��|ƽ����|�� Բ| ��  λ\n");
	fprintf(fp,"    |        | �� �� |�뾶  |�뾶  |�� ��| ��  ����    |        | �� �� |�뾶  |�뾶  |�� ��| ��  ��\n");
	fprintf(fp,"    |        | (cm2) |(cm)  |(cm)  |(cm) | (kg/m)��    |        | (cm2) |(cm)  |(cm)  |(cm) | (kg/m)\n");
	fprintf(fp,"---------------------------------------------------------------------------------------------------------\n");
	if(bHasAngle)
	{
		fprintf(fp,"�Ǹֹ��⣺\n");
		for(i=0;i<jgguige_N;)
		{
			char stype[3]=" L";
			if(toupper(jgguige_table[i].cType)!='L')
				sprintf(stype,"%CL",jgguige_table[i].cType);
			fprintf(fp," %3d %s%3.0f*%2.0f %7.3f %6.2f %6.2f %5.2f %7.3f",i+1,stype,jgguige_table[i].wing_wide,jgguige_table[i].wing_thick,
				jgguige_table[i].area,jgguige_table[i].Ry0,jgguige_table[i].Rx,jgguige_table[i].r,jgguige_table[i].theroy_weight);
			i++;
			if(toupper(jgguige_table[i].cType)!='L')
				sprintf(stype,"%CL",jgguige_table[i].cType);
			else
				strcpy(stype," L");
			if(i<jgguige_N)
				fprintf(fp,"�� %3d %s%3.0f*%2.0f %7.3f %6.2f %6.2f %5.2f %7.3f\n",i+1,stype,jgguige_table[i].wing_wide,jgguige_table[i].wing_thick,
				jgguige_table[i].area,jgguige_table[i].Ry0,jgguige_table[i].Rx,jgguige_table[i].r,jgguige_table[i].theroy_weight);
			else
				fprintf(fp,"��\n");
			i++;
		}
	}
	if(bHasTube)
	{
		fprintf(fp,"�ֹܹ��⣺\n");
		for(i=0;i<tubeguige_N;)
		{
			char stype[3]="��";
			fprintf(fp," %3d %s%3.0f*%2.0f %7.3f %6.2f              %7.3f",i+1,stype,tubeguige_table[i].D,tubeguige_table[i].thick,
				tubeguige_table[i].area,tubeguige_table[i].r,tubeguige_table[i].theroy_weight);
			i++;
			if(i<tubeguige_N)
				fprintf(fp,"�� %3d %s%3.0f*%2.0f %7.3f %6.2f              %7.3f\n",i+1,stype,tubeguige_table[i].D,tubeguige_table[i].thick,
				tubeguige_table[i].area,tubeguige_table[i].r,tubeguige_table[i].theroy_weight);
			else
				fprintf(fp,"��\n");
			i++;
		}
	}

	fprintf(fp,"================================\n");
	fprintf(fp,"�ڢ�����: ��˨��е����\n");
	fprintf(fp,"\n");
	fprintf(fp," ��˨|    ����|    ����|�ױڳ�ѹ\n");
	fprintf(fp," ����|    ǿ��|    ǿ��|    ǿ��\n");
	fprintf(fp,"     |   (MPa)|   (MPa)|   (MPa)\n");
	fprintf(fp,"--------------------------------\n");
	for(i=0;i<LsTypeCount;i++)
	{
		if(i<LsTypeCount)
		{
			if(strlen(ls_table[i].grade)==3)
				fprintf(fp,"  %s %8.2f %8.2f %8.2f\n",ls_table[i].grade,ls_table[i].fv,ls_table[i].f,ls_table[i].fc);
			else	//10.9����˨
				fprintf(fp," %s %8.2f %8.2f %8.2f\n",ls_table[i].grade,ls_table[i].fv,ls_table[i].f,ls_table[i].fc);
		}
	}
	fprintf(fp,"\n");
	fprintf(fp,"=======================================================================================================\n");
	fprintf(fp,"�ڢ�����: �ڵ㼸����Ϣ\n");
	fprintf(fp,"\n");
	fprintf(fp," �ڵ�|�Գ�| ����X����|����Y����|����Z����|X��λ��|Y��λ��|Z��λ��|X��ת��|Y��ת��|Z��ת��| ���ߺ�\n");
	fprintf(fp," ���|��Ϣ|       (m)|      (m)|      (m)|   Լ��|   Լ��|   Լ��|   Լ��|   Լ��|   Լ��|   �б�\n");
	fprintf(fp,"-------------------------------------------------------------------------------------------------------\n");
	for(POSTRPT_NODE *pRptNode=postreport.hashNodes.GetFirst();pRptNode;pRptNode=postreport.hashNodes.GetNext())
	{
		char symmetry[20]="";
		char sDispRest[100]="";
		if(g_sysPara.bMergeRptFileMirObj&&!pRptNode->bFatherNode)
			continue;	//�ϲ������Գ���ʱ���ӶԳ����ʾ���
		if(pRptNode->pNode->IsHasRestrict(0,X_RESTRICT))
			strcat(sDispRest,"   �̶�");
		else
			strcat(sDispRest,"   ����");
		if(pRptNode->pNode->IsHasRestrict(0,Y_RESTRICT))
			strcat(sDispRest,"    �̶�");
		else
			strcat(sDispRest,"    ����");
		if(pRptNode->pNode->IsHasRestrict(0,Z_RESTRICT))
			strcat(sDispRest,"    �̶�");
		else
			strcat(sDispRest,"    ����");
		if(pRptNode->pNode->IsHasRestrict(0,X_ROT_RESTRICT))
			strcat(sDispRest,"    �̶�");
		else
			strcat(sDispRest,"    ����");
		if(pRptNode->pNode->IsHasRestrict(0,Y_ROT_RESTRICT))
			strcat(sDispRest,"    �̶�");
		else
			strcat(sDispRest,"    ����");
		if(pRptNode->pNode->IsHasRestrict(0,Z_ROT_RESTRICT))
			strcat(sDispRest,"    �̶�");
		else
			strcat(sDispRest,"    ����");
		
		CXhChar50 moduleStr;
		if(pRptNode->pNode->IsLegObj())
			pRptNode->pNode->cfgword.GetLegScopeStr(moduleStr,51,true);
		else
			pRptNode->pNode->cfgword.GetBodyScopeStr(moduleStr,51);
		strcat(sDispRest,"  ");
		strcat(sDispRest,moduleStr);
		fprintf(fp,"%5d %s  %8.3f  %8.3f  %8.3f %s\n",pRptNode->pNode->point_i,(char*)pRptNode->symmetry,
			pRptNode->pNode->Position(false).x/1000,pRptNode->pNode->Position(false).y/1000,pRptNode->pNode->Position(false).z/1000,sDispRest);
	}
	fprintf(fp,"\n");
	//Part ��: �����б�
	fprintf(fp,"======================================================\n");
	fprintf(fp,"�ڢ�����: �����嵥\n");
	fprintf(fp,"\n");
	fprintf(fp,"           ����|      ������|       ������|    ������ \n");
	fprintf(fp,"    ����/���Ⱥ�|        (kg)|         (kN)|������ϵ��(kg)\n");
	fprintf(fp,"------------------------------------------------------\n");
	i=0;
	for(TOWERWEIGHT *pWeight=postreport.TowerWeight.GetFirst();pWeight;pWeight=postreport.TowerWeight.GetNext())
	{
		char csMark[20]="";
		CXhChar16 cfgStr;
		if(pWeight->iBody0Leg1==0)	//����
		{
			cfgStr.Printf("%d",pWeight->cfg_no);
			cfgStr.ResizeLength(3,' ',true);
			sprintf(csMark," %s           ",(char*)cfgStr,pWeight->cfg_no);
		}
		else						//����
		{
			CFGWORD cfgword(pWeight->cfg_no);
			cfgword.GetLegScopeStr(cfgStr,17,true);
			cfgStr.ResizeLength(4,' ',true);
			sprintf(csMark," %s          ",(char*)cfgStr);//(pWeight->cfg_no-1)/8+1,(pWeight->cfg_no-1)%8+'A');
		}
		int str_len=strlen(csMark);
		if(str_len<15)
		{
			for(int j=15;j>=0;j--)
			{
				if(j-15+str_len<0)
					csMark[j]=' ';
				else
					csMark[j]=csMark[j-15+str_len];
			}
		}
		fprintf(fp,"%s    %8.2f    %8.3f    %8.3f\n",csMark,pWeight->netWeight,pWeight->netWeight*0.0098,pWeight->weight);
	}
	fprintf(fp,"\n");
	//Part ��: ����������
	if(postreport.m_bHasStdFoundationForce)
		fprintf(fp,"================================================================================================\n");
	else
		fprintf(fp,"============================================================\n");
	fprintf(fp,"�ڢ�����: �����ڵ㷴������\n");
	fprintf(fp,"\n");
	//if(postreport.m_bHasStdFoundationForce)
	//{
	//	//fprintf(fp," �ڵ�|          ����״̬|     Rx(kN)|     Ry(kN)|     Rz(kN)|     Rx(kN)|     Ry(kN)|     Rz(kN)\n");
	//	//fprintf(fp," ���|����,������,���Ⱥ�|     ���ֵ|     ���ֵ|     ���ֵ|     ��׼ֵ|     ��׼ֵ|     ��׼ֵ\n");
	//	fprintf(fp," �ڵ�|          ����״̬|     Rx(kN)|     Ry(kN)|     Rz(kN)\n");//
	//	fprintf(fp," ���|����,������,���Ⱥ�|     ��׼ֵ|     ��׼ֵ|     ��׼ֵ\n");
	//	fprintf(fp,"------------------------------------------------------------------------------------------------\n");
	//}
	//else
	{
		fprintf(fp," �ڵ�|          ����״̬|     Rx(kN)|     Ry(kN)|     Rz(kN)\n");
		fprintf(fp," ���|����,������,���Ⱥ�|\n");//     ���ֵ|     ���ֵ|     ���ֵ\n");
		fprintf(fp,"------------------------------------------------------------\n");
	}
	int nReaction=postreport.Reaction.GetNodeNum();
	REACTION_PTR *react_arr=new REACTION_PTR[nReaction];
	i=0;
	for(BASENODE_REACTION *pReact=postreport.Reaction.GetFirst();pReact;pReact=postreport.Reaction.GetNext())
	{
		pReact->status=0;
		react_arr[i]=pReact;
		i++;
	}
	//CQuickSort<REACTION_PTR>::QuickSort(react_arr,nReaction,compare_func3);
	//�˴����ÿ�������������������ֻ�ʱ����ݹ���ȳ���ת���������򣬽�������Ч�ʼ����½����������� wjh-2013-5-1
	CHeapSort<REACTION_PTR>::HeapSort(react_arr,nReaction,compare_func3);
	long prev_point_i=0,nCount=0;
	double Rzmax=0,Rzmin=0;
	ARRAY_LIST<REACTION_PTR>exportReactArr;
	exportReactArr.SetSize(0,10);
	long iRzmax=0,iRzmin=0;
	for(i=0;i<nReaction;i++)
	{
		if(g_sysPara.reactionExportType==0)
		{
			exportReactArr.append(react_arr[i]);
			continue;
		}
		if(i==0)
			iRzmin=iRzmax=0;
		else //if(i>0)
		{
			
			if( g_sysPara.reactionExportType==1&&(react_arr[i-1]->pNode->point_i==react_arr[i]->pNode->point_i)&&
				react_arr[i-1]->pModule==react_arr[i]->pModule&&react_arr[i-1]->pStatus==react_arr[i]->pStatus)
			{	//�ߵ��ȼ�ɸѡ
				//if(postreport.m_bHasStdFoundationForce&&react_arr[i]->reaction_force_std.z>react_arr[iRzmax]->reaction_force_std.z)
				//	iRzmax=i;
				//else if(!postreport.m_bHasStdFoundationForce&&
				if(react_arr[i]->reaction_force_design.z>react_arr[iRzmax]->reaction_force_design.z)
					iRzmax=i;
				//if(postreport.m_bHasStdFoundationForce&&react_arr[i]->reaction_force_std.z<react_arr[iRzmin]->reaction_force_std.z)
				//	iRzmin=i;
				//else if(!postreport.m_bHasStdFoundationForce&&
				if(react_arr[i]->reaction_force_design.z<react_arr[iRzmin]->reaction_force_design.z)
					iRzmin=i;
			}
			else if(g_sysPara.reactionExportType==2&&(react_arr[i-1]->pNode->point_i==react_arr[i]->pNode->point_i)&&
				react_arr[i-1]->pModule==react_arr[i]->pModule)
			{	//������ɸѡ
				//if(postreport.m_bHasStdFoundationForce&&react_arr[i]->reaction_force_std.z>react_arr[iRzmax]->reaction_force_std.z)
				//	iRzmax=i;
				//else if(!postreport.m_bHasStdFoundationForce&&
				if(react_arr[i]->reaction_force_design.z>react_arr[iRzmax]->reaction_force_design.z)
					iRzmax=i;
				//if(postreport.m_bHasStdFoundationForce&&react_arr[i]->reaction_force_std.z<react_arr[iRzmin]->reaction_force_std.z)
				//	iRzmin=i;
				//else if(!postreport.m_bHasStdFoundationForce&&
				if(react_arr[i]->reaction_force_design.z<react_arr[iRzmin]->reaction_force_design.z)
					iRzmin=i;
			}
			else
			{	//�ֻ�
				if(iRzmin<iRzmax)
				{
					//if((postreport.m_bHasStdFoundationForce&&react_arr[iRzmin]->reaction_force_std.z<0)||
					//  (!postreport.m_bHasStdFoundationForce&&react_arr[iRzmin]->reaction_force_design.z<0))
					if(react_arr[iRzmin]->reaction_force_design.z<0)
						exportReactArr.append(react_arr[iRzmin]);
					//if((postreport.m_bHasStdFoundationForce&&react_arr[iRzmax]->reaction_force_std.z>0)||
					// (!postreport.m_bHasStdFoundationForce&&react_arr[iRzmax]->reaction_force_design.z>0))
					if(react_arr[iRzmax]->reaction_force_design.z>0)
						exportReactArr.append(react_arr[iRzmax]);
				}
				else if(iRzmin>iRzmax)
				{
					//if((postreport.m_bHasStdFoundationForce&&react_arr[iRzmax]->reaction_force_std.z>0)||
					//  (!postreport.m_bHasStdFoundationForce&&react_arr[iRzmax]->reaction_force_design.z>0))
					if(react_arr[iRzmax]->reaction_force_design.z>0)
						exportReactArr.append(react_arr[iRzmax]);
					//if((postreport.m_bHasStdFoundationForce&&react_arr[iRzmin]->reaction_force_std.z<0)||
					//(!postreport.m_bHasStdFoundationForce&&react_arr[iRzmin]->reaction_force_design.z<0))
					if(react_arr[iRzmin]->reaction_force_design.z<0)
						exportReactArr.append(react_arr[iRzmin]);
				}
				else
					exportReactArr.append(react_arr[iRzmin]);
				iRzmin=iRzmax=i;
			}
		}
		if(i==nReaction-1)
		{	//���һ����¼ʱ����
			if(iRzmin<iRzmax)
			{
				//if((postreport.m_bHasStdFoundationForce&&react_arr[iRzmin]->reaction_force_std.z<0)||
				//  (!postreport.m_bHasStdFoundationForce&&react_arr[iRzmin]->reaction_force_design.z<0))
				if(react_arr[iRzmin]->reaction_force_design.z<0)
					exportReactArr.append(react_arr[iRzmin]);
				//if((postreport.m_bHasStdFoundationForce&&react_arr[iRzmax]->reaction_force_std.z>0)||
				//  (!postreport.m_bHasStdFoundationForce&&react_arr[iRzmax]->reaction_force_design.z>0))
				if(react_arr[iRzmax]->reaction_force_design.z>0)
					exportReactArr.append(react_arr[iRzmax]);
			}
			else if(iRzmin>iRzmax)
			{
				//if((postreport.m_bHasStdFoundationForce&&react_arr[iRzmax]->reaction_force_std.z>0)||
				//  (!postreport.m_bHasStdFoundationForce&&react_arr[iRzmax]->reaction_force_design.z>0))
				if(react_arr[iRzmax]->reaction_force_design.z>0)
					exportReactArr.append(react_arr[iRzmax]);
				//if((postreport.m_bHasStdFoundationForce&&react_arr[iRzmin]->reaction_force_std.z<0)||
				//  (!postreport.m_bHasStdFoundationForce&&react_arr[iRzmin]->reaction_force_design.z<0))
				if(react_arr[iRzmin]->reaction_force_design.z<0)
					exportReactArr.append(react_arr[iRzmin]);
			}
			else
				exportReactArr.append(react_arr[iRzmin]);
		}
	}
	delete []react_arr;
	for(i=0;i<exportReactArr.GetSize();i++)
	{
		char quadLeg[4]={0};
		CXhChar50 csMark;
		for(int k=0;k<4;k++)
			quadLeg[k]=(exportReactArr[i]->leg_no[k]-1)%8+'A';
		csMark.Printf("%2d,%3d,%C-%C-%C-%C ",exportReactArr[i]->pModule->iNo,exportReactArr[i]->pStatus->iNo,
			quadLeg[0],quadLeg[1],quadLeg[2],quadLeg[3]);
		csMark.ResizeLength(19,' ',true);
		if(prev_point_i==0)
		{
			nCount++;
			/*
			if(postreport.m_bHasStdFoundationForce)
			{
				if(exportReactArr[i]->reaction_force_std.z>0)
					Rzmax=exportReactArr[i]->reaction_force_std.z;
				else
					Rzmin=exportReactArr[i]->reaction_force_std.z;
				prev_point_i=exportReactArr[i]->pNode->point_i;
				fprintf(fp,"%5d %s  %9.3f   %9.3f   %9.3f   %9.3f   %9.3f   %9.3f\n",exportReactArr[i]->pNode->point_i,(char*)csMark,
					exportReactArr[i]->reaction_force_design.x/1000,exportReactArr[i]->reaction_force_design.y/1000,exportReactArr[i]->reaction_force_design.z/1000,
					exportReactArr[i]->reaction_force_std.x/1000,exportReactArr[i]->reaction_force_std.y/1000,exportReactArr[i]->reaction_force_std.z/1000);
			}
			else
			*/
			{
				if(exportReactArr[i]->reaction_force_design.z>0)
					Rzmax=exportReactArr[i]->reaction_force_design.z;
				else
					Rzmin=exportReactArr[i]->reaction_force_design.z;
				prev_point_i=exportReactArr[i]->pNode->point_i;
				fprintf(fp,"%5d %s  %9.3f   %9.3f   %9.3f\n",exportReactArr[i]->pNode->point_i,(char*)csMark,
					exportReactArr[i]->reaction_force_design.x/1000,exportReactArr[i]->reaction_force_design.y/1000,exportReactArr[i]->reaction_force_design.z/1000);
			}
		}
		else if(prev_point_i==exportReactArr[i]->pNode->point_i)
		{
			nCount++;
			/*
			if(postreport.m_bHasStdFoundationForce)
			{
				if(exportReactArr[i]->reaction_force_std.z>Rzmax)
					Rzmax=exportReactArr[i]->reaction_force_std.z;
				else if(exportReactArr[i]->reaction_force_std.z<Rzmin)
					Rzmin=exportReactArr[i]->reaction_force_std.z;
				prev_point_i=exportReactArr[i]->pNode->point_i;
				fprintf(fp,"      %s  %9.3f   %9.3f   %9.3f   %9.3f   %9.3f   %9.3f\n",(char*)csMark,
					exportReactArr[i]->reaction_force_design.x/1000,exportReactArr[i]->reaction_force_design.y/1000,exportReactArr[i]->reaction_force_design.z/1000,
					exportReactArr[i]->reaction_force_std.x/1000,exportReactArr[i]->reaction_force_std.y/1000,exportReactArr[i]->reaction_force_std.z/1000);
			}
			else
			*/
			{
				if(exportReactArr[i]->reaction_force_design.z>Rzmax)
					Rzmax=exportReactArr[i]->reaction_force_design.z;
				else if(exportReactArr[i]->reaction_force_design.z<Rzmin)
					Rzmin=exportReactArr[i]->reaction_force_design.z;
				prev_point_i=exportReactArr[i]->pNode->point_i;
				fprintf(fp,"      %s  %9.3f   %9.3f   %9.3f\n",(char*)csMark,
					exportReactArr[i]->reaction_force_design.x/1000,exportReactArr[i]->reaction_force_design.y/1000,exportReactArr[i]->reaction_force_design.z/1000);
			}
			if(i==exportReactArr.GetSize()-1&&nCount>=2)	//���һ��������������
				fprintf(fp,"                    +Rzmax=%8.3f(kN),-Rzmax=%8.3f(kN)\n",Rzmax/1000,Rzmin/1000);
			continue;
		}
		else	//ת���ڵ�
		{
			if(nCount>=2)
				fprintf(fp,"                    +Rzmax=%8.3f(kN),-Rzmax=%8.3f(kN)\n",Rzmax/1000,Rzmin/1000);
			Rzmax=Rzmin=0;
			nCount=1;
			prev_point_i=exportReactArr[i]->pNode->point_i;
			/*
			if(postreport.m_bHasStdFoundationForce)
			{
				if(exportReactArr[i]->reaction_force_std.z>0)
					Rzmax=exportReactArr[i]->reaction_force_std.z;
				else
					Rzmin=exportReactArr[i]->reaction_force_std.z;
				fprintf(fp,"\n");
				fprintf(fp,"%5d %s  %9.3f   %9.3f   %9.3f   %9.3f   %9.3f   %9.3f\n",exportReactArr[i]->pNode->point_i,(char*)csMark,
					exportReactArr[i]->reaction_force_design.x/1000,exportReactArr[i]->reaction_force_design.y/1000,exportReactArr[i]->reaction_force_design.z/1000,
					exportReactArr[i]->reaction_force_std.x/1000,exportReactArr[i]->reaction_force_std.y/1000,exportReactArr[i]->reaction_force_std.z/1000);
			}
			else
			*/
			{
				if(exportReactArr[i]->reaction_force_design.z>0)
					Rzmax=exportReactArr[i]->reaction_force_design.z;
				else
					Rzmin=exportReactArr[i]->reaction_force_design.z;
				fprintf(fp,"\n");
				fprintf(fp,"%5d %s  %9.3f   %9.3f   %9.3f\n",exportReactArr[i]->pNode->point_i,(char*)csMark,
					exportReactArr[i]->reaction_force_design.x/1000,exportReactArr[i]->reaction_force_design.y/1000,exportReactArr[i]->reaction_force_design.z/1000);
			}
		}
	}
	//Part �� & Part ��
	fprintf(fp,"\n");
	int nElemNum=postreport.hashElems.GetNodeNum();
	ELEM_PTR *elem_arr=new ELEM_PTR[nElemNum];
	i=0;
	for(pElem=postreport.hashElems.GetFirst();pElem;pElem=postreport.hashElems.GetNext())
	{
		elem_arr[i]=pElem;
		i++;
	}
	CQuickSort<ELEM_PTR>::QuickSort(elem_arr,nElemNum,compare_func2);
	//Part : �˼�������Ϣ
	//fprintf(fp,"===========================================================================================================\n");
	//fprintf(fp,"�ڢ�����: ��Ԫ���������Ϣ\n");
	//fprintf(fp,"\n");
	//fprintf(fp,"     ��Ԫ|  ��Ԫ|   ��Ԫ|  ͳ�ĺ�|  ��λ|     ��|  ë��|    ��˨|   ƽ����|   ��С��| �����|   ��˨|  ��˨\n");
	//fprintf(fp,"     ��ʶ|  ����|   ���|        |  ����|   ����|  ���|  ������| ��ת�뾶| ��ת�뾶| ��ģ��|   ��Ϣ|  ����\n");
	//fprintf(fp,"         |      |   (mm)|        |(kg/m)|    (m)| (cm2)|        |   Rx(cm)|  Ry0(cm)|Wx(cm3)|       |  ϵ��\n");
	//fprintf(fp,"-----------------------------------------------------------------------------------------------------------\n");
	fprintf(fp,"==================================================================================================================\n");
	fprintf(fp,"�ڢ�����: ��Ԫ���������Ϣ\n");
	fprintf(fp,"\n");
	fprintf(fp,"     ��Ԫ|      �Գ�|��Ԫ|��Ԫ�� |ͳ��| ����|�� ��|    ��˨  | ����| �� | ����|     ���㳤��       | G.R.| ��ϸ�� \n");
	fprintf(fp,"     ��ʶ|      ��Ϣ|����|��(mm) | �� | ����|״ ��|    ��Ϣ  | ϵ��|����| (mm)|       (mm)         | (mm)|���㹫ʽ\n");
	fprintf(fp,"------------------------------------------------------------------------------------------------------------------\n");

	for(i=0;i<nElemNum;i++)
	{
		long No[2];
		//��Ԫ��ʶ
		bool bReverse=elem_arr[i]->pStart->point_i>elem_arr[i]->pEnd->point_i;
		No[0]=min(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		No[1]=max(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		//�Գ���Ϣ
		char symmetry[20]="";
		if(elem_arr[i]->relaElem.bFatherElem)//pOrgPole->IsFatherObject())	//���˼�
		{
			if(elem_arr[i]->relaElem.idElemMirX>0)
				strcat(symmetry,"  X");
			else
				strcat(symmetry,"   ");
			if(elem_arr[i]->relaElem.idElemMirY>0)
				strcat(symmetry,"Y");
			else
				strcat(symmetry," ");
			if(elem_arr[i]->relaElem.idElemMirZ>0)
				strcat(symmetry,"Z");
			else
				strcat(symmetry," ");
			if(stricmp(symmetry,"     ")==0)
				strcpy(symmetry,"        ��");
			else //if(strlen(symmetry)>0)
				strcat(symmetry,"-�Գ�");
		}
		else
		{
			long idFather=preprocessor.GetFatherElemId(elem_arr[i]);
			if(idFather>0&&idFather==elem_arr[i]->relaElem.idElemMirX)
				strcpy(symmetry,"X-�Գ�����");
			else if(idFather>0&&idFather==elem_arr[i]->relaElem.idElemMirY)
				strcpy(symmetry,"Y-�Գ�����");
			else if(idFather>0&&idFather==elem_arr[i]->relaElem.idElemMirZ)
				strcpy(symmetry,"Z-�Գ�����");
			else
				strcpy(symmetry,"        ��");
			/*RELATIVE_OBJECT relaObj;
			if(elem_arr[i]->pOrgPole->GetFatherMirObj(&relaObj))
			{
				if(relaObj.mirInfo.axis_flag==1)
					strcpy(symmetry,"X-�Գ�����");
				else if(relaObj.mirInfo.axis_flag==2)
					strcpy(symmetry,"Y-�Գ�����");
				else if(relaObj.mirInfo.axis_flag==4)
					strcpy(symmetry,"Z-�Գ�����");
				else
					strcpy(symmetry,"        ��");
			}
			else
				strcpy(symmetry,"        ��");*/
		}
		//��Ԫ����
		char elemtype[5]="";
		_snprintf(&elemtype[2],2,"%s",elem_arr[i]->pOrgPole->GetBriefPartTypeMark());
		elemtype[0]=elem_arr[i]->pOrgPole->layer(1);
		elemtype[1]='^';
		if(strlen(elemtype)==3)
		{	//��λ���뵥�Ǹֵ�Ԫ����
			elemtype[3]=elemtype[2];
			elemtype[2]=' ';	
		}
		//ͳ�ĺ�
		char statmatno[5]="";
		if(elem_arr[i]->pOrgPole->m_uStatMatNo==0)
			strcpy(statmatno,"    ");
		else
			_snprintf(statmatno,4,"%4d",elem_arr[i]->pOrgPole->m_uStatMatNo);
		//��������
		char jointtype[6]="";
		if(elem_arr[i]->real_start_joint_type==JOINT_HINGE)
			strcpy(jointtype,"��-");
		else
			strcpy(jointtype,"��-");
		if(elem_arr[i]->real_end_joint_type==JOINT_HINGE)
			strcat(jointtype,"��");
		else
			strcat(jointtype,"��");
		//�˶�״��
		char forcetype[6]="";
		if(elem_arr[i]->design_info.bStartRotRest)
			forcetype[0]='P';
		else
			forcetype[0]='F';
		if(elem_arr[i]->design_info.bEndRotRest)
			forcetype[1]='P';
		else
			forcetype[1]='F';
		forcetype[2]='^';
		if(elem_arr[i]->start_force_type==CENTRIC_FORCE)
			forcetype[3]='C';
		else
			forcetype[3]='E';
		if(elem_arr[i]->end_force_type==CENTRIC_FORCE)
			forcetype[4]='C';
		else
			forcetype[4]='E';
		if(bReverse)
		{
			char ch=jointtype[0];
			jointtype[0]=jointtype[3];
			jointtype[3]=ch;
			ch=jointtype[1];
			jointtype[1]=jointtype[4];
			jointtype[4]=ch;

			ch=forcetype[0];
			forcetype[0]=forcetype[1];
			forcetype[1]=ch;
			ch=forcetype[3];
			forcetype[3]=forcetype[4];
			forcetype[4]=ch;
		}
		CXhChar16 ss_ls;
		/*if(elem_arr[i]->pOrgPole->guige.pole_type==TRUSS_POLE_USERDEF)
		{
		fprintf(fp,"%4d-%4d      U  %3.0fX%2.0f        %8.3f%8.3f %6.2f\n",No[0],No[1],
		elem_arr[i]->pOrgPole->GetWidth(),elem_arr[i]->pOrgPole->GetThick(),
		elem_arr[i]->pOrgPole->userdef.unit_weight,elem_arr[i]->GetLength()/1000,
		elem_arr[i]->pOrgPole->userdef.area/100);
		}*/
		if(elem_arr[i]->pStart==elem_arr[i]->pOrgPole->pStart)	//ʼ�˵�Ԫ
			ss_ls.Printf("%sM%2dX%2d %5.3f",elem_arr[i]->pOrgPole->connectStart.grade,elem_arr[i]->pOrgPole->connectStart.d,elem_arr[i]->pOrgPole->connectStart.wnSummBoltN,elem_arr[i]->pOrgPole->connectStart.bearloadcoef);
		else if(elem_arr[i]->pEnd==elem_arr[i]->pOrgPole->pEnd)	//ĩ�˵�Ԫ
			ss_ls.Printf("%sM%2dX%2d %5.3f",elem_arr[i]->pOrgPole->connectEnd.grade,elem_arr[i]->pOrgPole->connectEnd.d,elem_arr[i]->pOrgPole->connectEnd.wnSummBoltN,elem_arr[i]->pOrgPole->connectEnd.bearloadcoef);
		else
			ss_ls.Printf("Middle Elem.   ");
		CXhChar50 size_spec;//[200]="";
		if(elem_arr[i]->pOrgPole->size_idClassType==CLS_CABLE)
			size_spec.Copy(elem_arr[i]->pOrgPole->GetSpec());
		else
			size_spec.Printf("%3.0fX%2.0f%C",elem_arr[i]->pOrgPole->GetWidth(),
			elem_arr[i]->pOrgPole->GetThick(),elem_arr[i]->pOrgPole->cMaterial);
		fprintf(fp,"%4d-%4d %s %s %s %s %s %s  %s %4.2f %5.0f%s=%4.0f %5.1f %s\n",
			No[0],No[1],symmetry,elemtype,(char*)size_spec,statmatno,jointtype,forcetype,(char*)ss_ls,
			elem_arr[i]->design_info.hole_out,elem_arr[i]->Length(),
			(char*)elem_arr[i]->design_info.LoExpression,elem_arr[i]->LamdaL(),elem_arr[i]->LamdaR(),(char*)elem_arr[i]->design_info.lamdaExpression);
	}
	fprintf(fp,"\n");
	if(rpt_type!=0)
	{
		//fprintf(fp,"=======================================================================================================================================================================================\n");
		//fprintf(fp,"�ڢ�����: ��Ԫ��ϸ�����Ϣ\n");
		//fprintf(fp,"\n");
		//fprintf(fp,"     ��Ԫ|    ����|   ���|          ����״̬|    ���|          ����״̬|   ǿ��  |��ѹ��|ѹ����|   ���㳤��(mm)     |   ��ϸ��      | ���|����ϵ��|   ʼ��|   �ն�|   ʼ��|   �ն�\n");
		//fprintf(fp,"     ��ʶ|    ����| ��Ӧ��|(����,ģ��,���Ⱥ�)|  ѹӦ��|(����,ģ��,���Ⱥ�)|   �ۼ�  |����ϵ|��ϵ��|                    |    ����       | ����|        |   ת��|   ת��|   ����|   ����\n");
		//fprintf(fp,"         |        |  (Mpa)|                  |   (Mpa)|                  |   ϵ��  |��(��)| (mN) |                    |  �����       | Lo/r|        |   Լ��|   Լ��|   ����|   ����\n");
		//fprintf(fp,"---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		fprintf(fp,"=====================================================================================================================\n");
		fprintf(fp,"�ڢ�����: ��Ԫ��ϸ�����Ϣ\n");
		fprintf(fp,"\n");
		fprintf(fp,"     ��Ԫ|    ����|��Ԫ|  ��Ԫ |  ���| ���� |  ���| ���� |  ���|������| ��ѹ��|ǿ����|    |����|��  |���|����ϵ��\n");
		fprintf(fp,"     ��ʶ|    ����|����|  ��� |  ����|  -   |  ѹ��|  -   |  Ӧ��|��Ӧ��| ����ϵ|��ϵ��|����|����|(K*)|����|        \n");
		fprintf(fp,"         |        |    |  (mm) |  (kN)| ���� |  (kN)| ���� | (MPa)|(MPa) | ��(��)|      |(mm)|(mm)|Lo/r|Lo/r|        \n");
		fprintf(fp,"---------------------------------------------------------------------------------------------------------------------\n");
	}
	else
	{
		//fprintf(fp,"==============================================================================================================================================================================\n");
		//fprintf(fp,"�ڢ�����: ��Ԫ��ϸ�����Ϣ\n");
		//fprintf(fp,"\n");
		//fprintf(fp,"     ��Ԫ|    ���|          ����״̬|    ���|          ����״̬|   ǿ��  |��ѹ��|ѹ����|   ���㳤��(mm)     |   ��ϸ��      | ���|����ϵ��|   ʼ��|   �ն�|   ʼ��|   �ն�\n");
		//fprintf(fp,"     ��ʶ|  ��Ӧ��|(����,ģ��,���Ⱥ�)|  ѹӦ��|(����,ģ��,���Ⱥ�)|   �ۼ�  |����ϵ|��ϵ��|                    |    ����       | ����|        |   ת��|   ת��|   ����|   ����\n");
		//fprintf(fp,"         |   (Mpa)|                  |   (Mpa)|                  |   ϵ��  |��(��)| (mN) |                    |  �����       | Lo/r|        |   Լ��|   Լ��|   ����|   ����\n");
		//fprintf(fp,"------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		fprintf(fp,"=======================================================================================================================\n");
		fprintf(fp,"�ڢ�����: ��Ԫ��ϸ�����Ϣ\n");
		fprintf(fp,"\n");
		if(g_sysPara.iEfficacyCeofType==0)
			fprintf(fp,"     ��Ԫ|��Ԫ|  ��Ԫ |  ���| ���� |   ���| ���� |  ����|������|��ѹ��| ǿ����|    |����|��  |���|���ԣ��\n");
		else
			fprintf(fp,"     ��Ԫ|��Ԫ|  ��Ԫ |  ���| ���� |   ���| ���� |  ����|������|��ѹ��| ǿ����|    |����|��  |���|����ϵ��\n");
		fprintf(fp,"     ��ʶ|����|  ��� |  ����|  -   |   ѹ��|  -   |  Ӧ��|��Ӧ��|����ϵ| ��ϵ��|����|����|(K*)|����|        \n");
		fprintf(fp,"         |    |  (mm) |  (kN)| ���� |   (kN)| ���� | (MPa)|(MPa) |��(��)|       |(mm)|(mm)|Lo/r|Lo/r|        \n");
		fprintf(fp,"-------------------------------------------------------------------------------------------------------------\n");
	}
	for(i=0;i<nElemNum;i++)
	{
		long No[2];
		No[0]=min(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		No[1]=max(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		double L=elem_arr[i]->Length();
		char ss[12];
		if(elem_arr[i]->m_iCtrlRule==1)
			strcpy(ss,"  K*Lo/r");
		else if(elem_arr[i]->m_iCtrlRule==2)
			strcpy(ss,"   TENS.");
		else if(elem_arr[i]->m_iCtrlRule==3)
			strcpy(ss,"   COMP.");
		else if(elem_arr[i]->m_iCtrlRule==4)
			strcpy(ss,"   STAB.");
		else if(elem_arr[i]->m_iCtrlRule==5)
			strcpy(ss,"M&S.Code");
		else if(elem_arr[i]->m_iCtrlRule==6)
			strcpy(ss,"    BOLT");
		else //if(elem_arr[i]->pOrgPole->m_iCtrlRule==0)
			strcpy(ss,"    NONE");
		if(rpt_type!=0&&elem_arr[i]->m_iCtrlRule>0
			&&elem_arr[i]->m_iCtrlRule!=5)
			ss[0]='>';
		//��Ԫ����
		char elemtype[5]="";
		_snprintf(&elemtype[2],2,"%s",elem_arr[i]->pOrgPole->GetBriefPartTypeMark());
		elemtype[0]=elem_arr[i]->pOrgPole->layer(1);
		elemtype[1]='^';
		if(strlen(elemtype)==3)
		{	//��λ���뵥�Ǹֵ�Ԫ����
			elemtype[3]=elemtype[2];
			elemtype[2]=' ';	
		}
		CString s1,s2,s3,s4,s5,s6,stable_s;
		long module_no=0,status_no=0;
		hashModuleNo.GetValueAt(elem_arr[i]->MaxTension.hModule,module_no);
		hashStatusNo.GetValueAt(elem_arr[i]->MaxTension.hWorkStatus.Id(),status_no);
		SUBID caseno=SUBID(status_no,elem_arr[i]->MaxTension.hWorkStatus.Group());
		if(elem_arr[i]->MaxTension.hWorkStatus.Group()>0)
			s1.Format("%2d-%s",module_no,(char*)caseno.ToString('@',false,3));
		else
		s1.Format("%2d-%3d",module_no,status_no);
		module_no=status_no=0;	//����
		hashModuleNo.GetValueAt(elem_arr[i]->MaxCompression.hModule,module_no);
		hashStatusNo.GetValueAt(elem_arr[i]->MaxCompression.hWorkStatus.Id(),status_no);
		if(elem_arr[i]->MaxCompression.hWorkStatus.Group()>0)
		{
			caseno=SUBID(status_no,elem_arr[i]->MaxCompression.hWorkStatus.Group());
			s2.Format("%2d-%s",module_no,(char*)caseno.ToString('@',false,3));
		}
		else
		s2.Format("%2d-%3d",module_no,status_no);
		if(elem_arr[i]->MaxCompression.hWorkStatus!=elem_arr[i]->MaxCompStable.hWorkStatus&&elem_arr[i]->MaxCompStable.hWorkStatus.Id()>0&&
			fabs(elem_arr[i]->MaxCompStable.N-elem_arr[i]->MaxCompression.N)>10&&elem_arr[i]->m_iCtrlRule==4)
		{
			status_no=0;
			hashStatusNo.GetValueAt(elem_arr[i]->MaxCompStable.hWorkStatus.Id(),status_no);
			if(elem_arr[i]->MaxCompStable.hWorkStatus.Group()>0)
			{
				caseno=SUBID(status_no,elem_arr[i]->MaxCompStable.hWorkStatus.Group());
				stable_s.Format("                                              %7.2f %2d-%s Max Compression Stability Force",elem_arr[i]->MaxCompStable.N*0.001,module_no,(char*)caseno.ToString('@',false,3));
			}
			else
				stable_s.Format("                                              %7.2f %2d-%3d Max Compression Stability Force",elem_arr[i]->MaxCompStable.N*0.001,module_no,status_no);
		}
		CXhChar16 caserule;
		double ctrlstress=0;
		double coefStrength=1.0;
		double A=elem_arr[i]->pOrgPole->GetArea();
		double An=A-elem_arr[i]->design_info.hole_out*elem_arr[i]->design_info.hole_d*elem_arr[i]->pOrgPole->GetThick();
		STEELMAT *pSteelMat=QuerySteelMatByBriefMark(elem_arr[i]->pOrgPole->cMaterial);
		if(elem_arr[i]->design_info.iDangerousLoadCase==2)		//T
		{
			coefStrength=elem_arr[i]->design_info.fCoefTensStrengthDecrease;
			ctrlstress=elem_arr[i]->MaxTension.N/An;
		}
		else if(elem_arr[i]->design_info.iDangerousLoadCase==3)	//C
		{
			coefStrength=elem_arr[i]->design_info.fCoefCompStrengthDecrease;
			ctrlstress=elem_arr[i]->MaxCompression.N/An;
		}
		else //if(elem_arr[i]->design_info.iDangerousLoadCase==4)	//S
		{
			coefStrength=elem_arr[i]->design_info.mN;
			if(elem_arr[i]->MaxCompression.N>-EPS||elem_arr[i]->design_info.fCoefStablity<EPS)
				ctrlstress=0;
			else	//�������ѹ��Ӧ�����ֹ�����һ�������ѹ��������һ�£��ʲ��������ѹ������ѹ��Ӧ�����ƹ����µ�ѹ��ϵ�� wjh-2013.10.15
				ctrlstress=elem_arr[i]->MaxCompStable.N/(A*elem_arr[i]->design_info.fCoefStablity);
		}
		double limStrength=pSteelMat->f(elem_arr[i]->GetThick())*coefStrength;
		if(g_sysPara.iEfficacyCeofType==0)
		{
			if(elem_arr[i]->design_info.iDangerousRule==1)
				caserule.Printf("%5.3f@��",elem_arr[i]->design_info.fCoefEffic);
			else if(elem_arr[i]->design_info.iDangerousRule==2)
			{
				if(elem_arr[i]->design_info.bBlockShearActivated)
					caserule.Printf("%5.3f@+T",elem_arr[i]->design_info.fCoefEffic);
				else
					caserule.Printf("%5.3f@ T",elem_arr[i]->design_info.fCoefEffic);
			}
			else if(elem_arr[i]->design_info.iDangerousRule==3)
				caserule.Printf("%5.3f@ C",elem_arr[i]->design_info.fCoefEffic);
			else if(elem_arr[i]->design_info.iDangerousRule==4)
			{
				if(elem_arr[i]->MaxCompStable.bothComp)
					caserule.Printf("%5.3f@+S",elem_arr[i]->design_info.fCoefEffic);
				else
					caserule.Printf("%5.3f@ S",elem_arr[i]->design_info.fCoefEffic);
			}
			else
				caserule.Printf("%5.3f@ 0",elem_arr[i]->design_info.fCoefEffic);
		}
		else
		{
			double effic=0;
			if(coefStrength>EPS)
				effic=fabs(ctrlstress)/limStrength;
			if(elem_arr[i]->design_info.iDangerousLoadCase==2)		//T
			{
				if(elem_arr[i]->design_info.bBlockShearActivated)
					caserule.Printf("%5.3f@+T",effic);
				else
					caserule.Printf("%5.3f@ T",effic);
			}
			else if(elem_arr[i]->design_info.iDangerousLoadCase==3)	//C
				caserule.Printf("%5.3f@ C",effic);
			else if(elem_arr[i]->design_info.iDangerousLoadCase==4)	//S
			{
				if(elem_arr[i]->MaxCompStable.bothComp)
					caserule.Printf("%5.3f@+S",effic);
				else
					caserule.Printf("%5.3f@ S",effic);
			}
			else	//���Ҳ���ܲ������������治�ÿ�
				caserule.Printf("0.000@ S");
		}
		char maxCompForceSymbol=' ';
		if(elem_arr[i]->MaxCompStable.N<elem_arr[i]->MaxCompression.N)
			maxCompForceSymbol='>';
		char lamda_s[10];
		//if(elem_arr[i]->LamdaMax()>elem_arr[i]->Lamda())
		//	_snprintf(lamda_s,9,">%3.0f",elem_arr[i]->Lamda());
		//else
			_snprintf(lamda_s,9," %3.0f",elem_arr[i]->Lamda());
		CXhChar200 size_spec="";
		if(elem_arr[i]->pOrgPole->size_idClassType==CLS_CABLE)
			size_spec.Copy(elem_arr[i]->pOrgPole->GetSpec());
		else
			size_spec.Printf("%3.0fX%2.0f%C",elem_arr[i]->pOrgPole->GetWidth(),elem_arr[i]->pOrgPole->GetThick(),elem_arr[i]->pOrgPole->cMaterial);
		if(rpt_type!=0)
		{
			fprintf(fp,"%4d-%4d %s %s %s%7.2f %s%C%7.2f %s %6.1f %6.1f %6.3f %6.3f %4.0f %4.0f %s %4.0f %s\n",
				No[0],No[1],ss,elemtype,(char*)size_spec,
				elem_arr[i]->MaxTension.N/1000,s1,maxCompForceSymbol,elem_arr[i]->MaxCompression.N/1000,s2,ctrlstress,limStrength,
				elem_arr[i]->design_info.fCoefStablity,coefStrength,L,elem_arr[i]->LamdaL(),lamda_s,
				elem_arr[i]->design_info.UltimateLo_r,(char*)caserule);
		}
		else
		{
			fprintf(fp,"%4d-%4d %s %s%7.2f %s%C%7.2f %s %6.1f %6.1f %6.3f %6.3f %4.0f %4.0f %4.0f %4.0f %s\n",No[0],No[1],
				elemtype,(char*)size_spec,
				elem_arr[i]->MaxTension.N/1000,s1,maxCompForceSymbol,elem_arr[i]->MaxCompression.N/1000,s2,ctrlstress,limStrength,
				elem_arr[i]->design_info.fCoefStablity,coefStrength,L,elem_arr[i]->LamdaL(),elem_arr[i]->Lamda(),
				elem_arr[i]->design_info.UltimateLo_r,(char*)caserule);
		}
		if(stable_s.GetLength()>0)
			fprintf(fp,"%s\n",stable_s);
	}
	//Part ��
	fprintf(fp,"=========================================================\n");
	fprintf(fp,"�ڢ�����: ����Ԫ������Ϣ\n\n");
	fprintf(fp,"     ��Ԫ|��Ԫ|  ��Ԫ | ���� |  �� ��|�����|����Ӧ|\n");
	fprintf(fp,"     ��ʶ|����|  ��� |  -   |  �� ��|��Ӧ��|��ռ��|\n");
	fprintf(fp,"         |    |  (mm) | ���� |  (N*m)| (MPa)| (%%)  |\n");
	fprintf(fp,"---------------------------------------------------------\n");
	for(i=0;i<nElemNum;i++)
	{
		long No[2];
		if(elem_arr[i]->MaxMoment.N<=0)
			continue;	//������Ԫ����������
		double bend_stress=0;	//��λMPa
		double ctrlstress=0;	//�����ѹӦ��(MPa)
		double bend_percent=0;
		double W=elem_arr[i]->GetWorkW(0);
		if(W>0)
			bend_stress=elem_arr[i]->MaxMoment.N/W;
		double A=elem_arr[i]->pOrgPole->GetArea();
		double An=A-elem_arr[i]->design_info.hole_out*elem_arr[i]->design_info.hole_d*elem_arr[i]->pOrgPole->GetThick();
		if(elem_arr[i]->design_info.iDangerousLoadCase==2)		//T
			ctrlstress=elem_arr[i]->MaxTension.N/An;
		else if(elem_arr[i]->design_info.iDangerousLoadCase==3)	//C
			ctrlstress=elem_arr[i]->MaxCompression.N/An;
		else //if(elem_arr[i]->design_info.iDangerousLoadCase==4)	//S
		{
			if(elem_arr[i]->MaxCompression.N>-EPS||elem_arr[i]->design_info.fCoefStablity<EPS)
				ctrlstress=0;
			else
				ctrlstress=elem_arr[i]->MaxCompression.N/A;
		}
		if(fabs(ctrlstress)>0)
			bend_percent=fabs(100*bend_stress/ctrlstress);
		No[0]=min(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		No[1]=max(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		//��Ԫ����
		char elemtype[5]="";
		_snprintf(&elemtype[2],2,"%s",elem_arr[i]->pOrgPole->GetBriefPartTypeMark());
		elemtype[0]=elem_arr[i]->pOrgPole->layer(1);
		elemtype[1]='^';
		if(strlen(elemtype)==3)
		{	//��λ���뵥�Ǹֵ�Ԫ����
			elemtype[3]=elemtype[2];
			elemtype[2]=' ';	
		}
		long module_no=0,status_no=0;
		hashModuleNo.GetValueAt(elem_arr[i]->MaxMoment.hModule,module_no);
		hashStatusNo.GetValueAt(elem_arr[i]->MaxMoment.hWorkStatus.Id(),status_no);
		CString s1;
		s1.Format("%2d-%3d",module_no,status_no);
		fprintf(fp,"%4d-%4d %s %3.0fX%2.0f%C %s%8.2f%7.2f %3.0f%%\n",No[0],No[1],
			elemtype,elem_arr[i]->pOrgPole->GetWidth(),elem_arr[i]->pOrgPole->GetThick(),elem_arr[i]->pOrgPole->cMaterial,s1,
			elem_arr[i]->MaxMoment.N/1000,bend_stress,bend_percent);
	}
	//Part ��
	//fprintf(fp,"=========================================================\n");
	//fprintf(fp,"�ڢ�����: ��Ԫ���ƹ����ߵ����ֻ���Ϣ\n\n");
	//fprintf(fp,"     ��Ԫ|   �������״̬   |   ���ѹ��״̬   |\n");
	//fprintf(fp,"     ��ʶ|������,����,���Ⱥ�|������,����,���Ⱥ�|\n");
	//fprintf(fp,"---------------------------------------------------------\n");
	//fprintf(fp,"  10-  12  31, 1, 1- 1- 1- 1   1, 1, 1- 1- 1- 1 \n");
	fclose(fp);
	delete []elem_arr;	
	WinExec(CXhChar200("notepad.exe %s",dlg.GetPathName()),SW_SHOW);
}
#ifdef __TIMER_COUNT_
#include "TimerCount.h"
extern CTimerCount timer;
#endif
void CreateDetailRptTxtFileByCN(char *file_name,CFGWORD cfgwordBody,CFGWORD cfgwordLeg,int rpt_type)
{
	CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	int i;
	char file_path[MAX_PATH],ext[5],filter[MAX_PATH];
	strcpy(file_path,file_name);
	if(rpt_type==1)
		strcpy(ext,".db2");
	else if(rpt_type==2)
		strcpy(ext,".eqk");
	else
		strcpy(ext,".rpt");
	strcat(file_path,ext);
	CFileFind fileFind;
	if(fileFind.FindFile(file_path))
	{
		sprintf(filter,"������(*%s)|*%s||",ext,ext);
		CFileDialog savedlg(FALSE,ext,file_path,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filter);
		if(savedlg.DoModal()!=IDOK)
			return;
		sprintf(file_path,"%s",savedlg.GetFileName());
	}

	FILE *fp=fopen(file_path,"wt");
	if(fp==NULL)
	{
		pCmdLine->PromptStrInCmdLine("Report�ļ�����ʧ�ܣ�");
		return;
	}
	CLDSModule *pModule;
	CWorkStatus *pStatus;
	CHashTable<long>hashModuleNo,hashStatusNo;
	hashModuleNo.CreateHashTable(Ta.Module.GetNodeNum());
	hashStatusNo.CreateHashTable(Ta.WorkStatus.GetNodeNum());
	for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		hashModuleNo.SetValueAt(pModule->handle,pModule->iNo);
	for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
		hashStatusNo.SetValueAt(pStatus->handle,pStatus->iNo);
	fprintf(fp,"\n");
	fprintf(fp,"***********************************************************************************************\n");
	fprintf(fp,"*                                                                                             *\n");
	fprintf(fp,"*            �����������������  - (C) 2002-2016 �����ź��������Ƽ����޹�˾                 *\n");
	fprintf(fp,"*                                                                                             *\n");
	fprintf(fp,"***********************************************************************************************\n");
	fprintf(fp,"���̴��� : %s\n",Ta.m_sPrjCode);
	fprintf(fp,"�������� : %s\n",Ta.m_sPrjName);
	fprintf(fp,"�����Ա : %s\n",Ta.m_sOperator);
	CTime t=CTime::GetCurrentTime();
#ifdef __TIMER_COUNT_
	fprintf(fp,"����ʱ�� : %s����ʱ%.2fs\n",t.Format("%H:%M:%S %B %d, %Y"),0.001*(timer.GetEndTicks()-timer.GetStartTicks()));
#else
	fprintf(fp,"����ʱ�� : %s\n",t.Format("%H:%M:%S %B %d, %Y"));
#endif
	CString product_version;
	theApp.GetProductVersion(product_version);
#ifdef __LDS_
	fprintf(fp,"����汾 : LDS Version %s\n",product_version);
#else
	fprintf(fp,"����汾 : TSA Version %s\n",product_version);
#endif
	fprintf(fp,"��Ȩ�û� : %s\n",Ta.m_sCompanyName);
	fprintf(fp,"\n");
	if(g_sysPara.AnalysisPara.m_iNonlinearStyle==0)
		fprintf(fp,"�ɹ����������Է���!\n");
	else
		fprintf(fp,"�ɹ������˷����Է���!\n");
	fprintf(fp,"\n");
	fprintf(fp,"===============================================================================================\n");
	fprintf(fp,"�ڢ񲿷�:��ƹ�̣��涨��\n");
	CXhChar200 code_name;
	Ta.DesignCode()->GetSupportCodesStr(code_name,201);
	fprintf(fp,"��ִ�е���ƹ�̣��涨��Ϊ:%s��\n",Ta.DesignCode()->CodeName());
	fprintf(fp,"\n");

	fprintf(fp,"==================================================================================================\n");
	fprintf(fp,"�ڢ򲿷�: �ֲĻ�е����\n\n");
	
	fprintf(fp,"ֱ������ ��16 mm, ѡ�� \"��1��\"��\n");
	fprintf(fp,"ֱ������ ��16~40 mm ����Q235, ѡ�� \"�ڶ���\"��\n");
	fprintf(fp,"ֱ������ ��16~35 mm ����Q345��Q390, ѡ�� \"�ڶ���\"��\n");
	fprintf(fp,"ֱ������ ��40~60 mm ����Q235, ѡ�� \"������\"��\n");
	fprintf(fp,"ֱ������ ��35~50 mm ����Q345��Q390, ѡ�� \"������\"��\n");
	fprintf(fp,"\n");
	fprintf(fp,"    �ֲ�|   �ֲ�|        ����|       ���|       ���|      ���|      ����|       ����|      ����\n");
	fprintf(fp,"        |   ��|            |       ǿ��|       ǿ��|      ǿ��|      ǿ��|       ǿ��|      ǿ��\n");
	fprintf(fp,"    �ƺ�|   �ƺ�|        ģ��|      ��1��|      ��2��|     ��3��|     ��1��|      ��2��|     ��3��\n");
	fprintf(fp,"        |       |       (MPa)|      (MPa)|      (MPa)|      (MPa)|     (MPa)|     (MPa)|     (MPa)\n");
	fprintf(fp,"--------------------------------------------------------------------------------------------------\n");
	for(i=0;i<CSteelMatLibrary::GetCount();i++)
	{
		STEELMAT mat = CSteelMatLibrary::RecordAt(i);
		fprintf(fp,"    %s     '%C'   %10.2f  %10.2f  %10.2f  %10.2f %10.2f %10.2f %10.2f\n",
			mat.mark,mat.cBriefMark,mat.E,mat.intensity[4],mat.intensity[5],mat.intensity[6],
			mat.intensity[8],mat.intensity[9],mat.intensity[10]);
	}
	fprintf(fp,"\n");
	fprintf(fp,"===============================================================================================\n");
	fprintf(fp,"�ڢ󲿷�: ��������б�\n\n");
	bool bHasAngle=false,bHasTube=false;
	for(CElement *pElem=postprocessor.hashElems.GetFirst();pElem;pElem=postprocessor.hashElems.GetNext())
	{
		if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE||pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
			bHasAngle=true;
		else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
			bHasTube=true;
		if(bHasAngle&&bHasTube)
			break;
	}
	fprintf(fp,"���| ��  �� | �� �� |��С��|ƽ����|�� Բ| ��  λ�����| ��  �� | �� �� |��С��|ƽ����|�� Բ| ��  λ\n");
	fprintf(fp,"    |        | �� �� |�뾶  |�뾶  |�� ��| ��  ����    |        | �� �� |�뾶  |�뾶  |�� ��| ��  ��\n");
	fprintf(fp,"    |        | (cm2) |(cm)  |(cm)  |(cm) | (kg/m)��    |        | (cm2) |(cm)  |(cm)  |(cm) | (kg/m)\n");
	fprintf(fp,"---------------------------------------------------------------------------------------------------------\n");
	if(bHasAngle)
	{
		fprintf(fp,"�Ǹֹ��⣺\n");
		for(i=0;i<jgguige_N;)
		{
			char stype[3]=" L";
			if(toupper(jgguige_table[i].cType)!='L')
				sprintf(stype,"%CL",jgguige_table[i].cType);
			fprintf(fp," %3d %s%3.0f*%2.0f %7.3f %6.2f %6.2f %5.2f %7.3f",i+1,stype,jgguige_table[i].wing_wide,jgguige_table[i].wing_thick,
				jgguige_table[i].area,jgguige_table[i].Ry0,jgguige_table[i].Rx,jgguige_table[i].r,jgguige_table[i].theroy_weight);
			i++;
			if(toupper(jgguige_table[i].cType)!='L')
				sprintf(stype,"%CL",jgguige_table[i].cType);
			else
				strcpy(stype," L");
			if(i<jgguige_N)
				fprintf(fp,"�� %3d %s%3.0f*%2.0f %7.3f %6.2f %6.2f %5.2f %7.3f\n",i+1,stype,jgguige_table[i].wing_wide,jgguige_table[i].wing_thick,
				jgguige_table[i].area,jgguige_table[i].Ry0,jgguige_table[i].Rx,jgguige_table[i].r,jgguige_table[i].theroy_weight);
			else
				fprintf(fp,"��\n");
			i++;
		}
	}
	if(bHasTube)
	{
		fprintf(fp,"�ֹܹ��⣺\n");
		for(i=0;i<tubeguige_N;)
		{
			char stype[3]="��";
			fprintf(fp," %3d %s%3.0f*%2.0f %7.3f %6.2f              %7.3f",i+1,stype,tubeguige_table[i].D,tubeguige_table[i].thick,
				tubeguige_table[i].area,tubeguige_table[i].r,tubeguige_table[i].theroy_weight);
			i++;
			if(i<tubeguige_N)
				fprintf(fp,"�� %3d %s%3.0f*%2.0f %7.3f %6.2f              %7.3f\n",i+1,stype,tubeguige_table[i].D,tubeguige_table[i].thick,
				tubeguige_table[i].area,tubeguige_table[i].r,tubeguige_table[i].theroy_weight);
			else
				fprintf(fp,"��\n");
			i++;
		}
	}

	fprintf(fp,"================================\n");
	fprintf(fp,"�ڢ�����: ��˨��е����\n");
	fprintf(fp,"\n");
	fprintf(fp," ��˨|    ����|    ����|�ױڳ�ѹ\n");
	fprintf(fp," ����|    ǿ��|    ǿ��|    ǿ��\n");
	fprintf(fp,"     |   (MPa)|   (MPa)|   (MPa)\n");
	fprintf(fp,"--------------------------------\n");
	for(i=0;i<LsTypeCount;i++)
	{
		if(i<LsTypeCount)
		{
			if(strlen(ls_table[i].grade)==3)
				fprintf(fp,"  %s %8.2f %8.2f %8.2f\n",ls_table[i].grade,ls_table[i].fv,ls_table[i].f,ls_table[i].fc);
			else	//10.9����˨
				fprintf(fp," %s %8.2f %8.2f %8.2f\n",ls_table[i].grade,ls_table[i].fv,ls_table[i].f,ls_table[i].fc);
		}
	}
	fprintf(fp,"\n");
	fprintf(fp,"=======================================================================================================\n");
	fprintf(fp,"�ڢ�����: �ڵ㼸����Ϣ\n");
	fprintf(fp,"\n");
	fprintf(fp," �ڵ�|�Գ�| ����X����|����Y����|����Z����|X��λ��|Y��λ��|Z��λ��|X��ת��|Y��ת��|Z��ת��| ���ߺ�\n");
	fprintf(fp," ���|��Ϣ|       (m)|      (m)|      (m)|   Լ��|   Լ��|   Լ��|   Լ��|   Լ��|   Լ��|   �б�\n");
	fprintf(fp,"-------------------------------------------------------------------------------------------------------\n");
	ARRAY_LIST<NODE_PTR>node_arr;
	i=0;
	node_arr.SetSize(0,1000);
	for(CLDSNode *pNode=preprocessor.result.allnodeset.GetFirst();pNode;pNode=preprocessor.result.allnodeset.GetNext())
	{
		if(pNode->IsBodyObj()&&!pNode->cfgword.And(cfgwordBody))
			continue;
		if(pNode->IsLegObj()&&!pNode->cfgword.And(cfgwordLeg))
			continue;
		node_arr.append(pNode);
	}
	CQuickSort<NODE_PTR>::QuickSort(node_arr.m_pData,node_arr.GetSize(),compare_func1);
	for(i=0;i<node_arr.GetSize();i++)
	{
		char symmetry[20]="";
		char sDispRest[100]="";
		if(node_arr[i]->IsFatherObject())	//���ڵ�
		{
			long hMirX=node_arr[i]->GetRelativeMirObj(MIRMSG(1),FALSE);
			long hMirY=node_arr[i]->GetRelativeMirObj(MIRMSG(2),FALSE);
			long hMirZ=node_arr[i]->GetRelativeMirObj(MIRMSG(3),FALSE);
			if(hMirX>0x20&&hMirX!=node_arr[i]->handle)
				strcat(symmetry,"  X");
			else
				strcat(symmetry,"   ");
			if(hMirY>0x20&&hMirY!=node_arr[i]->handle)
				strcat(symmetry,"Y");
			else
				strcat(symmetry," ");
			if(hMirZ>0x20&&hMirZ!=hMirX&&hMirZ!=hMirY&&hMirZ!=node_arr[i]->handle)
				strcat(symmetry,"Z");
			else
				strcat(symmetry," ");
			if(stricmp(symmetry,"     ")==0)
				strcpy(symmetry,"        ��");
			else //if(strlen(symmetry)>0)
				strcat(symmetry,"-�Գ�");
		}
		else
		{
			RELATIVE_OBJECT relaObj;
			if(node_arr[i]->GetFatherMirObj(&relaObj))
			{
				if(relaObj.mirInfo.axis_flag==1||(fabs(node_arr[i]->Position(false).x)<EPS&&relaObj.mirInfo.axis_flag==4))
					strcpy(symmetry,"X-�Գ�����");
				else if(relaObj.mirInfo.axis_flag==2||(fabs(node_arr[i]->Position(false).y)<EPS&&relaObj.mirInfo.axis_flag==4))
					strcpy(symmetry,"Y-�Գ�����");
				else if(relaObj.mirInfo.axis_flag==4)
					strcpy(symmetry,"Z-�Գ�����");
				else
					strcpy(symmetry,"        ��");
				if(g_sysPara.bMergeRptFileMirObj)
					continue;	//�ϲ������Գ���ʱ���ӶԳ����ʾ���
			}
			else
				strcpy(symmetry,"        ��");
		}
		if(node_arr[i]->IsHasRestrict(0,X_RESTRICT))
			strcat(sDispRest,"   �̶�");
		else
			strcat(sDispRest,"   ����");
		if(node_arr[i]->IsHasRestrict(0,Y_RESTRICT))
			strcat(sDispRest,"    �̶�");
		else
			strcat(sDispRest,"    ����");
		if(node_arr[i]->IsHasRestrict(0,Z_RESTRICT))
			strcat(sDispRest,"    �̶�");
		else
			strcat(sDispRest,"    ����");
		if(node_arr[i]->IsHasRestrict(0,X_ROT_RESTRICT))
			strcat(sDispRest,"    �̶�");
		else
			strcat(sDispRest,"    ����");
		if(node_arr[i]->IsHasRestrict(0,Y_ROT_RESTRICT))
			strcat(sDispRest,"    �̶�");
		else
			strcat(sDispRest,"    ����");
		if(node_arr[i]->IsHasRestrict(0,Z_ROT_RESTRICT))
			strcat(sDispRest,"    �̶�");
		else
			strcat(sDispRest,"    ����");
		CXhChar50 moduleStr;
		if(node_arr[i]->IsLegObj())
			node_arr[i]->cfgword.GetLegScopeStr(moduleStr,51,true);
		else
			node_arr[i]->cfgword.GetBodyScopeStr(moduleStr,51);
		strcat(sDispRest,"  ");
		strcat(sDispRest,moduleStr);
		fprintf(fp,"%5d %s  %8.3f  %8.3f  %8.3f %s\n",node_arr[i]->point_i,symmetry,
			node_arr[i]->Position(false).x/1000,node_arr[i]->Position(false).y/1000,node_arr[i]->Position(false).z/1000,sDispRest);
	}
	fprintf(fp,"\n");
	//Part ��: �����б�
	fprintf(fp,"======================================================\n");
	fprintf(fp,"�ڢ�����: �����嵥\n");
	fprintf(fp,"\n");
	fprintf(fp,"           ����|      ������|       ������|    ������ \n");
	fprintf(fp,"    ����/���Ⱥ�|        (kg)|         (kN)|������ϵ��(kg)\n");
	fprintf(fp,"------------------------------------------------------\n");
	i=0;
	for(TOWERWEIGHT *pWeight=postprocessor.TowerWeight.GetFirst();pWeight;pWeight=postprocessor.TowerWeight.GetNext())
	{
		char csMark[20]="";
		CXhChar16 cfgStr;
		if(pWeight->iBody0Leg1==0)	//����
		{
			cfgStr.Printf("%d",pWeight->cfg_no);
			cfgStr.ResizeLength(3,' ',true);
			sprintf(csMark," %s           ",(char*)cfgStr,pWeight->cfg_no);
		}
		else						//����
		{
			CFGWORD cfgword(pWeight->cfg_no);
			cfgword.GetLegScopeStr(cfgStr,17,true);
			cfgStr.ResizeLength(4,' ',true);
			sprintf(csMark," %s          ",(char*)cfgStr);//(pWeight->cfg_no-1)/8+1,(pWeight->cfg_no-1)%8+'A');
		}
		int str_len=strlen(csMark);
		if(str_len<15)
		{
			for(int j=15;j>=0;j--)
			{
				if(j-15+str_len<0)
					csMark[j]=' ';
				else
					csMark[j]=csMark[j-15+str_len];
			}
		}
		fprintf(fp,"%s    %8.2f    %8.3f    %8.3f\n",csMark,pWeight->netWeight,pWeight->netWeight*0.0098,pWeight->weight);
	}
	fprintf(fp,"\n");
	//Part ��: ����������
	if(g_sysPara.bCalStdFoundationForce)
		fprintf(fp,"================================================================================================\n");
	else
		fprintf(fp,"============================================================\n");
	fprintf(fp,"�ڢ�����: �����ڵ㷴������\n");
	fprintf(fp,"\n");
	//if(g_sysPara.bCalStdFoundationForce)
	//{
	//	fprintf(fp," �ڵ�|          ����״̬|     Rx(kN)|     Ry(kN)|     Rz(kN)|     Rx(kN)|     Ry(kN)|     Rz(kN)\n");
	//	fprintf(fp," ���|����,������,���Ⱥ�|     ���ֵ|     ���ֵ|     ���ֵ|     ��׼ֵ|     ��׼ֵ|     ��׼ֵ\n");
	//	fprintf(fp,"------------------------------------------------------------------------------------------------\n");
	//}
	//else
	{
		fprintf(fp," �ڵ�|          ����״̬|     Rx(kN)|     Ry(kN)|     Rz(kN)\n");
		fprintf(fp," ���|����,������,���Ⱥ�|\n");//     ���ֵ|     ���ֵ|     ���ֵ\n");
		fprintf(fp,"------------------------------------------------------------\n");
	}
	int nReaction=postprocessor.Reaction.GetNodeNum();
	REACTION_PTR *react_arr=new REACTION_PTR[nReaction];
	i=0;
	for(BASENODE_REACTION *pReact=postprocessor.Reaction.GetFirst();pReact;pReact=postprocessor.Reaction.GetNext())
	{
		pReact->status=0;
		react_arr[i]=pReact;
		i++;
	}
	//CQuickSort<REACTION_PTR>::QuickSort(react_arr,nReaction,compare_func3);
	//�˴����ÿ�������������������ֻ�ʱ����ݹ���ȳ���ת���������򣬽�������Ч�ʼ����½����������� wjh-2013-5-1
	CHeapSort<REACTION_PTR>::HeapSort(react_arr,nReaction,compare_func3);
	long prev_point_i=0,nCount=0;
	double Rzmax=0,Rzmin=0;
	ARRAY_LIST<REACTION_PTR>exportReactArr;
	exportReactArr.SetSize(0,10);
	long iRzmax=0,iRzmin=0;
	for(i=0;i<nReaction;i++)
	{
		if(g_sysPara.reactionExportType==0)
		{
			exportReactArr.append(react_arr[i]);
			continue;
		}
		if(i==0)
			iRzmin=iRzmax=0;
		else //if(i>0)
		{
			
			if( g_sysPara.reactionExportType==1&&(react_arr[i-1]->pNode->point_i==react_arr[i]->pNode->point_i)&&
				react_arr[i-1]->pModule==react_arr[i]->pModule&&react_arr[i-1]->pStatus==react_arr[i]->pStatus)
			{	//�ߵ��ȼ�ɸѡ
				//if(g_sysPara.bCalStdFoundationForce&&react_arr[i]->reaction_force_std.z>react_arr[iRzmax]->reaction_force_std.z)
				//	iRzmax=i;
				//else if(!g_sysPara.bCalStdFoundationForce&&react_arr[i]->reaction_force_design.z>react_arr[iRzmax]->reaction_force_design.z)
				if(react_arr[i]->reaction_force_design.z>react_arr[iRzmax]->reaction_force_design.z)
					iRzmax=i;
				//if(g_sysPara.bCalStdFoundationForce&&react_arr[i]->reaction_force_std.z<react_arr[iRzmin]->reaction_force_std.z)
				//	iRzmin=i;
				//else if(!g_sysPara.bCalStdFoundationForce&&react_arr[i]->reaction_force_design.z<react_arr[iRzmin]->reaction_force_design.z)
				if(react_arr[i]->reaction_force_design.z<react_arr[iRzmin]->reaction_force_design.z)
					iRzmin=i;
			}
			else if(g_sysPara.reactionExportType==2&&(react_arr[i-1]->pNode->point_i==react_arr[i]->pNode->point_i)&&
				react_arr[i-1]->pModule==react_arr[i]->pModule)
			{	//������ɸѡ
				//if(g_sysPara.bCalStdFoundationForce&&react_arr[i]->reaction_force_std.z>react_arr[iRzmax]->reaction_force_std.z)
				//	iRzmax=i;
				//else if(!g_sysPara.bCalStdFoundationForce&&react_arr[i]->reaction_force_design.z>react_arr[iRzmax]->reaction_force_design.z)
				if(react_arr[i]->reaction_force_design.z>react_arr[iRzmax]->reaction_force_design.z)
					iRzmax=i;
				//if(g_sysPara.bCalStdFoundationForce&&react_arr[i]->reaction_force_std.z<react_arr[iRzmin]->reaction_force_std.z)
				//	iRzmin=i;
				//else if(!g_sysPara.bCalStdFoundationForce&&react_arr[i]->reaction_force_design.z<react_arr[iRzmin]->reaction_force_design.z)
				if(react_arr[i]->reaction_force_design.z<react_arr[iRzmin]->reaction_force_design.z)
					iRzmin=i;
			}
			else
			{	//�ֻ�
				if(iRzmin<iRzmax)
				{
					//if((g_sysPara.bCalStdFoundationForce&&react_arr[iRzmin]->reaction_force_std.z<0)||
					//  (!g_sysPara.bCalStdFoundationForce&&react_arr[iRzmin]->reaction_force_design.z<0))
					if(react_arr[iRzmin]->reaction_force_design.z<0)
						exportReactArr.append(react_arr[iRzmin]);
					//if((g_sysPara.bCalStdFoundationForce&&react_arr[iRzmax]->reaction_force_std.z>0)||
					//  (!g_sysPara.bCalStdFoundationForce&&react_arr[iRzmax]->reaction_force_design.z>0))
					if(react_arr[iRzmax]->reaction_force_design.z>0)
						exportReactArr.append(react_arr[iRzmax]);
				}
				else if(iRzmin>iRzmax)
				{
					//if((g_sysPara.bCalStdFoundationForce&&react_arr[iRzmax]->reaction_force_std.z>0)||
					//  (!g_sysPara.bCalStdFoundationForce&&react_arr[iRzmax]->reaction_force_design.z>0))
					if(react_arr[iRzmax]->reaction_force_design.z>0)
						exportReactArr.append(react_arr[iRzmax]);
					//if((g_sysPara.bCalStdFoundationForce&&react_arr[iRzmin]->reaction_force_std.z<0)||
					//  (!g_sysPara.bCalStdFoundationForce&&react_arr[iRzmin]->reaction_force_design.z<0))
					if(react_arr[iRzmin]->reaction_force_design.z<0)
						exportReactArr.append(react_arr[iRzmin]);
				}
				else
					exportReactArr.append(react_arr[iRzmin]);
				iRzmin=iRzmax=i;
			}
		}
		if(i==nReaction-1)
		{	//���һ����¼ʱ����
			if(iRzmin<iRzmax)
			{
				//if((g_sysPara.bCalStdFoundationForce&&react_arr[iRzmin]->reaction_force_std.z<0)||
				//  (!g_sysPara.bCalStdFoundationForce&&react_arr[iRzmin]->reaction_force_design.z<0))
				if(react_arr[iRzmin]->reaction_force_design.z<0)
					exportReactArr.append(react_arr[iRzmin]);
				//if((g_sysPara.bCalStdFoundationForce&&react_arr[iRzmax]->reaction_force_std.z>0)||
				//  (!g_sysPara.bCalStdFoundationForce&&react_arr[iRzmax]->reaction_force_design.z>0))
				if(react_arr[iRzmax]->reaction_force_design.z>0)
					exportReactArr.append(react_arr[iRzmax]);
			}
			else if(iRzmin>iRzmax)
			{
				//if((g_sysPara.bCalStdFoundationForce&&react_arr[iRzmax]->reaction_force_std.z>0)||
				//  (!g_sysPara.bCalStdFoundationForce&&react_arr[iRzmax]->reaction_force_design.z>0))
				if(react_arr[iRzmax]->reaction_force_design.z>0)
					exportReactArr.append(react_arr[iRzmax]);
				//if((g_sysPara.bCalStdFoundationForce&&react_arr[iRzmin]->reaction_force_std.z<0)||
				//  (!g_sysPara.bCalStdFoundationForce&&react_arr[iRzmin]->reaction_force_design.z<0))
				if(react_arr[iRzmin]->reaction_force_design.z<0)
					exportReactArr.append(react_arr[iRzmin]);
			}
			else
				exportReactArr.append(react_arr[iRzmin]);
		}
	}
	delete []react_arr;
	for(i=0;i<exportReactArr.GetSize();i++)
	{
		char quadLeg[4]={0};
		CXhChar50 csMark;
		for(int k=0;k<4;k++)
			quadLeg[k]=(exportReactArr[i]->leg_no[k]-1)%8+'A';
		csMark.Printf("%2d,%3d,%C-%C-%C-%C ",exportReactArr[i]->pModule->iNo,exportReactArr[i]->pStatus->iNo,
			quadLeg[0],quadLeg[1],quadLeg[2],quadLeg[3]);
		csMark.ResizeLength(19,' ',true);
		if(prev_point_i==0)
		{
			nCount++;
			/*
			if(g_sysPara.bCalStdFoundationForce)
			{
				if(exportReactArr[i]->reaction_force_std.z>0)
					Rzmax=exportReactArr[i]->reaction_force_std.z;
				else
					Rzmin=exportReactArr[i]->reaction_force_std.z;
				prev_point_i=exportReactArr[i]->pNode->point_i;
				fprintf(fp,"%5d %s  %9.3f   %9.3f   %9.3f   %9.3f   %9.3f   %9.3f\n",exportReactArr[i]->pNode->point_i,(char*)csMark,
					exportReactArr[i]->reaction_force_design.x/1000,exportReactArr[i]->reaction_force_design.y/1000,exportReactArr[i]->reaction_force_design.z/1000,
					exportReactArr[i]->reaction_force_std.x/1000,exportReactArr[i]->reaction_force_std.y/1000,exportReactArr[i]->reaction_force_std.z/1000);
			}
			else
			*/
			{
				if(exportReactArr[i]->reaction_force_design.z>0)
					Rzmax=exportReactArr[i]->reaction_force_design.z;
				else
					Rzmin=exportReactArr[i]->reaction_force_design.z;
				prev_point_i=exportReactArr[i]->pNode->point_i;
				fprintf(fp,"%5d %s  %9.3f   %9.3f   %9.3f\n",exportReactArr[i]->pNode->point_i,(char*)csMark,
					exportReactArr[i]->reaction_force_design.x/1000,exportReactArr[i]->reaction_force_design.y/1000,exportReactArr[i]->reaction_force_design.z/1000);
			}
		}
		else if(prev_point_i==exportReactArr[i]->pNode->point_i)
		{
			nCount++;
			/*
			if(g_sysPara.bCalStdFoundationForce)
			{
				if(exportReactArr[i]->reaction_force_std.z>Rzmax)
					Rzmax=exportReactArr[i]->reaction_force_std.z;
				else if(exportReactArr[i]->reaction_force_std.z<Rzmin)
					Rzmin=exportReactArr[i]->reaction_force_std.z;
				prev_point_i=exportReactArr[i]->pNode->point_i;
				fprintf(fp,"      %s  %9.3f   %9.3f   %9.3f   %9.3f   %9.3f   %9.3f\n",(char*)csMark,
					exportReactArr[i]->reaction_force_design.x/1000,exportReactArr[i]->reaction_force_design.y/1000,exportReactArr[i]->reaction_force_design.z/1000,
					exportReactArr[i]->reaction_force_std.x/1000,exportReactArr[i]->reaction_force_std.y/1000,exportReactArr[i]->reaction_force_std.z/1000);
			}
			else
			*/
			{
				if(exportReactArr[i]->reaction_force_design.z>Rzmax)
					Rzmax=exportReactArr[i]->reaction_force_design.z;
				else if(exportReactArr[i]->reaction_force_design.z<Rzmin)
					Rzmin=exportReactArr[i]->reaction_force_design.z;
				prev_point_i=exportReactArr[i]->pNode->point_i;
				fprintf(fp,"      %s  %9.3f   %9.3f   %9.3f\n",(char*)csMark,
					exportReactArr[i]->reaction_force_design.x/1000,exportReactArr[i]->reaction_force_design.y/1000,exportReactArr[i]->reaction_force_design.z/1000);
			}
			if(i==exportReactArr.GetSize()-1&&nCount>=2)	//���һ��������������
				fprintf(fp,"                    +Rzmax=%8.3f(kN),-Rzmax=%8.3f(kN)\n",Rzmax/1000,Rzmin/1000);
			continue;
		}
		else	//ת���ڵ�
		{
			if(nCount>=2)
				fprintf(fp,"                    +Rzmax=%8.3f(kN),-Rzmax=%8.3f(kN)\n",Rzmax/1000,Rzmin/1000);
			Rzmax=Rzmin=0;
			nCount=1;
			prev_point_i=exportReactArr[i]->pNode->point_i;
			/*
			if(g_sysPara.bCalStdFoundationForce)
			{
				if(exportReactArr[i]->reaction_force_std.z>0)
					Rzmax=exportReactArr[i]->reaction_force_std.z;
				else
					Rzmin=exportReactArr[i]->reaction_force_std.z;
				fprintf(fp,"\n");
				fprintf(fp,"%5d %s  %9.3f   %9.3f   %9.3f   %9.3f   %9.3f   %9.3f\n",exportReactArr[i]->pNode->point_i,(char*)csMark,
					exportReactArr[i]->reaction_force_design.x/1000,exportReactArr[i]->reaction_force_design.y/1000,exportReactArr[i]->reaction_force_design.z/1000,
					exportReactArr[i]->reaction_force_std.x/1000,exportReactArr[i]->reaction_force_std.y/1000,exportReactArr[i]->reaction_force_std.z/1000);
			}
			else
			*/
			{
				if(exportReactArr[i]->reaction_force_design.z>0)
					Rzmax=exportReactArr[i]->reaction_force_design.z;
				else
					Rzmin=exportReactArr[i]->reaction_force_design.z;
				fprintf(fp,"\n");
				fprintf(fp,"%5d %s  %9.3f   %9.3f   %9.3f\n",exportReactArr[i]->pNode->point_i,(char*)csMark,
					exportReactArr[i]->reaction_force_design.x/1000,exportReactArr[i]->reaction_force_design.y/1000,exportReactArr[i]->reaction_force_design.z/1000);
			}
		}
	}
	//Part �� & Part ��
	fprintf(fp,"\n");
	int nElemNum=postprocessor.hashElems.GetNodeNum();
	ELEM_PTR *elem_arr=new ELEM_PTR[nElemNum];
	i=0;
	for(pElem=postprocessor.hashElems.GetFirst();pElem;pElem=postprocessor.hashElems.GetNext())
	{
		elem_arr[i]=pElem;
		i++;
	}
	CQuickSort<ELEM_PTR>::QuickSort(elem_arr,nElemNum,compare_func2);
	//Part : �˼�������Ϣ
	//fprintf(fp,"===========================================================================================================\n");
	//fprintf(fp,"�ڢ�����: ��Ԫ���������Ϣ\n");
	//fprintf(fp,"\n");
	//fprintf(fp,"     ��Ԫ|  ��Ԫ|   ��Ԫ|  ͳ�ĺ�|  ��λ|     ��|  ë��|    ��˨|   ƽ����|   ��С��| �����|   ��˨|  ��˨\n");
	//fprintf(fp,"     ��ʶ|  ����|   ���|        |  ����|   ����|  ���|  ������| ��ת�뾶| ��ת�뾶| ��ģ��|   ��Ϣ|  ����\n");
	//fprintf(fp,"         |      |   (mm)|        |(kg/m)|    (m)| (cm2)|        |   Rx(cm)|  Ry0(cm)|Wx(cm3)|       |  ϵ��\n");
	//fprintf(fp,"-----------------------------------------------------------------------------------------------------------\n");
	fprintf(fp,"==================================================================================================================\n");
	fprintf(fp,"�ڢ�����: ��Ԫ���������Ϣ\n");
	fprintf(fp,"\n");
	fprintf(fp,"     ��Ԫ|      �Գ�|��Ԫ|��Ԫ�� |ͳ��| ����|�� ��|    ��˨  | ����| �� | ����|     ���㳤��       | G.R.| ��ϸ�� \n");
	fprintf(fp,"     ��ʶ|      ��Ϣ|����|��(mm) | �� | ����|״ ��|    ��Ϣ  | ϵ��|����| (mm)|       (mm)         | (mm)|���㹫ʽ\n");
	fprintf(fp,"------------------------------------------------------------------------------------------------------------------\n");

	for(i=0;i<nElemNum;i++)
	{
		long No[2];
		//��Ԫ��ʶ
		bool bReverse=elem_arr[i]->pStart->point_i>elem_arr[i]->pEnd->point_i;
		No[0]=min(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		No[1]=max(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		//�Գ���Ϣ
		char symmetry[20]="";
		if(elem_arr[i]->relaElem.bFatherElem)//pOrgPole->IsFatherObject())	//���˼�
		{
			if(elem_arr[i]->relaElem.idElemMirX>0)
				strcat(symmetry,"  X");
			else
				strcat(symmetry,"   ");
			if(elem_arr[i]->relaElem.idElemMirY>0)
				strcat(symmetry,"Y");
			else
				strcat(symmetry," ");
			if(elem_arr[i]->relaElem.idElemMirZ>0)
				strcat(symmetry,"Z");
			else
				strcat(symmetry," ");
			if(stricmp(symmetry,"     ")==0)
				strcpy(symmetry,"        ��");
			else //if(strlen(symmetry)>0)
				strcat(symmetry,"-�Գ�");
		}
		else
		{
			long idFather=preprocessor.GetFatherElemId(elem_arr[i]);
			if(idFather>0&&idFather==elem_arr[i]->relaElem.idElemMirX)
				strcpy(symmetry,"X-�Գ�����");
			else if(idFather>0&&idFather==elem_arr[i]->relaElem.idElemMirY)
				strcpy(symmetry,"Y-�Գ�����");
			else if(idFather>0&&idFather==elem_arr[i]->relaElem.idElemMirZ)
				strcpy(symmetry,"Z-�Գ�����");
			else
				strcpy(symmetry,"        ��");
			/*RELATIVE_OBJECT relaObj;
			if(elem_arr[i]->pOrgPole->GetFatherMirObj(&relaObj))
			{
				if(relaObj.mirInfo.axis_flag==1)
					strcpy(symmetry,"X-�Գ�����");
				else if(relaObj.mirInfo.axis_flag==2)
					strcpy(symmetry,"Y-�Գ�����");
				else if(relaObj.mirInfo.axis_flag==4)
					strcpy(symmetry,"Z-�Գ�����");
				else
					strcpy(symmetry,"        ��");
			}
			else
				strcpy(symmetry,"        ��");*/
		}
		//��Ԫ����
		char elemtype[5]="";
		_snprintf(&elemtype[2],2,"%s",elem_arr[i]->pOrgPole->GetBriefPartTypeMark());
		elemtype[0]=elem_arr[i]->pOrgPole->layer(1);
		elemtype[1]='^';
		if(strlen(elemtype)==3)
		{	//��λ���뵥�Ǹֵ�Ԫ����
			elemtype[3]=elemtype[2];
			elemtype[2]=' ';	
		}
		//ͳ�ĺ�
		char statmatno[5]="";
		if(elem_arr[i]->pOrgPole->m_uStatMatNo==0)
			strcpy(statmatno,"    ");
		else
			_snprintf(statmatno,4,"%4d",elem_arr[i]->pOrgPole->m_uStatMatNo);
		//��������
		char jointtype[6]="";
		if(elem_arr[i]->real_start_joint_type==JOINT_HINGE)
			strcpy(jointtype,"��-");
		else
			strcpy(jointtype,"��-");
		if(elem_arr[i]->real_end_joint_type==JOINT_HINGE)
			strcat(jointtype,"��");
		else
			strcat(jointtype,"��");
		//�˶�״��
		char forcetype[6]="";
		if(elem_arr[i]->design_info.bStartRotRest)
			forcetype[0]='P';
		else
			forcetype[0]='F';
		if(elem_arr[i]->design_info.bEndRotRest)
			forcetype[1]='P';
		else
			forcetype[1]='F';
		forcetype[2]='^';
		if(elem_arr[i]->start_force_type==CENTRIC_FORCE)
			forcetype[3]='C';
		else
			forcetype[3]='E';
		if(elem_arr[i]->end_force_type==CENTRIC_FORCE)
			forcetype[4]='C';
		else
			forcetype[4]='E';
		if(bReverse)
		{
			char ch=jointtype[0];
			jointtype[0]=jointtype[3];
			jointtype[3]=ch;
			ch=jointtype[1];
			jointtype[1]=jointtype[4];
			jointtype[4]=ch;

			ch=forcetype[0];
			forcetype[0]=forcetype[1];
			forcetype[1]=ch;
			ch=forcetype[3];
			forcetype[3]=forcetype[4];
			forcetype[4]=ch;
		}
		CXhChar16 ss_ls;
		/*if(elem_arr[i]->pOrgPole->guige.pole_type==TRUSS_POLE_USERDEF)
		{
		fprintf(fp,"%4d-%4d      U  %3.0fX%2.0f        %8.3f%8.3f %6.2f\n",No[0],No[1],
		elem_arr[i]->pOrgPole->GetWidth(),elem_arr[i]->pOrgPole->GetThick(),
		elem_arr[i]->pOrgPole->userdef.unit_weight,elem_arr[i]->GetLength()/1000,
		elem_arr[i]->pOrgPole->userdef.area/100);
		}*/
		if(elem_arr[i]->pStart==elem_arr[i]->pOrgPole->pStart)	//ʼ�˵�Ԫ
			ss_ls.Printf("%sM%2dX%2d %5.3f",elem_arr[i]->pOrgPole->connectStart.grade,elem_arr[i]->pOrgPole->connectStart.d,elem_arr[i]->pOrgPole->connectStart.wnSummBoltN,elem_arr[i]->pOrgPole->connectStart.bearloadcoef);
		else if(elem_arr[i]->pEnd==elem_arr[i]->pOrgPole->pEnd)	//ĩ�˵�Ԫ
			ss_ls.Printf("%sM%2dX%2d %5.3f",elem_arr[i]->pOrgPole->connectEnd.grade,elem_arr[i]->pOrgPole->connectEnd.d,elem_arr[i]->pOrgPole->connectEnd.wnSummBoltN,elem_arr[i]->pOrgPole->connectEnd.bearloadcoef);
		else
			ss_ls.Printf("Middle Elem.   ");
		CXhChar50 size_spec;//[200]="";
		if(elem_arr[i]->pOrgPole->size_idClassType==CLS_CABLE)
			size_spec.Copy(elem_arr[i]->pOrgPole->GetSpec());
		else
			size_spec.Printf("%3.0fX%2.0f%C",elem_arr[i]->pOrgPole->GetWidth(),
			elem_arr[i]->pOrgPole->GetThick(),elem_arr[i]->pOrgPole->cMaterial);
		fprintf(fp,"%4d-%4d %s %s %s %s %s %s  %s %4.2f %5.0f%s=%4.0f %5.1f %s\n",
			No[0],No[1],symmetry,elemtype,(char*)size_spec,statmatno,jointtype,forcetype,(char*)ss_ls,
			elem_arr[i]->design_info.hole_out,elem_arr[i]->Length(),
			(char*)elem_arr[i]->design_info.LoExpression,elem_arr[i]->LamdaL(),elem_arr[i]->LamdaR(),(char*)elem_arr[i]->design_info.lamdaExpression);
	}
	fprintf(fp,"\n");
	if(rpt_type!=0)
	{
		//fprintf(fp,"=======================================================================================================================================================================================\n");
		//fprintf(fp,"�ڢ�����: ��Ԫ��ϸ�����Ϣ\n");
		//fprintf(fp,"\n");
		//fprintf(fp,"     ��Ԫ|    ����|   ���|          ����״̬|    ���|          ����״̬|   ǿ��  |��ѹ��|ѹ����|   ���㳤��(mm)     |   ��ϸ��      | ���|����ϵ��|   ʼ��|   �ն�|   ʼ��|   �ն�\n");
		//fprintf(fp,"     ��ʶ|    ����| ��Ӧ��|(����,ģ��,���Ⱥ�)|  ѹӦ��|(����,ģ��,���Ⱥ�)|   �ۼ�  |����ϵ|��ϵ��|                    |    ����       | ����|        |   ת��|   ת��|   ����|   ����\n");
		//fprintf(fp,"         |        |  (Mpa)|                  |   (Mpa)|                  |   ϵ��  |��(��)| (mN) |                    |  �����       | Lo/r|        |   Լ��|   Լ��|   ����|   ����\n");
		//fprintf(fp,"---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		fprintf(fp,"=====================================================================================================================\n");
		fprintf(fp,"�ڢ�����: ��Ԫ��ϸ�����Ϣ\n");
		fprintf(fp,"\n");
		fprintf(fp,"     ��Ԫ|    ����|��Ԫ|  ��Ԫ |  ���| ���� |  ���| ���� |  ���|������| ��ѹ��|ǿ����|    |����|��  |���|����ϵ��\n");
		fprintf(fp,"     ��ʶ|    ����|����|  ��� |  ����|  -   |  ѹ��|  -   |  Ӧ��|��Ӧ��| ����ϵ|��ϵ��|����|����|(K*)|����|        \n");
		fprintf(fp,"         |        |    |  (mm) |  (kN)| ���� |  (kN)| ���� | (MPa)|(MPa) | ��(��)|      |(mm)|(mm)|Lo/r|Lo/r|        \n");
		fprintf(fp,"---------------------------------------------------------------------------------------------------------------------\n");
	}
	else
	{
		//fprintf(fp,"==============================================================================================================================================================================\n");
		//fprintf(fp,"�ڢ�����: ��Ԫ��ϸ�����Ϣ\n");
		//fprintf(fp,"\n");
		//fprintf(fp,"     ��Ԫ|    ���|          ����״̬|    ���|          ����״̬|   ǿ��  |��ѹ��|ѹ����|   ���㳤��(mm)     |   ��ϸ��      | ���|����ϵ��|   ʼ��|   �ն�|   ʼ��|   �ն�\n");
		//fprintf(fp,"     ��ʶ|  ��Ӧ��|(����,ģ��,���Ⱥ�)|  ѹӦ��|(����,ģ��,���Ⱥ�)|   �ۼ�  |����ϵ|��ϵ��|                    |    ����       | ����|        |   ת��|   ת��|   ����|   ����\n");
		//fprintf(fp,"         |   (Mpa)|                  |   (Mpa)|                  |   ϵ��  |��(��)| (mN) |                    |  �����       | Lo/r|        |   Լ��|   Լ��|   ����|   ����\n");
		//fprintf(fp,"------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		fprintf(fp,"=======================================================================================================================\n");
		fprintf(fp,"�ڢ�����: ��Ԫ��ϸ�����Ϣ\n");
		fprintf(fp,"\n");
		if(g_sysPara.iEfficacyCeofType==0)
			fprintf(fp,"     ��Ԫ|��Ԫ|  ��Ԫ |  ���| ���� |   ���| ���� |  ����|������|��ѹ��| ǿ����|    |����|��  |���|���ԣ��\n");
		else
			fprintf(fp,"     ��Ԫ|��Ԫ|  ��Ԫ |  ���| ���� |   ���| ���� |  ����|������|��ѹ��| ǿ����|    |����|��  |���|����ϵ��\n");
		fprintf(fp,"     ��ʶ|����|  ��� |  ����|  -   |   ѹ��|  -   |  Ӧ��|��Ӧ��|����ϵ| ��ϵ��|����|����|(K*)|����|        \n");
		fprintf(fp,"         |    |  (mm) |  (kN)| ���� |   (kN)| ���� | (MPa)|(MPa) |��(��)|       |(mm)|(mm)|Lo/r|Lo/r|        \n");
		fprintf(fp,"-------------------------------------------------------------------------------------------------------------\n");
	}
	for(i=0;i<nElemNum;i++)
	{
		long No[2];
		No[0]=min(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		No[1]=max(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		double L=elem_arr[i]->Length();
		char ss[12];
		if(elem_arr[i]->m_iCtrlRule==1)
			strcpy(ss,"  K*Lo/r");
		else if(elem_arr[i]->m_iCtrlRule==2)
			strcpy(ss,"   TENS.");
		else if(elem_arr[i]->m_iCtrlRule==3)
			strcpy(ss,"   COMP.");
		else if(elem_arr[i]->m_iCtrlRule==4)
			strcpy(ss,"   STAB.");
		else if(elem_arr[i]->m_iCtrlRule==5)
			strcpy(ss,"M&S.Code");
		else if(elem_arr[i]->m_iCtrlRule==6)
			strcpy(ss,"    BOLT");
		else //if(elem_arr[i]->pOrgPole->m_iCtrlRule==0)
			strcpy(ss,"    NONE");
		if(rpt_type!=0&&elem_arr[i]->m_iCtrlRule>0
			&&elem_arr[i]->m_iCtrlRule!=5)
			ss[0]='>';
		//��Ԫ����
		char elemtype[5]="";
		_snprintf(&elemtype[2],2,"%s",elem_arr[i]->pOrgPole->GetBriefPartTypeMark());
		elemtype[0]=elem_arr[i]->pOrgPole->layer(1);
		elemtype[1]='^';
		if(strlen(elemtype)==3)
		{	//��λ���뵥�Ǹֵ�Ԫ����
			elemtype[3]=elemtype[2];
			elemtype[2]=' ';	
		}
		CString s1,s2,s3,s4,s5,s6,stable_s;
		long module_no=0,status_no=0;
		SUBID caseno;
		hashModuleNo.GetValueAt(elem_arr[i]->MaxTension.hModule,module_no);
		hashStatusNo.GetValueAt(elem_arr[i]->MaxTension.hWorkStatus.Id(),status_no);
		if(elem_arr[i]->MaxTension.hWorkStatus.Group()>0)
		{
			caseno=SUBID(status_no,elem_arr[i]->MaxTension.hWorkStatus.Group());
			s1.Format("%2d-%s",module_no,(char*)caseno.ToString('@',false,3));
		}
		else
		s1.Format("%2d-%3d",module_no,status_no);
		module_no=status_no=0;	//����
		hashModuleNo.GetValueAt(elem_arr[i]->MaxCompression.hModule,module_no);
		hashStatusNo.GetValueAt(elem_arr[i]->MaxCompression.hWorkStatus.Id(),status_no);
		if(elem_arr[i]->MaxCompression.hWorkStatus.Group()>0)
		{
			caseno=SUBID(status_no,elem_arr[i]->MaxCompression.hWorkStatus.Group());
			s2.Format("%2d-%s",module_no,(char*)caseno.ToString('@',false,3));
		}
		else
		s2.Format("%2d-%3d",module_no,status_no);
		if(elem_arr[i]->MaxCompression.hWorkStatus!=elem_arr[i]->MaxCompStable.hWorkStatus&&elem_arr[i]->MaxCompStable.hWorkStatus.Id()>0&&
			fabs(elem_arr[i]->MaxCompStable.N-elem_arr[i]->MaxCompression.N)>10&&elem_arr[i]->m_iCtrlRule==4)
		{
			status_no=0;
			hashStatusNo.GetValueAt(elem_arr[i]->MaxCompStable.hWorkStatus.Id(),status_no);
			if(elem_arr[i]->MaxCompStable.hWorkStatus.Group()>0)
			{
				caseno=SUBID(status_no,elem_arr[i]->MaxCompStable.hWorkStatus.Group());
				stable_s.Format("                                              %7.2f %2d-%s Max Compression Stability Force",elem_arr[i]->MaxCompStable.N*0.001,module_no,(char*)caseno.ToString('@',false,3));
			}
			else
				stable_s.Format("                                              %7.2f %2d-%3d Max Compression Stability Force",elem_arr[i]->MaxCompStable.N*0.001,module_no,status_no);
		}
		CXhChar16 caserule;
		double ctrlstress=0;
		double coefStrength=1.0;
		double A=elem_arr[i]->pOrgPole->GetArea();
		double An=A-elem_arr[i]->design_info.hole_out*elem_arr[i]->design_info.hole_d*elem_arr[i]->pOrgPole->GetThick();
		STEELMAT *pSteelMat=QuerySteelMatByBriefMark(elem_arr[i]->pOrgPole->cMaterial);
		if(elem_arr[i]->design_info.iDangerousLoadCase==2)		//T
		{
			coefStrength=elem_arr[i]->design_info.fCoefTensStrengthDecrease;
			ctrlstress=elem_arr[i]->MaxTension.N/An;
		}
		else if(elem_arr[i]->design_info.iDangerousLoadCase==3)	//C
		{
			coefStrength=elem_arr[i]->design_info.fCoefCompStrengthDecrease;
			ctrlstress=elem_arr[i]->MaxCompression.N/An;
		}
		else //if(elem_arr[i]->design_info.iDangerousLoadCase==4)	//S
		{
			coefStrength=elem_arr[i]->design_info.mN;
			if(elem_arr[i]->MaxCompression.N>-EPS||elem_arr[i]->design_info.fCoefStablity<EPS)
				ctrlstress=0;
			else	//�������ѹ��Ӧ�����ֹ�����һ�������ѹ��������һ�£��ʲ��������ѹ����ѹ��Ӧ�����ƹ����µ�ѹ��ϵ�� wjh-2013.10.15
				ctrlstress=elem_arr[i]->MaxCompStable.N/(A*elem_arr[i]->design_info.fCoefStablity);
		}
		double limStrength=pSteelMat->f(elem_arr[i]->pOrgPole->GetThick())*coefStrength;
		if(g_sysPara.iEfficacyCeofType==0)
		{
			if(elem_arr[i]->design_info.iDangerousRule==1)
				caserule.Printf("%5.3f@��",elem_arr[i]->design_info.fCoefEffic);
			else if(elem_arr[i]->design_info.iDangerousRule==2)
			{
				if(elem_arr[i]->design_info.bBlockShearActivated)
					caserule.Printf("%5.3f@+T",elem_arr[i]->design_info.fCoefEffic);
				else
					caserule.Printf("%5.3f@ T",elem_arr[i]->design_info.fCoefEffic);
			}
			else if(elem_arr[i]->design_info.iDangerousRule==3)
				caserule.Printf("%5.3f@ C",elem_arr[i]->design_info.fCoefEffic);
			else if(elem_arr[i]->design_info.iDangerousRule==4)
			{
				if(elem_arr[i]->MaxCompStable.bothComp)
					caserule.Printf("%5.3f@+S",elem_arr[i]->design_info.fCoefEffic);
				else
					caserule.Printf("%5.3f@ S",elem_arr[i]->design_info.fCoefEffic);
			}
			else
				caserule.Printf("%5.3f@ 0",elem_arr[i]->design_info.fCoefEffic);
		}
		else
		{
			double effic=0;
			if(coefStrength>EPS)
				effic=fabs(ctrlstress)/limStrength;
			if(elem_arr[i]->design_info.iDangerousLoadCase==2)		//T
			{
				if(elem_arr[i]->design_info.bBlockShearActivated)
					caserule.Printf("%5.3f@+T",effic);
				else
					caserule.Printf("%5.3f@ T",effic);
			}
			else if(elem_arr[i]->design_info.iDangerousLoadCase==3)	//C
				caserule.Printf("%5.3f@ C",effic);
			else if(elem_arr[i]->design_info.iDangerousLoadCase==4)	//S
			{
				if(elem_arr[i]->MaxCompStable.bothComp)
					caserule.Printf("%5.3f@+S",effic);
				else
					caserule.Printf("%5.3f@ S",effic);
			}
			else	//���Ҳ���ܲ������������治�ÿ�
				caserule.Printf("0.000@ S");
		}
		char maxCompForceSymbol=' ';
		if(elem_arr[i]->MaxCompStable.N<elem_arr[i]->MaxCompression.N)
			maxCompForceSymbol='>';
		char lamda_s[10];
		//if(elem_arr[i]->LamdaMax()>elem_arr[i]->Lamda())
		//	_snprintf(lamda_s,9,">%3.0f",elem_arr[i]->Lamda());
		//else
			_snprintf(lamda_s,9," %3.0f",elem_arr[i]->Lamda());
		CXhChar50 size_spec;//[200]="";
		if(elem_arr[i]->pOrgPole->size_idClassType==CLS_CABLE)
			size_spec.Copy(elem_arr[i]->pOrgPole->GetSpec());
		else
			size_spec.Printf("%3.0fX%2.0f%C",elem_arr[i]->pOrgPole->GetWidth(),elem_arr[i]->pOrgPole->GetThick(),elem_arr[i]->pOrgPole->cMaterial);
		if(rpt_type!=0)
		{
			fprintf(fp,"%4d-%4d %s %s %s%7.2f %s%C%7.2f %s %6.1f %6.1f %6.3f %6.3f %4.0f %4.0f %s %4.0f %s\n",
				No[0],No[1],ss,elemtype,(char*)size_spec,
				elem_arr[i]->MaxTension.N/1000,s1,maxCompForceSymbol,elem_arr[i]->MaxCompression.N/1000,s2,ctrlstress,limStrength,
				elem_arr[i]->design_info.fCoefStablity,coefStrength,L,elem_arr[i]->LamdaL(),lamda_s,
				elem_arr[i]->design_info.UltimateLo_r,(char*)caserule);
		}
		else
		{
			fprintf(fp,"%4d-%4d %s %s%7.2f %s%C%7.2f %s %6.1f %6.1f %6.3f %6.3f %4.0f %4.0f %4.0f %4.0f %s\n",No[0],No[1],
				elemtype,(char*)size_spec,
				elem_arr[i]->MaxTension.N/1000,s1,maxCompForceSymbol,elem_arr[i]->MaxCompression.N/1000,s2,ctrlstress,limStrength,
				elem_arr[i]->design_info.fCoefStablity,coefStrength,L,elem_arr[i]->LamdaL(),elem_arr[i]->Lamda(),
				elem_arr[i]->design_info.UltimateLo_r,(char*)caserule);
		}
		if(stable_s.GetLength()>0)
			fprintf(fp,"%s\n",stable_s);
	}
	//Part ��
	fprintf(fp,"=========================================================\n");
	fprintf(fp,"�ڢ�����: ����Ԫ������Ϣ\n\n");
	fprintf(fp,"     ��Ԫ|��Ԫ|  ��Ԫ | ���� |  �� ��|�����|����Ӧ|\n");
	fprintf(fp,"     ��ʶ|����|  ��� |  -   |  �� ��|��Ӧ��|��ռ��|\n");
	fprintf(fp,"         |    |  (mm) | ���� |  (N*m)| (MPa)| (%%)  |\n");
	fprintf(fp,"---------------------------------------------------------\n");
	for(i=0;i<nElemNum;i++)
	{
		long No[2];
		if(elem_arr[i]->MaxMoment.N<=0)
			continue;	//������Ԫ����������
		double bend_stress=0;	//��λMPa
		double ctrlstress=0;	//�����ѹӦ��(MPa)
		double bend_percent=0;
		double W=elem_arr[i]->GetWorkW(0);
		if(W>0)
			bend_stress=elem_arr[i]->MaxMoment.N/W;
		double A=elem_arr[i]->pOrgPole->GetArea();
		double An=A-elem_arr[i]->design_info.hole_out*elem_arr[i]->design_info.hole_d*elem_arr[i]->pOrgPole->GetThick();
		if(elem_arr[i]->design_info.iDangerousLoadCase==2)		//T
			ctrlstress=elem_arr[i]->MaxTension.N/An;
		else if(elem_arr[i]->design_info.iDangerousLoadCase==3)	//C
			ctrlstress=elem_arr[i]->MaxCompression.N/An;
		else //if(elem_arr[i]->design_info.iDangerousLoadCase==4)	//S
		{
			if(elem_arr[i]->MaxCompression.N>-EPS||elem_arr[i]->design_info.fCoefStablity<EPS)
				ctrlstress=0;
			else
				ctrlstress=elem_arr[i]->MaxCompression.N/A;
		}
		if(fabs(ctrlstress)>0)
			bend_percent=fabs(100*bend_stress/ctrlstress);
		No[0]=min(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		No[1]=max(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		//��Ԫ����
		char elemtype[5]="";
		_snprintf(&elemtype[2],2,"%s",elem_arr[i]->pOrgPole->GetBriefPartTypeMark());
		elemtype[0]=elem_arr[i]->pOrgPole->layer(1);
		elemtype[1]='^';
		if(strlen(elemtype)==3)
		{	//��λ���뵥�Ǹֵ�Ԫ����
			elemtype[3]=elemtype[2];
			elemtype[2]=' ';	
		}
		long module_no=0,status_no=0;
		hashModuleNo.GetValueAt(elem_arr[i]->MaxMoment.hModule,module_no);
		hashStatusNo.GetValueAt(elem_arr[i]->MaxMoment.hWorkStatus.Id(),status_no);
		CString s1;
		s1.Format("%2d-%3d",module_no,status_no);
		fprintf(fp,"%4d-%4d %s %3.0fX%2.0f%C %s%8.2f%7.2f %3.0f%%\n",No[0],No[1],
			elemtype,elem_arr[i]->pOrgPole->GetWidth(),elem_arr[i]->pOrgPole->GetThick(),elem_arr[i]->pOrgPole->cMaterial,s1,
			elem_arr[i]->MaxMoment.N/1000,bend_stress,bend_percent);
	}
	//Part ��
	//fprintf(fp,"=========================================================\n");
	//fprintf(fp,"�ڢ�����: ��Ԫ���ƹ����ߵ����ֻ���Ϣ\n\n");
	//fprintf(fp,"     ��Ԫ|   �������״̬   |   ���ѹ��״̬   |\n");
	//fprintf(fp,"     ��ʶ|������,����,���Ⱥ�|������,����,���Ⱥ�|\n");
	//fprintf(fp,"---------------------------------------------------------\n");
	//fprintf(fp,"  10-  12  31, 1, 1- 1- 1- 1   1, 1, 1- 1- 1- 1 \n");
	fclose(fp);
	delete []elem_arr;	
	WinExec(CXhChar200("notepad.exe %s",file_path),SW_SHOW);
	pCmdLine->PromptStrInCmdLine("���İ���ϸ�����ļ������ɣ�");
}

void CreateDetailRptExcelFileFromReportByEN(char *file_name);
void CreateDetailRptFileFromReportByEN(char *file_name,int iDocType/*=0*/)
{
	//if(iDocType == 0)
	//	CreateDetailRptTxtFileFromReportByEN(file_name);
	//else if(iDocType == 1)  //�����EXCEL�ļ�
	//  CreateDetailRptExcelFileFromReportByEN(file_name);
}
void CreateDetailRptExcelFileFromReportByCN(char *file_name);
void CreateDetailRptFileFromReportByCN(char *file_name,int iDocType/*=0*/)
{
	if(iDocType == 0)
		CreateDetailRptTxtFileFromReportByCN(file_name);
	else if(iDocType == 1)  //�����EXCEL�ļ�
		CreateDetailRptExcelFileFromReportByCN(file_name);
}
void CreateFreqsAnalRptTxtFileByCN(char *file_name,DYNAMIC_ANAL_NODE* node_arr,VIBRAMODE_PROP modes_arr[],int modes_num)
{
	CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	char file_path[MAX_PATH];
	strcpy(file_path,file_name);
	strcat(file_path,".frt");
	FILE *fp=fopen(file_path,"wt");
	if(fp==NULL)
	{
		pCmdLine->PromptStrInCmdLine("�������Է��������ļ�����ʧ�ܣ�");
		return;
	}
	CLDSModule *pModule;
	CWorkStatus *pStatus;
	CHashTable<long>hashModuleNo,hashStatusNo;
	hashModuleNo.CreateHashTable(Ta.Module.GetNodeNum());
	hashStatusNo.CreateHashTable(Ta.WorkStatus.GetNodeNum());
	for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		hashModuleNo.SetValueAt(pModule->handle,pModule->iNo);
	for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
		hashStatusNo.SetValueAt(pStatus->handle,pStatus->iNo);
	fprintf(fp,"\n");
	fprintf(fp,"***********************************************************************************************\n");
	fprintf(fp,"*                                                                                             *\n");
	fprintf(fp,"*            �����������������  - (C) 2002-2011 �����ź��������Ƽ����޹�˾                 *\n");
	fprintf(fp,"*                                                                                             *\n");
	fprintf(fp,"***********************************************************************************************\n");
	fprintf(fp,"���̴��� : %s\n",Ta.m_sPrjCode);
	fprintf(fp,"�������� : %s\n",Ta.m_sPrjName);
	fprintf(fp,"�����Ա : %s\n",Ta.m_sOperator);
	CTime t=CTime::GetCurrentTime();
	fprintf(fp,"����ʱ�� : %s\n",t.Format("%H:%M:%S %B %d, %Y"));
	CString product_version;
	theApp.GetProductVersion(product_version);
#ifdef __LDS_
	fprintf(fp,"����汾 : LDS Version %s\n",product_version);
#else
	fprintf(fp,"����汾 : TSA Version %s\n",product_version);
#endif
	fprintf(fp,"��Ȩ�û� : %s\n",Ta.m_sCompanyName);
	fprintf(fp,"\n");
	fprintf(fp,"�ɹ������˶������Է���!\n");
	fprintf(fp,"===============================================================================================\n");
	if(Ta.m_iLoadExecuteCode==1)
		fprintf(fp,"��ǰִ�п���淶�������ʽṹ��ƹ淶��GB50135-2006\n");
	else
		fprintf(fp,"��ǰִ�п���淶���������￹����ƹ淶��GB50191-2006\n");
	fprintf(fp,"�����Ҷ�    =%d��\n",Ta.EarthQuake.grade);
	fprintf(fp,"��������(Tg)=%.3fs\n",Ta.EarthQuake.Tg);
	fprintf(fp,"�ṹ����Ȧ�=%.3f\n",Ta.EarthQuake.zeta);
	fprintf(fp,"ˮƽ����Ӱ��ϵ�����ֵ=%.3f\n",Ta.EarthQuake.alfa_max);
	fprintf(fp,"\n");

	CStringArray modes_name_arr;
	modes_name_arr.Add("��");
	modes_name_arr.Add("��");
	modes_name_arr.Add("��");
	modes_name_arr.Add("��");
	modes_name_arr.Add("��");
	modes_name_arr.Add("��");
	modes_name_arr.Add("��");
	modes_name_arr.Add("��");
	modes_name_arr.Add("��");
	modes_name_arr.Add("��");
	for(int iMode=0;iMode<modes_num;iMode++)
	{
		fprintf(fp,"===============================================================================================\n");
		if(iMode<10)
			fprintf(fp,"��%s���Ͷ�������\n\n",modes_name_arr[iMode]);
		else
			fprintf(fp,"��%d���Ͷ�������\n\n",iMode);
		fprintf(fp,"����T=%.3fs\n",modes_arr[iMode].period);
		double alfa=CalAlfaOfEQ(modes_arr[iMode].period,Ta.EarthQuake.Tg,Ta.EarthQuake.alfa_max,Ta.EarthQuake.zeta,Ta.m_iLoadExecuteCode);
		fprintf(fp,"����Ӱ��ϵ����=%.3f\n",alfa);
		fprintf(fp,"���Ͳ�Ԥϵ����.x==%.3f\n",modes_arr[iMode].gama_x);
		fprintf(fp,"���Ͳ�Ԥϵ����.y==%.3f\n",modes_arr[iMode].gama_y);
		fprintf(fp,"��ǰ���͵�i�ڵ��j���ɶȵĵ��������������ʽ��Fpi.j=Wi��j��j(i)��j(T),Wi=mi*9.8\n\n");
		
		fprintf(fp,"  �ڵ�| ��  ��| ��i.x | ��i.y | Fpi.x | Fpi.y \n");
		fprintf(fp,"  ���| mi(kg)|       |       | (kN)  | (kN)  \n");
		fprintf(fp,"----------------------------------------------\n");
		for(int i=0;i<modes_arr[0].num;i++)
		{
			double Fpix=node_arr[i].mass*9.8*modes_arr[iMode].gama_x*modes_arr[iMode].x_arr[i]*alfa/1000;
			double Fpiy=node_arr[i].mass*9.8*modes_arr[iMode].gama_y*modes_arr[iMode].y_arr[i]*alfa/1000;
			fprintf(fp,"%6d %7.3f %7.4f %7.4f %7.4f %7.4f\n",node_arr[i].id,node_arr[i].mass,
				modes_arr[iMode].x_arr[i],modes_arr[iMode].y_arr[i],Fpix,Fpiy);
		}
	}
	fclose(fp);
	pCmdLine->PromptStrInCmdLine("�������Է��������ļ��ļ������ɣ�");
}

#endif