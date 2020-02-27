//<LOCALE_TRANSLATE Confirm by hxr/>
// RevisionOperDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "RevisionOperDlg.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "Expression.h"
#include ".\revisionoperdlg.h"
#include ".\ExportRptFileOptionDlg.h"
#include ".\PostProcessReport.h"
#include ".\PostProcess.h"
#include "PreProcessor.h"
#include "SortFunc.h"
#include "MainFrm.h"

#if defined(__LDS_)||defined(__TSA_)
// CRevisionOperDlg 对话框

IMPLEMENT_DYNCREATE(CRevisionOperDlg, CDialog)

struct ELEM_ITEM_SORTPARA{
	bool bItemSortType;
	CXhChar50 columnLabel;
}itemsort;
double CalElemCtrlStress(CElement* pElem,double* coefStrength)
{
	double ctrlstress=0;
	double A=pElem->pOrgPole->GetArea();
	double An=A-pElem->design_info.hole_out*pElem->design_info.hole_d*pElem->pOrgPole->GetThick();
	if(pElem->design_info.iDangerousLoadCase==2)		//T
	{
		if(coefStrength)
			*coefStrength=pElem->design_info.fCoefTensStrengthDecrease;
		ctrlstress=pElem->MaxTension.N/An;
	}
	else if(pElem->design_info.iDangerousLoadCase==3)	//C
	{
		if(coefStrength)
			*coefStrength=pElem->design_info.fCoefCompStrengthDecrease;
		ctrlstress=pElem->MaxCompression.N/An;
	}
	else //if(pElem->design_info.iDangerousLoadCase==4)	//S
	{
		if(coefStrength)
			*coefStrength=pElem->design_info.mN;
		if(pElem->MaxCompression.N>-EPS||pElem->design_info.fCoefStablity<EPS)
			ctrlstress=0;
		else	//由于最大压稳应力出现工况不一定与最大压稳力工况一致，故不能用最大压稳力除压稳应力控制工况下的压稳系数 wjh-2013.10.15
			ctrlstress=pElem->MaxCompStable.N/(A*pElem->design_info.fCoefStablity);
	}
	return ctrlstress;
}
int CompareElemValue(const ELEM_PTR& elemptr1,const ELEM_PTR& elemptr2)
{
	double value[2]={0,0};
#ifdef AFX_TARG_ENU_ENGLISH
	if(itemsort.columnLabel.Equal("unit mark"))
#else
	if(itemsort.columnLabel.Equal("单元标识"))
#endif
	{
		int minpointi[2],maxpointi[2];
		minpointi[0]=min(elemptr1->pStart->point_i,elemptr1->pEnd->point_i);
		maxpointi[0]=max(elemptr1->pStart->point_i,elemptr1->pEnd->point_i);
		minpointi[1]=min(elemptr2->pStart->point_i,elemptr2->pEnd->point_i);
		maxpointi[1]=max(elemptr2->pStart->point_i,elemptr2->pEnd->point_i);
		int result=0;
		if(minpointi[0]<minpointi[1])
			result=-1;
		else if(minpointi[0]>minpointi[1])
			result=1;
		else if(maxpointi[0]<maxpointi[1])
			result=-1;
		else if(maxpointi[0]>maxpointi[1])
			result=1;
		if(!itemsort.bItemSortType)	//降序排列
			result=-result;
		return result;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(itemsort.columnLabel.Equal("specification"))
#else
	else if(itemsort.columnLabel.Equal("规  格"))
#endif
	{
		int result=0;
		if(elemptr1->pOrgPole->size_idClassType>elemptr2->pOrgPole->size_idClassType)
			result=1;
		else if(elemptr1->pOrgPole->size_idClassType<elemptr2->pOrgPole->size_idClassType)
			result=-1;
		else if(elemptr1->pOrgPole->size_idClassType==CLS_CABLE)
		{
			result=stricmp(elemptr1->pOrgPole->GetSpec(),elemptr2->pOrgPole->GetSpec());
		}
		else
		{
			if(elemptr1->pOrgPole->size_wide>elemptr2->pOrgPole->size_wide)
				result=1;
			else if(elemptr1->pOrgPole->size_wide<elemptr2->pOrgPole->size_wide)
				result=-1;
			else if(elemptr1->pOrgPole->size_thick>elemptr2->pOrgPole->size_thick)
				result=1;
			else if(elemptr1->pOrgPole->size_thick<elemptr2->pOrgPole->size_thick)
				result=-1;
			if(elemptr1->pOrgPole->size_idClassType==CLS_LINEANGLE)
			{
				char cSubClassType1=elemptr1->pOrgPole->size_cSubClassType;
				char cSubClassType2=elemptr2->pOrgPole->size_cSubClassType;
				if(cSubClassType1=='L')
					cSubClassType1=0;
				if(cSubClassType2=='L')
					cSubClassType2=0;
				if(cSubClassType1>cSubClassType2)
					result=1;
				else if(cSubClassType1<cSubClassType2)
					result=-1;
			}
		}
		if(!itemsort.bItemSortType)	//降序排列
			result=-result;
		return result;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(itemsort.columnLabel.Equal("area"))
#else
	else if(itemsort.columnLabel.Equal("截面积"))
#endif
	{
		int result=0;
		if(elemptr1->pOrgPole->GetArea()>elemptr2->pOrgPole->GetArea())
			result=1;
		else if(elemptr1->pOrgPole->GetArea()<elemptr2->pOrgPole->GetArea())
			result=-1;
		if(!itemsort.bItemSortType)	//降序排列
			result=-result;
		return result;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(itemsort.columnLabel.Equal("steel material"))
#else
	else if(itemsort.columnLabel.Equal("材质"))
#endif
	{
		int result=0;
		CXhChar16 matMark1,matMark2;
		QuerySteelMatMark(elemptr1->pOrgPole->cMaterial,matMark1);
		QuerySteelMatMark(elemptr2->pOrgPole->cMaterial,matMark2);
		result=stricmp(matMark1,matMark2);
		if(!itemsort.bItemSortType)	//降序排列
			result=-result;
		return result;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(itemsort.columnLabel.Equal("reduce hole numbers"))
#else
	else if(itemsort.columnLabel.Equal("减孔数"))
#endif
	{
		value[0]=value[1]=-1;
		if(elemptr1->pStart==elemptr1->pOrgPole->pStart||elemptr1->pEnd==elemptr1->pOrgPole->pEnd)
			value[0]=elemptr1->pOrgPole->hole_out;
		if(elemptr2->pStart==elemptr2->pOrgPole->pStart||elemptr2->pEnd==elemptr2->pOrgPole->pEnd)
			value[1]=elemptr2->pOrgPole->connectStart.bearloadcoef;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(itemsort.columnLabel.Equal("bolt's efficiency"))
#else
	else if(itemsort.columnLabel.Equal("螺栓效率"))
#endif
	{
		value[0]=value[1]=-1;
		if(elemptr1->pStart==elemptr1->pOrgPole->pStart)
			value[0]=elemptr1->pOrgPole->connectStart.bearloadcoef;
		else if(elemptr1->pEnd==elemptr1->pOrgPole->pEnd)
			value[0]=elemptr1->pOrgPole->connectEnd.bearloadcoef;
		if(elemptr2->pStart==elemptr2->pOrgPole->pStart)
			value[1]=elemptr2->pOrgPole->connectStart.bearloadcoef;
		else if(elemptr2->pEnd==elemptr2->pOrgPole->pEnd)
			value[1]=elemptr2->pOrgPole->connectEnd.bearloadcoef;
	}
	else if(itemsort.columnLabel.Equal("KLo/r"))
	{
		value[0]=elemptr1->Lamda();
		value[1]=elemptr2->Lamda();
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(itemsort.columnLabel.Equal("elem effic"))
#else
	else if(itemsort.columnLabel.Equal("单元效率"))
#endif
	{
		if(g_sysPara.iEfficacyCeofType==0)
		{
			value[0]=elemptr1->design_info.fCoefEffic;
			value[1]=elemptr2->design_info.fCoefEffic;
		}
		else
		{
			double coefStrength=1.0;
			double ctrlstress=CalElemCtrlStress(elemptr1,&coefStrength);
			STEELMAT *pSteelMat=QuerySteelMatByBriefMark(elemptr1->pOrgPole->cMaterial);
			double limStrength=pSteelMat->f(elemptr1->GetThick())*coefStrength;
			value[0]=0;
			if(coefStrength>EPS)
				value[0]=fabs(ctrlstress)/limStrength;

			coefStrength=1.0;
			ctrlstress=CalElemCtrlStress(elemptr2,&coefStrength);
			pSteelMat=QuerySteelMatByBriefMark(elemptr2->pOrgPole->cMaterial);
			limStrength=pSteelMat->f(elemptr2->GetThick())*coefStrength;
			value[1]=0;
			if(coefStrength>EPS)
				value[1]=fabs(ctrlstress)/limStrength;
		}
	}
	else if(itemsort.columnLabel.Equal("σmax"))
	{
		value[0]=fabs(CalElemCtrlStress(elemptr1,NULL));
		value[1]=fabs(CalElemCtrlStress(elemptr2,NULL));
	}
	else if(itemsort.columnLabel.Equal("+Nmax"))
	{
		value[0]=elemptr1->MaxTension.N;
		value[1]=elemptr2->MaxTension.N;
	}
	else if(itemsort.columnLabel.Equal("-Nmax"))
	{
		value[0]=-elemptr1->MaxCompression.N;
		value[1]=-elemptr2->MaxCompression.N;
	}
	else if(itemsort.columnLabel.Equal("-Nmaxφ"))
	{
		value[0]=-elemptr1->MaxCompStable.N;
		value[1]=-elemptr2->MaxCompStable.N;
	}
	else	//表示不支持
		return -2;
	int result=0;
	if(value[0]<value[1])
		result=-1;
	else if(value[0]>value[1])
		result=1;
	if(!itemsort.bItemSortType)	//降序排列
		result=-result;
	return result;
}
static int _LocalFireCompareItem(const CSuperGridCtrl::CSuperGridCtrlItemPtr& pItem1,const CSuperGridCtrl::CSuperGridCtrlItemPtr& pItem2,DWORD lPara)
{
	COMPARE_FUNC_EXPARA* pExPara=(COMPARE_FUNC_EXPARA*)lPara;
	BOOL bAscending=pExPara==NULL?true:pExPara->bAscending;
	ELEM_PTR pElem1=postreport.hashElems.GetValue(pItem1->m_idProp);
	ELEM_PTR pElem2=postreport.hashElems.GetValue(pItem2->m_idProp);
	int status=CompareElemValue(pElem1,pElem2);
	if(status!=-2)
		return status;
	else
	{
		CString itemText1=pItem1->m_lpNodeInfo->GetSubItemText(pExPara->iSubItem);
		CString itemText2=pItem2->m_lpNodeInfo->GetSubItemText(pExPara->iSubItem);
		status=itemText1.Compare(itemText2);
		return bAscending?status:-status;
	}
}
static BOOL _LocalFireColumnClick(CSuperGridCtrl* pListCtrl,int iSubItem,bool bAscending)
{
	itemsort.bItemSortType=(bool)bAscending;
	itemsort.columnLabel=pListCtrl->GetColumnInfo(iSubItem)->headerName;
	return TRUE;
}
static CHashStrList<PROPLIST_ITEM> propHashtable;	//属性字符串哈希表
static PROPLIST_ITEM* AddPropItem(char* propKey,PROPLIST_ITEM item){
	PROPLIST_ITEM* pItem=propHashtable.SetValue(propKey,item);
	pItem->propKeyStr.Copy(propKey);
	return pItem;
}
//设定标识位
static DWORD ItemFlag(const char* itemKeyStr,DWORD flag)
{
	PROPLIST_ITEM* pItem=propHashtable.GetValue(itemKeyStr);
	if(pItem)
		pItem->dwFlag=flag;
	else
		flag=0;
	return flag;
}
//返回标识位，同时设定标识索引
static DWORD ItemFlag(int id,const char* itemKeyStr)
{
	PROPLIST_ITEM* pItem=propHashtable.GetValue(itemKeyStr);
	if(pItem)
	{
		pItem->id=id;
		return pItem->dwFlag;
	}
	else
		return 0;
}
void CRevisionOperDlg::InitPropItemHashtable()	//初始肘板型肋板各参数的基础信息(数据键值、类型、属性名称及帮助提示）
{
	int id = 1;
	propHashtable.SetHashTableGrowSize(30);
	AddPropItem("elemid",PROPLIST_ITEM(id++,"单元标识"))->dwFlag=0x07;
	AddPropItem("symmetry",PROPLIST_ITEM(id++,"对称"))->dwFlag=0x01;
	AddPropItem("layer",PROPLIST_ITEM(id++,"图层"))->dwFlag=0x07;
	AddPropItem("type",PROPLIST_ITEM(id++,"规格类型"))->dwFlag=0x01;
	AddPropItem("spec",PROPLIST_ITEM(id++,"规格"))->dwFlag=0x07;
	AddPropItem("area",PROPLIST_ITEM(id++,"截面积"))->dwFlag=0x01;
	AddPropItem("material",PROPLIST_ITEM(id++,"材质"))->dwFlag=0x07;
	AddPropItem("StatMatI",PROPLIST_ITEM(id++,"统材号"))->dwFlag=0x01;
	AddPropItem("HorR",PROPLIST_ITEM(id++,"端部约束"))->dwFlag=0x06;
	AddPropItem("EndState",PROPLIST_ITEM(id++,"杆端状态"))->dwFlag=0x02;
	AddPropItem("BoltInfo",PROPLIST_ITEM(id++,"螺栓信息"))->dwFlag=0x01;
	AddPropItem("BoltEffic",PROPLIST_ITEM(id++,"螺栓效率"))->dwFlag=0x01;
	AddPropItem("HoleReduct",PROPLIST_ITEM(id++,"减孔"))->dwFlag=0x01;
	AddPropItem("μ",PROPLIST_ITEM(id++,"计算长度系数"))->dwFlag=0x04;
	AddPropItem("Lo",PROPLIST_ITEM(id++,"计算长度"))->dwFlag=0x04;
	AddPropItem("R",PROPLIST_ITEM(id++,"回转半径"))->dwFlag=0x04;
	AddPropItem("KLo/r",PROPLIST_ITEM(id++,"计算长细比"))->dwFlag=0x04;
	AddPropItem("+Nmax",PROPLIST_ITEM(id++,"最大拉力"))->dwFlag=0x02;
	AddPropItem("+Case",PROPLIST_ITEM(id++,"最大拉力工况"))->dwFlag=0x02;
	AddPropItem("-Nmax",PROPLIST_ITEM(id++,"最大压力"))->dwFlag=0x06;
	AddPropItem("-Case",PROPLIST_ITEM(id++,"最大压力工况"))->dwFlag=0x06;
	AddPropItem("-Nmaxφ",PROPLIST_ITEM(id++,"最大压稳力"))->dwFlag=0x04;
	AddPropItem("-Caseφ",PROPLIST_ITEM(id++,"最大压稳工况"))->dwFlag=0x04;
	AddPropItem("σmax",PROPLIST_ITEM(id++,"应力计算值"))->dwFlag=0x06;
	AddPropItem("σa",PROPLIST_ITEM(id++,"应力允许值"))->dwFlag=0x06;
	AddPropItem(" m",PROPLIST_ITEM(id++,"强度折减系数"))->dwFlag=0x02;
	AddPropItem("φ",PROPLIST_ITEM(id++,"压稳系数"))->dwFlag=0x04;
	AddPropItem("ElemEffic",PROPLIST_ITEM(id++,"单元承载效率"))->dwFlag=0x06;
}
static bool InitFromElemRevCfgFile()
{
	CXhChar200 revision_cfg_file("%srevelem.cfg",theApp.szExecPath);
	CFileFind fileFind;
	if(!fileFind.FindFile(revision_cfg_file))
		return false;
	FILE* fp=fopen(revision_cfg_file,"rb");
	if(fp==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox(CXhChar200("Element revision configuration files{%s} write failed!",(char*)revision_cfg_file));
#else 
		AfxMessageBox(CXhChar200("单元校审配置文件{%s}写入失败,Win7、Win8及更高版本系统请设置以管理员身份运行软件!",(char*)revision_cfg_file));
#endif
		return false;
	}
	DWORD version=0,dwContentLength=0;
	fread(&version,4,1,fp);
	fread(&dwContentLength,4,1,fp);
	CHAR_ARRAY pool(dwContentLength);
	CBuffer buff(pool,dwContentLength);
	fread(pool,dwContentLength,1,fp);
	int i,n=0;
	buff.ReadInteger(&n);
	CXhChar50 keystr;
	for(i=0;i<n;i++)
	{
		buff.ReadString(keystr,51);
		PROPLIST_ITEM* pPropItem=propHashtable.GetValue(keystr);
		DWORD dwFlag=buff.ReadDword();
		if(pPropItem)
			pPropItem->dwFlag=dwFlag;
	}
	fclose(fp);
	return true;
}
static bool WriteToElemRevCfgFile()
{
	CXhChar200 revision_cfg_file("%srevelem.cfg",theApp.szExecPath);
	FILE* fp=fopen(revision_cfg_file,"wb");
	if(fp==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox(CXhChar200("Element revision configuration files{%s} write failed!",(char*)revision_cfg_file));
#else 
		AfxMessageBox(CXhChar200("单元校审配置文件{%s}写入失败,Win7、Win8及更高版本系统请设置以管理员身份运行软件!",(char*)revision_cfg_file));
#endif
		return false;
	}
	DWORD version=0;
	fwrite(&version,4,1,fp);
	CBuffer buff;
	buff.WriteInteger(propHashtable.GetNodeNum());
	for(PROPLIST_ITEM* pPropItem=propHashtable.GetFirst();pPropItem;pPropItem=propHashtable.GetNext())
	{
		buff.WriteString(pPropItem->propKeyStr,50);
		buff.WriteDword(pPropItem->dwFlag);
	}
	DWORD dwContentLength=buff.GetLength();
	fwrite(&dwContentLength,4,1,fp);
	fwrite(buff.GetBufferPtr(),dwContentLength,1,fp);
	fclose(fp);
	return true;
}
CRevisionOperDlg::CRevisionOperDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRevisionOperDlg::IDD, pParent)
{
	int i=-1;
	if(propHashtable.GetNodeNum()==0)
		InitPropItemHashtable();	//初始肘板型肋板各参数的基础信息(数据键值、类型、属性名称及帮助提示）
	InitFromElemRevCfgFile();	//读取之前的默认配置文件
#ifdef AFX_TARG_ENU_ENGLISH
	i=m_listElems.AddColumnHeader("unit mark",90,ItemFlag(i+1,"elemid"),"two end's nodes number of the unit ");
	i=m_listElems.AddColumnHeader("symmetry",40,ItemFlag(i+1,"symmetry"),"unit's symmetry information");
	i=m_listElems.AddColumnHeader("layer",40,ItemFlag(i+1,"layer"),"use the second character of the layer name to mark rod's function,for example:Z:steel material,X:V-titled material,H:across material and F:auxiliary material.");
	i=m_listElems.AddColumnHeader("type",40,ItemFlag(i+1,"type"),"rod's material type,for example L:angle,Φ:tube.");//、[:U-steel
	i=m_listElems.AddColumnHeader("spec",65,ItemFlag(i+1,"spec"));
	i=m_listElems.AddColumnHeader("area",65,ItemFlag(i+1,"area"));
	i=m_listElems.AddColumnHeader("material",55,ItemFlag(i+1,"material"));
	i=m_listElems.AddColumnHeader("BOM",40,ItemFlag(i+1,"StatMatI"),"the mark numbers used to unify a group of rods,\"0X\" begining shows belonging to the same rod.");//、[:U-steel
	i=m_listElems.AddColumnHeader("H,R",35,ItemFlag(i+1,"HorR"),"H:articulate,R:rigid connect(with partly rotation limit).");
	i=m_listElems.AddColumnHeader("rod's end situation",65,ItemFlag(i+1,"EndState"),"F:no limitation for rotation; P:partly limitation for rotation; C:center stressed; E:Eccentric Force。");
	i=m_listElems.AddColumnHeader("bolt info",80,ItemFlag(i+1,"BoltInfo"));
	i=m_listElems.AddColumnHeader("bolt efficiency",70,ItemFlag(i+1,"BoltEffic"),"bolt actual maximum load /limit load ratio");
	i=m_listElems.AddColumnHeader("reduce hole number",60,ItemFlag(i+1,"HoleReduct"));
	i=m_listElems.AddColumnHeader("μ",50,ItemFlag(i+1,"μ"),"ratio between unit computation length under controlling condition and original length");
	i=m_listElems.AddColumnHeader("Lo",50,ItemFlag(i+1,"Lo"),"unit computation length under controlling condition,mm");
	i=m_listElems.AddColumnHeader("R",50,ItemFlag(i+1,"R"),"Current active gyration,mm");
	i=m_listElems.AddColumnHeader("KLo/r",60,ItemFlag(i+1,"KLo/r"),"the slenderness ratio after Considering the correction coefficient of Angle part");
	i=m_listElems.AddColumnHeader("+Nmax",60,ItemFlag(i+1,"+Nmax"),"maximum pull, kN");
	i=m_listElems.AddColumnHeader("+Case",60,ItemFlag(i+1,"+Case"),"module and working condition group when the maximum pull appears");
	i=m_listElems.AddColumnHeader("-Nmax",65,ItemFlag(i+1,"-Nmax"),"the maximum stress, kN");
	i=m_listElems.AddColumnHeader("-Case",60,ItemFlag(i+1,"-Case"),"module and working condition group when the maximum stress appears");
	i=m_listElems.AddColumnHeader("-Nmaxφ",65,ItemFlag(i+1,"-Nmaxφ"),"pressure when maximum pressure steady stress appears, kN");
	i=m_listElems.AddColumnHeader("-Caseφ",60,ItemFlag(i+1,"-Caseφ"),"module and working condition group when maximum pressure steady stress appears");
	i=m_listElems.AddColumnHeader("σmax",70,ItemFlag(i+1,"σmax"),"maximum design stress, MPa");
	i=m_listElems.AddColumnHeader("σa",70,ItemFlag(i+1,"σa"),"permissible stress, MPa");
	i=m_listElems.AddColumnHeader(" m",45,ItemFlag(i+1," m"),"strength reduction factor under Control working condition");
	i=m_listElems.AddColumnHeader("φ",50,ItemFlag(i+1,"φ"),"Axial compression rod stability coefficient");
	i=m_listElems.AddColumnHeader("elem effic",70,ItemFlag(i+1,"ElemEffic"),"under Control working condition,  ratio coefficient of index calculating value and the ultimate value (including slenderness ratio)，should be less than 1.0");
#else
	i=m_listElems.AddColumnHeader("单元标识",90,ItemFlag(i+1,"elemid"),"单元的两端节点编号");
	i=m_listElems.AddColumnHeader("对称",40,ItemFlag(i+1,"symmetry"),"单元的对称信息");
	i=m_listElems.AddColumnHeader("图层",40,ItemFlag(i+1,"layer"),"图层名第２个字符，用以标识杆件功能，如Z:主材、X:斜材、H:横材及F:辅材。");
	i=m_listElems.AddColumnHeader("类型",40,ItemFlag(i+1,"type"),"杆件材料类型，如L:角钢、Φ:钢管。");//、[:槽钢
	i=m_listElems.AddColumnHeader("规  格",65,ItemFlag(i+1,"spec"));
	i=m_listElems.AddColumnHeader("截面积",65,ItemFlag(i+1,"area"));
	i=m_listElems.AddColumnHeader("材质",55,ItemFlag(i+1,"material"));
	i=m_listElems.AddColumnHeader("统材",40,ItemFlag(i+1,"StatMatI"),"统一一组杆件时所用的标识编号，\"0X\"打头表示同时归属于某一杆件。");//、[:槽钢
	i=m_listElems.AddColumnHeader("H,R",35,ItemFlag(i+1,"HorR"),"H:铰接,R:刚接(有部分转动约束)。");
	i=m_listElems.AddColumnHeader("杆端状况",65,ItemFlag(i+1,"EndState"),"F:无转动约束; P:部分转动约束; C:中心受力; E:偏心受力。");
	i=m_listElems.AddColumnHeader("螺栓信息",80,ItemFlag(i+1,"BoltInfo"));
	i=m_listElems.AddColumnHeader("螺栓效率",70,ItemFlag(i+1,"BoltEffic"),"螺栓实际最大承受荷载/极限承受荷载比值");
	i=m_listElems.AddColumnHeader("减孔数",60,ItemFlag(i+1,"HoleReduct"));
	i=m_listElems.AddColumnHeader("μ",50,ItemFlag(i+1,"μ"),"控制条件下的单元计算长度与原始长度的比值");
	i=m_listElems.AddColumnHeader("Lo",50,ItemFlag(i+1,"Lo"),"控制条件下的单元计算长度,mm");
	i=m_listElems.AddColumnHeader("R",50,ItemFlag(i+1,"R"),"执行的回转半径,mm");
	i=m_listElems.AddColumnHeader("KLo/r",60,ItemFlag(i+1,"KLo/r"),"考虑角钢构件长细比修正系数后的长细比值");
	i=m_listElems.AddColumnHeader("+Nmax",60,ItemFlag(i+1,"+Nmax"),"最大拉力, kN");
	i=m_listElems.AddColumnHeader("+Case",60,ItemFlag(i+1,"+Case"),"最大拉力出现的呼高-工况组合");
	i=m_listElems.AddColumnHeader("-Nmax",65,ItemFlag(i+1,"-Nmax"),"最大压力, kN");
	i=m_listElems.AddColumnHeader("-Case",60,ItemFlag(i+1,"-Case"),"最大压力出现的呼高-工况组合");
	i=m_listElems.AddColumnHeader("-Nmaxφ",65,ItemFlag(i+1,"-Nmaxφ"),"最大压稳应力出现时的压力, kN");
	i=m_listElems.AddColumnHeader("-Caseφ",60,ItemFlag(i+1,"-Caseφ"),"最大压稳应力出现的呼高-工况组合");
	i=m_listElems.AddColumnHeader("σmax",70,ItemFlag(i+1,"σmax"),"最大设计应力, MPa");
	i=m_listElems.AddColumnHeader("σa",70,ItemFlag(i+1,"σa"),"允许应力, MPa");
	i=m_listElems.AddColumnHeader(" m",45,ItemFlag(i+1," m"),"控制工况控制条件下的强度折减系数");
	i=m_listElems.AddColumnHeader("φ",50,ItemFlag(i+1,"φ"),"轴心受压杆件稳定系数");
	i=m_listElems.AddColumnHeader("单元效率",70,ItemFlag(i+1,"ElemEffic"),"控制条件下，指标计算值与极限值的比值系数(含长细比)，应小于1.0");
#endif
	m_bLockedLocateElemFunc=false;
	m_bLockedUpdateElemFunc=false;
}

CRevisionOperDlg::~CRevisionOperDlg()
{
}

void CRevisionOperDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_CTRL, m_tabCtrlListGroup);
	DDX_Control(pDX, IDC_OUTPUT_INFO_LIST, m_listElems);
	DDX_Control(pDX, IDC_LIST_GROUP_SETTING, m_propList);
	DDX_CBString(pDX, IDC_CMB_ACTIVE_RSLT_ITEM_TITLE, m_sActiveRsltItemKeyStr);
}

BEGIN_MESSAGE_MAP(CRevisionOperDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CTRL, OnTcnSelchangeTabCtrl)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_OUTPUT_INFO_LIST, OnLvnColumnclickOutputInfoList)
	ON_BN_CLICKED(ID_READ_RSLT_ITEM, OnBnClickedReadRsltItem)
	ON_BN_CLICKED(ID_SAVE_NEW_ITEM, OnBnClickedSaveNewItem)
	ON_BN_CLICKED(ID_DEL_RSLT_ITEM, OnBnClickedDelRsltItem)
	ON_BN_CLICKED(ID_EXPORT_RSLT_RPT_FILE, OnBnClickedExportRsltRptFile)
END_MESSAGE_MAP()

void CRevisionOperDlg::OnBnClickedSaveNewItem()
{
	UpdateData();
	if(Ta.IsHasRsltBuffer(m_sActiveRsltItemKeyStr.GetBuffer())&&AfxMessageBox("已存在该名称的计算结果，继续将覆盖原计算结果!",MB_YESNO)!=IDYES)
		return;
	if(!Ta.IsHasRsltBuffer(m_sActiveRsltItemKeyStr.GetBuffer()))
	{
		CComboBox* pCmbBox=(CComboBox*)GetDlgItem(IDC_CMB_ACTIVE_RSLT_ITEM_TITLE);
		pCmbBox->AddString(m_sActiveRsltItemKeyStr);
	}
	
	CBuffer buffer;
	postreport.ToBuffer(buffer);	//将当前激活计算结果报告写入到一个数据缓存块中
	if(Ta.WriteRsltBuffer(m_sActiveRsltItemKeyStr,buffer))
	{
		postreport.m_sRsltItem.Copy(m_sActiveRsltItemKeyStr);
		MessageBox(CXhChar50("计算结果[%s]保存成功",m_sActiveRsltItemKeyStr));
	}
	else
		AfxMessageBox(CXhChar50("计算结果[%s]保存失败",m_sActiveRsltItemKeyStr));
}

void CRevisionOperDlg::OnBnClickedReadRsltItem()
{
	UpdateData();
	if(m_sActiveRsltItemKeyStr.Compare("***")!=0)
	{
		CBuffer buffer;
		if(!Ta.ReadRsltBuffer(m_sActiveRsltItemKeyStr,buffer))
		{
			AfxMessageBox(CXhChar50("计算结果[%s]读取失败",m_sActiveRsltItemKeyStr));
			return;
		}
		BeginWaitCursor();
		buffer.SeekToBegin();
		postreport.FromBuffer(buffer);
		postreport.m_sRsltItem.Copy(m_sActiveRsltItemKeyStr);
		EndWaitCursor();
		MessageBox(CXhChar50("计算结果[%s]已读入",m_sActiveRsltItemKeyStr));
	}
	if(theApp.m_pMainWnd)
	{
		CRevisionOperDlg *pRevision=((CMainFrame*)theApp.m_pMainWnd)->GetRevisionOperPage();
		pRevision->UpdateRevision(postreport);
	}
}
void CRevisionOperDlg::UpdateRsltNames()
{
	CAttachBuffer *pRsltBuffer=Ta.AttachBuffer.GetValue(CTower::RSLT_REPORT);
	int item_n=0;
	if(pRsltBuffer)
	{
		pRsltBuffer->SeekToBegin();
		pRsltBuffer->ReadInteger(&item_n);
	}
	CComboBox* pCmbBox=(CComboBox*)GetDlgItem(IDC_CMB_ACTIVE_RSLT_ITEM_TITLE);
	pCmbBox->ResetContent();
	CXhChar16 rsltItemKeyStr;
	for(int i=0;i<item_n;i++)
	{
		if(pRsltBuffer->ReadString(rsltItemKeyStr,17)>0)
		{
			pCmbBox->AddString(rsltItemKeyStr);
			int length=0;
			pRsltBuffer->ReadInteger(&length);
			pRsltBuffer->Offset(length);
		}
	}
}
void CRevisionOperDlg::OnBnClickedDelRsltItem()
{
	UpdateData();
	if(Ta.RemoveRsltBuffer(m_sActiveRsltItemKeyStr.GetBuffer()))
	{
		CComboBox* pCmbBox=(CComboBox*)GetDlgItem(IDC_CMB_ACTIVE_RSLT_ITEM_TITLE);
		if(pCmbBox->GetCurSel()>=0)
		{
			pCmbBox->DeleteString(pCmbBox->GetCurSel());
			MessageBox(CXhChar50("计算结果[%s]删除成功!",m_sActiveRsltItemKeyStr));
		}
	}
	else
		AfxMessageBox("已存计算结果中未找到当前项，删除失败!");
}

void CRevisionOperDlg::OnBnClickedExportRsltRptFile()
{
	UpdateData();

	static CExportRptFileOptionDlg rpt_dlg;
	rpt_dlg.m_iEfficType=g_sysPara.iEfficacyCeofType;
	if(rpt_dlg.DoModal()==IDOK)
	{
		CBuffer buffer;
		if( m_sActiveRsltItemKeyStr.Compare("***")!=0 && 
			!postreport.m_sRsltItem.EqualNoCase(m_sActiveRsltItemKeyStr))
		{
			if(!Ta.ReadRsltBuffer(m_sActiveRsltItemKeyStr,buffer))
			{
				AfxMessageBox(CXhChar50("计算结果[%s]读取失败",m_sActiveRsltItemKeyStr));
				return;
			}
			buffer.SeekToBegin();
			postreport.FromBuffer(buffer);
			postreport.m_sRsltItem.Copy(m_sActiveRsltItemKeyStr);
		}
		//结果文件目录生成
		char file_path[MAX_PATH], file_name[MAX_PATH];
		CString ss=theApp.GetLDSDoc()->GetPathName();
		_snprintf(file_path,MAX_PATH-1,"%s",ss);
		GetSysPath(file_path,file_path,file_name);
		strcat(file_path,file_name);
		strcat(file_path,"\\");
		strcat(file_path,file_name);
		g_sysPara.iEfficacyCeofType=rpt_dlg.m_iEfficType;
		if(rpt_dlg.m_iFileLanguage == 0)
			CreateDetailRptFileFromReportByCN(file_path,rpt_dlg.m_iDocFileType);
		else if(rpt_dlg.m_iFileLanguage == 1)
			CreateDetailRptFileFromReportByEN(file_path,rpt_dlg.m_iDocFileType);
	}
}
void CRevisionOperDlg::OnOK() 
{
	//确认命令行输入
}

void CRevisionOperDlg::OnCancel() 
{
}

BOOL ModifyItemProperty(CPropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{
	CRevisionOperDlg *pRevision=(CRevisionOperDlg *)pPropList->GetParent();
#ifdef AFX_TARG_ENU_ENGLISH
	if(valueStr.Compare("display")==0)
#else
	if(valueStr.Compare("显示")==0)
#endif
		pRevision->SetColumnItemState(pItem->m_idProp,true);
	else
		pRevision->SetColumnItemState(pItem->m_idProp,false);
	return TRUE;
}
bool UpdateElemPropertyPageByRevisionPage(DYN_ARRAY<ELEM_PTR>& elem_arr)
{
	CMainFrame* pMainWnd=(CMainFrame*)theApp.m_pMainWnd;
	pMainWnd->GetLDSView()->selectObjs.Empty();
	CTowerPropertyDlg* pPropPage=pMainWnd->GetTowerPropertyPage();
	pPropPage->m_propList.Lock();
	g_pSolidDraw->ReleaseSnapStatus();
	for(UINT i=0;i<elem_arr.Size();i++)
	{
		long hStartNode=elem_arr[i]->pStart->handle;
		long hEndNode=elem_arr[i]->pEnd->handle;
		CElemInfo* pElemInfo=Ta.Elems.GetValue(hStartNode,hEndNode);
		if(pElemInfo==NULL)
			pElemInfo=Ta.Elems.GetValue(hEndNode,hStartNode);
		if(pElemInfo)	
			g_pSolidDraw->SetEntSnapStatus(pElemInfo->Id|0x80000000);
	}
	pPropPage->m_propList.Unlock();
	pMainWnd->GetLDSView()->UpdatePropertyPage();
	//选中与当前单元对称的关联单元
	pPropPage->m_propList.Lock();
	for(i=0;i<elem_arr.Size();i++)
	{
		CLDSNode *pMirStart=NULL,*pMirEnd=NULL;
		//选中X轴对称单元
		pMirStart=elem_arr[i]->pStart->GetMirNode(MIRMSG(1));
		pMirEnd=elem_arr[i]->pEnd->GetMirNode(MIRMSG(1));
		if(pMirStart&&pMirEnd)
		{
			CElemInfo* pElemInfo=Ta.Elems.GetValue(pMirStart->handle,pMirEnd->handle);
			if(pElemInfo==NULL)
				pElemInfo=Ta.Elems.GetValue(pMirEnd->handle,pMirStart->handle);
			if(pElemInfo)
				g_pSolidDraw->SetEntSnapStatus(pElemInfo->Id|0x80000000);
		}
		//选中Y轴对称单元
		pMirStart=pMirEnd=NULL;
		pMirStart=elem_arr[i]->pStart->GetMirNode(MIRMSG(2));
		pMirEnd=elem_arr[i]->pEnd->GetMirNode(MIRMSG(2));
		if(pMirStart&&pMirEnd)
		{
			CElemInfo* pElemInfo=Ta.Elems.GetValue(pMirStart->handle,pMirEnd->handle);
			if(pElemInfo==NULL)
				pElemInfo=Ta.Elems.GetValue(pMirEnd->handle,pMirStart->handle);
			if(pElemInfo)
				g_pSolidDraw->SetEntSnapStatus(pElemInfo->Id|0x80000000);
		}
		//选中Z轴对称单元
		pMirStart=pMirEnd=NULL;
		pMirStart=elem_arr[i]->pStart->GetMirNode(MIRMSG(3));
		pMirEnd=elem_arr[i]->pEnd->GetMirNode(MIRMSG(3));
		if(pMirStart&&pMirEnd)
		{
			CElemInfo* pElemInfo=Ta.Elems.GetValue(pMirStart->handle,pMirEnd->handle);
			if(pElemInfo==NULL)
				pElemInfo=Ta.Elems.GetValue(pMirEnd->handle,pMirStart->handle);
			if(pElemInfo)
				g_pSolidDraw->SetEntSnapStatus(pElemInfo->Id|0x80000000);
		}
	}
	pPropPage->m_propList.Unlock();
	return pMainWnd->GetLDSView()->selectObjs.GetNodeNum()>0;
}
BOOL FireItemChanged(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem,NM_LISTVIEW* pNMListView)
{
	CElement* pElem=postreport.hashElems.GetValue(pItem->m_idProp);
	if(pElem==NULL)
		return FALSE;
	CRevisionOperDlg *pDlg=(CRevisionOperDlg*)pListCtrl->GetParent();
	if(pDlg->IsLockedLoacateElemFunc())
		return FALSE;
	DYN_ARRAY<ELEM_PTR> elem_arr(1);
	elem_arr[0]=pElem;
	SCOPE_STRU scope;
	scope.VerifyVertex(pElem->pStart->Position(false));
	scope.VerifyVertex(pElem->pEnd->Position(false));
	CLDSModule* pModule=Ta.GetActiveModule();
	CLDSLinePart* pRod=pElem->pOrgPole;
	if(pRod&&!pModule->IsSonPart(pRod))
	{
		for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			bool matched=false;
			if(pModule->IsSonPart(pRod))
			{
				Ta.m_hActiveModule=pModule->handle;
				matched=true;
			}
			else if(pRod->cfgword.IsHasBodyNo(pModule->GetBodyNo()))
			{
				Ta.m_hActiveModule=pModule->handle;
				if(!pModule->IsSonPart(pRod))
				{
					char iQuad=pRod->Layer(2)-'1';
					if(iQuad<0||iQuad>3)
						iQuad=0;
					for(int iLeg=1;iLeg<=192;iLeg++)
					{
						if(pRod->cfgword.IsHasNo(iLeg))
						{
							pModule->m_arrActiveQuadLegNo[iQuad]=iLeg;
							matched=true;
							break;
						}
					}
				}
			}
			else
				continue;
			if(matched)
			{
				for(CLDSNode* pNode=console.DispNodeSet.GetFirst();pNode;pNode=console.DispNodeSet.GetNext())
					pNode->is_visible=true;
				for(CLDSPart* pPart=console.DispPartSet.GetFirst();pPart;pPart=console.DispPartSet.GetNext())
					pPart->is_visible=true;
				g_pSolidDraw->BuildDisplayList(NULL);	
			}
			break;
		}
		if(pModule)
		{
			CMainFrame* pMainWnd=(CMainFrame*)theApp.m_pMainWnd;
			CTowerTreeDlg* pModelPage=pMainWnd->GetTowerTreePage();
			pModelPage->ShiftActiveItemByObj(pModule);	//更新模型树中当前呼高节点的激活状态
		}
	}
	if(theApp.m_eEnvType!=FEMELEM_ENV)
	{
		theApp.m_eEnvType=FEMELEM_ENV;
		preprocessor.InitSourceDesignInfo(&Ta);
		g_pSolidDraw->BuildDisplayList(NULL);
	}
	g_pSolidOper->FocusTo(scope,g_sysPara.m_fMaxScaleScr2User);
	f3dPoint center;
	scope.centre(&center.x,&center.y,&center.z);
	g_pSolidSet->SetRotOrg(center);
	UpdateElemPropertyPageByRevisionPage(elem_arr);
	return TRUE;
}
BOOL ModifyElemProperty(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem,int iSubItem,CString& valueStr)
{
	CXhChar50 colHeader=pListCtrl->GetColumnInfo(iSubItem)->headerName;
	CElemInfo* pRelaFemElem=NULL;
	CElement* pElem=postreport.hashElems.GetValue(pItem->m_idProp);
	if(pElem==NULL)
		return FALSE;
	CRevisionOperDlg *pRevDlg=(CRevisionOperDlg*)pListCtrl->GetParent();
	CMainFrame* pMainWnd=(CMainFrame*)theApp.m_pMainWnd;
	CTowerPropertyDlg* pPropList=pMainWnd->GetTowerPropertyPage();
	if(pPropList->m_propList.m_nObjClassTypeId==CLS_FEMELEMENT)
	{
		for(CLDSObject* pObj=pMainWnd->GetLDSView()->selectObjs.GetFirst();pObj;
			pObj=pMainWnd->GetLDSView()->selectObjs.GetNext())
		{
			if(pObj->GetClassTypeId()==CLS_FEMELEMENT)
			{
				pRelaFemElem=(CElemInfo*)pObj;
				if(pRelaFemElem->m_hStartNode==pElem->pStart->handle&&pRelaFemElem->m_hEndNode==pElem->pEnd->handle)
                    break;	//左侧属性栏中选中找到与校审栏当前选中项一致的关联单元
			}
		}
	}
	if(pRelaFemElem==NULL)
	{	//左侧属性栏与校审栏不一致时,需更新左侧属性栏
	}
#ifdef AFX_TARG_ENU_ENGLISH
	if(colHeader.Equal("spec"))
#else
	if(colHeader.Equal("规  格"))
#endif
	{
		CXhChar16 spec(valueStr.GetBuffer());
		char* key=strtok(spec,"*Xx");
		if(key!=NULL)
			pElem->pOrgPole->size_wide=atof(key);
		key=strtok(NULL,"*Xx");
		if(key!=NULL)
			pElem->pOrgPole->size_thick=atof(key);
		for(RELATIVE_OBJECT *pMirObj=pElem->pOrgPole->relativeObjs.GetFirst();pMirObj;pMirObj=pElem->pOrgPole->relativeObjs.GetNext())
		{
			CLDSLinePart *pMirLinePart=(CLDSLinePart*)pElem->pOrgPole->BelongModel()->FromPartHandle(pMirObj->hObj);
			if(pMirLinePart)
			{
				pMirLinePart->size_wide=pElem->pOrgPole->size_wide;
				pMirLinePart->size_thick=pElem->pOrgPole->size_thick;
			}
		}
		CElemInfo *pElemInfo=Ta.Elems.GetValue(pElem->pStart->handle,pElem->pEnd->handle);
		if(pElemInfo)
			pRevDlg->UpdateElem(pElem->pStart->point_i,pElem->pEnd->point_i);
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(colHeader.Equal("material"))
#else
	else if(colHeader.Equal("材质"))
#endif
	{
		pElem->pOrgPole->cMaterial = QuerySteelBriefMatMark(valueStr.GetBuffer());
		for(RELATIVE_OBJECT *pMirObj=pElem->pOrgPole->relativeObjs.GetFirst();pMirObj;pMirObj=pElem->pOrgPole->relativeObjs.GetNext())
		{
			CLDSLinePart *pMirLinePart=(CLDSLinePart*)pElem->pOrgPole->BelongModel()->FromPartHandle(pMirObj->hObj);
			if(pMirLinePart)
				pMirLinePart->cMaterial=pElem->pOrgPole->cMaterial;
		}
		CElemInfo *pElemInfo=Ta.Elems.GetValue(pElem->pStart->handle,pElem->pEnd->handle);
		if(pElemInfo)
			pRevDlg->UpdateElem(pElem->pStart->point_i,pElem->pEnd->point_i);
	}
	else
		return FALSE;
	if(pRelaFemElem)	//关联更新左侧单元属性栏
		pMainWnd->GetLDSView()->DisplayFemElementProperty(FALSE);
	return TRUE;
}
// CRevisionOperDlg 消息处理程序

BOOL CRevisionOperDlg::OnInitDialog()
{
	m_sActiveRsltItemKeyStr=postreport.m_sRsltItem;
	CDialog::OnInitDialog();
	CButton *pBut=(CButton *)GetDlgItem(ID_SAVE_NEW_ITEM);
	if(m_curBitmap1.LoadBitmap(IDB_BMP_SAVE_BUTTON))
	{
		pBut->SetBitmap(m_curBitmap1);
	}
	pBut=(CButton *)GetDlgItem(ID_DEL_RSLT_ITEM);
	if(m_curBitmap2.LoadBitmap(IDB_BMP_DELETE_BUTTON))
	{
		pBut->SetBitmap(m_curBitmap2);
	}
	pBut=(CButton *)GetDlgItem(ID_READ_RSLT_ITEM);
	if(m_curBitmap3.LoadBitmap(IDB_BMP_READ_BUTTON))
	{
		pBut->SetBitmap(m_curBitmap3);
	}

#ifdef AFX_TARG_ENU_ENGLISH
	m_tabCtrlListGroup.InsertItem(0,"setting");
	m_tabCtrlListGroup.InsertItem(1,"basic");
	m_tabCtrlListGroup.InsertItem(2,"strength");
	m_tabCtrlListGroup.InsertItem(3,"pressure stability");
#else
	m_tabCtrlListGroup.InsertItem(0,"设置");
	m_tabCtrlListGroup.InsertItem(1,"基本");
	m_tabCtrlListGroup.InsertItem(2,"强度");
	m_tabCtrlListGroup.InsertItem(3,"压稳");
#endif

	m_listElems.InitListCtrl();
	m_listElems.EnableSortItems();
	m_listElems.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_listElems.SetModifyValueFunc(ModifyElemProperty);
	m_listElems.SetItemChangedFunc(FireItemChanged);
	m_listElems.SetCompareItemFunc(_LocalFireCompareItem);
	m_listElems.SetColumnClickFunc(_LocalFireColumnClick);
	m_propList.SetModifyValueFunc(ModifyItemProperty);
	DisplaySetPropList();
	OnTcnSelchangeTabCtrl(NULL,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

CString CRevisionOperDlg::GetPropValueStr(int idProp)
{
	int classify=idProp/100;
	int idItem=idProp%100;
	if(m_listElems.GetColumnInfo(idItem-1)->dwFlag&GetSingleWord(classify))
#ifdef AFX_TARG_ENU_ENGLISH
		return "display";
	else
		return "no display";
#else
		return "显示";
	else
		return "不显示";
#endif
}
void CRevisionOperDlg::SetColumnItemState(int idProp,bool bDisplay)
{
	int classify=idProp/100;
	int idItem=idProp%100;
	DWORD flag=GetSingleWord(classify);
	if(bDisplay)
		m_listElems.GetColumnInfo(idItem-1)->dwFlag|=flag;
	else if(m_listElems.GetColumnInfo(idItem-1)->dwFlag&flag)	//移除显示状态
		m_listElems.GetColumnInfo(idItem-1)->dwFlag^=flag;
	for(PROPLIST_ITEM* pItem=propHashtable.GetFirst();pItem;pItem=propHashtable.GetNext())
	{
		if(pItem->id!=idItem-1)
			continue;
		pItem->dwFlag=m_listElems.GetColumnInfo(idItem-1)->dwFlag;
		break;
	}
	WriteToElemRevCfgFile();	//改动配置后自动存储配置文件
}
void CRevisionOperDlg::DisplayOneGroupSetProperty(int group,CPropTreeItem *pGroupItem)
{
	CItemInfo *lpInfo;
	CPropTreeItem *pPropItem;

	int serial=0;
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "unit mark";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "unit mark";
#else
	lpInfo->m_strPropName = "单元标识";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "单元标识";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "symmetry info";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "symmetry information";
#else
	lpInfo->m_strPropName = "对称信息";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "对称信息";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "layer";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "layer,can show rod's load type, such as leg member, V titiled material, transverse material, and auxiliary material,etc.";
#else
	lpInfo->m_strPropName = "图层";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "图层，可表示杆件承力类型，如主材、斜材、横材辅材等";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "type";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "material type";
#else
	lpInfo->m_strPropName = "类型";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "材料类型";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "spec";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "specification";
#else
	lpInfo->m_strPropName = "规格";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "规格";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "area";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "section area, cm2";
#else
	lpInfo->m_strPropName = "截面积";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "截面积, cm2";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "material";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "steel material";
#else
	lpInfo->m_strPropName = "材质";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "材质";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "BOM Number";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "BOM Number";
#else
	lpInfo->m_strPropName = "统材号";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "统材号";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
	lpInfo->m_strPropName = "H,R";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "connecting type,H:articulate，R:rigid connect.";
#else
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "连接类型,H:铰接，R:刚接。";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "rod end status";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "rod end limit,F:no rotation limit,P:partly rotation limit; rod end stressed,C:center stressed,E:Eccentric Force.";
#else
	lpInfo->m_strPropName = "杆端状况";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "杆端约束,F:无转动约束,P:部分转动约束; 杆端受力,C:中心受力,E:偏心受力。";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "bolt info";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "bolt information";
#else
	lpInfo->m_strPropName = "螺栓信息";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "螺栓信息";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "bolt efficiency";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "Bolt actual maximum load/Limit load ratio";
#else
	lpInfo->m_strPropName = "螺栓效率";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "螺栓实际最大承受荷载/极限承受荷载比值";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "reduce hole number";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "reduce hole number";
#else
	lpInfo->m_strPropName = "减孔数";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "减孔数";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "μ";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "calculate length comparative coefficient";
#else
	lpInfo->m_strPropName = "μ";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "计算长度相当系数";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
	lpInfo->m_strPropName = "Lo";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "calculate length";
#else
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "计算长度";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
	lpInfo->m_strPropName = "R";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "calculate length";
#else
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "计算长度";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
	lpInfo->m_strPropName = "KLo/r";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "Slenderness ratio ";
#else
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "长细比";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
	lpInfo->m_strPropName = "+Nmax";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "maximum pull,kN";
#else
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "最大拉力,kN";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
	lpInfo->m_strPropName = "+Case";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "module and working conditions when maximum pull appears";
#else
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "最大拉力出现的呼高及工况";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
	lpInfo->m_strPropName = "-Nmax";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "maximum pressure,kN";
#else
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "最大压力,kN";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
	lpInfo->m_strPropName = "-Case";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "module and working conditions when maximum pressure appears";
#else
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "最大压力出现的呼高及工况";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "-Nmaxφ";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "pressure when maximum pressure stability appears";,kN";
#else
	lpInfo->m_strPropName = "-Nmaxφ";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "最大压稳应力出现时的压力,kN";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "-Caseφ";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "module and working conditions when maximum pressure stability stress appears";
#else
	lpInfo->m_strPropName = "-Caseφ";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "最大压稳应力出现的呼高及工况";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "σmax";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "maximum stress,MPa";
#else
	lpInfo->m_strPropName = "σmax";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "最大应力,MPa";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "σa";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "persimissible stress,MPa";
#else
	lpInfo->m_strPropName = "σa";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "允许应力,MPa";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
	lpInfo->m_strPropName = "m";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "strength reduction factor";
#else
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "强度折减系数";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "φ";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "pressure stability factor";
#else
	lpInfo->m_strPropName = "φ";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "压稳系数";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "unit effeciency";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "ratio between design value and limit value";
#else
	lpInfo->m_strPropName = "单元效率";
	lpInfo->m_cmbItems="显示|不显示";
	lpInfo->m_strPropHelp = "设计值/极限值的比值系数";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
}
void CRevisionOperDlg::DisplaySetPropList()
{
	CItemInfo *lpInfo;
	CPropTreeItem *pRoot,*pGroupItem;
	m_propList.CleanTree();
	m_propList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	pRoot=m_propList.GetRootItem();

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "basic";
	lpInfo->m_strPropHelp = "rod's specification and unit connecting bolts information";
#else
	lpInfo->m_strPropName = "基本";
	lpInfo->m_strPropHelp = "杆件规格及单元连接螺栓信息";
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp = 0;
	pGroupItem->m_bHideChildren=FALSE;
	DisplayOneGroupSetProperty(100,pGroupItem);

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "strength";
	lpInfo->m_strPropHelp = "rod's specification and unit connecting bolts information";
#else
	lpInfo->m_strPropName = "强度";
	lpInfo->m_strPropHelp = "杆件规格及单元连接螺栓信息";
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp = 0;
	pGroupItem->m_bHideChildren=FALSE;
	DisplayOneGroupSetProperty(200,pGroupItem);

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "pressure stability";
	lpInfo->m_strPropHelp = "rod's specification and unit connecting bolts information";
#else
	lpInfo->m_strPropName = "压稳";
	lpInfo->m_strPropHelp = "杆件规格及单元连接螺栓信息";
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp = 0;
	pGroupItem->m_bHideChildren=FALSE;
	DisplayOneGroupSetProperty(300,pGroupItem);

	m_propList.Redraw();
}

void CRevisionOperDlg::OnTcnSelchangeTabCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iSelPage=m_tabCtrlListGroup.GetCurSel();
	if(iSelPage==0)// TODO: 在此添加控件通知处理程序代码
	{
		GetDlgItem(IDC_LIST_GROUP_SETTING)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_OUTPUT_INFO_LIST)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_OUTPUT_INFO_LIST)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST_GROUP_SETTING)->ShowWindow(SW_HIDE);
		m_listElems.SelPage(iSelPage-1);
	}
	if(pResult)
		*pResult = 0;
}

void CRevisionOperDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	RECT rect;
	GetClientRect(&rect);
	int headerBtnHeight=20;
	if(m_tabCtrlListGroup.GetSafeHwnd())
	{
		RECT rc;
		m_tabCtrlListGroup.GetClientRect(&rc);
		headerBtnHeight=rc.bottom-rc.top;
	}
	rect.top=headerBtnHeight+20;
	CWnd *pSetWnd=GetDlgItem(IDC_LIST_GROUP_SETTING);
	if(pSetWnd!=NULL&&pSetWnd->GetSafeHwnd()!=NULL)
		pSetWnd->MoveWindow(&rect);
	CWnd *pRevWnd=GetDlgItem(IDC_OUTPUT_INFO_LIST);
	if(pRevWnd!=NULL&&pRevWnd->GetSafeHwnd()!=NULL)
		pRevWnd->MoveWindow(&rect);
	CWnd *pPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR);
	if(m_tabCtrlListGroup.GetSafeHwnd()!=NULL&&pPromptWnd!=NULL&&pPromptWnd->GetSafeHwnd()!=NULL)
	{
		RECT rect1,rect2;
		m_tabCtrlListGroup.GetWindowRect(&rect1);
		pPromptWnd->GetWindowRect(&rect2);
		int width=rect2.right-rect2.left;
		rect2.left=rect1.right+2;
		rect2.right=rect2.left+width;
		ScreenToClient(&rect2);
		rect2.right=rect.right;
		rect2.top=25;
		rect2.bottom=headerBtnHeight+20;
		pPromptWnd->MoveWindow(&rect2);
	}

	// TODO: 在此处添加消息处理程序代码
}
void CRevisionOperDlg::OnLvnColumnclickOutputInfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
#ifdef AFX_TARG_ENU_ENGLISH
	if( m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("mark unit")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("specification")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("steel material")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("+Nmax")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("-Nmax")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("-Nmaxφ")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("σmax")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("bolt efficiency")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("reduce hole number")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("elem effic"))
#else
	if( m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("单元标识")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("规  格")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("材质")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("+Nmax")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("-Nmax")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("-Nmaxφ")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("σmax")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("螺栓效率")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("减孔数")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("单元效率"))
#endif
	{
		if(hashSortOrderType.GetValue(pNMLV->iSubItem+1))
			hashSortOrderType[pNMLV->iSubItem+1]=!hashSortOrderType[pNMLV->iSubItem+1];
		else
			hashSortOrderType.SetValue(pNMLV->iSubItem+1,true);
		itemsort.columnLabel=m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName;
		UpdateRevision(postreport,pNMLV->iSubItem,hashSortOrderType[pNMLV->iSubItem+1]);
	}
	*pResult = 0;
}
void CRevisionOperDlg::UpdateRevision(CPostProcessReport &report,int iOrderCol,bool bAscending)
{
	m_sActiveRsltItemKeyStr=report.m_sRsltItem;
	UpdateData(FALSE);
	CLDSModule *pModule;
	CWorkStatus *pStatus;
	CHashTable<long>hashModuleNo,hashStatusNo;
	hashModuleNo.CreateHashTable(Ta.Module.GetNodeNum());
	hashStatusNo.CreateHashTable(Ta.WorkStatus.GetNodeNum());
	for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		hashModuleNo.SetValueAt(pModule->handle,pModule->iNo);
	for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
		hashStatusNo.SetValueAt(pStatus->handle,pStatus->iNo);
	m_listElems.DeleteAllItems();
	UINT i=0;
	CElement* pElem;
	DYN_ARRAY<CElement*> elem_arr(report.hashElems.GetNodeNum()); 
	for(pElem=report.hashElems.GetFirst();pElem;pElem=report.hashElems.GetNext(),i++)
		elem_arr[i]=pElem;
	itemsort.bItemSortType=bAscending;
	CHeapSort<ELEM_PTR>::HeapSort(elem_arr,report.hashElems.GetNodeNum(),CompareElemValue);
	for(i=0;i<report.hashElems.GetNodeNum();i++)
	{
		pElem=elem_arr[i];
		if(pElem->pStart==NULL||pElem->pEnd==NULL)
			continue;
		int index=1;
		long min_pointi=min(pElem->pStart->point_i,pElem->pEnd->point_i);
		long max_pointi=max(pElem->pStart->point_i,pElem->pEnd->point_i);
		CXhChar50 itemText("%d-%d",min_pointi,max_pointi);
		CListCtrlItemInfo *pInfo=new CListCtrlItemInfo();
		pInfo->AddSubItemText((char*)itemText,TRUE);	//单元标识
		//if(pElem->relaElem.bFatherElem)
		{
			if(pElem->relaElem.idElemMirX>0)
				itemText.Copy("X");
			else
				itemText.Copy(" ");
			if(pElem->relaElem.idElemMirY>0)
				itemText.Append("Y");
			else
				itemText.Append(" ");
			if(pElem->relaElem.idElemMirZ>0)
				itemText.Append("Z");
			else
				itemText.Append(" ");
		}
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//对称信息
		if(pElem->pOrgPole)	//图层
			pInfo->SetSubItemText(index++,CXhChar16("%C",pElem->pOrgPole->layer(1)),TRUE);
		else
			pInfo->SetSubItemText(index++,"",TRUE);
		if(pElem->pOrgPole)	//材料类型
		{
			if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE)
				pInfo->SetSubItemText(index++,_T(" L"),TRUE);
			else if(pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
			{
				if(pElem->pOrgPole->size_cSubClassType=='D')
					pInfo->SetSubItemText(index++,_T("DL"),TRUE);
				else if(pElem->pOrgPole->size_cSubClassType=='X')
					pInfo->SetSubItemText(index++,_T("XL"),TRUE);
				else if(pElem->pOrgPole->size_cSubClassType=='T')
					pInfo->SetSubItemText(index++,_T("TL"),TRUE);
				else
					pInfo->SetSubItemText(index++,_T("?L"),TRUE);
			}
			else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
				pInfo->SetSubItemText(index++,_T("φ"),TRUE);
			else if(pElem->pOrgPole->size_idClassType==CLS_LINESLOT)
				pInfo->SetSubItemText(index++,_T(" ["),TRUE);
			else
				pInfo->SetSubItemText(index++,_T(" U"),TRUE);
		}
		else
			pInfo->SetSubItemText(index++,"",TRUE);
		itemText.Printf("%sX%s",(char*)CXhChar16(pElem->GetWidth()),(char*)CXhChar16(pElem->GetThick()));
		pInfo->SetSubItemText(index++,(char*)itemText);	//规格
		CString itemsStr;
		if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE)
			itemsStr=MakeAngleSetString();
		else if(pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
			itemsStr=MakeAngleSetString(pElem->pOrgPole->size_cSubClassType);
		else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
			itemsStr=MakeTubeSetString();
		else if(pElem->pOrgPole->size_idClassType==CLS_LINESLOT)
			itemsStr=MakeSlotSetString();
		else if(pElem->pOrgPole->size_idClassType==CLS_LINEFLAT)
			itemsStr=MakeFlatSetString();
		else if(pElem->pOrgPole->size_idClassType==CLS_CABLE)
			itemsStr=MakeWireSetString(pElem->pOrgPole->GetWireType());
		pInfo->SetControlType(index-1,GCT_CMB_LIST);
		pInfo->SetListItemsStr(index-1,itemsStr);
		itemText.Printf(CXhChar16("%g",0.01*pElem->pOrgPole->GetArea()));
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//截面积, cm2

		//itemText.Printf("%C",pElem->pOrgPole->cMaterial);
		QuerySteelMatMark(pElem->pOrgPole->cMaterial,itemText);
		pInfo->SetSubItemText(index++,(char*)itemText);	//材质
		pInfo->SetControlType(index-1,GCT_CMB_LIST);
		itemsStr=MakeMaterialMarkSetString();
		pInfo->SetListItemsStr(index-1,itemsStr);

		if(pElem->pOrgPole->m_uStatMatNo>0)
			itemText.Printf("%d",pElem->pOrgPole->m_uStatMatNo);
		else
			itemText.Copy("");
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//统材号
		if(pElem->real_start_joint_type==JOINT_HINGE&&pElem->real_end_joint_type==JOINT_HINGE)
			itemText.Copy("HH");
		else if(pElem->real_start_joint_type==JOINT_HINGE&&pElem->real_end_joint_type==JOINT_RIGID)
			itemText.Copy("HR");
		else if(pElem->real_start_joint_type==JOINT_RIGID&&pElem->real_end_joint_type==JOINT_HINGE)
			itemText.Copy("RH");
		else
			itemText.Copy("RR");
		char cTemp;
		if(min_pointi!=pElem->pStart->point_i)
		{
			cTemp=itemText[0];
			itemText[0]=itemText[1];
			itemText[1]=cTemp;
		}
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//连接类型
		if(pElem->design_info.bStartRotRest)
			itemText.Copy("P");
		else
			itemText.Copy("F");
		if(pElem->design_info.bEndRotRest)
			itemText.Append("P");
		else
			itemText.Append("F");
		if(pElem->start_force_type==CENTRIC_FORCE)
			itemText.Append(", C");
		else
			itemText.Append(", E");
		if(pElem->end_force_type==CENTRIC_FORCE)
			itemText.Append('C');
		else
			itemText.Append("E");
		if(min_pointi!=pElem->pStart->point_i)
		{
			cTemp=itemText[0];
			itemText[0]=itemText[1];
			itemText[1]=cTemp;
			cTemp=itemText[4];
			itemText[4]=itemText[5];
			itemText[5]=cTemp;
		}
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//杆端受力
		if(pElem->pStart==pElem->pOrgPole->pStart)
			itemText.Printf("%sM%dX%d",pElem->pOrgPole->connectStart.grade,pElem->pOrgPole->connectStart.d,pElem->pOrgPole->connectStart.wnSummBoltN);
		else if(pElem->pEnd==pElem->pOrgPole->pEnd)
			itemText.Printf("%sM%dX%d",pElem->pOrgPole->connectEnd.grade,pElem->pOrgPole->connectEnd.d,pElem->pOrgPole->connectEnd.wnSummBoltN);
		else
			itemText.Printf("--");
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);//itemText.Equal("--"));	//螺栓信息
		if(pElem->pStart==pElem->pOrgPole->pStart)
			itemText.Printf("%.3f",pElem->pOrgPole->connectStart.bearloadcoef);
		else if(pElem->pEnd==pElem->pOrgPole->pEnd)
			itemText.Printf("%.3f",pElem->pOrgPole->connectEnd.bearloadcoef);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//螺栓效率
		if(pElem->pStart==pElem->pOrgPole->pStart)
			itemText.Printf("%.2f",pElem->design_info.hole_out);
		else if(pElem->pEnd==pElem->pOrgPole->pEnd)
			itemText.Printf("%.2f",pElem->design_info.hole_out);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//减孔数
		double L=DISTANCE(pElem->pStart->Position(false),pElem->pEnd->Position(false));
		//itemText.Printf("%.3f",pElem->LamdaL()/L);
		CExpression::ToExpression(pElem->LamdaL()/L,3,itemText,50);
		SimplifiedNumString(itemText);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//μ
		itemText.Printf("%.0f",pElem->LamdaL());
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//Lo
		itemText.Printf("%.1f",pElem->LamdaR());
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//R
		itemText.Printf("%.0f",pElem->Lamda());
		//if(pElem->LamdaRType()==0)
		//	itemText.Append(":Rx");
		//else if(pElem->LamdaRType()==1)
		//	itemText.Append(":Ryo");
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//KLo/r
		
		long module_no=0,status_no=0;
		itemText.Printf("%.2f",pElem->MaxTension.N/1000);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//+Nmax
		hashModuleNo.GetValueAt(pElem->MaxTension.hModule,module_no);
		hashStatusNo.GetValueAt(pElem->MaxTension.hWorkStatus,status_no);
		itemText.Printf("%2d-%3d",module_no,status_no);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//+Case
		
		itemText.Printf("%.2f",pElem->MaxCompression.N/1000);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//-Nmax
		hashModuleNo.GetValueAt(pElem->MaxCompression.hModule,module_no);
		hashStatusNo.GetValueAt(pElem->MaxCompression.hWorkStatus,status_no);
		itemText.Printf("%2d-%3d",module_no,status_no);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//-Case

		itemText.Printf("%.2f",pElem->MaxCompStable.N/1000);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//-Nmaxφ
		hashModuleNo.GetValueAt(pElem->MaxCompStable.hModule,module_no);
		hashStatusNo.GetValueAt(pElem->MaxCompStable.hWorkStatus,status_no);
		itemText.Printf("%2d-%3d",module_no,status_no);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//-Caseφ

		CXhChar16 caserule;
		double ctrlstress=0;
		double coefStrength=1.0;
		double A=pElem->pOrgPole->GetArea();
		double An=A-pElem->design_info.hole_out*pElem->design_info.hole_d*pElem->pOrgPole->GetThick();
		STEELMAT *pSteelMat=QuerySteelMatByBriefMark(pElem->pOrgPole->cMaterial);
		if(pElem->design_info.iDangerousLoadCase==2)		//T
		{
			coefStrength=pElem->design_info.fCoefTensStrengthDecrease;
			ctrlstress=pElem->MaxTension.N/An;
		}
		else if(pElem->design_info.iDangerousLoadCase==3)	//C
		{
			coefStrength=pElem->design_info.fCoefCompStrengthDecrease;
			ctrlstress=pElem->MaxCompression.N/An;
		}
		else //if(pElem->design_info.iDangerousLoadCase==4)	//S
		{
			coefStrength=pElem->design_info.mN;
			if(pElem->MaxCompression.N>-EPS||pElem->design_info.fCoefStablity<EPS)
				ctrlstress=0;
			else	//由于最大压稳应力出现工况不一定与最大压稳力工况一致，故不能用最大压稳力除压稳应力控制工况下的压稳系数 wjh-2013.10.15
				ctrlstress=pElem->MaxCompStable.N/(A*pElem->design_info.fCoefStablity);
		}
		double limStrength=pSteelMat->f(pElem->GetThick())*coefStrength;
		if(g_sysPara.iEfficacyCeofType==0)
		{
			if(pElem->design_info.iDangerousRule==1)
				caserule.Printf("%5.3f@λ",pElem->design_info.fCoefEffic);
			else if(pElem->design_info.iDangerousRule==2)
			{
				if(pElem->design_info.bBlockShearActivated)
					caserule.Printf("%5.3f@+T",pElem->design_info.fCoefEffic);
				else
					caserule.Printf("%5.3f@ T",pElem->design_info.fCoefEffic);
			}
			else if(pElem->design_info.iDangerousRule==3)
				caserule.Printf("%5.3f@ C",pElem->design_info.fCoefEffic);
			else if(pElem->design_info.iDangerousRule==4)
			{
				if(pElem->MaxCompStable.bothComp)
					caserule.Printf("%5.3f@+S",pElem->design_info.fCoefEffic);
				else
					caserule.Printf("%5.3f@ S",pElem->design_info.fCoefEffic);
			}
			else
				caserule.Printf("%5.3f@ 0",pElem->design_info.fCoefEffic);
		}
		else
		{
			double effic=0;
			if(coefStrength>EPS)
				effic=fabs(ctrlstress)/limStrength;
			if(pElem->design_info.iDangerousLoadCase==2)		//T
			{
				if(pElem->design_info.bBlockShearActivated)
					caserule.Printf("%5.3f@+T",effic);
				else
					caserule.Printf("%5.3f@ T",effic);
			}
			else if(pElem->design_info.iDangerousLoadCase==3)	//C
				caserule.Printf("%5.3f@ C",effic);
			else if(pElem->design_info.iDangerousLoadCase==4)	//S
			{
				if(pElem->MaxCompStable.bothComp)
					caserule.Printf("%5.3f@+S",effic);
				else
					caserule.Printf("%5.3f@ S",effic);
			}
			else
#ifdef AFX_TARG_ENU_ENGLISH
				caserule.Printf("zero rod");
#else
				caserule.Printf("零杆");
#endif
		}

		itemText.Printf("%.1f",ctrlstress);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//σmax
		itemText.Printf("%.1f",limStrength);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//σa
		itemText.Printf("%.2f",coefStrength);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//m
		itemText.Printf("%.3f",pElem->design_info.fCoefStablity);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//φ
		pInfo->SetSubItemText(index++,(char*)caserule,TRUE);	//裕度
		CSuperGridCtrl::CTreeItem* pItem=m_listElems.InsertRootItem(pInfo,FALSE);
		pItem->m_idProp=pElem->id;
		//m_listElems.AddColumnHeader("对称信息",80);
	}
	m_listElems.Redraw();
}
void CRevisionOperDlg::ReleaseAllItemSelMark()
{
	int i,count=m_listElems.GetCount();
	for(i=0;i<count;i++)
		m_listElems.SetItemState(i,0,LVIS_SELECTED);
}
void CRevisionOperDlg::LocateElem(long iStartPointI,long iEndPointI)
{
	if(!m_bLockedLocateElemFunc)
	{
		m_bLockedLocateElemFunc=true;
		int i,count=m_listElems.GetCount();
		long min_pointi=min(iStartPointI,iEndPointI);
		long max_pointi=max(iStartPointI,iEndPointI);
		CXhChar50 matchText("%d-%d",min_pointi,max_pointi);
		for(i=0;i<count;i++)
		{
			CString label=m_listElems.GetItemText(i,0);
			if(label.CompareNoCase((char*)matchText)==0)
			{
				m_listElems.SetItemState(i,LVIS_SELECTED,LVIS_SELECTED);
				m_listElems.EnsureVisible(i,FALSE);
				break;
			}
		}
		m_bLockedLocateElemFunc=false;
	}
}
//from MatchPoleGuige.cpp
int MemberCheck(CElement *pElem,BOOL bCheckAll,int iDesignCode,bool bEqCheck=false,bool bUpdateBoltInfo=false);
void CRevisionOperDlg::UpdateElem(long iStartPointI,long iEndPointI)
{
	if(m_bLockedUpdateElemFunc)
		return;
	try{
	int iSelected,count=m_listElems.GetCount();
	long min_pointi=min(iStartPointI,iEndPointI);
	long max_pointi=max(iStartPointI,iEndPointI);
	CXhChar50 matchText("%d-%d",min_pointi,max_pointi);
	for(iSelected=0;iSelected<count;iSelected++)
	{
		CString label=m_listElems.GetItemText(iSelected,0);
		if(label.CompareNoCase((char*)matchText)==0)
			break;
	}
	if(iSelected==count)
		return;	//未找到相应的单元

	CSuperGridCtrl::CTreeItem *pItem=m_listElems.GetTreeItem(iSelected);
	CElement* pElem=postreport.hashElems.GetValue(pItem->m_idProp);
	if(pElem==NULL||pElem->pStart==NULL||pElem->pEnd==NULL)
		return;
	m_bLockedUpdateElemFunc=true;
	int index=1;
	CXhChar50 itemText;
	//m_listElems.SetSubItemText(pItem,0,itemText);			//0.单元标识
	index++;//pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//1.对称信息
	//以下信息部分涉及设计验算结果,需要重新计算更新,故此处调用MemberCheck函数 wjh-2013.2.24
	pElem->pParaType=NULL;	//因pParaType是用于优化选材时临时记载规格信息,优先级高于杆件本身规格信息,此处需赋空
	MemberCheck(pElem,TRUE,Ta.m_iMemberSizeCheckCode,false);
	if(pElem->pOrgPole)										//2.图层信息
		m_listElems.SetSubItemText(pItem,index++,CXhChar16("%C",pElem->pOrgPole->layer(1)));
	else
		m_listElems.SetSubItemText(pItem,index++,"");
	if(pElem->pOrgPole)		//3.材料类型
	{
		if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE)
			m_listElems.SetSubItemText(pItem,index++,_T(" L"));
		else if(pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
		{
			if(pElem->pOrgPole->size_cSubClassType=='D')
				m_listElems.SetSubItemText(pItem,index++,_T("DL"));
			else if(pElem->pOrgPole->size_cSubClassType=='X')
				m_listElems.SetSubItemText(pItem,index++,_T("XL"));
			else if(pElem->pOrgPole->size_cSubClassType=='T')
				m_listElems.SetSubItemText(pItem,index++,_T("TL"));
			else
				m_listElems.SetSubItemText(pItem,index++,_T("?L"));
		}
		else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
			m_listElems.SetSubItemText(pItem,index++,_T("φ"));
		else if(pElem->pOrgPole->size_idClassType==CLS_LINESLOT)
			m_listElems.SetSubItemText(pItem,index++,_T(" ["));
		else
			m_listElems.SetSubItemText(pItem,index++,_T(" U"));
	}
	else
		m_listElems.SetSubItemText(pItem,index++,"");
	itemText.Printf("%sX%s",(char*)CXhChar16(pElem->GetWidth()),(char*)CXhChar16(pElem->GetThick()));
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//4.规格
	CString itemsStr;
	if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE)
		itemsStr=MakeAngleSetString();
	else if(pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
		itemsStr=MakeAngleSetString(pElem->pOrgPole->size_cSubClassType);
	else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
		itemsStr=MakeTubeSetString();
	else if(pElem->pOrgPole->size_idClassType==CLS_LINESLOT)
		itemsStr=MakeSlotSetString();
	else if(pElem->pOrgPole->size_idClassType==CLS_LINEFLAT)
		itemsStr=MakeFlatSetString();
	else if(pElem->pOrgPole->size_idClassType==CLS_CABLE)
		itemsStr=MakeWireSetString(pElem->pOrgPole->GetWireType());
	pItem->m_lpNodeInfo->SetListItemsStr(index-1,itemsStr);
	itemText.Printf(CXhChar16("%g",0.01*pElem->pOrgPole->GetArea()));
	pItem->m_lpNodeInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//截面积, cm2

	//itemText.Printf("%C",pElem->pOrgPole->cMaterial);
	QuerySteelMatMark(pElem->pOrgPole->cMaterial,itemText);
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//5.材质

	if(pElem->pOrgPole->m_uStatMatNo>0)
		itemText.Printf("%d",pElem->pOrgPole->m_uStatMatNo);
	else
		itemText.Copy("");
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//6.统材号
	if(pElem->real_start_joint_type==JOINT_HINGE&&pElem->real_end_joint_type==JOINT_HINGE)
		itemText.Copy("HH");
	else if(pElem->real_start_joint_type==JOINT_HINGE&&pElem->real_end_joint_type==JOINT_RIGID)
		itemText.Copy("HR");
	else if(pElem->real_start_joint_type==JOINT_RIGID&&pElem->real_end_joint_type==JOINT_HINGE)
		itemText.Copy("RH");
	else
		itemText.Copy("RR");
	char cTemp;
	if(min_pointi!=pElem->pStart->point_i)
	{
		cTemp=itemText[0];
		itemText[0]=itemText[1];
		itemText[1]=cTemp;
	}
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//连接类型
	if(pElem->design_info.bStartRotRest)
		itemText.Copy("P");
	else
		itemText.Copy("F");
	if(pElem->design_info.bEndRotRest)
		itemText.Append("P");
	else
		itemText.Append("F");
	if(pElem->start_force_type==CENTRIC_FORCE)
		itemText.Append(", C");
	else
		itemText.Append(", E");
	if(pElem->end_force_type==CENTRIC_FORCE)
		itemText.Append('C');
	else
		itemText.Append("E");
	if(min_pointi!=pElem->pStart->point_i)
	{
		cTemp=itemText[0];
		itemText[0]=itemText[1];
		itemText[1]=cTemp;
		cTemp=itemText[4];
		itemText[4]=itemText[5];
		itemText[5]=cTemp;
	}
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//杆端状况
	if(pElem->pStart==pElem->pOrgPole->pStart)
		itemText.Printf("%sM%dX%d",pElem->pOrgPole->connectStart.grade,pElem->pOrgPole->connectStart.d,pElem->pOrgPole->connectStart.wnSummBoltN);
	else if(pElem->pEnd==pElem->pOrgPole->pEnd)
		itemText.Printf("%sM%dX%d",pElem->pOrgPole->connectEnd.grade,pElem->pOrgPole->connectEnd.d,pElem->pOrgPole->connectEnd.wnSummBoltN);
	else
		itemText.Printf("--");
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);//itemText.Equal("--"));	//螺栓信息
	if(pElem->pStart==pElem->pOrgPole->pStart)
		itemText.Printf("%.3f",pElem->pOrgPole->connectStart.bearloadcoef);
	else if(pElem->pEnd==pElem->pOrgPole->pEnd)
		itemText.Printf("%.3f",pElem->pOrgPole->connectEnd.bearloadcoef);
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//螺栓效率
	if(pElem->pStart==pElem->pOrgPole->pStart)
		itemText.Printf("%.2f",pElem->design_info.hole_out);
	else if(pElem->pEnd==pElem->pOrgPole->pEnd)
		itemText.Printf("%.2f",pElem->design_info.hole_out);
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//减孔数
	double L=DISTANCE(pElem->pStart->Position(false),pElem->pEnd->Position(false));
	//itemText.Printf("%.3f",pElem->LamdaL()/L);
	CExpression::ToExpression(pElem->LamdaL()/L,3,itemText,50);
	SimplifiedNumString(itemText);
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//μ
	itemText.Printf("%.0f",pElem->LamdaL());
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//Lo
	itemText.Printf("%.1f",pElem->LamdaR());
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//R
	itemText.Printf("%.0f",pElem->Lamda());
	//if(pElem->LamdaRType()==0)
	//	itemText.Append(":Rx");
	//else if(pElem->LamdaRType()==1)
	//	itemText.Append(":Ryo");
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//KLo/r
	
	index++;//m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//+Nmax
	index++;//m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//+Case
	index++;//m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//-Nmax
	index++;//m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//-Case
	index++;//m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//-Nmaxφ
	index++;//m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//-Caseφ

	CXhChar16 caserule;
	double ctrlstress=0;
	double coefStrength=1.0;
	double A=pElem->pOrgPole->GetArea();
	double An=A-pElem->design_info.hole_out*pElem->design_info.hole_d*pElem->pOrgPole->GetThick();
	STEELMAT *pSteelMat=QuerySteelMatByBriefMark(pElem->pOrgPole->cMaterial);
	if(pElem->design_info.iDangerousLoadCase==2)		//T
	{
		coefStrength=pElem->design_info.fCoefTensStrengthDecrease;
		ctrlstress=pElem->MaxTension.N/An;
	}
	else if(pElem->design_info.iDangerousLoadCase==3)	//C
	{
		coefStrength=pElem->design_info.fCoefCompStrengthDecrease;
		ctrlstress=pElem->MaxCompression.N/An;
	}
	else //if(pElem->design_info.iDangerousLoadCase==4)	//S
	{
		coefStrength=pElem->design_info.mN;
		if(pElem->MaxCompression.N>-EPS||pElem->design_info.fCoefStablity<EPS)
			ctrlstress=0;
		else	//由于最大压稳应力出现工况不一定与最大压稳力工况一致，故不能用最大压稳力除压稳应力控制工况下的压稳系数 wjh-2013.10.15
			ctrlstress=pElem->MaxCompStable.N/(A*pElem->design_info.fCoefStablity);
	}
	double limStrength=pSteelMat->f(pElem->GetThick())*coefStrength;
	if(g_sysPara.iEfficacyCeofType==0)
	{	//0.功效系数
		if(pElem->design_info.iDangerousRule==1)
			caserule.Printf("%5.3f@λ",pElem->design_info.fCoefEffic);
		else if(pElem->design_info.iDangerousRule==2)
		{
			if(pElem->design_info.bBlockShearActivated)
				caserule.Printf("%5.3f@+T",pElem->design_info.fCoefEffic);
			else
				caserule.Printf("%5.3f@ T",pElem->design_info.fCoefEffic);
		}
		else if(pElem->design_info.iDangerousRule==3)
			caserule.Printf("%5.3f@ C",pElem->design_info.fCoefEffic);
		else if(pElem->design_info.iDangerousRule==4)
		{
			if(pElem->MaxCompStable.bothComp)
				caserule.Printf("%5.3f@+S",pElem->design_info.fCoefEffic);
			else
				caserule.Printf("%5.3f@ S",pElem->design_info.fCoefEffic);
		}
		else
			caserule.Printf("%5.3f@ 0",pElem->design_info.fCoefEffic);
	}
	else
	{	//1.承载系数
		double effic=0;
		if(coefStrength>EPS)
			effic=fabs(ctrlstress)/limStrength;
		if(pElem->design_info.iDangerousLoadCase==2)		//T
		{
			if(pElem->design_info.bBlockShearActivated)
				caserule.Printf("%5.3f@+T",effic);
			else
				caserule.Printf("%5.3f@ T",effic);
		}
		else if(pElem->design_info.iDangerousLoadCase==3)	//C
			caserule.Printf("%5.3f@ C",effic);
		else if(pElem->design_info.iDangerousLoadCase==4)	//S
		{
			if(pElem->MaxCompStable.bothComp)
				caserule.Printf("%5.3f@+S",effic);
			else
				caserule.Printf("%5.3f@ S",effic);
		}
	}

	itemText.Printf("%.1f",ctrlstress);
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//σmax
	itemText.Printf("%.1f",limStrength);
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//σa
	itemText.Printf("%.2f",coefStrength);
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//m
	itemText.Printf("%.3f",pElem->design_info.fCoefStablity);
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//φ
	m_listElems.SetSubItemText(pItem,index++,(char*)caserule);	//裕度
	m_bLockedUpdateElemFunc=false;
	}
	catch(char* error)
	{	//MemberCheck(...)中可能会抛出 char* 类型的异常
		AfxMessageBox(error);
	}
}

#endif
