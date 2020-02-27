#include "StdAfx.h"
#include "Tower.h"
#include "IntelliJointConnect.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
//辅材设计
static bool LocalDesignJointConnect(INTELLI_JOINT_CONNECT* pJointConnect)
{
	INTELLI_JOINT_CONNECT::MAINANGLE xMinAngle=pJointConnect->pairangles[0];
	if (xMinAngle.wiAngleWidth>pJointConnect->pairangles[1].wiAngleWidth||(
		xMinAngle.wiAngleWidth==pJointConnect->pairangles[1].wiAngleWidth&&
		xMinAngle.ciAngleThick>pJointConnect->pairangles[1].ciAngleThick))
		xMinAngle=pJointConnect->pairangles[1];
	if (xMinAngle.wiAngleWidth<63)
		return false;
	if (xMinAngle.wiAngleWidth<=90)
	{
		pJointConnect->pairangles[0].ciBoltD=pJointConnect->pairangles[1].ciBoltD=20;
		pJointConnect->pairangles[0].cnMinBoltN=pJointConnect->pairangles[1].cnMinBoltN=4;
		pJointConnect->ciJointStyle=0;
	}
	else if (xMinAngle.wiAngleWidth<=110)
	{
		pJointConnect->pairangles[0].ciBoltD=pJointConnect->pairangles[1].ciBoltD=20;
		pJointConnect->pairangles[0].cnMinBoltN=pJointConnect->pairangles[1].cnMinBoltN=5;
		pJointConnect->ciJointStyle=0;
	}
	else if (xMinAngle.wiAngleWidth<=125)
	{
		pJointConnect->pairangles[0].ciBoltD=pJointConnect->pairangles[1].ciBoltD=20;
		pJointConnect->pairangles[0].cnMinBoltN=pJointConnect->pairangles[1].cnMinBoltN=5;
		pJointConnect->ciJointStyle=2;
	}
	else if (xMinAngle.wiAngleWidth<=160)
	{
		pJointConnect->pairangles[0].ciBoltD=pJointConnect->pairangles[1].ciBoltD=20;
		pJointConnect->pairangles[0].cnMinBoltN=pJointConnect->pairangles[1].cnMinBoltN=6;
		pJointConnect->ciJointStyle=2;
	}
	else if (xMinAngle.wiAngleWidth<=180)
	{
		pJointConnect->pairangles[0].ciBoltD=pJointConnect->pairangles[1].ciBoltD=24;
		pJointConnect->pairangles[0].cnMinBoltN=pJointConnect->pairangles[1].cnMinBoltN=6;
		pJointConnect->ciJointStyle=2;
	}
	else if (xMinAngle.wiAngleWidth<=220)
	{
		pJointConnect->pairangles[0].ciBoltD=pJointConnect->pairangles[1].ciBoltD=24;
		pJointConnect->pairangles[0].cnMinBoltN=pJointConnect->pairangles[1].cnMinBoltN=7;
		pJointConnect->ciJointStyle=2;
	}
	else if (xMinAngle.wiAngleWidth<=250)
	{
		pJointConnect->pairangles[0].ciBoltD=pJointConnect->pairangles[1].ciBoltD=24;
		pJointConnect->ciJointStyle=2;
		if(xMinAngle.ciAngleThick<=26)
			pJointConnect->pairangles[0].cnMinBoltN=pJointConnect->pairangles[1].cnMinBoltN=8;
		else if(xMinAngle.ciAngleThick<=30)
			pJointConnect->pairangles[0].cnMinBoltN=pJointConnect->pairangles[1].cnMinBoltN=9;
		else //if(xMinAngle.ciAngleThick<=35)
			pJointConnect->pairangles[0].cnMinBoltN=pJointConnect->pairangles[1].cnMinBoltN=10;
	}
	else if (xMinAngle.wiAngleWidth<=280)
	{
		pJointConnect->pairangles[0].ciBoltD=pJointConnect->pairangles[1].ciBoltD=24;
		pJointConnect->ciJointStyle=2;
		if(xMinAngle.ciAngleThick<=24)
			pJointConnect->pairangles[0].cnMinBoltN=pJointConnect->pairangles[1].cnMinBoltN=8;
		else if(xMinAngle.ciAngleThick<=30)
			pJointConnect->pairangles[0].cnMinBoltN=pJointConnect->pairangles[1].cnMinBoltN=10;
		else //if(xMinAngle.ciAngleThick<=35)
			pJointConnect->pairangles[0].cnMinBoltN=pJointConnect->pairangles[1].cnMinBoltN=12;
	}
	else //if (xMinAngle.wiAngleWidth<=300)
	{
		pJointConnect->pairangles[0].ciBoltD=pJointConnect->pairangles[1].ciBoltD=24;
		pJointConnect->ciJointStyle=2;
		if(xMinAngle.ciAngleThick<=24)
			pJointConnect->pairangles[0].cnMinBoltN=pJointConnect->pairangles[1].cnMinBoltN=9;
		else if(xMinAngle.ciAngleThick<=30)
			pJointConnect->pairangles[0].cnMinBoltN=pJointConnect->pairangles[1].cnMinBoltN=11;
		else //if(xMinAngle.ciAngleThick<=35)
			pJointConnect->pairangles[0].cnMinBoltN=pJointConnect->pairangles[1].cnMinBoltN=13;
	}
	pJointConnect->pairangles[0].cnRows=(pJointConnect->pairangles[0].cnMinBoltN<=5)?1:2;
	pJointConnect->pairangles[1].cnRows=(pJointConnect->pairangles[1].cnMinBoltN<=5)?1:2;
	pJointConnect->blRowsSpecified=true;
	return true;
}
/////////////////////////////////////////////////////////////////////////////
// INTELLI_JOINT_CONNECT
DefDesignJointConnectFunc INTELLI_JOINT_CONNECT::FireIntelliDesignJointConnectFunc=NULL;
INTELLI_JOINT_CONNECT::INTELLI_JOINT_CONNECT()
{
	m_bRedesigned=blRowsSpecified=false;
	blPostAngle=true;		//[输入]主材接头
	ciJointStyle=2;		//[输出]0.外包铁;1.内外包铁;2.内包铁外包钢板;3.内包钢板外包铁;
	pairangles[0].ciBoltD=20;	//接头所连接的角钢
	pairangles[0].cnMinBoltN=3;
	pairangles[0].cnRows=1;
	pairangles[0].ciAngleThick=10;
	pairangles[0].wiAngleWidth=100;
	pairangles[1]=pairangles[0];
	FireIntelliDesignJointConnectFunc=LocalDesignJointConnect;
}
bool INTELLI_JOINT_CONNECT::DesignJointConnect()
{
	if (FireIntelliDesignJointConnectFunc!=NULL)
		return m_bRedesigned=FireIntelliDesignJointConnectFunc(this);
	else
		return m_bRedesigned=LocalDesignJointConnect(this);
}
