#include "XhCharString.h"
enum DIM_ITEM_TYPE{
	DIM_JG_SPART_NO=0,		//�Ǹֱ��
	DIM_BOLT_DIA=1,			//��˨ֱ��
	DIM_BOLT_HOR_DIST,		//��˨ˮƽ�߾�
	DIM_BOLT_VER_DIST,		//��˨����߾�
	DIM_BOLT_HIGH,			//��˨��λ�߶�
	DIM_RIB_SPART_NO1,		//�߰���1
	DIM_RIB_SPART_NO2,		//�߰���2
	DIM_RIB_H_DIST,			//�߰�̨�ڿ��
	DIM_RIB_V_DIST,			//�߰����߶�
	DIM_RIB_HIGH,			//�߰�߶�
	DIM_RIB_EDGE_DIST,		//�߰�߾�
	DIM_FL_THICK,			//�������
	DIM_WELD_THICK,			//���Ӻ��
	DIM_FL_SPART_NO=1,		//�������
	DIM_TUBE_DIA,			//�ֹ�ֱ��
	DIM_VIEW_SYMBOL,		//����ͼ��
	DIM_VIEW_NAME,			//��ͼ����
};

typedef struct tagTJ_DIM_INFO
{
	CXhChar16 sPartNo;			//�Ǹּ���
	CXhChar16 sBoltDim;			//��˨ֱ��
	CXhChar16 sBoltLocHigh;		//��˨��λ�߶�
	CXhChar16 sBoltHorDist;		//��˨ˮƽƫ�ƾ�
	CXhChar16 sBoltVerDist;		//��˨����ƫ�ƾ�
	CXhChar16 sRibHorDist;		//�߰�̨�ڿ��			
	CXhChar16 sRibVerDist;		//�߰����߶�	
	CXhChar16 sTjThick;			//���Ű���
	CXhChar16 sWeldThick;		//���Ӻ��
	CXhChar16 sTubeDim;			//�ֹ�ֱ��
	CXhChar16 sViewSymbol;		//����ͼ��
	CXhChar16 sViewName;		//��ͼ����
}TJ_DIM_INFO;

typedef struct tagFL_DIM_INFO
{
	CXhChar16 sFlPartNo;		//�������
	CXhChar16 sRibHorDist;		//�߰�̨�ڿ��
	CXhChar16 sRibVerDist;		//�߰����߶�
	CXhChar16 sRibPartNo1;		//�߰��ע1
	CXhChar16 sRibPartNo2;		//�߰��ע2
	CXhChar16 sViewSymbol;		//����ͼ��
	CXhChar16 sViewName;		//��ͼ����
}FL_DIM_INFO;