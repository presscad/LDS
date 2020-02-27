#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
#include "Tower.h"

//�ֹܶ˳�
class CEndBraceTempl{	
public:
	struct BASEPLATE{	//����
		char cMaterial;
		short thick;
		CXhChar16 sPartNo;
		double fR;			//���β���
		int nNormOffset;
		CLDSGeneralPlate* pPlate;
		BASEPLATE();
	};
	struct ENDPLATE{	//�˰�
		char cMaterial;
		short thick;
		CXhChar16 sPartNo;
		double H,r,R,C;	//���β���
		BYTE ibPosVec;		//0.����X+ 1.����X- 2.����Y+ 3.����Y-
		CLDSGeneralPlate* pPlate;
		ENDPLATE();
	};
	struct SPILEPLATE{	//���
		char cMaterial;
		short thick;
		CXhChar16 sPartNo;
		BYTE ibPosVec;	//0.X 1.X+ 2.X- 3.Y 4.Y+ 5.Y-
		double H;		//���߶�
		CLDSGeneralPlate* pPlate;
		SPILEPLATE();
	}; 
public:
	ATOM_LIST<ENDPLATE> listEndPlates;
	ATOM_LIST<SPILEPLATE> listSpilePlates;
	BASEPLATE m_xBasePlate;
	BYTE m_iEndPlateType;	//0.�޶˰� 1.X��˰� 2.Y��˰�
	BYTE m_iSpilePlateType;	//0.�޲�� 1.X��ͨ�� 2.Y��ͨ��
	double m_fEndPlateDist;
	long m_hBaseNode;
	long m_hBaseTube;
private:
	f3dPoint CalPosition();
	BOOL DesignBasePlate();
	BOOL DesignStationConnPlates();
	BOOL DesignStationCrossPlates();
	SPILEPLATE* FindAssistPlate(SPILEPLATE* pDesPlate);
public:
	CEndBraceTempl();
	BOOL DesignEndBrace();
	void RetrieveAttachPlates();
};
//�����а����
class CPillorPlywoodTemp
{
public:
	char cMaterial;
	short thick;
	CXhChar16 sPartNo;
	long m_hBasePlate;
	long m_hEndPlate;
	double R,H,A,B;	//���β���
	double fCutR;
	CLDSGeneralPlate* m_pPlate;
public:
	CPillorPlywoodTemp();
	BOOL DesignStationShoePlate();
};
#endif