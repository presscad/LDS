#include "stdafx.h"
#include "database.h"
#include "Query.h"
#include "SortFunc.h"
#include "XhCharString.h"

/////////////////////////////////////////////////////////////////////////////
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

BOOL restore_Ls_guige(const char* guige, double* pGrade,long* pD, long* pL )
{
	CXhChar50 temstr;
	long d,L;
	char grade[5]="";
	temstr.Copy(guige);
	char* splitter=strchr(temstr,'M');
	size_t pos=splitter-temstr;
	if(splitter==NULL)
		return FALSE;
	*splitter='\0';
	splitter+=1;
	if(pos>0)	//包含级别信息
		strncpy(grade,temstr,4);
	char* key=strtok(splitter,"Xx");
	if(key!=NULL&&strlen(key)>0)
		d=atoi(key);
	else
		return FALSE;
	key=strtok(NULL,"Xx");
	if(key!=NULL&&strlen(key)>0)
		L=atoi(key);
	else
		return FALSE;
	if(pGrade)
		*pGrade=atof(grade);
	if(pD)
		*pD=d;
	if(pL)
		*pL=L;
	return TRUE;
}
BOOL restore_Ls_guige(const char* guige, CLDSBolt& ls)
{
	long d,L;
	double grade;
	if (!restore_Ls_guige(guige, &grade, &d, &L))
		return false;
	ls.SetGrade(CXhChar16("%.1f",grade));
	ls.set_d(d);
	ls.set_L(L);
	return TRUE;
}
/*
以下代码效率低，稳定性差，被以上代码取代，等以上代码稳定后，此代码可删除 wjh-2011.10.9
{
    int i;
    char d[8];
    char Len[8];
	if(guige.GetLength()<5)	//非法的螺栓规格
		return FALSE;
				//  012345678	
	//规格格式举例：6.8M20X60	长度位有可能不是2位
	if(toupper(guige[3])=='M')
	{
		if(guige.GetLength()<5)
			return FALSE;
		d[0] = guige[4];
		d[1] = guige[5];
		d[2] = '\0';
		for(i=7;i<guige.GetLength();i++)
			Len[i-7] = guige[i];
		Len[i-7]='\0';
		ls.set_d(atoi(d));
		ls.set_L(atoi(Len));
		ls.grade[0] = guige[0];
		ls.grade[1] = guige[1];
		ls.grade[2] = guige[2];
		ls.grade[3] = '\0';
	}
	else	//这里处理不带有级别的螺栓规格如M16X60
	{
		if(guige.GetLength()<4)
			return FALSE;
		d[0] = guige[1];
		d[1] = guige[2];
		d[2] = '\0';
		for(i=4;i<guige.GetLength();i++)
			Len[i-4] = guige[i];
		Len[i-4]='\0';
		ls.set_d(atoi(d));
		ls.set_L(atoi(Len));
		if(ls.get_d()==12)
			strcpy(ls.grade,"4.8");
		else if(ls.get_d()==16)
			strcpy(ls.grade,"4.8");
		else if(ls.get_d()==20)
			strcpy(ls.grade,"6.8");
		else if(ls.get_d()==24)
			strcpy(ls.grade,"6.8");
	}
	return TRUE;
}*/
#endif
void AddSteelMatRecord(CComboBox *pCMB)
{
	int i,j,n=pCMB->GetCount();
	for(i=0;i<n;i++)
		pCMB->DeleteString(0);
	for(j=0;j<CSteelMatLibrary::GetCount();j++)
	{
		STEELMAT mat=CSteelMatLibrary::RecordAt(j);
		if(pCMB->FindString(-1,mat.mark)==CB_ERR)
		{
			int iNow=pCMB->AddString(mat.mark);
			pCMB->SetItemData(iNow,mat.cBriefMark);
		}
	}
}
void AddJgRecord(CComboBox *pCMB)
{
	for(int i=0;i<jgguige_N;i++)
	{
		CString type;
		if( (int)(jgguige_table[i].wing_wide*10)%10>0||
			(int)(jgguige_table[i].wing_thick*10)%10>0)
		{
			type.Format("%.1fX%.1f",	
				jgguige_table[i].wing_wide, jgguige_table[i].wing_thick);
		}
		else
		{
			type.Format("%.0fX%.0f",	
				jgguige_table[i].wing_wide, jgguige_table[i].wing_thick);
		}

		if(pCMB->FindString(-1,type)==CB_ERR)
			pCMB->AddString(type);
	}
}
void AddTubeRecord(CComboBox *pCMB,BOOL bOnlyAddTubeD/*=FALSE*/)
{
	pCMB->ResetContent();
	for(int i=0;i<tubeguige_N;i++)
	{
		CString type;
		if(bOnlyAddTubeD)
		{
			if((int)(tubeguige_table[i].D*10)%10>0)
				type.Format("%.1f",tubeguige_table[i].D);
			else 
				type.Format("%.0f",tubeguige_table[i].D);
		}
		else
		{
			if((int)(tubeguige_table[i].D*10)%10>0&&(int)(tubeguige_table[i].thick*10)%10==0)
			{
				type.Format("%.1fX%.0f",	
					tubeguige_table[i].D, tubeguige_table[i].thick);
			}
			else if((int)(tubeguige_table[i].D*10)%10==0&&(int)(tubeguige_table[i].thick*10)%10>0)
			{
				type.Format("%.0fX%.1f",	
					tubeguige_table[i].D, tubeguige_table[i].thick);
			}
			else if((int)(tubeguige_table[i].D*10)%10>0&&(int)(tubeguige_table[i].thick*10)%10>0)
			{
				type.Format("%.1fX%.1f",	
					tubeguige_table[i].D, tubeguige_table[i].thick);
			}
			else
			{
				type.Format("%.0fX%.0f",	
					tubeguige_table[i].D, tubeguige_table[i].thick);
			}
		}	
		if(pCMB->FindString(-1,type)==CB_ERR)
			pCMB->AddString(type);
	}
}
void AddBianRecord(CComboBox *pCMB)
{
	pCMB->ResetContent();
	for(int i=0;i<bianguige_N;i++)
	{
		CString type;
		if((int)(bianguige_table[i].wide*10)%10>0&&(int)(bianguige_table[i].thick*10)%10==0)
		{
			type.Format("%.1fX%.0f",	
				bianguige_table[i].wide, bianguige_table[i].thick);
		}
		else if((int)(bianguige_table[i].wide*10)%10==0&&(int)(bianguige_table[i].thick*10)%10>0)
		{
			type.Format("%.0fX%.1f",	
				bianguige_table[i].wide, bianguige_table[i].thick);
		}
		else if((int)(bianguige_table[i].wide*10)%10>0&&(int)(bianguige_table[i].thick*10)%10>0)
		{
			type.Format("%.1fX%.1f",	
				bianguige_table[i].wide, bianguige_table[i].thick);
		}
		else
		{
			type.Format("%.0fX%.0f",	
				bianguige_table[i].wide, bianguige_table[i].thick);
		}

		if(pCMB->FindString(-1,type)==CB_ERR)
			pCMB->AddString(type);
	}
}

void AddLsRecord(CComboBox* pCMB,long hLsFamily,BOOL bOnlyDiameter/*=FALSE*/ )
{
	pCMB->ResetContent();
	while(pCMB->GetCount()>0)
		pCMB->DeleteString(0);
	CLsFamily *pLsFamily=NULL;
	if(hLsFamily>0)
		pLsFamily=CLsLibrary::FromHandle(hLsFamily);
	if(pLsFamily==NULL)
		pLsFamily=CLsLibrary::DefaultFamily();
	if(pLsFamily)
	{
		INT_ARRAY diameterArr(pLsFamily->GetCount());
		for(int i=0;i<pLsFamily->GetCount();i++)
		{
			CString type;
			if(bOnlyDiameter)
				diameterArr[i]=pLsFamily->RecordAt(i)->d;
			else
			{
				type.Format("%s",pLsFamily->RecordAt(i)->guige);
				if(pCMB->FindString(-1,type)==CB_ERR)
					pCMB->AddString(type);
			}
		}
		if(bOnlyDiameter)
		{
			CBubbleSort<long>::BubSort(diameterArr,pLsFamily->GetCount(),compare_long);
			for(int i=0;i<pLsFamily->GetCount();i++)
			{
				CString str;
				str.Format("%d",diameterArr[i]);
				if(pCMB->FindString(-1,str)==CB_ERR)
					pCMB->AddString(str);
			}
		}
	}

}

void AddSlotRecord(CComboBox* pCMB)
{
	pCMB->ResetContent();
	for(int i=0;i<slotguige_N;i++)
	{
		CString type;
		type.Format("%s",	
			slotguige_table[i].guige);
		if(pCMB->FindString(-1,type)==CB_ERR)
			pCMB->AddString(type);
	}
}
