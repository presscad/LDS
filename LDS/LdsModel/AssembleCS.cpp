//<LOCALE_TRANSLATE BY hxr /> 2014-12-9
#include "stdafx.h"
#include "Tower.h"
#include "MemberProperty.h"


CParaCS::CParaCS()
{
	id=0;
	m_bNeedUpdateCS=true;
	cs.InitStdCS();
	//LoadDefaultUCS(&acs);
	param.m_cDefStyle=CS_PARAM;
	param.m_eDatumPlane=CParaCS::DP_XY;
}

bool CParaCS::SetACS(const GECS& ucs,bool bModifyParam/*=true*/)
{
	if(param.m_cDefStyle==CS_PARAM)
	{
		if(bModifyParam)
		{
			param.desOrg.datum_pos_style=0;
			param.desAxisX.norm_style=param.desAxisY.norm_style=param.desAxisZ.norm_style=0;
		}
		param.desOrg.SetPosition(ucs.origin.x,ucs.origin.y,ucs.origin.z);
		param.desAxisX.vector=ucs.axis_x;
		param.desAxisY.vector=ucs.axis_y;
		param.desAxisZ.vector=ucs.axis_z;
		cs.origin=ucs.origin;
		cs.axis_x=ucs.axis_x;
		cs.axis_y=ucs.axis_y;
		cs.axis_z=ucs.axis_z;
		m_bNeedUpdateCS=false;
		return RightHandRuled();
	}
	else
		return false;
}
bool CParaCS::SetACS(const UCS_STRU& ucs,bool bModifyParam/*=true*/)
{
	GECS cs;
	cs.origin.Set(ucs.origin.x,ucs.origin.y,ucs.origin.z);
	cs.axis_x.Set(ucs.axis_x.x,ucs.axis_x.y,ucs.axis_x.z);
	cs.axis_y.Set(ucs.axis_y.x,ucs.axis_y.y,ucs.axis_y.z);
	cs.axis_z.Set(ucs.axis_z.x,ucs.axis_z.y,ucs.axis_z.z);
	return SetACS(cs,bModifyParam);
}
bool CParaCS::RightHandRuled()
{
	if(param.m_eDatumPlane==CParaCS::DP_XY||param.m_eDatumPlane==CParaCS::DP_YX)
	{
		cs.axis_z=cs.axis_x^cs.axis_y;
		if(param.m_eDatumPlane==CParaCS::DP_XY)	//X-Y
			cs.axis_y=cs.axis_z^cs.axis_x;
		else					//Y-X
			cs.axis_x=cs.axis_y^cs.axis_z;
		normalize(cs.axis_x);
		normalize(cs.axis_z);
		normalize(cs.axis_y);
	}
	else if(param.m_eDatumPlane==CParaCS::DP_YZ||param.m_eDatumPlane==CParaCS::DP_ZY)//2.Y-Z 3.Z-Y
	{
		cs.axis_x=cs.axis_y^cs.axis_z;
		if(param.m_eDatumPlane==CParaCS::DP_YZ)
			cs.axis_z=cs.axis_x^cs.axis_y;
		else 
			cs.axis_y=cs.axis_z^cs.axis_x;
		normalize(cs.axis_x);
		normalize(cs.axis_z);
		normalize(cs.axis_y);
	}
	else if(param.m_eDatumPlane==CParaCS::DP_ZX||param.m_eDatumPlane==CParaCS::DP_XZ)//4.Z-X 5.X-Z
	{
		cs.axis_y=cs.axis_z^cs.axis_x;
		if(param.m_eDatumPlane==CParaCS::DP_ZX)
			cs.axis_x=cs.axis_y^cs.axis_z;
		else 
			cs.axis_z=cs.axis_x^cs.axis_y;
		normalize(cs.axis_x);
		normalize(cs.axis_z);
		normalize(cs.axis_y);
	}
	if(cs.axis_x.IsZero()||cs.axis_y.IsZero()||cs.axis_z.IsZero())
		return false;
	else
		return true;
}

BOOL CParaCS::CalMirAcs(GECS& cs,CLDSPart *pMotherPart,const MIRMSG& mirInfo)
{
#ifdef __ANGLE_PART_INC_
	if(pMotherPart==NULL)
		return FALSE;
	if(mirInfo.axis_flag&0x13&&pMotherPart->IsAngle())
		return FALSE;//������ԽǸֽ���ƽ��Գ�Ӱ��
	if(pMotherPart->IsAngle())
		((CLDSLineAngle*)pMotherPart)->getUCS(pMotherPart->ucs);
	else if(pMotherPart->GetClassTypeId()==CLS_LINETUBE)
		((CLDSLineTube*)pMotherPart)->BuildUCS();
	else if(pMotherPart->GetClassTypeId()==CLS_LINESLOT)
		((CLDSLineSlot*)pMotherPart)->BuildUCS();
	else if(pMotherPart->GetClassTypeId()==CLS_LINEFLAT)
		((CLDSLineFlat*)pMotherPart)->BuildUCS();
	else if(pMotherPart->GetClassTypeId()==CLS_BOLT)
		((CLDSBolt*)pMotherPart)->GetUCS();
	if(pMotherPart->GetClassTypeId()==CLS_BOLT)
	{	//��˨Ӱ��Գ�ʱӦ����ȥ����˨�Ի���ķ���ƫ����
		CLDSBolt *pMotherBolt=(CLDSBolt*)pMotherPart;
		cs.origin=pMotherBolt->ucs.origin-
			pMotherBolt->ucs.axis_z*pMotherBolt->des_base_pos.norm_offset.Thick(pMotherBolt->BelongModel());
		cs.origin=GetMirPos(cs.origin,mirInfo);
	}
	else
		cs.origin=GetMirPos(pMotherPart->ucs.origin,mirInfo);
	if((mirInfo.axis_flag&0x13)==0)	//��(��ת)�Գ�
	{
		cs.axis_x=GetMirVector(pMotherPart->ucs.axis_x,mirInfo);
		cs.axis_y=GetMirVector(pMotherPart->ucs.axis_y,mirInfo);
		cs.axis_z=GetMirVector(pMotherPart->ucs.axis_z,mirInfo);
	}
	else if(pMotherPart->GetClassTypeId()==CLS_BOLT||pMotherPart->GetClassTypeId()==CLS_LINETUBE)
	{	//������˨���ֹ���X�ἰZ��Ϊ�ؼ�����
		cs.axis_x=GetMirVector(pMotherPart->ucs.axis_x,mirInfo);
		cs.axis_z=GetMirVector(pMotherPart->ucs.axis_z,mirInfo);
		cs.axis_y=cs.axis_z^cs.axis_x;
	}
	else if(pMotherPart->GetClassTypeId()==CLS_LINESLOT||pMotherPart->GetClassTypeId()==CLS_LINEFLAT)
	{	//���ڲ۸ּ������˼���Y�ἰZ��Ϊ�ؼ�����
		cs.axis_y=GetMirVector(pMotherPart->ucs.axis_y,mirInfo);
		cs.axis_z=GetMirVector(pMotherPart->ucs.axis_z,mirInfo);
		cs.axis_x=GetMirVector(pMotherPart->ucs.axis_x,mirInfo);
		cs.axis_x=cs.axis_y^cs.axis_z;
	}
	else if(pMotherPart->GetClassTypeId()==CLS_PLATE)
	{	//���ڸְ���X�ἰY��Ϊ�ؼ����򣬵�Ӧ����Z�ᷴ��ʱ�������Ļ��㷨��ƫ����
		cs.axis_x=GetMirVector(pMotherPart->ucs.axis_x,mirInfo);
		cs.axis_y=GetMirVector(pMotherPart->ucs.axis_y,mirInfo);
		cs.axis_z=cs.axis_x^cs.axis_y;
	}
	else if(pMotherPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate* pParamPlate=(CLDSParamPlate*)pMotherPart;
		if( pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND||
			pParamPlate->m_iParamType==TYPE_XEND)	//�����X�ἰZ��Ϊ�ؼ�����
		{
			cs.axis_x=GetMirVector(pMotherPart->ucs.axis_x,mirInfo);
			cs.axis_z=GetMirVector(pMotherPart->ucs.axis_z,mirInfo);
			cs.axis_y=cs.axis_z^cs.axis_x;
		}
		else if(pParamPlate->m_iParamType==TYPE_ELBOWPLATE||pParamPlate->m_iParamType==TYPE_CIRRIBPLATE||
			pParamPlate->m_iParamType==TYPE_LINEANGLERIBPLATE||pParamPlate->m_iParamType==TYPE_RIBPLATE||
			pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)	//�߰���X�ἰY��Ϊ�ؼ�����
		{
			cs.axis_x=GetMirVector(pMotherPart->ucs.axis_x,mirInfo);
			cs.axis_y=GetMirVector(pMotherPart->ucs.axis_y,mirInfo);
			cs.axis_z=cs.axis_x^cs.axis_y;
		}
		else if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{	//TODO:��������Ҫ��һ��ȷ����֮ǰ����X�ἰY��Ϊ�ؼ����򣬵��п�����Ҫ����������Ļ��㷨��ƫ����
			cs.axis_x=GetMirVector(pMotherPart->ucs.axis_x,mirInfo);
			cs.axis_y=GetMirVector(pMotherPart->ucs.axis_y,mirInfo);
			cs.axis_z=cs.axis_x^cs.axis_y;
		}
		else if(pParamPlate->m_iParamType==TYPE_FL||
			pParamPlate->m_iParamType==TYPE_FLD||pParamPlate->m_iParamType==TYPE_FLP||
			pParamPlate->m_iParamType==TYPE_FLG||pParamPlate->m_iParamType==TYPE_FLR)
		{	//��������Y�ἰZ��Ϊ�ؼ�����
			cs.axis_x=GetMirVector(pMotherPart->ucs.axis_x,mirInfo);
			cs.axis_z=GetMirVector(pMotherPart->ucs.axis_z,mirInfo);
			cs.axis_y=cs.axis_z^cs.axis_x;
		}
		else	//���������ݲ�֧��
			return FALSE;	
	}
#endif
	if(cs.axis_x.IsZero()||cs.axis_y.IsZero()||cs.axis_z.IsZero())
		return FALSE;
	else
		return TRUE;
}
BOOL CParaCS::UpdateACS(CLDSPart *pMotherPart)
{
	if(!m_bNeedUpdateCS)
		return TRUE;
	m_bNeedUpdateCS=false;
	if(param.m_cDefStyle==CS_MIRTRANS)
	{
		if(CalMirAcs(cs,pMotherPart,param.m_xMirInfo))
			return RightHandRuled();
		else
			return FALSE;
	}
	else if(param.m_cDefStyle==CS_TRANSLATE)
	{
		cs.axis_x=pMotherPart->ucs.axis_x;
		cs.axis_y=pMotherPart->ucs.axis_y;
		cs.axis_z=pMotherPart->ucs.axis_z;
		if(param.desAxisX.UpdateVector(pMotherPart->BelongModel())&&RightHandRuled())
			cs.origin=pMotherPart->ucs.origin+param.desAxisX.vector*param.m_xMirInfo.origin.x;
		else
			return FALSE;
		return TRUE;
	}
	else
		return FALSE;
}
BOOL CParaCS::UpdateACS(IModel* pModel)
{
	if(!m_bNeedUpdateCS)
		return TRUE;
	m_bNeedUpdateCS=false;
	//UCS_STRU cs;
	if(param.m_cDefStyle==CS_PARAM)
	{	//����������ϵ
		param.desOrg.UpdatePos(pModel);
		cs.origin=param.desOrg.Position();
		if(param.m_eDatumPlane==CParaCS::DP_XY||param.m_eDatumPlane==CParaCS::DP_YX)
		{
			param.desAxisX.UpdateVector(pModel);
			param.desAxisY.UpdateVector(pModel);
			cs.axis_x=param.desAxisX.vector;
			cs.axis_y=param.desAxisY.vector;
			cs.axis_z=cs.axis_x^cs.axis_y;
			return RightHandRuled();
		}
		else if(param.m_eDatumPlane==CParaCS::DP_YZ||param.m_eDatumPlane==CParaCS::DP_ZY)//2.Y-Z 3.Z-Y
		{
			param.desAxisY.UpdateVector(pModel);
			param.desAxisZ.UpdateVector(pModel);
			cs.axis_y=param.desAxisY.vector;
			cs.axis_z=param.desAxisZ.vector;
			cs.axis_x=cs.axis_y^cs.axis_z;
			return RightHandRuled();
		}
		else if(param.m_eDatumPlane==CParaCS::DP_ZX||param.m_eDatumPlane==CParaCS::DP_XZ)//4.Z-X 5.X-Z
		{
			param.desAxisX.UpdateVector(pModel);
			param.desAxisZ.UpdateVector(pModel);
			cs.axis_x=param.desAxisX.vector;
			cs.axis_z=param.desAxisZ.vector;
			cs.axis_y=cs.axis_z^cs.axis_x;
			return RightHandRuled();
		}
		else
			return FALSE;
	}
	else
		return FALSE;
}
void CParaCS::ToBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	buffer.WriteInteger(id);
	buffer.WriteByte(param.m_cDefStyle);
	if(param.m_cDefStyle==CS_PARAM)
	{	//1.������������㼰װ��ƽ��
		param.desOrg.ToBuffer(buffer,version,doc_type);
		buffer.WriteInteger(param.m_eDatumPlane);
		if(param.m_eDatumPlane==CParaCS::DP_XY||param.m_eDatumPlane==CParaCS::DP_YX)
		{
			param.desAxisX.ToBuffer(buffer,version,doc_type);
			param.desAxisY.ToBuffer(buffer,version,doc_type);
		}
		else if(param.m_eDatumPlane==CParaCS::DP_YZ||param.m_eDatumPlane==CParaCS::DP_ZY)
		{
			param.desAxisY.ToBuffer(buffer,version,doc_type);
			param.desAxisZ.ToBuffer(buffer,version,doc_type);
		}
		else if(param.m_eDatumPlane==CParaCS::DP_ZX||param.m_eDatumPlane==CParaCS::DP_XZ)
		{
			param.desAxisZ.ToBuffer(buffer,version,doc_type);
			param.desAxisX.ToBuffer(buffer,version,doc_type);
		}
	}
	else if(param.m_cDefStyle==CS_MIRTRANS)
	{	//2.�Գ�---�����ĸ�����ĶԳ���Ϣ
		buffer.WritePoint(param.m_xMirInfo.origin);
		buffer.WritePoint(param.m_xMirInfo.mir_norm);
		buffer.WriteByte(param.m_xMirInfo.axis_flag);
		buffer.WriteByte(param.m_xMirInfo.array_num);
		buffer.WriteWord(param.m_xMirInfo.rotate_angle);
	}
	else if(param.m_cDefStyle==CS_TRANSLATE)
	{
		param.desAxisX.ToBuffer(buffer,version,doc_type);
		buffer.WritePoint(param.m_xMirInfo.origin);
	}
}
void CParaCS::ToXmlFile(FILE* fp,DWORD schema)
{
	fprintf(fp,"<������������Ϣ id=\"%d\">\n",id);
	fprintf(fp,"<����ϵ���巽ʽ DefStyle=\"%d\"/>\n",param.m_cDefStyle);
	if(param.m_cDefStyle==CS_PARAM)
	{	//1.������������㼰װ��ƽ��
		param.desOrg.ToXmlFile(fp,schema);
		fprintf(fp,"<��׼�淽ʽ DatumPlane=\"%d\"/>\n",param.m_eDatumPlane);
		if(param.m_eDatumPlane==CParaCS::DP_XY||param.m_eDatumPlane==CParaCS::DP_YX)
		{
			param.desAxisX.ToXmlFile(fp,schema);
			param.desAxisY.ToXmlFile(fp,schema);
		}
		else if(param.m_eDatumPlane==CParaCS::DP_YZ||param.m_eDatumPlane==CParaCS::DP_ZY)
		{
			param.desAxisY.ToXmlFile(fp,schema);
			param.desAxisZ.ToXmlFile(fp,schema);
		}
		else if(param.m_eDatumPlane==CParaCS::DP_ZX||param.m_eDatumPlane==CParaCS::DP_XZ)
		{
			param.desAxisZ.ToXmlFile(fp,schema);
			param.desAxisX.ToXmlFile(fp,schema);
		}
	}
	else if(param.m_cDefStyle==CS_MIRTRANS)
	{	//2.�Գ�---�����ĸ�����ĶԳ���Ϣ
		fprintf(fp,"<�����ĸ�����ĶԳ���Ϣ>\n");
		fprintf(fp,"<�Գƻ������� origin=\"(%.3f,%.3f,%.3f)\"/>\n",param.m_xMirInfo.origin.x,param.m_xMirInfo.origin.y,param.m_xMirInfo.origin.z);
		fprintf(fp,"<��ת�᷽�� mir_norm=\"%.3f,%.3f,%.3f)\"/>\n",param.m_xMirInfo.mir_norm.x,param.m_xMirInfo.mir_norm.y,param.m_xMirInfo.mir_norm.z);
		fprintf(fp,"<�ԳƷ�ʽ axis_flag=\"%c\"/>\n",param.m_xMirInfo.axis_flag);
		fprintf(fp,"<��ת�Գƴ��� arrar_num=\"%c\"/>\n",param.m_xMirInfo.array_num);
		fprintf(fp,"<��ת�Ƕ� rotate_angle=\"%d\"/>\n",param.m_xMirInfo.rotate_angle);
		fprintf(fp,"</�����ĸ�����ĶԳ���Ϣ>\n");
	}
	else if(param.m_cDefStyle==CS_TRANSLATE)
	{
		param.desAxisX.ToXmlFile(fp,schema);
		fprintf(fp,"<����ƽ�Ʒ��� origin=\"(%.3f,%.3f,%.3f)\"/>\n",param.m_xMirInfo.origin.x,param.m_xMirInfo.origin.y,param.m_xMirInfo.origin.z);
	}
	fprintf(fp,"</������������Ϣ>\n");
}
void CParaCS::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	buffer.ReadInteger(&id);
	buffer.ReadByte(&param.m_cDefStyle);
	if(param.m_cDefStyle==CS_PARAM)
	{	//0.������������㼰װ��ƽ��
		param.desOrg.FromBuffer(buffer,version,doc_type);
		int nDatumPlayType=0;
		buffer.ReadInteger(&nDatumPlayType);	
		param.m_eDatumPlane=(BYTE)nDatumPlayType;
		if(param.m_eDatumPlane==CParaCS::DP_XY||param.m_eDatumPlane==CParaCS::DP_YX)
		{
			param.desAxisX.FromBuffer(buffer,version,doc_type);
			param.desAxisY.FromBuffer(buffer,version,doc_type);
		}
		else if(param.m_eDatumPlane==CParaCS::DP_YZ||param.m_eDatumPlane==CParaCS::DP_ZY)
		{
			param.desAxisY.FromBuffer(buffer,version,doc_type);
			param.desAxisZ.FromBuffer(buffer,version,doc_type);
		}
		else if(param.m_eDatumPlane==CParaCS::DP_ZX||param.m_eDatumPlane==CParaCS::DP_XZ)
		{
			param.desAxisZ.FromBuffer(buffer,version,doc_type);
			param.desAxisX.FromBuffer(buffer,version,doc_type);
		}
	}
	else if(param.m_cDefStyle==CS_MIRTRANS)
	{	//1.�Գ�---�����ĸ�����ĶԳ���Ϣ
		if( version!=0&&
		   ((doc_type==1&&version<=4020003)||	//TMA(V4.2.0.3)
			(doc_type==2&&version<=1020003)||	//LMA(V1.2.0.3)
			(doc_type==3&&version<=2000003)||	//TSA(V2.0.0.3)
			(doc_type==4&&version<=1020003)||	//LDS(V1.2.0.3)
			(doc_type==5&&version<=1020003)))	//TDA(V1.2.0.3)
		{
			//buffer.ReadInteger(&param.m_hMotherPart);
			buffer.Offset(4);
		}
		buffer.ReadPoint(param.m_xMirInfo.origin);
		buffer.ReadPoint(param.m_xMirInfo.mir_norm);
		buffer.ReadByte(&param.m_xMirInfo.axis_flag);
		buffer.ReadByte(&param.m_xMirInfo.array_num);
		buffer.ReadWord(&param.m_xMirInfo.rotate_angle);
	}
	else if(param.m_cDefStyle==CS_TRANSLATE)
	{
		param.desAxisX.FromBuffer(buffer,version,doc_type);
		buffer.ReadPoint(param.m_xMirInfo.origin);
	}
}
f3dPoint CParaCS::CoordTrans(f3dPoint pos)
{
	if(param.m_cDefStyle==CS_PARAM)
		coord_trans(pos,cs,TRUE,TRUE);
	else if(param.m_cDefStyle==CS_MIRTRANS)
		return param.m_xMirInfo.MirPoint(pos);
	else if(param.m_cDefStyle==CS_TRANSLATE)
		pos+=param.desAxisX.vector*param.m_xMirInfo.origin.x;
	return pos;
}
f3dPoint CParaCS::VectorTrans(f3dPoint vec)
{
	if(param.m_cDefStyle==CS_PARAM)
		vector_trans(vec,cs,TRUE,TRUE);
	else if(param.m_cDefStyle==CS_MIRTRANS)
		return param.m_xMirInfo.MirVector(vec);
	return vec;
}

BOOL CParaCS::IsDesAxisX()
{
	if(param.m_cDefStyle==0&&(param.m_eDatumPlane==CParaCS::DP_XY||param.m_eDatumPlane==CParaCS::DP_YX||
		param.m_eDatumPlane==CParaCS::DP_ZX||param.m_eDatumPlane==CParaCS::DP_XZ))
		return TRUE;
	return FALSE;
}

BOOL CParaCS::IsDesAxisY()
{
	if(param.m_cDefStyle==0&&(param.m_eDatumPlane==CParaCS::DP_XY||param.m_eDatumPlane==CParaCS::DP_YX||
		param.m_eDatumPlane==CParaCS::DP_YZ||param.m_eDatumPlane==CParaCS::DP_ZY))
		return TRUE;
	return FALSE;
}

BOOL CParaCS::IsDesAxisZ()
{
	if(param.m_cDefStyle==0&&(param.m_eDatumPlane==CParaCS::DP_ZX||param.m_eDatumPlane==CParaCS::DP_XZ||
		param.m_eDatumPlane==CParaCS::DP_YZ||param.m_eDatumPlane==CParaCS::DP_ZY))
		return TRUE;
	return FALSE;
}

char *CParaCS::GetAxisDescription(char cAxis)
{
	if(param.m_cDefStyle!=0)
		return " ";
	if(cAxis=='X'||cAxis=='x')
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(param.m_eDatumPlane==CParaCS::DP_XY||param.m_eDatumPlane==CParaCS::DP_XZ)
			return "Datum Axis";
		else if(param.m_eDatumPlane==CParaCS::DP_YX||param.m_eDatumPlane==CParaCS::DP_ZX)
			return "Auxiliary Axis";
#else
		if(param.m_eDatumPlane==CParaCS::DP_XY||param.m_eDatumPlane==CParaCS::DP_XZ)
			return "��׼";
		else if(param.m_eDatumPlane==CParaCS::DP_YX||param.m_eDatumPlane==CParaCS::DP_ZX)
			return "����";
#endif
	}
	else if(cAxis=='Y'||cAxis=='y')
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(param.m_eDatumPlane==CParaCS::DP_YX||param.m_eDatumPlane==CParaCS::DP_YZ)
			return "Datum Axis";
		else if(param.m_eDatumPlane==CParaCS::DP_XY||param.m_eDatumPlane==CParaCS::DP_ZY)
			return "Auxiliary Axis";
#else
		if(param.m_eDatumPlane==CParaCS::DP_YX||param.m_eDatumPlane==CParaCS::DP_YZ)
			return "��׼";
		else if(param.m_eDatumPlane==CParaCS::DP_XY||param.m_eDatumPlane==CParaCS::DP_ZY)
			return "����";
#endif
	}
	else if(cAxis=='Z'||cAxis=='z')
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(param.m_eDatumPlane==CParaCS::DP_ZX||param.m_eDatumPlane==CParaCS::DP_ZY)
			return "Datum Axis";
		else if(param.m_eDatumPlane==CParaCS::DP_XZ||param.m_eDatumPlane==CParaCS::DP_YZ)
			return "Auxiliary Axis";
#else
		if(param.m_eDatumPlane==CParaCS::DP_ZX||param.m_eDatumPlane==CParaCS::DP_ZY)
			return "��׼";
		else if(param.m_eDatumPlane==CParaCS::DP_XZ||param.m_eDatumPlane==CParaCS::DP_YZ)
			return "����";
#endif
	}
	return " ";
}
char *CParaCS::GetDatumPlaneCmbItems()
{
#ifdef AFX_TARG_ENU_ENGLISH
	return "0.X-Y Plane|1.Y-X Plane|2.Y-Z Plane|3.Z-Y Plane|4.Z-X Plane|5.X-Z Plane|";
#else
	return "0.X-Y��|1.Y-X��|2.Y-Z��|3.Z-Y��|4.Z-X��|5.X-Z��|";
#endif
}