#include "StdAfx.h"
#include "SegI.h"
#include "lds_co_part.h"

struct ROUTINE_CHKINFO
{
	SEGI segI;
	CXhChar50 sLabel;
	BOOL bPassed;
	CXhChar16 sSpec;
	virtual CXhChar200 GetErrorMessage(){return "";};
	ROUTINE_CHKINFO(){bPassed=FALSE;}
};
//1.��ϽǸֱ�ż���϶�Ŵ�ֵ
struct GROUPANGLESPACE_CHKINFO  :public ROUTINE_CHKINFO
{
	CLDSGroupLineAngle* pGroupAngle;
	CLDSGeneralPlate* pPlate;
	GROUPANGLESPACE_CHKINFO(){
		pGroupAngle=NULL;
		pPlate=NULL;
	}
	static WORD MANU_SPACE;
	virtual CXhChar200 GetErrorMessage();
};
typedef GROUPANGLESPACE_CHKINFO* EXPORT_INFO_PTR;
//2.�ְ�߾�(��ͨ�ְ�\���ְ�\���)�쳣��ʾ��Ϣ
struct PLATESPACE_CHKINFO:public ROUTINE_CHKINFO
{
	static const int  GENERAL_PLATE	=0;
	static const int  WRAPPER_PLATE	=1;
	static const int  GASKET_PLATE	=2;
	long hPlate,hBolt;
	int iSubType;	//0.��ͨ��;1.���ְ�;2.���
	double actualEndSpace;	//ʵ�ʵ���˨�߾�
	long needEndSpace;		//���涨��Ҫ����˨�߾�
	virtual CXhChar200 GetErrorMessage();
	PLATESPACE_CHKINFO(){iSubType=GENERAL_PLATE;}
};
//3.�˼�����ͷ�쳣��ʾ��Ϣ
struct RODODD_CHKINFO  :public ROUTINE_CHKINFO
{
	char ciCurrIsStart0End1;
	CLDSLinePart *pRod,*pRod2;
	RODODD_CHKINFO(){
		pRod=pRod2=NULL;
		ciCurrIsStart0End1=0;
	}
	virtual CXhChar200 GetErrorMessage();
};
//2.���߰弰���߰屨�漰��Բ������
struct RIB_EXPORT_INFO :public ROUTINE_CHKINFO
{
	double fThick;
	CXhChar16 sAngle;		//����
	CXhChar16 sPlateType;
	RIB_EXPORT_INFO(){fThick=0;sAngle=0;}
};
//3.��ߴ���
struct ROLL_EDGE_EXPORT_INFO  :public ROUTINE_CHKINFO
{
	CXhChar100 sRollEdgeHeigth;
	int   nRollEdge;
	char cMaterial;
	CXhChar16 sRollEdgeDispose;
	ROLL_EDGE_EXPORT_INFO(){bPassed=FALSE;nRollEdge=0;cMaterial=' ';}
};

//4.��������˨��׼�϶
struct BOLTEXTERSPACE_CHKINFO : public ROUTINE_CHKINFO
{
	WORD Md;			//��˨ֱ��
	WORD minSpace;		//������Сֵ
	long hBolt;
	long hAngle;
	double clearance;	//ʵ����׼�϶
	CXhChar16 labelAngle,labelPlate;
};
//5���Ǹ���˨�߾�
struct JGBOLTSPACE_CHKINFO:public ROUTINE_CHKINFO
{
	static const BYTE  ROLLING_SPACE	=0;	//���Ʊ߾���
	static const BYTE  CUTANGLE_SPACE	=1;	//�нǱ߾���
	static const BYTE  LS_END_SPACE		=2;	//��˨�˾���
	//
	BYTE ciCheckType;
	long hJg,hBolt;
	BOOL bLsInStart;
	double realEndSpace;	//ʵ�ʵ���˨�߾�
	long needEndSpace;		//���涨��Ҫ����˨�߾�
	JGBOLTSPACE_CHKINFO(){ciCheckType=0;hJg=hBolt=0;bLsInStart=FALSE;}
	virtual CXhChar200 GetErrorMessage();
};