// ExcelTblManager.cpp: implementation of the CExcelTblManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "LDS.h"
#include "ExcelTblManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExcelTblManager::CExcelTblManager()
{
	title_row=title_column=0;
}

CExcelTblManager::~CExcelTblManager()
{

}
BOOL CExcelTblManager::InitExcelDataDriver(char *driver)
{
	if(driver==NULL)
		return FALSE;
	else if(strlen(driver)==0)
		return FALSE;
	FILE *fp =fopen(driver,"rt");
	if(fp==NULL)
		return FALSE;
	char line_txt[200];
	cell_content.Empty();
	mergecell_cmd.Empty();
	column_cell.Empty();
	if(fgets(line_txt,200,fp)==NULL)
		return FALSE;
	sscanf(line_txt,"%d%d",&title_row,&title_column);
	while(!feof(fp))
	{
		if(fgets(line_txt,200,fp)==NULL)
			break;
		line_txt[strlen(line_txt)-1]='\0';
		char szTokens[] = " -,=\n" ;
		
		char* szToken = strtok(line_txt, szTokens) ; 
		if(szToken&&_stricmp(szToken,"</TITLE>")==0)
		{
			while(!feof(fp))
			{
				if(fgets(line_txt,200,fp)==NULL)
					break;
				line_txt[strlen(line_txt)-1]='\0';
				szToken = strtok(line_txt, szTokens) ; 
				if(_stricmp(szToken,"MERGE")==0)
				{	//合并单元格命令
					MERGECELL_CMD *pCmd=mergecell_cmd.append();
					strcpy(pCmd->cell_start,strtok(NULL, szTokens));
					strcpy(pCmd->cell_end,strtok(NULL, szTokens));
				}
				else if(_stricmp(szToken,"<TITLE>")==0)
					break;
				else//标题单元格
				{
					CELL_CONTENT *pCell=cell_content.append();
					strcpy(pCell->cell_name,szToken);
					strcpy(pCell->cell_content,strtok(NULL,szTokens));
					szToken=strtok(NULL,szTokens);
					if(stricmp(szToken,"STRING")==0)
						pCell->string0_keyword1=0;
					else if(stricmp(szToken,"KEYWORD")==0)
						pCell->string0_keyword1=1;
				}
			}
		}
		else if(szToken&&_stricmp(szToken,"</DATA>")==0)
		{
			while(!feof(fp))
			{
				if(fgets(line_txt,200,fp)==NULL)
					break;
				szToken = strtok(line_txt, szTokens) ; 
				if(_stricmp(line_txt,"<DATA>")==0)
					break;
				COLUMN_CELL *pCell=column_cell.append();
				pCell->cell_collumn=szToken[0];
				strcpy(pCell->cell_content,strtok(NULL,szTokens));
			}
		}
	}
	fclose(fp);
	return TRUE;
}
