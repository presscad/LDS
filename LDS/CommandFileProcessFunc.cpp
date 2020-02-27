//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "MainFrm.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "NewFileDlg.h"
#include "DefinedView.h"
#include "database.h"
#include "ShankDefCls.h"
#include "NewTowerDlg.h"
#include "NewSubstationDlg.h"
#include "NewConeTubeDlg.h"
#include "ConeTubeParts.h"
#include "LicFuncDef.h"
#include "LocalFeatureDef.h"
#include "DbObjPropId.h"
#include "NewTowerDataOper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int CLDSView::ShrinkObjHandle()	//���������
{
	return Ta.handle=Ta.ShrinkObjHandle();
}
//CorrectNodeFatherRodCfgWord()����������ModelOper.cpp���� wjh-2015.12.10
//int CLDSView::CorrectNodeFatherRodCfgWord()	//�����ڵ㸸�˼���ĺ�
int CLDSView::MultiLegSchema()	//ת������ģ�͵ĺ��߽�����ĺ�ռλģʽ
{
	CString cmdStr;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("���߽�����ĺ�ռλģʽ[8 Leg Model(8)/16 Leg Model(16)/24 Leg Model(24)]<%d>:",CFGLEG::MaxLegs());
#else 
	cmdStr.Format("���߽�����ĺ�ռλģʽ[8��ģʽ(8)/16��ģʽ(16)/24��ģʽ(24)]<%d>:",CFGLEG::MaxLegs());
#endif
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	pCmdLine->FillCmdLine(cmdStr,"");
	BYTE schema=CFGLEG::Schema();
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			pCmdLine->m_bGettingStrFromCmdLine=FALSE;
			return -1;
		}
		pCmdLine->m_bGettingStrFromCmdLine=FALSE;
		if(cmdStr.CompareNoCase("8")==0)
			schema=1;
		else if(cmdStr.CompareNoCase("16")==0)
			schema=2;
		else if(cmdStr.CompareNoCase("24")==0)
			schema=3;
		else if(cmdStr.GetLength()>0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please select key word","");
			pCmdLine->FinishCmdLine();
			cmdStr.Format("���߽�����ĺ�ռλģʽ[8 Leg Model(8)/16 Leg Model(16)/24 Leg Model(24)]<%d>:",CFGLEG::MaxLegs());
#else 
			pCmdLine->FillCmdLine("��Ҫѡ��ؼ���","");
			pCmdLine->FinishCmdLine();
			cmdStr.Format("���߽�����ĺ�ռλģʽ[8��ģʽ(8)/16��ģʽ(16)/24��ģʽ(24)]<%d>:",CFGLEG::MaxLegs());
#endif
			pCmdLine->FillCmdLine(cmdStr,"");
			continue;
		}
		break;
	}
	if(schema==CFGLEG::Schema())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->PromptStrInCmdLine("Model Unchanged");
#else 
		pCmdLine->PromptStrInCmdLine("ģʽδ�����仯");
#endif
		return 0;
	}
	//CUndoOperObject undo;
	CXhChar50 sError;
	CLogErrorLife life;
	for(CLDSModule* pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		sError.Copy("");
		pModule->m_dwLegCfgWord=pModule->m_dwLegCfgWord.ConvertSchema(CFGLEG::MULTILEG_DEFAULT,schema,sError,51);
		for(int i=0;i<4;i++)
		{
			int iBodyNo=pModule->m_arrActiveQuadLegNo[i]/CFGLEG::MaxLegs()+1;	//��Ӧ���ߺ�
			int iLegNo =pModule->m_arrActiveQuadLegNo[i]%CFGLEG::MaxLegs();		//��Ӧ���Ⱥ�
			pModule->m_arrActiveQuadLegNo[i]=(iBodyNo-1)*CFGLEG::MaxLegs(schema)+iLegNo;
		}
		if(sError.GetLength()>0)
			logerr.LogString(sError);
	}
	for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		sError.Copy("");
		pNode->cfgword=pNode->cfgword.ConvertSchema(CFGLEG::MULTILEG_DEFAULT,schema,sError,51);
		if(sError.GetLength()>0)
			logerr.LogString(sError);
	}
	for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		sError.Copy("");
		pPart->cfgword=pPart->cfgword.ConvertSchema(CFGLEG::MULTILEG_DEFAULT,schema,sError,51);
		if(sError.GetLength()>0)
			logerr.LogString(sError);
	}
	CFGLEG::SetSchema(schema);
#ifdef AFX_TARG_ENU_ENGLISH
	if(!logerr.IsHasContents())
		pCmdLine->PromptStrInCmdLine("Model transform successfully!");
	else
		pCmdLine->PromptStrInCmdLine("The process of model transform exists interceptive information!");
#else 
	if(!logerr.IsHasContents())
		pCmdLine->PromptStrInCmdLine("ģʽת���ɹ�!");
	else
		pCmdLine->PromptStrInCmdLine("ģʽת�������д�����Ϣ�ض�!");
#endif
	return 0;
}
bool IsPointInQuad(const double* coord,int cQuad)
{
	GEPOINT pos(coord);
	if(cQuad==1&&pos.x>=-EPS&&pos.y>=-EPS)
		return true;
	else if(cQuad==2&&pos.x<=EPS&&pos.y>=-EPS)
		return true;
	else if(cQuad==3&&pos.x>=-EPS&&pos.y<=EPS)
		return true;
	else if(cQuad==4&&pos.x<=EPS&&pos.y<=EPS)
		return true;
	else
		return false;
}
bool IsPartInQuad(CLDSPart* pPart,int cQuad)
{
	CLDSLinePart* pRod=NULL;
	if(pPart->IsLinePart())
	{
		pRod=(CLDSLinePart*)pPart;
		GEPOINT start,end;
		if(pRod->pStart)
			start=pRod->pStart->Position(false);
		else
			start=pRod->Start();
		if(pRod->pEnd)
			end=pRod->pEnd->Position(false);
		else
			end=pRod->End();
		if(IsPointInQuad(start,cQuad)&&IsPointInQuad(end,cQuad))
			return true;
		else
			return false;
	}
	else
		return IsPointInQuad(pPart->ucs.origin,cQuad);
}
bool GetLegKeyNodes(CLDSModule* pModule,int cLegQuad,WORD wLegNo,CLDSNodePtr keyNodes[6])
{
	int nMaxLegs=CFGLEG::MaxLegs();
	int iBodyNo=pModule->GetBodyNo();
	if(cLegQuad<=0||cLegQuad>4)
	{
		logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%d�ź�����%d���޺�",pModule->iNo,wLegNo);
		return false;
	}
	WORD cOldActiveLegNo=pModule->m_arrActiveQuadLegNo[cLegQuad-1];
	pModule->m_arrActiveQuadLegNo[cLegQuad-1]=wLegNo;
	//��ȡԭ��:�Ȳ������ڵ���������Vб�������������Ľڵ�
	CLDSLinePart* pRod;
	CLDSNode *pNode,*pVFrontNode=NULL,*pVSideNode=NULL,*pKTopNode=NULL,*pKBtmNode=NULL;
	CMinDouble minLegZ;
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(!pModule->IsSonPart(pRod))
			continue;	//���ڵ�ǰ���ߵļ��������
		if(pRod->layer(0)=='L'&&pRod->pStart&&pRod->pEnd)
		{
			minLegZ.Update(pRod->pStart->Position('Z'));
			minLegZ.Update(pRod->pEnd->Position('Z'));
		}
	}
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		//1����ȡ�Ȳ���׼�ڵ�
		f3dPoint pos=pNode->Position(false);
		if(pNode->IsBodyObj()&&pos.z<minLegZ.number-EPS)			//�ڵ�λ������
			continue;
		else if(!IsPointInQuad(pNode->Position(false),cLegQuad))
			continue;	//���ڵ�ǰ����
		if(!pModule->IsSonPart(pNode))
			continue;	//���ڵ�ǰ���ߵļ��������
		if(fabs(pos.x)<EPS)
		{
			if(pVFrontNode==NULL)
				pVFrontNode=pNode;
			else if(pos.z<pVFrontNode->Position('Z',false))
				pVFrontNode=pNode;
		}
		else if(fabs(pos.y)<EPS)
		{
			if(pVSideNode==NULL)
				pVSideNode=pNode;
			else if(pos.z<pVSideNode->Position('Z',false))
				pVSideNode=pNode;
		}
		else
		{
			if(pKBtmNode==NULL)
				pKBtmNode=pNode;
			else if(pKBtmNode->Position('Z',false)<pos.z)
				pKBtmNode=pNode;
		}
	}
	CLDSLinePart *pVFrontRod=NULL,*pVSideRod=NULL;
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(pRod->Layer(0)!='L'||pRod->Layer(1)!='X'||pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		if(!pModule->IsSonPart(pRod))
			continue;	//���ڵ�ǰ���ߵļ��������
		GEPOINT lenRodVec;
		if(pRod->pStart==pKBtmNode)
			lenRodVec=pRod->pEnd->Position(false)-pKBtmNode->Position(false);
		else if(pRod->pEnd==pKBtmNode)
			lenRodVec=pRod->pStart->Position(false)-pKBtmNode->Position(false);
		else
			continue;
		if(abs(lenRodVec.x)>abs(lenRodVec.y))	//�����б��
			pVFrontRod=pRod;
		else
			pVSideRod=pRod;
		if(pVFrontRod&&pVSideRod)
			break;
	}
	//����ý�����Ҫ����б�Ĳ���V��ڵ� wht 19-01-25
	if(pVFrontNode==NULL&&pVFrontRod!=NULL&&pVFrontRod->pStart&&pVFrontRod->pEnd)
	{
		if(pVFrontRod->pStart->Position('Z',false)<pVFrontRod->pEnd->Position('Z'))
			pVFrontNode=pVFrontRod->pStart;
		else
			pVFrontNode=pVFrontRod->pEnd;
	}
	if(pVSideNode==NULL&&pVSideRod!=NULL&&pVSideRod->pStart&&pVSideRod->pEnd)
	{
		if(pVSideRod->pStart->Position('Z',false)<pVSideRod->pEnd->Position('Z'))
			pVSideNode=pVSideRod->pStart;
		else
			pVSideNode=pVSideRod->pEnd;
	}
	if(pVFrontNode==NULL||pVSideNode==NULL||pKBtmNode==NULL)
	{
		pModule->m_arrActiveQuadLegNo[cLegQuad-1]=cOldActiveLegNo;
		if(pVFrontNode==NULL||pVSideNode==NULL)
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%d�ź���δ�ҵ�������Vб�Ķ����ڵ�",pModule->iNo);
		else //if(pKBtmNode==NULL)
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%d�ź���δ�ҵ�����Ļ����ڵ�",pModule->iNo);
		return false;
	}
	if(pVFrontRod==NULL||pVSideRod==NULL)
	{
		logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%d�ź���δ�ҵ�����Ľ���������Vб��",pModule->iNo);
		return false;
	}
	GEPOINT frontTop=pVFrontRod->pStart->Position(false);
	GEPOINT frontBtm=pVFrontRod->pEnd->Position(false);
	if(frontTop.z>frontBtm.z)
	{	//����ʼĩ��
		frontTop=frontBtm;
		frontBtm=pVFrontRod->pStart->Position(false);
	}
	GEPOINT sideTop=pVSideRod->pStart->Position(false);
	GEPOINT sideBtm=pVSideRod->pEnd->Position(false);
	if(frontTop.z>frontBtm.z)
	{	//����ʼĩ��
		sideTop=sideBtm;
		sideBtm=pVSideRod->pStart->Position(false);
	}
	CPtInLineCheck frontline(frontBtm,frontTop), sideline(sideBtm,sideTop);
	bool bFrontOnline=false,bSideOnline=false;
	frontline.CheckPointEx(pVFrontNode->Position(false),&bFrontOnline);
	sideline.CheckPointEx(pVSideNode->Position(false),&bSideOnline);
	if(bFrontOnline&&bSideOnline)
	{
		keyNodes[4]=pVFrontNode;
		keyNodes[5]=pVSideNode;
	}
	else
	{
		keyNodes[4]=keyNodes[5]=NULL;
		for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			//1����ȡ�Ȳ���׼�ڵ�
			f3dPoint pos=pNode->Position(false);
			if(fabs(pos.z-pVFrontNode->Position('Z',false))>EPS)
				continue;	//�����������ƽ����
			if(pNode->IsBodyObj()&&pos.z<minLegZ.number-EPS)			//�ڵ�λ������
				continue;
			else if(!IsPointInQuad(pNode->Position(false),cLegQuad))
				continue;	//���ڵ�ǰ����
			if(!pModule->IsSonPart(pNode))
				continue;	//���ڵ�ǰ���ߵļ��������
			bool online=false;
			if(keyNodes[4]==NULL)
			{
				frontline.CheckPointEx(pos,&online);
				if(online)	//��б�����涥���ڵ�
					keyNodes[4]=pNode;
			}
			if(keyNodes[5]==NULL&&!online)
			{
				sideline.CheckPointEx(pos,&online);
				if(online)	//��б�Ĳ��涥���ڵ�
					keyNodes[5]=pNode;
			}
		}
	}
	CLDSLinePart* pLegRod=Ta.FromRodHandle(pKBtmNode->hFatherPart);
	if(pLegRod==NULL)
	{
		pModule->m_arrActiveQuadLegNo[cLegQuad-1]=cOldActiveLegNo;
		logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"δ�ҵ�%d�ź���%d���޻����ڵ㸸�˼�",pModule->iNo,wLegNo);
		return false;
	}
	f3dPoint lineStart,lineEnd=pKBtmNode->Position(false);
	if(pLegRod->pStart==pKBtmNode)
		lineEnd=pLegRod->pEnd->Position(false);
	else
		lineEnd=pLegRod->pStart->Position(false);
	CPtInLineCheck legline(lineStart,lineEnd);
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		f3dPoint pos=pNode->Position(false);
		//1����ȡ�Ȳ���׼�ڵ�
		if(pNode->IsBodyObj()&&pos.z<minLegZ.number-EPS)			//�ڵ�λ������
			continue;
		else if(!IsPointInQuad(pNode->Position(false),cLegQuad))
			continue;	//���ڵ�ǰ����
		if(!pModule->IsSonPart(pNode))
			continue;	//���ڵ�ǰ���ߵļ��������
		if(fabs(pos.x)<EPS||fabs(pos.y)<EPS)
			continue;
		if(pKTopNode==NULL)
			pKTopNode=pNode;
		else if(pos.z<pKTopNode->Position('Z',false)-EPS)
			pKTopNode=pNode;
		else if(fabs(pos.z-pKTopNode->Position('Z',false))<EPS)
		{	//�ȸ�ʱ���ж��Ƿ��ڽ�����
			double justify=legline.CheckPoint(pos);
			if(justify>=0&&justify<=1.0)
				pKTopNode=pNode;
		}
	}
	keyNodes[0]=pVFrontNode;
	keyNodes[1]=pVSideNode;
	keyNodes[2]=pKTopNode;
	keyNodes[3]=pKBtmNode;
	pModule->m_arrActiveQuadLegNo[cLegQuad-1]=cOldActiveLegNo;
	if(keyNodes[0]==NULL||keyNodes[1]==NULL||keyNodes[2]==NULL||keyNodes[3]==NULL)
	{
		logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"δ�ҵ�%d�ź���%d�����Ȳ����Ķ����ڵ�",pModule->iNo,wLegNo);
		return false;
	}
	return true;
}
bool ParseLegMarks(const char* markstr,char* pcBodyNo=NULL,char* pcLegMark=NULL,int* pcLegQuad=NULL)
{
	if(markstr==NULL||strcmp(markstr,"")==0)
		return false;
	CXhChar16 legmarkstr(markstr);
	char delimiter[4]="-:#";
	char* legquad=strchr(legmarkstr,delimiter[0]);
	for(int i=1;legquad==NULL&&i<3;i++)
		legquad=strchr(legmarkstr,delimiter[i]);
	if(legquad!=NULL)
	{
		*legquad=0;
		legquad++;
		if(*legquad<='0'||*legquad>'4')
			return false;	//���޺�����
		else if(pcLegQuad)
			*pcLegQuad=*legquad-'0';
	}
	int len=legmarkstr.GetLength();
	legmarkstr[len-1]=toupper(legmarkstr[len-1]);
	if(!(legmarkstr[len-1]>'@'&&legmarkstr[len-1]-'@'<CFGLEG::MaxLegs()))
		return false;	//���Ⱥų�����ǰ����ģʽ��������
	else if(pcLegMark)
		*pcLegMark=legmarkstr[len-1];
	legmarkstr[len-1]=0;
	if(legmarkstr.GetLength()==0)
		return true;
	int iBodyNo=atoi(legmarkstr);
	for(CLDSModule* pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		if(pModule->GetBodyNo()==iBodyNo)
		{
			if(pcBodyNo)
				*pcBodyNo=(char)iBodyNo;
			return true;	//�ҵ��ú��߱����
		}
	}
	return false;
}
#include "IntelliAgent\IntelliModel.h"
#ifndef __STATION_
int CorrectNodeFatherRodInfo(NODESET* pNodeSet=NULL,COLINEAR_TOLERANCE* pTolerance=NULL,bool bCorrectColinear=true);	//�����ڵ㸸�˼���ĺ�
int CLDSView::CopyLeg()
{
	CString cmdStr;
	static int cCopyLegQuad;
	static char cCopyLegSchema;	//'A'ȫ������,'P'ֻ�����˼��ڵ�
	static CXhChar16 sCopyLegStr;
	bool bFirstLegQuad=true;
	if(!PRODUCT_FUNC::IsHasCopyLeg())
	{
		AfxMessageBox("��Ȩ֤���в����жԴ���ܵ�ʹ����Ȩ");
		return 0;
	}
	if(cCopyLegQuad<1||cCopyLegQuad>4)
		cCopyLegQuad=1;
	if(sCopyLegStr.GetLength()==0)
		sCopyLegStr.Copy("1");
	if(cCopyLegSchema==0)
		cCopyLegSchema='A';
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Please select the identification number of leg quadrant which will be copied[1/2/3/4]]<%s>:",(char*)sCopyLegStr);
#else 
	cmdStr.Format("��ѡ��ǰҪ�����Ľ������ޱ�ʶ��[1/2/3/4]]<%s>:",(char*)sCopyLegStr);
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			pCmdLine->m_bGettingStrFromCmdLine=FALSE;
			return -1;
		}
		pCmdLine->m_bGettingStrFromCmdLine=FALSE;
		if(cmdStr.GetLength()>0)
			sCopyLegStr.Copy(cmdStr.GetString());
		CXhChar16 copystr=sCopyLegStr;
		for(long cQuad=FindAdaptNo(copystr,",","-");cQuad>0;cQuad=FindAdaptNo(NULL,",","-"))
		{
			if(cQuad<1||cQuad>4)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("Please select key word","");
				pCmdLine->FinishCmdLine();
				cmdStr.Format("Please select the identification number of leg quadrant which will be copied[1/2/3/4]]<%s>:",(char*)sCopyLegStr);
#else 
				pCmdLine->FillCmdLine("��Ҫѡ��ؼ���","");
				pCmdLine->FinishCmdLine();
				cmdStr.Format("��ѡ��ǰҪ�����Ľ������ޱ�ʶ��[1/2/3/4]]<%s>:",(char*)sCopyLegStr);
#endif
				pCmdLine->FillCmdLine(cmdStr,"");
				continue;
			}
		}
		break;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Please select part type of joint leg which will be copied[All(A)/Node & Rod(P)]]<%C>:",cCopyLegSchema);
#else 
	cmdStr.Format("��ѡ��Ҫ�����Ľ��ȹ�������[ȫ��(A)/�ڵ���˼�(P)]]<%C>:",cCopyLegSchema);
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			pCmdLine->m_bGettingStrFromCmdLine=FALSE;
			return -1;
		}
		pCmdLine->m_bGettingStrFromCmdLine=FALSE;
		if(cmdStr.GetLength()>0)
			cCopyLegSchema=toupper(cmdStr[0]);
		if(cCopyLegSchema!='A'&&cCopyLegSchema!='P')
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please select key word","");
			pCmdLine->FinishCmdLine();
			cmdStr.Format("Please select part type of joint leg which will be copied[All(A)/Node & Rod(P)]]<%C>:",cCopyLegSchema);
#else 
			pCmdLine->FillCmdLine("��Ҫѡ��ؼ���","");
			pCmdLine->FinishCmdLine();
			cmdStr.Format("��ѡ��Ҫ�����Ľ��ȹ�������[ȫ��(A)/�ڵ���˼�(P)]]<%C>:",cCopyLegSchema);
#endif
			pCmdLine->FillCmdLine(cmdStr,"");
			continue;
		}
		else
			break;
	}
	//TODO:ʶ��ǰ����
	CLDSModule* pModule=Ta.GetActiveModule();
	if(pModule==NULL)
		return 0;
	CLDSModule* pSrcModule=pModule;
	int nMaxLegs=CFGLEG::MaxLegs();
	int iBodyNo=pModule->GetBodyNo();
	CLogErrorLife life(&logerr);
	//��ȡԭ��:�Ȳ������ڵ���������Vб�������������Ľڵ�
	CLDSPart* pPart;
	CLDSLinePart* pRod;
	CLDSNode *pNode,*pVFrontNode=NULL,*pVSideNode=NULL,*pKTopNode=NULL,*pKBtmNode=NULL;
	//Vб�Ķ���ĩ�˽ڵ㣬һ����pVFrontNode��pVSideNode��ͬ�������������������ʱ�ǲ�ͬ�ġ�wjh-2017.2.23
	CLDSNode *pVFrontTipNode=NULL,*pVSideTipNode=NULL;
	CMinDouble minLegZ;
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(!pModule->IsSonPart(pRod))
			continue;	//���ڵ�ǰ���ߵļ��������
		if(pRod->layer(0)=='L'&&pRod->pStart&&pRod->pEnd)
		{
			minLegZ.Update(pRod->pStart->Position('Z'));
			minLegZ.Update(pRod->pEnd->Position('Z'));
		}
	}
	CXhChar16 copystr=sCopyLegStr;
	CLDSNodePtr legKeyNodes[6]={NULL};
	CUndoOperObject undo(&Ta,true);
	COLINEAR_TOLERANCE tolerance(1,0.001);
	int nCopyLegCounts=0;
	for(cCopyLegQuad=FindAdaptNo(copystr,",","-");cCopyLegQuad>0;cCopyLegQuad=FindAdaptNo(NULL,",","-"))
	{
		if(GetLegKeyNodes(pSrcModule,cCopyLegQuad,pSrcModule->m_arrActiveQuadLegNo[cCopyLegQuad-1],legKeyNodes))
		{
			pVFrontNode=legKeyNodes[0];
			pVSideNode =legKeyNodes[1];
			pKTopNode=legKeyNodes[2];
			pKBtmNode=legKeyNodes[3];
			pVFrontTipNode=legKeyNodes[4];
			pVSideTipNode =legKeyNodes[5];
			g_pSolidDraw->SetEntSnapStatus(pVFrontNode->handle);
			g_pSolidDraw->SetEntSnapStatus(pVSideNode->handle);
			g_pSolidDraw->SetEntSnapStatus(pKTopNode->handle);
			g_pSolidDraw->SetEntSnapStatus(pKBtmNode->handle);
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox(CXhChar50("Joint leg extracts failed,please make sure that input number of joint leg '%d' and structure are correct!",cCopyLegQuad));
#else 
			AfxMessageBox(CXhChar50("������ȡʧ��,��ȷ��������Ⱥ�'%d'���ṹ����ȷ����!",cCopyLegQuad));
#endif
			return 0;
		}
		//TODO:�����Ȳ�������ѡ����ȹ�������
		CHashSet<CLDSNodePtr> legnodes;
		CHashSet<CLDSPartPtr> legparts;
		CHashSet<CLDSNodePtr> hashBoundaryLegacyNodes;	//��ֵΪԭ�ڵ���
		//1.ѡ���Ȳ��ڵ�
		legnodes.SetValue(pVFrontNode->handle,pVFrontNode);
		legnodes.SetValue(pVSideNode->handle,pVSideNode);
		legnodes.SetValue(pKTopNode->handle,pKTopNode);
		legnodes.SetValue(pKBtmNode->handle,pKBtmNode);
		for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			f3dPoint pos=pNode->Position(false);
			//1����ȡ�Ȳ���׼�ڵ�
			if(pNode->IsBodyObj()&&pos.z<=minLegZ.number+EPS)			//�ڵ�λ������
				continue;
			else if(!IsPointInQuad(pNode->Position(false),cCopyLegQuad))
				continue;	//���ڵ�ǰ����
			if(!pSrcModule->IsSonPart(pNode))
				continue;	//���ڵ�ǰ���ߵļ��������
			if(fabs(pos.x)<EPS||fabs(pos.y)<EPS)
				continue;
			legnodes.SetValue(pNode->handle,pNode);
		}
		//����������ƽ����ĸ����޽ǽڵ�(��Ҫ���ڲ��ָ����ں�������ӽڵ�Ϊ���޽ǽڵ��ĵȷֵ�,�粻���λ�ý��޷���ȷ����)
		//�ⲿ�ִ���δдδ, ��һ��Ӧ��Ŀ�������ҵ���Ӧ���ĸ����޽ǽڵ�,�����Ӧ�滻 wjh-2014.12.24
		//CLDSNode* pMirKTopNode=pKTopNode->GetMirNode(MIRMSG(1));
		//legnodes.SetValue(pMirKTopNode->handle,pMirKTopNode);
		//pMirKTopNode=pKTopNode->GetMirNode(MIRMSG(2));
		//legnodes.SetValue(pMirKTopNode->handle,pMirKTopNode);
		//pMirKTopNode=pKTopNode->GetMirNode(MIRMSG(3));
		//legnodes.SetValue(pMirKTopNode->handle,pMirKTopNode);

		//2.ѡ��˼�
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			f3dPoint lineStart,lineEnd;
			lineStart=lineEnd=pPart->ucs.origin;
			if(pPart->IsLinePart())
				pRod=(CLDSLinePart*)pPart;
			else
				pRod=NULL;
			if(pRod&&pRod->pStart&&pRod->pEnd)
			{
				lineStart=pRod->pStart->Position(false);
				lineEnd=pRod->pEnd->Position(false);
			}
			if(pPart->IsBodyObj()||(lineStart.z<=minLegZ.number+EPS&&lineEnd.z<=minLegZ.number+EPS))
				continue;
			else if(!IsPartInQuad(pPart,cCopyLegQuad))
				continue;	//���ڵ�ǰ����
			if(!pSrcModule->IsSonPart(pPart))
				continue;	//���ڵ�ǰ���ߵļ��������
			if(pRod&&pRod->pStart&&legnodes.GetValue(pRod->pStart->handle)==NULL)
			{
				legnodes.SetValue(pRod->pStart->handle,pRod->pStart);
				pRod->pStart->m_cPosCalType=0;	//�߽���޷��ټ������������������ͣ�ֻ�ܰ�ԭʼ�������ӳ��
				hashBoundaryLegacyNodes.SetValue(pRod->pStart->handle,pRod->pStart);
			}
			if(pRod&&pRod->pEnd&&legnodes.GetValue(pRod->pEnd->handle)==NULL)
			{
				legnodes.SetValue(pRod->pEnd->handle,pRod->pEnd);
				pRod->pEnd->m_cPosCalType=0;	//�߽���޷��ټ������������������ͣ�ֻ�ܰ�ԭʼ�������ӳ��
				hashBoundaryLegacyNodes.SetValue(pRod->pEnd->handle,pRod->pEnd);
			}
			legparts.SetValue(pPart->handle,pPart);
		}
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(pPart->IsBodyObj())
				continue;
			if( pPart->GetClassTypeId()==CLS_PLATE&&
				legnodes.GetValue(((CLDSPlate*)pPart)->designInfo.m_hBaseNode))
				legparts.SetValue(pPart->handle,pPart);
			if( pPart->GetClassTypeId()==CLS_BOLT&&
				legparts.GetValue(((CLDSBolt*)pPart)->des_base_pos.hPart))
				legparts.SetValue(pPart->handle,pPart);
		}
		ARRAY_LIST<long> id_arr(0,legnodes.GetNodeNum()+legparts.GetNodeNum());
		for(pNode=legnodes.GetFirst();pNode;pNode=legnodes.GetNext())
			id_arr.append(pNode->handle);
		for(pPart=legparts.GetFirst();pPart;pPart=legparts.GetNext())
			id_arr.append(pPart->handle);
		g_pSolidDraw->SetEntSnapStatus(id_arr.m_pData,id_arr.GetSize());
		CXhChar16 destLegStr;
		int cDestLegQuad=cCopyLegQuad;
		char cDestLegMark,cDestBodyNo=(char)pModule->GetBodyNo();
		if(bFirstLegQuad)
		{	//����Ŀ����Ⱥ�
			bFirstLegQuad=false;
			char cLegBriefMark='A';
			CString listLegMarks;
			destLegStr[0]='@'+pModule->m_arrActiveQuadLegNo[cCopyLegQuad-1]%nMaxLegs;
			char cValidLegMark=destLegStr[0];
			bool hasLegs[26]={false};
			for(int i=1;i<=nMaxLegs;i++)
			{
				if(pModule->m_dwLegCfgWord.And(CFGWORD(iBodyNo,i)))
				{
					if(listLegMarks.GetLength()>0)
						listLegMarks.AppendChar('/');
					listLegMarks.AppendChar('A'+i-1);
					hasLegs[i-1]=true;
					if(destLegStr[0]=='A'+i-1)
						continue;
					if(cValidLegMark==destLegStr[0])
						cValidLegMark='A'+i-1;
					if(cValidLegMark<destLegStr[0]&&'A'+i-1>cValidLegMark)
						cValidLegMark='A'+i-1;
				}
			}
			cDestLegMark=destLegStr[0]=cValidLegMark;
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("Please select the number of joint leg which will be copied[%s]]<%s>:",listLegMarks.GetString(),(char*)destLegStr);
#else 
			cmdStr.Format("��ѡ��ǰҪ��������Ŀ����߽��Ⱥ�[%s]]<%s>:",listLegMarks.GetString(),(char*)destLegStr);
#endif
			CSysPromptObject prompt;
			prompt.Prompt("���ȺŸ�ʽΪ: '�����(����)+���Ⱥ�(��ĸ)+�ָ���(-,#,:)+���޺�(����1~4),ʾ������:\r\n"\
						  "    '2A-3': 2�ź���A���ȵ�3����\r\n"\
						  "    '2A'  : 2�ź���A���ȵĵ�ǰ����\r\n"\
						  "    'A-2' : ��ǰ����A���ȵ�2����\r\n");
			pCmdLine->FillCmdLine(cmdStr,"");
			while(1)
			{
				if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				{
					pCmdLine->m_bGettingStrFromCmdLine=FALSE;
					return -1;
				}
				pCmdLine->m_bGettingStrFromCmdLine=FALSE;
				//if(cmdStr.GetLength()>0&&(toupper(cmdStr[0])>='A'+26 || !hasLegs[toupper(cmdStr[0])-'A']))
				if(cmdStr.GetLength()>0&&!ParseLegMarks(cmdStr))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("Please select key word","");
					pCmdLine->FinishCmdLine();
					cmdStr.Format("Please select the number of joint leg which will be copied[%s]]<%s>:",listLegMarks.GetString(),(char*)destLegStr);
#else 
					pCmdLine->FillCmdLine("��Ҫѡ��ؼ���","");
					pCmdLine->FinishCmdLine();
					cmdStr.Format("��ѡ��ǰҪ��������Ŀ����߽��Ⱥ�[%s]]<%s>:",listLegMarks.GetString(),(char*)destLegStr);
#endif
					pCmdLine->FillCmdLine(cmdStr,"");
					continue;
				}
				else if(cmdStr.GetLength()>0)
				{
					destLegStr.Copy(cmdStr.GetString());
					destLegStr.ToUpper();
				}
				break;
			}
			ParseLegMarks(destLegStr,&cDestBodyNo,&cDestLegMark,&cDestLegQuad);
		}
		CLDSNode *pDestVFrontNode=NULL,*pDestVSideNode=NULL,*pDestKTopNode=NULL,*pDestKBtmNode=NULL;
		if(pModule->GetBodyNo()!=cDestBodyNo)
		{	//������ǰ�������ģ����Ŀ�����ģ��
			for(CLDSModule* pDestModule=Ta.Module.GetFirst();pDestModule;pDestModule=Ta.Module.GetNext())
			{
				if(pDestModule->GetBodyNo()==cDestBodyNo)
				{
					pModule=pDestModule;
					Ta.m_hActiveModule=pModule->handle;
					break;
				}
			}
		}
		pModule->m_arrActiveQuadLegNo[cDestLegQuad-1]=(cDestBodyNo-1)*nMaxLegs+cDestLegMark-'@';
		if(GetLegKeyNodes(pModule,cDestLegQuad,pModule->m_arrActiveQuadLegNo[cDestLegQuad-1],legKeyNodes))
		{
			if(cCopyLegQuad==cDestLegQuad||cCopyLegQuad+cDestLegQuad==5)
			{	//Դ���ݽ���������Ŀ�����������ͬ��ԽǶԳ�
				pDestVFrontNode=legKeyNodes[0];
				pDestVSideNode =legKeyNodes[1];
			}
			else
			{	//Դ���ݽ���������Ŀ���������X���Y��Գ�
				pDestVFrontNode=legKeyNodes[1];
				pDestVSideNode =legKeyNodes[0];
			}
			pDestKTopNode  =legKeyNodes[2];
			pDestKBtmNode  =legKeyNodes[3];
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox(CXhChar50("Joint leg extracts failed,please make sure that input number of joint leg '%d' and structure are correct!",cDestLegQuad));
#else 
			AfxMessageBox(CXhChar50("������ȡʧ��,��ȷ��������Ⱥ�'%d'���ṹ����ȷ����!",cDestLegQuad));
#endif
			return 0;
		}
		//TODO:����ǰ���ȶ����װ����ʱCTowerģ����,�����¼���λ��
		CBuffer clone(4096);
		CTower model;
		CLDSNode* pNewNode;
		CLDSPart* pNewPart;
		for(pNode=legnodes.GetFirst();pNode;pNode=legnodes.GetNext())
			model.Node.append(FALSE)->handle=pNode->handle;
		for(pPart=legparts.GetFirst();pPart;pPart=legparts.GetNext())
			model.Parts.append(pPart->GetClassTypeId(),FALSE)->handle=pPart->handle;
		model.Node.RebuildHashtable();
		model.Parts.RebuildHashtable();
		for(pNode=legnodes.GetFirst();pNode;pNode=legnodes.GetNext())
		{
			clone.ClearContents();
			pNode->ToBuffer(clone);
			clone.SeekToBegin();
			pNewNode=model.FromNodeHandle(pNode->handle);
			pNewNode->FromBuffer(clone);
			if(hashBoundaryLegacyNodes.GetValue(pNode->handle)&&fabs(pNode->Position('Z')-pKTopNode->Position('Z'))<EPS)
			{
				pNewNode->SetPositionZ(pDestKTopNode->Position('Z'));
				double deitaX=pNode->Position('X')-pKTopNode->Position('X');
				double deitaY=pNode->Position('Y')-pKTopNode->Position('Y');
				pNewNode->SetPositionX(pDestKTopNode->Position('X')+deitaX);
				pNewNode->SetPositionY(pDestKTopNode->Position('Y')+deitaY);
			}
			if(pNode->handle==pVFrontNode->handle)
				pVFrontNode=pNewNode;
			else if(pNode->handle==pVSideNode->handle)
				pVSideNode=pNewNode;
			else if(pNode->handle==pKTopNode->handle)
				pKTopNode=pNewNode;
			else if(pNode->handle==pKBtmNode->handle)
				pKBtmNode=pNewNode;
			else if(pNode->handle==pVFrontTipNode->handle)
				pVFrontTipNode=pNewNode;
			else if(pNode->handle==pVSideTipNode->handle)
				pVSideTipNode=pNewNode;
		}
		CHashList<long>hashHandleMapping;		//��ֵΪ�¾��,����Ϊԭ���
		CHashSet<CLDSNode*>hashRelaNodes;		//�������ڵ�ǰ������,��Ϊ��ǰ�����нڵ�Ĺ����ڵ�,��ֵΪԭ���
		for(pPart=legparts.GetFirst();pPart;pPart=legparts.GetNext())
		{
			clone.ClearContents();
			pPart->ToBuffer(clone);
			clone.SeekToBegin();
			pNewPart=model.FromPartHandle(pPart->handle);
			pNewPart->FromBuffer(clone);
		}
		for(pNode=model.Node.GetFirst();pNode;pNode=model.Node.GetNext())
		{
			for(int j=0;j<4;j++)
			{
				if(pNode->arrRelationNode[j]==0)
					continue;
				CLDSNode* pTemporyNode=legnodes.GetValue(pNode->arrRelationNode[j]);
				if(pTemporyNode==NULL)
				{
					if((pTemporyNode=Ta.Node.FromHandle(pNode->arrRelationNode[j]))!=NULL)
						hashRelaNodes.SetValue(pNode->arrRelationNode[j],pTemporyNode);
				}
			}
			long hNew=Ta.GetNewHandle();
			hashHandleMapping.SetValue(hNew,pNode->handle);
			model.ReplaceReferenceHandle(CHandleReplacer(pNode->handle,hNew,NULL,true),CLS_NODE);
			pNode->handle=hNew;
		}
		for(pPart=model.Parts.GetFirst();pPart;pPart=model.Parts.GetNext())
		{
			if(cCopyLegSchema=='P'&&!pPart->IsLinePart())
			{	//���ֿ���ʱӦȥ����Ǹ˼�������ľ������, ���Ƴ��Ǹ˼�����
				model.ReplaceReferenceHandle(CHandleReplacer(pPart->handle,0,NULL,true));
				model.Parts.DeleteCursor();
			}
			else
			{
				long hNew=Ta.GetNewHandle();
				hashHandleMapping.SetValue(hNew,pPart->handle);
				model.ReplaceReferenceHandle(CHandleReplacer(pPart->handle,hNew,NULL,true));
				pPart->handle=hNew;
			}
		}
		//TODO:��CBlockModel�ںϵ���һ�����в�,�ϲ��ؼ��ڵ�͸˼�
		legnodes.Empty();
		legparts.Empty();
		for(pNode=model.Node.GetFirst();pNode;pNode=model.Node.GetNext())
		{
			pNewNode=Ta.Node.append(FALSE);
			pNewNode->handle=pNode->handle;
			if( pNode->handle==pVFrontNode->handle||pNode->handle==pVSideNode->handle||
				pNode->handle==pKTopNode->handle||pNode->handle==pKBtmNode->handle)
				continue;
			legnodes.SetValue(pNewNode->handle,pNewNode);
		}
		Ta.Node.RebuildHashtable();
		for(pPart=model.Parts.GetFirst();pPart;pPart=model.Parts.GetNext())
		{
			pNewPart=Ta.Parts.append(pPart->GetClassTypeId(),FALSE);
			pNewPart->handle=pPart->handle;
			legparts.SetValue(pNewPart->handle,pNewPart);
		}
		Ta.Parts.RebuildHashtable();
		//����ʱ����ģ�����ںϿ������ݶ�����ȫ�ָ���ģ��
		CFGWORD actCfgword(cDestBodyNo,cDestLegMark-'@');
		NODESET nodesNeedCorrect;
		for(pNode=model.Node.GetFirst();pNode;pNode=model.Node.GetNext())
		{
			clone.ClearContents();
			pNode->ToBuffer(clone);
			clone.SeekToBegin();
			pNewNode=Ta.FromNodeHandle(pNode->handle);
			pNewNode->FromBuffer(clone);
			if(pNewNode->IsLegObj())
				pNewNode->cfgword=actCfgword;
			else
			{
				pNewNode->cfgword.Clear();
				pNewNode->cfgword.SetBodyLegs(cDestBodyNo);
			}
			if(cCopyLegQuad!=cDestLegQuad&&isdigit(pNewNode->layer(2)))
				pNewNode->layer(2)='0'+cDestLegQuad;
			long* phSrcNode=hashHandleMapping.GetValue(pNewNode->handle);
			if(phSrcNode==NULL||hashBoundaryLegacyNodes.GetValue(*phSrcNode)==NULL)
				continue;
			nodesNeedCorrect.append(pNewNode);
		}

		for(pPart=model.Parts.GetFirst();pPart;pPart=model.Parts.GetNext())
		{
			clone.ClearContents();
			pPart->ToBuffer(clone);
			clone.SeekToBegin();
			pNewPart=Ta.FromPartHandle(pPart->handle);
			pNewPart->FromBuffer(clone);
			if(pNewPart->IsLegObj())
				pNewPart->cfgword=actCfgword;
			if(cCopyLegQuad!=cDestLegQuad&&isdigit(pNewPart->layer(2)))
				pNewPart->layer(2)='0'+cDestLegQuad;
			if(pNewPart->GetClassTypeId()==CLS_LINEANGLE||pNewPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSLineAngle* pAngle=(CLDSLineAngle*)pNewPart;
				if(cCopyLegQuad+cDestLegQuad==5)	//���޶ԽǶԳƿ�������
				{
					pAngle->des_norm_x.near_norm.x*=-1;
					pAngle->des_norm_x.near_norm.y*=-1;
					pAngle->des_norm_y.near_norm.x*=-1;
					pAngle->des_norm_y.near_norm.y*=-1;
					f3dPoint start=pAngle->Start();
					f3dPoint end=pAngle->End();
					start.Set(-start.x,-start.y,start.z);
					end.Set(-end.x,-end.y,end.z);
					pAngle->SetStart(start);
					pAngle->SetEnd(end);
					CalMirAtomLayer(pAngle->layer(),pAngle->layer(),MIRMSG(3));
					if(!pAngle->des_norm_x.bSpecific&&!pAngle->des_norm_x.bByOtherWing)
					{
						CLDSLinePart* pRod=Ta.FromRodHandle(pAngle->des_norm_x.hViceJg);
						if(pRod)
							pRod=pRod->GetMirRod(MIRMSG(3));
						if(pRod)
							pAngle->des_norm_x.hViceJg=pRod->handle;
					}
					if(!pAngle->des_norm_y.bSpecific&&!pAngle->des_norm_y.bByOtherWing)
					{
						CLDSLinePart* pRod=Ta.FromRodHandle(pAngle->des_norm_y.hViceJg);
						if(pRod)
							pRod=pRod->GetMirRod(MIRMSG(3));
						if(pRod)
							pAngle->des_norm_y.hViceJg=pRod->handle;
					}
				}
				else if(cCopyLegQuad!=cDestLegQuad)	//����X/Y��Գƿ�������
				{
					MIRMSG mirmsg;
					mirmsg.axis_flag=0x08;
					if(cCopyLegQuad==1&&cDestLegQuad==2)
						mirmsg.rotate_angle= 90;
					else if(cCopyLegQuad==2&&cDestLegQuad==1)
						mirmsg.rotate_angle=-90;
					else if(cCopyLegQuad==1&&cDestLegQuad==3)
						mirmsg.rotate_angle=-90;
					else if(cCopyLegQuad==3&&cDestLegQuad==1)
						mirmsg.rotate_angle= 90;
					else if(cCopyLegQuad==2&&cDestLegQuad==4)
						mirmsg.rotate_angle= 90;
					else if(cCopyLegQuad==4&&cDestLegQuad==2)
						mirmsg.rotate_angle=-90;
					else if(cCopyLegQuad==3&&cDestLegQuad==4)
						mirmsg.rotate_angle=-90;
					else if(cCopyLegQuad==4&&cDestLegQuad==3)
						mirmsg.rotate_angle= 90;
					CalMirAtomLayer(pAngle->layer(),pAngle->layer(),mirmsg);
					f3dPoint start=pAngle->Start();
					f3dPoint end=pAngle->End();
					start=GetMirPos(start,mirmsg);
					end=GetMirPos(end,mirmsg);
					pAngle->SetStart(start);
					pAngle->SetEnd(end);
					pAngle->des_norm_x.near_norm=GetMirVector(pAngle->des_norm_x.near_norm,mirmsg);
					pAngle->des_norm_y.near_norm=GetMirVector(pAngle->des_norm_y.near_norm,mirmsg);
					if(!pAngle->des_norm_x.bSpecific&&!pAngle->des_norm_x.bByOtherWing)
					{
						CLDSLinePart* pRod=Ta.FromRodHandle(pAngle->des_norm_x.hViceJg);
						if(pRod)
							pRod=pRod->GetMirRod(mirmsg);
						if(pRod)
							pAngle->des_norm_x.hViceJg=pRod->handle;
					}
					if(!pAngle->des_norm_y.bSpecific&&!pAngle->des_norm_y.bByOtherWing)
					{
						CLDSLinePart* pRod=Ta.FromRodHandle(pAngle->des_norm_y.hViceJg);
						if(pRod)
							pRod=pRod->GetMirRod(mirmsg);
						if(pRod)
							pAngle->des_norm_y.hViceJg=pRod->handle;
					}
				}
			}
		}
		//����߽�ڵ�ĸ��˼�
		if(nodesNeedCorrect.GetNodeNum()>0)
			CorrectNodeFatherRodInfo(&nodesNeedCorrect,&tolerance);
		Ta.ReplaceReferenceHandle(CHandleReplacer(pVFrontNode->handle,pDestVFrontNode->handle));
		Ta.ReplaceReferenceHandle(CHandleReplacer(pVSideNode->handle,pDestVSideNode->handle));
		Ta.ReplaceReferenceHandle(CHandleReplacer(pKTopNode->handle,pDestKTopNode->handle));
		Ta.ReplaceReferenceHandle(CHandleReplacer(pKBtmNode->handle,pDestKBtmNode->handle));
		Ta.Node.DeleteNode(pVFrontNode->handle);
		Ta.Node.DeleteNode(pVSideNode->handle);
		Ta.Node.DeleteNode(pKTopNode->handle);
		Ta.Node.DeleteNode(pKBtmNode->handle);
		for(pNode=legnodes.GetFirst();pNode;pNode=legnodes.GetNext())
		{
			pNode->Set();
			CLDSNode* pOverlapNode=Ta.CheckOverlappedNode(pNode->Position(false),pModule);
			if(pOverlapNode&&pOverlapNode!=pNode)
			{
				Ta.ReplaceReferenceHandle(CHandleReplacer(pNode->handle,pOverlapNode->handle));
				Ta.Node.DeleteNode(pNode->handle);
				legnodes.DeleteCursor();
			}
			else
				pNode->relativeObjs.Empty();
		}
		for(pPart=legparts.GetFirst();pPart;pPart=legparts.GetNext())
		{
			if(pPart->IsLinePart()&&((CLDSLinePart*)pPart)->pStart!=NULL&&((CLDSLinePart*)pPart)->pEnd!=NULL)
			{
				CLDSLinePart* pOverlapRod=Ta.CheckOverLappedLinePart(((CLDSLinePart*)pPart)->pStart->Position(false),
					((CLDSLinePart*)pPart)->pEnd->Position(false),pModule);
				if(pOverlapRod&&pOverlapRod!=pPart)
				{
					pRod=(CLDSLinePart*)pPart;
					for(CLsRef* pLsRef=pRod->GetFirstLsRef();pLsRef;pLsRef=pRod->GetNextLsRef())
						pOverlapRod->AppendMidLsRef(*pLsRef);
					Ta.ReplaceReferenceHandle(CHandleReplacer(pPart->handle,pOverlapRod->handle));
					Ta.Parts.DeleteNode(pPart->handle);
					legparts.DeleteCursor();
					continue;
				}
				else
					pPart->relativeObjs.Empty();
			}
			else
				pPart->relativeObjs.Empty();
			if(pPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				CLDSLineAngle *pAngle = (CLDSLineAngle*)pPart;
				pAngle->ClearFlag();
				pAngle->CalPosition();
				pAngle->cal_x_wing_norm();
				pAngle->cal_y_wing_norm();
				//��������Ǹ�����ͷ,���ܻ���Ϊ��ؼ���������˨δ���µ�������ͷ������� �ʳٺ����� wjh-2014.12.19
				if(pAngle->desStartOdd.m_iOddCalStyle==0)
					pAngle->CalStartOddment();
				if(pAngle->desEndOdd.m_iOddCalStyle==0)
					pAngle->CalEndOddment();
				//pAngle->SetModified();
				//pAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				//g_pSolidDraw->NewSolidPart(pAngle->GetSolidPartObject());
			}
			else if(pPart->GetClassTypeId()==CLS_LINETUBE)
			{
				CLDSLineTube *pTube = (CLDSLineTube*)pPart;
				pTube->ClearFlag();
				pTube->CalWorkPlaneNorm();
				pTube->CalPosition();
				pTube->DesignStartJoint();
				pTube->DesignEndJoint();
				pTube->CalStartOddment();
				pTube->CalEndOddment();
				pTube->SetModified();
				pTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pTube->GetSolidPartObject());
			}
			else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
			{
				CLDSLineFlat *pFlat = (CLDSLineFlat*)pPart;
				pFlat->ClearFlag();
				pFlat->CalPosition();
				pFlat->CalWorkPlaneNorm();
				pFlat->CalStartOddment();
				pFlat->CalEndOddment();
				pFlat->SetModified();
				pFlat->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				g_pSolidDraw->NewSolidPart(pFlat->GetSolidPartObject());
			}
			else if(pPart->GetClassTypeId()==CLS_LINESLOT)
			{
				CLDSLineSlot *pSlot = (CLDSLineSlot*)pPart;
				pSlot->ClearFlag();
				pSlot->CalPosition();
				pSlot->CalWorkPlaneNorm();
				pSlot->CalStartOddment();
				pSlot->CalEndOddment();
				pSlot->SetModified();
				pSlot->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				g_pSolidDraw->NewSolidPart(pSlot->GetSolidPartObject());
			}
			else if(pPart->GetClassTypeId()==CLS_BOLT)
			{
				CLDSBolt *pBolt = (CLDSBolt*)pPart;
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				pBolt->CalGuigeAuto();
				if(!pBolt->m_bVirtualPart)
				{	//��������˨����Ҫ��ʾ��˨ʵ�� wht 11-07-08
					pBolt->SetModified();
					pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}
			else if(pPart->GetClassTypeId()==CLS_PLATE)
			{
				CLDSPlate *pPlate = (CLDSPlate*)pPart;
				pPlate->DesignPlate();
			}
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				CLDSParamPlate *pPlate = (CLDSParamPlate*)pPart;
				pPlate->DesignPlate();
			}
		}
		for(pPart=legparts.GetFirst();pPart;pPart=legparts.GetNext())
		{
			if(pPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				CLDSLineAngle *pAngle = (CLDSLineAngle*)pPart;
				if(pAngle->desStartOdd.m_iOddCalStyle==1)
					pAngle->CalStartOddment();
				if(pAngle->desEndOdd.m_iOddCalStyle==1)
					pAngle->CalEndOddment();
				pAngle->SetModified();
				pAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				g_pSolidDraw->NewSolidPart(pAngle->GetSolidPartObject());
			}
		}
		nCopyLegCounts++;
	}
	CLDSModule* pCurrActiveModule=Ta.GetActiveModule();
	CMainFrame* pMainWnd=(CMainFrame*)theApp.m_pMainWnd;
	CTowerTreeDlg* pModelPage=pMainWnd->GetTowerTreePage();
	pModelPage->ShiftActiveItemByObj(pModule);	//����ģ�����е�ǰ���߽ڵ�ļ���״̬
	RedrawAll();
	//TODO:�������ģ���е����нڵ㼰��������
#ifdef AFX_TARG_ENU_ENGLISH
	if(nCopyLegCounts>0)
		MessageBox("Joint leg copy success!");
	else
		AfxMessageBox("Joint leg copy failed.Please check identification number of leg quadrant");
#else 
	if(nCopyLegCounts>0)
		MessageBox("���ȿ����ɹ�!");
	else
		AfxMessageBox("���ȿ���ʧ��, ���ʵ�Ƿ�'�������ޱ�ʶ��'�����Ƿ���ȷ");
#endif
	return 0;
}
#endif

int	CLDSView::NewManuTowerModel()
{
#ifndef __STATION_
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	pCmdLine->FillCmdLine("NewTower:","");
	if(!GetDocument()->OnNewDocument())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FinishCmdLine("Dcoument create failed");
#else
		pCmdLine->FinishCmdLine("�ĵ�����ʧ��");
#endif
		return 0;
	}
	//������ģ�㷨���ɵ�CShankDefineToTowerModel::UpdateTowerModel(CTower* pTower,CFrameDefine *pDefine,int nPoly=4); wxc-2019.3.21
	CNewTowerDataOper oper(CNewTowerDataOper::NEW_UI);
	if(oper.RunNewTowerUi())
	{
		//ˢ�������ͼ�����ǰ���ͼ
		CTowerTreeDlg *pTreeView = (CTowerTreeDlg*)((CMainFrame*)AfxGetMainWnd())->GetTowerTreePage();
		pTreeView->RefreshTreeCtrl();
		//ˢ�¹�����ʾ��
		console.Display(NULL);
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FinishCmdLine();
#else
		pCmdLine->FinishCmdLine();
#endif
	}
	else
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FinishCmdLine("Canceled");
#else
		pCmdLine->FinishCmdLine("ȡ��");
#endif
#endif
	return 0;
}

bool CorrectSingleAngleLayout(CLDSLineAngle* pJg,bool updatePartSolid=true);	//defined in LDSView.cpp
int CLDSView::NewDesignTowerModel()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	pCmdLine->FillCmdLine("NewTower:","");
	if(!GetDocument()->OnNewDocument())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FinishCmdLine("Dcoument create failed");
#else
		pCmdLine->FinishCmdLine("�ĵ�����ʧ��");
#endif
		return 0;
	}
#if defined(__LDS_)||defined(__TSA_)||defined(__TDA_)
	CNewTowerDlg dlg;
	if(dlg.DoModal()!=IDOK)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FinishCmdLine("Canceled");
#else
		pCmdLine->FinishCmdLine("ȡ��");
#endif
		return 0;
	}
	int nRetCode=dlg.m_xShankDef.ValidCheck();
	if(nRetCode<0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FinishCmdLine("Dcoument create failed");
#else
		pCmdLine->FinishCmdLine("�ĵ�����ʧ��");
#endif
		return 0;
	}
	try
	{
		int i=0,idClassType=CLS_LINEANGLE;
		if(dlg.m_iMainPartType==0)
			idClassType=CLS_LINETUBE;
		int nPoly=3;
		if(dlg.m_iSideNum<4)
			nPoly=3+dlg.m_iSideNum;
		else if(dlg.m_iSideNum==4)
			nPoly=8;
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FinishCmdLine("Dcoument create failed");
#else
			pCmdLine->FinishCmdLine("�ĵ�����ʧ��");
#endif
			return 0;
		}
		/*CString sJgZhunJuTblPath;
		sJgZhunJuTblPath.Format("%sTower.jgg",APP_PATH);
		FILE *fp;
		char file_path[MAX_PATH];
		sprintf(file_path,"%s",sJgZhunJuTblPath);
		if((fp=fopen(file_path,"rt")))
		{	//����ϵͳĿ¼���Ƿ����Tower.jgg
			fclose(fp);	//�ر�
			JgZhunJuSerialize(sJgZhunJuTblPath,FALSE); //����Tower.jgg�е�����
		}
		else
		{
			for(i=0;i<18;i++)
			{   //ʹ��ԭʼ�Ǹ�׼����ʼ���Ǹ�׼��� 
				zhunju_table[i][0]=original_zhunju_table[i][0];
				zhunju_table[i][1]=original_zhunju_table[i][1];
				zhunju_table[i][2]=original_zhunju_table[i][2];
				zhunju_table[i][3]=original_zhunju_table[i][3];
				zhunju_table[i][4]=original_zhunju_table[i][4];
			}
		}*/
		CLogErrorLife life;
		CFGLEG::SetSchema(dlg.m_iMultiLegSchema+1);
		BOOL bPreferAngleSimplePosMode=g_sysPara.m_bAnglePreferSimplePosMode;
		g_sysPara.m_bAnglePreferSimplePosMode=FALSE;
		CShankDefineToTowerModel modeler;
		modeler.UpdateTowerModel(&Ta, &dlg.m_xShankDef, nPoly);
		g_sysPara.m_bAnglePreferSimplePosMode=bPreferAngleSimplePosMode;
		//TODO:Ӧ�Զ�ִ�нǸְڷ�λ������
		for(CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pAngle;pAngle=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
		{
			if(pAngle->layer(0)!='T'&&pAngle->layer(1)=='Z')
				continue;	//������Ȳ����Ĳ�����
			CorrectSingleAngleLayout(pAngle);
		}
		//�л���������ʾ״̬��δ��λ֮ǰ��ʾʵ����ܻ���ʾ����������� 
		g_pSolidSet->SetDisplayType(DISP_LINE);
		//ȫ����λ
		AutoDaoWei();
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
	//ˢ�������ͼ�����ǰ���ͼ
	CTowerTreeDlg *pTreeView = (CTowerTreeDlg*)((CMainFrame*)AfxGetMainWnd())->GetTowerTreePage();
	pTreeView->RefreshTreeCtrl();
	//ˢ�¹�����ʾ��
	console.Display(NULL);
#ifdef __CONNECT_REMOTE_SERVER_
	if(theApp.m_bNetModelWork)
	{
		pTreeView->NewOrCloneTowerFile(NULL);	//�½������ļ����ڵ�
		CServerTowerFile *pTowerFile=console.GetActiveTowerFile();
		if(pTowerFile)
		{
			pTowerFile->m_dataBuff.ClearBuffer();
			GetDocument()->SerializeTowerFileBuffer(pTowerFile->m_dataBuff,TRUE);
		}
	}
#endif
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FinishCmdLine();
#else
	pCmdLine->FinishCmdLine();
#endif
	return 0;
}

long GetGlobalNewHandle()
{
	return Ta.GetNewHandle();
}
CLDSNode *AppendNode(BOOL bGetNewHandle)
{
	return Ta.AppendNode(bGetNewHandle);
}
CLDSPart *AppendPart(int CLASS_TYPEID,BOOL bGetNewHandle)
{
	return Ta.AppendPart(CLASS_TYPEID,bGetNewHandle);
}

#include "BlockTempl.h"
static BOOL fbCreateHumanPost=TRUE;	//Ĭ���������ס
static BOOL fbCreateBeam=TRUE;		//Ĭ����ƺ���
int CLDSView::NewSubstationModel()
{
#if defined(__COMPLEX_PART_INC_) && defined(__LMA_FILE_)
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	if(!VerifyValidFunction(LICFUNC::FUNC_IDENTITY_SUBSTATION) || !GetDocument()->OnNewDocument())
	{
		pCmdLine->CancelCmdLine();
		return 0;
	}
	CLogErrorLife life(&logerr);
	CParaBlockModel model;
	CSubstationTemplate para_template;
	model.m_pTemplate=&para_template;
	CNewSubstationDlg dlg;
	dlg.m_bDesignPost=fbCreateHumanPost;
	dlg.m_bDesignBeam=fbCreateBeam;
	dlg.m_pModel=&model;
	dlg.m_bWizardDialog=TRUE;
	if(dlg.DoModal()!=IDOK)
	{
		pCmdLine->CancelCmdLine();
		return 0;
	}
	else
	{
		fbCreateHumanPost=dlg.m_bDesignPost;
		fbCreateBeam=dlg.m_bDesignBeam;
	}
	model.GetNewHandleHookFunc=GetGlobalNewHandle;
	model.RebuildModel();
	model.BuildAcs();
	CDisplayView* pBackView=Ta.View.GetByIndex(3);
	for(int i=0;i<15;i++)
	{
		if(pBackView->filter.ThirdCharSet[i]==0)
		{
			pBackView->filter.ThirdCharSet[i]='1';	//������Ϊǰ����ͼ���ɼ�
			pBackView->filter.ThirdCharSet[i+1]=0;
			break;
		}
	}
	KEY4C* pKey;
	DESIGN_PARAM_ITEM *pItem;
	for(pKey=model.listGlobalParams.EnumObjectFirst();pKey;pKey=model.listGlobalParams.EnumObjectNext())
	{
		if((pItem=model.listParams.GetValue(*pKey))!=NULL)
			Ta.hashParams.SetValue(pItem->key,*pItem);
	}
	SLAVE_NODE* pSlaveNode;
	for(pKey=model.listControlNodes.EnumObjectFirst();pKey;pKey=model.listControlNodes.EnumObjectNext())
	{
		CParaModelNode* pParaNode=model.GetParaNode(*pKey);
		if(pParaNode&&pParaNode->TagInfo()&&pParaNode->TagInfo()->pModelNode)
		{
			pSlaveNode=Ta.hashSlaveNodes.Add(pParaNode->TagInfo()->pModelNode->handle);
			if(strlen(pParaNode->posx_expression)>0)
			{
				StrCopy(pSlaveNode->posx_expr,pParaNode->posx_expression,20);
				pSlaveNode->SetControlState('X');
			}
			if(strlen(pParaNode->posy_expression)>0)
			{
				StrCopy(pSlaveNode->posy_expr,pParaNode->posy_expression,20);
				pSlaveNode->SetControlState('Y');
			}
			if(strlen(pParaNode->posz_expression)>0)
			{
				StrCopy(pSlaveNode->posz_expr,pParaNode->posz_expression,20);
				pSlaveNode->SetControlState('Z');
			}
		}
	}
	CLDSNode* pTemNode=NULL;
	for(CLDSNode* pNode=model.Node.GetFirst();pNode;pNode=model.Node.GetNext())
	{	
		pNode->TransToCS(model.acs);
		pTemNode=Ta.Node.append();
		pTemNode->handle=pNode->handle;
		pTemNode->SetPosition(pNode->Position());
		pTemNode->CopyProperty(pNode);
		pTemNode->hFatherPart=pNode->hFatherPart;
		for(int i=0;i<4;i++)
			pTemNode->arrRelationNode[i]=pNode->arrRelationNode[i];
		NewNode(pTemNode);
	}
	Ta.Node.RebuildHashtable();
	CLDSLinePart *pLinePart, *pTemLinePart=NULL;
	for(pLinePart=model.EnumRodFirst();pLinePart;pLinePart=model.EnumRodNext())
	{
		pLinePart->TransToCS(model.acs);
		pTemLinePart=(CLDSLinePart*)Ta.AppendPart(pLinePart->GetClassTypeId());
		pTemLinePart->handle=pLinePart->handle;
		CLDSNode* pStartNode=Ta.FromNodeHandle(pLinePart->pStart->handle);
		CLDSNode* pEndNode=Ta.FromNodeHandle(pLinePart->pEnd->handle);
		pTemLinePart->CopyProperty(pLinePart);
		pTemLinePart->pStart=pStartNode;
		pTemLinePart->SetStart(pStartNode->Position());
		pTemLinePart->pEnd=pEndNode;
		pTemLinePart->SetEnd(pEndNode->Position());
		{
		CLdsPartListStack stack(&Ta.Parts);
		if(pTemLinePart->IsAngle(FALSE))
		{
			((CLDSLineAngle*)pTemLinePart)->set_norm_x_wing(((CLDSLineAngle*)pLinePart)->vxWingNorm);
			((CLDSLineAngle*)pTemLinePart)->set_norm_y_wing(((CLDSLineAngle*)pLinePart)->vyWingNorm);
			((CLDSLineAngle*)pTemLinePart)->des_norm_x=((CLDSLineAngle*)pLinePart)->des_norm_x;
			((CLDSLineAngle*)pTemLinePart)->des_norm_y=((CLDSLineAngle*)pLinePart)->des_norm_y;
			if(pTemLinePart->Layer(2)!='G')
				((CLDSLineAngle*)pTemLinePart)->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
			((CLDSLineAngle*)pTemLinePart)->connectStart.m_iConnectWing=1;
			((CLDSLineAngle*)pTemLinePart)->connectStart.wnConnBoltN=2;
			((CLDSLineAngle*)pTemLinePart)->connectEnd.m_iConnectWing=1;
			((CLDSLineAngle*)pTemLinePart)->connectEnd.wnConnBoltN=2;
		}
		else if(pTemLinePart->IsTube(FALSE))
		{
			f3dPoint work_norm=((CLDSLineTube*)pLinePart)->WorkPlaneNorm();
			((CLDSLineTube*)pTemLinePart)->SetWorkPlaneNorm(work_norm);
			((CLDSLineTube*)pTemLinePart)->m_iNormCalStyle=((CLDSLineTube*)pLinePart)->m_iNormCalStyle;
			((CLDSLineTube*)pTemLinePart)->hWorkNormRefPart=((CLDSLineTube*)pLinePart)->hWorkNormRefPart;
			if(((CLDSLineTube*)pTemLinePart)->m_iNormCalStyle==2)
			{
				for(int i=0;i<3;i++)
				{
					((CLDSLineTube*)pTemLinePart)->pickWorkPlane[i]=((CLDSLineTube*)pLinePart)->pickWorkPlane[i];
					//��ʼ����ʰȡ��ʱ��Ĭ����Ϊ(0��0��0�����ڴ˽������� wjh-2016.10.10
					((CLDSLineTube*)pTemLinePart)->pickWorkPlane[i]+=((double*)&model.acs.origin.x)[i];
				}
			}
		}
	#ifdef __PART_DESIGN_INC_
		pTemLinePart->m_iElemType=pLinePart->m_iElemType;
	#endif
		NewLinePart(pTemLinePart);
		}
	}
	for(pLinePart=Ta.EnumRodFirst();pLinePart;pLinePart=Ta.EnumRodNext())
	{
		if(pLinePart->IsAngle())
		{
			((CLDSLineAngle*)pLinePart)->cal_x_wing_norm(true);
			((CLDSLineAngle*)pLinePart)->cal_y_wing_norm(true);
			CorrectSingleAngleLayout((CLDSLineAngle*)pLinePart);
		}
		else if(pLinePart->IsTube())
			((CLDSLineTube*)pLinePart)->CalWorkPlaneNorm();
	}
	CLDSParamPlate* pParamPlate=NULL,* pTemParamPlate=NULL;
	for(pParamPlate=(CLDSParamPlate*)model.Parts.GetFirst(CLS_PARAMPLATE);pParamPlate;pParamPlate=(CLDSParamPlate*)model.Parts.GetNext(CLS_PARAMPLATE))
	{
		pTemParamPlate=(CLDSParamPlate*)Ta.AppendPart(CLS_PARAMPLATE);
		pTemParamPlate->handle=pParamPlate->handle;
		pTemParamPlate->CopyProperty(pParamPlate);
		if(pTemParamPlate->m_iParamType==TYPE_BASEPLATE)
		{	//�����������ԭ��
			f3dPoint origin;
			long hDatunNode=0,hDatunTube=0;
			pTemParamPlate->GetDesignItemValue('A',&hDatunNode);
			CLDSNode* pNode=Ta.FromNodeHandle(hDatunNode);
			if(pNode)
				origin=pNode->Position(true);
			pTemParamPlate->design.origin.SetPosition(origin);
			pTemParamPlate->GetDesignItemValue('L',&hDatunTube);
			CLDSLineTube* pBaseTube=(CLDSLineTube*)Ta.FromPartHandle(hDatunTube,CLS_LINETUBE);
			if(pBaseTube)
			{	//�����ֹ�����ͷ
				if(pBaseTube->pStart->handle=pNode->handle)
					pBaseTube->SetStartOdd(-1000);
				else
					pBaseTube->SetEndOdd(-1000);		
			}
			//����ĸ�����Ϣ
			ATTACHPART_ITEM* pItem=NULL,*pTemItem=NULL;
			for(ATTACHPART_ITEM* pItem=pParamPlate->AttachPart.GetFirst();pItem;pItem=pParamPlate->AttachPart.GetNext())
			{
				pTemItem=pTemParamPlate->AttachPart.Add(pItem->h);
				pTemItem->idClsType=pItem->idClsType;
			}
		}
		else if(pTemParamPlate->m_iParamType==TYPE_STATIONLINKPLATE)
		{	//�˳��м����Ӱ壨�ԽǸֽ�����˨���ã�
			long hDatunNode=0,hBaseTube=0,hAssistJg1=0,hAssistJg2=0;
			pTemParamPlate->GetDesignItemValue('A',&hBaseTube);
			pTemParamPlate->GetDesignItemValue('B',&hDatunNode);
			pTemParamPlate->GetDesignItemValue('C',&hAssistJg1);
			pTemParamPlate->GetDesignItemValue('D',&hAssistJg2);
			CLDSNode* pNode=Ta.FromNodeHandle(hDatunNode);
			CLDSLineTube* pBaseTube=(CLDSLineTube*)Ta.FromPartHandle(hBaseTube,CLS_LINETUBE);
			CLDSLineAngle* pAssistJg1=(CLDSLineAngle*)Ta.FromPartHandle(hAssistJg1,CLS_LINEANGLE);
			CLDSLineAngle* pAssistJg2=(CLDSLineAngle*)Ta.FromPartHandle(hAssistJg2,CLS_LINEANGLE);
			f3dPoint norm(0,0,1);
			long iAxisZType=0;
			pTemParamPlate->GetDesignItemValue('Z',&iAxisZType);
			norm=CStdVector::GetVector(iAxisZType);
			double radius=pBaseTube->GetDiameter()*0.5;
			for(int i=0;i<2;i++)
			{
				CLDSLineAngle* pAngle=(i==0)?pAssistJg1:pAssistJg2;
				if(pAngle==NULL)
					continue;
				//�������ӽǸֵ�֫����
				BYTE biWorkWingX0_Y1=0;
				pAngle->des_norm_x.hViceJg=pAngle->des_norm_y.hViceJg=0;
				double ddx=pAngle->vxWingNorm*norm;
				double ddy=pAngle->vyWingNorm*norm;
				if(fabs(ddx)>fabs(ddy))
				{	//X֫Ϊ����֫
					biWorkWingX0_Y1=0;
					pAngle->des_norm_x.bSpecific=TRUE;
					pAngle->des_norm_x.spec_norm.vector=ddx>0?norm:(norm*-1);
					pAngle->des_norm_y.bByOtherWing=TRUE;
				}
				else
				{	//Y֫Ϊ����֫
					biWorkWingX0_Y1=1;
					pAngle->des_norm_y.bSpecific=TRUE;
					pAngle->des_norm_y.spec_norm.vector=ddy>0?norm:(norm*-1);
					pAngle->des_norm_x.bByOtherWing=TRUE;
				}
				pAngle->cal_x_wing_norm();
				pAngle->cal_y_wing_norm();
				//�������ӽǸֵ�����ͷ
				BYTE biLinkS0_E1=0;
				if(pAngle->pEnd->handle==hDatunNode)
					biLinkS0_E1=1;
				if(biLinkS0_E1==0)
				{
					pAngle->desStartOdd.m_iOddCalStyle=0;	//������ײ��������ͷ
					pAngle->desStartOdd.m_hRefPart1=hBaseTube;
					pAngle->desStartOdd.m_hRefPart2=0;
					pAngle->desStartOdd.m_fCollideDist=g_sysPara.VertexDist;
					pAngle->CalStartOddment();
				}
				else //if(biLinkS0_E1==1)
				{
					pAngle->desEndOdd.m_iOddCalStyle=0;	//������ײ��������ͷ
					pAngle->desEndOdd.m_hRefPart1=hBaseTube;
					pAngle->desEndOdd.m_hRefPart2=0;
					pAngle->desEndOdd.m_fCollideDist=g_sysPara.VertexDist;
					pAngle->CalEndOddment();
				}
				if(biLinkS0_E1==0&&biWorkWingX0_Y1==0)
				{
					pAngle->desStartPos.wing_x_offset.gStyle=4;
					pAngle->desStartPos.wing_x_offset.offsetDist=-pTemParamPlate->Thick*0.5;
				}
				else if(biLinkS0_E1==0&&biWorkWingX0_Y1==1)
				{
					pAngle->desStartPos.wing_y_offset.gStyle=4;
					pAngle->desStartPos.wing_y_offset.offsetDist=-pTemParamPlate->Thick*0.5;
				}
				if(biLinkS0_E1==1&&biWorkWingX0_Y1==0)
				{
					pAngle->desEndPos.wing_x_offset.gStyle=4;
					pAngle->desEndPos.wing_x_offset.offsetDist=-pTemParamPlate->Thick*0.5;
				}
				if(biLinkS0_E1==1&&biWorkWingX0_Y1==1)
				{	
					pAngle->desEndPos.wing_y_offset.gStyle=4;
					pAngle->desEndPos.wing_y_offset.offsetDist=-pTemParamPlate->Thick*0.5;
				}
				pAngle->CalPosition(true);
				pAngle->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pAngle->GetSolidPartObject());
				//��������˨
				JGZJ jgzj;
				CConnectInfo ls_group;
				LSSPACE_STRU LsSpace;
				biWorkWingX0_Y1==0?jgzj=pAngle->GetZhunJu('X'):jgzj=pAngle->GetZhunJu('Y');
				biLinkS0_E1==0?ls_group=pAngle->connectStart:ls_group=pAngle->connectEnd;
				GetLsSpace(LsSpace,ls_group.d);
				CLDSPoint datumPoint;
				datumPoint.datum_pos_style=1;
				datumPoint.des_para.RODEND.hRod=pAngle->handle;
				datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
				datumPoint.des_para.RODEND.direction=biLinkS0_E1;
				datumPoint.des_para.RODEND.offset_wing=biWorkWingX0_Y1;
				datumPoint.des_para.RODEND.wing_offset_dist=0;
				datumPoint.des_para.RODEND.wing_offset_style=4;
				int offset=LsSpace.EndSpace;
				for(int j=0;j<ls_group.wnConnBoltN;j++)
				{
					CLDSBolt *pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					pRealLs->des_base_pos.hPart=pAngle->handle;
					pRealLs->des_base_pos.datumPoint=datumPoint;
					pRealLs->des_base_pos.offset_wing=biWorkWingX0_Y1;
					pRealLs->des_base_pos.direction=biLinkS0_E1;
					pRealLs->des_base_pos.len_offset_dist=offset;
					pRealLs->des_base_pos.wing_offset_dist=jgzj.g;
					_snprintf(pRealLs->des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-pAngle->GetThick());
					pRealLs->des_work_norm.norm_style=1;	//ָ���Ǹ�֫����	
					pRealLs->des_work_norm.direction=0;
					pRealLs->des_work_norm.hVicePart = pAngle->handle;
					pRealLs->des_work_norm.norm_wing=biWorkWingX0_Y1;
					offset += LsSpace.SingleRowSpace;
					//
					if(biLinkS0_E1)
						pAngle->AppendStartLsRef(pRealLs->GetLsRef());
					else
						pAngle->AppendEndLsRef(pRealLs->GetLsRef());
					pTemParamPlate->AppendLsRef(pRealLs->GetLsRef());
					pRealLs->AddL0Thick(pAngle->handle,TRUE);
					pRealLs->AddL0Thick(pTemParamPlate->handle,TRUE);
					pRealLs->CalGuigeAuto();
					pRealLs->correct_worknorm();
					pRealLs->correct_pos();
				}
			}
		}
		else if(pTemParamPlate->m_iParamType==TYPE_STATIONNODEPLATE)
		{	//�������м����Ӱ壨�ԽǸֽ�����˨���ã�
			long hDatunNode=0,hBaseTube=0,hLinkJg=0;
			pTemParamPlate->GetDesignItemValue('A',&hDatunNode);
			pTemParamPlate->GetDesignItemValue('B',&hBaseTube);
			pTemParamPlate->GetDesignItemValue('C',&hLinkJg);
			CLDSNode* pBaseNode=Ta.FromNodeHandle(hDatunNode);
			CLDSLineTube* pBaseTube=(CLDSLineTube*)Ta.FromPartHandle(hBaseTube,CLS_LINETUBE);
			CLDSLineAngle* pLinkJg=(CLDSLineAngle*)Ta.FromPartHandle(hLinkJg,CLS_LINEANGLE);
			f3dPoint norm(0,0,1);
			long iAxisZType=0;
			pTemParamPlate->GetDesignItemValue('Z',&iAxisZType);
			norm=CStdVector::GetVector(iAxisZType);
			if(pLinkJg&&pBaseTube)
			{	//����Ǹ�����ͷ
				BYTE biWorkWingX0_Y1=0,biLinkS0_E1=0;
				if(fabs(pLinkJg->vxWingNorm*norm)<fabs(pLinkJg->vyWingNorm*norm))
					biWorkWingX0_Y1=1;
				if(pLinkJg->pEnd->handle==hDatunNode)
					biLinkS0_E1=1;
				if(biLinkS0_E1==0)
				{
					pLinkJg->desStartOdd.m_iOddCalStyle=0;	//������ײ��������ͷ
					pLinkJg->desStartOdd.m_hRefPart1=hBaseTube;
					pLinkJg->desStartOdd.m_hRefPart2=0;
					pLinkJg->desStartOdd.m_fCollideDist=g_sysPara.VertexDist;
					pLinkJg->CalStartOddment();
				}
				else //if(biLinkS0_E1==1)
				{
					pLinkJg->desEndOdd.m_iOddCalStyle=0;	//������ײ��������ͷ
					pLinkJg->desEndOdd.m_hRefPart1=hBaseTube;
					pLinkJg->desEndOdd.m_hRefPart2=0;
					pLinkJg->desEndOdd.m_fCollideDist=g_sysPara.VertexDist;
					pLinkJg->CalEndOddment();
				}
				if(biLinkS0_E1==0&&biWorkWingX0_Y1==0)
				{
					pLinkJg->desStartPos.wing_x_offset.gStyle=4;
					pLinkJg->desStartPos.wing_x_offset.offsetDist=-pTemParamPlate->Thick*0.5;
				}
				else if(biLinkS0_E1==0&&biWorkWingX0_Y1==1)
				{
					pLinkJg->desStartPos.wing_y_offset.gStyle=4;
					pLinkJg->desStartPos.wing_y_offset.offsetDist=-pTemParamPlate->Thick*0.5;
				}
				if(biLinkS0_E1==1&&biWorkWingX0_Y1==0)
				{
					pLinkJg->desEndPos.wing_x_offset.gStyle=4;
					pLinkJg->desEndPos.wing_x_offset.offsetDist=-pTemParamPlate->Thick*0.5;
				}
				if(biLinkS0_E1==1&&biWorkWingX0_Y1==1)
				{	
					pLinkJg->desEndPos.wing_y_offset.gStyle=4;
					pLinkJg->desEndPos.wing_y_offset.offsetDist=-pTemParamPlate->Thick*0.5;
				}
				pLinkJg->CalPosition(true);
				pLinkJg->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pLinkJg->GetSolidPartObject());
				//��������˨
				JGZJ jgzj;
				CConnectInfo ls_group;
				LSSPACE_STRU LsSpace;
				biWorkWingX0_Y1==0?jgzj=pLinkJg->GetZhunJu('X'):jgzj=pLinkJg->GetZhunJu('Y');
				biLinkS0_E1==0?ls_group=pLinkJg->connectStart:ls_group=pLinkJg->connectEnd;
				GetLsSpace(LsSpace,ls_group.d);
				CLDSPoint datumPoint;
				datumPoint.datum_pos_style=1;
				datumPoint.des_para.RODEND.hRod=pLinkJg->handle;
				datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
				datumPoint.des_para.RODEND.direction=biLinkS0_E1;
				datumPoint.des_para.RODEND.offset_wing=biWorkWingX0_Y1;
				datumPoint.des_para.RODEND.wing_offset_dist=0;
				datumPoint.des_para.RODEND.wing_offset_style=4;
				int offset=LsSpace.EndSpace;
				for(int j=0;j<ls_group.wnConnBoltN;j++)
				{
					CLDSBolt *pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					pRealLs->des_base_pos.hPart=pLinkJg->handle;
					pRealLs->des_base_pos.datumPoint=datumPoint;
					pRealLs->des_base_pos.offset_wing=biWorkWingX0_Y1;
					pRealLs->des_base_pos.direction=biLinkS0_E1;
					pRealLs->des_base_pos.len_offset_dist=offset;
					pRealLs->des_base_pos.wing_offset_dist=jgzj.g;
					_snprintf(pRealLs->des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-pLinkJg->GetThick());
					pRealLs->des_work_norm.norm_style=1;	//ָ���Ǹ�֫����	
					pRealLs->des_work_norm.direction=0;
					pRealLs->des_work_norm.hVicePart = pLinkJg->handle;
					pRealLs->des_work_norm.norm_wing=biWorkWingX0_Y1;
					offset += LsSpace.SingleRowSpace;
					//
					if(biLinkS0_E1)
						pLinkJg->AppendStartLsRef(pRealLs->GetLsRef());
					else
						pLinkJg->AppendEndLsRef(pRealLs->GetLsRef());
					pTemParamPlate->AppendLsRef(pRealLs->GetLsRef());
					pRealLs->AddL0Thick(pLinkJg->handle,TRUE);
					pRealLs->AddL0Thick(pTemParamPlate->handle,TRUE);
					pRealLs->CalGuigeAuto();
					pRealLs->correct_worknorm();
					pRealLs->correct_pos();
				}
			}
		}
		if(pTemParamPlate->DesignPlate())
			NewPlate(pTemParamPlate);
	}
	//��ӹ�����Լ��
	for(pParamPlate=(CLDSParamPlate*)Ta.Parts.GetFirst(CLS_PARAMPLATE);pParamPlate;pParamPlate=(CLDSParamPlate*)Ta.Parts.GetNext(CLS_PARAMPLATE))
	{
		CONSTRAINT* pConstraint=NULL;
		if(pParamPlate->m_iParamType==TYPE_STATIONSHOEPLATE)
		{	//��繹����������ѥ�壨���˳�ʱ��ѥ�弫���ƣ�
			long hEndPlate=0,hBasePlate=0;
			pParamPlate->GetDesignItemValue(KEY2C("LB"),&hBasePlate);
			CLDSParamPlate* pBasePlate=(CLDSParamPlate*)Ta.FromPartHandle(hBasePlate,CLS_PARAMPLATE);
			if(pBasePlate==NULL)
				continue;
			//������������������ѥ��֮������Թ���
			pConstraint=pBasePlate->AppendConstraint();
			pConstraint->hMasterObj=pBasePlate->handle;
			pConstraint->hAcceptObj=pParamPlate->handle;
			pConstraint->wiMasterPropId=PLATE_PROPID::PROP_PROFILE;
			pConstraint->wiAcceptPropId=PLATE_PROPID::PROP_PROFILE;
			pConstraint->dual=true;
			pParamPlate->IncrementConstraintRef();
			//�����а���˳Ŷ˰�֮������Թ���
			pParamPlate->GetDesignItemValue(KEY2C("LE"),&hEndPlate);
			CLDSParamPlate* pEndPlate=(CLDSParamPlate*)Ta.FromPartHandle(hEndPlate,CLS_PARAMPLATE);
			if(pEndPlate)
			{
				pConstraint=pParamPlate->AppendConstraint();
				pConstraint->hMasterObj=pParamPlate->handle;
				pConstraint->hAcceptObj=pEndPlate->handle;
				pConstraint->dual=true;
				pEndPlate->IncrementConstraintRef();
				pConstraint->wiAcceptPropId=PLATE_PROPID::STATIONCONNPLATE::PROP_HOLE_R;
				pConstraint->wiMasterPropId=PLATE_PROPID::STATIONSHOEPLATE::PROP_HOLE_R;
				pConstraint=pParamPlate->AppendConstraint(pConstraint);
				pEndPlate->IncrementConstraintRef();
				pConstraint->wiAcceptPropId=PLATE_PROPID::STATIONCONNPLATE::PROP_HOLE_H;
				pConstraint->wiMasterPropId=PLATE_PROPID::STATIONSHOEPLATE::PROP_HOLE_H;
				//��������ѥ��ֻ���ҵ���ߵ�����һ��˳����������Ӱ壬����ͨ�����彨���ɶӶ˳Ű���������ϵ
				pEndPlate->GetDesignItemValue(KEY2C("LP"),&hBasePlate);
				CLdsPartListStack stack(&Ta);
				CLDSParamPlate* pEndPlate2=NULL;
				for(pEndPlate2=(CLDSParamPlate*)Ta.Parts.GetFirst(CLS_PARAMPLATE);hBasePlate>0&&pEndPlate2;
					pEndPlate2=(CLDSParamPlate*)Ta.Parts.GetNext(CLS_PARAMPLATE))
				{
					long hBasePlate2=0;
					if(pEndPlate2!=pEndPlate&&pEndPlate2->GetDesignItemValue(KEY2C("LP"),&hBasePlate2)&&hBasePlate2==hBasePlate)
						break;
				}
				if(pEndPlate2)
				{
					pConstraint=pEndPlate->AppendConstraint();
					pConstraint->hMasterObj=pEndPlate->handle;
					pConstraint->hAcceptObj=pEndPlate2->handle;
					pConstraint->dual=true;
					pEndPlate2->IncrementConstraintRef();
					pConstraint->wiAcceptPropId=PLATE_PROPID::STATIONCONNPLATE::PROP_HOLE_R;
					pConstraint->wiMasterPropId=PLATE_PROPID::STATIONCONNPLATE::PROP_HOLE_R;
					pConstraint=pEndPlate->AppendConstraint(pConstraint);
					pEndPlate2->IncrementConstraintRef();
					pConstraint->wiAcceptPropId=PLATE_PROPID::STATIONCONNPLATE::PROP_HOLE_H;
					pConstraint->wiMasterPropId=PLATE_PROPID::STATIONCONNPLATE::PROP_HOLE_H;
				}
			}
		}
		else if(pParamPlate->m_iParamType==TYPE_STATIONCONNPLATE)
		{	//���ּܶ˳Ŷ����ϵĶ˰�
			long hBasePlate=0;
			pParamPlate->GetDesignItemValue(KEY2C("LP"),&hBasePlate);
			CLDSParamPlate* pBasePlate=(CLDSParamPlate*)Ta.FromPartHandle(hBasePlate,CLS_PARAMPLATE);
			if(pBasePlate==NULL)
				continue;
			//����������˰�֮������Թ���
			pConstraint=pBasePlate->AppendConstraint();
			pConstraint->hMasterObj=pBasePlate->handle;
			pConstraint->hAcceptObj=pParamPlate->handle;
			pConstraint->dual=true;
			pParamPlate->IncrementConstraintRef();
			pConstraint->wiMasterPropId=PLATE_PROPID::BASE::R;
			pConstraint->wiAcceptPropId=PLATE_PROPID::PROP_PROFILE;
		}
		else if(pParamPlate->m_iParamType==TYPE_STATIONCROSSPLATE)
		{	//���ּܶ˳Ŷ�����ʮ�ֲ��
			long hBasePlate=0;
			pParamPlate->GetDesignItemValue(KEY2C("LP"),&hBasePlate);
			CLDSParamPlate* pBasePlate=(CLDSParamPlate*)Ta.FromPartHandle(hBasePlate,CLS_PARAMPLATE);
			if(pBasePlate==NULL)
				continue;
			//������������֮������Թ���
			pConstraint=pBasePlate->AppendConstraint();
			pConstraint->hMasterObj=pBasePlate->handle;
			pConstraint->hAcceptObj=pParamPlate->handle;
			pConstraint->dual=true;
			pParamPlate->IncrementConstraintRef();
			pConstraint->wiMasterPropId=PLATE_PROPID::BASE::R;
			pConstraint->wiAcceptPropId=PLATE_PROPID::PROP_PROFILE;
			//�������֮������Թ���
			if(pParamPlate->GetConstraintByPropID(PLATE_PROPID::STATIONCROSSPLATE::PROP_PARA_H)==NULL)
			{
				CLdsPartListStack stack(&Ta);
				for(CLDSParamPlate* pChaPlate=(CLDSParamPlate*)Ta.Parts.GetFirst(CLS_PARAMPLATE);pChaPlate;pChaPlate=(CLDSParamPlate*)Ta.Parts.GetNext(CLS_PARAMPLATE))
				{
					if(pChaPlate->wiType!=TYPE_STATIONCROSSPLATE)
						continue;
					if(pChaPlate==pParamPlate)
						continue;
					long hBasePlate2=0;
					if(pChaPlate->GetDesignItemValue(KEY2C("LP"),&hBasePlate2)&&hBasePlate2==hBasePlate)
					{
						pConstraint=pParamPlate->AppendConstraint();
						pConstraint->hMasterObj=pParamPlate->handle;
						pConstraint->hAcceptObj=pChaPlate->handle;
						pConstraint->wiMasterPropId=PLATE_PROPID::STATIONCROSSPLATE::PROP_PARA_H;
						pConstraint->wiAcceptPropId=PLATE_PROPID::STATIONCROSSPLATE::PROP_PARA_H;
						pConstraint->dual=true;
						pChaPlate->IncrementConstraintRef();
					}
				}
			}
		}
	}
	for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_BOLT)
			continue;
		CLDSBolt* pBolt=(CLDSBolt*)pPart;
		pBolt->CalGuigeAuto();
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	Ta.Parts.RebuildHashtable();
	Ta.AddMirRelativeObjInfo();
	pCmdLine->FinishCmdLine();
#endif
	return 0;
}
#ifdef __GGG_
int CLDSView::NewFile3()
{
	CNewConeTubeDlg dlg;
	dlg.DoModal();
	CLDSConeTube *pConeTube = (CLDSConeTube *)console.AppendPart(CLS_CONETUBE);
	pConeTube->basePoint = f3dPoint(dlg.m_fX,dlg.m_fY,dlg.m_fZ);
	pConeTube->faceN = dlg.m_nFaceNum;
	pConeTube->longth = dlg.m_fLongth;
	pConeTube->size_thick = dlg.m_fThick;
	pConeTube->size_wide = dlg.m_fDownRadius;
	pConeTube->size_wideUpSide = dlg.m_fUpRadius;
	f3dPoint endPt(pConeTube->basePoint.x,pConeTube->basePoint.y,pConeTube->basePoint.z+pConeTube->longth);
	pConeTube->pStart = console.AppendNode();
	pConeTube->pStart->SetPosition(pConeTube->basePoint);
	pConeTube->pEnd = console.AppendNode();
	pConeTube->pEnd->SetPosition(endPt);
	NewLinePart(pConeTube);
	pConeTube->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pConeTube->GetSolidPartObject());
	pConeTube->cMaterial = QuerySteelBriefMatMark(dlg.m_sMaterial.GetBuffer(0));
#ifdef AFX_TARG_ENU_ENGLISH
	if (dlg.m_sType.CompareNoCase("")==0 || dlg.m_sType.CompareNoCase("")==0)
#else 
	if (dlg.m_sType.CompareNoCase("˫�ֹܸ�")==0 || dlg.m_sType.CompareNoCase("�ĸֹܸ�")==0)
#endif
	{
		CLDSConeTube *coneTubeFront = (CLDSConeTube *)console.AppendPart(CLS_CONETUBE);
		coneTubeFront->CopyProperty(*pConeTube);
		coneTubeFront->basePoint.x +=dlg.m_fFrontOpen+pConeTube->size_wide*2;
		f3dPoint endPtFront(coneTubeFront->basePoint.x,coneTubeFront->basePoint.y,coneTubeFront->basePoint.z+coneTubeFront->longth);
		coneTubeFront->pStart = console.AppendNode();
		coneTubeFront->pStart->SetPosition(coneTubeFront->basePoint);
		coneTubeFront->pEnd = console.AppendNode();
		coneTubeFront->pEnd->SetPosition(endPtFront);
		NewLinePart(pConeTube);
		coneTubeFront->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(coneTubeFront->GetSolidPartObject());
		pConeTube->relativeObjs.Add(coneTubeFront->handle);
		coneTubeFront->relativeObjs.Add(pConeTube->handle);
		
#ifdef AFX_TARG_ENU_ENGLISH
		if (dlg.m_sType.CompareNoCase("")==0)
#else 
		if (dlg.m_sType.CompareNoCase("�ĸֹܸ�")==0)
#endif
		{
			CLDSConeTube *coneTubeSideLeft = (CLDSConeTube *)console.AppendPart(CLS_CONETUBE);
			coneTubeSideLeft->CopyProperty(*pConeTube);
			coneTubeSideLeft->basePoint.y +=dlg.m_fSideOpen+coneTubeSideLeft->size_wide*2;
			f3dPoint endPtSideLeft(coneTubeSideLeft->basePoint.x,coneTubeSideLeft->basePoint.y,coneTubeSideLeft->basePoint.z+coneTubeSideLeft->longth);
			coneTubeSideLeft->pStart = console.AppendNode();
			coneTubeSideLeft->pStart->SetPosition(coneTubeSideLeft->basePoint);
			coneTubeSideLeft->pEnd = console.AppendNode();
			coneTubeSideLeft->pEnd->SetPosition(endPtSideLeft);
			NewLinePart(coneTubeSideLeft);
			coneTubeSideLeft->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(coneTubeSideLeft->GetSolidPartObject());
			pConeTube->relativeObjs.Add(coneTubeSideLeft->handle);
			coneTubeFront->relativeObjs.Add(coneTubeSideLeft->handle);
			coneTubeSideLeft->relativeObjs.Add(pConeTube->handle);
			coneTubeSideLeft->relativeObjs.Add(coneTubeFront->handle);
			
			CLDSConeTube *coneTubeSideRight = (CLDSConeTube *)console.AppendPart(CLS_CONETUBE);
			coneTubeSideRight->CopyProperty(*coneTubeFront);
			coneTubeSideRight->basePoint.y +=dlg.m_fSideOpen+coneTubeSideRight->size_wide*2;
			f3dPoint endPtSideRight(coneTubeSideRight->basePoint.x,coneTubeSideRight->basePoint.y,coneTubeSideRight->basePoint.z+coneTubeSideRight->longth);
			coneTubeSideRight->pStart = console.AppendNode();
			coneTubeSideRight->pStart->SetPosition(coneTubeSideRight->basePoint);
			coneTubeSideRight->pEnd = console.AppendNode();
			coneTubeSideRight->pEnd->SetPosition(endPtSideRight);
			NewLinePart(coneTubeSideRight);
			
			coneTubeSideRight->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(coneTubeSideRight->GetSolidPartObject());
			pConeTube->relativeObjs.Add(coneTubeSideRight->handle);
			coneTubeFront->relativeObjs.Add(coneTubeSideRight->handle);
			coneTubeSideLeft->relativeObjs.Add(coneTubeSideRight->handle);
			coneTubeSideRight->relativeObjs.Add(pConeTube->handle);
			coneTubeSideRight->relativeObjs.Add(coneTubeSideLeft->handle);
			coneTubeSideRight->relativeObjs.Add(coneTubeFront->handle);
		}
	}
//	CConeTubeFl *fl = (CConeTubeFl *)console.AppendPart(CLS_CONETUBEFL);
//	fl->Create3dSolidModel(coneTube->ucs);
//	g_pSolidDraw->NewSolidPart(fl->GetSolidPartObject());
//	g_pSolidDraw->Draw();
	return 0;
}
#endif