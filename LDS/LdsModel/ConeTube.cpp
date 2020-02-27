
#include "stdafx.h"
#include "Createface.h"
#include "Tower.h"
//#include "env_def.h"
#ifdef __GGG_
CLDSConeTube::CLDSConeTube()
{
	size.idClassType = m_nClassTypeId = CLS_CONETUBE;
	faceN = 6;				//面数
	size.wide = 200;		//顶面外接圆半径
	size.wideUpSide = 400;  //底面外接圆半径
	size.thick = 100;       //厚度
	nNum = 1;				//塔体数量
	longth = 2000;          //塔高
	is_visible=TRUE;
	cMaterial = 'S';
	crMaterial = RGB(150,150,255);
	basePoint = f3dPoint(0,0,0);
	m_bHasWeldRoad = false;
	memset(&desStartPos,0,sizeof(CTubeEndPosPara));
	memset(&desEndPos,0,sizeof(CTubeEndPosPara));
}

CLDSConeTube::~CLDSConeTube()
{
	
}

void CLDSConeTube::CalWeldRoadVec(double fAngle)
{
	BuildUCS();
	double fRadain = fAngle*RADTODEG_COEF;
	f3dPoint tmpWeldRoadVec = ucs.axis_x;
	RotateVectorAroundVector(tmpWeldRoadVec,fRadain,ucs.axis_z);
	nearWeldRoadVec=tmpWeldRoadVec;
}

double CLDSConeTube::CalWeldRoadAngle()
{
	BuildUCS();
	f3dPoint weldRoadVec=nearWeldRoadVec;
	if(weldRoadVec.IsZero())
	{
		weldRoadVec.Set(ucs.axis_z.x,ucs.axis_z.y);
		f3dPoint verify_vec(Start().x+End().x,Start().y+End().y);
		if(weldRoadVec*verify_vec>0)
			weldRoadVec*=-1;
		if(weldRoadVec.x!=0&&weldRoadVec.y!=0)
		{	//不是X轴或Y轴向平行的杆件
			if(weldRoadVec.y<0)
				weldRoadVec.y=-fabs(weldRoadVec.x);
			else
				weldRoadVec.y=fabs(weldRoadVec.x);
		}
		nearWeldRoadVec=weldRoadVec;
	}
	normalize(weldRoadVec);
	vector_trans(weldRoadVec,ucs,FALSE);
	return Cal2dLineAng(0,0,weldRoadVec.x,weldRoadVec.y);
}

void CLDSConeTube::CalWorkPlaneNorm()
{
	if(m_iNormCalStyle!=0)
	{	//根据参照杆件计算工作面法线
		CLDSLinePart *pDatumLinePart=NULL;
		//1.指定了工作面参照杆件
		if(hWorkNormRefPart!=0)	
			pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(hWorkNormRefPart,CLS_LINEPART);
		//2.未指定工作面参照杆件时，以始端搭接杆件为工作面参照杆件
		if(pDatumLinePart==NULL&&pStart->hFatherPart>=0x20&&pStart->hFatherPart!=handle)	//始端搭接在其它杆件上
		{	
			if(pStart->hFatherPart>=0x20)
				pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(pStart->hFatherPart,CLS_LINEPART);
			else if(desStartPos.hDatumPart>=0x20)
				pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(desStartPos.hDatumPart,CLS_LINEPART);
		}
		//3.未指定工作面参照杆件且始端无搭接杆件时，以终端搭接杆件为工作面参照杆件
		if(pDatumLinePart==NULL&&pEnd->hFatherPart>=0x20&&pEnd->hFatherPart!=handle)	//终端搭接在其它杆件上
		{	
			if(pEnd->hFatherPart>=0x20)
				pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(pEnd->hFatherPart,CLS_LINEPART);
			else if(desEndPos.hDatumPart>=0x20)
				pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(desEndPos.hDatumPart,CLS_LINEPART);
		}
		if(pDatumLinePart)
		{
			workPlaneNorm=CalFaceNorm(Start(),End(),
				pDatumLinePart->Start(false),pDatumLinePart->End(false));
			normalize(workPlaneNorm);
		}
		else if(!nearWorkPlaneNorm.IsZero())
		{	//无合理工作面法线参照杆件，以近似工作面法线计算工作面法线
			f3dPoint current_vec;
			current_vec=baseline.Start()-baseline.End();
			normalize(current_vec);
			workPlaneNorm=nearWorkPlaneNorm;
			f3dPoint vec=workPlaneNorm^current_vec;
			workPlaneNorm=current_vec^vec;
			normalize(workPlaneNorm);
		}
		if(nearWorkPlaneNorm*workPlaneNorm<0)
			workPlaneNorm=-workPlaneNorm;
	}
}

UCS_STRU CLDSConeTube::BuildUCS()
{
	ucs.axis_z=baseline.End()-baseline.Start();
	ucs.origin=baseline.Start();
	if(!normalize(workPlaneNorm))
		workPlaneNorm=inters_vec(ucs.axis_z);
	ucs.axis_y=workPlaneNorm;
	normalize(ucs.axis_z);
	ucs.axis_x=ucs.axis_y^ucs.axis_z;
	normalize(ucs.axis_x);
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	return ucs;
}
void CLDSConeTube::CopyProperty(CLDSConeTube &conetube)
{
	basePoint = conetube.basePoint;
	faceN = conetube.faceN;
	longth = conetube.longth;
	size = conetube.size;
}

BOOL CLDSConeTube::Create3dSolidModel(BOOL bDisplayHole,double scale_of_user2scr, double sample_len,int smoothness)
{
	baseline.SetStart(pStart->Position(false));
	baseline.SetEnd(pEnd->Position(false));
	f3dLine vec(baseline.Start(),baseline.End());
	if(pSolidBody!=NULL&& !IsModified())
		return TRUE;
	else
	{
		if(pSolidBody == NULL)
			pSolidBody = new CSolidBody;
		else
			pSolidBody->Empty();
	}
	fBody body;
	fBody* pBody=&body;
	BuildUCS();
	f3dPoint firstPt = Start()-ucs.axis_z*startOdd() + ucs.axis_x*size.wide;
	f3dPoint firstPtInside = firstPt - ucs.axis_x*size.thick;
	f3dPoint firstPt1 = End()+ucs.axis_z*endOdd() + ucs.axis_x*size.wideUpSide;
	f3dLine vec1 = vec;
	if (smoothness != 0)
	{
		vec1.startPt += ucs.axis_x*smoothness;
		vec1.endPt += ucs.axis_x*smoothness;
		firstPt1 += ucs.axis_x*smoothness;
	}
	f3dPoint firstPtInside1 = firstPt1 - ucs.axis_x*size.thick;
	pBody->vertex.append(firstPt);
	pBody->vertex.append(firstPt1);
	pBody->vertex.append(firstPtInside);
	pBody->vertex.append(firstPtInside1);
	for (int i = 0;i<faceN;i++)
	{
		f3dPoint nextPt = firstPt;
		rotate_point_around_axis(nextPt,2*Pi/faceN,vec);
		pBody->vertex.append(nextPt);
		firstPt = nextPt;
		
		f3dPoint nextPt1 = firstPt1;
		rotate_point_around_axis(nextPt1,2*Pi/faceN,vec1);
		pBody->vertex.append(nextPt1);
		firstPt1 = nextPt1;
		
		f3dPoint nextInsidePt = firstPtInside;
		rotate_point_around_axis(nextInsidePt,2*Pi/faceN,vec);
		pBody->vertex.append(nextInsidePt);
		firstPtInside = nextInsidePt;

		f3dPoint nextInsidePt1 = firstPtInside1;
		rotate_point_around_axis(nextInsidePt1,2*Pi/faceN,vec1);
		pBody->vertex.append(nextInsidePt1);
		firstPtInside1 = nextInsidePt1;
	}
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	f3dPolyFace *pFace;
	COLORREF tube_color=crMaterial;
	
	for(i=0;i<faceN;i++)
	{
		pFace=pBody->faceList.append();
		pFace->material = tube_color;
		createFace.NewOutterEdgeLine(pFace,i*4+2,i*4);
		createFace.NewOutterEdgeLine(pFace,((i+1)%faceN)*4);
		createFace.NewOutterEdgeLine(pFace,i*4);
		pFace=pBody->faceList.append();
		pFace->material = tube_color;
		createFace.NewOutterEdgeLine(pFace,i*4+2,((i+1)%faceN)*4);
		createFace.NewOutterEdgeLine(pFace,((i+1)%faceN)*4+2);
		createFace.NewOutterEdgeLine(pFace,((i+1)%faceN)*4);
	}
	//钢管顶面创建
	for(i=0;i<faceN;i++)
	{
		pFace=pBody->faceList.append();
		pFace->material = tube_color;
		createFace.NewOutterEdgeLine(pFace,((i+1)%faceN)*4+1,i*4+1);
		createFace.NewOutterEdgeLine(pFace,i*4+3);
		createFace.NewOutterEdgeLine(pFace,i*4+1);
		pFace=pBody->faceList.append();
		pFace->material = tube_color;
		createFace.NewOutterEdgeLine(pFace,((i+1)%faceN)*4+3,((i+1)%faceN)*4+1);
		createFace.NewOutterEdgeLine(pFace,i*4+3);
		createFace.NewOutterEdgeLine(pFace,((i+1)%faceN)*4+1);
	}
	//里外侧面创建
	for (i = 0;i<faceN;i++)
	{
		pFace=pBody->faceList.append();
		pFace->material = tube_color;
		
		createFace.NewOutterEdgeLine(pFace,((i+1)%faceN)*4,i*4);
		createFace.NewOutterEdgeLine(pFace,((i+1)%faceN)*4+1);
		createFace.NewOutterEdgeLine(pFace,i*4+1);
		createFace.NewOutterEdgeLine(pFace,i*4);
	
		pFace = pBody->faceList.append();
		pFace->material = tube_color;
		createFace.NewOutterEdgeLine(pFace,i*4+2,((i+1)%faceN)*4+2);
		createFace.NewOutterEdgeLine(pFace,i*4+3);
		createFace.NewOutterEdgeLine(pFace,((i+1)%faceN)*4+3);
		createFace.NewOutterEdgeLine(pFace,((i+1)%faceN)*4+2);
	}
	pSolidBody->ConvertFrom(pBody);
	return TRUE;
}
const DWORD HASHTABLESIZE = 500;
const DWORD STATUSHASHTABLESIZE = 50;
CHashStrList<PROPLIST_ITEM> CLDSConeTube::propHashtable;
CXhChar100 CLDSConeTube::HELPLINK_URL;
CHashSet<PROPLIST_ITEM*>CLDSConeTube::hashPropItemById;
CHashTable<DWORD> CLDSConeTube::propStatusHashtable;
int CLDSConeTube::m_iCurDisplayPropGroup;
void CLDSConeTube::InitConeTubePropHashtable()
{
	int id=1;
	CLDSConeTube::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSConeTube::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	CLDSConeTube::AddPropItem("basicInfo",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("handle",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("layer",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("cMaterial",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("specification",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("iSeg",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("sPartNo",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("cfgword",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("length",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("crMaterial",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("sNotes",PROPLIST_ITEM(id++,""));

	CLDSConeTube::AddPropItem("weldRoadAngle",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("m_iWeldRoadStyle",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("nearWeldRoadVec",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("nearWeldRoadVec.x",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("nearWeldRoadVec.y",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("nearWeldRoadVec.z",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("workPlaneNorm",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("hWorkNormRefPart",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("m_iNormCalStyle",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("workPlaneNorm.x",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("workPlaneNorm.y",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("workPlaneNorm.z",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("nearWorkPlaneNorm",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("nearWorkPlaneNorm.x",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("nearWorkPlaneNorm.y",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("nearWorkPlaneNorm.z",PROPLIST_ITEM(id++,""));

	CLDSConeTube::AddPropItem("cutPlane",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("cutPlaneNorm",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("cutPlaneNorm.x",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("cutPlaneNorm.y",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("cutPlaneNorm.z",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("cutPlaneOrigin",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("cutPlaneOrigin.x",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("cutPlaneOrigin.y",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("cutPlaneOrigin.z",PROPLIST_ITEM(id++,""));

	CLDSConeTube::AddPropItem("startInfo",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("hStart",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("oddStart",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("jointStart",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("startPos",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("startPos.x",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("startPos.y",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("startPos.z",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desStartPos",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desStartPos.endPosType",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desStartPos.datum_pos_style",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desStartPos.hDatumPart",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desStartPos.fNormOffset",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desStartPos.fEccentricDist",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desStartPos.face_offset_norm",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desStartPos.hDatumStartPart",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desStartPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desStartPos.hDatumEndPart",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desStartPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("endInfo",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("hEnd",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("oddEnd",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("jointEnd",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("endPos",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("endPos.x",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("endPos.y",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("endPos.z",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desEndPos",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desEndPos.endPosType",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desEndPos.datum_pos_style",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desEndPos.hDatumPart",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desEndPos.fNormOffset",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desEndPos.fEccentricDist",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desEndPos.face_offset_norm",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desEndPos.hDatumStartPart",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desEndPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desEndPos.hDatumEndPart",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("desEndPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("m_bHasWeldRoad",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("startWorkPlane",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("endWorkPlane",PROPLIST_ITEM(id++,""));
	//内力分析所需要的属性
	CLDSConeTube::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,""));	//统材编号
	CLDSConeTube::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,""));
	//设计信息
	CLDSConeTube::AddPropItem("designInfo",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("hole_out",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,""));
		//计算长度信息
	CLDSConeTube::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,""));
		//始端设计信息
	CLDSConeTube::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("start_force_type",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("start_ls.bearloadcoef",PROPLIST_ITEM(id++,""));
		//终端设计信息
	CLDSConeTube::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("end_force_type",PROPLIST_ITEM(id++,""));
	CLDSConeTube::AddPropItem("end_ls.bearloadcoef",PROPLIST_ITEM(id++,""));
}

int CLDSConeTube::GetPropValueStr(long id,char* valueStr)
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
	else if(GetPropID("specification")==id)
	{
		char s_d[200],s_t[200];
		sprintf(s_d,"%f",size.wide);
		sprintf(s_t,"%f",size.wideUpSide);
		SimplifiedNumString(s_d);
		SimplifiedNumString(s_t);
		sprintf(sText,"%sX%s", s_d,s_t);
	}
	else if(GetPropID("iSeg")==id)
		strcpy(sText,(char*)iSeg.ToString());
	else if(GetPropID("sPartNo")==id)
		sprintf(sText,"%s",(char*)sPartNo);
	else if(GetPropID("cfgword")==id)
	{
		if(layer(0)=='L')	//接腿构件
			return cfgword.GetLegScopeStr(valueStr,50,true);
		else
			return cfgword.GetBodyScopeStr(valueStr);
	}
	else if(GetPropID("m_uStatMatCoef")==id)
		sprintf(sText,"%d",m_uStatMatCoef);
	else if(GetPropID("m_hPartWeldParent")==id)
		sprintf(sText,"0X%X",m_hPartWeldParent);
	else if(GetPropID("m_bDisplayHole")==id)
	{
		if(m_bDisplayHole)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
	}
	else if(GetPropID("m_bVirtualPart")==id)
	{
		if(m_bVirtualPart)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
	}
	else if(GetPropID("length")==id)
	{
		sprintf(sText,"%f",GetLength());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_bUserSpecColor")==id)
	{
		if(m_bUserSpecColor)
			sprintf(sText,"是");
		else
			sprintf(sText,"否");
	}
	else if(GetPropID("crMaterial")==id)
		sprintf(sText,"RGB%X",crMaterial);
	else if(GetPropID("sNotes")==id)
		strcpy(sText,sNotes);
	else if(GetPropID("m_bHasWeldRoad")==id)
	{
		if(m_bHasWeldRoad)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
	}
	else if(GetPropID("nearWeldRoadVec.x")==id)
	{
		sprintf(sText,"%f",nearWeldRoadVec.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("weldRoadAngle")==id)
	{
		//返回值为弧度需要转为角度并且在0-360度之间
		double fAngle = this->CalWeldRoadAngle()*DEGTORAD_COEF;
		int iTmp=0;
		if(fAngle>0)
		{
			iTmp=(int)fAngle/360;
			fAngle = fAngle-iTmp*360;
		}
		else if(fAngle<0)
		{
			iTmp=(int)fAngle/360;
			fAngle = fAngle-iTmp*360+360;
		}
		sprintf(sText,"%f",fAngle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("nearWeldRoadVec.y")==id)
	{
		sprintf(sText,"%f",nearWeldRoadVec.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_iWeldRoadStyle")==id)
	{
		if(m_iWeldRoadStyle==0)
			strcpy(sText,"0.直接指定");
		else //if (m_iWeldRoadStyle==1)
			strcpy(sText,"1.角度定位");
	}
	else if(GetPropID("nearWeldRoadVec.y")==id)
	{
		sprintf(sText,"%f",nearWeldRoadVec.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("nearWeldRoadVec.z")==id)
	{
		sprintf(sText,"%f",nearWeldRoadVec.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_iNormCalStyle")==id)
	{
		if(m_iNormCalStyle==0)
			strcpy(sText,"0.直接指定");
		else
			strcpy(sText,"1.参照杆件");
	}
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
	else if(GetPropID("cutPlaneNorm.x")==id)
	{
		sprintf(sText,"%f",cutPlaneNorm.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cutPlaneNorm.y")==id)
	{
		sprintf(sText,"%f",cutPlaneNorm.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cutPlaneNorm.z")==id)
	{
		sprintf(sText,"%f",cutPlaneNorm.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cutPlaneOrigin.x")==id)
	{
		sprintf(sText,"%f",cutPlaneOrigin.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cutPlaneOrigin.y")==id)
	{
		sprintf(sText,"%f",cutPlaneOrigin.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cutPlaneOrigin.z")==id)
	{
		sprintf(sText,"%f",cutPlaneOrigin.z);
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
	else if(GetPropID("jointStart")==id)
	{
		if(m_tJointStart.type==1)
			sprintf(sText,"套接");
		else
			sprintf(sText,"无连接");
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
			strcpy(sText,"节点即基点");
		else if(desStartPos.endPosType==1)
			strcpy(sText,"搭接连接");
		else if(desStartPos.endPosType==2)
			strcpy(sText,"点线投影");
	}
	else if(GetPropID("desStartPos.datum_pos_style")==id)
	{
		if(desStartPos.datum_pos_style==1)
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
	else if(GetPropID("desStartPos.hDatumStartPart")==id)
		sprintf(sText,"0X%X", desStartPos.hDatumStartPart); 
	else if(GetPropID("desStartPos.bDatumStartStartEnd")==id)
	{
		if(desStartPos.bDatumStartStartEnd)
			strcpy(sText,"始端");
		else //if(!desStartPos.bDatumStartStartEnd)
			strcpy(sText,"终端");
	}
	else if(GetPropID("desStartPos.hDatumEndPart")==id)
		sprintf(sText,"0X%X", desStartPos.hDatumEndPart); 
	else if(GetPropID("desStartPos.bDatumEndStartEnd")==id)
	{
		if(desStartPos.bDatumEndStartEnd)
			strcpy(sText,"始端");
		else //if(!desStartPos.bDatumEndStartEnd)
			strcpy(sText,"终端");
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
	else if(GetPropID("jointEnd")==id)
	{
		if(m_tJointEnd.type==1)
			sprintf(sText,"套接");
		else
			sprintf(sText,"无连接");
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
			strcpy(sText,"节点即基点");
		else if(desEndPos.endPosType==1)
			strcpy(sText,"搭接连接");
		else if(desEndPos.endPosType==2)
			strcpy(sText,"点线投影");
	}
	else if(GetPropID("desEndPos.datum_pos_style")==id)
	{
		if(desEndPos.datum_pos_style==0)
			strcpy(sText,"向楞线投影");
		else if(desEndPos.datum_pos_style==1)
			strcpy(sText,"X肢心线投影");
		else if(desEndPos.datum_pos_style==2)
			strcpy(sText,"Y肢心线投影");
		else if(desEndPos.datum_pos_style==3)
			strcpy(sText,"向楞线平推");
		else if(desEndPos.datum_pos_style==4)
			strcpy(sText,"X肢心线平推");
		else if(desEndPos.datum_pos_style==5)
			strcpy(sText,"Y肢心线平推");
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
	else if(GetPropID("desEndPos.hDatumStartPart")==id)
		sprintf(sText,"0X%X", desEndPos.hDatumStartPart); 
	else if(GetPropID("desEndPos.bDatumStartStartEnd")==id)
	{
		if(desEndPos.bDatumStartStartEnd)
			strcpy(sText,"始端");
		else //if(!desEndPos.bDatumStartStartEnd)
			strcpy(sText,"终端");
	}
	else if(GetPropID("desEndPos.hDatumEndPart")==id)
		sprintf(sText,"0X%X", desEndPos.hDatumEndPart); 
	else if(GetPropID("desEndPos.bDatumEndStartEnd")==id)
	{
		if(desEndPos.bDatumEndStartEnd)
			strcpy(sText,"始端");
		else //if(!desEndPos.bDatumEndStartEnd)
			strcpy(sText,"终端");
	}
#ifdef __PART_DESIGN_INC_
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
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
	}
	else if(GetPropID("m_bNeedSelSize")==id)
	{
		if(m_bNeedSelSize)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
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
			strcpy(sText,"5.指定计算长度");
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
	/*else if(GetPropID("CalLenCoef.spec_callenMinR")==id)
	{
		sprintf(sText,"%f",CalLenCoef.spec_callenMinR);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("CalLenCoef.spec_callenParaR")==id)
	{
		sprintf(sText,"%f",CalLenCoef.spec_callenParaR);
		SimplifiedNumString(sText);
	}*/
	else if(GetPropID("start_joint_type")==id)
	{
		if(start_joint_type==JOINT_HINGE)
			strcpy(sText,"铰接");
		else if(start_joint_type==JOINT_RIGID)
			strcpy(sText,"刚接");
		else //if(start_joint_type==JOINT_AUTOJUSTIFY)
			strcpy(sText,"自动判断");
	}
	else if(GetPropID("start_force_type")==id)
	{
		if(start_force_type==CENTRIC_FORCE)
			strcpy(sText,"中心受力");
		else //if(start_force_type==ECCENTRIC_FORCE)
			strcpy(sText,"偏心受力");
	}
	/*else if(GetPropID("start_ls.grade")==id)
		strcpy(sText,start_ls.grade);
	else if(GetPropID("start_ls.d")==id)
		sprintf(sText,"M%d",start_ls.d);
	else if(GetPropID("start_ls.N")==id)
		sprintf(sText,"%d",start_ls.N);
	else if(GetPropID("start_ls.bearloadcoef")==id)
	{
		sprintf(sText,"%f",start_ls.bearloadcoef);
		SimplifiedNumString(sText);
	}*/
	else if(GetPropID("end_joint_type")==id)
	{
		if(end_joint_type==JOINT_HINGE)
			strcpy(sText,"铰接");
		else if(end_joint_type==JOINT_RIGID)
			strcpy(sText,"刚接");
		else //if(end_joint_type==JOINT_AUTOJUSTIFY)
			strcpy(sText,"自动判断");
	}
	else if(GetPropID("end_force_type")==id)
	{
		if(end_force_type==CENTRIC_FORCE)
			strcpy(sText,"中心受力");
		else //if(end_force_type==ECCENTRIC_FORCE)
			strcpy(sText,"偏心受力");
	}
	/*else if(GetPropID("end_ls.grade")==id)
		strcpy(sText,end_ls.grade);
	else if(GetPropID("end_ls.d")==id)
		sprintf(sText,"M%d",end_ls.d);
	else if(GetPropID("end_ls.N")==id)
		sprintf(sText,"%d",end_ls.N);
	else if(GetPropID("end_ls.bearloadcoef")==id)
	{
		sprintf(sText,"%f",end_ls.bearloadcoef);
		SimplifiedNumString(sText);
	}*/
	else if(GetPropID("m_bAutoCalHoleOutCoef")==id)
	{
		if(m_bAutoCalHoleOutCoef)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
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
long CLDSConeTube::GetPropID(char* propStr)
{
	PROPLIST_ITEM *pItem = CLDSConeTube::propHashtable.GetValue(propStr);
	if(pItem)
		return pItem->id;
	else
		return 0;
}

BOOL CLDSConeTube::IsPropEqual(HASHOBJSET &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSConeTube *pLineTube=NULL, *pFirstLineTube = (CLDSConeTube*)selectObjs.GetFirst();
	pFirstLineTube->GetPropValueStr(idProp,valueStr);
	for(pLineTube=(CLDSConeTube*)selectObjs.GetNext();pLineTube;pLineTube=(CLDSConeTube*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pLineTube->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

int CLDSConeTube::GetPropStatus(long id)
{	
	DWORD dwStatus;
	if(CLDSConeTube::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
}

BOOL CLDSConeTube::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSConeTube::propStatusHashtable.SetValueAt(id,dwStatus);
}
#endif