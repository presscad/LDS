#include "StdAfx.h"
#include "Tower.h"
#include "ClonePlate.h"
#include "SortFunc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#ifdef __COMMON_PART_INC_

void ANGLE_MAPPING::InitSrcAngle(CLDSLineAngle* pSrcAngle,char _ciSrcRayOrgTipS0E1,GECS& acsSrc)
{
	ciSrcRayOrgTipS0E1 =_ciSrcRayOrgTipS0E1;
	GEPOINT vxWingNorm=acsSrc.TransVToCS(pSrcAngle->vxWingNorm);
	GEPOINT vyWingNorm=acsSrc.TransVToCS(pSrcAngle->vyWingNorm);
	if(fabs(vxWingNorm.z)>=fabs(vyWingNorm.z))
		xSrcLayout=vxWingNorm.z>0?'x':'X';	//X֫����/����
	else //if(fabs(vxWingNorm.z)<=fabs(vyWingNorm.z))
		xSrcLayout=vyWingNorm.z>0?'y':'Y';	//Y֫����/����
}
bool ANGLE_MAPPING::Match(CLDSLineAngle* pDestAngle,char ciDestRayOrgTipS0E1,GECS& lcs)
{
	ANGLE_LAYOUT xDestLayout;
	GEPOINT vxWingNorm=lcs.TransVToCS(pDestAngle->vxWingNorm);
	GEPOINT vyWingNorm=lcs.TransVToCS(pDestAngle->vyWingNorm);
	if(fabs(vxWingNorm.z)>=fabs(vyWingNorm.z))
		xDestLayout=vxWingNorm.z>0?'x':'X';	//X֫����/����
	else //if(fabs(vxWingNorm.z)<=fabs(vyWingNorm.z))
		xDestLayout=vyWingNorm.z>0?'y':'Y';	//Y֫����/����
	if(toupper(xSrcLayout.ciAngleWorkWing)==toupper(xDestLayout.ciAngleWorkWing))
	{
		if(xSrcLayout.ciAngleWorkWing==xDestLayout.ciAngleWorkWing)
		{
			ciLegX2DestLeg='X';
			ciLegY2DestLeg='Y';
		}
		else
		{
			ciLegX2DestLeg='x';
			ciLegY2DestLeg='y';
		}
	}
	else
	{
		bool blSrcInsideAngle =(xSrcLayout.ciAngleWorkWing=='x'||xSrcLayout.ciAngleWorkWing=='y');
		bool blDestInsideAngle=(xDestLayout.ciAngleWorkWing=='x'||xDestLayout.ciAngleWorkWing=='y');
		if(blSrcInsideAngle==blDestInsideAngle)
		{	//ͬ����
			if(xSrcLayout.ciAngleWorkWing=='x'||xSrcLayout.ciAngleWorkWing=='X')
			{	//Դ�Ǹ�X֫Ϊ����֫��Ŀ��Ǹ�Y֫Ϊ����֫
				ciLegX2DestLeg='Y';
				ciLegY2DestLeg='x';
			}
			else
			{	//Դ�Ǹ�Y֫Ϊ����֫��Ŀ��Ǹ�X֫Ϊ����֫
				ciLegX2DestLeg='y';
				ciLegY2DestLeg='X';
			}
		}
		else
		{	//���ⷴ��
			if(xSrcLayout.ciAngleWorkWing=='x'||xSrcLayout.ciAngleWorkWing=='X')
			{	//Դ�Ǹ�X֫Ϊ����֫��Ŀ��Ǹ�Y֫Ϊ����֫
				ciLegX2DestLeg='y';
				ciLegY2DestLeg='X';
			}
			else
			{	//Դ�Ǹ�Y֫Ϊ����֫��Ŀ��Ǹ�X֫Ϊ����֫
				ciLegX2DestLeg='Y';
				ciLegY2DestLeg='x';
			}
		}
	}
	return true;
}
bool ANGLE_MAPPING::Mapping(CLDSLineAngle* pDestAngle,char ciDestRayOrgTipS0E1,GECS& acsMapTo)
{
	//char ciDestRayOrgTipS0E1=_ciDestRayOrgTipS0E1;
	bool blReverseTipSnE=(ciSrcRayOrgTipS0E1!=ciDestRayOrgTipS0E1);
	if(!Match(pDestAngle,ciDestRayOrgTipS0E1,acsMapTo))
		return false;
	return true;
}
typedef CSchemaRod* CSchemaRodPtr;
//////////////////////////////////////////////////////////////////////////
//��̬��������
static int compare_pole(const CSchemaRodPtr &pLinkPole1,const CSchemaRodPtr &pLinkPole2)
{
	if(pLinkPole1->offset_angle==pLinkPole2->offset_angle)
		return 0;
	else if(pLinkPole1->offset_angle>pLinkPole2->offset_angle)
		return 1;
	else
		return -1;
}
static void CopyInfoFromDesignLjRod(CDesignLjPartPara* pSrcLjRod,CSchemaRod* pToSchemaRod)
{
	if(pSrcLjRod==NULL || pToSchemaRod==NULL)
		return;
	pToSchemaRod->angle.bDualEdge=pSrcLjRod->angle.bTwoEdge;			//�Ƿ���˨�����඼����������
	pToSchemaRod->angle.bEndLjAngle=pSrcLjRod->angle.bEndLjJg;			//�����ӽǸ�
	pToSchemaRod->angle.cbSpaceFlag=pSrcLjRod->angle.cbSpaceFlag;
	pToSchemaRod->angle.ciWorkWingX0Y1=pSrcLjRod->angle.cur_wing_x0_y1;	//��ǰ����֫
	pToSchemaRod->m_idClassType=pSrcLjRod->m_nClassTypeId;
	if(pSrcLjRod->start0_end1==0&&pSrcLjRod->pCurLinePart)
	{//hOddRefRod1��2����Ҫ�ں���������ת��ΪCClonePlate�е�Idֵ
		pToSchemaRod->odd.ciOddCalStyle=pSrcLjRod->pCurLinePart->desStartOdd.m_iOddCalStyle;
		pToSchemaRod->odd.fCollideDist=pSrcLjRod->pCurLinePart->desStartOdd.m_fCollideDist;
		pToSchemaRod->odd.hOddRefRod1=pSrcLjRod->pCurLinePart->desStartOdd.m_hRefPart1;	//����ͷУ����չ���
		pToSchemaRod->odd.hOddRefRod2=pSrcLjRod->pCurLinePart->desStartOdd.m_hRefPart2;	//����ͷУ����չ���
	}
	else if(pSrcLjRod->start0_end1==1&&pSrcLjRod->pCurLinePart)
	{
		pToSchemaRod->odd.ciOddCalStyle=pSrcLjRod->pCurLinePart->desEndOdd.m_iOddCalStyle;
		pToSchemaRod->odd.fCollideDist=pSrcLjRod->pCurLinePart->desEndOdd.m_fCollideDist;
		pToSchemaRod->odd.hOddRefRod1=pSrcLjRod->pCurLinePart->desEndOdd.m_hRefPart1;	//����ͷУ����չ���
		pToSchemaRod->odd.hOddRefRod2=pSrcLjRod->pCurLinePart->desEndOdd.m_hRefPart2;	//����ͷУ����չ���
	}
	pToSchemaRod->ciConnectTipS0E1=(BYTE)pSrcLjRod->start0_end1;	//���ʼ��0���ն�1;
	pToSchemaRod->siPlaneNo=pSrcLjRod->iFaceNo;	//	����������
	//���������ʱ�Ĳ���
	pToSchemaRod->siEndSpace =ftoi(pSrcLjRod->end_space);
	pToSchemaRod->siWingSpace=ftoi(pSrcLjRod->wing_space);
	pToSchemaRod->siBerSpace =ftoi(pSrcLjRod->ber_space);
}
//////////////////////////////////////////////////////////////////////////
//CSchemaRod
//////////////////////////////////////////////////////////////////////////
CSchemaRod::CSchemaRod()
{
	angle.bEndLjAngle=false;
	memset(this,0,sizeof(CSchemaRod));
	blReverseTipSnE=false;
	ciConnectTipS0E1=2;
	ciLegX2DestLeg='X';
	ciLegY2DestLeg='Y';
	this->m_bInitSchemaInfo=m_bUpdateMatchRodInfo=false;
	this->siEndSpace=this->siWingSpace=this->siBerSpace=30;
	angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_BOLTEND);
	angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_BOLTEND);
	angle.cbSpaceFlag.SetEndSpaceStyle(ANGLE_SPACE_FLAG::SPACE_BOLTEND);
}
void CSchemaRod::InitSchemaAngle(CLDSLineAngle* pSrcAngle,GECS& scs)
{
	GEPOINT vxWingNorm=scs.TransVToCS(pSrcAngle->vxWingNorm);
	GEPOINT vyWingNorm=scs.TransVToCS(pSrcAngle->vyWingNorm);
	if(fabs(vxWingNorm.z)>=fabs(vyWingNorm.z))
		angle.xWingLayout=vxWingNorm.z>0?'x':'X';	//X֫����/����
	else //if(fabs(vxWingNorm.z)<=fabs(vyWingNorm.z))
		angle.xWingLayout=vyWingNorm.z>0?'y':'Y';	//Y֫����/����
	m_bInitSchemaInfo=true;
}
bool CSchemaRod::Match(CLDSLineAngle* pDestAngle,char ciDestConnTipS0E1,GECS& lcs)
{
	blReverseTipSnE=(ciConnectTipS0E1!=ciDestConnTipS0E1);	//�Ƿ����ʼĩ��
	ANGLE_LAYOUT xDestLayout;
	GEPOINT vxWingNorm=lcs.TransVToCS(pDestAngle->vxWingNorm);
	GEPOINT vyWingNorm=lcs.TransVToCS(pDestAngle->vyWingNorm);
	if(fabs(vxWingNorm.z)>=fabs(vyWingNorm.z))
		xDestLayout=vxWingNorm.z>0?'x':'X';	//X֫����/����
	else //if(fabs(vxWingNorm.z)<=fabs(vyWingNorm.z))
		xDestLayout=vyWingNorm.z>0?'y':'Y';	//Y֫����/����
	if(toupper(angle.xWingLayout.ciAngleWorkWing)==toupper(xDestLayout.ciAngleWorkWing))
	{
		if(angle.xWingLayout.ciAngleWorkWing==xDestLayout.ciAngleWorkWing)
		{
			ciLegX2DestLeg='X';
			ciLegY2DestLeg='Y';
		}
		else
		{
			ciLegX2DestLeg='x';
			ciLegY2DestLeg='y';
		}
	}
	else
	{
		bool blSrcInsideAngle =(angle.xWingLayout.ciAngleWorkWing=='x'||angle.xWingLayout.ciAngleWorkWing=='y');
		bool blDestInsideAngle=(xDestLayout.ciAngleWorkWing=='x'||xDestLayout.ciAngleWorkWing=='y');
		if(blSrcInsideAngle==blDestInsideAngle)
		{	//ͬ����
			if(angle.xWingLayout.ciAngleWorkWing=='x'||angle.xWingLayout.ciAngleWorkWing=='X')
			{	//Դ�Ǹ�X֫Ϊ����֫��Ŀ��Ǹ�Y֫Ϊ����֫
				ciLegX2DestLeg='Y';
				ciLegY2DestLeg='x';
			}
			else
			{	//Դ�Ǹ�Y֫Ϊ����֫��Ŀ��Ǹ�X֫Ϊ����֫
				ciLegX2DestLeg='y';
				ciLegY2DestLeg='X';
			}
		}
		else
		{	//���ⷴ��
			if(angle.xWingLayout.ciAngleWorkWing=='x'||angle.xWingLayout.ciAngleWorkWing=='X')
			{	//Դ�Ǹ�X֫Ϊ����֫��Ŀ��Ǹ�Y֫Ϊ����֫
				ciLegX2DestLeg='y';
				ciLegY2DestLeg='X';
			}
			else
			{	//Դ�Ǹ�Y֫Ϊ����֫��Ŀ��Ǹ�X֫Ϊ����֫
				ciLegX2DestLeg='Y';
				ciLegY2DestLeg='x';
			}
		}
	}
	m_bUpdateMatchRodInfo=true;
	return true;
}
void CSchemaRod::CopyProperty(CSchemaRod* pScrSchemaRod)
{
	vec_type=pScrSchemaRod->vec_type;
	offset_angle=pScrSchemaRod->offset_angle;
	hLinePart=pScrSchemaRod->hLinePart;
	pLinePart=pScrSchemaRod->pLinePart;
	m_idClassType=pScrSchemaRod->m_idClassType;
	ciConnectTipS0E1=pScrSchemaRod->ciConnectTipS0E1;
	siPlaneNo=pScrSchemaRod->siPlaneNo;
	siBerSpace=pScrSchemaRod->siBerSpace;
	siEndSpace=pScrSchemaRod->siEndSpace;
	siWingSpace=pScrSchemaRod->siWingSpace;
	angle.bDualEdge=pScrSchemaRod->angle.bDualEdge;
	angle.bEndLjAngle=pScrSchemaRod->angle.bEndLjAngle;
	angle.cbSpaceFlag=pScrSchemaRod->angle.cbSpaceFlag;
	angle.ciSonAngleQuad=pScrSchemaRod->angle.ciSonAngleQuad;
	angle.ciWorkWingX0Y1=pScrSchemaRod->angle.ciWorkWingX0Y1;
	angle.xWingLayout=pScrSchemaRod->angle.xWingLayout;
	this->m_bInitSchemaInfo=pScrSchemaRod->blInitSchemaInfo;
	this->m_bUpdateMatchRodInfo=pScrSchemaRod->blUpdateMatchRodInfo;
	this->blReverseTipSnE=pScrSchemaRod->blReverseTipSnE;
	this->ciLegX2DestLeg=pScrSchemaRod->ciLegX2DestLeg;
	this->ciLegY2DestLeg=pScrSchemaRod->ciLegY2DestLeg;
	odd.ciOddCalStyle=pScrSchemaRod->odd.ciOddCalStyle;
	odd.fCollideDist=pScrSchemaRod->odd.fCollideDist;
	odd.hOddRefRod1=pScrSchemaRod->odd.hOddRefRod1;
	odd.hOddRefRod2=pScrSchemaRod->odd.hOddRefRod2;
}
//////////////////////////////////////////////////////////////////////////
//CSchemaBolt
//////////////////////////////////////////////////////////////////////////
CSchemaBolt::CSchemaBolt()
{
	memset(this,0,sizeof(CSchemaBolt));
}
void CSchemaBolt::InitThrough(const char* thickStr)
{
	CXhChar50 keystr(thickStr);
	int ciIndex=0;
	for(char* key=strtok(keystr,", ");key!=NULL;key=strtok(NULL,", "))
	{
		if((key[0]=='1'||key[0]=='2')&&key[1]=='_')
		{
			CXhChar16 str(key);
			str.Replace('_',' ');
			int iType=0,iValue=0;
			sscanf(str,"%d %d",&iType,&iValue);
			through.items[ciIndex].ciItemType=iType;
			through.items[ciIndex].ciThickVal=iValue;
		}
		ciIndex++;
	}
}
char* CSchemaBolt::PARATHICK::ToThickStr()
{
	CXhChar50 thick_str;
	for(BYTE i=0;i<6;i++)
	{
		if(items[i].ciItemType==0)
			break;
		else if(items[i].ciItemType==1)
		{	//���׹���ID
			if(thick_str.GetLength()>0)
				thick_str.Append(',');
			thick_str.Append(CXhChar16("1_%d",items[i].ciThickVal));
		}
		else if(items[i].ciItemType==2)
		{	//ָ�����ֵ
			if(thick_str.GetLength()>0)
				thick_str.Append(',');
			thick_str.Append(CXhChar16("2_%d",items[i].ciThickVal));
		}
	}
	return (char*)thick_str;
}
//////////////////////////////////////////////////////////////////////////
// CClonePlate��ش�����������Plate.cpp wjh-2017.8.7
//////////////////////////////////////////////////////////////////////////
//��¡�ְ� 
CClonePlate::CClonePlate()
{
	m_iMatchResult=0;
	m_pSrcPlate=NULL;
	m_pTower=NULL;
	m_nPlaneN=1;
	ciDatumAxis='Y';
	ciDirectionAxisY=ciDirectionAxisZ=0;
	m_idDatumRod=1;
	m_bFlexibleDesign=TRUE;
	m_ciProfileSty=1;
}

CClonePlate::CClonePlate(IModel *pTower, CLDSPlate *pPlate/*=NULL*/,CLDSNode *pNode/*=NULL*/)
{
	m_iMatchResult=0;
	m_pSrcPlate=pPlate;
	m_pTower=pTower;
}

CClonePlate::~CClonePlate()
{

}

void CClonePlate::SetSuperiorTower(IModel *pTower)
{
	if(pTower)
		m_pTower=pTower;
}
CSchemaRod* CClonePlate::AppendSchemaRod(long hLinkRod,DWORD keyid/*=0*/)
{
	CSchemaRod* pSchemaRod=hashLinkRodsByH.GetValue(hLinkRod);
	if(pSchemaRod)
		return pSchemaRod;
	else
	{
		pSchemaRod=hashLinkRods.Add(keyid);
		pSchemaRod->hLinePart=hLinkRod;
		hashLinkRodsByH.SetValue(hLinkRod,pSchemaRod);
		return pSchemaRod;
	}
}
bool CClonePlate::RemoveSchemaRod(CSchemaRod* pSchemaRod)
{
	hashLinkRodsByH.DeleteNode(pSchemaRod->hLinePart);
	return hashLinkRods.DeleteNode(pSchemaRod->Id)!=FALSE;
}

//�����ְ��øְ����Ӹ˼��б�
void CClonePlate::EmptyLinkRods()
{
	hashLinkRods.Empty();
	hashLinkRodsByH.Empty();
}
void CClonePlate::AnalyzePlateInfo(CLDSPlate *pPlate,IModel *pTower/*=NULL*/)
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	SetSuperiorTower(pTower);
	if(pPlate)
		m_pSrcPlate=pPlate;
	if(m_pSrcPlate==NULL||m_pTower==NULL)
		return;
	CLDSLinePart *pDatumLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(m_pSrcPlate->designInfo.m_hBasePart,CLS_LINEPART);
	if(pDatumLinePart==NULL||pDatumLinePart->pStart==NULL||pDatumLinePart->pEnd==NULL)
		return;
	GECS acs;
	f3dPoint datum_line_vec=pDatumLinePart->pEnd->Position(true)-pDatumLinePart->pStart->Position(true);
	normalize(datum_line_vec);
	f3dPoint datum_wing_vec,work_plane_norm=m_pSrcPlate->ucs.axis_z;
	if(pDatumLinePart->IsAngle())
	{
		CLDSLineAngle* pDatumJg=(CLDSLineAngle*)pDatumLinePart;
		CDesignLjPartPara *pLjPara=m_pSrcPlate->designInfo.FromHandle(pDatumJg->handle);
		if(pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)	//��׼�˼�Ϊ��ϽǸ�
			pLjPara=m_pSrcPlate->designInfo.FromHandle(((CLDSGroupLineAngle*)pDatumJg)->m_hDatumSonAngle);
		if(pLjPara)
		{
			datum_wing_vec=pLjPara->angle.cur_wing_x0_y1==0?pDatumJg->GetWingVecX():pDatumJg->GetWingVecY();
			work_plane_norm=pLjPara->angle.cur_wing_x0_y1==0?pDatumJg->vxWingNorm:pDatumJg->vyWingNorm;
		}
		//��ʼ���������ϵ
		acs.axis_z=work_plane_norm;
		acs.axis_y=this->ciDirectionAxisY==0?datum_line_vec:-datum_line_vec;
		acs.axis_x=acs.axis_y^acs.axis_z;
	}
	//
	EmptyLinkRods();
	hashLinkRods.SetMinmalId(2);	//1�̶�Ϊ�ְ�Ķ�λ��׼�Ǹ�
	int nPoleNum=0,nCount=m_pSrcPlate->designInfo.partList.GetNodeNum();
	DYN_ARRAY<CSchemaRod*> linkPoleArr(nCount);
	CXhSimpleList<CSchemaRod> linkList;
	//��¼��һ�����߸˼������췽���������������߸˼��ֵ���׼�˼���������
	//��׼�˼�Ϊ�Ǹֻ�����ϽǸ�ʱ����׼�˼����ൽ��׼֫���췽��һ��
	//��׼�˼�Ϊ�ֹ�ʱ������׼�˼��ֵ��κ�һ�����vec_type=0; 
	BOOL bFirstRay=TRUE;
	GEPOINT vLeftDirect=-m_pSrcPlate->ucs.axis_x;
	for(CDesignLjPartPara* pLjPara=m_pSrcPlate->GetFirstLjPara();pLjPara;pLjPara=m_pSrcPlate->GetNextLjPara())
	{
		CLDSLinePart *pCurLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pLjPara->hPart,CLS_LINEPART);
		if(pCurLinePart==NULL||pCurLinePart->pStart==NULL||pCurLinePart->pEnd==NULL)
			continue;
		pLjPara->pCurLinePart=pCurLinePart;
		pLjPara->m_nClassTypeId=pCurLinePart->GetClassTypeId();
		f3dPoint cur_line_vec=pCurLinePart->pEnd->Position(true)-pCurLinePart->pStart->Position(true);
		normalize(cur_line_vec);
		if(pLjPara->start0_end1==1)
			cur_line_vec*=-1.0;	
		if(bFirstRay&&pDatumLinePart->handle!=pCurLinePart->handle)
		{	//��¼��һ�����߸˼����췽�� 
			if(vLeftDirect*cur_line_vec<0)
				vLeftDirect*=-1.0;	//Ĭ���Ե�һ�����߸˼���Ϊ����ࣨX�����ࣩ
			bFirstRay=FALSE;
		}
		CSchemaRod* pLinkRod=linkList.AttachObject();
		pLinkRod->ciConnectTipS0E1=2;
		if(pCurLinePart->pStart->handle==m_pSrcPlate->designInfo.m_hBaseNode)
			pLinkRod->ciConnectTipS0E1=0;
		else if(pCurLinePart->pEnd->handle==m_pSrcPlate->designInfo.m_hBaseNode)
			pLinkRod->ciConnectTipS0E1=1;
		if(pCurLinePart->IsAngle())
			pLinkRod->InitSchemaAngle((CLDSLineAngle*)pCurLinePart,acs);

		CopyInfoFromDesignLjRod(pLjPara,pLinkRod);
		if(pDatumLinePart->IsAngle())
		{	//��׼�˼�Ϊ�Ǹֻ��߸ֹ�	
			if(pCurLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{	//���߸˼��ڻ�׼֫�����췽��һ��Ϊ1������Ϊ-1
				CLDSLineAngle *pCurAngle=(CLDSLineAngle*)pCurLinePart;
				if(pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE&&pCurAngle->group_father_jg_h==pDatumLinePart->handle)
					pLinkRod->vec_type=1;	//��׼�Ǹ�Ϊ��ϽǸ�
				else if(pCurLinePart->handle==pDatumLinePart->handle)
					pLinkRod->vec_type=1;	//��׼�˼����ൽ�Ǹֵ�ǰ����֫���췽��һ��ĸ˼���
				else if(!datum_wing_vec.IsZero()&&cur_line_vec*datum_wing_vec>0)
					pLinkRod->vec_type=1;	
				else if(!datum_wing_vec.IsZero()&&cur_line_vec*datum_wing_vec<0)
					pLinkRod->vec_type=-1;
				//���߸˼�Ϊ��׼��ϽǸֵ��ӽǸ�ʱ����ʼ���ӽǸ���������
				if(pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE&&pCurAngle->group_father_jg_h==pDatumLinePart->handle)
					pLinkRod->angle.ciSonAngleQuad=JustifySonAngleResideQuad(acs,pCurAngle);
			}	
		}
		else if(pDatumLinePart->GetClassTypeId()==CLS_LINETUBE)
		{	//��׼�˼�Ϊ�ֹ�	
			//��׼�˼�����Ϊһ�� ��Ϊ���ܷ����ڵ�ʱ���ܴ���������˼������������Ͳ鿴�Ƿ�ƥ�� �޹̶�������
			if(pCurLinePart->handle==pDatumLinePart->handle)
				pLinkRod->vec_type=0;	 
			else 
			{
				if(!vLeftDirect.IsZero()&&vLeftDirect*cur_line_vec>0)
					pLinkRod->vec_type=1;
				else if(!vLeftDirect.IsZero()&&vLeftDirect*cur_line_vec<0)
					pLinkRod->vec_type=-1;
				else 
					pLinkRod->vec_type=0;
			}
		}
		pLinkRod->hLinePart=pLjPara->hPart;
		pLinkRod->pLinePart=pCurLinePart;
		pLinkRod->offset_angle=fabs(cur_line_vec*datum_line_vec);
		linkPoleArr[nPoleNum]=pLinkRod;
		nPoleNum++;
	}
	CQuickSort<CSchemaRod*>::QuickSort(linkPoleArr,nPoleNum,compare_pole);
	for(int i=0;i<nPoleNum;i++)
	{
		DWORD keyid=(linkPoleArr[i]->pLinePart==pDatumLinePart)?1:0;
		if(keyid==0&&hashLinkRods.GetValue(1)==NULL&&linkPoleArr[i]->pLinePart->IsAngle()&&
			pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//����ְ����ƻ�׼�˼�Ϊ��ϽǸ�ʱ�����ְ����Ӹ˼��е��ӽǸ���Ϊ���׼�˼�
			CLDSLineAngle* pLineAngle=(CLDSLineAngle*)linkPoleArr[i]->pLinePart;
			if(pLineAngle->group_father_jg_h==pDatumLinePart->handle)
				keyid=1;
		}
		CSchemaRod* pLinkPoleInfo=AppendSchemaRod(linkPoleArr[i]->hLinePart,keyid);
		pLinkPoleInfo->CopyProperty(linkPoleArr[i]);
		pLinkPoleInfo->hMatchPole=0;
		pLinkPoleInfo->pMatchLinePart=NULL;
	}
	CSchemaRod* pSchemaRod,*pTemporySchemaRod=NULL;
	for(pSchemaRod=hashLinkRods.GetFirst();pSchemaRod;pSchemaRod=hashLinkRods.GetNext())
	{
		if((pTemporySchemaRod=hashLinkRodsByH.GetValue(pSchemaRod->odd.hOddRefRod1))!=NULL)
			pSchemaRod->odd.hOddRefRod1=pTemporySchemaRod->Id;
		else
			pSchemaRod->odd.hOddRefRod1=0;
		if((pTemporySchemaRod=hashLinkRodsByH.GetValue(pSchemaRod->odd.hOddRefRod2))!=NULL)
			pSchemaRod->odd.hOddRefRod2=pTemporySchemaRod->Id;
		else
			pSchemaRod->odd.hOddRefRod2=0;
	}
#endif
}

int CClonePlate::JustifySonAngleResideQuad(GECS acs,CLDSLineAngle* pAngle)
{
	char ciQuad=0;
	GEPOINT lenVec=(pAngle->End()-pAngle->Start()).normalized();
	double dd2y=lenVec*acs.axis_y,dd2x=lenVec*acs.axis_x;
	if(fabs(dd2y)>fabs(dd2x))
	{	//Y��Ϊ��׼
		if(dd2y<0)
		{
			double nx_x=pAngle->vxWingNorm*acs.axis_x;
			double nx_z=pAngle->vxWingNorm*acs.axis_z;
			double ny_x=pAngle->vyWingNorm*acs.axis_x;
			double ny_z=pAngle->vyWingNorm*acs.axis_z;
			if(nx_z<-EPS_COS2&&ny_x<-EPS_COS2)
				ciQuad=1;	//��һ����
			else if(nx_z>EPS_COS2&&ny_x>EPS_COS2)
				ciQuad=3;	//��������
			else if(nx_x>EPS_COS2&&ny_z<-EPS_COS2)
				ciQuad=2;
			else if(nx_x<-EPS_COS2&&ny_z>EPS_COS2)
				ciQuad=4;
		}
		else
		{
			double nx_x=pAngle->vxWingNorm*acs.axis_x;
			double nx_z=pAngle->vxWingNorm*acs.axis_z;
			double ny_x=pAngle->vyWingNorm*acs.axis_x;
			double ny_z=pAngle->vyWingNorm*acs.axis_z;
			if(ny_z<-EPS_COS2&&nx_x<-EPS_COS2)
				ciQuad=1;	//��һ����
			else if(ny_z>EPS_COS2&&nx_x>EPS_COS2)
				ciQuad=3;	//��������
			else if(ny_x>EPS_COS2&&nx_z<-EPS_COS2)
				ciQuad=2;
			else if(ny_x<-EPS_COS2&&nx_z>EPS_COS2)
				ciQuad=4;
		}
	}
	else
	{
		//TODO:�������ã���δ��� wjh-2017.8.5
	}
	return ciQuad;
}
CClonePlate::VECTORS CClonePlate::UpdateModelCSVectors(GECS acs)	//�ְ�ֲ�װ������ϵ
{
	VECTORS xModelVec;
	xModelVec.vDatumLenVec=acs.axis_y;//acs.TransVFromCS(xLocalVec.vDatumLenVec);
	xModelVec.vDatumPlaneNorm=acs.axis_z;//acs.TransVFromCS(xLocalVec.vDatumPlaneNorm);
	if(!xLocalVec.vBendPlaneNorm[0].IsZero())
		xModelVec.vBendPlaneNorm[0]=acs.TransVFromCS(xLocalVec.vBendPlaneNorm[0]);
	if(!xLocalVec.vBendPlaneNorm[1].IsZero())
		xModelVec.vBendPlaneNorm[1]=acs.TransVFromCS(xLocalVec.vBendPlaneNorm[1]);
	return xModelVec;
}
//�������Ӹ˼��б��Լ���ǰ�ڵ�ƥ��ְ�
int CClonePlate::IntelligentMatchPlate(CLDSNode *pNode,PARTSET& DispPartSet,IModel *pTower/*=NULL*/)
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	SetSuperiorTower(pTower);
	if(pNode==NULL||m_pTower==NULL)//||m_pSrcPlate==NULL)
		return 0;
	m_pDestNode=pNode;
	CSchemaRod* pSrcBaseSchemaRod=hashLinkRods.GetValue(1);
	CLDSLinePart *pDatumLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pNode->hFatherPart,CLS_LINEPART);
	if(pDatumLinePart==NULL||pSrcBaseSchemaRod==NULL)
		return 0;
	if(pDatumLinePart->pStart==NULL||pDatumLinePart->pEnd==NULL)
		return 0;
	if(pDatumLinePart->IsAngle()&&pSrcBaseSchemaRod->m_idClassType!=CLS_LINEANGLE)
		return 0;
	else if(!pDatumLinePart->IsAngle()&&pDatumLinePart->GetClassTypeId()!=pSrcBaseSchemaRod->m_idClassType)
		return 0;
	f3dPoint datum_line_vec=pDatumLinePart->pEnd->Position(true)-pDatumLinePart->pStart->Position(true);
	normalize(datum_line_vec);
	//���ڵ�ǰ��ʾ�����в��� ճ���ְ�ʱ������͸��ͼ�²���
	for(CLDSPart *pPart=DispPartSet.GetFirst();pPart;pPart=DispPartSet.GetNext())
	{
		if(!pPart->IsLinePart())
			continue;
		CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL||pLinePart==pDatumLinePart)
			continue;
		if(pLinePart->pStart->handle==pNode->handle||pLinePart->pEnd->handle==pNode->handle)
		{
			f3dPoint temp_line_vec=pLinePart->pEnd->Position(true)-pLinePart->pStart->Position(true);
			normalize(temp_line_vec);
			if(fabs(temp_line_vec*datum_line_vec)>EPS_COS)
				continue;	//�������߸˼�
			pLinePart->feature=0;	//��ʼ��feature
			partset.append(pLinePart);
		}
	}
	//ȷ���Ǹֵ�ǰ����֫
	GEPOINT vLeftDirect;
	f3dPoint ray_line_vec,work_norm=CLDSObject::_console->TransVFromScr(GEPOINT(0,0,-1));
	f3dPoint vCurrViewNormal=work_norm;	//Ĭ��Ϊ��ǰ������ͼ��Ļ���ߣ��������߸˼��ٽ���У��
	f3dPoint datum_wing_vec,datum_wing_norm;
	int work_wing_x0_y1=-1;
	CLDSLinePart *pRayLinePart=partset.GetFirst();
	if(pRayLinePart)
	{
		ray_line_vec=pRayLinePart->End()-pRayLinePart->Start();
		normalize(ray_line_vec);
		work_norm=ray_line_vec^datum_line_vec;
		normalize(work_norm);
		if(work_norm*vCurrViewNormal<0)
			work_norm*=-1.0;
	}
	GECS acs;	//��ǰ����Ƹְ��Ԥ������ϵ
	if(pDatumLinePart->IsAngle())
	{
		CLDSLineAngle *pDatumAngle=NULL;
		if(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE)
			pDatumAngle=(CLDSLineAngle*)pDatumLinePart;
		else	//��ϽǸ�
			pDatumAngle=(CLDSLineAngle*)m_pTower->FromPartHandle(((CLDSGroupLineAngle*)pDatumLinePart)->m_hDatumSonAngle,CLS_LINEANGLE);
		if(fabs(work_norm*pDatumAngle->vxWingNorm)>fabs(work_norm*pDatumAngle->vyWingNorm))
		{
			//pSrcBaseSchemaRod->angle.ciWorkWingX0Y1=0;	�������� wjh 2017.10.10
			acs.axis_z=this->ciDirectionAxisZ==0?pDatumAngle->vxWingNorm:-pDatumAngle->vxWingNorm;
			datum_wing_vec=pDatumAngle->GetWingVecX();
			datum_wing_norm=pDatumAngle->vxWingNorm;
		}
		else
		{
			//pSrcBaseSchemaRod->angle.ciWorkWingX0Y1=1;	�������� wjh 2017.10.10
			acs.axis_z=this->ciDirectionAxisZ==0?pDatumAngle->vyWingNorm:-pDatumAngle->vyWingNorm;
			datum_wing_vec=pDatumAngle->GetWingVecY();
			datum_wing_norm=pDatumAngle->vyWingNorm;
		}
	}
	if(this->ciDatumAxis!='X')
	{
		acs.axis_y=this->ciDirectionAxisY==0?datum_line_vec:-datum_line_vec;
		acs.axis_x=acs.axis_y^acs.axis_z;
	}
	else
	{	//���������ϴ��ڣ��������������
		acs.axis_x=this->ciDirectionAxisY==0?datum_line_vec:-datum_line_vec;
		acs.axis_y=acs.axis_z^acs.axis_x;
	}
	VECTORS xModelVec = this->UpdateModelCSVectors(acs);
	int nCurLeftNum=0,nCurRightNum=0;	//��׼�˼��������߸˼�����
	int	nCurLeftTubeNum=0,nCurRightTubeNum=0;	//��׼�˼�����ֹ�����
	//����׼�Ǹ���ӵ����߸˼�������
	CLDSGroupLineAngle *pGroupLineAngle=NULL;
	if(pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		pGroupLineAngle=(CLDSGroupLineAngle*)pDatumLinePart;
		for(int i=0;i<4;i++)
		{
			if(pGroupLineAngle->group_style<2&&i>=2)
				continue;	//�����ĽǸ�ת��Ϊ˫ƴ�Ǹ�ʱ����ѵ������ĸ��ӽǸ�ת��Ϊ��Ǹִ��� wht 11-08-05
			CLDSLineAngle *pSonLineAngle=NULL;
			if(pGroupLineAngle->son_jg_h[i]>0x20)
				pSonLineAngle=(CLDSLineAngle*)m_pTower->FromPartHandle(pGroupLineAngle->son_jg_h[i],CLS_LINEANGLE);
			if(pSonLineAngle)
			{
				pSonLineAngle->feature=1;	//��׼�˼�Ĭ��������֫���췽��һ��
				partset.append(pSonLineAngle);
				nCurLeftNum++;
			}
		}
	}
	else 
	{
		pDatumLinePart->feature=1;	//��׼�˼�Ĭ��������֫���췽��һ��
		partset.append(pDatumLinePart);
		if(pDatumLinePart->GetClassTypeId()!=CLS_LINETUBE)
			nCurLeftNum++;	//��׼�˼�Ϊ�ֹ�ʱ��׼�˼����������������е��κ�һ��
	}
	if(partset.GetNodeNum()!=hashLinkRods.GetNodeNum())
		return 0;	//�ڵ㴦�����Ӹ˼�������һ��
	//����ƥ�����߸˼���Ϣ
	BOOL bAngleMatch=TRUE,bPoleNumMatch=TRUE,bGuiGeMatch=TRUE;
	if(pRayLinePart)
	{	//ͳ�Ƶ�ǰ�ְ����ӵ����߸˼�
		for(CLDSLinePart* pLinePart=partset.GetFirst();pLinePart;pLinePart=partset.GetNext())
		{	
			if(pLinePart->feature!=0)
				continue;
			f3dPoint cur_line_vec=pLinePart->pEnd->Position(true)-pLinePart->pStart->Position(true);
			normalize(cur_line_vec);
			if(pLinePart->pEnd->handle==m_pDestNode->handle)
				cur_line_vec*=-1.0;
			if(pDatumLinePart->GetClassTypeId()==CLS_LINETUBE)
			{	//��׼�˼�Ϊ�ֹ�ʱ��Ҫͳ����������ֹ�����
				if(!vLeftDirect.IsZero()&&cur_line_vec*vLeftDirect>0)
				{
					pLinePart->feature=1;
					nCurLeftNum++;		//������߸˼�����
					if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
						nCurLeftTubeNum++;	//���ֹ�����
				}
				else if(!vLeftDirect.IsZero()&&cur_line_vec*vLeftDirect<0)
				{
					pLinePart->feature=-1;
					nCurRightNum++;		//�Ҳ����߸˼�����
					if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
						nCurRightTubeNum++;	//�Ҳ�ֹ�����
				}
			}
			else
			{
				if(!datum_wing_vec.IsZero()&&cur_line_vec*datum_wing_vec>0)
				{
					pLinePart->feature=1;
					nCurLeftNum++;
				}
				else if(!datum_wing_vec.IsZero()&&cur_line_vec*datum_wing_vec<0)
				{
					pLinePart->feature=-1;
					nCurRightNum++;
				}
			}
		}
		//ͳ��Դ�ְ����Ҳ�˼�����
		int nLeftNum=0,nRightNum=0;			//�����������߸˼�����
		int nLeftTubeNum=0,nRightTubeNum=0;	//��������ֹ�����
		for(CSchemaRod *pLinkPoleInfo=hashLinkRods.GetFirst();pLinkPoleInfo;pLinkPoleInfo=hashLinkRods.GetNext())
		{
			//if(pLinkPoleInfo->pLinePart==NULL)
			//	pLinkPoleInfo->pLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pLinkPoleInfo->hLinePart,CLS_LINEPART);
			//if(pLinkPoleInfo->pLinePart==NULL)
			//	continue;
			if(pLinkPoleInfo->vec_type==1)
			{
				nLeftNum++;
				if(pLinkPoleInfo->m_idClassType==CLS_LINETUBE)
					nLeftTubeNum++;
			}
			else if(pLinkPoleInfo->vec_type==-1)
			{
				nRightNum++;
				if(pLinkPoleInfo->m_idClassType==CLS_LINETUBE)
					nRightTubeNum++;
			}
		}
		//����ڵ�˼�����
		SortLinePartSet(pDatumLinePart);
		//��׼����Ϊ�ֹ�ʱ��������˼��������;���ͬ ��ƥ��˼�ʱ������������
		//��Ϊ��������޷���ȷ�������� ֻ�ܰ��Ƕ��Լ����ƥ��	wht 10-01-16
		BOOL bIgnore=FALSE;	//������������
		BOOL bReverseLeftRight=FALSE;	//��ת�˼����Ҳ��־λ
		if(pDatumLinePart->GetClassTypeId()==CLS_LINETUBE)
		{	
			//��������˼�����������ȵ����
			if(nCurLeftNum!=nLeftNum&&nCurLeftNum!=nRightNum		//���˼������������������ƥ��
				&&nCurRightNum!=nLeftNum&&nCurRightNum!=nRightNum)	//�Ҳ�˼������������������ƥ��
				bPoleNumMatch=FALSE;
			else
			{	//��֤��������ֹ�������һ������ȵ�
				if(nCurLeftTubeNum!=nLeftTubeNum&&nCurLeftTubeNum!=nRightTubeNum		//���ֹ������������������ƥ��
					&&nCurRightTubeNum!=nLeftTubeNum&&nCurRightTubeNum!=nRightTubeNum)	//�Ҳ�ֹ������������������ƥ��
					bPoleNumMatch=FALSE;	//���߸˼�������ƥ��
				else if(nCurLeftNum==nLeftNum&&nCurRightNum==nRightNum&&
					nCurLeftTubeNum==nLeftTubeNum&&nCurRightTubeNum==nRightTubeNum)	//��==�� ��==��
				{	//����������ȫƥ��
					if(nLeftNum==nRightNum&&nLeftTubeNum==nRightTubeNum)
						bIgnore=TRUE;	//ƥ��˼�ʱ������������
				}
				else if(nCurLeftNum==nLeftNum&&nCurRightNum==nRightNum
					&&nCurLeftTubeNum==nLeftTubeNum&&nCurRightTubeNum==nRightTubeNum)	//��==�� ��==��
				{	//��������ߵ�����ȫƥ��
					if(nLeftNum==nRightNum&&nLeftTubeNum==nRightTubeNum)
						bIgnore=TRUE;	//ƥ��˼�ʱ������������
					else
						bReverseLeftRight=TRUE;
				}
				else if((nCurLeftNum==nRightNum&&nCurLeftTubeNum==nRightTubeNum)
					||(nCurRightNum==nLeftNum&&nCurRightTubeNum==nLeftTubeNum))	//��==�һ���==��
				{
					bReverseLeftRight=TRUE;				
				}
				/*else if((nCurLeftNum==nLeftNum&&nCurLeftTubeNum==nLefTubeNum)
					||(nCurRightNum==nRightNum&&nCurRightTubeNum==nRightTubeNum))//��==�����==��
				{}*/
			}
			if(bPoleNumMatch&&bReverseLeftRight)
			{
				for(pLinePart=partset.GetFirst();pLinePart;pLinePart=partset.GetNext())
					pLinePart->feature*=-1;
			}
		}
		else if(nCurLeftNum!=nLeftNum||nCurRightNum!=nRightNum)
			bPoleNumMatch=FALSE;	//���߸˼�������ƥ��
		if(bPoleNumMatch)
		{	//�˼�����ƥ�����Ҫ����ƥ�����߸˼�
			for(pLinePart=partset.GetFirst();pLinePart;pLinePart=partset.GetNext())
			{
				if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
					continue;
				if(pLinePart->feature==0&&pLinePart->handle!=pDatumLinePart->handle)
					continue;	//��עλΪ0�ҷǻ�׼�˼�
				f3dPoint cur_line_vec=pLinePart->pEnd->Position(true)-pLinePart->pStart->Position(true);
				normalize(cur_line_vec);
				if(pLinePart->pEnd==this->m_pDestNode)
					cur_line_vec*=-1.0;
				//��ǰ�˼����׼�˼�ȷ���Ĺ����淨�߷���
				f3dPoint work_plane_norm=datum_wing_norm;
				if(pLinePart->handle!=pDatumLinePart->handle)
					work_plane_norm=datum_line_vec^cur_line_vec;
				normalize(work_plane_norm);
				//���ܳ��������߸˼����׼�˼��н���ͬ�����
				//��ʱӦ��һ���Ƚϸ˼����ͣ��˼����ѡ����ƥ���
				CSchemaRod *pPrevLinkPoleInfo=NULL;
				double offset_angle=fabs(cur_line_vec*datum_line_vec);
				for(pLinkPoleInfo=hashLinkRods.GetFirst();pLinkPoleInfo;pLinkPoleInfo=hashLinkRods.GetNext())
				{
					if(!bIgnore&&pLinkPoleInfo->Id!=m_idDatumRod&&pLinkPoleInfo->vec_type!=pLinePart->feature)
					{	//��׼�˼�������������
						pPrevLinkPoleInfo=pLinkPoleInfo;
						continue;	//λ�ڻ�׼�˼�����
					}
					if(pLinePart->GetClassTypeId()!=pLinkPoleInfo->m_idClassType)
						continue;	//�˼����Ͳ���ͬ
					if(this->m_nPlaneN>1&&!work_plane_norm.IsZero())
					{	//˫������������Ҵ��ں���Ĺ����淨�߷��� �����߸˼����������ƥ�� 
						f3dPoint src_work_plane_norm=xModelVec.vDatumPlaneNorm;
						if(pLinkPoleInfo->siPlaneNo==1||pLinkPoleInfo->siPlaneNo==2)
							src_work_plane_norm=xModelVec.vBendPlaneNorm[pLinkPoleInfo->siPlaneNo-1];
						double a=fabs(src_work_plane_norm*work_plane_norm);
						//if(fabs(src_work_plane_norm*work_plane_norm)<0.9)
						//	continue;	//����ͬһ��������
					}
					
					BOOL bIsSonJg=FALSE;	//��ǰ�˼��Ƿ�Ϊ��׼�˼��ӽǸ�
					if(pDatumLinePart->GetClassTypeId()==CLS_LINETUBE&&pLinePart->handle==pDatumLinePart->handle)
					{	//��׼�˼�Ϊ�ֹ�ʱ����ƥ���׼�˼� 
						if(pLinkPoleInfo->Id==1)//pCurLinkPart->handle==pSrcDatumLinePart->handle)
						{
							pLinkPoleInfo->hMatchPole=pLinePart->handle;
							pLinkPoleInfo->pMatchLinePart=pLinePart;
						}
						else
							continue;
					}
					else if(pGroupLineAngle&&pLinePart->GetClassTypeId()==CLS_LINEANGLE&&pLinkPoleInfo->m_idClassType==CLS_LINEANGLE)
					{	//��׼�˼�Ϊ��ϽǸ�ʱ�ڴ˴�ƥ����ϽǸ��ӽǸ�
						CLDSLineAngle *pCurJg=(CLDSLineAngle*)pLinePart;
						if(pCurJg->group_father_jg_h==pGroupLineAngle->handle)
						{
							bIsSonJg=TRUE;	//��ǰ�˼���׼�Ǹ��ӽǸ�
							BOOL bFinishMatch=FALSE;
							char ciQuad=JustifySonAngleResideQuad(acs,pCurJg);
							//for(int j=0;j<4;j++)
							//{
							//	if(pGroupLineAngle->group_style<2&&j>=2)
							//		continue;	//�����ĽǸ�ת��Ϊ˫ƴ�Ǹ�ʱ����ѵ������ĸ��ӽǸ�ת��Ϊ��Ǹִ��� wht 11-08-05
								if(ciQuad==pLinkPoleInfo->angle.ciSonAngleQuad)
								{
									pLinkPoleInfo->hMatchPole=pCurJg->handle;
									pLinkPoleInfo->pMatchLinePart=pLinePart;
									bFinishMatch=TRUE;
								}
								//if(pCurJg->handle==pGroupLineAngle->son_jg_h[j]
								//	&&pCurLinkPart->handle==pSrcGroupJg->son_jg_h[j])
								//{
								//	pLinkPoleInfo->hMatchPole=pGroupLineAngle->son_jg_h[j];
								//	pLinkPoleInfo->pMatchLinePart=pLinePart;
								//	bFinishMatch=TRUE;
								//}
							//}
							if(bFinishMatch)
								break;	//����linkPoleListѭ��
						}
					}
					//�ǻ�׼����(��ϽǸ�)�ӽǸ���ƥ��˼�Ϊ��  wht 10-03-08
					if(!bIsSonJg&&pLinkPoleInfo->hMatchPole<0x20&&pLinkPoleInfo->pMatchLinePart==NULL)
					{
						if(pPrevLinkPoleInfo&&pPrevLinkPoleInfo->vec_type==pLinkPoleInfo->vec_type
							&&fabs(pPrevLinkPoleInfo->offset_angle-pLinkPoleInfo->offset_angle)<EPS2)
						{	//���������Ƕ���ͬ�ĸ˼� ����ƥ������һ�����ŵ�
							CLDSLinePart *pSameLinePart=pPrevLinkPoleInfo->pLinePart;
							if(pLinkPoleInfo->m_idClassType!=pSameLinePart->GetClassTypeId())
							{	//�Ƕ���ͬ����������Ϣ�����Ӹ˼����Ͳ�ͬ
								if(pSameLinePart->GetClassTypeId()==pLinePart->GetClassTypeId())
								{
									pLinkPoleInfo->hMatchPole=pPrevLinkPoleInfo->hMatchPole;
									pLinkPoleInfo->pMatchLinePart=pPrevLinkPoleInfo->pMatchLinePart;
									pPrevLinkPoleInfo->hMatchPole=pLinePart->handle;
									pPrevLinkPoleInfo->pMatchLinePart=pLinePart;
									break;
								}
								else
								{
									pLinkPoleInfo->hMatchPole=pLinePart->handle;
									pLinkPoleInfo->pMatchLinePart=pLinePart;
									break;
								}
							}
							/*else if(m_pSrcPlate->face_N>1&&pCurLinkPart				//˫���������
								&&pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE	//��׼�˼�Ϊ�Ǹ�
								&&pLinePart->GetClassTypeId()==CLS_LINEANGLE		//���߸˼�Ϊ�Ǹ�
								&&pLinePart->GetClassTypeId()==pCurLinkPart->GetClassTypeId())	//wht 10-01-13
							{	//˫��������� ����������׼�˼��н���ͬ���������߸˼� 
								//Ӧ�������߽Ǹֵ�ǰ֫���߷������׼�Ǹ�����֫���ߵļнǽ���ƥ�� 
								
							}*/
							/*else if(pCurLinkPart&&pLinePart->GetClassTypeId()==pCurLinkPart->GetClassTypeId()
								&&fabs(pCurLinkPart->GetWidth()-pLinePart->GetWidth())<EPS
								&&fabs(pCurLinkPart->GetThick()-pLinePart->GetThick())<EPS)
							{	//��ǰ�˼��뵱ǰ���Ӹ˼���ȫƥ��
								pLinkPoleInfo->hMatchPole=pLinePart->handle;
								pLinkPoleInfo->pMatchLinePart=pLinePart;
								break;
							}*/
							else if(pSameLinePart&&pLinePart->GetClassTypeId()==pSameLinePart->GetClassTypeId()
								&&fabs(pSameLinePart->GetWidth()-pLinePart->GetWidth())<EPS
								&&fabs(pSameLinePart->GetThick()-pLinePart->GetThick())<EPS)
							{	//��ǰ�˼���ǰһ���Ƕ���ͬ��������Ϣ��ȫƥ��
								pLinkPoleInfo->hMatchPole=pPrevLinkPoleInfo->hMatchPole;
								pLinkPoleInfo->pMatchLinePart=pPrevLinkPoleInfo->pMatchLinePart;
								pPrevLinkPoleInfo->hMatchPole=pLinePart->handle;
								pPrevLinkPoleInfo->pMatchLinePart=pLinePart;
								break;
							}
							else 
							{	
								pLinkPoleInfo->hMatchPole=pLinePart->handle;
								pLinkPoleInfo->pMatchLinePart=pLinePart;
								if(fabs(offset_angle-pLinkPoleInfo->offset_angle)>EPS2)
									bAngleMatch=FALSE;
								break;
							}
						}
						else
						{	//����һ����ƥ��ĸ˼�
							pLinkPoleInfo->hMatchPole=pLinePart->handle;
							pLinkPoleInfo->pMatchLinePart=pLinePart;
							if(fabs(offset_angle-pLinkPoleInfo->offset_angle)>EPS2)
								bAngleMatch=FALSE;
							break;
						}
					}
					pPrevLinkPoleInfo=pLinkPoleInfo;
				}
			}
		}
	}
	else 
	{	//�����߸˼�
		if(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{	//��׼�˼�Ϊ�Ǹ�
			if(hashLinkRods.GetNodeNum()!=1)
				bPoleNumMatch=FALSE;	//�˼�������ƥ��
			else 
			{
				CSchemaRod *pLinkPoleInfo=hashLinkRods.GetFirst();
				if(pLinkPoleInfo&&pLinkPoleInfo->Id==1)//pLinkPoleInfo->hLinePart==pSrcDatumLinePart->handle)
				{
					pLinkPoleInfo->hMatchPole=pDatumLinePart->handle;
					pLinkPoleInfo->pMatchLinePart=pDatumLinePart;
				}
			}
		}
		else if(pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//��׼����Ϊ��ϽǸ�
			if((pGroupLineAngle->group_style==0||pGroupLineAngle->group_style==1)
				&&hashLinkRods.GetNodeNum()!=2)
				bPoleNumMatch=FALSE;
			else if(pGroupLineAngle->group_style==2&&hashLinkRods.GetNodeNum()!=4)
				bPoleNumMatch=FALSE;
			else 
			{
				for(CSchemaRod *pLinkPoleInfo=hashLinkRods.GetFirst();pLinkPoleInfo;pLinkPoleInfo=hashLinkRods.GetNext())
				{
					if(pLinkPoleInfo->pLinePart==NULL)
						pLinkPoleInfo->pLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pLinkPoleInfo->hLinePart,CLS_LINEPART);
					CLDSLinePart *pCurLinkPart=pLinkPoleInfo->pLinePart;
					if(pCurLinkPart==NULL)
						continue;
					for(int j=0;j<4;j++)
					{
						if(pGroupLineAngle->group_style<2&&j>=2)
							continue;	//�����ĽǸ�ת��Ϊ˫ƴ�Ǹ�ʱ����ѵ������ĸ��ӽǸ�ת��Ϊ��Ǹִ��� wht 11-08-05
						CLDSLineAngle *pSonJg=NULL;
						pSonJg=(CLDSLineAngle*)m_pTower->FromPartHandle(pGroupLineAngle->son_jg_h[j],CLS_LINEANGLE);
						char ciQuad=JustifySonAngleResideQuad(acs,pSonJg);
						if(ciQuad==pLinkPoleInfo->angle.ciSonAngleQuad)
						{
							pLinkPoleInfo->hMatchPole=pSonJg->handle;
							pLinkPoleInfo->pMatchLinePart=pSonJg;
							break;//bFinishMatch=TRUE;
						}
						//if(pSonJg&&pCurLinkPart->handle==pSrcGroupJg->son_jg_h[j])
						//{
						//	pLinkPoleInfo->hMatchPole=pGroupLineAngle->son_jg_h[j];
						//	pLinkPoleInfo->pMatchLinePart=pSonJg;
						//	break;
						//}
					}
				}
			}	
		}
	}
	for(CSchemaRod *pLinkPoleInfo=hashLinkRods.GetFirst();pLinkPoleInfo;pLinkPoleInfo=hashLinkRods.GetNext())
	{
		if(pLinkPoleInfo->hMatchPole<0x20)
			bPoleNumMatch=FALSE;
		if(pLinkPoleInfo->pMatchLinePart==NULL)
			pLinkPoleInfo->pMatchLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pLinkPoleInfo->hMatchPole,CLS_LINEPART);
		if(pLinkPoleInfo->pMatchLinePart==NULL)
		{
			bAngleMatch=FALSE;
			bPoleNumMatch=FALSE;
			continue;
		}
		else if(pLinkPoleInfo->pMatchLinePart->IsAngle())
		{
			char ciConnTipS0E1=2;
			if(pLinkPoleInfo->pMatchLinePart->pStart==pNode)
				ciConnTipS0E1=0;
			else if(pLinkPoleInfo->pMatchLinePart->pEnd==pNode)
				ciConnTipS0E1=1;
			pLinkPoleInfo->Match((CLDSLineAngle*)pLinkPoleInfo->pMatchLinePart,ciConnTipS0E1,acs);
		}
		if(pLinkPoleInfo->pLinePart&&(fabs(pLinkPoleInfo->pLinePart->GetThick()-pLinkPoleInfo->pMatchLinePart->GetThick())>EPS_COS
			||fabs(pLinkPoleInfo->pLinePart->GetWidth()-pLinkPoleInfo->pMatchLinePart->GetWidth())>EPS_COS))
			bGuiGeMatch=FALSE;

	}
	if(bAngleMatch&&bGuiGeMatch&&bPoleNumMatch)
		m_iMatchResult=1;	//��ȫƥ��
	else if(bPoleNumMatch&&bGuiGeMatch)
		m_iMatchResult=2;	//���ƥ��
	else if(bPoleNumMatch)
		m_iMatchResult=3;	//����ƥ��,���ƥ��
	else
		m_iMatchResult=0;
#endif
	return m_iMatchResult;
}

bool CClonePlate::UpdateTipPosition(long hSchemaRelaRod,CLDSPlate* pNewPlate/*=NULL*/)
{
	CSchemaRod *pSchemaRod,*pDatumSchemaRod;
	pSchemaRod = QuerySchemaRod(hSchemaRelaRod,0);
	if(!pSchemaRod->blInitSchemaInfo||!pSchemaRod->blUpdateMatchRodInfo||!pSchemaRod->pLinePart->IsAngle()||
		pSchemaRod->pLinePart==NULL||pSchemaRod->pMatchLinePart==NULL||pSchemaRod->ciConnectTipS0E1>1)
		return false;
	CLDSLineAngle* pSrcAngle=(CLDSLineAngle*)pSchemaRod->pLinePart;
	CLDSLineAngle* pMapAngle=(CLDSLineAngle*)pSchemaRod->pMatchLinePart;
	
	CMultiOffsetPos *pSrcDesPosPara=(pSchemaRod->ciConnectTipS0E1==0)?&pSrcAngle->desStartPos:&pSrcAngle->desEndPos;
	BYTE ciDestConnTipS0E1=pSchemaRod->blReverseTipSnE?1-pSchemaRod->ciConnectTipS0E1:pSchemaRod->ciConnectTipS0E1;
	CMultiOffsetPos *pDesPosPara=(ciDestConnTipS0E1==0)?&pMapAngle->desStartPos:&pMapAngle->desEndPos;
	pDesPosPara->len_offset_type=pSrcDesPosPara->len_offset_type;
	pDesPosPara->len_offset_dist=pSrcDesPosPara->len_offset_dist;
	pDesPosPara->datum_offset_dist=pSrcDesPosPara->datum_offset_dist;
	pDesPosPara->huoqu_dist=pSrcDesPosPara->huoqu_dist;
	pDesPosPara->offset_zhun=pSrcDesPosPara->offset_zhun;
	if(pSrcDesPosPara->spatialOperationStyle>=1&&pSrcDesPosPara->spatialOperationStyle<=15)
	{	//��ƥ��������߸˼���Ӷ�λ���
		long hDatumRod=(pSchemaRod->ciConnectTipS0E1==0)?pSrcAngle->hStartDatumRod:pSrcAngle->hEndDatumRod;
		long hDestDatumRod=ciDestConnTipS0E1==0?pMapAngle->hStartDatumRod:pMapAngle->hEndDatumRod;
		pDatumSchemaRod=QuerySchemaRod(hDatumRod,0);
		if(pDatumSchemaRod==NULL||pDatumSchemaRod->pMatchLinePart->handle!=hDestDatumRod)
		{
			if(ciDestConnTipS0E1<2)
				logerr.Log("error");
			return false;
		}
		else
		{
			pDesPosPara->spatialOperationStyle=pSrcDesPosPara->spatialOperationStyle;
			if(pSrcDesPosPara->IsInDatumJgWingX()&&toupper(pDatumSchemaRod->ciLegX2DestLeg)=='Y')
			{
				pDesPosPara->cDatumAngleWing='X'+('Y'-pDesPosPara->cDatumAngleWing);	//����X/Y֫
				pDesPosPara->spatialOperationStyle+=1;	//���X֫��ɴ��Y֫
			}
			else if(pSrcDesPosPara->IsInDatumJgWingY()&&toupper(pDatumSchemaRod->ciLegX2DestLeg)=='Y')
			{
				pDesPosPara->cDatumAngleWing='X'+('Y'-pDesPosPara->cDatumAngleWing);	//����X/Y֫
				pDesPosPara->spatialOperationStyle-=1;	//���Y֫��ɴ��X֫
			}
		}
	}
	else
		return false;
	if(pSchemaRod->blReverseLegXnY)
	{
		pDesPosPara->wing_x_offset=pSrcDesPosPara->wing_y_offset;
		pDesPosPara->wing_y_offset=pSrcDesPosPara->wing_x_offset;
	}
	else
	{
		pDesPosPara->wing_x_offset=pSrcDesPosPara->wing_x_offset;
		pDesPosPara->wing_y_offset=pSrcDesPosPara->wing_y_offset;
	}
	BOOL ciDestWorkWingX0Y1=(pSchemaRod->blReverseLegXnY)?1-pSchemaRod->angle.ciWorkWingX0Y1:pSchemaRod->angle.ciWorkWingX0Y1;
	CDesignLjPartPara* pDesRodPara=(pNewPlate==NULL)?NULL:pNewPlate->designInfo.partList.GetValue(pMapAngle->handle);
	if(pDesRodPara)
	{
		pDesRodPara->angle.cur_wing_x0_y1=ciDestWorkWingX0Y1;
		WING_OFFSET_STYLE* pWorkLegOffset=(ciDestWorkWingX0Y1==0)?&pDesPosPara->wing_x_offset:&pDesPosPara->wing_y_offset;
		//�Զ������Ǹַ���ƫ����
		if(pDatumSchemaRod->pMatchLinePart->GetClassTypeId()==CLS_LINETUBE)
			pWorkLegOffset->offsetDist=-0.5*pNewPlate->GetThick();
		else
		{
			char ciDestWorkWing=pSchemaRod->angle.ciWorkWingX0Y1==0?pSchemaRod->ciLegX2DestLeg:pSchemaRod->ciLegY2DestLeg;
			if(pSchemaRod->angle.xWingLayout.ciAngleWorkWing>='x')	//ԭʼģ��Ϊ����
				ciDestWorkWing=(ciDestWorkWing>='x')?toupper(ciDestWorkWing):tolower(ciDestWorkWing);
			if(ciDestWorkWing>='x')	//����
				pWorkLegOffset->offsetDist=pDesRodPara->angle.bEndLjJg?-pDatumSchemaRod->pMatchLinePart->GetThick():0;
			else 
				pWorkLegOffset->offsetDist=-pNewPlate->GetThick();
		}
	}
	return true;
}
//���ݸְ�ƥ�������ɸְ�
CLDSPlate *CClonePlate::CreatePlate(PARTSET& DispPartSet,IModel *pTower/*=NULL*/)
{
	SetSuperiorTower(pTower);
	if(m_pTower==NULL||m_pDestNode==NULL||m_pSrcPlate==NULL)
		return NULL;
	int i=0;
	CLsRef* pLsRef=NULL;
	CLDSPlate *pPlate=NULL;
	long hSrcDatumNode=0,hSrcDatumPole=0;
	hSrcDatumNode=m_pSrcPlate->designInfo.m_hBaseNode;
	hSrcDatumPole=m_pSrcPlate->designInfo.m_hBasePart;
	CLDSLinePart *pDatumLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(m_pDestNode->hFatherPart,CLS_LINEPART);
	CLDSLinePart *pSrcLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(hSrcDatumPole,CLS_LINEPART);
	if(pDatumLinePart==NULL||pSrcLinePart==NULL)
		return NULL;	//��ѡ�ڵ��޸��˼����Ƶ�Դ�ְ��޻�׼����
	if(pSrcLinePart->GetClassTypeId()!=pDatumLinePart->GetClassTypeId())
		return NULL;	//Դ�ְ���Ҫ���ɵĸְ��׼�˼����Ͳ�ͬ
	for(pLsRef=m_pSrcPlate->GetFirstLsRef();pLsRef;pLsRef=m_pSrcPlate->GetNextLsRef())
		pLsRef->GetLsPtr()->feature=0;	//�����Ա����洢�����ʶ
	if(m_iMatchResult==1||m_iMatchResult==2||m_iMatchResult==3)
	{	//m_iMatchResult==1��������ͬ��ŵĸְ�
		pPlate=(CLDSPlate*)m_pTower->AppendPart(CLS_PLATE);
		long hNew=pPlate->handle;
		m_pSrcPlate->CloneTo(*pPlate);
		pPlate->relativeObjs.Empty();	//��չ����б�
		pPlate->EmptyLsRef();			//�����˨�����б�
		pPlate->handle=hNew;
		//���Ӹ�����
		if(pDatumLinePart->GetClassTypeId()==CLS_LINETUBE)
			pPlate->m_hPartWeldParent=pDatumLinePart->handle;
		//���¸ְ���Ʋ���
		LINKPOLEINFO *pLinkPoleInfo=NULL;
		pPlate->designInfo.ReplaceReferenceHandle(CHandleReplacer(m_pSrcPlate->designInfo.m_hBaseNode,m_pDestNode->handle));
		pPlate->designInfo.ReplaceReferenceHandle(CHandleReplacer(m_pSrcPlate->designInfo.m_hBasePart,m_pDestNode->hFatherPart));
		for(pLinkPoleInfo=hashLinkRods.GetFirst();pLinkPoleInfo;pLinkPoleInfo=hashLinkRods.GetNext())
			pPlate->designInfo.ReplaceReferenceHandle(CHandleReplacer(pLinkPoleInfo->hLinePart,pLinkPoleInfo->hMatchPole));
		//�����淨�߷���
		f3dPoint datum_line_vec,ray_line_vec,work_norm;
		datum_line_vec=pDatumLinePart->End()-pDatumLinePart->Start();
		normalize(datum_line_vec);
		CLDSLinePart *pRayLinePart=NULL;
		CDesignLjPartPara *pLjPartPara=NULL;
		pLjPartPara=pPlate->GetFirstLjPara();
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		if(pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//��׼�˼�Ϊ��ϽǸ�
			CLDSGroupLineAngle *pGroupLineAngle=(CLDSGroupLineAngle*)pDatumLinePart;
			while(pRayLinePart==NULL)
			{	//����һ�����߸˼���Ϊ��׼����ո˼�
				pLjPartPara=pPlate->GetNextLjPara();
				if(pLjPartPara)
					pRayLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pLjPartPara->hPart,CLS_LINEPART);
				else
					break;
				BOOL bIsSonJg=FALSE;
				for(i=0;i<4;i++)
				{
					if(pGroupLineAngle->group_style<2&&i>=2)
						continue;	//�����ĽǸ�ת��Ϊ˫ƴ�Ǹ�ʱ����ѵ������ĸ��ӽǸ�ת��Ϊ��Ǹִ��� wht 11-08-05
					if(pRayLinePart&&pRayLinePart->handle==pGroupLineAngle->son_jg_h[i])
					{
						bIsSonJg=TRUE;
						break;
					}
				}
				if(bIsSonJg)
					pRayLinePart=NULL;
			}	
		}
		else 
#endif
		{
			while(pRayLinePart==NULL||(pRayLinePart&&pRayLinePart==pDatumLinePart))
			{	//����һ�����߸˼���Ϊ��׼����ո˼�
				pLjPartPara=pPlate->GetNextLjPara();
				if(pLjPartPara)
					pRayLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pLjPartPara->hPart,CLS_LINEPART);
				else
					break;
			}
		}
		int work_wing_x0_y1=-1;
		if(pRayLinePart&&(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE
			||pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE))
		{
			ray_line_vec=pRayLinePart->End()-pRayLinePart->Start();
			normalize(ray_line_vec);
			work_norm=ray_line_vec^datum_line_vec;
			normalize(work_norm);
			CMultiOffsetPos *pDesPosPara=NULL;
			if(pRayLinePart&&pRayLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{	//���»�׼�Ǹ�����֫
				if(pRayLinePart&&pRayLinePart->pStart&&pRayLinePart->pStart->handle==m_pDestNode->handle)
					pDesPosPara=&((CLDSLineAngle*)pRayLinePart)->desStartPos;
				else if(pRayLinePart&&pRayLinePart->pEnd&&pRayLinePart->pEnd->handle==m_pDestNode->handle)
					pDesPosPara=&((CLDSLineAngle*)pRayLinePart)->desEndPos;
				if(pDesPosPara->IsInDatumJgWingX())
					work_wing_x0_y1=0;
				else if(pDesPosPara->IsInDatumJgWingY())
					work_wing_x0_y1=1;
			}
			if(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{
				CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)pDatumLinePart;
				if(work_wing_x0_y1==0)
				{	//X֫Ϊ��ǰ����֫
					if(work_norm*pDatumLineAngle->get_norm_x_wing()<0)
						work_norm*=-1;
				}
				else if(work_wing_x0_y1==1)
				{	//Y֫Ϊ��ǰ����֫
					if(work_norm*pDatumLineAngle->get_norm_y_wing()<0)
						work_norm*=-1;
				}
				else 
				{
					double ddxx=work_norm*pDatumLineAngle->get_norm_x_wing();
					double ddyy=work_norm*pDatumLineAngle->get_norm_y_wing();
					if(fabs(ddxx) > fabs(ddyy))
					{	//X֫Ϊ��ǰ����֫
						if(ddxx<0)
							work_norm*=-1;
					}
					else
					{	//Y֫Ϊ��ǰ����֫
						if(ddyy<0)
							work_norm*=-1;
					}
				}
			}
		}
		//else //�����߸˼�
		BOLTSET boltSet;	//��¼��Ҫ���ɵ���˨
		for(pLjPartPara=pPlate->GetFirstLjPara();pLjPartPara;pLjPartPara=pPlate->GetNextLjPara())
		{
			if(pLjPartPara->pCurLinePart==NULL)
				pLjPartPara->pCurLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pLjPartPara->hPart,CLS_LINEPART);
			if(pLjPartPara->pCurLinePart==NULL)
				continue;
			if(pLjPartPara->pCurLinePart->pStart==NULL||pLjPartPara->pCurLinePart->pEnd==NULL)
				continue;
			CMultiOffsetPos *pDesPosPara=NULL,*pSrcDesPosPara=NULL;
			DESIGNODDMENT *pDesignOdd=NULL,*pSrcDesignOdd=NULL;
			TUBEJOINT *pCurTubeJoint=NULL,*pSrcTubeJoint=NULL;
			CTubeEndPosPara *pCurTubePosPara=NULL,*pSrcTubePosPara=NULL;
			CLDSLinePart *pSrcPole=FindSrcPoleByMatchHandle(pLjPartPara->hPart);
			if(pLjPartPara->pCurLinePart!=pDatumLinePart)
			{
				if(pLjPartPara->pCurLinePart->pStart==m_pDestNode)
				{
					pDesignOdd=&((CLDSLineAngle*)pLjPartPara->pCurLinePart)->desStartOdd;
					if(pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINEANGLE)
						pDesPosPara=&((CLDSLineAngle*)pLjPartPara->pCurLinePart)->desStartPos; 
					else if(pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINETUBE)
					{
						pCurTubePosPara=&((CLDSLineTube*)pLjPartPara->pCurLinePart)->desStartPos;
						pCurTubeJoint=&((CLDSLineTube*)pLjPartPara->pCurLinePart)->m_tJointStart;
					}
					pLjPartPara->start0_end1=0;
					//�����˼�����ͷ
					if(pSrcPole&&pSrcPole->pStart&&pSrcPole->pStart->handle==m_pSrcPlate->designInfo.m_hBaseNode)
						pLjPartPara->pCurLinePart->SetStartOdd(pSrcPole->startOdd());
					else if(pSrcPole&&pSrcPole->pEnd&&pSrcPole->pEnd->handle==m_pSrcPlate->designInfo.m_hBaseNode)
						pLjPartPara->pCurLinePart->SetStartOdd(pSrcPole->endOdd());
				}
				else if(pLjPartPara->pCurLinePart->pEnd==m_pDestNode)
				{
					pDesignOdd=&((CLDSLineAngle*)pLjPartPara->pCurLinePart)->desEndOdd;
					if(pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINEANGLE)
						pDesPosPara=&((CLDSLineAngle*)pLjPartPara->pCurLinePart)->desEndPos; 
					else if(pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINETUBE)
					{
						pCurTubePosPara=&((CLDSLineTube*)pLjPartPara->pCurLinePart)->desEndPos;
						pCurTubeJoint=&((CLDSLineTube*)pLjPartPara->pCurLinePart)->m_tJointEnd;
					}
					pLjPartPara->start0_end1=1;
					//�����˼�����ͷ
					if(pSrcPole&&pSrcPole->pStart&&pSrcPole->pStart->handle==m_pSrcPlate->designInfo.m_hBaseNode)
						pLjPartPara->pCurLinePart->SetEndOdd(pSrcPole->startOdd());
					else if(pSrcPole&&pSrcPole->pEnd&&pSrcPole->pEnd->handle==m_pSrcPlate->designInfo.m_hBaseNode)
						pLjPartPara->pCurLinePart->SetEndOdd(pSrcPole->endOdd());
				}
				else
					pLjPartPara->start0_end1=2;
				if(pSrcPole&&pSrcPole->pStart->handle==m_pSrcPlate->designInfo.m_hBaseNode)
					pSrcDesignOdd=&pSrcPole->desStartOdd;
				else
					pSrcDesignOdd=&pSrcPole->desEndOdd;
				if(pSrcPole&&(pSrcPole->GetClassTypeId()==CLS_LINEANGLE||pSrcPole->GetClassTypeId()==CLS_GROUPLINEANGLE))
				{	//���߸ֹ�
					if(pSrcPole->pStart&&pSrcPole->pStart->handle==m_pSrcPlate->designInfo.m_hBaseNode)
						pSrcDesPosPara=&((CLDSLineAngle*)pSrcPole)->desStartPos;
					else if(pSrcPole->pEnd&&pSrcPole->pEnd->handle==m_pSrcPlate->designInfo.m_hBaseNode)
						pSrcDesPosPara=&((CLDSLineAngle*)pSrcPole)->desEndPos;
				}
				else if(pSrcPole&&pSrcPole->GetClassTypeId()==CLS_LINETUBE)
				{	//���߸ֹ�
					if(pSrcPole->pStart&&pSrcPole->pStart->handle==m_pSrcPlate->designInfo.m_hBaseNode)
					{
						pSrcTubeJoint=&((CLDSLineTube*)pSrcPole)->m_tJointStart;
						pSrcTubePosPara=&((CLDSLineTube*)pSrcPole)->desStartPos;
					}
					else if(pSrcPole->pEnd&&pSrcPole->pEnd->handle==m_pSrcPlate->designInfo.m_hBaseNode)
					{
						pSrcTubeJoint=&((CLDSLineTube*)pSrcPole)->m_tJointEnd;
						pSrcTubePosPara=&((CLDSLineTube*)pSrcPole)->desEndPos;
					}
				}
			}	
			double ddx=0,ddy=0;
			if(pLjPartPara->pCurLinePart&&pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{	//�������߸˼�,�������߸˼���ǰ����֫
				ddx=work_norm*((CLDSLineAngle*)pLjPartPara->pCurLinePart)->get_norm_x_wing();
				ddy=work_norm*((CLDSLineAngle*)pLjPartPara->pCurLinePart)->get_norm_y_wing();
				if(!UpdateTipPosition(pSrcPole->handle,pPlate))
				{
					if(pDesPosPara&&pSrcDesPosPara)
					{	//�������߸˼����㶨λ����,���⸴�Ƹְ��������ʼ�ն˶�λ���֫ wht 16-11-08
						//TODO:���ϴ��������ⶥ�������ڵ���壬����V������������ wjh-2019.3.28
						BYTE cOldOperStyle=pDesPosPara->spatialOperationStyle;
						*pDesPosPara=*pSrcDesPosPara;
						pDesPosPara->spatialOperationStyle=cOldOperStyle;
					}
					if(fabs(ddx)>fabs(ddy))
					{
						pLjPartPara->angle.cur_wing_x0_y1=0;
						if(pDesPosPara)
						{	//�Զ������Ǹַ���ƫ����
							pDesPosPara->wing_x_offset.gStyle=4;
							pDesPosPara->wing_y_offset.gStyle=0;
							if(pDatumLinePart->GetClassTypeId()==CLS_LINETUBE)
								pDesPosPara->wing_x_offset.offsetDist=-0.5*pPlate->GetThick();
							else
							{
								if(ddx>0)
									pDesPosPara->wing_x_offset.offsetDist=pLjPartPara->angle.bEndLjJg?-pDatumLinePart->GetThick():0;
								else 
									pDesPosPara->wing_x_offset.offsetDist=-pPlate->GetThick();
							}
						}	
					}
					else if(fabs(ddx)<fabs(ddy))
					{
						pLjPartPara->angle.cur_wing_x0_y1=1;
						if(pDesPosPara)
						{	//�Զ������Ǹַ���ƫ����
							pDesPosPara->wing_x_offset.gStyle=0;
							pDesPosPara->wing_y_offset.gStyle=4;
							if(pDatumLinePart->GetClassTypeId()==CLS_LINETUBE)
								pDesPosPara->wing_y_offset.offsetDist=-0.5*pPlate->GetThick();
							else
							{
								if(ddy>0)
									pDesPosPara->wing_y_offset.offsetDist=pLjPartPara->angle.bEndLjJg?-pDatumLinePart->GetThick():0;
								else
									pDesPosPara->wing_y_offset.offsetDist=-pPlate->GetThick();
							}
						}
					}
				}
				if(pDatumLinePart->GetClassTypeId()==CLS_LINETUBE&&pSrcPole)
				{	//ͳ����Ҫ�ڵ�ǰ�ְ������ɶ�Ӧ��˨��Դ��˨	wht 10-01-16
					for(pLsRef=pSrcPole->GetFirstLsRef();pLsRef;pLsRef=pSrcPole->GetNextLsRef())
					{
						if(m_pSrcPlate->FindLsByHandle(pLsRef->GetLsPtr()->handle))
							boltSet.append(pLsRef->GetLsPtr());
					}
				}
			}
			//Ӧ�ø��ݸֹܹ����Һ��ʵĲ������	
			else if(pCurTubeJoint&&pSrcTubeJoint&&pRayLinePart
				&&pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINETUBE)
			{	//���ڲ���򽫲���ϵ���˨���뵽�ְ��ϣ������¡�²���Լ���˨	wht 10-01-16
				CLDSParamPlate *pParamPlate=(CLDSParamPlate*)m_pTower->FromPartHandle(pCurTubeJoint->hLinkObj,CLS_PARAMPLATE);
				CLDSParamPlate *pSrcParamPlate=(CLDSParamPlate*)m_pTower->FromPartHandle(pSrcTubeJoint->hLinkObj,CLS_PARAMPLATE);
				if(pParamPlate==NULL&&pSrcParamPlate&&(pSrcParamPlate->m_iParamType==TYPE_ROLLEND||
					pSrcParamPlate->m_iParamType==TYPE_UEND||pSrcParamPlate->m_iParamType==TYPE_XEND))
				{	//���ɶ�Ӧ�Ĳ���Լ���˨  Ӧͬʱ����ʮ�ֲ����弰����
					pParamPlate=(CLDSParamPlate*)m_pTower->AppendPart(CLS_PARAMPLATE);
					long hNew=pParamPlate->handle;
					pSrcParamPlate->CloneTo(*pParamPlate);
					pParamPlate->relativeObjs.Empty();	//��չ����б�
					pParamPlate->EmptyLsRef();			//�����˨�����б�
					pParamPlate->handle=hNew;
					pParamPlate->m_hPartWeldParent=pLjPartPara->pCurLinePart->handle;
					pCurTubeJoint->hLinkObj=pParamPlate->handle;
					pCurTubeJoint->type=pSrcTubeJoint->type;
					//������Ʋ������ϵ
					if(pLjPartPara->start0_end1==0)
						((CLDSLineTube*)pLjPartPara->pCurLinePart)->DesignStartJointUCS(pParamPlate);
					else
						((CLDSLineTube*)pLjPartPara->pCurLinePart)->DesignEndJointUCS(pParamPlate);
					if(pSrcParamPlate->m_bStdPart)
					{	//��׼����ϵ���˨Ӧ���ݱ�׼��屾����˨���ò������ã����ܸ��ƣ�����Щ�����˨����ְ干�õģ���ʱӦ�����ظ�����
						for(pLsRef=pSrcParamPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcParamPlate->GetNextLsRef())
							pLsRef->GetLsPtr()->feature=1;	//�趨�����ʶ���Է������������˨����ʱ�ٴδ���
					}
					else
					{
						for(pLsRef=pSrcParamPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcParamPlate->GetNextLsRef())
						{	//���ɲ���ϵ���˨	
							CLDSBolt *pBolt=(CLDSBolt*)m_pTower->AppendPart(CLS_BOLT);
							hNew=pBolt->handle;
							(*pLsRef)->CloneTo(*pBolt);
							pBolt->handle=hNew;
							pParamPlate->AppendLsRef(pBolt->GetLsRef());
							//������˨��Ʋ���
							pBolt->ReplaceReferenceHandle(CHandleReplacer(pSrcParamPlate->handle,pParamPlate->handle));
							pBolt->correct_worknorm();	
							pBolt->correct_pos();
							pBolt->CalGuigeAuto();
						}
					}
				}
				//�����ֹܷ���ƫ����
				if(pSrcTubePosPara&&pCurTubePosPara&&pParamPlate&&pParamPlate->m_iParamType==TYPE_ROLLEND)
					pCurTubePosPara->fNormOffset=pSrcTubePosPara->fNormOffset;
				else if(pCurTubePosPara)
					pCurTubePosPara->fNormOffset=0;
				if(pParamPlate&&(pParamPlate->m_iParamType==TYPE_ROLLEND||
					pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_XEND))
				{	//����˨�����ְ���
					for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
						pPlate->AppendLsRef(*pLsRef);
				}
			}
			if(pLjPartPara->pCurLinePart==pDatumLinePart)
			{	//��׼�˼���������
				if(work_wing_x0_y1!=-1)
					pLjPartPara->angle.cur_wing_x0_y1=work_wing_x0_y1;
			}
			else if(pSrcDesignOdd!=NULL&&pDesignOdd!=NULL)
			{	//��������ͷ
				*pDesignOdd=*pSrcDesignOdd;
				if(pDesignOdd->m_hRefPart1==m_pSrcPlate->designInfo.m_hBasePart)
					pDesignOdd->m_hRefPart1=m_pDestNode->hFatherPart;
				if(pDesignOdd->m_hRefPart2==m_pSrcPlate->designInfo.m_hBasePart)
					pDesignOdd->m_hRefPart2=m_pDestNode->hFatherPart;
				for(pLinkPoleInfo=hashLinkRods.GetFirst();pLinkPoleInfo;pLinkPoleInfo=hashLinkRods.GetNext())
				{
					if(pDesignOdd->m_hRefPart1==pLinkPoleInfo->hLinePart)
						pDesignOdd->m_hRefPart1=pLinkPoleInfo->hMatchPole;
					if(pDesignOdd->m_hRefPart2==pLinkPoleInfo->hLinePart)
						pDesignOdd->m_hRefPart2=pLinkPoleInfo->hMatchPole;
				}
			}
		}
		//���¸ְ�λ����Ʋ����Լ�������Ʋ���
		pLjPartPara=pPlate->designInfo.FromHandle(pPlate->designInfo.norm.hVicePart);
		if(pLjPartPara)
			pPlate->designInfo.norm.norm_wing=pLjPartPara->angle.cur_wing_x0_y1;
		if(pPlate->designInfo.origin.datum_pos_style==1)	//�Ǹ��������Ϊ��׼
		{
			pLjPartPara=pPlate->designInfo.FromHandle(pPlate->designInfo.origin.des_para.RODEND.hRod);
			if(pLjPartPara)
			{
				pPlate->designInfo.origin.des_para.RODEND.offset_wing=pLjPartPara->angle.cur_wing_x0_y1;	
				pPlate->designInfo.origin.des_para.RODEND.direction=pLjPartPara->start0_end1; //�������췽��
			}
		}
		else if(pPlate->designInfo.origin.datum_pos_style==2)	//�˼��ϵĽڵ�Ϊ��׼
		{
			pLjPartPara=pPlate->designInfo.FromHandle(pPlate->designInfo.origin.des_para.RODNODE.hRod);
			if(pLjPartPara)
				pPlate->designInfo.origin.des_para.RODNODE.offset_wing=pLjPartPara->angle.cur_wing_x0_y1;
		}
		pPlate->DesignSetupUcs();	//������Ƹְ�����ϵ
		if(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE||pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//��׼�˼�Ϊ�Ǹֻ�����ϽǸ�������˨
			boltSet.Empty();
			for(pLsRef=m_pSrcPlate->GetFirstLsRef();pLsRef;pLsRef=m_pSrcPlate->GetNextLsRef())
				boltSet.append(pLsRef->GetLsPtr());
		}
		for(pLsRef=m_pSrcPlate->GetFirstLsRef();pLsRef;pLsRef=m_pSrcPlate->GetNextLsRef())
		{
			if(pLsRef->GetLsPtr()->feature==1)
				continue;	//��׼�������˨�ѽ��д����
			CLDSBolt *pBolt=(CLDSBolt*)m_pTower->AppendPart(CLS_BOLT);
			hNew=pBolt->handle;
			(*pLsRef)->CloneTo(*pBolt);
			pBolt->handle=hNew;
			pBolt->relativeObjs.Empty();	//�����˨�����б� wht 11-01-07
			//������˨��Ʋ���
			pBolt->ReplaceReferenceHandle(CHandleReplacer(m_pSrcPlate->handle,pPlate->handle));	//���ɸְ����滻Ϊ�¸ְ��� ��˨ͨ�����ƫ����
			pBolt->ReplaceReferenceHandle(CHandleReplacer(m_pSrcPlate->designInfo.m_hBaseNode,m_pDestNode->handle));
			pBolt->ReplaceReferenceHandle(CHandleReplacer(m_pSrcPlate->designInfo.m_hBasePart,m_pDestNode->hFatherPart));
			for(pLinkPoleInfo=hashLinkRods.GetFirst();pLinkPoleInfo;pLinkPoleInfo=hashLinkRods.GetNext())	
				pBolt->ReplaceReferenceHandle(CHandleReplacer(pLinkPoleInfo->hLinePart,pLinkPoleInfo->hMatchPole));
			//���������Ӧ�ϸ�������ͽ����޸ģ�ֻ�ܶԵ�ǰ�������õı��������޸ģ�
			//�����۸ĵ�ǰ����ʹ�ñ�������ֵ  wht 10-01-01
			if(pBolt->des_base_pos.datumPoint.datum_pos_style==2)	//�˼��ϵĽڵ�Ϊ��׼
			{
				pLjPartPara=pPlate->designInfo.FromHandle(pBolt->des_base_pos.datumPoint.des_para.RODNODE.hRod);
				if(pLjPartPara)
					pBolt->des_base_pos.datumPoint.des_para.RODNODE.offset_wing=pLjPartPara->angle.cur_wing_x0_y1;
			}
			else if(pBolt->des_base_pos.datumPoint.datum_pos_style==1)//�Ǹ��������Ϊ��׼
			{
				pLjPartPara=pPlate->designInfo.FromHandle(pBolt->des_base_pos.datumPoint.des_para.RODEND.hRod);
				if(pLjPartPara)
				{
					pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pLjPartPara->angle.cur_wing_x0_y1;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.direction=pLjPartPara->start0_end1; //�������췽��
					pBolt->des_base_pos.direction=(BYTE)pLjPartPara->start0_end1;
				}
			}
			pLjPartPara=pPlate->designInfo.FromHandle(pBolt->des_base_pos.hPart);
			if(pLjPartPara)
				pBolt->des_base_pos.offset_wing=(BYTE)pLjPartPara->angle.cur_wing_x0_y1;
			pLjPartPara=pPlate->designInfo.FromHandle(pBolt->des_work_norm.hVicePart);
			if(pLjPartPara)
				pBolt->des_work_norm.norm_wing=(BYTE)pLjPartPara->angle.cur_wing_x0_y1;
			pBolt->correct_worknorm();
			//���ݸְ巨�߷��������˨���߷��� һ������¸ְ巨�߷�������˨���߷��򱣳�һ��
			if(pBolt->ucs.axis_z*pPlate->ucs.axis_z<0)
				pBolt->des_work_norm.direction=1-pBolt->des_work_norm.direction;
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->CalGuigeAuto();
			pBolt->SetModified();
			if(pDatumLinePart&&pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(((CLDSLineAngle*)pDatumLinePart)->IsLsInWing(pBolt))
					pDatumLinePart->AppendMidLsRef(pBolt->GetLsRef());
			}
			for(pLjPartPara=pPlate->GetFirstLjPara();pLjPartPara;pLjPartPara=pPlate->GetNextLjPara())
			{
				if(pLjPartPara->pCurLinePart==NULL)
					pLjPartPara->pCurLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pLjPartPara->hPart,CLS_LINEPART);
				if(pLjPartPara->pCurLinePart==NULL)
					continue;
				if(pLjPartPara->pCurLinePart&&pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINEANGLE)
				{
					CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pLjPartPara->pCurLinePart;
					if(pLineAngle->IsLsInWing(pBolt))
						pLineAngle->AppendMidLsRef(pBolt->GetLsRef());
				}
			}
			pPlate->AppendLsRef(pBolt->GetLsRef());
		}
		//�������ڸְ弰���߽Ǹ��ϵ���˨��ֱ�� wjh-2015.2.28
		for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
		{
			bool singularResideBolt=true;
			CDesignLjPartPara* pResideLjRod=NULL;
			for(pLjPartPara=pPlate->GetFirstLjPara();pLjPartPara;pLjPartPara=pPlate->GetNextLjPara())
			{
				if(pLjPartPara->pCurLinePart&&pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINEANGLE)
				{
					if(pLjPartPara->pCurLinePart->FindLsByHandle((*pLsRef)->handle)!=NULL)
					{
						if(pResideLjRod==NULL)
							pResideLjRod=pLjPartPara;
						else
						{	//����Ǹ��ϵĹ�����˨,�޷�ȷ������˨���
							singularResideBolt=false;
							break;
						}
					}
				}
			}
			if(singularResideBolt&&pResideLjRod)
			{
				CLDSLineAngle* pAngle=(CLDSLineAngle*)pResideLjRod->pCurLinePart;
				if(pResideLjRod->start0_end1==0)
				{
					(*pLsRef)->set_d(pAngle->connectStart.d);
					(*pLsRef)->CalGuigeAuto();
				}
				else if(pResideLjRod->start0_end1==1)
				{
					(*pLsRef)->set_d(pAngle->connectEnd.d);
					(*pLsRef)->CalGuigeAuto();
				}
				//else
				//{
				//	int d=SelBoltDByWingWide(pAngle->GetWidth())
				//	(*pLsRef)->set_d(d);
				//	(*pLsRef)->CalGuigeAuto();
				//}
			}
		}
		pPlate->DesignPlate();	//������Ƹְ�
	}
	if(pPlate->vertex_list.GetNodeNum()<3)
	{
		if(CLDSObject::_console!=NULL)
			CLDSObject::_console->FireDeleteObject(pPlate);
		m_pTower->DeletePart(pPlate->handle);
		//DispPartSet.DeleteNode(pPlate->handle);
		//Parts.DeleteNode(pPlate->handle);
	}
	return pPlate;
}

void CClonePlate::ToClipperBoard(CBuffer* pClipperBoard,IModel *pTower/*=NULL*/)
{
	SetSuperiorTower(pTower);
	if(m_pTower==NULL)
		return;
	pClipperBoard->ClearBuffer();
	pClipperBoard->WriteString("ClonePlate");
	pClipperBoard->WriteInteger(m_pSrcPlate->handle);
	pClipperBoard->WriteInteger(m_iMatchResult);
	int nPoleNum=hashLinkRods.GetNodeNum();
	pClipperBoard->WriteInteger(nPoleNum);
	LINKPOLEINFO *pLinkPoleInfo=NULL;
	for(pLinkPoleInfo=hashLinkRods.GetFirst();pLinkPoleInfo;pLinkPoleInfo=hashLinkRods.GetNext())
	{
		pClipperBoard->WriteInteger(pLinkPoleInfo->hLinePart);
		pClipperBoard->WriteInteger(pLinkPoleInfo->hMatchPole);
		pClipperBoard->WriteDouble(pLinkPoleInfo->offset_angle);
		pClipperBoard->WriteInteger(pLinkPoleInfo->vec_type);
	}
}

void CClonePlate::FromClipperBoard(CBuffer* pClipperBoard,IModel *pTower/*=NULL*/)
{
	SetSuperiorTower(pTower);
	if(m_pTower==NULL)
		return;
	EmptyLinkRods();
	//���Զ�ȡ�����������ݣ���鱣��������Ƿ�Ϊ�ְ�����
	char sName[50]="";
	pClipperBoard->SeekToBegin();
	pClipperBoard->ReadString(sName,50);
	if(strcmp(sName,"ClonePlate")!=0)
		return;
	long hCurPart=0;
	pClipperBoard->ReadInteger(&hCurPart);
	m_pSrcPlate=(CLDSPlate*)m_pTower->FromPartHandle(hCurPart,CLS_PLATE);
	pClipperBoard->ReadInteger(&m_iMatchResult);
	int i=0,nPoleNum=0;
	pClipperBoard->ReadInteger(&nPoleNum);
	for(i=0;i<nPoleNum;i++)
	{
		LINKPOLEINFO *pLinkPoleInfo=hashLinkRods.Add(0);
		pClipperBoard->ReadInteger(&pLinkPoleInfo->hLinePart);
		pLinkPoleInfo->pLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pLinkPoleInfo->hLinePart,CLS_LINEPART);
		pClipperBoard->ReadInteger(&pLinkPoleInfo->hMatchPole);
		pClipperBoard->ReadDouble(&pLinkPoleInfo->offset_angle);
		pClipperBoard->ReadInteger(&pLinkPoleInfo->vec_type);
	}

}

//���������Դ�˼����������ƥ��ĸ˼�
CLDSLinePart *CClonePlate::FindMatchPoleBySrcHandle(long hLinePart)
{
	if(m_pTower==NULL)
		return NULL;
	LINKPOLEINFO *pLinkPoleInfo=NULL;
	for(pLinkPoleInfo=hashLinkRods.GetFirst();pLinkPoleInfo;pLinkPoleInfo=hashLinkRods.GetNext())
	{
		if(pLinkPoleInfo->hLinePart==hLinePart)
		{
			if(pLinkPoleInfo->pMatchLinePart==NULL)
				pLinkPoleInfo->pMatchLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pLinkPoleInfo->hMatchPole,CLS_LINEPART);
			return pLinkPoleInfo->pMatchLinePart;
		}
	}
	return NULL;	
}

//���������ƥ��˼����������ƥ���Դ�˼�
CLDSLinePart *CClonePlate::FindSrcPoleByMatchHandle(long hMatchPole)
{
	if(m_pTower==NULL)
		return NULL;
	LINKPOLEINFO *pLinkPoleInfo=NULL;
	for(pLinkPoleInfo=hashLinkRods.GetFirst();pLinkPoleInfo;pLinkPoleInfo=hashLinkRods.GetNext())
	{
		if(pLinkPoleInfo->hMatchPole==hMatchPole)
		{
			if(pLinkPoleInfo->pLinePart==NULL)
				pLinkPoleInfo->pLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pLinkPoleInfo->hLinePart,CLS_LINEPART);
			return pLinkPoleInfo->pLinePart;
		}
	}
	return NULL;	
}
//���������ƥ��˼����������ƥ���Դ�˼�
CSchemaRod* CClonePlate::QuerySchemaRod(long hRod,char ciRodTypeSrc0Map1/*=0*/)
{
	if(m_pTower==NULL)
		return NULL;
	CSchemaRod *pSchemaRod=NULL;
	for(pSchemaRod=hashLinkRods.GetFirst();pSchemaRod;pSchemaRod=hashLinkRods.GetNext())
	{
		if( (pSchemaRod->hLinePart==hRod&&ciRodTypeSrc0Map1==0)||
			(pSchemaRod->hMatchPole==hRod&&ciRodTypeSrc0Map1==1))
		{
			if(pSchemaRod->pLinePart==NULL)
				pSchemaRod->pLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pSchemaRod->hLinePart,CLS_LINEPART);
			return pSchemaRod;
		}
	}
	return NULL;	
}
void CClonePlate::SortLinePartSet(CLDSLinePart *pDatumLinePart/*=NULL*/,int iSortType/*=0*/,CLDSPlate *pPlate/*=NULL*/)
{
	if(partset.GetNodeNum()<=0||pDatumLinePart==NULL)
		return;
	if(iSortType==1&&pPlate==NULL)
		return;	//���ְ����Ӹ˼����ϰ���ʱ������
	int i,j,n;
	BOOL sorted;	//ð������ʱ�������ʶ
	double temangle;
	CLDSLinePart* pTemPart;
	n = partset.GetNodeNum();
	f3dPoint datum_line_vec=pDatumLinePart->pEnd->Position(true)-pDatumLinePart->pStart->Position(true);
	normalize(datum_line_vec);
	double *angle = new double[n];
	for(i=0;i<n;i++)
	{
		if(iSortType==1)
		{	//ʹ�Ǹּ�����ʱ������
			f3dLine line3d;
			f2dLine line2d;
			CDesignLjPartPara *pLjPartPara=NULL;
			pLjPartPara=pPlate->designInfo.FromHandle(partset[i]->handle);
			if(pLjPartPara)
			{
				if(pLjPartPara->start0_end1==0)
					line3d.endPt=partset[i]->Start();
				else if(pLjPartPara->start0_end1==1)
					line3d.endPt=partset[i]->End();
				coord_trans(line3d.endPt,pPlate->ucs,FALSE);
				line2d.endPt.Set(line3d.endPt.x,line3d.endPt.y);
				angle[i] = Cal2dLineAng(line2d);
			}
		}
		else if(iSortType==0)
		{	//ʹ�Ǹּ������׼�˼�֮��нǵ���Ǵ�С��С��������
			f3dPoint cur_line_vec=partset[i]->pEnd->Position(true)-partset[i]->pStart->Position(true);
			normalize(cur_line_vec);
			if(partset[i]->pEnd==this->m_pDestNode)
				cur_line_vec*=-1.0;
			angle[i] = fabs(cur_line_vec*datum_line_vec);
		}
	}
	
    for(i=0;i<n-1;i++)   //ð������
    {
        for(j=0;j<n-i-1;j++)
        {
			sorted = TRUE;
            if(angle[j]>angle[j+1])
            {
                temangle = angle[j];
                angle[j]   = angle[j+1];
                angle[j+1] = temangle;

				pTemPart = partset[j];
				partset[j] = partset[j+1];
				partset[j+1] = pTemPart;
				sorted = FALSE;
            }
        }
		if(sorted)	//������
			break;	
    }
	delete []angle;
}

//���ݸְ����ӵ����߸˼���������Ʋ���
void CClonePlate::CorrectPlateDesignPara(CLDSPlate *pPlate)
{
	if(m_pTower==NULL||pPlate==NULL)
		return;
	CDesignLjPartPara *pLjPartPara=NULL;
	for(pLjPartPara=pPlate->GetFirstLjPara();pLjPartPara;pLjPartPara=pPlate->GetNextLjPara())
	{
		if(pLjPartPara->pCurLinePart==NULL)
			pLjPartPara->pCurLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pLjPartPara->hPart,CLS_LINEPART);
		if(pLjPartPara->pCurLinePart==NULL)
			continue;
		if(pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLDSLineAngle *pCurLineAngle=(CLDSLineAngle*)pLjPartPara->pCurLinePart;
			JGZJ jgzj;
			if(pCurLineAngle->m_bEnableTeG)
			{
				if(pLjPartPara->angle.cur_wing_x0_y1==0)
					jgzj = pCurLineAngle->xWingXZhunJu;
				else
					jgzj = pCurLineAngle->xWingYZhunJu;
			}
			else
				getjgzj(jgzj,pCurLineAngle->GetWidth());
			pLjPartPara->wing_space=pCurLineAngle->GetWidth()-jgzj.g;
			pLjPartPara->ber_space=jgzj.g;
		}
		else if(pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINETUBE)
		{	//���߸˼�Ϊ�ֹܣ�Ӧ���ݹ������ֹܲ������
			
		}
	}
	pPlate->DesignPlate();
}

//�����ְ���˨�����˨������˨��Ϣ,������˨ͨ��
void CClonePlate::CorrectPlateLsInfo(CLDSPlate *pPlate)
{
	if(m_pTower==NULL||pPlate==NULL)
		return;
	//��¼���������߸˼�
	CLDSLineAngle *pEndLjAngle1=NULL,*pEndLjAngle2=NULL;
	for(CDesignLjPartPara* pLjPartPara=pPlate->GetFirstLjPara();pLjPartPara;pLjPartPara=pPlate->GetNextLjPara())
	{
		pLjPartPara->pCurLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pLjPartPara->hPart,CLS_LINEPART);
		if(pLjPartPara->pCurLinePart==NULL)
			continue;
		if(pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pLjPartPara->pCurLinePart;
			if(pEndLjAngle1==NULL&&pLjPartPara->angle.bEndLjJg)
				pEndLjAngle1=pLineAngle;
			else if(pEndLjAngle2==NULL&&pLjPartPara->angle.bEndLjJg)
				pEndLjAngle2=pLineAngle;
			LSSPACE_STRU OldLsSpace,NewLsSpace;	//��ǰ�Ǹ�Ԥ�����õ���˨��� ��׼��˨���
			GetLsSpace(NewLsSpace,pLineAngle->connectStart.d);
			int nNewBoltD=pLineAngle->connectStart.d;//��ǰ�Ǹ���˨ֱ��
			for(CLsRef *pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
			{
				if(!pPlate->FindLsByHandle((*pLsRef)->handle))	
					continue;	//�������ڵ�ǰ�ְ��ϵ���˨
				if(nNewBoltD==(*pLsRef)->get_d())
					continue;
				GetLsSpace(OldLsSpace,(*pLsRef)->get_d());
				if(pLjPartPara->start0_end1==0)
					(*pLsRef)->set_d(pLineAngle->connectStart.d);
				else //if(pLjPartPara->start0_end1==1) //�м���˨�������ն���˨���ȷ��
					(*pLsRef)->set_d(pLineAngle->connectEnd.d);
				//�����Ǹ��϶˾�Ϊ��׼�˾�ͱ�׼������˨����Ʋ���
				int nFlag=1;		//��ʾ��˨ƫ�ƾ��������
				BOOL bSingleLs=TRUE;//TRUE:����  FALSE:˫��
				int nSpaceNum=0;	//�����:��¼��˨��Ǹֶ˴��ڼ�����׼���
				int nLsEndSpace=0;	//��˨�����׼�㲻��һ�����Ĳ���
				if((*pLsRef)->des_base_pos.len_offset_dist<0)
					nFlag=-1;
				nLsEndSpace=abs((int)(*pLsRef)->des_base_pos.len_offset_dist)%OldLsSpace.SingleRowSpace;
				if((*pLsRef)->des_base_pos.datumPoint.datum_pos_style==1)
				{	//�Ǹֶ˽ڵ㶨λ
					if(abs(nLsEndSpace-OldLsSpace.EndSpace)<EPS) //������˨
						nSpaceNum=abs((int)(*pLsRef)->des_base_pos.len_offset_dist)/OldLsSpace.SingleRowSpace;
					else 
					{	//˫����˨
						bSingleLs=FALSE;	
						nLsEndSpace=abs((int)(*pLsRef)->des_base_pos.len_offset_dist)%OldLsSpace.doubleRowSpace;
						nSpaceNum=abs((int)(*pLsRef)->des_base_pos.len_offset_dist)/OldLsSpace.doubleRowSpace;
					}
					if(abs(nLsEndSpace-OldLsSpace.EndSpace)<EPS)
					{	//��˨Ϊ����׼��಼�õ���˨
						if(bSingleLs)
							(*pLsRef)->des_base_pos.len_offset_dist=(NewLsSpace.SingleRowSpace*nSpaceNum+NewLsSpace.EndSpace)*nFlag;
						else	//˫����˨��Ҫ������˨����׼�� ������޸Ĺʲ��ڴ˴�����
							(*pLsRef)->des_base_pos.len_offset_dist=(NewLsSpace.doubleRowSpace*nSpaceNum+NewLsSpace.EndSpace)*nFlag;
					}
				}
				else if((*pLsRef)->des_base_pos.datumPoint.datum_pos_style==2)
				{	//�˼��Ͻڵ㶨λ
					if(fabs(nLsEndSpace-OldLsSpace.SingleRowSpace*0.5)<EPS||abs(nLsEndSpace)<EPS) //������˨
						nSpaceNum=abs((int)(*pLsRef)->des_base_pos.len_offset_dist)/OldLsSpace.SingleRowSpace;
					else 
					{	//˫����˨
						bSingleLs=FALSE;	
						nLsEndSpace=abs((int)(*pLsRef)->des_base_pos.len_offset_dist)%OldLsSpace.doubleRowSpace;
						nSpaceNum=abs((int)(*pLsRef)->des_base_pos.len_offset_dist)/OldLsSpace.doubleRowSpace;
					}
					if(abs(nLsEndSpace-OldLsSpace.EndSpace)<EPS)
					{	//��˨Ϊ����׼��಼�õ���˨
						if(bSingleLs)
							(*pLsRef)->des_base_pos.len_offset_dist=(NewLsSpace.SingleRowSpace*nSpaceNum+NewLsSpace.EndSpace)*nFlag;
						else	//˫����˨��Ҫ������˨����׼�� ������޸Ĺʲ��ڴ˴�����
							(*pLsRef)->des_base_pos.len_offset_dist=(NewLsSpace.doubleRowSpace*nSpaceNum+NewLsSpace.EndSpace)*nFlag;
					}
				}
			}
		}
		/*else if(pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINETUBE)
		{
		
		}*/
	}
	//��������������߸˼���ص���˨��λ��
	int i=0;
	int nMinDist=13;	//��˨��Ǹ�֮�����С����
	//�������������߽Ǹ��ϵ���˨λ��
	for(i=0;i<2;i++)
	{
		CLDSLineAngle *pEndLjAngle=NULL;
		if(i==0)
			pEndLjAngle=pEndLjAngle1;
		else 
			pEndLjAngle=pEndLjAngle2;
		if(pEndLjAngle==NULL)
			continue;
		BOLTSET boltset;
		for(CLsRef *pLsRef=pEndLjAngle->GetFirstLsRef();pLsRef;pLsRef=pEndLjAngle->GetNextLsRef())
		{
			if(!pPlate->FindLsByHandle((*pLsRef)->handle))	
				continue;	//�������ڵ�ǰ�ְ��ϵ���˨
			if((*pLsRef)->des_base_pos.datumPoint.datum_pos_style!=3)
				continue;	//��˨���㶨λ��ʽ�ǽǸ����߽������˨���õ���λ��
			//�����������߽Ǹ��϶�λ��ʽΪ�Ǹ����߽������˨����˨����ƫ������С��������
			CLDSBolt *pBolt=NULL;
			for(pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
			{
				if((*pLsRef)->des_base_pos.len_offset_dist<pBolt->des_base_pos.len_offset_dist)
				{
					boltset.insert(pLsRef->GetLsPtr());
					break;
				}
			}
			if(pBolt==NULL)
				boltset.append(pBolt);
		}
		if(boltset.GetNodeNum()<=0)
			continue;
		CLDSBolt *pFirstBolt=boltset.GetFirst();
		//������˨ֱ��������С����
		if(pFirstBolt->get_d()==12)
			nMinDist=13;
		else if(pFirstBolt->get_d()==16)
			nMinDist=18;
		else if(pFirstBolt->get_d()==20)
			nMinDist=20;
		else if(pFirstBolt->get_d()==24)
			nMinDist=25;
		else
			nMinDist=pFirstBolt->get_d();
		//ȷ����׼�Ǹּ����߽���׼��
		long hDatumPart=pFirstBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1;
		CLDSLineAngle * pDatumLineAngle=(CLDSLineAngle*)m_pTower->FromPartHandle(hDatumPart,CLS_LINEANGLE);
		if(pDatumLineAngle==NULL)
			continue;
		JGZJ jgzj;
		getjgzj(jgzj,pDatumLineAngle->GetWidth());
		int offset_type=0;
		int wing_offset_dist=0;
		if(pFirstBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1==4)
			wing_offset_dist=pFirstBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1;
		else	
			offset_type=pFirstBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1;
		if(hDatumPart==pEndLjAngle->handle)
		{
			hDatumPart=pFirstBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2;
			if(pFirstBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2==4)
				wing_offset_dist=pFirstBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2;
			else	
				offset_type=pFirstBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2;
		}
		if(offset_type==0)
			wing_offset_dist=jgzj.g;
		else if(offset_type==1)
			wing_offset_dist=jgzj.g1;
		else if(offset_type==2)
			wing_offset_dist=jgzj.g2;
		else if(offset_type==3)
			wing_offset_dist=jgzj.g3;
		//�����׼�Ǹ�������ӽǸ�֮��ļн�
		f3dPoint datum_line_vec=pDatumLineAngle->End()-pDatumLineAngle->Start();
		normalize(datum_line_vec);
		f3dPoint cur_line_vec=pEndLjAngle->End()-pEndLjAngle->Start();
		normalize(cur_line_vec);
		double cosa=fabs(datum_line_vec*cur_line_vec);
		double sina=sqrt(1-cosa*cosa);
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace,pFirstBolt->get_d());
		//��˨����׼�Ǹ����ߵĴ�ֱ����
		double vertical_offset_dist=pDatumLineAngle->GetWidth()-wing_offset_dist+nMinDist;
		//��˨�����߽Ǹ��ϵ�ƫ�ƾ���
		int len_offset_dist=(int)(vertical_offset_dist/sina);
		if(len_offset_dist%5>0)	//��5Բ��
			len_offset_dist=(len_offset_dist/5+1)*5;
		len_offset_dist+=LsSpace.EndSpace;
	}
}

//�������߸˼�ƫ���� ��ȷ����仯����Ҫ����ƫ�������˴��������������߸˼�ƫ����
void CClonePlate::UpdateRayLinePartOffset(CLDSPlate *pPlate,int nAutoAdjustTubePos/*=0*/)
{
	if(m_pTower==NULL||pPlate==NULL)
		return;
	CSuperSmartPtr<CLDSLinePart> pDatumLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pPlate->designInfo.m_hBasePart,CLS_LINEPART);
	if(pDatumLinePart.IsNULL())
		return;

	f3dPoint work_norm;
	CDesignLjPartPara *pLjPartPara=NULL;
	if(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE)
	{	//��׼�˼�Ϊ�Ǹ� ���㹤�����߷���
		pLjPartPara=pPlate->designInfo.FromHandle(pDatumLinePart->handle);
		if(pLjPartPara&&pLjPartPara->angle.cur_wing_x0_y1==0)
			work_norm=pDatumLinePart.LineAnglePointer()->get_norm_x_wing();
		else if(pLjPartPara&&pLjPartPara->angle.cur_wing_x0_y1)
			work_norm=pDatumLinePart.LineAnglePointer()->get_norm_y_wing();
	}
	for(pLjPartPara=pPlate->GetFirstLjPara();pLjPartPara;pLjPartPara=pPlate->GetNextLjPara())
	{
		if(pLjPartPara->pCurLinePart==NULL)
			pLjPartPara->pCurLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pLjPartPara->hPart,CLS_LINEPART);
		if(pLjPartPara->pCurLinePart==NULL)
			continue;
		if(pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			if(pLjPartPara->pCurLinePart&&pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINEANGLE
				&&((CLDSLineAngle*)pLjPartPara->pCurLinePart)->group_father_jg_h==pDatumLinePart->handle)
				continue;
		}
		else if(pLjPartPara->hPart==pDatumLinePart->handle)
			continue;
		if(!work_norm.IsZero()&&pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINEANGLE
			&&pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{	//��׼�˼�Ϊ�Ǹ� �������߸˼�ƫ����
			CMultiOffsetPos *pDesPosPara=NULL;
			if(pLjPartPara->start0_end1==0)
				pDesPosPara=&((CLDSLineAngle*)pLjPartPara->pCurLinePart)->desStartPos;
			else if(pLjPartPara->start0_end1==1)
				pDesPosPara=&((CLDSLineAngle*)pLjPartPara->pCurLinePart)->desEndPos;
			double ddx=work_norm*((CLDSLineAngle*)pLjPartPara->pCurLinePart)->get_norm_x_wing();
			double ddy=work_norm*((CLDSLineAngle*)pLjPartPara->pCurLinePart)->get_norm_y_wing();
			if(pDesPosPara&&pLjPartPara->angle.cur_wing_x0_y1==0)
			{	//�Զ������Ǹַ���ƫ����
				pDesPosPara->wing_x_offset.gStyle=4;
				pDesPosPara->wing_y_offset.gStyle=0;
				if(ddx>0)
				{	//����
					if(pLjPartPara->angle.bEndLjJg)	//������
						pDesPosPara->wing_x_offset.offsetDist=-pDatumLinePart->GetThick();
					else 
						pDesPosPara->wing_x_offset.offsetDist=0;
				} 
				else //���� 
					pDesPosPara->wing_x_offset.offsetDist=-pPlate->GetThick();	
			}
			else if(pDesPosPara&&pLjPartPara->angle.cur_wing_x0_y1==1)
			{	//�Զ������Ǹַ���ƫ����
				pDesPosPara->wing_x_offset.gStyle=0;
				pDesPosPara->wing_y_offset.gStyle=4;
				if(ddy>0)
				{	//����
					if(pLjPartPara->angle.bEndLjJg)	//������
						pDesPosPara->wing_y_offset.offsetDist=-pDatumLinePart->GetThick();
					else
						pDesPosPara->wing_y_offset.offsetDist=0;
				}
				else
					pDesPosPara->wing_y_offset.offsetDist=-pPlate->GetThick();
			}
		}
		else if(pDatumLinePart->GetClassTypeId()==CLS_LINETUBE)
		{	//��׼�˼�Ϊ�ֹ�
			if(pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{	//���߸˼�Ϊ�Ǹ� ��������ƫ�ư�����
				CMultiOffsetPos *pDesPosPara=NULL;
				if(pLjPartPara->start0_end1==0)
					pDesPosPara=&((CLDSLineAngle*)pLjPartPara->pCurLinePart)->desStartPos;
				else if(pLjPartPara->start0_end1==1)
					pDesPosPara=&((CLDSLineAngle*)pLjPartPara->pCurLinePart)->desEndPos;
				if(pDesPosPara&&pLjPartPara->angle.cur_wing_x0_y1==0)
				{	//X֫Ϊ��ǰ����֫
					pDesPosPara->wing_y_offset.gStyle=0;
					pDesPosPara->wing_x_offset.gStyle=4;
					pDesPosPara->wing_x_offset.offsetDist=-0.5*pPlate->GetThick();
				}
				else if(pDesPosPara&&pLjPartPara->angle.cur_wing_x0_y1==1)
				{	//Y֫Ϊ��ǰ����֫
					pDesPosPara->wing_x_offset.gStyle=0;
					pDesPosPara->wing_y_offset.gStyle=4;
					pDesPosPara->wing_y_offset.offsetDist=-0.5*pPlate->GetThick();
				}
			}
			else if(pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINETUBE)
			{	//�Զ������ֹܻ���λ��λ��
				TUBEJOINT *pCurJoint=NULL;
				CTubeEndPosPara *pCurDesPara=NULL;
				if(pLjPartPara->start0_end1==0)
				{
					pCurDesPara=&((CLDSLineTube*)pLjPartPara->pCurLinePart)->desStartPos;
					pCurJoint=&((CLDSLineTube*)pLjPartPara->pCurLinePart)->m_tJointStart;
				}
				else //if(pLjPartPara->start0_end1==1)
				{
					pCurDesPara=&((CLDSLineTube*)pLjPartPara->pCurLinePart)->desEndPos;
					pCurJoint=&((CLDSLineTube*)pLjPartPara->pCurLinePart)->m_tJointEnd;
				}
				CLDSParamPlate *pParamPlate=NULL;
				pParamPlate=(CLDSParamPlate*)m_pTower->FromPartHandle(pCurJoint->hLinkObj,CLS_PARAMPLATE);
				if(pParamPlate&&pParamPlate->m_iParamType==TYPE_ROLLEND)
				{
					double fWorkPlaneRotAngle=0;	//����ƽ����ת�Ƕ�
					double offset_dist=-0.5*(pParamPlate->GetThick()+pPlate->GetThick());
					double X=0;	//D���׼������ز���ͬ��,��X��ʾ��ת�Ƕ�  ��ʱ��˴��� wht 10-01-21
					if(pParamPlate->m_bStdPart)
						pParamPlate->GetDesignItemValue('X',&fWorkPlaneRotAngle);
					else
						pParamPlate->GetDesignItemValue('D',&fWorkPlaneRotAngle);
					if(fabs(fabs(fWorkPlaneRotAngle)-180)<EPS)	//����߷����෴���巨�߷����෴
						offset_dist*=-1.0;	
					if(pCurDesPara&&nAutoAdjustTubePos==2)	//�ֹ�ƫ��
						pCurDesPara->fNormOffset+=offset_dist;
					else if(pCurDesPara&&nAutoAdjustTubePos==1) //���ƫ��
						pParamPlate->m_fNormOffset-=offset_dist;
				}
			}
		}
	}
}

//�������߸˼�����ͷ
void CClonePlate::UpdateRayLinePartOddment(CLDSPlate *pPlate)
{
	if(m_pTower==NULL||pPlate==NULL)
		return;
	CLDSLinePart *pDatumLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pPlate->designInfo.m_hBasePart,CLS_LINEPART);
	if(pDatumLinePart==NULL)
		return;
	//���ְ����߸˼�����ʱ������ ȡ��׼�˼����һ���˼�Ϊ��ײ��׼�˼�
	SortLinePartSet(pDatumLinePart,1,pPlate);
	CDesignLjPartPara *pLjPartPara=NULL;
	for(pLjPartPara=pPlate->GetFirstLjPara();pLjPartPara;pLjPartPara=pPlate->GetNextLjPara())
	{
		if(pLjPartPara->pCurLinePart==NULL)
			pLjPartPara->pCurLinePart=(CLDSLinePart*)m_pTower->FromPartHandle(pLjPartPara->hPart,CLS_LINEPART);
		if(pLjPartPara->pCurLinePart==NULL)
			continue;
		//���ҵڶ�����ײ��׼�˼�
		CLDSLinePart *pCurLinePart=NULL,*pNextPart=NULL,*pPrevPart=NULL;
		for(pCurLinePart=partset.GetFirst();pCurLinePart;pCurLinePart=partset.GetNext())
		{
			if(pCurLinePart==pLjPartPara->pCurLinePart)
				break;
			if(pCurLinePart->handle!=pDatumLinePart->handle)
				pPrevPart=pCurLinePart;
		}
		pNextPart=partset.GetNext();
		if(pNextPart==NULL)
		{
			pNextPart=partset.GetFirst();	//��ĩβ�Ǹֵ���һ�Ǹ־����׽Ǹ�
			if(pNextPart->handle==pDatumLinePart->handle)
				pNextPart=partset.GetNext();
			if(pNextPart==pLjPartPara->pCurLinePart)
				pNextPart=NULL;
		}
		long hRefPart2=0;
		if(pNextPart&&pNextPart->handle!=pDatumLinePart->handle)
			hRefPart2=pNextPart->handle;
		else if(pPrevPart)
			hRefPart2=pPrevPart->handle;
		if(hRefPart2==pLjPartPara->pCurLinePart->handle)
			hRefPart2=0;	//��ײ��׼�˼�����Ϊ�˼�����
		if(pLjPartPara->start0_end1==0)
		{
			pLjPartPara->pCurLinePart->desStartOdd.m_hRefPart1=pDatumLinePart->handle;
			pLjPartPara->pCurLinePart->desStartOdd.m_hRefPart2=hRefPart2;
			pLjPartPara->pCurLinePart->desStartOdd.m_iOddCalStyle=0;		//������ײ��������ͷ
			if(pLjPartPara->angle.bEndLjJg&&pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINEANGLE)
				pLjPartPara->pCurLinePart->desStartOdd.m_iOddCalStyle=1;	//���������߸˼�������˨��������ͷ
			pLjPartPara->pCurLinePart->CalStartOddment();
		}
		else
		{
			pLjPartPara->pCurLinePart->desEndOdd.m_hRefPart1=pDatumLinePart->handle;
			pLjPartPara->pCurLinePart->desEndOdd.m_hRefPart2=hRefPart2;
			pLjPartPara->pCurLinePart->desEndOdd.m_iOddCalStyle=0;		//������ײ��������ͷ
			if(pLjPartPara->angle.bEndLjJg&&pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINEANGLE)
				pLjPartPara->pCurLinePart->desEndOdd.m_iOddCalStyle=1;	//���������߸˼�������˨��������ͷ
			pLjPartPara->pCurLinePart->CalEndOddment();
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//CDeepClonePlate
//////////////////////////////////////////////////////////////////////////
CDeepClonePlate::CDeepClonePlate(IModel *pTower):CClonePlate(pTower)
{
	m_cMateria='S';
	m_nThick=8;
}
CDeepClonePlate::~CDeepClonePlate()
{

}
int CDeepClonePlate::GetSchemaRodId(long hRod,BOOL bMatchRod/*=FALSE*/)
{
	int idKey=-1;
	for(CSchemaRod* pSchemaRod=hashLinkRods.GetFirst();pSchemaRod;pSchemaRod=hashLinkRods.GetNext())
	{
		if(!bMatchRod && pSchemaRod->hLinePart==hRod)
		{
			idKey=pSchemaRod->Id;
			break;
		}
		else if(bMatchRod && pSchemaRod->hMatchPole==hRod)
		{
			idKey=pSchemaRod->Id;
			break;
		}
	}
	return idKey;
}
void CDeepClonePlate::ExtractPlateInfo(CLDSPlate* pSrcPlate)
{
	if(pSrcPlate==NULL|| m_pTower==NULL)
		return;
	CLDSLinePart *pDatumRod=(CLDSLinePart*)m_pTower->FromPartHandle(pSrcPlate->designInfo.m_hBasePart,CLS_LINEPART);
	if(pDatumRod==NULL||pDatumRod->pStart==NULL||pDatumRod->pEnd==NULL)
		return;
	if(pSrcPlate->designInfo.origin.datum_pos_style!=2)	//�ڵ�嶨λ��ʽΪ�˼��ڵ�
		return;
	CClonePlate::AnalyzePlateInfo(pSrcPlate);
	m_cMateria=pSrcPlate->cMaterial;
	m_nThick=(int)pSrcPlate->GetThick();
	m_nPlaneN=(BYTE)m_pSrcPlate->face_N;
	m_idDatumRod=GetSchemaRodId(pDatumRod->handle);
	m_bFlexibleDesign=pSrcPlate->designInfo.m_bEnableFlexibleDesign;
	m_ciProfileSty=(BYTE)pSrcPlate->designInfo.iProfileStyle0123;
	ciDirectionAxisZ=pSrcPlate->designInfo.norm.direction;	//�ְ巨����
	//����װ���{hAnchorAngle,iStyleG}
	origin.g_arr[0].ciStyleG=(BYTE)pSrcPlate->designInfo.origin.des_para.RODNODE.wing_offset_style;
	origin.g_arr[0].g=(short)pSrcPlate->designInfo.origin.des_para.RODNODE.wing_offset_dist;
	origin.ciLenDirection=(BYTE)pSrcPlate->designInfo.origin.des_para.RODNODE.direction;
	origin.siLenOffset=(short)pSrcPlate->designInfo.origin.des_para.RODNODE.len_offset_dist;
	if(m_nPlaneN>1)
	{	//������Ϣ
		for(int i=0;i<2;i++)
		{
			huoqu_pt[i].g_arr[0].ciStyleG=(BYTE)pSrcPlate->designInfo.huoquline_start[i].des_para.RODNODE.wing_offset_style;
			huoqu_pt[i].g_arr[0].g=(short)pSrcPlate->designInfo.huoquline_start[i].des_para.RODNODE.wing_offset_dist;
			huoqu_pt[i].ciLenDirection=(BYTE)pSrcPlate->designInfo.huoquline_start[i].des_para.RODNODE.direction;
			huoqu_pt[i].siLenOffset=(short)pSrcPlate->designInfo.huoquline_start[i].des_para.RODNODE.len_offset_dist;
			huoqu_norm[i].m_idViceRod=GetSchemaRodId(pSrcPlate->designInfo.huoqufacenorm[i].hVicePart);
			huoqu_norm[i].m_idCrossRod=GetSchemaRodId(pSrcPlate->designInfo.huoqufacenorm[i].hCrossPart);
		}
	}
	//�ְ����˨��Ϣ
	int nKey=1;
	for(CLsRef* pLsRef=pSrcPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcPlate->GetNextLsRef())
	{
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		CSchemaBolt* pSchemaBolt=hashTemplBolts.Add(nKey);
		pSchemaBolt->d=pBolt->get_d();
		strcpy(pSchemaBolt->grade,pBolt->Grade());
		//��˨����ַ���
		CXhChar100 key_str;
		pBolt->GetL0KeyStr(key_str);
		CXhChar100 keystr=key_str;
		int ciIndex=0;
		for(char* key=strtok(keystr,", ");key!=NULL;key=strtok(NULL,", "))
		{
			if(key[0]=='0'||(strlen(key)>2&&key[0]=='-'&&key[1]=='0'))	//���
			{
				int hVal=0,iKey=-1;
				sscanf(key,"%X",&hVal);
				iKey=GetSchemaRodId(hVal);
				if(iKey>0)
				{
					pSchemaBolt->through.items[ciIndex].ciItemType=1;
					pSchemaBolt->through.items[ciIndex].ciThickVal=iKey;
					ciIndex++;
				}
			}
			else	//ָ�����ֵ
			{
				pSchemaBolt->through.items[ciIndex].ciItemType=2;
				pSchemaBolt->through.items[ciIndex].ciThickVal=atoi(key);
				ciIndex++;
			}
		}
		//����
		pSchemaBolt->des_base_pos.anchor.ciAnchorType=pBolt->des_base_pos.datumPoint.datum_pos_style;
		if(pBolt->des_base_pos.datumPoint.datum_pos_style==1)		//�Ǹ��������Ϊ��׼
		{	
			pSchemaBolt->des_base_pos.anchor.idPriorAngle=GetSchemaRodId(pBolt->des_base_pos.datumPoint.des_para.RODEND.hRod);
			pSchemaBolt->des_base_pos.anchor.g_arr[0].ciStyleG=pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style;
			pSchemaBolt->des_base_pos.anchor.g_arr[1].g=ftoi(pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist);
			pSchemaBolt->des_base_pos.anchor.bIncOddEffect=pBolt->des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect;
		}
		else if(pBolt->des_base_pos.datumPoint.datum_pos_style==2)	//�˼��Ͻڵ�Ϊ��׼
		{	
			pSchemaBolt->des_base_pos.anchor.idPriorAngle=GetSchemaRodId(pBolt->des_base_pos.datumPoint.des_para.RODNODE.hRod);
			pSchemaBolt->des_base_pos.anchor.g_arr[0].ciStyleG=pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style;
			pSchemaBolt->des_base_pos.anchor.g_arr[0].g=ftoi(pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist);
			pSchemaBolt->des_base_pos.anchor.ciIntersPlane=(BYTE)pBolt->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset;
			pSchemaBolt->des_base_pos.anchor.vPlaneNormal=pBolt->des_base_pos.datumPoint.des_para.RODNODE.vPlaneNormal;
		}
		else if(pBolt->des_base_pos.datumPoint.datum_pos_style==3)	//�Ǹ����߽���
		{
			pSchemaBolt->des_base_pos.anchor.idPriorAngle=GetSchemaRodId(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1);
			pSchemaBolt->des_base_pos.anchor.g_arr[0].ciStyleG=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1;
			pSchemaBolt->des_base_pos.anchor.g_arr[0].g=ftoi(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1);
			pSchemaBolt->des_base_pos.anchor.idSecondaryAngle=GetSchemaRodId(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2);
			pSchemaBolt->des_base_pos.anchor.g_arr[1].ciStyleG=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2;
			pSchemaBolt->des_base_pos.anchor.g_arr[1].g=ftoi(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2);
			pSchemaBolt->des_base_pos.anchor.ciIntersPlane=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.cProjectPlane;
		}
		pSchemaBolt->des_base_pos.idResideAngle=GetSchemaRodId(pBolt->des_base_pos.hPart);
		pSchemaBolt->des_base_pos.ciWorkWing=pBolt->des_base_pos.offset_wing;
		pSchemaBolt->des_base_pos.cLocationStyle=pBolt->des_base_pos.cLocationStyle;
		pSchemaBolt->des_base_pos.ciDirection=pBolt->des_base_pos.direction;
		pSchemaBolt->des_base_pos.len_offset_dist=ftoi(pBolt->des_base_pos.len_offset_dist);
		pSchemaBolt->des_base_pos.ciWingOffsetStyle=pBolt->des_base_pos.m_biWingOffsetStyle;
		pSchemaBolt->des_base_pos.wing_offset_dist=ftoi(pBolt->des_base_pos.wing_offset_dist);
		//����
		pSchemaBolt->normal.ciNormStyle=1;
		pSchemaBolt->normal.ciDirection=(BYTE)pBolt->des_work_norm.direction;
		pSchemaBolt->normal.ciWorkWing=(BYTE)pBolt->des_work_norm.norm_wing;
		pSchemaBolt->normal.idResideAngle=GetSchemaRodId(pBolt->des_work_norm.hVicePart);
		nKey++;
	}
}
char* CDeepClonePlate::GetThroughStr(CSchemaBolt* pSchemaBolt)
{
	CXhChar50 thick_str;
	for(BYTE i=0;i<6;i++)
	{
		if(pSchemaBolt->through.items[i].ciItemType==0)
			break;
		else if(pSchemaBolt->through.items[i].ciItemType==1)
		{	//���׹���ID
			if(thick_str.GetLength()>0)
				thick_str.Append(',');
			int idKey=abs(pSchemaBolt->through.items[i].ciThickVal);
			CSchemaRod* pMatchRod=hashLinkRods.GetValue(idKey);
			if(pMatchRod)
			{
				if(pSchemaBolt->through.items[i].ciThickVal<0)
					thick_str.Append('-');
				thick_str.Append(CXhChar16("0X%X",pMatchRod->hMatchPole));
			}
		}
		else if(pSchemaBolt->through.items[i].ciItemType==2)
		{	//ָ�����ֵ
			if(thick_str.GetLength()>0)
				thick_str.Append(',');
			thick_str.Append(CXhChar16("%d",pSchemaBolt->through.items[i].ciThickVal));
		}
	}
	return (char*)thick_str;
}
bool CDeepClonePlate::UpdateBoltBySchema(CSchemaBolt* pSchemaBolt,CLDSBolt* pBolt)
{
	if(pSchemaBolt==NULL || pBolt==NULL)
		return false;
	pBolt->set_d(pSchemaBolt->d);
	pBolt->SetGrade(pSchemaBolt->grade);
	//����
	pBolt->des_base_pos.datumPoint.datum_pos_style=pSchemaBolt->des_base_pos.anchor.ciAnchorType;
	CSchemaRod* pMatchRod=hashLinkRods.GetValue(pSchemaBolt->des_base_pos.anchor.idPriorAngle);
	CSchemaRod* pMatchCrosRod=hashLinkRods.GetValue(pSchemaBolt->des_base_pos.anchor.idSecondaryAngle);
	if(pBolt->des_base_pos.datumPoint.datum_pos_style==1)		//�Ǹ��������Ϊ��׼
	{	
		pBolt->des_base_pos.datumPoint.des_para.RODEND.hRod=pMatchRod->hMatchPole;
		pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=pSchemaBolt->des_base_pos.anchor.g_arr[0].ciStyleG;
		pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist=pSchemaBolt->des_base_pos.anchor.g_arr[1].g;
		pBolt->des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect=pSchemaBolt->des_base_pos.anchor.bIncOddEffect;
	}
	else if(pBolt->des_base_pos.datumPoint.datum_pos_style==2)	//�˼��Ͻڵ�Ϊ��׼
	{	
		pBolt->des_base_pos.datumPoint.des_para.RODNODE.hNode=m_pDestNode->handle;
		pBolt->des_base_pos.datumPoint.des_para.RODNODE.hRod=pMatchRod->hMatchPole;
		pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style=pSchemaBolt->des_base_pos.anchor.g_arr[0].ciStyleG;
		pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist=pSchemaBolt->des_base_pos.anchor.g_arr[0].g;
		pBolt->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset=pSchemaBolt->des_base_pos.anchor.ciIntersPlane;
		if(pBolt->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset)
			pBolt->des_base_pos.datumPoint.des_para.RODNODE.vPlaneNormal=pSchemaBolt->des_base_pos.anchor.vPlaneNormal;
	}
	else if(pBolt->des_base_pos.datumPoint.datum_pos_style==3)	//�Ǹ����߽���
	{
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1=pMatchRod->hMatchPole;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2=pMatchCrosRod->hMatchPole;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=pSchemaBolt->des_base_pos.anchor.g_arr[0].ciStyleG;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1=pSchemaBolt->des_base_pos.anchor.g_arr[0].g;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=pSchemaBolt->des_base_pos.anchor.g_arr[1].ciStyleG;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2=pSchemaBolt->des_base_pos.anchor.g_arr[1].g;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.cProjectPlane=pSchemaBolt->des_base_pos.anchor.ciIntersPlane;
	}
	CSchemaRod* pMatchBaseRod=hashLinkRods.GetValue(pSchemaBolt->des_base_pos.idResideAngle);
	pBolt->des_base_pos.hPart=pMatchBaseRod->hMatchPole;
	pBolt->des_base_pos.offset_wing=pSchemaBolt->des_base_pos.ciWorkWing;
	pBolt->des_base_pos.cLocationStyle=pSchemaBolt->des_base_pos.cLocationStyle;
	pBolt->des_base_pos.direction=pSchemaBolt->des_base_pos.ciDirection;
	pBolt->des_base_pos.len_offset_dist=pSchemaBolt->des_base_pos.len_offset_dist;
	pBolt->des_base_pos.m_biWingOffsetStyle=pSchemaBolt->des_base_pos.ciWingOffsetStyle;
	pBolt->des_base_pos.wing_offset_dist=pSchemaBolt->des_base_pos.wing_offset_dist;
	//����
	CSchemaRod* pMatchViceRod=hashLinkRods.GetValue(pSchemaBolt->normal.idResideAngle);
	pBolt->des_work_norm.hVicePart=pMatchViceRod->hMatchPole;
	pBolt->des_work_norm.norm_style=1;
	pBolt->des_work_norm.direction=pSchemaBolt->normal.ciDirection;
	pBolt->des_work_norm.norm_wing=pSchemaBolt->normal.ciWorkWing;
	return true;
}
//
GECS CDeepClonePlate::CalACS(CSchemaRod* pDatumSchemaRod)
{
	GECS acs;
	SmartRodPtr pDatumRod=(CLDSLinePart*)m_pTower->FromPartHandle(pDatumSchemaRod->hMatchPole,CLS_LINEPART);
	GEPOINT vWorkPlaneNorm,vStdDatumLenVec=pDatumRod->End()-pDatumRod->Start();
	normalize(vStdDatumLenVec);
	for(CSchemaRod* pSchemaRod=hashLinkRods.GetFirst();pSchemaRod;pSchemaRod=hashLinkRods.GetNext())
	{
		if(pSchemaRod->siPlaneNo!=1&&pSchemaRod->vec_type!=0)
			continue;
		if(pSchemaRod->pMatchLinePart!=NULL)
		{
			GEPOINT vRayVec=pSchemaRod->pMatchLinePart->End()-pSchemaRod->pMatchLinePart->Start();
			normalize(vRayVec);
			vWorkPlaneNorm=vRayVec^vStdDatumLenVec;
			normalize(vWorkPlaneNorm);
			break;
		}
	}
	GEPOINT dynScrNorm=CLDSObject::_console->TransVFromScr(GEPOINT(0,0,-1));
	if(vWorkPlaneNorm.IsZero()&&pDatumRod->IsAngle())
	{	//���ݵ�ǰ��ͼ����ȷ����ǰ����֫
		if(fabs(pDatumRod.pAngle->vxWingNorm*dynScrNorm)>fabs(pDatumRod.pAngle->vyWingNorm*dynScrNorm))
			pDatumSchemaRod->angle.ciWorkWingX0Y1=0;
		else
			pDatumSchemaRod->angle.ciWorkWingX0Y1=1;
	}
	else if(pDatumRod->IsAngle())
	{
		if(fabs(pDatumRod.pAngle->vxWingNorm*vWorkPlaneNorm)>fabs(pDatumRod.pAngle->vyWingNorm*vWorkPlaneNorm))
			pDatumSchemaRod->angle.ciWorkWingX0Y1=0;
		else
			pDatumSchemaRod->angle.ciWorkWingX0Y1=1;
	}
	else if(!vWorkPlaneNorm.IsZero())
	{
		acs.axis_z=vWorkPlaneNorm;
		if(acs.axis_z*dynScrNorm<0)
			acs.axis_z=-1.0;
	}
	if(pDatumRod->IsAngle())
		acs.axis_z=pDatumSchemaRod->angle.ciWorkWingX0Y1==0?pDatumRod.pAngle->vxWingNorm:pDatumRod.pAngle->vyWingNorm;
	if(ciDatumAxis!='X')
	{
		acs.axis_y=this->ciDirectionAxisY==0?vStdDatumLenVec:-vStdDatumLenVec;
		acs.axis_x=acs.axis_y^acs.axis_z;
	}
	else
	{	//���������ϴ��ڣ��������������
		acs.axis_x=this->ciDirectionAxisY==0?vStdDatumLenVec:-vStdDatumLenVec;
		acs.axis_y=acs.axis_z^acs.axis_x;
	}
	return acs;
}
void CDeepClonePlate::UpdatePlateDesUcs(CLDSPlate* pNewPlate)
{
	if(pNewPlate==NULL)
		return;
	CLDSLinePart *pDatumRod=(CLDSLinePart*)m_pTower->FromPartHandle(m_pDestNode->hFatherPart,CLS_LINEPART);
	CSchemaRod* pDatumSchemaRod=hashLinkRods.GetValue(GetSchemaRodId(pDatumRod->handle,TRUE));
	pNewPlate->designInfo.origin.datum_pos_style=2;
	pNewPlate->designInfo.origin.des_para.RODNODE.hNode=m_pDestNode->handle;
	pNewPlate->designInfo.origin.des_para.RODNODE.hRod=pDatumRod->handle;
	pNewPlate->designInfo.origin.des_para.RODNODE.wing_offset_style=origin.g_arr[0].ciStyleG;
	pNewPlate->designInfo.origin.des_para.RODNODE.wing_offset_dist=origin.g_arr[0].g;
	pNewPlate->designInfo.origin.des_para.RODNODE.direction=origin.ciLenDirection;
	pNewPlate->designInfo.origin.des_para.RODNODE.len_offset_dist=origin.siLenOffset;
	pNewPlate->designInfo.norm.norm_style=1;
	pNewPlate->designInfo.norm.direction=ciDirectionAxisZ;
	pNewPlate->designInfo.norm.hVicePart=pDatumRod->handle;
	pNewPlate->designInfo.norm.norm_wing=pDatumSchemaRod->angle.ciWorkWingX0Y1;
	CDesignLjPartPara *pLjPartPara=pNewPlate->designInfo.FromHandle(pNewPlate->designInfo.norm.hVicePart);
	if(pLjPartPara)
	{
		pNewPlate->designInfo.origin.des_para.RODNODE.offset_wing=pLjPartPara->angle.cur_wing_x0_y1;
		pNewPlate->designInfo.norm.norm_wing=pLjPartPara->angle.cur_wing_x0_y1;
	}
	if(pNewPlate->face_N>1)
	{
		for(int i=0;i<2;i++)
		{
			pNewPlate->designInfo.huoquline_end[i].datum_pos_style=pNewPlate->designInfo.huoquline_start[0].datum_pos_style=2;
			pNewPlate->designInfo.huoquline_start[i].des_para.RODNODE.hNode=pNewPlate->designInfo.huoquline_end[i].des_para.RODNODE.hNode=m_pDestNode->handle;
			pNewPlate->designInfo.huoquline_start[i].des_para.RODNODE.hRod=pNewPlate->designInfo.huoquline_end[i].des_para.RODNODE.hRod=pDatumRod->handle;
			pNewPlate->designInfo.huoquline_start[i].des_para.RODNODE.offset_wing=pNewPlate->designInfo.norm.norm_wing;
			pNewPlate->designInfo.huoquline_end[i].des_para.RODNODE.offset_wing=pNewPlate->designInfo.norm.norm_wing;
			pNewPlate->designInfo.huoquline_start[i].des_para.RODNODE.wing_offset_style=pNewPlate->designInfo.huoquline_end[i].des_para.RODNODE.wing_offset_style=huoqu_pt[i].g_arr[0].ciStyleG;
			pNewPlate->designInfo.huoquline_start[i].des_para.RODNODE.wing_offset_dist=pNewPlate->designInfo.huoquline_end[i].des_para.RODNODE.wing_offset_dist=huoqu_pt[i].g_arr[0].g;
			pNewPlate->designInfo.huoquline_start[i].des_para.RODNODE.direction=pNewPlate->designInfo.huoquline_end[i].des_para.RODNODE.direction=huoqu_pt[i].ciLenDirection;
			pNewPlate->designInfo.huoquline_start[i].des_para.RODNODE.len_offset_dist=huoqu_pt[i].siLenOffset;
			pNewPlate->designInfo.huoquline_end[i].des_para.RODNODE.len_offset_dist=huoqu_pt[i].siLenOffset*(-1);
			//�����淨��
			pNewPlate->designInfo.huoqufacenorm[i].norm_style=2;
			CSchemaRod* pViceSchemaRod=hashLinkRods.GetValue(huoqu_norm[i].m_idViceRod);
			if(pViceSchemaRod)
				pNewPlate->designInfo.huoqufacenorm[i].hVicePart=pViceSchemaRod->hMatchPole;
			CSchemaRod* pCrosSchemaRod=hashLinkRods.GetValue(huoqu_norm[i].m_idCrossRod);
			if(pCrosSchemaRod)
				pNewPlate->designInfo.huoqufacenorm[i].hCrossPart=pCrosSchemaRod->hMatchPole;
		}
	}
	pNewPlate->DesignSetupUcs();
}
CLDSPlate *CDeepClonePlate::CreatePlate(PARTSET& dispPartSet,IModel *pTower/*=NULL*/)
{
	if(m_pDestNode==NULL)
		return NULL;
	if(m_pSrcPlate)
		return CClonePlate::CreatePlate(dispPartSet);
	//����ģ�����������¸ְ�
	CLDSLinePart *pDatumRod=(CLDSLinePart*)m_pTower->FromPartHandle(m_pDestNode->hFatherPart,CLS_LINEPART);
	CSchemaRod* pDatumSchemaRod=hashLinkRods.GetValue(GetSchemaRodId(pDatumRod->handle,TRUE));
	if(pDatumRod==NULL||pDatumSchemaRod==NULL||pDatumSchemaRod->m_idClassType!=pDatumRod->GetClassTypeId())
		return NULL;
	CLDSPlate *pPlate=(CLDSPlate*)m_pTower->AppendPart(CLS_PLATE);
	pPlate->CopyModuleInstanceInfo(pDatumRod);
	pPlate->iSeg=pDatumRod->iSeg;
	pPlate->Thick=m_nThick;
	pPlate->cMaterial=m_cMateria;
	pPlate->face_N=m_nPlaneN;
	pPlate->jdb_style=1;
	pPlate->designInfo.iFaceType=m_nPlaneN;
	pPlate->designInfo.m_bEnableFlexibleDesign=m_bFlexibleDesign;
	pPlate->designInfo.iProfileStyle0123=m_ciProfileSty;
	pPlate->designInfo.m_hBaseNode=m_pDestNode->handle;
	pPlate->designInfo.m_hBasePart=pDatumRod->handle;
	pPlate->ucs=CalACS(pDatumSchemaRod);
	//���¸ְ���Ʋ���
	for(CSchemaRod* pSchemaRod=hashLinkRods.GetFirst();pSchemaRod;pSchemaRod=hashLinkRods.GetNext())
	{
		SmartRodPtr pMatchRod=(CLDSLinePart*)m_pTower->FromPartHandle(pSchemaRod->hMatchPole,pSchemaRod->m_idClassType);
		if(pMatchRod.IsNULL() || pMatchRod->pStart==NULL || pMatchRod->pEnd==NULL)
			continue;
		CDesignLjPartPara *pLjPartPara=pPlate->designInfo.partList.Add(pMatchRod->handle);
		pLjPartPara->pCurLinePart=pMatchRod;
		pLjPartPara->m_nClassTypeId=pSchemaRod->m_idClassType;
		pLjPartPara->iFaceNo=pSchemaRod->siPlaneNo;
		pLjPartPara->start0_end1=pSchemaRod->ciConnectTipS0E1;
		if(pMatchRod->pStart==m_pDestNode)
		{
			pLjPartPara->start0_end1=0;
			pMatchRod->desStartOdd.m_iOddCalStyle=pSchemaRod->odd.ciOddCalStyle;
			pMatchRod->desStartOdd.m_fCollideDist=pSchemaRod->odd.fCollideDist;
			CSchemaRod* pRefSchemaRod1=hashLinkRods.GetValue(pSchemaRod->odd.hOddRefRod1);
			if(pRefSchemaRod1)
				pMatchRod->desStartOdd.m_hRefPart1=pRefSchemaRod1->hMatchPole;
			CSchemaRod* pRefSchemaRod2=hashLinkRods.GetValue(pSchemaRod->odd.hOddRefRod2);
			if(pRefSchemaRod2)
				pMatchRod->desStartOdd.m_hRefPart2=pRefSchemaRod2->hMatchPole;
			pMatchRod->CalStartOddment();
		}
		else if(pMatchRod->pEnd==m_pDestNode)
		{	
			pLjPartPara->start0_end1=1;
			pMatchRod->desEndOdd.m_iOddCalStyle=pSchemaRod->odd.ciOddCalStyle;
			pMatchRod->desEndOdd.m_fCollideDist=pSchemaRod->odd.fCollideDist;
			CSchemaRod* pRefSchemaRod1=hashLinkRods.GetValue(pSchemaRod->odd.hOddRefRod1);
			if(pRefSchemaRod1)
				pMatchRod->desEndOdd.m_hRefPart1=pRefSchemaRod1->hMatchPole;
			CSchemaRod* pRefSchemaRod2=hashLinkRods.GetValue(pSchemaRod->odd.hOddRefRod2);
			if(pRefSchemaRod2)
				pMatchRod->desEndOdd.m_hRefPart2=pRefSchemaRod2->hMatchPole;
			pMatchRod->CalEndOddment();
		}
		pLjPartPara->end_space=pSchemaRod->siEndSpace;
		pLjPartPara->ber_space=pSchemaRod->siBerSpace;
		pLjPartPara->wing_space=pSchemaRod->siWingSpace;
		if(pMatchRod->IsAngle())
		{	//�Ǹֵĵ�ǰ����֫Ӧʵʱ�ж�
			if(fabs(pPlate->ucs.axis_z*pMatchRod.pAngle->vxWingNorm)>fabs(pPlate->ucs.axis_z*pMatchRod.pAngle->vyWingNorm))
				pLjPartPara->angle.cur_wing_x0_y1=0;	//pSchemaRod->angle.ciWorkWingX0Y1;
			else
				pLjPartPara->angle.cur_wing_x0_y1=1;
			pLjPartPara->angle.bTwoEdge=pSchemaRod->angle.bDualEdge;
			pLjPartPara->angle.bEndLjJg=pSchemaRod->angle.bEndLjAngle;
			pLjPartPara->angle.cbSpaceFlag=pSchemaRod->angle.cbSpaceFlag;
		}
	}
	//�������߸˼�ƫ����
	for(CDesignLjPartPara* pLjPartPara=pPlate->GetFirstLjPara();pLjPartPara;pLjPartPara=pPlate->GetNextLjPara())
	{	
		if(pLjPartPara->pCurLinePart==pDatumRod||pLjPartPara->pCurLinePart->IsAngle()==false)
			continue;
		CMultiOffsetPos *pDesPosPara=NULL;	
		CLDSLineAngle* pRayJg=(CLDSLineAngle*)pLjPartPara->pCurLinePart;
		if(pLjPartPara->start0_end1==0)
			pDesPosPara=&pRayJg->desStartPos; 
		else
			pDesPosPara=&pRayJg->desEndPos;
		if(pLjPartPara->angle.cur_wing_x0_y1==0)
		{	
			pDesPosPara->wing_x_offset.gStyle=4;
			pDesPosPara->wing_y_offset.gStyle=0;
			if(pPlate->ucs.axis_z*pRayJg->vxWingNorm>0)
				pDesPosPara->wing_x_offset.offsetDist=pLjPartPara->angle.bEndLjJg?-pDatumRod->GetThick():0;
			else 
				pDesPosPara->wing_x_offset.offsetDist=-pPlate->GetThick();
		}
		else
		{
			pDesPosPara->wing_x_offset.gStyle=0;
			pDesPosPara->wing_y_offset.gStyle=4;
			if(pPlate->ucs.axis_z*pRayJg->vyWingNorm>0)
				pDesPosPara->wing_y_offset.offsetDist=pLjPartPara->angle.bEndLjJg?-pDatumRod->GetThick():0;
			else
				pDesPosPara->wing_y_offset.offsetDist=-pPlate->GetThick();
		}
	}
	//���¸ְ�λ����Ʋ����Լ�������Ʋ���
	UpdatePlateDesUcs(pPlate);
	//������˨��Ϣ
	for(CSchemaBolt* pSchemaBolt=hashTemplBolts.GetFirst();pSchemaBolt;pSchemaBolt=hashTemplBolts.GetNext())
	{
		CLDSBolt *pBolt=(CLDSBolt*)m_pTower->AppendPart(CLS_BOLT);
		pBolt->iSeg=pPlate->iSeg;
		UpdateBoltBySchema(pSchemaBolt,pBolt);
		//
		if(pBolt->des_base_pos.datumPoint.datum_pos_style==2)	//�˼��ϵĽڵ�Ϊ��׼
		{
			pLjPartPara=pPlate->designInfo.FromHandle(pBolt->des_base_pos.datumPoint.des_para.RODNODE.hRod);
			if(pLjPartPara)
				pBolt->des_base_pos.datumPoint.des_para.RODNODE.offset_wing=pLjPartPara->angle.cur_wing_x0_y1;
		}
		else if(pBolt->des_base_pos.datumPoint.datum_pos_style==1)//�Ǹ��������Ϊ��׼
		{
			pLjPartPara=pPlate->designInfo.FromHandle(pBolt->des_base_pos.datumPoint.des_para.RODEND.hRod);
			if(pLjPartPara)
			{
				pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pLjPartPara->angle.cur_wing_x0_y1;
				pBolt->des_base_pos.datumPoint.des_para.RODEND.direction=pLjPartPara->start0_end1; //�������췽��
				pBolt->des_base_pos.direction=(BYTE)pLjPartPara->start0_end1;
			}
		}
		pLjPartPara=pPlate->designInfo.FromHandle(pBolt->des_base_pos.hPart);
		if(pLjPartPara)
			pBolt->des_base_pos.offset_wing=(BYTE)pLjPartPara->angle.cur_wing_x0_y1;
		pLjPartPara=pPlate->designInfo.FromHandle(pBolt->des_work_norm.hVicePart);
		if(pLjPartPara)
			pBolt->des_work_norm.norm_wing=(BYTE)pLjPartPara->angle.cur_wing_x0_y1;
		pBolt->correct_worknorm();
		//���ݸְ巨�߷��������˨���߷��� һ������¸ְ巨�߷�������˨���߷��򱣳�һ��
		if(pBolt->ucs.axis_z*pPlate->ucs.axis_z<0)
			pBolt->des_work_norm.direction=1-pBolt->des_work_norm.direction;
		//�����˨�ᴩ����������ʼ��ͨ��
		pPlate->AppendLsRef(pBolt->GetLsRef());
		pBolt->AddL0Thick(pPlate->handle,TRUE);
		for(int i=0;i<6;i++)
		{
			if(pSchemaBolt->through.items[i].ciItemType==0)
				continue;
			if(pSchemaBolt->through.items[i].ciItemType==1)
			{
				CSchemaRod* pSchemaRod=hashLinkRods.GetValue(pSchemaBolt->through.items[i].ciThickVal);
				if(pSchemaRod)
				{
					pSchemaRod->pMatchLinePart->AppendMidLsRef(pBolt->GetLsRef());
					pBolt->AddL0Thick(pSchemaRod->pMatchLinePart->handle,TRUE);
				}
			}
			else if(pSchemaBolt->through.items[i].ciItemType==2)
				pBolt->AddL0Thick(pSchemaBolt->through.items[i].ciThickVal);
		}
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		pBolt->CalGuigeAuto();
		pBolt->SetModified();
	}
	pPlate->DesignPlate();	//������Ƹְ�
	return pPlate;
}
#ifdef __LDS_CONTEXT_
bool CDeepClonePlate::ImportTemplChildXmlNode(CMarkup xml,const XmlNode& xmlnode)
{
	CString sValue=_T("");
	while(xml.FindChildElem(xmlnode.sElemName))   
	{ 
		xml.IntoElem();
		if(stricmp(xmlnode.sElemName,"BasicParams")==0)
		{
			sValue=xml.GetAttrib("thick");
			m_nThick=atoi(sValue);
			sValue=xml.GetAttrib("material");
			m_cMateria=sValue[0];
			sValue=xml.GetAttrib("planeN");
			m_nPlaneN=atoi(sValue);
			sValue=xml.GetAttrib("datumRodId");
			m_idDatumRod=atoi(sValue);
			sValue=xml.GetAttrib("datumAxis");
			ciDatumAxis=sValue.GetString()!=NULL?*sValue.GetString():'Y';
			sValue=xml.GetAttrib("FlexibleDesign");
			m_bFlexibleDesign=atoi(sValue);
			sValue=xml.GetAttrib("ProfileSty");
			m_ciProfileSty=atoi(sValue);
		}
		else if(stricmp(xmlnode.sElemName,"DesPostion")==0)
		{
			sValue=xml.GetAttrib("StyleG");
			origin.g_arr[0].ciStyleG=atoi(sValue);
			sValue=xml.GetAttrib("WingOffset");
			origin.g_arr[0].g=atoi(sValue);
			sValue=xml.GetAttrib("Direction");
			origin.ciLenDirection=atoi(sValue);
			sValue=xml.GetAttrib("LenOffset");
			origin.siLenOffset=atoi(sValue);
			if(m_nPlaneN>1)
				ImportTemplChildXmlNode(xml,XmlNode("HuoquInfo1"));
			if(m_nPlaneN==3)
				ImportTemplChildXmlNode(xml,XmlNode("HuoquInfo2"));
		}
		else if(stricmp(xmlnode.sElemName,"HuoquInfo1")==0)
		{
			sValue=xml.GetAttrib("StyleG");
			huoqu_pt[0].g_arr[0].ciStyleG=atoi(sValue);
			sValue=xml.GetAttrib("WingOffset");
			huoqu_pt[0].g_arr[0].g=atoi(sValue);
			sValue=xml.GetAttrib("Direction");
			huoqu_pt[0].ciLenDirection=atoi(sValue);
			sValue=xml.GetAttrib("LenOffset");
			huoqu_pt[0].siLenOffset=atoi(sValue);
			sValue=xml.GetAttrib("norm.ViceRod");
			huoqu_norm[0].m_idViceRod=atoi(sValue);
			sValue=xml.GetAttrib("norm.CrossRod");
			huoqu_norm[0].m_idCrossRod=atoi(sValue);
		}
		else if(stricmp(xmlnode.sElemName,"HuoquInfo2")==0)
		{
			sValue=xml.GetAttrib("StyleG");
			huoqu_pt[1].g_arr[0].ciStyleG=atoi(sValue);
			sValue=xml.GetAttrib("WingOffset");
			huoqu_pt[1].g_arr[0].g=atoi(sValue);
			sValue=xml.GetAttrib("Direction");
			huoqu_pt[1].ciLenDirection=atoi(sValue);
			sValue=xml.GetAttrib("LenOffset");
			huoqu_pt[1].siLenOffset=atoi(sValue);
			sValue=xml.GetAttrib("norm.ViceRod");
			huoqu_norm[1].m_idViceRod=atoi(sValue);
			sValue=xml.GetAttrib("norm.CrossRod");
			huoqu_norm[1].m_idCrossRod=atoi(sValue);
		}
		else if(stricmp(xmlnode.sElemName,"SchemaRodSet")==0)
			ImportTemplChildXmlNode(xml,XmlNode("SchemaRod"));
		else if(stricmp(xmlnode.sElemName,"SchemaBoltSet")==0)
			ImportTemplChildXmlNode(xml,XmlNode("SchemaBolt"));
		else if(stricmp(xmlnode.sElemName,"SchemaRod")==0)
		{
			sValue=xml.GetAttrib("Key");
			CSchemaRod* pSchemRod=hashLinkRods.Add(atoi(sValue));
			sValue=xml.GetAttrib("IdClass");	//�˼�����
			pSchemRod->m_idClassType=atoi(sValue);
			sValue=xml.GetAttrib("VecType");	//����λ��
			pSchemRod->vec_type=atoi(sValue);
			sValue=xml.GetAttrib("OffAngle");	//��׼�н�
			pSchemRod->offset_angle=atof(sValue);
			sValue=xml.GetAttrib("LinkSE");		//���Ӷ�
			pSchemRod->ciConnectTipS0E1=atoi(sValue);
			sValue=xml.GetAttrib("PlaneNo");	//�������
			pSchemRod->siPlaneNo=atoi(sValue);
			sValue=xml.GetAttrib("BerSpace");	//�����
			pSchemRod->siBerSpace=atoi(sValue);
			sValue=xml.GetAttrib("EndSpace");	//��ͷ���
			pSchemRod->siEndSpace=atoi(sValue);
			sValue=xml.GetAttrib("WingSpace");	//֫����
			pSchemRod->siWingSpace=atoi(sValue);
			//
			ImportTemplChildXmlNode(xml,XmlNode("LineAngle",pSchemRod));
			ImportTemplChildXmlNode(xml,XmlNode("OddInfo",pSchemRod));
		}
		else if(stricmp(xmlnode.sElemName,"LineAngle")==0)
		{
			CSchemaRod* pSchemaRod=(CSchemaRod*)xmlnode.pRelaObj;
			if(pSchemaRod==NULL)
				return false;
			sValue=xml.GetAttrib("WorkWing");
			pSchemaRod->angle.ciWorkWingX0Y1=atoi(sValue);
			sValue=xml.GetAttrib("DualEdge");
			pSchemaRod->angle.bDualEdge=atoi(sValue);
			sValue=xml.GetAttrib("EndLj");
			pSchemaRod->angle.bEndLjAngle=atoi(sValue);
			sValue=xml.GetAttrib("SpaceFlag");
			pSchemaRod->angle.cbSpaceFlag=ANGLE_SPACE_FLAG(atoi(sValue));
		}
		else if(stricmp(xmlnode.sElemName,"OddInfo")==0)
		{
			CSchemaRod* pSchemaRod=(CSchemaRod*)xmlnode.pRelaObj;
			if(pSchemaRod==NULL)
				return false;
			sValue=xml.GetAttrib("OddCalStyle");
			pSchemaRod->odd.ciOddCalStyle=atoi(sValue);
			sValue=xml.GetAttrib("CollideDist");
			pSchemaRod->odd.fCollideDist=atof(sValue);
			sValue=xml.GetAttrib("OddRefRod1");
			pSchemaRod->odd.hOddRefRod1=atoi(sValue);			
			sValue=xml.GetAttrib("OddRefRod2");
			pSchemaRod->odd.hOddRefRod2=atoi(sValue);
		}
		else if(stricmp(xmlnode.sElemName,"SchemaBolt")==0)
		{	//������˨
			sValue=xml.GetAttrib("Key");
			CSchemaBolt* pSchemaBolt=hashTemplBolts.Add(atoi(sValue));
			sValue=xml.GetAttrib("BoltD");
			pSchemaBolt->d=atoi(sValue);
			sValue=xml.GetAttrib("Grade");
			strcpy(pSchemaBolt->grade,sValue);
			sValue=xml.GetAttrib("L0");
			pSchemaBolt->InitThrough(sValue);
			//
			ImportTemplChildXmlNode(xml,XmlNode("DesignPos",pSchemaBolt));
			ImportTemplChildXmlNode(xml,XmlNode("Norm",pSchemaBolt));
		}
		else if(stricmp(xmlnode.sElemName,"DesignPos")==0)
		{
			CSchemaBolt* pSchemaBolt=(CSchemaBolt*)xmlnode.pRelaObj;
			if(pSchemaBolt==NULL)
				return false;
			sValue=xml.GetAttrib("IdReside");
			pSchemaBolt->des_base_pos.idResideAngle=atoi(sValue);
			sValue=xml.GetAttrib("WorkWing");
			pSchemaBolt->des_base_pos.ciWorkWing=atoi(sValue);
			sValue=xml.GetAttrib("LocationS");
			pSchemaBolt->des_base_pos.cLocationStyle=atoi(sValue);
			sValue=xml.GetAttrib("Direction");
			pSchemaBolt->des_base_pos.ciDirection=atoi(sValue);
			sValue=xml.GetAttrib("LenOffset");
			pSchemaBolt->des_base_pos.len_offset_dist=atoi(sValue);
			sValue=xml.GetAttrib("WingOffStyle");
			pSchemaBolt->des_base_pos.ciWingOffsetStyle=atoi(sValue);
			sValue=xml.GetAttrib("WingOffset");
			pSchemaBolt->des_base_pos.wing_offset_dist=atoi(sValue);
			//
			ImportTemplChildXmlNode(xml,XmlNode("Anchor",pSchemaBolt));
		}
		else if(stricmp(xmlnode.sElemName,"Anchor")==0)
		{
			CSchemaBolt* pSchemaBolt=(CSchemaBolt*)xmlnode.pRelaObj;
			if(pSchemaBolt==NULL)
				return false;
			sValue=xml.GetAttrib("AnchorType");
			pSchemaBolt->des_base_pos.anchor.ciAnchorType=atoi(sValue);
			sValue=xml.GetAttrib("IdFirst");
			pSchemaBolt->des_base_pos.anchor.idPriorAngle=atoi(sValue);
			sValue=xml.GetAttrib("Type1");
			pSchemaBolt->des_base_pos.anchor.g_arr[0].ciStyleG=atoi(sValue);
			sValue=xml.GetAttrib("OffG1");
			pSchemaBolt->des_base_pos.anchor.g_arr[0].g=atoi(sValue);
			if(pSchemaBolt->des_base_pos.anchor.ciAnchorType==1)
			{
				sValue=xml.GetAttrib("OddEffect");
				pSchemaBolt->des_base_pos.anchor.bIncOddEffect=atoi(sValue);
			}
			else if(pSchemaBolt->des_base_pos.anchor.ciAnchorType==2)
			{
				sValue=xml.GetAttrib("IntersPlane");
				pSchemaBolt->des_base_pos.anchor.ciIntersPlane=atoi(sValue);
				sValue=xml.GetAttrib("PlaneNormalX");
				pSchemaBolt->des_base_pos.anchor.vPlaneNormal.x=atof(sValue);
				sValue=xml.GetAttrib("PlaneNormalY");
				pSchemaBolt->des_base_pos.anchor.vPlaneNormal.y=atof(sValue);
				sValue=xml.GetAttrib("PlaneNormalZ");
				pSchemaBolt->des_base_pos.anchor.vPlaneNormal.z=atof(sValue);
			}
			else if(pSchemaBolt->des_base_pos.anchor.ciAnchorType==3)
			{
				sValue=xml.GetAttrib("IdSecond");
				pSchemaBolt->des_base_pos.anchor.idSecondaryAngle=atoi(sValue);
				sValue=xml.GetAttrib("Type2");
				pSchemaBolt->des_base_pos.anchor.g_arr[1].ciStyleG=atoi(sValue);
				sValue=xml.GetAttrib("OffG2");
				pSchemaBolt->des_base_pos.anchor.g_arr[1].g=atoi(sValue);
			}
		}
		else if(stricmp(xmlnode.sElemName,"Norm")==0)
		{
			CSchemaBolt* pSchemaBolt=(CSchemaBolt*)xmlnode.pRelaObj;
			if(pSchemaBolt==NULL)
				return false;
			sValue=xml.GetAttrib("NormStyle");
			pSchemaBolt->normal.ciNormStyle=atoi(sValue);
			sValue=xml.GetAttrib("NormDirection");
			pSchemaBolt->normal.ciDirection=atoi(sValue);
			sValue=xml.GetAttrib("NormWorkWing");
			pSchemaBolt->normal.ciWorkWing=atoi(sValue);
			sValue=xml.GetAttrib("NormIdReside");
			pSchemaBolt->normal.idResideAngle=atoi(sValue);
		}
		xml.OutOfElem();
	}
	return true;
}
BOOL CDeepClonePlate::FromXmlSchemaFile(const char* filename)
{
	CMarkup xml;
	if(!xml.Load(filename))
		return FALSE;
	xml.ResetMainPos();
	ImportTemplChildXmlNode(xml,XmlNode("BasicParams"));
	ImportTemplChildXmlNode(xml,XmlNode("SchemaRodSet"));
	ImportTemplChildXmlNode(xml,XmlNode("DesPostion"));
	ImportTemplChildXmlNode(xml,XmlNode("SchemaBoltSet"));
	return TRUE;
}
BOOL CDeepClonePlate::ToXmlSchemaFile(const char* filename)
{
	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("PLATE");
	xml.AddAttrib("Type",(char*)CXhChar16("node-plate"));
	//������Ϣ
	xml.IntoElem();
	xml.AddElem("BasicParams");
	xml.AddAttrib("thick",(char*)CXhChar16("%d",m_nThick));
	xml.AddAttrib("material",(char*)CXhChar16("%c",m_cMateria));
	xml.AddAttrib("planeN",(char*)CXhChar16("%d",m_nPlaneN));
	xml.AddAttrib("datumRodId",(char*)CXhChar16("%d",m_idDatumRod));
	xml.AddAttrib("datumAxis","Y");
	xml.AddAttrib("FlexibleDesign",(char*)CXhChar16("%d",m_bFlexibleDesign));
	xml.AddAttrib("ProfileSty",(char*)CXhChar16("%d",m_ciProfileSty));
	xml.OutOfElem();
	//������Ӹ˼�����
	xml.IntoElem();
	xml.AddElem("SchemaRodSet");
	for(CSchemaRod* pRod=hashLinkRods.GetFirst();pRod;pRod=hashLinkRods.GetNext())
	{	//���Ӹ˼���Ϣ
		xml.IntoElem();
		xml.AddElem("SchemaRod");
		xml.AddAttrib("Key",(char*)CXhChar16("%d",pRod->Id));
		xml.AddAttrib("IdClass",(char*)CXhChar16("%d",pRod->m_idClassType));
		xml.AddAttrib("VecType",(char*)CXhChar16("%d",pRod->vec_type));
		xml.AddAttrib("OffAngle",(char*)CXhChar16("%f",pRod->offset_angle));
		xml.AddAttrib("LinkSE",(char*)CXhChar16("%d",pRod->ciConnectTipS0E1));
		xml.AddAttrib("PlaneNo",(char*)CXhChar16("%d",pRod->siPlaneNo));
		xml.AddAttrib("BerSpace",(char*)CXhChar16("%d",pRod->siBerSpace));
		xml.AddAttrib("EndSpace",(char*)CXhChar16("%d",pRod->siEndSpace));
		xml.AddAttrib("WingSpace",(char*)CXhChar16("%d",pRod->siWingSpace));
		xml.IntoElem();
		xml.AddElem("LineAngle");
		xml.AddAttrib("WorkWing",(char*)CXhChar16("%d",pRod->angle.ciWorkWingX0Y1));
		xml.AddAttrib("DualEdge",(char*)CXhChar16("%d",pRod->angle.bDualEdge));
		xml.AddAttrib("EndLj",(char*)CXhChar16("%d",pRod->angle.bEndLjAngle));
		xml.AddAttrib("SpaceFlag",(char*)CXhChar16("%d",pRod->angle.cbSpaceFlag));
		xml.OutOfElem();
		xml.IntoElem();
		xml.AddElem("OddInfo");
		xml.AddAttrib("OddCalStyle",(char*)CXhChar16("%d",pRod->odd.ciOddCalStyle));
		xml.AddAttrib("CollideDist",(char*)CXhChar16("%f",pRod->odd.fCollideDist));
		xml.AddAttrib("OddRefRod1",(char*)CXhChar16("%d",pRod->odd.hOddRefRod1));
		xml.AddAttrib("OddRefRod2",(char*)CXhChar16("%d",pRod->odd.hOddRefRod2));
		xml.OutOfElem();
		xml.OutOfElem();
	}
	xml.OutOfElem();
	//���������Ʋ���
	xml.IntoElem();
	xml.AddElem("DesPostion");
	xml.AddAttrib("StyleG",(char*)CXhChar16("%d",origin.g_arr[0].ciStyleG));
	xml.AddAttrib("WingOffset",(char*)CXhChar16("%d",origin.g_arr[0].g));
	xml.AddAttrib("Direction",(char*)CXhChar16("%d",origin.ciLenDirection));
	xml.AddAttrib("LenOffset",(char*)CXhChar16("%d",origin.siLenOffset));
	if(m_nPlaneN>1)
	{
		xml.IntoElem();
		xml.AddElem("HuoquInfo1");
		xml.AddAttrib("StyleG",(char*)CXhChar16("%d",huoqu_pt[0].g_arr[0].ciStyleG));
		xml.AddAttrib("WingOffset",(char*)CXhChar16("%d",huoqu_pt[0].g_arr[0].g));
		xml.AddAttrib("Direction",(char*)CXhChar16("%d",huoqu_pt[0].ciLenDirection));
		xml.AddAttrib("LenOffset",(char*)CXhChar16("%d",huoqu_pt[0].siLenOffset));
		xml.AddAttrib("norm.ViceRod",(char*)CXhChar16("%d",huoqu_norm[0].m_idViceRod));
		xml.AddAttrib("norm.CrossRod",(char*)CXhChar16("%d",huoqu_norm[0].m_idCrossRod));
		xml.OutOfElem();
		if(m_nPlaneN==3)
		{
			xml.IntoElem();
			xml.AddElem("HuoquInfo2");
			xml.AddAttrib("StyleG",(char*)CXhChar16("%d",huoqu_pt[1].g_arr[0].ciStyleG));
			xml.AddAttrib("WingOffset",(char*)CXhChar16("%d",huoqu_pt[1].g_arr[0].g));
			xml.AddAttrib("Direction",(char*)CXhChar16("%d",huoqu_pt[1].ciLenDirection));
			xml.AddAttrib("LenOffset",(char*)CXhChar16("%d",huoqu_pt[1].siLenOffset));
			xml.AddAttrib("norm.ViceRod",(char*)CXhChar16("%d",huoqu_norm[1].m_idViceRod));
			xml.AddAttrib("norm.CrossRod",(char*)CXhChar16("%d",huoqu_norm[1].m_idCrossRod));
			xml.OutOfElem();
		}
	}
	xml.OutOfElem();
	//���������˨����
	xml.IntoElem();
	xml.AddElem("SchemaBoltSet");
	for(CSchemaBolt* pBolt=hashTemplBolts.GetFirst();pBolt;pBolt=hashTemplBolts.GetNext())
	{	//������˨��Ϣ
		xml.IntoElem();
		xml.AddElem("SchemaBolt");
		xml.AddAttrib("Key",(char*)CXhChar16("%d",pBolt->Id));
		xml.AddAttrib("BoltD",(char*)CXhChar16("%d",pBolt->d));
		xml.AddAttrib("Grade",(char*)CXhChar16("%s",pBolt->grade));
		xml.AddAttrib("L0",pBolt->through.ToThickStr());
		xml.IntoElem();
		xml.AddElem("DesignPos");
		xml.AddAttrib("IdReside",(char*)CXhChar16("%d",pBolt->des_base_pos.idResideAngle));
		xml.AddAttrib("WorkWing",(char*)CXhChar16("%d",pBolt->des_base_pos.ciWorkWing));
		xml.AddAttrib("LocationS",(char*)CXhChar16("%d",pBolt->des_base_pos.cLocationStyle));
		xml.AddAttrib("Direction",(char*)CXhChar16("%d",pBolt->des_base_pos.ciDirection));
		xml.AddAttrib("LenOffset",(char*)CXhChar16("%d",pBolt->des_base_pos.len_offset_dist));
		xml.AddAttrib("WingOffStyle",(char*)CXhChar16("%d",pBolt->des_base_pos.ciWingOffsetStyle));
		xml.AddAttrib("WingOffset",(char*)CXhChar16("%d",pBolt->des_base_pos.wing_offset_dist));
		xml.IntoElem();
		xml.AddElem("Anchor");
		xml.AddAttrib("AnchorType",(char*)CXhChar16("%d",pBolt->des_base_pos.anchor.ciAnchorType));
		xml.AddAttrib("IdFirst",(char*)CXhChar16("%d",pBolt->des_base_pos.anchor.idPriorAngle));
		xml.AddAttrib("Type1",(char*)CXhChar16("%d",pBolt->des_base_pos.anchor.g_arr[0].ciStyleG));
		xml.AddAttrib("OffG1",(char*)CXhChar16("%d",pBolt->des_base_pos.anchor.g_arr[0].g));
		if(pBolt->des_base_pos.anchor.ciAnchorType==1)
			xml.AddAttrib("OddEffect",(char*)CXhChar16("%d",pBolt->des_base_pos.anchor.bIncOddEffect));
		else if(pBolt->des_base_pos.anchor.ciAnchorType==2)
		{
			xml.AddAttrib("IntersPlane",(char*)CXhChar16("%d",pBolt->des_base_pos.anchor.ciIntersPlane));
			xml.AddAttrib("PlaneNormalX",(char*)CXhChar16("%f",pBolt->des_base_pos.anchor.vPlaneNormal.x));
			xml.AddAttrib("PlaneNormalY",(char*)CXhChar16("%f",pBolt->des_base_pos.anchor.vPlaneNormal.y));
			xml.AddAttrib("PlaneNormalZ",(char*)CXhChar16("%f",pBolt->des_base_pos.anchor.vPlaneNormal.z));
		}
		else if(pBolt->des_base_pos.anchor.ciAnchorType==3)
		{	//�Ǹ����߽���
			xml.AddAttrib("IdSecond",(char*)CXhChar16("%d",pBolt->des_base_pos.anchor.idSecondaryAngle));
			xml.AddAttrib("Type2",(char*)CXhChar16("%d",pBolt->des_base_pos.anchor.g_arr[1].ciStyleG));
			xml.AddAttrib("OffG2",(char*)CXhChar16("%d",pBolt->des_base_pos.anchor.g_arr[1].g));
		}
		xml.OutOfElem();
		xml.OutOfElem();
		xml.IntoElem();
		xml.AddElem("Norm");
		xml.AddAttrib("NormStyle",(char*)CXhChar16("%d",pBolt->normal.ciNormStyle));
		xml.AddAttrib("NormDirection",(char*)CXhChar16("%d",pBolt->normal.ciDirection));
		xml.AddAttrib("NormWorkWing",(char*)CXhChar16("%d",pBolt->normal.ciWorkWing));
		xml.AddAttrib("NormIdReside",(char*)CXhChar16("%d",pBolt->normal.idResideAngle));
		xml.OutOfElem();
		xml.OutOfElem();
	}
	xml.OutOfElem();	//�˳�<CSchemaBoltSet>
	xml.OutOfElem();	//�˳�<PLATE>
	return xml.Save(filename);
}
//
BOOL CDeepClonePlate::WriteToClipboard()  
{  
	if(!::OpenClipboard(NULL))
		return FALSE;	//�򿪼��а� 
	//׼������
	CBuffer buffer(100000);
	buffer.ClearBuffer();
	buffer.WriteString("ClonePlate");
	buffer.WriteInteger(m_nThick);
	buffer.WriteByte((BYTE)m_cMateria);
	buffer.WriteByte(m_nPlaneN);
	buffer.WriteInteger(m_idDatumRod);
	buffer.WriteInteger(m_bFlexibleDesign);
	buffer.WriteByte(m_ciProfileSty);
	buffer.WriteByte((BYTE)ciDatumAxis);
	buffer.WriteWord(ciDirectionAxisZ);
	buffer.WriteByte(origin.ciLenDirection);
	buffer.WriteWord(origin.siLenOffset);
	buffer.WriteWord(origin.g_arr[0].ciStyleG);
	buffer.WriteWord(origin.g_arr[0].g);
	if(m_nPlaneN>1)
	{
		buffer.WriteByte(huoqu_pt[0].ciLenDirection);
		buffer.WriteWord(huoqu_pt[0].siLenOffset);
		buffer.WriteInteger(huoqu_norm[0].m_idViceRod);
		buffer.WriteInteger(huoqu_norm[0].m_idCrossRod);
		buffer.WriteWord(huoqu_pt[0].g_arr[0].ciStyleG);
		buffer.WriteWord(huoqu_pt[0].g_arr[0].g);
		if(m_nPlaneN==3)
		{
			buffer.WriteByte(huoqu_pt[1].ciLenDirection);
			buffer.WriteWord(huoqu_pt[1].siLenOffset);
			buffer.WriteInteger(huoqu_norm[1].m_idViceRod);
			buffer.WriteInteger(huoqu_norm[1].m_idCrossRod);
			buffer.WriteWord(huoqu_pt[1].g_arr[0].ciStyleG);
			buffer.WriteWord(huoqu_pt[1].g_arr[0].g);
		}
	}
	buffer.WriteInteger(hashLinkRods.GetNodeNum());
	for(CSchemaRod* pRod=hashLinkRods.GetFirst();pRod;pRod=hashLinkRods.GetNext())
	{	//���Ӹ˼���Ϣ
		buffer.WriteInteger(pRod->Id);
		buffer.WriteInteger(pRod->m_idClassType);
		buffer.WriteInteger(pRod->vec_type);
		buffer.WriteDouble(pRod->offset_angle);
		buffer.WriteByte(pRod->ciConnectTipS0E1);
		buffer.WriteWord(pRod->siPlaneNo);
		buffer.WriteWord(pRod->siBerSpace);
		buffer.WriteWord(pRod->siEndSpace);
		buffer.WriteWord(pRod->siWingSpace);
		buffer.WriteByte(pRod->angle.ciWorkWingX0Y1);
		buffer.WriteInteger(pRod->angle.bDualEdge);
		buffer.WriteInteger(pRod->angle.bEndLjAngle);
		buffer.WriteInteger(pRod->angle.cbSpaceFlag);
		buffer.WriteByte(pRod->odd.ciOddCalStyle);
		buffer.WriteDouble(pRod->odd.fCollideDist);
		buffer.WriteInteger(pRod->odd.hOddRefRod1);
		buffer.WriteInteger(pRod->odd.hOddRefRod2);
	}
	buffer.WriteInteger(hashTemplBolts.GetNodeNum());
	for(CSchemaBolt* pBolt=hashTemplBolts.GetFirst();pBolt;pBolt=hashTemplBolts.GetNext())
	{	//������˨��Ϣ
		buffer.WriteInteger(pBolt->Id);
		buffer.WriteInteger(pBolt->d);
		buffer.WriteString(pBolt->grade);
		buffer.WriteString(pBolt->through.ToThickStr());
		buffer.WriteInteger(pBolt->des_base_pos.idResideAngle);
		buffer.WriteByte(pBolt->des_base_pos.ciWorkWing);
		buffer.WriteByte(pBolt->des_base_pos.cLocationStyle);
		buffer.WriteByte(pBolt->des_base_pos.ciDirection);
		buffer.WriteWord(pBolt->des_base_pos.len_offset_dist);
		buffer.WriteByte(pBolt->des_base_pos.ciWingOffsetStyle);
		buffer.WriteWord(pBolt->des_base_pos.wing_offset_dist);
		buffer.WriteByte(pBolt->des_base_pos.anchor.ciAnchorType);
		buffer.WriteInteger(pBolt->des_base_pos.anchor.idPriorAngle);
		buffer.WriteWord(pBolt->des_base_pos.anchor.g_arr[0].ciStyleG);
		buffer.WriteWord(pBolt->des_base_pos.anchor.g_arr[0].g);
		if(pBolt->des_base_pos.anchor.ciAnchorType==1)
			buffer.WriteInteger(pBolt->des_base_pos.anchor.bIncOddEffect);
		else if(pBolt->des_base_pos.anchor.ciAnchorType==2)
		{
			buffer.WriteByte(pBolt->des_base_pos.anchor.ciIntersPlane);
			buffer.WriteDouble(pBolt->des_base_pos.anchor.vPlaneNormal.x);
			buffer.WriteDouble(pBolt->des_base_pos.anchor.vPlaneNormal.y);
			buffer.WriteDouble(pBolt->des_base_pos.anchor.vPlaneNormal.z);
		}
		else if(pBolt->des_base_pos.anchor.ciAnchorType==3)
		{	//�Ǹ����߽���
			buffer.WriteInteger(pBolt->des_base_pos.anchor.idSecondaryAngle);
			buffer.WriteWord(pBolt->des_base_pos.anchor.g_arr[1].ciStyleG);
			buffer.WriteWord(pBolt->des_base_pos.anchor.g_arr[1].g);
		}
		buffer.WriteByte(pBolt->normal.ciNormStyle);
		buffer.WriteByte(pBolt->normal.ciDirection);
		buffer.WriteByte(pBolt->normal.ciWorkWing);
		buffer.WriteInteger(pBolt->normal.idResideAngle);
	}
	//д����а�
	EmptyClipboard();//��ռ��а�,�õ�ǰ���ڽ���ӵ�м��а�  
	HANDLE hClip=GlobalAlloc(GHND,buffer.GetLength());//�����ڴ���󷵻ص�ַ(�ú�����ϵͳ��ȫ�ֶ��з����ڴ�)   
	char *pBuf=(char *)GlobalLock(hClip);//����ȫ���ڴ���ָ�����ڴ�飬������һ����ֵַ������ָ���ڴ�����ʼ��
	memcpy(pBuf,buffer.GetBufferPtr(),buffer.GetLength());
	GlobalUnlock(hClip);			//����ȫ���ڴ��  
	SetClipboardData(CF_TEXT,hClip);//���ü���������  
	CloseClipboard();				//�رռ�����
	return TRUE;
}  
BOOL CDeepClonePlate::ReadFromClipboard()  
{  
	if(!IsClipboardFormatAvailable(CF_TEXT)) 
		return FALSE;	//��ȡ��������ĸ�ʽ�Ƿ���Դ���
	if(!::OpenClipboard(NULL)) 
		return FALSE;	//�򿪼��а�
	CBuffer buffer(100000);
	HANDLE hClip=GetClipboardData(CF_TEXT); //������ָ����ʽ����������,������һ������������� 
	if(hClip!=NULL)
	{
		DWORD dwLength=GlobalSize(hClip);
		char *pBuf=(char*)GlobalLock(hClip);//����ȫ���ڴ���ָ�����ڴ�� ������һ����ֵַ,����������ָ��ͷ��ַ
		if(pBuf!=NULL)
			buffer.Write(pBuf,dwLength);
		GlobalUnlock(hClip);           //���� 
	}
	CloseClipboard(); 
	//��������
	char sName[50]="";
	buffer.SeekToBegin();
	buffer.ReadString(sName,50);
	if(strcmp(sName,"ClonePlate")!=0)
		return FALSE;
	buffer.ReadInteger(&m_nThick);
	buffer.ReadByte(&m_cMateria);
	buffer.ReadByte(&m_nPlaneN);
	buffer.ReadInteger(&m_idDatumRod);
	buffer.ReadInteger(&m_bFlexibleDesign);
	buffer.ReadByte(&m_ciProfileSty);
	buffer.ReadByte(&ciDatumAxis);
	buffer.ReadWord(&ciDirectionAxisZ);
	buffer.ReadByte(&origin.ciLenDirection);
	buffer.ReadWord(&origin.siLenOffset);
	buffer.ReadWord(&origin.g_arr[0].ciStyleG);
	buffer.ReadWord(&origin.g_arr[0].g);
	if(m_nPlaneN>1)
	{
		buffer.ReadByte(&huoqu_pt[0].ciLenDirection);
		buffer.ReadWord(&huoqu_pt[0].siLenOffset);
		buffer.ReadInteger(&huoqu_norm[0].m_idViceRod);
		buffer.ReadInteger(&huoqu_norm[0].m_idCrossRod);
		buffer.ReadWord(&huoqu_pt[0].g_arr[0].ciStyleG);
		buffer.ReadWord(&huoqu_pt[0].g_arr[0].g);
		if(m_nPlaneN==3)
		{
			buffer.ReadByte(&huoqu_pt[1].ciLenDirection);
			buffer.ReadWord(&huoqu_pt[1].siLenOffset);
			buffer.ReadInteger(&huoqu_norm[1].m_idViceRod);
			buffer.ReadInteger(&huoqu_norm[1].m_idCrossRod);
			buffer.ReadWord(&huoqu_pt[1].g_arr[0].ciStyleG);
			buffer.ReadWord(&huoqu_pt[1].g_arr[0].g);
		}
	}
	int nCount=0,iKey=0,iFlag=0;
	buffer.ReadInteger(&nCount);
	for(int i=0;i<nCount;i++)
	{	//���Ӹ˼���Ϣ
		buffer.ReadInteger(&iKey);
		CSchemaRod* pRod=hashLinkRods.Add(iKey);
		buffer.ReadInteger(&pRod->m_idClassType);
		buffer.ReadInteger(&pRod->vec_type);
		buffer.ReadDouble(&pRod->offset_angle);
		buffer.ReadByte(&pRod->ciConnectTipS0E1);
		buffer.ReadWord(&pRod->siPlaneNo);
		buffer.ReadWord(&pRod->siBerSpace);
		buffer.ReadWord(&pRod->siEndSpace);
		buffer.ReadWord(&pRod->siWingSpace);
		buffer.ReadByte(&pRod->angle.ciWorkWingX0Y1);
		buffer.ReadInteger(&pRod->angle.bDualEdge);
		buffer.ReadInteger(&pRod->angle.bEndLjAngle);
		buffer.ReadInteger(&iFlag);
		pRod->angle.cbSpaceFlag=ANGLE_SPACE_FLAG(iFlag);
		buffer.ReadByte(&pRod->odd.ciOddCalStyle);
		buffer.ReadDouble(&pRod->odd.fCollideDist);
		buffer.ReadInteger(&pRod->odd.hOddRefRod1);
		buffer.ReadInteger(&pRod->odd.hOddRefRod2);
	}
	buffer.ReadInteger(&nCount);
	CXhChar50 sValue;
	for(int i=0;i<nCount;i++)
	{	//������˨��Ϣ
		buffer.ReadInteger(&iKey);
		CSchemaBolt* pSchemaBolt=hashTemplBolts.Add(iKey);
		buffer.ReadInteger(&pSchemaBolt->d);
		buffer.ReadString(pSchemaBolt->grade,5);
		buffer.ReadString((char*)sValue);
		pSchemaBolt->InitThrough(sValue);
		buffer.ReadInteger(&pSchemaBolt->des_base_pos.idResideAngle);
		buffer.ReadByte(&pSchemaBolt->des_base_pos.ciWorkWing);
		buffer.ReadByte(&pSchemaBolt->des_base_pos.cLocationStyle);
		buffer.ReadByte(&pSchemaBolt->des_base_pos.ciDirection);
		buffer.ReadWord(&pSchemaBolt->des_base_pos.len_offset_dist);
		buffer.ReadByte(&pSchemaBolt->des_base_pos.ciWingOffsetStyle);
		buffer.ReadWord(&pSchemaBolt->des_base_pos.wing_offset_dist);
		buffer.ReadByte(&pSchemaBolt->des_base_pos.anchor.ciAnchorType);
		buffer.ReadInteger(&pSchemaBolt->des_base_pos.anchor.idPriorAngle);
		buffer.ReadWord(&pSchemaBolt->des_base_pos.anchor.g_arr[0].ciStyleG);
		buffer.ReadWord(&pSchemaBolt->des_base_pos.anchor.g_arr[0].g);
		if(pSchemaBolt->des_base_pos.anchor.ciAnchorType==1)
			buffer.ReadInteger(&pSchemaBolt->des_base_pos.anchor.bIncOddEffect);
		else if(pSchemaBolt->des_base_pos.anchor.ciAnchorType==2)
		{
			buffer.ReadByte(&pSchemaBolt->des_base_pos.anchor.ciIntersPlane);
			buffer.ReadDouble(&pSchemaBolt->des_base_pos.anchor.vPlaneNormal.x);
			buffer.ReadDouble(&pSchemaBolt->des_base_pos.anchor.vPlaneNormal.y);
			buffer.ReadDouble(&pSchemaBolt->des_base_pos.anchor.vPlaneNormal.z);
		}
		else if(pSchemaBolt->des_base_pos.anchor.ciAnchorType==3)
		{	//�Ǹ����߽���
			buffer.ReadInteger(&pSchemaBolt->des_base_pos.anchor.idSecondaryAngle);
			buffer.ReadWord(&pSchemaBolt->des_base_pos.anchor.g_arr[1].ciStyleG);
			buffer.ReadWord(&pSchemaBolt->des_base_pos.anchor.g_arr[1].g);
		}
		buffer.ReadByte(&pSchemaBolt->normal.ciNormStyle);
		buffer.ReadByte(&pSchemaBolt->normal.ciDirection);
		buffer.ReadByte(&pSchemaBolt->normal.ciWorkWing);
		buffer.ReadInteger(&pSchemaBolt->normal.idResideAngle);
	}
	return TRUE;
} 

#endif
#endif
