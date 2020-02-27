#include "StdAfx.h"
#include "Tower.h"
#include "BraceConnectPoint.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
//�������
static bool LocalDesignBraceNodeConnect(BRACE_CONNECT_POINT* pBraceConnPoint)
{
	if (pBraceConnPoint==NULL||pBraceConnPoint->niBaseAngleWidth<40||pBraceConnPoint->cnBraceRayAngles<1)
		return false;
	short siMinAngle = 30;
	if (pBraceConnPoint->cnBraceRayAngles==1)
	{	//����������FK-01
		if (pBraceConnPoint->niBaseAngleWidth<=110)	//����ԭ��������ľ಻��
		{	//<110��֧��˫����˨
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
	{	//˫��������FK-02
		if (pBraceConnPoint->xarrRayAngles[0].blInsideAngle != pBraceConnPoint->xarrRayAngles[1].blInsideAngle)
		{	//һ����һ����
			short siAngleByRod[2];
			//��֤�Ƿ������߲����׼�ļнǹ�С�����ǿ������ͬһ��Ե����ᵼ�¸˼�������ƫ�ƹ���
			for (int j = 0; j < 2; j++)
			{
				//���н�ת����0~90��֮�䣬�Ƚ����׼�Ǹֵ����ֵ�Ƿ��С
				if (pBraceConnPoint->xarrRayAngles[j].siRayAngle <= 90)
					siAngleByRod[j] = pBraceConnPoint->xarrRayAngles[j].siRayAngle;
				else if (pBraceConnPoint->xarrRayAngles[j].siRayAngle >= 270)
					siAngleByRod[j] = 360 - pBraceConnPoint->xarrRayAngles[j].siRayAngle;
				else
					siAngleByRod[j] = abs(pBraceConnPoint->xarrRayAngles[j].siRayAngle - 180);
				//��֤�Ƿ������߲����׼�ļнǹ�С�����ǿ������ͬһ��Ե����ᵼ�¸˼�������ƫ�ƹ���
				if (siAngleByRod[j] <= siMinAngle)
				{
					pBraceConnPoint->xarrRayAngles[j].ciTipPosStyle = 0;
					pBraceConnPoint->xarrRayAngles[j].ciBaseStyleG = -1;
				}
			}
			if (siAngleByRod[0] >siMinAngle && siAngleByRod[1] >siMinAngle)
			{	//�޼нǹ�С�����߲ģ���ʱ���Խ��н���Խ�С�����߲�������һ�ĵĽ��湲����˨λ��
				//TODO:
				if (siAngleByRod[0] < siAngleByRod[1])
					pBraceConnPoint->xarrRayAngles[0].ciTipPosStyle = 1;
				else
					pBraceConnPoint->xarrRayAngles[1].ciTipPosStyle = 1;
			}
		}
		if (pBraceConnPoint->niBaseAngleWidth<=110)	//����ԭ��������ľ಻��
		{	//<110��֧��˫����˨
			for (int j=0;j<2;j++)
			{
				pBraceConnPoint->xarrRayAngles[j].ciTipPosStyle = 0;
				pBraceConnPoint->xarrRayAngles[j].ciBaseStyleG = -1;
			}
		}
		else //if(pBraceConnPoint->niBaseAngleWidth>=125)
		{	//����֫���125ʱ������������Ӧ���ھ�������֫��40mm���Ҳ�С��2d��dΪ��˨ֱ������
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
		//��ȡ���и˼����׼�ĵļн�
		for (j = 0; j < 3; j++)
		{
			if (pBraceConnPoint->xarrRayAngles[j].siRayAngle <= 90)
				siAngleByRod[j] = pBraceConnPoint->xarrRayAngles[j].siRayAngle;
			else if (pBraceConnPoint->xarrRayAngles[j].siRayAngle >= 270)
				siAngleByRod[j] = 360 - pBraceConnPoint->xarrRayAngles[j].siRayAngle;
			else
				siAngleByRod[j] = abs(pBraceConnPoint->xarrRayAngles[j].siRayAngle - 180);
			//��֤�Ƿ������߲����׼�ļнǹ�С�����ǿ������ͬһ��Ե����ᵼ�¸˼�������ƫ�ƹ���
			if (siAngleByRod[j] <= siMinAngle)
			{
				pBraceConnPoint->xarrRayAngles[j].ciTipPosStyle = 0;
				pBraceConnPoint->xarrRayAngles[j].ciBaseStyleG = -1;
			}
			else
			{	//�нǲ���С�ķ�Χ�ڣ�ɸѡ�н���Խ�С�ĸ˼�����
				if (nMinAngleRodIndex == -1)
					nMinAngleRodIndex = j;
				else
				{
					if(siAngleByRod[j] < siAngleByRod[nMinAngleRodIndex])
						nMinAngleRodIndex = j;
				}
			}
		}
		//���н���Խ�С�����߲�������һ�ĵĽ��湲����˨λ��(��ˮƽ�Ǹֵ�������������ͬʱ��Ĭ��ƫ��)
		if (nMinAngleRodIndex >= 0)
			pBraceConnPoint->xarrRayAngles[nMinAngleRodIndex].ciTipPosStyle = 1;
		if (pBraceConnPoint->niBaseAngleWidth <= 110)	//����ԭ��������ľ಻��
		{	//<110��֧��˫����˨
			for (j=0;j<3;j++)
			{
				if(j!=nMinAngleRodIndex)
					pBraceConnPoint->xarrRayAngles[j].ciTipPosStyle = 0;
				pBraceConnPoint->xarrRayAngles[j].ciBaseStyleG = -1;
			}
		}
		else //if(pBraceConnPoint->niBaseAngleWidth>=125)
		{	//����֫���125ʱ������������Ӧ���ھ�������֫��40mm���Ҳ�С��2d��dΪ��˨ֱ������
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
