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
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
	hWorkNormRefPart = 0;
	m_iNormCalStyle = 1;				//默认为杆件定位
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
		//buffer.ReadString(spec);	//规格
		CfgwordFromBuffer(cfgword,buffer,version,doc_type);
		BelongModel()->EvolveCfgword(cfgword,version,doc_type);
		//buffer.ReadString(sPartNo);	//构件编号
		buffer.ReadString(layer(),4);	//扁钢所在图层.
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
		//关联构件列表
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
			if(pMirObj->mirInfo.axis_flag&0x10)	//镜像
				buffer.ReadPoint(pMirObj->mirInfo.mir_norm);
		}
	}
}
void CLDSLineHShape::IndependentShadowPropToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(pStart)
		buffer.WriteInteger(pStart->handle);	// 起点句柄
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
	//buffer.WriteString(spec);	//规格
	CfgwordToBuffer(cfgword,buffer,version,doc_type);
	//buffer.WriteString(sPartNo);	//构件编号
	buffer.WriteString(layer());	//扁钢所在图层.
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
	//关联构件列表
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
		if(pMirObj->mirInfo.axis_flag&0x10)	//镜像
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
	bool bPrevDeflateVersion=false;	//文件瘦身之前的版本
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
		m_bVirtualPart=(dw!=0);//隐含构件
	}
	else
		buffer.ReadBooleanThin(&_bVirtualPart);
	buffer.ReadInteger(&_iSeg.iSeg);
	buffer.ReadString(sPartNo);			//构件编号
	buffer.ReadString(layer(),4);			//扁钢所在图层.
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
		buffer.ReadInteger(&dw);//显示螺栓孔及切割面
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
	pStart=BelongModel()->FromNodeHandle(dw);	// 起点句柄
	buffer.ReadInteger(&dw);
	pEnd=BelongModel()->FromNodeHandle(dw);	// 终点句柄

	if( (doc_type==1&&(version==0||version>=4000028)) ||	//TMA
		(doc_type==2&&(version==0||version>=1000083))||		//LMA
		(doc_type==4&&(version==0||version>=1000010))||		//LDS
		doc_type==5)	//TDA
		buffer.ReadWord(&m_iNormCalStyle);
	buffer.ReadDouble(&nearWorkPlaneNorm.x);
	buffer.ReadDouble(&nearWorkPlaneNorm.y);
	buffer.ReadDouble(&nearWorkPlaneNorm.z);
	buffer.ReadInteger(&hWorkNormRefPart);
	buffer.ReadDouble(&workPlaneNorm.x);	// 主工作平面法线方向.
	buffer.ReadDouble(&workPlaneNorm.y);	// 主工作平面法线方向.
	buffer.ReadDouble(&workPlaneNorm.z);	// 主工作平面法线方向.
	if((doc_type==1&&version>0&&version<4000021)	//TMA
		||(doc_type==2&&version>0&&version<1000076))//LMA
		workPlaneNorm*=-1.0;	//纠正旧版本中脚钉板法线方向错误的问题
	f3dPoint start,end;
	buffer.ReadDouble(&start.x);// 实际楞线起点
	buffer.ReadDouble(&start.y);// 实际楞线起点
	buffer.ReadDouble(&start.z);// 实际楞线起点
	buffer.ReadDouble(&end.x);	// 实际楞线终点
	buffer.ReadDouble(&end.y);	// 实际楞线终点
	buffer.ReadDouble(&end.z);	// 实际楞线终点
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
	//始端正头计算参数
	buffer.ReadDouble(&start_oddment);
	buffer.ReadInteger(&desStartOdd.m_iOddCalStyle);
	buffer.ReadInteger(&desStartOdd.m_hRefPart1);
	buffer.ReadInteger(&desStartOdd.m_hRefPart2);
	buffer.ReadDouble(&desStartOdd.m_fCollideDist);
	//终端正头计算参数
	buffer.ReadDouble(&end_oddment);
	buffer.ReadInteger(&desEndOdd.m_iOddCalStyle);
	buffer.ReadInteger(&desEndOdd.m_hRefPart1);
	buffer.ReadInteger(&desEndOdd.m_hRefPart2);
	buffer.ReadDouble(&desEndOdd.m_fCollideDist);
	//始端定位参数
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
	{	//平推面法线 10000084
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
	//终端定位参数
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
	{	//平推面法线 10000084
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
	//关联构件列表
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
	//H型钢螺栓区
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
	buffer.WriteString(spec);	//规格
	CfgwordToBuffer(cfgword,buffer,version,doc_type);
	buffer.WriteByte(cMaterial);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.WriteByte(m_cQualityLevel);
	buffer.WriteBooleanThin(_bVirtualPart);//隐含构件
	buffer.WriteInteger(iSeg);
	buffer.WriteString(sPartNo);	//构件编号
	buffer.WriteString(layer());	//扁钢所在图层.
	buffer.WriteInteger(m_uStatMatCoef);
	buffer.WriteInteger(m_hPartWeldParent);
	if(	version==0||
		(doc_type==1&&version>=5000000)||	//TMA V5.0.0.0
		(doc_type==2&&version>=5000000)||	//LMA V2.0.0.0
		(doc_type==3&&version>=2010000)||	//TSA V2.1.0.0
		(doc_type==4&&version>=1030000)||	//LDS V1.3.0.0
		(doc_type==5&&version>=1030000))	//TDA V1.3.0.0
		buffer.WriteByte(m_cFuncType);
	buffer.WriteBooleanThin(m_bDisplayHole);//显示螺栓孔及切割面
	buffer.WriteInteger(m_bLockStartPos);
	buffer.WriteInteger(m_bLockEndPos);
	buffer.WriteString(sNotes);
	buffer.WriteDouble(size_wide);	
	buffer.WriteDouble(size_height);
	buffer.WriteDouble(size.thick);
	buffer.WriteDouble(size.thick2);
	buffer.WriteBooleanThin(m_bUserSpecColor);	//用户指定构件颜色
	if(m_bUserSpecColor)
		buffer.WriteDword(crMaterial);		//构件颜色
	if(pStart)
		buffer.WriteInteger(pStart->handle);	// 起点句柄
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
	buffer.WriteDouble(workPlaneNorm.x);	// 主工作平面法线方向.
	buffer.WriteDouble(workPlaneNorm.y);	// 主工作平面法线方向.
	buffer.WriteDouble(workPlaneNorm.z);	// 主工作平面法线方向.
	buffer.WriteDouble(baseline.Start().x);	// 实际楞线起点
	buffer.WriteDouble(baseline.Start().y);	// 实际楞线起点
	buffer.WriteDouble(baseline.Start().z);	// 实际楞线起点
	buffer.WriteDouble(baseline.End().x);	// 实际楞线终点
	buffer.WriteDouble(baseline.End().y);	// 实际楞线终点
	buffer.WriteDouble(baseline.End().z);	// 实际楞线终点
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
	//始端正头计算参数
	buffer.WriteDouble(start_oddment);
	buffer.WriteInteger(desStartOdd.m_iOddCalStyle);
	buffer.WriteInteger(desStartOdd.m_hRefPart1);
	buffer.WriteInteger(desStartOdd.m_hRefPart2);
	buffer.WriteDouble(desStartOdd.m_fCollideDist);
	//终端正头计算参数
	buffer.WriteDouble(end_oddment); 
	buffer.WriteInteger(desEndOdd.m_iOddCalStyle);
	buffer.WriteInteger(desEndOdd.m_hRefPart1);
	buffer.WriteInteger(desEndOdd.m_hRefPart2);
	buffer.WriteDouble(desEndOdd.m_fCollideDist);
	//始端定位参数
	buffer.WriteInteger(desStartPos.endPosType);
	buffer.WriteByte(desStartPos.datum_pos_style);	//1000085
	buffer.WriteInteger(desStartPos.hDatumPart);
	buffer.WriteInteger(desStartPos.hDatumStartPart);
	buffer.WriteInteger(desStartPos.hDatumEndPart);
	buffer.WriteInteger(desStartPos.bDatumEndStartEnd);
	buffer.WriteInteger(desStartPos.bDatumStartStartEnd);
	//平推面法线 10000084
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
	//终端定位参数
	buffer.WriteInteger(desEndPos.endPosType);
	buffer.WriteByte(desEndPos.datum_pos_style);	//1000085
	buffer.WriteInteger(desEndPos.hDatumPart);
	buffer.WriteInteger(desEndPos.hDatumStartPart);
	buffer.WriteInteger(desEndPos.hDatumEndPart);
	buffer.WriteInteger(desEndPos.bDatumEndStartEnd);
	buffer.WriteInteger(desEndPos.bDatumStartStartEnd);
	//平推面法线 10000084
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
	//关联构件列表
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
		CLDSObject::Log2File()->Log("关联构件记录数出现错误!");
#endif

	//H型钢螺栓区
	pLsRefList->ToBuffer(buffer,version,doc_type);
}
void CLDSLineHShape::ToXmlFile(FILE* fp,DWORD schema)
{
	char sText[500]="";
	fprintf(fp,"<H型钢信息 dwPermission=\"%d\" dwStartPermission=\"%d\" dwEndPermission=\"%d\">\n",dwPermission,dwStartPermission,dwEndPermission);
	fprintf(fp,"<规格 spec=\"%s\"/>\n",spec);
	fprintf(fp,"<配材号 cfgword=\"(%d,%d,%d,%d)\"/>\n",cfgword.flag.word[0],cfgword.flag.word[1],cfgword.flag.word[2],cfgword.flag.word[3]);
	fprintf(fp,"<材质 cMaterial=\"%c\"/>\n",cMaterial);
	if(m_bVirtualPart)
		strcpy(sText,"true");
	else
		strcpy(sText,"false");
	fprintf(fp,"<是否隐含构件 bVirtualPart=\"%s\"/>\n",sText);
	fprintf(fp,"<段号 iSeg=\"%d\"/>\n",iSeg);
	fprintf(fp,"<构件编号 sPartNo=\"%s\"/>\n",(char*)sPartNo);
	fprintf(fp,"<所在图层 layer=\"%s\"/>\n",layer());
	fprintf(fp,"<统材系数 StatMatCoef=\"%d\"/>\n",m_uStatMatCoef);
	fprintf(fp,"<焊接父构件 hPartWeldParent=\"0x%x\"/>\n",m_hPartWeldParent);
	if(m_bDisplayHole)
		strcpy(sText,"true");
	else 
		strcpy(sText,"false");
	fprintf(fp,"<是否显示螺栓孔 bDisplayHole=\"%s\"/>\n",sText);
	fprintf(fp,"<是否锁定起始位置 bLockStartPos=\"%d\"/>\n",m_bLockStartPos);
	fprintf(fp,"<是否锁定终止位置 bLockEndPos=\"%d\"/>\n",m_bLockEndPos);
	fprintf(fp,"<备注 sNotes=\"%s\"/>\n",sNotes);
	sprintf(sText,"%f",size_thick);
	SimplifiedNumString(sText);
	fprintf(fp,"<腹板厚度 thick1=\"%s\"/>\n",sText);
	sprintf(sText,"%f",size_thick2);
	SimplifiedNumString(sText);
	fprintf(fp,"<翼缘厚度 thick2=\"%s\"/>\n",sText);
	if(m_bUserSpecColor)
		strcpy(sText,"true");
	else
		strcpy(sText,"false");
	fprintf(fp,"<是否用户指定构件颜色 bUserSpecColor=\"%s\"/>\n",sText);
	if(m_bUserSpecColor)
		fprintf(fp,"<构件颜色 crMaterial=\"%d\"/>\n",crMaterial);
	if(pStart)
		fprintf(fp,"<起点句柄 hStart=\"0x%x\"/>\n",pStart->handle);
	else
		fprintf(fp,"<起点句柄 hStart=\"0x0\"/>\n");
	if(pEnd)
		fprintf(fp,"<终点句柄 hEnd=\"0x%x\"/>\n",pEnd->handle);
	else
		fprintf(fp,"<终点句柄 hEnd=\"0x0\"/>\n");
	fprintf(fp,"<基准面法线计算方式 NormCalStyle=\"%d\"/>\n",m_iNormCalStyle);
	fprintf(fp,"<工作平面法线方向 nearWorkPlaneNorm=\"(%.3f,%.3f,%.3f)\"/>\n",nearWorkPlaneNorm.x,nearWorkPlaneNorm.y,nearWorkPlaneNorm.z);
	fprintf(fp,"<参照杆件 hWorkNormRefPart=\"0x%x\"/>\n",hWorkNormRefPart);
	fprintf(fp,"<主工作平面法线方向 workPlaneNorm=\"(%.3f,%.3f,%.3f)\"/>\n",workPlaneNorm.x,workPlaneNorm.y,workPlaneNorm.z);
	fprintf(fp,"<实际楞线起点坐标 StartPos=\"(%.3f,%.3f,%.3f)\"/>\n",baseline.Start().x,baseline.Start().y,baseline.Start().z);
	fprintf(fp,"<实际楞线终点坐标 EndPos=\"(%.3f,%.3f,%.3f)\"/>\n",baseline.End().x,baseline.End().y,baseline.End().z);
	fprintf(fp,"<始端正头计算参数信息 oddment=\"%.2f\" OddCalStyle=\"%d\" hRefPart1=\"0x%x\" hRefPart2=\"0x%x\" CollideDist=\"%.2f\"/>\n",
		start_oddment,desStartOdd.m_iOddCalStyle,desStartOdd.m_hRefPart1,desStartOdd.m_hRefPart2,desStartOdd.m_fCollideDist);
	fprintf(fp,"<终端正头计算参数信息 oddment=\"%.2f\" OddCalStyle=\"%d\" hRefPart1=\"0x%x\" hRefPart2=\"0x%x\" CollideDist=\"%.2f\"/>\n",
		end_oddment,desEndOdd.m_iOddCalStyle,desEndOdd.m_hRefPart1,desEndOdd.m_hRefPart2,desEndOdd.m_fCollideDist);

	fprintf(fp,"<始端定位参数信息 endPosType=\"%d\" datum_pos_style=\"%d\" hDatumPart=\"0x%x\" hDatumStartPart=\"0x%x\" hDatumEndPart=\"0x%x\" bDatumEndStartEnd=\"%d\" bDatumStartStartEnd=\"%d\">\n",
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
	fprintf(fp,"<是否启用平推面法线 bUsefaceOffsetNorm=\"%s\"/>\n",sText);
	if(desStartPos.bUseFaceOffsetNorm)
		fprintf(fp,"<平推门法线方向 face_offset_norm=\"(%.3f,%.3f,%.3f)\"/>\n",desStartPos.face_offset_norm.x,desStartPos.face_offset_norm.y,desStartPos.face_offset_norm.z);
	sprintf(sText,"%f",desStartPos.fEccentricDist);
	SimplifiedNumString(sText);
	fprintf(fp,"<偏心距 EccentricDist=\"%s\"/>\n",sText);
	sprintf(sText,"%f",desStartPos.fNormOffset);
	SimplifiedNumString(sText);
	fprintf(fp,"<基准面法向偏移量 NormOffset=\"%s\"/>\n",sText);
	sprintf(sText,"%f",desStartPos.fTransverseOffset);
	SimplifiedNumString(sText);
	fprintf(fp,"<横向偏移量 TransverseOffset=\"%s\"/>\n",sText);
	fprintf(fp,"</始端定位参数信息>\n");

	fprintf(fp,"<终端定位参数信息 endPosType=\"%d\" datum_pos_style=\"%d\" hDatumPart=\"0x%x\" hDatumStartPart=\"0x%x\" hDatumEndPart=\"0x%x\" bDatumEndStartEnd=\"%d\" bDatumStartStartEnd=\"%d\">\n",
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
	fprintf(fp,"<是否启用平推门法线 bUseFaceOffsetNorm=\"%s\"/>\n",sText);
	if(desEndPos.bUseFaceOffsetNorm)
		fprintf(fp,"<平推面法线方向 face_offset_norm=\"(%.3f,%.3f,%.3f)\"/>\n",desEndPos.face_offset_norm.x,desEndPos.face_offset_norm.y,desEndPos.face_offset_norm.z);
	sprintf(sText,"%f",desEndPos.fEccentricDist);
	SimplifiedNumString(sText);
	fprintf(fp,"<偏心距 EccentricDist=\"%s\"/>\n",sText);
	sprintf(sText,"%f",desEndPos.fNormOffset);
	SimplifiedNumString(sText);
	fprintf(fp,"<基准面法向偏移量 NormOffset=\"%s\"/>\n",sText);
	sprintf(sText,"%f",desEndPos.fTransverseOffset);
	SimplifiedNumString(sText);
	fprintf(fp,"<横向偏移量 TransverseOffset=\"%s\"/>\n",sText);
	fprintf(fp,"</终端定位参数信息>\n");

	//关联构件列表
	RELATIVE_OBJECT *pRelaObj=NULL;
	fprintf(fp,"<关联构件数量 objsN=\"%d\"/>\n",relativeObjs.GetNodeNum());
	for(pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		fprintf(fp,"<关联构件信息 hObj=\"0x%x\" axis_flag=\"%d\">\n",pRelaObj->hObj,pRelaObj->mirInfo.axis_flag);
		bool bSpecOrigin=!pRelaObj->mirInfo.origin.IsZero();
		if(bSpecOrigin)
			strcpy(sText,"true");
		else
			strcpy(sText,"false");
		fprintf(fp,"<是否有对称基点 bSpecOrigin=\"%s\"/>\n",sText);
		if(bSpecOrigin)
			fprintf(fp,"<对称基点坐标 origin=\"(%.3f,%.3f,%.3f)\"/>\n",pRelaObj->mirInfo.origin.x,pRelaObj->mirInfo.origin.y,pRelaObj->mirInfo.origin.z);
		if(pRelaObj->mirInfo.axis_flag&0x08)
		{
			fprintf(fp,"<对称旋转信息>\n");
			fprintf(fp,"<对称旋转次数 array_num=\"%d\"/>\n",pRelaObj->mirInfo.array_num);
			fprintf(fp,"<对称旋转角度 rotate_angle=\"%d\"/>\n",pRelaObj->mirInfo.rotate_angle);
			fprintf(fp,"</旋转对称信息>\n");
		}
		fprintf(fp,"</关联构件信息>\n");
	}
	pLsRefList->ToXmlFile(fp,schema);
	fprintf(fp,"</H型钢信息>\n");
}
//自协同中心通过管道读取数据
void CLDSLineHShape::FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord)
{
	//读取影射构件参数和构件从属句柄 wht 12-09-14
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
		buffer.ReadString(spec);	//规格
		buffer.Read(&cfgword.flag.word,24);
		buffer.ReadByte(&_material);
		buffer.ReadByte(&_cQualityLevel);
		buffer.ReadBooleanThin(&_bVirtualPart);//隐含构件
		buffer.ReadInteger(&_iSeg.iSeg);
		buffer.ReadString(sPartNo);	//构件编号
		buffer.ReadInteger(&m_hArcLift);//预拱项 wht 16-01-07
		buffer.ReadString(layer(),4);	//扁钢所在图层.
		buffer.ReadInteger(&m_uStatMatCoef);
		buffer.ReadInteger(&m_hPartWeldParent);
		buffer.ReadBooleanThin(&m_bDisplayHole);//显示螺栓孔及切割面
		buffer.ReadString(sNotes,51);
		buffer.ReadDouble(&size.wide);	
		buffer.ReadDouble(&size.height);
		buffer.ReadDouble(&size.thick);
		buffer.ReadDouble(&size.thick2);
		buffer.ReadBooleanThin(&m_bUserSpecColor);	//用户指定构件颜色
		if(m_bUserSpecColor)
			buffer.ReadDword(&crMaterial);		//构件颜色
		buffer.ReadWord(&m_iNormCalStyle);
		buffer.ReadDouble(&nearWorkPlaneNorm.x);
		buffer.ReadDouble(&nearWorkPlaneNorm.y);
		buffer.ReadDouble(&nearWorkPlaneNorm.z);
		buffer.ReadInteger(&hWorkNormRefPart);
		buffer.ReadDouble(&workPlaneNorm.x);	// 主工作平面法线方向.
		buffer.ReadDouble(&workPlaneNorm.y);	// 主工作平面法线方向.
		buffer.ReadDouble(&workPlaneNorm.z);	// 主工作平面法线方向.
		//关联构件列表
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
		//H型钢螺栓区
		pLsRefList->FromBuffer(buffer,m_pModel);
	}
	if(dwStartPermission&dwUpdateWord)
	{
		buffer.ReadInteger(&dw);
		pStart=BelongModel()->FromNodeHandle(dw);	// 起点句柄
		f3dPoint start;
		buffer.ReadDouble(&start.x);// 实际楞线起点
		buffer.ReadDouble(&start.y);// 实际楞线起点
		buffer.ReadDouble(&start.z);// 实际楞线起点
		baseline.SetStart(start);
		buffer.ReadPoint(m_xStartBeforeLift);
		//始端正头计算参数
		buffer.ReadDouble(&start_oddment);
		buffer.ReadInteger(&desStartOdd.m_iOddCalStyle);
		buffer.ReadInteger(&desStartOdd.m_hRefPart1);
		buffer.ReadInteger(&desStartOdd.m_hRefPart2);
		buffer.ReadDouble(&desStartOdd.m_fCollideDist);
		//始端定位参数
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
		//设计参数
		buffer.ReadInteger(&m_bLockStartPos);
	}
	if(dwEndPermission&dwUpdateWord)
	{
		buffer.ReadInteger(&dw);
		pStart=BelongModel()->FromNodeHandle(dw);	// 起点句柄
		f3dPoint end;
		buffer.ReadDouble(&end.x);// 实际楞线起点
		buffer.ReadDouble(&end.y);// 实际楞线起点
		buffer.ReadDouble(&end.z);// 实际楞线起点
		baseline.SetEnd(end);
		buffer.ReadPoint(m_xEndBeforeLift);
		//终端正头计算参数
		buffer.ReadDouble(&end_oddment); 
		buffer.ReadInteger(&desEndOdd.m_iOddCalStyle);
		buffer.ReadInteger(&desEndOdd.m_hRefPart1);
		buffer.ReadInteger(&desEndOdd.m_hRefPart2);
		buffer.ReadDouble(&desEndOdd.m_fCollideDist);
		//终端定位参数
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
		//设计参数
		buffer.ReadInteger(&m_bLockEndPos);
	}
	SetModified();
}
//通过管道向协同中心写入数据
void CLDSLineHShape::ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord)
{
	//保存影射构件参数和构件从属句柄 wht 12-09-14
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
		buffer.WriteString(spec);	//规格
		buffer.Write(cfgword.flag.word,24);
		buffer.WriteByte(cMaterial);
		buffer.WriteByte(m_cQualityLevel);
		buffer.WriteBooleanThin(_bVirtualPart);//隐含构件
		buffer.WriteInteger(iSeg);
		buffer.WriteString(sPartNo);	//构件编号
		buffer.WriteInteger(m_hArcLift);//预拱项 wht 16-01-07
		buffer.WriteString(layer());	//扁钢所在图层.
		buffer.WriteInteger(m_uStatMatCoef);
		buffer.WriteInteger(m_hPartWeldParent);
		buffer.WriteBooleanThin(m_bDisplayHole);//显示螺栓孔及切割面
		buffer.WriteString(sNotes);
		buffer.WriteDouble(size_wide);	
		buffer.WriteDouble(size_height);
		buffer.WriteDouble(size_thick);
		buffer.WriteDouble(size_thick2);
		buffer.WriteBooleanThin(m_bUserSpecColor);	//用户指定构件颜色
		if(m_bUserSpecColor)
			buffer.WriteDword(crMaterial);		//构件颜色

		buffer.WriteWord(m_iNormCalStyle);	//1000083
		buffer.WriteDouble(nearWorkPlaneNorm.x);
		buffer.WriteDouble(nearWorkPlaneNorm.y);
		buffer.WriteDouble(nearWorkPlaneNorm.z);
		buffer.WriteInteger(hWorkNormRefPart);
		buffer.WriteDouble(workPlaneNorm.x);	// 主工作平面法线方向.
		buffer.WriteDouble(workPlaneNorm.y);	// 主工作平面法线方向.
		buffer.WriteDouble(workPlaneNorm.z);	// 主工作平面法线方向.
		//关联构件列表
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
		//H型钢螺栓区
		pLsRefList->ToBuffer(buffer);
	}
	if(dwStartPermission&dwUpdateWord)
	{
		if(pStart)
			buffer.WriteInteger(pStart->handle);	// 起点句柄
		else
			buffer.WriteInteger(0);
		buffer.WriteDouble(baseline.Start().x);	// 实际楞线起点
		buffer.WriteDouble(baseline.Start().y);	// 实际楞线起点
		buffer.WriteDouble(baseline.Start().z);	// 实际楞线起点
		buffer.WritePoint(m_xStartBeforeLift);
		//始端正头计算参数
		buffer.WriteDouble(start_oddment);
		buffer.WriteInteger(desStartOdd.m_iOddCalStyle);
		buffer.WriteInteger(desStartOdd.m_hRefPart1);
		buffer.WriteInteger(desStartOdd.m_hRefPart2);
		buffer.WriteDouble(desStartOdd.m_fCollideDist);
		//始端定位参数
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
		//设计参数
		buffer.WriteInteger(m_bLockStartPos);
	}
	if(dwEndPermission&dwUpdateWord)
	{
		if(pEnd)
			buffer.WriteInteger(pEnd->handle);
		else
			buffer.WriteInteger(0);
		buffer.WriteDouble(baseline.End().x);	// 实际楞线终点
		buffer.WriteDouble(baseline.End().y);	// 实际楞线终点
		buffer.WriteDouble(baseline.End().z);	// 实际楞线终点
		buffer.WritePoint(m_xEndBeforeLift);
		//终端正头计算参数
		buffer.WriteDouble(end_oddment); 
		buffer.WriteInteger(desEndOdd.m_iOddCalStyle);
		buffer.WriteInteger(desEndOdd.m_hRefPart1);
		buffer.WriteInteger(desEndOdd.m_hRefPart2);
		buffer.WriteDouble(desEndOdd.m_fCollideDist);
		//终端定位参数
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
		//设计参数
		buffer.WriteInteger(m_bLockEndPos);
	}
}
void CLDSLineHShape::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/)
{
	if(_console==NULL||m_pModel==NULL||!Lock())	//已调用过)
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
	//数据未修改
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
		//初始化顶点集合
		double length=DISTANCE(Start(),End());
		for(int index=0;index<3;index++)
		{
			if(index==0 || index==1)
			{	//X-|X+钢板顶点
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
			{	//中间连板顶点
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
		//初始化面链表
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		COLORREF crSlot = GetColor(CLS_LINESLOT,3,cMaterial);
		if(m_bUserSpecColor)	//用户指定构件颜色
			crSlot=crMaterial;
		f3dPolyFace* pLeftOutsideFace=NULL,*pLeftInsideFace=NULL,
					 *pRightOutsideFace=NULL,*pRightInsideFace=NULL,
					 *pCenterTopFace=NULL,*pCenterBtmFace=NULL;
		for(int index=0;index<3;index++)
		{
			int iStart=index*8;
			//顶面
			f3dPolyFace* pTopFace=pBody->faceList.append();
			pTopFace->material = crSlot;
			for(int i=0;i<4;i++)
			{
				if(i==0)
					createFace.NewOutterEdgeLine(pTopFace,((i+1)%4)*2+1+iStart,1+iStart);
				else
					createFace.NewOutterEdgeLine(pTopFace,((i+1)%4)*2+1+iStart);
			}
			//底面
			f3dPolyFace *pBtmFace=pBody->faceList.append();
			pBtmFace->material = crSlot;
			for(int i=0;i<4;i++)
			{
				if(i==0)
					createFace.NewOutterEdgeLine(pBtmFace,(4-1)*2+iStart,iStart);
				else
					createFace.NewOutterEdgeLine(pBtmFace,(4-1-i)*2+iStart);
			}
			//侧面
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
		//定义螺栓孔
		ATOM_LIST<BOLTHOLE_INFO> left_wing_ls,right_wing_ls,middle_wing_ls;
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(m_bDisplayHole || bDisplayHole|| pLsRef->GetLsPtr()->m_bVirtualPart)
			{	//系统要求显示螺栓孔或当前螺栓为虚拟螺栓
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
					if(centre.x>0)	//右(X+)侧肢螺栓
						right_wing_ls.append(boltHole);
					else			//左(X-)侧肢螺栓
						left_wing_ls.append(boltHole);
				}
				else				//中间肢螺栓
					middle_wing_ls.append(boltHole);
			}
		}
		CHoleMaker holeMaker;
		UCS_STRU hole_ucs;
		for(BOLTHOLE_INFO *pLsCir=left_wing_ls.GetFirst();pLsCir;pLsCir=left_wing_ls.GetNext())
		{	//左(X-)侧肢螺栓
			COLORREF crLsHole=GetColor(CLS_BOLT,ftoi(pLsCir->bolt_d),' ');
			hole_ucs.origin = pLsCir->centre;
			hole_ucs.axis_x = pLsCir->axis_x;
			hole_ucs.axis_z = pLsCir->norm;
			holeMaker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pLeftOutsideFace,pLeftInsideFace,pBody,crLsHole);
		}
		for(BOLTHOLE_INFO *pLsCir=right_wing_ls.GetFirst();pLsCir;pLsCir=right_wing_ls.GetNext())
		{	//右(X+)侧肢螺栓
			COLORREF crLsHole=GetColor(CLS_BOLT,ftoi(pLsCir->bolt_d),' ');
			hole_ucs.origin = pLsCir->centre;
			hole_ucs.axis_x = pLsCir->axis_x;
			hole_ucs.axis_z = pLsCir->norm;
			holeMaker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pRightOutsideFace,pRightInsideFace,pBody,crLsHole);
		}
		for(BOLTHOLE_INFO *pLsCir=middle_wing_ls.GetFirst();pLsCir;pLsCir=middle_wing_ls.GetNext())
		{	//中间肢螺栓
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
		return;	//指定工作面法线
	CSuperSmartPtr<CLDSPart>pDatumPart;
	f3dPoint datum_vec,current_vec;
	current_vec=Start()-End();
	normalize(current_vec);
	if(hWorkNormRefPart!=0)
	{
		pDatumPart=BelongModel()->FromPartHandle(hWorkNormRefPart);
		if(pDatumPart.IsHasPtr()&&pDatumPart->IsLinePart())
		{	//直线型杆件
			datum_vec=pDatumPart.LinePartPointer()->End()-pDatumPart.LinePartPointer()->Start();
			workPlaneNorm=datum_vec^current_vec;
		}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(pDatumPart.IsHasPtr()&&pDatumPart->IsArcPart())
		{	//环形杆件
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
	else if(pStart&&pStart->hFatherPart>=0x20&&pStart->hFatherPart!=handle)	//始端搭接在其它杆件上
	{
		if(pStart->hFatherPart>=0x20)
			pDatumPart=BelongModel()->FromPartHandle(pStart->hFatherPart);
		else if(desStartPos.hDatumPart>=0x20)
			pDatumPart=BelongModel()->FromPartHandle(desStartPos.hDatumPart);
		if(pDatumPart.IsHasPtr()&&pDatumPart->IsLinePart())
		{	//直线型杆件
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
		{	//环形杆件
			workPlaneNorm=pStart->Position(true)-f3dPoint(pDatumPart.ArcPartPointer()->baseline.Center());
		}
#endif
		normalize(workPlaneNorm);
	}
	else if(pEnd&&pEnd->hFatherPart>=0x20&&pEnd->hFatherPart!=handle)	//终端搭接在其它杆件上
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
		{	//环形杆件
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
		return;	//已到位
	if(pStart==NULL)
		return;
#ifdef __COMPLEX_PART_INC_
	if(pLift==NULL&&m_hArcLift>0x20)
		pLift=BelongModel()->FromArcLiftHandle(m_hArcLift);
#else
	pLift=NULL;
#endif
	if(IsStartPosLocked())
		return;	//始端位置已被锁定（可能是火曲点）
	SetModified();
	if(desStartPos.endPosType==0)
		SetStart(pStart->Position(true)+workPlaneNorm*desStartPos.fNormOffset);
	else if(desStartPos.endPosType==1)
	{	//搭接连接
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
				if(desStartPos.datum_pos_style==0)	//以父角钢楞线为基准
					startPos = pDatumLineAngle->GetDatumPosBer(pStart);
				else if(desStartPos.datum_pos_style==1)	//以父角钢X支为基准
					startPos = pDatumLineAngle->GetDatumPosWingX(pStart);
				else if(desStartPos.datum_pos_style==2)	//以父角钢Y支为基准
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
					if(desStartPos.datum_pos_style==3)	//以父角钢楞线为基准平推
						startPos = pDatumLineAngle->GetDatumPosBerByFaceOffset(pStart,face_norm);
					else if(desStartPos.datum_pos_style==4)	//以父角钢X肢为基准平推
						startPos = pDatumLineAngle->GetDatumPosWingXByFaceOffset(pStart,face_norm);
					else //if(desStartPos.datum_pos_style==5)//以父角钢Y肢为基准平推
						startPos = pDatumLineAngle->GetDatumPosWingYByFaceOffset(pStart,face_norm);
				}
			}
			else
			{
				CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
				f3dLine datumLine(pLinePart->Start(),pLinePart->End());
				f3dPoint datum_vec=datumLine.endPt-datumLine.startPt;
				f3dPoint current_vec=End()-Start();
				//current_vec为0时使用单线方向 wht 10-09-09
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
				normalize(transverseOffsetVec);	//横向偏移方向
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
	{	//点线投影
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
	//增加心线交点以及指定坐标两种定位方式 wht 10-10-20
	else if(desStartPos.endPosType==3)
	{	//心线交点(两杆件交点)
		CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(desStartPos.hDatumPart,CLS_LINETUBE);		//基准钢管
		CLDSLineTube *pCrossTube=(CLDSLineTube*)BelongModel()->FromPartHandle(desStartPos.hDatumStartPart,CLS_LINETUBE);//交叉钢管
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
			inters_pt+=workPlaneNorm*desEndPos.fNormOffset;//考虑法向偏移量 wht 11-07-01
			SetStart(inters_pt);
		}
		else	//未求得合法交点 
			return;
	}
	else if(desStartPos.endPosType==4)//直接指定坐标
		return;
}
void CLDSLineHShape::CalEndPos(CLDSArcLift *pLift/*=NULL*/)
{
	if(!LockEnd())
		return;	//已到位
	if(pEnd==NULL)
		return;
#ifdef __COMPLEX_PART_INC_
	if(pLift==NULL&&m_hArcLift>0x20)
		pLift=BelongModel()->FromArcLiftHandle(m_hArcLift);
#else
	pLift=NULL;
#endif
	if(IsEndPosLocked())
		return;	//终端位置已被锁定（可能是火曲点）
	SetModified();
	if(desEndPos.endPosType==0)
		SetEnd(pEnd->Position(true)+workPlaneNorm*desEndPos.fNormOffset);
	else if(desEndPos.endPosType==1)
	{	//搭接连接
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
				if(desEndPos.datum_pos_style==0)	//以父角钢楞线为基准
					endPos = pDatumLineAngle->GetDatumPosBer(pEnd);
				else if(desEndPos.datum_pos_style==1)	//以父角钢X支为基准
					endPos = pDatumLineAngle->GetDatumPosWingX(pEnd);
				else if(desEndPos.datum_pos_style==2)	//以父角钢Y支为基准
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
					if(desEndPos.datum_pos_style==3)	//以父角钢楞线为基准平推
						endPos = pDatumLineAngle->GetDatumPosBerByFaceOffset(pEnd,face_norm);
					else if(desEndPos.datum_pos_style==4)	//以父角钢X肢为基准平推
						endPos = pDatumLineAngle->GetDatumPosWingXByFaceOffset(pEnd,face_norm);
					else //if(desEndPos.datum_pos_style==5)//以父角钢Y肢为基准平推
						endPos = pDatumLineAngle->GetDatumPosWingYByFaceOffset(pEnd,face_norm);
				}
			}
			else
			{
				CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
				f3dLine datumLine(pLinePart->Start(),pLinePart->End());
				f3dPoint datum_vec=datumLine.endPt-datumLine.startPt;
				f3dPoint current_vec=Start()-End();
				//current_vec为0时使用单线方向 wht 10-09-09
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
				normalize(transverseOffsetVec);	//横向偏移方向
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
	{	//点线投影
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
	//增加心线交点以及指定坐标两种定位方式 wht 10-10-20
	else if(desEndPos.endPosType==3)
	{	//心线交点(两杆件交点)
		CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(desEndPos.hDatumPart,CLS_LINETUBE);		//基准钢管
		CLDSLineTube *pCrossTube=(CLDSLineTube*)BelongModel()->FromPartHandle(desEndPos.hDatumStartPart,CLS_LINETUBE);//交叉钢管
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
			inters_pt+=workPlaneNorm*desEndPos.fNormOffset;//考虑法向偏移量 wht 11-07-01
			SetEnd(inters_pt);
		}
		else	//未求得合法交点 
			return;
	}
	else if(desEndPos.endPosType==4)//直接指定坐标
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
//属性管理
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
	//基本信息
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
	CLDSLineHShape::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Project Information","Project Information"));//影射信息
	CLDSLineHShape::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Shadow Mother Object","The shadow mother object's handle of currently part,when it's 0x0 means that the currently object is original object。"));//影射母构件
	CLDSLineHShape::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly","The handle of vest in block assembly 。"));//归属的部件引用
	CLDSLineHShape::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block","The handle of vest in block。"));//归属的部件
	CLDSLineHShape::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"BOM Number","BOM Number"));
	CLDSLineHShape::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"Weld Parent Part"," if the part needs to weld to other parts, then this attribute is the handle of attached part; If the part does not need to be welded, the value is 0x0。"));
	CLDSLineHShape::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"Hole Visible","Whethe to show the hole","Yes|No"));
	CLDSLineHShape::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"Virtual Auxiliary Part","Virtual auxiliary parts are only used to assist other part's position, is not the real statistic material of parts,so not contributing to the material statistics。","Yes|No"));
	CLDSLineHShape::AddPropItem("length",PROPLIST_ITEM(id++,"length","H-steel's physical length"));
	CLDSLineHShape::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"Specify Part Color","Whether the part's color is specified by the user","Yes|No"));
	CLDSLineHShape::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Part Color","Part Color"));
	CLDSLineHShape::AddPropItem("sNotes",PROPLIST_ITEM(id++,"Notes","Notes"));
	//定位信息
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
	CLDSLineHShape::AddPropItem("workPlaneNorm",PROPLIST_ITEM(id++,"Work Plane Normal","H型钢基准面法线"));
	CLDSLineHShape::AddPropItem("hWorkNormRefPart",PROPLIST_ITEM(id++,"Normal Reference Rod","It is work reference rod when calculating datum plane's normal。It is start node or end node of parent rod when it is 0X0(It can't be rod itself)"));
	CLDSLineHShape::AddPropItem("workPlaneNorm.x",PROPLIST_ITEM(id++,"X Coordinate","The x coordinate of datum plane's normal"));
	CLDSLineHShape::AddPropItem("workPlaneNorm.y",PROPLIST_ITEM(id++,"Y Coordinate","The y coordinate of datum plane's normal"));
	CLDSLineHShape::AddPropItem("workPlaneNorm.z",PROPLIST_ITEM(id++,"Z Coordinate","The z coordinate of datum plane's normal"));
	CLDSLineHShape::AddPropItem("nearWorkPlaneNorm",PROPLIST_ITEM(id++,"Datum Plane Near Normal","Its generally specified by the user, the system can calculate it according to the near normals and channel steel axis。"));
	CLDSLineHShape::AddPropItem("m_iNormCalStyle",PROPLIST_ITEM(id++,"Calculate Mode","直接给定还是通过父杆件给定","0.Specify Directly|1.Reference Rod|2.Horizontal Pedestal"));
	CLDSLineHShape::AddPropItem("nearWorkPlaneNorm.x",PROPLIST_ITEM(id++,"X Coordinate","The x coordinate of datum plane's normal"));
	CLDSLineHShape::AddPropItem("nearWorkPlaneNorm.y",PROPLIST_ITEM(id++,"Y Coordinate","The y coordinate of datum plane's normal"));
	CLDSLineHShape::AddPropItem("nearWorkPlaneNorm.z",PROPLIST_ITEM(id++,"Z Coordinate","The z coordinate of datum plane's normal"));
	//始端信息
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
	CLDSLineHShape::AddPropItem("desStartPos.datum_pos_style",PROPLIST_ITEM(id++,"Datum Point Locate Mode"，"The start locate mode of H-steel"));
	CLDSLineHShape::AddPropItem("desStartPos.hDatumPart",PROPLIST_ITEM(id++,"Lap Joint Datum Angle","The handle of lap joint datum linePart"));
	CLDSLineHShape::AddPropItem("desStartPos.fNormOffset",PROPLIST_ITEM(id++,"Normal Offset"));
	CLDSLineHShape::AddPropItem("desStartPos.fEccentricDist",PROPLIST_ITEM(id++,"Offset Distance"));
	CLDSLineHShape::AddPropItem("desStartPos.face_offset_norm",PROPLIST_ITEM(id++,"Offset Normal"));
	CLDSLineHShape::AddPropItem("desStartPos.fTransverseOffset",PROPLIST_ITEM(id++,"Lateral Offset","The offset of x negative direction"));
	CLDSLineHShape::AddPropItem("desStartPos.hDatumStartPart",PROPLIST_ITEM(id++,"Start Datum Rod","The datum rod  is at project line's start"));
	CLDSLineHShape::AddPropItem("desStartPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,"Datum Endpoint","The datum h steel's datum endpoint"));
	CLDSLineHShape::AddPropItem("desStartPos.hDatumEndPart",PROPLIST_ITEM(id++,"End Datum Rod","The datum rod at project line's end"));
	CLDSLineHShape::AddPropItem("desStartPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"Datum Endpoint","The datum rod's datum endpoint"));
	//终端信息
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
	//基本信息
	CLDSLineHShape::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"H型钢基本信息","H型钢基本信息"));
	CLDSLineHShape::AddPropItem("handle",PROPLIST_ITEM(id++,"句柄","H型钢句柄"));
	CLDSLineHShape::AddPropItem("layer",PROPLIST_ITEM(id++,"图层","槽钢所在图层"));
	CLDSLineHShape::AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号","配材号"));
	CLDSLineHShape::AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号","分段号"));
	CLDSLineHShape::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号","构件编号"));
	CLDSLineHShape::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质","H型钢材质"));
	CLDSLineHShape::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"质量等级","质量等级"));
	CLDSLineHShape::AddPropItem("ibHShapeType",PROPLIST_ITEM(id++,"类别","H型钢的类别","0.HW|1.HM|2.HN|3.HP"));
	CLDSLineHShape::AddPropItem("specification",PROPLIST_ITEM(id++,"规格","H型钢的规格"));
	CLDSLineHShape::AddPropItem("thick1",PROPLIST_ITEM(id++,"腹板厚度","H型钢的腹板宽度"));
	CLDSLineHShape::AddPropItem("thick2",PROPLIST_ITEM(id++,"翼缘厚度","H型钢的翼缘厚度"));
	CLDSLineHShape::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"统材系数","统材系数"));
	CLDSLineHShape::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"焊接父构件","如果此构件需要焊接到其它构件上,那么此属性就是本构件所依附的焊接构件的句柄;如果此构件不需要焊接,此值为0X0。"));
	CLDSLineHShape::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"显示孔","是否显示孔","是|否"));
	CLDSLineHShape::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"虚拟辅助构件","虚拟辅助构件只用于辅助其它构件定位，并不是结构体真实具有的统材构件，所以不参预材料统计。","是|否"));
	CLDSLineHShape::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"指定构件颜色","构件颜色是否为用户指定","是|否"));
	CLDSLineHShape::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"构件颜色","构件颜色"));
	CLDSLineHShape::AddPropItem("length",PROPLIST_ITEM(id++,"实际长度","H型钢的实际长度"));
	CLDSLineHShape::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"关联构件","与当前构件相关联的构件以及对称方式"));
	CLDSLineHShape::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"影射信息","影射信息"));//影射信息
	CLDSLineHShape::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"影射母构件","当前构件对应的影射母构件句柄，该句柄为0x0时表示当前构件为原始构件。"));//影射母构件
	CLDSLineHShape::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"归属的部件引用","当前构件归属的部件引用句柄。"));//归属的部件引用
	CLDSLineHShape::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"归属部件","当前构件归属的部件句柄。"));//归属的部件
	CLDSLineHShape::AddPropItem("sNotes",PROPLIST_ITEM(id++,"备注","备注"));
	//基准面信息
	CLDSLineHShape::AddPropItem("workPlaneNorm",PROPLIST_ITEM(id++,"基准面法线","H型钢基准面法线"));
	CLDSLineHShape::AddPropItem("m_iNormCalStyle",PROPLIST_ITEM(id++,"计算方式","直接给定还是通过父杆件给定","0.直接指定|1.参照杆件|2.水平底座"));
	CLDSLineHShape::AddPropItem("workPlaneNorm.x",PROPLIST_ITEM(id++,"X坐标分量","槽钢基准面法线X轴方向的坐标"));
	CLDSLineHShape::AddPropItem("workPlaneNorm.y",PROPLIST_ITEM(id++,"Y坐标分量","槽钢基准面法线Y轴方向的坐标"));
	CLDSLineHShape::AddPropItem("workPlaneNorm.z",PROPLIST_ITEM(id++,"Z坐标分量","槽钢基准面法线Z轴方向的坐标"));
	CLDSLineHShape::AddPropItem("nearWorkPlaneNorm",PROPLIST_ITEM(id++,"基准面近似法线","H型钢基准面近似法线一般由用户指定，系统可根据近似法线及H型钢轴线精确计算出H型钢的基准面法线。"));
	CLDSLineHShape::AddPropItem("nearWorkPlaneNorm.x",PROPLIST_ITEM(id++,"X坐标分量","槽钢基准面法线X轴方向的坐标"));
	CLDSLineHShape::AddPropItem("nearWorkPlaneNorm.y",PROPLIST_ITEM(id++,"Y坐标分量","槽钢基准面法线Y轴方向的坐标"));
	CLDSLineHShape::AddPropItem("nearWorkPlaneNorm.z",PROPLIST_ITEM(id++,"Z坐标分量","槽钢基准面法线Z轴方向的坐标"));
	CLDSLineHShape::AddPropItem("hWorkNormRefPart",PROPLIST_ITEM(id++,"法线参照杆件","计算槽钢基准面法线时的工作面参照杆件。0X0时默认为始端或终端节点父杆件(不能为杆件本身)"));
	//始终定位信息
	CLDSLineHShape::AddPropItem("startInfo",PROPLIST_ITEM(id++,"始端信息","H型钢始端信息"));
	CLDSLineHShape::AddPropItem("hStart",PROPLIST_ITEM(id++,"起点句柄","H型钢起点句柄"));
	CLDSLineHShape::AddPropItem("oddStart",PROPLIST_ITEM(id++,"正负头","H型钢始端正负头值"));
	CLDSLineHShape::AddPropItem("startPos",PROPLIST_ITEM(id++,"始端设计坐标","H型钢始端设计坐标"));
	CLDSLineHShape::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X坐标","H型钢始端X轴方向的坐标"));
	CLDSLineHShape::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y坐标","H型钢始端Y轴方向的坐标"));
	CLDSLineHShape::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z坐标","H型钢始端Z轴方向的坐标"));
	CLDSLineHShape::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"始端实际坐标","H型钢始端考虑正负头后的坐标位置"));
	CLDSLineHShape::AddPropItem("desStartPos",PROPLIST_ITEM(id++,""));
	CLDSLineHShape::AddPropItem("desStartPos.endPosType",PROPLIST_ITEM(id++,"定位方式","H型钢始端基点位置","节点即基点|搭接连接|点线投影|心线交点|指定坐标|"));
	CLDSLineHShape::AddPropItem("desStartPos.datum_pos_style",PROPLIST_ITEM(id++,"基点定位方式","始端基点定位方式","楞线投影|X肢心线投影|Y肢心线投影|向楞线平推|X肢心线平推|Y肢心线平推"));
	CLDSLineHShape::AddPropItem("desStartPos.hDatumPart",PROPLIST_ITEM(id++,"搭接位置基准杆件","搭接位置基准杆件"));
	CLDSLineHShape::AddPropItem("desStartPos.fNormOffset",PROPLIST_ITEM(id++,"基准面法线偏移量","基准面法线偏移量"));
	CLDSLineHShape::AddPropItem("desStartPos.fEccentricDist",PROPLIST_ITEM(id++,"偏心距","基准偏心距"));
	CLDSLineHShape::AddPropItem("desStartPos.face_offset_norm",PROPLIST_ITEM(id++,"平推面法线","平推面法线"));
	CLDSLineHShape::AddPropItem("desStartPos.fTransverseOffset",PROPLIST_ITEM(id++,"横向偏移量","X轴负方向偏移量"));
	CLDSLineHShape::AddPropItem("desStartPos.hDatumStartPart",PROPLIST_ITEM(id++,"投影线始端所在基准杆件"));
	CLDSLineHShape::AddPropItem("desStartPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,"基准扁钢的基准端点","基准扁钢的基准端点","始端|终端"));
	CLDSLineHShape::AddPropItem("desStartPos.hDatumEndPart",PROPLIST_ITEM(id++,"投影线终端所在基准杆件"));
	CLDSLineHShape::AddPropItem("desStartPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"基准杆件的基准端点","基准杆件的基准端点","始端|终端"));
	//终端定位信息
	CLDSLineHShape::AddPropItem("endInfo",PROPLIST_ITEM(id++,"终端信息","H型钢终端信息"));
	CLDSLineHShape::AddPropItem("hEnd",PROPLIST_ITEM(id++,"终点句柄","H型钢终点句柄"));
	CLDSLineHShape::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"正负头","H型钢终端正负头值"));
	CLDSLineHShape::AddPropItem("endPos",PROPLIST_ITEM(id++,"终端设计坐标","H型钢终端设计坐标"));
	CLDSLineHShape::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"终端实际坐标","H型钢终端考虑正负头后的坐标位置"));
	CLDSLineHShape::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X坐标","H型钢终端坐标X轴方向的坐标值"));
	CLDSLineHShape::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y坐标","H型钢终端坐标Y轴方向的坐标值"));
	CLDSLineHShape::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z坐标","H型钢终端坐标Z轴方向的坐标值"));
	CLDSLineHShape::AddPropItem("desEndPos",PROPLIST_ITEM(id++,""));
	CLDSLineHShape::AddPropItem("desEndPos.endPosType",PROPLIST_ITEM(id++,"定位方式","H型钢终端基点位置","节点即基点|搭接连接|点线投影|心线交点|指定坐标|"));
	CLDSLineHShape::AddPropItem("desEndPos.datum_pos_style",PROPLIST_ITEM(id++,"基点定位方式","终端基点定位方式","楞线投影|X肢心线投影|Y肢心线投影|向楞线平推|X肢心线平推|Y肢心线平推"));
	CLDSLineHShape::AddPropItem("desEndPos.hDatumPart",PROPLIST_ITEM(id++,"搭接位置基准杆件","搭接位置基准杆件"));
	CLDSLineHShape::AddPropItem("desEndPos.fNormOffset",PROPLIST_ITEM(id++,"基准面法线偏移量","基准面法线偏移量"));
	CLDSLineHShape::AddPropItem("desEndPos.fEccentricDist",PROPLIST_ITEM(id++,"偏心距","基准偏心距"));
	CLDSLineHShape::AddPropItem("desEndPos.face_offset_norm",PROPLIST_ITEM(id++,"平推面法线","平推面法线"));
	CLDSLineHShape::AddPropItem("desEndPos.fTransverseOffset",PROPLIST_ITEM(id++,"横向偏移量","X轴负方向偏移量"));
	CLDSLineHShape::AddPropItem("desEndPos.hDatumStartPart",PROPLIST_ITEM(id++,"投影线始端所在基准杆件"));
	CLDSLineHShape::AddPropItem("desEndPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,"基准扁钢的基准端点","基准扁钢的基准端点","始端|终端"));
	CLDSLineHShape::AddPropItem("desEndPos.hDatumEndPart",PROPLIST_ITEM(id++,"投影线终端所在基准杆件"));
	CLDSLineHShape::AddPropItem("desEndPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"基准杆件的基准端点","基准杆件的基准端点","始端|终端"));
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
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
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
		if(layer(0)=='L')	//接腿构件
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
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
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
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
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
			sprintf(sText,"是");
		else
			sprintf(sText,"否");
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
			sprintf(sText,"0.直接指定");
		else
			sprintf(sText,"1.参照杆件");
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
			strcpy(sText,"节点即基点");
		else if(desStartPos.endPosType==1)
			strcpy(sText,"搭接连接");
		else if(desStartPos.endPosType==2)
			strcpy(sText,"点线投影");
		else if(desStartPos.endPosType==3)
			strcpy(sText,"心线交点");
		else //if(desStartPos.endPosType==4)
			strcpy(sText,"指定坐标");
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
			strcpy(sText,"X肢心线投影");
		else if(desStartPos.datum_pos_style==2)
			strcpy(sText,"Y肢心线投影");
		else if(desStartPos.datum_pos_style==3)
			strcpy(sText,"向楞线平推");
		else if(desStartPos.datum_pos_style==4)
			strcpy(sText,"X肢心线平推");
		else if(desStartPos.datum_pos_style==5)
			strcpy(sText,"Y肢心线平推");
		else //if(desStartPos.datum_pos_style==0)
			strcpy(sText,"向楞线投影");
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
			strcpy(sText,"始端");
		else //if(!desStartPos.bDatumStartStartEnd)
			strcpy(sText,"终端");
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
			strcpy(sText,"始端");
		else //if(!desStartPos.bDatumEndStartEnd)
			strcpy(sText,"终端");
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
			strcpy(sText,"节点即基点");
		else if(desEndPos.endPosType==1)
			strcpy(sText,"搭接连接");
		else if(desEndPos.endPosType==2)
			strcpy(sText,"点线投影");
		else if(desEndPos.endPosType==3)
			strcpy(sText,"心线交点");
		else //if(desEndPos.endPosType==4)
			strcpy(sText,"指定坐标");
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
			strcpy(sText,"X肢心线投影");
		else if(desEndPos.datum_pos_style==2)
			strcpy(sText,"Y肢心线投影");
		else if(desEndPos.datum_pos_style==3)
			strcpy(sText,"向楞线平推");
		else if(desEndPos.datum_pos_style==4)
			strcpy(sText,"X肢心线平推");
		else if(desEndPos.datum_pos_style==5)
			strcpy(sText,"Y肢心线平推");
		else //if(desStartPos.datum_pos_style==0)
			strcpy(sText,"向楞线投影");
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
			strcpy(sText,"始端");
		else //if(!desEndPos.bDatumStartStartEnd)
			strcpy(sText,"终端");
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
			strcpy(sText,"始端");
		else //if(!desEndPos.bDatumEndStartEnd)
			strcpy(sText,"终端");
#endif
	}
#if defined(__TSA_)||defined(__LDS_)
	else if(GetPropID("size.idClassType")==id)
	{
		if(size_idClassType==CLS_LINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText, "Same Leg Angle");
#else
			strcpy(sText, "等肢角钢");
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
				strcpy(sText, "T型组合");
			else if(size_cSubClassType=='D')
				strcpy(sText, "对角组合");
			else if(size_cSubClassType=='X')
				strcpy(sText, "十字组合");
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
			strcpy(sText, "钢管");
		else if(size_idClassType==CLS_LINEFLAT)
			strcpy(sText, "扁钢");
		else if(size_idClassType==CLS_LINESLOT)
			strcpy(sText, "槽钢");
		else if(size_idClassType==0)
			strcpy(sText, "用户定制");
#endif
	}
	else if(GetPropID("m_uStatMatNo")==id)
		sprintf(sText,"%d",m_uStatMatNo);
	else if(GetPropID("m_fWeightRaiseCoef")==id)
	{
		sprintf(sText,"%f",m_fWeightRaiseCoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_iElemType")==id)	//自动判断单元类型
	{
		if(m_iElemType==0)
			strcpy(sText,"0.自动判断");
		else if(m_iElemType==2)
			strcpy(sText,"2.经典梁");
		else if(m_iElemType==3)
			strcpy(sText,"3.柔索");
		else
			strcpy(sText,"1.二力杆");
	}
	else if(GetPropID("m_bNeedSelMat")==id)
	{
		if(m_bNeedSelMat)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
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
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("CalLenCoef.iTypeNo")==id)
	{
		if(CalLenCoef.iTypeNo==0)
			strcpy(sText,"0.智能判断");
		else if(CalLenCoef.iTypeNo==1)
			strcpy(sText,"1.V斜材(跨脚材连接)");
		else if(CalLenCoef.iTypeNo==2)
			strcpy(sText,"2.V斜材(V面连接)");
		else if(CalLenCoef.iTypeNo==3)
			strcpy(sText,"3.V斜材3节间带横连杆");
		else if(CalLenCoef.iTypeNo==4)
			strcpy(sText,"4.V斜材4节间带横连杆");
		else if(CalLenCoef.iTypeNo==5)
			strcpy(sText,"5.指定杆件计算长度");
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
			strcpy(sText,"铰接");
		else if(start_joint_type==JOINT_RIGID)
			strcpy(sText,"刚接");
		else //if(start_joint_type==JOINT_AUTOJUSTIFY)
			strcpy(sText,"自动判断");
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
			strcpy(sText,"中心受力");
		else //if(start_force_type==ECCENTRIC_FORCE)
			strcpy(sText,"偏心受力");
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
			strcpy(sText,"铰接");
		else if(end_joint_type==JOINT_RIGID)
			strcpy(sText,"刚接");
		else //if(end_joint_type==JOINT_AUTOJUSTIFY)
			strcpy(sText,"自动判断");
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
			strcpy(sText,"中心受力");
		else //if(end_force_type==ECCENTRIC_FORCE)
			strcpy(sText,"偏心受力");
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
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
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