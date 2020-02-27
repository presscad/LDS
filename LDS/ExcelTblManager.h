// ExcelTblManager.h: interface for the CExcelTblManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXCELTBLMANAGER_H__52CDE700_9A70_4300_9A0A_E0624A7FE69A__INCLUDED_)
#define AFX_EXCELTBLMANAGER_H__52CDE700_9A70_4300_9A0A_E0624A7FE69A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "f_ent_list.h"

typedef struct tagCell
{
	char cell_name[8];
	char cell_content[100];
	int  string0_keyword1;
}CELL_CONTENT;
typedef struct tagMergeCellCmd
{
	char cell_start[8];
	char cell_end[8];
}MERGECELL_CMD;
typedef struct tagColumnCell
{
	char cell_collumn;
	char cell_content[100];
}COLUMN_CELL;
class CExcelTblManager  
{
public:
	int title_row,title_column;
	CExcelTblManager();
	virtual ~CExcelTblManager();
	BOOL InitExcelDataDriver(char *nc_driver);
	ATOM_LIST<CELL_CONTENT> cell_content;
	ATOM_LIST<MERGECELL_CMD>mergecell_cmd;
	ATOM_LIST<COLUMN_CELL> column_cell;
};

#endif // !defined(AFX_EXCELTBLMANAGER_H__52CDE700_9A70_4300_9A0A_E0624A7FE69A__INCLUDED_)
