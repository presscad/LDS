// NcPlank.h: interface for the CNcPlank class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NCPLANK_H__2D238B60_4DDA_47FF_9D2C_3E8D830E2D93__INCLUDED_)
#define AFX_NCPLANK_H__2D238B60_4DDA_47FF_9D2C_3E8D830E2D93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "f_ent.h"
#include "Tower.h"
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
class CNcPlank  
{
	UCS_STRU machine_ucs;	//加工坐标系（相对板相对坐标系而言）
public:
	CNcPlank();
	virtual ~CNcPlank();
	BOOL GenNCFile(CLDSGeneralPlate *pPlank,char *sFileName,BOOL bExportDeformedProfile=TRUE);

};
#endif
#endif // !defined(AFX_NCPLANK_H__2D238B60_4DDA_47FF_9D2C_3E8D830E2D93__INCLUDED_)
