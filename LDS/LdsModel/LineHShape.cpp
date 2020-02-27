#include "stdafx.h"
#include "Tower.h"
#include "CreateFace.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef __H_SHAPE_STEEL_
CLDSLineHShape::CLDSLineHShape()
{
	size_idClassType=m_nClassTypeId = CLS_LINEHSHAPE;
	strcpy(m_sClassName,"CLDSLineHShape");
	size_height=200;
	size_wide=200;
	size_thick=8;
	size_thick2=12;
	_material = CSteelMatLibrary::Q235BriefMark();//'S';
	strcpy(spec,"");
	pSolidBody = NULL;
	is_visible=TRUE;
	is_data_modified = TRUE;
	m_uStatMatCoef = 1;
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN��������Ա���;
	hWorkNormRefPart = 0;
	m_iNormCalStyle = 1;				//Ĭ��Ϊ�˼���λ
	memset(&desStartPos,0,sizeof(CTubeEndPosPara));
	memset(&desEndPos,0,sizeof(CTubeEndPosPara));
	m_ibHShapeType=0;
}
CLDSLineHShape::~CLDSLineHShape()
{
	if(pSolidBody)
	{
		delete pSolidBody;
		pSolidBody=NULL;
	}
}
void CLDSLineHShape::IndependentShadowPropFromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	long h=0;
	buffer.ReadInteger(&h);
	pStart=BelongModel()->FromNodeHandle(h);	
	buffer.ReadInteger(&h);
	pEnd  =BelongModel()->FromNodeHandle(h);	
	pLsRefList->FromBuffer(buffer,BelongModel(),version,doc_type);
	if( version==0||
		(doc_type==1&&version>=4020005)||	//TMA(V4.2.0.5)
		(doc_type==2&&version>=1020005)||	//LMA(V1.2.0.5)
		(doc_type==3&&version>=2000005)||	//TSA(V2.0.0.5)
		(doc_type==4&&version>=1020005)||	//LDS(V1.2.0.5)
		(doc_type==5&&version>=1020005))	//TDA(V1.2.0.5)
	{
		buffer.ReadDword(&dwPermission);
		buffer.ReadDword(&dwStartPermission);
		buffer.ReadDword(&dwEndPermission);
		//buffer.ReadString(spec);	//���
		CfgwordFromBuffer(cfgword,buffer,version,doc_type);
		BelongModel()->EvolveCfgword(cfgword,version,doc_type);
		//buffer.ReadString(sPartNo);	//�������
		buffer.ReadString(layer(),4);	//�������ͼ��.
		buffer.ReadInteger(&m_uStatMatCoef);
		buffer.ReadInteger(&m_hPartWeldParent);
		buffer.ReadString(sNotes,51);
		buffer.ReadWord(&m_iNormCalStyle);	
		buffer.ReadPoint(nearWorkPlaneNorm);
		buffer.ReadInteger(&hWorkNormRefPart);
		buffer.ReadPoint(workPlaneNorm);
		f3dPoint start,end;
		buffer.ReadPoint(start);
		buffer.ReadPoint(end);
		baseline.SetStart(start);
		baseline.SetEnd(end);
		//���������б�
		int n=0;
		buffer.ReadInteger(&n);
		for(int j=0;j<n;j++)
		{
			RELATIVE_OBJECT *pMirObj=relativeObjs.Add(buffer.ReadInteger());
			buffer.ReadByte(&pMirObj->mirInfo.axis_flag);
			buffer.ReadPoint(pMirObj->mirInfo.origin);
			if((pMirObj->mirInfo.axis_flag&0x08)>0)
			{
				buffer.ReadWord(&pMirObj->mirInfo.rotate_angle);
				buffer.ReadByte(&pMirObj->mirInfo.array_num);
			}
			if(pMirObj->mirInfo.axis_flag&0x10)	//����
				buffer.ReadPoint(pMirObj->mirInfo.mir_norm);
		}
	}
}
void CLDSLineHShape::IndependentShadowPropToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(pStart)
		buffer.WriteInteger(pStart->handle);	// �����
	else
		buffer.WriteInteger(0);
	if(pEnd)
		buffer.WriteInteger(pEnd->handle);
	else
		buffer.WriteInteger(0);
	pLsRefList->ToBuffer(buffer,version,doc_type);

	buffer.WriteDword(dwPermission);
	buffer.WriteDword(dwStartPermission);
	buffer.WriteDword(dwEndPermission);
	//buffer.WriteString(spec);	//���
	CfgwordToBuffer(cfgword,buffer,version,doc_type);
	//buffer.WriteString(sPartNo);	//�������
	buffer.WriteString(layer());	//�������ͼ��.
	buffer.WriteInteger(m_uStatMatCoef);
	buffer.WriteInteger(m_hPartWeldParent);
	buffer.WriteString(sNotes);

	buffer.WriteWord(m_iNormCalStyle);	
	buffer.WriteDouble(nearWorkPlaneNorm.x);
	buffer.WriteDouble(nearWorkPlaneNorm.y);
	buffer.WriteDouble(nearWorkPlaneNorm.z);
	buffer.WriteInteger(hWorkNormRefPart);
	buffer.WritePoint(workPlaneNorm);
	buffer.WritePoint(baseline.Start());
	buffer.WritePoint(baseline.End());
	//���������б�
	buffer.WriteInteger(relativeObjs.GetNodeNum());
	for(RELATIVE_OBJECT *pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		buffer.WriteInteger(pMirObj->hObj);
		buffer.WriteByte(pMirObj->mirInfo.axis_flag);
		buffer.WritePoint(pMirObj->mirInfo.origin);
		if((pMirObj->mirInfo.axis_flag&8)>0)
		{
			buffer.WriteWord(pMirObj->mirInfo.rotate_angle);
			buffer.WriteByte(pMirObj->mirInfo.array_num);
		}
		if(pMirObj->mirInfo.axis_flag&0x10)	//����
			buffer.WritePoint(pMirObj->mirInfo.mir_norm);
	}
}
void CLDSLineHShape::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropFromBuffer(buffer,version,doc_type);
		return;
	}
	DWORD dw;
	bool bPrevDeflateVersion=false;	//�ļ�����֮ǰ�İ汾
	if((doc_type==1&&version>0&&version<4000027)	//TMA
		||(doc_type==2&&version>0&&version<1000082)	//LMA
		||(doc_type==4&&version>0&&version<1000009))//LDS
		bPrevDeflateVersion=true;
	buffer.ReadDword(&dwPermission);
	buffer.ReadDword(&dwStartPermission);
	buffer.ReadDword(&dwEndPermission);
	if( (doc_type==1&&(version==0||version>=4000020))||	//TMA
		(doc_type==2&&(version==0||version>=1000075))||	//LMA
		doc_type==4||doc_type==5)	//LDS|TDA
		buffer.ReadString(spec);
	CfgwordFromBuffer(cfgword,buffer,version,doc_type);
	BelongModel()->EvolveCfgword(cfgword,version,doc_type);
	buffer.ReadByte(&_material);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.ReadByte(&_cQualityLevel);
	if(bPrevDeflateVersion)
	{
		buffer.ReadInteger(&dw);
		m_bVirtualPart=(dw!=0);//��������
	}
	else
		buffer.ReadBooleanThin(&_bVirtualPart);
	buffer.ReadInteger(&_iSeg.iSeg);
	buffer.ReadString(sPartNo);			//�������
	buffer.ReadString(layer(),4);			//�������ͼ��.
	buffer.ReadInteger(&m_uStatMatCoef);
	buffer.ReadInteger(&m_hPartWeldParent);
	if(	version==0||
		(doc_type==1&&version>=5000000)||	//TMA V5.0.0.0
		(doc_type==2&&version>=5000000)||	//LMA V2.0.0.0
		(doc_type==3&&version>=2010000)||	//TSA V2.1.0.0
		(doc_type==4&&version>=1030000)||	//LDS V1.3.0.0
		(doc_type==5&&version>=1030000))	//TDA V1.3.0.0
		buffer.ReadByte(&_cFuncType);
	if(bPrevDeflateVersion)
	{
		buffer.ReadInteger(&dw);//��ʾ��˨�׼��и���
		m_bDisplayHole=(dw!=0);
	}
	else
		buffer.ReadBooleanThin(&m_bDisplayHole);
	buffer.ReadInteger(&m_bLockStartPos);
	buffer.ReadInteger(&m_bLockEndPos);
	buffer.ReadString(sNotes,51);
	buffer.ReadDouble(&size.wide);
	buffer.ReadDouble(&size.height);
	buffer.ReadDouble(&size.thick);
	buffer.ReadDouble(&size.thick2);
	if( (doc_type==1&&(version==0||version>=4000021))||	//TMA
		(doc_type==2&&(version==0||version>=1000076))||	//LMA
		doc_type==4||doc_type==5)	//LDS|TDA
	{
		if(bPrevDeflateVersion)
		{
			buffer.ReadInteger(&dw);
			m_bUserSpecColor=(dw!=0);
		}
		else
			buffer.ReadBooleanThin(&m_bUserSpecColor);
		if(m_bUserSpecColor)
			buffer.ReadDword(&crMaterial);
	}

	buffer.ReadInteger(&dw);
	pStart=BelongModel()->FromNodeHandle(dw);	// �����
	buffer.ReadInteger(&dw);
	pEnd=BelongModel()->FromNodeHandle(dw);	// �յ���

	if( (doc_type==1&&(version==0||version>=4000028)) ||	//TMA
		(doc_type==2&&(version==0||version>=1000083))||		//LMA
		(doc_type==4&&(version==0||version>=1000010))||		//LDS
		doc_type==5)	//TDA
		buffer.ReadWord(&m_iNormCalStyle);
	buffer.ReadDouble(&nearWorkPlaneNorm.x);
	buffer.ReadDouble(&nearWorkPlaneNorm.y);
	buffer.ReadDouble(&nearWorkPlaneNorm.z);
	buffer.ReadInteger(&hWorkNormRefPart);
	buffer.ReadDouble(&workPlaneNorm.x);	// ������ƽ�淨�߷���.
	buffer.ReadDouble(&workPlaneNorm.y);	// ������ƽ�淨�߷���.
	buffer.ReadDouble(&workPlaneNorm.z);	// ������ƽ�淨�߷���.
	if((doc_type==1&&version>0&&version<4000021)	//TMA
		||(doc_type==2&&version>0&&version<1000076))//LMA
		workPlaneNorm*=-1.0;	//�����ɰ汾�нŶ��巨�߷�����������
	f3dPoint start,end;
	buffer.ReadDouble(&start.x);// ʵ���������
	buffer.ReadDouble(&start.y);// ʵ���������
	buffer.ReadDouble(&start.z);// ʵ���������
	buffer.ReadDouble(&end.x);	// ʵ�������յ�
	buffer.ReadDouble(&end.y);	// ʵ�������յ�
	buffer.ReadDouble(&end.z);	// ʵ�������յ�
	baseline.SetStart(start);
	baseline.SetEnd(end);
#ifdef __COMMON_PART_INC_
	if( version==0||
		(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
		(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
		(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
		(doc_type==5&&version>=1030400))	//TDA(V1,3,4,0)
	{
		buffer.ReadInteger(&m_hArcLift);
		buffer.ReadPoint(m_xStartBeforeLift);
		buffer.ReadPoint(m_xEndBeforeLift);
	}
#endif
	//ʼ����ͷ�������
	buffer.ReadDouble(&start_oddment);
	buffer.ReadInteger(&desStartOdd.m_iOddCalStyle);
	buffer.ReadInteger(&desStartOdd.m_hRefPart1);
	buffer.ReadInteger(&desStartOdd.m_hRefPart2);
	buffer.ReadDouble(&desStartOdd.m_fCollideDist);
	//�ն���ͷ�������
	buffer.ReadDouble(&end_oddment);
	buffer.ReadInteger(&desEndOdd.m_iOddCalStyle);
	buffer.ReadInteger(&desEndOdd.m_hRefPart1);
	buffer.ReadInteger(&desEndOdd.m_hRefPart2);
	buffer.ReadDouble(&desEndOdd.m_fCollideDist);
	//ʼ�˶�λ����
	buffer.ReadInteger(&desStartPos.endPosType);
	if((doc_type==1&&(version==0||version>=4000030))||	//TMA
		(doc_type==2&&(version==0||version>=1000085))||	//LMA
		(doc_type==3&&(version==0||version>=1070002))||	//TSA
		(doc_type==4&&(version==0||version>=1000013))||//LDS
		doc_type==5)	//TDA
		buffer.ReadByte(&desStartPos.datum_pos_style);
	buffer.ReadInteger(&desStartPos.hDatumPart);
	buffer.ReadInteger(&desStartPos.hDatumStartPart);
	buffer.ReadInteger(&desStartPos.hDatumEndPart);
	buffer.ReadInteger(&desStartPos.bDatumEndStartEnd);
	buffer.ReadInteger(&desStartPos.bDatumStartStartEnd);
	if((doc_type==1&&(version==0||version>=4000029))||	//TMA
		(doc_type==2&&(version==0||version>=1000084))||	//LMA
		(doc_type==4&&(version==0||version>=1000011))||	//LDS
		doc_type==5)	//TDA
	{	//ƽ���淨�� 10000084
		buffer.ReadBooleanThin(&desStartPos.bUseFaceOffsetNorm);
		if(desStartPos.bUseFaceOffsetNorm)
		{
			buffer.ReadDouble(&desStartPos.face_offset_norm.x);
			buffer.ReadDouble(&desStartPos.face_offset_norm.y);
			buffer.ReadDouble(&desStartPos.face_offset_norm.z);
		}	
	}
	buffer.ReadDouble(&desStartPos.fEccentricDist);
	buffer.ReadDouble(&desStartPos.fNormOffset);
	if( (doc_type==1&&(version==0||version>=4000020))||	//TMA
		(doc_type==2&&(version==0||version>=1000075))||
		doc_type==4||doc_type==5)	//LDS|TDA
		buffer.ReadDouble(&desStartPos.fTransverseOffset);
	//�ն˶�λ����
	buffer.ReadInteger(&desEndPos.endPosType);
	if( (doc_type==1&&(version==0||version>=4000030))||	//TMA
		(doc_type==2&&(version==0||version>=1000085))||	//LMA
		(doc_type==3&&(version==0||version>=1070002))||	//TSA
		(doc_type==4&&(version==0||version>=1000013))||	//LDS
		doc_type==5)	//TDA
		buffer.ReadByte(&desEndPos.datum_pos_style);
	buffer.ReadInteger(&desEndPos.hDatumPart);
	buffer.ReadInteger(&desEndPos.hDatumStartPart);
	buffer.ReadInteger(&desEndPos.hDatumEndPart);
	buffer.ReadInteger(&desEndPos.bDatumEndStartEnd);
	buffer.ReadInteger(&desEndPos.bDatumStartStartEnd);
	if( (doc_type==1&&(version==0||version>=4000029))||	//TMA
		(doc_type==2&&(version==0||version>=1000084))||	//LMA
		(doc_type==4&&(version==0||version>=1000011))||	//LDS
		doc_type==5)	//TDA
	{	//ƽ���淨�� 10000084
		buffer.ReadBooleanThin(&desEndPos.bUseFaceOffsetNorm);
		if(desEndPos.bUseFaceOffsetNorm)
		{
			buffer.ReadDouble(&desEndPos.face_offset_norm.x);
			buffer.ReadDouble(&desEndPos.face_offset_norm.y);
			buffer.ReadDouble(&desEndPos.face_offset_norm.z);
		}	
	}
	buffer.ReadDouble(&desEndPos.fEccentricDist);
	buffer.ReadDouble(&desEndPos.fNormOffset);
	if( (doc_type==1&&(version==0||version>=4000020))||	//TMA
		(doc_type==2&&(version==0||version>=1000075))||
		doc_type==4||doc_type==5)	//LDS|TDA
		buffer.ReadDouble(&desEndPos.fTransverseOffset);
	//���������б�
	int i,nn=0;
	buffer.ReadInteger(&nn);
	RELATIVE_OBJECT *pRelaObj=NULL;
	relativeObjs.Empty();
	for(i=0;i<nn;i++)
	{
		pRelaObj=relativeObjs.Add(buffer.ReadInteger());
		if(bPrevDeflateVersion)
		{
			buffer.ReadDword(&dw);
			pRelaObj->mirInfo.axis_flag=(BYTE)dw;
			buffer.ReadInteger(&dw);
			pRelaObj->mirInfo.array_num=(BYTE)dw;
			buffer.ReadPoint(pRelaObj->mirInfo.origin);
			double ff;
			buffer.ReadDouble(&ff);
			pRelaObj->mirInfo.rotate_angle=(short)ff;
		}
		else
		{
			buffer.ReadByte(&pRelaObj->mirInfo.axis_flag);
			bool bSpecOrigin;
			buffer.ReadBooleanThin(&bSpecOrigin);
			if(bSpecOrigin)
				buffer.ReadPoint(pRelaObj->mirInfo.origin);
			if(pRelaObj->mirInfo.axis_flag&0x08)
			{
				buffer.ReadByte(&pRelaObj->mirInfo.array_num);
				buffer.ReadWord(&pRelaObj->mirInfo.rotate_angle);
			}
		}
	}
	//H�͸���˨��
	pLsRefList->FromBuffer(buffer,BelongModel(),version,doc_type);
	SetModified();
}
void CLDSLineHShape::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if(ShadowParaAndBlockIdToBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropToBuffer(buffer,version,doc_type);
		return;
	}
	buffer.WriteDword(dwPermission);
	buffer.WriteDword(dwStartPermission);
	buffer.WriteDword(dwEndPermission);
	buffer.WriteString(spec);	//���
	CfgwordToBuffer(cfgword,buffer,version,doc_type);
	buffer.WriteByte(cMaterial);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.WriteByte(m_cQualityLevel);
	buffer.WriteBooleanThin(_bVirtualPart);//��������
	buffer.WriteInteger(iSeg);
	buffer.WriteString(sPartNo);	//�������
	buffer.WriteString(layer());	//�������ͼ��.
	buffer.WriteInteger(m_uStatMatCoef);
	buffer.WriteInteger(m_hPartWeldParent);
	if(	version==0||
		(doc_type==1&&version>=5000000)||	//TMA V5.0.0.0
		(doc_type==2&&version>=5000000)||	//LMA V2.0.0.0
		(doc_type==3&&version>=2010000)||	//TSA V2.1.0.0
		(doc_type==4&&version>=1030000)||	//LDS V1.3.0.0
		(doc_type==5&&version>=1030000))	//TDA V1.3.0.0
		buffer.WriteByte(m_cFuncType);
	buffer.WriteBooleanThin(m_bDisplayHole);//��ʾ��˨�׼��и���
	buffer.WriteInteger(m_bLockStartPos);
	buffer.WriteInteger(m_bLockEndPos);
	buffer.WriteString(sNotes);
	buffer.WriteDouble(size_wide);	
	buffer.WriteDouble(size_height);
	buffer.WriteDouble(size.thick);
	buffer.WriteDouble(size.thick2);
	buffer.WriteBooleanThin(m_bUserSpecColor);	//�û�ָ��������ɫ
	if(m_bUserSpecColor)
		buffer.WriteDword(crMaterial);		//������ɫ
	if(pStart)
		buffer.WriteInteger(pStart->handle);	// �����
	else
		buffer.WriteInteger(0);
	if(pEnd)
		buffer.WriteInteger(pEnd->handle);
	else
		buffer.WriteInteger(0);

	buffer.WriteWord(m_iNormCalStyle);	//1000083
	buffer.WriteDouble(nearWorkPlaneNorm.x);
	buffer.WriteDouble(nearWorkPlaneNorm.y);
	buffer.WriteDouble(nearWorkPlaneNorm.z);
	buffer.WriteInteger(hWorkNormRefPart);
	buffer.WriteDouble(workPlaneNorm.x);	// ������ƽ�淨�߷���.
	buffer.WriteDouble(workPlaneNorm.y);	// ������ƽ�淨�߷���.
	buffer.WriteDouble(workPlaneNorm.z);	// ������ƽ�淨�߷���.
	buffer.WriteDouble(baseline.Start().x);	// ʵ���������
	buffer.WriteDouble(baseline.Start().y);	// ʵ���������
	buffer.WriteDouble(baseline.Start().z);	// ʵ���������
	buffer.WriteDouble(baseline.End().x);	// ʵ�������յ�
	buffer.WriteDouble(baseline.End().y);	// ʵ�������յ�
	buffer.WriteDouble(baseline.End().z);	// ʵ�������յ�
#ifdef __COMMON_PART_INC_
	if( version==0||
		(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
		(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
		(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
		(doc_type==5&&version>=1030400))	//TDA(V1,3,4,0)
	{
		buffer.WriteInteger(m_hArcLift);
		buffer.WritePoint(m_xStartBeforeLift);
		buffer.WritePoint(m_xEndBeforeLift);
	}
#endif
	//ʼ����ͷ�������
	buffer.WriteDouble(start_oddment);
	buffer.WriteInteger(desStartOdd.m_iOddCalStyle);
	buffer.WriteInteger(desStartOdd.m_hRefPart1);
	buffer.WriteInteger(desStartOdd.m_hRefPart2);
	buffer.WriteDouble(desStartOdd.m_fCollideDist);
	//�ն���ͷ�������
	buffer.WriteDouble(end_oddment); 
	buffer.WriteInteger(desEndOdd.m_iOddCalStyle);
	buffer.WriteInteger(desEndOdd.m_hRefPart1);
	buffer.WriteInteger(desEndOdd.m_hRefPart2);
	buffer.WriteDouble(desEndOdd.m_fCollideDist);
	//ʼ�˶�λ����
	buffer.WriteInteger(desStartPos.endPosType);
	buffer.WriteByte(desStartPos.datum_pos_style);	//1000085
	buffer.WriteInteger(desStartPos.hDatumPart);
	buffer.WriteInteger(desStartPos.hDatumStartPart);
	buffer.WriteInteger(desStartPos.hDatumEndPart);
	buffer.WriteInteger(desStartPos.bDatumEndStartEnd);
	buffer.WriteInteger(desStartPos.bDatumStartStartEnd);
	//ƽ���淨�� 10000084
	if(desStartPos.face_offset_norm.IsZero())
		desStartPos.bUseFaceOffsetNorm=false;
	else
		desStartPos.bUseFaceOffsetNorm=true;
	buffer.WriteBooleanThin(desStartPos.bUseFaceOffsetNorm);
	if(desStartPos.bUseFaceOffsetNorm)
	{
		buffer.WriteDouble(desStartPos.face_offset_norm.x);
		buffer.WriteDouble(desStartPos.face_offset_norm.y);
		buffer.WriteDouble(desStartPos.face_offset_norm.z);
	}
	buffer.WriteDouble(desStartPos.fEccentricDist);
	buffer.WriteDouble(desStartPos.fNormOffset);
	buffer.WriteDouble(desStartPos.fTransverseOffset);
	//�ն˶�λ����
	buffer.WriteInteger(desEndPos.endPosType);
	buffer.WriteByte(desEndPos.datum_pos_style);	//1000085
	buffer.WriteInteger(desEndPos.hDatumPart);
	buffer.WriteInteger(desEndPos.hDatumStartPart);
	buffer.WriteInteger(desEndPos.hDatumEndPart);
	buffer.WriteInteger(desEndPos.bDatumEndStartEnd);
	buffer.WriteInteger(desEndPos.bDatumStartStartEnd);
	//ƽ���淨�� 10000084
	if(desEndPos.face_offset_norm.IsZero())
		desEndPos.bUseFaceOffsetNorm=false;
	else
		desEndPos.bUseFaceOffsetNorm=true;
	buffer.WriteBooleanThin(desEndPos.bUseFaceOffsetNorm);
	if(desEndPos.bUseFaceOffsetNorm)
	{
		buffer.WriteDouble(desEndPos.face_offset_norm.x);
		buffer.WriteDouble(desEndPos.face_offset_norm.y);
		buffer.WriteDouble(desEndPos.face_offset_norm.z);
	}
	buffer.WriteDouble(desEndPos.fEccentricDist);
	buffer.WriteDouble(desEndPos.fNormOffset);
	buffer.WriteDouble(desEndPos.fTransverseOffset);
	//���������б�
	RELATIVE_OBJECT *pRelaObj=NULL;
	BUFFERPOP stack(&buffer,relativeObjs.GetNodeNum());
	buffer.WriteInteger(relativeObjs.GetNodeNum());
	for(pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		buffer.WriteInteger(pRelaObj->hObj);
		buffer.WriteByte(pRelaObj->mirInfo.axis_flag);
		bool bSpecOrigin=!pRelaObj->mirInfo.origin.IsZero();
		buffer.WriteBooleanThin(bSpecOrigin);
		if(bSpecOrigin)
			buffer.WritePoint(pRelaObj->mirInfo.origin);
		if(pRelaObj->mirInfo.axis_flag&0x08)
		{
			buffer.WriteByte(pRelaObj->mirInfo.array_num);
			buffer.WriteWord(pRelaObj->mirInfo.rotate_angle);
		}
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of relative object is wrong!");
#else
		CLDSObject::Log2File()->Log("����������¼�����ִ���!");
#endif

	//H�͸���˨��
	pLsRefList->ToBuffer(buffer,version,doc_type);
}
void CLDSLineHShape::ToXmlFile(FILE* fp,DWORD schema)
{
	char sText[500]="";
	fprintf(fp,"<H�͸���Ϣ dwPermission=\"%d\" dwStartPermission=\"%d\" dwEndPermission=\"%d\">\n",dwPermission,dwStartPermission,dwEndPermission);
	fprintf(fp,"<��� spec=\"%s\"/>\n",spec);
	fprintf(fp,"<��ĺ� cfgword=\"(%d,%d,%d,%d)\"/>\n",cfgword.flag.word[0],cfgword.flag.word[1],cfgword.flag.word[2],cfgword.flag.word[3]);
	fprintf(fp,"<���� cMaterial=\"%c\"/>\n",cMaterial);
	if(m_bVirtualPart)
		strcpy(sText,"true");
	else
		strcpy(sText,"false");
	fprintf(fp,"<�Ƿ��������� bVirtualPart=\"%s\"/>\n",sText);
	fprintf(fp,"<�κ� iSeg=\"%d\"/>\n",iSeg);
	fprintf(fp,"<������� sPartNo=\"%s\"/>\n",(char*)sPartNo);
	fprintf(fp,"<����ͼ�� layer=\"%s\"/>\n",layer());
	fprintf(fp,"<ͳ��ϵ�� StatMatCoef=\"%d\"/>\n",m_uStatMatCoef);
	fprintf(fp,"<���Ӹ����� hPartWeldParent=\"0x%x\"/>\n",m_hPartWeldParent);
	if(m_bDisplayHole)
		strcpy(sText,"true");
	else 
		strcpy(sText,"false");
	fprintf(fp,"<�Ƿ���ʾ��˨�� bDisplayHole=\"%s\"/>\n",sText);
	fprintf(fp,"<�Ƿ�������ʼλ�� bLockStartPos=\"%d\"/>\n",m_bLockStartPos);
	fprintf(fp,"<�Ƿ�������ֹλ�� bLockEndPos=\"%d\"/>\n",m_bLockEndPos);
	fprintf(fp,"<��ע sNotes=\"%s\"/>\n",sNotes);
	sprintf(sText,"%f",size_thick);
	SimplifiedNumString(sText);
	fprintf(fp,"<������ thick1=\"%s\"/>\n",sText);
	sprintf(sText,"%f",size_thick2);
	SimplifiedNumString(sText);
	fprintf(fp,"<��Ե��� thick2=\"%s\"/>\n",sText);
	if(m_bUserSpecColor)
		strcpy(sText,"true");
	else
		strcpy(sText,"false");
	fprintf(fp,"<�Ƿ��û�ָ��������ɫ bUserSpecColor=\"%s\"/>\n",sText);
	if(m_bUserSpecColor)
		fprintf(fp,"<������ɫ crMaterial=\"%d\"/>\n",crMaterial);
	if(pStart)
		fprintf(fp,"<����� hStart=\"0x%x\"/>\n",pStart->handle);
	else
		fprintf(fp,"<����� hStart=\"0x0\"/>\n");
	if(pEnd)
		fprintf(fp,"<�յ��� hEnd=\"0x%x\"/>\n",pEnd->handle);
	else
		fprintf(fp,"<�յ��� hEnd=\"0x0\"/>\n");
	fprintf(fp,"<��׼�淨�߼��㷽ʽ NormCalStyle=\"%d\"/>\n",m_iNormCalStyle);
	fprintf(fp,"<����ƽ�淨�߷��� nearWorkPlaneNorm=\"(%.3f,%.3f,%.3f)\"/>\n",nearWorkPlaneNorm.x,nearWorkPlaneNorm.y,nearWorkPlaneNorm.z);
	fprintf(fp,"<���ո˼� hWorkNormRefPart=\"0x%x\"/>\n",hWorkNormRefPart);
	fprintf(fp,"<������ƽ�淨�߷��� workPlaneNorm=\"(%.3f,%.3f,%.3f)\"/>\n",workPlaneNorm.x,workPlaneNorm.y,workPlaneNorm.z);
	fprintf(fp,"<ʵ������������� StartPos=\"(%.3f,%.3f,%.3f)\"/>\n",baseline.Start().x,baseline.Start().y,baseline.Start().z);
	fprintf(fp,"<ʵ�������յ����� EndPos=\"(%.3f,%.3f,%.3f)\"/>\n",baseline.End().x,baseline.End().y,baseline.End().z);
	fprintf(fp,"<ʼ����ͷ���������Ϣ oddment=\"%.2f\" OddCalStyle=\"%d\" hRefPart1=\"0x%x\" hRefPart2=\"0x%x\" CollideDist=\"%.2f\"/>\n",
		start_oddment,desStartOdd.m_iOddCalStyle,desStartOdd.m_hRefPart1,desStartOdd.m_hRefPart2,desStartOdd.m_fCollideDist);
	fprintf(fp,"<�ն���ͷ���������Ϣ oddment=\"%.2f\" OddCalStyle=\"%d\" hRefPart1=\"0x%x\" hRefPart2=\"0x%x\" CollideDist=\"%.2f\"/>\n",
		end_oddment,desEndOdd.m_iOddCalStyle,desEndOdd.m_hRefPart1,desEndOdd.m_hRefPart2,desEndOdd.m_fCollideDist);

	fprintf(fp,"<ʼ�˶�λ������Ϣ endPosType=\"%d\" datum_pos_style=\"%d\" hDatumPart=\"0x%x\" hDatumStartPart=\"0x%x\" hDatumEndPart=\"0x%x\" bDatumEndStartEnd=\"%d\" bDatumStartStartEnd=\"%d\">\n",
		desStartPos.endPosType,desStartPos.datum_pos_style,desStartPos.hDatumPart,desStartPos.hDatumStartPart,desStartPos.hDatumEndPart,desStartPos.bDatumEndStartEnd,desStartPos.bDatumStartStartEnd);
	if(desStartPos.face_offset_norm.IsZero())
	{
		desStartPos.bUseFaceOffsetNorm=false;
		strcpy(sText,"false");
	}
	else
	{
		desStartPos.bUseFaceOffsetNorm=true;
		strcpy(sText,"true");
	}
	fprintf(fp,"<�Ƿ�����ƽ���淨�� bUsefaceOffsetNorm=\"%s\"/>\n",sText);
	if(desStartPos.bUseFaceOffsetNorm)
		fprintf(fp,"<ƽ���ŷ��߷��� face_offset_norm=\"(%.3f,%.3f,%.3f)\"/>\n",desStartPos.face_offset_norm.x,desStartPos.face_offset_norm.y,desStartPos.face_offset_norm.z);
	sprintf(sText,"%f",desStartPos.fEccentricDist);
	SimplifiedNumString(sText);
	fprintf(fp,"<ƫ�ľ� EccentricDist=\"%s\"/>\n",sText);
	sprintf(sText,"%f",desStartPos.fNormOffset);
	SimplifiedNumString(sText);
	fprintf(fp,"<��׼�淨��ƫ���� NormOffset=\"%s\"/>\n",sText);
	sprintf(sText,"%f",desStartPos.fTransverseOffset);
	SimplifiedNumString(sText);
	fprintf(fp,"<����ƫ���� TransverseOffset=\"%s\"/>\n",sText);
	fprintf(fp,"</ʼ�˶�λ������Ϣ>\n");

	fprintf(fp,"<�ն˶�λ������Ϣ endPosType=\"%d\" datum_pos_style=\"%d\" hDatumPart=\"0x%x\" hDatumStartPart=\"0x%x\" hDatumEndPart=\"0x%x\" bDatumEndStartEnd=\"%d\" bDatumStartStartEnd=\"%d\">\n",
		desEndPos.endPosType,desEndPos.datum_pos_style,desEndPos.hDatumPart,desEndPos.hDatumStartPart,desEndPos.hDatumEndPart,desEndPos.bDatumEndStartEnd,desEndPos.bDatumStartStartEnd);
	if(desEndPos.face_offset_norm.IsZero())
	{
		desEndPos.bUseFaceOffsetNorm=false;
		strcpy(sText,"false");
	}
	else
	{
		desEndPos.bUseFaceOffsetNorm=true;
		strcpy(sText,"true");
	}
	fprintf(fp,"<�Ƿ�����ƽ���ŷ��� bUseFaceOffsetNorm=\"%s\"/>\n",sText);
	if(desEndPos.bUseFaceOffsetNorm)
		fprintf(fp,"<ƽ���淨�߷��� face_offset_norm=\"(%.3f,%.3f,%.3f)\"/>\n",desEndPos.face_offset_norm.x,desEndPos.face_offset_norm.y,desEndPos.face_offset_norm.z);
	sprintf(sText,"%f",desEndPos.fEccentricDist);
	SimplifiedNumString(sText);
	fprintf(fp,"<ƫ�ľ� EccentricDist=\"%s\"/>\n",sText);
	sprintf(sText,"%f",desEndPos.fNormOffset);
	SimplifiedNumString(sText);
	fprintf(fp,"<��׼�淨��ƫ���� NormOffset=\"%s\"/>\n",sText);
	sprintf(sText,"%f",desEndPos.fTransverseOffset);
	SimplifiedNumString(sText);
	fprintf(fp,"<����ƫ���� TransverseOffset=\"%s\"/>\n",sText);
	fprintf(fp,"</�ն˶�λ������Ϣ>\n");

	//���������б�
	RELATIVE_OBJECT *pRelaObj=NULL;
	fprintf(fp,"<������������ objsN=\"%d\"/>\n",relativeObjs.GetNodeNum());
	for(pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		fprintf(fp,"<����������Ϣ hObj=\"0x%x\" axis_flag=\"%d\">\n",pRelaObj->hObj,pRelaObj->mirInfo.axis_flag);
		bool bSpecOrigin=!pRelaObj->mirInfo.origin.IsZero();
		if(bSpecOrigin)
			strcpy(sText,"true");
		else
			strcpy(sText,"false");
		fprintf(fp,"<�Ƿ��жԳƻ��� bSpecOrigin=\"%s\"/>\n",sText);
		if(bSpecOrigin)
			fprintf(fp,"<�Գƻ������� origin=\"(%.3f,%.3f,%.3f)\"/>\n",pRelaObj->mirInfo.origin.x,pRelaObj->mirInfo.origin.y,pRelaObj->mirInfo.origin.z);
		if(pRelaObj->mirInfo.axis_flag&0x08)
		{
			fprintf(fp,"<�Գ���ת��Ϣ>\n");
			fprintf(fp,"<�Գ���ת���� array_num=\"%d\"/>\n",pRelaObj->mirInfo.array_num);
			fprintf(fp,"<�Գ���ת�Ƕ� rotate_angle=\"%d\"/>\n",pRelaObj->mirInfo.rotate_angle);
			fprintf(fp,"</��ת�Գ���Ϣ>\n");
		}
		fprintf(fp,"</����������Ϣ>\n");
	}
	pLsRefList->ToXmlFile(fp,schema);
	fprintf(fp,"</H�͸���Ϣ>\n");
}
//��Эͬ����ͨ���ܵ���ȡ����
void CLDSLineHShape::FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord)
{
	//��ȡӰ�乹�������͹���������� wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer))
	{
		IndependentShadowPropFromBuffer(buffer);	
		return;
	}
	long dw=0;
	buffer.ReadDword(&dwPermission);
	buffer.ReadDword(&dwStartPermission);
	buffer.ReadDword(&dwEndPermission);
	if(dwPermission&dwUpdateWord)
	{
		buffer.ReadString(spec);	//���
		buffer.Read(&cfgword.flag.word,24);
		buffer.ReadByte(&_material);
		buffer.ReadByte(&_cQualityLevel);
		buffer.ReadBooleanThin(&_bVirtualPart);//��������
		buffer.ReadInteger(&_iSeg.iSeg);
		buffer.ReadString(sPartNo);	//�������
		buffer.ReadInteger(&m_hArcLift);//Ԥ���� wht 16-01-07
		buffer.ReadString(layer(),4);	//�������ͼ��.
		buffer.ReadInteger(&m_uStatMatCoef);
		buffer.ReadInteger(&m_hPartWeldParent);
		buffer.ReadBooleanThin(&m_bDisplayHole);//��ʾ��˨�׼��и���
		buffer.ReadString(sNotes,51);
		buffer.ReadDouble(&size.wide);	
		buffer.ReadDouble(&size.height);
		buffer.ReadDouble(&size.thick);
		buffer.ReadDouble(&size.thick2);
		buffer.ReadBooleanThin(&m_bUserSpecColor);	//�û�ָ��������ɫ
		if(m_bUserSpecColor)
			buffer.ReadDword(&crMaterial);		//������ɫ
		buffer.ReadWord(&m_iNormCalStyle);
		buffer.ReadDouble(&nearWorkPlaneNorm.x);
		buffer.ReadDouble(&nearWorkPlaneNorm.y);
		buffer.ReadDouble(&nearWorkPlaneNorm.z);
		buffer.ReadInteger(&hWorkNormRefPart);
		buffer.ReadDouble(&workPlaneNorm.x);	// ������ƽ�淨�߷���.
		buffer.ReadDouble(&workPlaneNorm.y);	// ������ƽ�淨�߷���.
		buffer.ReadDouble(&workPlaneNorm.z);	// ������ƽ�淨�߷���.
		//���������б�
		int i,nn=0;
		buffer.ReadInteger(&nn);
		RELATIVE_OBJECT *pRelaObj=NULL;
		relativeObjs.Empty();
		for(i=0;i<nn;i++)
		{
			pRelaObj=relativeObjs.Add(buffer.ReadInteger());
			buffer.ReadByte(&pRelaObj->mirInfo.axis_flag);
			bool bSpecOrigin;
			buffer.ReadBooleanThin(&bSpecOrigin);
			if(bSpecOrigin)
				buffer.ReadPoint(pRelaObj->mirInfo.origin);
			if(pRelaObj->mirInfo.axis_flag&0x08)
			{
				buffer.ReadByte(&pRelaObj->mirInfo.array_num);
				buffer.ReadWord(&pRelaObj->mirInfo.rotate_angle);
			}
		}
		//H�͸���˨��
		pLsRefList->FromBuffer(buffer,m_pModel);
	}
	if(dwStartPermission&dwUpdateWord)
	{
		buffer.ReadInteger(&dw);
		pStart=BelongModel()->FromNodeHandle(dw);	// �����
		f3dPoint start;
		buffer.ReadDouble(&start.x);// ʵ���������
		buffer.ReadDouble(&start.y);// ʵ���������
		buffer.ReadDouble(&start.z);// ʵ���������
		baseline.SetStart(start);
		buffer.ReadPoint(m_xStartBeforeLift);
		//ʼ����ͷ�������
		buffer.ReadDouble(&start_oddment);
		buffer.ReadInteger(&desStartOdd.m_iOddCalStyle);
		buffer.ReadInteger(&desStartOdd.m_hRefPart1);
		buffer.ReadInteger(&desStartOdd.m_hRefPart2);
		buffer.ReadDouble(&desStartOdd.m_fCollideDist);
		//ʼ�˶�λ����
		buffer.ReadInteger(&desStartPos.endPosType);
		buffer.ReadByte(&desStartPos.datum_pos_style);
		buffer.ReadInteger(&desStartPos.hDatumPart);
		buffer.ReadInteger(&desStartPos.hDatumStartPart);
		buffer.ReadInteger(&desStartPos.hDatumEndPart);
		buffer.ReadInteger(&desStartPos.bDatumEndStartEnd);
		buffer.ReadInteger(&desStartPos.bDatumStartStartEnd);
		buffer.ReadDouble(&desStartPos.face_offset_norm.x);
		buffer.ReadDouble(&desStartPos.face_offset_norm.y);
		buffer.ReadDouble(&desStartPos.face_offset_norm.z);
		buffer.ReadDouble(&desStartPos.fEccentricDist);
		buffer.ReadDouble(&desStartPos.fNormOffset);
		buffer.ReadDouble(&desStartPos.fTransverseOffset);	
		//��Ʋ���
		buffer.ReadInteger(&m_bLockStartPos);
	}
	if(dwEndPermission&dwUpdateWord)
	{
		buffer.ReadInteger(&dw);
		pStart=BelongModel()->FromNodeHandle(dw);	// �����
		f3dPoint end;
		buffer.ReadDouble(&end.x);// ʵ���������
		buffer.ReadDouble(&end.y);// ʵ���������
		buffer.ReadDouble(&end.z);// ʵ���������
		baseline.SetEnd(end);
		buffer.ReadPoint(m_xEndBeforeLift);
		//�ն���ͷ�������
		buffer.ReadDouble(&end_oddment); 
		buffer.ReadInteger(&desEndOdd.m_iOddCalStyle);
		buffer.ReadInteger(&desEndOdd.m_hRefPart1);
		buffer.ReadInteger(&desEndOdd.m_hRefPart2);
		buffer.ReadDouble(&desEndOdd.m_fCollideDist);
		//�ն˶�λ����
		buffer.ReadInteger(&desEndPos.endPosType);
		buffer.ReadByte(&desEndPos.datum_pos_style);
		buffer.ReadInteger(&desEndPos.hDatumPart);
		buffer.ReadInteger(&desEndPos.hDatumStartPart);
		buffer.ReadInteger(&desEndPos.hDatumEndPart);
		buffer.ReadInteger(&desEndPos.bDatumEndStartEnd);
		buffer.ReadInteger(&desEndPos.bDatumStartStartEnd);
		buffer.ReadDouble(&desEndPos.face_offset_norm.x);
		buffer.ReadDouble(&desEndPos.face_offset_norm.y);
		buffer.ReadDouble(&desEndPos.face_offset_norm.z);
		buffer.ReadDouble(&desEndPos.fEccentricDist);
		buffer.ReadDouble(&desEndPos.fNormOffset);
		buffer.ReadDouble(&desEndPos.fTransverseOffset);
		//��Ʋ���
		buffer.ReadInteger(&m_bLockEndPos);
	}
	SetModified();
}
//ͨ���ܵ���Эͬ����д������
void CLDSLineHShape::ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord)
{
	//����Ӱ�乹�������͹���������� wht 12-09-14
	if(ShadowParaAndBlockIdToBuffer(buffer))
	{
		IndependentShadowPropToBuffer(buffer);
		return;
	}
	buffer.WriteDword(dwPermission);
	buffer.WriteDword(dwStartPermission);
	buffer.WriteDword(dwEndPermission);
	if(dwPermission&dwUpdateWord)
	{
		buffer.WriteString(spec);	//���
		buffer.Write(cfgword.flag.word,24);
		buffer.WriteByte(cMaterial);
		buffer.WriteByte(m_cQualityLevel);
		buffer.WriteBooleanThin(_bVirtualPart);//��������
		buffer.WriteInteger(iSeg);
		buffer.WriteString(sPartNo);	//�������
		buffer.WriteInteger(m_hArcLift);//Ԥ���� wht 16-01-07
		buffer.WriteString(layer());	//�������ͼ��.
		buffer.WriteInteger(m_uStatMatCoef);
		buffer.WriteInteger(m_hPartWeldParent);
		buffer.WriteBooleanThin(m_bDisplayHole);//��ʾ��˨�׼��и���
		buffer.WriteString(sNotes);
		buffer.WriteDouble(size_wide);	
		buffer.WriteDouble(size_height);
		buffer.WriteDouble(size_thick);
		buffer.WriteDouble(size_thick2);
		buffer.WriteBooleanThin(m_bUserSpecColor);	//�û�ָ��������ɫ
		if(m_bUserSpecColor)
			buffer.WriteDword(crMaterial);		//������ɫ

		buffer.WriteWord(m_iNormCalStyle);	//1000083
		buffer.WriteDouble(nearWorkPlaneNorm.x);
		buffer.WriteDouble(nearWorkPlaneNorm.y);
		buffer.WriteDouble(nearWorkPlaneNorm.z);
		buffer.WriteInteger(hWorkNormRefPart);
		buffer.WriteDouble(workPlaneNorm.x);	// ������ƽ�淨�߷���.
		buffer.WriteDouble(workPlaneNorm.y);	// ������ƽ�淨�߷���.
		buffer.WriteDouble(workPlaneNorm.z);	// ������ƽ�淨�߷���.
		//���������б�
		RELATIVE_OBJECT *pRelaObj=NULL;
		buffer.WriteInteger(relativeObjs.GetNodeNum());
		for(pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
		{
			buffer.WriteInteger(pRelaObj->hObj);
			buffer.WriteByte(pRelaObj->mirInfo.axis_flag);
			bool bSpecOrigin=!pRelaObj->mirInfo.origin.IsZero();
			buffer.WriteBooleanThin(bSpecOrigin);
			if(bSpecOrigin)
				buffer.WritePoint(pRelaObj->mirInfo.origin);
			if(pRelaObj->mirInfo.axis_flag&0x08)
			{
				buffer.WriteByte(pRelaObj->mirInfo.array_num);
				buffer.WriteWord(pRelaObj->mirInfo.rotate_angle);
			}
		}
		//H�͸���˨��
		pLsRefList->ToBuffer(buffer);
	}
	if(dwStartPermission&dwUpdateWord)
	{
		if(pStart)
			buffer.WriteInteger(pStart->handle);	// �����
		else
			buffer.WriteInteger(0);
		buffer.WriteDouble(baseline.Start().x);	// ʵ���������
		buffer.WriteDouble(baseline.Start().y);	// ʵ���������
		buffer.WriteDouble(baseline.Start().z);	// ʵ���������
		buffer.WritePoint(m_xStartBeforeLift);
		//ʼ����ͷ�������
		buffer.WriteDouble(start_oddment);
		buffer.WriteInteger(desStartOdd.m_iOddCalStyle);
		buffer.WriteInteger(desStartOdd.m_hRefPart1);
		buffer.WriteInteger(desStartOdd.m_hRefPart2);
		buffer.WriteDouble(desStartOdd.m_fCollideDist);
		//ʼ�˶�λ����
		buffer.WriteInteger(desStartPos.endPosType);
		buffer.WriteByte(desStartPos.datum_pos_style);
		buffer.WriteInteger(desStartPos.hDatumPart);
		buffer.WriteInteger(desStartPos.hDatumStartPart);
		buffer.WriteInteger(desStartPos.hDatumEndPart);
		buffer.WriteInteger(desStartPos.bDatumEndStartEnd);
		buffer.WriteInteger(desStartPos.bDatumStartStartEnd);
		buffer.WriteDouble(desStartPos.face_offset_norm.x);
		buffer.WriteDouble(desStartPos.face_offset_norm.y);
		buffer.WriteDouble(desStartPos.face_offset_norm.z);
		buffer.WriteDouble(desStartPos.fEccentricDist);
		buffer.WriteDouble(desStartPos.fNormOffset);
		buffer.WriteDouble(desStartPos.fTransverseOffset);
		//��Ʋ���
		buffer.WriteInteger(m_bLockStartPos);
	}
	if(dwEndPermission&dwUpdateWord)
	{
		if(pEnd)
			buffer.WriteInteger(pEnd->handle);
		else
			buffer.WriteInteger(0);
		buffer.WriteDouble(baseline.End().x);	// ʵ�������յ�
		buffer.WriteDouble(baseline.End().y);	// ʵ�������յ�
		buffer.WriteDouble(baseline.End().z);	// ʵ�������յ�
		buffer.WritePoint(m_xEndBeforeLift);
		//�ն���ͷ�������
		buffer.WriteDouble(end_oddment); 
		buffer.WriteInteger(desEndOdd.m_iOddCalStyle);
		buffer.WriteInteger(desEndOdd.m_hRefPart1);
		buffer.WriteInteger(desEndOdd.m_hRefPart2);
		buffer.WriteDouble(desEndOdd.m_fCollideDist);
		//�ն˶�λ����
		buffer.WriteInteger(desEndPos.endPosType);
		buffer.WriteByte(desEndPos.datum_pos_style);
		buffer.WriteInteger(desEndPos.hDatumPart);
		buffer.WriteInteger(desEndPos.hDatumStartPart);
		buffer.WriteInteger(desEndPos.hDatumEndPart);
		buffer.WriteInteger(desEndPos.bDatumEndStartEnd);
		buffer.WriteInteger(desEndPos.bDatumStartStartEnd);
		buffer.WriteDouble(desEndPos.face_offset_norm.x);
		buffer.WriteDouble(desEndPos.face_offset_norm.y);
		buffer.WriteDouble(desEndPos.face_offset_norm.z);
		buffer.WriteDouble(desEndPos.fEccentricDist);
		buffer.WriteDouble(desEndPos.fNormOffset);
		buffer.WriteDouble(desEndPos.fTransverseOffset);
		//��Ʋ���
		buffer.WriteInteger(m_bLockEndPos);
	}
}
void CLDSLineHShape::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/)
{
	if(_console==NULL||m_pModel==NULL||!Lock())	//�ѵ��ù�)
		return;
	_console->AddToObjSetBuf(this);
	if(pStart)
		pStart->AddToConsoleObjBuf(bAddRelaObj);
	if(pEnd)
		pEnd->AddToConsoleObjBuf(bAddRelaObj);

	for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		pLsRef->GetLsPtr()->AddToConsoleObjBuf(bAddRelaObj);
}
UCS_STRU CLDSLineHShape::BuildUCS()
{
	ucs.axis_z=End()-Start();
	ucs.origin=Start();
	if(!normalize(workPlaneNorm))
		workPlaneNorm=inters_vec(ucs.axis_z);
	ucs.axis_y=-workPlaneNorm;
	normalize(ucs.axis_z);
	ucs.axis_x=ucs.axis_y^ucs.axis_z;
	normalize(ucs.axis_x);
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	return ucs;
}
BOOL CLDSLineHShape::Create3dSolidModel(BOOL bDisplayHole,double scale_of_user2scr,double sample_len,int smoothness/*=36*/)
{
	if(CreateShadowPart3dSolidModel(bDisplayHole,scale_of_user2scr,sample_len,smoothness))
		return TRUE;
	//����δ�޸�
	if(pSolidBody!=NULL&& !IsModified())
		return FALSE;
	else
	{
		if(pSolidBody == NULL)
			pSolidBody = new CSolidBody();
		else
			pSolidBody->Empty();
		SetModified(FALSE);

		fBody body;
		fBody *pBody=&body;
		f3dPoint vertex_arr[8],vertex;
		BuildUCS();
		//��ʼ�����㼯��
		double length=DISTANCE(Start(),End());
		for(int index=0;index<3;index++)
		{
			if(index==0 || index==1)
			{	//X-|X+�ְ嶥��
				vertex_arr[0].y = -size_wide/2;
				vertex_arr[0].z = -startOdd();
				vertex_arr[1].y = size_wide/2;
				vertex_arr[1].z = -startOdd();
				vertex_arr[2].y = size_wide/2;
				vertex_arr[2].z = length+endOdd();
				vertex_arr[3].y = -size_wide/2;
				vertex_arr[3].z = length+endOdd();
			}
			else
			{	//�м����嶥��
				vertex_arr[0].x = size_height/2-size_thick2;
				vertex_arr[0].z = -startOdd();
				vertex_arr[1].x = -size_height/2+size_thick2;
				vertex_arr[1].z = -startOdd();
				vertex_arr[2].x = -size_height/2+size_thick2;
				vertex_arr[2].z = length+endOdd();
				vertex_arr[3].x = size_height/2-size_thick2;
				vertex_arr[3].z = length+endOdd();
			}
			for(int i=0;i<4;i++)
			{
				vertex=vertex_arr[i];
				if(index==0)
					vertex.x=-size_height/2;
				else if(index==1)
					vertex.x=size_height/2-size_thick2;
				else 
					vertex.y=-size_thick/2;
				pBody->vertex.append(vertex);
				//
				if(index==0)
					vertex.x=-size_height/2+size_thick2;
				else if(index==1)
					vertex.x=size_height/2;
				else 
					vertex.y=size_thick/2;
				pBody->vertex.append(vertex);
			}
		}
		//��ʼ��������
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		COLORREF crSlot = GetColor(CLS_LINESLOT,3,cMaterial);
		if(m_bUserSpecColor)	//�û�ָ��������ɫ
			crSlot=crMaterial;
		f3dPolyFace* pLeftOutsideFace=NULL,*pLeftInsideFace=NULL,
					 *pRightOutsideFace=NULL,*pRightInsideFace=NULL,
					 *pCenterTopFace=NULL,*pCenterBtmFace=NULL;
		for(int index=0;index<3;index++)
		{
			int iStart=index*8;
			//����
			f3dPolyFace* pTopFace=pBody->faceList.append();
			pTopFace->material = crSlot;
			for(int i=0;i<4;i++)
			{
				if(i==0)
					createFace.NewOutterEdgeLine(pTopFace,((i+1)%4)*2+1+iStart,1+iStart);
				else
					createFace.NewOutterEdgeLine(pTopFace,((i+1)%4)*2+1+iStart);
			}
			//����
			f3dPolyFace *pBtmFace=pBody->faceList.append();
			pBtmFace->material = crSlot;
			for(int i=0;i<4;i++)
			{
				if(i==0)
					createFace.NewOutterEdgeLine(pBtmFace,(4-1)*2+iStart,iStart);
				else
					createFace.NewOutterEdgeLine(pBtmFace,(4-1-i)*2+iStart);
			}
			//����
			for(int i=0;i<4;i++)
			{
				f3dPolyFace* pFace=pBody->faceList.append();
				pFace->material = crSlot;
				createFace.NewOutterEdgeLine(pFace,((i+1)%4)*2+iStart,i*2+iStart);
				createFace.NewOutterEdgeLine(pFace,((i+1)%4)*2+1+iStart);
				createFace.NewOutterEdgeLine(pFace,i*2+1+iStart);
				createFace.NewOutterEdgeLine(pFace,i*2+iStart);
			}
			if(index==0)
			{
				pLeftOutsideFace=pTopFace;
				pLeftInsideFace=pBtmFace;
			}
			else if(index==1)
			{
				pRightOutsideFace=pTopFace;
				pRightInsideFace=pBtmFace;
			}
			else
			{
				pCenterTopFace=pTopFace;
				pCenterBtmFace=pBtmFace;
			}
		}
		for(int i=0;i<24;i++)
		{
			if(coord_trans(pBody->vertex[i],ucs,TRUE)!=TRUE)
				return FALSE;
		}
		//������˨��
		ATOM_LIST<BOLTHOLE_INFO> left_wing_ls,right_wing_ls,middle_wing_ls;
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(m_bDisplayHole || bDisplayHole|| pLsRef->GetLsPtr()->m_bVirtualPart)
			{	//ϵͳҪ����ʾ��˨�׻�ǰ��˨Ϊ������˨
				BOLTHOLE_INFO boltHole;
				boltHole.axis_x = (*pLsRef)->ucs.axis_x;
				boltHole.norm = (*pLsRef)->get_norm();
				boltHole.bolt_d = (*pLsRef)->get_d();
				boltHole.waistLen = pLsRef->waistLen;
				boltHole.hole_d = (*pLsRef)->hole_d_increment+(*pLsRef)->get_d();
				boltHole.centre = (*pLsRef)->ucs.origin;
				double ddx=ucs.axis_x*boltHole.norm;
				double ddy=ucs.axis_y*boltHole.norm;
				if(fabs(ddx)>fabs(ddy))	
				{
					f3dPoint centre=boltHole.centre;
					coord_trans(centre,ucs,FALSE);
					if(centre.x>0)	//��(X+)��֫��˨
						right_wing_ls.append(boltHole);
					else			//��(X-)��֫��˨
						left_wing_ls.append(boltHole);
				}
				else				//�м�֫��˨
					middle_wing_ls.append(boltHole);
			}
		}
		CHoleMaker holeMaker;
		UCS_STRU hole_ucs;
		for(BOLTHOLE_INFO *pLsCir=left_wing_ls.GetFirst();pLsCir;pLsCir=left_wing_ls.GetNext())
		{	//��(X-)��֫��˨
			COLORREF crLsHole=GetColor(CLS_BOLT,ftoi(pLsCir->bolt_d),' ');
			hole_ucs.origin = pLsCir->centre;
			hole_ucs.axis_x = pLsCir->axis_x;
			hole_ucs.axis_z = pLsCir->norm;
			holeMaker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pLeftOutsideFace,pLeftInsideFace,pBody,crLsHole);
		}
		for(BOLTHOLE_INFO *pLsCir=right_wing_ls.GetFirst();pLsCir;pLsCir=right_wing_ls.GetNext())
		{	//��(X+)��֫��˨
			COLORREF crLsHole=GetColor(CLS_BOLT,ftoi(pLsCir->bolt_d),' ');
			hole_ucs.origin = pLsCir->centre;
			hole_ucs.axis_x = pLsCir->axis_x;
			hole_ucs.axis_z = pLsCir->norm;
			holeMaker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pRightOutsideFace,pRightInsideFace,pBody,crLsHole);
		}
		for(BOLTHOLE_INFO *pLsCir=middle_wing_ls.GetFirst();pLsCir;pLsCir=middle_wing_ls.GetNext())
		{	//�м�֫��˨
			COLORREF crLsHole=GetColor(CLS_BOLT,ftoi(pLsCir->bolt_d),' ');
			hole_ucs.origin = pLsCir->centre;
			hole_ucs.axis_x = pLsCir->axis_x;
			hole_ucs.axis_z = pLsCir->norm;
			holeMaker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pCenterTopFace,pCenterBtmFace,pBody,crLsHole);
		}
		pSolidBody->ConvertFrom(pBody);
	}
	return TRUE;
}

void CLDSLineHShape::CopyProperty(CLDSDbObject* pObj)
{
	if(pObj->GetClassTypeId()!=CLS_LINEHSHAPE)
		return;
	CLDSLineHShape *pHShape=(CLDSLineHShape*)pObj;
	size_height=pHShape->GetHeight();
	size_wide=pHShape->GetWidth();
	size_thick=pHShape->GetThick();
	size_thick2=pHShape->GetThick2();
	if(!IsStartPosLocked())
		desStartPos.endPosType=pHShape->desStartPos.endPosType;
	if(!IsEndPosLocked())
		desEndPos.endPosType=pHShape->desEndPos.endPosType;
	SetLayer(pHShape->layer());		
	iSeg = pHShape->iSeg;
	strcpy(sPartNo,pHShape->sPartNo);
	m_uStatMatCoef = pHShape->m_uStatMatCoef;
	is_data_modified = TRUE;
	cMaterial = pHShape->cMaterial;
	cfgword = pHShape->cfgword;
	strcpy(spec,pHShape->spec);
}
void CLDSLineHShape::CalWorkPlaneNorm()
{
	if(m_iNormCalStyle==0)
		return;	//ָ�������淨��
	CSuperSmartPtr<CLDSPart>pDatumPart;
	f3dPoint datum_vec,current_vec;
	current_vec=Start()-End();
	normalize(current_vec);
	if(hWorkNormRefPart!=0)
	{
		pDatumPart=BelongModel()->FromPartHandle(hWorkNormRefPart);
		if(pDatumPart.IsHasPtr()&&pDatumPart->IsLinePart())
		{	//ֱ���͸˼�
			datum_vec=pDatumPart.LinePartPointer()->End()-pDatumPart.LinePartPointer()->Start();
			workPlaneNorm=datum_vec^current_vec;
		}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(pDatumPart.IsHasPtr()&&pDatumPart->IsArcPart())
		{	//���θ˼�
			if(pStart&&pStart->hFatherPart==pDatumPart->handle)
				workPlaneNorm=pStart->Position(true)-f3dPoint(pDatumPart.ArcPartPointer()->baseline.Center());
			else if(pEnd&&pEnd->hFatherPart==pDatumPart.ArcPartPointer()->handle)
				workPlaneNorm=pEnd->Position(true)-f3dPoint(pDatumPart.ArcPartPointer()->baseline.Center());
			else 
			{
				datum_vec=Start()-f3dPoint(pDatumPart.ArcPartPointer()->baseline.Center());
				workPlaneNorm=(datum_vec^current_vec)^current_vec;
			}
		}
#endif
		else if(!nearWorkPlaneNorm.IsZero())
		{
			workPlaneNorm=nearWorkPlaneNorm;
			f3dPoint vec=workPlaneNorm^current_vec;
			workPlaneNorm=current_vec^vec;
		}
		normalize(workPlaneNorm);
	}
	else if(pStart&&pStart->hFatherPart>=0x20&&pStart->hFatherPart!=handle)	//ʼ�˴���������˼���
	{
		if(pStart->hFatherPart>=0x20)
			pDatumPart=BelongModel()->FromPartHandle(pStart->hFatherPart);
		else if(desStartPos.hDatumPart>=0x20)
			pDatumPart=BelongModel()->FromPartHandle(desStartPos.hDatumPart);
		if(pDatumPart.IsHasPtr()&&pDatumPart->IsLinePart())
		{	//ֱ���͸˼�
			datum_vec=pDatumPart.LinePartPointer()->End()-pDatumPart.LinePartPointer()->Start();
			normalize(datum_vec);
			if(fabs(datum_vec*current_vec)<EPS_COS2)
				workPlaneNorm=datum_vec^current_vec;
			else
			{
				workPlaneNorm = nearWorkPlaneNorm;
				f3dPoint vec=workPlaneNorm^current_vec;
				workPlaneNorm=current_vec^vec;
			}
		}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(pDatumPart&&pDatumPart->IsArcPart())
		{	//���θ˼�
			workPlaneNorm=pStart->Position(true)-f3dPoint(pDatumPart.ArcPartPointer()->baseline.Center());
		}
#endif
		normalize(workPlaneNorm);
	}
	else if(pEnd&&pEnd->hFatherPart>=0x20&&pEnd->hFatherPart!=handle)	//�ն˴���������˼���
	{
		if(pEnd->hFatherPart>=0x20)
			pDatumPart=BelongModel()->FromPartHandle(pEnd->hFatherPart);
		else if(desEndPos.hDatumPart>=0x20)
			pDatumPart=BelongModel()->FromPartHandle(desEndPos.hDatumPart);
		if(pDatumPart.IsHasPtr()&&pDatumPart->IsLinePart())
		{
			datum_vec=pDatumPart.LinePartPointer()->End()-pDatumPart.LinePartPointer()->Start();
			normalize(datum_vec);
			if(fabs(datum_vec*current_vec)<EPS_COS2)
				workPlaneNorm=datum_vec^current_vec;
			else
			{
				workPlaneNorm = nearWorkPlaneNorm;
				f3dPoint vec=workPlaneNorm^current_vec;
				workPlaneNorm=current_vec^vec;
			}
		}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(pDatumPart&&pDatumPart->IsArcPart())
		{	//���θ˼�
			workPlaneNorm=pEnd->Position(true)-f3dPoint(pDatumPart.ArcPartPointer()->baseline.Center());
		}
#endif
		normalize(workPlaneNorm);
	}
	else if(!nearWorkPlaneNorm.IsZero())
	{
		workPlaneNorm=nearWorkPlaneNorm;
		f3dPoint vec=workPlaneNorm^current_vec;
		workPlaneNorm=current_vec^vec;
		normalize(workPlaneNorm);
	}
	if(nearWorkPlaneNorm*workPlaneNorm<0)
		workPlaneNorm=-workPlaneNorm;
}
bool CLDSLineHShape::IsEqual(CLDSLineHShape* pMirHShape,double epsilon/*=1.0*/, char *error_info/*=NULL*/,
	int *pErrorInfo/*=NULL*/,DWORD *pChkStyleFlag/*=NULL*/)
{
	return false;
}
int CLDSLineHShape::GetSpecification(char *h_spec,BOOL bIncMat/*=TRUE*/)
{
	char steelmark[20]="";
	char tmp_spec[200];
	if(bIncMat&&cMaterial!=CSteelMatLibrary::Q235BriefMark())//'S')
	{
		QuerySteelMatMark(cMaterial,steelmark);
		if(strlen(spec)>0)
			sprintf(tmp_spec,"%s-%s",steelmark,spec);
		else 
			sprintf(tmp_spec,"%s-%fX%f",steelmark,size_height,size_wide);
	}
	else
	{
		if(strlen(spec)>0)
			sprintf(tmp_spec,"%s",spec);
		else
			sprintf(tmp_spec,"%fX%f",size_height,size_wide);
	}
	SimplifiedNumString(tmp_spec);
	if(h_spec)
		strcpy(h_spec,tmp_spec);
	return strlen(h_spec);
}
double CLDSLineHShape::GetWeight(BOOL bSolidLength/*=TRUE*/)
{
	double length;
	if(bSolidLength||pStart==NULL||pEnd==NULL)
		length=GetLength();
	else
		length=DISTANCE(pStart->Position(false),pEnd->Position(false));
	return CalSteelPlankWei((size_height-2*size_thick+2*size_wide)*size_thick*length);
}
void CLDSLineHShape::CalStartPos(CLDSArcLift *pLift/*=NULL*/)
{
	if(!LockStart())
		return;	//�ѵ�λ
	if(pStart==NULL)
		return;
#ifdef __COMPLEX_PART_INC_
	if(pLift==NULL&&m_hArcLift>0x20)
		pLift=BelongModel()->FromArcLiftHandle(m_hArcLift);
#else
	pLift=NULL;
#endif
	if(IsStartPosLocked())
		return;	//ʼ��λ���ѱ������������ǻ����㣩
	SetModified();
	if(desStartPos.endPosType==0)
		SetStart(pStart->Position(true)+workPlaneNorm*desStartPos.fNormOffset);
	else if(desStartPos.endPosType==1)
	{	//�������
		long hDatumPart=0;
		if(desStartPos.hDatumPart>0x20)
			hDatumPart=desStartPos.hDatumPart;
		else if(pStart)
			hDatumPart=pStart->hFatherPart;
		CLDSPart *pPart=BelongModel()->FromPartHandle(hDatumPart);
		if(pPart==NULL)
			return;
		else if(pPart->IsLinePart())
		{
			f3dPoint startPos;
			if(pPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)pPart;
				if(desStartPos.datum_pos_style==0)	//�Ը��Ǹ�����Ϊ��׼
					startPos = pDatumLineAngle->GetDatumPosBer(pStart);
				else if(desStartPos.datum_pos_style==1)	//�Ը��Ǹ�X֧Ϊ��׼
					startPos = pDatumLineAngle->GetDatumPosWingX(pStart);
				else if(desStartPos.datum_pos_style==2)	//�Ը��Ǹ�Y֧Ϊ��׼
					startPos = pDatumLineAngle->GetDatumPosWingY(pStart);
				else
				{
					f3dPoint face_norm=desStartPos.face_offset_norm;
					if(pLift)
					{
						bool bInRightSide=true;
						if(pLift->m_cRotAxis=='Y'&&pStart->Position(false).x<0)
							bInRightSide=false;
						else if(pLift->m_cRotAxis=='X'&&pStart->Position(false).y>0)
							bInRightSide=false;
						face_norm=pLift->LiftVecTrans(face_norm,bInRightSide);
					}
					if(desStartPos.datum_pos_style==3)	//�Ը��Ǹ�����Ϊ��׼ƽ��
						startPos = pDatumLineAngle->GetDatumPosBerByFaceOffset(pStart,face_norm);
					else if(desStartPos.datum_pos_style==4)	//�Ը��Ǹ�X֫Ϊ��׼ƽ��
						startPos = pDatumLineAngle->GetDatumPosWingXByFaceOffset(pStart,face_norm);
					else //if(desStartPos.datum_pos_style==5)//�Ը��Ǹ�Y֫Ϊ��׼ƽ��
						startPos = pDatumLineAngle->GetDatumPosWingYByFaceOffset(pStart,face_norm);
				}
			}
			else
			{
				CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
				f3dLine datumLine(pLinePart->Start(),pLinePart->End());
				f3dPoint datum_vec=datumLine.endPt-datumLine.startPt;
				f3dPoint current_vec=End()-Start();
				//current_vecΪ0ʱʹ�õ��߷��� wht 10-09-09
				if(current_vec.IsZero()&&pStart&&pEnd)
					current_vec=pEnd->Position(true)-pStart->Position(true);
				f3dPoint datumPos,plane_norm,eccentricOffsetVec,transverseOffsetVec;
				plane_norm=datum_vec^current_vec;
				normalize(plane_norm);
				if(plane_norm*workPlaneNorm<0)
					plane_norm=-plane_norm;
				SnapPerp(&datumPos,datumLine,pStart->Position(true));
				eccentricOffsetVec=plane_norm^datum_vec;
				if(eccentricOffsetVec*current_vec<0)
					eccentricOffsetVec*=-1.0;
				normalize(eccentricOffsetVec);
				transverseOffsetVec=plane_norm^current_vec;
				normalize(transverseOffsetVec);	//����ƫ�Ʒ���
				startPos=datumPos+eccentricOffsetVec*desStartPos.fEccentricDist
					+transverseOffsetVec*desStartPos.fTransverseOffset;
			}
			startPos+=workPlaneNorm*desStartPos.fNormOffset;
			SetStart(startPos);
		}
		else if(pPart->IsArcPart())
		{
			CLDSArcPart *pArcPart=(CLDSArcPart*)pPart;
			f3dPoint datumPos=pArcPart->GetDatumPos(pStart->Position(true));
			SetStart(datumPos);
		}
	}
	else if(desStartPos.endPosType==2)
	{	//����ͶӰ
		CLDSLineHShape *pStartDatumFlat=(CLDSLineHShape*)BelongModel()->FromPartHandle(desStartPos.hDatumStartPart,CLS_LINEHSHAPE);
		CLDSLineHShape *pEndDatumFlat=(CLDSLineHShape*)BelongModel()->FromPartHandle(desStartPos.hDatumEndPart,CLS_LINEHSHAPE);
		if(pStartDatumFlat==NULL||pEndDatumFlat==NULL)
			return;
		f3dPoint datumPosStart,datumPosEnd,pos;
		if(desStartPos.bDatumStartStartEnd)
			datumPosStart=pStartDatumFlat->Start();
		else
			datumPosStart=pStartDatumFlat->End();
		if(desStartPos.bDatumEndStartEnd)
			datumPosEnd=pEndDatumFlat->Start();
		else
			datumPosEnd=pEndDatumFlat->End();
		SnapPerp(&pos,datumPosStart,datumPosEnd,pStart->Position(true));
		SetStart(pos);
	}
	//�������߽����Լ�ָ���������ֶ�λ��ʽ wht 10-10-20
	else if(desStartPos.endPosType==3)
	{	//���߽���(���˼�����)
		CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(desStartPos.hDatumPart,CLS_LINETUBE);		//��׼�ֹ�
		CLDSLineTube *pCrossTube=(CLDSLineTube*)BelongModel()->FromPartHandle(desStartPos.hDatumStartPart,CLS_LINETUBE);//����ֹ�
		if(pDatumTube==NULL||pCrossTube==NULL)
			return;
		f3dPoint inters_pt;
		f3dLine datum_line,cross_line;
		datum_line.startPt=pDatumTube->Start();
		datum_line.endPt=pDatumTube->End();
		cross_line.startPt=pCrossTube->Start();
		cross_line.endPt=pCrossTube->End();
		int ret=Int3dll(datum_line,cross_line,inters_pt);
		if(ret==-2||ret==1||ret==2)
		{
			inters_pt+=workPlaneNorm*desEndPos.fNormOffset;//���Ƿ���ƫ���� wht 11-07-01
			SetStart(inters_pt);
		}
		else	//δ��úϷ����� 
			return;
	}
	else if(desStartPos.endPosType==4)//ֱ��ָ������
		return;
}
void CLDSLineHShape::CalEndPos(CLDSArcLift *pLift/*=NULL*/)
{
	if(!LockEnd())
		return;	//�ѵ�λ
	if(pEnd==NULL)
		return;
#ifdef __COMPLEX_PART_INC_
	if(pLift==NULL&&m_hArcLift>0x20)
		pLift=BelongModel()->FromArcLiftHandle(m_hArcLift);
#else
	pLift=NULL;
#endif
	if(IsEndPosLocked())
		return;	//�ն�λ���ѱ������������ǻ����㣩
	SetModified();
	if(desEndPos.endPosType==0)
		SetEnd(pEnd->Position(true)+workPlaneNorm*desEndPos.fNormOffset);
	else if(desEndPos.endPosType==1)
	{	//�������
		long hDatumPart=0;
		if(desEndPos.hDatumPart>0x20)
			hDatumPart=desEndPos.hDatumPart;
		else if(pEnd)
			hDatumPart=pEnd->hFatherPart;
		CLDSPart *pPart=BelongModel()->FromPartHandle(hDatumPart);
		if(pPart==NULL)
			return;
		else if(pPart->IsLinePart())
		{
			f3dPoint endPos;
			if(pPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)pPart;
				if(desEndPos.datum_pos_style==0)	//�Ը��Ǹ�����Ϊ��׼
					endPos = pDatumLineAngle->GetDatumPosBer(pEnd);
				else if(desEndPos.datum_pos_style==1)	//�Ը��Ǹ�X֧Ϊ��׼
					endPos = pDatumLineAngle->GetDatumPosWingX(pEnd);
				else if(desEndPos.datum_pos_style==2)	//�Ը��Ǹ�Y֧Ϊ��׼
					endPos = pDatumLineAngle->GetDatumPosWingY(pEnd);
				else
				{
					f3dPoint face_norm=desEndPos.face_offset_norm;
					if(pLift)
					{
						bool bInRightSide=true;
						if(pLift->m_cRotAxis=='Y'&&pStart->Position(false).x<0)
							bInRightSide=false;
						else if(pLift->m_cRotAxis=='X'&&pStart->Position(false).y>0)
							bInRightSide=false;
						face_norm=pLift->LiftVecTrans(face_norm,bInRightSide);
					}
					if(desEndPos.datum_pos_style==3)	//�Ը��Ǹ�����Ϊ��׼ƽ��
						endPos = pDatumLineAngle->GetDatumPosBerByFaceOffset(pEnd,face_norm);
					else if(desEndPos.datum_pos_style==4)	//�Ը��Ǹ�X֫Ϊ��׼ƽ��
						endPos = pDatumLineAngle->GetDatumPosWingXByFaceOffset(pEnd,face_norm);
					else //if(desEndPos.datum_pos_style==5)//�Ը��Ǹ�Y֫Ϊ��׼ƽ��
						endPos = pDatumLineAngle->GetDatumPosWingYByFaceOffset(pEnd,face_norm);
				}
			}
			else
			{
				CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
				f3dLine datumLine(pLinePart->Start(),pLinePart->End());
				f3dPoint datum_vec=datumLine.endPt-datumLine.startPt;
				f3dPoint current_vec=Start()-End();
				//current_vecΪ0ʱʹ�õ��߷��� wht 10-09-09
				if(current_vec.IsZero()&&pStart&&pEnd)
					current_vec=pEnd->Position(true)-pStart->Position(true);
				f3dPoint datumPos,plane_norm,eccentricOffsetVec,transverseOffsetVec;
				plane_norm=datum_vec^current_vec;
				normalize(plane_norm);
				if(plane_norm*workPlaneNorm<0)
					plane_norm = -plane_norm;
				SnapPerp(&datumPos,datumLine,pEnd->Position(true));
				eccentricOffsetVec=plane_norm^datum_vec;
				if(eccentricOffsetVec*current_vec<0)
					eccentricOffsetVec*=-1.0;
				normalize(eccentricOffsetVec);
				transverseOffsetVec=plane_norm^current_vec;
				normalize(transverseOffsetVec);	//����ƫ�Ʒ���
				endPos=datumPos+eccentricOffsetVec*desEndPos.fEccentricDist
					+transverseOffsetVec*desEndPos.fTransverseOffset;
			}
			endPos+=workPlaneNorm*desEndPos.fNormOffset;
			SetEnd(endPos);
		}
		else if(pPart->IsArcPart())
		{
			CLDSArcPart *pArcPart=(CLDSArcPart*)pPart;
			f3dPoint datumPos=pArcPart->GetDatumPos(pEnd->Position(true));
			SetEnd(datumPos);
		}
	}
	else if(desEndPos.endPosType==2)
	{	//����ͶӰ
		CLDSLineHShape *pStartDatumFlat=(CLDSLineHShape*)BelongModel()->FromPartHandle(desEndPos.hDatumStartPart,CLS_LINEHSHAPE);
		CLDSLineHShape *pEndDatumFlat=(CLDSLineHShape*)BelongModel()->FromPartHandle(desEndPos.hDatumEndPart,CLS_LINEHSHAPE);
		if(pStartDatumFlat==NULL||pEndDatumFlat==NULL)
			return;
		if(pStartDatumFlat!=this)
			pStartDatumFlat->CalPosition();
		if(pEndDatumFlat!=this)
			pEndDatumFlat->CalPosition();
		f3dPoint datumPosStart,datumPosEnd,pos;
		if(desEndPos.bDatumStartStartEnd)
			datumPosStart=pStartDatumFlat->Start();
		else
			datumPosStart=pStartDatumFlat->End();
		if(desEndPos.bDatumEndStartEnd)
			datumPosEnd=pEndDatumFlat->Start();
		else
			datumPosEnd=pEndDatumFlat->End();
		SnapPerp(&pos,datumPosStart,datumPosEnd,pEnd->Position(true));
		SetEnd(pos);
	}
	//�������߽����Լ�ָ���������ֶ�λ��ʽ wht 10-10-20
	else if(desEndPos.endPosType==3)
	{	//���߽���(���˼�����)
		CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(desEndPos.hDatumPart,CLS_LINETUBE);		//��׼�ֹ�
		CLDSLineTube *pCrossTube=(CLDSLineTube*)BelongModel()->FromPartHandle(desEndPos.hDatumStartPart,CLS_LINETUBE);//����ֹ�
		if(pDatumTube==NULL||pCrossTube==NULL)
			return;
		f3dPoint inters_pt;
		f3dLine datum_line,cross_line;
		datum_line.startPt=pDatumTube->Start();
		datum_line.endPt=pDatumTube->End();
		cross_line.startPt=pCrossTube->Start();
		cross_line.endPt=pCrossTube->End();
		int ret=Int3dll(datum_line,cross_line,inters_pt);
		if(ret==-2||ret==1||ret==2)
		{
			inters_pt+=workPlaneNorm*desEndPos.fNormOffset;//���Ƿ���ƫ���� wht 11-07-01
			SetEnd(inters_pt);
		}
		else	//δ��úϷ����� 
			return;
	}
	else if(desEndPos.endPosType==4)//ֱ��ָ������
		return;
}
void CLDSLineHShape::CalPosition(bool bIgnoreRepeatFlag/*=false*/)
{
	if(IsShadowObject())
	{
		CalShadowRodPos();
		return;
	}
	if(bIgnoreRepeatFlag)
		ClearFlag();
	CLDSArcLift *pLift=NULL;
	if(m_hArcLift>0x20)
		pLift=BelongModel()->FromArcLiftHandle(m_hArcLift);
	CalStartPos(pLift);
	CalEndPos(pLift);
}
//���Թ���
const DWORD HASHTABLESIZE = 500;
const DWORD STATUSHASHTABLESIZE = 100;
CHashStrList<PROPLIST_ITEM> CLDSLineHShape::propHashtable;
CXhChar100 CLDSLineHShape::HELPLINK_URL;
CHashSet <PROPLIST_ITEM*> CLDSLineHShape::hashPropItemById;
CHashTable<DWORD> CLDSLineHShape::propStatusHashtable;
int CLDSLineHShape::m_iCurDisplayPropGroup;
void CLDSLineHShape::InitLineHShapePropHashtable()
{
	int id=1;
	CLDSLineHShape::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSLineHShape::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	//������Ϣ
	CLDSLineHShape::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"H-steel Basic Information","H-steel's basic information"));
	CLDSLineHShape::AddPropItem("handle",PROPLIST_ITEM(id++,"handle","H-steel handle"));
	CLDSLineHShape::AddPropItem("layer",PROPLIST_ITEM(id++,"layer","The layer of U-steel"));
	CLDSLineHShape::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"Material","H-steel material"));
	CLDSLineHShape::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"Quality Grade","H-steel quality grade"));
	CLDSLineHShape::AddPropItem("specification",PROPLIST_ITEM(id++,"Specification","H-steel's specification"));
	CLDSLineHShape::AddPropItem("thick1",PROPLIST_ITEM(id++,"Flank Thick","H-steel's flank thick"));
	CLDSLineHShape::AddPropItem("thick2",PROPLIST_ITEM(id++,"Link Thick","U-steel's thick"));
	CLDSLineHShape::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number","Segment Number"));
	CLDSLineHShape::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"Part Number","Part Number"));
	CLDSLineHShape::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word","Configure Word"));
	CLDSLineHShape::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"Relative Object","The object relate to currently object and mirror style"));
	CLDSLineHShape::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Project Information","Project Information"));//Ӱ����Ϣ
	CLDSLineHShape::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Shadow Mother Object","The shadow mother object's handle of currently part,when it's 0x0 means that the currently object is original object��"));//Ӱ��ĸ����
	CLDSLineHShape::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly","The handle of vest in block assembly ��"));//�����Ĳ�������
	CLDSLineHShape::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block","The handle of vest in block��"));//�����Ĳ���
	CLDSLineHShape::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"BOM Number","BOM Number"));
	CLDSLineHShape::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"Weld Parent Part"," if the part needs to weld to other parts, then this attribute is the handle of attached part; If the part does not need to be welded, the value is 0x0��"));
	CLDSLineHShape::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"Hole Visible","Whethe to show the hole","Yes|No"));
	CLDSLineHShape::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"Virtual Auxiliary Part","Virtual auxiliary parts are only used to assist other part's position, is not the real statistic material of parts,so not contributing to the material statistics��","Yes|No"));
	CLDSLineHShape::AddPropItem("length",PROPLIST_ITEM(id++,"length","H-steel's physical length"));
	CLDSLineHShape::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"Specify Part Color","Whether the part's color is specified by the user","Yes|No"));
	CLDSLineHShape::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Part Color","Part Color"));
	CLDSLineHShape::AddPropItem("sNotes",PROPLIST_ITEM(id++,"Notes","Notes"));
	//��λ��Ϣ
	CLDSLineHShape::AddPropItem("specialSlot",PROPLIST_ITEM(id++,"Special Position Parameter"));
	CLDSLineHShape::AddPropItem("hAttachTube",PROPLIST_ITEM(id++,"Attached Tube"));
	CLDSLineHShape::AddPropItem("fLen",PROPLIST_ITEM(id++,""));
	CLDSLineHShape::AddPropItem("desDatumPoint",PROPLIST_ITEM(id++,"Location Postion"));
	CLDSLineHShape::AddPropItem("desDatumPoint.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineHShape::AddPropItem("desDatumPoint.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineHShape::AddPropItem("desDatumPoint.z",PROPLIST_ITEM(id++,"Z"));
	CLDSLineHShape::AddPropItem("desDatumPoint.param.hOffsetDatumNode",PROPLIST_ITEM(id++,"Offset from node"));
	CLDSLineHShape::AddPropItem("desDatumPoint.param.lenOffsetType",PROPLIST_ITEM(id++,"Offset Direction"));
	CLDSLineHShape::AddPropItem("desDatumPoint.param.lenOffset",PROPLIST_ITEM(id++,"Offset Distance"));
	CLDSLineHShape::AddPropItem("desDatumPoint.param.RayVecStyle",PROPLIST_ITEM(id++,"Ray Vector"));
	CLDSLineHShape::AddPropItem("desDatumPoint.param.RayVecStdStyle",PROPLIST_ITEM(id++,"Specify"));
	CLDSLineHShape::AddPropItem("desDatumPoint.param.RayVec.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineHShape::AddPropItem("desDatumPoint.param.RayVec.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineHShape::AddPropItem("desDatumPoint.param.RayVec.z",PROPLIST_ITEM(id++,"Z"));
	CLDSLineHShape::AddPropItem("desDatumPoint.param.RayVecRotateAngle",PROPLIST_ITEM(id++,"Rotate Angle"));
	CLDSLineHShape::AddPropItem("workPlaneNorm",PROPLIST_ITEM(id++,"Work Plane Normal","H�͸ֻ�׼�淨��"));
	CLDSLineHShape::AddPropItem("hWorkNormRefPart",PROPLIST_ITEM(id++,"Normal Reference Rod","It is work reference rod when calculating datum plane's normal��It is start node or end node of parent rod when it is 0X0(It can't be rod itself)"));
	CLDSLineHShape::AddPropItem("workPlaneNorm.x",PROPLIST_ITEM(id++,"X Coordinate","The x coordinate of datum plane's normal"));
	CLDSLineHShape::AddPropItem("workPlaneNorm.y",PROPLIST_ITEM(id++,"Y Coordinate","The y coordinate of datum plane's normal"));
	CLDSLineHShape::AddPropItem("workPlaneNorm.z",PROPLIST_ITEM(id++,"Z Coordinate","The z coordinate of datum plane's normal"));
	CLDSLineHShape::AddPropItem("nearWorkPlaneNorm",PROPLIST_ITEM(id++,"Datum Plane Near Normal","Its generally specified by the user, the system can calculate it according to the near normals and channel steel axis��"));
	CLDSLineHShape::AddPropItem("m_iNormCalStyle",PROPLIST_ITEM(id++,"Calculate Mode","ֱ�Ӹ�������ͨ�����˼�����","0.Specify Directly|1.Reference Rod|2.Horizontal Pedestal"));
	CLDSLineHShape::AddPropItem("nearWorkPlaneNorm.x",PROPLIST_ITEM(id++,"X Coordinate","The x coordinate of datum plane's normal"));
	CLDSLineHShape::AddPropItem("nearWorkPlaneNorm.y",PROPLIST_ITEM(id++,"Y Coordinate","The y coordinate of datum plane's normal"));
	CLDSLineHShape::AddPropItem("nearWorkPlaneNorm.z",PROPLIST_ITEM(id++,"Z Coordinate","The z coordinate of datum plane's normal"));
	//ʼ����Ϣ
	CLDSLineHShape::AddPropItem("startInfo",PROPLIST_ITEM(id++,"Start Information","H-steel's start information"));
	CLDSLineHShape::AddPropItem("hStart",PROPLIST_ITEM(id++,"Start Handle","H-steel's start handle"));
	CLDSLineHShape::AddPropItem("oddStart",PROPLIST_ITEM(id++,"Rod Oddment","H-steel's start oddment"));
	CLDSLineHShape::AddPropItem("startPos",PROPLIST_ITEM(id++,"Start Design Coordinate","H-steel's start design coordinate"));
	CLDSLineHShape::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"Start Actual Coordinate","The start coordinate of  H-steel after considering oddment"));
	CLDSLineHShape::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X Coordinate","The H-steel's start x coordinate"));
	CLDSLineHShape::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y Coordinate","The H-steel's start y coordinate"));
	CLDSLineHShape::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z Coordinate","The H-steel's start z coordinate"));
	CLDSLineHShape::AddPropItem("desStartPos",PROPLIST_ITEM(id++,""));
	CLDSLineHShape::AddPropItem("desStartPos.endPosType",PROPLIST_ITEM(id++,"Locate Mode","The datum positon of H-steel angle's start ","Node Is Datum Node|Lap Joint Connect|Point Line Project|Center Line Cross Point|Specified Coordinate|"));
	CLDSLineHShape::AddPropItem("desStartPos.datum_pos_style",PROPLIST_ITEM(id++,"Datum Point Locate Mode"��"The start locate mode of H-steel"));
	CLDSLineHShape::AddPropItem("desStartPos.hDatumPart",PROPLIST_ITEM(id++,"Lap Joint Datum Angle","The handle of lap joint datum linePart"));
	CLDSLineHShape::AddPropItem("desStartPos.fNormOffset",PROPLIST_ITEM(id++,"Normal Offset"));
	CLDSLineHShape::AddPropItem("desStartPos.fEccentricDist",PROPLIST_ITEM(id++,"Offset Distance"));
	CLDSLineHShape::AddPropItem("desStartPos.face_offset_norm",PROPLIST_ITEM(id++,"Offset Normal"));
	CLDSLineHShape::AddPropItem("desStartPos.fTransverseOffset",PROPLIST_ITEM(id++,"Lateral Offset","The offset of x negative direction"));
	CLDSLineHShape::AddPropItem("desStartPos.hDatumStartPart",PROPLIST_ITEM(id++,"Start Datum Rod","The datum rod  is at project line's start"));
	CLDSLineHShape::AddPropItem("desStartPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,"Datum Endpoint","The datum h steel's datum endpoint"));
	CLDSLineHShape::AddPropItem("desStartPos.hDatumEndPart",PROPLIST_ITEM(id++,"End Datum Rod","The datum rod at project line's end"));
	CLDSLineHShape::AddPropItem("desStartPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"Datum Endpoint","The datum rod's datum endpoint"));
	//�ն���Ϣ
	CLDSLineHShape::AddPropItem("endInfo",PROPLIST_ITEM(id++,"End Information","H-steel's end information"));
	CLDSLineHShape::AddPropItem("hEnd",PROPLIST_ITEM(id++,"End Handle","U-steel's end handle"));
	CLDSLineHShape::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"Rod Oddment","H-steel's end oddment"));
	CLDSLineHShape::AddPropItem("endPos",PROPLIST_ITEM(id++,"U-Steel End Design Coordinate","The design coodinate of U-steel's end"));
	CLDSLineHShape::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"End Actual Coordinatem","The end coordinate of  H-steel after considering oddment"));
	CLDSLineHShape::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X Coordinate","The x coordinate of H-steel angle's end"));
	CLDSLineHShape::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y Coordinate","The y coordinate of H-steel angle's end"));
	CLDSLineHShape::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z Coordinate","The z coordinate of H-steel angle's end"));
	CLDSLineHShape::AddPropItem("desEndPos",PROPLIST_ITEM(id++,""));
	CLDSLineHShape::AddPropItem("desEndPos.endPosType",PROPLIST_ITEM(id++,"Locate Mode","The datum positon of H-steel angle's end","Node Is Datum Node|Lap Joint Connect|Point Line Project|Center Line Cross Point|Specified Coordinate|"));
	CLDSLineHShape::AddPropItem("desEndPos.datum_pos_style",PROPLIST_ITEM(id++,"Datum Point Locate Mode","The end locate mode of H-steel"));
	CLDSLineHShape::AddPropItem("desEndPos.hDatumPart",PROPLIST_ITEM(id++,"Lap Joint Datum Angle","The handle of lap joint datum linePart"));
	CLDSLineHShape::AddPropItem("desEndPos.fNormOffset",PROPLIST_ITEM(id++,"Normal Offset"));
	CLDSLineHShape::AddPropItem("desEndPos.fEccentricDist",PROPLIST_ITEM(id++,"Offset Distance"));
	CLDSLineHShape::AddPropItem("desEndPos.face_offset_norm",PROPLIST_ITEM(id++,"Offset Normal"));
	CLDSLineHShape::AddPropItem("desEndPos.fTransverseOffset",PROPLIST_ITEM(id++,"Lateral Offset","The offset of x negative direction"));
	CLDSLineHShape::AddPropItem("desEndPos.hDatumStartPart",PROPLIST_ITEM(id++,"Start Datum Rod","The datum rod  is at project line's start"));
	CLDSLineHShape::AddPropItem("desEndPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,"Datum Endpoint","The datum h steel's datum endpoint"));
	CLDSLineHShape::AddPropItem("desEndPos.hDatumEndPart",PROPLIST_ITEM(id++,"End Datum Rod","The datum rod at project line's end"));
	CLDSLineHShape::AddPropItem("desEndPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"Datum Endpoint","The datum rod's datum endpoint"));
#else
	//������Ϣ
	CLDSLineHShape::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"H�͸ֻ�����Ϣ","H�͸ֻ�����Ϣ"));
	CLDSLineHShape::AddPropItem("handle",PROPLIST_ITEM(id++,"���","H�͸־��"));
	CLDSLineHShape::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��","�۸�����ͼ��"));
	CLDSLineHShape::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�","��ĺ�"));
	CLDSLineHShape::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�","�ֶκ�"));
	CLDSLineHShape::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������","�������"));
	CLDSLineHShape::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����","H�͸ֲ���"));
	CLDSLineHShape::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"�����ȼ�","�����ȼ�"));
	CLDSLineHShape::AddPropItem("ibHShapeType",PROPLIST_ITEM(id++,"���","H�͸ֵ����","0.HW|1.HM|2.HN|3.HP"));
	CLDSLineHShape::AddPropItem("specification",PROPLIST_ITEM(id++,"���","H�͸ֵĹ��"));
	CLDSLineHShape::AddPropItem("thick1",PROPLIST_ITEM(id++,"������","H�͸ֵĸ�����"));
	CLDSLineHShape::AddPropItem("thick2",PROPLIST_ITEM(id++,"��Ե���","H�͸ֵ���Ե���"));
	CLDSLineHShape::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"ͳ��ϵ��","ͳ��ϵ��"));
	CLDSLineHShape::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"���Ӹ�����","����˹�����Ҫ���ӵ�����������,��ô�����Ծ��Ǳ������������ĺ��ӹ����ľ��;����˹�������Ҫ����,��ֵΪ0X0��"));
	CLDSLineHShape::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"��ʾ��","�Ƿ���ʾ��","��|��"));
	CLDSLineHShape::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"���⸨������","���⸨������ֻ���ڸ�������������λ�������ǽṹ����ʵ���е�ͳ�Ĺ��������Բ���Ԥ����ͳ�ơ�","��|��"));
	CLDSLineHShape::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"ָ��������ɫ","������ɫ�Ƿ�Ϊ�û�ָ��","��|��"));
	CLDSLineHShape::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"������ɫ","������ɫ"));
	CLDSLineHShape::AddPropItem("length",PROPLIST_ITEM(id++,"ʵ�ʳ���","H�͸ֵ�ʵ�ʳ���"));
	CLDSLineHShape::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"��������","�뵱ǰ����������Ĺ����Լ��ԳƷ�ʽ"));
	CLDSLineHShape::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Ӱ����Ϣ","Ӱ����Ϣ"));//Ӱ����Ϣ
	CLDSLineHShape::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Ӱ��ĸ����","��ǰ������Ӧ��Ӱ��ĸ����������þ��Ϊ0x0ʱ��ʾ��ǰ����Ϊԭʼ������"));//Ӱ��ĸ����
	CLDSLineHShape::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"�����Ĳ�������","��ǰ���������Ĳ������þ����"));//�����Ĳ�������
	CLDSLineHShape::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"��������","��ǰ���������Ĳ��������"));//�����Ĳ���
	CLDSLineHShape::AddPropItem("sNotes",PROPLIST_ITEM(id++,"��ע","��ע"));
	//��׼����Ϣ
	CLDSLineHShape::AddPropItem("workPlaneNorm",PROPLIST_ITEM(id++,"��׼�淨��","H�͸ֻ�׼�淨��"));
	CLDSLineHShape::AddPropItem("m_iNormCalStyle",PROPLIST_ITEM(id++,"���㷽ʽ","ֱ�Ӹ�������ͨ�����˼�����","0.ֱ��ָ��|1.���ո˼�|2.ˮƽ����"));
	CLDSLineHShape::AddPropItem("workPlaneNorm.x",PROPLIST_ITEM(id++,"X�������","�۸ֻ�׼�淨��X�᷽�������"));
	CLDSLineHShape::AddPropItem("workPlaneNorm.y",PROPLIST_ITEM(id++,"Y�������","�۸ֻ�׼�淨��Y�᷽�������"));
	CLDSLineHShape::AddPropItem("workPlaneNorm.z",PROPLIST_ITEM(id++,"Z�������","�۸ֻ�׼�淨��Z�᷽�������"));
	CLDSLineHShape::AddPropItem("nearWorkPlaneNorm",PROPLIST_ITEM(id++,"��׼����Ʒ���","H�͸ֻ�׼����Ʒ���һ�����û�ָ����ϵͳ�ɸ��ݽ��Ʒ��߼�H�͸����߾�ȷ�����H�͸ֵĻ�׼�淨�ߡ�"));
	CLDSLineHShape::AddPropItem("nearWorkPlaneNorm.x",PROPLIST_ITEM(id++,"X�������","�۸ֻ�׼�淨��X�᷽�������"));
	CLDSLineHShape::AddPropItem("nearWorkPlaneNorm.y",PROPLIST_ITEM(id++,"Y�������","�۸ֻ�׼�淨��Y�᷽�������"));
	CLDSLineHShape::AddPropItem("nearWorkPlaneNorm.z",PROPLIST_ITEM(id++,"Z�������","�۸ֻ�׼�淨��Z�᷽�������"));
	CLDSLineHShape::AddPropItem("hWorkNormRefPart",PROPLIST_ITEM(id++,"���߲��ո˼�","����۸ֻ�׼�淨��ʱ�Ĺ�������ո˼���0X0ʱĬ��Ϊʼ�˻��ն˽ڵ㸸�˼�(����Ϊ�˼�����)"));
	//ʼ�ն�λ��Ϣ
	CLDSLineHShape::AddPropItem("startInfo",PROPLIST_ITEM(id++,"ʼ����Ϣ","H�͸�ʼ����Ϣ"));
	CLDSLineHShape::AddPropItem("hStart",PROPLIST_ITEM(id++,"�����","H�͸������"));
	CLDSLineHShape::AddPropItem("oddStart",PROPLIST_ITEM(id++,"����ͷ","H�͸�ʼ������ͷֵ"));
	CLDSLineHShape::AddPropItem("startPos",PROPLIST_ITEM(id++,"ʼ���������","H�͸�ʼ���������"));
	CLDSLineHShape::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X����","H�͸�ʼ��X�᷽�������"));
	CLDSLineHShape::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y����","H�͸�ʼ��Y�᷽�������"));
	CLDSLineHShape::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z����","H�͸�ʼ��Z�᷽�������"));
	CLDSLineHShape::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"ʼ��ʵ������","H�͸�ʼ�˿�������ͷ�������λ��"));
	CLDSLineHShape::AddPropItem("desStartPos",PROPLIST_ITEM(id++,""));
	CLDSLineHShape::AddPropItem("desStartPos.endPosType",PROPLIST_ITEM(id++,"��λ��ʽ","H�͸�ʼ�˻���λ��","�ڵ㼴����|�������|����ͶӰ|���߽���|ָ������|"));
	CLDSLineHShape::AddPropItem("desStartPos.datum_pos_style",PROPLIST_ITEM(id++,"���㶨λ��ʽ","ʼ�˻��㶨λ��ʽ","����ͶӰ|X֫����ͶӰ|Y֫����ͶӰ|������ƽ��|X֫����ƽ��|Y֫����ƽ��"));
	CLDSLineHShape::AddPropItem("desStartPos.hDatumPart",PROPLIST_ITEM(id++,"���λ�û�׼�˼�","���λ�û�׼�˼�"));
	CLDSLineHShape::AddPropItem("desStartPos.fNormOffset",PROPLIST_ITEM(id++,"��׼�淨��ƫ����","��׼�淨��ƫ����"));
	CLDSLineHShape::AddPropItem("desStartPos.fEccentricDist",PROPLIST_ITEM(id++,"ƫ�ľ�","��׼ƫ�ľ�"));
	CLDSLineHShape::AddPropItem("desStartPos.face_offset_norm",PROPLIST_ITEM(id++,"ƽ���淨��","ƽ���淨��"));
	CLDSLineHShape::AddPropItem("desStartPos.fTransverseOffset",PROPLIST_ITEM(id++,"����ƫ����","X�Ḻ����ƫ����"));
	CLDSLineHShape::AddPropItem("desStartPos.hDatumStartPart",PROPLIST_ITEM(id++,"ͶӰ��ʼ�����ڻ�׼�˼�"));
	CLDSLineHShape::AddPropItem("desStartPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,"��׼��ֵĻ�׼�˵�","��׼��ֵĻ�׼�˵�","ʼ��|�ն�"));
	CLDSLineHShape::AddPropItem("desStartPos.hDatumEndPart",PROPLIST_ITEM(id++,"ͶӰ���ն����ڻ�׼�˼�"));
	CLDSLineHShape::AddPropItem("desStartPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"��׼�˼��Ļ�׼�˵�","��׼�˼��Ļ�׼�˵�","ʼ��|�ն�"));
	//�ն˶�λ��Ϣ
	CLDSLineHShape::AddPropItem("endInfo",PROPLIST_ITEM(id++,"�ն���Ϣ","H�͸��ն���Ϣ"));
	CLDSLineHShape::AddPropItem("hEnd",PROPLIST_ITEM(id++,"�յ���","H�͸��յ���"));
	CLDSLineHShape::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"����ͷ","H�͸��ն�����ͷֵ"));
	CLDSLineHShape::AddPropItem("endPos",PROPLIST_ITEM(id++,"�ն��������","H�͸��ն��������"));
	CLDSLineHShape::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"�ն�ʵ������","H�͸��ն˿�������ͷ�������λ��"));
	CLDSLineHShape::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X����","H�͸��ն�����X�᷽�������ֵ"));
	CLDSLineHShape::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y����","H�͸��ն�����Y�᷽�������ֵ"));
	CLDSLineHShape::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z����","H�͸��ն�����Z�᷽�������ֵ"));
	CLDSLineHShape::AddPropItem("desEndPos",PROPLIST_ITEM(id++,""));
	CLDSLineHShape::AddPropItem("desEndPos.endPosType",PROPLIST_ITEM(id++,"��λ��ʽ","H�͸��ն˻���λ��","�ڵ㼴����|�������|����ͶӰ|���߽���|ָ������|"));
	CLDSLineHShape::AddPropItem("desEndPos.datum_pos_style",PROPLIST_ITEM(id++,"���㶨λ��ʽ","�ն˻��㶨λ��ʽ","����ͶӰ|X֫����ͶӰ|Y֫����ͶӰ|������ƽ��|X֫����ƽ��|Y֫����ƽ��"));
	CLDSLineHShape::AddPropItem("desEndPos.hDatumPart",PROPLIST_ITEM(id++,"���λ�û�׼�˼�","���λ�û�׼�˼�"));
	CLDSLineHShape::AddPropItem("desEndPos.fNormOffset",PROPLIST_ITEM(id++,"��׼�淨��ƫ����","��׼�淨��ƫ����"));
	CLDSLineHShape::AddPropItem("desEndPos.fEccentricDist",PROPLIST_ITEM(id++,"ƫ�ľ�","��׼ƫ�ľ�"));
	CLDSLineHShape::AddPropItem("desEndPos.face_offset_norm",PROPLIST_ITEM(id++,"ƽ���淨��","ƽ���淨��"));
	CLDSLineHShape::AddPropItem("desEndPos.fTransverseOffset",PROPLIST_ITEM(id++,"����ƫ����","X�Ḻ����ƫ����"));
	CLDSLineHShape::AddPropItem("desEndPos.hDatumStartPart",PROPLIST_ITEM(id++,"ͶӰ��ʼ�����ڻ�׼�˼�"));
	CLDSLineHShape::AddPropItem("desEndPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,"��׼��ֵĻ�׼�˵�","��׼��ֵĻ�׼�˵�","ʼ��|�ն�"));
	CLDSLineHShape::AddPropItem("desEndPos.hDatumEndPart",PROPLIST_ITEM(id++,"ͶӰ���ն����ڻ�׼�˼�"));
	CLDSLineHShape::AddPropItem("desEndPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"��׼�˼��Ļ�׼�˵�","��׼�˼��Ļ�׼�˵�","ʼ��|�ն�"));
#endif
}
long CLDSLineHShape::GetPropID(char* propStr)
{
	PROPLIST_ITEM *pItem = CLDSLineHShape::propHashtable.GetValue(propStr);
	if(pItem!=NULL)
		return pItem->id;
	else
		return 0;
}
PROPLIST_ITEM* CLDSLineHShape::GetPropItem(long id)
{
	return hashPropItemById.GetValue(id);
}
BOOL CLDSLineHShape::IsPropEqual(OBJSET &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSObject *pFirstObj = selectObjs.GetFirst();
	CLDSLineHShape *pFirstLineHShape = (CLDSLineHShape*)pFirstObj;
	pFirstLineHShape->GetPropValueStr(idProp,valueStr);
	for(CLDSLineHShape *pLineHShape=(CLDSLineHShape*)selectObjs.GetNext();pLineHShape;pLineHShape=(CLDSLineHShape*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pLineHShape->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}
int CLDSLineHShape::GetPropStatus(long id)
{
	DWORD dwStatus;
	if(CLDSLineHShape::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//������
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//����
	}
	return -1;	//δ�ҵ������״̬
}
BOOL CLDSLineHShape::SetPropStatus(long id,BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSLineHShape::propStatusHashtable.SetValueAt(id,dwStatus);
}
BOOL CLDSLineHShape::GetObjPropReadOnlyDefaultState(long id)
{
	return GetObjPropReadOnlyDefaultState(id,ShadowMode());
}
BOOL CLDSLineHShape::GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode)
{
	if(shadow_mode==CLDSObject::SHADOWMODE_COMMON)
	{
		if(GetPropID("layer")==id)
			return FALSE;
		else if(GetPropID("cMaterial")==id)
			return FALSE;
		else if(GetPropID("m_cQualityLevel")==id)
			return FALSE;
		else if(GetPropID("m_hPartWeldParent")==id)
			return FALSE;
		else if(GetPropID("specification")==id)
			return FALSE;
		else if(GetPropID("sPartNo")==id)
			return FALSE;
		else if(GetPropID("cfgword")==id)
			return FALSE;
		else if(GetPropID("m_uStatMatCoef")==id)
			return FALSE;
		else if(GetPropID("sNotes")==id)
			return FALSE;
		else if(GetPropID("relativeObjs")==id)
			return FALSE;
		else
			return TRUE;
	}
	else if(shadow_mode==CLDSObject::SHADOWMODE_BLOCK)
	{
		if(GetPropID("layer")==id)
			return FALSE;
		else if(GetPropID("m_hPartWeldParent")==id)
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		if(GetPropID("handle")==id)
			return TRUE;
		else if(GetPropID("length")==id)
			return TRUE;
		else if(GetPropID("thick1")==id)
			return TRUE;
		else if(GetPropID("thick2")==id)
			return TRUE;
		else if(GetPropID("workPlaneNorm.x")==id)
			return TRUE;
		else if(GetPropID("workPlaneNorm.y")==id)
			return TRUE;
		else if(GetPropID("workPlaneNorm.z")==id)
			return TRUE;
		else
			return FALSE;
	}
}
int CLDSLineHShape::GetPropValueStr(long id,char* valueStr)
{
	char sText[100]="";
	if(GetPropID("basicInfo")==0)
		strcpy(sText,"");
	else if(GetPropID("handle")==id)
		sprintf(sText,"0X%X",handle);
	else if(GetPropID("layer")==id)
		strcpy(sText,layer());
	else if(GetPropID("cMaterial")==id)	
	{
		char matStr[20];
		if(QuerySteelMatMark(cMaterial,matStr))
			sprintf(sText,"%s",matStr);
	}
	else if(GetPropID("m_cQualityLevel")==id)
		sprintf(sText,"%C",m_cQualityLevel);
	else if(GetPropID("thick1")==id)
	{
		sprintf(sText,"%f",size_thick);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("thick2")==id)
	{
		sprintf(sText,"%f",size_thick2);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ibHShapeType")==id)
	{
		if(m_ibHShapeType==0)
			strcpy(sText,"0.HW");
		else if(m_ibHShapeType==1)
			strcpy(sText,"1.HM");
		else if(m_ibHShapeType==2)
			strcpy(sText,"2.HN");
		else if(m_ibHShapeType==3)
			strcpy(sText,"3.HP");
	}
	else if(GetPropID("specification")==id)
	{
		char s_w[100],s_h[100];
		sprintf(s_w,"%f",size_wide);
		SimplifiedNumString(s_w);
		sprintf(s_h,"%f",size_height);
		SimplifiedNumString(s_h);
		sprintf(sText,"%sX%s",s_h,s_w); 
	}
	else if(GetPropID("iSeg")==id)
		strcpy(sText,(char*)iSeg.ToString());
	else if(GetPropID("sPartNo")==id)
		sprintf(sText,"%s",(char*)GetPartNo());
	else if(GetPropID("cfgword")==id)
	{
		if(layer(0)=='L')	//���ȹ���
			return cfgword.GetLegScopeStr(valueStr,50,true);
		else
			return cfgword.GetBodyScopeStr(valueStr);
	}
	else if(GetPropID("relativeObjs")==id)
	{
		for(RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
		{
			if(strlen(sText)==0)
				_snprintf(sText,99,"0X%X",pRelaObj->hObj);
			else
				_snprintf(sText,99,"%s,0X%X",sText,pRelaObj->hObj);
		}
	}
	else if(GetPropID("shadowInfo")==0)
		strcpy(sText,"");
	else if(GetPropID("m_hMotherObj")==id)
	{
		CLDSLineHShape *pMotherLineHShape=(CLDSLineHShape*)MotherObject(false);
		if(pMotherLineHShape!=this)
			sprintf(sText,"0X%X",pMotherLineHShape->handle);
		else 
			strcpy(sText,"0X0");
	}
	else if(GetPropID("m_hBlockRef")==id)
	{
		CBlockParameter blockPara;
		if (GetBlockParameter(blockPara))
			sprintf(sText,"0X%X",blockPara.m_pBlockRef->handle);
		else 
			strcpy(sText,"0X0");
	}
	else if(GetPropID("m_hBlock")==id)
	{
		CBlockParameter blockPara;
		if (GetBlockParameter(blockPara))
			sprintf(sText,"0X%X",blockPara.m_pBlock->handle);
		else 
			strcpy(sText,"0X0");
	}
	else if(GetPropID("m_uStatMatCoef")==id)
		sprintf(sText,"%d",m_uStatMatCoef);
	else if(GetPropID("m_hPatWeldParent")==id)
		sprintf(sText,"0X%X",m_hPartWeldParent);
	else if(GetPropID("m_bDisplayHole")==id)
	{
		if(m_bDisplayHole)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("m_bVirtualPart")==id)
	{
		if(m_bVirtualPart)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("width")==id)
	{
		sprintf(sText,"%f",size_wide);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("thick")==id)
	{
		sprintf(sText,"%f",size_thick);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("length")==id)
	{
		sprintf(sText,"%f",GetLength());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_bUserSpecColor")==id)
	{
		if(m_bUserSpecColor)
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sText,"Yes");
		else
			sprintf(sText,"No");
#else
			sprintf(sText,"��");
		else
			sprintf(sText,"��");
#endif
	}
	else if(GetPropID("crMaterial")==id)
		sprintf(sText,"RGB%X",crMaterial);
	else if(GetPropID("sNotes")==id)
		strcpy(sText,sNotes);
	else if(GetPropID("hWorkNormRefPart")==id)
		sprintf(sText,"0X%X",hWorkNormRefPart);
	else if(GetPropID("m_iNormCalStyle")==id)
	{
		if(m_iNormCalStyle == 0)
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sText,"0.Specify");
		else
			sprintf(sText,"1.Reference Rod");
#else
			sprintf(sText,"0.ֱ��ָ��");
		else
			sprintf(sText,"1.���ո˼�");
#endif
	}
	else if(GetPropID("workPlaneNorm.x")==id)
	{
		sprintf(sText,"%f",workPlaneNorm.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("workPlaneNorm.y")==id)
	{
		sprintf(sText,"%f",workPlaneNorm.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("workPlaneNorm.z")==id)
	{
		sprintf(sText,"%f",workPlaneNorm.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("nearWorkPlaneNorm")==id)
	{
		sprintf(sText,"%f",nearWorkPlaneNorm.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("nearWorkPlaneNorm.x")==id)
	{
		sprintf(sText,"%f",nearWorkPlaneNorm.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("nearWorkPlaneNorm.y")==id)
	{
		sprintf(sText,"%f",nearWorkPlaneNorm.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("nearWorkPlaneNorm.z")==id)
	{
		sprintf(sText,"%f",nearWorkPlaneNorm.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("hStart")==id && pStart)
		sprintf(sText,"0X%X",pStart->handle);
	else if(GetPropID("oddStart")==id)
	{
		sprintf(sText,"%f",startOdd());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("startPos.x")==id)
	{
		sprintf(sText,"%f",Start().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("startPos.y")==id)
	{
		sprintf(sText,"%f",Start().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("startPos.z")==id)
	{
		sprintf(sText,"%f",Start().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.endPosType")==id)
	{
		if(desStartPos.endPosType==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Node Is Datum Node");
		else if(desStartPos.endPosType==1)
			strcpy(sText,"Lap Joint");
		else if(desStartPos.endPosType==2)
			strcpy(sText,"Point And Line Project");
		else if(desStartPos.endPosType==3)
			strcpy(sText,"Bolt Line Intersection");
		else //if(desStartPos.endPosType==4)
			strcpy(sText,"Specify");
#else
			strcpy(sText,"�ڵ㼴����");
		else if(desStartPos.endPosType==1)
			strcpy(sText,"�������");
		else if(desStartPos.endPosType==2)
			strcpy(sText,"����ͶӰ");
		else if(desStartPos.endPosType==3)
			strcpy(sText,"���߽���");
		else //if(desStartPos.endPosType==4)
			strcpy(sText,"ָ������");
#endif
	}
	else if(GetPropID("desStartPos.datum_pos_style")==id)
	{
		if(desStartPos.datum_pos_style==1)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"X Leg Project");
		else if(desStartPos.datum_pos_style==2)
			strcpy(sText,"Y Leg Project");
		else if(desStartPos.datum_pos_style==3)
			strcpy(sText,"Ridge Plane Offset");
		else if(desStartPos.datum_pos_style==4)
			strcpy(sText,"X Leg Plane Offset");
		else if(desStartPos.datum_pos_style==5)
			strcpy(sText,"Y Leg Plane Offset");
		else //if(desStartPos.datum_pos_style==0)
			strcpy(sText,"Ridge Project");
#else
			strcpy(sText,"X֫����ͶӰ");
		else if(desStartPos.datum_pos_style==2)
			strcpy(sText,"Y֫����ͶӰ");
		else if(desStartPos.datum_pos_style==3)
			strcpy(sText,"������ƽ��");
		else if(desStartPos.datum_pos_style==4)
			strcpy(sText,"X֫����ƽ��");
		else if(desStartPos.datum_pos_style==5)
			strcpy(sText,"Y֫����ƽ��");
		else //if(desStartPos.datum_pos_style==0)
			strcpy(sText,"������ͶӰ");
#endif
	}
	else if(GetPropID("desStartPos.hDatumPart")==id)
		sprintf(sText,"0X%X", desStartPos.hDatumPart);
	else if(GetPropID("desStartPos.fNormOffset")==id)
	{
		sprintf(sText,"%f",desStartPos.fNormOffset);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.fEccentricDist")==id)
	{
		sprintf(sText,"%f",desStartPos.fEccentricDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.fTransverseOffset")==id)
	{
		sprintf(sText,"%f",desStartPos.fTransverseOffset);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.hDatumStartPart")==id)
		sprintf(sText,"0X%X", desStartPos.hDatumStartPart); 
	else if(GetPropID("desStartPos.bDatumStartStartEnd")==id)
	{
		if(desStartPos.bDatumStartStartEnd)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Start");
		else //if(!desStartPos.bDatumStartStartEnd)
			strcpy(sText,"End");
#else
			strcpy(sText,"ʼ��");
		else //if(!desStartPos.bDatumStartStartEnd)
			strcpy(sText,"�ն�");
#endif
	}
	else if(GetPropID("desStartPos.hDatumEndPart")==id)
		sprintf(sText,"0X%X", desStartPos.hDatumEndPart); 
	else if(GetPropID("desStartPos.bDatumEndStartEnd")==id)
	{
		if(desStartPos.bDatumEndStartEnd)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Start");
		else //if(!desStartPos.bDatumEndStartEnd)
			strcpy(sText,"End");
#else
			strcpy(sText,"ʼ��");
		else //if(!desStartPos.bDatumEndStartEnd)
			strcpy(sText,"�ն�");
#endif
	}
	else if(GetPropID("hEnd")==id && pEnd)
		sprintf(sText,"0X%X",pEnd->handle);
	else if(GetPropID("oddEnd")==id)
	{
		sprintf(sText,"%f",endOdd());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("endPos.x")==id)
	{
		sprintf(sText,"%f",End().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("endPos.y")==id)
	{
		sprintf(sText,"%f",End().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("endPos.z")==id)
	{
		sprintf(sText,"%f",End().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.endPosType")==id)
	{
		if(desEndPos.endPosType==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Node Is Datum Node");
		else if(desEndPos.endPosType==1)
			strcpy(sText,"Lap Joint");
		else if(desEndPos.endPosType==2)
			strcpy(sText,"Point And Line Project");
		else if(desEndPos.endPosType==3)
			strcpy(sText,"Bolt Line Intersection");
		else //if(desEndPos.endPosType==4)
			strcpy(sText,"Specify");
#else
			strcpy(sText,"�ڵ㼴����");
		else if(desEndPos.endPosType==1)
			strcpy(sText,"�������");
		else if(desEndPos.endPosType==2)
			strcpy(sText,"����ͶӰ");
		else if(desEndPos.endPosType==3)
			strcpy(sText,"���߽���");
		else //if(desEndPos.endPosType==4)
			strcpy(sText,"ָ������");
#endif
	}
	else if(GetPropID("desEndPos.datum_pos_style")==id)
	{
		if(desEndPos.datum_pos_style==1)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"X Leg Project");
		else if(desEndPos.datum_pos_style==2)
			strcpy(sText,"Y Leg Project");
		else if(desEndPos.datum_pos_style==3)
			strcpy(sText,"Ridge Plane Offset");
		else if(desEndPos.datum_pos_style==4)
			strcpy(sText,"X Leg Plane Offset");
		else if(desEndPos.datum_pos_style==5)
			strcpy(sText,"Y Leg Plane Offset");
		else //if(desStartPos.datum_pos_style==0)
			strcpy(sText,"Ridge Project");
#else
			strcpy(sText,"X֫����ͶӰ");
		else if(desEndPos.datum_pos_style==2)
			strcpy(sText,"Y֫����ͶӰ");
		else if(desEndPos.datum_pos_style==3)
			strcpy(sText,"������ƽ��");
		else if(desEndPos.datum_pos_style==4)
			strcpy(sText,"X֫����ƽ��");
		else if(desEndPos.datum_pos_style==5)
			strcpy(sText,"Y֫����ƽ��");
		else //if(desStartPos.datum_pos_style==0)
			strcpy(sText,"������ͶӰ");
#endif
	}
	else if(GetPropID("desEndPos.hDatumPart")==id)
		sprintf(sText,"0X%X", desEndPos.hDatumPart);
	else if(GetPropID("desEndPos.fNormOffset")==id)
	{
		sprintf(sText,"%f",desEndPos.fNormOffset);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.fEccentricDist")==id)
	{
		sprintf(sText,"%f",desEndPos.fEccentricDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.fTransverseOffset")==id)
	{
		sprintf(sText,"%f",desEndPos.fTransverseOffset);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.hDatumStartPart")==id)
		sprintf(sText,"0X%X", desEndPos.hDatumStartPart); 
	else if(GetPropID("desEndPos.bDatumStartStartEnd")==id)
	{
		if(desEndPos.bDatumStartStartEnd)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Start");
		else //if(!desEndPos.bDatumStartStartEnd)
			strcpy(sText,"End");
#else
			strcpy(sText,"ʼ��");
		else //if(!desEndPos.bDatumStartStartEnd)
			strcpy(sText,"�ն�");
#endif
	}
	else if(GetPropID("desEndPos.hDatumEndPart")==id)
		sprintf(sText,"0X%X", desEndPos.hDatumEndPart); 
	else if(GetPropID("desEndPos.bDatumEndStartEnd")==id)
	{
		if(desEndPos.bDatumEndStartEnd)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Start");
		else //if(!desEndPos.bDatumEndStartEnd)
			strcpy(sText,"End");
#else
			strcpy(sText,"ʼ��");
		else //if(!desEndPos.bDatumEndStartEnd)
			strcpy(sText,"�ն�");
#endif
	}
#if defined(__TSA_)||defined(__LDS_)
	else if(GetPropID("size.idClassType")==id)
	{
		if(size_idClassType==CLS_LINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText, "Same Leg Angle");
#else
			strcpy(sText, "��֫�Ǹ�");
#endif
		else if(size_idClassType==CLS_GROUPLINEANGLE)
		{
			if(size_cSubClassType=='T')
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText, "T Group");
			else if(size_cSubClassType=='D')
				strcpy(sText, "Diagonal Group");
			else if(size_cSubClassType=='X')
				strcpy(sText, "Cross Group");
#else
				strcpy(sText, "T�����");
			else if(size_cSubClassType=='D')
				strcpy(sText, "�Խ����");
			else if(size_cSubClassType=='X')
				strcpy(sText, "ʮ�����");
#endif
		}
		else if(size_idClassType==CLS_LINETUBE)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText, "Tube");
		else if(size_idClassType==CLS_LINEFLAT)
			strcpy(sText, "Flat");
		else if(size_idClassType==CLS_LINESLOT)
			strcpy(sText, "Slot");
		else if(size_idClassType==0)
			strcpy(sText, "User-Defined");
#else
			strcpy(sText, "�ֹ�");
		else if(size_idClassType==CLS_LINEFLAT)
			strcpy(sText, "���");
		else if(size_idClassType==CLS_LINESLOT)
			strcpy(sText, "�۸�");
		else if(size_idClassType==0)
			strcpy(sText, "�û�����");
#endif
	}
	else if(GetPropID("m_uStatMatNo")==id)
		sprintf(sText,"%d",m_uStatMatNo);
	else if(GetPropID("m_fWeightRaiseCoef")==id)
	{
		sprintf(sText,"%f",m_fWeightRaiseCoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_iElemType")==id)	//�Զ��жϵ�Ԫ����
	{
		if(m_iElemType==0)
			strcpy(sText,"0.�Զ��ж�");
		else if(m_iElemType==2)
			strcpy(sText,"2.������");
		else if(m_iElemType==3)
			strcpy(sText,"3.����");
		else
			strcpy(sText,"1.������");
	}
	else if(GetPropID("m_bNeedSelMat")==id)
	{
		if(m_bNeedSelMat)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("m_bNeedSelSize")==id)
	{
		if(m_bNeedSelSize)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("CalLenCoef.iTypeNo")==id)
	{
		if(CalLenCoef.iTypeNo==0)
			strcpy(sText,"0.�����ж�");
		else if(CalLenCoef.iTypeNo==1)
			strcpy(sText,"1.Vб��(��Ų�����)");
		else if(CalLenCoef.iTypeNo==2)
			strcpy(sText,"2.Vб��(V������)");
		else if(CalLenCoef.iTypeNo==3)
			strcpy(sText,"3.Vб��3�ڼ��������");
		else if(CalLenCoef.iTypeNo==4)
			strcpy(sText,"4.Vб��4�ڼ��������");
		else if(CalLenCoef.iTypeNo==5)
			strcpy(sText,"5.ָ���˼����㳤��");
	}
	else if(GetPropID("CalLenCoef.pHorizPole")==id)
	{
		if(CalLenCoef.hRefPole>0x20)
			sprintf(sText,"0X%X",CalLenCoef.hRefPole);
		else
			strcpy(sText,"0X0");
	}
	else if(GetPropID("CalLenCoef.pRefPole")==id)
	{
		if(CalLenCoef.hRefPole>0x20)
			sprintf(sText,"0X%X",CalLenCoef.hRefPole);
		else
			strcpy(sText,"0X0");
	}
	else if(GetPropID("CalLenCoef.spec_callenMinR")==id)
	{
		sprintf(sText,"%f",CalSpecLamdaLenMinR());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("CalLenCoef.spec_callenParaR")==id)
	{
		sprintf(sText,"%f",CalSpecLamdaLenParaR());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("start_joint_type")==id)
	{
		if(start_joint_type==JOINT_HINGE)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Hinge Joint");
		else if(start_joint_type==JOINT_RIGID)
			strcpy(sText,"Rigid Joint");
		else //if(start_joint_type==JOINT_AUTOJUSTIFY)
			strcpy(sText,"Auto Justify");
#else
			strcpy(sText,"�½�");
		else if(start_joint_type==JOINT_RIGID)
			strcpy(sText,"�ս�");
		else //if(start_joint_type==JOINT_AUTOJUSTIFY)
			strcpy(sText,"�Զ��ж�");
#endif
	}
	else if(GetPropID("start_force_type")==id)
	{
		if(start_force_type==CENTRIC_FORCE)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Centric Force");
		else //if(start_force_type==ECCENTRIC_FORCE)
			strcpy(sText,"Eccentric Force");
#else
			strcpy(sText,"��������");
		else //if(start_force_type==ECCENTRIC_FORCE)
			strcpy(sText,"ƫ������");
#endif
	}
	else if(GetPropID("connectStart.grade")==id)
		strcpy(sText,connectStart.grade);
	else if(GetPropID("connectStart.d")==id)
		sprintf(sText,"M%d",connectStart.d);
	else if(GetPropID("connectStart.N")==id)
		sprintf(sText,"%d",connectStart.N);
	else if(GetPropID("connectStart.bearloadcoef")==id)
	{
		sprintf(sText,"%f",connectStart.bearloadcoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("end_joint_type")==id)
	{
		if(end_joint_type==JOINT_HINGE)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Hinge Joint");
		else if(end_joint_type==JOINT_RIGID)
			strcpy(sText,"Rigid Joint");
		else //if(end_joint_type==JOINT_AUTOJUSTIFY)
			strcpy(sText,"Atuo Justify");
#else
			strcpy(sText,"�½�");
		else if(end_joint_type==JOINT_RIGID)
			strcpy(sText,"�ս�");
		else //if(end_joint_type==JOINT_AUTOJUSTIFY)
			strcpy(sText,"�Զ��ж�");
#endif
	}
	else if(GetPropID("end_force_type")==id)
	{
		if(end_force_type==CENTRIC_FORCE)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Centric Force");
		else //if(end_force_type==ECCENTRIC_FORCE)
			strcpy(sText,"Eccentric Force");
#else
			strcpy(sText,"��������");
		else //if(end_force_type==ECCENTRIC_FORCE)
			strcpy(sText,"ƫ������");
#endif
	}
	else if(GetPropID("connectEnd.grade")==id)
		strcpy(sText,connectEnd.grade);
	else if(GetPropID("connectEnd.d")==id)
		sprintf(sText,"M%d",connectEnd.d);
	else if(GetPropID("connectEnd.N")==id)
		sprintf(sText,"%d",connectEnd.N);
	else if(GetPropID("connectEnd.bearloadcoef")==id)
	{
		sprintf(sText,"%f",connectEnd.bearloadcoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_bAutoCalHoleOutCoef")==id)
	{
		if(m_bAutoCalHoleOutCoef)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("hole_out")==id)
	{
		sprintf(sText,"%f",hole_out);
		SimplifiedNumString(sText);
	}
#endif
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
#endif