#pragma once
#include "ArrayList.h"

struct WELD_PLATE_ITEM
{
	long m_hBaseTube;
	long m_hBaseNode;
	long m_liAmendOffsetY;	//�Ի��ƫ�ƾ�
	struct PLATE{
		CLDSPlate* pPlate;
		GEPOINT vRegWeldVec;//���ӱ���������vRegWeldVec�����׼�ֹ�ʼ->��һ�£� wjh-2019.11.02
		f3dLine weld_line;
		long liEdgeLength;	//���ӱ߳���
		long liSecondAmendOffsetY;	//�Ի��Y�����ƫ�ƾ�
		PLATE(CLDSPlate* _pPlate=NULL,long _liSecondAmendOffsetY=0) {
			pPlate=_pPlate;
			liEdgeLength=0;
			liSecondAmendOffsetY =_liSecondAmendOffsetY;
		}
	};
	ATOM_LIST<PLATE> xarrPlates;
public:
	WELD_PLATE_ITEM(){m_hBaseTube=0;m_hBaseNode=0; m_liAmendOffsetY =0;}
	BOOL IsWeldPtOutWeldLine(IModel* pModel);
};
