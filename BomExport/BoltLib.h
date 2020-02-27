#pragma once
#include "XhCharString.h"
#include "HashTable.h"
#include "BOM.h"

struct BOLT_LIB_REC{
public:
	CXhChar16 m_sSpec;
	int d,L;
	double m_fWeight;
	double m_fWeight2;
	BOLT_LIB_REC(){d=L=0;m_fWeight=m_fWeight2=0;}
};
class CBoltLib
{
public:
	BYTE m_cType;	//ÂÝË¨¿âÀàÐÍ
	CHashList<double> m_xHashWeightByD;	//¼ÇÂ¼Ã¿5mmÖØÁ¿
	CMapList<BOLT_LIB_REC> m_xMapBoltRec;
	CBoltLib(){m_cType=0;}
	double FindWeight(int d,int L,BOOL bTwoCap=FALSE);
};

class CBoltLibSet
{
private:
	bool ReadSheet(const char* file_path,BYTE bolt_type);
	
public:
	CBoltLibSet(void){;}
	~CBoltLibSet(void){;}

	CHashList<CBoltLib> m_xHashBoltsLib;
	static const BYTE TYPE_THIN_NUT		= 1;//±¡ÂÝÄ¸
	static const BYTE TYPE_FASTEN_NUT	= 2;//¿Û½ôÂÝÄ¸

	static const BYTE TYPE_COMMON_L					= 2;	//ÆÕÍ¨ÂÝË¨	Ë«Ã±ÂÝË¨(·ÀËÉ)
	static const BYTE TYPE_COMMON_T					= 6;	//Ë«Ã±ÂÝË¨ £¨·ÀµÁºñÐÍ,1ÂÝÄ¸1·ÀÐ¶ÂÝË¨1Æ½µæÈ¦£©
	static const BYTE TYPE_COMMON_T_THIN			= 30;	//·ÀµÁÂÝË¨ £¨»ù±¾ÂÝË¨Îª±¡ÂÝÄ¸·ÀµÁ£©
	static const BYTE TYPE_COMMON_T_FASTEN			= 31;	//·ÀµÁÂÝË¨ (ÔÝÎ´ÆôÓÃ)
	static const BYTE TYPE_FOOTNAIL_TWO_CAP_L		= 8;	//·ÀËÉ½Å¶¤(½Å¶¤Åä2ÂÝÄ¸1±¡ÂÝÄ¸1Æ½µæÈ¦,½Å¶¤Åä2ÂÝÄ¸1Æ½µæÈ¦1·ÀËÉ¿Û½ôÂÝÄ¸)
	static const BYTE TYPE_FOOTNAIL_TWO_CAP_T		= 7;	//·ÀµÁ½Å¶¤(½Å¶¤Åä2ÂÝÄ¸1·ÀÐ¶ÂÝÄ¸1Æ½µæÈ¦)
	static const BYTE TYPE_FOOTNAIL_THREE_CAP		= 36;	//·ÀµÁ½Å¶¤(½Å¶¤Åä3ÂÝÄ¸1Æ½µæÈ¦)
	static const BYTE TYPE_DIANQUAN					= 15;	//µæÈ¦
	static const BYTE TYPE_DIANQUAN_SPRING			= 37;	//µ¯»ÉµæÈ¦
	double FindWeight(int d,int L,BYTE cType,BOOL bTwoCap=FALSE);
	double FindWeight(BOMBOLT &bolt,BYTE cBaseBoltType);
	bool ImportBoltLibByExcelFile(const char* file_path);
};

