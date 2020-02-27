//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"

#include "MainFrm.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "Dlg.h"
#include "ScrTools.h"
#include "PartNoRuleDlg.h"
#include "CfgPartNoDlg.h"
#include "SortFunc.h"
#include "LocatePartByNoDlg.h"
#include "CheckPartNoDlg.h"
#include "PartNoListDlg.h"
#include "BriefWeldPart.h"
#include "OutputInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//��������
int CompareFL(const CLDSPartPtr& pFl1,const CLDSPartPtr& pFl2)
{
	if(pFl1->ucs.origin.z>pFl2->ucs.origin.z)
		return 1;
	else if(pFl1->ucs.origin.z<pFl2->ucs.origin.z)
		return -1;
	else if(pFl1->ucs.origin.y>pFl2->ucs.origin.y)
		return -1;
	else if(pFl1->ucs.origin.y<pFl2->ucs.origin.y)
		return 1;
	else if(pFl1->ucs.origin.x>pFl2->ucs.origin.x)
		return 1;
	else if(pFl1->ucs.origin.x<pFl2->ucs.origin.x)
		return -1;
	else
	{
		if(pFl1->Layer(2)=='T'&&pFl2->Layer(2)!='T')
			return 1;
		else if(pFl1->Layer(2)!='T'&&pFl2->Layer(2)=='T')
			return -1;
	}
	return 0;
}

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
static SCOPE_STRU GetTaAtomScope(const CLDSPartPtr pObj)
{
	SCOPE_STRU scope,scope2;
	CLDSNode *pNode;
	CLDSLineTube *pTube;
	CLDSLineAngle *pCommJg;
	CLDSPlate *pPlank;
	CLDSParamPlate *pParamPlate;
	CJoint *pJoint;
	CFoot *pFoot;
	memset(&scope,0,sizeof(SCOPE_STRU));
	if(pObj==NULL)
		return scope;
	f3dPoint start,end;
	switch(pObj->GetClassTypeId())
	{
	/*case CLS_NODE:
		pNode = (CLDSNode*)pObj;
		scope.fMinX = scope.fMaxX = pNode->pos.x;
		scope.fMinY = scope.fMaxY = pNode->pos.y;
		scope.fMinZ = scope.fMaxZ = pNode->pos.z;
		break;*/
	case CLS_FOOT:
		pFoot = (CFoot*)pObj;
		pNode = console.FromNodeHandle(pFoot->base_node_handle);
		scope.fMinX = scope.fMaxX = pNode->Position(false).x;
		scope.fMinY = scope.fMaxY = pNode->Position(false).y;
		scope.fMinZ = scope.fMaxZ = pNode->Position(false).z;
		break;
	case CLS_LINEANGLE:
		pCommJg = (CLDSLineAngle*)pObj;
		if(pCommJg->pStart)
			start = pCommJg->pStart->Position(false);
		else
			start = pCommJg->Start();
		if(pCommJg->pEnd)
			end = pCommJg->pEnd->Position(false);
		else
			end = pCommJg->End();
		scope.fMinX = __min(start.x,end.x);
		scope.fMaxX = __max(start.x,end.x);
		scope.fMinY = __min(start.y,end.y);
		scope.fMaxY = __max(start.y,end.y);
		scope.fMinZ = __min(start.z,end.z);
		scope.fMaxZ = __max(start.z,end.z);
		break;
	case CLS_LINETUBE:
		pTube = (CLDSLineTube*)pObj;
		if(pTube->pStart)
			start = pTube->pStart->Position(false);
		else
			start = pTube->Start();
		if(pTube->pEnd)
			end = pTube->pEnd->Position(false);
		else
			end = pTube->End();
		scope.fMinX = __min(start.x,end.x);
		scope.fMaxX = __max(start.x,end.x);
		scope.fMinY = __min(start.y,end.y);
		scope.fMaxY = __max(start.y,end.y);
		scope.fMinZ = __min(start.z,end.z);
		scope.fMaxZ = __max(start.z,end.z);
		break;
	case CLS_PLATE:
		pPlank = (CLDSPlate*)pObj;
		scope.fMinX = scope.fMaxX = pPlank->ucs.origin.x;
		scope.fMinY = scope.fMaxY = pPlank->ucs.origin.y;
		scope.fMinZ = scope.fMaxZ = pPlank->ucs.origin.z;
		break;
	case CLS_PARAMPLATE:
		pParamPlate = (CLDSParamPlate*)pObj;
		scope.fMinX = scope.fMaxX = pParamPlate->ucs.origin.x;
		scope.fMinY = scope.fMaxY = pParamPlate->ucs.origin.y;
		scope.fMinZ = scope.fMaxZ = pParamPlate->ucs.origin.z;
		break;
	case CLS_JOINT:
		pJoint = (CJoint*)pObj;
		pNode = console.FromNodeHandle(pJoint->base_node_handle);
		scope.fMinX = scope.fMaxX = pNode->Position(false).x;
		scope.fMinY = scope.fMaxY = pNode->Position(false).y;
		scope.fMinZ = scope.fMaxZ = pNode->Position(false).z;
	default:
		if(pObj->IsLinePart())
		{
			CLDSLinePart* pRod=(CLDSLinePart*)pObj;
			if(pRod->pStart)
				start = pRod->pStart->Position(false);
			else
				start = pRod->Start();
			if(pRod->pEnd)
				end = pRod->pEnd->Position(false);
			else
				end = pRod->End();
			scope.fMinX = __min(start.x,end.x);
			scope.fMaxX = __max(start.x,end.x);
			scope.fMinY = __min(start.y,end.y);
			scope.fMaxY = __max(start.y,end.y);
			scope.fMinZ = __min(start.z,end.z);
			scope.fMaxZ = __max(start.z,end.z);
		}
		else
		{
			scope.fMinX = scope.fMaxX = pObj->ucs.origin.x;
			scope.fMinY = scope.fMaxY = pObj->ucs.origin.y;
			scope.fMinZ = scope.fMaxZ = pObj->ucs.origin.z;
		}
		break;
	}
	return scope;
}
//�������Ͷ���������������ԭ�����£�
//�����浽���棬������
//�����ͼ���ǰ����0 ������1������2 ����3��������4
static int GetFaceTypeGrade(const CLDSPartPtr pPart)
{
	char ch;	//��ʶ��ĸ
	//�����ͼ���ǰ����0 ������1������2 ����3��������4
	int grade=4;
	if(pPart->IsLinePart())
	{
		ch = pPart->layer(2);
		if(ch=='1'||ch=='2'||ch=='Q')//ǰ��
			grade = 0;
		else if(ch=='3'||ch=='4'||ch=='H')//����
			grade = 1;
		else if(ch=='Z')//�����
			grade = 2;
		else if(ch=='Y')//�Ҳ���
			grade = 3;
		else			//������
			grade = 4;
	}
	else if(pPart->IsPlate())
	{
		ch = pPart->layer(2);
		if(ch=='1')//ǰ��
		{
			if(fabs(pPart->ucs.axis_z.y)>fabs(pPart->ucs.axis_z.x))
				grade = 0;	//ǰ��
			else
				grade = 2;	//�����
		}
		else if(ch=='2')
		{
			if(fabs(pPart->ucs.axis_z.y)>fabs(pPart->ucs.axis_z.x))
				grade = 0;	//ǰ��
			else
				grade = 3;	//�Ҳ���
		}
		else if(ch=='3')
		{
			if(fabs(pPart->ucs.axis_z.y)>fabs(pPart->ucs.axis_z.x))
				grade = 1;	//����
			else
				grade = 3;	//�Ҳ���
		}
		else if(ch=='4')
		{
			if(fabs(pPart->ucs.axis_z.y)>fabs(pPart->ucs.axis_z.x))
				grade = 1;	//����
			else
				grade = 2;	//�����
		}
		else if(ch=='Q')//ǰ��
			grade = 0;
		else if(ch=='H')//����
			grade = 1;
		else if(ch=='Z')//�����
			grade = 2;
		else if(ch=='Y')//�Ҳ���
			grade = 3;
		else			//������
			grade = 4;
	}
	return grade;
}
//��Ҫ�湹�������,������ֵ��(������Ϊ0)
//-1��ʾ��һ��������� 0��ʾ��ͬ 1��ʾ�ڶ����������
static int CompareFaceType(const CLDSPartPtr& pObj1,const CLDSPartPtr& pObj2)
{
	//�����ͼ���ǰ����0 ������1������2 ����3��������4
	int grade1,grade2;
	grade1 = GetFaceTypeGrade(pObj1);
	grade2 = GetFaceTypeGrade(pObj2);
	if(grade1<grade2)//�������ν���(����ֵ��������)
		return -1;
	else if(grade1>grade2)
		return 1;
	else
		return 0;
}
//������ǰ�����򷵻�TRUE���򷵻�FALSE
static BOOL SortFaceType(CLDSPartPtr& pObj1,CLDSPartPtr& pObj2)
{
	//�����ͼ���ǰ����0 ������1������2 ����3��������4
	int grade1,grade2;
	grade1 = GetFaceTypeGrade(pObj1);
	grade2 = GetFaceTypeGrade(pObj2);
	if(grade1>grade2)//�������ν���(����ֵ��������)
	{
		swap_ta_atom(pObj1,pObj2);
		return FALSE;
	}
	return TRUE;
}
//��������������,ԭ������:
//���ĽǸ�/�ֹ�0;���Ű�2,��������1
static int GetTaAtomTypeGrade(const CLDSPartPtr pObj)
{
	if(pObj->IsLinePart()&&(((CLDSLinePart*)pObj)->pStart==NULL||((CLDSLinePart*)pObj)->pEnd==NULL))
		return 4;	//ȱ�ٶ˽ڵ�ĸ˼���̽Ǹ�������ͨ�ְ弶��
	if(pObj->GetClassTypeId()==CLS_LINEANGLE)
	{
		CLDSLineAngle *pCommJg = (CLDSLineAngle*)pObj;
		char layertag;
		layertag = pCommJg->Layer(1);
		if(layertag=='Z')
			return 0;
		else if(layertag=='X')
			return 1;
		else if(layertag=='H')
			return 2;
		else
			return 3;
	}
	else if(pObj->GetClassTypeId()==CLS_LINETUBE)
	{
		CLDSLineTube *pTube = (CLDSLineTube*)pObj;
		char layertag;
		layertag = pTube->layer(1);
		if(layertag=='Z')
			return 0;
		else if(layertag=='X')
			return 1;
		else if(layertag=='H')
			return 2;
		else
			return 3;
	}
	else if(pObj->IsLinePart())
		return 3;
	else if(pObj->IsPlate())
	{
		if(pObj->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pObj)->jdb_style==4)
			return 7;	//��˨���
		else if(pObj->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pObj;
			if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
				return 5;	//������
			else if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE||pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
				return 6;	//�����߰�
			else if(pParamPlate->m_iParamType==TYPE_BASEPLATE)
				return 10;	//���ŵװ�
			else if(pParamPlate->m_iParamType==TYPE_SHOEPLATE)
				return 11;	//����ѥ��
			else if(pParamPlate->m_hPartWeldParent==TYPE_SHOEPLATE)
				return 8;	//����ѥ��
		}
		return 4;	//��ͨ�ְ�
	}
	else
		return 10;	//���฽������
}
//���ĽǸ�/�ֹ�0;���Ű�2,��������1
static double GetPlateSubTypeGrade(const CLDSPartPtr pObj)
{
	if(!pObj->IsPlate())
		return 0;	//ȱ�ٶ˽ڵ�ĸ˼���̽Ǹ�������ͨ�ְ弶��
	CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)pObj;
	if(pPlate->GetClassTypeId()==CLS_PLATE)
	{
		if(pObj->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pObj)->jdb_style==4)
			return 7;	//��˨���
		else
			return 1.0/pPlate->GetThick();
	}
	else if(pObj->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pObj;
		if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			return 2;	//������
		else if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE||pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
			return 3;	//�����߰�
		else if(pParamPlate->m_iParamType==TYPE_BASEPLATE)
			return 4;	//���ŵװ�
		else if(pParamPlate->m_iParamType==TYPE_SHOEPLATE)
			return 5;	//����ѥ��
		else
			return 6;	//�������Ͱ�,�������߰�
	}
	return 0;
}
//���ȼ��ߵĹ��������,������ֵ��(������Ϊ0)
//-1��ʾ��һ��������� 0��ʾ��ͬ 1��ʾ�ڶ����������
static int CompareTaAtomType(const CLDSPartPtr& pObj1,const CLDSPartPtr& pObj2)
{
	int grade1,grade2;
	grade1 = GetTaAtomTypeGrade(pObj1);
	grade2 = GetTaAtomTypeGrade(pObj2);
	if(grade1<grade2)
		return -1;
	else if(grade1>grade2)
		return 1;
	else
		return 0;
}
//��һ����ZֵС�򷵻���ֵ,������ȷ���0,���򷵻ظ�ֵ
static int CompareTaAtomUpDown(const CLDSPartPtr& pObj1,const CLDSPartPtr& pObj2)
{
	SCOPE_STRU scope1,scope2;
	scope1 = GetTaAtomScope(pObj1);
	scope2 = GetTaAtomScope(pObj2);
	double maxz1=ftoi(scope1.fMaxZ)/10;
	double maxz2=ftoi(scope2.fMaxZ)/10;
	
	//ʵ������£�����������ͬ����Ӧ��ȡ�е㣬��������Ѽ���ͺᵣ�ϵ����渨���޷���֤�ɽ���Զ����wjh-2017.8.3
	//if(ftoi(maxz2-maxz1)==0 && pObj1->IsLinePart())
	//{	//�˼�ʱ,�����͵�һ������м��λ��
	//	double midz1=(scope1.fMaxZ+scope1.fMinZ)*0.5;
	//	double midz2=(scope2.fMaxZ+scope2.fMinZ)*0.5;
	//	return ftoi(midz2-midz1);//ZԽСӦԽ����
	//}
	//else
		return ftoi(maxz2-maxz1);	//ZԽСӦԽ����
}
//��һ����XֵС�򷵻ظ�ֵ,������ȷ���0,���򷵻���ֵ
static int CompareTaAtomL_R(const CLDSPartPtr& pObj1,const CLDSPartPtr& pObj2)
{
	SCOPE_STRU scope1,scope2;
	scope1 = GetTaAtomScope(pObj1);
	scope2 = GetTaAtomScope(pObj2);
	double minx1=scope1.fMinX;
	double minx2=scope2.fMinX;
	if(ftoi(minx2-minx1)==0 && pObj1->IsLinePart())
	{	//�˼�ʱ,��������һ������м��λ��
		double midx1=(scope1.fMinX+scope1.fMaxX)*0.5;
		double midx2=(scope2.fMinX+scope2.fMaxX)*0.5;
		return ftoi(midx1-midx2);//XԽСӦԽ��ǰ
	}
	else
		return ftoi(minx1-minx2);//XԽСӦԽ��ǰ
}
static int CompareRodPrior(const CLDSPartPtr&pObj1,const CLDSPartPtr&pObj2)
{
	if(pObj1->IsLinePart()&&!pObj2->IsLinePart())
		return -1;
	else if(!pObj1->IsLinePart()&&pObj2->IsLinePart())
		return 1;
	else
		return 0;
}
static BOOL ComplexCompareFunc(const CLDSPartPtr&pObj1,const CLDSPartPtr&pObj2)
{
	int k,ret=0;
	//1.�ȶԶκţ���С����
	if(pObj1->iSeg>pObj2->iSeg)
		return 1;
	else if(pObj1->iSeg<pObj2->iSeg)
		return -1;
	long harr[4]={0x370,0x372,0,0};//,0x337,0x338};
	bool a=false,b=false;
	if(pObj1->handle==harr[0]||pObj1->handle==harr[1]||pObj1->handle==harr[2]||pObj1->handle==harr[3])
		a=true;
	if(pObj2->handle==harr[0]||pObj2->handle==harr[1]||pObj2->handle==harr[2]||pObj2->handle==harr[3])
		b=true;
	RULE_STRU rule_arr[4];
	for(k=0;k<4;k++)
		rule_arr[g_sysPara.arrRule[k].iOrder]=g_sysPara.arrRule[k];
	//�ᵣ�����ɽ���Զ���������ȣ����������µ��ϡ�wjh-2017.8.3
	//�������ı�ŷ��ڵ�һλ�������ġ�б�ġ���ġ���������ֿ�)
	//�������(ǰ��)��(����)��
	//�ٴ������������ϣ��ᵣ���ɽ���Զ������б��ȡ��ֵ���Ḩ��ȡ��ֵ��
	//������б��ĸ���
	//2.ͬһ������Զ������������
	bool bIsObj1Post=false,bIsObj2Post=false;
	if(pObj1->Layer(1)=='Z'&&pObj1->IsLinePart())
		bIsObj1Post=((CLDSLinePart*)pObj1)->pStart!=NULL&&((CLDSLinePart*)pObj1)->pEnd!=NULL;
	if(pObj2->Layer(1)=='Z'&&pObj2->IsLinePart())
		bIsObj2Post=((CLDSLinePart*)pObj2)->pStart!=NULL&&((CLDSLinePart*)pObj2)->pEnd!=NULL;
	if(bIsObj1Post&&!bIsObj2Post)
		return -1;	//ͬһ������Զ������������
	else if(!bIsObj1Post&&bIsObj2Post)
		return 1;	//ͬһ������Զ������������
	if(pObj1->Layer(0)=='T'&&pObj2->Layer(0)=='T')
	{
		//3.������,����������,������
		if((ret=CompareFaceType(pObj1,pObj2))!=0)
			return ret;
		//4.ͬһ����ƽ���ڸ˼�����
		if((ret=CompareRodPrior(pObj1,pObj2))!=0)
			return ret;
		//5.������������ͷʱ����Ѽ���ͺᵣΪ��һ���Ծ������ľ����ɽ���Զ�������������������ϣ� wjh-2017.8.3
		CLDSLinePart* pRod1=pObj1->IsLinePart()?(CLDSLinePart*)pObj1:NULL;
		CLDSLinePart* pRod2=pObj2->IsLinePart()?(CLDSLinePart*)pObj2:NULL;
		double dist1,dist2;
		if(pRod1&&pRod2)
		{
			GEPOINT start1=pRod1->pStart!=NULL?pRod1->pStart->Position(false):pRod1->Start();
			GEPOINT end1=pRod1->pEnd!=NULL?pRod1->pEnd->Position(false):pRod1->End();
			GEPOINT start2=pRod2->pStart!=NULL?pRod2->pStart->Position(false):pRod2->Start();
			GEPOINT end2=pRod2->pEnd!=NULL?pRod2->pEnd->Position(false):pRod2->End();
			if(pRod1->Layer(1)=='Z'&&pRod2->Layer(1)=='Z')
			{
				if(max(start1.z,end1.z)<max(start2.z,end2.z))
					return 1;
				else if(max(start1.z,end1.z)>max(start2.z,end2.z))
					return -1;
			}
			if(pRod1->Layer(1)!='F')	//��б��ȡ��ֵ������ȡ��ֵ
				dist1=min(fabs(start1.x)+fabs(start1.y),fabs(end1.x)+fabs(end1.y))/10;
			else
				dist1=ftoi(fabs(pObj1->ucs.origin.x)+fabs(pObj1->ucs.origin.y))/10;
			if(pRod2->Layer(1)!='F')	//��б��ȡ��ֵ������ȡ��ֵ
				dist2=min(fabs(start2.x)+fabs(start2.y),fabs(end2.x)+fabs(end2.y))/10;
			else
				dist2=ftoi(fabs(start2.x+end2.x)+fabs(start2.y+end2.y))/10;
			if(dist1==dist2)
			{	//��ֵ��ͬʱȡ��ֵ
				if(pRod1->Layer(1)!='F')	//��б��ȡ��ֵ������ȡ��ֵ
					dist1=ftoi(fabs(start1.x+end1.x)+fabs(start1.y+end1.y))/10;
				if(pRod2->Layer(1)!='F')	//��б��ȡ��ֵ������ȡ��ֵ
					dist2=ftoi(fabs(start2.x+end2.x)+fabs(start2.y+end2.y))/10;
			}
		}
		else
		{
			dist1=ftoi(fabs(pObj1->ucs.origin.x)+fabs(pObj1->ucs.origin.y))/10;
			dist2=ftoi(fabs(pObj2->ucs.origin.x)+fabs(pObj2->ucs.origin.y))/10;
		}
		if(dist1>dist2)
			return 1;
		else if(dist1<dist2)
			return -1;
		//6.��������
		if((ret=CompareTaAtomUpDown(pObj1,pObj2))!=0)
			return ret;	//��������
		//7.���ְ����������
		if(pObj1->IsPlate()&&pObj2->IsPlate())
		{	//���иְ��������𣬲�����
			double fType1=GetPlateSubTypeGrade(pObj1);
			double fType2=GetPlateSubTypeGrade(pObj2);
			if(fType1>fType2)
				return 1;
			else if(fType1<fType2)
				return -1;
		}
		//8.���ݹ�����������δ��Ӧ��7�������ϣ�
		if((ret=CompareTaAtomType(pObj1,pObj2))!=0)
			return ret;	//��������,�˼���ͨ�ְ�,������,�����߰�,��˨���,���฽������
		return 0;
	}
	else
	{
		for(k=0;k<4;k++)
		{
			if(!rule_arr[k].bActive)
				continue;
			if(rule_arr[k].name==MAIN_OTHER)
			{
				if((ret=CompareTaAtomType(pObj1,pObj2))!=0)
					return ret;	//��������,�˼���ͨ�ְ�,������,�����߰�,��˨���,���฽������
			}
			else if(rule_arr[k].name==DOWN_UP)
			{
				if((ret=CompareTaAtomUpDown(pObj1,pObj2))!=0)
					return ret;	//��������
				if(pObj1->IsPlate()&&pObj2->IsPlate())
				{	//���иְ���������
					double fType1=GetPlateSubTypeGrade(pObj1);
					double fType2=GetPlateSubTypeGrade(pObj2);
					if(fType1>fType2)
						return 1;
					else if(fType1<fType2)
						return -1;
				}
			}
			else if(rule_arr[k].name==LEFT_RIGHT)
			{
				if(pObj1->Layer(0)=='L'&&pObj2->Layer(0)=='L')
				{	//���������ڽ���ʱһ���Ե�һ����Ϊ��׼��һ�㲻��ѭ�������ҵĹ��� wjh-2017.4.13
					CLDSLinePart* pRod1=pObj1->IsLinePart()?(CLDSLinePart*)pObj1:NULL;
					CLDSLinePart* pRod2=pObj2->IsLinePart()?(CLDSLinePart*)pObj2:NULL;
					char cQuad1=0,cQuad2=0;
					if(pRod1&&pRod2)
					{
						cQuad1=CalPoleQuad(pRod1->Start(),pRod1->End());
						cQuad2=CalPoleQuad(pRod2->Start(),pRod2->End());
					}
					else
					{
						cQuad1=CalPointQuadPos(pObj1->ucs.origin)+'0';
						cQuad2=CalPointQuadPos(pObj2->ucs.origin)+'0';
					}
					if(cQuad1!=cQuad2)
						return cQuad1-cQuad2;
				}
				else if(pObj1->Layer(0)=='T'&&pObj2->Layer(0)=='T')
				{	//������������ͷʱ����Ѽ���ͺᵣΪ��һ���Ծ������ľ����ɽ���Զ���� wjh-2017.8.3
					CLDSLinePart* pRod1=pObj1->IsLinePart()?(CLDSLinePart*)pObj1:NULL;
					CLDSLinePart* pRod2=pObj2->IsLinePart()?(CLDSLinePart*)pObj2:NULL;
					double dist1,dist2;
					if(pRod1&&pRod2)
					{
						GEPOINT start1=pRod1->pStart!=NULL?pRod1->pStart->Position(false):pRod1->Start();
						GEPOINT end1=pRod1->pEnd!=NULL?pRod1->pEnd->Position(false):pRod1->End();
						GEPOINT start2=pRod2->pStart!=NULL?pRod2->pStart->Position(false):pRod2->Start();
						GEPOINT end2=pRod2->pEnd!=NULL?pRod2->pEnd->Position(false):pRod2->End();
						dist1=min(fabs(start1.x)+fabs(start1.y),fabs(end1.x)+fabs(end1.y));
						dist2=min(fabs(start2.x)+fabs(start2.y),fabs(end2.x)+fabs(end2.y));
						dist1=ftoi(dist1)/10;
						dist2=ftoi(dist2)/10;
						if(dist1==dist2)
						{
							dist1=ftoi(fabs(start1.x+end1.x)+fabs(start1.y+end1.y))/10;
							dist2=ftoi(fabs(start2.x+end2.x)+fabs(start2.y+end2.y))/10;
						}
					}
					else
					{
						dist1=ftoi(fabs(pObj1->ucs.origin.x)+fabs(pObj1->ucs.origin.y))/10;
						dist2=ftoi(fabs(pObj2->ucs.origin.x)+fabs(pObj2->ucs.origin.y))/10;
					}
					if(dist1>dist2)
						return 1;
					else if(dist1<dist2)
						return -1;
				}
				else if((ret=CompareTaAtomL_R(pObj1,pObj2))!=0)
					return ret;	//��������
			}
			else if(rule_arr[k].name==FRONT_SIDE)
			{
				if((ret=CompareFaceType(pObj1,pObj2))!=0)
					return ret;	//������,����������,������
			}
		}
		//�˼����Ȼ�������ͬһ���������ȸ˼����Ǹ֣���ְ壨������������飩��wjh-2017.8.3
		if(g_sysPara.bFristJgCodeNo&&(ret=CompareTaAtomType(pObj1,pObj2))!=0)
			return ret;	//��������,�˼���ͨ�ְ�,������,�����߰�,��˨���,���฽������
	}
	return 0;
}
static BOOL CheckPartSegI()
{
	CLDSNode *pNode;
	CLDSPart *pPart;
	CString sError;
	FILE *fp=fopen("PartSegI.txt","wt");
	int nHits=0;
	g_pSolidDraw->ReleaseSnapStatus();
	for(pNode=Ta.Node.GetFirst();pNode!=NULL;pNode=Ta.Node.GetNext())
	{
		if(pNode->iSeg.iSeg<1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sError.Format("The segment number of node 0x%X is smaller than 1��You need to specify the segment number",pNode->handle);
#else 
			sError.Format("�ڵ�0x%X�ķֶκ�С��1����Ҫָ���ֶκ�",pNode->handle);
#endif
			nHits++;
			fprintf(fp,"%s\n",sError);
			g_pSolidDraw->SetEntSnapStatus(pNode->handle);
		}
	}
	for(pPart=Ta.Parts.GetFirst();pPart!=NULL;pPart=Ta.Parts.GetNext())
	{
		if( pPart->m_bVirtualPart||pPart->GetClassTypeId()==CLS_POLYJOINT||
			pPart->GetClassTypeId()==CLS_ANCHORBOLT||pPart->GetClassTypeId()==CLS_FITTINGS)
			continue;
		if(pPart->iSeg.iSeg<1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sError.Format("The segment number of part 0x%X is smaller than 1��You need to specify the segment number",pPart->handle);
#else 
			sError.Format("����0X%X�ķֶκ�С��1����Ҫָ���ֶκ�",pPart->handle);
#endif
			nHits++;
			fprintf(fp,"%s\n",sError);
			g_pSolidDraw->SetEntSnapStatus(pPart->handle);
		}
	}
	fclose(fp);
	if(nHits>0)
	{
		WinExec("notepad.exe PartSegI.txt",SW_SHOW);
		return FALSE;
	}
	else
		return TRUE;
}
CStringArray part_no_arr;
static BOOL FindPartNo(const char *sPartNo=NULL)
{
	CLDSPart *pPart;
	if(sPartNo)
	{
		if(strlen(sPartNo)==0)
			return TRUE;
		for(int i=0;i<part_no_arr.GetSize();i++)
		{
			if(part_no_arr[i].CompareNoCase(sPartNo)==0)
				return TRUE;	//�˱���ѱ�ʹ��
		}
		return FALSE;
	}
	else
		part_no_arr.RemoveAll();
	int i;
	for(pPart=Ta.Parts.GetFirst();pPart!=NULL;pPart=Ta.Parts.GetNext())
	{
		if(strlen(pPart->GetPartNo())==0)
			continue;
		for(i=0;i<part_no_arr.GetSize();i++)
			if(part_no_arr[i].CompareNoCase(pPart->GetPartNo())==0)	//�д˺�
				break;
		if(i==part_no_arr.GetSize())
			part_no_arr.Add(pPart->GetPartNo());
	}
	for(CSupplementPart *pSuppPart=Ta.SuppPart.GetFirst();pSuppPart;pSuppPart=Ta.SuppPart.GetNext())
	{
		if(strlen(pSuppPart->GetPartNo())==0)
			continue;
		for(i=0;i<part_no_arr.GetSize();i++)
			if(part_no_arr[i].CompareNoCase(pSuppPart->GetPartNo())==0)	//�д˺�
				break;
		if(i==part_no_arr.GetSize())
			part_no_arr.Add(pSuppPart->GetPartNo());
	}
	/*for(pHuoQuJg=(CLDSPolyAngle*)Ta.Parts.GetFirst(CLS_POLYANGLE);pHuoQuJg!=NULL;pHuoQuJg=(CLDSPolyAngle*)Ta.Parts.GetNext(CLS_POLYANGLE))
	{
		if(strlen(pHuoQuJg->GetPartNo())==0)
			continue;
		for(int i=0;i<part_no_arr.GetSize();i++)
			if(part_no_arr[i].CompareNoCase(pHuoQuJg->GetPartNo())==0)	//�д˺�
				break;
		if(i==part_no_arr.GetSize())
			part_no_arr.Add(pHuoQuJg->GetPartNo());
	}*/
	return TRUE;
}
#endif
void CLDSView::OnCodePartNoRule() 
{
	CPartNoRuleDlg dlg;
	for(int i=0;i<4;i++)
		dlg.SetPartOrderRule(i,g_sysPara.arrRule[i].iOrder,g_sysPara.arrRule[i].bActive);
	dlg.m_bJgFirstCodeNo=g_sysPara.bFristJgCodeNo;
	if(dlg.DoModal()==IDOK)
	{
		for(int i=0;i<4;i++)
			dlg.GetPartOrderRule(i,&g_sysPara.arrRule[i].iOrder,&g_sysPara.arrRule[i].bActive);
		g_sysPara.bFristJgCodeNo= dlg.m_bJgFirstCodeNo;
	}
}

void CLDSView::OnClearPartNoI() 
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
#ifdef AFX_TARG_ENU_ENGLISH
	if(MessageBox("Really want to clear all the part label?","LDS",MB_YESNO)==IDYES)
#else 
	if(MessageBox("���Ҫ������й��������?","LDS",MB_YESNO)==IDYES)
#endif
	{
		CLDSPart *pPart;
		//�������й�����ǰ�ı��
		CTmaPtrList<CLDSParamPlate,CLDSParamPlate*> xendset;
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&((((CLDSParamPlate*)pPart)->m_bStdPart)||
				(true/*codesetdlg.m_bReservFlNo*/&&((CLDSParamPlate*)pPart)->m_iParamType==TYPE_FL)))
			{	//��׼�������������������µ���ͨ����������չ������	wjh-2012.4.21
				if(((CLDSParamPlate*)pPart)->m_iParamType==TYPE_XEND)
					xendset.append((CLDSParamPlate*)pPart);
				pPart->feature=0;	//������ձ�Ź���
			}
			else
				pPart->feature=1;	//����ձ�Ź���
		}
		for(CLDSParamPlate* pParamPlate=xendset.GetFirst();pParamPlate;pParamPlate=xendset.GetNext())
		{	//ʮ�ֲ���׼���ĸ���һ��ҲӦ���������Ӽ���
			if(pParamPlate->m_iParamType!=TYPE_XEND)
				continue;
			for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
			{
				pPart=console.FromPartHandle(pItem->h,pItem->idClsType);
				pPart->feature=0;
			}
		}
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(pPart->feature==0)	//������ձ�ŵĹ���
				continue;
			pPart->SetPartNo("");
		}
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("All part number has been cleared!","LDS");
#else 
		MessageBox("���й�����������!","LDS");
#endif
	}
#endif
	
}
static int _LocalCompareWeldSonPartItem(const WELDSONPART& item1,const WELDSONPART& item2)
{
	return item1.iFuncType-item2.iFuncType;
}
void CLDSView::OnCodePartNo() 
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	int i,j,serial_no,n;
	/*if(theApp.m_bAnalysisWorkEnv)
	{
		theApp.m_bAnalysisWorkEnv=FALSE;
		LdsWorkEnvShift(FALSE);	//������ű���ת������ͼģʽ����
	}*/
	BeginWaitCursor();
	long MaxCodeNo=0;
/*		�����༭����
	1.�ṹͼ�г���˨���Ŷ�����Ȧ�⣬���й�����Ӧ�б��
	2.�������˳��:�������ϡ������ҡ������ĺ���������,
	  �����浽����,������
	3.�������������,���˳��ֿպŻ��ź��A��B�����
	4.�������ʱ����λ���ڱ�ʾ����е���ˮ�ţ�����λ���ڱ�ʾ�κ�
*/
	n = Ta.Parts.GetNodeNum();
	//n+= Ta.HuoQuJg.GetNodeNum();
	if(n<=0)		//������������б��
		return;
	static CCheckPartNoDlg codesetdlg;
#ifdef AFX_TARG_ENU_ENGLISH
	codesetdlg.m_sCaption="Part Number";
#else 
	codesetdlg.m_sCaption="�������";
#endif
	codesetdlg.m_bDisplayFlNoCtrl=true;
	if(codesetdlg.DoModal()!=IDOK)
		return;
	//�ֽ�κ��ַ���
	CHashList<SEGI>hashSegTbl;
	GetSegNoHashTblBySegStr(codesetdlg.m_sSegStr,hashSegTbl);
	if(codesetdlg.m_sSegStr.Find('*')>=0)
	{
		if(!CheckPartSegI())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("No through part segment number check, cannot number part");
#else 
			AfxMessageBox("û��ͨ�������κż��,���ܽ��й������");
#endif
			UpdatePropertyPage();	//�������Կ򷽱��޸Ķκ�
			return;
		}
		//���´��뱣֤��δָ���κŷ�Χʱ����ϣ���о��������漰���Ķκ� wjh-2012.12.22
		for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(pPart->iSeg.iSeg!=0&&!pPart->m_bVirtualPart)
				hashSegTbl.SetValue(pPart->iSeg.iSeg,pPart->iSeg);
		}
	}
	//��Ž�����
	CString PartNoRecord;
	CStringArray PartNoRecordArr;
	CLDSPartPtr* ta_atom_arr = new CLDSPartPtr[n];
	try
	{
		CLDSPart *pPart;
		//�������й�����ǰ�ı��
		CTmaPtrList<CLDSParamPlate,CLDSParamPlate*> xendset;
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&((((CLDSParamPlate*)pPart)->m_bStdPart)||
				(codesetdlg.m_bReservFlNo&&((CLDSParamPlate*)pPart)->m_iParamType==TYPE_FL)))
			{	//��׼�������������������µ���ͨ����������չ������	wjh-2012.4.21
				if(((CLDSParamPlate*)pPart)->m_iParamType==TYPE_XEND)
					xendset.append((CLDSParamPlate*)pPart);
				pPart->feature=0;	//������ձ�Ź���
			}
			else
				pPart->feature=1;	//����ձ�Ź���
		}
		for(CLDSParamPlate* pParamPlate=xendset.GetFirst();pParamPlate;pParamPlate=xendset.GetNext())
		{	//ʮ�ֲ���׼���ĸ���һ��ҲӦ���������Ӽ���
			if(pParamPlate->m_iParamType!=TYPE_XEND)
				continue;
			for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
			{
				pPart=console.FromPartHandle(pItem->h,pItem->idClsType);
				pPart->feature=0;
			}
		}
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(pPart->feature==0 || pPart->m_bVirtualPart)	//������ձ�ŵĹ���
				continue;
			if(hashSegTbl.GetNodeNum()>0&&hashSegTbl.GetValue(pPart->iSeg.iSeg)==NULL)
				continue;//���ǵ�ǰ�η�Χ�ڵĹ���
			pPart->SetPartNo("");
		}
		POLYPARTLIST polylist;
		Ta.RetrievePolyPartList(polylist);
#ifdef AFX_TARG_ENU_ENGLISH
		PutMessage(this,"Tt is setting up the summary library of whole tower's parts!");
#else 
		PutMessage(this,"���ڽ���ȫ���Ĺ������ܿ�!");
#endif
		i = n = 0;
		PARTSET flset;
		//ͳ����ͨ��Ź���
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(pPart->IsShadowObject())
				continue;	//Ӱ�乹���������Զ���� wht 13-04-28
			else if(pPart->GetClassTypeId()==CLS_BOLT||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE||pPart->m_bVirtualPart)
				continue;	//������˨����ϽǸ֡����⹹���Ȳ���ͳ�ƵĹ���
			else if(pPart->IsLinePart()&&(((CLDSLinePart*)pPart)->bHuoQuStart||((CLDSLinePart*)pPart)->bHuoQuEnd))
				continue;	//�����ӹ����ɻ�����������ͳ��
			else if(hashSegTbl.GetNodeNum()>0&&hashSegTbl.GetValue(pPart->iSeg.iSeg)==NULL)
				continue; //���ǵ�ǰ�η�Χ�ڵĹ���
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)pPart)->m_bStdPart)
				continue;	//��׼�����������Զ���� wht 11-01-11
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)pPart)->m_iParamType==TYPE_FL)
			{	//�����������
				flset.append(pPart);
				continue;
			}
			else	//δ�������乹��
				ta_atom_arr[n]=pPart;
			n++;
		}
		//ͳ�����乹��
		CLDSPolyPart *pPolyPart;
		for(pPolyPart=polylist.GetFirst();pPolyPart;pPolyPart=polylist.GetNext())
		{
			if(hashSegTbl.GetNodeNum()>0&&hashSegTbl.GetValue(pPolyPart->iSeg.iSeg)==NULL)
				continue;	//���ǵ�ǰ�η�Χ�ڵĹ���
			ta_atom_arr[n] = pPolyPart;
			n++;
		}
		//ָ���κŷ�Χʱ,ta_atom_arr��������ֱ�Ӽ������乹�������� wht 12-02-24
		//n+=polylist.GetNodeNum();
		//ͳ�Ʒ���
		CLDSPartPtr* fl_arr=new CLDSPartPtr[flset.GetNodeNum()];
		i=0;
		for(pPart=flset.GetFirst(),i=0;pPart;pPart=flset.GetNext(),i++)
			fl_arr[i]=pPart;
		CQuickSort<CLDSPartPtr>::QuickSort(fl_arr,flset.GetNodeNum(),CompareFL);
		serial_no=1;
		CXhChar16 sPartNo;
		for(i=0;i<flset.GetNodeNum();i++)
		{
			CLDSParamPlate *pFl=(CLDSParamPlate*)fl_arr[i];
			int str_len=strlen(pFl->GetPartNo());
			if(str_len==0)
			{	//δ���
				sPartNo.Printf("FL%d",serial_no);
				pFl->SetPartNo(sPartNo);
				for(j=i+1;j<flset.GetNodeNum();j++)
				{
					CLDSParamPlate *pCompareFl=(CLDSParamPlate*)fl_arr[j];
					if(strlen(pCompareFl->GetPartNo())>0)
						continue;
					if(DISTANCE(pFl->ucs.origin,pCompareFl->ucs.origin)<100&&!pFl->IsEqual(pCompareFl,codesetdlg.m_fScope))
					{	//�гɶԷ������Ҳ�һ��ʱ��A,B�ŷֱ���
						sPartNo.Append("A");
						pFl->SetPartNo(sPartNo);
						//
						CXhChar16 sCoupleFlPartNo("FL%dB*",serial_no);
						pCompareFl->SetPartNo(sCoupleFlPartNo);
						break;
					}
				}
				PartNoRecord.Format("%4X,%s,F,N,%s",pFl->handle,(char*)pFl->iSeg.ToString(),(char*)pFl->GetPartNo());
				PartNoRecordArr.Add(PartNoRecord);
				serial_no++;
			}
			sPartNo.Printf("%s",(char*)pFl->GetPartNo());
#ifndef __ENABLE_XHSTRING_LENGTH_PROPERTY_
			if(sPartNo.Length()>0&&!sPartNo.EndWith('*'))
#else
			if(sPartNo.Length>0&&!sPartNo.EndWith('*'))
#endif
				continue;
			else
			{	//��*�Ž����ķ����ǳɶ����ɶ����ǱȽ����ɵı����Ҫ������������й����Ƚ�
				if(str_len>0)
				{
					sPartNo.At(str_len-1)='\0';	//ȥ��'*'
					pFl->SetPartNo(sPartNo);	
					PartNoRecord.Format("%4X,%s,F,N,%s",pFl->handle,(char*)pFl->iSeg.ToString(),(char*)pFl->GetPartNo());
					PartNoRecordArr.Add(PartNoRecord);
				}
				for(j=i+1;j<flset.GetNodeNum();j++)
				{
					CLDSParamPlate *pCompareFl=(CLDSParamPlate*)fl_arr[j];
					if(strlen(pCompareFl->GetPartNo())>0)
						continue;	//���б��
					if(pFl->IsEqual(pCompareFl,codesetdlg.m_fScope))
					{	//�гɶԷ���
						pCompareFl->SetPartNo(pFl->GetPartNo());
						PartNoRecord.Format("%4X,%s,F,R,%s",pCompareFl->handle,(char*)pCompareFl->iSeg.ToString(),(char*)pCompareFl->GetPartNo());
						PartNoRecordArr.Add(PartNoRecord);
					}
				}
			}
		}
		char sPrevPartNo[20]="";
		for(i=0;i<flset.GetNodeNum();i++)
		{
			CLDSParamPlate *pFl=(CLDSParamPlate*)fl_arr[i];
			int nRib=pFl->GetAttachPartCount();
			CLDSPart** rib_arr=new CLDSPart*[nRib];
			j=0;
			for(ATTACHPART_ITEM *pItem=pFl->GetFirstAttachPart();pItem;pItem=pFl->GetNextAttachPart())
			{
				rib_arr[j]=console.FromPartHandle(pItem->h);
				j++;
			}
			int iRib=2;	//���Ű�Ĭ����ָ�����װ壬�����ӣ��ſ�ʼ���
			for(j=0;j<nRib;j++)
			{
				if(rib_arr[j]==NULL)
					continue;
				else if(strlen(rib_arr[j]->GetPartNo())>0)
					continue;
				else
				{
					sPartNo.Printf("%s-%d",(char*)pFl->GetPartNo(),iRib);
					rib_arr[j]->SetPartNo(sPartNo);
				}
				if(stricmp(sPrevPartNo,rib_arr[j]->GetPartNo())==0)
					PartNoRecord.Format("%4X,%s,-,R,%s",rib_arr[j]->handle,(char*)rib_arr[j]->iSeg.ToString(),(char*)rib_arr[j]->GetPartNo());
				else
				{
					PartNoRecord.Format("%4X,%s,-,N,%s",rib_arr[j]->handle,(char*)rib_arr[j]->iSeg.ToString(),(char*)rib_arr[j]->GetPartNo());
					strcpy(sPrevPartNo,rib_arr[j]->GetPartNo());
				}
				PartNoRecordArr.Add(PartNoRecord);
				for(int k=j+1;k<nRib;k++)
				{
					if(rib_arr[k]==NULL)
						continue;
					else if(rib_arr[j]->IsEqual(rib_arr[k],TRUE,codesetdlg.m_fScope))
					{
						rib_arr[k]->SetPartNo(rib_arr[j]->GetPartNo());
						PartNoRecord.Format("%4X,%s,-,R,%s",rib_arr[k]->handle,(char*)rib_arr[k]->iSeg.ToString(),(char*)rib_arr[k]->GetPartNo());
						PartNoRecordArr.Add(PartNoRecord);
					}
				}
			}
			delete []rib_arr;
		}
		delete []fl_arr;

		//�����ۺ�����,
		CBubbleSort<CLDSPart*>::BubSort(ta_atom_arr,n,ComplexCompareFunc);
		//�����麸���鲢��λ����
		PLACEINDEX* pIndex;
		CHashListEx<PLACEINDEX> dynaObjIndex;
		for(i=0;i<n;i++)
		{
			pIndex=dynaObjIndex.Add(ta_atom_arr[i]->handle);
			pIndex->index=i;
			pIndex->pObj=ta_atom_arr[i];
		}
		CHashListEx<CBriefWeldPart> hashWeldParts;
		Ta.RetrieveBriefWeldPartGroups(hashWeldParts,codesetdlg.m_sSegStr);
		for(CBriefWeldPart* pWeldPart=hashWeldParts.GetFirst();pWeldPart;pWeldPart=hashWeldParts.GetNext())
		{
			if( pWeldPart->ciWeldType==CBriefWeldPart::WELD_ANGLEFOOT||
				pWeldPart->ciWeldType==CBriefWeldPart::WELD_STDX_PLATES||
				pWeldPart->ciWeldType==CBriefWeldPart::WELD_TUBECONNPLATES_INC_COVER)
			{
				WELDSONPART* pSonPart;
				ARRAY_LIST<WELDSONPART> arraySonParts(0,8);
				for(pSonPart=pWeldPart->EnumObjectFirst();pSonPart;pSonPart=pWeldPart->EnumObjectNext())
					arraySonParts.append(*pSonPart);
				CBubbleSort<WELDSONPART>::BubSort(arraySonParts.m_pData,arraySonParts.GetSize(),_LocalCompareWeldSonPartItem);
				for(i=0;i<arraySonParts.GetSize();i++)
				{
					if(i==0)
					{	
						if(arraySonParts[0].iFuncType<pWeldPart->pRootSonPart->iFuncType)
							PLACEINDEX::MoveBefore(&dynaObjIndex,arraySonParts[0].hPart,pWeldPart->pRootSonPart->hPart);
						else if(arraySonParts[0].iFuncType>pWeldPart->pRootSonPart->iFuncType)
							PLACEINDEX::MoveAfter(&dynaObjIndex,arraySonParts[0].hPart,pWeldPart->pRootSonPart->hPart);
					}
					else
					{
						if(arraySonParts[i].iFuncType>arraySonParts[i-1].iFuncType)
							PLACEINDEX::MoveAfter(&dynaObjIndex,arraySonParts[i].hPart,arraySonParts[i-1].hPart);
						//else if(arraySonParts[0].iFuncType==pWeldPart->pRootSonPart->iFuncType)
						//	continue;
					}
				}
			}
			//PLACEINDEX::MoveBefore
		}
		memset(ta_atom_arr,0,sizeof(CLDSPart*)*n);
		for(pIndex=dynaObjIndex.GetFirst();pIndex;pIndex=dynaObjIndex.GetNext())
			ta_atom_arr[pIndex->index]=(CLDSPartPtr)pIndex->pObj;
		for(i=0;i<n;i++)
		{
			if(ta_atom_arr[i]==NULL)
				logerr.Log("�麸��������λ�������!");
		}

		char sPrompt[MAX_TEMP_CHAR_100+1];
		for(SEGI* pIseg=hashSegTbl.GetFirst();pIseg;pIseg=hashSegTbl.GetNext())
		{	
			serial_no = 1;
#ifdef AFX_TARG_ENU_ENGLISH
			_snprintf(sPrompt,MAX_TEMP_CHAR_100,"The %s section of part number is in progress",(char*)pIseg->ToString());
#else 
			_snprintf(sPrompt,MAX_TEMP_CHAR_100,"���ڽ��е�%s�ֶεĹ������",(char*)pIseg->ToString());
#endif
			PutMessage(this,sPrompt);
			for(j=0;j<n;j++)
			{
				if(ta_atom_arr[j]->iSeg.iSeg!=pIseg->iSeg)	//�κ��ж�
					continue;
				if(ta_atom_arr[j]->m_bVirtualPart)
					continue;
				if(strlen(ta_atom_arr[j]->GetPartNo())>0)				//�Ƿ��ѱ��
					continue;
				while(FindPartNo(ta_atom_arr[j]->GetPartNo()))
				{
					if(serial_no<10)
						ta_atom_arr[j]->SetPartNo(CXhChar16("%s0%d",(char*)ta_atom_arr[j]->iSeg.ToString(),serial_no));
					else if(serial_no<100)
						ta_atom_arr[j]->SetPartNo(CXhChar16("%s%d",(char*)ta_atom_arr[j]->iSeg.ToString(),serial_no));
					else
						ta_atom_arr[j]->SetPartNo(CXhChar16("%s-%d",(char*)ta_atom_arr[j]->iSeg.ToString(),serial_no));
					serial_no++;
				}
				if(ta_atom_arr[j]->GetClassTypeId()==CLS_POLYPART)
				{	//���乹���������ӹ�����ӵ����ͬ�ı��
					pPolyPart=(CLDSPolyPart*)ta_atom_arr[j];
					CLDSLinePart *pLinePart;
					for(pLinePart=pPolyPart->segset.GetFirst();pLinePart;pLinePart=pPolyPart->segset.GetNext())
						pLinePart->SetPartNo(pPolyPart->GetPartNo());
					pLinePart=((CLDSPolyPart*)ta_atom_arr[j])->segset.GetFirst();
					PartNoRecord.Format("%4X,%s,L,N,%s",pLinePart->handle,(char*)pLinePart->iSeg.ToString(),(char*)pLinePart->GetPartNo());
				}
				else if(ta_atom_arr[j]->GetClassTypeId()==CLS_LINEANGLE)
					PartNoRecord.Format("%4X,%s,L,N,%s",ta_atom_arr[j]->handle,(char*)ta_atom_arr[j]->iSeg.ToString(),(char*)ta_atom_arr[j]->GetPartNo());
				else if(ta_atom_arr[j]->GetClassTypeId()==CLS_LINETUBE)
					PartNoRecord.Format("%4X,%s,P,N,%s",ta_atom_arr[j]->handle,(char*)ta_atom_arr[j]->iSeg.ToString(),(char*)ta_atom_arr[j]->GetPartNo());
				else if(ta_atom_arr[j]->GetClassTypeId()==CLS_PLATE||ta_atom_arr[j]->GetClassTypeId()==CLS_PARAMPLATE)
					PartNoRecord.Format("%4X,%s,-,N,%s",ta_atom_arr[j]->handle,(char*)ta_atom_arr[j]->iSeg.ToString(),(char*)ta_atom_arr[j]->GetPartNo());
				else //if(ta_atom_arr[j]->GetClassTypeId()==CLS_PLATE||ta_atom_arr[j]->GetClassTypeId()==CLS_PARAMPLATE)
					PartNoRecord.Format("%4X,%s,?,N,%s",ta_atom_arr[j]->handle,(char*)ta_atom_arr[j]->iSeg.ToString(),(char*)ta_atom_arr[j]->GetPartNo());
				PartNoRecordArr.Add(PartNoRecord);
				Ta.Parts.push_stack();
				for(CLDSPart *pEqualPart=Ta.Parts.GetFirst();pEqualPart;pEqualPart=Ta.Parts.GetNext())
				{
					if(pEqualPart==ta_atom_arr[j])
						continue;
					else if(pEqualPart->m_bVirtualPart)
						continue;
					else if(strlen(pEqualPart->GetPartNo())>0)
						continue;	//���б��
					else if(strlen(pEqualPart->GetPartNo())==0&&pEqualPart->iSeg.iSeg==pIseg->iSeg
						&&ta_atom_arr[j]->IsEqual(pEqualPart,TRUE,codesetdlg.m_fScope))
					{
						pEqualPart->SetPartNo(ta_atom_arr[j]->GetPartNo());
						if(ta_atom_arr[j]->GetClassTypeId()==CLS_LINEANGLE)
							PartNoRecord.Format("%4X,%s,L,R,%s",pEqualPart->handle,(char*)pEqualPart->iSeg.ToString(),(char*)pEqualPart->GetPartNo());
						else if(ta_atom_arr[j]->GetClassTypeId()==CLS_LINETUBE)
							PartNoRecord.Format("%4X,%s,P,R,%s",pEqualPart->handle,(char*)pEqualPart->iSeg.ToString(),(char*)pEqualPart->GetPartNo());
						else if(ta_atom_arr[j]->GetClassTypeId()==CLS_PLATE||ta_atom_arr[j]->GetClassTypeId()==CLS_PARAMPLATE)
							PartNoRecord.Format("%4X,%s,-,R,%s",pEqualPart->handle,(char*)pEqualPart->iSeg.ToString(),(char*)pEqualPart->GetPartNo());
						else //if(ta_atom_arr[j]->GetClassTypeId()==CLS_PLATE||ta_atom_arr[j]->GetClassTypeId()==CLS_PARAMPLATE)
							PartNoRecord.Format("%4X,%s,?,R,%s",pEqualPart->handle,(char*)pEqualPart->iSeg.ToString(),(char*)pEqualPart->GetPartNo());
						PartNoRecordArr.Add(PartNoRecord);
					}
				}
				if(ta_atom_arr[j]->GetClassTypeId()==CLS_POLYPART)
				{	//���乹���������ӹ�����ӵ����ͬ�ı��
					pPolyPart=(CLDSPolyPart*)ta_atom_arr[j];
					CLDSLinePart *pLinePart;
					for(CLDSPolyPart *pEqualPolyPart=polylist.GetFirst();pEqualPolyPart;pEqualPolyPart=polylist.GetNext())
					{
						if(strlen(pEqualPolyPart->GetPartNo())==0&&pEqualPolyPart->iSeg.iSeg==pIseg->iSeg
							&&ta_atom_arr[j]->IsEqual(pEqualPolyPart,TRUE,codesetdlg.m_fScope))
						{
							pEqualPolyPart->SetPartNo(ta_atom_arr[j]->GetPartNo());
							for(pLinePart=pEqualPolyPart->segset.GetFirst();pLinePart;pLinePart=pEqualPolyPart->segset.GetNext())
								pLinePart->SetPartNo(pEqualPolyPart->GetPartNo());
							pLinePart=pEqualPolyPart->segset.GetFirst();
							if(ta_atom_arr[j]->GetClassTypeId()==CLS_LINEANGLE)
								PartNoRecord.Format("%4X,%s,L,R,%s",pLinePart->handle,(char*)pLinePart->iSeg.ToString(),(char*)pLinePart->GetPartNo());
							else if(ta_atom_arr[j]->GetClassTypeId()==CLS_LINETUBE)
								PartNoRecord.Format("%4X,%s,P,R,%s",pLinePart->handle,(char*)pLinePart->iSeg.ToString(),(char*)pLinePart->GetPartNo());
							else if(ta_atom_arr[j]->GetClassTypeId()==CLS_PLATE||ta_atom_arr[j]->GetClassTypeId()==CLS_PARAMPLATE)
								PartNoRecord.Format("%4X,%s,-,R,%s",pLinePart->handle,(char*)pLinePart->iSeg.ToString(),(char*)pLinePart->GetPartNo());
							else //if(ta_atom_arr[j]->GetClassTypeId()==CLS_PLATE||ta_atom_arr[j]->GetClassTypeId()==CLS_PARAMPLATE)
								PartNoRecord.Format("%4X,%s,?,R,%s",pLinePart->handle,(char*)pLinePart->iSeg.ToString(),(char*)pLinePart->GetPartNo());
							PartNoRecordArr.Add(PartNoRecord);
						}
					}
				}
				Ta.Parts.pop_stack();
			}
			
			if(serial_no>9999)	//����������� ��ʱ����� wht 08-7-16
			{
				delete []ta_atom_arr;
				char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
				_snprintf(sError,199,"The first %s segment more than limits of 999, number failed!",(char*)pIseg->ToString());
#else 
				_snprintf(sError,199,"��%s�γ���999���Ĺ���������,���ʧ��!",(char*)pIseg->ToString());
#endif
				throw sError;
			}
			MaxCodeNo += serial_no;
		}
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
		PutMessage(this,NULL);
		delete []ta_atom_arr;
		return;
	}
	delete []ta_atom_arr;
	EndWaitCursor();
#ifdef AFX_TARG_ENU_ENGLISH
	MessageBox("Complete tower's label");
#else 
	MessageBox("���ı�����");
#endif
	PutMessage(this,NULL);
	m_pDoc->SetModifiedFlag();
	/*if(MessageBox("�Ƿ񽫱�Ź��̱����������Ա��ٴα��ʹ�ã�","LDS",MB_YESNO)==IDYES)
	{
		CFileDialog file_dlg(FALSE,"no","LDS",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"��Ź����ļ�(*.no)|*.no||");
		if(file_dlg.DoModal()==IDOK)
		{
			char file_name[MAX_PATH];
			_snprintf(file_name,MAX_PATH-1,"%s",file_dlg.GetPathName());
			FILE *fp=fopen(file_name,"wt");
			fprintf(fp,"����������κţ��������ͣ�������ͣ��������\n'L':�Ǹ�;'P':�ֹ�;'-':�ְ�;'F'����; 'N':�±��;'R':�ظ���һ���;'S':ָ��Ϊ����ı��\n");
			for(i=0;i<PartNoRecordArr.GetSize();i++)
				fprintf(fp,"%s\n",PartNoRecordArr[i]);
			fclose(fp);
		}
	}*/
#endif
}

void CLDSView::OnSegICheck() 
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	m_nPrevCommandID=ID_SEG_I_CHECK;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat Check Segment Number";
	if(CheckPartSegI())
		MessageBox("Through the detection of part's segment number ");
#else 
	m_sPrevCommandName="�ظ��κż��";
	if(CheckPartSegI())
		MessageBox("ͨ�������ֶκż��");
#endif
	UpdatePropertyPage();	//�������Կ򷽱��޸Ķκ�
#endif
}

static double scope=0.5;
CCheckPartNoDlg dlg;
void CLDSView::OnPartNoCheck() 
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	m_nPrevCommandID=ID_PART_NO_CHECK;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat Check Part Number";
#else 
	m_sPrevCommandName="�ظ������ż��";
#endif
	//Input("У����Χ",&scope,'f');
	dlg.m_fScope=scope;
	dlg.m_bDisplayFlNoCtrl=false;
	if(IDOK!=dlg.DoModal())
		return;
	scope=dlg.m_fScope;
	POLYPARTLIST polylist;
	Ta.RetrievePolyPartList(polylist);
	int i=0,nHits=0,n=Ta.Parts.GetNodeNum(),poly_n=polylist.GetNodeNum();
	CSuperSmartPtr<CLDSPart>pPart;//,pOthPart;
	FILE *fp;
	//�ֽ�κ��ַ���
	CHashList<SEGI>segTbl;
	GetSegNoHashTblBySegStr(dlg.m_sSegStr,segTbl);
	fp = fopen("NoCheck.txt","wt");
	CString sError,sCurIndex;
	CHashStrTable<CLDSPart*>partTbl;
	partTbl.CreateHashTable(n);
	g_pSolidDraw->ReleaseSnapStatus();
	CTaskOutputList itemList;
	try{
		for(pPart=Ta.Parts.GetFirst();pPart.IsHasPtr();pPart=Ta.Parts.GetNext())
		{
			i++;
			//if(pPart->handle==0x9026||pPart->handle==0x9027)
				int b=-3;
			//else
			//	continue;
			if(pPart->GetClassTypeId()==CLS_BOLT||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE||
				pPart->GetClassTypeId()==CLS_POLYJOINT||pPart->m_bVirtualPart)
				continue;	//��˨�ޱ�Ų���Ԥ�����ż��
			else if(pPart->IsLinePart()&&(pPart.LinePartPointer()->bHuoQuStart||pPart.LinePartPointer()->bHuoQuEnd))
				continue;
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPart->dwPermission))
				continue;	//����Эͬʱ���Դ˽Ǹ�û��ӵ���޸�Ȩ��
			if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pPart->iSeg.iSeg)==NULL)
				continue;	//����Ƿ���ָ���κŷ�Χ��
			else if(strlen(pPart->GetPartNo())<1)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(pPart->GetClassTypeId()==CLS_LINEANGLE)
					sError.Format("The angle's number of 0X%X inputs error,please check it!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_LINETUBE)
					sError.Format("The tube's number of 0X%X inputs error,please check it!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_LINESLOT)
					sError.Format("The U-steel's number of 0X%X inputs error,please check it!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_PLATE)
					sError.Format("The plate's number of 0X%X inputs error,please check it!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
					sError.Format("The parameter plate's number of 0X%X inputs error,please check it!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
					sError.Format("The flat angle's number of 0X%X inputs error,please check it!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_ARCANGLE)
					sError.Format("The circular angle's number of 0X%X inputs error,please check it!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_ARCFLAT)
					sError.Format("The circular flat angle's number of 0X%X inputs error,please check it!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_ARCSLOT)
					sError.Format("The circular U-steel's number of 0X%X inputs error,please check it!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_SPHERE)
					sError.Format("The sphere's number of 0X%X inputs error,please check it!",pPart->handle);
#else 
				if(pPart->GetClassTypeId()==CLS_LINEANGLE)
					sError.Format("0X%X�ǸֵĹ�����δ��ȷ����,���ʵ!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_LINETUBE)
					sError.Format("0X%X�ֹܵĹ�����δ��ȷ����,���ʵ!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_LINESLOT)
					sError.Format("0X%X�۸ֵĹ�����δ��ȷ����,���ʵ!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_PLATE)
					sError.Format("0X%X�ְ�Ĺ�����δ��ȷ����,���ʵ!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
					sError.Format("0X%X�������ְ�Ĺ�����δ��ȷ����,���ʵ!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
					sError.Format("0X%X��ֵĹ�����δ��ȷ����,���ʵ!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_ARCANGLE)
					sError.Format("0X%X���νǸֵĹ�����δ��ȷ����,���ʵ!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_ARCFLAT)
					sError.Format("0X%X���α�ֵĹ�����δ��ȷ����,���ʵ!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_ARCSLOT)
					sError.Format("0X%X���β۸ֵĹ�����δ��ȷ����,���ʵ!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_SPHERE)
					sError.Format("0X%X����Ĺ�����δ��ȷ����,���ʵ!",pPart->handle);
#endif
				if(strlen(sError)>0)
				{
					fprintf(fp,"%s\n",sError);
					nHits++;
					AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo((char*)pPart->GetPartNo(),pPart->handle,0,sError));
				}
				continue;
			}
			else
			{
				CLDSPart *pTmPart=NULL;
				//���ڲ���Ժ��׼����Ŵ���Сд��ĸ,���Ա����¡��һ������ַ�������Ϊ��д wjh-2012.3.18
				CXhChar50 upCasePartNo(pPart->GetPartNo());
				strupr(upCasePartNo);	//����������е���ĸ��Ϊ��д wht 09-09-27
				if(!partTbl.GetValueAt(upCasePartNo,pTmPart))	//�ü��ŵ�һ������
					partTbl.SetValueAt(upCasePartNo,pPart);
				else
				{
					CLDSParamPlate *pParamPlate=NULL;
					if(pTmPart->GetClassTypeId()==CLS_PARAMPLATE)
						pParamPlate=(CLDSParamPlate*)pTmPart;
					char error_info[200]="";
					if(pPart->GetClassTypeId()!=pTmPart->GetClassTypeId())
					{
#ifdef AFX_TARG_ENU_ENGLISH
						sError.Format("The type of 0X%X and 0X%X is different,but they have same piece number��",pPart->handle,pTmPart->handle);
#else 
						sError.Format("0X%X������0X%X��������ͬһ���͹�������������ͬ�Ĺ����ţ�",pPart->handle,pTmPart->handle);
#endif
						//g_pSolidDraw->SetEntSnapStatus(pPart->handle);
						//g_pSolidDraw->SetEntSnapStatus(pTmPart->handle);
						fprintf(fp,"%s\n",sError);
						AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo((char*)pPart->GetPartNo(),pPart->handle,pTmPart->handle,sError));
						nHits++;
					}
					//������׼����
					/*else if(pPart->iSeg!=pTmPart->iSeg&&(pParamPlate==NULL||(pParamPlate&&!pParamPlate->m_bStdPart)))
					{	//�����ͬ�Ĺ�������һ��Ҫλ��ͬһ����(��׼����������)����������������ͬ��λ��ͬһ��
						if(pParamPlate&&pParamPlate->m_iParamType==TYPE_FL)
							sError.Format("0X%X������0X%X��������������ͬ�Ĺ�����(%s)��",pPart->handle,pTmPart->handle,(char*)pPart->GetPartNo());
						else
							sError.Format("0X%X������0X%X��������������ͬ�Ĺ�����(%s)��",pPart->handle,pTmPart->handle,(char*)pPart->GetPartNo());
						g_pSolidDraw->SetEntSnapStatus(pPart->handle);
						g_pSolidDraw->SetEntSnapStatus(pTmPart->handle);
						fprintf(fp,"%s\n",sError);
						nHits++;
					}*/
					else if(!pPart->IsEqual(pTmPart,TRUE,scope,error_info))
					{
#ifdef AFX_TARG_ENU_ENGLISH
						if(pPart->GetClassTypeId()==CLS_LINEANGLE)
							sError.Format("Angle 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_LINETUBE)
							sError.Format("Tube 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_LINESLOT)
							sError.Format("U-steel 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_PLATE)
							sError.Format("Plane 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
							sError.Format("Parameter Plate 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
							sError.Format("Flat angle 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_ARCSLOT)
							sError.Format("Circular U-steel 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_ARCANGLE)
							sError.Format("Circular angle 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_ARCFLAT)
							sError.Format("Circular flat angle 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_SPHERE)
							sError.Format("Sphere 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
#else 
						if(pPart->GetClassTypeId()==CLS_LINEANGLE)
							sError.Format("�Ǹ�0X%X��0X%X(%s)��������ͬ�Ĺ�����",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_LINETUBE)
							sError.Format("�ֹ�0X%X��0X%X(%s)��������ͬ�Ĺ�����",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_LINESLOT)
							sError.Format("�ֹ�0X%X��0X%X(%s)��������ͬ�Ĺ�����",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_PLATE)
							sError.Format("�ְ�0X%X��0X%X(%s)��������ͬ�Ĺ�����",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
							sError.Format("�������ְ�0X%X��0X%X(%s)��������ͬ�Ĺ�����",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
							sError.Format("���0X%X��0X%X(%s)��������ͬ�Ĺ�����",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_ARCSLOT)
							sError.Format("���β۸�0X%X��0X%X(%s)��������ͬ�Ĺ�����",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_ARCANGLE)
							sError.Format("���νǸ�0X%X��0X%X(%s)��������ͬ�Ĺ�����",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_ARCFLAT)
							sError.Format("���α��0X%X��0X%X(%s)��������ͬ�Ĺ�����",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_SPHERE)
							sError.Format("����0X%X��0X%X(%s)��������ͬ�Ĺ�����",pPart->handle,pTmPart->handle,error_info);
#endif
						//g_pSolidDraw->SetEntSnapStatus(pPart->handle);
						//g_pSolidDraw->SetEntSnapStatus(pTmPart->handle);
						fprintf(fp,"%s\n",sError);
						AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo((char*)pPart->GetPartNo(),pPart->handle,pTmPart->handle,sError));
						nHits++;
					}
				}
			}
			DisplayProcess((i*100)/(n+poly_n));
		}
		CLDSPolyPart *pPolyPart=NULL,*pOthPolyPart=NULL;
		CSuperSmartPtr<CLDSLinePart>pFirstLinePart;
		for(pPolyPart=polylist.GetFirst();pPolyPart;pPolyPart=polylist.GetNext())
		{
			i++;
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPolyPart->dwPermission))
				continue;	//����Эͬʱ���Դ˽Ǹ�û��ӵ���޸�Ȩ��
			if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pPolyPart->iSeg.iSeg)==NULL)
				continue;	//����Ƿ���ָ��κŷ�Χ��
			pFirstLinePart=pPolyPart->segset.GetFirst();
			if(strlen(pPolyPart->GetPartNo())<1)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(pFirstLinePart->GetClassTypeId()==CLS_LINEANGLE)
					sError.Format("The angle's number of 0X%X inputs error,please check it!",pFirstLinePart->handle);
				else if(pFirstLinePart->GetClassTypeId()==CLS_LINETUBE)
					sError.Format("The tube's number of 0X%X inputs error,please check it!",pFirstLinePart->handle);
				else if(pFirstLinePart->GetClassTypeId()==CLS_LINESLOT)
					sError.Format("The U-steel's number of 0X%X inputs error,please check it!",pFirstLinePart->handle);
				else if(pFirstLinePart->GetClassTypeId()==CLS_LINEFLAT)
					sError.Format("The flat angle's number of 0X%X inputs error,please check it!",pFirstLinePart->handle);
#else 
				if(pFirstLinePart->GetClassTypeId()==CLS_LINEANGLE)
					sError.Format("0X%X�ǸֵĹ�����δ��ȷ����,���ʵ!",pFirstLinePart->handle);
				else if(pFirstLinePart->GetClassTypeId()==CLS_LINETUBE)
					sError.Format("0X%X�ֹܵĹ�����δ��ȷ����,���ʵ!",pFirstLinePart->handle);
				else if(pFirstLinePart->GetClassTypeId()==CLS_LINESLOT)
					sError.Format("0X%X�۸ֵĹ�����δ��ȷ����,���ʵ!",pFirstLinePart->handle);
				else if(pFirstLinePart->GetClassTypeId()==CLS_LINEFLAT)
					sError.Format("0X%X��ֵĹ�����δ��ȷ����,���ʵ!",pFirstLinePart->handle);
#endif
				else
					continue;
				fprintf(fp,"%s\n",sError);
				AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo((char*)pFirstLinePart->GetPartNo(),pFirstLinePart->handle,0,sError));
				nHits++;
				continue;
			}
			else
			{
				CLDSPart *pTmPart=NULL;
				CXhChar50 upCasePartNo(pPolyPart->GetPartNo());
				strupr(upCasePartNo);	//����������е���ĸ��Ϊ��д wht 09-09-27
				if(partTbl.GetValueAt(upCasePartNo,pTmPart))
				{	//��������������Ƿ�������������ų�ͻ
					if(pTmPart->GetClassTypeId()!=pPolyPart->GetClassTypeId())
					{
#ifdef AFX_TARG_ENU_ENGLISH
						sError.Format("The type of 0X%X and 0X%X is different,but they have same piece number��",pTmPart->handle,pFirstLinePart->handle,(char*)pTmPart->GetPartNo());
#else 
						sError.Format("0X%X������0X%X����{%s}����ͬһ���͹�������������ͬ�Ĺ����ţ�",pTmPart->handle,pFirstLinePart->handle,(char*)pTmPart->GetPartNo());
#endif
						//g_pSolidDraw->SetEntSnapStatus(pFirstLinePart->handle);
						//g_pSolidDraw->SetEntSnapStatus(pTmPart->handle);
						fprintf(fp,"%s\n",sError);
						AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo((char*)pFirstLinePart->GetPartNo(),pFirstLinePart->handle,pTmPart->handle,sError));
						nHits++;
					}
				}
				polylist.push_stack();
				for(pOthPolyPart=polylist.GetNext();pOthPolyPart;pOthPolyPart=polylist.GetNext())
				{
					if(stricmp(pPolyPart->GetPartNo(),pOthPolyPart->GetPartNo())==0)
					{
						char error_info[50]="";
						CLDSLinePart *pOthFirstLinePart=pOthPolyPart->segset.GetFirst();
						if(pFirstLinePart->GetClassTypeId()!=pOthFirstLinePart->GetClassTypeId())
						{
#ifdef AFX_TARG_ENU_ENGLISH
							sError.Format("The type of 0X%X and 0X%X is different,but they have same piece number��",pFirstLinePart->handle,pOthFirstLinePart->handle,(char*)pFirstLinePart->GetPartNo());
#else 
							sError.Format("0X%X������0X%X����{%s}����ͬһ���͹�������������ͬ�Ĺ����ţ�",pFirstLinePart->handle,pOthFirstLinePart->handle,(char*)pFirstLinePart->GetPartNo());
#endif
							//g_pSolidDraw->SetEntSnapStatus(pFirstLinePart->handle);
							//g_pSolidDraw->SetEntSnapStatus(pOthFirstLinePart->handle);
							fprintf(fp,"%s\n",sError);
							AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo((char*)pFirstLinePart->GetPartNo(),pFirstLinePart->handle,pOthFirstLinePart->handle,sError));
							nHits++;
						}
						else if(!pPolyPart->IsEqual(pOthPolyPart,scope,error_info))
						{
#ifdef AFX_TARG_ENU_ENGLISH
							if(pFirstLinePart->GetClassTypeId()==CLS_LINEANGLE)
								sError.Format("0X%X angle and 0X%X angle are different(%s)but they have same piece number",pFirstLinePart->handle,pOthFirstLinePart->handle,error_info);
							else if(pFirstLinePart->GetClassTypeId()==CLS_LINETUBE)
								sError.Format("0X%X tube and 0X%X tube are different(%s)but they have same piece number",pFirstLinePart->handle,pOthFirstLinePart->handle,error_info);
							else if(pFirstLinePart->GetClassTypeId()==CLS_LINESLOT)
								sError.Format("0X%X U-steel and 0X%X U-steel are different(%s)but they have same piece number",pFirstLinePart->handle,pOthFirstLinePart->handle,error_info);
							else if(pFirstLinePart->GetClassTypeId()==CLS_LINEFLAT)
								sError.Format("0X%X flat angle and 0X%X flat angle are different(%s)but they have same piece number",pFirstLinePart->handle,pOthFirstLinePart->handle,error_info);
#else 
							if(pFirstLinePart->GetClassTypeId()==CLS_LINEANGLE)
								sError.Format("0X%X�Ǹ���0X%X�Ǹֲ�����ͬ(%s)��������ͬ�Ĺ�����",pFirstLinePart->handle,pOthFirstLinePart->handle,error_info);
							else if(pFirstLinePart->GetClassTypeId()==CLS_LINETUBE)
								sError.Format("0X%X�ֹ���0X%X�ֹܲ�����ͬ(%s)��������ͬ�Ĺ�����",pFirstLinePart->handle,pOthFirstLinePart->handle,error_info);
							else if(pFirstLinePart->GetClassTypeId()==CLS_LINESLOT)
								sError.Format("0X%X�۸���0X%X�۸ֲ�����ͬ(%s)��������ͬ�Ĺ�����",pFirstLinePart->handle,pOthFirstLinePart->handle,error_info);
							else if(pFirstLinePart->GetClassTypeId()==CLS_LINEFLAT)
								sError.Format("0X%X�����0X%X��ֲ�����ͬ(%s)��������ͬ�Ĺ�����",pFirstLinePart->handle,pOthFirstLinePart->handle,error_info);
#endif
							else
								continue;
							//g_pSolidDraw->SetEntSnapStatus(pFirstLinePart->handle);
							//g_pSolidDraw->SetEntSnapStatus(pOthFirstLinePart->handle);
							fprintf(fp,"%s\n",sError);
							AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo((char*)pFirstLinePart->GetPartNo(),pFirstLinePart->handle,pOthFirstLinePart->handle,sError));
							nHits++;
						}
					}
				}
				polylist.pop_stack();
			}
			DisplayProcess((i*100)/(n+poly_n));
		}
		DisplayProcess(100);
#ifdef AFX_TARG_ENU_ENGLISH
		fprintf(fp,"\n %d parts have problem��\n",nHits);
		sError.Format("%d parts have problem��",nHits);
#else 
		fprintf(fp,"\n�ܼ���%d�Թ����Ƚ������⣡\n",nHits);
		sError.Format("�ܼ���%d�Թ����Ƚ������⣡",nHits);
#endif
		fclose(fp);
		if(nHits>0)
			WinExec("notepad.exe NoCheck.txt",SW_SHOW);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox("Through part number check!");
#else 
			MessageBox("ͨ��������ż��!");
#endif
		
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
#ifdef AFX_TARG_ENU_ENGLISH
		fprintf(fp,"\n %d parts have problem��\n",nHits);
#else 
		fprintf(fp,"\n�ܼ���%d�Թ����Ƚ������⣡\n",nHits);
#endif
		fclose(fp);
		if(nHits>0)
			WinExec("notepad.exe NoCheck.txt",SW_SHOW);
	}
#endif
}
//�����������
/*void CLDSView::OnCorrectPartNo() 
{
	CLDSLineAngle *pJg,*pCheckJg;
	//CLDSPolyAngle *pHuoQuJg,*pCheckHuoQuJg;
	CLDSPlate *pPlank,*pCheckPlank;
	FILE *fp;
	fp = fopen("NoCorrect.txt","wt");
	int nHits=0;
	for(pJg=Ta.LineAngles.GetFirst();pJg;pJg=Ta.LineAngles.GetNext())
	{
		if(pJg->sub_type==TA_JG_EXPLODE)
			continue;
		else if(pJg->sub_type==TA_JG_VIRTUAL)
			continue;
		else if(pJg->m_bVirtualPart)
			continue;
		else if(strlen(pJg->GetPartNo())<3)
			continue;
		else
			pJg->Unlock();	//������������
	}
	CString sError;
	for(pPlank=Ta.Plank.GetFirst();pPlank;pPlank=Ta.Plank.GetNext())
	{
		if(pPlank->vertex_list.GetNodeNum()<3)
			continue;
		if(strlen(pPlank->GetPartNo())<3)
			continue;
		else
			pPlank->Unlock();	//������������
	}
	for(pJg=Ta.LineAngles.GetFirst();pJg;pJg=Ta.LineAngles.GetNext())
	{
		if(pJg->sub_type==TA_JG_EXPLODE)
			continue;
		else if(pJg->sub_type==TA_JG_VIRTUAL)
			continue;
		else if(pJg->m_bVirtualPart)
			continue;
		Ta.LineAngles.push_stack();
		pCheckJg=pJg;
		for(pJg=Ta.LineAngles.GetNext();pJg;pJg=Ta.LineAngles.GetNext())
		{
			if(pJg->huoqu_handle>0x20)	//�����ӽǸ�
				continue;
			if(strlen(pJg->GetPartNo())==0)
				continue;
			if(stricmp(pCheckJg->GetPartNo(),pJg->GetPartNo())==0)
			{
				if(!pCheckJg->IsEqual(pJg,1.0))
				{
					if( pJg->sPartNo[strlen(pCheckJg->GetPartNo())-1]<'0'
						||pJg->sPartNo[strlen(pCheckJg->GetPartNo())-1]>'9')
						pJg->sPartNo[strlen(pCheckJg->GetPartNo())-1]+=1;
					else
						strcat(pJg->sPartNo,"A");
					sError.Format("0X%X�Ǹֱ��:%s-->%s",pJg->handle,(char*)pCheckJg->GetPartNo(),(char*)pJg->GetPartNo());
					//AfxMessageBox(sError);
					g_pSolidDraw->SetEntSnapStatus(pJg->handle);
					g_pSolidDraw->SetEntSnapStatus(pCheckJg->handle);
					fprintf(fp,"%s\n",sError);
					nHits++;
				}
			}
		}
		Ta.LineAngles.pop_stack();
	}
	for(CLDSPolyAngle* pHuoQuJg=Ta.HuoQuJg.GetFirst();pHuoQuJg;pHuoQuJg=Ta.HuoQuJg.GetNext())
	{
		Ta.HuoQuJg.push_stack();
		CLDSPolyAngle* pCheckHuoQuJg=pHuoQuJg;
		for(pHuoQuJg=Ta.HuoQuJg.GetNext();pHuoQuJg;pHuoQuJg=Ta.HuoQuJg.GetNext())
		{
			if(stricmp(pCheckHuoQuJg->GetPartNo(),pHuoQuJg->GetPartNo())==0)
			{
				if(!pCheckHuoQuJg->IsEqual(pHuoQuJg,1.0))
				{
					char sPartNo[20],sCheckPartNo[20];
					strcpy(sPartNo,pHuoQuJg->seg_arr.GetFirst()->GetPartNo());
					strcpy(sCheckPartNo,pCheckHuoQuJg->seg_arr.GetFirst()->GetPartNo());
					if( sPartNo[strlen(sCheckPartNo)-1]<'0'
						||sPartNo[strlen(sCheckPartNo)-1]>'9')
						sPartNo[strlen(sCheckPartNo)-1]+=1;
					else
						strcat(sPartNo,"A");
					pHuoQuJg->SetPartNo(sPartNo);
					sError.Format("0X%X�����Ǹֱ��:%s-->%s",pHuoQuJg->handle,sCheckPartNo,sPartNo);
					//AfxMessageBox(sError);
					for(pJg=pHuoQuJg->seg_arr.GetFirst();pJg;pJg=pHuoQuJg->seg_arr.GetNext())
						g_pSolidDraw->SetEntSnapStatus(pJg->handle);
					for(pJg=pCheckHuoQuJg->seg_arr.GetFirst();pJg;pJg=pCheckHuoQuJg->seg_arr.GetNext())
						g_pSolidDraw->SetEntSnapStatus(pJg->handle);
					fprintf(fp,"%s\n",sError);
					nHits++;
				}
				else
					pCheckHuoQuJg->Lock();
			}
		}
		Ta.HuoQuJg.pop_stack();
	}
	for(pPlank=Ta.Plank.GetFirst();pPlank;pPlank=Ta.Plank.GetNext())
	{
		pCheckPlank=pPlank;
		if(pPlank->vertex_list.GetNodeNum()<3)
			continue;	//����˨���Ӱ�(��ʵ�ް�,ֻ����˨)
		if(strlen(pPlank->GetPartNo())==0)
			continue;
		Ta.Plank.push_stack();
		for(pPlank=Ta.Plank.GetNext();pPlank;pPlank=Ta.Plank.GetNext())
		{
			if(stricmp(pCheckPlank->GetPartNo(),pPlank->GetPartNo())==0)
			{
				if(!pCheckPlank->IsEqual(pPlank,1.0))
				{
					if( pPlank->sPartNo[strlen(pCheckPlank->sPartNo)-1]<'0'
						||pPlank->sPartNo[strlen(pCheckPlank->sPartNo)-1]>'9')
						pPlank->sPartNo[strlen(pCheckPlank->sPartNo)-1]+=1;
					else
						strcat(pPlank->sPartNo,"A");
					sError.Format("0X%X����:%s-->%s",pPlank->handle,pCheckPlank->sPartNo,pPlank->sPartNo);
					//AfxMessageBox(sError);
					g_pSolidDraw->SetEntSnapStatus(pPlank->base_node_handle);
					g_pSolidDraw->SetEntSnapStatus(pCheckPlank->base_node_handle);
					fprintf(fp,"%s\n",sError);
					nHits++;
				}
			}
		}
		Ta.Plank.pop_stack();
	}
	fclose(fp);
	if(nHits>0)
		WinExec("notepad.exe NoCorrect.txt",SW_SHOW);
	else
		MessageBox("������Ų�������!");
}*/

void CLDSView::OnLocatePartByNo() 
{
	if(m_curAppType!=TA_ASSEMBLE_ENV)
		return;	//�����༭�������θ������ݼ�F2 wht 11-05-18
	m_nPrevCommandID=ID_LOCATE_PART_BY_NO;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat Locating Part";
#else 
	m_sPrevCommandName="�ظ���λ����";
#endif
	/*
	CString sPartNo,sText;
	Input("�����빹�����:",&sPartNo,'s');
	CLDSPart *pPart;
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->m_bVirtualPart)
			continue;
		else if(sPartNo.CompareNoCase(pPart->GetPartNo())==0)
		{
			if(pPart->GetClassTypeId()==CLS_LINEANGLE)
				sText.Format("�˹���Ϊ���Ϊ0X%X�ĽǸ�",pPart->handle);
			else if(pPart->GetClassTypeId()==CLS_LINETUBE)
				sText.Format("�˹���Ϊ���Ϊ0X%X�ĸֹ�",pPart->handle);
			else if(pPart->GetClassTypeId()==CLS_LINESLOT)
				sText.Format("�˹���Ϊ���Ϊ0X%X�Ĳ۸�",pPart->handle);
			else if(pPart->GetClassTypeId()==CLS_PLATE)
				sText.Format("�˹���Ϊ���Ϊ0X%X�ĸְ�",pPart->handle);
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
				sText.Format("�˹���Ϊ���Ϊ0X%X�Ĳ������ְ�",pPart->handle);
			MessageBox(sText);
			return;
		}
	}
	AfxMessageBox("δ�ҵ��ù���!");
	*/
	//�л���ʵ����ʾ״̬
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
	long *id_arr=NULL;
	long n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(n<=0)
		return;
	CLDSPart *pSelPart=console.FromPartHandle(id_arr[0]);
#ifndef __ENABLE_XHSTRING_LENGTH_PROPERTY_
	if(pSelPart==NULL||pSelPart->GetPartNo().Length()==0)
#else
	if(pSelPart==NULL||strlen(pSelPart->GetPartNo())==0)
#endif
		return;
	if(n>1)
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pSelPart->handle);
	}
	//����ѡ��������Ĭ�ϼ��ţ������Ҷ�Ӧ����
	CXhChar16 sPartNo=pSelPart->GetPartNo();
	for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if( pPart->GetClassTypeId()==CLS_BOLT||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE||
			pPart->GetClassTypeId()==CLS_POLYJOINT)//||pPart->m_bVirtualPart)
			continue;	//��˨�ޱ�Ų���Ԥ�����ż��
		if(pPart->handle==pSelPart->handle)
			continue;
		else if(stricmp(sPartNo,pPart->GetPartNo())==0)
		{
			g_pSolidDraw->SetEntSnapStatus(pPart->handle);
			g_pSolidSet->SetOperType(OPER_ROTATE);	//���ò�������Ϊ��ת
			f3dPoint org=pPart->ucs.origin;
			g_pSolidSet->SetRotOrg(org);
			g_pSolidSet->SetZoomStyle(ROT_CENTER);	//����Ϊָ����ת��������
		}
	}
	UpdatePropertyPage();
}

//��AutoCad R14�н��л�ͼ
void CLDSView::OnDrawTaInAcad() 
{
	m_nPrevCommandID=ID_DRAW_TA_IN_ACAD;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="�ظ�Acad��ͼ";
#else 
	m_sPrevCommandName="Repeat Acad Draw";
#endif
#ifdef __LDS_TSA_	//LDS����������ģ��
	HKEY hKey;
	char acad_path[MAX_PATH]="";
	DWORD dwDataType,dwLength=MAX_PATH;
	RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\Xerofox\\LDS\\ACAD",0,KEY_READ,&hKey);
	if(RegQueryValueEx(hKey,_T("ACAD_PATH"),NULL,&dwDataType,(BYTE*)&acad_path[0],&dwLength)== ERROR_SUCCESS)
		m_sAcadPath = acad_path;
#else
		m_sAcadPath = AfxGetApp()->GetProfileString("ACAD","ACAD_PATH","");
#endif
	FILE* fp;
	CString file;
	file = m_sAcadPath.Left(m_sAcadPath.GetLength()-8);
	file+="Acad.rx";
	if((fp=fopen(file,"wt"))==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Failed to create mapping startup files,so it can't start drawing module!");
#else 
		AfxMessageBox("��ͼ�����ļ�����ʧ��,����������ͼģ��!");
#endif
		return;
	}
	else
	{
#ifdef __LDS_
		fprintf(fp,"%sMapLds.arx\n",APP_PATH);
#elif defined(__TSA_)
		fprintf(fp,"%sMapTsa.arx\n",APP_PATH);
#elif defined(__LMA_)
		fprintf(fp,"%sMapLma.arx\n",APP_PATH);
#else
		fprintf(fp,"%sMapTma.arx\n",APP_PATH);
#endif
		fclose(fp);
	}
	WinExec(m_sAcadPath,SW_SHOW);
	::DeleteFile(file);
}
