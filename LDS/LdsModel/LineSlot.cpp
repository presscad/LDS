//<LOCALE_TRANSLATE BY hxr /> 2015-04-17
// LineSlot.cpp: implementation of the CLDSSlot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tower.h"
#include "CreateFace.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
//槽钢与扁铁暂时放在一个CPP中
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//槽钢
CLDSLineSlot::CLDSLineSlot(void)
{
	size_idClassType=m_nClassTypeId = CLS_LINESLOT;
	//strcpy(m_sClassName,"CLDSLineSlot");
	size_height=50;
	size_wide=50;
	size_thick=8;
	_material = CSteelMatLibrary::Q235BriefMark();//'S';
	strcpy(spec,"");
	pSolidBody = NULL;
	is_visible=TRUE;
	is_data_modified = TRUE;
	m_uStatMatCoef = 1;
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
	hWorkNormRefPart = 0;
	m_iNormCalStyle = 1;				//默认为杆件定位
	hAttachTube = 0;
	fLen = 0;
	memset(&desStartPos,0,sizeof(CTubeEndPosPara));
	memset(&desEndPos,0,sizeof(CTubeEndPosPara));
}

CLDSLineSlot::~CLDSLineSlot()
{
	if(pSolidBody)
	{
		delete pSolidBody;
		pSolidBody=NULL;
	}
}
void CLDSLineSlot::IndependentShadowPropFromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
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
		buffer.ReadInteger(&_hPartWeldParent);
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
		//buffer.ReadDouble(&start_oddment);
		//buffer.ReadInteger(&desStartOdd.m_iOddCalStyle);
		//buffer.ReadInteger(&desStartOdd.m_hRefPart1);
		//buffer.ReadInteger(&desStartOdd.m_hRefPart2);
		//buffer.ReadDouble(&desStartOdd.m_fCollideDist);
		//buffer.ReadDouble(&end_oddment); 
		//buffer.ReadInteger(&desEndOdd.m_iOddCalStyle);
		//buffer.ReadInteger(&desEndOdd.m_hRefPart1);
		//buffer.ReadInteger(&desEndOdd.m_hRefPart2);
		//buffer.ReadDouble(&desEndOdd.m_fCollideDist);
		//buffer.ReadInteger(&desStartPos.endPosType);
		//buffer.ReadByte(&desStartPos.datum_pos_style);	
		//buffer.ReadInteger(&desStartPos.hDatumPart);
		//buffer.ReadInteger(&desStartPos.hDatumStartPart);
		//buffer.ReadInteger(&desStartPos.hDatumEndPart);
		//buffer.ReadInteger(&desStartPos.bDatumEndStartEnd);
		//buffer.ReadInteger(&desStartPos.bDatumStartStartEnd);
		//buffer.ReadBooleanThin(&desStartPos.bUseFaceOffsetNorm);
		//if(desStartPos.bUseFaceOffsetNorm)
		//{
		//	buffer.ReadDouble(&desStartPos.face_offset_norm.x);
		//	buffer.ReadDouble(&desStartPos.face_offset_norm.y);
		//	buffer.ReadDouble(&desStartPos.face_offset_norm.z);
		//}
		//buffer.ReadDouble(&desStartPos.fEccentricDist);
		//buffer.ReadDouble(&desStartPos.fNormOffset);
		//buffer.ReadDouble(&desStartPos.fTransverseOffset);
		//终端
		//buffer.ReadInteger(&desEndPos.endPosType);
		//buffer.ReadByte(&desEndPos.datum_pos_style);	
		//buffer.ReadInteger(&desEndPos.hDatumPart);
		//buffer.ReadInteger(&desEndPos.hDatumStartPart);
		//buffer.ReadInteger(&desEndPos.hDatumEndPart);
		//buffer.ReadInteger(&desEndPos.bDatumEndStartEnd);
		//buffer.ReadInteger(&desEndPos.bDatumStartStartEnd);
		//buffer.ReadBooleanThin(&desEndPos.bUseFaceOffsetNorm);
		//if(desEndPos.bUseFaceOffsetNorm)
		//{
		//	buffer.ReadDouble(&desEndPos.face_offset_norm.x);
		//	buffer.ReadDouble(&desEndPos.face_offset_norm.y);
		//	buffer.ReadDouble(&desEndPos.face_offset_norm.z);
		//}
		//buffer.ReadDouble(&desEndPos.fEccentricDist);
		//buffer.ReadDouble(&desEndPos.fNormOffset);
		//buffer.ReadDouble(&desEndPos.fTransverseOffset);
		//if( version==0||
		//	(doc_type==1&&version>=4010100)||	//TMA(V4.1.1.0)
		//	(doc_type==2&&version>=1010100)||	//LMA(V1.1.1.0)
		//	(doc_type==3&&version>=2000100)||	//TSA(V2.0.1.0)
		//	(doc_type==4&&version>=1020100)||	//TDA(V1.2.1.0)
		//	(doc_type==5&&version>=1020100))	//TMA(V1.2.1.0)
		//{
		//	buffer.ReadInteger(&hAttachTube);
		//	if(hAttachTube>0x20)
		//	{
		//		desDatumPoint.FromBuffer(buffer);
		//		buffer.ReadDouble(&fLen);
		//	}
		//}
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
void CLDSLineSlot::IndependentShadowPropToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
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

	//buffer.WriteDouble(start_oddment);
	//buffer.WriteInteger(desStartOdd.m_iOddCalStyle);
	//buffer.WriteInteger(desStartOdd.m_hRefPart1);
	//buffer.WriteInteger(desStartOdd.m_hRefPart2);
	//buffer.WriteDouble(desStartOdd.m_fCollideDist);
	//buffer.WriteDouble(end_oddment); 
	//buffer.WriteInteger(desEndOdd.m_iOddCalStyle);
	//buffer.WriteInteger(desEndOdd.m_hRefPart1);
	//buffer.WriteInteger(desEndOdd.m_hRefPart2);
	//buffer.WriteDouble(desEndOdd.m_fCollideDist);
	//始端定位参数
	//buffer.WriteInteger(desStartPos.endPosType);
	//buffer.WriteByte(desStartPos.datum_pos_style);	//1000085
	//buffer.WriteInteger(desStartPos.hDatumPart);
	//buffer.WriteInteger(desStartPos.hDatumStartPart);
	//buffer.WriteInteger(desStartPos.hDatumEndPart);
	//buffer.WriteInteger(desStartPos.bDatumEndStartEnd);
	//buffer.WriteInteger(desStartPos.bDatumStartStartEnd);
	//buffer.WriteBooleanThin(desStartPos.bUseFaceOffsetNorm);
	//if(desStartPos.bUseFaceOffsetNorm)
	//{
	//	buffer.WriteDouble(desStartPos.face_offset_norm.x);
	//	buffer.WriteDouble(desStartPos.face_offset_norm.y);
	//	buffer.WriteDouble(desStartPos.face_offset_norm.z);
	//}
	//buffer.WriteDouble(desStartPos.fEccentricDist);
	//buffer.WriteDouble(desStartPos.fNormOffset);
	//buffer.WriteDouble(desStartPos.fTransverseOffset);
	//终端定位参数
	//buffer.WriteInteger(desEndPos.endPosType);
	//buffer.WriteByte(desEndPos.datum_pos_style);	//1000085
	//buffer.WriteInteger(desEndPos.hDatumPart);
	//buffer.WriteInteger(desEndPos.hDatumStartPart);
	//buffer.WriteInteger(desEndPos.hDatumEndPart);
	//buffer.WriteInteger(desEndPos.bDatumEndStartEnd);
	//buffer.WriteInteger(desEndPos.bDatumStartStartEnd);
	//buffer.WriteBooleanThin(desEndPos.bUseFaceOffsetNorm);
	//if(desEndPos.bUseFaceOffsetNorm)
	//{
	//	buffer.WriteDouble(desEndPos.face_offset_norm.x);
	//	buffer.WriteDouble(desEndPos.face_offset_norm.y);
	//	buffer.WriteDouble(desEndPos.face_offset_norm.z);
	//}
	//buffer.WriteDouble(desEndPos.fEccentricDist);
	//buffer.WriteDouble(desEndPos.fNormOffset);
	//buffer.WriteDouble(desEndPos.fTransverseOffset);
	//buffer.WriteInteger(hAttachTube);
	//if(hAttachTube>0x20)
	//{
	//	desDatumPoint.ToBuffer(buffer);
	//	buffer.WriteDouble(fLen);
	//}
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
void CLDSLineSlot::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//保存影射构件参数和构件从属句柄 wht 12-09-14
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
	buffer.WriteDouble(size_thick);
	buffer.WriteDouble(size_height);
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
	//
	buffer.WriteInteger(hAttachTube);		//根据依附钢管计算槽钢法线偏移量
	if(hAttachTube>0x20)
	{
		desDatumPoint.ToBuffer(buffer,version,doc_type);		//根据desDatumPoint计算槽钢中心位置
		if(  version==0||
			(doc_type==1&&version>=5000300)||	//TMA V5.0.3.0
			(doc_type==2&&version>=2000300)||	//LMA V2.0.3.0
			(doc_type==3&&version>=2010300)||	//TSA V2.1.3.0
			(doc_type==4&&version>=1030300)||	//LDS V1.3.3.0
			(doc_type==5&&version>=1030300))	//TDA V1.3.3.0
			buffer.WritePoint(desDatumPoint.Position());
		buffer.WriteDouble(fLen);			//根据输入长度及基准点位置计算槽钢始终端位置
	}
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

	//槽钢螺栓区
	pLsRefList->ToBuffer(buffer,version,doc_type);
}
void CLDSLineSlot::ToXmlFile(FILE* fp,DWORD schema)
{
	char sText[500]="";
	fprintf(fp,"<槽钢信息 dwPermission=\"%d\" dwStartPermission=\"%d\" dwEndPermission=\"%d\">\n",dwPermission,dwStartPermission,dwEndPermission);
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
	sprintf(sText,"%f",size_wide);
	SimplifiedNumString(sText);
	fprintf(fp,"<宽度 wide=\"%s\"/>\n",sText);
	sprintf(sText,"%f",size_thick);
	SimplifiedNumString(sText);
	fprintf(fp,"<厚度 thick=\"%s\"/>\n",sText);
	sprintf(sText,"%f",size_height);
	SimplifiedNumString(sText);
	fprintf(fp,"<高度 height=\"%s\"/>\n",sText);
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
	fprintf(fp,"</槽钢信息>\n");
}
void CLDSLineSlot::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/,bool bAddSubLevelRelaObj/*=true*/)
{
	if(_console==NULL||m_pModel==NULL||!Lock())	//已调用过)
		return;
	_console->AddToObjSetBuf(this);
	if(!bAddRelaObj)
		return;
	bAddRelaObj=bAddSubLevelRelaObj;
	if(pStart)
		pStart->AddToConsoleObjBuf(bAddRelaObj);
	if(pEnd)
		pEnd->AddToConsoleObjBuf(bAddRelaObj);

	for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		pLsRef->GetLsPtr()->AddToConsoleObjBuf(bAddRelaObj);
}
BYTE CLDSLineSlot::FuncType()
{
	//人为自动设置
	if(m_cFuncType>0)
		return m_cFuncType;
	//智能判断
	if(pStart==NULL&&pEnd==NULL&&GetLsCount()==1)
		m_cFuncType=CLDSPart::FUNC_FOOTNAIL;
	return m_cFuncType;
}

void CLDSLineSlot::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	//读取影射构件参数和构件从属句柄 wht 12-09-14
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
	buffer.ReadInteger(&_hPartWeldParent);
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
	buffer.ReadDouble(&size.thick);
	buffer.ReadDouble(&size.height);
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
	//if(SuperiorTower()->hashObjs.GetHashTableSize()>0)
	//	SuperiorTower()->hashObjs.GetValueAt(dw,(CLDSObject*&)pStart);
	//else
		pStart=BelongModel()->FromNodeHandle(dw);	// 起点句柄
	buffer.ReadInteger(&dw);
	//if(SuperiorTower()->hashObjs.GetHashTableSize()>0)
	//	SuperiorTower()->hashObjs.GetValueAt(dw,(CLDSObject*&)pEnd);
	//else
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
	//依附与钢管上的槽钢定位参数 wht 13-10-17
	if(	version==0||
		(doc_type==1&&version>=4010100)||	//TMA(V4.1.1.0)
		(doc_type==2&&version>=1010100)||	//LMA(V1.1.1.0)
		(doc_type==3&&version>=2000100)||	//TSA(V2.0.1.0)
		(doc_type==4&&version>=1020100)||	//TDA(V1.2.1.0)
		(doc_type==5&&version>=1020100))	//TMA(V1.2.1.0)
	{
		buffer.ReadInteger(&hAttachTube);
		if(hAttachTube>0x20)
		{
			desDatumPoint.FromBuffer(buffer,version,doc_type);
			if(  version==0||
				(doc_type==1&&version>=5000300)||	//TMA V5.0.3.0
				(doc_type==2&&version>=2000300)||	//LMA V2.0.3.0
				(doc_type==3&&version>=2010300)||	//TSA V2.1.3.0
				(doc_type==4&&version>=1030300)||	//LDS V1.3.3.0
				(doc_type==5&&version>=1030300))	//TDA V1.3.3.0
			{
				GEPOINT pos;
				buffer.ReadPoint(pos);
				desDatumPoint.SetPosition(pos);
			}
			buffer.ReadDouble(&fLen);
		}
	}
	else 
		hAttachTube=0;

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
	//槽钢螺栓区
	pLsRefList->FromBuffer(buffer,BelongModel(),version,doc_type);
	SetModified();
}
//自协同中心通过管道读取数据
void CLDSLineSlot::FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord)
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
		buffer.ReadInteger(&_hPartWeldParent);
		buffer.ReadBooleanThin(&m_bDisplayHole);//显示螺栓孔及切割面
		buffer.ReadString(sNotes,51);
		buffer.ReadDouble(&size.wide);	
		buffer.ReadDouble(&size.thick);
		buffer.ReadDouble(&size.height);
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
		//槽钢螺栓区
		pLsRefList->FromBuffer(buffer,m_pModel);
	}
	if(dwStartPermission&dwUpdateWord)
	{
		buffer.ReadInteger(&dw);
		//if(SuperiorTower()->hashObjs.GetHashTableSize()>0)
		//	SuperiorTower()->hashObjs.GetValueAt(dw,(CLDSObject*&)pStart);
		//else
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
		buffer.ReadInteger(&hAttachTube);
		if(hAttachTube>0x20)
		{
			desDatumPoint.FromBuffer(buffer);
			GEPOINT pos;
			buffer.ReadPoint(pos);
			desDatumPoint.SetPosition(pos);
			buffer.ReadDouble(&fLen);
		}		
		//设计参数
		buffer.ReadInteger(&m_bLockStartPos);
	}
	if(dwEndPermission&dwUpdateWord)
	{
		buffer.ReadInteger(&dw);
		//if(SuperiorTower()->hashObjs.GetHashTableSize()>0)
		//	SuperiorTower()->hashObjs.GetValueAt(dw,(CLDSObject*&)pEnd);
		//else
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
void CLDSLineSlot::ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord)
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
		buffer.WriteDouble(size_thick);
		buffer.WriteDouble(size_height);
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
		//槽钢螺栓区
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
		buffer.WriteInteger(hAttachTube);	//根据依附钢管计算槽钢法线偏移量
		if(hAttachTube>0x20)
		{
			desDatumPoint.ToBuffer(buffer);	//根据desDatumPoint计算槽钢中心位置
			buffer.WritePoint(desDatumPoint.Position());
			buffer.WriteDouble(fLen);		//根据输入长度及基准点位置计算槽钢始终端位置
		}
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
int CLDSLineSlot::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
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
	{
		if(isalpha(m_cQualityLevel))
			sprintf(sText,"%C",m_cQualityLevel);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Default");
#else
			strcpy(sText,"默认");
#endif
	}
	else if(GetPropID("specification")==id)
	{
		if(strlen(spec)<=0)	
			sprintf(sText,"%.f",size_height*0.1);
		else
			sprintf(sText,"%s",spec);
	}
	else if(GetPropID("iSeg")==id)
		strcpy(sText,(char*)iSeg.ToString());
	else if(GetPropID("sPartNo")==id)
		sprintf(sText,"%s",(char*)GetPartNo());
	else if(GetPropID("cfgword")==id)
		//return MakeWordToString(cfgword,valueStr);
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
		CLDSLineSlot *pMotherLineSlot=(CLDSLineSlot*)MotherObject(false);
		if(pMotherLineSlot!=this)
			sprintf(sText,"0X%X",pMotherLineSlot->handle);
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
		strcpy(sText,GetBlockIdName());
	else if(GetPropID("m_uStatMatCoef")==id)
		sprintf(sText,"%d",m_uStatMatCoef);
	else if(GetPropID("m_hPartWeldParent")==id)
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
	else if(GetPropID("height")==id)
	{
		sprintf(sText,"%f",size_height);
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
	else if(GetPropID("hAttachTube")==id)
		sprintf(sText,"0X%X",hAttachTube);
	else if(GetPropID("hWorkNormRefPart")==id)
		sprintf(sText,"0X%X",hWorkNormRefPart);
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
	else if(GetPropID("m_iNormCalStyle")==id)
	{
		if(m_iNormCalStyle == 0)
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sText,"0.Specify");
		else if(m_iNormCalStyle == 1)
			sprintf(sText,"1.Reference Rod");
		else //if(m_iNormCalStyle == 2)
			sprintf(sText,"2.Horizontal Pedestal");
#else
			sprintf(sText,"0.直接指定");
		else if(m_iNormCalStyle == 1)
			sprintf(sText,"1.参照杆件");
		else //if(m_iNormCalStyle == 2)
			sprintf(sText,"2.水平底座");
#endif
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
	else if(GetPropID("hStart")==id)
	{
		if(pStart)
			sprintf(sText,"0X%X",pStart->handle);
		else
			strcpy(sText,"0X0");
	}
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
	else if(GetPropID("hEnd")==id)
	{
		if(pEnd)
			sprintf(sText,"0X%X",pEnd->handle);
		else
			strcpy(sText,"0X0");
	}
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
			strcpy(sText,"Blot Line Intersection");
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
		if(CalLenCoef.hHorizPole>0x20)
			sprintf(sText,"0X%X",CalLenCoef.hHorizPole);
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
		sprintf(sText,"%d",connectStart.wnConnBoltN);
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
			strcpy(sText,"Auto Justify");
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
		sprintf(sText,"%d",connectEnd.wnConnBoltN);
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
	else if (GetPropID("desDatumPoint")==id)
		strcpy(sText,CXhChar100(desDatumPoint.Position()));
	else if (GetPropID("desDatumPoint.x")==id)
	{
		sprintf(sText,"%f",desDatumPoint.Position().x);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("desDatumPoint.y")==id)
	{
		sprintf(sText,"%f",desDatumPoint.Position().y);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("desDatumPoint.z")==id)
	{
		sprintf(sText,"%f",desDatumPoint.Position().z);
		SimplifiedNumString(sText);
	}
	else if(desDatumPoint.datum_pos_style==11&&GetPropID("desDatumPoint.param.hOffsetDatumNode")==id)
		sprintf(sText,"0X%X",abs(desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode));
	else if(desDatumPoint.datum_pos_style==11&&GetPropID("desDatumPoint.param.lenOffsetType")==id)
	{
		strcpy(sText,"0.始->终");
		if(desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode<0)
			strcpy(sText,"1.终->始");
	}
	else if(desDatumPoint.datum_pos_style==11&&GetPropID("desDatumPoint.param.lenOffset")==id)
		sprintf(sText,"%g",desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.len_offset);
	else if(desDatumPoint.datum_pos_style==11&&GetPropID("desDatumPoint.param.RayVecStyle")==id)
	{
		if(desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style==0)
			strcpy(sText,"0.直接指定");
		else if(desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style==1)
			strcpy(sText,"1.指定基准边");
		else if(desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style==2)
			strcpy(sText,"2.自基准边旋转");
		else if(desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style==3)
			strcpy(sText,"3.自焊道线旋转");
		else if(desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style==4)
			strcpy(sText,"4.水平方向");
	}
	else if(desDatumPoint.datum_pos_style==11&&GetPropID("desDatumPoint.param.RayVecStdStyle")==id)
	{
		sprintf(sText,"%s",CStdVector::GetVecTypeString(desDatumPoint.des_para.COLUMN_INTERS.RayVector.ray_vec));
	}
	else if(desDatumPoint.datum_pos_style==11&&GetPropID("desDatumPoint.param.RayVec.x")==id)
		sprintf(sText,"%g",desDatumPoint.des_para.COLUMN_INTERS.RayVector.ray_vec.x);
	else if(desDatumPoint.datum_pos_style==11&&GetPropID("desDatumPoint.param.RayVec.y")==id)
		sprintf(sText,"%g",desDatumPoint.des_para.COLUMN_INTERS.RayVector.ray_vec.y);
	else if(desDatumPoint.datum_pos_style==11&&GetPropID("desDatumPoint.param.RayVec.z")==id)
		sprintf(sText,"%g",desDatumPoint.des_para.COLUMN_INTERS.RayVector.ray_vec.z);
	else if(desDatumPoint.datum_pos_style==11&&GetPropID("desDatumPoint.param.RayVecRotateAngle")==id)
		sprintf(sText,"%g",desDatumPoint.des_para.COLUMN_INTERS.ray_angle);
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
long CLDSLineSlot::GetPropID(char* propStr)
{
	PROPLIST_ITEM *pItem = CLDSLineSlot::propHashtable.GetValue(propStr);
	if(pItem!=NULL)
		return pItem->id;
	else
	{
		/*CString prompt="没找到名为\"";
		prompt+=propStr;
		prompt+="\"的属性ID！";
		AfxMessageBox(prompt);*/
		return 0;
	}
}

BOOL CLDSLineSlot::IsPropEqual(HASHOBJSET &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSObject *pFirstObj = selectObjs.GetFirst();
	CLDSLineSlot *pLineSlot=NULL, *pFirstLineSlot = (CLDSLineSlot*)pFirstObj;
	pFirstLineSlot->GetPropValueStr(idProp,valueStr);
	for(pLineSlot=(CLDSLineSlot*)selectObjs.GetNext();pLineSlot;pLineSlot=(CLDSLineSlot*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pLineSlot->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

int CLDSLineSlot::GetPropStatus(long id)
{	
	DWORD dwStatus;
	if(CLDSLineSlot::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
}

BOOL CLDSLineSlot::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSLineSlot::propStatusHashtable.SetValueAt(id,dwStatus);
}

BOOL CLDSLineSlot::GetObjPropReadOnlyDefaultState(long id)
{
	return GetObjPropReadOnlyDefaultState(id,ShadowMode());
}
BOOL CLDSLineSlot::GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode)
{
	if (shadow_mode==CLDSObject::SHADOWMODE_COMMON)
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
		else
			return FALSE;
	}
}

void CLDSLineSlot::CalWorkPlaneNorm()
{
	//计算钢管槽钢工作面法线方向
	CLDSLineTube *pLineTube=(CLDSLineTube*)BelongModel()->FromPartHandle(hAttachTube,CLS_LINETUBE);
	if(pLineTube)
	{	
		CLDSArcLift *pLift=NULL;
		if(m_hArcLift>0x20)
			pLift=BelongModel()->FromArcLiftHandle(m_hArcLift);
		desDatumPoint.UpdatePos(BelongModel(),pLift);
		f3dPoint datumPos=desDatumPoint.Position();
		f3dPoint tube_vec=pLineTube->End()-pLineTube->Start();
		normalize(tube_vec);
		f3dPoint near_norm=datumPos-pLineTube->Start();
		if(m_iNormCalStyle==2)
			workPlaneNorm=f3dPoint(0,0,1)^tube_vec;
		else
			workPlaneNorm=(tube_vec^near_norm)^tube_vec;
		normalize(workPlaneNorm);
		if(workPlaneNorm*near_norm<0)
			workPlaneNorm*=-1;
		return;
	}
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

UCS_STRU CLDSLineSlot::BuildUCS()
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

int CLDSLineSlot::GetSpecification(char *slot_spec,BOOL bIncMat/*=TRUE*/)
{	//槽钢规格 wht 10-08-26
	char steelmark[20]="";
	char tmp_spec[200];
	if(bIncMat&&cMaterial!=CSteelMatLibrary::Q235BriefMark())//'S')
	{
		QuerySteelMatMark(cMaterial,steelmark);
		if(strlen(spec)>0)
			sprintf(tmp_spec,"%s-[%s",steelmark,spec);
		else 
			sprintf(tmp_spec,"%s-[%.1f",steelmark,size_height*0.1);
	}
	else
	{
		if(strlen(spec)>0)
			sprintf(tmp_spec,"[%s",spec);
		else
			sprintf(tmp_spec,"[%.1f",size_height*0.1);
	}
	SimplifiedNumString(tmp_spec);
	if(slot_spec)
		strcpy(slot_spec,tmp_spec);
	return strlen(tmp_spec);
}

double CLDSLineSlot::GetWeight(BOOL bSolidLength/*=TRUE*/)
{
	/*BIAN_PARA_TYPE* pFlatPara=FindBianType(w,t);
	if(pFlatPara)
		return pFlatPara->theroy_weight*GetLength()*0.001;
	else*/
	double length;
	if(bSolidLength||pStart==NULL||pEnd==NULL)
		length=GetLength();
	else
		length=DISTANCE(pStart->Position(false),pEnd->Position(false));
	return CalSteelPlankWei((size_height-2*size_thick+2*size_wide)*size_thick*length);
}

bool CLDSLineSlot::IsEqual(CLDSLineSlot* pMirSlot,double epsilon/*=1.0*/,char *error_info/*=NULL*/,
						   int *pErrorInfo/*=NULL*/,DWORD *pChkStyleFlag/*=NULL*/)
{
	if(pChkStyleFlag==NULL||(*pChkStyleFlag)&CHKPARTNO_RODINFO)
	{
		bool bRet=true;
		if(fabs(GetLength()-pMirSlot->GetLength())>epsilon)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s} Different length",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}长度不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(size_wide-pMirSlot->GetWidth()!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s} Different width",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}宽度不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(size_height-pMirSlot->GetHeight()!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s} Different hight",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}槽高不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(size_thick-pMirSlot->GetThick()!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s} Different thickness",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}厚度不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(cMaterial!=pMirSlot->cMaterial)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s} Different material",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}材质不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		if(!bRet)
		{
			if(pErrorInfo)
				*pErrorInfo=CHKPARTNO_RODINFO;
			return false;
		}
	}
	if(pChkStyleFlag==NULL||(*pChkStyleFlag)&CHKPARTNO_RODBOLTINFO)
	{
		//比较螺栓信息
		BuildUCS();
		pMirSlot->BuildUCS();;
		UCS_STRU tem_ucs = ucs;
		UCS_STRU tem_vice_ucs = pMirSlot->ucs;
		Sub_Pnt(tem_ucs.origin,ucs.origin,ucs.axis_z*start_oddment);
		Sub_Pnt(tem_vice_ucs.origin,pMirSlot->ucs.origin,pMirSlot->ucs.axis_z*pMirSlot->start_oddment);
		char sError[200]="";
		if(!IsLsRefListEqual( pLsRefList,tem_ucs,
			pMirSlot->pLsRefList,tem_vice_ucs,epsilon,sError,NULL))
		{
			if(error_info)
				sprintf(error_info,"{%s}%s",(char*)sPartNo,sError);
			if(pErrorInfo)
				*pErrorInfo=CHKPARTNO_RODBOLTINFO;
			return false;
		}
	}
	return true;
}
BOOL CLDSLineSlot::CalTubeSlotPos(CLDSArcLift *pLift/*=NULL*/)
{
	if(hAttachTube<=0)
		return FALSE;	//槽钢脚钉较多时会由于hAttachTube=0耗费大量的到位计算时间 wjh-2014.9.20
	CLDSLineTube *pLineTube=(CLDSLineTube*)BelongModel()->FromPartHandle(hAttachTube,CLS_LINETUBE);
	if(pLineTube)
	{	
		SetModified();
		//1.计算基准点位置
		desDatumPoint.UpdatePos(BelongModel(),pLift);
		f3dPoint datumPos=desDatumPoint.Position();;
		//2.计算法向偏移量
		if(size_height<=pLineTube->GetDiameter())
		{
			double tube_r=pLineTube->GetDiameter()*0.5;
			double height=size_height-2*size_thick;
			double fNormOffset=size_wide-(tube_r-sqrt(tube_r*tube_r-0.5*height*0.5*height));
			desStartPos.fNormOffset=desEndPos.fNormOffset=fNormOffset;
		}
		CalWorkPlaneNorm();
		//3.计算槽钢位置
		f3dPoint tubeVec=pLineTube->End()-pLineTube->Start();
		normalize(tubeVec);
		if(!workPlaneNorm.IsZero())
		{
			ucs.axis_y=-workPlaneNorm;
			ucs.axis_z=-tubeVec;
			ucs.axis_x=ucs.axis_y^ucs.axis_z;
		}
		//管塔脚钉布置中，默认是与基准管终->始方向为槽钢底座正向，为保持一致此处也如此处理 wjh-2019.7.29
		SetStart(datumPos+tubeVec*fLen*0.5+workPlaneNorm*desStartPos.fNormOffset);
		SetEnd(datumPos-tubeVec*fLen*0.5+workPlaneNorm*desEndPos.fNormOffset);
		return TRUE;
	}
	return FALSE;
}

void CLDSLineSlot::CalStartPos(CLDSArcLift *pLift/*=NULL*/)
{
	if(!LockStart())
		return;	//已到位
	if(CalTubeSlotPos(pLift))
		return;
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
						startPos = pDatumLineAngle->GetDatumPosBerByFaceOffset(pStart,desStartPos.face_offset_norm);
					else if(desStartPos.datum_pos_style==4)	//以父角钢X肢为基准平推
						startPos = pDatumLineAngle->GetDatumPosWingXByFaceOffset(pStart,desStartPos.face_offset_norm);
					else //if(desStartPos.datum_pos_style==5)//以父角钢Y肢为基准平推
						startPos = pDatumLineAngle->GetDatumPosWingYByFaceOffset(pStart,desStartPos.face_offset_norm);
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
		CLDSLineSlot *pStartDatumSlot=(CLDSLineSlot*)BelongModel()->FromPartHandle(desStartPos.hDatumStartPart,CLS_LINESLOT);
		CLDSLineSlot *pEndDatumSlot=(CLDSLineSlot*)BelongModel()->FromPartHandle(desStartPos.hDatumEndPart,CLS_LINESLOT);
		if(pStartDatumSlot==NULL||pEndDatumSlot==NULL)
			return;
		f3dPoint datumPosStart,datumPosEnd,pos;
		if(desStartPos.bDatumStartStartEnd)
			datumPosStart=pStartDatumSlot->Start();
		else
			datumPosStart=pStartDatumSlot->End();
		if(desStartPos.bDatumEndStartEnd)
			datumPosEnd=pEndDatumSlot->Start();
		else
			datumPosEnd=pEndDatumSlot->End();
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
void CLDSLineSlot::CalEndPos(CLDSArcLift *pLift/*=NULL*/)
{
	if(!LockEnd())
		return;	//已到位
	if(CalTubeSlotPos(pLift))
		return;
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
						if(pLift->m_cRotAxis=='Y'&&pEnd->Position(false).x<0)
							bInRightSide=false;
						else if(pLift->m_cRotAxis=='X'&&pEnd->Position(false).y>0)
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
		CLDSLineSlot *pStartDatumSlot=(CLDSLineSlot*)BelongModel()->FromPartHandle(desEndPos.hDatumStartPart,CLS_LINESLOT);
		CLDSLineSlot *pEndDatumSlot=(CLDSLineSlot*)BelongModel()->FromPartHandle(desEndPos.hDatumEndPart,CLS_LINESLOT);
		if(pStartDatumSlot==NULL||pEndDatumSlot==NULL)
			return;
		if(pStartDatumSlot!=this)
			pStartDatumSlot->CalPosition();
		if(pEndDatumSlot!=this)
			pEndDatumSlot->CalPosition();
		f3dPoint datumPosStart,datumPosEnd,pos;
		if(desEndPos.bDatumStartStartEnd)
			datumPosStart=pStartDatumSlot->Start();
		else
			datumPosStart=pStartDatumSlot->End();
		if(desEndPos.bDatumEndStartEnd)
			datumPosEnd=pEndDatumSlot->Start();
		else
			datumPosEnd=pEndDatumSlot->End();
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

void CLDSLineSlot::CalPosition(bool bIgnoreRepeatFlag/*=FALSE*/)
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
	if(hAttachTube<=0)	//hAttachTube>0时，为脚钉底座型槽钢已在起始端进行了相应计算 wjh-201510.11
		CalEndPos(pLift);
}

void CLDSLineSlot::CopyProperty(CLDSDbObject* pObj)
{
	if(pObj->GetClassTypeId()!=CLS_LINESLOT)
		return;
	CLDSLineSlot *pSlot=(CLDSLineSlot*)pObj;
	size_thick = pSlot->GetThick();
	size_height = pSlot->GetHeight();
	size_wide = pSlot->GetWidth();
	if(!IsStartPosLocked())
		desStartPos.endPosType=pSlot->desStartPos.endPosType;
	if(!IsEndPosLocked())
		desEndPos.endPosType=pSlot->desEndPos.endPosType;
    SetLayer(pSlot->layer());		
	iSeg = pSlot->iSeg;
	strcpy(sPartNo,pSlot->sPartNo);
	m_uStatMatCoef = pSlot->m_uStatMatCoef;
	is_data_modified = TRUE;
	cMaterial = pSlot->cMaterial;
	cfgword = pSlot->cfgword;
	strcpy(spec,pSlot->spec);	//拷贝规格 wht 10-08-26
}

BOOL CLDSLineSlot::Create3dSolidModel(BOOL bDisplayHole,double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	if(CreateShadowPart3dSolidModel(bDisplayHole,scale_of_user2scr,sample_len,smoothness))
		return TRUE;

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
		int i=0;
		f3dPoint vertex_arr[16];
		BuildUCS();
		double length=DISTANCE(Start(),End());
		is_data_modified = FALSE;
		vertex_arr[0].x = -size_height/2;
		vertex_arr[0].y = size_wide;
		vertex_arr[0].z = -startOdd();

		vertex_arr[1].x = -size_height/2;
		vertex_arr[1].y = 0;
		vertex_arr[1].z = -startOdd();

		vertex_arr[2].x = size_height/2;
		vertex_arr[2].y = 0;
		vertex_arr[2].z = -startOdd();

		vertex_arr[3].x = size_height/2;
		vertex_arr[3].y = size_wide;
		vertex_arr[3].z = -startOdd();

		vertex_arr[4].x = size_height/2-size_thick;
		vertex_arr[4].y = size_wide;
		vertex_arr[4].z = -startOdd();

		vertex_arr[5].x = size_height/2-size_thick;
		vertex_arr[5].y = size_thick;
		vertex_arr[5].z = -startOdd();

		vertex_arr[6].x = -size_height/2+size_thick;
		vertex_arr[6].y = size_thick;
		vertex_arr[6].z = -startOdd();

		vertex_arr[7].x = -size_height/2+size_thick;
		vertex_arr[7].y = size_wide;
		vertex_arr[7].z = -startOdd();
		for(i=0;i<8;i++)
			vertex_arr[i+8].Set(vertex_arr[i].x,vertex_arr[i].y,length+endOdd());

		for(i=0;i<16;i++)
			pBody->vertex.append(vertex_arr[i]);
		//初始化链表
		CCreateFace createFace;
		createFace.InitVertexList(pBody);

		long liColorItemType = 3;
		if (pStart == NULL || pEnd == NULL)
			liColorItemType = 'S';
		COLORREF crSlot = GetColor(CLS_LINESLOT, liColorItemType,cMaterial);
		if(m_bUserSpecColor)	//用户指定构件颜色
			crSlot=crMaterial;
		//1.槽钢外侧正面生成
		f3dPolyFace *pTopFace = pBody->faceList.append();
		pTopFace->material = crSlot;
		createFace.NewOutterEdgeLine(pTopFace,2,1);
		createFace.NewOutterEdgeLine(pTopFace,10);
		createFace.NewOutterEdgeLine(pTopFace,9);
		createFace.NewOutterEdgeLine(pTopFace,1);

		//2.槽钢右(X+)侧外表面生成
		f3dPolyFace *pRightOutsideFace = pBody->faceList.append();
		pRightOutsideFace->material = crSlot;
		createFace.NewOutterEdgeLine(pRightOutsideFace,3,2);
		createFace.NewOutterEdgeLine(pRightOutsideFace,11);
		createFace.NewOutterEdgeLine(pRightOutsideFace,10);
		createFace.NewOutterEdgeLine(pRightOutsideFace,2);

		//3.槽钢右(X+)侧端面生成
		f3dPolyFace *pFace = pBody->faceList.append();
		pFace->material = crSlot;
		createFace.NewOutterEdgeLine(pFace,4,3);
		createFace.NewOutterEdgeLine(pFace,12);
		createFace.NewOutterEdgeLine(pFace,11);
		createFace.NewOutterEdgeLine(pFace,3);

		//4.槽钢右(X+)侧内表面生成
		f3dPolyFace *pRightInsideFace = pBody->faceList.append();
		pRightInsideFace->material = crSlot;
		createFace.NewOutterEdgeLine(pRightInsideFace,5,4);
		createFace.NewOutterEdgeLine(pRightInsideFace,13);
		createFace.NewOutterEdgeLine(pRightInsideFace,12);
		createFace.NewOutterEdgeLine(pRightInsideFace,4);
		//5.槽钢内侧正面生成
		f3dPolyFace *pBottomFace = pBody->faceList.append();
		pBottomFace->material = crSlot;
		createFace.NewOutterEdgeLine(pBottomFace,6,5);
		createFace.NewOutterEdgeLine(pBottomFace,14);
		createFace.NewOutterEdgeLine(pBottomFace,13);
		createFace.NewOutterEdgeLine(pBottomFace,5);
		//6.槽钢左(X-)侧内表面生成
		f3dPolyFace *pLeftInsideFace = pBody->faceList.append();
		pLeftInsideFace->material = crSlot;
		createFace.NewOutterEdgeLine(pLeftInsideFace,7,6);
		createFace.NewOutterEdgeLine(pLeftInsideFace,15);
		createFace.NewOutterEdgeLine(pLeftInsideFace,14);
		createFace.NewOutterEdgeLine(pLeftInsideFace,6);
		//7.槽钢左(X-)侧端面生成
		pFace = pBody->faceList.append();
		pFace->material = crSlot;
		createFace.NewOutterEdgeLine(pFace,0,7);
		createFace.NewOutterEdgeLine(pFace,8);
		createFace.NewOutterEdgeLine(pFace,15);
		createFace.NewOutterEdgeLine(pFace,7);
		//8.槽钢左(X-)侧外表面生成
		f3dPolyFace *pLeftOutsideFace = pBody->faceList.append();
		pLeftOutsideFace->material = crSlot;
		createFace.NewOutterEdgeLine(pLeftOutsideFace,1,0);
		createFace.NewOutterEdgeLine(pLeftOutsideFace,9);
		createFace.NewOutterEdgeLine(pLeftOutsideFace,8);
		createFace.NewOutterEdgeLine(pLeftOutsideFace,0);
		//9.槽钢底面生成
		pFace = pBody->faceList.append();
		pFace->material = crSlot;
		createFace.NewOutterEdgeLine(pFace,7,0);
		createFace.NewOutterEdgeLine(pFace,6);
		createFace.NewOutterEdgeLine(pFace,5);
		createFace.NewOutterEdgeLine(pFace,4);
		createFace.NewOutterEdgeLine(pFace,3);
		createFace.NewOutterEdgeLine(pFace,2);
		createFace.NewOutterEdgeLine(pFace,1);
		createFace.NewOutterEdgeLine(pFace,0);
		//10.槽钢顶面生成
		pFace = pBody->faceList.append();
		pFace->material = crSlot;
		createFace.NewOutterEdgeLine(pFace,9,8);
		createFace.NewOutterEdgeLine(pFace,10);
		createFace.NewOutterEdgeLine(pFace,11);
		createFace.NewOutterEdgeLine(pFace,12);
		createFace.NewOutterEdgeLine(pFace,13);
		createFace.NewOutterEdgeLine(pFace,14);
		createFace.NewOutterEdgeLine(pFace,15);
		createFace.NewOutterEdgeLine(pFace,8);
		for(i=0;i<16;i++)
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
			holeMaker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pTopFace,pBottomFace,pBody,crLsHole);
		}
		pSolidBody->ConvertFrom(pBody);
	}
	return TRUE;
}

// 扁钢
CLDSLineFlat::CLDSLineFlat(void)
{
	size_idClassType=m_nClassTypeId = CLS_LINEFLAT;
	//strcpy(m_sClassName,"CLDSLineFlat");
	size_wide=50;
	size_thick=8;
	_material = 'S';
	pSolidBody = NULL;
	is_visible=TRUE;
	is_data_modified = TRUE;
	m_uStatMatCoef = 1;
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
	hWorkNormRefPart = 0;
	m_iNormCalStyle = 1;				//默认为通过杆件定位
	memset(&desStartPos,0,sizeof(CTubeEndPosPara));
	memset(&desEndPos,0,sizeof(CTubeEndPosPara));
}

CLDSLineFlat::~CLDSLineFlat()
{
	if(pSolidBody)
	{
		delete pSolidBody;
		pSolidBody=NULL;
	}
}
void CLDSLineFlat::IndependentShadowPropFromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	long h=0;
	buffer.ReadInteger(&h);
	pStart=BelongModel()->FromNodeHandle(h);
	buffer.ReadInteger(&h);
	pEnd=BelongModel()->FromNodeHandle(h);
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
		CfgwordFromBuffer(cfgword,buffer,version,doc_type);
		BelongModel()->EvolveCfgword(cfgword,version,doc_type);
		//buffer.ReadString(sPartNo);	
		buffer.ReadString(layer(),4);
		buffer.ReadInteger(&m_uStatMatCoef);
		buffer.ReadInteger(&_hPartWeldParent);
		buffer.ReadInteger(&m_bLockStartPos);
		buffer.ReadInteger(&m_bLockEndPos);
		buffer.ReadString(sNotes,51);
		buffer.ReadWord(&m_iNormCalStyle);	//1000083
		buffer.ReadDouble(&nearWorkPlaneNorm.x);
		buffer.ReadDouble(&nearWorkPlaneNorm.y);
		buffer.ReadDouble(&nearWorkPlaneNorm.z);
		buffer.ReadInteger(&hWorkNormRefPart);
		buffer.ReadDouble(&workPlaneNorm.x);	
		buffer.ReadDouble(&workPlaneNorm.y);	
		buffer.ReadDouble(&workPlaneNorm.z);	
		f3dPoint start,end;
		buffer.ReadDouble(&start.x);
		buffer.ReadDouble(&start.y);
		buffer.ReadDouble(&start.z);
		buffer.ReadDouble(&end.x);	
		buffer.ReadDouble(&end.y);	
		buffer.ReadDouble(&end.z);	
		baseline.SetStart(start);
		baseline.SetEnd(end);
		m_xStartBeforeLift=start;	//赋默认值
		m_xEndBeforeLift  =end;		//赋默认值
		connectStart.FromBuffer(buffer,version,doc_type);
		connectEnd.FromBuffer(buffer,version,doc_type);
		
		buffer.ReadDouble(&start_oddment);
		buffer.ReadInteger(&desStartOdd.m_iOddCalStyle);
		buffer.ReadInteger(&desStartOdd.m_hRefPart1);
		buffer.ReadInteger(&desStartOdd.m_hRefPart2);
		buffer.ReadDouble(&desStartOdd.m_fCollideDist);
		buffer.ReadDouble(&end_oddment); 
		buffer.ReadInteger(&desEndOdd.m_iOddCalStyle);
		buffer.ReadInteger(&desEndOdd.m_hRefPart1);
		buffer.ReadInteger(&desEndOdd.m_hRefPart2);
		buffer.ReadDouble(&desEndOdd.m_fCollideDist);
		//始端定位参数
		//buffer.ReadInteger(&desStartPos.endPosType);
		//buffer.ReadByte(&desStartPos.datum_pos_style);
		//buffer.ReadInteger(&desStartPos.hDatumPart);
		//buffer.ReadInteger(&desStartPos.hDatumStartPart);
		//buffer.ReadInteger(&desStartPos.hDatumEndPart);
		//buffer.ReadInteger(&desStartPos.bDatumEndStartEnd);
		//buffer.ReadInteger(&desStartPos.bDatumStartStartEnd);
		//buffer.ReadBooleanThin(&desEndPos.bUseFaceOffsetNorm);
		//if(desEndPos.bUseFaceOffsetNorm)
		//{
		//	buffer.ReadDouble(&desEndPos.face_offset_norm.x);
		//	buffer.ReadDouble(&desEndPos.face_offset_norm.y);
		//	buffer.ReadDouble(&desEndPos.face_offset_norm.z);
		//}
		//buffer.ReadDouble(&desEndPos.fEccentricDist);
		//buffer.ReadDouble(&desEndPos.fNormOffset);
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
void CLDSLineFlat::IndependentShadowPropToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
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
	buffer.Write(cfgword.flag.word,24);
	//buffer.WriteString(sPartNo);	
	buffer.WriteString(layer());	
	buffer.WriteInteger(m_uStatMatCoef);
	buffer.WriteInteger(m_hPartWeldParent);
	buffer.WriteInteger(m_bLockStartPos);
	buffer.WriteInteger(m_bLockEndPos);
	buffer.WriteString(sNotes);	

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
	connectStart.ToBuffer(buffer);
	connectEnd.ToBuffer(buffer);
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
	//buffer.WriteInteger(desStartPos.endPosType);
	//buffer.WriteByte(desStartPos.datum_pos_style);	//1000085
	//buffer.WriteInteger(desStartPos.hDatumPart);
	//buffer.WriteInteger(desStartPos.hDatumStartPart);
	//buffer.WriteInteger(desStartPos.hDatumEndPart);
	//buffer.WriteInteger(desStartPos.bDatumEndStartEnd);
	//buffer.WriteInteger(desStartPos.bDatumStartStartEnd);
	//if(desStartPos.face_offset_norm.IsZero())
	//	desStartPos.bUseFaceOffsetNorm=false;
	//else
	//	desStartPos.bUseFaceOffsetNorm=true;
	//buffer.WriteBooleanThin(desStartPos.bUseFaceOffsetNorm);
	//if(desStartPos.bUseFaceOffsetNorm)
	//{
	//	buffer.WriteDouble(desStartPos.face_offset_norm.x);
	//	buffer.WriteDouble(desStartPos.face_offset_norm.y);
	//	buffer.WriteDouble(desStartPos.face_offset_norm.z);
	//}
	//buffer.WriteDouble(desStartPos.fEccentricDist);
	//buffer.WriteDouble(desStartPos.fNormOffset);
	////终端定位参数
	//buffer.WriteInteger(desEndPos.endPosType);
	//buffer.WriteByte(desEndPos.datum_pos_style);	//1000085
	//buffer.WriteInteger(desEndPos.hDatumPart);
	//buffer.WriteInteger(desEndPos.hDatumStartPart);
	//buffer.WriteInteger(desEndPos.hDatumEndPart);
	//buffer.WriteInteger(desEndPos.bDatumEndStartEnd);
	//buffer.WriteInteger(desEndPos.bDatumStartStartEnd);
	////平推面法线
	//if(desEndPos.face_offset_norm.IsZero())
	//	desEndPos.bUseFaceOffsetNorm=false;
	//else
	//	desEndPos.bUseFaceOffsetNorm=true;
	//buffer.WriteBooleanThin(desEndPos.bUseFaceOffsetNorm);
	//if(desEndPos.bUseFaceOffsetNorm)
	//{
	//	buffer.WriteDouble(desEndPos.face_offset_norm.x);
	//	buffer.WriteDouble(desEndPos.face_offset_norm.y);
	//	buffer.WriteDouble(desEndPos.face_offset_norm.z);
	//}
	//buffer.WriteDouble(desEndPos.fEccentricDist);
	//buffer.WriteDouble(desEndPos.fNormOffset);
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

void CLDSLineFlat::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//保存影射构件参数和构件从属句柄 wht 12-09-14
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
	buffer.WriteBooleanThin(m_bDisplayHole);//显示螺栓孔及切割面
	buffer.WriteInteger(m_bLockStartPos);
	buffer.WriteInteger(m_bLockEndPos);
	buffer.WriteString(sNotes);
	buffer.WriteDouble(size_wide);	
	buffer.WriteDouble(size_thick);
	buffer.WriteBooleanThin(m_bUserSpecColor);
	if(m_bUserSpecColor)
		buffer.WriteDword(crMaterial);
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
	connectStart.ToBuffer(buffer,version,doc_type);
	connectEnd.ToBuffer(buffer,version,doc_type);
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
		CLDSObject::Log2File()->Log("关联对象记录数出现错误!");
#endif

	//扁钢螺栓区
	pLsRefList->ToBuffer(buffer,version,doc_type);
}

void CLDSLineFlat::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/,bool bAddSubLevelRelaObj/*=true*/)
{
	if(_console==NULL||m_pModel==NULL||!Lock())	//已调用过)
		return;
	_console->AddToObjSetBuf(this);
	if(!bAddRelaObj)
		return;
	bAddRelaObj=bAddSubLevelRelaObj;
	if(pStart)
		pStart->AddToConsoleObjBuf(bAddRelaObj);
	if(pEnd)
		pEnd->AddToConsoleObjBuf(bAddRelaObj);

	for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		pLsRef->GetLsPtr()->AddToConsoleObjBuf(bAddRelaObj);
}

void CLDSLineFlat::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	//读取影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropFromBuffer(buffer,version,doc_type);	
		return;
	}
	DWORD dw;
	bool bPrevDeflateVersion=false;	//文件瘦身之前的版本
	if( (doc_type==1&&version>0&&version<4000027)||	//TMA
		(doc_type==2&&version>0&&version<1000082)||	//LMA
		(doc_type==4&&version>0&&version<1000009))	//LDS
		bPrevDeflateVersion=true;
	buffer.ReadDword(&dwPermission);
	buffer.ReadDword(&dwStartPermission);
	buffer.ReadDword(&dwEndPermission);
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
	buffer.ReadInteger(&_hPartWeldParent);
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
	buffer.ReadDouble(&size.thick);
	if(version==0||doc_type==4||doc_type==5||
		(doc_type==1&&version>=4000021)||	//TMA
		(doc_type==2&&version>=1000076))	//LMA
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
	long h;
	buffer.ReadInteger(&h);
	//if(SuperiorTower()->hashObjs.GetHashTableSize()>0)
	//	SuperiorTower()->hashObjs.GetValueAt(h,(CLDSObject*&)pStart);
	//else
		pStart=BelongModel()->FromNodeHandle(h);	// 起点句柄
	buffer.ReadInteger(&h);
	//if(SuperiorTower()->hashObjs.GetHashTableSize()>0)
	//	SuperiorTower()->hashObjs.GetValueAt(h,(CLDSObject*&)pEnd);
	//else
		pEnd=BelongModel()->FromNodeHandle(h);	// 终点句柄
	
	if( version==0||doc_type==5||			//TDA
		(doc_type==1&&version>=4000028)||	//TMA
		(doc_type==2&&version>=1000083)||	//LMA
		(doc_type==4&&version>=1000010))	//LDS
		buffer.ReadWord(&m_iNormCalStyle);
	buffer.ReadDouble(&nearWorkPlaneNorm.x);
	buffer.ReadDouble(&nearWorkPlaneNorm.y);
	buffer.ReadDouble(&nearWorkPlaneNorm.z);
	buffer.ReadInteger(&hWorkNormRefPart);
	buffer.ReadDouble(&workPlaneNorm.x);	// 主工作平面法线方向.
	buffer.ReadDouble(&workPlaneNorm.y);	// 主工作平面法线方向.
	buffer.ReadDouble(&workPlaneNorm.z);	// 主工作平面法线方向.
	f3dPoint start,end;
	buffer.ReadDouble(&start.x);// 实际楞线起点
	buffer.ReadDouble(&start.y);// 实际楞线起点
	buffer.ReadDouble(&start.z);// 实际楞线起点
	buffer.ReadDouble(&end.x);	// 实际楞线终点
	buffer.ReadDouble(&end.y);	// 实际楞线终点
	buffer.ReadDouble(&end.z);	// 实际楞线终点
	baseline.SetStart(start);
	baseline.SetEnd(end);
	m_xStartBeforeLift=start;	//赋默认值
	m_xEndBeforeLift  =end;		//赋默认值
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
	if(version==0||(doc_type==2&&version>=1000106)||(doc_type==3&&version>=1070306)||
		(doc_type==4&&version>=1000306)||(doc_type==5&&version>=1000306))
	{
		connectStart.FromBuffer(buffer,version,doc_type);
		connectEnd.FromBuffer(buffer,version,doc_type);
	}
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
	if( version==0||doc_type==5||
		(doc_type==1&&version>=4000030)||	//TMA
		(doc_type==2&&version>=1000085)||	//LMA
		(doc_type==4&&version>=1000013))//LDS
		buffer.ReadByte(&desStartPos.datum_pos_style);
	buffer.ReadInteger(&desStartPos.hDatumPart);
	buffer.ReadInteger(&desStartPos.hDatumStartPart);
	buffer.ReadInteger(&desStartPos.hDatumEndPart);
	buffer.ReadInteger(&desStartPos.bDatumEndStartEnd);
	buffer.ReadInteger(&desStartPos.bDatumStartStartEnd);
	if( version==0||doc_type==5||
		(doc_type==1&&version>=4000029)||	//TMA
		(doc_type==2&&version>=1000084)||	//LMA
		(doc_type==4&&version>=1000011))//LDS
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
	//终端定位参数
	buffer.ReadInteger(&desEndPos.endPosType);
	if( version==0||doc_type==5||
		(doc_type==1&&version>=4000030)||	//TMA
		(doc_type==2&&version>=1000085)||	//LMA
		(doc_type==4&&version>=1000013))//LDS
		buffer.ReadByte(&desEndPos.datum_pos_style);
	buffer.ReadInteger(&desEndPos.hDatumPart);
	buffer.ReadInteger(&desEndPos.hDatumStartPart);
	buffer.ReadInteger(&desEndPos.hDatumEndPart);
	buffer.ReadInteger(&desEndPos.bDatumEndStartEnd);
	buffer.ReadInteger(&desEndPos.bDatumStartStartEnd);
	if( version==0||doc_type==5||
		(doc_type==1&&version>=4000029)||	//TMA
		(doc_type==2&&version>=1000084)||	//LMA
		(doc_type==4&&version>=1000011))//LDS
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
	//扁钢螺栓区
	pLsRefList->FromBuffer(buffer,BelongModel(),version,doc_type);
}
//自协同中心通过管道读取数据
void CLDSLineFlat::FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord)
{
	//读取影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer))
	{
		IndependentShadowPropFromBuffer(buffer);
		return;
	}
	long handle=0;
	buffer.ReadDword(&dwPermission);
	buffer.ReadDword(&dwStartPermission);
	buffer.ReadDword(&dwEndPermission);
	if(dwPermission&dwUpdateWord)
	{
		buffer.Read(&cfgword.flag.word,24);
		buffer.ReadByte(&_material);
		buffer.ReadByte(&_cQualityLevel);
		buffer.ReadBooleanThin(&_bVirtualPart);//隐含构件
		buffer.ReadInteger(&_iSeg.iSeg);
		buffer.ReadString(sPartNo);	//构件编号
		buffer.ReadInteger(&m_hArcLift);//预拱项 wht 16-01-07
		buffer.ReadString(layer(),4);	//扁钢所在图层.
		buffer.ReadInteger(&m_uStatMatCoef);
		buffer.ReadInteger(&_hPartWeldParent);
		buffer.ReadBooleanThin(&m_bDisplayHole);//显示螺栓孔及切割面
		buffer.ReadString(sNotes,51);
		buffer.ReadDouble(&size.wide);	
		buffer.ReadDouble(&size.thick);
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
		//扁钢螺栓区
		pLsRefList->FromBuffer(buffer,m_pModel);
	}
	if(dwStartPermission&dwUpdateWord)
	{
		buffer.ReadInteger(&handle);
		//if(SuperiorTower()->hashObjs.GetHashTableSize()>0)
		//	SuperiorTower()->hashObjs.GetValueAt(handle,(CLDSObject*&)pStart);
		//else
			pStart=BelongModel()->FromNodeHandle(handle);	// 起点句柄
		f3dPoint start;
		buffer.ReadDouble(&start.x);// 实际楞线起点
		buffer.ReadDouble(&start.y);// 实际楞线起点
		buffer.ReadDouble(&start.z);// 实际楞线起点
		baseline.SetStart(start);
		buffer.ReadPoint(m_xStartBeforeLift);
		connectStart.FromBuffer(buffer);
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
		buffer.ReadInteger(&handle);
		//if(SuperiorTower()->hashObjs.GetHashTableSize()>0)
		//	SuperiorTower()->hashObjs.GetValueAt(handle,(CLDSObject*&)pEnd);
		//else
			pStart=BelongModel()->FromNodeHandle(handle);	// 起点句柄
		f3dPoint end;
		buffer.ReadDouble(&end.x);// 实际楞线起点
		buffer.ReadDouble(&end.y);// 实际楞线起点
		buffer.ReadDouble(&end.z);// 实际楞线起点
		baseline.SetStart(end);
		buffer.ReadPoint(m_xEndBeforeLift);
		connectEnd.FromBuffer(buffer);
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
void CLDSLineFlat::ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord)
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
		buffer.WriteDouble(size_thick);
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
		//扁钢螺栓区
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
		connectStart.ToBuffer(buffer);
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
		connectEnd.ToBuffer(buffer);
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
int CLDSLineFlat::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
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
	{
		if(isalpha(m_cQualityLevel))
			sprintf(sText,"%C",m_cQualityLevel);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Default");
#else
			strcpy(sText,"默认");
#endif
	}
	else if(GetPropID("specification")==id)
	{
		char s_w[100],s_t[100];
		sprintf(s_w,"%f",size_wide);
		SimplifiedNumString(s_w);
		sprintf(s_t,"%f",size_thick);
		SimplifiedNumString(s_t);
		sprintf(sText,"%sX%s",s_w,s_t); 
	}
	else if(GetPropID("iSeg")==id)
		strcpy(sText,(char*)iSeg.ToString());
	else if(GetPropID("sPartNo")==id)
		sprintf(sText,"%s",(char*)GetPartNo());
	else if(GetPropID("cfgword")==id)
		//return MakeWordToString(cfgword,valueStr);
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
		CLDSLineFlat *pMotherLineFlat=(CLDSLineFlat*)MotherObject(false);
		if(pMotherLineFlat!=this)
			sprintf(sText,"0X%X",pMotherLineFlat->handle);
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
	else if(GetPropID("m_hPartWeldParent")==id)
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
	else if(GetPropID("hStart")==id)
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
	else if(GetPropID("hEnd")==id)
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
		sprintf(sText,"%d",connectStart.wnConnBoltN);
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
		sprintf(sText,"%d",connectEnd.wnConnBoltN);
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
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}

long CLDSLineFlat::GetPropID(char* propStr)
{
	PROPLIST_ITEM *pItem = CLDSLineFlat::propHashtable.GetValue(propStr);
	if(pItem!=NULL)
		return pItem->id;
	else
	{
		/*CString prompt="没找到名为\"";
		prompt+=propStr;
		prompt+="\"的属性ID！";
		AfxMessageBox(prompt);*/
		return 0;
	}
}

BOOL CLDSLineFlat::IsPropEqual(HASHOBJSET &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSObject *pFirstObj = selectObjs.GetFirst();
	CLDSLineFlat *pLineFlat=NULL, *pFirstLineFlat = (CLDSLineFlat*)pFirstObj;
	pFirstLineFlat->GetPropValueStr(idProp,valueStr);
	for(pLineFlat=(CLDSLineFlat*)selectObjs.GetNext();pLineFlat;pLineFlat=(CLDSLineFlat*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pLineFlat->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

int CLDSLineFlat::GetPropStatus(long id)
{	
	DWORD dwStatus;
	if(CLDSLineFlat::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
}

BOOL CLDSLineFlat::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSLineFlat::propStatusHashtable.SetValueAt(id,dwStatus);
}

BOOL CLDSLineFlat::GetObjPropReadOnlyDefaultState(long id)
{
	return GetObjPropReadOnlyDefaultState(id,ShadowMode());
}
BOOL CLDSLineFlat::GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode)
{
	if (shadow_mode==CLDSObject::SHADOWMODE_COMMON)
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
		else
			return FALSE;
	}
}

void CLDSLineFlat::CalWorkPlaneNorm()
{
	if(m_iNormCalStyle==0)
		return;	//指定工作面法线
	CLDSLinePart *pDatumLinePart;
	f3dPoint datum_vec,current_vec;
	current_vec=Start()-End();
	normalize(current_vec);
	if(hWorkNormRefPart!=0)
	{
		pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(hWorkNormRefPart);
		if(pDatumLinePart&&pDatumLinePart->IsLinePart())
		{
			datum_vec=pDatumLinePart->End()-pDatumLinePart->Start();
			workPlaneNorm=datum_vec^current_vec;
		}
		else if(!nearWorkPlaneNorm.IsZero())
		{
			workPlaneNorm=nearWorkPlaneNorm;
			f3dPoint vec=workPlaneNorm^current_vec;
			workPlaneNorm=current_vec^vec;
		}
		normalize(workPlaneNorm);
	}
	else if(pStart->hFatherPart>=0x20&&pStart->hFatherPart!=handle)	//始端搭接在其它杆件上
	{
		if(pStart->hFatherPart>=0x20)
			pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(pStart->hFatherPart);
		else if(desStartPos.hDatumPart>=0x20)
			pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(desStartPos.hDatumPart);
		if(pDatumLinePart&&pDatumLinePart->IsLinePart())
			datum_vec=pDatumLinePart->End()-pDatumLinePart->Start();
		normalize(datum_vec);
		if(fabs(datum_vec*current_vec)<EPS_COS2)
			workPlaneNorm=datum_vec^current_vec;
		else
		{
			workPlaneNorm = nearWorkPlaneNorm;
			f3dPoint vec=workPlaneNorm^current_vec;
			workPlaneNorm=current_vec^vec;
		}
		normalize(workPlaneNorm);
	}
	else if(pEnd->hFatherPart>=0x20&&pEnd->hFatherPart!=handle)	//终端搭接在其它杆件上
	{
		if(pEnd->hFatherPart>=0x20)
			pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(pEnd->hFatherPart);
		else if(desEndPos.hDatumPart>=0x20)
			pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(desEndPos.hDatumPart);
		if(pDatumLinePart&&pDatumLinePart->IsLinePart())
		{
			datum_vec=pDatumLinePart->End()-pDatumLinePart->Start();
			workPlaneNorm=datum_vec^current_vec;
			normalize(workPlaneNorm);
		}
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

UCS_STRU CLDSLineFlat::BuildUCS()
{
	ucs.axis_x=End()-Start();
	ucs.origin=Start();
	if(!normalize(workPlaneNorm))
		workPlaneNorm=inters_vec(ucs.axis_x);
	ucs.axis_z=workPlaneNorm;
	normalize(ucs.axis_x);
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	normalize(ucs.axis_y);
	ucs.axis_z=ucs.axis_x^ucs.axis_y;
	return ucs;
}

int CLDSLineFlat::GetSpecification(char *spec,BOOL bIncMat/*=TRUE*/)
{
	char steelmark[20]="";
	char tmp_spec[200],tmp_spec2[200];
	if(bIncMat&&cMaterial!='S')
		QuerySteelMatMark(cMaterial,steelmark);
	sprintf(tmp_spec,"%s-%f",steelmark,size_wide);
	SimplifiedNumString(tmp_spec);
	_snprintf(tmp_spec2,199,"%sx%f",tmp_spec,size_thick);
	SimplifiedNumString(tmp_spec2);
	if(spec)
		strcpy(spec,tmp_spec2);
	return strlen(tmp_spec2);
}

double CLDSLineFlat::GetWeight(BOOL bSolidLength/*=TRUE*/)
{
	BIAN_PARA_TYPE* pFlatPara=FindBianType(size_wide,size_thick);
	double length;
	if(bSolidLength||pStart==NULL||pEnd==NULL)
		length=GetLength();
	else
		length=DISTANCE(pStart->Position(false),pEnd->Position(false));
	if(pFlatPara)
		return pFlatPara->theroy_weight*length*0.001;
	else
		return CalSteelPlankWei(size_wide*size_thick*length);
}

void CLDSLineFlat::CopyProperty(CLDSDbObject *pObj)
{
	if(pObj->GetClassTypeId()!=CLS_LINEFLAT)
		return;
	CLDSLineFlat *pFlat=(CLDSLineFlat*)pObj;
	size_thick = pFlat->GetThick();
	size_wide = pFlat->GetWidth();
	if(!IsStartPosLocked())
		desStartPos.endPosType=pFlat->desStartPos.endPosType;
	if(!IsEndPosLocked())
		desEndPos.endPosType=pFlat->desEndPos.endPosType;
    SetLayer(pFlat->layer());		
	iSeg = pFlat->iSeg;
	sPartNo.Copy(pFlat->sPartNo);
	m_uStatMatCoef = pFlat->m_uStatMatCoef;
	is_data_modified = TRUE;
	cMaterial = pFlat->cMaterial;
	cfgword = pFlat->cfgword;
}

void CLDSLineFlat::CalStartPos(CLDSArcLift *pLift/*=NULL*/)
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
		CLDSLineFlat *pStartDatumFlat=(CLDSLineFlat*)BelongModel()->FromPartHandle(desStartPos.hDatumStartPart,CLS_LINEFLAT);
		CLDSLineFlat *pEndDatumFlat=(CLDSLineFlat*)BelongModel()->FromPartHandle(desStartPos.hDatumEndPart,CLS_LINEFLAT);
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
void CLDSLineFlat::CalEndPos(CLDSArcLift *pLift/*=NULL*/)
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
		CLDSLineFlat *pStartDatumFlat=(CLDSLineFlat*)BelongModel()->FromPartHandle(desEndPos.hDatumStartPart,CLS_LINEFLAT);
		CLDSLineFlat *pEndDatumFlat=(CLDSLineFlat*)BelongModel()->FromPartHandle(desEndPos.hDatumEndPart,CLS_LINEFLAT);
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

void CLDSLineFlat::CalPosition(bool bIgnoreRepeatFlag/*=FALSE*/)
{
	if(IsShadowObject())
	{
		CalShadowRodPos();
		return;
	}
	CLDSArcLift *pLift=NULL;
	if(m_hArcLift>0x20)
		pLift=BelongModel()->FromArcLiftHandle(m_hArcLift);
	if(bIgnoreRepeatFlag)
		ClearFlag();
	CalStartPos(pLift);
	CalEndPos(pLift);
}

bool CLDSLineFlat::IsEqual(CLDSLineFlat* pMirFlat,double epsilon/*=1.0*/,char *error_info/*=NULL*/,
						   int *pErrorInfo/*=NULL*/,DWORD *pChkStyleFlag/*=NULL*/)
{
	if(pChkStyleFlag==NULL||(*pChkStyleFlag)&CHKPARTNO_RODINFO)
	{
		bool bRet=true;
		if(fabs(GetLength()-pMirFlat->GetLength())>epsilon)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s} Different length",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}长度不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(size_wide-pMirFlat->GetWidth()!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s} Different width",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}宽度不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(size_thick-pMirFlat->GetThick()!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s} Different thickness",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}厚度不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(cMaterial!=pMirFlat->cMaterial)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Different material",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}材质不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		if(!bRet)
		{
			if(pErrorInfo)
				*pErrorInfo=CHKPARTNO_RODINFO;
			return false;
		}
	}
	if(pChkStyleFlag==NULL||(*pChkStyleFlag)&CHKPARTNO_RODBOLTINFO)
	{	//比较螺栓信息
		BuildUCS();
		pMirFlat->BuildUCS();;
		UCS_STRU tem_ucs = ucs;
		UCS_STRU tem_vice_ucs = pMirFlat->ucs;
		Sub_Pnt(tem_ucs.origin,ucs.origin,ucs.axis_z*start_oddment);
		Sub_Pnt(tem_vice_ucs.origin,pMirFlat->ucs.origin,pMirFlat->ucs.axis_z*pMirFlat->start_oddment);
		char sError[200]="";
		if(!IsLsRefListEqual( pLsRefList,tem_ucs,
			pMirFlat->pLsRefList,tem_vice_ucs,epsilon,sError,NULL))
		{
			if(error_info)
				sprintf(error_info,"{%s}%s",(char*)sPartNo,sError);
			if(pErrorInfo)
				*pErrorInfo=CHKPARTNO_RODBOLTINFO;
			return false;
		}
	}
	return true;
}

//槽钢是否为脚钉板 wht 11-07-21
int CLDSLineSlot::IsBaseSlot()
{
	//if(GetLsCount()!=1) 有时候本是脚钉底座，但用户误操作误删了脚钉螺栓　wjh-2016.10.9
	if(GetLsCount()>2||(pStart!=NULL||pEnd!=NULL))
		return FALSE;	
	BOOL bNailSlot=2;
	for(CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
	{
		if((*pLsRef)->IsFootNail())
			bNailSlot=TRUE;
		else
			return 2;
	}
	return bNailSlot;
}
//获取螺栓穿过槽钢外皮的交点位置 wht 15-09-19
bool CLDSLineSlot::GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* btm0_left1_right2,int iIntersPlane)
{
	GEPOINT inters;
	f3dPoint worknorm=pBolt->ucs.axis_z;
	if(pBolt->m_dwFlag.IsHasFlag(CLDSBolt::REVERSE_NORM))
		worknorm*=-1.0;
	double ddx=worknorm*ucs.axis_y;
	double ddy=worknorm*ucs.axis_x;
	if( fabs(ddx)>fabs(ddy))	//槽钢底面螺栓
	{
		Int3dlf(inters,pBolt->ucs.origin,pBolt->get_norm(),Start(),ucs.axis_y);
		if(iIntersPlane==CLDSPart::BOLTBTM_PLANE&&ddx<0)
			inters+=ucs.axis_y*size.thick;
		if(btm0_left1_right2!=NULL)
			*btm0_left1_right2=0;
	}
	else
	{
		BuildUCS();
		f3dPoint ls_pos=pBolt->ucs.origin;
		coord_trans(ls_pos,ucs,FALSE);
		double fHalfHeight=size_height*0.5;
		f3dPoint face_norm=ucs.axis_x;
		if(ls_pos.x<0)	//X轴负方向
		{
			face_norm=-ucs.axis_x;
			ddy*=-1.0;
		}
		if(btm0_left1_right2!=NULL)
			*btm0_left1_right2=ls_pos.x<0?1:2;
		Int3dlf(inters,pBolt->ucs.origin,pBolt->get_norm(),Start()+face_norm*fHalfHeight,face_norm);
		if(iIntersPlane==CLDSPart::BOLTBTM_PLANE&&ddy>0)
			inters+=ucs.axis_x*size.thick;
	}
	if(inters_coords)
		memcpy(inters_coords,&inters,24);
	return true;
}
#ifdef __LDS_CONTEXT_
bool CLDSLineSlot::CloneToBomPart(BOMPART* pBomPart)
{
	if(pBomPart->cPartType!=BOMPART::SLOT)
		return false;
	PART_SLOT *pBomSlot=(PART_SLOT*)pBomPart;
	CLDSPart::CloneToBomPart(pBomPart);	//克隆基本属性
	//克隆槽钢特有属性
	//螺栓
	pBomSlot->nMSumLs = GetLsCount();
	pBomSlot->m_arrBoltRecs.SetSize(pBomSlot->nMSumLs);
	int i=0;
	for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
	{
		pBomSlot->m_arrBoltRecs[i].d = ftoi((*pLsRef)->get_d());
		pBomSlot->m_arrBoltRecs[i].hole_d_increment = (float)(*pLsRef)->hole_d_increment;
		f3dPoint ls_pos=(*pLsRef)->ucs.origin;
		coord_trans(ls_pos,ucs,FALSE);
		pBomSlot->m_arrBoltRecs[i].x = (float)ls_pos.x;
		pBomSlot->m_arrBoltRecs[i].y = (float)ls_pos.y;
		pBomSlot->m_arrBoltRecs[i].cFuncType=(*pLsRef)->FuncType();
		pBomSlot->m_arrBoltRecs[i].cFlag=pLsRef->cFlag;
		pBomSlot->m_arrBoltRecs[i].waistLen=pLsRef->waistLen;
		i++;
	}
	return true;
}
#endif

BOOL CLDSLineFlat::Create3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	if(CreateShadowPart3dSolidModel(bDisplayHole,scale_of_user2scr,sample_len,smoothness))
		return TRUE;

	if(pSolidBody!=NULL&& !IsModified())
		return FALSE;
	else
	{
		if(pSolidBody == NULL)
			pSolidBody = new CSolidBody;
		else
			pSolidBody->Empty();
		SetModified(FALSE);
		/*if(pBody==NULL)
			pBody = new fBody;
		else if(!is_data_modified)	//数据未修改
			return TRUE;
		else
			pBody->Empty();*/
		fBody body;
		fBody *pBody=&body;
		int i=0;
		BuildUCS();
		f3dPoint vertex_arr[4];
		COLORREF pole_color=GetColor(CLS_LINEFLAT,3,cMaterial);	//默认杆件颜色
		if(m_bUserSpecColor)	//用户指定构件颜色
			pole_color=crMaterial;
		double length=DISTANCE(Start(),End());
		is_data_modified = FALSE;
		double fHalfWide=size_wide/2;
		vertex_arr[0].x = -startOdd();
		vertex_arr[0].y = -fHalfWide;
		vertex_arr[0].z = 0;

		vertex_arr[1].x = -startOdd();
		vertex_arr[1].y = fHalfWide;
		vertex_arr[1].z = 0;

		vertex_arr[2].x = length+endOdd();
		vertex_arr[2].y = fHalfWide;
		vertex_arr[2].z = 0;

		vertex_arr[3].x = length+endOdd();
		vertex_arr[3].y = -fHalfWide;
		vertex_arr[3].z = 0;
		f3dPolyFace *pTopFace,*pBottomFace,*pFace;
		for(i=0;i<4;i++)
		{
			vertex_arr[i].z=size_thick;
			pBody->vertex.append(vertex_arr[i]);
			vertex_arr[i].z=0;
			pBody->vertex.append(vertex_arr[i]);
		}
		//初始化链表
		CCreateFace createFace;
		createFace.InitVertexList(pBody);

		//扁钢顶面生成
		pTopFace = pBody->faceList.append();
		pTopFace->material = pole_color;
		createFace.NewOutterEdgeLine(pTopFace,3*2,0);
		createFace.NewOutterEdgeLine(pTopFace,2*2);
		createFace.NewOutterEdgeLine(pTopFace,1*2);
		createFace.NewOutterEdgeLine(pTopFace,0);
		//扁钢各侧面生成
		for(i=0; i<4; i++)
		{
			pFace=pBody->faceList.append();
			pFace->material = pole_color;
			createFace.NewOutterEdgeLine(pFace,0+2*i,1+2*i);
			createFace.NewOutterEdgeLine(pFace,(2+2*i)%8);
			createFace.NewOutterEdgeLine(pFace,(3+2*i)%8);
			createFace.NewOutterEdgeLine(pFace,1+2*i);
		}
		//扁钢底面生成
		pBottomFace = pBody->faceList.append();
		pBottomFace->material = pole_color;
		createFace.NewOutterEdgeLine(pBottomFace,0+1,3*2+1);
		createFace.NewOutterEdgeLine(pBottomFace,1*2+1);
		createFace.NewOutterEdgeLine(pBottomFace,2*2+1);
		createFace.NewOutterEdgeLine(pBottomFace,3*2+1);
		for(i=0;i<8;i++)
		{
			if(coord_trans(pBody->vertex[i],ucs,TRUE)!=TRUE)
				return FALSE;
		}
		//定义螺栓孔
		CHoleMaker holeMaker;
		UCS_STRU hole_ucs;
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(m_bDisplayHole || bDisplayHole|| pLsRef->GetLsPtr()->m_bVirtualPart)
			{	//系统要求显示螺栓孔或当前螺栓为虚拟螺栓
				hole_ucs.origin=(*pLsRef)->ucs.origin;
				hole_ucs.axis_z=(*pLsRef)->get_norm();
				hole_ucs.axis_x=(*pLsRef)->ucs.axis_x;
				//hole_ucs.axis_x=inters_vec(hole_ucs.axis_z);
				//hole_ucs.axis_y=hole_ucs.axis_z^hole_ucs.axis_x;
				//hole_ucs.axis_x=hole_ucs.axis_y^hole_ucs.axis_z;
				COLORREF crLsHole=GetColor(CLS_BOLT,(*pLsRef)->get_d(),' ');
				holeMaker.Create(hole_ucs,(*pLsRef)->get_d()+(*pLsRef)->hole_d_increment,pLsRef->waistLen,pTopFace,pBottomFace,pBody,crLsHole);
			}
		}
		pSolidBody->ConvertFrom(pBody);
		return TRUE;
	}
}
#endif