#ifndef __FORMAT_TABLE_H_
#define __FORMAT_TABLE_H_

#include "stdafx.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "TblDef.h"
#ifdef __LOCALDEF_ENTITYDATABASE_
#include "EntityDataBase.h"
#else
#include "IXeroCad.h"
#endif

class CFormatTbl;
class CGrid;
/*HIDE_CLASS */class CGridMap
{
	CGrid *grid_map;		//表格存储区
	int m_nRow,m_nColumn;	//行总数与列总数
public:
	CGridMap();
	~CGridMap();
public:
	void Destroy();
	BOOL Create(int nRow,int nColumn);
	int GetRowsCount(){return m_nRow;}
	int GetColumnsCount(){return m_nColumn;}
	BOOL InsertRow(int iRow);
	BOOL DelRow(int iRow);
	BOOL InsertColumn(int iColumn);
	BOOL DelColumn(int iColumn);

	CGrid* GetGridAt(int iRow,int iColumn);					//根据行列号获得指定单元格指针
	BOOL SetGrid(int iRow, int iColumn,const CGrid &grid);	//填充单元格内容
};
class CFlexTbl;
class CFormatTbl
{
	long ID;
	friend class CFlexTbl;
public:
	CFormatTbl();
	~CFormatTbl();
private:
	TBL_LOCATE_STYLE locate_style;//左上角;左下角;右下角;右上角
	f2dPoint base_pos;			//基(定位)点二维坐标
	double *rows_arr_high;			//行高列表
	double *columns_arr_wide;		//列宽列表
	CGridMap grid_map;			//表格存储区
	double getRowLinePos(BOOL bUpToDown,int iRow);		//获得指定行线距离格式表相应外缘的位置距离
	double getColumnLinePos(BOOL bLToR,int iColumn);	//获得指定列线距离格式表相应外缘的位置距离
public:
#ifndef __MAPTA_
	HRESULT WriteFormatTbl(IStream *pStream);
	HRESULT ReadFormatTbl(IStream *pStream);
#endif
	void CopyFrom(CFormatTbl *pFormatTbl);
	//复制表格但不复制iRemoveStart至iRemoveEnd之间的行 wht 11-03-13
	void CopyFrom(CFormatTbl *pFormatTbl,int iRemoveStart,int iRemoveEnd);
	BOOL SnapGridPos(double x,double y, f2dPoint *snap_pos,double scope=1.0);
	long GetFormatTblID();
	BOOL GetTblRect(f2dRect *tbl_rect);
	CGrid * SnapGrid(double x,double y,int *iRow=NULL,int *iCol=NULL);
	void SelectAll();
	int ReleaseSnapStatus();
	int GetSelectedCounts();
	CGrid *GetFirstSelectedGrid(CFormatTbl **pFormatTbl=NULL,int *pos_i=NULL,int *pos_j=NULL);
	void Destroy();			//销毁表格,释放内存
	int GetColumnsCount();	//获得表格总列数
	int GetRowsCount();		//获得表格总行数
	BOOL Create(int iRow,int iColumn);						//初始化并生成缺省指定行列数表格
	long GetID(){return ID;}
	BOOL IsValidGridIndex(int iRow=1,int iColumn=1);		//行列索引号标识有效性判断
	CGrid* GetGridAt(int iRow,int iColumn);					//根据行列号获得指定单元格指针
	BOOL GetGridRect(int iRow,int iColumn,f2dRect &rect);	//获得指定单元格位置矩形
	BOOL GetGridSize(int iRow,int iColumn,					//获得指定单元格大小(高宽)
			double *height=NULL, double *wide=NULL);
	BOOL GetGridRange(int &start_i,int &start_j,int *end_i, int *end_j);		//获得指定单元格大小(高宽)
			
	//iRow和iColumn必须大于等于0并分别小于规格表的行总数和列总数
	BOOL SetGrid(int iRow, int iColumn,char *string,BOOL bForceToString=TRUE);	//填充单元格内容
	BOOL SetRowHigh(int iRow,double high);					//设定指定行高
	BOOL SetColumnWide(int iColumn,double wide);			//设定指定列宽
	BOOL GetGridString(int iRow, int iColumn,char *string);	//获得指定单元格的数据字符串
	BOOL GetRowHigh(int iRow,double *high);					//获得指定行高
	BOOL GetColumnWide(int iColumn,double *wide);			//获得指定列宽
	BOOL InsertRow(int iRow,double high=0);
	BOOL DelRow(int iRow);
	BOOL InsertColumn(int iColumn,double wide=0);
	BOOL DelColumn(int iColumn);
	BOOL CleanGrid(int iStartRow,int iEndRow);				//清空指定行的单元格内容

	//iRow和iColumn必须大于等于0并分别小于等于规格表的行总数和列总数
	BOOL GetRowLineStart(int iRow,f2dPoint &start);			//获得指定行线的起始位置
	BOOL GetRowLineEnd(int iRow,f2dPoint &end);				//获得指定行线的终止位置
	BOOL GetColumnLineStart(int iColumn,f2dPoint &start);	//获得指定列线的起始位置
	BOOL GetColumnLineEnd(int iColumn,f2dPoint &end);		//获得指定列线的终止位置

	void SetBasePos(f2dPoint &pos);						//设定定位点二维坐标
	void GetBasePos(f2dPoint &pos);						//获得格式表的定位点二维坐标
	void SetLocateStyle(TBL_LOCATE_STYLE eStyle);		//设定表格定位形式
	TBL_LOCATE_STYLE GetLocateStyle();					//获得格式表的定位方式
	double GetTableWide();								//获得整张格式表的总宽
	double GetTableHeight();							//获得整张格式表的总高
	BOOL Import(char *filename);						//从文本文件导入格式表
	BOOL Export(char *filename);						//向文本文件导出格式表
	BOOL Import(FILE *fp);						//从文本文件导入格式表
	BOOL Export(FILE *fp);						//向文本文件导出格式表
};

class CFlexTbl  
{
	int iCursor;
	long curFormatTblID;
	long m_nFormatTblCounts;
public:
	CFlexTbl();
	virtual ~CFlexTbl();
	void RemoveAll();
	CArray<CFormatTbl*,CFormatTbl*&>flex_tbl;
public:
	BOOL Import(char *filename);						//从文本文件导入柔性表
	BOOL Export(char *filename);						//向文本文件导出柔性表
	void CopyFrom(CFlexTbl *pFlexTbl);
	BOOL SnapGridPos(double x,double y, long nExcludeFormatTblID,
		f2dPoint *snap_pos,double scope=1.0);
	CGrid * SnapGrid(double x,double y,int *iRow=NULL,int *iCol=NULL,long *nFormatTblID=NULL);
	int ReleaseSnapStatus();
	int GetSelectedCounts();
	CGrid *GetFirstSelectedGrid(CFormatTbl **pFormatTbl=NULL,int *pos_i=NULL,int *pos_j=NULL);
	CFormatTbl *GetFirst();
	CFormatTbl *GetNext();
	CFormatTbl *GetFormatTblByID(long ID);
	long AddFormatTbl(f2dPoint base_pos,TBL_LOCATE_STYLE mode,int nRow, int nColumn);
	CFormatTbl *AddEmptyFormatTbl();
	BOOL DelFormatTbl(long ID);
	long GetFormatTblCount();
	BOOL CreateTblFrame(CFormatTbl* pFormatTbl,IEntityDatabase*pEDB);
	CFormatTbl *SnapFormatTbl(double x, double y);

};

#endif