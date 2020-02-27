//<LOCALE_TRANSLATE/>
/*
说明 ---- 表格形式有以下八种:
                  1.定位原点位于左上角且纵向排列
                  2.定位原点位于左下角且纵向排列
                  3.定位原点位于右上角且纵向排列
                  4.定位原点位于右下角且纵向排列
                  5.定位原点位于左上角且分段横向排列
                  6.定位原点位于左下角且分段横向排列
                  7.定位原点位于右上角且分段横向排列
                  8.定位原点位于右下角且分段横向排列
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
//构件明细表
//[in]iSegNo>——段号
//预先准备分段构件表数据
BOOL PrepareSegPartTable(CFormatTbl &xPartTbl,SEGI nSegI=-1)
{
#ifdef AFX_TARG_ENU_ENGLISH
	bool EXPORT_BY_ENGLISH=true;
#else
	bool EXPORT_BY_ENGLISH=(sys.stru_map.bExportByEnglish!=FALSE);
#endif
	CSegMaterialSummator::EXPORT_BY_ENGLISH=EXPORT_BY_ENGLISH;

	CSegMaterialSummator xSegMatSum;
	//attach_rows是留作备用的空行总数
	int i,j,nRecordN,attach_rows=2;
	if(nSegI.iSeg==-1)
		nSegI=Ta.m_iCurSeg;
	if(bom.Parts.GetNodeNum()>0)
		xSegMatSum.RunSummarizeFromBOM(bom,nSegI,true,false);
	else if(!xSegMatSum.RunSummarize(nSegI))
		return FALSE;
	nRecordN = xSegMatSum.GetJgPlankRecCount();
	int nRowsN = nRecordN+4+attach_rows;	//表行总数
	const int nColumnsN = 7;	//表列总数
	CGrid::PREFER_TEXT_SIZE=sys.dim_map.fNumericTextSize;	//初始化表格单元字体高度
	xPartTbl.Create(nRowsN,nColumnsN);
	for(i=0;i<nColumnsN;i++)	//首行为标题行
		xPartTbl.GetGridAt(0,i)->textsize = sys.dim_map.fHeaderTextSize;
	for(i=0;i<nColumnsN;i++)	//首行为标题行
	{
		xPartTbl.GetGridAt(1,i)->textsize = sys.dim_map.fCommonTextSize;
		xPartTbl.GetGridAt(2,i)->textsize = sys.dim_map.fCommonTextSize;
	}

	//生成大标题-------材料汇总表-----------
	xPartTbl.SetRowHigh(0,15);		//标题行
	xPartTbl.SetRowHigh(1,8);		//字段行之一
	xPartTbl.SetRowHigh(2,8);		//字段行之二
	xPartTbl.SetRowHigh(nRowsN-1,10);//合计行
	for(i=3;i<nRowsN-1;i++)			//数据行
		xPartTbl.SetRowHigh(i,6);
	if(sys.general_set.iPartBOMStyle==0)
	{	//标准样式 主要用于TMA
		xPartTbl.SetColumnWide(0,10);	//编号栏
		xPartTbl.SetColumnWide(1,25);	//规格栏
		xPartTbl.SetColumnWide(2,15);	//长度栏
		xPartTbl.SetColumnWide(3,10);	//数量栏
		xPartTbl.SetColumnWide(4,15);	//重量之一件栏
		xPartTbl.SetColumnWide(5,15);	//重量之小计栏
		xPartTbl.SetColumnWide(6,30);	//备注栏
	}
	else
	{	//加宽样式
		xPartTbl.SetColumnWide(0,19);	//编号栏//原为13因为标准构件的编号比较长故调整为19 同时调整螺栓统计表宽度 
		xPartTbl.SetColumnWide(1,27);	//规格栏//原为25但由于--Q345有些规格放不下,故放大为27 WJH--01.11.01
		xPartTbl.SetColumnWide(2,23);	//长度栏//原为15因为标注法兰长度时需标注一个内圆直径~外圆直径的范围 故修改为23
		xPartTbl.SetColumnWide(3,10);	//数量栏
		xPartTbl.SetColumnWide(4,15);	//重量之一件栏
		xPartTbl.SetColumnWide(5,15);	//重量之小计栏
		xPartTbl.SetColumnWide(6,30);	//备注栏
	}
	//表格中的文字缺省是右下角对齐即feature = 0
	//feature个位数表示，文字的对齐方式0:右中央 1表示正中央 2表示右下角
	//十位表示文字书写方向0:从左到右1:从上到下

	//定义标题行
	for(j=1;j<nColumnsN;j++)//水平合并第一行中的所有表格单元
		xPartTbl.GetGridAt(0,j)->m_bHGridVirtual = TRUE;
	if(EXPORT_BY_ENGLISH)
		xPartTbl.GetGridAt(0,0)->data.SetGridString("BILL OF MATERIAL");
	else
		xPartTbl.GetGridAt(0,0)->data.SetGridString("构 件 明 细 表");

	xPartTbl.GetGridAt(0,0)->feature = 1;	//中央对称MC
	//定义--编号表格单元
	xPartTbl.GetGridAt(2,0)->m_bVGridVirtual = TRUE;//坚直合并单元格
	if(EXPORT_BY_ENGLISH)
		xPartTbl.GetGridAt(1,0)->data.SetGridString("Mark\\PNo.");
	else
		xPartTbl.GetGridAt(1,0)->data.SetGridString("编  号");

	xPartTbl.GetGridAt(1,0)->feature = 1;	//中央对称MC
	//定义--规格表格单元
	xPartTbl.GetGridAt(2,1)->m_bVGridVirtual = TRUE;//坚直合并单元格
	xPartTbl.GetGridAt(1,1)->feature = 1;
	if(EXPORT_BY_ENGLISH)
		xPartTbl.GetGridAt(1,1)->data.SetGridString("Specification");
	else
		xPartTbl.GetGridAt(1,1)->data.SetGridString("规    格");
	//定义--长度表格单元
	xPartTbl.GetGridAt(2,2)->m_bVGridVirtual = TRUE;//坚直合并单元格
	xPartTbl.GetGridAt(1,2)->feature = 1;
	if(EXPORT_BY_ENGLISH)
		xPartTbl.GetGridAt(1,2)->data.SetGridString("Length\\P(mm)");
	else
		xPartTbl.GetGridAt(1,2)->data.SetGridString("长  度\\P(mm)");
	//定义--数量表格单元
	xPartTbl.GetGridAt(2,3)->m_bVGridVirtual = TRUE;//坚直合并单元格
	xPartTbl.GetGridAt(1,3)->feature = 1;
	if(EXPORT_BY_ENGLISH)
		xPartTbl.GetGridAt(1,3)->data.SetGridString("Qty");
	else
		xPartTbl.GetGridAt(1,3)->data.SetGridString("数\\P量");
	//定义--重量表格单元
	xPartTbl.GetGridAt(1,5)->m_bHGridVirtual = TRUE;//水平合并单元格
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
		xPartTbl.GetGridAt(1,4)->data.SetGridString("重    量(kg)");
			//重量之一件表格单元
		xPartTbl.GetGridAt(2,4)->feature = 1;
		xPartTbl.GetGridAt(2,4)->data.SetGridString("一 件");
			//重量之小计表格单元
		xPartTbl.GetGridAt(2,5)->feature = 1;
		xPartTbl.GetGridAt(2,5)->data.SetGridString("小 计");
	}
	//定义--备注表格单元
	xPartTbl.GetGridAt(2,6)->m_bVGridVirtual = TRUE;//坚直合并单元格
	xPartTbl.GetGridAt(1,6)->feature = 1;
	if(EXPORT_BY_ENGLISH)
		xPartTbl.GetGridAt(1,6)->data.SetGridString( "Notes");
	else
		xPartTbl.GetGridAt(1,6)->data.SetGridString( "备    注");
	//定义--数据区
	char tem_str[MAX_PATH];
	int nInvalidNum=0;		//记录不可用数据个数 wht 11-08-03
	double sum_weight=0;
	for(i=3;i<nRowsN-1-attach_rows;i++)
	{
		CSegMatPolePlankRecord *pRec = xSegMatSum.GetJgPlankRecAt(i-3);
		if (strlen(pRec->GetPartNo())<=0||pRec->GetPartN()<=0)
		{	//构件编号为NULL或构件数为0
			nInvalidNum++;
			continue;	
		}
		//编号域
		strcpy(tem_str,pRec->GetPartNo(sys.general_set.iMatCharPosType));
		xPartTbl.GetGridAt(i-nInvalidNum,0)->data.SetGridString(tem_str);
		xPartTbl.GetGridAt(i-nInvalidNum,0)->feature=TEXTALIGN_LEFT_CENTER;	//由正中对齐修改为左中对齐,为与合并后的构件号对齐 wht 12-03-22
		if(pRec->bStdPart)
		{	//当前记录为标准件,合并件号栏与规格栏,不标注规格与长度 wht 12-03-22
			xPartTbl.GetGridAt(i-nInvalidNum,1)->m_bHGridVirtual=TRUE;
		}
		else 
		{
			//规格域
			xPartTbl.GetGridAt(i-nInvalidNum,1)->data.SetGridString(pRec->guige);
			xPartTbl.GetGridAt(i-nInvalidNum,1)->feature=TEXTALIGN_LEFT_CENTER;	//左中央对齐
			//长度域
			//sprintf(tem_str,"%.0f",pRec->GetLength());
			xPartTbl.GetGridAt(i-nInvalidNum,2)->data.SetGridString(pRec->sLength);
			xPartTbl.GetGridAt(i-nInvalidNum,2)->feature=TEXTALIGN_RIGHT_CENTER;
		}
		//数量域
		sprintf(tem_str,"%d",pRec->GetPartN());
		xPartTbl.GetGridAt(i-nInvalidNum,3)->data.SetGridString(tem_str);
		xPartTbl.GetGridAt(i-nInvalidNum,3)->feature=TEXTALIGN_MIDDLE_CENTER;	//正中央对齐
		//重量之一件域
		sprintf(tem_str,"%.2f",pRec->GetUnitWeight());
		xPartTbl.GetGridAt(i-nInvalidNum,4)->data.SetGridString(tem_str);
		xPartTbl.GetGridAt(i-nInvalidNum,4)->feature=TEXTALIGN_RIGHT_CENTER;
		//重量之小计域
		sprintf(tem_str,"%.1f",pRec->GetSumWeight());
		xPartTbl.GetGridAt(i-nInvalidNum,5)->data.SetGridString(tem_str);
		xPartTbl.GetGridAt(i-nInvalidNum,5)->feature=TEXTALIGN_RIGHT_CENTER;
		//备注域
		xPartTbl.GetGridAt(i-nInvalidNum,6)->data.SetGridString(pRec->sNotes);
		xPartTbl.GetGridAt(i-nInvalidNum,6)->feature=TEXTALIGN_LEFT_CENTER;	//左中央对齐
		//总重
		sum_weight+=TruncateDouble(pRec->GetSumWeight(),1);
	}
	//定义合计行
	xPartTbl.GetGridAt(nRowsN-1,1)->m_bHGridVirtual = TRUE;//水平合并单元格
	if(EXPORT_BY_ENGLISH)
		xPartTbl.GetGridAt(nRowsN-1,0)->data.SetGridString("Total");
	else
		xPartTbl.GetGridAt(nRowsN-1,0)->data.SetGridString("合      计");
	xPartTbl.GetGridAt(nRowsN-1,0)->feature = 1; //文字中央对齐
	for(j=3;j<nColumnsN;j++)
		xPartTbl.GetGridAt(nRowsN-1,j)->m_bHGridVirtual = TRUE;//水平合并单元格
	//
	sprintf(tem_str,"%.1fkg",sum_weight);
	xPartTbl.GetGridAt(nRowsN-1,2)->data.SetGridString(tem_str);
	xPartTbl.GetGridAt(nRowsN-1,2)->feature = 1;	//文字中央对齐
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
//编号按升序排列
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
//组合构件明细表
void PartGroupBOM()
{
	sys.m_iCurMapType = 4;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
    CAcModuleResourceOverride resOverride;
	CSelPartGroupDlg selPartGroupDlg;
	selPartGroupDlg.m_bPartGroupBOM=TRUE;
	if(selPartGroupDlg.DoModal()!=IDOK)
		return;
	//attach_rows是留作备用的空行总数
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
			continue;	//无编号构件略去不统计
		else if(pPart->m_bVirtualPart)
			continue;	//虚拟辅助构件
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
	int nRowsN = nRecordN+4+attach_rows;	//表行总数
	const int nColumnsN = 9;	//表列总数
	CGrid::PREFER_TEXT_SIZE=sys.dim_map.fNumericTextSize;	//初始化表格单元字体高度
	xPartTbl.Create(nRowsN,nColumnsN);
	for(i=0;i<nColumnsN;i++)	//首行为标题行
		xPartTbl.GetGridAt(0,i)->textsize = sys.dim_map.fHeaderTextSize;
	for(i=0;i<nColumnsN;i++)	//首行为标题行
	{
		xPartTbl.GetGridAt(1,i)->textsize = sys.dim_map.fCommonTextSize;
		xPartTbl.GetGridAt(2,i)->textsize = sys.dim_map.fCommonTextSize;
	}

	//生成大标题-------材料汇总表-----------
	xPartTbl.SetRowHigh(0,15);		//标题行
	xPartTbl.SetRowHigh(1,8);		//字段行之一
	xPartTbl.SetRowHigh(2,8);		//字段行之二
	xPartTbl.SetRowHigh(nRowsN-1,10);//合计行
	for(i=3;i<nRowsN-1;i++)			//数据行
		xPartTbl.SetRowHigh(i,6);
	xPartTbl.SetColumnWide(0,13);	//组合构件编号栏
	xPartTbl.SetColumnWide(1,13);	//单个构件编号栏
	xPartTbl.SetColumnWide(2,27);	//规格栏//原为25但由于--Q345有些规格放不下,故放大为27 WJH--01.11.01
	xPartTbl.SetColumnWide(3,15);	//长度栏
	xPartTbl.SetColumnWide(4,10);	//数量栏
	xPartTbl.SetColumnWide(5,15);	//重量之一件栏
	xPartTbl.SetColumnWide(6,15);	//重量之小计栏
	xPartTbl.SetColumnWide(7,30);	//备注栏
	xPartTbl.SetColumnWide(8,13);	//组合构件单基数量
	//表格中的文字缺省是右下角对齐即feature = 0
	//feature个位数表示，文字的对齐方式0:右中央 1表示正中央 2表示右下角
	//十位表示文字书写方向0:从左到右1:从上到下

	//定义标题行
	for(j=1;j<nColumnsN;j++)//水平合并第一行中的所有表格单元
		xPartTbl.GetGridAt(0,j)->m_bHGridVirtual = TRUE;
	for(i=4;i<nRowsN;i++)//竖直合并左侧组合构件编号一列和右侧组合构件数量一列
	{
		xPartTbl.GetGridAt(i,0)->m_bVGridVirtual = TRUE;
		xPartTbl.GetGridAt(i,nColumnsN-1)->m_bVGridVirtual = TRUE;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(0,0)->data.SetGridString("weld parts list table");
#else
	xPartTbl.GetGridAt(0,0)->data.SetGridString("组 焊 构 件 明 细 表");
#endif
	xPartTbl.GetGridAt(0,0)->feature = 1;	//中央对称MC
	//定义--组合构件编号表格单元
	xPartTbl.GetGridAt(2,0)->m_bVGridVirtual = TRUE;//坚直合并单元格
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,0)->data.SetGridString("group weld \\P number");
#else
	xPartTbl.GetGridAt(1,0)->data.SetGridString("组焊\\P编号");
#endif
	xPartTbl.GetGridAt(1,0)->feature = 1;	//中央对称MC
	//定义--单个构件编号表格单元
	xPartTbl.GetGridAt(2,1)->m_bVGridVirtual = TRUE;//坚直合并单元格
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,1)->data.SetGridString("part \\P number");
#else
	xPartTbl.GetGridAt(1,1)->data.SetGridString("构件\\P编号");
#endif
	xPartTbl.GetGridAt(1,1)->feature = 1;	//中央对称MC
	//定义--规格表格单元
	xPartTbl.GetGridAt(2,2)->m_bVGridVirtual = TRUE;//坚直合并单元格
	xPartTbl.GetGridAt(1,2)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,2)->data.SetGridString("size");
#else
	xPartTbl.GetGridAt(1,2)->data.SetGridString("规    格");
#endif
	//定义--长度表格单元
	xPartTbl.GetGridAt(2,3)->m_bVGridVirtual = TRUE;//坚直合并单元格
	xPartTbl.GetGridAt(1,3)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,3)->data.SetGridString("length\\P(mm)");
#else
	xPartTbl.GetGridAt(1,3)->data.SetGridString("长  度\\P(mm)");
#endif
	//定义--数量表格单元
	xPartTbl.GetGridAt(2,4)->m_bVGridVirtual = TRUE;//坚直合并单元格
	xPartTbl.GetGridAt(1,4)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,4)->data.SetGridString("amount");
#else
	xPartTbl.GetGridAt(1,4)->data.SetGridString("数\\P量");
#endif
	//定义--重量表格单元
	xPartTbl.GetGridAt(1,6)->m_bHGridVirtual = TRUE;//水平合并单元格
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
	xPartTbl.GetGridAt(1,5)->data.SetGridString("重    量(kg)");
		//重量之一件表格单元
	xPartTbl.GetGridAt(2,5)->feature = 1;
	xPartTbl.GetGridAt(2,5)->data.SetGridString("一 件");
		//重量之小计表格单元
	xPartTbl.GetGridAt(2,6)->feature = 1;
	xPartTbl.GetGridAt(2,6)->data.SetGridString("小 计");
#endif
	//定义--备注表格单元
	xPartTbl.GetGridAt(2,7)->m_bVGridVirtual = TRUE;//坚直合并单元格
	xPartTbl.GetGridAt(1,7)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,7)->data.SetGridString( "notes");
#else
	xPartTbl.GetGridAt(1,7)->data.SetGridString( "备    注");
#endif
	//定义--组合构件单基数
	xPartTbl.GetGridAt(2,8)->m_bVGridVirtual = TRUE;//坚直合并单元格
	xPartTbl.GetGridAt(1,8)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,8)->data.SetGridString( "group weld\\Pamount");
#else
	xPartTbl.GetGridAt(1,8)->data.SetGridString( "组焊\\P数量");
#endif
	//定义--数据区
	char tem_str[MAX_PATH];
	//组合编号域
	xPartTbl.GetGridAt(3,0)->data.SetGridString(pPartGroup->sGroupName);//组焊件编号不显示材质字符 wjh-2013.5.20
	xPartTbl.GetGridAt(3,0)->feature=TEXTALIGN_MIDDLE_CENTER;	//正中央对齐
	xPartTbl.GetGridAt(3,0)->text_angle=Pi/2;
	for(i=3;i<nRowsN-1-attach_rows;i++)
	{
		//构件编号域
		strcpy(tem_str,recArr[i-3].sPartNo);
		xPartTbl.GetGridAt(i,1)->data.SetGridString(tem_str);
		xPartTbl.GetGridAt(i,1)->feature=TEXTALIGN_MIDDLE_CENTER;	//正中央对齐
		//规格域
		xPartTbl.GetGridAt(i,2)->data.SetGridString(recArr[i-3].spec);
		xPartTbl.GetGridAt(i,2)->feature=TEXTALIGN_LEFT_CENTER;	//左中央对齐
		//长度域
		sprintf(tem_str,"%.0f",recArr[i-3].length);	
		xPartTbl.GetGridAt(i,3)->data.SetGridString(tem_str);
		xPartTbl.GetGridAt(i,3)->feature=TEXTALIGN_RIGHT_CENTER;
		//数量域
		sprintf(tem_str,"%d",recArr[i-3].num);
		xPartTbl.GetGridAt(i,4)->data.SetGridString(tem_str);
		xPartTbl.GetGridAt(i,4)->feature=TEXTALIGN_MIDDLE_CENTER;	//正中央对齐
		//重量之一件域
		sprintf(tem_str,"%.2f",recArr[i-3].piece_weight);
		xPartTbl.GetGridAt(i,5)->data.SetGridString(tem_str);
		xPartTbl.GetGridAt(i,5)->feature=TEXTALIGN_RIGHT_CENTER;
		//重量之小计域
		sprintf(tem_str,"%.1f",recArr[i-3].num*recArr[i-3].piece_weight);
		xPartTbl.GetGridAt(i,6)->data.SetGridString(tem_str);
		xPartTbl.GetGridAt(i,6)->feature=TEXTALIGN_RIGHT_CENTER;
		//备注域
		xPartTbl.GetGridAt(i,7)->data.SetGridString(recArr[i-3].sNotes);
		xPartTbl.GetGridAt(i,7)->feature=TEXTALIGN_LEFT_CENTER;	//左中央对齐
	}
	//组合构件数量
	sprintf(tem_str,"%d",pPartGroup->m_uStatMatCoef);
	xPartTbl.GetGridAt(3,8)->data.SetGridString(tem_str);
	xPartTbl.GetGridAt(3,8)->feature=TEXTALIGN_MIDDLE_CENTER;	//左中央对齐
	//定义合计行
	xPartTbl.GetGridAt(nRowsN-1,2)->m_bHGridVirtual = TRUE;//水平合并单元格
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(nRowsN-1,1)->data.SetGridString("total");
#else
	xPartTbl.GetGridAt(nRowsN-1,1)->data.SetGridString("合      计");
#endif
	xPartTbl.GetGridAt(nRowsN-1,1)->feature = 1; //文字中央对齐
	for(j=4;j<nColumnsN-1;j++)
		xPartTbl.GetGridAt(nRowsN-1,j)->m_bHGridVirtual = TRUE;//水平合并单元格
	double weight = 0;
	for(i=0;i<matStat.GetNodeNum();i++)
		weight += TruncateDouble(recArr[i].num*recArr[i].piece_weight,1);
	sprintf(tem_str,"%.1fkg",weight);
	xPartTbl.GetGridAt(nRowsN-1,3)->data.SetGridString(tem_str);
	xPartTbl.GetGridAt(nRowsN-1,3)->feature = 1;	//文字中央对齐
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

//预先准备分段螺栓明细表数据
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
		attach_nail_rows+nPadRecN+attach_pad_rows+1;	//表行总数
	const int nColumnsN = 7;	//表列总数
	CGrid::PREFER_TEXT_SIZE=sys.dim_map.fNumericTextSize;	//初始化表格单元字体高度
	xBoltPadTbl.Create(nRowsN,nColumnsN);
	for(i=0;i<nColumnsN;i++)	//首行为标题行
		xBoltPadTbl.GetGridAt(0,i)->textsize = sys.dim_map.fHeaderTextSize;
	for(i=0;i<nColumnsN;i++)	//第二行
		xBoltPadTbl.GetGridAt(1,i)->textsize = sys.dim_map.fCommonTextSize;
	for(i=2;i<nRowsN;i++)	//第二行
	{
		xBoltPadTbl.GetGridAt(i,0)->textsize = sys.dim_map.fCommonTextSize;
		xBoltPadTbl.GetGridAt(i,1)->textsize = sys.dim_map.fCommonTextSize;
	}
	//生成大标题-------材料汇总表-----------
	xBoltPadTbl.SetRowHigh(0,15);		//标题行
	xBoltPadTbl.SetRowHigh(1,10);		//字段行

	xBoltPadTbl.SetRowHigh(nRowsN-1,10);//合计行
	xBoltPadTbl.GetGridAt(nRowsN-1,1)->m_bHGridVirtual = TRUE;//水平合并单元格
	xBoltPadTbl.GetGridAt(nRowsN-1,0)->feature = 1;	//文字中央对齐
	if(EXPORT_BY_ENGLISH)
		xBoltPadTbl.GetGridAt(nRowsN-1,0)->data.SetGridString( "Total");
	else
		xBoltPadTbl.GetGridAt(nRowsN-1,0)->data.SetGridString( "合    计");
	for(i=2;i<nRowsN-1;i++)			//数据行
		xBoltPadTbl.SetRowHigh(i,6);
	if(sys.general_set.iPartBOMStyle==0)
	{	//标准样式 主要用于TMA
		xBoltPadTbl.SetColumnWide(0,10);	//名称栏
		xBoltPadTbl.SetColumnWide(1,15);	//级别栏
		xBoltPadTbl.SetColumnWide(2,25);	//规格栏
		xBoltPadTbl.SetColumnWide(3,15);	//符号栏
		xBoltPadTbl.SetColumnWide(4,10);	//数量栏
		xBoltPadTbl.SetColumnWide(5,15);	//重量栏
		xBoltPadTbl.SetColumnWide(6,30);	//备注栏
	}
	else
	{	//加宽样式
		xBoltPadTbl.SetColumnWide(0,10);	//名称栏
		xBoltPadTbl.SetColumnWide(1,18);	//级别栏//原为15由于构件明细表编号栏加宽6 故此处加3 重量栏加3
		xBoltPadTbl.SetColumnWide(2,30);	//规格栏//原为25,现修改为30,为保持与构件明细表宽度一致
		xBoltPadTbl.SetColumnWide(3,15);	//符号栏
		xBoltPadTbl.SetColumnWide(4,13);	//数量栏//原为10,现修改为13,为保持与构件明细表宽度一致
		xBoltPadTbl.SetColumnWide(5,23);	//重量栏//原为20,现修改为23,为保持与构件明细表宽度一致	
		xBoltPadTbl.SetColumnWide(6,30);	//备注栏
	}
	//表格中的文字缺省是右下角对齐即feature = 0
	//feature个位数表示，文字的对齐方式0:右中央 1表示正中央 2表示右下角
	//十位表示文字书写方向0:从左到右1:从上到下

	//定义标题行
	for(j=1;j<nColumnsN;j++)//水平合并第一行中的所有表格单元
		xBoltPadTbl.GetGridAt(0,j)->m_bHGridVirtual = TRUE;
	char* title_en[] = {"Type","Grade","Size","Symbol","Qty","Wt.(kg)","Notes"};
	char* title_cn[] = {"名称","级别","规格","符号","数量","重量(kg)","备注"};
	if(EXPORT_BY_ENGLISH)
	{
		xBoltPadTbl.GetGridAt(0,0)->data.SetGridString("BILL OF BOLTS AND WASHERES");
		xBoltPadTbl.GetGridAt(0,0)->feature = 1;
	}
	else
	{
		xBoltPadTbl.GetGridAt(0,0)->data.SetGridString("螺栓、脚钉、垫圈明细表");
		xBoltPadTbl.GetGridAt(0,0)->feature = 1;
	}
	for(j=0;j<nColumnsN;j++)
	{
		if(EXPORT_BY_ENGLISH)
			xBoltPadTbl.GetGridAt(1,j)->data.SetGridString(title_en[j]);
		else
			xBoltPadTbl.GetGridAt(1,j)->data.SetGridString(title_cn[j]);
		xBoltPadTbl.GetGridAt(1,j)->feature = 1;//中央对称MC
	}
	//定义--螺栓区表格单元
	int nCurRow = 2+nLsRecN+nTwoCapLsRecN+attach_ls_rows;
	if(nLsRecN+nTwoCapLsRecN>0)
	{
		for(i=3;i<2+nLsRecN+nTwoCapLsRecN+attach_ls_rows;i++)
		{
			xBoltPadTbl.GetGridAt(i,0)->m_bVGridVirtual=TRUE;//坚直合并单元格
			xBoltPadTbl.GetGridAt(i,1)->m_bVGridVirtual=TRUE;//坚直合并单元格
		}
		char sOldGrade[5]="",grade[8]="";
		for(i=2;i<2+nLsRecN+nTwoCapLsRecN;i++)
		{
			char guige[100];
			CSegMatLsRecord *pRec = xSegMatSum.GetLsRecAt(i-2);
			//if(i>=2+nLsRecN)	//双帽螺栓 wht 11-04-25
			//	pRec = xSegMatSum.GetTwoCapLsRecAt(i-2-nLsRecN);
			pRec->GetGrade(grade);
			if(strcmp(grade,sOldGrade)!=0)	//螺栓级别变换
			{
				strcpy(sOldGrade,grade);
				if(EXPORT_BY_ENGLISH)
					strcat(grade,"G");
				else
					strcat(grade,"级");
				xBoltPadTbl.GetGridAt(i,1)->data.SetGridString(grade);
				xBoltPadTbl.GetGridAt(i,1)->feature = 1;	//中央对称MC
				xBoltPadTbl.GetGridAt(i,1)->m_bVGridVirtual=FALSE;
			}
			pRec->GetGuiGe(guige,false);
			xBoltPadTbl.GetGridAt(i,2)->data.SetGridString(guige);
			xBoltPadTbl.GetGridAt(i,2)->feature = TEXTALIGN_LEFT_CENTER;
			pRec->GetGuiGe(guige);
			for(size_t j=0;j<strlen(guige);j++)
			{
				if(guige[j]=='M')	//去掉螺栓级别
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
			xBoltPadTbl.GetGridAt(2,0)->data.SetGridString("螺\\P\\P栓");
		xBoltPadTbl.GetGridAt(2,0)->feature = 1;	//中央对称MC
	}
	//定义--脚钉区表格单元
	if(nNailRecN>0)
	{
		for(i=nCurRow+1;i<nCurRow+nNailRecN+attach_nail_rows;i++)
		{
			xBoltPadTbl.GetGridAt(i,0)->m_bVGridVirtual=TRUE;//坚直合并单元格
			xBoltPadTbl.GetGridAt(i,1)->m_bVGridVirtual=TRUE;//坚直合并单元格
			xBoltPadTbl.GetGridAt(i,3)->m_bVGridVirtual=TRUE;//坚直合并单元格
		}
		char sOldGrade[5]="",grade[8]="";
		for(i=nCurRow;i<nCurRow+nNailRecN;i++)
		{
			char tem_str[100];
			CSegMatLsRecord *pRec = xSegMatSum.GetNailRecAt(i-nCurRow);
			pRec->GetGrade(grade);
			if(strcmp(grade,sOldGrade)!=0)	//螺栓级别变换
			{
				strcpy(sOldGrade,grade);
				if(EXPORT_BY_ENGLISH)
					strcat(grade,"G");
				else
					strcat(grade,"级");
				xBoltPadTbl.GetGridAt(i,1)->data.SetGridString(grade);
				xBoltPadTbl.GetGridAt(i,1)->feature = 1;	//中央对称MC
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
			xBoltPadTbl.GetGridAt(nCurRow,0)->data.SetGridString("脚\\P钉");
		//xBoltPadTbl.GetGridAt(nCurRow,1)->feature = 1;
		//xBoltPadTbl.GetGridAt(nCurRow,1)->data.SetGridString("4.8级\\P或\\P6.8级");
		nCurRow +=nNailRecN+attach_nail_rows;
	}
	//定义--垫圈区表格单元
	if(nPadRecN>0)
	{
		for(i=nCurRow+1;i<nCurRow+nPadRecN+attach_pad_rows;i++)
		{
			xBoltPadTbl.GetGridAt(i,0)->m_bVGridVirtual = TRUE;//坚直合并单元格
			xBoltPadTbl.GetGridAt(i,1)->m_bVGridVirtual = TRUE;//坚直合并单元格
			xBoltPadTbl.GetGridAt(i,3)->m_bVGridVirtual = TRUE;//坚直合并单元格
		}
		for(i=nCurRow;i<nCurRow+nPadRecN;i++)
		{
			char tem_str[100];
			CSegMatPadRecord *pRec = xSegMatSum.GetPadRecAt(i-nCurRow);
			pRec->GetGuiGe(tem_str);
			//垫圈按照绘图规范应标为-3(Φ17.5)的形式
			CLDSBolt ls;
			restore_Ls_guige(tem_str,ls);	//解析出垫圈规格中的螺栓直径及厚度数据
			int thick = (int)ls.get_L();
			int d = ls.get_d();
			if(d<=16)	//生成新规格形式
				sprintf(tem_str,"-%d(Φ%.1f)",thick,d+1.5);
			else
				sprintf(tem_str,"-%d(Φ%.0f)",thick,d+2.0);
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
			xBoltPadTbl.GetGridAt(nCurRow,0)->data.SetGridString("垫\\P圈");
		xBoltPadTbl.GetGridAt(nCurRow,1)->feature = 1;
		xBoltPadTbl.GetGridAt(nCurRow,1)->data.SetGridString("Q235");
	}
	//定义合计数据行
	for(j=3;j<7;j++)
		xBoltPadTbl.GetGridAt(nRowsN-1,j)->m_bHGridVirtual = TRUE;//水平合并
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
	xBoltPadTbl.GetGridAt(nRowsN-1,2)->feature = 1;	//文字中间对齐
}

//螺栓脚钉垫圈明细表
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