#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Tower.h"
#include "DrawUnit.h"
#include "PartGroupDrawing.h"	//IDbEntsContext�����ͷ�ļ�������

class CTubeDrawing{
	bool CreateSectSlotDrawing(CLDSLineTube *pTube,CLDSGeneralPlate* pInsertPlate,CDrawCoordTrans& dcs,IDbEntsContext* pContext);		//���ƺ����ڲ۵ĸֹܽ���ͼ
public:
	struct TUBE_DRAWING_STYLE{
		bool bDrawExterCircle;	//�뷨��һ�����ʱ��Բ�����ƣ�Ӧ��Ϊfalse�ص�����
		bool bDimSectSlotAngle;	//��ע����ͼ�н��濪�ڲ۷Ǳ�Ƕȣ����ڷǴ�ֱ��ʮ�ֲ�忪�ۣ�
		bool bDrawWeldline;		//�к���ʱ�Ƿ���Ҫ���ƺ���
		long hSectSlotPlate;	//����ͼ�л��ƿ��ڲ۹����ĸְ�,0��ʾ�����ƿ��ڲ�
		char ciDrawModeH0V1;
		bool bMandatoryDimLabel;//ǿ���Ա�ע���
		TUBE_DRAWING_STYLE();
	}style;
	CTubeDrawing();
	void CreateUprightLayingDrawing(CLDSLineTube *pTube,CDrawCoordTrans& dcs,IDbEntsContext* pContext);	//���ƺ��ֹܽ���ͼ
	void CreateFlatLayingDrawing(CLDSLineTube *pTube,CDrawCoordTrans& dcs,IDbEntsContext* pContext);	//���ƺ��ֹ�ƽ��ͼ
	BOOL CreateTubeDrawing(CLDSLineTube *pTube,CDrawCoordTrans& dcs,IDbEntsContext* pContext,AcDbObjectId *pOutEntId=NULL);
};
