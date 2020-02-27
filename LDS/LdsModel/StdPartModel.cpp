//<LOCALE_TRANSLATE BY hxr /> 2015-04-23
#include "stdafx.h"
#include "stdio.h"
#include "Tower.h"
#include "CreateFace.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

CLDSStdPartModel::CLDSStdPartModel()
{
	//strcpy(guige,"");
	m_iStdPartType=TYPE_FLD;
	//LoadDefaultUCS(&ucs);
	m_fRelateTubeThick=0;
	memset(&param.fl_param,0,sizeof(param.fl_param));
	memset(&param.insert_plate_param,0,sizeof(param.insert_plate_param));
}
CLDSStdPartModel::~CLDSStdPartModel()
{
}

void CLDSStdPartModel::CreateLsHole(fBody* pBody,f3dPolyFace *pTopFace,f3dPolyFace *pBottomFace,f3dPoint norm)
{
	CHoleMaker holeMaker;
	UCS_STRU hole_ucs;
	int iBoltD=0;
	double fHoleD=0;
	if(m_iStdPartType==TYPE_FLD||m_iStdPartType==TYPE_FLP)
	{
		iBoltD=param.fl_param.nBoltD;
		fHoleD=param.fl_param.L;
	}
	else if(m_iStdPartType==TYPE_FLG||m_iStdPartType==TYPE_FLR)
	{
		iBoltD=param.fl_param2.nBoltD;
		fHoleD=param.fl_param2.fBoltHoleD;
	}
	else
	{
		iBoltD=param.insert_plate_param.ls_param[0].iBoltD;
		fHoleD=iBoltD+2;
	}
	for(f3dPoint *pPt=boltPosList.GetFirst();pPt;pPt=boltPosList.GetNext())
	{
		hole_ucs.origin=*pPt;
		if(m_iStdPartType==TYPE_XEND)
		{	//十字插板上的螺栓孔
			if(fabs(norm*f3dPoint(0,0,1))>EPS_COS)
			{	//跳过竖向板上的螺栓孔
				if(pPt->feature==31||pPt->feature==41)
					continue;
			}
			else
			{	//跳过横向板上的螺栓孔
				if(pPt->feature!=31&&pPt->feature!=41)
					continue;
			}
			if(pPt->x>param.insert_plate_param.L)
				continue; //不生成十字插板范围外的螺栓孔
		}
		hole_ucs.axis_z=norm;
		hole_ucs.axis_x=inters_vec(hole_ucs.axis_z);
		hole_ucs.axis_y=hole_ucs.axis_z^hole_ucs.axis_x;
		hole_ucs.axis_x=hole_ucs.axis_y^hole_ucs.axis_z;
		COLORREF crLsHole=CLDSPart::GetColor(CLS_BOLT,iBoltD,' ');
		holeMaker.Create(hole_ucs,fHoleD,0,pTopFace,pBottomFace,pBody,crLsHole);
	}
}

char* CLDSStdPartModel::GetPartNo()
{
	if(m_iStdPartType==TYPE_FLD||m_iStdPartType==TYPE_FLP)
		return param.fl_param.codeName;
	else if(m_iStdPartType==TYPE_FLR||m_iStdPartType==TYPE_FLG)
		return param.fl_param2.codeName;
	else
		return param.insert_plate_param.codeName;
}

//根据输入的第一个螺栓的位置,以及矩阵(行X列),行/列间距布置螺栓
void CLDSStdPartModel::LayoutBolt()
{
	int iLayoutBolt,iBoltNum;
	if(boltPosList.GetNodeNum()>0)
		return;	//螺栓已布置
	if(m_iStdPartType==TYPE_FLD||m_iStdPartType==TYPE_FLP)
	{	//环向布置螺栓
		iLayoutBolt=1;
		iBoltNum=param.fl_param.nBoltNum;
		if(iBoltNum==0)
			return;
	}
	else if(m_iStdPartType==TYPE_FLG||m_iStdPartType==TYPE_FLR)
	{
		iLayoutBolt=1;
		iBoltNum=param.fl_param2.nBoltNum;
		if(iBoltNum==0)
			return;
	}
	else if(m_iStdPartType==TYPE_XEND||m_iStdPartType==TYPE_UEND||m_iStdPartType==TYPE_ROLLEND)
	{	//矩阵布置螺栓
		iLayoutBolt=param.insert_plate_param.iBoltRect;
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("Can't automatic lay bolts on wrong standard part type.");
#else
		logerr.Log("无法在错误的标准件类型上自动布置螺栓");
#endif
		return;
	}
	for(int iBoltRect=0;iBoltRect<iLayoutBolt;iBoltRect++)
	{
		if((m_iStdPartType!=TYPE_FLD&&m_iStdPartType!=TYPE_FLP&&	//环向布置螺栓时不需要判断行数或列数 wht 11-05-26
			m_iStdPartType!=TYPE_FLG&&m_iStdPartType!=TYPE_FLR)&&
			(param.insert_plate_param.ls_param[iBoltRect].iRow==0
			||param.insert_plate_param.ls_param[iBoltRect].iColumn==0))
			continue; //行数或列数为0
		int i,iRow,d;
		double fHorzSpace,fBoltPosX,fVertSpace,fBoltPosY;
		if(m_iStdPartType==TYPE_ROLLEND||m_iStdPartType==TYPE_UEND)
		{	//矩阵布置螺栓
			iRow=param.insert_plate_param.ls_param[iBoltRect].iRow;
			iBoltNum=param.insert_plate_param.ls_param[iBoltRect].iBoltNum;
			d=param.insert_plate_param.ls_param[iBoltRect].iBoltD;
			fHorzSpace=param.insert_plate_param.ls_param[iBoltRect].fHorzSpace;
			fVertSpace=param.insert_plate_param.ls_param[iBoltRect].fVertSpace;
			fBoltPosX=param.insert_plate_param.ls_param[iBoltRect].fBoltPosX;
			fBoltPosY=param.insert_plate_param.ls_param[iBoltRect].fBoltPosY;
			for(i=0;i<iBoltNum;i++)
			{
				f3dPoint *pPt=boltPosList.append();
				pPt->x = fBoltPosX+fHorzSpace*(i/iRow);
				pPt->y = fBoltPosY+fVertSpace*(i%iRow);
				pPt->z = 0;
			}
			int nLast = iBoltNum%iRow;
			if(iBoltNum>0&&nLast>0)	//最后一个螺栓位于中线上(且每列布置螺栓数应该为偶数)
			{
				int i=0;
				for (f3dPoint *pPt = boltPosList.GetTail();i<nLast;i++,pPt = boltPosList.GetPrev())
					pPt->y += 0.5*(iRow-nLast)*fVertSpace;
			}
		}
		else if(m_iStdPartType==TYPE_XEND)
		{	//矩阵布置螺栓
			//if(iBoltRect>0)
			//	continue;
			iRow=param.insert_plate_param.ls_param[iBoltRect].iRow;
			iBoltNum=param.insert_plate_param.ls_param[iBoltRect].iBoltNum;
			d=param.insert_plate_param.ls_param[iBoltRect].iBoltD;
			fHorzSpace=param.insert_plate_param.ls_param[iBoltRect].fHorzSpace;
			fVertSpace=param.insert_plate_param.ls_param[iBoltRect].fVertSpace;
			fBoltPosX=param.insert_plate_param.ls_param[iBoltRect].fBoltPosX+param.insert_plate_param.A+param.insert_plate_param.M;
			fBoltPosY=param.insert_plate_param.ls_param[iBoltRect].fBoltPosY+param.insert_plate_param.R;
			//十字型插板存储布置螺栓参数时,如果第一组布置螺栓参数的行数列数均为-1，
			//则表示该十字插板上的螺栓为双排螺栓,且竖直面和水平面呈交错布置 wht 09-06-06
			BOOL bStaggerLayout=FALSE; //是否交错布置十字插板上的螺栓
			//读取十字插板数据库时，行数由-1变为1  原因还未找到.......
			if(param.insert_plate_param.ls_param[0].iColumn==-1||param.insert_plate_param.ls_param[0].iRow==-1)
				bStaggerLayout=TRUE;	
			//double Y=0.5*insert_plate_param.H-(insert_plate_param.N-(0.5*insert_plate_param.H-fBoltPosY));
			//布置十字插板螺栓时用户输入的为相对包板的坐标此处需要将相对包板的坐标转换到十字插板坐标系下
			//水平面与竖直面交错布置时的螺栓Y坐标与H无关 wht 10-01-22
			double Y=param.insert_plate_param.R+(param.insert_plate_param.N-param.insert_plate_param.ls_param[iBoltRect].fBoltPosY);
			for(i=0;i<iBoltNum;i++)
			{
				f3dPoint *pPt=boltPosList.append();
				pPt->x = fBoltPosX+fHorzSpace*(i/abs(iRow));
				pPt->y =  fBoltPosY+fVertSpace*(i%abs(iRow));
				pPt->z = 0;
				pPt->feature=11;//X-Y平面: Y正方向一侧的螺栓
				pPt=boltPosList.append();
				pPt->x = fBoltPosX+fHorzSpace*(i/abs(iRow));
				pPt->y = -( fBoltPosY+fVertSpace*(i%abs(iRow)));
				pPt->z = 0;
				pPt->feature=21;//X-Y平面: Y负方向一侧的螺栓
				if(bStaggerLayout)
				{	//包板上螺栓为双排时十字插板水平面和竖直面交错布置螺栓
					pPt=boltPosList.append();
					pPt->x = fBoltPosX+fHorzSpace*(i/abs(iRow));
					pPt->y = 0;
					pPt->z = Y;
					pPt->feature=31;//X-Z平面: Z正方向一侧的螺栓
					pPt=boltPosList.append();
					pPt->x = fBoltPosX+fHorzSpace*(i/abs(iRow));
					pPt->y = 0;
					pPt->z = -Y;
					pPt->feature=41;//X-Z平面: Z负方向一侧的螺栓
				}
				else
				{
					pPt=boltPosList.append();
					pPt->x = fBoltPosX+fHorzSpace*(i/abs(iRow));
					pPt->y = 0;
					pPt->z = fBoltPosY+fVertSpace*(i%abs(iRow));
					pPt->feature=31;//X-Z平面: Z正方向一侧的螺栓
					pPt=boltPosList.append();
					pPt->x = fBoltPosX+fHorzSpace*(i/abs(iRow));
					pPt->y = 0;
					pPt->z = -(fBoltPosY+fVertSpace*(i%abs(iRow)));
					pPt->feature=41;//X-Z平面: Z负方向一侧的螺栓
				}
			}
		}
		else if(m_iStdPartType==TYPE_FLD||m_iStdPartType==TYPE_FLP)
		{	//环向布置螺栓
			double piece_angle=2*Pi/iBoltNum;
			for(i=0;i<iBoltNum;i++)
			{
				double angle=i*piece_angle;
				f3dPoint *pPt=boltPosList.append();
				pPt->x = 0.5*param.fl_param.K*cos(angle);
				pPt->y = 0.5*param.fl_param.K*sin(angle);
				pPt->z = 0;
			}
		}
		else if(m_iStdPartType==TYPE_FLG||m_iStdPartType==TYPE_FLR)
		{	//环向布置螺栓
			double piece_angle=2*Pi/iBoltNum;
			for(i=0;i<iBoltNum;i++)
			{
				double angle=i*piece_angle;
				f3dPoint *pPt=boltPosList.append();
				pPt->x = 0.5*param.fl_param2.fBoltCirD*cos(angle);
				pPt->y = 0.5*param.fl_param2.fBoltCirD*sin(angle);
				pPt->z = 0;
			}
		}
	}
}
typedef f3dPoint* f3dPointPtr;
BOOL CLDSStdPartModel::Create3dSolidModel(fBody *pBody,BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/,
										  double sample_len/*=5.0*/,int smoothness/*=36*/,BYTE cRollProcessType/*=0xFF*/)
{
 	if(pBody==NULL)
		return FALSE;
	else
		pBody->Empty();
	f3dPoint vertice;
	f3dPolyFace *pFace=NULL;
	int i=0,j=0;
	LayoutBolt();	//布置螺栓
	char cMaterial='S';	//材质
	if(m_iStdPartType==TYPE_FLD||m_iStdPartType==TYPE_FLP)
		cMaterial=param.fl_param.cMaterial;
	else if(m_iStdPartType==TYPE_FLR||m_iStdPartType==TYPE_FLG)
		cMaterial=param.fl_param2.cMaterial;
	else
		cMaterial=param.insert_plate_param.cMaterial;
	COLORREF crStdPart=CLDSPart::GetColor(CLS_PARAMPLATE,'S',cMaterial);
	if(m_iStdPartType==TYPE_FLD)
	{
		//设置构件各个顶点的坐标
		int uSlices = CalArcResolution(0.5*param.fl_param.D,2*Pi,scale_of_user2scr,sample_len,smoothness);
		param.fl_param.B=param.fl_param.A-(param.fl_param.S+m_fRelateTubeThick)*2;	//新标准中法兰内径非独立自由参数 wjh-2011.11.22
		double coshalf_alfa = cos(Pi/uSlices);	//半角余弦
		double outter_radius=0.5*param.fl_param.D/coshalf_alfa;//外环半径
		double scaleN=0.5*param.fl_param.N/outter_radius;
		double scaleS=(0.5*param.fl_param.B+m_fRelateTubeThick+param.fl_param.S)/outter_radius;
		double scaleB=0.5*param.fl_param.B/outter_radius;
		uSlices = min(uSlices,200);
		f3dPoint tmpPoint;
		f3dPoint vertex_arr[200];
		CLDSPart::GetCircleSimuPolyVertex(outter_radius,vertex_arr,uSlices);
		for(i =0;i<uSlices;i++)
		{
			tmpPoint=vertex_arr[i];
			tmpPoint.z=0;
			pBody->vertex.append(tmpPoint);
			tmpPoint.z=param.fl_param.C;
			pBody->vertex.append(tmpPoint);
			tmpPoint=vertex_arr[i];
			tmpPoint*=scaleN;
			tmpPoint.z=param.fl_param.C;
			pBody->vertex.append(tmpPoint);
			tmpPoint=vertex_arr[i];
			tmpPoint*=scaleS;
			tmpPoint.z=param.fl_param.H-param.fl_param.H1;//param.fl_param.C+param.fl_param.R;
			pBody->vertex.append(tmpPoint);
			tmpPoint=vertex_arr[i];
			tmpPoint*=scaleS;
			tmpPoint.z=param.fl_param.H;
			pBody->vertex.append(tmpPoint);
			tmpPoint=vertex_arr[i];
			tmpPoint*=scaleB;
			tmpPoint.z=param.fl_param.H;
			pBody->vertex.append(tmpPoint);
			tmpPoint.z=0;
			pBody->vertex.append(tmpPoint);
			
		}
		//初始化链表
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		//底面
		f3dPolyFace *pFLDownFace=pBody->faceList.append();
		pFLDownFace->poly_norm.Set(0,0,-1);
		pFLDownFace->material = crStdPart;
		fLoop *pInnerLoop=pFLDownFace->inner_loop.append();
		for(i=0;i<uSlices;i++)
		{
			createFace.NewOutterEdgeLine(pFLDownFace,((uSlices-i-1)%uSlices)*7,(uSlices-i)%uSlices*7);
			createFace.NewInnerLoopEdgeLine(pInnerLoop,(i+1)%uSlices*7+6,i*7+6);
		}
		//顶面
		f3dPolyFace *pFLUpFace=pBody->faceList.append();
		pFLUpFace->poly_norm.Set(0,0,1);
		pFLUpFace->material=crStdPart;
		pInnerLoop=pFLUpFace->inner_loop.append();
		for(i=0;i<uSlices;i++)
		{
			createFace.NewOutterEdgeLine(pFLUpFace,i*7+1,((i+1)%uSlices)*7+1);
			createFace.NewInnerLoopEdgeLine(pInnerLoop,((i+1)%uSlices)*7+2,i*7+2);
		}
		//颈面
		f3dPolyFace *pNeckFlatFace=pBody->faceList.append();
		pNeckFlatFace->poly_norm.Set(0,0,1);
		pNeckFlatFace->material=crStdPart;
		pInnerLoop=pNeckFlatFace->inner_loop.append();
		for(i=0;i<uSlices;i++)
		{
			createFace.NewOutterEdgeLine(pNeckFlatFace,i*7+4,((i+1)%uSlices)*7+4);
			createFace.NewInnerLoopEdgeLine(pInnerLoop,((i+1)%uSlices)*7+5,i*7+5);
		}
		//侧面
		for(i=0;i<uSlices;i++)
		{
			pFace=pBody->faceList.append();
			pFace->material = crStdPart;
			createFace.NewOutterEdgeLine(pFace,((i+1)%uSlices)*7,i*7);
			createFace.NewOutterEdgeLine(pFace,((i+1)%uSlices)*7+1);
			createFace.NewOutterEdgeLine(pFace,i*7+1);
			createFace.NewOutterEdgeLine(pFace,i*7);
			
			pFace=pBody->faceList.append();
			pFace->material = crStdPart;
			createFace.NewOutterEdgeLine(pFace,((i+1)%uSlices)*7+2,i*7+2);
			createFace.NewOutterEdgeLine(pFace,((i+1)%uSlices)*7+3);
			createFace.NewOutterEdgeLine(pFace,i*7+3);
			createFace.NewOutterEdgeLine(pFace,i*7+2);
			
			pFace=pBody->faceList.append();
			pFace->material = crStdPart;
			createFace.NewOutterEdgeLine(pFace,((i+1)%uSlices)*7+3,i*7+3);
			createFace.NewOutterEdgeLine(pFace,((i+1)%uSlices)*7+4);
			createFace.NewOutterEdgeLine(pFace,i*7+4);
			createFace.NewOutterEdgeLine(pFace,i*7+3);
			
			pFace=pBody->faceList.append();
			pFace->material = crStdPart;
			createFace.NewOutterEdgeLine(pFace,((i+1)%uSlices)*7+5,i*7+5);
			createFace.NewOutterEdgeLine(pFace,((i+1)%uSlices)*7+6);
			createFace.NewOutterEdgeLine(pFace,i*7+6);
			createFace.NewOutterEdgeLine(pFace,i*7+5);
		}
		if(bDisplayHole)
			CreateLsHole(pBody,pFLUpFace,pFLDownFace,f3dPoint(0,0,1));//定义螺栓孔
	}
	else if(m_iStdPartType==TYPE_FLP)
	{
		//设置构件各个顶点的坐标
		int uSlices = CalArcResolution(0.5*param.fl_param.D,2*Pi,scale_of_user2scr,sample_len,smoothness);
		double coshalf_alfa = cos(Pi/uSlices);	//半角余弦
		double outter_radius=0.5*param.fl_param.D/coshalf_alfa;//外环半径
		double scaleN=0.5*param.fl_param.N/outter_radius;
		double scaleS=(0.5*param.fl_param.B+param.fl_param.S)/outter_radius;
		double scaleB=0.5*param.fl_param.B/outter_radius;
		uSlices = min(uSlices,200);
		f3dPoint tmpPoint;
		f3dPoint vertex_arr[200];
		CLDSPart::GetCircleSimuPolyVertex(outter_radius,vertex_arr,uSlices);
		for(i=0;i<uSlices;i++)
		{
			tmpPoint=vertex_arr[i];
			tmpPoint.z=0;
			pBody->vertex.append(tmpPoint);//7*i
			tmpPoint.z=param.fl_param.C;
			pBody->vertex.append(tmpPoint);//7*i+1
			tmpPoint*=scaleN;
			tmpPoint.z=param.fl_param.C;
			pBody->vertex.append(tmpPoint);//7*i+2
			tmpPoint=vertex_arr[i];
			tmpPoint*=scaleS;
			tmpPoint.z=param.fl_param.R+param.fl_param.C;
			pBody->vertex.append(tmpPoint);//7*i+3
			tmpPoint.z=param.fl_param.H;
			pBody->vertex.append(tmpPoint);//7*i+4
			tmpPoint=vertex_arr[i];
			tmpPoint*=scaleB;
			tmpPoint.z=param.fl_param.H;
			pBody->vertex.append(tmpPoint);//7*i+5
			tmpPoint.z=0;
			pBody->vertex.append(tmpPoint);//7*i+6
		}
		//初始化链表
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		//底面
		f3dPolyFace *pFLDownFace=pBody->faceList.append();
		pFLDownFace->poly_norm.Set(0,0,-1);
		pFLDownFace->material = crStdPart;
		fLoop *pInnerLoop=pFLDownFace->inner_loop.append();
		for(i=0;i<uSlices;i++)
		{
			createFace.NewOutterEdgeLine(pFLDownFace,((uSlices-i-1)%uSlices)*7,(uSlices-i)%uSlices*7);
			createFace.NewInnerLoopEdgeLine(pInnerLoop,(i+1)%uSlices*7+6,i*7+6);
		}
		//顶面
		f3dPolyFace *pFLUpFace=pBody->faceList.append();
		pFLUpFace->poly_norm.Set(0,0,1);
		pFLUpFace->material=crStdPart;
		pInnerLoop=pFLUpFace->inner_loop.append();
		for(i=0;i<uSlices;i++)
		{
			createFace.NewOutterEdgeLine(pFLUpFace,i*7+1,((i+1)%uSlices)*7+1);
			createFace.NewInnerLoopEdgeLine(pInnerLoop,((i+1)%uSlices)*7+2,i*7+2);
		}
		//颈面
		f3dPolyFace *pNeckFlatFace=pBody->faceList.append();
		pNeckFlatFace->poly_norm.Set(0,0,1);
		pNeckFlatFace->material=crStdPart;
		pInnerLoop=pNeckFlatFace->inner_loop.append();
		for(i=0;i<uSlices;i++)
		{
			createFace.NewOutterEdgeLine(pNeckFlatFace,i*7+4,((i+1)%uSlices)*7+4);
			createFace.NewInnerLoopEdgeLine(pInnerLoop,((i+1)%uSlices)*7+5,i*7+5);
		}
		//侧面
		for(i=0;i<uSlices;i++)
		{
			pFace=pBody->faceList.append();
			pFace->material = crStdPart;
			createFace.NewOutterEdgeLine(pFace,((i+1)%uSlices)*7,i*7);
			createFace.NewOutterEdgeLine(pFace,((i+1)%uSlices)*7+1);
			createFace.NewOutterEdgeLine(pFace,i*7+1);
			createFace.NewOutterEdgeLine(pFace,i*7);
			
			pFace=pBody->faceList.append();
			pFace->material = crStdPart;
			createFace.NewOutterEdgeLine(pFace,((i+1)%uSlices)*7+2,i*7+2);
			createFace.NewOutterEdgeLine(pFace,((i+1)%uSlices)*7+3);
			createFace.NewOutterEdgeLine(pFace,i*7+3);
			createFace.NewOutterEdgeLine(pFace,i*7+2);

			pFace=pBody->faceList.append();
			pFace->material = crStdPart;
			createFace.NewOutterEdgeLine(pFace,((i+1)%uSlices)*7+3,i*7+3);
			createFace.NewOutterEdgeLine(pFace,((i+1)%uSlices)*7+4);
			createFace.NewOutterEdgeLine(pFace,i*7+4);
			createFace.NewOutterEdgeLine(pFace,i*7+3);

			pFace=pBody->faceList.append();
			pFace->material = crStdPart;
			createFace.NewOutterEdgeLine(pFace,((i+1)%uSlices)*7+5,i*7+5);
			createFace.NewOutterEdgeLine(pFace,((i+1)%uSlices)*7+6);
			createFace.NewOutterEdgeLine(pFace,i*7+6);
			createFace.NewOutterEdgeLine(pFace,i*7+5);
		}
		if(bDisplayHole)
			CreateLsHole(pBody,pFLUpFace,pFLDownFace,f3dPoint(0,0,1));//定义螺栓孔
	}
	else if(m_iStdPartType==TYPE_FLG||m_iStdPartType==TYPE_FLR)
	{
		double fOuterRadius=param.fl_param2.fOuterD*0.5;
		double fInnerRadius=param.fl_param2.fInnerD*0.5;
		int slices= CalArcResolution(fOuterRadius,2*Pi,scale_of_user2scr,sample_len,smoothness);
		slices=min(slices,200);	//防止后面临时建立的outter_down_vertex_arr数组越界
		double coshalf_alfa = cos(Pi/slices);	//半角余弦
		double RO=fOuterRadius/coshalf_alfa;	//法兰外环显示半径
		double RI=fInnerRadius/coshalf_alfa;	//法兰内环显示半径
		double scaleRI=RI/RO;	//内环显示半径占外环显示半径的比例值
		//生成法兰不含螺栓孔在内的外轮廓顶点
		f3dPoint vertex_arr[200];
		CLDSPart::GetCircleSimuPolyVertex(RO,vertex_arr,slices);
		for(i=0;i<slices;i++)
		{
			pBody->vertex.append(vertex_arr[i]);	//外侧底部,i*4
			vertex_arr[i].z=param.fl_param2.fThick;
			pBody->vertex.append(vertex_arr[i]);	//外侧顶部,i*4+1
			vertex_arr[i]*=scaleRI;
			vertex_arr[i].z=param.fl_param2.fThick;
			pBody->vertex.append(vertex_arr[i]);	//内侧顶部,i*4+2
			vertex_arr[i].z=0;
			pBody->vertex.append(vertex_arr[i]);	//内侧底部,i*4+3
		}
		//初始化链表
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		//顶平
		fLoop* pLoop=NULL;
		f3dPolyFace *pTopFace=pBody->faceList.append();
		pTopFace->poly_norm.Set(0,0,1);
		pTopFace->material = crStdPart;
		if(fInnerRadius>0)
			pLoop=pTopFace->inner_loop.append();
		for(i=0;i<slices;i++)
		{
			createFace.NewOutterEdgeLine(pTopFace,i*4+1,((i+1)%slices)*4+1);
			if(fInnerRadius>0)
				createFace.NewInnerLoopEdgeLine(pLoop,((slices-i)%slices)*4+2,(slices-i-1)*4+2);
		}
		//底平面
		f3dPolyFace *pBottomFace=pBody->faceList.append();
		pBottomFace->poly_norm.Set(0,0,-1);
		pBottomFace->material = crStdPart;
		if(fInnerRadius>0)
			pLoop=pBottomFace->inner_loop.append();
		for(i=0;i<slices;i++)
		{
			createFace.NewOutterEdgeLine(pBottomFace,((slices-i)%slices)*4,(slices-i-1)*4);
			if(fInnerRadius>0)
				createFace.NewInnerLoopEdgeLine(pLoop,i*4+3,((i+1)%slices)*4+3);
		}
		//外圆柱侧面
		for(i=0;i<slices;i++)
		{
			pFace=pBody->faceList.append();
			pFace->material = crStdPart;
			createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4,i*4);
			createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+1);
			createFace.NewOutterEdgeLine(pFace,i*4+1);
			createFace.NewOutterEdgeLine(pFace,i*4);
		}
		//内圆柱侧面
		for(i=0;i<slices;i++)
		{
			pFace=pBody->faceList.append();
			pFace->material = crStdPart;
			createFace.NewOutterEdgeLine(pFace,i*4+2,i*4+3);
			createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+2);
			createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+3);
			createFace.NewOutterEdgeLine(pFace,i*4+3);
		}
		//定义螺栓孔
		if(bDisplayHole)
			CreateLsHole(pBody,pTopFace,pBottomFace,f3dPoint(0,0,1));
	}
	else if(m_iStdPartType==TYPE_ROLLEND)
	{
		int nVertex=6;
		//设置构件各个顶点的坐标
		double L=param.insert_plate_param.L;
		double H=param.insert_plate_param.H;
		double W=param.insert_plate_param.W;
		double N=param.insert_plate_param.N;
		double S=param.insert_plate_param.S;
		double T=param.insert_plate_param.Thick;
		//根据卷边处理方式设置卷边显示样式
		double fOuterOffsetLen=0.0,fInnerOffsetLen=0.0;	//
		if(cRollProcessType==1)
		{	//最少刀数1 保证卷边外侧边长度为L-N wht 11-04-18
			if((S+W)>0)	//最小导出1，内侧长度计算错误，应为N-(S*N)/(S+W)	wht 18-08-16
				fInnerOffsetLen=(N-(S*N)/(S+W));
		}
		else if(cRollProcessType==2)
		{	//最少刀数2 保证卷边内侧边长度为L-N
			if(S>0)
				fOuterOffsetLen=W*N/S;
		}
		f3dPoint vertex_arr[6];
		vertex_arr[0].Set(L,0.5*H-S);
		vertex_arr[1].Set(L-N+fInnerOffsetLen,0.5*H);
		vertex_arr[2].Set(0,0.5*H);
		vertex_arr[3].Set(0,-0.5*H);
		vertex_arr[4].Set(L-N+fInnerOffsetLen,-0.5*H);
		vertex_arr[5].Set(L,-0.5*H+S);
		for(i=0;i<6;i++)
		{
			vertice = vertex_arr[i];
			vertice.z=-0.5*T;
			pBody->vertex.append(vertice);
			vertice.z=0.5*T;
			pBody->vertex.append(vertice);
		}
		if(param.insert_plate_param.W>0)
		{	//上侧卷边  wht 08-02-27 
			vertice.Set(L-N+fInnerOffsetLen,0.5*H,-0.5*T);
			pBody->vertex.append(vertice);
			vertice.Set(L-N+fInnerOffsetLen,0.5*H-T,-0.5*T);
			pBody->vertex.append(vertice);
			vertice.Set(0,0.5*H-T,-0.5*T);
			pBody->vertex.append(vertice);
			vertice.Set(0,0.5*H,-0.5*T);
			pBody->vertex.append(vertice);
			
			vertice.Set(L-N-fOuterOffsetLen,0.5*H,-W+0.5*T);
			pBody->vertex.append(vertice);
			vertice.Set(L-N-fOuterOffsetLen,0.5*H-T,-W+0.5*T);
			pBody->vertex.append(vertice);
			vertice.Set(0,0.5*H-T,-W+0.5*T);
			pBody->vertex.append(vertice);
			vertice.Set(0,0.5*H,-W+0.5*T);
			pBody->vertex.append(vertice);
			//下侧卷边
			vertice.Set(0,-0.5*H,-0.5*T);
			pBody->vertex.append(vertice);
			vertice.Set(0,-0.5*H+T,-0.5*T);
			pBody->vertex.append(vertice);
			vertice.Set(L-N+fInnerOffsetLen,-0.5*H+T,-0.5*T);
			pBody->vertex.append(vertice);
			vertice.Set(L-N+fInnerOffsetLen,-0.5*H,-0.5*T);
			pBody->vertex.append(vertice);
		
			vertice.Set(0,-0.5*H,-W+0.5*T);
			pBody->vertex.append(vertice);
			vertice.Set(0,-0.5*H+T,-W+0.5*T);
			pBody->vertex.append(vertice);
			vertice.Set(L-N-fOuterOffsetLen,-0.5*H+T,-W+0.5*T);
			pBody->vertex.append(vertice);
			vertice.Set(L-N-fOuterOffsetLen,-0.5*H,-W+0.5*T);
			pBody->vertex.append(vertice);
		}
		//初始化链表
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		//基准面顶面
		f3dPolyFace *pTopDatumFace=pBody->faceList.append();
		pTopDatumFace->poly_norm.Set(0,0,1);
		pTopDatumFace->material=crStdPart;
		for(i=0;i<nVertex;i++)
			createFace.NewOutterEdgeLine(pTopDatumFace,((i+1)%nVertex)*2+1,i*2+1);
		//基准面底面
		f3dPolyFace *pBottomDatumFace=pBody->faceList.append();
		pBottomDatumFace->poly_norm.Set(0,0,-1);
		pBottomDatumFace->material=crStdPart;
		for(i=nVertex-1;i>=0;i--)
			createFace.NewOutterEdgeLine(pBottomDatumFace,i*2,((i+1)%nVertex)*2);
		//基准面侧面
		pFace;
		for(i=0;i<nVertex;i++)
		{
			pFace=pBody->faceList.append();
			pFace->material=crStdPart;
			createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*2,i*2);
			createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*2+1);
			createFace.NewOutterEdgeLine(pFace,i*2+1);
			createFace.NewOutterEdgeLine(pFace,i*2);
		}
		if(param.insert_plate_param.W>0)
		{
			nVertex*=2;
			//上卷边
			pFace=pBody->faceList.append();
			pFace->material=crStdPart;
			for(i=0;i<4;i++)
				pBody->NewEdgeLine(pFace,nVertex+4+(i+1)%4,nVertex+4+i);
			pFace=pBody->faceList.append();
			pFace->material=crStdPart;
			createFace.NewOutterEdgeLine(pFace,nVertex+1,nVertex);
			createFace.NewOutterEdgeLine(pFace,nVertex+5);
			createFace.NewOutterEdgeLine(pFace,nVertex+4);
			createFace.NewOutterEdgeLine(pFace,nVertex);
			pFace=pBody->faceList.append();
			pFace->material=crStdPart;
			createFace.NewOutterEdgeLine(pFace,nVertex+2,nVertex+1);
			createFace.NewOutterEdgeLine(pFace,nVertex+6);
			createFace.NewOutterEdgeLine(pFace,nVertex+5);
			createFace.NewOutterEdgeLine(pFace,nVertex+1);
			pFace=pBody->faceList.append();
			pFace->material=crStdPart;
			createFace.NewOutterEdgeLine(pFace,nVertex+3,nVertex+2);
			createFace.NewOutterEdgeLine(pFace,nVertex+7);
			createFace.NewOutterEdgeLine(pFace,nVertex+6);
			createFace.NewOutterEdgeLine(pFace,nVertex+2);
			pFace=pBody->faceList.append();
			pFace->material=crStdPart;
			createFace.NewOutterEdgeLine(pFace,nVertex,nVertex+3);
			createFace.NewOutterEdgeLine(pFace,nVertex+4);
			createFace.NewOutterEdgeLine(pFace,nVertex+7);
			createFace.NewOutterEdgeLine(pFace,nVertex+3);
			//下卷边
			pFace=pBody->faceList.append();
			pFace->material=crStdPart;
			for(i=0;i<4;i++)
				createFace.NewOutterEdgeLine(pFace,nVertex+12+(i+1)%4,nVertex+12+i);
			pFace=pBody->faceList.append();
			pFace->material=crStdPart;
			createFace.NewOutterEdgeLine(pFace,nVertex+1+8,nVertex+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+5+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+4+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+8);
			pFace=pBody->faceList.append();
			pFace->material=crStdPart;
			createFace.NewOutterEdgeLine(pFace,nVertex+2+8,nVertex+1+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+6+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+5+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+1+8);
			pFace=pBody->faceList.append();
			pFace->material=crStdPart;
			createFace.NewOutterEdgeLine(pFace,nVertex+3+8,nVertex+2+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+7+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+6+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+2+8);
			pFace=pBody->faceList.append();
			pFace->material=crStdPart;
			createFace.NewOutterEdgeLine(pFace,nVertex+8,nVertex+3+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+4+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+7+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+3+8);
		}
		if(bDisplayHole)
			CreateLsHole(pBody,pTopDatumFace,pBottomDatumFace,f3dPoint(0,0,1));//定义螺栓孔
	}
	else if(m_iStdPartType==TYPE_UEND)
	{
		int nVertex=6;
		//设置构件各个顶点的坐标
		f3dPoint vertex_arr[6];
		vertex_arr[0].Set(0,0.5*param.insert_plate_param.H,0);
		vertex_arr[1].Set(0,-0.5*param.insert_plate_param.H,0);
		vertex_arr[2].Set(param.insert_plate_param.L-param.insert_plate_param.N,-0.5*param.insert_plate_param.H,0);
		vertex_arr[3].Set(param.insert_plate_param.L,-0.5*param.insert_plate_param.H+param.insert_plate_param.S,0);
		vertex_arr[4].Set(param.insert_plate_param.L,0.5*param.insert_plate_param.H-param.insert_plate_param.S,0);
		vertex_arr[5].Set(param.insert_plate_param.L-param.insert_plate_param.N,0.5*param.insert_plate_param.H,0);
		f3dLine baseline;
		baseline.startPt=vertex_arr[0];
		baseline.endPt=vertex_arr[1];
		f3dPoint base_vec=baseline.endPt-baseline.startPt;
		base_vec.z=0;
		normalize(base_vec);
		f3dPoint base_normoffset_vec=f3dPoint(0,0,1)^base_vec;
		baseline.startPt+=base_normoffset_vec*(param.insert_plate_param.Thick+0.5*param.insert_plate_param.W);
		baseline.endPt+=base_normoffset_vec*(param.insert_plate_param.Thick+0.5*param.insert_plate_param.W);
		baseline.startPt.z=param.insert_plate_param.Thick+0.5*param.insert_plate_param.W;
		pBody->vertex.append(baseline.startPt);
		baseline.startPt.z=0.5*param.insert_plate_param.W;
		pBody->vertex.append(baseline.startPt);
		baseline.startPt.z=-0.5*param.insert_plate_param.W;
		pBody->vertex.append(baseline.startPt);
		baseline.startPt.z=-param.insert_plate_param.Thick-0.5*param.insert_plate_param.W;
		pBody->vertex.append(baseline.startPt);

		baseline.endPt.z=param.insert_plate_param.Thick+0.5*param.insert_plate_param.W;
		pBody->vertex.append(baseline.endPt);
		baseline.endPt.z=0.5*param.insert_plate_param.W;
		pBody->vertex.append(baseline.endPt);
		baseline.endPt.z=-0.5*param.insert_plate_param.W;
		pBody->vertex.append(baseline.endPt);
		baseline.endPt.z=-param.insert_plate_param.Thick-0.5*param.insert_plate_param.W;
		pBody->vertex.append(baseline.endPt);
		for(i=2;i<6;i++)
		{
			f3dPoint vertice=vertex_arr[i];
			vertice.z= 0.5*param.insert_plate_param.W+param.insert_plate_param.Thick;
			pBody->vertex.append(vertice);
			vertice.z= 0.5*param.insert_plate_param.W;
			pBody->vertex.append(vertice);
			vertice.z=-0.5*param.insert_plate_param.W;
			pBody->vertex.append(vertice);
			vertice.z=-0.5*param.insert_plate_param.W-param.insert_plate_param.Thick;
			pBody->vertex.append(vertice);
		}
		//用来显示曲形面 有八个点和以前的重复 为了以后添加面方便
		int uSlices = CalArcResolution(0.5*param.insert_plate_param.W+param.insert_plate_param.Thick
										,Pi,scale_of_user2scr,sample_len,smoothness);
		uSlices = min(uSlices,200);
		f3dPoint arc_vertex_arr[200],arc_vertex_arr_inner[200];
		double outter_radius = 0.5*param.insert_plate_param.W+param.insert_plate_param.Thick;
		double inner_radius = 0.5*param.insert_plate_param.W;
		f3dArcLine arcLine;
		f3dPoint arcLineNorm,arcLineStart,arcLineEnd,arcLineOringin;
		arcLineNorm.Set(0,0,1);
		arcLineOringin.Set(0,0.5*param.insert_plate_param.W+param.insert_plate_param.Thick,0);
		arcLineStart.Set(0.5*param.insert_plate_param.W+param.insert_plate_param.Thick,
							0.5*param.insert_plate_param.W+param.insert_plate_param.Thick,0);
		arcLineEnd.Set(0.5*param.insert_plate_param.W+param.insert_plate_param.Thick,
							-(0.5*param.insert_plate_param.W+param.insert_plate_param.Thick),0);
		//外环
		arcLine.CreateMethod3(arcLineStart,arcLineEnd,arcLineNorm,outter_radius,arcLineOringin);
		CLDSPart::GetArcSimuPolyVertex(&arcLine,arc_vertex_arr,uSlices);
		//内环
		arcLineStart.Set(0.5*param.insert_plate_param.W+param.insert_plate_param.Thick,0.5*param.insert_plate_param.W,0);
		arcLineEnd.Set(0.5*param.insert_plate_param.W+param.insert_plate_param.Thick,-0.5*param.insert_plate_param.W,0);
		arcLine.CreateMethod3(arcLineStart,arcLineEnd,arcLineNorm,inner_radius,arcLineOringin);
		CLDSPart::GetArcSimuPolyVertex(&arcLine,arc_vertex_arr_inner,uSlices);
		for(i=0;i<=uSlices;i++)
		{
			vertice.x = arc_vertex_arr[i].x;
			vertice.z = arc_vertex_arr[i].y;
			vertice.y = 0.5*param.insert_plate_param.H;
			pBody->vertex.append(vertice);//i*4+nVertex*4
			vertice.y = -0.5*param.insert_plate_param.H;
			pBody->vertex.append(vertice);//i*4+nVertex*4+1
			vertice.x = arc_vertex_arr_inner[i].x;
			vertice.z = arc_vertex_arr_inner[i].y;
			vertice.y = 0.5*param.insert_plate_param.H;
			pBody->vertex.append(vertice);//i*4+nVertex*4+2
			vertice.y = -0.5*param.insert_plate_param.H;
			pBody->vertex.append(vertice);//i*4+nVertex*4+3
		}
		//初始化链表
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		//顶平面
		f3dPolyFace *pTopFace=pBody->faceList.append();
		pTopFace->material = crStdPart;
		for(i=0;i<nVertex;i++)
			createFace.NewOutterEdgeLine(pTopFace,((i+1)%nVertex)*4,i*4);
		//中间层上平面
		f3dPolyFace *pMidUpFace=pBody->faceList.append();
		pMidUpFace->material = crStdPart;
		for(i=nVertex;i>0;i--)
			createFace.NewOutterEdgeLine(pMidUpFace,(i-1)*4+1,(i%nVertex)*4+1);
		//中间层下平面
		f3dPolyFace *pMidDownFace=pBody->faceList.append();
		pMidDownFace->material = crStdPart;
		for(i=0;i<nVertex;i++)
			createFace.NewOutterEdgeLine(pMidDownFace,((i+1)%nVertex)*4+2,i*4+2);
		//底平面
		f3dPolyFace *pBottomFace=pBody->faceList.append();
		pBottomFace->material = crStdPart;
		for(i=nVertex;i>0;i--)
			createFace.NewOutterEdgeLine(pBottomFace,(i-1)*4+3,(i%nVertex)*4+3);
		
		//定义螺栓孔
		if(bDisplayHole)
		{
			CreateLsHole(pBody,pTopFace,pMidUpFace,f3dPoint(0,0,1));		
			CreateLsHole(pBody,pMidDownFace,pBottomFace,f3dPoint(0,0,1));
		}
//		vector_trans(base_vec,ucs,TRUE);	//临时隐去将来应仔细考虑 wjh 2010-4-3
		//显示四个曲面
		int iArcStart = nVertex*4;
		for(i=0;i<uSlices;i++)
		{	//制弯外圆柱面
			pFace=pBody->faceList.append();
			pFace->material = crStdPart;
			createFace.NewOutterEdgeLine(pFace,(i+1)*4+iArcStart,i*4+iArcStart);
			createFace.NewOutterEdgeLine(pFace,(i+1)*4+iArcStart+1);
			createFace.NewOutterEdgeLine(pFace,i*4+iArcStart+1);
			createFace.NewOutterEdgeLine(pFace,i*4+iArcStart);
			//制弯内圆柱面
			pFace=pBody->faceList.append();
			pFace->material = crStdPart;
			createFace.NewOutterEdgeLine(pFace,i*4+iArcStart+2,(i+1)*4+iArcStart+2);
			createFace.NewOutterEdgeLine(pFace,i*4+iArcStart+3);
			createFace.NewOutterEdgeLine(pFace,(i+1)*4+iArcStart+3);
			createFace.NewOutterEdgeLine(pFace,(i+1)*4+iArcStart+2);
			//正半环形面
			pFace=pBody->faceList.append();
			pFace->material=crStdPart;
			createFace.NewOutterEdgeLine(pFace,i*4+iArcStart,(i+1)*4+iArcStart);
			createFace.NewOutterEdgeLine(pFace,i*4+iArcStart+2);
			createFace.NewOutterEdgeLine(pFace,(i+1)*4+iArcStart+2);
			createFace.NewOutterEdgeLine(pFace,(i+1)*4+iArcStart);
			//负半环形面
			pFace=pBody->faceList.append();
			pFace->material=crStdPart;
			createFace.NewOutterEdgeLine(pFace,(i+1)*4+iArcStart+1,i*4+iArcStart+1);
			createFace.NewOutterEdgeLine(pFace,(i+1)*4+iArcStart+3);
			createFace.NewOutterEdgeLine(pFace,i*4+iArcStart+3);
			createFace.NewOutterEdgeLine(pFace,i*4+iArcStart+1);
		}
		//各上层矩形侧面
		for(i=0;i<nVertex;i++)
		{
			if(i==0)
				continue;
			pFace=pBody->faceList.append();
			pFace->material = crStdPart;
			createFace.NewOutterEdgeLine(pFace,i*4+1,i*4);
			createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*4+1);
			createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*4);
			createFace.NewOutterEdgeLine(pFace,i*4);
		}
		//各下层矩形侧面
		for(i=0;i<nVertex;i++)
		{
			if(i==0)
				continue;
			pFace=pBody->faceList.append();
			pFace->material = crStdPart;
			createFace.NewOutterEdgeLine(pFace,i*4+3,i*4+2);
			createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*4+3);
			createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*4+2);
			createFace.NewOutterEdgeLine(pFace,i*4+2);
		}
	}
	else if(m_iStdPartType==TYPE_XEND)
	{
		int nVertex=8;
		//设置构件各个顶点的坐标
		f3dPoint vertex_arr[8];
		vertex_arr[0].Set(0,0.5*param.insert_plate_param.H,0);
		vertex_arr[1].Set(0,-0.5*param.insert_plate_param.H,0);
		vertex_arr[2].Set(param.insert_plate_param.L,-0.5*param.insert_plate_param.H,0);
		vertex_arr[3].Set(param.insert_plate_param.L,0.5*param.insert_plate_param.H,0);
		vertex_arr[4].Set(0,0,0.5*param.insert_plate_param.H);
		vertex_arr[5].Set(0,0,-0.5*param.insert_plate_param.H);
		vertex_arr[6].Set(param.insert_plate_param.L,0,-0.5*param.insert_plate_param.H);
		vertex_arr[7].Set(param.insert_plate_param.L,0,0.5*param.insert_plate_param.H);
		for(i=0;i<4;i++)
		{
			vertice = vertex_arr[i];
			vertice.z=-0.5*param.insert_plate_param.Thick;
			pBody->vertex.append(vertice);
			vertice.z=0.5*param.insert_plate_param.Thick;
			pBody->vertex.append(vertice);
		}
		for(i=4;i<nVertex;i++)
		{
			vertice = vertex_arr[i];
			vertice.y=0.5*param.insert_plate_param.Thick;
			pBody->vertex.append(vertice);
			vertice.y=-0.5*param.insert_plate_param.Thick;
			pBody->vertex.append(vertice);
		}
		//初始化链表
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		//基准面顶面
		f3dPolyFace *pTopDatumFace=pBody->faceList.append();
		pTopDatumFace->poly_norm.Set(0,0,1);
		pTopDatumFace->material=crStdPart;
		for(i=0;i<4;i++)
			createFace.NewOutterEdgeLine(pTopDatumFace,((i+1)%4)*2+1,i*2+1);
		//基准面底面
		f3dPolyFace *pBottomDatumFace=pBody->faceList.append();
		pBottomDatumFace->poly_norm.Set(0,0,-1);
		pBottomDatumFace->material=crStdPart;
		for(i=4-1;i>=0;i--)
			createFace.NewOutterEdgeLine(pBottomDatumFace,i*2,((i+1)%4)*2);
		//基准面侧面
		pFace;
		for(i=0;i<4;i++)
		{
			pFace=pBody->faceList.append();
			pFace->material=crStdPart;
			createFace.NewOutterEdgeLine(pFace,((i+1)%4)*2,i*2);
			createFace.NewOutterEdgeLine(pFace,((i+1)%4)*2+1);
			createFace.NewOutterEdgeLine(pFace,i*2+1);
			createFace.NewOutterEdgeLine(pFace,i*2);
		}
		//定义螺栓孔
		if(bDisplayHole)
			CreateLsHole(pBody,pTopDatumFace,pBottomDatumFace,f3dPoint(0,0,1));
		/*
		//基准面顶面
		f3dPolyFace *pTopDatumFace1=pBody->faceList.append();
		pTopDatumFace1->poly_norm=-ucs.axis_y;
		pTopDatumFace1->material=crStdPart;
		for(i=4;i<nVertex;i++)
			pBody->NewEdgeLine(pTopDatumFace1,((i+1)*2+1)%nVertex+8,i*2+1);
		//基准面底面
		f3dPolyFace *pBottomDatumFace1=pBody->faceList.append();
		pBottomDatumFace1->poly_norm=ucs.axis_y;
		pBottomDatumFace1->material=crStdPart;
		for(i=nVertex-1;i>=4;i--)
			pBody->NewEdgeLine(pBottomDatumFace1,i*2,((i+1)*2)%nVertex+8);
		//基准面侧面
		pFace;
		for(i=4;i<nVertex;i++)
		{
			pFace=pBody->faceList.append();
			pFace->material=crStdPart;
			pBody->NewEdgeLine(pFace,((i+1)*2)%nVertex+8,i*2);
			pBody->NewEdgeLine(pFace,((i+1)*2)%nVertex+8+1);
			pBody->NewEdgeLine(pFace,i*2+1);
			pBody->NewEdgeLine(pFace,i*2);
		}
		//定义螺栓孔
		CreateLsHole(pTopDatumFace1,pBottomDatumFace1,-ucs.axis_y);*/
	}
	return TRUE;
}

BOOL CLDSStdPartModel::CreateBoltInPlate(CLDSParamPlate *pParamPlate)
{
	long m_hLsFamily=1;
	if(pParamPlate->m_iParamType==TYPE_FLD)
	{
		if(!param.fl_param.IsSameCode(pParamPlate->GetPartNo()))
			return FALSE;	//保证要布置螺栓的构件为该类型的标准件
		m_hLsFamily=4;
	}
	else if(pParamPlate->m_iParamType==TYPE_FLP)
	{
		if(stricmp(param.fl_param.codeName,pParamPlate->GetPartNo())!=0)
			return FALSE;	//保证要布置螺栓的构件为该类型的标准件
		m_hLsFamily=4;
	}
	else if(pParamPlate->m_iParamType==TYPE_FLG||
		pParamPlate->m_iParamType==TYPE_FLR)
	{
		if(stricmp(param.fl_param2.codeName,pParamPlate->GetPartNo())!=0)
			return FALSE;
		m_hLsFamily=4;
	}
	else 
	{
		if(pParamPlate->m_iParamType==TYPE_XEND)
		{	//十字插板代号与基准钢板编号不相同,使用构件编号查找十字插板参数时应将编号与基板编号对比 wht 10-01-24
			if(stricmp(pParamPlate->GetPartNo(),param.insert_plate_param.codeName)!=0
				&&stricmp(pParamPlate->GetPartNo(),param.insert_plate_param.datumPlatePartNo)!=0)
				return FALSE;
		}
		else if(stricmp(pParamPlate->GetPartNo(),param.insert_plate_param.codeName)!=0)
			return FALSE;
	}
	if(boltPosList.GetNodeNum()<=0)
		LayoutBolt();
	int i=0;
	double piece_angle=0;	//法兰上两螺栓之间的夹角	wht 10-01-29
	if(pParamPlate->m_iParamType==TYPE_FLD||pParamPlate->m_iParamType==TYPE_FLP)
		piece_angle=2*Pi/param.fl_param.nBoltNum;
	else if(pParamPlate->m_iParamType==TYPE_FLG||pParamPlate->m_iParamType==TYPE_FLR)
		piece_angle=2*Pi/param.fl_param2.nBoltNum;
	for(f3dPoint *pPt=GetFirstBoltPos();pPt;pPt=GetNextBoltPos())
	{
		CLDSBolt *pBolt=(CLDSBolt*)pParamPlate->BelongModel()->AppendPart(CLS_BOLT);
		pBolt->m_hFamily=m_hLsFamily;
		pBolt->iSeg = pParamPlate->iSeg;
		pBolt->SetLayer(pParamPlate->layer());//调整螺栓的图层名
		pBolt->cfgword=pParamPlate->cfgword;	//调整螺栓配材号与基准构件配材号一致
		pBolt->des_base_pos.hPart = pParamPlate->handle;
		pBolt->des_base_pos.cLocationStyle=TRUE;
		pBolt->des_work_norm.norm_style=3;	//构件法线
		pBolt->des_work_norm.hVicePart=pParamPlate->handle;
		pBolt->des_work_norm.direction=0;	//0-朝外 1-朝内
		pBolt->ucs.axis_z=pParamPlate->ucs.axis_z;
		if(m_iStdPartType==TYPE_ROLLEND||m_iStdPartType==TYPE_UEND)
		{
			pBolt->des_base_pos.len_offset_dist = pPt->x;
			pBolt->des_base_pos.wing_offset_dist = pPt->y;
			pBolt->set_d(param.insert_plate_param.ls_param[0].iBoltD);
			if(pParamPlate->m_iParamType==TYPE_ROLLEND)
				pPt->z=-pParamPlate->GetThick()*0.5;
			else if(pParamPlate->m_iParamType==TYPE_UEND)
				pPt->z=-pParamPlate->GetThick()-param.insert_plate_param.W*0.5;
			pBolt->des_base_pos.norm_offset.AddThick((long)(pPt->z));
			pBolt->AddL0Thick(pParamPlate->handle,TRUE);
		}
		else if(m_iStdPartType==TYPE_XEND)
		{
			pBolt->des_base_pos.len_offset_dist = pPt->x;
			pBolt->des_base_pos.wing_offset_dist = pPt->y;
			if(pPt->feature==11)
			{	//X-Y平面: Y正方向一侧的螺栓
				if(param.insert_plate_param.L>pPt->x)//Y正方向一侧,连接插板与包板的螺栓
					pBolt->feature=11;
				else
					pBolt->feature=12;	//Y正方向一侧,连接包板与其他连板的螺栓
			}
			else if(pPt->feature==21)
			{	//X-Y平面: Y负方向一侧的螺栓
				if(param.insert_plate_param.L>pPt->x)//Y负方向一侧,连接插板与包板的螺栓
					pBolt->feature=21;
				else
					pBolt->feature=22;	//Y负方向一侧,连接包板与其他连板的螺栓
			}
			else if(pPt->feature==31)	
			{	//X-Z平面: Z正方向一侧
				if(param.insert_plate_param.L>pPt->x)
					pBolt->feature=31;	//Z正方向一侧，连接立板与其他连板的螺栓
				else
					pBolt->feature=32;	//Z正方向一侧，连接包板与其他连板的螺栓
				pBolt->des_work_norm.norm_style=4;
				pBolt->des_work_norm.nearVector.Set(0,1,0);
				pBolt->des_base_pos.wing_offset_dist = pPt->z;
				pBolt->ucs.axis_z=pParamPlate->ucs.axis_y;
			}
			else if(pPt->feature==41)	
			{	//X-Z平面: Z负方向一侧
				if(param.insert_plate_param.L>pPt->x)
					pBolt->feature=41;	//Z负方向一侧，连接立板与其他连板的螺栓
				else
					pBolt->feature=42;	//Z负方向一侧，连接包板与其他连板的螺栓
				pBolt->des_work_norm.norm_style=4;
				pBolt->des_work_norm.nearVector.Set(0,1,0);
				pBolt->des_base_pos.wing_offset_dist = pPt->z;
				pBolt->ucs.axis_z=pParamPlate->ucs.axis_y;
			}
			double W=param.insert_plate_param.W;
			pBolt->set_d(param.insert_plate_param.ls_param[0].iBoltD);
			pBolt->des_base_pos.norm_offset.AddThick((long)(-pParamPlate->GetThick()*0.5));
			//仅调整十字插板基板上的螺栓通厚 wht 10-01-24
			if(param.insert_plate_param.L>pPt->x&&(pPt->feature==11||pPt->feature==21))
				pBolt->AddL0Thick(pParamPlate->handle,TRUE);
		}
		else if(m_iStdPartType==TYPE_FLD||m_iStdPartType==TYPE_FLP)
		{	
			pBolt->des_base_pos.offset_angle = i*piece_angle;//螺栓相对于基准钢管的偏移角度
			pBolt->des_base_pos.R=0.5*param.fl_param.K;			//该变量小于0表示螺栓为非环向布置
			pBolt->ucs.axis_z=-pParamPlate->ucs.axis_z;
			pBolt->set_d(param.fl_param.nBoltD);
			pBolt->des_work_norm.direction=1;//标准法兰上的螺栓默认朝内(即与基准构件Z轴方向相反)
			pBolt->des_base_pos.norm_offset.AddThick(-(LONG)pParamPlate->GetThick());
			pBolt->AddL0Thick(pParamPlate->handle,TRUE);
			i++;	//螺栓数
		}
		else if(m_iStdPartType==TYPE_FLG||m_iStdPartType==TYPE_FLR)
		{	
			pBolt->des_base_pos.offset_angle = i*piece_angle;	//螺栓相对于基准钢管的偏移角度
			pBolt->des_base_pos.R=0.5*param.fl_param2.fBoltCirD;//该变量小于0表示螺栓为非环向布置
			pBolt->ucs.axis_z=-pParamPlate->ucs.axis_z;
			pBolt->set_d(param.fl_param2.nBoltD);
			pBolt->AddL0Thick(pParamPlate->handle,TRUE);
			i++;	//螺栓数
		}
		CLsRef *pLsRef=pParamPlate->AppendLsRef(pBolt->GetLsRef());
		if(pLsRef)
			pLsRef->start1_end2_mid0=(BYTE)pBolt->feature;	//标记螺栓所在位置
		pBolt->SetModified();
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		pBolt->CalGuigeAuto();
		pBolt->Create3dSolidModel();
	}
	return TRUE;
}
#endif