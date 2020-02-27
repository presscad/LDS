//<LOCALE_TRANSLATE BY hxr /> 2014-12-9
// ArcPart.cpp: implementation of the CLDSSlot class.
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
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//ArcPart
CLDSArcPart::CLDSArcPart()
{
	m_uDisplaySlices=16;
	m_nClassTypeId = CLS_ARCPART;
	LoadDefaultUCS(&ucs);
	//strcpy(m_sClassName,"CLDSArcPart");
	m_uStatMatCoef=1;
	m_bVirtualPart = false;
	m_bDisplayHole=false;
	strcpy(sPartNo,"");
	m_hPartWeldParent=0;
	m_siLayoutStyle=0;
	start_oddment=end_oddment=0;
	m_fZoomLenCoef=0.98;
	m_fBaseLineRadius=0;
	offset_h=56;
	offset_v=-28;
	start_rot_angle=sector_angle=0;
	pStart=pEnd=_pCenterPickNode=_pPlanePickNode=NULL;
	flag=1;	//指定圆弧起点及扇形角
	pSolidBody=NULL;
	pLsRefList=new CLsRefList();
	crMaterial=GetColor(CLS_ARCPART,3,'S');
}
bool CLDSArcPart::IsAcceptableTypeId(int type_id)
{
	if(type_id==CLS_ARCPART||type_id==CLS_PART||type_id==CLS_DBOBJECT||type_id==CLS_OBJECT)
		return true;
	else if(type_id==m_nClassTypeId)
		return true;
	else
		return false;
}
CLDSArcPart::~CLDSArcPart()
{
	if(pLsRefList!=NULL)
	{
		delete pLsRefList;
		pLsRefList=NULL;
	}
	if(pSolidBody)
	{
		delete pSolidBody;
		pSolidBody=NULL;
	}
}
IMPLEMENT_GET(CLDSArcPart,type)
{	//flag末尾0~2位(低位)
	return flag&0x07;
}
IMPLEMENT_SET(CLDSArcPart,type)
{
	flag&=0xf8;
	flag|=value&0x07;
}
//圆弧圆心点或圆心侧拾取点
IMPLEMENT_GET(CLDSArcPart,pCenterPickNode)
{
	BYTEFLAG flager(flag);
	if(flager.GetBitState(3))
		return _pCenterPickNode;
	else
		return NULL;
}
IMPLEMENT_SET(CLDSArcPart,pCenterPickNode)
{
	BYTEFLAG flager(flag);
	flager.SetBitState(3,value!=NULL);
	_pCenterPickNode=value;
}
IMPLEMENT_GET(CLDSArcPart,center)
{
	if(baseline.SectorAngle()>0&&!baseline.WorkNorm().IsZero())
		return baseline.Center();
	else
		return des_arc_center.Position();
}
IMPLEMENT_SET(CLDSArcPart,center)
{
	des_arc_center.SetPosition(value.x,value.y,value.z);
}
//圆弧工作平面拾取点
IMPLEMENT_GET(CLDSArcPart,pPlanePickNode)
{
	BYTEFLAG flager(flag);
	if(flager.GetBitState(4))
		return _pPlanePickNode;
	else
		return NULL;
}
IMPLEMENT_SET(CLDSArcPart,pPlanePickNode)
{
	BYTEFLAG flager(flag);
	flager.SetBitState(4,value!=NULL);
	_pPlanePickNode=value;
}
IMPLEMENT_GET(CLDSArcPart,worknorm)
{
	return des_arc_norm.vector;
}
IMPLEMENT_SET(CLDSArcPart,worknorm)
{
	des_arc_norm.vector=value;
}
void CLDSArcPart::IndependentShadowPropFromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{

}
void CLDSArcPart::IndependentShadowPropToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{

}
void CLDSArcPart::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	DWORD dw;
	bool bPrevDeflateVersion=false;	//文件瘦身之前的版本
	if( (doc_type==1&&version!=0&&version<4000027)||
		(doc_type==2&&version!=0&&version<1000082)||
		(doc_type==4&&version!=0&&version<1000009))
		bPrevDeflateVersion=true;
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropFromBuffer(buffer,version,doc_type);
		return;
	}
	CfgwordFromBuffer(cfgword,buffer,version,doc_type);
	BelongModel()->EvolveCfgword(cfgword,version,doc_type);
	buffer.ReadByte(&_material);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.ReadByte(&_cQualityLevel);
	buffer.ReadInteger(&_iSeg.iSeg);
	buffer.ReadString(sPartNo,16);			//构件编号
	buffer.ReadString(layer(),4);			//扁钢所在图层.
	if(bPrevDeflateVersion)
	{
		buffer.ReadInteger(&dw);//隐含构件
		m_bVirtualPart=(dw!=0);
	}
	else
		buffer.ReadBooleanThin(&_bVirtualPart);
	buffer.ReadInteger(&m_uStatMatCoef);
	buffer.ReadInteger(&_hPartWeldParent);
	if(bPrevDeflateVersion)
	{
		buffer.ReadInteger(&dw);//显示螺栓孔及切割面
		m_bDisplayHole=(dw!=0);
	}
	else
		buffer.ReadBooleanThin(&m_bDisplayHole);
	buffer.ReadInteger(&m_uDisplaySlices);	//圆弧显示片数
	buffer.ReadDouble(&m_fBaseLineRadius);		//圆弧中性层半径
	buffer.ReadDouble(&start_oddment);
	buffer.ReadDouble(&end_oddment);
	buffer.ReadDouble(&offset_h);	//基准母线距离截面形状特征(标定)原点的横向位移
	buffer.ReadDouble(&offset_v);	//基准母线距离截面形状特征(标定)原点的竖向位移
	buffer.ReadWord(&m_siLayoutStyle);
	if(	version==0||
		(doc_type==1&&version>=5010301)||	//TMA V5.1.3.1
		(doc_type==2&&version>=2000601)||	//LMA V2.0.6.1
		//(doc_type==3&&version>=2000100)||	//TSA V2.0.1.0
		(doc_type==4&&version>=1030601)||	//LDS V1.3.6.1
		(doc_type==5&&version>=1030601))	//TDA V1.3.6.1
	{
		long hNode=0;
		buffer.ReadByte(&flag);
		buffer.ReadDouble(&m_fZoomLenCoef);
		pStart=pEnd=_pCenterPickNode=_pPlanePickNode=NULL;
		if(type==0)
		{
			buffer.ReadInteger(&hNode);
			pStart=BelongModel()->FromNodeHandle(hNode);
			buffer.ReadInteger(&hNode);
			pEnd  =BelongModel()->FromNodeHandle(hNode);
		}
		if(flag&0x08)	//flager.GetBitState(3)
		{
			buffer.ReadInteger(&hNode);
			_pCenterPickNode=BelongModel()->FromNodeHandle(hNode);
		}
		if(flag&0x10)	//flager.GetBitState(4))
		{
			buffer.ReadInteger(&hNode);
			_pPlanePickNode=BelongModel()->FromNodeHandle(hNode);
		}
	}
	if(	version==0||
		(doc_type==1&&version>=4010100)||	//TMA V4.1.1.0
		(doc_type==2&&version>=1010100)||	//LMA V1.1.1.0
		(doc_type==3&&version>=2000100)||	//TSA V2.0.1.0
		(doc_type==4&&version>=1020100)||	//LDS V1.2.1.0
		(doc_type==5&&version>=1020100))	//TDA V1.2.1.0
	{
		buffer.ReadPoint(arc_start);		
		buffer.ReadDouble(&start_rot_angle);//圆弧起始角度
		buffer.ReadDouble(&sector_angle);	//圆弧扇区角度
		des_arc_center.FromBuffer(buffer,version,doc_type);
		des_arc_start.FromBuffer(buffer,version,doc_type);
		des_arc_norm.FromBuffer(buffer,version,doc_type);
	}
	else 
	{
		f3dPoint center,worknorm,columnnorm;
		buffer.ReadPoint(center);			//圆弧中心
		buffer.ReadPoint(arc_start);
		buffer.ReadDouble(&start_rot_angle);
		buffer.ReadDouble(&sector_angle);	//圆弧扇区角度
		buffer.ReadPoint(worknorm);			//工作面法线
		buffer.ReadPoint(columnnorm);		//圆柱面法线
		//将旧版本中圆弧数据参数化
		des_arc_center.datum_pos_style = 0;
		des_arc_center.SetPosition(center);
		des_arc_start.datum_pos_style = 0;
		des_arc_start.SetPosition(arc_start);
		des_arc_norm.norm_style = 0;
		des_arc_norm.vector = worknorm;
	}
	DesignArcBaseLine();

	buffer.ReadString(sNotes,51);
	if(version==0||doc_type>2||(doc_type==1&&version>=4000021)||(doc_type==2&&version>=1000076))
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
	if(version==0||(doc_type==1&&version>=4000024)||(doc_type==2&&version>=1000079)||(doc_type>2&&version>=1000006))
		pLsRefList->FromBuffer(buffer,BelongModel(),version,doc_type);	//环形构件螺栓引用
}
void CLDSArcPart::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if(ShadowParaAndBlockIdToBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropToBuffer(buffer,version,doc_type);
		return;
	}
	CfgwordToBuffer(cfgword,buffer,version,doc_type);
	buffer.WriteByte(cMaterial);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.WriteByte(m_cQualityLevel);
	buffer.WriteInteger(iSeg);
	buffer.WriteString(sPartNo);	//构件编号
	buffer.WriteString(layer());	//扁钢所在图层.
	buffer.WriteBooleanThin(_bVirtualPart);//隐含构件
	buffer.WriteInteger(m_uStatMatCoef);
	buffer.WriteInteger(m_hPartWeldParent);
	buffer.WriteBooleanThin(m_bDisplayHole);//显示螺栓孔及切割面
	buffer.WriteInteger(m_uDisplaySlices);	//圆弧显示片数
	buffer.WriteDouble(m_fBaseLineRadius);		//圆弧中性层半径
	buffer.WriteDouble(start_oddment);
	buffer.WriteDouble(end_oddment);
	buffer.WriteDouble(offset_h);	//基准母线距离截面形状特征(标定)原点的横向位移
	buffer.WriteDouble(offset_v);	//基准母线距离截面形状特征(标定)原点的竖向位移
	buffer.WriteWord(m_siLayoutStyle);
	if(	version==0||
		(doc_type==1&&version>=5010301)||	//TMA V5.1.3.1
		(doc_type==2&&version>=2000601)||	//LMA V2.0.6.1
		//(doc_type==3&&version>=2000100)||	//TSA V2.0.1.0
		(doc_type==4&&version>=1030601)||	//LDS V1.3.6.1
		(doc_type==5&&version>=1030601))	//TDA V1.3.6.1
	{
		buffer.WriteByte(flag);
		buffer.WriteDouble(m_fZoomLenCoef);
		if(type==0)
		{
			buffer.WriteInteger(pStart?pStart->handle:0);
			buffer.WriteInteger(pEnd?pEnd->handle:0);
		}
		if(flag&0x08)	//flager.GetBitState(3)
			buffer.WriteInteger((_pCenterPickNode!=NULL)?_pCenterPickNode->handle:0);
		if(flag&0x10)	//flager.GetBitState(4))
			buffer.WriteInteger((_pPlanePickNode!=NULL)?_pPlanePickNode->handle:0);
	}
	buffer.WritePoint(arc_start);
	buffer.WriteDouble(start_rot_angle);
	buffer.WriteDouble(sector_angle);
	//保存设计参数
	des_arc_center.ToBuffer(buffer,version,doc_type);
	des_arc_start.ToBuffer(buffer,version,doc_type);
	des_arc_norm.ToBuffer(buffer,version,doc_type);
	buffer.WriteString(sNotes);
	buffer.WriteBooleanThin(m_bUserSpecColor);
	if(m_bUserSpecColor)
		buffer.WriteDword(crMaterial);
	
	//螺栓引用
	pLsRefList->ToBuffer(buffer,version,doc_type);
}
void CLDSArcPart::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="",sText2[500]="",sText3[500]="";
	fprintf(fp,"<环形构件>\n");
	fprintf(fp,"<配材号 cfgword.word=\"%d,%d,%d,%d,%d\"/>\n",cfgword.flag.word[0],cfgword.flag.word[1],cfgword.flag.word[2],cfgword.flag.word[3],cfgword.flag.word[4]);
	fprintf(fp,"<材质 cMaterial=\"%c\"/>\n",cMaterial);
	fprintf(fp,"<分段号 iSeg=\"%d\"/>\n",iSeg);
	fprintf(fp,"<构件编号 sPartNo=\"%s\"/>\n",(char*)sPartNo);
	fprintf(fp,"<图层 layer=\"%s\"/>\n",layer());
	if (m_bVirtualPart)
		fprintf(fp,"<隐含构件 m_bVirtualPart=\"true\"/>\n");
	else
		fprintf(fp,"<隐含构件 m_bVirtualPart=\"false\"/>\n");
	fprintf(fp,"<统材系数 m_uStatMatCoef=\"%d\"/>\n",m_uStatMatCoef);
	fprintf(fp,"<焊接父构件 m_hPartWeldParent=\"0X%X\"/>\n",m_hPartWeldParent);
	if (m_bDisplayHole)
		fprintf(fp,"<显示螺栓孔及切割面 m_bDisplayHole=\"true\"/>\n");
	else
		fprintf(fp,"<显示螺栓孔及切割面 m_bDisplayHole=\"false\"/>\n");
	fprintf(fp,"<圆弧显示片数 m_uDisplaySlices=\"%d\"/>\n",m_uDisplaySlices);
	sprintf(sText,"%f",m_fBaseLineRadius);
	SimplifiedNumString(sText);
	fprintf(fp,"<圆弧中性层半径 m_fBaseLineRadius=\"%s\"/>\n",sText);
	sprintf(sText,"%f",start_oddment);
	SimplifiedNumString(sText);
	fprintf(fp,"<始端正负头 start_oddment=\"%s\"/>\n",sText);
	sprintf(sText,"%f",end_oddment);
	SimplifiedNumString(sText);
	fprintf(fp,"<始端正负头 end_oddment=\"%s\"/>\n",sText);
	sprintf(sText,"%f",offset_h);
	SimplifiedNumString(sText);
	fprintf(fp,"<原点的横向位移 offset_h=\"%s\"/>\n",sText);
	sprintf(sText,"%f",offset_v);
	SimplifiedNumString(sText);
	fprintf(fp,"<原点的竖向位移 offset_v=\"%s\"/>\n",sText);
	fprintf(fp,"<构件截面相对圆弧的摆放方式 m_siLayoutStyle=\"%d\"/>\n",m_siLayoutStyle);
	sprintf(sText,"%f",arc_start.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",arc_start.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",arc_start.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<起始位置 arc_start=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",start_rot_angle);
	SimplifiedNumString(sText);
	fprintf(fp,"<起始位置旋转角度 start_rot_angle=\"%s\"/>\n",sText);
	sprintf(sText,"%f",sector_angle);
	SimplifiedNumString(sText);
	fprintf(fp,"<圆弧扇区角度 sector_angle=\"%s\"/>\n",sText);
	des_arc_center.ToXmlFile(fp,schema);
	des_arc_start.ToXmlFile(fp,schema);
	des_arc_norm.ToXmlFile(fp,schema);
	fprintf(fp,"<备注 sNotes=\"%s\"/>\n",sNotes);
	if (m_bUserSpecColor)
		fprintf(fp,"<用户指定构件颜色 m_bUserSpecColor=\"true\"/>\n");
	else
		fprintf(fp,"<用户指定构件颜色 m_bUserSpecColor=\"false\"/>\n");
	if(m_bUserSpecColor)
		fprintf(fp,"<构件颜色 crMaterial=\"%d\"/>\n",crMaterial);

	pLsRefList->ToXmlFile(fp,schema);
	fprintf(fp,"</环形构件>\n");
}
BOOL CLDSArcPart::DesignArcBaseLine()
{
	if(pStart)
		des_arc_start.SetPosition(pStart->Position());
	else
		des_arc_start.UpdatePos();
	SetModified();
	if(type>0||pStart==NULL||pEnd==NULL)
	{
		des_arc_center.UpdatePos();
		des_arc_norm.UpdateVector(BelongModel());
		f3dPoint arcStart = des_arc_start.Position();
		f3dPoint arcCenter = des_arc_center.Position();
		f3dPoint arcNorm = des_arc_norm.vector;
		normalize(arcNorm);
		//
		f3dPoint start_vec=arcStart-arcCenter;
		arcStart-=(start_vec*arcNorm)*arcNorm;
		if(fabs(start_rot_angle)>EPS)
			rotate_point_around_axis(arcStart,start_rot_angle,arcCenter,arcCenter+arcNorm*1000);
		return baseline.CreateMethod1(arcCenter,arcStart,arcNorm,sector_angle);
	}
	else
	{	//0.指定圆弧始末端点及半径
		GEPOINT arcStart=pStart->Position();
		GEPOINT arcEnd  =pEnd->Position();
		GEPOINT arcvec=arcEnd-arcStart;
		double arclen_half=arcvec.mod()*0.5;
		//double sectAngle=param.sector_angle;
		sector_angle=Pi-2*ACOS(arclen_half/radius);
		GEPOINT center_pick=pCenterPickNode==NULL?center:pCenterPickNode->Position();
		GEPOINT vecFromStart=center_pick-arcStart;
		GEPOINT vecFromEnd=center_pick-arcEnd;
		GEPOINT work_norm=vecFromStart^vecFromEnd;
		if(worknorm.IsZero())
			worknorm=work_norm;
		else if(worknorm*work_norm<0)
			sector_angle=2*Pi-sector_angle;
		return baseline.CreateMethod2(arcStart,arcEnd,worknorm,sector_angle);
	}
}
BOOL CLDSArcPart::DesignArcBaseLine(CLDSPoint &center,CLDSPoint &start,CLDSVector &workNorm,double rotAngle,double sectorAngle)
{
	des_arc_center = center;
	des_arc_start = start;
	des_arc_norm = workNorm;
	start_rot_angle = rotAngle;
	sector_angle  = sectorAngle;
	return DesignArcBaseLine();
}

int CLDSArcPart::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
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
	else if(GetPropID("m_uStatMatCoef")==id)
		sprintf(sText,"%d",m_uStatMatCoef);
	else if(GetPropID("m_fZoomLenCoef")==id)
	{
		sprintf(sText,"%f",m_fZoomLenCoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_hPartWeldParent")==id)
		sprintf(sText,"0X%X",m_hPartWeldParent);
	else if(GetPropID("m_bDisplayHole")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bDisplayHole)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
		if(m_bDisplayHole)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("m_bVirtualPart")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bVirtualPart)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
		if(m_bVirtualPart)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("m_uDisplaySlices")==id)
		sprintf(sText,"%d",m_uDisplaySlices);
	else if(GetPropID("m_fBaseLineRadius")==id)
	{
		sprintf(sText,"%f",m_fBaseLineRadius);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("offset_h")==id)
	{
		sprintf(sText,"%f",offset_h);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("offset_v")==id)
	{
		sprintf(sText,"%f",offset_v);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_bUserSpecColor")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bUserSpecColor)
			sprintf(sText,"Yes");
		else
			sprintf(sText,"No");
#else
		if(m_bUserSpecColor)
			sprintf(sText,"是");
		else
			sprintf(sText,"否");
#endif
	}
	else if(GetPropID("crMaterial")==id)
		sprintf(sText,"RGB%X",crMaterial);
	else if(GetPropID("sNotes")==id)
		strcpy(sText,sNotes);
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
long CLDSArcPart::GetPropID(char* propStr)
{
	PROPLIST_ITEM *pItem = CLDSArcPart::propHashtable.GetValue(propStr);
	if(pItem!=NULL)
		return pItem->id ;
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		/*CString prompt="Did not find a property ID named \"";
		prompt+=propStr;
		prompt+="\"!";
		AfxMessageBox(prompt);*/
#else
		/*CString prompt="没找到名为\"";
		prompt+=propStr;
		prompt+="\"的属性ID！";
		AfxMessageBox(prompt);*/
#endif
		return 0;
	}
}
int CLDSArcPart::GetPropStr(long id,char* propStr)
{
	char sText[100]="";
	if(GetPropID("basicInfo")==0)
		strcpy(sText,"basicInfo");
	else if(GetPropID("handle")==id)
		strcpy(sText,"handle");
	else if(GetPropID("layer")==id)
		strcpy(sText,"layer");
	else if(GetPropID("cMaterial")==id)
		strcpy(sText,"cMaterial");
	else if(GetPropID("m_cQualityLevel")==id)
		strcpy(sText,"m_cQualityLevel");
	else if(GetPropID("iSeg")==id)
		strcpy(sText,"iSeg");
	else if(GetPropID("sPartNo")==id)
		strcpy(sText,"sPartNo");
	else if(GetPropID("cfgword")==id)
		strcpy(sText,"cfgword");
	else if(GetPropID("m_uStatMatCoef")==id)
		strcpy(sText,"m_uStatMatCoef");
	else if(GetPropID("m_hPartWeldParent")==id)
		strcpy(sText,"m_hPartWeldParent");
	else if(GetPropID("m_bDisplayHole")==id)
		strcpy(sText,"m_bDisplayHole");
	else if(GetPropID("m_bVirtualPart")==id)
		strcpy(sText,"m_bVirtualPart");
	else if(GetPropID("m_uDisplaySlices")==id)
		strcpy(sText,"m_uDisplaySlices");
	else if(GetPropID("m_fBaseLineRadius")==id)
		strcpy(sText,"m_fBaseLineRadius");
	else if(GetPropID("m_fZoomLenCoef")==id)
		strcpy(sText,"m_fZoomLenCoef");
	else if(GetPropID("offset_h")==id)
		strcpy(sText,"offset_h");
	else if(GetPropID("offset_v")==id)
		strcpy(sText,"offset_v");
	else if(GetPropID("sNotes")==id)
		strcpy(sText,"sNotes");
	if(propStr)
		strcpy(propStr,sText);
	return strlen(sText);
}
BOOL CLDSArcPart::IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSArcPart *pPart=NULL, *pFirstPart=(CLDSArcPart*)selectObjs.GetFirst();
	pFirstPart->GetPropValueStr(idProp,valueStr);
	for(pPart=(CLDSArcPart*)selectObjs.GetNext();pPart;pPart=(CLDSArcPart*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pPart->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

int CLDSArcPart::GetPropStatus(long id)
{	
	DWORD dwStatus;
	if(CLDSArcPart::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
}

BOOL CLDSArcPart::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSArcPart::propStatusHashtable.SetValueAt(id,dwStatus);
}

CLsRef* CLDSArcPart::AppendLsRef(CLsRef &ls_ref,BOOL bNearLsCheck/*=TRUE*/)
{
	if(bNearLsCheck)	//检查附近螺栓
	{
		CLsRef *pLsRef=pLsRefList->FindLsByPos(ls_ref->ucs.origin,ls_ref->get_norm(),ls_ref->get_d());
		if(pLsRef&&(*pLsRef)->get_d()==ls_ref->get_d())
			return pLsRef;
	}
	SetModified();
	return pLsRefList->append(ls_ref);
}

CLDSPart *CLDSArcPart::GetMirPart(MIRMSG mirmsg,const double *pPickPos/*=NULL*/,BOOL bCheckProfile/*=TRUE*/)
{
	for(RELATIVE_OBJECT *pItem=relativeObjs.GetFirst();pItem;pItem=relativeObjs.GetNext())
	{	//先在关联构件列表中查找
		CLDSArcPart *pArcPart=NULL;
		if(pItem->mirInfo.IsSameMirMsg(mirmsg))
		{
			pArcPart=(CLDSArcPart*)BelongModel()->FromPartHandle(pItem->hObj,CLS_ARCPART);
			if(pArcPart)	//此处未能保证杆件类型一致 
				return pArcPart;
			else
				break;
		}
	}
	//两环形构件 如果起始点，起始偏转角以及扇形角相同则可认为位置相同
	CLDSArcPart *pMirArcPart=NULL;
	f3dPoint mir_arc_start=GetMirPos(arc_start,mirmsg);
	int cls_typeid=GetClassTypeId();
	BOOL pushed=m_pModel->PushPartStack(); //遍历构件列表前应先压栈 wht 12-03-05
	for(pMirArcPart=(CLDSArcPart*)BelongModel()->EnumPartFirst(cls_typeid);pMirArcPart;pMirArcPart=(CLDSArcPart*)BelongModel()->EnumPartNext(cls_typeid))
	{
		if(!pMirArcPart->cfgword.And(cfgword))
			continue;
		if(fabs(pMirArcPart->baseline.SectorAngle()-baseline.SectorAngle())<EPS)
			continue;	//圆弧扇形角不同
		if(fabs(pMirArcPart->baseline.WorkNorm()*baseline.WorkNorm())<EPS_COS)
			continue;	//法线方向不在同一直线上
		if(pMirArcPart->baseline.WorkNorm()*baseline.WorkNorm()>EPS)
		{	//法线方向相同
			if(pMirArcPart->arc_start==mir_arc_start)
			{	//起始点相同
				if(fabs(pMirArcPart->start_rot_angle-start_rot_angle)<EPS)
					break;	//起始偏转角相同
			}
			else
			{	//起始点不同
				if(fabs(pMirArcPart->start_rot_angle-start_rot_angle)>EPS)
				{
					double fOffsetAngle=pMirArcPart->start_rot_angle-start_rot_angle;
					f3dPoint start=mir_arc_start;
					rotate_point_around_axis(start,fOffsetAngle,baseline.Center(),baseline.Center()+100*baseline.WorkNorm());
					if(start=pMirArcPart->arc_start)
						break;
				}
			}
		}
		else	//法线方向相反
		{
			if(pMirArcPart->arc_start==mir_arc_start)
			{	//起始点相同
				//法线方向相反起始点相同的两构件如果起始偏转角之和等于负的圆弧扇形角则认为两构件相同
				if(fabs(pMirArcPart->start_rot_angle+start_rot_angle+baseline.SectorAngle())<EPS)
					break;
			}
			else
			{	//起始点不同
				//法线方向相反起始点不相同的两构件A,B 如果A的起始点绕自己的法线方向
				//旋转（A->start_rot_angle+B->start_rot_angle+SectorAngle()）°与B起始点重合，则认为两构件相同
				double fOffsetAngle=pMirArcPart->start_rot_angle+start_rot_angle+baseline.SectorAngle();
				f3dPoint start=mir_arc_start;
				rotate_point_around_axis(start,fOffsetAngle,baseline.Center(),baseline.Center()+100*baseline.WorkNorm());
				if(start==pMirArcPart->arc_start)
					break;
			}
		}
	}
	m_pModel->PopPartStack(pushed);
	return pMirArcPart;
}

void CLDSArcPart::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/,bool bAddSubLevelRelaObj/*=true*/)
{
	if(_console==NULL||m_pModel==NULL||!Lock())	//已调用过)
		return;
	_console->AddToObjSetBuf(this);
	for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		pLsRef->GetLsPtr()->AddToConsoleObjBuf(bAddRelaObj);
}
void CLDSArcPart::getUCS(UCS_STRU& ucs)
{
	ucs.origin=baseline.Center();
	ucs.axis_x=f3dPoint(baseline.PositionInAngle(0))-ucs.origin;
	ucs.axis_z=baseline.WorkNorm();
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	normalize(ucs.axis_x);
	normalize(ucs.axis_y);
	normalize(ucs.axis_z);
}
GEPOINT CLDSArcPart::GetArcRayNorm(const double* point)		//弧形杆件在pNode节点处的射线方向(圆弧切面法线)
{
	GEPOINT rayNormal;
	f3dPoint pick(point);
	project_point(pick,center,worknorm);
	rayNormal=pick-f3dPoint(center);
	Standarized(rayNormal);
	return rayNormal;
}
GEPOINT CLDSArcPart::GetArcTangentVec(const double* point)	//弧形杆件在pNode节点处的切线方向(长度方向切矢量)
{
	f3dPoint pick(point);
	project_point(pick,center,worknorm);
	GEPOINT rayNormal,tangent;
	rayNormal=pick-f3dPoint(center);
	normalize(rayNormal);
	tangent=worknorm^rayNormal;
	Standarized(tangent);
	return tangent;
}

CLDSArcAngle::CLDSArcAngle()
{
	//strcpy(m_sClassName,"CLDSArcAngle");
	m_nClassTypeId=CLS_ARCANGLE;
	m_uDisplaySlices=16;
	LoadDefaultUCS(&ucs);
	m_uStatMatCoef=1;
	m_bVirtualPart = false;
	m_bDisplayHole=false;
	strcpy(sPartNo,"");
	m_hPartWeldParent=0;
	m_fBaseLineRadius=0;
	wing_wide=56;
	wing_thick=4;
	offset_h=-28;
	offset_v=0;
	pSolidBody=NULL;
}

int CLDSArcAngle::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;//[100]={""};
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
	else if(GetPropID("m_uStatMatCoef")==id)
		sprintf(sText,"%d",m_uStatMatCoef);
	else if(GetPropID("m_fZoomLenCoef")==id)
	{
		sprintf(sText,"%f",m_fZoomLenCoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_hPartWeldParent")==id)
		sprintf(sText,"0X%X",m_hPartWeldParent);
	else if(GetPropID("m_bDisplayHole")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bDisplayHole)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
		if(m_bDisplayHole)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("m_bVirtualPart")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bVirtualPart)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
		if(m_bVirtualPart)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("m_uDisplaySlices")==id)
		sprintf(sText,"%d",m_uDisplaySlices);
	else if(GetPropID("m_fBaseLineRadius")==id)
	{
		sprintf(sText,"%f",m_fBaseLineRadius);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("arc_radius")==id)
	{
		sprintf(sText,"%f",baseline.Radius());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("offset_h")==id)
	{
		sprintf(sText,"%f",offset_h);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("offset_v")==id)
	{
		sprintf(sText,"%f",offset_v);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("wing_thick")==id)
	{
		sprintf(sText,"%f",wing_thick);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("wing_wide")==id)
	{
		sprintf(sText,"%f",wing_wide);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("specification")==id)
	{	
		char s_wing_wide[200],s_wing_thick[200];
		sprintf(s_wing_wide,"%f",wing_wide);
		sprintf(s_wing_thick,"%f",wing_thick);
		SimplifiedNumString(s_wing_wide);
		SimplifiedNumString(s_wing_thick);
		sprintf(sText,"%sX%s", s_wing_wide,s_wing_thick);
	}
	else if(GetPropID("arc_start.x")==id)
	{
		sprintf(sText,"%f",arc_start.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("arc_start.y")==id)
	{
		sprintf(sText,"%f",arc_start.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("arc_start.z")==id)
	{
		sprintf(sText,"%f",arc_start.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("start_rot_angle")==id)
	{
		sprintf(sText,"%f",start_rot_angle*DEGTORAD_COEF);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("sector_angle")==id)
	{
		sprintf(sText,"%f",sector_angle*DEGTORAD_COEF);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("start_oddment")==id)
	{
		sprintf(sText,"%f",start_oddment);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("end_oddment")==id)
	{
		sprintf(sText,"%f",end_oddment);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_bUserSpecColor")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bUserSpecColor)
			sprintf(sText,"Yes");
		else
			sprintf(sText,"No");
#else
		if(m_bUserSpecColor)
			sprintf(sText,"是");
		else
			sprintf(sText,"否");
#endif
	}
	else if(GetPropID("crMaterial")==id)
		sprintf(sText,"RGB%X",crMaterial);
	else if(GetPropID("sNotes")==id)
		strcpy(sText,sNotes);
	else if(GetPropID("center.x")==id)
	{
		sprintf(sText,"%f",baseline.Center().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("center.y")==id)
	{
		sprintf(sText,"%f",baseline.Center().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("center.z")==id)
	{
		sprintf(sText,"%f",baseline.Center().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("workNorm.x")==id)
	{
		sprintf(sText,"%f",baseline.WorkNorm().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("workNorm.y")==id)
	{
		sprintf(sText,"%f",baseline.WorkNorm().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("workNorm.z")==id)
	{
		sprintf(sText,"%f",baseline.WorkNorm().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.x")==id)
	{
		sprintf(sText,"%f",ucs.origin.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.y")==id)
	{
		sprintf(sText,"%f",ucs.origin.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.z")==id)
	{
		sprintf(sText,"%f",ucs.origin.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_arc_norm")==id)
		sText.ConvertFromPoint(des_arc_norm.vector);
	else if(GetPropID("des_arc_center")==id)
		sText.ConvertFromPoint(des_arc_center.Position());
	else if(GetPropID("des_arc_start")==id)
		sText.ConvertFromPoint(des_arc_start.Position());
	else if(GetPropID("hStart")==id)
	{
		if(pStart)
			sprintf(sText,"0X%X",pStart->handle);
		else
			strcpy(sText,"0X0");
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
	else if(GetPropID("hEnd")==id)
	{
		if(pEnd)
			sprintf(sText,"0X%X",pEnd->handle);
		else
			strcpy(sText,"0X0");
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
	else
		strcpy(sText,"");
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
long CLDSArcAngle::GetPropID(char* propStr)
{
	PROPLIST_ITEM *pItem =CLDSArcAngle::propHashtable.GetValue(propStr);
	if(pItem!=NULL)
		return pItem->id;
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		/*CString prompt="Did not find a property ID named\"";
		prompt+=propStr;
		prompt+="\"!";
		AfxMessageBox(prompt);*/
#else
		/*CString prompt="没找到名为\"";
		prompt+=propStr;
		prompt+="\"的属性ID！";
		AfxMessageBox(prompt);*/
#endif
		return 0;
	}
}
int CLDSArcAngle::GetPropStr(long id,char* propStr)
{
	char sText[200]="";
	if(GetPropID("basicInfo")==0)
		strcpy(sText,"basicInfo");
	else if(GetPropID("handle")==id)
		strcpy(sText,"handle");
	else if(GetPropID("layer")==id)
		strcpy(sText,"layer");
	else if(GetPropID("cMaterial")==id)
		strcpy(sText,"cMaterial");
	else if(GetPropID("m_cQualityLevel")==id)
		strcpy(sText,"m_cQualityLevel");
	else if(GetPropID("iSeg")==id)
		strcpy(sText,"iSeg");
	else if(GetPropID("sPartNo")==id)
		strcpy(sText,"sPartNo");
	else if(GetPropID("cfgword")==id)
		strcpy(sText,"cfgword");
	else if(GetPropID("m_uStatMatCoef")==id)
		strcpy(sText,"m_uStatMatCoef");
	else if(GetPropID("m_hPartWeldParent")==id)
		strcpy(sText,"m_hPartWeldParent");
	else if(GetPropID("m_bDisplayHole")==id)
		strcpy(sText,"m_bDisplayHole");
	else if(GetPropID("m_bVirtualPart")==id)
		strcpy(sText,"m_bVirtualPart");
	else if(GetPropID("m_uDisplaySlices")==id)
		strcpy(sText,"m_uDisplaySlices");
	else if(GetPropID("m_fBaseLineRadius")==id)
		strcpy(sText,"m_fBaseLineRadius");
	else if(GetPropID("m_fZoomLenCoef")==id)
		strcpy(sText,"m_fZoomLenCoef");
	else if(GetPropID("arc_radius")==id)
		strcpy(sText,"arc_radius");
	else if(GetPropID("offset_h")==id)
		strcpy(sText,"offset_h");
	else if(GetPropID("offset_v")==id)
		strcpy(sText,"offset_v");
	else if(GetPropID("wing_thick")==id)
		strcpy(sText,"wing_thick");
	else if(GetPropID("wing_wide")==id)
		strcpy(sText,"wing_wide");
	else if(GetPropID("sNotes")==id)
		strcpy(sText,"sNotes");
	else if(GetPropID("assemblePos")==id)
		strcpy(sText,"assemblePos");
	else if(GetPropID("ucs.origin")==id)
		strcpy(sText,"ucs.origin");
	else if(GetPropID("ucs.origin.x")==id)
		strcpy(sText,"ucs.origin.x");
	else if(GetPropID("ucs.origin.y")==id)
		strcpy(sText,"ucs.origin.y");
	else if(GetPropID("ucs.origin.z")==id)
		strcpy(sText,"ucs.origin.z");
	else if(GetPropID("ucs.axis_x")==id)
		strcpy(sText,"ucs.axis_x");
	else if(GetPropID("ucs.axis_x.x")==id)
		strcpy(sText,"ucs.axis_x.x");
	else if(GetPropID("ucs.axis_x.y")==id)
		strcpy(sText,"ucs.axis_x.y");
	else if(GetPropID("ucs.axis_x.z")==id)
		strcpy(sText,"ucs.axis_x.z");
	else if(GetPropID("ucs.axis_y")==id)
		strcpy(sText,"ucs.axis_y");
	else if(GetPropID("ucs.axis_y.x")==id)
		strcpy(sText,"ucs.axis_y.x");
	else if(GetPropID("ucs.axis_y.y")==id)
		strcpy(sText,"ucs.axis_y.y");
	else if(GetPropID("ucs.axis_y.z")==id)
		strcpy(sText,"ucs.axis_y.z");
	else if(GetPropID("ucs.axis_z")==id)
		strcpy(sText,"ucs.axis_z");
	else if(GetPropID("ucs.axis_z.x")==id)
		strcpy(sText,"ucs.axis_z.x");
	else if(GetPropID("ucs.axis_z.y")==id)
		strcpy(sText,"ucs.axis_z.y");
	else if(GetPropID("ucs.axis_z.z")==id)
		strcpy(sText,"ucs.axis_z.z");
	if(propStr)
		strcpy(propStr,sText);
	return strlen(sText);
}
BOOL CLDSArcAngle::IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSArcAngle *pPart=NULL, *pFirstPart=(CLDSArcAngle*)selectObjs.GetFirst();
	//pPart->GetPropStr(idProp);
	pFirstPart->GetPropValueStr(idProp,valueStr);
	for(pPart=(CLDSArcAngle*)selectObjs.GetNext();pPart;pPart=(CLDSArcAngle*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pPart->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

int CLDSArcAngle::GetPropStatus(long id)
{	
	DWORD dwStatus;
	if(CLDSArcAngle::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
}

BOOL CLDSArcAngle::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSArcAngle::propStatusHashtable.SetValueAt(id,dwStatus);
}

void CLDSArcAngle::IndependentShadowPropFromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{

}
void CLDSArcAngle::IndependentShadowPropToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{

}
void CLDSArcAngle::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//保存影射构件参数和构件从属句柄 wht 12-09-14
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if(ShadowParaAndBlockIdToBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropToBuffer(buffer,version,doc_type);
		return;
	}
	CLDSArcPart::ToBuffer(buffer);
	buffer.WriteDouble(wing_wide);	//肢宽
	buffer.WriteDouble(wing_thick);	//肢厚
}

void CLDSArcAngle::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="";
	fprintf(fp,"<环形角钢>\n");
	CLDSArcPart::ToXmlFile(fp,schema);
	sprintf(sText,"%f",wing_wide);
	SimplifiedNumString(sText);
	fprintf(fp,"<肢宽 wing_wide=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wing_thick);
	SimplifiedNumString(sText);
	fprintf(fp,"<肢厚 wing_thick=\"%s\"/>\n",sText);
	fprintf(fp,"</环形角钢>\n");
}

void CLDSArcAngle::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_tma1_lma2_tsa3_lds4_default0/*=0*/)
{
	if(doc_tma1_lma2_tsa3_lds4_default0==0)
		doc_tma1_lma2_tsa3_lds4_default0=PRODUCT_TYPE;
	//读取影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_tma1_lma2_tsa3_lds4_default0))
	{
		IndependentShadowPropFromBuffer(buffer,version,doc_tma1_lma2_tsa3_lds4_default0);
		return;
	}
	CLDSArcPart::FromBuffer(buffer,version,doc_tma1_lma2_tsa3_lds4_default0);
	buffer.ReadDouble(&wing_wide);	//肢宽
	buffer.ReadDouble(&wing_thick);	//肢厚
}

bool CLDSArcAngle::IsEqual(CLDSArcAngle* pMirAngle,double epsilon/*=1.0*/,char *error_info/*=NULL*/,
						   int *pErrorInfo/*=NULL*/,DWORD *pChkStyleFlag/*=NULL*/)
{
	if(pChkStyleFlag==NULL||(*pChkStyleFlag)&CHKPARTNO_RODINFO)
	{
		bool bRet=true;
		if(fabs(baseline.SectorAngle()-pMirAngle->baseline.SectorAngle())>epsilon)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Sector angle is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}扇区角度不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(baseline.Radius()-pMirAngle->baseline.Radius()!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Angle is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}角度不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(wing_wide-pMirAngle->wing_wide!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Width is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}宽度不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(wing_thick-pMirAngle->wing_thick!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Thickness is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}厚度不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(cMaterial!=pMirAngle->cMaterial)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Material is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}材质不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(offset_h-pMirAngle->offset_h!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Radial offset is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}横向位移不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(offset_v-pMirAngle->offset_v!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Normal offset is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}竖向位移不同",(char*)sPartNo);
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
		UCS_STRU tem_ucs = ucs;
		UCS_STRU tem_vice_ucs = pMirAngle->ucs;
		char sError[200]="";
		if(!IsLsRefListEqual( pLsRefList,tem_ucs,
			pMirAngle->pLsRefList,tem_vice_ucs,epsilon,sError,NULL))
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

double CLDSArcAngle::GetLength()
{
	return baseline.Radius()*baseline.SectorAngle();
}

double CLDSArcAngle::GetWeight(BOOL bSolidLength/*=TRUE*/)
{
	double v=(wing_wide*wing_wide-(wing_wide-wing_thick)*(wing_wide-wing_thick))*GetLength();
	return CalSteelPlankWei(v);
}

int CLDSArcAngle::GetSpecification(char *spec,BOOL bIncMat/*=TRUE*/)
{
	char steelmark[20]="";
	if(bIncMat&&cMaterial!='S')
		QuerySteelMatMark(cMaterial,steelmark);
	char simple_spec[200]="";
	CLDSLineAngle::GetAngleSimpleSpec(simple_spec,wing_wide,wing_wide,wing_thick);
	char tmp_spec[200]="";
	sprintf(tmp_spec,"%s L%s",steelmark,simple_spec);
	if(spec)
		strcpy(spec,tmp_spec);
	return strlen(tmp_spec);
}

BOOL CLDSArcAngle::Create3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	if(CreateShadowPart3dSolidModel(bDisplayHole,scale_of_user2scr,sample_len,smoothness))
		return TRUE;

	if(pSolidBody==NULL)
		pSolidBody = new CSolidBody;
	else if(!IsModified())	//数据未修改
		return TRUE;
	else
		pSolidBody->Empty();
	SetModified(FALSE);
	fBody body;
	fBody *pBody=&body;
	getUCS(ucs);
	f3dPoint start(baseline.Radius()+offset_h,0,offset_v);
	//计算正负头影响
	double start_oddment_angle=0.0,end_oddment_angle=0.0;
	start_oddment_angle=start_oddment/baseline.Radius();
	end_oddment_angle=end_oddment/baseline.Radius();
	if(fabs(start_oddment_angle)>EPS)
		rotate_point_around_axis(start,-start_oddment_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
	int i;
	f3dPoint vertex_arr[6];
	COLORREF crArcAngle=GetColor(CLS_ARCANGLE,3,cMaterial);
	if(m_bUserSpecColor)	//用户指定构件颜色
		crArcAngle=crMaterial;
	double radius = baseline.Radius();
	double sector_angle=baseline.SectorAngle()+start_oddment_angle+end_oddment_angle;
	int uSlices = CalArcResolution(radius,sector_angle,scale_of_user2scr,sample_len,smoothness);
	uSlices = max(uSlices,(int)m_uDisplaySlices);
	double piece_angle=sector_angle/uSlices;
	for(i=0;i<=uSlices;i++)
	{
		vertex_arr[0]=start;
		double angle=i*piece_angle;
		double cosa = cos(angle-start_oddment_angle);
		double sina = sin(angle-start_oddment_angle);
		if(i>0)
			rotate_point_around_axis(vertex_arr[0],angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
		if(m_siLayoutStyle==0)
		{	//┛模式
			vertex_arr[1]=vertex_arr[0]+f3dPoint(0,0,wing_wide);
			vertex_arr[2]=vertex_arr[1]-wing_thick*f3dPoint(cosa,sina);
			vertex_arr[3]=vertex_arr[2]-f3dPoint(0,0,wing_wide-wing_thick);
			vertex_arr[4]=vertex_arr[3]-(wing_wide-wing_thick)*f3dPoint(cosa,sina);
			vertex_arr[5]=vertex_arr[0]-wing_wide*f3dPoint(cosa,sina);
		}
		else if(m_siLayoutStyle==1)
		{	//┓模式
			vertex_arr[1]=vertex_arr[0]-wing_wide*f3dPoint(cosa,sina);
			vertex_arr[2]=vertex_arr[1]-f3dPoint(0,0,wing_thick);
			vertex_arr[3]=vertex_arr[2]+(wing_wide-wing_thick)*f3dPoint(cosa,sina);
			vertex_arr[4]=vertex_arr[3]-f3dPoint(0,0,wing_wide-wing_thick);
			vertex_arr[5]=vertex_arr[0]-f3dPoint(0,0,wing_wide);
		}
		else if(m_siLayoutStyle==2)
		{	//┏模式
			vertex_arr[1]=vertex_arr[0]-f3dPoint(0,0,wing_wide);
			vertex_arr[2]=vertex_arr[1]+wing_thick*f3dPoint(cosa,sina);
			vertex_arr[3]=vertex_arr[2]+f3dPoint(0,0,wing_wide-wing_thick);
			vertex_arr[4]=vertex_arr[3]+(wing_wide-wing_thick)*f3dPoint(cosa,sina);
			vertex_arr[5]=vertex_arr[0]+wing_wide*f3dPoint(cosa,sina);
		}
		else if(m_siLayoutStyle==3)
		{	//┗模式
			vertex_arr[1]=vertex_arr[0]+wing_wide*f3dPoint(cosa,sina);
			vertex_arr[2]=vertex_arr[1]+f3dPoint(0,0,wing_thick);
			vertex_arr[3]=vertex_arr[2]-(wing_wide-wing_thick)*f3dPoint(cosa,sina);
			vertex_arr[4]=vertex_arr[3]+f3dPoint(0,0,wing_wide-wing_thick);
			vertex_arr[5]=vertex_arr[0]+f3dPoint(0,0,wing_wide);
		}
		for(int j=0;j<6;j++)
			pBody->vertex.append(vertex_arr[j]);
	}
	SetModified(FALSE);
	//初始化链表
	CCreateFace createFace;
	createFace.InitVertexList(pBody);

	f3dPolyFace *pFace=NULL;
	//始端端截面
	pFace=pBody->faceList.append();
	pFace->material = crArcAngle;
	createFace.NewOutterEdgeLine(pFace,1,0);
	createFace.NewOutterEdgeLine(pFace,2);
	createFace.NewOutterEdgeLine(pFace,3);
	createFace.NewOutterEdgeLine(pFace,4);
	createFace.NewOutterEdgeLine(pFace,5);
	createFace.NewOutterEdgeLine(pFace,0);
	for(i=0;i<uSlices;i++)
	{
		int base_index=i*6,next_index=i*6+6;
		//Y肢底面创建
		pFace=pBody->faceList.append();
		pFace->material = crArcAngle;
		createFace.NewOutterEdgeLine(pFace,base_index,base_index+1);
		createFace.NewOutterEdgeLine(pFace,next_index);
		createFace.NewOutterEdgeLine(pFace,next_index+1);
		createFace.NewOutterEdgeLine(pFace,base_index+1);
		//Y肢右侧面创建
		pFace=pBody->faceList.append();
		pFace->material = crArcAngle;
		createFace.NewOutterEdgeLine(pFace,next_index+1,base_index+1);
		createFace.NewOutterEdgeLine(pFace,next_index+2);
		createFace.NewOutterEdgeLine(pFace,base_index+2);
		createFace.NewOutterEdgeLine(pFace,base_index+1);
		//Y肢顶面创建
		pFace=pBody->faceList.append();
		pFace->material = crArcAngle;
		createFace.NewOutterEdgeLine(pFace,next_index+2,base_index+2);
		createFace.NewOutterEdgeLine(pFace,next_index+3);
		createFace.NewOutterEdgeLine(pFace,base_index+3);
		createFace.NewOutterEdgeLine(pFace,base_index+2);
		//X肢右面创建
		pFace=pBody->faceList.append();
		pFace->material = crArcAngle;
		createFace.NewOutterEdgeLine(pFace,next_index+3,base_index+3);
		createFace.NewOutterEdgeLine(pFace,next_index+4);
		createFace.NewOutterEdgeLine(pFace,base_index+4);
		createFace.NewOutterEdgeLine(pFace,base_index+3);
		//X肢顶面创建
		pFace=pBody->faceList.append();
		pFace->material = crArcAngle;
		createFace.NewOutterEdgeLine(pFace,next_index+4,base_index+4);
		createFace.NewOutterEdgeLine(pFace,next_index+5);
		createFace.NewOutterEdgeLine(pFace,base_index+5);
		createFace.NewOutterEdgeLine(pFace,base_index+4);
		//X肢左面创建
		pFace=pBody->faceList.append();
		pFace->material = crArcAngle;
		createFace.NewOutterEdgeLine(pFace,next_index+5,base_index+5);
		createFace.NewOutterEdgeLine(pFace,next_index);
		createFace.NewOutterEdgeLine(pFace,base_index);
		createFace.NewOutterEdgeLine(pFace,base_index+5);
	}
	//终端端截面
	pFace=pBody->faceList.append();
	pFace->material = crArcAngle;
	createFace.NewOutterEdgeLine(pFace,uSlices*6+4,uSlices*6+5);
	createFace.NewOutterEdgeLine(pFace,uSlices*6+3);
	createFace.NewOutterEdgeLine(pFace,uSlices*6+2);
	createFace.NewOutterEdgeLine(pFace,uSlices*6+1);
	createFace.NewOutterEdgeLine(pFace,uSlices*6);
	createFace.NewOutterEdgeLine(pFace,uSlices*6+5);
	for(f3dPoint *pVertex=pBody->vertex.GetFirst();pVertex;pVertex=pBody->vertex.GetNext())
	{
		coord_trans(*pVertex,ucs,TRUE);
	}
	pSolidBody->ConvertFrom(pBody);
	return TRUE;
}


CLDSArcFlat::CLDSArcFlat()
{
	//strcpy(m_sClassName,"CLDSArcFlat");
	m_nClassTypeId=CLS_ARCFLAT;
	m_uDisplaySlices=16;
	LoadDefaultUCS(&ucs);
	m_uStatMatCoef=1;
	m_bVirtualPart = false;
	m_bDisplayHole=false;
	strcpy(sPartNo,"");
	m_hPartWeldParent=0;
	m_fBaseLineRadius=0;
	wide=200;
	thick=4;
	offset_h=-100;
	offset_v=0;
	pSolidBody=NULL;
}

int CLDSArcFlat::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;//[100]={""};
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
	else if(GetPropID("m_uStatMatCoef")==id)
		sprintf(sText,"%d",m_uStatMatCoef);
	else if(GetPropID("m_fZoomLenCoef")==id)
	{
		sprintf(sText,"%f",m_fZoomLenCoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_hPartWeldParent")==id)
		sprintf(sText,"0X%X",m_hPartWeldParent);
	else if(GetPropID("m_bDisplayHole")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bDisplayHole)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
		if(m_bDisplayHole)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("m_bVirtualPart")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bVirtualPart)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
		if(m_bVirtualPart)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("m_uDisplaySlices")==id)
		sprintf(sText,"%d",m_uDisplaySlices);
	else if(GetPropID("m_fBaseLineRadius")==id)
	{
		sprintf(sText,"%f",m_fBaseLineRadius);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("arc_radius")==id)
	{
		sprintf(sText,"%f",baseline.Radius());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("offset_h")==id)
	{
		sprintf(sText,"%f",offset_h);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("offset_v")==id)
	{
		sprintf(sText,"%f",offset_v);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("thick")==id)
	{
		sprintf(sText,"%f",thick);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("wide")==id)
	{
		sprintf(sText,"%f",wide);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("specification")==id)
	{	
		char s_wing_wide[200],s_wing_thick[200];
		sprintf(s_wing_wide,"%f",wide);
		sprintf(s_wing_thick,"%f",thick);
		SimplifiedNumString(s_wing_wide);
		SimplifiedNumString(s_wing_thick);
		sprintf(sText,"%sX%s", s_wing_wide,s_wing_thick);
	}
	else if(GetPropID("arc_start.x")==id)
	{
		sprintf(sText,"%f",arc_start.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("arc_start.y")==id)
	{
		sprintf(sText,"%f",arc_start.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("arc_start.z")==id)
	{
		sprintf(sText,"%f",arc_start.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("start_rot_angle")==id)
	{
		sprintf(sText,"%f",start_rot_angle*DEGTORAD_COEF);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("sector_angle")==id)
	{
		sprintf(sText,"%f",sector_angle*DEGTORAD_COEF);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("start_oddment")==id)
	{
		sprintf(sText,"%f",start_oddment);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("end_oddment")==id)
	{
		sprintf(sText,"%f",end_oddment);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_bUserSpecColor")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bUserSpecColor)
			sprintf(sText,"Yes");
		else
			sprintf(sText,"No");
#else
		if(m_bUserSpecColor)
			sprintf(sText,"是");
		else
			sprintf(sText,"否");
#endif
	}
	else if(GetPropID("crMaterial")==id)
		sprintf(sText,"RGB%X",crMaterial);
	else if(GetPropID("sNotes")==id)
		strcpy(sText,sNotes);
	else if(GetPropID("center.x")==id)
	{
		sprintf(sText,"%f",baseline.Center().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("center.y")==id)
	{
		sprintf(sText,"%f",baseline.Center().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("center.z")==id)
	{
		sprintf(sText,"%f",baseline.Center().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("workNorm.x")==id)
	{
		sprintf(sText,"%f",baseline.WorkNorm().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("workNorm.y")==id)
	{
		sprintf(sText,"%f",baseline.WorkNorm().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("workNorm.z")==id)
	{
		sprintf(sText,"%f",baseline.WorkNorm().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.x")==id)
	{
		sprintf(sText,"%f",ucs.origin.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.y")==id)
	{
		sprintf(sText,"%f",ucs.origin.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.z")==id)
	{
		sprintf(sText,"%f",ucs.origin.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_arc_norm")==id)
		sText.ConvertFromPoint(des_arc_norm.vector);
	else if(GetPropID("des_arc_center")==id)
		sText.ConvertFromPoint(des_arc_center.Position());
	else if(GetPropID("des_arc_start")==id)
		sText.ConvertFromPoint(des_arc_start.Position());
	else if(GetPropID("hStart")==id)
	{
		if(pStart)
			sprintf(sText,"0X%X",pStart->handle);
		else
			strcpy(sText,"0X0");
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
	else if(GetPropID("hEnd")==id)
	{
		if(pEnd)
			sprintf(sText,"0X%X",pEnd->handle);
		else
			strcpy(sText,"0X0");
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
	else
		strcpy(sText,"");
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
long CLDSArcFlat::GetPropID(char* propStr)
{
	PROPLIST_ITEM *pItem =CLDSArcFlat::propHashtable.GetValue(propStr);
	if(pItem!=NULL)
		return pItem->id;
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		/*CString prompt="Did not find a property ID named\"";
		prompt+=propStr;
		prompt+="\"!";
		AfxMessageBox(prompt);*/
#else
		/*CString prompt="没找到名为\"";
		prompt+=propStr;
		prompt+="\"的属性ID！";
		AfxMessageBox(prompt);*/
#endif
		return 0;
	}
}
int CLDSArcFlat::GetPropStr(long id,char* propStr)
{
	char sText[200]="";
	if(GetPropID("basicInfo")==0)
		strcpy(sText,"basicInfo");
	else if(GetPropID("handle")==id)
		strcpy(sText,"handle");
	else if(GetPropID("layer")==id)
		strcpy(sText,"layer");
	else if(GetPropID("cMaterial")==id)
		strcpy(sText,"cMaterial");
	else if(GetPropID("m_cQualityLevel")==id)
		strcpy(sText,"m_cQualityLevel");
	else if(GetPropID("iSeg")==id)
		strcpy(sText,"iSeg");
	else if(GetPropID("sPartNo")==id)
		strcpy(sText,"sPartNo");
	else if(GetPropID("cfgword")==id)
		strcpy(sText,"cfgword");
	else if(GetPropID("m_uStatMatCoef")==id)
		strcpy(sText,"m_uStatMatCoef");
	else if(GetPropID("m_hPartWeldParent")==id)
		strcpy(sText,"m_hPartWeldParent");
	else if(GetPropID("m_bDisplayHole")==id)
		strcpy(sText,"m_bDisplayHole");
	else if(GetPropID("m_bVirtualPart")==id)
		strcpy(sText,"m_bVirtualPart");
	else if(GetPropID("m_uDisplaySlices")==id)
		strcpy(sText,"m_uDisplaySlices");
	else if(GetPropID("m_fBaseLineRadius")==id)
		strcpy(sText,"m_fBaseLineRadius");
	else if(GetPropID("m_fZoomLenCoef")==id)
		strcpy(sText,"m_fZoomLenCoef");
	else if(GetPropID("arc_radius")==id)
		strcpy(sText,"arc_radius");
	else if(GetPropID("offset_h")==id)
		strcpy(sText,"offset_h");
	else if(GetPropID("offset_v")==id)
		strcpy(sText,"offset_v");
	else if(GetPropID("thick")==id)
		strcpy(sText,"thick");
	else if(GetPropID("wide")==id)
		strcpy(sText,"wide");
	else if(GetPropID("sNotes")==id)
		strcpy(sText,"sNotes");
	else if(GetPropID("assemblePos")==id)
		strcpy(sText,"assemblePos");
	else if(GetPropID("ucs.origin")==id)
		strcpy(sText,"ucs.origin");
	else if(GetPropID("ucs.origin.x")==id)
		strcpy(sText,"ucs.origin.x");
	else if(GetPropID("ucs.origin.y")==id)
		strcpy(sText,"ucs.origin.y");
	else if(GetPropID("ucs.origin.z")==id)
		strcpy(sText,"ucs.origin.z");
	else if(GetPropID("ucs.axis_x")==id)
		strcpy(sText,"ucs.axis_x");
	else if(GetPropID("ucs.axis_x.x")==id)
		strcpy(sText,"ucs.axis_x.x");
	else if(GetPropID("ucs.axis_x.y")==id)
		strcpy(sText,"ucs.axis_x.y");
	else if(GetPropID("ucs.axis_x.z")==id)
		strcpy(sText,"ucs.axis_x.z");
	else if(GetPropID("ucs.axis_y")==id)
		strcpy(sText,"ucs.axis_y");
	else if(GetPropID("ucs.axis_y.x")==id)
		strcpy(sText,"ucs.axis_y.x");
	else if(GetPropID("ucs.axis_y.y")==id)
		strcpy(sText,"ucs.axis_y.y");
	else if(GetPropID("ucs.axis_y.z")==id)
		strcpy(sText,"ucs.axis_y.z");
	else if(GetPropID("ucs.axis_z")==id)
		strcpy(sText,"ucs.axis_z");
	else if(GetPropID("ucs.axis_z.x")==id)
		strcpy(sText,"ucs.axis_z.x");
	else if(GetPropID("ucs.axis_z.y")==id)
		strcpy(sText,"ucs.axis_z.y");
	else if(GetPropID("ucs.axis_z.z")==id)
		strcpy(sText,"ucs.axis_z.z");
	if(propStr)
		strcpy(propStr,sText);
	return strlen(sText);
}
BOOL CLDSArcFlat::IsPropEqual(HASHOBJSET &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSArcFlat *pPart=NULL, *pFirstPart=(CLDSArcFlat*)selectObjs.GetFirst();
	pFirstPart->GetPropValueStr(idProp,valueStr);
	for(pPart=(CLDSArcFlat*)selectObjs.GetNext();pPart;pPart=(CLDSArcFlat*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pPart->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

int CLDSArcFlat::GetPropStatus(long id)
{	
	DWORD dwStatus;
	if(CLDSArcFlat::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
}

BOOL CLDSArcFlat::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSArcFlat::propStatusHashtable.SetValueAt(id,dwStatus);
}

double CLDSArcFlat::GetLength()
{
	return baseline.Radius()*baseline.SectorAngle();
}

double CLDSArcFlat::GetWeight(BOOL bSolidLength/*=TRUE*/)
{
	double v=GetWidth()*GetThick()*GetLength();
	return CalSteelPlankWei(v);
}
int CLDSArcFlat::GetSpecification(char *spec,BOOL bIncMat/*=TRUE*/)
{
	char steelmark[20]="";
	char tmp_spec[200],tmp_spec2[200];
	if(bIncMat&&cMaterial!='S')
		QuerySteelMatMark(cMaterial,steelmark);
	sprintf(tmp_spec,"%s-%f",steelmark,wide);
	SimplifiedNumString(tmp_spec);
	_snprintf(tmp_spec2,199,"%sx%f",tmp_spec,thick);
	SimplifiedNumString(tmp_spec2);
	if(spec)
		strcpy(spec,tmp_spec2);
	return strlen(tmp_spec2);
}
void CLDSArcFlat::IndependentShadowPropFromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{

}
void CLDSArcFlat::IndependentShadowPropToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{

}

void CLDSArcFlat::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//保存影射构件参数和构件从属句柄 wht 12-09-14
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if(ShadowParaAndBlockIdToBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropToBuffer(buffer,version,doc_type);
		return;
	}
	CLDSArcPart::ToBuffer(buffer);
	buffer.WriteDouble(wide);	//宽
	buffer.WriteDouble(thick);	//厚
}

void CLDSArcFlat::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="";
	fprintf(fp,"<环形扁铁>\n");
	CLDSArcPart::ToXmlFile(fp,schema);
	sprintf(sText,"%f",wide);
	SimplifiedNumString(sText);
	fprintf(fp,"<宽 wing_wide=\"%s\"/>\n",sText);
	sprintf(sText,"%f",thick);
	SimplifiedNumString(sText);
	fprintf(fp,"<厚 wing_thick=\"%s\"/>\n",sText);
	fprintf(fp,"</环形扁铁>\n");

}
void CLDSArcFlat::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_tma1_lma2_tsa3_lds4_default0/*=0*/)
{
	if(doc_tma1_lma2_tsa3_lds4_default0==0)
		doc_tma1_lma2_tsa3_lds4_default0=PRODUCT_TYPE;
	//读取影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_tma1_lma2_tsa3_lds4_default0))
	{
		IndependentShadowPropFromBuffer(buffer,version,doc_tma1_lma2_tsa3_lds4_default0);
		return;
	}
	CLDSArcPart::FromBuffer(buffer,version,doc_tma1_lma2_tsa3_lds4_default0);
	buffer.ReadDouble(&wide);	//宽
	buffer.ReadDouble(&thick);	//厚
}

bool CLDSArcFlat::IsEqual(CLDSArcFlat* pMirFlat,double epsilon/*=1.0*/,char *error_info/*=NULL*/,
						   int *pErrorInfo/*=NULL*/,DWORD *pChkStyleFlag/*=NULL*/)
{
	if(pChkStyleFlag==NULL||(*pChkStyleFlag)&CHKPARTNO_RODINFO)
	{
		bool bRet=true;
		if(fabs(baseline.SectorAngle()-pMirFlat->baseline.SectorAngle())>epsilon)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Sector angle is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}扇区角度不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(baseline.Radius()-pMirFlat->baseline.Radius()!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Radius is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}半径不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(wide-pMirFlat->wide!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Width is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}宽度不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(thick-pMirFlat->thick!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Thickness is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}厚度不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(cMaterial!=pMirFlat->cMaterial)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Material is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}材质不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(offset_h-pMirFlat->offset_h!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Radial offset is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}横向位移不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(offset_v-pMirFlat->offset_v!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Normal offset is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}竖向位移不同",(char*)sPartNo);
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
		UCS_STRU tem_ucs = ucs;
		UCS_STRU tem_vice_ucs = pMirFlat->ucs;
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

f3dPoint CLDSArcFlat::GetDatumPos(f3dPoint pickPos)
{
	ucs.origin=baseline.Center();
	ucs.axis_z=baseline.WorkNorm();
	f3dPoint pickVec=pickPos-f3dPoint(baseline.Center());
	pickVec-=(pickVec*ucs.axis_z)*ucs.axis_z;	//将拾取点矢径投影到圆弧平面上
	normalize(pickVec);
	if(fabs(baseline.ColumnNorm()*baseline.WorkNorm())>EPS_COS)
		pickPos=ucs.origin+pickVec*baseline.Radius();
	else
	{
		ucs.axis_x=f3dPoint(baseline.Start())-ucs.origin;
		normalize(ucs.axis_x);
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		coord_trans(pickPos,ucs,FALSE);
		double angle=Cal2dLineAng(0,0,pickPos.x,pickPos.y);
		pickPos=baseline.PositionInAngle(angle);
	}
	pickPos+=pickVec*(offset_h+thick)+ucs.axis_z*offset_v;
	return pickPos;
}
BOOL CLDSArcFlat::Create3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	if(CreateShadowPart3dSolidModel(bDisplayHole,scale_of_user2scr,sample_len,smoothness))
		return TRUE;

	if(pSolidBody==NULL)
		pSolidBody = new CSolidBody;
	else if(!IsModified())	//数据未修改
		return TRUE;
	else
		pSolidBody->Empty();
	SetModified(FALSE);
	fBody body;
	fBody *pBody=&body;
	COLORREF crArcFlat=GetColor(CLS_ARCFLAT,3,cMaterial);
	if(m_bUserSpecColor)	//用户指定构件颜色
		crArcFlat=crMaterial;
	int i,j;
	getUCS(ucs);
	f3dPoint start(baseline.Radius()+offset_h,0,offset_v);
	//计算正负头影响
	double start_oddment_angle=0.0,end_oddment_angle=0.0;
	start_oddment_angle=start_oddment/baseline.Radius();
	end_oddment_angle=end_oddment/baseline.Radius();
	if(fabs(start_oddment_angle)>EPS)
		rotate_point_around_axis(start,-start_oddment_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
	double sector_angle=baseline.SectorAngle()+start_oddment_angle+end_oddment_angle;
	double radius = baseline.Radius();
	int uSlices = CalArcResolution(radius,sector_angle,scale_of_user2scr,sample_len,smoothness);
	uSlices = max(uSlices,(int)m_uDisplaySlices);
	double piece_angle=sector_angle/uSlices;
	f3dPoint vertex_arr[4];
	for(i=0;i<=uSlices;i++)
	{
		vertex_arr[0]=start;
		double angle=i*piece_angle;
		double cosa = cos(angle-start_oddment_angle);
		double sina = sin(angle-start_oddment_angle);
		if(i>0)
			rotate_point_around_axis(vertex_arr[0],i*piece_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
		vertex_arr[1]=vertex_arr[0]+f3dPoint(thick*cosa,thick*sina);
		vertex_arr[2].Set(vertex_arr[1].x,vertex_arr[1].y,vertex_arr[0].z+wide);
		vertex_arr[3]=vertex_arr[2]-thick*f3dPoint(cosa,sina);
		for(j=0;j<4;j++)
			pBody->vertex.append(vertex_arr[j]);
	}
	SetModified(FALSE);
	//初始化链表
	CCreateFace createFace;
	createFace.InitVertexList(pBody);

	f3dPolyFace *pFace=NULL;
	//始端端截面
	pFace=pBody->faceList.append();
	pFace->material = crArcFlat;
	createFace.NewOutterEdgeLine(pFace,1,0);
	createFace.NewOutterEdgeLine(pFace,2);
	createFace.NewOutterEdgeLine(pFace,3);
	createFace.NewOutterEdgeLine(pFace,0);
	for(i=0;i<uSlices;i++)
	{
		int base_index=i*4,next_index=i*4+4;
		//Y肢底面创建
		pFace=pBody->faceList.append();
		pFace->material = crArcFlat;
		createFace.NewOutterEdgeLine(pFace,base_index,base_index+1);
		createFace.NewOutterEdgeLine(pFace,next_index);
		createFace.NewOutterEdgeLine(pFace,next_index+1);
		createFace.NewOutterEdgeLine(pFace,base_index+1);
		//Y肢右侧面创建
		pFace=pBody->faceList.append();
		pFace->material = crArcFlat;
		createFace.NewOutterEdgeLine(pFace,next_index+1,base_index+1);
		createFace.NewOutterEdgeLine(pFace,next_index+2);
		createFace.NewOutterEdgeLine(pFace,base_index+2);
		createFace.NewOutterEdgeLine(pFace,base_index+1);
		//Y肢顶面创建
		pFace=pBody->faceList.append();
		pFace->material = crArcFlat;
		createFace.NewOutterEdgeLine(pFace,next_index+2,base_index+2);
		createFace.NewOutterEdgeLine(pFace,next_index+3);
		createFace.NewOutterEdgeLine(pFace,base_index+3);
		createFace.NewOutterEdgeLine(pFace,base_index+2);
		//X肢左面创建
		pFace=pBody->faceList.append();
		pFace->material = crArcFlat;
		createFace.NewOutterEdgeLine(pFace,next_index+3,base_index+3);
		createFace.NewOutterEdgeLine(pFace,next_index);
		createFace.NewOutterEdgeLine(pFace,base_index);
		createFace.NewOutterEdgeLine(pFace,base_index+3);
	}
	//终端端截面
	pFace=pBody->faceList.append();
	pFace->material = crArcFlat;
	createFace.NewOutterEdgeLine(pFace,uSlices*4+2,uSlices*4+3);
	createFace.NewOutterEdgeLine(pFace,uSlices*4+1);
	createFace.NewOutterEdgeLine(pFace,uSlices*4);
	createFace.NewOutterEdgeLine(pFace,uSlices*4+3);
	for(f3dPoint *pVertex=pBody->vertex.GetFirst();pVertex;pVertex=pBody->vertex.GetNext())
	{
		coord_trans(*pVertex,ucs,TRUE);
	}
	pSolidBody->ConvertFrom(pBody);
	return TRUE;
}

CLDSArcSlot::CLDSArcSlot()
{
	//strcpy(m_sClassName,"CLDSArcSlot");
	strcpy(spec,"6.3");
	m_nClassTypeId=CLS_ARCSLOT;
	m_uDisplaySlices=16;
	LoadDefaultUCS(&ucs);
	m_uStatMatCoef=1;
	m_bVirtualPart = false;
	is_data_modified=1;
	m_bDisplayHole=false;
	strcpy(sPartNo,"");
	m_hPartWeldParent=0;
	m_fBaseLineRadius=0;
	wide=56;
	thick=4;
	height=200;
	offset_h=-100;
	offset_v=0;
	start_rot_angle=0;
	pSolidBody=NULL;
}

int CLDSArcSlot::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;//[100]={""};
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
	else if(GetPropID("m_uStatMatCoef")==id)
		sprintf(sText,"%d",m_uStatMatCoef);
	else if(GetPropID("m_fZoomLenCoef")==id)
	{
		sprintf(sText,"%f",m_fZoomLenCoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_hPartWeldParent")==id)
		sprintf(sText,"0X%X",m_hPartWeldParent);
	else if(GetPropID("m_bDisplayHole")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bDisplayHole)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
		if(m_bDisplayHole)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("m_bVirtualPart")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bVirtualPart)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
		if(m_bVirtualPart)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("m_uDisplaySlices")==id)
		sprintf(sText,"%d",m_uDisplaySlices);
	else if(GetPropID("m_fBaseLineRadius")==id)
	{
		sprintf(sText,"%f",m_fBaseLineRadius);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("arc_radius")==id)
	{
		sprintf(sText,"%f",baseline.Radius());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("offset_h")==id)
	{
		sprintf(sText,"%f",offset_h);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("offset_v")==id)
	{
		sprintf(sText,"%f",offset_v);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("specification")==id)
		sprintf(sText,"%s",spec);
	else if(GetPropID("height")==id)
	{
		sprintf(sText,"%f",height);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("wide")==id)
	{
		sprintf(sText,"%f",wide);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("thick")==id)
	{
		sprintf(sText,"%f",thick);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("arc_start.x")==id)
	{
		sprintf(sText,"%f",arc_start.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("arc_start.y")==id)
	{
		sprintf(sText,"%f",arc_start.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("arc_start.z")==id)
	{
		sprintf(sText,"%f",arc_start.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("start_rot_angle")==id)
	{
		sprintf(sText,"%f",start_rot_angle*DEGTORAD_COEF);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("sector_angle")==id)
	{
		sprintf(sText,"%f",sector_angle*DEGTORAD_COEF);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("start_oddment")==id)
	{
		sprintf(sText,"%f",start_oddment);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("end_oddment")==id)
	{
		sprintf(sText,"%f",end_oddment);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_bUserSpecColor")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bUserSpecColor)
			sprintf(sText,"Yes");
		else
			sprintf(sText,"No");
#else
		if(m_bUserSpecColor)
			sprintf(sText,"是");
		else
			sprintf(sText,"否");
#endif
	}
	else if(GetPropID("crMaterial")==id)
		sprintf(sText,"RGB%X",crMaterial);
	else if(GetPropID("sNotes")==id)
		strcpy(sText,sNotes);
	else if(GetPropID("center.x")==id)
	{
		sprintf(sText,"%f",baseline.Center().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("center.y")==id)
	{
		sprintf(sText,"%f",baseline.Center().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("center.z")==id)
	{
		sprintf(sText,"%f",baseline.Center().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("workNorm.x")==id)
	{
		sprintf(sText,"%f",baseline.WorkNorm().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("workNorm.y")==id)
	{
		sprintf(sText,"%f",baseline.WorkNorm().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("workNorm.z")==id)
	{
		sprintf(sText,"%f",baseline.WorkNorm().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.x")==id)
	{
		sprintf(sText,"%f",ucs.origin.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.y")==id)
	{
		sprintf(sText,"%f",ucs.origin.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.z")==id)
	{
		sprintf(sText,"%f",ucs.origin.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_arc_norm")==id)
		sText.ConvertFromPoint(des_arc_norm.vector);
	else if(GetPropID("des_arc_center")==id)
		sText.ConvertFromPoint(des_arc_center.Position());
	else if(GetPropID("des_arc_start")==id)
		sText.ConvertFromPoint(des_arc_start.Position());
	else if(GetPropID("hStart")==id)
	{
		if(pStart)
			sprintf(sText,"0X%X",pStart->handle);
		else
			strcpy(sText,"0X0");
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
	else if(GetPropID("hEnd")==id)
	{
		if(pEnd)
			sprintf(sText,"0X%X",pEnd->handle);
		else
			strcpy(sText,"0X0");
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
	else 
		strcpy(sText,"");
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
long CLDSArcSlot::GetPropID(char* propStr)
{
	PROPLIST_ITEM *pItem = CLDSArcSlot::propHashtable.GetValue(propStr);
	if(pItem!=NULL)
		return pItem->id;
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		/*CString prompt="Did not find a property Id named\"";
		prompt+=propStr;
		prompt+="\"!";
		AfxMessageBox(prompt);*/
#else
		/*CString prompt="没找到名为\"";
		prompt+=propStr;
		prompt+="\"的属性ID！";
		AfxMessageBox(prompt);*/
#endif
		return 0;
	}
}
int CLDSArcSlot::GetPropStr(long id,char* propStr)
{
	char sText[200]="";
	if(GetPropID("basicInfo")==0)
		strcpy(sText,"basicInfo");
	else if(GetPropID("handle")==id)
		strcpy(sText,"handle");
	else if(GetPropID("layer")==id)
		strcpy(sText,"layer");
	else if(GetPropID("cMaterial")==id)
		strcpy(sText,"cMaterial");
	else if(GetPropID("m_cQualityLevel")==id)
		strcpy(sText,"m_cQualityLevel");
	else if(GetPropID("iSeg")==id)
		strcpy(sText,"iSeg");
	else if(GetPropID("sPartNo")==id)
		strcpy(sText,"sPartNo");
	else if(GetPropID("cfgword")==id)
		strcpy(sText,"cfgword");
	else if(GetPropID("m_uStatMatCoef")==id)
		strcpy(sText,"m_uStatMatCoef");
	else if(GetPropID("m_hPartWeldParent")==id)
		strcpy(sText,"m_hPartWeldParent");
	else if(GetPropID("m_bDisplayHole")==id)
		strcpy(sText,"m_bDisplayHole");
	else if(GetPropID("m_bVirtualPart")==id)
		strcpy(sText,"m_bVirtualPart");
	else if(GetPropID("m_uDisplaySlices")==id)
		strcpy(sText,"m_uDisplaySlices");
	else if(GetPropID("m_fBaseLineRadius")==id)
		strcpy(sText,"m_fBaseLineRadius");
	else if(GetPropID("m_fZoomLenCoef")==id)
		strcpy(sText,"m_fZoomLenCoef");
	else if(GetPropID("arc_radius")==id)
		strcpy(sText,"arc_radius");
	else if(GetPropID("offset_h")==id)
		strcpy(sText,"offset_h");
	else if(GetPropID("offset_v")==id)
		strcpy(sText,"offset_v");
	else if(GetPropID("thick")==id)
		strcpy(sText,"thick");
	else if(GetPropID("wide")==id)
		strcpy(sText,"wide");
	else if(GetPropID("h")==id)
		strcpy(sText,"h");
	else if(GetPropID("sNotes")==id)
		strcpy(sText,"sNotes");
	else if(GetPropID("assemblePos")==id)
		strcpy(sText,"assemblePos");
	else if(GetPropID("ucs.origin")==id)
		strcpy(sText,"ucs.origin");
	else if(GetPropID("ucs.origin.x")==id)
		strcpy(sText,"ucs.origin.x");
	else if(GetPropID("ucs.origin.y")==id)
		strcpy(sText,"ucs.origin.y");
	else if(GetPropID("ucs.origin.z")==id)
		strcpy(sText,"ucs.origin.z");
	else if(GetPropID("ucs.axis_x")==id)
		strcpy(sText,"ucs.axis_x");
	else if(GetPropID("ucs.axis_x.x")==id)
		strcpy(sText,"ucs.axis_x.x");
	else if(GetPropID("ucs.axis_x.y")==id)
		strcpy(sText,"ucs.axis_x.y");
	else if(GetPropID("ucs.axis_x.z")==id)
		strcpy(sText,"ucs.axis_x.z");
	else if(GetPropID("ucs.axis_y")==id)
		strcpy(sText,"ucs.axis_y");
	else if(GetPropID("ucs.axis_y.x")==id)
		strcpy(sText,"ucs.axis_y.x");
	else if(GetPropID("ucs.axis_y.y")==id)
		strcpy(sText,"ucs.axis_y.y");
	else if(GetPropID("ucs.axis_y.z")==id)
		strcpy(sText,"ucs.axis_y.z");
	else if(GetPropID("ucs.axis_z")==id)
		strcpy(sText,"ucs.axis_z");
	else if(GetPropID("ucs.axis_z.x")==id)
		strcpy(sText,"ucs.axis_z.x");
	else if(GetPropID("ucs.axis_z.y")==id)
		strcpy(sText,"ucs.axis_z.y");
	else if(GetPropID("ucs.axis_z.z")==id)
		strcpy(sText,"ucs.axis_z.z");
	if(propStr)
		strcpy(propStr,sText);
	return strlen(sText);
}
BOOL CLDSArcSlot::IsPropEqual(HASHOBJSET &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSArcSlot *pPart=NULL, *pFirstPart=(CLDSArcSlot*)selectObjs.GetFirst();
	//pPart->GetPropStr(idProp);
	pFirstPart->GetPropValueStr(idProp,valueStr);
	for(pPart=(CLDSArcSlot*)selectObjs.GetNext();pPart;pPart=(CLDSArcSlot*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pPart->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

int CLDSArcSlot::GetPropStatus(long id)
{	
	DWORD dwStatus;
	if(CLDSArcSlot::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
}

BOOL CLDSArcSlot::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSArcSlot::propStatusHashtable.SetValueAt(id,dwStatus);
}

double CLDSArcSlot::GetLength()
{
	return baseline.Radius()*baseline.SectorAngle();
}

double CLDSArcSlot::GetWeight(BOOL bSolidLength/*=TRUE*/)
{
	double v=(wide*height-(wide-thick)*(height-2*thick))*GetLength();
	return CalSteelPlankWei(v);
}

int CLDSArcSlot::GetSpecification(char *slot_spec,BOOL bIncMat/*=TRUE*/)
{	//槽钢规格
	char steelmark[20]="";
	char tmp_spec[200];
	if(bIncMat&&cMaterial!=CSteelMatLibrary::Q235BriefMark())//'S')
	{
		QuerySteelMatMark(cMaterial,steelmark);
		if(strlen(spec)>0)
			sprintf(tmp_spec,"%s-[%s",steelmark,spec);
		else 
			sprintf(tmp_spec,"%s-[%.1f",steelmark,height*0.1);
	}
	else
	{
		if(strlen(spec)>0)
			sprintf(tmp_spec,"[%s",spec);
		else
			sprintf(tmp_spec,"[%.1f",height*0.1);
	}
	SimplifiedNumString(tmp_spec);
	if(slot_spec)
		strcpy(slot_spec,tmp_spec);
	return strlen(tmp_spec);
}

void CLDSArcSlot::IndependentShadowPropFromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{

}
void CLDSArcSlot::IndependentShadowPropToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{

}
void CLDSArcSlot::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//保存影射构件参数和构件从属句柄 wht 12-09-14
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if(ShadowParaAndBlockIdToBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropToBuffer(buffer,version,doc_type);
		return;
	}
	CLDSArcPart::ToBuffer(buffer);
	buffer.WriteString(spec);
	buffer.WriteDouble(height);	//高
	buffer.WriteDouble(thick);	//厚
	buffer.WriteDouble(wide);	//宽
}

void CLDSArcSlot::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="";
	fprintf(fp,"<环形槽钢>\n");
	CLDSArcPart::ToXmlFile(fp,schema);
	fprintf(fp,"<规格 spec=\"%s\"/>\n",spec);
	sprintf(sText,"%f",height);
	SimplifiedNumString(sText);
	fprintf(fp,"<高 height=\"%s\"/>\n",sText);
	sprintf(sText,"%f",thick);
	SimplifiedNumString(sText);
	fprintf(fp,"<厚 thick=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wide);
	SimplifiedNumString(sText);
	fprintf(fp,"<宽 wide=\"%s\"/>\n",sText);
	fprintf(fp,"</环形槽钢>\n");
}

void CLDSArcSlot::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_tma1_lma2_tsa3_lds4_default0/*=0*/)
{
	if(doc_tma1_lma2_tsa3_lds4_default0==0)
		doc_tma1_lma2_tsa3_lds4_default0=PRODUCT_TYPE;
	//读取影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_tma1_lma2_tsa3_lds4_default0))
	{
		IndependentShadowPropFromBuffer(buffer,version,doc_tma1_lma2_tsa3_lds4_default0);
		return;
	}
	CLDSArcPart::FromBuffer(buffer,version,doc_tma1_lma2_tsa3_lds4_default0);
	buffer.ReadString(spec,16);
	buffer.ReadDouble(&height);	//高
	buffer.ReadDouble(&thick);	//厚
	buffer.ReadDouble(&wide);	//宽
}


bool CLDSArcSlot::IsEqual(CLDSArcSlot* pMirSlot,double epsilon/*=1.0*/,char *error_info/*=NULL*/,
						   int *pErrorInfo/*=NULL*/,DWORD *pChkStyleFlag/*=NULL*/)
{
	if(pChkStyleFlag==NULL||(*pChkStyleFlag)&CHKPARTNO_RODINFO)
	{
		bool bRet=true;
		if(fabs(baseline.SectorAngle()-pMirSlot->baseline.SectorAngle())>epsilon)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Sector angle sector is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}扇区角度不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(baseline.Radius()-pMirSlot->baseline.Radius()!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Radius is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}半径不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(wide-pMirSlot->wide!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Width is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}宽度不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(thick-pMirSlot->thick!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Thickness is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}厚度不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(height-pMirSlot->height!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Height is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}槽高不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(cMaterial!=pMirSlot->cMaterial)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Material is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}材质不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(offset_h-pMirSlot->offset_h!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Radial offset is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}横向位移不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(offset_v-pMirSlot->offset_v!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Normal offset is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}竖向位移不同",(char*)sPartNo);
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
		UCS_STRU tem_ucs = ucs;
		UCS_STRU tem_vice_ucs = pMirSlot->ucs;
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
f3dPoint CLDSArcSlot::GetDatumPos(f3dPoint pickPos)
{
	ucs.origin=baseline.Center();
	ucs.axis_z=baseline.WorkNorm();
	f3dPoint pickVec=pickPos-f3dPoint(baseline.Center());
	pickVec-=(pickVec*ucs.axis_z)*ucs.axis_z;	//将拾取点矢径投影到圆弧平面上
	normalize(pickVec);
	if(fabs(baseline.ColumnNorm()*baseline.WorkNorm())>EPS_COS)
		pickPos=ucs.origin+pickVec*baseline.Radius();
	else
	{
		ucs.axis_x=f3dPoint(baseline.Start())-ucs.origin;
		normalize(ucs.axis_x);
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		coord_trans(pickPos,ucs,FALSE);
		double angle=Cal2dLineAng(0,0,pickPos.x,pickPos.y);
		pickPos=baseline.PositionInAngle(angle);
	}
	if(m_siLayoutStyle==0)	//]模式
		pickPos+=pickVec*offset_h;
	else if(m_siLayoutStyle==1)	//┌┐模式
		pickPos+=pickVec*(offset_h+height/2);
	else if(m_siLayoutStyle==2)	//[模式
		pickPos+=pickVec*offset_h;
	else if(m_siLayoutStyle==3)
		pickPos+=pickVec*(offset_h+height/2);
	return pickPos;
}

BOOL CLDSArcSlot::Create3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	if(CreateShadowPart3dSolidModel(bDisplayHole,scale_of_user2scr,sample_len,smoothness))
		return TRUE;

	if(pSolidBody==NULL)
		pSolidBody = new CSolidBody;
	else if(!IsModified())	//数据未修改
		return TRUE;
	else
		pSolidBody->Empty();
	SetModified(FALSE);
	fBody body;
	fBody* pBody=&body;
	getUCS(ucs);
	f3dPoint start(baseline.Radius()+offset_h,0,offset_v);
	//计算正负头影响
	double start_oddment_angle=0.0,end_oddment_angle=0.0;
	start_oddment_angle=start_oddment/baseline.Radius();
	end_oddment_angle=end_oddment/baseline.Radius();
	if(fabs(start_oddment_angle)>EPS)
		rotate_point_around_axis(start,-start_oddment_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
	int i;
	f3dPoint vertex_arr[8];
	COLORREF crArcSlot=GetColor(CLS_ARCSLOT,3,cMaterial);
	if(m_bUserSpecColor)	//用户指定构件颜色
		crArcSlot=crMaterial;
	double radius = baseline.Radius();
	double sector_angle=baseline.SectorAngle()+start_oddment_angle+end_oddment_angle;
	int uSlices = CalArcResolution(radius,sector_angle,scale_of_user2scr,sample_len,smoothness);
	uSlices = max(uSlices,(int)m_uDisplaySlices);
	double piece_angle=sector_angle/uSlices;
	for(i=0;i<=uSlices;i++)
	{
		vertex_arr[0]=start;
		double angle=i*piece_angle;
		double cosa = cos(angle-start_oddment_angle);
		double sina = sin(angle-start_oddment_angle);
		if(i>0)
			rotate_point_around_axis(vertex_arr[0],i*piece_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
		if(m_siLayoutStyle==0)
		{	//]模式
			vertex_arr[1]=vertex_arr[0]-wide*f3dPoint(cosa,sina);
			vertex_arr[2]=vertex_arr[1]-f3dPoint(0,0,thick);
			vertex_arr[3]=vertex_arr[2]+(wide-thick)*f3dPoint(cosa,sina);
			vertex_arr[4]=vertex_arr[3]-f3dPoint(0,0,height-2*thick);
			vertex_arr[5]=vertex_arr[4]-(wide-thick)*f3dPoint(cosa,sina);
			vertex_arr[6].Set(vertex_arr[5].x,vertex_arr[5].y,vertex_arr[0].z-height);
			vertex_arr[7].Set(vertex_arr[0].x,vertex_arr[0].y,vertex_arr[0].z-height);
		}
		else if(m_siLayoutStyle==1)
		{	//┌┐模式
			vertex_arr[1]=vertex_arr[0]-f3dPoint(0,0,wide);
			vertex_arr[2]=vertex_arr[1]+thick*f3dPoint(cosa,sina);
			vertex_arr[3]=vertex_arr[2]+f3dPoint(0,0,wide-thick);
			vertex_arr[4]=vertex_arr[3]+(height-2*thick)*f3dPoint(cosa,sina);
			vertex_arr[5]=vertex_arr[4]-f3dPoint(0,0,wide-thick);
			vertex_arr[6]=vertex_arr[5]+thick*f3dPoint(cosa,sina);
			vertex_arr[7]=vertex_arr[6]+f3dPoint(0,0,wide);
		}
		else if(m_siLayoutStyle==2)
		{	//[模式
			vertex_arr[1]=vertex_arr[0]+wide*f3dPoint(cosa,sina);
			vertex_arr[2]=vertex_arr[1]+f3dPoint(0,0,thick);
			vertex_arr[3]=vertex_arr[2]-(wide-thick)*f3dPoint(cosa,sina);
			vertex_arr[4]=vertex_arr[3]+f3dPoint(0,0,height-2*thick);
			vertex_arr[5]=vertex_arr[4]+(wide-thick)*f3dPoint(cosa,sina);
			vertex_arr[6].Set(vertex_arr[5].x,vertex_arr[5].y,vertex_arr[0].z+height);
			vertex_arr[7].Set(vertex_arr[0].x,vertex_arr[0].y,vertex_arr[0].z+height);
		}
		else if(m_siLayoutStyle==3)
		{	//└┘模式
			vertex_arr[1]=vertex_arr[0]+f3dPoint(0,0,wide);
			vertex_arr[2]=vertex_arr[1]-thick*f3dPoint(cosa,sina);
			vertex_arr[3]=vertex_arr[2]-f3dPoint(0,0,wide-thick);
			vertex_arr[4]=vertex_arr[3]-(height-2*thick)*f3dPoint(cosa,sina);
			vertex_arr[5]=vertex_arr[4]+f3dPoint(0,0,wide-thick);
			vertex_arr[6]=vertex_arr[5]-thick*f3dPoint(cosa,sina);
			vertex_arr[7]=vertex_arr[6]-f3dPoint(0,0,wide);
		}
		for(int j=0;j<8;j++)
			pBody->vertex.append(vertex_arr[j]);
	}
	is_data_modified = FALSE;
	//初始化链表
	CCreateFace createFace;
	createFace.InitVertexList(pBody);

	f3dPolyFace *pFace=NULL;
	//始端端截面
	pFace=pBody->faceList.append();
	pFace->material = crArcSlot;
	createFace.NewOutterEdgeLine(pFace,1,0);
	createFace.NewOutterEdgeLine(pFace,2);
	createFace.NewOutterEdgeLine(pFace,3);
	createFace.NewOutterEdgeLine(pFace,4);
	createFace.NewOutterEdgeLine(pFace,5);
	createFace.NewOutterEdgeLine(pFace,6);
	createFace.NewOutterEdgeLine(pFace,7);
	createFace.NewOutterEdgeLine(pFace,0);
	for(i=0;i<uSlices;i++)
	{
		int base_index=i*8,next_index=i*8+8;
		//上Y肢底面创建
		pFace=pBody->faceList.append();
		pFace->material = crArcSlot;
		createFace.NewOutterEdgeLine(pFace,base_index,base_index+1);
		createFace.NewOutterEdgeLine(pFace,next_index);
		createFace.NewOutterEdgeLine(pFace,next_index+1);
		createFace.NewOutterEdgeLine(pFace,base_index+1);
		//上Y肢右侧面创建
		pFace=pBody->faceList.append();
		pFace->material = crArcSlot;
		createFace.NewOutterEdgeLine(pFace,next_index+1,base_index+1);
		createFace.NewOutterEdgeLine(pFace,next_index+2);
		createFace.NewOutterEdgeLine(pFace,base_index+2);
		createFace.NewOutterEdgeLine(pFace,base_index+1);
		//上Y肢顶面创建
		pFace=pBody->faceList.append();
		pFace->material = crArcSlot;
		createFace.NewOutterEdgeLine(pFace,next_index+2,base_index+2);
		createFace.NewOutterEdgeLine(pFace,next_index+3);
		createFace.NewOutterEdgeLine(pFace,base_index+3);
		createFace.NewOutterEdgeLine(pFace,base_index+2);
		//X肢右面创建
		pFace=pBody->faceList.append();
		pFace->material = crArcSlot;
		createFace.NewOutterEdgeLine(pFace,next_index+3,base_index+3);
		createFace.NewOutterEdgeLine(pFace,next_index+4);
		createFace.NewOutterEdgeLine(pFace,base_index+4);
		createFace.NewOutterEdgeLine(pFace,base_index+3);
		//下Y肢顶面创建
		pFace=pBody->faceList.append();
		pFace->material = crArcSlot;
		createFace.NewOutterEdgeLine(pFace,next_index+4,base_index+4);
		createFace.NewOutterEdgeLine(pFace,next_index+5);
		createFace.NewOutterEdgeLine(pFace,base_index+5);
		createFace.NewOutterEdgeLine(pFace,base_index+4);
		//下Y肢右面创建
		pFace=pBody->faceList.append();
		pFace->material = crArcSlot;
		createFace.NewOutterEdgeLine(pFace,next_index+5,base_index+5);
		createFace.NewOutterEdgeLine(pFace,next_index+6);
		createFace.NewOutterEdgeLine(pFace,base_index+6);
		createFace.NewOutterEdgeLine(pFace,base_index+5);
		//下Y肢底面创建
		pFace=pBody->faceList.append();
		pFace->material = crArcSlot;
		createFace.NewOutterEdgeLine(pFace,next_index+6,base_index+6);
		createFace.NewOutterEdgeLine(pFace,next_index+7);
		createFace.NewOutterEdgeLine(pFace,base_index+7);
		createFace.NewOutterEdgeLine(pFace,base_index+6);
		//X肢左面创建
		pFace=pBody->faceList.append();
		pFace->material = crArcSlot;
		createFace.NewOutterEdgeLine(pFace,next_index+7,base_index+7);
		createFace.NewOutterEdgeLine(pFace,next_index);
		createFace.NewOutterEdgeLine(pFace,base_index);
		createFace.NewOutterEdgeLine(pFace,base_index+7);
	}
	//终端端截面
	pFace=pBody->faceList.append();
	pFace->material = crArcSlot;
	createFace.NewOutterEdgeLine(pFace,uSlices*8+6,uSlices*8+7);
	createFace.NewOutterEdgeLine(pFace,uSlices*8+5);
	createFace.NewOutterEdgeLine(pFace,uSlices*8+4);
	createFace.NewOutterEdgeLine(pFace,uSlices*8+3);
	createFace.NewOutterEdgeLine(pFace,uSlices*8+2);
	createFace.NewOutterEdgeLine(pFace,uSlices*8+1);
	createFace.NewOutterEdgeLine(pFace,uSlices*8);
	createFace.NewOutterEdgeLine(pFace,uSlices*8+7);
	for(f3dPoint *pVertex=pBody->vertex.GetFirst();pVertex;pVertex=pBody->vertex.GetNext())
	{
		coord_trans(*pVertex,ucs,TRUE);
	}
	pSolidBody->ConvertFrom(pBody);
	return TRUE;
}
#endif