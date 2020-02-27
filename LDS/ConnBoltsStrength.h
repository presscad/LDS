#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <math.h>
#include <WinDef.h>
#include "XhCharString.h"

struct CONNBOLTS {
	char grade[5];
	WORD wiBoltD;			//螺栓直径
	WORD wnBoltN;			//螺栓数
	WORD wiHoleWallThick;	//孔壁厚度
	double Avf, Ahf;		//螺栓有效承剪面积,孔壁承压面积
	double get_dfCtrlAf() { return min(Avf, Ahf); }	//控制强度
	__declspec(property(get=get_dfCtrlAf)) double dfCtrlAf;
	CONNBOLTS(int d=20, int N=2, int thick=0, const char* boltgrade=NULL){
		Init(d, N, thick, boltgrade);
	}
	void Init(int d, int N, int thick=0, const char* boltgrade=NULL)
	{
		this->wiBoltD=(WORD)d;
		this->wnBoltN=(WORD)N;
		this->wiHoleWallThick=(WORD)thick;
		Avf=3.1415926*0.25*d*d*N;	//抗剪截面积强度值	//Pi=3.1415926
		Ahf=d*wiHoleWallThick*N;	//孔壁承压强度值
		WORD wiGradeLevel=68;
		if(boltgrade!=NULL)
			wiGradeLevel=(WORD)(10*atof(boltgrade)+0.5);
		StrCopy(grade, boltgrade, 5);
		//因为螺栓连接强度验算不仅受螺栓本身强度还受被连接构件材料孔壁承压强度限制, 故除4.8级外
		//一般可统一为6.8级,只比螺栓直径及数量不计螺栓强度等级因素 wjh-2017.8.24
		if(wiGradeLevel==48)
		{
			Avf*=170;
			Ahf*=420;
		}
		else if(wiGradeLevel==68)
		{
			Avf*=240;
			Ahf*=600;
		}
		else if(wiGradeLevel==88)
		{
			Avf*=300;
			Ahf*=800;
		}
		else if (d <= 20)
		{
			Avf *= 240;
			Ahf *= 600;
		}
		else //if (d >= 24)
		{
			Avf *= 300;
			Ahf *= 800;
		}
	}
	void Clear(){
		Avf=Ahf=wiBoltD=wnBoltN=wiHoleWallThick=0;
	}
};

