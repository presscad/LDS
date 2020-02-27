#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <math.h>
#include <WinDef.h>
#include "XhCharString.h"

struct CONNBOLTS {
	char grade[5];
	WORD wiBoltD;			//��˨ֱ��
	WORD wnBoltN;			//��˨��
	WORD wiHoleWallThick;	//�ױں��
	double Avf, Ahf;		//��˨��Ч�м����,�ױڳ�ѹ���
	double get_dfCtrlAf() { return min(Avf, Ahf); }	//����ǿ��
	__declspec(property(get=get_dfCtrlAf)) double dfCtrlAf;
	CONNBOLTS(int d=20, int N=2, int thick=0, const char* boltgrade=NULL){
		Init(d, N, thick, boltgrade);
	}
	void Init(int d, int N, int thick=0, const char* boltgrade=NULL)
	{
		this->wiBoltD=(WORD)d;
		this->wnBoltN=(WORD)N;
		this->wiHoleWallThick=(WORD)thick;
		Avf=3.1415926*0.25*d*d*N;	//���������ǿ��ֵ	//Pi=3.1415926
		Ahf=d*wiHoleWallThick*N;	//�ױڳ�ѹǿ��ֵ
		WORD wiGradeLevel=68;
		if(boltgrade!=NULL)
			wiGradeLevel=(WORD)(10*atof(boltgrade)+0.5);
		StrCopy(grade, boltgrade, 5);
		//��Ϊ��˨����ǿ�����㲻������˨����ǿ�Ȼ��ܱ����ӹ������Ͽױڳ�ѹǿ������, �ʳ�4.8����
		//һ���ͳһΪ6.8��,ֻ����˨ֱ��������������˨ǿ�ȵȼ����� wjh-2017.8.24
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

