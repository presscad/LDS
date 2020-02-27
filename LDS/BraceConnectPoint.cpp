#include "StdAfx.h"
#include "Tower.h"
#include "BraceConnectPoint.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
//辅材设计
static bool LocalDesignBraceNodeConnect(BRACE_CONNECT_POINT* pBraceConnPoint)
{
	if (pBraceConnPoint==NULL||pBraceConnPoint->niBaseAngleWidth<40||pBraceConnPoint->cnBraceRayAngles<1)
		return false;
	short siMinAngle = 30;
	if (pBraceConnPoint->cnBraceRayAngles==1)
	{	//单射线连接FK-01
		if (pBraceConnPoint->niBaseAngleWidth<=110)	//保持原设计连接心距不变
		{	//<110不支持双排螺栓
			pBraceConnPoint->xarrRayAngles[0].ciTipPosStyle=0;
			pBraceConnPoint->xarrRayAngles[0].ciBaseStyleG=-1;
		}
		else //if(pBraceConnPoint->niBaseAngleWidth>=125)
		{
			pBraceConnPoint->xarrRayAngles[0].ciTipPosStyle=false;
			pBraceConnPoint->xarrRayAngles[0].ciBaseStyleG=5;
			pBraceConnPoint->xarrRayAngles[0].siOffsetG=-max(40,pBraceConnPoint->xarrRayAngles[0].ciBoltD*2);
		}
		return true;
	}
	else if (pBraceConnPoint->cnBraceRayAngles==2)
	{	//双射线连接FK-02
		if (pBraceConnPoint->xarrRayAngles[0].blInsideAngle != pBraceConnPoint->xarrRayAngles[1].blInsideAngle)
		{	//一里铁一外铁
			short siAngleByRod[2];
			//验证是否有射线材与基准材夹角过小（如果强行移至同一外缘交点会导致杆件合力线偏移过大）
			for (int j = 0; j < 2; j++)
			{
				//将夹角转换到0~90°之间，比较与基准角钢的锐角值是否过小
				if (pBraceConnPoint->xarrRayAngles[j].siRayAngle <= 90)
					siAngleByRod[j] = pBraceConnPoint->xarrRayAngles[j].siRayAngle;
				else if (pBraceConnPoint->xarrRayAngles[j].siRayAngle >= 270)
					siAngleByRod[j] = 360 - pBraceConnPoint->xarrRayAngles[j].siRayAngle;
				else
					siAngleByRod[j] = abs(pBraceConnPoint->xarrRayAngles[j].siRayAngle - 180);
				//验证是否有射线材与基准材夹角过小（如果强行移至同一外缘交点会导致杆件合力线偏移过大）
				if (siAngleByRod[j] <= siMinAngle)
				{
					pBraceConnPoint->xarrRayAngles[j].ciTipPosStyle = 0;
					pBraceConnPoint->xarrRayAngles[j].ciBaseStyleG = -1;
				}
			}
			if (siAngleByRod[0] >siMinAngle && siAngleByRod[1] >siMinAngle)
			{	//无夹角过小的射线材，此时可以将夹角相对较小的射线材移至另一材的交叉共用螺栓位置
				//TODO:
				if (siAngleByRod[0] < siAngleByRod[1])
					pBraceConnPoint->xarrRayAngles[0].ciTipPosStyle = 1;
				else
					pBraceConnPoint->xarrRayAngles[1].ciTipPosStyle = 1;
			}
		}
		if (pBraceConnPoint->niBaseAngleWidth<=110)	//保持原设计连接心距不变
		{	//<110不支持双排螺栓
			for (int j=0;j<2;j++)
			{
				pBraceConnPoint->xarrRayAngles[j].ciTipPosStyle = 0;
				pBraceConnPoint->xarrRayAngles[j].ciBaseStyleG = -1;
			}
		}
		else //if(pBraceConnPoint->niBaseAngleWidth>=125)
		{	//主材肢宽≥125时，辅助材心线应交于距受力材肢边40mm（且不小于2d，d为螺栓直径）处
			for (int j = 0; j < 2; j++)
			{
				pBraceConnPoint->xarrRayAngles[j].ciTipPosStyle = 0;
				pBraceConnPoint->xarrRayAngles[j].ciBaseStyleG = 5;
				pBraceConnPoint->xarrRayAngles[j].siOffsetG = -max(40,pBraceConnPoint->xarrRayAngles[0].ciBoltD * 2);
				int odd=pBraceConnPoint->xarrRayAngles[j].siOffsetG%10;
				if (odd>0)
					pBraceConnPoint->xarrRayAngles[j].siOffsetG+=10-odd;
			}
		}
		return true;
	}
	else if (pBraceConnPoint->cnBraceRayAngles == 3) 
	{
		short siAngleByRod[3];
		int j = 0,nMinAngleRodIndex = -1;
		//获取所有杆件与基准材的夹角
		for (j = 0; j < 3; j++)
		{
			if (pBraceConnPoint->xarrRayAngles[j].siRayAngle <= 90)
				siAngleByRod[j] = pBraceConnPoint->xarrRayAngles[j].siRayAngle;
			else if (pBraceConnPoint->xarrRayAngles[j].siRayAngle >= 270)
				siAngleByRod[j] = 360 - pBraceConnPoint->xarrRayAngles[j].siRayAngle;
			else
				siAngleByRod[j] = abs(pBraceConnPoint->xarrRayAngles[j].siRayAngle - 180);
			//验证是否有射线材与基准材夹角过小（如果强行移至同一外缘交点会导致杆件合力线偏移过大）
			if (siAngleByRod[j] <= siMinAngle)
			{
				pBraceConnPoint->xarrRayAngles[j].ciTipPosStyle = 0;
				pBraceConnPoint->xarrRayAngles[j].ciBaseStyleG = -1;
			}
			else
			{	//夹角不过小的范围内，筛选夹角相对较小的杆件索引
				if (nMinAngleRodIndex == -1)
					nMinAngleRodIndex = j;
				else
				{
					if(siAngleByRod[j] < siAngleByRod[nMinAngleRodIndex])
						nMinAngleRodIndex = j;
				}
			}
		}
		//将夹角相对较小的射线材移至另一材的交叉共用螺栓位置(与水平角钢的里外铁方向相同时，默认偏移)
		if (nMinAngleRodIndex >= 0)
			pBraceConnPoint->xarrRayAngles[nMinAngleRodIndex].ciTipPosStyle = 1;
		if (pBraceConnPoint->niBaseAngleWidth <= 110)	//保持原设计连接心距不变
		{	//<110不支持双排螺栓
			for (j=0;j<3;j++)
			{
				if(j!=nMinAngleRodIndex)
					pBraceConnPoint->xarrRayAngles[j].ciTipPosStyle = 0;
				pBraceConnPoint->xarrRayAngles[j].ciBaseStyleG = -1;
			}
		}
		else //if(pBraceConnPoint->niBaseAngleWidth>=125)
		{	//主材肢宽≥125时，辅助材心线应交于距受力材肢边40mm（且不小于2d，d为螺栓直径）处
			for (j = 0; j < 3; j++)
			{
				if (j != nMinAngleRodIndex)
					pBraceConnPoint->xarrRayAngles[j].ciTipPosStyle = 0;
				pBraceConnPoint->xarrRayAngles[j].ciBaseStyleG = 5;
				pBraceConnPoint->xarrRayAngles[j].siOffsetG = -max(40, pBraceConnPoint->xarrRayAngles[0].ciBoltD * 2);
			}
		}
		if (pBraceConnPoint->xarrRayAngles[1].ciBoltN > 1)
		{
			pBraceConnPoint->xarrRayAngles[1].ciTipPosStyle = -1;
			pBraceConnPoint->xarrRayAngles[1].ciBaseStyleG = -1;
		}
		return true;
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////
// BRACE_CONNECT_POINT
DefDesignBraceNodeConnectFunc BRACE_CONNECT_POINT::FireDesignBraceNodeConnectFunc=NULL;
BRACE_CONNECT_POINT::BRACE_CONNECT_POINT(WORD niBaseWidth/*=0*/,
					RAYANGLE* pAttachRayAngles/*=NULL*/,BYTE cnRayAngels/*=0*/)
{
	m_bRedesigned=false;
	niBaseAngleWidth=niBaseWidth;
	cnBraceRayAngles=cnRayAngels;
	xarrRayAngles=pAttachRayAngles;
	FireDesignBraceNodeConnectFunc=LocalDesignBraceNodeConnect;
}
bool BRACE_CONNECT_POINT::DesignConnectPoint()
{
	if (FireDesignBraceNodeConnectFunc!=NULL)
		return m_bRedesigned=FireDesignBraceNodeConnectFunc(this);
	else
		return m_bRedesigned=LocalDesignBraceNodeConnect(this);
}
