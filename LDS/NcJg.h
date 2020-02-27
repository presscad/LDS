// NcJg.h: interface for the CNcJg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NCJG_H__C14AAEC8_CE1B_4BF3_8409_4CC48D8AA5D7__INCLUDED_)
#define AFX_NCJG_H__C14AAEC8_CE1B_4BF3_8409_4CC48D8AA5D7__INCLUDED_

#include "f_ent.h"
#include "lds_part.h"
#include "lds_co_part.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
class CNcJg  
{
	CArray<f3dCircle,f3dCircle>LsCirX;
	CArray<f3dCircle,f3dCircle>LsCirY;
	double wing_wide,wing_thick,length,current_len_pos,current_accurate_len_pos;
	char cMaterial;
	char sPartNo[20];
	bool blScatterBoltLocaTolerance;
	BOOL InitJg(CLDSLineAngle *pJg);
	BOOL InitJg(CLDSPolyPart *pPolyAngle);
	const static BYTE STAMP_POS_IN_WING_X	= 0;	//查找在X肢有效的钢印号位置
	const static BYTE STAMP_POS_IN_WING_Y	= 1;	//查找在Y肢有效的钢印号位置
	const static BYTE STAMP_POS_IN_WING_XY	= 2;	//查找在XY肢同时有效的钢印号位置
	BOOL FindPartNoStampPos(BOOL bReverseOrder,BYTE wingx0_y1_all2,double *pfStampPos);
public:
	char sPrefix[20];
	CJgNcDataManager NcManager;
	void PrintLn(FILE* fp,char *format,f3dCircle *ls_cir=NULL);
	CNcJg();
	virtual ~CNcJg();
	BOOL InitJg(CLDSPart *pAnglePart);
	BOOL GenNCFile(char *sFileName,char *sPartNoPrefix=NULL);
	BOOL GetXLsCircle(f3dCircle &Cir,int index);
	BOOL GetYLsCircle(f3dCircle &Cir,int index);
	int GetXLsCount();
	int GetYLsCount();

};
#endif
#endif // !defined(AFX_NCJG_H__C14AAEC8_CE1B_4BF3_8409_4CC48D8AA5D7__INCLUDED_)
