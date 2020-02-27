//<LOCALE_TRANSLATE Confirm by hxr/>
// RevisionOperDlg.cpp : ʵ���ļ�
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
// CRevisionOperDlg �Ի���

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
		else	//�������ѹ��Ӧ�����ֹ�����һ�������ѹ��������һ�£��ʲ��������ѹ������ѹ��Ӧ�����ƹ����µ�ѹ��ϵ�� wjh-2013.10.15
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
	if(itemsort.columnLabel.Equal("��Ԫ��ʶ"))
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
		if(!itemsort.bItemSortType)	//��������
			result=-result;
		return result;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(itemsort.columnLabel.Equal("specification"))
#else
	else if(itemsort.columnLabel.Equal("��  ��"))
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
		if(!itemsort.bItemSortType)	//��������
			result=-result;
		return result;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(itemsort.columnLabel.Equal("area"))
#else
	else if(itemsort.columnLabel.Equal("�����"))
#endif
	{
		int result=0;
		if(elemptr1->pOrgPole->GetArea()>elemptr2->pOrgPole->GetArea())
			result=1;
		else if(elemptr1->pOrgPole->GetArea()<elemptr2->pOrgPole->GetArea())
			result=-1;
		if(!itemsort.bItemSortType)	//��������
			result=-result;
		return result;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(itemsort.columnLabel.Equal("steel material"))
#else
	else if(itemsort.columnLabel.Equal("����"))
#endif
	{
		int result=0;
		CXhChar16 matMark1,matMark2;
		QuerySteelMatMark(elemptr1->pOrgPole->cMaterial,matMark1);
		QuerySteelMatMark(elemptr2->pOrgPole->cMaterial,matMark2);
		result=stricmp(matMark1,matMark2);
		if(!itemsort.bItemSortType)	//��������
			result=-result;
		return result;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(itemsort.columnLabel.Equal("reduce hole numbers"))
#else
	else if(itemsort.columnLabel.Equal("������"))
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
	else if(itemsort.columnLabel.Equal("��˨Ч��"))
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
	else if(itemsort.columnLabel.Equal("��ԪЧ��"))
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
	else if(itemsort.columnLabel.Equal("��max"))
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
	else if(itemsort.columnLabel.Equal("-Nmax��"))
	{
		value[0]=-elemptr1->MaxCompStable.N;
		value[1]=-elemptr2->MaxCompStable.N;
	}
	else	//��ʾ��֧��
		return -2;
	int result=0;
	if(value[0]<value[1])
		result=-1;
	else if(value[0]>value[1])
		result=1;
	if(!itemsort.bItemSortType)	//��������
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
static CHashStrList<PROPLIST_ITEM> propHashtable;	//�����ַ�����ϣ��
static PROPLIST_ITEM* AddPropItem(char* propKey,PROPLIST_ITEM item){
	PROPLIST_ITEM* pItem=propHashtable.SetValue(propKey,item);
	pItem->propKeyStr.Copy(propKey);
	return pItem;
}
//�趨��ʶλ
static DWORD ItemFlag(const char* itemKeyStr,DWORD flag)
{
	PROPLIST_ITEM* pItem=propHashtable.GetValue(itemKeyStr);
	if(pItem)
		pItem->dwFlag=flag;
	else
		flag=0;
	return flag;
}
//���ر�ʶλ��ͬʱ�趨��ʶ����
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
void CRevisionOperDlg::InitPropItemHashtable()	//��ʼ������߰�������Ļ�����Ϣ(���ݼ�ֵ�����͡��������Ƽ�������ʾ��
{
	int id = 1;
	propHashtable.SetHashTableGrowSize(30);
	AddPropItem("elemid",PROPLIST_ITEM(id++,"��Ԫ��ʶ"))->dwFlag=0x07;
	AddPropItem("symmetry",PROPLIST_ITEM(id++,"�Գ�"))->dwFlag=0x01;
	AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"))->dwFlag=0x07;
	AddPropItem("type",PROPLIST_ITEM(id++,"�������"))->dwFlag=0x01;
	AddPropItem("spec",PROPLIST_ITEM(id++,"���"))->dwFlag=0x07;
	AddPropItem("area",PROPLIST_ITEM(id++,"�����"))->dwFlag=0x01;
	AddPropItem("material",PROPLIST_ITEM(id++,"����"))->dwFlag=0x07;
	AddPropItem("StatMatI",PROPLIST_ITEM(id++,"ͳ�ĺ�"))->dwFlag=0x01;
	AddPropItem("HorR",PROPLIST_ITEM(id++,"�˲�Լ��"))->dwFlag=0x06;
	AddPropItem("EndState",PROPLIST_ITEM(id++,"�˶�״̬"))->dwFlag=0x02;
	AddPropItem("BoltInfo",PROPLIST_ITEM(id++,"��˨��Ϣ"))->dwFlag=0x01;
	AddPropItem("BoltEffic",PROPLIST_ITEM(id++,"��˨Ч��"))->dwFlag=0x01;
	AddPropItem("HoleReduct",PROPLIST_ITEM(id++,"����"))->dwFlag=0x01;
	AddPropItem("��",PROPLIST_ITEM(id++,"���㳤��ϵ��"))->dwFlag=0x04;
	AddPropItem("Lo",PROPLIST_ITEM(id++,"���㳤��"))->dwFlag=0x04;
	AddPropItem("R",PROPLIST_ITEM(id++,"��ת�뾶"))->dwFlag=0x04;
	AddPropItem("KLo/r",PROPLIST_ITEM(id++,"���㳤ϸ��"))->dwFlag=0x04;
	AddPropItem("+Nmax",PROPLIST_ITEM(id++,"�������"))->dwFlag=0x02;
	AddPropItem("+Case",PROPLIST_ITEM(id++,"�����������"))->dwFlag=0x02;
	AddPropItem("-Nmax",PROPLIST_ITEM(id++,"���ѹ��"))->dwFlag=0x06;
	AddPropItem("-Case",PROPLIST_ITEM(id++,"���ѹ������"))->dwFlag=0x06;
	AddPropItem("-Nmax��",PROPLIST_ITEM(id++,"���ѹ����"))->dwFlag=0x04;
	AddPropItem("-Case��",PROPLIST_ITEM(id++,"���ѹ�ȹ���"))->dwFlag=0x04;
	AddPropItem("��max",PROPLIST_ITEM(id++,"Ӧ������ֵ"))->dwFlag=0x06;
	AddPropItem("��a",PROPLIST_ITEM(id++,"Ӧ������ֵ"))->dwFlag=0x06;
	AddPropItem(" m",PROPLIST_ITEM(id++,"ǿ���ۼ�ϵ��"))->dwFlag=0x02;
	AddPropItem("��",PROPLIST_ITEM(id++,"ѹ��ϵ��"))->dwFlag=0x04;
	AddPropItem("ElemEffic",PROPLIST_ITEM(id++,"��Ԫ����Ч��"))->dwFlag=0x06;
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
		AfxMessageBox(CXhChar200("��ԪУ�������ļ�{%s}д��ʧ��,Win7��Win8�����߰汾ϵͳ�������Թ���Ա����������!",(char*)revision_cfg_file));
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
		AfxMessageBox(CXhChar200("��ԪУ�������ļ�{%s}д��ʧ��,Win7��Win8�����߰汾ϵͳ�������Թ���Ա����������!",(char*)revision_cfg_file));
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
		InitPropItemHashtable();	//��ʼ������߰�������Ļ�����Ϣ(���ݼ�ֵ�����͡��������Ƽ�������ʾ��
	InitFromElemRevCfgFile();	//��ȡ֮ǰ��Ĭ�������ļ�
#ifdef AFX_TARG_ENU_ENGLISH
	i=m_listElems.AddColumnHeader("unit mark",90,ItemFlag(i+1,"elemid"),"two end's nodes number of the unit ");
	i=m_listElems.AddColumnHeader("symmetry",40,ItemFlag(i+1,"symmetry"),"unit's symmetry information");
	i=m_listElems.AddColumnHeader("layer",40,ItemFlag(i+1,"layer"),"use the second character of the layer name to mark rod's function,for example:Z:steel material,X:V-titled material,H:across material and F:auxiliary material.");
	i=m_listElems.AddColumnHeader("type",40,ItemFlag(i+1,"type"),"rod's material type,for example L:angle,��:tube.");//��[:U-steel
	i=m_listElems.AddColumnHeader("spec",65,ItemFlag(i+1,"spec"));
	i=m_listElems.AddColumnHeader("area",65,ItemFlag(i+1,"area"));
	i=m_listElems.AddColumnHeader("material",55,ItemFlag(i+1,"material"));
	i=m_listElems.AddColumnHeader("BOM",40,ItemFlag(i+1,"StatMatI"),"the mark numbers used to unify a group of rods,\"0X\" begining shows belonging to the same rod.");//��[:U-steel
	i=m_listElems.AddColumnHeader("H,R",35,ItemFlag(i+1,"HorR"),"H:articulate,R:rigid connect(with partly rotation limit).");
	i=m_listElems.AddColumnHeader("rod's end situation",65,ItemFlag(i+1,"EndState"),"F:no limitation for rotation; P:partly limitation for rotation; C:center stressed; E:Eccentric Force��");
	i=m_listElems.AddColumnHeader("bolt info",80,ItemFlag(i+1,"BoltInfo"));
	i=m_listElems.AddColumnHeader("bolt efficiency",70,ItemFlag(i+1,"BoltEffic"),"bolt actual maximum load /limit load ratio");
	i=m_listElems.AddColumnHeader("reduce hole number",60,ItemFlag(i+1,"HoleReduct"));
	i=m_listElems.AddColumnHeader("��",50,ItemFlag(i+1,"��"),"ratio between unit computation length under controlling condition and original length");
	i=m_listElems.AddColumnHeader("Lo",50,ItemFlag(i+1,"Lo"),"unit computation length under controlling condition,mm");
	i=m_listElems.AddColumnHeader("R",50,ItemFlag(i+1,"R"),"Current active gyration,mm");
	i=m_listElems.AddColumnHeader("KLo/r",60,ItemFlag(i+1,"KLo/r"),"the slenderness ratio after Considering the correction coefficient of Angle part");
	i=m_listElems.AddColumnHeader("+Nmax",60,ItemFlag(i+1,"+Nmax"),"maximum pull, kN");
	i=m_listElems.AddColumnHeader("+Case",60,ItemFlag(i+1,"+Case"),"module and working condition group when the maximum pull appears");
	i=m_listElems.AddColumnHeader("-Nmax",65,ItemFlag(i+1,"-Nmax"),"the maximum stress, kN");
	i=m_listElems.AddColumnHeader("-Case",60,ItemFlag(i+1,"-Case"),"module and working condition group when the maximum stress appears");
	i=m_listElems.AddColumnHeader("-Nmax��",65,ItemFlag(i+1,"-Nmax��"),"pressure when maximum pressure steady stress appears, kN");
	i=m_listElems.AddColumnHeader("-Case��",60,ItemFlag(i+1,"-Case��"),"module and working condition group when maximum pressure steady stress appears");
	i=m_listElems.AddColumnHeader("��max",70,ItemFlag(i+1,"��max"),"maximum design stress, MPa");
	i=m_listElems.AddColumnHeader("��a",70,ItemFlag(i+1,"��a"),"permissible stress, MPa");
	i=m_listElems.AddColumnHeader(" m",45,ItemFlag(i+1," m"),"strength reduction factor under Control working condition");
	i=m_listElems.AddColumnHeader("��",50,ItemFlag(i+1,"��"),"Axial compression rod stability coefficient");
	i=m_listElems.AddColumnHeader("elem effic",70,ItemFlag(i+1,"ElemEffic"),"under Control working condition,  ratio coefficient of index calculating value and the ultimate value (including slenderness ratio)��should be less than 1.0");
#else
	i=m_listElems.AddColumnHeader("��Ԫ��ʶ",90,ItemFlag(i+1,"elemid"),"��Ԫ�����˽ڵ���");
	i=m_listElems.AddColumnHeader("�Գ�",40,ItemFlag(i+1,"symmetry"),"��Ԫ�ĶԳ���Ϣ");
	i=m_listElems.AddColumnHeader("ͼ��",40,ItemFlag(i+1,"layer"),"ͼ�����ڣ����ַ������Ա�ʶ�˼����ܣ���Z:���ġ�X:б�ġ�H:��ļ�F:���ġ�");
	i=m_listElems.AddColumnHeader("����",40,ItemFlag(i+1,"type"),"�˼��������ͣ���L:�Ǹ֡���:�ֹܡ�");//��[:�۸�
	i=m_listElems.AddColumnHeader("��  ��",65,ItemFlag(i+1,"spec"));
	i=m_listElems.AddColumnHeader("�����",65,ItemFlag(i+1,"area"));
	i=m_listElems.AddColumnHeader("����",55,ItemFlag(i+1,"material"));
	i=m_listElems.AddColumnHeader("ͳ��",40,ItemFlag(i+1,"StatMatI"),"ͳһһ��˼�ʱ���õı�ʶ��ţ�\"0X\"��ͷ��ʾͬʱ������ĳһ�˼���");//��[:�۸�
	i=m_listElems.AddColumnHeader("H,R",35,ItemFlag(i+1,"HorR"),"H:�½�,R:�ս�(�в���ת��Լ��)��");
	i=m_listElems.AddColumnHeader("�˶�״��",65,ItemFlag(i+1,"EndState"),"F:��ת��Լ��; P:����ת��Լ��; C:��������; E:ƫ��������");
	i=m_listElems.AddColumnHeader("��˨��Ϣ",80,ItemFlag(i+1,"BoltInfo"));
	i=m_listElems.AddColumnHeader("��˨Ч��",70,ItemFlag(i+1,"BoltEffic"),"��˨ʵ�������ܺ���/���޳��ܺ��ر�ֵ");
	i=m_listElems.AddColumnHeader("������",60,ItemFlag(i+1,"HoleReduct"));
	i=m_listElems.AddColumnHeader("��",50,ItemFlag(i+1,"��"),"���������µĵ�Ԫ���㳤����ԭʼ���ȵı�ֵ");
	i=m_listElems.AddColumnHeader("Lo",50,ItemFlag(i+1,"Lo"),"���������µĵ�Ԫ���㳤��,mm");
	i=m_listElems.AddColumnHeader("R",50,ItemFlag(i+1,"R"),"ִ�еĻ�ת�뾶,mm");
	i=m_listElems.AddColumnHeader("KLo/r",60,ItemFlag(i+1,"KLo/r"),"���ǽǸֹ�����ϸ������ϵ����ĳ�ϸ��ֵ");
	i=m_listElems.AddColumnHeader("+Nmax",60,ItemFlag(i+1,"+Nmax"),"�������, kN");
	i=m_listElems.AddColumnHeader("+Case",60,ItemFlag(i+1,"+Case"),"����������ֵĺ���-�������");
	i=m_listElems.AddColumnHeader("-Nmax",65,ItemFlag(i+1,"-Nmax"),"���ѹ��, kN");
	i=m_listElems.AddColumnHeader("-Case",60,ItemFlag(i+1,"-Case"),"���ѹ�����ֵĺ���-�������");
	i=m_listElems.AddColumnHeader("-Nmax��",65,ItemFlag(i+1,"-Nmax��"),"���ѹ��Ӧ������ʱ��ѹ��, kN");
	i=m_listElems.AddColumnHeader("-Case��",60,ItemFlag(i+1,"-Case��"),"���ѹ��Ӧ�����ֵĺ���-�������");
	i=m_listElems.AddColumnHeader("��max",70,ItemFlag(i+1,"��max"),"������Ӧ��, MPa");
	i=m_listElems.AddColumnHeader("��a",70,ItemFlag(i+1,"��a"),"����Ӧ��, MPa");
	i=m_listElems.AddColumnHeader(" m",45,ItemFlag(i+1," m"),"���ƹ������������µ�ǿ���ۼ�ϵ��");
	i=m_listElems.AddColumnHeader("��",50,ItemFlag(i+1,"��"),"������ѹ�˼��ȶ�ϵ��");
	i=m_listElems.AddColumnHeader("��ԪЧ��",70,ItemFlag(i+1,"ElemEffic"),"���������£�ָ�����ֵ�뼫��ֵ�ı�ֵϵ��(����ϸ��)��ӦС��1.0");
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
	if(Ta.IsHasRsltBuffer(m_sActiveRsltItemKeyStr.GetBuffer())&&AfxMessageBox("�Ѵ��ڸ����Ƶļ�����������������ԭ������!",MB_YESNO)!=IDYES)
		return;
	if(!Ta.IsHasRsltBuffer(m_sActiveRsltItemKeyStr.GetBuffer()))
	{
		CComboBox* pCmbBox=(CComboBox*)GetDlgItem(IDC_CMB_ACTIVE_RSLT_ITEM_TITLE);
		pCmbBox->AddString(m_sActiveRsltItemKeyStr);
	}
	
	CBuffer buffer;
	postreport.ToBuffer(buffer);	//����ǰ�������������д�뵽һ�����ݻ������
	if(Ta.WriteRsltBuffer(m_sActiveRsltItemKeyStr,buffer))
	{
		postreport.m_sRsltItem.Copy(m_sActiveRsltItemKeyStr);
		MessageBox(CXhChar50("������[%s]����ɹ�",m_sActiveRsltItemKeyStr));
	}
	else
		AfxMessageBox(CXhChar50("������[%s]����ʧ��",m_sActiveRsltItemKeyStr));
}

void CRevisionOperDlg::OnBnClickedReadRsltItem()
{
	UpdateData();
	if(m_sActiveRsltItemKeyStr.Compare("***")!=0)
	{
		CBuffer buffer;
		if(!Ta.ReadRsltBuffer(m_sActiveRsltItemKeyStr,buffer))
		{
			AfxMessageBox(CXhChar50("������[%s]��ȡʧ��",m_sActiveRsltItemKeyStr));
			return;
		}
		BeginWaitCursor();
		buffer.SeekToBegin();
		postreport.FromBuffer(buffer);
		postreport.m_sRsltItem.Copy(m_sActiveRsltItemKeyStr);
		EndWaitCursor();
		MessageBox(CXhChar50("������[%s]�Ѷ���",m_sActiveRsltItemKeyStr));
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
			MessageBox(CXhChar50("������[%s]ɾ���ɹ�!",m_sActiveRsltItemKeyStr));
		}
	}
	else
		AfxMessageBox("�Ѵ��������δ�ҵ���ǰ�ɾ��ʧ��!");
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
				AfxMessageBox(CXhChar50("������[%s]��ȡʧ��",m_sActiveRsltItemKeyStr));
				return;
			}
			buffer.SeekToBegin();
			postreport.FromBuffer(buffer);
			postreport.m_sRsltItem.Copy(m_sActiveRsltItemKeyStr);
		}
		//����ļ�Ŀ¼����
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
	//ȷ������������
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
	if(valueStr.Compare("��ʾ")==0)
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
	//ѡ���뵱ǰ��Ԫ�ԳƵĹ�����Ԫ
	pPropPage->m_propList.Lock();
	for(i=0;i<elem_arr.Size();i++)
	{
		CLDSNode *pMirStart=NULL,*pMirEnd=NULL;
		//ѡ��X��ԳƵ�Ԫ
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
		//ѡ��Y��ԳƵ�Ԫ
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
		//ѡ��Z��ԳƵ�Ԫ
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
			pModelPage->ShiftActiveItemByObj(pModule);	//����ģ�����е�ǰ���߽ڵ�ļ���״̬
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
                    break;	//�����������ѡ���ҵ���У������ǰѡ����һ�µĹ�����Ԫ
			}
		}
	}
	if(pRelaFemElem==NULL)
	{	//�����������У������һ��ʱ,��������������
	}
#ifdef AFX_TARG_ENU_ENGLISH
	if(colHeader.Equal("spec"))
#else
	if(colHeader.Equal("��  ��"))
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
	else if(colHeader.Equal("����"))
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
	if(pRelaFemElem)	//����������൥Ԫ������
		pMainWnd->GetLDSView()->DisplayFemElementProperty(FALSE);
	return TRUE;
}
// CRevisionOperDlg ��Ϣ�������

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
	m_tabCtrlListGroup.InsertItem(0,"����");
	m_tabCtrlListGroup.InsertItem(1,"����");
	m_tabCtrlListGroup.InsertItem(2,"ǿ��");
	m_tabCtrlListGroup.InsertItem(3,"ѹ��");
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
	// �쳣: OCX ����ҳӦ���� FALSE
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
		return "��ʾ";
	else
		return "����ʾ";
#endif
}
void CRevisionOperDlg::SetColumnItemState(int idProp,bool bDisplay)
{
	int classify=idProp/100;
	int idItem=idProp%100;
	DWORD flag=GetSingleWord(classify);
	if(bDisplay)
		m_listElems.GetColumnInfo(idItem-1)->dwFlag|=flag;
	else if(m_listElems.GetColumnInfo(idItem-1)->dwFlag&flag)	//�Ƴ���ʾ״̬
		m_listElems.GetColumnInfo(idItem-1)->dwFlag^=flag;
	for(PROPLIST_ITEM* pItem=propHashtable.GetFirst();pItem;pItem=propHashtable.GetNext())
	{
		if(pItem->id!=idItem-1)
			continue;
		pItem->dwFlag=m_listElems.GetColumnInfo(idItem-1)->dwFlag;
		break;
	}
	WriteToElemRevCfgFile();	//�Ķ����ú��Զ��洢�����ļ�
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
	lpInfo->m_strPropName = "��Ԫ��ʶ";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "��Ԫ��ʶ";
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
	lpInfo->m_strPropName = "�Գ���Ϣ";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "�Գ���Ϣ";
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
	lpInfo->m_strPropName = "ͼ��";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "ͼ�㣬�ɱ�ʾ�˼��������ͣ������ġ�б�ġ���ĸ��ĵ�";
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
	lpInfo->m_strPropName = "����";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "��������";
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
	lpInfo->m_strPropName = "���";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "���";
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
	lpInfo->m_strPropName = "�����";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "�����, cm2";
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
	lpInfo->m_strPropName = "����";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "����";
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
	lpInfo->m_strPropName = "ͳ�ĺ�";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "ͳ�ĺ�";
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
	lpInfo->m_strPropHelp = "connecting type,H:articulate��R:rigid connect.";
#else
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "��������,H:�½ӣ�R:�սӡ�";
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
	lpInfo->m_strPropName = "�˶�״��";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "�˶�Լ��,F:��ת��Լ��,P:����ת��Լ��; �˶�����,C:��������,E:ƫ��������";
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
	lpInfo->m_strPropName = "��˨��Ϣ";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "��˨��Ϣ";
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
	lpInfo->m_strPropName = "��˨Ч��";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "��˨ʵ�������ܺ���/���޳��ܺ��ر�ֵ";
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
	lpInfo->m_strPropName = "������";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "������";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "��";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "calculate length comparative coefficient";
#else
	lpInfo->m_strPropName = "��";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "���㳤���൱ϵ��";
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
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "���㳤��";
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
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "���㳤��";
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
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "��ϸ��";
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
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "�������,kN";
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
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "����������ֵĺ��߼�����";
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
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "���ѹ��,kN";
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
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "���ѹ�����ֵĺ��߼�����";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "-Nmax��";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "pressure when maximum pressure stability appears";,kN";
#else
	lpInfo->m_strPropName = "-Nmax��";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "���ѹ��Ӧ������ʱ��ѹ��,kN";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "-Case��";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "module and working conditions when maximum pressure stability stress appears";
#else
	lpInfo->m_strPropName = "-Case��";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "���ѹ��Ӧ�����ֵĺ��߼�����";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "��max";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "maximum stress,MPa";
#else
	lpInfo->m_strPropName = "��max";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "���Ӧ��,MPa";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "��a";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "persimissible stress,MPa";
#else
	lpInfo->m_strPropName = "��a";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "����Ӧ��,MPa";
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
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "ǿ���ۼ�ϵ��";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = group+(++serial);
	lpInfo->m_strPropValue=GetPropValueStr(pPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "��";
	lpInfo->m_cmbItems="display|no display";
	lpInfo->m_strPropHelp = "pressure stability factor";
#else
	lpInfo->m_strPropName = "��";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "ѹ��ϵ��";
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
	lpInfo->m_strPropName = "��ԪЧ��";
	lpInfo->m_cmbItems="��ʾ|����ʾ";
	lpInfo->m_strPropHelp = "���ֵ/����ֵ�ı�ֵϵ��";
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
	lpInfo->m_strPropName = "����";
	lpInfo->m_strPropHelp = "�˼���񼰵�Ԫ������˨��Ϣ";
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
	lpInfo->m_strPropName = "ǿ��";
	lpInfo->m_strPropHelp = "�˼���񼰵�Ԫ������˨��Ϣ";
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
	lpInfo->m_strPropName = "ѹ��";
	lpInfo->m_strPropHelp = "�˼���񼰵�Ԫ������˨��Ϣ";
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
	if(iSelPage==0)// TODO: �ڴ���ӿؼ�֪ͨ����������
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

	// TODO: �ڴ˴������Ϣ����������
}
void CRevisionOperDlg::OnLvnColumnclickOutputInfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
#ifdef AFX_TARG_ENU_ENGLISH
	if( m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("mark unit")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("specification")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("steel material")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("+Nmax")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("-Nmax")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("-Nmax��")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("��max")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("bolt efficiency")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("reduce hole number")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("elem effic"))
#else
	if( m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("��Ԫ��ʶ")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("��  ��")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("����")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("+Nmax")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("-Nmax")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("-Nmax��")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("��max")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("��˨Ч��")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("������")||
		m_listElems.GetColumnInfo(pNMLV->iSubItem)->headerName.Equal("��ԪЧ��"))
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
		pInfo->AddSubItemText((char*)itemText,TRUE);	//��Ԫ��ʶ
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
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//�Գ���Ϣ
		if(pElem->pOrgPole)	//ͼ��
			pInfo->SetSubItemText(index++,CXhChar16("%C",pElem->pOrgPole->layer(1)),TRUE);
		else
			pInfo->SetSubItemText(index++,"",TRUE);
		if(pElem->pOrgPole)	//��������
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
				pInfo->SetSubItemText(index++,_T("��"),TRUE);
			else if(pElem->pOrgPole->size_idClassType==CLS_LINESLOT)
				pInfo->SetSubItemText(index++,_T(" ["),TRUE);
			else
				pInfo->SetSubItemText(index++,_T(" U"),TRUE);
		}
		else
			pInfo->SetSubItemText(index++,"",TRUE);
		itemText.Printf("%sX%s",(char*)CXhChar16(pElem->GetWidth()),(char*)CXhChar16(pElem->GetThick()));
		pInfo->SetSubItemText(index++,(char*)itemText);	//���
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
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//�����, cm2

		//itemText.Printf("%C",pElem->pOrgPole->cMaterial);
		QuerySteelMatMark(pElem->pOrgPole->cMaterial,itemText);
		pInfo->SetSubItemText(index++,(char*)itemText);	//����
		pInfo->SetControlType(index-1,GCT_CMB_LIST);
		itemsStr=MakeMaterialMarkSetString();
		pInfo->SetListItemsStr(index-1,itemsStr);

		if(pElem->pOrgPole->m_uStatMatNo>0)
			itemText.Printf("%d",pElem->pOrgPole->m_uStatMatNo);
		else
			itemText.Copy("");
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//ͳ�ĺ�
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
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//��������
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
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//�˶�����
		if(pElem->pStart==pElem->pOrgPole->pStart)
			itemText.Printf("%sM%dX%d",pElem->pOrgPole->connectStart.grade,pElem->pOrgPole->connectStart.d,pElem->pOrgPole->connectStart.wnSummBoltN);
		else if(pElem->pEnd==pElem->pOrgPole->pEnd)
			itemText.Printf("%sM%dX%d",pElem->pOrgPole->connectEnd.grade,pElem->pOrgPole->connectEnd.d,pElem->pOrgPole->connectEnd.wnSummBoltN);
		else
			itemText.Printf("--");
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);//itemText.Equal("--"));	//��˨��Ϣ
		if(pElem->pStart==pElem->pOrgPole->pStart)
			itemText.Printf("%.3f",pElem->pOrgPole->connectStart.bearloadcoef);
		else if(pElem->pEnd==pElem->pOrgPole->pEnd)
			itemText.Printf("%.3f",pElem->pOrgPole->connectEnd.bearloadcoef);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//��˨Ч��
		if(pElem->pStart==pElem->pOrgPole->pStart)
			itemText.Printf("%.2f",pElem->design_info.hole_out);
		else if(pElem->pEnd==pElem->pOrgPole->pEnd)
			itemText.Printf("%.2f",pElem->design_info.hole_out);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//������
		double L=DISTANCE(pElem->pStart->Position(false),pElem->pEnd->Position(false));
		//itemText.Printf("%.3f",pElem->LamdaL()/L);
		CExpression::ToExpression(pElem->LamdaL()/L,3,itemText,50);
		SimplifiedNumString(itemText);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//��
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
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//-Nmax��
		hashModuleNo.GetValueAt(pElem->MaxCompStable.hModule,module_no);
		hashStatusNo.GetValueAt(pElem->MaxCompStable.hWorkStatus,status_no);
		itemText.Printf("%2d-%3d",module_no,status_no);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//-Case��

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
			else	//�������ѹ��Ӧ�����ֹ�����һ�������ѹ��������һ�£��ʲ��������ѹ������ѹ��Ӧ�����ƹ����µ�ѹ��ϵ�� wjh-2013.10.15
				ctrlstress=pElem->MaxCompStable.N/(A*pElem->design_info.fCoefStablity);
		}
		double limStrength=pSteelMat->f(pElem->GetThick())*coefStrength;
		if(g_sysPara.iEfficacyCeofType==0)
		{
			if(pElem->design_info.iDangerousRule==1)
				caserule.Printf("%5.3f@��",pElem->design_info.fCoefEffic);
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
				caserule.Printf("���");
#endif
		}

		itemText.Printf("%.1f",ctrlstress);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//��max
		itemText.Printf("%.1f",limStrength);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//��a
		itemText.Printf("%.2f",coefStrength);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//m
		itemText.Printf("%.3f",pElem->design_info.fCoefStablity);
		pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//��
		pInfo->SetSubItemText(index++,(char*)caserule,TRUE);	//ԣ��
		CSuperGridCtrl::CTreeItem* pItem=m_listElems.InsertRootItem(pInfo,FALSE);
		pItem->m_idProp=pElem->id;
		//m_listElems.AddColumnHeader("�Գ���Ϣ",80);
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
		return;	//δ�ҵ���Ӧ�ĵ�Ԫ

	CSuperGridCtrl::CTreeItem *pItem=m_listElems.GetTreeItem(iSelected);
	CElement* pElem=postreport.hashElems.GetValue(pItem->m_idProp);
	if(pElem==NULL||pElem->pStart==NULL||pElem->pEnd==NULL)
		return;
	m_bLockedUpdateElemFunc=true;
	int index=1;
	CXhChar50 itemText;
	//m_listElems.SetSubItemText(pItem,0,itemText);			//0.��Ԫ��ʶ
	index++;//pInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//1.�Գ���Ϣ
	//������Ϣ�����漰���������,��Ҫ���¼������,�ʴ˴�����MemberCheck���� wjh-2013.2.24
	pElem->pParaType=NULL;	//��pParaType�������Ż�ѡ��ʱ��ʱ���ع����Ϣ,���ȼ����ڸ˼���������Ϣ,�˴��踳��
	MemberCheck(pElem,TRUE,Ta.m_iMemberSizeCheckCode,false);
	if(pElem->pOrgPole)										//2.ͼ����Ϣ
		m_listElems.SetSubItemText(pItem,index++,CXhChar16("%C",pElem->pOrgPole->layer(1)));
	else
		m_listElems.SetSubItemText(pItem,index++,"");
	if(pElem->pOrgPole)		//3.��������
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
			m_listElems.SetSubItemText(pItem,index++,_T("��"));
		else if(pElem->pOrgPole->size_idClassType==CLS_LINESLOT)
			m_listElems.SetSubItemText(pItem,index++,_T(" ["));
		else
			m_listElems.SetSubItemText(pItem,index++,_T(" U"));
	}
	else
		m_listElems.SetSubItemText(pItem,index++,"");
	itemText.Printf("%sX%s",(char*)CXhChar16(pElem->GetWidth()),(char*)CXhChar16(pElem->GetThick()));
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//4.���
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
	pItem->m_lpNodeInfo->SetSubItemText(index++,(char*)itemText,TRUE);	//�����, cm2

	//itemText.Printf("%C",pElem->pOrgPole->cMaterial);
	QuerySteelMatMark(pElem->pOrgPole->cMaterial,itemText);
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//5.����

	if(pElem->pOrgPole->m_uStatMatNo>0)
		itemText.Printf("%d",pElem->pOrgPole->m_uStatMatNo);
	else
		itemText.Copy("");
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//6.ͳ�ĺ�
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
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//��������
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
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//�˶�״��
	if(pElem->pStart==pElem->pOrgPole->pStart)
		itemText.Printf("%sM%dX%d",pElem->pOrgPole->connectStart.grade,pElem->pOrgPole->connectStart.d,pElem->pOrgPole->connectStart.wnSummBoltN);
	else if(pElem->pEnd==pElem->pOrgPole->pEnd)
		itemText.Printf("%sM%dX%d",pElem->pOrgPole->connectEnd.grade,pElem->pOrgPole->connectEnd.d,pElem->pOrgPole->connectEnd.wnSummBoltN);
	else
		itemText.Printf("--");
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);//itemText.Equal("--"));	//��˨��Ϣ
	if(pElem->pStart==pElem->pOrgPole->pStart)
		itemText.Printf("%.3f",pElem->pOrgPole->connectStart.bearloadcoef);
	else if(pElem->pEnd==pElem->pOrgPole->pEnd)
		itemText.Printf("%.3f",pElem->pOrgPole->connectEnd.bearloadcoef);
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//��˨Ч��
	if(pElem->pStart==pElem->pOrgPole->pStart)
		itemText.Printf("%.2f",pElem->design_info.hole_out);
	else if(pElem->pEnd==pElem->pOrgPole->pEnd)
		itemText.Printf("%.2f",pElem->design_info.hole_out);
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//������
	double L=DISTANCE(pElem->pStart->Position(false),pElem->pEnd->Position(false));
	//itemText.Printf("%.3f",pElem->LamdaL()/L);
	CExpression::ToExpression(pElem->LamdaL()/L,3,itemText,50);
	SimplifiedNumString(itemText);
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//��
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
	index++;//m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//-Nmax��
	index++;//m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//-Case��

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
		else	//�������ѹ��Ӧ�����ֹ�����һ�������ѹ��������һ�£��ʲ��������ѹ������ѹ��Ӧ�����ƹ����µ�ѹ��ϵ�� wjh-2013.10.15
			ctrlstress=pElem->MaxCompStable.N/(A*pElem->design_info.fCoefStablity);
	}
	double limStrength=pSteelMat->f(pElem->GetThick())*coefStrength;
	if(g_sysPara.iEfficacyCeofType==0)
	{	//0.��Чϵ��
		if(pElem->design_info.iDangerousRule==1)
			caserule.Printf("%5.3f@��",pElem->design_info.fCoefEffic);
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
	{	//1.����ϵ��
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
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//��max
	itemText.Printf("%.1f",limStrength);
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//��a
	itemText.Printf("%.2f",coefStrength);
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//m
	itemText.Printf("%.3f",pElem->design_info.fCoefStablity);
	m_listElems.SetSubItemText(pItem,index++,(char*)itemText);	//��
	m_listElems.SetSubItemText(pItem,index++,(char*)caserule);	//ԣ��
	m_bLockedUpdateElemFunc=false;
	}
	catch(char* error)
	{	//MemberCheck(...)�п��ܻ��׳� char* ���͵��쳣
		AfxMessageBox(error);
	}
}

#endif
