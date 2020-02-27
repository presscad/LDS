#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
#include "Tower.h"

//钢管端撑
class CEndBraceTempl{	
public:
	struct BASEPLATE{	//顶板
		char cMaterial;
		short thick;
		CXhChar16 sPartNo;
		double fR;			//外形参数
		int nNormOffset;
		CLDSGeneralPlate* pPlate;
		BASEPLATE();
	};
	struct ENDPLATE{	//端板
		char cMaterial;
		short thick;
		CXhChar16 sPartNo;
		double H,r,R,C;	//外形参数
		BYTE ibPosVec;		//0.顶板X+ 1.顶板X- 2.顶板Y+ 3.顶板Y-
		CLDSGeneralPlate* pPlate;
		ENDPLATE();
	};
	struct SPILEPLATE{	//插板
		char cMaterial;
		short thick;
		CXhChar16 sPartNo;
		BYTE ibPosVec;	//0.X 1.X+ 2.X- 3.Y 4.Y+ 5.Y-
		double H;		//插板高度
		CLDSGeneralPlate* pPlate;
		SPILEPLATE();
	}; 
public:
	ATOM_LIST<ENDPLATE> listEndPlates;
	ATOM_LIST<SPILEPLATE> listSpilePlates;
	BASEPLATE m_xBasePlate;
	BYTE m_iEndPlateType;	//0.无端板 1.X向端板 2.Y向端板
	BYTE m_iSpilePlateType;	//0.无插板 1.X向通板 2.Y向通板
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
//立柱夹板设计
class CPillorPlywoodTemp
{
public:
	char cMaterial;
	short thick;
	CXhChar16 sPartNo;
	long m_hBasePlate;
	long m_hEndPlate;
	double R,H,A,B;	//外形参数
	double fCutR;
	CLDSGeneralPlate* m_pPlate;
public:
	CPillorPlywoodTemp();
	BOOL DesignStationShoePlate();
};
#endif