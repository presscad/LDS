#include "stdafx.h"
#include "LDS.h"
#include "stdio.h"
#include "math.h"
#include "Tower.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "LDSDoc.h"
#include "MainFrm.h"
#include "CmdLineDlg.h"
//#include "MsgPromptDlg.h"
#include "excel9.h"
#include "database.h"
#include "Query.h"
#include "SortFunc.h"
#include "XhCharString.h"
#include "Variant.h"
#include "ExcelOper.h"
#include "LibraryQuery.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if defined(__LDS_)||defined(__TSA_)
#include "PreProcessor.h"
#include "PostProcessor.h"
#include "PostProcessReport.h"
//������
typedef CLDSNode* NODE_PTR;
typedef CLDSLinePart* POLE_PTR;
typedef BASENODE_REACTION* REACTION_PTR;
int compare_func1(const NODE_PTR& pNode1,const NODE_PTR& pNode2);
int compare_func2(const ELEM_PTR &pElem1,const ELEM_PTR &pElem2);
int compare_func3(const REACTION_PTR &pReact1,const REACTION_PTR &pReact2);
static void SetExcelCellValue(_Worksheet& sheet,char* cell,COleVariant& varValue,double colWidth=0)
{
	Range excel_range;
	LPDISPATCH pRange = sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetValue(varValue);
	if(colWidth>0)
		excel_range.SetColumnWidth(COleVariant(colWidth));
	excel_range.DetachDispatch();
	pRange->Release();
}
static void SetReactionExcelRecord(int iLine,BASENODE_REACTION* pReact,CVariant2dArray &Var2dArr,bool bSetPointI=true)
{
	CXhChar16 ss;
	if(bSetPointI)
	{	//��д�ڵ��
		ss.Printf("%5d",pReact->pNode->point_i);
		Var2dArr.SetValueAt(iLine-1,0,COleVariant(ss));//��������
	}
	ss.Printf("%2d",pReact->pModule->iNo);
	Var2dArr.SetValueAt(iLine-1,1,COleVariant(ss));//��������

	ss.Printf("%2d",pReact->pStatus->iNo);
	Var2dArr.SetValueAt(iLine-1,2,COleVariant(ss));//��������

	char quadLeg[4]={0};
	for(int k=0;k<4;k++)
		quadLeg[k]=(pReact->leg_no[k]-1)%8+'A';
	ss.Printf("%C-%C-%C-%C",quadLeg[0],quadLeg[1],quadLeg[2],quadLeg[3]);
	Var2dArr.SetValueAt(iLine-1,3,COleVariant(ss));//��������
	
	ss.Printf("%8.3f",pReact->reaction_force_design.x/1000);
	Var2dArr.SetValueAt(iLine-1,4,COleVariant(ss));//��������

	ss.Printf("%8.3f",pReact->reaction_force_design.y/1000);
	Var2dArr.SetValueAt(iLine-1,5,COleVariant(ss));//��������

	ss.Printf("%8.3f",pReact->reaction_force_design.z/1000);
	Var2dArr.SetValueAt(iLine-1,6,COleVariant(ss));//��������
	
	/*if(g_sysPara.bCalStdFoundationForce)
	{
		ss.Printf("%8.3f",pReact->reaction_force_std.x/1000);
		Var2dArr.SetValueAt(iLine-1,7,COleVariant(ss));//��������

		ss.Printf("%8.3f",pReact->reaction_force_std.y/1000);
		Var2dArr.SetValueAt(iLine-1,8,COleVariant(ss));//��������

		ss.Printf("%8.3f",pReact->reaction_force_std.z/1000);
		Var2dArr.SetValueAt(iLine-1,9,COleVariant(ss));//��������
	}*/
}

void CreateDetailRptExcelFileFromReportByEN(char *file_name)
{
	CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	int rpt_type;
	if(strstr(file_name,"Check")!=NULL)
		rpt_type=1;	//����
	else
		rpt_type=0;	//��ѡ
	
	LPDISPATCH pDisp;
	LPUNKNOWN pUnk;
	CLSID clsid;
	_Application excel_app;
	CExcelOperObject::ExcelCLSIDFromProgID(clsid);
	if(::GetActiveObject(clsid, NULL, &pUnk) == S_OK) 
	{
		VERIFY(pUnk->QueryInterface(IID_IDispatch,(void**) &pDisp) == S_OK);
		excel_app.AttachDispatch(pDisp);
		pUnk->Release();
	} 
	else if(!excel_app.CreateDispatch(clsid)) 
		AfxMessageBox("Excel program not found");
	LPDISPATCH pWorkbooks;
	
	/*CWnd* pWnd = CWnd::FindWindow("XLMAIN", NULL);
	if (pWnd != NULL) 
	{
	pWnd->ShowWindow(SW_SHOWNORMAL);
	pWnd->UpdateWindow();
	pWnd->BringWindowToTop();
	}*/
	
	excel_app.SetSheetsInNewWorkbook(8);	//�����ɹ�������Ĭ�Ϲ������������
	
	Sheets excel_sheets;
	Workbooks excel_workbooks;
	_Workbook excel_workbook;
	_Worksheet excel_sheet;
	Range excel_range;
	LPDISPATCH pRange;
	Borders borders;
	Border border;
	FontLegacy font;
	COleVariant covTrue((short)TRUE),covFalse((short)FALSE);
	char cRangeInfo[500];//ÿ�е�����
	int  iLineNum=0;//�к�
	VERIFY(pWorkbooks = excel_app.GetWorkbooks());
	excel_workbooks.AttachDispatch(pWorkbooks);
	
	LPDISPATCH pWorkbook = excel_workbooks.Add(); // Save the pointer for
	excel_workbook.AttachDispatch(pWorkbook);
	LPDISPATCH pWorksheets = excel_workbook.GetWorksheets();
	ASSERT(pWorksheets != NULL);
	excel_sheets.AttachDispatch(pWorksheets);
	CXhChar16 cell_start;
	CXhChar16 cell_end;
	CXhChar100 cell,ss;
	int iSheet=1;
	int iRow=0;
	int i=0;
	LPDISPATCH pWorksheet;
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("Design Code");
	//�������
	//��һҳ
	pRange = excel_sheet.GetRange(COleVariant("A1"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetValue(COleVariant("***********************************************************************************************"));
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A2"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetValue(COleVariant("*                                                                                             *"));
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A3"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetValue(COleVariant("*            TOWER ANALYSIS AND DESIGN  - (C) 2002-2006 Xerofox Software,Ltd.                 *"));
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A4"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetValue(COleVariant("*                                                                                             *"));
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A5"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetValue(COleVariant("***********************************************************************************************"));	
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A7"));
	excel_range.AttachDispatch(pRange,FALSE);
	sprintf(cRangeInfo,"Project Code : %s%s",Ta.m_sPrjCode);
	excel_range.SetValue(COleVariant(cRangeInfo));
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A8"));
	excel_range.AttachDispatch(pRange,FALSE);
	sprintf(cRangeInfo,"Project Name : %s",Ta.m_sPrjName);
	excel_range.SetValue(COleVariant(cRangeInfo));
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A9"));
	excel_range.AttachDispatch(pRange,FALSE);
	sprintf(cRangeInfo,"Designed by  : %s",Ta.m_sOperator);
	excel_range.SetValue(COleVariant(cRangeInfo));
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A10"));
	excel_range.AttachDispatch(pRange,FALSE);
	CTime t=CTime::GetCurrentTime();
	sprintf(cRangeInfo,"Date run     : %s",t.Format("%H:%M:%S %B %d, %Y"));
	excel_range.SetValue(COleVariant(cRangeInfo));
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A11"));
	excel_range.AttachDispatch(pRange,FALSE);
	//sprintf(cRangeInfo,"�����Ա : %s",Ta.m_sOperator);
	CString product_version;
	theApp.GetProductVersion(product_version);
	sprintf(cRangeInfo,"by           : TSA Version %s",product_version);
	excel_range.SetValue(COleVariant(cRangeInfo));
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A12"));
	excel_range.AttachDispatch(pRange,FALSE);
	sprintf(cRangeInfo,"Licensed to  : %s",Ta.m_sCompanyName);
	excel_range.SetValue(COleVariant(cRangeInfo));
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A13"));
	excel_range.AttachDispatch(pRange,FALSE);
	if(g_sysPara.AnalysisPara.m_iNonlinearStyle==0)
		sprintf(cRangeInfo,"Successfully performed linear analysis!");
	else
		sprintf(cRangeInfo,"Successfully performed nonlinear analysis!");
	excel_range.SetValue(COleVariant(cRangeInfo));
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A14"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetValue(COleVariant("***********************************************************************************************"));	
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A15"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetValue(COleVariant("Part ��:Design Code"));
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A16"));
	excel_range.AttachDispatch(pRange,FALSE);
	if(Ta.m_iMemberSizeCheckCode==0)
		sprintf(cRangeInfo,"Member check option:DL/T 5092-1999 and DL/T 5154-2002, China.");
	else
		sprintf(cRangeInfo,"Member check option:ASCE 10-97, America.");
	excel_range.SetValue(COleVariant(cRangeInfo));
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A18"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetValue(COleVariant("***********************************************************************************************"));
	excel_range.DetachDispatch();
	pRange->Release();
	pWorksheet->Release();
	
	//�ڶ�ҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("Steel Material Properties");
	
	strcpy(cell_start,"A1");
	strcpy(cell_end,"I2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Part ��: Steel Material Properties"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	//borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)-4119));	//xlDouble =-4119;
	border.SetWeight(COleVariant((short)4));	//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	excel_range.DetachDispatch();
	pRange->Release();

	
	pRange = excel_sheet.GetRange(COleVariant("A3"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetValue(COleVariant("When diameter or thickness ��16 mm, it belongs to \"Group 1\"."));
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A4"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetValue(COleVariant("When diameter or thickness ��16~40 mm for Q235, it belongs to \"Group 2\"."));
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A5"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetValue(COleVariant("When diameter or thickness ��16~35 mm for Q345 and Q390, it belongs to \"Group 2\"."));
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A6"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetValue(COleVariant("When diameter or thickness ��40~60 mm for Q235, it belongs to \"Group 3\"."));
	excel_range.DetachDispatch();
	pRange->Release();
	
	pRange = excel_sheet.GetRange(COleVariant("A7"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetValue(COleVariant("When diameter or thickness ��35~50 mm for Q345 and Q390, it belongs to \"Group 3\"."));
	excel_range.DetachDispatch();
	pRange->Release();

	strcpy(cell_start,"A9");
	strcpy(cell_end,"A12");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Steel\nMaterial\nLabel\n"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"B9");
	strcpy(cell_end,"B12");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("  Brief\n\n   Mark\n"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"C9");
	strcpy(cell_end,"C12");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("   Modulus\n        of\nElasticity\n(MPa)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"D9");
	strcpy(cell_end,"D12");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("   Design\n Strength\n  Group 1\n(MPa)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(9.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"E9");
	strcpy(cell_end,"E12");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("   Design\n Strength\n  Group 2\n(MPa)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(9.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"F9");
	strcpy(cell_end,"F12");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("   Design\n Strength\n  Group 3\n(MPa)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(9.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"G9");
	strcpy(cell_end,"G12");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Bearing\n Strength\n  Group 1\n(MPa)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(9.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"H9");
	strcpy(cell_end,"H12");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Bearing\n Strength\n  Group 2\n(MPa)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(9.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"I9");
	strcpy(cell_end,"I12");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Bearing\n Strength\n  Group 3\n(MPa)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(9.0));
	excel_range.ReleaseDispatch();
	
	iRow=13;
	for(i=0;i<CSteelMatLibrary::GetCount();i++)
	{
		STEELMAT mat=CSteelMatLibrary::RecordAt(i);
		cell.Printf("A%d",iRow);
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%s",mat.mark);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();

		cell.Printf("B%d",iRow);
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%c",mat.cBriefMark);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();

		cell.Printf("C%d",iRow);
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%10.2f",mat.E);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();

		cell.Printf("D%d",iRow);
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%10.2f",mat.intensity[4]);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();

		cell.Printf("E%d",iRow);
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%10.2f",mat.intensity[5]);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();

		cell.Printf("F%d",iRow);
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%10.2f",mat.intensity[6]);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();

		cell.Printf("G%d",iRow);
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%10.2f",mat.intensity[8]);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();

		cell.Printf("H%d",iRow);
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%10.2f",mat.intensity[9]);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();

		cell.Printf("I%d",iRow);
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%10.2f",mat.intensity[10]);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();

		iRow++;
	}
	pWorksheet->Release();
	//����ҳ
	/*pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("Part Size List");
	
	strcpy(cell_start,"A1");
	strcpy(cell_end,"N2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Part ��: Part Size List"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	//borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)-4119));	//xlDouble =-4119;
	border.SetWeight(COleVariant((short)4));	//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	excel_range.DetachDispatch();
	pRange->Release();
	
	strcpy(cell_start,"A3");
	strcpy(cell_end,"A5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("No. "));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"B3");
	strcpy(cell_end,"B5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("  Size  "));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"C3");
	strcpy(cell_end,"C5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Area  \n\n(cm2)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"D3");
	strcpy(cell_end,"D5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("G.Ry0\n\n(cm)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();	
	
	strcpy(cell_start,"E3");
	strcpy(cell_end,"E5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("G.Rx \n\n(cm)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"F3");
	strcpy(cell_end,"F5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Fillet\nRadius\n(cm)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"G3");
	strcpy(cell_end,"G5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Unit  \n Weight\n(kg/m)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"H3");
	strcpy(cell_end,"H5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("No. "));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"I3");
	strcpy(cell_end,"I5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Size "));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"J3");
	strcpy(cell_end,"J5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Area \n\n(cm)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"K3");
	strcpy(cell_end,"K5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("G.Ry0\n\n(cm)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"L3");
	strcpy(cell_end,"L5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("G.Rx\n\n(cm)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"M3");
	strcpy(cell_end,"M5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Fillet\nRadius\n(cm)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"N3");
	strcpy(cell_end,"N5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Unit \nWeight\n(kg/m)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();
	
	int iRow=6;

	for(int i=0;i<jgguige_N;)
	{
		CXhChar16 stype;
		CXhChar100 ss;
		stype.Printf("%s","L");
		if(toupper(jgguige_table[i].cType)!='L')
			stype.Printf("%CL",jgguige_table[i].cType);
		
		cell.Printf("A%d",iRow);
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%3d",i+1);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();

		cell.Printf("B%d",iRow);
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%s%3.0f*%2.0f",(char*)stype,jgguige_table[i].wing_wide,jgguige_table[i].wing_thick);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();

		cell.Printf("C%d",iRow);
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%7.3f",jgguige_table[i].area);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();

		cell.Printf("D%d",iRow);
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%6.2f",jgguige_table[i].Ry0);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();

		cell.Printf("E%d",iRow);
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%6.2f",jgguige_table[i].Rx);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();

		cell.Printf("F%d",iRow);
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%5.2f",jgguige_table[i].r);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();

		cell.Printf("G%d",iRow);
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%7.3f",jgguige_table[i].theroy_weight);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();
		i++;
		if(i<jgguige_N)
		{
			stype.Printf("%s","L");
			if(toupper(jgguige_table[i].cType)!='L')
				stype.Printf("%CL",jgguige_table[i].cType);
			
			cell.Printf("H%d",iRow);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%3d",i+1);
			excel_range.SetValue(COleVariant(ss));
			excel_range.SetVerticalAlignment(COleVariant((long)-4108));
			excel_range.ReleaseDispatch();
			
			cell.Printf("I%d",iRow);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%s%3.0f*%2.0f",(char*)stype,jgguige_table[i].wing_wide,jgguige_table[i].wing_thick);
			excel_range.SetValue(COleVariant(ss));
			excel_range.SetVerticalAlignment(COleVariant((long)-4108));
			excel_range.ReleaseDispatch();
			
			cell.Printf("J%d",iRow);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%7.3f",jgguige_table[i].area);
			excel_range.SetValue(COleVariant(ss));
			excel_range.SetVerticalAlignment(COleVariant((long)-4108));
			excel_range.ReleaseDispatch();
			
			cell.Printf("K%d",iRow);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%6.2f",jgguige_table[i].Ry0);
			excel_range.SetValue(COleVariant(ss));
			excel_range.SetVerticalAlignment(COleVariant((long)-4108));
			excel_range.ReleaseDispatch();
			
			cell.Printf("L%d",iRow);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%6.2f",jgguige_table[i].Rx);
			excel_range.SetValue(COleVariant(ss));
			excel_range.SetVerticalAlignment(COleVariant((long)-4108));
			excel_range.ReleaseDispatch();
			
			cell.Printf("M%d",iRow);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%5.2f",jgguige_table[i].r);
			excel_range.SetValue(COleVariant(ss));
			excel_range.SetVerticalAlignment(COleVariant((long)-4108));
			excel_range.ReleaseDispatch();
			
			cell.Printf("N%d",iRow);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%7.3f",jgguige_table[i].theroy_weight);
			excel_range.SetValue(COleVariant(ss));
			excel_range.SetVerticalAlignment(COleVariant((long)-4108));
			excel_range.ReleaseDispatch();
		}
		i++;
		iRow++;
	}
	pWorksheet->Release();*/
	
	//����ҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName(" Bolt Properties");
	
	strcpy(cell_start,"A1");
	strcpy(cell_end,"C2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Part ��: Bolt Properties"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	//borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)-4119));	//xlDouble =-4119;
	border.SetWeight(COleVariant((short)4));	//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	excel_range.DetachDispatch();
	pRange->Release();

	strcpy(cell_start,"A3");
	strcpy(cell_end,"A5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Bolt\nGrade\n(MPa)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"B3");
	strcpy(cell_end,"B5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Shearing\nStrength\n(MPa)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"C3");
	strcpy(cell_end,"C5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Tension\nStrength\n(MPa)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	iRow=6;
	for(i=0;i<LsTypeCount;i++)
	{
		if(i<LsTypeCount)
		{	
			cell.Printf("A%d",iRow);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%s",ls_table[i].grade);
			excel_range.SetValue(COleVariant(ss));
			excel_range.SetVerticalAlignment(COleVariant((long)-4108));
			excel_range.ReleaseDispatch();
			
			cell.Printf("B%d",iRow);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%8.2f",ls_table[i].fv);
			excel_range.SetValue(COleVariant(ss));
			excel_range.SetVerticalAlignment(COleVariant((long)-4108));
			excel_range.ReleaseDispatch();
			
			cell.Printf("C%d",iRow);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%8.2f",ls_table[i].f);
			excel_range.SetValue(COleVariant(ss));
			excel_range.SetVerticalAlignment(COleVariant((long)-4108));
			excel_range.ReleaseDispatch();
			
			iRow++;
		}
	}
	pWorksheet->Release();
	
	//����ҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("Nodes Geometry");
	
	strcpy(cell_start,"A1");
	strcpy(cell_end,"L2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Part ��: Nodes Geometry"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	//borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)-4119));	//xlDouble =-4119;
	border.SetWeight(COleVariant((short)4));	//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	excel_range.DetachDispatch();
	pRange->Release();
	
	strcpy(cell_start,"A3");
	strcpy(cell_end,"A4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Node\nLabel"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"B3");
	strcpy(cell_end,"B4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("  Symmetry\n     Code"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"C3");
	strcpy(cell_end,"C4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" X Coord.\n(m)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"D3");
	strcpy(cell_end,"D4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Y Coord.\n(m)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"E3");
	strcpy(cell_end,"E4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Z Coord.\n(m)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"F3");
	strcpy(cell_end,"F4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("X Disp.\n"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(8.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"G3");
	strcpy(cell_end,"G4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Y Disp.\n"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(8.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"H3");
	strcpy(cell_end,"H4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Z Disp.\n"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(8.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"I3");
	strcpy(cell_end,"I4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("X Rot .\n"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(8.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"J3");
	strcpy(cell_end,"J4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Y Rot .\n"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(8.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"K3");
	strcpy(cell_end,"K4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Z Rot .\n"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(8.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"L3");
	strcpy(cell_end,"L4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Module No.\n"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();

	
	long nNodeNum=Ta.Node.GetNodeNum();
	NODE_PTR *node_arr=new NODE_PTR[nNodeNum];
	i=0;
	iRow=5;
	for(CLDSNode *pNode=preprocessor.result.allnodeset.GetFirst();pNode;pNode=preprocessor.result.allnodeset.GetNext())
	{
		node_arr[i]=pNode;
		i++;
	}
	CQuickSort<NODE_PTR>::QuickSort(node_arr,nNodeNum,compare_func1);
	
	for(i=0;i<nNodeNum;i++)
	{
		if(node_arr[i]->IsFatherObject())	//���ڵ�
		{
			if(node_arr[i]->GetRelativeMirObj(MIRMSG(1))>0x20)
				ss.Append("  X");
			else
				ss.Append("   ");
			if(node_arr[i]->GetRelativeMirObj(MIRMSG(2))>0x20)
				ss.Append("Y");
			else
				ss.Append(" ");
			if(node_arr[i]->GetRelativeMirObj(MIRMSG(3))>0x20)
				ss.Append("Z");
			else
				ss.Append(" ");
			if(stricmp(ss,"     ")==0)
				ss.Copy("      None ");
			else //if(strlen(symmetry)>0)
				ss.Append("-Symm");
		}
		else
		{
			RELATIVE_OBJECT relaObj;
			if(node_arr[i]->GetFatherMirObj(&relaObj))
			{
				if(relaObj.mirInfo.axis_flag==1)
					ss.Copy("     X-Gen ");
				else if(relaObj.mirInfo.axis_flag==2)
					ss.Copy("     Y-Gen");
				else if(relaObj.mirInfo.axis_flag==4)
					ss.Copy("     Z-Gen ");
				else
					ss.Copy("      None ");
			}
			else
				ss.Copy("      None ");
		}
		
		cell.Printf("B%d",iRow);
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();
		
		CLDSModule *pPrevModule=NULL;
		BOOL bFirstLine=TRUE;
		
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			ss.Printf("%s"," ");
			if(!pModule->IsSonPart(node_arr[i],NULL,false))
				continue;	//�ýڵ㲻�Ǵ�ģ���еĽڵ�
			if(pPrevModule==NULL)
			{
				if(node_arr[i]->IsHasRestrict(0,X_RESTRICT))
					ss.Printf("%s","  FIXED");
				else
					ss.Printf("%s","   FREE");
				sprintf(cell_start,"F%d",iRow);
				pRange = excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.SetValue(COleVariant(ss));
				excel_range.SetVerticalAlignment(COleVariant((long)-4108));
				excel_range.ReleaseDispatch();
				
				if(node_arr[i]->IsHasRestrict(0,Y_RESTRICT))
					ss.Printf("%s","  FIXED");
				else
					ss.Printf("%s","   FREE");
				sprintf(cell_start,"G%d",iRow);
				pRange = excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.SetValue(COleVariant(ss));
				excel_range.SetVerticalAlignment(COleVariant((long)-4108));
				excel_range.ReleaseDispatch();
				
				if(node_arr[i]->IsHasRestrict(0,Z_RESTRICT))
					ss.Printf("%s","  FIXED");
				else
					ss.Printf("%s","   FREE");
				sprintf(cell_start,"H%d",iRow);
				pRange = excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.SetValue(COleVariant(ss));
				excel_range.SetVerticalAlignment(COleVariant((long)-4108));
				excel_range.ReleaseDispatch();
				
				if(node_arr[i]->IsHasRestrict(0,X_ROT_RESTRICT))
					ss.Printf("%s","  FIXED");
				else
					ss.Printf("%s","   FREE");
				sprintf(cell_start,"I%d",iRow);
				pRange = excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.SetValue(COleVariant(ss));
				excel_range.SetVerticalAlignment(COleVariant((long)-4108));
				excel_range.ReleaseDispatch();
				
				if(node_arr[i]->IsHasRestrict(0,Y_ROT_RESTRICT))
					ss.Printf("%s","  FIXED");
				else
					ss.Printf("%s","   FREE");
				sprintf(cell_start,"J%d",iRow);
				pRange = excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.SetValue(COleVariant(ss));
				excel_range.SetVerticalAlignment(COleVariant((long)-4108));
				excel_range.ReleaseDispatch();
				
				if(node_arr[i]->IsHasRestrict(0,Z_ROT_RESTRICT))
					ss.Printf("%s","  FIXED");
				else
					ss.Printf("%s","   FREE");
				sprintf(cell_start,"K%d",iRow);
				pRange = excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.SetValue(COleVariant(ss));
				excel_range.SetVerticalAlignment(COleVariant((long)-4108));
				excel_range.ReleaseDispatch();
				
				ss.Printf("  %d",pModule->iNo);
				sprintf(cell_start,"L%d",iRow);
				pRange = excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.SetValue(COleVariant(ss));
				excel_range.SetVerticalAlignment(COleVariant((long)-4108));
				excel_range.ReleaseDispatch();
			}
			else if(node_arr[i]->GetRestrictWord(pPrevModule->handle)!=node_arr[i]->GetRestrictWord(pModule->handle))
			{	//ǰ������ģ�ͽڵ�Լ��״����ͬ
				if(bFirstLine)
				{
					sprintf(cRangeInfo,"%5d %s  %8.3f  %8.3f  %8.3f %s",node_arr[i]->point_i,(char*)ss,
						node_arr[i]->Position(false).x/1000,node_arr[i]->Position(false).y/1000,node_arr[i]->Position(false).z/1000,ss);
					bFirstLine=FALSE;
				}
				else
					sprintf(cRangeInfo,"                                               %s",(char*)ss);
				//��֪���������ǵ�
				ss.Copy("");
				if(node_arr[i]->IsHasRestrict(0,X_RESTRICT))
					ss.Append("  FIXED");
				else
					ss.Append("   FREE");
				if(node_arr[i]->IsHasRestrict(0,Y_RESTRICT))
					ss.Append("  FIXED");
				else
					ss.Append("   FREE");
				if(node_arr[i]->IsHasRestrict(0,Z_RESTRICT))
					ss.Append("  FIXED");
				else
					ss.Append("   FREE");
				char module_no[6];
				sprintf(module_no,"  %d",pModule->iNo);
				ss.Append(module_no);
			}
			else
			{	//ǰ������ģ�ͽڵ�Լ��״����ͬ
				char module_no[6];
				sprintf(module_no,",%d",pModule->iNo);
				ss.Append(module_no);
			}
			pPrevModule=pModule;
		}
		if(bFirstLine)
		{
			sprintf(cRangeInfo,"%5d %s  %8.3f  %8.3f  %8.3f %s",node_arr[i]->point_i,(char*)ss,
				node_arr[i]->Position(false).x/1000,node_arr[i]->Position(false).y/1000,node_arr[i]->Position(false).z/1000,ss);
			bFirstLine=FALSE;
			
			sprintf(cell_start,"A%d",iRow);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%5d",node_arr[i]->point_i);
			excel_range.SetValue(COleVariant(ss));
			excel_range.SetVerticalAlignment(COleVariant((long)-4108));
			excel_range.ReleaseDispatch();
			
			sprintf(cell_start,"C%d",iRow);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%8.3f",node_arr[i]->Position(false).x/1000);
			excel_range.SetValue(COleVariant(ss));
			excel_range.SetVerticalAlignment(COleVariant((long)-4108));
			excel_range.ReleaseDispatch();
			
			sprintf(cell_start,"D%d",iRow);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%8.3f",node_arr[i]->Position(false).y/1000);
			excel_range.SetValue(COleVariant(ss));
			excel_range.SetVerticalAlignment(COleVariant((long)-4108));
			excel_range.ReleaseDispatch();
			
			sprintf(cell_start,"E%d",iRow);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%8.3f",node_arr[i]->Position(false).z/1000);
			excel_range.SetValue(COleVariant(ss));
			excel_range.SetVerticalAlignment(COleVariant((long)-4108));
			excel_range.ReleaseDispatch();
		}
		iRow++;
		iLineNum++;
	}
	delete []node_arr;
	pWorksheet->Release();
		
	//����ҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("Weight Bill of Tower");
	
	strcpy(cell_start,"A1");
	strcpy(cell_end,"F2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Part ��: Weight Bill of Tower"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	//borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)-4119));	//xlDouble =-4119;
	border.SetWeight(COleVariant((short)4));	//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	excel_range.DetachDispatch();
	pRange->Release();
		
	strcpy(cell_start,"A3");
	strcpy(cell_end,"B4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Working Module\n(Body/Leg)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"C3");
	strcpy(cell_end,"D4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Total Weight(kg)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"E3");
	strcpy(cell_end,"F4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Total Gravity(kN)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
		
	i=0;
	iRow=5;
	for(TOWERWEIGHT *pWeight=postprocessor.TowerWeight.GetFirst();pWeight;pWeight=postprocessor.TowerWeight.GetNext())
	{
		CXhChar50 csMark;
		CXhChar16 cfgStr;
		if(pWeight->iBody0Leg1==0)	//����
		{
			cfgStr.Printf("%d",pWeight->cfg_no);
			cfgStr.ResizeLength(3,' ',true);
			sprintf(csMark," %s           ",(char*)cfgStr,pWeight->cfg_no);
		}
		else						//����
		{
			CFGWORD cfgword(pWeight->cfg_no);
			cfgword.GetLegScopeStr(cfgStr,17,true);
			cfgStr.ResizeLength(4,' ',true);
			sprintf(csMark," %s          ",(char*)cfgStr);//(pWeight->cfg_no-1)/8+1,(pWeight->cfg_no-1)%8+'A');
		}
		int str_len=strlen(csMark);
		if(str_len<15)
		{
			for(int j=15;j>=0;j--)
			{
				if(j-15+str_len<0)
					csMark[j]=' ';
				else
					csMark[j]=csMark[j-15+str_len];
			}
		}
		
		cell_start.Printf("A%d",iRow);
		cell_end.Printf("B%d",iRow);
		pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.Merge(COleVariant((BYTE)1));
		ss.Printf("%s",(char*)csMark);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();
		
		cell_start.Printf("C%d",iRow);
		cell_end.Printf("D%d",iRow);
		pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.Merge(COleVariant((BYTE)1));
		ss.Printf("%8.2f",pWeight->netWeight);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();
		
		cell_start.Printf("E%d",iRow);
		cell_end.Printf("F%d",iRow);
		pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.Merge(COleVariant((BYTE)1));
		ss.Printf("%8.3f",pWeight->netWeight*0.0098);
		excel_range.SetValue(COleVariant(ss));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();
		
		iRow++;
	}
	pWorksheet->Release();
		
	//����ҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("Foundation Node Reaction");
		
	strcpy(cell_start,"A1");
	strcpy(cell_end,"J2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Part ��: Foundation Node Reaction"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	//borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)-4119));	//xlDouble =-4119;
	border.SetWeight(COleVariant((short)4));	//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	excel_range.DetachDispatch();
	pRange->Release();

	strcpy(cell_start,"A3");
	strcpy(cell_end,"A4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Node\nLabel"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"B3");
	strcpy(cell_end,"D3");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("    Working Phase)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"B4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant("Case"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"C4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant("Module"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"D4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant("Leg"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(12.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"E3");
	strcpy(cell_end,"E4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Rx(kN)\n     Design"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(7.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"F3");
	strcpy(cell_end,"F4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Ry(kN)\n     Design"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(7.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"G3");
	strcpy(cell_end,"G4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Rz(kN)\n     Design"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(7.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"H3");
	strcpy(cell_end,"H4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Rx(kN)\n  Std."));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(7.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"I3");
	strcpy(cell_end,"I4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Ry(kN)\n  Std."));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(7.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"J3");
	strcpy(cell_end,"J4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Rz(kN)\n  Std."));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(7.0));
	excel_range.ReleaseDispatch();
		
	int nReaction=postprocessor.Reaction.GetNodeNum();
	REACTION_PTR *react_arr=new REACTION_PTR[nReaction];
	i=0;
	iLineNum=0;
	for(BASENODE_REACTION *pReact=postprocessor.Reaction.GetFirst();pReact;pReact=postprocessor.Reaction.GetNext())
	{
		pReact->status=0;
		react_arr[i]=pReact;
		i++;
	}
	//CQuickSort<REACTION_PTR>::QuickSort(react_arr,nReaction,compare_func3);
	//�˴����ÿ�������������������ֻ�ʱ����ݹ���ȳ���ת���������򣬽�������Ч�ʼ����½����������� wjh-2013-5-1
	CHeapSort<REACTION_PTR>::HeapSort(react_arr,nReaction,compare_func3);
	long prev_point_i=0,nCount=0;;
	double Rzmax=0,Rzmin=0;
	//CXhChar500 ss;
	for(i=0;i<nReaction;i++)
	{
		if(prev_point_i==0)
		{
			nCount++;
			/*
			if(g_sysPara.bCalStdFoundationForce)
			{
				if(react_arr[i]->reaction_force_std.z>0)
					Rzmax=react_arr[i]->reaction_force_std.z;
				else
					Rzmin=react_arr[i]->reaction_force_std.z;
			}
			else
			*/
			{
				if(react_arr[i]->reaction_force_design.z>0)
					Rzmax=react_arr[i]->reaction_force_design.z;
				else
					Rzmin=react_arr[i]->reaction_force_design.z;
			}
			prev_point_i=react_arr[i]->pNode->point_i;

			sprintf(cell_start,"A%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%5d",react_arr[i]->pNode->point_i);
			excel_range.SetValue(COleVariant(ss));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			excel_range.DetachDispatch();
			pRange->Release();

			sprintf(cell_start,"B%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%2d",react_arr[i]->pStatus->iNo);
			excel_range.SetValue(COleVariant(ss));
			excel_range.DetachDispatch();
			pRange->Release();

			sprintf(cell_start,"C%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%2d",react_arr[i]->pModule->iNo);
			excel_range.SetValue(COleVariant(ss));
			excel_range.DetachDispatch();
			pRange->Release();

			sprintf(cell_start,"D%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%2d-%2d-%2d-%2d",react_arr[i]->leg_no[0],react_arr[i]->leg_no[1],
										react_arr[i]->leg_no[2],react_arr[i]->leg_no[3]);
			excel_range.SetValue(COleVariant(ss));
			excel_range.DetachDispatch();
			pRange->Release();
			
			sprintf(cell_start,"E%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%8.3f",react_arr[i]->reaction_force_design.x/1000);
			excel_range.SetValue(COleVariant(ss));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			excel_range.DetachDispatch();
			pRange->Release();
			
			sprintf(cell_start,"F%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%8.3f",react_arr[i]->reaction_force_design.y/1000);
			excel_range.SetValue(COleVariant(ss));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			excel_range.DetachDispatch();
			pRange->Release();
			
			sprintf(cell_start,"G%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%8.3f",react_arr[i]->reaction_force_design.z/1000);
			excel_range.SetValue(COleVariant(ss));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			excel_range.DetachDispatch();
			pRange->Release();
			/*
			if(g_sysPara.bCalStdFoundationForce)
			{
				sprintf(cell_start,"H%d",iLineNum+5);
				pRange = excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				ss.Printf("%8.3f",react_arr[i]->reaction_force_std.x/1000);
				excel_range.SetValue(COleVariant(ss));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				
				sprintf(cell_start,"I%d",iLineNum+5);
				pRange = excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				ss.Printf("%8.3f",react_arr[i]->reaction_force_std.y/1000);
				excel_range.SetValue(COleVariant(ss));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				
				sprintf(cell_start,"J%d",iLineNum+5);
				pRange = excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				ss.Printf("%8.3f",react_arr[i]->reaction_force_std.z/1000);
				excel_range.SetValue(COleVariant(ss));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
			}
			*/
			iLineNum++;
		}
		else if(prev_point_i==react_arr[i]->pNode->point_i)
		{
			nCount++;
			/*
			if(g_sysPara.bCalStdFoundationForce)
			{
				if(react_arr[i]->reaction_force_std.z>Rzmax)
					Rzmax=react_arr[i]->reaction_force_std.z;
				else if(react_arr[i]->reaction_force_std.z<Rzmin)
					Rzmin=react_arr[i]->reaction_force_std.z;
			}
			else
			*/
			{
				if(react_arr[i]->reaction_force_design.z>Rzmax)
					Rzmax=react_arr[i]->reaction_force_design.z;
				else if(react_arr[i]->reaction_force_design.z<Rzmin)
					Rzmin=react_arr[i]->reaction_force_design.z;
			}
			prev_point_i=react_arr[i]->pNode->point_i;

			/*��֪������ж��Ǹ�ʲô�õ�
			if(i==nReaction-1&&nCount>2)	//���һ��������������
			sprintf(cRangeInfo,"                    +Rzmax=%8.3f(kN),-Rzmax=%8.3f(kN)",Rzmax/1000,Rzmin/1000);
			*/
			
			sprintf(cell_start,"A%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%5d",react_arr[i]->pNode->point_i);
			excel_range.SetValue(COleVariant(ss));
			excel_range.DetachDispatch();
			pRange->Release();

			sprintf(cell_start,"B%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%2d",react_arr[i]->pStatus->iNo);
			excel_range.SetValue(COleVariant(ss));
			excel_range.DetachDispatch();
			pRange->Release();

			sprintf(cell_start,"C%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%2d",react_arr[i]->pModule->iNo);
			excel_range.SetValue(COleVariant(ss));
			excel_range.DetachDispatch();
			pRange->Release();

			sprintf(cell_start,"D%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%2d-%2d-%2d-%2d",react_arr[i]->leg_no[0],react_arr[i]->leg_no[1],
										react_arr[i]->leg_no[2],react_arr[i]->leg_no[3]);
			excel_range.SetValue(COleVariant(ss));
			excel_range.DetachDispatch();
			pRange->Release();
			
			sprintf(cell_start,"E%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%8.3f",react_arr[i]->reaction_force_design.x/1000);
			excel_range.SetValue(COleVariant(ss));
			excel_range.DetachDispatch();
			pRange->Release();
			
			sprintf(cell_start,"F%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%8.3f",react_arr[i]->reaction_force_design.y/1000);
			excel_range.SetValue(COleVariant(ss));
			excel_range.DetachDispatch();
			pRange->Release();
			
			sprintf(cell_start,"G%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%8.3f",react_arr[i]->reaction_force_design.z/1000);
			excel_range.SetValue(COleVariant(ss));
			excel_range.DetachDispatch();
			pRange->Release();
			/*
			if(g_sysPara.bCalStdFoundationForce)
			{
				sprintf(cell_start,"H%d",iLineNum+5);
				pRange = excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				ss.Printf("%8.3f",react_arr[i]->reaction_force_std.x/1000);
				excel_range.SetValue(COleVariant(ss));
				excel_range.DetachDispatch();
				pRange->Release();
				
				sprintf(cell_start,"I%d",iLineNum+5);
				pRange = excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				ss.Printf("%8.3f",react_arr[i]->reaction_force_std.y/1000);
				excel_range.SetValue(COleVariant(ss));
				excel_range.DetachDispatch();
				pRange->Release();
				
				sprintf(cell_start,"J%d",iLineNum+5);
				pRange = excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				ss.Printf("%8.3f",react_arr[i]->reaction_force_std.z/1000);
				excel_range.SetValue(COleVariant(ss));
				excel_range.DetachDispatch();
				pRange->Release();
			}
			*/
			iLineNum++;
			continue;
		}
		else	//ת���ڵ�
		{
		/*��֪������ж��Ǹ�ʲô�õ�
		if(nCount>2)
		sprintf(cRangeInfo,"                    +Rzmax=%8.3f(kN),-Rzmax=%8.3f(kN)",Rzmax/1000,Rzmin/1000);
			*/
			Rzmax=Rzmin=0;
			nCount=1;
			/*
			if(g_sysPara.bCalStdFoundationForce)
			{
				if(react_arr[i]->reaction_force_std.z>0)
					Rzmax=react_arr[i]->reaction_force_std.z;
				else
					Rzmin=react_arr[i]->reaction_force_std.z;
			}
			else
			*/
			{
				if(react_arr[i]->reaction_force_design.z>0)
					Rzmax=react_arr[i]->reaction_force_design.z;
				else
					Rzmin=react_arr[i]->reaction_force_design.z;
			}

			prev_point_i=react_arr[i]->pNode->point_i;

			sprintf(cell_start,"A%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%5d",react_arr[i]->pNode->point_i);
			excel_range.SetValue(COleVariant(ss));
			excel_range.DetachDispatch();
			pRange->Release();

			sprintf(cell_start,"B%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%2d",react_arr[i]->pStatus->iNo);
			excel_range.SetValue(COleVariant(ss));
			excel_range.DetachDispatch();
			pRange->Release();

			sprintf(cell_start,"C%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%2d",react_arr[i]->pModule->iNo);
			excel_range.SetValue(COleVariant(ss));
			excel_range.DetachDispatch();
			pRange->Release();

			sprintf(cell_start,"D%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%2d-%2d-%2d-%2d",react_arr[i]->leg_no[0],react_arr[i]->leg_no[1],
										react_arr[i]->leg_no[2],react_arr[i]->leg_no[3]);
			excel_range.SetValue(COleVariant(ss));
			excel_range.DetachDispatch();
			pRange->Release();
			
			sprintf(cell_start,"E%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%8.3f",react_arr[i]->reaction_force_design.x/1000);
			excel_range.SetValue(COleVariant(ss));
			excel_range.DetachDispatch();
			pRange->Release();
			
			sprintf(cell_start,"F%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%8.3f",react_arr[i]->reaction_force_design.y/1000);
			excel_range.SetValue(COleVariant(ss));
			excel_range.DetachDispatch();
			pRange->Release();
			
			sprintf(cell_start,"G%d",iLineNum+5);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			ss.Printf("%8.3f",react_arr[i]->reaction_force_design.z/1000);
			excel_range.SetValue(COleVariant(ss));
			excel_range.DetachDispatch();
			pRange->Release();
			/*
			if(g_sysPara.bCalStdFoundationForce)
			{
				sprintf(cell_start,"H%d",iLineNum+5);
				pRange = excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				ss.Printf("%8.3f",react_arr[i]->reaction_force_std.x/1000);
				excel_range.SetValue(COleVariant(ss));
				excel_range.DetachDispatch();
				pRange->Release();
				
				sprintf(cell_start,"I%d",iLineNum+5);
				pRange = excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				ss.Printf("%8.3f",react_arr[i]->reaction_force_std.y/1000);
				excel_range.SetValue(COleVariant(ss));
				excel_range.DetachDispatch();
				pRange->Release();
				
				sprintf(cell_start,"J%d",iLineNum+5);
				pRange = excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				ss.Printf("%8.3f",react_arr[i]->reaction_force_std.z/1000);
				excel_range.SetValue(COleVariant(ss));
				excel_range.DetachDispatch();
				pRange->Release();
			}
			*/
			iLineNum++;
		}
	}
	delete []react_arr;
	pWorksheet->Release();
		
	//Part �� & Part ��
	typedef CElement* ELEM_PTR;
	int nElemNum=postprocessor.hashElems.GetNodeNum();
	ELEM_PTR *elem_arr=new ELEM_PTR[nElemNum];
	i=0;
	for(CElement *pElem=postprocessor.hashElems.GetFirst();pElem;pElem=postprocessor.hashElems.GetNext())
	{
		elem_arr[i]=pElem;
		i++;
	}
	CQuickSort<ELEM_PTR>::QuickSort(elem_arr,nElemNum,compare_func2);
		
	//����ҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("Member Basic Properties");
		
	strcpy(cell_start,"A1");
	strcpy(cell_end,"M2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Part ��: Member Basic Properties"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	//borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)-4119));	//xlDouble =-4119;
	border.SetWeight(COleVariant((short)4));	//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	excel_range.DetachDispatch();
	pRange->Release();

	strcpy(cell_start,"A3");
	strcpy(cell_end,"A5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("   Member\nLabel\n"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"B3");
	strcpy(cell_end,"B5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("   Member\nType\n"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"C3");
	strcpy(cell_end,"C5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("   Member\nSize\n"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(8.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"D3");
	strcpy(cell_end,"D5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Material\n  & Size\n    Code"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"E3");
	strcpy(cell_end,"E5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("  Unit\nWeight\n(kg/m)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"F3");
	strcpy(cell_end,"F5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("  Total\n Length\n(m)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"G3");
	strcpy(cell_end,"G5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Gross\n  Area\n(cm2)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"H3");
	strcpy(cell_end,"H5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Coef.of\nHole_Out\n"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"I3");
	strcpy(cell_end,"I5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Radius of\n Gyration\nRx(cm)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"J3");
	strcpy(cell_end,"J5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Radius of\n Gyration\nRy0(cm)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"K3");
	strcpy(cell_end,"K5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Section\nModulus\nWx0(cm3)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"L3");
	strcpy(cell_end,"L5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("   Bolt\n  Info.\n"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"M3");
	strcpy(cell_end,"M5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("  Bear\n  Load\n  Coef"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
		
	for(i=0;i<nElemNum;i++)
	{
		long No[2];
		No[0]=min(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		No[1]=max(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		double Rx=elem_arr[i]->pOrgPole->GetWorkR(0)/10;
		double Ry0=elem_arr[i]->pOrgPole->GetWorkR(1)/10;
		double Wx=elem_arr[i]->pOrgPole->GetWorkW(0)/1000;
		double area=elem_arr[i]->pOrgPole->GetArea()/100;
		double length=elem_arr[i]->Length()/1000;	// ��λm
		double theroy_weight=globalLibrary.GetWeightOf1Meter(
			elem_arr[i]->pOrgPole->size_wide,elem_arr[i]->pOrgPole->size_thick,0,
			elem_arr[i]->pOrgPole->size_idClassType,elem_arr[i]->pOrgPole->size_cSubClassType);
		double fStatMatNo = elem_arr[i]->pOrgPole->m_uStatMatNo;
		double fholeOut = elem_arr[i]->design_info.hole_out;
		CXhChar50 ss_ls,ss_ls_modu,ss_spec,ss_type;
		strcpy(ss_type,elem_arr[i]->pOrgPole->GetBriefPartTypeMark());
		sprintf(ss_spec,"%3.0fX%2.0f%C",elem_arr[i]->pOrgPole->GetWidth(),elem_arr[i]->pOrgPole->GetThick(),elem_arr[i]->pOrgPole->cMaterial);
		if(elem_arr[i]->pStart==elem_arr[i]->pOrgPole->pStart)	//ʼ�˵�Ԫ
		{
			ss_ls.Printf("%sM%dX%d",elem_arr[i]->pOrgPole->connectStart.grade,elem_arr[i]->pOrgPole->connectStart.d,elem_arr[i]->pOrgPole->connectStart.wnSummBoltN);
			ss_ls_modu.Printf("%5.3f",elem_arr[i]->pOrgPole->connectStart.bearloadcoef);
		}
		else if(elem_arr[i]->pEnd==elem_arr[i]->pOrgPole->pEnd)	//ĩ�˵�Ԫ
		{
			ss_ls.Printf("%sM%dX%d",elem_arr[i]->pOrgPole->connectEnd.grade,elem_arr[i]->pOrgPole->connectEnd.d,elem_arr[i]->pOrgPole->connectEnd.wnSummBoltN);
			ss_ls_modu.Printf("%5.3f",elem_arr[i]->pOrgPole->connectEnd.bearloadcoef);
		}
		else
		{
			sprintf(ss_ls,"Middle Elem.");
			ss_ls_modu.Empty();
		}
		sprintf(cell_start,"A%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%4d-%4d",No[0],No[1]);
		excel_range.SetValue(COleVariant(ss));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"B%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%s",(char*)ss_type);
		excel_range.SetValue(COleVariant(ss));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"C%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%s",(char*)ss_spec);
		excel_range.SetValue(COleVariant(ss));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"D%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%8.3f",fStatMatNo);
		excel_range.SetValue(COleVariant(ss));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"E%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%8.3f",theroy_weight);
		excel_range.SetValue(COleVariant(ss));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"F%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%6.2f",length);
		excel_range.SetValue(COleVariant(ss));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"G%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%6.2f",area);
		excel_range.SetValue(COleVariant(ss));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"H%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%8.3f",fholeOut);
		excel_range.SetValue(COleVariant(ss));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"I%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%8.3f",Rx);
		excel_range.SetValue(COleVariant(ss));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"J%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%8.3f",Ry0);
		excel_range.SetValue(COleVariant(ss));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"K%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%8.3f",Wx);
		excel_range.SetValue(COleVariant(ss));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"L%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%s",(char*)ss_ls);
		excel_range.SetValue(COleVariant(ss));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"M%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		ss.Printf("%s",(char*)ss_ls_modu);
		excel_range.SetValue(COleVariant(ss));
		excel_range.DetachDispatch();
		pRange->Release();
	}
	pWorksheet->Release();
		
	//�ڰ�ҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("Member Design Information");

	strcpy(cell_start,"A1");
	strcpy(cell_end,"U2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Part ��: Member Design Information"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	//borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)-4119));	//xlDouble =-4119;
	border.SetWeight(COleVariant((short)4));	//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	excel_range.DetachDispatch();
	pRange->Release();
		
	strcpy(cell_start,"A3");
	strcpy(cell_end,"A5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("  Member\n   Label\n"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"B3");
	strcpy(cell_end,"B5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Control\n Criter-\n     ion"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"C3");
	strcpy(cell_end,"C5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("   Max.\n  Tens.\n(Mpa)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"D3");
	strcpy(cell_end,"F3");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.SetValue(COleVariant("    Working Phase"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"D4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.SetValue(COleVariant("Case"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(5.0));
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"E4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.SetValue(COleVariant("Module"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(5.0));
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"F4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.SetValue(COleVariant("Leg"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"G3");
	strcpy(cell_end,"G5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("    Max.\n   Comp.\n(Mpa)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(7.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"H3");
	strcpy(cell_end,"J3");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.SetValue(COleVariant("    Working Phase"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"H4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.SetValue(COleVariant("Case"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(5.0));
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"I4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.SetValue(COleVariant("Module"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(5.0));
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"J4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.SetValue(COleVariant("Leg"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"K3");
	strcpy(cell_end,"K5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Coef. of\nStrength\nDecrease"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"L3");
	strcpy(cell_end,"L5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Coef. of\nStability\n     (��)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"M3");
	strcpy(cell_end,"M5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Coef.\n   of \n   mN"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"N3");
	strcpy(cell_end,"N5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("  L/r\n Sel.\nStyle"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"O3");
	strcpy(cell_end,"O5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("K*Lo/r\n\n(Mpa)"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"P3");
	strcpy(cell_end,"P5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("Ulti-\n mate\nLo/r"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"Q3");
	strcpy(cell_end,"Q5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant(" Coef.of\n   Bear\n   Load"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(9.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"R3");
	strcpy(cell_end,"R5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("  Begin\n   Rot.\n  Rest."));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"S3");
	strcpy(cell_end,"S5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("   End\n  Rot.\n Rest."));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"T3");
	strcpy(cell_end,"T5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("  Begin\n  Force\n  Type"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"U3");
	strcpy(cell_end,"U5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetValue(COleVariant("  End\nForce\n Type"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();

	for(i=0;i<nElemNum;i++)
	{
		long No[2];
		No[0]=min(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		No[1]=max(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		char ss[12];
		CXhChar100 tmpValue;
		if(elem_arr[i]->m_iCtrlRule==1)
			strcpy(ss,"K*Lo/r");
		else if(elem_arr[i]->m_iCtrlRule==2)
			strcpy(ss,"TENS.");
		else if(elem_arr[i]->m_iCtrlRule==3)
			strcpy(ss,"COMP.");
		else if(elem_arr[i]->m_iCtrlRule==4)
			strcpy(ss,"STAB.");
		else if(elem_arr[i]->m_iCtrlRule==5)
			strcpy(ss,"M&S.Code");
		else if(elem_arr[i]->m_iCtrlRule==6)
			strcpy(ss,"BOLT");
		else //if(elem_arr[i]->pOrgPole->m_iCtrlRule==0)
			strcpy(ss,"NONE");
		if(rpt_type==1&&elem_arr[i]->m_iCtrlRule>0
			&&elem_arr[i]->m_iCtrlRule!=5)
			ss[0]='>';
		CString sRx_Ry0,s1_1,s1_2,s1_3,s2_1,s2_2,s2_3,s3,s4,s5,s6;
		CLDSModule *pModule=Ta.Module.FromHandle(elem_arr[i]->MaxTension.hModule);
		CWorkStatus *pStatus=Ta.WorkStatus.FromHandle(elem_arr[i]->MaxTension.hWorkStatus.Id());
		if(pModule&&pStatus)
		{
			s1_1.Format("%2d",pStatus->iNo);
			s1_2.Format("%2d",pModule->iNo);
			s1_3.Format("%2d-%2d-%2d-%2d",
				elem_arr[i]->MaxTension.leg_no[0],elem_arr[i]->MaxTension.leg_no[1],
				elem_arr[i]->MaxTension.leg_no[2],elem_arr[i]->MaxTension.leg_no[3]);
			/*s1.Format("%2d,%2d,%2d-%2d-%2d-%2d",pStatus->iNo,pModule->iNo,
				elem_arr[i]->MaxTension.leg_no[0],elem_arr[i]->MaxTension.leg_no[1],
				elem_arr[i]->MaxTension.leg_no[2],elem_arr[i]->MaxTension.leg_no[3]);*/
		}
		else
		{
			s1_1.Format("0");
			s1_2.Format("0");
			s1_3.Format("0- 0- 0- 0");
			//s1.Format(" 0, 0, 0- 0- 0- 0");
		}
		pModule=Ta.Module.FromHandle(elem_arr[i]->MaxCompression.hModule);
		pStatus=Ta.WorkStatus.FromHandle(elem_arr[i]->MaxCompression.hWorkStatus.Id());
		if(pModule&&pStatus)
		{
			s2_1.Format("%2d",pStatus->iNo);
			s2_2.Format("%2d",pModule->iNo);
			s2_3.Format("%2d-%2d-%2d-%2d",elem_arr[i]->MaxCompression.leg_no[0],elem_arr[i]->MaxCompression.leg_no[1],
							  elem_arr[i]->MaxCompression.leg_no[2],elem_arr[i]->MaxCompression.leg_no[3]);
		}
		else
		{
			s2_1.Format("0");
			s2_2.Format("0");
			s2_3.Format("0- 0- 0- 0");
		}
		if(elem_arr[i]->LamdaRType()==0)
			sRx_Ry0="Rx ";
		else
			sRx_Ry0="Ryo";
		if(elem_arr[i]->design_info.bStartRotRest)
			s3="PARTIAL";
		else
			s3="   FREE";
		if(elem_arr[i]->design_info.bEndRotRest)
			s4="PARTIAL";
		else
			s4="   FREE";
		if(elem_arr[i]->start_force_type==CENTRIC_FORCE)
			s5="CENTRIC";
		else
			s5=" ECCEN.";
		if(elem_arr[i]->end_force_type==CENTRIC_FORCE)
			s6="CENTRIC";
		else
			s6=" ECCEN.";
		double An=elem_arr[i]->pOrgPole->GetArea()-elem_arr[i]->design_info.hole_out*elem_arr[i]->design_info.hole_d*elem_arr[i]->pOrgPole->GetThick();
		CXhChar16 caserule;
		if(elem_arr[i]->design_info.iDangerousRule==1)
			strcpy(caserule,"��");
		else if(elem_arr[i]->design_info.iDangerousRule==2)
			strcpy(caserule," T");
		else if(elem_arr[i]->design_info.iDangerousRule==3)
			strcpy(caserule," C");
		else if(elem_arr[i]->design_info.iDangerousRule==4)
			strcpy(caserule," S");
		else
			strcpy(caserule,"  ");
		/*
		if(No[0]==elem_arr[i]->pStart->point_i)
		{
			sprintf(cRangeInfo,"%4d-%4d %s%8.3f %s %8.3f %s   %4.2f/%4.2f %8.3f %6.2f   %s %5.0f%7.0f %5.3f@%s %s %s %s %s",No[0],No[1],ss,
				elem_arr[i]->MaxTension.N/An,s1,elem_arr[i]->MaxCompression.N/An,s2,
				elem_arr[i]->design_info.fCoefTensStrengthDecrease,elem_arr[i]->design_info.fCoefCompStrengthDecrease,
				elem_arr[i]->design_info.fCoefStablity,elem_arr[i]->design_info.mN,sRx_Ry0,
				elem_arr[i]->design_info.Lo_r,elem_arr[i]->design_info.UltimateLo_r,elem_arr[i]->design_info.fCoefOfBearLoad,caserule,s3,s4,s5,s6);
		}
		else
		{
			sprintf(cRangeInfo,"%4d-%4d %s%8.3f %s %8.3f %s   %4.2f/%4.2f %8.3f %6.2f   %s %5.0f%7.0f %5.3f@%s %s %s %s %s",No[0],No[1],ss,
				elem_arr[i]->MaxTension.N/An,s1,elem_arr[i]->MaxCompression.N/An,s2,
				elem_arr[i]->design_info.fCoefTensStrengthDecrease,elem_arr[i]->design_info.fCoefCompStrengthDecrease,
				elem_arr[i]->design_info.fCoefStablity,elem_arr[i]->design_info.mN,sRx_Ry0,
				elem_arr[i]->design_info.Lo_r,elem_arr[i]->design_info.UltimateLo_r,elem_arr[i]->design_info.fCoefOfBearLoad,caserule,s4,s3,s6,s5);
		}
		*/
		sprintf(cell_start,"A%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		tmpValue.Printf("%4d-%4d",No[0],No[1]);
		excel_range.SetValue(COleVariant(tmpValue));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"B%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		tmpValue.Printf("%s",ss);
		excel_range.SetValue(COleVariant(tmpValue));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"C%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		tmpValue.Printf("%8.3f",elem_arr[i]->MaxTension.N/An);
		excel_range.SetValue(COleVariant(tmpValue));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"D%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		tmpValue.Printf("%s",s1_1);
		excel_range.SetValue(COleVariant(tmpValue));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"E%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		tmpValue.Printf("%s",s1_2);
		excel_range.SetValue(COleVariant(tmpValue));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"F%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		tmpValue.Printf("%s",s1_3);
		excel_range.SetValue(COleVariant(tmpValue));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"G%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		tmpValue.Printf("%8.3f",elem_arr[i]->MaxCompression.N/An);
		excel_range.SetValue(COleVariant(tmpValue));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"H%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		tmpValue.Printf("%s",s2_1);
		excel_range.SetValue(COleVariant(tmpValue));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"I%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		tmpValue.Printf("%s",s2_2);
		excel_range.SetValue(COleVariant(tmpValue));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"J%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		tmpValue.Printf("%s",s2_3);
		excel_range.SetValue(COleVariant(tmpValue));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"K%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		tmpValue.Printf("%4.2f/%4.2f",elem_arr[i]->design_info.fCoefTensStrengthDecrease,elem_arr[i]->design_info.fCoefCompStrengthDecrease);
		excel_range.SetValue(COleVariant(tmpValue));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"L%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		tmpValue.Printf("%8.3f",elem_arr[i]->design_info.fCoefStablity);
		excel_range.SetValue(COleVariant(tmpValue));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"M%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		tmpValue.Printf("%6.2f",elem_arr[i]->design_info.mN);
		excel_range.SetValue(COleVariant(tmpValue));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"N%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		tmpValue.Printf("%s",sRx_Ry0);
		excel_range.SetValue(COleVariant(tmpValue));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"O%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		tmpValue.Printf("%5.0f",elem_arr[i]->Lamda());
		excel_range.SetValue(COleVariant(tmpValue));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"P%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		tmpValue.Printf("%7.0f",elem_arr[i]->design_info.UltimateLo_r);
		excel_range.SetValue(COleVariant(tmpValue));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.DetachDispatch();
		pRange->Release();

		sprintf(cell_start,"Q%d",i+6);
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		tmpValue.Printf("%5.3f@%s",elem_arr[i]->design_info.fCoefEffic,(char*)caserule);
		excel_range.SetValue(COleVariant(tmpValue));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.DetachDispatch();
		pRange->Release();

		if(No[0]==elem_arr[i]->pStart->point_i)
		{
			sprintf(cell_start,"R%d",i+6);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			tmpValue.Printf("%s",s3);
			excel_range.SetValue(COleVariant(tmpValue));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			excel_range.DetachDispatch();
			pRange->Release();
			
			sprintf(cell_start,"S%d",i+6);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			tmpValue.Printf("%s",s4);
			excel_range.SetValue(COleVariant(tmpValue));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			excel_range.DetachDispatch();
			pRange->Release();
			
			sprintf(cell_start,"T%d",i+6);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			tmpValue.Printf("%s",s5);
			excel_range.SetValue(COleVariant(tmpValue));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			excel_range.DetachDispatch();
			pRange->Release();
			
			sprintf(cell_start,"U%d",i+6);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			tmpValue.Printf("%s",s6);
			excel_range.SetValue(COleVariant(tmpValue));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			excel_range.DetachDispatch();
			pRange->Release();
		}
		else
		{
			
			sprintf(cell_start,"R%d",i+6);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			tmpValue.Printf("%s",s4);
			excel_range.SetValue(COleVariant(tmpValue));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			excel_range.DetachDispatch();
			pRange->Release();
			
			sprintf(cell_start,"S%d",i+6);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			tmpValue.Printf("%s",s3);
			excel_range.SetValue(COleVariant(tmpValue));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			excel_range.DetachDispatch();
			pRange->Release();
			
			sprintf(cell_start,"T%d",i+6);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			tmpValue.Printf("%s",s6);
			excel_range.SetValue(COleVariant(tmpValue));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			excel_range.DetachDispatch();
			pRange->Release();
			
			sprintf(cell_start,"U%d",i+6);
			pRange = excel_sheet.GetRange(COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			tmpValue.Printf("%s",s5);
			excel_range.SetValue(COleVariant(tmpValue));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			excel_range.DetachDispatch();
			pRange->Release();
		}
		

	}
	delete []elem_arr;
	
	pWorksheet->Release();
	excel_app.ReleaseDispatch();
	CXhChar500 save_file_name;
	save_file_name.Copy(file_name);
	//if(bCheckPart)
		strcat(save_file_name,".xls");	//_DetailCheck
	//else
	//	strcat(save_file_name,".xls");	//_DetailReport
	DeleteFile(save_file_name);
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND,   VT_ERROR);  
	excel_workbook.SaveAs(COleVariant(save_file_name),covOptional,covOptional, covOptional,covOptional,covOptional,0,covOptional,covOptional,covOptional,covOptional);
	
	excel_sheets.ReleaseDispatch();
	excel_workbook.ReleaseDispatch();
	excel_workbooks.ReleaseDispatch();
	excel_app.ReleaseDispatch();
	excel_app.Quit();

	pCmdLine->PromptStrInCmdLine("Ӣ�İ���ϸ�����ļ������ɣ�");
}

void CreateDetailRptExcelFileFromReportByCN(char *file_name)
{
	LPDISPATCH pDisp;
	LPUNKNOWN pUnk;
	CLSID clsid;
	_Application excel_app;
	CExcelOperObject::ExcelCLSIDFromProgID(clsid);
	if(::GetActiveObject(clsid, NULL, &pUnk) == S_OK) 
	{
		VERIFY(pUnk->QueryInterface(IID_IDispatch,(void**) &pDisp) == S_OK);
		excel_app.AttachDispatch(pDisp);
		pUnk->Release();
	} 
	else if(!excel_app.CreateDispatch(clsid)) 
		AfxMessageBox("Excel program not found");
	
	CWnd* pWnd = CWnd::FindWindow("XLMAIN", NULL);
	if (pWnd != NULL) 
	{
		pWnd->ShowWindow(SW_SHOWNORMAL);
		pWnd->UpdateWindow();
		pWnd->BringWindowToTop();
	}

	LPDISPATCH pWorkbooks;
	CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	int rpt_type=1;
	/*if(strstr(file_name,"Check")!=NULL)
		rpt_type=1;	//����
	else
		rpt_type=0;	//��ѡ*/
	
	Sheets excel_sheets;
	Workbooks excel_workbooks;
	_Workbook excel_workbook;
	_Worksheet excel_sheet;
	Range excel_range;
	LPDISPATCH pRange;
	Borders borders;
	Border border;
	FontLegacy font;
	char cRangeInfo[500];//ÿ�е�����
	int  i=0,iRow=0,iLineNum=0;//�к�
	COleVariant covTrue((short)TRUE),covFalse((short)FALSE);
	excel_app.SetSheetsInNewWorkbook(9);	//�����ɹ�������Ĭ�Ϲ������������
	VERIFY(pWorkbooks = excel_app.GetWorkbooks());
	excel_workbooks.AttachDispatch(pWorkbooks);
	
	LPDISPATCH pWorkbook = excel_workbooks.Add(); // Save the pointer for
	excel_workbook.AttachDispatch(pWorkbook);
	LPDISPATCH pWorksheets = excel_workbook.GetWorksheets();
	ASSERT(pWorksheets != NULL);
	excel_sheets.AttachDispatch(pWorksheets);
	CXhChar16 cell_start;
	cell_start.Printf("%s","A1");
	CXhChar16 cell_end;
	cell_end.Printf("%s","A1");
	CXhChar100 cell,ss;
	int iSheet=1;
	LPDISPATCH pWorksheet;

	//��һҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("��ƹ��");
	//��������
	int index=0;
	CVariant2dArray mapPage1(21,9);//���ݵĶ�ά����

	mapPage1.SetValueAt(0,0,COleVariant("*******************************************************************************"));
	mapPage1.SetValueAt(1,0,COleVariant("*                                                                             *"));
	mapPage1.SetValueAt(2,0,COleVariant("*    �����������������  - (C) 2002-2013 �����ź��������Ƽ����޹�˾         *"));
	mapPage1.SetValueAt(3,0,COleVariant("*                                                                             *"));
	mapPage1.SetValueAt(4,0,COleVariant("*******************************************************************************"));
	sprintf(cRangeInfo,"���̴��� : %s",Ta.m_sPrjCode);
	mapPage1.SetValueAt(6,0,COleVariant(cRangeInfo));
	sprintf(cRangeInfo,"�������� : %s",Ta.m_sPrjName);
	mapPage1.SetValueAt(7,0,COleVariant(cRangeInfo));
	sprintf(cRangeInfo,"�����Ա : %s",Ta.m_sOperator);
	mapPage1.SetValueAt(8,0,COleVariant(cRangeInfo));
	CTime t=CTime::GetCurrentTime();
	sprintf(cRangeInfo,"����ʱ�� : %s",t.Format("%H:%M:%S %B %d, %Y"));
	mapPage1.SetValueAt(9,0,COleVariant(cRangeInfo));
	CString product_version;
	theApp.GetProductVersion(product_version);
#ifdef __LDS_
	sprintf(cRangeInfo,"����汾 : LDS Version %s",product_version);
#else
	sprintf(cRangeInfo,"����汾 : TSA Version %s",product_version);
#endif
	mapPage1.SetValueAt(10,0,COleVariant(cRangeInfo));
	sprintf(cRangeInfo,"��Ȩ�û� : %s",Ta.m_sCompanyName);
	mapPage1.SetValueAt(11,0,COleVariant(cRangeInfo));
	if(g_sysPara.AnalysisPara.m_iNonlinearStyle==0)
		sprintf(cRangeInfo,"�ɹ����������Է���!");
	else
		sprintf(cRangeInfo,"�ɹ������˷����Է���!");
	mapPage1.SetValueAt(13,0,COleVariant(cRangeInfo));
	mapPage1.SetValueAt(15,0,COleVariant("*******************************************************************************"));
	mapPage1.SetValueAt(17,0,COleVariant("�ڢ񲿷�:��ƹ�̣��涨��"));
	IMemberDesignCode* pCode=IMemberDesignCode::FromCodeSerial(postreport.m_iMemberSizeCheckCode);
	sprintf(cRangeInfo,"��ִ�е���ƹ�̣��涨��Ϊ:%s��",pCode->CodeName());
	mapPage1.SetValueAt(18,0,COleVariant(cRangeInfo));
	mapPage1.SetValueAt(20,0,COleVariant("*******************************************************************************"));

	sprintf(cell_end,"I%d",21);
	//���ñ������
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,mapPage1.var);
	excel_sheet.DetachDispatch();
	pWorksheet->Release();

	//�ڶ�ҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("�ֲĻ�е����");

	//��������
	CVariant2dArray mapPage2(CSteelMatLibrary::GetCount()+9,9);//���ݵĶ�ά����
	
	strcpy(cell_start,"A1");
	strcpy(cell_end,"I2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage2.SetValueAt(0,0,COleVariant("�ڢ򲿷�: �ֲĻ�е����"));//������ӽ�����
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	//borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)-4119));	//xlDouble =-4119;
	border.SetWeight(COleVariant((short)4));	//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	excel_range.DetachDispatch();
	pRange->Release();

	mapPage2.SetValueAt(2,0,COleVariant("ֱ������ ��16 mm, ѡ�� \"��1��\"��"));
	mapPage2.SetValueAt(3,0,COleVariant("ֱ������ ��16~40 mm ����Q235, ѡ�� \"�ڶ���\"��"));
	mapPage2.SetValueAt(4,0,COleVariant("ֱ������ ��16~35 mm ����Q345��Q390, ѡ�� \"�ڶ���\"��"));
	mapPage2.SetValueAt(5,0,COleVariant("ֱ������ ��40~60 mm ����Q235, ѡ�� \"������\"��"));
	mapPage2.SetValueAt(6,0,COleVariant("ֱ������ ��35~50 mm ����Q345��Q390��Q420��Q460, ѡ�� \"������\"��"));
	mapPage2.SetValueAt(8,0,COleVariant("�ֲ�\n\n�ƺ�"));
	mapPage2.SetValueAt(8,1,COleVariant("�ֲ�\n��\n�ƺ�"));
	mapPage2.SetValueAt(8,2,COleVariant("����\nģ��\n(MPa)"));
	mapPage2.SetValueAt(8,3,COleVariant("��  ��\nǿ  ��\n��һ��\n(MPa)"));
	mapPage2.SetValueAt(8,4,COleVariant("��  ��\nǿ  ��\n�ڶ���\n(MPa)"));
	mapPage2.SetValueAt(8,5,COleVariant("��  ��\nǿ  ��\n������\n(MPa)"));
	mapPage2.SetValueAt(8,6,COleVariant("��  ��\nǿ  ��\n��һ��\n(MPa)"));
	mapPage2.SetValueAt(8,7,COleVariant("��  ��\nǿ  ��\n�ڶ���\n(MPa)"));
	mapPage2.SetValueAt(8,8,COleVariant("��  ��\nǿ  ��\n������\n(MPa)"));

	//�趨�����и�ʽ
	strcpy(cell_start,"A9");
	strcpy(cell_end,"I9");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();
	
	iRow=9;
	for(i=0;i<CSteelMatLibrary::GetCount();i++)
	{
		STEELMAT mat=CSteelMatLibrary::RecordAt(i);
		ss.Printf("%s",mat.mark);
		mapPage2.SetValueAt(iRow,0,COleVariant(ss));
		ss.Printf("%c",mat.cBriefMark);
		mapPage2.SetValueAt(iRow,1,COleVariant(ss));
		ss.Printf("%10.2f",mat.E);
		mapPage2.SetValueAt(iRow,2,COleVariant(ss));
		ss.Printf("%10.2f",mat.intensity[4]);
		mapPage2.SetValueAt(iRow,3,COleVariant(ss));
		ss.Printf("%10.2f",mat.intensity[5]);
		mapPage2.SetValueAt(iRow,4,COleVariant(ss));
		ss.Printf("%10.2f",mat.intensity[6]);
		mapPage2.SetValueAt(iRow,5,COleVariant(ss));
		ss.Printf("%10.2f",mat.intensity[8]);
		mapPage2.SetValueAt(iRow,6,COleVariant(ss));
		ss.Printf("%10.2f",mat.intensity[9]);
		mapPage2.SetValueAt(iRow,7,COleVariant(ss));
		ss.Printf("%10.2f",mat.intensity[10]);
		mapPage2.SetValueAt(iRow,8,COleVariant(ss));
		iRow++;
	}

	//�趨�����и�ʽ
	cell_start.Copy("A10");
	cell_end.Printf("I%d",iRow);
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((long)-4108));
	excel_range.ReleaseDispatch();
	//����
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,mapPage2.var);
	excel_sheet.DetachDispatch();
	pWorksheet->Release();

	//����ҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("��������б�");
	bool bHasAngle=false,bHasTube=false;
	for(CElement *pElem=postreport.hashElems.GetFirst();pElem;pElem=postreport.hashElems.GetNext())
	{
		if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE||pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
			bHasAngle=true;
		else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
			bHasTube=true;
		if(bHasAngle&&bHasTube)
			break;
	}
	
	//��������  �����С������
	int cycleLen = 0;
	if(bHasAngle)
		cycleLen=jgguige_N;
	if(bHasTube)
		cycleLen+=tubeguige_N;
	CVariant2dArray mapPage3(cycleLen+6,14);//���ݵĶ�ά����

	strcpy(cell_start,"A1");
	strcpy(cell_end,"N2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage3.SetValueAt(0,0,COleVariant("�ڢ󲿷�: ��������б�"));//������ӽ�����
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)-4119));	//xlDouble =-4119;
	border.SetWeight(COleVariant((short)4));	//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	excel_range.DetachDispatch();
	pRange->Release();

	strcpy(cell_start,"A3");
	strcpy(cell_end,"A5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage3.SetValueAt(2,0,COleVariant("���"));//����
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"B3");
	strcpy(cell_end,"B5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage3.SetValueAt(2,1,COleVariant("��  ��"));//����
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"C3");
	strcpy(cell_end,"C5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage3.SetValueAt(2,2,COleVariant("�� ��\n�� ��\n(cm2)"));//����
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"D3");
	strcpy(cell_end,"D5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage3.SetValueAt(2,3,COleVariant("��С��\n�� ��\n(cm)"));//����
	excel_range.ReleaseDispatch();	
	
	strcpy(cell_start,"E3");
	strcpy(cell_end,"E5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage3.SetValueAt(2,4,COleVariant("ƽ����\n�� ��\n(cm)"));//����
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"F3");
	strcpy(cell_end,"F5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage3.SetValueAt(2,5,COleVariant("�� Բ\n�� ��\n(cm)"));//����
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"G3");
	strcpy(cell_end,"G5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage3.SetValueAt(2,6,COleVariant("��  λ\n��  ��\n(kg/m)"));//����
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"H3");
	strcpy(cell_end,"H5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage3.SetValueAt(2,7,COleVariant("���"));//����
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"I3");
	strcpy(cell_end,"I5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage3.SetValueAt(2,8,COleVariant("��  ��"));//����
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"J3");
	strcpy(cell_end,"J5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage3.SetValueAt(2,9,COleVariant("��  ��\n��  ��\n(cm)"));//����
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"K3");
	strcpy(cell_end,"K5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage3.SetValueAt(2,10,COleVariant("��С��\n��  ��\n(cm)"));//����
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"L3");
	strcpy(cell_end,"L5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage3.SetValueAt(2,11,COleVariant("ƽ����\n��  ��\n(cm)"));//����
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"M3");
	strcpy(cell_end,"M5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage3.SetValueAt(2,12,COleVariant("��  Բ\n��  ��\n(cm)"));//����
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"N3");
	strcpy(cell_end,"N5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage3.SetValueAt(2,13,COleVariant("��  λ\n��  ��\n(kg/m)"));//����
	excel_range.ReleaseDispatch();

	iRow=5;
	int serial=0;

	if(bHasAngle)
	{
		for(i=0;i<jgguige_N;i++)
		{
			CXhChar16 stype;
			stype.Printf("%s","L");
			if(toupper(jgguige_table[i].cType)!='L')
				stype.Printf("%CL",jgguige_table[i].cType);
			int tmpCellNum = 0;
			if(i%2==1)
				tmpCellNum+=7;
			
			ss.Printf("%3d",i+1);
			mapPage3.SetValueAt(iRow,0+tmpCellNum,COleVariant(ss));//����
			ss.Printf("%s%3.0f*%2.0f",(char*)stype,jgguige_table[i].wing_wide,jgguige_table[i].wing_thick);
			mapPage3.SetValueAt(iRow,1+tmpCellNum,COleVariant(ss));//����
			ss.Printf("%7.3f",jgguige_table[i].area);
			mapPage3.SetValueAt(iRow,2+tmpCellNum,COleVariant(ss));//����
			ss.Printf("%6.2f",jgguige_table[i].Ry0);
			mapPage3.SetValueAt(iRow,3+tmpCellNum,COleVariant(ss));//����
			ss.Printf("%6.2f",jgguige_table[i].Rx);
			mapPage3.SetValueAt(iRow,4+tmpCellNum,COleVariant(ss));//����
			ss.Printf("%5.2f",jgguige_table[i].r);
			mapPage3.SetValueAt(iRow,5+tmpCellNum,COleVariant(ss));//����
			ss.Printf("%7.3f",jgguige_table[i].theroy_weight);
			mapPage3.SetValueAt(iRow,6+tmpCellNum,COleVariant(ss));//����

			if(i%2==1||i==jgguige_N-1)
				iRow++;
		}
		serial=jgguige_N;
	}
	if(bHasTube)
	{
		for(i=0;i<tubeguige_N;i++)
		{
			CXhChar16 stype;
			stype.Copy("��");
			int tmpCellNum = 0;
			if(i%2==1)
				tmpCellNum+=7;
			
			ss.Printf("%3d",serial+i+1);
			mapPage3.SetValueAt(iRow,0+tmpCellNum,COleVariant(ss));//����			
			ss.Printf("%s%3.0f*%2.0f",(char*)stype,tubeguige_table[i].D,tubeguige_table[i].thick);
			mapPage3.SetValueAt(iRow,1+tmpCellNum,COleVariant(ss));//����
			ss.Printf("%7.3f",tubeguige_table[i].area);
			mapPage3.SetValueAt(iRow,2+tmpCellNum,COleVariant(ss));//����
			ss.Printf("%6.2f",tubeguige_table[i].r);
			mapPage3.SetValueAt(iRow,3+tmpCellNum,COleVariant(ss));//����
			ss.Printf("%7.3f",tubeguige_table[i].theroy_weight);
			mapPage3.SetValueAt(iRow,4+tmpCellNum,COleVariant(ss));//����

			if(i%2==1||i==tubeguige_N-1)
				iRow++;
		}
	}
	//���ñ������
	strcpy(cell_start,"A1");
	sprintf(cell_end,"N%d",iRow);
	//���뷽ʽ
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();
	//����
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,mapPage3.var);
	excel_sheet.DetachDispatch();
	pWorksheet->Release();

	//����ҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("��˨��е����");

	
	//��������
	CVariant2dArray mapPage4(LsTypeCount+6,5);//���ݵĶ�ά����
	
	strcpy(cell_start,"A1");
	strcpy(cell_end,"D2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage4.SetValueAt(0,0,COleVariant("�ڢ�����: ��˨��е����"));//����
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)-4119));
	border.SetWeight(COleVariant((short)4));
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	excel_range.DetachDispatch();
	pRange->Release();

	strcpy(cell_start,"A3");
	strcpy(cell_end,"A5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage4.SetValueAt(2,0,COleVariant("��  ˨\n��  ��\n(MPa)"));//����
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"B3");
	strcpy(cell_end,"B5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage4.SetValueAt(2,1,COleVariant("��  ��\nǿ  ��\n(MPa)"));//����
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"C3");
	strcpy(cell_end,"C5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage4.SetValueAt(2,2,COleVariant("��  ��\nǿ  ��\n(MPa)"));//����
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"D3");
	strcpy(cell_end,"D5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage4.SetValueAt(2,3,COleVariant("�ױڳ�ѹ\nǿ��\n(MPa)"));//����
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	iRow=5;
	for(i=0;i<LsTypeCount;i++)
	{
		if(i<LsTypeCount)
		{	
			ss.Printf("%s",ls_table[i].grade);
			mapPage4.SetValueAt(iRow,0,COleVariant(ss));//����
			ss.Printf("%8.2f",ls_table[i].fv);
			mapPage4.SetValueAt(iRow,1,COleVariant(ss));//����
			ss.Printf("%8.2f",ls_table[i].f);
			mapPage4.SetValueAt(iRow,2,COleVariant(ss));//����
			ss.Printf("%8.2f",ls_table[i].fc);
			mapPage4.SetValueAt(iRow,3,COleVariant(ss));//����
			
			iRow++;
		}
	}
	//���ñ������
	sprintf(cell_end,"D%d",iRow);
	strcpy(cell_start,"A2");
	//���뷽ʽ
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();
	//����
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,mapPage4.var);
	excel_sheet.DetachDispatch();
	pWorksheet->Release();

	//����ҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("�ڵ㼸����Ϣ");
	
	//��������
	CVariant2dArray mapPage5(1000,12);//���ݵĶ�ά����

	strcpy(cell_start,"A1");
	strcpy(cell_end,"L2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage5.SetValueAt(0,0,COleVariant("�ڢ�����: �ڵ㼸����Ϣ"));//����
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)-4119));
	border.SetWeight(COleVariant((short)4));
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	excel_range.DetachDispatch();
	pRange->Release();

	strcpy(cell_start,"A3");
	strcpy(cell_end,"A4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage5.SetValueAt(2,0,COleVariant("�ڵ�\n���"));//����
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"B3");
	strcpy(cell_end,"B4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage5.SetValueAt(2,1,COleVariant("�Գ�\n��Ϣ"));//����
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"C3");
	strcpy(cell_end,"C4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage5.SetValueAt(2,2,COleVariant("X����\n(mm)"));//����
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"D3");
	strcpy(cell_end,"D4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage5.SetValueAt(2,3,COleVariant("Y����\n(mm))"));//����
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"E3");
	strcpy(cell_end,"E4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage5.SetValueAt(2,4,COleVariant("Z����\n(mm)"));//����
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"F3");
	strcpy(cell_end,"F4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage5.SetValueAt(2,5,COleVariant("X��λ��\nԼ��"));//����
	excel_range.SetColumnWidth(COleVariant(7.5));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"G3");
	strcpy(cell_end,"G4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage5.SetValueAt(2,6,COleVariant("Y��λ��\nԼ��"));//����
	excel_range.SetColumnWidth(COleVariant(7.5));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"H3");
	strcpy(cell_end,"H4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage5.SetValueAt(2,7,COleVariant("Z��λ��\nԼ��"));//����
	excel_range.SetColumnWidth(COleVariant(7.5));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"I3");
	strcpy(cell_end,"I4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage5.SetValueAt(2,8,COleVariant("X��ת��\nԼ��"));//����
	excel_range.SetColumnWidth(COleVariant(7.5));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"J3");
	strcpy(cell_end,"J4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage5.SetValueAt(2,9,COleVariant("Y��ת��\nԼ��"));//����
	excel_range.SetColumnWidth(COleVariant(7.5));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"K3");
	strcpy(cell_end,"K4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage5.SetValueAt(2,10,COleVariant("Z��ת��\nԼ��"));//����
	excel_range.SetColumnWidth(COleVariant(8.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"L3");
	strcpy(cell_end,"L4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage5.SetValueAt(2,11,COleVariant("���ߺ�\n�б�"));//����
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();

	cell_start.Copy("A3");
	cell_end.Copy("L4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	//������
	border.AttachDispatch(borders.GetItem(8),FALSE);	//xlEdgeTop=8
	border.SetLineStyle(COleVariant((short)-4119));		//xlDouble =-4119;
	border.SetWeight(COleVariant((short)4));			//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	//�ڲ�������
	border.AttachDispatch(borders.GetItem(11),FALSE);	//xlInsideVertical=11
	border.SetLineStyle(COleVariant((short)1));			//xlContinuous =1;
	border.SetWeight(COleVariant((short)2));			//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	//�ڲ�������
	border.AttachDispatch(borders.GetItem(12),FALSE);	//xlInsideHorizontal=12
	border.SetLineStyle(COleVariant((short)1));			//xlContinuous =1;
	border.SetWeight(COleVariant((short)2));			//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	//�ײ���
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)1));	//xlContinuous =1;
	border.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();

	long nNodeNum=Ta.Node.GetNodeNum();
	ARRAY_LIST<NODE_PTR> node_arr;
	i=0;
	iRow=4;
	node_arr.SetSize(0,1000);
	for(POSTRPT_NODE *pRptNode=postreport.hashNodes.GetFirst();pRptNode;pRptNode=postreport.hashNodes.GetNext())
	{
		char symmetry[20]="";
		char sDispRest[100]="";
		if(!pRptNode->bFatherNode&&g_sysPara.bMergeRptFileMirObj)
			continue;	//	//�ϲ������Գ���ʱ���ӶԳ����ʾ���
		if(pRptNode->pNode->IsHasRestrict(0,X_RESTRICT))
			mapPage5.SetValueAt(iRow,5,COleVariant("   �̶�"));//����
		else
			mapPage5.SetValueAt(iRow,5,COleVariant("   ����"));//����
		if(pRptNode->pNode->IsHasRestrict(0,Y_RESTRICT))
			mapPage5.SetValueAt(iRow,6,COleVariant("   �̶�"));//����
		else
			mapPage5.SetValueAt(iRow,6,COleVariant("   ����"));//����
		if(pRptNode->pNode->IsHasRestrict(0,Z_RESTRICT))
			mapPage5.SetValueAt(iRow,7,COleVariant("   �̶�"));//����
		else
			mapPage5.SetValueAt(iRow,7,COleVariant("   ����"));//����
		if(pRptNode->pNode->IsHasRestrict(0,X_ROT_RESTRICT))
			mapPage5.SetValueAt(iRow,8,COleVariant("   �̶�"));//����
		else
			mapPage5.SetValueAt(iRow,8,COleVariant("   ����"));//����
		if(pRptNode->pNode->IsHasRestrict(0,Y_ROT_RESTRICT))
			mapPage5.SetValueAt(iRow,9,COleVariant("   �̶�"));//����
		else
			mapPage5.SetValueAt(iRow,9,COleVariant("   ����"));//����
		if(pRptNode->pNode->IsHasRestrict(0,Z_ROT_RESTRICT))
			mapPage5.SetValueAt(iRow,10,COleVariant("   �̶�"));//����
		else
			mapPage5.SetValueAt(iRow,10,COleVariant("   ����"));//����
		CXhChar50 moduleStr;
		if(pRptNode->pNode->IsLegObj())
			pRptNode->pNode->cfgword.GetLegScopeStr(moduleStr,51,true);
		else
			pRptNode->pNode->cfgword.GetBodyScopeStr(moduleStr,51);
		mapPage5.SetValueAt(iRow,11,COleVariant((char*)moduleStr));//����*/
		mapPage5.SetValueAt(iRow,0,COleVariant((long)pRptNode->pNode->point_i));//����
		mapPage5.SetValueAt(iRow,1,COleVariant(pRptNode->symmetry));//����
		mapPage5.SetValueAt(iRow,2,COleVariant((char*)CXhChar16("%8.0f",pRptNode->pNode->Position(false).x)));//����
		mapPage5.SetValueAt(iRow,3,COleVariant((char*)CXhChar16("%8.0f",pRptNode->pNode->Position(false).y)));//����
		mapPage5.SetValueAt(iRow,4,COleVariant((char*)CXhChar16("%8.0f",pRptNode->pNode->Position(false).z)));//����
		iRow++;
	}
	//���ñ������
	sprintf(cell_end,"L%d",iRow);
	strcpy(cell_start,"A5");
	//���뷽ʽ
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetHorizontalAlignment(COleVariant((short)-4152));	//xlRight:-4152
	excel_range.SetVerticalAlignment(COleVariant((short)2));		//xlCenter:-4108
	excel_range.ReleaseDispatch();
	//�趨���һ��Ϊ�ı�������Ὣ"1-5"������ģ�ͷ�Χ�Զ�ת��������
	pRange = excel_sheet.GetRange(COleVariant("L1"),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetNumberFormatLocal(COleVariant("@"));
	excel_range.ReleaseDispatch();
	//����
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,mapPage5.var);
	excel_sheet.DetachDispatch();
	pWorksheet->Release();
	
	//����ҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("�����嵥");

	//��������
	CVariant2dArray mapPage6(postreport.TowerWeight.GetNodeNum()+4,12);//���ݵĶ�ά����
	
	strcpy(cell_start,"A1");
	strcpy(cell_end,"F2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage6.SetValueAt(0,0,COleVariant("�ڢ�����: �����嵥(�������󲹳�ϵ��Cgδ��������)"));//����
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	//borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)-4119));	//xlDouble =-4119;
	border.SetWeight(COleVariant((short)4));	//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	excel_range.DetachDispatch();
	pRange->Release();
	
	strcpy(cell_start,"A3");
	strcpy(cell_end,"B4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage6.SetValueAt(2,0,COleVariant("����\n����/���Ⱥ�"));//����
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"C3");
	strcpy(cell_end,"D4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage6.SetValueAt(2,2,COleVariant("������(kg)"));//����
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"E3");
	strcpy(cell_end,"F4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage6.SetValueAt(2,4,COleVariant("������(kN)"));//����
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.ReleaseDispatch();
	
	i=0;
	iRow=4;
	for(TOWERWEIGHT *pWeight=postreport.TowerWeight.GetFirst();pWeight;pWeight=postreport.TowerWeight.GetNext())
	{
		CXhChar50 csMark;
		CXhChar16 cfgStr;
		if(pWeight->iBody0Leg1==0)	//����
		{
			cfgStr.Printf("%d",pWeight->cfg_no);
			cfgStr.ResizeLength(3,' ',true);
			sprintf(csMark," %s           ",(char*)cfgStr,pWeight->cfg_no);
		}
		else						//����
		{
			CFGWORD cfgword(pWeight->cfg_no);
			cfgword.GetLegScopeStr(cfgStr,17,true);
			cfgStr.ResizeLength(4,' ',true);
			sprintf(csMark," %s          ",(char*)cfgStr);//(pWeight->cfg_no-1)/8+1,(pWeight->cfg_no-1)%8+'A');
		}
		int str_len=strlen(csMark);
		if(str_len<15)
		{
			for(int j=15;j>=0;j--)
			{
				if(j-15+str_len<0)
					csMark[j]=' ';
				else
					csMark[j]=csMark[j-15+str_len];
			}
		}
		
		cell_start.Printf("A%d",iRow);
		cell_end.Printf("B%d",iRow);
		pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.Merge(COleVariant((BYTE)1));
		ss.Printf("%s",(char*)csMark);
		mapPage6.SetValueAt(iRow-1,0,COleVariant(ss));//����
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();
		
		cell_start.Printf("C%d",iRow);
		cell_end.Printf("D%d",iRow);
		pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.Merge(COleVariant((BYTE)1));
		ss.Printf("%8.2f",pWeight->netWeight);
		mapPage6.SetValueAt(iRow-1,2,COleVariant(ss));//����
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();
		
		cell_start.Printf("E%d",iRow);
		cell_end.Printf("F%d",iRow);
		pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.Merge(COleVariant((BYTE)1));
		ss.Printf("%8.3f",pWeight->netWeight*0.0098);
		mapPage6.SetValueAt(iRow-1,4,COleVariant(ss));//����
		excel_range.SetVerticalAlignment(COleVariant((long)-4108));
		excel_range.ReleaseDispatch();
		
		iRow++;
	}
	//���ñ������
	sprintf(cell_end,"F%d",iRow);
	strcpy(cell_start,"A3");
	//���뷽ʽ
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();
	//����
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,mapPage6.var);
	excel_sheet.DetachDispatch();
	pWorksheet->Release();

	//����ҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("�����ڵ㷴������");

	//�������� ��ʱ��˴���20
	CVariant2dArray mapPage7(postreport.Reaction.GetNodeNum()+4+20,7);//���ݵĶ�ά����
	
	strcpy(cell_start,"A1");
	if(postreport.m_bHasStdFoundationForce)
		strcpy(cell_end,"J2");
	else
		strcpy(cell_end,"G2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage7.SetValueAt(0,0,COleVariant("�ڢ�����: �����ڵ㷴������"));//����
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	//borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)-4119));	//xlDouble =-4119;
	border.SetWeight(COleVariant((short)4));	//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	excel_range.DetachDispatch();
	pRange->Release();
	
	strcpy(cell_start,"A3");
	strcpy(cell_end,"A4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage7.SetValueAt(2,0,COleVariant("�ڵ�\n���"));//����
	excel_range.SetColumnWidth(COleVariant(5.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"B3");
	strcpy(cell_end,"D3");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage7.SetValueAt(2,1,COleVariant("����״̬"));//����
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"B4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	mapPage7.SetValueAt(3,1,COleVariant("����"));//����
	excel_range.SetColumnWidth(COleVariant(5.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"C4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	mapPage7.SetValueAt(3,2,COleVariant("����"));//����
	excel_range.SetColumnWidth(COleVariant(5.0));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"D4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	mapPage7.SetValueAt(3,3,COleVariant("���Ⱥ�"));//����
	excel_range.SetColumnWidth(COleVariant(12.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"E3");
	strcpy(cell_end,"E4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage7.SetValueAt(2,4,COleVariant("Rx(kN)\n���ֵ"));//����
	excel_range.SetColumnWidth(COleVariant(7.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"F3");
	strcpy(cell_end,"F4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage7.SetValueAt(2,5,COleVariant("Ry(kN)\n���ֵ"));//����
	excel_range.SetColumnWidth(COleVariant(7.0));
	excel_range.ReleaseDispatch();
	
	strcpy(cell_start,"G3");
	strcpy(cell_end,"G4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage7.SetValueAt(2,6,COleVariant("Rz(kN)\n���ֵ"));//����
	excel_range.SetColumnWidth(COleVariant(7.0));
	excel_range.ReleaseDispatch();
	
	if(postreport.m_bHasStdFoundationForce)
	{
		strcpy(cell_start,"H3");
		strcpy(cell_end,"H4");
		pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		excel_range.AttachDispatch(pRange);
		excel_range.Merge(COleVariant((BYTE)0));
		mapPage7.SetValueAt(2,7,COleVariant("Rx(kN)\n��׼ֵ"));//����
		excel_range.SetColumnWidth(COleVariant(7.0));
		excel_range.ReleaseDispatch();
		
		strcpy(cell_start,"I3");
		strcpy(cell_end,"I4");
		pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		excel_range.AttachDispatch(pRange);
		excel_range.Merge(COleVariant((BYTE)0));
		mapPage7.SetValueAt(2,8,COleVariant("Ry(kN)\n��׼ֵ"));//����
		excel_range.SetColumnWidth(COleVariant(7.0));
		excel_range.ReleaseDispatch();
		
		strcpy(cell_start,"J3");
		strcpy(cell_end,"J4");
		pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		excel_range.AttachDispatch(pRange);
		excel_range.Merge(COleVariant((BYTE)0));
		mapPage7.SetValueAt(2,8,COleVariant("Rz(kN)\n��׼ֵ"));//����
		excel_range.SetColumnWidth(COleVariant(7.0));
		excel_range.ReleaseDispatch();
	}

	//�趨�����и�ʽ
	cell_start.Copy("A3");
	if(postreport.m_bHasStdFoundationForce)
		cell_end.Copy("J4");
	else
		cell_end.Copy("G4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	//������
	border.AttachDispatch(borders.GetItem(8),FALSE);	//xlEdgeTop=8
	border.SetLineStyle(COleVariant((short)-4119));		//xlDouble =-4119;
	border.SetWeight(COleVariant((short)4));			//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	//�ڲ�������
	border.AttachDispatch(borders.GetItem(11),FALSE);	//xlInsideVertical=11
	border.SetLineStyle(COleVariant((short)1));			//xlContinuous =1;
	border.SetWeight(COleVariant((short)2));			//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	//�ڲ�������
	border.AttachDispatch(borders.GetItem(12),FALSE);	//xlInsideHorizontal=12
	border.SetLineStyle(COleVariant((short)1));			//xlContinuous =1;
	border.SetWeight(COleVariant((short)2));			//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	//�ײ���
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)1));	//xlContinuous =1;
	border.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();

	int nReaction=postreport.Reaction.GetNodeNum();
	REACTION_PTR *react_arr=new REACTION_PTR[nReaction];
	i=0;
	for(BASENODE_REACTION *pReact=postreport.Reaction.GetFirst();pReact;pReact=postreport.Reaction.GetNext())
	{
		pReact->status=0;
		react_arr[i]=pReact;
		i++;
	}
	//CQuickSort<REACTION_PTR>::QuickSort(react_arr,nReaction,compare_func3);
	//�˴����ÿ�������������������ֻ�ʱ����ݹ���ȳ���ת���������򣬽�������Ч�ʼ����½����������� wjh-2013-5-1
	CHeapSort<REACTION_PTR>::HeapSort(react_arr,nReaction,compare_func3);
	long prev_point_i=0,nCount=0;
	double Rzmax=0,Rzmin=0;
	ARRAY_LIST<REACTION_PTR>exportReactArr;
	exportReactArr.SetSize(0,10);
	long iRzmax=0,iRzmin=0;
	for(i=0;i<nReaction;i++)
	{
		if(g_sysPara.reactionExportType==0)
		{
			exportReactArr.append(react_arr[i]);
			continue;
		}
		if(i==0)
			iRzmin=iRzmax=0;
		else //if(i>0)
		{
			if( g_sysPara.reactionExportType==1&&(react_arr[i-1]->pNode->point_i==react_arr[i]->pNode->point_i)&&
				react_arr[i-1]->pModule==react_arr[i]->pModule&&react_arr[i-1]->pStatus==react_arr[i]->pStatus)
			{	//�ߵ��ȼ�ɸѡ
				//if(postreport.m_bHasStdFoundationForce&&react_arr[i]->reaction_force_std.z>react_arr[iRzmax]->reaction_force_std.z)
				//	iRzmax=i;
				//else if(!postreport.m_bHasStdFoundationForce&&react_arr[i]->reaction_force_design.z>react_arr[iRzmax]->reaction_force_design.z)
				if(react_arr[i]->reaction_force_design.z>react_arr[iRzmax]->reaction_force_design.z)
					iRzmax=i;
				//if(postreport.m_bHasStdFoundationForce&&react_arr[i]->reaction_force_std.z<react_arr[iRzmin]->reaction_force_std.z)
				//	iRzmin=i;
				//else if(!postreport.m_bHasStdFoundationForce&&react_arr[i]->reaction_force_design.z<react_arr[iRzmin]->reaction_force_design.z)
				if(react_arr[i]->reaction_force_design.z<react_arr[iRzmin]->reaction_force_design.z)
					iRzmin=i;
			}
			else if(g_sysPara.reactionExportType==2&&(react_arr[i-1]->pNode->point_i==react_arr[i]->pNode->point_i)&&
				react_arr[i-1]->pModule==react_arr[i]->pModule)
			{	//������ɸѡ
				//if(postreport.m_bHasStdFoundationForce&&react_arr[i]->reaction_force_std.z>react_arr[iRzmax]->reaction_force_std.z)
				//	iRzmax=i;
				//else if(!postreport.m_bHasStdFoundationForce&&react_arr[i]->reaction_force_design.z>react_arr[iRzmax]->reaction_force_design.z)
				if(react_arr[i]->reaction_force_design.z>react_arr[iRzmax]->reaction_force_design.z)
					iRzmax=i;
				//if(postreport.m_bHasStdFoundationForce&&react_arr[i]->reaction_force_std.z<react_arr[iRzmin]->reaction_force_std.z)
				//	iRzmin=i;
				//else if(!postreport.m_bHasStdFoundationForce&&react_arr[i]->reaction_force_design.z<react_arr[iRzmin]->reaction_force_design.z)
				if(react_arr[i]->reaction_force_design.z<react_arr[iRzmin]->reaction_force_design.z)
					iRzmin=i;
			}
			else
			{	//�ֻ�
				if(iRzmin<iRzmax)
				{
					//if((postreport.m_bHasStdFoundationForce&&react_arr[iRzmin]->reaction_force_std.z<0)||
					//  (!postreport.m_bHasStdFoundationForce&&react_arr[iRzmin]->reaction_force_design.z<0))
					if(react_arr[iRzmin]->reaction_force_design.z<0)
						exportReactArr.append(react_arr[iRzmin]);
					//if((postreport.m_bHasStdFoundationForce&&react_arr[iRzmax]->reaction_force_std.z>0)||
					//  (!postreport.m_bHasStdFoundationForce&&react_arr[iRzmax]->reaction_force_design.z>0))
					if(react_arr[iRzmax]->reaction_force_design.z>0)
						exportReactArr.append(react_arr[iRzmax]);
				}
				else if(iRzmin>iRzmax)
				{
					//if((postreport.m_bHasStdFoundationForce&&react_arr[iRzmax]->reaction_force_std.z>0)||
					//  (!postreport.m_bHasStdFoundationForce&&react_arr[iRzmax]->reaction_force_design.z>0))
					if(react_arr[iRzmax]->reaction_force_design.z>0)
						exportReactArr.append(react_arr[iRzmax]);
					//if((postreport.m_bHasStdFoundationForce&&react_arr[iRzmin]->reaction_force_std.z<0)||
					//  (!postreport.m_bHasStdFoundationForce&&react_arr[iRzmin]->reaction_force_design.z<0))
					if(react_arr[iRzmin]->reaction_force_design.z<0)
						exportReactArr.append(react_arr[iRzmin]);
				}
				else
					exportReactArr.append(react_arr[iRzmin]);
				iRzmin=iRzmax=i;
			}
		}
		if(i==nReaction-1)
		{	//���һ����¼ʱ����
			if(iRzmin<iRzmax)
			{
				//if((postreport.m_bHasStdFoundationForce&&react_arr[iRzmin]->reaction_force_std.z<0)||
				//  (!postreport.m_bHasStdFoundationForce&&react_arr[iRzmin]->reaction_force_design.z<0))
				if(react_arr[iRzmin]->reaction_force_design.z<0)
					exportReactArr.append(react_arr[iRzmin]);
				//if((postreport.m_bHasStdFoundationForce&&react_arr[iRzmax]->reaction_force_std.z>0)||
				//  (!postreport.m_bHasStdFoundationForce&&react_arr[iRzmax]->reaction_force_design.z>0))
				if(react_arr[iRzmax]->reaction_force_design.z>0)
					exportReactArr.append(react_arr[iRzmax]);
			}
			else if(iRzmin>iRzmax)
			{
				//if((postreport.m_bHasStdFoundationForce&&react_arr[iRzmax]->reaction_force_std.z>0)||
				//  (!postreport.m_bHasStdFoundationForce&&react_arr[iRzmax]->reaction_force_design.z>0))
				if(react_arr[iRzmax]->reaction_force_design.z>0)
					exportReactArr.append(react_arr[iRzmax]);
				//if((postreport.m_bHasStdFoundationForce&&react_arr[iRzmin]->reaction_force_std.z<0)||
				//  (!postreport.m_bHasStdFoundationForce&&react_arr[iRzmin]->reaction_force_design.z<0))
				if(react_arr[iRzmin]->reaction_force_design.z<0)
					exportReactArr.append(react_arr[iRzmin]);
			}
			else
				exportReactArr.append(react_arr[iRzmin]);
		}
	}
	delete []react_arr;
	iLineNum=0;
	for(i=0;i<exportReactArr.GetSize();i++)
	{
		if(prev_point_i==0)
		{
			nCount++;
			/*
			if(postreport.m_bHasStdFoundationForce)
			{
				if(exportReactArr[i]->reaction_force_std.z>0)
					Rzmax=exportReactArr[i]->reaction_force_std.z;
				else
					Rzmin=exportReactArr[i]->reaction_force_std.z;
			}
			else
			*/
			{
				if(exportReactArr[i]->reaction_force_design.z>0)
					Rzmax=exportReactArr[i]->reaction_force_design.z;
				else
					Rzmin=exportReactArr[i]->reaction_force_design.z;
			}
			SetReactionExcelRecord(iLineNum+5,exportReactArr[i],mapPage7,true);
			iLineNum++;
			prev_point_i=exportReactArr[i]->pNode->point_i;
		}
		else if(prev_point_i==exportReactArr[i]->pNode->point_i)
		{
			nCount++;
			/*
			if(postreport.m_bHasStdFoundationForce)
			{
				if(exportReactArr[i]->reaction_force_std.z>Rzmax)
					Rzmax=exportReactArr[i]->reaction_force_std.z;
				else if(exportReactArr[i]->reaction_force_std.z<Rzmin)
					Rzmin=exportReactArr[i]->reaction_force_std.z;
			}
			else
			*/
			{
				if(exportReactArr[i]->reaction_force_design.z>Rzmax)
					Rzmax=exportReactArr[i]->reaction_force_design.z;
				else if(exportReactArr[i]->reaction_force_design.z<Rzmin)
					Rzmin=exportReactArr[i]->reaction_force_design.z;
			}
			SetReactionExcelRecord(iLineNum+5,exportReactArr[i],mapPage7,false);
			iLineNum++;
			if(i==exportReactArr.GetSize()-1&&nCount>=2)	//���һ��������������
			{
				ss.Printf("'+Rzmax=%8.3f(kN),-Rzmax=%8.3f(kN)",Rzmax/1000,Rzmin/1000);
				cell_start.Printf("B%d",iLineNum+5);
				cell_end.Copy(cell_start);
				if(postreport.m_bHasStdFoundationForce)
					cell_end[0]='J';
				else
					cell_end[0]='G';
				pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange);
				excel_range.Merge(COleVariant((BYTE)1));
				mapPage7.SetValueAt(iLineNum+4,1,COleVariant(ss));//����
				excel_range.DetachDispatch();
				pRange->Release();
				iLineNum++;
			}
			continue;
		}
		else	//ת���ڵ�
		{
			if(nCount>=2)
			{
				ss.Printf("'+Rzmax=%8.3f(kN),-Rzmax=%8.3f(kN)",Rzmax/1000,Rzmin/1000);
				cell_start.Printf("B%d",iLineNum+5);
				cell_end.Copy(cell_start);
				if(postreport.m_bHasStdFoundationForce)
					cell_end[0]='J';
				else
					cell_end[0]='G';
				pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange);
				excel_range.Merge(COleVariant((BYTE)1));
				mapPage7.SetValueAt(iLineNum+4,1,COleVariant(ss));//����
				excel_range.DetachDispatch();
				pRange->Release();
				iLineNum++;
			}
			/*
			if(postreport.m_bHasStdFoundationForce)
			{
				if(exportReactArr[i]->reaction_force_std.z>0)
					Rzmax=exportReactArr[i]->reaction_force_std.z;
				else
					Rzmin=exportReactArr[i]->reaction_force_std.z;
			}
			else
			*/
			{
				if(exportReactArr[i]->reaction_force_design.z>0)
					Rzmax=exportReactArr[i]->reaction_force_design.z;
				else
					Rzmin=exportReactArr[i]->reaction_force_design.z;
			}
			nCount=1;
			SetReactionExcelRecord(iLineNum+5,exportReactArr[i],mapPage7,true);
			iLineNum++;
			prev_point_i=exportReactArr[i]->pNode->point_i;
		}
	}
	sprintf(cell_end,"G%d",iLineNum+4);
	strcpy(cell_start,"A3");
	//���뷽ʽ
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();
	//����
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,mapPage7.var);
	excel_sheet.DetachDispatch();
	pWorksheet->Release();

	//Part �� & Part ��
	//�ڰ�ҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("��Ԫ���������Ϣ");

	int nElemNum=postreport.hashElems.GetNodeNum();
	//��������
	CVariant2dArray mapPage8(nElemNum+3,15);//���ݵĶ�ά����

	ELEM_PTR *elem_arr=new ELEM_PTR[nElemNum];
	i=0;
	for(pElem=postreport.hashElems.GetFirst();pElem;pElem=postreport.hashElems.GetNext())
	{
		elem_arr[i]=pElem;
		i++;
	}
	CQuickSort<ELEM_PTR>::QuickSort(elem_arr,nElemNum,compare_func2);
	
	strcpy(cell_start,"A1");
	strcpy(cell_end,"O2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage8.SetValueAt(0,0,COleVariant("�ڢ�����: ��Ԫ���������Ϣ"));//����
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	excel_range.DetachDispatch();
	pRange->Release();
	
	mapPage8.SetValueAt(2,0,COleVariant("��  Ԫ\n��  ʶ"));//����
	mapPage8.SetValueAt(2,1,COleVariant("�Գ�\n��Ϣ"));//����
	mapPage8.SetValueAt(2,2,COleVariant("��\n��"));//����
	mapPage8.SetValueAt(2,3,COleVariant("��Ԫ\n���"));//����
	mapPage8.SetValueAt(2,4,COleVariant("ͳ��\n��"));//����
	mapPage8.SetValueAt(2,5,COleVariant("����\n����"));//����
	mapPage8.SetValueAt(2,6,COleVariant("�˶�\n״��"));//����
	mapPage8.SetValueAt(2,7,COleVariant("��˨��Ϣ"));//����
	mapPage8.SetValueAt(2,8,COleVariant("����\nϵ��"));//����
	mapPage8.SetValueAt(2,9,COleVariant("��\n����"));//����
	mapPage8.SetValueAt(2,10,COleVariant("����\n(mm)"));//����
	mapPage8.SetValueAt(2,11,COleVariant("���㳤��\n(mm)"));//����
	mapPage8.SetValueAt(2,12,COleVariant(""));//����
	//�����п�
	pRange = excel_sheet.GetRange(COleVariant("A3"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant((short)9));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("B3"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant((short)4));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("C3"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant((short)2));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("D3"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant((short)10));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("E3"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant((short)4));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("F3"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant((short)5));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("G3"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant((short)5));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("H3"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant((short)10));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("I3"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant((short)5));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("J3"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant((short)5));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("K3"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant((short)5));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("L3"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant((short)16));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("M3"),COleVariant("N3"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant((short)5));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("O3"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant((short)7));
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"L3");
	strcpy(cell_end,"M3");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	excel_range.ReleaseDispatch();

	mapPage8.SetValueAt(2,13,COleVariant("G.R.\n(mm)"));//����
	mapPage8.SetValueAt(2,14,COleVariant("��ϸ��\n��ʽ"));//����
	//���ñ����и�ʽ
	strcpy(cell_start,"A3");
	strcpy(cell_end,"O3");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	//������
	border.AttachDispatch(borders.GetItem(8),FALSE);	//xlEdgeTop=8
	border.SetLineStyle(COleVariant((short)-4119));		//xlDouble =-4119;
	border.SetWeight(COleVariant((short)4));			//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	//�ڲ�������
	border.AttachDispatch(borders.GetItem(11),FALSE);	//xlInsideVertical=11
	border.SetLineStyle(COleVariant((short)1));			//xlContinuous =1;
	border.SetWeight(COleVariant((short)2));			//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	//�ڲ�������
	/*border.AttachDispatch(borders.GetItem(12),FALSE);	//xlInsideHorizontal=12
	border.SetLineStyle(COleVariant((short)1));			//xlContinuous =1;
	border.SetWeight(COleVariant((short)2));			//xlThin=2;xlThick=4;
	border.ReleaseDispatch();*/
	//�ײ���
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)1));	//xlContinuous =1;
	border.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	
	iRow = 3;
	for(i=0;i<nElemNum;i++)
	{
		long No[2];
		//��Ԫ��ʶ
		bool bReverse=elem_arr[i]->pStart->point_i>elem_arr[i]->pEnd->point_i;
		No[0]=min(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		No[1]=max(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		ss.Printf("%4d-%4d",No[0],No[1]);
		cell.Printf("A%d",iRow);
		mapPage8.SetValueAt(iRow,0,COleVariant(ss));//����
		//�Գ���Ϣ
		char symmetry[20]="";
		if(elem_arr[i]->relaElem.bFatherElem)//pOrgPole->IsFatherObject())	//���˼�
		{
			if(elem_arr[i]->relaElem.idElemMirX>0)
				strcat(symmetry," X");
			else
				strcpy(symmetry,"  ");
			if(elem_arr[i]->relaElem.idElemMirY>0)
				strcat(symmetry,"Y");
			else
				strcat(symmetry," ");
			if(elem_arr[i]->relaElem.idElemMirZ>0)
				strcat(symmetry,"Z");
			else
				strcat(symmetry," ");
			if(!elem_arr[i]->IsHasMirInfo())
				strcpy(symmetry,"  ��");
		}
		else
		{
			long idFather=preprocessor.GetFatherElemId(elem_arr[i]);
			if(idFather>0&&idFather==elem_arr[i]->relaElem.idElemMirX)
				strcpy(symmetry,"-X");
			else if(idFather>0&&idFather==elem_arr[i]->relaElem.idElemMirY)
				strcpy(symmetry," -Y");
			else if(idFather>0&&idFather==elem_arr[i]->relaElem.idElemMirZ)
				strcpy(symmetry,"  -Z");
			else
				strcpy(symmetry,"  ��");
		}
		cell[0]='B';
		mapPage8.SetValueAt(iRow,1,COleVariant(symmetry));//����
		cell[0]='C';
		//��Ԫ����
		char elemtype[5]="";
		elemtype[0]=elem_arr[i]->pOrgPole->layer(1);
		mapPage8.SetValueAt(iRow,2,COleVariant(elemtype));//����
		//��Ԫ���
		cell[0]='D';
		CXhChar16 wideStr,thickStr;
		wideStr.Printf("%f",elem_arr[i]->GetWidth());
		thickStr.Printf("%f",elem_arr[i]->GetThick());
		SimplifiedNumString(wideStr);
		SimplifiedNumString(thickStr);
		ss.Printf("%s%sX%s%C",elem_arr[i]->pOrgPole->GetBriefPartTypeMark(),(char*)wideStr,(char*)thickStr,elem_arr[i]->pOrgPole->cMaterial);
		mapPage8.SetValueAt(iRow,3,COleVariant(ss));//����
		//ͳ�ĺ�
		char statmatno[5]="";
		if(elem_arr[i]->pOrgPole->m_uStatMatNo==0)
			strcpy(statmatno,"    ");
		else
			_snprintf(statmatno,4,"%4d",elem_arr[i]->pOrgPole->m_uStatMatNo);
		cell[0]='E';
		mapPage8.SetValueAt(iRow,4,COleVariant(statmatno));//����
		//��������
		char jointtype[6]="";
		if(elem_arr[i]->real_start_joint_type==JOINT_HINGE)
			strcpy(jointtype,"��-");
		else
			strcpy(jointtype,"��-");
		if(elem_arr[i]->real_end_joint_type==JOINT_HINGE)
			strcat(jointtype,"��");
		else
			strcat(jointtype,"��");
		//�˶�״��
		char forcetype[6]="";
		if(elem_arr[i]->design_info.bStartRotRest)
			forcetype[0]='P';
		else
			forcetype[0]='F';
		if(elem_arr[i]->design_info.bEndRotRest)
			forcetype[1]='P';
		else
			forcetype[1]='F';
		forcetype[2]='^';
		if(elem_arr[i]->start_force_type==CENTRIC_FORCE)
			forcetype[3]='C';
		else
			forcetype[3]='E';
		if(elem_arr[i]->end_force_type==CENTRIC_FORCE)
			forcetype[4]='C';
		else
			forcetype[4]='E';
		if(bReverse)
		{
			char ch=jointtype[0];
			jointtype[0]=jointtype[3];
			jointtype[3]=ch;
			ch=jointtype[1];
			jointtype[1]=jointtype[4];
			jointtype[4]=ch;

			ch=forcetype[0];
			forcetype[0]=forcetype[1];
			forcetype[1]=ch;
			ch=forcetype[3];
			forcetype[3]=forcetype[4];
			forcetype[4]=ch;
		}
		cell[0]='F';
		mapPage8.SetValueAt(iRow,5,COleVariant(jointtype));//����
		cell[0]='G';
		mapPage8.SetValueAt(iRow,6,COleVariant(forcetype));//����

		//��˨��Ϣ
		CXhChar50 ss_ls;
		if(elem_arr[i]->pStart==elem_arr[i]->pOrgPole->pStart)	//ʼ�˵�Ԫ
			sprintf(ss_ls,"%sM%2dX%2d",elem_arr[i]->pOrgPole->connectStart.grade,elem_arr[i]->pOrgPole->connectStart.d,elem_arr[i]->pOrgPole->connectStart.wnSummBoltN);
		else if(elem_arr[i]->pEnd==elem_arr[i]->pOrgPole->pEnd)	//ĩ�˵�Ԫ
			sprintf(ss_ls,"%sM%2dX%2d",elem_arr[i]->pOrgPole->connectEnd.grade,elem_arr[i]->pOrgPole->connectEnd.d,elem_arr[i]->pOrgPole->connectEnd.wnSummBoltN);
		else
			sprintf(ss_ls,"Middle Elem.   ");
		cell[0]='H';
		mapPage8.SetValueAt(iRow,7,COleVariant(ss_ls));//����
		//����ϵ��
		if(elem_arr[i]->pStart==elem_arr[i]->pOrgPole->pStart)	//ʼ�˵�Ԫ
			ss.Printf("%5.3f",elem_arr[i]->pOrgPole->connectStart.bearloadcoef);
		else if(elem_arr[i]->pEnd==elem_arr[i]->pOrgPole->pEnd)	//ĩ�˵�Ԫ
			ss.Printf("%5.3f",elem_arr[i]->pOrgPole->connectEnd.bearloadcoef);
		else
			ss.Copy("");
		cell[0]='I';
		if(ss.GetLength()>0)
			mapPage8.SetValueAt(iRow,8,COleVariant(ss));//����
		//������
		ss.Printf("%4.2f",elem_arr[i]->design_info.hole_out);
		cell[0]='J';
		mapPage8.SetValueAt(iRow,9,COleVariant(ss));//����
		//��Ԫ����
		ss.Printf("%5.0f",elem_arr[i]->Length());
		cell[0]='K';
		mapPage8.SetValueAt(iRow,10,COleVariant(ss));//����
		cell[0]='L';
		mapPage8.SetValueAt(iRow,11,COleVariant(elem_arr[i]->design_info.LoExpression));//����
		cell[0]='M';
		ss.Printf("%4.0f",elem_arr[i]->LamdaL());
		mapPage8.SetValueAt(iRow,12,COleVariant(ss));//����
		cell[0]='N';
		ss.Printf("%4.1f",elem_arr[i]->LamdaR());
		mapPage8.SetValueAt(iRow,13,COleVariant(ss));//����
		cell[0]='O';
		mapPage8.SetValueAt(iRow,14,COleVariant(elem_arr[i]->design_info.lamdaExpression));//����
		iRow++;
	}
	sprintf(cell_end,"O%d",iRow);
	strcpy(cell_start,"A3");
	//���뷽ʽ
	/*pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();*/

	cell_start[0]='D';
	cell_end[0]='E';
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetHorizontalAlignment(COleVariant((short)-4152));	//xlRight:-4152,D,E,J,K,M,N
	excel_range.ReleaseDispatch();
	cell_start[0]='I';
	cell_end[0]='K';
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetHorizontalAlignment(COleVariant((short)-4152));	//xlRight:-4152,D,E,J,K,M,N
	excel_range.ReleaseDispatch();
	cell_start[0]='M';
	cell_end[0]='N';
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetHorizontalAlignment(COleVariant((short)-4152));	//xlRight:-4152,D,E,J,K,M,N
	excel_range.ReleaseDispatch();

	cell_start[0]=cell_end[0]='H';
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetHorizontalAlignment(COleVariant((short)-4131));	//xlLeft:-4131,H
	excel_range.ReleaseDispatch();
	cell_start[0]=cell_end[0]='O';
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetHorizontalAlignment(COleVariant((short)-4131));	//xlLeft:-4131,O
	excel_range.ReleaseDispatch();
	//����
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,mapPage8.var);
	excel_sheet.DetachDispatch();
	pWorksheet->Release();
	
	//�ھ�ҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("��Ԫ��ϸ�����Ϣ");

	CLDSModule *pModule;
	CWorkStatus *pStatus;
	CHashTable<long>hashModuleNo,hashStatusNo;
	hashModuleNo.CreateHashTable(Ta.Module.GetNodeNum());
	hashStatusNo.CreateHashTable(Ta.WorkStatus.GetNodeNum());
	for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		hashModuleNo.SetValueAt(pModule->handle,pModule->iNo);
	for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
		hashStatusNo.SetValueAt(pStatus->handle,pStatus->iNo);

	//��������

	int cols_n=17;
	strcpy(cell_start,"A1");
	strcpy(cell_end,"Q2");
	if(rpt_type==0)//!bCheckPart)
	{
		cell_end[0]-=1;
		cols_n-=1;
	}
	CVariant2dArray mapPage9(nElemNum+3,cols_n);//���ݵĶ�ά����
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage9.SetValueAt(0,0,COleVariant("�ڢ�����: ��Ԫ��ϸ�����Ϣ"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	excel_range.DetachDispatch();
	pRange->Release();
	
	int colheader_incre=0;
	if(rpt_type!=0)
	{
		colheader_incre=1;
		mapPage9.SetValueAt(2,0,COleVariant("��  Ԫ\n��  ʶ"));
		mapPage9.SetValueAt(2,1,COleVariant("��  ��\n��  ��"));
		mapPage9.SetValueAt(2,2,COleVariant("��\n��"));
		mapPage9.SetValueAt(2,3,COleVariant("��Ԫ\n���\n(mm)"));
		mapPage9.SetValueAt(2,4,COleVariant("���\n����"));
		mapPage9.SetValueAt(2,5,COleVariant("����\n-\n����"));
		mapPage9.SetValueAt(2,6,COleVariant("���\nѹ��"));
		mapPage9.SetValueAt(2,7,COleVariant("����\n-\n����"));
		mapPage9.SetValueAt(2,8,COleVariant("���\nӦ��\n(MPa)"));
		mapPage9.SetValueAt(2,9,COleVariant("����\nӦ��\n(MPa)"));
		mapPage9.SetValueAt(2,10,COleVariant("ѹ��\nϵ��\n(��)"));
		mapPage9.SetValueAt(2,11,COleVariant("ǿ��\n�ۼ�\nϵ��"));
		mapPage9.SetValueAt(2,12,COleVariant("����\n(mm)"));
		mapPage9.SetValueAt(2,13,COleVariant("����\n����\n(mm)"));
		mapPage9.SetValueAt(2,14,COleVariant("(K*)\nLo/r"));
		mapPage9.SetValueAt(2,15,COleVariant("���\n����\nLo/r"));
		if(g_sysPara.iEfficacyCeofType==0)
			mapPage9.SetValueAt(2,16,COleVariant("���\nԣ��"));
		else
			mapPage9.SetValueAt(2,16,COleVariant("����\nϵ��"));
	}
	else
	{
		mapPage9.SetValueAt(2,0,COleVariant("��  Ԫ\n��  ʶ"));
		mapPage9.SetValueAt(2,1,COleVariant("��Ԫ\n����\n"));
		mapPage9.SetValueAt(2,2,COleVariant("��Ԫ\n���\n(mm)"));
		mapPage9.SetValueAt(2,3,COleVariant("���\n����"));
		mapPage9.SetValueAt(2,4,COleVariant("����\n-\n����"));
		mapPage9.SetValueAt(2,5,COleVariant("���\nѹ��"));
		mapPage9.SetValueAt(2,6,COleVariant("����\n-\n����"));
		mapPage9.SetValueAt(2,7,COleVariant("���\nӦ��\n(MPa)"));
		mapPage9.SetValueAt(2,8,COleVariant("����\nӦ��\n(MPa)"));
		mapPage9.SetValueAt(2,9,COleVariant("ѹ��\nϵ��\n(��)"));
		mapPage9.SetValueAt(2,10,COleVariant("ǿ��\n�ۼ�\nϵ��"));
		mapPage9.SetValueAt(2,11,COleVariant("����\n(mm)"));
		mapPage9.SetValueAt(2,12,COleVariant("����\n����\n(mm)"));
		mapPage9.SetValueAt(2,13,COleVariant("(K*)\nLo/r"));
		mapPage9.SetValueAt(2,14,COleVariant("���\n����\nLo/r"));
		if(g_sysPara.iEfficacyCeofType==0)
			mapPage9.SetValueAt(2,15,COleVariant("���\nԣ��"));
		else
			mapPage9.SetValueAt(2,15,COleVariant("����\nϵ��"));
	}
	//�����п�
	pRange = excel_sheet.GetRange(COleVariant("A3"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant((short)9));
	excel_range.ReleaseDispatch();
	if(rpt_type!=0)
	{	//����ģʽ
		pRange = excel_sheet.GetRange(COleVariant("B3"));
		excel_range.AttachDispatch(pRange);
		excel_range.SetColumnWidth(COleVariant((short)6));
		excel_range.ReleaseDispatch();
	}
	cell_start.Printf("%C3",'B'+colheader_incre);
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant((short)2));
	excel_range.ReleaseDispatch();
	cell_start.Printf("%C3",'C'+colheader_incre);
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant((short)10));
	excel_range.ReleaseDispatch();
	cell_start.Printf("%C3",'D'+colheader_incre);
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant(6.5));
	excel_range.ReleaseDispatch();
	cell_start.Printf("%C3",'E'+colheader_incre);
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant(6.5));
	excel_range.ReleaseDispatch();
	cell_start.Printf("%C3",'F'+colheader_incre);
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant(7.5));
	excel_range.ReleaseDispatch();
	cell_start.Printf("%C3",'G'+colheader_incre);
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant(6.5));
	excel_range.ReleaseDispatch();
	cell_start.Printf("%C3",'H'+colheader_incre);
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant(6.5));
	excel_range.ReleaseDispatch();
	cell_start.Printf("%C3",'I'+colheader_incre);
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant(6.0));
	excel_range.ReleaseDispatch();
	cell_start.Printf("%C3",'J'+colheader_incre);
	cell_end.Printf("%C3",'O'+colheader_incre);
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant((short)5));
	excel_range.ReleaseDispatch();
	cell_start.Printf("%C3",'P'+colheader_incre);
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant(8.0));
	excel_range.ReleaseDispatch();
	//�趨�����и�ʽ
	cell_start.Copy("A3");
	if(rpt_type!=0)
		cell_end.Copy("Q3");
	else
		cell_end.Copy("P3");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	//������
	border.AttachDispatch(borders.GetItem(8),FALSE);	//xlEdgeTop=8
	border.SetLineStyle(COleVariant((short)-4119));		//xlDouble =-4119;
	border.SetWeight(COleVariant((short)4));			//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	//�ڲ�������
	border.AttachDispatch(borders.GetItem(11),FALSE);	//xlInsideVertical=11
	border.SetLineStyle(COleVariant((short)1));			//xlContinuous =1;
	border.SetWeight(COleVariant((short)2));			//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	//�ײ���
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)1));	//xlContinuous =1;
	border.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();

	iRow = 3;
	for(i=0;i<nElemNum;i++)
	{
		long No[2];
		No[0]=min(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		No[1]=max(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		double L=elem_arr[i]->Length();
		char ss[12];
		if(elem_arr[i]->m_iCtrlRule==1)
			strcpy(ss," KLo/r");
		else if(elem_arr[i]->m_iCtrlRule==2)
			strcpy(ss,"  TENS");
		else if(elem_arr[i]->m_iCtrlRule==3)
			strcpy(ss,"  COMP");
		else if(elem_arr[i]->m_iCtrlRule==4)
			strcpy(ss,"  STAB");
		else if(elem_arr[i]->m_iCtrlRule==5)
			strcpy(ss,"  Code");
		else if(elem_arr[i]->m_iCtrlRule==6)
			strcpy(ss,"  BOLT");
		else //if(elem_arr[i]->pOrgPole->m_iCtrlRule==0)
			strcpy(ss,"     N");
		if(rpt_type!=0&&elem_arr[i]->m_iCtrlRule>0
			&&elem_arr[i]->m_iCtrlRule!=5)
			ss[0]='>';
		//��Ԫ����
		char elemtype[2]="";
		elemtype[0]=elem_arr[i]->pOrgPole->layer(1);
		elemtype[1]=0;
		CString s1,s2,s3,s4,s5,s6,stable_s;
		long module_no=0,status_no=0;
		SUBID caseno;
		hashModuleNo.GetValueAt(elem_arr[i]->MaxTension.hModule,module_no);
		hashStatusNo.GetValueAt(elem_arr[i]->MaxTension.hWorkStatus.Id(),status_no);
		if(elem_arr[i]->MaxTension.hWorkStatus.Group()>0)
		{
			caseno=SUBID(status_no,elem_arr[i]->MaxTension.hWorkStatus.Group());
			s1.Format("%2d-%s",module_no,(char*)caseno.ToString('@',false,3));
		}
		else
		s1.Format("%2d-%3d",module_no,status_no);
		module_no=status_no=0;	//����
		hashModuleNo.GetValueAt(elem_arr[i]->MaxCompression.hModule,module_no);
		hashStatusNo.GetValueAt(elem_arr[i]->MaxCompression.hWorkStatus.Id(),status_no);
		if(elem_arr[i]->MaxCompression.hWorkStatus.Group()>0)
		{
			caseno=SUBID(status_no,elem_arr[i]->MaxCompression.hWorkStatus.Group());
			s2.Format("%2d-%s",module_no,(char*)caseno.ToString('@',false,3));
		}
		else
		s2.Format("%2d-%3d",module_no,status_no);
		if(elem_arr[i]->MaxCompression.hWorkStatus!=elem_arr[i]->MaxCompStable.hWorkStatus&&elem_arr[i]->MaxCompStable.hWorkStatus.Id()>0&&
			fabs(elem_arr[i]->MaxCompStable.N-elem_arr[i]->MaxCompression.N)>10&&elem_arr[i]->m_iCtrlRule==4)
		{
			status_no=0;
			hashStatusNo.GetValueAt(elem_arr[i]->MaxCompStable.hWorkStatus.Id(),status_no);
			if(elem_arr[i]->MaxCompStable.hWorkStatus.Group()>0)
			{
				caseno=SUBID(status_no,elem_arr[i]->MaxCompStable.hWorkStatus.Group());
				stable_s.Format("                                              %7.2f %2d-%s Max Compression Stability Force",elem_arr[i]->MaxCompStable.N*0.001,module_no,(char*)caseno.ToString('@',false,3));
			}
			else
				stable_s.Format("                                              %7.2f %2d-%3d Max Compression Stability Force",elem_arr[i]->MaxCompStable.N*0.001,module_no,status_no);
		}
		CXhChar16 caserule;
		double ctrlstress=0;
		double coefStrength=1.0;
		double A=elem_arr[i]->pOrgPole->GetArea();
		double An=A-elem_arr[i]->design_info.hole_out*elem_arr[i]->design_info.hole_d*elem_arr[i]->pOrgPole->GetThick();
		STEELMAT *pSteelMat=QuerySteelMatByBriefMark(elem_arr[i]->pOrgPole->cMaterial);
		if(elem_arr[i]->design_info.iDangerousLoadCase==2)		//T
		{
			coefStrength=elem_arr[i]->design_info.fCoefTensStrengthDecrease;
			ctrlstress=elem_arr[i]->MaxTension.N/An;
		}
		else if(elem_arr[i]->design_info.iDangerousLoadCase==3)	//C
		{
			coefStrength=elem_arr[i]->design_info.fCoefCompStrengthDecrease;
			ctrlstress=elem_arr[i]->MaxCompression.N/An;
		}
		else //if(elem_arr[i]->design_info.iDangerousLoadCase==4)	//S
		{
			coefStrength=elem_arr[i]->design_info.mN;
			if(elem_arr[i]->MaxCompression.N>-EPS||elem_arr[i]->design_info.fCoefStablity<EPS)
				ctrlstress=0;
			else	//�������ѹ��Ӧ�����ֹ�����һ�������ѹ��������һ�£��ʲ��������ѹ������ѹ��Ӧ�����ƹ����µ�ѹ��ϵ�� wjh-2013.10.15
				ctrlstress=elem_arr[i]->MaxCompStable.N/(A*elem_arr[i]->design_info.fCoefStablity);
		}
		double limStrength=pSteelMat->f(elem_arr[i]->GetThick())*coefStrength;
		if(g_sysPara.iEfficacyCeofType==0)
		{	//ԣ��ϵ��
			if(elem_arr[i]->design_info.iDangerousRule==1)
				caserule.Printf("%5.3f��",elem_arr[i]->design_info.fCoefEffic);
			else if(elem_arr[i]->design_info.iDangerousRule==2)
			{
				if(elem_arr[i]->design_info.bBlockShearActivated)
					caserule.Printf("%5.3f+T",elem_arr[i]->design_info.fCoefEffic);
				else
					caserule.Printf("%5.3f T",elem_arr[i]->design_info.fCoefEffic);
			}
			else if(elem_arr[i]->design_info.iDangerousRule==3)
				caserule.Printf("%5.3f C",elem_arr[i]->design_info.fCoefEffic);
			else if(elem_arr[i]->design_info.iDangerousRule==4)
			{
				if(elem_arr[i]->MaxCompStable.bothComp)
					caserule.Printf("%5.3f+S",elem_arr[i]->design_info.fCoefEffic);
				else
					caserule.Printf("%5.3f S",elem_arr[i]->design_info.fCoefEffic);
			}
			else
				caserule.Printf("%5.3f 0",elem_arr[i]->design_info.fCoefEffic);
		}
		else
		{	//����ϵ��
			double effic=0;
			if(coefStrength>EPS)
				effic=fabs(ctrlstress)/limStrength;
			if(elem_arr[i]->design_info.iDangerousLoadCase==2)		//T
			{
				if(elem_arr[i]->design_info.bBlockShearActivated)
					caserule.Printf("%5.3f+T",effic);
				else
					caserule.Printf("%5.3f T",effic);
			}
			else if(elem_arr[i]->design_info.iDangerousLoadCase==3)	//C
				caserule.Printf("%5.3f C",effic);
			else if(elem_arr[i]->design_info.iDangerousLoadCase==4)	//S
			{
				if(elem_arr[i]->MaxCompStable.bothComp)
					caserule.Printf("%5.3f+S",effic);
				else
					caserule.Printf("%5.3f S",effic);
			}
			else	//���Ҳ���ܲ������������治�ÿ�
				caserule.Printf("0.000@ S");
		}
		char maxCompForceSymbol=' ';
		if(elem_arr[i]->MaxCompStable.N<elem_arr[i]->MaxCompression.N)
			maxCompForceSymbol='>';
		char lamda_s[10];
		//if(elem_arr[i]->LamdaMax()>elem_arr[i]->Lamda())
		//	_snprintf(lamda_s,9,">%3.0f",elem_arr[i]->Lamda());
		//else
		_snprintf(lamda_s,9," %3.0f",elem_arr[i]->Lamda());
		CXhChar200 size_spec;
		if(elem_arr[i]->pOrgPole->size_idClassType==CLS_CABLE)
			size_spec.Copy(elem_arr[i]->pOrgPole->GetSpec());
		else
			size_spec.Printf("%s%3.0fX%2.0f%C",elem_arr[i]->pOrgPole->GetBriefPartTypeMark(),elem_arr[i]->GetWidth(),elem_arr[i]->GetThick(),elem_arr[i]->pOrgPole->cMaterial);
		if(rpt_type!=0)
		{
			CXhChar100 sTmp;
			sTmp.Printf("%4d-%4d",No[0],No[1]);
			mapPage9.SetValueAt(iRow,0,COleVariant(sTmp));
			mapPage9.SetValueAt(iRow,1,COleVariant(ss));
			mapPage9.SetValueAt(iRow,2,COleVariant(elemtype));
			mapPage9.SetValueAt(iRow,3,COleVariant(size_spec));
			sTmp.Printf("%7.2f",elem_arr[i]->MaxTension.N/1000);
			mapPage9.SetValueAt(iRow,4,COleVariant(sTmp));
			mapPage9.SetValueAt(iRow,5,COleVariant(s1));
			sTmp.Printf("%C%7.2f",maxCompForceSymbol,elem_arr[i]->MaxCompression.N/1000);
			mapPage9.SetValueAt(iRow,6,COleVariant(sTmp));
			mapPage9.SetValueAt(iRow,7,COleVariant(s2));
			sTmp.Printf("%6.1f",ctrlstress);
			mapPage9.SetValueAt(iRow,8,COleVariant(sTmp));
			sTmp.Printf("%6.1f",limStrength);
			mapPage9.SetValueAt(iRow,9,COleVariant(sTmp));
			sTmp.Printf("%6.3f",elem_arr[i]->design_info.fCoefStablity);
			mapPage9.SetValueAt(iRow,10,COleVariant(sTmp));
			sTmp.Printf("%6.3f",coefStrength);
			mapPage9.SetValueAt(iRow,11,COleVariant(sTmp));
			sTmp.Printf("%4.0f",L);
			mapPage9.SetValueAt(iRow,12,COleVariant(sTmp));
			sTmp.Printf("%4.0f",elem_arr[i]->LamdaL());
			mapPage9.SetValueAt(iRow,13,COleVariant(sTmp));
			mapPage9.SetValueAt(iRow,14,COleVariant(lamda_s));
			sTmp.Printf("%4.0f",elem_arr[i]->design_info.UltimateLo_r);
			mapPage9.SetValueAt(iRow,15,COleVariant(sTmp));
			mapPage9.SetValueAt(iRow,16,COleVariant(caserule));
		}
		else
		{
			CXhChar100 sTmp;
			sTmp.Printf("%4d-%4d",No[0],No[1]);
			mapPage9.SetValueAt(iRow,0,COleVariant(sTmp));
			mapPage9.SetValueAt(iRow,1,COleVariant(elemtype));
			mapPage9.SetValueAt(iRow,2,COleVariant(size_spec));
			sTmp.Printf("%7.2f",elem_arr[i]->MaxTension.N/1000);
			mapPage9.SetValueAt(iRow,3,COleVariant(sTmp));
			mapPage9.SetValueAt(iRow,4,COleVariant(s1));
			sTmp.Printf("%C%7.2f",maxCompForceSymbol,elem_arr[i]->MaxCompression.N/1000);
			mapPage9.SetValueAt(iRow,5,COleVariant(sTmp));
			mapPage9.SetValueAt(iRow,6,COleVariant(s2));
			sTmp.Printf("%6.1f",ctrlstress);
			mapPage9.SetValueAt(iRow,7,COleVariant(sTmp));
			sTmp.Printf("%6.1f",limStrength);
			mapPage9.SetValueAt(iRow,8,COleVariant(sTmp));
			sTmp.Printf("%6.3f",elem_arr[i]->design_info.fCoefStablity);
			mapPage9.SetValueAt(iRow,9,COleVariant(sTmp));
			sTmp.Printf("%6.3f",coefStrength);
			mapPage9.SetValueAt(iRow,10,COleVariant(sTmp));
			sTmp.Printf("%4.0f",L);
			mapPage9.SetValueAt(iRow,11,COleVariant(sTmp));
			sTmp.Printf("%4.0f",elem_arr[i]->LamdaL());
			mapPage9.SetValueAt(iRow,12,COleVariant(sTmp));
			mapPage9.SetValueAt(iRow,13,COleVariant(lamda_s));
			sTmp.Printf("%4.0f",elem_arr[i]->design_info.UltimateLo_r);
			mapPage9.SetValueAt(iRow,14,COleVariant(sTmp));
			mapPage9.SetValueAt(iRow,15,COleVariant(caserule));
		}
		//TODO: ��һ������mapPage9�޷���̬����,�ݻ�û��� wjh-2013.12.10
		//if(elem_arr[i]->MaxCompression.hWorkStatus!=elem_arr[i]->MaxCompStable.hWorkStatus)
		//	fprintf(fp,"%s\n",stable_s);
		iRow++;
	}
	cell_end.Printf("%C%d",'P'+colheader_incre,iRow);
	strcpy(cell_start,"A3");
	//���뷽ʽ
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetHorizontalAlignment(COleVariant((short)-4152));	//xlRight:-4152
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();
	//����
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,mapPage9.var);
	excel_sheet.DetachDispatch();
	pWorksheet->Release();

	/*
	//��Xҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("����Ԫ������Ϣ");

	//��������
	CVariant2dArray mapPage9(nElemNum+3,16);//���ݵĶ�ά����
	
	strcpy(cell_start,"A1");
	strcpy(cell_end,"G2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.Merge(COleVariant((BYTE)0));
	mapPage9.SetValueAt(0,0,COleVariant("�ڢ�����: ����Ԫ������Ϣ"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	excel_range.DetachDispatch();
	pRange->Release();
	*/
	delete []elem_arr;

	char save_file_name[100];
	strcpy(save_file_name,file_name);
	
	if(rpt_type!=0)
		strcat(save_file_name,"_Check.xls");
	else
		strcat(save_file_name,"_Report.xls");

	DeleteFile(save_file_name);
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND,   VT_ERROR);  
	excel_workbook.SaveAs(COleVariant(save_file_name),covOptional,covOptional, covOptional,covOptional,covOptional,0,covOptional,covOptional,covOptional,covOptional);
	//�ͷ�
	excel_sheets.ReleaseDispatch();
	excel_workbook.ReleaseDispatch();
	excel_workbooks.ReleaseDispatch();
	excel_app.ReleaseDispatch();
	excel_app.Quit();
	
	pCmdLine->PromptStrInCmdLine("���İ���ϸ�����ļ������ɣ�");
}
#endif