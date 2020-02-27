//<LOCALE_TRANSLATE Confirm by hxr/>
// ShankDefCls.cpp: implementation of the CShankSlope class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "lds.h"
#include "ShankDefCls.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#if defined(__LDS_)||defined(__TSA_)||defined(__TDA_)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShankSlope::CShankSlope()
{
	cObjType='K';
	m_cMainRodType = 'L';
	m_pShankDef=NULL;
	m_fTopFrontWidth=m_fTopSideWidth=m_fTopCoordZ=0;
}

CShankSlope::~CShankSlope()
{
	
}

CShankSegment* CShankSlope::AppendSegment()
{
	CShankSegment *pSegment=segList.append();
	pSegment->m_pShankDef=m_pShankDef;
	pSegment->m_pParent=this;
	return pSegment;
}
CShankSegment* CShankSlope::InsertSegment(int index/*=-1*/)
{
	CShankSegment segment;
	segment.m_pShankDef=m_pShankDef;
	segment.m_pParent=this;
	CShankSegment *pSegment=segList.insert(segment,index);
	return pSegment;
}
bool CShankSlope::DeleteSegmentAtIndex(int index)
{
	return segList.RemoveAt(index);
}

CShankSegment* CShankSlope::SegmentAtIndex(int index)
{
	return &segList[index];
}
int CShankSlope::GetSegmentIndex(CShankSegment* pSeg)
{
	return segList.GetIndex(pSeg);
}
double CShankSlope::SlopeBodyHeight()
{
	double height=0;
	for(int i=0;i<segList.GetNodeNum();i++)
		height+=SegmentAtIndex(i)->m_fHeight;
	return height;
}
CShankModule* CShankSlope::AppendModule()
{
	CShankModule* pModule=m_pShankDef->Modules.append();
	pModule->m_pShankDef=m_pShankDef;
	pModule->m_pParent=this;
	modulePtrList.append(pModule);
	return pModule;
}

CShankModule* CShankSlope::ModuleAtIndex(int index)
{
	if(index<0||index>=modulePtrList.GetSize())
		return NULL;
	else
		return modulePtrList[index];
}
bool CShankSlope::DeleteModuleAtIndex(int index)
{
	return modulePtrList.RemoveAt(index);
}
CShankLeg* CShankSlope::GetDatumLeg()
{
	int i,j;
	CShankLeg *pLeg=NULL;
	//�������¶��µĺ��߷����в��һ�׼����
	for(i=0;i<modulePtrList.GetSize();i++)
	{
		CShankModule *pShankModule=modulePtrList[i];
		for(j=0;j<pShankModule->SegmentNum();j++)
		{
			pLeg =pShankModule->SegmentAtIndex(j)->GetDatumLeg();
			if(pLeg!=NULL)
				return pLeg;
		}
		for(j=0;j<pShankModule->LegNum();j++)
		{
			if(pShankModule->LegAtIndex(j)->m_fFrontWidth>0&&pShankModule->LegAtIndex(j)->m_fSideWidth>0)
				return pShankModule->LegAtIndex(j);
		}
	}
	//����¶�������û�к��߷������ڷֶ������¼������һ�׼����
	for(i=0;i<SegmentNum();i++)
	{
		CShankLeg *pDatumLeg=SegmentAtIndex(i)->GetDatumLeg();
		if(pDatumLeg)
			return pDatumLeg;
	}
	return NULL;
}
bool CShankSlope::GetDatumLegFrontWidth(double* pdfWidth)
{	//��׼��������������
	CShankLeg *pDatumLeg = GetDatumLeg();
	if (pDatumLeg&&pdfWidth)
	{
		*pdfWidth = pDatumLeg->m_fFrontWidth;
		return true;
	}
	return false;
}
bool CShankSlope::GetDatumLegSideWidth(double* pdfWidth)
{	//��׼�Ȳ�����������
	CShankLeg *pDatumLeg = GetDatumLeg();
	if (pDatumLeg&&pdfWidth)
	{
		*pdfWidth = pDatumLeg->m_fSideWidth;
		return true;
	}
	return false;
}
bool CShankSlope::GetDatumLegHeight(double* pdfDatumLegZ)
{
	int i,j;
	double dfDatumLegZ = 0,height=0;
	if (pdfDatumLegZ == NULL)	//��֤��Ϊ��
		pdfDatumLegZ = &dfDatumLegZ;
	//�������¶��µĺ��߷����в��һ�׼����
	for(i=0;i<modulePtrList.GetSize();i++)
	{
		CShankModule *pShankModule=modulePtrList[i];
		height=0;
		for(j=0;j<pShankModule->SegmentNum();j++)
		{
			double h=pShankModule->SegmentAtIndex(j)->GetDatumLegHeight();
			if (h > 0)
			{
				*pdfDatumLegZ = height + h;	//return height + h;
				return true;
			}
		}
		for(j=0;j<pShankModule->LegNum();j++)
		{
			if(pShankModule->LegAtIndex(j)->m_fFrontWidth>0&&pShankModule->LegAtIndex(j)->m_fSideWidth>0)
			{
				for(int k=0;k<pShankModule->SegmentNum();k++)	//���߷����µķֶ�����
					height+=pShankModule->SegmentAtIndex(i)->m_fHeight;
				*pdfDatumLegZ = height + pShankModule->LegAtIndex(j)->m_fHeight;	//return height+pShankModule->LegAtIndex(j)->m_fHeight;
				return true;
			}
		}
	}
	//����¶�������û�к��߷������ڷֶ������¼������һ�׼����
	height=0;
	for(i=0;i<SegmentNum();i++)
	{
		CShankSegment *pSegment=SegmentAtIndex(i);
		double h=pSegment->GetDatumLegHeight();
		if (h > 0)	//�ҵ���׼��
		{
			*pdfDatumLegZ = height + h;	//return height + h;
			return true;
		}
		else
			height+=pSegment->m_fHeight;
	}
	return false;
}
//����ָ�����ȸ߶ȣ�Ҫ��pLeg����ָ��ǰ�¶��е�ĳһ���ȶ���
double CShankSlope::GetSpecLegHeight(CShankLeg *pLeg)
{
	int i,j;
	//�������¶��µĺ��߷����в��һ�׼����
	for(i=0;i<modulePtrList.GetSize();i++)
	{
		CShankModule *pShankModule=modulePtrList[i];
		double height=0;
		for(j=0;j<pShankModule->SegmentNum();j++)
		{
			double h=pShankModule->SegmentAtIndex(j)->GetDatumLegHeight();
			if(h>0)
				return height+h;
		}
		for(j=0;j<pShankModule->LegNum();j++)
		{
			if(pShankModule->LegAtIndex(j)==pLeg)
			{
				for(int k=0;k<pShankModule->SegmentNum();k++)	//���߷����µķֶ�����
					height+=pShankModule->SegmentAtIndex(i)->m_fHeight;
				return height+pLeg->m_fHeight;
			}
		}
	}
	//����¶�������û�к��߷������ڷֶ������¼������һ�׼����
	double height=0;
	for(i=0;i<SegmentNum();i++)
	{
		CShankSegment *pSegment=SegmentAtIndex(i);
		double h=pSegment->GetDatumLegHeight();
		if(h>0)	//�ҵ���׼��
			return height+h;
		else
			height+=pSegment->m_fHeight;
	}
	return 0;
}
CShankLeg* CShankSlope::GetLowestLeg(double* pdfLowestZ/*=NULL*/)
{	//��ȡ�÷ֶ��½ӵ�����ȣ�����ݻ�����Z�����жϣ����¶��Ͽڴ���
	double dfLowestSlopeZ=0;
	CShankLeg* pLowestLeg = NULL;
	//�������¶��µĺ��߷����в��һ�׼����
	for (int i = 0; i < modulePtrList.GetSize(); i++)
	{
		CShankModule *pShankModule = modulePtrList[i];
		double dfHeightLowestZ = 0;
		CShankLeg* pLowestHeightLeg=pShankModule->GetLowestLeg(&dfHeightLowestZ);
		if (pLowestLeg == NULL || dfHeightLowestZ > dfLowestSlopeZ)
		{
			dfLowestSlopeZ = dfHeightLowestZ;
			pLowestLeg = pLowestHeightLeg;
		}
	}
	//����¶�������û�к��߷������ڷֶ������¼������һ�׼����
	double dfLegHeight = 0;
	for (int i = 0; i < SegmentNum(); i++)
	{
		CShankSegment *pSegment = SegmentAtIndex(i);
		double dfSegmentLowestZ=0;
		CShankLeg *pLowestSegLeg = pSegment->GetLowestLeg(&dfSegmentLowestZ);
		if (pLowestSegLeg!=NULL)
		{
			dfSegmentLowestZ+=dfLegHeight;
			if (pLowestLeg == NULL || dfSegmentLowestZ > dfLowestSlopeZ)
			{
				dfLowestSlopeZ = dfSegmentLowestZ;
				pLowestLeg = pLowestSegLeg;
			}
		}
		dfLegHeight += SegmentAtIndex(i)->m_fHeight;	//���ܻ���ɾ�����Ⱥ������¶�ʱ�ټ�������������� wjh-2011-11-25
	}
	if (pdfLowestZ&&pLowestLeg!=NULL)
		*pdfLowestZ = dfLowestSlopeZ;
	return pLowestLeg;
}
bool CShankSlope::GetLowestLegHeight(double* pdfLowestLegZ)
{
	int i, j;
	double dfDatumLegZ = 0, dfLegHeight = 0;
	if (pdfLowestLegZ == NULL)	//��֤��Ϊ��
		pdfLowestLegZ = &dfDatumLegZ;
	CMaxDouble maxheight;
	//�������¶��µĺ��߷����в��һ�׼����
	for (i = 0; i < modulePtrList.GetSize(); i++)
	{
		CShankModule *pShankModule = modulePtrList[i];
		dfLegHeight = 0;
		for (j = 0; j < pShankModule->SegmentNum(); j++)
		{	//���߷����µķֶ�����
			double h = pShankModule->SegmentAtIndex(j)->GetLowestLegHeight();
			if (h > 0)
				maxheight.Update(h+ dfLegHeight);
			dfLegHeight += pShankModule->SegmentAtIndex(j)->m_fHeight;	//���ܻ���ɾ�����Ⱥ������¶�ʱ�ټ�������������� wjh-2011-11-25
		}
		for (j = 0; j < pShankModule->LegNum(); j++)
			maxheight.Update(pShankModule->LegAtIndex(j)->m_fHeight + dfLegHeight);
	}
	//����¶�������û�к��߷������ڷֶ������¼������һ�׼����
	dfLegHeight = 0;
	for (i = 0; i < SegmentNum(); i++)
	{
		CShankSegment *pSegment = SegmentAtIndex(i);
		double h = SegmentAtIndex(i)->GetLowestLegHeight();
		if (h > 0)
			maxheight.Update(h + dfLegHeight);
		dfLegHeight += SegmentAtIndex(i)->m_fHeight;	//���ܻ���ɾ�����Ⱥ������¶�ʱ�ټ�������������� wjh-2011-11-25
	}
	if (maxheight.IsInited())
		*pdfLowestLegZ = maxheight.number;
	return maxheight.IsInited();
}
//�õ�ָ�����߷���
CShankModule *CShankSlope::GetSpecModule(char *sModuleName)
{
	int i,j;
	//�������¶��µĺ��߷����в��һ�׼����
	for(i=0;i<modulePtrList.GetSize();i++)
	{
		CShankModule *pShankModule=modulePtrList[i];
		if(pShankModule==NULL)
			continue;
		for(j=0;j<pShankModule->SegmentNum();j++)
		{
			CShankModule *pModule=pShankModule->SegmentAtIndex(j)->GetSpecModule(sModuleName);
			if(pModule)
				return pModule;
		}
		if(sModuleName==NULL)
		{
			for(j=0;j<pShankModule->LegNum();j++)
			{
				if(pShankModule->LegAtIndex(j)->m_fFrontWidth>0&&pShankModule->LegAtIndex(j)->m_fSideWidth>0)
					return pShankModule;	//��û�׼���߷���
			}
		}
		else if(stricmp(pShankModule->name,sModuleName)==0)
			return pShankModule;	//���ָ�����߷���
	}
	//����¶�������û�к��߷������ڷֶ������¼������һ�׼����
	for(i=0;i<SegmentNum();i++)
	{
		CShankModule *pShankModule=SegmentAtIndex(i)->GetSpecModule(sModuleName);
		if(pShankModule)
			return pShankModule;
	}
	return NULL;
}
//���ָ�����߷������ȸ߶�
double CShankSlope::GetSpecModuleLegHeight(char *sModuleName)
{
	int i,j,k;
	if(sModuleName==NULL)
		return 0;
	//�������¶��µĺ��߷����в��һ�׼����
	for(i=0;i<modulePtrList.GetSize();i++)
	{
		CShankModule *pShankModule=modulePtrList[i];
		double height=0;
		for(j=0;j<pShankModule->SegmentNum();j++)
		{
			double h=pShankModule->SegmentAtIndex(j)->GetSpecModuleLegHeight(sModuleName);
			if(h>0)
				return height+h;
		}
		if(stricmp(pShankModule->name,sModuleName)==0)
		{
			for(j=0;j<pShankModule->LegNum();j++)
				height+=pShankModule->LegAtIndex(j)->m_fHeight;
			for(k=0;k<pShankModule->SegmentNum();k++)	//���߷����µķֶ�����
				height+=pShankModule->SegmentAtIndex(k)->m_fHeight;
			return height;
		}
	}
	//����¶�������û�к��߷������ڷֶ������¼������һ�׼����
	double height=0;
	for(i=0;i<SegmentNum();i++)
	{
		CShankSegment *pSegment=SegmentAtIndex(i);
		double h=pSegment->GetSpecModuleLegHeight(sModuleName);
		if(h>0)	//�ҵ���׼��
			return height+h;
		else
			height+=pSegment->m_fHeight;
	}
	return 0;
}
void CShankSlope::CloneShankSlope(CShankSlope* pSrcSlope)
{
	m_fTopCoordZ=pSrcSlope->m_fTopCoordZ;
	m_fTopFrontWidth=pSrcSlope->m_fTopFrontWidth;
	m_fTopSideWidth=pSrcSlope->m_fTopSideWidth;
	m_cMainRodType=pSrcSlope->m_cMainRodType;
	for(int i=0;i<pSrcSlope->ModuleNum();i++)
	{
		CShankModule* pModule=AppendModule();
		pModule->CloneShankModule(pSrcSlope->ModuleAtIndex(i));
	}
	for(int i=0;i<pSrcSlope->SegmentNum();i++)
	{
		CShankSegment *pSeg=AppendSegment();
		pSeg->CloneShankSegment(pSrcSlope->SegmentAtIndex(i));
	}
}
//////////////////////////////////////////////////////////////////////
// CShankSection Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CShankSection::CShankSection()
{
	cObjType='J';
	m_pParent=NULL;
	btm_zstr.Copy("1000");	//�ײ�Z����λ���ַ���
	strcpy(front_layout,"");
	strcpy(side_layout,"");	//�ձ�ʾ�������Զ�����
	strcpy(hsect_layout,"");
	strcpy(v_layout,"");	//���沼�ġ����沼��
	divide_n=1;				//�ڼ��ڲ��ȷ��ӽڼ���
}
void CShankSection::CloneShankSection(CShankSection* pSrcSection)
{
	strcpy(btm_zstr,pSrcSection->btm_zstr);
	strcpy(front_layout,pSrcSection->front_layout);
	strcpy(side_layout,pSrcSection->side_layout);
	strcpy(hsect_layout,pSrcSection->hsect_layout);
	strcpy(v_layout,pSrcSection->v_layout);
	divide_n=pSrcSection->divide_n;
}
//////////////////////////////////////////////////////////////////////
// CShankSegment Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShankSegment::CShankSegment()
{
	cObjType='D';
	m_fHeight=0;
	m_iSeg=SEGI();
	m_pShankDef=NULL;
	m_pParent=NULL;
	strcpy(front_layout,"");
	strcpy(side_layout,"");	//�ձ�ʾ�������Զ�����
	strcpy(hsect_layout,"");
	strcpy(v_layout,"");	//���沼�ġ����沼��
	divide_n=1;				//�ڼ��ڲ��ȷ��ӽڼ���
}

CShankSegment::~CShankSegment()
{

}
IMPLEMENT_GET(CShankSegment,m_fHeight)
{
	double height=0;
	for(int i=0;i<sectionList.GetSize();i++)
	{
		double h=atof(sectionList[i].btm_zstr);
		if(h>0)
			height+=h;
		else
			height+=h*_fHeight;
	}
	if(height>0&&_fHeight<=EPS)
		return height;
	return _fHeight;
}
IMPLEMENT_SET(CShankSegment,m_fHeight){_fHeight=value;}
CShankModule* CShankSegment::AppendModule()
{
	CShankModule* pModule=m_pShankDef->Modules.append();
	pModule->m_pShankDef=m_pShankDef;
	pModule->m_pParent=this;
	modulePtrList.append(pModule);
	return pModule;
}

CShankModule* CShankSegment::ModuleAtIndex(int index)
{
	if(index<0||index>=modulePtrList.GetSize())
		return NULL;
	else
		return modulePtrList[index];
}
bool CShankSegment::DeleteModuleAtIndex(int index)
{
	return modulePtrList.RemoveAt(index);
}
CShankSection* CShankSegment::AppendSection()
{
	CShankSection section;
	section.m_pParent=this;
	CShankSection *pSection=sectionList.append(section);
	return pSection;
}

CShankSection* CShankSegment::InsertSection(int index/*=-1*/)
{
	CShankSection section;
	CShankSection *pSection=sectionList.insert(section,index);
	pSection->m_pParent=this;
	return pSection;
}

int CShankSegment::GetSectionIndex(CShankSection* pSection)
{
	return sectionList.GetIndex(pSection);
}

CShankSection* CShankSegment::SectionAtIndex(int index)
{
	if(index<0||index>=sectionList.GetSize())
		return NULL;
	else
		return sectionList.GetByIndex(index);
}
bool CShankSegment::DeleteSectionAtIndex(int index)
{
	return sectionList.RemoveAt(index);
}
CShankSegment* CShankSegment::AppendJunctionSegment()
{
	CShankSegment *pSegment=segmentList.append();
	pSegment->m_pShankDef=m_pShankDef;
	pSegment->m_pParent=this;
	return pSegment;
}
CShankSegment* CShankSegment::InsertJunctionSegment(int index/*=-1*/)
{
	CShankSegment segment;
	CShankSegment *pSegment=segmentList.insert(segment,index);
	pSegment->m_pShankDef=m_pShankDef;
	pSegment->m_pParent=this;
	return pSegment;
}
CShankSegment* CShankSegment::JunctionSegmentAtIndex(int index)
{
	if(index<0||index>=segmentList.GetSize())
		return NULL;
	else
		return &segmentList[index];
}
bool CShankSegment::DeleteSegmentAtIndex(int index)
{
	return segmentList.RemoveAt(index);
}
CShankLeg* CShankSegment::GetLowestLeg(double* pdfLowestZ/*=NULL*/)
{
	double dfLowestZ;
	CShankLeg *pLowestLeg=NULL;
	for (int i = 0; i < modulePtrList.GetSize(); i++)
	{
		CShankModule *pShankModule = modulePtrList[i];
		double dfHeightLowestZ = 0;
		CShankLeg* pLowestHeightLeg=pShankModule->GetLowestLeg(&dfHeightLowestZ);
		if (pLowestLeg == NULL || dfHeightLowestZ > dfLowestZ)
		{
			dfLowestZ = dfHeightLowestZ;
			pLowestLeg = pLowestHeightLeg;
		}
	}
	//����¶�������û�к��߷������ڷֶ������¼������һ�׼����
	double dfLegHeight = 0;
	for (int i = 0; i < segmentList.GetSize(); i++)
	{
		CShankSegment *pSegment = &segmentList[i];
		double dfSegmentLowestZ=0;
		CShankLeg *pLowestSegLeg = pSegment->GetLowestLeg(&dfSegmentLowestZ);
		if (pLowestSegLeg!=NULL)
		{
			dfSegmentLowestZ+=dfLegHeight;
			if (pLowestLeg == NULL || dfSegmentLowestZ > dfLowestZ)
			{
				dfLowestZ = dfSegmentLowestZ;
				pLowestLeg = pLowestSegLeg;
			}
		}
		dfLegHeight += pSegment->m_fHeight;	//���ܻ���ɾ�����Ⱥ������¶�ʱ�ټ�������������� wjh-2011-11-25
	}
	if (pdfLowestZ&&pLowestLeg!=NULL)
		*pdfLowestZ = dfLowestZ;
	return pLowestLeg;
}
CShankLeg* CShankSegment::GetDatumLeg()
{
	int i,j;
	CShankLeg *pLeg;
	for(i=0;i<modulePtrList.GetSize();i++)
	{
		CShankModule *pShankModule=modulePtrList[i];
		for(j=0;j<pShankModule->SegmentNum();j++)
		{
			pLeg =pShankModule->SegmentAtIndex(j)->GetDatumLeg();
			if(pLeg!=NULL)
				return pLeg;
		}
		for(j=0;j<pShankModule->LegNum();j++)
		{
			if(pShankModule->LegAtIndex(j)->m_fFrontWidth>0&&pShankModule->LegAtIndex(j)->m_fSideWidth>0)
				return pShankModule->LegAtIndex(j);
		}
	}
	return NULL;
}
double CShankSegment::GetLowestLegHeight()
{
	int i, j;
	double maxheight = 0;
	for (i = 0;i < modulePtrList.GetSize();i++)
	{
		CShankModule *pShankModule = modulePtrList[i];
		double height = m_fHeight;
		for (j = 0;j < pShankModule->SegmentNum();j++)
		{
			double h = pShankModule->SegmentAtIndex(j)->GetLowestLegHeight();
			if (h > 0)
				maxheight = max(maxheight, height + h);
		}
		for (j = 0;j < pShankModule->LegNum();j++)
		{
			double dfLegHeight = 0;
			for (int k = 0;k < pShankModule->SegmentNum();k++)	//���߷����µķֶ�����
				dfLegHeight += pShankModule->SegmentAtIndex(k)->m_fHeight;	//���ܻ���ɾ�����Ⱥ������¶�ʱ�ټ�������������� wjh-2011-11-25
			dfLegHeight += pShankModule->LegAtIndex(j)->m_fHeight;
			maxheight = max(maxheight, dfLegHeight);
		}
	}
	return maxheight;
}
double CShankSegment::GetDatumLegHeight()
{
	int i,j;
	for(i=0;i<modulePtrList.GetSize();i++)
	{
		CShankModule *pShankModule=modulePtrList[i];
		double height=m_fHeight;
		for(j=0;j<pShankModule->SegmentNum();j++)
		{
			double h=pShankModule->SegmentAtIndex(j)->GetDatumLegHeight();
			if(h>0)
				return height+h;
		}
		for(j=0;j<pShankModule->LegNum();j++)
		{
			if(pShankModule->LegAtIndex(j)->m_fFrontWidth>0&&pShankModule->LegAtIndex(j)->m_fSideWidth>0)
			{
				for(int k=0;k<pShankModule->SegmentNum();k++)	//���߷����µķֶ�����
					height+=pShankModule->SegmentAtIndex(k)->m_fHeight;	//���ܻ���ɾ�����Ⱥ������¶�ʱ�ټ�������������� wjh-2011-11-25
				return height+pShankModule->LegAtIndex(j)->m_fHeight;
			}
		}
	}
	return 0;
}
//�õ�ָ�����߷���
CShankModule *CShankSegment::GetSpecModule(char *sModuleName)
{
	int i,j;
	for(i=0;i<modulePtrList.GetSize();i++)
	{
		CShankModule *pShankModule=modulePtrList[i];
		if(pShankModule==NULL)
			continue;
		for(j=0;j<pShankModule->SegmentNum();j++)
		{
			CShankModule *pModule=pShankModule->SegmentAtIndex(j)->GetSpecModule(sModuleName);
			if(pModule)
				return pModule;
		}
		if(sModuleName==NULL)
		{
			for(j=0;j<pShankModule->LegNum();j++)
			{
				if(pShankModule->LegAtIndex(j)->m_fFrontWidth>0&&pShankModule->LegAtIndex(j)->m_fSideWidth>0)
					return pShankModule;	//��û�׼���߷���
			}
		}
		else if(stricmp(pShankModule->name,sModuleName)==0)
			return pShankModule;	//���ָ�����߷���
	}
	return NULL;
}
//���ָ�����߷������ȸ߶�
double CShankSegment::GetSpecModuleLegHeight(char *sModuleName)
{
	int i,j,k;
	if(sModuleName==NULL)
		return 0;
	for(i=0;i<modulePtrList.GetSize();i++)
	{
		CShankModule *pShankModule=modulePtrList[i];
		double height=m_fHeight;
		for(j=0;j<pShankModule->SegmentNum();j++)
		{
			double h=pShankModule->SegmentAtIndex(j)->GetSpecModuleLegHeight(sModuleName);
			if(h>0)
				return height+h;
		}
		if(stricmp(pShankModule->name,sModuleName)==0)
		{
			for(j=0;j<pShankModule->LegNum();j++)
				height+=pShankModule->LegAtIndex(j)->m_fHeight;
			for(k=0;k<pShankModule->SegmentNum();k++)	//���߷����µķֶ�����
				height+=pShankModule->SegmentAtIndex(k)->m_fHeight;
			return height;
		}
	}
	return 0;
}
//����ָ�����ȸ߶ȣ�Ҫ��pLeg����ָ��ǰ�¶��е�ĳһ���ȶ���
double CShankSegment::GetSpecLegHeight(CShankLeg *pLeg)
{
	int i,j;
	for(i=0;i<modulePtrList.GetSize();i++)
	{
		CShankModule *pShankModule=modulePtrList[i];
		double height=m_fHeight;
		for(j=0;j<pShankModule->SegmentNum();j++)
		{
			double h=pShankModule->SegmentAtIndex(j)->GetSpecLegHeight(pLeg);
			if(h>0)
				return height+h;
			else
				height+=pShankModule->SegmentAtIndex(j)->m_fHeight;
		}
		for(j=0;j<pShankModule->LegNum();j++)
		{
			if(pLeg==pShankModule->LegAtIndex(j))
			{
				height+=pLeg->m_fHeight;
				return height;
			}
		}
	}
	return 0;
}

int CShankSegment::AllocateBodyNo(int iInitLegNo)	//������Ϳɷ�����Ⱥ�,������ռ����߽��Ⱥ�
{
	for(int j=0;j<segmentList.GetSize();j++)
		iInitLegNo=segmentList[j].AllocateBodyNo(iInitLegNo);
	for(int i=0;i<modulePtrList.GetSize();i++)
		iInitLegNo=modulePtrList[i]->AllocateBodyNo(iInitLegNo);
	return iInitLegNo;
}
CFGLEG CShankSegment::AllocateBodyWord()
{
	bodyword=0;
	for(int i=0;i<modulePtrList.GetSize();i++)
		bodyword.AddBodyNo(modulePtrList[i]->m_iBodyNo);
	for(int j=0;j<segmentList.GetSize();j++)
		bodyword.AddSpecWord(segmentList[j].AllocateBodyWord());
	return bodyword;
}
bool CShankSegment::IsSegParentsHasModule()
{
	if(m_pParent->cObjType=='K')
		return false;
	else if(m_pParent->cObjType=='B')
		return true;
	else if(m_pParent->cObjType=='D')
		return m_pParent->Segment()->IsSegParentsHasModule();
	return false;
}
void CShankSegment::CloneShankSegment(CShankSegment* pSrcSegment)
{
	m_fHeight=pSrcSegment->m_fHeight;
	m_iSeg=pSrcSegment->m_iSeg;
	divide_n=pSrcSegment->divide_n;
	strcpy(front_layout,pSrcSegment->front_layout);
	strcpy(side_layout,pSrcSegment->side_layout);
	strcpy(hsect_layout,pSrcSegment->hsect_layout);
	strcpy(v_layout,pSrcSegment->v_layout);
	for(int i=0;i<pSrcSegment->JunctionSegmentNum();i++)
	{
		CShankSegment* pJuntionSeg=AppendJunctionSegment();
		pJuntionSeg->CloneShankSegment(pSrcSegment->JunctionSegmentAtIndex(i));
	}
	for(int i=0;i<pSrcSegment->ModuleNum();i++)
	{
		CShankModule* pModule=AppendModule();
		pModule->CloneShankModule(pSrcSegment->ModuleAtIndex(i));
	}
	for(int i=0;i<pSrcSegment->SectionNum();i++)
	{
		CShankSection* pSection=AppendSection();
		pSection->CloneShankSection(pSrcSegment->SectionAtIndex(i));
	}
}
//////////////////////////////////////////////////////////////////////
// CShankModule Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShankModule::CShankModule()
{
	cObjType='B';
	m_pShankDef=NULL;
	m_iBodyNo=1;						//���岿�ֶ�Ӧ���Ⱥ�
	legword.SetLegFlag(1);				//������ĺ�
	m_pParent=NULL;
	for(int i=0;i<4;i++)
		m_arrActiveQuadLegNo[i]=1;				//��ģ�͵�ǰ��һ�������޵ĵ�ǰ���Ⱥ�
}

CShankModule::~CShankModule()
{

}
bool CShankModule::IsFirstModule()
{
	if(m_pShankDef&&m_pShankDef->Modules.GetSize()>0&&m_pShankDef->Modules.GetFirst()==this)
		return true;
	else
		return false;
}
CShankSegment* CShankModule::AppendSegment()
{
	CShankSegment *pSegment=segList.append();
	pSegment->m_pShankDef=m_pShankDef;
	pSegment->m_pParent=this;
	return pSegment;
}
CShankSegment* CShankModule::InsertSegment(int index/*=-1*/)
{
	CShankSegment segment;
	CShankSegment *pSegment=segList.insert(segment,index);
	pSegment->m_pShankDef=m_pShankDef;
	pSegment->m_pParent=this;
	return pSegment;
}
bool CShankModule::DeleteSegmentAtIndex(int index)
{
	return segList.RemoveAt(index);
}

CShankLeg* CShankModule::AppendLeg()
{
	CShankLeg* pLeg=legList.append();
	pLeg->m_pParentModule=this;
	return pLeg;
}
bool CShankModule::DeleteLegAtIndex(int index)
{
	return legList.RemoveAt(index);
}

CShankSegment* CShankModule::SegmentAtIndex(int index)
{
	if(index<0||index>=segList.GetSize())
		return NULL;
	else
		return &segList[index];
}
int CShankModule::GetSegmentIndex(CShankSegment* pSeg)
{
	return segList.GetIndex(pSeg);
}
CShankLeg* CShankModule::LegAtIndex(int index)
{
	return &legList[index];
}
int CShankModule::AllocateBodyNo(int iInitLegNo)	//������Ϳɷ�����Ⱥ�,������ռ����߽��Ⱥ�
{
	m_iBodyNo=iInitLegNo;
	int nMaxLegs=CFGLEG::MaxLegs();
	m_arrActiveQuadLegNo[0]=m_arrActiveQuadLegNo[1]=m_arrActiveQuadLegNo[2]=m_arrActiveQuadLegNo[3]=(iInitLegNo-1)*nMaxLegs+1;
	for(int i=SegmentNum()-1;i>=0;i--)
		SegmentAtIndex(i)->bodyword.SetBodyFlag(GetSingleWord(m_iBodyNo));
	iInitLegNo++;
	return iInitLegNo;
}
bool CShankModule::AllocateLegWord()		//������Ϳɷ�����Ⱥ�,������ռ����߽��Ⱥ�
{
	legword=CFGLEG((BYTE)m_iBodyNo,1);
	int nMaxLegs=CFGLEG::MaxLegs();
	if(legList.GetNodeNum()>CFGLEG::MaxLegs())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox(CXhChar100("Joint leg total number(%d) of current module %s is more than the joint leg distribution mode limited number %d of current module.",legList.GetNodeNum(),(char*)name,nMaxLegs));
#else
		AfxMessageBox(CXhChar100("��ǰ����%s��������(%d��)������ǰ���߽��ȷ���ģʽ�޶�%d��",(char*)name,legList.GetNodeNum(),nMaxLegs));
#endif
		return false;
	}
	for(char i=0;i<legList.GetNodeNum()&&i<nMaxLegs;i++)
	{
		LegAtIndex(i)->legword=CFGLEG((BYTE)m_iBodyNo,i+1);
		legword.AddLegNo(i+1);
	}
	return true;
}
void CShankModule::CloneShankModule(CShankModule* pSrcModule)
{
	strcpy(name,pSrcModule->name);
	for(int i=0;i<pSrcModule->SegmentNum();i++)
	{
		CShankSegment* pSeg=AppendSegment();
		pSeg->CloneShankSegment(pSrcModule->SegmentAtIndex(i));
	}
	for(int i=0;i<pSrcModule->LegNum();i++)
	{
		CShankLeg* pLeg=AppendLeg();
		pLeg->CloneShankLeg(pSrcModule->LegAtIndex(i));
	}
}
CShankLeg* CShankModule::GetLowestLeg(double* pdfLowestZ/*=NULL*/)
{
	double dfLowestZ=0,dfLegHeight = 0;
	CShankLeg *pLowestLeg=NULL;
	for (int j = 0; j < SegmentNum(); j++)
	{	//���߷����µķֶ�����
		double dfSegLowestZ;
		CShankSegment* pSegment = SegmentAtIndex(j);
		CShankLeg* pLeg= pSegment->GetLowestLeg(&dfSegLowestZ);
		if (pLeg != NULL)
		{
			dfSegLowestZ += dfLegHeight;
			if (pLowestLeg == NULL || dfSegLowestZ > dfLowestZ)
			{
				pLowestLeg = pLeg;
				dfLowestZ = dfSegLowestZ;
			}
		}
		dfLegHeight += pSegment->m_fHeight;	//���ܻ���ɾ�����Ⱥ������¶�ʱ�ټ�������������� wjh-2011-11-25
	}
	for (j = 0; j < LegNum(); j++)
	{
		CShankLeg* pLeg = LegAtIndex(j);
		if (pLowestLeg == NULL || dfLegHeight+pLeg->m_fHeight > dfLowestZ)
		{
			pLowestLeg = pLeg;
			dfLowestZ = dfLegHeight+pLeg->m_fHeight;
		}
	}
	if (pdfLowestZ!=NULL&&pLowestLeg!=NULL)
		*pdfLowestZ = dfLowestZ;
	return pLowestLeg;
}
//////////////////////////////////////////////////////////////////////
// CShankLeg Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShankLeg::CShankLeg()
{
	cObjType='L';
	m_bDatumLeg=FALSE;
	m_fFrontWidth=m_fSideWidth=m_fHeight=0;
	strcpy(front_layout,"");
	strcpy(side_layout,"");	//�ձ�ʾ�������Զ�����
	strcpy(hsect_layout,"");
	strcpy(v_layout,"");	//���沼�ġ����沼��
	m_iSeg=SEGI();
	m_pParentModule=NULL;
	divide_n=5;
}

CShankLeg::~CShankLeg()
{

}
CShankSection* CShankLeg::AppendSection()
{
	CShankSection section;
	section.m_pParent=this;
	CShankSection *pSection=sectionList.append(section);
	return pSection;
}
CShankSection* CShankLeg::InsertSection(int index/*=-1*/)
{
	CShankSection section;
	CShankSection *pSection=sectionList.insert(section,index);
	pSection->m_pParent=this;
	return pSection;
}

int CShankLeg::GetSectionIndex(CShankSection* pSection)
{
	return sectionList.GetIndex(pSection);
}
CShankSection* CShankLeg::SectionAtIndex(int index)
{
	if(index<0||index>=sectionList.GetSize())
		return NULL;
	else
		return &sectionList[index];
}
bool CShankLeg::DeleteSectionAtIndex(int index)
{
	return sectionList.RemoveAt(index);
}
void CShankLeg::CloneShankLeg(CShankLeg* pSrcLeg)
{
	m_iSeg=pSrcLeg->m_iSeg;
	m_fHeight=pSrcLeg->m_fHeight;
	m_fFrontWidth=pSrcLeg->m_fFrontWidth;
	m_fSideWidth=pSrcLeg->m_fSideWidth;
	divide_n=pSrcLeg->divide_n;
	strcpy(front_layout,pSrcLeg->front_layout);
	strcpy(side_layout,pSrcLeg->side_layout);
	strcpy(hsect_layout,pSrcLeg->hsect_layout);
	strcpy(v_layout,pSrcLeg->v_layout);
	for(int i=0;i<pSrcLeg->SectionNum();i++)
	{
		CShankSection* pSection=AppendSection();
		pSection->CloneShankSection(pSrcLeg->SectionAtIndex(i));
	}
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CShankDefine::DEFINE_MODE::DEFINE_MODE()
{
	blAppendMode = false;
	ciInitHeightSerial = 1;
	dfDefaultHeightDiff = 16000;
	dfGradientDetaX2H = dfGradientDetaY2H = 0.035;
	memset(xarrTopNodesPtr, 0, sizeof(CLDSNodePtr) * 4);
	memset(xarrBtmNodesPtr, 0, sizeof(CLDSNodePtr) * 4);		//�¶���ʼ�ڵ��µĽڵ�����
	memset(xarrBtmLegRodsPtr, 0, sizeof(CLDSLinePartPtr) * 4);	//�¶���ʼ�ڵ��µĸ˼�����
}
double CShankDefine::DEFINE_MODE::get_dfTopCoordZ()
{
	if (xarrTopNodesPtr[0] != NULL)
		return xarrTopNodesPtr[0]->xOriginalPos.z;
	else
		return 0;
}
double CShankDefine::DEFINE_MODE::get_dfTopFrontWidth()
{
	if (xarrTopNodesPtr[0] != NULL)
		return xarrTopNodesPtr[0]->xOriginalPos.x*2;
	else
		return 0;
}
double CShankDefine::DEFINE_MODE::get_dfTopSideWidth()
{
	if (xarrTopNodesPtr[0] != NULL)
		return xarrTopNodesPtr[0]->xOriginalPos.y * 2;
	else
		return 0;
}
CShankDefine::CShankDefine()
{
	
}

CShankDefine::~CShankDefine()
{
	
}
CShankSlope *CShankDefine::AppendSlope()
{
	CShankSlope *pSlope=Slopes.append();
	pSlope->m_pShankDef=this;
	return pSlope;
}

CShankSlope *CShankDefine::SlopeAtIndex(int index)
{
	if(index<0||index>=Slopes.GetSize())
		return NULL;
	return &Slopes[index];
}
bool CShankDefine::DeleteSlopeAtIndex(int index)
{
	return Slopes.RemoveAt(index);
}
CShankModule *CShankDefine::ModuleAtIndex(int index)
{
	if(index<0||index>=Modules.GetSize())
		return NULL;
	else
		return &Modules[index];
}
bool CShankDefine::DeleteModuleAtIndex(int index)
{
	return Modules.RemoveAt(index);
}
//ÿ�������е������������������ж����ʵĺ���-����ģʽ wjh-2019.8.1
int CShankDefine::MaxLegsPerHeightGroup()
{
	int maxlegs = 0;
	for (int i = 0; i < Modules.GetSize(); i++)
	{
		CShankModule* pHeightGroup = &Modules[i];
		maxlegs=max(maxlegs,pHeightGroup->LegNum());
	}
	return maxlegs;
}
bool CShankDefine::FromString(char* strDef)
{
	return false;
}

double CShankDefine::SlopeHeight(int iSlope,CShankLeg *pLeg/*=NULL*/)
{
	if(iSlope>=0&&iSlope<Slopes.GetSize()-1)
		return SlopeAtIndex(iSlope)->SlopeBodyHeight();
	else if(iSlope<0||iSlope>=Slopes.GetSize())
		return 0;
	else
	{
		double height=0;
		/*for(int i=0;i<SlopeAtIndex(iSlope)->SegmentNum();i++)
		{
			CShankSegment *pSegment=SlopeAtIndex(iSlope)->SegmentAtIndex(i);
			double h=0;
			if(sModuleName==NULL)	//����Ͽ�����׼�����ŵĸ߶�
				h=pSegment->GetDatumLegHeight();
			else					//����Ͽ�����ָ�����߷������ŵĸ߶� wht 10-06-08
				h=pSegment->GetSpecModuleLegHeight(sModuleName);
			if(h>0)	//�ҵ���׼��
				return height+h;
			else
				height+=pSegment->m_fHeight;
		}*/
		if(pLeg)
			return height=SlopeAtIndex(iSlope)->GetSpecLegHeight(pLeg);
		else
		{
			if (this->mode.blAppendMode&& SlopeAtIndex(iSlope)->GetLowestLegHeight(&height))
				return height;
			else if (!SlopeAtIndex(iSlope)->GetDatumLegHeight(&height))
				height = mode.dfDefaultHeightDiff;
			return height;
		}
	}
}
double CShankDefine::SlopeTopFrontWidth(int iSlope,CShankLeg *pLeg/*=NULL*/)
{
	if(iSlope>=0&&iSlope<Slopes.GetSize())
		return SlopeAtIndex(iSlope)->m_fTopFrontWidth;
	else if(iSlope==Slopes.GetSize())	//��ĩβ�¶ȶεײ����,
	{
		CShankSlope *pSlope=SlopeAtIndex(iSlope-1);
		if(pSlope==NULL)
			return 0;
		if(pLeg==NULL)
		{	//��׼����������
			double dfWidth=0;
			if (pSlope->GetDatumLegFrontWidth(&dfWidth))
				return dfWidth;
			else
			{
				double dfSlopeHeight = mode.dfDefaultHeightDiff;
				pSlope->GetLowestLegHeight(&dfSlopeHeight);
				return pSlope->m_fTopFrontWidth + mode.dfGradientDetaX2H*dfSlopeHeight * 2;
			}
		}
		else
		{	//ָ������������
			double fSlopeTopWidth=SlopeTopFrontWidth(iSlope-1);
			double fSlopeBottomWidth=SlopeBottomFrontWidth(iSlope-1);
			double fSlopeHeight=SlopeHeight(iSlope-1);
			double fSpecSlopeHeight=SlopeHeight(iSlope-1,pLeg);
			if(fSlopeHeight>0)
			{
				double fScale=0.5*(fSlopeBottomWidth-fSlopeTopWidth)/fSlopeHeight;
				return fSlopeTopWidth+fScale*fSpecSlopeHeight*2;
			}
		}
	}
	return 0;
}
double CShankDefine::SlopeBottomFrontWidth(UINT iSlope,CShankLeg *pLeg/*=NULL*/)
{
	return SlopeTopFrontWidth(iSlope+1,pLeg);
}
double CShankDefine::SlopeTopSideWidth(int iSlope,CShankLeg *pLeg/*=NULL*/)
{
	if(iSlope>=0&&iSlope<Slopes.GetSize())
		return SlopeAtIndex(iSlope)->m_fTopSideWidth;
	else if(iSlope==Slopes.GetSize())	//��ĩβ�¶ȶεײ����,
	{
		CShankSlope *pSlope=SlopeAtIndex(iSlope-1);
		if(pSlope==NULL)
			return 0;
		if(pLeg==NULL)
		{
			double dfWidth = 0;
			if (pSlope->GetDatumLegSideWidth(&dfWidth))
				return dfWidth;
			else
			{
				double dfSlopeHeight = mode.dfDefaultHeightDiff;
				pSlope->GetLowestLegHeight(&dfSlopeHeight);
				return pSlope->m_fTopSideWidth + mode.dfGradientDetaY2H*dfSlopeHeight * 2;
			}
		}
		else
		{	//ָ�����Ȳ�����
			double fSlopeTopWidth=SlopeTopSideWidth(iSlope-1);
			double fSlopeBottomWidth=SlopeBottomSideWidth(iSlope-1);
			double fSlopeHeight=SlopeHeight(iSlope-1);
			double fSpecSlopeHeight=SlopeHeight(iSlope-1,pLeg);
			if(fSlopeHeight>0)
			{
				double fScale=0.5*(fSlopeBottomWidth-fSlopeTopWidth)/fSlopeHeight;
				return fSlopeTopWidth+fScale*fSpecSlopeHeight*2;
			}
		}

	}
	return 0;
}
double CShankDefine::SlopeBottomSideWidth(UINT iSlope,CShankLeg *pLeg/*=NULL*/)
{
	return SlopeTopSideWidth(iSlope+1,pLeg);
}
double CShankDefine::SlopeTopCoordZ(int iSlope)
{
	if(iSlope>0&&iSlope<Slopes.GetSize())
	{
		double coordZ=SlopeAtIndex(0)->m_fTopCoordZ;
		for(int i=0;i<iSlope;i++)
			coordZ+=SlopeAtIndex(i)->SlopeBodyHeight();
		return coordZ;
	}
	else if(iSlope==0)
		return SlopeAtIndex(0)->m_fTopCoordZ;
	else if(iSlope==Slopes.GetSize())
		return SlopeBottomCoordZ(iSlope-1);
	else
		return 0;
}
double CShankDefine::SlopeBottomCoordZ(int iSlope,CShankLeg *pLeg/*=NULL*/)
{
	return SlopeTopCoordZ(iSlope)+SlopeHeight(iSlope,pLeg);
}
CFGWORD CShankDefine::SlopeTopCfgword(int iSlope)
{
	if(iSlope<Slopes.GetSize())
	{
		CFGLEG bodyword;
		for(int i=0;i<SlopeAtIndex(iSlope)->ModuleNum();i++)
			bodyword.AddBodyNo(SlopeAtIndex(iSlope)->ModuleAtIndex(i)->m_iBodyNo);
		if(SlopeAtIndex(iSlope)->SegmentNum()>0)
			bodyword.AddSpecWord(SlopeAtIndex(iSlope)->SegmentAtIndex(0)->bodyword);
		return bodyword.ToCfgWord();
	}
	else if(iSlope==Slopes.GetSize())
	{
		CShankLeg *pLeg;
		CShankSlope *pSlope=SlopeAtIndex(iSlope-1);
		for(int i=0;i<pSlope->SegmentNum();i++)
		{
			if((pLeg=pSlope->SegmentAtIndex(i)->GetDatumLeg())!=NULL)
				return pLeg->legword.ToCfgWord();
		}
		if((pLeg=pSlope->GetLowestLeg())!=NULL)
			return pLeg->legword.ToCfgWord();
	}
	return CFGWORD(1);
}
//����¶ζ����ڵ�κ�
SEGI CShankDefine::SlopeTopSegI(int iSlope)
{
	if(iSlope<Slopes.GetSize())
	{
		if(SlopeAtIndex(iSlope)->SegmentNum()>0)
			return SlopeAtIndex(iSlope)->SegmentAtIndex(0)->m_iSeg;
	}
	else if(iSlope==Slopes.GetSize())
	{	
		for(int i=0;i<SlopeAtIndex(iSlope-1)->SegmentNum();i++)
		{
			CShankLeg *pLeg=SlopeAtIndex(iSlope-1)->SegmentAtIndex(i)->GetDatumLeg();
			if(pLeg)
				return pLeg->m_iSeg;
		}
	}
	return SEGI();
}
bool CShankDefine::AllocateCfgWord(BYTE ciInitHeightNumber/* = 1*/)
{
	int i,j,k;
	int iInitLegNo= ciInitHeightNumber;
	CShankSlope *pBottomSlope=Slopes.GetTail();
	if(pBottomSlope==NULL)
		return false;
	//1.����Ϊֱ���ڵײ��¶εĺ��߷��������ĺ�	wht 10-07-24
	if(pBottomSlope->ModuleNum()>CFGLEG::MaxBodys())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox(CXhChar50("Module total number (%d) is more than the joint leg distribution mode limited number %d of current module.",pBottomSlope->ModuleNum(),CFGLEG::MaxBodys()));
#else
		AfxMessageBox(CXhChar50("��������(%d��)������ǰ���߽��ȷ���ģʽ�޶�%d��",pBottomSlope->ModuleNum(),CFGLEG::MaxBodys()));
#endif
		return false;
	}
	for(i=0;i<pBottomSlope->ModuleNum();i++)
		iInitLegNo=pBottomSlope->ModuleAtIndex(i)->AllocateBodyNo(iInitLegNo);
	//2.Ȼ��Ϊ������ڵײ��¶εĺ��߷��������ĺ�
	for(i=0;i<pBottomSlope->SegmentNum();i++)
		iInitLegNo=pBottomSlope->SegmentAtIndex(i)->AllocateBodyNo(iInitLegNo);
	CFGLEG bodyword(0);
	for(CShankSlope *pSlope=Slopes.GetTail();pSlope;pSlope=Slopes.GetPrev())
	{
		for(j=pSlope->SegmentNum()-1;j>=0;j--)
		{
			//����ǰ��֮�µķֶε�������Ű�������
			bodyword.AddSpecWord(pSlope->SegmentAtIndex(j)->AllocateBodyWord());
			pSlope->SegmentAtIndex(j)->bodyword=bodyword;
			//��Ҫ�ӵ�ǰ�¶�������ֶε���ĺ��н��¶�ֱ�����߷��鱾����޳�����ʵ�о�����Ҫ���´��룬����Ҫ���Ժ���ɾ���� wjh-2013.5.17
			//for(k=0;k<pSlope->ModuleNum();k++)
			//	pSlope->SegmentAtIndex(j)->bodyword.RemoveSpecNoWord(pSlope->ModuleAtIndex(k)->m_iBodyNo,false);
		}
		//���ͨ�����������¶ν�һ�����߽��ȶ�����������Ӧ��ĺ� wjh-2016.3.2
		for(k=0;k<pSlope->ModuleNum();k++)
			bodyword.AddBodyNo(pSlope->ModuleAtIndex(k)->m_iBodyNo);
	}
	//�Զ������Ȳ����Ⱥ�
	for(j=0;j<ModuleNum();j++)
	{
		if(!ModuleAtIndex(j)->AllocateLegWord())
			return false;
	}
	return true;
}
int CShankDefine::ValidCheck()
{
	if(Slopes.GetSize()<=0)
		return -1;
	return 0;
}
void CShankDefine::CloneShankDef(CShankDefine* pSrcShankDef)
{
	Empty();
	for(int i=0;i<pSrcShankDef->SlopeNum();i++)
	{
		CShankSlope *pSlope=AppendSlope();
		pSlope->CloneShankSlope(pSrcShankDef->SlopeAtIndex(i));
	}
}
//#endif