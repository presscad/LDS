// TtaDataFile.cpp: implementation of the CTtaDataFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "lds.h"
#include "Tower.h"
#include "env_def.h"
#include "XhCharString.h"
#include "SortFunc.h"
#include "PartLib.h"
#include "PostProcessReport.h"
#include "GlobalFunc.h"
#include "DesWireNodeDlg.h"
#include "IModel.h"
#include "LifeObj.h"
#include "LdsDoc.h"
#include "PreProcessor.h"
#include "TtaDataFile.h"
#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#if defined(__LDS_)||defined(__TSA_)

int SafeIsDigit(char ch){return ch<0?0:isdigit(ch);}
TTA_BODYLEG::TTA_BODYLEG()
{
	m_pTtaFile=NULL;
	MylBraceNodeMAX=0;
	listId=0;
	niLegN=0;
}
static CHashPtrList<LIST_TTANODE_SET> _localTtaNodeSets;
DWORD TTA_BODYLEG::ApplyListTtaNodeSet(CTtaDataFile* pTtaFile)
{
	LIST_TTANODE_SET *pList=pTtaFile->localTtaNodeSets.Add(0);//new CXhSimpleList<TTA_NODEPTR>();
	m_pTtaFile=pTtaFile;
	return listId=pList->id;
}
static CXhSimpleList<TTA_NODEPTR> _localEmptyTtaNodeSet;
CXhSimpleList<TTA_NODEPTR>& TTA_BODYLEG::getListTtaNodeSet()
{
	LIST_TTANODE_SET *pList=m_pTtaFile->localTtaNodeSets.GetValue(listId);
	if(pList)
		return pList->listTtaNodeSet;
	else
	{
		logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"δ�ر�Ϊ��ǰ�����ָ��TTA�ڵ㼯�ϣ�ϵͳת�Ƶ�Ĭ�ϼ�����!");
		return _localEmptyTtaNodeSet;
	}
}
TTA_NODE::TTA_NODE(TTA_NODE* pSrcNode/*=NULL*/)
{
	if (pSrcNode)
	{
		memset(this,0,sizeof(TTA_NODE));
		CopyFrom(pSrcNode);
	}
	else
	{
		memset(this,0,sizeof(TTA_NODE));
		cPosLayer='L';
	}
}
void TTA_NODE::CopyFrom(TTA_NODE* pSrcNode)
{
	CStackVariant<TTA_NODE*> stackvar1(&_pMirFromNode,NULL);
	CStackVariant<char> stackvar2(&m_ciMirFromType,NULL);
	CStackVariant<bool> stackvar3(&_blInitPosCalParams,NULL);
	*this=*pSrcNode;
}
bool TTA_NODE::set_blInitPosCalParams(bool blValue) {
	return this->_blInitPosCalParams=blValue;
}
bool TTA_NODE::get_blInitPosCalParams()
{
	if (_pMirFromNode==NULL||this->_blInitPosCalParams)
		return true;
	else
		return false;
}
TTA_NODE* TTA_NODE::get_pMirFromNode()
{
	return _pMirFromNode;
}
char TTA_NODE::get_ciMirFromType()
{
	return this->m_ciMirFromType;
}
void TTA_NODE::MirBasicPropFrom(TTA_NODE* pSrcNode,int mir_x1y2z3)
{
	IS=pSrcNode->IS;
	posType=pSrcNode->posType;
	cfgword=pSrcNode->cfgword;
	bBraceNode=pSrcNode->bBraceNode;
	bFoundationNode=pSrcNode->bFoundationNode;
	m_cbPosUpdated=pSrcNode->IsPosUpdated(0x07);
	MIRMSG mirmsg(mir_x1y2z3);
	GEPOINT mirpos=mirmsg.MirPoint(pSrcNode->pos);
	if(posType==0)
	{
		pos=mirpos;
		SetPosUpdateState(0x07);
	}
	else if(posType==1)
	{	//1.ָ��X���������;
		pos.x=mirpos.x;
		if(m_cbPosUpdated&0x06)
			pos=mirpos;
	}
	else if(posType==2)
	{	//2.ָ��Y���������;
		pos.y=mirpos.y;
		if(m_cbPosUpdated&0x05)
			pos=mirpos;
	}
	else if(posType==3)
	{	//3.ָ��Z���������;4.�����ȷֵ�;5.�����
		pos.z=mirpos.z;
		if(m_cbPosUpdated&0x03)
			pos=mirpos;
	}
	else if(posType==4&&(m_cbPosUpdated&0x07))
	{	//4.�����ȷֵ�;5.�����
		pos=mirpos;
	}
	else if(posType==5&&(m_cbPosUpdated&0x07))
	{	//5.�����
		pos=mirpos;
	}
	if(mir_x1y2z3==1)
		m_ciMirFromType='X';
	else if(mir_x1y2z3==2)
		m_ciMirFromType='Y';
	else if(mir_x1y2z3==3)
		m_ciMirFromType='Z';
	_pMirFromNode=pSrcNode;
}
bool TTA_NODE::UpdateBasicPropFromParentNode()
{
	if (_pMirFromNode==NULL)
		return false;
	if(posType!=12)	//ԭʼ��Գ�����ʱ����ԭʼ���ͬ wjh-2019.10.22
		posType=_pMirFromNode->posType;
	if (m_ciMirFromType=='X')
		pos.Set(_pMirFromNode->pos.x,-_pMirFromNode->pos.y,_pMirFromNode->pos.z);
	else if (m_ciMirFromType=='Y')
		pos.Set(-_pMirFromNode->pos.x,_pMirFromNode->pos.y,_pMirFromNode->pos.z);
	else if (m_ciMirFromType=='Z')
		pos.Set(-_pMirFromNode->pos.x,-_pMirFromNode->pos.y,_pMirFromNode->pos.z);
	else
		return false;
	return true;
}
bool CTtaDataFile::UpdateDatumLine(TANS_LINE* pLine)
{
	TTA_NODE* pRefStartNode=hashTtaNodeI.GetValue(pLine->iStartPointI);
	TTA_NODE* pRefEndNode=hashTtaNodeI.GetValue(pLine->iEndPointI);
	pRefStartNode->UpdatePosition();
	pRefEndNode->UpdatePosition();
	pLine->posStart=pRefStartNode->pos;
	pLine->posEnd=pRefEndNode->pos;
	return true;
}
bool TANS_PLANE::UpdatePlane(CTtaDataFile* pTtaModel/*=NULL*/)
{
	if(m_bUpdatePlane)
		return true;
	TTA_NODE* pRefNode1st=pTtaModel->hashTtaNodeI.GetValue(iPoint1stI);
	TTA_NODE* pRefNode2st=pTtaModel->hashTtaNodeI.GetValue(iPoint2ndI);
	TTA_NODE* pRefNode3st=pTtaModel->hashTtaNodeI.GetValue(iPoint3rdI);
	pRefNode1st->UpdatePosition(pTtaModel);
	pRefNode2st->UpdatePosition(pTtaModel);
	pRefNode3st->UpdatePosition(pTtaModel);
	xPlaneOrg=pRefNode1st->pos;
	GEPOINT v1=pRefNode2st->pos-pRefNode1st->pos;
	GEPOINT v2=pRefNode3st->pos-pRefNode1st->pos;
	vPlaneNormal=v1^v2;
	normalize(vPlaneNormal);
	m_bUpdatePlane=true;
	return true;
}
bool CTtaDataFile::UpdateDatumPlane(TANS_PLANE* pPlane)
{
	TTA_NODE* pRefNode1st=hashTtaNodeI.GetValue(pPlane->iPoint1stI);
	TTA_NODE* pRefNode2st=hashTtaNodeI.GetValue(pPlane->iPoint2ndI);
	TTA_NODE* pRefNode3st=hashTtaNodeI.GetValue(pPlane->iPoint3rdI);
	pRefNode1st->UpdatePosition(this);
	pRefNode2st->UpdatePosition(this);
	pRefNode3st->UpdatePosition(this);
	pPlane->xPlaneOrg=pRefNode1st->pos;
	GEPOINT v1=pRefNode2st->pos-pRefNode1st->pos;
	GEPOINT v2=pRefNode3st->pos-pRefNode1st->pos;
	pPlane->vPlaneNormal=v1^v2;
	normalize(pPlane->vPlaneNormal);
	return true;
}
bool TTA_NODE::UpdatePosition(CTtaDataFile* pTtaModel/*=NULL*/)
{
	if(m_cbPosUpdated==0x07)
		return true;
	if(posType<=0)
	{
		m_cbPosUpdated=0x07;
		return true;
	}
	if(pAttachStart==NULL&&pTtaModel!=NULL&&J1>0)
		pAttachStart=pTtaModel->hashTtaNodeI.GetValue(J1);
	if(pAttachEnd==NULL&&pTtaModel!=NULL&&J2>0)
		pAttachEnd=pTtaModel->hashTtaNodeI.GetValue(J2);
	if(posType<6&&(pAttachStart==NULL||pAttachEnd==NULL))
		return false;
	CDepthCounter<BYTE> visitor(&m_cnVisitHits);
	if(m_cnVisitHits>1)
	{
		m_cbPosUpdated=0x07;
		logerr.Log("%d�Žڵ����������ѭ��ʽ��������λ�ø���ʧ��!",pointI);
		return false;
	}
	if(posType==12)
	{
		if(pAttachStart==NULL||!pAttachStart->UpdatePosition(pTtaModel))
			return false;
		m_cbPosUpdated = 0x07;
		if(cMirTransType=='X')
			pos.Set( pAttachStart->pos.x,-pAttachStart->pos.y,pAttachStart->pos.z);
		else if(cMirTransType=='Y')
			pos.Set(-pAttachStart->pos.x, pAttachStart->pos.y,pAttachStart->pos.z);
		else if(cMirTransType=='Z')
			pos.Set(-pAttachStart->pos.x,-pAttachStart->pos.y,pAttachStart->pos.z);
		else
			pos=pAttachStart->pos;
		return true;
	}
	if((pAttachStart&&!pAttachStart->UpdatePosition(pTtaModel)) || (pAttachEnd&&!pAttachEnd->UpdatePosition(pTtaModel)))
		return false;	//��ǰ�ýڵ������ڵ����ʧ�ܣ����������������в�������ѭ��ʽ����������Ӧ�����ͷŷ��ʴ���
	m_cbPosUpdated=0x07;
	if(pAttachStart&&pAttachEnd&&posType==0)
	{	//�Զ������ļ�����������������ʽ��ͨ�����)��ֵposType
		if(pos.x<10000)
			posType=1;
		else if(pos.y<10000)
			posType=2;
		else if(pos.z<10000)
			posType=3;
		if(pos.x>10000)
			pos.x-=10000;
		if(pos.y>10000)
			pos.y-=10000;
		if(pos.z>10000)
			pos.z-=10000;
	}
	if(posType==1)
	{	//ָ�������������
		double scale_x=(pos.x-pAttachStart->pos.x)/(pAttachEnd->pos.x-pAttachStart->pos.x);
		pos=pAttachStart->pos+scale_x*(pAttachEnd->pos-pAttachStart->pos);
	}
	else if(posType==2)
	{	//ָ�������������
		double scale_y=(pos.y-pAttachStart->pos.y)/(pAttachEnd->pos.y-pAttachStart->pos.y);
		pos=pAttachStart->pos+scale_y*(pAttachEnd->pos-pAttachStart->pos);
	}
	else if(posType==3)
	{	//ָ�������������
		double scale_z=(pos.z-pAttachStart->pos.z)/(pAttachEnd->pos.z-pAttachStart->pos.z);
		pos=pAttachStart->pos+scale_z*(pAttachEnd->pos-pAttachStart->pos);
	}
	else if(posType==4)
	{	//�����ȷֵ�
		pos=pAttachStart->pos+coefScale*(pAttachEnd->pos-pAttachStart->pos);
	}
	else if(posType==5)
	{	//�����
		GEPOINT lineStart,lineEnd;
		TTA_NODE* pRefStart=pTtaModel->hashTtaNodeI.GetValue(J3);
		if(pRefStart==NULL)
			pRefStart=pTtaModel->hashTtaNodeI.GetValue(J3%10000);
		TTA_NODE* pRefEnd  =pTtaModel->hashTtaNodeI.GetValue(J4);
		if(pRefEnd==NULL)
			pRefEnd  =pTtaModel->hashTtaNodeI.GetValue(J4%10000);
		if(pRefStart==NULL||pRefEnd==NULL)
		{
			this->m_cbPosUpdated=0;
			return false;
		}
		pRefStart->UpdatePosition(pTtaModel);
		pRefEnd->UpdatePosition(pTtaModel);
		lineStart=pRefStart->pos;
		lineEnd  =pRefEnd->pos;
		Int3dll(pAttachStart->pos,pAttachEnd->pos,lineStart,lineEnd,pos);
	}
	else if (posType == 6)
	{	//�����������
		//TODO:Ŀǰ���������ļ���δ�õ����д����� wjh-2018.6.2
		TTA_NODE* pOrgNode = pTtaModel->hashTtaNodeI.GetValue(J3 % 10000);
		if (pAttachStart&&pAttachEnd&&pOrgNode)
		{
			f3dPoint vec = (pAttachEnd->pos - pAttachStart->pos).normalized();
			if (ciPosViceType == 'X')
				pos = pAttachStart->pos + vec * (pOrgNode->pos.x - pAttachStart->pos.x)*(vec.mod() / vec.x);
			else if (ciPosViceType == 'Y')
				pos = pAttachStart->pos + vec * (pOrgNode->pos.y - pAttachStart->pos.y)*(vec.mod() / vec.y);
			else //if(ciPosViceType==0||ciPosViceType=='Z')
				pos = pAttachStart->pos + vec * (pOrgNode->pos.z - pAttachStart->pos.z)*(vec.mod() / vec.z);
		}
	}
	else if (posType == 13)
	{	//����սڵ����������Ľڵ� wjh-2019.7.10
		TTA_NODE* pOrgNode = NULL;
		if (J1 > 0)
		{
			pOrgNode = pTtaModel->hashTtaNodeI.GetValue(J1 % 10000);
			if (!pOrgNode->IsPosUpdated(0x01))
				pOrgNode->UpdatePosition(pTtaModel);
			pos.x = pOrgNode->pos.x;
		}
		if (J2 > 0)
		{
			pOrgNode = pTtaModel->hashTtaNodeI.GetValue(J2 % 10000);
			if (!pOrgNode->IsPosUpdated(0x02))
				pOrgNode->UpdatePosition(pTtaModel);
			pos.y = pOrgNode->pos.y;
		}
		if (J3 > 0)
		{
			pOrgNode = pTtaModel->hashTtaNodeI.GetValue(J3 % 10000);
			if (!pOrgNode->IsPosUpdated(0x04))
				pOrgNode->UpdatePosition(pTtaModel);
			pos.z = pOrgNode->pos.z;
		}
	}
	else if(posType==7)
	{	//Ԥ��ƫ�Ƶ�
		f3dPoint vec=(pAttachEnd->pos-pAttachStart->pos).normalized();
		if(ciPosViceType=='X')
			pos= pAttachEnd->pos+vec*offsetDist*(vec.mod()/fabs(vec.x));
		else if(ciPosViceType=='Y')
			pos= pAttachEnd->pos+vec*offsetDist*(vec.mod()/fabs(vec.y));
		else if(ciPosViceType=='Z')
			pos= pAttachEnd->pos+vec*offsetDist*(vec.mod()/fabs(vec.z));
		else
			pos= pAttachStart->pos+vec*offsetDist;
	}
	else if(posType==8)
	{	//��׼����XY����ֵ�����
		f3dLine line;
		TANS_PLANE* pPlane=pTtaModel->hashPlanes.GetValue(J3);
		if(pPlane)
		{
			pPlane->UpdatePlane(pTtaModel);
			if(pAttachStart&&pAttachEnd)
				line=f3dLine(pAttachStart->pos,pAttachEnd->pos);
			else
			{
				line.startPt=pos;
				line.endPt.Set(pos.x,pos.y,pos.z+1000);
			}
			Int3dlf(pos,line,pPlane->xPlaneOrg,pPlane->vPlaneNormal);
		}
		else
		{	//�ڻ�׼�����ʱ���Գƽڵ�
			m_cbPosUpdated=0;
			return false;
		}
	}
	else if(posType==9)
	{	//��׼����YZ����ֵ�����
		f3dLine line;
		TANS_PLANE* pPlane=pTtaModel->hashPlanes.GetValue(J3);
		if(pPlane!=NULL)
		{
			if(pAttachStart&&pAttachEnd)
				line=f3dLine(pAttachStart->pos,pAttachEnd->pos);
			else
			{
				line.startPt=pos;
				line.endPt.Set(pos.x+1000,pos.y,pos.z);
			}
			GEPOINT xPlaneOrg=pPlane->xPlaneOrg;
			GEPOINT vPlaneNormal=pPlane->vPlaneNormal;
			if(cMirTransType=='X')
			{
				xPlaneOrg.Set(xPlaneOrg.x,-xPlaneOrg.y,xPlaneOrg.z);
				vPlaneNormal.Set(vPlaneNormal.x,-vPlaneNormal.y,vPlaneNormal.z);
			}
			else if(cMirTransType=='Y')
			{
				xPlaneOrg.Set(-xPlaneOrg.x,xPlaneOrg.y,xPlaneOrg.z);
				vPlaneNormal.Set(-vPlaneNormal.x,vPlaneNormal.y,vPlaneNormal.z);
			}
			else if(cMirTransType=='Z')
			{
				xPlaneOrg.Set(-xPlaneOrg.x,-xPlaneOrg.y,xPlaneOrg.z);
				vPlaneNormal.Set(-vPlaneNormal.x,-vPlaneNormal.y,vPlaneNormal.z);
			}
			Int3dlf(pos,line,xPlaneOrg,vPlaneNormal);
		}
		else
		{
			this->m_cbPosUpdated=0;
			return false;
		}
	}
	else if(posType==10)
	{	//��׼����XZ����ֵ�����
		f3dLine line;
		TANS_PLANE* pPlane=pTtaModel->hashPlanes.GetValue(J3);
		if(pPlane!=NULL)
		{
			if(pAttachStart&&pAttachEnd)
				line=f3dLine(pAttachStart->pos,pAttachEnd->pos);
			else
			{
				line.startPt=pos;
				line.endPt.Set(pos.x,pos.y+1000,pos.z);
			}
			GEPOINT xPlaneOrg=pPlane->xPlaneOrg;
			GEPOINT vPlaneNormal=pPlane->vPlaneNormal;
			if(cMirTransType=='X')
			{
				xPlaneOrg.Set(xPlaneOrg.x,-xPlaneOrg.y,xPlaneOrg.z);
				vPlaneNormal.Set(vPlaneNormal.x,-vPlaneNormal.y,vPlaneNormal.z);
			}
			else if(cMirTransType=='Y')
			{
				xPlaneOrg.Set(-xPlaneOrg.x,xPlaneOrg.y,xPlaneOrg.z);
				vPlaneNormal.Set(-vPlaneNormal.x,vPlaneNormal.y,vPlaneNormal.z);
			}
			else if(cMirTransType=='Z')
			{
				xPlaneOrg.Set(-xPlaneOrg.x,-xPlaneOrg.y,xPlaneOrg.z);
				vPlaneNormal.Set(-vPlaneNormal.x,-vPlaneNormal.y,vPlaneNormal.z);
			}
			Int3dlf(pos,line,xPlaneOrg,vPlaneNormal);
		}
		else
		{
			this->m_cbPosUpdated=0;
			return false;
		}
	}
	return true;
}
int CLDSView::TowerFile()
{
	if(!PRODUCT_FUNC::IsHasInternalTest())
		return -1;
	//1.ģ������ϵת�����������ϣ�
	//2.Group Label��ţ������š�ͳ�ĺŵ��ۺϣ�
	CLogFile towerfile("d:/lds.tow");
	CLogErrorLife life(&towerfile);
	CXhChar200 rowtext;
	rowtext.Append("TYPE='TOWER INPUT FILE'");	//�ļ�����
	rowtext.Append("VERSION='13.2'",' ');		//�ļ��汾��
	rowtext.Append("UNITS='INTERNAL'",' ');		//��λ
	rowtext.Append("SOURCE='Tower Version 7.52'",' ');//Tower�汾��
	rowtext.Append("USER='Power Line Systems, Inc.'",' ');//�û�
	rowtext.Append("FILENAME='",' ');	//�ļ�����
	CXhChar200 filename;
	theApp.GetFileName(filename);
	rowtext.Append(filename);
	rowtext.Append('\'');
	//д���ļ�ͷ
	towerfile.Log(rowtext);
	rowtext.Empty();
	//д��������
	towerfile.Log("0 ; use edf suffixes");
	towerfile.Log("");	//��������
	towerfile.Log("");	//���̱�ע˵��
	towerfile.Log("0 0 ; write saps sum, page length");
	towerfile.Log("1.225000 0.000000 1e-008 ; rho, input temp, EP1");
	towerfile.Log("1 1000 20 ; analysis type, max iterations, # points on cable");	//0.���� 1.������
	towerfile.Log("0.1000000000 1.0000002772 1000000.2771775152 3.0000000000 0.0000000000 ; max imbalance, ascorm, dasat, pwiter, min stiffness");	//����������Ԫ�������
	//д�����ڵ㣨�������κ������ڵ㼰�������˱�Ϊ�����ڵ�Ľڵ����ͣ�
	int countOfPrimaryNodes=7;	//TODO:�Զ���ȡ��̬����
	towerfile.Log("%d ; Joints Geometry: joint label\nrestraint 1-6\nx,y,z\nhas master, symetry code, relative dist\nfrom, to joint, is secondary, fraction",countOfPrimaryNodes);
	towerfile.Log("'TOP'");//д��ڵ����ƣ���ţ�
	towerfile.Log("0 0 0 0 0 0");//д��ڵ�Լ������
	towerfile.Log("0 0 12.192 12.192");//д��ڵ����꼰��ѹ����
	towerfile.Log("0 3 0");	//�м�symmetry code 0:None;1:X-Symmetry;2:Y-Symmetry;3:XY-Symmetry;4:Tri-Symmetry
	towerfile.Log("'' '' 0 0");	//�����ڴ���II��ڵ㣺from, to joint, is secondary, fraction
	//д�뵥Ԫ������Ϣ
	towerfile.Log("\n");
	towerfile.Log("0 ; number truss properties");
	towerfile.Log("\n");
	towerfile.Log("0  ; number beam properties");
	towerfile.Log("\n");
	towerfile.Log("0  ; number cable properties");
	towerfile.Log("\n");
	towerfile.Log("0 ; number subs properties");
	//д������ļ���Ϣ
	towerfile.Log("0 0 1 0 0; analysis option, print rotations, echo input, gen diffs, load type to use, followed by .lca, .lic, .eia filename lines");
	towerfile.Log("c:\\demos\\tower\\examples\\ex2.lca");
	towerfile.Log("c:\\demos\\tower\\examples\\*.lic");
	towerfile.Log("\n");	//EIA file
	return 0;
}
int CLDSView::TtaFile()
{
	CString cmdStr;
	int retCode=0;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)theApp.m_pMainWnd)->GetCmdLinePage();
	static char cDefaultType;
	if(cDefaultType==0)
		cDefaultType='I';
	cmdStr.Format("TTA �ļ���������[����(I)/����(O)]<%I>:",cDefaultType);
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			pCmdLine->m_bGettingStrFromCmdLine=FALSE;
			return 0;
		}
		pCmdLine->m_bGettingStrFromCmdLine=FALSE;
		if(cmdStr.CompareNoCase("I")==0)
			cDefaultType='I';
		else if(cmdStr.CompareNoCase("O")==0)
			cDefaultType='O';
		else if(cmdStr.GetLength()>0)
		{
			pCmdLine->FillCmdLine("��Ҫѡ��ؼ���","");
			pCmdLine->FinishCmdLine();
			cmdStr.Format("TTA �ļ���������[����(I)/����(O)]<%C>:",cDefaultType);
			pCmdLine->FillCmdLine(cmdStr,"");
			continue;
		}
		break;
	}
	if(cDefaultType=='I')	//�����½ڼ�
		TtaFileIn();
	else
		TtaFileExport();
	return 0;
}
//����һ��ʵЧ������У�������ע�����ų�����) wjh-2017.3.4
char* ReadEffectLineText(char* _Str,int MaxCount,FILE* fp)
{
	char *semi=NULL,*marker=NULL;	//�ֺ�
	bool blReadFromLineStart=true;
	do{
		if(fgets(_Str,MaxCount,fp)==NULL)
			return NULL;
		size_t nstr=strlen(_Str);
		bool blFindFinalSymbol=(_Str[nstr-1]=='\n');
		if (!blReadFromLineStart)
		{
			blReadFromLineStart=blFindFinalSymbol;
			continue;	//��ǰ������֮ǰ�еĽض�����
		}
		blReadFromLineStart=blFindFinalSymbol;
		semi=_Str;
		while(*semi==' '||*semi=='\t'){semi++;}
		if(*semi==';'||*semi=='\r'||*semi=='\n')
			marker=semi;
		else
			break;
	}while(marker!=NULL);
	return _Str;
}
bool CTtaDataFile::ReadTowerTypeCfgFile(FILE* fp,BYTE ciFileType/*=0*/)
{
	char line_txt[200]="";
	//��ʼ��Ĭ�ϸֲĻ�е����(5�ֲָ�)����˨��е����(5�ּ���)��
	char plb_file[MAX_PATH];
#ifdef __LDS_
	sprintf(plb_file,"%sLDS.plb",theApp.work_path);//APP_PATH);
#elif defined(__TSA_)
	sprintf(plb_file,"%sTSA.plb",theApp.work_path);//APP_PATH);
#endif
	InitPartLibrary(plb_file);
	//��˨����TTA�趨��Ϣ
	int nLineNum=0;
	const int START_VAILD_LINENUM=20;	//��20��Ϊ��Ч��ʼ��
	//��ȡIW����ֵ(��ǰ�е���������)
	IW =125;
	long JW=0,TW=0;
	int line_count=0,imat=0;
	while(ReadEffectLineText(line_txt,200,fp)!=NULL)
	{
		CString linestr=line_txt;
		linestr.TrimLeft();
		//if(linestr.IsEmpty())//strlen(line_txt)==0)
		//	continue;//����
		if (strstr(line_txt,"YIELD STRENGTH")!=NULL&&ciFileType==FILETYPE_GTOM)
		{
			CXhChar100 linetxt(line_txt);
			char* keymat=strtok(linetxt," \t");
			if (keymat!=NULL)
			{
				double dfAllowStrength=atof(keymat)*10;
				if (fabs(dfAllowStrength-235)<15)
					xarrSteelSymbols.Append('S');
				else if (fabs(dfAllowStrength-345)<15)
					xarrSteelSymbols.Append('H');
				else if (fabs(dfAllowStrength-390)<15)
					xarrSteelSymbols.Append('G');
				else if (fabs(dfAllowStrength-420)<15)
					xarrSteelSymbols.Append('P');
				else if (fabs(dfAllowStrength-460)<15)
					xarrSteelSymbols.Append('T');
				else
					continue;
				imat++;
			}
		}
		line_count++;
		if(strstr(line_txt,"IWT1")!=NULL||ciFileType==1&&line_count==14)
		{
			sscanf(line_txt,"%d%d%d",&JW,&TW,&IW);
			break;
		}
		//if(nLineNum<START_VAILD_LINENUM-1)
		//	nLineNum++;
		//else
		//	break;
	}
	//char sPropName[50]="";
	//sscanf(line_txt,"%d%d%d%s",&JW,&TW,&IW,sPropName);
	//if(stricmp(sPropName,"IWT1:")!=0)
	//	return false;
	//��ȡ��ϵ������
	while(ReadEffectLineText(line_txt,200,fp)!=NULL)
	{
		CString linestr=line_txt;
		//linestr.TrimLeft();
		//if(linestr.IsEmpty())//strlen(line_txt)==0)
		//	continue;//����
		line_count++;
		if(strstr(line_txt,"FMNS1,FMNS2,FMNS3,FMNS4")!=NULL||ciFileType==1&&line_count==18)
		{
			sscanf(line_txt,"%lf%lf%lf%lf",&HOLECOEF_ARR[0],&HOLECOEF_ARR[1],&HOLECOEF_ARR[2],&HOLECOEF_ARR[3]);
			break;
		}
	}
	//�˼��������
	fgets(line_txt,200,fp);
	int i=1,n=0,serial=0;
	sscanf(line_txt,"%d",&n);
	m_arrRodSpec.SetSize(n);
	CXhChar50 xarrItems[8];
	for(i=0;i<n;i++)
	{
		if(ReadEffectLineText(line_txt,200,fp)==NULL)
			break;
		int nItemCount = DELIMITER_STR::ParseWordsFromStr(line_txt, ", \t", xarrItems, 8);
		serial=0;		//������
		m_arrRodSpec[i].iSizeSpecType=1;	//�˼�������ͣ�5����ʾ�ֹ�;1��������5��ʾ�Ǹ�
		m_arrRodSpec[i].size=atof(xarrItems[0]);
		m_arrRodSpec[i].area=atof(xarrItems[1]);
		m_arrRodSpec[i].minR=atof(xarrItems[2]);
		m_arrRodSpec[i].paraR=atof(xarrItems[3]);
		m_arrRodSpec[i].interR=atof(xarrItems[4]);
		if(nItemCount==6)
			serial=atoi(xarrItems[5]);
		else if(ciFileType==CTtaDataFile::FILETYPE_TTA&&nItemCount>=7)
		{
			serial=atoi(xarrItems[6]);
			char* pchDot=strchr(xarrItems[5],'.');
			double dfItemVal=atof(xarrItems[5]);
			if(pchDot)
				m_arrRodSpec[i].Wx=dfItemVal;//��С�����ʾ����Ժ��TTA�ĸ˼�����ģ��Wx
			else if(dfItemVal>=1&&dfItemVal<=5)
				m_arrRodSpec[i].iSizeSpecType=(int)dfItemVal;
			if (nItemCount==8)
				m_arrRodSpec[i].iSizeSpecType=atoi(xarrItems[7]);
		}
		else if(ciFileType==CTtaDataFile::FILETYPE_MYL&&nItemCount>=7)
		{
			serial=atoi(xarrItems[5]);
			char* pchDot=strchr(xarrItems[6],'.');
			double dfItemVal=atof(xarrItems[6]);
			if(pchDot)	//��С�����ʾ����Ժ��TTA�ľ����
				m_arrRodSpec[i].iSizeSpecType=5;//ת��ΪMYL�еĸֹ�
			else if(dfItemVal>1&&dfItemVal<=5)
				m_arrRodSpec[i].iSizeSpecType=(int)dfItemVal;
			else
				m_arrRodSpec[i].iSizeSpecType=max(1,((int)m_arrRodSpec[i].size)/1000);
		}
		//sscanf(line_txt,"%lf%lf%lf%lf%lf%d%d",&,&m_arrRodSpec[i].area,
		//	&m_arrRodSpec[i].minR,&m_arrRodSpec[i].paraR,&m_arrRodSpec[i].interR,&serial,&m_arrRodSpec[i].iSizeSpecType);
	}
	m_bLoadDataIniFile = true;
	return true;
}
bool CTtaDataFile::ReadTansNewFormatIniFile(FILE* fp)
{
	char ciFileType = 3;
	char line_txt[200] = "";
	//��ʼ��Ĭ�ϸֲĻ�е����(5�ֲָ�)����˨��е����(5�ּ���)��
	char plb_file[MAX_PATH];
#ifdef __LDS_
	sprintf(plb_file, "%sLDS.plb", theApp.work_path);//APP_PATH);
#elif defined(__TSA_)
	sprintf(plb_file, "%sTSA.plb", theApp.work_path);//APP_PATH);
#endif
	InitPartLibrary(plb_file);
	//��˨����TTA�趨��Ϣ
	int nLineNum = 0;
	const int START_VAILD_LINENUM = 20;	//��20��Ϊ��Ч��ʼ��
	//��ȡIW����ֵ(��ǰ�е���������)
	IW = 125;
	long JW = 0, TW = 0;
	int line_count = 0;
	int SKIP_ROWS = 0;
	static const int KEYVAL_TYPE_MATTBL		= 7;
	static const int KEYVAL_TYPE_IW			= 8;
	static const int KEYVAL_TYPE_HOLECOEF	= 9;
	static const int KEYVAL_TYPE_SIZETBL = 10;
	static const int KEYVAL_TYPE_SIZEREC = 11;
	int NEXT_GET_KEYVAL_TYPE = 0;

	while (ReadEffectLineText(line_txt, 200, fp) != NULL)
	{
		line_count++;
		CString linestr = line_txt;
		linestr.TrimLeft();
		if (SKIP_ROWS > 0)
		{
			SKIP_ROWS--;
			continue;
		}
		//if(linestr.IsEmpty())//strlen(line_txt)==0)
		//	continue;//����
		//line_count==1  //    0.050     0.175     1.300(�Ǹ֦�s��     0.850(�ָ֦�s��     1.300(��ϽǸ֦�s�� 1.500
		//line_count==2  //    4    3    3���Ǹ���С���
		//line_count==3  //    4    3    3���ֹ���С���
		//line_count==4  //150.000   200.000   250.000   400.000  �Ǹֳ�ϸ��
		//line_count==6  //150.000   160.000   160.000   160.000   140.000  �ֹܳ�ϸ�ȣ���һ������Ϊˮƽ�ֹ�����ֵ��TYPE���� 7�ࣨ���øֹܣ� 
		if (line_count < 6)
			continue;
		else if (line_count == 6)
		{
			int nBoltCount = 4;
			sscanf(line_txt, "%d", &nBoltCount);	// 4  ��˨ǿ��������������
			SKIP_ROWS = nBoltCount;
			NEXT_GET_KEYVAL_TYPE = KEYVAL_TYPE_MATTBL;
			continue;
		}
		//else if (line_count > 6)
		if (NEXT_GET_KEYVAL_TYPE == KEYVAL_TYPE_MATTBL)
		{
			int nMatCount = 5;
			sscanf(line_txt, "%d", &nMatCount);	// 5  �ֲ�ǿ��������������
			SKIP_ROWS = nMatCount * 3;
			NEXT_GET_KEYVAL_TYPE = KEYVAL_TYPE_IW;
		}
		else if (NEXT_GET_KEYVAL_TYPE == KEYVAL_TYPE_IW)
		{
			sscanf(line_txt, "%d", &this->IW);	// 125 ���Ǹ����ļ�����������֫�� iw 
			NEXT_GET_KEYVAL_TYPE = KEYVAL_TYPE_HOLECOEF;
		}
		else if (NEXT_GET_KEYVAL_TYPE == KEYVAL_TYPE_HOLECOEF)
		{	//��ȡ��ϵ������
			sscanf(line_txt, "%lf%lf%lf%lf", &HOLECOEF_ARR[0], &HOLECOEF_ARR[1], &HOLECOEF_ARR[2], &HOLECOEF_ARR[3]);
			NEXT_GET_KEYVAL_TYPE = KEYVAL_TYPE_SIZETBL;
			break;
		}
	}
	fgets(line_txt, 200, fp);
	int i = 1, n = 0, serial = 0;
	sscanf(line_txt, "%d", &n);// 52   40   63   63   63��.  ����������ÿ�ֲָĵȼ���Ӧ��С֫������ѡ������ʹ��ĸ˼��� 	//�˼��������
	m_arrRodSpec.SetSize(n);
	double dfMatchBoltD = 0.0;
	for (i = 0; i < n; i++)
	{
		if (ReadEffectLineText(line_txt, 200, fp) == NULL)
			break;
		serial = 0;		//������
		m_arrRodSpec[i].iSizeSpecType = 1;	//�˼�������ͣ�5����ʾ�ֹ�;1��������5��ʾ�Ǹ�
		sscanf(line_txt, "%lf%lf%lf%lf%lf%lf%d%d", &m_arrRodSpec[i].size, &m_arrRodSpec[i].area,
			&m_arrRodSpec[i].minR, &m_arrRodSpec[i].paraR, &m_arrRodSpec[i].interR, &dfMatchBoltD, &m_arrRodSpec[i].iSizeSpecType, &serial);
		m_arrRodSpec.m_pData[i].iSizeSpecType = abs(m_arrRodSpec.m_pData[i].iSizeSpecType);	//-5��ʾ�к���ĸֹܣ�ͳһ����Ϊ�ֹ�
	}
	m_bLoadDataIniFile = true;
	return true;
}
struct ANGLE_SECTION
{
	char cSubType;	//'D'˫ƴ�Խ����;'T'˫ƴT�����;'X'��ƴʮ�����
	double width,thick,innerR;
	double Ix,Wx,Rx,Ry0;
	ANGLE_SECTION(double wing_wide=0,double wing_thick=0)
	{
		width=wing_wide;
		thick=wing_thick;
	}
	double QueryInnerR()
	{
		if(width<=45)
			return innerR=5;
		else if(width<=50)
			return innerR=5.5;
		else if(width<=56)
			return innerR=6;
		else if(width<=63)
			return innerR=7;
		else if(width<=70)
			return innerR=8;
		else if(width<=80)
			return innerR=9;
		else if(width<=90)
			return innerR=10;
		else if(width<=110)
			return innerR=12;
		else if(width<=140)
			return innerR=14;
		else if(width<=180)
			return innerR=16;
		else //if(width<=200)
			return innerR=18;
	}
};
double CalAngleSectionZ0(double width,double thick)
{
	//ANGLE_SECTION section(width,thick);
	//section.innerR=section.QueryInnerR();
	double A1=width*thick,A2=(width-thick)*thick;
	double z0=(A1*width*0.5+A2*thick*0.5)/(A1+A2);
	return z0;
	//section.Wx=section.Ix/(width-z0);
	//double Ix,Wx,Rx,Ry0;
	//return section;
}

bool CTtaDataFile::ClassifyRodSizeSpecs(int iDefaultSizeSpecType/*=1*/,BYTE ciFileType/*=0*/)
{
	int nTemp=0,serial=0;
	int i,angle_size_n=0,tube_size_n=0;
	for(i=0;i<m_arrRodSpec.GetSize();i++)
	{
		int iSizeSpecType=iDefaultSizeSpecType;
		if(ciFileType== FILETYPE_TTA||ciFileType== FILETYPE_MYL|| ciFileType == FILETYPE_TANS)
			iSizeSpecType=m_arrRodSpec[i].iSizeSpecType;	//�˼�������ͣ�5����ʾ�ֹ�;1��������5��ʾ�Ǹ�
		int index=i;
		if(iSizeSpecType==5)
		{	//�ֹ����͹��
			index=tube_size_n;
			tube_size_n++;
			tubeguige_N=tube_size_n;
			if(index>=200)
			{
				if(index==200)
	#ifdef AFX_TARG_ENU_ENGLISH
					logerr.Log("The tube library can only store 200 specification records.");
	#else
					logerr.Log("ĿǰϵͳĬ�ϸֹܹ��������ֻ�ܴ洢200���ֹܹ���¼");
	#endif
				continue;
			}
			tubeguige_table[index].D =(int)m_arrRodSpec[i].size;
			tubeguige_table[index].thick=(m_arrRodSpec[i].size-(int)m_arrRodSpec[i].size)*100;
			tubeguige_table[index].area=m_arrRodSpec[i].area;
			tubeguige_table[index].r=m_arrRodSpec[i].minR;
			tubeguige_table[index].r=m_arrRodSpec[i].paraR;
			//tubeguige_table[index].r =m_arrRodSpec[i].interR;
			tubeguige_table[index].theroy_weight=tubeguige_table[index].area*0.785; // �����ܶ�*ƽ�����׵�ƽ���׵Ľ���ת��=0.785
			tubeguige_table[index].I=tubeguige_table[index].r*tubeguige_table[index].r*tubeguige_table[index].area;
			tubeguige_table[index].W=0;
		}
		else
		{	//�Ǹ����͹��
			index=angle_size_n;
			angle_size_n++;
			AngleLibrary()->SetAngleCount(angle_size_n);	//Ŀǰ�Ǹֿ���֧�����޽Ǹֹ���¼�� wjh-2019.09.11
			nTemp=int(m_arrRodSpec[i].size)/1000;
			if(nTemp==1||nTemp==0)
				jgguige_table[index].cType ='L';
			else if(nTemp==2)
				jgguige_table[index].cType='D';
			else if(nTemp==4)
				jgguige_table[index].cType='X';
			jgguige_table[index].wing_wide =int(m_arrRodSpec[i].size)%1000;
			jgguige_table[index].wing_thick=(m_arrRodSpec[i].size-(int)m_arrRodSpec[i].size)*100;
			jgguige_table[index].area=m_arrRodSpec[i].area;
			jgguige_table[index].Ry0=m_arrRodSpec[i].minR;
			jgguige_table[index].Rx=m_arrRodSpec[i].paraR;
			jgguige_table[index].r =m_arrRodSpec[i].interR;
			jgguige_table[index].theroy_weight=jgguige_table[index].area*0.785; // �����ܶ�*ƽ�����׵�ƽ���׵Ľ���ת��=0.785
			jgguige_table[index].Ix=0.01*ftoi(100*jgguige_table[index].Rx*jgguige_table[index].Rx*jgguige_table[index].area);
			jgguige_table[index].Iy0=0.01*ftoi(100*jgguige_table[index].Ry0*jgguige_table[index].Ry0*jgguige_table[index].area);
			double z0=0;
			if(jgguige_table[index].cType=='L'||jgguige_table[index].cType=='T')
				z0=CalAngleSectionZ0(jgguige_table[index].wing_wide,jgguige_table[index].wing_thick);	//mm
			jgguige_table[index].Wx=0.01*ftoi(1000*jgguige_table[index].Ix/(jgguige_table[index].wing_wide-z0));//cm3
			jgguige_table[index].Wy0=0;
		}
	}
	return true;
}
#ifdef __TIMER_COUNT_
#include "TimerCount.h"
extern CTimerCount timer;
#endif
bool ParseMylCfgWireNodeLineText(char* lineText,CTtaDataFile::MYLCFG_WIRENODE* pWireNode)
{
	int indexOfKey=0;
	char *iter,*pszKeyStart=lineText;
	CXhChar50 itemtext;
	for(iter=lineText;*iter!=0;iter++)
	{
		if(*iter==',')
		{
			int nstr=iter-pszKeyStart;
			itemtext.NCopy(pszKeyStart,nstr);
			if(indexOfKey==0)
				pWireNode->iWirePlacePointI=atoi(itemtext);
			else if(indexOfKey==1)
				pWireNode->wirename=itemtext;
			//else if(indexOfKey==2)//ĿǰΪ�հײ�֪��ʲô���� wjh-2018.7.1
			else if(indexOfKey==3)
				pWireNode->xLocation.x=atof(itemtext)*1000;
			else if(indexOfKey==4)
				pWireNode->xLocation.y=atof(itemtext)*1000;
			else if(indexOfKey==5)
				pWireNode->xLocation.z=atof(itemtext)*1000;
			//else if(indexOfKey==6)//ĿǰΪ�հײ�֪��ʲô���� wjh-2018.7.1
			else if(indexOfKey==7)	//��ѹ�ȼ�
				pWireNode->voltage=atoi(itemtext);
			indexOfKey++;
			pszKeyStart=iter+1;
		}
	}
	return indexOfKey>=7&&pWireNode->wirename.Length>0;
}
//pNode:�ϲ�����Ҫ�����Ľڵ�;pOthNode:�ϲ�����Ҫɾ���Ľڵ� defined in OutputInfo.cpp
bool MergeOverlapNodes(CLDSNode *pReserveNode,CLDSNode *pMergeNode,COutputInfo *pOutputInfo=NULL,bool updateRelaRodSolid=true);
bool ResetModuleNameByHeight();	//defined in TowerTreeDlg.cpp
bool SortModuleByHeight();		//defined in TowerTreeDlg.cpp
GTM_FZC_FNAME::GTM_FZC_FNAME(const char* pcszFileName/*=NULL*/,char _ciFzcPanelType/*=0*/)
{
	Clear();
	fname.Copy(pcszFileName);
	ciFzcPanelType=_ciFzcPanelType;
}
void GTM_FZC_FNAME::Clear()
{
	liPointA=liPointB=0;
	ciPrefixSymbol=0;
	ciFzcPanelType=0;
	subleg.liHeight=subleg.liLegDiff=subleg.liVTopPointA,subleg.liVTopPointB=0;
}
void TtaFileIn(LPCTSTR lpszPathName/*=NULL*/,char ciFileFormatType/*=-1*/)
{
	CLDSDoc *pDoc=theApp.GetLDSDoc();
	CXhChar200 data_file,ini_file,outfile,mylcfgfile;
	if(lpszPathName==NULL)
	{
		CXhChar500 filter= "TTA�����ļ�(*.*)|*.*|"\
			"MYL�����ļ�(*.myl)|*.myl|"\
			"SmartTower�����ļ�(*.dat)|*.dat|"\
			"����TANS�����ļ�(*.*)|*.*|"\
			"�㶫ԺGTower�����ļ�(*.gtm)|*.gtm||";
		data_file==pDoc->GetPathName();
		CFileDialog dlg(TRUE,"",data_file,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filter);
		if(dlg.DoModal()!=IDOK)
			return;
		data_file=dlg.GetPathName();
		CXhChar50 xarrFileFormats[16], xarrFileFormatItems[32];
		UINT niFilterCount = DELIMITER_STR::ParseWordsFromStr(filter, "|", xarrFileFormatItems, 32);
		for (UINT i = 0; i < niFilterCount; i++)
		{
			if (i % 2 == 0)
				xarrFileFormats[i / 2] = xarrFileFormatItems[i];
		}
		ciFileFormatType = -1;
		CXhChar50 szCurrFormat = xarrFileFormats[dlg.m_ofn.nFilterIndex - 1];
		if (strstr(szCurrFormat, "TTA") != NULL)	//"������"
			ciFileFormatType = -1;// CTtaDataFile::FILETYPE_TTA;��ʱ��������ļ���չ���ж��ļ���ʽ���� wjh-2019.7.10
		if (strstr(szCurrFormat, "TANS") != NULL)	//"������"
			ciFileFormatType = CTtaDataFile::FILETYPE_TANS;
	}
	else
		data_file=lpszPathName;
	mylcfgfile=outfile=ini_file=data_file;
	char* pszExtSpliter=SearchChar(ini_file,'.',true);
	BYTE ciFileType=-1;
	if (ciFileFormatType < 0)
	{
		if (pszExtSpliter == NULL)
			ciFileType = CTtaDataFile::FILETYPE_TTA;
		else if (stricmp(pszExtSpliter, ".myl") == 0)
			ciFileType = CTtaDataFile::FILETYPE_MYL;	//1.Myl
		else if (stricmp(pszExtSpliter, ".dat") == 0)
			ciFileType = CTtaDataFile::FILETYPE_ST;	//2.SmartTower
		else if (stricmp(pszExtSpliter, ".tans") == 0)
			ciFileType = CTtaDataFile::FILETYPE_TANS;//3
		else if (stricmp(pszExtSpliter, ".gtm") == 0)
			ciFileType = CTtaDataFile::FILETYPE_GTOM;//4
	}
	else
		ciFileType = ciFileFormatType;
	char* pszSplashSpliter=SearchChar(ini_file,'\\',true);
	if(pszSplashSpliter==NULL)
		pszSplashSpliter=SearchChar(ini_file,'/',true);
	if(pszSplashSpliter)
		*(pszSplashSpliter+1)=0;
	if(ciFileType!=2)
	{
		bool inifile_finded=false;
		if(pszSplashSpliter)
		{
			ini_file.Append("data.ini");
			WIN32_FIND_DATA findfile;
			if(FindFirstFile(ini_file,&findfile)!=NULL)
				inifile_finded=true;
		}
		if(!inifile_finded)
		{
			CFileDialog dlg(TRUE,"ini",ini_file,
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				"����ģ�������ļ�(*.ini)|*.ini||");
			if(dlg.DoModal()!=IDOK)
				return;
			ini_file=dlg.GetPathName();
		}
	}
	CTtaDataFile tta;
	Ta.Empty();
	Ta.InitTower();
	console.InitTowerModel();
	console.DispNodeSet.Empty();
	console.DispPartSet.Empty();
	console.DispBlockRefSet.Empty();
	console.DispFoundationSet.Empty();
	g_pSolidDraw->BuildDisplayList(NULL);
	CLogErrorLife life(&logerr);
	//1.���������ļ�
	FILE *fp=NULL;
	if(ciFileType!=2)
	{
		if((fp=fopen(ini_file,"rt"))!=NULL)
		{
			CSelfCloseFile selfclose(fp);
			tta.ReadTowerTypeCfgFile(fp, ciFileType);
			selfclose.EarlyCloseFile();
		}
		else if(ciFileType!=CTtaDataFile::FILETYPE_TANS)
		{	//TANS����KIND��λΪ6��9ʱ�������¸�ʽ"�ļ���_Data.ini" wjh-2019.7.8
			AfxMessageBox("δ�ҵ�Data.ini�ļ�");
			return;
		}
	}
	//2.����ģ�������ļ�
	theApp.GetLDSDoc()->SetPathName(data_file);
	theApp.GetLDSDoc()->SetModifiedFlag();  // dirty during de-serialize
	//CString filepath=dlg.GetPathName();
	fp=fopen(data_file,"rt");
	if(fp==NULL)
		return;
#ifdef __TIMER_COUNT_
	timer.Start();
#endif
	//����δ��򵥶�λģʽ����ʱ�����������Ǹְڷ�λ�ò����� wjh-2019.6.28
	CStackVariant<BOOL> stackAnglePreferSimpleMode(&g_sysPara.m_bAnglePreferSimplePosMode, TRUE);
	tta.ReadDataFile(fp,ciFileType,data_file);
#ifdef __TIMER_COUNT_
	timer.End();
	FILE* timerfp=fopen("d:\\ReadTtaData.txt","wt");
	for(int iKey=1;iKey<100;iKey++)
	{
		DWORD *pCost=timer.hashTicks.GetValue(iKey);
		if(pCost)
			fprintf(timerfp,"%2d:%.3fs\n",iKey,*pCost*0.001);
	}
	fprintf(timerfp,"sum cos %.3fs\n",(timer.GetEndTicks()-timer.GetStartTicks())*0.001);
	fclose(timerfp);
#endif
	fclose(fp);
	if(ciFileType==1)
	{	//�������cfg�ļ��еĺ��ؽڵ�
		pszExtSpliter=SearchChar(mylcfgfile,'.',true);
		*pszExtSpliter=0;
		strcpy(pszExtSpliter,".cfg");
		FILE* cfgfp=fopen(mylcfgfile,"rt");
		if(cfgfp!=NULL)
		{
			CXhChar200 line_txt;
			bool findWireNodeMarker=false,bWireNodeSectionClose=false;
			CXhSimpleList<CTtaDataFile::MYLCFG_WIRENODE> listCfgWireNodes;
			CFGWORD allbodyword;
			CLDSNode* pNode;
			for(CLDSModule* pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
				allbodyword.AddBodyLegs(pModule->GetBodyNo());
			do{
				if(ReadEffectLineText(line_txt,200,cfgfp)==NULL)
					break;
				if(!findWireNodeMarker&&strstr(line_txt,"ģ�͹ҵ�")!=NULL)
					findWireNodeMarker=true;
				else if(findWireNodeMarker&&!bWireNodeSectionClose)
				{
					if(strchr(line_txt,'[')!=NULL&&strchr(line_txt,']')!=NULL)
					{
						bWireNodeSectionClose=true;
						break;
					}
					else
					{
						CTtaDataFile::MYLCFG_WIRENODE *pWireNode,wirenode;
						if(ParseMylCfgWireNodeLineText(line_txt,&wirenode))
						{
							pWireNode=listCfgWireNodes.Append(wirenode);
							for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
							{
								if(pNode->xOriginalPos.IsEqual(wirenode.xLocation,1))
								{
									pWireNode->pRelaNode=pNode;
									pNode->m_sHangWireDesc=pWireNode->wirename;
									char* pszWireType=NULL;
									if((pszWireType=strstr(pWireNode->wirename,"��"))!=NULL)
										pNode->wireplace.ciWireType=pWireNode->ciWireType='C';
									else if((pszWireType=strstr(pWireNode->wirename,"��"))!=NULL)
										pNode->wireplace.ciWireType=pWireNode->ciWireType='E';
									else if((pszWireType=strstr(pWireNode->wirename,"��"))!=NULL)
										pNode->wireplace.ciWireType=pWireNode->ciWireType='J';
									pszWireType+=2;
									if(*pszWireType>='0'&&*pszWireType<='9')
										pNode->wireplace.ciPhaseSerial=*pszWireType-'0';
									break;
								}
							}
							if(pNode==NULL)
							{
								pNode=Ta.Node.append();
								pNode->cfgword=allbodyword;
								pNode->SetPosition(pWireNode->xLocation);
								pWireNode->pRelaNode=pNode;
								pNode->m_sHangWireDesc=pWireNode->wirename;
								char* pszWireType=NULL;
								if((pszWireType=strstr(pWireNode->wirename,"��"))!=NULL)
									pNode->wireplace.ciWireType=pWireNode->ciWireType='C';
								else if((pszWireType=strstr(pWireNode->wirename,"��"))!=NULL)
									pNode->wireplace.ciWireType=pWireNode->ciWireType='E';
								else if((pszWireType=strstr(pWireNode->wirename,"��"))!=NULL)
									pNode->wireplace.ciWireType=pWireNode->ciWireType='J';
								if(pszWireType!=NULL)
								{
									pszWireType+=2;
									BYTE serial=0;
									if(*pszWireType>='0'&&*pszWireType<='9')
										serial=*pszWireType-'0';
									if(pWireNode->ciWireType=='E')
										pNode->wireplace.ciPhaseSerial=serial;
									else
									{
										pNode->wireplace.ciCircuitSerial=serial/3+1;
										pNode->wireplace.ciPhaseSerial=serial%3;
									}
								}
							}
						}
					}
				}
			}while(true);
			fclose(cfgfp);
			if(listCfgWireNodes.Count>=4)
			{
				for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
					pNode->wireplace.ciWireType=0;
				for(CTtaDataFile::MYLCFG_WIRENODE* pWireNode=listCfgWireNodes.EnumObjectFirst();pWireNode;pWireNode=listCfgWireNodes.EnumObjectNext())
					pWireNode->pRelaNode->wireplace.ciWireType=pWireNode->ciWireType;
			}
		}
	}
#ifdef __SMART_DRAW_INC_
	//�Զ�����ѧ����ת��Ϊʵ��˼�
	logerr.GlobalEnabled(false);
	CLDSLinePart* pRod;
	int i,count=0;
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		count++;
	DisplayProcess(0,"���ɸ˼�ʵ��...");
	for(pRod=Ta.EnumRodFirst(),i=0;pRod;pRod=Ta.EnumRodNext(),i++)
	{
		if(pRod->GetClassTypeId()!=pRod->size_idClassType&&pRod->size_idClassType!=0)			//ת����������
		{
			pRod=(CLDSLinePart*)Ta.Parts.InstantiateCursorLinePart();
		}
		int process=i*100/count;
		DisplayProcess(process,"���ɸ˼�ʵ��...");
	}
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		pRod->IntelliUpdateConnBoltsBySummBolts();
	DisplayProcess(100,"���ɸ˼�ʵ��...");
	logerr.GlobalEnabled(true);
#endif
	SortModuleByHeight();
	ResetModuleNameByHeight();
	//���ݹ�ӽ������Ĭ����ߺ������Ϊ�������ģ�ͺ�ĳ�ʼ�趨����ʦ������ wjh-2019.7.9
	CLDSModule* pHighestModule = Ta.Module.GetTail();
	Ta.m_hActiveModule = pHighestModule->handle;
	//WORD wnMaxLegs=CFGLEG::MaxLegs();
	BYTE ciStartAddr=CFGLEG::BitAddrIndexOf((BYTE)pHighestModule->idBodyNo);
	pHighestModule->m_arrActiveQuadLegNo[0] = pHighestModule->m_arrActiveQuadLegNo[1] =
		pHighestModule->m_arrActiveQuadLegNo[2] = pHighestModule->m_arrActiveQuadLegNo[3] = ciStartAddr + 1;
	//ˢ�������ͼ�����ǰ���ͼ
	CTowerTreeDlg *pTreeView = (CTowerTreeDlg*)((CMainFrame*)AfxGetMainWnd())->GetTowerTreePage();
	pTreeView->RefreshTreeCtrl();
}
#include "GlobalFunc.h"
void TtaFileExport()
{
	FILE* fp;
	CTtaDataFile tta;
	CLDSDoc *pDoc=theApp.GetLDSDoc();
	CString filepath=pDoc->GetPathName();
	filepath=filepath.Left(filepath.GetLength()-4);
	filepath+=".dat";
	CFileDialog dlg2(FALSE,"",filepath,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"TTA�����ļ�(*.*)|*.*||");
	if(dlg2.DoModal()!=IDOK)
		return;
	CLogErrorLife life;
	filepath=dlg2.GetPathName();
	if((fp=fopen(filepath,"wt"))!=NULL)
	{
		tta.WriteDataFile(fp);
		fclose(fp);
	}
	//д��data.ini�ļ�
	CXhChar200 inifile=filepath;
	char* separator1=SearchChar(inifile,'/',true);
	char* separator2=SearchChar(inifile,'\\',true);
	char* separator=max(separator1,separator2);
	*(separator+1)=0;
	inifile.Append("data.ini");
	fp=fopen(inifile,"wt");
	if(fp==NULL)
		return;
	char* header=
		"    0.050     0.175 -------IF UNBALANCE LOAD EXCEED THE VALUE,THE UNBALANCED LOAD WILL BE DISPLAYED.\n"
		"    3    3    3     -------MIN. THICKNESS OF LEG MEMBER,STRESSED OTHER MEMBERS & UNSTRESSED MEMBERS.\n"
		"    21.500    20.500    20.000  ----ALLOWABLE STRESS OF CHINESE STANDARD STEEL      Q235\n"
		"    31.000    29.500    26.500  WABLE STRESS OF CHINESE HIGH TENSILE STRENGTH STEEL Q345\n"
		"    35.000    33.500    31.500  WABLE STRESS OF CHINESE HIGH TENSILE STRENGTH STEEL Q390\n"
		"    38.000    36.000    34.000  WABLE STRESS OF CHINESE HIGH TENSILE STRENGTH STEEL Q420\n"
		"    41.500    39.500    38.000  WABLE STRESS OF CHINESE HIGH TENSILE STRENGTH STEEL Q460\n"
		"    23.500          -------YIELD STRENGTH OF STANDARD STEEL\n"
		"    34.500          -------YIELD STRENGTH OF HIGH TENSILE STRENGTH STEEL Q345\n"
		"    39.000          -------YIELD STRENGTH OF HIGH TENSILE STRENGTH STEEL Q390\n"
		"    42.000          -------YIELD STRENGTH OF HIGH TENSILE STRENGTH STEEL Q420\n"
		"    46.000          -------YIELD STRENGTH OF HIGH TENSILE STRENGTH STEEL Q460\n"
		"   150.000          DLM:  ALLOWABLE SLANDERNESS RATIO OF MAIN MEMBER\n"
		"   250.000          DLA:  ALLOWABLE SLANDERNESS RATIO OF MAIN AUX. MEMBER\n"
		"   400.000          DLT:  ALLOWABLE SLANDERNESS RATIO OF TENSION-ONLY MEMBER\n"
		"   200.000          DLL1: ALLOWABLE SLANDERNESS RATIO OF BRACING MEMBER IN LEG PART-CHINA\n"
		"   200.000          DLL2: ALLOWABLE SLANDERNESS RATIO OF BRACING MEMBER IN OTHER PART-CHINA\n"
		"   200.000          DLL:  ALLOWABLE SLANDERNESS RATIO OF BRACING MEMBER ASCE\n"
		"    37.000    51.000    53.000    56.000    56.000                        READ(100,'(2F10.3)')Q235,Q345,Q390,Q420,Q460\n"
		"   63  180  110     IWT1: IF WIDTH OF ANGLE LESS THAN IWT1, FIRST KIND OF BOLT WILLBE USED\n"
		"   16    24.000    30.000     6.8M                READ(100,'(I5,2F10.3,5X,A4)')ND1,BS1,BS1T,CGD1\n"
		"   20    24.000    30.000     6.8M                READ(100,'(I5,2F10.3,5X,A4)')ND2,BS2,BS2T,CGD2\n"
		"   20    24.000    30.000     6.8M                READ(100,'(I5,2F10.3,5X,A4)')ND3,BS3,BS3T,CGD3\n"
		"   20    24.000    30.000     6.8M                READ(100,'(I5,2F10.3,5X,A4)')ND4,BS4,BS4T,CGD4  8.8G\n"
		"   20    24.000    30.000     6.8M                READ(100,'(I5,2F10.3,5X,A4)')ND5,BS5,BS5T,CGD5  8.8G\n"
		"     1.000     2.000     1.000     1.000     READ(100,'(15,4F10.3)')IWT,FMNS1,FMNS2,FMNS3,FMNS4\n";
	fprintf(fp,header);
	bool hasTubeRod=false,hasAngleRod=false;
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		if(pRod->size_idClassType==CLS_LINETUBE)
			hasTubeRod=true;
		else if(pRod->size_idClassType==CLS_LINEANGLE||pRod->size_idClassType==CLS_GROUPLINEANGLE)
			hasAngleRod=true;
	}
	int size_N=0;
	if(hasAngleRod)
		size_N+=jgguige_N;
	if(hasTubeRod)
		size_N+=tubeguige_N;
	fprintf(fp,"  %3d   63    1.150     1.100  1.200\n",jgguige_N+tubeguige_N);
	int serial=1;
	if(hasAngleRod)
	{
		for(int i=0;i<jgguige_N;i++)
		{
			JG_PARA_TYPE* pAngleSize=&jgguige_table[i];
			double anglesize=pAngleSize->wing_wide;
			if(pAngleSize->cType=='X')
				anglesize+=4000;
			else if(pAngleSize->cType=='D'||pAngleSize->cType=='T')
				anglesize+=2000;
			anglesize+=pAngleSize->wing_thick/100;
			fprintf(fp,"  %8.3f   %8.3f   %8.3f   %8.3f   %8.3f  %5d\n",anglesize,pAngleSize->area,
				pAngleSize->Ry0,pAngleSize->Rx,pAngleSize->r,serial);
			serial++;
		}
	}
	if(hasTubeRod)
	{
		for(int i=0;i<tubeguige_N;i++)
		{
			TUBE_PARA_TYPE* pTubeSize=&tubeguige_table[i];
			double tubesize=pTubeSize->D;
			tubesize+=pTubeSize->thick/100;
			fprintf(fp,"  %8.3f   %8.3f   %8.3f   %8.3f   %8.3f  %5d\n",tubesize,pTubeSize->area,
				pTubeSize->r,pTubeSize->r,0.0,serial);
			serial++;
		}
	}
	fclose(fp);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//��Ҫ�������ǰ����
static int CompareTtaNode(const TTA_NODEPTR& pNode1,const TTA_NODEPTR& pNode2)
{
	if(pNode1->pAttachStart==pNode2||pNode1->pAttachEnd==pNode2)
		return 1;
	else if(pNode2->pAttachStart==pNode1||pNode2->pAttachEnd==pNode1)
		return -1;
	else if(pNode1->pNode->point_i>pNode2->pNode->point_i)
		return 1;
	else if(pNode1->pNode->point_i==pNode2->pNode->point_i)
		return 0;
	else
		return -1;
}

CTtaDataFile::CTtaDataFile(CTower* pBelongModel/*=NULL*/)
{
	m_pContextModel=pBelongModel!=NULL?pBelongModel:&Ta;
	m_ciFileType=0;
	m_bLoadDataIniFile = false;
	KIND=1004;	//�����ڼ����涨ѡ�ģ����Զ������������������
	N10=0;	//����һ�����뷽ʽ����Ľڵ���������
	N20=0;	//���ڶ������뷽ʽ����Ľڵ���������
	M10=0;	//�����ĵ���������
	M20=0;	//�����ĵ������������ݲ���������ģ�
	IE=1;
	NL=42;	//���������
	NC=16;	//���ؽڵ���
	NW=0;
	NSEC=0;
	K1=-201;
	K2=NL;
	K3=K4=1;
	K5=1;
	K6=1;
	NA=0;
	TSM=0;	//����
	REN= 1;	//�Ż��ڵ��ţ��Ҳ��̳�
	NGP=-1;
	NBY=1;
	IW=125;
	HOLECOEF_ARR[0]=2.0;	//Angle width<IW
	HOLECOEF_ARR[1]=2.23;	//Angle width>=IW
	HOLECOEF_ARR[2]=1.0;	//Common Primary Angle
	HOLECOEF_ARR[3]=1.0;	//Brace Angle
	WIND=VAL2=0;
	xarrSteelSymbols.Clear();
	memset(xarrSteelSymbols.pPresetObjs,0,xarrSteelSymbols.Count);
	m_nOverlapBraceNodes=0;
}
CTtaDataFile::IBOLT::IBOLT(DWORD uiBoltGrade/*=0*/)
{
	ParseFromDword(uiBoltGrade);
}
void CTtaDataFile::IBOLT::ParseFromDword(DWORD uiBoltGrade)
{
	fM12Grade=fM16Grade=fM20Grade=fM24Grade=6.8f;
	UINT ciGrade=uiBoltGrade/1000;
	if(ciGrade==1)
		fM12Grade=4.8f;
	else if(ciGrade==2)
		fM12Grade=5.8f;
	else if(ciGrade==3)
		fM12Grade=6.8f;
	else if(ciGrade==4)
		fM12Grade=8.8f;
	else if(ciGrade==5)
		fM12Grade=10.9f;
	//M16��˨�������
	uiBoltGrade%=1000;
	if(ciGrade==1)
		fM16Grade=4.8f;
	else if(ciGrade==2)
		fM16Grade=5.8f;
	else if(ciGrade==3)
		fM16Grade=6.8f;
	else if(ciGrade==4)
		fM16Grade=8.8f;
	else if(ciGrade==5)
		fM16Grade=10.9f;
	//M20��˨�������
	uiBoltGrade%=100;
	if(ciGrade==1)
		fM20Grade=4.8f;
	else if(ciGrade==2)
		fM20Grade=5.8f;
	else if(ciGrade==3)
		fM20Grade=6.8f;
	else if(ciGrade==4)
		fM20Grade=8.8f;
	else if(ciGrade==5)
		fM20Grade=10.9f;
	//M24��˨�������
	uiBoltGrade%=10;
	if(ciGrade==1)
		fM24Grade=4.8f;
	else if(ciGrade==2)
		fM24Grade=5.8f;
	else if(ciGrade==3)
		fM24Grade=6.8f;
	else if(ciGrade==4)
		fM24Grade=8.8f;
	else if(ciGrade==5)
		fM24Grade=10.9f;
}
float CTtaDataFile::IBOLT::GradeOfMd(short d)
{
	if(d==12)
		return fM12Grade;
	else if(d==16)
		return fM16Grade;
	else if(d==20)
		return fM20Grade;
	else if(d==24)
		return fM24Grade;
	else
		return 6.8f;
}

CTtaDataFile::~CTtaDataFile()
{

}
bool CTtaDataFile::MIRFLAG::EnableMirX(bool enabled/*=true*/)
{
	if(enabled)
		cbFlag|=0x02;
	else
		cbFlag&=0xfd;
	return enabled;
}
bool CTtaDataFile::MIRFLAG::EnableMirY(bool enabled/*=true*/)
{
	if(enabled)
		cbFlag|=0x01;
	else
		cbFlag&=0xfe;
	return enabled;
}
bool CTtaDataFile::MIRFLAG::EnableMirZ(bool enabled/*=true*/)
{
	if(enabled)
		cbFlag|=0x04;
	else
		cbFlag&=0xfb;
	return enabled;
}
CTtaDataFile::MIRFLAG* CTtaDataFile::GetMirFlag(long pointI)
{
	long serial=pointI/10;
	if(serial<1000)
	{
		if((arrMirFlags[serial].cbFlag&0x80)>0)	//δ��ʼ��
			return NULL;
		else
			return &arrMirFlags[serial];
	}
	else
		return hashMirFlags.GetValue(serial);
}
CTtaDataFile::MIRFLAG* CTtaDataFile::AddMirFlag(long pointI,BYTE cbMirFlag/*=0x8f*/)
{
	MIRFLAG* pMirFlag=NULL;
	long serial=pointI/10;
	if(serial<1000)
		pMirFlag=&arrMirFlags[serial];
	else
		pMirFlag=hashMirFlags.Add(serial);
	*pMirFlag=cbMirFlag;
	pMirFlag->cbFlag&=0x7f;
	return pMirFlag;
}
bool CTtaDataFile::IsEnableMirX(long pointI)
{
	MIRFLAG* pMirFlag=NULL;
	if(pointI<10000)
		pMirFlag=&arrMirFlags[pointI/10];
	else
		pMirFlag=hashMirFlags.Add(pointI/10);
	return pMirFlag->IsEnableMirX();
}
bool CTtaDataFile::IsEnableMirY(long pointI)
{
	MIRFLAG* pMirFlag=NULL;
	if(pointI<10000)
		pMirFlag=&arrMirFlags[pointI/10];
	else
		pMirFlag=hashMirFlags.Add(pointI/10);
	return pMirFlag->IsEnableMirY();
}
bool CTtaDataFile::IsEnableMirZ(long pointI)
{
	MIRFLAG* pMirFlag=NULL;
	if(pointI<10000)
		pMirFlag=&arrMirFlags[pointI/10];
	else
		pMirFlag=hashMirFlags.Add(pointI/10);
	return pMirFlag->IsEnableMirZ();
}
bool CTtaDataFile::EnableMirX(long pointI,bool enabled/*=true*/)
{
	MIRFLAG* pMirFlag=NULL;
	if(pointI<10000)
		pMirFlag=&arrMirFlags[pointI/10];
	else
		pMirFlag=hashMirFlags.Add(pointI/10);
	return pMirFlag->EnableMirX(enabled);
}
bool CTtaDataFile::EnableMirY(long pointI,bool enabled/*=true*/)
{
	MIRFLAG* pMirFlag=NULL;
	if(pointI<10000)
		pMirFlag=&arrMirFlags[pointI/10];
	else
		pMirFlag=hashMirFlags.Add(pointI/10);
	return pMirFlag->EnableMirY(enabled);
}
bool CTtaDataFile::EnableMirZ(long pointI,bool enabled/*=true*/)
{
	MIRFLAG* pMirFlag=NULL;
	if(pointI<10000)
		pMirFlag=&arrMirFlags[pointI/10];
	else
		pMirFlag=hashMirFlags.Add(pointI/10);
	return pMirFlag->EnableMirZ(enabled);
}
bool CTtaDataFile::AppendTtaNodePtr(ARRAY_LIST<TTA_NODEPTR>& arrNodePtr,TTA_NODE* pTtaNode)
{
	for(int i=0;i<arrNodePtr.GetSize();i++)
	{
		if(arrNodePtr[i]==pTtaNode)
			return false;	//�Ѳ���ýڵ�
	}
	if(pTtaNode->pNode->m_cPosCalType==6||pTtaNode->pNode->m_cPosCalType==7)
	{	//�ȸߵ��ֱƫ�Ƶ�
		if(hashTtaNodeH.GetValue(pTtaNode->pNode->arrRelationNode[1]))
			pTtaNode->pAttachStart=hashTtaNodeH[pTtaNode->pNode->arrRelationNode[1]];
		if(hashTtaNodeH.GetValue(pTtaNode->pNode->arrRelationNode[2]))
			pTtaNode->pAttachEnd=hashTtaNodeH[pTtaNode->pNode->arrRelationNode[2]];
	}
	else
	{
		if(hashTtaNodeH.GetValue(pTtaNode->pNode->arrRelationNode[0]))
			pTtaNode->pAttachStart=hashTtaNodeH[pTtaNode->pNode->arrRelationNode[0]];
		if(hashTtaNodeH.GetValue(pTtaNode->pNode->arrRelationNode[1]))
			pTtaNode->pAttachEnd=hashTtaNodeH[pTtaNode->pNode->arrRelationNode[1]];
	}
	if(pTtaNode->pAttachStart)
		AppendTtaNodePtr(arrNodePtr,pTtaNode->pAttachStart);	//�Ѵ���
	if(pTtaNode->pAttachEnd)
		AppendTtaNodePtr(arrNodePtr,pTtaNode->pAttachEnd);	//�Ѵ���
	arrNodePtr.append(pTtaNode);
	return true;		
}
//from Node.cpp
int MirPointI(int pointI,MIRMSG msg);
int reverse_compare_long(const long& item1,const long& item2)
{
	if(item1<item2)
		return 1;
	else if(item1>item2)
		return -1;
	else
		return 0;
}
void AppendTtaNodes(CLDSNode* pNode,CHashList<TTA_NODE>& hashTtaNodes)
{
	TTA_NODE* pTtaNode=hashTtaNodes.Add(pNode->handle);
	if(pTtaNode->pNode!=NULL)
		return;	//�����
	pTtaNode->pNode=pNode;
	pTtaNode->pos=pNode->Position(false)*0.001;
	if(pNode->m_cPosCalType==1)
		pTtaNode->posType=1;	//�˼���X����ֵ�����
	else if(pNode->m_cPosCalType==2)
		pTtaNode->posType=2;	//�˼���Y����ֵ�����
	else if(pNode->m_cPosCalType==3||pNode->m_cPosCalType==6||(pNode->m_cPosCalType==7&&pNode->m_cPosCalViceType=='Z'))
		pTtaNode->posType=3;	//�˼���Z����ֵ����㡢��ֱƫ�Ƶ㡢�ȸߵ�
	else if(pNode->m_cPosCalType==5)
	{	//�����ȷֵ�
		pTtaNode->posType=4;
		pTtaNode->coefScale=0;
		for(int j=2;j<8;j++)
		{
			double sum=pNode->attach_scale*j;
			if(fabs(sum-1)<EPS)
			{
				pTtaNode->coefScale=j;
				break;
			}
			else if(fabs(j-sum-1)<EPS)
			{	//��Ҫ����ʼĩ�˽ڵ��
				pTtaNode->coefScale=-j;
				break;
			}
		}
		if(pTtaNode->coefScale==0) 
			pTtaNode->coefScale=pNode->attach_scale;
	}
	else //if(pNode->m_cPosCalType==0)
		pTtaNode->posType=0;
	//���䵱ǰδ������ķ�Χ֮�ڵ������ڵ�(һ��Ϊ�����Ա��ӵĸ�����λ�ڵ�)
	if(pNode->arrRelationNode[0]>0x20&&hashTtaNodes.GetValue(pNode->arrRelationNode[0])==NULL)
	{
		CLDSNode* pAttachNode=Ta.Node.FromHandle(pNode->arrRelationNode[0]);
		if(pAttachNode)
			AppendTtaNodes(pAttachNode,hashTtaNodes);
	}
	if(pNode->arrRelationNode[1]>0x20&&hashTtaNodes.GetValue(pNode->arrRelationNode[1])==NULL)
	{
		CLDSNode* pAttachNode=Ta.Node.FromHandle(pNode->arrRelationNode[1]);
		if(pAttachNode)
			AppendTtaNodes(pAttachNode,hashTtaNodes);
	}
}
bool CTtaDataFile::PrepareTtaModel()
{
	int i;
	CLDSNode *pNode;
	CLDSModule *pModule;
	TTA_BODYLEG* pTtaBodyLeg;
	CPreProcessor processor;
	CFGWORD cfgwordBody,cfgwordLeg;
	for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		//cfgwordBody.flag.bytes[pModule->GetBodyNo()-1]=0xff;
		cfgwordBody.SetBodyLegs(pModule->GetBodyNo());
		cfgwordLeg.AddSpecWord(pModule->m_dwLegCfgWord);
	}
	processor.PrepareTower(&Ta,cfgwordBody,cfgwordLeg);
	//һ���ڵ���Ϣ
	TTA_NODE *pTtaNode;
	//1.1�����������Ķ˵�ӵ�TTA�ڵ㻺������m_arrTtaNode��
	for(pNode=processor.result.nodeSet.GetFirst();pNode;pNode=processor.result.nodeSet.GetNext())
	{
		AppendTtaNodes(pNode,hashTtaNodesByH);
	}
	m_arrTtaNode.SetSize(0,hashTtaNodesByH.GetNodeNum());
	for(pTtaNode=hashTtaNodesByH.GetFirst();pTtaNode;pTtaNode=hashTtaNodesByH.GetNext())
	{
		pTtaNode=m_arrTtaNode.append(*pTtaNode);
		hashTtaNodeH.SetValue(pTtaNode->pNode->handle,pTtaNode);
	}
	// 1.2��TTA�ڵ㻺�����������нڵ�������±�ţ���ͳ�Ʊ����������Ϣ���Գ���Ϣ���趨��ϣ��
	if(Ta.Module.GetNodeNum()==1)//&&StatActualModuleLegs(Ta.Module.GetFirst())==1)	//��һ������(һ�������)
	{	//ֻ��Ҫ��֤ÿ�����ȵĻ����ڵ�ž���������ģ����ÿ���ǻ����ڵ㼴��
		CXhSimpleList<TTA_NODEPTR> baseNodes;
		int maxBodyPointI=0;
		int maxBasePointI=0;	//��λ������
		for(i=0;i<m_arrTtaNode.GetSize();i++)
		{
			pTtaNode=&m_arrTtaNode[i];
			pTtaNode->pointI=pTtaNode->pNode->point_i;
			if(pTtaNode->pNode->IsLegObj()&&(((WORD)pTtaNode->pNode->restrict)&0x0007)==0x0007)
			{
				baseNodes.AttachNode(pTtaNode);
				maxBasePointI=max(maxBasePointI/10,pTtaNode->pointI/10)*10;
			}
			else if(maxBodyPointI<pTtaNode->pointI)
				maxBodyPointI=pTtaNode->pointI;
		}
		CHashList<long> hashBasePointI;
		int minBasePointI=maxBasePointI;
		for(LIST_NODE<TTA_NODEPTR>* pListNode=baseNodes.EnumFirst();pListNode;pListNode=baseNodes.EnumNext())
		{
			pTtaNode=pListNode->data;
			if(pTtaNode->pointI>=maxBodyPointI)
			{
				minBasePointI=min(minBasePointI,pTtaNode->pointI);
				continue;	//��ǰ�����ڵ����Ѵ���ȫ������ڵ�,�������±��
			}
			long *pMapPointI=hashBasePointI.GetValue(pTtaNode->pointI/10);
			if(pMapPointI)	//֮ǰ������ضԳƽڵ��������»����ڵ���
				pTtaNode->pointI=*pMapPointI+pTtaNode->pointI%10;
			else
			{
				maxBasePointI+=10;
				pTtaNode->pointI=maxBasePointI+pTtaNode->pointI%10;
				hashBasePointI.SetValue(pTtaNode->pointI/10,maxBasePointI);
			}
		}
		pTtaBodyLeg=m_arrBodyLeg.append();
		pTtaBodyLeg->ApplyListTtaNodeSet(this);
		pTtaBodyLeg->Fmax=maxBasePointI+3;
		pTtaBodyLeg->Jmin=pTtaBodyLeg->Fmin=minBasePointI;
		pTtaBodyLeg->MAX=pTtaBodyLeg->LB=pTtaBodyLeg->Var=0;
	}
	else
	{
		CHashList<long>hashBodyFlags;
		for(i=0;i<m_arrTtaNode.GetSize();i++)
		{	//ɸѡ��TTA�еĹ��ñ����Ӧ��32λ�����
			pTtaNode=&m_arrTtaNode[i];
			if(pTtaNode->pNode->IsLegObj())
				continue;	//��ǰ���������
			CFGLEG cfgflag=pTtaNode->pNode->cfgword.ToCfgLeg();
			DWORD bodyflag=cfgflag.BodyFlag();
			if(bodyflag==0)
			{
				logerr.Log("%d�ڵ����ĺ�δ�����κι���������Ϣ",pTtaNode->pNode->point_i);
				continue;
			}
			bool bModuleOnly=false;
			for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
			{
				if(bodyflag==GetSingleWord(pModule->GetBodyNo()))
				{
					bModuleOnly=true;
					break;	//��������ĳһ���ߵĽڵ��ݹ鵽TTA��������
				}
			}
			if(!bModuleOnly)
				hashBodyFlags.SetValue(bodyflag,bodyflag);
		}
		ARRAY_LIST<long> bodies(0,hashBodyFlags.GetNodeNum());
		m_arrBodyLeg.SetSize(0,hashBodyFlags.GetNodeNum()+Ta.Module.GetNodeNum());
		for(long* pBodyFlag=hashBodyFlags.GetFirst();pBodyFlag;pBodyFlag=hashBodyFlags.GetNext())
			bodies.append(*pBodyFlag);
		//�ɴ���С����
		CQuickSort<long>::QuickSort(bodies.m_pData,bodies.GetSize(),reverse_compare_long);
		//��ȡ���TTA����
		for(i=0;i<bodies.GetSize();i++)
		{
			if(i>0)
			{
				if((bodies[i]&bodies[i-1])!=bodies[i])	//��һ����δ��ǰһ����ȫ����������
				{
					CFGLEG diff;
					diff.SetBodyFlag(bodies[i]);
					CXhChar50 bodystr;
					diff.GetBodyScopeStr(bodystr);
					logerr.Log("���ٴ���һ���ڵ�ֻר����LDS{%s}���߱��壬������TTA���嶨�����, �ڵ���������������!",(char*)bodystr);
				}
			}
			pTtaBodyLeg=m_arrBodyLeg.append();
			pTtaBodyLeg->ApplyListTtaNodeSet(this);
			pTtaBodyLeg->ciType=0;
			pTtaBodyLeg->dwBodyFlag=bodies[i];
			CFGLEG cfgleg;
			cfgleg.SetBodyFlag(bodies[i]);
			pTtaBodyLeg->Fmax=0;
			for(int ii=0;ii<m_arrTtaNode.GetSize();ii++)
			{	//ɸѡ��TTA�еĹ��ñ����Ӧ��32λ�����
				pTtaNode=&m_arrTtaNode[ii];
				if(pTtaNode->pNode->IsLegObj())
					continue;	//��ǰ���������
				CFGLEG flag=pTtaNode->pNode->cfgword.ToCfgLeg();
				DWORD bodyflag=flag.BodyFlag();
				bool bModuleOnly=false;
				for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
				{
					if(bodyflag==GetSingleWord(pModule->GetBodyNo()))
					{
						bModuleOnly=true;
						break;	//��������ĳһ���ߵĽڵ��ݹ鵽TTA��������
					}
				}
				if(!bModuleOnly && (bodyflag&bodies[i])==bodies[i])
					pTtaBodyLeg->listTtaNodeSet.AttachNode(pTtaNode);
			}
		}
		//��ȡ���TTA���ȷ���
		for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			pTtaBodyLeg=m_arrBodyLeg.append();
			pTtaBodyLeg->ApplyListTtaNodeSet(this);
			pTtaBodyLeg->ciType=1;
			pTtaBodyLeg->dwBodyFlag=GetSingleWord(pModule->GetBodyNo());
			for(i=0;i<m_arrTtaNode.GetSize();i++)
			{	//ɸѡ��TTA�еĹ��ñ����Ӧ��32λ�����
				pTtaNode=&m_arrTtaNode[i];
				CFGLEG flag=pTtaNode->pNode->cfgword.ToCfgLeg();
				DWORD bodyflag=flag.BodyFlag();
				if(bodyflag==pTtaBodyLeg->dwBodyFlag)	//�����ڵ�ǰ���ߵĽڵ�
					pTtaBodyLeg->listTtaNodeSet.AttachNode(pTtaNode);
			}
		}
		//��TTA�����ģ�ͽ������½ڵ���
		for(i=0;i<m_arrTtaNode.GetSize();i++)
			m_arrTtaNode[i].pointI%=10;	//����TTA�ڵ���
		long serial=10;
		for(pTtaBodyLeg=m_arrBodyLeg.GetFirst();pTtaBodyLeg;pTtaBodyLeg=m_arrBodyLeg.GetNext())
		{
			long maxBodyQuadPointI=0;
			long minLegNodeI=1000000,minBaseNodeI=1000000,maxBaseNodeI=0;
			for(LIST_NODE<TTA_NODEPTR>* pListNode=pTtaBodyLeg->listTtaNodeSet.EnumFirst();pListNode;pListNode=pTtaBodyLeg->listTtaNodeSet.EnumNext())
			{
				pTtaNode=pListNode->data;
				if(pTtaNode->pointI>=10)
					continue;	//���ýڵ�֮ǰ�ѽ��б��
				if(!pTtaNode->pNode->IsFatherObject())
					continue;	//�ǶԳƸ��ڵ����ڸ��ڵ㴦��ͬʱ������
				pTtaNode->pointI=serial+CalPointQuadPos(pTtaNode->pNode->Position(false))-1;
				serial+=10;
				//���ƶԳƽڵ���
				long hMirX=pTtaNode->pNode->GetRelativeMirObj(MIRMSG(1),FALSE);
				long hMirY=pTtaNode->pNode->GetRelativeMirObj(MIRMSG(2),FALSE);
				long hMirZ=pTtaNode->pNode->GetRelativeMirObj(MIRMSG(3),FALSE);
				pTtaNode->IS=0;
				if(hMirX>0&&hMirY>0&&hMirZ>0&&hMirX!=hMirY)
					pTtaNode->IS=4;
				else if(hMirX>0)
					pTtaNode->IS=2;
				else if(hMirY>0)
					pTtaNode->IS=1;
				else if(hMirZ>0)
					pTtaNode->IS=3;
				else
					pTtaNode->IS=0;
				hashTtaNodeI.SetValue(pTtaNode->pNode->point_i,pTtaNode);
				TTA_NODEPTR* ppTtaNode=NULL;
				if(hMirX>0&&hMirX!=pTtaNode->pNode->handle&&(ppTtaNode=hashTtaNodeH.GetValue(hMirX)))
				{
					(*ppTtaNode)->pointI=MirPointI(pTtaNode->pointI,MIRMSG(1));
					hashTtaNodeI.SetValue((*ppTtaNode)->pNode->point_i,*ppTtaNode);
				}
				if(hMirY>0&&hMirY!=pTtaNode->pNode->handle&&(ppTtaNode=hashTtaNodeH.GetValue(hMirY)))
				{
					(*ppTtaNode)->pointI=MirPointI(pTtaNode->pointI,MIRMSG(2));
					hashTtaNodeI.SetValue((*ppTtaNode)->pNode->point_i,*ppTtaNode);
				}
				if( hMirZ>0&&(ppTtaNode=hashTtaNodeH.GetValue(hMirZ))&&
					hMirZ!=pTtaNode->pNode->handle&&hMirZ!=hMirX&&hMirZ!=hMirY)
				{
					(*ppTtaNode)->pointI=MirPointI(pTtaNode->pointI,MIRMSG(3));
					hashTtaNodeI.SetValue((*ppTtaNode)->pNode->point_i,*ppTtaNode);
				}
				if(pTtaBodyLeg->ciType==0)	//TTA����
				{
					//if(isdigit(pTtaNode->pNode->Layer(2)))
						maxBodyQuadPointI=max(maxBodyQuadPointI,pTtaNode->pointI);
				}
				else	//���Ȳ���
				{
					if(pTtaNode->pointI<minLegNodeI)
						minLegNodeI=pTtaNode->pointI;
					if(pTtaNode->pNode->restrict.GetRestrictWord()>0)
					{	//����Լ���ڵ�
						if(pTtaNode->pointI<minBaseNodeI)
							minBaseNodeI=pTtaNode->pointI;
						if(pTtaNode->pointI>maxBaseNodeI)
							maxBaseNodeI=pTtaNode->pointI;
					}
				}
			}
			if(pTtaBodyLeg->ciType==0)
			{
				pTtaBodyLeg->Jmin=pTtaBodyLeg->Fmin=maxBodyQuadPointI-maxBodyQuadPointI%10;
				pTtaBodyLeg->Fmax=maxBodyQuadPointI-maxBodyQuadPointI%10+3;
				pTtaBodyLeg->LB=pTtaBodyLeg->MAX=pTtaBodyLeg->Var=0;
			}
			else
			{
				pTtaBodyLeg->Jmin=minLegNodeI;
				pTtaBodyLeg->Fmin=minBaseNodeI;
				pTtaBodyLeg->Fmax=maxBaseNodeI-maxBaseNodeI%10+3;
				pTtaBodyLeg->LB=(maxBaseNodeI/10-minBaseNodeI/10)*10;//��ǰ���ȷ����а����Ľ�����
				pTtaBodyLeg->MAX=pTtaBodyLeg->Var=0;
				for(int iBody=0;iBody<m_arrBodyLeg.GetSize();iBody++)
				{
					if(m_arrBodyLeg[iBody].ciType!=0)
						break;	//���岿�����Ѳ����
					if(m_arrBodyLeg[iBody].dwBodyFlag&pTtaBodyLeg->dwBodyFlag)
						pTtaBodyLeg->LB=pTtaBodyLeg->LB-pTtaBodyLeg->LB%10+iBody+1;
				}
			}
		}
		for(i=0;i<m_arrTtaNode.GetSize();i++)
		{
			if(m_arrTtaNode[i].pointI<10)
			{
				m_arrTtaNode[i].pointI+=serial;	//һЩδ�������κα���ͽ����еĸ����ڵ�TTA�ڵ���
				serial+=10;
			}
		}
	}
	IE=0;
	for(i=0;i<m_arrBodyLeg.GetSize();i++)
	{
		if(m_arrBodyLeg[i].ciType==0)
			IE=i*100+100;
	}
	IE+=Ta.Module.GetNodeNum();
	//1.3���ݽڵ�������ϵ��������TTA�ڵ�˳��
	ARRAY_LIST<TTA_NODEPTR>arrTtaNodePtr;
	arrTtaNodePtr.SetSize(0,m_arrTtaNode.GetSize());
	for(i=0;i<m_arrTtaNode.GetSize();i++)
		AppendTtaNodePtr(arrTtaNodePtr,&m_arrTtaNode[i]);
	CBubbleSort<TTA_NODEPTR>::BubSort(arrTtaNodePtr.m_pData,arrTtaNodePtr.GetSize(),CompareTtaNode);
	//1.4���������ڵ�˳��ּ�TTA����ļ��е�����ڵ㣨��һ�ֺ͵ڶ������뷽ʽ�ڵ㣩
	for(i=0;i<arrTtaNodePtr.GetSize();i++)
	{
		if(!arrTtaNodePtr[i]->pNode->IsFatherObject())
			continue;
		if(arrTtaNodePtr[i]->posType!=0&&arrTtaNodePtr[i]->pAttachStart==NULL)
		{
			CLDSNode* pAttachNode=Ta.Node.FromHandle(arrTtaNodePtr[i]->pNode->arrRelationNode[0]);
			if(pAttachNode)
				AfxMessageBox(CXhChar50("%d�Žڵ��ʼ�������ڵ�%dδ���������TTA�����ļ���",arrTtaNodePtr[i]->pointI,pAttachNode->point_i));
			else
				AfxMessageBox(CXhChar50("%d�Žڵ��ʼ�������ڵ�0X%Xδ���������TTA�����ļ���",arrTtaNodePtr[i]->pointI,arrTtaNodePtr[i]->pNode->arrRelationNode[0]));
			return false;
		}
		if(arrTtaNodePtr[i]->posType!=0&&arrTtaNodePtr[i]->pAttachEnd==NULL)
		{
			CLDSNode* pAttachNode=Ta.Node.FromHandle(arrTtaNodePtr[i]->pNode->arrRelationNode[1]);
			if(pAttachNode)
				AfxMessageBox(CXhChar50("%d�Žڵ���ն������ڵ�%dδ���������TTA�����ļ���",arrTtaNodePtr[i]->pointI,pAttachNode->point_i));
			else
				AfxMessageBox(CXhChar50("%d�Žڵ���ն������ڵ�0X%Xδ���������TTA�����ļ���",arrTtaNodePtr[i]->pointI,arrTtaNodePtr[i]->pNode->arrRelationNode[0]));
			return false;
		}
		if(arrTtaNodePtr[i]->posType==1)	//ָ��X���������
		{
			arrTtaNodePtr[i]->pos.y=10000+arrTtaNodePtr[i]->pAttachStart->pointI;
			arrTtaNodePtr[i]->pos.z=10000+arrTtaNodePtr[i]->pAttachEnd->pointI;
		}
		else if(arrTtaNodePtr[i]->posType==2)	//ָ��Y���������
		{
			arrTtaNodePtr[i]->pos.x=10000+arrTtaNodePtr[i]->pAttachStart->pointI;
			arrTtaNodePtr[i]->pos.z=10000+arrTtaNodePtr[i]->pAttachEnd->pointI;
		}
		else if(arrTtaNodePtr[i]->posType==3)	//ָ��Z���������
		{
			arrTtaNodePtr[i]->pos.x=10000+arrTtaNodePtr[i]->pAttachStart->pointI;
			arrTtaNodePtr[i]->pos.y=10000+arrTtaNodePtr[i]->pAttachEnd->pointI;
		}
		else if(arrTtaNodePtr[i]->posType==4)
		{
			arrTtaNodePtr[i]->J1=arrTtaNodePtr[i]->pAttachStart->pointI;
			arrTtaNodePtr[i]->J2=arrTtaNodePtr[i]->pAttachEnd->pointI;
		}
		if(arrTtaNodePtr[i]->posType<4)
			m_arrTtaNodePtr1.append(arrTtaNodePtr[i]);
		else
			m_arrTtaNodePtr2.append(arrTtaNodePtr[i]);
	}
	//������������Ϣ
	CElement *pElem;
	bool bHasAngle=false,bHasTube=false;
	for(pElem=processor.result.elemList.GetFirst();pElem;pElem=processor.result.elemList.GetNext())
	{
		if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE||pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
			bHasAngle=true;
		if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
			bHasTube=true;
		if(bHasAngle&&bHasTube)
			break;
	}
	int angle_n=jgguige_N;
	int tube_n=tubeguige_N;
	int guige_n=0;
	if(bHasAngle)
		guige_n+=jgguige_N;
	if(bHasTube)
		guige_n+=tubeguige_N;
	if(guige_n>100)
		logerr.Log("�����ڹ��������100����TTA��֧��,���޸ĵ����ļ��е������Ϣ��");
	for(pElem=processor.result.elemList.GetFirst();pElem;pElem=processor.result.elemList.GetNext())
	{
		if(pElem->pOrgPole->IsAuxPole())	//����
			continue;
		if(!pElem->relaElem.bFatherElem)
			continue;	//�ǶԳƵ�Ԫ�еĸ���Ԫ
		TTA_ELEM *pTtaElem=m_arrTtaElem.append();
		pTtaElem->pStart=hashTtaNodeH[pElem->pStart->handle];
		pTtaElem->pEnd=hashTtaNodeH[pElem->pEnd->handle];
		if(pTtaElem->pStart->pointI>pTtaElem->pEnd->pointI)
		{	//����ʹʼ�˽ڵ��С���ն˽ڵ��
			pTtaNode=pTtaElem->pStart;
			pTtaElem->pStart=pTtaElem->pEnd;
			pTtaElem->pEnd=pTtaNode;
		}
		pTtaElem->N=0;
		pTtaElem->size=pElem->pOrgPole->GetSizeSpec();
		if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE||pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
		{
			for(i=0;i<jgguige_N;i++)
			{
				JG_PARA_TYPE* pParaType=&jgguige_table[i];
				if( fabs(pParaType->wing_wide-pTtaElem->size.wide)<EPS &&
					fabs(pParaType->wing_thick-pTtaElem->size.thick)<EPS&&pParaType->cType==pTtaElem->size.cSubClassType)
				{
					pTtaElem->N=i+1;
					break;
				}
			}
		}
		else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
		{
			for(i=0;i<tubeguige_N;i++)
			{
				if( fabs(tubeguige_table[i].D-pElem->pOrgPole->size_wide)<EPS &&
					fabs(tubeguige_table[i].thick-pElem->pOrgPole->size_thick)<EPS)
				{
					pTtaElem->N=i+1;
					if(bHasAngle)
						pTtaElem->N+=jgguige_N;
					break;
				}
			}
		}
		if(pTtaElem->N==0)
		{
			CXhChar16 sizestr;
			sizestr.Printf("%3.0fx%2.0f",pTtaElem->size.wide,pTtaElem->size.thick);
			if(pTtaElem->size.idClassType==CLS_LINEANGLE||pTtaElem->size.idClassType==CLS_GROUPLINEANGLE)
			{
				sizestr.InsertBefore(" L",0);
				if(pTtaElem->size.cSubClassType!='L')
					sizestr[0]=pTtaElem->size.cSubClassType;
			}
			else if(pTtaElem->size.idClassType==CLS_LINETUBE)
				sizestr.InsertBefore("��",0);
			AfxMessageBox(CXhChar50("�и˼����{%s}���ڹ��������ʵ��",(char*)sizestr));
			return false;
		}
		//���ʼ�ͳ����Ϣ
		STEELMAT *pSteelMat=QuerySteelMatByBriefMark(pElem->pOrgPole->cMaterial);
		if(stricmp(pSteelMat->mark,"Q345")==0)
			pTtaElem->N+=9100;
		else if(stricmp(pSteelMat->mark,"Q390")==0)
			pTtaElem->N+=9200;
		else if(stricmp(pSteelMat->mark,"Q420")==0)
			pTtaElem->N+=9300;
		else if(stricmp(pSteelMat->mark,"Q460")==0)
			pTtaElem->N+=9400;
		else //if(pElem->pOrgPole->cMaterial=='S')
			pTtaElem->N+=9000;
		if(pElem->relaElem.idElemMirX>0&&pElem->relaElem.idElemMirY>0&&pElem->relaElem.idElemMirZ>0)
			pTtaElem->IS=4;
		else if(pElem->relaElem.idElemMirX>0)
			pTtaElem->IS=2;
		else if(pElem->relaElem.idElemMirY>0)
			pTtaElem->IS=1;
		else if(pElem->relaElem.idElemMirZ>0)
			pTtaElem->IS=3;
		else
			pTtaElem->IS=0;
		if(pElem->pOrgPole->layer(1)=='Z')
		{
			if(pElem->start_force_type==CENTRIC_FORCE&&pElem->end_force_type==CENTRIC_FORCE)
				pTtaElem->TYPE=1;
			else
				pTtaElem->TYPE=-1;
		}
		else if(!pElem->pOrgPole->IsAuxPole())	//б��
		{
			if(pElem->pOrgPole->m_bLegVRod)//layer(0)=='L')
				pTtaElem->TYPE=2;	//�Ȳ�б��
			else //if(pElem->pOrgPole->CalLenCoef.hRefPole>0x20)
				pTtaElem->TYPE=3;	//����б��
			//else
			//	pTtaElem->TYPE=6;	//��б�Ĳ�
		}
		//�˶�����Լ��״��
		pTtaElem->M=0;
		if(pElem->start_force_type==ECCENTRIC_FORCE)
			pTtaElem->M+=10;
		if(pElem->end_force_type==ECCENTRIC_FORCE)
			pTtaElem->M+=10;
		if(pElem->start_joint_type==JOINT_RIGID)
			pTtaElem->M+=1;
		if(pElem->end_joint_type==JOINT_RIGID)
			pTtaElem->M+=1;
		pElem->CalLength();
		CPreProcessor::CalElemCompStableInfo(pElem,IMemberDesignCode::CODE_DLT_2012());
		pTtaElem->L1=ftoi(pElem->lamdaRyo.L/pElem->Length()*100);
		pTtaElem->L2=ftoi(pElem->lamdaRx.L/pElem->Length()*100);
		if(pTtaElem->L1==100)
			pTtaElem->L1=1;
		if(pTtaElem->L2==100)
			pTtaElem->L2=1;
		if(pElem->pOrgPole->CalLenCoef.hRefPole>0x20)
		{
			CLDSLinePart *pLinePart=(CLDSLinePart*)pElem->pOrgPole->GetMirPart(MIRMSG(1));
			if(pLinePart&&pLinePart->handle==pElem->pOrgPole->CalLenCoef.hRefPole)
			{
				if(pElem->lamdaRyo.L>0)		//
				{
					if( (pElem->lamdaRyo.pStart==pElem->pStart&&pElem->lamdaRyo.pEnd->m_cPosCalType==4)||
						(pElem->lamdaRyo.pEnd==pElem->pEnd&&pElem->lamdaRyo.pStart->m_cPosCalType==4))
						pTtaElem->L1=-6;	//����б��һ��һѹʱ��С��˵���ǽ���б���޸���
					else
						pTtaElem->L1=-16;
				}
				else if(pElem->lamdaRx.L>0)	//����б��һ��һѹʱƽ����˵���ǽ���б���и���
					pTtaElem->L2=-26;
				pTtaElem->SMC=2;		//���ҶԳ�ͬʱ��ѹ
			}
			else
			{
				pLinePart=(CLDSLinePart*)pElem->pOrgPole->GetMirPart(MIRMSG(2));
				if(pLinePart&&pLinePart->handle==pElem->pOrgPole->CalLenCoef.hRefPole)
				{
					if(pElem->lamdaRyo.L>0)		//����б��һ��һѹʱ��С��˵���ǽ���б���޸���
					{
						if( (pElem->lamdaRyo.pStart==pElem->pStart&&pElem->lamdaRyo.pEnd->m_cPosCalType==4)||
							(pElem->lamdaRyo.pEnd==pElem->pEnd&&pElem->lamdaRyo.pStart->m_cPosCalType==4))
							pTtaElem->L1=-5;	//����б��һ��һѹʱ��С��˵���ǽ���б���޸���
						else
							pTtaElem->L1=-15;
					}
					else if(pElem->lamdaRx.L>0)	//����б��һ��һѹʱƽ����˵���ǽ���б���и���
						pTtaElem->L2=-25;
					pTtaElem->SMC=1;	//ǰ��Գ�ͬʱ��ѹ
				}
			}
		}
	}
	//�������������Ϣ����
	N10=m_arrTtaNodePtr1.GetSize();
	N20=m_arrTtaNodePtr2.GetSize();
	M10=m_arrTtaElem.GetSize();
	K2=NL=Ta.WorkStatus.GetNodeNum();
	NC=0;	//ͳ�ƺ��ؽڵ���
	for(i=0;i<m_arrTtaNode.GetSize();i++)
	{
		bool bLoadNode=false;
		CExternalNodeLoad *pNodeLoad=NULL;
		for(CWorkStatus* pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
		{
			pNodeLoad=pStatus->hashNodeLoad.GetValue(m_arrTtaNode[i].pNode->handle);
			if(pNodeLoad!=NULL)
			{
				bLoadNode=true;
				break;
			}
		}
		if(!bLoadNode)
			continue;	//�Ǻ��ؽڵ�
		NC++;
	}
	K4=NLEG=Ta.Module.GetNodeNum();
	for(CWindSegment *pSeg=Ta.WindLoadSeg.GetFirst();pSeg;pSeg=Ta.WindLoadSeg.GetNext())
	{
		TTA_WINDSEG *pTtaWindSeg;
		DWORD bodyflag=0xffffffff;
		int NLG=1,count=pSeg->StatValidNodeCount(&Ta);
		for(i=0;i<count;i++)
			bodyflag&=pSeg->nodePtrArr[i]->cfgword.ToCfgLeg().BodyFlag();
		for(i=0;i<m_arrBodyLeg.GetSize();i++)
		{
			if(bodyflag==m_arrBodyLeg[i].dwBodyFlag)
				NLG=i+1;
		}
		if(pSeg->cType==0)	//��׼�����
		{
			TTA_NODE *pTtaKeyNode1=NULL;
			TTA_NODE *pTtaKeyNode2=NULL;
			if(hashTtaNodeH.GetValue(pSeg->nodeArr[0]))
				pTtaKeyNode1=*hashTtaNodeH.GetValue(pSeg->nodeArr[0]);
			if(hashTtaNodeH.GetValue(pSeg->nodeArr[1]))
				pTtaKeyNode2=*hashTtaNodeH.GetValue(pSeg->nodeArr[1]);
			if(pTtaKeyNode1&&pTtaKeyNode2)
			{
				pTtaWindSeg=m_arrWindSeg.append();
				pTtaWindSeg->TYPE=1;
				pTtaWindSeg->Fa=ftoi(pSeg->CqiFront*100);
				pTtaWindSeg->Fb=ftoi(pSeg->CqiSide*100);
				pTtaWindSeg->Fw=110;
				pTtaWindSeg->NLG=NLG;
				pTtaWindSeg->arrPointI[0]=pTtaKeyNode1->pointI;
				pTtaWindSeg->arrPointI[1]=pTtaKeyNode2->pointI;
				pTtaWindSeg->BetaZ=ftoi(pSeg->BetaZ*100);
			}
		}
		else if(pSeg->cType==1)
		{
			int nodeN=pSeg->StatValidNodeCount(&Ta);
			pTtaWindSeg=NULL;
			if(nodeN==4)
			{
				pTtaWindSeg=m_arrWindSeg.append();
				pTtaWindSeg->TYPE=2;
			}
			else if(nodeN==3)
			{
				pTtaWindSeg=m_arrWindSeg.append();
				pTtaWindSeg->TYPE=3;
			}
			if(pTtaWindSeg)
			{
				pTtaWindSeg->TYPE+=pSeg->cMirType*100;
				pTtaWindSeg->TYPE+=pSeg->cCalStyle*1000;
				pTtaWindSeg->Fa=ftoi(pSeg->CqiFront*100);
				pTtaWindSeg->Fb=ftoi(pSeg->CqiSide*100);
				pTtaWindSeg->Fw=110;
				pTtaWindSeg->NLG=NLG;
				pTtaWindSeg->BetaZ=ftoi(pSeg->BetaZ*100);
				for(i=0;i<nodeN;i++)
				{
					pTtaNode=*hashTtaNodeH.GetValue(pSeg->nodeArr[i]);
					pTtaWindSeg->arrPointI[i]=pTtaNode->pointI;
				}
			}
		}
	}
	return true;
}

TTA_NODE* CTtaDataFile::AddOrUpdateMirTtaNode(TTA_NODE* pSrcTtaNode, int mir_i,bool anywayReturn/*=true*/)
{
	//TTA_NODE destNode(pSrcTtaNode);	//�Ƶ�pSrcTtaNode->UpdatePosition(this)֮�󣬷���ʱ����ʱdestNode���겻��
	bool bParentNode=(hashParentTtaNodeI.GetValue(pSrcTtaNode->pointI)!=NULL);
	if (!pSrcTtaNode->IsPosUpdated(0x07))
		pSrcTtaNode->UpdatePosition(this);
	TTA_NODE destNode(pSrcTtaNode);		//��ʱ����destNode���ܱ�֤��Գ�Դ�ڵ������һ�� wjh-2019.12.26

	if(mir_i==1)	//Y��Գ�
		destNode.pos.x*=-1.0;
	else if(mir_i==2)	//X��Գ�
		destNode.pos.y*=-1.0;
	else if(mir_i==3)
	{	//Z��Գ�
		destNode.pos.x*=-1.0;
		destNode.pos.y*=-1.0;
	}
	//���ڲ��ֽڵ����굱ǰ���ܻ�δ���£��ʲ��ܰ�����λ�ý��������ԶԳ����
	if(mir_i==2)	//ע��˴�TTA��XY��Գ�������LDS�෴
	{	//X��Գ�
		if(bParentNode&&pSrcTtaNode->IS!=2&&pSrcTtaNode->IS!=4&&!pSrcTtaNode->IsPosUpdated(0x02))
			pSrcTtaNode->UpdatePosition(this);
		if((pSrcTtaNode->IsPosUpdated(0x02)&&fabs(pSrcTtaNode->pos.y)<EPS)&&
			pSrcTtaNode->IS!=2&&pSrcTtaNode->IS!=4)
				destNode.pointI=pSrcTtaNode->pointI;
		else if((pSrcTtaNode->IsPosUpdated(0x01)&&fabs(pSrcTtaNode->pos.x)<EPS)&&pSrcTtaNode->IS==3)
			destNode.pointI=MirPointI(pSrcTtaNode->pointI,MIRMSG(3));
		else if(!anywayReturn&&pSrcTtaNode->IS!=2&&pSrcTtaNode->IS!=4)
		{
			int pointI=MirPointI(pSrcTtaNode->pointI,MIRMSG(1));
			TTA_NODE* pMirNode=hashTtaNodeI.GetValue(pointI);
			if( pMirNode!=NULL&&pMirNode->IsPosUpdated()&&
				fabs(pMirNode->pos.x-pSrcTtaNode->pos.x)+fabs(pMirNode->pos.y+pSrcTtaNode->pos.y)<0.001)
			{
				destNode.pointI=pointI;	//����������ڰ�930�ĶȶԳƷ�Ϊ930+931Y��Գ���ʽ����
				return pMirNode;
			}
			else
				return NULL;
		}
		else
			destNode.pointI=MirPointI(pSrcTtaNode->pointI,MIRMSG(1));
		if(!IsEnableMirX(pSrcTtaNode->pointI))
		{
			TTA_NODE* pMirNode=hashTtaNodeI.GetValue(destNode.pointI);
			if( pMirNode!=NULL&&pMirNode->IsPosUpdated()&&
				fabs(pMirNode->pos.x-pSrcTtaNode->pos.x)+fabs(pMirNode->pos.y+pSrcTtaNode->pos.y)<0.001)
				return pMirNode;
			return anywayReturn?pSrcTtaNode:NULL;
		}
	}
	else if(mir_i==1)
	{	//Y��Գ�
		if(bParentNode&&pSrcTtaNode->IS!=1&&pSrcTtaNode->IS!=4&&!pSrcTtaNode->IsPosUpdated(0x01))
			pSrcTtaNode->UpdatePosition(this);
		if((pSrcTtaNode->IsPosUpdated(0x01)&&fabs(pSrcTtaNode->pos.x)<EPS)&&
			pSrcTtaNode->IS!=1&&pSrcTtaNode->IS!=4)
				destNode.pointI=pSrcTtaNode->pointI;
		else if((pSrcTtaNode->IsPosUpdated(0x02)&&fabs(pSrcTtaNode->pos.y)<EPS)&&pSrcTtaNode->IS==3)
			destNode.pointI=MirPointI(pSrcTtaNode->pointI,MIRMSG(3));
		else if(!anywayReturn&&pSrcTtaNode->IS!=1&&pSrcTtaNode->IS!=4)
		{
			int pointI=MirPointI(pSrcTtaNode->pointI,MIRMSG(2));
			TTA_NODE* pMirNode=hashTtaNodeI.GetValue(pointI);
			if( pMirNode!=NULL&&pMirNode->IsPosUpdated()&&
				fabs(pMirNode->pos.x+pSrcTtaNode->pos.x)+fabs(pMirNode->pos.y-pSrcTtaNode->pos.y)<0.001)
			{
				destNode.pointI=pointI;
				return pMirNode;
			}
			else
				return NULL;
		}
		else
			destNode.pointI=MirPointI(pSrcTtaNode->pointI,MIRMSG(2));
		if(!IsEnableMirY(pSrcTtaNode->pointI))
		{
			TTA_NODE* pMirNode=hashTtaNodeI.GetValue(destNode.pointI);
			if( pMirNode!=NULL&&pMirNode->IsPosUpdated()&&
				fabs(pMirNode->pos.x+pSrcTtaNode->pos.x)+fabs(pMirNode->pos.y-pSrcTtaNode->pos.y)<0.001)
				return pMirNode;
			return anywayReturn?pSrcTtaNode:NULL;
		}
	}
	else if(mir_i==3)
	{	//Z��Գ�
		if(bParentNode&&pSrcTtaNode->IS!=3&&pSrcTtaNode->IS!=4&&!pSrcTtaNode->IsPosUpdated(0x03))
			pSrcTtaNode->UpdatePosition(this);
		if( pSrcTtaNode->IsPosUpdated(0x03)&&(fabs(pSrcTtaNode->pos.x)+fabs(pSrcTtaNode->pos.y)<EPS)&&
			pSrcTtaNode->IS!=3&&pSrcTtaNode->IS!=4)
			destNode.pointI=pSrcTtaNode->pointI;
		else if((pSrcTtaNode->IsPosUpdated(0x03)&&fabs(pSrcTtaNode->pos.x)>EPS&&fabs(pSrcTtaNode->pos.y)>EPS)||
			pSrcTtaNode->IS==3||pSrcTtaNode->IS==4)
			destNode.pointI=MirPointI(pSrcTtaNode->pointI,MIRMSG(3));
		else if(pSrcTtaNode->IsPosUpdated(0x03)&&fabs(pSrcTtaNode->pos.x)<EPS&&pSrcTtaNode->IS==2)
			destNode.pointI=MirPointI(pSrcTtaNode->pointI,MIRMSG(1));
		else if(pSrcTtaNode->IsPosUpdated(0x03)&&fabs(pSrcTtaNode->pos.y)<EPS&&pSrcTtaNode->IS==1)
			destNode.pointI=MirPointI(pSrcTtaNode->pointI,MIRMSG(2));
		else if(!anywayReturn&&pSrcTtaNode->IS!=3&&pSrcTtaNode->IS!=4)
		{
			int pointI=MirPointI(pSrcTtaNode->pointI,MIRMSG(3));
			TTA_NODE* pMirNode=hashTtaNodeI.GetValue(pointI);
			if( pMirNode!=NULL&&pMirNode->IsPosUpdated()&&
				fabs(pMirNode->pos.x+pSrcTtaNode->pos.x)+fabs(pMirNode->pos.y+pSrcTtaNode->pos.y)<0.001)
			{
				destNode.pointI=pointI;
				return pMirNode;
			}
			else
				return NULL;
		}
		else
		{
			if(!pSrcTtaNode->IsPosUpdated())
				pSrcTtaNode->UpdatePosition(this);
			if( pSrcTtaNode->IsPosUpdated())
			{
				if(fabs(pSrcTtaNode->pos.x)<EPS)
					destNode.pointI=MirPointI(pSrcTtaNode->pointI,MIRMSG(1));
				else if(fabs(pSrcTtaNode->pos.y)<EPS)
					destNode.pointI=MirPointI(pSrcTtaNode->pointI,MIRMSG(2));
				else
					destNode.pointI=pSrcTtaNode->pointI;
			}
		}
		if(!IsEnableMirZ(pSrcTtaNode->pointI))
		{
			TTA_NODE* pMirNode=hashTtaNodeI.GetValue(destNode.pointI);
			if( pMirNode!=NULL&&pMirNode->IsPosUpdated()&&
				fabs(pMirNode->pos.x+pSrcTtaNode->pos.x)+fabs(pMirNode->pos.y+pSrcTtaNode->pos.y)<0.001)
				return pMirNode;
			return anywayReturn?pSrcTtaNode:NULL;
		}
	}
	if(destNode.pointI==pSrcTtaNode->pointI)
		return pSrcTtaNode;	//���п���������
	TTA_NODE*  pMirTtaNode=NULL;
	if((pMirTtaNode=hashTtaNodeI.GetValue(destNode.pointI))==NULL)
	{
		destNode.J1=destNode.J2=destNode.J3=destNode.J4=0;
		pMirTtaNode=listTtaNodes.append(destNode);
		pMirTtaNode->pointI=destNode.pointI;
		hashTtaNodeI.SetValue(destNode.pointI,pMirTtaNode);
		if(pMirTtaNode->bBraceNode)
			this->hashOnlyBraceNodes.SetValue(destNode.pointI,pMirTtaNode);
	}
	else
		pMirTtaNode->pos=destNode.pos;
	if (pSrcTtaNode->IsPosUpdated(0x07))	//���Դ�ڵ��ѳɹ����¹����꣬Ҳ�൱�ڶԳƽڵ�������Ѹ��¹� wjh-2019.12.26
		pMirTtaNode->SetPosUpdateState(pSrcTtaNode->cbPosUpdated);
	if(hashParentTtaNodeI.GetValue(pMirTtaNode->pointI)!=NULL)
		return pMirTtaNode;	//ԭʼ���ڵ�
	if(pMirTtaNode->J1==0&&pMirTtaNode->posType>0)
	{	//���posType>0�ж�������Ϊ�˱����������κ������ڵ㣬���TTA_NODE::pNode���� wjh-2019.10.18
		int J3=pMirTtaNode->J3;
		CLDSNode* pOldRelaNode=pMirTtaNode->pNode;
		pMirTtaNode->CopyFrom(&destNode);
		pMirTtaNode->pNode=pOldRelaNode;
		pMirTtaNode->pAttachStart=pMirTtaNode->pAttachEnd=NULL;
		pMirTtaNode->J3=J3;
		if(pSrcTtaNode->pAttachStart==NULL)
			pSrcTtaNode->pAttachStart=hashTtaNodeI.GetValue(pSrcTtaNode->J1);
		if(pSrcTtaNode->pAttachStart)
			pMirTtaNode->pAttachStart=AddOrUpdateMirTtaNode(pSrcTtaNode->pAttachStart,mir_i,false);
		if(pSrcTtaNode->pAttachEnd==NULL)
			pSrcTtaNode->pAttachEnd=hashTtaNodeI.GetValue(pSrcTtaNode->J2);
		if(pSrcTtaNode->pAttachEnd)
			pMirTtaNode->pAttachEnd=AddOrUpdateMirTtaNode(pSrcTtaNode->pAttachEnd,mir_i,false);
		pMirTtaNode->J1=pMirTtaNode->pAttachStart!=NULL?pMirTtaNode->pAttachStart->pointI:0;
		pMirTtaNode->J2=pMirTtaNode->pAttachEnd  !=NULL?pMirTtaNode->pAttachEnd  ->pointI:0;
		if(pMirTtaNode->posType>0&&pMirTtaNode->posType<=6&&(pMirTtaNode->J1==0||pMirTtaNode->J2==0))
		{
			pMirTtaNode->J1=pSrcTtaNode->pointI;
			pMirTtaNode->pAttachStart=pSrcTtaNode;
			pMirTtaNode->posType=12;
		}
		if(pMirTtaNode->posType>=8&&!pMirTtaNode->blInitPosCalParams)
		{
			if (pMirTtaNode->pMirFromNode!=NULL)
			{
				if(pMirTtaNode->posType==12)
				{
					pMirTtaNode->J1=pMirTtaNode->pMirFromNode->pointI;
					pMirTtaNode->pAttachStart=pMirTtaNode->pMirFromNode;
				}
				pMirTtaNode->cMirTransType=pMirTtaNode->ciMirFromType;
			}
			else
			{
				if (mir_i==1)	//Y��Գ�
					pMirTtaNode->cMirTransType='Y';
				else if (mir_i==2)	//X��Գ�
					pMirTtaNode->cMirTransType='X';
				else if (mir_i==3)	//Z��Գ�
					pMirTtaNode->cMirTransType='Z';
				else
					pMirTtaNode->cMirTransType=0;
			}
			pMirTtaNode->blInitPosCalParams=true;
		}
	}
	if(destNode.posType==5&&pMirTtaNode->J3==0)
	{	//�����
		TTA_NODE* pCrossStart=hashTtaNodeI.GetValue(pSrcTtaNode->J3);
		TTA_NODE* pCrossEnd  =hashTtaNodeI.GetValue(pSrcTtaNode->J4);
		TTA_NODE* pMirCrossStart=NULL,*pMirCrossEnd=NULL;
		if(pCrossStart)
			pMirCrossStart=AddOrUpdateMirTtaNode(pCrossStart,mir_i);
		if(pCrossEnd)
			pMirCrossEnd  =AddOrUpdateMirTtaNode(pCrossEnd,mir_i);
		
		pMirTtaNode->J3=pMirCrossStart!=NULL?pMirCrossStart->pointI:0;
		pMirTtaNode->J4=pMirCrossEnd  !=NULL?pMirCrossEnd  ->pointI:0;
	}
	else if(destNode.posType==7)
	{
		if(pSrcTtaNode->J3>0&&pSrcTtaNode->J4>0)
		{
			TTA_NODE* pDistStart=hashTtaNodeI.GetValue(pSrcTtaNode->J3);
			TTA_NODE* pDistEnd  =hashTtaNodeI.GetValue(pSrcTtaNode->J4);
			TTA_NODE* pMirDistStart=NULL,*pMirDistEnd=NULL;
			if(pDistStart)
				pMirDistStart=AddOrUpdateMirTtaNode(pDistStart,mir_i);
			if(pDistEnd)
				pMirDistEnd  =AddOrUpdateMirTtaNode(pDistEnd,mir_i);
		
			if(pMirDistStart&&pMirDistEnd)
			{
				pMirTtaNode->J3=pMirDistStart->pointI;
				pMirTtaNode->J4=pMirDistEnd  ->pointI;
			}
			else
			{
				pMirTtaNode->J3=pDistStart->pointI;
				pMirTtaNode->J4=pDistEnd  ->pointI;
			}
		}
		else if(pSrcTtaNode->J3>0&&pSrcTtaNode->J4==0&&pSrcTtaNode->J5==0)
		{
			if(mir_i==1||mir_i==3)	//Y��Գ�
				pMirTtaNode->offsetDist=-pSrcTtaNode->offsetDist;
			else//
				pMirTtaNode->offsetDist= pSrcTtaNode->offsetDist;
		}
		else if(pSrcTtaNode->J3==0&&pSrcTtaNode->J4>0&&pSrcTtaNode->J5==0)
		{
			if(mir_i==2||mir_i==3)	//X��Գ�
				pMirTtaNode->offsetDist=-pSrcTtaNode->offsetDist;
			else//
				pMirTtaNode->offsetDist= pSrcTtaNode->offsetDist;
		}
		else if(pSrcTtaNode->J3==0&&pSrcTtaNode->J4==0&&pSrcTtaNode->J5>0)
			pMirTtaNode->offsetDist= pSrcTtaNode->offsetDist;
	}
	else if(destNode.posType>=8&&destNode.posType<=10)
	{
		if( (pSrcTtaNode->posType==8&&pSrcTtaNode->ciPosViceType=='I')||
			(pSrcTtaNode->posType==9&&mir_i!=2)||(pSrcTtaNode->posType==10&&mir_i!=1))
		{
			pMirTtaNode->posType=12;
			pMirTtaNode->pAttachStart=pSrcTtaNode;
			pMirTtaNode->J1=pSrcTtaNode->pointI;
			if(mir_i==1)	//Y��Գ�
				pMirTtaNode->cMirTransType='Y';
			else if(mir_i==2)	//X��Գ�
				pMirTtaNode->cMirTransType='X';
			else if(mir_i==3)	//Z��Գ�
				pMirTtaNode->cMirTransType='Z';
		}
		pMirTtaNode->J3=pSrcTtaNode->J3;
	}
	else if(destNode.posType==6)
	{
		int J3=pSrcTtaNode->J3%10000;
		TTA_NODE* pRefNode,*pMirRefNode=NULL;
		pRefNode=hashTtaNodeI.GetValue(J3);
		if(pRefNode)
			pMirRefNode=AddOrUpdateMirTtaNode(pRefNode,mir_i);
		if(pMirRefNode)
			pMirTtaNode->J3=pMirRefNode->pointI;
	}
	else if (destNode.posType == 13)
	{
		int xarrJI[3];
		xarrJI[0] = pSrcTtaNode->J1;
		xarrJI[1] = pSrcTtaNode->J2;
		xarrJI[2] = pSrcTtaNode->J3;
		for (int j = 0; j < 3; j++)
		{
			TTA_NODE* pRefNode, *pMirRefNode = NULL;
			pRefNode = hashTtaNodeI.GetValue(xarrJI[j]);
			if (pRefNode)
				pMirRefNode = AddOrUpdateMirTtaNode(pRefNode, mir_i);
			if (pMirRefNode&&j == 0)
				pMirTtaNode->J1 = pMirRefNode->pointI;
			else if (pMirRefNode&&j == 1)
				pMirTtaNode->J2 = pMirRefNode->pointI;
			else if (pMirRefNode&&j == 2)
				pMirTtaNode->J3 = pMirRefNode->pointI;
		}
	}
	return pMirTtaNode;
}

struct ANGLESIZE_RECORD {
	long liIndexInTable;	//��ǰ����ڽǸֹ����е���ţ�0Ϊ��ʼ������
	//ͬһͳ�ĺ�������һ�˼���'D'����ϽǸֱ�Ϊ'T'����ϽǸ֣�ͬͳ�ĺŸ˼�ȫ����'D'����ϽǸֱ�Ϊ'T'����ϽǸ�
	long liSizeStatNo;		//ԭʼ�������ļ��ж�Ӧ��ͳ�ĺ�
	ANGLESIZE_RECORD(long indexInSizeTbl = 0, long statSizeMatNo = -1) {
		liIndexInTable = indexInSizeTbl;
		liSizeStatNo = statSizeMatNo;
	}
};
void CTtaDataFile::BuildLdsGeoModel(	//����TTA������Ϣ����LDS����ģ��
	CHashSet<TTA_NODEPTR>* pHashOnlyBraceNodes/*=NULL*/,CMapList<TTA_BRACEELEM*>* pMapOnlyBraceElems/*=NULL*/,
	bool updateMirRelaInfo/*=true*/)
{
	CXhChar50 title="����LDS��ģ�͸˼���ڵ����";
	if(pHashOnlyBraceNodes)
		title="����LDS���ĸ˼���˽ڵ����";
	DisplayProcess(0,title);
	//��ӽڵ�
	TTA_NODE *pTtaNode;
	CLDSNode *pNode,*pRelaNode;
	CHashList<CLDSNodePtr> hashLegacyIntersNodes;
	int i=0,n=listTtaNodes.GetNodeNum();
	if(pHashOnlyBraceNodes)
		n=pHashOnlyBraceNodes->GetNodeNum();
	for(pTtaNode=listTtaNodes.GetFirst();pTtaNode;pTtaNode=listTtaNodes.GetNext())
	{
		if(pHashOnlyBraceNodes&&pHashOnlyBraceNodes->GetValue(pTtaNode->pointI)==NULL)
			continue;	//���ؽ����ļ��˽ڵ�ʱ���˵�֮ǰ¼��ڵ�
		i++;
		DisplayProcess(i*30/n,title);
		if((pNode=pTtaNode->pNode)!=NULL)
			pTtaNode->cfgword=pNode->cfgword;
		f3dPoint position=pTtaNode->pos*1000;
		if(pHashOnlyBraceNodes&&pNode==NULL)
		{	//��ֹ���������Ķ˵��еĽ������֮ǰϵͳ�Զ���Ľ�����ص�
			for(CLDSNode *pIntersNode=listIntersNodes.GetFirst();pIntersNode;pIntersNode=listIntersNodes.GetNext())
			{
				if(!pTtaNode->cfgword.And(pIntersNode->cfgword))
					continue;	//������
				GEPOINT distvec=position-pIntersNode->Position(false);
				if(fabs(distvec.x)+fabs(distvec.y)+fabs(distvec.z)>1)
					continue;
				pNode=pIntersNode;

				if(pIntersNode->point_i==0)
					pIntersNode->point_i=pTtaNode->pointI;
				break;
			}
		}
		if(pNode==NULL)
		{
			pNode=console.AppendNode();
			pNode->point_i=pTtaNode->pointI;
			pNode->SetPosition(position);	//100����m->mm
		}
		else
			hashLegacyIntersNodes.SetValue(pNode->handle,pNode);
		pTtaNode->pNode=pNode;
		hashLdsNodeI.SetValue(pTtaNode->pointI,pNode);
	}
	//��ӻ�׼��
	double epsilon=1;
	CLDSLine* pDatunLine=NULL;
	for(TANS_LINE* pTansLine=hashLines.GetFirst();pTansLine;pTansLine=hashLines.GetNext())
	{
		if(pTansLine->pRelaDatumLine)
			continue;
		CLDSNode* pNodeS=hashLdsNodeI[pTansLine->iStartPointI];
		CLDSNode* pNodeE=hashLdsNodeI[pTansLine->iEndPointI];
		if(pNodeS==NULL||pNodeE==NULL)
			continue;
		for(pDatunLine=m_pContextModel->EnumLineFirst();pDatunLine;pDatunLine=m_pContextModel->EnumLineNext())
		{
			if((pDatunLine->hPointArr[0]==pNodeS->handle&&pDatunLine->hPointArr[1]==pNodeE->handle)||
				(pDatunLine->hPointArr[0]==pNodeE->handle&&pDatunLine->hPointArr[1]==pNodeS->handle))
				break;
		}
		if(pDatunLine==NULL)
		{
			pDatunLine=m_pContextModel->AppendLine();
			pDatunLine->hPointArr[0]=HANDLEPOINT(pNodeS->handle,1);
			pDatunLine->hPointArr[1]=HANDLEPOINT(pNodeE->handle,1);
			pDatunLine->start_pos=pNodeS->Position();
			pDatunLine->end_pos=pNodeE->Position();
		}
		pTansLine->pRelaDatumLine=pDatunLine;
	}
	//��ӻ�׼��
	CLDSPlane* pDatunPlane=NULL;
	for(TANS_PLANE* pTansPlane=hashPlanes.GetFirst();pTansPlane;pTansPlane=hashPlanes.GetNext())
	{
		if(pTansPlane->pRelaDatumPlane)
			continue;
		CLDSNode* pNode1=hashLdsNodeI[pTansPlane->iPoint1stI];
		CLDSNode* pNode2=hashLdsNodeI[pTansPlane->iPoint2ndI];
		CLDSNode* pNode3=hashLdsNodeI[pTansPlane->iPoint3rdI];
		if(pNode1==NULL||pNode2==NULL||pNode3==NULL)
			continue;
		for(pDatunPlane=m_pContextModel->EnumPlaneFirst();pDatunPlane;pDatunPlane=m_pContextModel->EnumPlaneNext())
		{
			if(pDatunPlane->IsSameWith(pTansPlane->xPlaneOrg,pTansPlane->vPlaneNormal))
				break;
		}
		if(pDatunPlane==NULL)
		{
			pDatunPlane=m_pContextModel->AppendPlane();
			pDatunPlane->hPointArr[0]=HANDLEPOINT(pNode1->handle,1);
			pDatunPlane->hPointArr[1]=HANDLEPOINT(pNode2->handle,1);
			pDatunPlane->hPointArr[2]=HANDLEPOINT(pNode3->handle,1);
			pDatunPlane->UpdatePlane(&Ta);
		}
		pTansPlane->pRelaDatumPlane=pDatunPlane;
	}
	//���½ڵ�������
	for(pTtaNode=listTtaNodes.GetFirst(),i=0;pTtaNode;pTtaNode=listTtaNodes.GetNext())
	{
		if(pHashOnlyBraceNodes&&pHashOnlyBraceNodes->GetValue(pTtaNode->pointI)==NULL)
			continue;	//���ؽ����ļ��˽ڵ�ʱ���˵�֮ǰ¼��ڵ�
		i++;
		DisplayProcess(30+i*5/n,title);
		pNode=hashLdsNodeI[pTtaNode->pointI];
		pNode->layer(0)=pTtaNode->cPosLayer;
		if(pTtaNode->cPosLayer=='L')
			pNode->layer(2)='0'+CalPointQuadPos(pNode->Position(false));
		//pNode->layer(2)=CalPointQuad(
		pNode->cfgword=pTtaNode->cfgword;
		for(CLDSModule *pModule=m_pContextModel->Module.GetFirst();pModule;pModule=m_pContextModel->Module.GetNext())
		{
			if(pTtaNode->bFoundationNode&&pModule->IsSonPart(pNode,NULL,false))	//�趨���������ڵ�Լ���ڵ�λ��Լ��
				pNode->SetRestrictWord(pModule->handle,X_RESTRICT|Y_RESTRICT|Z_RESTRICT);
		}
		if(pTtaNode->posType>0&&hashLegacyIntersNodes.GetValue(pNode->handle)==NULL)
		{	//ָ������������ڵ�
			if(pTtaNode->posType<=3)
			{
				pNode->m_cPosCalType=(char)pTtaNode->posType;
				pRelaNode=hashLdsNodeI[pTtaNode->pAttachStart->pointI];
				if(pRelaNode)
					pNode->arrRelationNode[0]=pRelaNode->handle;
				pRelaNode=hashLdsNodeI[pTtaNode->pAttachEnd->pointI];
				if(pRelaNode)
					pNode->arrRelationNode[1]=pRelaNode->handle;
			}
			else if(pTtaNode->posType==4)
			{	//�����ȷֵ�
				pNode->m_cPosCalType=5;
				pNode->attach_scale=pTtaNode->coefScale;
				pRelaNode=hashLdsNodeI[pTtaNode->pAttachStart->pointI];
				if(pRelaNode)
					pNode->arrRelationNode[0]=pRelaNode->handle;
				pRelaNode=hashLdsNodeI.GetValue(pTtaNode->pAttachEnd->pointI);
				if(pRelaNode)
					pNode->arrRelationNode[1]=pRelaNode->handle;
			}
			else if(pTtaNode->posType==5)
			{	//�����
				pNode->m_cPosCalType=CLDSNode::COORD_INTERSECTION;
				pRelaNode=hashLdsNodeI[pTtaNode->J1];//pAttachStart->pointI];
				if(pRelaNode)	
					pNode->arrRelationNode[0]=pRelaNode->handle;
				pRelaNode=hashLdsNodeI[pTtaNode->J2];//pAttachEnd->pointI];
				if(pRelaNode)
					pNode->arrRelationNode[1]=pRelaNode->handle;
				pRelaNode=hashLdsNodeI[pTtaNode->J3];
				if(pRelaNode)
					pNode->arrRelationNode[2]=pRelaNode->handle;
				pRelaNode=hashLdsNodeI[pTtaNode->J4];
				if(pRelaNode)
					pNode->arrRelationNode[3]=pRelaNode->handle;
			}
			else if (pTtaNode->posType == 6)
			{	//�ظ˼���ĳ�ڵ�ȸߵĽڵ�
				pNode->m_cPosCalType = 6;
				pNode->m_cPosCalViceType = pTtaNode->ciPosViceType;
				pRelaNode = hashLdsNodeI[pTtaNode->J3];
				if (pRelaNode)
					pNode->arrRelationNode[0] = pRelaNode->handle;
				pRelaNode = hashLdsNodeI[pTtaNode->pAttachStart->pointI];
				if (pRelaNode)
					pNode->arrRelationNode[1] = pRelaNode->handle;
				pRelaNode = hashLdsNodeI[pTtaNode->pAttachEnd->pointI];
				if (pRelaNode)
					pNode->arrRelationNode[2] = pRelaNode->handle;
			}
			else if (pTtaNode->posType == 13)
			{	//����սڵ����������Ľڵ�
				pNode->m_cPosCalType = 13;
				pNode->arrRelationNode[0] = pNode->arrRelationNode[1] = pNode->arrRelationNode[2] = 0;
				if (pTtaNode->J1 > 0)
				{
					if ((pRelaNode = hashLdsNodeI[pTtaNode->J1]) != NULL)
						pNode->arrRelationNode[0] = pRelaNode->handle;
				}
				if (pTtaNode->J2 > 0)
				{
					if ((pRelaNode = hashLdsNodeI[pTtaNode->J2]) != NULL)
						pNode->arrRelationNode[1] = pRelaNode->handle;
				}
				if (pTtaNode->J3 > 0)
				{
					if ((pRelaNode = hashLdsNodeI[pTtaNode->J3]) != NULL)
						pNode->arrRelationNode[2] = pRelaNode->handle;
				}
			}
			else if(pTtaNode->posType==7)
			{	//Ԥ��ƫ�Ƶ�
				pNode->m_cPosCalType=CLDSNode::COORD_OFFSET;
				if((pRelaNode=hashLdsNodeI[pTtaNode->J1])!=NULL)
					pNode->arrRelationNode[0]=pNode->arrRelationNode[1]=pRelaNode->handle;
				if((pRelaNode=hashLdsNodeI[pTtaNode->J2])!=NULL)
					pNode->arrRelationNode[2]=pRelaNode->handle;
				if(pTtaNode->ciPosViceType!='X'&&pTtaNode->ciPosViceType!='Y'&&pTtaNode->ciPosViceType!='Z')
				{
					pNode->m_cPosCalViceType=pTtaNode->ciPosViceType;
					CLDSNode* pDistStart=hashLdsNodeI[pTtaNode->J3];
					CLDSNode* pDistEnd  =hashLdsNodeI[pTtaNode->J4];
					if(pDistStart&&pDistEnd)
					{
						pDistStart->CalPosition();
						pDistEnd->CalPosition();
						pNode->attach_offset=DISTANCE(pDistStart->xOriginalPos,pDistEnd->xOriginalPos);
					}
				}
				else
				{
					pNode->arrRelationNode[0]=pNode->arrRelationNode[2];
					pNode->m_cPosCalViceType=pTtaNode->ciPosViceType;	//'L'
					pNode->attach_offset=pTtaNode->offsetDist;
				}
					
			}
			else if(pTtaNode->posType>=8&&pTtaNode->posType<=10)
			{	//��׼����XY|YZ|XZ����ֵ�����
				TANS_PLANE* pTansPlane=hashPlanes.GetValue(pTtaNode->J3);
				if(pTansPlane && pTansPlane->pRelaDatumPlane)
				{
					pNode->m_cPosCalType=(char)pTtaNode->posType;
					pNode->m_cPosCalViceType=0;
					if(pTtaNode->posType==8&&pTtaNode->ciPosViceType=='I')
					{
						pNode->m_cPosCalViceType='I';
						if(pTtaNode->pAttachStart&&pTtaNode->pAttachStart->pNode)
							pNode->arrRelationNode[0]=pTtaNode->pAttachStart->pNode->handle;
						if(pTtaNode->pAttachEnd&&pTtaNode->pAttachEnd->pNode)
							pNode->arrRelationNode[1]=pTtaNode->pAttachEnd->pNode->handle;
					}
					else
						pNode->m_cPosCalViceType=pTtaNode->cMirTransType;
					pNode->hRelationPlane=pTansPlane->pRelaDatumPlane->handle;
				}
			}
			else if(pTtaNode->posType==12)
			{	//ԭʼ�����������Գ� 
				pNode->m_cPosCalType=12;
				if(pTtaNode->pAttachStart!=NULL&&pTtaNode->pAttachStart->pNode!=NULL)
					pNode->arrRelationNode[0]=pTtaNode->pAttachStart->pNode->handle;
				else
				{
					pNode->arrRelationNode[0]=0;
					logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"error");
				}
				pNode->m_cPosCalViceType=pTtaNode->cMirTransType;
			}
		}
	}
	//���½ڵ����꣬�粻���¿������ڽڵ�������ϵ���Ⱥ�˳���ͻ���²��ֽڵ�Ĭ��������� wjh-2015.6.25
	for(pNode=m_pContextModel->EnumNodeFirst(),i=0;pNode;pNode=m_pContextModel->EnumNodeNext())
	{
		if(pHashOnlyBraceNodes&&pHashOnlyBraceNodes->GetValue(pNode->pointI)==NULL)
			continue;	//���ؽ����ļ��˽ڵ�ʱ���˵�֮ǰ¼��ڵ�
		else if(hashLegacyIntersNodes.GetValue(pNode->handle)!=NULL)
			continue;
		pNode->Set();
		pNode->feature=0;	//�����Ա������ѡ�ڵ㸸�˼�ʱʹ��
		NewNode(pNode);
		i++;
		int STEP=pHashOnlyBraceNodes!=NULL?5:20;
		DisplayProcess(35+i*STEP/n,title);
	}
	if(pHashOnlyBraceNodes)
	{	//��ǰ���ٺϲ��ص��ĸ��Ľڵ�
		i=0;
		n=pHashOnlyBraceNodes->GetNodeNum();
		CHashList<long>hashReplaceNodes;
		for(pTtaNode=pHashOnlyBraceNodes->GetFirst();pTtaNode;pTtaNode=pHashOnlyBraceNodes->GetNext(),i++)
		{
			if(pTtaNode->pNode==NULL||pTtaNode->pointI!=pTtaNode->pNode->point_i)
				continue;	//֮ǰ�Ѻϲ����Ľڵ㣬���򲻿��ܵ�Ų�һ�� wjh-2018.7.4
			DisplayProcess(40+i*15/n,title);
			BOOL pushed=pHashOnlyBraceNodes->push_stack();
			TTA_NODE* pNextTtaNode=NULL;
			for(pNextTtaNode=pHashOnlyBraceNodes->GetNext();pNextTtaNode;pNextTtaNode=pHashOnlyBraceNodes->GetNext())
			{
				if(!(pTtaNode->pNode&&pNextTtaNode->pNode&&pTtaNode->pNode->ModuleTogetherWith(pNextTtaNode->pNode)))
					continue;	//�������ص�������������
				GEPOINT distvec=pTtaNode->pNode->xOriginalPos-pNextTtaNode->pNode->xOriginalPos;
				if(fabs(distvec.x)+fabs(distvec.y)+fabs(distvec.z)>1)
					continue;
				//����ϲ��ڵ�ľ�ʾ��Ϣ
				CXhChar16 nodestr1,nodestr2;
				nodestr1.Printf("0X%X",pTtaNode->pNode->handle);
				if(pTtaNode->pointI>0)
					nodestr1.Append(CXhChar16("{%d}",pTtaNode->pointI));
				nodestr2.Printf("0X%X",pNextTtaNode->pNode->handle);
				if(pNextTtaNode->pointI>0)
					nodestr2.Append(CXhChar16("{%d}",pNextTtaNode->pointI));
				logerr.Log("�ϲ�%s�ڵ�&%s�ڵ�-->�ڵ�%s\n",(char*)nodestr1,(char*)nodestr2,(char*)nodestr1);
				m_nOverlapBraceNodes++;
				hashLdsNodeI.SetValue(pNextTtaNode->pointI,pTtaNode->pNode);
				hashReplaceNodes.SetValue(pNextTtaNode->pointI,pTtaNode->pNode->handle);
				if(pTtaNode->pNode!=pNextTtaNode->pNode)
				{	//�û��п����ظ����彻��㣬���´�ʱ��ͬ���ȴָ��ͬһLDS�ڵ� wjh-2018.8.6
					console.DeleteNode(pNextTtaNode->pNode->handle);
					g_pSolidDraw->DelEnt(pNextTtaNode->pNode->handle);
					m_pContextModel->DeleteNode(pNextTtaNode->pNode->handle);
				}
				pNextTtaNode->pNode=pTtaNode->pNode;
			}
			pHashOnlyBraceNodes->pop_stack(pushed);
		}
		//������ϲ��ڵ���Ҫ�滻�Ľڵ��������
		for(pTtaNode=pHashOnlyBraceNodes->GetFirst();pTtaNode;pTtaNode=pHashOnlyBraceNodes->GetNext())
		{
			long *phReplNode=NULL;
			if(pTtaNode->pointI!=pTtaNode->pNode->pointI)
				continue;
			if((pTtaNode->posType>=1&&pTtaNode->posType<=5)||(pTtaNode->posType>=7&&pTtaNode->posType<=10))
			{
				if((phReplNode=hashReplaceNodes.GetValue(pTtaNode->J1))!=NULL)
					pTtaNode->pNode->arrRelationNode[0]=*phReplNode;
				if((phReplNode=hashReplaceNodes.GetValue(pTtaNode->J2))!=NULL)
					pTtaNode->pNode->arrRelationNode[1]=*phReplNode;
			}
			if(pTtaNode->posType==5)
			{	//�����
				if((phReplNode=hashReplaceNodes.GetValue(pTtaNode->J3))!=NULL)
					pTtaNode->pNode->arrRelationNode[2]=*phReplNode;
				if((phReplNode=hashReplaceNodes.GetValue(pTtaNode->J4))!=NULL)
					pTtaNode->pNode->arrRelationNode[3]=*phReplNode;
			}
		}
	}
	CLDSLinePart *pRod,*pMirRod;
	//��ʼ���Ǹֹ����е�'D'����ϽǸ�,�Ա���˼���������Ϊ'T'����ϽǸ�
	CHashList<ANGLESIZE_RECORD>hashGroupAngleT;	//��ֵΪ֫��*100+֫������Ϊ�Ǹֹ����е������� wjh-2019.7.11
	for (i = 0; i < jgguige_N; i++)
	{
		if (jgguige_table[i].cType != 'D'&&jgguige_table[i].cType != 'T')
			continue;
		int liKey = ftoi(jgguige_table[i].wing_wide) * 100 + ftoi(jgguige_table[i].wing_thick);
		ANGLESIZE_RECORD *pSizeReccord = NULL;
		if((pSizeReccord = hashGroupAngleT.GetValue(liKey))!=NULL&&
			jgguige_table[pSizeReccord->liIndexInTable].cType == 'T')
			continue;
		hashGroupAngleT.SetValue(liKey, ANGLESIZE_RECORD(i));	//ֻ�ڲ�������ͬ����'T'����ϽǸ�ǰ���²��趨��ǰ'D'����ϽǸ�������
	}
	if(pMapOnlyBraceElems==NULL)
	{
		i=0,n=listTtaElems.GetNodeNum();
		for(TTA_ELEM *pTtaElem=listTtaElems.GetFirst();pTtaElem;pTtaElem=listTtaElems.GetNext())
		{
			pRod=(CLDSLinePart*)console.AppendPart(CLS_LINEPART);
			pRod->m_uStatMatNo = pTtaElem->iStatMatNo;
			pRod->pStart=hashLdsNodeI.GetValue(pTtaElem->pStart->pointI);
			pRod->pEnd=hashLdsNodeI.GetValue(pTtaElem->pEnd->pointI);
			if(pRod->pStart)
				pRod->SetStart(pRod->pStart->Position(false));
			else
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"�˼�%d-%d��ʼ�˽ڵ㶪ʧ",pTtaElem->pStart->pointI,pTtaElem->pEnd->pointI);
			if(pRod->pEnd)
				pRod->SetEnd(pRod->pEnd->Position(false));
			else
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"�˼�%d-%d���ն˽ڵ㶪ʧ",pTtaElem->pStart->pointI,pTtaElem->pEnd->pointI);
			//1.��ԭN���������Ĺ�����
			if(m_ciFileType==2||pTtaElem->bReadSizeSpec)
			{	//SmartTower ��ʽ�й������iSizeOrder���������ǿ�����sizeֱ�ӱ��
				pRod->size_cSubClassType = pTtaElem->size.cSubClassType;
				if (pTtaElem->size.cSubClassType == 'T' || pTtaElem->size.cSubClassType == 'D' || pTtaElem->size.cSubClassType == 'X')
					pRod->size_idClassType = CLS_GROUPLINEANGLE;
				else
					pRod->size_idClassType=pTtaElem->size.idClassType;
				pRod->size_wide=pTtaElem->size.wide;
				pRod->size_thick=pTtaElem->size.thick;
			}
			else
			{
				if(pTtaElem->iSizeOrder>0)	//ָ�����
				{
					int sizeIndexOrder = pTtaElem->iSizeOrder;
					if (this->m_ciFileType == FILETYPE_TANS && (this->KIND % 10 == 6 || this->KIND % 10 == 9) && ((REN / 10) % 10 == 2))
					{	//����ԺTANS��ִ��6��9�淶ʱ����RENʰλΪ2��N�����⺬��
						//��ʱ��ǰ�ڶ�ȡ����ʱ��ͳ�ĺŴ������ظ���������Ҫ���ĺϲ����� wjh-2019.7.11
						char ch3ndDigit = (pTtaElem->iSizeOrder / 10000000) % 10;
						int indexOrder = pTtaElem->iSizeOrder % 10000000;
						if (ch3ndDigit != 9)
							logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT, "�����ļ��еĸ˼�%d-%d�Ĺ�����%dʶ��������ʵdata.ini�ļ�", pTtaElem->pStart->pointI, pTtaElem->pEnd->pointI, pTtaElem->iSizeOrder);
						else
						{
							char ch4thDigit = indexOrder / 1000000;
							indexOrder %= 1000000;
							if (ch4thDigit == 1)
								pRod->size_idClassType = CLS_LINEANGLE;
							else if (ch4thDigit == 5)
								pRod->size_idClassType = CLS_LINETUBE;
							WORD wiMaterial = indexOrder / 10000;	//5~6λ��ʾ����
							sizeIndexOrder = indexOrder % 10000;
						}
					}
					if (sizeIndexOrder > m_arrRodSpec.GetSize())
					{
						logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"�����ļ��еĸ˼�%d-%d�Ĺ�����%dʶ��������ʵdata.ini�ļ�",pTtaElem->pStart->pointI,pTtaElem->pEnd->pointI,sizeIndexOrder);
						pRod->size_idClassType = CLS_LINEANGLE;
					}
					else
					{
						pRod->size_wide=(int)m_arrRodSpec[sizeIndexOrder-1].size;
						pRod->size_thick=ftoi(100*(m_arrRodSpec[sizeIndexOrder-1].size-pRod->size_wide));
						if (m_arrRodSpec[sizeIndexOrder-1].iSizeSpecType==5&&pRod->size_wide<=2000)
							pRod->size_idClassType=CLS_LINETUBE;	//��ȴ���2000ʱ��ʵ��Ӧ������ϽǸֿ������û��汾��ʹ��ϰ��ԭ����ɸֹܡ�wjh-2016.1.11
						else
						{
							pRod->size_idClassType=CLS_LINEANGLE;
							if(pRod->size_wide>1000)
							{
								int group=(int)(pRod->size_wide/1000);
								pRod->size_wide=((int)pRod->size_wide)%1000;
								if(group==2)	//˫ƴ��������ϽǸ�
								{
									pRod->size_idClassType=CLS_GROUPLINEANGLE;
									pRod->size_cSubClassType='D';
								}
								else if(group==3)
								{
									pRod->size_idClassType = CLS_GROUPLINEANGLE;
									pRod->size_cSubClassType = 'T';
								}
								else if(group==4)
								{
									pRod->size_idClassType=CLS_GROUPLINEANGLE;
									pRod->size_cSubClassType='X';
								}
							}
						}
					}
				}
				else if (KIND%10==5)
					pRod->size_idClassType=CLS_LINETUBE;
				else
					pRod->size_idClassType=CLS_LINEANGLE;
			}
			pRod->cMaterial=pTtaElem->cMaterial;
			pRod->cfgword=pRod->pStart->cfgword&pRod->pEnd->cfgword;
			if(pRod->cfgword.IsNull()&&pRod->pStart->layer(0)=='L')
				pRod->cfgword=pRod->pStart->cfgword;
			else if(pRod->cfgword.IsNull()&&pRod->pEnd->layer(0)=='L')
				pRod->cfgword=pRod->pEnd->cfgword;
			if(pRod->pStart->layer(0)=='L'||pRod->pEnd->layer(0)=='L')
				pRod->layer(0)='L';
			//2.����TYPE������
			pRod->start_force_type=pRod->end_force_type=ECCENTRIC_FORCE;//Ĭ��ƫ������
			if(pTtaElem->TYPE==1||pTtaElem->TYPE==-1)
			{
				//if(pTtaElem->TYPE==1)
					pRod->layer(1)='Z';
				/* ���ͼ��������ڴ˲������޸ģ���Ӧ�ø���ʵ���������ִ��� wjh-2017.6.12
				else if(pRod->Layer(0)=='T')
					pRod->layer(1)='Z';
				else
				{	//����ֻ��˫���������Ĳų�֮Ϊ���ģ�ͷ���ᵣ�п��ܴ��ڵ����������������wjh-5-16
					if(fabs(pRod->pStart->Position('Z',false)-pRod->pEnd->Position('Z',false))<EPS2)
						pRod->layer(1)='H';	//��ЩԺ��Ѻ��������ΪTYPE=-1���������ܵ��½ڵ㸸�˼�����ʱ�޷�������Ĭ�ϲ����Ȳ����ģ� wjh-2017.5.12
					else
						pRod->layer(1)='X';
				}*/
				if(pTtaElem->TYPE>0)	//˫֫���ӣ���������
					pRod->start_force_type=pRod->end_force_type=CENTRIC_FORCE;
				else					//��֫���ӣ�ƫ������
					pRod->start_force_type=pRod->end_force_type=ECCENTRIC_FORCE;
			}
			else if(pTtaElem->TYPE==2)	//(�Ȳ�)б�ģ��Ƿ�Ϊ�Ȳ�Ӧ�Խڵ�����Ϊ׼
			{
				pRod->layer(1)='X';
				pRod->m_bLegVRod=true;//ǿ�ư��Ȳ�Vб�ĳ�1.2ϵ�����Ǽ��㳤��
			}
			else if(pTtaElem->TYPE==3)	//����б��
				pRod->layer(1)='X';
			//�������͵ĸ������ǲ�Ԥ����Ԫ�նȾ������ģ���ֱ�Ӹ�Ϊ���Ĳ����� wjh-2019.7.8
			else if(pTtaElem->TYPE==5)
			{	//������������
				pRod->layer(1)='X';
				pRod->m_bCheckByBrace=true;
			}
			else
				pRod->layer(1)='X';
			if(pRod->layer(0)=='L')
				pRod->layer(2)=CalPoleQuad(pRod->pStart->Position(false),pRod->pEnd->Position(false));
			if(pRod->start_force_type==CENTRIC_FORCE)
				pRod->connectStart.m_iConnectWing=0;	//
			else
				pRod->connectStart.m_iConnectWing=1;	//
			if(pRod->end_force_type==CENTRIC_FORCE)
				pRod->connectEnd.m_iConnectWing=0;	//
			else
				pRod->connectEnd.m_iConnectWing=1;	//
			//3.����M������
			if(KIND%10>2)
			{
				int M01=pTtaElem->M%10;	//��λ
				int M10=(pTtaElem->M/10)%10;//ʰλ
				if(M10==0)		//������������
					pRod->start_force_type=pRod->end_force_type=CENTRIC_FORCE;
				else if(M10==2)	//����ƫ������
					pRod->start_force_type=pRod->end_force_type=ECCENTRIC_FORCE;
				else			//һ������һ��ƫ������
				{
					pRod->start_force_type=CENTRIC_FORCE;
					pRod->end_force_type=ECCENTRIC_FORCE;
				}
				if(M01==0)		//���˽½ӣ����κ�ת��Լ��
					pRod->start_joint_type=pRod->end_joint_type=JOINT_AUTOJUSTIFY;
				else if(M01==2)	//���˸սӣ����ܲ���ת��Լ��
					pRod->start_joint_type=pRod->end_joint_type=JOINT_RIGID;
				else			//һ�˽½ӣ�����ܲ���ת��Լ��
				{
					pRod->start_joint_type=JOINT_AUTOJUSTIFY;
					pRod->end_joint_type=JOINT_RIGID;
				}
			}
			//4.L1��L2��ϸ�ȼ���������
			pRod->CalLenCoef.iTypeNo=5;	//ָ����ϸ��
			pRod->CalLenCoef.minR.hStartNode=pRod->CalLenCoef.paraR.hStartNode=pRod->pStart->handle;
			pRod->CalLenCoef.minR.hEndNode=pRod->CalLenCoef.paraR.hEndNode=pRod->pEnd->handle;
			if(pTtaElem->L1>0&&pTtaElem->L1<10)
				pRod->CalLenCoef.minR.coef=1.0/pTtaElem->L1;
			else if(pTtaElem->L1>=10)
				pRod->CalLenCoef.minR.coef=0.01*pTtaElem->L1;
			else	//L1<0,��������ȫ���˼��󲹳�ͬʱ��ѹ��Ϣ
				pRod->CalLenCoef.minR.coef=pTtaElem->L1;
			if (pTtaElem->L1 == 0 && pRod->size_cSubClassType == 'D')
			{	//ͬʱ���Ĺ��⣬����ᵼ�¼���������  wjh - 2019.7.11
				pRod->size_cSubClassType = 'T';	//T����ϽǸ�����ʱ����С������0 wjh-2018.6.1
				int liKey = ftoi(pRod->size_wide) * 100 + ftoi(pRod->size_thick);
				ANGLESIZE_RECORD *pSizeRec = NULL;
				//ֻ�ڲ�������ͬ����'T'����ϽǸ�ǰ���²��趨��ǰ'D'����ϽǸ�������
				if ((pSizeRec = hashGroupAngleT.GetValue(liKey)) != NULL)
				{
					if (pSizeRec->liSizeStatNo < 0)
						pSizeRec->liSizeStatNo = pTtaElem->iStatMatNo;
					jgguige_table[pSizeRec->liIndexInTable].cType = 'T';
				}
			}
			if(pTtaElem->L2>0&&pTtaElem->L2<10)
				pRod->CalLenCoef.paraR.coef=1.0/pTtaElem->L2;
			else if(pTtaElem->L2>=10)
				pRod->CalLenCoef.paraR.coef=0.01*pTtaElem->L2;
			else	//L2<0,��������ȫ���˼��󲹳�ͬʱ��ѹ��Ϣ
				pRod->CalLenCoef.paraR.coef=pTtaElem->L2;
			pRod->connectStart.wnSummBoltN=pTtaElem->boltN;
			pRod->connectStart.d=pTtaElem->boltd;
			pRod->IntelliUpdateConnBoltsBySummBolts();
			CXhString szGrade(pRod->connectStart.grade,5);
			if(pTtaElem->boltgrade>0)
				szGrade.Printf("%.1f",pTtaElem->boltgrade);
			else
			{
				//TODO:���ܸ�ֵ��˨�ȼ�pRod->connectStart.d=SelBoltDByWingWide(pRod->size_wide,pRod->connectStart.grade);
			}
			pRod->connectEnd=pRod->connectStart;
			//5.SMC��Ϣ
			if(KIND%10!=3)
			{	//ִ���й���׼ʱ��SMC����ͬʱ��ѹ��Ϣ������
				//if(pTtaElem->SMC==1||pTtaElem->SMC==2)
				//	pRod->CalLenCoef.hRefPole=pTtaElem->SMC;//1:Y��(����)�Գƶ���;2:X��(ǰ��)�ԳƲ���
				//else if(abs(pTtaElem->SMC>=3&&abs(pTtaElem->SMC)<10000)//�ݲ�֧��
				if(abs(pTtaElem->SMC)>=10000&&abs(pTtaElem->SMC)<20000)
				{	//8�Խ���б��ͬ�˽ڵ�ͬʱ��ѹ
					//���ǶԳƸ˼���ͬʱ��ѹ��Ϣ
					pRod->CalLenCoef.iTypeNo=1;
				}
				else if(abs(pTtaElem->SMC)>=20000&&abs(pTtaElem->SMC)<30000)
					pRod->CalLenCoef.iTypeNo=3;
				else if(abs(pTtaElem->SMC)>=30000)
					pRod->CalLenCoef.iTypeNo=4;
				//���������������ļ��ģɣ׼������Ϣ�ж�
				pRod->m_bAutoCalHoleOutCoef=FALSE;	//TTA����ָ������ϵ����
				if(pTtaElem->dfHoleOutCoef>0)
					pRod->hole_out=pTtaElem->dfHoleOutCoef;
				else if(pRod->IsAuxPole())
					pRod->hole_out=HOLECOEF_ARR[3];
				else if(pRod->Layer(1)!='Z')	//��ͨ������
					pRod->hole_out=HOLECOEF_ARR[2];
				else if(pTtaElem->size.wide<IW)
					pRod->hole_out=HOLECOEF_ARR[0];
				else //if(pTtaElem->size.wide>=IW)
					pRod->hole_out=HOLECOEF_ARR[1];
			}
			else //if(KIND%10==3)
			{	//ִ������ASCE��׼ʱ��SMC���������
				pRod->m_bAutoCalHoleOutCoef=FALSE;
				pRod->hole_out=pTtaElem->SMC*0.01;
			}
			if(pRod->pStart&&(pRod->pStart->hFatherPart==0||pRod->pStart->feature<pRod->size_wide*100+pRod->size_thick))
			{
				pRod->pStart->hFatherPart=pRod->handle;
				pRod->pStart->feature=(int)(pRod->size_wide*100+pRod->size_thick);
			}
			if(pRod->pEnd&&(pRod->pEnd->hFatherPart==0||pRod->pEnd->feature<pRod->size_wide*100+pRod->size_thick))
			{
				pRod->pEnd->hFatherPart=pRod->handle;
				pRod->pEnd->feature=(int)(pRod->size_wide*100+pRod->size_thick);
			}
			NewLinePart(pRod,false);
			i++;
			DisplayProcess(55+i*10/n,title);
		}
		//����ϸ���н���б��ͬʱ��ѹ���
		CMapList<CLDSNode*> hashIntersNodeMap;
		for(pRod=m_pContextModel->EnumRodFirst();pRod;pRod=m_pContextModel->EnumRodNext())
		{
			CLdsPartListStack stack(m_pContextModel);
			//BOOL pushed=Ta.Parts.push_stack();
			if(pRod->CalLenCoef.minR.coef<0||pRod->CalLenCoef.paraR.coef<0)
			{	//��ͨ�����彻��㲹�佻��б�Ĳ��ո˼�
				MIRMSG mirmsg;
				if(abs((int)pRod->CalLenCoef.minR.coef)%10==5)	//���ҶԳ�
					pMirRod=(CLDSLinePart*)pRod->GetMirPart(MIRMSG(2));
				else if(abs((int)pRod->CalLenCoef.minR.coef)%10==6)	//ǰ��Գ�
					pMirRod=(CLDSLinePart*)pRod->GetMirPart(MIRMSG(1));
				else if(abs((int)pRod->CalLenCoef.paraR.coef)%10==5)	//���ҶԳ�
					pMirRod=(CLDSLinePart*)pRod->GetMirPart(MIRMSG(2));
				else if(abs((int)pRod->CalLenCoef.paraR.coef)%10==6)	//ǰ��Գ�
					pMirRod=(CLDSLinePart*)pRod->GetMirPart(MIRMSG(1));
				else
					pMirRod=NULL;
				pRod->CalLenCoef.iTypeNo=0;	//���ܼ���
				if(pMirRod!=NULL&&pRod!=pMirRod)
				{
					//pMirRod->CalLenCoef.iTypeNo=0;
					//pMirRod->CalLenCoef.minR.coef=pMirRod->CalLenCoef.paraR.coef=0;
					if(pRod->CalLenCoef.hRefPole==0)
					{
						pRod->CalLenCoef.hRefPole=pMirRod->handle;
						pMirRod->CalLenCoef.hRefPole=pRod->handle;
					}
					int minPointI=min(pRod->pStart->point_i,pRod->pEnd->point_i);
					int minMirRodPointI=min(pMirRod->pStart->point_i,pMirRod->pEnd->point_i);
					CLDSNode*  pIntersNode=NULL;
					CLDSNode** ppIntersNode=NULL;
					if(minPointI<minMirRodPointI)
						ppIntersNode=hashIntersNodeMap.GetValue(pRod->handle,pMirRod->handle);
					else
						ppIntersNode=hashIntersNodeMap.GetValue(pMirRod->handle,pRod->handle);
					if(ppIntersNode!=NULL)	//�ý�����Ѿ�����
						pIntersNode=*ppIntersNode;
					else
					{
						pIntersNode=console.AppendNode();
						listIntersNodes.append(pIntersNode);
						pIntersNode->layer(0)=pRod->layer(0);
						pIntersNode->layer(2)=pRod->layer(2);
						pIntersNode->cfgword=pRod->cfgword;
						pIntersNode->m_cPosCalType=4;	//�����
						pIntersNode->arrRelationNode[0]=pRod->pStart->handle;
						pIntersNode->arrRelationNode[1]=pRod->pEnd->handle;
						pIntersNode->arrRelationNode[2]=pMirRod->pStart->handle;
						pIntersNode->arrRelationNode[3]=pMirRod->pEnd->handle;
						pIntersNode->hFatherPart=pRod->handle;
						pIntersNode->arrRelationPole[0]=pRod->handle;
						pIntersNode->arrRelationPole[1]=pMirRod->handle;
						pIntersNode->Set();
						NewNode(pIntersNode);
						if(minPointI<minMirRodPointI)
							hashIntersNodeMap.SetValue(pRod->handle,pMirRod->handle,pIntersNode);
						else
							hashIntersNodeMap.SetValue(pMirRod->handle,pRod->handle,pIntersNode);
					}
					double Ls=DISTANCE(pRod->pStart->Position(false),pIntersNode->Position(false));
					double Le=DISTANCE(pRod->pEnd->Position(false),pIntersNode->Position(false));
					CLDSNode* pDatumEndNode=pRod->pEnd;
					if(Ls>Le)
						pDatumEndNode=pRod->pStart;
					if(pRod->CalLenCoef.minR.coef!=0)
					{
						if(abs((int)pRod->CalLenCoef.minR.coef)/10==0)	//��С�᲼��
						{
							pRod->CalLenCoef.minR.hStartNode=pIntersNode->handle;
							pRod->CalLenCoef.minR.hEndNode=pDatumEndNode->handle;
							pRod->CalLenCoef.minR.coef=1.0;
							pRod->CalLenCoef.iTypeNo=6;	//������TTA���޸���,�����ݲ������ܼ���,����Ϊָ�����㳤�� wjh-2015.5.26
						}
						else if(abs((int)pRod->CalLenCoef.minR.coef)/10==1)	//��С�᲼��
						{
							pRod->CalLenCoef.minR.hStartNode=pIntersNode->handle;
							pRod->CalLenCoef.minR.hEndNode=pDatumEndNode->handle;
							pRod->CalLenCoef.minR.coef=0.5;
							pRod->CalLenCoef.iTypeNo=6;	//������TTA���޸���,�����ݲ������ܼ���,����Ϊָ�����㳤�� wjh-2015.5.26
						}
					}
					else
					{
						pRod->CalLenCoef.minR.hStartNode=pRod->CalLenCoef.minR.hEndNode=0;
						pRod->CalLenCoef.minR.coef=0;
					}

					if(pRod->CalLenCoef.paraR.coef!=0)
					{
						if(abs((int)pRod->CalLenCoef.paraR.coef)/10==0)	//��С�᲼��
						{
							pRod->CalLenCoef.paraR.hStartNode=pIntersNode->handle;
							pRod->CalLenCoef.paraR.hEndNode=pDatumEndNode->handle;
							pRod->CalLenCoef.paraR.coef=1.0;
							pRod->CalLenCoef.iTypeNo=6;	//������TTA���޸���,�����ݲ������ܼ���,����Ϊָ�����㳤�� wjh-2015.5.26
						}
						else if(abs((int)pRod->CalLenCoef.paraR.coef)/10==1)	//ƽ���᲼��
						{
							pRod->CalLenCoef.paraR.hStartNode=pIntersNode->handle;
							pRod->CalLenCoef.paraR.hEndNode=pDatumEndNode->handle;
							pRod->CalLenCoef.paraR.coef=0.5;
							pRod->CalLenCoef.iTypeNo=6;	//������TTA���޸���,�����ݲ������ܼ���,����Ϊָ�����㳤�� wjh-2015.5.26
						}
						else if(abs((int)pRod->CalLenCoef.paraR.coef)/10==2)	//ȫ��ƽ���᲼��
						{
							pRod->CalLenCoef.paraR.hStartNode=pIntersNode->handle;
							pRod->CalLenCoef.paraR.hEndNode=pDatumEndNode->handle;
							pRod->CalLenCoef.paraR.coef=1.1;
							pRod->CalLenCoef.iTypeNo=6;	//������TTA���޸���,�����ݲ������ܼ���,����Ϊָ�����㳤�� wjh-2015.5.26
						}
					}
					else
					{
						pRod->CalLenCoef.paraR.hStartNode=pRod->CalLenCoef.paraR.hEndNode=0;
						pRod->CalLenCoef.paraR.coef=0;
					}
				}
				if(pRod->CalLenCoef.iTypeNo==0)
				{
					pRod->CalLenCoef.minR.hStartNode=pRod->CalLenCoef.minR.hEndNode=0;
					pRod->CalLenCoef.paraR.hStartNode=pRod->CalLenCoef.paraR.hEndNode=0;
					pRod->CalLenCoef.minR.coef=pRod->CalLenCoef.paraR.coef=0;
				}
			}
			//Ta.Parts.pop_stack(pushed);
		}
		for (pRod = m_pContextModel->EnumRodFirst(); pRod; pRod = m_pContextModel->EnumRodNext())
		{	//����ͬһT����ϽǸ��е�ĳһ�ο��ܴ���У����С�ᣬ����ʶ���'D'����ϽǸֵ���� wjh-2019.7.11
			if (!pRod->IsAngle(true) || pRod->size_cSubClassType != 'D')
				continue;
			long liKey = ftoi(pRod->size_wide) * 100 + ftoi(pRod->size_thick);
			ANGLESIZE_RECORD *pSizeRec = NULL;
			//ֻ�ڲ�������ͬ����'T'����ϽǸ�ǰ���²��趨��ǰ'D'����ϽǸ�������
			if ((pSizeRec = hashGroupAngleT.GetValue(liKey)) != NULL)
			{	//ͳ�ĺ���ͬ�����ݽǸֹ������½Ǹֽ��������� wjh-2019.7.11
				if (pSizeRec->liSizeStatNo == pRod->m_uStatMatNo)
					pRod->size_cSubClassType = jgguige_table[pSizeRec->liIndexInTable].cType;
			}
		}
	}
	if(pMapOnlyBraceElems!=NULL)
	{
		i=0;
		n=listTtaBraceElems.GetNodeNum();
	}
	for(TTA_BRACEELEM *pBrace=listTtaBraceElems.GetFirst();pBrace;pBrace=listTtaBraceElems.GetNext())
	{
		if(pMapOnlyBraceElems&&pMapOnlyBraceElems->GetValue(pBrace->pStart->pointI,pBrace->pEnd->pointI)==NULL)
			continue;	//���ؽ����ļ��˽ڵ�ʱ���˵�֮ǰ¼��ڵ�
		CLDSNode* pNodeS=hashLdsNodeI.GetValue(pBrace->pStart->pointI);
		CLDSNode* pNodeE=hashLdsNodeI.GetValue(pBrace->pEnd->pointI);
		if(pNodeS==NULL)
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"�˼�[%d-%d]��ʼ�˽ڵ㶪ʧ",pBrace->pStart->pointI,pBrace->pEnd->pointI);
		if(pNodeE==NULL)
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"�˼�[%d-%d]���ն˽ڵ㶪ʧ",pBrace->pStart->pointI,pBrace->pEnd->pointI);
		if(pNodeS==NULL||pNodeE==NULL)
			continue;
		CLDSLinePart *pRod=(CLDSLinePart*)console.AppendPart(CLS_LINEPART);
		pBrace->pRod=pRod;
		pRod->pStart=pNodeS;
		pRod->pEnd=pNodeE;
		pRod->SetStart(pRod->pStart->Position(false));
		pRod->SetEnd(pRod->pEnd->Position(false));
		if(KIND%10==5)
			pRod->size_idClassType=CLS_LINETUBE;
		else
			pRod->size_idClassType=CLS_LINEANGLE;
		pRod->cfgword=pRod->pStart->cfgword&pRod->pEnd->cfgword;
		if(pRod->cfgword.IsNull()&&pRod->pStart->layer(0)=='L')
			pRod->cfgword=pRod->pStart->cfgword;
		else if(pRod->cfgword.IsNull()&&pRod->pEnd->layer(0)=='L')
			pRod->cfgword=pRod->pEnd->cfgword;
		if(pRod->pStart->layer(0)=='L'||pRod->pEnd->layer(0)=='L')
			pRod->layer(0)='L';
		pRod->layer(1)='F';
		if(pRod->layer(0)=='L')
			pRod->layer(2)=CalPoleQuad(pRod->pStart->Position(false),pRod->pEnd->Position(false));
		if(pBrace->cSectType=='R')
			pRod->size_idClassType=CLS_LINETUBE;
		else
		{
			pRod->size_idClassType=CLS_LINEANGLE;
			if(pBrace->cSectType=='T')
				pRod->size_cSubClassType=pBrace->cSectType;
		}
		//����ͳһ����֫���ӿ���
		pRod->connectStart.m_iConnectWing=pRod->connectEnd.m_iConnectWing=1;	//
		pRod->size_wide=pBrace->size_wide;
		pRod->size_thick=pBrace->size_thick;
		pRod->cMaterial=pBrace->cBriefMaterial;
		pRod->connectStart.wnSummBoltN=1;
		pRod->connectStart.d=SelBoltDByWingWide(pRod->size_wide,pRod->connectStart.grade);
		pRod->connectEnd=pRod->connectStart;
		pRod->IntelliUpdateConnBoltsBySummBolts();
		if(KIND%10>2)
		{
			int M01=pBrace->M%10;	//��λ
			int M10=(pBrace->M/10)%10;//ʰλ
			if(M10==0)		//������������
				pRod->start_force_type=pRod->end_force_type=CENTRIC_FORCE;
			else if(M10==2)	//����ƫ������
				pRod->start_force_type=pRod->end_force_type=ECCENTRIC_FORCE;
			else			//һ������һ��ƫ������
			{
				pRod->start_force_type=CENTRIC_FORCE;
				pRod->end_force_type=ECCENTRIC_FORCE;
			}
			if(M01==0)		//���˽½ӣ����κ�ת��Լ��
				pRod->start_joint_type=pRod->end_joint_type=JOINT_AUTOJUSTIFY;
			else if(M01==2)	//���˸սӣ����ܲ���ת��Լ��
				pRod->start_joint_type=pRod->end_joint_type=JOINT_RIGID;
			else			//һ�˽½ӣ�����ܲ���ת��Լ��
			{
				pRod->start_joint_type=JOINT_AUTOJUSTIFY;
				pRod->end_joint_type=JOINT_RIGID;
			}
		}
		pRod->CalLenCoef.minR.hStartNode=pRod->CalLenCoef.minR.hEndNode=0;
		pRod->CalLenCoef.paraR.hStartNode=pRod->CalLenCoef.paraR.hEndNode=0;
		pRod->CalLenCoef.minR.coef=pRod->CalLenCoef.paraR.coef=0;
		if(pBrace->percentOfMinRL>=10)
		{
			pRod->CalLenCoef.iTypeNo=5;
			pRod->CalLenCoef.minR.coef=0.01*pBrace->percentOfMinRL;
		}
		if(pBrace->percentOfParaRL>=10)
		{
			pRod->CalLenCoef.iTypeNo=5;
			pRod->CalLenCoef.paraR.coef=0.01*pBrace->percentOfParaRL;
		}
		NewLinePart(pRod,false);
		if(pMapOnlyBraceElems!=NULL)
		{
			i++;
			DisplayProcess(50+i*15/n,title);
		}
	}
	if(updateMirRelaInfo)
	{
		PROGRESS_CONTEXT processcontext;
		processcontext.ciStartProgressNumber=65;
		processcontext.ciToProgressNumber=100;
		processcontext.title="����������������Գ���Ϣ";
		m_pContextModel->AddMirRelativeObjInfo(&processcontext);
	}
	DisplayProcess(100,title);
}
char* ReadLineText(char* destStr,int maxStrLength,FILE* fp)
{
	fgets(destStr,maxStrLength,fp);
	char dumpStr[201]="";
	dumpStr[0]=destStr[strlen(destStr)-1];
	dumpStr[1]=0;
	while(dumpStr[strlen(dumpStr)-1]!='\n'&&!feof(fp))	//.�]β�������ǻ��з����ļ���ֹ
		fgets(dumpStr,200,fp);
	return destStr;
}
bool CTtaDataFile::ReadSmartTowerLoadSection(FILE* fp)
{
	char ciFileType = FILETYPE_ST;
	//�š��������ػ������ݼ��������ϵ��
	CXhChar200 sText;
	ReadLineText(sText, 100, fp);
	int i,j,NPANEL, VG, VH;
	char EXP0[50] = "";	//����ֲڶ�
	if (KIND >= 1000)	//�Զ�������������
	{
		//sscanf(sText,"%d%d%d%s",&NPANEL,&VG,&VH,EXP0);	//�����������У�" 0.000     1.000     1.000"
		CXhChar50 xarrItems[8];
		DELIMITER_STR::ParseWordsFromStr(sText, " \t", xarrItems, 8);
		NPANEL = atoi(xarrItems[0]);
		VG = atoi(xarrItems[1]);
		VH = (int)atof(xarrItems[2]);
		StrCopy(EXP0, xarrItems[3], 50);
		m_arrWindSeg.SetSize(NPANEL);
		Ta.m_cLandscape = 0;
		for (i = 0;EXP0[i] != 0;i++)
		{
			if (EXP0[i]<'A' || EXP0[i]>'D')
				continue;
			Ta.m_cLandscape = EXP0[i];
			break;
		}
		if (Ta.m_cLandscape<'A' || Ta.m_cLandscape>'D')
		{
			logerr.Log("����ص���ֲڶ��������,����ʧ��,ϵͳĬ��Ϊ'B'��");
			Ta.m_cLandscape = 'B';
		}
	}
	CWorkStatus *pStatus;
	Ta.WorkStatus.Empty();
	Ta.m_iLoadExecuteCode = 3;	//�û�ָ������ִ�й淶
	CObjNoGroup *pWorkStatusGroup = Ta.NoManager.FromGroupID(WORKSTATUS_GROUP);
	pWorkStatusGroup->EmptyNo();
	for (i = 0;i < NL;i++)
	{
		pStatus = Ta.WorkStatus.append();
		pStatus->iNo = pWorkStatusGroup->EnumIdleNo();
		pWorkStatusGroup->SetNoState(pStatus->iNo);
		ReadLineText(sText, 200, fp);
		//���뽫������Ϊָ������ϵ��ģʽ�������޷�����������ָ���ɱ�������ϵ��
		pStatus->m_bSpecLoadFactors = true;
		pStatus->m_bLoadIncFactor = true;
		//FyΪ��ѹ����ϵ������������ϵ���˻�,FacΪ�������ϵ�����������к���
		double Fx, Fy, Fz;
		double Fac;	//�ܺ���ϵ��(����Myl�ļ��н�ָx,y�����ϵ����
		double fMylIceWeightAmplifyCoef = 0;	//���า����������ϵ��(����Ҫ��ϵ��*����ϵ��)
		double dfSmartTowerLoadTypeValue = 0;	//SmartTower����ʱ��������������1.���;2.����;3.����;4.��װ;5.����6.����;7.����2
		//TODO:mylCoef������֪��ʲô�ģ�����Fac��Fy�����������Ҳ�б� wjh-2015.12.07
		sscanf(sText, "%lf%lf%lf%lf%lf%lf%s", &Fx, &Fy, &Fz, &Fac, &fMylIceWeightAmplifyCoef, &dfSmartTowerLoadTypeValue,pStatus->description);
		if (KIND >= 1000)
		{
			pStatus->WindLoad.velocity = Fx;
			pStatus->WindLoad.azimuth = ftoi(Fz);
			pStatus->SetKesai(1.0);		//�趨��ͨ�ɱ�������ϵ��Ϊ1.0
			switch (ftoi(dfSmartTowerLoadTypeValue))
			{	//0:���1:����2:����3:���߼������ȸ���4:��װ5:ˮƽ����6:�������7:��ˮƽ����8:���������
			case 1:	//���
				pStatus->m_cMainLoadType;
				break;
			case 2:	//����
				pStatus->m_cMainLoadType=1;
				break;
			case 3:	//���߼������ȸ���-����
				pStatus->m_cMainLoadType=3;
				break;
			case 4:	//��װ
				pStatus->m_cMainLoadType=4;
				break;
			case 6:	//����
				pStatus->m_cMainLoadType=2;
				break;
			case 5:	//����
			case 7:	//����2
			default:
				break;
			}

			if (ciFileType == 1)
			{
				pStatus->SetKesai_W(Fy*Fac);//�趨��������ϵ��ΪFy*Fac;
				if (fMylIceWeightAmplifyCoef < 1.0)
					fMylIceWeightAmplifyCoef = 1.0;
				pStatus->SetGamaQ(fMylIceWeightAmplifyCoef);		//TTA�������ѿ��ǿɱ���ط���ϵ����wjh-2015.6.14
			}
			else
			{
				pStatus->SetKesai_W(Fy*Fac);//�趨��������ϵ��ΪFy*Fac;
				pStatus->SetGamaQ(1.0*Fac);		//TTA�������ѿ��ǿɱ���ط���ϵ����wjh-2015.6.14
			}
			//pStatus->SetKesai(Fac);
		}
		/*double betaZ=1.0;
		if(Ta.m_fTowerHeight<=20)
			betaZ=1.0;
		else if(Ta.m_fTowerHeight<=30)
			betaZ=1.0+0.025*(Ta.m_fTowerHeight-20);
		else if(Ta.m_fTowerHeight<=40)
			betaZ=1.25+0.01*(Ta.m_fTowerHeight-30);
		else if(Ta.m_fTowerHeight<=50)
			betaZ=1.35+0.015*(Ta.m_fTowerHeight-40);
		else //if(m_fTowerHeight<=60)
			betaZ=1.5+0.01*(Ta.m_fTowerHeight-50);
		betaZ*=pStatus->Bw_I();
		fprintf(fp,"%8.3f%8.3f%5d.000   1.000       %s\n",pStatus->WindLoad.velocity,
			pStatus->Kesai_W()*betaZ,pStatus->WindLoad.azimuth,pStatus->description);*/
	}
	//ʮ�������ߺ�������	
	for (i = 0;i < NC;i++)
	{
		bool bLoadNode = false;
		GEPOINT stdload, load;
		CExternalNodeLoad nodeLoad;
		int pointI;
		ReadEffectLineText(sText, 100, fp);
		sscanf(sText, "%d%lf%lf%lf", &pointI, &load.x, &load.y, &load.z);
		load *= 1000;	//kN->N
		CLDSNode *pNode = hashLdsNodeI[pointI];
		CWorkStatus *pFirstStatus = Ta.WorkStatus.GetFirst();
		//������ر�׼ֵ wjh-2015.3.4
		double dfDesignCoef=pFirstStatus->GamaQ()*pFirstStatus->Kesai();
		if (fabs(dfDesignCoef)<EPS)
			dfDesignCoef=1.0;
		stdload = load / dfDesignCoef;
		if (pFirstStatus->m_cMainLoadType < 4)
			stdload /= Ta.m_fSafetyCoef;
		nodeLoad.designLoad = load;
		nodeLoad.stdLoad = stdload;
		if (pNode == NULL)
		{
			logerr.Log("%d��δ�ҵ�����ȴ�Ǻ��ؽڵ�", pointI);
			continue;
		}
		CExternalNodeLoad* pNodeLoad = pFirstStatus->hashNodeLoad.Add(pNode->handle);
		//��������Ӧ������Ӧ������������ͬһ���ڵ����������(ͬΪ���߹ҵ�����߹ҵ�) wjh-2017.3.16
		pNodeLoad->designLoad += nodeLoad.designLoad;
		pNodeLoad->stdLoad += nodeLoad.stdLoad;
		for (pStatus = Ta.WorkStatus.GetNext();pStatus;pStatus = Ta.WorkStatus.GetNext())
		{
			ReadEffectLineText(sText, 100, fp);
			sscanf(sText, "%lf%lf%lf", &load.x, &load.y, &load.z);
			load *= 1000;	//kN->N
			//������ر�׼ֵ wjh-2015.3.4
			dfDesignCoef=pStatus->GamaQ()*pStatus->Kesai();
			if (fabs(dfDesignCoef)<EPS)
				dfDesignCoef=1.0;
			stdload = load / dfDesignCoef;
			if (pStatus->m_cMainLoadType < 4)
				stdload /= Ta.m_fSafetyCoef;
			nodeLoad.designLoad = load;
			nodeLoad.stdLoad = stdload;
			pNodeLoad = pStatus->hashNodeLoad.Add(pNode->handle);
			pNodeLoad->designLoad += nodeLoad.designLoad;
			pNodeLoad->stdLoad += nodeLoad.stdLoad;
		}
	}
	if (NC > 0)
		ExtractWireNode();
	//ʮһ��������������
	for (i = 0;i < NW;i++)
		ReadEffectLineText(sText, 100, fp);	//����������������
	//ʮһ�������ѹ������
	TTA_WINDSEG ttaseg;
	Ta.WindLoadSeg.Empty();
	CObjNoGroup *pWindSegGroup = Ta.NoManager.FromGroupID(WINDSEG_GROUP);
	pWindSegGroup->EmptyNo();
	CHashSet<CWindSegment*> hashBelongWindSegByRodId;
	for (i = 0;i < NPANEL;i++)
	{
		ReadEffectLineText(sText, 100, fp);
		sscanf(sText, "%d%d%d%d%d%d%d%d%d%d", &ttaseg.TYPE, &ttaseg.arrPointI[0],
			&ttaseg.arrPointI[1], &ttaseg.arrPointI[2], &ttaseg.arrPointI[3],
			&ttaseg.NLG, &ttaseg.Fa, &ttaseg.Fb, &ttaseg.Fw, &ttaseg.BetaZ);
		CWindSegment *pSeg = Ta.WindLoadSeg.append();
		pSeg->iNo = pWindSegGroup->EnumIdleNo();
		pWindSegGroup->SetNoState(pSeg->iNo);
		int calStyle = ttaseg.TYPE / 1000;
		if (calStyle == 0)
			pSeg->cCalStyle = 0;	//����ʽ����
		else if (calStyle == 1)
			pSeg->cCalStyle = 1;
		else if (calStyle == 2)
			pSeg->cCalStyle = 2;
		if (ttaseg.TYPE % 1000 >= 100)
			pSeg->cMirType = 1;
		else
			pSeg->cMirType = 0;
		int segType = ttaseg.TYPE % 10;
		if (segType == 1)
			pSeg->cType = 0;	//�˽ڵ���������
		else
			pSeg->cType = 1;	//�Զ����ڵ�����
		//if(segType==
		for (j = 0;j < 4;j++)
		{
			if (ttaseg.arrPointI[j] > 0)
			{
				if (hashLdsNodeI.GetValue(ttaseg.arrPointI[j]) != NULL)
				{
					pSeg->nodePtrArr[j] = hashLdsNodeI[ttaseg.arrPointI[j]];
					pSeg->nodeArr[j] = pSeg->nodePtrArr[j]->handle;
				}
			}
		}
		pSeg->CqiFront = ttaseg.Fa*0.01;
		pSeg->CqiSide = ttaseg.Fb*0.01;
		if (ttaseg.BetaZ > 100)
			pSeg->BetaZ = ttaseg.BetaZ*0.01;
		else
			pSeg->BetaZ = 1.0;
		//Ӧ����ttaseg.Fw���·�ѹ���ڸ˼���Ԫ����������ϵ�� wjh-2016.6.02
		pSeg->StatValidNodeCount(&Ta);
		CLDSLinePart* pRod;
		if (pSeg->cType == 0)
		{	//�������ͱ�׼��ѹ��
			TAPER_SCOPE taper;
			for (j = 0;j < 2;j++)
			{
				if (pSeg->nodePtrArr[j] == NULL)
					continue;
				taper.VerifyVertex(pSeg->nodePtrArr[j]->Position(false));
				CLDSNode* pMirNode = pSeg->nodePtrArr[j]->GetMirNode(MIRMSG(3));
				if (pMirNode)
					taper.VerifyVertex(pMirNode->Position(false));
			}
			for (pRod = Ta.EnumRodFirst();pRod;pRod = Ta.EnumRodNext())
			{
				if (pRod->pStart == NULL || pRod->pEnd == NULL)
					continue;
				if (taper.IsIncludePoint(pRod->pStart->Position(false)) && taper.IsIncludePoint(pRod->pEnd->Position(false)))
				{
					CWindSegment* pBelongSeg=hashBelongWindSegByRodId.GetValue(pRod->Id);
					if(pBelongSeg==NULL||pBelongSeg->cType==1)	//������ǰͳ�Ƶ�����ᵣ��ѹ�Σ������������������ϵ��Ϊ׼
					{
						pRod->m_fWeightRaiseCoef = ttaseg.Fw*0.01;
						hashBelongWindSegByRodId.SetValue(pRod->Id,pSeg);
					}
				}
			}
		}
		else if (pSeg->cType == 1)//||pSeg->cType==2)
		{	//X��Y��ᵣ���ͱ�׼��ѹ�μ���
			WINDSEG_FILTER filter;
			filter.InitFilter(pSeg->nodePtrArr, pSeg->ValidNodeCount(), f3dPoint());
			for (pRod = Ta.EnumRodFirst();pRod;pRod = Ta.EnumRodNext())
			{
				if (pRod->pStart == NULL || pRod->pEnd == NULL)
					continue;
				if (filter.IsIncludeRod(pRod))
				{
					CWindSegment* pBelongSeg=hashBelongWindSegByRodId.GetValue(pRod->Id);
					if(pBelongSeg==NULL)	//������ǰͳ�Ƶ�����ᵣ��ѹ�Σ������ظ��趨
					{
						pRod->m_fWeightRaiseCoef = ttaseg.Fw*0.01;
						hashBelongWindSegByRodId.SetValue(pRod->Id,pSeg);
					}
				}
			}
		}
	}
	//ʮ������ͼ����
	//if (ciFileType != 1 && ciFileType != 2 && ciFileType != 3)
	//	return;
	//else if (ReadEffectLineText(sText, 100, fp) == NULL)
	//	return;
	return true;
}
void CTtaDataFile::Empty()
{
	hashTtaNodeI.Empty();
	hashTtaNodeH.Empty();
	hashLdsNodeI.Empty();
	m_arrBodyLeg.Empty();
	listTtaNodes.Empty();
	listTtaElems.Empty();
	listTtaBraceElems.Empty();
	localTtaNodeSets.Empty();
}
DWORD Hash2Key(DWORD key1,DWORD key2,DWORD nHashSize);	//defined in tower.cpp
CFGWORD CTtaDataFile::ValidateBraceNodeCfgWord(TTA_NODE* pTtaNode,bool bOnlyByDependency/*=false*/)
{
	if(!bOnlyByDependency)
	{
		if(pTtaNode->pointI<10000)	//�����Ķ˵���Ѹ��ݽڵ������жϣ�һ������Ч��
			return pTtaNode->cfgword;
		if(pTtaNode->bValidateCfgWord)
			return pTtaNode->cfgword;	//����֤���ĸ��Ľڵ�
		if(pTtaNode->posType==0)
			return pTtaNode->cfgword;
	}
	TTA_NODE* pDependStart=hashTtaNodeI.GetValue(pTtaNode->J1);
	TTA_NODE* pDependEnd  =hashTtaNodeI.GetValue(pTtaNode->J2);
	TTA_NODE* pDepend3  =hashTtaNodeI.GetValue(pTtaNode->J3);
	TTA_NODE* pDepend4  =hashTtaNodeI.GetValue(pTtaNode->J4);
	pTtaNode->cPosLayer='S';	//���Ľڵ���Ĭ��Ϊ����������ڽ����ϸ��Ľڵ�һ��������׷�ݵ����Ȼ����ڵ�
	if(pDependStart!=NULL)
	{
		if(pTtaNode->posType==12)
		{
			if(pDependStart->pNode)
			{
				pTtaNode->cPosLayer=pDependStart->pNode->layer(0);
				pTtaNode->cfgword=pDependStart->pNode->UnifiedCfgword();
			}
			else
			{
				pTtaNode->cPosLayer=pDependStart->cPosLayer;
				pTtaNode->cfgword=pDependStart->cfgword;
			}
		}
		else if(pDependStart->pNode)
		{	//�������ڵ�
			pTtaNode->cPosLayer=pDependStart->pNode->layer(0);
			pTtaNode->cfgword=pDependStart->pNode->UnifiedCfgword();
			if(pDependStart->pNode->IsLegObj())
				pTtaNode->cfgword=pDependStart->pNode->cfgword;	
		}
		else
		{	//��
			ValidateBraceNodeCfgWord(pDependStart,bOnlyByDependency);
			if(pDependStart->cPosLayer=='L')
			{
				pTtaNode->cPosLayer='L';
				pTtaNode->cfgword=pDependStart->cfgword;
			}
			else
			{
				if(bOnlyByDependency)
					pTtaNode->cfgword=pDependStart->cfgword;
				else
					pTtaNode->cfgword&=pDependStart->cfgword;
			}
		}
		pTtaNode->bValidateCfgWord=true;
	}
	if(pDependEnd!=NULL && pTtaNode->cPosLayer!='L')
	{
		if(pDependEnd->pNode)
		{
			pTtaNode->cPosLayer=pDependEnd->pNode->layer(0);
			pTtaNode->cfgword&=pDependEnd->pNode->UnifiedCfgword();
			if(pDependEnd->pNode->IsLegObj())
				pTtaNode->cfgword=pDependEnd->pNode->cfgword;
		}
		else
		{
			ValidateBraceNodeCfgWord(pDependEnd,bOnlyByDependency);
			if(pDependEnd->cPosLayer=='L')
			{
				pTtaNode->cPosLayer='L';
				pTtaNode->cfgword=pDependEnd->cfgword;
			}
			else
				pTtaNode->cfgword&=pDependEnd->cfgword;
		}
		pTtaNode->bValidateCfgWord=true;
	}
	if(pDepend3!=NULL && pTtaNode->cPosLayer!='L')
	{
		if(pDepend3->pNode)
		{
			pTtaNode->cPosLayer=pDepend3->pNode->layer(0);
			pTtaNode->cfgword&=pDepend3->pNode->UnifiedCfgword();
			if(pDepend3->pNode->IsLegObj())
				pTtaNode->cfgword=pDepend3->pNode->cfgword;
		}
		else
		{
			ValidateBraceNodeCfgWord(pDepend3,bOnlyByDependency);
			if(pDepend3->cPosLayer=='L')
			{
				pTtaNode->cPosLayer='L';
				pTtaNode->cfgword=pDepend3->cfgword;
			}
			else
				pTtaNode->cfgword&=pDepend3->cfgword;
		}
		pTtaNode->bValidateCfgWord=true;
	}
	if(pDepend4!=NULL && pTtaNode->cPosLayer!='L')
	{
		if(pDepend4->pNode)
		{
			pTtaNode->cPosLayer=pDepend4->pNode->layer(0);
			pTtaNode->cfgword&=pDepend4->pNode->UnifiedCfgword();
			if(pDepend4->pNode->IsLegObj())
				pTtaNode->cfgword=pDepend4->pNode->cfgword;
		}
		else
		{
			ValidateBraceNodeCfgWord(pDepend4,bOnlyByDependency);
			if(pDepend4->cPosLayer=='L')
			{
				pTtaNode->cPosLayer='L';
				pTtaNode->cfgword=pDepend4->cfgword;
			}
			else
				pTtaNode->cfgword&=pDepend4->cfgword;
		}
		pTtaNode->bValidateCfgWord=true;
	}
	return pTtaNode->cfgword;
}
bool CTtaDataFile::UpdateTtaNodeCfgWord(TTA_NODE* pTtaNode)
{
	if(IE==0)	//ֻ��һ�����ߵ���
	{
		pTtaNode->cfgword=CFGWORD(1);
		if(pTtaNode->pointI<m_arrBodyLeg[0].Fmin)
			pTtaNode->cPosLayer='S';	//С����С���ȱ�ţ���Ϊ����
		else
			pTtaNode->cPosLayer='L';
		return true;
	}
	//�������ɸ�鵱ǰ�ڵ��Ƿ��������Ӧ����ģ����
	int i,maxFmaxI=m_arrBodyLeg[NBY+IE-1].Fmax;	//ģ�������к��ߵ��������ڵ�ţ�֮����Ϊ���Ľڵ��
	for(i=NBY;i<NBY+IE;i++)
	{
		//pTtaNode=hashTtaNodeI[m_arrBodyLeg[i].Jmin];
		//ɸ�鱾�干�ö�
		int iBody=m_arrBodyLeg[i].LB%10;	//��λ����ʾ��ǰ����ר�в������Խӵı���߶Ⱥ�
		for(int j=0;j<iBody;j++)
		{
			if(  pTtaNode->pointI<=m_arrBodyLeg[j].Fmax ||
				(pTtaNode->pointI>maxFmaxI&&pTtaNode->pointI<=m_arrBodyLeg[j].MAX))
			{
				pTtaNode->cPosLayer='S';
				pTtaNode->cfgword.AddBodyLegs(m_arrBodyLeg[i].pModule->GetBodyNo());
			}
		}
		if( m_arrBodyLeg[i-1].MylBraceNodeMAX>0&&
			pTtaNode->pointI>m_arrBodyLeg[i-1].MylBraceNodeMAX&&pTtaNode->pointI<=m_arrBodyLeg[i].MylBraceNodeMAX)
		{	//������Ӧ�����Ķ˽ڵ�
			//��ǰ���߶�������λ���ȶ��ϵĸ��Ľڵ��ͼ������'S'��'L'����ĺ�ȱ���������壬ֻ��ȡ�����������ڵ� wjh-2018.5.25
			pTtaNode->bFoundationNode=false;	//���ɽڵ�
			pTtaNode->cfgword.AddBodyLegs(m_arrBodyLeg[i].pModule->GetBodyNo());
		}
		if(  pTtaNode->pointI>=m_arrBodyLeg[i].Jmin&&pTtaNode->pointI<=m_arrBodyLeg[i].Fmax||
			(pTtaNode->pointI>maxFmaxI&&pTtaNode->pointI<=m_arrBodyLeg[i].MAX))
		{	//ר���ڵ�ǰ���ߵ��������ȶ�
			//pTtaNode->cPosLayer='L';
			if(pTtaNode->pointI<m_arrBodyLeg[i].Fmin)	//��ǰ�����еĹ��ý��������
			{
				pTtaNode->cPosLayer='S';	//�ⲿ�ֽڵ����Ϊ��ǰ����ר�е������
				pTtaNode->cfgword.AddBodyLegs(m_arrBodyLeg[i].pModule->GetBodyNo());
			}
			/*else
			{	//TODO:Ŀǰ��û�жԸ��Ƚ���ϸ��
				pTtaNode->bFoundationNode=true;		//�����ڵ�
				BODYLEG_I iBodyLeg(m_arrBodyLeg[i].pModule->GetBodyNo(),(WORD)(pTtaNode->pointI/10));
				if(hashBodyLegs.GetValue(iBodyLeg.idBodyLeg)!=NULL)
				{
					DWORD legword=GetSingleWord(hashBodyLegs[iBodyLeg.idBodyLeg]-'A'+1);
					pTtaNode->cfgword.AddBodyLegs(m_arrBodyLeg[i].pModule->GetBodyNo(),legword);
				}
				else
				{
					char cMaxIdleLeg='A';
					for(char* pcLeg=hashBodyLegs.GetFirst();pcLeg;pcLeg=hashBodyLegs.GetNext())
					{
						BODYLEG_I dwBodyLeg(hashBodyLegs.GetCursorKey());
						if(dwBodyLeg.wBody!=iBodyLeg.wBody)
							continue;
						if(cMaxIdleLeg<=*pcLeg)
							cMaxIdleLeg+=1;
					}
					DWORD legword=GetSingleWord(cMaxIdleLeg-'A'+1);
					pTtaNode->cfgword.AddBodyLegs(m_arrBodyLeg[i].pModule->GetBodyNo(),legword);
					m_arrBodyLeg[i].pModule->m_dwLegCfgWord.AddSpecWord(pTtaNode->cfgword);
					hashBodyLegs.SetValue(iBodyLeg.idBodyLeg,cMaxIdleLeg);
				}
			}*/
		}
	}
	return true;
}
//���ݹ�����ȡ�ҵ���Ϣ
void CTtaDataFile::ExtractWireNode()
{
	NODESET wireNodeSet;
	CWorkStatus *pFirstStatus=Ta.WorkStatus.GetFirst();
	if(pFirstStatus && pFirstStatus->hashNodeLoad.GetNodeNum()>0)
	{
		CMinDouble minZ;
		for(CLDSNode* pNode=Ta.EnumNodeFirst();pNode;pNode=Ta.EnumNodeNext())
		{
			if(pFirstStatus->hashNodeLoad.GetValue(pNode->handle)==NULL)
				continue;	//�ǵ��ߺ��عҵ�
			pNode->wireplace.ciWireType='C';
			wireNodeSet.append(pNode);
			minZ.Update(pNode->xOriginalPos.z);
		}
		//����ߵ�ҵ�����Ϊ����
		for(CLDSNode* pNode=wireNodeSet.GetFirst();pNode;pNode=wireNodeSet.GetNext())
		{
			if(fabs(pNode->xOriginalPos.z-minZ.number)<1)
				pNode->wireplace.ciWireType='E';
		}
		//��ʼ���ҵ���Ϣ
		CDesWireNodeDlg dlg;
		dlg.ExtractWireNodeFromTa();
		for(WIRE_NODE* pWireNode=dlg.m_arrWireNode.GetFirst();pWireNode;pWireNode=dlg.m_arrWireNode.GetNext())
		{
			//pWireNode->UpdateNodeName();
			CLDSNode* pNode=console.FromNodeHandle(pWireNode->m_hRelatedObj);
			if(pNode==NULL)
				continue;
			pNode->wireplace = pWireNode->m_xWirePlace;
			pNode->UpdateWireDesc();
		}
	}
}
static bool ParseMylBraceNodeStr(const char* szText,CXhChar50* itemstr1,CXhChar50* itemstr2,CXhChar50* itemstr3,CXhChar50* itemstr4)
{
	CXhChar200 itemtext(szText);
	int indexOfKey=0;
	char* key=strtok(itemtext," ");
	while(key!=NULL)
	{
		if(indexOfKey==0&&itemstr1!=NULL)
			itemstr1->Copy(key);
		else if(indexOfKey==1&&itemstr2!=NULL)
			itemstr2->Copy(key);
		else if(indexOfKey==2&&itemstr3!=NULL)
			itemstr3->Copy(key);
		else if(indexOfKey==3&&itemstr4!=NULL)
		{
			itemstr4->Copy(key);
			indexOfKey++;
			break;
		}
		indexOfKey++;
		key=strtok(NULL," ");
	}
	if(indexOfKey<4)
	{
		itemtext.Copy(szText);
		char* iterstr=(char*)itemtext;
		if(itemstr1)
			itemstr1->NCopy(&iterstr,11,true);
		if(itemstr2)
			itemstr2->NCopy(&iterstr,11,true);
		if(itemstr3)
			itemstr3->NCopy(&iterstr,11,true);
		if(itemstr4)
			itemstr4->NCopy(&iterstr,11,true);
	}
	return true;
}
static bool ParseMylBraceElemLabelStr(const char* szText,CXhChar50* itemstr1,CXhChar50* itemstr2)
{
	CXhChar200 itemtext(szText);
	int indexOfKey=0;
	char* key=strtok(itemtext," ");
	while(key!=NULL)
	{
		if(indexOfKey==0&&itemstr1!=NULL)
			itemstr1->Copy(key);
		else if(indexOfKey==1&&itemstr2!=NULL)
		{
			itemstr2->Copy(key);
			indexOfKey++;
			break;
		}
		indexOfKey++;
		key=strtok(NULL," ");
	}
	if(indexOfKey<2||key-(char*)itemtext>20)
	{
		itemtext.Copy(szText);
		char* iterstr=(char*)itemtext;
		if(itemstr1)
			itemstr1->NCopy(&iterstr,7,true);
		if(itemstr2)
			itemstr2->NCopy(&iterstr,7,true);
	}
	return true;
}

//����0��ʾ������Ժ��ʽ;1��ʾ������Ժ��ʽ
int CTtaDataFile::JustifyCtrlInfoFormatType(BYTE ciFileType,CXhChar50* pxarrWords1,int count1,CXhChar50* pxarrWords2,int count2)
{
	if (ciFileType==FILETYPE_GTOM)
	{	//��һ�п�����Ϣ
		KIND=atoi(pxarrWords1[0]);
		N10 =atoi(pxarrWords1[1]);
		N20 =atoi(pxarrWords1[2]);
		M10 =atoi(pxarrWords1[3]);
		M20 =atoi(pxarrWords1[4]);
		IE  =atoi(pxarrWords1[5]);
		NL  =atoi(pxarrWords1[6]);
		NC  =atoi(pxarrWords1[7]);
		NW  =atoi(pxarrWords1[8]);
		NSEC=atoi(pxarrWords1[9]);
		Wk  =count1>10?atof(pxarrWords1[10]):1e50;
		//�ڶ��п�����Ϣ
		K1  =atoi(pxarrWords2[0]);
		K2  =atoi(pxarrWords2[1]);
		K3  =atoi(pxarrWords2[2]);
		K4  =atoi(pxarrWords2[3]);
		K5  =atoi(pxarrWords2[4]);
		K6  =atoi(pxarrWords2[5]);
		NA  =atoi(pxarrWords2[6]);
		TSM =atoi(pxarrWords2[7]);
		REN =atoi(pxarrWords2[8]);
		//NGP =atoi(pxarrWords2[9]);	//������ظ˼��ظ�����ģʽ
		//CODE=atoi(pxarrWords2[10]);	//�淶�汾
		LOAD_GROUP=NGP=count2>11?atoi(pxarrWords2[11]):0;
		return 0;
	}
	else if(ciFileType!=2)
	{	//��һ�п�����Ϣ
		KIND=atoi(pxarrWords1[0]);
		N10 =atoi(pxarrWords1[1]);
		N20 =atoi(pxarrWords1[2]);
		M10 =atoi(pxarrWords1[3]);
		M20 =atoi(pxarrWords1[4]);
		IE  =atoi(pxarrWords1[5]);
		NL  =atoi(pxarrWords1[6]);
		NC  =atoi(pxarrWords1[7]);
		NW  =atoi(pxarrWords1[8]);
		NSEC=atoi(pxarrWords1[9]);
		//�ڶ��п�����Ϣ
		K1  =atoi(pxarrWords2[0]);
		K2  =atoi(pxarrWords2[1]);
		K3  =atoi(pxarrWords2[2]);
		K4  =atoi(pxarrWords2[3]);
		K5  =atoi(pxarrWords2[4]);
		K6  =atoi(pxarrWords2[5]);
		NA  =atoi(pxarrWords2[6]);
		TSM =atoi(pxarrWords2[7]);
		REN =atoi(pxarrWords2[8]);
		NGP =atoi(pxarrWords2[9]);
		return 0;
	}
	bool bTtaFormat=true;
	while(true)
	{	//������Ժ��ʽ����
		//��һ�п�����Ϣ
		KIND=atoi(pxarrWords1[0]);
		if((KIND =atoi(pxarrWords1[0]))<0)
		{	//������Ժ��ʽ֧��<0��ʾ���������ṹ���ع淶��
			bTtaFormat=false;
			break;
		}
		N10 =atoi(pxarrWords1[1]);
		N20 = abs(atoi(pxarrWords1[2]));	//����������ö���Ժ��ʽ��N20<0��� wjh
		/*if((N20 =atoi(pxarrWords1[2]))<0)
		{	//������Ժ��ʽ֧�֡����ȷֵ�ڵ㡢�ȳ��߶νڵ㡢�����ཻ�������ཻ"���½ڵ㶨�巽ʽ
			bTtaFormat=false;
			break;
		}*/
		M10 =atoi(pxarrWords1[3]);
		M20 =atoi(pxarrWords1[4]);
		IE  =atoi(pxarrWords1[5]);
		if((NL  =atoi(pxarrWords1[6]))<0)
		{
			bTtaFormat=false;
			break;
		}
		if((NC  =atoi(pxarrWords1[7]))<0)
		{
			bTtaFormat=false;
			break;
		}
		NW  =atoi(pxarrWords1[8]);	//�ٶ�����(��λ�Ƶ�Ч����)���ýڵ����
		NSEC=atoi(pxarrWords1[9]);
		ibolt.ParseFromDword(NSEC);
		WIND=atoi(pxarrWords1[10]);
		//�ڶ��п�����Ϣ
		K1  =atoi(pxarrWords2[0]);
		K2  =atoi(pxarrWords2[1]);
		K3  =atoi(pxarrWords2[2]);
		K4  =atoi(pxarrWords2[3]);
		K5  =atoi(pxarrWords2[4]);
		K6  =atoi(pxarrWords2[5]);
		NA  =atoi(pxarrWords2[6]);
		TSM =atoi(pxarrWords2[7]);
		REN =atoi(pxarrWords2[8]);
		NGP =atoi(pxarrWords2[9]);
		if (NGP >= 10&&NGP<100)	//����ԺNGPȡֵ1~6�������˷�Χ���ܱ�ʾ����ԺIVER,��ʾԭʼ���ݽ���ʱ��Ӧ�ĳ���汾�ţ���30����ʾV3.0�� wjh-2019.7.8
		{
			bTtaFormat = false;
			break;
		}
		VAL2=atoi(pxarrWords2[10]);
		return 0;
	}
	if(bTtaFormat)
		return 0;
	else
	{	//������Ժ��ʽ����
		//��һ�п�����Ϣ
		KIND=atoi(pxarrWords1[0]);	//������Ժ��ʽ֧��<0��ʾ���������ṹ���ع淶��
		N10 =atoi(pxarrWords1[1]);
		N20 =abs(atoi(pxarrWords1[2]));	//������Ժ��ʽ֧�֡����ȷֵ�ڵ㡢�ȳ��߶νڵ㡢�����ཻ�������ཻ"���½ڵ㶨�巽ʽ
		M10 =atoi(pxarrWords1[3]);
		M20 =atoi(pxarrWords1[4]);
		IE  =atoi(pxarrWords1[5]);
		NL  =abs(atoi(pxarrWords1[6]));
		NC  =abs(atoi(pxarrWords1[7]));
		NW  =atoi(pxarrWords1[8]);	//�ٶ�����(��λ�Ƶ�Ч����)���ýڵ����
		NSEC=atoi(pxarrWords1[9]);
		ibolt.ParseFromDword(NSEC);
		WIND=atoi(pxarrWords1[10]);
		//�ڶ��п�����Ϣ
		K1  =atoi(pxarrWords2[0]);
		K2  =atoi(pxarrWords2[1]);
		K3  =atoi(pxarrWords2[2]);
		K4  =atoi(pxarrWords2[3]);
		K6  =atoi(pxarrWords2[4]);	//K5
		NA  =atoi(pxarrWords2[5]);	//K6
		TSM =atoi(pxarrWords2[6]);	//NA
		NGP =atoi(pxarrWords2[7]);	//TSM
		int NSE,IVER,ELM;
		NSE =atoi(pxarrWords2[8]);	//REN	//�ɶ����ɶ���
		IVER=atoi(pxarrWords2[9]);	//NGP	//�汾��
		ELM =atoi(pxarrWords2[10]);	//VAL2	//�����Ʊ�־��
		return 1;
	}
	return 0;
}
static bool ParseRodSizeSpec(const char* sizestr, SIZESPEC& size, char ciFormatType = 1)
{
	CXhChar50 szSizeStr = sizestr;
	char* pIterKey = NULL;
	if (pIterKey = strstr(szSizeStr, "��"))
	{
		size.idClassType = CLS_LINETUBE;
		pIterKey += 2;
	}
	else if (pIterKey = strstr(szSizeStr, "��"))
	{
		size.idClassType = CLS_LINETUBE;
		pIterKey += 2;
	}
	else if (pIterKey = strchr(szSizeStr, 'L'))
	{
		size.idClassType = CLS_LINEANGLE;
		CXhChar16 sPrefix;
		sPrefix.NCopy(sizestr, pIterKey - (char*)szSizeStr);
		sPrefix.Remove('x');
		sPrefix.Remove('X');
		int number = atoi(sPrefix);
		if (number > 1)
		{
			size.idClassType = CLS_GROUPLINEANGLE;
			size.cSubClassType = number == 4 ? 'X' : 'D';
		}
		pIterKey += 1;
	}
	else
		return false;
	restore_JG_guige(pIterKey, size.wide, size.thick);
	return true;
}
bool TestValidGTowerBodyFzcFileName(const char* _fzcfname,GTM_FZC_FNAME *pFzcFile=NULL)
{	//�б��ļ������Ƿ���Ϲ㶫ԺGTower������������ļ�������������
	char fzcfname[500]="";
	StrCopy(fzcfname,_fzcfname,500);
	char* pKeyHeader=fzcfname;
	CXhChar50 xarrPointLabels[2];
	int labelI=0;
	for(char* pchIter=fzcfname;*pchIter!=0&&pchIter-fzcfname<200;pchIter++)
	{
		if(*pchIter!='-'&&*pchIter!='_'&&*pchIter!='=')
			continue;
		*pchIter=0;
		int nstr=pchIter-pKeyHeader;
		if(nstr<2)
			continue;
		xarrPointLabels[labelI].NCopy(pKeyHeader,nstr);
		pKeyHeader=pchIter+1;
		labelI++;
		if(labelI>=2)
			break;
	}
	long liPointA=atoi(xarrPointLabels[0]);
	long liPointB=atoi(xarrPointLabels[1]);

	if(pFzcFile)
	{
		pFzcFile->liPointA=liPointA;
		pFzcFile->liPointB=liPointB;
	}
	return liPointA>=10&&liPointB>=10&&liPointA!=liPointB;
}
bool TestValidGTowerLegFzcFileName(const char* _fzcfname,GTM_FZC_FNAME *pFzcFile=NULL)
{	//�б��ļ������Ƿ���Ϲ㶫ԺGTower��������ȸ����ļ�������������
	char fzcfname[500]="";
	StrCopy(fzcfname,_fzcfname,500);
	char* pKeyHeader=NULL,*pNextKeyHeader=fzcfname;
	CXhChar50 xarrPointLabels[2];
	int labelI=-1;
	pNextKeyHeader=fzcfname;
	for(char* pchIter=fzcfname;*pchIter!=0&&pchIter-fzcfname<200;pchIter++)
	{
		if(labelI<=-1&&*pchIter!='=')
			continue;
		else if(labelI>=0&&*pchIter!='-'&&*pchIter!='_')
			continue;
		pKeyHeader=pNextKeyHeader;
		*pchIter=0;
		pNextKeyHeader=pchIter+1;
		int nstr=pchIter-pKeyHeader;
		if(nstr<2)
			continue;
		if(labelI==-1)
		{
			if(strstr(pKeyHeader,"V_Leg")!=NULL)
			{	//ȷ�ϵ�ǰΪ���ȵ�V�渨��
				labelI=0;
				pFzcFile->ciFzcPanelType=2;
				continue;
			}
			else if(strstr(pKeyHeader,"Leg")!=NULL)
			{	//ȷ�ϵ�ǰΪ���ȵ����ศ��
				labelI=0;
				pFzcFile->ciFzcPanelType=1;
				continue;
			}
			else
				continue;
		}
		xarrPointLabels[labelI].NCopy(pKeyHeader,nstr);
		labelI++;
		if(labelI>=2)
			break;
	}
	if (labelI==1&&*pNextKeyHeader!=0)
		xarrPointLabels[1].Copy(pNextKeyHeader);
	long liHeight =ftoi(atof(xarrPointLabels[0])*10);
	long liLegDiff=ftoi(atoi(xarrPointLabels[1])*10);
	CLDSModule* pHeight=NULL;
	for(pHeight=Ta.Module.GetFirst();pHeight!=NULL;pHeight=Ta.Module.GetNext())
	{
		if(fabs(pHeight->m_fNamedHeight-liHeight*0.1)<0.1)
			 break;
	}
	if (pHeight==NULL)
	{
		logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%s�����ļ�û��ƥ���Ϻ��ʵĺ���%g",_fzcfname,liHeight*0.1);
		return false;
	}
	if(pFzcFile)
	{
		pFzcFile->subleg.liHeight=liHeight;
		pFzcFile->subleg.liLegDiff=liLegDiff;
	}
	return liHeight>0;
}
void CTtaDataFile::ReadDataFile(FILE* fp,BYTE ciFileType/*=0*/,const char* szDataFilePathName/*=NULL*/)
{
	int i,j;
	Ta.Empty();
	console.DispNodeSet.Empty();
	console.DispPartSet.Empty();
	console.DispBlockRefSet.Empty();
	console.DispFoundationSet.Empty();
	listIntersNodes.Empty();
	m_ciFileType=ciFileType;
	FILE* fpTmp=NULL;
	CXhChar200 strDataFilePathName=theApp.GetLDSDoc()->GetPathName();
	if(szDataFilePathName==NULL)
		szDataFilePathName=strDataFilePathName;
	//һ��������Ϣ����
	char sText[201]="";

	ReadLineText(sText,100,fp);
	Ta.m_sPrjCode=sText;
	ReadLineText(sText,50,fp);
	strcpy(Ta.m_sPrjName,sText);
	ReadLineText(sText,50,fp);
	strcpy(Ta.m_sOperator,sText);

	CXhChar100 lineText;
	//����������Ϣ����
	//KIND  N10	N20	M10	M20	IE	NL	NC	NW	NSEC  | WIND
	//��K1  K2	K3	K4	K5	K6	NA	TSM	REN	��NGP | CODE LoadGroup
	//A�㶫ԺGTower��ʽ���� 
	//A.1 NWIND(��ӦԭNGPռλ)==������ص����㷽����С�ڵ���5��ʾ�۳��ظ�Ӱ�죬����5��ʾ���۳��˼��ظ��� 
	//A.2 Wk=���Եػ�����(kN/m^3)��������������Ԫ����ʱ�����ã����ҽ������Ըն�Ϊ��ֱ
	//����ģ��Ե�Ԫ���Ϊ3�ĵ�Ԫ�����ã���������ѹ����һ��Ҫѹ��������Ľ������ȷ��
	//LoadGroup=���ط�������
	ReadLineText(sText,101,fp);	//��ֱ����fgets���Ա�������Ŀհ��ַ����ŵ����ж�ȡ����
	PRESET_ARRAY16<CXhChar50> xarrWords1,xarrWords2;
	if (ciFileType==FILETYPE_GTOM)
	{	//�㶫Ժ������Ϣ�ǰ�ÿ��10��������ÿ�������洢��ʽ��ΪI4
		int index=0,len=strlen(sText);
		if (sText[len-1]=='\n')
		{
			len--;
			sText[len]=0;
		}
		for (i=0;i<len;i+=4,index++)
			xarrWords1[index].NCopy(&sText[i],4);
		xarrWords1.SetSize(index);
		ReadLineText(sText,101,fp);
		if (sText[len-1]=='\n')
		{
			len--;
			sText[len]=0;
		}
		index=0,len=strlen(sText);
		for (i=0;i<len;i+=4,index++)
			xarrWords2[index].NCopy(&sText[i],4);
		xarrWords2.SetSize(index);
	}
	else
	{
		int count1=DELIMITER_STR::ParseWordsFromStr(sText," ",xarrWords1.pPresetObjs,16);
		ReadLineText(sText,101,fp);
		int count2=DELIMITER_STR::ParseWordsFromStr(sText," ",xarrWords2.pPresetObjs,16);
		xarrWords1.ReSize(count1);
		xarrWords2.ReSize(count2);
	}
	int ciFormatType=JustifyCtrlInfoFormatType(ciFileType,xarrWords1.pPresetObjs,16,xarrWords2.pPresetObjs,16);
	if (ciFileType == FILETYPE_TANS && (KIND % 10 == 6 || KIND % 10 == 9))
	{
		CXhChar200 szIniFile = szDataFilePathName;
		char* szExt = SearchChar(szIniFile, '.', true);
		if (szExt&&stricmp(szExt,".tans")==0)
			*szExt = 0;
		szIniFile.Append("_Data.ini");
		FILE* fpini = fopen(szIniFile, "rt");
		if (fpini == NULL)
		{
			AfxMessageBox(CXhChar50("δ�ҵ�%s�ļ�", (char*)szIniFile));
			return;
		}
		ReadTansNewFormatIniFile(fpini);
		fclose(fpini);
	}
	ClassifyRodSizeSpecs(KIND%10,ciFileType);
	//�����ڵ�����
	NBY=IE/100;
	IE%=100;
	Ta.Module.Empty();
	CLDSModule *pModule=console.InitTowerModel();
	pModule->m_dwLegCfgWord.Clear();
	Ta.m_hActiveModule=pModule->handle;
	if(NBY==0&&IE==0)
		NBY=1;	//������һ����¼
	int iLegNo=NBY+1;
	CObjNoGroup *pGroup=Ta.NoManager.FromGroupID(MODEL_GROUP);
	pGroup->EmptyNo();
	DisplayProcess(10,"�������ģ�ͽڵ���Ϣ");
	//m_arrBodyLeg.SetSize(0,NBY+IE+2);
	for(i=0;i<NBY+IE;i++)
	{
		TTA_BODYLEG *pBodyLeg=m_arrBodyLeg.append();
		pBodyLeg->ApplyListTtaNodeSet(this);
		ReadLineText(sText,100,fp);
		//////////////////////////////////////////////////////////////
		//���´����Ǳ���ԭ���������г�����������һ�����������磺
		// 1660 1660 166319993    0    0      19993
		// 2660 3010 301335993  111    0    1335993
		char *key_prev,*key=sText;
		key_prev=key;
		for(int ik=0;ik<3;ik++){
			while(!SafeIsDigit(*key)) {key++;}
			if((key=SearchChar(key,' '))==NULL)
				break;
			if(ik==2&&key!=NULL&&key_prev!=NULL&&key-key_prev>6)
			{
				CXhString xhstr(key_prev,101-(key_prev-sText));
				xhstr.InsertBefore(' ',key-5-key_prev);
			}
			key_prev=key;
		}
		//////////////////////////////////////////////////////////////

		key=strtok(sText," ");
		for(j=0;key!=NULL;j++)
		{
			if(j==0)
				pBodyLeg->Jmin=atoi(key);
			else if(j==1)
				pBodyLeg->Fmin=atoi(key);
			else if(j==2)
				pBodyLeg->Fmax=atoi(key);
			else if(j==3)
				pBodyLeg->MAX=atoi(key);
			else if(j==4)
			{
				int nstr=strlen(key);
				pBodyLeg->LB=key[nstr-1]-'0';
			}
			else if(j==5)
				pBodyLeg->Var=atoi(key);
			else //if(i==6)
				pBodyLeg->MylBraceNodeMAX=atoi(key);
			key=strtok(NULL," ");
		}
		//��������������������10(��)ʱ��LB��Ϣ���ܸ�ʽΪ1A4����ʱ���������´���ֱ��ת�� wjh-2018.6.27
		//sscanf(sText,"%d%d%d%d%d%d%d",&pBodyLeg->Jmin,&pBodyLeg->Fmin,&pBodyLeg->Fmax,
		//	&pBodyLeg->MAX,&pBodyLeg->LB,&pBodyLeg->Var,&pBodyLeg->MylBraceNodeMAX);
		pBodyLeg->iLeg=0;
		pBodyLeg->pModule=NULL;
		pBodyLeg->LB%=100;	//����������Ϣ����
		if(i<NBY)
			continue;
		if(i>=NBY)
		{	//TTA�����飬��LDS�еĺ���ģ�ͷ���
			int iBody=i-NBY;
			if(iBody==0)
			{
				pBodyLeg->pModule=pModule;
				pBodyLeg->pModule->SetBodyNo(1);
				pGroup->SetNoState(1);
				pBodyLeg->pModule->m_dwLegCfgWord.Clear();
				pBodyLeg->pModule->m_dwLegCfgWord.AddBodyLegs(pModule->GetBodyNo(),1);
			}
			else
			{
				pBodyLeg->pModule=Ta.Module.append();
				pBodyLeg->pModule->iNo=pGroup->EnumIdleNo();
				pBodyLeg->pModule->SetBodyNo((WORD)pBodyLeg->pModule->iNo);
				pGroup->SetNoState(pBodyLeg->pModule->iNo);
				sprintf(pBodyLeg->pModule->description,"������%d",iBody+1);
				pBodyLeg->pModule->SetBodyNo(iBody+1);
				pBodyLeg->pModule->m_dwLegCfgWord.Clear();
				pBodyLeg->pModule->m_dwLegCfgWord.AddBodyLegs(pBodyLeg->pModule->GetBodyNo(),1);
				//Ĭ��A��Ϊÿ�����ߵ�Ĭ�ϼ������
				pBodyLeg->pModule->m_arrActiveQuadLegNo[0]=pBodyLeg->pModule->m_arrActiveQuadLegNo[1]=
					pBodyLeg->pModule->m_arrActiveQuadLegNo[2]=pBodyLeg->pModule->m_arrActiveQuadLegNo[3]=CFGLEG::MaxLegs()*iBody+1;
			}

			 //pBodyLeg->iLeg=iLegNo;
			 //int nLeg=max(1,pBodyLeg->LB/10);//ʰλ����ʾ�������ȸ��ȵĸ߶���
			 //int iModule=pBodyLeg->LB%10;	//��λ��ʾ���弴�߶Ⱥ�
			 //if(iModule>0)
			 //	pModule=Ta.Module.GetByIndex(iModule-1);
			 //else
			 //	pModule=Ta.Module.GetByIndex(iModule);
			 //for(j=0;j<nLeg;j++)
			 //	pModule->m_dwLegCfgWord.AddSpecNoWord(iLegNo+j);
			 //iLegNo+=nLeg;
		}
	}
	//�ġ����ط�������
		//�ݲ�֧�ֺ��ط���,����
	int M;
	if(ciFileType==FILETYPE_GTOM)
		M=NGP;
	else if (abs(NGP)>100)	//Ŀǰ�Ѽ�����SmartTower���ݷ�����1010�������ʱ����10�ֺ��ط��� wjh-2019.9.26
		M=abs(NGP)%100;
	else
		M=abs(NGP)%10;
	if(M>1||(M==1&&ciFileType==FILETYPE_GTOM))
	{	//�㶫Ժ��GTower������NGP=1ʱ���ʾ���ڴ�������
		for(i=0;i<M;i++)
			ReadLineText(sText,100,fp);
	}
	//�塢��һ�ֽڵ����뷽ʽ����
	TANS_LINE* pLine;
	TANS_PLANE* pPlane;
	TTA_NODE* pTtaNode,*pMirTtaNode;
	m_arrTtaNode.Empty();
	DYN_ARRAY<TTA_NODE*>primaryNodes(N10);
	int nMaxLegsPerBody=0;
	hashTansLegBraceNodes.Empty();
	CXhChar50 xarrItems[8];
	int nItemCount = DELIMITER_STR::ParseWordsFromStr(sText, ", \t", xarrItems, 8);
	for(i=0;i<N10;i++)
	{
		TTA_NODE xTempObj;
		ReadLineText(sText,100,fp);
		sscanf(sText,"%d%d%lf%lf%lf",&xTempObj.pointI,&xTempObj.IS,
			&xTempObj.pos.x,&xTempObj.pos.y,&xTempObj.pos.z);
		if(xTempObj.pointI<0&&ciFileType==FILETYPE_TANS)
		{
			long hRefPointI = nItemCount > 5 ? atoi(xarrItems[5]) : abs(xTempObj.pointI);
			hashTansLegBraceNodes.SetValue(abs(xTempObj.pointI), hRefPointI);
		}
		xTempObj.pointI = abs(xTempObj.pointI);
		primaryNodes[i]=NULL;
		for(int j=NBY;j<NBY+IE;j++)
		{
			if(xTempObj.pointI>=m_arrBodyLeg[j].Fmin&&xTempObj.pointI<=m_arrBodyLeg[j].Fmax)
			{
				m_arrBodyLeg[j].niLegN++;
				nMaxLegsPerBody=max(nMaxLegsPerBody,m_arrBodyLeg[j].niLegN);
				break;
			}
		}
		if(xTempObj.pointI/10000==7)
		{	//TANS����Ļ�׼��
			pLine=hashLines.Add(xTempObj.pointI);
			if(xTempObj.pos.x>10000)
				pLine->iStartPointI=ftoi(xTempObj.pos.x)%10000;
			if(xTempObj.pos.y>10000&&pLine->iStartPointI==0)
				pLine->iStartPointI=ftoi(xTempObj.pos.y)%10000;
			else if(xTempObj.pos.y>10000&&pLine->iStartPointI==0)
				pLine->iEndPointI=ftoi(xTempObj.pos.y)%10000;
			pLine->iEndPointI=ftoi(xTempObj.pos.z)%10000;
			continue;
		}
		else if(xTempObj.pointI/10000==9)
		{	//TANS����Ļ�׼��
			pPlane=hashPlanes.Add(xTempObj.pointI);
			pPlane->iPoint1stI=ftoi(xTempObj.pos.x)%10000;
			pPlane->iPoint2ndI=ftoi(xTempObj.pos.y)%10000;
			pPlane->iPoint3rdI=ftoi(xTempObj.pos.z)%10000;
			continue;
		}
		MIRFLAG* pMirFlag=GetMirFlag(xTempObj.pointI);
		if(pMirFlag==NULL)
			pMirFlag = AddMirFlag(xTempObj.pointI);
		if(xTempObj.IS==1)
		{	//ͬ���Žڵ�ڶ��м�¼����Y�ᣨ���ң��Գ�ʱ��Ӧ�ر�ͬ����X��ԳƼ��ԽǶԳ�
			pMirFlag->EnableMirX(false);
			pMirFlag->EnableMirZ(false);
		}
		else if(xTempObj.IS==2)
		{	//ͬ���Žڵ�ڶ��м�¼����X�ᣨǰ�󣩶Գ�ʱ��Ӧ�ر�ͬ����Y��ԳƼ��ԽǶԳ�
			pMirFlag->EnableMirY(false);
			pMirFlag->EnableMirZ(false);
		}
		else if(xTempObj.IS==3)
		{	//ͬ���Žڵ�ڶ��м�¼����Z�ᣨ�Խǣ��Գ�ʱ��Ӧ�ر�ͬ����Z��ԳƼ��ԽǶԳ�
			pMirFlag->EnableMirX(false);
			pMirFlag->EnableMirY(false);
		}
		primaryNodes[i]=pTtaNode=listTtaNodes.append();
		*primaryNodes[i]=xTempObj;
		hashParentTtaNodeI.SetValue(pTtaNode->pointI,true);
		hashTtaNodeI.SetValue(pTtaNode->pointI,pTtaNode);
		if(pTtaNode->IS==1)	//Y��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(2));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,2);	//��ֵIS�Ա���������ԶԳƽڵ㻹�ǶԳƽڵ�
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
		else if(pTtaNode->IS==2)	//X��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(1));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,1);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
		else if(pTtaNode->IS==3)	//Z��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(3));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,3);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
		else if(pTtaNode->IS==4)	//XYZ��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(1));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,1);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(2));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,2);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(3));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,3);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
	}
	//�����ڶ��ֽڵ����뷽ʽ����(�����ڵ�һ����������ǰ���룬��Ϊ����ԺTANS�����е�һ����п������õڶ���㣩 wjh-2018.6.2
	DYN_ARRAY<TTA_NODE*>secondaryNodes(N20);
	for(i=0;i<N20;i++)
	{
		secondaryNodes[i]=pTtaNode=listTtaNodes.append();
		//if(m_arrTtaNodePtr2[i]->posType<4)
		//	continue;	//��һ��ڵ�
		ReadLineText(sText,100,fp);
		CXhChar200 key_str(sText);
		int nUnit = DELIMITER_STR::ParseWordsFromStr(sText, ", \t", xarrItems, 8);
		//for(char* sKey=strtok(key_str," ");sKey;sKey=strtok(NULL," "))
		//	nUnit++;
		if(nUnit==5||ciFileType!=2)
		{	//ԭʼ����㣨�����ȷֵ㣩
			double R=0;
			//sscanf(sText,"%d%d%d%lf%d",&pTtaNode->pointI,&pTtaNode->J1,&pTtaNode->J2,&R,&pTtaNode->IS);
			pTtaNode->pointI = atoi(xarrItems[0]);
			pTtaNode->J1 = atoi(xarrItems[1]);
			pTtaNode->J2 = atoi(xarrItems[2]);
			R = atof(xarrItems[3]);
			pTtaNode->IS = atoi(xarrItems[4]);
			if (pTtaNode->pointI < 0 && ciFileType == FILETYPE_TANS)
			{
				long hRefPointI = nUnit>5?atoi(xarrItems[5]):abs(pTtaNode->pointI);
				hashTansLegBraceNodes.SetValue(abs(pTtaNode->pointI), hRefPointI);
			}
			pTtaNode->pointI = abs(pTtaNode->pointI);
			pTtaNode->posType=4;	//������
			if(R<=1.0)	//С��
				pTtaNode->coefScale=R;
			else
				pTtaNode->coefScale=1.0/R;
		}
		else if(nUnit==7)
		{	//����չ�Ķ����
			long J1=0,J2=0,J3=0,J4=0,J5=0;
			sscanf(sText,"%d%d%d%d%d%d%d",&pTtaNode->pointI,&J1,&J2,&J3,&J4,&J5,&pTtaNode->IS);
			if(pTtaNode->pointI<0&&ciFileType==FILETYPE_TANS)
				hashTansLegBraceNodes.SetValue(abs(pTtaNode->pointI),abs(pTtaNode->pointI));
			pTtaNode->pointI = abs(pTtaNode->pointI);
			if(J1>10000)
				pTtaNode->J1=J1%10000;
			if(J2>10000)
				pTtaNode->J2=J2%10000;
			if(J3>10000&&J4>10000&&J5>10000)
			{	
				//��ӻ�׼��
				pPlane=hashPlanes.Add(0);
				pPlane->iPoint1stI=J3%10000;
				pPlane->iPoint2ndI=J4%10000;
				pPlane->iPoint3rdI=J5%10000;
				//��׼���ϵ�
				pTtaNode->posType=8;
				pTtaNode->J3=pPlane->idPlane;
			}
			else if(J3>10000&&J4>10000&&J5==0)
			{	//�����
				if(ciFileType==2&&J3/10000==2&&J4/10000==2)
				{
					pTtaNode->posType=7;
					pTtaNode->ciPosViceType='L';
				}
				else
					pTtaNode->posType=5;
				pTtaNode->J3=J3%10000;
				pTtaNode->J4=J4%10000;
			}
			//�˴�����ƫ������J3,J4,J5�п���Ϊ��ֵ���ʲ�����J3>0 wjh-2018.10.9
			else if(ciFileType==2&&J3!=0&&J4==0&&J5==0)
			{
				pTtaNode->posType=7;
				pTtaNode->ciPosViceType='X';
				pTtaNode->offsetDist=J3;
			}
			else if(ciFileType==2&&J3==0&&J4!=0&&J5==0)
			{
				pTtaNode->posType=7;
				pTtaNode->ciPosViceType='Y';
				pTtaNode->offsetDist=J4;
			}
			else if(ciFileType==2&&J3==0&&J4==0&&J5!=0)
			{
				pTtaNode->posType=7;
				pTtaNode->ciPosViceType='Z';
				pTtaNode->offsetDist=J5;
			}
			else if(J3<1000&&J4<1000&&J5==0)
			{	//�����ȷֵ�
				pTtaNode->posType=4;	//������
				double fV1=J3,fV2=(J3+J4);
				pTtaNode->coefScale=fV1/fV2;
			}
		}
		else
			continue;
		for(int j=NBY;j<NBY+IE;j++)
		{
			if(pTtaNode->pointI>=m_arrBodyLeg[j].Fmin&&pTtaNode->pointI<=m_arrBodyLeg[j].Fmax)
			{
				m_arrBodyLeg[j].niLegN++;
				nMaxLegsPerBody=max(nMaxLegsPerBody,m_arrBodyLeg[j].niLegN);
				break;
			}
		}
		MIRFLAG* pMirFlag=GetMirFlag(pTtaNode->pointI);
		if(pMirFlag==NULL)
			pMirFlag = AddMirFlag(pTtaNode->pointI);
		if(pTtaNode->IS==1)
		{	//ͬ���Žڵ�ڶ��м�¼����Y�ᣨ���ң��Գ�ʱ��Ӧ�ر�ͬ����X��ԳƼ��ԽǶԳ�
			pMirFlag->EnableMirX(false);
			pMirFlag->EnableMirZ(false);
		}
		else if(pTtaNode->IS==2)
		{	//ͬ���Žڵ�ڶ��м�¼����X�ᣨǰ�󣩶Գ�ʱ��Ӧ�ر�ͬ����Y��ԳƼ��ԽǶԳ�
			pMirFlag->EnableMirY(false);
			pMirFlag->EnableMirZ(false);
		}
		else if(pTtaNode->IS==3)
		{	//ͬ���Žڵ�ڶ��м�¼����Z�ᣨ�Խǣ��Գ�ʱ��Ӧ�ر�ͬ����Z��ԳƼ��ԽǶԳ�
			pMirFlag->EnableMirX(false);
			pMirFlag->EnableMirY(false);
		}
		hashParentTtaNodeI.SetValue(pTtaNode->pointI,true);
		hashTtaNodeI.SetValue(pTtaNode->pointI,pTtaNode);
		if(pTtaNode->IS==1)	//Y��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(2));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,2);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
		else if(pTtaNode->IS==2)	//Y��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(1));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,1);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
		else if(pTtaNode->IS==3)	//Z��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(3));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,3);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
		else if(pTtaNode->IS==4)	//XYZ��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(1));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,1);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(2));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,2);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(3));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,3);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
	}
	//�������Ҳ���ܴ��ڻ����ڵ㣬�����������ԭ��һ���֮���Ƶ����ڵĶ����֮�� wjh -2018.8.6
	bool legschema_modified=false;
	long niModuleN=Ta.Module.GetNodeNum();
	bool blMultilegOverflow=false;
	if(nMaxLegsPerBody<=8||niModuleN>12)
	{
		if(CFGLEG::MaxLegs()!=8)
		{
			CFGLEG::SetSchema(CFGLEG::MULTILEG_MAX08);
			legschema_modified=true;
		}
		if (nMaxLegsPerBody>CFGLEG::MaxLegs())
			blMultilegOverflow=true;
	}
	else if(((nMaxLegsPerBody>8&&nMaxLegsPerBody<=16)||niModuleN>8)&&CFGLEG::MaxLegs()!=12)
	{
		CFGLEG::SetSchema(CFGLEG::MULTILEG_MAX16);
		legschema_modified=true;
		if (nMaxLegsPerBody>CFGLEG::MaxLegs())
			blMultilegOverflow=true;
	}
	else if(nMaxLegsPerBody>16&&nMaxLegsPerBody<=24&&CFGLEG::MaxLegs()!=24)
	{
		CFGLEG::SetSchema(CFGLEG::MULTILEG_MAX24);
		legschema_modified=true;
		if (nMaxLegsPerBody>CFGLEG::MaxLegs())
			blMultilegOverflow=true;
	}
	if (blMultilegOverflow)
	{
		CFGLEG::SetSchema(CFGLEG::MULTILEG_UDF);
		legschema_modified=true;
		CFGLEG::xUdfSchema.cnHeightCount=(BYTE)niModuleN;
		CXhChar50 szLimit;
		if (niModuleN<=8)
			szLimit="8x24��";
		else if(niModuleN<=12)
			szLimit="12x16��";
		else //if(niModuleN<=24)
			szLimit="24x8��";
		if (niModuleN*nMaxLegsPerBody<192)
		{
			for (i=0;i<niModuleN;i++)
			{
				CFGLEG::xUdfSchema.xarrHeights[i].ciStartAddr=i*nMaxLegsPerBody;
				CFGLEG::xUdfSchema.xarrHeights[i].cnBodyLegs=nMaxLegsPerBody;
			}
		}
		else
		{
			int indicator=0;
			for (i=0;i<niModuleN;i++)
			{
				CFGLEG::xUdfSchema.xarrHeights[i].ciStartAddr=indicator;
				CFGLEG::xUdfSchema.xarrHeights[i].cnBodyLegs=m_arrBodyLeg[j+NBY].niLegN;
				indicator+=m_arrBodyLeg[j+NBY].niLegN;
			}
		}
		logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"��ǰ����ģ���е�[�������x��������(%dx%d)]�����������(%s),ϵͳ���Զ���չ��������������Ӻ���",niModuleN,nMaxLegsPerBody,(char*)szLimit);
	}
	for(pModule=Ta.Module.GetFirst();legschema_modified&&pModule;pModule=Ta.Module.GetNext())
	{
		pModule->m_dwLegCfgWord.Clear();
		pModule->m_dwLegCfgWord.AddBodyLegs(pModule->GetBodyNo(),1);
		//Ĭ��A��Ϊÿ�����ߵ�Ĭ�ϼ������
		//WORD wiFirstLegNo=CFGLEG::MaxLegs()*(pModule->GetBodyNo()-1)+1;
		WORD wiFirstLegNo=CFGLEG::BitAddrIndexOf((BYTE)pModule->GetBodyNo())+1;
		pModule->m_arrActiveQuadLegNo[0]=pModule->m_arrActiveQuadLegNo[1]=
			pModule->m_arrActiveQuadLegNo[2]=pModule->m_arrActiveQuadLegNo[3]=wiFirstLegNo;
	}
	//�ߡ�ƥ��ڵ������������Ͳ���ʼ���ڵ�������ڵ�
	for(i=0;i<N10;i++)
	{
		pTtaNode=primaryNodes[i];
		if(pTtaNode==NULL||pTtaNode->pointI>=10000)
			continue;	//����ԺTANS�ǽڵ���չ����(��׼�߻��׼�棩
		int relaPointIArr[2]={0,0};
		bool hasSameCoord2Node = false;
		int idDatumLine=0,idDatumPlane=0,idOrthoPlane=0;
		int numbers[3];
		numbers[0]=(int)pTtaNode->pos.x;
		numbers[1]=(int)pTtaNode->pos.y;
		numbers[2]=(int)pTtaNode->pos.z;
		for(int j=0;j<3;j++)
		{
			if(numbers[j]/10000==7)
				idDatumLine=numbers[j];
			else if(numbers[j]/10000==8)
				idOrthoPlane=numbers[j];
			else if(numbers[j]/10000==9)
				idDatumPlane=numbers[j];
			else if (ciFileType == FILETYPE_ST && numbers[j] / 10000 == 5)
			{
				hasSameCoord2Node = true;
				if (j == 0)
					pTtaNode->J1 = numbers[j] % 10000;
				else if (j == 1)
					pTtaNode->J2 = numbers[j] % 10000;
				else if (j == 2)
					pTtaNode->J3 = numbers[j] % 10000;
			}
			else if(numbers[j]/10000==1)
			{
				if(relaPointIArr[0]==0)
					relaPointIArr[0]=numbers[j]-10000;
				else
					relaPointIArr[1]=numbers[j]-10000;
			}
		}
		if(idDatumLine>0)
		{
			TANS_LINE* pLine=hashLines.GetValue(idDatumLine);
			if(pLine==NULL)
			{
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"��ʧ��׼��%d",idDatumLine);
				continue;
			}
			pTtaNode->posType=5;	//�����
			pTtaNode->J3=pLine->iStartPointI;
			pTtaNode->J4=pLine->iEndPointI;
		}
		else if(idOrthoPlane>0)
		{	//����սڵ����������Ľڵ�
			pTtaNode->posType=6;
			if(numbers[0]==idDatumPlane)
				pTtaNode->ciPosViceType='X';	//��׼����ָ��YZ����
			else if(numbers[1]==idDatumPlane)
				pTtaNode->ciPosViceType='Y';	//��׼����ָ��XZ����
			else if(numbers[2]==idDatumPlane)
				pTtaNode->ciPosViceType='Z';	//��׼����ָ��XY����
			pTtaNode->J3=idOrthoPlane%10000;	//���յ�
		}
		else if(idDatumPlane>0)
		{
			pTtaNode->posType=8;		//�˴������ܸ���idDatumPlane��λ���жϻ�׼����ָ������������(XY/YZ/XZ),��Ĭ��ΪXY���ͣ������ж� wjh-2018.6.2
			pTtaNode->J3=idDatumPlane;	//������׼��J3=90000+��ʾ��׼��ţ�10000+��ʾ�����ڵ��
		}
		else if (hasSameCoord2Node)
		{
			if (relaPointIArr[1] > 0)
			{
				pTtaNode->posType = 6;
				TTA_NODE* pTtaNode0 = hashTtaNodeI.GetValue(relaPointIArr[0]);
				TTA_NODE* pTtaNode1 = hashTtaNodeI.GetValue(relaPointIArr[1]);
				if (pTtaNode->J1>0)
				{
					pTtaNode->ciPosViceType = 'X';	//��׼����ָ��YZ����
					pTtaNode->J3 = pTtaNode->J1;
					pTtaNode->pAttachStart = pTtaNode0;
					pTtaNode->pAttachEnd = pTtaNode1;
					pTtaNode->J1 = relaPointIArr[0];
					pTtaNode->J2 = relaPointIArr[1];
				}
				else if (pTtaNode->J2 > 0)
				{
					pTtaNode->ciPosViceType = 'Y';	//��׼����ָ��XZ����
					pTtaNode->J3 = pTtaNode->J2;
					pTtaNode->pAttachStart = pTtaNode0;
					pTtaNode->pAttachEnd = pTtaNode1;
					pTtaNode->J1 = relaPointIArr[0];
					pTtaNode->J2 = relaPointIArr[1];
				}
				else if (pTtaNode->J3 > 0)
				{
					pTtaNode->ciPosViceType = 'Z';	//��׼����ָ��XY����
					pTtaNode->pAttachStart = pTtaNode0;
					pTtaNode->pAttachEnd = pTtaNode1;
					pTtaNode->J1 = relaPointIArr[0];
					pTtaNode->J2 = relaPointIArr[1];
				}
				relaPointIArr[0] = relaPointIArr[1] = 0;
			}
			else
				pTtaNode->posType = 13;	//����սڵ������������͵Ľڵ�
		}
		TTA_NODE* pTtaNode0=hashTtaNodeI.GetValue(relaPointIArr[0]);
		TTA_NODE* pTtaNode1=hashTtaNodeI.GetValue(relaPointIArr[1]);
		if(pTtaNode0&&pTtaNode1)
		{
			pTtaNode->pAttachStart=pTtaNode0;
			pTtaNode->pAttachEnd=pTtaNode1;
			pTtaNode->J1=relaPointIArr[0];
			pTtaNode->J2=relaPointIArr[1];
			if(pTtaNode->posType==0)//relaPointIArr[0]>0&&relaPointIArr[1]>0)
			{
				if(pTtaNode->pos.x<10000)
					pTtaNode->posType=1;
				else if(pTtaNode->pos.y<10000)
					pTtaNode->posType=2;
				else if(pTtaNode->pos.z<10000)
					pTtaNode->posType=3;
			}
			if(pTtaNode->posType>=8&&pTtaNode->posType<=10)
			{
				double DIFF_XY=fabs(pTtaNode0->pos.x-pTtaNode1->pos.x)+fabs(pTtaNode0->pos.y-pTtaNode1->pos.y);
				double DIFF_YZ=fabs(pTtaNode0->pos.y-pTtaNode1->pos.y)+fabs(pTtaNode0->pos.z-pTtaNode1->pos.z);
				double DIFF_XZ=fabs(pTtaNode0->pos.x-pTtaNode1->pos.x)+fabs(pTtaNode0->pos.z-pTtaNode1->pos.z);
				//LDS���������󽻵Ļ�׼�߷�����ʱ����ݲ��ܴ���ʵ�ʹ���Ҳ�������漰����������ʽ����õ�ʱ�ٴ��� wjh-2018.6.2
				if(DIFF_XY<EPS)//DIFF_YZ&&DIFF_XY<DIFF_XZ)
				{
					pTtaNode->ciPosViceType='Z';
					pTtaNode->posType=8;	//��׼����ָ��XY����
					if(DIFF_XY>0.001)
						logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON,"LDS�ݲ��ܴ��������󽻵Ļ�׼�߷��������,���˹��޸�ģ�Ͳ���ʱ���ܴ�������");
				}
				else if(DIFF_YZ<EPS)//DIFF_XY&&DIFF_YZ<DIFF_XZ)
				{
					pTtaNode->ciPosViceType='X';
					pTtaNode->posType=9;	//��׼����ָ��YZ����
					if(DIFF_YZ>0.001)
						logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON,"LDS�ݲ��ܴ��������󽻵Ļ�׼�߷��������,���˹��޸�ģ�Ͳ���ʱ���ܴ�������");
				}
				else if(DIFF_XZ<EPS)//DIFF_XY&&DIFF_XZ<DIFF_YZ)
				{
					pTtaNode->ciPosViceType='Y';
					pTtaNode->posType=10;	//��׼����ָ��XZ����
					if(DIFF_XZ>0.001)
						logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON,"LDS�ݲ��ܴ��������󽻵Ļ�׼�߷��������,���˹��޸�ģ�Ͳ���ʱ���ܴ�������");
				}
				else
				{
					pTtaNode->ciPosViceType='I';
					pTtaNode->posType=8;	//��׼����ָ����׼���󽻶�������
				}
			}
		}
		if(pTtaNode->pos.x>10000)
			pTtaNode->pos.x=numbers[0]%10000;
		if(pTtaNode->pos.y>10000)
			pTtaNode->pos.y=numbers[1]%10000;
		if(pTtaNode->pos.z>10000)
			pTtaNode->pos.z=numbers[2]%10000;
	}
	for(i=0;i<N20;i++)
	{	//��ʼ��ԭʼ(1����)��������ڵ�������ڵ�
		pTtaNode=secondaryNodes[i];
		if((pTtaNode->pAttachStart=hashTtaNodeI.GetValue(pTtaNode->J1))==NULL)
		{
			AfxMessageBox(CXhChar50("δ����%d�Žڵ�������ڵ�%d",pTtaNode->pointI,pTtaNode->J1));
			return;
		}
		if((pTtaNode->pAttachEnd=hashTtaNodeI.GetValue(pTtaNode->J2))==NULL)
		{
			AfxMessageBox(CXhChar50("δ����%d�Žڵ�������ڵ�%d",pTtaNode->pointI,pTtaNode->J2));
			return;
		}
		//�����жϻ�׼����ָ����������	
		if(pTtaNode->posType>=8&&pTtaNode->posType<=10)
		{
			double DIFF_XY=fabs(pTtaNode->pAttachStart->pos.x-pTtaNode->pAttachEnd->pos.x)+fabs(pTtaNode->pAttachStart->pos.y-pTtaNode->pAttachEnd->pos.y);
			double DIFF_YZ=fabs(pTtaNode->pAttachStart->pos.y-pTtaNode->pAttachEnd->pos.y)+fabs(pTtaNode->pAttachStart->pos.z-pTtaNode->pAttachEnd->pos.z);
			double DIFF_XZ=fabs(pTtaNode->pAttachStart->pos.x-pTtaNode->pAttachEnd->pos.x)+fabs(pTtaNode->pAttachStart->pos.z-pTtaNode->pAttachEnd->pos.z);
			//LDS���������󽻵Ļ�׼�߷�����ʱ����ݲ��ܴ���ʵ�ʹ���Ҳ�������漰����������ʽ����õ�ʱ�ٴ��� wjh-2018.6.2
			if(DIFF_XY<EPS)//DIFF_YZ&&DIFF_XY<DIFF_XZ)
			{
				pTtaNode->ciPosViceType='Z';
				pTtaNode->posType=8;	//��׼����ָ��XY����
				pTtaNode->pos=pTtaNode->pAttachStart->pos;
				if(DIFF_XY>0.001)
					logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON,"LDS�ݲ��ܴ��������󽻵Ļ�׼�߷��������,���˹��޸�ģ�Ͳ���ʱ���ܴ�������");
			}
			else if(DIFF_YZ<EPS)//DIFF_XY&&DIFF_YZ<DIFF_XZ)
			{
				pTtaNode->ciPosViceType='X';
				pTtaNode->posType=9;	//��׼����ָ��YZ����
				pTtaNode->pos=pTtaNode->pAttachStart->pos;
				if(DIFF_YZ>0.001)
					logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON,"LDS�ݲ��ܴ��������󽻵Ļ�׼�߷��������,���˹��޸�ģ�Ͳ���ʱ���ܴ�������");
			}
			else if(DIFF_XZ<EPS)//DIFF_XY&&DIFF_XZ<DIFF_YZ)
			{
				pTtaNode->ciPosViceType='Y';
				pTtaNode->posType=10;	//��׼����ָ��XZ����
				pTtaNode->pos=pTtaNode->pAttachStart->pos;
				if(DIFF_XZ>0.001)
					logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON,"LDS�ݲ��ܴ��������󽻵Ļ�׼�߷��������,���˹��޸�ģ�Ͳ���ʱ���ܴ�������");
			}
			else
			{
				pTtaNode->ciPosViceType='I';
				pTtaNode->posType=8;	//��׼����ָ����׼���󽻶�������
			}
		}
	}
	//�ˡ����¼���ڵ�����
	for(pLine=hashLines.GetFirst();pLine;pLine=hashLines.GetNext())
		UpdateDatumLine(pLine);
	for(pPlane=hashPlanes.GetFirst();pPlane;pPlane=hashPlanes.GetNext())
		UpdateDatumPlane(pPlane);
	//���ݸ��ڵ�����ӽڵ����꼰�������ͣ�����AddOrUpdateMirTtaNode���жϳ��� wjh-2019.10.18
	for (pTtaNode=listTtaNodes.GetFirst();pTtaNode;pTtaNode=listTtaNodes.GetNext())
		pTtaNode->UpdateBasicPropFromParentNode();
	for(i=0;i<N10;i++)
	{
		pTtaNode=primaryNodes[i];
		if(pTtaNode==NULL||pTtaNode->pointI>=10000)
			continue;	//����ԺTANS�ǽڵ���չ����(��׼�߻��׼�棩
		if(pTtaNode->IS>0&&pTtaNode->IS<4)	//Y��Գ�
			AddOrUpdateMirTtaNode(pTtaNode,pTtaNode->IS);
		else if(pTtaNode->IS==4)
		{
			AddOrUpdateMirTtaNode(pTtaNode,1);
			AddOrUpdateMirTtaNode(pTtaNode,2);
			AddOrUpdateMirTtaNode(pTtaNode,3);
		}
	}
	for(i=0;i<N20;i++)
	{
		pTtaNode=secondaryNodes[i];
		if(pTtaNode->IS>0&&pTtaNode->IS<4)	//Y��Գ�
			AddOrUpdateMirTtaNode(pTtaNode,pTtaNode->IS);
		else if(pTtaNode->IS==4)
		{
			AddOrUpdateMirTtaNode(pTtaNode,1);
			AddOrUpdateMirTtaNode(pTtaNode,2);
			AddOrUpdateMirTtaNode(pTtaNode,3);
		}
	}
	for(i=0;i<N10;i++)
	{
		pTtaNode=primaryNodes[i];
		if(pTtaNode==NULL||pTtaNode->pointI>=10000)
			continue;	//����ԺTANS�ǽڵ���չ����(��׼�߻��׼�棩
		pTtaNode->UpdatePosition(this);
	}
	for(i=0;i<N20;i++)
	{
		pTtaNode=secondaryNodes[i];
		pTtaNode->UpdatePosition(this);
	}
	CHashList<char>hashBodyLegs;//��ΪBODYLEG_I���洢ֵΪ��ǰ����ڹ��������еĽ��Ⱥ�'A','B'��
	for(pTtaNode=listTtaNodes.GetFirst();pTtaNode;pTtaNode=listTtaNodes.GetNext())
	{
		pTtaNode->UpdatePosition(this);
		if(IE==0)	//ֻ��һ�����ߵ���
		{
			pTtaNode->cfgword=CFGWORD(1);
			if(pTtaNode->pointI<m_arrBodyLeg[0].Fmin)
				pTtaNode->cPosLayer='S';	//С����С���ȱ�ţ���Ϊ����
			else
				pTtaNode->cPosLayer='L';
			continue;
		}
		//�������ɸ�鵱ǰ�ڵ��Ƿ��������Ӧ����ģ����
		int maxFmaxI=m_arrBodyLeg[NBY+IE-1].Fmax;	//ģ�������к��ߵ��������ڵ�ţ�֮����Ϊ���Ľڵ��
		for(i=NBY;i<NBY+IE;i++)
		{
			//pTtaNode=hashTtaNodeI[m_arrBodyLeg[i].Jmin];
			//ɸ�鱾�干�ö�
			int iBody=m_arrBodyLeg[i].LB%10;	//��λ����ʾ��ǰ����ר�в������Խӵı���߶Ⱥ�
			for(int j=0;j<iBody;j++)
			{
				if(  pTtaNode->pointI<=m_arrBodyLeg[j].Fmax ||
					(pTtaNode->pointI>maxFmaxI&&pTtaNode->pointI<=m_arrBodyLeg[j].MAX))
				{
					pTtaNode->cPosLayer='S';
					pTtaNode->cfgword.AddBodyLegs(m_arrBodyLeg[i].pModule->GetBodyNo());
				}
			}
			if(  pTtaNode->pointI>=m_arrBodyLeg[i].Jmin&&pTtaNode->pointI<=m_arrBodyLeg[i].Fmax||
				(pTtaNode->pointI>maxFmaxI&&pTtaNode->pointI<=m_arrBodyLeg[i].MAX))
			{	//ר���ڵ�ǰ���ߵ��������ȶ�
				pTtaNode->cPosLayer='L';
				if(pTtaNode->pointI<m_arrBodyLeg[i].Fmin)	//��ǰ�����еĹ��ý��������
				{
					//pTtaNode->bFoundationNode=false;	//���ɽڵ�
					pTtaNode->cPosLayer='S';	//�ⲿ�ֽڵ����Ϊ��ǰ����ר�е������
					pTtaNode->cfgword.AddBodyLegs(m_arrBodyLeg[i].pModule->GetBodyNo());
				}
				else if(pTtaNode->pointI<=m_arrBodyLeg[i].Fmax)
				{	//Ŀǰ��ʱ���Ⱥ�˳��Ϊ���ȷ�����Ⱥ�'A'-'N'
					pTtaNode->bFoundationNode=true;		//�����ڵ�
					BODYLEG_I iBodyLeg(m_arrBodyLeg[i].pModule->GetBodyNo(),(WORD)(pTtaNode->pointI/10));
					if(hashBodyLegs.GetValue(iBodyLeg.idBodyLeg)!=NULL)
					{
						DWORD legword=GetSingleWord(hashBodyLegs[iBodyLeg.idBodyLeg]-'A'+1);
						pTtaNode->cfgword.AddBodyLegs(m_arrBodyLeg[i].pModule->GetBodyNo(),legword);
					}
					else
					{
						char cMaxIdleLeg='A';
						for(char* pcLeg=hashBodyLegs.GetFirst();pcLeg;pcLeg=hashBodyLegs.GetNext())
						{
							BODYLEG_I dwBodyLeg(hashBodyLegs.GetCursorKey());
							if(dwBodyLeg.wBody!=iBodyLeg.wBody)
								continue;
							if(cMaxIdleLeg<=*pcLeg)
								cMaxIdleLeg+=1;
						}
						DWORD legword=GetSingleWord(cMaxIdleLeg-'A'+1);
						pTtaNode->cfgword.AddBodyLegs(m_arrBodyLeg[i].pModule->GetBodyNo(),legword);
						if (cMaxIdleLeg-'@'>CFGLEG::MaxLegOfBody(iBodyLeg.wBody))
							logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%d�ڵ���ѳ������������",pTtaNode->pointI);
						m_arrBodyLeg[i].pModule->m_dwLegCfgWord.AddSpecWord(pTtaNode->cfgword);
						hashBodyLegs.SetValue(iBodyLeg.idBodyLeg,cMaxIdleLeg);
					}
				}
			}
		}
	}
	//�ߡ�����������
	//��ȡdata.ini�еĹ����������ʱ�
	DisplayProcess(30,"�������ģ�͸˼�����");
	K1=abs(K1);
	for(i=0;i<M10;i++)
	{
		ReadLineText(sText,200,fp);
		TTA_ELEM *pTtaElem,*pMirTtaElem;
		pTtaElem=listTtaElems.append();
		int start_i,end_i;
		//2.N������
		if(ciFileType==2)
		{
			CXhChar200 szRodText=sText;
			char* key=strtok(szRodText,", ");
			CXhChar16 szStatMat;
			pTtaElem->size.idClassType=CLS_LINEANGLE;	//Ĭ��Ϊ�Ǹ�
			for(int j=0;key!=NULL&&j<16;j++)
			{
				if(j==0)
					start_i=atoi(key);
				else if(j==1)
					end_i=atoi(key);
				else if(j==2)
				{
					SEGI zStatMat(key);
					szStatMat.Copy(key);
					pTtaElem->iStatMatNo = pTtaElem->N = zStatMat.iSeg;
				}
				else if(j==3)
					pTtaElem->IS=atoi(key);
				else if(j==4)
					pTtaElem->TYPE=atoi(key);
				else if(j==5)
					pTtaElem->M=atoi(key);
				else if(j==6)
					pTtaElem->L1=atoi(key);
				else if(j==7)
					pTtaElem->L2=atoi(key);
				else if(j==8)
					pTtaElem->SMC=atoi(key);
				else if(j==11)
				{
					double wide_thick=atof(key);
					pTtaElem->size.wide=ftoi(wide_thick);
					wide_thick-=pTtaElem->size.wide;
					pTtaElem->size.thick=ftoi(wide_thick*100);
					pTtaElem->size.idClassType=CLS_LINEANGLE;	//TODO:�ݲ�֪���ֹ���α�ʾ
					if (strchr(szStatMat,'R')!=NULL)
						pTtaElem->size.idClassType=CLS_LINETUBE;
					else if((int)(pTtaElem->size.wide/1000)==2)
					{
						if(strchr(szStatMat,'T')!=NULL)	//T����ϽǸ�
							pTtaElem->size.cSubClassType='T';
						else
							pTtaElem->size.cSubClassType='D';
					}
					else if ((int)(pTtaElem->size.wide / 1000) == 3)
						pTtaElem->size.cSubClassType = 'T';	//T����ϽǸ�
					else if((int)(pTtaElem->size.wide/1000)==4)
						pTtaElem->size.cSubClassType='X';
					pTtaElem->size.wide=(int)(pTtaElem->size.wide)%1000;
				}
				else if(j==12)
				{
					if(key[0]=='H')
						pTtaElem->cMaterial=CSteelMatLibrary::Q345BriefMark();
					else if(key[0]=='D')
						pTtaElem->cMaterial=CSteelMatLibrary::Q420BriefMark();
					else if(key[0]=='E')
						pTtaElem->cMaterial=CSteelMatLibrary::Q460BriefMark();
					else
						pTtaElem->cMaterial=CSteelMatLibrary::Q235BriefMark();
				}
				else if(j==13)
				{
					if((pTtaElem->boltd=atoi(key))==0)//M20*2
						pTtaElem->boltd=20;	//Ҫ������˨ֱ��Ϊ�㣬������������Ԥ�ڴ��� wjh-2019.11.28
				}
				else if(j==14)
					pTtaElem->boltN=max(1,atoi(key));//M20*2
				else if(j==15)	//����
					pTtaElem->dfHoleOutCoef=atof(key);
				key=strtok(NULL," ");
			}
			//1.JB��JE������
			if(hashTtaNodeI.GetValue(start_i)!=NULL)
				pTtaElem->pStart=hashTtaNodeI[start_i];
			if(hashTtaNodeI.GetValue(end_i)!=NULL)
				pTtaElem->pEnd=hashTtaNodeI[end_i];
		}
		else
		{
			if(ciFileType!=1)
			{
				sscanf(sText, "%d%d%d%d%d%d%d%d%d", &start_i, &end_i,
					&pTtaElem->N, &pTtaElem->IS, &pTtaElem->TYPE, &pTtaElem->M,
					&pTtaElem->L1, &pTtaElem->L2, &pTtaElem->SMC);
			}
			else
			{
				CXhChar50 itemstr;
				char* pIterStr = sText;
				CXhChar50 xarrItemKeys[30];
				int count = 0;
				if(NGP<0)	//���޸�ʽ����
				{
					count=DELIMITER_STR::ParseWordsFromStr(sText,", \t",xarrItemKeys,10);
					count=min(count,30);
				}
				for (int index = 0; index <= 9; index++)
				{
					if(NGP>0||count==0)
					{
						BYTE cnReadBytes = index == 8 ? 6 : 5;
						itemstr.NCopy(pIterStr, cnReadBytes);
						pIterStr += cnReadBytes;
					}
					else if(index<count)
						itemstr=xarrItemKeys[index];
					else
						continue;
					if (index == 0)
						start_i = atoi(itemstr);
					else if (index == 1)
						end_i = atoi(itemstr);
					else if (index == 2)
						pTtaElem->N = atoi(itemstr);
					else if (index == 3)
						pTtaElem->IS = atoi(itemstr);
					else if (index == 4)
						pTtaElem->TYPE = atoi(itemstr);
					else if (index == 5)
						pTtaElem->M = atoi(itemstr);
					else if (index == 6)
						pTtaElem->L1 = atoi(itemstr);
					else if (index == 7)
						pTtaElem->L2 = atoi(itemstr);
					else if (index == 8)
						pTtaElem->SMC = atoi(itemstr);
					else if (index == 9)
						break;//δ֪������
				}
				char* pchSizeFinal = pIterStr;// strchr(pIterStr, ' ');
				bool blAlphabetOccured = false;
				BYTE cnReadBytes = pchSizeFinal - pIterStr;
				do
				{
					pchSizeFinal++;
					cnReadBytes = cnReadBytes;
					if (*pchSizeFinal == ' '&&blAlphabetOccured)
						break;
					else if (*pchSizeFinal == ' ' && !blAlphabetOccured)
						continue;
					blAlphabetOccured=true;
				} while (*pchSizeFinal!=0&&cnReadBytes < 25);
				if (blAlphabetOccured&&cnReadBytes < 20)
				{
					itemstr.NCopy(pIterStr, cnReadBytes);
					if(ParseRodSizeSpec(pIterStr, pTtaElem->size))
						pTtaElem->bReadSizeSpec = true;
					if (itemstr[cnReadBytes - 1]<'0' || itemstr[cnReadBytes - 1]>'9')
						pTtaElem->cMaterial = itemstr[cnReadBytes - 1];	//���������
				}
			}
			//1.JB��JE������
			if(hashTtaNodeI.GetValue(start_i)!=NULL)
				pTtaElem->pStart=hashTtaNodeI[start_i];
			if(hashTtaNodeI.GetValue(end_i)!=NULL)
				pTtaElem->pEnd=hashTtaNodeI[end_i];
			if (pTtaElem->pStart == NULL)
				logerr.Log("%d-%d��Ԫʼ�˽ڵ�δ�ҵ�", start_i, end_i);
			if (pTtaElem->pEnd == NULL)
				logerr.Log("%d-%d��Ԫ�ն˽ڵ�δ�ҵ�", start_i, end_i);
			if (pTtaElem->pStart == NULL || pTtaElem->pEnd == NULL)
				continue;
			if(ciFileType==FILETYPE_GTOM)
			{
				pTtaElem->iStatMatNo=pTtaElem->N;
				int imat=pTtaElem->N/1000;
				if (imat>0&&imat<=(int)xarrSteelSymbols.Count)
					pTtaElem->cMaterial=xarrSteelSymbols.At(imat-1);
				else if(imat<=1)
					pTtaElem->cMaterial=CSteelMatLibrary::Q235BriefMark();
				else if(imat==2)
					pTtaElem->cMaterial=CSteelMatLibrary::Q345BriefMark();
				else if(imat==3)
					pTtaElem->cMaterial=CSteelMatLibrary::Q390BriefMark();
				else if(imat==4)
					pTtaElem->cMaterial=CSteelMatLibrary::Q420BriefMark();
				else if(imat==5)
					pTtaElem->cMaterial=CSteelMatLibrary::Q460BriefMark();
				else
					pTtaElem->cMaterial=CSteelMatLibrary::Q235BriefMark();
				int matseri=pTtaElem->N%1000;
				pTtaElem->iSizeOrder=matseri;	//������
			}
			else if(K1<200)
			{	//��ʹ��Q235��Q345���ֲ���
				if(pTtaElem->N>0)
					pTtaElem->cMaterial=CSteelMatLibrary::Q235BriefMark();//'S';
				else
					pTtaElem->cMaterial=CSteelMatLibrary::Q345BriefMark();//'H';
				if(pTtaElem->N>=0)
					pTtaElem->iSizeOrder=pTtaElem->N;
				else if(pTtaElem->N<=-1&&pTtaElem->N>=-490)
					pTtaElem->iSizeOrder=-pTtaElem->N;
				else if(pTtaElem->N<=-500&&pTtaElem->N>=-990)
					pTtaElem->iSizeOrder=(-pTtaElem->N)%500+1;
				else if(pTtaElem->N<=-1001&&pTtaElem->N>=-1490)
				{
					pTtaElem->iSizeOrder=(-pTtaElem->N)%1000;
					pTtaElem->cMaterial=CSteelMatLibrary::Q390BriefMark();//'G'
				}
				else if(pTtaElem->N<=-2001&&pTtaElem->N>=-2490)
				{
					pTtaElem->iSizeOrder=(-pTtaElem->N)%2000;
					pTtaElem->cMaterial=CSteelMatLibrary::Q420BriefMark();//'P'
				}
				else if(pTtaElem->N<=-3001&&pTtaElem->N>=-3490)
				{
					pTtaElem->iSizeOrder=(-pTtaElem->N)%3000;
					pTtaElem->cMaterial=CSteelMatLibrary::Q460BriefMark();//'T'
				}
			}
			else
			{	//ʹ�ö��ֲ��ʵĸֲ�
				if(TSM>0)
				{	//��Ӧ������Ż�ѡ��
					if(pTtaElem->N<=3000)
						pTtaElem->cMaterial=QuerySteelBriefMatMark("Q235");//'S';
					else if(pTtaElem->N<=6000)
						pTtaElem->cMaterial=QuerySteelBriefMatMark("Q345");//'H';
					else if(pTtaElem->N<=7000)
						pTtaElem->cMaterial=QuerySteelBriefMatMark("Q390");//'G';
					else if(pTtaElem->N<=8000)
						pTtaElem->cMaterial=QuerySteelBriefMatMark("Q420");//'P';
					else if(pTtaElem->N<=9000)
						pTtaElem->cMaterial=QuerySteelBriefMatMark("Q460");//'T';
				}
				else
				{	//����
					if(pTtaElem->N<=9100)
						pTtaElem->cMaterial=QuerySteelBriefMatMark("Q235");//'S';
					else if(pTtaElem->N<=9200)
						pTtaElem->cMaterial=QuerySteelBriefMatMark("Q345");//'H';
					else if(pTtaElem->N<=9300)
						pTtaElem->cMaterial=QuerySteelBriefMatMark("Q390");//'G';
					else if(pTtaElem->N<=9400)
						pTtaElem->cMaterial=QuerySteelBriefMatMark("Q420");//'P';
					else if(pTtaElem->N<=9500)
						pTtaElem->cMaterial=QuerySteelBriefMatMark("Q460");//'T';
				}
				//KIND>0&&TSM>0&&
				if(pTtaElem->N<=9000)	//��Ӧ������Ż�ѡ��
					pTtaElem->iStatMatNo=pTtaElem->N;
				else	//pTtaElem->N������ϱ��е�ĳһ������
					pTtaElem->iStatMatNo=pTtaElem->iSizeOrder=pTtaElem->N%100;	//������
				//TYPE������
			}
			if (ciFileType == FILETYPE_TANS && (this->KIND % 10 == 6 || this->KIND % 10 == 9) && ((REN / 10) % 10 == 2))
			{	//����ԺTANS��ִ��6��9�淶ʱ����RENʰλΪ2��N�����⺬��
				char ch3ndDigit = (pTtaElem->N / 10000000) % 10;
				int indexOrder = pTtaElem->N % 10000000;
				if (ch3ndDigit != 9)
					logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT, "�����ļ��еĸ˼�%d-%d�Ĺ�����%dʶ��������ʵdata.ini�ļ�", pTtaElem->pStart->pointI, pTtaElem->pEnd->pointI, pTtaElem->iSizeOrder);
				else
				{
					char ch4thDigit = indexOrder / 1000000;
					indexOrder %= 1000000;
					//if (ch4thDigit == 1)
					//	pRod->size_idClassType = CLS_LINEANGLE;
					//else if (ch4thDigit == 5)
					//	pRod->size_idClassType = CLS_LINETUBE;
					WORD wiMaterial = indexOrder / 10000;	//5~6λ��ʾ����
					//pTtaElem->iSizeOrder = indexOrder % 10000;
					pTtaElem->iStatMatNo=indexOrder % 10000;
				}
			}
		}
		//3.IS������
		if(pTtaElem->IS>0&&pTtaElem->IS<3)	//Y��Գ�
		{
			pMirTtaElem=listTtaElems.append(*pTtaElem);
			pMirTtaElem->pStart=AddOrUpdateMirTtaNode(pTtaElem->pStart,pTtaElem->IS);
			pMirTtaElem->pEnd  =AddOrUpdateMirTtaNode(pTtaElem->pEnd  ,pTtaElem->IS);
		}
		else if(pTtaElem->IS==4)	//XYZ��Գ�
		{
			for(j=1;j<4;j++)
			{
				pMirTtaElem=listTtaElems.append(*pTtaElem);
				pMirTtaElem->pStart=AddOrUpdateMirTtaNode(pTtaElem->pStart,j);
				pMirTtaElem->pEnd  =AddOrUpdateMirTtaNode(pTtaElem->pEnd  ,j);
			}
		}
		//4.TYPE������
	}
	//�ˡ�����������
	if(ciFileType==1)
	{
		for(i=0;i<M20;i++)
		{
			fgets(sText,201,fp);
			TTA_BRACEELEM *pMirTtaBraceElem;
			TTA_BRACEELEM *pTtaBraceElem=listTtaBraceElems.append();
			long start_i,end_i;
			sscanf(sText,"%d%d%d%d",&start_i,&end_i,&pTtaBraceElem->IS,&pTtaBraceElem->N);
			pTtaBraceElem->pStart=hashTtaNodeI[start_i];
			pTtaBraceElem->pEnd=hashTtaNodeI[end_i];
			//pTtaBraceElem->IS��ԭʼ������ʮλ���λ��ʾ�������� wjh-2018.6.7
			if(pTtaBraceElem->IS%10>=4)	//ʶ��XYZ��Գ�
			{
				for(int j=1;j<4;j++)
				{
					pMirTtaBraceElem=listTtaBraceElems.append(*pTtaBraceElem);
					pMirTtaBraceElem->pStart=AddOrUpdateMirTtaNode(pTtaBraceElem->pStart,j);
					pMirTtaBraceElem->pEnd  =AddOrUpdateMirTtaNode(pTtaBraceElem->pEnd  ,j);
				}
			}
			else if(pTtaBraceElem->IS%10==1)	//ʶ��X��Y��Գ�,����Y��Գ�
			{
				pMirTtaBraceElem=listTtaBraceElems.append(*pTtaBraceElem);
				int mir_i=1;
				if(pTtaBraceElem->pStart->pos.x==0||pTtaBraceElem->pEnd->pos.x==0)
					mir_i=2;	//�޷���������(Y)�Գ�ʱ������ǰ��(X)�Գ�
				pMirTtaBraceElem->pStart=AddOrUpdateMirTtaNode(pTtaBraceElem->pStart,mir_i);
				pMirTtaBraceElem->pEnd  =AddOrUpdateMirTtaNode(pTtaBraceElem->pEnd  ,mir_i);
			}
		}
	}
	else if(ciFileType==2)
	{

	}
	else if(ciFileType==3)
	{	//�������Ժ�ĸ������������¼���Ǹ���������֮��Ĺ�����ϵ
		for(i=0;i<M20;i++)
		{
			fgets(sText,201,fp);
			TTA_ASSISTELEM* pAssElem=listTtaAssElems.append();
			pAssElem->index=i+1;
			sscanf(sText,"%d%d%d%d",&pAssElem->iKeyNode[0],&pAssElem->iKeyNode[1],&pAssElem->iKeyNode[2],&pAssElem->iKeyNode[3]);
		}
	}
#ifdef __TIMER_COUNT_
	timer.Relay();
#endif
	DisplayProcess(50,"���ɸ�����ģ�ͼ�������");
	BuildLdsGeoModel();
	if(ciFileType==FILETYPE_TANS&&hashTansLegBraceNodes.GetNodeNum()>0)
	{	//�����Ȳ��ֻ����Ľڵ㼰�˼�����ĺ�
		NODESET nodeset;
		for(long* piPointI=hashTansLegBraceNodes.GetFirst();piPointI!=NULL;piPointI=hashTansLegBraceNodes.GetNext())
		{
			long hCurrPointI = hashTansLegBraceNodes.GetCursorKey();
			long hRefPointI = (*piPointI) > 0 ? (*piPointI) : hCurrPointI;
			pTtaNode=hashTtaNodeI.GetValue(hCurrPointI);
			TTA_NODE* pRefBaseTtaNode = (hRefPointI == hCurrPointI) ? NULL : hashTtaNodeI.GetValue(hRefPointI);
			if(pTtaNode->pNode!=NULL)
			{
				nodeset.append(pTtaNode->pNode);
				pTtaNode->pNode->chLayer1st = 'L';
				if (pRefBaseTtaNode != NULL && pRefBaseTtaNode->pNode != NULL)
				{
					pTtaNode->pNode->cfgword = pRefBaseTtaNode->pNode->cfgword;
					pTtaNode->pNode->feature = 1;
				}
				else
					pTtaNode->pNode->feature = 0;
			}
		}
		//���������Ȳ��ڵ���ĺ�
		CLDSNode* pNode,*pRelaNode;
		bool blValModified=false;
		//���������������ã���Ϊ�ѷ���TANS�к����ڵ���Ϊ�������Ȼ������� wjh-2019.7.10
		do{
			blValModified=false;
			for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
			{
				if (pNode->feature == 1)
					continue;	//ǰ���Ѵ����
				for(int i=0;i<4;i++)
				{
					if((pRelaNode=Ta.FromNodeHandle(pNode->arrRelationNode[i]))==NULL)
						continue;
					CFGWORD oldword=pNode->cfgword;
					pNode->cfgword&=pRelaNode->cfgword;
					if(!oldword.IsEqual(pNode->cfgword))
						blValModified=true;
				}
			}
		}while(blValModified);
		//ͳ���Ȳ����ĸ˼�
		CLDSLinePart* pRod;
		CHashSet<CLDSLinePartPtr> hashLegBraceRods;
		for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
		{
			for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
			{
				if(pRod->pStart==pNode||pRod->pEnd==pNode)
					hashLegBraceRods.SetValue(pRod->handle,pRod);
			}
		}
		//���ݸ��ĸ˼������������Ľڵ㣨�������Ȳ����ģ��͸˼��������ĺ�
		do{
			blValModified=false;
			for(pRod=hashLegBraceRods.GetFirst();pRod;pRod=hashLegBraceRods.GetNext())
			{
				pRod->chLayer1st='L';
				CFGWORD oldword=pRod->cfgword;
				pRod->cfgword=pRod->pStart->cfgword&pRod->pEnd->cfgword;
				if(!oldword.IsEqual(pRod->cfgword))
					blValModified=true;
				if(!pRod->cfgword.IsEqual(pRod->pStart->cfgword)&& 
					hashTansLegBraceNodes.GetValue(pRod->pStart->handle)!=NULL)
				{
					pRod->pStart->cfgword=pRod->cfgword;
					blValModified=true;
				}
				if(!pRod->cfgword.IsEqual(pRod->pEnd->cfgword)&&
					hashTansLegBraceNodes.GetValue(pRod->pEnd->handle) != NULL)
				{
					pRod->pEnd->cfgword=pRod->cfgword;
					blValModified=true;
				}
			}
		}while(blValModified);
		//���¶ԳƸ��Ľڵ����ĺ�
		for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
		{
			pNode->SyncMirProp("cfgword");
			pNode->SyncMirProp("layer");
		}
		for(pRod=hashLegBraceRods.GetFirst();pRod;pRod=hashLegBraceRods.GetNext())
		{
			pRod->SyncMirProp("cfgword");
			pRod->SyncMirProp("layer");
		}
	}
	DisplayProcess(80,"����������ؼ�������");
#ifdef __TIMER_COUNT_
	timer.Relay(1);
#endif
	if (ciFileType == FILETYPE_ST)	//����SmartTower�ļ��еĵ����ߺ�����Ϣ
		ReadSmartTowerLoadSection(fp);
	else if(ciFileType!=2)
	{	//Ŀǰ�����붫��Ժ����������ĺ������� wjh-2018.10.16
	//�š��������ػ������ݼ��������ϵ��
	ReadLineText(sText,100,fp);
	int NPANEL,VG,VH;
	char EXP0[50]="";	//����ֲڶ�
	if(KIND>=1000)	//�Զ�������������
	{
		//sscanf(sText,"%d%d%d%s",&NPANEL,&VG,&VH,EXP0);	//�����������У�" 0.000     1.000     1.000"
		char* key=strtok(sText," ");
		for(j=0;key!=NULL;j++)
		{
			if(j==0)
				NPANEL=atoi(key);
			else if(j==1)
				VG=atoi(key);
			else if(j==2)
				VH=(int)atof(key);
			else if(j==3)
				StrCopy(EXP0,key,50);
			key=strtok(NULL," ");
		}
		m_arrWindSeg.SetSize(NPANEL);
		Ta.m_cLandscape=0;
		for(i=0;EXP0[i]!=0;i++)
		{
			if(EXP0[i]<'A'||EXP0[i]>'D')
				continue;
			Ta.m_cLandscape=EXP0[i];
			break;
		}
		if(Ta.m_cLandscape<'A'||Ta.m_cLandscape>'D')
		{
			logerr.Log("����ص���ֲڶ��������,����ʧ��,ϵͳĬ��Ϊ'B'��");
			Ta.m_cLandscape='B';
		}
	}
	CWorkStatus *pStatus;
	Ta.WorkStatus.Empty();
	Ta.m_iLoadExecuteCode=3;	//�û�ָ������ִ�й淶
	CObjNoGroup *pWorkStatusGroup=Ta.NoManager.FromGroupID(WORKSTATUS_GROUP);
	pWorkStatusGroup->EmptyNo();
	for(i=0;i<NL;i++)
	{
		pStatus=Ta.WorkStatus.append();
		pStatus->iNo=pWorkStatusGroup->EnumIdleNo();
		pWorkStatusGroup->SetNoState(pStatus->iNo);
		ReadLineText(sText,100,fp);
		//���뽫������Ϊָ������ϵ��ģʽ�������޷�����������ָ���ɱ�������ϵ��
		pStatus->m_bSpecLoadFactors=true;
		pStatus->m_bLoadIncFactor=true;
		//FyΪ��ѹ����ϵ������������ϵ���˻�,FacΪ�������ϵ�����������к���
		double Fx,Fy,Fz;
		double Fac;	//�ܺ���ϵ��(����Myl�ļ��н�ָx,y�����ϵ����
		double fMylIceWeightAmplifyCoef = 0;	//���า����������ϵ��(����Ҫ��ϵ��*����ϵ��)
		if(ciFileType==1)
			sscanf(sText,"%lf%lf%lf%lf%lf%s",&Fx,&Fy,&Fz,&Fac,&fMylIceWeightAmplifyCoef,pStatus->description);
		else
			sscanf(sText,"%lf%lf%lf%lf%s",&Fx,&Fy,&Fz,&Fac,pStatus->description);
		if(KIND>=1000)
		{
			pStatus->WindLoad.velocity=Fx;
			pStatus->WindLoad.azimuth=ftoi(Fz);
			pStatus->SetKesai(1.0);		//�趨��ͨ�ɱ�������ϵ��Ϊ1.0
			if(ciFileType==1)
			{
				pStatus->SetKesai_W(Fy*Fac);//�趨��������ϵ��ΪFy*Fac;
				if(fMylIceWeightAmplifyCoef<1.0)
					fMylIceWeightAmplifyCoef=1.0;
				pStatus->SetGamaQ(fMylIceWeightAmplifyCoef);		//TTA�������ѿ��ǿɱ���ط���ϵ����wjh-2015.6.14
			}
			else
			{
				pStatus->SetKesai_W(Fy*Fac);//�趨��������ϵ��ΪFy*Fac;
				pStatus->SetGamaQ(1.0*Fac);		//TTA�������ѿ��ǿɱ���ط���ϵ����wjh-2015.6.14
			}
			//pStatus->SetKesai(Fac);
		}
		/*double betaZ=1.0;
		if(Ta.m_fTowerHeight<=20)
			betaZ=1.0;
		else if(Ta.m_fTowerHeight<=30)
			betaZ=1.0+0.025*(Ta.m_fTowerHeight-20);
		else if(Ta.m_fTowerHeight<=40)
			betaZ=1.25+0.01*(Ta.m_fTowerHeight-30);
		else if(Ta.m_fTowerHeight<=50)
			betaZ=1.35+0.015*(Ta.m_fTowerHeight-40);
		else //if(m_fTowerHeight<=60)
			betaZ=1.5+0.01*(Ta.m_fTowerHeight-50);
		betaZ*=pStatus->Bw_I();
		fprintf(fp,"%8.3f%8.3f%5d.000   1.000       %s\n",pStatus->WindLoad.velocity,
			pStatus->Kesai_W()*betaZ,pStatus->WindLoad.azimuth,pStatus->description);*/
	}
	//ʮ�������ߺ�������	
	for(i=0;i<NC;i++)
	{
		bool bLoadNode=false;
		GEPOINT stdload,load;
		CExternalNodeLoad nodeLoad;
		int pointI;
		ReadLineText(sText,100,fp);
		sscanf(sText,"%d%lf%lf%lf",&pointI,&load.x,&load.y,&load.z);
		load*=1000;	//kN->N
		CLDSNode *pNode=hashLdsNodeI[pointI];
		CWorkStatus *pFirstStatus=Ta.WorkStatus.GetFirst();
		//������ر�׼ֵ wjh-2015.3.4
		double dfDesignCoef=pFirstStatus->GamaQ()*pFirstStatus->Kesai();
		if (fabs(dfDesignCoef)<EPS)
			dfDesignCoef=1.0;
		stdload = load / dfDesignCoef;
		if(pFirstStatus->m_cMainLoadType<4)
			stdload/=Ta.m_fSafetyCoef;
		nodeLoad.designLoad=load;
		nodeLoad.stdLoad=stdload;
		if(pNode==NULL)
		{
			logerr.Log("%d��δ�ҵ�����ȴ�Ǻ��ؽڵ�",pointI);
			continue;
		}
		CExternalNodeLoad* pNodeLoad=pFirstStatus->hashNodeLoad.Add(pNode->handle);
		//��������Ӧ������Ӧ������������ͬһ���ڵ����������(ͬΪ���߹ҵ�����߹ҵ�) wjh-2017.3.16
		pNodeLoad->designLoad+=nodeLoad.designLoad;
		pNodeLoad->stdLoad+=nodeLoad.stdLoad;
		for(pStatus=Ta.WorkStatus.GetNext();pStatus;pStatus=Ta.WorkStatus.GetNext())
		{
			ReadLineText(sText,100,fp);
			sscanf(sText,"%lf%lf%lf",&load.x,&load.y,&load.z);
			load*=1000;	//kN->N
			//������ر�׼ֵ wjh-2015.3.4
			double dfDesignCoef=pStatus->GamaQ()*pStatus->Kesai();
			if (fabs(dfDesignCoef)<EPS)
				dfDesignCoef=1.0;
			stdload=load/dfDesignCoef;
			if(pStatus->m_cMainLoadType<4)
				stdload/=Ta.m_fSafetyCoef;
			nodeLoad.designLoad=load;
			nodeLoad.stdLoad=stdload;
			pNodeLoad=pStatus->hashNodeLoad.Add(pNode->handle);
			pNodeLoad->designLoad+=nodeLoad.designLoad;
			pNodeLoad->stdLoad+=nodeLoad.stdLoad;
		}
	}
	if(NC>0)
		ExtractWireNode();
	//ʮһ��������������
	for(i=0;i<NW;i++)
		ReadLineText(sText,100,fp);	//����������������
	//ʮһ�������ѹ������
	TTA_WINDSEG ttaseg;
	Ta.WindLoadSeg.Empty();
	CObjNoGroup *pWindSegGroup=Ta.NoManager.FromGroupID(WINDSEG_GROUP);
	pWindSegGroup->EmptyNo();
	CHashSet<CWindSegment*> hashBelongWindSegByRodId;
	for(i=0;i<NPANEL;i++)
	{
		ReadLineText(sText,100,fp);
		sscanf(sText,"%d%d%d%d%d%d%d%d%d%d",&ttaseg.TYPE,&ttaseg.arrPointI[0],
			&ttaseg.arrPointI[1],&ttaseg.arrPointI[2],&ttaseg.arrPointI[3],
			&ttaseg.NLG,&ttaseg.Fa,&ttaseg.Fb,&ttaseg.Fw,&ttaseg.BetaZ);
		CWindSegment *pSeg=Ta.WindLoadSeg.append();
		pSeg->iNo=pWindSegGroup->EnumIdleNo();
		pWindSegGroup->SetNoState(pSeg->iNo);
		int calStyle=ttaseg.TYPE/1000;
		if(calStyle==0)
			pSeg->cCalStyle=0;	//����ʽ����
		else if(calStyle==1)
			pSeg->cCalStyle=1;
		else if(calStyle==2)
			pSeg->cCalStyle=2;
		if(ttaseg.TYPE%1000>=100)
			pSeg->cMirType=1;
		else
			pSeg->cMirType=0;
		int segType=ttaseg.TYPE%10;
		if(segType==1)
			pSeg->cType=0;	//�˽ڵ���������
		else
			pSeg->cType=1;	//�Զ����ڵ�����
		//if(segType==
		for(j=0;j<4;j++)
		{
			if(ttaseg.arrPointI[j]>0)
			{
				if(hashLdsNodeI.GetValue(ttaseg.arrPointI[j])!=NULL)
				{
					pSeg->nodePtrArr[j]=hashLdsNodeI[ttaseg.arrPointI[j]];
					pSeg->nodeArr[j]=pSeg->nodePtrArr[j]->handle;
				}
			}
		}
		pSeg->CqiFront=ttaseg.Fa*0.01;
		pSeg->CqiSide=ttaseg.Fb*0.01;
		if(ttaseg.BetaZ>100)
			pSeg->BetaZ=ttaseg.BetaZ*0.01;
		else
			pSeg->BetaZ=1.0;
		//Ӧ����ttaseg.Fw���·�ѹ���ڸ˼���Ԫ����������ϵ�� wjh-2016.6.02
		pSeg->StatValidNodeCount(&Ta);
		CLDSLinePart* pRod;
		if(pSeg->cType==0)
		{	//�������ͱ�׼��ѹ��
			TAPER_SCOPE taper;
			for(j=0;j<2;j++)
			{
				if(pSeg->nodePtrArr[j]==NULL)
					continue;
				taper.VerifyVertex(pSeg->nodePtrArr[j]->Position(false));
				CLDSNode* pMirNode=pSeg->nodePtrArr[j]->GetMirNode(MIRMSG(3));
				if(pMirNode)
					taper.VerifyVertex(pMirNode->Position(false));
			}
			for (pRod = Ta.EnumRodFirst();pRod;pRod = Ta.EnumRodNext())
			{
				if (pRod->pStart == NULL || pRod->pEnd == NULL)
					continue;
				if (taper.IsIncludePoint(pRod->pStart->Position(false)) && taper.IsIncludePoint(pRod->pEnd->Position(false)))
				{
					CWindSegment* pBelongSeg=hashBelongWindSegByRodId.GetValue(pRod->Id);
					if(pBelongSeg==NULL||pBelongSeg->cType==1)	//������ǰͳ�Ƶ�����ᵣ��ѹ�Σ������������������ϵ��Ϊ׼
					{
						pRod->m_fWeightRaiseCoef = ttaseg.Fw*0.01;
						hashBelongWindSegByRodId.SetValue(pRod->Id,pSeg);
					}
				}
			}
		}
		else if(pSeg->cType==1)//||pSeg->cType==2)
		{	//X��Y��ᵣ���ͱ�׼��ѹ�μ���
			WINDSEG_FILTER filter;
			filter.InitFilter(pSeg->nodePtrArr,pSeg->ValidNodeCount(),f3dPoint());
			for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
			{
				if(pRod->pStart==NULL||pRod->pEnd==NULL)
					continue;
				if (filter.IsIncludeRod(pRod))
				{
					CWindSegment* pBelongSeg=hashBelongWindSegByRodId.GetValue(pRod->Id);
					if(pBelongSeg==NULL)	//������ǰͳ�Ƶ�����ᵣ��ѹ�Σ������ظ��趨
					{
						pRod->m_fWeightRaiseCoef = ttaseg.Fw*0.01;
						hashBelongWindSegByRodId.SetValue(pRod->Id,pSeg);
					}
				}
			}
		}
	}
	//ʮ������ͼ����
	if (ciFileType == 0)
	{	//����Ժ���ݲ�����������Ϣ��ֻ��Ҫ��ǰ���뱨������ļ����� wjh-2019.7.1
		//��ȡ�����ļ������ļ�
		CXhChar200 outfile = szDataFilePathName;
		outfile.Append(".out");
		if ((fpTmp = fopen(outfile, "rt"))!=NULL)
		{	//���������ļ�����������ļ�
			ReadOutFile(fpTmp, NULL, NULL, ciFileType);
			fclose(fpTmp);
		}
		DisplayProcess(100,"���ɼ���ģ��...");
		return;
	}
	//else if(ciFileType!=1&&ciFileType!=2&&ciFileType!=3)
	//	return;
	else if(ReadEffectLineText(sText,100,fp)==NULL&&ciFileType<3)
		return;
	}
	int NFACE=0,LEGINDEX=1,DRAW_FACTOR=5;
	CXhChar50 itemtext;
	ARRAY_LIST<TTA_NODEPTR>braceNodes(0,128);
	CMapList<TTA_BRACEELEM*> mapOnlyBraceElems;
	mapOnlyBraceElems.SetHashFunc(Hash2Key);
	CXhChar200 szST_IA2_file="";	//����Ժ&SmartTower�Ķ������������ļ�
	if(ciFileType==1)
	{	//������Ӧ��
		sscanf(sText,"%d%d%d",&NFACE,&LEGINDEX,&DRAW_FACTOR);
		for(i=0;i<NFACE;i++)
		{	//����NFACE�л�ͼ����
			if(ReadEffectLineText(sText,100,fp)==NULL)
				return;	//�����ж�
		}
#ifdef __TIMER_COUNT_
		timer.Relay(2);
#endif
		DisplayProcess(100,"����������ģ�͵������");
		//ʮ������ȡ������Ӧ���ļ��еĸ�����
		//13.1���븨�Ľڵ�
		while(true)
		{
			sText[0]=0;
			if(ReadEffectLineText(sText,100,fp)==NULL)
				break;
			//if(sText[0]!='b'&&sText[1]!='p'&&sText[1]!='r'&&sText[1]!='h'&&sText[1]!='x')
			//	break;
			TTA_NODE ttanode;
			char* iterstr=sText;
			itemtext.NCopy(iterstr+1,9);
			ttanode.pointI=atoi(itemtext);
			iterstr=sText+10;
			itemtext.NCopy(iterstr,3);
			ttanode.IS=atoi(itemtext);
			iterstr+=3;
			UpdateTtaNodeCfgWord(&ttanode);
			ttanode.SetPosUpdateState(0);
			if(sText[0]=='b')
			{	//����㷨����㣨�������κ�������
				CXhChar50 itemtextarr[4];
				ParseMylBraceNodeStr(iterstr,&itemtextarr[0],&itemtextarr[1],&itemtextarr[2],&itemtextarr[3]);
				//itemtext.NCopy(&iterstr,11,true);
				ttanode.pos.x=atof(itemtextarr[0]);
				//itemtext.NCopy(&iterstr,11,true);
				ttanode.pos.y=atof(itemtextarr[1]);
				//itemtext.NCopy(&iterstr,11,true);
				ttanode.pos.z=atof(itemtextarr[2]);
				ttanode.SetPosUpdateState(0x07);
			}
			else if(sText[0]=='p')
			{	//��ͨ�ؾ෨����㣨ָ����������㣩
				CXhChar50 itemtextarr[4];
				ParseMylBraceNodeStr(iterstr,&itemtextarr[0],&itemtextarr[1],&itemtextarr[2],&itemtextarr[3]);
				//itemtext.NCopy(&iterstr,11,true);
				ttanode.J1=(int)atof(itemtextarr[0]);
				//itemtext.NCopy(&iterstr,11,true);
				ttanode.J2=(int)atof(itemtextarr[1]);
				//itemtext.NCopy(&iterstr,11,true);
				ttanode.posType=(int)atof(itemtextarr[2])+1;	//itemtext=0:ָ��X����;=1:ָ��Y����
				//itemtext.NCopy(&iterstr,11,true);
				if(ttanode.posType==1)
				{
					ttanode.pos.x=atof(itemtextarr[3]);	//itemtext=0:ָ��X����;=1:ָ��Y����
					ttanode.SetPosUpdateState(0x01);
				}
				else if(ttanode.posType==2)
				{
					ttanode.pos.y=atof(itemtextarr[3]);	//itemtext=0:ָ��X����;=1:ָ��Y����
					ttanode.SetPosUpdateState(0x02);
				}
				else //if(ttanode.posType==3)
				{
					ttanode.pos.z=atof(itemtextarr[3]);	//itemtext=0:ָ��X����;=1:ָ��Z����
					ttanode.SetPosUpdateState(0x04);
				}
			}
			else if(sText[0]=='r')
			{	//���ȷֵ㷨����㣨�����ȷֵ㣩
				CXhChar50 itemtextarr[4];
				ParseMylBraceNodeStr(iterstr,&itemtextarr[0],&itemtextarr[1],&itemtextarr[2],&itemtextarr[3]);
				//itemtext.NCopy(&iterstr,11,true);
				ttanode.J1=(int)atof(itemtextarr[0]);
				//itemtext.NCopy(&iterstr,11,true);
				ttanode.J2=(int)atof(itemtextarr[1]);
				//itemtext.NCopy(&iterstr,11,true);
				double M=atof(itemtextarr[2]);	//��������
				//itemtext.NCopy(&iterstr,11,true);
				double D=atof(itemtextarr[3]);	//������ĸ
				ttanode.coefScale=M/D;
				ttanode.posType=4;	//�����ȷֵ�
			}
			else if(sText[0]=='h')
			{	//�ȸߵ�����㣨ָ��Z��������㣩
				itemtext.NCopy(&iterstr,11,true);
				ttanode.J1=(int)atof(itemtext);
				itemtext.NCopy(&iterstr,11,true);
				ttanode.J2=(int)atof(itemtext);
				itemtext.NCopy(&iterstr,11,true);
				ttanode.pos.z=atof(itemtext);	//itemtext=0:ָ��X����;=1:ָ��Y����
				if (ttanode.pos.z > 10000)
					ttanode.J3 = ftoi(ttanode.pos.z);
				ttanode.SetPosUpdateState(0x04);
			}
			else if(sText[0]=='x')
			{	//�����ཻ������㣨����㣩
				CXhChar50 itemtextarr[4];
				ParseMylBraceNodeStr(iterstr,&itemtextarr[0],&itemtextarr[1],&itemtextarr[2],&itemtextarr[3]);
				//itemtext.NCopy(&iterstr,11,true);
				ttanode.J1=(int)atof(itemtextarr[0]);
				//itemtext.NCopy(&iterstr,11,true);
				ttanode.J2=(int)atof(itemtextarr[1]);
				//itemtext.NCopy(&iterstr,11,true);
				ttanode.J3=(int)atof(itemtextarr[2]);
				//itemtext.NCopy(&iterstr,11,true);
				ttanode.J4=(int)atof(itemtextarr[3]);
				ttanode.posType=5;	//�����
			}
			else
				break;
			pTtaNode=listTtaNodes.append(ttanode);
			pTtaNode->bBraceNode=true;	//���Ľڵ�
			hashOnlyBraceNodes.SetValue(pTtaNode->pointI,pTtaNode);
			braceNodes.append(pTtaNode);
			hashTtaNodeI.SetValue(pTtaNode->pointI,pTtaNode);
			if(pTtaNode->IS==1)	//Y��Գ�
			{
				pMirTtaNode=listTtaNodes.append();
				pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(2));
				pMirTtaNode->MirBasicPropFrom(pTtaNode,2);
				hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
				hashOnlyBraceNodes.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			}
			else if(pTtaNode->IS==2)	//X��Գ�
			{
				pMirTtaNode=listTtaNodes.append();
				pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(1));
				pMirTtaNode->MirBasicPropFrom(pTtaNode,1);
				hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
				hashOnlyBraceNodes.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			}
			else if(pTtaNode->IS==3)	//Z��Գ�
			{
				pMirTtaNode=listTtaNodes.append();
				pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(3));
				pMirTtaNode->MirBasicPropFrom(pTtaNode,3);
				hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
				hashOnlyBraceNodes.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			}
			else if(pTtaNode->IS==4)	//XYZ��Գ�
			{
				pMirTtaNode=listTtaNodes.append();
				pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(1));
				pMirTtaNode->MirBasicPropFrom(pTtaNode,1);
				hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
				hashOnlyBraceNodes.SetValue(pMirTtaNode->pointI,pMirTtaNode);
				pMirTtaNode=listTtaNodes.append();
				pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(2));
				pMirTtaNode->MirBasicPropFrom(pTtaNode,2);
				hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
				hashOnlyBraceNodes.SetValue(pMirTtaNode->pointI,pMirTtaNode);
				pMirTtaNode=listTtaNodes.append();
				pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(3));
				pMirTtaNode->MirBasicPropFrom(pTtaNode,3);
				hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
				hashOnlyBraceNodes.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			}
		}
		//MYL�ļ���ȱ�ٹ���ָ���������ж�������λ���ȶ��ϵĸ��Ľڵ��ͼ������'S'��'L'����ĺ�ȱ���������壬
		//ֻ���ڴ˴������������ڵ���¸��Ľڵ����ĺ� wjh-2018.5.25
#ifdef __TIMER_COUNT_
		timer.Relay(3);
#endif
		if (braceNodes.Count>0)
		{	//������������������¼��㸨�Ķ˵�����
			for (i=0;i<braceNodes.GetSize();i++)
			{
				pTtaNode=braceNodes[i];
				hashParentTtaNodeI.SetValue(pTtaNode->pointI,true);
				CFGWORD oldcfgword=pTtaNode->cfgword;
				ValidateBraceNodeCfgWord(pTtaNode,true);
				TTA_NODE* pMirTtaNode=NULL;
				if (pTtaNode->IS==1)		//���ҶԳ�
					pMirTtaNode=hashTtaNodeI.GetValue(MirPointI(pTtaNode->pointI,MIRMSG(2)));
				else if (pTtaNode->IS==2)//ǰ��Գ�
					pMirTtaNode=hashTtaNodeI.GetValue(MirPointI(pTtaNode->pointI,MIRMSG(1)));
				else if (pTtaNode->IS==3)//Z��Գ�
					pMirTtaNode=hashTtaNodeI.GetValue(MirPointI(pTtaNode->pointI,MIRMSG(3)));
				if (pMirTtaNode)
				{
					pMirTtaNode->cfgword=pTtaNode->cfgword;
					pMirTtaNode->cPosLayer=pTtaNode->cPosLayer;
				}
				if (pTtaNode->IS==4)
				{	//�ĶȶԳ�
					for (j=1;j<4;j++)
					{
						if ((pMirTtaNode=hashTtaNodeI.GetValue(MirPointI(pTtaNode->pointI,MIRMSG(j))))!=NULL)
						{
							pMirTtaNode->cfgword=pTtaNode->cfgword;
							pMirTtaNode->cPosLayer=pTtaNode->cPosLayer;
						}
					}
				}
			}
			for (i=0;i<braceNodes.GetSize();i++)
			{
				pTtaNode=braceNodes[i];
				pTtaNode->UpdatePosition(this);
			}
			//���ݸ��ڵ�����ӽڵ����꼰�������ͣ�����AddOrUpdateMirTtaNode���жϳ��� wjh-2019.10.18
			for (pTtaNode=listTtaNodes.GetFirst();pTtaNode;pTtaNode=listTtaNodes.GetNext())
			{
				if (pTtaNode->bBraceNode)
					pTtaNode->UpdateBasicPropFromParentNode();
			}
			for (i=0;i<braceNodes.GetSize();i++)
			{
				pTtaNode=braceNodes[i];
				if (pTtaNode->IS>0&&pTtaNode->IS<4)
					AddOrUpdateMirTtaNode(pTtaNode,pTtaNode->IS);
				else if (pTtaNode->IS==4)
				{
					AddOrUpdateMirTtaNode(pTtaNode,1);
					AddOrUpdateMirTtaNode(pTtaNode,2);
					AddOrUpdateMirTtaNode(pTtaNode,3);
				}
			}
			for (i=0;i<braceNodes.GetSize();i++)
			{
				pTtaNode=braceNodes[i];
				if (pTtaNode->IsPosUpdated())
					continue;
				//���¼���δ���¹��ĸ����ڵ�����(�����ڵ�Ƕ�׽϶�����)
				if (pTtaNode->UpdatePosition(this))
				{
					if (pTtaNode->IS>0&&pTtaNode->IS<4)
						AddOrUpdateMirTtaNode(pTtaNode,pTtaNode->IS);
					else if (pTtaNode->IS==4)
					{
						AddOrUpdateMirTtaNode(pTtaNode,1);
						AddOrUpdateMirTtaNode(pTtaNode,2);
						AddOrUpdateMirTtaNode(pTtaNode,3);
					}
				}
			}
		}
#ifdef __TIMER_COUNT_
		timer.Relay(4);
#endif
		//13.2���븨����
		do
		{
			if(sText[0]==0&&ReadEffectLineText(sText,100,fp)==NULL)
				break;
			//CXhChar50 labelstart,labelend;
			//ParseMylBraceElemLabelStr(iterstr,&labelstart,&labelend);
			TTA_BRACEELEM braceElem;
			char* iterstr=sText;
			itemtext.NCopy(&iterstr,7,true);
			if(itemtext[6]<'0'||itemtext[6]>'9')
			{
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"����������'%s'��ʽ����, ���ĵ����ж�",sText);
				break;
			}
			int JS=atoi(itemtext);
			TTA_NODE* pTtaNode0=hashTtaNodeI.GetValue(JS);
			int ncpy=7;
			if(iterstr[7]!=' ')
				ncpy=8;	//MYL��ͬ�汾�е�JS��JE���пո�ָ����е�û�У�������������ռ7�ַ��Ƕ��ģ�ֻ�ܾݴ������� wjh-2018.7.5
			itemtext.NCopy(&iterstr,ncpy,true);
			if(itemtext[ncpy-1]<'0'||itemtext[ncpy-1]>'9')
			{
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"����������'%s'��ʽ����, ���ĵ����ж�",sText);
				break;
			}
			int JE=atoi(itemtext);
			TTA_NODE* pTtaNode1=hashTtaNodeI.GetValue(JE);
			if(pTtaNode0&&pTtaNode1)
			{
				braceElem.pStart=pTtaNode0;
				braceElem.pEnd  =pTtaNode1;
			}
			else
			{
				int J=(pTtaNode0==NULL)?JS:JE;
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"�Ҳ�������'%d-%d'�Ķ˽ڵ�'%d', ���ĵ����ж�!",JS,JE,J);
				break;
			}
			itemtext.NCopy(&iterstr,3,true);
			braceElem.IS=atoi(itemtext);
			itemtext.NCopy(&iterstr,5,true);
			braceElem.M=atoi(itemtext);
			itemtext.NCopy(&iterstr,5,true);
			braceElem.percentOfMinRL =atoi(itemtext);
			itemtext.NCopy(&iterstr,5,true);
			braceElem.percentOfParaRL=atoi(itemtext);
			itemtext.NCopy(&iterstr,5,true);
			braceElem.iStatSerial=atoi(itemtext);	//TODO:��δ����
			itemtext.NCopy(&iterstr,11,true);	//����L110X10S���͵Ĺ���ַ����ܼ���Ҫ11���ַ� wjh-2017.5.11
			char* pchSectType=NULL;
			if(strstr(itemtext,"��")!=NULL)
			{
				braceElem.cSectType=CLS_LINETUBE;
				itemtext.Replace("��","");
			}
			else if(strstr(itemtext,"��")!=NULL)
			{
				braceElem.cSectType=CLS_LINETUBE;
				itemtext.Replace("��","");
			}
			else if((pchSectType=strchr(itemtext,'L'))!=NULL)
			{
				//ֱ����pchSectType��Ϊ�˱������ "-102L40x3S"�����Ĺ���ַ��� wjh-2019.9.3
				CXhChar50 tmptstr=(pchSectType+1);
				itemtext=tmptstr;	//itemtext.Replace("L","");
				braceElem.cSectType=CLS_LINEANGLE;
			}
			braceElem.cBriefMaterial=restore_JG_guige(itemtext,braceElem.size_wide,braceElem.size_thick);
			if(QuerySteelMatByBriefMark(braceElem.cBriefMaterial)==NULL)
			{	//���ʹ����ʧ�ܣ�����ȡĬ��ֵ
				braceElem.cBriefMaterial=CSteelMatLibrary::Q235BriefMark();
				braceElem.size_wide=40;
				braceElem.size_thick=3;
			}
			TTA_BRACEELEM *pMirTtaBraceElem;
			TTA_BRACEELEM *pTtaBraceElem=listTtaBraceElems.append(braceElem);
			mapOnlyBraceElems.SetValue(JS,JE,pTtaBraceElem);
			if(pTtaBraceElem->IS%10>=4)	//ʶ��XYZ��Գ�
			{
				for(int i=1;i<4;i++)
				{
					pMirTtaBraceElem=listTtaBraceElems.append(*pTtaBraceElem);
					pMirTtaBraceElem->pStart=AddOrUpdateMirTtaNode(pTtaBraceElem->pStart,i);
					pMirTtaBraceElem->pEnd=AddOrUpdateMirTtaNode(pTtaBraceElem->pEnd,i);
					mapOnlyBraceElems.SetValue(pMirTtaBraceElem->pStart->pointI,pMirTtaBraceElem->pEnd->pointI,pMirTtaBraceElem);
				}
			}
			else if(pTtaBraceElem->IS%10==1)	//ʶ��Y��(����)�Գ�
			{
				pMirTtaBraceElem=listTtaBraceElems.append(*pTtaBraceElem);
				int mir_i=1;
				if(pTtaBraceElem->pStart->pos.x==0&&pTtaBraceElem->pEnd->pos.x==0)
					continue;	//�޷���������(Y)�Գ�ʱ
				pMirTtaBraceElem->pStart=AddOrUpdateMirTtaNode(pTtaBraceElem->pStart,mir_i);
				pMirTtaBraceElem->pEnd=AddOrUpdateMirTtaNode(pTtaBraceElem->pEnd,mir_i);
				mapOnlyBraceElems.SetValue(pMirTtaBraceElem->pStart->pointI,pMirTtaBraceElem->pEnd->pointI,pMirTtaBraceElem);
			}
			else if(pTtaBraceElem->IS%10==2)	//ʶ��X��(ǰ��)�Գ�
			{
				pMirTtaBraceElem=listTtaBraceElems.append(*pTtaBraceElem);
				int mir_i=2;
				if(pTtaBraceElem->pStart->pos.y==0&&pTtaBraceElem->pEnd->pos.y==0)
					continue;	//�޷�����ǰ��(X)�Գ�ʱ
				pMirTtaBraceElem->pStart=AddOrUpdateMirTtaNode(pTtaBraceElem->pStart,mir_i);
				pMirTtaBraceElem->pEnd=AddOrUpdateMirTtaNode(pTtaBraceElem->pEnd,mir_i);
				mapOnlyBraceElems.SetValue(pMirTtaBraceElem->pStart->pointI,pMirTtaBraceElem->pEnd->pointI,pMirTtaBraceElem);
			}
			else if(pTtaBraceElem->IS%10==3)	//ʶ��X��(ǰ��)�Գ�
			{
				pMirTtaBraceElem=listTtaBraceElems.append(*pTtaBraceElem);
				int mir_i=3;
				//if(pTtaBraceElem->pStart->pos.y==0&&pTtaBraceElem->pEnd->pos.y==0)
				//	continue;	//�޷�����ǰ��(X)�Գ�ʱ
				pMirTtaBraceElem->pStart=AddOrUpdateMirTtaNode(pTtaBraceElem->pStart,mir_i);
				pMirTtaBraceElem->pEnd=AddOrUpdateMirTtaNode(pTtaBraceElem->pEnd,mir_i);
				mapOnlyBraceElems.SetValue(pMirTtaBraceElem->pStart->pointI,pMirTtaBraceElem->pEnd->pointI,pMirTtaBraceElem);
			}
			if(ReadEffectLineText(sText,100,fp)==NULL)
				break;
		}while(sText[0]!=0);
#ifdef __TIMER_COUNT_
		timer.Relay(5);
#endif
	}
	else if(ciFileType==2)
	{	//SmartTower�ļ���ȡ������͸���������
#ifdef __TIMER_COUNT_
		timer.Relay();
#endif
		DisplayProcess(100,"����������ģ�͵������");
		//��������Ϣ
		if(WIND%10==3)
		{
			int nWindBetaZDataRec=0;
			sscanf(sText,"%d",&nWindBetaZDataRec);
			for(i=0;i<3;i++)
			{	//����NFACE�л�ͼ����
				ReadLineText(sText,100,fp);
				//sscanf(sText,"%d%d%d%d%d%d%d",&pTtaNode->pointI,&pTtaNode->IS,&J1,&J2,&J3,&J4,&cType);
			}
		}
		if(!ReadDatBraceElemsDataFile(fp,mapOnlyBraceElems,sText))
		{	//DAT�ļ������и�����͸�������Ϣ����DAT2�ļ��л�ȡ
			szST_IA2_file=szDataFilePathName;
			szST_IA2_file.Append("2");
			if((fpTmp=fopen(szST_IA2_file,"rt"))!=NULL)
			{
				ReadDatBraceElemsDataFile(fpTmp,mapOnlyBraceElems);
				fclose(fpTmp);
			}
		}
	}
	else if(ciFileType==3)
	{	//��ȡ����Ժ�ĸ��ļ����ļ�
		UINT uiMaxPointI=0;
		long hMinCanUseHandle=Ta.GetMinAvailableHandle();
		for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			uiMaxPointI=max(uiMaxPointI,pNode->point_i);
		uiMaxPointI-=uiMaxPointI%10;
		if(uiMaxPointI<10000)
			uiMaxPointI=10000;

		CXhChar200 szFilePath=szDataFilePathName;
		_splitpath(szFilePath,NULL,NULL,NULL,NULL);
		char* pchSlash=SearchChar(szFilePath,'\\',true);
		if(pchSlash==NULL)
			pchSlash=SearchChar(szFilePath,'/',true);
		if(pchSlash)
			*pchSlash=0;
		DisplayProcess(0,"��ȡ���ļ����ļ�...");
		int nCount=listTtaAssElems.GetNodeNum();
		bool bBraceElemFileInFolder_FZC=false;
		for(int i=0;i<nCount;i++)
		{
			CXhChar50 _szTitle("��ȡ���ļ����ļ�%d/%d...",i+1,nCount);
			DisplayProcess(ftoi(i*100/nCount),_szTitle);
			TTA_ASSISTELEM assElem=listTtaAssElems[i];
			CLDSNode ldsnode;
			ldsnode.layer(0)='S';
			ldsnode.cfgword.Clear();
			for(int iKey=0;iKey<4;iKey++)
			{
				TTA_NODE* pKeyNode=hashTtaNodeI.GetValue(assElem.iKeyNode[iKey]);
				if(pKeyNode==NULL || pKeyNode->pNode==NULL)
					continue;
				if(ldsnode.cfgword.IsNull())
				{
					ldsnode.layer(0)=pKeyNode->pNode->layer(0);
					ldsnode.cfgword=pKeyNode->pNode->UnifiedCfgword();
				}
				if(pKeyNode->pNode->layer(0)=='S')
					ldsnode.cfgword&=pKeyNode->pNode->UnifiedCfgword();
				else if(pKeyNode->pNode->IsLegObj())
				{
					ldsnode.layer(0)=pKeyNode->pNode->layer(0);
					ldsnode.cfgword=pKeyNode->pNode->cfgword;
					break;
				}
			}
			CTtaDataFile bracetta;
			CXhChar200 szFileName("%s\\%d",(char*)szFilePath,assElem.index);
			if(!bracetta.ReadTANSBraceElemsDataFile(szFileName,&ldsnode))
			{
				szFileName.Printf("%s\\FZC\\%d",(char*)szFilePath,assElem.index);
				if(bracetta.ReadTANSBraceElemsDataFile(szFileName,&ldsnode))
					bBraceElemFileInFolder_FZC=true;
			}
		}
		//
		CLDSNode *pBraceNode,*pMergeToNode=NULL;
		for(pBraceNode=Ta.Node.GetFirst();pBraceNode;pBraceNode=Ta.Node.GetNext())
		{	//���Ľڵ�һ�㲻Ӧ�����κ������㣬���������������������ƫ�Ӧ���Ȱ������ʶ��ʧ���򰴱�����ʶ��
			if(pBraceNode->handle<hMinCanUseHandle)
				continue;	//�ɽڵ�
			if(pBraceNode->pointI<uiMaxPointI)
			{	//��������Ӹ�����ʱ���ļ��Žڵ���п������ظ���Ҫ���·����� wjh-2018.6.1
				uiMaxPointI+=10;
				pBraceNode->point_i=uiMaxPointI+pBraceNode->point_i%10;
			}
			CAtomListStack<CLDSNode> stack(&Ta.Node);
			//BOOL pushed=Ta.Node.push_stack();
			//1.0)�ϲ��ظ��ڵ�
			pMergeToNode=NULL;
			for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{
				if(pBraceNode==pNode)
					break;
				if(!pNode->ModuleTogetherWith(pBraceNode))
					continue;
				GEPOINT distvec=pNode->xOriginalPos-pBraceNode->xOriginalPos;
				if(fabs(distvec.x)<1&&fabs(distvec.y)<1&&fabs(distvec.z)<1)
				{
					pMergeToNode=pNode;
					break;
				}
			}
			if(pMergeToNode)
			{
				if(pMergeToNode->pointI==0&&pBraceNode->pointI>0)
					pMergeToNode->point_i=pBraceNode->pointI;
				if(!MergeOverlapNodes(pMergeToNode,pBraceNode))
					logerr.Log("�ڵ�0x%X{%d��}��0x%X{%d��}λ�ýӽ������ϲ�ʧ��",pMergeToNode->handle,pMergeToNode->pointI,pBraceNode->handle,pBraceNode->pointI);
				else
				{
					console.DeleteNode(pBraceNode->handle);
					g_pSolidDraw->DelEnt(pBraceNode->handle);
					pBraceNode=pMergeToNode;
				}
			}
			//1.1)�鲢�����������ڵ㣬����һ�㶼�������ڵ�
			if(pBraceNode->m_cPosCalType==CLDSNode::COORD_INDEPENDENT)
			{
				bool recognized=false;
				pBraceNode->m_cPosCalType=CLDSNode::COORD_INTERSECTION;
				if(!(recognized=pBraceNode->SearchRelaDependNodeRods()))
				{
					pBraceNode->m_cPosCalType=CLDSNode::COORD_SCALE;
					recognized=pBraceNode->SearchRelaDependNodeRods();
				}
				if(!recognized)	//��������ʧ�ܣ���ԭ�������κ�������
					pBraceNode->m_cPosCalType=CLDSNode::COORD_INDEPENDENT;
			}
			//Ta.Node.pop_stack(pushed);
		}
		//2.0)�ϲ����ศ�ĸ˼�
		for(CLDSLinePart* pBraceRod=Ta.EnumRodFirst();pBraceRod;pBraceRod=Ta.EnumRodNext())
		{
			if(pBraceRod->handle<hMinCanUseHandle)
				continue;	//������ģ�������������ĸ˼�
			CPtInLineCheck lineCheck(pBraceRod->pStart->xOriginalPos,pBraceRod->pEnd->xOriginalPos);
			//CLDSLinePart* pColinearRod=NULL;
			CLdsPartListStack stack(&Ta);
			for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
			{
				if(pRod->handle>=hMinCanUseHandle)
					break;	//��������׷�Ӹ����ģ�˵�����������������
				if(!pRod->ModuleTogetherWith(pBraceRod->pStart)||!pRod->ModuleTogetherWith(pBraceRod->pEnd))
					continue;
				bool startOnRod=false,endOnRod=false;
				double scale_start=lineCheck.CheckPointEx(pRod->pStart->xOriginalPos,&startOnRod);
				double scale_end=lineCheck.CheckPointEx(pRod->pEnd->xOriginalPos,&endOnRod);
				if(!startOnRod||!endOnRod)
					continue;	//���˼�������
				if((scale_start<EPS&&scale_end<EPS)||(scale_start>1+EPS&&scale_end>1+EPS))
					continue;	//���˼����ߵ����ص�
				//pColinearRod=pRod;
				logerr.Log("�˼�{%d-%d}��{%d-%d}λ���ص����ѽ��кϲ�����",pBraceRod->pStart->pointI,pBraceRod->pEnd->pointI,
					pRod->pStart->pointI,pRod->pEnd->pointI);
				console.DeletePart(pBraceRod->handle);
				break;
			}
		}
		Ta.AddMirRelativeObjInfo();
		DisplayProcess(100,"��ȡ���ļ����ļ�...");
	}
	else if(ciFileType==FILETYPE_GTOM)
	{	//��ȡ�㶫Ժ�ĸ��ļ����ļ�
		UINT uiMaxPointI=0;
		long hMinCanUseHandle=Ta.GetMinAvailableHandle();
		for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			uiMaxPointI=max(uiMaxPointI,pNode->point_i);
		uiMaxPointI-=uiMaxPointI%10;
		if(uiMaxPointI<10000)
			uiMaxPointI=10000;

		CXhChar200 szFilePath=szDataFilePathName;
		_splitpath(szFilePath,NULL,NULL,NULL,NULL);
		char* pchSlash=SearchChar(szFilePath,'\\',true);
		if(pchSlash==NULL)
			pchSlash=SearchChar(szFilePath,'/',true);
		if(pchSlash)
			*pchSlash=0;
		DisplayProcess(0,"��ȡ���ļ����ļ�...");
		HANDLE hFindFile;
		WIN32_FIND_DATA xFindFileData;
		CXhChar200 szFilePathBdyFZC("%s\\FZC-Body\\*.",(char*)szFilePath);
		CXhChar200 szFilePathLegFZC("%s\\FZC-Leg\\*.0",(char*)szFilePath);
		//��ȡ��������Ƭ�ļ�
		CXhSimpleList<GTM_FZC_FNAME> listFzcPanelFiles;
		if((hFindFile=FindFirstFile(szFilePathBdyFZC, &xFindFileData))!=INVALID_HANDLE_VALUE)
		{
			do {
				CXhChar500 szFzcPath("%s\\FZC-Body\\%s",(char*)szFilePath,xFindFileData.cFileName);
				GTM_FZC_FNAME fzcfile(szFzcPath,0);
				if (!TestValidGTowerBodyFzcFileName(xFindFileData.cFileName,&fzcfile))
					continue;
				listFzcPanelFiles.AttachObject(fzcfile);
				logerr.Log("Fzc File#%s",xFindFileData.cFileName);
			} while (FindNextFile(hFindFile,&xFindFileData));
			FindClose(hFindFile);
		}
		//��ȡ���ȸ�����Ƭ�ļ�
		if((hFindFile=FindFirstFile(szFilePathLegFZC, &xFindFileData))!=INVALID_HANDLE_VALUE)
		{
			do {
				CXhChar500 szFzcPath("%s\\FZC-Leg\\%s",(char*)szFilePath,xFindFileData.cFileName);
				GTM_FZC_FNAME fzcfile(szFzcPath,1);
				if (!TestValidGTowerLegFzcFileName(xFindFileData.cFileName,&fzcfile))
					continue;
				listFzcPanelFiles.AttachObject(fzcfile);
				logerr.Log("Fzc File-%d#%s",fzcfile.ciFzcPanelType,xFindFileData.cFileName);
			} while (FindNextFile(hFindFile,&xFindFileData));
			FindClose(hFindFile);
		}
		i=0;
		int niCount=listFzcPanelFiles.Count;
		for(GTM_FZC_FNAME* pFzcPanel=listFzcPanelFiles.EnumObjectFirst();pFzcPanel!=NULL;
			pFzcPanel=listFzcPanelFiles.EnumObjectNext(),i++)
		{
			i=1;
			CXhChar50 _szTitle("��ȡ���ļ����ļ�%d/%d...",i+1,niCount);
			DisplayProcess(ftoi(i*100/niCount),_szTitle);
			CLDSNodePtr xarrKeyNodes[4]={NULL};
			if (pFzcPanel->ciFzcPanelType==0)
			{	//��������ؼ��ڵ�
				xarrKeyNodes[0]=Ta.FromPointI(pFzcPanel->liPointA);
				xarrKeyNodes[1]=Ta.FromPointI(pFzcPanel->liPointB);
			}
			else
			{

			}
			CLDSNode ldsnode;
			ldsnode.layer(0)='S';
			ldsnode.cfgword.Clear();
			/*for(int iKey=0;iKey<4;iKey++)
			{
				TTA_NODE* pKeyNode=hashTtaNodeI.GetValue(assElem.iKeyNode[iKey]);
				if(pKeyNode==NULL || pKeyNode->pNode==NULL)
					continue;
				if(ldsnode.cfgword.IsNull())
				{
					ldsnode.layer(0)=pKeyNode->pNode->layer(0);
					ldsnode.cfgword=pKeyNode->pNode->UnifiedCfgword();
				}
				if(pKeyNode->pNode->layer(0)=='S')
					ldsnode.cfgword&=pKeyNode->pNode->UnifiedCfgword();
				else if(pKeyNode->pNode->IsLegObj())
				{
					ldsnode.layer(0)=pKeyNode->pNode->layer(0);
					ldsnode.cfgword=pKeyNode->pNode->cfgword;
					break;
				}
			}*/
			//CTtaDataFile bracetta;
			//if (!bracetta.ReadSubBraceElemsDataFile(pFzcPanel->fname,xarrKeyNodes,2,FILETYPE_GTOM))
			//	logerr.Log("error");
		}
		//
		CLDSNode *pBraceNode,*pMergeToNode=NULL;
		for(pBraceNode=Ta.Node.GetFirst();pBraceNode;pBraceNode=Ta.Node.GetNext())
		{	//���Ľڵ�һ�㲻Ӧ�����κ������㣬���������������������ƫ�Ӧ���Ȱ������ʶ��ʧ���򰴱�����ʶ��
			if(pBraceNode->handle<hMinCanUseHandle)
				continue;	//�ɽڵ�
			if(pBraceNode->pointI<uiMaxPointI)
			{	//��������Ӹ�����ʱ���ļ��Žڵ���п������ظ���Ҫ���·����� wjh-2018.6.1
				uiMaxPointI+=10;
				pBraceNode->point_i=uiMaxPointI+pBraceNode->point_i%10;
			}
			CAtomListStack<CLDSNode> stack(&Ta.Node);
			//BOOL pushed=Ta.Node.push_stack();
			//1.0)�ϲ��ظ��ڵ�
			pMergeToNode=NULL;
			for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{
				if(pBraceNode==pNode)
					break;
				if(!pNode->ModuleTogetherWith(pBraceNode))
					continue;
				GEPOINT distvec=pNode->xOriginalPos-pBraceNode->xOriginalPos;
				if(fabs(distvec.x)<1&&fabs(distvec.y)<1&&fabs(distvec.z)<1)
				{
					pMergeToNode=pNode;
					break;
				}
			}
			if(pMergeToNode)
			{
				if(pMergeToNode->pointI==0&&pBraceNode->pointI>0)
					pMergeToNode->point_i=pBraceNode->pointI;
				if(!MergeOverlapNodes(pMergeToNode,pBraceNode))
					logerr.Log("�ڵ�0x%X{%d��}��0x%X{%d��}λ�ýӽ������ϲ�ʧ��",pMergeToNode->handle,pMergeToNode->pointI,pBraceNode->handle,pBraceNode->pointI);
				else
				{
					console.DeleteNode(pBraceNode->handle);
					g_pSolidDraw->DelEnt(pBraceNode->handle);
					pBraceNode=pMergeToNode;
				}
			}
			//1.1)�鲢�����������ڵ㣬����һ�㶼�������ڵ�
			if(pBraceNode->m_cPosCalType==CLDSNode::COORD_INDEPENDENT)
			{
				bool recognized=false;
				pBraceNode->m_cPosCalType=CLDSNode::COORD_INTERSECTION;
				if(!(recognized=pBraceNode->SearchRelaDependNodeRods()))
				{
					pBraceNode->m_cPosCalType=CLDSNode::COORD_SCALE;
					recognized=pBraceNode->SearchRelaDependNodeRods();
				}
				if(!recognized)	//��������ʧ�ܣ���ԭ�������κ�������
					pBraceNode->m_cPosCalType=CLDSNode::COORD_INDEPENDENT;
			}
			//Ta.Node.pop_stack(pushed);
		}
		//2.0)�ϲ����ศ�ĸ˼�
		for(CLDSLinePart* pBraceRod=Ta.EnumRodFirst();pBraceRod;pBraceRod=Ta.EnumRodNext())
		{
			if(pBraceRod->handle<hMinCanUseHandle)
				continue;	//������ģ�������������ĸ˼�
			CPtInLineCheck lineCheck(pBraceRod->pStart->xOriginalPos,pBraceRod->pEnd->xOriginalPos);
			//CLDSLinePart* pColinearRod=NULL;
			CLdsPartListStack stack(&Ta);
			for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
			{
				if(pRod->handle>=hMinCanUseHandle)
					break;	//��������׷�Ӹ����ģ�˵�����������������
				if(!pRod->ModuleTogetherWith(pBraceRod->pStart)||!pRod->ModuleTogetherWith(pBraceRod->pEnd))
					continue;
				bool startOnRod=false,endOnRod=false;
				double scale_start=lineCheck.CheckPointEx(pRod->pStart->xOriginalPos,&startOnRod);
				double scale_end=lineCheck.CheckPointEx(pRod->pEnd->xOriginalPos,&endOnRod);
				if(!startOnRod||!endOnRod)
					continue;	//���˼�������
				if((scale_start<EPS&&scale_end<EPS)||(scale_start>1+EPS&&scale_end>1+EPS))
					continue;	//���˼����ߵ����ص�
				//pColinearRod=pRod;
				logerr.Log("�˼�{%d-%d}��{%d-%d}λ���ص����ѽ��кϲ�����",pBraceRod->pStart->pointI,pBraceRod->pEnd->pointI,
					pRod->pStart->pointI,pRod->pEnd->pointI);
				console.DeletePart(pBraceRod->handle);
				break;
			}
		}
		Ta.AddMirRelativeObjInfo();
		DisplayProcess(100,"��ȡ���ļ����ļ�...");
	}
	//�ؽ����ļ��˽ڵ�
	m_nOverlapBraceNodes=0;	//ĿǰBuildLdsGeoModel����֧����ǰ�ϲ��ص����Ľڵ㣬�ʴ˴������� wjh-2018.7.5
	BuildLdsGeoModel(&hashOnlyBraceNodes,&mapOnlyBraceElems);
	TTA_NODE* pBraceTtaNode;
	CLDSNode *pBraceNode,*pMergeToNode=NULL;
	//1.0)�鲢'b'�������������ڵ㣬����һ�㶼�������ڵ�
	
	int count=hashOnlyBraceNodes.GetNodeNum();
	for(pBraceTtaNode=hashOnlyBraceNodes.GetFirst(),i=0;pBraceTtaNode;pBraceTtaNode=hashOnlyBraceNodes.GetNext(),i++)
	{	//���Ľڵ�һ�㲻Ӧ�����κ������㣬���������������������ƫ�Ӧ���Ȱ������ʶ��ʧ���򰴱�����ʶ��
		pBraceNode=pBraceTtaNode->pNode;
		if(pBraceNode==NULL)
		{
			logerr.Log("error");
			continue;
		}
		if(pBraceNode->m_cPosCalType==CLDSNode::COORD_INDEPENDENT)
		{
			bool recognized=false;
			DisplayProcess(i*100/count,"����ʶ������(����)�˽ڵ㶨λ������Ϣ");
			pBraceNode->m_cPosCalType=CLDSNode::COORD_INTERSECTION;
			if(!(recognized=pBraceNode->SearchRelaDependNodeRods()))
			{
				pBraceNode->m_cPosCalType=CLDSNode::COORD_SCALE;
				recognized=pBraceNode->SearchRelaDependNodeRods();
			}
			if(!recognized)	//��������ʧ�ܣ���ԭ�������κ�������
				pBraceNode->m_cPosCalType=CLDSNode::COORD_INDEPENDENT;
		}
	}
	DisplayProcess(100,"����ʶ������(����)�˽ڵ㶨λ������Ϣ");
#ifdef __TIMER_COUNT_
	DWORD dwStartTicks=timer.Relay(6);
#endif
	if(m_nOverlapBraceNodes>0)
		logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"��⵽%d���ظ��ڵ㣬�ѽ��кϲ�����!",m_nOverlapBraceNodes);
#ifdef __TIMER_COUNT_
	timer.Relay(7,dwStartTicks);
#endif
	//2.0)�ϲ����ศ�ĸ˼�
	TTA_BRACEELEM** ppElem;
	count=mapOnlyBraceElems.GetNodeNum();
	int overlap_elem_hits=0;
	for(ppElem=mapOnlyBraceElems.GetFirst(),i=0;ppElem;ppElem=mapOnlyBraceElems.GetNext(),i++)
	{
		TTA_BRACEELEM* pBraceElem=*ppElem;
		if(pBraceElem==NULL)
			continue;
		CPtInLineCheck lineCheck(pBraceElem->pStart->pNode->xOriginalPos,pBraceElem->pEnd->pNode->xOriginalPos);
		SCOPE_STRU scope;
		scope.VerifyVertex(pBraceElem->pStart->pNode->xOriginalPos);
		scope.VerifyVertex(pBraceElem->pEnd->pNode->xOriginalPos);
		CLDSLinePart* pColinearRod=NULL;
		DisplayProcess(i*100/count,"��⼰�ϲ����ศ�ĸ˼�");
		for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		{
			if(mapOnlyBraceElems.GetValue(pRod->pStart->pointI,pRod->pEnd->pointI))
				break;	//�������ศ���ģ�˵�����������������
			if(!pRod->ModuleTogetherWith(pBraceElem->pStart->pNode)||!pRod->ModuleTogetherWith(pBraceElem->pEnd->pNode))
				continue;
			if(!scope.IsIntersWithLine(pRod->pStart->xOriginalPos,pRod->pEnd->xOriginalPos))
				continue;
			bool startOnRod=false,endOnRod=false;
			double scale_start=lineCheck.CheckPointEx(pRod->pStart->Position(false),&startOnRod);
			if(!startOnRod)
				continue;	//���˼�������
			double scale_end=lineCheck.CheckPointEx(pRod->pEnd->Position(false),&endOnRod);
			if(!endOnRod)
				continue;	//���˼�������
			if((scale_start<EPS&&scale_end<EPS)||(scale_start>1+EPS&&scale_end>1+EPS))
				continue;	//���˼����ߵ����ص�
			pColinearRod=pRod;
			break;
		}
		if(pColinearRod!=NULL&&pBraceElem->pRod&&pColinearRod!=pBraceElem->pRod)
		{
			console.DeletePart(pBraceElem->pRod->handle);
			mapOnlyBraceElems.DeleteCursor();
			overlap_elem_hits++;
		}
	}
	if(overlap_elem_hits>0)
		logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"��⵽%d���ظ��˼����ѽ��кϲ�����!",overlap_elem_hits);
	DisplayProcess(100,"��⼰�ϲ����ศ�ĸ˼�");
#ifdef __TIMER_COUNT_
	timer.Relay(8);
#endif
	//������˨���˼������Ϣ
	CXhChar200 outfile=szDataFilePathName;
	char* pszExtSpliter=SearchChar(outfile,'.',true);
	if(pszExtSpliter==NULL)
		pszExtSpliter=((char*)outfile)+outfile.Length;
	bool readPrimaryElemAndBoltSizeInfoFile=false;
	bool readBraceElemAndBoltSizeInfoFile=false;
	if(ciFileType==2)
	{	//��ȡ��DAT���׵������Ĺ����Ϣ�ļ�IA0 wxc -2018.7.22
		CMapList<TTA_ELEM*> mapElems;
		for(TTA_ELEM *pTtaElem=listTtaElems.GetFirst();pTtaElem;pTtaElem=listTtaElems.GetNext())
			mapElems.SetValue(pTtaElem->pStart->pointI,pTtaElem->pEnd->pointI,pTtaElem);
		CXhChar200 sIA0File=szDataFilePathName;
		char* szExt=SearchChar(sIA0File,'.',true);
		if(szExt)
			*szExt=0;
		sIA0File.Append(".IA0");
		if((fpTmp=fopen(sIA0File,"rt"))!=NULL)
		{
			readPrimaryElemAndBoltSizeInfoFile=ReadDatElemsSpecFile(fpTmp,&mapElems,NULL);
			fclose(fpTmp);
		}
		//if(szST_IA2_file.Length>0)
		{	//��ȡ��DAT���׵ĸ����Ĺ����Ϣ�ļ�IA2
			CXhChar200 ia2_file=sIA0File;
			char* szExt=SearchChar(ia2_file,'.',true);
			if(szExt)
				*szExt=0;
			ia2_file.Append(".IA2");
			if((fpTmp=fopen(ia2_file,"rt"))!=NULL)
			{
				readBraceElemAndBoltSizeInfoFile=ReadDatElemsSpecFile(fpTmp,NULL,&mapOnlyBraceElems);
				fclose(fpTmp);
			}
		}
		if(readPrimaryElemAndBoltSizeInfoFile||readBraceElemAndBoltSizeInfoFile)
		{
			for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
			{
				if(pRod->pStart==NULL||pRod->pEnd==NULL)
					continue;
				TTA_ELEM** ppTtaElem=mapElems.GetValue(pRod->pStart->pointI,pRod->pEnd->pointI);
				if(ppTtaElem==NULL)
					ppTtaElem=mapElems.GetValue(pRod->pEnd->pointI,pRod->pStart->pointI);
				if(ppTtaElem!=NULL)
				{
					pTtaElem=*ppTtaElem;
					if(!pTtaElem->bReadSizeSpec)
						continue;	//��ԭʼ����Ԫ��δ��ȡ�������Ϣ
					pRod->size_idClassType=pTtaElem->size.idClassType;//CLS_LINETUBE;
					pRod->cMaterial=pTtaElem->cMaterial;
					pRod->connectStart.d=pRod->connectEnd.d=pTtaElem->boltd;
					pRod->connectStart.wnSummBoltN=pRod->connectEnd.wnSummBoltN=pTtaElem->boltN;
					if(pTtaElem->boltgrade>0)
					{
						_snprintf(pRod->connectStart.grade,4,"%.1f",pTtaElem->boltgrade);
						strcpy(pRod->connectEnd.grade,pRod->connectStart.grade);
					}
					if(pRod->size_idClassType==CLS_LINEANGLE||pRod->size_idClassType==CLS_GROUPLINEANGLE)
					{
						int nSubAngles=ftoi(pTtaElem->size.wide)/1000;
						pRod->size_wide=ftoi(pTtaElem->size.wide)%1000;
						if(nSubAngles<2)
							pRod->size_idClassType=CLS_LINEANGLE;
						else if(nSubAngles==2)
						{
							pRod->size_idClassType=CLS_GROUPLINEANGLE;
							if(pRod->size_idClassType=='L')
								pRod->size_cSubClassType='D';
						}
						else if(nSubAngles==3)
						{	//����Ժϰ����3��ʾT����ϽǸ�
							pRod->size_idClassType=CLS_GROUPLINEANGLE;
							if(pRod->size_idClassType=='L')
								pRod->size_cSubClassType='T';
						}
						else if(nSubAngles==4)
						{
							pRod->size_idClassType=CLS_GROUPLINEANGLE;
							pRod->size_cSubClassType='X';
						}
					}
					else
						pRod->size_wide=pTtaElem->size.wide;
					pRod->size_thick=pTtaElem->size.thick;
					//TODO:��ǿ��ʵ��Ӧ�ӵ���Q460Q235.ini�ļ��ж�ȡ�򻯷��ţ�Ŀǰ��ʱд�� wjh-2017.5.12
					if(QuerySteelMatByBriefMark(pTtaElem->cMaterial)!=NULL)
						pRod->cMaterial=pTtaElem->cMaterial;
					else if(pTtaElem->cMaterial=='A')
						pRod->cMaterial=CSteelMatLibrary::Q390BriefMark();
					else if(pTtaElem->cMaterial=='B')
						pRod->cMaterial=CSteelMatLibrary::Q420BriefMark();
					else if(pTtaElem->cMaterial=='C')
						pRod->cMaterial=CSteelMatLibrary::Q460BriefMark();
					pRod->SyncMirProp(NULL);
				}
				else
				{
					TTA_BRACEELEM** ppTtaBraceElem=mapOnlyBraceElems.GetValue(pRod->pStart->pointI,pRod->pEnd->pointI);
					if(ppTtaBraceElem==NULL)
						ppTtaBraceElem=mapOnlyBraceElems.GetValue(pRod->pEnd->pointI,pRod->pStart->pointI);
					if(ppTtaBraceElem!=NULL)
					{
						TTA_BRACEELEM* pTtaBraceElem=*ppTtaBraceElem;
						if(!pTtaBraceElem->bReadSizeSpec)
							continue;	//��ԭʼ����Ԫ��δ��ȡ�������Ϣ
						if(pTtaBraceElem->cSectType=='R')
							pRod->size_idClassType=CLS_LINETUBE;
						else
							pRod->size_idClassType=CLS_LINEANGLE;
						pRod->cMaterial=pTtaElem->cMaterial;
						pRod->connectStart.d=pRod->connectEnd.d=pTtaBraceElem->boltd;
						pRod->connectStart.wnSummBoltN=pRod->connectEnd.wnSummBoltN=pTtaBraceElem->boltN;
						_snprintf(pRod->connectStart.grade,4,"%.1f",pTtaBraceElem->boltgrade);
						strcpy(pRod->connectEnd.grade,pRod->connectStart.grade);
						if(pRod->size_idClassType==CLS_LINEANGLE||pRod->size_idClassType==CLS_GROUPLINEANGLE)
						{
							int nSubAngles=ftoi(pTtaBraceElem->size_wide)/1000;
							pRod->size_wide=ftoi(pTtaBraceElem->size_wide)%1000;
							if(nSubAngles<2)
								pRod->size_idClassType=CLS_LINEANGLE;
							else if(nSubAngles==2)
							{
								pRod->size_idClassType=CLS_GROUPLINEANGLE;
								if(pRod->size_idClassType=='L')
									pRod->size_cSubClassType='D';
							}
							else if(nSubAngles==3)
							{	//����Ժϰ����3��ʾT����ϽǸ�
								pRod->size_idClassType=CLS_GROUPLINEANGLE;
								if(pRod->size_idClassType=='L')
									pRod->size_cSubClassType='T';
							}
							else if(nSubAngles==4)
							{
								pRod->size_idClassType=CLS_GROUPLINEANGLE;
								pRod->size_cSubClassType='X';
							}
						}
						else
							pRod->size_wide=pTtaBraceElem->size_wide;
						pRod->size_thick=pTtaBraceElem->size_thick;
						//TODO:��ǿ��ʵ��Ӧ�ӵ���Q460Q235.ini�ļ��ж�ȡ�򻯷��ţ�Ŀǰ��ʱд�� wjh-2017.5.12
						if(QuerySteelMatByBriefMark(pTtaBraceElem->cBriefMaterial)!=NULL)
							pRod->cMaterial=pTtaBraceElem->cBriefMaterial;
						else if(pTtaBraceElem->cBriefMaterial=='A')
							pRod->cMaterial=CSteelMatLibrary::Q390BriefMark();
						else if(pTtaBraceElem->cBriefMaterial=='B')
							pRod->cMaterial=CSteelMatLibrary::Q420BriefMark();
						else if(pTtaBraceElem->cBriefMaterial=='C')
							pRod->cMaterial=CSteelMatLibrary::Q460BriefMark();
						pRod->SyncMirProp(NULL);
					}
				}
			}
		}
		if(!readPrimaryElemAndBoltSizeInfoFile)
		{
			strcpy(pszExtSpliter,".out");
			if((fpTmp=fopen(outfile,"rt"))!=NULL)
			{
				ReadOutFile(fpTmp,NULL,NULL,ciFileType);
				fclose(fpTmp);
			}
		}
		if(!readBraceElemAndBoltSizeInfoFile)
		{
			strcpy(pszExtSpliter,".out2");
			if((fpTmp=fopen(outfile,"rt"))!=NULL)
			{
				ReadDaoHengAuxElemOutFile(fpTmp,NULL,ciFileType);
				fclose(fpTmp);
			}
		}
	}
	else if(ciFileType==1)
	{	//��ȡ�����ļ������ļ�
		outfile.Append(".out");
		fpTmp=fopen(outfile,"rt");
		if(fpTmp==NULL&&pszExtSpliter!=NULL)	//&&dotIndex>0
		{	//��ʱ������ļ��в���".myl"
			*pszExtSpliter=0;
			outfile.Append(".out");
			fpTmp=fopen(outfile,"rt");
		}
		if(fpTmp!=NULL)
		{	//���ศ�����
			ReadOutFile(fpTmp,NULL,NULL,ciFileType);
			fclose(fpTmp);
		}
		//��ȡ�����ļ������ļ�
		CString outfile2=outfile;
		outfile2.Append("2");//dlg2.GetPathName()+".out2";
		if((fpTmp=fopen(outfile2,"rt"))!=NULL)
		{
			ReadDaoHengAuxElemOutFile(fpTmp,NULL,ciFileType);
			fclose(fpTmp);
		}
	}
	else //if(ciFileType!=1)
	{
		strcpy(pszExtSpliter,".out");
		if((fpTmp=fopen(outfile,"rt"))!=NULL)
		{
			ReadOutFile(fpTmp,NULL,NULL,ciFileType);
			fclose(fpTmp);
		}
	}
	//Ta.AddMirRelativeObjInfo();	//����Գƹ�����Ϣ
	//timer.Relay(3);
}
bool CTtaDataFile::ReadDatBraceElemsDataFile(FILE* fp,CMapList<TTA_BRACEELEM*>& mapOnlyBraceElems,const char* sKeyText/*=NULL*/)
{
	if(fp==NULL)
		return false;
	char sText[101]="";
	BOOL bHasBraceElemData=FALSE;
	if(sKeyText)
		StrCopy(sText,sKeyText,101);
	else
		ReadEffectLineText(sText,100,fp);
	while(strlen(sText)>0)
	{
		if(strstr(sText,"���������Ŀ"))
		{
			bHasBraceElemData=TRUE;
			break;
		}
		if(ReadEffectLineText(sText,100,fp)==NULL)
			return false;	//�����ж�
	}
	if(!bHasBraceElemData)
		return false;
	int i=0,nBraceNode=0;
	sscanf(sText,"%d",&nBraceNode);
	TTA_NODE* pTtaNode=NULL,*pMirTtaNode=NULL;
	DYN_ARRAY<TTA_NODE*> braceNodeArr(nBraceNode);
	for(i=0;i<nBraceNode;i++)
	{	//����NFACE�л�ͼ����
		braceNodeArr[i]=pTtaNode=listTtaNodes.append();
		pTtaNode->bBraceNode=true;	//���Ľڵ�
		long J1=0,J2=0,J3=0,J4=0,cType=0;
		ReadLineText(sText,100,fp);
		sscanf(sText,"%d%d%d%d%d%d%d",&pTtaNode->pointI,&pTtaNode->IS,&J1,&J2,&J3,&J4,&cType);
		if(cType==0)
		{	//�����ȷֵ�
			pTtaNode->posType=4;
			pTtaNode->J1=J1;
			pTtaNode->J2=J2;
			double fV1=J3,fV2=(J3+J4);
			pTtaNode->coefScale=fV1/fV2;
		}
		else if(cType==1)
		{	//�����
			pTtaNode->posType=5;
			pTtaNode->J1=J1;
			pTtaNode->J2=J2;
			pTtaNode->J3=J3;
			pTtaNode->J4=J4;
		}
		//��ӶԳƵ�
		hashOnlyBraceNodes.SetValue(pTtaNode->pointI,pTtaNode);
		hashTtaNodeI.SetValue(pTtaNode->pointI,pTtaNode);
		if(pTtaNode->IS==1)	//Y��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(2));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,2);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			hashOnlyBraceNodes.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
		else if(pTtaNode->IS==2)	//X��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(1));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,1);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			hashOnlyBraceNodes.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
		else if(pTtaNode->IS==3)	//Z��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(3));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,3);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			hashOnlyBraceNodes.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
		else if(pTtaNode->IS==4)	//XYZ��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(1));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,1);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			hashOnlyBraceNodes.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(2));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,2);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			hashOnlyBraceNodes.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(3));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,3);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			hashOnlyBraceNodes.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
	}
	//���µ����꼰��ĺ�
	for(i=0;i<nBraceNode;i++)
	{
		pTtaNode=braceNodeArr[i];
		pTtaNode->UpdatePosition(this);
	}
	//���ݸ��ڵ�����ӽڵ����꼰�������ͣ�����AddOrUpdateMirTtaNode���жϳ��� wjh-2019.10.18
	for (pTtaNode=listTtaNodes.GetFirst();pTtaNode;pTtaNode=listTtaNodes.GetNext())
	{
		if (pTtaNode->bBraceNode)
			pTtaNode->UpdateBasicPropFromParentNode();
	}
	for(i=0;i<nBraceNode;i++)
	{
		pTtaNode=braceNodeArr[i];
		if(pTtaNode->IS>0&&pTtaNode->IS<4)	//Y��Գ�
			AddOrUpdateMirTtaNode(pTtaNode,pTtaNode->IS);
		else if(pTtaNode->IS==4)
		{
			AddOrUpdateMirTtaNode(pTtaNode,1);
			AddOrUpdateMirTtaNode(pTtaNode,2);
			AddOrUpdateMirTtaNode(pTtaNode,3);
		}
	}
	//������ĺ�
	for(i=0;i<nBraceNode;i++)
	{
		pTtaNode=braceNodeArr[i];
		ValidateBraceNodeCfgWord(pTtaNode,true);
		hashParentTtaNodeI.SetValue(pTtaNode->pointI,true);
		TTA_NODE* pMirTtaNode=NULL;
		if(pTtaNode->IS==1)		//���ҶԳ�
			pMirTtaNode=hashTtaNodeI.GetValue(MirPointI(pTtaNode->pointI,MIRMSG(2)));
		else if(pTtaNode->IS==2)//ǰ��Գ�
			pMirTtaNode=hashTtaNodeI.GetValue(MirPointI(pTtaNode->pointI,MIRMSG(1)));
		else if(pTtaNode->IS==3)//Z��Գ�
			pMirTtaNode=hashTtaNodeI.GetValue(MirPointI(pTtaNode->pointI,MIRMSG(3)));
		if(pMirTtaNode)
		{
			pMirTtaNode->cfgword=pTtaNode->cfgword;
			pMirTtaNode->cPosLayer=pTtaNode->cPosLayer;
		}
		if(pTtaNode->IS==4)
		{	//�ĶȶԳ�
			for(int j=1;j<4;j++)
			{
				if((pMirTtaNode=hashTtaNodeI.GetValue(MirPointI(pTtaNode->pointI,MIRMSG(j))))!=NULL)
				{
					pMirTtaNode->cfgword=pTtaNode->cfgword;
					pMirTtaNode->cPosLayer=pTtaNode->cPosLayer;
				}
			}
		}
	}
	for(pTtaNode=hashOnlyBraceNodes.GetFirst();pTtaNode;pTtaNode=hashOnlyBraceNodes.GetNext())
		pTtaNode->UpdatePosition(this);
	//�����ĸ˼�
	if(ReadEffectLineText(sText,100,fp)==NULL)
		return false;
	int nBraceElem=0,start_i=0,end_i=0;
	sscanf(sText,"%d",&nBraceElem);
	for(i=0;i<nBraceElem;i++)
	{
		TTA_BRACEELEM *pTtaBraceElem=listTtaBraceElems.append();
		ReadLineText(sText,100,fp);
		CXhChar200 szRodText=sText;
		pTtaBraceElem->cSectType='L';	//����Ĭ��Ϊ�Ǹ�
		int TYPE=0;	//�˼����ͣ��˴���Ϊ����һ��Ӧ��5
		CXhChar16 szStatMat;
		char* key=strtok(szRodText," ");
		for(int j=0;key!=NULL&&j<16;j++)
		{
			if(j==0)
				start_i=atoi(key);
			else if(j==1)
				end_i=atoi(key);
			else if(j==2)
			{
				SEGI zStatMat(key);
				szStatMat.Copy(key);
				pTtaBraceElem->N=zStatMat.iSeg;
			}
			else if(j==3)
				pTtaBraceElem->IS=atoi(key);
			else if(j==4)
				TYPE=atoi(key);
			else if(j==5)
				pTtaBraceElem->M=atoi(key);
			else if(j==11)
			{
				double wide_thick=atof(key);
				pTtaBraceElem->size_wide=ftoi(wide_thick);
				wide_thick-=pTtaBraceElem->size_wide;
				//������(int)ֱ��ת�����ͣ���Ϊ����������50.04�п���Ϊ50.039999999999 wjh-2018.7.17
				pTtaBraceElem->size_thick=ftoi(wide_thick*100);
				pTtaBraceElem->cSectType='L';	//CLS_LINEANGLE;
				if(strchr(szStatMat,'R')!=NULL)
					pTtaBraceElem->cSectType='R';//CLS_LINETUBE;
				if((int)(pTtaBraceElem->size_wide/1000)==2)
				{
					if(strchr(szStatMat,'T')!=NULL)	//T����ϽǸ�
						pTtaBraceElem->cSectType='T';
					else
						pTtaBraceElem->cSectType='D';
				}
			}
			else if(j==12)
			{
				if(key[0]=='H')
					pTtaBraceElem->cBriefMaterial=CSteelMatLibrary::Q345BriefMark();
				else if(key[0]=='D')
					pTtaBraceElem->cBriefMaterial=CSteelMatLibrary::Q420BriefMark();
				else if(key[0]=='E')
					pTtaBraceElem->cBriefMaterial=CSteelMatLibrary::Q460BriefMark();
				else
					pTtaBraceElem->cBriefMaterial=CSteelMatLibrary::Q235BriefMark();
			}
			else if(j==13)
				pTtaBraceElem->boltd=atoi(key);//M20*2
			else if(j==14)
				pTtaBraceElem->boltN=atoi(key);//M20*2
			key=strtok(NULL," ");
		}
		if(hashTtaNodeI.GetValue(start_i)!=NULL)
			pTtaBraceElem->pStart=hashTtaNodeI[start_i];
		if(hashTtaNodeI.GetValue(end_i)!=NULL)
			pTtaBraceElem->pEnd=hashTtaNodeI[end_i];
		if(pTtaBraceElem->pStart==NULL||pTtaBraceElem->pEnd==NULL)
		{
			logerr.Log("�˼�[%d-%d]û�ж�Ӧ�Ľڵ�!",start_i,end_i);
			listTtaBraceElems.DeleteCursor(TRUE);
			continue;
		}
		mapOnlyBraceElems.SetValue(pTtaBraceElem->pStart->pointI,pTtaBraceElem->pEnd->pointI,pTtaBraceElem);
		//��ӶԳƸ�����
		TTA_BRACEELEM *pMirTtaBraceElem=NULL;
		if(pTtaBraceElem->IS%10>=4)	//ʶ��XYZ��Գ�
		{
			for(int i=1;i<4;i++)
			{
				pMirTtaBraceElem=listTtaBraceElems.append(*pTtaBraceElem);
				pMirTtaBraceElem->pStart=AddOrUpdateMirTtaNode(pTtaBraceElem->pStart,i);
				pMirTtaBraceElem->pEnd=AddOrUpdateMirTtaNode(pTtaBraceElem->pEnd,i);
				mapOnlyBraceElems.SetValue(pMirTtaBraceElem->pStart->pointI,pMirTtaBraceElem->pEnd->pointI,pMirTtaBraceElem);
			}
		}
		else if(pTtaBraceElem->IS%10==1)	//ʶ��Y��(����)�Գ�
		{
			pMirTtaBraceElem=listTtaBraceElems.append(*pTtaBraceElem);
			int mir_i=1;
			if(pTtaBraceElem->pStart->pos.x==0&&pTtaBraceElem->pEnd->pos.x==0)
				continue;	//�޷���������(Y)�Գ�ʱ
			pMirTtaBraceElem->pStart=AddOrUpdateMirTtaNode(pTtaBraceElem->pStart,mir_i);
			pMirTtaBraceElem->pEnd=AddOrUpdateMirTtaNode(pTtaBraceElem->pEnd,mir_i);
			mapOnlyBraceElems.SetValue(pMirTtaBraceElem->pStart->pointI,pMirTtaBraceElem->pEnd->pointI,pMirTtaBraceElem);
		}
		else if(pTtaBraceElem->IS%10==2)	//ʶ��X��(ǰ��)�Գ�
		{
			pMirTtaBraceElem=listTtaBraceElems.append(*pTtaBraceElem);
			int mir_i=2;
			if(pTtaBraceElem->pStart->pos.y==0&&pTtaBraceElem->pEnd->pos.y==0)
				continue;	//�޷�����ǰ��(X)�Գ�ʱ
			pMirTtaBraceElem->pStart=AddOrUpdateMirTtaNode(pTtaBraceElem->pStart,mir_i);
			pMirTtaBraceElem->pEnd=AddOrUpdateMirTtaNode(pTtaBraceElem->pEnd,mir_i);
			mapOnlyBraceElems.SetValue(pMirTtaBraceElem->pStart->pointI,pMirTtaBraceElem->pEnd->pointI,pMirTtaBraceElem);
		}
		else if(pTtaBraceElem->IS%10==3)	//ʶ��X��(ǰ��)�Գ�
		{
			pMirTtaBraceElem=listTtaBraceElems.append(*pTtaBraceElem);
			int mir_i=3;
			pMirTtaBraceElem->pStart=AddOrUpdateMirTtaNode(pTtaBraceElem->pStart,mir_i);
			pMirTtaBraceElem->pEnd=AddOrUpdateMirTtaNode(pTtaBraceElem->pEnd,mir_i);
			mapOnlyBraceElems.SetValue(pMirTtaBraceElem->pStart->pointI,pMirTtaBraceElem->pEnd->pointI,pMirTtaBraceElem);
		}
	}
	return true;
	//�����ļ�����Ϣ��
	//�Ժ������öԽӼ��㹦��ʱ �����øô��� wjh -2018.10.7
	//if(ReadEffectLineText(sText,100,fp)==NULL)
	//	return;
	//int nBraceCal=0;
	//sscanf(sText,"%d",&nBraceCal);
	//for(i=0;i<nBraceCal;i++)
	//	ReadLineText(sText,100,fp);	//����������������
	////��ͼ����
	//if(ReadEffectLineText(sText,100,fp)==NULL)
	//	return;
	//sscanf(sText,"%d%d%d",&NFACE,&LEGINDEX,&DRAW_FACTOR);
	//for(i=0;i<NFACE;i++)
	//{	//����NFACE�л�ͼ����
	//	if(ReadEffectLineText(sText,100,fp)==NULL)
	//		return;	//�����ж�
	//}
}
bool CTtaDataFile::ReadDatElemsSpecFile(FILE* fp,CMapList<TTA_ELEM*>* pMapElems,CMapList<TTA_BRACEELEM*>* pMapBraceElems)
{
	char sText[101]="";
	char ciValidOutFile=0;
	ReadEffectLineText(sText,100,fp);
	while(strlen(sText)>0)
	{
		if(strstr(sText,"Version"))
		{
			ciValidOutFile=1;//TRUE;	//����Ժ��ʽ*.ia0
			break;
		}
		if (strstr(sText,"MEMBER REPORT"))
		{
			ciValidOutFile=2;//TRUE;	//��Ӧ��MYL��ʽ*.ia0
			break;
		}
		if(ReadEffectLineText(sText,100,fp)==NULL)
			return false;	//�����ж�
	}
	if (ciValidOutFile==2)
	{	//��MYL������ļ���ʽ��������ļ�
		fseek(fp,0,SEEK_SET);
		ReadOutFile(fp,NULL,NULL,CTtaDataFile::FILETYPE_ST);
		return true;
	}
	if(!ciValidOutFile)
		return false;
	int nElemNum=0,indexS=0,indexE=0,iType=0,iMat=0;
	int size_wide=0,size_thick=0,boltD=0,boltN=0;
	sscanf(sText,"%d",&nElemNum);
	for(int i=0;i<nElemNum;i++)
	{
		ReadLineText(sText,100,fp);
		CXhChar200 szRodText=sText;
		char* key=strtok(szRodText," ");
		for(int j=0;j<6;j++)
		{
			if(j==0)
				indexS=atoi(key);
			else if(j==1)
				indexE=atoi(key);
			else if(j==2)
			{
				int nValue=atoi(key);
				iType=nValue%10;
				iMat=nValue/1000;
			}
			else if(j==3)
			{	//���
				double wide_thick=atof(key);
				size_wide=ftoi(wide_thick);
				wide_thick-=size_wide;
				size_thick=ftoi(wide_thick*100);
			}
			else if(j==4)
				boltD=atoi(key);//M20*2
			else if(j==5)
				boltN=atoi(key);//M20*2
			key=strtok(NULL," ");
		}
		if(pMapElems)
		{
			TTA_ELEM** ppTtaElem=pMapElems->GetValue(indexS,indexE);
			if(ppTtaElem==NULL)
				ppTtaElem=pMapElems->GetValue(indexE,indexS);
			if(ppTtaElem)
			{
				(*ppTtaElem)->bReadSizeSpec=true;
				(*ppTtaElem)->size.wide=size_wide;
				(*ppTtaElem)->size.thick=size_thick;
				if(iType==0)
					(*ppTtaElem)->size.idClassType=CLS_LINEANGLE;
				else
					(*ppTtaElem)->size.idClassType=CLS_LINETUBE;
				if(iMat==2)
					(*ppTtaElem)->cMaterial=CSteelMatLibrary::Q345BriefMark();
				else if(iMat==3)
					(*ppTtaElem)->cMaterial=CSteelMatLibrary::Q420BriefMark();
				else if(iMat==4)
					(*ppTtaElem)->cMaterial=CSteelMatLibrary::Q460BriefMark();
				else
					(*ppTtaElem)->cMaterial=CSteelMatLibrary::Q235BriefMark();
				(*ppTtaElem)->boltd=boltD;
				(*ppTtaElem)->boltN=boltN;
				(*ppTtaElem)->boltgrade=ibolt.GradeOfMd(boltD);
				//
				for(int i=1;i<4;i++)
				{
					TTA_NODE* pMirNodeS=AddOrUpdateMirTtaNode((*ppTtaElem)->pStart,i);
					TTA_NODE* pMirNodeE=AddOrUpdateMirTtaNode((*ppTtaElem)->pEnd,i);
					if(pMirNodeS==NULL||pMirNodeE==NULL)
						continue;
					TTA_ELEM** ppMirTtaElem=pMapElems->GetValue(pMirNodeS->pointI,pMirNodeE->pointI);
					if(ppMirTtaElem==NULL)
						ppMirTtaElem=pMapElems->GetValue(pMirNodeE->pointI,pMirNodeS->pointI);
					if(ppMirTtaElem)
					{
						(*ppMirTtaElem)->size.wide=(*ppTtaElem)->size.wide;
						(*ppMirTtaElem)->size.thick=(*ppTtaElem)->size.thick;
						(*ppMirTtaElem)->size.idClassType=(*ppTtaElem)->size.idClassType;
						(*ppMirTtaElem)->cMaterial=(*ppTtaElem)->cMaterial;
					}
				}
			}
		}
		if(pMapBraceElems)
		{
			TTA_BRACEELEM** ppBraceElem=pMapBraceElems->GetValue(indexS,indexE);
			if(ppBraceElem==NULL)
				ppBraceElem=pMapBraceElems->GetValue(indexE,indexS);
			if(ppBraceElem)
			{
				(*ppBraceElem)->bReadSizeSpec=true;
				(*ppBraceElem)->size_wide=size_wide;
				(*ppBraceElem)->size_thick=size_thick;
				if(iType==0)
					(*ppBraceElem)->cSectType='L';
				else
					(*ppBraceElem)->cSectType='R';
				if(iMat==2)
					(*ppBraceElem)->cBriefMaterial=CSteelMatLibrary::Q345BriefMark();
				else if(iMat==3)
					(*ppBraceElem)->cBriefMaterial=CSteelMatLibrary::Q420BriefMark();
				else if(iMat==4)
					(*ppBraceElem)->cBriefMaterial=CSteelMatLibrary::Q460BriefMark();
				else
					(*ppBraceElem)->cBriefMaterial=CSteelMatLibrary::Q235BriefMark();
				(*ppBraceElem)->boltd=boltD;
				(*ppBraceElem)->boltN=boltN;
				(*ppBraceElem)->boltgrade=ibolt.GradeOfMd(boltD);
			}
			else
			{
				logerr.Log("��ǰ�������ļ���%d-%d ��Ԫδ�ڸ���ģ�����ҵ�!",indexS,indexE);
				continue;
			}
			//
			for(int i=1;i<4;i++)
			{
				TTA_NODE* pMirNodeS=AddOrUpdateMirTtaNode((*ppBraceElem)->pStart,i);
				TTA_NODE* pMirNodeE=AddOrUpdateMirTtaNode((*ppBraceElem)->pEnd,i);
				if(pMirNodeS==NULL||pMirNodeE==NULL)
					continue;
				TTA_BRACEELEM** ppMirBraceElem=pMapBraceElems->GetValue(pMirNodeS->pointI,pMirNodeE->pointI);
				if(ppMirBraceElem==NULL)
					ppMirBraceElem=pMapBraceElems->GetValue(pMirNodeE->pointI,pMirNodeS->pointI);
				if(ppMirBraceElem)
				{
					(*ppMirBraceElem)->size_wide=(*ppBraceElem)->size_wide;
					(*ppMirBraceElem)->size_thick=(*ppBraceElem)->size_thick;
					(*ppMirBraceElem)->cSectType=(*ppBraceElem)->cSectType;
					(*ppMirBraceElem)->cBriefMaterial=(*ppBraceElem)->cBriefMaterial;
				}
			}
		}
	}
	return true;
}
bool CTtaDataFile::ReadTANSBraceElemsDataFile(const char* szBraceDataFile,CLDSNode* pDependLdsNodes/*=NULL*/)
{
	GTM_KEYNODE_MAP mapnode;
	mapnode.pMainModelNode=pDependLdsNodes;
	return ReadSubBraceElemsDataFile(szBraceDataFile,&mapnode,1,FILETYPE_TANS);
}
bool CTtaDataFile::ReadSubBraceElemsDataFile(const char* szBraceDataFile,
					GTM_KEYNODE_MAP* pxarrMapNodes/*=NULL*/,int count/*=1*/,char ciFileType/*=3*/)
{
	//char ciFileType=3;
	CLDSNode ldsnode, *pDependLdsNode=NULL;
	if(count<=1)
		pDependLdsNode=pxarrMapNodes!=NULL?pxarrMapNodes[0].pMainModelNode:NULL;
	else
	{ 
		ldsnode.layer(0)='S';
		ldsnode.cfgword.Clear();
		pDependLdsNode=&ldsnode;
		for(int ki=0;ki<4;ki++)
		{
			if(ldsnode.cfgword.IsNull())
			{
				ldsnode.layer(0)=pxarrMapNodes[ki].pMainModelNode->layer(0);
				ldsnode.cfgword=pxarrMapNodes[ki].pMainModelNode->UnifiedCfgword();
			}
			if(!pxarrMapNodes[ki].pMainModelNode->IsLegObj())
				ldsnode.cfgword&=pxarrMapNodes[ki].pMainModelNode->UnifiedCfgword();
			else if(pxarrMapNodes[ki].pMainModelNode->IsLegObj())
			{
				ldsnode.layer(0)=pxarrMapNodes[ki].pMainModelNode->layer(0);
				ldsnode.cfgword=pxarrMapNodes[ki].pMainModelNode->cfgword;
				break;
			}
		}
	}
	FILE* fp=fopen(szBraceDataFile,"rt");
	if(fp==NULL)
		return false;
	//������˨���˼������Ϣ
	CXhChar200 outfile=szBraceDataFile;
	char* szExt=SearchChar(outfile,".",true);
	if(szExt)
		*szExt=0;
	outfile.Append(".out");
	int i,j;
	//һ��������Ϣ����
	char sText[201]="";

	ReadLineText(sText,100,fp);
	Ta.m_sPrjCode=sText;
	ReadLineText(sText,50,fp);
	strcpy(Ta.m_sPrjName,sText);
	ReadLineText(sText,50,fp);
	strcpy(Ta.m_sOperator,sText);

	CXhChar200 lineText;
	//����������Ϣ����
	//KIND  N10	N20	M10	M20	IE	NL	NC	NW	NSEC
	//��K1  K2	K3	K4	K5	K6	NA	TSM	REN	��NGP
	fgets(sText,101,fp);
	sscanf(sText,"%d %d %d %d %d %d %d %d %d %d",&KIND,&N10,&N20,&M10,&M20,&IE,&NL, &NC, &NW,&NSEC);
	fgets(sText,101,fp);
	sscanf(sText,"%d %d %d %d %d %d %d %d %d %d",&K1  ,&K2 ,&K3 ,&K4 ,&K5 ,&K6,&NA,&TSM,&REN,&NGP);
	//ClassifyRodSizeSpecs(KIND%10);
	//�����ڵ�����
	NBY=IE/100;
	IE%=100;
	if(NBY==0&&IE==0)
		NBY=1;	//������һ����¼
	int iLegNo=NBY+1;
	//m_arrBodyLeg.SetSize(0,NBY+IE+2);
	for(i=0;i<NBY+IE;i++)	//������Ӧ�ı�����ȶ�����
		ReadLineText(sText,100,fp);
	//�ġ����ط�������
		//�ݲ�֧�ֺ��ط���,����
	int M=abs(NGP)%10;
	if(M>1)
	{
		for(i=0;i<M;i++)
			ReadLineText(sText,100,fp);
	}
	ARRAY_LIST<TTA_BRACEELEM*> braceElems(0,128);
	CMapList<TTA_BRACEELEM*> mapOnlyBraceElems;
	//�塢��һ�ֽڵ����뷽ʽ����
	TANS_LINE* pLine;
	TANS_PLANE* pPlane;
	TTA_NODE* pTtaNode,*pMirTtaNode;
	m_arrTtaNode.Empty();
	DYN_ARRAY<TTA_NODE*>primaryNodes(N10);
	for(i=0;i<N10;i++)
	{
		TTA_NODE xTempObj;
		ReadLineText(sText,100,fp);
		sscanf(sText,"%d%d%lf%lf%lf",&xTempObj.pointI,&xTempObj.IS,
			&xTempObj.pos.x,&xTempObj.pos.y,&xTempObj.pos.z);
		primaryNodes[i]=NULL;
		if(xTempObj.pointI/10000==7)
		{	//TANS����Ļ�׼��
			pLine=hashLines.Add(xTempObj.pointI);
			if(xTempObj.pos.x>10000)
				pLine->iStartPointI=ftoi(xTempObj.pos.x)%10000;
			if(xTempObj.pos.y>10000&&pLine->iStartPointI==0)
				pLine->iStartPointI=ftoi(xTempObj.pos.y)%10000;
			else if(xTempObj.pos.y>10000)
				pLine->iEndPointI=ftoi(xTempObj.pos.y)%10000;
			if(xTempObj.pos.z>10000&&pLine->iEndPointI==0)
				pLine->iEndPointI=ftoi(xTempObj.pos.z)%10000;
			continue;
		}
		else if(xTempObj.pointI/10000==9)
		{	//TANS����Ļ�׼��
			pPlane=hashPlanes.Add(xTempObj.pointI);
			pPlane->iPoint1stI=ftoi(xTempObj.pos.x)%10000;
			pPlane->iPoint2ndI=ftoi(xTempObj.pos.y)%10000;
			pPlane->iPoint3rdI=ftoi(xTempObj.pos.z)%10000;
			continue;
		}
		primaryNodes[i]=pTtaNode=listTtaNodes.append();
		*primaryNodes[i]=xTempObj;
		hashParentTtaNodeI.SetValue(pTtaNode->pointI,true);
		hashTtaNodeI.SetValue(pTtaNode->pointI,pTtaNode);
		pTtaNode->bBraceNode=true;	//Ĭ��Ϊ�����Ķ˽ڵ�
		if(pTtaNode->IS==1)	//Y��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(2));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,2);	//��ֵIS�Ա���������ԶԳƽڵ㻹�ǶԳƽڵ�
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
		else if(pTtaNode->IS==2)	//X��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(1));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,1);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
		else if(pTtaNode->IS==3)	//Z��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(3));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,3);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
		else if(pTtaNode->IS==4)	//XYZ��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(1));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,1);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(2));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,2);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(3));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,3);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
	}
	//�����ڶ��ֽڵ����뷽ʽ����(�����ڵ�һ����������ǰ���룬��Ϊ����ԺTANS�����е�һ����п������õڶ���㣩 wjh-2018.6.2
	DYN_ARRAY<TTA_NODE*>secondaryNodes(N20);
	for(i=0;i<N20;i++)
	{
		secondaryNodes[i]=pTtaNode=listTtaNodes.append();
		hashParentTtaNodeI.SetValue(pTtaNode->pointI,true);
		//if(m_arrTtaNodePtr2[i]->posType<4)
		//	continue;	//��һ��ڵ�
		pTtaNode->posType=4;	//������
		pTtaNode->bBraceNode=true;	//Ĭ��Ϊ�����Ķ˽ڵ�
		double R;
		ReadLineText(sText,100,fp);
		sscanf(sText,"%d%d%d%lf%d",&pTtaNode->pointI,&pTtaNode->J1,&pTtaNode->J2,&R,&pTtaNode->IS);
		hashTtaNodeI.SetValue(pTtaNode->pointI,pTtaNode);
		if(R<=1.0)	//С��
			pTtaNode->coefScale=R;
		else
			pTtaNode->coefScale=1.0/R;
		if(pTtaNode->IS==1)	//Y��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(2));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,2);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
		else if(pTtaNode->IS==2)	//Y��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(1));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,1);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
		else if(pTtaNode->IS==3)	//Z��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(3));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,3);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
		else if(pTtaNode->IS==4)	//XYZ��Գ�
		{
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(1));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,1);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(2));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,2);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
			pMirTtaNode=listTtaNodes.append();
			pMirTtaNode->pointI=MirPointI(pTtaNode->pointI,MIRMSG(3));
			pMirTtaNode->MirBasicPropFrom(pTtaNode,3);
			hashTtaNodeI.SetValue(pMirTtaNode->pointI,pMirTtaNode);
		}
	}
	//ƥ��ڵ������������Ͳ���ʼ���ڵ�������ڵ�
	for(i=0;i<N10;i++)
	{
		pTtaNode=primaryNodes[i];
		if(pTtaNode==NULL||pTtaNode->pointI>=10000)
			continue;	//����ԺTANS�ǽڵ���չ����(��׼�߻��׼�棩
		int relaPointIArr[2]={0,0};
		int idDatumLine=0,idDatumPlane=0,idOrthoPlane=0;
		int numbers[3];
		numbers[0]=(int)pTtaNode->pos.x;
		numbers[1]=(int)pTtaNode->pos.y;
		numbers[2]=(int)pTtaNode->pos.z;
		for(int j=0;j<3;j++)
		{
			if(numbers[j]/10000==7)
				idDatumLine=numbers[j];
			else if(numbers[j]/10000==8)
				idOrthoPlane=numbers[j];
			else if(numbers[j]/10000==9)
				idDatumPlane=numbers[j];
			else if(numbers[j]/10000==1)
			{
				if(relaPointIArr[0]==0)
					relaPointIArr[0]=numbers[j]-10000;
				else
					relaPointIArr[1]=numbers[j]-10000;
			}
		}
		if(idDatumLine>0)
		{
			TANS_LINE* pLine=hashLines.GetValue(idDatumLine);
			if(pLine==NULL)
			{
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"��ʧ��׼��%d",idDatumLine);
				continue;
			}
			pTtaNode->posType=5;	//�����
			pTtaNode->J3=pLine->iStartPointI;
			pTtaNode->J4=pLine->iEndPointI;
		}
		else if(idOrthoPlane>0)
		{	//����սڵ����������Ľڵ�
			pTtaNode->posType=6;
			if(numbers[0]==idDatumPlane)
				pTtaNode->ciPosViceType='X';	//��׼����ָ��YZ����
			else if(numbers[1]==idDatumPlane)
				pTtaNode->ciPosViceType='Y';	//��׼����ָ��XZ����
			else if(numbers[2]==idDatumPlane)
				pTtaNode->ciPosViceType='Z';	//��׼����ָ��XY����
			pTtaNode->J3=idOrthoPlane%10000;	//���յ�
		}
		else if(idDatumPlane>0)
		{
			pTtaNode->posType=8;		//�˴������ܸ���idDatumPlane��λ���жϻ�׼����ָ������������(XY/YZ/XZ),��Ĭ��ΪXY���ͣ������ж� wjh-2018.6.2
			pTtaNode->J3=idDatumPlane;	//������׼��J3=90000+��ʾ��׼��ţ�10000+��ʾ�����ڵ��
		}
		TTA_NODE* pTtaNode0=hashTtaNodeI.GetValue(relaPointIArr[0]);
		TTA_NODE* pTtaNode1=hashTtaNodeI.GetValue(relaPointIArr[1]);
		if(pTtaNode0&&pTtaNode1)
		{
			pTtaNode->pAttachStart=pTtaNode0;
			pTtaNode->pAttachEnd=pTtaNode1;
			pTtaNode->J1=relaPointIArr[0];
			pTtaNode->J2=relaPointIArr[1];
			if(pTtaNode->posType==0)//relaPointIArr[0]>0&&relaPointIArr[1]>0)
			{
				if(pTtaNode->pos.x<10000)
					pTtaNode->posType=1;
				else if(pTtaNode->pos.y<10000)
					pTtaNode->posType=2;
				else if(pTtaNode->pos.z<10000)
					pTtaNode->posType=3;
			}
			if(pTtaNode->posType>=8&&pTtaNode->posType<=10)
			{
				double DIFF_XY=fabs(pTtaNode0->pos.x-pTtaNode1->pos.x)+fabs(pTtaNode0->pos.y-pTtaNode1->pos.y);
				double DIFF_YZ=fabs(pTtaNode0->pos.y-pTtaNode1->pos.y)+fabs(pTtaNode0->pos.z-pTtaNode1->pos.z);
				double DIFF_XZ=fabs(pTtaNode0->pos.x-pTtaNode1->pos.x)+fabs(pTtaNode0->pos.z-pTtaNode1->pos.z);
				//LDS���������󽻵Ļ�׼�߷�����ʱ����ݲ��ܴ���ʵ�ʹ���Ҳ�������漰����������ʽ����õ�ʱ�ٴ��� wjh-2018.6.2
				if(DIFF_XY<DIFF_YZ&&DIFF_XY<DIFF_XZ)
				{
					pTtaNode->ciPosViceType='Z';
					pTtaNode->posType=8;	//��׼����ָ��XY����
					if(DIFF_XY>0.001)
						logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON,"LDS�ݲ��ܴ��������󽻵Ļ�׼�߷��������,���˹��޸�ģ�Ͳ���ʱ���ܴ�������");
				}
				else if(DIFF_YZ<DIFF_XY&&DIFF_YZ<DIFF_XZ)
				{
					pTtaNode->ciPosViceType='X';
					pTtaNode->posType=9;	//��׼����ָ��YZ����
					if(DIFF_YZ>0.001)
						logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON,"LDS�ݲ��ܴ��������󽻵Ļ�׼�߷��������,���˹��޸�ģ�Ͳ���ʱ���ܴ�������");
				}
				else if(DIFF_XZ<DIFF_XY&&DIFF_XZ<DIFF_YZ)
				{
					pTtaNode->ciPosViceType='Y';
					pTtaNode->posType=10;	//��׼����ָ��XZ����
					if(DIFF_XZ>0.001)
						logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON,"LDS�ݲ��ܴ��������󽻵Ļ�׼�߷��������,���˹��޸�ģ�Ͳ���ʱ���ܴ�������");
				}
			}
		}
		if(pTtaNode->pos.x>10000)
			pTtaNode->pos.x=numbers[0]%10000;
		if(pTtaNode->pos.y>10000)
			pTtaNode->pos.y=numbers[1]%10000;
		if(pTtaNode->pos.z>10000)
			pTtaNode->pos.z=numbers[2]%10000;
	}
	for(i=0;i<N20;i++)
	{	//��ʼ��ԭʼ(1����)��������ڵ�������ڵ�
		pTtaNode=secondaryNodes[i];
		if(hashTtaNodeI.GetValue(pTtaNode->J1)!=NULL)
			pTtaNode->pAttachStart=hashTtaNodeI[pTtaNode->J1];
		else
		{
			AfxMessageBox(CXhChar50("��%d�Žڵ�֮ǰδ�����������ڵ�%d",pTtaNode->pointI,pTtaNode->J1));
			return false;
		}
		if(hashTtaNodeI.GetValue(pTtaNode->J2)!=NULL)
			pTtaNode->pAttachEnd=hashTtaNodeI[pTtaNode->J2];
		else
		{
			AfxMessageBox(CXhChar50("��%d�Žڵ�֮ǰδ�����������ڵ�%d",pTtaNode->pointI,pTtaNode->J2));
			return false;
		}
	}
	for(pLine=hashLines.GetFirst();pLine;pLine=hashLines.GetNext())
		UpdateDatumLine(pLine);
	for(pPlane=hashPlanes.GetFirst();pPlane;pPlane=hashPlanes.GetNext())
		UpdateDatumPlane(pPlane);
	//���¼���ڵ�����
	for(i=0;i<N10;i++)
	{
		pTtaNode=primaryNodes[i];
		if(pTtaNode==NULL||pTtaNode->pointI>=10000)
			continue;	//����ԺTANS�ǽڵ���չ����(��׼�߻��׼�棩
		pTtaNode->UpdatePosition(this);
		if(pTtaNode->IS>0&&pTtaNode->IS<4)	//Y��Գ�
			AddOrUpdateMirTtaNode(pTtaNode,pTtaNode->IS);
		else if(pTtaNode->IS==4)
		{
			AddOrUpdateMirTtaNode(pTtaNode,1);
			AddOrUpdateMirTtaNode(pTtaNode,2);
			AddOrUpdateMirTtaNode(pTtaNode,3);
		}
	}
	
	for(i=0;i<N20;i++)
	{
		pTtaNode=secondaryNodes[i];
		pTtaNode->UpdatePosition();
		//pTtaNode->pos=pTtaNode->pAttachStart->pos+pTtaNode->coefScale*(pTtaNode->pAttachEnd->pos-pTtaNode->pAttachStart->pos);
		if(pTtaNode->IS>0&&pTtaNode->IS<4)	//Y��Գ�
			AddOrUpdateMirTtaNode(pTtaNode,pTtaNode->IS);
		else if(pTtaNode->IS==4)
		{
			AddOrUpdateMirTtaNode(pTtaNode,1);
			AddOrUpdateMirTtaNode(pTtaNode,2);
			AddOrUpdateMirTtaNode(pTtaNode,3);
		}
	}
	//�ߡ�����������
	//��ȡdata.ini�еĹ����������ʱ�
	K1=abs(K1);
	for(i=0;i<M10;i++)
	{
		ReadLineText(sText,200,fp);
		TTA_ELEM *pTtaElem,*pMirTtaElem;
		pTtaElem=listTtaElems.append();
		int start_i,end_i;
		sscanf(sText,"%d%d%d%d%d%d%d%d%d",&start_i,&end_i,
			&pTtaElem->N,&pTtaElem->IS,&pTtaElem->TYPE,&pTtaElem->M,
			&pTtaElem->L1,&pTtaElem->L2,&pTtaElem->SMC);
		//1.JB��JE������
		if(hashTtaNodeI.GetValue(start_i)!=NULL)
			pTtaElem->pStart=hashTtaNodeI[start_i];
		if(hashTtaNodeI.GetValue(end_i)!=NULL)
			pTtaElem->pEnd=hashTtaNodeI[end_i];
		if(pTtaElem->TYPE!=5)
		{
			if(pTtaElem->pStart)	//ʼ�˽ڵ��趨Ϊ�����Ķ˽ڵ�
				pTtaElem->pStart->bBraceNode=false;
			if(pTtaElem->pEnd)		//�ն˽ڵ��趨Ϊ�����Ķ˽ڵ�
				pTtaElem->pEnd->bBraceNode=false;
			continue;
		}
		//2.N������
		if(K1<200)
		{	//��ʹ��Q235��Q345���ֲ���
			if(pTtaElem->N>=0)
				pTtaElem->cMaterial=CSteelMatLibrary::Q235BriefMark();//'S';
			else
				pTtaElem->cMaterial=CSteelMatLibrary::Q345BriefMark();//'H';
			if(pTtaElem->N>=0)
				pTtaElem->iSizeOrder=pTtaElem->N;
			else if(pTtaElem->N<=-1&&pTtaElem->N>=-490)
				pTtaElem->iSizeOrder=-pTtaElem->N;
			else if(pTtaElem->N<=-500&&pTtaElem->N>=-990)
				pTtaElem->iSizeOrder=(-pTtaElem->N)%500+1;
			else if(pTtaElem->N<=-1001&&pTtaElem->N>=-1490)
			{
				pTtaElem->iSizeOrder=(-pTtaElem->N)%1000;
				pTtaElem->cMaterial=CSteelMatLibrary::Q390BriefMark();//'G'
			}
			else if(pTtaElem->N<=-2001&&pTtaElem->N>=-2490)
			{
				pTtaElem->iSizeOrder=(-pTtaElem->N)%2000;
				pTtaElem->cMaterial=CSteelMatLibrary::Q420BriefMark();//'P'
			}
			else if(pTtaElem->N<=-3001&&pTtaElem->N>=-3490)
			{
				pTtaElem->iSizeOrder=(-pTtaElem->N)%3000;
				pTtaElem->cMaterial=CSteelMatLibrary::Q460BriefMark();//'T'
			}
		}
		else
		{	//ʹ�ö��ֲ��ʵĸֲ�
			if(TSM>0)
			{	//��Ӧ������Ż�ѡ��
				if(pTtaElem->N<=3000)
					pTtaElem->cMaterial=QuerySteelBriefMatMark("Q235");//'S';
				else if(pTtaElem->N<=6000)
					pTtaElem->cMaterial=QuerySteelBriefMatMark("Q345");//'H';
				else if(pTtaElem->N<=7000)
					pTtaElem->cMaterial=QuerySteelBriefMatMark("Q390");//'G';
				else if(pTtaElem->N<=8000)
					pTtaElem->cMaterial=QuerySteelBriefMatMark("Q420");//'P';
				else if(pTtaElem->N<=9000)
					pTtaElem->cMaterial=QuerySteelBriefMatMark("Q460");//'T';
			}
			else
			{	//����
				if(pTtaElem->N<=9100)
					pTtaElem->cMaterial=QuerySteelBriefMatMark("Q235");//'S';
				else if(pTtaElem->N<=9200)
					pTtaElem->cMaterial=QuerySteelBriefMatMark("Q345");//'H';
				else if(pTtaElem->N<=9300)
					pTtaElem->cMaterial=QuerySteelBriefMatMark("Q390");//'G';
				else if(pTtaElem->N<=9400)
					pTtaElem->cMaterial=QuerySteelBriefMatMark("Q420");//'P';
				else if(pTtaElem->N<=9500)
					pTtaElem->cMaterial=QuerySteelBriefMatMark("Q460");//'T';
			}
			//KIND>0&&TSM>0&&
			if(pTtaElem->N<=9000)	//��Ӧ������Ż�ѡ��
				pTtaElem->iStatMatNo=pTtaElem->N;
			else	//pTtaElem->N������ϱ��е�ĳһ������
				pTtaElem->iSizeOrder=pTtaElem->N%100;	//������
		}
		//TYPE������
		TTA_BRACEELEM braceElem;
		braceElem.pStart=pTtaElem->pStart;
		braceElem.pEnd=pTtaElem->pEnd;
		braceElem.cBriefMaterial=pTtaElem->cMaterial;
		braceElem.size_wide=pTtaElem->size.wide;
		braceElem.size_thick=pTtaElem->size.thick;
		braceElem.cSectType=pTtaElem->size.cSubClassType;
		TTA_BRACEELEM *pMirTtaBraceElem,*pTtaBraceElem=listTtaBraceElems.append(braceElem);
		mapOnlyBraceElems.SetValue(start_i,end_i,pTtaBraceElem);
		pTtaBraceElem->IS=pTtaElem->IS;
		if(pTtaBraceElem->IS%10>=4)	//ʶ��XYZ��Գ�
		{
			for(j=1;j<4;j++)
			{
				pMirTtaBraceElem=listTtaBraceElems.append(*pTtaBraceElem);
				pMirTtaBraceElem->pStart=AddOrUpdateMirTtaNode(pTtaBraceElem->pStart,j);
				pMirTtaBraceElem->pEnd=AddOrUpdateMirTtaNode(pTtaBraceElem->pEnd,j);
				mapOnlyBraceElems.SetValue(pMirTtaBraceElem->pStart->pointI,pMirTtaBraceElem->pEnd->pointI,pMirTtaBraceElem);
			}
		}
		else if(pTtaBraceElem->IS%10>0)//if(pTtaBraceElem->IS%10==1)	//ʶ��Y��(����)�Գ�
		{
			int mir_i=pTtaBraceElem->IS%10;
			pMirTtaBraceElem=listTtaBraceElems.append(*pTtaBraceElem);
			pMirTtaBraceElem->pStart=AddOrUpdateMirTtaNode(pTtaBraceElem->pStart,mir_i);
			pMirTtaBraceElem->pEnd=AddOrUpdateMirTtaNode(pTtaBraceElem->pEnd,mir_i);
			if(mir_i==1&&pMirTtaBraceElem->pStart->pos.x==0&&pMirTtaBraceElem->pEnd->pos.x==0)
				continue;	//�޷���������(Y)�Գ�ʱ
			else if(mir_i==2&&pTtaBraceElem->pStart->pos.y==0&&pTtaBraceElem->pEnd->pos.y==0)
				continue;	//�޷�����ǰ��(X)�Գ�ʱ
			mapOnlyBraceElems.SetValue(pMirTtaBraceElem->pStart->pointI,pMirTtaBraceElem->pEnd->pointI,pMirTtaBraceElem);
		}
		//3.IS������
		if(pTtaElem->IS>0&&pTtaElem->IS<3)	//Y��Գ�
		{
			pMirTtaElem=listTtaElems.append(*pTtaElem);
			pMirTtaElem->pStart=AddOrUpdateMirTtaNode(pTtaElem->pStart,pTtaElem->IS);//hashTtaNodeI[start.pointI];
			pMirTtaElem->pEnd=AddOrUpdateMirTtaNode(pTtaElem->pEnd,pTtaElem->IS);//hashTtaNodeI[end.pointI];
		}
		else if(pTtaElem->IS==4)	//XYZ��Գ�
		{
			for(j=1;j<4;j++)
			{
				pMirTtaElem=listTtaElems.append(*pTtaElem);
				pMirTtaElem->pStart=AddOrUpdateMirTtaNode(pTtaElem->pStart,j);//hashTtaNodeI[start.pointI];
				pMirTtaElem->pEnd=AddOrUpdateMirTtaNode(pTtaElem->pEnd,j);//hashTtaNodeI[end.pointI];
			}
		}
		//4.TYPE������
	}
	//�ˡ�����������
	for(i=0;i<M20;i++)
	{
		fgets(sText,101,fp);
		TTA_BRACEELEM *pMirTtaBraceElem;
		TTA_BRACEELEM *pTtaBraceElem=listTtaBraceElems.append();
		long start_i,end_i;
		sscanf(sText,"%d%d%d%d",&start_i,&end_i,&pTtaBraceElem->IS,&pTtaBraceElem->N);
		pTtaBraceElem->pStart=hashTtaNodeI[start_i];
		pTtaBraceElem->pEnd=hashTtaNodeI[end_i];
		if(pTtaBraceElem->IS%10>=4)	//ʶ��XYZ��Գ�
		{
			for(int i=1;i<4;i++)
			{
				pMirTtaBraceElem=listTtaBraceElems.append(*pTtaBraceElem);
				pMirTtaBraceElem->pStart=AddOrUpdateMirTtaNode(pTtaBraceElem->pStart,i);//hashTtaNodeI[start.pointI];
				pMirTtaBraceElem->pEnd=AddOrUpdateMirTtaNode(pTtaBraceElem->pEnd,i);//hashTtaNodeI[end.pointI];
			}
		}
		else if(pTtaBraceElem->IS%10==1)	//ʶ��X��Y��Գ�,����Y��Գ�
		{
			pMirTtaBraceElem=listTtaBraceElems.append(*pTtaBraceElem);
			int mir_i=1;
			if(pTtaBraceElem->pStart->pos.x==0||pTtaBraceElem->pEnd->pos.x==0)
				mir_i=2;	//�޷���������(Y)�Գ�ʱ������ǰ��(X)�Գ�
			pMirTtaBraceElem->pStart=AddOrUpdateMirTtaNode(pTtaBraceElem->pStart,mir_i);//hashTtaNodeI[start.pointI];
			pMirTtaBraceElem->pEnd=AddOrUpdateMirTtaNode(pTtaBraceElem->pEnd,mir_i);//hashTtaNodeI[end.pointI];
		}
	}
	fclose(fp);
#ifdef __TIMER_COUNT_
	timer.Relay(1);
#endif
	//�ؽ����ļ��˽ڵ�
	CLDSNode* pNode;
	for(pNode=Ta.EnumNodeFirst();pNode;pNode=Ta.EnumNodeNext())
	{
		if(pDependLdsNode&&!pNode->ModuleTogetherWith(pDependLdsNode))
			continue;	//����ͬһ����������
		for(pTtaNode=hashTtaNodeI.GetFirst();pTtaNode;pTtaNode=hashTtaNodeI.GetNext())
		{
			if(pTtaNode->pNode!=NULL)
				continue;
			GEPOINT pos=pTtaNode->pos*1000;
			GEPOINT dv(pos.x-pNode->xOriginalPos.x,pos.y-pNode->xOriginalPos.y,pos.z-pNode->xOriginalPos.z);
			if(fabs(dv.x)<1&&fabs(dv.y)<1&&fabs(dv.z)<1)
			{
				pTtaNode->pNode=pNode;
				pTtaNode->cfgword=pNode->cfgword;
				pTtaNode->cPosLayer=pNode->layer(0);
				break;
			}
		}
	}
	for(pTtaNode=hashTtaNodeI.GetFirst();pTtaNode;pTtaNode=hashTtaNodeI.GetNext())
	{
		if(pTtaNode->pNode==NULL)
		{
			if(pDependLdsNode)
			{
				pTtaNode->cfgword=pDependLdsNode->cfgword;
				pTtaNode->cPosLayer=pDependLdsNode->layer(0);
			}
			else
				ValidateBraceNodeCfgWord(pTtaNode,true);
		}
	}
	m_nOverlapBraceNodes=0;
	BuildLdsGeoModel(&hashTtaNodeI,&mapOnlyBraceElems,false);
	//��ȡ�������ļ�
	FILE* braceout_fp=fopen(outfile,"rt");
	if(braceout_fp)
		ReadDaoHengAuxElemOutFile(braceout_fp,NULL,ciFileType);
	fclose(braceout_fp);
	return true;
}
bool TTAELEM_DESIGN_RSLT::InitFromElemRsltStr(char* rsltstr,char ciFileType/*=0*/)
{
	CXhChar200 szLineText=rsltstr;	//�������ı����Է�������strtok�޸�
	char *spliter,*iter;
	CXhChar16 itemtext;
	char ciSectType=0;	//�Ǹ�
	if(ciFileType<=2)
	{
		if(strchr(rsltstr,'R')!=NULL)
			size.cSubClassType=ciSectType='R';
		if(ciFileType==2&&strchr(rsltstr,'L')!=NULL)
			size.idClassType=CLS_LINEANGLE;
		while(SafeIsDigit(*rsltstr)==0&&*rsltstr!=0)
			rsltstr++;	//��λ��֫����ʼ�ַ�
	}
	else if(ciFileType==3)
	{
		while((SafeIsDigit(*rsltstr)==0||SafeIsDigit(*(rsltstr+1))==0)&&*rsltstr!=0)
		{
			if(SafeIsDigit(*rsltstr)!=0)
				ciSectType=*rsltstr-'0';
			rsltstr++;	//��λ��֫����ʼ�ַ�
		}
	}
	spliter=SearchChar(rsltstr,"Xx");
	if(spliter!=NULL)
	//while(toupper(*rsltstr)!='X'&&*rsltstr!=0)
	{
		itemtext.NCopy(rsltstr,spliter-rsltstr);
		rsltstr=spliter+1;
		//itemtext.Append(*rsltstr);
		//rsltstr++;
	}
	size.wide=atoi(itemtext);	//��ȡ֫��
	if(ciSectType!='R'&&ciSectType>0)
		size.wide+=1000*ciSectType;
	//rsltstr++;//���'X'�ַ�
	for(iter=rsltstr;*iter!=0;iter++)
		continue;
	if(iter-rsltstr<124)
		return false;
	spliter=SearchChar(rsltstr,'.');
	if(spliter&&spliter-rsltstr<=3)
	{	//����MYL�����������֫��֧��С����
		for(iter=spliter+1;*iter!=0&&SafeIsDigit(*iter);iter++)
			continue;
		itemtext.NCopy(rsltstr,iter-rsltstr);
	}
	else	//��TTAģʽ֫���֧������
		itemtext.NCopy(rsltstr,2);
	size.thick=atoi(itemtext);
	rsltstr+=itemtext.Length;	//���֫������
	cMaterial=*rsltstr;
	if(cMaterial=='_'&&*(rsltstr+1)=='Q')
	{	//����Ժ�������ϰ����'_'+"Q345"��ʽ
		if(*(rsltstr+2)=='3')
			cMaterial=CSteelMatLibrary::Q345BriefMark();
		else if(*(rsltstr+2)=='4')
			cMaterial=CSteelMatLibrary::Q420BriefMark();
		else
			cMaterial='S';
		rsltstr+=4;
	}
	else if(ciFileType==2&&cMaterial=='D')
		cMaterial=CSteelMatLibrary::Q420BriefMark();
	else if(ciFileType==2&&cMaterial=='E')
		cMaterial=CSteelMatLibrary::Q460BriefMark();
	rsltstr++;	//��������ַ�����
	while(*rsltstr==' '||*rsltstr=='\t')
		rsltstr++;	//�������ո��
	if((spliter=SearchChar(rsltstr,' '))!=NULL)
		itemtext.NCopy(rsltstr,spliter-rsltstr);
	length=atof(itemtext)*1000;
	rsltstr+=itemtext.Length;	//�����Ԫ��������
	itemtext.NCopy(rsltstr,7);
	ulength=atof(itemtext)*10;
	rsltstr+=7;	//�����Ԫ���㳤������
	itemtext.NCopy(rsltstr,8);
	gyration=atof(itemtext)*10;	//cm->mm
	rsltstr+=8;	//�����ת�뾶����
	itemtext.NCopy(rsltstr,7);
	lamda=atof(itemtext);
	rsltstr+=7;	//�����ϸ������
	itemtext.NCopy(rsltstr,7);
	allowLamda=atof(itemtext);
	rsltstr+=7;	//�������ϸ������
	itemtext.NCopy(rsltstr,7);
	stableFactor=atof(itemtext);
	rsltstr+=7;	//���ѹ��ϵ������
	itemtext.NCopy(rsltstr,9);
	maxTension=atof(itemtext)*1000;	//kN->N
	rsltstr+=9;	//��������������
	itemtext.NCopy(rsltstr,6);
	idMaxTensionCase=atoi(itemtext);
	rsltstr+=6;	//��������������-��������
	itemtext.NCopy(rsltstr,9);
	maxCompression=atof(itemtext)*1000;	//kN->N
	rsltstr+=9;	//������ѹ������
	itemtext.NCopy(rsltstr,6);
	idMaxCompressionCase=atoi(itemtext);
	rsltstr+=6;	//������ѹ������-��������
	itemtext.NCopy(rsltstr,8);
	strengthDeductCoef=atof(itemtext);
	rsltstr+=8;	//���ǿ���ۼ�ϵ������
	//����Ӧ�����Ρ�����Ӧ�����Ρ���ԪЧ�ʼ���˨��Ϣ
	//itemtext.NCopy(rsltstr,6);
	char* key=strtok(rsltstr,"MXx ");
	if(key!=NULL)
	{	//����Ӧ������
		actualStress=atof(key)*10;	//MPa
		key=strtok(NULL,"MXx ");
	}
	if(key!=NULL)
	{	//����Ӧ������
		allowStress=atof(key)*10;	//kN/cm^2->MPa
		key=strtok(NULL,"MXx ");
	}
	if(key!=NULL)
	{	//��ԪЧ������
		efficiency=atof(key);
		key=strtok(NULL,"MXx ");
	}
	//��˨��Ϣ
	if((spliter=SearchChar(szLineText,'M'))==NULL)
		return false;
	itemtext.NCopy(spliter-4);
	key=strtok(itemtext,"MXx");
	if(key!=NULL)
	{
		boltgrade=(float)atof(key);
		key=strtok(NULL,"MXx");
	}
	if(key!=NULL)
	{
		boltd=atoi(key);
		key=strtok(NULL,"MXx");
	}
	if(key!=NULL)
		boltN=atoi(key);
	/* �ɴ���һ����ʽ�Է����仯�Ϳ��ܶ�������˨��Ϣ wjh-2018.6.2
	if(key!=NULL)
	{
		boltgrade=(float)atof(key);
		key=strtok(NULL,"MXx ");
	}
	if(key!=NULL)
	{
		boltd=atoi(key);
		key=strtok(NULL,"MXx ");
	}
	if(key!=NULL)
		boltN=atoi(key);*/
	return true;
}
bool TTAELEM_DESIGN_RSLT::InitFromTANSElemRsltStr(char* rsltstr)
{
	CXhChar200 szLineText=rsltstr;	//�������ı����Է�������strtok�޸�
	char *spliter,*iter;
	CXhChar16 itemtext;
	char ciFileType=3;
	char ciSectType=0;	//�Ǹ�

	if (strchr(rsltstr, 'O') != NULL)
		size.cSubClassType = ciSectType = 'R';
	else if(strstr(rsltstr,"2L")!=NULL)
	{
		size.idClassType = CLS_GROUPLINEANGLE;
		size.cSubClassType = 'D';
	}
	else if(strstr(rsltstr, "4L") != NULL)
	{
		size.idClassType = CLS_GROUPLINEANGLE;
		size.cSubClassType = 'X';
	}
	else if (strchr(rsltstr, 'L') != NULL)
		size.idClassType = CLS_LINEANGLE;

	while ((SafeIsDigit(*rsltstr) == 0 || SafeIsDigit(*(rsltstr + 1)) == 0) && *rsltstr != 0)
	{
		if (SafeIsDigit(*rsltstr) != 0)
			ciSectType = *rsltstr - '0';
		rsltstr++;	//��λ��֫����ʼ�ַ�
	}

	spliter=SearchChar(rsltstr,"Xx");
	if(spliter!=NULL)
	//while(toupper(*rsltstr)!='X'&&*rsltstr!=0)
	{
		itemtext.NCopy(rsltstr,spliter-rsltstr);
		rsltstr=spliter+1;
		//itemtext.Append(*rsltstr);
		//rsltstr++;
	}
	size.wide=atoi(itemtext);	//��ȡ֫��
	if(ciSectType!='R'&&ciSectType>0)
		size.wide+=1000*ciSectType;
	//rsltstr++;//���'X'�ַ�
	for(iter=rsltstr;*iter!=0;iter++)
		continue;
	if(iter-rsltstr<124)
		return false;
	spliter=SearchChar(rsltstr,'.');
	if(spliter&&spliter-rsltstr<=3)
	{	//����MYL�����������֫��֧��С����
		for(iter=spliter+1;*iter!=0&&SafeIsDigit(*iter);iter++)
			continue;
		itemtext.NCopy(rsltstr,iter-rsltstr);
	}
	else	//��TTAģʽ֫���֧������
		itemtext.NCopy(rsltstr,2);
	size.thick=atoi(itemtext);
	rsltstr+=itemtext.Length;	//���֫������
	cMaterial=*rsltstr;
	if(cMaterial=='_'&&*(rsltstr+1)=='Q')
	{	//����Ժ�������ϰ����'_'+"Q345"��ʽ
		if(*(rsltstr+2)=='3')
			cMaterial=CSteelMatLibrary::Q345BriefMark();
		else if(*(rsltstr+2)=='4')
			cMaterial=CSteelMatLibrary::Q420BriefMark();
		else
			cMaterial='S';
		rsltstr+=4;
	}

	rsltstr++;	//��������ַ�����
	while(*rsltstr==' '||*rsltstr=='\t')
		rsltstr++;	//�������ո��
	if((spliter=SearchChar(rsltstr,' '))!=NULL)
		itemtext.NCopy(rsltstr,spliter-rsltstr);
	length=atof(itemtext)*1000;
	rsltstr+=itemtext.Length;	//�����Ԫ��������
	itemtext.NCopy(rsltstr,7);
	ulength=atof(itemtext)*10;
	rsltstr+=7;	//�����Ԫ���㳤������
	itemtext.NCopy(rsltstr,8);
	gyration=atof(itemtext)*10;	//cm->mm
	rsltstr+=8;	//�����ת�뾶����
	itemtext.NCopy(rsltstr,7);
	lamda=atof(itemtext);
	rsltstr+=7;	//�����ϸ������
	itemtext.NCopy(rsltstr,7);
	allowLamda=atof(itemtext);
	rsltstr+=7;	//�������ϸ������
	itemtext.NCopy(rsltstr,7);
	stableFactor=atof(itemtext);
	rsltstr+=7;	//���ѹ��ϵ������
	itemtext.NCopy(rsltstr,9);
	maxTension=atof(itemtext)*1000;	//kN->N
	rsltstr+=9;	//��������������
	itemtext.NCopy(rsltstr,6);
	idMaxTensionCase=atoi(itemtext);
	rsltstr+=6;	//��������������-��������
	itemtext.NCopy(rsltstr,9);
	maxCompression=atof(itemtext)*1000;	//kN->N
	rsltstr+=9;	//������ѹ������
	itemtext.NCopy(rsltstr,6);
	idMaxCompressionCase=atoi(itemtext);
	rsltstr+=6;	//������ѹ������-��������
	itemtext.NCopy(rsltstr,8);
	strengthDeductCoef=atof(itemtext);
	rsltstr+=8;	//���ǿ���ۼ�ϵ������
	//����Ӧ�����Ρ�����Ӧ�����Ρ���ԪЧ�ʼ���˨��Ϣ
	//itemtext.NCopy(rsltstr,6);
	char* key=strtok(rsltstr,"MXx ");
	if(key!=NULL)
	{	//����Ӧ������
		actualStress=atof(key)*10;	//MPa
		key=strtok(NULL,"MXx ");
	}
	if(key!=NULL)
	{	//����Ӧ������
		allowStress=atof(key)*10;	//kN/cm^2->MPa
		key=strtok(NULL,"MXx ");
	}
	if(key!=NULL)
	{	//��ԪЧ������
		efficiency=atof(key);
		key=strtok(NULL,"MXx ");
	}
	//��˨��Ϣ
	if((spliter=SearchChar(szLineText,'M'))==NULL)
		return false;
	itemtext.NCopy(spliter-4);
	key=strtok(itemtext,"MXx");
	if(key!=NULL)
	{
		boltgrade=(float)atof(key);
		key=strtok(NULL,"MXx");
	}
	if(key!=NULL)
	{
		boltd=atoi(key);
		key=strtok(NULL,"MXx");
	}
	if(key!=NULL)
		boltN=atoi(key);
	/* �ɴ���һ����ʽ�Է����仯�Ϳ��ܶ�������˨��Ϣ wjh-2018.6.2
	if(key!=NULL)
	{
		boltgrade=(float)atof(key);
		key=strtok(NULL,"MXx ");
	}
	if(key!=NULL)
	{
		boltd=atoi(key);
		key=strtok(NULL,"MXx ");
	}
	if(key!=NULL)
		boltN=atoi(key);*/
	return true;
}
//��ȡ���Զ�������һ��Ŀ�ַ�����ʼλ
char* ReadNextItemStr(char* rsltstr,char* itemtext,int maxItemStrLen=10,int defaultItemStrLen=9)
{
	char* spliter;
	CXhString text(itemtext,maxItemStrLen+1);
	while(*rsltstr==' '||*rsltstr=='\t')
		rsltstr++;	//�������ո��
	if((spliter=SearchChar(rsltstr,' '))!=NULL&&spliter-rsltstr<maxItemStrLen)
		text.NCopy(rsltstr,spliter-rsltstr);
	else
		text.NCopy(rsltstr,defaultItemStrLen);
	return rsltstr+text.Length;
}
bool TTAELEM_DESIGN_RSLT::InitFromDaoHengAuxElemRsltStr(char* rsltstr,char ciFileType/*=0*/)
{
	char *spliter,*iter;
	CXhChar16 itemtext;
	CXhChar200 szOriginalRsltStr=rsltstr;
	while(SafeIsDigit(*rsltstr)==0&&*rsltstr!=0)
		rsltstr++;	//��λ��֫����ʼ�ַ�
	spliter=SearchChar(rsltstr,"Xx");
	if(spliter!=NULL)
		//while(toupper(*rsltstr)!='X'&&*rsltstr!=0)
	{
		itemtext.NCopy(rsltstr,spliter-rsltstr);
		rsltstr=spliter+1;
		//itemtext.Append(*rsltstr);
		//rsltstr++;
	}
	size.wide=atoi(itemtext);	//��ȡ֫��
	//rsltstr++;//���'X'�ַ�
	for(iter=rsltstr;*iter!=0;iter++)
		continue;
	if(iter-rsltstr<128)
		return false;
	spliter=SearchChar(rsltstr,'.');
	if(spliter&&spliter-rsltstr<=3)
	{	//����MYL�����������֫��֧��С����
		for(iter=spliter+1;*iter!=0&&SafeIsDigit(*iter);iter++)
			continue;
		itemtext.NCopy(rsltstr,iter-rsltstr);
	}
	else if(SafeIsDigit(*(rsltstr+1)))	//��TTAģʽ֫���֧������
		itemtext.NCopy(&rsltstr,2,true);
	else
		itemtext.NCopy(&rsltstr,1,true);
	size.thick=atoi(itemtext);
	//rsltstr+=itemtext.Length;	//���֫������
	cMaterial=*rsltstr;
	if(cMaterial=='_'&&*(rsltstr+1)=='Q')
	{
		if(*(rsltstr+2)=='3')
			cMaterial=CSteelMatLibrary::Q345BriefMark();
		else if(*(rsltstr+2)=='4')
			cMaterial=CSteelMatLibrary::Q420BriefMark();
		else
			cMaterial='S';
		rsltstr+=4;
	}
	rsltstr++;	//��������ַ�����
	while(*rsltstr==' '||*rsltstr=='\t')
		rsltstr++;	//�������ո��
	if((spliter=SearchChar(rsltstr,' '))!=NULL)
		itemtext.NCopy(rsltstr,spliter-rsltstr);
	length=atof(itemtext)*1000;
	rsltstr+=itemtext.Length;	//�����Ԫ��������
	while(*rsltstr==' '||*rsltstr=='\t')
		rsltstr++;	//�������ո��
	rsltstr=ReadNextItemStr(rsltstr,itemtext,12,9);
	ulength=atof(itemtext)*10;
	//rsltstr+=itemtext.Length;	//�����Ԫ���㳤������
	rsltstr=ReadNextItemStr(rsltstr,itemtext,10,9);
	gyration=atof(itemtext)*10;	//cm->mm
	//rsltstr+=itemtext.Length;	//�����ת�뾶����
	rsltstr=ReadNextItemStr(rsltstr,itemtext,10,9);
	lamda=atof(itemtext);
	//rsltstr+=itemtext.Length;	//�����ϸ������
	rsltstr=ReadNextItemStr(rsltstr,itemtext,10,9);
	allowLamda=atof(itemtext);
	//rsltstr+=itemtext.Length;	//�������ϸ������
	rsltstr=ReadNextItemStr(rsltstr,itemtext,10,9);
	stableFactor=fabs(atof(itemtext));	//TODO:��̫�������ѹ��ϵ��������������ʲô wjh-2017.5.4
	//rsltstr+=itemtext.Length;	//���ѹ��ϵ������
	rsltstr=ReadNextItemStr(rsltstr,itemtext,10,9);
	maxTension=atof(itemtext)*1000;	//kN->N
	//rsltstr+=itemtext.Length;	//��������������
	//itemtext.NCopy(rsltstr,6);
	idMaxTensionCase=0;//atoi(itemtext);//TODO:���ศ��û�п��ƹ���
	//rsltstr+=6;	//��������������-��������
	rsltstr=ReadNextItemStr(rsltstr,itemtext,10,9);
	maxCompression=atof(itemtext)*1000;	//kN->N
	//rsltstr+=itemtext.Length;	//������ѹ������
	//itemtext.NCopy(rsltstr,6);
	idMaxCompressionCase=0;//atoi(itemtext);
	//rsltstr+=6;	//������ѹ������-��������
	rsltstr=ReadNextItemStr(rsltstr,itemtext,10,9);
	strengthDeductCoef=atof(itemtext);
	//rsltstr+=itemtext.Length;	//���ǿ���ۼ�ϵ������
	//����Ӧ�����Ρ�����Ӧ�����Ρ���ԪЧ�ʼ���˨��Ϣ
	//itemtext.NCopy(rsltstr,6);
	char* key=strtok(rsltstr,"MXx ");
	if(key!=NULL)
	{	//����Ӧ������
		actualStress=atof(key)*10;	//MPa
		key=strtok(NULL,"MXx ");
	}
	if(key!=NULL)
	{	//����Ӧ������
		allowStress=atof(key)*10;	//kN/cm^2->MPa
		key=strtok(NULL,"MXx ");
	}
	if(key!=NULL)
	{	//��ԪЧ������
		efficiency=atof(key);
		key=strtok(NULL,"MXx ");
	}
	//��ԪЧ�ʼ���˨��Ϣ
	/*
		if(key!=NULL)
		{
			boltgrade=(float)atof(key);
			key=strtok(NULL,"MXx ");
		}
		if(key!=NULL)
		{
			boltd=atoi(key);
			key=strtok(NULL,"MXx ");
		}
		if(key!=NULL)
			boltN=atoi(key);
	*/
	if((spliter=SearchChar(szOriginalRsltStr,'M'))==NULL)
		return false;
	itemtext.Copy(spliter-4);
	key=strtok(itemtext,"MXx ");
	if(key!=NULL)
	{
		boltgrade=(float)atof(key);
		key=strtok(NULL,"MXx ");
	}
	if(key!=NULL)
	{
		boltd=atoi(key);
		key=strtok(NULL,"MXx ");
	}
	if(key!=NULL)
		boltN=atoi(key);
	return true;
}
void CTtaDataFile::ReadOutFile(FILE* fp,FILE* auxout_fp2/*=NULL*/,void* pRptObj/*=NULL*/,char ciFileType/*=0*/)
{
	CPostProcessReport* pReport=(CPostProcessReport*)pRptObj;
	if(pReport==NULL)
		pReport=&postreport;
	pReport->Empty();
	pReport->m_iMemberSizeCheckCode=0xff;	//��ȡTTA���ļ�
	char sText[201]="";
	bool bStartElemSection=false;
	CMapList<long>mapping;	//��ǰLDSģ�͵�Ԫ�뵼��TTAģ�͵�Ԫ���ӳ�䣨���ݽڵ��ţ�,����ֵΪ��Ԫ��ʶId
	CHashList<long>hashModuleI,hashCaseI;
	CHashSet<CLDSNode*>hashPointI;
	if(pRptObj!=NULL)
	{
		for(CElement* pElem=postreport.hashElems.GetFirst();pElem;pElem=postreport.hashElems.GetNext())
		{
			UINT minPointI=min(pElem->pStart->pointI,pElem->pEnd->pointI);
			UINT maxPointI=max(pElem->pStart->pointI,pElem->pEnd->pointI);
			mapping.SetValue(minPointI,maxPointI,pElem->id);
		}
		for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			if(pNode->pointI>0)
				hashPointI.SetValue(pNode->pointI,pNode);
		}
		for(CLDSModule* pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
			hashModuleI.SetValue(pModule->iNo,pModule->handle);
		for(CWorkStatus* pCase=Ta.WorkStatus.GetFirst();pCase;pCase=Ta.WorkStatus.GetNext())
			hashCaseI.SetValue(pCase->iNo,pCase->handle);
	}

	while(!feof(fp))
	{
		ReadLineText(sText,200,fp);
		//if(!bStartElemSection &&
		//	strstr(sText,"MEMBER     SIZE      LEN.  U.LEN.   G.R.    S.R.  A.S.R. S.FAC.")!=NULL)
		if(!bStartElemSection &&strstr(sText,"MEMBER")!=NULL&&strstr(sText,"SIZE")!=NULL&&
			strstr(sText,"U.LEN")!=NULL&&strstr(sText,"A.S.R")!=NULL&&strstr(sText,"S.FAC.")!=NULL)
			bStartElemSection=true;
		if(!bStartElemSection)
			continue;
		if( strstr(sText,"MEMBER")!=NULL || strstr(sText,"====================")!=NULL ||
			strstr(sText,"--------------------")!=NULL || strlen(sText)<100)
			continue;
		if( strstr(sText,"TOWER       WEIGHT")!=NULL||
			strstr(sText,"����")!=NULL)
			break;
		//��Ч��Ԫ�����
		CXhChar200 linetext=sText;
		char* key=strtok(sText,"- \t");
		TTAELEM_DESIGN_RSLT elemrslt;
		elemrslt.startPointI=0,elemrslt.endPointI=0;
		while(key!=NULL)
		{
			char* separator=NULL;
			if(elemrslt.startPointI==0)
				elemrslt.startPointI=atoi(key);
			else if(elemrslt.endPointI==0)
				elemrslt.endPointI=atoi(key);
			else
				separator=strchr(key,'M');
			key=strtok(NULL,"- \t");
			if(elemrslt.startPointI>0&&elemrslt.endPointI>0&&key!=NULL)
			{
				if(ciFileType== FILETYPE_TANS)	//����Ժ���
					elemrslt.InitFromTANSElemRsltStr((char*)linetext+(key-&sText[0]));
				else
					elemrslt.InitFromElemRsltStr((char*)linetext+(key-&sText[0]),ciFileType);
				break;
			}
		}
		if(elemrslt.startPointI==0||elemrslt.endPointI==0)
			continue;
		else if(elemrslt.size.cSubClassType!='R'&&(elemrslt.boltd==0||elemrslt.boltN==0))
			continue;
		if(pRptObj!=NULL)
		{
			UINT minPointI=min(elemrslt.startPointI,elemrslt.endPointI);
			UINT maxPointI=max(elemrslt.startPointI,elemrslt.endPointI);
			long* pId=mapping.GetValue(minPointI,maxPointI);
			if(pId==NULL)
			{
				logerr.Log("error");
				continue;
			}
			else
			{
				CElement* pElem=pReport->hashElems.Add(*pId);
				pElem->id=*pId;
				pElem->pStart=hashPointI.GetValue(elemrslt.startPointI);
				pElem->pEnd=hashPointI.GetValue(elemrslt.endPointI);
				pElem->relaElem.bFatherElem=true;
				pElem->design_info.Lo_r=elemrslt.lamda;			//���㳤ϸ��(��K����ϵ��)
				pElem->design_info.lamdaL = elemrslt.ulength;	//ִ�еļ��㳤��
				pElem->design_info.lamdaR = elemrslt.gyration;	//ִ�еĻ�ת�뾶
				pElem->MaxCompression.N=elemrslt.maxCompression;
				pElem->MaxTension.N=elemrslt.maxTension;
				if(elemrslt.idMaxTensionCase>1000)
				{
					long* phModule=hashModuleI.GetValue(elemrslt.idMaxTensionCase/1000);
					pElem->MaxTension.hModule=(phModule!=NULL) ? *phModule : 0;	//���ߺ�
					long* phCase=hashCaseI.GetValue(elemrslt.idMaxTensionCase%1000);
					pElem->MaxTension.hWorkStatus=(phCase!=NULL) ? *phCase : 0;	//������
				}
				else
				{
					long* phModule=hashModuleI.GetValue(elemrslt.idMaxTensionCase/100);
					pElem->MaxTension.hModule=(phModule!=NULL) ? *phModule : 0;	//���ߺ�
					long* phCase=hashCaseI.GetValue(elemrslt.idMaxTensionCase%100);
					pElem->MaxTension.hWorkStatus=(phCase!=NULL) ? *phCase : 0;	//������
				}
				if(elemrslt.idMaxCompressionCase>1000)
				{
					long* phModule=hashModuleI.GetValue(elemrslt.idMaxCompressionCase/1000);
					pElem->MaxCompression.hModule=(phModule!=NULL) ? *phModule : 0;	//���ߺ�
					long* phCase=hashCaseI.GetValue(elemrslt.idMaxCompressionCase%1000);
					pElem->MaxCompression.hWorkStatus=(phCase!=NULL) ? *phCase : 0;	//������
				}
				else
				{
					long* phModule=hashModuleI.GetValue(elemrslt.idMaxCompressionCase/100);
					pElem->MaxCompression.hModule=(phModule!=NULL) ? *phModule : 0;	//���ߺ�
					long* phCase=hashCaseI.GetValue(elemrslt.idMaxCompressionCase%100);
					pElem->MaxCompression.hWorkStatus=(phCase!=NULL) ? *phCase : 0;	//������
				}
				pElem->design_info.fCoefEffic=pElem->design_info.fCoefOfBearLoad=elemrslt.efficiency;
				pElem->design_info.fCoefStablity=fabs(elemrslt.stableFactor);
				pElem->design_info.workSigema=elemrslt.actualStress;
				if(elemrslt.actualStress<0)
				{	//��ѹ����
					pElem->design_info.fCoefCompStrengthDecrease=elemrslt.strengthDeductCoef;
					if(elemrslt.stableFactor>0&&elemrslt.length>0)
						pElem->design_info.iDangerousLoadCase=3;	//������ѹǿ�ȿ���
					else if(elemrslt.stableFactor<0)
					{
						pElem->design_info.iDangerousLoadCase=4;	//��ѹ�ȶ�����
						pElem->MaxCompStable.bothComp=elemrslt.length<0;
						pElem->MaxCompStable.hModule=pElem->MaxCompression.hModule;
						pElem->MaxCompStable.hWorkStatus=pElem->MaxCompression.hWorkStatus;
						pElem->MaxCompStable.N=pElem->MaxCompression.N;
						pElem->MaxCompStable.sigema=elemrslt.actualStress;
					}
				}
				else
				{	//��������
					pElem->design_info.fCoefTensStrengthDecrease=elemrslt.strengthDeductCoef;
					pElem->design_info.iDangerousLoadCase=2;	//����ǿ�ȿ���
				}
			}
			continue;
		}
		for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		{
			if(pRod->pStart&&pRod->pEnd&&((pRod->pStart->point_i==elemrslt.startPointI&&pRod->pEnd->point_i==elemrslt.endPointI)||
				(pRod->pStart->point_i==elemrslt.endPointI&&pRod->pEnd->point_i==elemrslt.startPointI)))
			{
				_snprintf(pRod->connectStart.grade,4,"%.1f",elemrslt.boltgrade);
				strcpy(pRod->connectEnd.grade,pRod->connectStart.grade);
				if(elemrslt.size.cSubClassType=='R')
					pRod->size_idClassType=CLS_LINETUBE;
				if(pRod->size_idClassType==CLS_LINEANGLE||pRod->size_idClassType==CLS_GROUPLINEANGLE)
				{
					int nSubAngles=ftoi(elemrslt.size.wide)/1000;
					pRod->size_wide=ftoi(elemrslt.size.wide)%1000;
					if(nSubAngles<2)
						pRod->size_idClassType=CLS_LINEANGLE;
					else if(nSubAngles==2)
					{
						pRod->size_idClassType=CLS_GROUPLINEANGLE;
						if(pRod->size_cSubClassType =='L')
							pRod->size_cSubClassType='D';
					}
					else if(nSubAngles==4)
					{
						pRod->size_idClassType=CLS_GROUPLINEANGLE;
						pRod->size_cSubClassType='X';
					}
				}
				else
					pRod->size_wide=elemrslt.size.wide;
				pRod->size_thick=elemrslt.size.thick;
				//TODO:��ǿ��ʵ��Ӧ�ӵ���Q460Q235.ini�ļ��ж�ȡ�򻯷��ţ�Ŀǰ��ʱд�� wjh-2017.5.12
				if(QuerySteelMatByBriefMark(elemrslt.cMaterial)!=NULL)
					pRod->cMaterial=elemrslt.cMaterial;
				else if(elemrslt.cMaterial=='A')
					pRod->cMaterial=CSteelMatLibrary::Q390BriefMark();
				else if(elemrslt.cMaterial=='B')
					pRod->cMaterial=CSteelMatLibrary::Q420BriefMark();
				else if(elemrslt.cMaterial=='C')
					pRod->cMaterial=CSteelMatLibrary::Q460BriefMark();
				pRod->connectStart.d    =pRod->connectEnd.d=elemrslt.boltd;
				pRod->connectStart.wnSummBoltN=pRod->connectEnd.wnSummBoltN=elemrslt.boltN;
				pRod->SyncMirProp(NULL);
				break;
			}
		}
	}
	if(ciFileType!=3)
		ReadDaoHengAuxElemOutFile(auxout_fp2,pRptObj,ciFileType);
}
void CTtaDataFile::ReadDaoHengAuxElemOutFile(FILE* fp,void* pRptObj/*=NULL*/,char ciFileType/*=0*/)
{
	if(fp==NULL)
		return;
	CPostProcessReport* pReport=(CPostProcessReport*)pRptObj;
	if(pReport==NULL)
		pReport=&postreport;
	pReport->Empty();
	pReport->m_iMemberSizeCheckCode=0xff;	//��ȡTTA���ļ�
	char sText[201]="";
	bool bStartElemSection=false;
	CMapList<long>mapping;	//��ǰLDSģ�͵�Ԫ�뵼��TTAģ�͵�Ԫ���ӳ�䣨���ݽڵ��ţ�,����ֵΪ��Ԫ��ʶId
	CHashList<long>hashModuleI,hashCaseI;
	CHashSet<CLDSNode*>hashPointI;

	while(!feof(fp))
	{
		ReadLineText(sText,200,fp);
		if(!bStartElemSection &&strstr(sText,"MEMBER")!=NULL&&strstr(sText,"SIZE")!=NULL&&
			strstr(sText,"U.LEN")!=NULL&&strstr(sText,"A.S.R")!=NULL&&strstr(sText,"S.FAC.")!=NULL)
		//if(!bStartElemSection &&
		//	strstr(sText,"MEMBER                 SIZE      LEN.   U.LEN.     G.R.       S.R.    A.S.R.  S.FAC.")!=NULL)
			bStartElemSection=true;
		if(!bStartElemSection)
			continue;
		if( strstr(sText,"MEMBER")!=NULL || strstr(sText,"====================")!=NULL ||
			strstr(sText,"--------------------")!=NULL || strlen(sText)<100)
			continue;
		if( strstr(sText,"TOWER       WEIGHT")!=NULL||
			strstr(sText,"����")!=NULL)
			break;
		//��Ч��Ԫ�����
		CXhChar200 linetext=sText;
		char* key=strtok(sText,"- \t");
		TTAELEM_DESIGN_RSLT elemrslt;
		elemrslt.startPointI=0,elemrslt.endPointI=0;
		while(key!=NULL)
		{
			char* separator=NULL;
			if(elemrslt.startPointI==0)
				elemrslt.startPointI=atoi(key);
			else if(elemrslt.endPointI==0)
				elemrslt.endPointI=atoi(key);
			else
				separator=strchr(key,'M');
			if(strstr(linetext,"��")!=NULL)
				elemrslt.size.idClassType=CLS_LINETUBE;
			else if(strstr(linetext,"��")!=NULL)
				elemrslt.size.idClassType=CLS_LINETUBE;
			else //if(strchr(itemtext,'L')!=NULL)
				elemrslt.size.idClassType=CLS_LINEANGLE;
			key=strtok(NULL,"- \t");
			if(elemrslt.startPointI>0&&elemrslt.endPointI>0&&key!=NULL)
			{
				if (ciFileType == FILETYPE_TANS)	//����Ժ���
					elemrslt.InitFromTANSElemRsltStr((char*)linetext + (key - &sText[0]));
				else
					elemrslt.InitFromDaoHengAuxElemRsltStr((char*)linetext+(key-&sText[0]),ciFileType);
				break;
			}
		}
		if(elemrslt.startPointI==0||elemrslt.endPointI==0||elemrslt.boltd==0)//||elemrslt.boltN==0)
			continue;
		if(pRptObj!=NULL)
		{
			UINT minPointI=min(elemrslt.startPointI,elemrslt.endPointI);
			UINT maxPointI=max(elemrslt.startPointI,elemrslt.endPointI);
			long* pId=mapping.GetValue(minPointI,maxPointI);
			if(pId==NULL)
			{
				logerr.Log("error");
				continue;
			}
			else
			{
				CElement* pElem=pReport->hashElems.Add(*pId);
				pElem->id=*pId;
				pElem->pStart=hashPointI.GetValue(elemrslt.startPointI);
				pElem->pEnd=hashPointI.GetValue(elemrslt.endPointI);
				pElem->relaElem.bFatherElem=true;
				pElem->design_info.Lo_r=elemrslt.lamda;			//���㳤ϸ��(��K����ϵ��)
				pElem->design_info.lamdaL = elemrslt.ulength;	//ִ�еļ��㳤��
				pElem->design_info.lamdaR = elemrslt.gyration;	//ִ�еĻ�ת�뾶
				pElem->MaxCompression.N=elemrslt.maxCompression;
				pElem->MaxTension.N=elemrslt.maxTension;
				if(elemrslt.idMaxTensionCase>1000)
				{
					long* phModule=hashModuleI.GetValue(elemrslt.idMaxTensionCase/1000);
					pElem->MaxTension.hModule=(phModule!=NULL) ? *phModule : 0;	//���ߺ�
					long* phCase=hashCaseI.GetValue(elemrslt.idMaxTensionCase%1000);
					pElem->MaxTension.hWorkStatus=(phCase!=NULL) ? *phCase : 0;	//������
				}
				else
				{
					long* phModule=hashModuleI.GetValue(elemrslt.idMaxTensionCase/100);
					pElem->MaxTension.hModule=(phModule!=NULL) ? *phModule : 0;	//���ߺ�
					long* phCase=hashCaseI.GetValue(elemrslt.idMaxTensionCase%100);
					pElem->MaxTension.hWorkStatus=(phCase!=NULL) ? *phCase : 0;	//������
				}
				if(elemrslt.idMaxCompressionCase>1000)
				{
					long* phModule=hashModuleI.GetValue(elemrslt.idMaxCompressionCase/1000);
					pElem->MaxCompression.hModule=(phModule!=NULL) ? *phModule : 0;	//���ߺ�
					long* phCase=hashCaseI.GetValue(elemrslt.idMaxCompressionCase%1000);
					pElem->MaxCompression.hWorkStatus=(phCase!=NULL) ? *phCase : 0;	//������
				}
				else
				{
					long* phModule=hashModuleI.GetValue(elemrslt.idMaxCompressionCase/100);
					pElem->MaxCompression.hModule=(phModule!=NULL) ? *phModule : 0;	//���ߺ�
					long* phCase=hashCaseI.GetValue(elemrslt.idMaxCompressionCase%100);
					pElem->MaxCompression.hWorkStatus=(phCase!=NULL) ? *phCase : 0;	//������
				}
				pElem->design_info.fCoefEffic=pElem->design_info.fCoefOfBearLoad=elemrslt.efficiency;
				pElem->design_info.fCoefStablity=fabs(elemrslt.stableFactor);
				pElem->design_info.workSigema=elemrslt.actualStress;
				if(elemrslt.actualStress<0)
				{	//��ѹ����
					pElem->design_info.fCoefCompStrengthDecrease=elemrslt.strengthDeductCoef;
					if(elemrslt.stableFactor>0&&elemrslt.length>0)
						pElem->design_info.iDangerousLoadCase=3;	//������ѹǿ�ȿ���
					else if(elemrslt.stableFactor<0)
					{
						pElem->design_info.iDangerousLoadCase=4;	//��ѹ�ȶ�����
						pElem->MaxCompStable.bothComp=elemrslt.length<0;
						pElem->MaxCompStable.hModule=pElem->MaxCompression.hModule;
						pElem->MaxCompStable.hWorkStatus=pElem->MaxCompression.hWorkStatus;
						pElem->MaxCompStable.N=pElem->MaxCompression.N;
						pElem->MaxCompStable.sigema=elemrslt.actualStress;
					}
				}
				else
				{	//��������
					pElem->design_info.fCoefTensStrengthDecrease=elemrslt.strengthDeductCoef;
					pElem->design_info.iDangerousLoadCase=2;	//����ǿ�ȿ���
				}
			}
			continue;
		}
		for(TTA_BRACEELEM* pBraceElem=listTtaBraceElems.GetFirst();pBraceElem;pBraceElem=listTtaBraceElems.GetNext())
		{
			CLDSLinePart* pRod=pBraceElem->pRod;
			//�������ֱ����pRod�����������ڽڵ�λ���ظ������ܵ���ƥ�䲻��
			if(pBraceElem->pStart&&pBraceElem->pEnd&&((pBraceElem->pStart->pointI==elemrslt.startPointI&&pBraceElem->pEnd->pointI==elemrslt.endPointI)||
				(pBraceElem->pStart->pointI==elemrslt.endPointI&&pBraceElem->pEnd->pointI==elemrslt.startPointI)))
			{
				_snprintf(pRod->connectStart.grade,4,"%.1f",elemrslt.boltgrade);
				strcpy(pRod->connectEnd.grade,pRod->connectStart.grade);
				pRod->connectStart.d    =pRod->connectEnd.d=elemrslt.boltd;
				pRod->connectStart.wnSummBoltN=pRod->connectEnd.wnSummBoltN=elemrslt.boltN;
				if(pRod->size_idClassType==0)
					pRod->size_idClassType=elemrslt.size.idClassType;
				if(pRod->size_idClassType==CLS_LINEANGLE||pRod->size_idClassType==CLS_GROUPLINEANGLE)
				{
					int nSubAngles=ftoi(elemrslt.size.wide)/1000;
					pRod->size_wide=ftoi(elemrslt.size.wide)%1000;
					if(nSubAngles<2)
						pRod->size_idClassType=CLS_LINEANGLE;
					else if(nSubAngles==2)
					{
						pRod->size_idClassType=CLS_GROUPLINEANGLE;
						pRod->size_cSubClassType='D';
					}
					else if(nSubAngles==4)
					{
						pRod->size_idClassType=CLS_GROUPLINEANGLE;
						pRod->size_cSubClassType='X';
					}
				}
				else
					pRod->size_wide=elemrslt.size.wide;
				pRod->size_thick=elemrslt.size.thick;
				//TODO:��ǿ��ʵ��Ӧ�ӵ���Q460Q235.ini�ļ��ж�ȡ�򻯷��ţ�Ŀǰ��ʱд�� wjh-2017.5.12
				if(QuerySteelMatByBriefMark(elemrslt.cMaterial)!=NULL)
					pRod->cMaterial=elemrslt.cMaterial;
				else if(elemrslt.cMaterial=='A')
					pRod->cMaterial=CSteelMatLibrary::Q390BriefMark();
				else if(elemrslt.cMaterial=='B')
					pRod->cMaterial=CSteelMatLibrary::Q420BriefMark();
				else if(elemrslt.cMaterial=='C')
					pRod->cMaterial=CSteelMatLibrary::Q460BriefMark();
				CLDSLinePartPtr parrMirRod[3]={NULL};
				parrMirRod[0]=(pBraceElem->IS==2||pBraceElem->IS==4)?(CLDSLinePart*)pRod->GetMirRod(MIRMSG(1)):NULL;
				parrMirRod[1]=(pBraceElem->IS==1||pBraceElem->IS==4)?(CLDSLinePart*)pRod->GetMirRod(MIRMSG(2)):NULL;
				parrMirRod[2]=(pBraceElem->IS==3||pBraceElem->IS==4)?(CLDSLinePart*)pRod->GetMirRod(MIRMSG(3)):NULL;
				for(int i=0;i<3;i++)
				{
					if(parrMirRod[i]==NULL)
						continue;
					parrMirRod[i]->cMaterial=pRod->cMaterial;
					parrMirRod[i]->size_idClassType=pRod->size_idClassType;
					parrMirRod[i]->size_cSubClassType=pRod->size_cSubClassType;
					parrMirRod[i]->size_wide =pRod->size_wide;
					parrMirRod[i]->size_thick=pRod->size_thick;
					parrMirRod[i]->connectStart=pRod->connectStart;
					parrMirRod[i]->connectEnd=pRod->connectEnd;
				}
				break;
			}
		}
	}
}
void CTtaDataFile::WriteDataFile(FILE *fp)
{
	int i;
	if(!PrepareTtaModel())
		return;
	//һ��������Ϣ����
	fprintf(fp,"%s\n",Ta.m_sPrjCode);
	fprintf(fp,"%s\n",Ta.m_sPrjName);
	fprintf(fp,"%s\n",Ta.m_sOperator);
	CXhChar50 itemText;
	CXhChar100 lineText;
	//����������Ϣ����
	TSM=1;	//TSM������Ϊ�㣬TTA����Ҫ*.mem�ļ���������
	//KIND  N10	N20	M10	M20	IE	NL	NC	NW	NSEC
	//��K1  K2	K3	K4	K5	K6	NA	TSM	REN	��NGP
	fprintf(fp,"%4d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n",KIND,N10,N20,M10,M20,IE,NL, NC, NW,NSEC);
	fprintf(fp,"%4d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n",K1  ,K2 ,K3 ,K4 ,K5 ,K6,NA,TSM,REN,NGP);
	//�����ڵ�����
	for(i=0;i<m_arrBodyLeg.GetSize();i++)
	{
		fprintf(fp,"%5d%5d%5d%5d%5d%5d\n",m_arrBodyLeg[i].Jmin,m_arrBodyLeg[i].Fmin,m_arrBodyLeg[i].Fmax,
			m_arrBodyLeg[i].MAX,m_arrBodyLeg[i].LB,m_arrBodyLeg[i].Var);
	}
	//�ġ����ط�������
		//�ݲ�֧�ֺ��ط���
	//�塢��һ�ֽڵ����뷽ʽ����
	for(i=0;i<m_arrTtaNodePtr1.GetSize();i++)
	{
		if(m_arrTtaNodePtr1[i]->posType>=4)
			continue;	//�ڶ���ڵ�
		fprintf(fp,"%5d%3d%10.3f%10.3f%10.3f\n",m_arrTtaNodePtr1[i]->pointI,m_arrTtaNodePtr1[i]->IS,
			m_arrTtaNodePtr1[i]->pos.x,m_arrTtaNodePtr1[i]->pos.y,m_arrTtaNodePtr1[i]->pos.z);
	}
	//�����ڶ��ֽڵ����뷽ʽ����
	for(i=0;i<m_arrTtaNodePtr2.GetSize();i++)
	{
		if(m_arrTtaNodePtr2[i]->posType<4)
			continue;	//��һ��ڵ�
		if(fabs(m_arrTtaNodePtr2[i]->coefScale)>1)
		{
			if(m_arrTtaNodePtr2[i]->coefScale>0)
			{
				fprintf(fp,"%5d%5d%5d%3d%3d\n",m_arrTtaNodePtr2[i]->pointI,m_arrTtaNodePtr2[i]->J1,
					m_arrTtaNodePtr2[i]->J2,(int)m_arrTtaNodePtr2[i]->coefScale,m_arrTtaNodePtr2[i]->IS);
			}
			else
			{	//��Ҫ����ʼĩ�������ڵ��
				fprintf(fp,"%5d%5d%5d%3d%3d\n",m_arrTtaNodePtr2[i]->pointI,m_arrTtaNodePtr2[i]->J2,
					m_arrTtaNodePtr2[i]->J1,(int)(-m_arrTtaNodePtr2[i]->coefScale),m_arrTtaNodePtr2[i]->IS);
			}
		}
		else
		{
			fprintf(fp,"%5d%5d%5d %4.3f%3d\n",m_arrTtaNodePtr2[i]->pointI,m_arrTtaNodePtr2[i]->J1,
				m_arrTtaNodePtr2[i]->J2,m_arrTtaNodePtr2[i]->coefScale,m_arrTtaNodePtr2[i]->IS);
		}
	}
	//�ߡ�����������
	for(i=0;i<m_arrTtaElem.GetSize();i++)
	{
		fprintf(fp,"%5d%5d%5d%5d%5d%5d%5d%5d%5d\n",m_arrTtaElem[i].pStart->pointI,m_arrTtaElem[i].pEnd->pointI,
			m_arrTtaElem[i].N,m_arrTtaElem[i].IS,m_arrTtaElem[i].TYPE,m_arrTtaElem[i].M,
			m_arrTtaElem[i].L1,m_arrTtaElem[i].L2,m_arrTtaElem[i].SMC);
	}
	//�ˡ�����������
		// ����TTA������֧�ֲ��ã��ݲ����������
	//�š��������ػ������ݼ��������ϵ��
	fprintf(fp,"%5d%5d%5d  %C\n",m_arrWindSeg.GetSize(),0,0,Ta.m_cLandscape);
	CWorkStatus *pStatus;
	
	if(Ta.m_fTowerHeight==0)
	{
		SCOPE_STRU scope;
		Ta.GetTaSizeScope(scope);
		Ta.m_fTowerHeight=scope.depth()/1000;	//mm->m
	}
	for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
	{
		double betaZ=1.0;
		/** ����ϵ���Ѽ��뵽��ѹ����
		if(Ta.m_fTowerHeight<=20)
			betaZ=1.0;
		else if(Ta.m_fTowerHeight<=30)
			betaZ=1.0+0.025*(Ta.m_fTowerHeight-20);
		else if(Ta.m_fTowerHeight<=40)
			betaZ=1.25+0.01*(Ta.m_fTowerHeight-30);
		else if(Ta.m_fTowerHeight<=50)
			betaZ=1.35+0.015*(Ta.m_fTowerHeight-40);
		else //if(m_fTowerHeight<=60)
			betaZ=1.5+0.01*(Ta.m_fTowerHeight-50);*/
		betaZ*=pStatus->Bw_I();
		fprintf(fp,"%8.3f%8.3f%5d.000   1.000       %s\n",pStatus->WindLoad.velocity,
			pStatus->Kesai_W()*betaZ,pStatus->WindLoad.azimuth,pStatus->description);
	}
	//ʮ�������ߺ�������
	for(i=0;i<m_arrTtaNode.GetSize();i++)
	{
		bool bLoadNode=false;
		CExternalNodeLoad *pNodeLoad=NULL;
		CWorkStatus *pFirstStatus=Ta.WorkStatus.GetFirst();
		for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
		{
			pNodeLoad=pStatus->hashNodeLoad.GetValue(m_arrTtaNode[i].pNode->handle);
			if(pNodeLoad!=NULL)
			{
				bLoadNode=true;
				break;
			}
		}
		if(!bLoadNode)
			continue;	//�Ǻ��ؽڵ�
		for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
		{
			f3dPoint Fxyz;
			pNodeLoad=pStatus->hashNodeLoad.GetValue(m_arrTtaNode[i].pNode->handle);
			if(pNodeLoad)
			{
				if(pStatus->m_bLoadIncFactor)
					Fxyz=pNodeLoad->designLoad*0.001;
				else
				{
					Fxyz=pNodeLoad->Fxyz*pStatus->GamaQ()*pStatus->Kesai()/1000;	//N->kN
					Fxyz.z+=pNodeLoad->permanentFz*Ta.m_fGamaGForDesign/1000;					//N->kN
					if(pStatus->m_cMainLoadType!=4)
						Fxyz*=Ta.SafetyCoef();
				}
			}
			if(pStatus==pFirstStatus)
				fprintf(fp,"%5d     %9.2f%9.2f%9.2f\n",m_arrTtaNode[i].pointI,Fxyz.x,Fxyz.y,Fxyz.z);
			else
				fprintf(fp,"          %9.2f%9.2f%9.2f\n",Fxyz.x,Fxyz.y,Fxyz.z);
		}
	}
	//ʮһ��������������
	for(i=0;i<m_arrWindSeg.GetSize();i++)
	{
		fprintf(fp,"%5d%5d%5d%5d%5d%5d%5d%5d%5d%5d\n",m_arrWindSeg[i].TYPE,m_arrWindSeg[i].arrPointI[0],
			m_arrWindSeg[i].arrPointI[1],m_arrWindSeg[i].arrPointI[2],m_arrWindSeg[i].arrPointI[3],
			m_arrWindSeg[i].NLG,m_arrWindSeg[i].Fa,m_arrWindSeg[i].Fb,m_arrWindSeg[i].Fw,m_arrWindSeg[i].BetaZ);
	}
	//ʮ������ͼ����
	fprintf(fp,"    0    3   10");	//�����д���
}
#include "folder_dialog.h"
static int AddIdxFzcPanelFiles(const char* szIdxFileName,CXhSimpleList<GTM_FZC_FNAME> *pListFzcPanelFiles)
{
	FILE* fp=fopen(szIdxFileName,"rt");
	if (fp==NULL)
		return 0;
	CSelfCloseFile close(fp);
	char fzcname[200]={0};
	CXhString szText(fzcname,200);
	int nhits=0;
	while (!feof(fp))
	{
		if (ReadEffectLineText(szText,200,fp)==NULL)
			break;	//���һ����󣬿��ܻ���� ��ȡʧ�ܷ���NULL���
		char* pchFinalSymbol=strchr(szText,';');
		if (pchFinalSymbol!=NULL)
			*pchFinalSymbol=0;	//��֮���ע��ȥ��
		char ciHeadSymbol=0;
		if (fzcname[0]=='B'||fzcname[0]=='b')
			ciHeadSymbol='B';	//�˸�����ƬΪ��������Ƭ
		else if (fzcname[0]=='C'||fzcname[0]=='c')
			ciHeadSymbol='C';	//�˸�����ƬΪ�ᵣ������Ƭ
		else if (fzcname[0]=='R'||fzcname[0]=='r')
			ciHeadSymbol='R';	//�����ڵ��Ϊ������
		else if (fzcname[0]=='A'||fzcname[0]=='a')
			ciHeadSymbol='A';	//�����ڵ��Ϊ��ģ�;��Ե��
		if (ciHeadSymbol!=0)	//ǰ��һλ��ȥ���ļ����ͱ�ʶ�ַ�
			memmove(fzcname,fzcname+1,199);
		GTM_FZC_FNAME fzcfile(szText,0);
		fzcfile.ciPrefixSymbol=ciHeadSymbol;
		if (strstr(fzcname,"Leg")==NULL)
		{	//��������Ƭ�жϴ���
			char* pchDelimiter=strchr(fzcname,'#');
			int ciMirSymbol=pchDelimiter!=0 ? (*(pchDelimiter+5))-'0' : 0;
			if(pchDelimiter!=NULL)
				*pchDelimiter=0;
			if (TestValidGTowerBodyFzcFileName(fzcname,&fzcfile))
			{
				pListFzcPanelFiles->AttachObject(fzcfile);
				//logerr.Log("Fzc File@%3d#%s",nhits+1,(char*)fzcfile.fname);
				nhits++;
			}
		}
		else
		{	//�����������V�渨����Ƭ
			fzcfile.ciFzcPanelType=1;
			char* pchDelimiter=strchr(fzcname,'#');
			if (pchDelimiter!=NULL)
			{
				CXhChar100 label;
				CXhChar50 xarrPointLabels[5];
				label.NCopy(fzcname,pchDelimiter-fzcname);
				int n=DELIMITER_STR::ParseWordsFromStr(label,"-",xarrPointLabels,5);
				if (n>0)
					fzcfile.liPointTop=atoi(xarrPointLabels[0]);
				if (n>1)
					fzcfile.liPointBase=atoi(xarrPointLabels[1]);
				if (n>2)
					fzcfile.subleg.liVTopPointA=atoi(xarrPointLabels[2]);
				if (n>3)
					fzcfile.subleg.liVTopPointB=atoi(xarrPointLabels[3]);
				fzcfile.fname.Copy(pchDelimiter+1);
			}
			//else//�����ļ��������'#'�ָ��
			if (TestValidGTowerLegFzcFileName(fzcfile.fname,&fzcfile))
			{	//�����Ƿ�Ϊ�Ϸ������ȸ�����Ƭ�ļ�
				pListFzcPanelFiles->AttachObject(fzcfile);
				//logerr.Log("Fzc File@%3d#%s",nhits+1,(char*)fzcfile.fname);
				nhits++;
			}
		}
	}
	return nhits;
}
int CLDSView::AddGtmBraceElemPanels()
{	//��ȡ�㶫Ժ�ĸ��ļ����ļ�
	/*CXhChar200 szFilePath=szDataFilePathName;
	_splitpath(szFilePath,NULL,NULL,NULL,NULL);
	char* pchSlash=SearchChar(szFilePath,'\\',true);
	if(pchSlash==NULL)
		pchSlash=SearchChar(szFilePath,'/',true);
	if(pchSlash)
		*pchSlash=0;*/
	CString szBraceElemFilePath=theApp.szWorkPath;
	if (!InvokeFolderPickerDlg(szBraceElemFilePath))
		return 0;
	UINT uiMaxPointI=0;
	long hMinCanUseHandle=Ta.GetMinAvailableHandle();
	for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		uiMaxPointI=max(uiMaxPointI,pNode->point_i);
	uiMaxPointI-=uiMaxPointI%10;
	if(uiMaxPointI<10000)
		uiMaxPointI=10000;

	DisplayProcess(0,"��ȡGTower���򸨲ļ����ļ�...");
	HANDLE hFindFile;
	WIN32_FIND_DATA xFindFileData;
	CXhChar200 szFilePathOfIDX("%s\\*.idx",szBraceElemFilePath);
	//��ȡGTower������Ƭ�����ļ�
	CXhSimpleList<CXhChar100> listFzcIdxFiles;
	CXhSimpleList<GTM_FZC_FNAME> listFzcPanelFiles;
	if((hFindFile=FindFirstFile(szFilePathOfIDX, &xFindFileData))!=INVALID_HANDLE_VALUE)
	{
		do {
			listFzcIdxFiles.AttachObject(xFindFileData.cFileName);
		} while (FindNextFile(hFindFile,&xFindFileData));
		FindClose(hFindFile);
	}
	if (listFzcIdxFiles.Count>=0)
	{
		for (CXhChar100* pIdxFileName=listFzcIdxFiles.EnumObjectFirst();pIdxFileName;pIdxFileName=listFzcIdxFiles.EnumObjectNext())
		{
			CXhChar500 szIdxFullPathName("%s\\%s",szBraceElemFilePath,(char*)(*pIdxFileName));
			AddIdxFzcPanelFiles(szIdxFullPathName,&listFzcPanelFiles);
		}
	}
	else
	{
		//��ȡ��������Ƭ�ļ�
		CXhChar200 szFilePathOfBodyFZC("%s\\*.",szBraceElemFilePath);
		if ((hFindFile=FindFirstFile(szFilePathOfBodyFZC,&xFindFileData))!=INVALID_HANDLE_VALUE)
		{
			do {
				CXhChar500 szFzcPath("%s\\%s",szBraceElemFilePath,xFindFileData.cFileName);
				GTM_FZC_FNAME fzcfile(szFzcPath,0);
				if (!TestValidGTowerBodyFzcFileName(xFindFileData.cFileName,&fzcfile))
					continue;
				listFzcPanelFiles.AttachObject(fzcfile);
				logerr.Log("Fzc File#%s",xFindFileData.cFileName);
			} while (FindNextFile(hFindFile,&xFindFileData));
			FindClose(hFindFile);
		}
		//��ȡ���ȸ�����Ƭ�ļ�
		CXhChar200 szFilePathLegFZC("%s\\*.*",szBraceElemFilePath);
		if ((hFindFile=FindFirstFile(szFilePathLegFZC,&xFindFileData))!=INVALID_HANDLE_VALUE)
		{
			do {
				CXhChar500 szFzcPath("%s\\%s",szBraceElemFilePath,xFindFileData.cFileName);
				GTM_FZC_FNAME fzcfile(szFzcPath,1);
				if (!TestValidGTowerLegFzcFileName(xFindFileData.cFileName,&fzcfile))
					continue;
				listFzcPanelFiles.AttachObject(fzcfile);
				logerr.Log("Fzc File-%d#%s",fzcfile.ciFzcPanelType,xFindFileData.cFileName);
			} while (FindNextFile(hFindFile,&xFindFileData));
			FindClose(hFindFile);
		}
	}
	int i=0,niCount=listFzcPanelFiles.Count;
	for(GTM_FZC_FNAME* pFzcPanel=listFzcPanelFiles.EnumObjectFirst();pFzcPanel!=NULL;
		pFzcPanel=listFzcPanelFiles.EnumObjectNext(),i++)
	{
		i=1;
		CXhChar50 _szTitle("��ȡ���ļ����ļ�%d/%d...",i+1,niCount);
		DisplayProcess(ftoi(i*100/niCount),_szTitle);
		GECS acs;	//������Ƭ��������ģ���е�װ������ϵ
		GECS lcs;	//������Ƭ�ڵĶ�λ����ϵ
		GTM_KEYNODE_MAP xarrKeyNodes[4];
		if (pFzcPanel->ciFzcPanelType==0)
		{	//��������ؼ��ڵ�
			xarrKeyNodes[0].pMainModelNode=Ta.FromPointI(pFzcPanel->liPointA);
			xarrKeyNodes[1].pMainModelNode=Ta.FromPointI(pFzcPanel->liPointB);
		}
		else
		{
			xarrKeyNodes[0].pMainModelNode=pFzcPanel->liPointTop>0?Ta.FromPointI(pFzcPanel->liPointTop):NULL;
			xarrKeyNodes[1].pMainModelNode=pFzcPanel->liPointBase>0?Ta.FromPointI(pFzcPanel->liPointBase):NULL;
			xarrKeyNodes[2].pMainModelNode=pFzcPanel->subleg.liVTopPointA>0?Ta.FromPointI(pFzcPanel->subleg.liVTopPointA):NULL;
			xarrKeyNodes[3].pMainModelNode=pFzcPanel->subleg.liVTopPointB>0?Ta.FromPointI(pFzcPanel->subleg.liVTopPointB):NULL;
		}
		CTower panelmodel;
		CTtaDataFile bracetta(&panelmodel);
		if (bracetta.ReadSubBraceElemsDataFile(pFzcPanel->fname,xarrKeyNodes,2,CTtaDataFile::FILETYPE_GTOM))
		{
			acs.origin=xarrKeyNodes[0].pMainModelNode->xOriginalPos;
			lcs.origin=xarrKeyNodes[0].pModelPanelNode->xOriginalPos;
			//acs.axis_x
		}
		else
			logerr.Log("error");
	}
	//
	CLDSNode *pBraceNode,*pMergeToNode=NULL;
	for(pBraceNode=Ta.Node.GetFirst();pBraceNode;pBraceNode=Ta.Node.GetNext())
	{	//���Ľڵ�һ�㲻Ӧ�����κ������㣬���������������������ƫ�Ӧ���Ȱ������ʶ��ʧ���򰴱�����ʶ��
		if(pBraceNode->handle<hMinCanUseHandle)
			continue;	//�ɽڵ�
		if(pBraceNode->pointI<uiMaxPointI)
		{	//��������Ӹ�����ʱ���ļ��Žڵ���п������ظ���Ҫ���·����� wjh-2018.6.1
			uiMaxPointI+=10;
			pBraceNode->point_i=uiMaxPointI+pBraceNode->point_i%10;
		}
		CAtomListStack<CLDSNode> stack(&Ta.Node);
		//BOOL pushed=Ta.Node.push_stack();
		//1.0)�ϲ��ظ��ڵ�
		pMergeToNode=NULL;
		for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			if(pBraceNode==pNode)
				break;
			if(!pNode->ModuleTogetherWith(pBraceNode))
				continue;
			GEPOINT distvec=pNode->xOriginalPos-pBraceNode->xOriginalPos;
			if(fabs(distvec.x)<1&&fabs(distvec.y)<1&&fabs(distvec.z)<1)
			{
				pMergeToNode=pNode;
				break;
			}
		}
		if(pMergeToNode)
		{
			if(pMergeToNode->pointI==0&&pBraceNode->pointI>0)
				pMergeToNode->point_i=pBraceNode->pointI;
			if(!MergeOverlapNodes(pMergeToNode,pBraceNode))
				logerr.Log("�ڵ�0x%X{%d��}��0x%X{%d��}λ�ýӽ������ϲ�ʧ��",pMergeToNode->handle,pMergeToNode->pointI,pBraceNode->handle,pBraceNode->pointI);
			else
			{
				console.DeleteNode(pBraceNode->handle);
				g_pSolidDraw->DelEnt(pBraceNode->handle);
				pBraceNode=pMergeToNode;
			}
		}
		//1.1)�鲢�����������ڵ㣬����һ�㶼�������ڵ�
		if(pBraceNode->m_cPosCalType==CLDSNode::COORD_INDEPENDENT)
		{
			bool recognized=false;
			pBraceNode->m_cPosCalType=CLDSNode::COORD_INTERSECTION;
			if(!(recognized=pBraceNode->SearchRelaDependNodeRods()))
			{
				pBraceNode->m_cPosCalType=CLDSNode::COORD_SCALE;
				recognized=pBraceNode->SearchRelaDependNodeRods();
			}
			if(!recognized)	//��������ʧ�ܣ���ԭ�������κ�������
				pBraceNode->m_cPosCalType=CLDSNode::COORD_INDEPENDENT;
		}
		//Ta.Node.pop_stack(pushed);
	}
	//2.0)�ϲ����ศ�ĸ˼�
	for(CLDSLinePart* pBraceRod=Ta.EnumRodFirst();pBraceRod;pBraceRod=Ta.EnumRodNext())
	{
		if(pBraceRod->handle<hMinCanUseHandle)
			continue;	//������ģ�������������ĸ˼�
		CPtInLineCheck lineCheck(pBraceRod->pStart->xOriginalPos,pBraceRod->pEnd->xOriginalPos);
		//CLDSLinePart* pColinearRod=NULL;
		CLdsPartListStack stack(&Ta);
		for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		{
			if(pRod->handle>=hMinCanUseHandle)
				break;	//��������׷�Ӹ����ģ�˵�����������������
			if(!pRod->ModuleTogetherWith(pBraceRod->pStart)||!pRod->ModuleTogetherWith(pBraceRod->pEnd))
				continue;
			bool startOnRod=false,endOnRod=false;
			double scale_start=lineCheck.CheckPointEx(pRod->pStart->xOriginalPos,&startOnRod);
			double scale_end=lineCheck.CheckPointEx(pRod->pEnd->xOriginalPos,&endOnRod);
			if(!startOnRod||!endOnRod)
				continue;	//���˼�������
			if((scale_start<EPS&&scale_end<EPS)||(scale_start>1+EPS&&scale_end>1+EPS))
				continue;	//���˼����ߵ����ص�
			//pColinearRod=pRod;
			logerr.Log("�˼�{%d-%d}��{%d-%d}λ���ص����ѽ��кϲ�����",pBraceRod->pStart->pointI,pBraceRod->pEnd->pointI,
				pRod->pStart->pointI,pRod->pEnd->pointI);
			console.DeletePart(pBraceRod->handle);
			break;
		}
	}
	Ta.AddMirRelativeObjInfo();
	DisplayProcess(100,"��ȡ���ļ����ļ�...");
	return 0;
}
void CLDSView::OnAddGtmBraceElemPanels()
{
	CLogErrorLife life(&logerr);
	AddGtmBraceElemPanels();
}
#else
void CLDSView::OnAddGtmBraceElemPanels()
{
}
#endif
