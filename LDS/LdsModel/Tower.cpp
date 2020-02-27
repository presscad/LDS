//<LOCALE_TRANSLATE BY hxr /> 2015-04-23
// Tower.cpp: implementation of the CTower class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdio.h"
#include "Tower.h"
#include "BriefWeldPart.h"
#include "WindLoad.h"
#include "XhCharString.h"
#include "LogFile.h"
#include "SortFunc.h"
#include "ParseAdaptNo.h"
#include "ElectricTower.h"
#ifndef __DRAWING_CONTEXT_
#include "ListenCollector.h"
#endif
#if defined(__LDS_CONTEXT_)&&defined(__PART_DESIGN_INC_)
#include "Codes.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//CONNECTFL_STRU
static const BYTE FL_TYPE = 0x01;
static const BYTE BOLT_RADIUS = 0x02;
static const BYTE BOLT_DIA = 0x03;
static const BYTE BOLT_NUM = 0x04;
double CConnectFL::SingleWeight()	//����������
{
	double outter_radius=m_fInnerRadius+m_nWidthFL;
	double sum_weight=Pi*(outter_radius*outter_radius-m_fInnerRadius*m_fInnerRadius)*m_nThickFL;
	if(m_nThickRib>0)	//�мӾ��߰巨��
		sum_weight+=bolt_n*m_nThickRib*m_nWidthFL*m_nHeightRib;
	return sum_weight*7.85e-6;
}
double CConnectFL::SumWeight()
{
	double outter_radius=m_fInnerRadius+m_nWidthFL;
	double sum_weight=Pi*(outter_radius*outter_radius-m_fInnerRadius*m_fInnerRadius)*m_nCoupleThickFL;
	//�ۼӼ�����Է�����
	sum_weight+=Pi*(outter_radius*outter_radius-m_fCoupleInnerRadius*m_fCoupleInnerRadius)*m_nThickFL;
	if(m_nThickRib>0)	//�мӾ��߰巨��
	{
		sum_weight+=bolt_n*m_nThickRib*m_nWidthFL*m_nHeightRib;
		//�ۼӼ�����Է����Ӿ��߰���
		sum_weight+=bolt_n*m_nThickRib*(m_nWidthFL+m_fInnerRadius-m_fCoupleInnerRadius)*m_nCoupleHeightRib;
	}
	return sum_weight*7.85e-6;
}
void CConnectFL::Optimize(BYTE flag)
{
}
BOOL CConnectFL::IsEqual(CConnectFL flConnect,BOOL isNear /* = FALSE */)
{
	double fBoltRadius = 2;
	long nHeightRib = 2;
	long nThickRib = 2;
	long nThickFL = 2;
	long nWidthFL = 2;
	
	if (flConnect.bolt_n == bolt_n && flConnect.bolt_d == bolt_d && flConnect.m_fInnerRadius == m_fInnerRadius
		&& (flConnect.m_nThickRib == 0&&m_nThickRib==0) && (flConnect.m_nThickRib>0&&m_nThickRib>0))
	{
		if (!isNear)
		{
			if(flConnect.m_fBoltRadius-m_fBoltRadius<EPS&&flConnect.m_nHeightRib==m_nHeightRib
				&&flConnect.m_nThickFL==m_nThickFL&&flConnect.m_nWidthFL==m_nWidthFL&&
				flConnect.m_nThickRib == m_nThickRib)
				return TRUE;
		}
		else if (isNear)
		{
			if (flConnect.m_fBoltRadius-m_fBoltRadius<=fBoltRadius && flConnect.m_nHeightRib-m_nHeightRib<=nHeightRib
				&& flConnect.m_nThickFL-m_nThickFL<=nThickFL && flConnect.m_nWidthFL-m_nWidthFL<=nWidthFL
				&& flConnect.m_nThickRib - m_nThickRib<=nThickRib)
				return TRUE;
		}
	}
	return FALSE;
}

ANGLE_JOINT::ANGLE_JOINT()
{
	_id=0;
	cJointType=0;
	innerAngle.width=100;innerAngle.thick=7;	//�ڰ���֫��֫��
	outer.angle.width=110;outer.angle.thick=7;	//�����֫��֫��
	innerAngle.cMaterial=CSteelMatLibrary::Q345BriefMark();	//�ڰ�������
	outer.angle.cMaterial=CSteelMatLibrary::Q345BriefMark();	//���������
	outer.angle.idClsType=innerAngle.idClsType=smallAngle.idClsType=bigAngle.idClsType=CLS_LINEANGLE;
	outer.angle.cSubClassType=innerAngle.cSubClassType=smallAngle.cSubClassType=bigAngle.cSubClassType='L';
	//wrap_width=80;wrap_thick=8;		//����ְ��
	//cWrapPlateMaterial=CSteelMatLibrary::Q345BriefMark();	//����ְ����
	bolt.rows=1;
	bolt.n=8;
	bolt.d=20;
	bolt.grade=68;
	bolt.holeout_coef=1.33;
	hNode=hSmallRod=hBigRod=0;
	pTagInfo=NULL;
}
ANGLE_JOINT ANGLE_JOINT::operator =(ANGLE_JOINT& joint)
{
	memcpy(this,&joint,sizeof(ANGLE_JOINT));
	if(joint.pTagInfo!=NULL)
	{
		pTagInfo=LoadTagInfo();
		pTagInfo->pNode=joint.pTagInfo->pNode;
		pTagInfo->pSmallAngle=joint.pTagInfo->pSmallAngle;
		pTagInfo->pBigAngle=joint.pTagInfo->pBigAngle;
		pTagInfo->scaleAreaJ2A1=joint.pTagInfo->scaleAreaJ2A1;	//��ͷ�뱻����С�ǸֵĽ����֮��
		pTagInfo->scaleAreaJ2A2=joint.pTagInfo->scaleAreaJ2A2;	//��ͷ�뱻���Ӵ�ǸֵĽ����֮��
		pTagInfo->scaleForceB2A1=joint.pTagInfo->scaleForceB2A1;	//��˨�뱻����С�Ǹֵĳ�������֮��
		pTagInfo->scaleForceB2A2=joint.pTagInfo->scaleForceB2A2;	//��˨�뱻���Ӵ�Ǹֵĳ�������֮��
	}
	return *this;
}
ANGLE_JOINT::~ANGLE_JOINT()
{
	if(pTagInfo)
		pTagInfo->DetachObject();
}
DWORD ANGLE_JOINT::InitId(DWORD id)
{
	if(_id==0)	//Ϊ��ֹ�Ƿ�ߥ�ģ�idֻ�ܳ�ʼ��һ��
		_id=id;
	return _id;
}
void ANGLE_JOINT::ToBuffer(CBuffer &buffer,long version/* =0 */,long doc_type/* =0 */)
{
	buffer.WriteByte(cJointType);
	buffer.WriteInteger(hNode);
	buffer.WriteInteger(hBigRod);
	buffer.WriteInteger(hSmallRod);
	//��˨��Ϣ
	buffer.WriteByte(bolt.d);
	buffer.WriteByte(bolt.grade);
	buffer.WriteByte(bolt.n);
	buffer.WriteByte(bolt.rows);
	buffer.WriteDouble(bolt.holeout_coef);
	//�ڰ���֫��֫�񼰲���
	innerAngle.ToBuffer(buffer);
	//����ְ��񼰲��ʻ������֫��֫�񼰲���
	outer.angle.ToBuffer(buffer);
	//��hSmallRod��Ӧ�ĽǸֹ��
	smallAngle.ToBuffer(buffer);
	//��hBigRod��Ӧ�Ǹֹ��
	bigAngle.ToBuffer(buffer);
	//��ͷ�Գ���Ϣ
	buffer.WriteByte(mirinfo.axis_flag);
	bool bSpecOrigin=!mirinfo.origin.IsZero();
	buffer.WriteBooleanThin(bSpecOrigin);
	if(bSpecOrigin)
		buffer.WritePoint(mirinfo.origin);
	if(mirinfo.axis_flag&0x08)
	{
		buffer.WriteByte(mirinfo.array_num);
		buffer.WriteWord(mirinfo.rotate_angle);
	}
	if(mirinfo.axis_flag&0x10)	//����
		buffer.WritePoint(mirinfo.mir_norm);
}
void ANGLE_JOINT::FromBuffer(CBuffer &buffer,long version/* =0 */,long doc_type/* =0 */)
{
	buffer.ReadByte(&cJointType);
	buffer.ReadInteger(&hNode);
	buffer.ReadInteger(&hBigRod);
	buffer.ReadInteger(&hSmallRod);
	//��˨��Ϣ
	buffer.ReadByte(&bolt.d);
	buffer.ReadByte(&bolt.grade);
	buffer.ReadByte(&bolt.n);
	buffer.ReadByte(&bolt.rows);
	buffer.ReadDouble(&bolt.holeout_coef);
	//�ڰ���֫��֫�񼰲���
	innerAngle.FromBuffer(buffer,version,doc_type);
	//����ְ��񼰲��ʻ������֫��֫�񼰲���
	outer.angle.FromBuffer(buffer,version,doc_type);
	//��hSmallRod��Ӧ�ĽǸֹ��
	smallAngle.FromBuffer(buffer,version,doc_type);
	//��hBigRod��Ӧ�Ǹֹ��
	bigAngle.FromBuffer(buffer,version,doc_type);
	//��ͷ�Գ���Ϣ
	buffer.ReadByte(&mirinfo.axis_flag);
	bool bSpecOrigin=false;
	buffer.ReadBooleanThin(&bSpecOrigin);
	if(bSpecOrigin)
		buffer.ReadPoint(mirinfo.origin);
	if(mirinfo.axis_flag&0x08)
	{
		buffer.ReadByte(&mirinfo.array_num);
		buffer.ReadWord(&mirinfo.rotate_angle);
	}
	if(mirinfo.axis_flag&0x10)	//����
		buffer.ReadPoint(mirinfo.mir_norm);
}
bool ANGLE_JOINT::VerifyLinkAngles()
{
	IModel* pActiveModel=CLDSObject::_console->GetActiveModel();
	if(pActiveModel==NULL)
		return false;
	CLDSLinePart* pConnSmallAngle=NULL,*pConnBigAngle=NULL;
	pTagInfo=LoadTagInfo();
	if(pTagInfo->pSmallAngle==NULL||pTagInfo->pSmallAngle->handle!=hSmallRod)
	{
		pConnSmallAngle=pActiveModel->FromRodHandle(hSmallRod);
		if(pConnSmallAngle==NULL || pConnSmallAngle->size_idClassType!=CLS_LINEANGLE)
			pTagInfo->pSmallAngle=NULL;
		else
			pTagInfo->pSmallAngle=(CLDSLineAngle*)pConnSmallAngle;
	}
	if(pTagInfo->pBigAngle==NULL||pTagInfo->pBigAngle->handle!=hBigRod)
	{
		pConnBigAngle=pActiveModel->FromRodHandle(hBigRod);
		if(pConnBigAngle==NULL || pConnBigAngle->size_idClassType!=CLS_LINEANGLE)
			pTagInfo->pBigAngle=NULL;
		else
			pTagInfo->pBigAngle=(CLDSLineAngle*)pConnBigAngle;
	}

	if(pTagInfo->pNode==NULL || pTagInfo->pNode->handle!=hNode)
		pTagInfo->pNode=pActiveModel->FromNodeHandle(hNode);
	if(pTagInfo->pSmallAngle!=NULL)
	{
		smallAngle.cMaterial=pTagInfo->pSmallAngle->cMaterial;
		smallAngle.width=pTagInfo->pSmallAngle->size_wide;
		smallAngle.thick=pTagInfo->pSmallAngle->size_thick;
		smallAngle.idClsType=CLS_LINEANGLE;
		smallAngle.cSubClassType='L';
	}
	if(pTagInfo->pBigAngle!=NULL)
	{
		bigAngle.cMaterial=pTagInfo->pBigAngle->cMaterial;
		bigAngle.width=pTagInfo->pBigAngle->size_wide;
		bigAngle.thick=pTagInfo->pBigAngle->size_thick;
		bigAngle.idClsType=CLS_LINEANGLE;
		bigAngle.cSubClassType='L';
	}
	if( bigAngle.width <smallAngle.width ||
	   (bigAngle.width==smallAngle.width&&
	    bigAngle.thick<smallAngle.thick))
	{
		SIZE_WTM tempAngleSize=bigAngle;
		bigAngle=smallAngle;
		smallAngle=tempAngleSize;
		long hTempRod=hBigRod;
		hBigRod=hSmallRod;
		hSmallRod=hTempRod;
		CLDSLineAngle* pTempAngle=pTagInfo->pSmallAngle;
		pTagInfo->pSmallAngle=pTagInfo->pBigAngle;
		pTagInfo->pBigAngle=pTempAngle;
	}
	JG_PARA_TYPE* pType=FindJgType(smallAngle.width,smallAngle.thick,smallAngle.cSubClassType);
	if(pType==NULL)
		return false;
	pType=FindJgType(bigAngle.width,bigAngle.thick,bigAngle.cSubClassType);
	if(pType==NULL)
		return false;
	return true;
}
int ANGLE_JOINT::TestCalMinBoltN()	//����������Ҫ����˨��
{
	if(pTagInfo==NULL)
		return 8;
	if(smallAngle.width<80)
		return 4;
	else if(smallAngle.width<100)
		return 6;
	else if(smallAngle.width<125)
		return 8;
	else //if(pTagInfo->smallAngle.width<140)
		return 10;
}
bool ANGLE_JOINT::DesignCheckAreaAndBoltForceScale()
{
	if(!VerifyLinkAngles())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("The information of angle which current joint connects is wrong!");
#else
		logerr.Log("��ǰ��ͷ�����ӽǸ���Ϣ����!");
#endif
		return false;
	}
	JG_PARA_TYPE* pSmallAngleType=FindJgType(smallAngle.width,smallAngle.thick,'L');//pConnSmallAngle->size_cSubClassType);
	JG_PARA_TYPE* pBigAngleType=FindJgType(bigAngle.width,bigAngle.thick,'L');//pConnBigAngle->size_cSubClassType);
	if(pSmallAngleType->cType!=pBigAngleType->cType)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("The type of angles which current joint two side connects is different!");
#else
		logerr.Log("��ǰ��ͷ���������ӵĽǸ����Ͳ�ͬ!");
#endif
		return false;
	}
	double boltHoleOutArea=(bolt.d+1.5)*bolt.holeout_coef;	//��λ��ȵļ��������mm
	//���㱻���ӽǸ��е�С���ľ���������ѿ۳���˨���ף���mm2
	double connSmallArea=pSmallAngleType->area*100-boltHoleOutArea*pSmallAngleType->wing_thick;
	//���㱻���ӽǸ��еĴ����ľ���������ѿ۳���˨���ף���mm2
	double connBigArea=pBigAngleType->area*100-boltHoleOutArea*pBigAngleType->wing_thick;
	//�����ͷ���Ӽ����ڰ���������塢�����������Ч�����
	if(connSmallArea<EPS)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("The available loading sectional area of (small) angle which joint connects is zero,data error.");
#else
		logerr.Log("��ͷ����(С)�Ǹ���Ч���ؽ����Ϊ�㣬��������");
#endif
		return false;
	}
	if(connBigArea<EPS)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("The available loading sectional area of (big) angle which joint connects is zero,data error.");
#else
		logerr.Log("��ͷ����(��)�Ǹ���Ч���ؽ����Ϊ�㣬��������");
#endif
		return false;
	}
	double connJointArea=0;
	if(cJointType==0)
	{	//�ڰ�������ְ�
		JG_PARA_TYPE* pInnerAngleType=FindJgType(innerAngle.width,innerAngle.thick,'L');
		if(pInnerAngleType==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Data error");
#else
			logerr.Log("��������");
#endif
			return false;
		}
		connJointArea =pInnerAngleType->area*100-boltHoleOutArea*innerAngle.thick;
		connJointArea+=outer.plate.width*outer.plate.thick*2-boltHoleOutArea*outer.plate.thick;
		TagInfo(this)->scaleAreaJ2A1=connJointArea/connSmallArea;
		TagInfo(this)->scaleAreaJ2A2=connJointArea/connBigArea;
	}
	else if(cJointType==1)
	{	//�����
		JG_PARA_TYPE* pOuterAngleType=FindJgType(outer.angle.width,outer.angle.thick,'L');
		if(pOuterAngleType==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Data error");
#else
			logerr.Log("��������");
#endif
			return false;
		}
		connJointArea=pOuterAngleType->area*100-boltHoleOutArea*outer.angle.thick;
		TagInfo(this)->scaleAreaJ2A1=connJointArea/connSmallArea;
		TagInfo(this)->scaleAreaJ2A2=connJointArea/connBigArea;
	}
	else if(cJointType==2)
	{	//�������
		JG_PARA_TYPE* pInnerAngleType=FindJgType(innerAngle.width,innerAngle.thick,'L');
		JG_PARA_TYPE* pOuterAngleType=FindJgType(outer.angle.width,outer.angle.thick,'L');
		if(pInnerAngleType==NULL||pOuterAngleType==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Data error");
#else
			logerr.Log("��������");
#endif
			return false;
		}
		connJointArea=pInnerAngleType->area*100-boltHoleOutArea*innerAngle.thick;
		connJointArea =pInnerAngleType->area*100-boltHoleOutArea*innerAngle.thick;
		connJointArea+=pOuterAngleType->area*100-boltHoleOutArea*outer.angle.thick;
		TagInfo(this)->scaleAreaJ2A1=connJointArea/connSmallArea;
		TagInfo(this)->scaleAreaJ2A2=connJointArea/connBigArea;
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("Didn't support automatic designing group angle's joint now.");
#else
		logerr.Log("�ݲ�֧����ϽǸֽ�ͷ���Զ����");
#endif
		return false;
	}
	//2.������˨�����������ĳ�������ֵ
	//���㱻���ӽǸ��е�С���ľ���������ѿ۳���˨���ף���mm2
	STEELMAT *pMatType=QuerySteelMatByBriefMark(smallAngle.cMaterial);
	double connSmallAngleForce=connSmallArea*pMatType->f(pSmallAngleType->wing_thick);//N
	//���㱻���ӽǸ��еĴ����ľ���������ѿ۳���˨���ף���mm2
	pMatType=QuerySteelMatByBriefMark(bigAngle.cMaterial);
	double connBigAngleForce=connBigArea*pMatType->f(pBigAngleType->wing_thick);//N
	//�����ͷ���Ӽ����ڰ���������塢�����������Ч�����
	LS_GRADE *pLsGrade=FindLsMatType(CXhChar16(bolt.grade*0.1));
	double boltShearForce=bolt.n*Pi*0.25*bolt.d*bolt.d*pLsGrade->fv;	//n����˨�ĵ���м���,N
	double minAngleThick=min(smallAngle.thick,bigAngle.thick);
	double boltComprForce=bolt.n*minAngleThick*bolt.d*pLsGrade->fc;	//n����˨�ĳ�ѹ��,N
	if(cJointType==0||cJointType==2)//�ڰ�������ְ�������������Ϊ˫����
		boltShearForce*=2;
	double boltActiveForce=min(boltShearForce,boltComprForce);	//N
	if(connSmallAngleForce<EPS)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("The loading ability of (small) angle which joint connects is zero,data error.");
#else
		logerr.Log("��ͷ����(С)�Ǹֳ�����Ϊ�㣬��������");
#endif
		return false;
	}
	if(connBigAngleForce<EPS)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("The loading ability of (big) angle which joint connects is zero,data error.");
#else
		logerr.Log("��ͷ����(��)�Ǹֳ�����Ϊ�㣬��������");
#endif
		return false;
	}
	TagInfo(this)->scaleForceB2A1=boltActiveForce/connSmallAngleForce;
	TagInfo(this)->scaleForceB2A2=boltActiveForce/connBigAngleForce;
	return true;
}
CXhChar50 ANGLE_JOINT::GetJointNo()
{
	CXhChar50 jointNo;
	if(!VerifyLinkAngles())
		return jointNo;
	if(pTagInfo&&pTagInfo->pNode&&pTagInfo->pSmallAngle&&pTagInfo->pBigAngle)
	{	//��ͷ���
		jointNo.Printf("%4d:",pTagInfo->pNode->point_i);
		if(pTagInfo->pSmallAngle->pStart->handle==hNode)
			jointNo.Append(CXhChar16("%4d",pTagInfo->pSmallAngle->pEnd->point_i));
		else //if(pTagInfo->pSmallAngle->pEnd->handle==hNode)
			jointNo.Append(CXhChar16("%4d",pTagInfo->pSmallAngle->pStart->point_i));
		if(pTagInfo->pBigAngle->pStart->handle==hNode)
			jointNo.Append(CXhChar16("-%4d",pTagInfo->pBigAngle->pEnd->point_i));
		else //if(pTagInfo->pBigAngle->pEnd->handle==hNode)
			jointNo.Append(CXhChar16("-%4d",pTagInfo->pBigAngle->pStart->point_i));
	}
	return jointNo;
}
CXhChar100 ANGLE_JOINT::GetBoltInfo(bool bAlignRowCount/*=TRUE*/)
{
	CXhChar100 sBoltInfo("%.1fM",bolt.grade*0.1);//��˨����
	sBoltInfo.Append(CXhChar16(bolt.d));
	if(bAlignRowCount)
		sBoltInfo.Append((char*)CXhChar16("*%2d/%d",bolt.n,bolt.rows));
	else 
		sBoltInfo.Append((char*)CXhChar16("*%d/%d",bolt.n,bolt.rows));
	return sBoltInfo;
}
CXhChar100 ANGLE_JOINT::GetInnerPartSpec(bool bIncAngleType/*=FALSE*/)
{
	CXhChar100 sInnerPartSpec;
	if(bIncAngleType)
		sInnerPartSpec.Append("L");
	sInnerPartSpec.Printf("%sX%s%",(char*)CXhChar16(innerAngle.width),
			(char*)CXhChar16(innerAngle.thick));
	return sInnerPartSpec;
}
CXhChar100 ANGLE_JOINT::GetOutterPartSpec(bool bIncAngleType/*=FALSE*/)
{
	CXhChar100 sOutterPartSpec="";
	if(cJointType==2||cJointType==1)
	{
		if(bIncAngleType)
			sOutterPartSpec.Append("L");
		sOutterPartSpec.Printf("%sX%s",(char*)CXhChar16(outer.angle.width),
			(char*)CXhChar16(outer.angle.thick));
	}
	else if(cJointType==0)
		sOutterPartSpec.Printf("-%d*%d%C",(int)outer.plate.thick,
			(int)outer.plate.width,outer.plate.cMaterial);
	return sOutterPartSpec;
}
//���ӱ�ǩ��Ա��������
ANGLE_JOINT::ITEM_TAG_INFO* TagInfo(ANGLE_JOINT* pItem){return (ANGLE_JOINT::ITEM_TAG_INFO*)pItem->pTagInfo;}
ATOM_LIST<ANGLE_JOINT::ITEM_TAG_INFO> ANGLE_JOINT::listTagInfo;	//��ʱ�ҽӵĸ��ӵ�������{ANGLE_JOINT}�ϵ����ݶ�������
ANGLE_JOINT::ITEM_TAG_INFO::ITEM_TAG_INFO()
{
	pNode=NULL;
	pSmallAngle=pBigAngle=NULL;
	scaleAreaJ2A1=scaleAreaJ2A2=scaleForceB2A1=scaleForceB2A2=1.0;
}
ANGLE_JOINT::ITEM_TAG_INFO* ANGLE_JOINT::LoadTagInfo()
{
	if(pTagInfo!=NULL)
		return (ANGLE_JOINT::ITEM_TAG_INFO*)pTagInfo;
	pTagInfo=listTagInfo.append();
	((ITEM_TAG_INFO*)pTagInfo)->AttachObject(this);
	return (ITEM_TAG_INFO*)pTagInfo;
}
CWorkStatus::CWorkStatus()
{
	//strcpy(m_sClassName,"CWorkStatus");
	m_nClassTypeId=CLS_WORKCASE;
	strcpy(description,"");
	Fai=1.0;
	GamaQi=1.4;
	ice_thick=0;
	strcpy(limit_str,"*");
	WindLoad.azimuth=0;
	WindLoad.velocity=0;
	coefIce.B_I=1.0;
	coefIce.gamaQ=1.4;
	coefIce.Kesai=1.0;
	coefWind.gamaQ=coefAn.gamaQ=coefU.gamaQ=coefT.gamaQ=1.4;
	coefWind.Kesai=coefAn.Kesai=coefU.Kesai=coefT.Kesai=1;
	gamaEh=gamaEv=1.3;
	m_cMainLoadType=0;
	m_bCheckWorkCond=false;
	m_bSpecLoadFactors=false;
	m_bLoadIncFactor=false;
	m_iEiffelCaseNo=0;
	iNo=0;
	iWireLayoutNo=1;
	m_cHoriEarthQuakeAxis=90;
	strcpy(vibration_mode_str,"");
}
CWorkStatus::~CWorkStatus()
{
}
const DWORD HASHTABLESIZE1 = 500;
IMPLEMENT_PROP_FUNC(CWorkStatus);
void CWorkStatus::InitPropHashtable()
{
#if defined(__PART_DESIGN_INC_)&&!defined(__DRAWING_CONTEXT_)
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE1);
	propStatusHashtable.CreateHashTable(HASHTABLESIZE1);
	//
	int id=1;
#ifdef AFX_TARG_ENU_ENGLISH
	AddPropItem("attribute",PROPLIST_ITEM(id++,"attribute","attribute"));
	AddPropItem("description",PROPLIST_ITEM(id++,"Working condition of the name",""));
	AddPropItem("main_load",PROPLIST_ITEM(id++,"The main load","","The main load","Gale|ice|at low temperature the vertical seismic|wire|install|level earthquake only horizontal earthquake|only vertical earthquake"));
	AddPropItem("limit_str",PROPLIST_ITEM(id++,"Scope of application",""));
	AddPropItem("WindLoad",PROPLIST_ITEM(id++,"WindLoad","WindLoad"));
	AddPropItem("wind_speed",PROPLIST_ITEM(id++,"The wind speed,m/s","The wind speed,m/s"));
	AddPropItem("azimuth",PROPLIST_ITEM(id++,"Direction Angle","Direction Angle"));
	AddPropItem("ice_thick",PROPLIST_ITEM(id++,"The thickness of the ice",""));
	AddPropItem("tower_ice_factor",PROPLIST_ITEM(id++,"Tower biggest ice wind load increment coefficient",""));
	AddPropItem("vibration_mode",PROPLIST_ITEM(id++,"Applicable scope of vibration mode",""));
	AddPropItem("HoriEarthQuakeAxis",PROPLIST_ITEM(id++,"Direction Angle to the axis of the horizontal earthquake",""));
	AddPropItem("LineLoad",PROPLIST_ITEM(id++,"Line Load","Line Load"));
	AddPropItem("WireGamaQi",PROPLIST_ITEM(id++,"Combination coefficient ","Line load combination coefficient"));
	AddPropItem("WireKesai",PROPLIST_ITEM(id++,"Block coefficient","Line load block coefficient"));
	AddPropItem("TowerWindLoad",PROPLIST_ITEM(id++,"Tower wind load","Tower wind load"));
	AddPropItem("TowerWindKesai",PROPLIST_ITEM(id++,"Combination coefficient","Tower coefficient of wind load combination"));
	AddPropItem("TowerWindGamaQi",PROPLIST_ITEM(id++,"Block coefficient","Tower wind load block coefficient"));
	AddPropItem("TowerIceLoad",PROPLIST_ITEM(id++,"Tower ice load","Tower ice load"));
	AddPropItem("TowerIceKesai",PROPLIST_ITEM(id++,"Combination coefficient","Tower ice load combination coefficient"));
	AddPropItem("TowerIceGamaQi",PROPLIST_ITEM(id++,"Block coefficient","Tower block coefficient of ice load"));
	AddPropItem("HorizontalSeismicLoad",PROPLIST_ITEM(id++,"The horizontal seismic load","The horizontal seismic load"));
	AddPropItem("HorizEarthQuakeKesai",PROPLIST_ITEM(id++,"Combination coefficient","The horizontal seismic load combination coefficient"));
	AddPropItem("HorizEarthQuakeQamaQi",PROPLIST_ITEM(id++,"Block coefficient","The horizontal seismic load block coefficient"));
	AddPropItem("VerticalSeismicLoad",PROPLIST_ITEM(id++,"The vertical seismic load","The vertical seismic load"));
	AddPropItem("VertEarthQuakeKesai",PROPLIST_ITEM(id++,"Combination coefficient","Coefficient of vertical seismic load combinations"));
	AddPropItem("VertEarthQuakeQamaQi",PROPLIST_ITEM(id++,"Block coefficient","The vertical seismic load block coefficient"));
#else
	AddPropItem("attribute",PROPLIST_ITEM(id++,"��������","��������"));
	AddPropItem("description",PROPLIST_ITEM(id++,"��������","��������"));
	AddPropItem("main_load",PROPLIST_ITEM(id++,"��Ҫ����","��Ҫ����","0.���|1.����|2.����|3.����|4.��װ|5.ˮƽ����|6.�������|7.��ˮƽ����|8.���������"));
	AddPropItem("limit_str",PROPLIST_ITEM(id++,"���÷�Χ","���÷�Χ"));
	AddPropItem("WindLoad",PROPLIST_ITEM(id++,"�����","�������Ϣ"));
	AddPropItem("wind_speed",PROPLIST_ITEM(id++,"����,m/s","����,m/s"));
	AddPropItem("azimuth",PROPLIST_ITEM(id++,"�����","�����"));
	AddPropItem("ice_thick",PROPLIST_ITEM(id++,"�������,mm","�������"));
	AddPropItem("tower_ice_factor",PROPLIST_ITEM(id++,"�����������������ϵ��","�����������������ϵ��"));
	AddPropItem("vibration_mode",PROPLIST_ITEM(id++,"�������ͷ�Χ","�������ͷ�Χ"));
	AddPropItem("HoriEarthQuakeAxis",PROPLIST_ITEM(id++,"���᷽���","ˮƽ�������᷽���"));
	AddPropItem("LineLoad",PROPLIST_ITEM(id++,"��������","��������"));
	AddPropItem("WireKesai",PROPLIST_ITEM(id++,"���ϵ��","�����������ϵ��"));
	AddPropItem("WireGamaQi",PROPLIST_ITEM(id++,"����ϵ��","�������ط���ϵ��"));
	AddPropItem("TowerWindLoad",PROPLIST_ITEM(id++,"���������","���������"));
	AddPropItem("TowerWindKesai",PROPLIST_ITEM(id++,"���ϵ��","������������ϵ��"));
	AddPropItem("TowerWindGamaQi",PROPLIST_ITEM(id++,"����ϵ��","��������ط���ϵ��"));
	AddPropItem("TowerIceLoad",PROPLIST_ITEM(id++,"������������","������������"));
	AddPropItem("TowerIceKesai",PROPLIST_ITEM(id++,"���ϵ��","���������������ϵ��"));
	AddPropItem("TowerIceGamaQi",PROPLIST_ITEM(id++,"����ϵ��","�����������ط���ϵ��"));
	AddPropItem("HorizontalSeismicLoad",PROPLIST_ITEM(id++,"ˮƽ��������","ˮƽ��������"));
	AddPropItem("HorizEarthQuakeKesai",PROPLIST_ITEM(id++,"���ϵ��","ˮƽ�����������ϵ��"));
	AddPropItem("HorizEarthQuakeQamaQi",PROPLIST_ITEM(id++,"����ϵ��","ˮƽ�������ط���ϵ��"));
	AddPropItem("VerticalSeismicLoad",PROPLIST_ITEM(id++,"����������","����������"));
	AddPropItem("VertEarthQuakeKesai",PROPLIST_ITEM(id++,"���ϵ��","�������������ϵ��"));
	AddPropItem("VertEarthQuakeQamaQi",PROPLIST_ITEM(id++,"����ϵ��","���������ط���ϵ��"));
#endif
#endif
}
int CWorkStatus::GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
#if defined(__PART_DESIGN_INC_)&&!defined(__DRAWING_CONTEXT_)
	if(GetPropID("description")==id)
		strcpy(sText,description);
	else if(GetPropID("main_load")==id)
	{
		if(m_cMainLoadType==0)
			sText.Copy("0.���");
		else if(m_cMainLoadType==1)
			sText.Copy("1.����");
		else if(m_cMainLoadType==2)
			sText.Copy("2.����");
		else if(m_cMainLoadType==3)
			sText.Copy("3.���߼������ȸ���");
		else if(m_cMainLoadType==4)
			sText.Copy("4.��װ");
		else if(m_cMainLoadType==5)
			sText.Copy("5.ˮƽ����");
		else if(m_cMainLoadType==6)
			sText.Copy("6.�������");
		else if(m_cMainLoadType==7)
			sText.Copy("7.��ˮƽ����");
		else if(m_cMainLoadType==8)
			sText.Copy("8.���������");
	}
	else if(GetPropID("limit_str")==id)
		strcpy(sText,limit_str);
	else if(GetPropID("wind_speed")==id)
		sText.Printf("%lg",WindLoad.velocity);
	else if(GetPropID("azimuth")==id)
		sText.Printf("%d",WindLoad.azimuth);
	else if(GetPropID("ice_thick")==id)
		sText.Printf("%lg",ice_thick);
	else if(GetPropID("tower_ice_factor")==id)
		sText.Printf("%lg",Bw_I());
	else if(GetPropID("vibration_mode")==id)
		strcpy(sText,vibration_mode_str);
	else if(GetPropID("HoriEarthQuakeAxis")==id)
		sText.Printf("%d",m_cHoriEarthQuakeAxis);
	else if(GetPropID("WireKesai")==id)
		sText.Printf("%lg",Kesai());
	else if(GetPropID("TowerWindKesai")==id)
		sText.Printf("%lg",Kesai_W());
	else if(GetPropID("TowerIceKesai")==id)
		sText.Printf("%lg",Kesai_I());
	else if(GetPropID("HorizEarthQuakeKesai")==id)
		sText.Printf("%lg",Kesai());
	else if(GetPropID("VertEarthQuakeKesai")==id)
		sText.Printf("%lg",Kesai());
	else if(GetPropID("WireGamaQi")==id)
		sText.Printf("%lg",GamaQ());
	else if(GetPropID("TowerWindGamaQi")==id)
		sText.Printf("%lg",GamaQ_W());
	else if(GetPropID("TowerIceGamaQi")==id)
		sText.Printf("%lg",GamaQ_I());
	else if(GetPropID("HorizEarthQuakeQamaQi")==id)
		sText.Printf("%lg",GamaQ_Eh());
	else if(GetPropID("VertEarthQuakeQamaQi")==id)
		sText.Printf("%lg",GamaQ_Ev());
	if(valueStr)
		strcpy(valueStr,sText);
#endif
	return sText.GetLength();
}
CTower* CWorkStatus::SuperiorTower()
{
	if(BelongModel()!=NULL&&BelongModel()->IsTowerModel())
		return (CTower*)BelongModel();
	else
		return NULL;
}

#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
double CWorkStatus::SetBw_I(double B_I)
{		//����ʱ�������������ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		coefIce.B_I=B_I;
	return Bw_I();
}
double CWorkStatus::SetGamaQ_W(double gamaQ)
{	//����ط���ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		coefWind.gamaQ=gamaQ;
	return GamaQ_W();
}
double CWorkStatus::SetGamaQ_I(double gamaQ)
{	//�������ط���ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		coefIce.gamaQ=gamaQ;
	return GamaQ_I();
}
double CWorkStatus::SetGamaQ_A(double gamaQ)
{	//��װ���ط���ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		coefAn.gamaQ=gamaQ;
	return GamaQ_A();
}
double CWorkStatus::SetGamaQ_U(double gamaQ)
{	//���ߡ������ȸ������ط���ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		coefU.gamaQ=gamaQ;
	return GamaQ_U();
}
double CWorkStatus::SetGamaQ_T(double gamaQ)
{	//�¶Ⱥ��ط���ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		coefT.gamaQ=gamaQ;
	return GamaQ_T();
}
double CWorkStatus::SetGamaQ_Eh(double gamaQ)
{	//ˮƽ�������ط���ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		gamaEh=gamaQ;
	return GamaQ_Eh();
}
double CWorkStatus::SetGamaQ_Ev(double gamaQ)
{	//���������ط���ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		gamaEv=gamaQ;
	return GamaQ_Ev();
}
double CWorkStatus::Bw_I()
{	//����ʱ�������������ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		return coefIce.B_I;
	else if(ice_thick<5)
		return 1.0;
	else if(ice_thick<10)
		return 1.1+(ice_thick-5)*0.02;
	else if(ice_thick<15)
		return 1.2+(ice_thick-10)*0.08;
	else if(ice_thick<20)
		return 1.6+(ice_thick-15)*0.04;
	else if(ice_thick<30)
		return 2.0+(ice_thick-20)*0.05;
	else
		return 2.5;
}
double CWorkStatus::SetGamaQ(double gamaQ)
{	//����ط���ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		GamaQi=gamaQ;
	return GamaQi;
}
double CWorkStatus::GamaQ()
{	//�ۺϿɱ���ط���ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		return GamaQi;
	else if(m_cMainLoadType<5)
		return 1.4;
	else
		return 0.5;
}
double CWorkStatus::GamaQ_W()
{	//����ط���ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		return coefWind.gamaQ;
	else if(m_cMainLoadType>=5)	//����
		return 1.0;
	else
		return 1.4;
}
double CWorkStatus::GamaQ_I()
{	//�������ط���ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		return coefIce.gamaQ;
	else
		return 1.4;
}
double CWorkStatus::GamaQ_A()
{	//��װ���ط���ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		return coefAn.gamaQ;
	else
		return 1.4;
}
double CWorkStatus::GamaQ_U()
{	//���ߡ������ȸ������ط���ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		return coefU.gamaQ;
	else
		return 1.4;
}
double CWorkStatus::GamaQ_T()
{	//�¶Ⱥ��ط���ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		return coefT.gamaQ;
	else
		return 1.4;
}
double CWorkStatus::GamaQ_Eh()
{	//ˮƽ�������ط���ϵ��
	if(m_cMainLoadType==5||m_cMainLoadType==7)
		return 1.3;
	else if(m_cMainLoadType==6)
		return 0.5;
	else
		return 0.0;
}
double CWorkStatus::GamaQ_Ev()
{	//���������ط���ϵ��
	if(m_cMainLoadType==6||m_cMainLoadType==8)
		return 1.3;
	else if(m_cMainLoadType==5)
		return 0.5;
	else
		return 0.0;
}
double CWorkStatus::SetKesai(double kesai)
{	//��������ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		Fai=kesai;
	return Kesai();
}
double CWorkStatus::Kesai()
{	//�ɱ�������ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		return Fai;
	else
	{
		if(m_cMainLoadType<=2)	//������������
			return 1.0;
		else if(m_cMainLoadType>=5)	//���𹤿�
			return 1.0;
		else if(m_bCheckWorkCond)	//���㹤��
			return 0.75;
		else if(m_cMainLoadType==3||m_cMainLoadType==4)	//���߻�װ�������ȸ���
			return 0.9;
		else
			return 1.0;
	}
}
double CWorkStatus::SetKesai_W(double kesai)
{	//��������ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		coefWind.Kesai=kesai;
	return Kesai_W();
}
double CWorkStatus::Kesai_W()	//��������ϵ��
{
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		return coefWind.Kesai;
	else if(m_cMainLoadType>=5)	//���𹤿�
		return 0.3;
	else if(m_cMainLoadType<=2)
		return 1.0;
	else if(m_bCheckWorkCond)
		return 0.75;
	else if(m_cMainLoadType==3||m_cMainLoadType==4)
		return 0.9;
	else 
		return 1.0;
}
double CWorkStatus::SetKesai_I(double kesai)
{	//�����������ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		coefIce.Kesai=kesai;
	return Kesai_I();
}
double CWorkStatus::Kesai_I()	//�����������ϵ��
{
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		return coefIce.Kesai;
	else if(m_cMainLoadType>=5)
		return 0.0;
	else if(m_cMainLoadType<=2)
		return 1.0;
	else if(m_bCheckWorkCond)
		return 0.75;
	else if(m_cMainLoadType==3||m_cMainLoadType==4)
		return 0.9;
	else 
		return 1.0;
}
double CWorkStatus::SetKesai_A(double kesai)
{	//��װ�������ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		coefAn.Kesai=kesai;
	return Kesai_A();
}
double CWorkStatus::Kesai_A()	//��װ�������ϵ��
{
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		return coefAn.Kesai;
	else if(m_cMainLoadType>=5)
		return 0.0;
	else if(m_bCheckWorkCond)
		return 0.75;
	else if(m_cMainLoadType<3)	//�������й���
		return 1.0;
	else //if(m_cMainLoadType==4||5)	//��װ�����߻򲻾��ȸ���
		return 0.9;
}
double CWorkStatus::SetKesai_U(double kesai)
{	//���ߡ������ȸ����������ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		coefU.Kesai=kesai;
	return Kesai_U();
}
double CWorkStatus::Kesai_U()	//���ߡ������ȸ����������ϵ��
{
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		return coefU.Kesai;
	else if(m_cMainLoadType>=5)
		return 0.0;
	else if(m_cMainLoadType<=2)
		return 1.0;
	else if(m_bCheckWorkCond)
		return 0.75;
	else if(m_cMainLoadType==3||m_cMainLoadType==4)
		return 0.9;
	else 
		return 1.0;
}
double CWorkStatus::SetKesai_T(double kesai)
{	//�¶Ⱥ������ϵ��
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		coefT.Kesai=kesai;
	return Kesai_T();
}
double CWorkStatus::Kesai_T()	//�¶Ⱥ������ϵ��
{
	if((SuperiorTower()&&SuperiorTower()->m_iLoadExecuteCode==3)||m_bSpecLoadFactors)
		return coefT.Kesai;
	else if(m_cMainLoadType>=5)
		return 0.0;
	else if(m_cMainLoadType<=2)
		return 1.0;
	else if(m_bCheckWorkCond)
		return 0.75;
	else if(m_cMainLoadType==3||m_cMainLoadType==4)
		return 0.9;
	else 
		return 1.0;
}
#endif
void CWorkStatus::FromRawBuffer(CBuffer &buffer,long version/*=0*/)	//���ڴ���ؼ�����������ݿ�
{	//�̶����Ƚṹ������Ϣ
	buffer.ReadDouble(&ice_thick);			//�����������(mm)
	buffer.ReadInteger(&WindLoad.azimuth);
	buffer.ReadDouble(&WindLoad.velocity);		//�����
	buffer.ReadByte(&m_cMainLoadType);			//0:���1:����2:����3:���߼������ȸ���4:��װ5:ˮƽ����6:�������7:��ˮƽ����8:���������
	buffer.ReadBooleanThin(&m_bCheckWorkCond);	//���㹤��
	buffer.ReadByte(&m_cHoriEarthQuakeAxis);	//ˮƽ��������ʱ���񶯷���ǡ㣬'X'=0��'Y'=90
	if(m_cHoriEarthQuakeAxis=='X')
		m_cHoriEarthQuakeAxis=0;
	else //if(m_cHoriEarthQuakeAxis=='Y')
		m_cHoriEarthQuakeAxis=90;
	//m_bSpecLoadFactors;	//m_bSpecLoadFactors��Ҫ���ڸ�������ؼ��㣬����ؼ�������޹أ��ʴ����Բ����޸� wjh-2014.5.27
	m_bLoadIncFactor=true;	//�Ӻ��ؼ��������ĺ��������Ѱ�������ϵ��, �ʴ�������Ϊtrue
	buffer.ReadString(description);					//���ڹ����ļ�Ҫ����
	buffer.ReadString(vibration_mode_str);		//���ͷֽⷴӦ�׷�����ʱ��Ҫ���ǵ������б�
}
void CWorkStatus::ToRawBuffer(CBuffer &buffer,long nVersion/*=0*/,long doc_type/*=3*/)	//��������ؼ�����򵼳����ݿ�
{
	//�̶����Ƚṹ������Ϣ
	buffer.WriteDouble(ice_thick);			//�����������(mm)
	buffer.WriteInteger(WindLoad.azimuth);
	buffer.WriteDouble(WindLoad.velocity);		//�����
	buffer.WriteByte(m_cMainLoadType);			//0:���1:����2:����3:���߼������ȸ���4:��װ5:ˮƽ����6:�������7:��ˮƽ����8:���������
	buffer.WriteBooleanThin(m_bCheckWorkCond);	//���㹤��
	buffer.WriteByte(m_cHoriEarthQuakeAxis);	//ˮƽ��������ʱ���񶯷���ǡ㣬'X'=0��'Y'=90
	//��ԭʼ����������صļ�����Ϣ
	buffer.WriteString(description);		//���ڹ����ļ�Ҫ����
	buffer.WriteString(vibration_mode_str);	//���ͷֽⷴӦ�׷�����ʱ��Ҫ���ǵ������б�
	//TODO: ��ʱ��Ҫ���ƵĴ���
	buffer.WriteInteger(0);//hashMergeSrcCases.GetNodeNum());//�洢ԭʼ���ع�����ʶ��,keyֵΪ���߷�����ʶ��
	//����ԭ�а汾δ���ļ��м��ع�����ģ���Ĺ�����Ϣ�����ⲿ����Ϣֻ�ܸ���
	/*for(long *pRawCaseId=hashMergeSrcCases.GetFirst();pRawCaseId;pRawCaseId=hashMergeSrcCases.GetNext())
	{
		buffer.WriteInteger(hashMergeSrcCases.GetCursorKey());	//���߷�����ʶ��
		buffer.WriteInteger(*pRawCaseId);						//���߷�����ԭʼ���ع�����ʶ��
	}*/
}
void CWorkStatus::FromBasicCoefBuffer(CBuffer &buffer)	//���ڹ����俽������ϵ������������
{
	buffer.ReadInteger(&iNo);
	//buffer.ReadInteger(&iWireLayoutNo);
	buffer.ReadString(description);
	buffer.ReadDouble(&Fai);
	buffer.ReadDouble(&GamaQi);
	buffer.ReadDouble(&ice_thick);
	buffer.ReadDouble(&WindLoad.velocity);
	buffer.ReadInteger(&WindLoad.azimuth);
	buffer.ReadBooleanThin(&m_bCheckWorkCond);
	buffer.ReadByte(&m_cMainLoadType);

	buffer.ReadDouble(&coefIce.B_I);
	buffer.ReadDouble(&coefIce.gamaQ);
	buffer.ReadDouble(&coefIce.Kesai);
	buffer.ReadDouble(&coefWind.gamaQ);
	buffer.ReadDouble(&coefWind.Kesai);
	buffer.ReadDouble(&coefAn.gamaQ);
	buffer.ReadDouble(&coefAn.Kesai);
	buffer.ReadDouble(&coefU.gamaQ);
	buffer.ReadDouble(&coefU.Kesai);
	buffer.ReadDouble(&coefT.gamaQ);
	buffer.ReadDouble(&coefT.Kesai);
	buffer.ReadDouble(&gamaEh);
	buffer.ReadDouble(&gamaEv);

	buffer.ReadBooleanThin(&m_bSpecLoadFactors);
	buffer.ReadBooleanThin(&m_bLoadIncFactor);

	buffer.ReadByte(&m_cHoriEarthQuakeAxis);
	buffer.ReadString(vibration_mode_str);
	buffer.ReadString(limit_str);
}
void CWorkStatus::ToBasicCoefBuffer(CBuffer &buffer)	//���ڹ����俽������ϵ������������
{
	buffer.WriteInteger(iNo);
	//buffer.WriteInteger(iWireLayoutNo);
	buffer.WriteString(description);
	buffer.WriteDouble(Fai);
	buffer.WriteDouble(GamaQi);
	buffer.WriteDouble(ice_thick);
	buffer.WriteDouble(WindLoad.velocity);
	buffer.WriteInteger(WindLoad.azimuth);
	buffer.WriteBooleanThin(m_bCheckWorkCond);
	buffer.WriteByte(m_cMainLoadType);

	buffer.WriteDouble(coefIce.B_I);
	buffer.WriteDouble(coefIce.gamaQ);
	buffer.WriteDouble(coefIce.Kesai);
	buffer.WriteDouble(coefWind.gamaQ);
	buffer.WriteDouble(coefWind.Kesai);
	buffer.WriteDouble(coefAn.gamaQ);
	buffer.WriteDouble(coefAn.Kesai);
	buffer.WriteDouble(coefU.gamaQ);
	buffer.WriteDouble(coefU.Kesai);
	buffer.WriteDouble(coefT.gamaQ);
	buffer.WriteDouble(coefT.Kesai);
	buffer.WriteDouble(gamaEh);
	buffer.WriteDouble(gamaEv);

	buffer.WriteBooleanThin(m_bSpecLoadFactors);
	buffer.WriteBooleanThin(m_bLoadIncFactor);

	buffer.WriteByte(m_cHoriEarthQuakeAxis);
	buffer.WriteString(vibration_mode_str);
	buffer.WriteString(limit_str);
}
void CWorkStatus::FromBuffer(CBuffer &buffer,long nVersion/*=0*/,long doc_type/*=3*/)
{
	buffer.ReadInteger(&iNo);
#if defined(__TSA_)||defined(__TSA_FILE_)
	if(nVersion==0||nVersion>=1070101)//,"1.7.1.1")>=0)
		buffer.ReadInteger(&iWireLayoutNo);
#elif defined(__LDS_)||defined(__LDS_FILE_)
	if(nVersion==0||nVersion>=1000101)//,"1.0.1.1")>=0)
		buffer.ReadInteger(&iWireLayoutNo);
#endif
	buffer.ReadString(description);
	buffer.ReadDouble(&Fai);
	buffer.ReadDouble(&GamaQi);
	buffer.ReadDouble(&ice_thick);
	/*buffer.Read(cfgword.word,20);
	buffer.ReadDword(&dwQuad1LegCfgNo);
	buffer.ReadDword(&dwQuad2LegCfgNo);
	buffer.ReadDword(&dwQuad3LegCfgNo);
	buffer.ReadDword(&dwQuad4LegCfgNo);*/
	if(nVersion==0||nVersion>=1000004)//,"1.0.0.4")>=0)
		buffer.ReadDouble(&WindLoad.velocity);
	else
	{
		long v;
		buffer.ReadInteger(&v);
	}
	buffer.ReadInteger(&WindLoad.azimuth);
	if(nVersion>0&&nVersion<1000011)//!=NULL&&compareVersion(version,"1.0.0.11")<0)
	{
		double case_coef;	//ԭָ��������ʱ�ĸ�������ص���ϵ��������ת��ϵͳ����
		buffer.ReadDouble(&case_coef);
	}
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
#if defined(__LDS_)||defined(__LDS_FILE_)
	if(nVersion==0||nVersion>=1000013)//,"1.0.0.13")>=0)
#elif defined(__TSA_)||defined(__TSA_FILE_)
	if(nVersion==0||nVersion>=1070002)//,"1.7.0.2")>=0)
#endif
	{
#if defined(__TSA_)||defined(__TSA_FILE_)
		if(nVersion==0||nVersion>=1070003)//,"1.7.0.3")>=0)
#endif
		{
			buffer.ReadBooleanThin(&m_bCheckWorkCond);
			buffer.ReadByte(&m_cMainLoadType);
		}
		buffer.ReadDouble(&coefIce.B_I);
		buffer.ReadDouble(&coefIce.gamaQ);
		buffer.ReadDouble(&coefIce.Kesai);
		buffer.ReadDouble(&coefWind.gamaQ);
		buffer.ReadDouble(&coefWind.Kesai);
		buffer.ReadDouble(&coefAn.gamaQ);
		buffer.ReadDouble(&coefAn.Kesai);
		buffer.ReadDouble(&coefU.gamaQ);
		buffer.ReadDouble(&coefU.Kesai);
		buffer.ReadDouble(&coefT.gamaQ);
		buffer.ReadDouble(&coefT.Kesai);
		buffer.ReadDouble(&gamaEh);
		buffer.ReadDouble(&gamaEv);
	}
	if(nVersion==0||((doc_type==3&&nVersion>=2000101)||(doc_type==4&&nVersion>=1020101)))
	{
		buffer.ReadBooleanThin(&m_bSpecLoadFactors);
		buffer.ReadBooleanThin(&m_bLoadIncFactor);
	}
	long i,n,h;
#if defined(__TSA_)||defined(__TSA_FILE_)
	if(nVersion==0||nVersion>=1070001)//,"1.7.0.1")>=0)
#elif defined(__LDS_)||defined(__LDS_FILE_)
	if(nVersion==0||nVersion>=1000012)//,"1.0.0.12")>=0)
#endif
	{
		buffer.ReadInteger(&n);
		for(i=0;i<n;i++)
		{
			buffer.ReadInteger(&h);
			CExternalNodeLoad *pNodeLoad=hashNodeLoad.Add(h);
			buffer.ReadPoint(pNodeLoad->Fxyz);
			buffer.ReadDouble(&pNodeLoad->permanentFz);
			if(m_bLoadIncFactor)
			{
				buffer.ReadPoint(pNodeLoad->stdLoad);
				buffer.ReadPoint(pNodeLoad->designLoad);
			}
			else//if(!m_bLoadIncFactor)
			{
				double gamaG=1.0,safetyCoef=1.0;
				if(SuperiorTower()!=NULL)
				{
					gamaG=SuperiorTower()->m_fGamaGForDesign;
					if(m_cMainLoadType<4)	//��װ���������㹤������Ҫ���ǽṹ��Ҫ��ϵ��
						safetyCoef=SuperiorTower()->SafetyCoef();
				}
				pNodeLoad->designLoad=pNodeLoad->Fxyz*Kesai()*GamaQ()+GEPOINT(0,0,pNodeLoad->permanentFz*gamaG);
				pNodeLoad->designLoad*=safetyCoef;
				pNodeLoad->stdLoad=pNodeLoad->Fxyz+GEPOINT(0,0,pNodeLoad->permanentFz);
			}
		}
	}
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		WINDLOAD_REFPARAM *pWindLoadRef=AttachWindLoad.Add(h);
#if defined(__TSA_)||defined(__TSA_FILE_)
		if(nVersion==0||nVersion>=1070001)//,"1.7.0.1")>=0)
#elif defined(__LDS_)||defined(__LDS_FILE_)
		if(nVersion==0||nVersion>=1000012)//,"1.0.0.12")>=0)
#endif
		{
			buffer.ReadDouble(&pWindLoadRef->attachMass);
			buffer.ReadDouble(&pWindLoadRef->shieldCoef);
		}
	}
#if defined(__TSA_)||defined(__TSA_FILE_)
	if(nVersion==0||nVersion>=1070303)//,"1.7.3.3")>=0)
#elif defined(__LDS_)||defined(__LDS_FILE_)
	if(nVersion==0||nVersion>=1000303)//,"1.0.3.3")>=0)
#endif
	{
		buffer.ReadByte(&m_cHoriEarthQuakeAxis);
		if((doc_type==3&&nVersion<=1070307)||(doc_type==4&&nVersion<=1000307))
		{
			if(m_cHoriEarthQuakeAxis=='X')
				m_cHoriEarthQuakeAxis=0;
			else //if(m_cHoriEarthQuakeAxis=='Y')
				m_cHoriEarthQuakeAxis=90;
		}
		buffer.ReadString(vibration_mode_str);
	}
	buffer.ReadString(limit_str);
#endif
}
void CWorkStatus::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteInteger(iNo);
	buffer.WriteInteger(iWireLayoutNo);
	buffer.WriteString(description);
	buffer.WriteDouble(Fai);
	buffer.WriteDouble(GamaQi);
	buffer.WriteDouble(ice_thick);
	/*buffer.Write(cfgword.word,20);
	buffer.WriteDword(dwQuad1LegCfgNo);
	buffer.WriteDword(dwQuad2LegCfgNo);
	buffer.WriteDword(dwQuad3LegCfgNo);
	buffer.WriteDword(dwQuad4LegCfgNo);*/
	buffer.WriteDouble(WindLoad.velocity);
	buffer.WriteInteger(WindLoad.azimuth);
	buffer.WriteBooleanThin(m_bCheckWorkCond);
	buffer.WriteByte(m_cMainLoadType);
	buffer.WriteDouble(coefIce.B_I);
	buffer.WriteDouble(coefIce.gamaQ);
	buffer.WriteDouble(coefIce.Kesai);
	buffer.WriteDouble(coefWind.gamaQ);
	buffer.WriteDouble(coefWind.Kesai);
	buffer.WriteDouble(coefAn.gamaQ);
	buffer.WriteDouble(coefAn.Kesai);
	buffer.WriteDouble(coefU.gamaQ);
	buffer.WriteDouble(coefU.Kesai);
	buffer.WriteDouble(coefT.gamaQ);
	buffer.WriteDouble(coefT.Kesai);
	buffer.WriteDouble(gamaEh);
	buffer.WriteDouble(gamaEv);
	if(version==0||((doc_type==3&&version>=2000101)||(doc_type==4&&version>=1020101)))
	{
		buffer.WriteBooleanThin(m_bSpecLoadFactors);
		buffer.WriteBooleanThin(m_bLoadIncFactor);
	}
	BUFFERPOP stack(&buffer,hashNodeLoad.GetNodeNum());
	buffer.WriteInteger(hashNodeLoad.GetNodeNum());
	for(CExternalNodeLoad *pExNodeLoad=hashNodeLoad.GetFirst();pExNodeLoad;pExNodeLoad=hashNodeLoad.GetNext())
	{
		buffer.WriteInteger(hashNodeLoad.GetCursorKey());
		buffer.WritePoint(pExNodeLoad->Fxyz);
		buffer.WriteDouble(pExNodeLoad->permanentFz);
		if(m_bLoadIncFactor)
		{
			buffer.WritePoint(pExNodeLoad->stdLoad);
			buffer.WritePoint(pExNodeLoad->designLoad);
		}
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of Node concentrated load is wrong!");
#else
		CLDSObject::Log2File()->Log("�ڵ㼯�к��ؼ�¼�����ִ���!");
#endif
	stack.Initialize(&buffer,AttachWindLoad.GetNodeNum());
	buffer.WriteInteger(AttachWindLoad.GetNodeNum());
	for(WINDLOAD_REFPARAM *pWindLoadRef=AttachWindLoad.GetFirst();pWindLoadRef;pWindLoadRef=AttachWindLoad.GetNext())
	{
		buffer.WriteInteger(AttachWindLoad.GetCursorKey());
		buffer.WriteDouble(pWindLoadRef->attachMass);
		buffer.WriteDouble(pWindLoadRef->shieldCoef);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of Attached Load is wrong!");
#else
		CLDSObject::Log2File()->Log("���ӷ���ؼ�¼�����ִ���!");
#endif
	buffer.WriteByte(m_cHoriEarthQuakeAxis);
	buffer.WriteString(vibration_mode_str);
	buffer.WriteString(limit_str);
}
void CWorkStatus::ToXmlFile(FILE* fp,DWORD schema)
{
	char sText[500]="";
	fprintf(fp,"<������Ϣ iNo=\"%d\" WireLayoutNo=\"%d\" description=\"%s\">\n",iNo,iWireLayoutNo,description);
	sprintf(sText,"%f",Fai);
	SimplifiedNumString(sText);
	fprintf(fp,"<�ɱ�������ϵ�� Fai=\"%s\"/>\n",sText);
	sprintf(sText,"%f",GamaQi);
	SimplifiedNumString(sText);
	fprintf(fp,"<�ɱ���ط���ϵ�� GamaQi=\"%s\"/>\n",sText);
	sprintf(sText,"%f",ice_thick);
	SimplifiedNumString(sText);
	fprintf(fp,"<������� ice_thick=\"%s\"/>\n",sText);
	fprintf(fp,"<����� velocity=\"%.2f\" azimuth=\"%d\"/>\n",WindLoad.velocity,WindLoad.azimuth);
	if(m_bCheckWorkCond)
		strcpy(sText,"true");
	else 
		strcpy(sText,"false");
	fprintf(fp,"<���㹤�� bCheckWorkCond=\"%s\"/>\n",sText);
	fprintf(fp,"<�������� MainLoadType=\"%c\"/>\n",m_cMainLoadType);
	fprintf(fp,"<�����������ȡֵϵ�� B_I=\"%.2f\" gamaQ=\"%.2f\" Kesai=\"%.2f\"/>\n",coefIce.B_I,coefIce.gamaQ,coefIce.Kesai);
	fprintf(fp,"<����ص����ȡֵϵ�� gamaQ=\"%.2f\" kesai=\"%.2f\"/>\n",coefWind.gamaQ,coefWind.Kesai);
	fprintf(fp,"<��װ�������ȡֵϵ�� gamaQ=\"%.2f\" kesai=\"%.2f\"/>\n",coefAn.gamaQ,coefAn.Kesai);
	fprintf(fp,"<�������������ȡֵϵ�� gamaQ=\"%.2f\" kesai=\"%.2f\"/>\n",coefU.gamaQ,coefU.Kesai);
	fprintf(fp,"<�¶����ú��ص����ȡֵϵ�� gamaQ=\"%.2f\" kesai=\"%.2f\"/>\n",coefT.gamaQ,coefT.Kesai);
	sprintf(sText,"%f",gamaEh);
	SimplifiedNumString(sText);
	fprintf(fp,"<ˮƽ�������ú��ط���ϵ�� gamaEh=\"%s\"/>\n",sText);
	sprintf(sText,"%f",gamaEv);
	SimplifiedNumString(sText);
	fprintf(fp,"<����������ú��ط���ϵ�� gamaEv=\"%s\"/>\n",sText);
	fprintf(fp,"<���ؽڵ����� NodeNum=\"%d\"/>\n",hashNodeLoad.GetNodeNum());
	for(CExternalNodeLoad *pExNodeLoad=hashNodeLoad.GetFirst();pExNodeLoad;pExNodeLoad=hashNodeLoad.GetNext())
	{
		fprintf(fp,"<���ؽڵ���Ϣ key=\"%d\">\n",hashNodeLoad.GetCursorKey());
		fprintf(fp,"<XYZ����ļ��пɱ������ֵ Fx=\"%.2f\" Fy=\"%.2f\" Fz=\"%.2f\"/>\n",pExNodeLoad->Fxyz.x,pExNodeLoad->Fxyz.y,pExNodeLoad->Fxyz.z);
		fprintf(fp,"<�����������ú���ֵ permanentFz=\"%.2f\"/>\n",pExNodeLoad->permanentFz);
		fprintf(fp,"</���ؽڵ���Ϣ>\n");
	}
	fprintf(fp,"<���ӷ���ص���ϵ������ num=\"%d\"/>\n",AttachWindLoad.GetNodeNum());
	for(WINDLOAD_REFPARAM *pWindLoadRef=AttachWindLoad.GetFirst();pWindLoadRef;pWindLoadRef=AttachWindLoad.GetNext())
	{
		fprintf(fp,"<���ӷ���ص���ϵ����Ϣ key=\"%d\">\n",AttachWindLoad.GetCursorKey());
		sprintf(sText,"%f",pWindLoadRef->attachMass);
		SimplifiedNumString(sText);
		fprintf(fp,"<������ʩ�����ĵ�Ч�������� attachMass=\"%s\"/>\n",sText);
		sprintf(sText,"%f",pWindLoadRef->shieldCoef);
		SimplifiedNumString(sText);
		fprintf(fp,"<����ϵ�� shieldCoef=\"%s\"/>\n",sText);
		fprintf(fp,"</���ӷ���ص���ϵ����Ϣ>\n");
	}
	fprintf(fp,"<ˮƽ��������ʱ���񶯷���� HoriEarthQuakeAxis=\"%c\"/>\n",m_cHoriEarthQuakeAxis);
	fprintf(fp,"<����ģʽ vibration_mode=\"%s\"/>\n",vibration_mode_str);
	fprintf(fp,"<����ģ�� limit_str=\"%s\"/>\n",limit_str);
	fprintf(fp,"</������Ϣ>\n");
}
void CWorkStatus::Serialize(CArchive &ar)
{
	/*if(ar.IsStoring())
	{
	}
	else
	{
	}*/
}
int CWorkStatus::SetEiffelCaseNo(int iCaseNo)
{
	if(m_cMainLoadType==0)
		m_iEiffelCaseNo=iCaseNo;
	return m_iEiffelCaseNo;
}
int CWorkStatus::GetEiffelCaseNo()
{
	if(m_cMainLoadType==0&&(WindLoad.azimuth%90==0))
		return m_iEiffelCaseNo;
	else
		return 0;
}
CXhChar16 CWorkStatus::ToCaseNoString()
{
	int caseno=GetEiffelCaseNo();
	if(caseno>0)
		return CXhChar16("%d%C",iNo,caseno+'@');	//'A'-1='@'
	else
		return CXhChar16("%d",iNo);
}

long CWorkStatus::GetEiffelStatusId()	//���ذ������ƶ�ЧӦ�����ŵ����Id
{
	return SUBID(handle,GetEiffelCaseNo());
}

long CWorkStatus::GetEiffelStatusNo()	//���ذ������ƶ�ЧӦ�����ŵ����Id
{
	return SUBID(iNo,GetEiffelCaseNo());
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//��������Ĺؼ���key������Ӧ��Hashӳ��ֵ��ͬʱ��֤С��key��ǰ���key�ں�
DWORD Hash2Key(DWORD key1,DWORD key2,DWORD nHashSize)
{
	HASH2KEY hashkey(min(key1,key2),max(key1,key2));
	DWORD key=hashkey.HashCode();
	if(nHashSize>0)
		return key%nHashSize;
	else
		return 0;
}
void LoadElemKeyInfo(CElemInfo* pElemInfo,DWORD key1,DWORD key2)
{
	pElemInfo->m_hStartNode=key1;
	pElemInfo->m_hEndNode=key2;
}
void LoadSubGeometryEnt(SUB_OBJ* pSubObj,DWORD serial)
{
	pSubObj->iSubId=serial;
}
void* SUB_OBJ::SubObj(IModel* pModel){
	return pSubObj;
}
/////////////////////////////////////////////////////////////
PROP_OBJ::PROP_OBJ(CLDSObject* _pObj/*=NULL*/){
	memset(this,0,sizeof(SUB_OBJ));
	ciObjType=TYPE_LDSOBJ;
	obj.pLdsObj=_pObj;
}
PROP_OBJ::PROP_OBJ(CLDSBolt* _pBolt,long _hResidePart){
	memset(this,0,sizeof(SUB_OBJ));
	ciObjType=TYPE_HOLEWALL;
	obj.hole.pBolt=pBolt;
	obj.hole.hResidePart=_hResidePart;
}
CLDSObject *PROP_OBJ::get_LdsObject(){
	return (ciObjType==TYPE_LDSOBJ)?this->obj.pLdsObj:NULL;
}
CLDSObject *PROP_OBJ::set_LdsObject(CLDSObject* pPropObj){
	if(pPropObj==NULL)
		return NULL;
	ciObjType=TYPE_LDSOBJ;
	return this->obj.pLdsObj=pPropObj;
}
//LDS�����ϵ��߿�ͼԪ�Ӷ�������
long PROP_OBJ::get_hObject(){
	if(ciObjType==TYPE_LDSOBJ&&obj.pLdsObj!=NULL)
		return obj.pLdsObj->handle;
	else if(ciObjType==TYPE_HOLEWALL)
		return obj.hole.pBolt->handle;
	else if(ciObjType==TYPE_DBENTITY)
		return obj.subobj.hObj;
	else
		return 0;
}
long PROP_OBJ::SubId(){
	return (ciObjType==TYPE_DBENTITY)?obj.subobj.iSubId:0;
}
void* PROP_OBJ::SubObj(){
	return (ciObjType==TYPE_DBENTITY)?obj.subobj.pSubObj:NULL;
}
//�����ϵĿױ�ʵ�帴�϶���
CLDSBolt* PROP_OBJ::set_pBolt(CLDSBolt* pHoleBolt){
	if(pHoleBolt==NULL)
		return NULL;
	ciObjType=TYPE_HOLEWALL;
	return obj.hole.pBolt=pHoleBolt;
}
CLDSBolt* PROP_OBJ::get_pBolt(){
	return (ciObjType==TYPE_HOLEWALL)?obj.hole.pBolt:NULL;
}
long PROP_OBJ::set_hResidePart(long hPart){
	if(hPart==0)
		return 0;
	ciObjType=TYPE_HOLEWALL;
	return obj.hole.hResidePart=hPart;
}
long PROP_OBJ::get_hResidePart(){
	return (ciObjType==TYPE_HOLEWALL)?obj.hole.hResidePart:NULL;
}
/////////////////////////////////////////////////////////////
double CTower::W0_EIFFEL_FACTOR=0.3;	//����簣�ƶ�ЧӦ��ѹ�ۼ�ϵ��,�ݰ��淶ȷ��
#ifdef __PART_DESIGN_INC_
DefWriteWeatherFunc CTower::WriteWireLayoutWeather=NULL;
DefReadWireLayoutDataFunc CTower::ReadWireLayoutData=NULL;
#endif
static void LoadDesignParamItemInfo(DESIGN_PARAM_ITEM *pItem,DWORD key){pItem->key=key;}
CTower::CTower()
{
	m_bInitNewObjDefaultInfo=true;
	m_ciRepositionAlgor=0;	//��ʾ��ǰ�����㷨
	m_ciRollProcessType=0;
	m_ciRollOffsetType=0;
	m_ciDeformType=1;		//�������η�ʽĬ��Ϊ���Բ�
	m_fDeformCoef=0.35;		//�����������Բ�ϵ��
	m_fHuoquR=0;			//�����뾶
	m_uiOriginalDogKey=0;
	m_uiLastSaveDogKey = 0;
	m_nFileDivisionPos=0;
	m_bCooperativeWork=FALSE;
	m_cStructureType=0;
	m_idServerFile=0;
	m_wiVerSerial=0;//��ǰ���ļ��洢��� Ĭ��ֵΪ0
	m_bDataModified=false;
	handle = 0x20;
	//M16��˨ѡ������
	BOLT_SELCOND *pSelBolt=listSelBoltCond.append();
	pSelBolt->d=16;
	strcpy(pSelBolt->grade,"4.8");
	strcpy(pSelBolt->matStr,"*");
	pSelBolt->upWidthLim=56;
	//M20��˨ѡ������
	pSelBolt=listSelBoltCond.append();
	pSelBolt->d=20;
	strcpy(pSelBolt->grade,"6.8");
	strcpy(pSelBolt->matStr,"*");
	pSelBolt->upWidthLim=140;
	//M24��˨ѡ������
	pSelBolt=listSelBoltCond.append();
	pSelBolt->d=24;
	strcpy(pSelBolt->grade,"8.8");
	strcpy(pSelBolt->matStr,"*");
	pSelBolt->upWidthLim=250;
	//��ʼ����������
		//ģ����ͼ
	View.SetDocModifiedFlagAddress(&m_bDataModified);
	Module.SetDocModifiedFlagAddress(&m_bDataModified);
		//����ͼԪ
	Point.SetDocModifiedFlagAddress(&m_bDataModified);
	Line.SetDocModifiedFlagAddress(&m_bDataModified);
	Plane.SetDocModifiedFlagAddress(&m_bDataModified);
	ArcLift.SetDocModifiedFlagAddress(&m_bDataModified);
		//���ݶ���
	Node.SetDocModifiedFlagAddress(&m_bDataModified);
	Parts.SetDocModifiedFlagAddress(&m_bDataModified);
	BlockRef.SetDocModifiedFlagAddress(&m_bDataModified);

	View.SetBelongModel(this);
	Module.SetBelongModel(this);
	Point.SetBelongModel(this);
	Line.SetBelongModel(this);
	Plane.SetBelongModel(this);
	ArcLift.SetBelongModel(this);
	Node.SetBelongModel(this);
	Parts.SetBelongModel(this);
	Block.SetBelongModel(this);
	BlockRef.SetBelongModel(this);
	Foundations.SetBelongModel(this);
	m_bGroupUndoStart=false;

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	SuppPart.SetDocModifiedFlagAddress(&m_bDataModified);
	PartGroup.SetDocModifiedFlagAddress(&m_bDataModified);
	SuppPart.SetBelongModel(this);
	PartGroup.SetBelongModel(this);
#endif
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	Elems.SetHashFunc(Hash2Key);
	Elems.LoadDefaultObjectInfo=LoadElemKeyInfo;
	WorkStatus.SetDocModifiedFlagAddress(&m_bDataModified);	//����״��
	AttachWindLoad.SetDocModifiedFlagAddress(&m_bDataModified);	//���ӷ����
	WorkStatus.SetBelongModel(this);	//����״��
	AttachWindLoad.SetBelongModel(this);	//���ӷ����
#endif
	hashParams.LoadDefaultObjectInfo=LoadDesignParamItemInfo;
	//m_bufferDrawingScript.ClearBuffer();	//��ջ�ͼ�ű�
	//m_bufferSumTaMatCfg.ClearBuffer();		//��ղ��ϻ��ܱ������ļ�
	DisplayProcess=NULL;
	ParseDrawingScriptBuffer=NULL;			//������ͼ�ű�����
	GetNewHandleHookFunc=NULL;
	//���Ӹ��ӻ���
	AttachBuffer.Add(BOM_CONFIG);
	AttachBuffer.Add(SMART_DRAW);
	AttachBuffer.Add(PRJPARAMPART_LIBRAY);
	AttachBuffer.Add(PPE_MODEL);
	AttachBuffer.Add(BOM_COMPARE_MODEL);
	AttachBuffer.Add(MODEL_SIZE_CHECK);
#ifdef __PART_DESIGN_INC_
	AttachBuffer.Add(TOWER_MODEL);
	AttachBuffer.Add(LOGGER);
	AttachBuffer.Add(WIRELOAD_MODEL);
	AttachBuffer.Add(RSLT_REPORT);
	AttachBuffer.Add(TSA_TURBO_MODEL);
	AttachBuffer.Add(UNI_WIREPOINT_MODEL);
#endif
	//�����ڻ�δ����֤��,���������ʼ������LDS.cpp�е���֤���ִ��
	//InitTower();
	//InitModule();
	//ȱʡĬ�ϵ�����������ԳƱ任
	CParaCS::PARAMETRIC_INFO param;
	param.m_cDefStyle=CParaCS::CS_MIRTRANS;
	param.m_xMirInfo=MIRMSG(1);
	defaultAcsMirX.SetParamInfo(param);
	param.m_xMirInfo=MIRMSG(2);
	defaultAcsMirY.SetParamInfo(param);
	param.m_xMirInfo=MIRMSG(3);
	defaultAcsMirZ.SetParamInfo(param);
	GeEnts.LoadDefaultObjectInfo=LoadSubGeometryEnt;
	Acs.SetMinmalId(4);	//1~3�������̶�X/Y/Z��ԳƱ任
	m_pExterModel=NULL;
}

CTower::~CTower()
{
	Empty();
}
//�˺�����ModDataFile.cpp��FindLowestHorizArmControlNode()��ֲ���� wjh-2018.8.9
double CTower::_IntelliCalNamedHeightZeroZ(bool *pblCalSuccessed/*=NULL*/)
{
#ifdef __STATION_
	return 0;
#else
	if(pblCalSuccessed)
		*pblCalSuccessed=true;
	//ͳ��ˮƽ�ᵣ���ļ��������ˮƽ���
	CMapList<CLDSLinePart*> hashHoriTrunkRods;
	LINEPARTSET horizArmRodSet;
	CMaxDouble maxBtmArmZ;
	for(CLDSLinePart* pRod=EnumRodFirst();pRod;pRod=EnumRodNext())
	{
		if(pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		if(pRod->chLayer1st=='T'&&pRod->chLayer2nd=='Z')
		{
			maxBtmArmZ.Update(pRod->pStart->xOriginalPos.z);
			maxBtmArmZ.Update(pRod->pEnd->xOriginalPos.z);
			if (fabs(pRod->pStart->xOriginalPos.z - pRod->pEnd->xOriginalPos.z) < 10)
				horizArmRodSet.append(pRod);	//��ͷ���ӽ���ˮƽ������
			continue;
		}
		else if(pRod->chLayer1st!='S')
			continue;
		CLDSLinePart *pStartRod=FromRodHandle(pRod->pStart->hFatherPart);
		CLDSLinePart *pEndRod=FromRodHandle(pRod->pEnd->hFatherPart);
		if( pStartRod==NULL||pEndRod==NULL||pStartRod== pRod ||pEndRod== pRod ||
			toupper(pStartRod->Layer(1))!='Z'||toupper(pEndRod->Layer(1))!='Z')
			continue;
		GEPOINT vLenStdVec=(pRod->pEnd->xOriginalPos-pRod->pStart->xOriginalPos);
		normalize(vLenStdVec);
		if(fabs(vLenStdVec.x)<EPS_COS2&&fabs(vLenStdVec.y)<EPS_COS2)
			continue;
		hashHoriTrunkRods.SetValue(pRod->pStart->handle, pRod->pEnd->handle, pRod);
	}
	CLDSNode* pNode,*pMaxZNode=NULL;
	CMaxDouble lowestHorizNodeZ;
#ifdef __RAPID_ROUGH_3DMODEL_
	if(horizArmRodSet.GetNodeNum()<=0)
	{
		for(pNode=EnumNodeFirst();pNode;pNode=EnumNodeNext())
		{	//�Եڶ����޵Ľڵ�Ϊ��׼�ڵ㣬���ݶԳƵ�ȷ�����������
			if(pNode->wireplace.ciWireType!=WIREPLACE_CODE::CONDUCTOR_WIRE)
				continue;
			lowestHorizNodeZ.Update(pNode->xOriginalPos.z);
		}
		if(pblCalSuccessed)
			*pblCalSuccessed=lowestHorizNodeZ.IsInited();
		if(lowestHorizNodeZ.IsInited())
			return lowestHorizNodeZ.number;
		else
			return 0;
	}
#endif
	//CMinDouble maxTopArmZ;
	double dfTopTrunkZ = 0;
	for(pNode=EnumNodeFirst();pNode;pNode=EnumNodeNext())
	{	//�Ե�һ���޵Ľڵ�Ϊ��׼�ڵ㣬���ݶԳƵ�ȷ�����������
		if(pNode->chLayer1st!='S'||pNode->chLayer3rd!='1'|| pNode->xOriginalPos.x < 0 || pNode->xOriginalPos.y < 0)
			continue;
		if (maxBtmArmZ.IsInited() && pNode->xOriginalPos.z > maxBtmArmZ.number + 1000)
			continue;
		int iPush=PushNodeStack();
		CLDSNode* pMirX = pNode->GetMirNode(MIRMSG(1));
		CLDSNode* pMirY = pNode->GetMirNode(MIRMSG(2));
		CLDSNode* pMirZ = pNode->GetMirNode(MIRMSG(3));
		PopNodeStack(iPush);
		if(pMirX==NULL || pMirY==NULL || pMirZ==NULL)		//�������ԳƵĽڵ�
			continue;
		if( (hashHoriTrunkRods.GetValue(pNode->handle,pMirX->handle)==NULL&&hashHoriTrunkRods.GetValue(pMirX->handle,pNode->handle)==NULL)||
			(hashHoriTrunkRods.GetValue(pNode->handle,pMirY->handle)==NULL&&hashHoriTrunkRods.GetValue(pMirY->handle,pNode->handle)==NULL)||
			(hashHoriTrunkRods.GetValue(pMirZ->handle,pMirX->handle)==NULL&&hashHoriTrunkRods.GetValue(pMirX->handle,pMirZ->handle)==NULL)||
			(hashHoriTrunkRods.GetValue(pMirZ->handle,pMirY->handle)==NULL&&hashHoriTrunkRods.GetValue(pMirY->handle,pMirZ->handle)==NULL))
			continue;	//�Գƽڵ�������Ӹ˼�
		//�ж��Ƿ�Ϊ�ᵣ���ĺ�����
		for(CLDSLinePart* pHorizArmRod=horizArmRodSet.GetFirst();pHorizArmRod;pHorizArmRod=horizArmRodSet.GetNext())
		{
			if (pHorizArmRod->pStart!=pNode&&pHorizArmRod->pEnd!=pNode)
				continue;
			if (lowestHorizNodeZ.Update(pNode->xOriginalPos.z) == pNode->xOriginalPos.z)
				pMaxZNode = pNode;
			break;
		}
	}
	if(pMaxZNode!=NULL)
		return pMaxZNode->xOriginalPos.z;
	else //if(pMaxZNode==NULL)
#ifdef __RAPID_ROUGH_3DMODEL_
	{
		CMaxDouble lowestWireNodeZ;
		for(pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
		{
			if(pNode->wireplace.ciWireType!=0)
				lowestWireNodeZ.Update(pNode->xOriginalPos.z,pNode);
		}
		if(!lowestWireNodeZ.IsInited()&&pblCalSuccessed)
			*pblCalSuccessed=false;

		return lowestWireNodeZ.IsInited()?lowestWireNodeZ.number:0;
	}
#else
		if(pblCalSuccessed)
			*pblCalSuccessed=false;
		return 0;
#endif
#endif
}
double CTower::set_NamedHeightZeroZ(double fNamedHeightZeroZ)
{
	m_bUserSpecifyNamedHeightZeroZ=true;
	return m_fNamedHeightZeroZ=fNamedHeightZeroZ;
}
double CTower::get_NamedHeightZeroZ()
{
	if(m_bUserSpecifyNamedHeightZeroZ)
		return m_fNamedHeightZeroZ;
	else
		return m_fNamedHeightZeroZ=_IntelliCalNamedHeightZeroZ();
}

BOOL CTower::SetDocModifiedFlag(BOOL bModified/*=TRUE*/)
{
	m_bDataModified=bModified;
	return m_bDataModified;
}
BOOL CTower::IsDataModified()
{
	return m_bDataModified;
}
#ifdef __PART_DESIGN_INC_
static void LoadAngleJointIdFunc(ANGLE_JOINT* pAngleJoint,DWORD id){pAngleJoint->InitId(id);}
#endif
void CTower::InitTower()
{
	ucs.axis_x.x =  1;
	ucs.axis_y.z =  1;
	ucs.axis_z.y = -1;
	handle = 0x20;
	m_fMaxXieJGLen =8500.0;
	cfgword.flag.word[0] = 1;
	m_iCurLeg = 1;
	strcpy(m_sPrjName,"");
	strcpy(m_sTaType,"");
	strcpy(m_sTaAlias,"");
	strcpy(m_sTaStampNo,"");
	strcpy(m_sOperator,"");
	strcpy(m_sCritic,"");	//����
	strcpy(m_sAuditor,"");	//���
	m_ciRepositionAlgor=0;
	strcpy(segstr,"*");
	m_nTaNum= 1;
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(weatherCode,"Unknown");
#else
	strcpy(weatherCode,"δ֪");
#endif
	weather.m_nHighestT=40;	//�������
	weather.m_nLowestT=-10;	//�������
	weather.m_nAverageT=0;	//ƽ������
	weather.m_fMaxWindV=26.8;	//������
	weather.m_nMaxWindT=0;		//������ʱ�¶�
	weather.m_fMaxIceThick=15;			//��󸲱���(mm)
	weather.m_nMaxIceT=-5;				//��󸲱�ʱ�¶�
	weather.m_fMaxIceWindV=10;			//��󸲱�ʱ����
	weather.m_nSetupT=0;				//��װʱ�¶�
	weather.m_fSetupWindV=10;		//��װʱ����

	volt_grade=500;	//��ѹ�ȼ�:330,500
	m_cLandscape='B';	//��λ����:'A','B','C'
	m_cLineRoadType=0;	//ƽ����·
	/*��ȫ�ȼ�(�����ṹ��Ҫ��ϵ��gama0)��
	0:һ���ر���Ҫ�ĸ����ṹ(gama0=1.1)��
	1:����������ѹ��·�ĸ������(gama0=1.0)��
	2:������ʱʹ�õĸ������(gama0=0.9).*/
	m_siSafetyGrade=1;
	m_fSafetyCoef=1.0;
	m_fVibrationT1=0;
	m_fIceDensity=900;	//kg/m3
	m_iLoadExecuteCode=0;
	m_iMemberSizeCheckCode=0;
	m_iSlendernessLimCode=0;
	m_bWeightAmplifyCoefIncBraceRods=false;	//��ѹ������ϵ�����Ƿ�������ĸ˼� wjh-2019.7.13
	m_bAreaAmplifyCoefIncBraceRods =false;	//��ѹ������Ŵ�ϵ�����Ƿ�������ĸ˼� wjh-2019.7.13
	//���ѡ����Ϣ
	m_fLimitBearCoef=1.0;
	//SelMatPara.m_fWindBetaZForFoundation=1.3;
	m_fGamaGForDesign=1.2;
	m_fGamaGForFoundation=1.0;
	m_fGamaQForTowerWindLoad=1.4;
	SelMatPara.m_nConvergeLim=4;
	SelMatPara.m_nMaxSelMatCount=4;
	SelMatPara.m_nMainPoleSlendernessLimit=150;
	SelMatPara.m_nXiePoleSlendernessLimit=200;
	SelMatPara.m_nAuxPoleSlendernessLimit=250;
	SelMatPara.m_nTensivePoleSlendernessLimit=400;
	SelMatPara.m_nMainTubeSlendernessLimit=130;
	SelMatPara.m_nXieTubeSlendernessLimit=200;
	SelMatPara.m_nAuxTubeSlendernessLimit=250;
	SelMatPara.m_nTensiveTubeSlendernessLimit=400;
	SelMatPara.m_bPreferSingleAngle=TRUE;
	SelMatPara.m_iSizeOrderRule=0;
	SelMatPara.m_nMaxSelMatCount=4;
	SelMatPara.m_nConvergeLim=4;
	strcpy(SelMatPara.m_csMinAngleSize,"40X3");
	strcpy(SelMatPara.m_csMinForceAngleSize,"40X4");
	strcpy(SelMatPara.m_csMinMainAngleSize,"45X4");
	//���������Ϣ
	EarthQuake.grade=9;			//�����Ҷ�6,7,8,9
	EarthQuake.alfa_max=0.32;	//ˮƽ����Ӱ��ϵ�����ֵ
	EarthQuake.cLandType=2;		//�������1,2,3,4
	EarthQuake.cDesignGroup=1;	//��Ƶ������1,2,3
	EarthQuake.Tg=0.35;			//��������,s
	EarthQuake.zeta=0.02;		//����ϵ����=0.02s
	EarthQuake.gamaRE=0.8;		//�������������ϵ��,һ���Խ��ȡ0.85��������ȡ0.8
	EarthQuake.nFreqsRank=8;
	memset(EarthQuake.modes_arr,0,sizeof(VIBRAMODE_PROP)*16);
	hashAngleJoint.LoadDefaultObjectInfo=LoadAngleJointIdFunc;
#endif
}

CLDSModule* CTower::InitModule()
{
	//��ո�����
	NoManager.EmptyGroupNo();
	/*CLDSArcLift *pLift=ArcLift.append();
	pLift->iNo=1;
	strcpy(pLift->name,"���ϵ��ߺᵣ");
	pLift->m_cRotAxis='Y';
	pLift->m_fLiftHeight=170;
	pLift->m_xDatumLiftPos.Set(18750,650,2800);
	pLift->m_xRotOrg.Set(1947.258,1947.258,2800);*/
	//���Ĭ�Ϻ��ع���
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	AppendGeneralStatus();
#endif
	//����Ĭ�Ϻ���
	CLDSModule *pModule=Module.append();
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(pModule->description,"Module");
#else
	strcpy(pModule->description,"����");
#endif
	CObjNoGroup *pGroup=NoManager.FromGroupID(MODEL_GROUP);
	pModule->iNo=pGroup->EnumIdleNo();
	pGroup->SetNoState(pModule->iNo);
	m_hActiveModule=pModule->handle;
	for(int i=0;i<4;i++)
		m_arrActiveQuadLegNo[i]=1;

	//����Ĭ����ͼ
	CDisplayView *pDispView=View.append();
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(pDispView->name,"Perspective View");
#else
	strcpy(pDispView->name,"͸��ͼ");
#endif
	pDispView->m_iViewType=0;
	strcpy(pDispView->defaultLayer,"SPQ");
	strcpy(pDispView->filter.FirstCharSet,"?");
	strcpy(pDispView->filter.SecondCharSet,"?");
	strcpy(pDispView->filter.ThirdCharSet,"?");
	pDispView->ucs.axis_x.Set(1,0,0);
	pDispView->ucs.axis_y.Set(0,0,-1);
	pDispView->ucs.axis_z.Set(0,1,0);

	pDispView=View.append();
	//m_pActiveDisplayView=pDispView;
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(pDispView->name,"Front View");
#else
	strcpy(pDispView->name,"ǰ��ͼ");
#endif
	pDispView->m_iViewType=0;
	strcpy(pDispView->defaultLayer,"SPQ");
	strcpy(pDispView->filter.FirstCharSet,"TSL");
	strcpy(pDispView->filter.SecondCharSet,"ZXHFP");
	strcpy(pDispView->filter.ThirdCharSet,"Q12");
	pDispView->ucs.axis_x.Set(1,0,0);
	pDispView->ucs.axis_y.Set(0,0,-1);
	pDispView->ucs.axis_z.Set(0,1,0);
	
	pDispView=View.append();
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(pDispView->name,"Right View");
#else
	strcpy(pDispView->name,"����ͼ");
#endif
	pDispView->m_iViewType=0;
	strcpy(pDispView->defaultLayer,"SPY");
	strcpy(pDispView->filter.FirstCharSet,"TSL");
	strcpy(pDispView->filter.SecondCharSet,"ZXHFP");
	strcpy(pDispView->filter.ThirdCharSet,"Y13");
	pDispView->ucs.axis_x.Set(0,-1,0);
	pDispView->ucs.axis_y.Set(0,0,-1);
	pDispView->ucs.axis_z.Set(1,0,0);

	pDispView=View.append();
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(pDispView->name,"Back View");
#else
	strcpy(pDispView->name,"����ͼ");
#endif
	pDispView->m_iViewType=0;
	strcpy(pDispView->defaultLayer,"SPH");
	strcpy(pDispView->filter.FirstCharSet,"TSL");
	strcpy(pDispView->filter.SecondCharSet,"ZXHFP");
	strcpy(pDispView->filter.ThirdCharSet,"H34");
	pDispView->ucs.axis_x.Set(-1,0,0);
	pDispView->ucs.axis_y.Set(0,0,-1);
	pDispView->ucs.axis_z.Set(0,-1,0);

	pDispView=View.append();
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(pDispView->name,"Left View");
#else
	strcpy(pDispView->name,"����ͼ");
#endif
	pDispView->m_iViewType=0;
	strcpy(pDispView->defaultLayer,"SPZ");
	strcpy(pDispView->filter.FirstCharSet,"TSL");
	strcpy(pDispView->filter.SecondCharSet,"ZXHFP");
	strcpy(pDispView->filter.ThirdCharSet,"Z24");
	pDispView->ucs.axis_x.Set(0,1,0);
	pDispView->ucs.axis_y.Set(0,0,-1);
	pDispView->ucs.axis_z.Set(-1,0,0);
	//����ͼ
	pDispView=View.append();
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(pDispView->name,"Downward View");
#else
	strcpy(pDispView->name,"����ͼ");
#endif
	pDispView->m_iViewType=0;
	strcpy(pDispView->defaultLayer,"SPQ");
	strcpy(pDispView->filter.FirstCharSet,"?");
	strcpy(pDispView->filter.SecondCharSet,"?");
	strcpy(pDispView->filter.ThirdCharSet,"?");
	pDispView->ucs.axis_x.Set(1,0,0);
	pDispView->ucs.axis_y.Set(0,-1,0);
	pDispView->ucs.axis_z.Set(0,0,-1);
	return pModule;
}
CLDSLinePart *CTower::FromRodHandle(long hRod)
{
	CLDSPart* pPart=Parts.FromHandle(hRod);
	if(pPart&&pPart->IsLinePart())
		return (CLDSLinePart*)pPart;
	else
		return NULL;
}
/*#include "WireNodeDef.h"
#include "ElectricTower.h"
bool CTower::RetrieveWireModelFromNodes(CUniWireModel* pWireModel)
{
#if defined(__RAPID_ROUGH_3DMODEL_)&&defined(__PART_DESIGN_INC_)
	CLDSNode* pNode;
	pWireModel->hashWirePoints.Empty();
	for(pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		if(pNode->m_cHangWireType!='C'&&pNode->m_cHangWireType!='E'&&pNode->m_cHangWireType!='J')
			continue;
		CXhWirePoint* pWirePoint=pWireModel->hashWirePoints.Append();
		pWirePoint->xPosition=pNode->xOriginalPos;
		pWirePoint->wireplace.ciWireType=pNode->m_cHangWireType;
		pWirePoint->m_hRelaWireLoadNode=pNode->handle;
		//pWirePoint->szInsulatorStyleName
		{
			//CXhChar50 szWireName="����";
			//if(pNode->wireplace.ciWireType=='J')
			//	szWireName.Copy("����");
			//else if(pNode->wireplace.ciWireType=='E')
			//	szWireName.Copy("����");
			//if(pNode->m_sHangWireDesc.GetLength()<=0)
			//	pNode->UpdateWireDesc();
			//if(pNode->m_sHangWireDesc.GetLength()>0)
			//	szWireName=pNode->m_sHangWireDesc;
		}
	}
#endif
	return true;
}*/

CLDSFoundation* CTower::AddFoundation(BYTE ciHeightSerial,BYTE ciLegSerial,BYTE ciLegQuad)
{
	CLDSFoundation* pFoundation;
	for(pFoundation=this->Foundations.GetFirst();pFoundation;pFoundation=Foundations.GetNext())
	{
		if( pFoundation->ciHeightSerial==ciHeightSerial&&pFoundation->ciLegSerial==ciLegSerial&&
			pFoundation->ciLegQuad==ciLegQuad)
			return pFoundation;
	}
	pFoundation=Foundations.append();
	pFoundation->ciHeightSerial=ciHeightSerial;
	pFoundation->ciLegSerial=ciLegSerial;
	pFoundation->ciLegQuad=ciLegQuad;
	return pFoundation;
}
CLDSFoundation* CTower::GetFoundation(BYTE ciHeightSerial,BYTE ciLegSerial,BYTE ciLegQuad)
{
	for(CLDSFoundation* pFoundation=this->Foundations.GetFirst();pFoundation;pFoundation=Foundations.GetNext())
	{
		if( pFoundation->ciHeightSerial!=ciHeightSerial||pFoundation->ciLegSerial!=ciLegSerial||
			pFoundation->ciLegQuad!=ciLegQuad)
			continue;
		return pFoundation;
	}
	return NULL;
}
//Լ����صĺ���
CONSTRAINT* CTower::AppendConstraint(CONSTRAINT* pCopyFrom/*=NULL*/)
{
	CONSTRAINT* pConstraint=hashConstraints.Add(0);
	pConstraint->CopyProperty(pCopyFrom);
	return pConstraint;
}
CONSTRAINT* CTower::GetConstraint(long constraint_id)
{
	return hashConstraints.GetValue(constraint_id);
}
bool CTower::RemoveConstraint(long constraint_id,long hFromMasterObj/*=0*/)
{
	for(CONSTRAINT* pConstraint=hashConstraints.GetFirst();pConstraint;pConstraint=hashConstraints.GetNext())
	{
		if(pConstraint->Id!=constraint_id)	//������ɾ���ڵ�
			continue;
		if(pConstraint->hMasterObj!=hFromMasterObj&&hFromMasterObj>0x20)
			return false;
		else
			return hashConstraints.DeleteCursor()!=FALSE;
	}
	return false;
}
CONSTRAINT* CTower::EnumConstraintFirst(BOOL bIterDelete/*=FALSE*/)
{
	return hashConstraints.GetFirst(bIterDelete);
}
CONSTRAINT* CTower::EnumConstraintNext(BOOL bIterDelete/*=FALSE*/)
{
	return hashConstraints.GetNext(bIterDelete);
}
void CTower::ClearConstraintSyncStates()
{
	for(CONSTRAINT* pConstraint=hashConstraints.GetFirst();pConstraint;pConstraint=hashConstraints.GetNext())
		pConstraint->ClearSynchronize();
}
long CTower::GetObjNum(int CLASS_TYPEID)
{
	int hits=0;
	CLDSLinePart* pRod;
	switch(CLASS_TYPEID)
	{
	case CLS_NODE:
		return Node.GetNodeNum();
	case CLS_GEPOINT:
		return Point.GetNodeNum();
	case CLS_GELINE:
		return Line.GetNodeNum();
	case CLS_GEPLANE:
		return Plane.GetNodeNum();
	case CLS_PART:
		return Parts.GetNodeNum();
	case CLS_LINEPART:
		for(pRod=EnumRodFirst(),hits=0;pRod;pRod=EnumRodNext())
			hits++;
		return hits;
	case CLS_DISPLAYVIEW:
		return View.GetNodeNum();
	default:
		return 0;
	}
}
long CTower::GetNewHandle()
{
	if(GetNewHandleHookFunc!=NULL)
		return GetNewHandleHookFunc();
	long h;
	if(m_bCooperativeWork&&GetCooperativeNewHandle)
		h=GetCooperativeNewHandle();
	else
	{
		h = handle;
		handle++;
	}
	return h;
}
void IModel::AddMirRelativeObjInfo(PROGRESS_CONTEXT* pDisplayProcessContext/*=NULL*/)
{
	RELATIVE_OBJECT relaObj;
	int i=0,n=GetObjNum(CLS_NODE);
	CTower* pTower=IsTowerModel()?(CTower*)this:NULL;
	for(CLDSNode *pNode=EnumNodeFirst();pNode;pNode=EnumNodeNext())
	{
		BOOL pushed=PushNodeStack();
		CLDSNode *pXMirNode=pNode->GetMirNode(MIRMSG(1));
		CLDSNode *pYMirNode=pNode->GetMirNode(MIRMSG(2));
		CLDSNode *pZMirNode=pNode->GetMirNode(MIRMSG(3));
		if(pXMirNode==pNode||(pXMirNode&&pXMirNode->m_cPosCalType!=pNode->m_cPosCalType))
			pXMirNode=NULL;
		if(pYMirNode==pNode||(pXMirNode&&pXMirNode->m_cPosCalType!=pNode->m_cPosCalType))
			pYMirNode=NULL;
		if(pZMirNode==pNode||(pXMirNode&&pXMirNode->m_cPosCalType!=pNode->m_cPosCalType))
			pZMirNode=NULL;
		if(pXMirNode&&pXMirNode!=pNode)
		{
			relaObj.hObj=pXMirNode->handle;
			relaObj.mirInfo.axis_flag=0x01;
			pNode->AppendRelativeObj(relaObj);
			relaObj.hObj=pNode->handle;
			pXMirNode->AppendRelativeObj(relaObj);
			if(pYMirNode&&pYMirNode!=pXMirNode)
			{
				relaObj.hObj=pYMirNode->handle;
				relaObj.mirInfo.axis_flag=0x04;
				pXMirNode->AppendRelativeObj(relaObj);
				relaObj.hObj=pXMirNode->handle;
				pYMirNode->AppendRelativeObj(relaObj);
			}
			if(pZMirNode&&pZMirNode!=pXMirNode)
			{
				relaObj.hObj=pZMirNode->handle;
				relaObj.mirInfo.axis_flag=0x02;
				pXMirNode->AppendRelativeObj(relaObj);
				relaObj.hObj=pXMirNode->handle;
				pZMirNode->AppendRelativeObj(relaObj);
			}
		}
		if(pYMirNode&&pYMirNode!=pNode)
		{
			relaObj.hObj=pYMirNode->handle;
			relaObj.mirInfo.axis_flag=0x02;
			pNode->AppendRelativeObj(relaObj);
			relaObj.hObj=pNode->handle;
			pYMirNode->AppendRelativeObj(relaObj);
			if(pZMirNode&&pYMirNode!=pZMirNode)
			{
				relaObj.hObj=pZMirNode->handle;
				relaObj.mirInfo.axis_flag=0x01;
				pYMirNode->AppendRelativeObj(relaObj);
				relaObj.hObj=pYMirNode->handle;
				pZMirNode->AppendRelativeObj(relaObj);
			}
		}
		if(pZMirNode&&pZMirNode!=pNode)
		{
			relaObj.hObj=pZMirNode->handle;
			relaObj.mirInfo.axis_flag=0x04;
			pNode->AppendRelativeObj(relaObj);
			relaObj.hObj=pNode->handle;
			pZMirNode->AppendRelativeObj(relaObj);
		}
		PopNodeStack(pushed);
		if(pTower&&pTower->DisplayProcess&&pDisplayProcessContext)
		{
			i++;
			pTower->DisplayProcess(pDisplayProcessContext->Mapping(i*50/n),pDisplayProcessContext->title);
		}
	}
	i=0;
	n=this->GetObjNum(CLS_LINEPART);
	for(CLDSLinePart *pRod=EnumRodFirst();pRod;pRod=EnumRodNext())
	{
		if(pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		BOOL pushed=PushPartStack();
		CLDSLinePart *pXMirRod=(CLDSLinePart*)pRod->GetMirRod(MIRMSG(1));
		CLDSLinePart *pYMirRod=(CLDSLinePart*)pRod->GetMirRod(MIRMSG(2));
		CLDSLinePart *pZMirRod=(CLDSLinePart*)pRod->GetMirRod(MIRMSG(3));
		if(pXMirRod==pRod)
			pXMirRod=NULL;
		if(pYMirRod==pRod)
			pYMirRod=NULL;
		if(pZMirRod==pRod)
			pZMirRod=NULL;
		if(pXMirRod&&pXMirRod!=pRod)
		{
			relaObj.hObj=pXMirRod->handle;
			relaObj.mirInfo.axis_flag=0x01;
			pRod->AppendRelativeObj(relaObj);
			relaObj.hObj=pRod->handle;
			pXMirRod->AppendRelativeObj(relaObj);
			if(pYMirRod&&pXMirRod!=pYMirRod)
			{
				relaObj.hObj=pYMirRod->handle;
				relaObj.mirInfo.axis_flag=0x04;
				pXMirRod->AppendRelativeObj(relaObj);
				relaObj.hObj=pXMirRod->handle;
				pYMirRod->AppendRelativeObj(relaObj);
			}
			if(pZMirRod&&pXMirRod!=pZMirRod)
			{
				relaObj.hObj=pZMirRod->handle;
				relaObj.mirInfo.axis_flag=0x02;
				pXMirRod->AppendRelativeObj(relaObj);
				relaObj.hObj=pXMirRod->handle;
				pZMirRod->AppendRelativeObj(relaObj);
			}
		}
		if(pYMirRod&&pYMirRod!=pRod)
		{
			relaObj.hObj=pYMirRod->handle;
			relaObj.mirInfo.axis_flag=0x02;
			pRod->AppendRelativeObj(relaObj);
			relaObj.hObj=pRod->handle;
			pYMirRod->AppendRelativeObj(relaObj);
			if(pZMirRod&&pYMirRod!=pZMirRod)
			{
				relaObj.hObj=pZMirRod->handle;
				relaObj.mirInfo.axis_flag=0x01;
				pYMirRod->AppendRelativeObj(relaObj);
				relaObj.hObj=pYMirRod->handle;
				pZMirRod->AppendRelativeObj(relaObj);
			}
		}
		if(pZMirRod&&pZMirRod!=pRod)
		{
			relaObj.hObj=pZMirRod->handle;
			relaObj.mirInfo.axis_flag=0x04;
			pRod->AppendRelativeObj(relaObj);
			relaObj.hObj=pRod->handle;
			pZMirRod->AppendRelativeObj(relaObj);
		}
		PopPartStack(pushed);
		if(pTower&&pTower->DisplayProcess&&pDisplayProcessContext)
		{
			i++;
			pTower->DisplayProcess(pDisplayProcessContext->Mapping(50+i*50/n),pDisplayProcessContext->title);
		}
	}
	if(pTower&&pTower->DisplayProcess&&pDisplayProcessContext)
		pTower->DisplayProcess(pDisplayProcessContext->Mapping(100),pDisplayProcessContext->title);
}

NODERELAINFO::~NODERELAINFO()
{
	relationNodes.Clear();
	relationRods.Clear();
}
bool NODERELAINFO::Visit(void (*taskfunc)(NODERELAINFO*pSubRelaNode,void* pContext),
						 void* pContext/*=NULL*/,UINT MAX_VISITS/*=2*/,UINT MAX_SUBVISITS/*=1*/)
{
	CDepthCounter<UINT> visit(&uiVisits);
	if(uiVisits>=MAX_VISITS)
		return false;	//�����ٽ���ʴ���
	uiVisits++;
	if(taskfunc!=NULL)
	{
		taskfunc(this,pContext);
		for(WORD i=0;i<relationNodes.Count;i++)
		{
			NODERELAINFO* pSubRelaNode=relationNodes.At(i);
			if(pSubRelaNode->uiVisits>=MAX_SUBVISITS)
				continue;
			taskfunc(pSubRelaNode,pContext);
		}
	}
	return true;
}
NODERELAINFO* IModel::GetNodeRelaInfo(long hNode)
{
	CHashListEx<NODERELAINFO>* pHashNodeRelations= (CHashListEx<NODERELAINFO>*)Get_hashNodeRelations();
	if(pHashNodeRelations==NULL)
		return NULL;
	return pHashNodeRelations->GetValue(hNode);
}
void IModel::InitializeNodeRelations()
{
	CHashListEx<NODERELAINFO>* pHashNodeRelations= (CHashListEx<NODERELAINFO>*)Get_hashNodeRelations();
	if(pHashNodeRelations==NULL)
		return;
	pHashNodeRelations->Empty();
	NODERELAINFO* pNodeRela;
	BOOL pushed=PushNodeStack();
	for(CLDSNode* pNode=EnumNodeFirst();pNode;pNode=EnumNodeNext())
	{
		pNodeRela=pHashNodeRelations->Add(pNode->handle);
		pNodeRela->pNode=pNode;
	}
	if(pushed)
		PopNodeStack(pushed);
	pushed=PushPartStack();
	for(CLDSLinePart* pRod=this->EnumRodFirst();pRod;pRod=EnumRodNext())
	{
		if(pRod->pStart)
		{
			if((pNodeRela=this->GetNodeRelaInfo(pRod->pStart->handle))!=NULL)
				pNodeRela->relationRods.Append(pRod);
			else
				logerr.Log("error");
		}
		if(pRod->pEnd)
		{
			if((pNodeRela=this->GetNodeRelaInfo(pRod->pEnd->handle))!=NULL)
				pNodeRela->relationRods.Append(pRod);
			else
				logerr.Log("error");
		}
	}
	if(pushed)
		PopPartStack(pushed);
	for(pNodeRela=pHashNodeRelations->GetFirst();pNodeRela;pNodeRela=pHashNodeRelations->GetNext())
	{
		if(pNodeRela->pNode->m_cPosCalType==0||pNodeRela->pNode->m_cPosCalType>CLDSNode::COORD_OFFSET)
			continue;
		NODERELAINFO* pRelaRef;
		for(int i=0;i<4;i++)
		{
			if(i==2&&pNodeRela->pNode->m_cPosCalType!=CLDSNode::COORD_OFFSET&&pNodeRela->pNode->m_cPosCalType!=CLDSNode::COORD_INTERSECTION)
				continue;
			else if(i==3&&pNodeRela->pNode->m_cPosCalType!=CLDSNode::COORD_INTERSECTION)
				continue;
			if((pRelaRef=pHashNodeRelations->GetValue(pNodeRela->pNode->arrRelationNode[i]))!=NULL)
				pRelaRef->relationNodes.Append(pNodeRela);
		}
		for(RELATIVE_OBJECT* pRelaObj=pNodeRela->pNode->relativeObjs.GetFirst();pRelaObj;pRelaObj=pNodeRela->pNode->relativeObjs.GetNext())
		{
			if((pRelaRef=pHashNodeRelations->GetValue(pRelaObj->hObj))!=NULL)
				pRelaRef->relationNodes.Append(pNodeRela);
		}
	}
}
long IModel::GetMinAvailableHandle()//��ʼ����С���ܵ�ͼԪ���;
{
	long handle = 0x20;	//������ε���ʱ,����һ����
	for(CLDSPoint *pPoint=EnumPointFirst();pPoint;pPoint=EnumPointNext())
	{
		handle = max(handle,pPoint->handle);
		handle = max(handle,pPoint->m_hBlock);
	}
	for(CLDSLine *pLine=EnumLineFirst();pLine;pLine=EnumLineNext())
	{	
		handle = max(handle,pLine->handle);
		handle = max(handle,pLine->m_hBlock);
	}
	for(CLDSPlane *pPlane=EnumPlaneFirst();pPlane;pPlane=EnumPlaneNext())
	{
		handle = max(handle,pPlane->handle);
		handle = max(handle,pPlane->m_hBlock);
	}
	for(CDisplayView* pView=EnumViewFirst();pView;pView=EnumViewNext())
	{
		handle = max(handle,pView->handle);
		handle = max(handle,pView->m_hBlock);
	}
	for(CLDSNode *pNode=EnumNodeFirst();pNode;pNode=EnumNodeNext())
	{	
		handle = max(handle,pNode->handle);
		handle = max(handle,pNode->m_hBlock);
	}
	for(CLDSPart *pPart=EnumPartFirst();pPart;pPart=EnumPartNext())
	{	
		handle = max(handle,pPart->handle);
		handle = max(handle,pPart->m_hBlock);
	}
	return ++handle;
}
long CTower::GetMinAvailableHandle()//��ʼ�����ľ��;
{
	handle = IModel::GetMinAvailableHandle()-1;	//������ε���ʱ,����һ����
	for(CLDSModule *pModule=Module.GetFirst();pModule;pModule=Module.GetNext())
	{	
		handle = max(handle,pModule->handle);
		handle = max(handle,pModule->m_hBlock);
	}
	for(CLDSArcLift *pLift=ArcLift.GetFirst();pLift;pLift=ArcLift.GetNext())
		handle = max(handle,pLift->handle);
	for(CBlockModel *pBlock=Block.GetFirst();pBlock;pBlock=Block.GetNext())
		handle = max(handle,pBlock->handle);
	for(CBlockReference *pBlockRef=BlockRef.GetFirst();pBlockRef;pBlockRef=BlockRef.GetNext())
	{	
		handle = max(handle,pBlockRef->handle);
		handle = max(handle,pBlockRef->m_hBlock);
	}
	for(CLDSFoundation *pFoundation=Foundations.GetFirst();pFoundation;pFoundation=Foundations.GetNext())
		handle=max(handle,pFoundation->handle);
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	for(CWorkStatus *pStatus=WorkStatus.GetFirst();pStatus;pStatus=WorkStatus.GetNext())
		handle = max(handle,pStatus->handle);
	for(CAttachWindLoad *pWindLoad=AttachWindLoad.GetFirst();pWindLoad;pWindLoad=AttachWindLoad.GetNext())
		handle = max(handle,pWindLoad->handle);
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	for(CSupplementPart *pSuppPart=SuppPart.GetFirst();pSuppPart;pSuppPart=SuppPart.GetNext())
		handle = max(handle,pSuppPart->handle);
#endif
	return ++handle;
}
static const double COLINEAR_MINCOSA = 0.99;	//max sina=0.1 = 50/500
static bool IsColinear(const double* pxDatumStart,const double* pxLenStdVec,const double* pxVerifyPoint,
		double tolerance=0.1,double tolerance_tangent=0.001)
{
	GEPOINT xVerifyLenVec=GEPOINT(pxVerifyPoint)-GEPOINT(pxDatumStart);
	GEPOINT lenStdVec(pxLenStdVec);
	double prjlen=xVerifyLenVec*lenStdVec;
	GEPOINT prjvec =prjlen*lenStdVec;
	GEPOINT vertvec=xVerifyLenVec-prjvec;
	double tolerance_square=tolerance*tolerance;
	double tangent_square  =tolerance_tangent*tolerance_tangent;
	double height_square=vertvec.mod2();
	if(height_square>tolerance_square)
		return false;
	if(vertvec.mod2()/(prjlen*prjlen)>tangent_square)
		return false;
	else
		return true;
}
//TODO:���׶�ʧ�м�˼�
struct LINEARROD
{
	CLDSLinePart *pRod;	//�����˼�
	double distance2start;	//������Ԫ����׼�����
	double distance2end;	//������Ԫ����׼�����
	LINEARROD(CLDSLinePart* pLinearRod=0,double distanceToStart=0,double distanceToEnd=0)
	{
		pRod=pLinearRod;
		distance2start=distanceToStart;
		distance2end  =distanceToEnd;
	}
};
static int _LocalCompareLinearRodDistanceFunc(const LINEARROD& item1,const LINEARROD& item2)
{
	if(item1.distance2start+item1.distance2end>item2.distance2start+item2.distance2end)
		return 1;
	else if(item1.distance2start+item1.distance2end<item2.distance2start+item2.distance2end)
		return -1;
	else
		return 0;
}
struct NODEPOSLEN{
	long hNode;
	double scaleOfLenPos;
	NODEPOSLEN(long _hNode=0,double scale=0)
	{
		hNode=_hNode;
		scaleOfLenPos=scale;
	}
};
static NODEPOSLEN* SearchNode(IXhArray<NODEPOSLEN>* pArrayNodes,long hNode)
{
	for(UINT i=0;i<pArrayNodes->Size();i++)
	{
		NODEPOSLEN* pNodePos=&pArrayNodes->At(i);
		if(pNodePos->hNode==hNode)
			return pNodePos;
	}
	return NULL;
}
bool IModel::SearchSortedColinearRods(CLDSLinePart* pOriginalRod,CTmaPtrList<CLDSLinePart,CLDSLinePart*>& rodset,bool* pbStartTipIsStartNode,bool* pbEndTipIsStartNode,
				double tolerance/*=0.1*/,double tolerance_tangent/*=0.001*/,//double minColinearCosA)
				bool (*pfnVerifyIterRod)(CLDSLinePart* pRod)/*=NULL*/)
{
	if(pOriginalRod->pStart==NULL||pOriginalRod->pEnd==NULL)
		return false;
	CLdsPartListStack stack(this);
	CPtInLineCheck lineCheck(pOriginalRod->pStart->xOriginalPos,pOriginalRod->pEnd->xOriginalPos);
	//����ʼ�˲๲�߶�ͷ�˼�
	CLDSNode* pStart=pOriginalRod->pStart;
	CLDSNode* pEnd=pOriginalRod->pEnd;
	if(pbStartTipIsStartNode)
		*pbStartTipIsStartNode=true;
	if(pbEndTipIsStartNode)
		*pbEndTipIsStartNode=false;
	CLDSLinePart *pRod,*pStartTipRod=pOriginalRod,*pEndTipRod=pOriginalRod;
	double minposlen=0,maxposlen=1;
	LINEPARTSET startset,endset;
	bool searched=true;
	PRESET_ARRAY32<NODEPOSLEN> arrayNodes;
	ARRAY_LIST<LINEARROD> arrStartRods(0,8),arrEndRods(0,8);
	GEPOINT xTipStart=pOriginalRod->pStart->xOriginalPos,xTipEnd=pOriginalRod->pEnd->xOriginalPos;
	for(pRod=EnumRodFirst();pRod;pRod=EnumRodNext())
	{
		if(pfnVerifyIterRod&&!pfnVerifyIterRod(pRod))
			continue;
		if(pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		if(pOriginalRod==pRod || !pOriginalRod->ModuleTogetherWith(pRod))
			continue;
		if(pRod->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pRod)->group_father_jg_h>0x20)
			continue;	//������ϽǸ��ӽǸ�
		NODEPOSLEN* pSearchStart=SearchNode(&arrayNodes,pRod->pStart->handle);
		NODEPOSLEN* pSearchEnd  =SearchNode(&arrayNodes,pRod->pEnd->handle);
		double posstartlen,posendlen;
		if(pSearchStart)
		{	//������Ϊ����У���˼����췽�򣬵���ͬһ�ڵ�����Ϊ��ͬ�˼��˽ڵ�ʱ�ı���λ�ó���ϸ΢ƫ�� wjh-2018.7.8
			searched=true;
			posstartlen=pSearchStart->scaleOfLenPos;
		}
		else
			posstartlen=lineCheck.CheckPointEx(pRod->pStart->xOriginalPos,&searched,tolerance,tolerance_tangent);
		if(!searched)
			continue;
		if(pSearchEnd)
		{
			searched=true;
			posendlen=pSearchEnd->scaleOfLenPos;
		}
		else
			posendlen=lineCheck.CheckPointEx(pRod->pEnd->xOriginalPos,&searched,tolerance,tolerance_tangent);
		if(!searched)
			continue;
		searched=false;
		pStartTipRod=pRod;
		if(posstartlen+posendlen<0)
		{
			if(posstartlen<posendlen)
			{
				xTipStart=pRod->pStart->xOriginalPos;
				arrayNodes.Append(NODEPOSLEN(pRod->pStart->handle,posstartlen));
			}
			else
			{
				xTipStart=pRod->pEnd->xOriginalPos;
				arrayNodes.Append(NODEPOSLEN(pRod->pEnd->handle,posendlen));
			}
			GEPOINT vLenStdVec=xTipEnd-xTipStart;
			normalize(vLenStdVec);
			double distance=0;
			GEPOINT vOldLenStdVec=lineCheck.LineStdVec();
			lineCheck.ReviseLenStdVec(vLenStdVec,&distance);
			if(distance>tolerance)
				lineCheck.ReviseLenStdVec(vOldLenStdVec);
			else
				arrStartRods.append(LINEARROD(pRod,posstartlen,posendlen));
		}
		else if((posstartlen+posendlen)>2)
		{
			if(posstartlen>posendlen)
			{
				xTipEnd=pRod->pStart->xOriginalPos;
				arrayNodes.Append(NODEPOSLEN(pRod->pStart->handle,posstartlen));
			}
			else
			{
				xTipEnd=pRod->pEnd->xOriginalPos;
				arrayNodes.Append(NODEPOSLEN(pRod->pEnd->handle,posendlen));
			}
			GEPOINT vLenStdVec=xTipEnd-xTipStart;
			normalize(vLenStdVec);
			double distance=0;
			GEPOINT vOldLenStdVec=lineCheck.LineStdVec();
			lineCheck.ReviseLenStdVec(vLenStdVec,&distance);
			if(distance>tolerance)
				lineCheck.ReviseLenStdVec(vOldLenStdVec);
			else
				arrEndRods.append(LINEARROD(pRod,posstartlen,posendlen));
		}
		else
			continue;
	}
	//��ʼ->�շ�������
	CQuickSort<LINEARROD>::QuickSort(arrStartRods.m_pData,arrStartRods.GetSize(),_LocalCompareLinearRodDistanceFunc);
	CQuickSort<LINEARROD>::QuickSort(arrEndRods.m_pData,arrEndRods.GetSize(),_LocalCompareLinearRodDistanceFunc);
	int i;
	double CURR_EPS=1e-5;	//1/10000=1e-5;	//����֮ǰ���������˼��߷��򣬹ʴ�ͬһ��ǰ��Ͽ�����ƫ��ʷŴ�˼�϶
	for(i=arrStartRods.GetSize()-1;i>=0;i--)
	{
		LINEARROD* pCurrRod=&arrStartRods[i];
		bool startOnLeft=pCurrRod->distance2start<pCurrRod->distance2end;
		double righttipposlen=startOnLeft?pCurrRod->distance2end:pCurrRod->distance2start;
		if(fabs(righttipposlen-minposlen)>CURR_EPS)	//�������߶γ����ж�
			break;
		pStart=startOnLeft?pCurrRod->pRod->pStart:pCurrRod->pRod->pEnd;
		minposlen=startOnLeft?pCurrRod->distance2start:pCurrRod->distance2end;
		if(pbStartTipIsStartNode)
			*pbStartTipIsStartNode=startOnLeft;
		startset.append(pCurrRod->pRod);
	}
	for(i=0;i<arrEndRods.GetSize();i++)
	{
		LINEARROD* pCurrRod=&arrEndRods[i];
		bool startOnLeft=pCurrRod->distance2start<pCurrRod->distance2end;
		double lefttipposlen=startOnLeft?pCurrRod->distance2start:pCurrRod->distance2end;
		if(fabs(lefttipposlen-maxposlen)>CURR_EPS)	//�������߶γ����ж�
			break;
		pEnd=startOnLeft?pCurrRod->pRod->pEnd:pCurrRod->pRod->pStart;
		maxposlen=startOnLeft?pCurrRod->distance2end:pCurrRod->distance2start;
		if(pbEndTipIsStartNode)
			*pbEndTipIsStartNode=!startOnLeft;
		endset.append(pCurrRod->pRod);
	}
	for(pRod=startset.GetTail();pRod;pRod=startset.GetPrev())
		rodset.append(pRod);
	rodset.append(pOriginalRod);
	for(pRod=endset.GetFirst();pRod;pRod=endset.GetNext())
		rodset.append(pRod);
	return rodset.GetNodeNum()>0;
}
double CTower::GetDeformCoef()
{
	if(m_ciDeformType==1)
		return m_fDeformCoef;
	else if(m_ciDeformType==2)	//͹��
		return 1;
	else //if(m_ciDeformType==3)	//����
		return 0;
}
void CTower::Empty()
{
	//clipper_board.ClearBuffer();			//��ռ��а�
	AttachBuffer[BOM_CONFIG].ClearContents();
	AttachBuffer[SMART_DRAW].ClearContents();
	AttachBuffer[PRJPARAMPART_LIBRAY].ClearContents();
	AttachBuffer[PPE_MODEL].ClearContents();
	AttachBuffer[BOM_COMPARE_MODEL].ClearContents();
	AttachBuffer[MODEL_SIZE_CHECK].ClearContents();
#ifdef __PART_DESIGN_INC_
	AttachBuffer[TOWER_MODEL].ClearContents();
	AttachBuffer[LOGGER].ClearContents();
	AttachBuffer[WIRELOAD_MODEL].ClearContents();
	AttachBuffer[RSLT_REPORT].ClearContents();
	CAttachBuffer* pAttachBuff;
	if((pAttachBuff=AttachBuffer.GetValue(TSA_TURBO_MODEL))!=NULL)
		pAttachBuff->ClearContents();
	if((pAttachBuff=AttachBuffer.GetValue(UNI_WIREPOINT_MODEL))!=NULL)
		pAttachBuff->ClearContents();
#endif
	Point.Empty();
	Line.Empty();
	Plane.Empty();
	ArcLift.Empty();
	Node.Empty();
	Parts.Empty();
	Module.Empty();
	View.Empty();
	Block.Empty();
	BlockRef.Empty();
	Foundations.Empty();
	Acs.Empty();
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	SuppPart.Empty();
	PartGroup.Empty();
	localStdPartLibray.Empty();
#endif
	hashConstraints.Empty();
	CONSTRAINT::hashExpressions.Empty();
	hashObjs.Empty();
	UndoBufferStack.Empty();
	UndoBuffer.ClearBuffer();
	this->m_bUserSpecifyNamedHeightZeroZ=false;	//false:��ʾϵͳ�Զ��������Z0��׼ƽ��;true:�����û�ָ��ֵ����
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
	//���ؼ�����Ϣ
	Elems.Empty();
	WorkStatus.Empty();
	AttachWindLoad.Empty();
	WindLoadSeg.Empty();
	handle = 0x20;
	NoManager.EmptyGroupNo();
#endif
}

CLDSObject* CTower::FromHandle(THANDLE part_handle,BOOL bDbObjOnly/*=FALSE*/)
{
	CLDSObject* pObj;
	if(!bDbObjOnly)
	{
		pObj = Point.FromHandle(part_handle);
		if(pObj!=NULL)
			return pObj;
		pObj = Line.FromHandle(part_handle);
		if(pObj!=NULL)
			return pObj;
		pObj = Plane.FromHandle(part_handle);
		if(pObj!=NULL)
			return pObj;
	}
	/*
	//������CLDSDbObject
	pObj = (CLDSDbObject*)Module.FromHandle(part_handle);
	if(pObj!=NULL)
		return pObj;
	pObj = (CLDSDbObject*)View.FromHandle(part_handle);
	if(pObj!=NULL)
		return pObj;
	*/
	pObj = Node.FromHandle(part_handle);
	if(pObj!=NULL)
		return pObj;
	pObj = Parts.FromHandle(part_handle);
	if(pObj!=NULL)
		return pObj;
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
	pObj = WorkStatus.FromHandle(part_handle);
	if(pObj!=NULL)
		return pObj;
#endif 
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	pObj = SuppPart.FromHandle(part_handle);
	if(pObj!=NULL)
		return pObj;
#endif
#ifdef __COMPLEX_PART_INC_
	pObj = BlockRef.FromHandle(part_handle);
	if(pObj!=NULL)
		return pObj;
#endif
	pObj = Foundations.FromHandle(part_handle);
	if(pObj!=NULL)
		return pObj;
	return NULL;
}

void CTower::GetTaSizeScope(SCOPE_STRU &scope)
{
	CLDSLinePart* pRod;
	CLdsPartListStack stack(this);
	for(pRod=EnumRodFirst();pRod!=NULL;pRod=EnumRodNext())
	{
		if(pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		scope.fMaxX = __max(scope.fMaxX, pRod->pStart->Position(false).x);
		scope.fMaxY = __max(scope.fMaxY, pRod->pStart->Position(false).y);
		scope.fMaxZ = __max(scope.fMaxZ, pRod->pStart->Position(false).z);
		scope.fMinX = __min(scope.fMinX, pRod->pStart->Position(false).x);
		scope.fMinY = __min(scope.fMinY, pRod->pStart->Position(false).y);
		scope.fMinZ = __min(scope.fMinZ, pRod->pStart->Position(false).z);

		scope.fMaxX = __max(scope.fMaxX, pRod->pEnd->Position(false).x);
		scope.fMaxY = __max(scope.fMaxY, pRod->pEnd->Position(false).y);
		scope.fMaxZ = __max(scope.fMaxZ, pRod->pEnd->Position(false).z);
		scope.fMinX = __min(scope.fMinX, pRod->pEnd->Position(false).x);
		scope.fMinY = __min(scope.fMinY, pRod->pEnd->Position(false).y);
		scope.fMinZ = __min(scope.fMinZ, pRod->pEnd->Position(false).z);
	}
}
static int compare_node_z(const CLDSNodePtr& node1,const CLDSNodePtr& node2)
{
	if(node1->Position(false).z>node2->Position(false).z)
		return 1;
	else if(node1->Position(false).z<node2->Position(false).z)
		return -1;
	else
		return 0;
}
long CTower::GetSlopeList(ARRAY_LIST<GEPOINT2D>& joint_z_arr,ATOM_LIST<f3dLine>* slopeset)
{
	CLDSNode* pNode;
	ARRAY_LIST<CLDSNodePtr> arrQuad1Node;
	arrQuad1Node.SetSize(0,4);
	for(pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		char cQuad=pNode->layer(2);
		if(isdigit(cQuad)&&(cQuad==0||cQuad>'4'))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("The tower isn't four rod tower,can't calculate getting slope!");
#else
			logerr.Log("����������������֧���¶���ȡ����!�����Ƿ����㹻��1���ޣ�ͼ�����������ַ�Ϊ1���������κ������ڵ㣡");
#endif
			return 0;
		}
		if(cQuad=='1'&&pNode->m_cPosCalType==0&&(pNode->layer(0)=='S'||pNode->Layer(0)=='L'))
			arrQuad1Node.append(pNode);
	}
	if(arrQuad1Node.GetSize()<2)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("The tower should include one slope data at least!");
#else
		logerr.Log("��������Ӧ����һ���¶�����!");
#endif
		return 0;
	}
	//��һ���ޱ��µ㰴Z�����������½�������
	CBubbleSort<CLDSNodePtr>::BubSort(arrQuad1Node.m_pData,arrQuad1Node.GetSize(),compare_node_z);
	f3dLine slopeline;
	f3dPoint prev_slope_vec;
	slopeline.startPt=arrQuad1Node[0]->Position(false);
	int i;
	ARRAY_LIST<f3dLine> slope_arr;
	slope_arr.SetSize(0,4);
	for(i=1;i<arrQuad1Node.GetSize();i++)
	{
		slopeline.endPt=arrQuad1Node[i]->Position(false);
		f3dPoint slope_vec=slopeline.startPt-slopeline.endPt;
		normalize(slope_vec);
		if(fabs(slope_vec.z)<EPS)
		{
		#ifdef __PART_DESIGN_INC_
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("The main rod's end node %d and %d of tower's first quadrant at the same horizontal plane,it's not a reasonable Eiffel effect structure!",arrQuad1Node[i-1]->point_i,arrQuad1Node[i]->point_i);
#else
			logerr.Log("������1�������Ķ˽ڵ�%d��%dλ��ͬһˮƽ�棬���Ǻ���İ��ƶ�ЧӦ�ṹ!",arrQuad1Node[i-1]->point_i,arrQuad1Node[i]->point_i);
#endif
		#endif
			return 0;
		}
		else if(prev_slope_vec*slope_vec>EPS_COS)
		{	//����һ�¶���ͬ����������һ�¶εĵײ��ڵ�
			slope_arr[slope_arr.GetSize()-1].endPt=slopeline.endPt;
		}
		else//�¶Ȳ�ͬ, Ϊ������¶�
		{
			prev_slope_vec=slope_vec;
			slope_arr.append(slopeline);
			slopeline.startPt=slopeline.endPt;
		}
	}
	if(slope_arr.GetSize()<=0)
		return 0;		//ȱ���¶����ݣ��д���
	joint_z_arr.SetSize(slope_arr.GetSize());
	if(slopeset)
		slopeset->Empty();
	for(i=0;i<slope_arr.GetSize();i++)
	{
		f3dLine* pLine=&slope_arr[i];
		if(slopeset)
			slopeset->append(*pLine);
		GEPOINT line_vec=pLine->startPt-pLine->endPt;
		if(line_vec.x>=-EPS)	//���¶�ΪX������ֱ�λ��ϴ���С,����ӦС��0
			joint_z_arr[i].x=0x80000000;	//��Ϊ����Ϊ���޸�
		else
			joint_z_arr[i].x=pLine->startPt.z-line_vec.z*pLine->startPt.x/line_vec.x;
		if(line_vec.y>=-EPS)	//���¶�ΪY������ֱ�λ��ϴ���С,����ӦС��0
			joint_z_arr[i].y=0x80000000;	//��Ϊ����Ϊ���޸�
		else
			joint_z_arr[i].y=pLine->startPt.z-line_vec.z*pLine->startPt.y/line_vec.y;
	}
	return slope_arr.GetSize();
}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
void Cal4MirJgWingNorm(CLDSLineAngle* pJg[4])
{	// ����Ǹָ�������ķ��߷���
	if(pJg[0])
	{
		pJg[0]->SetStart(pJg[0]->pStart->Position());
		pJg[0]->SetEnd(pJg[0]->pEnd->Position());
		pJg[0]->des_norm_x.near_norm.Set( 0, 1,0);
		pJg[0]->des_norm_y.near_norm.Set( 1, 0,0);
	}
	if(pJg[1])
	{
		pJg[1]->SetStart(pJg[1]->pStart->Position());
		pJg[1]->SetEnd(pJg[1]->pEnd->Position());
		pJg[1]->des_norm_x.near_norm.Set(-1, 0,0);
		pJg[1]->des_norm_y.near_norm.Set( 0, 1,0);
	}
	if(pJg[2])
	{
		pJg[2]->SetStart(pJg[2]->pStart->Position());
		pJg[2]->SetEnd(pJg[2]->pEnd->Position());
		pJg[2]->des_norm_x.near_norm.Set( 1, 0,0);
		pJg[2]->des_norm_y.near_norm.Set( 0,-1,0);
	}
	if(pJg[3])
	{
		pJg[3]->SetStart(pJg[3]->pStart->Position());
		pJg[3]->SetEnd(pJg[3]->pEnd->Position());
		pJg[3]->des_norm_x.near_norm.Set( 0,-1,0);
		pJg[3]->des_norm_y.near_norm.Set(-1, 0,0);
	}
	if(pJg[0]&&pJg[1])
		pJg[0]->des_norm_x.hViceJg = pJg[1]->handle;
	if(pJg[0]&&pJg[2])
		pJg[0]->des_norm_y.hViceJg = pJg[2]->handle;
	if(pJg[1]&&pJg[3])
		pJg[1]->des_norm_x.hViceJg = pJg[3]->handle;
	if(pJg[0]&&pJg[1])
		pJg[1]->des_norm_y.hViceJg = pJg[0]->handle;
	if(pJg[0]&&pJg[2])
		pJg[2]->des_norm_x.hViceJg = pJg[0]->handle;
	if(pJg[2]&&pJg[3])
		pJg[2]->des_norm_y.hViceJg = pJg[3]->handle;
	if(pJg[2]&&pJg[3])
		pJg[3]->des_norm_x.hViceJg = pJg[2]->handle;
	if(pJg[1]&&pJg[3])
		pJg[3]->des_norm_y.hViceJg = pJg[1]->handle;
	if(pJg[0])
	{
		pJg[0]->des_norm_x.bSpecific = FALSE;
		pJg[0]->des_norm_y.bSpecific = FALSE;
	}
	if(pJg[1])
	{
		pJg[1]->des_norm_x.bSpecific = FALSE;
		pJg[1]->des_norm_y.bSpecific = FALSE;
	}
	if(pJg[2])
	{
		pJg[2]->des_norm_x.bSpecific = FALSE;
		pJg[2]->des_norm_y.bSpecific = FALSE;
	}
	if(pJg[3])
	{
		pJg[3]->des_norm_x.bSpecific = FALSE;
		pJg[3]->des_norm_y.bSpecific = FALSE;
	}
	for( int i=0; i<4; i++ )
	{
		if(pJg[i])
		{
			pJg[i]->cal_x_wing_norm();
			pJg[i]->cal_y_wing_norm();
		}
	}
}
#endif
BOOL CTower::AssertValid()
{
	/*CLDSPart *pPart;
	for(pPart=Parts.GetFirst(CLS_PLATE);pPart;pPart=Parts.GetNext(CLS_PLATE))
	{
		CLDSPlate* pPlate = (CLDSPlate*)pPart;
		if(pPlate->sub_type==TA_PLANK_LJB||pPlate->jdb_style!=5)
			if(!pPlate->AssertValid())
			{
				CString ss;
				ss.Format("��0X%X�Ļ�׼�ڵ���Ϊ��,Ϊ���Ϸ���,�����!",pPlate->handle);
				AfxMessageBox(ss);
				return FALSE;
			}
	}*/
	return TRUE;
}

//�Ƴ���ָ����������������������
int CTower::RemoveRelativeObjs(CLDSDbObject *pDbObj)
{
	if(pDbObj==NULL)
		return 0;
	//��ͨ��CLDSPartList::DeletePart�м�����console.FireDeleteObject���� wjh 2012.11.21
	//if(m_pWorkPart==pDbObj)
	//	m_pWorkPart=NULL;
	if(pDbObj->GetClassTypeId()==CLS_NODE)
		return RemoveNodeRelativeObjs((CLDSNode*)pDbObj);
	else
	{
		int nHits=0;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		CLsRefList *pLsRefList=NULL;
#endif
		CLDSPart *pPart=(CLDSPart*)pDbObj;
		if(pPart->IsLinePart())
		{
			BOOL pushed;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
			pushed=Parts.push_stack();
			if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pPart;
				for(int i=0;i<4;i++)
				{
					CLDSLineAngle *pLineAngle=NULL;
					if(pGroupAngle->son_jg_h[i]>0x20)
						pLineAngle=(CLDSLineAngle*)Parts.FromHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
					if(pLineAngle)
					{
						nHits+=RemoveRelativeObjs(pLineAngle);
						if(CLDSObject::_console!=NULL)
							CLDSObject::_console->FireDeleteObject(pLineAngle);
						Parts.DeleteNode(pLineAngle->handle);
						nHits++;
					}
				}
			}
			else
			{
				for(CLDSPolyJoint *pJoint=(CLDSPolyJoint*)Parts.GetFirst(CLS_POLYJOINT);pJoint;pJoint=(CLDSPolyJoint*)Parts.GetNext())
				{
					if(pJoint->pPrevLinePart==pDbObj||pJoint->pNextLinePart==pDbObj)
					{
						if(CLDSObject::_console!=NULL)
							CLDSObject::_console->FireDeleteObject(pJoint);
						Parts.DeleteNode(pJoint->handle);
						nHits++;
						break;
					}
				}
			}
			Parts.pop_stack(pushed);
			pLsRefList=((CLDSLinePart*)pPart)->GetLsRefList();	//�˼���˨�����б�
#endif
			pushed=Node.push_stack();
			for(CLDSNode *pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
			{
				if(pNode->IsLocked())
					continue;	//�Ѵ����
				if(pNode->hFatherPart==pPart->handle||
					pNode->arrRelationPole[0]==pPart->handle||pNode->arrRelationPole[1]==pPart->handle)
				{
					pNode->Lock();
					if(pNode->m_cPosCalType==0&&pNode->hFatherPart==pPart->handle)	
					{
						pNode->hFatherPart=0;	//��ɾ�����κ������������ wht 09-10-15
						pNode->SetModified();
					}
					else
					{	
						BOOL pushed2=Node.push_stack();
						nHits+=RemoveRelativeObjs(pNode);
						Node.pop_stack(pushed2);
						if(CLDSObject::_console!=NULL)
							CLDSObject::_console->FireDeleteObject(pNode);
						Node.DeleteCursor();
						nHits++;
					}
				}
			}
			Node.pop_stack(pushed);
			//return nHits;
		}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(pPart->GetClassTypeId()==CLS_PLATE)
			pLsRefList=((CLDSPlate*)pPart)->GetLsRefList();		//�ְ������б�
		else if(pPart->GetClassTypeId()==CLS_BOLT)
			return DelBoltAndRelativeBoltRef(pPart->handle);
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			nHits=DelTubeEndLj(pPart->handle);	//ɾ���ֹܶ�����
			pLsRefList=((CLDSParamPlate*)pPart)->GetLsRefList();//�ְ������б�
		}
		if(pLsRefList)
		{	//ɾ����˼���������������˨ wht 11-04-23
			BOLTSET bolts;
			for(CLsRef *pLsRef = pLsRefList->GetFirst(); pLsRef; pLsRef = pLsRefList->GetNext())
				bolts.append(pLsRef->GetLsPtr());
			for (CLDSBolt* pBolt = bolts.GetFirst(); pBolt; pBolt = bolts.GetNext())
			{	//������˨һ�����һ�������ϵ������ڣ�ԭ��Ϊ����Ŀǰ�������ڸ���ϰ�������������� wjh-2015.5.14
				if(!pBolt->m_bVirtualPart)
					continue;
				//������˨�ڶ�������ϴ���ʱ��ɾ������һ��������ֱ�����������˨ wjh-2015.8.31
				bool hasOtherBoltRef=false;
				CLdsPartListStack stack(&Parts);
				for(CLDSPart* pResidePart=Parts.GetFirst();pResidePart;pResidePart=Parts.GetNext())
				{
					if (pResidePart == pPart)
						continue;
					if(pResidePart->FindLsByHandle(pBolt->handle))
					{
						hasOtherBoltRef=true;
						break;
					}
				}
				if(!hasOtherBoltRef)
				{
					if(CLDSObject::_console!=NULL)
						CLDSObject::_console->FireDeleteObject(pBolt);
					Parts.DeleteNode(pBolt->handle);
				}
			}
			if(pPart->IsPlate()&&CLDSObject::sysPara->IsDelBoltsResideOnPlate())
			{	
				CLdsPartListStack stack(&Parts);
				for(CLDSPart* pResidePart=Parts.GetFirst();pResidePart;pResidePart=Parts.GetNext())
				{
					if(pResidePart->GetClassTypeId()==CLS_BOLT)
						continue;
					for(CLDSBolt* pLs=bolts.GetFirst();pLs;pLs=bolts.GetNext())
					{
						if (pResidePart->DeleteLsRef(pLs->handle))
							pResidePart->SetModified();
					}
				}
				for (CLDSBolt* pLs = bolts.GetFirst(); pLs; pLs = bolts.GetNext())
				{
					if (CLDSObject::_console != NULL)
						CLDSObject::_console->FireDeleteObject(pLs, TRUE);
					Parts.DeleteNode(pLs->handle);
					nHits++;
				}
			}
		}
#endif
		return nHits;
	}
}

int CTower::RemoveNodeRelativeObjs(CLDSNode *pNode)
{
	int nHits=0;

	BOOL pushed=Parts.push_stack();
	for(CLDSPart *pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		if(pPart->IsLocked())
			continue;
		if(pPart->IsLinePart()&&(((CLDSLinePart*)pPart)->pStart==pNode||((CLDSLinePart*)pPart)->pEnd==pNode))
		{
			pPart->Lock();
			BOOL pushed2=Parts.push_stack();
			nHits+=RemoveRelativeObjs(pPart);
			Parts.pop_stack(pushed2);
			Parts.DeleteCursor();
			if(CLDSObject::_console!=NULL)
				CLDSObject::_console->FireDeleteObject(pPart);
			nHits++;
		}
	}
	Parts.pop_stack(pushed);
	pushed=Node.push_stack();
	for(CLDSNode *pRelativeNode=Node.GetFirst();pRelativeNode;pRelativeNode=Node.GetNext())
	{
		if(pRelativeNode->IsLocked())
			continue;
		//if( pRelativeNode->arrRelationNode[0]==pNode->handle||pRelativeNode->arrRelationNode[1]==pNode->handle||
		//	pRelativeNode->arrRelationNode[2]==pNode->handle||pRelativeNode->arrRelationNode[3]==pNode->handle)
		if(pRelativeNode->HasRelativeNode(pNode->handle))//���ݽڵ㶨λ����,ֻ�����Ч�Ĺ����ڵ���Ϣ����ƥ��
		{
			pRelativeNode->Lock();
			BOOL pushed2=Node.push_stack();
			nHits+=RemoveRelativeObjs(pRelativeNode);
			Node.pop_stack(pushed2);
			// TODO: ��������ǰ�����BUG,��Ҫ��֤��wjh-2012.8.12
			//DispPartSet.DeleteNode(pRelativeNode->handle);
			if(CLDSObject::_console!=NULL)
				CLDSObject::_console->FireDeleteObject(pRelativeNode);
			Node.DeleteCursor();
			nHits++;
		}
	}
	Node.pop_stack(pushed);

	return nHits;
}

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
int CTower::DelBoltAndRelativeBoltRef(THANDLE ls_handle,BOOL bIncPerm/*=TRUE*/)
{
	CLDSBolt *pLs = (CLDSBolt*)Parts.FromHandle(ls_handle);
	if(pLs==NULL)
		return 0;
	int nHits = 0;
	for(CLDSPart *pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		if (pPart->DeleteLsRef(ls_handle))
		{
			pPart->SetModified();
#ifndef __DRAWING_CONTEXT_
			collector.hashBoltRelaParts.SetValue(pPart->handle,pPart);
#endif
		}
		nHits++;
	}
	if(pLs->IsAggregated())
	{
		CBlockParameter blkpara;
		pLs->GetBlockParameter(blkpara);
		if(blkpara.m_pBlockRef)
			blkpara.m_pBlockRef->hashReserveBolts.DeleteNode(blkpara.m_hMotherObj);
	}
	if(CLDSObject::_console!=NULL)
		CLDSObject::_console->FireDeleteObject(pLs,TRUE);
	Parts.DeleteNode(ls_handle,bIncPerm);
	return nHits;
}
//base���ʾ��˨�Ļ���λ��(�������ϵ��),ucs��ʾһ����˨�����ڵ��������ϵ,
//������ϵ������һ��������˨�Ĺ��������࿿��,iNormX0Y1Z2�ͱ�ʾ
//����˨���������࿿�����������ϵ��������
CLDSBolt* CTower::FindLsByUcsPos(f3dPoint base,UCS_STRU ucs,int iNormX0Y1Z2,double epsilon)
{
	f3dPoint pt,work_norm;
	CLDSBolt *pLs;
	for(pLs=(CLDSBolt*)Parts.GetFirst(CLS_BOLT);pLs!=NULL;pLs=(CLDSBolt*)Parts.GetNext(CLS_BOLT))
	{
		pt = pLs->ucs.origin;
		work_norm = pLs->get_norm();
		coord_trans(pt,ucs,FALSE);
		vector_trans(work_norm,ucs,FALSE);
		if(iNormX0Y1Z2==0)	//X��Ϊ����
		{
			if(fabs(work_norm.x)<EPS_COS2)	//�ж���˨�������߷���
				continue;
			if(fabs(pt.y -base.y)>epsilon)		//�ж���˨λ���Ƿ����
				continue;
			else if(fabs(pt.z -base.z)>epsilon)
				continue;
		}
		else if(iNormX0Y1Z2==1)//Y��Ϊ����
		{
			if(fabs(work_norm.y)<EPS_COS2)	//�ж���˨�������߷���
				continue;
			if(fabs(pt.x -base.x)>epsilon)		//�ж���˨λ���Ƿ����
				continue;
			else if(fabs(pt.z -base.z)>epsilon)
				continue;
		}
		else	//Z��Ϊ����
		{
			if(fabs(work_norm.z)<EPS_COS2)	//�ж���˨�������߷���
				continue;
			if(fabs(pt.y -base.y)>epsilon)		//�ж���˨λ���Ƿ����
				continue;
			else if(fabs(pt.x -base.x)>epsilon)
				continue;
		}
		//��˨��λ��ͬ
		return pLs;
	}
	return NULL;//û�ҵ�
}
CLDSBolt* CTower::FindLsByPos(f3dPoint base,f3dPoint work_norm,double epsilon)
{
	CLDSBolt *pLs;
	normalize(work_norm);
	for(pLs=(CLDSBolt*)Parts.GetFirst(CLS_BOLT);pLs!=NULL;pLs=(CLDSBolt*)Parts.GetNext(CLS_BOLT))
	{
		double dd = DISTANCE(pLs->ucs.origin,base);
		if(dd>epsilon)		//��˨λ�ò���
			continue;
		else if(fabs(dot_prod(work_norm,pLs->get_norm()))<EPS_COS2)
			continue;//��˨�������߷��򲻷�
		else			//�ҵ���ƥ�����˨
			return pLs;
	}
	return NULL;
}

int CTower::GetLsRefCounts(THANDLE hLs,long **ref_part_arr/*=NULL*/)
{
	ATOM_LIST<long> tm_ref_part_arr;
	long ref_hits=0;
	CSuperSmartPtr<CLDSPart>pPart;
	for(pPart=Parts.GetFirst();pPart.IsHasPtr();pPart=Parts.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			if(pPart.PlatePointer()->GetLsRefList()->FromHandle(hLs))
			{
				tm_ref_part_arr.append(pPart->handle);
				ref_hits++;
			}
		}
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			if(pPart.ParamPlatePointer()->GetLsRefList()->FromHandle(hLs))
			{
				tm_ref_part_arr.append(pPart->handle);
				ref_hits++;
			}
		}
		else if(pPart->IsLinePart())
		{
			if(pPart.LinePartPointer()->GetLsRefList()&&pPart.LinePartPointer()->GetLsRefList()->FromHandle(hLs))
			{
				tm_ref_part_arr.append(pPart->handle);
				ref_hits++;
			}
		}
	}
	if(ref_part_arr&&ref_hits>0)
	{
		for(int i=0;i<tm_ref_part_arr.GetNodeNum();i++)
			(*ref_part_arr)[i]=tm_ref_part_arr[i];
	}
	return ref_hits;
}
#endif
CLDSNode *CTower::FromPointI(long point_i)
{
	BOOL pushed=Node.push_stack();
	CLDSNode* pNode;
	for (pNode=EnumNodeFirst();pNode;pNode=EnumNodeNext())
	{
		if (pNode->pointI==point_i)
			break;
	}
	Node.pop_stack(pushed);
	return pNode;
}
bool CTower::ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType/*=0*/)
{
	bool replaced=false;
	Point.push_stack();
	for(CLDSPoint *pPoint=Point.GetFirst();pPoint;pPoint=Point.GetNext())
		replaced|=pPoint->ReplaceReferenceHandle(replacer,NULL);
	Point.pop_stack();
	/*Line.pop_stack();
	for(CLDSLine *pLine=Line.GetFirst();pLine;pLine=Line.GetNext())
		replaced|=pLine->ReplaceReferenceHandle(hOld,hNew);
	Line.pop_stack();*/
	if(idClassType==0||idClassType==CLS_GEPOINT)
	{
		Plane.push_stack();
		for(CLDSPlane *pPlane=Plane.GetFirst();pPlane;pPlane=Plane.GetNext())
		{
			if(replacer.hOld==0)
				pPlane->handle+=replacer.hNew;
			for(int i=0;i<3;i++)
				replaced|=replacer.VerifyAndReplace(&pPlane->hPointArr[i].handle);
		}
		Plane.pop_stack();
	}
	if(replacer.pTag)
	{	//�滻���ǰ���ȸ��½Ǹ�ʼ�ն˵������Ϣ״̬
		replacer.pTag->hashAngles.Empty();
		CLDSLinePart* pDatumPartS=NULL,*pDatumPartE=NULL;
		CLdsPartListStack stack(&Parts);
		for(CLDSPart* pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
		{
			if(pPart->GetClassTypeId()==CLS_LINEANGLE||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSLineAngle* pJg=(CLDSLineAngle*)pPart;
				pDatumPartS=pJg->GetStartDatumPart();
				pDatumPartE=pJg->GetEndDatumPart();
				ANGLEREPLACER_TAG::ANGLE_STATE* pJgState=NULL;
				if((pDatumPartS&&pDatumPartS->handle==replacer.hOld)||
					(pDatumPartE&&pDatumPartE->handle==replacer.hOld))
				{
					pJgState=replacer.pTag->hashAngles.Add(pJg->handle);
					if(pDatumPartS&&pDatumPartS->handle==replacer.hOld)
						pJgState->replaceStartDependRod=true;
					if(pDatumPartE&&pDatumPartE->handle==replacer.hOld)
						pJgState->replaceEndDependRod=true;
				}
			}
			/*else if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle* pGroupJg=(CLDSGroupLineAngle*)pPart;
				for(int i=0;i<4;i++)
				{
					CLDSLineAngle* pJg=(CLDSLineAngle*)FromPartHandle(pGroupJg->son_jg_h[i]);
					if(pJg==NULL)
						continue;
					pDatumPartS=pJg->GetStartDatumPart();
					pDatumPartE=pJg->GetEndDatumPart();
					ANGLEREPLACER_TAG::ANGLE_STATE* pJgState=NULL;
					if((pDatumPartS&&pDatumPartS->handle==replacer.hOld)||
						(pDatumPartE&&pDatumPartE->handle==replacer.hOld))
					{
						pJgState=replacer.pTag->hashAngles.Add(pJg->handle);
						if(pDatumPartS&&pDatumPartS->handle==replacer.hOld)
							pJgState->replaceStartDependRod=true;
						if(pDatumPartE&&pDatumPartE->handle==replacer.hOld)
							pJgState->replaceEndDependRod=true;
					}
				}
			}*/
		}
	}
	//
	int push=Node.push_stack();
	for(CLDSNode *pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
		replaced|=pNode->ReplaceReferenceHandle(replacer,idClassType);
	Node.pop_stack(push);
	push=Parts.push_stack();
	for(CLDSPart* pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
		replaced|=pPart->ReplaceReferenceHandle(replacer,idClassType);
	Parts.pop_stack(push);
	return replaced;
}

CLDSModule *CTower::GetActiveModule()
{
	return Module.FromHandle(m_hActiveModule);
}

CFGWORD CTower::GetDefaultCfgPartNo(int iQuad/*=0*/)
{
	CLDSModule *pModule=GetActiveModule();
	if(pModule)
	{
		if(iQuad>=1&&iQuad<=4)
			return GetCfgWord(pModule->m_arrActiveQuadLegNo[iQuad-1]);
		else
			return CFGWORD(pModule->GetBodyNo(),0);
	}
	else
		return 1;
}

CLDSModule::CLDSModule()
{
	handle=0;
	land_high=0;
	m_bUserSpecifyLowestZ=false;//false:��ʾϵͳ�Զ��������Z0��׼ƽ��;true:�����û�ָ��ֵ����
	m_fNamedHeight=0;			//ָ���������߼���ʱ����0��׼ƽ���Z����ֵ
	m_bSpecifyLevelheightDiff=false;
	m_wnLevelHeight=1000;
	this->iNo = 0;
	m_iBodyNo = 1;				//������(��&ͷ��)����Ӧ����ģ����ȣ���
	m_dwLegCfgWord.SetWordByNo(1);	//������ĺ�
	for(int i=0;i<4;i++)
		m_arrActiveQuadLegNo[i] = 1;//��ģ�͵�ǰ��һ�������޵ĵ�ǰ���Ⱥ�
	m_fBurglarLsStartZ=m_fBurglarLsEndZ=0;
	m_fLockPadStartZ=m_fLockPadEndZ=0;
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN��������Ա���;
	memset(description,0,51);		//����strcpy(,"")��Ϊ�˷�ֹstrncpy(,n)ʱĩβ�ַ�δ��β���µĴ���
	memset(segStr,0,201);
	//strcpy(m_sClassName,"CLDSModule");
	m_nClassTypeId=CLS_HEIGHTMODULE;
	lowest_z=0;
	//AttachWindLoad=NULL;
}

CFGWORD CLDSModule::GetBodyCfgWord()			//��ĺ�m_iBodyLegNo����һ�ֱ�����ʽ
{
	return CFGWORD(GetBodyNo(),0);
}
void CLDSModule::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	buffer.ReadString(description);
#ifdef AFX_TARG_ENU_ENGLISH
	if(stricmp(description,"����")==0)
		strcpy(description,"Module");
#endif
	buffer.ReadString(segStr);
	if(doc_type==3||doc_type==4)
	{
		buffer.ReadInteger(&iNo);
		if( doc_type==3&&(version==0||version>=1070205) ||
			doc_type==4&&(version==0||version>=1000205))
			buffer.ReadDouble(&land_high);
	}
	buffer.ReadWord(&m_iBodyNo);			//������(��&ͷ��)����Ӧ����ģ����ȣ���
	int i;
	//�´������汾��ʱȥ��forѭ�� ��Ϊm_arrQuadLegCfgWord[4]��Ϊ��m_arrQuadLegCfgWord wht 10-05-25
/*#if defined(__TMA_FILE_)||defined(__TMA_)
	if(version==0||version>=4000030)
#elif defined(__LMA_FILE_)||defined(__LMA_)
	if(version==0||version>=1000085)
#elif defined(__LDS_FILE_)||defined(__LDS_) 
	if(version==0||version>=1000012)
#elif defined(__TDA_FILE_)
	if(true)
#else	//TSA
	if(version==0||version>=1070002)
#endif*/
	if(version==0||(doc_type==1&&version>=4000030)||(doc_type==2&&version>=1000085)||
		(doc_type==3&&version>=1070002)||(doc_type==4&&version>=1000012)||doc_type==5)
		CfgwordFromBuffer(m_dwLegCfgWord,buffer,version,doc_type);
	else
	{
		for(i=0;i<4;i++)
			buffer.Read(m_dwLegCfgWord.flag.word,20);
	}
	for(i=0;i<4;i++)
		buffer.ReadWord(&m_arrActiveQuadLegNo[i]);	//��ģ�͵�ǰ��һ~�����޵ĵ�ǰ���Ⱥ�
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020101) ||	//TMA V5.2.1.1
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
	{
		buffer.ReadBooleanThin(&m_bSpecifyLevelheightDiff);
		buffer.ReadWord(&m_wnLevelHeight);	//<=0��ʾ�Զ����㣬>0��ʾָ�� wjh-2019.4.17
	}
	buffer.ReadDouble(&m_fBurglarLsStartZ);	//������˨��ʼ��Χ
	buffer.ReadDouble(&m_fBurglarLsEndZ);	//������˨��ֹ��Χ
/*#if defined(__TMA_FILE_)||defined(__TMA_)
	if(version==0||version>=4000024)//version==NULL||compareVersion(version,"4.0.0.24")>=0)
#elif defined(__LMA_FILE_)||defined(__LMA_)
	if(version==0||version>=1000079)//version==NULL||compareVersion(version,"1.0.0.79")>=0)
#elif defined(__TDA_FILE_)
	if(true)
#else
	if(version==0||version>=1000006)//version==NULL||compareVersion(version,"1.0.0.6")>=0)
#endif*/
	if(version==0||(doc_type==1&&version>=4000024)||(doc_type==2&&version>=1000079)||
		((doc_type==3||doc_type==4)&&version>=1000006)||doc_type==5)
	{
		buffer.ReadDouble(&m_fLockPadStartZ);	//���ɵ�Ȧ��ʼ��Χ
		buffer.ReadDouble(&m_fLockPadEndZ);		//���ɵ�Ȧ��ֹ��Χ
	}
}
void CLDSModule::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	buffer.WriteString(description);
	buffer.WriteString(segStr);
#if defined(__TSA_FILE_)||defined(__LDS_FILE_)
	if(doc_type==3||doc_type==4)
	{
	buffer.WriteInteger(iNo);
	buffer.WriteDouble(land_high);
	}
#endif
	if( (doc_type==1&&version>0&&version<4020006)||	//TMA
		(doc_type==2&&version>0&&version<1020006)||	//LMA
		(doc_type==3&&version>0&&version<2000006)||	//TSA
		(doc_type==4&&version>0&&version<1020006)||	//LDS
		(doc_type==5&&version>0&&version<1020006))	//TDA
		buffer.WriteWord((WORD)(m_iBodyNo*CFGLEG::MaxLegs()));
	else
		buffer.WriteWord(m_iBodyNo);			//������(��&ͷ��)����Ӧ����ģ����ȣ���
	int i;
	//m_arrQuadLegCfgWord[4]��Ϊ��m_arrQuadLegCfgWord wht 10-06-09
	CfgwordToBuffer(m_dwLegCfgWord,buffer,version,doc_type);
	for(i=0;i<4;i++)
		buffer.WriteWord(m_arrActiveQuadLegNo[i]);	//��ģ�͵�ǰ��һ���޵ĵ�ǰ���Ⱥ�
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020101) ||	//TMA V5.2.1.1
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
	{
		buffer.WriteBooleanThin(m_bSpecifyLevelheightDiff);
		buffer.WriteWord(m_wnLevelHeight);	//��λ0x80000000(<=0)��ʾ�Զ����㣬>0��ʾָ�� wjh-2019.4.17
	}
	buffer.WriteDouble(m_fBurglarLsStartZ);	//������˨��ʼ��Χ
	buffer.WriteDouble(m_fBurglarLsEndZ);	//������˨��ֹ��Χ
	buffer.WriteDouble(m_fLockPadStartZ);	//������˨��ʼ��Χ
	buffer.WriteDouble(m_fLockPadEndZ);		//������˨��ֹ��Χ
}
void CLDSModule::FromCdfBuffer(CBuffer &buffer,long version/*=0*/)
{
	buffer.ReadString(description);
	buffer.ReadInteger(&iNo);
	buffer.ReadWord(&m_iBodyNo);			//������(��&ͷ��)����Ӧ����ģ����ȣ���
	buffer.Read(m_dwLegCfgWord.flag.word,24);
	for(int i=0;i<4;i++)
		buffer.ReadWord(&m_arrActiveQuadLegNo[i]);	//��ģ�͵�ǰ��һ~�����޵ĵ�ǰ���Ⱥ�
}
void CLDSModule::ToCdfBuffer(CBuffer &buffer,long version/*=0*/)
{
	buffer.WriteString(description);
	buffer.WriteInteger(iNo);
		buffer.WriteWord(m_iBodyNo);			//������(��&ͷ��)����Ӧ����ģ����ȣ���
	buffer.Write(m_dwLegCfgWord.flag.word,24);
	for(int i=0;i<4;i++)
		buffer.WriteWord(m_arrActiveQuadLegNo[i]);	//��ģ�͵�ǰ��һ���޵ĵ�ǰ���Ⱥ�
}

void CLDSModule::ToXmlFile(FILE* fp,DWORD schema)
{
	char sText[500]="";
	fprintf(fp,"<��ģ����Ϣ>\n");
	fprintf(fp,"<���� description=\"%s\"/>\n",description);
	fprintf(fp,"<�κŷ�Χ segStr=\"%s\"/>\n",segStr);
#if defined(__TSA_FILE_)||defined(__LDS_FILE_)
	fprintf(fp,"<ģ�ͱ�� iNo=\"%d\"/>\n",iNo);
	fprintf(fp,"<���θ߶� land_high=\"%.2f\"/>\n",land_high);
#endif
	fprintf(fp,"<���Ⱥ� BodyLegNo=\"%d\"/>\n",m_iBodyNo);
	int i=0;
	fprintf(fp,"<��ĺ� LegCfgWord=\"(%d,%d,%d,%d)\"/>\n",m_dwLegCfgWord.flag.word[0],m_dwLegCfgWord.flag.word[1],m_dwLegCfgWord.flag.word[2],m_dwLegCfgWord.flag.word[3]);
	for(i=0;i<4;i++)
		fprintf(fp,"<��ǰ���Ⱥ� ActiveQuadLegNo=\"%d\"/>\n",m_arrActiveQuadLegNo[i]);
	sprintf(sText,"%f",m_fBurglarLsStartZ);
	SimplifiedNumString(sText);
	fprintf(fp,"<������˨��ʼ��Χ BurglarLsStartZ=\"%s\"/>\n",sText);
	sprintf(sText,"%f",m_fBurglarLsEndZ);
	SimplifiedNumString(sText);
	fprintf(fp,"<������˨��ֹ��Χ BurglarLsEndZ=\"%s\"/>\n",sText);
	sprintf(sText,"%f",m_fLockPadStartZ);
	SimplifiedNumString(sText);
	fprintf(fp,"<������˨��ʼ��Χ LockPadStartZ=\"%s\"/>\n",sText);
	sprintf(sText,"%f",m_fLockPadEndZ);
	SimplifiedNumString(sText);
	fprintf(fp,"<������˨��ֹ��Χ LockPadEndZ=\"%s\"/>\n",sText);
	fprintf(fp,"</��ģ����Ϣ>\n");
}
//���ز�����͵Ļ���Z���꣨���ո˼�����͵���㣩
//���ز�����ߵ�ͷ��Z���꣨���ո˼�����ߵ���㣩
//����ȫ�����ܸ߶�(mm)
double CLDSModule::GetModuleScopeZ(double *highest/*=NULL*/,double *lowest/*=NULL*/)
{
	BOOL bInit=TRUE;
	double highest_z=0;
	lowest_z=0;
	if(m_pModel==NULL)
		return 0;
	BOOL pushed=m_pModel->PushPartStack(); //���������б�ǰӦ��ѹջ wht 12-03-05
	for(CLDSLinePart *pPole=m_pModel->EnumRodFirst();pPole;pPole=m_pModel->EnumRodNext())
	{
		if (pPole->pStart == NULL || pPole->pEnd == NULL || pPole->m_bVirtualPart)//||pRod->Layer(1)!='Z'
			continue;
		if (!IsSonPart(pPole, NULL, false))
			continue;
		if (bInit)
		{
			bInit = FALSE;
			highest_z = pPole->pStart->xOriginalPos.z;
			highest_z = min(highest_z, pPole->pEnd->xOriginalPos.z);

			lowest_z = pPole->pEnd->xOriginalPos.z;
			lowest_z = max(lowest_z, pPole->pStart->xOriginalPos.z);
		}
		else
		{
			highest_z = min(highest_z, pPole->pStart->xOriginalPos.z);
			highest_z = min(highest_z, pPole->pEnd->xOriginalPos.z);

			lowest_z = max(lowest_z, pPole->pStart->xOriginalPos.z);
			lowest_z = max(lowest_z, pPole->pEnd->xOriginalPos.z);
		}
		/*f3dPoint lineStart,lineEnd;
		if(pPole->pStart)
			lineStart=pPole->pStart->Position(false);
		else
			lineStart=pPole->Start();
		if(pPole->pEnd)
			lineEnd=pPole->pEnd->Position(false);
		else
			lineEnd=pPole->End();
		if(pPole->Layer(0)=='L')
		{
			if(m_dwLegCfgWord.And(pPole->cfgword))
			{
				if(bInit)
				{
					bInit=FALSE;
					highest_z=lineStart.z;
					highest_z=min(highest_z,lineEnd.z);

					lowest_z=lineEnd.z;
					lowest_z=max(lowest_z,lineStart.z);
				}
				else
				{
					highest_z=min(highest_z,lineStart.z);
					highest_z=min(highest_z,lineEnd.z);

					lowest_z=max(lowest_z,lineStart.z);
					lowest_z=max(lowest_z,lineEnd.z);
				}
			}
			else
				continue;
		}
		else if(pPole->cfgword.IsHasBodyNo(GetBodyNo()))	//�����ͷ���˼�
		{
			if(bInit)
			{
				bInit=FALSE;
				highest_z=lineStart.z;
				highest_z=min(highest_z,lineEnd.z);
				lowest_z=lineEnd.z;
				lowest_z=max(lowest_z,lineStart.z);
			}
			else
			{
				highest_z=min(highest_z,lineStart.z);
				highest_z=min(highest_z,lineEnd.z);

				lowest_z=max(lowest_z,lineStart.z);
				lowest_z=max(lowest_z,lineEnd.z);
			}
		}*/
	}
	m_pModel->PopPartStack(pushed);
	if(highest)
		*highest=highest_z;
	if(lowest)
		*lowest=lowest_z;
	return lowest_z-highest_z;
}
CTower* CLDSModule::SuperiorTower()
{
	if(BelongModel()->IsTowerModel())
		return (CTower*)BelongModel();
	else
		return NULL;
}
void CLDSModule::RefreshModule(NODESET &nodeset, LINEPARTSET &poleset,BOOL bVerifyLeg/*=FALSE*/)
{
	if(m_pModel==NULL)
		return;
	BOOL pushed=m_pModel->PushPartStack(); //���������б�ǰӦ��ѹջ wht 12-03-05
	for(CLDSPart *pPart=m_pModel->EnumPartFirst();pPart;pPart=m_pModel->EnumPartNext())
	{
		if(!pPart->IsLinePart())
			continue;
		CSuperSmartPtr<CLDSLinePart> pLinePart = (CLDSLinePart*)pPart;
		if(pLinePart->IsLegObj())
		{
			f3dPoint lineStart,lineEnd;
			if(pLinePart->pStart)
				lineStart=pLinePart->pStart->Position(false);
			else
				lineStart=pLinePart->Start();
			if(pLinePart->pEnd)
				lineEnd=pLinePart->pEnd->Position(false);
			else
				lineEnd=pLinePart->End();
			char cPos=CalPoleQuad(lineStart,lineEnd);
			if(bVerifyLeg)
			{
				if(!(cPos>='1'&&cPos<='4'&&pLinePart->cfgword.IsHasNo(m_arrActiveQuadLegNo[cPos-'1'])))
					continue;	//�ǵ�ǰ��Ч����
				else
					poleset.append(pLinePart);
			}
			else
			{
				if(cPos>='1'&&cPos<='4'&&m_dwLegCfgWord.And(pLinePart->cfgword))
					poleset.append(pLinePart);
				else
					continue;
			}
		}
		else if(pLinePart->cfgword.IsHasBodyNo(GetBodyNo()))	//�����ͷ���˼�
			poleset.append(pLinePart);
	}
	m_pModel->PopPartStack(pushed);
	for(CLDSNode *pNode=m_pModel->EnumNodeFirst();pNode;pNode=m_pModel->EnumNodeNext())
	{
		if(pNode->IsLegObj())
		{
			int iPos=CalPointQuadPos(pNode->Position(false));
			if(bVerifyLeg)
			{
				if(!(iPos>=1&&iPos<=4&&pNode->cfgword.IsHasNo(m_arrActiveQuadLegNo[iPos-1])))
					continue;	//�ǵ�ǰ��Ч���Ƚڵ�
				else
					nodeset.append(pNode);
			}
			else
			{
				if(iPos>=1&&iPos<=4&&m_dwLegCfgWord.And(pNode->cfgword))
					nodeset.append(pNode);
				else
					continue;
			}
		}
		else if(pNode->cfgword.IsHasBodyNo(GetBodyNo()))	//�����ͷ���˼�
			nodeset.append(pNode);
	}
}
#ifdef __PART_DESIGN_INC_
bool CLDSModule::SubBlockCheck(NODESET &boundary,CXhChar50* error_info)
{
	DWORD legFlag=m_dwLegCfgWord.GetBodyLegs(GetBodyNo());//flag.bytes[GetBodyNo()-1];
	DWORD byteConstArr[24]={
		0x000001,0x000002,0x000004,0x000008,0x000010,0x000020,0x000040,0x000080,
		0x000100,0x000200,0x000400,0x000800,0x001000,0x002000,0x004000,0x008000,
		0x010000,0x020000,0x040000,0x080000,0x100000,0x200000,0x400000,0x800000	};
	CLDSNode* pNode;
	CLDSLinePart* pRod;
	NODESET nodeset;
	LINEPARTSET rodset;
	RefreshModule(nodeset,rodset);
	for(int i=0;i<24;i++)
	{
		if((legFlag&byteConstArr[i])==0)
			continue;
		CFGWORD legcfgword(GetBodyNo(),i+1);
		CLDSLinePartPtr legrods[4]={NULL};
		CLDSNodePtr foundationNodes[4]={NULL};
		for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
		{
			pNode->feature=0;
			if(pNode->IsLegObj()&&pNode->cfgword.And(legcfgword))
			{	//���ڵ�ǰ���Ⱥ�
				WORD wRestrict=pNode->GetRestrictWord(handle);
				if((wRestrict&X_RESTRICT)&&(wRestrict&Y_RESTRICT)&&(wRestrict&Z_RESTRICT))
				{	//�����ڵ�
					char cPosQuad=pNode->layer(2);
					if(cPosQuad < '1'||cPosQuad > '4')
					{
						if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
							error_info->Printf("Layer end character of %C leg's base node 0X%X isn't reasonable quadrant number.",'A'+i,pNode->handle);
#else
							error_info->Printf("%C���Ȼ����ڵ�0X%Xͼ��ĩ�ַ����Ǻ������������",'A'+i,pNode->handle);
#endif
						return false;
					}
					if(foundationNodes[cPosQuad-'1']!=NULL)
					{
						if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
							error_info->Printf("%C leg's no. %C quadrant exists multiple base node��0X%X and 0X%X��",'A'+i,cPosQuad,foundationNodes[cPosQuad-'1']->handle,pNode->handle);
#else
							error_info->Printf("%C���ȵ�%C���޴��ڶ�������ڵ㣨0X%X��0X%X��",'A'+i,cPosQuad,foundationNodes[cPosQuad-'1']->handle,pNode->handle);
#endif
						return false;
					}
					else
						foundationNodes[cPosQuad-'1']=pNode;
				}
			}
		}
		for(pRod=rodset.GetFirst();pRod;pRod=rodset.GetNext())
		{
			if(pRod->pStart==NULL||pRod->pEnd==NULL)
				continue;
			if(pRod->pStart->IsLegObj()&&!pRod->cfgword.And(legcfgword))
				continue;	//�����ڵ�ǰ����
			if(pRod->IsAuxPole())
				continue;	//����鸨����
			if(pRod->IsBodyObj())
				pRod->pStart->feature|=0x01;	//���������������
			else
				pRod->pStart->feature|=0x02;	//�н��Ȳ���������
			if(pRod->IsBodyObj())
				pRod->pEnd->feature|=0x01;	//���������������
			else
				pRod->pEnd->feature|=0x02;	//�н��Ȳ���������
			if(pRod->IsLegObj()&&pRod->layer(2)>='1'&&pRod->layer(2)<='4')
				legrods[pRod->layer(2)-'1']=pRod;
		}
		boundary.Empty();
		for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
		{
			if(pNode->IsBodyObj()&&pNode->feature==0x03)
				boundary.append(pNode);
			else if(pNode->IsBodyObj()&&pNode->feature==0x02)
			{
				if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
					error_info->Printf("%d node on tower body(head) only connects with rod on tower leg,data unreasonable!",pNode->point_i);
#else
					error_info->Printf("��(ͷ)��%d�Žڵ�ֻ���Ȳ��˼����������ݲ�����!",pNode->point_i);
#endif
				return false;
			}
			else if(pNode->IsLegObj()&&(pNode->feature==0x01||pNode->feature==0x03))
			{
				if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
					error_info->Printf("%d node on tower leg connects with rod on tower body,data unreasonable!",pNode->point_i);
#else
					error_info->Printf("�Ȳ�%d�Žڵ������˼����������ݲ�����!",pNode->point_i);
#endif
				return false;
			}
		}
		if(boundary.GetNodeNum()<4)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				error_info->Printf("Boundary node less than four.");
#else
				error_info->Printf("�߽�ڵ����ڣ���");
#endif
			return false;
		}
		for(int j=0;j<4;j++)
		{
			if(foundationNodes[j]==NULL)
			{
				if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
					error_info->Printf("%C leg no. %d quadrant lacks of base node.",'A'+i,j+1);
#else
					error_info->Printf("%C���ȵ�%d���޵Ļ����ڵ�ȱʧ",'A'+i,j+1);
#endif
				return false;
			}
			if(legrods[j]==NULL)
			{
				if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
					error_info->Printf("%C leg no. %d quadrant lacks of main rod for tower leg.",'A'+i,j+1);
#else
					error_info->Printf("%C���ȵ�%d���޵��Ȳ�����ȱʧ",'A'+i,j+1);
#endif
				return false;
			}
		}
	}
	return true;
}
#endif

WORD CLDSModule::SetBodyNo(WORD iBodyNo)
{
	//if(iBodyNo>0&&iBodyNo<24)	���ڶ�ȡ���ļ����������ģʱ��ʱ�ı���ſ��ܻᳬ��24,�ʴ˴����ܼӴ��ж�
	//{
	if(m_iBodyNo!=iBodyNo)
	{
		int nMaxLegs=CFGLEG::MaxLegs();
		for(int i=0;i<4;i++)
			m_arrActiveQuadLegNo[i]=nMaxLegs*(iBodyNo-1)+m_arrActiveQuadLegNo[i]%nMaxLegs;
	}
		m_iBodyNo=iBodyNo;
	//}
	return m_iBodyNo;

}
WORD CLDSModule::GetBodyNo(bool bVerityValid/*=true*/)	const//����Ϊһ��������Ϊ��ֹ����ų���1~24���޵���Ӧ��ʱ����Խ�� wjh-2013.4.12
{
	if(!bVerityValid)
		return m_iBodyNo;
	else if(m_iBodyNo<=0)
		return 1;
	else if(m_iBodyNo>24)
		return 24;
	else
		return m_iBodyNo;
}
double CLDSModule::set_NamedHeight(double fNamedHeight)
{
	m_fNamedHeight=fNamedHeight;
	return 0;
}
double CLDSModule::get_NamedHeight()
{
	CTower* pTower=(CTower*)m_pModel;
	m_fNamedHeight=0;
	CLdsPartListStack stack(m_pModel);
	CMaxDouble zLowestCoord;
	for(CLDSLinePart* pRod=pTower->EnumRodFirst();pRod;pRod=pTower->EnumRodNext())
	{
		if(!pRod->IsLegObj()||pRod->pStart==NULL||pRod->pEnd==NULL||pRod->m_bVirtualPart)//||pRod->Layer(1)!='Z'
			continue;
		if(!IsSonPart(pRod,NULL,false))
			continue;
		if( pRod->pStart->xOriginalPos.x<0||pRod->pEnd->xOriginalPos.x<0||
			pRod->pStart->xOriginalPos.y<0||pRod->pEnd->xOriginalPos.y<0)
			continue;	//�˼����ڵ�һ����
		SmartRodPtr pSmartRod=pRod;
		if(pSmartRod->IsCompositeSonAngle())
			continue;
		double fLowRodZ=max(pRod->pStart->xOriginalPos.z,pRod->pEnd->xOriginalPos.z);
		zLowestCoord.Update(fLowRodZ);
	}
	if(zLowestCoord.IsInited())
		m_fNamedHeight=zLowestCoord.number;
	m_fNamedHeight-=pTower->m_fNamedHeightZeroZ;
	return m_fNamedHeight;
}
UINT CLDSModule::set_uiLevelHeight(UINT uiLevelHeightDiff)
{
	if(uiLevelHeightDiff!=0)
		m_bSpecifyLevelheightDiff=true;
	return m_wnLevelHeight=(WORD)uiLevelHeightDiff;
}
UINT CLDSModule::get_uiLevelHeight()
{
	if(this->m_bSpecifyLevelheightDiff)
		return m_wnLevelHeight;
	UpdateValidLegs();
	return m_wnLevelHeight;
}
GEPOINT CLDSModule::GetFoundationLocation(BYTE ciLegSerial/*=1*/)	//ȡֵ1~24����ʾ���Ⱥ�
{
	if(ciLegSerial<=24)
		return xarrBaseLocations[ciLegSerial-1];
	else
		return GEPOINT();
}

UINT CLDSModule::UpdateValidLegs(MODULE_LEGVOIDPTR parrSortedLegs/*=NULL*/,UINT uiMaxCount/*=16*/)
{
	WORD i,count=0,maxCount=CFGLEG::MaxLegs();
	CFGLEG cfgleg=m_dwLegCfgWord.ToCfgLeg();
	DWORD legword=cfgleg.LegFlag();
	PRESET_ARRAY32<MODULE_LEG> validlegs;
	for(i=1;i<=maxCount;i++)
	{
		if((legword&GetSingleWord(i))==0)
			continue;
		MODULE_LEG* pLeg=validlegs.Append(MODULE_LEG(i));
		pLeg->valid=false;
		pLeg->cfgword=CFGWORD(m_iBodyNo,i);
	}
	CLdsPartListStack stack(m_pModel);
	//����ǰ���ݽڵ��жϸ�Ϊ���ݸ˼��˵��жϸ�׼ȷһЩ�����Ա���һЩ�����ڵ㵼�µ����� wjh-2019.4.25
	for(CLDSLinePart* pRod=BelongModel()->EnumRodFirst();pRod;pRod=BelongModel()->EnumRodNext())
	{
		if(!pRod->IsLegObj()||pRod->pStart==NULL||pRod->pEnd==NULL||pRod->m_bVirtualPart)//||pRod->Layer(1)!='Z')
			continue;
		if( pRod->pStart->xOriginalPos.x<0||pRod->pEnd->xOriginalPos.x<0||
			pRod->pStart->xOriginalPos.y<0||pRod->pEnd->xOriginalPos.y<0)
			continue;	//�˼����ڵ�һ����
		SmartRodPtr pSmartRod=pRod;
		if(pSmartRod->IsCompositeSonAngle())//>GetClassTypeId()==CLS_LINEANGLE&&pSmartRod.pAngle->group_father_jg_h>0x20)
			continue;
		double fLowRodZ=max(pRod->pStart->xOriginalPos.z,pRod->pEnd->xOriginalPos.z);
		for(i=0;i<validlegs.Count;i++)
		{
			MODULE_LEG* pSubLeg=&validlegs[i];//hashSubLegs.Add(i+1);
			if(!pRod->cfgword.And(pSubLeg->cfgword))
				continue;
			pSubLeg->valid=true;
			if(pSubLeg->pLegPostRod==NULL)
			{
				pSubLeg->m_fLowestZ=fLowRodZ;
				pSubLeg->pLegPostRod=pRod;
			}
			else if(pSubLeg->m_fLowestZ<fLowRodZ)
			{
				pSubLeg->m_fLowestZ=fLowRodZ;
				pSubLeg->pLegPostRod=pRod;
			}
			break;
		}
	}
	CHeapSort<MODULE_LEG>::HeapSortClassic(validlegs.pPresetObjs,validlegs.Count);
	m_wnLevelHeight=validlegs.Count>=2?ftoi(validlegs[0].m_fLowestZ-validlegs[1].m_fLowestZ):1000;
	UINT nCopyCount=validlegs.Count;
	if(parrSortedLegs)
	{
		nCopyCount=min(validlegs.Count,uiMaxCount);
		memcpy(parrSortedLegs,validlegs.pPresetObjs,nCopyCount*sizeof(MODULE_LEG));
	}
	return nCopyCount;
}
bool CLDSModule::UpdateFoundationLocations()
{
	WORD i,count=0,maxCount=CFGLEG::MaxLegs();
	CFGLEG cfgleg=m_dwLegCfgWord.ToCfgLeg();
	DWORD legword=cfgleg.LegFlag();
	PRESET_ARRAY32<MODULE_LEG> validlegs;
	UINT nCopyCount=UpdateValidLegs(validlegs.pPresetObjs,32);
	validlegs.ReSize(nCopyCount);

	CTower* pTower=(CTower*)BelongModel();
	for(i=0;i<validlegs.Count;i++)
	{
		long liLevelHeightDiff=i<=2?m_wnLevelHeight:ftoi(validlegs[i-1].m_fLowestZ-validlegs[i].m_fLowestZ);
		if(liLevelHeightDiff!=m_wnLevelHeight)
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%d#�ź��߸��ӽ��ȼ�߶ȼ����",this->m_iBodyNo);
		SmartRodPtr pSmartRod=validlegs[i].pLegPostRod;
		if(!pSmartRod.IsHasPtr())
			continue;
		CLDSNode* pBaseNode=pSmartRod->pEnd;
		if(pSmartRod->pStart->xOriginalPos.z>pSmartRod->pEnd->xOriginalPos.z)
			pBaseNode=pSmartRod->pStart;
		if(!pSmartRod->IsAngle()||pSmartRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
			xarrBaseLocations[i]=pBaseNode->xOriginalPos;
		else
		{
			CLDSLineAngle* pAngle=(CLDSLineAngle*)pSmartRod.LineAnglePointer();
			GEPOINT vDiagonal=-(pAngle->vxWingNorm+pAngle->vyWingNorm);
			if(vDiagonal.x>0)
				vDiagonal.x*=-1.0;
			if(vDiagonal.y>0)
				vDiagonal.y*=-1.0;
			normalize(vDiagonal);
			double offsetlen=pAngle->GetWeigthCenterG()*1.414;
			f3dLine line(pAngle->xPosStart+vDiagonal*offsetlen,pAngle->xPosEnd+vDiagonal*offsetlen);
			GEPOINT xWeightCenter;
			Int3dlf(xWeightCenter,line,pBaseNode->xOriginalPos,GEPOINT(0,0,1));
			xarrBaseLocations[i].x=fabs(xWeightCenter.x);
			xarrBaseLocations[i].y=fabs(xWeightCenter.y);
			xarrBaseLocations[i].z=pBaseNode->xOriginalPos.z;
		}
		if(pTower->m_xFoundation.biBasePlateType==FOUNDATION_PARA::TYPE_TOP)	//���ڵװ���ƽ��
			xarrBaseLocations[i].z+=pTower->m_xFoundation.wiBasePlateThick;
		else if(pTower->m_xFoundation.biBasePlateType==FOUNDATION_PARA::TYPE_BTM)	//���ڵװ���ƽ��
			xarrBaseLocations[i].z=pBaseNode->xOriginalPos.z;
		else //if(pTreeModel->pTower->m_xFoundation.biBasePlateType==FOUNDATION_PARA::TYPE_MID)	//���ڵװ��м�
			xarrBaseLocations[i].z+=pTower->m_xFoundation.wiBasePlateThick*0.5;
	}
	return true;
}
DWORD CLDSModule::ActiveQuadLegDword()
{
	DWORD dwQuadLeg=0;
	for(int i=0;i<4;i++)
		((BYTE*)&dwQuadLeg)[i]=(BYTE)(m_arrActiveQuadLegNo[i]-1)%8+1;
	return dwQuadLeg;
}
short CLDSModule::GetCfgWordLegCount()	//��ǰ����m_dwLegCfgWord�����а����Ľ�������
{
	DWORD legword = m_dwLegCfgWord.GetBodyLegs(this->idBodyNo);
	DWORD xarrConstDwords[24] = {
		0x00000001,0x00000002,0x00000004,0x00000008,0x00000010,0x00000020,0x00000040,0x00000080,
		0x00000100,0x00000200,0x00000400,0x00000800,0x00001000,0x00002000,0x00004000,0x00008000,
		0x00001000,0x00002000,0x00004000,0x00008000,0x00010000,0x00020000,0x00040000,0x00080000};
	int i, count = 0;
	for (i = 0; i < 24; i++)
	{
		if (xarrConstDwords[i] & legword)
			count = i;
	}
	return count;
}
//���ݵ�ǰ�����ڵĽ�����ţ�1Ϊ����,1~24��ת��Ϊȫģ�͵�������(1~192) wjh-2019.7.22
WORD  CLDSModule::ToLegSerialBySerialInHeight(int serialInHeight)
{
	WORD wiLegSerial=CFGLEG::MaxLegs()*(idBodyNo-1)+ serialInHeight;
	return wiLegSerial;
}
short CLDSModule::GetLegCount()	//ͳ�Ƶ�ǰ�����������ʵ����Ч�������� wjh-2016.6.14
{
	WORD i,maxCount=CFGLEG::MaxLegs();
	CFGLEG cfgleg=m_dwLegCfgWord.ToCfgLeg();
	DWORD legword=cfgleg.LegFlag();
	PRESET_ARRAY32<MODULE_LEG> validlegs;;
	for(i=1;i<=maxCount;i++)
	{
		if((legword&GetSingleWord(i))==0)
			continue;
		MODULE_LEG* pLeg=validlegs.Append(MODULE_LEG(i));
		pLeg->valid=false;
		pLeg->cfgword=CFGWORD(m_iBodyNo,i);
	}
	CLdsPartListStack stack(m_pModel);
	for(CLDSLinePart* pRod=BelongModel()->EnumRodFirst();pRod;pRod=BelongModel()->EnumRodNext())
	{
		if(!pRod->IsLegObj()||pRod->pStart==NULL||pRod->pEnd==NULL||pRod->m_bVirtualPart)
			continue;
		bool hasEmptyFlag=false;
		for(i=0;i<validlegs.Count;i++)
		{
			validlegs[i].valid|=pRod->cfgword.And(validlegs[i].cfgword);
			hasEmptyFlag|=!validlegs[i].valid;
		}
		if(!hasEmptyFlag)
			break;
	}
	WORD count=0;
	for(i=0;i<validlegs.Count;i++)
	{
		if(validlegs[i].valid)
			count++;
	}
	return count;
}

BOOL CLDSModule::IsSonPart(CLDSDbObject *pObj,CFGWORD *pSpecCfgWord/*=NULL*/,bool bVerityActLeg/*=true*/)
{
	CLDSNode *pNode;
	CFGWORD soncfgword;
	if(pObj==NULL)
		return FALSE;
	if(pSpecCfgWord==NULL)
		soncfgword=pObj->cfgword;
	else
		soncfgword=*pSpecCfgWord;
	if(pObj->GetClassTypeId()==CLS_NODE)
	{
		pNode=(CLDSNode*)pObj;
		if(pNode->IsLegObj())
		{
			if(bVerityActLeg)
			{
				int iPos=CalPointQuadPos(pNode->Position(false));
				if(iPos>=1&&iPos<=4&&soncfgword.IsHasNo(m_arrActiveQuadLegNo[iPos-1]))
					return TRUE;
				else
					return FALSE;
			}
			else if(m_dwLegCfgWord.And(soncfgword))
				return TRUE;
			else
				return FALSE;
		}
		else if(soncfgword.IsHasBodyNo(GetBodyNo()))	//�����ͷ���˼�
			return TRUE;
		else
			return FALSE;
	}
	else if(pObj->IsPart())
	{
		CSuperSmartPtr<CLDSPart> pPart=(CLDSPart*)pObj;
		char cPos;
		if(pPart->IsLinePart())
		{
			f3dPoint lineStart,lineEnd;
			if(pPart.LinePartPointer()->pStart)
				lineStart=pPart.LinePartPointer()->pStart->Position(false);
			else
				lineStart=pPart.LinePartPointer()->Start();
			if(pPart.LinePartPointer()->pEnd)
				lineEnd=pPart.LinePartPointer()->pEnd->Position(false);
			else
				lineEnd=pPart.LinePartPointer()->End();
			cPos=CalPoleQuad(lineStart,lineEnd);
		}
		else//CalPointQuadPos�������ͺ�CalPoleQuad��һ�� Ԥ��Ϊchar����ֵΪint ��ʱ�޸� 
			cPos='0'+(BYTE)CalPointQuadPos(pPart->ucs.origin);
		if(pPart->IsLegObj())
		{
			if(bVerityActLeg)
			{
				if(cPos>='1'&&cPos<='4'&&soncfgword.IsHasNo(m_arrActiveQuadLegNo[cPos-'1']))
					return TRUE;
				else
					return FALSE;
			}
			else if(m_dwLegCfgWord.And(soncfgword))
				return TRUE;
			else
				return FALSE;
		}
		else if(soncfgword.IsHasBodyNo(GetBodyNo()))	//�����ͷ���˼�
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}

CObjNoGroup::CObjNoGroup(int nMaxNo/*=0*/)
{
	m_idInit=1;
	m_nMaxNo=nMaxNo;
}
void CObjNoGroup::InitMaxNo(int nMaxNo)
{
	m_nMaxNo=nMaxNo;
	int i=0;
	for(LDSOBJNO *pNo=list.GetFirst();pNo;pNo=list.GetNext())
	{
		if(pNo->iNo>nMaxNo)
		{
			list.DeleteAt(i);
			list.GetPrev();
			i--;
		}
		i++;
	}
}
long CObjNoGroup::GetUsedNoCount()
{
	long nHits=0;
	for(long iNo=GetFirstUsedNo();iNo>0;iNo=GetNextUsedNo())
		nHits++;
	return nHits;
}
long CObjNoGroup::GetFirstUsedNo()
{
	for(LDSOBJNO *pNo=list.GetFirst();pNo;pNo=list.GetNext())
	{
		if(pNo->bInUse)
			return pNo->iNo;
	}
	return 0;
}
long CObjNoGroup::GetNextUsedNo()
{
	for(LDSOBJNO *pNo=list.GetNext();pNo;pNo=list.GetNext())
	{
		if(pNo->bInUse)
			return pNo->iNo;
	}
	return 0;
}
int CObjNoGroup::EnumIdleNo()
{
	for(int i=m_idInit;i<=m_nMaxNo;i++)
	{
		BOOL bInUse=FALSE;
		LDSOBJNO *pNo;
		for(pNo=list.GetFirst();pNo;pNo=list.GetNext())
		{
			if(pNo->iNo==i&&pNo->bInUse)
			{
				bInUse=TRUE;
				break;
			}
			else if(pNo->iNo==i)
				return pNo->iNo;
			else
				continue;
		}
		if(!bInUse)
		{
			pNo=list.append();
			pNo->iNo=i;
			pNo->bInUse=FALSE;
			return i;
		}
	}
	return 0;
}
void CObjNoGroup::SetNoState(int iNo,BOOL bInUse/*=TRUE*/)
{
	LDSOBJNO *pNo;
	for(pNo=list.GetFirst();pNo;pNo=list.GetNext())
	{
		if(pNo->iNo==iNo)
		{
			pNo->bInUse=bInUse;
			return;
		}
	}
	pNo=list.append();
	pNo->iNo=iNo;
	pNo->bInUse=bInUse;
}

void CObjNoGroup::EmptyNo()
{
	list.Empty();
}

CObjNoManager::CObjNoManager()
{
	AppendGroup(MODEL_DATUM_POINT_GROUP,1000);		//��׼�ڵ������1000
	AppendGroup(MODEL_DATUM_LINE_GROUP,1000);		//��׼�ߵ������1000
	AppendGroup(MODEL_DATUM_PLANE_GROUP,1000);		//��׼��������1000
	AppendGroup(MODEL_ARCLIFT_GROUP,1000);			//Ԥ���������1000
	AppendGroup(MODEL_GROUP,1000);					//����ģ�������1000
	AppendGroup(WIRELAYOUT_GROUP,100);				//���߷��������100
	AppendGroup(WORKSTATUS_GROUP,10000);			//���㹤�������10000
	AppendGroup(WINDSEG_GROUP,1000);				//����ѹ�α��1000
	AppendGroup(WORKSTATUS_ATTACH_WINDLOAD,1000);	//����ѹ�α��1000
	AppendGroup(WIRELOAD_MODEL_GROUP,1000);			//����������ر��1000
	AppendGroup(FL_OPTIMIZOR_GROUP,10000);			//����Ż�ѡ�ķ������10000
	//CObjNoGroup* pParaCSGroup=AppendGroup(PARAMETRIC_CS_GROUP,100000);			//���װ������ϵ���100000
	//pParaCSGroup->m_idInit=4;	//1~3�������̶�X/Y/Z��ԳƱ任
}
CObjNoGroup* CObjNoManager::AppendGroup(long ID,long nMaxNo)
{
	CObjNoGroup* pGroup=NoGroupList.append();
	pGroup->m_idGroup=ID;
	pGroup->m_nMaxNo=nMaxNo;
	return pGroup;
}

CObjNoGroup* CObjNoManager::FromGroupID(long ID)
{
	for(CObjNoGroup* pGroup=NoGroupList.GetFirst();pGroup;pGroup=NoGroupList.GetNext())
	{
		if(pGroup->m_idGroup==ID)
			return pGroup;
	}
	return NULL;
}
void CObjNoManager::EmptyGroupNo()
{
	for(CObjNoGroup*pGroup=NoGroupList.GetFirst();pGroup;pGroup=NoGroupList.GetNext())
		pGroup->EmptyNo();
}
void CObjNoManager::EmptyGroup()
{
	NoGroupList.Empty();
}

/*BOOL CTower::SelectPartSet(PARTSET* pPartSet,int CLASS_TYPEID,char* key,...)
{
	int j=0,k=0;
	//����ѡ��ģʽ TRUE:��ģʽ FALSE:��ģʽ  **δ���**  WJH--00.08.15
	int cond_mode=TRUE;
	CStack <BOOL> cond_stack;
	CStack <BOOL> flag_stack;
	BOOL bFlag,bTmFlag;//�ڵ�ѡ��״̬ TRUE:ѡ�� FALSEδѡ��
	va_list ap;
	f3dPoint pt;
    char keyword[50];
    long   lval;
	PARTSET allpartset;
	if(CLASS_TYPEID == CLS_LINEANGLE || CLASS_TYPEID==0)
	{
		Parts.push_stack();
		for(CLDSLineAngle *pLineAngle=(CLDSLineAngle*)Parts.GetFirst(CLS_LINEANGLE);pLineAngle;pLineAngle=(CLDSLineAngle*)Parts.GetNext(CLS_LINEANGLE))
		{
			if(pLineAngle->sub_type&TA_JG_GROUPSON)
				continue;
			allpartset.append(pLineAngle);
		}
		Parts.pop_stack();
		Parts.push_stack();
		for(CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)Parts.GetFirst(CLS_GROUPLINEANGLE);pGroupAngle;pGroupAngle=(CLDSGroupLineAngle*)Parts.GetNext(CLS_GROUPLINEANGLE))
			allpartset.append(pGroupAngle);
		Parts.pop_stack();
	}
	if(CLASS_TYPEID == CLS_LINETUBE || CLASS_TYPEID==0)
	{
		Parts.push_stack();
		for(CLDSLineTube *pPipe=(CLDSLineTube*)Parts.GetFirst(CLS_LINETUBE);pPipe;pPipe=(CLDSLineTube*)Parts.GetNext(CLS_LINETUBE))
			allpartset.append(pPipe);
		Parts.pop_stack();
	}
	if(CLASS_TYPEID == CLS_LINEFLAT || CLASS_TYPEID==0)
	{
		Parts.push_stack();
		//for(CLDSLineFlat *pLineFlat=Flats.GetFirst();pLineFlat;pLineFlat=Flats.GetNext())
		//	allpartset.append(pLineFlat);
		Parts.pop_stack();
	}

	pPartSet->Empty();
	for(CLDSPart *pPart=allpartset.GetFirst();pPart!=NULL;pPart=allpartset.GetNext())
	{
		CLDSLinePart *pLinePart=NULL;
		if(pPart->IsLinePart())
			pLinePart = (CLDSLinePart*)pPart;
		if(pLinePart==NULL||pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;	//��һ�˽ڵ�Ϊ��ʱ��֤�������͹������û��ֶ����ӽǸ�

		bFlag = TRUE;
		va_start(ap, key);
		strcpy(keyword, key);
		
		for(j=0;j<30;j++)
		{
			if(strcmp(keyword, "<��")==0)
			{
				if(!cond_stack.push(cond_mode))
					AfxMessageBox("����̫����,�������");
				cond_mode=FALSE;
				flag_stack.push(bFlag);
				//if(j==0)
					bFlag = FALSE;
			}
			else if(strcmp(keyword, "��>")==0)
			{
				if(cond_mode)
					AfxMessageBox("����ƥ�����,������ܳ���");
				cond_stack.pop(cond_mode);
				flag_stack.pop(bTmFlag);
				if(cond_mode)
					bFlag&=bTmFlag;
				else
					bFlag|=bTmFlag;
			}
			else if(strcmp(keyword, "<��")==0)
			{
				if(!cond_stack.push(cond_mode))
					AfxMessageBox("����̫����,�������");
				cond_mode=TRUE;
				flag_stack.push(bFlag);
				//if(j==0)
					bFlag = TRUE;
			}
			else if(strcmp(keyword, "��>")==0)
			{
				if(!cond_mode)
					AfxMessageBox("���������,������ܳ���");
				cond_stack.pop(cond_mode);
				flag_stack.pop(bTmFlag);
				if(cond_mode)
					bFlag&=bTmFlag;
				else
					bFlag|=bTmFlag;
			}
			
			else if(strcmp(keyword, "�����")==0)
			{
				//strcpy(keyword, va_arg(ap,char*));
				lval = va_arg(ap,THANDLE);
				BOOL ff = FALSE;
				if(pLinePart->pStart->handle==lval)
					ff = TRUE;
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else //��ģʽ
						bFlag = bFlag||ff;
			}
			
			else if(strcmp(keyword, "�յ���")==0)
			{
				lval = va_arg(ap,THANDLE);
				BOOL ff = FALSE;
				if(pLinePart->pEnd->handle==lval)//��ģʽ
					ff = TRUE;
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else //��ģʽ
						bFlag = bFlag||ff;
			}
			
			else if(strcmp(keyword, "���Ⱥ�")==0)
			{
				//strcpy(keyword, va_arg(ap,char*));
				BOOL ff = FALSE;
				lval = va_arg(ap,long);
				if(pLinePart->cfgword.IsHasNo(lval))
					ff = TRUE;
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else //��ģʽ
						bFlag = bFlag||ff;
			}
			
			else if(strcmp(keyword, "�ֶκ�")==0)
			{
				BOOL ff = FALSE;
				lval = va_arg(ap,long);
				if(pLinePart->iSeg==lval)
					ff = TRUE;
				//strcpy(keyword, va_arg(ap,char*));
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else //��ģʽ
						bFlag = bFlag||ff;
			}

			else if(strcmp(keyword, "ͼ��")==0)
			{
				strcpy(keyword, va_arg(ap,char*));
				CString str(keyword);
				CStringArray str_arr;
				for(j=0;;j++)
				{
					if(str.IsEmpty())
						break;
					str_arr.SetAtGrow(j,str.SpanExcluding(",;:"));
					if(str_arr[j].IsEmpty())
						str.Delete(0);
					else
					{
						int n = str_arr[j].GetLength();
						if(n == str.GetLength())
							str.Empty();
						else
							str.Delete(0,n+1);
					}
				}

				//TRUE��ʾƥ�� FALSE ��ʾ��ƥ��  WJH--00.08.15
				BOOL ff;//��ʶ�Ƿ�ƥ��  WJH--00.08.15
				for(k=0;k<str_arr.GetSize();k++)
				{
					ff = TRUE;
					str = str_arr[k];
					for(int m=0;m<3;m++)
					{
						if(toupper(pLinePart->layer(m))!=toupper(str[m])
							&& str[m]!='?')//��һ�ƥ��  WJH--00.08.15
							ff = FALSE;
					}
					if(ff) 
						break;
				}
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else		//��ģʽ
						bFlag = bFlag||ff;
				
			}

			else if(strcmp(keyword, "�ַ�1")==0)
			{
				strcpy(keyword, va_arg(ap,char*));
				int n = strlen(keyword);
				BOOL ff = FALSE;
				for(k=0;k<n;k++)
				{
					if(toupper(pLinePart->layer(0))==toupper(keyword[k])||pLinePart->layer(0)=='?'||keyword[k]=='?')
						ff = TRUE;
				}
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else //��ģʽ
						bFlag = bFlag||ff;
			}
			
			else if(strcmp(keyword, "�ַ�2")==0)
			{
				strcpy(keyword, va_arg(ap,char*));
				int n = strlen(keyword);
				BOOL ff = FALSE;
				for(k=0;k<n;k++)
				{
					if(toupper(pLinePart->layer(1))==toupper(keyword[k])||pLinePart->layer(1)=='?'||keyword[k]=='?')
						ff = TRUE;
				}
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else //��ģʽ
						bFlag = bFlag||ff;
			}
			
			else if(strcmp(keyword, "�ַ�3")==0)
			{
				strcpy(keyword, va_arg(ap,char*));
				int n = strlen(keyword);
				BOOL ff = FALSE;
				for(k=0;k<n;k++)
				{
					if(toupper(pLinePart->layer(2))==toupper(keyword[k])||pLinePart->layer(2)=='?'||keyword[k]=='?')
						ff = TRUE;
				}
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else //��ģʽ
						bFlag = bFlag||ff;
			}
			
			else if(strcmp(keyword, "����")==0)
			{
				if(bFlag)
					pPartSet->append(pLinePart);
				va_end(ap);
				break;
			}
			else
				return FALSE;
			strcpy(keyword, va_arg(ap,char*));
		}
	}
	return TRUE;
}*/

BOOL CTower::SelectNodeSet(NODESET* pNodeSet,char* key,...)
{
	int j=0,k=0;
	//����ѡ��ģʽ TRUE:��ģʽ FALSE:��ģʽ  **δ���**  WJH--00.08.15
	int cond_mode=TRUE;
	CStack <BOOL> cond_stack;
	CStack <BOOL> flag_stack;
	BOOL bFlag,bTmFlag;//�ڵ�ѡ��״̬ TRUE:ѡ�� FALSEδѡ��
	va_list ap;
	f3dPoint pt;
	CLDSNode *pNode;
    char keyword[50];
/*    char   ch;
    double fval;
    short  nval;*/
    long   lval;
	pNodeSet->Empty();
	for(pNode=Node.GetFirst();pNode!=NULL;pNode=Node.GetNext())
	{
		bFlag = TRUE;
		va_start(ap, key);
		strcpy(keyword, key);
#ifdef AFX_TARG_ENU_ENGLISH
		if(strcmp(keyword, "Node")!=0)
#else
		if(strcmp(keyword, "�ڵ�")!=0)
#endif
			return FALSE;
		
		for(j=0;j<30;j++)
		{
			strcpy(keyword, va_arg(ap,char*));
#ifdef AFX_TARG_ENU_ENGLISH
			if(strcmp(keyword, "< Or")==0)
#else
			if(strcmp(keyword, "<��")==0)
#endif
			{
				cond_stack.push(cond_mode);
				cond_mode=FALSE;
				flag_stack.push(bFlag);
				//if(j==0)
					bFlag = FALSE;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(strcmp(keyword, "Or >")==0)
#else
			else if(strcmp(keyword, "��>")==0)
#endif
			{
				if(cond_mode)
					return FALSE;	//AfxMessageBox("����ƥ�����,������ܳ���");
				cond_stack.pop(cond_mode);
				flag_stack.pop(bTmFlag);
				if(cond_mode)
					bFlag&=bTmFlag;
				else
					bFlag|=bTmFlag;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(strcmp(keyword, "< And")==0)
#else
			else if(strcmp(keyword, "<��")==0)
#endif
			{
				cond_stack.push(cond_mode);
				cond_mode=TRUE;
				flag_stack.push(bFlag);
				//if(j==0)
					bFlag = TRUE;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(strcmp(keyword, "And >")==0)
#else
			else if(strcmp(keyword, "��>")==0)
#endif
			{
				if(!cond_mode)
					return FALSE;	//AfxMessageBox("����ƥ�����,������ܳ���");
				cond_stack.pop(cond_mode);
				flag_stack.pop(bTmFlag);
				if(cond_mode)
					bFlag&=bTmFlag;
				else
					bFlag|=bTmFlag;
			}

#ifdef AFX_TARG_ENU_ENGLISH
			else if(strcmp(keyword, "Node Coordinate")==0)
#else
			else if(strcmp(keyword, "�ڵ�����")==0)
#endif
			{
				//strcpy(keyword, va_arg(ap,char*));
				BOOL ff = FALSE;
				pt = va_arg(ap,f3dPoint);
				if(pNode->Position(false)==pt)
					ff = TRUE;
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else //��ģʽ
						bFlag = bFlag||ff;
			}

#ifdef AFX_TARG_ENU_ENGLISH
			else if(strcmp(keyword, "Leg Label")==0)
#else
			else if(strcmp(keyword, "���Ⱥ�")==0)
#endif
			{
				//strcpy(keyword, va_arg(ap,char*));
				BOOL ff = FALSE;
				lval = va_arg(ap,long);
				if(pNode->cfgword.IsHasNo(lval))
					ff = TRUE;
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else //��ģʽ
						bFlag = bFlag||ff;
			}

#ifdef AFX_TARG_ENU_ENGLISH
			else if(strcmp(keyword, "Segment Number")==0)
#else
			else if(strcmp(keyword, "�ֶκ�")==0)
#endif
			{
				BOOL ff = FALSE;
				lval = va_arg(ap,long);
				if(pNode->iSeg.iSeg==lval)
				{
					ff = TRUE;
					break;
				}
				//strcpy(keyword, va_arg(ap,char*));
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else //��ģʽ
						bFlag = bFlag||ff;
			}

#ifdef AFX_TARG_ENU_ENGLISH
			else if(strcmp(keyword, "Father Angle")==0)
#else
			else if(strcmp(keyword, "���Ǹ�")==0)
#endif
			{
				BOOL ff = FALSE;
				lval = va_arg(ap,long);
				if(pNode->hFatherPart==lval)
					ff = TRUE;
				//strcpy(keyword, va_arg(ap,char*));
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else //��ģʽ
						bFlag = bFlag||ff;
			}

#ifdef AFX_TARG_ENU_ENGLISH
			else if(strcmp(keyword, "Relation Type")==0)
#else
			else if(strcmp(keyword, "��������")==0)
#endif
			{
				BOOL ff = FALSE;
				lval = va_arg(ap,long);
				if(pNode->m_cPosCalType==lval)
					ff = TRUE;
				//strcpy(keyword, va_arg(ap,char*));
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else //��ģʽ
						bFlag = bFlag||ff;
			}

#ifdef AFX_TARG_ENU_ENGLISH
			else if(strcmp(keyword, "Relation Angle")==0)
#else
			else if(strcmp(keyword, "�����Ǹ�")==0)
#endif
			{
				BOOL ff = FALSE;
				lval = va_arg(ap,long);
				if(pNode->arrRelationPole[0]==lval||pNode->arrRelationPole[1]==lval)
					ff = TRUE;
				//strcpy(keyword, va_arg(ap,char*));
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else //��ģʽ
						bFlag = bFlag||ff;
			}

#ifdef AFX_TARG_ENU_ENGLISH
			else if(strcmp(keyword, "Relation Node")==0)
#else
			else if(strcmp(keyword, "�����ڵ�")==0)
#endif
			{
				BOOL ff = FALSE;
				lval = va_arg(ap,long);
				if(pNode->arrRelationNode[0]==lval||pNode->arrRelationNode[1]==lval)
					ff = TRUE;
				//strcpy(keyword, va_arg(ap,char*));
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else //��ģʽ
						bFlag = bFlag||ff;
			}

#ifdef AFX_TARG_ENU_ENGLISH
			else if(strcmp(keyword, "Layer")==0)
#else
			else if(strcmp(keyword, "ͼ��")==0)
#endif
			{
				strcpy(keyword, va_arg(ap,char*));
				char* str = keyword;
				char *key=strtok(str,",;:");
				BOOL ff;//��ʶ�Ƿ�ƥ��  WJH--00.08.15
				while(key!=NULL)
				{
					if(strlen(key)!=3)
						continue;
					ff = TRUE;
					for(int m=0;m<3;m++)
					{
						if(toupper(pNode->layer(m))!=toupper(str[m])
							&& str[m]!='?')//��һ�ƥ��  WJH--00.08.15
							ff = FALSE;
					}
					if(ff) 
						break;
					key=strtok(NULL,",;:");
				}
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else if(pNode->Position(false)==pt)//��ģʽ
						bFlag = bFlag||ff;
				/*CStringArray str_arr;
				for(j=0;;j++)
				{
					if(str.IsEmpty())
						break;
					str_arr.SetAtGrow(j,str.SpanExcluding(",;:"));
					if(str_arr[j].IsEmpty())
						str.Delete(0);
					else
					{
						int n = str_arr[j].GetLength();
						if(n == str.GetLength())
							str.Empty();
						else
							str.Delete(0,n+1);
					}
				}

				//TRUE��ʾƥ�� FALSE ��ʾ��ƥ��  WJH--00.08.15
				BOOL ff;//��ʶ�Ƿ�ƥ��  WJH--00.08.15
				for(k=0;k<str_arr.GetSize();k++)
				{
					ff = TRUE;
					str = str_arr[k];
					for(int m=0;m<3;m++)
					{
						if(toupper(pNode->layer(m))!=toupper(str[m])
							&& str[m]!='?')//��һ�ƥ��  WJH--00.08.15
							ff = FALSE;
					}
					if(ff) 
						break;
				}*/
				
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(strcmp(keyword, "Character 1")==0)
#else
			else if(strcmp(keyword, "�ַ�1")==0)
#endif
			{
				strcpy(keyword, va_arg(ap,char*));
#ifdef _ARX_2007
				int n=int(strlen(keyword));
#else 
				int n = strlen(keyword);
#endif
				BOOL ff = FALSE;
				for(k=0;k<n;k++)
				{
					if(toupper(pNode->layer(0))==toupper(keyword[k])||pNode->layer(0)=='?'||keyword[k]=='?')
						ff = TRUE;
				}
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else //��ģʽ
						bFlag = bFlag||ff;
			}

#ifdef AFX_TARG_ENU_ENGLISH
			else if(strcmp(keyword, "Character 2")==0)
#else
			else if(strcmp(keyword, "�ַ�2")==0)
#endif
			{
				strcpy(keyword, va_arg(ap,char*));
#ifdef _ARX_2007
				int n = int(strlen(keyword));
#else
				int n = strlen(keyword);
#endif
				BOOL ff = FALSE;
				for(k=0;k<n;k++)
				{
					if(toupper(pNode->layer(1))==toupper(keyword[k])||pNode->layer(1)=='?'||keyword[k]=='?')
						ff = TRUE;
				}
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else //��ģʽ
						bFlag = bFlag||ff;
			}

#ifdef AFX_TARG_ENU_ENGLISH
			else if(strcmp(keyword, "Character 3")==0)
#else
			else if(strcmp(keyword, "�ַ�3")==0)
#endif
			{
				strcpy(keyword, va_arg(ap,char*));
#ifdef _ARX_2007
				int n = int(strlen(keyword));
#else
				int n = strlen(keyword);
#endif
				BOOL ff = FALSE;
				for(k=0;k<n;k++)
				{
					if(toupper(pNode->layer(2))==toupper(keyword[k])||pNode->layer(2)=='?'||keyword[k]=='?')
						ff = TRUE;
				}
				if(cond_mode)//��ģʽ
					bFlag = bFlag&&ff;
				else //��ģʽ
						bFlag = bFlag||ff;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(strcmp(keyword, "End")==0)
#else
			else if(strcmp(keyword, "����")==0)
#endif
			{
				if(bFlag)
					pNodeSet->append(pNode);
				va_end(ap);
				break;
			}
			else
				return FALSE;
		}
	}
	return TRUE;
}
//<DEVELOP_PROCESS_MARK nameId="CTower.RetrieveUdfViews">
//����V��б�ĵ�����ڵ�
CLDSNode* FindExtendNode(CLDSLinePart* pLinePart,BOOL bStart)
{
	//���������б�Ĺ��ߵ��Ȳ�б�ļ���
	CLDSNode* pNode=NULL;
	if(bStart)
		pNode=pLinePart->pEnd;
	else
		pNode=pLinePart->pStart;
	f3dPoint vec=pLinePart->pStart->Position()-pLinePart->pEnd->Position();
	normalize(vec);
	IModel* pModel=pLinePart->BelongModel();
	LINEPARTSET linePartSet;
	for(CLDSPart* pPart=pModel->EnumPartFirst();pPart;pPart=pModel->EnumPartNext())
	{
		if(!pPart->IsLinePart())
			continue;
		CLDSLinePart* pLine=(CLDSLinePart*)pPart;
		if(pLine->IsBodyObj() || pLine->layer(1)!='X')
			continue;
		if(pLine==pLinePart||pLine->pStart==NULL||pLine->pEnd==NULL)
			continue;
		f3dPoint line_vec=pLine->pStart->Position()-pLine->pEnd->Position();
		normalize(line_vec);
		if(fabs(vec*line_vec)<EPS_COS)	//������
			continue;
		linePartSet.append(pLine);
	}
	//�����˼����ϣ�����ͨ�������ڵ����ӵ�б�ģ�ֱ������б��
	BOOL bFinished=FALSE;
	while(!bFinished)
	{
		int flag=0;
		for(CLDSLinePart* pLine=linePartSet.GetFirst();pLine;pLine=linePartSet.GetNext())
		{
			if(pLine->pStart==pNode)
				pNode=pLine->pEnd;
			else if(pLine->pEnd==pNode)
				pNode=pLine->pStart;
			if(pLine->pStart==pNode || pLine->pEnd==pNode)
			{	
				flag=1;
				linePartSet.DeleteCursor();
				break;
			}
		}	
		linePartSet.Clean();
		if(flag==0)
			bFinished=TRUE;
	}
	return pNode;
}
long GetFrontPartHandle(LINEPARTSET &linePartSet,CLDSNode* pDatumNode)
{
	if(linePartSet.GetNodeNum()!=2)
		return 0;
	double fVX1=0,fVX2=0,fVY1=0,fVY2=0;
	f3dPoint startPt,endPt;
	if(linePartSet[0]->pStart==pDatumNode)
	{
		startPt=linePartSet[0]->pStart->xOriginalPos;
		endPt=linePartSet[0]->pEnd->xOriginalPos;
	}
	else
	{
		startPt=linePartSet[0]->pEnd->xOriginalPos;
		endPt=linePartSet[0]->pStart->xOriginalPos;
	}
	fVX1=fabs(startPt.x-endPt.x);
	fVY1=fabs(startPt.y-endPt.y);
	if(linePartSet[1]->pStart==pDatumNode)
	{
		startPt=linePartSet[1]->pStart->xOriginalPos;
		endPt=linePartSet[1]->pEnd->xOriginalPos;
	}
	else
	{
		startPt=linePartSet[1]->pEnd->xOriginalPos;
		endPt=linePartSet[1]->pStart->xOriginalPos;
	}
	fVX2=fabs(startPt.x-endPt.x);
	fVY2=fabs(startPt.y-endPt.y);
	if(fVX1>fVX2 && fVY1<fVY2)
		return linePartSet[0]->handle;
	else if(fVX1<fVX2 && fVY1>fVY2)
		return linePartSet[1]->handle;
	else
		return 0;
}
static CDisplayView* AppendUdfView(UCS_STRU ucs,char* view_name,char* folder_name,char* layer)
{
	IModel* pModel=CLDSObject::_console->GetActiveModel();
	if(pModel==NULL)
		return NULL;
	CDisplayView *pView;
	for(pView=pModel->EnumViewFirst();pView;pView=pModel->EnumViewNext())
	{
		if(stricmp(pView->name,view_name)==0)
		{
			pView->EmptyPartSet();
			break;
		}
	}
	if(pView==NULL)
		pView=pModel->AppendView();
	pView->m_iViewType=1;	//������ͼ
	strncpy(pView->name,view_name,50);
	strncpy(pView->folderName,folder_name,16);
	strncpy(pView->defaultLayer,layer,3);
	pView->ucs=ucs;
	for(CLDSPart *pDispPart=CLDSObject::_console->EnumFirstDispPart();pDispPart;pDispPart=CLDSObject::_console->EnumNextDispPart())
	{
		CViewPart *pViewPart = pView->AppendViewPart(pDispPart->handle);
		pViewPart->idClassType=pDispPart->GetClassTypeId();
	}
	for(CLDSNode *pDispNode=CLDSObject::_console->EnumFirstDispNode();pDispNode;pDispNode=CLDSObject::_console->EnumNextDispNode())
	{
		CViewPart *pViewPart = pView->AppendViewPart(pDispNode->handle);
		pViewPart->idClassType=CLS_NODE;
	}
	return pView;
}
static long GetLinePartHandle(CLDSNode* pStart,CLDSNode* pEnd)
{
	if(pStart==NULL || pEnd==NULL)
		return 0;
	IModel* pModel=pStart->BelongModel();
	for(CLDSPart* pPart=pModel->EnumPartFirst();pPart;pPart=pModel->EnumPartNext())
	{
		if(!pPart->IsLinePart())
			continue;
		CLDSLinePart* pLinePart=(CLDSLinePart*)pPart;
		if((pLinePart->pStart==pStart && pLinePart->pEnd==pEnd) ||
			(pLinePart->pStart==pEnd && pLinePart->pEnd==pStart))
			return pLinePart->handle;
	}
	return 0;
}
//��ʼ���Ȳ�V��չ����ͼ�л���Ĺ�������
static BOOL InitBasePlacePartSet(CLDSNode* pDatumNode,CLDSNode* pVNode)
{
	if(pDatumNode==NULL)
		return FALSE;
	IModel* pModel=pDatumNode->BelongModel();
	CLDSLinePart* pFatherPart=(CLDSLinePart*)pModel->FromHandle(pDatumNode->hFatherPart);
	if(pFatherPart==NULL)
		return FALSE;
	CLDSNode* pBaseNode=pDatumNode;	//�����ڵ�
	DYN_ARRAY<f3dPoint> pt_arr(3);
	pt_arr[0]=pDatumNode->Position();
	pt_arr[1]=pVNode->Position();
	if(pFatherPart->pStart==pDatumNode)
		pDatumNode=pFatherPart->pEnd;
	else
		pDatumNode=pFatherPart->pStart;
	if(pDatumNode->Position().z<=pVNode->Position().z)
		pt_arr[2]=pDatumNode->Position();
	else
	{
		f3dPoint vec,line_vec;
		vec=pDatumNode->Position()-pt_arr[0];
		normalize(vec);
		BOOL bFinded=FALSE;
		for(CLDSPart* pPart=pModel->EnumPartFirst();pPart;pPart=pModel->EnumPartNext())
		{
			if(!pPart->IsLinePart() || pPart==pFatherPart)
				continue;
			if (pPart->IsBodyObj() || pPart->Layer(1)!='Z')
				continue;
			CLDSLinePart* pLine=(CLDSLinePart*)pPart;
			line_vec=(pLine->pStart->Position()-pLine->pEnd->Position());
			normalize(line_vec);
			if(fabs(vec*line_vec)<EPS_COS)	//������
				continue;
			if(pLine->pStart==pDatumNode&&line_vec.z<0)
				pDatumNode=pLine->pEnd;
			else if(line_vec.z>0)
				pDatumNode=pLine->pStart;
			if(pDatumNode->Position().z<=pVNode->Position().z)
			{
				bFinded=TRUE;
				break;
			}
		}
		pt_arr[2]=pDatumNode->Position();
		if(!bFinded)
			return FALSE;
	}
	POLYGON selRgn;
	if(!selRgn.CreatePolygonRgn(pt_arr,3))
		return FALSE;
	if(!CTower::InitDisplaySetByPolyRgn(selRgn,pBaseNode))
		return FALSE;
	return TRUE;
}
void CTower::RetrieveUdfViews()
{
	/*1.��ȡ����������ͼ
	  ������ʽ:'H'+'X'/'Y'+'������'+'-�Ҳ�ڵ���'
	  ��ȡԭ��:�����������Ľڵ�Գ�, ˮƽ���в���
	*/
	UCS_STRU ucs;
	CLDSNode* pNode;
	NODESET nodeset;
	for(pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
		nodeset.append(pNode);
	for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
	{
		//1������ͼ�㼰�Գ�����ѡȡ��׼�ڵ�
		if(pNode->Layer(0)!='S')			//�ڵ�λ������
			continue;
		CLDSLinePart* pFatherPart=FromRodHandle(pNode->hFatherPart);
		if(pFatherPart->Layer(1)!='Z')		//�ڵ㸸�˼���������
			continue;
		if(pNode->Position().x>0 || pNode->Position().y<0)	//�Եڶ����޵Ľڵ�Ϊ��׼�ڵ�
			continue;
		LINEPARTSET linePartSet;
		CLDSNode* pMirX = pNode->GetMirNode(MIRMSG(1));
		CLDSNode* pMirY = pNode->GetMirNode(MIRMSG(2));
		CLDSNode* pMirZ = pNode->GetMirNode(MIRMSG(3));
		if(pMirX==NULL || pMirY==NULL || pMirZ==NULL)		//�������ԳƵĽڵ�
			continue;
		//2���ж϶Գƽڵ�֮���Ƿ�����
		for(int i=0;i<4;i++)
		{
			CLDSNode* pStart=NULL,*pEnd=NULL;
			if(i==0 || i==1)
				pStart=pNode;
			else
				pStart=pMirZ;
			if(i==1 || i==2)
				pEnd=pMirX;
			else
				pEnd=pMirY;
			CLDSLinePart* pLinePart=(CLDSLinePart*)FromPartHandle(GetLinePartHandle(pStart,pEnd));
			if(pLinePart)
				linePartSet.append(pLinePart);
		}
		if(linePartSet.GetNodeNum()!=4)
			continue;
		//3������1,2���޵Ľڵ���ȡ�������ͼ�Ĺ�������
		int nCounts = 4;
		f3dPoint pPoints[4];
		POLYGON selRgn;
		Sub_Pnt(ucs.axis_x,pNode->xOriginalPos,pMirY->xOriginalPos);
		ucs.axis_y=-CLDSObject::_console->GetActiveView()->ucs.axis_z;
		ucs.axis_z = cross_prod(ucs.axis_x,ucs.axis_y);
		normalize(ucs.axis_z);
		if(ucs.axis_z.z>0)
			ucs.axis_z *= -1.0;
		ucs.axis_x = cross_prod(ucs.axis_y,ucs.axis_z);
		ucs.axis_y = cross_prod(ucs.axis_z,ucs.axis_x);
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		ucs.origin = pNode->xOriginalPos;
		//ȷ������ι�������
		nCounts=4;
		pPoints[0] = pNode->xOriginalPos+CLDSObject::_console->GetActiveView()->ucs.axis_z*100000;
		pPoints[1] = pMirY->xOriginalPos+CLDSObject::_console->GetActiveView()->ucs.axis_z*100000;
		pPoints[2] = pMirY->xOriginalPos-CLDSObject::_console->GetActiveView()->ucs.axis_z*100000;
		pPoints[3] = pNode->xOriginalPos-CLDSObject::_console->GetActiveView()->ucs.axis_z*100000;
		if(!selRgn.CreatePolygonRgn(pPoints,nCounts))
			continue;
		int pushed=Node.push_stack();
		BOOL bTrue=InitDisplaySetByPolyRgn(selRgn);	//�����趨�������ʼ����ʾ��������
		Node.pop_stack(pushed);
		if(bTrue==FALSE)
			continue;
		//4���жϺ�������Ƿ񲼲ģ����ο�����б�ģ�
		BOOL bLayoutMat=FALSE;
		for(CLDSPart* pDispPart=CLDSPart::_console->EnumFirstDispPart();pDispPart;pDispPart=CLDSPart::_console->EnumNextDispPart())
		{
			if(!pDispPart->IsLinePart())
				continue;
			if(pDispPart->handle==linePartSet[0]->handle || pDispPart->handle==linePartSet[1]->handle ||
				pDispPart->handle==linePartSet[2]->handle || pDispPart->handle==linePartSet[3]->handle)
				continue;
			if(pDispPart->Layer(1)!='Z')
			{
				bLayoutMat=TRUE;
				break;
			}
		}
		if(!bLayoutMat)
			continue;
		//5����������������ͼ
		CXhChar50 name,folder_name,layer;
		sprintf(folder_name,"A%s",&pNode->iSeg.ToString());
		strcpy(layer,"SPG");			//������ͼĬ��ͼ��
		sprintf(name, "HX0X%X-0X%X",pNode->handle,pMirY->handle);
		AppendUdfView(ucs,name,folder_name,layer);
	}
	/*2.��ȡͷ���������ͼ
	  ������ʽ:'T'+'X'/'Y'+'��(ʼ)����'+'-��(��)����'
	  ��ȡԭ��:ͷ�����ļ�ǰ������ҶԳ�, չ�����в���,չ����������
	*/
	CLDSPart* pPart;
	PARTSET partset;
	for(pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
		partset.append(pPart);
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		if(!pPart->IsLinePart())
			continue;
		//1������ͼ�㼰�Գƹ�ϵ��ȡ��׼�˼�
		CLDSLinePart* pLinePart=(CLDSLinePart*)pPart;
		if(pLinePart->pStart==NULL || pLinePart->pEnd==NULL)		//�������˼�
			continue;
		if(pLinePart->Layer(0)!='T' || pLinePart->Layer(1)!='Z')	//��ͷ������
			continue;
		CLDSNode* pStart=pLinePart->pStart;
		CLDSNode* pEnd=pLinePart->pEnd;
		if(pStart->Position().x>0 || pStart->Position().y<0 ||
			pEnd->Position().x>0 || pEnd->Position().y<0)			//�Եڶ����޵ĸ˼�Ϊ��׼�˼�
			continue;
		CLDSLinePart* pMirPart=pMirPart=(CLDSLinePart*)pLinePart->GetMirPart(MIRMSG(1));
		if(pMirPart==NULL)			//û�жԳƵ�����
			continue;
		//2�����ݻ�׼�˼���ȡ�ᵣչ������ͼ�Ĺ�������
		f3dPoint jg_vec;
		Sub_Pnt(jg_vec,pStart->xOriginalPos,pEnd->xOriginalPos);
		if(jg_vec.z>0)	//�Ǹַ�����ȡΪ����Ϊ��
			jg_vec = -1.0*jg_vec;
		CLDSNode* pMirStart=pMirPart->pStart,*pMirEnd=pMirPart->pEnd;
		Sub_Pnt(ucs.axis_y,pMirStart->xOriginalPos,pStart->xOriginalPos);
		ucs.axis_z = cross_prod(jg_vec,ucs.axis_y);
		normalize(ucs.axis_z);
		if(ucs.axis_z.z>0)
			ucs.axis_z *= -1.0;
		ucs.axis_x = cross_prod(ucs.axis_y,ucs.axis_z);
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		normalize(ucs.axis_z);
		ucs.origin = pStart->xOriginalPos;
		int pushed=Parts.push_stack();
		BOOL bTrue=InitDisplaySetByMirPoint(pLinePart,pMirStart,pMirEnd);	//���ݶԳƽڵ���ɵľ��������ʼ����ͼ�Ĺ�������
		Parts.pop_stack(pushed);
		if(!bTrue)
			continue;
		//3���жϺᵣչ�������Ƿ񲼲�(���ο�����б��)
		pushed=Parts.push_stack();
		long hStartPart=GetLinePartHandle(pStart,pMirStart);
		long hEndPart=GetLinePartHandle(pEnd,pMirEnd);
		Parts.pop_stack(pushed);
		BOOL bLayoutMat=FALSE;
		for(CLDSPart* pDispPart=CLDSPart::_console->EnumFirstDispPart();pDispPart;pDispPart=CLDSPart::_console->EnumNextDispPart())
		{
			if(!pDispPart->IsLinePart())
				continue;
			if(pDispPart->handle==pLinePart->handle || pDispPart->handle==pMirPart->handle || 
				pDispPart->handle==hStartPart || pDispPart->handle==hEndPart)
				continue;
			if(pDispPart->Layer(1)!='Z')
			{
				bLayoutMat=TRUE;
				break;
			}
		}
		if(!bLayoutMat)
			continue;
		//4������ͷ���������ͼ
		CXhChar50 view_name,folder_name,layer;
		sprintf(view_name,"TX0X%X-0X%X",pLinePart->pStart->handle,pLinePart->pEnd->handle);
		sprintf(folder_name,"A%s",&pLinePart->iSeg.ToString());
		strcpy(layer,"TPG");	//������ͼĬ��ͼ��
		AppendUdfView(ucs,view_name,folder_name,layer);
	}
	/*3.��ȡV��չ����ͼ
	  ��ȡԭ��:�Ȳ������ڵ���������Vб�������������Ľڵ�
    */
	for(CLDSNode* pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		//1����ȡ�Ȳ���׼�ڵ�
		if(pNode->IsBodyObj())			//�ڵ�λ������
			continue;
		CLDSLinePart* pFatherPart=(CLDSLinePart*)FromHandle(pNode->hFatherPart);
		if(pFatherPart->IsBodyObj() || pFatherPart->Layer(1)!='Z' || pFatherPart->Layer(2)!='1')		//�ڵ㸸�˼������Ȳ�����
			continue;
		//2����ȡ��ڵ����ӵ���Vб��
		LINEPARTSET linePartSet; 
		for(CLDSPart* pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
		{
			if(!pPart->IsLinePart())
				continue;
			CLDSLinePart* pLinePart=(CLDSLinePart*)pPart;
			if(pLinePart->IsBodyObj() || pLinePart->Layer(1)!='X')		//�˼������Ȳ�б��
				continue;
			if(pLinePart->pStart!=pNode && pLinePart->pEnd!=pNode)			//б����ڵ�����
				continue;
			linePartSet.append(pLinePart);
		}
		if(linePartSet.GetNodeNum()!=2)
			continue;
		//3����ȡ��Vб�������������Ľڵ�
		ARRAY_LIST<CLDSNodePtr> nodeSet;
		nodeSet.SetSize(0,2);
		BOOL bStart=FALSE;
		for(CLDSLinePart* pLine=linePartSet.GetFirst();pLine;pLine=linePartSet.GetNext())
		{
			if(pLine->pStart==pNode)
				bStart=TRUE;
			else
				bStart=FALSE;
			CLDSNode* pVNode=FindExtendNode(pLine,bStart);
			if(pVNode)
				nodeSet.append(pVNode);
		}
		if(nodeSet.GetSize()!=2)
			continue;
		//4�����������ڵ���ȡV��չ����ͼ�Ĺ�������
		f3dPoint jg_vec;
		DYN_ARRAY<f3dPoint> pt_arr(3);
		pt_arr[0]=pNode->xOriginalPos;
		for(int i=0;i<nodeSet.GetSize();i++)
			pt_arr[i+1] = nodeSet[i]->xOriginalPos;
		ucs.axis_x.Set(1,0,0);	//���ȣ�V��������ͼ�̶���ģ��X��Ϊ��ͼ����ϵX�� wjh-2014.4.17
		if(fabs(pt_arr[1].x-pt_arr[0].x)>fabs(pt_arr[2].x-pt_arr[0].x))	//pt1��ΪV�������pt2ΪV������
			Sub_Pnt(jg_vec,pNode->xOriginalPos,nodeSet[0]->xOriginalPos);
		else
			Sub_Pnt(jg_vec,pNode->xOriginalPos,nodeSet[1]->xOriginalPos);
		ucs.axis_z=ucs.axis_x^jg_vec;
		normalize(ucs.axis_z);
		if(ucs.axis_z.z>0)
			ucs.axis_z*=-1.0;
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		if(ucs.axis_y.z>0)
		{
			ucs.axis_x*=-1.0;
			ucs.axis_y*=-1.0;
		}
		ucs.origin = pNode->xOriginalPos;
		POLYGON selRgn;
		if(!selRgn.CreatePolygonRgn(pt_arr,3))
			continue;
		int pushed=Node.push_stack();
		BOOL bTrue=InitDisplaySetByPolyRgn(selRgn,pNode);
		Node.pop_stack(pushed);
		if(bTrue==FALSE)
			continue;
		//5������˾��ͼV��չ����ͼ��������ʽ:'LV'+'�����ڵ���'(��������ͼΪ��׼��V�����Ȳ�Vб��չ��)
		CXhChar50 view_name,folder_name,layer;
		sprintf(folder_name,"A%s",&pNode->iSeg.ToString());
		strcpy(layer,"LPG");			//������ͼĬ��ͼ��
		sprintf(view_name,"LV0X%X",pNode->handle);
		CDisplayView *pViewLV=AppendUdfView(ucs,view_name,folder_name,layer);
		CObjNoGroup NoGroup;
		NoGroup.InitMaxNo(15);
		int idle_no=NoGroup.EnumIdleNo();
		pViewLV->listAssist.Empty();
		CDrawingAssistant *pAssistant=pViewLV->listAssist.append();
		//��ʼ��չ������Ϣ����ͼ�����Ĺ�������Ϣ
		pAssistant->iNo=idle_no;
		pAssistant->m_nStyle=0;		//չ����
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(pAssistant->name,"Extend Plane %d",pAssistant->iNo);
#else
		sprintf(pAssistant->name,"չ����%d",pAssistant->iNo);
#endif
		pAssistant->m_iSeg=pNode->iSeg;
		pAssistant->m_xLineStart.datum_pos_style=8;				//��ڵ�ͬΪ
		pAssistant->m_xLineStart.des_para.hNode=pNode->handle;
		pAssistant->m_xFaceNorm.norm_style=2;					//���˼������淨��
		pAssistant->m_xFaceNorm.hVicePart=linePartSet[0]->handle;	//��׼�˼�
		pAssistant->m_xFaceNorm.hCrossPart=linePartSet[1]->handle;			//���潻��˼�
		pAssistant->m_xFaceNorm.nearVector.Set(0,0,1);
		THANDLE hFrontPart=GetFrontPartHandle(linePartSet,pNode);
		for(CViewPart *pItem=pViewLV->GetFirstViewPart();pItem;pItem=pViewLV->GetNextViewPart())
		{
			if(pItem->idClassType==CLS_NODE)
				continue;
			if(pItem->handle==hFrontPart)
				continue;
			pItem->iResidePlaneNo=pAssistant->iNo;	//��ͼ����������
		}
		//��ȡ����Ĺ�������
		CLDSNode* pVNode=NULL;
		double fX=pNode->Position().x;
		if(fabs(nodeSet[0]->Position().x-fX)>fabs(nodeSet[1]->Position().x-fX))
			pVNode=nodeSet[0];
		else
			pVNode=nodeSet[1];
		pushed=Node.push_stack();
		bTrue=InitBasePlacePartSet(pNode,pVNode);
		Node.pop_stack();
		if(!bTrue)
			continue;
		for(CLDSPart *pDispPart=CLDSPart::_console->EnumFirstDispPart();pDispPart;pDispPart=CLDSPart::_console->EnumNextDispPart())
		{
			if(pDispPart->handle==hFrontPart)
				continue;		//��������й���
			CViewPart *pViewPart = pViewLV->AppendViewPart(pDispPart->handle);
			pViewPart->idClassType=pDispPart->GetClassTypeId();
		}
		for(CLDSNode *pDispNode=CLDSPart::_console->EnumFirstDispNode();pDispNode;pDispNode=CLDSPart::_console->EnumNextDispNode())
		{
			if(pDispNode->handle==pNode->handle)
				continue;		//��������нڵ�
			CViewPart *pViewPart = pViewLV->AppendViewPart(pDispNode->handle);
			pViewPart->idClassType=CLS_NODE;
		}
	}
}
//</DEVELOP_PROCESS_MARK>
void CTower::RetrieveDrawingViews()
{
	/*1.��ȡ����������ͼ
	  ������ʽ:'H'+'X'/'Y'+'������'+'-�Ҳ�ڵ���'
	  ��ȡԭ��:�����������Ľڵ�Գ�, ˮƽ���в���
	*/
	UCS_STRU ucs;
	for(CLDSNode* pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		//1������ͼ�㼰�Գ�����ѡȡ��׼�ڵ�
		if(pNode->Layer(0)!='S')			//�ڵ�λ������
			continue;
		CLDSLinePart* pFatherPart=FromRodHandle(pNode->hFatherPart);
		if(pFatherPart==NULL||pFatherPart->Layer(1)!='Z')		//�ڵ㸸�˼���������
			continue;
		if(pNode->Position().x>0 || pNode->Position().y<0)	//�Եڶ����޵Ľڵ�Ϊ��׼�ڵ�
			continue;
		CAtomListStack<CLDSNode> stack(&Node);
		LINEPARTSET linePartSet;
		CLDSNode* pMirX = pNode->GetMirNode(MIRMSG(1));
		CLDSNode* pMirY = pNode->GetMirNode(MIRMSG(2));
		CLDSNode* pMirZ = pNode->GetMirNode(MIRMSG(3));
		if(pMirX==NULL || pMirY==NULL || pMirZ==NULL)		//�������ԳƵĽڵ�
			continue;
		//2���ж϶Գƽڵ�֮���Ƿ�����
		for(int i=0;i<4;i++)
		{
			CLDSNode* pStart=NULL,*pEnd=NULL;
			if(i==0 || i==1)
				pStart=pNode;
			else
				pStart=pMirZ;
			if(i==1 || i==2)
				pEnd=pMirX;
			else
				pEnd=pMirY;
			CLDSLinePart* pLinePart=(CLDSLinePart*)FromPartHandle(GetLinePartHandle(pStart,pEnd));
			if(pLinePart)
				linePartSet.append(pLinePart);
		}
		if(linePartSet.GetNodeNum()!=4)
			continue;
		//3������1,2���޵Ľڵ���ȡ�������ͼ�Ĺ�������
		int nCounts = 4;
		f3dPoint pPoints[4];
		POLYGON selRgn;
		Sub_Pnt(ucs.axis_x,pNode->xOriginalPos,pMirY->xOriginalPos);
		ucs.axis_y=-CLDSObject::_console->GetActiveView()->ucs.axis_z;
		ucs.axis_z = cross_prod(ucs.axis_x,ucs.axis_y);
		normalize(ucs.axis_z);
		if(ucs.axis_z.z>0)
			ucs.axis_z *= -1.0;
		ucs.axis_x = cross_prod(ucs.axis_y,ucs.axis_z);
		ucs.axis_y = cross_prod(ucs.axis_z,ucs.axis_x);
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		ucs.origin = pNode->xOriginalPos;
		//ȷ������ι�������
		nCounts=4;
		pPoints[0] = pNode->xOriginalPos+CLDSObject::_console->GetActiveView()->ucs.axis_z*100000;
		pPoints[1] = pMirY->xOriginalPos+CLDSObject::_console->GetActiveView()->ucs.axis_z*100000;
		pPoints[2] = pMirY->xOriginalPos-CLDSObject::_console->GetActiveView()->ucs.axis_z*100000;
		pPoints[3] = pNode->xOriginalPos-CLDSObject::_console->GetActiveView()->ucs.axis_z*100000;
		if(!selRgn.CreatePolygonRgn(pPoints,nCounts))
			continue;
		int pushed=Node.push_stack();
		BOOL bTrue=InitDisplaySetByPolyRgn(selRgn);	//�����趨�������ʼ����ʾ��������
		Node.pop_stack(pushed);
		if(bTrue==FALSE)
			continue;
		//4���жϺ�������Ƿ񲼲ģ����ο�����б�ģ�
		BOOL bLayoutMat=FALSE;
		for(CLDSPart* pDispPart=CLDSPart::_console->EnumFirstDispPart();pDispPart;pDispPart=CLDSPart::_console->EnumNextDispPart())
		{
			if(!pDispPart->IsLinePart())
				continue;
			if(pDispPart->handle==linePartSet[0]->handle || pDispPart->handle==linePartSet[1]->handle ||
				pDispPart->handle==linePartSet[2]->handle || pDispPart->handle==linePartSet[3]->handle)
				continue;
			if(pDispPart->Layer(1)!='Z')
			{
				bLayoutMat=TRUE;
				break;
			}
		}
		if(!bLayoutMat)
			continue;
		//5����������������ͼ
		CXhChar50 name,folder_name,layer;
		sprintf(folder_name,"A%s",&pNode->iSeg.ToString());
		strcpy(layer,"SPG");			//������ͼĬ��ͼ��
		sprintf(name, "HX0X%X-0X%X",pNode->handle,pMirY->handle);
		AppendUdfView(ucs,name,folder_name,layer);
	}
	/*2.��ȡͷ���������ͼ
	  ������ʽ:'T'+'X'/'Y'+'��(ʼ)����'+'-��(��)����'
	  ��ȡԭ��:ͷ�����ļ�ǰ������ҶԳ�, չ�����в���,չ����������
	*/
	for(CLDSPart* pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		if(!pPart->IsLinePart())
			continue;
		//1������ͼ�㼰�Գƹ�ϵ��ȡ��׼�˼�
		CLDSLinePart* pLinePart=(CLDSLinePart*)pPart;
		if(pLinePart->pStart==NULL || pLinePart->pEnd==NULL)		//�������˼�
			continue;
		if(pLinePart->Layer(0)!='T' || pLinePart->Layer(1)!='Z')	//��ͷ������
			continue;
		CLDSNode* pStart=pLinePart->pStart;
		CLDSNode* pEnd=pLinePart->pEnd;
		if(pStart->Position().x>0 || pStart->Position().y<0 ||
			pEnd->Position().x>0 || pEnd->Position().y<0)			//�Եڶ����޵ĸ˼�Ϊ��׼�˼�
			continue;
		CLDSLinePart* pMirPart=pMirPart=(CLDSLinePart*)pLinePart->GetMirPart(MIRMSG(1));
		if(pMirPart==NULL)			//û�жԳƵ�����
			continue;
		//2�����ݻ�׼�˼���ȡ�ᵣչ������ͼ�Ĺ�������
		f3dPoint jg_vec;
		Sub_Pnt(jg_vec,pStart->xOriginalPos,pEnd->xOriginalPos);
		if(jg_vec.z>0)	//�Ǹַ�����ȡΪ����Ϊ��
			jg_vec = -1.0*jg_vec;
		CLDSNode* pMirStart=pMirPart->pStart,*pMirEnd=pMirPart->pEnd;
		Sub_Pnt(ucs.axis_y,pMirStart->xOriginalPos,pStart->xOriginalPos);
		ucs.axis_z = cross_prod(jg_vec,ucs.axis_y);
		normalize(ucs.axis_z);
		if(ucs.axis_z.z>0)
			ucs.axis_z *= -1.0;
		ucs.axis_x = cross_prod(ucs.axis_y,ucs.axis_z);
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		normalize(ucs.axis_z);
		ucs.origin = pStart->xOriginalPos;
		int pushed=Parts.push_stack();
		BOOL bTrue=InitDisplaySetByMirPoint(pLinePart,pMirStart,pMirEnd);	//���ݶԳƽڵ���ɵľ��������ʼ����ͼ�Ĺ�������
		Parts.pop_stack(pushed);
		if(!bTrue)
			continue;
		//3���жϺᵣչ�������Ƿ񲼲�(���ο�����б��)
		pushed=Parts.push_stack();
		long hStartPart=GetLinePartHandle(pStart,pMirStart);
		long hEndPart=GetLinePartHandle(pEnd,pMirEnd);
		Parts.pop_stack(pushed);
		BOOL bLayoutMat=FALSE;
		for(CLDSPart* pDispPart=CLDSPart::_console->EnumFirstDispPart();pDispPart;pDispPart=CLDSPart::_console->EnumNextDispPart())
		{
			if(!pDispPart->IsLinePart())
				continue;
			if(pDispPart->handle==pLinePart->handle || pDispPart->handle==pMirPart->handle || 
				pDispPart->handle==hStartPart || pDispPart->handle==hEndPart)
				continue;
			if(pDispPart->Layer(1)!='Z')
			{
				bLayoutMat=TRUE;
				break;
			}
		}
		if(!bLayoutMat)
			continue;
		//4������ͷ���������ͼ
		CXhChar50 view_name,folder_name,layer;
		sprintf(view_name,"TX0X%X-0X%X",pLinePart->pStart->handle,pLinePart->pEnd->handle);
		sprintf(folder_name,"A%s",&pLinePart->iSeg.ToString());
		strcpy(layer,"TPG");	//������ͼĬ��ͼ��
		AppendUdfView(ucs,view_name,folder_name,layer);
	}
	/*3.��ȡV��չ����ͼ
	  ��ȡԭ��:�Ȳ������ڵ���������Vб�������������Ľڵ�
    */
	for(CLDSNode* pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		//1����ȡ�Ȳ���׼�ڵ�
		if(pNode->IsBodyObj())			//�ڵ�λ������
			continue;
		CLDSLinePart* pFatherPart=(CLDSLinePart*)FromHandle(pNode->hFatherPart);
		if(pFatherPart==NULL||pFatherPart->IsBodyObj() || pFatherPart->Layer(1)!='Z' || pFatherPart->Layer(2)!='1')		//�ڵ㸸�˼������Ȳ�����
			continue;
		//2����ȡ��ڵ����ӵ���Vб��
		LINEPARTSET linePartSet; 
		CLDSPart* pPart;
		for(pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
		{
			if(!pPart->IsLinePart())
				continue;
			CLDSLinePart* pLinePart=(CLDSLinePart*)pPart;
			if(pLinePart->IsBodyObj() || pLinePart->Layer(1)!='X')		//�˼������Ȳ�б��
				continue;
			if(pLinePart->pStart!=pNode && pLinePart->pEnd!=pNode)			//б����ڵ�����
				continue;
			linePartSet.append(pLinePart);
		}
		if(linePartSet.GetNodeNum()!=2)
			continue;
		//3����ȡ��Vб�������������Ľڵ�
		CLDSNode* pTopVNode=NULL;
		CLDSLinePart *pLeftRod=NULL,*pRightRod=NULL;
		ARRAY_LIST<CLDSNodePtr> nodeSet;
		nodeSet.SetSize(0,2);
		BOOL bStart=FALSE;

		ucs.axis_y.Set();
		for(CLDSLinePart* pLine=linePartSet.GetFirst();pLine;pLine=linePartSet.GetNext())
		{
			f3dPoint v;
			if(pLine->pStart==pNode)
			{
				bStart=TRUE;
				v=pLine->End()-pLine->Start();
			}
			else
			{
				bStart=FALSE;
				v=pLine->Start()-pLine->End();
			}
			normalize(v);
			if(ucs.axis_y.IsZero())
			{
				ucs.axis_y=v;
				pLeftRod=pLine;
			}
			else
			{
				ucs.axis_z=ucs.axis_y^v;
				if(ucs.axis_z.z<0)
				{
					pRightRod=pLine;
					ucs.axis_z*=-1;
				}
				else
				{
					pRightRod=pLeftRod;
					pLeftRod=pLine;
				}
				ucs.axis_y+=v;
			}
			CLDSNode* pVNode=FindExtendNode(pLine,bStart);
			if(pVNode)
				nodeSet.append(pVNode);
			if(pTopVNode==NULL)
				pTopVNode=pVNode;
		}
		normalize(ucs.axis_y);
		ucs.axis_x=ucs.axis_y^ucs.axis_z;
		normalize(ucs.axis_x);
		if(nodeSet.GetSize()!=2)
			continue;
		//4�����������ڵ���ȡV��չ����ͼ�Ĺ�������
		f3dPoint jg_vec;
		DYN_ARRAY<f3dPoint> pt_arr(3);
		pt_arr[0]=pNode->xOriginalPos;
		for(int i=0;i<nodeSet.GetSize();i++)
			pt_arr[i+1] = nodeSet[i]->xOriginalPos;
		ucs.origin = pNode->xOriginalPos;
		POLYGON selRgn;
		if(!selRgn.CreatePolygonRgn(pt_arr,3))
			continue;
		int pushed=Node.push_stack();
		BOOL bTrue=InitDisplaySetByPolyRgn(selRgn,pNode);
		Node.pop_stack(pushed);
		if(bTrue==FALSE)
			continue;
		//5������˾��ͼV��չ����ͼ��������ʽ:'LV'+'�����ڵ���'(��������ͼΪ��׼��V�����Ȳ�Vб��չ��)
		CXhChar50 view_name,folder_name,layer;
		sprintf(folder_name,"A%s",&pNode->iSeg.ToString());
		strcpy(layer,"LPG");			//������ͼĬ��ͼ��
		sprintf(view_name,"LVS0X%X",pNode->handle);
		CDisplayView *pViewLV=AppendUdfView(ucs,view_name,folder_name,layer);
		CObjNoGroup NoGroup;
		NoGroup.InitMaxNo(15);
		int idle_no=NoGroup.EnumIdleNo();
		pViewLV->listAssist.Empty();
		CLDSLinePart* pRod;
		CLDSLineAngle* pLeftAngle=pLeftRod->GetClassTypeId()==CLS_LINEANGLE?(CLDSLineAngle*)pLeftRod:NULL;
		CLDSLineAngle* pRightAngle=pRightRod->GetClassTypeId()==CLS_LINEANGLE?(CLDSLineAngle*)pRightRod:NULL;
		CDrawingAssistant *pLeftSpread=NULL,*pRightSpread=NULL,*pLeftBendLine=NULL,*pRightBendLine=NULL;
		CPtInLineCheck left(pLeftRod->pStart->Position(false),pLeftRod->pEnd->Position(false));
		CPtInLineCheck right(pRightRod->pStart->Position(false),pRightRod->pEnd->Position(false));
		for(pPart=CLDSPart::_console->EnumFirstDispPart();pPart;pPart=CLDSPart::_console->EnumNextDispPart())
		{
			if(!pPart->IsLinePart())
				continue;
			pRod=(CLDSLinePart*)pPart;
			if(pRod->pStart==NULL||pRod->pEnd==NULL)
				continue;
			bool onLine=false;
			int startOnLeft1Right2=0,endOnLeft1Right2=0;
			left.CheckPointEx(pRod->pStart->Position(false),&onLine);
			if(onLine)
				startOnLeft1Right2=1;
			right.CheckPointEx(pRod->pStart->Position(false),&onLine);
			if(onLine)
				startOnLeft1Right2|=2;
			left.CheckPointEx(pRod->pEnd->Position(false),&onLine);
			if(onLine)
				endOnLeft1Right2=1;
			right.CheckPointEx(pRod->pEnd->Position(false),&onLine);
			if(onLine)
				endOnLeft1Right2|=2;
			if(startOnLeft1Right2>0&&endOnLeft1Right2>0&&(startOnLeft1Right2&endOnLeft1Right2)>0)
				continue;	//��б�ĸ˼�
			if(pLeftAngle&&pRod->GetClassTypeId()==CLS_LINEANGLE&&pLeftSpread==NULL&&(startOnLeft1Right2&0x01||endOnLeft1Right2&0x01))
			{
				pLeftSpread=pViewLV->listAssist.append();
				pLeftSpread->m_nStyle=0;		//չ����
				pLeftSpread->iNo=NoGroup.EnumIdleNo();
				NoGroup.SetNoState(pLeftSpread->iNo);
#ifdef AFX_TARG_ENU_ENGLISH
				strncpy(pLeftSpread->name,"L-Angle",9);
#else
				strncpy(pLeftSpread->name,"��б����",9);
#endif
				pLeftSpread->m_xFaceNorm.norm_style=1;
				pLeftSpread->m_xFaceNorm.hVicePart=pLeftAngle->handle;
				pLeftSpread->m_xFaceNorm.direction=0;
				CLDSLineAngle* pAngle=(CLDSLineAngle*)pRod;
				CMultiOffsetPos *pDesPos=&pAngle->desStartPos;
				if(!(startOnLeft1Right2&0x01)&&(endOnLeft1Right2&0x01))
					pDesPos=&pAngle->desEndPos;	//ʼ�˲�����࣬�ն���
				if(pDesPos->IsInDatumJgWingX())
				{
					pLeftSpread->m_xFaceNorm.vector=pLeftAngle->get_norm_x_wing();
					pLeftSpread->m_xFaceNorm.norm_wing=0;
				}
				else //if(pAngle->desStartPos.IsInDatumJgWingY())
				{
					pLeftSpread->m_xFaceNorm.vector=pLeftAngle->get_norm_y_wing();
					pLeftSpread->m_xFaceNorm.norm_wing=1;
				}
				//if(pAngle->desStartPos.huoqu_dist
				pLeftSpread->m_xLineStart.datum_pos_style=pLeftSpread->m_xLineEnd.datum_pos_style=1;
				if(pDesPos->huoqu_jg_h>0x20)
					pLeftSpread->m_xLineStart.des_para.RODEND.hRod=pLeftSpread->m_xLineEnd.des_para.RODEND.hRod=pDesPos->huoqu_jg_h;
				else
					pLeftSpread->m_xLineStart.des_para.RODEND.hRod=pLeftSpread->m_xLineEnd.des_para.RODEND.hRod=pLeftAngle->handle;
				pLeftSpread->m_xLineStart.des_para.RODEND.offset_wing=pLeftSpread->m_xLineEnd.des_para.RODEND.offset_wing=pLeftSpread->m_xFaceNorm.norm_wing;
				pLeftSpread->m_xLineStart.des_para.RODEND.wing_offset_dist=pLeftSpread->m_xLineEnd.des_para.RODEND.wing_offset_dist=pLeftAngle->GetWidth()+pDesPos->huoqu_dist;
				pLeftSpread->m_xLineStart.des_para.RODEND.wing_offset_style=pLeftSpread->m_xLineEnd.des_para.RODEND.wing_offset_style=4;
				pLeftSpread->m_xLineStart.des_para.RODEND.direction=0;
				pLeftSpread->m_xLineEnd.des_para.RODEND.direction=1;
				if(pDesPos->datum_offset_dist!=0)
				{
					_snprintf(pLeftSpread->m_xLineStart.des_para.RODEND.norm_offset,50,"%g",pDesPos->datum_offset_dist);
					strncpy(pLeftSpread->m_xLineEnd.des_para.RODEND.norm_offset,pLeftSpread->m_xLineStart.des_para.RODEND.norm_offset,50);
				}
				//�������
				pLeftBendLine=pViewLV->listAssist.append();
				pLeftBendLine->m_nStyle=2;		//����������
#ifdef AFX_TARG_ENU_ENGLISH
				strncpy(pLeftBendLine->name,"L-Bend",9);
#else
				strncpy(pLeftBendLine->name,"�������",9);
#endif
				pLeftBendLine->iNo=NoGroup.EnumIdleNo();
				NoGroup.SetNoState(pLeftBendLine->iNo);
				pLeftBendLine->m_xLineStart=pLeftSpread->m_xLineStart;
				pLeftBendLine->m_xLineEnd=pLeftSpread->m_xLineEnd;
			}
			if(pRightAngle&&pRod->GetClassTypeId()==CLS_LINEANGLE&&pRightSpread==NULL&&(startOnLeft1Right2&0x02||endOnLeft1Right2&0x02))
			{
				pRightSpread=pViewLV->listAssist.append();
				pRightSpread->m_nStyle=0;		//չ����
				pRightSpread->iNo=NoGroup.EnumIdleNo();
				NoGroup.SetNoState(pRightSpread->iNo);
#ifdef AFX_TARG_ENU_ENGLISH
				strncpy(pRightSpread->name,"R-Angle",9);
#else
				strncpy(pRightSpread->name,"��б����",9);
#endif
				pRightSpread->m_xFaceNorm.norm_style=1;
				pRightSpread->m_xFaceNorm.hVicePart=pRightAngle->handle;
				pRightSpread->m_xFaceNorm.direction=0;
				CLDSLineAngle* pAngle=(CLDSLineAngle*)pRod;
				CMultiOffsetPos *pDesPos=&pAngle->desStartPos;
				if(!(startOnLeft1Right2&0x02)&&(endOnLeft1Right2&0x02))
					pDesPos=&pAngle->desEndPos;
				if(pDesPos->IsInDatumJgWingX())
				{
					pRightSpread->m_xFaceNorm.vector=pRightAngle->get_norm_x_wing();
					pRightSpread->m_xFaceNorm.norm_wing=0;
				}
				else //if(pAngle->desStartPos.IsInDatumJgWingY())
				{
					pRightSpread->m_xFaceNorm.vector=pRightAngle->get_norm_y_wing();
					pRightSpread->m_xFaceNorm.norm_wing=1;
				}
				//if(pAngle->desStartPos.huoqu_dist
				pRightSpread->m_xLineStart.datum_pos_style=pRightSpread->m_xLineEnd.datum_pos_style=1;
				if(pDesPos->huoqu_jg_h>0x20)
					pRightSpread->m_xLineStart.des_para.RODEND.hRod=pRightSpread->m_xLineEnd.des_para.RODEND.hRod=pDesPos->huoqu_jg_h;
				else
					pRightSpread->m_xLineStart.des_para.RODEND.hRod=pRightSpread->m_xLineEnd.des_para.RODEND.hRod=pRightAngle->handle;
				pRightSpread->m_xLineStart.des_para.RODEND.offset_wing=pRightSpread->m_xLineEnd.des_para.RODEND.offset_wing=pRightSpread->m_xFaceNorm.norm_wing;
				pRightSpread->m_xLineStart.des_para.RODEND.wing_offset_dist=pRightSpread->m_xLineEnd.des_para.RODEND.wing_offset_dist=pRightAngle->GetWidth()+pDesPos->huoqu_dist;
				pRightSpread->m_xLineStart.des_para.RODEND.wing_offset_style=pRightSpread->m_xLineEnd.des_para.RODEND.wing_offset_style=4;
				pRightSpread->m_xLineStart.des_para.RODEND.direction=0;
				pRightSpread->m_xLineEnd.des_para.RODEND.direction=1;
				if(pDesPos->datum_offset_dist!=0)
				{
					_snprintf(pRightSpread->m_xLineStart.des_para.RODEND.norm_offset,50,"%g",pDesPos->datum_offset_dist);
					strncpy(pRightSpread->m_xLineEnd.des_para.RODEND.norm_offset,pRightSpread->m_xLineStart.des_para.RODEND.norm_offset,50);
				}
				//�һ�����
				pRightBendLine=pViewLV->listAssist.append();
				pRightBendLine->m_nStyle=2;		//����������
#ifdef AFX_TARG_ENU_ENGLISH
				strncpy(pRightBendLine->name,"R-Bend",9);
#else
				strncpy(pRightBendLine->name,"�һ�����",9);
#endif
				pRightBendLine->iNo=NoGroup.EnumIdleNo();
				NoGroup.SetNoState(pRightBendLine->iNo);
				pRightBendLine->m_xLineStart=pRightSpread->m_xLineStart;
				pRightBendLine->m_xLineEnd=pRightSpread->m_xLineEnd;
			}
		}
		/*CDrawingAssistant *pAssistant=pViewLV->listAssist.append();
		//��ʼ��չ������Ϣ����ͼ�����Ĺ�������Ϣ
		pAssistant->m_iSeg=pNode->iSeg;
		pAssistant->m_xLineStart.datum_pos_style=8;				//��ڵ�ͬΪ
		pAssistant->m_xLineStart.des_para.hNode=pNode->handle;
		pAssistant->m_xFaceNorm.norm_style=2;					//���˼������淨��
		pAssistant->m_xFaceNorm.hVicePart=linePartSet[0]->handle;	//��׼�˼�
		pAssistant->m_xFaceNorm.hCrossPart=linePartSet[1]->handle;			//���潻��˼�
		pAssistant->m_xFaceNorm.nearVector.Set(0,0,1);
		for(CViewPart *pItem=pViewLV->GetFirstViewPart();pItem;pItem=pViewLV->GetNextViewPart())
		{
			if(pItem->idClassType==CLS_NODE)
				continue;
			if(pItem->handle==hFrontPart)
				continue;
			pItem->iResidePlaneNo=pAssistant->iNo;	//��ͼ����������
		}*/
		for(CLDSPart *pDispPart=CLDSPart::_console->EnumFirstDispPart();pDispPart;pDispPart=CLDSPart::_console->EnumNextDispPart())
		{
			//if(pDispPart->handle==hFrontPart)
			//	continue;		//��������й���
			CViewPart *pViewPart = pViewLV->AppendViewPart(pDispPart->handle);
			pViewPart->idClassType=pDispPart->GetClassTypeId();
		}
		for(CLDSNode *pDispNode=CLDSPart::_console->EnumFirstDispNode();pDispNode;pDispNode=CLDSPart::_console->EnumNextDispNode())
		{
			//if(pDispNode->handle==pNode->handle)
			//	continue;		//��������нڵ�
			CViewPart *pViewPart = pViewLV->AppendViewPart(pDispNode->handle);
			pViewPart->idClassType=CLS_NODE;
		}
	}
}
struct NODE_POS : public ICompareClass{
	int index;
	CLDSNode* pNode;
	double lenFactor;	//(current-lineStart)/(lineEnd-lineStart)
	NODE_POS(CLDSNode* _pNode=NULL,double _lenFactor=0)
	{
		index=0;
		pNode=_pNode;
		lenFactor=_lenFactor;
	}
	virtual int Compare(const ICompareClass *pCompareObj) const
	{
		NODE_POS* pCompareNode=(NODE_POS*)pCompareObj;
		if(lenFactor>pCompareNode->lenFactor)
			return 1;
		else if(lenFactor<pCompareNode->lenFactor)
			return -1;
		else
			return 0;
	}
};
bool CTower::UpgradeToCtrlNode(CLDSNode* pNode)
{
	if(pNode==NULL)
		return false;
	if(pNode->m_cPosCalType==0)
		return true;
	if(pNode->m_cPosCalType==4||pNode->m_cPosCalType>5)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("Didn't support this kind of type {%d} node change to control node(without coordinate relation)",pNode->m_cPosCalType);
#else
		logerr.Log("��֧�ָ�������{%d}�Ľڵ�����ƽڵ�(������������)��ת��",pNode->m_cPosCalType);
#endif
		return false;	//��֧�������������������ȷֵ�
	}
	BOOL pushed=Node.push_stack();
	CLDSNode* pAttachStart=Node.FromHandle(pNode->arrRelationNode[0]);
	CLDSNode* pAttachEnd=Node.FromHandle(pNode->arrRelationNode[1]);
	Node.pop_stack(pushed);
	if(pAttachStart==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("Didn't find start node 0X%X coordinate related",pNode->arrRelationNode[0]);
#else
		logerr.Log("��������ʼ�˽ڵ�0X%Xδ�ҵ�",pNode->arrRelationNode[0]);
#endif
		return false;
	}
	if(pAttachEnd==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("Didn't fine end node 0X%X coordinate related",pNode->arrRelationNode[1]);
#else
		logerr.Log("��������ʼ�˽ڵ�0X%Xδ�ҵ�",pNode->arrRelationNode[1]);
#endif
		return false;
	}
	GEPOINT splitPos=pNode->Position(false);
	CPtInLineCheck lineCheck(pAttachStart->Position(false),pAttachEnd->Position(false));
	pushed=Node.push_stack();
	ARRAY_LIST<NODE_POS> arrNodePos(0,4);
	for(CLDSNode* pCheckNode=Node.GetFirst();pCheckNode;pCheckNode=Node.GetNext())
	{
		bool onLine=false;
		double rslt=lineCheck.CheckPointEx(pCheckNode->Position(false),&onLine);
		if(pCheckNode==pNode&&!onLine)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Current node 0X%X isn't on connecting line of coordinate relative node,please change after check",pNode->handle);
#else
			logerr.Log("��ǰ�ڵ�0X%X�������������ڵ�������ϣ����ʵ����ת��",pNode->handle);
#endif
			Node.pop_stack(pushed);
			return false;
		}
		if(!onLine)	//�㲻��ֱ����
			continue;
		arrNodePos.append(NODE_POS(pCheckNode,rslt));
	}
	Node.pop_stack(pushed);
	if(arrNodePos.GetSize()>3)
	{
		CHeapSort<NODE_POS>::HeapSortClassic(arrNodePos.m_pData,arrNodePos.GetSize());
		int i,split_index=0;
		CHashList<NODE_POS> hashOnLineNodes;
		for(i=0;i<arrNodePos.GetSize();i++)
		{
			arrNodePos[i].index=i;
			if(arrNodePos[i].pNode==pNode)
				split_index=i;
			hashOnLineNodes.SetValue(arrNodePos[i].pNode->handle,arrNodePos[i]);
		}
		NODE_POS* pPosNode;
		for(i=0;i<arrNodePos.GetSize();i++)
		{
			if(i==split_index)
				continue;	//��ǰ�ڵ�
			if(arrNodePos[i].pNode->m_cPosCalType==0)
				continue;	//�������κ������ڵ�
			else if(arrNodePos[i].pNode->m_cPosCalType<=3)
			{	//ָ����������ڵ�
				pPosNode=hashOnLineNodes.GetValue(arrNodePos[i].pNode->arrRelationNode[0]);
				if(pPosNode!=NULL && (i-split_index)*(pPosNode->index-split_index)<0)
				{
					arrNodePos[i].pNode->arrRelationNode[0]=pNode->handle;
					arrNodePos[i].pNode->SetModified();
				}
				pPosNode=hashOnLineNodes.GetValue(arrNodePos[i].pNode->arrRelationNode[1]);
				if(pPosNode!=NULL && (i-split_index)*(pPosNode->index-split_index)<0)
				{
					arrNodePos[i].pNode->arrRelationNode[1]=pNode->handle;
					arrNodePos[i].pNode->SetModified();
				}
			}
			else if(arrNodePos[i].pNode->m_cPosCalType==5)
			{	//�����ȷֵ�
				CLDSNode* pAttachNodeStart=Node.FromHandle(arrNodePos[i].pNode->arrRelationNode[0]);
				CLDSNode* pAttachNodeEnd  =Node.FromHandle(arrNodePos[i].pNode->arrRelationNode[1]);
				NODE_POS* pPosNodeStart	  =hashOnLineNodes.GetValue(arrNodePos[i].pNode->arrRelationNode[0]);
				NODE_POS* pPosNodeEnd	  =hashOnLineNodes.GetValue(arrNodePos[i].pNode->arrRelationNode[1]);
				if(pAttachNodeStart==NULL||pAttachNodeEnd==NULL)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.Log("Lack the relative node of  0X%X node's coordinate",arrNodePos[i].pNode->handle);
#else
					logerr.Log("0X%X�ڵ����������ڵ�ȱʧ",arrNodePos[i].pNode->handle);
#endif
					continue;
				}
				double len_whole=DISTANCE(pAttachNodeEnd->Position(false),pAttachNodeStart->Position(false));
				double len_sect =DISTANCE(pNode->Position(false),pAttachNodeStart->Position(false));
				if(pPosNodeStart!=NULL && (i-split_index)*(pPosNodeStart->index-split_index)<0)
				{
					arrNodePos[i].pNode->arrRelationNode[0]=pNode->handle;
					if(len_whole-len_sect>EPS)
					{
						double len_offset=arrNodePos[i].pNode->attach_scale*len_whole;
						arrNodePos[i].pNode->attach_scale=(len_offset-len_sect)/(len_whole-len_sect);
						arrNodePos[i].pNode->SetModified();
					}
				}
				if(pPosNodeEnd!=NULL && (i-split_index)*(pPosNodeEnd->index-split_index)<0)
				{
					arrNodePos[i].pNode->arrRelationNode[0]=pNode->handle;
					if(len_sect>EPS)
					{
						arrNodePos[i].pNode->attach_scale*=len_whole/len_sect;
						arrNodePos[i].pNode->SetModified();
					}
				}
			}
		}
	}
	pNode->m_cPosCalType=0;	//ת��Ϊ�������κ������ڵ�����
	return true;
}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
BOOL CTower::DelTubeEndLj(THANDLE handle)	//ɾ���ֹܶ����� ��Ҫ����ɾ��ʮ�ֲ��
{
	//ɾ��ʮ�ֲ������ʱ���𸽼Ӱ�һ��ɾ��
	double X=0;
	CLDSParamPlate *pParamPlate=(CLDSParamPlate*)Parts.FromHandle(handle,CLS_PARAMPLATE);
	if(pParamPlate==NULL)
		return FALSE;
	if(pParamPlate->m_iParamType==TYPE_XEND||pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND)
	{	//ɾ�����ʱ��ͬʱɾ�������˨
		for(CLsRef* pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			if(pBolt)
				DelBoltAndRelativeBoltRef(pBolt->handle);
		}
	}
	if(pParamPlate->m_iParamType==TYPE_XEND)
	{	//ɾ��ʮ�ֲ���ϵĸ��Ӱ�
		for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
		{
			CLDSPart *pAttachPart=Parts.FromHandle(pItem->h);
			if(pAttachPart)
			{
				if(CLDSObject::_console!=NULL)
					CLDSObject::_console->FireDeleteObject(pAttachPart,TRUE);
				//DispPartSet.DeleteNode(pAttachPart->handle);
				Parts.DeleteNode(pAttachPart->handle);
			}
		}
	}
	else if(pParamPlate->IsFL())
	{	//ɾ���������Ӱ壬����������������ڴ����⴦��
		for(ATTACHPART_ITEM* pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
		{
			CLDSPart* pAttachPart=Parts.FromHandle(pItem->h);
			if(pAttachPart&&pAttachPart->GetClassTypeId()==CLS_PARAMPLATE)
			{	//ֻɾ���Ӱ��еĲ��������
				if(CLDSObject::_console!=NULL)
					CLDSObject::_console->FireDeleteObject(pAttachPart,TRUE);
				Parts.DeleteNode(pAttachPart->handle);
			}
		}
	}
	if(CLDSObject::_console!=NULL)
		CLDSObject::_console->FireDeleteObject(pParamPlate,TRUE);
	//DispPartSet.DeleteNode(pParamPlate->handle);
	Parts.DeleteNode(pParamPlate->handle);
	return TRUE;
}
int CTower::RetrievePolyPartList(POLYPARTLIST &list)
{
	return CTower::RetrievePolyPartList(this,list);
}
static void InitPolyLinePartHuoqu(CLDSLinePart* pPreLinePart,CLDSLinePart* pNextLinePart)
{
	if(pPreLinePart==NULL || pNextLinePart==NULL)
		return;
	if(pPreLinePart->pStart&&pPreLinePart->pStart==pNextLinePart->pStart)
	{
		pPreLinePart->bHuoQuStart=TRUE;
		pNextLinePart->bHuoQuStart=TRUE;
	}
	else if(pPreLinePart->pEnd&&pPreLinePart->pEnd==pNextLinePart->pStart)
	{
		pPreLinePart->bHuoQuEnd=TRUE;
		pNextLinePart->bHuoQuStart=TRUE;
	}
	else if(pPreLinePart->pStart&&pPreLinePart->pStart==pNextLinePart->pEnd)
	{
		pPreLinePart->bHuoQuStart=TRUE;
		pNextLinePart->bHuoQuEnd=TRUE;
	}
	else if(pPreLinePart->pEnd&&pPreLinePart->pEnd==pNextLinePart->pEnd)
	{
		pPreLinePart->bHuoQuEnd=TRUE;
		pNextLinePart->bHuoQuEnd=TRUE;
	}
}
int CTower::RetrievePolyPartList(IModel *pModel,POLYPARTLIST &list)
{
	if(pModel==NULL)
		return 0;
	int hits=0;
	CLDSPart* pPart=NULL;
	list.Empty();
	list.SetBelongModel(pModel);
	for(pPart=pModel->EnumRodFirst();pPart;pPart=pModel->EnumRodNext())
	{
		((CLDSLinePart*)pPart)->bHuoQuStart=FALSE;
		((CLDSLinePart*)pPart)->bHuoQuEnd=FALSE;
	}
	CLDSPolyPart *pPolyPart;
	char sError[100]="",prev_spec[100]="",next_spec[100]="";
	try{
		for(pPart=pModel->EnumPartFirst(CLS_POLYJOINT);pPart;pPart=pModel->EnumPartNext(CLS_POLYJOINT))
		{
			CLDSPolyJoint *pPolyJoint=(CLDSPolyJoint*)pPart;
			if(pPolyJoint->pPrevLinePart==NULL||pPolyJoint->pNextLinePart==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"0X%X bending point invalid,previous or latter part is null!\n",pPolyJoint->handle);
#else
				sprintf(sError,"0X%X����㲻�Ϸ���ǰһ���������һ������ΪNULL!\n",pPolyJoint->handle);
#endif
				throw sError;
			}
			pPolyJoint->pPrevLinePart->GetSpecification(prev_spec);
			pPolyJoint->pNextLinePart->GetSpecification(next_spec);
			if(pPolyJoint->pPrevLinePart->GetClassTypeId()!=pPolyJoint->pNextLinePart->GetClassTypeId()||stricmp(prev_spec,next_spec)!=0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"The spec of 0X%X and 0X%X part which 0X%X bending point connects is different,can't create vaild bending part!\n",
					pPolyJoint->pPrevLinePart->handle,pPolyJoint->pNextLinePart->handle,pPolyJoint->handle);
#else
				sprintf(sError,"0X%X�����������0X%X������0X%X����������Ͳ�һ�£��޷��γɺϷ������乹��!\n",
					pPolyJoint->handle,pPolyJoint->pPrevLinePart->handle,pPolyJoint->pNextLinePart->handle);
#endif
				throw sError;
			}
			for(pPolyPart=list.GetFirst();pPolyPart;pPolyPart=list.GetNext())
			{
				CLDSLinePart *pFirstLinePart=pPolyPart->segset.GetFirst();
				CLDSLinePart *pTailLinePart=pPolyPart->segset.GetTail();
				if(pTailLinePart&&pTailLinePart==pPolyJoint->pPrevLinePart)
				{
					InitPolyLinePartHuoqu(pPolyJoint->pPrevLinePart,pPolyJoint->pNextLinePart);
					pPolyPart->segset.append(pPolyJoint->pNextLinePart);
					break;
				}
				else if(pFirstLinePart&&pFirstLinePart==pPolyJoint->pNextLinePart)
				{
					InitPolyLinePartHuoqu(pPolyJoint->pNextLinePart,pPolyJoint->pPrevLinePart);
					pPolyPart->segset.insert(pPolyJoint->pPrevLinePart,0);
					break;
				}
			}
			if(pPolyPart==NULL)
			{
				pPolyPart=list.append();
				InitPolyLinePartHuoqu(pPolyJoint->pPrevLinePart,pPolyJoint->pNextLinePart);
				pPolyPart->segset.append(pPolyJoint->pPrevLinePart);
				pPolyPart->segset.append(pPolyJoint->pNextLinePart);
				hits++;
			}
		}
		//��ֵ������Ϲ����Ĳ�����Ч��������
		for(pPolyPart=list.GetFirst();pPolyPart;pPolyPart=list.GetNext())
		{
			CLDSLinePart *pFirstLinePart=pPolyPart->segset.GetFirst();
			pPolyPart->handle=pFirstLinePart->handle;
			pPolyPart->m_hBlock=pFirstLinePart->m_hBlock;
			strcpy(pPolyPart->layer(),pFirstLinePart->layer());	//ͼ��
			pPolyPart->ucs=pFirstLinePart->ucs;	//����ϵ
			pPolyPart->m_uStatMatCoef=pFirstLinePart->m_uStatMatCoef;
			pPolyPart->iSeg=pFirstLinePart->iSeg;
			pPolyPart->cfgword=pFirstLinePart->cfgword;
			pPolyPart->dwPermission=pFirstLinePart->dwPermission;
			pPolyPart->cMaterial=pFirstLinePart->cMaterial;
			pPolyPart->SetPartNo(pFirstLinePart->GetPartNo());
			for(pPart=pPolyPart->segset.GetNext();pPart;pPart=pPolyPart->segset.GetNext())
			{
				pPolyPart->cfgword&=pPart->UnifiedCfgword();
				pPolyPart->dwPermission&=pPart->dwPermission;
			}
		}
	}
	catch(char* sError)
	{
		//AfxMessageBox(sError);
#if defined(__LDS_FILE_)||defined(__LDS_)
		MessageBox(NULL,sError,"LDS",MB_OK);
#elif defined(__LMA_FILE_)||defined(__LMA_)
		MessageBox(NULL,sError,"LMA",MB_OK);
#elif defined(__TMA_FILE_)||defined(__TMA_)
		MessageBox(NULL,sError,"TMA",MB_OK);
#else
		strcpy(sError,"");
#endif
	}
	return hits;
}

int CTower::GetSegNoHashList(CHashList<SEGI> &segNoHashList,CLDSModule *pModule/*=NULL*/,BOOL bEqualCfg/*=FALSE*/)
{
	segNoHashList.Empty();
	if(pModule)
	{	//ָ��ģ�� wht 10-11-04
		for(CLDSNode *pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
		{
			if(!pModule->IsSonPart(pNode)||pNode->iSeg.iSeg<0)
				continue;
			if(segNoHashList.GetValue(pNode->iSeg)==NULL)
				segNoHashList.SetValue(pNode->iSeg,pNode->iSeg);
		}
		for(CLDSPart *pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
		{
			if(pPart->m_bVirtualPart)
				continue;
			else if(pPart->GetClassTypeId()==CLS_POLYJOINT)
				continue;
			if(!pModule->IsSonPart(pPart)||pPart->iSeg.iSeg<0)
				continue;
			if(bEqualCfg&&!pPart->cfgword.IsHasBodyNoOnly(pModule->GetBodyNo()))
				continue;	//��������ĺ���ȵĹ���
			if(segNoHashList.GetValue(pPart->iSeg)==NULL)
				segNoHashList.SetValue(pPart->iSeg,pPart->iSeg);
		}
		for(CBlockReference *pBlockRef=BlockRef.GetFirst();pBlockRef;pBlockRef=BlockRef.GetNext())
		{
			if(!pModule->IsSonPart(pBlockRef)||pBlockRef->iSeg.iSeg<0)
				continue;
			if(segNoHashList.GetValue(pBlockRef->iSeg)==NULL)
				segNoHashList.SetValue(pBlockRef->iSeg,pBlockRef->iSeg);
		}
	}
	else
	{	//ȫ��
		for(CLDSNode *pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
		{
			if(pNode->iSeg==0)
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("0X%X node's segment number is 0!",pNode->handle);
#else
				logerr.Log("0X%X�ڵ�Ķκ�Ϊ0!",pNode->handle);
#endif
			else if(segNoHashList.GetValue(pNode->iSeg)==NULL)
				segNoHashList.SetValue(pNode->iSeg,pNode->iSeg);
		}
		bool hasZeroSegI=false;
		for(CLDSPart *pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
		{
			if(pPart->m_bVirtualPart)
				continue;
			else if(pPart->GetClassTypeId()==CLS_POLYJOINT)
				continue;
			if((!hasZeroSegI||pPart->iSeg!=0)&&segNoHashList.GetValue(pPart->iSeg)==NULL)
				segNoHashList.SetValue(pPart->iSeg,pPart->iSeg);
			if(pPart->iSeg==0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("0X%X %s segment number is 0!",pPart->handle,pPart->GetPartTypeName());
#else
				logerr.Log("0X%X%s�Ķκ�Ϊ0!",pPart->handle,pPart->GetPartTypeName());
#endif
				hasZeroSegI=true;
			}
		}
		for(CBlockReference *pBlockRef=BlockRef.GetFirst();pBlockRef;pBlockRef=BlockRef.GetNext())
		{
			if((!hasZeroSegI||pBlockRef->iSeg!=0)&&segNoHashList.GetValue(pBlockRef->iSeg)==NULL)
				segNoHashList.SetValue(pBlockRef->iSeg,pBlockRef->iSeg);
			if(pBlockRef->iSeg==0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("The segment number of object that 0X%X  block assemble is 0!",pBlockRef->handle);
#else
				logerr.Log("0X%X����װ�����Ķκ�Ϊ0!",pBlockRef->handle);
#endif
				hasZeroSegI=true;
			}
		}
	}
	return segNoHashList.GetNodeNum();
}

int CTower::GetSegNoList(ATOM_LIST<SEGI> &segList,CLDSModule *pModule/*=NULL*/,BOOL bEqualCfg/*=FALSE*/)
{
	CHashList<SEGI> segNoHashList;
	GetSegNoHashList(segNoHashList,pModule,bEqualCfg);
	//ѡ������ֶκ�
	int n=segNoHashList.GetNodeNum();
	SEGI *seg_i_arr = new SEGI[n];
	int i=0;
	for(SEGI *pSegI=segNoHashList.GetFirst();pSegI;pSegI=segNoHashList.GetNext())
	{
		if(i>0)
		{
			int j;
			for(j=0;j<i;j++)
			{
				if(*pSegI<seg_i_arr[j])
				{
					memmove(&seg_i_arr[j+1],&seg_i_arr[j],(n-j-1)*sizeof(int));
					seg_i_arr[j]=*pSegI;
					break;
				}
			}
			if(j==i)
				seg_i_arr[i]=*pSegI;
		}
		else
			seg_i_arr[i]=*pSegI;
		i++;
	}
	segList.Empty();
	for(int i=0;i<n;i++)
		segList.append(seg_i_arr[i]);
	delete []seg_i_arr;
	return n;
}

CXhChar200 CTower::SegNoListToString(ATOM_LIST<SEGI> &segNoList)
{
	CXhChar200 sSegStr;
	SEGI startSegI=-1,endSegI=-1;
	for(SEGI *pSegI=segNoList.GetFirst();pSegI;pSegI=segNoList.GetNext())
	{
		if(startSegI==-1)
			startSegI = *pSegI;
		else if(endSegI==-1&&*pSegI==startSegI+1)
			endSegI = *pSegI;
		else if(endSegI>-1&&*pSegI==endSegI+1)
			endSegI = *pSegI;
		else 
		{
			if(sSegStr.GetLength()>0)
				sSegStr.Append(",");
			if(startSegI<endSegI)	//���������
				sSegStr.Printf("%s%s-%s",(char*)sSegStr,(char*)startSegI.ToString(),(char*)endSegI.ToString());
			else if(startSegI>-1)
				sSegStr.Printf("%s%s",(char*)sSegStr,(char*)startSegI.ToString());
			startSegI = *pSegI;
			endSegI = -1;
		}
	}
	//�����һ����ӵ��ַ�����
	if(sSegStr.GetLength()>0)
		sSegStr.Append(",");
	if(startSegI<endSegI)	//���������
		sSegStr.Printf("%s%s-%s",(char*)sSegStr,(char*)startSegI.ToString(),(char*)endSegI.ToString());
	else if(startSegI>-1)
		sSegStr.Printf("%s%s",(char*)sSegStr,(char*)startSegI.ToString());
	return sSegStr;
}

int CTower::GetSegStr(char *seg_str,CLDSModule *pModule/*=NULL*/,BOOL bEqualCfg/*=FALSE*/)
{
	ATOM_LIST<SEGI> segNoList;
	GetSegNoList(segNoList,pModule,bEqualCfg);
	CXhChar200 sSegStr=SegNoListToString(segNoList);
	if(seg_str)
		sprintf(seg_str,"%s",(char*)sSegStr);
	return sSegStr.GetLength();
};
//�����¶κ��޸�ĳ�ι�������
void CTower::ModifyPartNoByNewSegNo(SEGI oldSegI,SEGI newSegI)
{
	for(CLDSPart* pPart = Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)pPart)->m_bStdPart)
			continue;	//������׼���������ְ幹���������޶κųɷ֣�
		if(pPart->iSeg.iSeg==oldSegI.iSeg)
			pPart->ModifyPartNoSegI(newSegI);
	}
}
CLDSObject * CTower::FromPartNo(const char *sPartNo)
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	for(CLDSPart *pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		if(pPart->m_bVirtualPart)
			continue;
		else if(stricmp(sPartNo,pPart->GetPartNo())==0)
			return pPart;
	}
	for(CSupplementPart *pSuppPart=SuppPart.GetFirst();pSuppPart;pSuppPart=SuppPart.GetNext())
	{
		if(stricmp(sPartNo,pSuppPart->GetPartNo())==0)
			return pSuppPart;
	}
#endif
	return NULL;
}

void CTower::RefreshBurglarLs()
{
	//���ԭ�з�����˨
	for(CLDSBolt *pLs=(CLDSBolt*)Parts.GetFirst(CLS_BOLT);pLs;pLs=(CLDSBolt*)Parts.GetNext(CLS_BOLT))
	{
		if(pLs->m_dwFlag.IsHasFlag(CLDSBolt::ANTI_THEFT))
			pLs->SetModified();
		pLs->m_dwFlag.SetFlagDword(CLDSBolt::ANTI_THEFT,FALSE);
	}
	CLsRef *pLsRef;
	CLDSModule *pActModule=GetActiveModule();
	for(CLDSPlate *pPlank=(CLDSPlate*)Parts.GetFirst(CLS_PLATE);pPlank;pPlank=(CLDSPlate*)Parts.GetNext(CLS_PLATE))
	{
		f3dPoint pos;
		if(pPlank->GetLsCount()==1)	//����˨ʱ����˨����Ϊ�жϻ�׼
			pos=(*pPlank->GetFirstLsRef())->ucs.origin;
		else						//������������������Ϊ�жϻ�׼
			pos=pPlank->ucs.origin;
		if(pos.z<=pActModule->m_fBurglarLsEndZ&&pos.z>=pActModule->m_fBurglarLsStartZ)
		{
			for(pLsRef=pPlank->GetFirstLsRef();pLsRef;pLsRef=pPlank->GetNextLsRef())
				(*pLsRef)->m_dwFlag.SetFlagDword(CLDSBolt::ANTI_THEFT,TRUE);
		}
	}
	for(CLDSLineAngle *pJg=(CLDSLineAngle*)Parts.GetFirst(CLS_LINEANGLE);pJg;pJg=(CLDSLineAngle*)Parts.GetNext(CLS_LINEANGLE))
	{
		for(pLsRef=pJg->GetFirstLsRef();pLsRef;pLsRef=pJg->GetNextLsRef())
		{
			f3dPoint pos=(*pLsRef)->ucs.origin;
			if( pos.z<=pActModule->m_fBurglarLsEndZ&&
				pos.z>=pActModule->m_fBurglarLsStartZ)
			{
				(*pLsRef)->m_dwFlag.SetFlagDword(CLDSBolt::ANTI_THEFT,TRUE);
				(*pLsRef)->SetModified();
			}
		}
	}
}

void CTower::RefreshLockPad()
{
	//���ԭ�з��ɵ�Ȧ
	for(CLDSBolt *pLs=(CLDSBolt*)Parts.GetFirst(CLS_BOLT);pLs;pLs=(CLDSBolt*)Parts.GetNext(CLS_BOLT))
	{
		if(pLs->m_dwFlag.IsHasFlag(CLDSBolt::ANTI_LOOSE))
			pLs->SetModified();
		pLs->m_dwFlag.SetFlagDword(CLDSBolt::ANTI_LOOSE,FALSE);
	}
	CLsRef *pLsRef;
	CLDSModule *pActModule=GetActiveModule();
	for(CLDSPlate *pPlank=(CLDSPlate*)Parts.GetFirst(CLS_PLATE);pPlank;pPlank=(CLDSPlate*)Parts.GetNext(CLS_PLATE))
	{
		f3dPoint pos;
		if(pPlank->GetLsCount()==1)	//����˨ʱ����˨����Ϊ�жϻ�׼
			pos=(*pPlank->GetFirstLsRef())->ucs.origin;
		else						//������������������Ϊ�жϻ�׼
			pos=pPlank->ucs.origin;
		if(pos.z<=pActModule->m_fLockPadEndZ&&pos.z>=pActModule->m_fLockPadStartZ)
		{
			for(pLsRef=pPlank->GetFirstLsRef();pLsRef;pLsRef=pPlank->GetNextLsRef())
				(*pLsRef)->m_dwFlag.SetFlagDword(CLDSBolt::ANTI_LOOSE,TRUE);
		}
	}
	for(CLDSLineAngle *pJg=(CLDSLineAngle*)Parts.GetFirst(CLS_LINEANGLE);pJg;pJg=(CLDSLineAngle*)Parts.GetNext(CLS_LINEANGLE))
	{
		for(pLsRef=pJg->GetFirstLsRef();pLsRef;pLsRef=pJg->GetNextLsRef())
		{
			f3dPoint pos=(*pLsRef)->ucs.origin;
			if( pos.z<=pActModule->m_fLockPadEndZ&&
				pos.z>=pActModule->m_fLockPadStartZ)
			{
				(*pLsRef)->m_dwFlag.SetFlagDword(CLDSBolt::ANTI_LOOSE,TRUE);
				(*pLsRef)->SetModified();
			}
		}
	}
}

//�õ��麸����������ʱ֧�����͹��� wht 10-10-22
static CHashSet<CLDSPart*> hashLocalDeadlockLoop;
UINT CTower::AddJointTubeWeldRelations(CLDSPart* pTubePart)
{
	UINT hits=0;
	if(pTubePart==NULL||!pTubePart->IsTube())
		return 0;;
	CLDSLineTube* pDatumTube=(CLDSLineTube*)pTubePart;
	if(pDatumTube->m_tJointStart.type==1&&pDatumTube->m_tJointStart.hLinkObj>0x20&&pDatumTube->m_hPartWeldParent!=pDatumTube->m_tJointStart.hLinkObj)
	{
		SmartPartPtr pAttachPart=FromPartHandle(pDatumTube->m_tJointStart.hLinkObj);
		bool validEndJointTube=false;
		if(pAttachPart.IsHasPtr()&&pAttachPart->IsTube()&&(pAttachPart.pTube->pStart==pDatumTube->pStart||pAttachPart.pTube->pEnd==pDatumTube->pStart))
			validEndJointTube=true;
		if(validEndJointTube&&pAttachPart->m_hPartWeldParent!=pDatumTube->handle)
		{
			if(pDatumTube->handle<pAttachPart->handle)
				pAttachPart->m_hPartWeldParent=pDatumTube->handle;
			else
				pDatumTube->m_hPartWeldParent=pAttachPart->handle;
			hits++;
		}
		pAttachPart=pDatumTube->m_tJointStart.hViceLinkObj>0x20?FromPartHandle(pDatumTube->m_tJointStart.hViceLinkObj):NULL;
		//����̫ȷ��hViceLinkObj��δ��� wjh-2017.11.16
		//if(pAttachPart.IsHasPtr()&&pAttachPart->IsTube()&&(pAttachPart.pTube->pStart==pDatumTube->pStart||pAttachPart.pTube->pEnd==pDatumTube->pStart))
		//	validEndJointTube=true;
		if(pAttachPart.IsHasPtr()&&pDatumTube->m_hPartWeldParent!=pDatumTube->m_tJointStart.hViceLinkObj)
			{
			pAttachPart->m_hPartWeldParent=pDatumTube->handle;
			hits++;
		}
	}
	if(pDatumTube->m_tJointEnd.type==1&&pDatumTube->m_tJointEnd.hLinkObj>0x20&&pDatumTube->m_hPartWeldParent!=pDatumTube->m_tJointEnd.hLinkObj)
	{
		SmartPartPtr pAttachPart=FromPartHandle(pDatumTube->m_tJointEnd.hLinkObj);
		bool validEndJointTube=false;
		if(pAttachPart.IsHasPtr()&&pAttachPart->IsTube()&&(pAttachPart.pTube->pStart==pDatumTube->pEnd||pAttachPart.pTube->pEnd==pDatumTube->pEnd))
			validEndJointTube=true;
		if(validEndJointTube&&pAttachPart->m_hPartWeldParent!=pDatumTube->handle)
		{
			if(pDatumTube->handle<pAttachPart->handle)
				pAttachPart->m_hPartWeldParent=pDatumTube->handle;
			else
				pDatumTube->m_hPartWeldParent=pAttachPart->handle;
			hits++;
		}
		pAttachPart=pDatumTube->m_tJointEnd.hViceLinkObj>0x20?FromPartHandle(pDatumTube->m_tJointEnd.hViceLinkObj):NULL;
		//����̫ȷ��hViceLinkObj��δ��� wjh-2017.11.16
		//if(pAttachPart.IsHasPtr()&&pAttachPart->IsTube()&&(pAttachPart.pTube->pStart==pDatumTube->pEnd||pAttachPart.pTube->pEnd==pDatumTube->pEnd))
		//	validEndJointTube=true;
		if(pAttachPart.IsHasPtr()&&pDatumTube->m_hPartWeldParent!=pDatumTube->m_tJointEnd.hViceLinkObj)
		{
			pAttachPart->m_hPartWeldParent=pDatumTube->handle;
			hits++;
		}
	}
	return hits;
}
UINT CTower::AddJointTubeWeldRelations(CHashList<SEGI>* pFilterSegHashtable/*=NULL*/)
{
	UINT hits=0;
	CLDSPart* pRootPart;
	for(pRootPart=EnumPartFirst();pRootPart!=NULL;pRootPart=EnumPartNext())
	{
		if(!pRootPart->IsTube())
			continue;
		if(pFilterSegHashtable!=NULL&&pFilterSegHashtable->GetValue(pRootPart->iSeg)==NULL)
			continue;	//���ڵ�ǰɸѡ�κŷ�Χ��
		hits+=AddJointTubeWeldRelations(pRootPart);
	}
	return hits;
}
int CTower::GetPartGroupPartSet(long hPartGroup,PARTSET &partset,int CLASS_TYPEID/*=0*/,bool bClearLock/*=true*/,
								long* phExcludePartBranchArr/*=NULL*/,int excludeCount/*=1*/)
{
	PARTSET set;	//��ʱ��
	CLdsPartListStack stack(&Parts);
	SmartPartPtr pRootPart;
	if(bClearLock)
		hashLocalDeadlockLoop.Empty();
	pRootPart=hPartGroup>0x20?hashLocalDeadlockLoop.GetValue(hPartGroup):NULL;
	if(pRootPart.IsHasPtr())
	{
		pRootPart->Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X�������Ӹ������趨��������ѭ��",hPartGroup);
		return -1;
	}
	pRootPart=hPartGroup>0x20?FromPartHandle(hPartGroup):NULL;
	if(pRootPart.IsHasPtr())
		hashLocalDeadlockLoop.SetValue(hPartGroup,pRootPart);
	else
		return 0;
	AddJointTubeWeldRelations(pRootPart);
	if(bClearLock&&phExcludePartBranchArr==NULL&&pRootPart->m_hPartWeldParent>0x20&&pRootPart->m_hPartWeldParent<pRootPart->handle)	//���ڴ������빹������Ǹ���������� wjh-2017.10.16
		return GetPartGroupPartSet(pRootPart->m_hPartWeldParent,partset,CLASS_TYPEID,bClearLock);
	else if(pRootPart->IsTube())
	{
		CLDSLineTube* pTube=pRootPart.pTube;
		CLDSPart* pStartConnPart=(pTube->m_tJointStart.type==0&&pTube->m_tJointStart.hLinkObj>0)?FromPartHandle(pTube->m_tJointStart.hLinkObj):NULL;
		CLDSPart* pEndConnPart=(pTube->m_tJointEnd.type==0&&pTube->m_tJointEnd.hLinkObj>0)?FromPartHandle(pTube->m_tJointEnd.hLinkObj):NULL;
		if(pStartConnPart!=NULL&&pTube->m_tJointStart.hLinkObj!=pStartConnPart->m_hPartWeldParent)
		{	//���ڴ���ֹ�ƽ���и�ʱ���溸�ӹ�����ֹ�ȱ�ٺ��ӹ�����ϵ�����ʵ���к��ӹ�ϵ��
			CLDSPart *pUpLinkPart=FromPartHandle(pTube->m_tJointStart.hLinkObj);
			if(pUpLinkPart&&pUpLinkPart->IsTube())
			{	//���ݷɽ���������һ�������ļ������ڸֹ�ƽ���и�ʱ�����Ӹ˼��Ǵ���ĵ����麸ͼ��ȡ���� wjh-2017.11.16
				CLDSLineTube* pLinkTube=(CLDSLineTube*)pUpLinkPart;
				if(pLinkTube->pStart!=pTube->pStart&&pLinkTube->pEnd!=pTube->pStart)
					pUpLinkPart=NULL;
			}
			CLDSPart *pBranchRootWeldPart=pUpLinkPart;
			while(pUpLinkPart!=NULL)
			{
				pUpLinkPart=FromPartHandle(pBranchRootWeldPart->m_hPartWeldParent);
				if(pUpLinkPart!=NULL)
					pBranchRootWeldPart=pUpLinkPart;
			}
			if(pBranchRootWeldPart)
			{
				PARTSET tmpSet;
				partset.append(pBranchRootWeldPart);
				if(GetPartGroupPartSet(pBranchRootWeldPart->handle,tmpSet,CLASS_TYPEID,false,&pTube->handle)>0)
				{
					for(CLDSPart *pTmPart=tmpSet.GetFirst();pTmPart;pTmPart=tmpSet.GetNext())
						partset.append(pTmPart);
				}
			}
		}
		if(pEndConnPart!=NULL&&pTube->m_tJointEnd.hLinkObj!=pEndConnPart->m_hPartWeldParent)
		{	//���ڴ���ֹ�ƽ���и�ʱ���溸�ӹ�����ֹ�ȱ�ٺ��ӹ�����ϵ�����ʵ���к��ӹ�ϵ��
			CLDSPart *pUpLinkPart=FromPartHandle(pTube->m_tJointEnd.hLinkObj);
			if(pUpLinkPart&&pUpLinkPart->IsTube())
			{	//���ݷɽ���������һ�������ļ������ڸֹ�ƽ���и�ʱ�����Ӹ˼��Ǵ���ĵ����麸ͼ��ȡ���� wjh-2017.11.16
				CLDSLineTube* pLinkTube=(CLDSLineTube*)pUpLinkPart;
				if(pLinkTube->pStart!=pTube->pEnd&&pLinkTube->pEnd!=pTube->pEnd)
					pUpLinkPart=NULL;
			}
			CLDSPart *pBranchRootWeldPart=pUpLinkPart;
			while(pUpLinkPart!=NULL)
			{
				pUpLinkPart=FromPartHandle(pBranchRootWeldPart->m_hPartWeldParent);
				if(pUpLinkPart!=NULL)
					pBranchRootWeldPart=pUpLinkPart;
			}
			if(pBranchRootWeldPart)
			{
				PARTSET tmpSet;
				partset.append(pBranchRootWeldPart);
				if(GetPartGroupPartSet(pBranchRootWeldPart->handle,tmpSet,CLASS_TYPEID,false,&pTube->handle)>0)
				{
					for(CLDSPart *pTmPart=tmpSet.GetFirst();pTmPart;pTmPart=tmpSet.GetNext())
						partset.append(pTmPart);
				}
			}
		}
	}
	for(CLDSPart *pPart=Parts.GetFirst(CLASS_TYPEID);pPart;pPart=Parts.GetNext(CLASS_TYPEID))
	{
		if(pPart->handle==hPartGroup)
			continue;
		bool excluded=false;
		for(int k=0;phExcludePartBranchArr!=NULL&&k<excludeCount;k++)
		{
			if(phExcludePartBranchArr[k]!=pPart->handle)
				continue;
			excluded=true;
			break;
		}
		if(excluded)
			continue;	//�÷�֧�ѱ��ų���ͳ��֮��
		else if(pPart->m_hPartWeldParent==hPartGroup)
		{
			partset.append(pPart);
			set.Empty();
			int count=GetPartGroupPartSet(pPart->handle,set,CLASS_TYPEID,false);
			if(count<0)
				return -1;
			else if(count>0)
			{
				for(CLDSPart *pTmPart=set.GetFirst();pTmPart;pTmPart=set.GetNext())
					partset.append(pTmPart);
			}
		}
	}
	return partset.GetNodeNum();
}
#endif
typedef	ATOM_LIST<RELATIVE_OBJECT> LIST;
//�����׼������
void WriteStdPartLibToBuffer(CBuffer &buffer)
{
	int i=0,j=0;
	//���Ͳ���
	buffer.WriteInteger(rollend_param_N);
	for(i=0;i<rollend_param_N;i++)
	{
		buffer.WriteDouble(rollend_param_table[i].d);	
		buffer.WriteDouble(rollend_param_table[i].t);	
		buffer.WriteByte(rollend_param_table[i].cMaterial);	
		buffer.WriteInteger(rollend_param_table[i].iLevelMark);
		buffer.WriteString(rollend_param_table[i].codeName);
		buffer.WriteByte(rollend_param_table[i].cTubeMat);
		buffer.WriteDouble(rollend_param_table[i].L);	
		buffer.WriteDouble(rollend_param_table[i].M);	
		buffer.WriteDouble(rollend_param_table[i].H);	
		buffer.WriteDouble(rollend_param_table[i].W);	
		buffer.WriteDouble(rollend_param_table[i].N);	
		buffer.WriteDouble(rollend_param_table[i].S);	
		buffer.WriteInteger(rollend_param_table[i].Thick);
		buffer.WriteDouble(rollend_param_table[i].theroy_weight);//��������
		//��˨�����������˨λ�ò���
		buffer.WriteInteger(rollend_param_table[i].iBoltRect);
		for(j=0;j<rollend_param_table[i].iBoltRect;j++)
		{
			buffer.WriteInteger(rollend_param_table[i].ls_param[j].iBoltNum);
			buffer.WriteInteger(rollend_param_table[i].ls_param[j].iRow);
			buffer.WriteInteger(rollend_param_table[i].ls_param[j].iColumn);
			buffer.WriteInteger(rollend_param_table[i].ls_param[j].iBoltD);
			buffer.WriteDouble(rollend_param_table[i].ls_param[j].fBoltPosX);
			buffer.WriteDouble(rollend_param_table[i].ls_param[j].fBoltPosY);
			buffer.WriteDouble(rollend_param_table[i].ls_param[j].fHorzSpace);
			buffer.WriteDouble(rollend_param_table[i].ls_param[j].fVertSpace);
		}
	}
	//U�Ͳ���
	buffer.WriteInteger(uend_param_N);
	for(i=0;i<uend_param_N;i++)
	{
		buffer.WriteDouble(uend_param_table[i].d);	
		buffer.WriteDouble(uend_param_table[i].t);	
		buffer.WriteByte(uend_param_table[i].cMaterial);	
		buffer.WriteInteger(uend_param_table[i].iLevelMark);
		buffer.WriteString(uend_param_table[i].codeName);
		buffer.WriteByte(uend_param_table[i].cTubeMat);
		buffer.WriteDouble(uend_param_table[i].L);	
		buffer.WriteDouble(uend_param_table[i].M);	
		buffer.WriteDouble(uend_param_table[i].H);	
		buffer.WriteDouble(uend_param_table[i].W);	
		buffer.WriteDouble(uend_param_table[i].N);	
		buffer.WriteDouble(uend_param_table[i].S);	
		buffer.WriteInteger(uend_param_table[i].Thick);
		buffer.WriteDouble(uend_param_table[i].theroy_weight);//��������
		//��˨�����������˨λ�ò���
		buffer.WriteInteger(uend_param_table[i].iBoltRect);
		for(j=0;j<uend_param_table[i].iBoltRect;j++)
		{
			buffer.WriteInteger(uend_param_table[i].ls_param[j].iBoltNum);
			buffer.WriteInteger(uend_param_table[i].ls_param[j].iRow);
			buffer.WriteInteger(uend_param_table[i].ls_param[j].iColumn);
			buffer.WriteInteger(uend_param_table[i].ls_param[j].iBoltD);
			buffer.WriteDouble(uend_param_table[i].ls_param[j].fBoltPosX);
			buffer.WriteDouble(uend_param_table[i].ls_param[j].fBoltPosY);
			buffer.WriteDouble(uend_param_table[i].ls_param[j].fHorzSpace);
			buffer.WriteDouble(uend_param_table[i].ls_param[j].fVertSpace);
		}
	}
	//ʮ���Ͳ���
	buffer.WriteInteger(xend_param_N);
	for(i=0;i<xend_param_N;i++)
	{
		buffer.WriteDouble(xend_param_table[i].d);	
		buffer.WriteDouble(xend_param_table[i].t);	
		buffer.WriteByte(xend_param_table[i].cMaterial);	
		buffer.WriteInteger(xend_param_table[i].iLevelMark);
		buffer.WriteString(xend_param_table[i].codeName);
		buffer.WriteString(xend_param_table[i].datumPlatePartNo);
		buffer.WriteString(xend_param_table[i].weldPlatePartNo);
		buffer.WriteString(xend_param_table[i].wrapPlatePartNo);
		buffer.WriteByte(xend_param_table[i].cTubeMat);
		buffer.WriteDouble(xend_param_table[i].L);	
		buffer.WriteDouble(xend_param_table[i].M);	
		buffer.WriteDouble(xend_param_table[i].H);	
		buffer.WriteDouble(xend_param_table[i].W);	
		buffer.WriteDouble(xend_param_table[i].N);	
		buffer.WriteDouble(xend_param_table[i].S);	
		buffer.WriteInteger(xend_param_table[i].Thick);
		buffer.WriteDouble(xend_param_table[i].theroy_weight);//��������
		buffer.WriteDouble(xend_param_table[i].A);	
		buffer.WriteDouble(xend_param_table[i].R);	
		//��˨�����������˨λ�ò���
		buffer.WriteInteger(xend_param_table[i].iBoltRect);
		for(j=0;j<xend_param_table[i].iBoltRect;j++)
		{
			buffer.WriteInteger(xend_param_table[i].ls_param[j].iBoltNum);
			buffer.WriteInteger(xend_param_table[i].ls_param[j].iRow);
			buffer.WriteInteger(xend_param_table[i].ls_param[j].iColumn);
			buffer.WriteInteger(xend_param_table[i].ls_param[j].iBoltD);
			buffer.WriteDouble(xend_param_table[i].ls_param[j].fBoltPosX);
			buffer.WriteDouble(xend_param_table[i].ls_param[j].fBoltPosY);
			buffer.WriteDouble(xend_param_table[i].ls_param[j].fHorzSpace);
			buffer.WriteDouble(xend_param_table[i].ls_param[j].fVertSpace);
		}
	}
	//�����ԽӼ����
	/*buffer.WriteInteger(fl_couple_rule_N);
	for(i=0;i<fl_couple_rule_N;i++)
	{
		buffer.WriteDouble(fl_couple_rule_table[i].d);
		buffer.WriteDouble(fl_couple_rule_table[i].A);
		buffer.WriteDouble(fl_couple_rule_table[i].B);
		buffer.WriteDouble(fl_couple_rule_table[i].C);
		buffer.WriteDouble(fl_couple_rule_table[i].D);
	}*/
	//�Ժ�����
	buffer.WriteInteger(fld_param_N);
	for(i=0;i<fld_param_N;i++)
	{
		buffer.WriteDouble(fld_param_table[i].d);
		buffer.WriteDouble(fld_param_table[i].couple_d);
		buffer.WriteDouble(fld_param_table[i].t);
		buffer.WriteByte(fld_param_table[i].cLevelMark);
		buffer.WriteByte(fld_param_table[i].cMaterial);
		buffer.WriteString(fld_param_table[i].codeName);
		buffer.WriteDouble(fld_param_table[i].A);
		buffer.WriteDouble(fld_param_table[i].B);
		buffer.WriteDouble(fld_param_table[i].D);
		buffer.WriteDouble(fld_param_table[i].K);
		buffer.WriteDouble(fld_param_table[i].L);
		buffer.WriteInteger(fld_param_table[i].nBoltNum); 
		buffer.WriteInteger(fld_param_table[i].nBoltD);
		buffer.WriteDouble(fld_param_table[i].C);
		buffer.WriteDouble(fld_param_table[i].H);
		buffer.WriteDouble(fld_param_table[i].N);
		buffer.WriteDouble(fld_param_table[i].S);
		buffer.WriteDouble(fld_param_table[i].H1);
		buffer.WriteDouble(fld_param_table[i].R);
		for(int j=0;j<5;j++)
		{
			buffer.WriteInteger(fld_param_table[i].group[j].t);
			buffer.WriteDouble(fld_param_table[i].group[j].weight);
		}
	}
	//ƽ������
	buffer.WriteInteger(flp_param_N);
	for(i=0;i<flp_param_N;i++)
	{
		buffer.WriteDouble(flp_param_table[i].d);
		buffer.WriteDouble(flp_param_table[i].couple_d);
		buffer.WriteDouble(flp_param_table[i].t);
		buffer.WriteByte(flp_param_table[i].cLevelMark);
		buffer.WriteByte(flp_param_table[i].cMaterial);
		buffer.WriteString(flp_param_table[i].codeName);
		buffer.WriteDouble(flp_param_table[i].A);
		buffer.WriteDouble(flp_param_table[i].B);
		buffer.WriteDouble(flp_param_table[i].D);
		buffer.WriteDouble(flp_param_table[i].K);
		buffer.WriteDouble(flp_param_table[i].L);
		buffer.WriteInteger(flp_param_table[i].nBoltNum); 
		buffer.WriteInteger(flp_param_table[i].nBoltD);
		buffer.WriteDouble(flp_param_table[i].C);
		buffer.WriteDouble(flp_param_table[i].H);
		buffer.WriteDouble(flp_param_table[i].N);
		buffer.WriteDouble(flp_param_table[i].S);
		buffer.WriteDouble(flp_param_table[i].H1);
		buffer.WriteDouble(flp_param_table[i].R);
		buffer.WriteDouble(flp_param_table[i].group[0].weight);
	}
	//���Է���
	buffer.WriteInteger(flg_param_N);
	for(int i=0;i<flg_param_N;i++)
	{
		buffer.WriteDouble(flg_param_tbl[i].d);
		buffer.WriteDouble(flg_param_tbl[i].couple_d);
		buffer.WriteByte(flg_param_tbl[i].cLevelMark);
		buffer.WriteByte(flg_param_tbl[i].cMaterial);
		buffer.WriteString(flg_param_tbl[i].codeName);
		buffer.WriteDouble(flg_param_tbl[i].fInnerD);
		buffer.WriteDouble(flg_param_tbl[i].fOuterD);
		buffer.WriteDouble(flg_param_tbl[i].fBoltCirD);
		buffer.WriteDouble(flg_param_tbl[i].fBoltHoleD);
		buffer.WriteInteger(flg_param_tbl[i].nBoltNum);
		buffer.WriteInteger(flg_param_tbl[i].nBoltD);
		buffer.WriteDouble(flg_param_tbl[i].fThick);
		buffer.WriteDouble(flg_param_tbl[i].fH);
		buffer.WriteDouble(flg_param_tbl[i].theroy_weight);
		buffer.WriteInteger(flg_param_tbl[i].nRibPlateNum);
		buffer.WriteDouble(flg_param_tbl[i].para.weight);
		buffer.WriteByte(flg_param_tbl[i].para.cMat);
		buffer.WriteDouble(flg_param_tbl[i].para.T);
		buffer.WriteDouble(flg_param_tbl[i].para.A);
		buffer.WriteDouble(flg_param_tbl[i].para.B);
		buffer.WriteDouble(flg_param_tbl[i].para.C);
		buffer.WriteDouble(flg_param_tbl[i].para.D);
		buffer.WriteDouble(flg_param_tbl[i].para.E);
		buffer.WriteDouble(flg_param_tbl[i].para.F);
		buffer.WriteDouble(flg_param_tbl[i].para.G);
		buffer.WriteDouble(flg_param_tbl[i].para.H);
	}
	//���Է���
	buffer.WriteInteger(flr_param_N);
	for(int i=0;i<flr_param_N;i++)
	{
		buffer.WriteDouble(flr_param_tbl[i].d);
		buffer.WriteDouble(flr_param_tbl[i].couple_d);
		buffer.WriteByte(flr_param_tbl[i].cLevelMark);
		buffer.WriteByte(flr_param_tbl[i].cMaterial);
		buffer.WriteString(flr_param_tbl[i].codeName);
		buffer.WriteDouble(flr_param_tbl[i].fInnerD);
		buffer.WriteDouble(flr_param_tbl[i].fOuterD);
		buffer.WriteDouble(flr_param_tbl[i].fBoltCirD);
		buffer.WriteDouble(flr_param_tbl[i].fBoltHoleD);
		buffer.WriteInteger(flr_param_tbl[i].nBoltNum);
		buffer.WriteInteger(flr_param_tbl[i].nBoltD);
		buffer.WriteDouble(flr_param_tbl[i].fThick);
		buffer.WriteDouble(flr_param_tbl[i].fH);
		buffer.WriteDouble(flr_param_tbl[i].theroy_weight);
		buffer.WriteInteger(flr_param_tbl[i].nRibPlateNum);
		buffer.WriteDouble(flr_param_tbl[i].para.weight);
		buffer.WriteByte(flr_param_tbl[i].para.cMat);
		buffer.WriteDouble(flr_param_tbl[i].para.T);
		buffer.WriteDouble(flr_param_tbl[i].para.A);
		buffer.WriteDouble(flr_param_tbl[i].para.B);
		buffer.WriteDouble(flr_param_tbl[i].para.C);
		buffer.WriteDouble(flr_param_tbl[i].para.D);
		buffer.WriteDouble(flr_param_tbl[i].para.E);
		buffer.WriteDouble(flr_param_tbl[i].para.F);
		buffer.WriteDouble(flr_param_tbl[i].para.G);
		buffer.WriteDouble(flr_param_tbl[i].para.H);
	}
}
void WriteStdPartLibToXmlFile(FILE* fp,DWORD schema)
{
	int i=0,j=0;
	char sText[30]="";
	//���Ͳ���
	fprintf(fp,"<���Ͳ�����Ϣ rollend_param_N=\"%d\">\n",rollend_param_N);
	for(i=0;i<rollend_param_N;i++)
	{
		fprintf(fp,"<���Ͳ����Ϣ>\n");
		fprintf(fp,"<����ֱ�� d=\"%.2f\"/>\n",rollend_param_table[i].d);
		fprintf(fp,"<���ܺ�� t=\"%.2f\"/>\n",rollend_param_table[i].t);
		fprintf(fp,"<������ cMaterial=\"%c\"/>\n",rollend_param_table[i].cMaterial);
		fprintf(fp,"<�����־ LevelMark=\"%d\"/>\n",rollend_param_table[i].iLevelMark);
		fprintf(fp,"<������ codeName=\"%s\"/>\n",rollend_param_table[i].codeName);
		fprintf(fp,"<�ֹܲ��� TubeMat=\"%c\"/>\n",rollend_param_table[i].cTubeMat);
		fprintf(fp,"<���� L=\"%.2f\"/>\n",rollend_param_table[i].L);
		fprintf(fp,"<����ֹܳ��� M=\"%.2f\"/>\n",rollend_param_table[i].M);
		fprintf(fp,"<��� H=\"%.2f\"/>\n",rollend_param_table[i].H);
		fprintf(fp,"<��߸߶� W=\"%.2f\"/>\n",rollend_param_table[i].W);
		fprintf(fp,"<�нǳ��� N=\"%.2f\"/>\n",rollend_param_table[i].N);
		fprintf(fp,"<�нǿ�� S=\"%.2f\"/>\n",rollend_param_table[i].S);
		fprintf(fp,"<��� Thick=\"%d\"/>\n",rollend_param_table[i].Thick);
		fprintf(fp,"<�������� weight=\"%.3f\"/>\n",rollend_param_table[i].theroy_weight);
		//��˨�����������˨λ�ò���
		fprintf(fp,"<��˨������� BoltRect=\"%d\"/>\n",rollend_param_table[i].iBoltRect);
		for(j=0;j<rollend_param_table[i].iBoltRect;j++)
		{
			fprintf(fp,"<��˨������Ϣ>\n");
			fprintf(fp,"<��˨���� BoltNum=\"%d\"/>\n",rollend_param_table[i].ls_param[j].iBoltNum);
			fprintf(fp,"<��˨���� Row=\"%d\"/>\n",rollend_param_table[i].ls_param[j].iRow);
			fprintf(fp,"<��˨���� Column=\"%d\"/>\n",rollend_param_table[i].ls_param[j].iColumn);
			fprintf(fp,"<��˨ֱ�� BoltD=\"%d\"/>\n",rollend_param_table[i].ls_param[j].iBoltD);
			sprintf(sText,"%f",rollend_param_table[i].ls_param[j].fBoltPosX);
			SimplifiedNumString(sText);
			fprintf(fp,"<����˨X���� BoltPosX=\"%s\"/>\n",sText);
			sprintf(sText,"%f",rollend_param_table[i].ls_param[j].fBoltPosY);
			SimplifiedNumString(sText);
			fprintf(fp,"<����˨Y���� BoltPosY=\"%s\"/>\n",sText);
			sprintf(sText,"%f",rollend_param_table[i].ls_param[j].fHorzSpace);
			SimplifiedNumString(sText);
			fprintf(fp,"<������˨������ HorzSpace=\"%s\"/>\n",sText);
			sprintf(sText,"%f",rollend_param_table[i].ls_param[j].fVertSpace);
			SimplifiedNumString(sText);
			fprintf(fp,"<������˨������ VertSpace=\"%s\"/>\n",sText);
			fprintf(fp,"</��˨������Ϣ>\n");
		}
		fprintf(fp,"</���Ͳ����Ϣ>\n");
	}
	fprintf(fp,"</���Ͳ�����Ϣ>");
	//U�Ͳ���
	fprintf(fp,"<U�Ͳ�����Ϣ uend_param_N=\"%d\">\n",uend_param_N);
	for(i=0;i<uend_param_N;i++)
	{
		fprintf(fp,"<U�Ͳ����Ϣ>\n");
		fprintf(fp,"<����ֱ�� d=\"%.2f\"/>\n",uend_param_table[i].d);
		fprintf(fp,"<���ܺ�� t=\"%.2f\"/>\n",uend_param_table[i].t);
		fprintf(fp,"<������ cMaterial=\"%c\"/>\n",uend_param_table[i].cMaterial);
		fprintf(fp,"<�����־ LevelMark=\"%d\"/>\n",uend_param_table[i].iLevelMark);
		fprintf(fp,"<������ codeName=\"%s\"/>\n",uend_param_table[i].codeName);
		fprintf(fp,"<�ֹܲ��� TubeMat=\"%c\"/>\n",uend_param_table[i].cTubeMat);
		fprintf(fp,"<���� L=\"%.2f\"/>\n",uend_param_table[i].L);
		fprintf(fp,"<����ֹܳ��� M=\"%.2f\"/>\n",uend_param_table[i].M);
		fprintf(fp,"<��� H=\"%.2f\"/>\n",uend_param_table[i].H);
		fprintf(fp,"<����������� W=\"%.2f\"/>\n",uend_param_table[i].W);
		fprintf(fp,"<�нǳ��� N=\"%.2f\"/>\n",uend_param_table[i].N);
		fprintf(fp,"<�нǿ�� S=\"%.2f\"/>\n",uend_param_table[i].S);
		fprintf(fp,"<��� Thick=\"%d\"/>\n",uend_param_table[i].Thick);
		fprintf(fp,"<�������� weight=\"%.3f\"/>\n",uend_param_table[i].theroy_weight);
		//��˨�����������˨λ�ò���
		fprintf(fp,"<��˨������� BoltRect=\"%d\"/>\n",uend_param_table[i].iBoltRect);
		for(j=0;j<uend_param_table[i].iBoltRect;j++)
		{
			fprintf(fp,"<��˨������Ϣ>\n");
			fprintf(fp,"<��˨���� BoltNum=\"%d\"/>\n",uend_param_table[i].ls_param[j].iBoltNum);
			fprintf(fp,"<��˨���� Row=\"%d\"/>\n",uend_param_table[i].ls_param[j].iRow);
			fprintf(fp,"<��˨���� Column=\"%d\"/>\n",uend_param_table[i].ls_param[j].iColumn);
			fprintf(fp,"<��˨ֱ�� BoltD=\"%d\"/>\n",uend_param_table[i].ls_param[j].iBoltD);
			sprintf(sText,"%f",uend_param_table[i].ls_param[j].fBoltPosX);
			SimplifiedNumString(sText);
			fprintf(fp,"<����˨X���� BoltPosX=\"%s\"/>\n",sText);
			sprintf(sText,"%f",uend_param_table[i].ls_param[j].fBoltPosY);
			SimplifiedNumString(sText);
			fprintf(fp,"<����˨Y���� BoltPosY=\"%s\"/>\n",sText);
			sprintf(sText,"%f",uend_param_table[i].ls_param[j].fHorzSpace);
			SimplifiedNumString(sText);
			fprintf(fp,"<������˨������ HorzSpace=\"%s\"/>\n",sText);
			sprintf(sText,"%f",uend_param_table[i].ls_param[j].fVertSpace);
			SimplifiedNumString(sText);
			fprintf(fp,"<������˨������ VertSpace=\"%s\"/>\n",sText);
			fprintf(fp,"</��˨������Ϣ>\n");
		}
		fprintf(fp,"</U�Ͳ����Ϣ>\n");
	}
	fprintf(fp,"</U�Ͳ�����Ϣ>\n");
	//ʮ���Ͳ���
	fprintf(fp,"<ʮ���Ͳ�����Ϣ xend_param_N=\"%d\">\n",xend_param_N);
	for(i=0;i<xend_param_N;i++)
	{
		fprintf(fp,"<ʮ���Ͳ����Ϣ>\n");
		fprintf(fp,"<����ֱ�� d=\"%.2f\"/>\n",xend_param_table[i].d);
		fprintf(fp,"<���ܺ�� t=\"%.2f\"/>\n",xend_param_table[i].t);
		fprintf(fp,"<������ cMaterial=\"%c\"/>\n",xend_param_table[i].cMaterial);
		fprintf(fp,"<�����־ LevelMark=\"%d\"/>\n",xend_param_table[i].iLevelMark);
		fprintf(fp,"<������ codeName=\"%s\"/>\n",xend_param_table[i].codeName);
		fprintf(fp,"<������ datumPlatePartNo=\"%s\"/>\n",xend_param_table[i].datumPlatePartNo);
		fprintf(fp,"<������ weldPlatePartNo=\"%s\"/>\n",xend_param_table[i].weldPlatePartNo);
		fprintf(fp,"<������ wrapPlatePartNo=\"%s\"/>\n",xend_param_table[i].wrapPlatePartNo);
		fprintf(fp,"<�ֹܲ��� TubeMat=\"%c\"/>\n",xend_param_table[i].cTubeMat);
		fprintf(fp,"<���� L=\"%.2f\"/>\n",xend_param_table[i].L);
		fprintf(fp,"<����ֹܳ��� M=\"%.2f\"/>\n",xend_param_table[i].M);
		fprintf(fp,"<��� H=\"%.2f\"/>\n",xend_param_table[i].H);
		fprintf(fp,"<���Ӱ��� W=\"%.2f\"/>\n",xend_param_table[i].W);
		fprintf(fp,"<���Ӱ��� N=\"%.2f\"/>\n",xend_param_table[i].N);
		fprintf(fp,"<���Ӱ峤�� S=\"%.2f\"/>\n",xend_param_table[i].S);
		fprintf(fp,"<��� Thick=\"%d\"/>\n",xend_param_table[i].Thick);
		fprintf(fp,"<�������� weight=\"%.3f\"/>\n",xend_param_table[i].theroy_weight);
		fprintf(fp,"<���Ӱ嶨λ����A A=\"%.2f\"/>\n",xend_param_table[i].A);
		fprintf(fp,"<���Ӱ嶨λ����R R=\"%.2f\"/>\n",xend_param_table[i].R);
		//��˨�����������˨λ�ò���
		fprintf(fp,"<��˨������� BoltRect=\"%d\"/>\n",xend_param_table[i].iBoltRect);	
		for(j=0;j<xend_param_table[i].iBoltRect;j++)
		{
			fprintf(fp,"<��˨������Ϣ>\n");
			fprintf(fp,"<��˨��ʽ BoltNum=\"%d\"/>\n",xend_param_table[i].ls_param[j].iBoltNum);
			fprintf(fp,"<��˨���� Row=\"%d\"/>\n",xend_param_table[i].ls_param[j].iRow);
			fprintf(fp,"<��˨���� Column=\"%d\"/>\n",xend_param_table[i].ls_param[j].iColumn);
			fprintf(fp,"<��˨ֱ�� BoltD=\"%d\"/>\n",xend_param_table[i].ls_param[j].iBoltD);
			sprintf(sText,"%f",xend_param_table[i].ls_param[j].fBoltPosX);
			SimplifiedNumString(sText);
			fprintf(fp,"<�׸���˨X���� BoltPosX=\"%s\"/>\n",sText);
			sprintf(sText,"%f",xend_param_table[i].ls_param[j].fBoltPosY);
			SimplifiedNumString(sText);
			fprintf(fp,"<�׸���˨Y���� BoltPosY=\"%s\"/>\n",sText);
			sprintf(sText,"%f",xend_param_table[i].ls_param[j].fHorzSpace);
			SimplifiedNumString(sText);
			fprintf(fp,"<������˨������ HorzSpace=\"%s\"/>\n",sText);
			sprintf(sText,"%f",xend_param_table[i].ls_param[j].fVertSpace);
			SimplifiedNumString(sText);
			fprintf(fp,"<������˨������ VertSpace=\"%s\"/>\n",sText);
			fprintf(fp,"</��˨������Ϣ>\n");
		}
		fprintf(fp,"</ʮ���Ͳ����Ϣ>\n");
	}
	fprintf(fp,"</ʮ���Ͳ�����Ϣ>\n");
	//�����ԽӼ����
	fprintf(fp,"<�����ԽӼ���� fl_cuople_rule_N=\"%d\">\n",fl_couple_rule_N);
	for(i=0;i<fl_couple_rule_N;i++)
	{
		fprintf(fp,"<�����Խ���Ϣ>\n");
		fprintf(fp,"<����ֱ�� d=\"%.2f\"/>\n",fl_couple_rule_table[i].d);
		fprintf(fp,"<�ԽӸֹ�ֱ�� A=\"%.2f\"/>\n",fl_couple_rule_table[i].A);
		fprintf(fp,"<�ԽӸֹ�ֱ�� B=\"%.2f\"/>\n",fl_couple_rule_table[i].B);
		fprintf(fp,"<�ԽӸֹ�ֱ�� C=\"%.2f\"/>\n",fl_couple_rule_table[i].C);
		fprintf(fp,"<�ԽӸֹ�ֱ�� D=\"%.2f\"/>\n",fl_couple_rule_table[i].D);
		fprintf(fp,"</�����Խ���Ϣ>\n");
	}
	fprintf(fp,"</�����ԽӼ����>\n");
	//�Ժ�����
	fprintf(fp,"<�Ժ���������Ϣ fld_param_N=\"%d\">\n",fld_param_N);
	for(i=0;i<fld_param_N;i++)
	{
		fprintf(fp,"<�Ժ�������Ϣ>\n");
		fprintf(fp,"<����ֱ�� d=\"%.2f\"/>\n",fld_param_table[i].d);
		fprintf(fp,"<���ܺ�� t=\"%.2f\"/>\n",fld_param_table[i].t);
		fprintf(fp,"<�����־ LevelMark=\"%d\"/>\n",fld_param_table[i].cLevelMark);
		fprintf(fp,"<�������� cMaterial=\"%c\"/>\n",fld_param_table[i].cMaterial);
		fprintf(fp,"<�������� codeName=\"%s\"/>\n",fld_param_table[i].codeName);
		fprintf(fp,"<�����⾶ A=\"%.2f\"/>\n",fld_param_table[i].A);
		fprintf(fp,"<�����ھ� B=\"%.2f\"/>\n",fld_param_table[i].B);
		fprintf(fp,"<�����⾶ D=\"%.2f\"/>\n",fld_param_table[i].D);
		fprintf(fp,"<��˨�׶�λԲֱ�� K=\"%.2f\"/>\n",fld_param_table[i].K);
		fprintf(fp,"<��˨�׾� L=\"%.2f\"/>\n",fld_param_table[i].L);
		fprintf(fp,"<��˨���� BoltNum=\"%d\"/>\n",fld_param_table[i].nBoltNum);
		fprintf(fp,"<��˨ֱ�� BoltD=\"%d\"/>\n",fld_param_table[i].nBoltD);
		fprintf(fp,"<������� C=\"%.2f\"/>\n",fld_param_table[i].C);
		fprintf(fp,"<�����߶� H=\"%.2f\"/>\n",fld_param_table[i].H);
		fprintf(fp,"<��������ֱ�� N=\"%.2f\"/>\n",fld_param_table[i].N);
		fprintf(fp,"<����������� S=\"%.2f\"/>\n",fld_param_table[i].S);
		fprintf(fp,"<���������߶� H1=\"%.2f\"/>\n",fld_param_table[i].H1);
		fprintf(fp,"<�����������ǰ뾶 R=\"%.2f\"/>\n",fld_param_table[i].R);
		for(int j=0;j<5;j++)
			fprintf(fp,"<�Ժ�����������Ϣ t=\"%d\" weight=\"%.3f\"/>\n",fld_param_table[i].group[j].t,fld_param_table[i].group[j].weight);
		fprintf(fp,"</�Ժ�������Ϣ>\n");
	}
	fprintf(fp,"</�Ժ���������Ϣ>\n");
	//ƽ������
	fprintf(fp,"<ƽ����������Ϣ flp_param_N=\"%d\">\n",flp_param_N);
	for(i=0;i<flp_param_N;i++)
	{
		fprintf(fp,"<ƽ��������Ϣ>\n");
		fprintf(fp,"<����ֱ�� d=\"%.2f\"/>\n",flp_param_table[i].d);
		fprintf(fp,"<���ܺ�� t=\"%.2f\"/>\n",flp_param_table[i].t);
		fprintf(fp,"<�����־ LevelMark=\"%c\"/>\n",flp_param_table[i].cLevelMark);
		fprintf(fp,"<�������� Material=\"%c\"/>\n",flp_param_table[i].cMaterial);
		fprintf(fp,"<�������� codeName=\"%s\"/>\n",flp_param_table[i].codeName);
		fprintf(fp,"<�����⾶ A=\"%.2f\"/>\n",flp_param_table[i].A);
		fprintf(fp,"<�����ھ� B=\"%.2f\"/>\n",flp_param_table[i].B);
		fprintf(fp,"<�����⾶ D=\"%.2f\"/>\n",flp_param_table[i].D);
		fprintf(fp,"<��˨�׶�λԲֱ�� K=\"%.2f\"/>\n",flp_param_table[i].K);
		fprintf(fp,"<��˨�׾� L=\"%.2f\"/>\n",flp_param_table[i].L);
		fprintf(fp,"<��˨���� BoltNum=\"%d\"/>\n",flp_param_table[i].nBoltNum);
		fprintf(fp,"<��˨ֱ�� BoltD=\"%d\"/>\n",flp_param_table[i].nBoltD);
		fprintf(fp,"<������� C=\"%.2f\"/>\n",flp_param_table[i].C);
		fprintf(fp,"<�����߶� H=\"%.2f\"/>\n",flp_param_table[i].H);
		fprintf(fp,"<��������ֱ�� N=\"%.2f\"/>\n",flp_param_table[i].N);
		fprintf(fp,"<����������� S=\"%.2f\"/>\n",flp_param_table[i].S);
		fprintf(fp,"<���������߶� H1=\"%.2f\"/>\n",flp_param_table[i].H1);
		fprintf(fp,"<�����������ǰ뾶 R=\"%.2f\"/>\n",flp_param_table[i].R);
		fprintf(fp,"<�������� weight=\"%.3f\"/>\n",flp_param_table[i].group[0].weight);
		fprintf(fp,"</ƽ��������Ϣ>\n");
	}
	fprintf(fp,"</ƽ����������Ϣ>\n");
	//���Է���
	fprintf(fp,"<���Է�������Ϣ flg_param_N=\"%d\">\n",flg_param_N);
	for(int i=0;i<flg_param_N;i++)
	{
		fprintf(fp,"<���Է�����Ϣ>\n");
		fprintf(fp,"<����ֱ�� d=\"%.2f\"/>\n",flg_param_tbl[i].d);
		fprintf(fp,"<�ԽӸֹ�ֱ�� couple_d=\"%.2f\"/>\n",flg_param_tbl[i].couple_d);
		fprintf(fp,"<�����־ cLevelMark=\"%c\"/>\n",flg_param_tbl[i].cLevelMark);
		fprintf(fp,"<�������� cMaterial=\"%c\"/>\n",flg_param_tbl[i].cMaterial);
		fprintf(fp,"<�������� codeName=\"%s\"/>\n",flg_param_tbl[i].codeName);
		fprintf(fp,"<�����ھ� fInnerD=\"%.2f\"/>\n",flg_param_tbl[i].fInnerD);
		fprintf(fp,"<�����⾶ fOuterD=\"%.2f\"/>\n",flg_param_tbl[i].fOuterD);
		fprintf(fp,"<��˨�׶�λԲֱ�� fBoltCirD=\"%.2f\"/>\n",flg_param_tbl[i].fBoltCirD);
		fprintf(fp,"<��˨�׾� fBoltHoleD=\"%.2f\"/>\n",flg_param_tbl[i].fBoltHoleD);
		fprintf(fp,"<��˨���� nBoltNum=\"%d\"/>\n",flg_param_tbl[i].nBoltNum);
		fprintf(fp,"<��˨ֱ�� nBoltD=\"%d\"/>\n",flg_param_tbl[i].nBoltD);
		fprintf(fp,"<������� fThick=\"%.2f\"/>\n",flg_param_tbl[i].fThick);
		fprintf(fp,"<�ֹܶ��뷨������ľ��� fH=\"%.2f\"/>\n",flg_param_tbl[i].fH);
		fprintf(fp,"<�������� theroy_weight=\"%.3f\"/>\n",flg_param_tbl[i].theroy_weight);
		fprintf(fp,"<�߰���� nRibPlateNum=\"%d\"/>\n",flg_param_tbl[i].nRibPlateNum);
		fprintf(fp,"<�߰����� weight=\"%.3f\"/>\n",flg_param_tbl[i].para.weight);
		fprintf(fp,"<�߰���� cMat=\"%c\"/>\n",flg_param_tbl[i].para.cMat);
		fprintf(fp,"<�߰��� T=\"%.2f\"/>\n",flg_param_tbl[i].para.T);
		fprintf(fp,"<�߰�߶� A=\"%.2f\"/>\n",flg_param_tbl[i].para.A);
		fprintf(fp,"<�߰�ײ���� B=\"%.2f\"/>\n",flg_param_tbl[i].para.B);
		fprintf(fp,"<�߰����߶� C=\"%.2f\"/>\n",flg_param_tbl[i].para.C);
		fprintf(fp,"<�߰�ײ���� D=\"%.2f\"/>\n",flg_param_tbl[i].para.D);
		fprintf(fp,"<���ǿ�� E=\"%.2f\"/>\n",flg_param_tbl[i].para.E);
		fprintf(fp,"<�׶�λ���� F=\"%.2f\"/>\n",flg_param_tbl[i].para.F);
		fprintf(fp,"<�׶�λ���� G=\"%.2f\"/>\n",flg_param_tbl[i].para.G);
		fprintf(fp,"<��ֱ�� H=\"%.2f\"/>\n",flg_param_tbl[i].para.H);
		fprintf(fp,"</���Է�����Ϣ>\n");
	}
	fprintf(fp,"</���Է�������Ϣ>\n");
	//���Է���
	fprintf(fp,"<���Է�������Ϣ flg_param_N=\"%d\">\n",flr_param_N);
	for(int i=0;i<flr_param_N;i++)
	{
		fprintf(fp,"<���Է�����Ϣ>\n");
		fprintf(fp,"<����ֱ�� d=\"%.2f\"/>\n",flr_param_tbl[i].d);
		fprintf(fp,"<�ԽӸֹ�ֱ�� couple_d=\"%.2f\"/>\n",flr_param_tbl[i].couple_d);
		fprintf(fp,"<�����־ cLevelMark=\"%c\"/>\n",flr_param_tbl[i].cLevelMark);
		fprintf(fp,"<�������� cMaterial=\"%c\"/>\n",flr_param_tbl[i].cMaterial);
		fprintf(fp,"<�������� codeName=\"%s\"/>\n",flr_param_tbl[i].codeName);
		fprintf(fp,"<�����ھ� fInnerD=\"%.2f\"/>\n",flr_param_tbl[i].fInnerD);
		fprintf(fp,"<�����⾶ fOuterD=\"%.2f\"/>\n",flr_param_tbl[i].fOuterD);
		fprintf(fp,"<��˨�׶�λԲֱ�� fBoltCirD=\"%.2f\"/>\n",flr_param_tbl[i].fBoltCirD);
		fprintf(fp,"<��˨�׾� fBoltHoleD=\"%.2f\"/>\n",flr_param_tbl[i].fBoltHoleD);
		fprintf(fp,"<��˨���� nBoltNum=\"%d\"/>\n",flr_param_tbl[i].nBoltNum);
		fprintf(fp,"<��˨ֱ�� nBoltD=\"%d\"/>\n",flr_param_tbl[i].nBoltD);
		fprintf(fp,"<������� fThick=\"%.2f\"/>\n",flr_param_tbl[i].fThick);
		fprintf(fp,"<�ֹܶ��뷨������ľ��� fH=\"%.2f\"/>\n",flr_param_tbl[i].fH);
		fprintf(fp,"<�������� theroy_weight=\"%.3f\"/>\n",flr_param_tbl[i].theroy_weight);
		fprintf(fp,"<�߰���� nRibPlateNum=\"%d\"/>\n",flr_param_tbl[i].nRibPlateNum);
		fprintf(fp,"<�߰����� weight=\"%.3f\"/>\n",flr_param_tbl[i].para.weight);
		fprintf(fp,"<�߰���� cMat=\"%c\"/>\n",flr_param_tbl[i].para.cMat);
		fprintf(fp,"<�߰��� T=\"%.2f\"/>\n",flr_param_tbl[i].para.T);
		fprintf(fp,"<�߰�߶� A=\"%.2f\"/>\n",flr_param_tbl[i].para.A);
		fprintf(fp,"<�߰�ײ���� B=\"%.2f\"/>\n",flr_param_tbl[i].para.B);
		fprintf(fp,"<�߰����߶� C=\"%.2f\"/>\n",flr_param_tbl[i].para.C);
		fprintf(fp,"<�߰�ײ���� D=\"%.2f\"/>\n",flr_param_tbl[i].para.D);
		fprintf(fp,"<���ǿ�� E=\"%.2f\"/>\n",flr_param_tbl[i].para.E);
		fprintf(fp,"<�׶�λ���� F=\"%.2f\"/>\n",flr_param_tbl[i].para.F);
		fprintf(fp,"<�׶�λ���� G=\"%.2f\"/>\n",flr_param_tbl[i].para.G);
		fprintf(fp,"<��ֱ�� H=\"%.2f\"/>\n",flr_param_tbl[i].para.H);
		fprintf(fp,"</���Է�����Ϣ>\n");
	}
	fprintf(fp,"</���Է�������Ϣ>\n");
}
//��ȡ��׼������
void ReadStdPartLibFromBuffer(CBuffer &buffer,CTowerArchiveLibrary* pLoadToArchive,long version=0,long doc_type=0)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if( doc_type==1||doc_type==3
		||(doc_type==2&&version>0&&version<1000103)
		||(doc_type==4&&version>0&&version<1000303))
		return;	//֮ǰ�汾δ�����׼��
	int stdlib_version=0;
	if(  version==0||
		(doc_type==2&&version>=1010101)||	//LMA(V1.1.1.1)
		(doc_type==4&&version>=1020100)||	//LDS(V1.2.1.0)
		(doc_type==5&&version>=1020100))	//TDA(V1.2.1.0)
		stdlib_version=1;

	int i=0,j=0;
	if(pLoadToArchive!=NULL)
	{
		//���Ͳ���
		buffer.ReadInteger(&pLoadToArchive->rollend_param_N);
		pLoadToArchive->rollend_param_table.SetSize(pLoadToArchive->rollend_param_N);
		for(i=0;i<pLoadToArchive->rollend_param_N;i++)
		{
			buffer.ReadDouble(&pLoadToArchive->rollend_param_table[i].d);	
			buffer.ReadDouble(&pLoadToArchive->rollend_param_table[i].t);	
			buffer.ReadByte(&pLoadToArchive->rollend_param_table[i].cMaterial);	
			buffer.ReadInteger(&pLoadToArchive->rollend_param_table[i].iLevelMark);
			buffer.ReadString(pLoadToArchive->rollend_param_table[i].codeName,16);
			buffer.ReadByte(&pLoadToArchive->rollend_param_table[i].cTubeMat);
			buffer.ReadDouble(&pLoadToArchive->rollend_param_table[i].L);	
			buffer.ReadDouble(&pLoadToArchive->rollend_param_table[i].M);	
			buffer.ReadDouble(&pLoadToArchive->rollend_param_table[i].H);	
			buffer.ReadDouble(&pLoadToArchive->rollend_param_table[i].W);	
			buffer.ReadDouble(&pLoadToArchive->rollend_param_table[i].N);	
			buffer.ReadDouble(&pLoadToArchive->rollend_param_table[i].S);	
			buffer.ReadInteger(&pLoadToArchive->rollend_param_table[i].Thick);
			buffer.ReadDouble(&pLoadToArchive->rollend_param_table[i].theroy_weight);//��������
			//��˨�����������˨λ�ò���
			buffer.ReadInteger(&pLoadToArchive->rollend_param_table[i].iBoltRect);
			for(j=0;j<pLoadToArchive->rollend_param_table[i].iBoltRect;j++)
			{
				buffer.ReadInteger(&pLoadToArchive->rollend_param_table[i].ls_param[j].iBoltNum);
				buffer.ReadInteger(&pLoadToArchive->rollend_param_table[i].ls_param[j].iRow);
				buffer.ReadInteger(&pLoadToArchive->rollend_param_table[i].ls_param[j].iColumn);
				buffer.ReadInteger(&pLoadToArchive->rollend_param_table[i].ls_param[j].iBoltD);
				buffer.ReadDouble(&pLoadToArchive->rollend_param_table[i].ls_param[j].fBoltPosX);
				buffer.ReadDouble(&pLoadToArchive->rollend_param_table[i].ls_param[j].fBoltPosY);
				buffer.ReadDouble(&pLoadToArchive->rollend_param_table[i].ls_param[j].fHorzSpace);
				buffer.ReadDouble(&pLoadToArchive->rollend_param_table[i].ls_param[j].fVertSpace);
			}
		}
		//U�Ͳ���
		buffer.ReadInteger(&pLoadToArchive->uend_param_N);
		pLoadToArchive->uend_param_table.SetSize(pLoadToArchive->uend_param_N);
		for(i=0;i<pLoadToArchive->uend_param_N;i++)
		{
			buffer.ReadDouble(&pLoadToArchive->uend_param_table[i].d);	
			buffer.ReadDouble(&pLoadToArchive->uend_param_table[i].t);	
			buffer.ReadByte(&pLoadToArchive->uend_param_table[i].cMaterial);	
			buffer.ReadInteger(&pLoadToArchive->uend_param_table[i].iLevelMark);
			buffer.ReadString(pLoadToArchive->uend_param_table[i].codeName,16);
			buffer.ReadByte(&pLoadToArchive->uend_param_table[i].cTubeMat);
			buffer.ReadDouble(&pLoadToArchive->uend_param_table[i].L);	
			buffer.ReadDouble(&pLoadToArchive->uend_param_table[i].M);	
			buffer.ReadDouble(&pLoadToArchive->uend_param_table[i].H);	
			buffer.ReadDouble(&pLoadToArchive->uend_param_table[i].W);	
			buffer.ReadDouble(&pLoadToArchive->uend_param_table[i].N);	
			buffer.ReadDouble(&pLoadToArchive->uend_param_table[i].S);	
			buffer.ReadInteger(&pLoadToArchive->uend_param_table[i].Thick);
			buffer.ReadDouble(&pLoadToArchive->uend_param_table[i].theroy_weight);//��������
			//��˨�����������˨λ�ò���
			buffer.ReadInteger(&pLoadToArchive->uend_param_table[i].iBoltRect);
			for(j=0;j<pLoadToArchive->uend_param_table[i].iBoltRect;j++)
			{
				buffer.ReadInteger(&pLoadToArchive->uend_param_table[i].ls_param[j].iBoltNum);
				buffer.ReadInteger(&pLoadToArchive->uend_param_table[i].ls_param[j].iRow);
				buffer.ReadInteger(&pLoadToArchive->uend_param_table[i].ls_param[j].iColumn);
				buffer.ReadInteger(&pLoadToArchive->uend_param_table[i].ls_param[j].iBoltD);
				buffer.ReadDouble(&pLoadToArchive->uend_param_table[i].ls_param[j].fBoltPosX);
				buffer.ReadDouble(&pLoadToArchive->uend_param_table[i].ls_param[j].fBoltPosY);
				buffer.ReadDouble(&pLoadToArchive->uend_param_table[i].ls_param[j].fHorzSpace);
				buffer.ReadDouble(&pLoadToArchive->uend_param_table[i].ls_param[j].fVertSpace);
			}
		}
		//ʮ���Ͳ���
		buffer.ReadInteger(&pLoadToArchive->xend_param_N);
		pLoadToArchive->xend_param_table.SetSize(pLoadToArchive->xend_param_N);
		for(i=0;i<pLoadToArchive->xend_param_N;i++)
		{
			buffer.ReadDouble(&pLoadToArchive->xend_param_table[i].d);	
			buffer.ReadDouble(&pLoadToArchive->xend_param_table[i].t);	
			buffer.ReadByte(&pLoadToArchive->xend_param_table[i].cMaterial);	
			buffer.ReadInteger(&pLoadToArchive->xend_param_table[i].iLevelMark);
			buffer.ReadString(pLoadToArchive->xend_param_table[i].codeName,16);
			buffer.ReadString(pLoadToArchive->xend_param_table[i].datumPlatePartNo,16);
			buffer.ReadString(pLoadToArchive->xend_param_table[i].weldPlatePartNo,16);
			buffer.ReadString(pLoadToArchive->xend_param_table[i].wrapPlatePartNo,16);
			buffer.ReadByte(&pLoadToArchive->xend_param_table[i].cTubeMat);
			buffer.ReadDouble(&pLoadToArchive->xend_param_table[i].L);	
			buffer.ReadDouble(&pLoadToArchive->xend_param_table[i].M);	
			buffer.ReadDouble(&pLoadToArchive->xend_param_table[i].H);	
			buffer.ReadDouble(&pLoadToArchive->xend_param_table[i].W);	
			buffer.ReadDouble(&pLoadToArchive->xend_param_table[i].N);	
			buffer.ReadDouble(&pLoadToArchive->xend_param_table[i].S);	
			buffer.ReadInteger(&pLoadToArchive->xend_param_table[i].Thick);
			buffer.ReadDouble(&pLoadToArchive->xend_param_table[i].theroy_weight);
			buffer.ReadDouble(&pLoadToArchive->xend_param_table[i].A);
			buffer.ReadDouble(&pLoadToArchive->xend_param_table[i].R);
			//��˨�����������˨λ�ò���
			buffer.ReadInteger(&pLoadToArchive->xend_param_table[i].iBoltRect);
			for(j=0;j<pLoadToArchive->xend_param_table[i].iBoltRect;j++)
			{
				buffer.ReadInteger(&pLoadToArchive->xend_param_table[i].ls_param[j].iBoltNum);
				buffer.ReadInteger(&pLoadToArchive->xend_param_table[i].ls_param[j].iRow);
				buffer.ReadInteger(&pLoadToArchive->xend_param_table[i].ls_param[j].iColumn);
				buffer.ReadInteger(&pLoadToArchive->xend_param_table[i].ls_param[j].iBoltD);
				buffer.ReadDouble(&pLoadToArchive->xend_param_table[i].ls_param[j].fBoltPosX);
				buffer.ReadDouble(&pLoadToArchive->xend_param_table[i].ls_param[j].fBoltPosY);
				buffer.ReadDouble(&pLoadToArchive->xend_param_table[i].ls_param[j].fHorzSpace);
				buffer.ReadDouble(&pLoadToArchive->xend_param_table[i].ls_param[j].fVertSpace);
			}
		}
		if(stdlib_version<1)
		{
			//�����ԽӼ����
			buffer.ReadInteger(&pLoadToArchive->fl_couple_rule_N);
			pLoadToArchive->fl_couple_rule_table.SetSize(pLoadToArchive->fl_couple_rule_N);
			for(i=0;i<pLoadToArchive->fl_couple_rule_N;i++)
			{
				buffer.ReadDouble(&pLoadToArchive->fl_couple_rule_table[i].d);
				buffer.ReadDouble(&pLoadToArchive->fl_couple_rule_table[i].A);
				buffer.ReadDouble(&pLoadToArchive->fl_couple_rule_table[i].B);
				buffer.ReadDouble(&pLoadToArchive->fl_couple_rule_table[i].C);
				buffer.ReadDouble(&pLoadToArchive->fl_couple_rule_table[i].D);
			}
		}
		//�Ժ�����
		buffer.ReadInteger(&pLoadToArchive->fld_param_N);
		pLoadToArchive->fld_param_table.SetSize(pLoadToArchive->fld_param_N);
		for(i=0;i<pLoadToArchive->fld_param_N;i++)
		{
			FL_PARAM *pFl=&pLoadToArchive->fld_param_table[i];
			buffer.ReadDouble(&pLoadToArchive->fld_param_table[i].d);
			if(stdlib_version>=1)
				buffer.ReadDouble(&pLoadToArchive->fld_param_table[i].couple_d);
			buffer.ReadDouble(&pLoadToArchive->fld_param_table[i].t);
			buffer.ReadByte(&pLoadToArchive->fld_param_table[i].cLevelMark);
			buffer.ReadByte(&pLoadToArchive->fld_param_table[i].cMaterial);
			buffer.ReadString(pLoadToArchive->fld_param_table[i].codeName,16);
			buffer.ReadDouble(&pLoadToArchive->fld_param_table[i].A);
			buffer.ReadDouble(&pLoadToArchive->fld_param_table[i].B);
			buffer.ReadDouble(&pLoadToArchive->fld_param_table[i].D);
			buffer.ReadDouble(&pLoadToArchive->fld_param_table[i].K);
			buffer.ReadDouble(&pLoadToArchive->fld_param_table[i].L);
			buffer.ReadInteger(&pLoadToArchive->fld_param_table[i].nBoltNum); 
			buffer.ReadInteger(&pLoadToArchive->fld_param_table[i].nBoltD);
			buffer.ReadDouble(&pLoadToArchive->fld_param_table[i].C);
			buffer.ReadDouble(&pLoadToArchive->fld_param_table[i].H);
			buffer.ReadDouble(&pLoadToArchive->fld_param_table[i].N);
			buffer.ReadDouble(&pLoadToArchive->fld_param_table[i].S);
			buffer.ReadDouble(&pLoadToArchive->fld_param_table[i].H1);
			buffer.ReadDouble(&pLoadToArchive->fld_param_table[i].R);
			if( version==0||doc_type==5					//TDA
				||(doc_type==2&&version>=1000111)		//LMA V1.0.1.11
				||(doc_type==4&&version>=1000310))		//LDS V1.0.3.10
			{
				for(int j=0;j<5;j++)
				{
					buffer.ReadInteger(&pLoadToArchive->fld_param_table[i].group[j].t);
					buffer.ReadDouble(&pLoadToArchive->fld_param_table[i].group[j].weight);
				}
			}
			else 
			{
				memset(pLoadToArchive->fld_param_table[i].group,0,sizeof(FL_PARAM::THICK_GROUP)*5);
				buffer.ReadDouble(&pLoadToArchive->fld_param_table[i].group[0].weight);
			}
		}
		//ƽ������
		buffer.ReadInteger(&pLoadToArchive->flp_param_N);
		pLoadToArchive->flp_param_table.SetSize(pLoadToArchive->flp_param_N);
		for(i=0;i<pLoadToArchive->flp_param_N;i++)
		{
			buffer.ReadDouble(&pLoadToArchive->flp_param_table[i].d);
			if(stdlib_version>=1)
				buffer.ReadDouble(&pLoadToArchive->fld_param_table[i].couple_d);
			buffer.ReadDouble(&pLoadToArchive->flp_param_table[i].t);
			buffer.ReadByte(&pLoadToArchive->flp_param_table[i].cLevelMark);
			buffer.ReadByte(&pLoadToArchive->flp_param_table[i].cMaterial);
			buffer.ReadString(pLoadToArchive->flp_param_table[i].codeName,16);
			buffer.ReadDouble(&pLoadToArchive->flp_param_table[i].A);
			buffer.ReadDouble(&pLoadToArchive->flp_param_table[i].B);
			buffer.ReadDouble(&pLoadToArchive->flp_param_table[i].D);
			buffer.ReadDouble(&pLoadToArchive->flp_param_table[i].K);
			buffer.ReadDouble(&pLoadToArchive->flp_param_table[i].L);
			buffer.ReadInteger(&pLoadToArchive->flp_param_table[i].nBoltNum); 
			buffer.ReadInteger(&pLoadToArchive->flp_param_table[i].nBoltD);
			buffer.ReadDouble(&pLoadToArchive->flp_param_table[i].C);
			buffer.ReadDouble(&pLoadToArchive->flp_param_table[i].H);
			buffer.ReadDouble(&pLoadToArchive->flp_param_table[i].N);
			buffer.ReadDouble(&pLoadToArchive->flp_param_table[i].S);
			buffer.ReadDouble(&pLoadToArchive->flp_param_table[i].H1);
			buffer.ReadDouble(&pLoadToArchive->flp_param_table[i].R);
			buffer.ReadDouble(&pLoadToArchive->flp_param_table[i].group[0].weight);
		}
		if( version==0
			//||(doc_type==2&&version>=1020007)	//LMA V1.2.0.7	LDS��TDA���ڰ汾�п��ܲ��ܶ�дLMA V1.1.1.1�ļ� wjh-2013.11.06
			||(doc_type==2&&version>=1010101)	//LMA V1.1.1.1
			||(doc_type==4&&version>=1020007)	//LDS V1,2,0,7
			||(doc_type==5&&version>=1020007))	//TDA V1.2.0.7
		{	
			//���Է���
			buffer.ReadInteger(&pLoadToArchive->flg_param_N);
			pLoadToArchive->flg_param_tbl.SetSize(pLoadToArchive->flg_param_N);
			for(i=0;i<pLoadToArchive->flg_param_N;i++)
			{
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].d);
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].couple_d);
				buffer.ReadByte(&pLoadToArchive->flg_param_tbl[i].cLevelMark);
				buffer.ReadByte(&pLoadToArchive->flg_param_tbl[i].cMaterial);
				buffer.ReadString(pLoadToArchive->flg_param_tbl[i].codeName,16);
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].fInnerD);
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].fOuterD);
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].fBoltCirD);
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].fBoltHoleD);
				buffer.ReadInteger(&pLoadToArchive->flg_param_tbl[i].nBoltNum);
				buffer.ReadInteger(&pLoadToArchive->flg_param_tbl[i].nBoltD);
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].fThick);
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].fH);
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].theroy_weight);
				buffer.ReadInteger(&pLoadToArchive->flg_param_tbl[i].nRibPlateNum);
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].para.weight);
				buffer.ReadByte(&pLoadToArchive->flg_param_tbl[i].para.cMat);
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].para.T);
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].para.A);
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].para.B);
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].para.C);
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].para.D);
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].para.E);
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].para.F);
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].para.G);
				buffer.ReadDouble(&pLoadToArchive->flg_param_tbl[i].para.H);
			}
			//���Է���
			buffer.ReadInteger(&pLoadToArchive->flr_param_N);
			pLoadToArchive->flr_param_tbl.SetSize(pLoadToArchive->flr_param_N);
			for(i=0;i<pLoadToArchive->flr_param_N;i++)
			{
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].d);
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].couple_d);
				buffer.ReadByte(&pLoadToArchive->flr_param_tbl[i].cLevelMark);
				buffer.ReadByte(&pLoadToArchive->flr_param_tbl[i].cMaterial);
				buffer.ReadString(pLoadToArchive->flr_param_tbl[i].codeName,16);
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].fInnerD);
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].fOuterD);
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].fBoltCirD);
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].fBoltHoleD);
				buffer.ReadInteger(&pLoadToArchive->flr_param_tbl[i].nBoltNum);
				buffer.ReadInteger(&pLoadToArchive->flr_param_tbl[i].nBoltD);
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].fThick);
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].fH);
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].theroy_weight);
				buffer.ReadInteger(&pLoadToArchive->flr_param_tbl[i].nRibPlateNum);
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].para.weight);
				buffer.ReadByte(&pLoadToArchive->flr_param_tbl[i].para.cMat);
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].para.T);
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].para.A);
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].para.B);
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].para.C);
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].para.D);
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].para.E);
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].para.F);
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].para.G);
				buffer.ReadDouble(&pLoadToArchive->flr_param_tbl[i].para.H);
			}
		}
		return;
	}
	//else
	// ��������, ...
	//���Ͳ���
	buffer.ReadInteger(&rollend_param_N);
	for(i=0;i<rollend_param_N;i++)
	{
		buffer.ReadDouble(&rollend_param_table[i].d);	
		buffer.ReadDouble(&rollend_param_table[i].t);	
		buffer.ReadByte(&rollend_param_table[i].cMaterial);	
		buffer.ReadInteger(&rollend_param_table[i].iLevelMark);
		buffer.ReadString(rollend_param_table[i].codeName,16);
		buffer.ReadByte(&rollend_param_table[i].cTubeMat);
		buffer.ReadDouble(&rollend_param_table[i].L);	
		buffer.ReadDouble(&rollend_param_table[i].M);	
		buffer.ReadDouble(&rollend_param_table[i].H);	
		buffer.ReadDouble(&rollend_param_table[i].W);	
		buffer.ReadDouble(&rollend_param_table[i].N);	
		buffer.ReadDouble(&rollend_param_table[i].S);	
		buffer.ReadInteger(&rollend_param_table[i].Thick);
		buffer.ReadDouble(&rollend_param_table[i].theroy_weight);//��������
		//��˨�����������˨λ�ò���
		buffer.ReadInteger(&rollend_param_table[i].iBoltRect);
		for(j=0;j<rollend_param_table[i].iBoltRect;j++)
		{
			buffer.ReadInteger(&rollend_param_table[i].ls_param[j].iBoltNum);
			buffer.ReadInteger(&rollend_param_table[i].ls_param[j].iRow);
			buffer.ReadInteger(&rollend_param_table[i].ls_param[j].iColumn);
			buffer.ReadInteger(&rollend_param_table[i].ls_param[j].iBoltD);
			buffer.ReadDouble(&rollend_param_table[i].ls_param[j].fBoltPosX);
			buffer.ReadDouble(&rollend_param_table[i].ls_param[j].fBoltPosY);
			buffer.ReadDouble(&rollend_param_table[i].ls_param[j].fHorzSpace);
			buffer.ReadDouble(&rollend_param_table[i].ls_param[j].fVertSpace);
		}
	}
	//U�Ͳ���
	buffer.ReadInteger(&uend_param_N);
	for(i=0;i<uend_param_N;i++)
	{
		buffer.ReadDouble(&uend_param_table[i].d);	
		buffer.ReadDouble(&uend_param_table[i].t);	
		buffer.ReadByte(&uend_param_table[i].cMaterial);	
		buffer.ReadInteger(&uend_param_table[i].iLevelMark);
		buffer.ReadString(uend_param_table[i].codeName,16);
		buffer.ReadByte(&uend_param_table[i].cTubeMat);
		buffer.ReadDouble(&uend_param_table[i].L);	
		buffer.ReadDouble(&uend_param_table[i].M);	
		buffer.ReadDouble(&uend_param_table[i].H);	
		buffer.ReadDouble(&uend_param_table[i].W);	
		buffer.ReadDouble(&uend_param_table[i].N);	
		buffer.ReadDouble(&uend_param_table[i].S);	
		buffer.ReadInteger(&uend_param_table[i].Thick);
		buffer.ReadDouble(&uend_param_table[i].theroy_weight);//��������
		//��˨�����������˨λ�ò���
		buffer.ReadInteger(&uend_param_table[i].iBoltRect);
		for(j=0;j<uend_param_table[i].iBoltRect;j++)
		{
			buffer.ReadInteger(&uend_param_table[i].ls_param[j].iBoltNum);
			buffer.ReadInteger(&uend_param_table[i].ls_param[j].iRow);
			buffer.ReadInteger(&uend_param_table[i].ls_param[j].iColumn);
			buffer.ReadInteger(&uend_param_table[i].ls_param[j].iBoltD);
			buffer.ReadDouble(&uend_param_table[i].ls_param[j].fBoltPosX);
			buffer.ReadDouble(&uend_param_table[i].ls_param[j].fBoltPosY);
			buffer.ReadDouble(&uend_param_table[i].ls_param[j].fHorzSpace);
			buffer.ReadDouble(&uend_param_table[i].ls_param[j].fVertSpace);
		}
	}
	//ʮ���Ͳ���
	buffer.ReadInteger(&xend_param_N);
	for(i=0;i<xend_param_N;i++)
	{
		buffer.ReadDouble(&xend_param_table[i].d);	
		buffer.ReadDouble(&xend_param_table[i].t);	
		buffer.ReadByte(&xend_param_table[i].cMaterial);	
		buffer.ReadInteger(&xend_param_table[i].iLevelMark);
		buffer.ReadString(xend_param_table[i].codeName,16);
		buffer.ReadString(xend_param_table[i].datumPlatePartNo,16);
		buffer.ReadString(xend_param_table[i].weldPlatePartNo,16);
		buffer.ReadString(xend_param_table[i].wrapPlatePartNo,16);
		buffer.ReadByte(&xend_param_table[i].cTubeMat);
		buffer.ReadDouble(&xend_param_table[i].L);	
		buffer.ReadDouble(&xend_param_table[i].M);	
		buffer.ReadDouble(&xend_param_table[i].H);	
		buffer.ReadDouble(&xend_param_table[i].W);	
		buffer.ReadDouble(&xend_param_table[i].N);	
		buffer.ReadDouble(&xend_param_table[i].S);	
		buffer.ReadInteger(&xend_param_table[i].Thick);
		buffer.ReadDouble(&xend_param_table[i].theroy_weight);
		buffer.ReadDouble(&xend_param_table[i].A);
		buffer.ReadDouble(&xend_param_table[i].R);
		//��˨�����������˨λ�ò���
		buffer.ReadInteger(&xend_param_table[i].iBoltRect);
		for(j=0;j<xend_param_table[i].iBoltRect;j++)
		{
			buffer.ReadInteger(&xend_param_table[i].ls_param[j].iBoltNum);
			buffer.ReadInteger(&xend_param_table[i].ls_param[j].iRow);
			buffer.ReadInteger(&xend_param_table[i].ls_param[j].iColumn);
			buffer.ReadInteger(&xend_param_table[i].ls_param[j].iBoltD);
			buffer.ReadDouble(&xend_param_table[i].ls_param[j].fBoltPosX);
			buffer.ReadDouble(&xend_param_table[i].ls_param[j].fBoltPosY);
			buffer.ReadDouble(&xend_param_table[i].ls_param[j].fHorzSpace);
			buffer.ReadDouble(&xend_param_table[i].ls_param[j].fVertSpace);
		}
	}
	if(stdlib_version<1)
	{	//�����ԽӼ����
		buffer.ReadInteger(&fl_couple_rule_N);
		for(i=0;i<fl_couple_rule_N;i++)
		{
			buffer.ReadDouble(&fl_couple_rule_table[i].d);
			buffer.ReadDouble(&fl_couple_rule_table[i].A);
			buffer.ReadDouble(&fl_couple_rule_table[i].B);
			buffer.ReadDouble(&fl_couple_rule_table[i].C);
			buffer.ReadDouble(&fl_couple_rule_table[i].D);
		}
	}
	//�Ժ�����
	buffer.ReadInteger(&fld_param_N);
	for(i=0;i<fld_param_N;i++)
	{
		buffer.ReadDouble(&fld_param_table[i].d);
		if(stdlib_version>=1)
			buffer.ReadDouble(&fld_param_table[i].couple_d);
		buffer.ReadDouble(&fld_param_table[i].t);
		buffer.ReadByte(&fld_param_table[i].cLevelMark);
		buffer.ReadByte(&fld_param_table[i].cMaterial);
		buffer.ReadString(fld_param_table[i].codeName,16);
		buffer.ReadDouble(&fld_param_table[i].A);
		buffer.ReadDouble(&fld_param_table[i].B);
		buffer.ReadDouble(&fld_param_table[i].D);
		buffer.ReadDouble(&fld_param_table[i].K);
		buffer.ReadDouble(&fld_param_table[i].L);
		buffer.ReadInteger(&fld_param_table[i].nBoltNum); 
		buffer.ReadInteger(&fld_param_table[i].nBoltD);
		buffer.ReadDouble(&fld_param_table[i].C);
		buffer.ReadDouble(&fld_param_table[i].H);
		buffer.ReadDouble(&fld_param_table[i].N);
		buffer.ReadDouble(&fld_param_table[i].S);
		buffer.ReadDouble(&fld_param_table[i].H1);
		buffer.ReadDouble(&fld_param_table[i].R);
		if( version==0||doc_type==5					//TDA
			||(doc_type==2&&version>=1000111)		//LMA V1.0.1.11
			||(doc_type==4&&version>=1000310))		//LDS V1.0.3.10
		{
			for(int j=0;j<5;j++)
			{
				buffer.ReadInteger(&fld_param_table[i].group[j].t);
				buffer.ReadDouble(&fld_param_table[i].group[j].weight);
			}
		}
		else 
		{
			memset(fld_param_table[i].group,0,sizeof(FL_PARAM::THICK_GROUP)*5);
			buffer.ReadDouble(&fld_param_table[i].group[0].weight);
		}
	}
	//ƽ������
	buffer.ReadInteger(&flp_param_N);
	for(i=0;i<flp_param_N;i++)
	{
		buffer.ReadDouble(&flp_param_table[i].d);
		if(stdlib_version>=1)
			buffer.ReadDouble(&flp_param_table[i].couple_d);
		buffer.ReadDouble(&flp_param_table[i].t);
		buffer.ReadByte(&flp_param_table[i].cLevelMark);
		buffer.ReadByte(&flp_param_table[i].cMaterial);
		buffer.ReadString(flp_param_table[i].codeName,16);
		buffer.ReadDouble(&flp_param_table[i].A);
		buffer.ReadDouble(&flp_param_table[i].B);
		buffer.ReadDouble(&flp_param_table[i].D);
		buffer.ReadDouble(&flp_param_table[i].K);
		buffer.ReadDouble(&flp_param_table[i].L);
		buffer.ReadInteger(&flp_param_table[i].nBoltNum); 
		buffer.ReadInteger(&flp_param_table[i].nBoltD);
		buffer.ReadDouble(&flp_param_table[i].C);
		buffer.ReadDouble(&flp_param_table[i].H);
		buffer.ReadDouble(&flp_param_table[i].N);
		buffer.ReadDouble(&flp_param_table[i].S);
		buffer.ReadDouble(&flp_param_table[i].H1);
		buffer.ReadDouble(&flp_param_table[i].R);
		buffer.ReadDouble(&flp_param_table[i].group[0].weight);
	}
	if( version==0
		//||(doc_type==2&&version>=1020007)	//LMA V1.2.0.7	LDS��TDA���ڰ汾�п��ܲ��ܶ�дLMA V1.1.1.1�ļ� wjh-2013.11.06
		||(doc_type==2&&version>=1010101)	//LMA V1.1.1.1
		||(doc_type==4&&version>=1020007)	//LDS V1,2,0,7
		||(doc_type==5&&version>=1020007))	//TDA V1.2.0.7
	{	
		//���Է���
		buffer.ReadInteger(&flg_param_N);
		for(i=0;i<flg_param_N;i++)
		{
			buffer.ReadDouble(&flg_param_tbl[i].d);
			buffer.ReadDouble(&flg_param_tbl[i].couple_d);
			buffer.ReadByte(&flg_param_tbl[i].cLevelMark);
			buffer.ReadByte(&flg_param_tbl[i].cMaterial);
			buffer.ReadString(flg_param_tbl[i].codeName,16);
			buffer.ReadDouble(&flg_param_tbl[i].fInnerD);
			buffer.ReadDouble(&flg_param_tbl[i].fOuterD);
			buffer.ReadDouble(&flg_param_tbl[i].fBoltCirD);
			buffer.ReadDouble(&flg_param_tbl[i].fBoltHoleD);
			buffer.ReadInteger(&flg_param_tbl[i].nBoltNum);
			buffer.ReadInteger(&flg_param_tbl[i].nBoltD);
			buffer.ReadDouble(&flg_param_tbl[i].fThick);
			buffer.ReadDouble(&flg_param_tbl[i].fH);
			buffer.ReadDouble(&flg_param_tbl[i].theroy_weight);
			buffer.ReadInteger(&flg_param_tbl[i].nRibPlateNum);
			buffer.ReadDouble(&flg_param_tbl[i].para.weight);
			buffer.ReadByte(&flg_param_tbl[i].para.cMat);
			buffer.ReadDouble(&flg_param_tbl[i].para.T);
			buffer.ReadDouble(&flg_param_tbl[i].para.A);
			buffer.ReadDouble(&flg_param_tbl[i].para.B);
			buffer.ReadDouble(&flg_param_tbl[i].para.C);
			buffer.ReadDouble(&flg_param_tbl[i].para.D);
			buffer.ReadDouble(&flg_param_tbl[i].para.E);
			buffer.ReadDouble(&flg_param_tbl[i].para.F);
			buffer.ReadDouble(&flg_param_tbl[i].para.G);
			buffer.ReadDouble(&flg_param_tbl[i].para.H);
		}
		//���Է���
		buffer.ReadInteger(&flr_param_N);
		for(i=0;i<flr_param_N;i++)
		{
			buffer.ReadDouble(&flr_param_tbl[i].d);
			buffer.ReadDouble(&flr_param_tbl[i].couple_d);
			buffer.ReadByte(&flr_param_tbl[i].cLevelMark);
			buffer.ReadByte(&flr_param_tbl[i].cMaterial);
			buffer.ReadString(flr_param_tbl[i].codeName,16);
			buffer.ReadDouble(&flr_param_tbl[i].fInnerD);
			buffer.ReadDouble(&flr_param_tbl[i].fOuterD);
			buffer.ReadDouble(&flr_param_tbl[i].fBoltCirD);
			buffer.ReadDouble(&flr_param_tbl[i].fBoltHoleD);
			buffer.ReadInteger(&flr_param_tbl[i].nBoltNum);
			buffer.ReadInteger(&flr_param_tbl[i].nBoltD);
			buffer.ReadDouble(&flr_param_tbl[i].fThick);
			buffer.ReadDouble(&flr_param_tbl[i].fH);
			buffer.ReadDouble(&flr_param_tbl[i].theroy_weight);
			buffer.ReadInteger(&flr_param_tbl[i].nRibPlateNum);
			buffer.ReadDouble(&flr_param_tbl[i].para.weight);
			buffer.ReadByte(&flr_param_tbl[i].para.cMat);
			buffer.ReadDouble(&flr_param_tbl[i].para.T);
			buffer.ReadDouble(&flr_param_tbl[i].para.A);
			buffer.ReadDouble(&flr_param_tbl[i].para.B);
			buffer.ReadDouble(&flr_param_tbl[i].para.C);
			buffer.ReadDouble(&flr_param_tbl[i].para.D);
			buffer.ReadDouble(&flr_param_tbl[i].para.E);
			buffer.ReadDouble(&flr_param_tbl[i].para.F);
			buffer.ReadDouble(&flr_param_tbl[i].para.G);
			buffer.ReadDouble(&flr_param_tbl[i].para.H);
		}
	}
}

PROGRESS_CONTEXT::PROGRESS_CONTEXT(){
	title=NULL;
	ciStartProgressNumber=0;
	ciToProgressNumber=100;
}
int PROGRESS_CONTEXT::Mapping(double percent)//��0~100�İٷֱ�ӳ��ת����ciStartProgressNumber~ciToProgressNumber����
{
	if(ciStartProgressNumber==0&&ciToProgressNumber==100)
		return ftoi(percent);
	else if(ciToProgressNumber-ciStartProgressNumber<=0)
		return ftoi(percent);	//������ֹ�㲻��������ת��
	int step=ftoi((ciToProgressNumber-ciStartProgressNumber)*percent/100.0);
	return ciStartProgressNumber+step;
}
#include ".\WeldPlateNodeGroup.h"
//���������ֹ��Ϻ��Ӹְ��װ��Ի��
BOOL WELD_PLATE_ITEM::IsWeldPtOutWeldLine(IModel* pModel)
{
	CLDSLineTube* pDatumTube=(CLDSLineTube*)pModel->FromPartHandle(m_hBaseTube,CLS_LINETUBE);
	if(pDatumTube==NULL)
		return FALSE;
	GEPOINT vStdTubeLenVec = pDatumTube->xPosEnd - pDatumTube->xPosStart;
	normalize(vStdTubeLenVec);
	//�޳��Ի�����ĸְ�,��ʼ����׼�Ի��ƫ��λ��
	bool blInit1stAlongOffset=false;
	for (PLATE* pItem = xarrPlates.GetFirst(); pItem; pItem=xarrPlates.GetNext())
	{
		if (pItem->pPlate == NULL)
		{
			xarrPlates.DeleteCursor();
			continue;
		}
		BOOL bOutWeldLine = FALSE;
		f3dPoint vLocalTubeVec = pItem->pPlate->ucs.TransVToCS(vStdTubeLenVec);
		PROFILE_VERTEX *pPrevVertex = pItem->pPlate->vertex_list.GetTail(), *pVertex = NULL;
		for (pVertex = pItem->pPlate->vertex_list.GetFirst(); pVertex; pVertex = pItem->pPlate->vertex_list.GetNext())
		{
			GEPOINT vec = (pVertex->vertex - pPrevVertex->vertex);
			double dfEdgeLen=vec.mod();
			if(dfEdgeLen>EPS2)
				vec/=dfEdgeLen;
			if (!pPrevVertex->m_bWeldEdge || pPrevVertex->ciWeldFlowDirection==0 ||fabs(vec*vLocalTubeVec) < EPS_COS2)
			{	//���ظֹ����ߵĺ��ӱ�
				pPrevVertex = pVertex;
				continue;
			}
			//���ҵ����Ӷ�λ��׼��
			pItem->liEdgeLength=ftoi(dfEdgeLen);
			pItem->weld_line.startPt = pPrevVertex->vertex;
			pItem->weld_line.endPt = pVertex->vertex;
			int sign=vec.x*vLocalTubeVec.x + vec.y*vLocalTubeVec.y<0?-1:1;
			pItem->vRegWeldVec = sign*vec;
			f3dPoint perp, loca_pt, datum_org;
			SnapPerp(&perp, pItem->weld_line, datum_org);
			loca_pt = perp + pItem->vRegWeldVec * pPrevVertex->liLocationAlongOffset;
			if (pItem->weld_line.PtInLine(loca_pt) <= 0)
			{	//�Ի���ں��ӱ����
				if (!blInit1stAlongOffset)//m_liAmendOffsetY == -1)				
				{	//��¼��׼�Ի��ƫ��λ��
					blInit1stAlongOffset=true;
					GEPOINT mid_pt = 0.5*(pItem->weld_line.startPt + pItem->weld_line.endPt);
					GEPOINT off_vec = mid_pt - perp;
					m_liAmendOffsetY = ftoi(off_vec*pItem->vRegWeldVec);//ftoi(DISTANCE(mid_pt,perp)); 
					if (pItem->liEdgeLength>400)	//����Բ��Ϊ����
						m_liAmendOffsetY-=m_liAmendOffsetY%100;
					else if(pItem->liEdgeLength>200)
						m_liAmendOffsetY-=m_liAmendOffsetY%50;
					//if (off_vec.x*vLocalTubeVec.x + off_vec.y*vLocalTubeVec.y < 0)
					//	m_liAmendOffsetY *= -1;
				}
				bOutWeldLine = TRUE;
			}
			pPrevVertex = pVertex;
			break;
		}
		if (!bOutWeldLine)
			xarrPlates.DeleteCursor();
	}
	xarrPlates.Clean();
	//�����Ի��ƫ��λ��
	if (xarrPlates.GetNodeNum() > 0)
	{	//��һ��������ĶԻ��ƫ��λ��Ϊ��׼��У������Ƿ���Ҫ���ζԻ��ƫ�ƣ��������趨��Ӧ����ƫ���� wjh-2019.11.1
		for (PLATE* pItem = xarrPlates.GetFirst(); pItem; pItem = xarrPlates.GetNext())
		{
			f3dPoint perp, loca_pt, datum_org;
			SnapPerp(&perp, pItem->weld_line, datum_org);
			loca_pt = perp + pItem->vRegWeldVec * m_liAmendOffsetY;
			if (pItem->weld_line.PtInLine(loca_pt) <= 0)
			{	//���ж�������
				GEPOINT mid_pt = 0.5*(pItem->weld_line.startPt + pItem->weld_line.endPt);
				GEPOINT off_vec = mid_pt - perp;
				long liOffY = ftoi(off_vec*pItem->vRegWeldVec);//ftoi(DISTANCE(mid_pt,perp));
				//if (off_vec.x*pItem->vRegWeldVec.x + off_vec.y*pItem->vRegWeldVec.y < 0)
				//	liOffY *= -1;
				pItem->liSecondAmendOffsetY = liOffY-m_liAmendOffsetY;
				if(pItem->liEdgeLength>200)
					pItem->liSecondAmendOffsetY-=pItem->liSecondAmendOffsetY%50;
			}
		}
		return TRUE;
	}
	return FALSE;
}
void CTower::AmendPlateWeldLocationY()
{
	//��ȡ�ֹ��ϵĺ��Ӱ�
	//CHashList<WELD_PLATE_ITEM> hashWeldPlateItem;
	CMapList<WELD_PLATE_ITEM> hashWeldPlateItem;
	for (CLDSPart* pPart = Parts.GetFirst(); pPart; pPart = Parts.GetNext())
	{
		if (pPart->GetClassTypeId() != CLS_PLATE)
			continue;
		CLDSPlate *pPlank = (CLDSPlate*)pPart;
		CLDSLineTube* pDatumTube = (CLDSLineTube*)FromPartHandle(pPlank->m_hPartWeldParent, CLS_LINETUBE);
		if (pDatumTube == NULL)
			continue;
		CLDSNode* pDatumNode = FromNodeHandle(pPlank->designInfo.m_hBaseNode);
		if (pDatumNode == NULL)
			continue;
		//�˴����ʺ�ʹ��SUBID���������ʱ�ᵼ��"����ѳ�����ʶ���鼫��" wht 19-07-19
		//SUBID16 key(pDatumNode->handle,pDatumTube->handle);
		WELD_PLATE_ITEM* pItem = hashWeldPlateItem.GetValue(pDatumNode->handle, pDatumTube->handle);
		if (pItem == NULL)
			pItem = hashWeldPlateItem.Add(pDatumNode->handle, pDatumTube->handle);
		pItem->m_hBaseNode = pDatumNode->handle;
		pItem->m_hBaseTube = pDatumTube->handle;
		pItem->xarrPlates.append(WELD_PLATE_ITEM::PLATE(pPlank));
	}
	//�����޸����Ӹְ�ĶԻ��ƫ����
	for (WELD_PLATE_ITEM* pItem = hashWeldPlateItem.GetFirst(); pItem; pItem = hashWeldPlateItem.GetNext())
	{
		if (!pItem->IsWeldPtOutWeldLine(this))
			continue;
		WELD_PLATE_ITEM::PLATE* pPlateItem = NULL;
		for(pPlateItem=pItem->xarrPlates.GetFirst();pPlateItem;pPlateItem=pItem->xarrPlates.GetNext())
		{
			CLDSPlate* pPlank=pPlateItem->pPlate;
			PROFILE_VERTEX *pPrevVertex = pPlank->vertex_list.GetTail(), *pVertex = NULL;
			for (pVertex = pPlank->vertex_list.GetFirst(); pVertex; pVertex = pPlank->vertex_list.GetNext())
			{
				f3dPoint weld_vec = (pVertex->vertex - pPrevVertex->vertex).normalized();
				if (pPrevVertex&&pPrevVertex->m_bWeldEdge&&pPrevVertex->ciWeldFlowDirection !=0 && fabs(weld_vec*pPlateItem->vRegWeldVec) > EPS_COS2)
				{
					double old_local_pt_y = pPrevVertex->local_point_y;
					pPrevVertex->local_point_y = pItem->m_liAmendOffsetY + pPlateItem->liSecondAmendOffsetY;
					if (fabs(old_local_pt_y - pPrevVertex->local_point_y) > EPS2)
						logerr.Log("%s(0x%X),�Ի����%.1f�޶�Ϊ%.1f", pPlank->Label, pPlank->handle, old_local_pt_y, pPrevVertex->local_point_y);
					break;
				}
				pPrevVertex = pVertex;
			}
		}
	}
}
BOOL CTower::SetDesignItem(DWORD dwKey,DESIGN_PARAM_ITEM& param,BOOL bModify/*=FALSE*/)
{
	DESIGN_PARAM_ITEM *pItem=hashParams.Add(dwKey);
	if(pItem==NULL)
		return FALSE;
	pItem->dataType=param.dataType;
	pItem->value.fValue=param.value.fValue;
	pItem->uiCategoryId=param.uiCategoryId;
	pItem->sNotes=param.sNotes;
	pItem->bModify=bModify;
	return TRUE;
}
BOOL CTower::SetDesignItemValue(DWORD dwKey,double fValue,int iDataType/*=0*/,BOOL bModify/*=FALSE*/)
{
	DESIGN_PARAM_ITEM *pItem=hashParams.GetValue(dwKey);
	if(pItem==NULL)
		pItem=hashParams.Add(dwKey);
	if(pItem)
	{
		pItem->dataType=iDataType;
		if(iDataType==0)
			pItem->value.fValue=fValue;
		else if(iDataType==1)
			pItem->value.hPart=ftoi(fValue);
		else if(iDataType==2)
			pItem->value.iType=ftoi(fValue);
		pItem->bModify=bModify;
	}
	return TRUE;
}
#include "..\StdComTempl\IStdComTempl.h"
bool InitTowerModelTmplInfo(CTower* pTower)
{
	const UINT UPPER_ARM_ID=3,MID_ARM_ID=4,BTM_ARM_ID=5,UPPER_ARM_ID_LEVEL1=6,UPPER_ARM_ID_LEVEL2=7;
	//��һ�㵼���߸��Ϻᵣ
	DESIGN_PARAM_ITEM* pParam=pTower->hashParams.Add(KEY4C("X1"));
	pParam=pTower->hashParams.Add(KEY4C("Z0"));
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=3500;
	pParam->uiCategoryId=0;	//��̨���ز���������ʾ

	pParam=pTower->hashParams.Add(KEY4C("X1"));
	pParam->sLabel="X1";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=10400;
	pParam->uiCategoryId=UPPER_ARM_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("X2"));
	pParam->sLabel="X2";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=7700;
	pParam->uiCategoryId=UPPER_ARM_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("X3"));
	pParam->sLabel="X3";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=800;
	pParam->uiCategoryId=UPPER_ARM_ID_LEVEL2;
	pParam=pTower->hashParams.Add(KEY4C("Y1"));
	pParam->sLabel="Y1";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=600;
	pParam->uiCategoryId=UPPER_ARM_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("Y2"));
	pParam->sLabel="Y2";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=1200;
	pParam->uiCategoryId=UPPER_ARM_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("Z1"));
	pParam->sLabel="Z1";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=2300;
	pParam->uiCategoryId=UPPER_ARM_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("Z2"));
	pParam->sLabel="Z2";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=600;
	pParam->uiCategoryId=UPPER_ARM_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("Z3"));
	pParam->sLabel="Z3";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=600;
	pParam->uiCategoryId=UPPER_ARM_ID_LEVEL2;
	//�ڶ��㵼�ߺᵣ
	pParam=pTower->hashParams.Add(KEY4C("XB"));
	pParam->sLabel="XB";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=10100;
	pParam->uiCategoryId=MID_ARM_ID;
	pParam=pTower->hashParams.Add(KEY4C("YB"));
	pParam->sLabel="YB";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=1200;
	pParam->uiCategoryId=MID_ARM_ID;
	pParam=pTower->hashParams.Add(KEY4C("ZB"));
	pParam->sLabel="ZB";
	//pParam->sLabel="���е��߲��";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=12400;
	pParam->uiCategoryId=MID_ARM_ID;
	//�����㵼�ߺᵣ
	pParam=pTower->hashParams.Add(KEY4C("XC"));
	pParam->sLabel="XC";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=8350;
	pParam->uiCategoryId=BTM_ARM_ID;
	pParam=pTower->hashParams.Add(KEY4C("YC"));
	pParam->sLabel="YC";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=1400;
	pParam->uiCategoryId=BTM_ARM_ID;
	pParam=pTower->hashParams.Add(KEY4C("ZC"));
	pParam->sLabel="ZC";
	//pParam->sLabel="���µ��߲��";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=11200;
	pParam->uiCategoryId=BTM_ARM_ID;
	CATLOG_ITEM* pCatgItem=pTower->hashCatlogs.Add(1);
	pCatgItem->name="������Ʋ���";
	pCatgItem=pTower->hashCatlogs.Add(2);
	pCatgItem->name="�ֲ�ᵣ����";
	pCatgItem=pTower->hashCatlogs.Add(UPPER_ARM_ID);
	pCatgItem->name="�����߸��Ϻᵣ";
	pCatgItem->idParentCatg=2;
	pCatgItem=pTower->hashCatlogs.Add(MID_ARM_ID);
	pCatgItem->name="�е��ߺᵣ";
	pCatgItem->idParentCatg=2;
	pCatgItem=pTower->hashCatlogs.Add(BTM_ARM_ID);
	pCatgItem->name="�µ��ߺᵣ";
	pCatgItem->idParentCatg=2;

	pCatgItem=pTower->hashCatlogs.Add(UPPER_ARM_ID_LEVEL1);
	pCatgItem->idParentCatg=UPPER_ARM_ID;
	pCatgItem->name="һ����Ʋ���";
	pCatgItem=pTower->hashCatlogs.Add(UPPER_ARM_ID_LEVEL2);
	pCatgItem->idParentCatg=UPPER_ARM_ID;
	pCatgItem->name="�������β���";
	//�ڵ�������ϵ
	//��һ�������ߺᵣ
	SLAVE_NODE* pSlaveNode=pTower->hashSlaveNodes.Append(0x2A);	//N30�ڵ�
	pSlaveNode->cFlag=0x07;
	pSlaveNode->cbMirFlag=0x07;
	StrCopy(pSlaveNode->posx_expr,"X1",20);
	StrCopy(pSlaveNode->posy_expr,"Y1",20);
	StrCopy(pSlaveNode->posz_expr,"Z0-Z2-Z1",20);
	pSlaveNode=pTower->hashSlaveNodes.Append(0x2E);	//N70�ڵ�
	pSlaveNode->cFlag=0x01;
	pSlaveNode->cbMirFlag=0x07;
	StrCopy(pSlaveNode->posx_expr,"X1-X3",20);
	pSlaveNode=pTower->hashSlaveNodes.Append(0x3E);	//N50
	pSlaveNode->cFlag=0x05;
	pSlaveNode->cbMirFlag=0x07;
	StrCopy(pSlaveNode->posx_expr,"X1-X3",20);
	StrCopy(pSlaveNode->posz_expr,"Z0-Z2-Z1-Z3",20);
	//pSlaveNode=pTower->hashSlaveNodes.Append(0x3E);	//N60
	//pSlaveNode->cFlag=0x05;
	//pSlaveNode->cbMirFlag=0x07;
	//StrCopy(pSlaveNode->posx_expr,"X1-X3",20);
	//StrCopy(pSlaveNode->posz_expr,"-Z2-Z1-Z3",20);
	pSlaveNode=pTower->hashSlaveNodes.Append(0x52);	//N40
	pSlaveNode->cFlag=0x07;
	pSlaveNode->cbMirFlag=0x07;
	StrCopy(pSlaveNode->posx_expr,"X2",20);
	StrCopy(pSlaveNode->posy_expr,"Y2",20);
	StrCopy(pSlaveNode->posz_expr,"Z0-Z2",20);
	pSlaveNode=pTower->hashSlaveNodes.Append(0x4e);	//N80
	pSlaveNode->cFlag=0x01;
	pSlaveNode->cbMirFlag=0x07;
	StrCopy(pSlaveNode->posz_expr,"X2-X4",20);
	pSlaveNode=pTower->hashSlaveNodes.Append(0x46);	//N90
	pSlaveNode->cFlag=0x04;
	pSlaveNode->cbMirFlag=0x07;
	StrCopy(pSlaveNode->posz_expr,"Z0-Z2",20);
	//�ڶ����ߺᵣ
	pSlaveNode=pTower->hashSlaveNodes.Append(0x56);	//N30
	pSlaveNode->cFlag=0x07;
	pSlaveNode->cbMirFlag=0x07;
	StrCopy(pSlaveNode->posx_expr,"XB",20);
	StrCopy(pSlaveNode->posy_expr,"YB",20);
	StrCopy(pSlaveNode->posz_expr,"Z0-Z2+ZB",20);
	//�������ߺᵣ
	pSlaveNode=pTower->hashSlaveNodes.Append(0x5A);	//N30
	pSlaveNode->cFlag=0x07;
	pSlaveNode->cbMirFlag=0x07;
	StrCopy(pSlaveNode->posx_expr,"XC",20);
	StrCopy(pSlaveNode->posy_expr,"YC",20);
	StrCopy(pSlaveNode->posz_expr,"Z0-Z2+ZB+ZC",20);
	//
	return true;
}
bool InitTowerModelTmplInfo_ZBC(CTower* pTower)
{
	const UINT HEAD_ID_LEVEL1=1,HEAD_ID_LEVEL2=2,TRUNK_ID_LEVEL=3;
	//��һ�㵼���߸��Ϻᵣ
	DESIGN_PARAM_ITEM* pParam=pTower->hashParams.Add(KEY4C("X1"));
	pParam=pTower->hashParams.Add(KEY4C("Z0"));
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=0;
	pParam->uiCategoryId=0;	//��̨���ز���������ʾ

	pParam=pTower->hashParams.Add(KEY4C("X1"));
	pParam->sLabel="X1";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=9000;	//8970
	pParam->uiCategoryId=HEAD_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("X2"));
	pParam->sLabel="X2";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=600;	//500
	pParam->uiCategoryId=HEAD_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("X3"));
	pParam->sLabel="X3";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=600;	//500
	pParam->uiCategoryId=HEAD_ID_LEVEL2;
	pParam=pTower->hashParams.Add(KEY4C("X4"));
	pParam->sLabel="X4";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=4850;	//
	pParam->uiCategoryId=HEAD_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("X5"));
	pParam->sLabel="X5";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=1800;	//1800
	pParam->uiCategoryId=HEAD_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("X6"));
	pParam->sLabel="X6";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=4070;	//
	pParam->uiCategoryId=HEAD_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("X7"));
	pParam->sLabel="X7";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=5000;	//500
	pParam->uiCategoryId=HEAD_ID_LEVEL1;

	pParam=pTower->hashParams.Add(KEY4C("Y1"));
	pParam->sLabel="Y1";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=500;
	pParam->uiCategoryId=HEAD_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("Y2"));
	pParam->sLabel="Y2";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=1200;
	pParam->uiCategoryId=HEAD_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("Y3"));
	pParam->sLabel="Y2";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=1200;
	pParam->uiCategoryId=HEAD_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("Z1"));
	pParam->sLabel="Z1";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=9100;
	pParam->uiCategoryId=HEAD_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("Z2"));
	pParam->sLabel="Z2";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=5700;
	pParam->uiCategoryId=HEAD_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("Z3"));
	pParam->sLabel="Z3";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=5600;
	pParam->uiCategoryId=HEAD_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("Z4"));
	pParam->sLabel="Z4";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=1000;
	pParam->uiCategoryId=HEAD_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("Z5"));
	pParam->sLabel="Z5";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=4000;
	pParam->uiCategoryId=HEAD_ID_LEVEL1;
	pParam=pTower->hashParams.Add(KEY4C("Z6"));
	pParam->sLabel="Z6";
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=600;
	pParam->uiCategoryId=HEAD_ID_LEVEL1;

	CATLOG_ITEM* pCatgItem=pTower->hashCatlogs.Add(HEAD_ID_LEVEL1);
	pCatgItem->name="��ͷ�����Ʋ���";
	pCatgItem=pTower->hashCatlogs.Add(HEAD_ID_LEVEL2);
	pCatgItem->name="��ͷ�����Ʋ���";
	pCatgItem=pTower->hashCatlogs.Add(TRUNK_ID_LEVEL);
	pCatgItem->name="������Ȳ���";
	//�ڵ�������ϵ
	//�Ʊ���ͷ
	SLAVE_NODE* pSlaveNode=pTower->hashSlaveNodes.Append(0x2C);	//N10�ڵ�
	pSlaveNode->cFlag=0x07;
	pSlaveNode->cbMirFlag=0x07;
	StrCopy(pSlaveNode->posx_expr,"X1",20);
	StrCopy(pSlaveNode->posy_expr,"Y1",20);
	StrCopy(pSlaveNode->posz_expr,"Z0",20);
	pSlaveNode=pTower->hashSlaveNodes.Append(0x38);	//N20�ڵ�
	pSlaveNode->cFlag=0x05;
	pSlaveNode->cbMirFlag=0x07;
	StrCopy(pSlaveNode->posx_expr,"X1-X3",20);
	StrCopy(pSlaveNode->posz_expr,"Z0-Z6",20);
	pSlaveNode=pTower->hashSlaveNodes.Append(0x34);	//N30
	pSlaveNode->cFlag=0x01;
	pSlaveNode->cbMirFlag=0x07;
	StrCopy(pSlaveNode->posx_expr,"X1-X2",20);
	pSlaveNode=pTower->hashSlaveNodes.Append(0x30);	//N40
	pSlaveNode->cFlag=0x04;
	pSlaveNode->cbMirFlag=0x07;
	StrCopy(pSlaveNode->posz_expr,"Z0",20);
	pSlaveNode=pTower->hashSlaveNodes.Append(0x3c);	//N50
	pSlaveNode->cFlag=0x07;
	pSlaveNode->cbMirFlag=0x07;
	StrCopy(pSlaveNode->posx_expr,"X5+X6",20);
	StrCopy(pSlaveNode->posy_expr,"Y2",20);
	StrCopy(pSlaveNode->posz_expr,"Z5",20);
	pSlaveNode=pTower->hashSlaveNodes.Append(0x40);	//N60
	pSlaveNode->cFlag=0x05;
	pSlaveNode->cbMirFlag=0x07;
	StrCopy(pSlaveNode->posx_expr,"X6",20);
	StrCopy(pSlaveNode->posz_expr,"Z5",20);
	pSlaveNode=pTower->hashSlaveNodes.Append(0x4c);	//N70
	pSlaveNode->cFlag=0x07;
	pSlaveNode->cbMirFlag=0x07;
	StrCopy(pSlaveNode->posx_expr,"X4+X5+X6",20);
	StrCopy(pSlaveNode->posy_expr,"Y3",20);
	StrCopy(pSlaveNode->posz_expr,"Z3",20);
	pSlaveNode=pTower->hashSlaveNodes.Append(0x44);	//N80
	pSlaveNode->cFlag=0x07;
	pSlaveNode->cbMirFlag=0x07;
	StrCopy(pSlaveNode->posx_expr,"X5+X6",20);
	StrCopy(pSlaveNode->posy_expr,"Y2",20);
	StrCopy(pSlaveNode->posz_expr,"Z3",20);
	pSlaveNode=pTower->hashSlaveNodes.Append(0x48);	//N90
	pSlaveNode->cFlag=0x05;
	pSlaveNode->cbMirFlag=0x07;
	StrCopy(pSlaveNode->posx_expr,"X6",20);
	StrCopy(pSlaveNode->posz_expr,"Z3",20);
	pSlaveNode=pTower->hashSlaveNodes.Append(0x52);	//N100
	pSlaveNode->cFlag=0x04;
	pSlaveNode->cbMirFlag=0x07;
	//StrCopy(pSlaveNode->posx_expr,"X6",20);//=0
	StrCopy(pSlaveNode->posz_expr,"Z4+Z5",20);
	pSlaveNode=pTower->hashSlaveNodes.Append(0x52);	//N110
	pSlaveNode->cFlag=0x05;
	pSlaveNode->cbMirFlag=0x07;
	StrCopy(pSlaveNode->posx_expr,"X7",20);//=0
	StrCopy(pSlaveNode->posz_expr,"Z2+Z3",20);
	//
	return true;
}
bool CTower::FromBuffer(CBuffer &buffer,long doc_type/*=0*/,bool loadLibToArchive/*=false*/)
{
	buffer.SeekToBegin();
	Empty();		//��ǰ��Empty()�о����ܻ�����⵼��handle��Ϊ0x20
	InitTower();	//��ǰ��InitTower()�о����ܻ�����⵼��handle��Ϊ0x20
    long i;
	DWORD cursor_pipeline_no=0;
	char sDocType[40],sMessageTitle[20]="";
	buffer.ReadString(sDocType,40);
	if(doc_type==0)
	{
#if defined(__TMA_FILE_)
		doc_type=1;
		strcpy(sMessageTitle,"TMA");
#elif defined(__LMA_FILE_)
		doc_type=2;
		strcpy(sMessageTitle,"LMA");
#elif defined(__TSA_FILE_)
		doc_type=3;
		strcpy(sMessageTitle,"TSA");
#elif defined(__LDS_FILE_)
		doc_type=4;
		strcpy(sMessageTitle,"LDS");
#else //defined(__TDA_FILE_)
		doc_type=5;
		strcpy(sMessageTitle,"TDA");
#endif
	}
	if((doc_type==1&&stricmp(sDocType,"������������")!=0)
		||(doc_type==2&&stricmp(sDocType,"����������ϵͳ")!=0&&stricmp(sDocType,"��������������")!=0)
		||(doc_type==3&&stricmp(sDocType,"���������������")!=0)
		||(doc_type==4&&stricmp(sDocType,"������������ͼϵͳ")!=0)
		||(doc_type==5&&stricmp(sDocType,"������ͼ����")!=0))

	{
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox(NULL,"Wrong data block,read failed!",sMessageTitle,MB_OK);
#else
		MessageBox(NULL,"���ݿ��ʽ���ԣ���ȡʧ�ܣ�",sMessageTitle,MB_OK);
#endif
		return false;
	}
	long buffer_len=buffer.GetLength();
	double process_scale=100.0/buffer_len;
	if(DisplayProcess!=NULL)
		DisplayProcess(progress.Mapping(buffer.GetCursorPosition()*process_scale),progress.title);
	buffer.ReadString(version);
	long nVersion=FromStringVersion(version);
	buffer.ReadInteger(&m_uiOriginalDogKey);//�ļ��������ʱ�ļ�������
	buffer.ReadInteger(&m_uiLastSaveDogKey);//�ļ����һ�α���ʹ�õļ�������
	buffer.ReadString(m_sPrjName);
	buffer.ReadString(m_sTaType);
	buffer.ReadString(m_sTaAlias);
	buffer.ReadString(m_sTaStampNo);
	buffer.ReadDouble(&m_fMaxXieJGLen);
	CfgwordFromBuffer(cfgword,buffer,nVersion,doc_type);
	if(	version==0||
		(doc_type==1&&nVersion>=5000000)||	//TMA V5.0.0.0
		(doc_type==2&&nVersion>=2000502)||	//LMA V2.0.5.2
		(doc_type==3&&nVersion>=2010000)||	//TSA V2.1.0.0
		(doc_type==4&&nVersion>=1030001)||	//LDS V1.3.0.1
		(doc_type==5&&nVersion>=1030001))	//TDA V1.3.0.1
	{
		BYTE schema=0;
		buffer.ReadByte(&schema);
		CFGLEG::SetSchema(schema);
		if (schema==CFGLEG::MULTILEG_UDF&&(
			//(doc_type==1&&nVersion>=5000000)||	//TMA V5.0.0.0
			//(doc_type==2&&nVersion>=2000502)||	//LMA V2.0.5.2
			//(doc_type==3&&nVersion>=2010000)||	//TSA V2.1.0.0
			(doc_type==4&&nVersion>=1031101)||	//LDS V1.3.11.1
			(doc_type==5&&nVersion>=1031101)))	//TDA V1.3.11.1
		{
			buffer.ReadByte(&CFGLEG::xUdfSchema.cnHeightCount);
			buffer.Read(CFGLEG::xUdfSchema.xarrHeights,CFGLEG::xUdfSchema.cnHeightCount*sizeof(UDF_HEIGHTLEGS));
		}
	}
	else
		CFGLEG::SetSchema(CFGLEG::MULTILEG_MAX08);
	if(   (doc_type==1&&(nVersion>0&&nVersion<4000100))	//TMA V4.0.1.0
		||(doc_type==2&&(nVersion>0&&nVersion<1000100)) //LMA V1.0.1.0
		||(doc_type==3&&(nVersion>0&&nVersion<1070300)) //TSA V1.7.3.0
		||(doc_type==4&&(nVersion>0&&nVersion<1000300)))//LDS V1.0.3.0
	{
		WORD nSegNum;	//�ֽ�Ϊ��ȡ�ɸ�ʽ�ļ���
		buffer.ReadWord(&nSegNum);
	}
#ifdef  __COMMON_PART_INC_
	if(  version==0||
		(doc_type==1&&nVersion>=5000200)||	//TMA V5.0.2.0
		(doc_type==2&&nVersion>=2000200)||	//LMA V2.0.2.0
		//(doc_type==3&&nVersion>=2010100)||//TSA V2.1.1.0
		(doc_type==4&&nVersion>=1030200)||	//LDS V1.3.2.0
		(doc_type==5&&nVersion>=1030200))	//TDA V1.3.2.0
		buffer.ReadByte(&m_ciRollProcessType);
	if(	version==0||
		(doc_type==1&&nVersion>=5010500)||	//TMA V5.1.5.0
		(doc_type==2&&nVersion>=2000800)||	//LMA V2.0.8.0
		(doc_type==4&&nVersion>=1030700)||	//LDS V1.3.7.0
		(doc_type==5&&nVersion>=1030700))	//TDA V1.3.7.0
	{
		buffer.ReadByte(&m_ciRollOffsetType);
		buffer.ReadByte(&m_ciDeformType);
	}
	if(	version==0||
		(doc_type==1&&nVersion>=5010600)||	//TMA V5.1.6.0
		(doc_type==2&&nVersion>=2000800)||	//LMA V2.0.8.0
		(doc_type==4&&nVersion>=1030800)||	//LDS V1.3.8.0
		(doc_type==5&&nVersion>=1030800))	//TDA V1.3.8.0
	{
		BYTE cnThickItemCount=0,cnAngleItemCount=0;
		buffer.ReadByte(&cnThickItemCount);	//����������Ŀ
		buffer.ReadByte(&cnAngleItemCount);	//����Ƕ��������Ŀ
		if(cnAngleItemCount<1||cnThickItemCount<1)
		{
			buffer.ReadDouble(&m_fDeformCoef);
			buffer.ReadDouble(&m_fHuoquR);
		}
		else
		{	//������Ϊ�������飬������Ϊ����Ƕȷ���(��)
			DYN_ARRAY<double> thick_header(cnThickItemCount),angle_header(cnAngleItemCount);
			DYN_ARRAY<double> mapdata(cnThickItemCount*cnAngleItemCount);
			buffer.Read(thick_header,sizeof(double)*cnThickItemCount);
			buffer.Read(angle_header,sizeof(double)*cnAngleItemCount);
			//�������Բ�ϵ����ֵ��
			buffer.Read(mapdata,sizeof(double)*mapdata.Size());
			mapNeutroCoef.Init(cnAngleItemCount,cnThickItemCount,mapdata);
			mapNeutroCoef.InitHeader(thick_header,angle_header);
			//������������Բ����ֵ��
			buffer.Read(mapdata,sizeof(double)*mapdata.Size());
			mapBendR.Init(cnAngleItemCount,cnThickItemCount,mapdata);
			mapBendR.InitHeader(thick_header,angle_header);
		}
	}
#endif
	//buffer.ReadInteger(&m_iPlankWeightStatType);	//�˱������Ѳ������ļ��� wjh-2012.8.12
	if(   (doc_type==1&&(nVersion>0&&nVersion<4020002))	//TMA V4.2.0.2
		||(doc_type==2&&(nVersion>0&&nVersion<1020002)) //LMA V1.2.0.2
		||(doc_type==3&&(nVersion>0&&nVersion<1080103)) //TSA V1.8.1.3
		||(doc_type==4&&(nVersion>0&&nVersion<1020002))	//LDS V1.2.0.2
		||(doc_type==5&&(nVersion>0&&nVersion<1020002)))//TDA V1.2.0.2
		buffer.Offset(4);	//ֱ��ƫ�ƣ����ֽڣ�����������
	if((doc_type==1&&(nVersion==0||nVersion>=4000022))	  //TMA
		||(doc_type==2&&(nVersion==0||nVersion>=1000077)) //LMA
		||doc_type==3||doc_type==5									  //TSA|TDA
		||(doc_type==4&&(nVersion==0||nVersion>=1000003)))//LDS
	{
		buffer.ReadString(m_sOperator);	//����Ա==���Ʊ���
		buffer.ReadString(m_sAuditor);	//���Ա==�������		
	}
	if(	version==0&&doc_type!=3||
		(doc_type==1&&nVersion>=5010700)||	//TMA V5.1.7.0
		(doc_type==2&&nVersion>=2000800)||	//LMA V2.0.8.0
		(doc_type==4&&nVersion>=1030800)||	//LDS V1.3.8.0
		(doc_type==5&&nVersion>=1030800))	//TDA V1.3.8.0
		buffer.ReadByte(&m_ciRepositionAlgor);
	else if(version==0&&doc_type!=3||
		(doc_type==1&&nVersion>=5010200)||	//TMA V5.1.2.0
		(doc_type==2&&nVersion>=2000502)||	//LMA V2.0.5.2
		(doc_type==4&&nVersion>=1030502)||	//LDS V1.3.5.2
		(doc_type==5&&nVersion>=1030502))	//TDA V1.3.5.2
	{
		buffer.ReadByte(&m_ciRepositionAlgor);
		if(m_ciRepositionAlgor==0)
			m_ciRepositionAlgor=2;
	}
	else
		m_ciRepositionAlgor=1;//true;
	//���ǻ�̩�ѷ���V1.3.8.0LDS������´���Ҫ���� wjh-2017.11.19
	if(	version==0&&doc_type!=3||
		(doc_type==1&&nVersion>=5010800)||	//TMA V5.1.7.0
		(doc_type==2&&nVersion>=2000800)||	//LMA V2.0.8.0	//�����е����9�·ݷ��͵İ汾������11�·ݷ����İ���(˳̩,���ݷɽ������պ�Դ) wjh-2017.11.23
		(doc_type==4&&nVersion>=1030900)||	//LDS V1.3.8.0
		(doc_type==5&&nVersion>=1030800))	//TDA V1.3.8.0
	{
		UINT id=0,count=0;
		bool bDual=false;
		CXhChar50 exprstr;
		buffer.ReadInteger(&count);
		hashConstraints.Empty();
		CONSTRAINT::hashExpressions.Empty();
		for(i=0;i<(long)count;i++)
		{
			buffer.ReadInteger(&id);
			CONSTRAINT* pConstraint=hashConstraints.Add(id);
			buffer.ReadInteger(&pConstraint->hMasterObj);
			buffer.ReadWord(&pConstraint->wiMasterPropId);
			buffer.ReadInteger(&pConstraint->hAcceptObj);
			buffer.ReadWord(&pConstraint->wiAcceptPropId);
			buffer.ReadBooleanThin(&bDual);
			pConstraint->dual=bDual;
			buffer.ReadString(exprstr,exprstr.GetLengthMax());
			if(exprstr.GetLength()>0)
				pConstraint->expression=exprstr;
		}
	}

#ifdef __PART_DESIGN_INC_
	if(doc_type==3||doc_type==4)
	{
		char sText[100]="";
		if((doc_type==3&&nVersion<1070006)||(doc_type==4&&nVersion<1000017))
			buffer.ReadString(sText);	//m_sOperator
		buffer.ReadString(sText);		
		m_sCompanyName=sText;		//��Ƶ�λ
		if((doc_type==3&&nVersion>=1070004)||(doc_type==4&&nVersion>=1000015))
			weather.FromBuffer(buffer);//��������
		buffer.ReadInteger(&volt_grade);//��ѹ�ȼ�:330,500
		buffer.ReadByte(&m_cLandscape);	//��λ����:'A','B','C'
		if((doc_type==3&&nVersion>=1070202)||(doc_type==4&&nVersion>=1000202))
			buffer.ReadDouble(&m_fTowerHeight);
		/*��ȫ�ȼ�(�����ṹ��Ҫ��ϵ��gama0)��
		0:һ���ر���Ҫ�ĸ����ṹ(gama0=1.1)��
		1:����������ѹ��·�ĸ������(gama0=1.0)��
		2:������ʱʹ�õĸ������(gama0=0.9).*/
		buffer.ReadWord(&m_siSafetyGrade);
		if((doc_type==3&&(nVersion==0||nVersion>=1070204))||(doc_type==4&&(nVersion==0||nVersion>=1000204)))
			buffer.ReadDouble(&m_fSafetyCoef);
		buffer.ReadDouble(&m_fIceDensity);	//���������ض�kg/m3
		if((doc_type==3&&(nVersion==0||nVersion>=2000200))||(doc_type==4&&(nVersion==0||nVersion>=1020200)))
			buffer.ReadByte(&m_cStructureType);
		//ѡ������
		if((doc_type==3&&(nVersion==0||nVersion>=2000006))||(doc_type==4&&(nVersion==0||nVersion>=1020006)))
		{
			buffer.ReadInteger(&m_iLoadExecuteCode);	//���ؼ���ִ�й淶
			buffer.ReadInteger(&m_iMemberSizeCheckCode);	//���������Ż�ѡ��ִ�й淶
			buffer.ReadInteger(&m_iSlendernessLimCode);	//�˼���ϸ�����ƹ淶
			buffer.ReadDouble(&m_fLimitBearCoef);			//����ϵ��������ֵ
			buffer.ReadDouble(&m_fGamaGForDesign);		//�������ʱ�������ط���ϵ��
			buffer.ReadDouble(&m_fGamaGForFoundation);	//�������ʱ�������ط���ϵ��
			buffer.ReadDouble(&m_fGamaQForTowerWindLoad);//�����������ӷ���ط���ϵ��
			if (nVersion==0||(doc_type==4&&nVersion>=1031000))//LDS V1,3,10,0
			{
				buffer.ReadBooleanThin(&m_bWeightAmplifyCoefIncBraceRods);	//��ѹ������ϵ�����Ƿ�������ĸ˼� wjh-2019.7.13
				buffer.ReadBooleanThin(&m_bAreaAmplifyCoefIncBraceRods);	//��ѹ������Ŵ�ϵ�����Ƿ�������ĸ˼� wjh-2019.7.13
			}
			else
			{
				m_bWeightAmplifyCoefIncBraceRods=false;	//��ѹ������ϵ�����Ƿ�������ĸ˼� wjh-2019.7.13
				m_bAreaAmplifyCoefIncBraceRods=false;	//��ѹ������Ŵ�ϵ�����Ƿ�������ĸ˼� wjh-2019.7.13
			}
			buffer.ReadInteger(&SelMatPara.m_nMainPoleSlendernessLimit);
			buffer.ReadInteger(&SelMatPara.m_nXiePoleSlendernessLimit);
			buffer.ReadInteger(&SelMatPara.m_nAuxPoleSlendernessLimit);
			buffer.ReadInteger(&SelMatPara.m_nTensivePoleSlendernessLimit);
			buffer.ReadInteger(&SelMatPara.m_nMainTubeSlendernessLimit);
			buffer.ReadInteger(&SelMatPara.m_nXieTubeSlendernessLimit);
			buffer.ReadInteger(&SelMatPara.m_nAuxTubeSlendernessLimit);
			buffer.ReadInteger(&SelMatPara.m_nTensiveTubeSlendernessLimit);
			buffer.ReadInteger(&SelMatPara.m_bPreferSingleAngle);
			buffer.ReadInteger(&SelMatPara.m_iSizeOrderRule);
			buffer.ReadInteger(&SelMatPara.m_nMaxSelMatCount) ;
			buffer.ReadInteger(&SelMatPara.m_nConvergeLim);
			buffer.ReadString(SelMatPara.m_csMinAngleSize,9);
			buffer.ReadString(SelMatPara.m_csMinForceAngleSize,9);
			buffer.ReadString(SelMatPara.m_csMinMainAngleSize,9);
		}
		if(doc_type==4&&nVersion>=1020100)	//LDS(V1.2.1.0)
		{	//���ƶ�ЧӦ��ѹ�ۼ�ϵ��
			//double eiffel=1.0;
			//long hCase,iWindSegNo;
			WORD n=0;
			buffer.ReadWord(&n);
			if(n>0)	//�˷�ѹ�ۼ�ϵ������V1.2.1.0�汾���ѵ�ʱ����,Ŀǰ��Ӱ��2013.11.12~12.10�ڼ�����Ժ�����ļ�����˾�ڲ������ļ� wjh-2013.12.10
				buffer.Offset(-2);
			/*for(i=0;i<n;i++)
			{
				buffer.ReadInteger(&hCase);
				buffer.ReadInteger(&iWindSegNo);
				buffer.ReadDouble(&eiffel);
				hashMapEiffelCoef.SetValue(hCase,iWindSegNo,eiffel);
			}*/
		}
	}
#endif
	//׼���
	if(loadLibToArchive)
	{
		WORD N=0;
		buffer.ReadWord(&N);
		archiveLibrary.AngleG.SetSize(N);
		for(i=0;i<N;i++)
		{
			buffer.ReadWord(&archiveLibrary.AngleG[i].wide);	//֫��
			buffer.ReadWord(&archiveLibrary.AngleG[i].g);	//��׼׼��g
			buffer.ReadWord(&archiveLibrary.AngleG[i].g1);	//׼��g1
			buffer.ReadWord(&archiveLibrary.AngleG[i].g2);	//׼��g2
			buffer.ReadWord(&archiveLibrary.AngleG[i].g3);	//׼��g3
		}
	}
	else
	{
		buffer.ReadWord(&zhunju_N);
		for(i=0;i<zhunju_N;i++)
		{
			buffer.ReadWord(&zhunju_table[i].wide);	//֫��
			buffer.ReadWord(&zhunju_table[i].g);	//��׼׼��g
			buffer.ReadWord(&zhunju_table[i].g1);	//׼��g1
			buffer.ReadWord(&zhunju_table[i].g2);	//׼��g2
			buffer.ReadWord(&zhunju_table[i].g3);	//׼��g3
		}
	}
	//��˨����
	if(loadLibToArchive)
	{
		int N=0;
		buffer.ReadInteger(&N);
		archiveLibrary.BoltSpace.SetSize(N);
		for(i=0;i<N;i++)
		{
			buffer.ReadInteger(&archiveLibrary.BoltSpace[i].M_Diameter);
			buffer.ReadInteger(&archiveLibrary.BoltSpace[i].SingleRowSpace);
			buffer.ReadInteger(&archiveLibrary.BoltSpace[i].doubleRowSpace);
			buffer.ReadInteger(&archiveLibrary.BoltSpace[i].EndSpace);
			buffer.ReadInteger(&archiveLibrary.BoltSpace[i].PlateEdgeSpace);
			buffer.ReadInteger(&archiveLibrary.BoltSpace[i].RollingEdgeSpace);
			buffer.ReadInteger(&archiveLibrary.BoltSpace[i].CutAngleSpace);
			LSSPACEEX space=LSSPACE_TABLE[i];
			if(  nVersion==0||
				(doc_type==1&&nVersion>=5010500)||
				(doc_type==2&&nVersion>=2000800)||
				(doc_type==4&&nVersion>=1030700)||
				(doc_type==5&&nVersion>=1030700))
				buffer.ReadInteger(&archiveLibrary.BoltSpace[i].WasherEdgeSpace);
		}
	}
	else
	{
		buffer.ReadInteger(&lsspace_N);
		for(i=0;i<lsspace_N;i++)
		{
			buffer.ReadInteger(&LSSPACE_TABLE[i].M_Diameter);
			buffer.ReadInteger(&LSSPACE_TABLE[i].SingleRowSpace);
			buffer.ReadInteger(&LSSPACE_TABLE[i].doubleRowSpace);
			buffer.ReadInteger(&LSSPACE_TABLE[i].EndSpace);
			buffer.ReadInteger(&LSSPACE_TABLE[i].PlateEdgeSpace);
			buffer.ReadInteger(&LSSPACE_TABLE[i].RollingEdgeSpace);
			buffer.ReadInteger(&LSSPACE_TABLE[i].CutAngleSpace);
			LSSPACEEX space=LSSPACE_TABLE[i];
			if(  nVersion==0||
				(doc_type==1&&nVersion>=5010500)||
				(doc_type==2&&nVersion>=2000800)||
				(doc_type==4&&nVersion>=1030700)||
				(doc_type==5&&nVersion>=1030700))
			{
				buffer.ReadInteger(&space.WasherEdgeSpace);
				IBoltLibrary::SetLsSpaceEx(space.M_Diameter,space);
			}
		}
	}
	//��˨����
	if(loadLibToArchive)
	{
		int nSeries=0,record_n=0,intVal=0;
		buffer.ReadInteger(&nSeries);
		for(i=0;i<nSeries;i++)
		{
			CLsFamily family;
			buffer.ReadString(family.name,16);
			buffer.ReadInteger(&record_n);
			buffer.ReadString(family.descr,50);
			buffer.ReadInteger(&intVal);family.m_bAntiLoose=intVal>0;
			buffer.ReadInteger(&intVal);family.m_bDualCap=intVal>=2;
			buffer.ReadInteger(&intVal);family.m_bFootNail=(intVal!=FALSE);
			buffer.ReadInteger(&intVal);family.m_bBurglarproof=(intVal!=FALSE);
			if(	 nVersion==0&&doc_type!=3||
				(doc_type==1&&nVersion>=5010800)||	//TMA V5.1.8.0
				(doc_type==2&&nVersion>=2000900)||	//LMA V2.0.9.0
				(doc_type==4&&nVersion>=1030900)||	//LDS V1.3.9.0
				(doc_type==5&&nVersion>=1030800))	//TDA V1.3.8.0
			{
				bool bTemporyVersionDisableMappingBoltFamily=false;
				if(	doc_type==1&&nVersion>0&&nVersion<5010900)	//TMA V5.1.8.0
					bTemporyVersionDisableMappingBoltFamily=true;
				if(!bTemporyVersionDisableMappingBoltFamily)
				{
					buffer.ReadInteger(&family.m_hMappingAntiTheftFamily);
					buffer.ReadInteger(&family.m_hMappingAntiLooseFamily);
				}
				for(int i=0;i<8;i++)
				{
					buffer.ReadWord(&family.ls_sub_part[i].idSubPart);
					buffer.ReadWord(&family.ls_sub_part[i].nCount);
				}
			}
			for(int j=0;j<record_n;j++)
			{
				LS_XING_HAO XingHao;
				buffer.ReadDouble(&XingHao.grade);			//��˨����
				buffer.ReadInteger(&XingHao.d);			//��˨ֱ��
				buffer.ReadString(XingHao.guige);			//��˨���
				buffer.ReadInteger(&XingHao.valid_len);	//��Ч��
				buffer.ReadInteger(&XingHao.no_thread_len);//�޿۳�
				buffer.ReadInteger(&XingHao.down_lim);		//��˨ͨ������
				buffer.ReadInteger(&XingHao.up_lim);		//��˨ͨ������
				buffer.ReadInteger(&XingHao.washer_thick);	//��Ȧ���pXingHao->pad_thick
				buffer.ReadDouble(&XingHao.weight);		//��˨��������
			}
		}
	}
	else
	{
		long nLsXingHaoCount=0;
		if((doc_type==1&&nVersion<4000109)||(doc_type==2&&nVersion<1000109)||(doc_type==3&&nVersion<1070309)||(doc_type==4&&nVersion<1000309))
			buffer.ReadInteger(&nLsXingHaoCount);
		else if((doc_type==1&&nVersion==4000109)||(doc_type==2&&nVersion==1000109)||(doc_type==3&&nVersion==1070309)||(doc_type==4&&nVersion==1000309))
		{
			buffer.ReadInteger(&nLsXingHaoCount);
			int nData=nLsXingHaoCount;
			int increL;
			if(nLsXingHaoCount<30)	//�ð汾���Ѵ洢��˫ñ��˨����������Ϣ(��˨�ͺ�һ�㶼Զ����30��)
			{
				increL=nLsXingHaoCount;	//CLDSBolt::TwoCapM16IncreL
				buffer.ReadInteger(&increL);//CLDSBolt::TwoCapM20IncreL);
				buffer.ReadInteger(&increL);//CLDSBolt::TwoCapM24IncreL);
				buffer.ReadInteger(&nLsXingHaoCount);
			}
		}
		else if(nVersion==0 ||(doc_type==1&&nVersion==4000110)||(doc_type==2&&nVersion==1000110)||(doc_type==3&&nVersion==1070310)||(doc_type==4&&nVersion==1000310))
		{	//�ð汾���Ѵ洢��˫ñ��˨����������Ϣ
			int increL;
			buffer.ReadInteger(&increL);//CLDSBolt::TwoCapM16IncreL);
			buffer.ReadInteger(&increL);//CLDSBolt::TwoCapM20IncreL);
			buffer.ReadInteger(&increL);//CLDSBolt::TwoCapM24IncreL);
			buffer.ReadInteger(&nLsXingHaoCount);
		}
		if(nVersion==0 ||(doc_type==1&&nVersion>=4000111)||(doc_type==2&&nVersion>=1000111)||
			(doc_type==3&&nVersion>=1070311)||(doc_type==4&&nVersion>=1000311)||doc_type==5)
		{
			int nSeries=0,record_n=0,intVal=0;
			buffer.ReadInteger(&nSeries);
			CLsLibrary::ClearLibrary();
			for(i=0;i<nSeries;i++)
			{
				CLsFamily* pFamily=CLsLibrary::AppendFamily();
				if(	nVersion==0&&doc_type!=3||
					(doc_type==1&&nVersion>=5020802)||	//TMA V5,2,0,2
					(doc_type==2&&nVersion>=2010002)||	//LMA V2,1,0,2
					(doc_type==4&&nVersion>=1030901)||	//LDS V1.3.9.1
					(doc_type==5&&nVersion>=1030805))	//TDA V1,3,8,5
					buffer.ReadInteger(&pFamily->m_uiCodeSerial);	//��˨��ѭ�淶���
				buffer.ReadString(pFamily->name,17);
				buffer.ReadInteger(&record_n);
				buffer.ReadString(pFamily->descr,51);
				buffer.ReadInteger(&intVal);pFamily->m_bAntiLoose=intVal>0;
				buffer.ReadInteger(&intVal);pFamily->m_bDualCap=intVal>=2;
				buffer.ReadInteger(&intVal);pFamily->m_bFootNail=(intVal!=FALSE);
				buffer.ReadInteger(&intVal);pFamily->m_bBurglarproof=(intVal!=FALSE);
				if(	nVersion==0&&doc_type!=3||
					(doc_type==1&&nVersion>=5010800)||	//TMA V5.1.8.0
					(doc_type==2&&nVersion>=2000900)||	//LMA V2.0.9.0
					(doc_type==4&&nVersion>=1030900)||	//LDS V1.3.9.0
					(doc_type==5&&nVersion>=1030800))	//TDA V1.3.8.0
				{
					bool bTemporyVersionDisableMappingBoltFamily=false;
					if(	doc_type==1&&nVersion>0&&nVersion<5010900)	//TMA V5.1.8.0
						bTemporyVersionDisableMappingBoltFamily=true;
					if(!bTemporyVersionDisableMappingBoltFamily)
					{
						buffer.ReadInteger(&pFamily->m_hMappingAntiTheftFamily);
						buffer.ReadInteger(&pFamily->m_hMappingAntiLooseFamily);
					}
					for(int i=0;i<8;i++)
					{
						buffer.ReadWord(&pFamily->ls_sub_part[i].idSubPart);
						buffer.ReadWord(&pFamily->ls_sub_part[i].nCount);
					}
				}
				pFamily->CreateFamily(record_n);
				for(int j=0;j<record_n;j++)
				{
					LS_XING_HAO *pXingHao=pFamily->RecordAt(j);
					buffer.ReadDouble(&pXingHao->grade);			//��˨����
					buffer.ReadInteger(&pXingHao->d);			//��˨ֱ��
					buffer.ReadString(pXingHao->guige);			//��˨���
					buffer.ReadInteger(&pXingHao->valid_len);	//��Ч��
					buffer.ReadInteger(&pXingHao->no_thread_len);//�޿۳�
					buffer.ReadInteger(&pXingHao->down_lim);		//��˨ͨ������
					buffer.ReadInteger(&pXingHao->up_lim);		//��˨ͨ������
					buffer.ReadInteger(&pXingHao->washer_thick);	//��Ȧ���pXingHao->pad_thick
					buffer.ReadDouble(&pXingHao->weight);		//��˨��������
				}
			}
			//��ȡ��˨�����
			if(	 nVersion==0&&doc_type!=3||
				(doc_type==1&&nVersion>=5010900)||	//TMA V5.1.9.0
				(doc_type==2&&nVersion>=2000900)||	//LMA V2.0.9.0
				(doc_type==4&&nVersion>=1030900)||	//LDS V1.3.9.0
				(doc_type==5&&nVersion>=1030800))	//TDA V1.3.8.0
			{	
				buffer.ReadInteger(&nSeries);
				CLsLibrary::ClearLsSubPartLibrary();
				for(i=0;i<nSeries;i++)
				{
					CLsSubPartFamily xLsSubPart;
					buffer.ReadWord(&xLsSubPart.m_widType);
					buffer.ReadString(xLsSubPart.name);
					CLsSubPartFamily* pLsSubPartFamily=CLsLibrary::AppendLsSubPartFamily(xLsSubPart.m_widType);
					if(pLsSubPartFamily)
						strcpy(pLsSubPartFamily->name,xLsSubPart.name);		
				}
			}
		}
		else
		{
			//LS_XING_HAO lsguige_table[nLsXingHaoCount]
			LS_XING_HAO lsguige;
			for(i=0;i<nLsXingHaoCount;i++)
			{
				buffer.ReadDouble(&lsguige.grade);
				buffer.ReadInteger(&lsguige.d);
				buffer.ReadString(lsguige.guige);
				buffer.ReadInteger(&lsguige.valid_len);
				buffer.ReadInteger(&lsguige.no_thread_len);
				buffer.ReadInteger(&lsguige.down_lim);
				buffer.ReadInteger(&lsguige.up_lim);
				double D=0,H=0;	//��ʱΪ�˰汾һ�£���һ�汾ʱӦȥ������
				buffer.ReadDouble(&D);
				buffer.ReadDouble(&H);
				buffer.ReadDouble(&lsguige.weight);
				double pad_weight,twocap_weight;
				buffer.ReadDouble(&pad_weight);
				buffer.ReadDouble(&twocap_weight);
			}
		}
	}
	//�ؽ���˨��ϵ��
	if(	 nVersion==0&&doc_type!=3||
		(doc_type==1&&nVersion>=5020201)||	//TMA V5.2.2.1
		(doc_type==2&&nVersion>=2010201)||	//LMA V2.1.2.1
		(doc_type==4&&nVersion>=1031101)||	//LDS V1.3.11.1
		(doc_type==5&&nVersion>=1031101))	//TDA V1.3.11.1
	{
		int ii,nCount=0;
		buffer.ReadInteger(&nCount);	//��˨���ѡ������
		ANCHOR_BOLT::SetCount(nCount);
		for(ii=0;ii<nCount;ii++)
		{	
			ANCHOR_BOLT anchor;
			buffer.ReadWord(&anchor.d);
			buffer.ReadWord(&anchor.wiWidth);
			buffer.ReadWord(&anchor.wiThick);
			buffer.ReadWord(&anchor.wiHoleIncrement);
			buffer.ReadWord(&anchor.wiBasePlateHoleD);
			buffer.ReadFloat(&anchor.m_fNutH);
			buffer.ReadFloat(&anchor.m_fNutTaper);
			buffer.ReadFloat(&anchor.m_fNutDe);
			buffer.ReadFloat(&anchor.m_fNutDs);
			buffer.ReadFloat(&anchor.m_fLe);
			ANCHOR_BOLT::SetAnchorBolt(ii,anchor);
		}
	}
	if(nVersion==0 ||doc_type==5||(doc_type==1&&nVersion>=4000109)||(doc_type==2&&nVersion>=1000109)||
		(doc_type==3&&nVersion>=1070309)||(doc_type==4&&nVersion>=1000309))
	{	//TSA V1.7.3.9 or LDS V1.0.3.9
		int nRow;
		buffer.ReadInteger(&nRow);
		if(nRow>0)
			listSelBoltCond.Empty();
		for(i=0;i<nRow;i++)
		{
			BOLT_SELCOND *pSelBolt=listSelBoltCond.append();
			buffer.ReadInteger(&pSelBolt->d);
			buffer.ReadString(pSelBolt->grade,8);
			buffer.ReadString(pSelBolt->matStr,8);
			buffer.ReadInteger(&pSelBolt->upWidthLim);
		}
		if(loadLibToArchive)
		{	//���ʿ�
			int nRow;
			buffer.ReadInteger(&nRow);
			archiveLibrary.SteelMaterial.SetSize(nRow);
			for(i=0;i<nRow;i++)
			{
				buffer.ReadString(archiveLibrary.SteelMaterial[i].mark);
				buffer.ReadByte(&archiveLibrary.SteelMaterial[i].cBriefMark);
				buffer.ReadByte(&archiveLibrary.SteelMaterial[i].cPreferQualityLevel);
				buffer.ReadDouble(&archiveLibrary.SteelMaterial[i].E);
				buffer.ReadDouble(&archiveLibrary.SteelMaterial[i].price_coef);
				buffer.Read(archiveLibrary.SteelMaterial[i].t_lim,sizeof(int)*3);
				buffer.Read(archiveLibrary.SteelMaterial[i].intensity,sizeof(double)*16);
			}
		}
		else
		{
			if(nVersion==0||(doc_type==1&&nVersion>=4010003)||	//TMA V4.1.0.3
				(doc_type==2&&nVersion>=1010003)||	//LMA V1.1.0.3
				(doc_type==3&&nVersion>=1070310)||	//TSA V1.7.3.10
				(doc_type==4&&nVersion>=1000310)||	//LDS V1.0.3.10
				(doc_type==5&&nVersion>=1010003))	//TDA V1.1.0.3
			{
				int steelmat_N=4;
				buffer.ReadInteger(&steelmat_N);
				//��֪��Ϊʲô�����洢��һ�������ļ�ȱ������˨ѡ���������ֲ�����,�������д���Ϊ��ʱ�޸����ݴ��� wjh-2017.9.11
				//steelmat_N=3;
				//buffer.Offset(7);
				if(steelmat_N>0)
					CSteelMatLibrary::SetCount(steelmat_N);
				for(i=0;i<steelmat_N;i++)
				{
					STEELMAT mat;
					buffer.ReadString(mat.mark);
					buffer.ReadByte(&mat.cBriefMark);
					if ((version == 0 && doc_type != 3) ||
						(doc_type == 1 && nVersion >= 5020100) ||	//TMA V5.2.1.0
						(doc_type == 2 && nVersion >= 2010100) ||	//LMA V2.1.1.0
						(doc_type == 4 && nVersion >= 1030900) ||	//LDS V1.3.9.0
						(doc_type == 5 && nVersion >= 1030900))		//TDA V1.3.9.0
						buffer.ReadByte(&mat.cPreferQualityLevel);
					else
						mat.cPreferQualityLevel=0;
					buffer.ReadDouble(&mat.E);
					buffer.ReadDouble(&mat.price_coef);
					buffer.Read(mat.t_lim,sizeof(int)*3);
					buffer.Read(mat.intensity,sizeof(double)*16);
					CSteelMatLibrary::SetRecordAt(i,mat);
				}
			}
		}
#if defined(__TSA_FILE_)||defined(__LDS_FILE_)
		if(doc_type==3||doc_type==4)
		{
			if(loadLibToArchive)
			{	//�Ǹֿ�
				int nRow;
				buffer.ReadInteger(&nRow);
				archiveLibrary.AngleSize.SetSize(nRow);
				for(i=0;i<nRow;i++)
				{
					buffer.ReadDouble(&archiveLibrary.AngleSize[i].wing_wide);
					buffer.ReadByte(&archiveLibrary.AngleSize[i].cType);
					buffer.ReadDouble(&archiveLibrary.AngleSize[i].wing_thick);
					buffer.ReadDouble(&archiveLibrary.AngleSize[i].r);
					buffer.ReadDouble(&archiveLibrary.AngleSize[i].area);
					buffer.ReadDouble(&archiveLibrary.AngleSize[i].theroy_weight);
					buffer.ReadDouble(&archiveLibrary.AngleSize[i].Ix);
					buffer.ReadDouble(&archiveLibrary.AngleSize[i].Rx);
					buffer.ReadDouble(&archiveLibrary.AngleSize[i].Wx);
					buffer.ReadDouble(&archiveLibrary.AngleSize[i].Iy0);
					buffer.ReadDouble(&archiveLibrary.AngleSize[i].Ry0);
					buffer.ReadDouble(&archiveLibrary.AngleSize[i].Wy0);
				}
			}
			else
			{
				AngleLibrary()->Count=buffer.ReadInteger();
				for(i=0;i<jgguige_N;i++)
				{
					buffer.ReadDouble(&jgguige_table[i].wing_wide);
					buffer.ReadByte(&jgguige_table[i].cType);
					buffer.ReadDouble(&jgguige_table[i].wing_thick);
					buffer.ReadDouble(&jgguige_table[i].r);
					buffer.ReadDouble(&jgguige_table[i].area);
					buffer.ReadDouble(&jgguige_table[i].theroy_weight);
					buffer.ReadDouble(&jgguige_table[i].Ix);
					buffer.ReadDouble(&jgguige_table[i].Rx);
					buffer.ReadDouble(&jgguige_table[i].Wx);
					buffer.ReadDouble(&jgguige_table[i].Iy0);
					buffer.ReadDouble(&jgguige_table[i].Ry0);
					buffer.ReadDouble(&jgguige_table[i].Wy0);
				}
			}
		}
#endif
	}
//#ifndef __TMA_FILE_
	if(loadLibToArchive)
	{	//�Ǹֿ�
		int nRow;
		buffer.ReadInteger(&nRow);
		archiveLibrary.TubeSize.SetSize(nRow);
		for(i=0;i<nRow;i++)
		{
			buffer.ReadDouble(&archiveLibrary.TubeSize[i].D);
			buffer.ReadDouble(&archiveLibrary.TubeSize[i].thick);
			buffer.ReadDouble(&archiveLibrary.TubeSize[i].area);
			buffer.ReadDouble(&archiveLibrary.TubeSize[i].theroy_weight);
			buffer.ReadDouble(&archiveLibrary.TubeSize[i].I);
			buffer.ReadDouble(&archiveLibrary.TubeSize[i].r);
			buffer.ReadDouble(&archiveLibrary.TubeSize[i].W);
		}
	}
	else
	{
		if((nVersion==0&&doc_type>1)||(doc_type==2&&nVersion>=1000110)||
			(doc_type==3&&nVersion>=1070309)||(doc_type==4&&nVersion>=1000309)||doc_type==5)
		{	//TDA LMA V1.0.1.10 or TSA V1.7.3.9 or LDS V1.0.3.9
			buffer.ReadInteger(&tubeguige_N);
			for(i=0;i<tubeguige_N;i++)
			{
				buffer.ReadDouble(&tubeguige_table[i].D);
				buffer.ReadDouble(&tubeguige_table[i].thick);
				buffer.ReadDouble(&tubeguige_table[i].area);
				buffer.ReadDouble(&tubeguige_table[i].theroy_weight);
				buffer.ReadDouble(&tubeguige_table[i].I);
				buffer.ReadDouble(&tubeguige_table[i].r);
				buffer.ReadDouble(&tubeguige_table[i].W);
			}
		}
	}
	if((doc_type==3&&nVersion==1070309)||(doc_type==4&&nVersion==1000309))
	{	//TSA V1.7.3.9 or LDS V1.0.3.9
		int steelmat_N=4;
		buffer.ReadInteger(&steelmat_N);
		CSteelMatLibrary::SetCount(steelmat_N);
		for(i=0;i<steelmat_N;i++)
		{
			STEELMAT mat;
			buffer.ReadString(mat.mark);
			buffer.ReadByte(&mat.cBriefMark);
			buffer.ReadDouble(&mat.E);
			buffer.ReadDouble(&mat.price_coef);
			buffer.Read(mat.t_lim,sizeof(int)*3);
			buffer.Read(mat.intensity,sizeof(double)*16);
			CSteelMatLibrary::SetRecordAt(i,mat);
		}
	}
//#endif
	//LMA��LDS֧�ֱ���ֹܶ����ӱ�׼�� wht 11-05-03
#ifdef __COMPLEX_PART_INC_
	if(loadLibToArchive)
		ReadStdPartLibFromBuffer(buffer,&archiveLibrary,nVersion,doc_type);
	else
		ReadStdPartLibFromBuffer(buffer,NULL,nVersion,doc_type);
#endif
	long n,h,cls_typeid;
	CLDSObject *pObj=NULL;
	CLDSPoint *pPoint;
	CLDSLine *pLine;
	CLDSPlane *pPlane;
	CDisplayView *pDispView;
	CLDSNode *pNode;
	CLDSPart *pPart;

	int hits=0;
	//��ϣ�������Ϣ����
	buffer.ReadInteger(&n);
	hashObjs.SetHashTableGrowSize(n);
	View.Empty();
	BOOL bHasRepeatHandle=FALSE;
	//��ȡ�ļ�ʱ��ʱ��ͣ����������ó�ʼ����������,���������ļ��л������е��µ�ǰ��ͼ��Ч
	m_bInitNewObjDefaultInfo=false;	
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadInteger(&cls_typeid);
		if(h<=0)
			continue;
		if((pObj=hashObjs.GetValue(h))!=NULL)
		{
			if(cls_typeid==pObj->GetClassTypeId())
				continue;	//�Ѵ��ڴ˹������������
			else
			{	//���ֶ������ظ�,Ӧ����ļ�����ԭ��
				bHasRepeatHandle=TRUE;
				bool legacyPart=false,nowPart=false;
				if(pObj->IsDbObject()&&((CLDSDbObject*)pObj)->IsPart())
					legacyPart=true;
				if(cls_typeid==CLS_LINEANGLE||cls_typeid==CLS_GROUPLINEANGLE||cls_typeid==CLS_LINETUBE||
					cls_typeid==CLS_PLATE||cls_typeid==CLS_PARAMPLATE||cls_typeid==CLS_BOLT)
					nowPart=true;
				if(legacyPart==nowPart)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"Find repeated object handle,communicate with supplier of the software please!",sMessageTitle,MB_OK);
#else
					MessageBox(NULL,"���ֶ������ظ������������Ӧ����ϵ��",sMessageTitle,MB_OK);
#endif
					continue;
				}
				else
				{
					CXhChar50 clstypename;
					pObj->GetClassType((char*)clstypename);
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.Log("%s type 0x%X object has the same handle with part type object,exist hidden danger",(char*)clstypename,pObj->handle);
#else
					logerr.Log("%s����0x%X�����빹�����Ͷ������ظ�,��������",(char*)clstypename,pObj->handle);
#endif
				}
			}
		}
 		if(cls_typeid==CLS_GEPOINT)
		{
			pPoint=Point.append(FALSE);
			pPoint->handle=h;
			Point.UpdateHashtable(h,pPoint);
			hashObjs.SetValue(h,pPoint);
		}
		else if(cls_typeid==CLS_GELINE)
		{
			pLine=Line.append(FALSE);
			pLine->handle=h;
			Line.UpdateHashtable(h,pLine);
			hashObjs.SetValue(h,pLine);
		}
		else if(cls_typeid==CLS_GEPLANE)
		{
			pPlane=Plane.append(FALSE);
			pPlane->handle=h;
			Plane.UpdateHashtable(h,pPlane);
			hashObjs.SetValue(h,pPlane);
		}
		/*else if(cls_typeid==CLS_DISPLAYVIEW)
		{
			pDispView=View.append(FALSE);
			pDispView->handle=h;
			hashObjs.SetValue(h,pDispView);
		}*/
		else if(cls_typeid==CLS_NODE)
		{
			pNode=Node.append(FALSE);
			pNode->handle=h;
			Node.UpdateHashtable(h,pNode);
			hashObjs.SetValue(h,pNode);
		}
		else	//CLDSPart���͹���
		{
			pPart=Parts.append(cls_typeid,FALSE);
			if(pPart)
			{
				pPart->handle=h;
				Parts.UpdateHashtable(h,pPart);
				hashObjs.SetValue(h,pPart);
			}
		}
	}
	m_bInitNewObjDefaultInfo=true;
	Node.RebuildHashtable();
	Parts.RebuildHashtable();

	if(DisplayProcess!=NULL)
		DisplayProcess(progress.Mapping(buffer.GetCursorPosition()*100/buffer_len),progress.title);
	n=0;
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		if((pPoint=(CLDSPoint*)hashObjs.GetValue(h))!=NULL)
			pPoint->FromBuffer(buffer,nVersion,doc_type);
		else
		{
			CLDSPoint datum;
			datum.FromBuffer(buffer,nVersion,doc_type);
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Failed to read the handle of parametric point!");
#else
			logerr.Log("����������ֵ��ȡʧ��!");
#endif
		}
	}
	if(DisplayProcess!=NULL)
		DisplayProcess(progress.Mapping(buffer.GetCursorPosition()*100/buffer_len),progress.title);
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		if((pLine=(CLDSLine*)hashObjs.GetValue(h))!=NULL)
			pLine->FromBuffer(buffer,nVersion,doc_type);
		else
		{
			CLDSLine datum;
			datum.FromBuffer(buffer,nVersion,doc_type);
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Failed to read the handle of parametric line!");
#else
			logerr.Log("�������߾��ֵ��ȡʧ��!");
#endif
		}
	}
	if(DisplayProcess!=NULL)
		DisplayProcess(progress.Mapping(buffer.GetCursorPosition()*100/buffer_len),progress.title);
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		if ((pObj=hashObjs.GetValue(h))!=NULL)
		{
			CLDSPlane datum;
			if (pObj->GetClassTypeId()==CLS_GEPLANE)
				pPlane=(CLDSPlane*)pObj;
			else if((pPlane=Plane.FromHandle(h))==NULL)
				pPlane=&datum;
			pPlane->FromBuffer(buffer,nVersion,doc_type);
		}
		else
		{
			CLDSPlane datum;
			datum.FromBuffer(buffer,nVersion,doc_type);
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Failed to read the handle of parametric line!");
#else
			logerr.Log("�������߾��ֵ��ȡʧ��!");
#endif
		}
	}
	if(DisplayProcess!=NULL)
		DisplayProcess(progress.Mapping(buffer.GetCursorPosition()*100/buffer_len),progress.title);
	//����ģ����
	CObjNoGroup* pObjNoGroup=NoManager.FromGroupID(MODEL_GROUP);
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		CLDSModule *pModule=Module.append(FALSE);
		buffer.ReadInteger(&pModule->handle);
		pModule->FromBuffer(buffer,nVersion,doc_type);
		//LDS��TMA��LMA��TDA�ļ�ʱ������iNo��Ҫ��ֵ wht 13-04-02
		if(PRODUCT_TYPE==4&&(doc_type==1||doc_type==2||doc_type==5))
			pModule->iNo = pObjNoGroup->EnumIdleNo();
		pObjNoGroup->SetNoState(pModule->iNo);
	}
	InitEvolveCfgwordMap(nVersion,doc_type);
	EvolveCfgword(cfgword,nVersion,doc_type);
	buffer.ReadInteger(&m_hActiveModule);
	for(i=0;i<4;i++)
		buffer.ReadWord(&m_arrActiveQuadLegNo[i]);
	if( (nVersion==0&&doc_type!=3)||
		(doc_type==1&&nVersion>=5020000)||	//TMA V5.2.0.0
		(doc_type==2&&nVersion>=2010000)||	//LMA V2.1.0.0
		(doc_type==4&&nVersion>=1030804)||	//LDS V1.3.8.4
		(doc_type==5&&nVersion>=1030804))	//TDA V1.3.8.4
	{
		buffer.ReadBooleanThin(&this->m_bUserSpecifyNamedHeightZeroZ);
		buffer.ReadDouble(&this->m_fNamedHeightZeroZ);
	}
#ifdef __COMMON_PART_INC_
	if(  nVersion==0||
		(doc_type==1&&nVersion>=4020001)||	//TMA V4.2.0.1
		(doc_type==2&&nVersion>=1020001)||	//LMA V1.2.0.1
		//(doc_type==3&&nVersion>=1080001)||	//TSA V1.8.0.1
		(doc_type==4&&nVersion>=1020001)||	//LDS V1.2.0.1
		(doc_type==5&&nVersion>=1020001))	//TDA V1.2.0.1
	{
		buffer.ReadInteger(&n);
		pObjNoGroup=NoManager.FromGroupID(MODEL_ARCLIFT_GROUP);
		pObjNoGroup->EmptyNo();
		for(i=0;i<n;i++)
		{
			CLDSArcLift *pLift=ArcLift.append(FALSE);
			buffer.ReadInteger(&pLift->handle);
			pLift->FromBuffer(buffer,nVersion,doc_type);
			pObjNoGroup->SetNoState(pLift->iNo);
		}
	}
#endif
	//��ͼ����
	buffer.ReadInteger(&n);
	View.Empty();
	for(i=0;i<n;i++)
	{
		pDispView=View.append(FALSE);
		buffer.ReadInteger(&pDispView->handle);
		pDispView->FromBuffer(buffer,nVersion,doc_type);
		if(pDispView->m_iViewType==1&&compareVersion(version,"1.0.0.32")<0)
		{	//��ֵ��ʾ������
			for(CViewPart *pItem=pDispView->GetFirstViewPart();pItem;pItem=pDispView->GetNextViewPart())
			{
				CLDSObject *pObj;
				if((pObj=hashObjs.GetValue(pItem->handle))!=NULL)
					pItem->idClassType=pObj->GetClassTypeId();
			}
		}
	}
	buffer.ReadInteger(&h);
	if(CLDSObject::_console!=NULL)
		CLDSObject::_console->SetActiveView(h);

	// ------------ 1. vvvvvvvv �ڵ� --------------
	if(DisplayProcess!=NULL)
		DisplayProcess(progress.Mapping(buffer.GetCursorPosition()*100/buffer_len),progress.title);
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		//if(hashObjs.GetHashTableSize()>0)
		//	hashObjs.GetValueAt(h,(CLDSObject*&)pNode);
		if((pNode=Node.FromHandle(h))==NULL)
		{
			pNode=Node.append(FALSE);
			pNode->handle=h;
		}
		pNode->FromBuffer(buffer,nVersion,doc_type);
		if(DisplayProcess!=NULL)
			DisplayProcess(progress.Mapping(buffer.GetCursorPosition()*process_scale),progress.title);
	}
	if( version==0||
		(doc_type==1&&nVersion>=5010901)||	//TMA(V5.1.9.1)
		(doc_type==2&&nVersion>=2000901)||	//LMA(V2.0.9.1)
		(doc_type==3&&nVersion>=2000200)||	//TSA(V2.0.2.0)
		(doc_type==4&&nVersion>=1020200)||	//LDS(V1.0.2.0)
		(doc_type==5&&nVersion>=1030804))	//TDA(V1.3.8.4)
	{	//�洢ȫ�ֵ�������Ʋ���
		buffer.ReadInteger(&n);
		hashParams.Empty();
		for(i=0;i<n;i++)
		{
			DWORD key;
			buffer.ReadDword(&key);	//�ò����ı�ʶ��ֵ���򲻳���4�������ַ�����
			DESIGN_PARAM_ITEM* pItem=hashParams.Add(key);
			buffer.ReadString(pItem->sLabel);	//����������ʾ�Ķ����Ʊ�ǩ
			buffer.ReadString(pItem->sNotes);	//�����жԸ������ԵĽ���������
			buffer.ReadByte(&pItem->dataType);	//0.double��ֵ 1.��� 2. ����
			buffer.Read(&pItem->value.fValue,8);
		}
		//�洢�����ܿ���Ϣ
		buffer.ReadInteger(&n);
		hashSlaveNodes.Empty();
		for(i=0;i<n;i++)
		{
			buffer.ReadInteger(&h);
			SLAVE_NODE* pSlaveNode=hashSlaveNodes.Add(h);
			pSlaveNode->FromBuffer(buffer,nVersion,doc_type);
		}
	}
	// ------------ 2. vvvvvvvv ���� --------------
	buffer.ReadInteger(&n);
	bool bReplaceQ420BriefMark=false;
	if(  nVersion>=0&&
		(doc_type==1&&nVersion<4010003)||	//TMA V4.1.0.3
		(doc_type==2&&nVersion<1010003)||	//LMA V1.1.0.3
		(doc_type==3&&nVersion<1070309)||	//TSA V1.7.3.9
		(doc_type==4&&nVersion<1000309))	//LDS V1.0.3.9
		bReplaceQ420BriefMark=true;
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		//if(hashObjs.GetHashTableSize()>0&&hashObjs.GetValueAt(h,(CLDSObject*&)pPart))
		if((pPart=Parts.FromHandle(h))!=NULL)
		{
			pPart->FromBuffer(buffer,nVersion,doc_type);
			if(pPart->iSeg.iSeg==0&&strlen(pPart->GetPartNo())>0)
			{	//���ݱ�Ÿ��´�����κţ�ƴ��ģ��ʱ���ܵ��´�������κţ� wjh-2015.5.21
				SEGI iSeg;
				ParsePartNo(pPart->GetPartNo(),&iSeg,NULL);
				pPart->iSeg=iSeg;
			}
			if(bReplaceQ420BriefMark)
			{
				if(pPart->cMaterial=='T')
					pPart->cMaterial=CSteelMatLibrary::Q420BriefMark();
				else if(pPart->cMaterial=='U')
					pPart->cMaterial=CSteelMatLibrary::Q460BriefMark();
			}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
			if(pPart->GetClassTypeId()==CLS_POLYJOINT)
			{
				CLDSPolyJoint *pPolyJoint=(CLDSPolyJoint*)pPart;
				if(pPolyJoint->pPrevLinePart==NULL||pPolyJoint->pNextLinePart==NULL)
					Parts.DeleteNode(pPolyJoint->handle);
			}
#endif
		}
		else
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Wrong data format,can't open file perfectly��";
#else
			throw "���ݸ�ʽ�����޷��������ļ���";
#endif
		if(DisplayProcess!=NULL)	//���ļ��ﵽ��ʮM��Сʱ�����ֱ�ӳ�100�ٳ����ļ���С���׳����������ޣ����¸������� wjh 2011.7.23
			DisplayProcess(progress.Mapping(buffer.GetCursorPosition()*process_scale),progress.title);
	}
	for(CLDSPlate* pPlate=(CLDSPlate*)Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Parts.GetNext(CLS_PLATE))
	{
		if(!pPlate->gasketEdgeSpace.initialized)
			pPlate->gasketEdgeSpace.initialized=(pPlate->wiType!=CLDSPlate::WASHER_PLATE);
		if(!pPlate->gasketEdgeSpace.initialized&&pPlate->GetLsCount()>1)
		{	//�ɰ��ļ��ж����˨���ȱ�ٵ��߾���Ʋ�������Ҫ�Զ���ȡ wjh-2017.12.8
			//��������˨����е���˨�п���λ�ڵ��֮��˿���ȡ�������п��ܳ�������������ʼ�� wjh-2018.6.11
			if(!pPlate->IntelliRetrieveGasketEdgeSpace(NULL))
			{	//û�л�׼�Ǹ�ʱ������Сֱ����˨��Ӧ�ĵ��߾��ʼ��
				int min_d=0;
				for(CLsRef* pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
				{
					if(min_d==0)
						min_d=pLsRef->pBolt->d;
					else if(pLsRef->pBolt->d<min_d)
						min_d=pLsRef->pBolt->d;
				}
				LSSPACEEX space;
				IBoltLibrary::GetLsSpaceEx(min_d,&space);
				pPlate->gasketEdgeSpace.endSpaceS=pPlate->gasketEdgeSpace.endSpaceE=
					pPlate->gasketEdgeSpace.wingSpace=pPlate->gasketEdgeSpace.berSpace=(short)space.WasherEdgeSpace;
			}
		}
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	if(doc_type!=3)
	{
		buffer.ReadInteger(&n);
		for(i=0;i<n;i++)
		{
			CSupplementPart *pSuppPart=SuppPart.append(FALSE);
			buffer.ReadInteger(&pSuppPart->handle);
			pSuppPart->FromBuffer(buffer,nVersion,doc_type);
		}
		if(DisplayProcess!=NULL)
			DisplayProcess(ftoi(buffer.GetCursorPosition()/(buffer_len*0.01)),NULL);
		buffer.ReadInteger(&n);
		for(i=0;i<n;i++)
		{
			CLDSPartGroup *pPartGroup=PartGroup.append();
			pPartGroup->FromBuffer(buffer,nVersion,doc_type);
			if(DisplayProcess!=NULL)
				DisplayProcess(ftoi(buffer.GetCursorPosition()/(buffer_len*0.01)),NULL);
		}
		
		for(CLDSParamPlate *pParamPlate=(CLDSParamPlate*)Parts.GetFirst(CLS_PARAMPLATE);pParamPlate;pParamPlate=(CLDSParamPlate*)Parts.GetNext(CLS_PARAMPLATE))
		{
			for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
			{
				if(hashObjs.GetNodeNum()>0)
					pPart=(CLDSPart*)hashObjs.GetValue(pItem->h);
				else
					pPart=Parts.FromHandle(pItem->h);
				if(pPart)
					strcpy(pItem->sPartNo,pPart->GetPartNo());
			}
		}
	}
#endif
#ifdef __PART_DESIGN_INC_
	if(doc_type==3||doc_type==4)
	{
		if(nVersion==0||(doc_type==3&&nVersion>=1080101)||(doc_type==4&&nVersion>=1010101))
		{
			buffer.ReadInteger(&n);
			Elems.Empty();
			CElemInfo::ClearInitialGlobalId();
			ElemPtrArr.SetSize(0,n);
			for(i=0;i<n;i++)
			{
				long hStartNode,hEndNode;
				buffer.ReadInteger(&hStartNode);
				buffer.ReadInteger(&hEndNode);
				CElemInfo *pElemInfo=Elems.Add(hStartNode,hEndNode);
				pElemInfo->SetBelongModel(this);
				pElemInfo->Id=CElemInfo::GetNewId();	//������ͼ����ʾ������������ʱ��ʶ������ֻҪ��֤ͬһʱ���ȫ��Ψһ����
				ElemPtrArr.append(pElemInfo);
				pElemInfo->m_hStartNode=hStartNode;
				pElemInfo->m_hEndNode=hEndNode;
				pElemInfo->FromBuffer(buffer,nVersion,doc_type);
			}
		}

		if(version>0&&((doc_type==3&&nVersion<2000101)||(doc_type==4&&nVersion<1020101)))
		{
			CBuffer weather_buf(64);
			((WEATHER*)&weather)->ToBuffer(weather_buf);
			if(WriteWireLayoutWeather==NULL||ReadWireLayoutData==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr..LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"WireLayoutEvolver.dll failed load,can't analyze old LDS file format.");
#else
				logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"WireLayoutEvolver.dll����ʧ�ܣ��޷������ɰ�LDS�ļ���ʽ");
#endif
				GetMinAvailableHandle();	//�����ʱ����δ������С����ᵼ�º�������ظ� wjh-2016.12.15
				return false;
			}
			WriteWireLayoutWeather(weather_buf.GetBufferPtr(),weather_buf.GetLength());
			DWORD dwRead=ReadWireLayoutData(buffer.GetCursorBuffer(),buffer.GetRemnantSize(),nVersion,doc_type);
			buffer.Offset(dwRead);
		}
		//����״��
		n=0;
		buffer.ReadInteger(&n);
		if(nVersion==0||nVersion>=1000003)//version==NULL||compareVersion(version,"1.0.0.3")>=0)
		{
			pObjNoGroup=NoManager.FromGroupID(WORKSTATUS_GROUP);
			pObjNoGroup->EmptyNo();
			for(i=0;i<n;i++)
			{
				CWorkStatus *pStatus=WorkStatus.append();
				buffer.ReadInteger(&h);
				pStatus->handle=h;
				pStatus->FromBuffer(buffer,nVersion,doc_type);
				pObjNoGroup->SetNoState(pStatus->iNo);
			}
		}
		if(nVersion==0||nVersion>=1000003)//version==NULL||compareVersion(version,"1.0.0.3")>=0)
			buffer.ReadInteger(&m_hActiveStatus);
		//���ӷ����
		buffer.ReadInteger(&n);
		pObjNoGroup=NoManager.FromGroupID(WORKSTATUS_ATTACH_WINDLOAD);
		for(i=0;i<n;i++)
		{
			CAttachWindLoad *pAttachWindLoad=AttachWindLoad.append();
			buffer.ReadInteger(&h);
			pAttachWindLoad->handle=h;
			pAttachWindLoad->FromBuffer(buffer,nVersion,doc_type);
			pObjNoGroup->SetNoState(pAttachWindLoad->iNo);
		}
		//��ѹ����Ϣ
		buffer.ReadInteger(&n);
		pObjNoGroup=NoManager.FromGroupID(WINDSEG_GROUP);
		pObjNoGroup->EmptyNo();
		for(i=0;i<n;i++)
		{
			CWindSegment *pWindSeg=WindLoadSeg.append();
			pWindSeg->FromBuffer(buffer,nVersion,doc_type);
			pObjNoGroup->SetNoState(pWindSeg->iNo);
		}
		//�ֹܷ��������Ϣ
		if(nVersion==0||(doc_type==3&&nVersion>=1070201)||(doc_type==4&&nVersion>=1000201))
		{
			hashConnectFl.Empty();
			buffer.ReadInteger(&n);
			for(i=0;i<n;i++)
			{
				CConnectFL *pFL=hashConnectFl.append();
				buffer.ReadDouble(&pFL->m_fThickTube);			//���ܱں�(mm)
				buffer.ReadDouble(&pFL->m_fInnerRadius);			//��׼�����ڻ��뾶(mm)
				buffer.ReadDouble(&pFL->m_fCoupleInnerRadius);	//��Է����ڻ��뾶(mm)
				if(nVersion==0||(doc_type==3&&nVersion>=1070306)||(doc_type==4&&nVersion>=1000306))
					buffer.ReadString(pFL->bolt_grade,8);
				buffer.ReadInteger(&pFL->bolt_d);			//��˨ֱ�������
				buffer.ReadInteger(&pFL->bolt_n);			//��˨���е���˨����
				buffer.ReadDouble(&pFL->N);
				buffer.ReadDouble(&pFL->M);				//N ������(N),M���(N*m)
				buffer.ReadDouble(&pFL->m_fBoltRadius);	//��˨Բ�뾶(mm)
				buffer.ReadInteger(&pFL->m_nThickFL);		//�����̺��(mm)
				buffer.ReadInteger(&pFL->m_nWidthFL);		//�������δ���(mm)
				buffer.ReadInteger(&pFL->m_nThickRib);		//�߰���(mm),���Ϊ0��ʾ�޼Ӿ���
				buffer.ReadInteger(&pFL->m_nHeightRib);		//�߰�߶�(mm)
				if(nVersion==0||(doc_type==3&&nVersion>=1070306)||(doc_type==4&&nVersion>=1000306))
				{
					buffer.ReadInteger(&pFL->m_nRibS);			//�߰庸���нǸ�(��)��(mm)
					buffer.ReadInteger(&pFL->m_nCoupleRibS);	//����߰庸���нǸ�(��)��(mm)
				}
				buffer.ReadInteger(&pFL->m_nCoupleThickFL);	//�����̺��(mm)
				buffer.ReadInteger(&pFL->m_nCoupleHeightRib);//����߰�߶�(mm)
				buffer.ReadByte(&pFL->cMaterial);
				buffer.ReadString(pFL->m_sPartNo);     //�������
			}
		}
		if(nVersion==0||(doc_type==3&&nVersion>=1070304)||(doc_type==4&&nVersion>=1000304))
		{
			buffer.ReadByte(&EarthQuake.grade);	//�����Ҷ�6,7,8,9
			buffer.ReadDouble(&EarthQuake.alfa_max);//ˮƽ����Ӱ��ϵ�����ֵ
			buffer.ReadByte(&EarthQuake.cLandType);	//�������1,2,3,4
			buffer.ReadByte(&EarthQuake.cDesignGroup);	//��Ƶ������1,2,3
			buffer.ReadDouble(&EarthQuake.Tg);			//��������,s
			buffer.ReadDouble(&EarthQuake.zeta);		//����ϵ����=0.02s
			buffer.ReadDouble(&EarthQuake.gamaRE);		//�������������ϵ��,һ���Խ��ȡ0.85��������ȡ0.8
			buffer.ReadInteger(&EarthQuake.nFreqsRank);
		}
	}
	//��ͷ��Ϣ
	if((doc_type==3&&(nVersion==0||nVersion>=2000200))||(doc_type==4&&(nVersion==0||nVersion>=1020200)))
	{
		int nJointNum=0;
		buffer.ReadInteger(&nJointNum);
		hashAngleJoint.Clean();
		for (i=0;i<nJointNum;i++)
		{
			int nJointId=0;
			buffer.ReadInteger(&nJointId);
			ANGLE_JOINT *pJoint=hashAngleJoint.Add(nJointId);
			pJoint->FromBuffer(buffer);
		}
	}
#endif
#ifdef __COMMON_PART_INC_
	if( nVersion==0||	//����֮ǰ������趨��__COMPLEX_PART_INC_��ʹT/MA�޷���¼Ӱ��������ʰ汾�Ÿ�Ϊ5.0.0.1 wjh-2015.3.9
		(doc_type==1&&nVersion>=5000003)||//version>=4020003)||
		(doc_type==2&&nVersion>=1020003)||
		(doc_type==3&&nVersion>=1080103)||
		(doc_type==4&&nVersion>=1020003)||
		(doc_type==5&&nVersion>=1020003))
	{	//��ȡ�������������� wht 12-09-10
		n=0;
		buffer.ReadInteger(&n);
		Block.Empty();
		for(i=0;i<n;i++)
		{
			CBlockModel *pBlock=Block.append(FALSE);
			pBlock->DisplayProcess=DisplayProcess;	//���ò�����ʾ���ȵĻص�����
			buffer.ReadInteger(&pBlock->handle);
			pBlock->FromBuffer(buffer,nVersion,doc_type);
			if(DisplayProcess!=NULL)
				DisplayProcess(ftoi(buffer.GetCursorPosition()/(buffer_len*0.01)),NULL);
		}
		n=0;
		buffer.ReadInteger(&n);
		BlockRef.Empty();
		for(i=0;i<n;i++)
		{
			CBlockReference *pBlockRef=BlockRef.append(FALSE);
			buffer.ReadInteger(&pBlockRef->handle);
			pBlockRef->FromBuffer(buffer,nVersion,doc_type);
			if(DisplayProcess!=NULL)
				DisplayProcess(ftoi(buffer.GetCursorPosition()/(buffer_len*0.01)),NULL);
		}
	}
#endif
	if( nVersion==0||
		(doc_type==1&&nVersion>=4020004)||	//TMA(V4.2.0.4)
		(doc_type==2&&nVersion>=1020004)||	//LMA(V1.2.0.4)
		(doc_type==3&&nVersion>=1080104)||	//TSA(V1.8.1.4)
		(doc_type==4&&nVersion>=1020004)||	//LDS(V1.2.0.4)
		(doc_type==5&&nVersion>=1020004))	//TDA(V1.2.0.4)
	{	//��ȡ����������ϵ wht 13-01-06
		n=0;
		buffer.ReadInteger(&n);
		Acs.Empty();
		//CObjNoGroup *pAcsNoGroup=NoManager.FromGroupID(PARAMETRIC_CS_GROUP);
		for (i=0;i<n;i++)
		{
			int id=0;
			buffer.ReadInteger(&id);
			CParaCS *pCS = Acs.Add(id);
			pCS->id = id;
			pCS->FromBuffer(buffer,nVersion,doc_type);
			//pAcsNoGroup->SetNoState(id);
			if(DisplayProcess!=NULL)
				DisplayProcess(ftoi(buffer.GetCursorPosition()/(buffer_len*0.01)),NULL);
		}
	}
	if( nVersion==0||
		(doc_type==1&&nVersion>=5010901)||	//TMA(V5.2.0.0)
		(doc_type==2&&nVersion>=2000901)||	//LMA(V2.1.0.0)
		(doc_type==3&&nVersion>=2010303)||	//TSA(V2.1.3.3)
		(doc_type==4&&nVersion>=1030803)||	//LDS(V1.3.8.3)
		(doc_type==5&&nVersion>=1030803))	//TDA(V1.3.8.3)
	{	//��ȡ������������ wht 18-08-06
		//������Ʋ���
		buffer.ReadByte(&m_xFoundation.biBasePlateType);	//0.������ƽ�� 1.�����м� 2.������ƽ��
		buffer.ReadWord(&m_xFoundation.wiBasePlateThick);	//������
		buffer.ReadWord(&m_xFoundation.wiBasePlateWidth);	//������
		buffer.ReadWord(&m_xFoundation.wiBasePlateHoleD);	//����׾�
		buffer.ReadByte(&m_xFoundation.biLayoutType);		//�ؽ���˨�������ͣ�0.4��˨ 1.8��˨
		buffer.ReadWord(&m_xFoundation.wiBoltD);			//�ؽ���˨ֱ��
		buffer.ReadWord(&m_xFoundation.wiBoltSapce);		//��˨����
		buffer.ReadWord(&m_xFoundation.S2)			;		//��˨���S2(���ڰ˵ؽ���˨��
		buffer.ReadWord(&m_xFoundation.wiFoundationWidth);	//�������
		if((doc_type==4|| doc_type ==5)&&(nVersion==0|| nVersion >= 1030901))
		{	//�������Ű�ѥ��/�߰���Ʋ���
			buffer.ReadByte(&m_xFoundation.cnMainAngleBoltNum);
			buffer.ReadByte(&m_xFoundation.cnMaxSingularRowBolts);
			buffer.ReadByte(&m_xFoundation.cnRows);	//����������˨������������
			buffer.ReadWord(&m_xFoundation.wiShoeThick);
			buffer.ReadWord(&m_xFoundation.wiRibThick);
			buffer.ReadWord(&m_xFoundation.wiRibMargin);				//�װ��ȡ�ѥ���ȣ��߰��ȡ��߾�
			buffer.ReadWord(&m_xFoundation.wRibWidth);
			buffer.ReadWord(&m_xFoundation.wRibOuterHeight);
			buffer.ReadWord(&m_xFoundation.wRibInnerHeight);		//�߰���(�װ庸�ӱ�),�߶ȣ�����࣬�ڲࣩ
		}
		n=0;
		m_xFoundation.hashPostRodFlags.Empty();
		buffer.ReadInteger(&n);//���ڴ洢���������ľ༰���ȸ������Զ������趨
		for(i=0;i<n;i++)
		{
			buffer.ReadInteger(&h);
			POSTRODFLAG* pFlag=m_xFoundation.hashPostRodFlags.Add(h);
			buffer.ReadByte(&pFlag->m_cFlag);
		}
		n=0;
		buffer.ReadInteger(&n);
		Foundations.Empty();
		CHashSet<CLDSFoundation*> hashFoundations;
		for (i=0;i<n;i++)
		{
			int id=0;
			CLDSFoundation *pFoundation = Foundations.append(FALSE);
			buffer.ReadInteger(&id);
			pFoundation->handle = id;
			pFoundation->FromBuffer(buffer,nVersion,doc_type);
			//��ʱ����ȫ�ؽ���˨����ǰδͳ��MinAvailableHandle���¾���ظ� wjh-2019.8.21
			pFoundation->AddOrUpdateAnchorBolts(m_xFoundation.wiBasePlateThick,true,false);
			if(hashFoundations.GetValue(KEY4C(pFoundation->ciHeightSerial,pFoundation->ciLegSerial,pFoundation->ciLegQuad))==NULL)
				hashFoundations.SetValue(KEY4C(pFoundation->ciHeightSerial,pFoundation->ciLegSerial,pFoundation->ciLegQuad),pFoundation);
			else	//���������ظ��Ļ���
			{
				for(WORD wi=0;wi<pFoundation->anchors.Count;wi++)
					Parts.DeleteNode(pFoundation->anchors[wi].hAnchorBolt);
				Foundations.DeleteCursor();
			}
			if(DisplayProcess!=NULL)
				DisplayProcess(ftoi(buffer.GetCursorPosition()/(buffer_len*0.01)),NULL);
		}
	}
	GetMinAvailableHandle();
	//���������ޱ�����ĺ��������ޱ�Ҫ�洢���°汾���洢���ϰ汾��ִ������Ĵ��룬���ע�͵�wjh 2010-12-20
	//buffer.ReadInteger(&handle);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	//�����ɰ汾�ļ��еĴ��� xc 2010-8-9
	CorrectErrorBefore(nVersion,doc_type,bHasRepeatHandle);
#endif
	/*for(pDispView=View.GetFirst();pDispView;pDispView=View.GetNext())
	{
		if(pDispView->m_iViewType!=1)
			continue;
		for(CViewPart *pViewPart=pDispView->partset.GetFirst();pViewPart;pViewPart=pDispView->partset.GetNext())
		{
			if(pViewPart->idClassType==CLS_NODE)
				continue;
			pPart=Parts.FromHandle(pViewPart->handle,CLS_BOLT);
			if(pPart&&pPart->m_bVirtualPart)
				pDispView->partset.DeleteCursor();
		}
		pDispView->partset.Clean();
	}*/
	for(pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{	//����Ӱ��ڵ�λ�� wht 12-09-24
		if(pNode->IsShadowObject())
			pNode->Set();
	}
	//�����ֹ����麸�ְ�ĶԻ��־��λ��
	AmendPlateWeldLocationY();
	if(DisplayProcess!=NULL)
		DisplayProcess(100,NULL);
	//TODO:��ʱ���ô��� wjh-2018.11.9
	//InitTowerModelTmplInfo(this);
	return true;
}

#include "BufferStack.h"
COldVerDataFile::SIMPLE_DATUMPOINT::SIMPLE_DATUMPOINT(long _hNode/*=0*/,long _hNewDatumPoint/*=0*/)
{
	ciHidType=1;
	hNodeOrRod=_hNode;
	hNewTmpDatumPoint=_hNewDatumPoint;
}
void COldVerDataFile::Empty()
{
	hashGroupAngles.Empty();
	hashTmpDatumPoints.Empty();
}
char COldVerDataFile::SafeAngleWing(long hGroupAngle,char cAngleWing,long doc_type,long version)
{
	GROUP_ANGLE* pGroupAngleInfo=hashGroupAngles.GetValue(hGroupAngle);
	if(pGroupAngleInfo==NULL)
		return cAngleWing;	//����Ҫת��
	if(cAngleWing=='X')
		return 'Y';
	else if(cAngleWing=='x')
		return 'y';
	else if(cAngleWing=='Y')
		return 'x';
	else if(cAngleWing=='y')
		return 'X';
	else
		return cAngleWing;
}
void CTower::PrepareOldVerInfo(COldVerDataFile* pOldVerInfo,int doc_type,long version)
{
	long hCurrIdleHandle=GetNewHandle();
	//׼����������
	if(  version>=0&&
		(doc_type==1&&version<5020000)||(doc_type==2&&version<2010000)||	//TMA V5.2.0.0|LMA V2.1.0.0
		(doc_type==4&&version<1030804)||(doc_type==5&&version<1030804))		//LDS V1.3.8.4|TDA V1.3.8.4
	{
		COldVerDataFile::SIMPLE_DATUMPOINT* pTmpPoint;
		for(CLDSPlane* pPlane=Plane.GetFirst();pPlane;pPlane=Plane.GetNext())
		{
			for(int i=0;i<3;i++)
			{
				if(pPlane->hPointArr[i].bSimpleNodePoint)
				{
					pTmpPoint=pOldVerInfo->hashTmpDatumPoints.Add(pPlane->hPointArr[i].handle);
					if(pTmpPoint->hNewTmpDatumPoint!=0)
						continue;
					pTmpPoint->ciHidType=1;
					pTmpPoint->hNewTmpDatumPoint=hCurrIdleHandle;
					hCurrIdleHandle++;
				}
				if(pPlane->hPointArr[i].bRodTipStart)
				{
					pTmpPoint=pOldVerInfo->hashTmpDatumPoints.Add(pPlane->hPointArr[i].handle);
					pTmpPoint->hNewTmpDatumPoint=hCurrIdleHandle;
					hCurrIdleHandle++;
					pTmpPoint->ciHidType='S';
				}
				if(pPlane->hPointArr[i].bRodTipEnd)
				{
					pTmpPoint=pOldVerInfo->hashTmpDatumPoints.Add(pPlane->hPointArr[i].handle);
					pTmpPoint->hNewTmpDatumPoint=hCurrIdleHandle;
					hCurrIdleHandle++;
					pTmpPoint->ciHidType='E';
				}
			}
		}
	}
	//ͳ����Ҫת��GroupAngle.ciMasterIndex(1->0)����ϽǸ�
	SmartRodPtr pRod;
	for(pRod=EnumRodFirst();pRod.IsHasPtr();pRod=EnumRodNext())
	{
		if(pRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			if(pRod.pGroupAngle->ciMasterIndex==1)
			{
				COldVerDataFile::GROUP_ANGLE* pOldGroupAngle=pOldVerInfo->hashGroupAngles.Add(pRod->handle);
				pOldGroupAngle->wiSectStyle=pRod.pGroupAngle->wSectStyle;
			}
		}
	}
}
void CTower::ToBuffer(CBuffer &buffer,DWORD cursor_dog_no,char *file_version,char ciDocType/*=0*/)
{
	CBufferStack bufstack;
	if(buffer.BufferStack()==NULL)
		buffer.AttachStack(&bufstack);
	long i;
	if(ciDocType==1)
		buffer.WriteString("������������");
	else if(ciDocType==2)
		buffer.WriteString("��������������");
	else if(ciDocType==4)
		buffer.WriteString("������������ͼϵͳ");
	else if(ciDocType==5)
		buffer.WriteString("������ͼ����");
	else //if(ciDocType==0)
	{
#if defined(__TMA_FILE_)||defined(__TMA_)
	buffer.WriteString("������������");
#elif defined(__LMA_FILE_)||defined(__LMA_)
	buffer.WriteString("��������������");
#elif defined(__TSA_FILE_)||defined(__TSA_)
	buffer.WriteString("���������������");
#elif defined(__TDA_FILE_)
	buffer.WriteString("������ͼ����");
#else //defined(__LDS_FILE_)||defined(__LDS_)
	buffer.WriteString("������������ͼϵͳ");
#endif
	}
	buffer.WriteString(file_version);
	buffer.WriteInteger(m_uiOriginalDogKey);
	buffer.WriteInteger(cursor_dog_no);
	buffer.WriteString(m_sPrjName);
	buffer.WriteString(m_sTaType);
	buffer.WriteString(m_sTaAlias);
	buffer.WriteString(m_sTaStampNo);
	buffer.WriteDouble(m_fMaxXieJGLen);
	int doc_type=ciDocType;
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	long nVersion=FromStringVersion(file_version);
	CfgwordToBuffer(cfgword,buffer,nVersion,doc_type);
	if (version==0||
		(doc_type==1&&nVersion>=5000000)||	//TMA V5.0.0.0
		(doc_type==2&&nVersion>=2000502)||	//LMA V2.0.5.2
		(doc_type==3&&nVersion>=2010000)||	//TSA V2.1.0.0
		(doc_type==4&&nVersion>=1030001)||	//LDS V1.3.0.1
		(doc_type==5&&nVersion>=1030001))	//TDA V1.3.0.1
	{
		buffer.WriteByte(CFGLEG::Schema());
		if (CFGLEG::Schema()==CFGLEG::MULTILEG_UDF&&(
			//(doc_type==1&&nVersion>=5000000)||	//TMA V5.0.0.0
			//(doc_type==2&&nVersion>=2000502)||	//LMA V2.0.5.2
			//(doc_type==3&&nVersion>=2010000)||	//TSA V2.1.0.0
			(doc_type==4&&nVersion>=1031101)||	//LDS V1.3.11.1
			(doc_type==5&&nVersion>=1031101)))	//TDA V1.3.11.1
		{
			buffer.WriteByte(CFGLEG::xUdfSchema.cnHeightCount);
			buffer.Write(CFGLEG::xUdfSchema.xarrHeights,CFGLEG::xUdfSchema.cnHeightCount*sizeof(UDF_HEIGHTLEGS));
		}
	}
#ifdef  __COMMON_PART_INC_
	if(  version==0||
		(doc_type==1&&nVersion>=5000200)||	//TMA V5.0.2.0
		(doc_type==2&&nVersion>=2000200)||	//LMA V2.0.2.0
		//(doc_type==3&&nVersion>=2010100)||//TSA V2.1.1.0
		(doc_type==4&&nVersion>=1030200)||	//LDS V1.3.2.0
		(doc_type==5&&nVersion>=1030200))	//TDA V1.3.2.0
		buffer.WriteByte(m_ciRollProcessType);
	if(	version==0||
		(doc_type==1&&nVersion>=5010500)||	//TMA V5.1.5.0
		(doc_type==2&&nVersion>=2000800)||	//LMA V2.0.8.0
		(doc_type==4&&nVersion>=1030700)||	//LDS V1.3.7.0
		(doc_type==5&&nVersion>=1030700))	//TDA V1.3.7.0
	{
		buffer.WriteByte(m_ciRollOffsetType);
		buffer.WriteByte(m_ciDeformType);
	}
	if(	version==0||
		(doc_type==1&&nVersion>=5010600)||	//TMA V5.1.6.0
		(doc_type==2&&nVersion>=2000800)||	//LMA V2.0.8.0
		(doc_type==4&&nVersion>=1030800)||	//LDS V1.3.8.0
		(doc_type==5&&nVersion>=1030800))	//TDA V1.3.8.0
	{
		BYTE cnThickItemCount=(BYTE)mapNeutroCoef.ColCount;
		BYTE cnAngleItemCount=(BYTE)mapNeutroCoef.RowCount;
		buffer.WriteByte(cnThickItemCount);
		buffer.WriteByte(cnAngleItemCount);
		if(cnAngleItemCount<1||cnThickItemCount<1)
		{
			buffer.WriteDouble(m_fDeformCoef);
			buffer.WriteDouble(m_fHuoquR);
		}
		else
		{	//������Ϊ�������飬������Ϊ����Ƕȷ���(��)
			buffer.Write(mapNeutroCoef.RowHeader(),sizeof(double)*cnThickItemCount);
			buffer.Write(mapNeutroCoef.ColHeader(),sizeof(double)*cnAngleItemCount);
			//д�����Բ�ϵ����ֵ��
			buffer.Write(mapNeutroCoef.MapData,sizeof(double)*cnThickItemCount*cnAngleItemCount);
			//д����������Բ����ֵ��
			buffer.Write(mapBendR.MapData,sizeof(double)*cnThickItemCount*cnAngleItemCount);
		}
	}
#endif
	if(   (doc_type==1&&(nVersion>0&&nVersion<4020002))	//TMA V4.2.0.2
		||(doc_type==2&&(nVersion>0&&nVersion<1020002)) //LMA V1.2.0.2
		||(doc_type==3&&(nVersion>0&&nVersion<1080002)) //TSA V1.8.0.2
		||(doc_type==4&&(nVersion>0&&nVersion<1020002))	//LDS V1.2.0.2
		||(doc_type==5&&(nVersion>0&&nVersion<1020002)))//TDA V1.2.0.2
		buffer.WriteInteger(1);//m_iPlankWeightStatType);
	buffer.WriteString(m_sOperator);	//����Ա==���Ʊ���
	buffer.WriteString(m_sAuditor);		//���Ա==�������
	if(	version==0&&doc_type!=3||
		(doc_type==1&&nVersion>=5010200)||	//TMA V5.1.2.0
		(doc_type==2&&nVersion>=2000502)||	//LMA V2.0.5.2
		(doc_type==4&&nVersion>=1030502)||	//LDS V1.3.5.2
		(doc_type==5&&nVersion>=1030502))	//TDA V1.3.5.2
	{
		if(	version==0&&doc_type!=3||
			(doc_type==1&&nVersion<5010700)||	//TMA V5.1.7.0
			(doc_type==2&&nVersion<2000800)||	//LMA V2.0.8.0
			(doc_type==4&&nVersion<1030800)||	//LDS V1.3.8.0
			(doc_type==5&&nVersion<1030800))	//TDA V1.3.8.0
			buffer.WriteByte(m_ciRepositionAlgor>1?0:m_ciRepositionAlgor);	
		else
			buffer.WriteByte(m_ciRepositionAlgor);
	}
	if(	version==0&&doc_type!=3||
		(doc_type==1&&nVersion>=5010800)||	//TMA V5.1.7.0
		(doc_type==2&&nVersion>=2000800)||	//LMA V2.0.8.0
		(doc_type==4&&nVersion>=1030900)||	//LDS V1.3.8.0
		(doc_type==5&&nVersion>=1030800))	//TDA V1.3.8.0
	{
		buffer.WriteInteger(hashConstraints.GetNodeNum());
		for(CONSTRAINT* pConstraint=hashConstraints.GetFirst();pConstraint;pConstraint=hashConstraints.GetNext())
		{
			buffer.WriteInteger(pConstraint->Id);
			buffer.WriteInteger(pConstraint->hMasterObj);
			buffer.WriteWord(pConstraint->wiMasterPropId);
			buffer.WriteInteger(pConstraint->hAcceptObj);
			buffer.WriteWord(pConstraint->wiAcceptPropId);
			buffer.WriteBooleanThin(pConstraint->dual);
			buffer.WriteString(pConstraint->expression);
		}
	}
#ifdef __PART_DESIGN_INC_
	if(doc_type==3||doc_type==4)
	{
		char company[101]="";
		_snprintf(company,100,"%s",m_sCompanyName);
		buffer.WriteString(company);		//��Ƶ�λ
		weather.ToBuffer(buffer);	//��������
		buffer.WriteInteger(volt_grade);	//��ѹ�ȼ�:330,500
		buffer.WriteByte(m_cLandscape);		//��λ����:'A','B','C'
		buffer.WriteDouble(m_fTowerHeight);
		/*��ȫ�ȼ�(�����ṹ��Ҫ��ϵ��gama0)��
		0:һ���ر���Ҫ�ĸ����ṹ(gama0=1.1)��
		1:����������ѹ��·�ĸ������(gama0=1.0)��
		2:������ʱʹ�õĸ������(gama0=0.9).*/
		buffer.WriteWord(m_siSafetyGrade);
		buffer.WriteDouble(m_fSafetyCoef);
		buffer.WriteDouble(m_fIceDensity);	//���������ض�kg/m3
		if((doc_type==3&&(nVersion==0||nVersion>=2000200))||(doc_type==4&&(nVersion==0||nVersion>=1020200)))
			buffer.WriteByte(m_cStructureType);
		//ѡ������
		buffer.WriteInteger(m_iLoadExecuteCode);	//���ؼ���ִ�й淶
		buffer.WriteInteger(m_iMemberSizeCheckCode);	//���������Ż�ѡ��ִ�й淶
		buffer.WriteInteger(m_iSlendernessLimCode);	//�˼���ϸ�����ƹ淶
		buffer.WriteDouble(m_fLimitBearCoef);			//����ϵ��������ֵ
		buffer.WriteDouble(m_fGamaGForDesign);		//�������ʱ�������ط���ϵ��
		buffer.WriteDouble(m_fGamaGForFoundation);	//�������ʱ�������ط���ϵ��
		buffer.WriteDouble(m_fGamaQForTowerWindLoad);//�����������ӷ���ط���ϵ��
		if (nVersion==0||(doc_type==4&&nVersion>=1031000))//LDS V1,3,10,0
		{
			buffer.WriteBooleanThin(m_bWeightAmplifyCoefIncBraceRods);	//��ѹ������ϵ�����Ƿ�������ĸ˼� wjh-2019.7.13
			buffer.WriteBooleanThin(m_bAreaAmplifyCoefIncBraceRods);	//��ѹ������Ŵ�ϵ�����Ƿ�������ĸ˼� wjh-2019.7.13
		}
		buffer.WriteInteger(SelMatPara.m_nMainPoleSlendernessLimit);
		buffer.WriteInteger(SelMatPara.m_nXiePoleSlendernessLimit);
		buffer.WriteInteger(SelMatPara.m_nAuxPoleSlendernessLimit);
		buffer.WriteInteger(SelMatPara.m_nTensivePoleSlendernessLimit);
		buffer.WriteInteger(SelMatPara.m_nMainTubeSlendernessLimit);
		buffer.WriteInteger(SelMatPara.m_nXieTubeSlendernessLimit);
		buffer.WriteInteger(SelMatPara.m_nAuxTubeSlendernessLimit);
		buffer.WriteInteger(SelMatPara.m_nTensiveTubeSlendernessLimit);
		buffer.WriteInteger(SelMatPara.m_bPreferSingleAngle);
		buffer.WriteInteger(SelMatPara.m_iSizeOrderRule);
		buffer.WriteInteger(SelMatPara.m_nMaxSelMatCount) ;
		buffer.WriteInteger(SelMatPara.m_nConvergeLim);
		buffer.WriteString(SelMatPara.m_csMinAngleSize);
		buffer.WriteString(SelMatPara.m_csMinForceAngleSize);
		buffer.WriteString(SelMatPara.m_csMinMainAngleSize);
	}
#endif
	//׼���
	buffer.WriteWord(zhunju_N);
	for(i=0;i<zhunju_N;i++)
	{
		buffer.WriteWord(zhunju_table[i].wide);	//֫��
		buffer.WriteWord(zhunju_table[i].g);	//��׼׼��g
		buffer.WriteWord(zhunju_table[i].g1);	//׼��g1
		buffer.WriteWord(zhunju_table[i].g2);	//׼��g2
		buffer.WriteWord(zhunju_table[i].g3);	//׼��g3
	}
	//��˨����
	buffer.WriteInteger(lsspace_N);
	for(i=0;i<lsspace_N;i++)
	{
		buffer.WriteInteger(LSSPACE_TABLE[i].M_Diameter);
		buffer.WriteInteger(LSSPACE_TABLE[i].SingleRowSpace);
		buffer.WriteInteger(LSSPACE_TABLE[i].doubleRowSpace);
		buffer.WriteInteger(LSSPACE_TABLE[i].EndSpace);
		buffer.WriteInteger(LSSPACE_TABLE[i].PlateEdgeSpace);
		buffer.WriteInteger(LSSPACE_TABLE[i].RollingEdgeSpace);
		buffer.WriteInteger(LSSPACE_TABLE[i].CutAngleSpace);
		if(  nVersion==0||
			(doc_type==1&&nVersion>=5010500)||
			(doc_type==2&&nVersion>=2000800)||
			(doc_type==4&&nVersion>=1030700)||
			(doc_type==5&&nVersion>=1030700))
		{
			LSSPACEEX space=LSSPACE_TABLE[i];
			IBoltLibrary::GetLsSpaceEx(LSSPACE_TABLE[i].M_Diameter,&space);
			buffer.WriteInteger(space.WasherEdgeSpace);
		}
	}
	//��˨���ϵ����Ϣ
    BUFFERPOP stack(&buffer,CLsLibrary::GetFamilyNum());
	buffer.WriteInteger(CLsLibrary::GetFamilyNum());
	for(CLsFamily* pFamily=CLsLibrary::GetFirstFamily();pFamily;pFamily=CLsLibrary::GetNextFamily())
	{
		if(	nVersion==0&&doc_type!=3||
			(doc_type==1&&nVersion>=5020802)||	//TMA V5,2,0,2
			(doc_type==2&&nVersion>=2010002)||	//LMA V2,1,0,2
			(doc_type==4&&nVersion>=1030901)||	//LDS V1.3.9.1
			(doc_type==5&&nVersion>=1030805))	//TDA V1,3,8,5
			buffer.WriteInteger(pFamily->m_uiCodeSerial);	//��˨��ѭ�淶���
		buffer.WriteString(pFamily->name);
		buffer.WriteInteger(pFamily->GetCount());
		buffer.WriteString(pFamily->descr);
		buffer.WriteInteger(pFamily->m_bAntiLoose);
		buffer.WriteInteger(pFamily->m_bDualCap?2:1);
		buffer.WriteInteger(pFamily->m_bFootNail);
		buffer.WriteInteger(pFamily->m_bBurglarproof);
		if(	nVersion==0&&doc_type!=3||
			(doc_type==1&&nVersion>=5010800)||	//TMA V5.1.8.0
			(doc_type==2&&nVersion>=2000900)||	//LMA V2.0.9.0
			(doc_type==4&&nVersion>=1030900)||	//LDS V1.3.9.0
			(doc_type==5&&nVersion>=1030800))	//TDA V1.3.8.0
		{
			bool bTemporyVersionDisableMappingBoltFamily=false;
			if(	doc_type==1&&nVersion>0&&nVersion<5010900)	//TMA V5.1.8.0
				bTemporyVersionDisableMappingBoltFamily=true;
			if(!bTemporyVersionDisableMappingBoltFamily)
			{
				buffer.WriteInteger(pFamily->m_hMappingAntiTheftFamily);
				buffer.WriteInteger(pFamily->m_hMappingAntiLooseFamily);
			}
			for(int i=0;i<8;i++)
			{
				buffer.WriteWord(pFamily->ls_sub_part[i].idSubPart);
				buffer.WriteWord(pFamily->ls_sub_part[i].nCount);
			}
		}
		for(int j=0;j<pFamily->GetCount();j++)
		{
			LS_XING_HAO *pXingHao=pFamily->RecordAt(j);
			buffer.WriteDouble(pXingHao->grade);			//��˨����
			buffer.WriteInteger(pXingHao->d);			//��˨ֱ��
			buffer.WriteString(pXingHao->guige);			//��˨���
			buffer.WriteInteger(pXingHao->valid_len);	//��Ч��
			buffer.WriteInteger(pXingHao->no_thread_len);//�޿۳�
			buffer.WriteInteger(pXingHao->down_lim);		//��˨ͨ������
			buffer.WriteInteger(pXingHao->up_lim);		//��˨ͨ������
			buffer.WriteInteger(pXingHao->washer_thick);	//��Ȧ���pXingHao->pad_thick
			buffer.WriteDouble(pXingHao->weight);		//��˨��������
		}
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of bolt specification series information is wrong!");
#else
		CLDSObject::Log2File()->Log("��˨���ϵ����Ϣ��¼�����ִ���!");
#endif
	//��˨���ϵ����Ϣ
	if(	 nVersion==0&&doc_type!=3||
		(doc_type==1&&nVersion>=5010900)||	//TMA V5.1.9.0
		(doc_type==2&&nVersion>=2000900)||	//LMA V2.0.9.0
		(doc_type==4&&nVersion>=1030900)||	//LDS V1.3.9.0
		(doc_type==5&&nVersion>=1030800))	//TDA V1.3.8.0
	{	
		stack.Initialize(&buffer,CLsLibrary::GetLsSubPartFamilyNum());
		buffer.WriteInteger(CLsLibrary::GetLsSubPartFamilyNum());
		for(CLsSubPartFamily* pLsSubPart=CLsLibrary::GetFirstLsSubPartFamily();pLsSubPart;pLsSubPart=CLsLibrary::GetNextLsSubPartFamily())
		{
			buffer.WriteWord(pLsSubPart->m_widType);
			buffer.WriteString(pLsSubPart->name);
			stack.Increment();
		}
		if(!stack.VerifyAndRestore())
#ifdef   AFX_TARG_ENU_ENGLISH 
			CLDSObject::Log2File()->Log("The number record of bolt specification selection table is wrong!");
#else
			CLDSObject::Log2File()->Log("��˨���ϵ�б��¼�����ִ���!");
#endif
	}
	//�ؽ���˨��ϵ��
	if(	 nVersion==0&&doc_type!=3||
		(doc_type==1&&nVersion>=5020201)||	//TMA V5.2.2.1
		(doc_type==2&&nVersion>=2010201)||	//LMA V2.1.2.1
		(doc_type==4&&nVersion>=1031101)||	//LDS V1.3.11.1
		(doc_type==5&&nVersion>=1031101))	//TDA V1.3.11.1
	{
		int ii,nCount=ANCHOR_BOLT::GetCount();
		buffer.WriteInteger(nCount);	//��˨���ѡ������
		for(ii=0;ii<nCount;ii++)
		{	
			const ANCHOR_BOLT *pAnchorBolt=ANCHOR_BOLT::RecordAt(ii);
			buffer.WriteWord(pAnchorBolt->d);
			buffer.WriteWord(pAnchorBolt->wiWidth);
			buffer.WriteWord(pAnchorBolt->wiThick);
			buffer.WriteWord(pAnchorBolt->wiHoleIncrement);
			buffer.WriteWord(pAnchorBolt->wiBasePlateHoleD);
			buffer.WriteFloat(pAnchorBolt->m_fNutH);
			buffer.WriteFloat(pAnchorBolt->m_fNutTaper);
			buffer.WriteFloat(pAnchorBolt->m_fNutDe);
			buffer.WriteFloat(pAnchorBolt->m_fNutDs);
			buffer.WriteFloat(pAnchorBolt->m_fLe);
		}
	}
	//
	stack.Initialize(&buffer,listSelBoltCond.GetNodeNum());
	buffer.WriteInteger(listSelBoltCond.GetNodeNum());	//��˨���ѡ������
	for(BOLT_SELCOND *pSelBolt=listSelBoltCond.GetFirst();pSelBolt;pSelBolt=listSelBoltCond.GetNext())
	{
		buffer.WriteInteger(pSelBolt->d);
		//if(strlen(pSelBolt->grade)>4)
		//	pSelBolt->grade[5]=0;	//���ݴ���Ϊ��������������ȡ�ļ����⣬�����ݽ��н�ȡ��wjh 2011.12.2
		buffer.WriteString(pSelBolt->grade,4);
		buffer.WriteString(pSelBolt->matStr,7);
		buffer.WriteInteger(pSelBolt->upWidthLim);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef   AFX_TARG_ENU_ENGLISH 
		CLDSObject::Log2File()->Log("The number record of bolt specification selection table is wrong!");
#else
		CLDSObject::Log2File()->Log("��˨���ѡ����¼�����ִ���!");
#endif
	stack.Initialize(&buffer,CSteelMatLibrary::GetCount());
	buffer.WriteInteger(CSteelMatLibrary::GetCount());
	for(i=0;i<CSteelMatLibrary::GetCount();i++)
	{
		STEELMAT mat=CSteelMatLibrary::RecordAt(i);
		buffer.WriteString(mat.mark);
		buffer.WriteByte(mat.cBriefMark);
		if ((version == 0 && doc_type != 3) ||
			(doc_type == 1 && nVersion >= 5020100) ||	//TMA V5.2.1.0
			(doc_type == 2 && nVersion >= 2010100) ||	//LMA V2.1.1.0
			(doc_type == 4 && nVersion >= 1030900) ||	//LDS V1.3.9.0
			(doc_type == 5 && nVersion >= 1030900))		//TDA V1.3.9.0
			buffer.WriteByte(mat.cPreferQualityLevel);
		buffer.WriteDouble(mat.E);
		buffer.WriteDouble(mat.price_coef);
		buffer.Write(mat.t_lim,sizeof(int)*3);
		buffer.Write(mat.intensity,sizeof(double)*16);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of Material library is wrong!");
#else
		CLDSObject::Log2File()->Log("���ʿ��¼�����ִ���!");
#endif
#if defined(__TSA_FILE_)||defined(__LDS_FILE_)
	if(doc_type==3||doc_type==4)
	{
		buffer.WriteInteger(jgguige_N);
		for(i=0;i<jgguige_N;i++)
		{
			buffer.WriteDouble(jgguige_table[i].wing_wide);
			buffer.WriteByte(jgguige_table[i].cType);
			buffer.WriteDouble(jgguige_table[i].wing_thick);
			buffer.WriteDouble(jgguige_table[i].r);
			buffer.WriteDouble(jgguige_table[i].area);
			buffer.WriteDouble(jgguige_table[i].theroy_weight);
			buffer.WriteDouble(jgguige_table[i].Ix);
			buffer.WriteDouble(jgguige_table[i].Rx);
			buffer.WriteDouble(jgguige_table[i].Wx);
			buffer.WriteDouble(jgguige_table[i].Iy0);
			buffer.WriteDouble(jgguige_table[i].Ry0);
			buffer.WriteDouble(jgguige_table[i].Wy0);
		}
	}
#endif
#if !defined(__TMA_FILE_)
	if(doc_type!=1)
	{
		buffer.WriteInteger(tubeguige_N);
		for(i=0;i<tubeguige_N;i++)
		{
			buffer.WriteDouble(tubeguige_table[i].D);
			buffer.WriteDouble(tubeguige_table[i].thick);
			buffer.WriteDouble(tubeguige_table[i].area);
			buffer.WriteDouble(tubeguige_table[i].theroy_weight);
			buffer.WriteDouble(tubeguige_table[i].I);
			buffer.WriteDouble(tubeguige_table[i].r);
			buffer.WriteDouble(tubeguige_table[i].W);
		}
	}
#endif
	//LMA��LDS֧�ֱ���ֹܶ����ӱ�׼�� wht 11-05-03
#ifdef __COMPLEX_PART_INC_//defined(__LMA_FILE_)||defined(__LDS_FILE_)
	if(doc_type!=1&&doc_type!=3)
		WriteStdPartLibToBuffer(buffer);
#endif
	//////////////////////////
	//Ԥ����ɰ汾ת����Ϣ
	COldVerDataFile xTmpOldVersion;
	PrepareOldVerInfo(&xTmpOldVersion,doc_type,nVersion);
	//////////////////////////
	//��ϣ�������Ϣ����
	int objsN=0;
	buffer.LogPosition();
	buffer.WriteInteger(objsN);
	CLDSPoint *pPoint=NULL;
	for(pPoint=Point.GetFirst();pPoint;pPoint=Point.GetNext())
	{
		buffer.WriteInteger(pPoint->handle);
		buffer.WriteInteger(CLS_GEPOINT);
		objsN++;
	}

	COldVerDataFile::SIMPLE_DATUMPOINT* pSimpleDatumPoint;
	for(pSimpleDatumPoint=xTmpOldVersion.hashTmpDatumPoints.GetFirst();pSimpleDatumPoint;
		pSimpleDatumPoint=xTmpOldVersion.hashTmpDatumPoints.GetNext())
	{
		buffer.WriteInteger(pSimpleDatumPoint->hNewTmpDatumPoint);
		buffer.WriteInteger(CLS_GEPOINT);
		objsN++;
	}

	CLDSLine *pLine=NULL;
	for(pLine=Line.GetFirst();pLine;pLine=Line.GetNext())
	{
		buffer.WriteInteger(pLine->handle);
		buffer.WriteInteger(CLS_GELINE);
		objsN++;
	}
	CLDSPlane *pPlane=NULL;
	for(pPlane=Plane.GetFirst();pPlane;pPlane=Plane.GetNext())
	{
		buffer.WriteInteger(pPlane->handle);
		buffer.WriteInteger(CLS_GEPLANE);
		objsN++;
	}
	/*for(CDisplayView *pView=View.GetFirst();pView;pView=View.GetNext())
	{
		buffer.WriteInteger(pView->handle);
		buffer.WriteInteger(CLS_DISPLAYVIEW);
		objsN++;
	}*/
	CLDSNode *pNode=NULL;
	for(pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		buffer.WriteInteger(pNode->handle);
		buffer.WriteInteger(CLS_NODE);
		objsN++;
	}
	CLDSPart *pPart=NULL;
	//���ڲ�������ת��Ϊ��ͨ�ְ�ʱ���ܻ����������˨�������ڱ���֮ǰ�Ƚ���ͳһת�� wht 16-03-02
	CHashList<CLDSPlate> hashPlateByPeerPlateH;	
	for(CLDSParamPlate* pParamPlate=(CLDSParamPlate*)Parts.GetFirst(CLS_PARAMPLATE);pParamPlate;pParamPlate=(CLDSParamPlate*)Parts.GetNext(CLS_PARAMPLATE))
	{
		if(pParamPlate->IsNeedConvertToPlate(doc_type,nVersion))
		{
			CLDSPlate *pPlate=hashPlateByPeerPlateH.Add(pParamPlate->handle);
			pPlate->handle=pParamPlate->handle;
			//BOOL pushed=Parts.push_stack();	//ConvertToPlate�����������˨ʱ�Ѵ�����ѹջ��ջ���� wjh-2016.3.26
			pParamPlate->ConvertToPlate(pPlate);
			//Parts.pop_stack(pushed);
		}
	}
	for(pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		buffer.WriteInteger(pPart->handle);
		if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&hashPlateByPeerPlateH.GetValue(pPart->handle)!=NULL)
			buffer.WriteInteger(CLS_PLATE);
		else
			buffer.WriteInteger(pPart->GetClassTypeId());
		objsN++;
	}
	buffer.RecallPosition();
	buffer.WriteInteger(objsN);
	buffer.RecallPosition();
	int increcount=xTmpOldVersion.hashTmpDatumPoints.Count;
	stack.Initialize(&buffer,Point.GetNodeNum()+increcount);
	buffer.WriteInteger(Point.GetNodeNum()+increcount);
	for(pPoint=Point.GetFirst();pPoint;pPoint=Point.GetNext())
	{
		buffer.WriteInteger(pPoint->handle);
		pPoint->ToBuffer(buffer,nVersion,doc_type);
		stack.Increment();
	}
	if(increcount>0)
	{
		COldVerDataFile::SIMPLE_DATUMPOINT* pSimpleDatumPoint;
		CLDSPoint point;
		for(pSimpleDatumPoint=xTmpOldVersion.hashTmpDatumPoints.GetFirst();pSimpleDatumPoint;
			pSimpleDatumPoint=xTmpOldVersion.hashTmpDatumPoints.GetNext())
		{
			buffer.WriteInteger(pSimpleDatumPoint->hNewTmpDatumPoint);
			if(pSimpleDatumPoint->ciHidType==1)
			{
				point.datum_pos_style=8;
				point.des_para.hNode=pSimpleDatumPoint->hNodeOrRod;
			}
			else if(pSimpleDatumPoint->ciHidType=='S'||pSimpleDatumPoint->ciHidType=='E')
			{
				point.datum_pos_style=1;
				point.des_para.RODEND.hRod=pSimpleDatumPoint->hNodeOrRod;
				point.des_para.RODEND.bIncOddEffect=false;
				point.des_para.RODEND.SetWingOffset(0,4);
				point.des_para.RODEND.len_offset_dist=0;
				point.des_para.RODEND.direction=pSimpleDatumPoint->ciHidType=='S'?0:1;
			}
			point.ToBuffer(buffer,nVersion,doc_type);
			stack.Increment();
		}
	}
	if(!stack.VerifyAndRestore())
#ifdef   AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of datum point is wrong!");
#else 
		CLDSObject::Log2File()->Log("��׼���¼�����ִ���!");
#endif	
	stack.Initialize(&buffer,Line.GetNodeNum());
	buffer.WriteInteger(Line.GetNodeNum());
	for(pLine=Line.GetFirst();pLine;pLine=Line.GetNext())
	{
		buffer.WriteInteger(pLine->handle);
		pLine->ToBuffer(buffer,nVersion,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of datum  line is wrong!");
#else 
		CLDSObject::Log2File()->Log("��׼�߼�¼�����ִ���!");
#endif
	stack.Initialize(&buffer,Plane.GetNodeNum());
	buffer.WriteInteger(Plane.GetNodeNum());
	for(pPlane=Plane.GetFirst();pPlane;pPlane=Plane.GetNext())
	{
		buffer.WriteInteger(pPlane->handle);
		COldVerLdsObjLife oldver(&xTmpOldVersion,pPlane,nVersion,doc_type);
		pPlane->ToBuffer(buffer,nVersion,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of datum plane is wrong!");
#else 
		CLDSObject::Log2File()->Log("��׼���¼�����ִ���!");
#endif
	stack.Initialize(&buffer,Module.GetNodeNum());
	buffer.WriteInteger(Module.GetNodeNum());
	for(CLDSModule *pModule=Module.GetFirst();pModule;pModule=Module.GetNext())
	{
		buffer.WriteInteger(pModule->handle);
		pModule->ToBuffer(buffer,nVersion,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of module is wrong!");
#else 
		CLDSObject::Log2File()->Log("ģ�ͼ�¼������!");
#endif
	buffer.WriteInteger(m_hActiveModule);
	for(i=0;i<4;i++)
		buffer.WriteWord(m_arrActiveQuadLegNo[i]);
	if( (nVersion==0&&doc_type!=3)||
		(doc_type==1&&nVersion>=5020000)||	//TMA V5.2.0.0
		(doc_type==2&&nVersion>=2010000)||	//LMA V2.1.0.0
		(doc_type==4&&nVersion>=1030804)||	//LDS V1.3.8.4
		(doc_type==5&&nVersion>=1030804))	//TDA V1.3.8.4
	{
		buffer.WriteBooleanThin(this->m_bUserSpecifyNamedHeightZeroZ);
		buffer.WriteDouble(this->m_fNamedHeightZeroZ);
	}
#ifdef __COMMON_PART_INC_
	if(  nVersion==0||
		(doc_type==1&&nVersion>=4020001)||	//TMA V4.2.0.1
		(doc_type==2&&nVersion>=1020001)||	//LMA V1.2.0.1
		//(doc_type==3&&nVersion>=1080001)||	//TSA V1.8.0.1
		(doc_type==4&&nVersion>=1020001)||	//LDS V1.2.0.1
		(doc_type==5&&nVersion>=1020001))	//TDA V1.2.0.1
	{
	BUFFERPOP stack(&buffer,ArcLift.GetNodeNum());
	buffer.WriteInteger(ArcLift.GetNodeNum());
	for(CLDSArcLift* pLift=ArcLift.GetFirst();pLift;pLift=ArcLift.GetNext())
	{
		buffer.WriteInteger(pLift->handle);
		pLift->ToBuffer(buffer,nVersion,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of arc-lift is wrong!");
#else 
		CLDSObject::Log2File()->Log("Բ��Ԥ����¼������!");
#endif
	}
#endif
	//��ͼ����
	buffer.LogPosition();
	int nHits=0;
	buffer.WriteInteger(View.GetNodeNum());
	for(CDisplayView* pView=View.GetFirst();pView;pView=View.GetNext())
	{
		if(pView->m_iViewType==2)
			continue;	//��Ϲ���������ͼ��������ͼ�����棬��ʾʱ��ʱ����
		buffer.WriteInteger(pView->handle);
		pView->ToBuffer(buffer,nVersion,doc_type);
		nHits++;
	}
	buffer.RecallPosition();
	buffer.WriteInteger(nHits);
	buffer.RecallPosition();
	if(CLDSObject::_console)
		buffer.WriteInteger(CLDSObject::_console->GetActiveView()->handle);
	else
		buffer.WriteInteger(0);

	// ------------ 1. vvvvvvvv �ڵ� --------------
    stack.Initialize(&buffer,Node.GetNodeNum());
	buffer.WriteInteger(Node.GetNodeNum());
	for(pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		buffer.WriteInteger(pNode->handle);
		pNode->UnifiedCfgword();
		pNode->ToBuffer(buffer,nVersion,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of node is wrong!");
#else 
		CLDSObject::Log2File()->Log("�ڵ��¼������!");
#endif
	if( version==0||
		(doc_type==1&&nVersion>=5010901)||	//TMA(V5.1.9.1)
		(doc_type==2&&nVersion>=2000901)||	//LMA(V2.0.9.1)
		(doc_type==3&&nVersion>=2000200)||	//TSA(V2.0.2.0)
		(doc_type==4&&nVersion>=1020200)||	//LDS(V1.0.2.0)
		(doc_type==5&&nVersion>=1030804))	//TDA(V1.3.8.4)
	{	//�洢ȫ�ֵ�������Ʋ���
		int level=buffer.PushPositionStack();
		buffer.ClearLevelCount(level);//int count=0;
		buffer.WriteInteger(hashParams.GetNodeNum());
		for(DESIGN_PARAM_ITEM* pItem=hashParams.GetFirst();pItem;pItem=hashParams.GetNext())
		{
			buffer.WriteDword(hashParams.GetCursorKey());	//�ò����ı�ʶ��ֵ���򲻳���4�������ַ�����
			buffer.WriteString(pItem->sLabel);	//����������ʾ�Ķ����Ʊ�ǩ
			buffer.WriteString(pItem->sNotes);	//�����жԸ������ԵĽ���������
			buffer.WriteByte(pItem->dataType);	//0.double��ֵ 1.��� 2. ����
			buffer.Write(&pItem->value.fValue,8);
			buffer.IncrementLevelCount(level);//count++;
		}
		buffer.PopPositionStack();
		buffer.WriteInteger(buffer.LevelCount(level));
		buffer.RestoreNowPosition();
		//�洢�����ܿ���Ϣ
		level=buffer.PushPositionStack();
		buffer.WriteInteger(hashSlaveNodes.GetNodeNum());
		buffer.ClearLevelCount(level);//int count=0;
		for(SLAVE_NODE* pSlaveNode=hashSlaveNodes.GetFirst();pSlaveNode;pSlaveNode=hashSlaveNodes.GetNext())
		{
			buffer.WriteInteger(pSlaveNode->hRelaNode);//hashSlaveNodes.GetCursorKey());
			pSlaveNode->ToBuffer(buffer,nVersion,doc_type);
			buffer.IncrementLevelCount(level);//count++;
		}
		buffer.PopPositionStack();
		buffer.WriteInteger(buffer.LevelCount(level));
		buffer.RestoreNowPosition();
	}
	// ------------ 2. vvvvvvvv ���� --------------
	stack.Initialize(&buffer,Parts.GetNodeNum());
	buffer.WriteInteger(Parts.GetNodeNum());
	i=0;
	for(pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		i++;
		buffer.WriteInteger(pPart->handle);
		pPart->UnifiedCfgword();
		CLDSPlate *pPlate=pPart->GetClassTypeId()==CLS_PARAMPLATE?hashPlateByPeerPlateH.GetValue(pPart->handle):NULL;
		if(pPlate)
			pPlate->ToBuffer(buffer,nVersion,doc_type);
		else
			pPart->ToBuffer(buffer,nVersion,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of part is wrong!");
#else 
		CLDSObject::Log2File()->Log("������¼������!");
#endif
#ifndef __TSA_FILE_
	stack.Initialize(&buffer,SuppPart.GetNodeNum());
	buffer.WriteInteger(SuppPart.GetNodeNum());
	for(CSupplementPart *pSuppPart=SuppPart.GetFirst();pSuppPart;pSuppPart=SuppPart.GetNext())
	{
		buffer.WriteInteger(pSuppPart->handle);
		pSuppPart->ToBuffer(buffer,nVersion,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of supplement part is wrong!");
#else 
		CLDSObject::Log2File()->Log("����������¼������!");
#endif
	buffer.LogPosition();
	nHits=0;
	buffer.WriteInteger(PartGroup.GetNodeNum());
	for(CLDSPartGroup *pPartGroup=PartGroup.GetFirst();pPartGroup;pPartGroup=PartGroup.GetNext())
	{
		if(pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_FOLDER)
			continue;	//�����½���Ϲ�����ʱʹ�� ���洢
		pPartGroup->ToBuffer(buffer,nVersion,doc_type);
		nHits++;
	}
	buffer.RecallPosition();
	buffer.WriteInteger(nHits);
	buffer.RecallPosition();
#endif
#if defined(__TSA_FILE_)||defined(__LDS_FILE_)
	if(doc_type==3||doc_type==4)
	{
	stack.Initialize(&buffer,Elems.GetNodeNum());
    buffer.WriteInteger(Elems.GetNodeNum());
	for(CElemInfo *pElemInfo=Elems.GetFirst();pElemInfo;pElemInfo=Elems.GetNext())
	{
		buffer.WriteInteger(pElemInfo->m_hStartNode);
		buffer.WriteInteger(pElemInfo->m_hEndNode);
		pElemInfo->ToBuffer(buffer,nVersion,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of element is wrong!");
#else 
		CLDSObject::Log2File()->Log("��Ԫ��Ϣ��¼������!");
#endif
	if(version>0&&((doc_type==3&&nVersion<2000101)||(doc_type==4&&nVersion<1020101)))
	{	//��ǰ���ݱ��棬����ʧ���ؼ��߷�����Ϣ wjh-2014.5.15
		buffer.WriteInteger(1);
		buffer.WriteInteger(1);			//pWireLayout->iNo
#ifdef AFX_TARG_ENU_ENGLISH
		buffer.WriteString("General Plan");	//pWireLayout->name
#else
		buffer.WriteString("���淽��");	//pWireLayout->name
#endif
		buffer.WriteInteger(0);			//pWireLayout->hashWireLoadModel.GetNodeNum()
		buffer.WriteInteger(0);			//pWireLayout->groupWorkStatus.hashWorkStatus.GetNodeNum());
		buffer.WriteInteger(0);			//pWireLayout->groupWorkStatus.hashNodeRoom.GetNodeNum());
	}
	/* ���߷�����Ϣ��ת��CWireLoadBuffer�У������йغ��ع��߷�����д�����������CWireLayoutsEvolvement::ToBuffer�С�wjh-2014.4.28
	buffer.WriteInteger(evolve.WireLayouts.GetNodeNum());
	for(CWireLayout *pWireLayout=evolve.WireLayouts.GetFirst();pWireLayout;pWireLayout=evolve.WireLayouts.GetNext())
	{
		buffer.WriteInteger(pWireLayout->iNo);
		buffer.WriteString(pWireLayout->name);
		//ԭʼ�����������
		buffer.WriteInteger(pWireLayout->hashWireLoadModel.GetNodeNum());
		for(CWireLoadModel *pWireModel=pWireLayout->hashWireLoadModel.GetFirst();pWireModel;pWireModel=pWireLayout->hashWireLoadModel.GetNext())
		{
			buffer.WriteInteger(pWireModel->iNo);
			pWireModel->ToBuffer(buffer);
		}
		//���������Ϣ
		buffer.WriteInteger(pWireLayout->groupWorkStatus.hashWorkStatus.GetNodeNum());
		for(RAWWORKSTATUS_STRU* pRawStatus=pWireLayout->groupWorkStatus.hashWorkStatus.GetFirst();pRawStatus;pRawStatus=pWireLayout->groupWorkStatus.hashWorkStatus.GetNext())
		{
			buffer.WriteInteger(pWireLayout->groupWorkStatus.hashWorkStatus.GetCursorKey());
			buffer.WriteString(pRawStatus->name);
			buffer.WriteByte(pRawStatus->m_cHoriEarthQuakeAxis);
			buffer.WriteString(pRawStatus->vibration_mode_str);
			buffer.WriteString(pRawStatus->lim_str);	//�¼�
			buffer.WriteInteger(pRawStatus->m_hWorkStatus);			//��Ӧ��CWorkStatus�ľ��ֵ
			buffer.WriteByte(pRawStatus->m_cMainLoadType);			//0:���1:����2:����3:����4:��װ5:�����ȸ���6:ˮƽ����7:�������8:��ˮƽ����9:���������10:����
			buffer.WriteBooleanThin(pRawStatus->m_bCheckWorkCond);	//���㹤��
			buffer.WriteBooleanThin(pRawStatus->m_bMinVertSpace);	//��С��ֱ���๤��
			buffer.WriteDouble(pRawStatus->ice_thick);				//�����������(mm)
			buffer.WriteInteger(pRawStatus->WindLoad.azimuth);		//�����,�����(��)
			buffer.WriteDouble(pRawStatus->WindLoad.velocity);		//�����,����(m/s)
		}
		buffer.WriteInteger(pWireLayout->groupWorkStatus.hashNodeRoom.GetNodeNum());
		for(LOADNODEROOM_STRU* pRoom=pWireLayout->groupWorkStatus.hashNodeRoom.GetFirst();pRoom;pRoom=pWireLayout->groupWorkStatus.hashNodeRoom.GetNext())
		{
			long *pWorkEnvKey;
			buffer.WriteInteger(pWireLayout->groupWorkStatus.hashNodeRoom.GetCursorKey());
			buffer.WriteInteger(pRoom->m_iRawWireLoadNo);
			buffer.WriteString(pRoom->helpString);
			//д����λ��Ӧ��Ĭ��ԭʼ������ʶ
			buffer.WriteInteger(pRoom->hashWorkEnv.GetNodeNum());
			for(pWorkEnvKey=pRoom->hashWorkEnv.GetFirst();pWorkEnvKey;pWorkEnvKey=pRoom->hashWorkEnv.GetNext())
			{
				buffer.WriteInteger(pRoom->hashWorkEnv.GetCursorKey());	//ԭʼ������ʶ��
				buffer.WriteInteger(*pWorkEnvKey);						//��Ӧԭʼ�����µ�ԭʼ����������ʶ
			}
			//д�뵱ǰ���λ��Ӧ�ĵ����Ϣ
			buffer.WriteInteger(pRoom->hashNode.GetNodeNum());
			for(WIRENODE_STRU *pRawNode=pRoom->hashNode.GetFirst();pRawNode;pRawNode=pRoom->hashNode.GetNext())
			{
				buffer.WriteInteger(pRoom->hashNode.GetCursorKey());
				buffer.WriteDouble(pRawNode->fLoadAllocCoefX);
				buffer.WriteDouble(pRawNode->fLoadAllocCoefY);
				buffer.WriteDouble(pRawNode->fLoadAllocCoefZ);
				buffer.WriteInteger(pRawNode->hNode);
				buffer.WriteInteger(pRawNode->hashWorkEnv.GetNodeNum());
				for(pWorkEnvKey=pRawNode->hashWorkEnv.GetFirst();pWorkEnvKey;pWorkEnvKey=pRawNode->hashWorkEnv.GetNext())
				{
					buffer.WriteInteger(pRawNode->hashWorkEnv.GetCursorKey());	//ԭʼ������ʶ��
					buffer.WriteInteger(*pWorkEnvKey);							//��Ӧԭʼ�����µ�ԭʼ����������ʶ
				}
			}
		}
	}
	*/
	//����״��
	buffer.LogPosition();
	nHits=0;
	buffer.WriteInteger(WorkStatus.GetNodeNum());
	for(CWorkStatus *pStatus=WorkStatus.GetFirst();pStatus;pStatus=WorkStatus.GetNext())
	{
		buffer.WriteInteger(pStatus->handle);
		pStatus->ToBuffer(buffer,nVersion,doc_type);
		nHits++;
	}
	buffer.WriteInteger(m_hActiveStatus);
	buffer.RecallPosition();
	buffer.WriteInteger(nHits);
	buffer.RecallPosition();

	//���ӷ����
	buffer.LogPosition();
	nHits=0;
	buffer.WriteInteger(AttachWindLoad.GetNodeNum());
	for(CAttachWindLoad *pAttachWindLoad=AttachWindLoad.GetFirst();pAttachWindLoad;pAttachWindLoad=AttachWindLoad.GetNext())
	{
		buffer.WriteInteger(pAttachWindLoad->handle);
		pAttachWindLoad->ToBuffer(buffer,nVersion,doc_type);
		nHits++;
	}
	buffer.RecallPosition();
	buffer.WriteInteger(nHits);
	buffer.RecallPosition();
	//��ѹ����Ϣ
	buffer.LogPosition();
	long pos_start=buffer.GetCursorPosition();
	nHits=0;
	buffer.WriteInteger(WindLoadSeg.GetNodeNum());
	for(CWindSegment *pWindSeg=WindLoadSeg.GetFirst();pWindSeg;pWindSeg=WindLoadSeg.GetNext())
	{
		pWindSeg->ToBuffer(buffer,nVersion,doc_type);
		nHits++;
	}
	long pos=buffer.GetCursorPosition();
	buffer.SeekPosition(pos_start);
	buffer.WriteInteger(nHits);
	buffer.SeekPosition(pos);
	//�ֹܷ��������Ϣ
	stack.Initialize(&buffer,hashConnectFl.GetNodeNum());
	buffer.WriteInteger(hashConnectFl.GetNodeNum());
	for(CConnectFL *pFL=hashConnectFl.GetFirst();pFL;pFL=hashConnectFl.GetNext())
	{
		buffer.WriteDouble(pFL->m_fThickTube);			//���ܱں�(mm)
		buffer.WriteDouble(pFL->m_fInnerRadius);			//��׼�����ڻ��뾶(mm)
		buffer.WriteDouble(pFL->m_fCoupleInnerRadius);	//��Է����ڻ��뾶(mm)
		buffer.WriteString(pFL->bolt_grade);
		buffer.WriteInteger(pFL->bolt_d);			//��˨ֱ�������
		buffer.WriteInteger(pFL->bolt_n);			//��˨���е���˨����
		buffer.WriteDouble(pFL->N);	//N ������(N)
		buffer.WriteDouble(pFL->M);	//M���(N*m)
		buffer.WriteDouble(pFL->m_fBoltRadius);	//��˨Բ�뾶(mm)
		buffer.WriteInteger(pFL->m_nThickFL);		//�����̺��(mm)
		buffer.WriteInteger(pFL->m_nWidthFL);		//�������δ���(mm)
		buffer.WriteInteger(pFL->m_nThickRib);		//�߰���(mm),���Ϊ0��ʾ�޼Ӿ���
		buffer.WriteInteger(pFL->m_nHeightRib);		//�߰�߶�(mm)
		buffer.WriteInteger(pFL->m_nRibS);			//�߰庸���нǸ�(��)��(mm)
		buffer.WriteInteger(pFL->m_nCoupleRibS);	//����߰庸���нǸ�(��)��(mm)
		buffer.WriteInteger(pFL->m_nCoupleThickFL);	//�����̺��(mm)
		buffer.WriteInteger(pFL->m_nCoupleHeightRib);//����߰�߶�(mm)
		buffer.WriteByte(pFL->cMaterial);
		buffer.WriteString(pFL->m_sPartNo);     //�������
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of members Flange of the tower design is wrong!");
#else 
		CLDSObject::Log2File()->Log("�ֹܷ�����ƹ��ܼ�¼������!");
#endif
	buffer.WriteByte(EarthQuake.grade);	//�����Ҷ�6,7,8,9
	buffer.WriteDouble(EarthQuake.alfa_max);//ˮƽ����Ӱ��ϵ�����ֵ
	buffer.WriteByte(EarthQuake.cLandType);	//�������1,2,3,4
	buffer.WriteByte(EarthQuake.cDesignGroup);	//��Ƶ������1,2,3
	buffer.WriteDouble(EarthQuake.Tg);			//��������,s
	buffer.WriteDouble(EarthQuake.zeta);		//����ϵ����=0.02s
	buffer.WriteDouble(EarthQuake.gamaRE);		//�������������ϵ��,һ���Խ��ȡ0.85��������ȡ0.8
	buffer.WriteInteger(EarthQuake.nFreqsRank);
	//��ͷ��Ϣ
	if((doc_type==3&&(nVersion==0||nVersion>=2000200))||(doc_type==4&&(nVersion==0||nVersion>=1020200)))
	{
		BUFFERPOP stack(&buffer,hashAngleJoint.GetNodeNum());
		buffer.WriteInteger(hashAngleJoint.GetNodeNum());
		for(ANGLE_JOINT* pJoint=hashAngleJoint.GetFirst();pJoint;pJoint=hashAngleJoint.GetNext())
		{
			buffer.WriteInteger(pJoint->Id());
			pJoint->ToBuffer(buffer);
			stack.Increment();
		}
		if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
			CLDSObject::Log2File()->Log("The number record of joint information is wrong!");
#else 
			CLDSObject::Log2File()->Log("��ͷ��Ϣ��¼������!");
#endif
	}
	}
#endif
#ifdef __COMMON_PART_INC_
	if( nVersion==0||	//����֮ǰ������趨��__COMPLEX_PART_INC_��ʹT/MA�޷���¼Ӱ��������ʰ汾�Ÿ�Ϊ5.0.0.1 wjh-2015.3.9
		(doc_type==1&&nVersion>=5000003)||//version>=4020003)||
		(doc_type==2&&nVersion>=1020003)||
		(doc_type==3&&nVersion>=1080103)||
		(doc_type==4&&nVersion>=1020003)||
		(doc_type==5&&nVersion>=1020003))
	{	//��ȡ�������������� wht 12-09-10
		BUFFERPOP stack(&buffer,Block.GetNodeNum());
		buffer.WriteInteger(Block.GetNodeNum());
		for (CBlockModel *pBlock=Block.GetFirst();pBlock;pBlock=Block.GetNext())
		{
			buffer.WriteInteger(pBlock->handle);
			pBlock->ToBuffer(buffer,nVersion,doc_type);
			stack.Increment();
		}
		if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
			CLDSObject::Log2File()->Log("The number record of block is wrong!");
#else 
			CLDSObject::Log2File()->Log("����ģ�ͼ�¼������!");
#endif
		stack.Initialize(&buffer,BlockRef.GetNodeNum());
		buffer.WriteInteger(BlockRef.GetNodeNum());
		for (CBlockReference *pBlockRef=BlockRef.GetFirst();pBlockRef;pBlockRef=BlockRef.GetNext())
		{
			buffer.WriteInteger(pBlockRef->handle);
			pBlockRef->ToBuffer(buffer,nVersion,doc_type);
			stack.Increment();
		}
		if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of blockRef is wrong!");
#else 
		CLDSObject::Log2File()->Log("�������ü�¼������!");
#endif

	}
#endif
	//�����������ϵ	wht 13-01-06
	stack.Initialize(&buffer,Acs.GetNodeNum());
	buffer.WriteInteger(Acs.GetNodeNum());
	for (CParaCS *pCS=Acs.GetFirst();pCS;pCS=Acs.GetNext())
	{
		buffer.WriteInteger(pCS->id);
		pCS->ToBuffer(buffer,nVersion,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of CS is wrong!");
#else 
		CLDSObject::Log2File()->Log("����ϵ��¼������!");
#endif
	if( nVersion==0||
		(doc_type==1&&nVersion>=5020000)||	//TMA(V5.2.0.0)
		(doc_type==2&&nVersion>=2010000)||	//LMA(V2.1.0.0)
		(doc_type==3&&nVersion>=2010303)||	//TSA(V2.1.3.3)
		(doc_type==4&&nVersion>=1030803)||	//LDS(V1.3.8.3)
		(doc_type==5&&nVersion>=1030803))	//TDA(V1.3.8.3)
	{	//����ģ������������������ wht 18-08-06
		//������Ʋ���
		buffer.WriteByte(m_xFoundation.biBasePlateType);	//0.������ƽ�� 1.�����м� 2.������ƽ��
		buffer.WriteWord(m_xFoundation.wiBasePlateThick);	//������
		buffer.WriteWord(m_xFoundation.wiBasePlateWidth);	//������
		buffer.WriteWord(m_xFoundation.wiBasePlateHoleD);	//����׾�
		buffer.WriteByte(m_xFoundation.biLayoutType);		//�ؽ���˨�������ͣ�0.4��˨ 1.8��˨
		buffer.WriteWord(m_xFoundation.wiBoltD);			//�ؽ���˨ֱ��
		buffer.WriteWord(m_xFoundation.wiBoltSapce);		//��˨���
		buffer.WriteWord(m_xFoundation.S2)			;		//��˨���S2(���ڰ˵ؽ���˨��
		buffer.WriteWord(m_xFoundation.wiFoundationWidth);	//�������
		if ((doc_type == 4 || doc_type == 5) && (nVersion == 0 || nVersion >= 1030901))
		{	//�������Ű�ѥ��/�߰���Ʋ���
			buffer.WriteByte(m_xFoundation.cnMainAngleBoltNum);
			buffer.WriteByte(m_xFoundation.cnMaxSingularRowBolts);
			buffer.WriteByte(m_xFoundation.cnRows);	//����������˨������������
			buffer.WriteWord(m_xFoundation.wiShoeThick);
			buffer.WriteWord(m_xFoundation.wiRibThick);
			buffer.WriteWord(m_xFoundation.wiRibMargin);				//�װ��ȡ�ѥ���ȣ��߰��ȡ��߾�
			buffer.WriteWord(m_xFoundation.wRibWidth);
			buffer.WriteWord(m_xFoundation.wRibOuterHeight);
			buffer.WriteWord(m_xFoundation.wRibInnerHeight);		//�߰���(�װ庸�ӱ�),�߶ȣ�����࣬�ڲࣩ
		}
		buffer.WriteInteger(m_xFoundation.hashPostRodFlags.GetNodeNum());	//���ڴ洢���������ľ༰���ȸ������Զ������趨
		for(POSTRODFLAG* pFlag=m_xFoundation.hashPostRodFlags.GetFirst();pFlag;pFlag=m_xFoundation.hashPostRodFlags.GetNext())
		{
			buffer.WriteInteger(m_xFoundation.hashPostRodFlags.GetCursorKey());
			buffer.WriteByte(pFlag->m_cFlag);
		}
		//�洢������Ϣ
		stack.Initialize(&buffer,Foundations.GetNodeNum());
		buffer.WriteInteger(Foundations.GetNodeNum());
		for (CLDSFoundation *pFoundation=Foundations.GetFirst();pFoundation;pFoundation=Foundations.GetNext())
		{
			buffer.WriteInteger(pFoundation->handle);
			pFoundation->ToBuffer(buffer,nVersion,doc_type);
			stack.Increment();
		}
		if(!stack.VerifyAndRestore())
	#ifdef  AFX_TARG_ENU_ENGLISH
			CLDSObject::Log2File()->Log("The number record of CS is wrong!");
	#else 
			CLDSObject::Log2File()->Log("������¼������!");
	#endif
	}
	//�˴���������ô˺���,������Undo��������������,�ٱ����ļ�,
	//��Redo��ȥ��������������еľ���ͻ����ֶ�����Ӷ������ظ� wjh-2014.12.11
	//GetMinAvailableHandle();
}

void CTower::ToXmlFile(FILE* fp,DWORD schema,DWORD cursor_dog_no,char *file_version)
{
	//TODO:����,��ɾ������ wjh-2017.11.24
}
void CTower::FromAttachBuffer(CBuffer &buffer)
{
	do
	{
		DWORD buf_key=0,buf_len=0;
		buffer.ReadInteger(&buf_key);
		buffer.ReadInteger(&buf_len);
		CAttachBuffer* pBuff=AttachBuffer.GetValue(buf_key);
		if(pBuff)
		{
			pBuff->ClearContents();
			pBuff->SetBlockSize(buf_len);
			char *buf=new char[buf_len];
			buffer.Read(buf,buf_len);
			pBuff->Write(buf,buf_len);
			delete []buf;
		}
		else
			break;
	}while(buffer.GetCursorPosition()<buffer.GetLength());
}
void CTower::ToAttachBuffer(CBuffer &buffer)
{
	for(CAttachBuffer* pBuff=AttachBuffer.GetFirst();pBuff;pBuff=AttachBuffer.GetNext())
	{
		if( pBuff->Key()==TOWER_MODEL||pBuff->Key()==CLIPPER)
			continue;
		buffer.WriteInteger(pBuff->Key());
		buffer.WriteInteger(pBuff->GetLength());
		buffer.Write(pBuff->GetBufferPtr(),pBuff->GetLength());
	}
}
bool CTower::IsHasRsltBuffer(const char* itemKeyStr)
{
	CAttachBuffer *pRsltBuffer=AttachBuffer.GetValue(RSLT_REPORT);
	int item_n=0;
	if(pRsltBuffer==NULL)
		return false;
	pRsltBuffer->SeekToBegin();
	pRsltBuffer->ReadInteger(&item_n);
	CXhChar16 rsltItemKeyStr;
	for(int i=0;i<item_n;i++)
	{
		if(pRsltBuffer->ReadString(rsltItemKeyStr,17)>0)
		{
			if(rsltItemKeyStr.EqualNoCase(itemKeyStr))
				return true;
			int length=0;
			pRsltBuffer->ReadInteger(&length);
			pRsltBuffer->Offset(length);//����������
		}
		else
			break;
	}
	return false;
}
bool CTower::RemoveRsltBuffer(const char* itemKeyStr)
{
	CAttachBuffer *pRsltBuffer=AttachBuffer.GetValue(RSLT_REPORT);
	int item_n=0;
	if(pRsltBuffer==NULL)
		return false;
	pRsltBuffer->SeekToBegin();
	pRsltBuffer->ReadInteger(&item_n);
	CBuffer bufferTemp(pRsltBuffer->GetLength());

	CXhChar16 rsltItemKeyStr;
	int itemDeleted=0;
	for(int i=0;i<item_n;i++)
	{
		if(pRsltBuffer->ReadString(rsltItemKeyStr,17)>0)
		{
			int length=0;
			pRsltBuffer->ReadInteger(&length);
			if(!rsltItemKeyStr.EqualNoCase(itemKeyStr))
			{	//���濽������ʱ������
				bufferTemp.WriteString(rsltItemKeyStr);
				bufferTemp.WriteInteger(length);
				bufferTemp.Write(pRsltBuffer->GetCursorBuffer(),length);
			}
			else
				itemDeleted++;
			//����������
			pRsltBuffer->Offset(length);
		}
		else
			break;
	}
	if(itemDeleted>0)
	{
		pRsltBuffer->ClearContents();
		pRsltBuffer->WriteInteger(item_n-itemDeleted);
		pRsltBuffer->Write(bufferTemp.GetBufferPtr(),bufferTemp.GetLength());
		return true;
	}
	else
		return false;
}
bool CTower::ReadRsltBuffer(const char* itemKeyStr,CBuffer &buffer)
{
	CAttachBuffer *pRsltBuffer=AttachBuffer.GetValue(RSLT_REPORT);
	int item_n=0;
	if(pRsltBuffer==NULL)
		return false;
	pRsltBuffer->SeekToBegin();
	pRsltBuffer->ReadInteger(&item_n);
	CBuffer bufferTemp(pRsltBuffer->GetLength());
	CXhChar16 rsltItemKeyStr;
	buffer.ClearContents();
	for(int i=0;i<item_n;i++)
	{
		if(pRsltBuffer->ReadString(rsltItemKeyStr,17)>0)
		{
			int length=0;
			pRsltBuffer->ReadInteger(&length);
			if(rsltItemKeyStr.EqualNoCase(itemKeyStr))
			{
				buffer.Write(pRsltBuffer->GetCursorBuffer(),length);
				return true;
			}
			pRsltBuffer->Offset(length);//����������
		}
		else
			break;
	}
	return false;
}
bool CTower::WriteRsltBuffer(const char* itemKeyStr,CBuffer &buffer)
{
	CAttachBuffer *pRsltBuffer=AttachBuffer.GetValue(RSLT_REPORT);
	int item_n=0;
	if(pRsltBuffer==NULL)
		return false;
	pRsltBuffer->SeekToBegin();
	pRsltBuffer->ReadInteger(&item_n);
	CBuffer bufferTemp(pRsltBuffer->GetLength());
	CXhChar16 rsltItemKeyStr;
	bool overwrited=false;
	for(int i=0;i<item_n;i++)
	{
		if(pRsltBuffer->ReadString(rsltItemKeyStr,17)>0)
		{
			int length=0;
			pRsltBuffer->ReadInteger(&length);
			if(!overwrited&&rsltItemKeyStr.EqualNoCase(itemKeyStr))
			{
				overwrited=true;
				bufferTemp.WriteString(rsltItemKeyStr);
				bufferTemp.WriteInteger(buffer.GetLength());
				bufferTemp.Write(buffer.GetBufferPtr(),buffer.GetLength());
			}
			else
			{	//���濽������ʱ������
				bufferTemp.WriteString(rsltItemKeyStr);
				bufferTemp.WriteInteger(length);
				bufferTemp.Write(pRsltBuffer->GetCursorBuffer(),length);
			}
			pRsltBuffer->Offset(length);//����������
		}
		else
			break;
	}
	if(!overwrited)	//������������
	{
		item_n++;
		bufferTemp.WriteString(itemKeyStr);
		bufferTemp.WriteInteger(buffer.GetLength());
		bufferTemp.Write(buffer.GetBufferPtr(),buffer.GetLength());
	}
	pRsltBuffer->ClearContents();
	pRsltBuffer->WriteInteger(item_n);
	pRsltBuffer->Write(bufferTemp.GetBufferPtr(),bufferTemp.GetLength());
	return true;
}

void CTower::WriteAdditionalBuffer(CBuffer &buffer)
{
	//���ϻ��������ļ�������
	long nCfgBufferLen=AttachBuffer[BOM_CONFIG].GetLength();//m_bufferSumTaMatCfg.GetLength();
	buffer.WriteInteger(nCfgBufferLen);
	if(nCfgBufferLen>0)
		buffer.Write(AttachBuffer[BOM_CONFIG].GetBufferPtr(),nCfgBufferLen);
	//��ͼ�ű�������,��Ҫ�洢
	long nScriptLen=AttachBuffer[SMART_DRAW].GetLength();
	buffer.WriteInteger(nScriptLen);
	if(nScriptLen>0)
		buffer.Write(AttachBuffer[SMART_DRAW].GetBufferPtr(),nScriptLen);
}

void CTower::ReadAdditionalBuffer(CBuffer &buffer)
{
	buffer.SeekToBegin();
	//���ϻ��������ļ�������
	long nCfgBufferLen=0;
	buffer.ReadInteger(&nCfgBufferLen);
	if(nCfgBufferLen>0)
	{
		BYTE *pBuffer=new BYTE[nCfgBufferLen];
		buffer.Read(pBuffer,nCfgBufferLen);
		AttachBuffer[BOM_CONFIG].ClearBuffer();
		AttachBuffer[BOM_CONFIG].Write(pBuffer,nCfgBufferLen);
		delete []pBuffer;
	}
	long nScriptLen=0;
	buffer.ReadInteger(&nScriptLen);
	if(nScriptLen>0)
	{	//��ͼ�ű�������,��Ҫ��ȡ
		buffer.LogPosition();
		BYTE *pBuffer=new BYTE[nScriptLen];
		buffer.Read(pBuffer,nScriptLen);
		AttachBuffer[SMART_DRAW].ClearBuffer();
		AttachBuffer[SMART_DRAW].Write(pBuffer,nScriptLen);
		delete []pBuffer;
		buffer.RecallPosition();
		if(ParseDrawingScriptBuffer)
			ParseDrawingScriptBuffer(buffer);
		/*else if(DisplayProcess)
		{	//�޽ű���������ʱ����������λ�� wht 11-02-16
			long cur_percent=buffer.GetCursorPosition()*100/buffer_len;
			for(i=cur_percent;i<100;i++)
				DisplayProcess(i,NULL);
		}*/
	}
}


//ˢ���麸���б�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
//����ͼ������3λ�Ƚ�(ͬһ��ŵ�)������,ͳ���麸��ʱĬ�����ȼ�˳��Ϊ1>2>3>4>Q>H>Z>Y>G wht 10-08-26
static int ComparePartLayer(CLDSPart *pPart1,CLDSPart *pPart2)
{	//1>2>3>4>Q>H>Z>Y>G
	char cPos1=toupper(pPart1->layer(2));
	char cPos2=toupper(pPart2->layer(2));
	if(cPos1==cPos2)
		return 0;
	else if(cPos1=='1')
		return 1;
	else if(cPos1=='2')
	{
		if(cPos2=='1')
			return -1;
		else
			return 1;
	}
	else if(cPos1=='3')
	{
		if(cPos2=='1'||cPos2=='2')
			return -1;
		else
			return 1;
	}
	else if(cPos1=='4')
	{
		if(cPos2=='1'||cPos2=='2'||cPos2=='3')
			return -1;
		else 
			return 1;
	}
	else if(cPos1=='Q')
	{
		if(cPos2=='1'||cPos2=='2'||cPos2=='3'||cPos2=='4')
			return -1;
		else 
			return 1;
	}
	else if(cPos1=='H')
	{
		if(cPos2=='1'||cPos2=='2'||cPos2=='3'||cPos2=='4'||cPos2=='Q')
			return -1;
		else 
			return 1;
	}
	else if(cPos1=='Z')
	{
		if(cPos2=='1'||cPos2=='2'||cPos2=='3'||cPos2=='4'||cPos2=='Q'||cPos2=='H')
			return -1;
		else 
			return 1;
	}
	else if(cPos1=='Y')
	{
		if(cPos2=='1'||cPos2=='2'||cPos2=='3'||cPos2=='4'||cPos2=='Q'||cPos2=='H'||cPos2=='Z')
			return -1;
		else 
			return 1;
	}
	else if(cPos1=='G')
	{
		if(cPos2=='1'||cPos2=='2'||cPos2=='3'||cPos2=='4'||cPos2=='Q'||cPos2=='H'||cPos2=='Z'||cPos2=='Y')
			return -1;
		else 
			return 1;
	}
	else 
		return 0;
}

//����麸���ĺϷ��ԣ��Զ������麸��
BOOL CTower::CheckPartGroupValidAndCorrect(CLDSPartGroup *pPartGroup,BOOL bAutoCorrect)
{
	if(pPartGroup==NULL)
		return FALSE;
	CLDSLineTube *pDatumTube=(CLDSLineTube*)Parts.FromHandle(pPartGroup->handle,CLS_LINETUBE);
	if(pDatumTube==NULL)
		return FALSE;
	//1.���ҹ��������ͬ�������麸��
	TUBESET tubeSet,correctTubeSet;
	CLDSLineTube *pLineTube=NULL;
	for(pLineTube=(CLDSLineTube*)Parts.GetFirst(CLS_LINETUBE);pLineTube;pLineTube=(CLDSLineTube*)Parts.GetNext())
	{
		if(stricmp(pLineTube->GetPartNo(),pDatumTube->GetPartNo())!=0)
			continue;
		tubeSet.append(pLineTube);
	}
	//2.�Ƚϱ����ͬ���麸���Ƿ���ȫһ�£�����¼��һ�µ��麸�����ܾ��
	PARTSET datumPartSet,tempPartSet;
	GetPartGroupPartSet(pDatumTube->handle,datumPartSet);
	int nPartCount=datumPartSet.GetNodeNum();
	for(pLineTube=tubeSet.GetFirst();pLineTube;pLineTube=tubeSet.GetNext())
	{
		GetPartGroupPartSet(pLineTube->handle,tempPartSet);
		//2.1�麸���й���������ͬ 
		if(tempPartSet.GetNodeNum()!=nPartCount)
		{	
			correctTubeSet.append(pLineTube);
			continue;
		}
		//2.2�Ƚ��麸���й�����װλ���Ƿ���ͬ(����ȡ�����ȡ�ʼ�ն˵���(������))
		//2.2.1 �ȽϹ��������ͬ�Ĺ���λ���Ƿ���ͬ
		//2.2.2 
	}
	//3.����麸�����±��
	return FALSE;
}

void CTower::RetrieveWeldLineInfo()
{
	CLDSPart* pPart;
	//�Զ����ֹ������Ϣ
	BOOL pushed0=Parts.push_stack();
	CHashList<CPtInLineCheck> hashLineChecks;
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Intelligent identify tube intersecting information...");
#else
	DisplayProcess(0,"����ʶ��ֹ������Ϣ...");
#endif
	int i=0,n=Parts.GetNodeNum();
	for(pPart=Parts.GetFirst(CLS_LINETUBE);pPart;pPart=Parts.GetNext(CLS_LINETUBE),i++)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((i*8)/n,"Intelligent identify tube's intersecting information...");
#else
		DisplayProcess((i*8)/n,"����ʶ��ֹ������Ϣ...");
#endif
		pPart->IntelliRecogWeldLine();
	}
	//�Զ���ȡ�ְ庸����
	CHashSet<CLDSGeneralPlate*> listPlates;
	for(pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		if(pPart->IsPlate())
			listPlates.SetValue(pPart->handle,(CLDSGeneralPlate*)pPart);
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(8,"Intelligent identify the weld space information of plate's edge joint...");
#else
	DisplayProcess(8,"����ʶ��ְ��Ե���Ϻ�����Ϣ...");
#endif
	Parts.pop_stack(pushed0);
	n=listPlates.GetNodeNum();
	i=0;
	for(CLDSGeneralPlate* pPlate=listPlates.GetFirst();pPlate;pPlate=listPlates.GetNext(),i++)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess(8+(i*92)/n,"Intelligent identify the weld space information of plate's edge joint...");
#else
		DisplayProcess(8+(i*92)/n,"����ʶ��ְ��Ե���Ϻ�����Ϣ...");
#endif
		pPlate->IntelliRecogWeldLine();
	}
	DisplayProcess(100,NULL);
}

void CTower::RetrieveBriefWeldPartGroups(CHashListEx<CBriefWeldPart>& hashPartGroups,const char* segstr)
{
	CHashList<SEGI>hashSegTbl;
	CXhChar100 segIncStr=segstr;
	if(strlen(segstr)>0&&strchr(segstr,'*')==NULL)	//ָ���κŷ�Χ
	{
		if(GetSegNoHashTblBySegStr(segstr,hashSegTbl)==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("segment number cannot be empty,please confirm the segment number of combined weld parts!");
#else
			logerr.Log("�κŲ���Ϊ�գ���ȷ���麸���κţ�");
#endif
			return;
		}
	}
	CLogErrorLife log;
	CHashTable<long> grouptable;
	grouptable.CreateHashTable(Parts.GetNodeNum());
	CLDSPart *pPart=NULL;
	CBriefWeldPart *pWeldPart=NULL;
	hashPartGroups.Empty();
	int nCurPorcess=0,nPartCount=Parts.GetNodeNum();
	AddJointTubeWeldRelations(&hashSegTbl);
	for(pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		//01:��Ϻ������еĸ�(��)����;10:��Ϻ����е��ӹ���;11:��Ϻ������м伶����
		long status=0;
		//Ϊ��ֹ��κŲ������麸����Σ������´���ͳ�ƣ��˴������й��� wjh-2012.4.7
		if(hashSegTbl.GetNodeNum()>0&&!hashSegTbl.GetValue(pPart->iSeg))
			continue;	//���ڵ�ǰָ���κŷ�Χ��
		CLDSPart* pRootPart=NULL;
		if(pPart->m_hPartWeldParent>0x20)
		{
			CLdsPartListStack stack(&Parts);
			pRootPart=Parts.FromHandle(pPart->m_hPartWeldParent);
			if(pRootPart==NULL&&(hashSegTbl.GetNodeNum()==0||hashSegTbl.GetValue(pPart->iSeg)!=NULL))
			{
				long hWeldParent=pPart->m_hPartWeldParent;
				pPart->IntelliRecogWeldLine();
				if(hWeldParent!=pPart->m_hPartWeldParent)
					pRootPart=Parts.FromHandle(pPart->m_hPartWeldParent);
				if(pRootPart==NULL)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X{%s} part's welding parent part lost, may affect the statistics of the welding parts.",pPart->handle,(char*)pPart->GetPartNo());
#else
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X����{%s}�ĺ��Ӹ�������ʧ������Ӱ���麸����ȷͳ��",pPart->handle,(char*)pPart->GetPartNo());
#endif
				}
				else
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X{%s} part's welding parent part lost, but this error is corrected by software.",pPart->handle,(char*)pPart->GetPartNo());
#else
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X����{%s}�ĺ��Ӹ�������ʧ������������ʶ������Ӹ�����",pPart->handle,(char*)pPart->GetPartNo());
#endif
			}
		}
		if(pRootPart)//pPart->m_hPartWeldParent>0x20)
		{
			//�趨pPart->m_hPartWeldParent�ĸ���������(ֻ���ĸ�λ��Ϊ1)
			if(grouptable.GetValueAt(pPart->m_hPartWeldParent,status))
				status=status-status%10+1;
			else
				status=1;
			grouptable.SetValueAt(pPart->m_hPartWeldParent,status);
			//�趨pPart���ӹ�������(ֻ����ʰλ��Ϊ1)
			if(grouptable.GetValueAt(pPart->handle,status))
				status=10+status%10;
			else
				status=10;
			grouptable.SetValueAt(pPart->handle,status);
		}
		else //if(pPart-
		{
			if(grouptable.GetValueAt(pPart->handle,status))
				status=status%10;	//�趨���������ԣ���ʰλ��Ϊ0
			else
				status=0;
			grouptable.SetValueAt(pPart->handle,status);
		}
	}
	for(pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		long status=0;
		if(grouptable.GetValueAt(pPart->handle,status))
		{
			if(status!=1||(hashSegTbl.GetNodeNum()>0&&hashSegTbl.GetValue(pPart->iSeg.iSeg)==NULL))
				continue;	//���������ڵ�ǰָ���κŷ�Χ�ڣ����������з���ͳ��
			pWeldPart=hashPartGroups.Add(pPart->handle);//��Ϲ����ĸ�����
		}
	}
	//�麸�����Է�Ϊ��׼�ͷǱ�׼������
	//���麸����׼����Ϊ�ֹܣ����麸���н��������ֹ�ʼ/�ն����ӵĲ���/U��/ʮ�ֲ����߷���ʱΪ��׼�麸��
	CHashList<CLDSPart*>hashStatSonParts;
	int nPartGroupCount=hashPartGroups.GetNodeNum();
	for(pWeldPart=hashPartGroups.GetFirst();pWeldPart;pWeldPart=hashPartGroups.GetNext())
	{
		CLDSPart *pRootPart=Parts.FromHandle(pWeldPart->hRootPart);
		PARTSET partset;
		GetPartGroupPartSet(pWeldPart->hRootPart,partset);
		//��pWeldPart��Ӻ����Ӽ�
		for(CLDSPart* pSonPart=partset.GetFirst();pSonPart;pSonPart=partset.GetNext())
		{
			pWeldPart->AttachObject(WELDSONPART(pSonPart));
			//���ڼ�����趨���Ӹ�������β�ν���ѭ��������������ĺ��Ӽ� wjh-2015.8.26
			hashStatSonParts.SetValue(pSonPart->handle,pSonPart);
		}
	}
	CHashSet<CLDSParamPlate*> hashCoverPlates;
	for(pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		if(hashSegTbl.GetNodeNum()>0&&hashSegTbl.GetValue(pPart->iSeg.iSeg)==NULL)
			continue;	//���������ڵ�ǰָ���κŷ�Χ�ڣ����������з���ͳ��
		if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)pPart)->m_iParamType==TYPE_CIRCOVERPLATE)
			hashCoverPlates.SetValue(pPart->handle,(CLDSParamPlate*)pPart);
		long status=0;
		grouptable.GetValueAt(pPart->handle,status);
		if(status>10&&hashStatSonParts.GetValue(pPart->handle)==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X part{%s}is weld part��but not identified as welding group parts. Please check if there is a parent weld part deadlock situation.",pPart->handle,(char*)pPart->GetPartNo());
#else
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X����{%s}Ϊ���Ӽ������ǿ������ں��Ӹ������趨������ѭ��δ����ȷͳ��Ϊ�麸��",pPart->handle,(char*)pPart->GetPartNo());
#endif
		}
	}
	//��ӻ��������׼�ְ��ĺ��ӹ�ϵ
	CHashList<bool> hashSkipCheck;
	for(CLDSParamPlate* pCoverPlate=hashCoverPlates.GetFirst();pCoverPlate;pCoverPlate=hashCoverPlates.GetNext())
	{
		long iUpDownType=0;	//-1:�Ϸ��;1:�·��
		long hConnPlate1=0,hConnPlate2=0;
		pCoverPlate->GetDesignItemValue('F',&hConnPlate1);
		pCoverPlate->GetDesignItemValue('G',&hConnPlate2);
		pCoverPlate->GetDesignItemValue('S',&iUpDownType);
		WELDSONPART* pWeldSonPart=NULL;
		CBriefWeldPart* pCoverWeldPlate=NULL;
		long hPlate =hConnPlate1;
		if(hConnPlate2>0&&hConnPlate2<hConnPlate1)
		{	//ȡ��С����Ч�����Ϊ���ӻ�׼��
			hConnPlate1=hConnPlate2;
			hConnPlate2=hPlate;
		}
		if(hConnPlate1>0x20)
		{
			if(hashSkipCheck.GetValue(hConnPlate1)==NULL)
				pCoverWeldPlate=hashPartGroups.GetValue(hConnPlate1);
			if(pCoverWeldPlate)
				pCoverWeldPlate->DeleteList();
			else
				pCoverWeldPlate=hashPartGroups.Add(hConnPlate1);
			pCoverWeldPlate->SetConstWeldType(CBriefWeldPart::WELD_TUBECONNPLATES_INC_COVER);
			hashSkipCheck.SetValue(hConnPlate1,true);
			pWeldSonPart=pCoverWeldPlate->EnumObjectFirst();
			if(pWeldSonPart==NULL&&hConnPlate2>0x20)	//���ڶ����׼��ӽ�ȥ
				pCoverWeldPlate->AttachObject(WELDSONPART(hConnPlate2));
			pWeldSonPart=pCoverWeldPlate->AttachObject(WELDSONPART(pCoverPlate));
			if(iUpDownType<0)	//�����Ϸ����
				pWeldSonPart->iFuncType=1;
			else
				pWeldSonPart->iFuncType=2;
		}
	}
}
void CTower::RefreshPartGroups(CHashList<SEGI>& hashSegTbl,BOOL bDisplayProcess/*=FALSE*/)
{
	CLogErrorLife log;
	CHashTable<long> grouptable;
	grouptable.CreateHashTable(Parts.GetNodeNum());
	CLDSPart *pPart=NULL;
	CLDSPartGroup *pPartGroup=NULL;
	//PartGroup.Empty();	//ˢ���麸���б����  wht 10-04-15
	for(pPartGroup=PartGroup.GetFirst();pPartGroup;pPartGroup=PartGroup.GetNext())
		pPartGroup->m_uStatMatCoef=1;
	int nCurPorcess=0,nPartCount=Parts.GetNodeNum();
	if(bDisplayProcess)
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess(0,"Prepare data...");
#else
		DisplayProcess(0,"׼������...");
#endif
	AddJointTubeWeldRelations(&hashSegTbl);
	for(pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		//01:��Ϻ������еĸ�(��)����;10:��Ϻ����е��ӹ���;11:��Ϻ������м伶����
		long status=0;
		//Ϊ��ֹ��κŲ������麸����Σ������´���ͳ�ƣ��˴������й��� wjh-2012.4.7
		//if(hashSegTbl.GetHashTableSize()>0&&!hashSegTbl.ContainsKey(pPart->iSeg))
		//	continue;	//���ڵ�ǰָ���κŷ�Χ��
		CLDSPart* pRootPart=NULL;
		if(pPart->m_hPartWeldParent>0x20)
		{
			CLdsPartListStack stack(&Parts);
			pRootPart=Parts.FromHandle(pPart->m_hPartWeldParent);
			if(pRootPart==NULL&&(hashSegTbl.GetNodeNum()==0||hashSegTbl.GetValue(pPart->iSeg)!=NULL))
			{
				long hWeldParent=pPart->m_hPartWeldParent;
				pPart->IntelliRecogWeldLine();
				if(hWeldParent!=pPart->m_hPartWeldParent)
					pRootPart=Parts.FromHandle(pPart->m_hPartWeldParent);
				if(pRootPart==NULL)
				{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("0x%X{%s} part's welding parent part lost, may affect the statistics of the welding parts.",pPart->handle,(char*)pPart->GetPartNo());
#else
				logerr.Log("0x%X����{%s}�ĺ��Ӹ�������ʧ������Ӱ���麸����ȷͳ��",pPart->handle,(char*)pPart->GetPartNo());
#endif
				}
				else
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X{%s} part's welding parent part lost, but this error is corrected by software.",pPart->handle,(char*)pPart->GetPartNo());
#else
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X����{%s}�ĺ��Ӹ�������ʧ������������ʶ������Ӹ�����",pPart->handle,(char*)pPart->GetPartNo());
#endif
			}
		}
		if(pRootPart && pRootPart->m_uStatMatCoef!=0)//pPart->m_hPartWeldParent>0x20)
		{	//��ͳ��ͳ��ϵ��Ϊ0���麸��
			//�趨pPart->m_hPartWeldParent�ĸ���������(ֻ���ĸ�λ��Ϊ1)
			if(grouptable.GetValueAt(pPart->m_hPartWeldParent,status))
				status=status-status%10+1;
			else
				status=1;
			grouptable.SetValueAt(pPart->m_hPartWeldParent,status);
			//�趨pPart���ӹ�������(ֻ����ʰλ��Ϊ1)
			if(grouptable.GetValueAt(pPart->handle,status))
				status=10+status%10;
			else
				status=10;
			grouptable.SetValueAt(pPart->handle,status);
		}
		else if(pPart->m_uStatMatCoef!=0)
		{	//��ͳ��ͳ��ϵ��Ϊ0���麸��
			if(grouptable.GetValueAt(pPart->handle,status))
				status=status%10;	//�趨���������ԣ���ʰλ��Ϊ0
			else
				status=0;
			grouptable.SetValueAt(pPart->handle,status);
		}
		if(bDisplayProcess)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess(ftoi(100*nCurPorcess/(2*nPartCount)),"Prepare data...");
#else
			DisplayProcess(ftoi(100*nCurPorcess/(2*nPartCount)),"׼������...");
#endif
			nCurPorcess++;
		}
	}
	for(pPartGroup=PartGroup.GetFirst();pPartGroup;pPartGroup=PartGroup.GetNext())
	{
		CLDSPart *pRootPart=Parts.FromHandle(pPartGroup->handle);
		if(pRootPart)
		{
			strcpy(pPartGroup->sRootPartNo,pRootPart->GetPartNo());
			strcpy(pPartGroup->sGroupName,pRootPart->GetPartNo());
		}
	}
	for(pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		long status=0;
		if(bDisplayProcess)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess(ftoi(100*nCurPorcess/(2*nPartCount)),"Prepare data...");
#else
			DisplayProcess(ftoi(100*nCurPorcess/(2*nPartCount)),"׼������...");
#endif
			nCurPorcess++;
		}
		if(grouptable.GetValueAt(pPart->handle,status))
		{
			if(status!=1)	//���Ǹ�����
				continue;
			if(hashSegTbl.GetNodeNum()>0&&hashSegTbl.GetValue(pPart->iSeg.iSeg)==NULL)
				continue;	//���������ڵ�ǰָ���κŷ�Χ�ڣ����������з���ͳ��
			//��Ϲ����ĸ�����
			for(pPartGroup=PartGroup.GetFirst();pPartGroup;pPartGroup=PartGroup.GetNext())
			{
				if(pPartGroup->handle==pPart->handle)
					break;
				else if(stricmp(pPartGroup->sRootPartNo,pPart->GetPartNo())==0)
				{
					pPartGroup->m_uStatMatCoef++;
					CLDSPart *pRootPart=Parts.FromHandle(pPartGroup->handle);
					if(pRootPart&&ComparePartLayer(pPart,pRootPart)==1)
					{	//�����麸�����,����ϵ,�κ� wht 10-08-26
						pPartGroup->handle=pPart->handle;
						if(pPart->IsLinePart())
						{	//���͹���һ���Գ��ȷ���ΪZ��,���߷���ΪY��,���ͼʱ��ϰ�߲�һ��
							pPartGroup->ucs.axis_x=pPart->ucs.axis_x;
							pPartGroup->ucs.axis_z=pPart->ucs.axis_y;
							pPartGroup->ucs.axis_y=pPartGroup->ucs.axis_z^pPartGroup->ucs.axis_x;
							normalize(pPartGroup->ucs.axis_y);
						}
						else
							pPartGroup->ucs=pPart->ucs;
						if(pPart->iSeg.iSeg<10)	//��λ��ǰ�߼ӿո�����ʱ�õ�
#ifdef AFX_TARG_ENU_ENGLISH
							sprintf(pPartGroup->sFolderName," Segment %s",(char*)pPart->iSeg.ToString());
						else
							sprintf(pPartGroup->sFolderName,"Segment %s segment",(char*)pPart->iSeg.ToString());
#else
							sprintf(pPartGroup->sFolderName," %s��",(char*)pPart->iSeg.ToString());
						else
							sprintf(pPartGroup->sFolderName,"%s��",(char*)pPart->iSeg.ToString());
#endif
					}
					break;
				}
			}
			if(pPartGroup==NULL)
			{
				pPartGroup=PartGroup.append();
				pPartGroup->handle=pPart->handle;
				pPartGroup->m_uStatMatCoef=1;
				if(pPart->IsLinePart())
				{	//���͹���һ���Գ��ȷ���ΪZ��,���߷���ΪY��,���ͼʱ��ϰ�߲�һ��
					pPartGroup->ucs.axis_x=pPart->ucs.axis_x;
					pPartGroup->ucs.axis_z=pPart->ucs.axis_y;
					pPartGroup->ucs.axis_y=pPartGroup->ucs.axis_z^pPartGroup->ucs.axis_x;
				}
				else
					pPartGroup->ucs=pPart->ucs;
				strcpy(pPartGroup->sGroupName,pPart->GetPartNo());
				strcpy(pPartGroup->sRootPartNo,pPart->GetPartNo());
				if(pPart->iSeg<10)	//��λ��ǰ�߼ӿո�����ʱ�õ�
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(pPartGroup->sFolderName," Segment %s",(char*)pPart->iSeg.ToString());
				else
					sprintf(pPartGroup->sFolderName,"Segment %s",(char*)pPart->iSeg.ToString());
#else
					sprintf(pPartGroup->sFolderName," %s��",(char*)pPart->iSeg.ToString());
				else
					sprintf(pPartGroup->sFolderName,"%s��",(char*)pPart->iSeg.ToString());
#endif
			}
		}
	}
	if(bDisplayProcess)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess(100,"Prepare data...");
		//
		nCurPorcess=0;
		DisplayProcess(0,"Analyze weld part group");
#else
		DisplayProcess(100,"׼������...");
		//
		nCurPorcess=0;
		DisplayProcess(0,"�����麸��");
#endif
	}
	//�麸�����Է�Ϊ��׼�ͷǱ�׼������
	//���麸����׼����Ϊ�ֹܣ����麸���н��������ֹ�ʼ/�ն����ӵĲ���/U��/ʮ�ֲ����߷���ʱΪ��׼�麸��
	int nPartGroupCount=PartGroup.GetNodeNum();
	for(pPartGroup=PartGroup.GetFirst();pPartGroup;pPartGroup=PartGroup.GetNext())
	{
		if(bDisplayProcess)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess(ftoi(100*nCurPorcess/nPartGroupCount),"Analyze weld part group");
#else
			DisplayProcess(ftoi(100*nCurPorcess/nPartGroupCount),"�����麸��");
#endif
			nCurPorcess++;
		}
		CLDSPart *pRootPart=Parts.FromHandle(pPartGroup->handle);
		if(pRootPart==NULL||(hashSegTbl.GetNodeNum()>0&&hashSegTbl.GetValue(pRootPart->iSeg.iSeg)==NULL))
			continue;	//���������ڵ�ǰָ���κŷ�Χ�ڣ����������з���ͳ��
		PARTSET partset;
		partset.Empty();
		GetPartGroupPartSet(pPartGroup->handle,partset);
		if(partset.GetNodeNum()>=3||pRootPart->GetClassTypeId()!=CLS_LINETUBE)
			pPartGroup->m_iPartGroupType=CLDSPartGroup::GROUP_COMMON;	//��������Ϊ�ֹܣ����麸���е��ӹ������ڵ���3��ʱΪ�Ǳ�׼�麸��
		else 
		{
			CLDSLineTube *pDatumTube=NULL;
			BOOL bStdPartGroup=TRUE;
			pDatumTube=(CLDSLineTube*)pRootPart;
			for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
			{
				//if(pPart==pRootPart)
				//	continue;
				if(pPart->GetClassTypeId()!=CLS_PARAMPLATE)
				{	//����׼���������๹����Ϊ����������п���Ϊ��׼�麸��
					bStdPartGroup=FALSE;
					break;
				}
				CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
				if(pParamPlate->handle!=pDatumTube->m_tJointStart.hLinkObj&&
					pParamPlate->handle!=pDatumTube->m_tJointEnd.hLinkObj)
				{	//�麸���а����Ǹֹܶ����ӵĲ�������
					bStdPartGroup=FALSE;
					break;
				}
			}
			if(bStdPartGroup)
				pPartGroup->m_iPartGroupType=CLDSPartGroup::GROUP_ENDCONN;
			else
				pPartGroup->m_iPartGroupType=CLDSPartGroup::GROUP_COMMON;
		}
		//����˴���ִ�д˺������׵��¶�ʧ�麸���������ԣ�����ΪGROUP_COMMON����) wjh-2019.8.9
		pPartGroup->SmartIdentifyGroupType();
	}

	CHashList<CLDSPart*>hashStatSonParts;
	for(pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		long status=0;
		if(grouptable.GetValueAt(pPart->handle,status))
		{
			if(status!=1)	//���Ǹ�����
				continue;
			if(hashSegTbl.GetNodeNum()>0&&hashSegTbl.GetValue(pPart->iSeg.iSeg)==NULL)
				continue;	//���������ڵ�ǰָ���κŷ�Χ�ڣ����������з���ͳ��
			PARTSET partset;
			partset.Empty();
			GetPartGroupPartSet(pPart->handle,partset);
			for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())//���ڼ�����趨���Ӹ�������β�ν���ѭ��������������ĺ��Ӽ� wjh-2015.8.26
				hashStatSonParts.SetValue(pPart->handle,pPart);
		}
	}
	for(pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		if(hashSegTbl.GetNodeNum()>0&&hashSegTbl.GetValue(pPart->iSeg.iSeg)==NULL)
			continue;	//���������ڵ�ǰָ���κŷ�Χ�ڣ����������з���ͳ��
		long status=0;
		grouptable.GetValueAt(pPart->handle,status);
		if(status>1&&hashStatSonParts.GetValue(pPart->handle)==NULL)
		{
			if(status==10)
			{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("0x%X part{%s}'s parent weld part may not be in the current statistical segments. ",pPart->handle,(char*)pPart->GetPartNo());
#else
			logerr.Log("0x%X����{%s}��(�ռ�)���Ӹ��������ܲ��ڵ�ǰͳ�ƶκŷ�Χ�ڣ�δ����ȷͳ��Ϊ�麸��",pPart->handle,(char*)pPart->GetPartNo());
#endif
			}
			else //if(status>10)
			{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("0x%X part{%s}is weld part��but not identified as welding group parts. Please check if there is a parent weld part deadlock situation.",pPart->handle,(char*)pPart->GetPartNo());
#else
			logerr.Log("0x%X����{%s}Ϊ���Ӽ������ǿ������ں��Ӹ������趨������ѭ��δ����ȷͳ��Ϊ�麸��",pPart->handle,(char*)pPart->GetPartNo());
#endif
			}
		}
	}
	if(bDisplayProcess)
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess(100,"Analyze weld part group");
#else
		DisplayProcess(100,"�����麸��");
#endif
}
#endif
bool CTower::InitEvolveCfgwordMap(int version,long doc_type)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if( (doc_type==1&&version>0&&version<4020006)||	//TMA
		(doc_type==2&&version>0&&version<1020006)||	//LMA
		(doc_type==3&&version>0&&version<2000006)||	//TSA
		(doc_type==4&&version>0&&version<1020006)||	//LDS
		(doc_type==5&&version>0&&version<1020006))	//TDA
	{
		memset(cfglegMap,0,sizeof(CFGWORD)*160);
		int index=0;
		const DWORD flagConstArr[24]={
			0x000001,0x000002,0x000004,0x000008,0x000010,0x000020,0x000040,0x000080,
			0x000100,0x000200,0x000400,0x000800,0x001000,0x002000,0x004000,0x008000,
			0x010000,0x020000,0x040000,0x080000,0x100000,0x200000,0x400000,0x800000	};
		for(CLDSModule* pModule=Module.GetFirst();pModule&&index<CFGLEG::MaxBodys();pModule=Module.GetNext(),index++)
		{
			cfglegMap[pModule->GetBodyNo(false)-1].SetBodyLegs(index+1);
			pModule->SetBodyNo(index+1);
			int i,j,legs=0;
			CFGWORD legword;
			WORD arrNewLegNo[4]={0}; 
			for(i=0;i<160&&legs<CFGLEG::MaxLegs();i++)
			{
				if(pModule->m_dwLegCfgWord.IsHasNo(i+1))
				{
					//cfglegMap[i].flag.bytes[index]=byteConstArr[legs];
					cfglegMap[i].SetBodyLegs(index+1,flagConstArr[legs]);
					legword.AddSpecWord(cfglegMap[i]);
					legs++;
					for(j=0;j<4;j++)
					{
						if(arrNewLegNo[j]==0&&pModule->m_arrActiveQuadLegNo[j]==i+1)
							arrNewLegNo[j]=8*(pModule->GetBodyNo(false)-1)+legs;
					}
				}
			}
			pModule->m_dwLegCfgWord.Clear();
			pModule->m_dwLegCfgWord=legword;
			for(j=0;j<4;j++)
				pModule->m_arrActiveQuadLegNo[j]=arrNewLegNo[j];
		}
		return true;
	}
	return true;
}
bool CTower::EvolveCfgword(CFGWORD& cfgword,int version,long doc_type)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if( (doc_type==1&&version>0&&version<4020006)||	//TMA
		(doc_type==2&&version>0&&version<1020006)||	//LMA
		(doc_type==3&&version>0&&version<2000006)||	//TSA
		(doc_type==4&&version>0&&version<1020006)||	//LDS
		(doc_type==5&&version>0&&version<1020006))	//TDA
	{
		CFGWORD cfgnew;
		for(int i=1;i<=160;i++)
		{
			if(cfgword.IsHasNo(i))
				cfgnew.AddSpecWord(cfglegMap[i-1]);
		}
		cfgword=cfgnew;
		return true;
	}
	else
		return false;
}
static CFGWORD MappingCfgWord(CFGWORD cfgword,const int* fromBodyNoArr,const int* toBodyNoArr,int nBodyNo/*=1*/)
{
	int nLegBytes=CFGLEG::MaxLegs()/8;
	CFGWORD dest=cfgword;
	for(int i=0;i<nBodyNo;i++)
	{
		if(fromBodyNoArr[i]==toBodyNoArr[i])
			continue;
		int fromBytePos=(fromBodyNoArr[i]-1)*nLegBytes;
		int toBytePos  =(toBodyNoArr[i]-1)*nLegBytes;
		memcpy(&dest.flag.bytes[toBytePos],&cfgword.flag.bytes[fromBytePos],nLegBytes);
	}
	return dest;
}
bool CTower::ReplaceObjBodyNo(const int* fromBodyNoArr,const int* toBodyNoArr,int nBodyNo/*=1*/)
{
	int i;
	for(CLDSModule* pModule=Module.GetFirst();pModule;pModule=Module.GetNext())
	{
		for(i=0;i<nBodyNo;i++)
		{
			if(pModule->GetBodyNo()==fromBodyNoArr[i])
			{
				pModule->SetBodyNo(toBodyNoArr[i]);
				pModule->m_dwLegCfgWord=MappingCfgWord(pModule->m_dwLegCfgWord,fromBodyNoArr,toBodyNoArr,nBodyNo);
				break;
			}
		}
	}
	for(CLDSNode* pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		pNode->cfgword=MappingCfgWord(pNode->cfgword,fromBodyNoArr,toBodyNoArr,nBodyNo);
		pNode->SetModified(true,false);
	}
	for(CLDSPart* pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		pPart->cfgword=MappingCfgWord(pPart->cfgword,fromBodyNoArr,toBodyNoArr,nBodyNo);
		pPart->SetModified(true,false);
	}
	for(CSupplementPart* pSuppPart=SuppPart.GetFirst();pSuppPart;pSuppPart=SuppPart.GetNext())
	{
		pSuppPart->cfgword=MappingCfgWord(pSuppPart->cfgword,fromBodyNoArr,toBodyNoArr,nBodyNo);
		pSuppPart->SetModified(true,false);
	}
	return true;
}
bool CTower::ReBuildObjsHashTable()
{
	int n=Point.GetNodeNum();
	n+=Line.GetNodeNum();
	n+=Plane.GetNodeNum();
	n+=View.GetNodeNum();
	n+=Module.GetNodeNum();
	n+=Node.GetNodeNum();
	n+=Parts.GetNodeNum();
#ifdef __PART_DESIGN_INC_
	n+=WorkStatus.GetNodeNum();	//����״��
	n+=AttachWindLoad.GetNodeNum();	//���ӷ����
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	n+=ArcLift.GetNodeNum();
	n+=SuppPart.GetNodeNum();
#endif
#ifdef __COMPLEX_PART_INC_
	n+=BlockRef.GetNodeNum();
#endif
	n+=Foundations.GetNodeNum();
	hashObjs.Empty();
	hashObjs.SetHashTableGrowSize(n,false);
	for(CLDSPoint* pPoint=Point.GetFirst();pPoint;pPoint=Point.GetNext())	//��׼��
		hashObjs.SetValue(pPoint->handle,pPoint);
	for(CLDSLine* pLine=Line.GetFirst();pLine;pLine=Line.GetNext())			//��׼��
		hashObjs.SetValue(pLine->handle,pLine);
	for(CLDSPlane* pPlane=Plane.GetFirst();pPlane;pPlane=Plane.GetNext())	//��׼��
		hashObjs.SetValue(pPlane->handle,pPlane);
	bool duplicated=false;
	for(CDisplayView *pView=View.GetFirst();pView;pView=View.GetNext())
	{
		if(hashObjs.GetValue(pView->handle))
		{	//����2009��5��14��֮ǰ�İ汾GetMinAvailableHandle()���������⣬������ͼ����п��������������ظ�
			//������ʱ�Ȳ������������ͼ���
			duplicated=true;
			continue;
		}
		hashObjs.SetValue(pView->handle,pView);							//��ͼ
	}
	for(CLDSModule* pModule=Module.GetFirst();pModule;pModule=Module.GetNext())
		hashObjs.SetValue(pModule->handle,pModule);						//ģ��
	for(CLDSNode* pNode=Node.GetFirst();pNode;pNode=Node.GetNext())			//�ڵ�
		hashObjs.SetValue(pNode->handle,pNode);
	for(CLDSPart* pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())		//����
		hashObjs.SetValue(pPart->handle,pPart);
#ifdef __PART_DESIGN_INC_
	for(CWorkStatus* pCase=WorkStatus.GetFirst();pCase;pCase=WorkStatus.GetNext())	//����״��
		hashObjs.SetValue(pCase->handle,pCase);			//���ع���
	for(CAttachWindLoad* pWindLoad=AttachWindLoad.GetFirst();pWindLoad;pWindLoad=AttachWindLoad.GetNext())	//����״��
		hashObjs.SetValue(pWindLoad->handle,pWindLoad);	//���ӷ����
	n+=AttachWindLoad.GetNodeNum();
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	for(CLDSArcLift *pLift=ArcLift.GetFirst();pLift;pLift=ArcLift.GetNext())
		hashObjs.SetValue(pLift->handle,pLift);					//���乹��
	for(CSupplementPart *pSuppPart=SuppPart.GetFirst();pSuppPart;pSuppPart=SuppPart.GetNext())
		hashObjs.SetValue(pSuppPart->handle,pSuppPart);					//���乹��
#endif
#ifdef __COMPLEX_PART_INC_
	for(CBlockReference *pRef=BlockRef.GetFirst();pRef;pRef=BlockRef.GetNext())
		hashObjs.SetValue(pRef->handle,pRef);
#endif
	for(CLDSFoundation *pFoundation=Foundations.GetFirst();pFoundation;pFoundation=Foundations.GetNext())
		hashObjs.SetValue(pFoundation->handle,pFoundation);
	Parts.RebuildHashtable();
	return !duplicated;
}
long CTower::GetMinIdleHandle(long hStart/*=0x20*/)
{
	ReBuildObjsHashTable();
	long hMinCanUseHandle=0x50;	//��0x50��ʼ̽����þ��ֵ
	for(CLDSPoint* pPoint=Point.GetFirst();pPoint;pPoint=Point.GetNext())	//��׼��
	{
		BOOL pushed=Point.push_stack();
		if(hashObjs.GetValue(hMinCanUseHandle))
		{
			hMinCanUseHandle++;
			continue;
		}
		ReplaceReferenceHandle(CHandleReplacer(pPoint->hAgentBlockRef,hMinCanUseHandle));
		pPoint->handle=hMinCanUseHandle;
		Point.pop_stack(pushed);
	}
	/*for(CLDSLine* pLine=Line.GetFirst();pLine;pLine=Line.GetNext())			//��׼��
		hashObjs.SetValue(pLine->handle,pLine);
	for(CLDSPlane* pPlane=Plane.GetFirst();pPlane;pPlane=Plane.GetNext())	//��׼��
		hashObjs.SetValue(pPlane->handle,pPlane);
	//����2009��5��14��֮ǰ�İ汾GetMinAvailableHandle()���������⣬������ͼ����п��������������ظ�
	//������ʱ�Ȳ������������ͼ���
	//for(CDisplayView *pView=View.GetFirst();pView;pView=View.GetNext())
	//	hashObjs.SetValue(pView->handle,pView);							//��ͼ
	for(CLDSModule* pModule=Module.GetFirst();pModule;pModule=Module.GetNext())
		hashObjs.SetValue(pModule->handle,pModule);						//ģ��
	for(CLDSNode* pNode=Node.GetFirst();pNode;pNode=Node.GetNext())			//�ڵ�
		hashObjs.SetValue(pNode->handle,pNode);
	for(CLDSPart* pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())		//����
		hashObjs.SetValue(pPart->handle,pPart);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	for(CSupplementPart *pSuppPart=SuppPart.GetFirst();pSuppPart;pSuppPart=SuppPart.GetNext())
		hashObjs.SetValue(pSuppPart->handle,pSuppPart);					//���乹��
#endif
#ifdef __COMPLEX_PART_INC_
	for(CBlockReference *pRef=BlockRef.GetFirst();pRef;pRef=BlockRef.GetNext())
		hashObjs.SetValue(pRef->handle,pRef);
#endif*/
	return 0;
}
//���һЩ��Ч�ľ������Ӧ����Ķ����Ѳ����ڣ����Ա���������ʱ�����������⡡wjh-2015.3.8
void CTower::CleanInvalidHandleRef()
{
	for(CLDSPoint* pPoint=Point.GetFirst();pPoint;pPoint=Point.GetNext())	//��׼��
		pPoint->CleanInvalidObjRef(this);
	for(CLDSLine* pLine=Line.GetFirst();pLine;pLine=Line.GetNext())			//��׼��
		pLine->CleanInvalidObjRef(this);
	for(CLDSPlane* pPlane=Plane.GetFirst();pPlane;pPlane=Plane.GetNext())	//��׼��
		pPlane->CleanInvalidObjRef(this);
	for(CLDSNode* pNode=Node.GetFirst();pNode;pNode=Node.GetNext())			//�ڵ�
		pNode->CleanInvalidObjRef();
	for(CLDSPart* pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())		//����
		pPart->CleanInvalidObjRef();
	//TODO:δ�ꡡwjh-2015.3.8
	/*
	bool duplicated=false;
	for(CDisplayView *pView=View.GetFirst();pView;pView=View.GetNext())
		pView->CleanInvalidObjRef();
	for(CLDSModule* pModule=Module.GetFirst();pModule;pModule=Module.GetNext())
		pModule->CleanInvalidObjRef();
#ifdef __PART_DESIGN_INC_
	for(CWorkStatus* pCase=WorkStatus.GetFirst();pCase;pCase=WorkStatus.GetNext())	//����״��
		pCase->CleanInvalidObjRef();
	for(CWindSegment* pSeg=WindLoadSeg.GetFirst();pSeg;pSeg=WindLoadSeg.GetNext())
		pSeg->CleanInvalidObjRef(this);
	for(CAttachWindLoad* pWindLoad=AttachWindLoad.GetFirst();pWindLoad;pWindLoad=AttachWindLoad.GetNext())	//����״��
		pWindLoad->CleanInvalidObjRef();
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	for(CLDSArcLift *pLift=ArcLift.GetFirst();pLift;pLift=ArcLift.GetNext())
		pLift->CleanInvalidObjRef();
#endif
#ifdef __COMPLEX_PART_INC_
	for(CBlockReference *pRef=BlockRef.GetFirst();pRef;pRef=BlockRef.GetNext())
		pRef->CleanInvalidObjRef();
#endif
	*/
}
CXhChar500 ReplaceThickPartsStr(const char* szThickStr,long hShrinkStartHandle,CHashList<long>* pHashNewIdByOld)
{
	int hits=0;
	CXhChar500 rsltstr,keystr=szThickStr;
	char* key=strtok(keystr,", ");
	while(key)
	{
		long hRsltHandle=0;
		if(key[0]=='0'||(strlen(key)>2&&key[0]=='-'&&key[1]=='0'))	//���
		{
			long *pRsltHandle=NULL;;
			hRsltHandle=abs(HexStrToLong(key));
			if(hRsltHandle>=hShrinkStartHandle&&(pRsltHandle=pHashNewIdByOld->GetValue(hRsltHandle))!=NULL)
				hRsltHandle=(*pRsltHandle);
			CXhChar16 szHandle=key;
			if(key[1]=='x'||key[1]=='X'||key[1]=='*')
				szHandle[2]=0;
			else if(key[2]=='x'||key[2]=='X'||key[2]=='*')
				szHandle[3]=0;
			szHandle.Append(CXhChar16("%X",hRsltHandle));
			rsltstr.Append(szHandle,',');
			hits++;
			key=strtok(NULL,",");
		}
		else
		{
			rsltstr.Append(key,',');
			key=strtok(NULL,",");
			continue;	//ָ�����ֵ
		}
	}
	return rsltstr;
}
DWORD CTower::ShrinkObjHandle(DWORD hStartShrink/*=0x20*/)	//���������
{
	CleanInvalidHandleRef();	//TODO:��Ҫ��ʵ
	//�������һ�������Redo/Undo�����������
	UndoBuffer.ClearBuffer();
	UndoBufferStack.Empty();
	CXhSimpleList<HANDLE_ADDR> listShrinkHandles;
	for (CLDSPoint* pPoint = Point.GetFirst(); pPoint; pPoint = Point.GetNext())	//��׼��
	{
		if (pPoint->handle >= (long)hStartShrink)
			pPoint->StatObjHandleReference(&listShrinkHandles);
	}
	for (CLDSLine* pLine = Line.GetFirst(); pLine; pLine = Line.GetNext())			//��׼��
	{
		if (pLine->handle >= (long)hStartShrink)
			pLine->StatObjHandleReference(&listShrinkHandles);
	}
	for (CLDSPlane* pPlane = Plane.GetFirst(); pPlane; pPlane = Plane.GetNext())	//��׼��
	{
		if (pPlane->handle >= (long)hStartShrink)
			pPlane->StatObjHandleReference(&listShrinkHandles);
	}
	//for (CLDSModule* pModule = Module.GetFirst(); pModule; pModule = Module.GetNext())
	//	pModule->StatObjHandleReference(&listShrinkHandles);
	for (CLDSNode* pNode = Node.GetFirst(); pNode; pNode = Node.GetNext())			//�ڵ�
	{
		if (pNode->handle >= (long)hStartShrink)
			pNode->StatObjHandleReference(&listShrinkHandles);
	}
	CLDSPart* pTailPart = Parts.GetTail();
	for (CLDSPart* pPart = Parts.GetFirst(); pPart; pPart = Parts.GetNext())		//����
	{
		if (pPart->handle >= (long)hStartShrink)
			pPart->StatObjHandleReference(&listShrinkHandles);
	}
#ifdef __PART_DESIGN_INC_
	//for (CWorkStatus* pCase = WorkStatus.GetFirst(); pCase; pCase = WorkStatus.GetNext())	//����״��
	//	pCase->StatObjHandleReference(&listShrinkHandles);
	//for (CAttachWindLoad* pWindLoad = AttachWindLoad.GetFirst(); pWindLoad; pWindLoad = AttachWindLoad.GetNext())	//����״��
	//	pWindLoad->StatObjHandleReference(&listShrinkHandles);
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	for (CLDSArcLift *pLift = ArcLift.GetFirst(); pLift; pLift = ArcLift.GetNext())
	{
		if (pLift->handle >= (long)hStartShrink)
			listShrinkHandles.Append(HANDLE_ADDR(&pLift->handle));
	}
	for (CSupplementPart *pSuppPart = SuppPart.GetFirst(); pSuppPart; pSuppPart = SuppPart.GetNext())
		pSuppPart->StatObjHandleReference(&listShrinkHandles);
#endif

	CHashList<long> hashNewIdByOld;
	HANDLE_ADDR* pShrinkHandle;
	for (pShrinkHandle = listShrinkHandles.EnumObjectFirst(); pShrinkHandle; pShrinkHandle = listShrinkHandles.EnumObjectNext())
	{
		if(pShrinkHandle->HandleId>= (long)hStartShrink)
			hashNewIdByOld.Add(pShrinkHandle->HandleId);
	}
	long i = 0, _xarrIds[150000];
	ARRAY_LIST<long> xarrIds(_xarrIds, 150000);
	for (long* pId = hashNewIdByOld.GetFirst(); pId; pId = hashNewIdByOld.GetNext())
		xarrIds.append(hashNewIdByOld.GetCursorKey());
	CQuickSort<long>::QuickSort(xarrIds.m_pData, xarrIds.Count, &SimpleCompareFunc<long>);
	//logerr.Log("StartShrink InitObjH=0x%X", hStartShrink);
	for (i = 0; i < xarrIds.Count; i++)
	{
		hashNewIdByOld.SetValue(xarrIds[i], i + hStartShrink);
		//logerr.Log("%5d#0x%X->0x%X", i, xarrIds[i], i + hStartShrink);
	}
	for (pShrinkHandle = listShrinkHandles.EnumObjectFirst(); pShrinkHandle; pShrinkHandle = listShrinkHandles.EnumObjectNext())
	{
		if(pShrinkHandle->ciAddrType==1)
		{	//ָ���ȳ������ַ���������ַָ��
			CXhChar500 szThickStr=ReplaceThickPartsStr((char*)pShrinkHandle->pszThickPartStr,hStartShrink,&hashNewIdByOld);
			StrCopy(pShrinkHandle->pszThickPartStr,szThickStr,pShrinkHandle->wnMaxStrBuffLen);
			continue;
		}
		else if(pShrinkHandle->ciAddrType != 0)
			continue;	//�����Ƿ��������
		else if(pShrinkHandle->HandleId == 0||pShrinkHandle->HandleId<(long)hStartShrink)
			continue;
		long *pNewId = hashNewIdByOld.GetValue(abs(pShrinkHandle->HandleId));
		if (pNewId == NULL)
		{
			logerr.Log("0x%X���δƥ�䵽�¾��",pShrinkHandle->HandleId);
			continue;
		}
		if (pShrinkHandle->HandleId > 0)
			*pShrinkHandle->pHandlVarAddr = *pNewId;
		else if(pShrinkHandle->HandleId < 0)
			*pShrinkHandle->pHandlVarAddr = -(*pNewId);
	}
	Point.RebuildHashtable();
	Line.RebuildHashtable();
	Plane.RebuildHashtable();
	Node.RebuildHashtable();
	//Parts.RebuildHashtable();
	ReBuildObjsHashTable();//Include Parts.RebuildHashtable();
	handle = xarrIds.Count + hStartShrink;	//GetMinAvailableHandle();
	//logerr.Log("Current Idle handle=0x%X", handle);
	//////////////////////////////////////////////////////////////////////
	return handle;// (handle = hIdle);
}
//���ڲ����Ѵ��ڵı�׼��������ز���
/*CLDSStdPartModel* CTower::GetStdPartModel(CLDSParamPlate *pParamPlate)
{
	if(pParamPlate==NULL)
		return NULL;
	if(!pParamPlate->m_bStdPart)
		return NULL;
	//���ҶԺ���������ʱ��Ҫ���뷨�����ڸֹܵ�ֱ�� wht 10-01-21
	CLDSLineTube *pLineTube=(CLDSLineTube*)Parts.FromHandle(pParamPlate->m_hPartWeldParent,CLS_LINETUBE);
	double tube_t=0;
	if(pLineTube)
		tube_t=pLineTube->GetThick();
	return GetStdPartModel(pParamPlate->sPartNo,pParamPlate->m_iParamType,tube_t);
}*/

#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
//����һ������ڵ���������ڵ����и˼�(�����߽�˼�)��ͶӰ��ѹ�������(m2)
// double CTower::SnapNodeFace(NODESET &nodeset,f3dPoint prjOrg,double W,int iLoadExecuteCode,
// 							BOOL bAutoCalK,double *ProfileModeCoef,double *net_area,double *BtoH)
// {
// 	double Af=0.0;	//�������
// 	CLDSNode *pNode=NULL, *pStart=NULL, *pEnd=NULL;
// 	f3dPoint pos,jg_vec;
// 	f3dLine line;
// 	int i,node_n;
// 	f3dPoint origin,*pt_arr=NULL;
// 	fPolygonRgn selRgn;
// 	UCS_STRU ucs;		//�����չ��������ϵ
// 	UCS_STRU prj_ucs;	//����ε�����ķ�ѹͶӰ������ϵ
// 	node_n = nodeset.GetNodeNum();
// 	if(node_n<3)
// 	{
// 		if(net_area)
// 			*net_area=0;
// 		return 0;
// 	}
// 	pt_arr = new f3dPoint[node_n];
// 	for(pNode=nodeset.GetFirst(),i=0;pNode;pNode=nodeset.GetNext(),i++)
// 	{
// 		pt_arr[i] = pNode->pos;
// 		if(i==0)
// 			ucs.origin=pt_arr[i];
// 		else if(ucs.axis_x.IsZero())
// 		{
// 			ucs.axis_x=pt_arr[i]-pt_arr[i-1];
// 			normalize(ucs.axis_x);
// 		}
// 		else if(ucs.axis_y.IsZero())
// 		{
// 			ucs.axis_y=pt_arr[i]-pt_arr[i-1];
// 			normalize(ucs.axis_y);
// 			if(fabs(ucs.axis_y*ucs.axis_x)>EPS_COS)	//��X�Ṳ��Ϊ��ЧY�᷽��,��ռ���Ѱ��
// 				ucs.axis_y.Set();
// 			else
// 			{	//�ҵ���������ϵ
// 				ucs.axis_z = ucs.axis_x^ucs.axis_y;
// 				ucs.axis_y = ucs.axis_z^ucs.axis_x;
// 				normalize(ucs.axis_y);
// 				normalize(ucs.axis_z);
// 			}
// 		}
// 	}
// 	//�����ѹͶӰ��
// 	prj_ucs.origin=ucs.origin;
// 	prj_ucs.axis_z.Set(ucs.axis_z.x,ucs.axis_z.y);
// 	normalize(prj_ucs.axis_z);
// 	prj_ucs.axis_y.Set(0,0,-1);
// 	prj_ucs.axis_x=prj_ucs.axis_z^prj_ucs.axis_y;
// 	double min_z=0,max_z=0;
// 	SCOPE_STRU scope;
// 	for(i=0;i<node_n;i++)
// 	{
// 		pos=pt_arr[i];
// 		coord_trans(pos,prj_ucs,FALSE);
// 		scope.VerifyVertex(pos);
// 		coord_trans(pt_arr[i],ucs,FALSE);
// 		if(min_z>pt_arr[i].z)
// 			min_z=pt_arr[i].z;
// 		if(max_z<pt_arr[i].z)
// 			max_z=pt_arr[i].z;
// 		pt_arr[i].z=0;
// 	}
// 	//����BtoH
// 	if(BtoH)
// 	{
// 		double h;
// 		if(scope.wide()>scope.high())
// 			h=scope.high();
// 		else
// 			h=scope.wide();
// 		coord_trans(prjOrg,prj_ucs,FALSE);
// 		double b=fabs(2*prjOrg.z-(scope.fMaxZ+scope.fMinZ));
// 		*BtoH=b/h;
// 	}
// 	double prj_coef=fabs(prj_ucs.axis_z*ucs.axis_z)/1000000;	//��mm2->m2
// 	//�������ξ�ͶӰ���
// 	if(net_area)
// 	{
// 		if(!selRgn.CreatePolygonRgn(pt_arr, node_n))
// 			throw "��ѹ�νڵ�ѡ�񲻺Ϸ�!";
// 		double weightedK=0;
// 		for(CElement *pElem=ElementTemp.GetFirst();pElem;pElem=ElementTemp.GetNext())
// 		{
// 			pos = pElem->pStart->pos;
// 			coord_trans(pos,ucs,FALSE);
// 			if(pos.z>max_z||pos.z<min_z)
// 				continue;	//�����Զ��������淶Χ
// 			pos = pElem->pEnd->pos;
// 			coord_trans(pos,ucs,FALSE);
// 			if(pos.z>max_z||pos.z<min_z)
// 				continue;	//�����Զ��������淶Χ
// 			line.startPt = pElem->pStart->pos;
// 			line.endPt   = pElem->pEnd->pos;
// 			project_point(line.startPt,ucs.origin,ucs.axis_z);
// 			project_point(line.endPt,ucs.origin,ucs.axis_z);
// 			if(selRgn.LineInRgn(line)==1) //���ڶ���������ڣ������߽����ڣ�
// 			{
// 				double A;
// 				if(pElem->pOrgPole->GetClassTypeId()==CLS_GROUPLINEANGLE)//.pole_type==TRUSS_POLE_JG_D_COM	//�ԽǶԳ�˫�Ǹ����
// 					//||pElem->pOrgPole->guige.pole_type==TRUSS_POLE_JG_X_COM)//ʮ�ֶԳ��ĽǸ����
// 				{
// 					CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pElem->pOrgPole;
// 					if(pGroupAngle->group_style==1)	//T�����
// 						A=pElem->GetLength()*pElem->pOrgPole->GetWidth();
// 					else
// 						A=2*pElem->GetLength()*pElem->pOrgPole->GetWidth();
// 					if(bAutoCalK)
// 						weightedK+=A*1.1;
// 				}
// 				else
// 				{
// 					A=pElem->GetLength()*pElem->pOrgPole->GetWidth();
// 					if(pElem->pOrgPole->GetClassTypeId()==CLS_LINETUBE)
// 					{
// 						double d=pElem->pOrgPole->GetWidth();
// 						double W0d2=W*d*d*1e-6;
// 						double coef1,coef2,critical1=0.002,critical2=0.015;
// 						if(iLoadExecuteCode==0)
// 						{
// 							coef1=0.7/1.3;
// 							coef2=1.2/1.3;
// 						}
// 						else //if(iLoadExecuteCode==1||iLoadExecuteCode==2)
// 						{
// 							coef1=0.6;
// 							coef2=0.8;
// 						}
// 						if(iLoadExecuteCode==1)
// 						{
// 							critical1=0.003;
// 							critical2=0.02;
// 						}
// 						if(W0d2<=critical1)
// 							weightedK+=A*coef2;
// 						else if(W0d2>=critical2)
// 							weightedK+=A*coef1;
// 						else
// 							weightedK+=A*(coef1+(0.015-W0d2)*(coef2-coef1)/0.013);
// 					}
// 				}
// 				Af+=A;
// 			}
// 		}
// 		if(ProfileModeCoef)
// 			*ProfileModeCoef=weightedK/Af;
// 		if(pt_arr)
// 			delete []pt_arr;
// 		*net_area=Af*prj_coef;
// 	}
// 	//�����������������
// 	double wrap_area;
// 	for(i=1;i<node_n-1;i++)
// 	{
// 		double result=DistOf2dPtLine(pt_arr[i+1],pt_arr[0],pt_arr[i]);
// 		if(result>0)		// ���������࣬�����������
// 			wrap_area+=CalTriArea(pt_arr[0].x,pt_arr[0].y,pt_arr[i].x,pt_arr[i].y,pt_arr[i+1].x,pt_arr[i+1].y);
// 		else if(result<0)	// ��������Ҳ࣬�����������
// 			wrap_area-=CalTriArea(pt_arr[0].x,pt_arr[0].y,pt_arr[i].x,pt_arr[i].y,pt_arr[i+1].x,pt_arr[i+1].y);
// 	}
// 	return wrap_area*prj_coef;
// }
// void CTower::CalWindSegAf(int iLoadExecuteCode)
// {
// 	double v=GetActiveWorkStatus()->WindLoad.velocity;
// 	double W0=v*v/1600;
// 	for(CWindSegment *pWindSeg=WindLoadSeg.GetFirst();pWindSeg;pWindSeg=WindLoadSeg.GetNext())
// 	{
// 		int iSegNo=pWindSeg->cType;		//0.�˽ڵ���������;1.�Զ����ڵ�����;2.���ڵ����������
// 		int iStyle=pWindSeg->cCalStyle;	//0.����ʽ����1.X����ˮƽ�ᵣ2.Y����ˮƽ�ᵣ
// 		pWindSeg->FaiFront=pWindSeg->FaiSide=pWindSeg->B_AFront=pWindSeg->B_ASide=0.0;
// 		BOOL bIncK=pWindSeg->m_bAutoCalK;
// 		memset(pWindSeg->nodePtrArr,0,sizeof(pWindSeg->nodePtrArr));
// 		int i,nValidNode=pWindSeg->GetValidNodeCount(this);
// 		if(iStyle!=0)	//�ᵣ��ѹ��
// 			bIncK=FALSE;
// 		//int iMirMsg=((pWindSeg->iType/20)%1000)/100;	//�Գ���Ϣ0.�޶Գ�1.���ҶԳ�
// 		if(pWindSeg->cType==0)	//�˵���������
// 		{
// 			NODESET nodeset;
// 			if(pWindSeg->nodePtrArr[0]==NULL||pWindSeg->nodePtrArr[1]==NULL)
// 				pWindSeg->AfFront=pWindSeg->AfSide=0;	//����������
// 			else
// 			{
// 				nodeset.append(pWindSeg->nodePtrArr[0]);
// 				nodeset.append(pWindSeg->nodePtrArr[1]);
// 				//Y��Գ�
// 				pWindSeg->nodePtrArr[2]=pWindSeg->nodePtrArr[1]->GetMirNode(MIRMSG(2));
// 				pWindSeg->nodePtrArr[3]=pWindSeg->nodePtrArr[0]->GetMirNode(MIRMSG(2));
// 				if(pWindSeg->nodePtrArr[2]==NULL||pWindSeg->nodePtrArr[3]==NULL)
// 					pWindSeg->AfFront=pWindSeg->AfSide=0;	//����������
// 				else	//��������
// 				{
// 					nodeset.append(pWindSeg->nodePtrArr[2]);
// 					nodeset.append(pWindSeg->nodePtrArr[3]);
// 					double A = SnapNodeFace(nodeset,pWindSeg->project_origin,W0,iLoadExecuteCode,bIncK,&pWindSeg->ProfileModeCoef,&pWindSeg->AfFront,&pWindSeg->B_AFront);
// 					pWindSeg->FaiFront=pWindSeg->CqiFront*pWindSeg->AfFront/A;
// 				}
// 				CLDSNode *baknode[2];
// 				//X��Գ�
// 				baknode[0]=pWindSeg->nodePtrArr[1]->GetMirNode(MIRMSG(1));
// 				baknode[1]=pWindSeg->nodePtrArr[0]->GetMirNode(MIRMSG(1));
// 				if(nodeset.GetNodeNum()>=4)
// 				{
// 					nodeset.DeleteNode(pWindSeg->nodePtrArr[3]->handle);
// 					nodeset.DeleteNode(pWindSeg->nodePtrArr[2]->handle);
// 				}
// 				if(baknode[0]&&baknode[1])	//��������
// 				{
// 					nodeset.append(baknode[0]);
// 					nodeset.append(baknode[1]);
// 					double A = SnapNodeFace(nodeset,pWindSeg->project_origin,W0,iLoadExecuteCode,bIncK,&pWindSeg->ProfileModeCoef,&pWindSeg->AfSide,&pWindSeg->B_ASide);
// 					pWindSeg->FaiSide=pWindSeg->CqiSide*pWindSeg->AfSide/A;
// 				}
// 				else	//����������
// 					pWindSeg->AfSide=0;
// 				if(iStyle==1)	//X��ˮƽ�ᵣ
// 					pWindSeg->AfSide=0;
// 				else if(iStyle==2)	//Y��ˮƽ�ᵣ
// 					pWindSeg->AfFront=0;
// 			}
// 		}
// 		else if(pWindSeg->cType==1)	//�Զ����ڵ�����
// 		{
// 			NODESET nodeset;
// 			pWindSeg->FaiFront=pWindSeg->FaiSide=pWindSeg->B_AFront=pWindSeg->B_ASide=0;
// 			if(nValidNode<3)
// 				pWindSeg->AfFront=pWindSeg->AfSide=0;	//����������
// 			else
// 			{
// 				for(i=0;i<nValidNode;i++)
// 					nodeset.append(pWindSeg->nodePtrArr[i]);
// 				if(pWindSeg->cCalStyle==0||pWindSeg->cCalStyle==1)	//X����ᵣ
// 				{
// 					double A = SnapNodeFace(nodeset,pWindSeg->project_origin,W0,iLoadExecuteCode,bIncK,&pWindSeg->ProfileModeCoef,&pWindSeg->AfFront,&pWindSeg->B_AFront);
// 					pWindSeg->FaiFront=pWindSeg->CqiFront*pWindSeg->AfFront/A;
// 					pWindSeg->AfSide =0.0;
// 				}
// 				else// if(pWindSeg->cCalStyle==2)//Y����ᵣ
// 				{	
// 					double A=SnapNodeFace(nodeset,pWindSeg->project_origin,W0,iLoadExecuteCode,bIncK,&pWindSeg->ProfileModeCoef,&pWindSeg->AfSide,&pWindSeg->B_ASide);
// 					pWindSeg->FaiSide=pWindSeg->CqiSide*pWindSeg->AfSide/A;
// 					pWindSeg->AfFront=0.0;
// 				}
// 			}
// 		}
// 		else if(pWindSeg->cType==2)	//���������������
// 		{
// 			NODESET nodeset;
// 			if(pWindSeg->nodePtrArr[0]==NULL||pWindSeg->nodePtrArr[1]==NULL||pWindSeg->nodePtrArr[2]==NULL||pWindSeg->nodePtrArr[3]==NULL)
// 				pWindSeg->AfFront=pWindSeg->AfSide=0;	//����������
// 			else
// 			{
// 				nodeset.append(pWindSeg->nodePtrArr[0]);
// 				nodeset.append(pWindSeg->nodePtrArr[1]);
// 				nodeset.append(pWindSeg->nodePtrArr[2]);
// 				nodeset.append(pWindSeg->nodePtrArr[3]);
// 				double A=SnapNodeFace(nodeset,pWindSeg->project_origin,W0,iLoadExecuteCode,FALSE,&pWindSeg->ProfileModeCoef,&pWindSeg->AfFront,NULL);
// 				pWindSeg->FaiFront=pWindSeg->AfSide=pWindSeg->CqiFront*pWindSeg->AfFront/A;
// 				pWindSeg->B_AFront=(fabs(pWindSeg->nodePtrArr[0]->pos.x)+fabs(pWindSeg->nodePtrArr[1]->pos.x))
// 					/(fabs(pWindSeg->nodePtrArr[0]->pos.y)+fabs(pWindSeg->nodePtrArr[1]->pos.y));
// 
// 				if(iStyle==1)	//X��ˮƽ�ᵣ
// 					pWindSeg->AfSide=0;
// 				else if(iStyle==2)	//Y��ˮƽ�ᵣ
// 					pWindSeg->AfFront=0;
// 			}
// 		}
// 		pWindSeg->AfFront*=pWindSeg->CqiFront;	//��������ڵ㵲����Ŵ�ϵ��
// 		pWindSeg->AfSide*=pWindSeg->CqiSide;	//���ǲ���ڵ㵲����Ŵ�ϵ��
// 	}
// }
// void CTower::CalWindLoad(double preferredBetaZ/*=0.0*/,double magnifyCoef/*=1.0*/)		//�����ѹ�εĳ��ܷ�ѹ����
// {
// 	CWorkStatus *pStatus=GetActiveWorkStatus();
// 	CLDSModule *pModule=GetActiveModule();
// 	double lowest_z;	//����Z����
// 	double total_high=pModule->GetModuleScopeZ(NULL,&lowest_z)/1000;	//ȫ���ܸ�(m)
// 	double W0=pow(pStatus->WindLoad.velocity,2)/1.6;
// 	for(CLDSNode *pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
// 		pNode->attach_weight=0;
// 
// 	for(CWindSegment *pWindSeg=WindLoadSeg.GetFirst();pWindSeg;pWindSeg=WindLoadSeg.GetNext())
// 	{
// 		double mu_s;
// 		double Wsy=0,Wsx=0,Wx=0,Wy=0;
// 		double high_pos=0;//pModule->land_high;
// 		double coef_x=1.0,coef_y=1.0;
// 		double BH_X=0,BH_Y=0;
// 		BOOL bValidSeg=TRUE;
// 		int i,nValidNode=pWindSeg->GetValidNodeCount(this);
// 		for(i=0;i<nValidNode;i++)
// 		{
// 			if(!pModule->IsSonPart(pWindSeg->nodePtrArr[i]))
// 			{
// 				bValidSeg=FALSE;
// 				break;
// 			}
// 			high_pos+=pWindSeg->nodePtrArr[i]->pos.z;
// 		}
// 		if(!bValidSeg)	//�Ǳ�ģ�ͻ����ߵķ�ѹ��
// 			continue;
// 		if(pWindSeg->m_bAutoCalK||pWindSeg->cType!=0)
// 			pWindSeg->K=1.0;
// 		high_pos=(lowest_z-high_pos/nValidNode)/1000;	//�������ƽ����
// 		high_pos+=pModule->land_high;	//1000 for mm->m
// 		double mu_z=Find_MuZ(high_pos,m_cLandscape);	//���ҷ�ѹ�߶ȱ仯ϵ��
// 		double Xcoef_x,Xcoef_y,Ycoef_x,Ycoef_y;
// 		double betaZ=pWindSeg->BetaZ;
// 		if(preferredBetaZ>0)
// 			betaZ=preferredBetaZ;
// 		else if(magnifyCoef>0)
// 			betaZ*=magnifyCoef;
// 		if(pWindSeg->cType!=2)
// 		{
// 			mu_s = 1.3*(1+Find_YiTa(pWindSeg->FaiFront,pWindSeg->B_AFront));
// 			Wsy=W0*mu_z*mu_s*betaZ*pWindSeg->AfFront*pStatus->WindLoad.case_coef*pWindSeg->ProfileModeCoef;
// 			mu_s = 1.3*(1+Find_YiTa(pWindSeg->FaiSide,pWindSeg->B_ASide));
// 			Wsx=W0*mu_z*mu_s*betaZ*pWindSeg->AfSide*pStatus->WindLoad.case_coef*pWindSeg->ProfileModeCoef;
// 			GetWindLoadAllotCoef(&Xcoef_x,&Xcoef_y,&Ycoef_x,&Ycoef_y,pStatus->WindLoad.azimuth,pWindSeg->cCalStyle);
// 		}
// 		else if(pWindSeg->cType==2)
// 		{	//����������
// 			mu_s = Find_MuS(pWindSeg->FaiFront,0);
// 			Wsx=Wsy=W0*mu_z*mu_s*betaZ*pWindSeg->AfFront*pStatus->WindLoad.case_coef*pWindSeg->ProfileModeCoef;
// 			Xcoef_x=sin(pStatus->WindLoad.azimuth*RADTODEG_COEF);
// 			Ycoef_x=cos(pStatus->WindLoad.azimuth*RADTODEG_COEF);
// 			Xcoef_y=Ycoef_y=0;
// 		}
// 		if(pWindSeg->cCalStyle!=0)	//�ᵣ�λ�
// 		{
// 			Wx=Xcoef_x*Wsx+Xcoef_y*Wsy;
// 			Wy=Ycoef_x*Wsx+Ycoef_y*Wsy;
// 		}
// 		else
// 		{	//����б������
// 			Wx=pWindSeg->K*(Xcoef_x*Wsx+Xcoef_y*Wsy);
// 			Wy=pWindSeg->K*(Ycoef_x*Wsx+Ycoef_y*Wsy);
// 		}
// 		if(pWindSeg->cType==0&&nValidNode>=2)	//�˵���������
// 		{
// 			CLDSNode *mir_node;
// 			if(pWindSeg->nodePtrArr[0]&&pWindSeg->nodePtrArr[1])
// 			{
// 				pWindSeg->nodePtrArr[0]->WindLoad.x+=Wx/8;
// 				pWindSeg->nodePtrArr[0]->WindLoad.y+=Wy/8;
// 				pWindSeg->nodePtrArr[1]->WindLoad.x+=Wx/8;
// 				pWindSeg->nodePtrArr[1]->WindLoad.y+=Wy/8;
// 				for(i=1;i<4;i++)
// 				{
// 					mir_node=pWindSeg->nodePtrArr[0]->GetMirNode(MIRMSG(i));
// 					if(mir_node)
// 					{
// 						mir_node->WindLoad.x+=Wx/8;
// 						mir_node->WindLoad.y+=Wy/8;
// 					}
// 					mir_node=pWindSeg->nodePtrArr[1]->GetMirNode(MIRMSG(i));
// 					if(mir_node)
// 					{
// 						mir_node->WindLoad.x+=Wx/8;
// 						mir_node->WindLoad.y+=Wy/8;
// 					}
// 				}
// 			}
// 		}
// 		else if(pWindSeg->cType==1&&nValidNode>=3)	//�Զ�������
// 		{
// 			CLDSNode *mir_node;
// 			int nSumAllocLoadNode=2*nValidNode;
// 			for(i=0;i<nValidNode;i++)
// 			{
// 				pWindSeg->nodePtrArr[i]->WindLoad.x+=Wx/nSumAllocLoadNode;
// 				pWindSeg->nodePtrArr[i]->WindLoad.y+=Wy/nSumAllocLoadNode;
// 				MIRMSG mirmsg;
// 				if(pWindSeg->cCalStyle==1)	//X����ᵣ
// 					mirmsg.axis_flag=0x01;
// 				else if(pWindSeg->cCalStyle==2)
// 					mirmsg.axis_flag=0x02;
// 				mirmsg.origin=pWindSeg->project_origin;
// 				mir_node=pWindSeg->nodePtrArr[i]->GetMirNode(mirmsg);
// 				if(mir_node)
// 				{
// 					mir_node->WindLoad.x+=Wx/nSumAllocLoadNode;
// 					mir_node->WindLoad.y+=Wy/nSumAllocLoadNode;
// 				}
// 				if(pWindSeg->cMirType==1||pWindSeg->cMirType==2)
// 				{
// 					if(pWindSeg->cMirType==1)
// 						mirmsg.axis_flag=0x02;	//Y��ԳƷ�ѹ��
// 					else if(pWindSeg->cMirType==2)
// 						mirmsg.axis_flag=0x01;	//X��ԳƷ�ѹ��
// 					mirmsg.origin.Set();
// 					CLDSNode *pMirNode=pWindSeg->nodePtrArr[i]->GetMirNode(mirmsg);
// 					if(pMirNode)
// 					{
// 						pMirNode->WindLoad.x+=Wx/nSumAllocLoadNode;
// 						pMirNode->WindLoad.y+=Wy/nSumAllocLoadNode;
// 					}
// 					if(mir_node)
// 					{
// 						pMirNode=mir_node->GetMirNode(mirmsg);
// 						if(pMirNode)
// 						{
// 							pMirNode->WindLoad.x+=Wx/nSumAllocLoadNode;
// 							pMirNode->WindLoad.y+=Wy/nSumAllocLoadNode;
// 						}
// 					}
// 				}
// 			}
// 		}
// 		else if(pWindSeg->cType==2&&nValidNode==4)	//����������������
// 		{
// 			if(pWindSeg->nodePtrArr[0]&&pWindSeg->nodePtrArr[1]&&pWindSeg->nodePtrArr[2]&&pWindSeg->nodePtrArr[3])
// 			{
// 				pWindSeg->nodePtrArr[0]->WindLoad.x+=Wx/6;
// 				pWindSeg->nodePtrArr[0]->WindLoad.y+=Wy/6;
// 				pWindSeg->nodePtrArr[1]->WindLoad.x+=Wx/6;
// 				pWindSeg->nodePtrArr[1]->WindLoad.y+=Wy/6;
// 				pWindSeg->nodePtrArr[2]->WindLoad.x+=Wx/6;
// 				pWindSeg->nodePtrArr[2]->WindLoad.y+=Wy/6;
// 				pWindSeg->nodePtrArr[3]->WindLoad.x+=Wx/6;
// 				pWindSeg->nodePtrArr[3]->WindLoad.y+=Wy/6;
// 				f3dPoint v_vec(0,0,1),h_vec,prj_norm;
// 				h_vec=pWindSeg->nodePtrArr[3]->pos-pWindSeg->nodePtrArr[1]->pos;
// 				prj_norm=v_vec^h_vec;
// 				prj_norm.z=0;
// 				normalize(prj_norm);
// 				CLDSNode *pUpBackNode=NULL,*pDownBackNode=NULL;
// 				double dist,distUpBackNode=0,distDownBackNode=0;
// 				for(pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
// 				{
// 					if(fabs(pNode->pos.z-pWindSeg->nodePtrArr[0]->pos.z)<EPS&&
// 						pNode!=pWindSeg->nodePtrArr[0]&&pNode!=pWindSeg->nodePtrArr[2])
// 					{
// 						dist=DistOf3dPtLine(pNode->pos,pWindSeg->nodePtrArr[0]->pos,pWindSeg->nodePtrArr[2]->pos);
// 						if(dist>distUpBackNode)
// 						{
// 							distUpBackNode=dist;
// 							pUpBackNode=pNode;
// 						}
// 					}
// 					if(fabs(pNode->pos.z-pWindSeg->nodePtrArr[1]->pos.z)<EPS&&
// 						pNode!=pWindSeg->nodePtrArr[1]&&pNode!=pWindSeg->nodePtrArr[3])
// 					{
// 						dist=DistOf3dPtLine(pNode->pos,pWindSeg->nodePtrArr[1]->pos,pWindSeg->nodePtrArr[3]->pos);
// 						if(dist>distDownBackNode)
// 						{
// 							distDownBackNode=dist;
// 							pDownBackNode=pNode;
// 						}
// 					}
// 				}
// 				if(pUpBackNode)
// 				{
// 					pUpBackNode->WindLoad.x+=Wx/6;
// 					pUpBackNode->WindLoad.y+=Wy/6;
// 				}
// 				if(pDownBackNode)
// 				{
// 					pDownBackNode->WindLoad.x+=Wx/6;
// 					pDownBackNode->WindLoad.y+=Wy/6;
// 				}
// 			}
// 		}
// 	}
// 	for(double *pShieldCoef=pStatus->AttachWindLoad.GetFirst();pShieldCoef;pShieldCoef=pStatus->AttachWindLoad.GetNext())
// 	{
// 		CAttachWindLoad *pWindLoad=AttachWindLoad.FromHandle(pStatus->AttachWindLoad.GetCursorKey());
// 		if(pWindLoad==NULL)	//�Է����ݿ��д˺�����ɾ������������δ�ܼ�ʱ����2005/01/17 WJH
// 			continue;
// 		double betaZ=pWindLoad->m_fBetaZ;
// 		if(preferredBetaZ>0)
// 			betaZ=preferredBetaZ;
// 		double node_wind_area=pWindLoad->m_fShieldArea/pWindLoad->GetNodeList()->GetNodeNum();
// 		double node_weight=pWindLoad->m_fAttachWeight/pWindLoad->GetNodeList()->GetNodeNum();
// 		for(long *pH=pWindLoad->GetNodeList()->GetFirst();pH;pH=pWindLoad->GetNodeList()->GetNext())
// 		{
// 			pNode=Node.FromHandle(*pH);
// 			if(pNode)
// 			{
// 				double high_pos=(lowest_z-pNode->pos.z)/1000+pModule->land_high;	//1000 for mm->m
// 				double mu_z=Find_MuZ(high_pos,m_cLandscape);	//���ҷ�ѹ�߶ȱ仯ϵ��
// 				double Wk=*pShieldCoef*pWindLoad->m_fProfileCoef*betaZ*mu_z*node_wind_area*pWindLoad->m_fShieldCoef*W0;
// 				pNode->WindLoad.x+=Wk*sin(RADTODEG_COEF*pStatus->WindLoad.azimuth);
// 				pNode->WindLoad.y+=Wk*cos(RADTODEG_COEF*pStatus->WindLoad.azimuth);
// 				pNode->attach_weight+=node_weight*9.8;
// 			}
// 		}
// 	}
// }

BOOL CTower::ImportLoadFile(char *file,double z2_coef)
{
	FILE *fp=fopen(file,"rt");
	if(fp==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Can't find loading file specified!");
#else
		AfxMessageBox("�Ҳ���ָ���ĺ����ļ�!");
#endif
		return FALSE;
	}
	char line_txt[200]="";
	if(feof(fp))	//�ļ�Ϊ��
	{
		fclose(fp);
		return FALSE;
	}
	long nLoad=0,nNodeLoad=0;
	CWorkStatus *pStatus;
	CObjNoGroup *pStatusNoGroup=NoManager.FromGroupID(WORKSTATUS_GROUP);
	pStatusNoGroup->EmptyNo();
	WorkStatus.Empty();
	fgets(line_txt,200,fp);
	CString sLine(line_txt);
	sLine.Replace(',',' ');		//��,��ת��Ϊ�ո�,ͳһ�����ʽ
	sLine.Replace('\t',' ');	//��Tab��ת��Ϊ�ո�,ͳһ�����ʽ
	sscanf(sLine,"%d%d",&nLoad,&nNodeLoad);	//����Ϊ������,�ڵ㼯�к�����
	BOOL bUpdateAcitveStatus=FALSE;
	int i;
	for(i=0;i<nLoad;i++)
	{
		pStatus=WorkStatus.append();
		pStatus->iNo=pStatusNoGroup->EnumIdleNo();
		pStatusNoGroup->SetNoState(pStatus->iNo);
		if(!bUpdateAcitveStatus)
		{
			m_hActiveStatus=pStatus->handle;
			bUpdateAcitveStatus=TRUE;
		}
	}
	for(i=0;i<2*nLoad+nNodeLoad;i++)
	{
		if(feof(fp))
			break;
		fgets(line_txt,200,fp);
		if(strlen(line_txt)>0&&line_txt[strlen(line_txt)-1]=='\n')
			line_txt[strlen(line_txt)-1]='\0';
		sLine=line_txt;
		sLine.Replace(',',' ');	//��,��ת��Ϊ�ո�,ͳһ�����ʽ
		sLine.Replace('\t',' ');	//��Tab��ת��Ϊ�ո�,ͳһ�����ʽ
		sLine.Remove('\n');
		sLine.Remove('\r');
		sLine.TrimRight();
		if(sLine.IsEmpty())
		{	//���в�������
			i--;
			continue;
		}
		pStatus=&WorkStatus[i%nLoad];
		if(i<nLoad)			//��ȡ����������ģ�ͺ��ַ���
			strncpy(pStatus->limit_str,line_txt,100);
		else if(i<2*nLoad)	//����ؼ���������
		{
			double kesai=1.0;
			double azimuth=0;
			sscanf(sLine,"%lf%lf%lf%s",&pStatus->WindLoad.velocity,
				&azimuth,&kesai,pStatus->description);
			pStatus->WindLoad.azimuth=ftoi(azimuth);
			//���뽫������Ϊָ������ϵ��ģʽ�������޷�����������ָ���ɱ�������ϵ��
			pStatus->m_bSpecLoadFactors=true;
			pStatus->m_bLoadIncFactor=true;
			pStatus->SetKesai(kesai);
			pStatus->SetKesai_A(kesai);
			pStatus->SetKesai_I(kesai);
			pStatus->SetKesai_T(kesai);
			pStatus->SetKesai_U(kesai);
			pStatus->SetKesai_W(kesai);
			if(strlen(pStatus->description)==0)
				sprintf(pStatus->description,"%d",i+1-nLoad);
		}
		else //if(i<2*nLoad+nNodeLoad)
		{
			pStatus=&WorkStatus[0];
			int point_i;
			f3dPoint load,stdload;
			double loadz2=0,stdloadz2=0;
			sscanf(sLine,"%d%lf%lf%lf%lf",&point_i,&load.x,&load.y,&load.z,&loadz2);
			load*=1000;	//kN->N
			loadz2*=1000;
			//������ر�׼ֵ wjh-2015.3.4
			stdload=load/(pStatus->GamaQ()*pStatus->Kesai());
			stdloadz2=loadz2/z2_coef;
			if(pStatus->m_cMainLoadType<4)
			{
				stdload/=m_fSafetyCoef;
				stdloadz2/=m_fSafetyCoef;
			}
			CLDSNode *pNode;
			for(pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
			{
				if(pNode&&pNode->point_i==point_i)
				{
					CExternalNodeLoad *pNodeLoad=pStatus->hashNodeLoad.GetValue(pNode->handle);
					if(pNodeLoad==NULL)
					{
						CExternalNodeLoad nodeLoad;
						nodeLoad.designLoad=load+GEPOINT(0,0,loadz2);	//�ܺ������ֵ
						nodeLoad.Fxyz=stdload;				//�ɱ���ر�׼ֵ
						nodeLoad.permanentFz = stdloadz2;	//���ú��ر�׼ֵ
						pStatus->hashNodeLoad.SetValue(pNode->handle,nodeLoad);
					}
					else
					{
						pNodeLoad->designLoad=load+GEPOINT(0,0,loadz2);
						pNodeLoad->Fxyz=stdload;	//��׼ֵ
						pNodeLoad->permanentFz = stdloadz2;
					}
					break;
				}
			}
			int nn=0;
			for(int j=1;j<nLoad;j++)
			{
				if(feof(fp))
					break;
				fgets(line_txt,200,fp);
				nn++;
				sLine=line_txt;
				sLine.Replace(',',' ');	//��,��ת��Ϊ�ո�,ͳһ�����ʽ
				sLine.Remove('\r');
				sLine.Remove('\n');
				sLine.TrimRight();
				sLine.TrimRight('	');
				if(sLine.IsEmpty())
				{	//���в�������
					j--;
					continue;
				}
				pStatus=&WorkStatus[j];
				sscanf(sLine,"%lf%lf%lf%lf",&load.x,&load.y,&load.z,&loadz2);
				load*=1000;	//kN->N
				loadz2*=1000;
				//������ر�׼ֵ wjh-2015.3.4
				stdload=load/(pStatus->GamaQ()*pStatus->Kesai());
				stdloadz2=loadz2/z2_coef;
				if(pStatus->m_cMainLoadType<4)
				{
					stdload/=m_fSafetyCoef;
					stdloadz2/=m_fSafetyCoef;
				}
				if(pNode)
				{
					CExternalNodeLoad *pNodeLoad=pStatus->hashNodeLoad.GetValue(pNode->handle);
					if(pNodeLoad==NULL)
					{
						CExternalNodeLoad nodeLoad;
						nodeLoad.designLoad=load+GEPOINT(0,0,loadz2);	//�ܺ������ֵ
						nodeLoad.Fxyz=stdload;				//�ɱ���ر�׼ֵ
						nodeLoad.permanentFz = stdloadz2;	//���ú��ر�׼ֵ
						pStatus->hashNodeLoad.SetValue(pNode->handle,nodeLoad);
					}
					else
					{
						pNodeLoad->designLoad=load+GEPOINT(0,0,loadz2);	//�ܺ������ֵ
						pNodeLoad->Fxyz=stdload;				//�ɱ���ر�׼ֵ
						pNodeLoad->permanentFz = stdloadz2;	//���ú��ر�׼ֵ
					}
				}
			}
		}
	}
	fclose(fp);
	return TRUE;
}
//the following line is from PostProcess.cpp
typedef CLDSNode* NODE_PTR;
static int compare_func1(const NODE_PTR& pNode1,const NODE_PTR& pNode2)
{	return ((CLDSNode*)pNode1)->point_i-((CLDSNode*)pNode2)->point_i; }
BOOL CTower::ExportLoadFile(char *file,double z2_coef)
{
	FILE *fp=fopen(file,"wt");
	if(fp==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Can't open loading file specified!");
#else
		AfxMessageBox("�򲻿�ָ���ĺ����ļ�!");
#endif
		return FALSE;
	}

	fprintf(fp,"%d,%d\n",WorkStatus.GetNodeNum(),GetLoadNodeNum());
	fprintf(fp,"\n");
	CWorkStatus *pStatus;
	for(pStatus=WorkStatus.GetFirst();pStatus;pStatus=WorkStatus.GetNext())
		fprintf(fp,"%s\n",pStatus->limit_str);
	fprintf(fp,"\n");
	for(pStatus=WorkStatus.GetFirst();pStatus;pStatus=WorkStatus.GetNext())
		fprintf(fp,"%5.2f,%2d,%.2f,%s\n",pStatus->WindLoad.velocity,pStatus->WindLoad.azimuth,pStatus->Kesai(),pStatus->description);
	fprintf(fp,"\n");
	//..���к��ؽڵ�ĺ������
	long nNodeNum=Node.GetNodeNum();
	NODE_PTR *node_arr=new NODE_PTR[nNodeNum];
	int i=0;
	for(CLDSNode *pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		node_arr[i]=pNode;
		i++;
	}
	CQuickSort<NODE_PTR>::QuickSort(node_arr,nNodeNum,compare_func1);
	for(i=0;i<nNodeNum;i++)
	{
		CLDSNode *pNode=((CLDSNode**)node_arr)[i];
		bool bLoadNode=false;
		for(pStatus=WorkStatus.GetFirst();pStatus;pStatus=WorkStatus.GetNext())
		{
			if(pStatus->hashNodeLoad.GetValue(pNode->handle))
			{
				bLoadNode=true;
				break;
			}
		}
		if(bLoadNode)
		{
			bool bFirstStatus=true;
			CExternalNodeLoad zeroNodeLoad;
			for(pStatus=WorkStatus.GetFirst();pStatus;pStatus=WorkStatus.GetNext())
			{
				CExternalNodeLoad *pNodeLoad=pStatus->hashNodeLoad.GetValue(pNode->handle);
				if(pNodeLoad==NULL)
					pNodeLoad=&zeroNodeLoad;
				double permanentFz=0;
				f3dPoint Fxyz;
				if(pStatus->m_bLoadIncFactor)
					Fxyz=pNodeLoad->designLoad/1000;
				else
				{
					Fxyz=pNodeLoad->Fxyz*pStatus->GamaQ()*pStatus->Kesai()/1000;	//N->kN
					permanentFz=pNodeLoad->permanentFz*z2_coef/1000;					//N->kN
					if(pStatus->m_cMainLoadType!=4)
					{
						Fxyz*=m_fSafetyCoef;
						permanentFz*=m_fSafetyCoef;
					}
				}
				if(bFirstStatus)
				{
					if(pNodeLoad)
						fprintf(fp,"%6d, %.3f, %.3f, %.3f, %.3f\n",pNode->point_i,Fxyz.x,Fxyz.y,Fxyz.z,permanentFz);
					else
						fprintf(fp,"%6d, 0.0, 0.0, 0.0, 0.0\n");
					bFirstStatus=false;
				}
				else
				{
					if(pNodeLoad)
						fprintf(fp,"        %.3f, %.3f, %.3f, %.3f\n",Fxyz.x,Fxyz.y,Fxyz.z,permanentFz);
					else
						fprintf(fp,"        0.0, 0.0, 0.0, 0.0\n");
				}
			}
		}
	}
	delete []node_arr;
	fprintf(fp,"\n");
	fclose(fp);
	return TRUE;
}
int CTower::GetLoadNodeNum()
{
	int nHits=0;
	for(CLDSNode *pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		for(CWorkStatus *pStatus=WorkStatus.GetFirst();pStatus;pStatus=WorkStatus.GetNext())
		{
			if(pStatus->hashNodeLoad.GetValue(pNode->handle))
			{
				nHits++;
				break;
			}
		}
	}
	return nHits;
}

UINT CTower::GetNewStatNo()
{
	CObjNoGroup group(1000000);
	Parts.push_stack();
	for(CLDSLinePart *pLinePart= Parts.GetFirstLinePart();pLinePart;pLinePart=Parts.GetNextLinePart())
	{
		if(pLinePart->m_uStatMatNo>0)
			group.SetNoState(pLinePart->m_uStatMatNo);
	}
	Parts.pop_stack();
	UINT no = (UINT)group.EnumIdleNo();
	return no;
}
double CTower::SafetyCoef()
{
	if(m_siSafetyGrade==0)
		return 1.1;
	else if(m_siSafetyGrade==1)
		return 1.0;
	else if(m_siSafetyGrade==2)
		return 0.9;
	else
		return m_fSafetyCoef;
}
#ifdef __LDS_CONTEXT_
IMemberDesignCode* CTower::DesignCode()	//���ص�ǰ��Ԫ�����ִ�еĹ淶/�涨
{
	return IMemberDesignCode::FromCodeSerial(m_iMemberSizeCheckCode);
}
#endif
CWorkStatus *CTower::GetActiveWorkStatus()
{
	CWorkStatus *pStatus=WorkStatus.FromHandle(m_hActiveStatus);
	if(pStatus==NULL&&WorkStatus.GetNodeNum()>0)
	{
		pStatus=WorkStatus.GetFirst();
		m_hActiveStatus=pStatus->handle;
	}
	return pStatus;
}

void CTower::AppendGeneralStatus()
{
	/* ���ں��ع��߷���������������WireLoad.exe�У����´�����������
	CObjNoGroup *pGroup=NoManager.FromGroupID(WIRELAYOUT_GROUP);
	CWireLayout *pWireLay=groupWireLayout.append();
	strcpy(pWireLay->name,"���÷���");
	pWireLay->iNo=pGroup->EnumIdleNo();
	pGroup->SetNoState(pWireLay->iNo);
	*/
	//��������
	CObjNoGroup *pGroup=NoManager.FromGroupID(WORKSTATUS_GROUP);
	CWorkStatus *pStatus=WorkStatus.append();
#ifdef AFX_TARG_ENU_ENGLISH
	sprintf(pStatus->description,"Normal-Max wind speed,Ice-free,Without broken line");
#else
	sprintf(pStatus->description,"����-�����١��ޱ���δ����");
#endif
	pStatus->iNo=pGroup->EnumIdleNo();
	pGroup->SetNoState(pStatus->iNo);
	m_hActiveStatus=pStatus->handle;
}
//angleΪ�Ƕ���(��)
void GetWireWindLoadAllocCoef(double angle,double *coefX,double *coefY)
{
	double angle_rad=angle*RADTODEG_COEF;
	double sina=sin(angle_rad);
	double cosa=cos(angle_rad);
	if(coefX)
		*coefX=sina*sina;
	if(coefY)
	{
		*coefY=0.25*cosa*cosa+0.05*sina*cosa;
		if(*coefY<0.1)
			*coefY=0;	//�������ϵ����С��ͬ�淶һ�£��S�Բ��ƣ���57�����Ϸ紵,
	}
}
/* �ü��㹦����������������WireLoad.exe��
void CTower::SynchronizeWorkStatus(CWireLayout *pWireLayout)
{
	//��ԭʼ���ؽڵ�ĵ����ʵ�ʽڵ���ͬ������
	CLDSNode *pNode;
	CHashTable<CLDSNode*>hashNodeI;
	hashNodeI.CreateHashTable(Node.GetNodeNum());
	for(pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
		hashNodeI.SetValueAt(pNode->point_i,pNode);
	for(LOADNODEROOM_STRU *pRoom=pWireLayout->groupWorkStatus.hashNodeRoom.GetFirst();pRoom;pRoom=pWireLayout->groupWorkStatus.hashNodeRoom.GetNext())
	{
		for(WIRENODE_STRU *pRawNode=pRoom->hashNode.GetFirst();pRawNode;pRawNode=pRoom->hashNode.GetNext())
		{
			if(hashNodeI.GetValueAt(pRoom->hashNode.GetCursorKey(),pNode))
				pRawNode->hNode=pNode->handle;
			else
				pRawNode->hNode=0;
		}
	}

	//��ԭʼ������ʵ�ʹ���ͬ������
	CWorkStatus *pStatus;
	RAWWORKSTATUS_STRU *pRawStatus;
	CObjNoGroup *pWorkStatusGroup=NoManager.FromGroupID(WORKSTATUS_GROUP);
	CHashTable<CWorkStatus*>hashWorkStatus;
	hashWorkStatus.CreateHashTable(WorkStatus.GetNodeNum());
	for(pStatus=WorkStatus.GetFirst();pStatus;pStatus=WorkStatus.GetNext())
	{
		if(pStatus->iWireLayoutNo!=pWireLayout->iNo)
			pStatus->feature=1;
		else
			pStatus->feature=0;
		hashWorkStatus.SetValueAt(pStatus->handle,pStatus);
	}
	//���˳�Ҫ������ʵ�ʹ���
	for(pRawStatus=pWireLayout->groupWorkStatus.hashWorkStatus.GetFirst();pRawStatus;pRawStatus=pWireLayout->groupWorkStatus.hashWorkStatus.GetNext())
	{
		if(pRawStatus->m_hWorkStatus>=0x20)
		{
			if(hashWorkStatus.GetValueAt(pRawStatus->m_hWorkStatus,pStatus))
				pStatus->feature=1;	//Ҫ������ʵ�ʹ���
		}
	}
	//���ԭ�ж����ʵ�ʹ���
	for(pStatus=WorkStatus.GetFirst();pStatus;pStatus=WorkStatus.GetNext())
	{
		if(pStatus->feature==0)
		{
			pWorkStatusGroup->SetNoState(pStatus->iNo,FALSE);
			WorkStatus.DeleteCursor();
		}
	}
	WorkStatus.Clean();

	for(pRawStatus=pWireLayout->groupWorkStatus.hashWorkStatus.GetFirst();pRawStatus;pRawStatus=pWireLayout->groupWorkStatus.hashWorkStatus.GetNext())
	{
		pStatus=NULL;
		if(pRawStatus->m_hWorkStatus>0x20)
			pStatus=WorkStatus.FromHandle(pRawStatus->m_hWorkStatus);
		if(pStatus==NULL)
		{
			pStatus=WorkStatus.append();
			pRawStatus->m_hWorkStatus=pStatus->handle;
			pStatus->iNo=pWorkStatusGroup->EnumIdleNo();
			pStatus->iWireLayoutNo=pWireLayout->iNo;
			pWorkStatusGroup->SetNoState(pStatus->iNo);
		}
		strcpy(pStatus->description,pRawStatus->name);
		strcpy(pStatus->limit_str,pRawStatus->lim_str);
		pStatus->WindLoad=pRawStatus->WindLoad;
		pStatus->ice_thick=pRawStatus->ice_thick;
		pStatus->m_cMainLoadType=pRawStatus->m_cMainLoadType;
		pStatus->m_bCheckWorkCond=pRawStatus->m_bCheckWorkCond;
	}
}
*/
//��*.frq�ļ��ж�ȡ�����������ͷ���ʱ���ڵ�ĵ�Ч�ڵ�����
long CTower::ReadVibraNodeMass(FILE *fp,DYNAMIC_ANAL_NODE *node_arr)
{
	if(fp==NULL)
		return 0;
	long i,version,nFreqsRank=0,nNodeNum=0;
	fseek(fp,0,SEEK_SET);
	fread(&version,sizeof(long),1,fp);
	fread(&nFreqsRank,sizeof(long),1,fp);
	fread(&nNodeNum,sizeof(long),1,fp);
	if(node_arr!=NULL)
	{
		for(i=0;i<nNodeNum;i++)
		{
			fread(&node_arr[i].id,sizeof(long),1,fp);
			fread(&node_arr[i].mass,sizeof(double),1,fp);
		}
	}
	return nNodeNum;
}

//iMode��0��ʼ��������ʾ��iMode+1������
double CTower::ReadVibraMode(FILE *fp, int iMode, bool iReadAllFreqs/*=false*/)
{
	if(fp==NULL)
		return 0;
	long i,h,version,nFreqsRank=0,nNodeNum=0,seek_offset=0;
	fseek(fp,0,SEEK_SET);
	fread(&version,sizeof(long),1,fp);
	fread(&nFreqsRank,sizeof(long),1,fp);
	fread(&nNodeNum,sizeof(long),1,fp);
	EarthQuake.nFreqsRank = nFreqsRank;
	if(iMode<0||iMode>=nFreqsRank)
		return 0;
	long head_final=12+(4+8)*nNodeNum;	//12+(handle+mass)*nNodeNum
	if(iReadAllFreqs)
	{
		for(i=0;i<nFreqsRank;i++)
		{
			seek_offset=head_final+i*(24+nNodeNum*28);//(sizeof(double)*3+nNodeNum*(sizeof(long)+sizeof(double)*3)
			fseek(fp,seek_offset,SEEK_SET);
			fread(&EarthQuake.modes_arr[i].freqs,sizeof(double),1,fp);	//�ý����͵�����Ƶ��
			fread(&EarthQuake.modes_arr[i].gama_x,sizeof(double),1,fp);	//�ý�����X�����Ͳ�Ԥϵ��
			fread(&EarthQuake.modes_arr[i].gama_y,sizeof(double),1,fp);	//�ý�����Y�����Ͳ�Ԥϵ��
		}
	}
	seek_offset=head_final+iMode*(24+nNodeNum*28);//head_final+(sizeof(double)*3+nNodeNum*(sizeof(long)+sizeof(double)*3)
	fseek(fp,seek_offset,SEEK_SET);
	double frequency = 0, gama_x = 0, gama_y = 0;
	fread(&frequency, 8, 1, fp);	//��ǰ���͵�����Ƶ��
	fread(&gama_x, 8, 1, fp);		//����ʱX�����Ͳ�Ԥϵ��
	fread(&gama_y, 8, 1, fp);		//����ʱY�����Ͳ�Ԥϵ��
	f3dPoint offset;
	for(i=0;i<nNodeNum;i++)
	{
		fread(&h,sizeof(long),1,fp);
		fread(&offset.x,sizeof(double),1,fp);
		fread(&offset.y,sizeof(double),1,fp);
		fread(&offset.z,sizeof(double),1,fp);
		CLDSNode *pNode=Node.FromHandle(h);
		if(pNode)
			pNode->m_offsetAmplitude=offset;
	}
	return frequency;
}
#endif
bool CTower::IsPrevDeflateVersion(long version,long doc_type)
{
/*#if defined(__TMA_FILE_)||defined(__TMA_)
	if(doc_type==1&&version>0&&version<4000027)//compareVersion(version,"4.0.0.27")<0)
#elif defined(__LMA_FILE_)||defined(__LMA_)
	if(version>0&&version<1000082)//compareVersion(version,"1.0.0.82")<0)
#elif defined(__TDA_FILE_)
	if(false)
#else
	if(version>0&&version<1000009)//compareVersion(version,"1.0.0.09")<0)
#endif*/
	if( (doc_type==1&&version>0&&version<4000027)||
		(doc_type==2&&version>0&&version<1000082)||
		(doc_type>2&&doc_type<5&&version>0&&version<1000009))
		return true;
	else
		return false;
}
//��ȡ����ģ�Ϳռ���ֱ�ӹ���ʵ�ʴ��ڵĹ������飬filterShadowPart=true��ʾֻ����ĸ��������Ӱ�乹�������S��
bool CTower::GetRealityPartArr(DYN_ARRAY<CLDSBolt*>& arrBoltPtr,DYN_ARRAY<CLDSPart*>& arrPartPtr,void *pPolyPartList,bool filterShadowPart)
{
	CLDSPart* pPart;
	PARTSET partset,boltset;
	for(pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		if(pPart->m_bVirtualPart)
			continue;	//���⸨����������ʾ
		if(filterShadowPart&&pPart->IsShadowObject())
			continue;	//�S��Ӱ�乹��
		if(pPart->GetClassTypeId()==CLS_BOLT)
			boltset.append(pPart);
		else if(pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE)
			partset.append(pPart);
		else if(pPart->GetClassTypeId()==CLS_LINETUBE||
			pPart->GetClassTypeId()==CLS_LINEFLAT||pPart->GetClassTypeId()==CLS_LINESLOT)
			partset.append(pPart);
		else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			if(pPolyPartList==NULL||(!((CLDSLineAngle*)pPart)->bHuoQuStart&&!((CLDSLineAngle*)pPart)->bHuoQuEnd))
				partset.append(pPart);
		}
	}
	if(pPolyPartList)
	{
		for(CLDSPolyPart *pPolyPart=((POLYPARTLIST*)pPolyPartList)->GetFirst();pPolyPart;pPolyPart=((POLYPARTLIST*)pPolyPartList)->GetNext())
		{
			if(pPolyPart->m_bVirtualPart)
				continue;	//���⸨����������ʾ
			if(filterShadowPart&&pPolyPart->IsShadowObject())
				continue;	//�S��Ӱ�乹��
			partset.append(pPolyPart);
		}
	}
	int i=0;
	arrPartPtr.Resize(partset.GetNodeNum());
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext(),i++)
		arrPartPtr[i]=pPart;
	i=0;
	arrBoltPtr.Resize(boltset.GetNodeNum());
	for(pPart=boltset.GetFirst();pPart;pPart=boltset.GetNext(),i++)
		arrBoltPtr[i]=(CLDSBolt*)pPart;
	return (partset.GetNodeNum()+boltset.GetNodeNum())>0;
}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
void IModel::SmartCalAngleWingNorm(CLDSLinePart *pRod)
{
	if(!pRod->IsAngle(FALSE))
		return;
	CLDSLineAngle* pLineAngle=(CLDSLineAngle*)pRod;
	if(pLineAngle->m_bSpecAngleWingNorm)
	{
		if(pLineAngle->des_wing_norm.wLayoutStyle==0||pLineAngle->des_wing_norm.wLayoutStyle==2)
		{	//X֫Ϊ����֫������\����
			if(pLineAngle->des_wing_norm.wLayoutStyle==0)	//����
				pLineAngle->des_norm_x.near_norm=-pLineAngle->des_wing_norm.viewNorm;
			else											//����
				pLineAngle->des_norm_x.near_norm=pLineAngle->des_wing_norm.viewNorm;
			if(pLineAngle->des_wing_norm.hRefPoleArr[0]>0x20)
				pLineAngle->des_norm_x.hViceJg=pLineAngle->des_wing_norm.hRefPoleArr[0];
			else if(pLineAngle->pStart->hFatherPart!=pLineAngle->handle)
				pLineAngle->des_norm_x.hViceJg=pLineAngle->pStart->hFatherPart;
			else if(pLineAngle->pEnd->hFatherPart!=pLineAngle->handle)
				pLineAngle->des_norm_x.hViceJg=pLineAngle->pEnd->hFatherPart;
			else
			{
				pLineAngle->des_norm_x.bSpecific=TRUE;
				pLineAngle->des_norm_x.spec_norm.nearVector=pLineAngle->des_norm_x.near_norm;
			}
			pLineAngle->cal_x_wing_norm();
			pLineAngle->des_norm_y.bByOtherWing=TRUE;
			pLineAngle->cal_y_wing_norm();
			if(pLineAngle->des_wing_norm.hRefPoleArr[1]>0x20)
			{
				pLineAngle->des_norm_y.hViceJg=pLineAngle->des_wing_norm.hRefPoleArr[1];
				pLineAngle->des_norm_y.near_norm=pLineAngle->get_norm_y_wing();
				pLineAngle->cal_y_wing_norm();
			}
			return;
		}
		else if(pLineAngle->des_wing_norm.wLayoutStyle==1||pLineAngle->des_wing_norm.wLayoutStyle==3)
		{	//Y֫Ϊ����֫������/����
			if(pLineAngle->des_wing_norm.wLayoutStyle==1)	//����
				pLineAngle->des_norm_y.near_norm=pLineAngle->des_wing_norm.viewNorm;
			else											//����
				pLineAngle->des_norm_y.near_norm=-pLineAngle->des_wing_norm.viewNorm;
			if(pLineAngle->des_wing_norm.hRefPoleArr[0]>0x20)
				pLineAngle->des_norm_y.hViceJg=pLineAngle->des_wing_norm.hRefPoleArr[0];
			else if(pLineAngle->pStart->hFatherPart!=pLineAngle->handle)
				pLineAngle->des_norm_y.hViceJg=pLineAngle->pStart->hFatherPart;
			else if(pLineAngle->pEnd->hFatherPart!=pLineAngle->handle)
				pLineAngle->des_norm_y.hViceJg=pLineAngle->pEnd->hFatherPart;
			else
			{
				pLineAngle->des_norm_y.bSpecific=TRUE;
				pLineAngle->des_norm_y.spec_norm.nearVector=pLineAngle->des_norm_y.near_norm;
			}
			pLineAngle->cal_y_wing_norm();
			pLineAngle->des_norm_x.bByOtherWing=TRUE;
			pLineAngle->cal_x_wing_norm();
			if(pLineAngle->des_wing_norm.hRefPoleArr[1]>0x20)
			{
				pLineAngle->des_norm_x.hViceJg=pLineAngle->des_wing_norm.hRefPoleArr[1];
				pLineAngle->des_norm_x.near_norm=pLineAngle->get_norm_x_wing();
				pLineAngle->cal_x_wing_norm();
			}
			return;
		}
	}
	CSuperSmartPtr<CLDSPart>pRefPart;
	if(pLineAngle->layer(1)=='Z')
	{	//����
		if(pLineAngle->layer(0)=='T')
		{	//ͷ���ᵣ����
			pLineAngle->des_norm_x.bSpecific=TRUE;
			pLineAngle->des_norm_x.spec_norm.nearVector.Set(0,1,0);
			pLineAngle->des_norm_y.bByOtherWing=TRUE;
		}
		else //if(pLineAngle->layer(0)=='S')
		{	//������Ȳ�����
			if(pLineAngle->layer(2)=='1'||pLineAngle->layer(2)=='4')
			{
				pRefPart=pLineAngle->GetMirPart(MIRMSG(2));
				if(pRefPart.IsHasPtr()&&pRefPart->IsLinePart())
				{
					pLineAngle->des_norm_x.hViceJg=pRefPart->handle;
					pLineAngle->des_norm_x.near_norm.Set(0,1,0);
				}
				else
				{
					pLineAngle->des_norm_x.spec_norm.nearVector.Set(0,1,0);
					pLineAngle->des_norm_x.bSpecific=TRUE;
				}
				pRefPart=pLineAngle->GetMirPart(MIRMSG(1));
				if(pRefPart.IsHasPtr()&&pRefPart->IsLinePart())
				{
					pLineAngle->des_norm_y.hViceJg=pRefPart->handle;
					pLineAngle->des_norm_y.near_norm.Set(1,0,0);
				}
				else
				{
					pLineAngle->des_norm_y.spec_norm.nearVector.Set(1,0,0);
					pLineAngle->des_norm_y.bSpecific=TRUE;
				}
				if(pLineAngle->layer(2)=='4')
				{
					pLineAngle->des_norm_x.near_norm*=-1.0;
					pLineAngle->des_norm_x.spec_norm.nearVector*=-1.0;
					pLineAngle->des_norm_y.near_norm*=-1.0;
					pLineAngle->des_norm_y.spec_norm.nearVector*=-1.0;
				}
			}
			else if(pLineAngle->layer(2)=='2'||pLineAngle->layer(2)=='3')
			{
				pRefPart=pLineAngle->GetMirPart(MIRMSG(1));
				if(pRefPart.IsHasPtr()&&pRefPart->IsLinePart())
				{
					pLineAngle->des_norm_x.hViceJg=pRefPart->handle;
					pLineAngle->des_norm_x.near_norm.Set(-1,0,0);
				}
				else
				{
					pLineAngle->des_norm_x.spec_norm.nearVector.Set(-1,0,0);
					pLineAngle->des_norm_x.bSpecific=TRUE;
				}
				pRefPart=pLineAngle->GetMirPart(MIRMSG(2));
				if(pRefPart.IsHasPtr()&&pRefPart->IsLinePart())
				{
					pLineAngle->des_norm_y.hViceJg=pRefPart->handle;
					pLineAngle->des_norm_y.near_norm.Set(0,1,0);
				}
				else
				{
					pLineAngle->des_norm_y.spec_norm.nearVector.Set(1,0,0);
					pLineAngle->des_norm_y.bSpecific=TRUE;
				}
				if(pLineAngle->layer(2)=='3')
				{
					pLineAngle->des_norm_x.near_norm*=-1.0;
					pLineAngle->des_norm_x.spec_norm.nearVector*=-1.0;
					pLineAngle->des_norm_y.near_norm*=-1.0;
					pLineAngle->des_norm_y.spec_norm.nearVector*=-1.0;
				}
			}
		}
	}
	else if(pLineAngle->layer(1)=='X')
	{	//б��
		f3dPoint start,end;
		if(pLineAngle->pStart->Position(false).z<=pLineAngle->pEnd->Position(false).z)
		{
			start=pLineAngle->pStart->Position(false);
			end=pLineAngle->pEnd->Position(false);
		}
		else
		{
			start=pLineAngle->pEnd->Position(false);
			end=pLineAngle->pStart->Position(false);
		}
		if(pLineAngle->layer(0)=='T')
		{	//ͷ��б��
			;
		}
		else if(pLineAngle->layer(0)=='S')
		{	//����б��
			pLineAngle->des_norm_x.hViceJg=pLineAngle->pStart->hFatherPart;
			if(pLineAngle->layer(2)=='Q'||pLineAngle->layer(2)=='H')
			{
				if(start.x>end.x)
					pLineAngle->des_norm_x.near_norm.Set(0,-1,0);
				else
					pLineAngle->des_norm_x.near_norm.Set(0,1,0);
			}
			else if(pLineAngle->layer(2)=='Z'||pLineAngle->layer(2)=='Y')
			{
				if(start.y<end.y)
					pLineAngle->des_norm_x.near_norm.Set(-1,0,0);
				else
					pLineAngle->des_norm_x.near_norm.Set(1,0,0);
			}
			pLineAngle->des_norm_y.bByOtherWing=TRUE;
		}
		else
		{	//�Ȳ�б��
			if(pLineAngle->layer(2)=='Q')
			{
				if(start.x>end.x)	//Ʋ
				{
					pLineAngle->des_norm_x.near_norm.Set(0,1,0);
					pLineAngle->des_norm_x.hViceJg=pLineAngle->pStart->hFatherPart;
					pLineAngle->des_norm_y.bByOtherWing=TRUE;
				}
				else
				{
					pLineAngle->des_norm_y.near_norm.Set(0,1,0);
					pLineAngle->des_norm_y.hViceJg=pLineAngle->pStart->hFatherPart;
					pLineAngle->des_norm_x.bByOtherWing=TRUE;
				}
			}
			else if(pLineAngle->layer(2)=='H')
			{
				if(start.x<end.x)	//Ʋ
				{
					pLineAngle->des_norm_x.near_norm.Set(0,-1,0);
					pLineAngle->des_norm_x.hViceJg=pLineAngle->pStart->hFatherPart;
					pLineAngle->des_norm_y.bByOtherWing=TRUE;
				}
				else
				{
					pLineAngle->des_norm_y.near_norm.Set(0,-1,0);
					pLineAngle->des_norm_y.hViceJg=pLineAngle->pStart->hFatherPart;
					pLineAngle->des_norm_x.bByOtherWing=TRUE;
				}
			}
			else if(pLineAngle->layer(2)=='Z')
			{
				if(start.y>end.y)	//Ʋ
				{
					pLineAngle->des_norm_x.near_norm.Set(-1,0,0);
					pLineAngle->des_norm_x.hViceJg=pLineAngle->pStart->hFatherPart;
					pLineAngle->des_norm_y.bByOtherWing=TRUE;
				}
				else
				{
					pLineAngle->des_norm_y.near_norm.Set(-1,0,0);
					pLineAngle->des_norm_y.hViceJg=pLineAngle->pStart->hFatherPart;
					pLineAngle->des_norm_x.bByOtherWing=TRUE;
				}
			}
			else if(pLineAngle->layer(2)=='Y')
			{
				if(start.y<end.y)	//Ʋ
				{
					pLineAngle->des_norm_x.near_norm.Set(1,0,0);
					pLineAngle->des_norm_x.hViceJg=pLineAngle->pStart->hFatherPart;
					pLineAngle->des_norm_y.bByOtherWing=TRUE;
				}
				else
				{
					pLineAngle->des_norm_y.near_norm.Set(1,0,0);
					pLineAngle->des_norm_y.hViceJg=pLineAngle->pStart->hFatherPart;
					pLineAngle->des_norm_x.bByOtherWing=TRUE;
				}
			}
		}
	}
	else if(pLineAngle->layer(1)=='H')
	{	//���
		f3dPoint start,end;
		start=pLineAngle->pStart->Position(false);
		end=pLineAngle->pEnd->Position(false);
		//����б��
		pLineAngle->des_norm_x.hViceJg=pLineAngle->pStart->hFatherPart;
		if(pLineAngle->layer(2)=='Q')
		{
			if(start.x>end.x)
			{
				pLineAngle->des_norm_x.bByOtherWing=TRUE;
				pLineAngle->des_norm_y.near_norm.Set(0,1,0);
				pLineAngle->des_norm_y.hViceJg=pLineAngle->pStart->hFatherPart;
			}
			else
			{
				pLineAngle->des_norm_y.bByOtherWing=TRUE;
				pLineAngle->des_norm_x.near_norm.Set(0,1,0);
				pLineAngle->des_norm_x.hViceJg=pLineAngle->pStart->hFatherPart;
			}
		}
		else if(pLineAngle->layer(2)=='H')
		{
			if(start.x<end.x)
			{
				pLineAngle->des_norm_x.bByOtherWing=TRUE;
				pLineAngle->des_norm_y.near_norm.Set(0,-1,0);
				pLineAngle->des_norm_y.hViceJg=pLineAngle->pStart->hFatherPart;
			}
			else
			{
				pLineAngle->des_norm_y.bByOtherWing=TRUE;
				pLineAngle->des_norm_x.near_norm.Set(0,-1,0);
				pLineAngle->des_norm_x.hViceJg=pLineAngle->pStart->hFatherPart;
			}
		}
		else if(pLineAngle->layer(2)=='Z')
		{
			if(start.y>end.y)
			{
				pLineAngle->des_norm_x.bByOtherWing=TRUE;
				pLineAngle->des_norm_y.near_norm.Set(-1,0,0);
				pLineAngle->des_norm_y.hViceJg=pLineAngle->pStart->hFatherPart;
			}
			else
			{
				pLineAngle->des_norm_y.bByOtherWing=TRUE;
				pLineAngle->des_norm_x.near_norm.Set(-1,0,0);
				pLineAngle->des_norm_x.hViceJg=pLineAngle->pStart->hFatherPart;
			}
		}
		else// if(pLineAngle->layer(2)=='Y')
		{
			if(start.y<end.y)
			{
				pLineAngle->des_norm_x.bByOtherWing=TRUE;
				pLineAngle->des_norm_y.near_norm.Set(1,0,0);
				pLineAngle->des_norm_y.hViceJg=pLineAngle->pStart->hFatherPart;
			}
			else
			{
				pLineAngle->des_norm_y.bByOtherWing=TRUE;
				pLineAngle->des_norm_x.near_norm.Set(1,0,0);
				pLineAngle->des_norm_x.hViceJg=pLineAngle->pStart->hFatherPart;
			}
		}
	}
	else if(pLineAngle->layer(1)=='F')
	{
		pLineAngle->des_norm_x.hViceJg=pLineAngle->pStart->hFatherPart;
		pLineAngle->des_norm_y.bByOtherWing=TRUE;
		if(pLineAngle->layer(0)=='T')
		{
		}
		else if(pLineAngle->layer(0)=='S')
		{	//������
		}
		else //if(pLineAngle->layer(0)=='L')
		{
			
		}
	}
	bool colinear=false;
	GEPOINT lenStdVec,lenRefStdVec;
	if(!pLineAngle->des_norm_x.IsSpecNormActual()&&pLineAngle->des_norm_x.hViceJg==pLineAngle->handle)
		colinear=true;
	else
	{
		lenStdVec=pLineAngle->End()-pLineAngle->Start();
		CLDSLinePart* pRefRod=FromRodHandle(pLineAngle->des_norm_x.hViceJg);
		if(pRefRod)
			lenRefStdVec=pRefRod->End()-pRefRod->Start();
		normalize(lenStdVec);
		normalize(lenRefStdVec);
		if(fabs(lenStdVec*lenRefStdVec)>EPS_COS2)
			colinear=true;
	}
	if(colinear)
	{
		pLineAngle->des_norm_x.bSpecific=TRUE;
		pLineAngle->set_norm_x_wing(CalAxisYZByX(pLineAngle->End()-pLineAngle->Start(),1),TRUE);
	}
	colinear=false;
	if(!pLineAngle->des_norm_y.IsSpecNormActual()&&pLineAngle->des_norm_y.hViceJg==pLineAngle->handle)
		colinear=true;
	else
	{
		if(lenStdVec.IsZero())
		{
			lenStdVec=pLineAngle->End()-pLineAngle->Start();
			normalize(lenStdVec);
		}
		CLDSLinePart* pRefRod=FromRodHandle(pLineAngle->des_norm_x.hViceJg);
		if(pRefRod)
			lenRefStdVec=pRefRod->End()-pRefRod->Start();
		normalize(lenRefStdVec);
		if(fabs(lenStdVec*lenRefStdVec)>EPS_COS2)
			colinear=true;
	}
	if(colinear)
	{
		pLineAngle->des_norm_y.bByOtherWing=TRUE;
		pLineAngle->set_norm_y_wing(CalAxisYZByX(pLineAngle->End()-pLineAngle->Start(),0),false);
	}
	pLineAngle->cal_x_wing_norm();
	pLineAngle->cal_y_wing_norm();
}

void CTower::CorrectErrorBefore(long nVersion,long doc_type,BOOL bHasRepeatHandle)
{
	//�������ļ��д���Ľڵ�Գƹ�����Ϣ wjh-2011.8.15
	for(CLDSNode *pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
		pNode->ClearErrorRelativeObjs();
	//�������ļ��д��ڵľ���ظ�
	if(bHasRepeatHandle)
	{
		for(CLDSPart *pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
		{
			BOOL pushed=Parts.push_stack();
			CLDSObject *pObj=NULL;
			if((pObj=hashObjs.GetValue(pPart->handle))!=NULL)
			{
				if(pPart->GetClassTypeId()!=pObj->GetClassTypeId())
				{
					long h=GetNewHandle();
					ReplaceReferenceHandle(CHandleReplacer(pPart->handle,h),pPart->GetClassTypeId());
					pPart->handle=h;
				}
			}
			Parts.pop_stack(pushed);
		}
	}
#ifdef __COMPLEX_PART_INC_
	if(doc_type==2&&nVersion<=1000080)//compareVersion(version,"1.0.0.80")<=0)
	{
		for(CLDSPlate *pPlate=(CLDSPlate*)Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Parts.GetNext(CLS_PLATE))
		{
			BOOL pushed=Parts.push_stack();
			CSuperSmartPtr<CLDSPart>pBaseLinePart=Parts.FromHandle(pPlate->designInfo.m_hBasePart);
			if(pBaseLinePart.IsHasPtr()&&pBaseLinePart->IsLinePart()&&
				pBaseLinePart.LinePartPointer()->pStart&&pBaseLinePart.LinePartPointer()->pEnd)
			{
				f3dLine baseline;
				baseline.startPt=pBaseLinePart.LinePartPointer()->pStart->Position(false);
				baseline.endPt=pBaseLinePart.LinePartPointer()->pEnd->Position(false);
				for(CDesignLjPartPara *pLjPart=pPlate->designInfo.partList.GetFirst();pLjPart;
				pLjPart=pPlate->designInfo.partList.GetNext())
				{
					CSuperSmartPtr<CLDSPart>pLinePart=Parts.FromHandle(pLjPart->hPart);
					if(pLinePart.IsNULL()||!pLinePart->IsLinePart())
						continue;
					if(pLinePart.LinePartPointer()->pStart==NULL||pLinePart.LinePartPointer()->pEnd==NULL)
						continue;
					if(pLjPart->start0_end1==0)
					{
						if(pPlate->designInfo.m_hBaseNode==pLinePart.LinePartPointer()->pStart->handle)
							continue;
						if(baseline.PtInLine(pLinePart.LinePartPointer()->pStart->Position(false)))
							continue;
						f3dPoint rayLineVec=pLinePart.LinePartPointer()->pEnd->Position(false)-pLinePart.LinePartPointer()->pStart->Position(false);
						normalize(rayLineVec);
						CSuperSmartPtr<CLDSLinePart>pRayPart;
						for(pRayPart=(CLDSLinePart*)Parts.GetFirst(pLinePart->GetClassTypeId());pRayPart.IsHasPtr();
						pRayPart=(CLDSLinePart*)Parts.GetNext(pLinePart->GetClassTypeId()))
						{
							if(pRayPart->pStart==NULL||pRayPart->pEnd==NULL)
								continue;
							f3dPoint lineVec=pRayPart->pEnd->Position(false)-pRayPart->pStart->Position(false);
							normalize(lineVec);
							if(fabs(lineVec*rayLineVec)>EPS_COS)	//����
							{
								if(pRayPart->pStart->handle==pPlate->designInfo.m_hBaseNode)
								{
									pLjPart->start0_end1=0;
									pLjPart->hPart=pRayPart->handle;
									break;
								}
								else if(pRayPart->pEnd->handle==pPlate->designInfo.m_hBaseNode)
								{
									pLjPart->start0_end1=1;
									pLjPart->hPart=pRayPart->handle;
									break;
								}
							}
						}
					}
					else if(pLjPart->start0_end1==1)
					{
						if(pPlate->designInfo.m_hBaseNode==pLinePart.LinePartPointer()->pEnd->handle)
							continue;
						if(baseline.PtInLine(pLinePart.LinePartPointer()->pEnd->Position(false)))
							continue;
						f3dPoint rayLineVec=pLinePart.LinePartPointer()->pEnd->Position(false)-pLinePart.LinePartPointer()->pStart->Position(false);
						normalize(rayLineVec);
						CSuperSmartPtr<CLDSLinePart>pRayPart;
						for(pRayPart=(CLDSLinePart*)Parts.GetFirst(pLinePart->GetClassTypeId());pRayPart.IsHasPtr();
						pRayPart=(CLDSLinePart*)Parts.GetNext(pLinePart->GetClassTypeId()))
						{
							if(pRayPart->pStart==NULL||pRayPart->pEnd==NULL)
								continue;
							f3dPoint lineVec=pRayPart->pEnd->Position(false)-pRayPart->pStart->Position(false);
							normalize(lineVec);
							if(fabs(lineVec*rayLineVec)>EPS_COS)	//����
							{
								if(pRayPart->pEnd->handle==pPlate->designInfo.m_hBaseNode)
								{
									pLjPart->start0_end1=1;
									pLjPart->hPart=pRayPart->handle;
									break;
								}
								else if(pRayPart->pStart->handle==pPlate->designInfo.m_hBaseNode)
								{
									pLjPart->start0_end1=0;
									pLjPart->hPart=pRayPart->handle;
									break;
								}
							}
						}
					}
				}
			}
			Parts.pop_stack(pushed);
		}
	}
	//������ͷ�����ӵ�
	if (doc_type==2&&nVersion<=1000085)
	{
		for(CLDSPlate *pPlate=(CLDSPlate*)Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Parts.GetNext(CLS_PLATE))
		{
			if (pPlate->face_N != 1                     //ֻ�е��������ƴ��ַ�ͷ�� 
				||pPlate->vertex_list.GetNodeNum() != 6 //���ݼӵ㷽ʽ�����ְ�һ����6������
				||pPlate->m_fInnerRadius >0)             //���ְ���Բһ��Ϊ0
				continue;
			int j = 0;
			for (PROFILE_VERTEX *pVertex = pPlate->vertex_list.GetFirst();pVertex;pVertex = pPlate->vertex_list.GetNext())
			{
				if (pVertex->type != 0 || pVertex->vertex == f3dPoint(0,0,0))
					j++;
			}
			if (j == pPlate->vertex_list.GetNodeNum()-1)
			{
				pPlate->vertex_list.GetFirst();
				pPlate->vertex_list.DeleteCursor();
				pPlate->vertex_list.GetTail();
				pPlate->vertex_list.DeleteCursor();
				pPlate->vertex_list.Clean();
			}
		}
	}
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	if((doc_type==1&&nVersion<4000028)||(doc_type==2&&nVersion<1000083))
	{
		CSuperSmartPtr<CLDSPart>pPart;
		for(pPart=Parts.GetFirst();pPart.IsHasPtr();pPart=Parts.GetNext())
		{
			Parts.push_stack();
			if(pPart->GetClassTypeId()==CLS_BOLT)
			{
				CLDSPart *pDatumPart=Parts.FromHandle(pPart.BoltPointer()->des_base_pos.hPart);
				if(pDatumPart)
					pPart->iSeg=pDatumPart->iSeg;
			}
			Parts.pop_stack();
		}
	}
	if( (doc_type==1&&nVersion>4010102&&nVersion<=5010600)||	//TMA(V4.1.1.2)
		(doc_type==2&&nVersion>1010102&&nVersion<=2000700)||	//LMA(V1.1.1.2)
		(doc_type==4&&nVersion>1020100&&nVersion<=1030700)||	//LDS(V1.2.1.1)
		(doc_type==5&&nVersion>1020100&&nVersion<=1030700))		//TDA(V1.2.1.1)
	{	//CLDSLineAngle::GetStart(End)WingDatumPosByFaceOffset��2014��5��7��(V4.1.1.3�汾)ʱ�Ĵ�,���ļ�ʱ��ʾλ�ô���Ǹ� 	wht 17-04-20
		CLogErrorLife logLife;
		for(CLDSLineAngle *pAngle=(CLDSLineAngle*)Parts.GetFirst(CLS_LINEANGLE);pAngle;pAngle=(CLDSLineAngle*)Parts.GetNext(CLS_LINEANGLE))
		{
			if(!pAngle->m_bEnableTeG)
				continue;
			CXhChar16 sEnd;
			if( pAngle->desStartPos.spatialOperationStyle==19&&
			    (((pAngle->desStartPos.wing_x_offset.gStyle==0||pAngle->desStartPos.wing_y_offset.gStyle==0)&&pAngle->xWingXZhunJu.g!=pAngle->xWingYZhunJu.g)||
				 ((pAngle->desStartPos.wing_x_offset.gStyle==1||pAngle->desStartPos.wing_y_offset.gStyle==1)&&pAngle->xWingXZhunJu.g1!=pAngle->xWingYZhunJu.g1)||
				 ((pAngle->desStartPos.wing_x_offset.gStyle==2||pAngle->desStartPos.wing_y_offset.gStyle==2)&&pAngle->xWingXZhunJu.g2!=pAngle->xWingYZhunJu.g2)||
				 ((pAngle->desStartPos.wing_x_offset.gStyle==3||pAngle->desStartPos.wing_y_offset.gStyle==3)&&pAngle->xWingXZhunJu.g3!=pAngle->xWingYZhunJu.g3)))
				 sEnd.Append("ʼ");
			if( pAngle->desEndPos.spatialOperationStyle==19&&
				(((pAngle->desEndPos.wing_x_offset.gStyle==0||pAngle->desEndPos.wing_y_offset.gStyle==0)&&pAngle->xWingXZhunJu.g!=pAngle->xWingYZhunJu.g)||
				 ((pAngle->desEndPos.wing_x_offset.gStyle==1||pAngle->desEndPos.wing_y_offset.gStyle==1)&&pAngle->xWingXZhunJu.g1!=pAngle->xWingYZhunJu.g1)||
				 ((pAngle->desEndPos.wing_x_offset.gStyle==2||pAngle->desEndPos.wing_y_offset.gStyle==2)&&pAngle->xWingXZhunJu.g2!=pAngle->xWingYZhunJu.g2)||
				 ((pAngle->desEndPos.wing_x_offset.gStyle==3||pAngle->desEndPos.wing_y_offset.gStyle==3)&&pAngle->xWingXZhunJu.g3!=pAngle->xWingYZhunJu.g3)))
				sEnd.Append("��");
			if(sEnd.GetLength()>0)
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"����֮ǰ�汾Bug,��λ��Ǹ�0x%X(%s)λ�ûᷢ���仯,���ʵ��",pAngle->handle,pAngle->Label,(char*)sEnd);
		}
	}
#endif
}
struct BOLT_CAP_PARA{
  	double D;     	  		// ��ñֱ��.
  	double H;				// ��˨ͷ�߶�.
	double H2;				// ��ñ�߶�.
};
struct LS_FAMILY_INFO{
	CLsFamily* pFamily;
	BYTE indexId;	//��1Ϊ����������Ϊ��ʶ
	CHashStrList<WORD>* pHashLsIndexBySpec;
	LS_FAMILY_INFO(){pFamily=NULL;indexId=0;pHashLsIndexBySpec=NULL;}
	~LS_FAMILY_INFO(){
		if(pHashLsIndexBySpec)
			delete pHashLsIndexBySpec;
	}
};
struct BLOCK_PARTPTR{
	DWORD m_hBlock;
	CLDSPart* m_pPart;
	BLOCK_PARTPTR(CLDSPart* pPart=NULL,DWORD hBlock=0)
	{
		m_pPart=pPart;
		m_hBlock=hBlock;
	}
};
struct BLOCK_BOLTPTR{
	DWORD m_hBlock;
	CLDSBolt* m_pBolt;
	BLOCK_BOLTPTR(CLDSBolt* pBolt=NULL,DWORD hBlock=0)
	{
		m_pBolt=pBolt;
		m_hBlock=hBlock;
	}
};
//������ͬ������ţ����������Ͼ��л����ԵĹ������ϡ�wjh-2016.1.05
struct LABEL_PART{
	CXhChar50 label;
	int idPartIndex;
	long hBlock;
	CLDSPart* pPart;	//��ͬ��Ź��������е�ĸ������
	CHashListEx<CLDSPart::PEER_PART> hashActualParts;	//���׼��������ͬ��ŵĹ�������
	CHashList<long> hashHoleIdByDatumBoltH;	//��ֵ����˨���������ֵ���ױ�ʶ����
	CHashList<long> hashBoltIndexIdByHoleId;//��ֵ���ױ�ʶ��������ֵ����Ӧ��˨���ʶ����
	LABEL_PART()
	{
		hBlock=idPartIndex=0;
		pPart=NULL;
	}
};
struct ASSEMBLE_PARTRECORD{
	CLDSPart* pAssemblePart;
	//��¼ÿ����������˨�׶�Ӧ����˨����
	CHashList<DWORD> hashBoltIndexIdByHoleId;	//��ֵΪ����ű�ʶ������Ϊ��Ӧ����˨��׼��������ʶ
};
BYTE ToNodeCoordTypeFlag(CLDSNode* pNode)
{
	BYTE cbFlag=0;	//0x01:��ָ֤��X�������; 0x02:��ָ֤��Y�������; 0x04:��ָ֤��Z�������;0x08:��Ҫ������������ƫ�ƣ���ָ���˼�ƫ��ָ�����������
	if(pNode->m_cPosCalType==0)
		return cbFlag=0x07;
	if(pNode->m_cPosCalType==1)
		cbFlag=0x01;
	else if(pNode->m_cPosCalType==2)
		cbFlag=0x02;
	else if(pNode->m_cPosCalType==3)
		cbFlag=0x04;
	else if(pNode->m_cPosCalType==6||pNode->m_cPosCalType==7)
	{
		if(pNode->m_cPosCalViceType=='X')
			cbFlag=0x01;
		else if(pNode->m_cPosCalViceType=='Y')
			cbFlag=0x02;
		else if(pNode->m_cPosCalViceType=='Z')
			cbFlag=0x04;
		if(pNode->m_cPosCalType==7&&cbFlag>0)
			cbFlag|=0x08;
	}
	else if(pNode->m_cPosCalType==8)
		cbFlag=0x03;	//XY
	else if(pNode->m_cPosCalType==9)
		cbFlag=0x06;	//YZ
	else if(pNode->m_cPosCalType==10)
		cbFlag=0x05;	//XZ
	if(cbFlag==0x07)
	{
		CLDSLinePart* pFatherRod=pNode->BelongModel()->FromRodHandle(pNode->hFatherPart);
		if(pFatherRod&&pFatherRod->pStart!=pNode&&pFatherRod->pEnd!=pNode)
			cbFlag=0;	//�������ļ���֪��ʲôԭ����Щ�ͻ�����м�Ķ��������ڵ�(�ȷֻ�ƫ��)��Ϊ�������κ������� wjh-2017.3.23
	}
	return cbFlag;
}
//���½Ǹ�װ������ϵ,����װ������ϵ�������˨������
//UpdateAngleUCS��ת�Ƶ�CLDSLineAngle::CompareLabelPart()�С�wjh-2016.1.03
//void UpdateAngleUCS (CLDSPart *pDatumPart,CLDSPart *pCurPart,UCS_STRU &curUcs)
//��ȡ������˨λ���б�(��������ϵ�µ��������)
//ʹ��CTempFileBuffer�滻ԭ����CBuffer�Խ������̫��ʱ���ڴ����ʧ�� wjh-2016.6.13
struct GROUPANGLEID{
	BYTE ciSectType;	//��ϽǸֽ�������
	BYTE ciSonIndex;	//��ǰ�Ǹ�����ϽǸ�������λ��������0~3
	WORD wiGroupAngleId;//������ϽǸֵ�ģ��Ψһ��ʶ��
	GROUPANGLEID(){wiGroupAngleId=ciSectType=ciSonIndex=0;}
	void SetKey(DWORD key){wiGroupAngleId=(WORD)key;}
	operator DWORD(){return *((DWORD*)this);}
	static DWORD GetRelaGroupAngleId(CLDSPart* pPart,CHashListEx<GROUPANGLEID> *pHashGroupIdCenter){
		SmartPartPtr pSmartRod=pPart;
		DWORD dwGroupAngleId=0;
		if(pPart->GetClassTypeId()==CLS_LINEANGLE&&pSmartRod.pAngle->group_father_jg_h>0x20)
		{
			GROUPANGLEID* pGroupAngleId=pHashGroupIdCenter->Add(pSmartRod.pAngle->group_father_jg_h);
			CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)pPart->BelongModel()->FromPartHandle(pSmartRod.pAngle->group_father_jg_h,CLS_GROUPLINEANGLE);
			if(pGroupAngle!=NULL)
			{
				if(pGroupAngleId->ciSectType==0)
					pGroupAngleId->ciSectType=pGroupAngle->size_cSubClassType;
				if(pGroupAngle->son_jg_h[0]==pPart->handle)
					pGroupAngleId->ciSonIndex=0;
				else if(pGroupAngle->son_jg_h[1]==pPart->handle)
					pGroupAngleId->ciSonIndex=1;
				else if(pGroupAngle->son_jg_h[2]==pPart->handle)
					pGroupAngleId->ciSonIndex=2;
				else if(pGroupAngle->son_jg_h[3]==pPart->handle)
					pGroupAngleId->ciSonIndex=3;
				else
					pGroupAngleId->ciSonIndex=0;
				dwGroupAngleId=*pGroupAngleId;
			}
		}
		return dwGroupAngleId;
	}
};
//��֤���������߼��ȵĻ������ؽ���˨��Ϣ�Ƿ�����һ��
bool VerifyFoundationIntegrity(CTower* pTower)
{
	CLDSModule* pModule;
	CLDSFoundation* pFoundation;
	CHashSet<CLDSFoundation*> heightlegs;
	CHashSet<CLDSModule*> hashModuleSet;
	bool hasFatalErrors=false;
	for(pModule=pTower->Module.GetFirst();pModule;pModule=pTower->Module.GetNext())
		hashModuleSet.SetValue(pModule->idBodyNo,pModule);
	for(pFoundation=pTower->Foundations.GetFirst();pFoundation!=NULL;pFoundation=pTower->Foundations.GetNext())
	{
		CLDSFoundation* pExistFoundation=heightlegs.GetValue(KEY4C(pFoundation->ciHeightSerial,pFoundation->ciLegSerial));
		if(pExistFoundation==NULL||pFoundation->ciLegQuad==1)
			heightlegs.SetValue(KEY4C(pFoundation->ciHeightSerial,pFoundation->ciLegSerial),pFoundation);
	}
	for(pFoundation=heightlegs.GetFirst();pFoundation!=NULL;pFoundation=heightlegs.GetNext())
	{
		pModule=hashModuleSet.GetValue(pFoundation->ciHeightSerial);
		if(pModule==NULL)
		{
			hasFatalErrors=true;
			logerr.Log("ȱ��%d�ź��ߣ�",pFoundation->ciHeightSerial);
			continue;
		}
		if(pFoundation&&pFoundation->ciLegQuad!=1)
		{
			hasFatalErrors=true;
			if(pModule!=NULL)
				logerr.Log("[%s]����%d�ż��ȣ�1������ȱ�ٻ�������",pModule->description,pFoundation->ciLegSerial);
			else
				logerr.Log("%d�ź���%d�ż��ȣ�1������ȱ�ٻ�������",pFoundation->ciHeightSerial,pFoundation->ciLegSerial);
			continue;
		}
		GEPOINT location=pModule->GetFoundationLocation(pFoundation->ciLegSerial);
		if(!location.IsEqual(pFoundation->location,1))
		{
			logerr.Log("[%s]����%d�ż��Ȼ���λ����Ĭ�ϼ���λ�ò�һ�£�ϵͳ�Ѹ���Ĭ�ϼ���λ��",pModule->description,pFoundation->ciLegSerial);
			pModule->xarrBaseLocations[pFoundation->ciLegSerial-1]=pFoundation->location;
		}
		else
		{
			hasFatalErrors=true;
			logerr.Log("[%s]����%d�ż��Ȼ���λ����Ĭ�ϼ���λ��ƫ��ϴ�",pModule->description,pFoundation->ciLegSerial);
			pModule->xarrBaseLocations[pFoundation->ciLegSerial-1]=pFoundation->location;
		}
	}
	return hasFatalErrors;
}
void CTower::ToTowerSolidDataExchangeBuffer(BUFFER_IO* pIO,BUFFER_IO *pObjHandleIO/*=NULL*/)
{
	if(DisplayProcess)
		DisplayProcess(0,progress.title);
	//1.�ļ�ͷ����Ϣ
	//1.1 ������Ϣ
	pIO->WriteString("Xerofox tower solid data media file - 1.4");
	pIO->WriteString(m_sTaType);
	//1.2 ����ģ������ϵ����
	pIO->WritePoint(GEPOINT(1, 0,0));	//X��
	pIO->WritePoint(GEPOINT(0, 0,1));	//Y��
	pIO->WritePoint(GEPOINT(0,-1,0));	//Z��
	//1.3 �˸������Ĵ洢��ַ
	UINT uiSubSectStartAddr,uiCurrAddr;
	DWORD file_sect_index_pos,current_pos,sect_start_pos;
	file_sect_index_pos=pIO->GetCursorPosition();
	pIO->Write(NULL,32);
	//2.��������Ϣ����
	//2.0 д���������Ϣ������������ַ
	sect_start_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(file_sect_index_pos);
	pIO->WriteDword(sect_start_pos);
	pIO->SeekPosition(sect_start_pos);
	//2.1 ������ռ�õ������洢�ռ��С��������ǰ�洢��С��4���ֽڣ�
	pIO->WriteDword(0);	//Ԥռλ4B���ڴ洢��������Ϣ������С
	//2.2 �����������Ϣ�ӷ���
	//д�����ֵ�������ʼ���Z���꣨ģ������ϵ�£�(V1.4��������) wjh-2018.8.18
	pIO->WriteDouble(this->NamedHeightZeroZ);
	//2.2 ���Ͱ����ĺ�������������಻����24��
	pIO->WriteByte((BYTE)Module.GetNodeNum());
	//2.3 ���߻���(���)��Ϣ
	CLDSModule* pModule;
	for(pModule=Module.GetFirst();pModule;pModule=Module.GetNext())
	{
		pIO->WriteByte((BYTE)pModule->GetBodyNo());	//�����
		//���Ӽ���ƥ����Ϣ��wjh-2016.1.02
		BYTE ciLevelHeight=(BYTE)(pModule->uiLevelHeight/100);
		pIO->WriteByte(ciLevelHeight&0x3f);	//ȡĬ�ϼ�����Ϣ�ֽڡ�wjh-2016.1.02
		pIO->WriteInteger(ftoi(pModule->NamedHeight));	//������Ƹ� (V1.4��������) wjh-2018.8.18
		CFGWORD cfgword=pModule->m_dwLegCfgWord;	 //GetBodyCfgWord();
		pIO->Write(&cfgword,24);//sizeof(CFGWORD));//���Ⱥ�
	}
	//2.3.1 ����������Ϣ
	for(pModule=Module.GetFirst();pModule;pModule=Module.GetNext())
		pIO->WriteString(pModule->description);
	//2.1*����д�뵱ǰ�ӷ����Ĵ洢�ռ��С
	current_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(sect_start_pos);
	pIO->WriteDword(current_pos-sect_start_pos-4);//�ٺ�д���������Ϣ�����Ĵ洢�ռ��С
	pIO->SeekPosition(current_pos);
	//3. ������Ϣ
	//3.0 д���������Ϣ������������ַ
	sect_start_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(file_sect_index_pos+4);
	pIO->WriteDword(sect_start_pos);
	pIO->SeekPosition(sect_start_pos);
	pIO->WriteInteger(0);	//������С
	pIO->WriteByte((BYTE)m_xFoundation.wiBasePlateThick);	//�����������ȣ�����ȷ���ؽ���˨��ñ�����������϶ֵ
	//�����ؽ���˨���ã�����ڻ������ĵ㣩��
	m_xFoundation.InitAnchorsLocation();
	pIO->WriteByte((BYTE)m_xFoundation.anchors.Count);
	for(WORD aI=0;aI<m_xFoundation.anchors.Count;aI++)
	{
		FOUNDATION_PARA::LOCATION* pAnchor=m_xFoundation.anchors.GetAt(aI);
		pIO->WriteWord(pAnchor->siPosX);
		pIO->WriteWord(pAnchor->siPosY);
	}
	//����������ӽ�����Ϣ
	BYTE ciLegStartIndex=0;	//�������ӽ������ܽ����е��������
	for(pModule=Module.GetFirst();pModule;pModule=Module.GetNext())
	{
		pIO->WriteByte(ciLegStartIndex);	//�������ӽ�������
		ciLegStartIndex+=(BYTE)pModule->GetLegCount();
	}
	BYTE ciSumLegCount=ciLegStartIndex;
	pIO->WriteByte(ciSumLegCount);	//�������ӽ���������
	VerifyFoundationIntegrity(this);
	for(pModule=Module.GetFirst();pModule;pModule=Module.GetNext())
	{
		pModule->UpdateFoundationLocations();
		BYTE ciLegCount=(BYTE)pModule->GetLegCount();
		for(BYTE ciLegSerial=1;ciLegSerial<=ciLegCount;ciLegSerial++)
		{
			GEPOINT location=pModule->GetFoundationLocation(ciLegSerial);
			pIO->WritePoint(location);
		}
	}
	//3.0*����д�뵱ǰ�ӷ����Ĵ洢�ռ��С
	current_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(sect_start_pos);
	pIO->WriteDword(current_pos-sect_start_pos-4);//�ٺ�д�������Ϣ�����Ĵ洢�ռ��С
	pIO->SeekPosition(current_pos);
	//4.0 д��ҵ㶨����Ϣ����
	sect_start_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(file_sect_index_pos+8);
	pIO->WriteDword(sect_start_pos);
	pIO->SeekPosition(sect_start_pos);

	CLDSNode* pNode;
	WORD iwn=0,wiWireNodeCount=0;
#ifdef __PART_DESIGN_INC_
	ARRAY_LIST<HANG_NODE_INFO> listWireNodes(0,32);
	for(CXhWirePoint* pWirePt=gxWirePointModel.m_hashWirePoints.GetFirst();pWirePt;pWirePt=gxWirePointModel.m_hashWirePoints.GetNext())
	{
		HANG_NODE_INFO* pGuaInfo=listWireNodes.append();
		pGuaInfo->m_xWirePlace=pWirePt->m_xWirePlace;
		pGuaInfo->pos=pWirePt->xPosition;
		if(pWirePt->m_xWirePlace.ciWireType=='C')			//����
			pGuaInfo->m_ciType='C';
		else if(pWirePt->m_xWirePlace.ciWireType=='E')		//����
			pGuaInfo->m_ciType='G';
		else //if(pWirePt->m_xWirePlace.ciWireType=='J')	//����
			pGuaInfo->m_ciType='T';
		if(strlen(pWirePt->m_sGimName)>0)
			strcpy(pGuaInfo->m_sName,pWirePt->m_sGimName);
		//����ҵ�����ҿ�����
		for (int i = 0; i < 2; i++)
		{
			CLDSBolt* pBolt = (CLDSBolt*)FromPartHandle(pWirePt->holes[i].hBolt, CLS_BOLT);
			CLDSPart* pResidePart = FromPartHandle(pWirePt->holes[i].hResidePart);
			if(pBolt==NULL || pResidePart==NULL)
				break;
			GEPOINT xBasePt;
			if (pResidePart->GetBoltIntersPos(pBolt, xBasePt, NULL, CLDSPart::DATUM_PLANE))
				pGuaInfo->relaHolePt[i] = xBasePt;
		}
	}
	CQuickSort<HANG_NODE_INFO>::QuickSortClassic(listWireNodes.m_pData,listWireNodes.GetSize());
	wiWireNodeCount=(WORD)listWireNodes.GetSize();
#endif
	pIO->WriteInteger(2+wiWireNodeCount*64);
	pIO->WriteWord(wiWireNodeCount);
#ifdef __PART_DESIGN_INC_
	for(iwn=0;iwn<wiWireNodeCount;iwn++)
	{
		pIO->Write(&listWireNodes[iwn].m_xWirePlace,2);
		pIO->WritePoint(listWireNodes[iwn].pos);
		pIO->Write(listWireNodes[iwn].m_sName,38);
	}
#endif
	//5.0 д�빤�������Ϣ����
	sect_start_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(file_sect_index_pos+12);
	pIO->WriteDword(sect_start_pos);
	pIO->SeekPosition(sect_start_pos);
	uiSubSectStartAddr=pIO->GetCursorPosition();
	pIO->WriteInteger(0);		//��ǰ�ӷ�����С
	//д�빤�������Ϣ�ӷ�������
	pIO->WriteByte((BYTE)2);	//�������������
	pIO->WriteInteger(100);		//100��ʾ����GIM����, ֮ǰ23545686δ�������ã���ʾ�Թ������ӹ���GIM������ʶ����Ϊ���� wjh-2020.1.12
	pIO->WriteInteger(0);		//GIM�ӷ����洢��ַ
	pIO->WriteInteger(0);		//GIM�ӷ����洢����
	pIO->WriteInteger(101);		//101��ʾ�ҵ���Ϣ����
	pIO->WriteInteger(0);		//�ҵ���Ϣ�ӷ����洢��ַ
	pIO->WriteInteger(0);		//�ҵ���Ϣ�ӷ����洢����
	//5.1д��GIM��Ϣ
	sect_start_pos = pIO->GetCursorPosition();
	pIO->SeekPosition(uiSubSectStartAddr + 5 + 4);
	pIO->WriteDword(sect_start_pos);	//д��GIM�ӷ�����������ַ
	pIO->SeekPosition(sect_start_pos);
#ifdef __PART_DESIGN_INC_
	pIO->Write(gxTowerGimInfo.m_xCurrGimFileHeader.m_sFileTag, 16);
	pIO->Write(gxTowerGimInfo.m_xCurrGimFileHeader.m_sFileName, 256);
	pIO->Write(gxTowerGimInfo.m_xCurrGimFileHeader.m_sDesigner, 64);
	pIO->Write(gxTowerGimInfo.m_xCurrGimFileHeader.m_sUnit, 256);
	pIO->Write(gxTowerGimInfo.m_xCurrGimFileHeader.m_sSoftName, 128);
	pIO->Write(gxTowerGimInfo.m_xCurrGimFileHeader.m_sTime, 16);
	pIO->Write(gxTowerGimInfo.m_xCurrGimFileHeader.m_sSoftMajorVer, 8);
	pIO->Write(gxTowerGimInfo.m_xCurrGimFileHeader.m_sSoftMinorVer, 8);
	pIO->Write(gxTowerGimInfo.m_xCurrGimFileHeader.m_sMajorVersion, 8);
	pIO->Write(gxTowerGimInfo.m_xCurrGimFileHeader.m_sMinorVersion, 8);
	pIO->Write(gxTowerGimInfo.m_xCurrGimFileHeader.m_sBufSize, 8);
	//
	pIO->WriteInteger(gxTowerGimInfo.m_xTowerGimPropFile.m_nCircuit);
	pIO->WriteDouble(gxTowerGimInfo.m_xTowerGimPropFile.m_fWindSpeed);
	pIO->WriteDouble(gxTowerGimInfo.m_xTowerGimPropFile.m_fNiceThick);
	pIO->WriteDouble(gxTowerGimInfo.m_xTowerGimPropFile.m_fFrontRulingSpan);
	pIO->WriteDouble(gxTowerGimInfo.m_xTowerGimPropFile.m_fBackRulingSpan);
	pIO->WriteDouble(gxTowerGimInfo.m_xTowerGimPropFile.m_fMaxSpan);
	pIO->WriteDouble(gxTowerGimInfo.m_xTowerGimPropFile.m_fDesignKV);
	pIO->WriteDouble(gxTowerGimInfo.m_xTowerGimPropFile.m_fFrequencyRockAngle);
	pIO->WriteDouble(gxTowerGimInfo.m_xTowerGimPropFile.m_fLightningRockAngle);
	pIO->WriteDouble(gxTowerGimInfo.m_xTowerGimPropFile.m_fSwitchingRockAngle);
	pIO->WriteDouble(gxTowerGimInfo.m_xTowerGimPropFile.m_fWorkingRockAngle);
	pIO->WriteString((char*)gxTowerGimInfo.m_xTowerGimPropFile.m_sVoltGrade);
	pIO->WriteString((char*)gxTowerGimInfo.m_xTowerGimPropFile.m_sType);
	pIO->WriteString((char*)gxTowerGimInfo.m_xTowerGimPropFile.m_sTexture);
	pIO->WriteString((char*)gxTowerGimInfo.m_xTowerGimPropFile.m_sFixedType);
	pIO->WriteString((char*)gxTowerGimInfo.m_xTowerGimPropFile.m_sTaType);
	pIO->WriteString((char*)gxTowerGimInfo.m_xTowerGimPropFile.m_sCWireSpec);
	pIO->WriteString((char*)gxTowerGimInfo.m_xTowerGimPropFile.m_sEWireSpec);
	pIO->WriteString((char*)gxTowerGimInfo.m_xTowerGimPropFile.m_sWindSpan);
	pIO->WriteString((char*)gxTowerGimInfo.m_xTowerGimPropFile.m_sWeightSpan);
	pIO->WriteString((char*)gxTowerGimInfo.m_xTowerGimPropFile.m_sAngleRange);
	pIO->WriteString((char*)gxTowerGimInfo.m_xTowerGimPropFile.m_sRatedHeight);
	pIO->WriteString((char*)gxTowerGimInfo.m_xTowerGimPropFile.m_sHeightRange);
	pIO->WriteString((char*)gxTowerGimInfo.m_xTowerGimPropFile.m_sTowerWeight);
	pIO->WriteString((char*)gxTowerGimInfo.m_xTowerGimPropFile.m_sManuFacturer);
	pIO->WriteString((char*)gxTowerGimInfo.m_xTowerGimPropFile.m_sMaterialCode);
	pIO->WriteString((char*)gxTowerGimInfo.m_xTowerGimPropFile.m_sProModelCode);
#endif
	current_pos = pIO->GetCursorPosition();
	pIO->SeekPosition(uiSubSectStartAddr + 5 + 8);
	pIO->WriteDword(current_pos - sect_start_pos);
	pIO->SeekPosition(current_pos);
	//5.2д��ҵ���Ϣ
	sect_start_pos = pIO->GetCursorPosition();
	pIO->SeekPosition(uiSubSectStartAddr + 5 + 12 + 4);
	pIO->WriteDword(sect_start_pos);	//д��GIM�ӷ�����������ַ
	pIO->SeekPosition(sect_start_pos);
#ifdef __PART_DESIGN_INC_
	pIO->WriteWord(wiWireNodeCount);
	for (iwn = 0; iwn < wiWireNodeCount; iwn++)
	{
		pIO->Write(&listWireNodes[iwn].m_xWirePlace, 2);
		pIO->WritePoint(listWireNodes[iwn].pos);
		pIO->WritePoint(listWireNodes[iwn].relaHolePt[0]);
		pIO->WritePoint(listWireNodes[iwn].relaHolePt[1]);
		pIO->Write(listWireNodes[iwn].m_sName, 38);
	}
#endif
	current_pos = pIO->GetCursorPosition();
	pIO->SeekPosition(uiSubSectStartAddr + 5 + 12 + 8);
	pIO->WriteDword(current_pos - sect_start_pos);
	pIO->SeekPosition(current_pos);
	//д�빤�������Ϣ������С
	uiCurrAddr=pIO->CursorPosition;
	pIO->SeekPosition(uiSubSectStartAddr);
	pIO->WriteInteger(uiCurrAddr-uiSubSectStartAddr-4);
	pIO->SeekPosition(uiCurrAddr);
	//6.������Ϣ����
	//6.0 д�벿����Ϣ������������ַ
	sect_start_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(file_sect_index_pos+16);
	pIO->WriteDword(sect_start_pos);	//д�벿����Ϣ������������ַ
	pIO->SeekPosition(sect_start_pos);
	//6.1 ������ռ�õ������洢�ռ��С��������ǰ�洢��С��4���ֽڣ�
	pIO->WriteDword(0);
	//6.2 �����Ͱ����Ĳ��������������65535������
	WORD wBlockN=(WORD)Block.GetNodeNum();
	pIO->WriteWord(wBlockN);
	//6.3 ���δ洢ÿ�������Ķ�λ����ϵ��Ϣ[���䲿������]��ÿ����¼100 Byte
	CBlockModel *pBlock;
	DWORD block_info_addr=pIO->GetCursorPosition();
	for(pBlock=Block.GetFirst();pBlock!=NULL;pBlock=Block.GetNext())
	{
		UCS_STRU lcs=pBlock->GetLCS();
		pIO->WritePoint(lcs.origin);
		pIO->WritePoint(lcs.axis_x);
		pIO->WritePoint(lcs.axis_y);
		pIO->WritePoint(lcs.axis_z);
		//pIO->WriteByte(pBlock->IsEmbeded());
		pIO->WriteDword(0);	//���ƴ洢��ַ
	}
	//6.4 ���δ洢ÿ�����������ơ���λ����ϵ��Ϣ[���䲿������]
	WORD blocki=0;
	for(pBlock=Block.GetFirst();pBlock!=NULL;pBlock=Block.GetNext(),blocki++)
	{
		current_pos=pIO->GetCursorPosition();
		pIO->SeekPosition(block_info_addr+100*blocki+96);
		pIO->WriteDword(current_pos);
		pIO->SeekPosition(current_pos);
		pIO->WriteString(pBlock->name);
	}
	//6.5 ���±������Ĵ洢����(������ǰ����)
	current_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(sect_start_pos);
	pIO->WriteDword(current_pos-sect_start_pos-4);
	pIO->SeekPosition(current_pos);
	//7.��˨��׼����Ϣ����
	sect_start_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(file_sect_index_pos+20);
	pIO->WriteDword(sect_start_pos);
	pIO->SeekPosition(sect_start_pos);
	pIO->WriteDword(0);	//��ʼ��������ռ�õ������洢�ռ��С
	//7.1 ��˨���ϵ����N_bolt_series
	BYTE bolt_series_n=(BYTE)CLsLibrary::GetFamilyNum();
	pIO->WriteByte(bolt_series_n);
	//7.2 ��ϵ�еĴ洢��ַ
	DWORD series_start_pos=pIO->GetCursorPosition();
	pIO->Write(NULL,bolt_series_n*4);
	//7.3 �ؽ���˨���ݵĴ洢��ַ
	pIO->WriteDword(0);
	//7.4 ��ϵ����˨�����ݴ洢��
	int bolt_series_i=0;
	CLsFamily* pFamily;
	CHashList<LS_FAMILY_INFO>hashLsFamilyById;
	CMapList<LS_XING_HAO> hashFamilyLsByL_d[10];	//һ����˨ϵ�����������10������������������Ҫ��
	for(pFamily=CLsLibrary::GetFirstFamily();pFamily;pFamily=CLsLibrary::GetNextFamily(),bolt_series_i++)
	{
		LS_FAMILY_INFO* pFamilyInfo=hashLsFamilyById.Add(pFamily->GetFamilyHandle());
		pFamilyInfo->indexId=bolt_series_i+1;
		pFamilyInfo->pFamily=pFamily;
		pFamilyInfo->pHashLsIndexBySpec=new CHashStrList<WORD>;
		WORD count=(WORD)pFamily->GetCount();
		CHashList<BOLT_CAP_PARA> hashBoltCap;
		DWORD family_start_pos=pIO->GetCursorPosition();
		pIO->SeekPosition(series_start_pos+bolt_series_i*4);
		pIO->WriteDword(family_start_pos);
		//4.3.1	��ǰ�ӷ������ڿռ��С��������ǰ�����ֽڣ�
		pIO->SeekPosition(family_start_pos);
		pIO->WriteDword(0);
		//4.3.2 ��˨ֱ����������
		pIO->WriteByte(0);
		//4.3.3 ��˨Ĭ�ϵ�Ȧ��������ñ����
		pIO->WriteByte((BYTE)pFamily->m_bAntiLoose);
		pIO->WriteByte((BYTE)pFamily->m_bDualCap?2:1);
		//4.3.4	��˨�������
		pIO->WriteWord(count);
		//4.3.5 ��˨���ϵ�е����� wjh-2016.1.02
		char series_name[23]="";
		strncpy(series_name,pFamily->name,23);	//strcpy_s<23>��VS7.0ƽ̨û������
		pIO->Write(series_name,23);
		WORD bolt_i;
		char spec[18];
		for(bolt_i=0;bolt_i<count;bolt_i++)
		{	//����д����˨����¼���ݣ�ÿ����¼����Ϊ32Byte
			//V1.1�汾�����˨ͨ�����޺���˨ͨ������,ÿ����¼��������4Byte��Ϊ36Byte	wht 15-07-08
			LS_XING_HAO *pLsXingHao=pFamily->RecordAt(bolt_i);
			CXhChar16 boltspec("M%dX%d",pLsXingHao->d,pLsXingHao->valid_len);
			pFamilyInfo->pHashLsIndexBySpec->SetValue(boltspec,bolt_i+1);
			hashBoltCap.Add(pLsXingHao->d);

			pIO->WriteWord((WORD)pLsXingHao->d);				//��˨ֱ��(2Byte)
			pIO->WriteWord((WORD)pLsXingHao->valid_len);		//��˨��Ч��(2Byte)
			pIO->WriteWord((WORD)pLsXingHao->no_thread_len);	//�޿۳�(2Byte)
			//V1.1�汾�����˨ͨ�����޺���˨ͨ������	wht 15-07-08
			pIO->WriteWord((WORD)pLsXingHao->up_lim);			//ͨ������
			pIO->WriteWord((WORD)pLsXingHao->down_lim);		//ͨ������
			pIO->WriteDouble(pLsXingHao->weight);				//��������(kg)
			memset(spec,0,18);
			strncpy(spec,pLsXingHao->guige,17);
			pIO->Write(spec,18);	//�̶����ȵ���˨��������ַ�
		}
		//4.4 ��ʼ����˨ñ��ͷ��ʵ����Ϣ
		pIO->SeekPosition(family_start_pos+4);
		pIO->WriteByte((BYTE)hashBoltCap.GetNodeNum());
		BOLT_CAP_PARA *pCap;
		for(pCap=hashBoltCap.GetFirst();pCap;pCap=hashBoltCap.GetNext())
		{
			DWORD d=hashBoltCap.GetCursorKey();
			if(CLDSPart::library!=NULL)
				CLDSPart::library->FindLsBoltDH(d,&pCap->D,&pCap->H,&pCap->H2);
		}
		pIO->SeekToEnd();
		//4.4.1 ��ʼ����˨ñ����˨��ʵ������������
		DWORD bolt_soliddef_start_pos=pIO->GetCursorPosition();
		pIO->Write(NULL,hashBoltCap.GetNodeNum()*6);	//��ֱ����ñʵ�����ݴ洢��ַ
		pIO->Write(NULL,count*4);						//����˨����ʵ�����ݴ洢��ַ
		//4.4.2 ���ɲ�д����ñʵ������
		int di=0;
		for(pCap=hashBoltCap.GetFirst();pCap;pCap=hashBoltCap.GetNext(),di++)
		{
			//4.4.2.1 ���ɵ�ǰ��ñ��ʵ�嶨������
			fBody body;
			DWORD d=hashBoltCap.GetCursorKey();
			double H2=pCap->H2*(pFamily->m_bDualCap?2:1);
			CLDSBolt::CreateBoltCapSolidBody(&body,d,pCap->D,H2);
			CSolidBody solid;
			solid.ConvertFrom(&body);
			//4.4.2.2 ����ñʵ������д���ļ�������
			current_pos=pIO->GetCursorPosition();
			pIO->SeekPosition(bolt_soliddef_start_pos+di*6);
			pIO->WriteWord((WORD)d);
			pIO->WriteDword(current_pos);
			pIO->SeekPosition(current_pos);
			pIO->WriteDword(solid.BufferLength());
			pIO->Write(solid.BufferPtr(),solid.BufferLength());
		}
		//4.4.3 ���ɲ�д����˨ʵ������
		DWORD bolt_cap_index_len=hashBoltCap.GetNodeNum()*6;
		for(bolt_i=0;bolt_i<count;bolt_i++)
		{	//����д����˨����¼���ݣ�ÿ����¼����Ϊ32Byte
			//4.4.3.1 ���ɵ�ǰ��˨��ʵ�嶨������
			LS_XING_HAO *pXingHao=pFamily->RecordAt(bolt_i);
			current_pos=pIO->GetCursorPosition();
			fBody body;
			pCap=hashBoltCap.GetValue(pXingHao->d);
			CLDSBolt::CreateBoltSolidBody(&body,pXingHao->d,pXingHao->valid_len,pCap->D,pCap->H);
			CSolidBody solid;
			solid.ConvertFrom(&body);
			//4.4.3.2 ����˨ʵ������д���ļ�������
			current_pos=pIO->GetCursorPosition();
			pIO->SeekPosition(bolt_soliddef_start_pos+bolt_cap_index_len+bolt_i*4);
			pIO->WriteDword(current_pos);
			pIO->SeekPosition(current_pos);
			pIO->WriteDword(solid.BufferLength());
			pIO->Write(solid.BufferPtr(),solid.BufferLength());
		}
		current_pos=pIO->GetCursorPosition();
		pIO->SeekPosition(family_start_pos);
		pIO->WriteDword(current_pos-family_start_pos-4);
		pIO->SeekPosition(current_pos);
	}
	//4.5 ��˨������
	//for(pFamily=CLsLibrary::GetFirstFamily();pFamily;pFamily=CLsLibrary::GetNextFamily())
	//	pIO->WriteString(pFamily->name);
	//4.6 �ؽ���˨�����ݴ洢
	uiCurrAddr=pIO->GetCursorPosition();
	pIO->SeekPosition(series_start_pos+bolt_series_n*4);
	pIO->WriteDword(uiCurrAddr);
	pIO->SeekPosition(uiCurrAddr);
	pIO->WriteWord(this->m_xFoundation.wiBoltD);
	const ANCHOR_BOLT* pAnchorBolt=ANCHOR_BOLT::FromBoltD(m_xFoundation.wiBoltD);
	pIO->WriteWord((WORD)pAnchorBolt->m_fLe);	//�ؽ���˨��¶��
	pIO->WriteWord((WORD)pAnchorBolt->m_fLa);	////�ؽ���˨�޿۳�(mm) <��������
	pIO->WriteWord(pAnchorBolt->wiWidth);		//�����
	pIO->WriteWord(pAnchorBolt->wiThick);		//�����
	pIO->WriteWord((WORD)(m_xFoundation.wiBoltD+pAnchorBolt->wiHoleIncrement));	//���׾�
	pIO->WriteWord(pAnchorBolt->wiBasePlateHoleD);	//�ؽ���˨���ӵ����彨��׾�ֵ
	CXhChar100 anchorstr;
	UINT uiLo=ftoi(pAnchorBolt->m_fLe-pAnchorBolt->m_fLa);	//˿�۳�(mm)=��¶��-�޿۳�
	anchorstr.Printf("DL/T 1236-2013-M%dX1200X%d-L-5.6",pAnchorBolt->d,pAnchorBolt->m_fLe,uiLo);	//�ؽ���˨���ʾ��
	pIO->Write((char*)anchorstr,64);	//�̶�����64B�ĵؽ���˨������ַ���
		//4.6.1 ���ɵؽ���˨ñ��˫ñ+��壩�ĵ�ʵ�壬��������д���ļ�������
	fBody xSolidOfAnchorCap,xSolidOfAnchorBolt;
	CSolidBody solidOfAnchorCap,solidOfAnchorBolt;
	CLDSAnchorBolt::CreateAnchorBoltCapSolidBody(&xSolidOfAnchorCap,pAnchorBolt);
	solidOfAnchorCap.ConvertFrom(&xSolidOfAnchorCap);
	current_pos=pIO->GetCursorPosition();
	pIO->WriteDword(solidOfAnchorCap.BufferLength());
	pIO->Write(solidOfAnchorCap.BufferPtr(),solidOfAnchorCap.BufferLength());
		//4.6.2 ���ɵؽ���˨ʵ�壬��������д���ļ�������
	CLDSAnchorBolt::CreateAnchorBoltSolidBody(&xSolidOfAnchorBolt,pAnchorBolt);
	solidOfAnchorBolt.ConvertFrom(&xSolidOfAnchorBolt);
	pIO->WriteDword(solidOfAnchorBolt.BufferLength());
	pIO->Write(solidOfAnchorBolt.BufferPtr(),solidOfAnchorBolt.BufferLength());

	//7.7 ���±������Ĵ洢����(������ǰ����), V1.4�������� wjh-2018.8.18
	current_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(sect_start_pos);
	pIO->WriteDword(current_pos-sect_start_pos-4);
	pIO->SeekPosition(current_pos);
	//8.������Ϣ����
	sect_start_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(file_sect_index_pos+24);
	pIO->WriteDword(sect_start_pos);
	pIO->SeekPosition(sect_start_pos);
	pIO->WriteDword(0);	//��ʼ��������ռ�õ������洢�ռ��С
	pIO->WriteByte((BYTE)CSteelMatLibrary::GetCount());	//���Ͽ������������

	ATOM_LIST<BLOCK_PARTPTR> allPartList;
	ATOM_LIST<BLOCK_BOLTPTR> allBoltList;
	DYN_ARRAY<CLDSPartPtr> towerPartArr;
	DYN_ARRAY<CLDSBolt*> towerBoltArr;
	POLYPARTLIST polyPartList;
	RetrievePolyPartList(polyPartList);
	GetRealityPartArr(towerBoltArr,towerPartArr,&polyPartList);
	DWORD tower_part_n=towerPartArr.Size();
	DWORD tower_bolt_n=towerBoltArr.Size();
	DWORD parti=0;
	for(parti=0;parti<tower_part_n;parti++)
		allPartList.append(BLOCK_PARTPTR(towerPartArr[parti],0));
	for(parti=0;parti<tower_bolt_n;parti++)
		allBoltList.append(BLOCK_BOLTPTR(towerBoltArr[parti],0));
	CHashList<WORD>hashBlockById;	//�������ʶ���Ϊ��ֵ���洢����ֵ��1Ϊ������Ϊֵ
	blocki=1;
	for(pBlock=Block.GetFirst();pBlock;pBlock=Block.GetNext(),blocki++)
	{
		DYN_ARRAY<CLDSPartPtr> blockPartArr;
		DYN_ARRAY<CLDSBolt*> blockBoltArr;
		hashBlockById.SetValue(pBlock->handle,blocki);
		if(!pBlock->IsEmbeded())
		{
			pBlock->RetrievePolyPartList(polyPartList);
			pBlock->GetRealityPartArr(blockBoltArr,blockPartArr,&polyPartList);
			DWORD block_part_n=blockPartArr.Size();
			DWORD block_bolt_n=blockBoltArr.Size();
			for(parti=0;parti<block_part_n;parti++)
				allPartList.append(BLOCK_PARTPTR(blockPartArr[parti],pBlock->handle));
			for(parti=0;parti<block_bolt_n;parti++)
				allBoltList.append(BLOCK_BOLTPTR(blockBoltArr[parti],pBlock->handle));
		}
	}
	//��ȡ��˨����ģ����Ϣ
	CMapList<DWORD> hashBoltIndexByHandle;
	DYN_ARRAY<BLOCK_BOLTPTR> arrBoltPtr;
	arrBoltPtr.Resize(allBoltList.GetNodeNum());
	parti=0;
	for(BLOCK_BOLTPTR *pBlkBol=allBoltList.GetFirst();pBlkBol;pBlkBol=allBoltList.GetNext(),parti++)
	{
		arrBoltPtr[parti]=*pBlkBol;
		hashBoltIndexByHandle.SetValue(pBlkBol->m_hBlock,pBlkBol->m_pBolt->handle,parti+1);
	}
	//��ȡͬ��ŵĹ���������Ϣ
	CLDSPart *pPart=NULL;
	LABEL_PART* pLabelPart;
	CHashStrList<LABEL_PART>  hashLabelParts;
	for(BLOCK_PARTPTR *pBlkPart=allPartList.GetFirst();pBlkPart;pBlkPart=allPartList.GetNext())
	{
		if(pBlkPart->m_pPart==NULL)
			continue;
		//�ޱ�Ź�����Ӧ������Ӧ��ͬ����ͬ��űȶԻ��ڣ�һ��һ���������wjh-2016.1.02
		pPart=pBlkPart->m_pPart;
		CXhChar50 label(pPart->GetPartNo());
		if(label.GetLength()==0)
			label.Printf("0X%X",pPart->handle);
		LABEL_PART* pPartInfo=hashLabelParts.GetValue(label);
		if(pPartInfo==NULL)
		{
			pPartInfo=hashLabelParts.Add(label);
			pPartInfo->pPart=pPart;
			pPartInfo->label=label;
			pPartInfo->hBlock=pBlkPart->m_hBlock;
			pPartInfo->idPartIndex=hashLabelParts.GetNodeNum();
		}
		if(pPartInfo)
		{
			CLDSPart::PEER_PART peerpart,*pPeerPart;
			if(pPartInfo->pPart->CompareLabelPart(pPart,NULL,TRUE,&peerpart,1.0))
			{
				pPeerPart=pPartInfo->hashActualParts.Add(pPart->handle);
				pPeerPart->CopyFrom(peerpart);
			}
			else
			{
				label.Append(CXhChar16("#0X%X",pPart->handle));
				CLDSPart *pDatumPart=pPartInfo->pPart;
				pPartInfo=hashLabelParts.Add(label);
				pPartInfo->pPart=pPart;
				pPartInfo->label=label;
				pPartInfo->hBlock=pBlkPart->m_hBlock;
				pPartInfo->idPartIndex=hashLabelParts.GetNodeNum();
				peerpart.csPeer=pPart->GetUnifiedACS();
				pPeerPart=pPartInfo->hashActualParts.Add(pPart->handle);
				pPeerPart->csBase=pPeerPart->csPeer=pPart->GetUnifiedACS();
				pPeerPart->m_hBlock=pBlkPart->m_hBlock;
				if(strlen(pPart->GetPartNo())>0) 
					CLDSObject::Log2File()->Log("ͬ���(%s)%s0x%X��0x%X����ȫ��ͬ",
					(char*)pPart->GetPartNo(),pPart->GetPartTypeName(),pDatumPart->handle,pPart->handle);
			}
			pPeerPart->pPart=pPart;
		}
	}
	//д�벻ͬ���ŵĹ�������
	DWORD part_n_addr=pIO->GetCursorPosition();
	pIO->WriteDword(hashLabelParts.GetNodeNum());		
	//5.1 ���Ͽ���Ϣ��
	char mat_mark[7];
	for(int mati=0;mati<CSteelMatLibrary::GetCount()&&mati<255;mati++)
	{
		pIO->WriteByte(CSteelMatLibrary::RecordAt(mati).cBriefMark);	//���ʵļ��ַ�
		memset(mat_mark,0,7);
		strncpy(mat_mark,CSteelMatLibrary::RecordAt(mati).mark,7);
		pIO->Write(mat_mark,7);			//�̶����ȵĲ����ַ�����
	}
	//5.2 ����������Ϣ��
	DWORD part_basicinfo_pos=pIO->GetCursorPosition();
	for(pLabelPart=hashLabelParts.GetFirst();pLabelPart;pLabelPart=hashLabelParts.GetNext())
	{	//ÿ������������Ϣ��¼�̶�����33Byte
		pPart=pLabelPart->pPart;
		pBlock=NULL;
		blocki=0;
		if(pPart->m_hBlock>0)
			pBlock=Block.FromHandle(pPart->m_hBlock);
		if(pBlock!=NULL)
			blocki=hashBlockById[pBlock->handle];
		//5.2.1 д�������ģ�Ϳռ䣬����ʾ��������ģ�Ϳռ䣬����ֵ��ʾ���������ı�ʶ�����š�
		pIO->WriteWord(blocki);
		//5.2.2 д�빹������
		int cls_id=pPart->GetClassTypeId();
		if(cls_id==CLS_POLYPART)
		{
			CLDSLinePart *pFirstRod=((CLDSPolyPart*)pPart)->segset.GetFirst();
			if(pFirstRod)
				cls_id=pFirstRod->GetClassTypeId();
		}
		if(cls_id==CLS_LINEANGLE)
			pIO->WriteByte((BYTE)STEELTYPE_ANGLE);
		else if(cls_id==CLS_BOLT)
			pIO->WriteByte((BYTE)STEELTYPE_BOLT);
		else if(cls_id==CLS_PLATE||cls_id==CLS_PARAMPLATE)
			pIO->WriteByte((BYTE)STEELTYPE_PLATE);
		else if(cls_id==CLS_LINETUBE)
			pIO->WriteByte((BYTE)STEELTYPE_TUBE);
		else if(cls_id==CLS_LINEFLAT)
			pIO->WriteByte((BYTE)STEELTYPE_FLAT);
		else if(cls_id==CLS_LINESLOT)
			pIO->WriteByte((BYTE)STEELTYPE_SLOT);
		else
			pIO->WriteByte(0);
		//5.2.2 д�빹��״̬����
		if(blocki>0)
			pIO->WriteWord((WORD)0x8000);	//��λΪ1��ʾ����ͬʱ����Ƕ��ʽ�����͸���ģ�Ϳռ�
		else
			pIO->WriteWord((WORD)0);
		pIO->WriteByte(pPart->cMaterial); //д����ʼ��ַ�
		pIO->WriteInteger(pPart->iSeg);	//д������ֶκ�
		pIO->WriteFloat((float)pPart->GetWidth());	//д����
		pIO->WriteFloat((float)pPart->GetThick());	//д����
		pIO->WriteFloat((float)pPart->GetHeight());	//д��߶�
		pIO->WriteWord((WORD)ftoi(pPart->GetLength()));//д�볤��(mm)
		pIO->WriteFloat((float)pPart->GetWeight());	//д�뵥��(kg)
		pIO->WriteByte(pPart->FuncType());			//д�빦������
		pIO->WriteDword(0);	//��ʼ������������Ϣ�洢��ַΪ0
	}
	//5.3 ����������Ϣ��
	CXhChar200 spec;
	UCS_STRU stducs;
	LoadDefaultUCS(&stducs);
	CBuffer processBuffer;
	if(DisplayProcess)
		DisplayProcess(2,progress.title);
	for(pLabelPart=hashLabelParts.GetFirst();pLabelPart;pLabelPart=hashLabelParts.GetNext())
	{	//ÿ������������Ϣ��¼�̶�����33 Byte
		CLDSPartPtr pPart=pLabelPart->pPart;
		//5.3.1 ���¹���������Ϣ�洢λ��
		current_pos=pIO->GetCursorPosition();
		pIO->SeekPosition(part_basicinfo_pos+(pLabelPart->idPartIndex-1)*33+29);
		pIO->WriteDword(current_pos);
		pIO->SeekPosition(current_pos);
		//5.3.2 д�빹��������Ϣ
		pPart->Create3dSolidModel(pPart->IsPolyPart());
		if(pPart->pSolidBody==NULL)
			pIO->WriteDword(0);	//TODO:logerr.Log(...)
		else
		{	//ʵ���������Թ���Ĭ��װ�䶨λ����ϵΪ��׼��
			CSolidBody solid(pPart->pSolidBody->BufferPtr(),pPart->pSolidBody->BufferLength());
			//���ɹ���ʵ������ʱ����ϵ���忼�Ǹ˼���ͷ���ְ巨��ƫ���� wht 14-10-08
			solid.TransACS(pPart->GetUnifiedACS(),stducs);
			pIO->WriteDword(solid.BufferLength());
			pIO->Write(solid.BufferPtr(),solid.BufferLength());
		}
		pIO->WriteString(pPart->GetPartNo());
		pPart->GetSpecification(spec,TRUE);
		pIO->WriteString(spec);
		pIO->WriteString(pPart->sNotes); 
		//���湹��������Ϣ wht 14-08-16
		processBuffer.ClearContents();
		int bufferSize=pPart->WriteProcessBuffer(processBuffer,false,1000001);
		pIO->WriteDword(bufferSize);
		processBuffer.SeekToBegin();
		pIO->Write(processBuffer.GetBufferPtr(),processBuffer.GetLength());
		if(DisplayProcess)
			DisplayProcess(2+(pLabelPart->idPartIndex-1)*97/hashLabelParts.GetNodeNum(),progress.title);
	}
	current_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(sect_start_pos);
	pIO->WriteDword(current_pos-sect_start_pos-4);  
	pIO->SeekPosition(current_pos);
	//9.װ����Ϣ����
	sect_start_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(file_sect_index_pos+28);
	pIO->WriteDword(sect_start_pos);
	pIO->SeekPosition(sect_start_pos);
	pIO->WriteDword(0);	//��ʼ��������ռ�õ������洢�ռ��С
	//9.1 ����װ�������
	pIO->WriteDword(BlockRef.GetNodeNum());	//����װ���������
	CBlockReference *pBlockRef;
	for(pBlockRef=BlockRef.GetFirst();pBlockRef;pBlockRef=BlockRef.GetNext())
	{	//ÿ��װ������¼����Ϊ128 Bytes
		pIO->WriteWord(hashBlockById[pBlockRef->m_hBlock]);	//����������ʶ��
		UCS_STRU acs = pBlockRef->GetACS();
		pIO->WritePoint(acs.origin);
		pIO->WritePoint(acs.axis_x);
		pIO->WritePoint(acs.axis_y);
		pIO->WritePoint(acs.axis_z);
		pIO->WriteDword(pBlockRef->iSeg);	//�����κ�
		char cQuad=0;
		if(pBlockRef->IsLegObj())
		{
			cQuad=pBlockRef->layer(2);
			/*if(cQuad=='3')
				cQuad=4;
			else if(cQuad=='4')
				cQuad=3;
			else //if(cQuad=='1'||cQuad=='2')*/
				cQuad-='0';
		}
		pIO->WriteByte(cQuad);	//�������ȵ����޺�
		pIO->Write(&pBlockRef->cfgword,24);
		pIO->WriteByte(0);		//1Byte ����λ
	}
	//----VVVV----V1.4�������� wjh-2018.8.18
	//6.2 �ڵ�װ�������
	NODESET allnodes;
	for(pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		if(pNode->m_bVirtualObj)
			continue;
		allnodes.append(pNode);
	}
	DWORD node_assemble_pos=pIO->GetCursorPosition();
	pIO->WriteDword(0);	//��ʼ���ڵ㶨���ӷ�����ռ�洢�ռ��С
	//6.2.1 д�벿���ռ估�����ռ��нڵ��������
	pIO->WriteDword(0);//arrBoltPtr.Size()-tower_bolt_n);	//��ʼ������ģ�Ϳռ���˨��׼��װ������
	pIO->WriteDword(allnodes.GetNodeNum());//tower_bolt_n);					//��ʼ������ģ�Ϳռ���˨��׼��װ������
	//6.2.2 д�벿���ռ��нڵ���������Ϣ
	UINT uiNodeIndesSerial=1;	//�ڵ��������ʶ�ţ���˼��˽ڵ�ƥ��ʱ�ã�
	for(pNode=allnodes.GetFirst();pNode;pNode=allnodes.GetNext(),uiNodeIndesSerial++)
	{
		pNode->feature=uiNodeIndesSerial;
		pIO->WriteInteger(pNode->pointI);
		BYTE cLegQuad=(pNode->IsLegObj())?pNode->ciLegQuad:0;
		pIO->WriteByte(cLegQuad);
		pIO->Write(pNode->layer(),3);
		pIO->Write(&pNode->cfgword,24);
		pIO->WritePoint(pNode->xOriginalPos);
	}
	//���±�������ռ�洢�ռ��С
	current_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(node_assemble_pos); 
	pIO->WriteDword(current_pos-node_assemble_pos-4);
	pIO->SeekPosition(current_pos);
	//----^^^^----V1.4�������� wjh-2018.8.18
	//6.3 ��˨��׼��װ�������
	DWORD bolt_assemble_pos=pIO->GetCursorPosition();
	pIO->WriteDword(0);	//��ʼ����˨��׼����װ�䶨���ӷ�����ռ�洢�ռ��С
	//6.3.1 д�벿���ռ估�����ռ�����˨��׼��װ���������
	pIO->WriteDword(arrBoltPtr.Size()-tower_bolt_n);	//��ʼ������ģ�Ϳռ���˨��׼��װ������
	pIO->WriteDword(tower_bolt_n);					//��ʼ������ģ�Ϳռ���˨��׼��װ������
	//6.3.2 д�벿���ռ�����˨��׼��װ����������Ϣ
	CHashStrList<bool>hashBoltWarnings;
	for(parti=tower_bolt_n;parti<arrBoltPtr.Size();parti++)
	{	//�����ռ���ÿ����˨��׼��װ����������Ϣ��¼�̶�����64 Byte
		CLDSBolt* pBolt=(CLDSBolt*)arrBoltPtr[parti].m_pBolt;
		//6.2.2.1 д����˨���ϵ��������
		LS_FAMILY_INFO* pFamilyInfo=hashLsFamilyById.GetValue(pBolt->m_hFamily);
		if(pFamilyInfo==NULL)
		{
			CLsFamily* pFamily=CLsLibrary::DefaultFamily();
			pFamilyInfo=hashLsFamilyById.GetValue(pFamily->GetFamilyHandle());
		}
		pIO->WriteByte(pFamilyInfo->indexId);
		//6.2.2.2 д����˨�����ϵ���ڵ�������ʶ��
		CXhChar16 spec("M%dX%.0f",pBolt->get_d(),pBolt->get_L());
		WORD *pIndexId=pFamilyInfo->pHashLsIndexBySpec->GetValue(spec);
		if(pIndexId)
			pIO->WriteWord(*pIndexId);
		else
		{	//������ȱ����Ӧ���ʱӦ������˨ֱ����Ϊ������������ʶ������飬��λ��1 wjh-2016.1.25
			WORD d=(WORD)pBolt->get_d();
			WORD L=(WORD)min(255,pBolt->get_L());
			WORD K=0x8000|(d<<8)+L;
			pIO->WriteWord(K);
			if(hashBoltWarnings.GetValue(spec)==NULL)
			{
				hashBoltWarnings.Add(spec);
#ifdef AFX_TARG_ENU_ENGLISH
				CLDSObject::Log2File()->Log("Didn't find relative record from bolt spec library for Bolt spec %s,will loss spec information��",(char*)spec);
#else
				CLDSObject::Log2File()->Log("��˨���%sδ����Ӧ����˨���ϵ�����ҵ���Ӧ��¼������ʧ�����Ϣ��",(char*)spec);
#endif
			}
		}
		//6.2.2.3 д����˨����
		pIO->WriteFloat((float)atof(pBolt->Grade()));
		//6.2.2.4 д��װ��ԭ�㼰��˨�������߷���
		pIO->WritePoint(pBolt->ucs.origin);
		pIO->WritePoint(pBolt->ucs.axis_z);
		//6.2.2.5 д����˨ͨ�񳤶�
		pIO->WriteWord((WORD)pBolt->get_L0());
		//6.2.2.6 д������Ҫ���ʶ�ֽڣ�ĿǰĩλΪ����ʾ������˨������λ������
		if(pBolt->IsAntiTheft())
			pIO->WriteByte(1);
		else
			pIO->WriteByte(0);
		//6.2.2.7 д���Ȧ��Ϣ
		pIO->WriteByte((BYTE)pBolt->DianQuan.N);	//��Ȧ����
		pIO->WriteByte((BYTE)pBolt->DianQuan.thick);//����Ȧ���
		pIO->WriteWord((WORD)ftoi(pBolt->DianQuan.offset));//��ʾ��Ȧ����˨��λ�����ƫ����
		//6.2.2.8 д��װ��ԭ�㼰��˨�������߷���
		WORD blocki=hashBlockById[arrBoltPtr[parti].m_hBlock];
		pIO->WriteWord(blocki);	//
	}
	//6.2.3  д�����ģ�Ϳռ�����˨��׼��װ����������Ϣ
	for(parti=0;parti<tower_bolt_n;parti++)
	{	//����ģ�Ϳռ���ÿ����˨��׼��װ����������Ϣ��¼�̶�����92 Byte
		CLDSBolt* pBolt=(CLDSBolt*)arrBoltPtr[parti].m_pBolt;
		//6.2.3.1 д����˨���ϵ��������
		if(pBolt->m_hFamily==0&&pBolt->get_d()>24)
			pBolt->m_hFamily=4;	//TODO:�����ƣ�����Ϊ������˨
		LS_FAMILY_INFO* pFamilyInfo=hashLsFamilyById.GetValue(pBolt->m_hFamily);
		if(pFamilyInfo==NULL)
		{
			CLsFamily* pFamily=CLsLibrary::DefaultFamily();
			pFamilyInfo=hashLsFamilyById.GetValue(pFamily->GetFamilyHandle());
		}
		pIO->WriteByte(pFamilyInfo->indexId);
		//6.2.3.2 д����˨�����ϵ���ڵ�������ʶ��
		CXhChar16 spec("M%dX%.0f",pBolt->get_d(),pBolt->get_L());
		WORD *pIndexId=pFamilyInfo->pHashLsIndexBySpec->GetValue(spec);
		if(pIndexId)
			pIO->WriteWord(*pIndexId);
		else
		{	//������ȱ����Ӧ���ʱӦ������˨ֱ����Ϊ������������ʶ������飬��λ��1 wjh-2016.1.25
			WORD d=(WORD)pBolt->get_d();
			WORD L=(WORD)min(255,pBolt->get_L());
			WORD K=0x8000|(d<<8)+L;
			pIO->WriteWord(K);
			if(hashBoltWarnings.GetValue(spec)==NULL)
			{
				hashBoltWarnings.Add(spec);
#ifdef AFX_TARG_ENU_ENGLISH
				CLDSObject::Log2File()->Log("Didn't find relative record from bolt spec library for Bolt spec %s,will loss spec information��",(char*)spec);
#else
				CLDSObject::Log2File()->Log("��˨���%sδ����Ӧ����˨���ϵ�����ҵ���Ӧ��¼������ʧ�����Ϣ��",(char*)spec);
#endif
			}
		}
		//6.2.3.3 д����˨����
		pIO->WriteFloat((float)atof(pBolt->Grade()));
		//6.2.3.4 д��װ��ԭ�㼰��˨�������߷���
		pIO->WritePoint(pBolt->ucs.origin);
		pIO->WritePoint(pBolt->ucs.axis_z);
		//6.2.3.5 д����˨ͨ�񳤶�
		pIO->WriteWord((WORD)pBolt->get_L0());
		//6.2.3.6 д������Ҫ���ʶ�ֽڣ�ĿǰĩλΪ����ʾ������˨������λ������
		if(pBolt->IsAntiTheft())
			pIO->WriteByte(1);
		else
			pIO->WriteByte(0);
		//6.2.3.7 д���Ȧ��Ϣ
		pIO->WriteByte((BYTE)pBolt->DianQuan.N);	//��Ȧ����
		pIO->WriteByte((BYTE)pBolt->DianQuan.thick);//����Ȧ���
		pIO->WriteWord((WORD)ftoi(pBolt->DianQuan.offset));//��ʾ��Ȧ����˨��λ�����ƫ����
		//6.2.3.8 д��ͳ������:
		// ���λΪ����ʾ��˨���������������ֽ�������κţ�
		// ���λΪ����ʾ��˨ͬʱ��������κţ��κ�ͳ�Ʒ�Χ���ַ�����ʽ�洢�ں��������ֽ���ָ��ַ�У�
		// �θ�λΪ����ʾ�Ƿ�Ϊ��������˨��
		// ����ͣ�λ��ʾ�������ȵ����޺ţ���Ϊ����ʾ�����������塣
		BYTE cStatFlag=0;
		if(strlen(pBolt->statSegStr)>0)
			cStatFlag|=0x80;	//���λ��1��ʾ����ָ���κ�ͳ�Ʒ�Χ
		if(pBolt->IsLegObj())
		{
			cStatFlag|=0x40;	//�θ�λ��1��ʾΪ�����ϵ���˨
			char cQuad=min(63,pBolt->layer(2)-'0');
			cStatFlag|=cQuad;	//ĩβ6λ��ʾ��������
		}
		pIO->WriteByte(cStatFlag);
		//6.2.3.9 д������κ�
		pIO->WriteInteger(pBolt->iSeg);
		//6.2.3.10 д��������޽��Ⱥż���ĺ�
		char cQuad=(pBolt->IsLegObj())?pBolt->ciLegQuad:0;
		pIO->WriteByte(cQuad);
		pIO->Write(&pBolt->cfgword,24);
	}
	//6.2.4 д�뻻�ȴ�������˨�Ķκ�ͳ�Ʒ�Χ�ַ����洢��
	for(parti=0;parti<tower_bolt_n;parti++)
	{	//����ģ�Ϳռ���ÿ����˨��׼��װ����������Ϣ��¼�̶�����92 Byte
		CLDSBolt *pBolt=(CLDSBolt*)arrBoltPtr[parti].m_pBolt;
		if(strlen(pBolt->statSegStr)<=0)
			continue;
		current_pos=pIO->GetCursorPosition();
		pIO->SeekPosition(bolt_assemble_pos+12+parti*92+63);
		pIO->WriteDword(current_pos);
		pIO->SeekPosition(current_pos);
		pIO->WriteString(pBolt->statSegStr);
	}
	//���±�������ռ�洢�ռ��С
	current_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(bolt_assemble_pos); 
	pIO->WriteDword(current_pos-bolt_assemble_pos-4);
	pIO->SeekPosition(current_pos);
	//6.3 ����װ�������
	//��һ�д���"DYN_ARRAY<CHashList<DWORD>>"��Ϊ"DYN_ARRAY<CHashList<DWORD> >"����ΪVS 2003�²�ʶ�𣬻������� wjh-2015.8.12
	//DYN_ARRAY<CHashList<DWORD> > holeId2BoltIndexArr(allPartList.GetNodeNum());	//��¼ÿ����������˨�׶�Ӧ����˨���� wht 15-07-19
	ARRAY_LIST<ASSEMBLE_PARTRECORD> listAssemParts(0,allPartList.GetNodeNum());	
	DWORD assemble_part_pos=pIO->GetCursorPosition();
	pIO->WriteDword(0);	//��ʼ��������ռ�õ������洢�ռ��С
	//6.3.1 д�벿��ģ�Ϳռ估����ģ�Ϳռ乹������
	pIO->WriteDword(0);		//д�벿��ģ�Ϳռ乹������
	pIO->WriteDword(0);		//д�����ģ�Ϳռ乹������
	pIO->WriteDword(0);		//д������˼����߼�¼���� wjh-2016.1.02
	CHashListEx<GROUPANGLEID> hashGroupAngleIds;
	//6.3.2����ģ�͹���
	DWORD assemble_blockpart_pos =pIO->GetCursorPosition();
	long nBlockPart=0,nTaPart=0;
	WORD* pBlockIndexId;
	CLDSPart::PEER_PART* pPeerPart;
	ASSEMBLE_PARTRECORD* pAssemPart;
	for(pLabelPart=hashLabelParts.GetFirst();pLabelPart;pLabelPart=hashLabelParts.GetNext())
	{
		if(pLabelPart->hBlock<=0||(pBlockIndexId=hashBlockById.GetValue(pLabelPart->hBlock))==NULL)
			continue;	//ģ�Ϳռ�Ĺ���
		CLDSPart* pDatumPart=pLabelPart->pPart;
		pDatumPart->DispatchHoleIndexId(&pLabelPart->hashHoleIdByDatumBoltH);//������˨���->�ݿ�������ʶ��ϣ��
		for(pPeerPart=pLabelPart->hashActualParts.GetFirst();pPeerPart;pPeerPart=pLabelPart->hashActualParts.GetNext())
		{
			nBlockPart++;
			pIO->WriteDword(pLabelPart->idPartIndex);	//Դ�����ڹ�����Ϣ�����еı�ʶ������
			//----VVVV----V1.4�������� wjh-2018.8.18
			pIO->Write(pPeerPart->pPart->layer(),3);	//ͼ���� 
			DWORD dwGroupAngleId=GROUPANGLEID::GetRelaGroupAngleId(pPeerPart->pPart,&hashGroupAngleIds);
			pIO->WriteDword(dwGroupAngleId);	//д�������ϽǸֱ�ʶ
			//----^^^^----V1.4�������� wjh-2018.8.18
			pIO->WriteDword(0);	//��Ӧ�˼������ڼ������ݷ����еı�ʶ�����š�wjh-2016.1.02
			pIO->WriteWord(*pBlockIndexId);	//���������Ĳ���ģ�Ϳռ�
			pAssemPart=listAssemParts.append();
			pAssemPart->pAssemblePart=pPeerPart->pPart;
			//CHashList<DWORD> *pHashBoltIndexByHoleId=&holeId2BoltIndexArr[nBlockPart-1];
			if(pPeerPart->hPeerPart==pDatumPart->handle)
			{	//��׼������˨����
				DWORD holeId=1;
				pAssemPart->pAssemblePart=pDatumPart;
				for(long* pHoleId=pLabelPart->hashHoleIdByDatumBoltH.GetFirst();pHoleId;pHoleId=pLabelPart->hashHoleIdByDatumBoltH.GetNext())
				{
					long hBolt=pLabelPart->hashHoleIdByDatumBoltH.GetCursorKey();
					DWORD *pBoltIndexId=NULL;
					pBoltIndexId=hashBoltIndexByHandle.GetValue(0,hBolt);
					int boltIndexId=pBoltIndexId!=NULL ? (*pBoltIndexId ): 0;
					pAssemPart->hashBoltIndexIdByHoleId.SetValue(holeId,boltIndexId);
					pLabelPart->hashBoltIndexIdByHoleId.SetValue(holeId,boltIndexId);
					holeId++;
				}
			}
			else //if(pPart!=pDatumPart)
			{	//ͬ��������������˨���ã���Ҫ������˨λ�ò�����˨�׶�Ӧ����˨����
				for(CLDSPart::PEER_BOLT* pPeerBolt=pPeerPart->hashBoltsByBase.GetFirst();pPeerBolt;pPeerBolt=pPeerPart->hashBoltsByBase.GetNext())
				{
					long* pHoleId=pLabelPart->hashHoleIdByDatumBoltH.GetValue(pPeerBolt->hBaseBolt);
					if(pHoleId==NULL)
						continue;
					DWORD *pBoltIndexId = hashBoltIndexByHandle.GetValue(pLabelPart->hBlock,pPeerBolt->hPeerBolt);
					long boltIndexId=pBoltIndexId!=NULL ? (*pBoltIndexId ): 0;
					pAssemPart->hashBoltIndexIdByHoleId.SetValue(*pHoleId,boltIndexId);
				}
			}
			//���㹹��װ������ϵʱ���Ǹ˼���ͷ���ְ巨��ƫ����
			pIO->WritePoint(pPeerPart->csPeer.origin);
			pIO->WritePoint(pPeerPart->csPeer.axis_x);
			pIO->WritePoint(pPeerPart->csPeer.axis_y);
			pIO->WritePoint(pPeerPart->csPeer.axis_z);
			pIO->WriteDword(0);	//��ʼ����˨������˨��¼��Ӧ��ϵ�ڴ��ַ
		}
	}
	current_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(assemble_part_pos+4); 
	pIO->WriteInteger(nBlockPart);
	pIO->SeekPosition(current_pos);
	//6.3.3����ģ�͹���
	const int HOLE_POS_EPS = 2;
	DWORD assemble_modelpart_pos=pIO->GetCursorPosition();
	for(pLabelPart=hashLabelParts.GetFirst();pLabelPart;pLabelPart=hashLabelParts.GetNext())
	{
		if(pLabelPart->hBlock>0&&(pBlockIndexId=hashBlockById.GetValue(pLabelPart->hBlock))!=NULL)
			continue;	//�����ռ�Ĺ���
		CLDSPart* pDatumPart=pLabelPart->pPart;
		pDatumPart->DispatchHoleIndexId(&pLabelPart->hashHoleIdByDatumBoltH);//������˨���->�ݿ�������ʶ��ϣ��
		for(pPeerPart=pLabelPart->hashActualParts.GetFirst();pPeerPart;pPeerPart=pLabelPart->hashActualParts.GetNext())
		{
			nTaPart++;
			pPart=pPeerPart->pPart;
			pIO->WriteDword(pLabelPart->idPartIndex);	//Դ�����ڹ�����Ϣ�����еı�ʶ������	
			//----VVVV----V1.4�������� wjh-2018.8.18
			pIO->Write(pPart->layer(),3);	//ͼ���� 
			DWORD dwGroupAngleId=GROUPANGLEID::GetRelaGroupAngleId(pPart,&hashGroupAngleIds);
			pIO->WriteDword(dwGroupAngleId);	//д�������ϽǸֱ�ʶ
			//----^^^^----V1.4�������� wjh-2018.8.18
			pIO->WriteDword(0);	//��Ӧ�˼������ڼ������ݷ����еı�ʶ�����š�wjh-2016.1.02
			BYTE cLegQuad=(pPart->IsLegObj())?pPart->ciLegQuad:0;
			pIO->WriteByte(cLegQuad);
			pIO->Write(&pPart->cfgword,24);
			pAssemPart=listAssemParts.append();
			pAssemPart->pAssemblePart=pPart;
			//��¼��˨��id��Ӧ����˨index
			//CHashList<DWORD> *pHashBoltIndexByHoleId=&holeId2BoltIndexArr[nBlockPart+nTaPart-1];
			if(pPart==pDatumPart)
			{	//��׼������˨����
				for(long* pHoleId=pLabelPart->hashHoleIdByDatumBoltH.GetFirst();pHoleId;pHoleId=pLabelPart->hashHoleIdByDatumBoltH.GetNext())
				{
					long hBolt=pLabelPart->hashHoleIdByDatumBoltH.GetCursorKey();
					DWORD *pBoltIndexId=NULL;
					pBoltIndexId=hashBoltIndexByHandle.GetValue(0,hBolt);
					int boltIndexId=pBoltIndexId!=NULL ? (*pBoltIndexId ): 0;
					pAssemPart->hashBoltIndexIdByHoleId.SetValue(*pHoleId,boltIndexId);
					pLabelPart->hashBoltIndexIdByHoleId.SetValue(*pHoleId,boltIndexId);
				}
			}
			else //if(pPart!=pDatumPart)
			{	//ͬ��������������˨���ã���Ҫ������˨λ�ò�����˨�׶�Ӧ����˨����
				for(CLDSPart::PEER_BOLT* pPeerBolt=pPeerPart->hashBoltsByBase.GetFirst();pPeerBolt;pPeerBolt=pPeerPart->hashBoltsByBase.GetNext())
				{
					long* pHoleId=pLabelPart->hashHoleIdByDatumBoltH.GetValue(pPeerBolt->hBaseBolt);
					if(pHoleId==NULL)
						continue;
					DWORD *pBoltIndexId = hashBoltIndexByHandle.GetValue(pLabelPart->hBlock,pPeerBolt->hPeerBolt);
					long boltIndexId=pBoltIndexId!=NULL ? (*pBoltIndexId ): 0;
					pAssemPart->hashBoltIndexIdByHoleId.SetValue(*pHoleId,boltIndexId);
				}
			}
			pIO->WritePoint(pPeerPart->csPeer.origin);
			pIO->WritePoint(pPeerPart->csPeer.axis_x);
			pIO->WritePoint(pPeerPart->csPeer.axis_y);
			pIO->WritePoint(pPeerPart->csPeer.axis_z);
			pIO->WriteDword(0);	//��ʼ����˨������˨��¼��Ӧ��ϵ�ڴ��ַ
		}
	}
	current_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(assemble_part_pos+8); 
	pIO->WriteDword(nTaPart);
	pIO->SeekPosition(current_pos);
	//6.3.4��˨Ӱ����ϢpAssemPart=listAssemParts
	long i;
	for(i=0;i<listAssemParts.GetSize();i++)
	{
		CHashList<DWORD> *pHashBoltIndexByHoleId=&listAssemParts[i].hashBoltIndexIdByHoleId;
		if(pHashBoltIndexByHoleId->GetNodeNum()<=0)
			continue;
		current_pos=pIO->GetCursorPosition();
		int nV14Offset=7;
		if(i>=nBlockPart)
			pIO->SeekPosition((assemble_modelpart_pos+(i-nBlockPart)*(129+nV14Offset+4)+129+nV14Offset));
		else
			pIO->SeekPosition((assemble_blockpart_pos+i*(106+nV14Offset+4)+106+nV14Offset));
		pIO->WriteDword(current_pos);
		pIO->SeekPosition(current_pos);
		pIO->WriteDword(pHashBoltIndexByHoleId->GetNodeNum());
		for(DWORD *pBoltIndex=pHashBoltIndexByHoleId->GetFirst();pBoltIndex;pBoltIndex=pHashBoltIndexByHoleId->GetNext())
		{
			pIO->WriteDword(pHashBoltIndexByHoleId->GetCursorKey());
			pIO->WriteDword(*pBoltIndex);
		}
	}
	//6.3.5�����˼��������ݼ�¼��wjh-2016.1.06
	long nBriefLine=0;
	for(i=0;i<listAssemParts.GetSize();i++)
	{
		if(!listAssemParts[i].pAssemblePart->IsLinePart())
			continue;
		CSuperSmartPtr<CLDSLinePart> pRod=(CLDSLinePart*)listAssemParts[i].pAssemblePart;
		CLDSNode* pStart=pRod->pStart;
		CLDSNode* pEnd  =pRod->pEnd;
		if(pStart==NULL||pEnd==NULL)
			continue;
		nBriefLine++;
		current_pos=pIO->GetCursorPosition();
		int nV14Offset=7;
		if(i>=nBlockPart)
			pIO->SeekPosition((assemble_modelpart_pos+(i-nBlockPart)*(129+4+nV14Offset)+4+nV14Offset));
		else
			pIO->SeekPosition((assemble_blockpart_pos+i*(106+4+nV14Offset)+4+nV14Offset));
		pIO->WriteDword(current_pos);
		pIO->SeekPosition(current_pos);
		pIO->WritePoint(pStart->Position(false));
		pIO->WritePoint(pEnd->Position(false));
		//���Ӷ˽ڵ�Ĺ���������ʶ V1.4�������� 
		pIO->WriteDword(pStart->feature);
		pIO->WriteDword(pEnd->feature);
		//��Ӹ˼�ʼĩ�˽ڵ���������ͣ��Ա����ģ���п����Զ���ȡģ�ͳߴ��ע V1.2�������� ��ȡ�˼�ʼ�ն˽ڵ����ͣ�wht 16-12-01
		if(pRod->IsAngle())
		{
			if(pRod.pAngle->IsLappedAnchorStart())
				pIO->WriteByte(0);
			else
				pIO->WriteByte(ToNodeCoordTypeFlag(pStart));	//ʼ�˽ڵ���������
			if(pRod.pAngle->IsLappedAnchorEnd())
				pIO->WriteByte(0);
			else
				pIO->WriteByte(ToNodeCoordTypeFlag(pEnd));		//�ն˽ڵ���������
		}
		if(pRod->IsTube())
		{
			if(pRod.pTube->desStartPos.endPosType==1||pRod.pTube->desStartPos.endPosType==3)
				pIO->WriteByte(0);	//��Ӷ�λ�ֹ�
			else
				pIO->WriteByte(ToNodeCoordTypeFlag(pStart));	//ʼ�˽ڵ���������
			if(pRod.pTube->desEndPos.endPosType==1||pRod.pTube->desEndPos.endPosType==3)
				pIO->WriteByte(0);
			else
				pIO->WriteByte(ToNodeCoordTypeFlag(pEnd));		//�ն˽ڵ���������
		}
		else
		{
			pIO->WriteByte(ToNodeCoordTypeFlag(pStart));	//ʼ�˽ڵ���������
			pIO->WriteByte(ToNodeCoordTypeFlag(pEnd));		//�ն˽ڵ���������
		}
	}
	current_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(assemble_part_pos+12); 
	pIO->WriteDword(nBriefLine);
	pIO->SeekPosition(current_pos);
	//
	current_pos=pIO->GetCursorPosition();
	pIO->SeekPosition(assemble_part_pos);
	pIO->WriteDword(current_pos-assemble_part_pos-4);
	pIO->SeekPosition(sect_start_pos);
	pIO->WriteDword(current_pos-sect_start_pos-4);
	pIO->SeekToEnd();
	if(DisplayProcess)
		DisplayProcess(100,progress.title);
	//����װ���¼��Ӧ�Ĺ�������������У��ʱ��Ҫ wht 16-12-05
	if(pObjHandleIO)
	{
		pObjHandleIO->WriteInteger(hashLabelParts.GetNodeNum());
		for(pLabelPart=hashLabelParts.GetFirst();pLabelPart;pLabelPart=hashLabelParts.GetNext())
		{
			pObjHandleIO->WriteInteger(pLabelPart->idPartIndex);
			pObjHandleIO->WriteInteger(pLabelPart->hashActualParts.GetNodeNum());
			for(pPeerPart=pLabelPart->hashActualParts.GetFirst();pPeerPart;pPeerPart=pLabelPart->hashActualParts.GetNext())
				pObjHandleIO->WriteInteger(pPeerPart->pPart->handle);
		}
	}
}
#ifdef __LDS_CONTEXT_
#include "TempFile.h"
#endif
bool CTower::ExportTowerSolidDataExchangeFile(char *file_name)
{	
	FILE* fp=fopen(file_name,"wb");
	if(fp==NULL)
		return false;
#ifdef __LDS_CONTEXT_
	DYN_ARRAY<char>BUFF_POOL(0x00100000);	//1M����
	setvbuf(fp,BUFF_POOL,_IOFBF,0x00100000);
	CTempFileBuffer buffer(fp);	//Ĭ���ڴ�����ݶȴ�СΪ50M����С����Ƶ�������ڴ�ᵼ�º���
	ToTowerSolidDataExchangeBuffer(&buffer);
#else
	CBuffer buffer(50000000);
	ToTowerSolidDataExchangeBuffer(&buffer);
	//////////////////////////////////////////////////////////////////
	//���������ɵĸ���ʵ�����ݻ���д�뵽ָ���ļ���
	//1.1 �ļ��汾��Ϣ
	fwrite(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
#endif
	fclose(fp);
	return true;
}
#endif
CLDSAnchorBolt* CTower::FromAnchorBolt(long hAnchorBolt)
{
	return (CLDSAnchorBolt*)Parts.FromHandle(hAnchorBolt,CLS_ANCHORBOLT);
}
CLDSAnchorBolt* CTower::AppendAnchorBolt()
{
	return (CLDSAnchorBolt*)Parts.append(CLS_ANCHORBOLT);
}

BOOL CTower::IsSonPart(CLDSDbObject *pObj,CFGWORD *pBodyWord/*=NULL*/,CFGWORD* pLegWord/*=NULL*/)
{
	if(pObj->IsLegObj()&&pLegWord!=NULL&&pObj->cfgword.And(*pLegWord))
		return TRUE;
	else if(pObj->IsBodyObj()&&pBodyWord!=NULL&&pObj->cfgword.And(*pBodyWord))	//�����ͷ���˼�
		return TRUE;
	else if(pObj->IsBodyObj()&&pLegWord)
		return pObj->cfgword.And(pLegWord->ToBodyWord());
	else
		return FALSE;
}
//����װ��ʱ����λ��ƥ��ڵ�
CLDSNode* CTower::MatchExterNode(const double* nodecoords)
{
	BOOL pushed=Node.push_stack();
	for(CLDSNode* pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		if(pNode->xPreliftPos.IsEqual(nodecoords,0.1))
		{
			Node.pop_stack(pushed);
			return pNode;
		}
	}
	Node.pop_stack(pushed);
	return NULL;
}
CLDSLinePart* CTower::MatchExterRod(int idClsType,BYTE cbValidProp,const double* xStartPosCoords,const double* xEndPosCoords,
	const double* vxWingNorm/*=NULL*/,const double* vyWingNorm/*=NULL*/)
{
	static const BYTE PROP_START_NODEPOS= 1;	//ʼ�˽ڵ�����
	static const BYTE PROP_START_TIPPOS = 2;	//ʼ�˶��涥������(�Ǹ���㣬�ֹ����߶˵㣩
	static const BYTE PROP_END_NODEPOS	= 3;	//�ն˽ڵ�����
	static const BYTE PROP_END_TIPPOS	= 4;	//�ն˶��涥������(�Ǹ���㣬�ֹ����߶˵㣩
	static const BYTE PROP_WING_NORM	= 5;	//�Ǹ���֫����
	double tolerance=0.1;
	BYTEFLAG flag=cbValidProp;
	CLdsPartListStack stack(this);
	for(SmartPartPtr pSmartPart=Parts.GetFirst();pSmartPart.IsHasPtr();pSmartPart=Parts.GetNext())
	{
		if(!pSmartPart->IsLinePart())
			continue;
		if( flag.GetBitState(PROP_START_NODEPOS)&&(pSmartPart.pRod->pStart==NULL||
			!pSmartPart.pRod->pStart->xPreliftPos.IsEqual(xStartPosCoords,tolerance)))
			continue;
		else if(flag.GetBitState(PROP_START_TIPPOS)&&!pSmartPart.pRod->Start().IsEqual(xStartPosCoords,tolerance))
			continue;
		if( flag.GetBitState(PROP_END_NODEPOS)&&(pSmartPart.pRod->pEnd==NULL||
			!pSmartPart.pRod->pEnd->xPreliftPos.IsEqual(xEndPosCoords,tolerance)))
			continue;
		else if(flag.GetBitState(PROP_END_TIPPOS)&&!pSmartPart.pRod->Start().IsEqual(xEndPosCoords,tolerance))
			continue;
		if(pSmartPart->GetClassTypeId()!=idClsType)
			continue;
		if( flag.GetBitState(PROP_WING_NORM)&&
			(pSmartPart.pAngle->vxWingNorm*GEPOINT(vxWingNorm)<EPS_COS2||pSmartPart.pAngle->vyWingNorm*GEPOINT(vyWingNorm)<EPS_COS2))
			continue;
		return pSmartPart.pRod;
	}
	return NULL;
}
CLDSBolt* CTower::MatchExterBolt(WORD d,WORD wL,const double* xBasePosition,const double* vWorkNorm)
{
	CLdsPartListStack stack(this);
	GEPOINT xOrg(xBasePosition),vNormal(vWorkNorm);
	for(CLDSBolt* pBolt=(CLDSBolt*)Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Parts.GetNext(CLS_BOLT))
	{
		if(fabs(pBolt->ucs.axis_z*vNormal)<EPS_COS2)
			continue;	//���߲�һ��
		GEPOINT vDist=GEPOINT(pBolt->ucs.origin)-xOrg;
		double deltaL=vDist*vNormal;
		GEPOINT vhDist=(f3dPoint)vDist-deltaL*vNormal;
		double deltaH=vhDist.mod();
		if(deltaL>wL)
			continue;
		if(deltaH>1)
			continue;
		return pBolt;
	}
	return NULL;
}
//����Ƿ���ָ��(��ǰ)ģ���д����ص��Ľڵ�
CLDSNode* CTower::CheckOverlappedNode(f3dPoint pos,const CFGINFO& cfginfo,bool bFromStart/*=true*/,long hIgnoreHandle/*=0*/)
{
	CLDSNode *pNode=NULL;
	if(bFromStart)
		pNode=Node.GetFirst();
	else
		pNode=Node.GetNext();
	for(;pNode;pNode=Node.GetNext())
	{
		if(!pNode->ModuleTogetherWith(cfginfo))
			continue;	//�뵱ǰ�ڵ㲻������ͬһģ��
		if(!pos.IsEqual(pNode->xPreliftPos))
			continue;
		if(pNode->handle==hIgnoreHandle)
			continue;	//����ָ���ڵ���
		return pNode;
	}
	return NULL;
}
CLDSNode* CTower::CheckOverlappedNode(f3dPoint pos,CLDSModule* pInModule/*=NULL*/,
	CFGWORD* pBodyWord/*=NULL*/,CFGWORD* pLegWord/*=NULL*/)
{
	CAtomListStack<CLDSNode> stack(&Node);
	for(CLDSNode *pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		if(pBodyWord==NULL&&pLegWord==NULL)
		{
			if(pInModule&&!pInModule->IsSonPart(pNode))
				continue;	//���ǵ�ǰָ��ģ��
			else if(!pNode->cfgword.And(GetDefaultCfgPartNo()))
				continue;
		}
		else if(!IsSonPart(pNode,pBodyWord,pLegWord))
			continue;
		if(!pos.IsEqual(pNode->Position(false)))
			continue;
		//pLdsNode->SetModified();	//���ں���������޸Ľڵ�����,�����趨�޸ı��,����Undo��ʧ�� wjh 2011.8.12
		return pNode;
	}
	return NULL;
}
CLDSLinePart* CTower::CheckOverLappedLinePart(f3dPoint start,f3dPoint end,const CFGINFO& cfginfo,long hIgnoreHandle/*=0*/,
												const double* vxWingNorm/*=NULL*/,const double* vyWingNorm/*=NULL*/)
{
	CPtInLineCheck lineCheck(start,end);
	for(CLDSLinePart *pLinePart=Parts.GetFirstLinePart();pLinePart;pLinePart=Parts.GetNextLinePart())
	{
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;
		if(!pLinePart->ModuleTogetherWith(cfginfo))
			continue;	//�뵱ǰ�˼���������ͬһģ��
		if(pLinePart->handle==hIgnoreHandle)
			continue;	//����ָ���˼����
		if(pLinePart->IsAngle()&&vxWingNorm&&vyWingNorm)
		{
			CLDSLineAngle* pAngle=(CLDSLineAngle*)pLinePart;
			if(pAngle->vxWingNorm*GEPOINT(vxWingNorm)<EPS_COS2||pAngle->vyWingNorm*GEPOINT(vyWingNorm)<EPS_COS2)
				continue;	//�Ǹ�֫����ͬ
		}
		CPtInLineCheck lineCheck2(pLinePart->pStart->Position(false),pLinePart->pEnd->Position(false));
		double scale_start,scale_end;
		if(lineCheck.Length()>lineCheck2.Length())
		{
			scale_start=lineCheck.CheckPoint(pLinePart->pStart->Position(false));
			scale_end=lineCheck.CheckPoint(pLinePart->pEnd->Position(false));
		}
		else
		{
			scale_start=lineCheck2.CheckPoint(start);
			scale_end=lineCheck2.CheckPoint(end);
		}
		if((scale_start<0||scale_start>1)&&(scale_end<0||scale_end>1))
			continue;	//���˼����ص�
		if((scale_start>=0&&scale_start<=1)&&(scale_end>=0&&scale_end<=1))
			return pLinePart;
		else
		{	//�̸˼��볤�˼����ص�
			double dd=lineCheck.LineStdVec()*lineCheck2.LineStdVec();
			if(fabs(dd)<EPS_COS)
				continue;	//�����򲻿��ܹ���
			if(dd<0)
			{	//����ʱ��תʼĩ��
				double overlapp=scale_start;
				scale_start=scale_end;
				scale_end=overlapp;
			}
			if(scale_start<0)
				scale_start=0;
			if(scale_end>1)
				scale_end=1;
			if(scale_start<scale_end)
				return pLinePart;
		}
	}
	return NULL;
}
CLDSLinePart* CTower::CheckOverLappedLinePart(f3dPoint start, f3dPoint end,CLDSModule* pInModule/*=NULL*/,
	CFGWORD* pBodyWord/*=NULL*/,CFGWORD* pLegWord/*=NULL*/)
{
	CLdsPartListStack stack(&Parts);
	CPtInLineCheck lineCheck(start,end);
	for(CLDSLinePart *pLinePart=Parts.GetFirstLinePart();pLinePart;pLinePart=Parts.GetNextLinePart())
	{
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;
		if(pBodyWord!=NULL||pLegWord!=NULL)
		{
			if(!IsSonPart(pLinePart,pBodyWord,pLegWord))
				continue;
		}
		else if(pInModule&&!pInModule->IsSonPart(pLinePart))
			continue;	//���ǵ�ǰָ��ģ��
		CPtInLineCheck lineCheck2(pLinePart->pStart->Position(false),pLinePart->pEnd->Position(false));
		double scale_start,scale_end;
		if(lineCheck.Length()>lineCheck2.Length())
		{
			scale_start=lineCheck.CheckPoint(pLinePart->pStart->Position(false));
			scale_end=lineCheck.CheckPoint(pLinePart->pEnd->Position(false));
		}
		else
		{
			scale_start=lineCheck2.CheckPoint(start);
			scale_end=lineCheck2.CheckPoint(end);
		}
		if((scale_start<0||scale_start>1)&&(scale_end<0||scale_end>1))
			continue;	//���˼����ص�
		if((scale_start>=0&&scale_start<=1)&&(scale_end>=0&&scale_end<=1))
			return pLinePart;
		else
		{	//�̸˼��볤�˼����ص�
			double dd=lineCheck.LineStdVec()*lineCheck2.LineStdVec();
			if(fabs(dd)<EPS_COS)
				continue;	//�����򲻿��ܹ���
			if(dd<0)
			{	//����ʱ��תʼĩ��
				double overlapp=scale_start;
				scale_start=scale_end;
				scale_end=overlapp;
			}
			if(scale_start<0)
				scale_start=0;
			if(scale_end>1)
				scale_end=1;
			if(scale_start<scale_end)
				return pLinePart;
		}
	}
	return NULL;
}
CLDSLinePart* CTower::CheckOverLappedLinePart(f3dPoint start,f3dPoint end,CLDSDbObject* pTogetherLiveWith)
{
	CLdsPartListStack stack(&Parts);
	CPtInLineCheck lineCheck(start,end);
	for(CLDSLinePart *pLinePart=Parts.GetFirstLinePart();pLinePart;pLinePart=Parts.GetNextLinePart())
	{
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;
		if(!pTogetherLiveWith->ModuleTogetherWith(pLinePart))
			continue;	//���ǵ�ǰָ��ģ��
		CPtInLineCheck lineCheck2(pLinePart->pStart->Position(false),pLinePart->pEnd->Position(false));
		double scale_start,scale_end;
		if(lineCheck.Length()>lineCheck2.Length())
		{
			scale_start=lineCheck.CheckPoint(pLinePart->pStart->Position(false));
			scale_end=lineCheck.CheckPoint(pLinePart->pEnd->Position(false));
		}
		else
		{
			scale_start=lineCheck2.CheckPoint(start);
			scale_end=lineCheck2.CheckPoint(end);
		}
		if((scale_start<0||scale_start>1)&&(scale_end<0||scale_end>1))
			continue;	//���˼����ص�
		if((scale_start>=0&&scale_start<=1)&&(scale_end>=0&&scale_end<=1))
			return pLinePart;
		else
		{	//�̸˼��볤�˼����ص�
			double dd=lineCheck.LineStdVec()*lineCheck2.LineStdVec();
			if(fabs(dd)<EPS_COS)
				continue;	//�����򲻿��ܹ���
			if(dd<0)
			{	//����ʱ��תʼĩ��
				double overlapp=scale_start;
				scale_start=scale_end;
				scale_end=overlapp;
			}
			if(scale_start<0)
				scale_start=0;
			if(scale_end>1)
				scale_end=1;
			if(scale_start<scale_end)
				return pLinePart;
		}
	}
	return NULL;
}
CLDSBolt* CTower::CheckOverLappedBolt(const double* xBasePosition,const double* vWorkNorm,long hIgnoreHandle/*=0*/)
{
	GEPOINT xOrg(xBasePosition),vNormal(vWorkNorm);
	for(CLDSBolt* pBolt=(CLDSBolt*)Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Parts.GetNext(CLS_BOLT))
	{
		if(pBolt->handle==hIgnoreHandle)
			continue;
		if(fabs(pBolt->ucs.axis_z*vNormal)<EPS_COS2)
			continue;	//���߲�һ��
		GEPOINT vDist=GEPOINT(pBolt->ucs.origin)-xOrg;
		double deltaL=vDist*vNormal;
		if(fabs(deltaL)>pBolt->L)
			continue;
		f3dPoint intersPt;
		Int3dlf(intersPt,pBolt->ucs.origin,pBolt->ucs.axis_z,xOrg,vNormal);
		double deltaH=DISTANCE(intersPt,xOrg);
		if(deltaH>1)
			continue;
		return pBolt;
	}
	return NULL;
}
//�ϲ��Ǹ�ԭ��ģ���нǸֱ�����λ��Ϣ��ͨ�������Ǹָ���ģ�ͽǸֵĻ�����Ϣ(���ʡ����ŵ�)
BOOL CTower::IntelliMergeLineAngles(CLDSLineAngle* pModelJg,CLDSLineAngle* pBlockJg)
{
	if(pModelJg==NULL || pBlockJg==NULL)
		return FALSE;
	//���»�����Ϣ
	pModelJg->SetLayer(pBlockJg->layer());
	pModelJg->iSeg=pBlockJg->iSeg;
	pModelJg->cfgword=pBlockJg->cfgword;
	pModelJg->m_bVirtualPart=pBlockJg->m_bVirtualPart;
	pModelJg->cMaterial=pBlockJg->cMaterial;
	pModelJg->size_wide=pBlockJg->GetWidth();
	pModelJg->size_thick=pBlockJg->GetThick();
	pModelJg->SetPartNo(pBlockJg->GetPartNo());
	pModelJg->connectStart.CopyBasicBoltInfo(pBlockJg->connectStart);
	pModelJg->connectEnd.CopyBasicBoltInfo(pBlockJg->connectEnd);
	pModelJg->MirLayStyle=pBlockJg->MirLayStyle;
	pModelJg->m_bEnableTeG=pBlockJg->m_bEnableTeG;
	pModelJg->xWingXZhunJu=pBlockJg->xWingXZhunJu;
	pModelJg->xWingYZhunJu=pBlockJg->xWingYZhunJu;
	//������˨��Ϣ
	for(CLsRef* pLsRef=pBlockJg->GetFirstLsRef();pLsRef;pLsRef=pBlockJg->GetNextLsRef())
	{
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		CLDSBolt* pOverBolt=CheckOverLappedBolt(pBolt->ucs.origin,pBolt->ucs.axis_z,pBolt->handle);
		if(pOverBolt==NULL)	
			pModelJg->AppendMidLsRef(*pLsRef);
	}
	//�滻�Ǹ־����ɾ������Ǹ�
	ReplaceReferenceHandle(CHandleReplacer(pBlockJg->handle,pModelJg->handle));
	RemoveRelativeObjs(pBlockJg);
	Parts.DeleteNode(pBlockJg->handle);
	return TRUE;
}
//�ϲ���˨ԭ��ģ������˨������λ��Ϣ��ͨ��������˨����ģ����˨�Ļ�����Ϣ(���ʡ����ŵ�)
BOOL CTower::IntelliMergeBolt(CLDSBolt* pModelBolt,CLDSBolt* pBlockBolt)
{
	if(pModelBolt==NULL || pBlockBolt==NULL)
		return FALSE;
	pModelBolt->iSeg = pBlockBolt->iSeg;
	StrCopy(pModelBolt->statSegStr,pBlockBolt->statSegStr,24);
	pModelBolt->set_d(pBlockBolt->get_d());
	pModelBolt->set_L(pBlockBolt->get_L());
	pModelBolt->DianQuan = pBlockBolt->DianQuan;
	pModelBolt->SetGrade(pBlockBolt->Grade());
	pModelBolt->hole_d_increment = pBlockBolt->hole_d_increment;
	pModelBolt->dwRayNo = pBlockBolt->dwRayNo;
	pModelBolt->m_hFamily=pBlockBolt->m_hFamily;
	pModelBolt->m_cFuncType=pBlockBolt->m_cFuncType;
	//pModelBolt->m_nDisplayWasher = pBlockBolt->m_nDisplayWasher;
	pModelBolt->m_dwFlag=pBlockBolt->m_dwFlag;
	pModelBolt->m_bVirtualPart=pBlockBolt->m_bVirtualPart;
	//�滻��˨�����ɾ��������˨
	ReplaceReferenceHandle(CHandleReplacer(pBlockBolt->handle,pModelBolt->handle));
	RemoveRelativeObjs(pBlockBolt);
	Parts.DeleteNode(pBlockBolt->handle);
	return TRUE;
}

CParaCS *CTower::AppendAcs()
{
	//CObjNoGroup *pNoGroup=NoManager.FromGroupID(PARAMETRIC_CS_GROUP);
	//long id=pNoGroup->EnumIdleNo();
	//pNoGroup->SetNoState(id);
	//CParaCS *pAssembleCS=Acs.Add(id);
	//pAssembleCS->id=id;
	//return pAssembleCS;
	return Acs.Add(0);
}
BOOL CTower::DeleteAcs(long hAcs)
{
	//CObjNoGroup *pNoGroup=NoManager.FromGroupID(PARAMETRIC_CS_GROUP);
	//pNoGroup->SetNoState(hAcs,FALSE);
	return Acs.DeleteNode(hAcs);
}
CParaCS *CTower::FromAcsHandle(long hAcs)
{
	if(hAcs==1)
		return &defaultAcsMirX;
	else if(hAcs==2)
		return &defaultAcsMirY;
	else if(hAcs==3)
		return &defaultAcsMirZ;
	else
		return Acs.GetValue(hAcs);
}

static BOOL CreateLTMAVersionProcess(char* version_convert_exefile, HANDLE hClientPipeRead, HANDLE hClientPipeWrite )
{
	TCHAR cmd_str[MAX_PATH];//="D:\\Structure\\StruTower\\LTMAVersion\\Debug\\";
	CXhString szCmdStr(cmd_str,MAX_PATH);
	szCmdStr.Copy(version_convert_exefile);
	//GetSysPath(cmd_str);
	szCmdStr.Append(" -child",0);

	STARTUPINFO startInfo;
	memset( &startInfo, 0 , sizeof( STARTUPINFO));
	startInfo.cb= sizeof( STARTUPINFO );
	startInfo.dwFlags |= STARTF_USESTDHANDLES;
	startInfo.hStdError= 0;//hPipeWrite;
	startInfo.hStdInput= hClientPipeRead;
	startInfo.hStdOutput= hClientPipeWrite;

	PROCESS_INFORMATION processInfo;
	memset( &processInfo, 0, sizeof(PROCESS_INFORMATION) );

	BOOL b=CreateProcess( NULL,cmd_str,
		NULL,NULL, TRUE,CREATE_NEW_CONSOLE, NULL, NULL,&startInfo,&processInfo );
	DWORD er=GetLastError();
	/*if( b )
	{
		CloseHandle( processInfo.hProcess );
		CloseHandle( processInfo.hThread );
	}*/
	return b;
}
//�������������ܵ�д����
static BOOL WriteBufferToClient(HANDLE hPipeWrite,CBuffer &buffer)
{
	if( hPipeWrite == INVALID_HANDLE_VALUE )
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Invalid get pipe handle\n");
#else
		AfxMessageBox("��ȡ�ܵ������Ч\n");
#endif
		return FALSE;
	}
	//�������ܵ���д����
	return buffer.WriteToPipe(hPipeWrite,10000000);
}
//�˺���Ŀ�����ڷ�ֹ����������ʱ���ֲ��м����ȷ�Ϻ��ٽ��У�������ܻ�һ����ͨ���ܵ�����̫�����ݵ���
//�ܵ��������(Ŀǰֻ�ǲ²⣬��ȷ���Ƿ���������Ŀǰ�ѷ���Release��������ܵ�д��������ݻᵼ�����޵ȴ��� wjh 2013.5.24
static BOOL ReceiveACKFromClient(HANDLE hPipeRead)
{
	if( hPipeRead == INVALID_HANDLE_VALUE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Invalid get pipe handle\n");
#else
		AfxMessageBox("��ȡ�ܵ������Ч\n");
#endif
		return FALSE;
	}
	//�������ܵ��ж�ȡ����	
	CBuffer buffer(4);
	buffer.ReadFromPipe(hPipeRead,4);
	if(buffer.GetLength()<=0)
		return FALSE;
	DWORD ack=0;
	buffer.SeekPosition(0);
	buffer.ReadDword(&ack);
	return ack>0;
}

//��������������ܵ��е�����
static BOOL ReceiveFromClientProcess(HANDLE hPipeRead,CBuffer &buffer,BOOL &bSuccVersion)
{
	if( hPipeRead == INVALID_HANDLE_VALUE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Invalid get pipe handle\n");
#else
		AfxMessageBox("��ȡ�ܵ������Ч\n");
#endif
		return FALSE;
	}
	//�������ܵ��ж�ȡ����	
	buffer.ReadFromPipe(hPipeRead,10000000);
	if(buffer.GetLength()<=0)
		return FALSE;
	if (buffer.GetLength()==sizeof(BOOL))	//ת��ʧ��ʱֻ����һ��BOOLֵ
		buffer.ReadInteger(&bSuccVersion);
	else
		bSuccVersion=TRUE;
	buffer.SeekToBegin();
	return TRUE;
}

static bool GetFileVerConvertClientVersion(const char* version_convert_exefile,char* pszVersion, UINT uiMaxStrBufLen)
{
	VS_FIXEDFILEINFO *pVsInfo;
	unsigned int iFileInfoSize = sizeof(VS_FIXEDFILEINFO);
	DWORD dwVerInfoSize = GetFileVersionInfoSize(CXhChar500(version_convert_exefile), NULL);
	if (dwVerInfoSize == 0)
		return 0;
	CHAR_ARRAY bufpool(dwVerInfoSize + 1);
	CXhString szVersion(pszVersion, uiMaxStrBufLen);
	if (GetFileVersionInfo(CXhChar500(version_convert_exefile), 0, dwVerInfoSize, (char*)bufpool))
	{
		if (VerQueryValue(bufpool, "\\", (void **)&pVsInfo, &iFileInfoSize))
		{
			szVersion.Printf("%d.%d.%d.%d", HIWORD(pVsInfo->dwProductVersionMS), LOWORD(pVsInfo->dwProductVersionMS), HIWORD(pVsInfo->dwProductVersionLS), LOWORD(pVsInfo->dwProductVersionLS));
			return true;
		}
	}
	return false;
}
BOOL VersionOldLTMAFile(char* lic_file,char* version_convert_exefile,CBuffer &buffer)
{
	//step1:������һ���ܵ�: ���ڷ���������ͻ��˷�������
	SECURITY_ATTRIBUTES attrib;
	attrib.nLength = sizeof( SECURITY_ATTRIBUTES );
	attrib.bInheritHandle= true;
	attrib.lpSecurityDescriptor = NULL;
	HANDLE hPipeClientRead=NULL, hPipeSrcWrite=NULL;
	if( !CreatePipe( &hPipeClientRead, &hPipeSrcWrite, &attrib, 0 ) )
		return FALSE;
	//��Ϊ��ʱ����������ɼ̳У��ʿɸ���һ�����ɼ̳е�д�������ԭ����д����ص�
	HANDLE hPipeSrcWriteDup=NULL;
	if( !DuplicateHandle( GetCurrentProcess(), hPipeSrcWrite, GetCurrentProcess(), &hPipeSrcWriteDup, 0, false, DUPLICATE_SAME_ACCESS ) )
		return FALSE;
	CloseHandle( hPipeSrcWrite );
	//step2:�����ڶ����ܵ������ڿͻ�����������˷�������
	HANDLE hPipeClientWrite=NULL, hPipeSrcRead=NULL;
	if( !CreatePipe( &hPipeSrcRead, &hPipeClientWrite, &attrib, 0) )
		return FALSE;
	//���Ʋ��ɼ̳еĶ����������ԭ�еĶ�����ص�
	HANDLE hPipeSrcReadDup=NULL;
	if( !DuplicateHandle( GetCurrentProcess(), hPipeSrcRead, GetCurrentProcess(), &hPipeSrcReadDup, 0, false, DUPLICATE_SAME_ACCESS ) )
		return FALSE;
	CloseHandle( hPipeSrcRead );

	//step3:�����ӽ���,
	if( !CreateLTMAVersionProcess(version_convert_exefile, hPipeClientRead, hPipeClientWrite))
		return FALSE;
	CBuffer licbuffer;
	CXhChar50 szVersion;
	GetFileVerConvertClientVersion(version_convert_exefile, szVersion, 51);
	CXhChar200 szLicFilePath = lic_file;
	if (compareVersion(szVersion, "1.0.1.0") >= 0)
	{
		UINT uiProcessAuthNumer=GetCurrentProcessSerialNumber();
		szLicFilePath.Append(CXhChar16("%d", uiProcessAuthNumer), '|');
	}
	licbuffer.WriteString(szLicFilePath);
	WriteBufferToClient(hPipeSrcWriteDup,licbuffer);
	BOOL bSuccVersion=FALSE;
	if(ReceiveACKFromClient(hPipeSrcReadDup))
	{
		WriteBufferToClient(hPipeSrcWriteDup,buffer);
		ReceiveFromClientProcess(hPipeSrcReadDup,buffer,bSuccVersion);
	}
	return bSuccVersion;
}
//----------------------------------------------------------
//�����������ƶ���LDS.CPP��

/*
void ExportPropListHelpInfo(CBuffer& buffer)
{
	CXhChar100 m_defaultClsUrl;
	//���Ǹ�
	buffer.WriteInteger(CLS_LINEANGLE);
	buffer.WriteString("���Ǹ�");
	buffer.WriteString(CLDSLineAngle::HELPLINK_URL);
	buffer.WriteInteger(CLDSLineAngle::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM* pPropItem=CLDSLineAngle::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSLineAngle::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSLineAngle::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//��ϽǸ�
	buffer.WriteInteger(CLS_GROUPLINEANGLE);
	buffer.WriteString("��ϽǸ�");
	buffer.WriteString(CLDSGroupLineAngle::HELPLINK_URL);
	buffer.WriteInteger(CLDSGroupLineAngle::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM* pPropItem=CLDSGroupLineAngle::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSGroupLineAngle::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSGroupLineAngle::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//CLDSDbObject
	buffer.WriteInteger(CLS_DBOBJECT);
	buffer.WriteString("ͨ�����ݶ���");
	buffer.WriteString(CLDSDbObject::HELPLINK_URL);
	buffer.WriteInteger(CLDSDbObject::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSDbObject::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSDbObject::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSDbObject::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//����
	buffer.WriteInteger(CLS_PART);
	buffer.WriteString("ͨ�ù���");
	buffer.WriteString(CLDSPart::HELPLINK_URL);
	buffer.WriteInteger(CLDSPart::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSPart::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSPart::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSPart::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//��
	buffer.WriteInteger(CLS_SPHERE);
	buffer.WriteString("��");
	buffer.WriteString(CLDSSphere::HELPLINK_URL);
	buffer.WriteInteger(CLDSSphere::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSSphere::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSSphere::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSSphere::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//��˨
	buffer.WriteInteger(CLS_BOLT);
	buffer.WriteString("��˨");
	buffer.WriteString(CLDSBolt::HELPLINK_URL);
	buffer.WriteInteger(CLDSBolt::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSBolt::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSBolt::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSBolt::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//�ڵ�
	buffer.WriteInteger(CLS_NODE);
	buffer.WriteString("�ڵ�");
	buffer.WriteString(CLDSNode::HELPLINK_URL);
	buffer.WriteInteger(CLDSNode::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSNode::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSNode::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSNode::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//���θ˼�
	buffer.WriteInteger(CLS_LINEPART);
	buffer.WriteString("ֱ�߸˼�");
	buffer.WriteString(CLDSLinePart::HELPLINK_URL);
	buffer.WriteInteger(CLDSLinePart::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSLinePart::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSLinePart::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSLinePart::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//CLDSArcPart
	buffer.WriteInteger(CLS_ARCPART);
	buffer.WriteString("���θ˼�");
	buffer.WriteString(CLDSArcPart::HELPLINK_URL);
	buffer.WriteInteger(CLDSArcPart::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSArcPart::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSArcPart::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSArcPart::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//���νǸ�
	buffer.WriteInteger(CLS_ARCANGLE);
	buffer.WriteString("���νǸ�");
	buffer.WriteString(CLDSArcAngle::HELPLINK_URL);
	buffer.WriteInteger(CLDSArcAngle::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSArcAngle::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSArcAngle::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSArcAngle::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//���α��
	buffer.WriteInteger(CLS_ARCFLAT);
	buffer.WriteString("���α��");
	buffer.WriteString(CLDSArcFlat::HELPLINK_URL);
	buffer.WriteInteger(CLDSArcFlat::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSArcFlat::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSArcFlat::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSArcFlat::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//���β۸�
	buffer.WriteInteger(CLS_ARCSLOT);
	buffer.WriteString("���β۸�");
	buffer.WriteString(CLDSArcSlot::HELPLINK_URL);
	buffer.WriteInteger(CLDSArcSlot::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSArcSlot::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSArcSlot::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSArcSlot::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//���հ�
	buffer.WriteInteger(CLS_PARAMPLATE);
	buffer.WriteString("���ջ���");
	buffer.WriteString(CLDSParamPlate::HELPLINK_URL);
	buffer.WriteInteger(CLDSParamPlate::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSParamPlate::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSParamPlate::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSParamPlate::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//���
	buffer.WriteInteger(CLS_PLATE);
	buffer.WriteString("�ְ�");
	buffer.WriteString(CLDSPlate::HELPLINK_URL);
	buffer.WriteInteger(CLDSPlate::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSPlate::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSPlate::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSPlate::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//�ֹ�
	buffer.WriteInteger(CLS_LINETUBE);
	buffer.WriteString("�ֹ�");
	buffer.WriteString(CLDSLineTube::HELPLINK_URL);
	buffer.WriteInteger(CLDSLineTube::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSLineTube::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSLineTube::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSLineTube::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//�۸�
	buffer.WriteInteger(CLS_LINESLOT);
	buffer.WriteString("�۸�");
	buffer.WriteString(CLDSLineSlot::HELPLINK_URL);
	buffer.WriteInteger(CLDSLineSlot::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSLineSlot::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSLineSlot::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSLineSlot::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//���
	buffer.WriteInteger(CLS_LINEFLAT);
	buffer.WriteString("���");
	buffer.WriteString(CLDSLineFlat::HELPLINK_URL);
	buffer.WriteInteger(CLDSLineFlat::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSLineFlat::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSLineFlat::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSLineFlat::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//CLDSVector
	buffer.WriteInteger(0x010000);
	buffer.WriteString("����������");
	buffer.WriteString(CLDSVector::HELPLINK_URL);
	buffer.WriteInteger(CLDSVector::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSVector::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSVector::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSVector::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//CLDSPoint
	buffer.WriteInteger(CLS_GEPOINT);
	buffer.WriteString("��������λ");
	buffer.WriteString(CLDSPoint::HELPLINK_URL);
	buffer.WriteInteger(CLDSPoint::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSPoint::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSPoint::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSPoint::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//CElemInfo
	buffer.WriteInteger(CLS_FEMELEMENT);
	buffer.WriteString("���޵�Ԫ");
	buffer.WriteString(CElemInfo::HELPLINK_URL);
	buffer.WriteInteger(CElemInfo::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CElemInfo::propHashtable.GetFirst();pPropItem;
		pPropItem=CElemInfo::propHashtable.GetNext())
	{
		buffer.WriteString(CElemInfo::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//CLDSApp
	buffer.WriteInteger(0x010010);
	buffer.WriteString("");
	buffer.WriteString(CLDSApp::HELPLINK_URL);
	buffer.WriteInteger(CLDSApp::propHashtable.GetNodeNum());
	for (PROPLIST_ITEM *pPropItem=CLDSApp::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSApp::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSApp::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//CLDSView
	buffer.WriteInteger(0x010011);
	buffer.WriteString("");
	buffer.WriteString(CLDSView::HELPLINK_URL);
	buffer.WriteInteger(CLDSView::propHashtable.GetNodeNum());
	for (PROPLIST_ITEM *pPropItem=CLDSView::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSView::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSView::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//CBlockModel
	buffer.WriteInteger(0x010100);
	buffer.WriteString("");
	buffer.WriteString(CBlockModel::HELPLINK_URL);
	buffer.WriteInteger(CBlockModel::propHashtable.GetNodeNum());
	for (PROPLIST_ITEM *pPropItem =CBlockModel::propHashtable.GetFirst();pPropItem;
		pPropItem=CBlockModel::propHashtable.GetNext())
	{
		buffer.WriteString(CBlockModel::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//CBlockReference
	buffer.WriteInteger(0x010001);
	buffer.WriteString("�ְ����ζ���");
	buffer.WriteString(CBlockReference::HELPLINK_URL);
	buffer.WriteInteger(CBlockReference::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CBlockReference::propHashtable.GetFirst();pPropItem;
		pPropItem=CBlockReference::propHashtable.GetNext())
	{
		buffer.WriteString(CBlockReference::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//PROFILE_VERTEX
	buffer.WriteInteger(CLS_BLOCKREF);
	buffer.WriteString("װ�����");
	buffer.WriteString(PROFILE_VERTEX::HELPLINK_URL);
	buffer.WriteInteger(PROFILE_VERTEX::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=PROFILE_VERTEX::propHashtable.GetFirst();pPropItem;
		pPropItem=PROFILE_VERTEX::propHashtable.GetNext())
	{
		buffer.WriteString(PROFILE_VERTEX::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
}
void ImportPropListHelpInfo(CBuffer& buffer)
{
	//�����ļ���Ϣ
	//����PROPLIST_ITEM::url
	long cls_id;
	CXhChar50 cls_name ,propKeyStr,propName;
	CXhChar100 propUrl,defaultUrl,*pDefaultUrl=NULL;
	int nItem;
	char url[51]={0};
	do{
		buffer.ReadInteger(&cls_id);
		CHashStrList<PROPLIST_ITEM>* pPropHashList=NULL;
		CHashStrList<PROPLIST_ITEM> defaultHashList;
		if(cls_id==CLS_LINEANGLE)
		{
			pDefaultUrl=&CLDSLineAngle::HELPLINK_URL;
			pPropHashList=&CLDSLineAngle::propHashtable;
		}
		else if(cls_id==CLS_DBOBJECT)
		{
			pDefaultUrl=&CLDSDbObject::HELPLINK_URL;
			pPropHashList=&CLDSDbObject::propHashtable;
		}
		// �±���һ���д�
		else if(cls_id==CLS_PART)
		{
			pDefaultUrl=&CLDSPart::HELPLINK_URL;
			pPropHashList=&CLDSPart::propHashtable;
		}
		else if(cls_id==CLS_SPHERE)
		{
			pDefaultUrl =&CLDSSphere::HELPLINK_URL;
			pPropHashList=&CLDSSphere::propHashtable;
		}
		else if(cls_id==CLS_BOLT)
		{
			pDefaultUrl=&CLDSBolt::HELPLINK_URL;
			pPropHashList=&CLDSBolt::propHashtable;
		}
		else if(cls_id==CLS_NODE)
		{
			pDefaultUrl=&CLDSNode::HELPLINK_URL;
			pPropHashList=&CLDSNode::propHashtable;
		}
		else if(cls_id==CLS_GEPOINT)
		{
			pDefaultUrl=&CLDSPoint::HELPLINK_URL;
			pPropHashList=&CLDSPoint::propHashtable;
		}
		else if(cls_id==0x010000)
		{
			pPropHashList=&CLDSVector::propHashtable;
			pDefaultUrl =&CLDSVector::HELPLINK_URL;
		}
		else if(cls_id==CLS_GROUPLINEANGLE)
		{
			pPropHashList=&CLDSGroupLineAngle::propHashtable;
			pDefaultUrl =&CLDSGroupLineAngle::HELPLINK_URL;
		}
		else if(cls_id==CLS_FEMELEMENT)
		{
			pPropHashList=&CElemInfo::propHashtable;
			pDefaultUrl =&CElemInfo::HELPLINK_URL;
		}
		else if(cls_id==CLS_LINEPART)
		{
			pPropHashList=&CLDSLinePart::propHashtable;
			pDefaultUrl=&CLDSLinePart::HELPLINK_URL;
		}
		else if(cls_id==CLS_ARCPART)
		{
			pPropHashList=&CLDSArcPart::propHashtable;
			pDefaultUrl=&CLDSArcPart::HELPLINK_URL;
		}
		else if(cls_id==CLS_ARCANGLE)
		{
			pPropHashList=&CLDSArcAngle::propHashtable;
			pDefaultUrl =&CLDSArcAngle::HELPLINK_URL;
		}
		else if(cls_id==CLS_ARCFLAT)
		{
			pPropHashList=&CLDSArcFlat::propHashtable;
			pDefaultUrl=&CLDSArcFlat::HELPLINK_URL;
		}
		else if(cls_id==CLS_ARCSLOT)
		{
			pPropHashList=&CLDSArcSlot::propHashtable;
			pDefaultUrl = &CLDSArcSlot::HELPLINK_URL;
		}
		else if(cls_id==CLS_PARAMPLATE)
		{
			pPropHashList=&CLDSParamPlate::propHashtable;
			pDefaultUrl =&CLDSParamPlate::HELPLINK_URL;
		}
		else if(cls_id==CLS_PLATE)
		{
			pDefaultUrl=&CLDSPlate::HELPLINK_URL;
			pPropHashList=&CLDSPlate::propHashtable;
		}
		else if(cls_id==CLS_LINETUBE)
		{
			pPropHashList=&CLDSLineTube::propHashtable;
			pDefaultUrl=&CLDSLineTube::HELPLINK_URL;
		}
		else if(cls_id==CLS_LINESLOT)
		{
			pPropHashList=&CLDSLineSlot::propHashtable;
			pDefaultUrl=&CLDSLineSlot::HELPLINK_URL;
		}
		else if(cls_id==CLS_LINEFLAT)
		{
			pPropHashList=&CLDSLineFlat::propHashtable;
			pDefaultUrl=&CLDSLineFlat::HELPLINK_URL;
		}
		else if(cls_id==CLS_BLOCKREF)
		{
			pPropHashList=&CBlockReference::propHashtable;
			pDefaultUrl =&CBlockReference::HELPLINK_URL;
		}
		else if(cls_id==0x010001)
		{
			pPropHashList=&CLDSApp::propHashtable;
			pDefaultUrl =&CLDSApp::HELPLINK_URL;
		}
		else if(cls_id==0x010002)
		{
			pPropHashList=&CLDSView::propHashtable;
			pDefaultUrl =&CLDSView::HELPLINK_URL;
		}
		else if(cls_id==CLS_BLOCK)
		{
			pPropHashList=&CBlockModel::propHashtable;
			pDefaultUrl =&CBlockModel::HELPLINK_URL;
		}
		else if(cls_id==0x010003)
		{
			pPropHashList=&PROFILE_VERTEX::propHashtable;
			pDefaultUrl =&PROFILE_VERTEX::HELPLINK_URL;
		}
		//TODO:����else if
		else
		{
			pPropHashList=&defaultHashList;
			pDefaultUrl=NULL;
		}
		buffer.ReadString(cls_name);
		if(pDefaultUrl)
			buffer.ReadString(*pDefaultUrl,101);
		else
			buffer.ReadString(defaultUrl,101);
		buffer.ReadInteger(&nItem);
		for (int i =0;i<nItem;i++)
		{
			buffer.ReadString(propKeyStr);
			buffer.ReadString(propName,51);
			buffer.ReadString(url,101);
			PROPLIST_ITEM *pItem=pPropHashList->GetValue(propKeyStr);
			if(pItem)
				pItem->Url.Copy(url);
			//else
			//	logerr.Log("")
		}
	}while(buffer.GetRemnantSize()>0);
}
void ImportPropHelpListMapFile(char* bmp_file)
{
	FILE* fp=fopen(bmp_file,"rb");
	if(fp==NULL)
		return;
	long version=1;
	long buf_size=0;
	fread(&version,4,1,fp);
	fread(&buf_size,4,1,fp);	//�������泤��
	CBuffer buffer;
	buffer.Write(NULL,buf_size);
	fread(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
	fclose(fp);
	buffer.SeekToBegin();
	ImportPropListHelpInfo(buffer);
}*/

CLDSLine* IModel::MatchDatumLine(const double* pxLineOrg,const double* pvLineVec)
{
	for(CLDSLine* pLine=EnumLineFirst();pLine;pLine=EnumLineNext())
	{
		if(pLine->IsSameWith(pxLineOrg,pvLineVec))
			return pLine;
	}
	return NULL;
}

CLDSPlane* IModel::MatchDatumPlane(const double* pxPlaneOrg,const double* pvPlaneStdNormal)
{
	for(CLDSPlane* pPlane=EnumPlaneFirst();pPlane;pPlane=EnumPlaneNext())
	{
		if(pPlane->IsSameWith(pxPlaneOrg,pvPlaneStdNormal))
			return pPlane;
	}
	return NULL;
}
BOOL CTower::InitDisplaySetByPolyRgn(POLYGON &selRgn,CLDSDbObject* pSameLifeRefObj/*=NULL*/,
						NODESET* pRsltNodeSet/*=NULL*/,PARTSET* pRsltPartSet/*=NULL*/,bool rapidmode/*=false*/)
{
	IModel* pModel=CLDSObject::_console->GetActiveModel();
	if(pModel==NULL)
		return FALSE;
	CLDSModule *pActiveModule=NULL;
	if(pSameLifeRefObj!=NULL)
	{
		CTower* pTaModel=(CTower*)pSameLifeRefObj->BelongModel();
		for(pActiveModule=pTaModel->Module.GetFirst();pActiveModule;pActiveModule=pTaModel->Module.GetNext())
		{
			if(pActiveModule->IsSonPart(pSameLifeRefObj,0,true))
				break;
		}
	}
	if(pActiveModule==NULL)
		pActiveModule=CLDSObject::_console->GetActiveModule();
	if(pRsltNodeSet==NULL)
		pRsltNodeSet=&CLDSObject::_console->GetDispNodeSet();
	if(pRsltPartSet==NULL)
		pRsltPartSet=&CLDSObject::_console->GetDispPartSet();
	pRsltNodeSet->Empty();
	CHashList<bool>hashLiveNodes;
	for(CLDSNode* pNode=pModel->EnumNodeFirst();pNode;pNode=pModel->EnumNodeNext())
	{
		if(pSameLifeRefObj)
		{
			if(!pSameLifeRefObj->ModuleTogetherWith(pNode))
				continue;
		}
		else if(pActiveModule&&!pActiveModule->IsSonPart(pNode))
			continue;	//�ǵ�ǰģ���еĹ��� wht 11-07-22
		if(selRgn.PtInRgn(pNode->xOriginalPos)==1)//���ڶ���������ڣ������߽����ڣ�
		{
			pRsltNodeSet->append(pNode);
			hashLiveNodes.SetValue(pNode->handle,true);
		}
	}
	pRsltPartSet->Empty();
	for(CLDSPart *pPart=pModel->EnumPartFirst();pPart;pPart=pModel->EnumPartNext())
	{
		if(pSameLifeRefObj)
		{
			if(!pSameLifeRefObj->ModuleTogetherWith(pPart))
				continue;
		}
		else if(pActiveModule&&!pActiveModule->IsSonPart(pPart))
			continue;	//�ǵ�ǰģ���еĹ��� wht 11-07-22
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		if(!pPart->IsLinePart())
		{	//��
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{	//��������
				CLDSParamPlate *pParamPlate = (CLDSParamPlate*)pPart;
				if(pParamPlate->IsFL()||pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND)
					continue;	//���±ߴ���(���ݸֹ���Ӷ����ӹ���)	
				long h=0;
				if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
					pParamPlate->GetDesignItemValue('E',&h);
				else if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
					pParamPlate->GetDesignItemValue('P',&h);	//'A'��Ϊ'P' wht 11-05-20
				else if(pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
					pParamPlate->GetDesignItemValue('P',&h);
				CLDSNode *pBaseNode=pModel->FromNodeHandle(h);
				if(pBaseNode)
				{	//�������׼�ڵ㲻�ڶ����������(�����߽�����)�����
					if(selRgn.PtInRgn(pBaseNode->ActivePosition())!=1)	
						continue;
				}
				else	
				{	//������ԭ�㲻�ڶ����������(�����߽�����)
					if(selRgn.PtInRgn(pParamPlate->ucs.origin)!=1)
						continue;	
				}
				pRsltPartSet->append(pParamPlate);
			}
			else if(pPart->GetClassTypeId()==CLS_PLATE)
			{	//��ͨ�ְ�
				CLDSPlate *pPlate = (CLDSPlate*)pPart;
				CLDSNode *pBaseNode=pModel->FromNodeHandle(pPlate->designInfo.m_hBaseNode);
				if(pBaseNode)
				{	//�ְ��׼�ڵ㲻�ڶ����������(�����߽�����)�����
					if(selRgn.PtInRgn(pBaseNode->ActivePosition())!=1)	
						continue;
				}
				else	
				{	//�ְ�ԭ�㲻�ڶ����������(�����߽�����)�����
					if(selRgn.PtInRgn(pPlate->ucs.origin)!=1)
						continue;
				}
				pRsltPartSet->append(pPlate);
				for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
					pRsltPartSet->append(pLsRef->GetLsPtr());
			}
			else	
			{	//��������	���壬���ι����ȵ�
				if(selRgn.PtInRgn(pPart->ucs.origin)!=1)//����ԭ�㲻�ڶ����������(�����߽�����)�����
					continue;
				pRsltPartSet->append(pPart);
			}
		}
		else
#endif
		{	//�˼�
			CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
			CLDSNode* pStart = pLinePart->pStart;
			CLDSNode* pEnd   = pLinePart->pEnd;
			if(pStart==NULL||pEnd==NULL)
				continue;	//�������˼�
			f3dLine line;
			line.startPt = pStart->xOriginalPos;
			line.endPt   = pEnd->xOriginalPos;
			if(rapidmode)
			{	//����ģʽ
				if(hashLiveNodes.GetValue(pStart->handle)==NULL)
					continue;
				else if(hashLiveNodes.GetValue(pEnd->handle)==NULL)
					continue;
			}
			else if(selRgn.LineInRgn(line.startPt,line.endPt)!=1) //�߲��ڶ����������(�����߽�����)�����
				continue;
			pRsltPartSet->append(pLinePart);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
			if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
			{	//��Ӷ˰弰�丽����������ʾ�б�
				int nPush=pModel->PushPartStack();
				CLDSLineTube *pLineTube = (CLDSLineTube*)pLinePart;
				CLDSParamPlate *pStartPlate=(CLDSParamPlate*)pModel->FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
				if(pStartPlate)
				{	
					pRsltPartSet->append(pStartPlate);
					for(CLsRef *pLsRef=pStartPlate->GetFirstLsRef();pLsRef;pLsRef=pStartPlate->GetNextLsRef())
					{
						pRsltPartSet->append(pLsRef->GetLsPtr());
					}
					for(ATTACHPART_ITEM *pItem=pStartPlate->GetFirstAttachPart();pItem;pItem=pStartPlate->GetNextAttachPart())
					{
						CLDSPart *pTempPart=pModel->FromPartHandle(pItem->h);
						if(pTempPart)
							pRsltPartSet->append(pTempPart);
					}
				}
				CLDSParamPlate *pEndPlate=(CLDSParamPlate*)pModel->FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
				if(pEndPlate)
				{
					pRsltPartSet->append(pEndPlate);
					for(CLsRef *pLsRef=pEndPlate->GetFirstLsRef();pLsRef;pLsRef=pEndPlate->GetNextLsRef())
						pRsltPartSet->append(pLsRef->GetLsPtr());
					for(ATTACHPART_ITEM *pItem=pEndPlate->GetFirstAttachPart();pItem;pItem=pEndPlate->GetNextAttachPart())
					{
						CLDSPart *pTempPart=pModel->FromPartHandle(pItem->h);
						if(pTempPart)
							pRsltPartSet->append(pTempPart);
					}
				}
				pModel->PopPartStack(nPush);
			}
			else if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{	//��ӽǸ��ϵ���˨����ʾ�б�
				CLDSLineAngle *pLineAngle = (CLDSLineAngle*)pLinePart;
				for(CLsRef *pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
					pRsltPartSet->append(pLsRef->GetLsPtr());
			}
#endif
		}
	}
	if(pRsltPartSet->GetNodeNum()<=0 || pRsltNodeSet->GetNodeNum()<=0)
		return FALSE;
	return TRUE;
}
//���ݸ˼����Գƽڵ��ʼ��չ����ͼ�Ĺ�������
BOOL CTower::InitDisplaySetByMirPoint(CLDSLinePart* pLinePart,CLDSNode* pMirStart,CLDSNode* pMirEnd)
{
	if(pLinePart==NULL || pMirStart==NULL )
		return FALSE;
	IModel* pModel=pLinePart->BelongModel();
	f3dPoint pPoints[4];
	pPoints[0] = pLinePart->pStart->ActivePosition();
	pPoints[1] = pLinePart->pEnd->ActivePosition();
	int nCounts=4;
	if(pMirEnd!=NULL)
	{
		pPoints[2] = pMirEnd->ActivePosition();
		if(pMirStart!=NULL) //��㲻�غ�
			pPoints[3] = pMirStart->ActivePosition();
		else				//����غ�
			nCounts = 3;
	}
	else
	{
		pPoints[2] = pMirStart->ActivePosition();
		nCounts = 3;
	}
	fPolygonRgn selRgn;
	if(!selRgn.CreatePolygonRgn(pPoints, nCounts))
		return FALSE;
	CLDSModule *pActiveModule=CLDSObject::_console->GetActiveModule();
	if(pActiveModule==NULL)
		return FALSE;
	CLDSObject::_console->EmptyDispNode();
	for(CLDSNode* pNode=pModel->EnumNodeFirst();pNode;pNode=pModel->EnumNodeNext())
	{
		if(pActiveModule&&!pActiveModule->IsSonPart(pNode))
			continue;	//�ǵ�ǰģ���еĹ��� wht 11-07-22
		if(selRgn.PtInRgn(pNode->ActivePosition())==1)//���ڶ����������(�����߽�����)
			CLDSObject::_console->AddToDispSet(pNode);
	}
	f3dLine line;
	CLDSObject::_console->EmptyDispPart();
	for(CLDSPart *pPart=pModel->EnumPartFirst();pPart;pPart=pModel->EnumPartNext())
	{
		if(pActiveModule&&!pActiveModule->IsSonPart(pPart))
			continue;	//�ǵ�ǰģ���еĹ��� wht 11-07-22
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		if(!pPart->IsLinePart())
		{	//��
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{	//��������
				CLDSParamPlate *pParamPlate = (CLDSParamPlate*)pPart;
				if(pParamPlate->IsFL()||pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND)
					continue;	//���±ߴ���(���ݸֹ���Ӷ����ӹ���)
				CLDSNode *pBaseNode=pParamPlate->GetBaseNode();
				if(pBaseNode)
				{	//�������׼�ڵ㲻�ڶ����������(�����߽�����)�����
					if(selRgn.PtInRgn(pBaseNode->ActivePosition())!=1)	
						continue;
				}
				else	
				{	//������ԭ�㲻�ڶ����������(�����߽�����)
					if(selRgn.PtInRgn(pParamPlate->ucs.origin)!=1)
						continue;	
				}
				CLDSObject::_console->AddToDispSet(pParamPlate);
			}
			else if(pPart->GetClassTypeId()==CLS_PLATE)
			{	//��ͨ�ְ�
				CLDSPlate *pPlate = (CLDSPlate*)pPart;
				if(pPlate->jdb_style==4)	//��˨����Ի�׼�˼�Ϊ�жϻ�׼
				{
					CLDSLinePart* pLinePart=(CLDSLinePart*)pModel->FromPartHandle(pPlate->designInfo.m_hBasePart,CLS_LINEPART);
					if(pLinePart==NULL)
						continue;
					if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
						continue;	//�������˼�
					line.startPt = pLinePart->pStart->ActivePosition();
					line.endPt   = pLinePart->pEnd->ActivePosition();
					if(selRgn.LineInRgn(line)!=1) //�߲��ڶ����������(�����߽�����)�����
						continue;
				}
				else
				{
					CLDSNode *pBaseNode=pModel->FromNodeHandle(pPlate->designInfo.m_hBaseNode);
					if(pBaseNode)
					{	//�ְ��׼�ڵ㲻�ڶ����������(�����߽�����)�����
						if(selRgn.PtInRgn(pBaseNode->ActivePosition())!=1)	
							continue;
					}
					else	
					{	//�ְ�ԭ�㲻�ڶ����������(�����߽�����)�����
						if(selRgn.PtInRgn(pPlate->ucs.origin)!=1)
							continue;
					}
				}
				CLDSObject::_console->AddToDispSet(pPlate);
				for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
					CLDSObject::_console->AddToDispSet(pLsRef->GetLsPtr());
			}
			else	
			{	//��������	���壬���ι����ȵ�
				if(selRgn.PtInRgn(pPart->ucs.origin)!=1)//����ԭ�㲻�ڶ����������(�����߽�����)�����
					continue;
				CLDSObject::_console->AddToDispSet(pPart);
			}
		}
		else
#endif
		{	//�˼�
			pLinePart=(CLDSLinePart*)pPart;
			CLDSNode* pStart = pLinePart->pStart;
			CLDSNode* pEnd   = pLinePart->pEnd;
			if(pStart==NULL||pEnd==NULL)
				continue;	//�������˼�
			line.startPt = pStart->ActivePosition();
			line.endPt   = pEnd->ActivePosition();
			if(selRgn.LineInRgn(line)!=1) //�߲��ڶ����������(�����߽�����)�����
				continue;
			CLDSObject::_console->AddToDispSet(pLinePart);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
			if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
			{	//��Ӷ˰弰�丽����������ʾ�б�
				CLDSLineTube *pLineTube = (CLDSLineTube*)pLinePart;
				int nPush=pModel->PushPartStack();
				CSuperSmartPtr<CLDSPart> pStartPlate=pModel->FromPartHandle(pLineTube->m_tJointStart.hLinkObj);
				if(pStartPlate.IsHasPtr()&&pStartPlate->IsPlate())
				{	
					CLDSObject::_console->AddToDispSet(pStartPlate);
					if(pStartPlate->GetClassTypeId()==CLS_PARAMPLATE)
					{
						for(CLsRef *pLsRef=pStartPlate.ParamPlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pStartPlate.ParamPlatePointer()->GetNextLsRef())
							CLDSObject::_console->AddToDispSet(pLsRef->GetLsPtr());
						for(ATTACHPART_ITEM *pItem=pStartPlate.ParamPlatePointer()->GetFirstAttachPart();pItem;pItem=pStartPlate.ParamPlatePointer()->GetNextAttachPart())
						{
							CLDSPart *pTempPart=pModel->FromPartHandle(pItem->h);
							CLDSObject::_console->AddToDispSet(pTempPart);
						}
					}
					else if(pStartPlate->GetClassTypeId()==CLS_PLATE)
					{
						for(CLsRef *pLsRef=pStartPlate.PlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pStartPlate.ParamPlatePointer()->GetNextLsRef())
							CLDSObject::_console->AddToDispSet(pLsRef->GetLsPtr());
					}
				}
				CSuperSmartPtr<CLDSPart> pEndPlate=pModel->FromPartHandle(pLineTube->m_tJointEnd.hLinkObj);
				if(pEndPlate.IsHasPtr()&&pEndPlate->IsPlate())
				{
					CLDSObject::_console->AddToDispSet(pEndPlate);
					if(pEndPlate->GetClassTypeId()==CLS_PARAMPLATE)
					{
						for(CLsRef *pLsRef=pEndPlate.ParamPlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pEndPlate.ParamPlatePointer()->GetNextLsRef())
							CLDSObject::_console->AddToDispSet(pLsRef->GetLsPtr());
						for(ATTACHPART_ITEM *pItem=pEndPlate.ParamPlatePointer()->GetFirstAttachPart();pItem;pItem=pEndPlate.ParamPlatePointer()->GetNextAttachPart())
						{
							CLDSPart *pTempPart=pModel->FromPartHandle(pItem->h);
							CLDSObject::_console->AddToDispSet(pTempPart);
						}
					}
					else if(pEndPlate->GetClassTypeId()==CLS_PLATE)
					{
						for(CLsRef *pLsRef=pEndPlate.PlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pEndPlate.PlatePointer()->GetNextLsRef())
							CLDSObject::_console->AddToDispSet(pLsRef->GetLsPtr());
					}
				}
				pModel->PopPartStack(nPush);
			}
			else if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{	//��ӽǸ��ϵ���˨����ʾ�б�
				CLDSLineAngle *pLineAngle = (CLDSLineAngle*)pLinePart;
				for(CLsRef *pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
					CLDSObject::_console->AddToDispSet(pLsRef->GetLsPtr());
			}
#endif
		}
	}
	return TRUE;
}
