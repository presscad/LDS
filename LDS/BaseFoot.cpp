#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "LibraryQuery.h"
#include "FootDesignTemplate.h"
#include "BaseFoot.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
#ifdef __SMART_DRAW_INC_
ANCHOR_BOLT_D bolt_tbl[17]=
{
	20,16.75,
	22,18.75,//*
	24,20.10,
	27,23.10,
	30,25.45,
	33,28.45,//*
	36,30.80,
	39,34.09,//*
	42,36.16,
	45,39.15,
	48,41.51,
	52,45.51,
	56,48.86,
	60,52.86,
	64,56.21,
	68,60.21,
	72,64.21,
};
static CONCRETE_FC concrete_tbl[14]=
{
	"C15", 7.2,
	"C20", 9.6,
	"C25",11.9,
	"C30",14.3,
	"C35",16.7,
	"C40",19.1,
	"C45",21.2,
	"C50",23.1,
	"C55",25.3,
	"C60",27.5,
	"C65",29.7,
	"C70",31.8,
	"C75",33.9,
	"C80",35.9,
};
//////////////////////////////////////////////////////////////////////

bool CalFootDesignParams(CLDSLineAngle* pBaseAngle,CLDSLineAngle* pVAngle,CFootTempl* pFootDesign)
{
	GEPOINT vLenStdVec=pBaseAngle->xPosEnd-pBaseAngle->xPosStart;
	if(vLenStdVec.z<0)
		vLenStdVec*=-1.0;
	normalize(vLenStdVec);
	STEELMAT* pSteel=QuerySteelMatByBriefMark(pBaseAngle->cMaterial);
	STEELMAT* pBasePlateSteel=pSteel;
	double P=pBaseAngle->GetArea()*pSteel->f(pBaseAngle->size_thick)*0.001*0.8;
	double MAX_COMPRESSION=P*vLenStdVec.z;	//基础节点的最大下压力kN，未来应直接从报告书中读取
	double MAX_TENSION=MAX_COMPRESSION*0.9;	//基础节点的最大上拨力kN，未来应直接从报告书中读取
	int i,n;
	double PREFER_HOLE_MAGNIFY_COEF=1.3;//底脚螺栓孔径较底脚螺栓名义直径的放大系数
	double PREFER_BOLT_STRENGTH=275;	//底脚螺栓默认选Q345钢
	int PREFER_CONCRETE_INDEX=2;		//默认优先选用C25水泥
	double PREFER_SIGEMA_C=concrete_tbl[PREFER_CONCRETE_INDEX].fc;
	//计算底脚螺栓强度并选择孔径
	int MAX_ANCHOR_BOLT_N=4;
	if(pBaseAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		MAX_ANCHOR_BOLT_N=pFootDesign->base.listHoles.Count<8?4:8;
	int nShoePlateThick=ftoi(pBaseAngle->size_thick)+2;
	CLDSGroupLineAngle* pBaseGroupAngle=NULL;
	if(pBaseAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		pBaseGroupAngle=(CLDSGroupLineAngle*)pBaseAngle;
		nShoePlateThick=ftoi(pBaseGroupAngle->jg_space);
	}
	long B=500;
	int BOLT_N=n=MAX_ANCHOR_BOLT_N;
	//for(n=4;n<=MAX_ANCHOR_BOLT_N;n+=4)
	//底脚螺栓数
	for(i=0;i<17;i++)
	{
		double area=Pi*bolt_tbl[i].inner_d*bolt_tbl[i].inner_d*0.25;	//mm2
		double fAllowTensionPerBolt=PREFER_BOLT_STRENGTH*area*0.001;	//kN
		if(n*fAllowTensionPerBolt<MAX_TENSION)
			continue;
		break;
	}
	i=min(i,16);
	double hole_d=bolt_tbl[i].d*PREFER_HOLE_MAGNIFY_COEF;
	double z0=pBaseAngle->size_wide*0.25;	//角钢重心到靴板距离
	double a1=1.5*bolt_tbl[i].d+(hole_d-bolt_tbl[i].d)*0.5+20;
	double c=3*bolt_tbl[i].d+20;	//底脚孔心到底板边缘距离
	B=ftoi(z0+nShoePlateThick+a1+c)*2;	//底板宽度
	int odd=B%20;
	if(odd>0)	//20为步长进行圆整
		B+=(20-odd);

	//根据混凝土强度验算底板宽度
	double sigema_c,area;
	while(true)
	{
		area=B*B-BOLT_N*0.25*Pi*hole_d*hole_d;
		//area/=100;	//mm2-->cm2
		sigema_c=1000*MAX_COMPRESSION/area;	// N/cm2
		if(sigema_c>PREFER_SIGEMA_C)
			B+=20;
		else
			break;
	}
	//计算底板厚度,参照《技术规定》
	double Q=1000*MAX_COMPRESSION/(B*B);
	double a=0.707*B;
	double M=0.06*Q*a*a;
	int baseT=20;
	while(true)
	{
		double ft=pBasePlateSteel->f(baseT);
		baseT=ftoi(sqrt(5*M/ft));
		if(baseT%2!=0)
			baseT+=1;
		if(pBasePlateSteel->f(baseT)==ft)
			break;
	}
	pFootDesign->base.thick=max(nShoePlateThick+12,baseT);
	//暂只处理方塔脚
	int BOLT_S=ftoi(B/2-c);
	pFootDesign->base.XA=pFootDesign->base.XB=pFootDesign->base.YA=pFootDesign->base.YB=B/2;
	for(CFootTempl::BASEPLATE::HOLE* pHole=pFootDesign->base.listHoles.EnumObjectFirst();pHole;
		pHole=pFootDesign->base.listHoles.EnumObjectNext())
	{	//设定底脚螺栓直径
		pHole->d=bolt_tbl[i].d;
		pHole->hole_d=ftoi(hole_d);
		if(pHole->x>0)
			pHole->x=BOLT_S;
		else
			pHole->x=-BOLT_S;
		if(pHole->y>0)
			pHole->y=BOLT_S;
		else
			pHole->y=-BOLT_S;
	}
	a=z0+B*0.5;
	M=a*a*B*sigema_c/3.0;//pBaseAngle->connectStart.N
	short h=(short)(0.5+sqrt(6*M/(nShoePlateThick*pSteel->f(nShoePlateThick))));
	for(CFootTempl::SHOEPLATE* pShoePlate=pFootDesign->listShoePlates.GetFirst();pShoePlate;
		pShoePlate=pFootDesign->listShoePlates.GetNext())
	{
		pShoePlate->thick=nShoePlateThick;
		pShoePlate->cMaterial=pBaseAngle->cMaterial;
		pShoePlate->minheight=h;
	}
	//计算肋板的位置
	a=c+a1;
	M=a*a*a*sigema_c/3.0;//pBaseAngle->connectStart.N
	int nRibPlateThick=nShoePlateThick-2;
	h=(short)(0.5+sqrt(6*M/(nRibPlateThick*pSteel->f(nRibPlateThick))));
	for(CFootTempl::RIBPLATE* pRibPlate=pFootDesign->listRibPlates.GetFirst();pRibPlate;
		pRibPlate=pFootDesign->listRibPlates.GetNext())
	{
		if(pRibPlate->pos_para.PARA1.axisX_y==0||pRibPlate->axisXType==1||pRibPlate->axisXType==2)
		{	//X轴向肋板
			if(pRibPlate->pos_para.PARA1.OY>0)
				pRibPlate->pos_para.PARA1.OY= B*0.5-c;
			else
				pRibPlate->pos_para.PARA1.OY=-B*0.5+c;
		}
		else if(pRibPlate->pos_para.PARA1.axisX_x==0||pRibPlate->axisXType==3||pRibPlate->axisXType==4)
		{	//Y轴向肋板
			if(pRibPlate->pos_para.PARA1.OX>0)
				pRibPlate->pos_para.PARA1.OX= B*0.5-c;
			else
				pRibPlate->pos_para.PARA1.OX=-B*0.5+c;
		}
		pRibPlate->A=h;	//肋板高度
	}
	return true;
}
#endif