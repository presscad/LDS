//<LOCALE_TRANSLATE/>
#include "stdafx.h"
#include "stdarx.h"
#include "resource.h"
#include "Tower.h"
#include "MenuFunc.h"
#include "DrawTaMap.h"
#include "FormatTable.h"
#include "SelMapSegDlg.h"
#include "SysPara.h"
#include "JgDrawing.h"
#include "PlankDrawing.h"
#include "RxTools.h"
#include "DragEntSet.h"
#include "DefCard.h"
#include "PartGroupDrawing.h"
#include ".\LdsDrwLocale.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef __TSA_FILE_
//#ifndef ZHEJIANGSHENGDA
//	#define ZHEJIANGSHENGDA
//#endif
//进行材料汇总，并生成材料汇总表//from DrawTable.cpp
//extern void DrawTable(CFormatTbl *pTable,BOOL bPromptPos = TRUE,double scale=1,char *layer=NULL);
//返回
f2dRect CreateJgProcessCardTbl(CFlexTbl *pFlexTbl,f2dPoint insert_pos,double wide=225,double high=165,
						  BOOL bPrompt = FALSE,TBL_LOCATE_STYLE locate_style=BOTTOM_LEFT)
{
	CFormatTbl *pMainTbl,*pViceTbl;
	pMainTbl = pFlexTbl->AddEmptyFormatTbl();
	pViceTbl = pFlexTbl->AddEmptyFormatTbl();
	pMainTbl->Create(5,20);
	pViceTbl->Create(6,18);
	pMainTbl->SetBasePos(insert_pos);

	int i,j,sum_wide=0,sum_high=0;
	int row_high[5]  ={6,6,6,111,36};
	int colum_wide[20]={12,8,12,24,5,15,15,5,23,9,15,11,9,11,10,5,7,13,7,9};
	for(i=0;i<5;i++)
		sum_high+=row_high[i];
	for(j=0;j<20;j++)
		sum_wide+=colum_wide[j];
	for(i=0;i<5;i++)
		pMainTbl->SetRowHigh(i,row_high[i]*high/sum_high);
	for(i=0;i<20;i++)
		pMainTbl->SetColumnWide(i,colum_wide[i]*wide/sum_wide);
	for(i=0;i<5;i++)
		for(j=0;j<20;j++)
			pMainTbl->GetGridAt(i,j)->feature = 1;	//正中央对齐
	//pMainTbl->GetGridAt(0,2)->m_bHGridVirtual = TRUE;
	//pMainTbl->GetGridAt(0,5)->m_bHGridVirtual = TRUE;
	//pMainTbl->GetGridAt(0,7)->m_bHGridVirtual = TRUE;
	//for(j=9;j<20;j++)
	//	pMainTbl->GetGridAt(0,j)->m_bHGridVirtual = TRUE;
	for(j=2;j<20;j++)
	{
		pMainTbl->GetGridAt(3,j)->m_bHGridVirtual = TRUE;
		pMainTbl->GetGridAt(4,j)->m_bHGridVirtual = TRUE;
	}
	for(i=1;i<5;i++)
		pMainTbl->GetGridAt(i,0)->m_bVGridVirtual = TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
	pMainTbl->GetGridAt(0,1)->data.SetGridString("code");
	pMainTbl->GetGridAt(0,2)->textsize=2.5;
	pMainTbl->GetGridAt(0,3)->data.SetGridString("Project Name");
	pMainTbl->GetGridAt(0,4)->textsize=2.5;
	pMainTbl->GetGridAt(0,5)->m_bHGridVirtual = TRUE;
	pMainTbl->GetGridAt(0,6)->data.SetGridString("tower type");
	pMainTbl->GetGridAt(0,8)->textsize=2.5;
	pMainTbl->GetGridAt(0,7)->m_bHGridVirtual = TRUE;
	pMainTbl->GetGridAt(0,9)->data.SetGridString("length");
	pMainTbl->GetGridAt(0,11)->data.SetGridString("part no.");
	pMainTbl->GetGridAt(0,13)->data.SetGridString("draw no.");
#else
	pMainTbl->GetGridAt(0,1)->data.SetGridString("代号");
	pMainTbl->GetGridAt(0,2)->textsize=2.5;
	pMainTbl->GetGridAt(0,3)->data.SetGridString("工程名称");
	pMainTbl->GetGridAt(0,4)->textsize=2.5;
	pMainTbl->GetGridAt(0,5)->m_bHGridVirtual = TRUE;
	pMainTbl->GetGridAt(0,6)->data.SetGridString("塔型");
	pMainTbl->GetGridAt(0,8)->textsize=2.5;
	pMainTbl->GetGridAt(0,7)->m_bHGridVirtual = TRUE;
	pMainTbl->GetGridAt(0,9)->data.SetGridString("长度");
	pMainTbl->GetGridAt(0,11)->data.SetGridString("件号");
	pMainTbl->GetGridAt(0,13)->data.SetGridString("图号");
#endif
	pMainTbl->GetGridAt(0,14)->textsize=2.5;
	pMainTbl->GetGridAt(0,15)->m_bHGridVirtual = TRUE;
	pMainTbl->GetGridAt(0,16)->m_bHGridVirtual = TRUE;
	pMainTbl->GetGridAt(0,17)->m_bHGridVirtual = TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
	pMainTbl->GetGridAt(0,18)->data.SetGridString("base");
	pMainTbl->GetGridAt(1,9)->data.SetGridString("remove root");
	pMainTbl->GetGridAt(2,9)->data.SetGridString("back off");
	pMainTbl->GetGridAt(1,1)->data.SetGridString("\\Process");
	pMainTbl->GetGridAt(2,1)->m_bVGridVirtual = TRUE;
	pMainTbl->GetGridAt(1,2)->data.SetGridString("open-close");
	pMainTbl->GetGridAt(1,4)->data.SetGridString("\\P cut angle");
	pMainTbl->GetGridAt(2,4)->m_bVGridVirtual = TRUE;
	pMainTbl->GetGridAt(1,7)->data.SetGridString("bending\\P");
	pMainTbl->GetGridAt(2,7)->m_bVGridVirtual = TRUE;
	pMainTbl->GetGridAt(1,11)->data.SetGridString("single number");
	pMainTbl->GetGridAt(2,11)->data.SetGridString("piece weight");
	pMainTbl->GetGridAt(1,13)->data.SetGridString("process number");
	pMainTbl->GetGridAt(2,13)->data.SetGridString("total weight");
	pMainTbl->GetGridAt(1,15)->data.SetGridString("\\P specification");
	pMainTbl->GetGridAt(2,15)->m_bVGridVirtual = TRUE;
	pMainTbl->GetGridAt(1,16)->data.SetGridString("lend");
	pMainTbl->GetGridAt(2,16)->data.SetGridString("design");
	pMainTbl->GetGridAt(1,18)->data.SetGridString("\\P steel material");
#else
	pMainTbl->GetGridAt(0,18)->data.SetGridString("基数");
	pMainTbl->GetGridAt(1,9)->data.SetGridString("刨 根");
	pMainTbl->GetGridAt(2,9)->data.SetGridString("铲 背");
	pMainTbl->GetGridAt(1,1)->data.SetGridString("工\\P艺");
	pMainTbl->GetGridAt(2,1)->m_bVGridVirtual = TRUE;
	pMainTbl->GetGridAt(1,2)->data.SetGridString("开合角");
	pMainTbl->GetGridAt(1,4)->data.SetGridString("切\\P角");
	pMainTbl->GetGridAt(2,4)->m_bVGridVirtual = TRUE;
	pMainTbl->GetGridAt(1,7)->data.SetGridString("制\\P弯");
	pMainTbl->GetGridAt(2,7)->m_bVGridVirtual = TRUE;
	pMainTbl->GetGridAt(1,11)->data.SetGridString("单基数");
	pMainTbl->GetGridAt(2,11)->data.SetGridString("单重");
	pMainTbl->GetGridAt(1,13)->data.SetGridString("加工数");
	pMainTbl->GetGridAt(2,13)->data.SetGridString("总重");
	pMainTbl->GetGridAt(1,15)->data.SetGridString("规\\P格");
	pMainTbl->GetGridAt(2,15)->m_bVGridVirtual = TRUE;
	pMainTbl->GetGridAt(1,16)->data.SetGridString("借用");
	pMainTbl->GetGridAt(2,16)->data.SetGridString("设计");
	pMainTbl->GetGridAt(1,18)->data.SetGridString("材\\P质");
#endif
	pMainTbl->GetGridAt(2,18)->m_bVGridVirtual = TRUE;
	//pMainTbl->GetGridAt(0,0)->data.SetGridString("河\\P北\\P省\\P送\\P变\\P电\\P公\\P司\\P线\\P路\\P器\\P材\\P厂\\P铁\\P塔\\P加\\P工\\P工\\P艺\\P卡");
	//pMainTbl->GetGridAt(0,0)->data.SetGridString("四\\P川\\P电\\P力\\P送\\P变\\P电\\P铁\\P塔\\P厂\\P铁\\P塔\\P加\\P工\\P工\\P艺\\P卡");
#ifdef AFX_TARG_ENU_ENGLISH
	pMainTbl->GetGridAt(0,0)->data.SetGridString("Henan \\P\\P anyang \\P\\P electric \\P\\P rod \\P tower \\P\\P limited \\P\\P company \\P\\P tower \\P\\P process \\P\\P card");
#else
	pMainTbl->GetGridAt(0,0)->data.SetGridString("河\\P南\\P安\\P阳\\P电\\P业\\P杆\\P塔\\P有\\P限\\P责\\P任\\P公\\P司\\P铁\\P塔\\P加\\P工\\P工\\P艺\\P卡");
#endif
	//pMainTbl->GetGridAt(0,0)->data.SetGridString("锦\\P州\\P电\\P业\\P铁\\P塔\\P制\\P造\\P有\\P限\\P公\\P司\\P铁\\P塔\\P加\\P工\\P工\\P艺\\P卡");
	//pMainTbl->GetGridAt(0,0)->data.SetGridString("成\\P都\\P铁\\P塔\\P厂\\P铁\\P塔\\P加\\P工\\P工\\P艺\\P卡");
	//pMainTbl->GetGridAt(0,0)->data.SetGridString("霸\\P州\\P市\\P华\\P兴\\P钢\\P杆\\P有\\P限\\P公\\P司\\P铁\\P塔\\P加\\P工\\P工\\P艺\\P卡");
	pMainTbl->GetGridAt(0,0)->textsize = sys.dim_map.fCommonTextSize;

	//DrawTable(pBlockTableRecord,pMainTbl,bPrompt);
	for(i=0;i<6;i++)
		pViceTbl->SetRowHigh(i,6*high/sum_high);
	double vice_col_wide[18] = {13,12,15,11.5,11.5,11.5,11.5,11.5,11.5,11.5,11.5,11.5,11.5,11.5,11.5,11.5,9.5,14};
	for(j=0;j<18;j++)
		pViceTbl->SetColumnWide(j,vice_col_wide[j]*wide/sum_wide);
	for(i=0;i<6;i++)
		for(j=0;j<18;j++)
		pViceTbl->GetGridAt(i,j)->feature = 1;	//文字中央对齐
	for(j=4;j<18;j++)
		pViceTbl->GetGridAt(5,j)->m_bHGridVirtual = TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
	pViceTbl->GetGridAt(0,2)->data.SetGridString("process");
	pViceTbl->GetGridAt(0,3)->data.SetGridString("drawing");
	pViceTbl->GetGridAt(0,4)->data.SetGridString("blanking");
	pViceTbl->GetGridAt(0,5)->data.SetGridString("press No.");
	pViceTbl->GetGridAt(0,6)->data.SetGridString("draw line");
	pViceTbl->GetGridAt(0,7)->data.SetGridString("drill hole");
	pViceTbl->GetGridAt(0,8)->data.SetGridString("cut angle");
	pViceTbl->GetGridAt(0,9)->data.SetGridString("assembly line");
	pViceTbl->GetGridAt(0,10)->data.SetGridString("bending");
	pViceTbl->GetGridAt(0,11)->data.SetGridString("open-close angle");
	pViceTbl->GetGridAt(0,12)->data.SetGridString("peeling-off");
	pViceTbl->GetGridAt(0,13)->data.SetGridString("flat");
	pViceTbl->GetGridAt(0,14)->data.SetGridString("weld");
	pViceTbl->GetGridAt(0,15)->data.SetGridString("straightening");
	pViceTbl->GetGridAt(0,16)->data.SetGridString("checking");
	pViceTbl->GetGridAt(0,17)->data.SetGridString("approve");
#else
	pViceTbl->GetGridAt(0,2)->data.SetGridString("工序");
	pViceTbl->GetGridAt(0,3)->data.SetGridString("制表");
	pViceTbl->GetGridAt(0,4)->data.SetGridString("下料");
	pViceTbl->GetGridAt(0,5)->data.SetGridString("压号");
	pViceTbl->GetGridAt(0,6)->data.SetGridString("划线");
	pViceTbl->GetGridAt(0,7)->data.SetGridString("制孔");
	pViceTbl->GetGridAt(0,8)->data.SetGridString("切角");
	pViceTbl->GetGridAt(0,9)->data.SetGridString("流水线");
	pViceTbl->GetGridAt(0,10)->data.SetGridString("制弯");
	pViceTbl->GetGridAt(0,11)->data.SetGridString("开合角");
	pViceTbl->GetGridAt(0,12)->data.SetGridString("清铲");
	pViceTbl->GetGridAt(0,13)->data.SetGridString("打扁");
	pViceTbl->GetGridAt(0,14)->data.SetGridString("组焊");
	pViceTbl->GetGridAt(0,15)->data.SetGridString("调直");
	pViceTbl->GetGridAt(0,16)->data.SetGridString("校 核");
	pViceTbl->GetGridAt(0,17)->data.SetGridString("批 准");
#endif
	pViceTbl->GetGridAt(1,0)->data.SetGridString("M12_HOLE");
	pViceTbl->GetGridAt(2,0)->data.SetGridString("M16_HOLE");
	pViceTbl->GetGridAt(3,0)->data.SetGridString("M20_HOLE");
	pViceTbl->GetGridAt(4,0)->data.SetGridString("M24_HOLE");
#ifdef AFX_TARG_ENU_ENGLISH
	pViceTbl->GetGridAt(5,0)->data.SetGridString("total");
#else
	pViceTbl->GetGridAt(5,0)->data.SetGridString("总 计");
#endif
	pViceTbl->GetGridAt(1,0)->grid_mode = PIC_BLOCK_GRID;
	pViceTbl->GetGridAt(2,0)->grid_mode = PIC_BLOCK_GRID;
	pViceTbl->GetGridAt(3,0)->grid_mode = PIC_BLOCK_GRID;
	pViceTbl->GetGridAt(4,0)->grid_mode = PIC_BLOCK_GRID;

#ifdef AFX_TARG_ENU_ENGLISH
	pViceTbl->GetGridAt(1,2)->data.SetGridString("operator");
	pViceTbl->GetGridAt(2,2)->data.SetGridString("date");
	pViceTbl->GetGridAt(3,2)->data.SetGridString("tester");
	pViceTbl->GetGridAt(4,2)->data.SetGridString("date");
	pViceTbl->GetGridAt(5,2)->data.SetGridString("notes");
	pViceTbl->GetGridAt(0,5)->data.SetGridString("holes number");
#else
	pViceTbl->GetGridAt(1,2)->data.SetGridString("操作者");
	pViceTbl->GetGridAt(2,2)->data.SetGridString("日 期");
	pViceTbl->GetGridAt(3,2)->data.SetGridString("检验者");
	pViceTbl->GetGridAt(4,2)->data.SetGridString("日 期");
	pViceTbl->GetGridAt(5,2)->data.SetGridString("备 注");
	pViceTbl->GetGridAt(0,5)->data.SetGridString("孔数和");
#endif

	f2dPoint base_pos;
	pMainTbl->GetRowLineEnd(4,base_pos);
	pViceTbl->SetBasePos(base_pos);
	pViceTbl->SetLocateStyle(TOP_RIGHT);
	f2dRect rect;
	pMainTbl->GetGridRect(3,1,rect);
	return rect;
}
f2dRect CreatePlankProcessCardTbl(CFlexTbl *pFlexTbl,f2dPoint insert_pos,double wide=200,double high=126,
							 BOOL bPrompt = FALSE,TBL_LOCATE_STYLE locate_style=BOTTOM_LEFT)
{
	CFormatTbl *pMainTbl,*pViceTbl;
	pMainTbl = pFlexTbl->AddEmptyFormatTbl();
	pViceTbl = pFlexTbl->AddEmptyFormatTbl();
	pMainTbl->Create(13,16);
	pViceTbl->Create(4,7);
	pMainTbl->SetBasePos(insert_pos);

	int i,j,sum_wide=0,sum_high=0;
	int row_high[13]  ={6,6,6,6,6,6,6,6,6,6,36,24,36};
	int colum_wide[16]={10,16,16,24,10,20,10,16,10,10,12,12,12,12,12,10};
	for(i=0;i<13;i++)
		sum_high+=row_high[i];
	for(j=0;j<16;j++)
		sum_wide+=colum_wide[j];

	for(i=0;i<13;i++)
		pMainTbl->SetRowHigh(i,row_high[i]*high/sum_high);
	for(j=0;j<16;j++)
		pMainTbl->SetColumnWide(j,colum_wide[j]*wide/sum_wide);
	for(i=0;i<13;i++)
		for(j=0;j<16;j++)
			pMainTbl->GetGridAt(i,j)->feature = 1;	//正中央对齐
#ifdef AFX_TARG_ENU_ENGLISH
	pMainTbl->GetGridAt(0,0)->data.SetGridString("code");
	pMainTbl->GetGridAt(0,2)->data.SetGridString("Project Name");
	pMainTbl->GetGridAt(0,3)->textsize=2.5;
	pMainTbl->GetGridAt(0,4)->data.SetGridString("tower type");
	pMainTbl->GetGridAt(0,5)->textsize=2.5;
	pMainTbl->GetGridAt(0,6)->data.SetGridString("scale");
	pMainTbl->GetGridAt(0,8)->data.SetGridString("Part No.");
	pMainTbl->GetGridAt(0,10)->m_bHGridVirtual = TRUE;
	pMainTbl->GetGridAt(0,11)->data.SetGridString("draw No.");
	pMainTbl->GetGridAt(0,12)->textsize=2.5;
	pMainTbl->GetGridAt(0,13)->m_bHGridVirtual = TRUE;
	pMainTbl->GetGridAt(0,14)->data.SetGridString("base");
#else
	pMainTbl->GetGridAt(0,0)->data.SetGridString("代号");
	pMainTbl->GetGridAt(0,2)->data.SetGridString("工程名称");
	pMainTbl->GetGridAt(0,3)->textsize=2.5;
	pMainTbl->GetGridAt(0,4)->data.SetGridString("塔型");
	pMainTbl->GetGridAt(0,5)->textsize=2.5;
	pMainTbl->GetGridAt(0,6)->data.SetGridString("比例");
	pMainTbl->GetGridAt(0,8)->data.SetGridString("件号");
	pMainTbl->GetGridAt(0,10)->m_bHGridVirtual = TRUE;
	pMainTbl->GetGridAt(0,11)->data.SetGridString("图号");
	pMainTbl->GetGridAt(0,12)->textsize=2.5;
	pMainTbl->GetGridAt(0,13)->m_bHGridVirtual = TRUE;
	pMainTbl->GetGridAt(0,14)->data.SetGridString("基数");
#endif
	pMainTbl->GetGridAt(1,14)->data.SetGridString("M12_HOLE");
	pMainTbl->GetGridAt(2,14)->data.SetGridString("M16_HOLE");
	pMainTbl->GetGridAt(3,14)->data.SetGridString("M20_HOLE");
	pMainTbl->GetGridAt(4,14)->data.SetGridString("M24_HOLE");
	pMainTbl->GetGridAt(1,14)->grid_mode = PIC_BLOCK_GRID;
	pMainTbl->GetGridAt(2,14)->grid_mode = PIC_BLOCK_GRID;
	pMainTbl->GetGridAt(3,14)->grid_mode = PIC_BLOCK_GRID;
	pMainTbl->GetGridAt(4,14)->grid_mode = PIC_BLOCK_GRID;
#ifdef AFX_TARG_ENU_ENGLISH
	pMainTbl->GetGridAt(1,0)->data.SetGridString("\\Pbending");
	pMainTbl->GetGridAt(1,9)->data.SetGridString("process");
	pMainTbl->GetGridAt(2,9)->data.SetGridString("cut");
	pMainTbl->GetGridAt(3,9)->data.SetGridString("gas cutting");
	pMainTbl->GetGridAt(4,9)->data.SetGridString("press No.");
	pMainTbl->GetGridAt(5,9)->data.SetGridString("drill hole");
	pMainTbl->GetGridAt(6,9)->data.SetGridString("bending");
	pMainTbl->GetGridAt(7,9)->data.SetGridString("chamfering");
	pMainTbl->GetGridAt(8,9)->data.SetGridString("weld");
	pMainTbl->GetGridAt(1,10)->data.SetGridString("operator");
	pMainTbl->GetGridAt(1,11)->data.SetGridString("date");
	pMainTbl->GetGridAt(1,12)->data.SetGridString("tester");
	pMainTbl->GetGridAt(1,13)->data.SetGridString("date");
	pMainTbl->GetGridAt(9,14)->data.SetGridString("hole number");
	pMainTbl->GetGridAt(10,9)->data.SetGridString("notes");
#else
	pMainTbl->GetGridAt(1,0)->data.SetGridString("火\\P曲");
	pMainTbl->GetGridAt(1,9)->data.SetGridString("工序");
	pMainTbl->GetGridAt(2,9)->data.SetGridString("剪切");
	pMainTbl->GetGridAt(3,9)->data.SetGridString("气割");
	pMainTbl->GetGridAt(4,9)->data.SetGridString("压号");
	pMainTbl->GetGridAt(5,9)->data.SetGridString("制孔");
	pMainTbl->GetGridAt(6,9)->data.SetGridString("制弯");
	pMainTbl->GetGridAt(7,9)->data.SetGridString("刨边");
	pMainTbl->GetGridAt(8,9)->data.SetGridString("组焊");
	pMainTbl->GetGridAt(1,10)->data.SetGridString("操作者");
	pMainTbl->GetGridAt(1,11)->data.SetGridString("日期");
	pMainTbl->GetGridAt(1,12)->data.SetGridString("检验者");
	pMainTbl->GetGridAt(1,13)->data.SetGridString("日期");
	pMainTbl->GetGridAt(9,14)->data.SetGridString("孔数和");
	pMainTbl->GetGridAt(10,9)->data.SetGridString("备注:");
#endif
	pMainTbl->GetGridAt(10,9)->feature = 3;
	//pMainTbl->GetGridAt(12,9)->data.SetGridString("河北省送变电公司线路器材厂\\P铁塔加工工艺卡");
	//pMainTbl->GetGridAt(12,9)->data.SetGridString("四川电力送变电铁塔厂\\P铁塔加工工艺卡");
#ifdef AFX_TARG_ENU_ENGLISH
	pMainTbl->GetGridAt(12,9)->data.SetGridString("Henan Anyang Electric Rod Tower Limited Company\\P tower processing card");
#else
	pMainTbl->GetGridAt(12,9)->data.SetGridString("河南安阳电业杆塔有限责任公司\\P铁塔加工工艺卡");
#endif
	//pMainTbl->GetGridAt(12,9)->data.SetGridString("锦州电业铁塔制造有限公司\\P铁塔加工工艺卡");
	//pMainTbl->GetGridAt(12,9)->data.SetGridString("成都铁塔厂\\P铁塔加工工艺卡");
	//pMainTbl->GetGridAt(12,9)->data.SetGridString("霸州市华兴钢杆有限公司\\P铁塔加工工艺卡");
	//pMainTbl->GetGridAt(12,9)->textsize = sys.dim_map.fHeaderTextSize;
	pMainTbl->GetGridAt(12,9)->textsize=6.0;

	for(i=3;i<9;i++)
	{
		pMainTbl->GetGridAt(1,i)->m_bHGridVirtual = TRUE;
		pMainTbl->GetGridAt(2,i)->m_bHGridVirtual = TRUE;
		pMainTbl->GetGridAt(2,i)->m_bVGridVirtual = TRUE;
	}
	pMainTbl->GetGridAt(2,2)->m_bVGridVirtual = TRUE;
	for(i=1;i<9;i++)
	{
		if(i>1)
			pMainTbl->GetGridAt(3,i)->m_bVGridVirtual = TRUE;
		pMainTbl->GetGridAt(3,i)->m_bHGridVirtual = TRUE;
	}
	for(i=4;i<13;i++)
	{
		for(j=0;j<9;j++)
		{
			pMainTbl->GetGridAt(i,j)->m_bHGridVirtual = TRUE;
			pMainTbl->GetGridAt(i,j)->m_bVGridVirtual = TRUE;
		}
		if(i>9)
		{
			for(j=10;j<16;j++)
				pMainTbl->GetGridAt(i,j)->m_bHGridVirtual = TRUE;
		}
	}
	pMainTbl->GetGridAt(2,0)->m_bVGridVirtual = TRUE;

	//DrawTable(pBlockTableRecord,pMainTbl,bPrompt);
	for(i=0;i<4;i++)
		pViceTbl->SetRowHigh(i,6*high/sum_high);
	int vice_col_wide[7] = {15,11,8,6,15,10,15};
	for(j=0;j<7;j++)
		pViceTbl->SetColumnWide(j,vice_col_wide[j]*wide/sum_wide);
	for(i=0;i<4;i++)
		for(j=0;j<7;j++)
			pViceTbl->GetGridAt(i,j)->feature = 1;	//文字中央对齐
	pViceTbl->GetGridAt(0,3)->m_bHGridVirtual = TRUE;
	pViceTbl->GetGridAt(1,2)->m_bHGridVirtual = TRUE;
	pViceTbl->GetGridAt(2,2)->m_bHGridVirtual = TRUE;
	pViceTbl->GetGridAt(3,2)->m_bHGridVirtual = TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
	pViceTbl->GetGridAt(0,0)->data.SetGridString("single number");
	pViceTbl->GetGridAt(1,0)->data.SetGridString("piece weight");
	pViceTbl->GetGridAt(2,0)->data.SetGridString("alternative spec");
	pViceTbl->GetGridAt(2,1)->textsize=2.5;
	pViceTbl->GetGridAt(3,0)->data.SetGridString("design spec");
	pViceTbl->GetGridAt(3,1)->textsize=2.5;
	pViceTbl->GetGridAt(0,2)->data.SetGridString("processing number");
	pViceTbl->GetGridAt(1,3)->data.SetGridString("total weight");
	pViceTbl->GetGridAt(2,3)->data.SetGridString("steel material");
	pViceTbl->GetGridAt(3,3)->m_bVGridVirtual = TRUE;
	pViceTbl->GetGridAt(0,5)->data.SetGridString("lister");
	pViceTbl->GetGridAt(1,5)->data.SetGridString("check");
	pViceTbl->GetGridAt(2,5)->data.SetGridString("approved");
	pViceTbl->GetGridAt(3,5)->data.SetGridString("date");
#else
	pViceTbl->GetGridAt(0,0)->data.SetGridString("单基数");
	pViceTbl->GetGridAt(1,0)->data.SetGridString("单重");
	pViceTbl->GetGridAt(2,0)->data.SetGridString("代用规格");
	pViceTbl->GetGridAt(2,1)->textsize=2.5;
	pViceTbl->GetGridAt(3,0)->data.SetGridString("设计规格");
	pViceTbl->GetGridAt(3,1)->textsize=2.5;
	pViceTbl->GetGridAt(0,2)->data.SetGridString("加工数");
	pViceTbl->GetGridAt(1,3)->data.SetGridString("总重");
	pViceTbl->GetGridAt(2,3)->data.SetGridString("材\\P质");
	pViceTbl->GetGridAt(3,3)->m_bVGridVirtual = TRUE;
	pViceTbl->GetGridAt(0,5)->data.SetGridString("制表人");
	pViceTbl->GetGridAt(1,5)->data.SetGridString("校核");
	pViceTbl->GetGridAt(2,5)->data.SetGridString("批准");
	pViceTbl->GetGridAt(3,5)->data.SetGridString("日期");
#endif
	f2dPoint base_pos;
	pMainTbl->GetRowLineEnd(11,base_pos);
	pViceTbl->SetBasePos(base_pos);
	pViceTbl->SetLocateStyle(TOP_RIGHT);
	//DrawTable(pBlockTableRecord,pViceTbl,FALSE);
	f2dRect rect;
	pMainTbl->GetGridRect(3,0,rect);
	return rect;
}
//正常的工艺卡中不允许存在块(块名与自定义块重名)
BOOL IsValidProcessCard(AcDbDatabase* pBlkDb,const char* sCardName)
{
	AcDbBlockTable *pTempBlockTable;
	pBlkDb->getBlockTable(pTempBlockTable,AcDb::kForRead);
	AcDbBlockTableRecord *pTempBlockTableRecord;//定义块表记录指针
	pTempBlockTable->getAt(ACDB_MODEL_SPACE,pTempBlockTableRecord,AcDb::kForWrite);
	pTempBlockTable->close();//关闭块表
	AcDbBlockTableRecordIterator *pIterator=NULL;
	pTempBlockTableRecord->newIterator( pIterator);
	BOOL bValid=TRUE;
	if(pIterator)
	{
		AcDbEntity *pEnt=NULL;
		for(;!pIterator->done();pIterator->step())
		{
			pIterator->getEntity(pEnt,AcDb::kForRead);
			if(pEnt)
				pEnt->close();
			if(pEnt==NULL || !pEnt->isKindOf(AcDbBlockReference::desc()))
				continue;
			//工艺卡DWG文件中存在块
			AcDbBlockReference* pReference=(AcDbBlockReference*)pEnt;
			AcDbObjectId blockId=pReference->blockTableRecord();
			AcDbBlockTableRecord *pTempBlockTableRecord=NULL;
			acdbOpenObject(pTempBlockTableRecord,blockId,AcDb::kForRead);
			pTempBlockTableRecord->close();
			CXhChar50 sName;
#ifdef _ARX_2007
			ACHAR* sValue=new ACHAR[50];
			pTempBlockTableRecord->getName(sValue);
			sName.Copy((char*)_bstr_t(sValue));
			delete[] sValue;
#else
			char *sValue=new char[50];
			pTempBlockTableRecord->getName(sValue);
			sName.Copy(sValue);
			delete[] sValue;
#endif
			if(sName.Equal(sCardName))
			{
				bValid=FALSE;
				break;
			}
		}
	}
	pTempBlockTableRecord->close();
	return bValid;
}
//根据用户设置获取实时标记符
CXhChar16 GetMarkSymbol()
{
	CXhChar16 sMarkSymbol;
	LOCALE_PROPITEM* pLocaleItem=gxDrwLocalizer.GetLocaleItem("ProcessCard.HasSymbol");
	if(pLocaleItem)//sys.customize.ciCustomerSerial==5)	//青岛东方定制
		sMarkSymbol.Copy(pLocaleItem->szStrBuff);//"○");
	else if(sys.part_map.iPartMapStyle==1)	//浙江盛达铁塔有限公司
		sMarkSymbol.Copy("V");
	else
		sMarkSymbol.Copy("√");
	return sMarkSymbol;
}
//工艺卡数据格标注类型：0.当行文本 1.多行文本 2.图符图块
void DimCardTblGridData(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint datumPos,GRID_DATA_STRU &gridData,const char* sData,BYTE cDimType=0,double fWidth=0)
{
	if(strlen(sData)<=0||pBlockTableRecord==NULL)
		return;
	double ftextH=gridData.fTextHigh;
	AcDb::TextHorzMode hMode=AcDb::kTextCenter;
	AcDb::TextVertMode vMode=AcDb::kTextVertMid;
	f3dPoint dimPos=f3dPoint((gridData.max_x+gridData.min_x)/2.0,(gridData.max_y+gridData.min_y)/2.0,0)+datumPos;
	if(gridData.align_type>0&&gridData.align_type<10)
	{	//对齐方式为1-9
		AcDb::TextHorzMode hModeArr[3]={AcDb::kTextRight,AcDb::kTextLeft,AcDb::kTextCenter};
		AcDb::TextVertMode vModeArr[3]={AcDb::kTextTop,AcDb::kTextVertMid,AcDb::kTextBottom};
		int iHIndex=gridData.align_type%3;
		int iVIndex=0;
		if(gridData.align_type==4||gridData.align_type==5||gridData.align_type==6)
			iVIndex=1;
		else if(gridData.align_type==7||gridData.align_type==8||gridData.align_type==9)
			iVIndex=2;
		hMode=hModeArr[iHIndex];
		vMode=vModeArr[iVIndex];
		if(hMode==AcDb::kTextRight)
			dimPos=f3dPoint(gridData.max_x,(gridData.max_y+gridData.min_y)/2.0,0)+datumPos;
		else if(hMode==AcDb::kTextLeft)
			dimPos=f3dPoint(gridData.min_x,(gridData.max_y+gridData.min_y)/2.0,0)+datumPos;
	}
	else
		gridData.align_type=4;
	CXhChar100 sText;
	if(gridData.sContentPrefix)
		sText.Append(gridData.sContentPrefix);
	if(sData)
		sText.Append(sData);
	if(gridData.sContentSuffix)
		sText.Append(gridData.sContentSuffix);
	if(cDimType==0)
	{	//单行文本标注
		if(stricmp(sData,"○")==0)	//青岛东方实时标记需要绘制圆圈
			CreateAcadCircle(pBlockTableRecord,dimPos,2);
		else
			DimText(pBlockTableRecord,dimPos,sText,TextStyleTable::hzfs.textStyleId,ftextH,0,hMode,vMode);
	}
	else if(cDimType==1)
	{	//多行文本标注
		if(stricmp(sData,"○")==0)	//青岛东方实时标记需要绘制圆圈
			CreateAcadCircle(pBlockTableRecord,dimPos,2);
		else
			DimMText(pBlockTableRecord,(char*)sText,dimPos,fWidth,ftextH,TextStyleTable::hzfs.textStyleId,(AcDbMText::AttachmentPoint)gridData.align_type,AcDbMText::kLtoR,0);
	}
	else if(cDimType==2)	
	{	//插入指定图块
		CXhChar16 sBlkName(sData);
		InsertBlock(pBlockTableRecord,dimPos,sBlkName,1.0,0,0);
	}
}
f2dRect InsertSpecialProcessStatCardTbl(AcDbBlockTableRecord *pBlockTableRecord,f2dPoint insert_pos,SPECIAL_PROCESS_STAT_CARD_INFO *pProcessInfo)
{
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	AcDbDatabase blkDb(Adesk::kFalse);//定义空的数据库
	AcDbObjectId newEntId,blockId;
	f2dRect draw_rect;
	f3dPoint origin_pos(insert_pos.x,insert_pos.y);
	draw_rect.bottomRight.Set(236,110);
	AcDbBlockReference *pBlkRef = new AcDbBlockReference;
	char filename[MAX_PATH];
	sprintf(filename,"%s",sys.general_set.m_sProcessPartStatCard);
#ifdef _ARX_2007
	if(blkDb.readDwgFile((ACHAR*)_bstr_t(filename),_SH_DENYRW,true)==Acad::eOk&&IsValidProcessCard(&blkDb,"SpecialProcessCard"))
#else
	if(blkDb.readDwgFile(filename,_SH_DENYRW,true)==Acad::eOk&&IsValidProcessCard(&blkDb,"SpecialProcessCard"))
#endif
	{
		AcDbEntity *pEnt;
		AcDbBlockTable *pTempBlockTable;
		blkDb.getBlockTable(pTempBlockTable,AcDb::kForRead);
		//获得当前图形块表记录指针
		AcDbBlockTableRecord *pTempBlockTableRecord;//定义块表记录指针
		//以写方式打开模型空间，获得块表记录指针
		pTempBlockTable->getAt(ACDB_MODEL_SPACE,pTempBlockTableRecord,AcDb::kForWrite);
		pTempBlockTable->close();//关闭块表
		AcDbBlockTableRecordIterator *pIterator=NULL;
		pTempBlockTableRecord->newIterator( pIterator);
		if(pIterator)
		{
			for(;!pIterator->done();pIterator->step())
			{
				GRID_DATA_STRU grid_data;
				pIterator->getEntity(pEnt,AcDb::kForWrite);
				if(pEnt->isKindOf(AcDbPoint::desc()))
				{
					if(!GetGridKey((AcDbPoint*)pEnt,&grid_data))
					{
						pEnt->close();
						continue;
					}
					if(grid_data.data_type==1||grid_data.data_type==2)	//实时文本||实时数据
					{
						double fWidth=grid_data.max_x-grid_data.min_x;
						if(grid_data.type_id==ITEM_TYPE_CODE_NO)		//代号
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sTaAlias,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_TA_TYPE)	//塔型
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sTaType,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_TA_NUM)	//基数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,CXhChar16("%d",Ta.m_nTaNum),1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PRJ_NAME)	//工程名称
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sPrjName,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_TAB_MAKER)	//制表人
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sOperator,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CRITIC)	//评审人
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sCritic,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_AUDITOR)	//审核人
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sAuditor,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PROCESS_NAME)	//工艺名称
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pProcessInfo->sProcessName,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_DATE)	//日期
						{
							CTime t=CTime::GetCurrentTime();
							CString s=t.Format("%Y.%m.%d");
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,s,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PAGENUM)	//共 页
						{
							char ss[20];
#ifdef AFX_TARG_ENU_ENGLISH
							sprintf(ss,"page %4d total",pProcessInfo->m_nPageNum);
#else
							sprintf(ss,"共%4d页",pProcessInfo->m_nPageNum);
#endif
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PAGEINDEX)	//第 页
						{
							char ss[20];
#ifdef AFX_TARG_ENU_ENGLISH
							sprintf(ss,"page %4d total",pProcessInfo->m_nPageIndex);
#else
							sprintf(ss,"第%4d页",pProcessInfo->m_nPageIndex);
#endif
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_SKETCH_MAP)	//插入示意图
						{	//暂时在此处调整示意图比例 
							double scale=1.0; //示意图比例
							if(strcmp("Slot-U",pProcessInfo->sSketchMap)==0
#ifdef AFX_TARG_ENU_ENGLISH
								||strcmp("Slot-Cross",pProcessInfo->sSketchMap)==0
								||strcmp("SlotU Cross",pProcessInfo->sSketchMap)==0)
#else
								||strcmp("Slot-十",pProcessInfo->sSketchMap)==0
								||strcmp("SlotU十",pProcessInfo->sSketchMap)==0)
#endif
								scale=0.5;	
							if(strlen(pProcessInfo->sSketchMap)>0)
								DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pProcessInfo->sSketchMap,2);
							pEnt->erase();
							pEnt->close();
							continue;
						}
					}
					else if(grid_data.data_type==3)	//草图区域
					{
						draw_rect.topLeft.Set(grid_data.min_x,grid_data.max_y);
						draw_rect.bottomRight.Set(grid_data.max_x,grid_data.min_y);
					}
				}
				else if(pEnt->isKindOf(AcDbMText::desc()))
				{
					pEnt->close();
					continue;
				}
				else if(pEnt->isKindOf(AcDbLine::desc()))
				{
					pEnt->close();
					continue;
				}

				pEnt->close();
			}
		}
		pTempBlockTableRecord->close();

		AcDbDatabase *pTempDb;
		blkDb.wblock(pTempDb);
#ifdef _ARX_2007
		GetCurDwg()->insert(blockId,L"SpecialProcessCard",pTempDb);
#else
		GetCurDwg()->insert(blockId,"SpecialProcessCard",pTempDb);
#endif
		delete pTempDb;
	}
	else
	{
		pBlkRef->close();
		draw_rect.topLeft.x+=insert_pos.x;
		draw_rect.topLeft.y+=insert_pos.y;
		draw_rect.bottomRight.x+=insert_pos.x;
		draw_rect.bottomRight.y+=insert_pos.y;
		return draw_rect;
	}

	AcGeScale3d scaleXYZ(1.0,1.0,1.0);
	pBlkRef->setBlockTableRecord(blockId);
	pBlkRef->setPosition(AcGePoint3d(insert_pos.x,insert_pos.y,0));
	pBlkRef->setRotation(0);
	pBlkRef->setScaleFactors(scaleXYZ);
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,blockId,pBlkRef))
		pBlkRef->close();
	draw_rect.topLeft.x+=insert_pos.x;
	draw_rect.topLeft.y+=insert_pos.y;
	draw_rect.bottomRight.x+=insert_pos.x;
	draw_rect.bottomRight.y+=insert_pos.y;
	return draw_rect;
}

//0.角钢工艺卡 1.组焊图工艺卡 2.钢板工艺卡 3.钢管工艺卡 4.工艺件统计卡
f2dRect GetProcessCardDrawingRect(int iProcessCardType=0)
{
	AcDbDatabase blkDb(Adesk::kFalse);//定义空的数据库
	f2dRect draw_rect;
	draw_rect.bottomRight.Set(236,110);
	char filename[MAX_PATH];
	if(iProcessCardType==0)
		sprintf(filename,"%s",sys.general_set.m_sJgCard);
	else if(iProcessCardType==1)
		sprintf(filename,"%s",sys.general_set.m_sWeldPartGroupCard);
	else if(iProcessCardType==2)
		sprintf(filename,"%s",sys.general_set.m_sPlankCard);
	else if(iProcessCardType==3)
		sprintf(filename,"%s",sys.general_set.m_sTubeCard);
	else if(iProcessCardType==4)
		sprintf(filename,"%s",sys.general_set.m_sProcessPartStatCard);
	else if(iProcessCardType==5)
		sprintf(filename,"%s",sys.general_set.m_sSlotCard);
	else 
		return draw_rect;

#ifdef _ARX_2007
	if(blkDb.readDwgFile((ACHAR*)_bstr_t(filename),_SH_DENYRW,true)==Acad::eOk)
#else
	if(blkDb.readDwgFile(filename,_SH_DENYRW,true)==Acad::eOk)
#endif
	{
		AcDbEntity *pEnt;
		AcDbBlockTable *pTempBlockTable;
		blkDb.getBlockTable(pTempBlockTable,AcDb::kForRead);
		//获得当前图形块表记录指针
		AcDbBlockTableRecord *pTempBlockTableRecord;//定义块表记录指针
		//以写方式打开模型空间，获得块表记录指针
		pTempBlockTable->getAt(ACDB_MODEL_SPACE,pTempBlockTableRecord,AcDb::kForWrite);
		pTempBlockTable->close();//关闭块表
		AcDbBlockTableRecordIterator *pIterator=NULL;
		pTempBlockTableRecord->newIterator( pIterator);
		if(pIterator)
		{
			for(;!pIterator->done();pIterator->step())
			{
				GRID_DATA_STRU grid_data;
				pIterator->getEntity(pEnt,AcDb::kForWrite);
				if(pEnt->isKindOf(AcDbPoint::desc()))
				{
					if(!GetGridKey((AcDbPoint*)pEnt,&grid_data))
					{
						pEnt->close();
						continue;
					}
					if(grid_data.data_type==3)	//草图区域
					{
						draw_rect.topLeft.Set(grid_data.min_x,grid_data.max_y);
						draw_rect.bottomRight.Set(grid_data.max_x,grid_data.min_y);
						return draw_rect;
					}
				}
				pEnt->close();
			}
		}
		pTempBlockTableRecord->close();
	}
	return draw_rect;
}
f2dRect InsertJgProcessCardTbl(AcDbBlockTableRecord *pBlockTableRecord,f2dPoint insert_pos,PART_JG_CARD_INFO *pJgInfo,BOOL bPrompt = FALSE)
{
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	AcDbDatabase blkDb(Adesk::kFalse);//定义空的数据库
	AcDbObjectId newEntId,blockId;
	f2dRect draw_rect;
	f3dPoint origin_pos(insert_pos.x,insert_pos.y);
	draw_rect.bottomRight.Set(236,110);
	AcDbBlockReference *pBlkRef = new AcDbBlockReference;
	char filename[MAX_PATH];
	sprintf(filename,"%s",sys.general_set.m_sJgCard);
#ifdef _ARX_2007
	if(blkDb.readDwgFile((ACHAR*)_bstr_t(filename),_SH_DENYRW,true)==Acad::eOk&&IsValidProcessCard(&blkDb,"JgCard"))
#else
	if(blkDb.readDwgFile(filename,_SH_DENYRW,true)==Acad::eOk&&IsValidProcessCard(&blkDb,"JgCard"))
#endif
	{
		AcDbEntity *pEnt;
		AcDbBlockTable *pTempBlockTable;
		blkDb.getBlockTable(pTempBlockTable,AcDb::kForRead);
		//获得当前图形块表记录指针
		AcDbBlockTableRecord *pTempBlockTableRecord;//定义块表记录指针
		//以写方式打开模型空间，获得块表记录指针
		pTempBlockTable->getAt(ACDB_MODEL_SPACE,pTempBlockTableRecord,AcDb::kForWrite);
		pTempBlockTable->close();//关闭块表
		AcDbBlockTableRecordIterator *pIterator=NULL;
		pTempBlockTableRecord->newIterator( pIterator);
		if(pIterator)
		{
			for(;!pIterator->done();pIterator->step())
			{
				GRID_DATA_STRU grid_data;
				pIterator->getEntity(pEnt,AcDb::kForWrite);
				if(pEnt->isKindOf(AcDbPoint::desc()))
				{
					if(!GetGridKey((AcDbPoint*)pEnt,&grid_data))
					{
						pEnt->close();
						continue;
					}
					if(grid_data.data_type==1||grid_data.data_type==2)	//实时文本||实时数据
					{
						if(grid_data.type_id==ITEM_TYPE_CODE_NO)		//代号
						{
							CXhChar50 ss(Ta.m_sTaAlias);
							if(gxDrwLocalizer.ciCustomerSerial==5)
								ss.Append(CXhChar16("-%s",(char*)pJgInfo->sSeg));	//青岛东方要求代号中添加段号
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_TA_TYPE)	//塔型
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sTaType);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LENGTH)	//长度
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sLength);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PART_NO)	//件号
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sPartNo);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_MAP_NO)	//钢印号
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sTaStampNo);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_TA_NUM)	//基数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,CXhChar16("%d",pJgInfo->m_nTaNum));
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUT_ROOT)	//刨根
						{	//默认显示刨根标记，如果刨根长度已设置且标记类型为实时数据则显示长度  wht 12-03-10
							CXhChar50 ss(pJgInfo->sCutRoot);
							if(grid_data.data_type==2&&pJgInfo->sCutRootLen.GetLength()>0)	//实时数据	wht 10-11-17
								ss.Copy(pJgInfo->sCutRootLen);	//刨根长度
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUT_BER)	//铲背
						{	//默认显示铲背标记，如果铲背长度已设置且标记类型为实时数据则显示长度  wht 12-03-10
							CXhChar50 ss(pJgInfo->sCutBer);
							if(grid_data.data_type==2&&pJgInfo->sCutBerLen.GetLength()>0)	//实时数据	wht 10-11-17	
								ss.Copy(pJgInfo->sCutBerLen);	//铲背长度 
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_HUOQU_FST)	//一次火曲
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sHuoQuFst);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_HUOQU_SEC)	//二次火曲
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sHuoQuSec);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_ZUAN_KONG)	//钻孔
						{
							CXhChar50 ss;
							if(pJgInfo->sZuanKongNum.GetLength()>0)
							{
								if(grid_data.data_type==1)
									ss.Copy(GetMarkSymbol());
								else
									ss.Copy(pJgInfo->sZuanKongNum);
							}
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CHONG_KONG)	//冲孔
						{
							CXhChar50 ss;
							if(pJgInfo->sChongKongNum.GetLength()>0)
							{
								if(grid_data.data_type==1)
									ss.Copy(GetMarkSymbol());
								else
									ss.Copy(pJgInfo->sChongKongNum);
							}
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PUNCH_SHEAR)	//冲剪
						{
							CXhChar50 ss;
							if(grid_data.data_type==1&&pJgInfo->m_bPunchShear)
								ss.Copy(GetMarkSymbol());
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_GAS_CUTTING)			//气割
						{
							CXhChar50 ss;
							if(grid_data.data_type==1&&pJgInfo->m_bGasCutting)
								ss.Copy(GetMarkSymbol());
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_ONCE_OVER)		//一次完成
						{
							CXhChar50 ss;
							if(grid_data.data_type==1&&pJgInfo->m_bOneShot)
								ss.Copy(GetMarkSymbol());
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUT_ANGLE)	//切角
						{
							CXhChar50 ss;
							if(pJgInfo->sCutAngEX.GetLength()>0||pJgInfo->sCutAngEY.GetLength()>0
								||pJgInfo->sCutAngSX.GetLength()>0||pJgInfo->sCutAngSY.GetLength()>0)
								ss.Copy(GetMarkSymbol());
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUT_ANGLE_S_X)	//始端X肢切角
						{
							CXhChar50 ss;
							if(grid_data.data_type==1)
								ss.Copy(pJgInfo->sCutAngSX);
							else// if(grid_data.data_type==2);
								ss.Copy(pJgInfo->sCutAngParaSX);
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUT_ANGLE_S_Y)	//始端Y肢切角
						{
							CXhChar50 ss;
							if(grid_data.data_type==1)
								ss.Copy(pJgInfo->sCutAngSY);
							else// if(grid_data.data_type==2);
								ss.Copy(pJgInfo->sCutAngParaSY);
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUT_ANGLE_E_X)	//终端X肢切角
						{
							CXhChar50 ss;
							if(grid_data.data_type==1)
								ss.Copy(pJgInfo->sCutAngEX);
							else// if(grid_data.data_type==2);
								ss.Copy(pJgInfo->sCutAngParaEX);
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUT_ANGLE_E_Y)	//终端Y肢切角
						{
							CXhChar50 ss;
							if(grid_data.data_type==1)
								ss.Copy(pJgInfo->sCutAngEY);
							else// if(grid_data.data_type==2);
								ss.Copy(pJgInfo->sCutAngParaEY);
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PUSH_FLAT)	//压扁
						{
							CXhChar50 ss(pJgInfo->sPushFlat);
							if(grid_data.data_type==1&&strlen(ss)>0)	//实时文本
								ss.Copy(GetMarkSymbol());
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PUSH_FLAT_S_X)	//始端X肢压扁
						{
							CXhChar50 ss(pJgInfo->sPushFlatParaSX);
							if(grid_data.data_type==1&&strlen(ss)>0)	//实时文本
								ss.Copy(GetMarkSymbol());
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PUSH_FLAT_S_Y)	//始端Y肢压扁
						{
							CXhChar50 ss(pJgInfo->sPushFlatParaSY);
							if(grid_data.data_type==1&&strlen(ss)>0)	//实时文本
								ss.Copy(GetMarkSymbol());
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PUSH_FLAT_E_X)	//终端X肢压扁
						{
							CXhChar50 ss(pJgInfo->sPushFlatParaEX);
							if(grid_data.data_type==1&&strlen(ss)>0)	//实时文本
								ss.Copy(GetMarkSymbol());
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PUSH_FLAT_E_Y)	//终端Y肢压扁
						{
							CXhChar50 ss(pJgInfo->sPushFlatParaEY);
							if(grid_data.data_type==1&&strlen(ss)>0)	//实时文本
								ss.Copy(GetMarkSymbol());
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PUSH_FLAT_M_X)	//中间X肢压扁
						{
							CXhChar50 ss(pJgInfo->sPushFlatParaMX);
							if(grid_data.data_type==1&&strlen(ss)>0)	//实时文本
								ss.Copy(GetMarkSymbol());
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PUSH_FLAT_M_Y)	//中间Y肢压扁
						{
							CXhChar50 ss(pJgInfo->sPushFlatParaMY);
							if(grid_data.data_type==1&&strlen(ss)>0)	//实时文本
								ss.Copy(GetMarkSymbol());
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_WELD)	//焊接
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sWeld);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_KAIJIAO)	//开角
						{
							CXhChar50 ss(pJgInfo->sKaiJiao);
							if(grid_data.data_type==1&&pJgInfo->sKaiJiao.GetLength()>0)//实时文本
								ss.Copy(GetMarkSymbol());
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_HEJIAO)	//合角
						{
							CXhChar50 ss(pJgInfo->sHeJiao);
							if(grid_data.data_type==1&&pJgInfo->sHeJiao.GetLength()>0)//实时文本
								ss.Copy(GetMarkSymbol());
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_WING_ANGLE)	//两肢夹角
						{
							char ss[8]="";
							if(grid_data.data_type>0&&(pJgInfo->sKaiJiao.GetLength()>0||pJgInfo->sHeJiao.GetLength()>0))//实时文本或实时数据
								strcpy(ss,pJgInfo->sWingAngle);
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PART_NUM)	//单基数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sPartNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PIECE_WEIGHT)	//单重 
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sPartWeight);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_SUM_PART_NUM)	//加工数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sSumPartNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_SUM_WEIGHT)	//总重
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sSumPartWeight);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_DES_MAT)	//设计材质
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sDesMaterial);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_REPL_GUIGE)	//代用规格
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sReplGuiGe);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_DES_GUIGE)	//设计规格
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sDesGuiGe);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PRJ_NAME)	//工程名称
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sPrjName);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSM12_NUM)	//M12孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sM12HoleNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSM16_NUM)	//M16孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sM16HoleNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSM18_NUM)	//M18孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sM18HoleNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSM20_NUM)	//M20孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sM20HoleNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSM22_NUM)	//M22孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sM22HoleNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSM24_NUM)	//M24孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sM24HoleNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSMX_NUM)	//其它孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sMXHoleNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSSUM_NUM)	//总孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pJgInfo->sSumHoleNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_TAB_MAKER)	//制表人
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sOperator);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CRITIC)	//评审人
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sCritic);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_AUDITOR)	//审核人
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sAuditor);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PART_NOTES)//备注
						{	//角钢工艺卡中备注信息中普遍需要显示清根及铲背信息
							CXhChar100 ss(pJgInfo->sNotes);
							if(pJgInfo->sCutRoot[0]!=' '&&pJgInfo->sCutRoot[0]!='\0')	//需要清根
							{
#ifdef AFX_TARG_ENU_ENGLISH
								ss.Append("root removing",',');
#else
								ss.Append("清根",',');
#endif
							}
							if(pJgInfo->sCutBer[0]!=' '&&pJgInfo->sCutBer[0]!='\0')	//需要铲背
							{
#ifdef AFX_TARG_ENU_ENGLISH
								ss.Append(CXhChar16("back off R=%d",pJgInfo->m_nCutBerR),',');
#else
								ss.Append(CXhChar16("铲背R=%d",pJgInfo->m_nCutBerR),',');
#endif
							}
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_DATE)	//日期
						{
							CTime t=CTime::GetCurrentTime();
							CString s=t.Format("%Y.%m.%d");
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,s);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUTBERICON)	//铲背图标
						{
							if(strlen(pJgInfo->sCutBer)>0)
								DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,"cutBer",2);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUTROOTICON)	//刨根图标
						{
							if(strlen(pJgInfo->sCutRoot)>0)
								DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,"cutRoot",2);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_BENDICON)	//制弯图标
						{
							if(strlen(pJgInfo->sHuoQuFst)>0)
								DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,"bend",2);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_WELDICON)	//焊接图标
						{
							if(strlen(pJgInfo->sWeld)>0)
								DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,"weld",2);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_KAIHE_ANGLE_ICON)	//开合角图标 wht 10-07-27
						{
							double fKaiHeJiao=atof(pJgInfo->sWingAngle);
							if(fabs(fKaiHeJiao-90)>sys.part_map.angle.fKaiHeJiaoThreshold)
							{	//角钢两肢夹角与90的差值大于设置的开合角阈值时才需要插入开合角图符 wht 11-08-29
								f3dPoint pos((grid_data.max_x+grid_data.min_x)/2.0,(grid_data.max_y+grid_data.min_y)/2.0,0);
								if(fKaiHeJiao>90)
									InsertBlock(pBlockTableRecord,pos+origin_pos,"kaijiao",0.5,0,0);
								else 
									InsertBlock(pBlockTableRecord,pos+origin_pos,"hejiao",0.5,0,0);
								//标注开合角度数 92.5°
								pos.x+=grid_data.fTextHigh*2*0.7;
								if(fKaiHeJiao>90)
									pos.y+=(1.5*grid_data.fTextHigh);
								else 
									pos.y+=grid_data.fTextHigh;
								DimText(pBlockTableRecord,pos+origin_pos,pJgInfo->sWingAngle,TextStyleTable::hzfs.textStyleId,
									grid_data.fTextHigh,0,AcDb::kTextCenter,AcDb::kTextVertMid);
							}
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PAGENUM)	//共 页
						{
							char ss[20];
#ifdef AFX_TARG_ENU_ENGLISH
							sprintf(ss,"pages %4d total",pJgInfo->m_nPageNum);
#else
							sprintf(ss,"共%4d页",pJgInfo->m_nPageNum);
#endif
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PAGEINDEX)	//第 页
						{
							char ss[20];
#ifdef AFX_TARG_ENU_ENGLISH
							sprintf(ss,"pages %4d total",pJgInfo->m_nPageIndex);
#else
							sprintf(ss,"第%4d页",pJgInfo->m_nPageIndex);
#endif
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
					}
					else if(grid_data.data_type==3)	//草图区域
					{
						draw_rect.topLeft.Set(grid_data.min_x,grid_data.max_y);
						draw_rect.bottomRight.Set(grid_data.max_x,grid_data.min_y);
					}
				}
				else if(pEnt->isKindOf(AcDbMText::desc()))
				{
					pEnt->close();
					continue;
				}
				else if(pEnt->isKindOf(AcDbLine::desc()))
				{
					pEnt->close();
					continue;
				}

				pEnt->close();
			}
		}
		pTempBlockTableRecord->close();

		AcDbDatabase *pTempDb;
		blkDb.wblock(pTempDb);
#ifdef _ARX_2007
		GetCurDwg()->insert(blockId,L"JgCard",pTempDb);
#else
		GetCurDwg()->insert(blockId,"JgCard",pTempDb);
#endif
		delete pTempDb;
	}
	else
	{
		pBlkRef->close();
		draw_rect.topLeft.x+=insert_pos.x;
		draw_rect.topLeft.y+=insert_pos.y;
		draw_rect.bottomRight.x+=insert_pos.x;
		draw_rect.bottomRight.y+=insert_pos.y;
		return draw_rect;
	}

	AcGeScale3d scaleXYZ(1.0,1.0,1.0);
	pBlkRef->setBlockTableRecord(blockId);
	pBlkRef->setPosition(AcGePoint3d(insert_pos.x,insert_pos.y,0));
	pBlkRef->setRotation(0);
	pBlkRef->setScaleFactors(scaleXYZ);
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,blockId,pBlkRef))
		pBlkRef->close();
	draw_rect.topLeft.x+=insert_pos.x;
	draw_rect.topLeft.y+=insert_pos.y;
	draw_rect.bottomRight.x+=insert_pos.x;
	draw_rect.bottomRight.y+=insert_pos.y;
	return draw_rect;
}
static void Verify2dRect(f2dRect* pRc,AcGePoint3d& gp,bool firstpoint=false)
{
	if(firstpoint)
	{
		pRc->topLeft=pRc->bottomRight=f2dPoint(gp.x,gp.y);
		return;
	}
	if(gp.x<pRc->topLeft.x)
		pRc->topLeft.x=gp.x;
	if(gp.y>pRc->topLeft.y)
		pRc->topLeft.y=gp.y;
	if(gp.x>pRc->bottomRight.x)
		pRc->bottomRight.x=gp.x;
	if(gp.y<pRc->bottomRight.y)
		pRc->bottomRight.y=gp.y;
}
bool GetProcessCardRegion(const char* szCardFilePathName,f2dRect* prcCard)
{
	f2dRect rcCard;
	rcCard.bottomRight.Set(280,200);
	AcDbDatabase blkDb(Adesk::kFalse);//定义空的数据库
#ifdef _ARX_2007
	if(blkDb.readDwgFile((ACHAR*)_bstr_t((char*)szCardFilePathName),_SH_DENYRW,true)!=Acad::eOk)
#else
	if(blkDb.readDwgFile((char*)szCardFilePathName,_SH_DENYRW,true)!=Acad::eOk)
#endif
		return false;

	AcDbEntity *pEnt;
	AcDbBlockTable *pTempBlockTable;
	blkDb.getBlockTable(pTempBlockTable,AcDb::kForRead);
	//获得当前图形块表记录指针
	AcDbBlockTableRecord *pTempBlockTableRecord;//定义块表记录指针
	//以写方式打开模型空间，获得块表记录指针
	pTempBlockTable->getAt(ACDB_MODEL_SPACE,pTempBlockTableRecord,AcDb::kForWrite);
	pTempBlockTable->close();//关闭块表
	AcDbBlockTableRecordIterator *pIterator=NULL;
	pTempBlockTableRecord->newIterator( pIterator);
	AcGePoint3d gp;
	if(pIterator==NULL)
	{
		pTempBlockTableRecord->close();
		return false;
	}
	int verifyhits=0;
	for(;!pIterator->done();pIterator->step(),verifyhits++)
	{
		pIterator->getEntity(pEnt,AcDb::kForWrite);
		if(pEnt->isKindOf(AcDbPoint::desc()))
		{
			AcDbPoint* pPoint=(AcDbPoint*)pEnt;
			Verify2dRect(prcCard,pPoint->position(),verifyhits==0);
		}
		else if(pEnt->isKindOf(AcDbMText::desc()))
		{
			pEnt->close();
			continue;
		}
		else if(pEnt->isKindOf(AcDbLine::desc()))
		{
			AcDbLine* pLine=(AcDbLine*)pEnt;
			Verify2dRect(prcCard,pLine->startPoint(),verifyhits==0);
			Verify2dRect(prcCard,pLine->endPoint());
		}
		else if(pEnt->isKindOf(AcDbPolyline::desc()))
		{
			AcDbPolyline* pPolyLine=(AcDbPolyline*)pEnt;
			AcGePoint3d location;
			int nVertNum = pPolyLine->numVerts();
			for(int iVertIndex = 0;iVertIndex<nVertNum;iVertIndex++)
			{
				pPolyLine->getPointAt(iVertIndex,location);
				Verify2dRect(prcCard,location);
			}
		}
		pEnt->close();
	}
	pTempBlockTableRecord->close();
	return true;
}
f2dRect InsertPartGroupProcessCardTbl(AcDbBlockTableRecord *pBlockTableRecord,f2dPoint insert_pos,PART_GROUP_CARD_INFO *pPartGroupInfo,BOOL bPrompt = FALSE)
{
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	AcDbDatabase blkDb(Adesk::kFalse);//定义空的数据库
	AcDbObjectId newEntId,blockId;
	f2dRect draw_rect;
	f3dPoint origin_pos(insert_pos.x,insert_pos.y);
	draw_rect.bottomRight.Set(280,200);
	AcDbBlockReference *pBlkRef = new AcDbBlockReference;
	char filename[MAX_PATH];
	sprintf(filename,"%s",sys.general_set.m_sWeldPartGroupCard);
#ifdef _ARX_2007
	if(blkDb.readDwgFile((ACHAR*)_bstr_t(filename),_SH_DENYRW,true)==Acad::eOk&&IsValidProcessCard(&blkDb,"PartGroupCard"))
#else
	if(blkDb.readDwgFile(filename,_SH_DENYRW,true)==Acad::eOk&&IsValidProcessCard(&blkDb,"PartGroupCard"))
#endif
	{
		AcDbEntity *pEnt;
		AcDbBlockTable *pTempBlockTable;
		blkDb.getBlockTable(pTempBlockTable,AcDb::kForRead);
		//获得当前图形块表记录指针
		AcDbBlockTableRecord *pTempBlockTableRecord;//定义块表记录指针
		//以写方式打开模型空间，获得块表记录指针
		pTempBlockTable->getAt(ACDB_MODEL_SPACE,pTempBlockTableRecord,AcDb::kForWrite);
		pTempBlockTable->close();//关闭块表
		AcDbBlockTableRecordIterator *pIterator=NULL;
		pTempBlockTableRecord->newIterator( pIterator);
		if(pIterator)
		{
			for(;!pIterator->done();pIterator->step())
			{
				GRID_DATA_STRU grid_data;
				pIterator->getEntity(pEnt,AcDb::kForWrite);
				if(pEnt->isKindOf(AcDbPoint::desc()))
				{
					if(!GetGridKey((AcDbPoint*)pEnt,&grid_data))
					{
						pEnt->close();
						continue;
					}
					if(grid_data.data_type==1||grid_data.data_type==2)	//实时文本||实时数据
					{
						double fWidth=grid_data.max_x-grid_data.min_x;
						if(grid_data.type_id==ITEM_TYPE_FOOTNAIL_PLATE)		//脚钉底座材料表
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartGroupInfo->m_sFootNailPlate,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CODE_NO)		//代号
						{
							LOCALE_PROPITEM* pItem = gxDrwLocalizer.GetLocaleItem("CardCodeLabelAddSegI");
							CXhChar50 szPrjTowerTypeCode = Ta.m_sTaAlias;
							if (szPrjTowerTypeCode[szPrjTowerTypeCode.GetLength() - 1] == '-')
								szPrjTowerTypeCode[szPrjTowerTypeCode.GetLength() - 1] = 0;
							if (pItem != NULL && pItem->bVal)//sys.customize.ciCustomerSerial==5)
								szPrjTowerTypeCode.Append(CXhChar16("-%s", (char*)pPartGroupInfo->m_sSegStr));	//青岛东方要求代号中添加段号
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data, szPrjTowerTypeCode,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_TA_TYPE)	//塔型
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sTaType,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LENGTH)	//长度
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartGroupInfo->m_sLength,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PART_NO)	//件号
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartGroupInfo->m_sPartNo,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_MAP_NO)	//钢印号
						{
							LOCALE_PROPITEM* pItem = gxDrwLocalizer.GetLocaleItem("CardCodeLabelAddSegI");
							CXhChar50 szDrawCode= Ta.m_sTaStampNo;
							if (szDrawCode[szDrawCode.GetLength() - 1] == '-')
								szDrawCode[szDrawCode.GetLength() - 1] = 0;
							if (szDrawCode.GetLength()>0 && pItem != NULL && pItem->bVal)//sys.customize.ciCustomerSerial==5)
								szDrawCode.Append(CXhChar16("-%s", (char*)pPartGroupInfo->m_sSegStr));	//青岛东方要求代号中添加段号
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data, szDrawCode,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_TA_NUM)	//基数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,CXhChar16("%d",Ta.m_nTaNum),1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUT_ROOT)	//刨根
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartGroupInfo->m_sCutRoot,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUT_BER)	//铲背
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartGroupInfo->m_sCutBer,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUT_ANGLE)	//切角
						{
							CXhChar50 ss;
							if(pPartGroupInfo->m_sCutAngEX.GetLength()>0||pPartGroupInfo->m_sCutAngEY.GetLength()>0
								||pPartGroupInfo->m_sCutAngSX.GetLength()>0||pPartGroupInfo->m_sCutAngSY.GetLength()>0)
								ss.Copy(GetMarkSymbol());
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUT_ANGLE_S_X)	//始端X肢切角
						{
							CXhChar50 ss;
							if(grid_data.data_type==1)
								strcpy(ss,pPartGroupInfo->m_sCutAngSX);
							else// if(grid_data.data_type==2);
								strcpy(ss,pPartGroupInfo->m_sCutAngParaSX);
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUT_ANGLE_S_Y)	//始端Y肢切角
						{
							CXhChar50 ss;
							if(grid_data.data_type==1)
								strcpy(ss,pPartGroupInfo->m_sCutAngSY);
							else// if(grid_data.data_type==2);
								strcpy(ss,pPartGroupInfo->m_sCutAngParaSY);
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUT_ANGLE_E_X)	//终端X肢切角
						{
							CXhChar50 ss;
							if(grid_data.data_type==1)
								strcpy(ss,pPartGroupInfo->m_sCutAngEX);
							else// if(grid_data.data_type==2);
								strcpy(ss,pPartGroupInfo->m_sCutAngParaEX);
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUT_ANGLE_E_Y)	//终端Y肢切角
						{
							CXhChar50 ss;
							if(grid_data.data_type==1)
								strcpy(ss,pPartGroupInfo->m_sCutAngEY);
							else// if(grid_data.data_type==2);
								strcpy(ss,pPartGroupInfo->m_sCutAngParaEY);
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PUSH_FLAT)	//压扁
						{
							CXhChar50 ss(pPartGroupInfo->m_sPushFlat);
							if(grid_data.data_type==1&&pPartGroupInfo->m_sPushFlat.GetLength()>0)	//实时文本
								ss.Copy(GetMarkSymbol());
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_WELD)	//焊接
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartGroupInfo->m_sWeld,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_KAIJIAO)	//开角
						{
							char ss[8];
							if(grid_data.data_type==1)	//实时文本
								strcpy(ss,pPartGroupInfo->m_sKaiJiao);
							else
								strcpy(ss,pPartGroupInfo->m_sKaiHeJiao);
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_HEJIAO)	//合角
						{
							char ss[8];
							if(grid_data.data_type==1)	//实时文本
								strcpy(ss,pPartGroupInfo->m_sHeJiao);
							else
								strcpy(ss,pPartGroupInfo->m_sKaiHeJiao);
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PART_NUM)	//单基数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartGroupInfo->m_sPartNum,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_SUM_PART_NUM)	//加工数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartGroupInfo->m_sSumPartNum,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_DES_MAT)	//设计材质
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartGroupInfo->m_sMaterial,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_DES_GUIGE)	//设计规格
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartGroupInfo->m_sGuiGe,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PRJ_NAME)	//工程名称
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sPrjName,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_TAB_MAKER)	//制表人
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sOperator,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CRITIC)	//评审人
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sCritic,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_AUDITOR)	//审核人
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sAuditor,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PART_NOTES)//备注
						{
							CXhChar100 ss(pPartGroupInfo->m_sNotes);
							if(pPartGroupInfo->m_sCutRoot[0]!=' '&&pPartGroupInfo->m_sCutRoot[0]!='\0')	//需要清根
							{
#ifdef AFX_TARG_ENU_ENGLISH
								ss.Append("root removing",',');
#else
								ss.Append("清根",',');
#endif
							}
							if(pPartGroupInfo->m_sCutBer[0]!=' '&&pPartGroupInfo->m_sCutBer[0]!='\0')	//需要铲背
							{
#ifdef AFX_TARG_ENU_ENGLISH
								ss.Append(CXhChar16("back off R=%d",pPartGroupInfo->m_nCutBerR),',');
#else
								ss.Append(CXhChar16("铲背R=%d",pPartGroupInfo->m_nCutBerR),',');
#endif
							}
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_DATE)	//日期
						{
							CTime t=CTime::GetCurrentTime();
							CString s=t.Format("%Y.%m.%d");
							grid_data.fTextHigh=2;
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,s,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUTBERICON)	//铲背图标
						{
							if(strlen(pPartGroupInfo->m_sCutBer)>0)
								DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,"cutBer",2);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUTROOTICON)	//刨根图标
						{
							if(strlen(pPartGroupInfo->m_sCutRoot)>0)
								DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,"cutRoot",2);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_WELDICON)	//焊接图标
						{
							if(strlen(pPartGroupInfo->m_sWeld)>0)
								DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,"weld",2);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PAGENUM)	//共 页
						{
							char ss[20];
#ifdef AFX_TARG_ENU_ENGLISH
							sprintf(ss,"pages %4d total",pPartGroupInfo->m_nPageNum);
#else
							sprintf(ss,"共%4d页",pPartGroupInfo->m_nPageNum);
#endif
							grid_data.fTextHigh=2;	//页码字高默认为2
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PAGEINDEX)	//第 页
						{
							char ss[20];
#ifdef AFX_TARG_ENU_ENGLISH
							sprintf(ss,"pages %4d total",pPartGroupInfo->m_nPageIndex);
#else
							sprintf(ss,"第%4d页",pPartGroupInfo->m_nPageIndex);
#endif
							grid_data.fTextHigh=2;
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
					}
					else if(grid_data.data_type==3)	//草图区域
					{
						draw_rect.topLeft.Set(grid_data.min_x,grid_data.max_y);
						draw_rect.bottomRight.Set(grid_data.max_x,grid_data.min_y);
					}
				}
				else if(pEnt->isKindOf(AcDbMText::desc()))
				{
					pEnt->close();
					continue;
				}
				else if(pEnt->isKindOf(AcDbLine::desc()))
				{
					pEnt->close();
					continue;
				}

				pEnt->close();
			}
		}
		pTempBlockTableRecord->close();

		AcDbDatabase *pTempDb;
		blkDb.wblock(pTempDb);
#ifdef _ARX_2007
		GetCurDwg()->insert(blockId,L"PartGroupCard",pTempDb);
#else
		GetCurDwg()->insert(blockId,"PartGroupCard",pTempDb);
#endif
		delete pTempDb;
	}
	else
	{
		pBlkRef->close();
		draw_rect.topLeft.x+=insert_pos.x;
		draw_rect.topLeft.y+=insert_pos.y;
		draw_rect.bottomRight.x+=insert_pos.x;
		draw_rect.bottomRight.y+=insert_pos.y;
		return draw_rect;
	}

	AcGeScale3d scaleXYZ(1.0,1.0,1.0);
	pBlkRef->setBlockTableRecord(blockId);
	pBlkRef->setPosition(AcGePoint3d(insert_pos.x,insert_pos.y,0));
	pBlkRef->setRotation(0);
	pBlkRef->setScaleFactors(scaleXYZ);
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,blockId,pBlkRef))	
		pBlkRef->close();
	draw_rect.topLeft.x+=insert_pos.x;
	draw_rect.topLeft.y+=insert_pos.y;
	draw_rect.bottomRight.x+=insert_pos.x;
	draw_rect.bottomRight.y+=insert_pos.y;
	return draw_rect;
}
//未完成钢管工艺卡 
f2dRect InsertPartProcessCardTbl(AcDbBlockTableRecord *pBlockTableRecord,f2dPoint insert_pos,
							 PART_JG_CARD_INFO *pPartInfo,LDSCLASS_TYPEID cls_id,BOOL bPrompt = FALSE)
{
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	AcDbDatabase blkDb(Adesk::kFalse);//定义空的数据库
	AcDbObjectId newEntId,blockId;
	f2dRect draw_rect;
	f3dPoint origin_pos(insert_pos.x,insert_pos.y);
	draw_rect.bottomRight.Set(236,110);
	AcDbBlockReference *pBlkRef = new AcDbBlockReference;
	char filename[MAX_PATH];
	if(cls_id==CLS_LINETUBE)		//钢管
		sprintf(filename,"%s",sys.general_set.m_sTubeCard);
	else if(cls_id==CLS_LINESLOT ||	//槽钢
			cls_id==CLS_ARCSLOT ||	//环形槽钢
			cls_id==CLS_ARCANGLE||	//环形角钢
			cls_id==CLS_ARCFLAT)	//环形扁铁
		sprintf(filename,"%s",sys.general_set.m_sSlotCard);
	else 
		return draw_rect;
#ifdef _ARX_2007
	if(blkDb.readDwgFile((ACHAR*)_bstr_t(filename),_SH_DENYRW,true)==Acad::eOk)
#else
	if(blkDb.readDwgFile(filename,_SH_DENYRW,true)==Acad::eOk)
#endif
	{
		AcDbEntity *pEnt;
		AcDbBlockTable *pTempBlockTable;
		blkDb.getBlockTable(pTempBlockTable,AcDb::kForRead);
		//获得当前图形块表记录指针
		AcDbBlockTableRecord *pTempBlockTableRecord;//定义块表记录指针
		//以写方式打开模型空间，获得块表记录指针
		pTempBlockTable->getAt(ACDB_MODEL_SPACE,pTempBlockTableRecord,AcDb::kForWrite);
		pTempBlockTable->close();//关闭块表
		AcDbBlockTableRecordIterator *pIterator=NULL;
		pTempBlockTableRecord->newIterator( pIterator);
		if(pIterator)
		{
			for(;!pIterator->done();pIterator->step())
			{
				GRID_DATA_STRU grid_data;
				pIterator->getEntity(pEnt,AcDb::kForWrite);
				if(pEnt->isKindOf(AcDbPoint::desc()))
				{
					if(!GetGridKey((AcDbPoint*)pEnt,&grid_data))
					{
						pEnt->close();
						continue;
					}
					if(grid_data.data_type<=2)		//实时数据
					{
						double fWidth=grid_data.max_x-grid_data.min_x;
						if(grid_data.type_id==ITEM_TYPE_CODE_NO)		//代号
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sTaAlias,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_TA_TYPE)	//塔型
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sTaType,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LENGTH)	//长度
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sLength,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PART_NO)	//件号
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sPartNo,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_MAP_NO)	//钢印号
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sTaStampNo,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_TA_NUM)	//基数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,CXhChar16("%d",Ta.m_nTaNum),1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_WELD)	//焊接
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sWeld,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PART_NUM)	//单基数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sPartNum,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PIECE_WEIGHT)	//单重 
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sPartWeight,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_SUM_PART_NUM)	//加工数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sSumPartNum,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_SUM_WEIGHT)	//总重
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sSumPartWeight,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_DES_MAT)	//设计材质
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sDesMaterial,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_REPL_GUIGE)	//代用规格
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sReplGuiGe,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_DES_GUIGE)	//设计规格
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sDesGuiGe,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_MAP_SCALE)	//比例
						{
							/*scale_id=DimMText(pBlockTableRecord,pPlankInfo->sMapScale,
								f3dPoint((grid_data.max_x+grid_data.min_x)/2.0,(grid_data.max_y+grid_data.min_y)/2.0,0),
								grid_data.max_x-grid_data.min_x,
								grid_data.fTextHigh,TextStyleTable::hzfs.textStyleId,AcDbMText::kMiddleCenter,AcDbMText::kLtoR,0);*/
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PRJ_NAME)	//工程名称
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sPrjName,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSM12_NUM)	//M12孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sM12HoleNum,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSM16_NUM)	//M16孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sM16HoleNum,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSM18_NUM)	//M18孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sM18HoleNum,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSM20_NUM)	//M20孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sM20HoleNum,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSM22_NUM)	//M22孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sM22HoleNum,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSM24_NUM)	//M24孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sM24HoleNum,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSMX_NUM)	//其它孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sMXHoleNum,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSSUM_NUM)	//总孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sSumHoleNum,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_TAB_MAKER)	//制表人
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sOperator,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PART_NOTES)//备注
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPartInfo->sNotes,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_DATE)	//日期
						{
							CTime t=CTime::GetCurrentTime();
							CString s=t.Format("%Y-%m-%d");
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,s,1,fWidth);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_WELDICON)	//焊接图标
						{
							if(strlen(pPartInfo->sWeld)>0)
								DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,"weld",2);
							pEnt->erase();
							pEnt->close();
							continue;
						}
					}
					else if(grid_data.data_type==3)	//草图区域
					{
						draw_rect.topLeft.Set(grid_data.min_x,grid_data.max_y);
						draw_rect.bottomRight.Set(grid_data.max_x,grid_data.min_y);
					}
				}
				else if(pEnt->isKindOf(AcDbMText::desc()))
				{
					pEnt->close();
					continue;
				}
				else if(pEnt->isKindOf(AcDbLine::desc()))
				{
					pEnt->close();
					continue;
				}

				pEnt->close();
			}
		}
		pTempBlockTableRecord->close();

		AcDbDatabase *pTempDb;
		blkDb.wblock(pTempDb);
		if(cls_id==CLS_LINETUBE)	//钢管
		{
		#ifdef _ARX_2007
			GetCurDwg()->insert(blockId,L"TubeCard",pTempDb);
		#else
			GetCurDwg()->insert(blockId,"TubeCard",pTempDb);
		#endif
		}
		else if(cls_id==CLS_LINESLOT)	//槽钢
		{
		#ifdef _ARX_2007
			GetCurDwg()->insert(blockId,L"SlotCard",pTempDb);
		#else
			GetCurDwg()->insert(blockId,"SlotCard",pTempDb);
		#endif
		}
		else if(cls_id==CLS_ARCSLOT)	//环形槽钢
		{
		#ifdef _ARX_2007
			GetCurDwg()->insert(blockId,L"ArcSlotCard",pTempDb);
		#else
			GetCurDwg()->insert(blockId,"ArcSlotCard",pTempDb);
		#endif
		}
		else if(cls_id==CLS_ARCANGLE)	//环形角钢
		{
		#ifdef _ARX_2007
			GetCurDwg()->insert(blockId,L"ArcAngleCard",pTempDb);
		#else
			GetCurDwg()->insert(blockId,"ArcAngleCard",pTempDb);
		#endif
		}
		else if(cls_id==CLS_ARCFLAT)	//环形扁铁
		{
		#ifdef _ARX_2007
			GetCurDwg()->insert(blockId,L"ArcFlatCard",pTempDb);
		#else
			GetCurDwg()->insert(blockId,"ArcSFlatCard",pTempDb);
		#endif
		}
		delete pTempDb;
	}
	else
	{
		pBlkRef->close();
		draw_rect.topLeft.x+=insert_pos.x;
		draw_rect.topLeft.y+=insert_pos.y;
		draw_rect.bottomRight.x+=insert_pos.x;
		draw_rect.bottomRight.y+=insert_pos.y;
		return draw_rect;
	}

	AcGeScale3d scaleXYZ(1.0,1.0,1.0);
	pBlkRef->setBlockTableRecord(blockId);
	pBlkRef->setPosition(AcGePoint3d(insert_pos.x,insert_pos.y,0));
	pBlkRef->setRotation(0);
	pBlkRef->setScaleFactors(scaleXYZ);
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,blockId,pBlkRef))
		pBlkRef->close();
	draw_rect.topLeft.x+=insert_pos.x;
	draw_rect.topLeft.y+=insert_pos.y;
	draw_rect.bottomRight.x+=insert_pos.x;
	draw_rect.bottomRight.y+=insert_pos.y;
	return draw_rect;
}

f2dRect InsertPlankProcessCardTbl(AcDbBlockTableRecord *pBlockTableRecord,f2dPoint insert_pos,PART_PLANK_CARD_INFO *pPlankInfo,AcDbObjectId &scale_id,BOOL bPrompt = FALSE)
{
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	AcDbDatabase blkDb(Adesk::kFalse);//定义空的数据库
	AcDbObjectId newEntId,blockId;
	f2dRect draw_rect;
	f3dPoint origin_pos(insert_pos.x,insert_pos.y);
	draw_rect.bottomRight.Set(236,110);
	AcDbBlockReference *pBlkRef = new AcDbBlockReference;
	char filename[MAX_PATH];
	sprintf(filename,"%s",sys.general_set.m_sPlankCard);
#ifdef _ARX_2007
	if(blkDb.readDwgFile((ACHAR*)_bstr_t(filename),_SH_DENYRW,true)==Acad::eOk&&IsValidProcessCard(&blkDb,"PlankCard"))
#else
	if(blkDb.readDwgFile(filename,_SH_DENYRW,true)==Acad::eOk&&IsValidProcessCard(&blkDb,"PlankCard"))
#endif
	{
		AcDbEntity *pEnt;
		AcDbBlockTable *pTempBlockTable;
		blkDb.getBlockTable(pTempBlockTable,AcDb::kForRead);
		//获得当前图形块表记录指针
		AcDbBlockTableRecord *pTempBlockTableRecord;//定义块表记录指针
		//以写方式打开模型空间，获得块表记录指针
		pTempBlockTable->getAt(ACDB_MODEL_SPACE,pTempBlockTableRecord,AcDb::kForWrite);
		pTempBlockTable->close();//关闭块表
		AcDbBlockTableRecordIterator *pIterator=NULL;
		pTempBlockTableRecord->newIterator( pIterator);
		if(pIterator)
		{
			for(;!pIterator->done();pIterator->step())
			{
				GRID_DATA_STRU grid_data;
				pIterator->getEntity(pEnt,AcDb::kForWrite);
				if(pEnt->isKindOf(AcDbPoint::desc()))
				{
					if(!GetGridKey((AcDbPoint*)pEnt,&grid_data))
					{
						pEnt->close();
						continue;
					}
					if(grid_data.data_type<=2)		//实时数据
					{
						f3dPoint dimPos=f3dPoint((grid_data.max_x+grid_data.min_x)/2.0,(grid_data.max_y+grid_data.min_y)/2.0,0)+origin_pos;
						if(grid_data.type_id==ITEM_TYPE_CODE_NO)		//代号
						{
							CXhChar50 ss(Ta.m_sTaAlias);
							LOCALE_PROPITEM* pItem=gxDrwLocalizer.GetLocaleItem("CardCodeLabelAddSegI");
							if(pItem!=NULL&&pItem->bVal)//sys.customize.ciCustomerSerial==5)
								ss.Append(CXhChar16("-%s",(char*)pPlankInfo->sSeg));	//青岛东方要求代号中添加段号
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_TA_TYPE)	//塔型
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sTaType);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LENGTH)	//长度
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sLength);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PART_NO)	//件号
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sPartNo);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_MAP_NO)	//钢印号
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sTaStampNo);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_TA_NUM)	//基数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,CXhChar16("%d",pPlankInfo->m_nTaNum));
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUT_EDGE)	//铲边
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sCutEdge);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_WRAP_EDGE)	//卷边
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sWrapEdge);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_HUOQU_FST)	//一次火曲
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sHuoQuFst);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_HUOQU_SEC)	//二次火曲
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sHuoQuSec);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_WELD)	//焊接
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sWeld);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CUT_ARC)	//切弧
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sCutArc);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_FILLET)	//坡口
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sFillet);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PART_NUM)	//单基数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sPartNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PIECE_WEIGHT)	//单重 
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sPartWeight);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_SUM_PART_NUM)	//加工数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sSumPartNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_SUM_WEIGHT)	//总重
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sSumPartWeight);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_DES_MAT)	//设计材质
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sDesMaterial);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_REPL_GUIGE)	//代用规格
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sReplGuiGe);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_DES_GUIGE)	//设计规格
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sDesGuiGe);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_MAP_SCALE)	//比例
						{
							/*scale_id=DimMText(pBlockTableRecord,pPlankInfo->sMapScale,
								f3dPoint((grid_data.max_x+grid_data.min_x)/2.0,(grid_data.max_y+grid_data.min_y)/2.0,0),
								grid_data.max_x-grid_data.min_x,
								grid_data.fTextHigh,TextStyleTable::hzfs.textStyleId,AcDbMText::kMiddleCenter,AcDbMText::kLtoR,0);*/
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sMapScale);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PRJ_NAME)	//工程名称
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sPrjName);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSM12_NUM)	//M12孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sM12HoleNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSM16_NUM)	//M16孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sM16HoleNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSM18_NUM)	//M18孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sM18HoleNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSM20_NUM)	//M20孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sM20HoleNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSM22_NUM)	//M22孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sM22HoleNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSM24_NUM)	//M24孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sM24HoleNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSMX_NUM)	//其它孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sMXHoleNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_LSSUM_NUM)	//总孔数
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sSumHoleNum);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_ZUAN_KONG)	//钻孔
						{
							CXhChar50 ss;
							if(pPlankInfo->sZuanKongNum.GetLength()>0)
							{
								if(grid_data.data_type==1)
									ss.Copy(GetMarkSymbol());
								else
									ss.Copy(pPlankInfo->sZuanKongNum);
							}
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_CHONG_KONG)	//冲孔
						{
							CXhChar50 ss;
							if(pPlankInfo->sChongKongNum.GetLength()>0)
							{
								if(grid_data.data_type==1)
									ss.Copy(GetMarkSymbol());
								else
									ss.Copy(pPlankInfo->sChongKongNum);
							}
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PUNCH_SHEAR)	//冲剪
						{
							CXhChar50 ss;
							if(grid_data.data_type==1&&pPlankInfo->m_bPunchShear)
								ss.Copy(GetMarkSymbol());
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_GAS_CUTTING)			//气割
						{
							CXhChar50 ss;
							if(grid_data.data_type==1&&pPlankInfo->m_bGasCutting)
								ss.Copy(GetMarkSymbol());
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,ss);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_TAB_MAKER)	//制表人
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,Ta.m_sOperator);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_PART_NOTES)//备注
						{
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,pPlankInfo->sNotes);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_DATE)	//日期
						{
							CTime t=CTime::GetCurrentTime();
							CString s=t.Format("%Y-%m-%d");
							DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,s);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_BENDICON)	//制弯图标
						{
							if(strlen(pPlankInfo->sHuoQuFst)>0)
								DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,"bend",2);
							pEnt->erase();
							pEnt->close();
							continue;
						}
						else if(grid_data.type_id==ITEM_TYPE_WELDICON)	//焊接图标
						{
							if(strlen(pPlankInfo->sWeld)>0)
								DimCardTblGridData(pBlockTableRecord,origin_pos,grid_data,"weld",2);
							pEnt->erase();
							pEnt->close();
							continue;
						}
					}
					else if(grid_data.data_type==3)	//草图区域
					{
						draw_rect.topLeft.Set(grid_data.min_x,grid_data.max_y);
						draw_rect.bottomRight.Set(grid_data.max_x,grid_data.min_y);
					}
				}
				else if(pEnt->isKindOf(AcDbMText::desc()))
				{
					pEnt->close();
					continue;
				}
				else if(pEnt->isKindOf(AcDbLine::desc()))
				{
					pEnt->close();
					continue;
				}

				pEnt->close();
			}
		}
		pTempBlockTableRecord->close();

		AcDbDatabase *pTempDb;
		blkDb.wblock(pTempDb);
#ifdef _ARX_2007
		GetCurDwg()->insert(blockId,L"PlankCard",pTempDb);
#else
		GetCurDwg()->insert(blockId,"PlankCard",pTempDb);
#endif
		delete pTempDb;
	}
	else
	{
		pBlkRef->close();
		draw_rect.topLeft.x+=insert_pos.x;
		draw_rect.topLeft.y+=insert_pos.y;
		draw_rect.bottomRight.x+=insert_pos.x;
		draw_rect.bottomRight.y+=insert_pos.y;
		return draw_rect;
	}

	AcGeScale3d scaleXYZ(1.0,1.0,1.0);
	pBlkRef->setBlockTableRecord(blockId);
	pBlkRef->setPosition(AcGePoint3d(insert_pos.x,insert_pos.y,0));
	pBlkRef->setRotation(0);
	pBlkRef->setScaleFactors(scaleXYZ);
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,blockId,pBlkRef))
		pBlkRef->close();
	draw_rect.topLeft.x+=insert_pos.x;
	draw_rect.topLeft.y+=insert_pos.y;
	draw_rect.bottomRight.x+=insert_pos.x;
	draw_rect.bottomRight.y+=insert_pos.y;
	return draw_rect;
}
#endif