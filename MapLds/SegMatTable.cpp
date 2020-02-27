//<LOCALE_TRANSLATE/>
/*
˵�� ---- �����ʽ�����°���:
                  1.��λԭ��λ�����Ͻ�����������
                  2.��λԭ��λ�����½�����������
                  3.��λԭ��λ�����Ͻ�����������
                  4.��λԭ��λ�����½�����������
                  5.��λԭ��λ�����Ͻ��ҷֶκ�������
                  6.��λԭ��λ�����½��ҷֶκ�������
                  7.��λԭ��λ�����Ͻ��ҷֶκ�������
                  8.��λԭ��λ�����½��ҷֶκ�������
*/
#include "stdafx.h"
#include "stdarx.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "MxbDlg.h"
#include "MenuFunc.h"
#include "RxTools.h"
#include "AdsTools.h"
#include "GlobalFunc.h"
#include "..\Query.h"
#include "aced.h"
#include "adslib.h"
#include "SegMatSum.h"
#include "FormatTable.h"
#include "syspara.h"
#include "SelPartGroupDlg.h"
#include "PartStatRecord.h"
#include "SortFunc.h"
//#ifdef _MAPTMA_GRX
//#include "IcExtensionModule.h"
//#endif
#include "DrawUnit.h"

#ifndef __TSA_FILE_
//������ϸ��
//[in]iSegNo>�����κ�
//Ԥ��׼���ֶι���������
BOOL PrepareSegPartTable(CFormatTbl &xPartTbl,SEGI nSegI=-1)
{
#ifdef AFX_TARG_ENU_ENGLISH
	bool EXPORT_BY_ENGLISH=true;
#else
	bool EXPORT_BY_ENGLISH=(sys.stru_map.bExportByEnglish!=FALSE);
#endif
	CSegMaterialSummator::EXPORT_BY_ENGLISH=EXPORT_BY_ENGLISH;

	CSegMaterialSummator xSegMatSum;
	//attach_rows���������õĿ�������
	int i,j,nRecordN,attach_rows=2;
	if(nSegI.iSeg==-1)
		nSegI=Ta.m_iCurSeg;
	if(bom.Parts.GetNodeNum()>0)
		xSegMatSum.RunSummarizeFromBOM(bom,nSegI,true,false);
	else if(!xSegMatSum.RunSummarize(nSegI))
		return FALSE;
	nRecordN = xSegMatSum.GetJgPlankRecCount();
	int nRowsN = nRecordN+4+attach_rows;	//��������
	const int nColumnsN = 7;	//��������
	CGrid::PREFER_TEXT_SIZE=sys.dim_map.fNumericTextSize;	//��ʼ�����Ԫ����߶�
	xPartTbl.Create(nRowsN,nColumnsN);
	for(i=0;i<nColumnsN;i++)	//����Ϊ������
		xPartTbl.GetGridAt(0,i)->textsize = sys.dim_map.fHeaderTextSize;
	for(i=0;i<nColumnsN;i++)	//����Ϊ������
	{
		xPartTbl.GetGridAt(1,i)->textsize = sys.dim_map.fCommonTextSize;
		xPartTbl.GetGridAt(2,i)->textsize = sys.dim_map.fCommonTextSize;
	}

	//���ɴ����-------���ϻ��ܱ�-----------
	xPartTbl.SetRowHigh(0,15);		//������
	xPartTbl.SetRowHigh(1,8);		//�ֶ���֮һ
	xPartTbl.SetRowHigh(2,8);		//�ֶ���֮��
	xPartTbl.SetRowHigh(nRowsN-1,10);//�ϼ���
	for(i=3;i<nRowsN-1;i++)			//������
		xPartTbl.SetRowHigh(i,6);
	if(sys.general_set.iPartBOMStyle==0)
	{	//��׼��ʽ ��Ҫ����TMA
		xPartTbl.SetColumnWide(0,10);	//�����
		xPartTbl.SetColumnWide(1,25);	//�����
		xPartTbl.SetColumnWide(2,15);	//������
		xPartTbl.SetColumnWide(3,10);	//������
		xPartTbl.SetColumnWide(4,15);	//����֮һ����
		xPartTbl.SetColumnWide(5,15);	//����֮С����
		xPartTbl.SetColumnWide(6,30);	//��ע��
	}
	else
	{	//�ӿ���ʽ
		xPartTbl.SetColumnWide(0,19);	//�����//ԭΪ13��Ϊ��׼�����ı�űȽϳ��ʵ���Ϊ19 ͬʱ������˨ͳ�Ʊ��� 
		xPartTbl.SetColumnWide(1,27);	//�����//ԭΪ25������--Q345��Щ���Ų���,�ʷŴ�Ϊ27 WJH--01.11.01
		xPartTbl.SetColumnWide(2,23);	//������//ԭΪ15��Ϊ��ע��������ʱ���עһ����Բֱ��~��Բֱ���ķ�Χ ���޸�Ϊ23
		xPartTbl.SetColumnWide(3,10);	//������
		xPartTbl.SetColumnWide(4,15);	//����֮һ����
		xPartTbl.SetColumnWide(5,15);	//����֮С����
		xPartTbl.SetColumnWide(6,30);	//��ע��
	}
	//����е�����ȱʡ�����½Ƕ��뼴feature = 0
	//feature��λ����ʾ�����ֵĶ��뷽ʽ0:������ 1��ʾ������ 2��ʾ���½�
	//ʮλ��ʾ������д����0:������1:���ϵ���

	//���������
	for(j=1;j<nColumnsN;j++)//ˮƽ�ϲ���һ���е����б��Ԫ
		xPartTbl.GetGridAt(0,j)->m_bHGridVirtual = TRUE;
	if(EXPORT_BY_ENGLISH)
		xPartTbl.GetGridAt(0,0)->data.SetGridString("BILL OF MATERIAL");
	else
		xPartTbl.GetGridAt(0,0)->data.SetGridString("�� �� �� ϸ ��");

	xPartTbl.GetGridAt(0,0)->feature = 1;	//����Գ�MC
	//����--��ű��Ԫ
	xPartTbl.GetGridAt(2,0)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
	if(EXPORT_BY_ENGLISH)
		xPartTbl.GetGridAt(1,0)->data.SetGridString("Mark\\PNo.");
	else
		xPartTbl.GetGridAt(1,0)->data.SetGridString("��  ��");

	xPartTbl.GetGridAt(1,0)->feature = 1;	//����Գ�MC
	//����--�����Ԫ
	xPartTbl.GetGridAt(2,1)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
	xPartTbl.GetGridAt(1,1)->feature = 1;
	if(EXPORT_BY_ENGLISH)
		xPartTbl.GetGridAt(1,1)->data.SetGridString("Specification");
	else
		xPartTbl.GetGridAt(1,1)->data.SetGridString("��    ��");
	//����--���ȱ��Ԫ
	xPartTbl.GetGridAt(2,2)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
	xPartTbl.GetGridAt(1,2)->feature = 1;
	if(EXPORT_BY_ENGLISH)
		xPartTbl.GetGridAt(1,2)->data.SetGridString("Length\\P(mm)");
	else
		xPartTbl.GetGridAt(1,2)->data.SetGridString("��  ��\\P(mm)");
	//����--�������Ԫ
	xPartTbl.GetGridAt(2,3)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
	xPartTbl.GetGridAt(1,3)->feature = 1;
	if(EXPORT_BY_ENGLISH)
		xPartTbl.GetGridAt(1,3)->data.SetGridString("Qty");
	else
		xPartTbl.GetGridAt(1,3)->data.SetGridString("��\\P��");
	//����--�������Ԫ
	xPartTbl.GetGridAt(1,5)->m_bHGridVirtual = TRUE;//ˮƽ�ϲ���Ԫ��
	xPartTbl.GetGridAt(1,4)->feature = 1;
	if(EXPORT_BY_ENGLISH)
	{
		xPartTbl.GetGridAt(1,4)->data.SetGridString("Weight(kg)");
			//weight of a piece table unit
		xPartTbl.GetGridAt(2,4)->feature = 1;
		xPartTbl.GetGridAt(2,4)->data.SetGridString("Piece");
			//subtotal weight table unit
		xPartTbl.GetGridAt(2,5)->feature = 1;
		xPartTbl.GetGridAt(2,5)->data.SetGridString("Total");
	}
	else
	{
		xPartTbl.GetGridAt(1,4)->data.SetGridString("��    ��(kg)");
			//����֮һ�����Ԫ
		xPartTbl.GetGridAt(2,4)->feature = 1;
		xPartTbl.GetGridAt(2,4)->data.SetGridString("һ ��");
			//����֮С�Ʊ��Ԫ
		xPartTbl.GetGridAt(2,5)->feature = 1;
		xPartTbl.GetGridAt(2,5)->data.SetGridString("С ��");
	}
	//����--��ע���Ԫ
	xPartTbl.GetGridAt(2,6)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
	xPartTbl.GetGridAt(1,6)->feature = 1;
	if(EXPORT_BY_ENGLISH)
		xPartTbl.GetGridAt(1,6)->data.SetGridString( "Notes");
	else
		xPartTbl.GetGridAt(1,6)->data.SetGridString( "��    ע");
	//����--������
	char tem_str[MAX_PATH];
	int nInvalidNum=0;		//��¼���������ݸ��� wht 11-08-03
	double sum_weight=0;
	for(i=3;i<nRowsN-1-attach_rows;i++)
	{
		CSegMatPolePlankRecord *pRec = xSegMatSum.GetJgPlankRecAt(i-3);
		if (strlen(pRec->GetPartNo())<=0||pRec->GetPartN()<=0)
		{	//�������ΪNULL�򹹼���Ϊ0
			nInvalidNum++;
			continue;	
		}
		//�����
		strcpy(tem_str,pRec->GetPartNo(sys.general_set.iMatCharPosType));
		xPartTbl.GetGridAt(i-nInvalidNum,0)->data.SetGridString(tem_str);
		xPartTbl.GetGridAt(i-nInvalidNum,0)->feature=TEXTALIGN_LEFT_CENTER;	//�����ж����޸�Ϊ���ж���,Ϊ��ϲ���Ĺ����Ŷ��� wht 12-03-22
		if(pRec->bStdPart)
		{	//��ǰ��¼Ϊ��׼��,�ϲ�������������,����ע����볤�� wht 12-03-22
			xPartTbl.GetGridAt(i-nInvalidNum,1)->m_bHGridVirtual=TRUE;
		}
		else 
		{
			//�����
			xPartTbl.GetGridAt(i-nInvalidNum,1)->data.SetGridString(pRec->guige);
			xPartTbl.GetGridAt(i-nInvalidNum,1)->feature=TEXTALIGN_LEFT_CENTER;	//���������
			//������
			//sprintf(tem_str,"%.0f",pRec->GetLength());
			xPartTbl.GetGridAt(i-nInvalidNum,2)->data.SetGridString(pRec->sLength);
			xPartTbl.GetGridAt(i-nInvalidNum,2)->feature=TEXTALIGN_RIGHT_CENTER;
		}
		//������
		sprintf(tem_str,"%d",pRec->GetPartN());
		xPartTbl.GetGridAt(i-nInvalidNum,3)->data.SetGridString(tem_str);
		xPartTbl.GetGridAt(i-nInvalidNum,3)->feature=TEXTALIGN_MIDDLE_CENTER;	//���������
		//����֮һ����
		sprintf(tem_str,"%.2f",pRec->GetUnitWeight());
		xPartTbl.GetGridAt(i-nInvalidNum,4)->data.SetGridString(tem_str);
		xPartTbl.GetGridAt(i-nInvalidNum,4)->feature=TEXTALIGN_RIGHT_CENTER;
		//����֮С����
		sprintf(tem_str,"%.1f",pRec->GetSumWeight());
		xPartTbl.GetGridAt(i-nInvalidNum,5)->data.SetGridString(tem_str);
		xPartTbl.GetGridAt(i-nInvalidNum,5)->feature=TEXTALIGN_RIGHT_CENTER;
		//��ע��
		xPartTbl.GetGridAt(i-nInvalidNum,6)->data.SetGridString(pRec->sNotes);
		xPartTbl.GetGridAt(i-nInvalidNum,6)->feature=TEXTALIGN_LEFT_CENTER;	//���������
		//����
		sum_weight+=TruncateDouble(pRec->GetSumWeight(),1);
	}
	//����ϼ���
	xPartTbl.GetGridAt(nRowsN-1,1)->m_bHGridVirtual = TRUE;//ˮƽ�ϲ���Ԫ��
	if(EXPORT_BY_ENGLISH)
		xPartTbl.GetGridAt(nRowsN-1,0)->data.SetGridString("Total");
	else
		xPartTbl.GetGridAt(nRowsN-1,0)->data.SetGridString("��      ��");
	xPartTbl.GetGridAt(nRowsN-1,0)->feature = 1; //�����������
	for(j=3;j<nColumnsN;j++)
		xPartTbl.GetGridAt(nRowsN-1,j)->m_bHGridVirtual = TRUE;//ˮƽ�ϲ���Ԫ��
	//
	sprintf(tem_str,"%.1fkg",sum_weight);
	xPartTbl.GetGridAt(nRowsN-1,2)->data.SetGridString(tem_str);
	xPartTbl.GetGridAt(nRowsN-1,2)->feature = 1;	//�����������
	return TRUE;
}

void SegPartBOM()
{
	sys.m_iCurMapType = 3;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	CMxbDlg dlg;
	if(dlg.DoModal()!=IDOK)
		return;
	int nLocatePos=dlg.m_nLocatePos;
	for(SEGI* pSegI=dlg.m_xSegNoList.GetFirst();pSegI;pSegI=dlg.m_xSegNoList.GetNext())
	{
		CFormatTbl xPartTbl;
		PrepareSegPartTable(xPartTbl,*pSegI);
		AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
		DrawTable(pBlockTableRecord,&xPartTbl,TRUE,1,nLocatePos);
		pBlockTableRecord->close();
	}
}
//��Ű���������
int comparePartStatRecord(const CPartStatRecord &rec1,const CPartStatRecord &rec2)
{
	if(strlen(rec1.sPartNo)>strlen(rec2.sPartNo))
		return 1;
	else if(strlen(rec1.sPartNo)<strlen(rec2.sPartNo))
		return -1;
	else
		return stricmp(rec1.sPartNo,rec2.sPartNo);
}
UINT StatSameLabelPartCount(CLDSPart* pLabelPart, DBOBJSET* pFindedObjSet = NULL)
{
	if (strlen(pLabelPart->Label) <= 0)
		return 0;
	UINT uiStatCount=0;
	for (CLDSPart* pPart = Ta.Parts.GetFirst(); pPart; pPart = Ta.Parts.GetNext())
	{
		if (pPart->m_bVirtualPart|| !pLabelPart->ModuleTogetherWith(pLabelPart))
			continue;
		if (stricmp(pPart->Label, pLabelPart->Label) != 0)
			continue;
		if (pFindedObjSet)
			pFindedObjSet->append(pPart);
		uiStatCount += pPart->m_uStatMatCoef;
	}
	return uiStatCount;
}
//��Ϲ�����ϸ��
void PartGroupBOM()
{
	sys.m_iCurMapType = 4;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
    CAcModuleResourceOverride resOverride;
	CSelPartGroupDlg selPartGroupDlg;
	selPartGroupDlg.m_bPartGroupBOM=TRUE;
	if(selPartGroupDlg.DoModal()!=IDOK)
		return;
	//attach_rows���������õĿ�������
	int i,j,nRecordN,attach_rows=0;
	PARTSET partset;
	CLDSPartGroup *pPartGroup=NULL;
	for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
	{
		if(pPartGroup->handle==selPartGroupDlg.m_hSelPartGroup)
			break;
	}
	if(pPartGroup==NULL)
		return;
	Ta.GetPartGroupPartSet(selPartGroupDlg.m_hSelPartGroup,partset);
	CPartStatRecord *pRec=NULL;
	ATOM_LIST<CPartStatRecord>matStat;
	CLDSPart *pRootPart=Ta.FromPartHandle(pPartGroup->handle);
	if(pRootPart)
		partset.insert(pRootPart,0);
	for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		if(strlen(pPart->GetPartNo())==0)
			continue;	//�ޱ�Ź�����ȥ��ͳ��
		else if(pPart->m_bVirtualPart)
			continue;	//���⸨������
		for(pRec=matStat.GetFirst();pRec;pRec=matStat.GetNext())
		{
			if(stricmp(pRec->sPartNo,GetPartNoIncMat(pPart->GetPartNo(),pPart->cMaterial))==0)
			{
				pRec->num+=pPart->m_uStatMatCoef;
				break;
			}
		}
		if(pRec==NULL)
		{
			pRec=matStat.append();
			strcpy(pRec->sPartNo,GetPartNoIncMat(pPart->GetPartNo(),pPart->cMaterial));
			pRec->num+=pPart->m_uStatMatCoef;
			pRec->numPerTower = StatSameLabelPartCount(pPart);
			pRec->idClassType=pPart->GetClassTypeId();
			pPart->GetSpecification(pRec->spec);
			pRec->width=pPart->GetWidth();
			pRec->length=pPart->GetLength();
			pRec->piece_weight=pPart->GetWeight();
			strcpy(pRec->sNotes,pPart->sNotes);
		}
	}
	CPartStatRecord *recArr=new CPartStatRecord[matStat.GetNodeNum()];
	for(pRec=matStat.GetFirst(),i=0;pRec;pRec=matStat.GetNext(),i++)
		recArr[i]=*pRec;
	CHeapSort<CPartStatRecord>::HeapSort(recArr,matStat.GetNodeNum(),comparePartStatRecord);
	CFormatTbl xPartTbl;
	nRecordN = matStat.GetNodeNum();
	int nRowsN = nRecordN+4+attach_rows;	//��������
	const int nColumnsN = 9;	//��������
	CGrid::PREFER_TEXT_SIZE=sys.dim_map.fNumericTextSize;	//��ʼ�����Ԫ����߶�
	xPartTbl.Create(nRowsN,nColumnsN);
	for(i=0;i<nColumnsN;i++)	//����Ϊ������
		xPartTbl.GetGridAt(0,i)->textsize = sys.dim_map.fHeaderTextSize;
	for(i=0;i<nColumnsN;i++)	//����Ϊ������
	{
		xPartTbl.GetGridAt(1,i)->textsize = sys.dim_map.fCommonTextSize;
		xPartTbl.GetGridAt(2,i)->textsize = sys.dim_map.fCommonTextSize;
	}

	//���ɴ����-------���ϻ��ܱ�-----------
	xPartTbl.SetRowHigh(0,15);		//������
	xPartTbl.SetRowHigh(1,8);		//�ֶ���֮һ
	xPartTbl.SetRowHigh(2,8);		//�ֶ���֮��
	xPartTbl.SetRowHigh(nRowsN-1,10);//�ϼ���
	for(i=3;i<nRowsN-1;i++)			//������
		xPartTbl.SetRowHigh(i,6);
	xPartTbl.SetColumnWide(0,13);	//��Ϲ��������
	xPartTbl.SetColumnWide(1,13);	//�������������
	xPartTbl.SetColumnWide(2,27);	//�����//ԭΪ25������--Q345��Щ���Ų���,�ʷŴ�Ϊ27 WJH--01.11.01
	xPartTbl.SetColumnWide(3,15);	//������
	xPartTbl.SetColumnWide(4,10);	//������
	xPartTbl.SetColumnWide(5,15);	//����֮һ����
	xPartTbl.SetColumnWide(6,15);	//����֮С����
	xPartTbl.SetColumnWide(7,30);	//��ע��
	xPartTbl.SetColumnWide(8,13);	//��Ϲ�����������
	//����е�����ȱʡ�����½Ƕ��뼴feature = 0
	//feature��λ����ʾ�����ֵĶ��뷽ʽ0:������ 1��ʾ������ 2��ʾ���½�
	//ʮλ��ʾ������д����0:������1:���ϵ���

	//���������
	for(j=1;j<nColumnsN;j++)//ˮƽ�ϲ���һ���е����б��Ԫ
		xPartTbl.GetGridAt(0,j)->m_bHGridVirtual = TRUE;
	for(i=4;i<nRowsN;i++)//��ֱ�ϲ������Ϲ������һ�к��Ҳ���Ϲ�������һ��
	{
		xPartTbl.GetGridAt(i,0)->m_bVGridVirtual = TRUE;
		xPartTbl.GetGridAt(i,nColumnsN-1)->m_bVGridVirtual = TRUE;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(0,0)->data.SetGridString("weld parts list table");
#else
	xPartTbl.GetGridAt(0,0)->data.SetGridString("�� �� �� �� �� ϸ ��");
#endif
	xPartTbl.GetGridAt(0,0)->feature = 1;	//����Գ�MC
	//����--��Ϲ�����ű��Ԫ
	xPartTbl.GetGridAt(2,0)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,0)->data.SetGridString("group weld \\P number");
#else
	xPartTbl.GetGridAt(1,0)->data.SetGridString("�麸\\P���");
#endif
	xPartTbl.GetGridAt(1,0)->feature = 1;	//����Գ�MC
	//����--����������ű��Ԫ
	xPartTbl.GetGridAt(2,1)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,1)->data.SetGridString("part \\P number");
#else
	xPartTbl.GetGridAt(1,1)->data.SetGridString("����\\P���");
#endif
	xPartTbl.GetGridAt(1,1)->feature = 1;	//����Գ�MC
	//����--�����Ԫ
	xPartTbl.GetGridAt(2,2)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
	xPartTbl.GetGridAt(1,2)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,2)->data.SetGridString("size");
#else
	xPartTbl.GetGridAt(1,2)->data.SetGridString("��    ��");
#endif
	//����--���ȱ��Ԫ
	xPartTbl.GetGridAt(2,3)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
	xPartTbl.GetGridAt(1,3)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,3)->data.SetGridString("length\\P(mm)");
#else
	xPartTbl.GetGridAt(1,3)->data.SetGridString("��  ��\\P(mm)");
#endif
	//����--�������Ԫ
	xPartTbl.GetGridAt(2,4)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
	xPartTbl.GetGridAt(1,4)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,4)->data.SetGridString("amount");
#else
	xPartTbl.GetGridAt(1,4)->data.SetGridString("��\\P��");
#endif
	//����--�������Ԫ
	xPartTbl.GetGridAt(1,6)->m_bHGridVirtual = TRUE;//ˮƽ�ϲ���Ԫ��
	xPartTbl.GetGridAt(1,5)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,5)->data.SetGridString("weight(kg)");
		//weight of a piece table unit
	xPartTbl.GetGridAt(2,5)->feature = 1;
	xPartTbl.GetGridAt(2,5)->data.SetGridString("a piece");
		//subtotal weight table unit
	xPartTbl.GetGridAt(2,6)->feature = 1;
	xPartTbl.GetGridAt(2,6)->data.SetGridString("subtotal");
#else
	xPartTbl.GetGridAt(1,5)->data.SetGridString("��    ��(kg)");
		//����֮һ�����Ԫ
	xPartTbl.GetGridAt(2,5)->feature = 1;
	xPartTbl.GetGridAt(2,5)->data.SetGridString("һ ��");
		//����֮С�Ʊ��Ԫ
	xPartTbl.GetGridAt(2,6)->feature = 1;
	xPartTbl.GetGridAt(2,6)->data.SetGridString("С ��");
#endif
	//����--��ע���Ԫ
	xPartTbl.GetGridAt(2,7)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
	xPartTbl.GetGridAt(1,7)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,7)->data.SetGridString( "notes");
#else
	xPartTbl.GetGridAt(1,7)->data.SetGridString( "��    ע");
#endif
	//����--��Ϲ���������
	xPartTbl.GetGridAt(2,8)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
	xPartTbl.GetGridAt(1,8)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,8)->data.SetGridString( "group weld\\Pamount");
#else
	xPartTbl.GetGridAt(1,8)->data.SetGridString( "�麸\\P����");
#endif
	//����--������
	char tem_str[MAX_PATH];
	//��ϱ����
	xPartTbl.GetGridAt(3,0)->data.SetGridString(pPartGroup->sGroupName);//�麸����Ų���ʾ�����ַ� wjh-2013.5.20
	xPartTbl.GetGridAt(3,0)->feature=TEXTALIGN_MIDDLE_CENTER;	//���������
	xPartTbl.GetGridAt(3,0)->text_angle=Pi/2;
	for(i=3;i<nRowsN-1-attach_rows;i++)
	{
		//���������
		strcpy(tem_str,recArr[i-3].sPartNo);
		xPartTbl.GetGridAt(i,1)->data.SetGridString(tem_str);
		xPartTbl.GetGridAt(i,1)->feature=TEXTALIGN_MIDDLE_CENTER;	//���������
		//�����
		xPartTbl.GetGridAt(i,2)->data.SetGridString(recArr[i-3].spec);
		xPartTbl.GetGridAt(i,2)->feature=TEXTALIGN_LEFT_CENTER;	//���������
		//������
		sprintf(tem_str,"%.0f",recArr[i-3].length);	
		xPartTbl.GetGridAt(i,3)->data.SetGridString(tem_str);
		xPartTbl.GetGridAt(i,3)->feature=TEXTALIGN_RIGHT_CENTER;
		//������
		sprintf(tem_str,"%d",recArr[i-3].num);
		xPartTbl.GetGridAt(i,4)->data.SetGridString(tem_str);
		xPartTbl.GetGridAt(i,4)->feature=TEXTALIGN_MIDDLE_CENTER;	//���������
		//����֮һ����
		sprintf(tem_str,"%.2f",recArr[i-3].piece_weight);
		xPartTbl.GetGridAt(i,5)->data.SetGridString(tem_str);
		xPartTbl.GetGridAt(i,5)->feature=TEXTALIGN_RIGHT_CENTER;
		//����֮С����
		sprintf(tem_str,"%.1f",recArr[i-3].num*recArr[i-3].piece_weight);
		xPartTbl.GetGridAt(i,6)->data.SetGridString(tem_str);
		xPartTbl.GetGridAt(i,6)->feature=TEXTALIGN_RIGHT_CENTER;
		//��ע��
		xPartTbl.GetGridAt(i,7)->data.SetGridString(recArr[i-3].sNotes);
		xPartTbl.GetGridAt(i,7)->feature=TEXTALIGN_LEFT_CENTER;	//���������
	}
	//��Ϲ�������
	sprintf(tem_str,"%d",pPartGroup->m_uStatMatCoef);
	xPartTbl.GetGridAt(3,8)->data.SetGridString(tem_str);
	xPartTbl.GetGridAt(3,8)->feature=TEXTALIGN_MIDDLE_CENTER;	//���������
	//����ϼ���
	xPartTbl.GetGridAt(nRowsN-1,2)->m_bHGridVirtual = TRUE;//ˮƽ�ϲ���Ԫ��
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(nRowsN-1,1)->data.SetGridString("total");
#else
	xPartTbl.GetGridAt(nRowsN-1,1)->data.SetGridString("��      ��");
#endif
	xPartTbl.GetGridAt(nRowsN-1,1)->feature = 1; //�����������
	for(j=4;j<nColumnsN-1;j++)
		xPartTbl.GetGridAt(nRowsN-1,j)->m_bHGridVirtual = TRUE;//ˮƽ�ϲ���Ԫ��
	double weight = 0;
	for(i=0;i<matStat.GetNodeNum();i++)
		weight += TruncateDouble(recArr[i].num*recArr[i].piece_weight,1);
	sprintf(tem_str,"%.1fkg",weight);
	xPartTbl.GetGridAt(nRowsN-1,3)->data.SetGridString(tem_str);
	xPartTbl.GetGridAt(nRowsN-1,3)->feature = 1;	//�����������
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	DrawTable(pBlockTableRecord,&xPartTbl);
	pBlockTableRecord->close();
	delete []recArr;
}

SCOPE_STRU GetRectFromPoly(ATOM_LIST<PROFILE_VERTEX> &vertex_list)
{
	int nn;
	CRgn rgn;
	SCOPE_STRU scope;
	scope.ClearScope();
	nn = vertex_list.GetNodeNum();
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex!=NULL;pVertex=vertex_list.GetNext())
		scope.VerifyVertex(pVertex->vertex);
	return scope;
}

//Ԥ��׼���ֶ���˨��ϸ������
void PrepareSegBoltTable(CFormatTbl &xBoltPadTbl,SEGI nSegI=-1)
{
	CSegMaterialSummator xSegMatSum;
	int i,j,nLsRecN,nNailRecN,nPadRecN,nTwoCapLsRecN=0;
	int attach_ls_rows = 2;
	int attach_nail_rows = 0,attach_pad_rows = 0;
	if(nSegI.iSeg==-1)
		nSegI=Ta.m_iCurSeg;
	if(bom.Bolts.GetNodeNum()>0)
		xSegMatSum.RunSummarizeFromBOM(bom,nSegI,false,true);
	else if(!xSegMatSum.RunSummarize(nSegI))
		return;
#ifdef AFX_TARG_ENU_ENGLISH
	BOOL EXPORT_BY_ENGLISH=TRUE;
#else
	BOOL EXPORT_BY_ENGLISH=sys.stru_map.bExportByEnglish;
#endif
	nLsRecN = xSegMatSum.GetLsRecCount();
	nNailRecN = xSegMatSum.GetNailRecCount();
	nPadRecN  = xSegMatSum.GetPadRecCount();
//	nTwoCapLsRecN = xSegMatSum.GetTwoCapLsRecCount();
	if(nLsRecN>0&&nLsRecN+attach_ls_rows<3)
		attach_ls_rows = 3-nLsRecN;
	if(nTwoCapLsRecN>0&&nTwoCapLsRecN+attach_ls_rows<3)
		attach_ls_rows = 3-nTwoCapLsRecN;
	if(nNailRecN>0&&nNailRecN+attach_nail_rows<3)
		attach_nail_rows = 3-nNailRecN;
	if(nPadRecN>0&&nPadRecN+attach_pad_rows<3)
		attach_pad_rows = 3-nPadRecN;

	int nRowsN = 2+nLsRecN+attach_ls_rows+nNailRecN+nTwoCapLsRecN+
		attach_nail_rows+nPadRecN+attach_pad_rows+1;	//��������
	const int nColumnsN = 7;	//��������
	CGrid::PREFER_TEXT_SIZE=sys.dim_map.fNumericTextSize;	//��ʼ�����Ԫ����߶�
	xBoltPadTbl.Create(nRowsN,nColumnsN);
	for(i=0;i<nColumnsN;i++)	//����Ϊ������
		xBoltPadTbl.GetGridAt(0,i)->textsize = sys.dim_map.fHeaderTextSize;
	for(i=0;i<nColumnsN;i++)	//�ڶ���
		xBoltPadTbl.GetGridAt(1,i)->textsize = sys.dim_map.fCommonTextSize;
	for(i=2;i<nRowsN;i++)	//�ڶ���
	{
		xBoltPadTbl.GetGridAt(i,0)->textsize = sys.dim_map.fCommonTextSize;
		xBoltPadTbl.GetGridAt(i,1)->textsize = sys.dim_map.fCommonTextSize;
	}
	//���ɴ����-------���ϻ��ܱ�-----------
	xBoltPadTbl.SetRowHigh(0,15);		//������
	xBoltPadTbl.SetRowHigh(1,10);		//�ֶ���

	xBoltPadTbl.SetRowHigh(nRowsN-1,10);//�ϼ���
	xBoltPadTbl.GetGridAt(nRowsN-1,1)->m_bHGridVirtual = TRUE;//ˮƽ�ϲ���Ԫ��
	xBoltPadTbl.GetGridAt(nRowsN-1,0)->feature = 1;	//�����������
	if(EXPORT_BY_ENGLISH)
		xBoltPadTbl.GetGridAt(nRowsN-1,0)->data.SetGridString( "Total");
	else
		xBoltPadTbl.GetGridAt(nRowsN-1,0)->data.SetGridString( "��    ��");
	for(i=2;i<nRowsN-1;i++)			//������
		xBoltPadTbl.SetRowHigh(i,6);
	if(sys.general_set.iPartBOMStyle==0)
	{	//��׼��ʽ ��Ҫ����TMA
		xBoltPadTbl.SetColumnWide(0,10);	//������
		xBoltPadTbl.SetColumnWide(1,15);	//������
		xBoltPadTbl.SetColumnWide(2,25);	//�����
		xBoltPadTbl.SetColumnWide(3,15);	//������
		xBoltPadTbl.SetColumnWide(4,10);	//������
		xBoltPadTbl.SetColumnWide(5,15);	//������
		xBoltPadTbl.SetColumnWide(6,30);	//��ע��
	}
	else
	{	//�ӿ���ʽ
		xBoltPadTbl.SetColumnWide(0,10);	//������
		xBoltPadTbl.SetColumnWide(1,18);	//������//ԭΪ15���ڹ�����ϸ�������ӿ�6 �ʴ˴���3 ��������3
		xBoltPadTbl.SetColumnWide(2,30);	//�����//ԭΪ25,���޸�Ϊ30,Ϊ�����빹����ϸ����һ��
		xBoltPadTbl.SetColumnWide(3,15);	//������
		xBoltPadTbl.SetColumnWide(4,13);	//������//ԭΪ10,���޸�Ϊ13,Ϊ�����빹����ϸ����һ��
		xBoltPadTbl.SetColumnWide(5,23);	//������//ԭΪ20,���޸�Ϊ23,Ϊ�����빹����ϸ����һ��	
		xBoltPadTbl.SetColumnWide(6,30);	//��ע��
	}
	//����е�����ȱʡ�����½Ƕ��뼴feature = 0
	//feature��λ����ʾ�����ֵĶ��뷽ʽ0:������ 1��ʾ������ 2��ʾ���½�
	//ʮλ��ʾ������д����0:������1:���ϵ���

	//���������
	for(j=1;j<nColumnsN;j++)//ˮƽ�ϲ���һ���е����б��Ԫ
		xBoltPadTbl.GetGridAt(0,j)->m_bHGridVirtual = TRUE;
	char* title_en[] = {"Type","Grade","Size","Symbol","Qty","Wt.(kg)","Notes"};
	char* title_cn[] = {"����","����","���","����","����","����(kg)","��ע"};
	if(EXPORT_BY_ENGLISH)
	{
		xBoltPadTbl.GetGridAt(0,0)->data.SetGridString("BILL OF BOLTS AND WASHERES");
		xBoltPadTbl.GetGridAt(0,0)->feature = 1;
	}
	else
	{
		xBoltPadTbl.GetGridAt(0,0)->data.SetGridString("��˨���Ŷ�����Ȧ��ϸ��");
		xBoltPadTbl.GetGridAt(0,0)->feature = 1;
	}
	for(j=0;j<nColumnsN;j++)
	{
		if(EXPORT_BY_ENGLISH)
			xBoltPadTbl.GetGridAt(1,j)->data.SetGridString(title_en[j]);
		else
			xBoltPadTbl.GetGridAt(1,j)->data.SetGridString(title_cn[j]);
		xBoltPadTbl.GetGridAt(1,j)->feature = 1;//����Գ�MC
	}
	//����--��˨�����Ԫ
	int nCurRow = 2+nLsRecN+nTwoCapLsRecN+attach_ls_rows;
	if(nLsRecN+nTwoCapLsRecN>0)
	{
		for(i=3;i<2+nLsRecN+nTwoCapLsRecN+attach_ls_rows;i++)
		{
			xBoltPadTbl.GetGridAt(i,0)->m_bVGridVirtual=TRUE;//��ֱ�ϲ���Ԫ��
			xBoltPadTbl.GetGridAt(i,1)->m_bVGridVirtual=TRUE;//��ֱ�ϲ���Ԫ��
		}
		char sOldGrade[5]="",grade[8]="";
		for(i=2;i<2+nLsRecN+nTwoCapLsRecN;i++)
		{
			char guige[100];
			CSegMatLsRecord *pRec = xSegMatSum.GetLsRecAt(i-2);
			//if(i>=2+nLsRecN)	//˫ñ��˨ wht 11-04-25
			//	pRec = xSegMatSum.GetTwoCapLsRecAt(i-2-nLsRecN);
			pRec->GetGrade(grade);
			if(strcmp(grade,sOldGrade)!=0)	//��˨����任
			{
				strcpy(sOldGrade,grade);
				if(EXPORT_BY_ENGLISH)
					strcat(grade,"G");
				else
					strcat(grade,"��");
				xBoltPadTbl.GetGridAt(i,1)->data.SetGridString(grade);
				xBoltPadTbl.GetGridAt(i,1)->feature = 1;	//����Գ�MC
				xBoltPadTbl.GetGridAt(i,1)->m_bVGridVirtual=FALSE;
			}
			pRec->GetGuiGe(guige,false);
			xBoltPadTbl.GetGridAt(i,2)->data.SetGridString(guige);
			xBoltPadTbl.GetGridAt(i,2)->feature = TEXTALIGN_LEFT_CENTER;
			pRec->GetGuiGe(guige);
			for(size_t j=0;j<strlen(guige);j++)
			{
				if(guige[j]=='M')	//ȥ����˨����
					strcpy(guige,&guige[j]);
			}
			xBoltPadTbl.GetGridAt(i,3)->data.SetGridString(guige);
			xBoltPadTbl.GetGridAt(i,3)->grid_mode = PIC_BLOCK_GRID;
			sprintf(guige,"%d",pRec->GetPartN());
			xBoltPadTbl.GetGridAt(i,4)->data.SetGridString(guige);
			xBoltPadTbl.GetGridAt(i,4)->feature = TEXTALIGN_MIDDLE_CENTER;
			sprintf(guige,"%.1f",pRec->GetSumWeight());
			xBoltPadTbl.GetGridAt(i,5)->data.SetGridString(guige);
			xBoltPadTbl.GetGridAt(i,6)->feature = TEXTALIGN_LEFT_CENTER;
			xBoltPadTbl.GetGridAt(i,6)->data.SetGridString(pRec->sNotes);
		}
		if(EXPORT_BY_ENGLISH)
		{
			xBoltPadTbl.GetGridAt(2,0)->data.SetGridString("Bolts");
			xBoltPadTbl.GetGridAt(2,0)->text_angle=270*RADTODEG_COEF;
		}
		else
			xBoltPadTbl.GetGridAt(2,0)->data.SetGridString("��\\P\\P˨");
		xBoltPadTbl.GetGridAt(2,0)->feature = 1;	//����Գ�MC
	}
	//����--�Ŷ������Ԫ
	if(nNailRecN>0)
	{
		for(i=nCurRow+1;i<nCurRow+nNailRecN+attach_nail_rows;i++)
		{
			xBoltPadTbl.GetGridAt(i,0)->m_bVGridVirtual=TRUE;//��ֱ�ϲ���Ԫ��
			xBoltPadTbl.GetGridAt(i,1)->m_bVGridVirtual=TRUE;//��ֱ�ϲ���Ԫ��
			xBoltPadTbl.GetGridAt(i,3)->m_bVGridVirtual=TRUE;//��ֱ�ϲ���Ԫ��
		}
		char sOldGrade[5]="",grade[8]="";
		for(i=nCurRow;i<nCurRow+nNailRecN;i++)
		{
			char tem_str[100];
			CSegMatLsRecord *pRec = xSegMatSum.GetNailRecAt(i-nCurRow);
			pRec->GetGrade(grade);
			if(strcmp(grade,sOldGrade)!=0)	//��˨����任
			{
				strcpy(sOldGrade,grade);
				if(EXPORT_BY_ENGLISH)
					strcat(grade,"G");
				else
					strcat(grade,"��");
				xBoltPadTbl.GetGridAt(i,1)->data.SetGridString(grade);
				xBoltPadTbl.GetGridAt(i,1)->feature = 1;	//����Գ�MC
				xBoltPadTbl.GetGridAt(i,1)->m_bVGridVirtual=FALSE;
			}
			pRec->GetGuiGe(tem_str,false);
			xBoltPadTbl.GetGridAt(i,2)->data.SetGridString(tem_str);
			xBoltPadTbl.GetGridAt(i,2)->feature = TEXTALIGN_LEFT_CENTER;
			sprintf(tem_str,"%d",pRec->GetPartN());
			xBoltPadTbl.GetGridAt(i,4)->data.SetGridString(tem_str);
			xBoltPadTbl.GetGridAt(i,4)->feature = TEXTALIGN_MIDDLE_CENTER;
			sprintf(tem_str,"%.1f",pRec->GetSumWeight());
			xBoltPadTbl.GetGridAt(i,5)->data.SetGridString(tem_str);
			xBoltPadTbl.GetGridAt(i,6)->feature = TEXTALIGN_LEFT_CENTER;
			xBoltPadTbl.GetGridAt(i,6)->data.SetGridString(pRec->sNotes);
		}
		xBoltPadTbl.GetGridAt(nCurRow,3)->data.SetGridString("NAIL");
		xBoltPadTbl.GetGridAt(nCurRow,3)->grid_mode = PIC_BLOCK_GRID;
		xBoltPadTbl.GetGridAt(nCurRow,0)->feature = 1;
		if(EXPORT_BY_ENGLISH)
			xBoltPadTbl.GetGridAt(nCurRow,0)->data.SetGridString("Step Bolts");
		else
			xBoltPadTbl.GetGridAt(nCurRow,0)->data.SetGridString("��\\P��");
		//xBoltPadTbl.GetGridAt(nCurRow,1)->feature = 1;
		//xBoltPadTbl.GetGridAt(nCurRow,1)->data.SetGridString("4.8��\\P��\\P6.8��");
		nCurRow +=nNailRecN+attach_nail_rows;
	}
	//����--��Ȧ�����Ԫ
	if(nPadRecN>0)
	{
		for(i=nCurRow+1;i<nCurRow+nPadRecN+attach_pad_rows;i++)
		{
			xBoltPadTbl.GetGridAt(i,0)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
			xBoltPadTbl.GetGridAt(i,1)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
			xBoltPadTbl.GetGridAt(i,3)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
		}
		for(i=nCurRow;i<nCurRow+nPadRecN;i++)
		{
			char tem_str[100];
			CSegMatPadRecord *pRec = xSegMatSum.GetPadRecAt(i-nCurRow);
			pRec->GetGuiGe(tem_str);
			//��Ȧ���ջ�ͼ�淶Ӧ��Ϊ-3(��17.5)����ʽ
			CLDSBolt ls;
			restore_Ls_guige(tem_str,ls);	//��������Ȧ����е���˨ֱ�����������
			int thick = (int)ls.get_L();
			int d = ls.get_d();
			if(d<=16)	//�����¹����ʽ
				sprintf(tem_str,"-%d(��%.1f)",thick,d+1.5);
			else
				sprintf(tem_str,"-%d(��%.0f)",thick,d+2.0);
			xBoltPadTbl.GetGridAt(i,2)->data.SetGridString(tem_str);
			xBoltPadTbl.GetGridAt(i,2)->feature = TEXTALIGN_LEFT_CENTER;
			sprintf(tem_str,"%d",pRec->GetPartN());
			xBoltPadTbl.GetGridAt(i,4)->data.SetGridString(tem_str);
			xBoltPadTbl.GetGridAt(i,4)->feature = TEXTALIGN_MIDDLE_CENTER;
			sprintf(tem_str,"%.1f",pRec->GetSumWeight());
			xBoltPadTbl.GetGridAt(i,5)->data.SetGridString(tem_str);
			xBoltPadTbl.GetGridAt(i,6)->data.SetGridString(pRec->sNotes);
		}
		xBoltPadTbl.GetGridAt(nCurRow,3)->data.SetGridString("PAD");
		xBoltPadTbl.GetGridAt(nCurRow,3)->grid_mode = PIC_BLOCK_GRID;
		xBoltPadTbl.GetGridAt(nCurRow,0)->feature = 1;
		if(EXPORT_BY_ENGLISH)
			xBoltPadTbl.GetGridAt(nCurRow,0)->data.SetGridString("Washer");
		else
			xBoltPadTbl.GetGridAt(nCurRow,0)->data.SetGridString("��\\PȦ");
		xBoltPadTbl.GetGridAt(nCurRow,1)->feature = 1;
		xBoltPadTbl.GetGridAt(nCurRow,1)->data.SetGridString("Q235");
	}
	//����ϼ�������
	for(j=3;j<7;j++)
		xBoltPadTbl.GetGridAt(nRowsN-1,j)->m_bHGridVirtual = TRUE;//ˮƽ�ϲ�
	double weight = 0;
	for(i=0;i<xSegMatSum.GetLsRecCount();i++)
		weight += TruncateDouble(xSegMatSum.GetLsRecAt(i)->GetSumWeight(),1);
	//for(i=0;i<xSegMatSum.GetTwoCapLsRecCount();i++)
	//	weight += xSegMatSum.GetTwoCapLsRecAt(i)->GetSumWeight();
	for(i=0;i<xSegMatSum.GetNailRecCount();i++)
		weight += TruncateDouble(xSegMatSum.GetNailRecAt(i)->GetSumWeight(),1);
	for(i=0;i<xSegMatSum.GetPadRecCount();i++)
		weight += TruncateDouble(xSegMatSum.GetPadRecAt(i)->GetSumWeight(),1);
	char tem_str[100];
	sprintf(tem_str,"%.1fkg",weight);
	xBoltPadTbl.GetGridAt(nRowsN-1,2)->data.SetGridString(tem_str);
	xBoltPadTbl.GetGridAt(nRowsN-1,2)->feature = 1;	//�����м����
}

//��˨�Ŷ���Ȧ��ϸ��
void BoltPadBOM()
{
	sys.m_iCurMapType = 3;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	CMxbDlg dlg;
	if(dlg.DoModal()!=IDOK)
		return;
	int nLocatePos=dlg.m_nLocatePos;
	for(SEGI* pSegI=dlg.m_xSegNoList.GetFirst();pSegI;pSegI=dlg.m_xSegNoList.GetNext())
	{
		CFormatTbl xBoltPadTbl;
		PrepareSegBoltTable(xBoltPadTbl,*pSegI);
		AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
		DrawTable(pBlockTableRecord,&xBoltPadTbl,TRUE,1,nLocatePos);
		pBlockTableRecord->close();
	}
}
#else
void SegPartBOM(){;}
void BoltPadBOM(){;}
void PartGroupBOM(){;}
#endif