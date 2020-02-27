#include "stdafx.h"
#include "..\LDS.h"
#include "..\TowerCad\ComparePartNoString.h"
#include "..\ToolCode\XhCharString.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef __ALFA_TEST_VERSION_

int Test_ComparePartNoString(FILE* fp)
{
	fprintf(fp,"Test_ComparePartNoString result:\n");
	int hits=0;
	if(ComparePartNoString("110-89","112-75")>=0)
	{
		fprintf(fp,"'110-89'与'112-75'比较结果错误!\n");
		hits++;
	}
	if(ComparePartNoString("A10-89","12-75")>=0)
	{
		fprintf(fp,"'A10-89'与'12-75'比较结果错误!\n");
		hits++;
	}
	if(ComparePartNoString("A10-89","10-89")<=0)
	{
		fprintf(fp,"'A10-89'与'10-89'比较结果错误!\n");
		hits++;
	}
	if(ComparePartNoString("189","189A")>=0)
	{
		fprintf(fp,"'189'与'189A'比较结果错误!\n");
		hits++;
	}
	if(ComparePartNoString("189D","189A")<=0)
	{
		fprintf(fp,"'189D'与'189A'比较结果错误!\n");
		hits++;
	}
	if(ComparePartNoString("110-89A","110-89C")>=0)
	{
		fprintf(fp,"'110-89A'与'112-89C'比较结果错误!\n");
		hits++;
	}
	if(ComparePartNoString("FL7B","FL7A")<=0)
	{
		fprintf(fp,"FL7B<=FL7A\n");
		hits++;
	}
	if(ComparePartNoString("FL9A","FL8B")<=0)
	{
		fprintf(fp,"FL9A<=FL8B\n");
		hits++;
	}
	if(ComparePartNoString("FL10","FL11A")>=0)
	{
		fprintf(fp,"FL10>=FL11A\n");
		hits++;
	}
	if(ComparePartNoString("FL11B","FL12")>=0)
	{
		fprintf(fp,"FL11B>=FL12\n");
		hits++;
	}
	if(ComparePartNoString("FL11B-2","FL11A-2")<=0)
	{
		fprintf(fp,"FL11B-2<=FL11A-2\n");
		hits++;
	}
	if(ComparePartNoString("FL9A","FL10")>=0)
	{
		fprintf(fp,"FL9A>=FL10\n");
		hits++;
	}
	if(ComparePartNoString("2538","2-106")<=0)
	{
		fprintf(fp,"2538<=2-106\n");
		hits++;
	}
	if(ComparePartNoString("FL7B-2","801")<=0)
	{
		fprintf(fp,"FL7B-2<=801\n");
		hits++;
	}
	/** 暂不支持
	if(ComparePartNoString("FL57","FL7A-2")<=0)
	{
		fprintf(fp,"FL57<=FL7A-2\n");
		hits++;
	}*/
	if(ComparePartNoString("-5","-8")>=0)
	{
		fprintf(fp,"'-5'与'-8'比较结果错误!\n");
		hits++;
	}
	if(ComparePartNoString("6.8M20X50","6.8M20X60")>=0)
	{
		fprintf(fp,"'6.8M20X50'与'6.8M20X60'比较结果错误!\n");
		hits++;
	}
	if(ComparePartNoString("L100X10","L100X12")>=0)
	{
		fprintf(fp,"'L100X10'与'L100X12'比较结果错误!\n");
		hits++;
	}
	return hits;
}

int Test_CompareMultiSectionString(FILE* fp)
{
	fprintf(fp,"Test_CompareMultiSectionString result:\n");
	int hits=0;
	if(CompareMultiSectionString("0X163","0XFFF")>=0)
	{
		fprintf(fp,"'0X163'与'0XFFF'比较结果错误!\n");
		hits++;
	}
	if(CompareMultiSectionString("-5","-8")>=0)
	{
		fprintf(fp,"'-5'与'-8'比较结果错误!\n");
		hits++;
	}
	if(CompareMultiSectionString("6.8M20X50","6.8M20X60")>=0)
	{
		fprintf(fp,"'6.8M20X50'与'6.8M20X60'比较结果错误!\n");
		hits++;
	}
	if(CompareMultiSectionString("L100X10","L100X12")>=0)
	{
		fprintf(fp,"'L100X10'与'L100X12'比较结果错误!\n");
		hits++;
	}
	return hits;
}
int Test_XhCharStrToKey(FILE* fp)
{
	fprintf(fp,"Test_XhCharStrToKey result:\n");
	int hits=0;
	//1.
	char str[100]="4.1.2.5";
	char str2[100]="4.1.2.5";
	char *sub_str=strtok(str,"");
	CXhString xhStr;
	char *sub_xhStr=xhStr.StrToKey(str2,"");
    while (sub_str!=NULL&&sub_xhStr!=NULL)
    {
		if (strcmp(sub_str,sub_xhStr)!=0)
		{
			fprintf(fp,"'StrToKey'与'strtok'两个函数对'4.1.2.5'的''分隔运算结果不一致!\n");
			hits++;
			break;
		}
		sub_str=strtok(NULL,"");
		sub_xhStr=xhStr.StrToKey(NULL,"");
	}
	//2.
	strcpy(str,"4.1.2.5");
	strcpy(str2,"4.1.2.5");
	sub_str=strtok(str,".");
	sub_xhStr=xhStr.StrToKey(str2,".");
	while (sub_str!=NULL&&sub_xhStr!=NULL)
	{
		if (strcmp(sub_str,sub_xhStr)!=0)
		{
			fprintf(fp,"'StrToKey'与'strtok'两个函数对'4.1.2.5'的'.'分隔运算结果不一致!\n");
			hits++;
			break;
		}
		sub_str=strtok(NULL,".");
		sub_xhStr=xhStr.StrToKey(NULL,".");
	}
	//3.
	strcpy(str,"4,1.,.2..5");
	strcpy(str2,"4,1.,.2..5");
	sub_str=strtok(str,",.2");
	sub_xhStr=xhStr.StrToKey(str2,",.2");
	while (sub_str!=NULL&&sub_xhStr!=NULL)
	{
		if (strcmp(sub_str,sub_xhStr)!=0)
		{
			fprintf(fp,"'StrToKey'与'strtok'两个函数对'4,1.,.2..5'的',.2'分隔运算结果不一致!\n");
			hits++;
			break;
		}
		sub_str=strtok(NULL,",.2");
		sub_xhStr=xhStr.StrToKey(NULL,",.2");
	}
	//4.
	strcpy(str,".4,1.,.2..5.");
	strcpy(str2,".4,1.,.2..5.");
	sub_str=strtok(str,",.");
	sub_xhStr=xhStr.StrToKey(str2,",.");
	while (sub_str!=NULL&&sub_xhStr!=NULL)
	{
		if (strcmp(sub_str,sub_xhStr)!=0)
		{
			fprintf(fp,"'StrToKey'与'strtok'两个函数对'.4,1.,.2..5.'的',.'分隔运算结果不一致!\n");
			hits++;
			break;
		}
		sub_str=strtok(NULL,",.");
		sub_xhStr=xhStr.StrToKey(NULL,",.");
	}
	return hits;
}
#endif