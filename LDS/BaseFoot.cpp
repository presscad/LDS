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
	double MAX_COMPRESSION=P*vLenStdVec.z;	//�����ڵ�������ѹ��kN��δ��Ӧֱ�Ӵӱ������ж�ȡ
	double MAX_TENSION=MAX_COMPRESSION*0.9;	//�����ڵ������ϲ���kN��δ��Ӧֱ�Ӵӱ������ж�ȡ
	int i,n;
	double PREFER_HOLE_MAGNIFY_COEF=1.3;//�׽���˨�׾��ϵ׽���˨����ֱ���ķŴ�ϵ��
	double PREFER_BOLT_STRENGTH=275;	//�׽���˨Ĭ��ѡQ345��
	int PREFER_CONCRETE_INDEX=2;		//Ĭ������ѡ��C25ˮ��
	double PREFER_SIGEMA_C=concrete_tbl[PREFER_CONCRETE_INDEX].fc;
	//����׽���˨ǿ�Ȳ�ѡ��׾�
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
	//�׽���˨��
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
	double z0=pBaseAngle->size_wide*0.25;	//�Ǹ����ĵ�ѥ�����
	double a1=1.5*bolt_tbl[i].d+(hole_d-bolt_tbl[i].d)*0.5+20;
	double c=3*bolt_tbl[i].d+20;	//�׽ſ��ĵ��װ��Ե����
	B=ftoi(z0+nShoePlateThick+a1+c)*2;	//�װ���
	int odd=B%20;
	if(odd>0)	//20Ϊ��������Բ��
		B+=(20-odd);

	//���ݻ�����ǿ������װ���
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
	//����װ���,���ա������涨��
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
	//��ֻ��������
	int BOLT_S=ftoi(B/2-c);
	pFootDesign->base.XA=pFootDesign->base.XB=pFootDesign->base.YA=pFootDesign->base.YB=B/2;
	for(CFootTempl::BASEPLATE::HOLE* pHole=pFootDesign->base.listHoles.EnumObjectFirst();pHole;
		pHole=pFootDesign->base.listHoles.EnumObjectNext())
	{	//�趨�׽���˨ֱ��
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
	//�����߰��λ��
	a=c+a1;
	M=a*a*a*sigema_c/3.0;//pBaseAngle->connectStart.N
	int nRibPlateThick=nShoePlateThick-2;
	h=(short)(0.5+sqrt(6*M/(nRibPlateThick*pSteel->f(nRibPlateThick))));
	for(CFootTempl::RIBPLATE* pRibPlate=pFootDesign->listRibPlates.GetFirst();pRibPlate;
		pRibPlate=pFootDesign->listRibPlates.GetNext())
	{
		if(pRibPlate->pos_para.PARA1.axisX_y==0||pRibPlate->axisXType==1||pRibPlate->axisXType==2)
		{	//X�����߰�
			if(pRibPlate->pos_para.PARA1.OY>0)
				pRibPlate->pos_para.PARA1.OY= B*0.5-c;
			else
				pRibPlate->pos_para.PARA1.OY=-B*0.5+c;
		}
		else if(pRibPlate->pos_para.PARA1.axisX_x==0||pRibPlate->axisXType==3||pRibPlate->axisXType==4)
		{	//Y�����߰�
			if(pRibPlate->pos_para.PARA1.OX>0)
				pRibPlate->pos_para.PARA1.OX= B*0.5-c;
			else
				pRibPlate->pos_para.PARA1.OX=-B*0.5+c;
		}
		pRibPlate->A=h;	//�߰�߶�
	}
	return true;
}
#endif