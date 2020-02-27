#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "UI.h"
#include "FootDesignTemplate.h"
#include "GlobalFunc.h"
#include "LayAngleBoltDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/////////////////////////////////////////////////////////////////////////////
// CFootTempl class
//晓川任务: wjh-2014.12.15
//1.填充完善ModifyDesignFootProperty函数及ButtonClick函数
//2.可借用层级对话框技术处理显示底板连接角钢集合与靴板集合界面
//3.在调用FootDesignTemplDlg时,自动识别塔脚相关的各连接角钢,并判断生成位置标签
//4.完善各项property的操作,如定位原点,主材角钢选取等
//5.将对话框中的FootTempl参数应用生成DesignShoePlates中
//6.完善FootTempl函数导入导出成Xml文件
//晓川任务: wjh-2014.12.31
//1.根据基准材规格大致选择不同的底板宽及厚度的默认值
//2.根据实例设计并完善牛蹄板及焊接（搭焊，对焊）塔脚代码
//3.根据应用实践完善根据XML文件快速设计塔脚
//4.将塔脚底板及靴板的设计参数直接（无嵌套）显示到左侧属性栏中
CXhChar16 CFootTempl::LINKANGLE::PosLabel()
{
	//根据label的编号规则判断:如:X+,X-,X+Y+等,注意横担上的主材需要尽量识别为轴向
#ifdef AFX_TARG_ENU_ENGLISH
	if(label.IsEqual(KEY4C("ZA")))
		return CXhChar16("Upside primary rod");
	else if(label.IsEqual(KEY4C("ZB")))
		return CXhChar16("Downside primary rod");
	else if(label.IsEqual(KEY4C("XA")))
		return CXhChar16("X+ rod");
	else if(label.IsEqual(KEY4C("XB")))
		return CXhChar16("X- rod");
	else if(label.IsEqual(KEY4C("YA")))
		return CXhChar16("Y+ rod");
	else if(label.IsEqual(KEY4C("YB")))
		return CXhChar16("Y- rod");
	else if(label.IsEqual(KEY4C("XY1")))
		return CXhChar16("horizontal 1st Quadrant");
	else if(label.IsEqual(KEY4C("XY2")))
		return CXhChar16("horizontal 2nd Quadrant");
	else if(label.IsEqual(KEY4C("XY3")))
		return CXhChar16("horizontal 3rd Quadrant");
	else if(label.IsEqual(KEY4C("XY4")))
		return CXhChar16("horizontal 4th Quadrant");
	else if(label.IsEqual(KEY4C("UXZA")))
		return CXhChar16("Upside XZ +Plane");
	else if(label.IsEqual(KEY4C("UXZB")))
		return CXhChar16("Upside XZ -Plane");
	else if(label.IsEqual(KEY4C("UYZA")))
		return CXhChar16("Upside YZ +Plane");
	else if(label.IsEqual(KEY4C("UYZB")))
		return CXhChar16("Upside YZ -Plane");
	else if(label.IsEqual(KEY4C("DXZA")))
		return CXhChar16("Downside XZ +Plane");
	else if(label.IsEqual(KEY4C("DXZB")))
		return CXhChar16("Downside XZ -Plane");
	else if(label.IsEqual(KEY4C("DYZA")))
		return CXhChar16("Downside YZ +Plane");
	else //if(label.IsEqual(KEY4C("DYZB")))
		return CXhChar16("Downside YZ -Plane");
#else
	if(label.IsEqual(KEY4C("ZA")))
		return CXhChar16("上主材");
	else if(label.IsEqual(KEY4C("ZB")))
		return CXhChar16("下主材");
	else if(label.IsEqual(KEY4C("XA")))
		return CXhChar16("X轴正向");
	else if(label.IsEqual(KEY4C("XB")))
		return CXhChar16("X轴负向");
	else if(label.IsEqual(KEY4C("YA")))
		return CXhChar16("Y轴正向");
	else if(label.IsEqual(KEY4C("YB")))
		return CXhChar16("Y轴负向");
	else if(label.IsEqual(KEY4C("XY1")))
		return CXhChar16("水平面第1象限");
	else if(label.IsEqual(KEY4C("XY2")))
		return CXhChar16("水平面第2象限");
	else if(label.IsEqual(KEY4C("XY3")))
		return CXhChar16("水平面第3象限");
	else if(label.IsEqual(KEY4C("XY4")))
		return CXhChar16("水平面第4象限");
	else if(label.IsEqual(KEY4C("UXZA")))
		return CXhChar16("上XZ正平面");
	else if(label.IsEqual(KEY4C("UXZB")))
		return CXhChar16("上XZ负平面");
	else if(label.IsEqual(KEY4C("UYZA")))
		return CXhChar16("上YZ正平面");
	else if(label.IsEqual(KEY4C("UYZB")))
		return CXhChar16("上YZ负平面");
	else if(label.IsEqual(KEY4C("DXZA")))
		return CXhChar16("下XZ正平面");
	else if(label.IsEqual(KEY4C("DXZB")))
		return CXhChar16("下XZ负平面");
	else if(label.IsEqual(KEY4C("DYZA")))
		return CXhChar16("下YZ正平面");
	else //if(label.IsEqual(KEY4C("DYZB")))
		return CXhChar16("下YZ负平面");
#endif
}
void CFootTempl::LINKANGLE::UpdateBoltInfo(CLDSNode* pBaseNode)
{
	if(pAngle==NULL || pBaseNode==NULL)
		return;
	if(pAngle->pStart==pBaseNode)
	{
		bolt.d=(BYTE)pAngle->connectStart.d;
		bolt.num=(BYTE)pAngle->BoltsNumOn1Wing(TRUE);
		bolt.rows=(BYTE)pAngle->connectStart.rows;
	}
	else if(pAngle->pEnd==pBaseNode)
	{
		bolt.d=(BYTE)pAngle->connectEnd.d;
		bolt.num=(BYTE)pAngle->BoltsNumOn1Wing(FALSE);
		bolt.rows=(BYTE)pAngle->connectEnd.rows;
	}
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,bolt.d);
	if(bolt.rows==1)
		bolt.siBoltSpace=(int)LsSpace.SingleRowSpace;
	else
		bolt.siBoltSpace=(int)LsSpace.doubleRowSpace;
	bolt.siEndSpace=(int)LsSpace.EndSpace;
}
void CFootTempl::LINKANGLE::UpdateBoltInfo(double fMinH)
{
	double fLenOffset=bolt.siEndSpace;
	for(int i=0;i<bolt.num;i++)
	{
		if(bolt.rows==1)
			fLenOffset+=bolt.siBoltSpace;
		else
			fLenOffset+=bolt.siBoltSpace*0.5;
	}
	if(fMinH>fLenOffset)
	{
		double fDist=fMinH-fLenOffset;
		int n=ftoi(fDist/bolt.siBoltSpace);
		if(bolt.rows==1)
			bolt.num+=n+1;
		else
			bolt.num+=(n+1)*2;
	}
}
/////////////////////////////////////////////////////////////////////////////
// CFootTempl::BASEPLATE
CFootTempl::BASEPLATE::BASEPLATE()
{
	cMaterial=CSteelMatLibrary::Q235BriefMark();
	shape=0;	//0:表示为方板1:表示为圆板
	thick=30;
	XA=XB=YA=YB=200;	//A表示X(Y)轴正向,B表示X(Y)轴负向,C表示倒角宽
	fR=300;
	memset(XC,0,16);
	memset(YC,0,16);
	pPlate=NULL;
	cNormOffsetType=1;
	nNormOffset=0;
}
///////////////////////////////////////////////////////////////////////////////////////
//CFootTempl::SHOEPLATE
CFootTempl::SHOEPLATE::SHOEPLATE(DWORD key)
{
	label=key;
	pPlate=NULL;
	thick=20;
	minheight=0;
	m_bTranverse=false;
	fCutAngleWidth=0;
	norm_offset=0;
	cMaterial=CSteelMatLibrary::Q235BriefMark();
	bAutoLayBolt=FALSE;
}
CXhChar16 CFootTempl::SHOEPLATE::PosLabel()
{
	//根据label的编号规则判断:如:XQ(X轴前侧靴板),XH(X轴后侧靴板),XT(X轴贯通靴板)等
#ifdef AFX_TARG_ENU_ENGLISH
	if(label.IsEqual(KEY4C("UXB")))
	{
		if(m_bTranverse)
			return CXhChar16("Across plate on X");
		else
			return CXhChar16("Half plate on X-(connection side)");
	}
	else if(label.IsEqual(KEY4C("UXF")))
		return CXhChar16("Plate on X+(outside)");
	else if(label.IsEqual(KEY4C("UYB")))
	{
		if(m_bTranverse)
			return CXhChar16("Across plate on Y");
		else
			return CXhChar16("Half plate on Y+(connection side)");
	}
	else if(label.IsEqual(KEY4C("UYF")))
		return CXhChar16("Plate on Y-(outside)");
	else if(label.IsEqual(KEY4C("DXB")))
	{
		if(m_bTranverse)
			return CXhChar16("Downside across plate on X");
		else
			return CXhChar16("Downside half plate on X-(connection side)");
	}
	else if(label.IsEqual(KEY4C("DXF")))
		return CXhChar16("Downside plate on X+(outside)");
	else if(label.IsEqual(KEY4C("DYB")))
	{
		if(m_bTranverse)
			return CXhChar16("Downside across plate on Y");
		else
			return CXhChar16("Downside half plate on Y+(connection side)");
	}
	else //if(label.IsEqual(KEY4C("DYF")))
		return CXhChar16("Downside plate on Y-(outside)");
#else
	if(label.IsEqual(KEY4C("UXB")))
	{
		if(m_bTranverse)
			return CXhChar16("X轴通板");
		else
			return CXhChar16("X轴-(连接侧)");
	}
	else if(label.IsEqual(KEY4C("UXF")))
		return CXhChar16("X轴+(外侧)");
	else if(label.IsEqual(KEY4C("UYB")))
	{
		if(m_bTranverse)
			return CXhChar16("Y轴通板");
		else
			return CXhChar16("Y轴+(连接侧)");
	}
	else if(label.IsEqual(KEY4C("UYF")))
		return CXhChar16("Y轴-(外侧)");
	else if(label.IsEqual(KEY4C("DXB")))
	{
		if(m_bTranverse)
			return CXhChar16("下X轴通板");
		else
			return CXhChar16("下X轴-(连接侧)");
	}
	else if(label.IsEqual(KEY4C("DXF")))
		return CXhChar16("下X轴+(外侧)");
	else if(label.IsEqual(KEY4C("DYB")))
	{
		if(m_bTranverse)
			return CXhChar16("下Y轴通板");
		else
			return CXhChar16("下Y轴+(连接侧)");
	}
	else //if(label.IsEqual(KEY4C("DYF")))
		return CXhChar16("下Y轴-(外侧)");
#endif
}
//////////////////////////////////////////////////////////////////////////
//CFootTempl::RIBPLATE
CFootTempl::RIBPLATE::RIBPLATE()
{
	cMaterial=CSteelMatLibrary::Q235BriefMark();;
	upper=true;
	thick=16;
	A=350;
	B=150;
	C=30;
	D=30;
	MH=0;
	MF=MG=25;
	pos_para.PARA1.OX=pos_para.PARA1.OY=150;
	axisXType=1;
	pos_para.PARA1.axisX_x=1;
	pos_para.PARA1.axisX_y=0;
	water_slot_wide=30;
	iCutMode=0;
	fCutAngleWidth=0;
	fGradientAngle=90;
	pos_para.PARA2.angle=0;
	pos_para.PARA3.fRayDist=100;
}
//////////////////////////////////////////////////////////////////////////
//
CFootTempl::LINKSHOEPALTE::LINKSHOEPALTE()
{
	A=30;
	B=30;
	C=0;
	D=0;
	E=F=0;
	M=0;
	AL=0;
	CM=0;
	style_bolt0_weld1=0;		//连接形式:0.螺栓连接;1.对焊
	m_bBreakXueBan0C=FALSE;
	m_bBreakXueBan90C=!m_bBreakXueBan0C;
}
//////////////////////////////////////////////////////////////////////////
//CFootTempl
CFootTempl::CFootTempl()
{
	C=-50;
	m_bWeldDiZuo=FALSE;
	m_hBaseNode=0;
	axis_z.Set(0,0,-1);
}
bool CFootTempl::ImportFootDataFromXML(CMarkup xml,const XmlNode& xmlnode)
{
	CString sValue=_T("");
	while(xml.FindChildElem(xmlnode.sElemName))   
	{ 
		xml.IntoElem();
		if(stricmp(xmlnode.sElemName,"BasicParams")==0)
		{
			//为兼容旧版本格式，保留靴板连接信息  wxc-2017.1.13
			sValue=xml.GetAttrib("A");
			if(sValue.GetLength()>0)
				upLinkSP.A=downLinkSP.A=atoi(sValue);
			sValue=xml.GetAttrib("B");
			if(sValue.GetLength()>0)
				upLinkSP.B=downLinkSP.B=atoi(sValue);
			sValue=xml.GetAttrib("M");
			if(sValue.GetLength()>0)
				upLinkSP.M=downLinkSP.M=atoi(sValue);
			sValue=xml.GetAttrib("AL");
			if(sValue.GetLength()>0)
				upLinkSP.AL=downLinkSP.AL=atoi(sValue);
			sValue=xml.GetAttrib("C");
			C=atoi(sValue);
#ifdef AFX_TARG_ENU_ENGLISH
			sValue=xml.GetAttrib("WeldingBasePlate");
			if(sValue.CompareNoCase("Yes")==0)
				m_bWeldDiZuo=TRUE;
			else
				m_bWeldDiZuo=FALSE;
			sValue=xml.GetAttrib("ShoePlates");
			if(sValue.CompareNoCase("Bolts")==0)
				upLinkSP.style_bolt0_weld1=downLinkSP.style_bolt0_weld1=0;
			else if(sValue.CompareNoCase("ButtWelding")==0)
				upLinkSP.style_bolt0_weld1=downLinkSP.style_bolt0_weld1=1;
			else if(sValue.CompareNoCase("LapWelding")==0)
				upLinkSP.style_bolt0_weld1=downLinkSP.style_bolt0_weld1=2;
			sValue=xml.GetAttrib("0°break");
			if(sValue.CompareNoCase("Yes")==0)
				upLinkSP.m_bBreakXueBan0C=TRUE;
			else if(sValue.CompareNoCase("No")==0)
				upLinkSP.m_bBreakXueBan0C=FALSE;
			sValue=xml.GetAttrib("90°break");
			if(sValue.CompareNoCase("Yes")==0)
				upLinkSP.m_bBreakXueBan90C=TRUE;
			else if(sValue.CompareNoCase("No")==0)
				upLinkSP.m_bBreakXueBan90C=FALSE;
			sValue=xml.GetAttrib("Downside 0°break");
			if(sValue.CompareNoCase("Yes")==0)
				downLinkSP.m_bBreakXueBan0C=TRUE;
			else if(sValue.CompareNoCase("No")==0)
				downLinkSP.m_bBreakXueBan0C=FALSE;
			sValue=xml.GetAttrib("Downside 90°break");
			if(sValue.CompareNoCase("Yes")==0)
				downLinkSP.m_bBreakXueBan90C=TRUE;
			else if(sValue.CompareNoCase("No")==0)
				downLinkSP.m_bBreakXueBan90C=FALSE;
#else
			sValue=xml.GetAttrib("焊接底板");
			if(sValue.CompareNoCase("是")==0)
				m_bWeldDiZuo=TRUE;
			else
				m_bWeldDiZuo=FALSE;
			sValue=xml.GetAttrib("连接靴板");
			if(sValue.CompareNoCase("螺栓")==0)
				upLinkSP.style_bolt0_weld1=downLinkSP.style_bolt0_weld1=0;
			else if(sValue.CompareNoCase("对焊")==0)
				upLinkSP.style_bolt0_weld1=downLinkSP.style_bolt0_weld1=1;
			else if(sValue.CompareNoCase("搭焊")==0)
				upLinkSP.style_bolt0_weld1=downLinkSP.style_bolt0_weld1=2;
			sValue=xml.GetAttrib("水平截断");
			if(sValue.CompareNoCase("是")==0)
				upLinkSP.m_bBreakXueBan0C=TRUE;
			else if(sValue.CompareNoCase("否")==0)
				upLinkSP.m_bBreakXueBan0C=FALSE;
			sValue=xml.GetAttrib("竖直截断");
			if(sValue.CompareNoCase("是")==0)
				upLinkSP.m_bBreakXueBan90C=TRUE;
			else if(sValue.CompareNoCase("否")==0)
				upLinkSP.m_bBreakXueBan90C=FALSE;
			sValue=xml.GetAttrib("下水平截断");
			if(sValue.CompareNoCase("是")==0)
				downLinkSP.m_bBreakXueBan0C=TRUE;
			else if(sValue.CompareNoCase("否")==0)
				downLinkSP.m_bBreakXueBan0C=FALSE;
			sValue=xml.GetAttrib("下竖直截断");
			if(sValue.CompareNoCase("是")==0)
				downLinkSP.m_bBreakXueBan90C=TRUE;
			else if(sValue.CompareNoCase("否")==0)
				downLinkSP.m_bBreakXueBan90C=FALSE;
#endif
			ImportFootDataFromXML(xml,XmlNode("primaryAngle",XmlNode::TYPE_BASICPARAM));
			ImportFootDataFromXML(xml,XmlNode("primaryAngleDown",XmlNode::TYPE_BASICPARAM));
			ImportFootDataFromXML(xml,XmlNode("upLinkShoePlate",XmlNode::TYPE_BASICPARAM));
			ImportFootDataFromXML(xml,XmlNode("downLinkShoePlate",XmlNode::TYPE_BASICPARAM));
			cStateFlag.SetFromTemplFile();
		}
		else if(stricmp(xmlnode.sElemName,"upLinkShoePlate")==0||stricmp(xmlnode.sElemName,"downLinkShoePlate")==0)
		{
			LINKSHOEPALTE* pLinkSP=NULL;
			if(xmlnode.m_iObjType==XmlNode::TYPE_BASICPARAM&&stricmp(xmlnode.sElemName,"upLinkShoePlate")==0)
				pLinkSP=&upLinkSP;
			else if(xmlnode.m_iObjType==XmlNode::TYPE_BASICPARAM&&stricmp(xmlnode.sElemName,"downLinkShoePlate")==0)
				pLinkSP=&downLinkSP;
			//
			sValue=xml.GetAttrib("A");	//台阶高度
			if(sValue.GetLength()>0)
				pLinkSP->A=atoi(sValue);
			sValue=xml.GetAttrib("B");	//顶部余量
			if(sValue.GetLength()>0)
				pLinkSP->B=atoi(sValue);
			sValue=xml.GetAttrib("C");	//内侧台阶高度
			if(sValue.GetLength()>0)
				pLinkSP->C=atoi(sValue);
			sValue=xml.GetAttrib("D");	//内侧顶部余量
			if(sValue.GetLength()>0)
				pLinkSP->D=atoi(sValue);
			sValue=xml.GetAttrib("E");	//斜材楞线侧延伸量
			if(sValue.GetLength()>0)
				pLinkSP->E=atoi(sValue);
			sValue=xml.GetAttrib("F");	//斜材肢翼侧延伸量
			if(sValue.GetLength()>0)
				pLinkSP->F=atoi(sValue);
			sValue=xml.GetAttrib("M");	//边缘间隙
			if(sValue.GetLength()>0)
				pLinkSP->M=atoi(sValue);
			sValue=xml.GetAttrib("AL");	//延伸量
			if(sValue.GetLength()>0)
				pLinkSP->AL=atoi(sValue);
#ifdef AFX_TARG_ENU_ENGLISH
			sValue=xml.GetAttrib("CM");			//倒角模式
			if(sValue.CompareNoCase("Cut Angle")==0)
				pLinkSP->CM=0;
			else if(sValue.CompareNoCase("Cut Arc")==0)
				pLinkSP->CM=1;
			sValue=xml.GetAttrib("ShoePlates");	//靴板连接方式
			if(sValue.CompareNoCase("Bolts")==0)
				pLinkSP->style_bolt0_weld1=0;
			else if(sValue.CompareNoCase("ButtWelding")==0)
				pLinkSP->style_bolt0_weld1=1;
			else if(sValue.CompareNoCase("LapWelding")==0)
				pLinkSP->style_bolt0_weld1=2;
			sValue=xml.GetAttrib("0°break");	//水平截断
			if(sValue.CompareNoCase("Yes")==0)
				pLinkSP->m_bBreakXueBan0C=TRUE;
			else if(sValue.CompareNoCase("No")==0)
				pLinkSP->m_bBreakXueBan0C=FALSE;
			sValue=xml.GetAttrib("90°break");	//竖直截断
			if(sValue.CompareNoCase("Yes")==0)
				pLinkSP->m_bBreakXueBan90C=TRUE;
			else if(sValue.CompareNoCase("No")==0)
				pLinkSP->m_bBreakXueBan90C=FALSE;
#else
			sValue=xml.GetAttrib("CM");			//倒角模式
			if(sValue.CompareNoCase("切角")==0)
				pLinkSP->CM=0;
			else if(sValue.CompareNoCase("切弧")==0)
				pLinkSP->CM=1;
			sValue=xml.GetAttrib("连接靴板");
			if(sValue.CompareNoCase("螺栓")==0)
				pLinkSP->style_bolt0_weld1=0;
			else if(sValue.CompareNoCase("对焊")==0)
				pLinkSP->style_bolt0_weld1=1;
			else if(sValue.CompareNoCase("搭焊")==0)
				pLinkSP->style_bolt0_weld1=2;
			sValue=xml.GetAttrib("水平截断");
			if(sValue.CompareNoCase("是")==0)
				pLinkSP->m_bBreakXueBan0C=TRUE;
			else if(sValue.CompareNoCase("否")==0)
				pLinkSP->m_bBreakXueBan0C=FALSE;
			sValue=xml.GetAttrib("竖直截断");
			if(sValue.CompareNoCase("是")==0)
				pLinkSP->m_bBreakXueBan90C=TRUE;
			else if(sValue.CompareNoCase("否")==0)
				pLinkSP->m_bBreakXueBan90C=FALSE;
#endif
		}
		else if(stricmp(xmlnode.sElemName,"BasePlate")==0)
		{
			sValue=xml.GetAttrib("shape");
			if(sValue.CompareNoCase("square")==0)
				base.shape=0;
			else
				base.shape=1;
			base.cStateFlag.SetFromTemplFile();
			sValue=xml.GetAttrib("thick");
			base.thick=atoi(sValue);
			sValue=xml.GetAttrib("material");
			base.cMaterial=sValue[0];
			sValue=xml.GetAttrib("PartLabel");
			base.sPartNo.Copy(sValue.GetString());
#ifdef AFX_TARG_ENU_ENGLISH
			sValue=xml.GetAttrib("offset type");
			if(sValue.CompareNoCase("upper plane")==0)
				base.cNormOffsetType=0;
			else if(sValue.CompareNoCase("under plane")==0)
				base.cNormOffsetType=1;
			else if(sValue.CompareNoCase("specified offset")==0)
				base.cNormOffsetType=2;
#else
			sValue=xml.GetAttrib("偏移类型");
			if(sValue.CompareNoCase("上平面")==0)
				base.cNormOffsetType=0;
			else if(sValue.CompareNoCase("下平面")==0)
				base.cNormOffsetType=1;
			else if(sValue.CompareNoCase("指定偏移")==0)
				base.cNormOffsetType=2;
#endif
			sValue=xml.GetAttrib("nNormOffset");
			base.nNormOffset=atoi(sValue);
			if(base.shape==0)
				ImportFootDataFromXML(xml,"EDGEDIST");
			else
			{
				sValue=xml.GetAttrib("fR");
				base.fR=atof(sValue);
			}
			//
			ImportFootDataFromXML(xml,"HOLEINFO");
			ImportFootDataFromXML(xml,"LINKANGLE");
		}
		else if(stricmp(xmlnode.sElemName,"ShoePlate")==0)
		{
			sValue=xml.GetAttrib("label");
			SHOEPLATE* pShoePlate=listShoePlates.Add(KEY4C(sValue));
			pShoePlate->cStateFlag.SetFromTemplFile();
			sValue=xml.GetAttrib("thick");
			pShoePlate->thick=atoi(sValue);
			sValue=xml.GetAttrib("material");
			pShoePlate->cMaterial=sValue[0];
			sValue=xml.GetAttrib("tranverse");
			if(sValue.CompareNoCase("True")==0)
				pShoePlate->m_bTranverse=true;	//贯穿底板的通板
			else
				pShoePlate->m_bTranverse=false;	//中间分断的靴板
			sValue=xml.GetAttrib("PartLabel");
			pShoePlate->sPartNo.Copy(sValue);
			sValue=xml.GetAttrib("CutWidth");
			pShoePlate->fCutAngleWidth=atof(sValue);
			sValue=xml.GetAttrib("NL");
			pShoePlate->norm_offset=atoi(sValue);
			ImportFootDataFromXML(xml,XmlNode("LINKANGLE",XmlNode::TYPE_SHOEPLATE,pShoePlate));
		}
		else if(stricmp(xmlnode.sElemName,"RibPlate")==0)
		{	//肋板参数
			sValue=xml.GetAttrib("serial");
			RIBPLATE* pRibPlate=listRibPlates.Add(atoi(sValue));
			pRibPlate->cStateFlag.SetFromTemplFile();
			sValue=xml.GetAttrib("thick");
			pRibPlate->thick=atoi(sValue);
			sValue=xml.GetAttrib("material");
			pRibPlate->cMaterial=sValue[0];
			sValue=xml.GetAttrib("PartLabel");
			pRibPlate->sPartNo.Copy(sValue);
			sValue=xml.GetAttrib("Upper");	//肋板高度
			if(sValue.CompareNoCase("False")==0)
				pRibPlate->upper=false;
			else
				pRibPlate->upper=true;
			sValue=xml.GetAttrib("A");	//肋板高度
			pRibPlate->A=atoi(sValue);
			sValue=xml.GetAttrib("B");	//底边宽度
			pRibPlate->B=atoi(sValue);
			sValue=xml.GetAttrib("C");	//台阶高度
			pRibPlate->C=atoi(sValue);
			sValue=xml.GetAttrib("D");	//顶部宽度
			pRibPlate->D=atoi(sValue);
			sValue=xml.GetAttrib("e");	//倒角模式
#ifdef AFX_TARG_ENU_ENGLISH
			if(sValue.CompareNoCase("Cut Arc")==0)
#else
			if(sValue.CompareNoCase("切弧")==0)
#endif
				pRibPlate->iCutMode=1;
			else
				pRibPlate->iCutMode=0;
			sValue=xml.GetAttrib("E");	//切角宽度
			pRibPlate->fCutAngleWidth=atoi(sValue);
			sValue=xml.GetAttrib("Ga");	//倾斜角度，°
			pRibPlate->fGradientAngle=atof(sValue);
			sValue=xml.GetAttrib("MH");
			pRibPlate->MH=atoi(sValue);
			if(pRibPlate->MH>0)
			{
				sValue=xml.GetAttrib("MF");
				pRibPlate->MF=atoi(sValue);
				sValue=xml.GetAttrib("MG");
				pRibPlate->MG=atoi(sValue);
			}
			sValue=xml.GetAttrib("axisX");	//焊缝走向
#ifdef AFX_TARG_ENU_ENGLISH
			if(sValue.CompareNoCase("Specified-angle")==0)
				pRibPlate->axisXType=0;
			else if(sValue.CompareNoCase("Specified-vertical-line-angle")==0)
				pRibPlate->axisXType=5;
#else
			if(sValue.CompareNoCase("指定角度")==0)
				pRibPlate->axisXType=0;
			else if(sValue.CompareNoCase("指定垂线角度")==0)
				pRibPlate->axisXType=5;
#endif
			else if(sValue.CompareNoCase("X+")==0)
				pRibPlate->axisXType=1;
			else if(sValue.CompareNoCase("X-")==0)
				pRibPlate->axisXType=2;
			else if(sValue.CompareNoCase("Y+")==0)
				pRibPlate->axisXType=3;
			else //if(sValue.CompareNoCase("Y-")==0)
				pRibPlate->axisXType=4;
			if(pRibPlate->axisXType==0)
			{
				sValue=xml.GetAttrib("angle");		//射线角度
				pRibPlate->pos_para.PARA2.angle=atof(sValue);
			}
			else if(pRibPlate->axisXType==5)
			{
				sValue=xml.GetAttrib("angle");		//射线角度
				pRibPlate->pos_para.PARA3.angle=atof(sValue);
				sValue=xml.GetAttrib("fRayDist");	//射线距离
				pRibPlate->pos_para.PARA3.fRayDist=atof(sValue);
			}
			else
			{
				sValue=xml.GetAttrib("Xx");			//焊缝走向
				pRibPlate->pos_para.PARA1.axisX_x=atoi(sValue);
				sValue=xml.GetAttrib("Xy");			//焊缝走向
				pRibPlate->pos_para.PARA1.axisX_y=atoi(sValue);
				sValue=xml.GetAttrib("Ox");			//定位原点
				pRibPlate->pos_para.PARA1.OX=atoi(sValue);
				sValue=xml.GetAttrib("Oy");			//定位原点
				pRibPlate->pos_para.PARA1.OY=atoi(sValue);
			}
		}
		else if(stricmp(xmlnode.sElemName,"EDGEDIST")==0)
		{
			sValue=xml.GetAttrib("XA");
			base.XA=atoi(sValue);
			sValue=xml.GetAttrib("XB");
			base.XB=atoi(sValue);
			sValue=xml.GetAttrib("XC0");
			base.XC[0]=atoi(sValue);
			sValue=xml.GetAttrib("XC1");
			base.XC[1]=atoi(sValue);
			sValue=xml.GetAttrib("XC2");
			base.XC[2]=atoi(sValue);
			sValue=xml.GetAttrib("XC3");
			base.XC[3]=atoi(sValue);
			sValue=xml.GetAttrib("YA");
			base.YA=atoi(sValue);
			sValue=xml.GetAttrib("YB");
			base.YB=atoi(sValue);
			sValue=xml.GetAttrib("YC0");
			base.YC[0]=atoi(sValue);
			sValue=xml.GetAttrib("YC1");
			base.YC[1]=atoi(sValue);
			sValue=xml.GetAttrib("YC2");
			base.YC[2]=atoi(sValue);
			sValue=xml.GetAttrib("YC3");
			base.YC[3]=atoi(sValue);
		}
		else if(stricmp(xmlnode.sElemName,"LINKANGLE")==0||
			stricmp(xmlnode.sElemName,"primaryAngle")==0||stricmp(xmlnode.sElemName,"primaryAngleDown")==0)
		{
			LINKANGLE* pLinkAngle=NULL;
			sValue=xml.GetAttrib("label");
			if(xmlnode.m_iObjType==XmlNode::TYPE_BASICPARAM&&stricmp(xmlnode.sElemName,"primaryAngle")==0)
				pLinkAngle=&primaryAngle;
			else if(xmlnode.m_iObjType==XmlNode::TYPE_BASICPARAM&&stricmp(xmlnode.sElemName,"primaryAngleDown")==0)
				pLinkAngle=&primaryAngleDown;
			else if(xmlnode.m_iObjType==XmlNode::TYPE_SHOEPLATE&&xmlnode.pRelaObj)
			{
				SHOEPLATE* pShoePlate=(SHOEPLATE*)xmlnode.pRelaObj;
				pLinkAngle=pShoePlate->linkAngles.Add(KEY4C(sValue));
			}
			else
				pLinkAngle=base.linkAngles.Add(KEY4C(sValue));
			pLinkAngle->cStateFlag.SetFromTemplFile();
			//sValue=xml.GetAttrib("handle");
			//long handle=HexStrToLong(sValue);
			//if(handle>0x20)
			//	pLinkAngle->pAngle=(CLDSLineAngle*)console.FromPartHandle(handle);
			sValue=xml.GetAttrib("boltD");
			pLinkAngle->bolt.d=atoi(sValue);
			sValue=xml.GetAttrib("boltNum");
			pLinkAngle->bolt.num=atoi(sValue);
			sValue=xml.GetAttrib("BoltSpace");
			pLinkAngle->bolt.siBoltSpace=atoi(sValue);
			sValue=xml.GetAttrib("EndSpace");
			pLinkAngle->bolt.siEndSpace=atoi(sValue);
			sValue=xml.GetAttrib("AngleWidth");
			pLinkAngle->angleWidth=sValue.GetLength()>0?atof(sValue):0;
			sValue=xml.GetAttrib("g");
			pLinkAngle->g=atoi(sValue);
			sValue=xml.GetAttrib("g1");
			pLinkAngle->g1=atoi(sValue);
			sValue=xml.GetAttrib("g2");
			pLinkAngle->g2=atoi(sValue);
#ifdef AFX_TARG_ENU_ENGLISH
			sValue=xml.GetAttrib("BoltRows");
			if(sValue.CompareNoCase("Singular")==0)
				pLinkAngle->bolt.rows=1;
			else if(sValue.CompareNoCase("DoubleRows")==0)
				pLinkAngle->bolt.rows=2;
			sValue=xml.GetAttrib("BoltsLayout");
			if(sValue.CompareNoCase("LhRl")==0)
				pLinkAngle->bolt.layout=0;
			else if(sValue.CompareNoCase("LlRh")==0)
				pLinkAngle->bolt.layout=1;
#else
			sValue=xml.GetAttrib("螺栓排数");
			if(sValue.CompareNoCase("单排")==0)
				pLinkAngle->bolt.rows=1;
			else if(sValue.CompareNoCase("双排")==0)
				pLinkAngle->bolt.rows=2;
			sValue=xml.GetAttrib("螺栓布置");
			if(sValue.CompareNoCase("左高右低")==0)
				pLinkAngle->bolt.layout=0;
			else if(sValue.CompareNoCase("左低右高")==0)
				pLinkAngle->bolt.layout=1;
#endif
		}
		else if(stricmp(xmlnode.sElemName,"HOLEINFO")==0)
		{
			CFootTempl::BASEPLATE::HOLE  ls_hole;
			sValue=xml.GetAttrib("d");
			ls_hole.d=atoi(sValue);
			sValue=xml.GetAttrib("hole_d");
			ls_hole.hole_d=atoi(sValue);
			sValue=xml.GetAttrib("X");
			ls_hole.x=atoi(sValue);
			sValue=xml.GetAttrib("Y");
			ls_hole.y=atoi(sValue);
			base.listHoles.AttachNode(ls_hole);
		}
		xml.OutOfElem();
	}
	return true;
}
void CFootTempl::ExportXmlNodeLinkAngle(CMarkup& xml,LINKANGLE* pLinkAngle)
{
	xml.AddAttrib("label",pLinkAngle->label.Label());
	//xml.AddAttrib("handle",(char*)CXhChar16("0X%X",pLinkAngle->pAngle->handle));
	xml.AddAttrib("boltD",(char*)CXhChar16("%d",pLinkAngle->bolt.d));
	xml.AddAttrib("boltNum",(char*)CXhChar16("%d",pLinkAngle->bolt.num));
	xml.AddAttrib("BoltSpace",(char*)CXhChar16("%d",pLinkAngle->bolt.siBoltSpace));
	xml.AddAttrib("EndSpace",(char*)CXhChar16("%d",pLinkAngle->bolt.siEndSpace));
	if(pLinkAngle->pAngle)
		xml.AddAttrib("AngleWidth",(char*)CXhChar16("%.0f",pLinkAngle->pAngle->size_wide));
	xml.AddAttrib("g",(char*)CXhChar16("%d",pLinkAngle->g));
	xml.AddAttrib("g1",(char*)CXhChar16("%d",pLinkAngle->g1));
	xml.AddAttrib("g2",(char*)CXhChar16("%d",pLinkAngle->g2));
	char sText[16]="";
#ifdef AFX_TARG_ENU_ENGLISH
	if(pLinkAngle->bolt.rows==1)
		strcpy(sText,"Singular");
	else if(pLinkAngle->bolt.rows==2)
		strcpy(sText,"DoubleRows");
	xml.AddAttrib("BoltRows",sText);
	if(pLinkAngle->bolt.layout==0)
		strcpy(sText,"LhRl");
	else if(pLinkAngle->bolt.layout==1)
		strcpy(sText,"LlRh");
	xml.AddAttrib("BoltsLayout",sText);
#else
	if(pLinkAngle->bolt.rows==1)
		strcpy(sText,"单排");
	else if(pLinkAngle->bolt.rows==2)
		strcpy(sText,"双排");
	xml.AddAttrib("螺栓排数",sText);
	if(pLinkAngle->bolt.layout==0)
		strcpy(sText,"左高右低");
	else if(pLinkAngle->bolt.layout==1)
		strcpy(sText,"左低右高");
	xml.AddAttrib("螺栓布置",sText);
#endif
}
void CFootTempl::ExprotXmlNodeLinkXueBan(CMarkup& xml,LINKSHOEPALTE* pLinkShoePlate)
{
	CString sText;
	xml.AddAttrib("A",(char*)CXhChar16("%d",pLinkShoePlate->A));	//靴板台阶高
	xml.AddAttrib("B",(char*)CXhChar16("%d",pLinkShoePlate->B));	//顶部前侧余量
	xml.AddAttrib("C",(char*)CXhChar16("%d",pLinkShoePlate->C));	//内侧台阶高
	xml.AddAttrib("D",(char*)CXhChar16("%d",pLinkShoePlate->D));	//顶部内侧余量
	xml.AddAttrib("E",(char*)CXhChar16("%d",pLinkShoePlate->E));	//斜材楞线侧侧延伸量
	xml.AddAttrib("F",(char*)CXhChar16("%d",pLinkShoePlate->F));	//斜材肢翼侧延伸量
	xml.AddAttrib("M",(char*)CXhChar16("%d",pLinkShoePlate->M));	//靴板延伸至底板边缘时的间隙值M==0表示延伸至底板边缘
	xml.AddAttrib("AL",(char*)CXhChar16("%d",pLinkShoePlate->AL));	//靴板非连接侧角钢长度方向延伸量，默认根据角钢螺栓计算	
#ifdef AFX_TARG_ENU_ENGLISH
	sText=(pLinkShoePlate->CM==0)?"Cut Angle":"Cut Arc";
	xml.AddAttrib("CM",sText);	//靴板切角模式
	if(pLinkShoePlate->style_bolt0_weld1==0)
		sText="Bolts";
	else if(pLinkShoePlate->style_bolt0_weld1==1)
		sText="ButtWelding";
	else
		sText="LapWelding";
	xml.AddAttrib("ShoePlates",sText);			//靴板连接方式
	sText=pLinkShoePlate->m_bBreakXueBan0C?"Yes":"No";
	xml.AddAttrib("Downside 0°break",sText);	//沿水平截断
	sText=pLinkShoePlate->m_bBreakXueBan90C?"Yes":"No";
	xml.AddAttrib("Downside 90°break",sText);	//沿竖直截断
#else
	sText=(pLinkShoePlate->CM==0)?"切角":"切弧";
	xml.AddAttrib("CM",sText);	//靴板切角模式
	if(pLinkShoePlate->style_bolt0_weld1==0)
		sText="螺栓";
	else if(pLinkShoePlate->style_bolt0_weld1==1)
		sText="对焊";
	else
		sText="搭焊";
	xml.AddAttrib("连接靴板",sText);
	sText=pLinkShoePlate->m_bBreakXueBan0C?"是":"否";
	xml.AddAttrib("水平截断",sText);
	sText=pLinkShoePlate->m_bBreakXueBan90C?"是":"否";
	xml.AddAttrib("竖直截断",sText);
#endif
}
bool CFootTempl::ExportFootDataToXML(CMarkup& xml)
{
	CString sText;
	//1、基本参数
	xml.IntoElem();
	xml.AddElem("BasicParams");	//添加节间
	xml.AddAttrib("C",(char*)CXhChar16("%d",C));	//角钢楞点到底板距离
#ifdef AFX_TARG_ENU_ENGLISH
	sText=m_bWeldDiZuo?"Yes":"No";
	xml.AddAttrib("WeldingBasePlate",sText);	//是否焊接底板
#else
	sText=m_bWeldDiZuo?"是":"否";
	xml.AddAttrib("焊接底板",sText);
#endif
	//连接角钢信息
	xml.IntoElem();
	xml.AddElem("primaryAngle");
	ExportXmlNodeLinkAngle(xml,&primaryAngle);
	xml.OutOfElem();		//跳出</primaryAngle>
	if(primaryAngleDown.pAngle)
	{
		xml.IntoElem();
		xml.AddElem("primaryAngleDown");
		ExportXmlNodeLinkAngle(xml,&primaryAngleDown);
		xml.OutOfElem();	//跳出</primaryAngleDown>
	}
	//连接靴板信息
	xml.IntoElem();
	xml.AddElem("upLinkShoePlate");
	ExprotXmlNodeLinkXueBan(xml,&upLinkSP);
	xml.OutOfElem();		//跳出</upLinkShoePlate>
	if(primaryAngleDown.pAngle)
	{	//下侧靴板连接信息
		xml.IntoElem();
		xml.AddElem("downLinkShoePlate");
		ExprotXmlNodeLinkXueBan(xml,&downLinkSP);
		xml.OutOfElem();		//跳出</downLinkShoePlate>
	}
	xml.OutOfElem();		//跳出</BasicParams>
	//2、底板参数
	xml.IntoElem();
	xml.AddElem("BasePlate");
	if(base.shape==0)
		sText="square";
	else if(base.shape==1)
		sText="circle";
	xml.AddAttrib("shape",sText);
	xml.AddAttrib("thick",(char*)CXhChar16("%d",base.thick));//底板厚
	xml.AddAttrib("material",(char*)CXhChar16("%C",base.cMaterial));	//底板材质
	xml.AddAttrib("PartLabel",(char*)base.sPartNo);	//底板编号
#ifdef AFX_TARG_ENU_ENGLISH
	if(base.cNormOffsetType==0)
		sText="upper plane";
	else if(base.cNormOffsetType==1)
		sText="under plane";
	else
		sText="specified offset";
	xml.AddAttrib("offset type",sText);	//法线偏移方式
#else
	if(base.cNormOffsetType==0)
		sText="上平面";
	else if(base.cNormOffsetType==1)
		sText="下平面";
	else
		sText="指定偏移";
	xml.AddAttrib("偏移类型",sText);	//法线偏移方式
#endif
	xml.AddAttrib("nNormOffset",(char*)CXhChar16("%d",base.nNormOffset));	//法线偏移距离
	if(base.shape==0)
	{
		xml.IntoElem();
		xml.AddElem("EDGEDIST");
		xml.AddAttrib("XA",(char*)CXhChar16("%d",base.XA));
		xml.AddAttrib("XB",(char*)CXhChar16("%d",base.XB));
		for(int i=0;i<4;i++)
		{
			CXhChar16 label("XC%d",i);
			xml.AddAttrib((char*)label,(char*)CXhChar16("%d",base.XC[i]));
		}
		xml.AddAttrib("YA",(char*)CXhChar16("%d",base.YA));
		xml.AddAttrib("YB",(char*)CXhChar16("%d",base.YB));
		for(int i=0;i<4;i++)
		{
			CXhChar16 label("YC%d",i);
			xml.AddAttrib((char*)label,(char*)CXhChar16("%d",base.YC[i]));
		}
		xml.OutOfElem();
	}
	else if(base.shape==1)
		xml.AddAttrib("fR",(char*)CXhChar16("%f",base.fR));	//半径
	for(LIST_NODE<BASEPLATE::HOLE> *pListNode=base.listHoles.EnumFirst();pListNode;pListNode=base.listHoles.EnumNext())
	{
		xml.IntoElem();
		xml.AddElem("HOLEINFO");
		xml.AddAttrib("d",(char*)CXhChar16("%d",pListNode->data.d));
		xml.AddAttrib("hole_d",(char*)CXhChar16("%d",pListNode->data.hole_d));
		xml.AddAttrib("X",(char*)CXhChar16("%d",pListNode->data.x));
		xml.AddAttrib("Y",(char*)CXhChar16("%d",pListNode->data.y));
		xml.OutOfElem();
	}
	for(LINKANGLE* pLinkAngle=base.linkAngles.GetFirst();pLinkAngle;pLinkAngle=base.linkAngles.GetNext())
	{
		xml.IntoElem();
		xml.AddElem("LINKANGLE");
		ExportXmlNodeLinkAngle(xml,pLinkAngle);
		xml.OutOfElem();	//跳出</LINKANGLE>
	}
	xml.OutOfElem();	
	//3、上靴板参数
	for(SHOEPLATE* pShoePlate=listShoePlates.GetFirst();pShoePlate;pShoePlate=listShoePlates.GetNext())
	{
		xml.IntoElem();
		xml.AddElem("ShoePlate");
		xml.AddAttrib("label",pShoePlate->label.Label());	//标签
		xml.AddAttrib("thick",(char*)CXhChar16("%d",pShoePlate->thick));//靴板厚
		xml.AddAttrib("material",(char*)CXhChar16("%C",pShoePlate->cMaterial));	//靴板材质
		if(pShoePlate->m_bTranverse)
			xml.AddAttrib("tranverse","True");	//贯穿底板的通板
		else
			xml.AddAttrib("tranverse","False");//中间分断的靴板
		xml.AddAttrib("PartLabel",(char*)pShoePlate->sPartNo);//靴板厚
		xml.AddAttrib("CutWidth",(char*)CXhChar16("%g",pShoePlate->fCutAngleWidth));	//内倒角宽度
		xml.AddAttrib("NL",(char*)CXhChar16("%d",pShoePlate->norm_offset));	//沿法线方向偏移距离
		//连接角钢
		for(LINKANGLE* pLinkAngle=pShoePlate->linkAngles.GetFirst();pLinkAngle;pLinkAngle=pShoePlate->linkAngles.GetNext())
		{
			xml.IntoElem();
			xml.AddElem("LINKANGLE");
			ExportXmlNodeLinkAngle(xml,pLinkAngle);
			xml.OutOfElem();	//</LINKANGLE>
		}
		xml.OutOfElem();
	}
	//4、肋板参数
	for(RIBPLATE* pRibPlate=listRibPlates.GetFirst();pRibPlate;pRibPlate=listRibPlates.GetNext())
	{
		xml.IntoElem();
		xml.AddElem("RibPlate");
		xml.AddAttrib("serial",pRibPlate->Serial());	//标签
		xml.AddAttrib("thick",(char*)CXhChar16("%d",pRibPlate->thick));//靴板厚
		xml.AddAttrib("material",(char*)CXhChar16("%C",pRibPlate->cMaterial));	//靴板材质
		xml.AddAttrib("PartLabel",(char*)pRibPlate->sPartNo);	//肋板编号
		if(pRibPlate->upper)
			xml.AddAttrib("Upper","True");	//肋板编号
		else
			xml.AddAttrib("Upper","False");	//肋板编号
		xml.AddAttrib("A",pRibPlate->A);	//肋板高度
		xml.AddAttrib("B",pRibPlate->B);	//底边宽度
		xml.AddAttrib("C",pRibPlate->C);	//台阶高度
		xml.AddAttrib("D",pRibPlate->D);	//顶部宽度
#ifdef AFX_TARG_ENU_ENGLISH
		if(pRibPlate->iCutMode==0)
			xml.AddAttrib("e",(char*)CXhChar16("Cut Angle"));
		else
			xml.AddAttrib("e",(char*)CXhChar16("Cut Arc"));
#else
		if(pRibPlate->iCutMode==0)
			xml.AddAttrib("e",(char*)CXhChar16("切角"));
		else
			xml.AddAttrib("e",(char*)CXhChar16("切弧"));
#endif
		xml.AddAttrib("E",(char*)CXhChar16("%g",pRibPlate->fCutAngleWidth));	//切角宽度
		xml.AddAttrib("Ga",(char*)CXhChar16("%g",pRibPlate->fGradientAngle));	//倾斜角度，°
		xml.AddAttrib("MH",pRibPlate->MH);
		if(pRibPlate->MH>0)
		{
			xml.AddAttrib("MF",pRibPlate->MF);
			xml.AddAttrib("MG",pRibPlate->MG);
		}
		if(pRibPlate->axisXType==0)
#ifdef AFX_TARG_ENU_ENGLISH
			xml.AddAttrib("axisX",(char*)CXhChar16("Specified-angle"));
		else if(pRibPlate->axisXType==5)
			xml.AddAttrib("axisX",(char*)CXhChar16("Specified-vertical-line-angle"));
#else
			xml.AddAttrib("axisX",(char*)CXhChar16("指定角度"));
		else if(pRibPlate->axisXType==5)
			xml.AddAttrib("axisX",(char*)CXhChar16("指定垂线角度"));
#endif
		else if(pRibPlate->axisXType==1)
			xml.AddAttrib("axisX",(char*)CXhChar16("X+"));
		else if(pRibPlate->axisXType==2)
			xml.AddAttrib("axisX",(char*)CXhChar16("X-"));
		else if(pRibPlate->axisXType==3)
			xml.AddAttrib("axisX",(char*)CXhChar16("Y+"));
		else
			xml.AddAttrib("axisX",(char*)CXhChar16("Y-"));
		if(pRibPlate->axisXType==0)
			xml.AddAttrib("angle",(char*)CXhChar16("%g",pRibPlate->pos_para.PARA2.angle));		//射线角度
		if(pRibPlate->axisXType==5)
		{
			xml.AddAttrib("angle",(char*)CXhChar16("%g",pRibPlate->pos_para.PARA3.angle));		//射线角度
			xml.AddAttrib("fRayDist",(char*)CXhChar16("%g",pRibPlate->pos_para.PARA3.fRayDist));//射线距离
		}	
		else
		{
			xml.AddAttrib("Xx",(char*)CXhChar16("%g",pRibPlate->pos_para.PARA1.axisX_x));	//焊缝走向
			xml.AddAttrib("Xy",(char*)CXhChar16("%g",pRibPlate->pos_para.PARA1.axisX_y));	//焊缝走向
			xml.AddAttrib("Ox",(char*)CXhChar16("%g",pRibPlate->pos_para.PARA1.OX));		//定位原点
			xml.AddAttrib("Oy",(char*)CXhChar16("%g",pRibPlate->pos_para.PARA1.OY));		//定位原点
		}
		xml.OutOfElem();
	}
	return true;
}
BOOL CFootTempl::ToXmlTemplFile(const char* filename)
{	//导出XML塔脚设计模板文件
	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("FOOT");
#ifdef AFX_TARG_ENU_ENGLISH
	xml.AddAttrib("type","angle");
#else
	xml.AddAttrib("类型","角钢");
#endif
	ExportFootDataToXML(xml);
	return xml.Save(filename);
}
BOOL CFootTempl::FromXmlTemplFile(const char* filename)
{	//导入XML塔脚设计模板文件
	CMarkup xml;
	if(!xml.Load(filename))
	{	//加载XML文件
		return FALSE;	//由于加载默认文件不一定成功，故此时加载失败不进行错误提示
	}
	xml.ResetMainPos();
	ImportFootDataFromXML(xml,"BasicParams");
	ImportFootDataFromXML(xml,"BasePlate");
	ImportFootDataFromXML(xml,"ShoePlate");
	ImportFootDataFromXML(xml,"RibPlate");
	return TRUE;
}
void CFootTempl::GetUCS(UCS_STRU& ucs,bool bReUpdateCS/*=true*/)
{
	CLDSNode *pNode = console.FromNodeHandle(m_hBaseNode);
	if(pNode==NULL)
		return;
	if(!bReUpdateCS)
	{
		ucs=tempory.ucs;
		return;
	}
	CLDSLineAngle* pBaseJg=primaryAngle.pAngle;
	//计算底脚板相对坐标系原点
	ucs.origin=pNode->Position(true);
	double offset_gx=0,offset_gy=0;
	if(pBaseJg->GetClassTypeId()!=CLS_GROUPLINEANGLE&&primaryAngleDown.pAngle==NULL)	//不是组合角钢时原点在半角钢心距处
	{
		/*JGZJ JgZhunJuX=pBaseJg->GetZhunJu('X');
		JGZJ JgZhunJuY=pBaseJg->GetZhunJu('Y');
		offset_gx=JgZhunJuX.g*0.5;
		offset_gy=JgZhunJuY.g*0.5;*/
		//计算塔脚板原点位置时(刘伟建议)不考虑特殊准据 wht 16-10-14
		JGZJ jgzj;
		if(!getjgzj(jgzj,pBaseJg->GetWidth()))
			jgzj.g=ftoi(pBaseJg->GetWidth()*0.5);
		offset_gx=offset_gy=jgzj.g*0.5;
		/*
		f3dPoint JgBer;
		BOOL bHorizOffset=FALSE;
		if(pBaseJg->pStart->handle==m_hBaseNode)
		{
			JgBer=pBaseJg->Start();
			if(pBaseJg->desStartPos.IsFaceOffset())	//平推
				bHorizOffset=TRUE;
		}
		else
		{
			JgBer=pBaseJg->End();
			if(pBaseJg->desEndPos.IsFaceOffset())
				bHorizOffset=TRUE;
		}
		f3dPoint vertex1,vertex2,jg_vec;
		jg_vec =pBaseJg->End()-pBaseJg->Start();
		if(bHorizOffset)	
		{
			f3dLine line;
			line.startPt=pBaseJg->Start()+pBaseJg->GetWingVecX()*JgZhunJuX.g;
			line.endPt=pBaseJg->End()+pBaseJg->GetWingVecX()*JgZhunJuX.g;
			Int3dlf(vertex1,line,ucs.origin,f3dPoint(0,0,1));
			line.startPt=pBaseJg->Start()+pBaseJg->GetWingVecY()*JgZhunJuY.g;
			line.endPt=pBaseJg->End()+pBaseJg->GetWingVecY()*JgZhunJuY.g;
			Int3dlf(vertex2,line,ucs.origin,f3dPoint(0,0,1));
		}
		else
		{
			vertex1=JgBer+pBaseJg->GetWingVecX()*JgZhunJuX.g;
			vertex2=JgBer+pBaseJg->GetWingVecY()*JgZhunJuY.g;
		}
		ucs.origin=0.5*(vertex1+vertex2);	//
		double minz=min(vertex1.z,vertex2.z);
		if(fabs(ucs.origin.z-minz)>EPS)	//不是方塔，两肢心线端点不水平，默认以较高者为准
			Int3dlf(ucs.origin,ucs.origin,jg_vec,f3dPoint(0,0,minz),f3dPoint(0,0,1));
		*/
	}
	//确定底脚板根坐标系方向
	f3dPoint axisVecX,lenVec;
	if(pBaseJg->pStart==pNode)
	{
		lenVec=pBaseJg->End()-pBaseJg->Start();
		if(lenVec.z<0)	//基准角钢在底板上侧
			axisVecX=-pBaseJg->GetWingVecY();
		else
			axisVecX=-pBaseJg->GetWingVecX();
	}
	else if(pBaseJg->pEnd==pNode)
	{
		lenVec=pBaseJg->Start()-pBaseJg->End();
		if(lenVec.z<0)	//基准角钢在底板上侧
			axisVecX=-pBaseJg->GetWingVecX();
		else
			axisVecX=-pBaseJg->GetWingVecY();
	}
	if(ucs.origin.x>0&&ucs.origin.y>0)
		ucs.axis_x.Set(1,0,0);	//1号位
	else if(ucs.origin.x<0&&ucs.origin.y>0)
		ucs.axis_x.Set(0,1,0);	//2号位
	else if(ucs.origin.x>0&&ucs.origin.y<0)
		ucs.axis_x.Set(0,-1,0);	//3号位
	else if(ucs.origin.x<0&&ucs.origin.y<0)
		ucs.axis_x.Set(-1,0,0);	//4号位
	ucs.axis_z=!axis_z.IsZero()?axis_z:f3dPoint(0,0,-1);
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	if(fabs(ucs.axis_z.x)+fabs(ucs.axis_z.y)>EPS)
	{	//非典型塔脚板
		ucs.axis_x=ucs.axis_y^ucs.axis_z;
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		ucs.origin-=(offset_gx*pBaseJg->GetWingVecX()+offset_gy*pBaseJg->GetWingVecY());
	}
	else
	{
		if(fabs(axisVecX.x)>fabs(axisVecX.y)&&axisVecX.x>0)
		{	//1号位
			ucs.origin.x+=offset_gx;
			ucs.origin.y+=offset_gy;
		}
		else if(fabs(axisVecX.x)>fabs(axisVecX.y)&&axisVecX.x<0)
		{
			ucs.origin.x-=offset_gx;
			ucs.origin.y-=offset_gy;
		}
		else if(fabs(axisVecX.x)<fabs(axisVecX.y)&&axisVecX.y>0)
		{
			ucs.origin.x-=offset_gy;
			ucs.origin.y+=offset_gx;
		}
		else if(fabs(axisVecX.x)<fabs(axisVecX.y)&&axisVecX.y<0)
		{
			ucs.origin.x+=offset_gy;
			ucs.origin.y-=offset_gx;
		}
	}
	origin.SetPosition(ucs.origin);
	tempory.ucs=ucs;
	quadModes[0]=quadModes[1]=quadModes[2]=quadModes[3]=false;
	if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		if(pBaseJg->size_cSubClassType=='X')
			quadModes[0]=quadModes[1]=quadModes[2]=quadModes[3]=true;
		else if(pBaseJg->size_cSubClassType=='D')
		{
			if(fabs(axisVecX*ucs.axis_x)<fabs(axisVecX*ucs.axis_y))
				quadModes[0]=quadModes[2]=true;	//异型布置(1号角钢在外侧)
			else
				quadModes[1]=quadModes[3]=true;	//传统布置型式
		}
	}
}
void CFootTempl::UpdateBoltEndSpace(LINKANGLE& line_Angle)
{
	if(line_Angle.pAngle==NULL)
		return;
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,line_Angle.bolt.d);
	if(line_Angle.bolt.rows==1)
		line_Angle.bolt.siBoltSpace=(int)LsSpace.SingleRowSpace;
	else
		line_Angle.bolt.siBoltSpace=(int)LsSpace.doubleRowSpace;
	line_Angle.bolt.siEndSpace=(int)LsSpace.EndSpace;
}
void CFootTempl::UpdateLinkAngleZhunJu(LINKANGLE& line_Angle)
{
	if(line_Angle.pAngle==NULL)
		return;
	JGZJ jgzj;
	if(line_Angle.pAngle->m_bEnableTeG)
		jgzj = line_Angle.pAngle->xWingXZhunJu;
	else
		getjgzj(jgzj,line_Angle.pAngle->GetWidth());
	line_Angle.g =(int)jgzj.g;
	line_Angle.g1=(int)jgzj.g1;
	line_Angle.g2=(int)jgzj.g2;
}
void CFootTempl::UpdateLinkAngleOddment(LINKANGLE& link_angle,bool bUpJg/*=true*/)
{
	if(link_angle.pAngle==NULL)
		return;
	UCS_STRU ucs;
	GetUCS(ucs,false);
	f3dPoint vertice,vertex_x,vertex_y,vertex_up,vertex_down,line_vec;
	f3dPoint datumPlaneOrg=ucs.origin;
	if(bUpJg)
		datumPlaneOrg+=(base.nNormOffset+base.thick)*ucs.axis_z;
	else
		datumPlaneOrg+=base.nNormOffset*ucs.axis_z;
	CLDSLineAngle* pAngle=link_angle.pAngle;
	f3dPoint ber_pos;
	C=max(0,C);
	if(pAngle->pStart->handle==m_hBaseNode)
	{
		line_vec=pAngle->Start()-pAngle->End();
		normalize(line_vec);
		vertex_x = pAngle->Start()+pAngle->GetWingVecX()*pAngle->GetWidth();
		vertex_y = pAngle->Start()+pAngle->GetWingVecY()*pAngle->GetWidth();
		ber_pos = pAngle->Start();
	}
	else
	{
		line_vec=pAngle->End()-pAngle->Start();
		normalize(line_vec);
		vertex_x = pAngle->End()+pAngle->GetWingVecX()*pAngle->GetWidth();
		vertex_y = pAngle->End()+pAngle->GetWingVecY()*pAngle->GetWidth();
		ber_pos = pAngle->End();
	}
	//根据不同的连接方式计算角钢正负头
	double odd=0;
	if(m_bWeldDiZuo)
	{	//主材焊接到底座
		Int3dlf(vertice,ber_pos,line_vec,datumPlaneOrg,ucs.axis_z);
		odd=DISTANCE(vertice,ber_pos);
		if((datumPlaneOrg-ber_pos)*line_vec<0)
			odd*=-1.0;
	}
	else
	{	//主材与底座有间隔
		if(pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pAngle;
			vertex_up=vertex_down=ber_pos;
			for(int i=0;i<4;i++)
			{
				CLDSLineAngle *pSonJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
				if(pSonJg)
				{
					f3dPoint tem_pos;
					if(pAngle->pStart->handle==m_hBaseNode)
						tem_pos=pSonJg->Start();
					else
						tem_pos=pSonJg->End();
					vertice=tem_pos;
					if(vertice.z>vertex_down.z)
						vertex_down=vertice;
					if(vertice.z<vertex_up.z)
						vertex_up=vertice;
					vertice=tem_pos+pSonJg->GetWingVecX()*pSonJg->GetWidth();
					if(vertice.z>vertex_down.z)
						vertex_down=vertice;
					if(vertice.z<vertex_up.z)
						vertex_up=vertice;
					vertice=tem_pos+pSonJg->GetWingVecY()*pSonJg->GetWidth();
					if(vertice.z>vertex_down.z)
						vertex_down=vertice;
					if(vertice.z<vertex_up.z)
						vertex_up=vertice;
				}
			}
		}
		else
		{
			if(vertex_x.z>vertex_y.z)
			{	//X肢更低
				vertex_up=vertex_y;
				vertex_down=vertex_x;
			}
			else
			{	//Y肢更低
				vertex_up=vertex_x;
				vertex_down=vertex_y;
			}
		}
		if(fabs(ucs.axis_z.z)<EPS_COS)
		{
			GEPOINT posRefVertexDown=vertex_down,posRefVertexUp=vertex_up,posRefDatumPlaneOrg=datumPlaneOrg,vRefLineVec=line_vec;
			coord_trans(posRefVertexDown,ucs,FALSE,TRUE);
			coord_trans(posRefVertexUp,ucs,FALSE,TRUE);
			coord_trans(posRefDatumPlaneOrg,ucs,FALSE,TRUE);
			vector_trans(vRefLineVec,ucs,FALSE,TRUE);
			double vertex_down_z=posRefVertexDown.z;
			double vertex_up_z=posRefVertexUp.z;
			double datumPlaneOrgZ=posRefDatumPlaneOrg.z;
			if(bUpJg)	//上侧基准材
			{
				odd= vertex_down_z-datumPlaneOrgZ-C;
				odd*=(-1)*(vRefLineVec.mod()/vRefLineVec.z);
			}
			else					//下侧基准材
			{
				odd= vertex_up_z-datumPlaneOrgZ+C;
				odd*=(-1)*(vRefLineVec.mod()/vRefLineVec.z);
			}
		}
		else if(bUpJg)	//上侧基准材
		{
			odd= vertex_down.z-datumPlaneOrg.z+C;
			odd*=(-1)*(line_vec.mod()/line_vec.z);
		}
		else					//下侧基准材
		{
			odd= vertex_up.z-datumPlaneOrg.z-C;
			odd*=(-1)*(line_vec.mod()/line_vec.z);
		}
	}
	link_angle.oddment=odd;
}
void CFootTempl::RetrieveLinkAngles(SHOEPLATE* pShoePlate/*=NULL*/)
{
	CLDSNode* pBaseNode=Ta.FromNodeHandle(m_hBaseNode);
	if(pBaseNode==NULL)
		return;
	UCS_STRU ucs;
	GetUCS(ucs,false);
	CHashListEx<LINKANGLE> hashlinkAngles;
	for(CLDSLinePart* pAngle=Ta.Parts.GetFirstLinePart();pAngle;pAngle=Ta.Parts.GetNextLinePart())
	{
		if(!pAngle->IsAngle())
			continue;
		if(pAngle->pStart==NULL || pAngle->pEnd==NULL)
			continue;
		if(pAngle->pStart!=pBaseNode && pAngle->pEnd!=pBaseNode)
			continue;
		if(pAngle->GetClassTypeId()==CLS_LINEANGLE&&(
			(primaryAngle.pAngle    !=NULL&&((CLDSLineAngle*)pAngle)->group_father_jg_h==primaryAngle.pAngle->handle)||
			(primaryAngleDown.pAngle!=NULL&&((CLDSLineAngle*)pAngle)->group_father_jg_h==primaryAngleDown.pAngle->handle)))
			continue;	//跳过主角钢的子角钢
		LINKANGLE* pLinkAngle=NULL;
		if(pAngle==primaryAngle.pAngle)
			pLinkAngle=hashlinkAngles.Add(KEY4C("ZA"));	//Z+
		else if(pAngle==primaryAngleDown.pAngle)
			pLinkAngle=hashlinkAngles.Add(KEY4C("ZB"));	//Z-
		if(pLinkAngle==NULL)
		{//提取底板设计的射线角钢
			f3dPoint startPt,endPt,vec;
			startPt=pBaseNode->Position();
			if(pAngle->pStart==pBaseNode)
				endPt=pAngle->pEnd->Position();
			else
				endPt=pAngle->pStart->Position();
			vec=endPt-startPt;
			normalize(vec);
			if(fabs(vec*ucs.axis_z)<=EPS)	//水平面角钢
			{
				coord_trans(endPt,ucs,FALSE);
				double cosax=vec*ucs.axis_x;
				double cosay=vec*ucs.axis_y;
				if(fabs(cosax)>EPS_COS2 && fabs(cosax)>fabs(cosay) && cosax>0)
					pLinkAngle=hashlinkAngles.Add(KEY4C("XA"));	//趋于X轴正向
				else if(fabs(cosax)>EPS_COS2 && fabs(cosax)>fabs(cosay) && cosax<0)
					pLinkAngle=hashlinkAngles.Add(KEY4C("XB"));	//趋于X轴负向
				else if(fabs(cosay)>EPS_COS2 && fabs(cosay)>fabs(cosax) && cosay>0)
					pLinkAngle=hashlinkAngles.Add(KEY4C("YA"));	//趋于Y轴正向
				else if(fabs(cosay)>EPS_COS2 && fabs(cosay)>fabs(cosax) && cosay<0)
					pLinkAngle=hashlinkAngles.Add(KEY4C("YB"));	//趋于Y轴负向
				else if(endPt.x>0 && endPt.y>0)
					pLinkAngle=hashlinkAngles.Add(KEY4C("XY1"));//XY平面第1象限
				else if(endPt.x<0 && endPt.y>0)
					pLinkAngle=hashlinkAngles.Add(KEY4C("XY2"));//XY平面第2象限
				else if(endPt.x<0 && endPt.y<0)
					pLinkAngle=hashlinkAngles.Add(KEY4C("XY3"));//XY平面第3象限
				else if(endPt.x>0 && endPt.y<0)
					pLinkAngle=hashlinkAngles.Add(KEY4C("XY4"));//XY平面第4象限
			}
			else	//非水平面角钢
			{
				BOOL bInFaceUp=FALSE;
				coord_trans(endPt,ucs,FALSE);
				if(endPt.z>0)
					bInFaceUp=TRUE;
				vec.Set(endPt.x,endPt.y);
				normalize(vec);
				double fcos=fabs(vec.x);
				double fcos45=0.707106781;
				if(bInFaceUp)
				{
					if(fcos>fcos45 && endPt.x>0)		//上XZ正平面
						pLinkAngle=hashlinkAngles.Add(KEY4C("UXZA"));
					else if(fcos>fcos45 && endPt.x<0)	//上XZ负平面
						pLinkAngle=hashlinkAngles.Add(KEY4C("UXZB"));
					else if(fcos<fcos45 && endPt.y>0)	//上YZ正平面		
						pLinkAngle=hashlinkAngles.Add(KEY4C("UYZA"));
					else if(fcos<fcos45 && endPt.y<0)	//上YZ负平面
						pLinkAngle=hashlinkAngles.Add(KEY4C("UYZB"));
				}
				else
				{
					if(fcos>fcos45 && endPt.x>0)		//下XZ正平面
						pLinkAngle=hashlinkAngles.Add(KEY4C("DXZA"));
					else if(fcos>fcos45 && endPt.x<0)	//下XZ负平面
						pLinkAngle=hashlinkAngles.Add(KEY4C("DXZB"));
					else if(fcos<fcos45 && endPt.y>0)	//下YZ正平面			
						pLinkAngle=hashlinkAngles.Add(KEY4C("DYZA"));
					else if(fcos<fcos45 && endPt.y<0)	//下YZ负平面
						pLinkAngle=hashlinkAngles.Add(KEY4C("DYZB"));
				}
			}
		}
		if(pLinkAngle)
			pLinkAngle->pAngle=(CLDSLineAngle*)pAngle;
	}
	LINKANGLE* pLinkAngle=NULL;
	if(pShoePlate)	//靴板上的连接角钢
	{
		for(pLinkAngle=pShoePlate->linkAngles.GetFirst();pLinkAngle;pLinkAngle=pShoePlate->linkAngles.GetNext())
			pLinkAngle->cStateFlag.ClearFromModel();
		if(pShoePlate->label.IsEqual(KEY4C("UXB")))//||pShoePlate->label.IsEqual(KEY4C("UXF")))	//水平方向上靴板
		{
			for(LINKANGLE* pCurAngle=hashlinkAngles.GetFirst();pCurAngle;pCurAngle=hashlinkAngles.GetNext())
			{
				if(pCurAngle->label.IsEqual(KEY4C("ZA")) || 
					(pCurAngle->label.CharAt(0)=='U'&&pCurAngle->label.CharAt(1)=='X'))
				{
					pLinkAngle=pShoePlate->linkAngles.Add(pCurAngle->label);
					pLinkAngle->cStateFlag.SetFromModel();
					pLinkAngle->pAngle=pCurAngle->pAngle;
					if(!pLinkAngle->cStateFlag.IsFromTemplFile())
						pLinkAngle->UpdateBoltInfo(pBaseNode);
				}
			}
		}
		else if(pShoePlate->label.IsEqual(KEY4C("UYB")))//||pShoePlate->label.IsEqual(KEY4C("UY2"))	//竖直方向上靴板
		{
			for(LINKANGLE* pCurAngle=hashlinkAngles.GetFirst();pCurAngle;pCurAngle=hashlinkAngles.GetNext())
			{
				if(pCurAngle->label.IsEqual(KEY4C("ZA")) || 
					(pCurAngle->label.CharAt(0)=='U'&&pCurAngle->label.CharAt(1)=='Y'))
				{
					pLinkAngle=pShoePlate->linkAngles.Add(pCurAngle->label);
					pLinkAngle->cStateFlag.SetFromModel();
					pLinkAngle->pAngle=pCurAngle->pAngle;
					if(!pLinkAngle->cStateFlag.IsFromTemplFile())
						pLinkAngle->UpdateBoltInfo(pBaseNode);
				}
			}
		}
		else if(pShoePlate->label.IsEqual(KEY4C("DXB")))//||pShoePlate->label.IsEqual(KEY4C("DXF"))	//水平方向下靴板
		{
			for(LINKANGLE* pCurAngle=hashlinkAngles.GetFirst();pCurAngle;pCurAngle=hashlinkAngles.GetNext())
			{
				if(pCurAngle->label.IsEqual(KEY4C("ZB")) || pCurAngle->label.IsEqual(KEY4C("XA")) || pCurAngle->label.IsEqual(KEY4C("XB")) ||
					(pCurAngle->label.CharAt(0)=='D'&&pCurAngle->label.CharAt(1)=='X'))
				{
					pLinkAngle=pShoePlate->linkAngles.Add(pCurAngle->label);
					pLinkAngle->cStateFlag.SetFromModel();
					pLinkAngle->pAngle=pCurAngle->pAngle;
					if(!pLinkAngle->cStateFlag.IsFromTemplFile())
						pLinkAngle->UpdateBoltInfo(pBaseNode);
				}
			}
		}
		else if(pShoePlate->label.IsEqual(KEY4C("DYB")))//||pShoePlate->label.IsEqual(KEY4C("DYF"))	//竖直方向下靴板
		{
			for(LINKANGLE* pCurAngle=hashlinkAngles.GetFirst();pCurAngle;pCurAngle=hashlinkAngles.GetNext())
			{
				if(pCurAngle->label.IsEqual(KEY4C("ZB")) || pCurAngle->label.IsEqual(KEY4C("YA")) || pCurAngle->label.IsEqual(KEY4C("YB")) ||
					(pCurAngle->label.CharAt(0)=='D'&&pCurAngle->label.CharAt(1)=='Y'))
				{
					pLinkAngle=pShoePlate->linkAngles.Add(pCurAngle->label);
					pLinkAngle->cStateFlag.SetFromModel();
					pLinkAngle->pAngle=pCurAngle->pAngle;
					if(!pLinkAngle->cStateFlag.IsFromTemplFile())
						pLinkAngle->UpdateBoltInfo(pBaseNode);
				}
			}
		}
	}
	else		//底板上的连接角钢
	{
		for(pLinkAngle=base.linkAngles.GetFirst();pLinkAngle;pLinkAngle=base.linkAngles.GetNext())
			pLinkAngle->cStateFlag.ClearFromModel();
		for(LINKANGLE* pCurAngle=hashlinkAngles.GetFirst();pCurAngle;pCurAngle=hashlinkAngles.GetNext())
		{
			if(pCurAngle->label.CharAt(0)!='X' && pCurAngle->label.CharAt(0)!='Y')
				continue;
			pLinkAngle=base.linkAngles.Add(pCurAngle->label);
			pLinkAngle->cStateFlag.SetFromModel();
			pLinkAngle->pAngle=pCurAngle->pAngle;
			if(!pLinkAngle->cStateFlag.IsFromTemplFile())
				pLinkAngle->UpdateBoltInfo(pBaseNode);
		}	
	}
}
void CFootTempl::RetrieveCirularDibanRibPlates()
{
	RIBPLATE legacyRib;
	RIBPLATE* pLegacyRibPlate=listRibPlates.GetFirst();
	if(pLegacyRibPlate!=NULL)
		legacyRib=*pLegacyRibPlate;
	listRibPlates.Empty();
	int i;
	//补充直接焊到组合角钢角分线上的类似靴板的肋板
	for(i=1;i<=4;i++)
	{
		RIBPLATE* pRibPlate=listRibPlates.Add(i);
		pRibPlate->axisXType=0;	//指定角度
		pRibPlate->pos_para.PARA2.angle=(i*2-1)*45;
	}
	for(i=1;i<9;i++)
	{
		RIBPLATE* pRibPlate=listRibPlates.Add(i+4);
		pRibPlate->thick=legacyRib.thick;
		pRibPlate->cMaterial=legacyRib.cMaterial;
		pRibPlate->A=legacyRib.A;
		pRibPlate->axisXType=5;	//指定垂线角度
		pRibPlate->pos_para.PARA3.angle=(i*2-1)*22.5;
		pRibPlate->pos_para.PARA3.fRayDist=250;
	}
}
void CFootTempl::RetrieveShoePlates()
{
	UCS_STRU ucs;
	GetUCS(ucs,false);
	SHOEPLATE* pXShoePlate=NULL,*pYShoePlate=NULL;
	for(SHOEPLATE* pShoePlate=listShoePlates.GetFirst();pShoePlate;pShoePlate=listShoePlates.GetNext())
		pShoePlate->cStateFlag.ClearFromModel();
	CLDSGroupLineAngle* pBaseGroupAngle=NULL;
	if(primaryAngle.pAngle&&primaryAngle.pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		pBaseGroupAngle=(CLDSGroupLineAngle*)primaryAngle.pAngle;
	pXShoePlate=listShoePlates.Add(KEY4C("UXB"));	//X肢翼侧
	pXShoePlate->cStateFlag.SetFromModel();
	if(pBaseGroupAngle)
		pXShoePlate->thick=ftoi(pBaseGroupAngle->jg_space-g_sysPara.m_nGroupJgMachiningGap);
	pXShoePlate->m_bTranverse=!upLinkSP.m_bBreakXueBan0C;
	RetrieveLinkAngles(pXShoePlate);
	pYShoePlate=listShoePlates.Add(KEY4C("UYB"));	//Y肢翼侧
	pYShoePlate->cStateFlag.SetFromModel();
	if(pBaseGroupAngle)
		pYShoePlate->thick=ftoi(pBaseGroupAngle->jg_space-g_sysPara.m_nGroupJgMachiningGap);
	pYShoePlate->m_bTranverse=!upLinkSP.m_bBreakXueBan90C;
	RetrieveLinkAngles(pYShoePlate);
	if(upLinkSP.m_bBreakXueBan0C)
	{
		pShoePlate=listShoePlates.Add(KEY4C("UXF"));
		pShoePlate->thick=pXShoePlate->thick;	//楞线侧
		pShoePlate->cStateFlag.SetFromModel();
	}
	else
		listShoePlates.DeleteNode(KEY4C("UXF"));
	if(upLinkSP.m_bBreakXueBan90C)
	{
		pShoePlate=listShoePlates.Add(KEY4C("UYF"));
		pShoePlate->thick=pYShoePlate->thick;	//楞线侧
		pShoePlate->cStateFlag.SetFromModel();
	}
	else
		listShoePlates.DeleteNode(KEY4C("UYF"));
	if(primaryAngleDown.pAngle)
	{	//有下靴板
		if(primaryAngle.pAngle&&primaryAngle.pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
			pBaseGroupAngle=(CLDSGroupLineAngle*)primaryAngle.pAngle;
		else
			pBaseGroupAngle=NULL;
		pXShoePlate=listShoePlates.Add(KEY4C("DXB"));	//X肢翼侧
		pXShoePlate->cStateFlag.SetFromModel();
		if(pBaseGroupAngle)
			pXShoePlate->thick=ftoi(pBaseGroupAngle->jg_space-g_sysPara.m_nGroupJgMachiningGap);
		pXShoePlate->m_bTranverse=!downLinkSP.m_bBreakXueBan0C;
		RetrieveLinkAngles(pXShoePlate);
		pYShoePlate=listShoePlates.Add(KEY4C("DYB"));	//Y肢翼侧
		pYShoePlate->cStateFlag.SetFromModel();
		if(pBaseGroupAngle)
			pYShoePlate->thick=ftoi(pBaseGroupAngle->jg_space-g_sysPara.m_nGroupJgMachiningGap);
		pYShoePlate->m_bTranverse=!downLinkSP.m_bBreakXueBan90C;
		RetrieveLinkAngles(pYShoePlate);
		if(downLinkSP.m_bBreakXueBan0C)
		{
			pShoePlate=listShoePlates.Add(KEY4C("DXF"));
			pShoePlate->thick=pXShoePlate->thick;	//楞线侧
			pShoePlate->cStateFlag.SetFromModel();
		}
		else
			listShoePlates.DeleteNode(KEY4C("DXF"));
		if(downLinkSP.m_bBreakXueBan90C)
		{
			pShoePlate=listShoePlates.Add(KEY4C("DYF"));
			pShoePlate->thick=pYShoePlate->thick;	//楞线侧
			pShoePlate->cStateFlag.SetFromModel();
		}
		else
			listShoePlates.DeleteNode(KEY4C("DYF"));
	}
}
void CFootTempl::InitRibPlateParaInfo(RIBPLATE* pRibPlate)
{
	listRibPlates.GetValue(pRibPlate->Serial());
	RIBPLATE* pPreRibPlate=listRibPlates.GetPrev();
	if(pPreRibPlate)
	{
		if(strlen(pPreRibPlate->sPartNo)>0)
			strcpy(pRibPlate->sPartNo,pPreRibPlate->sPartNo);
		pRibPlate->cMaterial=pPreRibPlate->cMaterial;
		pRibPlate->thick=pPreRibPlate->thick;
		pRibPlate->A=pPreRibPlate->A;	//高度
		pRibPlate->B=pPreRibPlate->B;	//宽度
		pRibPlate->C=pPreRibPlate->C;	//台阶高度
		pRibPlate->D=pPreRibPlate->D;	//顶部宽度
		pRibPlate->iCutMode=pPreRibPlate->iCutMode;	//倒角模式
		pRibPlate->fCutAngleWidth=pPreRibPlate->fCutAngleWidth;	//切角宽度
		pRibPlate->fGradientAngle=pPreRibPlate->fGradientAngle;	//倾斜角度
		pRibPlate->axisXType=pPreRibPlate->axisXType;
		if(pRibPlate->axisXType==0)
			pRibPlate->pos_para.PARA2.angle=pPreRibPlate->pos_para.PARA2.angle;
		else if(pRibPlate->axisXType==5)
		{
			pRibPlate->pos_para.PARA3.angle=pPreRibPlate->pos_para.PARA3.angle;
			pRibPlate->pos_para.PARA3.fRayDist=pPreRibPlate->pos_para.PARA3.fRayDist;
		}
		else
		{	//定位基点
			pRibPlate->pos_para.PARA1.OX=pPreRibPlate->pos_para.PARA1.OX;
			pRibPlate->pos_para.PARA1.OY=pPreRibPlate->pos_para.PARA1.OY;
			//焊缝走向
			pRibPlate->pos_para.PARA1.axisX_x=pPreRibPlate->pos_para.PARA1.axisX_x;
			pRibPlate->pos_para.PARA1.axisX_y=pPreRibPlate->pos_para.PARA1.axisX_y;
		}
	}
	//智能判断方形塔脚板肋板参数初始值
	if(base.shape==0)
	{	//1.遍历已布肋板，初始化最常用的八个位置肋板布置状态
		//  以axisXType为Key值,key取值为(1.X+(Y+), 2.X-(Y+), 3.Y+(X+), 4.Y-(X+)
		//						       -1.X+(Y-),-2.X-(Y-),-3.Y+(X-),-4.Y-(X-))
		//添加肋板根据已经添加的肋板，判断下新加肋板轴向及偏移量
		CHashSet<RIBPLATE*> hashPlateByAixsType;
		for(RIBPLATE *pTempPlate=listRibPlates.GetFirst();pTempPlate;pTempPlate=listRibPlates.GetNext())
		{
			if(pTempPlate==pRibPlate)
				continue;	//跳过当前肋板
			if(pTempPlate->axisXType<1&&pTempPlate->axisXType>4)
				continue;
			long key=pTempPlate->axisXType;
			if(pTempPlate->axisXType==1||pTempPlate->axisXType==2)
			{
				if(pTempPlate->pos_para.PARA1.OY<0)
					key*=-1;
			}
			else if(pTempPlate->pos_para.PARA1.OX<0)
				key*=-1;
			if(hashPlateByAixsType.GetValue(key)==NULL)
				hashPlateByAixsType.SetValue(key,pTempPlate);
		}
		//2.1 选择肋板布置位置，并初始化参数
		//肋板优先顺序原则为:Y+方向靴板两块肋板，X-靴板两块肋板，X+Y+方向肋板，X-Y-方向肋板
		int plateKeyArr[8]={1,2,-3,-4,3,-2,4,-1};
		int refPlateKeyArr[8]={2,1,-4,-3,4,-1,3,-2};
		for(int i=0;i<8;i++)
		{
			if(hashPlateByAixsType.GetValue(plateKeyArr[i])==NULL)
			{	//找到未添加肋板的位置
				pRibPlate->axisXType=abs(plateKeyArr[i]);
				RIBPLATE *pTempPlate=hashPlateByAixsType.GetValue(refPlateKeyArr[i]);
				if(pTempPlate)
				{	//找到轴向相反的肋板拷贝参数
					pRibPlate->pos_para.PARA1.OX=pTempPlate->pos_para.PARA1.OX;
					pRibPlate->pos_para.PARA1.OY=pTempPlate->pos_para.PARA1.OY;
					pRibPlate->pos_para.PARA1.axisX_x=pTempPlate->pos_para.PARA1.axisX_x;
					pRibPlate->pos_para.PARA1.axisX_y=pTempPlate->pos_para.PARA1.axisX_y;
					if(pRibPlate->axisXType==1||pRibPlate->axisXType==2)
						pRibPlate->pos_para.PARA1.axisX_x*=-1;
					else if(pRibPlate->axisXType==3||pRibPlate->axisXType==4)
						pRibPlate->pos_para.PARA1.axisX_y*=-1;
				}
				else 
				{	//为找到轴向相反的肋板时根据底板宽度计算肋板初始位置
					if(plateKeyArr[i]==1||plateKeyArr[i]==2)
						pRibPlate->pos_para.PARA1.OY=base.YA-pRibPlate->thick*1.5;
					else if(plateKeyArr[i]==3||plateKeyArr[i]==4)
						pRibPlate->pos_para.PARA1.OX=base.XA-pRibPlate->thick*1.5;
					else if(plateKeyArr[i]==-1||plateKeyArr[i]==-2)
						pRibPlate->pos_para.PARA1.OY=-base.YB+pRibPlate->thick*1.5;
					else if(plateKeyArr[i]==-3||plateKeyArr[i]==-4)
						pRibPlate->pos_para.PARA1.OX=-base.XB+pRibPlate->thick*1.5;
					if(pRibPlate->axisXType==1)
					{	//X+
						pRibPlate->pos_para.PARA1.axisX_x=1;
						pRibPlate->pos_para.PARA1.axisX_y=0;
					}
					else if(pRibPlate->axisXType==2)
					{	//X-
						pRibPlate->pos_para.PARA1.axisX_x=-1;
						pRibPlate->pos_para.PARA1.axisX_y=0;
					}
					else if(pRibPlate->axisXType==3)
					{	//Y+
						pRibPlate->pos_para.PARA1.axisX_x=0;
						pRibPlate->pos_para.PARA1.axisX_y=1;
					}
					else if(pRibPlate->axisXType==4)
					{	//Y-
						pRibPlate->pos_para.PARA1.axisX_x=0;
						pRibPlate->pos_para.PARA1.axisX_y=-1;
					}
				}
				break;
			}
		}
	}
}
//底板设计
BOOL CFootTempl::DesignBasePlate()
{
	CLDSParamPlate* pParamPlate =(CLDSParamPlate*)Ta.Parts.append(CLS_PARAMPLATE);
	pParamPlate->m_iParamType=TYPE_BASEPLATE;	//塔脚底板
	pParamPlate->m_bDisplayHole = true;
	pParamPlate->cMaterial=base.cMaterial;
	pParamPlate->m_cFuncType=CLDSPart::FUNC_FOOTPLATE;
	pParamPlate->SetPartNo(base.sPartNo);
	SEGI iSeg;
	if(strlen(base.sPartNo)>0&&ParsePartNo(base.sPartNo,&iSeg,NULL))
		pParamPlate->iSeg=iSeg;
	else
		pParamPlate->iSeg=primaryAngle.pAngle->iSeg;
	pParamPlate->CopyModuleInstanceInfo(primaryAngle.pAngle);
	pParamPlate->SetDesignItemValue('A',m_hBaseNode,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE);
	if(primaryAngle.pAngle==NULL)
		return FALSE;
	pParamPlate->SetDesignItemValue('L',primaryAngle.pAngle->handle,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE);
	pParamPlate->SetDesignItemValue('N',base.nNormOffset,NULL,PARAM_POS);	//法线偏移
	pParamPlate->SetDesignItemValue('S',base.shape,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_INTEGER);		//底板外形
	if(base.shape==0)	//方形
	{
		pParamPlate->SetDesignItemValue(KEY2C("XA"),base.XA,NULL,PARAM_SHAPE);
		pParamPlate->SetDesignItemValue(KEY2C("XB"),base.XB,NULL,PARAM_SHAPE);
		pParamPlate->SetDesignItemValue(KEY2C("YA"),base.YA,NULL,PARAM_SHAPE);
		pParamPlate->SetDesignItemValue(KEY2C("YB"),base.YB,NULL,PARAM_SHAPE);
		for(int i=0;i<4;i++)
		{
			pParamPlate->SetDesignItemValue(KEY2C(CXhChar16("X%d",i)),base.XC[i],NULL,PARAM_SHAPE);
			pParamPlate->SetDesignItemValue(KEY2C(CXhChar16("Y%d",i)),base.YC[i],NULL,PARAM_SHAPE);
		}
	}
	else	//圆形
		pParamPlate->SetDesignItemValue(KEY2C("R"),base.fR,NULL,PARAM_SHAPE);	//半径
	pParamPlate->thick=base.thick;
	//第一次生成底板，根据CLDSPoint origin计算调整底板坐标
	UCS_STRU ucs;
	GetUCS(ucs,false);
	if(base.nNormOffset!=0)
	{
		ucs.origin+=base.nNormOffset*ucs.axis_z;
		if(!ucs.axis_z.IsEqual(f3dPoint(0,0,-1))&&!ucs.axis_z.IsEqual(f3dPoint(0,0,1)))
			Int3dlf(ucs.origin,origin.Position(),GEPOINT(0,0,1),ucs.origin,ucs.axis_z);	//斜塔脚
	}
	pParamPlate->design.origin=origin;	
	for(LIST_NODE<BASEPLATE::HOLE> *pListNode=base.listHoles.EnumFirst();pListNode;pListNode=base.listHoles.EnumNext())
	{
		BASEPLATE::HOLE hole=pListNode->data;
		CLDSBolt *pRealLs;
		pRealLs = (CLDSBolt*)Ta.Parts.append(CLS_BOLT);
		pRealLs->m_bVirtualPart=TRUE;
		pRealLs->m_cFuncType=CLDSBolt::FUNC_SETUPHOLE;	//装配孔
		pRealLs->set_norm(pParamPlate->ucs.axis_z);
		pRealLs->des_work_norm.norm_style=3;
		pRealLs->des_work_norm.hVicePart=pParamPlate->handle;
		pRealLs->des_work_norm.direction=0;
		pRealLs->set_d(hole.d);
		pRealLs->hole_d_increment = hole.hole_d-hole.d;
		pRealLs->AddL0Thick(base.thick,FALSE);
		f3dPoint dijiaols_pos(hole.x,hole.y);
		coord_trans(dijiaols_pos,ucs,TRUE);
		pRealLs->ucs.origin=dijiaols_pos;
		pRealLs->cfgword=pParamPlate->cfgword;	//调整螺栓配材号与基准构件配材号一致
		//填充螺栓位置设计参数
		pRealLs->des_base_pos.cLocationStyle=DESIGN_LS_POS::LOCAL_COORD;
		pRealLs->des_base_pos.wing_offset_dist=hole.y;
		pRealLs->des_base_pos.len_offset_dist=hole.x;
		pRealLs->des_base_pos.datumPoint.datum_pos_style=9;
		pRealLs->des_base_pos.datumPoint.des_para.hPart=pParamPlate->handle;
		pRealLs->des_base_pos.datumPoint.datum_pos.Set(hole.x,hole.y,0);
		pRealLs->des_base_pos.hPart=pParamPlate->handle;
		pParamPlate->AppendLsRef(pRealLs->GetLsRef());
	}
	base.pPlate = pParamPlate;
	pParamPlate->ucs.axis_z=ucs.axis_z;
	pParamPlate->DesignPlate();
	tempory.ucs=pParamPlate->ucs;
	return TRUE;
}
// 布置基准角钢上的螺栓
void CFootTempl::LayBaseJgLs(bool upper)
{
	int iLsLayStyle=primaryAngle.bolt.layout;
	CLDSLineAngle * pBaseJg=primaryAngle.pAngle;
	int nXueBanThick=tempory.pXUpBackPlank->thick;
	CLDSParamPlate* pXFrontPlate=tempory.pXUpFrontPlank;
	CLDSParamPlate* pXBackPlate =tempory.pXUpBackPlank;
	CLDSParamPlate* pYFrontPlate=tempory.pYUpFrontPlank;
	CLDSParamPlate* pYBackPlate =tempory.pYUpBackPlank;
	if(!upper&&tempory.pXDownBackPlank==NULL)
	{
		logerr.Log("tempory.pXDownBackPlank缺失");
		return;
	}
	if(!upper)
	{
		iLsLayStyle=1-primaryAngleDown.bolt.layout;
		pBaseJg=primaryAngleDown.pAngle;
		nXueBanThick=tempory.pXDownBackPlank->thick;
		pXFrontPlate=tempory.pXDownFrontPlank;
		pXBackPlate =tempory.pXDownBackPlank;
		pYFrontPlate=tempory.pYDownFrontPlank;
		pYBackPlate =tempory.pYDownBackPlank;
	}
	int mode=1;
	if( pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE&&((CLDSGroupLineAngle*)pBaseJg)->group_style==0)
	{
		f3dPoint wing_vec_x=pBaseJg->GetWingVecX();
		f3dPoint wing_vec_y=pBaseJg->GetWingVecY();
		double dd_wingx_x=wing_vec_x*tempory.ucs.axis_x;
		double dd_wingx_y=wing_vec_x*tempory.ucs.axis_y;
		double dd_wingy_x=wing_vec_y*tempory.ucs.axis_x;
		double dd_wingy_y=wing_vec_y*tempory.ucs.axis_y;
		if(fabs(dd_wingx_x)>fabs(dd_wingx_y)&&dd_wingx_x>0 && fabs(dd_wingy_y)>fabs(dd_wingy_x)&&dd_wingy_y>0)
			mode=0;	//背靠背组合角钢的布置型式为异型(1号角钢在外侧)
		else if(fabs(dd_wingx_y)>fabs(dd_wingx_x)&&dd_wingx_y>0 && fabs(dd_wingy_x)>fabs(dd_wingy_y)&&dd_wingy_x>0)
			mode=0;	//背靠背组合角钢的布置型式为异型(1号角钢在外侧)
		else if(fabs(dd_wingx_x)>fabs(dd_wingx_y)&&dd_wingx_x<0 && fabs(dd_wingy_y)>fabs(dd_wingy_x)&&dd_wingy_y<0)
			mode=0;	//背靠背组合角钢的布置型式为异型(1号角钢在外侧)
		else if(fabs(dd_wingx_y)>fabs(dd_wingx_x)&&dd_wingx_y<0 && fabs(dd_wingy_x)>fabs(dd_wingy_y)&&dd_wingy_x<0)
			mode=0;	//背靠背组合角钢的布置型式为异型(1号角钢在外侧)
	}
	//
	CConnectInfo ls_group;
	LSSPACE_STRU LsSpace;
	CLDSPoint datumPointX,datumPointY;
	if(pBaseJg->pStart->handle==m_hBaseNode)
	{
		if(upper)
		{
			pBaseJg->connectStart.d=primaryAngle.bolt.d;
			pBaseJg->connectStart.rows=primaryAngle.bolt.rows;
			pBaseJg->SetBoltsNumOn1Wing(primaryAngle.bolt.num,TRUE);
		}
		else
		{
			pBaseJg->connectStart.d=primaryAngleDown.bolt.d;
			pBaseJg->connectStart.rows=primaryAngleDown.bolt.rows;
			pBaseJg->SetBoltsNumOn1Wing(primaryAngleDown.bolt.num,TRUE);
		}
		ls_group=pBaseJg->connectStart;//获得基准材螺栓间距信息
	}
	else
	{
		if(upper)
		{
			pBaseJg->connectEnd.d=primaryAngle.bolt.d;
			pBaseJg->connectEnd.rows=primaryAngle.bolt.rows;
			pBaseJg->SetBoltsNumOn1Wing(primaryAngle.bolt.num,FALSE);
		}
		else
		{
			pBaseJg->connectEnd.d=primaryAngleDown.bolt.d;
			pBaseJg->connectEnd.rows=primaryAngleDown.bolt.rows;
			pBaseJg->SetBoltsNumOn1Wing(primaryAngleDown.bolt.num,FALSE);
		}
		ls_group=pBaseJg->connectEnd;//获得基准材螺栓间距信息
	}
	if(upper)
		ls_group.wnConnBoltN=primaryAngle.bolt.num;
	else
		ls_group.wnConnBoltN=primaryAngleDown.bolt.num;
	//设置布孔准距及螺栓端距,间距 wxc 15-11-05
	JGZJ jgzj_x,jgzj_y,defaultZJ;
	if(upper)
	{
		LsSpace.EndSpace=primaryAngle.bolt.siEndSpace;
		LsSpace.SingleRowSpace=primaryAngle.bolt.siBoltSpace;
		LsSpace.doubleRowSpace=primaryAngle.bolt.siBoltSpace;
		jgzj_x.g=primaryAngle.g;
		jgzj_x.g1=primaryAngle.g1;
		jgzj_x.g2=primaryAngle.g2;
	}
	else
	{
		LsSpace.EndSpace=primaryAngleDown.bolt.siEndSpace;
		LsSpace.SingleRowSpace=primaryAngleDown.bolt.siBoltSpace;
		LsSpace.doubleRowSpace=primaryAngleDown.bolt.siBoltSpace;
		jgzj_x.g=primaryAngleDown.g;
		jgzj_x.g1=primaryAngleDown.g1;
		jgzj_x.g2=primaryAngleDown.g2;
	}
	jgzj_y=jgzj_x;
	//布置主材螺栓
	if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)	//组合角钢
	{	//布置始端主材上的螺栓
		CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pBaseJg;
		pGroupJg->UpdateSonJgBasicInfo();
		if(pGroupJg->group_style==0)	//对角组合型牛蹄板基准材
		{
			CLDSLineAngle *pJgArr[2];
			pJgArr[0]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
			pJgArr[1]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
			for(int ii=0;ii<2;ii++)
			{
				CLDSLineAngle *pSonBaseJg=pJgArr[ii];
				f3dPoint wing_vec_x=pSonBaseJg->GetWingVecX();
				f3dPoint wing_vec_y=pSonBaseJg->GetWingVecY();
				//根据用户输入的准距进行角钢上螺栓的布置
				CLDSBolt ls_x(console.GetActiveModel()),ls_y(console.GetActiveModel());
				datumPointX.des_para.RODEND.offset_wing=0;
				datumPointY.des_para.RODEND.offset_wing=1;
				datumPointX.datum_pos_style					=datumPointY.datum_pos_style=1;
				datumPointX.des_para.RODEND.hRod			=datumPointY.des_para.RODEND.hRod=pSonBaseJg->handle;
				if(pBaseJg->pStart->handle==m_hBaseNode)
					datumPointX.des_para.RODEND.direction	=datumPointY.des_para.RODEND.direction=0;
				else
					datumPointX.des_para.RODEND.direction	=datumPointY.des_para.RODEND.direction=1;
				datumPointX.des_para.RODEND.len_offset_dist	=datumPointY.des_para.RODEND.len_offset_dist=0;
				datumPointX.des_para.RODEND.bIncOddEffect	=datumPointY.des_para.RODEND.bIncOddEffect=TRUE;
				datumPointX.des_para.RODEND.wing_offset_dist=datumPointY.des_para.RODEND.wing_offset_dist=0;
				datumPointX.des_para.RODEND.wing_offset_style=datumPointY.des_para.RODEND.wing_offset_style=4;
				ls_x.set_d(ls_group.d);
				ls_y.set_d(ls_group.d);
				if(mode)
				{
					if(ii==1&&pXFrontPlate)
						ls_x.AddL0Thick(pXFrontPlate->handle,TRUE);
					else
						ls_x.AddL0Thick(pXBackPlate->handle,TRUE);
					ls_x.AddL0Thick(pSonBaseJg->handle,TRUE);
					if(ii==1&&pYFrontPlate)
						ls_y.AddL0Thick(pYFrontPlate->handle,TRUE);
					else
						ls_y.AddL0Thick(pYBackPlate->handle,TRUE);
					ls_y.AddL0Thick(pSonBaseJg->handle,TRUE);
					if(!ls_x.CalGuigeAuto())
					{
						char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
						_snprintf(sError,199,"Bolt size M%dX%d not found!",ls_group.d,ls_x.get_L0());
#else
						_snprintf(sError,199,"找不到M%dX%d规格的螺栓!",ls_group.d,ls_x.get_L0());
#endif
						throw sError;
					}
					ls_x.iSeg=pSonBaseJg->iSeg;	//主材螺栓段号 wht 10-09-15
					ls_y.iSeg=pSonBaseJg->iSeg;
					ls_y.CalGuigeAuto();
					//ls_y.CopyProperty(&ls_x);
					ls_x.set_norm(pSonBaseJg->get_norm_x_wing());
					ls_y.set_norm(pSonBaseJg->get_norm_y_wing());

					f3dPoint ls_pos_x,ls_pos_y;
					int offset = LsSpace.EndSpace;
					CConnectInfo* pBoltConn=&pSonBaseJg->connectStart;
					if(pSonBaseJg->pEnd->handle==m_hBaseNode)
						pBoltConn=&pSonBaseJg->connectEnd;
					for(int j=0;j<ls_group.wnConnBoltN;j++)
					{
						if(pBoltConn->rows==1)
						{		//单排螺栓
							//ls_x.dwRayNo = 1;
							{	//单排螺栓螺栓正常排列
								if(iLsLayStyle==0)	//左高右底
								{
									ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,wing_vec_x,
										pSonBaseJg->Start(),offset+LsSpace.SingleRowSpace/2,jgzj_x.g);
									ls_x.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
									ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
								}
								else
								{
									ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
									ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,wing_vec_y,
										pSonBaseJg->Start(),offset+LsSpace.SingleRowSpace/2,jgzj_y.g);
									ls_y.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
								}
							}
							offset += LsSpace.SingleRowSpace;
						}
						else	//双排螺栓
						{
							if(iLsLayStyle==0)
							{	//靠近楞线一侧螺栓优先排列
								if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(2);
									ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g1);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
									ls_y.dwRayNo = GetSingleWord(3);
									ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g2);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
								}
								else		//奇数号螺栓位于背离楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(3);
									ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g2);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
									ls_y.dwRayNo = GetSingleWord(2);
									ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g1);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
								}
							}
							else
							{	//背离楞线一侧螺栓优先排列
								if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(3);
									ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g2);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
									ls_y.dwRayNo = GetSingleWord(2);
									ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g1);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
								}
								else		//奇数号螺栓位于背离楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(3);
									ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g1);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
									ls_y.dwRayNo = GetSingleWord(2);
									ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g2);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
								}
							}
							offset+=LsSpace.doubleRowSpace/2;
						}
						//布置X肢螺栓
						defaultZJ=pSonBaseJg->GetZhunJu('X');
						CLDSBolt *pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						ls_pos_x = ls_pos_x-pSonBaseJg->get_norm_x_wing()*pBaseJg->GetThick();
						ls_x.des_base_pos.norm_offset.key_str.Printf("%.0f",-pBaseJg->GetThick());
						ls_x.ucs.origin=ls_pos_x;
						pRealLs->CopyProperty(&ls_x);
						pRealLs->CopyModuleInstanceInfo(pSonBaseJg);	//调整螺栓配材号与基准构件配材号一致
						pRealLs->des_base_pos.datumPoint=datumPointX;
						pRealLs->des_base_pos.offset_wing=0;
						if(pBaseJg->pStart->handle==m_hBaseNode)
							pRealLs->des_base_pos.direction=0;
						else
							pRealLs->des_base_pos.direction=1;
						pRealLs->des_base_pos.hPart=pRealLs->des_base_pos.datumPoint.des_para.RODEND.hRod=pSonBaseJg->handle;
						pRealLs->des_base_pos.len_offset_dist=ls_x.des_base_pos.len_offset_dist;
						pRealLs->des_base_pos.wing_offset_dist = ls_x.des_base_pos.wing_offset_dist;
						if(pBoltConn->rows==1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g)
							pRealLs->des_base_pos.m_biWingOffsetStyle=0;
						else if(pBoltConn->rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g1)
							pRealLs->des_base_pos.m_biWingOffsetStyle=1;
						else if(pBoltConn->rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g2)
							pRealLs->des_base_pos.m_biWingOffsetStyle=2;
						else if(pBoltConn->rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g3)
							pRealLs->des_base_pos.m_biWingOffsetStyle=3;
						pRealLs->des_work_norm.norm_style=1;	//指定角钢肢法线
						pRealLs->des_work_norm.direction=0;
						pRealLs->des_work_norm.hVicePart = pSonBaseJg->handle;
						pRealLs->des_work_norm.norm_wing=0;
						pRealLs->correct_pos();
						pSonBaseJg->AppendStartLsRef(pRealLs->GetLsRef());

						if(fabs(pSonBaseJg->get_norm_x_wing()*base.pPlate->ucs.axis_y)>
							fabs(pSonBaseJg->get_norm_y_wing()*base.pPlate->ucs.axis_y))
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢为组合角钢特征角钢
								if(upper)
									tempory.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tempory.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
							else
							{	//当前角钢非组合角钢特征角钢
								if(upper)
								{
									if(tempory.pXUpFrontPlank)
										tempory.pXUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tempory.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tempory.pXDownFrontPlank)
										tempory.pXDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tempory.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
						}
						else
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢为组合角钢特征角钢
								if(upper)
									tempory.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tempory.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
							else
							{	//当前角钢非组合角钢特征角钢
								if(upper)
								{
									if(tempory.pYUpFrontPlank)
										tempory.pYUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tempory.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tempory.pYDownFrontPlank)
										tempory.pYDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tempory.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
						}
						//布置Y肢螺栓
						defaultZJ=pSonBaseJg->GetZhunJu('Y');
						pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						ls_pos_y = ls_pos_y-pSonBaseJg->get_norm_y_wing()*pBaseJg->GetThick();
						_snprintf(ls_y.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-pBaseJg->GetThick());
						ls_y.ucs.origin=ls_pos_y;
						pRealLs->CopyProperty(&ls_y);
						pRealLs->CopyModuleInstanceInfo(pSonBaseJg);	//调整螺栓配材号与基准构件配材号一致
						pRealLs->des_base_pos.datumPoint=datumPointY;
						pRealLs->des_base_pos.hPart=pSonBaseJg->handle;
						pRealLs->des_base_pos.offset_wing=1;;
						if(pBaseJg->pStart->handle==m_hBaseNode)
							pRealLs->des_base_pos.direction=0;
						else
							pRealLs->des_base_pos.direction=1;
						pRealLs->des_base_pos.len_offset_dist=ls_y.des_base_pos.len_offset_dist;
						pRealLs->des_base_pos.wing_offset_dist = ls_y.des_base_pos.wing_offset_dist;
						if(pBoltConn->rows==1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g)
							pRealLs->des_base_pos.m_biWingOffsetStyle=0;
						else if(pBoltConn->rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g1)
							pRealLs->des_base_pos.m_biWingOffsetStyle=1;
						else if(pBoltConn->rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g2)
							pRealLs->des_base_pos.m_biWingOffsetStyle=2;
						else if(pBoltConn->rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g3)
							pRealLs->des_base_pos.m_biWingOffsetStyle=3;
						pRealLs->des_work_norm.norm_style=1;
						pRealLs->des_work_norm.direction=0;
						pRealLs->des_work_norm.hVicePart = pSonBaseJg->handle;
						pRealLs->des_work_norm.norm_wing=1;
						pRealLs->correct_pos();
						pSonBaseJg->AppendStartLsRef(pRealLs->GetLsRef());

						if(fabs(pSonBaseJg->get_norm_x_wing()*base.pPlate->ucs.axis_y)>
							fabs(pSonBaseJg->get_norm_y_wing()*base.pPlate->ucs.axis_y))
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
								pYBackPlate->AppendLsRef(pRealLs->GetLsRef());	//当前角钢为组合角钢特征角钢
							else
							{	//当前角钢非组合角钢特征角钢
								if(pYFrontPlate)
									pYFrontPlate->AppendLsRef(pRealLs->GetLsRef());
								else
									pYBackPlate->AppendLsRef(pRealLs->GetLsRef());
							}
						}
						else
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
								pXBackPlate->AppendLsRef(pRealLs->GetLsRef());	//当前角钢为组合角钢特征角钢
							else
							{	//当前角钢非组合角钢特征角钢
								if(pXFrontPlate)
									pXFrontPlate->AppendLsRef(pRealLs->GetLsRef());
								else
									pXBackPlate->AppendLsRef(pRealLs->GetLsRef());
							}
						}
					}
				}
				else //if(mode==0)
				{
					double dd_wingx_x=wing_vec_x*tempory.ucs.axis_x;
					double dd_wingy_x=wing_vec_y*tempory.ucs.axis_x;
					double dd_wingx_y=wing_vec_x*tempory.ucs.axis_y;
					double dd_wingy_y=wing_vec_y*tempory.ucs.axis_y;
					if(dd_wingy_x>0&&pXFrontPlate)
						ls_y.AddL0Thick(pXFrontPlate->handle,TRUE);
					else
						ls_y.AddL0Thick(pXBackPlate->handle,TRUE);
					ls_y.AddL0Thick(pSonBaseJg->handle,TRUE);
					if(dd_wingx_y<0&&pYFrontPlate)
						ls_x.AddL0Thick(pYFrontPlate->handle,TRUE);
					else
						ls_x.AddL0Thick(pYBackPlate->handle,TRUE);
					ls_x.AddL0Thick(pSonBaseJg->handle,TRUE);
					if(!ls_x.CalGuigeAuto())
					{
						char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
						_snprintf(sError,199,"Bolt size M%dX%d not found!",ls_group.d,ls_x.get_L0());
#else
						_snprintf(sError,199,"找不到M%dX%d规格的螺栓!",ls_group.d,ls_x.get_L0());
#endif
						throw sError;
					}
					ls_x.iSeg=pSonBaseJg->iSeg;	//主材螺栓段号 wht 10-09-15
					ls_y.iSeg=pSonBaseJg->iSeg;
					ls_y.CalGuigeAuto();
					//ls_y.CopyProperty(&ls_x);
					ls_x.set_norm(pSonBaseJg->get_norm_x_wing());
					ls_y.set_norm(pSonBaseJg->get_norm_y_wing());

					f3dPoint ls_pos_x,ls_pos_y;
					int offset = LsSpace.EndSpace;
					CConnectInfo* pBoltConn=&pSonBaseJg->connectStart;
					if(pSonBaseJg->pEnd->handle==m_hBaseNode)
						pBoltConn=&pSonBaseJg->connectEnd;
					for(int j=0;j<ls_group.wnConnBoltN;j++)
					{
						if(pBoltConn->rows==1)
						{		//单排螺栓
							//ls_x.dwRayNo = 1;
							{	//单排螺栓螺栓正常排列
								if(iLsLayStyle==0)	//左高右底
								{
									ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,wing_vec_x,
										pSonBaseJg->Start(),offset+LsSpace.SingleRowSpace/2,jgzj_x.g);
									ls_x.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
									ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
								}
								else
								{
									ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
									ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,wing_vec_y,
										pSonBaseJg->Start(),offset+LsSpace.SingleRowSpace/2,jgzj_y.g);
									ls_y.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
								}
							}
							offset += LsSpace.SingleRowSpace;
						}
						else	//双排螺栓
						{
							if(iLsLayStyle==0)
							{	//靠近楞线一侧螺栓优先排列
								if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(2);
									ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g1);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
									ls_y.dwRayNo = GetSingleWord(3);
									ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g2);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
								}
								else		//奇数号螺栓位于背离楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(3);
									ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g2);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
									ls_y.dwRayNo = GetSingleWord(2);
									ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g1);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
								}
							}
							else
							{	//背离楞线一侧螺栓优先排列
								if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(3);
									ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g2);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
									ls_y.dwRayNo = GetSingleWord(2);
									ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g1);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
								}
								else		//奇数号螺栓位于背离楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(3);
									ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g1);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
									ls_y.dwRayNo = GetSingleWord(2);
									ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g2);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
								}
							}
							offset+=LsSpace.doubleRowSpace/2;
						}
						//布置X肢螺栓
						defaultZJ=pSonBaseJg->GetZhunJu('X');
						CLDSBolt *pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						ls_pos_x = ls_pos_x-pSonBaseJg->get_norm_x_wing()*pBaseJg->GetThick();
						ls_x.des_base_pos.norm_offset.key_str.Printf("%g",-pBaseJg->GetThick());
						ls_x.ucs.origin=ls_pos_x;
						pRealLs->CopyProperty(&ls_x);
						pRealLs->CopyModuleInstanceInfo(pSonBaseJg);	//调整螺栓配材号与基准构件配材号一致
						pRealLs->des_base_pos.datumPoint=datumPointX;
						pRealLs->des_base_pos.offset_wing=0;
						if(pBaseJg->pStart->handle==m_hBaseNode)
							pRealLs->des_base_pos.direction=0;
						else
							pRealLs->des_base_pos.direction=1;
						pRealLs->des_base_pos.hPart=pRealLs->des_base_pos.datumPoint.des_para.RODEND.hRod=pSonBaseJg->handle;
						pRealLs->des_base_pos.len_offset_dist=ls_x.des_base_pos.len_offset_dist;
						pRealLs->des_base_pos.wing_offset_dist = ls_x.des_base_pos.wing_offset_dist;
						if(pBoltConn->rows==1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g)
							pRealLs->des_base_pos.m_biWingOffsetStyle=0;
						else if(pBoltConn->rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g1)
							pRealLs->des_base_pos.m_biWingOffsetStyle=1;
						else if(pBoltConn->rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g2)
							pRealLs->des_base_pos.m_biWingOffsetStyle=2;
						else if(pBoltConn->rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g3)
							pRealLs->des_base_pos.m_biWingOffsetStyle=3;
						pRealLs->des_work_norm.norm_style=1;	//指定角钢肢法线
						pRealLs->des_work_norm.direction=0;
						pRealLs->des_work_norm.hVicePart = pSonBaseJg->handle;
						pRealLs->des_work_norm.norm_wing=0;
						pRealLs->correct_pos();
						pSonBaseJg->AppendStartLsRef(pRealLs->GetLsRef());

						if(fabs(dd_wingx_x)>fabs(dd_wingx_y))
						{
							if(dd_wingx_x>0&&pXFrontPlate)//if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前子角钢X肢向与底X轴正负一致，且方向相同(即当前子角钢在1象限)
								if(upper)
								{
									if(tempory.pXUpFrontPlank)
										tempory.pXUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tempory.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tempory.pXDownFrontPlank)
										tempory.pXDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tempory.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
							else
							{	//当前子角钢X肢向与底X轴正负一致，担方向相反(即当前子角钢在3象限)
								if(upper)
									tempory.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tempory.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
						}
						else
						{
							if(dd_wingx_y<0&&pYFrontPlate)//if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢非组合角钢特征角钢
								if(upper)
								{
									if(tempory.pYUpFrontPlank)
										tempory.pYUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tempory.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tempory.pYDownFrontPlank)
										tempory.pYDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tempory.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
							else
							{	//当前角钢为组合角钢特征角钢
								if(upper)
									tempory.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tempory.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
						}
						//布置Y肢螺栓
						defaultZJ=pSonBaseJg->GetZhunJu('Y');
						pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						ls_pos_y = ls_pos_y-pSonBaseJg->get_norm_y_wing()*pBaseJg->GetThick();
						_snprintf(ls_y.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-pBaseJg->GetThick());
						ls_y.ucs.origin=ls_pos_y;
						pRealLs->CopyProperty(&ls_y);
						pRealLs->CopyModuleInstanceInfo(pSonBaseJg);	//调整螺栓配材号与基准构件配材号一致
						pRealLs->des_base_pos.datumPoint=datumPointY;
						pRealLs->des_base_pos.hPart=pSonBaseJg->handle;
						pRealLs->des_base_pos.offset_wing=1;
						if(pBaseJg->pStart->handle==m_hBaseNode)
							pRealLs->des_base_pos.direction=0;
						else
							pRealLs->des_base_pos.direction=1;
						pRealLs->des_base_pos.len_offset_dist=ls_y.des_base_pos.len_offset_dist;
						pRealLs->des_base_pos.wing_offset_dist = ls_y.des_base_pos.wing_offset_dist;
						if(pBoltConn->rows==1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g)
							pRealLs->des_base_pos.m_biWingOffsetStyle=0;
						else if(pBoltConn->rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g1)
							pRealLs->des_base_pos.m_biWingOffsetStyle=1;
						else if(pBoltConn->rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g2)
							pRealLs->des_base_pos.m_biWingOffsetStyle=2;
						else if(pBoltConn->rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g3)
							pRealLs->des_base_pos.m_biWingOffsetStyle=3;
						pRealLs->des_work_norm.norm_style=1;
						pRealLs->des_work_norm.direction=0;
						pRealLs->des_work_norm.hVicePart = pSonBaseJg->handle;
						pRealLs->des_work_norm.norm_wing=1;
						pRealLs->correct_pos();
						pSonBaseJg->AppendStartLsRef(pRealLs->GetLsRef());

						if(fabs(dd_wingy_y)>fabs(dd_wingy_x))
						{	//当前子角钢Y肢向与底板Y轴方向一致
							if(dd_wingy_y<0&&pYFrontPlate)//if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前子角钢Y肢向与底板Y轴方向反向(即当前子角钢在3象限)
								if(upper)
								{
									if(tempory.pYUpFrontPlank)
										tempory.pYUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tempory.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tempory.pYDownFrontPlank)
										tempory.pYDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tempory.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
							else
							{
								if(upper)
									tempory.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tempory.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
						}
						else
						{	//当前子角钢Y肢向与底板X轴方向一致
							if(dd_wingy_x>0&&pXFrontPlate)
							{	//当前子角钢Y肢向与底板X轴正向同向(即当前子角钢在1象限)
								if(upper)
								{
									if(tempory.pXUpFrontPlank)
										tempory.pXUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tempory.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tempory.pXDownFrontPlank)
										tempory.pXDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tempory.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
							else
							{	//当前子角钢Y肢向与底板X轴正向反向(即当前子角钢在3象限)
								if(upper)
									tempory.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());	//当前角钢为组合角钢特征角钢
								else
									tempory.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());	//当前角钢为组合角钢特征角钢
							}
						}
					}
				}
			}
		}
		else if(pGroupJg->group_style==1)	//T型组合角钢
		{
#ifdef AFX_TARG_ENU_ENGLISH
			throw "This type of hoof plates not supported now!";
#else
			throw "系统暂时不支持这种形式的底牛蹄板!";
#endif
		}
		else	//十字交叉型牛蹄板基准材
		{
			CLDSLineAngle *pJgArr[4];
			pJgArr[0]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
			pJgArr[1]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
			pJgArr[2]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[2],CLS_LINEANGLE);
			pJgArr[3]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[3],CLS_LINEANGLE);
			for(int ii=0;ii<3;ii+=2)
			{
				CLDSLineAngle *pSonBaseJg=pJgArr[ii];
				f3dPoint wing_vec_x=pSonBaseJg->GetWingVecX();
				f3dPoint wing_vec_y=pSonBaseJg->GetWingVecY();
				//根据用户输入的准距进行角钢上螺栓的布置
				/*if(!getjgzj(jgzj_x,pBaseJg->GetWidth()))
				{
					char sError[200];
#ifdef AFX_TARG_ENU_ENGLISH
					_snprintf(sError,199,"Bolt G data of Angle L%d not found!",pBaseJg->GetWidth());
#else
					_snprintf(sError,199,"找不到肢宽%d的角钢准距数据!",pBaseJg->GetWidth());
#endif
					throw sError;
				}
				jgzj_y=jgzj_x;
				if(pSonBaseJg->m_bEnableTeG)
				{
					jgzj_x = pSonBaseJg->xWingXZhunJu;
					jgzj_y = pSonBaseJg->xWingYZhunJu;
				}*/
				CLDSBolt ls_x(console.GetActiveModel()),ls_y(console.GetActiveModel());
				datumPointX.des_para.RODEND.offset_wing=0;
				datumPointY.des_para.RODEND.offset_wing=1;
				datumPointX.datum_pos_style					=datumPointY.datum_pos_style=1;
				datumPointX.des_para.RODEND.hRod			=datumPointY.des_para.RODEND.hRod=pSonBaseJg->handle;
				if(pBaseJg->pStart->handle==m_hBaseNode)
					datumPointX.des_para.RODEND.direction		=datumPointY.des_para.RODEND.direction=0;
				else
					datumPointX.des_para.RODEND.direction		=datumPointY.des_para.RODEND.direction=1;
				datumPointX.des_para.RODEND.len_offset_dist	=datumPointY.des_para.RODEND.len_offset_dist=0;
				datumPointX.des_para.RODEND.bIncOddEffect	=datumPointY.des_para.RODEND.bIncOddEffect=TRUE;
				datumPointX.des_para.RODEND.wing_offset_dist=datumPointY.des_para.RODEND.wing_offset_dist=0;
				datumPointX.des_para.RODEND.wing_offset_style=datumPointY.des_para.RODEND.wing_offset_style=4;
				ls_x.set_d(ls_group.d);
				ls_y.set_d(ls_group.d);
				if(ii==1&&pXFrontPlate)
					ls_x.AddL0Thick(pXFrontPlate->handle,TRUE);
				else
					ls_x.AddL0Thick(pXBackPlate->handle,TRUE);
				ls_x.AddL0Thick(pSonBaseJg->handle,TRUE);
				ls_x.AddL0Thick(pSonBaseJg->handle,TRUE);
				if(ii==1&&pYFrontPlate)
					ls_y.AddL0Thick(pYFrontPlate->handle,TRUE);
				else
					ls_y.AddL0Thick(pYBackPlate->handle,TRUE);
				ls_y.AddL0Thick(pSonBaseJg->handle,TRUE);
				ls_y.AddL0Thick(pSonBaseJg->handle,TRUE);
				if(!ls_x.CalGuigeAuto())
				{
					char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
					_snprintf(sError,199,"Bolt size M%dX%d not found!",ls_group.d,ls_x.get_L0());
#else
					_snprintf(sError,199,"找不到M%dX%d规格的螺栓!",ls_group.d,ls_x.get_L0());
#endif
					throw sError;
				}
				ls_x.iSeg=pSonBaseJg->iSeg;	//主材螺栓段号 wht 10-09-15
				ls_y.CalGuigeAuto();
				ls_x.set_norm(pSonBaseJg->get_norm_x_wing());
				ls_y.set_norm(pSonBaseJg->get_norm_y_wing());

				f3dPoint ls_pos_x,ls_pos_y;
				int offset = LsSpace.EndSpace;
				CConnectInfo* pBoltConn=&pSonBaseJg->connectStart;
				if(pSonBaseJg->pEnd->handle==m_hBaseNode)
					pBoltConn=&pSonBaseJg->connectEnd;
				for(int j=0;j<ls_group.wnConnBoltN;j++)
				{
					if(pBoltConn->rows==1)
					{		//单排螺栓
						//ls_x.dwRayNo = 1;
						{	//单排螺栓螺栓正常排列
							if(iLsLayStyle==0)	//左高右底
							{
								ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,wing_vec_x,
									pSonBaseJg->Start(),offset+LsSpace.SingleRowSpace/2,jgzj_x.g);
								ls_x.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
								ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
								ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,wing_vec_y,
									pSonBaseJg->Start(),offset,jgzj_y.g);
								ls_y.des_base_pos.len_offset_dist = offset;
								ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
							}
							else
							{
								ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,wing_vec_x,
									pSonBaseJg->Start(),offset,jgzj_x.g);
								ls_x.des_base_pos.len_offset_dist = offset;
								ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
								ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,wing_vec_y,
									pSonBaseJg->Start(),offset+LsSpace.SingleRowSpace/2,jgzj_y.g);
								ls_y.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
								ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
							}
						}
						offset += LsSpace.SingleRowSpace;
					}
					else	//双排螺栓
					{
						if(iLsLayStyle==0)
						{	//靠近楞线一侧螺栓优先排列
							if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
							{
								ls_x.dwRayNo = GetSingleWord(2);
								ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,wing_vec_x,
									pSonBaseJg->Start(),offset,jgzj_x.g1);
								ls_x.des_base_pos.len_offset_dist = offset;
								ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
								ls_y.dwRayNo = GetSingleWord(3);
								ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,wing_vec_y,
									pSonBaseJg->Start(),offset,jgzj_y.g2);
								ls_y.des_base_pos.len_offset_dist = offset;
								ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
							}
							else		//奇数号螺栓位于背离楞线一侧	
							{
								ls_x.dwRayNo = GetSingleWord(3);
								ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,wing_vec_x,
									pSonBaseJg->Start(),offset,jgzj_x.g2);
								ls_x.des_base_pos.len_offset_dist = offset;
								ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
								ls_y.dwRayNo = GetSingleWord(2);
								ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,wing_vec_y,
									pSonBaseJg->Start(),offset,jgzj_y.g1);
								ls_y.des_base_pos.len_offset_dist = offset;
								ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
							}
						}
						else
						{	//背离楞线一侧螺栓优先排列
							if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
							{
								ls_x.dwRayNo = GetSingleWord(3);
								ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,wing_vec_x,
									pSonBaseJg->Start(),offset,jgzj_x.g2);
								ls_x.des_base_pos.len_offset_dist = offset;
								ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
								ls_y.dwRayNo = GetSingleWord(2);
								ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,wing_vec_y,
									pSonBaseJg->Start(),offset,jgzj_y.g1);
								ls_y.des_base_pos.len_offset_dist = offset;
								ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
							}
							else		//奇数号螺栓位于背离楞线一侧	
							{
								ls_x.dwRayNo = GetSingleWord(2);
								ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,wing_vec_x,
									pSonBaseJg->Start(),offset,jgzj_x.g1);
								ls_x.des_base_pos.len_offset_dist = offset;
								ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
								ls_y.dwRayNo = GetSingleWord(3);
								ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,wing_vec_y,
									pSonBaseJg->Start(),offset,jgzj_y.g2);
								ls_y.des_base_pos.len_offset_dist = offset;
								ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
							}
						}
						offset+=LsSpace.doubleRowSpace/2;
					}
					//布置X肢螺栓
					defaultZJ=pSonBaseJg->GetZhunJu('X');
					CLDSBolt *pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					ls_pos_x = ls_pos_x-pSonBaseJg->get_norm_x_wing()*pBaseJg->GetThick();
					ls_x.des_base_pos.norm_offset.key_str.Printf("%.0f",-pBaseJg->GetThick());
					ls_x.ucs.origin=ls_pos_x;
					pRealLs->CopyProperty(&ls_x);
					pRealLs->CopyModuleInstanceInfo(pSonBaseJg);	//调整螺栓配材号与基准构件配材号一致
					pRealLs->des_base_pos.datumPoint=datumPointX;
					pRealLs->des_base_pos.hPart=pSonBaseJg->handle;
					pRealLs->des_base_pos.offset_wing=0;
					if(pBaseJg->pStart->handle==m_hBaseNode)
						pRealLs->des_base_pos.direction=0;
					else
						pRealLs->des_base_pos.direction=1;
					pRealLs->des_base_pos.len_offset_dist=ls_x.des_base_pos.len_offset_dist;
					pRealLs->des_base_pos.wing_offset_dist = ls_x.des_base_pos.wing_offset_dist;
					if(pBoltConn->rows==1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g)
						pRealLs->des_base_pos.m_biWingOffsetStyle=0;
					else if(pBoltConn->rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g1)
						pRealLs->des_base_pos.m_biWingOffsetStyle=1;
					else if(pBoltConn->rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g2)
						pRealLs->des_base_pos.m_biWingOffsetStyle=2;
					else if(pBoltConn->rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g3)
						pRealLs->des_base_pos.m_biWingOffsetStyle=3;
					pRealLs->des_work_norm.norm_style=1;	//指定角钢肢法线
					pRealLs->des_work_norm.direction=0;
					pRealLs->des_work_norm.hVicePart = pSonBaseJg->handle;
					pRealLs->des_work_norm.norm_wing=0;
					pRealLs->correct_pos();
					pSonBaseJg->AppendStartLsRef(pRealLs->GetLsRef());
					pJgArr[(ii+1)%4]->AppendStartLsRef(pRealLs->GetLsRef());

					if(fabs(pSonBaseJg->get_norm_x_wing()*base.pPlate->ucs.axis_y)>
						fabs(pSonBaseJg->get_norm_y_wing()*base.pPlate->ucs.axis_y))
					{
						if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							pXBackPlate->AppendLsRef(pRealLs->GetLsRef());	//当前角钢为组合角钢特征角钢
						else
						{	//当前角钢非组合角钢特征角钢
							if(pXFrontPlate)
								pXFrontPlate->AppendLsRef(pRealLs->GetLsRef());
							else
								pXBackPlate->AppendLsRef(pRealLs->GetLsRef());
						}
					}
					else
					{
						if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							pYBackPlate->AppendLsRef(pRealLs->GetLsRef());	//当前角钢为组合角钢特征角钢
						else
						{	//当前角钢非组合角钢特征角钢
							if(pYFrontPlate)
								pYFrontPlate->AppendLsRef(pRealLs->GetLsRef());
							else
								pYBackPlate->AppendLsRef(pRealLs->GetLsRef());
						}
					}
					//添加Y肢螺栓
					defaultZJ=pSonBaseJg->GetZhunJu('Y');
					pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					ls_pos_y = ls_pos_y-pSonBaseJg->get_norm_y_wing()*pBaseJg->GetThick();
					_snprintf(ls_y.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-pBaseJg->GetThick());
					ls_y.ucs.origin=ls_pos_y;
					pRealLs->CopyProperty(&ls_y);
					pRealLs->CopyModuleInstanceInfo(pSonBaseJg);	//调整螺栓配材号与基准构件配材号一致
					pRealLs->des_base_pos.datumPoint=datumPointY;
					pRealLs->des_base_pos.hPart=pSonBaseJg->handle;
					pRealLs->des_base_pos.offset_wing=1;
					if(pBaseJg->pStart->handle==m_hBaseNode)
						pRealLs->des_base_pos.direction=0;
					else
						pRealLs->des_base_pos.direction=1;
					pRealLs->des_base_pos.len_offset_dist=ls_y.des_base_pos.len_offset_dist;
					pRealLs->des_base_pos.wing_offset_dist = ls_y.des_base_pos.wing_offset_dist;
					if(pBoltConn->rows==1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g)
						pRealLs->des_base_pos.m_biWingOffsetStyle=0;
					else if(pBoltConn->rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g1)
						pRealLs->des_base_pos.m_biWingOffsetStyle=1;
					else if(pBoltConn->rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g2)
						pRealLs->des_base_pos.m_biWingOffsetStyle=2;
					else if(pBoltConn->rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g3)
						pRealLs->des_base_pos.m_biWingOffsetStyle=3;
					pRealLs->des_work_norm.norm_style=1;	//指定角钢肢法线
					pRealLs->des_work_norm.direction=0;
					pRealLs->des_work_norm.hVicePart = pSonBaseJg->handle;
					pRealLs->des_work_norm.norm_wing=1;
					pRealLs->correct_pos();
					pSonBaseJg->AppendStartLsRef(pRealLs->GetLsRef());

					pJgArr[(ii+3)%4]->AppendStartLsRef(pRealLs->GetLsRef());

					if(fabs(pSonBaseJg->get_norm_x_wing()*base.pPlate->ucs.axis_y)>
						fabs(pSonBaseJg->get_norm_y_wing()*base.pPlate->ucs.axis_y))
					{
						if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							pYBackPlate->AppendLsRef(pRealLs->GetLsRef());	//当前角钢为组合角钢特征角钢
						else
						{	//当前角钢非组合角钢特征角钢
							if(pYFrontPlate)
								pYFrontPlate->AppendLsRef(pRealLs->GetLsRef());
							else
								pYBackPlate->AppendLsRef(pRealLs->GetLsRef());
						}
					}
					else
					{
						if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							pXBackPlate->AppendLsRef(pRealLs->GetLsRef());	//当前角钢为组合角钢特征角钢
						else
						{	//当前角钢非组合角钢特征角钢
							if(pXFrontPlate)
								pXFrontPlate->AppendLsRef(pRealLs->GetLsRef());
							else
								pXBackPlate->AppendLsRef(pRealLs->GetLsRef());
						}
					}
				}
			}
		}
	}
	else								//单角钢
	{	//布置始端主材上的螺栓(螺栓准据根据用户输入的为主)
		CLDSBolt ls_x(console.GetActiveModel()),ls_y(console.GetActiveModel());
		datumPointX.des_para.RODEND.offset_wing=0;
		datumPointY.des_para.RODEND.offset_wing=1;
		datumPointX.datum_pos_style					=datumPointY.datum_pos_style=1;
		datumPointX.des_para.RODEND.hRod			=datumPointY.des_para.RODEND.hRod=pBaseJg->handle;
		if(pBaseJg->pStart->handle==m_hBaseNode)
			datumPointX.des_para.RODEND.direction	=datumPointY.des_para.RODEND.direction=0;
		else
			datumPointX.des_para.RODEND.direction	=datumPointY.des_para.RODEND.direction=1;
		datumPointX.des_para.RODEND.len_offset_dist	=datumPointY.des_para.RODEND.len_offset_dist=0;
		datumPointX.des_para.RODEND.bIncOddEffect	=datumPointY.des_para.RODEND.bIncOddEffect=TRUE;
		datumPointX.des_para.RODEND.wing_offset_dist=datumPointY.des_para.RODEND.wing_offset_dist=0;
		datumPointX.des_para.RODEND.wing_offset_style=datumPointY.des_para.RODEND.wing_offset_style=4;
		ls_x.set_d(ls_group.d);
		ls_x.AddL0Thick(pXBackPlate->handle,TRUE);
		ls_x.AddL0Thick(pBaseJg->handle,TRUE);
		ls_y.set_d(ls_group.d);
		ls_y.AddL0Thick(pYBackPlate->handle,TRUE);
		ls_y.AddL0Thick(pBaseJg->handle,TRUE);
		if(!ls_x.CalGuigeAuto())
		{
			char sError[200];
#ifdef AFX_TARG_ENU_ENGLISH
			_snprintf(sError,199,"Bolt size M%dX%.f not found!",ls_group.d,ls_x.L0);
#else
			_snprintf(sError,199,"找不到M%dX%.f规格的螺栓!",ls_x.get_d(),ls_x.L0);
#endif
			throw sError;
		}
		ls_x.iSeg=pBaseJg->iSeg;	//主材螺栓段号 wht 10-09-15
		ls_y.iSeg=pBaseJg->iSeg;
		ls_y.CalGuigeAuto();
		ls_x.set_norm(pBaseJg->get_norm_x_wing());
		ls_y.set_norm(pBaseJg->get_norm_y_wing());

		f3dPoint ls_pos_x,ls_pos_y;
		int offset = LsSpace.EndSpace;
		for(int j=0;j<ls_group.wnConnBoltN;j++)
		{
			if(ls_group.rows==1)
			{		//单排螺栓
				{	//单排螺栓螺栓正常排列
					if(iLsLayStyle==0)	//左高右底
					{
						ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,tempory.wing_vec_x,
							pBaseJg->Start(),offset+LsSpace.SingleRowSpace/2,jgzj_x.g);
						ls_x.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
						ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
						ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,tempory.wing_vec_y,
							pBaseJg->Start(),offset,jgzj_y.g);
						ls_y.des_base_pos.len_offset_dist = offset;
						ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
					}
					else
					{
						ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,tempory.wing_vec_x,
							pBaseJg->Start(),offset,jgzj_x.g);
						ls_x.des_base_pos.len_offset_dist = offset;
						ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
						ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,tempory.wing_vec_y,
							pBaseJg->Start(),offset+LsSpace.SingleRowSpace/2,jgzj_y.g);
						ls_y.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
						ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
					}
				}
				offset += LsSpace.SingleRowSpace;
			}
			else	//双排螺栓
			{
				if(iLsLayStyle==0)
				{	//靠近楞线一侧螺栓优先排列
					if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
					{
						ls_x.dwRayNo = GetSingleWord(2);
						ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,tempory.wing_vec_x,
							pBaseJg->Start(),offset,jgzj_x.g1);
						ls_x.des_base_pos.len_offset_dist = offset;
						ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
						ls_y.dwRayNo = GetSingleWord(3);
						ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,tempory.wing_vec_y,
							pBaseJg->Start(),offset,jgzj_y.g2);
						ls_y.des_base_pos.len_offset_dist = offset;
						ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
					}
					else		//奇数号螺栓位于背离楞线一侧	
					{
						ls_x.dwRayNo = GetSingleWord(3);
						ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,tempory.wing_vec_x,
							pBaseJg->Start(),offset,jgzj_x.g2);
						ls_x.des_base_pos.len_offset_dist = offset;
						ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
						ls_y.dwRayNo = GetSingleWord(2);
						ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,tempory.wing_vec_y,
							pBaseJg->Start(),offset,jgzj_y.g1);
						ls_y.des_base_pos.len_offset_dist = offset;
						ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
					}
				}
				else
				{	//背离楞线一侧螺栓优先排列
					if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
					{
						ls_x.dwRayNo = GetSingleWord(3);
						ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,tempory.wing_vec_x,
							pBaseJg->Start(),offset,jgzj_x.g2);
						ls_x.des_base_pos.len_offset_dist = offset;
						ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
						ls_y.dwRayNo = GetSingleWord(2);
						ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,tempory.wing_vec_y,
							pBaseJg->Start(),offset,jgzj_y.g1);
						ls_y.des_base_pos.len_offset_dist = offset;
						ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
					}
					else		//奇数号螺栓位于背离楞线一侧	
					{
						ls_x.dwRayNo = GetSingleWord(2);
						ls_pos_x = get_pt_by_vec(-tempory.datumRayVec,tempory.wing_vec_x,
							pBaseJg->Start(),offset,jgzj_x.g1);
						ls_x.des_base_pos.len_offset_dist = offset;
						ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
						ls_y.dwRayNo = GetSingleWord(3);
						ls_pos_y = get_pt_by_vec(-tempory.datumRayVec,tempory.wing_vec_y,
							pBaseJg->Start(),offset,jgzj_y.g2);
						ls_y.des_base_pos.len_offset_dist = offset;
						ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
					}
				}
				offset+=LsSpace.doubleRowSpace/2;
			}
			//布置X肢上螺栓
			defaultZJ=pBaseJg->GetZhunJu('X');
			CLDSBolt *pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			ls_pos_x = ls_pos_x-pBaseJg->get_norm_x_wing()*pBaseJg->GetThick();
			_snprintf(ls_x.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-pBaseJg->GetThick());
			ls_x.ucs.origin=ls_pos_x;
			pRealLs->CopyProperty(&ls_x);
			pRealLs->CopyModuleInstanceInfo(pBaseJg);	//调整螺栓配材号与基准构件配材号一致
			pRealLs->des_base_pos.datumPoint=datumPointX;
			pRealLs->des_base_pos.hPart=pBaseJg->handle;
			pRealLs->des_base_pos.offset_wing=0;
			if(pBaseJg->pStart->handle==m_hBaseNode)
				pRealLs->des_base_pos.direction=0;
			else
				pRealLs->des_base_pos.direction=1;
			pRealLs->des_base_pos.len_offset_dist=ls_x.des_base_pos.len_offset_dist;
			pRealLs->des_base_pos.wing_offset_dist = ls_x.des_base_pos.wing_offset_dist;
			if(ls_group.rows==1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g)
				pRealLs->des_base_pos.m_biWingOffsetStyle=0;
			else if(ls_group.rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g1)
				pRealLs->des_base_pos.m_biWingOffsetStyle=1;
			else if(ls_group.rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g2)
				pRealLs->des_base_pos.m_biWingOffsetStyle=2;
			else if(ls_group.rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g3)
				pRealLs->des_base_pos.m_biWingOffsetStyle=3;
			pRealLs->des_work_norm.norm_style=1;	//指定角钢肢法线
			pRealLs->des_work_norm.direction=0;
			pRealLs->des_work_norm.hVicePart = pBaseJg->handle;
			pRealLs->des_work_norm.norm_wing=0;
			pRealLs->correct_pos();
			pBaseJg->AppendStartLsRef(pRealLs->GetLsRef());

			if(fabs(pBaseJg->get_norm_x_wing()*base.pPlate->ucs.axis_y)>
				fabs(pBaseJg->get_norm_y_wing()*base.pPlate->ucs.axis_y))
				pXBackPlate->AppendLsRef(pRealLs->GetLsRef());
			else
				pYBackPlate->AppendLsRef(pRealLs->GetLsRef());
			//布置Y肢上的螺栓
			defaultZJ=pBaseJg->GetZhunJu('Y');
			pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			ls_pos_y = ls_pos_y-pBaseJg->get_norm_y_wing()*pBaseJg->GetThick();
			_snprintf(ls_y.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-pBaseJg->GetThick());
			ls_y.ucs.origin=ls_pos_y;
			pRealLs->CopyProperty(&ls_y);
			pRealLs->CopyModuleInstanceInfo(pBaseJg);	//调整螺栓配材号与基准构件配材号一致
			pRealLs->des_base_pos.datumPoint=datumPointY;
			pRealLs->des_base_pos.hPart=pBaseJg->handle;
			pRealLs->des_base_pos.offset_wing=1;
			if(pBaseJg->pStart->handle==m_hBaseNode)
				pRealLs->des_base_pos.direction=0;
			else
				pRealLs->des_base_pos.direction=1;
			pRealLs->des_base_pos.len_offset_dist=ls_y.des_base_pos.len_offset_dist;
			pRealLs->des_base_pos.wing_offset_dist = ls_y.des_base_pos.wing_offset_dist;
			if(ls_group.rows==1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g)
				pRealLs->des_base_pos.m_biWingOffsetStyle=0;
			else if(ls_group.rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g1)
				pRealLs->des_base_pos.m_biWingOffsetStyle=1;
			else if(ls_group.rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g2)
				pRealLs->des_base_pos.m_biWingOffsetStyle=2;
			else if(ls_group.rows!=1&&pRealLs->des_base_pos.wing_offset_dist==defaultZJ.g3)
				pRealLs->des_base_pos.m_biWingOffsetStyle=3;
			pRealLs->des_work_norm.norm_style=1;	//指定角钢肢法线
			pRealLs->des_work_norm.direction=0;
			pRealLs->des_work_norm.hVicePart = pBaseJg->handle;
			pRealLs->des_work_norm.norm_wing=1;
			pRealLs->correct_pos();
			pBaseJg->AppendStartLsRef(pRealLs->GetLsRef());

			if(fabs(pBaseJg->get_norm_x_wing()*base.pPlate->ucs.axis_y)>
				fabs(pBaseJg->get_norm_y_wing()*base.pPlate->ucs.axis_y))
				pYBackPlate->AppendLsRef(pRealLs->GetLsRef());
			else
				pXBackPlate->AppendLsRef(pRealLs->GetLsRef());
		}
	}
	pBaseJg->SetModified();
}
bool CFootTempl::LayRayAngleBolt(ATOM_LIST<CDesignLsPara> *pLsList,CLDSLineAngle* pRayAngle,CLDSLineAngle *pBaseAngle,double *ber_space,double *wing_space)
{
	if(pRayAngle==NULL)
		return false;
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(pRayAngle->handle);
	if(pBaseAngle&&pBaseAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		CLDSGroupLineAngle* pBaseGroupAngle=(CLDSGroupLineAngle*)pBaseAngle;
		pBaseAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pBaseGroupAngle->m_hDatumSonAngle,CLS_LINEANGLE);
	}
	//获取连接面法线,根据连接面法线计算基准角钢及斜材角钢的当前连接肢及里外铁
	f3dPoint work_norm;
	GetWorkNorm(pBaseAngle,pRayAngle,&work_norm);
	int xie_x_wing0_y_wing1=0;
	BOOL bInsideXieJg=IsInsideJg(pRayAngle,work_norm,&xie_x_wing0_y_wing1);
	CConnectInfo ls_group=(pRayAngle->pStart->handle==m_hBaseNode)?pRayAngle->connectStart:pRayAngle->connectEnd;
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,ls_group.d);
	//初始化角钢螺栓设计参数
	CLayAngleBoltDlg dlg;
	dlg.m_bIncPlateProfilePara = TRUE;
	dlg.m_iOffsetWing=xie_x_wing0_y_wing1;
	if(pRayAngle->pStart->handle==m_hBaseNode)	//始端连接
	{
		dlg.m_pNode=pRayAngle->pStart;
		dlg.m_iRayDirection=0;
		dlg.m_iOddCalStyle=pRayAngle->desStartOdd.m_iOddCalStyle;
		if(dlg.m_iOddCalStyle!=1)
		{
			long hBaseAngle=pBaseAngle->handle;
			if(pBaseAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
				hBaseAngle=((CLDSGroupLineAngle*)pBaseAngle)->m_hDatumSonAngle;
			if(pRayAngle->desStartOdd.m_hRefPart1<0x20)
				pRayAngle->desStartOdd.m_hRefPart1=hBaseAngle;
			else if(pRayAngle->desStartOdd.m_hRefPart1!=hBaseAngle&&pRayAngle->desStartOdd.m_hRefPart2<0x20)
				pRayAngle->desStartOdd.m_hRefPart2=hBaseAngle;
			pRayAngle->CalStartOddment();
		}
		dlg.m_fOddment=pRayAngle->startOdd();
	}
	else
	{
		dlg.m_pNode=pRayAngle->pEnd;
		dlg.m_iRayDirection=1;
		dlg.m_iOddCalStyle=pRayAngle->desEndOdd.m_iOddCalStyle;
		if(dlg.m_iOddCalStyle!=1)
		{
			long hBaseAngle=pBaseAngle->handle;
			if(pBaseAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
				hBaseAngle=((CLDSGroupLineAngle*)pBaseAngle)->m_hDatumSonAngle;
			if(pRayAngle->desEndOdd.m_hRefPart1<0x20)
				pRayAngle->desEndOdd.m_hRefPart1=hBaseAngle;
			else if(pRayAngle->desEndOdd.m_hRefPart1!=hBaseAngle&&pRayAngle->desEndOdd.m_hRefPart2<0x20)
				pRayAngle->desEndOdd.m_hRefPart2=hBaseAngle;
			pRayAngle->CalEndOddment();
		}
		dlg.m_fOddment=pRayAngle->endOdd();
	}
	dlg.m_pLinePart=pRayAngle;
	dlg.m_bMirLayLs=FALSE;
	dlg.m_pLsList=pLsList;
	dlg.viewNorm = work_norm;
	dlg.m_sBoltSize.Format("%d",ls_group.d);
	dlg.m_iBoltRows = ls_group.rows-1;
	dlg.m_nLsNum = ls_group.wnConnBoltN;
	dlg.m_iBoltNorm=bInsideXieJg?0:1;
	dlg.m_nLsEndSpace = LsSpace.EndSpace;
	if(ls_group.rows==1)
		dlg.m_nLsSpace	= LsSpace.SingleRowSpace;
	else
		dlg.m_nLsSpace	= LsSpace.doubleRowSpace;
	if(dlg.DoModal()!=IDOK)
		return false;
	*ber_space=dlg.m_fBerSpace;
	*wing_space=dlg.m_fWingSpace;
	if(dlg.m_pNode==pRayAngle->pEnd)
	{
		pRayAngle->desEndOdd.m_iOddCalStyle=dlg.m_iOddCalStyle;
		pRayAngle->SetEndOdd(dlg.m_fOddment);
		if(dlg.m_iOffsetWing==0)	
			pRayAngle->desEndPos.wing_x_offset.offsetDist=dlg.m_fAngleEndNormOffset;
		else
			pRayAngle->desEndPos.wing_y_offset.offsetDist=dlg.m_fAngleEndNormOffset;
	}
	else if(dlg.m_pNode==pRayAngle->pStart)
	{
		pRayAngle->desStartOdd.m_iOddCalStyle=dlg.m_iOddCalStyle;
		pRayAngle->SetStartOdd(dlg.m_fOddment);
		if(dlg.m_iOffsetWing==0)
			pRayAngle->desStartPos.wing_x_offset.offsetDist=dlg.m_fAngleEndNormOffset;
		else
			pRayAngle->desStartPos.wing_y_offset.offsetDist=dlg.m_fAngleEndNormOffset;
	}
	return true;
}
//预设射线组合角钢摆放位置
void CFootTempl::PresetGroupRayJgEndPos(CLDSGroupLineAngle* pAngle,CLDSLineAngle *pBaseAngle,BYTE bolt0_weld1,double fPlateThick)
{
	if(pAngle==NULL || pBaseAngle==NULL)
		return ;
	//获取连接面法线
	f3dPoint work_norm;
	GetWorkNorm(pBaseAngle,pAngle,&work_norm);
	//根据连接面法线计算基准角钢及斜材角钢的当前连接肢及里外铁
	int xie_x_wing0_y_wing1=0,base_x_wing0_y_wing1=0;
	BOOL bInsideXieJg=IsInsideJg(pAngle,work_norm,&xie_x_wing0_y_wing1);
	IsInsideJg(pBaseAngle,work_norm,&base_x_wing0_y_wing1);
	if(pAngle->pStart->handle==m_hBaseNode)
	{
		if(pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE&&pBaseAngle->GetClassTypeId()==CLS_LINEANGLE)
		{	//射线角钢为组合角钢基准角钢为普通角钢
			if(pAngle->desStartPos.jgber_cal_style!=1&&pAngle->desStartPos.origin_pos_style==0&&pAngle->desStartPos.spatialOperationStyle==0)
			{
				//判断基准角钢是否为平推
				int offset_style=0;
				/*if(pBaseAngle->desEndPos.jgber_cal_style==2
					||(pBaseAngle->desEndPos.jgber_cal_style==0
					&&((pBaseAngle->desEndPos.datum_pos_style>3&&pBaseAngle->desEndPos.datum_pos_style<7)
					||(pBaseAngle->desEndPos.datum_pos_style>9&&pBaseAngle->desEndPos.datum_pos_style<13)
					||pBaseAngle->desEndPos.datum_to_ber_style==1)))*/
				if(pBaseAngle->desEndPos.IsFaceOffset()&&pBaseAngle->desEndPos.spatialOperationStyle<15)
					offset_style=3;	//搭接平推
				if(base_x_wing0_y_wing1==0)
					pAngle->desStartPos.spatialOperationStyle=2+offset_style;
				else if(base_x_wing0_y_wing1==1)
					pAngle->desStartPos.spatialOperationStyle=3+offset_style;
			}
			if(xie_x_wing0_y_wing1==0)
			{
				pAngle->desStartPos.wing_x_offset.gStyle = 4;
				if(bolt0_weld1==1)	//对焊
					pAngle->desStartPos.wing_x_offset.offsetDist=-((CLDSGroupLineAngle*)pAngle)->jg_space*0.5;
				else
					pAngle->desStartPos.wing_x_offset.offsetDist=fPlateThick-((CLDSGroupLineAngle*)pAngle)->jg_space*0.5;
			}
			else if(xie_x_wing0_y_wing1==1)
			{
				pAngle->desStartPos.wing_y_offset.gStyle = 4;
				if(bolt0_weld1==1)	//对焊
					pAngle->desStartPos.wing_y_offset.offsetDist=-((CLDSGroupLineAngle*)pAngle)->jg_space*0.5;
				else
					pAngle->desStartPos.wing_y_offset.offsetDist=fPlateThick-((CLDSGroupLineAngle*)pAngle)->jg_space*0.5;
			}
		}
	}
	else
	{
		if(pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE&&pBaseAngle->GetClassTypeId()==CLS_LINEANGLE)
		{	//射线角钢为组合角钢基准角钢为普通角钢
			if(pAngle->desEndPos.jgber_cal_style!=1&&pAngle->desEndPos.origin_pos_style==0&&pAngle->desEndPos.spatialOperationStyle==0)
			{
				//判断基准角钢是否为平推
				int offset_style=0;
				/*if(pBaseAngle->desEndPos.jgber_cal_style==2
					||(pBaseAngle->desEndPos.jgber_cal_style==0
					&&((pBaseAngle->desEndPos.datum_pos_style>3&&pBaseAngle->desEndPos.datum_pos_style<7)
					||(pBaseAngle->desEndPos.datum_pos_style>9&&pBaseAngle->desEndPos.datum_pos_style<13)
					||pBaseAngle->desEndPos.datum_to_ber_style==1)))*/
				if(pBaseAngle->desEndPos.IsFaceOffset()&&pBaseAngle->desEndPos.spatialOperationStyle<15)
					offset_style=3;	//搭接平推
				if(base_x_wing0_y_wing1==0)
					pAngle->desEndPos.spatialOperationStyle=2+offset_style;
				else if(base_x_wing0_y_wing1==1)
					pAngle->desEndPos.spatialOperationStyle=3+offset_style;
			}
			if(xie_x_wing0_y_wing1==0)
			{
				pAngle->desEndPos.wing_x_offset.gStyle = 4;
				if(bolt0_weld1==1)	//对焊
					pAngle->desEndPos.wing_x_offset.offsetDist=-((CLDSGroupLineAngle*)pAngle)->jg_space*0.5;
				else
					pAngle->desEndPos.wing_x_offset.offsetDist=fPlateThick-((CLDSGroupLineAngle*)pAngle)->jg_space*0.5;
			}
			else if(xie_x_wing0_y_wing1==1)
			{
				pAngle->desEndPos.wing_y_offset.gStyle = 4;
				if(bolt0_weld1==1)	//对焊
					pAngle->desEndPos.wing_y_offset.offsetDist=-((CLDSGroupLineAngle*)pAngle)->jg_space*0.5;
				else
					pAngle->desEndPos.wing_y_offset.offsetDist=fPlateThick-((CLDSGroupLineAngle*)pAngle)->jg_space*0.5;
			}
		}
	}
	//
	pAngle->ClearFlag();
	pAngle->CalPosition();
	pAngle->UpdateSonJgPos();
}
// 预设射线角钢摆放位置
void CFootTempl::PresetRayJgEndPos(CLDSLineAngle *pAngle,CLDSLineAngle *pBaseAngle,BYTE bolt0_weld1,double fPlateThick)
{	
	if(pAngle==NULL || pBaseAngle==NULL)
		return;
	//判断射线连接的基准角钢是否为组合角钢
	CLDSGroupLineAngle* pGroupBaseJg=NULL;
	if(pBaseAngle->GetClassTypeId()==CLS_GROUPLINEANGLE||pBaseAngle->group_father_jg_h>0X20)
	{
		if(pBaseAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
			pGroupBaseJg=(CLDSGroupLineAngle*)pBaseAngle;
		else
			pGroupBaseJg=(CLDSGroupLineAngle*)console.FromPartHandle(pBaseAngle->group_father_jg_h,CLS_GROUPLINEANGLE);
	}
	//获取连接面法线
	double fNormOffset=0;
	int xie_x_wing0_y_wing1=0;
	f3dPoint work_norm;
	GetWorkNorm(pBaseAngle,pAngle,&work_norm);
	BOOL bInsideXieJg=IsInsideJg(pAngle,work_norm,&xie_x_wing0_y_wing1);	
	if(bInsideXieJg)
	{	//里铁
		if(bolt0_weld1==1)
		{	//对焊
			if(pGroupBaseJg)
				fNormOffset=-pGroupBaseJg->jg_space*0.5-fPlateThick;
			else
				fNormOffset=-fPlateThick;
		}
		else //if(bolt0_weld1!=1)
		{	//螺栓连接
			if(pGroupBaseJg)
				fNormOffset=-pGroupBaseJg->jg_space*0.5;
			else
				fNormOffset=0;
		}
	}
	else if(!bInsideXieJg)
	{	//外铁
		if(bolt0_weld1==1)
		{	//对焊
			if(pGroupBaseJg)
				fNormOffset=pGroupBaseJg->jg_space*0.5;
			else
				fNormOffset=0;
		}
		else //if(bolt0_weld1!=1)
		{	//螺栓连接
			if(pGroupBaseJg)
				fNormOffset=pGroupBaseJg->jg_space*0.5-fPlateThick;
			else
				fNormOffset=-fPlateThick;
		}
	}
	BOOL bLinkStart=(pAngle->pStart->handle==m_hBaseNode)?TRUE:FALSE;
	if(xie_x_wing0_y_wing1==0)
	{	//斜材连接肢为X肢
		bLinkStart?pAngle->desStartPos.wing_x_offset.gStyle=4:pAngle->desEndPos.wing_x_offset.gStyle=4;
		bLinkStart?pAngle->desStartPos.wing_x_offset.offsetDist=fNormOffset:pAngle->desEndPos.wing_x_offset.offsetDist=fNormOffset;
	}
	else
	{	//斜材连接肢为Y肢
		bLinkStart?pAngle->desStartPos.wing_y_offset.gStyle=4:pAngle->desEndPos.wing_y_offset.gStyle=4;
		bLinkStart?pAngle->desStartPos.wing_y_offset.offsetDist=fNormOffset:pAngle->desEndPos.wing_y_offset.offsetDist=fNormOffset;
	}
	pAngle->ClearFlag();
	pAngle->CalPosition();
}
//the following function is from DesignJdb.cpp
int GetDeita23ByLsDiameter(int diameter);
//设计底脚板(牛蹄板)处的射线材角钢
bool CFootTempl::DesignRayAngle(LINKANGLE *pLinkAngle,SHOEPLATE* pShoePlate)
{
	if(pLinkAngle->pAngle==NULL)
		return false;
	if(pLinkAngle->pAngle==primaryAngle.pAngle||pLinkAngle->pAngle==primaryAngleDown.pAngle)
		return false;
	CLDSLineAngle *pBaseJg=NULL,*pAngle=pLinkAngle->pAngle;
	f3dPoint vec;
	if(pAngle->pStart->handle==m_hBaseNode)
		vec=(pAngle->End()-pAngle->Start()).normalized();
	else
		vec=(pAngle->Start()-pAngle->End()).normalized();
	if(vec*base.pPlate->ucs.axis_z>eps2)	//上侧斜材
		pBaseJg=primaryAngle.pAngle;
	else //if(vec*base.pPlate->ucs.axis_z<-eps2)	//下侧斜材或横材
		pBaseJg=primaryAngleDown.pAngle;
	if(pShoePlate->pPlate&&pShoePlate->pPlate->GetClassTypeId()==CLS_PARAMPLATE)
	{	//有靴板时，优先根据靴板依赖杆件获取角钢
		long hBaseJg=0;
		((CLDSParamPlate*)pShoePlate->pPlate)->GetDesignItemValue('D',&hBaseJg);
		pBaseJg=(CLDSLineAngle*)console.FromPartHandle(hBaseJg,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	}
	if(pBaseJg==NULL)	//无下侧角钢易死机 WJH-2003.11.30
		return false;
	typedef	CLDSLineAngle*	LINEANGLEPTR;
	LINEANGLEPTR angleArr[4]={NULL};
	if(pAngle->GetClassTypeId()==CLS_LINEANGLE)
		angleArr[0]=pAngle;
	else if(pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{	//组合角钢
		CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pAngle;
		for(int i=0;i<4;i++)
		{
			if(pGroupAngle->group_style<2&&i>=2)
				continue;	//个别四角钢转换为双拼角钢时，会把第三、四根子角钢转换为虚角钢存在
			if(pGroupAngle->son_jg_h[i]>=0x20)
				angleArr[i]=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
		}
	}
	//设置斜材角钢位置设计参数
	BYTE bolt0_weld1=(pShoePlate->label.CharAt(0)=='U')?upLinkSP.style_bolt0_weld1:downLinkSP.style_bolt0_weld1;
	if(pAngle->GetClassTypeId()==CLS_LINEANGLE)
		PresetRayJgEndPos(pAngle,pBaseJg,bolt0_weld1,pShoePlate->pPlate->Thick);
	else if(pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		PresetGroupRayJgEndPos((CLDSGroupLineAngle*)pAngle,pBaseJg,bolt0_weld1,pShoePlate->pPlate->Thick);
	//设计斜材角钢螺栓，计算特征轮廓点
	for(int i=0;i<4;i++)
	{
		if((pAngle=angleArr[i])==NULL)
			continue;
		//判断当前肢
		BOOL bInFootX;
		f3dPoint work_norm;
		GetWorkNorm(pBaseJg,pAngle,&work_norm);	
		if(fabs(dot_prod(work_norm,base.pPlate->ucs.axis_y))>fabs(dot_prod(work_norm,base.pPlate->ucs.axis_x)))
			bInFootX = TRUE;	//底脚板X轴向的肢
		else
			bInFootX = FALSE;	//底脚板Y轴向的肢
		//选择当前设计靴板
		CLDSParamPlate *pFrontPlank=NULL,*pBackPlank=NULL;
		if(pShoePlate->label.CharAt(0)=='U')	//上侧靴板
		{
			if(bInFootX)
			{
				pFrontPlank=tempory.pXUpFrontPlank;
				pBackPlank=tempory.pXUpBackPlank;
			}
			else
			{
				pFrontPlank=tempory.pYUpFrontPlank;
				pBackPlank=tempory.pYUpBackPlank;
			}
		}
		else	//下侧靴板
		{
			if(bInFootX)
			{
				pFrontPlank=tempory.pXDownFrontPlank;
				pBackPlank=tempory.pXDownBackPlank;
			}
			else
			{
				pFrontPlank=tempory.pYDownFrontPlank;
				pBackPlank=tempory.pYDownBackPlank;
			}
		}
		//确定射线斜材角钢的设计参数
		f3dPoint base_pos,line_vec;
		if(pAngle->pStart->handle==m_hBaseNode)
		{
			pAngle->SetBoltsNumOn1Wing(pLinkAngle->bolt.num,TRUE);
			pAngle->connectStart.d=pLinkAngle->bolt.d;
			pAngle->connectStart.rows=pLinkAngle->bolt.rows;
			//
			base_pos=pAngle->Start();
			line_vec=(pAngle->End()-pAngle->Start()).normalized();
			//
			long hBaseAngle=pBaseJg->handle;
			if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
				hBaseAngle=((CLDSGroupLineAngle*)pBaseJg)->m_hDatumSonAngle;
			if(pAngle->desStartOdd.m_hRefPart1<0x20)
				pAngle->desStartOdd.m_hRefPart1=hBaseAngle;
			else if(pAngle->desStartOdd.m_hRefPart1!=hBaseAngle&&pAngle->desStartOdd.m_hRefPart2<0x20)
				pAngle->desStartOdd.m_hRefPart2=hBaseAngle;
			pAngle->CalStartOddment();
		}
		else
		{
			pAngle->SetBoltsNumOn1Wing(pLinkAngle->bolt.num,FALSE);
			pAngle->connectEnd.d=pLinkAngle->bolt.d;
			pAngle->connectEnd.rows=pLinkAngle->bolt.rows;
			//
			base_pos=pAngle->End();
			line_vec=(pAngle->Start()-pAngle->End()).normalized();
			//
			long hBaseAngle=pBaseJg->handle;
			if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
				hBaseAngle=((CLDSGroupLineAngle*)pBaseJg)->m_hDatumSonAngle;
			if(pAngle->desEndOdd.m_hRefPart1<0x20)
				pAngle->desEndOdd.m_hRefPart1=hBaseAngle;
			else if(pAngle->desEndOdd.m_hRefPart1!=hBaseAngle&&pAngle->desEndOdd.m_hRefPart2<0x20)
				pAngle->desEndOdd.m_hRefPart2=hBaseAngle;
			pAngle->CalEndOddment();
		}
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace,pLinkAngle->bolt.d);
		double ber_space=LsSpace.EndSpace,wing_space=LsSpace.EndSpace;
		ATOM_LIST<CDesignLsPara>ls_list;
		if(pShoePlate->bAutoLayBolt)
			SmartInitRayJgLsParas(console.FromNodeHandle(m_hBaseNode),pAngle,pBaseJg,&ls_list);
		else
			LayRayAngleBolt(&ls_list,pAngle,pBaseJg,&ber_space,&wing_space);
		//根据用户输入在角钢上布置螺栓
		double tail_len=0,max_ber_dist=0,min_ber_dist=pAngle->GetWidth();
		f3dLine ber_line(pAngle->Start(),pAngle->End());
		CLDSBolt* pFirstLs=NULL;
		for(CDesignLsPara *pLsPara=ls_list.GetFirst();pLsPara;pLsPara=ls_list.GetNext())
		{
			CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->des_base_pos=pLsPara->des_base_pos;
			pBolt->des_work_norm=pLsPara->des_work_norm;
			if(pBolt->des_work_norm.direction==0)
				_snprintf(pBolt->des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"-0X%X",pAngle->handle);
			else
				_snprintf(pBolt->des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"-0X%X",pBackPlank->handle);
			pBolt->set_d(pLsPara->d);
			pBolt->iSeg = pLsPara->iSeg;
			pBolt->SetGrade(pLsPara->grade);
			pBolt->CopyModuleInstanceInfo(pAngle);	//调整螺栓配材号与基准构件配材号一致
			pBolt->AddL0Thick(pAngle->handle,TRUE);
			pBolt->AddL0Thick(pBackPlank->handle,TRUE);
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->CalGuigeAuto();	//计算通厚时可能会调用螺栓方位 wjh-2019.7.15
			if (UI::blEnableIntermediateUpdateUI)
			{
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			}
			//
			if(m_hBaseNode==pAngle->pStart->handle)
				pAngle->AppendStartLsRef(pBolt->GetLsRef());
			else if(m_hBaseNode==pAngle->pEnd->handle)
				pAngle->AppendEndLsRef(pBolt->GetLsRef());
			else
				pAngle->AppendMidLsRef(pBolt->GetLsRef());
			pBackPlank->AppendLsRef(pBolt->GetLsRef());
			//
			f3dPoint ls_pos=pBolt->ucs.origin;
			if(pLsPara->des_base_pos.offset_wing==0)
				project_point(ls_pos,base_pos,pAngle->get_norm_x_wing());
			else
				project_point(ls_pos,base_pos,pAngle->get_norm_y_wing());
			double dist;
			SnapPerp(&ls_pos,ber_line,ls_pos,&dist);
			if(pFirstLs==NULL)
				max_ber_dist=min_ber_dist=dist;
			else if(dist>max_ber_dist)
				max_ber_dist=dist;
			else if(dist<min_ber_dist)
				min_ber_dist=dist;
			f3dPoint ls_offset_vec = ls_pos-base_pos;
			if(ls_offset_vec*line_vec>0)
				dist = ls_offset_vec.mod();
			else
				dist = -ls_offset_vec.mod();
			if(pFirstLs==NULL)
				tail_len=dist;
			else if(dist>tail_len)
				tail_len=dist;
			if(pFirstLs==NULL)
				pFirstLs=pBolt;
		}
		if(pFirstLs==NULL)
			return false;
		//定制外形
		f3dPoint wing_vec,vertice,vv,vertex_up,vertex_down;
		if(pFirstLs->des_base_pos.offset_wing==0)
			wing_vec=pAngle->GetWingVecX();
		else
			wing_vec=pAngle->GetWingVecY();
		vv=base_pos+line_vec*(tail_len+LsSpace.EndSpace);
		vertice=vv+wing_vec*(min_ber_dist-ber_space);
		coord_trans(vertice,pBackPlank->ucs,FALSE);
		vertice.z=0;
		vertex_up=vertice;		//用于计算焊接线位置
		PROFILE_VERTEX *pVertex=pBackPlank->vertex_list.append(PROFILE_VERTEX(vertice));
		vertice=vv+wing_vec*(max_ber_dist+wing_space);
		coord_trans(vertice,pBackPlank->ucs,FALSE);
		vertice.z=0;
		vertex_down=vertice;	//用于计算焊接线位置
		pVertex=pBackPlank->vertex_list.append(PROFILE_VERTEX(vertice));
		//
		if(m_hBaseNode==pAngle->pStart->handle&&pAngle->desStartOdd.m_iOddCalStyle==1)
			pAngle->CalStartOddment();
		else if(m_hBaseNode==pAngle->pEnd->handle&&pAngle->desEndOdd.m_iOddCalStyle==1)
			pAngle->CalEndOddment();
		pAngle->SetModified();
		pAngle->CalPosition();
		if (UI::blEnableIntermediateUpdateUI)
		{
			pAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole, g_pSolidOper->GetScaleUserToScreen(), g_pSolidSet->GetArcSamplingLength(), g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pAngle->GetSolidPartObject());
		}
	}
	g_pSolidDraw->Draw();	
	
	return true;
}
bool CFootTempl::DesignShoePlates()
{
	SHOEPLATE* pShoePlate;
	for(pShoePlate=listShoePlates.GetFirst();pShoePlate;pShoePlate=listShoePlates.GetNext())
	{
		if(!pShoePlate->cStateFlag.IsFromModel())
			continue;
		CLDSParamPlate* pParamPlate =(CLDSParamPlate*)Ta.Parts.append(CLS_PARAMPLATE);
		pShoePlate->pPlate=pParamPlate;
		pParamPlate->m_hPartWeldParent=base.pPlate->handle;
		pParamPlate->CopyModuleInstanceInfo(base.pPlate);
		pParamPlate->m_iParamType=TYPE_SHOEPLATE;	//塔脚靴板
		pParamPlate->cMaterial=pShoePlate->cMaterial;
		pParamPlate->m_cFuncType=CLDSPart::FUNC_FOOTPLATE;
		pParamPlate->thick=pShoePlate->thick;
		pParamPlate->SetPartNo(pShoePlate->sPartNo);
		SEGI iSeg;
		if(strlen(pShoePlate->sPartNo)>0&&ParsePartNo(pShoePlate->sPartNo,&iSeg,NULL))
			pParamPlate->iSeg=iSeg;
		else
			pParamPlate->iSeg=base.pPlate->iSeg;
		//外形参数
		if(pShoePlate->label.CharAt(0)=='U')
		{
			pParamPlate->SetDesignItemValue('A',upLinkSP.A,NULL,PARAM_SHAPE);	//台阶高
			pParamPlate->SetDesignItemValue('B',upLinkSP.B,NULL,PARAM_SHAPE);	//顶部前侧余量
			pParamPlate->SetDesignItemValue(KEY2C("IA"),upLinkSP.C,NULL,PARAM_SHAPE);	//内侧台阶高
			pParamPlate->SetDesignItemValue(KEY2C("IB"),upLinkSP.D,NULL,PARAM_SHAPE);	//顶部内侧余量
			pParamPlate->SetDesignItemValue(KEY2C("IC"),upLinkSP.E,NULL,PARAM_SHAPE);	//斜材楞线侧延伸量
			pParamPlate->SetDesignItemValue(KEY2C("ID"),upLinkSP.F,NULL,PARAM_SHAPE);	//斜材肢翼侧延伸量
			pParamPlate->SetDesignItemValue('M',upLinkSP.M,NULL,PARAM_SHAPE);	//靴板延伸至底板边缘时的间隙值M==0表示延伸至底板边缘
			pParamPlate->SetDesignItemValue(KEY2C("AL"),upLinkSP.AL,NULL,PARAM_SHAPE);	//靴板非连接侧角钢长度方向延伸量，默认根据角钢螺栓计算
			pParamPlate->SetDesignItemValue(KEY2C("CM"),upLinkSP.CM,NULL,PARAM_SHAPE);
			if(upLinkSP.style_bolt0_weld1==1)	//设定对焊参数
				pParamPlate->SetDesignItemValue(KEY2C("CS"),1,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_INTEGER);
		}
		else
		{
			pParamPlate->SetDesignItemValue('A',downLinkSP.A,NULL,PARAM_SHAPE);	//台阶高
			pParamPlate->SetDesignItemValue('B',downLinkSP.B,NULL,PARAM_SHAPE);	//顶部前侧余量
			pParamPlate->SetDesignItemValue(KEY2C("IA"),downLinkSP.C,NULL,PARAM_SHAPE);	//内侧台阶高
			pParamPlate->SetDesignItemValue(KEY2C("IB"),downLinkSP.D,NULL,PARAM_SHAPE);	//顶部内侧余量
			pParamPlate->SetDesignItemValue(KEY2C("IC"),downLinkSP.E,NULL,PARAM_SHAPE);	//斜材楞线侧延伸量
			pParamPlate->SetDesignItemValue(KEY2C("ID"),downLinkSP.F,NULL,PARAM_SHAPE);	//斜材肢翼侧延伸量
			pParamPlate->SetDesignItemValue('M',downLinkSP.M,NULL,PARAM_SHAPE);	//靴板延伸至底板边缘时的间隙值M==0表示延伸至底板边缘
			pParamPlate->SetDesignItemValue(KEY2C("AL"),downLinkSP.AL,NULL,PARAM_SHAPE);	//靴板非连接侧角钢长度方向延伸量，默认根据角钢螺栓计算
			pParamPlate->SetDesignItemValue(KEY2C("CM"),downLinkSP.CM,NULL,PARAM_SHAPE);
			if(downLinkSP.style_bolt0_weld1==1)	//设定对焊参数
				pParamPlate->SetDesignItemValue(KEY2C("CS"),1,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_INTEGER);
		}
		pParamPlate->SetDesignItemValue('L',pShoePlate->fCutAngleWidth,NULL,PARAM_SHAPE);
		int profile_side=0;
		if(pShoePlate->label.CharAt(2)=='F')
			profile_side=2;
		else if(!pShoePlate->m_bTranverse)
			profile_side=1;
		pParamPlate->SetDesignItemValue('F',profile_side,"",PARAM_POS,2);	//外形类别 0.贯通 1.连接侧 2.外缘侧
		//装配参数
		pParamPlate->SetDesignItemValue(KEY2C("NL"),pShoePlate->norm_offset,NULL,PARAM_POS);
		pParamPlate->SetDesignItemValue('C',base.pPlate->handle,NULL,0,DESIGN_PARAM_ITEM::TYPE_HANDLE);	//塔脚底板句柄
	}
	tempory.pXUpFrontPlank=tempory.pXUpBackPlank=tempory.pYUpFrontPlank=tempory.pYUpBackPlank=
		tempory.pXDownFrontPlank=tempory.pXDownBackPlank=tempory.pYDownFrontPlank=tempory.pYDownBackPlank=NULL;
	if(listShoePlates.GetValue(KEY4C("UXB"))!=NULL)
		tempory.pXUpBackPlank=(CLDSParamPlate*)listShoePlates[KEY4C("UXB")].pPlate;
	else
		logerr.Log("UXB板缺失");
	if(listShoePlates.GetValue(KEY4C("UYB"))!=NULL)
		tempory.pYUpBackPlank=(CLDSParamPlate*)listShoePlates[KEY4C("UYB")].pPlate;
	else
		logerr.Log("UYB板缺失");
	if((pShoePlate=listShoePlates.GetValue(KEY4C("UXF")))!=NULL)
		tempory.pXUpFrontPlank=(CLDSParamPlate*)pShoePlate->pPlate;
	if((pShoePlate=listShoePlates.GetValue(KEY4C("UYF")))!=NULL)
		tempory.pYUpFrontPlank=(CLDSParamPlate*)pShoePlate->pPlate;
	if(primaryAngleDown.pAngle!=NULL)
	{
		if(listShoePlates.GetValue(KEY4C("DXB"))!=NULL)
			tempory.pXDownBackPlank=(CLDSParamPlate*)listShoePlates[KEY4C("DXB")].pPlate;
		else
			logerr.Log("DXB板缺失");
		if(listShoePlates.GetValue(KEY4C("DYB"))!=NULL)
			tempory.pYDownBackPlank=(CLDSParamPlate*)listShoePlates[KEY4C("DYB")].pPlate;
		else
			logerr.Log("DYB板缺失");
		if((pShoePlate=listShoePlates.GetValue(KEY4C("DXF")))!=NULL)
			tempory.pXDownFrontPlank=(CLDSParamPlate*)pShoePlate->pPlate;
		if((pShoePlate=listShoePlates.GetValue(KEY4C("DYF")))!=NULL)
			tempory.pYDownFrontPlank=(CLDSParamPlate*)pShoePlate->pPlate;
	}
	for(pShoePlate=listShoePlates.GetFirst();pShoePlate;pShoePlate=listShoePlates.GetNext())
	{
		CLDSLineAngle* pBaseAngle=NULL;
		if(!pShoePlate->cStateFlag.IsFromModel())
			continue;
		else if(pShoePlate->label.CharAt(0)=='U')
			pBaseAngle=primaryAngle.pAngle;
		else if(pShoePlate->label.CharAt(0)=='D')
			pBaseAngle=primaryAngleDown.pAngle;
		if(pBaseAngle==NULL)
			return false;
		CLDSParamPlate* pParamPlate =(CLDSParamPlate*)pShoePlate->pPlate;
		if(pShoePlate->label.CharAt(2)=='F'||!pShoePlate->m_bTranverse)
		{
			CXhChar16 key(pShoePlate->label.Label());
			key[1]='X'-key[1]+'Y';
			key[2]='B';
			SHOEPLATE* pConnectShoe=listShoePlates.GetValue(KEY4C(key));
			if(pConnectShoe&&pConnectShoe->pPlate)
				pParamPlate->SetDesignItemValue('G',pConnectShoe->pPlate->handle,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_HANDLE);
		}
		pParamPlate->SetDesignItemValue('D',pBaseAngle->handle,NULL,0,DESIGN_PARAM_ITEM::TYPE_HANDLE);	//依赖角钢
		if(pBaseAngle->size_cSubClassType=='D')
		{	//组合角钢为对角型组合时，确定靴板设计依赖的子角钢
			CLDSGroupLineAngle* pBaseGroupAngle=(CLDSGroupLineAngle*)pBaseAngle;
			long hMasterSonAngle=pBaseGroupAngle->son_jg_h[pBaseGroupAngle->ciMasterIndex];
			long hSlaveSonAngle=pBaseGroupAngle->son_jg_h[1-pBaseGroupAngle->ciMasterIndex];
			CLDSLineAngle* pMasterSonJg=(CLDSLineAngle*)console.FromPartHandle(hMasterSonAngle);
			CLDSLineAngle* pSlaveSonJg =(CLDSLineAngle*)console.FromPartHandle(hSlaveSonAngle);
			long profile_side=0;
			pParamPlate->GetDesignItemValue('F',&profile_side);
			CLDSLineAngle* pBackSonJg=pMasterSonJg,*pFrontSonJg=pSlaveSonJg;
			if (pShoePlate->label.CharAt(1)=='X')
			{	//取代下面通过quadModes的判断模式（因牛蹄板是因上面单角钢导致未赋值） wjh-2019.8.22
				double ddmx=pMasterSonJg->GetWingVecX()*tempory.ucs.axis_x;
				double ddmy=pMasterSonJg->GetWingVecY()*tempory.ucs.axis_x;
				double ddsx=pSlaveSonJg->GetWingVecX()*tempory.ucs.axis_x;
				double ddsy=pSlaveSonJg->GetWingVecY()*tempory.ucs.axis_x;
				if (max(ddsx,ddsy) < max(ddmx,ddmy))
				{	//X轴向靴板向心（后）侧与底座板X轴反向，取最大负值
					pBackSonJg=pSlaveSonJg;
					pFrontSonJg=pMasterSonJg;
				}
				if (profile_side==2)
				{
					pParamPlate->SetDesignItemValue('F',1,"",PARAM_POS,2);
					pParamPlate->SetDesignItemValue('D',pFrontSonJg->handle,"",PARAM_POS,1);
				}
				else //if(profile_side==1)
					pParamPlate->SetDesignItemValue('D',pBackSonJg->handle,"",PARAM_POS,1);
			}
			else if (pShoePlate->label.CharAt(1)=='Y')
			{	//取代下面通过quadModes的判断模式（因牛蹄板是因上面单角钢导致未赋值） wjh-2019.8.22
				double ddmx=pMasterSonJg->GetWingVecX()*tempory.ucs.axis_y;
				double ddmy=pMasterSonJg->GetWingVecY()*tempory.ucs.axis_y;
				double ddsx=pSlaveSonJg->GetWingVecX()*tempory.ucs.axis_y;
				double ddsy=pSlaveSonJg->GetWingVecY()*tempory.ucs.axis_y;
				if (max(ddmx,ddmy) < max(ddsx,ddsy))
				{	//Y轴向靴板向心（后）侧与底座板Y轴同向，取最大正值
					pBackSonJg=pSlaveSonJg;
					pFrontSonJg=pMasterSonJg;
				}
				if (profile_side==2)
				{
					pParamPlate->SetDesignItemValue('F',1,"",PARAM_POS,2);
					pParamPlate->SetDesignItemValue('D',pFrontSonJg->handle,"",PARAM_POS,1);
				}
				else //if(profile_side==1)
					pParamPlate->SetDesignItemValue('D',pBackSonJg->handle,"",PARAM_POS,1);
			}
			else if(quadModes[0]&&quadModes[2])
			{	//异型对角形式(射线角钢有的连接在基准子角钢，有的连接在其他子角钢)
				if(pShoePlate->label.CharAt(1)=='X')
				{	//X轴靴板
					double fMY_X=pMasterSonJg->GetWingVecY()*tempory.ucs.axis_x;
					double fSY_X=pSlaveSonJg->GetWingVecY()*tempory.ucs.axis_x;
					if(fMY_X<fSY_X)
					{
						if(profile_side==2)
						{
							pParamPlate->SetDesignItemValue('F',1,"",PARAM_POS,2);
							pParamPlate->SetDesignItemValue('D',hSlaveSonAngle,"",PARAM_POS,1);
						}
						else //if(profile_side==1||profile_side==0)
							pParamPlate->SetDesignItemValue('D',hMasterSonAngle,"",PARAM_POS,1);
					}
					else
					{
						if(profile_side==2)
						{
							pParamPlate->SetDesignItemValue('F',1,"",PARAM_POS,2);
							pParamPlate->SetDesignItemValue('D',hMasterSonAngle,"",PARAM_POS,1);
						}
						else //if(profile_side==1||profile_side==0)
							pParamPlate->SetDesignItemValue('D',hSlaveSonAngle,"",PARAM_POS,1);
					}
				}
				else
				{	//Y轴靴板
					double fMX_Y=pMasterSonJg->GetWingVecX()*tempory.ucs.axis_y;
					double fSX_Y=pSlaveSonJg->GetWingVecX()*tempory.ucs.axis_y;
					if(fMX_Y>fSX_Y)
					{
						if(profile_side==2)
						{
							pParamPlate->SetDesignItemValue('F',1,"",PARAM_POS,2);
							pParamPlate->SetDesignItemValue('D',hSlaveSonAngle,"",PARAM_POS,1);
						}
						else //if(profile_side==1)
							pParamPlate->SetDesignItemValue('D',hMasterSonAngle,"",PARAM_POS,1);
					}
					else
					{
						if(profile_side==2)
						{
							pParamPlate->SetDesignItemValue('F',1,"",PARAM_POS,2);
							pParamPlate->SetDesignItemValue('D',hMasterSonAngle,"",PARAM_POS,1);
						}
						else //if(profile_side==1)
							pParamPlate->SetDesignItemValue('D',hSlaveSonAngle,"",PARAM_POS,1);
					}
				}
			}
			else //if(quadModes[1]&&quadModes[3])
			{	//传统对角型式(射线角钢都与基准子角钢连接)
				if(profile_side==2)
				{
					pParamPlate->SetDesignItemValue('F',1,"",PARAM_POS,2);
					pParamPlate->SetDesignItemValue('D',hSlaveSonAngle,"",PARAM_POS,1);
				}
				else //if(profile_side==1)
					pParamPlate->SetDesignItemValue('D',hMasterSonAngle,"",PARAM_POS,1);
			}
		}
		for(CFootTempl::LINKANGLE* pLinkAngle=pShoePlate->linkAngles.GetFirst();pLinkAngle;pLinkAngle=pShoePlate->linkAngles.GetNext())
		{
			if(!pLinkAngle->cStateFlag.IsFromModel()||pLinkAngle->pAngle==NULL)
				continue;
			if(pLinkAngle->pAngle->handle==pBaseAngle->handle)
				continue;
			KEY2C key;
			if(pLinkAngle->label.CharAt(1)=='A'||pLinkAngle->label.CharAt(1)=='B')	//连接水平横材
				key=KEY2C("LH");
			else	//连接坡面斜辅材
				key=KEY2C("LX");
			if(pParamPlate->GetDesignItem(key)!=NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("0X%X shoe plate's connection angle 0X%X is too near with other angles，ignored!",pParamPlate->handle,pLinkAngle->pAngle->handle);
#else
				logerr.Log("0X%X靴板的连接角钢0X%X与其余连接材位置靠近，被系统怱略",pParamPlate->handle,pLinkAngle->pAngle->handle);
#endif
			else
			{
				pParamPlate->SetDesignItemValue(key,pLinkAngle->pAngle->handle,NULL,0,DESIGN_PARAM_ITEM::TYPE_HANDLE);	//射线角钢
				DesignRayAngle(pLinkAngle,pShoePlate);
			}
		}
		//判断靴板所在方向
		int work_wing=0;	//
		if(pShoePlate->label.CharAt(1)=='X')	//X轴向靴板
			work_wing=0;	
		else		//Y轴向靴板
			work_wing=1;
		pParamPlate->SetDesignItemValue('W',work_wing,NULL,0);	//
		//for(PROFILE_VERTEX *pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
		//	pVertex->vertex.feature = 1;
		//pShoePlate->pPlate->DesignProfile();
		//for(CLsRef* pLsRef=pShoePlate->pPlate->GetFirstLsRef();pLsRef;pLsRef=pShoePlate->pPlate->GetNextLsRef())
		//	pLsRef->GetLsPtr()->CopyModuleInstanceInfo(pShoePlate->pPlate);
	}
	//pParamPlate->SetDesignItemValue('N',pShoePlate->linkAngles.GetNodeNum(),NULL,0);
	//int i=1;
	//char lable[3]="";
	//布置基准主材螺栓
	if(upLinkSP.style_bolt0_weld1==0&&primaryAngle.pAngle)	//螺栓连接
	{
		CLDSLineAngle* pDatumAngle=primaryAngle.pAngle;
		// 获取当前基准角钢的基本信息
		if(pDatumAngle->pStart->handle==this->m_hBaseNode)
			tempory.datumRayVec=pDatumAngle->Start()-pDatumAngle->End();
		else
			tempory.datumRayVec=pDatumAngle->End()-pDatumAngle->Start();
		normalize(tempory.datumRayVec);
		tempory.wing_vec_x = pDatumAngle->GetWingVecX();
		tempory.wing_vec_y = pDatumAngle->GetWingVecY();
		tempory.pXUpFrontPlank=tempory.pXUpBackPlank=tempory.pYUpFrontPlank=tempory.pYUpBackPlank=NULL;
		tempory.pXUpBackPlank=(CLDSParamPlate*)listShoePlates[KEY4C("UXB")].pPlate;
		tempory.pYUpBackPlank=(CLDSParamPlate*)listShoePlates[KEY4C("UYB")].pPlate;
		if((pShoePlate=listShoePlates.GetValue(KEY4C("UXF")))!=NULL)
			tempory.pXUpFrontPlank=(CLDSParamPlate*)pShoePlate->pPlate;
		if((pShoePlate=listShoePlates.GetValue(KEY4C("UYF")))!=NULL)
			tempory.pYUpFrontPlank=(CLDSParamPlate*)pShoePlate->pPlate;
		SHOEPLATE* pUBShote=listShoePlates.GetValue(KEY4C("UXB"));
		if(pUBShote && pUBShote->minheight>0)
			primaryAngle.UpdateBoltInfo(pUBShote->minheight);
		LayBaseJgLs(true);
	}
	if(downLinkSP.style_bolt0_weld1==0 && primaryAngleDown.pAngle!=NULL)
	{
		tempory.pXDownFrontPlank=tempory.pXDownBackPlank=tempory.pYDownFrontPlank=tempory.pYDownBackPlank=NULL;
		if(listShoePlates.GetValue(KEY4C("DXB"))!=NULL)
			tempory.pXDownBackPlank=(CLDSParamPlate*)listShoePlates[KEY4C("DXB")].pPlate;
		else
			logerr.Log("DXB板缺失");
		if(listShoePlates.GetValue(KEY4C("DYB"))!=NULL)
			tempory.pYDownBackPlank=(CLDSParamPlate*)listShoePlates[KEY4C("DYB")].pPlate;
		else
			logerr.Log("DYB板缺失");
		if((pShoePlate=listShoePlates.GetValue(KEY4C("DXF")))!=NULL)
			tempory.pXDownFrontPlank=(CLDSParamPlate*)pShoePlate->pPlate;
		if((pShoePlate=listShoePlates.GetValue(KEY4C("DYF")))!=NULL)
			tempory.pYDownFrontPlank=(CLDSParamPlate*)pShoePlate->pPlate;
		SHOEPLATE* pDBShote=listShoePlates.GetValue(KEY4C("DXB"));
		if(pDBShote && pDBShote->minheight>0)
			primaryAngleDown.UpdateBoltInfo(pDBShote->minheight);
		LayBaseJgLs(false);
	}
	for(SHOEPLATE* pShoePlate=listShoePlates.GetFirst();pShoePlate;pShoePlate=listShoePlates.GetNext())
	{	// 3.设计与底脚板(牛蹄板)相连接的射线材位置、螺栓及正负头信息
		if(pShoePlate->cStateFlag.IsFromModel()&&pShoePlate->pPlate)
			((CLDSParamPlate*)pShoePlate->pPlate)->DesignPlate(false);
	}
	return TRUE;
}
bool CFootTempl::FindAssistPlates(RIBPLATE* pRibPlate,int bUpShoePlate,long& hAssistPlate1,long& hAssistPlate2)
{
	if(pRibPlate==NULL)
		return false;
	SHOEPLATE* pSholePlate=NULL;
	BOOL bTopFace=FALSE;
	char lablestr[4]="";
	LINKSHOEPALTE* pLinkSP=NULL;
	if(bUpShoePlate)
	{
		lablestr[0]='U';
		pLinkSP=&upLinkSP;
	}
	else
	{	
		lablestr[0]='D';
		pLinkSP=&downLinkSP;
	}
	if(pRibPlate->axisXType>0&&pRibPlate->axisXType<5)
	{
		if(pRibPlate->axisXType==1||pRibPlate->axisXType==2)		//肋板焊缝走向为底板X轴向
		{
			//确定肋板在靴板的顶面还是底面
			lablestr[1]='Y';
			lablestr[2]='B';
			if(pLinkSP->style_bolt0_weld1==1&&pRibPlate->pos_para.PARA1.OY<0)	//靴板对焊且肋板位于Y-，选择角钢楞线侧靴板
				lablestr[2]='F';
			pSholePlate=listShoePlates.GetValue(KEY4C(lablestr));
			f3dPoint baseAxisX=base.pPlate->ucs.axis_x;
			if(pSholePlate==NULL||pSholePlate->pPlate==NULL)
				return false;
			f3dPoint assistNorm=pSholePlate->pPlate->ucs.axis_z;
			if(pRibPlate->axisXType==1)		//X+
			{
				if(baseAxisX*assistNorm>0)
					bTopFace=TRUE;
				else
					bTopFace=FALSE;
			}
			else	//X-
			{
				if(baseAxisX*assistNorm>0)
					bTopFace=FALSE;
				else
					bTopFace=TRUE;
			}
		}
		else //if(pRibPlate->axisXType==3||pRibPlate->axisXType==4)	//肋板焊缝走向为底板Y轴向
		{
			//确定肋板在靴板的顶面还是底面
			lablestr[1]='X';
			lablestr[2]='B';
			if(pLinkSP->style_bolt0_weld1==1&&pRibPlate->pos_para.PARA1.OX>0)	//靴板对焊且肋板位于X+，选择角钢楞线侧靴板
				lablestr[2]='F';
			pSholePlate=listShoePlates.GetValue(KEY4C(lablestr));
			if(pSholePlate==NULL||pSholePlate->pPlate==NULL)
				return false;
			f3dPoint baseAxisY=base.pPlate->ucs.axis_y;
			f3dPoint assistNorm=pSholePlate->pPlate->ucs.axis_z;
			if(pRibPlate->axisXType==3)		//Y+
			{
				if(baseAxisY*assistNorm>0)
					bTopFace=TRUE;
				else
					bTopFace=FALSE;
			}
			else	//Y-
			{
				if(baseAxisY*assistNorm>0)
					bTopFace=FALSE;
				else
					bTopFace=TRUE;
			}
		}
		if(bTopFace)
			hAssistPlate1=pSholePlate->pPlate->handle;
		else
			hAssistPlate1=-pSholePlate->pPlate->handle;
		if(abs(hAssistPlate1)<0X20)
			return false;
	}
	else if(pRibPlate->axisXType==5)
	{
		int nPush=listRibPlates.push_stack();
		lablestr[1]='X';
		lablestr[2]='B';
		hAssistPlate1=listShoePlates.GetValue(KEY4C(lablestr))->pPlate->handle;
		lablestr[1]='Y';
		lablestr[2]='B';
		hAssistPlate2=listShoePlates.GetValue(KEY4C(lablestr))->pPlate->handle;
		long plateArr[8]={hAssistPlate1,0,hAssistPlate2,0,hAssistPlate1,0,hAssistPlate2,0};
		for(RIBPLATE* pTempPlate=listRibPlates.GetFirst();pTempPlate;pTempPlate=listRibPlates.GetNext())
		{
			if(pTempPlate->axisXType!=0)
				continue;
			if(fabs(pTempPlate->pos_para.PARA2.angle-45)<EPS)
				plateArr[1]=pTempPlate->pPlate->handle;
			else if(fabs(pTempPlate->pos_para.PARA2.angle-135)<EPS)
				plateArr[3]=pTempPlate->pPlate->handle;
			if(fabs(pTempPlate->pos_para.PARA2.angle-225)<EPS)
				plateArr[5]=pTempPlate->pPlate->handle;
			if(fabs(pTempPlate->pos_para.PARA2.angle-315)<EPS)
				plateArr[7]=pTempPlate->pPlate->handle;
		}
		listRibPlates.pop_stack(nPush);
		int iPlate=ftoi(pRibPlate->pos_para.PARA3.angle/22.5);
		hAssistPlate1=hAssistPlate2=0;
		if(iPlate>0&&iPlate<16)
		{
			int iStartPlate=(iPlate-1)/2;
			hAssistPlate1=plateArr[iStartPlate];
			hAssistPlate2=plateArr[(iStartPlate+1+8)%8];
		}
		if(abs(hAssistPlate1)<0X20||abs(hAssistPlate2)<0x20)
			return false;
	}
	else if(pRibPlate->axisXType==0)
	{	//
		lablestr[1]='X';
		lablestr[2]='B';
		hAssistPlate1=listShoePlates.GetValue(KEY4C(lablestr))->pPlate->handle;
		lablestr[1]='Y';
		lablestr[2]='B';
		hAssistPlate2=listShoePlates.GetValue(KEY4C(lablestr))->pPlate->handle;
		if(abs(hAssistPlate1)<0X20||abs(hAssistPlate2)<0x20)
			return false;
	}
	return true;
}
BOOL CFootTempl::DesignRibPlates()
{
	for(int i=0;i<2;i++)
	{
		for(RIBPLATE* pRibPlate=listRibPlates.GetFirst();pRibPlate;pRibPlate=listRibPlates.GetNext())
		{
			if(i==0&&pRibPlate->axisXType!=0)
				continue;	//先生成45度、135度、225度、315度四块肋板(圆形塔脚对角肋板),生成其他肋板时需要以对角肋板为参数
			else if(i==1&&pRibPlate->axisXType==0)
				continue;	//生成其余类型的肋板
			if(primaryAngleDown.pAngle==NULL)
				pRibPlate->upper=true;
			CLDSParamPlate* pParamPlate=(CLDSParamPlate*)Ta.Parts.append(CLS_PARAMPLATE);
			pRibPlate->pPlate=pParamPlate;
			pParamPlate->m_hPartWeldParent=base.pPlate->handle;
			pParamPlate->CopyModuleInstanceInfo(base.pPlate);
			pParamPlate->m_iParamType=TYPE_RIBPLATE;
			pParamPlate->cMaterial=pRibPlate->cMaterial;
			pParamPlate->m_cFuncType=CLDSPart::FUNC_FOOTPLATE;
			pParamPlate->thick=pRibPlate->thick;
			pParamPlate->SetPartNo(pRibPlate->sPartNo);
			pParamPlate->m_bDisplayHole=TRUE;
			SEGI iSeg;
			if(strlen(pRibPlate->sPartNo)>0&&ParsePartNo(pRibPlate->sPartNo,&iSeg,NULL))
				pParamPlate->iSeg=iSeg;
			else
				pParamPlate->iSeg=base.pPlate->iSeg;
			if(pRibPlate->upper)
				pParamPlate->SetDesignItemValue('A',base.pPlate->handle,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE);	//塔脚底板
			else
				pParamPlate->SetDesignItemValue('A',-base.pPlate->handle,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE);	//塔脚底板
			//定位参数
			if(pRibPlate->axisXType==0)
			{
				pParamPlate->SetDesignItemValue(KEY2C("Ra"),pRibPlate->pos_para.PARA2.angle,NULL,PARAM_POS);	//焊缝射线角度
				if( (fabs(pRibPlate->pos_para.PARA2.angle-45)<EPS&&quadModes[0])||
					(fabs(pRibPlate->pos_para.PARA2.angle-135)<EPS&&quadModes[1])||
					(fabs(pRibPlate->pos_para.PARA2.angle-225)<EPS&&quadModes[2])||
					(fabs(pRibPlate->pos_para.PARA2.angle-315)<EPS&&quadModes[3]))
					pParamPlate->SetDesignItemValue(KEY2C("Pa"),primaryAngle.pAngle->handle,NULL,PARAM_POS);	//肋板关联角钢
			}
			else if(pRibPlate->axisXType==5)
			{
				pParamPlate->SetDesignItemValue(KEY2C("Ra"),pRibPlate->pos_para.PARA3.angle,NULL,PARAM_POS);	//焊缝垂线角度
				pParamPlate->SetDesignItemValue(KEY2C("Rd"),pRibPlate->pos_para.PARA3.fRayDist,NULL,PARAM_POS);	//肋板射线距离
				pParamPlate->SetDesignItemValue(KEY2C("V"),primaryAngle.pAngle->handle,NULL,PARAM_POS);			//肋板Y轴方向杆件
			}
			else 
			{	
				pParamPlate->SetDesignItemValue(KEY2C("Ox"),pRibPlate->pos_para.PARA1.OX,NULL,PARAM_POS);
				pParamPlate->SetDesignItemValue(KEY2C("Oy"),pRibPlate->pos_para.PARA1.OY,NULL,PARAM_POS);
				pParamPlate->SetDesignItemValue(KEY2C("Xx"),pRibPlate->pos_para.PARA1.axisX_x,NULL,PARAM_POS);
				pParamPlate->SetDesignItemValue(KEY2C("Xy"),pRibPlate->pos_para.PARA1.axisX_y,NULL,PARAM_POS);
				//圆塔脚肋板(axisXType==0||axisType==5时)不需要设置倾斜角度 wht 16-11-01
				pParamPlate->SetDesignItemValue(KEY2C("Ga"),pRibPlate->fGradientAngle,NULL,PARAM_POS);		//旋转角度
			}
			long hAssistPlate1=0,hAssistPlate2=0;
			if(!FindAssistPlates(pRibPlate,pRibPlate->upper,hAssistPlate1,hAssistPlate2))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Foot rib plate 0x%X lost dependent shoe plate，design failed!",pParamPlate->handle);
#else
				logerr.Log("0x%X塔脚肋板未找到合理的依赖靴板，设计失败!",pParamPlate->handle);
#endif
				continue;
			}
			pParamPlate->SetDesignItemValue('C',hAssistPlate1,NULL,0,DESIGN_PARAM_ITEM::TYPE_HANDLE);
			if(abs(hAssistPlate2)>0X20)
				pParamPlate->SetDesignItemValue('D',hAssistPlate2,NULL,0,DESIGN_PARAM_ITEM::TYPE_HANDLE);
			//外形参数
			pParamPlate->SetDesignItemValue('El',1,NULL,PARAM_SHAPE,2);	//塔脚肋板侧边长度根据高度反算 wjh-2019.8.19
			pParamPlate->SetDesignItemValue('E',pRibPlate->A,NULL,PARAM_SHAPE);	//高度
			pParamPlate->SetDesignItemValue('F',pRibPlate->C,NULL,PARAM_SHAPE);	//台口高度
			pParamPlate->SetDesignItemValue('G',pRibPlate->B,NULL,PARAM_SHAPE);	//肋板长度
			pParamPlate->SetDesignItemValue('H',pRibPlate->D,NULL,PARAM_SHAPE);	//顶部开口宽度
			pParamPlate->SetDesignItemValue('I',pRibPlate->fCutAngleWidth,NULL,PARAM_SHAPE);	//切角宽度
			pParamPlate->SetDesignItemValue('e',pRibPlate->iCutMode,NULL,PARAM_SHAPE,2);
			pParamPlate->SetDesignItemValue(KEY2C("MF"),pRibPlate->MF,NULL,PARAM_SHAPE);	//孔间隙F
			pParamPlate->SetDesignItemValue(KEY2C("MG"),pRibPlate->MG,NULL,PARAM_SHAPE);	//孔间隙G
			pParamPlate->SetDesignItemValue(KEY2C("MH"),pRibPlate->MH,NULL,PARAM_SHAPE);	//内侧通孔孔径
			pParamPlate->DesignPlate();
		}
	}
	return TRUE;
}
void CFootTempl::CalBaseJgOddment(CLDSLineAngle* pBaseJg,BOOL bUpJg/*=TRUE*/)
{
	UCS_STRU ucs;
	GetUCS(ucs,false);	//此处不应该重新计算坐标原点位置,否则手动指定的坐标原点会被冲掉,导致基础根开错误 wht 16-10-14
	f3dPoint vertice,vertex_x,vertex_y,vertex_up,vertex_down,line_vec;
	f3dPoint datumPlaneOrg=ucs.origin,axisZ=ucs.axis_z;
	if(bUpJg)
		datumPlaneOrg+=ucs.axis_z*(base.nNormOffset+base.thick);
	else
		datumPlaneOrg+=ucs.axis_z*base.nNormOffset;
	C=max(0,C);
	if(pBaseJg->pStart->handle==m_hBaseNode)
	{	// 1. 根据角钢最低顶点距底座要求计算主材角钢正负头
		line_vec=pBaseJg->Start()-pBaseJg->End();
		normalize(line_vec);
		vertex_x = pBaseJg->Start()+pBaseJg->GetWingVecX()*pBaseJg->GetWidth();
		vertex_y = pBaseJg->Start()+pBaseJg->GetWingVecY()*pBaseJg->GetWidth();
		if(m_bWeldDiZuo)
		{	// 1.1主材焊接到底座
			f3dPoint ber_pos = pBaseJg->Start();
			Int3dlf(vertice,ber_pos,line_vec,datumPlaneOrg,ucs.axis_z);
			double dd=DISTANCE(vertice,ber_pos);
			if((datumPlaneOrg-ber_pos)*line_vec<0)
				dd*=-1.0;
			if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				f3dPoint inters_pt;
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pBaseJg;
				pGroupJg->m_bUpdateSonAngleOddment=FALSE;
				for(int i=0;i<4;i++)
				{
					CLDSLineAngle *pSonJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
					if(pSonJg==NULL)
						continue;
					f3dPoint son_ber_pos=pSonJg->Start();
					Int3dlf(inters_pt,son_ber_pos,line_vec,datumPlaneOrg,ucs.axis_z);
					double dd=DISTANCE(inters_pt,son_ber_pos);
					if((datumPlaneOrg-son_ber_pos)*line_vec<0)
						dd*=-1.0;
					pSonJg->SetStartOdd(dd);
				}
			}
			else
				pBaseJg->SetStartOdd(dd);

			//计算切肢量
			double ddX,ddY;
			if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pBaseJg;
				f3dPoint inters,vecx,vecy;
				double ddx,ddy;
				int nSonJg=2;
				if(pGroupJg->group_style==2)
					nSonJg=4;
				for(int i=0;i<nSonJg;i++)
				{
					CLDSLineAngle *pSonJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
					if(pSonJg)
					{
						vertex_x=pSonJg->Start()+pSonJg->GetWingVecX()*pSonJg->GetWidth();
						vertex_y=pSonJg->Start()+pSonJg->GetWingVecY()*pSonJg->GetWidth();
						Int3dlf(inters,vertex_x,line_vec,datumPlaneOrg,ucs.axis_z);
						vecx=inters-vertex_x;
						Int3dlf(inters,vertex_y,line_vec,datumPlaneOrg,ucs.axis_z);
						vecy=inters-vertex_y;
						ddx=vecx*line_vec;
						ddy=vecy*line_vec;
						if(ddx>pSonJg->startOdd())
						{
							pSonJg->cut_wing[0]=2;	//切Y肢
							pSonJg->cut_wing_para[0][0]=ddx-pSonJg->startOdd();
							pSonJg->cut_wing_para[0][1]=pSonJg->GetWidth();
							pSonJg->cut_wing_para[0][2]=ddx-ddy;
							pSonJg->SetStartOdd(ddx);
							pSonJg->m_bUserSpecStartCutAngle=TRUE;
						}
						else if(ddy>pSonJg->startOdd())
						{
							pSonJg->cut_wing[0]=1;	//切X肢
							pSonJg->cut_wing_para[0][0]=ddy-pSonJg->startOdd();
							pSonJg->cut_wing_para[0][1]=pSonJg->GetWidth();
							pSonJg->cut_wing_para[0][2]=ddy-ddx;
							pSonJg->SetStartOdd(ddy);
							pSonJg->m_bUserSpecStartCutAngle=TRUE;
						}
						else
						{
							pSonJg->cut_angle[0][0] = pSonJg->startOdd()-ddx;
							pSonJg->cut_angle[0][1] = pBaseJg->GetWidth();
							pSonJg->cut_angle[1][0] = pSonJg->startOdd()-ddy;
							pSonJg->cut_angle[1][1] = pBaseJg->GetWidth();
							pSonJg->m_bUserSpecStartCutAngle=TRUE;
						}
					}
				}
			}
			else
			{
				Int3dlf(vertex_x,vertex_x,line_vec,datumPlaneOrg,ucs.axis_z);
				Int3dlf(vertex_y,vertex_y,line_vec,datumPlaneOrg,ucs.axis_z);
				ddX = DISTANCE(vertice,vertex_x);
				ddY = DISTANCE(vertice,vertex_y);
				dd  = ddX*ddX-pBaseJg->GetWidth()*pBaseJg->GetWidth();
				if(dd>0)
				{
					pBaseJg->cut_angle[0][0] = sqrt(dd);
					pBaseJg->cut_angle[0][1] = pBaseJg->GetWidth();
					pBaseJg->m_bUserSpecStartCutAngle=TRUE;
				}
				dd  = ddY*ddY-pBaseJg->GetWidth()*pBaseJg->GetWidth();
				if(dd>0)
				{
					pBaseJg->cut_angle[1][0] = sqrt(dd);
					pBaseJg->cut_angle[1][1] = pBaseJg->GetWidth();
					pBaseJg->m_bUserSpecStartCutAngle=TRUE;
				}
			}
		}
		else
		{	// 1.2保证正负头
			if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pBaseJg;
				vertex_up=vertex_down=pBaseJg->Start();
				for(int i=0;i<4;i++)
				{
					CLDSLineAngle *pSonJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
					if(pSonJg)
					{
						vertice=pSonJg->Start();
						if(vertice.z>vertex_down.z)
							vertex_down=vertice;
						if(vertice.z<vertex_up.z)
							vertex_up=vertice;
						vertice=pSonJg->Start()+pSonJg->GetWingVecX()*pSonJg->GetWidth();
						if(vertice.z>vertex_down.z)
							vertex_down=vertice;
						if(vertice.z<vertex_up.z)
							vertex_up=vertice;
						vertice=pSonJg->Start()+pSonJg->GetWingVecY()*pSonJg->GetWidth();
						if(vertice.z>vertex_down.z)
							vertex_down=vertice;
						if(vertice.z<vertex_up.z)
							vertex_up=vertice;
					}
				}
			}
			else
			{
				if(vertex_x.z>vertex_y.z)
				{	//X肢更低
					vertex_up=vertex_y;
					vertex_down=vertex_x;
				}
				else
				{	//Y肢更低
					vertex_up=vertex_x;
					vertex_down=vertex_y;
				}
			}
			double odd;
			if(bUpJg)	//上侧基准材
				odd = -(vertex_down-datumPlaneOrg)*ucs.axis_z+C;
			else					//下侧基准材
				//double odd = vertex_up.z-datumPlaneOrgZ-C;
				odd = -(vertex_up-datumPlaneOrg)*ucs.axis_z-C;
			//pBaseJg->SetStartOdd((int)-(odd*line_vec.mod()/line_vec.z));
			pBaseJg->SetStartOdd((int)(odd*line_vec.mod()/(ucs.axis_z*line_vec)));
		}
	}
	else
	{	// 2.根据角钢最低顶点距底座要求计算主材角钢正负头
		line_vec=pBaseJg->End()-pBaseJg->Start();
		normalize(line_vec);
		vertex_x = pBaseJg->End()+pBaseJg->GetWingVecX()*pBaseJg->GetWidth();
		vertex_y = pBaseJg->End()+pBaseJg->GetWingVecY()*pBaseJg->GetWidth();
		if(m_bWeldDiZuo)
		{	// 2.1
			f3dPoint ber_pos = pBaseJg->End();
			Int3dlf(vertice,ber_pos,line_vec,datumPlaneOrg,ucs.axis_z);
			double dd=DISTANCE(vertice,ber_pos);
			if((datumPlaneOrg-ber_pos)*line_vec<0)
				dd*=-1.0;
			if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				f3dPoint inters_pt;
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pBaseJg;
				pGroupJg->m_bUpdateSonAngleOddment=FALSE;
				for(int i=0;i<4;i++)
				{
					CLDSLineAngle *pSonJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
					if(pSonJg==NULL)
						continue;
					f3dPoint son_ber_pos=pSonJg->End();
					Int3dlf(inters_pt,son_ber_pos,line_vec,datumPlaneOrg,ucs.axis_z);
					double dd=DISTANCE(inters_pt,son_ber_pos);
					if((datumPlaneOrg-son_ber_pos)*line_vec<0)
						dd*=-1.0;
					pSonJg->SetEndOdd(dd);
				}
			}
			else
				pBaseJg->SetEndOdd(ftoi(dd));
			//计算切肢量
			double ddX,ddY;
			if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				double ddx,ddy;
				f3dPoint inters,vecx,vecy;
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pBaseJg;

				int nSonJg=2;
				if(pGroupJg->group_style==2)
					nSonJg=4;
				for(int i=0;i<nSonJg;i++)
				{
					CLDSLineAngle *pSonJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
					if(pSonJg)
					{
						vertex_x=pSonJg->End()+pSonJg->GetWingVecX()*pSonJg->GetWidth();
						vertex_y=pSonJg->End()+pSonJg->GetWingVecY()*pSonJg->GetWidth();
						Int3dlf(inters,vertex_x,line_vec,datumPlaneOrg,ucs.axis_z);
						vecx=inters-vertex_x;
						Int3dlf(inters,vertex_y,line_vec,datumPlaneOrg,ucs.axis_z);
						vecy=inters-vertex_y;
						ddx=vecx*line_vec;
						ddy=vecy*line_vec;
						if(ddx>pSonJg->endOdd())
						{
							pSonJg->cut_wing[1]=2;	//切Y肢
							pSonJg->cut_wing_para[1][0]=ddx-pSonJg->endOdd();
							pSonJg->cut_wing_para[1][1]=pSonJg->GetWidth();
							pSonJg->cut_wing_para[1][2]=ddx-ddy;
							pSonJg->SetEndOdd(ddx);
							pSonJg->m_bUserSpecEndCutAngle=TRUE;
						}
						else if(ddy>pSonJg->endOdd())
						{
							pSonJg->cut_wing[1]=1;	//切X肢
							pSonJg->cut_wing_para[1][0]=ddy-pSonJg->endOdd();
							pSonJg->cut_wing_para[1][1]=pSonJg->GetWidth();
							pSonJg->cut_wing_para[1][2]=ddy-ddx;
							pSonJg->SetEndOdd(ddy);
							pSonJg->m_bUserSpecEndCutAngle=TRUE;
						}
						else
						{
							pSonJg->cut_angle[2][0] = pSonJg->endOdd()-ddx;
							pSonJg->cut_angle[2][1] = pBaseJg->GetWidth();
							pSonJg->cut_angle[3][0] = pSonJg->endOdd()-ddy;
							pSonJg->cut_angle[3][1] = pBaseJg->GetWidth();
							pSonJg->m_bUserSpecEndCutAngle=TRUE;
						}
					}
				}
			}
			else
			{
				Int3dlf(vertex_x,vertex_x,line_vec,datumPlaneOrg,ucs.axis_z);
				Int3dlf(vertex_y,vertex_y,line_vec,datumPlaneOrg,ucs.axis_z);
				ddX = DISTANCE(vertice,vertex_x);
				ddY = DISTANCE(vertice,vertex_y);
				dd  = ddX*ddX-pBaseJg->GetWidth()*pBaseJg->GetWidth();
				if(dd>0)
				{
					pBaseJg->cut_angle[2][0] = sqrt(dd);
					pBaseJg->cut_angle[2][1] = pBaseJg->GetWidth();
					pBaseJg->m_bUserSpecEndCutAngle=TRUE;
				}
				dd  = ddY*ddY-pBaseJg->GetWidth()*pBaseJg->GetWidth();
				if(dd>0)
				{
					pBaseJg->cut_angle[3][0] = sqrt(dd);
					pBaseJg->cut_angle[3][1] = pBaseJg->GetWidth();
					pBaseJg->m_bUserSpecEndCutAngle=TRUE;
				}
			}
		}
		else
		{	// 2.2
			if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pBaseJg;
				vertex_up=vertex_down=pBaseJg->End();
				int nSonJg=2;
				if(pGroupJg->group_style==2)
					nSonJg=4;
				for(int i=0;i<nSonJg;i++)
				{
					CLDSLineAngle *pSonJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
					if(pSonJg)
					{
						vertice=pSonJg->End();
						if(vertice.z>vertex_down.z)
							vertex_down=vertice;
						if(vertice.z<vertex_up.z)
							vertex_up=vertice;
						vertice=pSonJg->End()+pSonJg->GetWingVecX()*pSonJg->GetWidth();
						if(vertice.z>vertex_down.z)
							vertex_down=vertice;
						if(vertice.z<vertex_up.z)
							vertex_up=vertice;
						vertice=pSonJg->End()+pSonJg->GetWingVecY()*pSonJg->GetWidth();
						if(vertice.z>vertex_down.z)
							vertex_down=vertice;
						if(vertice.z<vertex_up.z)
							vertex_up=vertice;
					}
				}
			}
			else
			{
				if(vertex_x.z>vertex_y.z)
				{	//X肢更低
					vertex_up=vertex_y;
					vertex_down=vertex_x;
				}
				else
				{	//Y肢更低
					vertex_up=vertex_x;
					vertex_down=vertex_y;
				}
			}
			if(bUpJg)	//上侧基准材
			{
				double odd = -(vertex_down-datumPlaneOrg)*ucs.axis_z+C;
				pBaseJg->SetStartOdd((int)-(odd*line_vec.mod()/line_vec.z));
			}
			else					//下侧基准材
			{
				//double odd = vertex_up.z-datumPlaneOrgZ-C;
				double odd = -(vertex_up-datumPlaneOrg)*ucs.axis_z-C;
				pBaseJg->SetStartOdd((int)-(odd*line_vec.mod()/line_vec.z));
			}
			double odd;
			if(bUpJg)	//上侧基准材
				odd = -(vertex_down-datumPlaneOrg)*ucs.axis_z+C;
			else					//下侧基准材
				odd = -(vertex_up-datumPlaneOrg)*ucs.axis_z-C;
			//pBaseJg->SetEndOdd((int)-(odd*line_vec.mod()/line_vec.z));
			pBaseJg->SetEndOdd((int)(odd*line_vec.mod()/(line_vec*ucs.axis_z)));
		}
	}
}
BOOL CFootTempl::DesignFoot()
{
	//1、计算基准角钢的正负头及切角切肢
	if(primaryAngle.pAngle)
	{
		if(m_bWeldDiZuo)
			CalBaseJgOddment(primaryAngle.pAngle);
		if(primaryAngle.pAngle->pStart->handle==m_hBaseNode)
			primaryAngle.pAngle->SetStartOdd(primaryAngle.oddment);
		else
			primaryAngle.pAngle->SetEndOdd(primaryAngle.oddment);
	}
	if(primaryAngleDown.pAngle)
	{
		if(m_bWeldDiZuo)
			CalBaseJgOddment(primaryAngleDown.pAngle);
		if(primaryAngleDown.pAngle->pStart->handle==m_hBaseNode)
			primaryAngleDown.pAngle->SetStartOdd(primaryAngleDown.oddment);
		else
			primaryAngleDown.pAngle->SetEndOdd(primaryAngleDown.oddment);
	}
	//2、生成底座板
	if(!DesignBasePlate())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Base plates design failed!");
#else
		AfxMessageBox("塔脚底板设计失败!");
#endif
		return FALSE;
	}
	if(m_bWeldDiZuo&&base.pPlate)
	{
		if(primaryAngle.pAngle)
			primaryAngle.pAngle->m_hPartWeldParent=base.pPlate->handle;
		if(primaryAngleDown.pAngle)
			primaryAngleDown.pAngle->m_hPartWeldParent=base.pPlate->handle;
	}
	//3、生成靴板
	DesignShoePlates();
	//4、生成肋板
	DesignRibPlates();
	return TRUE;
}