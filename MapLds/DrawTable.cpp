//<LOCALE_TRANSLATE/>
#include "stdafx.h"
#include "stdarx.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "MxbDlg.h"
#include "MenuFunc.h"
#include "Query.h"
#include "FormatTable.h"
#include "RxTools.h"
#include "AdsTools.h"
#include "DragEntSet.h"

//from AdsTools.cpp
int DragEntSet(ads_point base, char *prompt);
//���ƶ�ά����д���е�����,��ͨ��nLocatePos���ⲿֱ��ָ������϶�λ�� wht 10-12-04
void DrawTable(AcDbBlockTableRecord *pBlockTableRecord, CFormatTbl *pTable,
				BOOL bPromptPos/*=TRUE*/, double scale/*=1*/, int nLocatePos/*=-1*/,
				const double text_margin/*=1.5*/)
{
	CMxbDlg dlg;
	int i,j;//,nColumnN;
	//int LocationPntStyle,LayoutStyle;
	f3dPoint LeftBottomPos,TopRightPos,text_base;
	ads_point box_LT,box_LB,box_RB,box_RT;
	f3dPoint line_start,line_end,insert_pos;
	f2dPoint start,end;
	f2dRect rect;
    CAcModuleResourceOverride resOverride;
	//��WCS��ͼ�����½��ߵ�X,Y����
	if(pTable->GetColumnsCount()<=0||pTable->GetRowsCount()<=0)
		return;
	if(bPromptPos)
	{
		//�Զ��������ʼλ��
		AcGePoint3d extmin=GetCurDwg()->extmin();
		AcGePoint3d extmax=GetCurDwg()->extmax();
		Cpy_Pnt(LeftBottomPos,extmin);	
		Cpy_Pnt(TopRightPos,extmax);
		if(fabs(LeftBottomPos.x)>1e5)
			LeftBottomPos.x = LeftBottomPos.y = 0;
		if(fabs(TopRightPos.x)>1e5)
			TopRightPos.x = TopRightPos.y = 0;
		//
		double wide,height;
		wide = pTable->GetTableWide();
		height = pTable->GetTableHeight();
		INT_PTR nRetCode=0;
		if(nLocatePos==-1)	//ѡ��λλ��
			nRetCode=dlg.DoModal();
		else
		{	//�ⲿָ����λλ��
			nRetCode=IDOK;	
			dlg.m_nLocatePos=nLocatePos;
			f2dPoint table_pos;
			pTable->GetBasePos(table_pos);
			if(dlg.m_nLocatePos==0)		//���Ͻ�
			{
				LeftBottomPos.Set(table_pos.x,table_pos.y-height);
				TopRightPos.Set(table_pos.x+wide,table_pos.y);
			}
			else if(dlg.m_nLocatePos==1)//���Ͻ�
			{
				LeftBottomPos.Set(table_pos.x-wide,table_pos.y-height);
				TopRightPos.Set(table_pos.x,table_pos.y);
			}
			else if(dlg.m_nLocatePos==2)//���½�
			{
				LeftBottomPos.Set(table_pos.x,table_pos.y);
				TopRightPos.Set(table_pos.x+wide,table_pos.y+height);
			}
			else if(dlg.m_nLocatePos==3)//���½�
			{
				LeftBottomPos.Set(table_pos.x-wide,table_pos.y);
				TopRightPos.Set(table_pos.x,table_pos.y+height);
			}
		}
		if(nRetCode==IDOK)
		{
			if(dlg.m_nLocatePos==0)		//���Ͻ�
			{
				rect.topLeft.Set(LeftBottomPos.x,TopRightPos.y);
				rect.bottomRight.Set(LeftBottomPos.x+wide,TopRightPos.y-height);
				insert_pos[X] = LeftBottomPos.x;
				insert_pos[Y] = TopRightPos.y;
				insert_pos[Z] = 0;
			}
			else if(dlg.m_nLocatePos==1)//���Ͻ�
			{
				pTable->SetLocateStyle(TOP_RIGHT);
				rect.topLeft.Set(TopRightPos.x-wide,TopRightPos.y);
				rect.bottomRight.Set(TopRightPos.x,TopRightPos.y-height);
				insert_pos[X] = TopRightPos.x;
				insert_pos[Y] = TopRightPos.y;
				insert_pos[Z] = 0;
			}
			else if(dlg.m_nLocatePos==2)//���½�
			{
				pTable->SetLocateStyle(BOTTOM_LEFT);
				rect.topLeft.Set(LeftBottomPos.x,LeftBottomPos.y+height);
				rect.bottomRight.Set(LeftBottomPos.x+wide,LeftBottomPos.y);
				insert_pos[X] = LeftBottomPos.x;
				insert_pos[Y] = LeftBottomPos.y;
				insert_pos[Z] = 0;
			}
			else if(dlg.m_nLocatePos==3)//���½�
			{
				pTable->SetLocateStyle(BOTTOM_RIGHT);
				rect.topLeft.Set(TopRightPos.x-wide,LeftBottomPos.y+height);
				rect.bottomRight.Set(TopRightPos.x,LeftBottomPos.y);
				insert_pos[X] = TopRightPos.x;
				insert_pos[Y] = LeftBottomPos.y;
				insert_pos[Z] = 0;
			}
			else		//����׼��λ�÷Ƿ�,�˳�����
				return;
		}
		else
			return;
		
		//---VVVV---����һ�����ο���ʾ�û���λ���ϱ�Ĳ���λ��---VVVV---
		box_LT[Z] = box_LB[Z] = box_RB[Z] = box_RT[Z] = 0;
		box_LT[X] = rect.topLeft.x;
		box_LT[Y] = rect.topLeft.y;
		box_LB[X] = rect.topLeft.x;
		box_LB[Y] = rect.bottomRight.y;
		box_RB[X] = rect.bottomRight.x;
		box_RB[Y] = rect.bottomRight.y;
		box_RT[X] = rect.bottomRight.x;
		box_RT[Y] = rect.topLeft.y;
		AcDbPolyline *pPoly=new AcDbPolyline(4);
		pPoly->addVertexAt(0,AcGePoint2d(box_LT[X],box_LT[Y]));
		pPoly->addVertexAt(1,AcGePoint2d(box_LB[X],box_LB[Y]));
		pPoly->addVertexAt(2,AcGePoint2d(box_RB[X],box_RB[Y]));
		pPoly->addVertexAt(3,AcGePoint2d(box_RT[X],box_RT[Y]));
		AcDbObjectId polyId;
		DRAGSET.ClearEntSet();
		if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,polyId,pPoly))
			pPoly->close();
#ifdef AFX_TARG_ENU_ENGLISH
		nRetCode=DragEntSet(insert_pos,"\nPlease click the position of material table,<Enter Exit>: ");
#else
		nRetCode=DragEntSet(insert_pos,"\n�����ϱ�λ��,<Enter�˳�>: ");
#endif
		DRAGSET.ClearEntSet();
		acdbOpenObject(pPoly,polyId,AcDb::kForWrite);
		pPoly->erase();
		pPoly->close();
		//---^^^^---����һ�����ο���ʾ�û���λ���ϱ�Ĳ���λ��---^^^^---
		if(nRetCode==RTNORM)
		{	//δ�����Ϸ�ʱ���λ�ò��䶯
			f2dPoint table_pos;
			table_pos.x = insert_pos[X];
			table_pos.y = insert_pos[Y];
			pTable->SetBasePos(table_pos);
		}
	}
	else
	{
		if(nLocatePos==0)		//���Ͻ�
			pTable->SetLocateStyle(TOP_LEFT);
		else if(nLocatePos==1)//���Ͻ�
			pTable->SetLocateStyle(TOP_RIGHT);
		else if(nLocatePos==2)//���½�
			pTable->SetLocateStyle(BOTTOM_LEFT);
		else if(nLocatePos==3)//���½�
			pTable->SetLocateStyle(BOTTOM_RIGHT);
	}
	// ��ʾ�û���Ϣ
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
	ads_prompt(L"\nPlease wait for a while......");
#else
	ads_prompt("\nPlease wait for a while......");
#endif
#else
#ifdef _ARX_2007
	ads_prompt(L"\n���Ժ�......");
#else
	ads_prompt("\n���Ժ�......");
#endif
#endif
#ifdef _ARX_2007
	acedCommand(RTSTR,L"undo",RTSTR,L"be",RTNONE);
#else
	acedCommand(RTSTR,"undo",RTSTR,"be",RTNONE);
#endif
	f2dRect grid_rect;
	int nColumnsCount = pTable->GetColumnsCount();
	int nRowsCount	  = pTable->GetRowsCount();

	//��д�����⼰����
	for(i=0;i<nRowsCount;i++)
	{
		for(j=0;j<nColumnsCount;j++)
		{
			double width,text_height;
			CGrid *pGrid = pTable->GetGridAt(i,j);
			if(pGrid->m_bHGridVirtual||pGrid->m_bVGridVirtual)//����Ԫ
				continue;
			pTable->GetGridRect(i,j,grid_rect);
			if(pGrid->text_angle==0)
				width = fabs(grid_rect.bottomRight.x-grid_rect.topLeft.x);
			else
				width = fabs(grid_rect.bottomRight.y-grid_rect.topLeft.y);
			if(pGrid->grid_mode==PIC_BLOCK_GRID)	//���Ԫ�ڲ���ͼ��
			{
				f3dPoint insert_pos;
				insert_pos.x = (grid_rect.topLeft.x+grid_rect.bottomRight.x)/2.0;
				insert_pos.y = (grid_rect.topLeft.y+grid_rect.bottomRight.y)/2.0;
				InsertBlock(pBlockTableRecord,insert_pos,pGrid->data.sVal,1.0);
				continue;
			}
			text_height = pGrid->textsize*scale;
			if(pGrid->feature%10==0)	//���������
			{
				text_base.x = grid_rect.bottomRight.x-text_margin*scale;
				text_base.y = (grid_rect.topLeft.y+grid_rect.bottomRight.y)/2.0;
				text_base.z = 0;
				if(pGrid->feature/10==0)	//����������д
					DimMText(pBlockTableRecord,pTable->GetGridAt(i,j)->data.sVal,
						text_base,width,text_height,TextStyleTable::hzfs.textStyleId,AcDbMText::kMiddleRight,
						AcDbMText::kLtoR,pGrid->text_angle);
				else						//����������д
					DimMText(pBlockTableRecord,pTable->GetGridAt(i,j)->data.sVal,
						text_base,width,text_height,TextStyleTable::hzfs.textStyleId,AcDbMText::kMiddleRight,
						AcDbMText::kTtoB,pGrid->text_angle);
			}
			else if(pGrid->feature%10==1)//���������
			{
				text_base.x = (grid_rect.topLeft.x+grid_rect.bottomRight.x)/2.0;
				text_base.y = (grid_rect.topLeft.y+grid_rect.bottomRight.y)/2.0;
				text_base.z = 0;
				if(pGrid->feature/10==0)	//����������д
					DimMText(pBlockTableRecord,pTable->GetGridAt(i,j)->data.sVal,
						text_base,width,text_height,TextStyleTable::hzfs.textStyleId,AcDbMText::kMiddleCenter,
						AcDbMText::kLtoR,pGrid->text_angle);
				else						//����������д
					DimMText(pBlockTableRecord,pTable->GetGridAt(i,j)->data.sVal,
						text_base,width,text_height,TextStyleTable::hzfs.textStyleId,AcDbMText::kMiddleCenter,
						AcDbMText::kTtoB,pGrid->text_angle);
			}
			else if(pGrid->feature%10==4)//���������
			{
				text_base.x = grid_rect.topLeft.x+text_margin*scale;
				text_base.y = (grid_rect.topLeft.y+grid_rect.bottomRight.y)/2.0;
				text_base.z = 0;
				if(pGrid->feature/10==0)	//����������д
					DimMText(pBlockTableRecord,pTable->GetGridAt(i,j)->data.sVal,
					text_base,width,text_height,TextStyleTable::hzfs.textStyleId,AcDbMText::kMiddleLeft,
						AcDbMText::kLtoR,pGrid->text_angle);
				else						//����������д
					DimMText(pBlockTableRecord,pTable->GetGridAt(i,j)->data.sVal,
						text_base,width,text_height,TextStyleTable::hzfs.textStyleId,AcDbMText::kMiddleLeft,
						AcDbMText::kTtoB,pGrid->text_angle);
			}
			else if(pGrid->feature%10==2)//���½Ƕ���
			{
				text_base.x = grid_rect.bottomRight.x-text_margin*scale;
				text_base.y = grid_rect.bottomRight.y+0.5*scale;
				text_base.z = 0;
				if(pGrid->feature/10==0)	//����������д
					DimMText(pBlockTableRecord,pTable->GetGridAt(i,j)->data.sVal,
						text_base,width,text_height,TextStyleTable::hzfs.textStyleId,AcDbMText::kBottomRight,
						AcDbMText::kLtoR,pGrid->text_angle);
				else						//����������д
					DimMText(pBlockTableRecord,pTable->GetGridAt(i,j)->data.sVal,
						text_base,width,text_height,TextStyleTable::hzfs.textStyleId,AcDbMText::kBottomRight,
						AcDbMText::kTtoB,pGrid->text_angle);
			}
			else if(pGrid->feature%10==3)//���ϽǶ���
			{
				text_base.x = grid_rect.topLeft.x+text_margin*scale;
				text_base.y = grid_rect.topLeft.y-0.5*scale;
				text_base.z = 0;
				if(pGrid->feature/10==0)	//����������д
					DimMText(pBlockTableRecord,pTable->GetGridAt(i,j)->data.sVal,
						text_base,width,text_height,TextStyleTable::hzfs.textStyleId,AcDbMText::kTopLeft,
						AcDbMText::kLtoR,pGrid->text_angle);
				else						//����������д
					DimMText(pBlockTableRecord,pTable->GetGridAt(i,j)->data.sVal,
						text_base,width,text_height,TextStyleTable::hzfs.textStyleId,AcDbMText::kBottomRight,
						AcDbMText::kTtoB,pGrid->text_angle);
			}
		}
	}
	//�����б����(����)
	for (i=0;i<=nRowsCount;i++)
	{
		pTable->GetRowLineStart(i,start);
		pTable->GetRowLineEnd(i,end);
		line_start.Set(start.x,start.y);
		line_end.Set(end.x,end.y);
		for(j=0;j<nColumnsCount;j++)
		{	//�������е�����Ԫ
			if(i==0||i==nRowsCount)	//��ĩ�в����ж��鵥Ԫ
				break;
			double wide;
			pTable->GetColumnWide(j,&wide);	//��õ�j�б���
			if(j==0&&pTable->GetGridAt(i,j)->m_bVGridVirtual)
			{		//���о�Ϊ�鵥Ԫ
				line_start.x += wide;
				line_end.x = line_start.x;
			}
			else if(pTable->GetGridAt(i,j)->m_bVGridVirtual==TRUE)
			{	//��ǰ��Ϊ�鵥Ԫ
				if(!pTable->GetGridAt(i,j-1)->m_bVGridVirtual)
				{
					CreateAcadLine(pBlockTableRecord,line_start,line_end);
					line_start.x = line_end.x+wide;
					line_end.x = line_start.x;
				}
				else
				{
					line_end.x += wide;
					line_start.x = line_end.x;
				}
			}
			else	//��ǰ�в����鵥Ԫ,�޸�����β����
			{
				if(j==0)
					line_end.x = line_start.x+wide;
				else
					line_end.x += wide;
			}
		}
		CreateAcadLine(pBlockTableRecord,line_start,line_end);
	}
	// ���ɱ������(����)
	for(j=0;j<=nColumnsCount;j++)
	{
		pTable->GetColumnLineStart(j,start);
		pTable->GetColumnLineEnd(j,end);
		line_start.Set(start.x,start.y);
		line_end.Set(end.x,end.y);
		for(i=0;i<nRowsCount;i++)
		{	//�������е�����Ԫ
			if(j==0||j==nColumnsCount)	//��ĩ�в����ж��鵥Ԫ
				break;
			double high;
			pTable->GetRowHigh(i,&high);
			if(i==0&&pTable->GetGridAt(i,j)->m_bHGridVirtual)
			{		//���о�Ϊ�鵥Ԫ
				line_start.y -= high;
				line_end.y = line_start.y;
			}
			else if(pTable->GetGridAt(i,j)->m_bHGridVirtual)
			{		//��ǰ�����鵥Ԫ
				if(!pTable->GetGridAt(i-1,j)->m_bHGridVirtual)
				{
					CreateAcadLine(pBlockTableRecord,line_start,line_end);
					line_start.y = line_end.y-high;
					line_end.y = line_start.y;
				}
				else
				{
					line_end.y -= high;
					line_start.y = line_end.y;
				}
			}
			else	//��ǰ�в����鵥Ԫ,�޸�����β����
			{
				if(i==0)
					line_end.y = line_start.y - high;
				else
					line_end.y -= high;
			}
		}
		if(fabs(line_start.y-line_end.y)<eps)	//��ĩ���غ�
			continue;
		CreateAcadLine(pBlockTableRecord,line_start,line_end);
	}
#ifdef _ARX_2007
	acedCommand(RTSTR,L"undo",RTSTR,L"e",RTNONE);//������ǰundo״̬
#else
	acedCommand(RTSTR,"undo",RTSTR,"e",RTNONE);//������ǰundo״̬
#endif
}

void DrawTable(CXhTable *pTable, BOOL bPromptPos, double scale, int nLocatePos,AcDbBlockTableRecord *pTemRecord)
{
	if (pTable->nColsCount <= 0 || pTable->nRowsCount <= 0)
		return;
	CAcModuleResourceOverride resOverride;
	AcDbBlockTableRecord *pBlockTableRecord = pTemRecord ? pTemRecord : GetBlockTableRecord();
	if (pBlockTableRecord == NULL)
		return;
	if (bPromptPos)
	{	//�Զ��������ʼλ��
		f3dPoint ptBtmL, ptTopR, insert_pos;
		AcGePoint3d extmin = GetCurDwg()->extmin();
		AcGePoint3d extmax = GetCurDwg()->extmax();
		Cpy_Pnt(ptBtmL, extmin);
		Cpy_Pnt(ptTopR, extmax);
		if (fabs(ptBtmL.x) > 1e5)
			ptBtmL.x = ptBtmL.y = 0;
		if (fabs(ptTopR.x) > 1e5)
			ptTopR.x = ptTopR.y = 0;
		double wide = pTable->GetTableWide();
		double height = pTable->GetTableHeight();
		if (nLocatePos == -1)	//ѡ��λλ��
		{
			CMxbDlg dlg;
			if (dlg.DoModal() != IDOK)
				return;
			nLocatePos = dlg.m_nLocatePos;
		}
		else
		{	//�ⲿָ����λλ��
			f2dPoint table_pos;
			pTable->GetBasePos(table_pos);
			if (nLocatePos == 0)		//���Ͻ�
			{
				ptBtmL.Set(table_pos.x, table_pos.y - height);
				ptTopR.Set(table_pos.x + wide, table_pos.y);
			}
			else if (nLocatePos == 1)//���Ͻ�
			{
				ptBtmL.Set(table_pos.x - wide, table_pos.y - height);
				ptTopR.Set(table_pos.x, table_pos.y);
			}
			else if (nLocatePos == 2)//���½�
			{
				ptBtmL.Set(table_pos.x, table_pos.y);
				ptTopR.Set(table_pos.x + wide, table_pos.y + height);
			}
			else if (nLocatePos == 3)//���½�
			{
				ptBtmL.Set(table_pos.x - wide, table_pos.y);
				ptTopR.Set(table_pos.x, table_pos.y + height);
			}
		}
		f2dRect rect;
		if (nLocatePos == 0)		//���Ͻ�
		{
			pTable->SetLocateStyle(TOP_LEFT);
			rect.topLeft.Set(ptBtmL.x, ptTopR.y);
			rect.bottomRight.Set(ptBtmL.x + wide, ptTopR.y - height);
			insert_pos[X] = ptBtmL.x;
			insert_pos[Y] = ptTopR.y;
			insert_pos[Z] = 0;
		}
		else if (nLocatePos == 1)//���Ͻ�
		{
			pTable->SetLocateStyle(TOP_RIGHT);
			rect.topLeft.Set(ptTopR.x - wide, ptTopR.y);
			rect.bottomRight.Set(ptTopR.x, ptTopR.y - height);
			insert_pos[X] = ptTopR.x;
			insert_pos[Y] = ptTopR.y;
			insert_pos[Z] = 0;
		}
		else if (nLocatePos == 2)//���½�
		{
			pTable->SetLocateStyle(BOTTOM_LEFT);
			rect.topLeft.Set(ptBtmL.x, ptBtmL.y + height);
			rect.bottomRight.Set(ptBtmL.x + wide, ptBtmL.y);
			insert_pos[X] = ptBtmL.x;
			insert_pos[Y] = ptBtmL.y;
			insert_pos[Z] = 0;
		}
		else if (nLocatePos == 3)//���½�
		{
			pTable->SetLocateStyle(BOTTOM_RIGHT);
			rect.topLeft.Set(ptTopR.x - wide, ptBtmL.y + height);
			rect.bottomRight.Set(ptTopR.x, ptBtmL.y);
			insert_pos[X] = ptTopR.x;
			insert_pos[Y] = ptBtmL.y;
			insert_pos[Z] = 0;
		}
		else		//����׼��λ�÷Ƿ�,�˳�����
			return;
		//---VVVV---����һ�����ο���ʾ�û���λ���ϱ�Ĳ���λ��---VVVV---
		ads_point box_LT, box_LB, box_RB, box_RT;
		box_LT[Z] = box_LB[Z] = box_RB[Z] = box_RT[Z] = 0;
		box_LT[X] = rect.topLeft.x;
		box_LT[Y] = rect.topLeft.y;
		box_LB[X] = rect.topLeft.x;
		box_LB[Y] = rect.bottomRight.y;
		box_RB[X] = rect.bottomRight.x;
		box_RB[Y] = rect.bottomRight.y;
		box_RT[X] = rect.bottomRight.x;
		box_RT[Y] = rect.topLeft.y;
		AcDbPolyline *pPoly = new AcDbPolyline(4);
		pPoly->addVertexAt(0, AcGePoint2d(box_LT[X], box_LT[Y]));
		pPoly->addVertexAt(1, AcGePoint2d(box_LB[X], box_LB[Y]));
		pPoly->addVertexAt(2, AcGePoint2d(box_RB[X], box_RB[Y]));
		pPoly->addVertexAt(3, AcGePoint2d(box_RT[X], box_RT[Y]));
		AcDbObjectId polyId;
		DRAGSET.ClearEntSet();
		if (DRAGSET.AppendAcDbEntity(pBlockTableRecord, polyId, pPoly))
			pPoly->close();
		int nRetCode = 0;
#ifdef AFX_TARG_ENU_ENGLISH
		nRetCode = DragEntSet(insert_pos, "\nPlease click the position of material table,<Enter Exit>: ");
#else
		nRetCode = DragEntSet(insert_pos, "\n�����ϱ�λ��,<Enter�˳�>: ");
#endif
		DRAGSET.ClearEntSet();
		acdbOpenObject(pPoly, polyId, AcDb::kForWrite);
		pPoly->erase();
		pPoly->close();
		//---^^^^---����һ�����ο���ʾ�û���λ���ϱ�Ĳ���λ��---^^^^---
		if (nRetCode == RTNORM)
		{	//δ�����Ϸ�ʱ���λ�ò��䶯
			f2dPoint table_pos;
			table_pos.x = insert_pos[X];
			table_pos.y = insert_pos[Y];
			pTable->SetBasePos(table_pos);
		}
	}
	else
	{
		if (nLocatePos == 0)		//���Ͻ�
			pTable->SetLocateStyle(TOP_LEFT);
		else if (nLocatePos == 1)//���Ͻ�
			pTable->SetLocateStyle(TOP_RIGHT);
		else if (nLocatePos == 2)//���½�
			pTable->SetLocateStyle(BOTTOM_LEFT);
		else if (nLocatePos == 3)//���½�
			pTable->SetLocateStyle(BOTTOM_RIGHT);
	}
	// ��ʾ�û���Ϣ
#ifdef _ARX_2007
	ads_prompt(L"\n���Ժ�......");
	acedCommand(RTSTR, L"undo", RTSTR, L"be", RTNONE);
#else
	ads_prompt("\n���Ժ�......");
	acedCommand(RTSTR, "undo", RTSTR, "be", RTNONE);
#endif
	//��д�����⼰����
	f3dPoint text_base;
	for (int iRow = 0; iRow < pTable->nRowsCount; iRow++)
	{
		for (int iCol = 0; iCol < pTable->nColsCount; iCol++)
		{
			XHGRID* pGrid = pTable->GetGridAt(iRow, iCol);
			if (pGrid->m_bHGridVirtual || pGrid->m_bVGridVirtual)//����Ԫ
				continue;
			f2dRect grid_rect;
			pTable->GetGridRect(iRow, iCol, grid_rect);
			double textH = pGrid->simfont.sfTextSize*scale;
			double angle = pGrid->simfont.sfTextAngle;
			double width = (angle == 0) ? grid_rect.Width() : grid_rect.Height();
			if (pGrid->grid_mode == PIC_BLOCK_GRID)	//���Ԫ�ڲ���ͼ��
			{
				f3dPoint insert_pos;
				insert_pos.x = (grid_rect.topLeft.x + grid_rect.bottomRight.x) / 2.0;
				insert_pos.y = (grid_rect.topLeft.y + grid_rect.bottomRight.y) / 2.0;
				InsertBlock(pBlockTableRecord, insert_pos, pGrid->data.sVal, 1.0);
				continue;
			}
			if (pGrid->xFontStyle.m_nTextAlignFlags == 0)
			{	//���������
				text_base.x = grid_rect.bottomRight.x - pGrid->xFontStyle.m_fRightDist*scale;
				text_base.y = (grid_rect.topLeft.y + grid_rect.bottomRight.y) / 2.0;
				DimMText(pBlockTableRecord, pGrid->data.sVal, text_base, width, textH,
					TextStyleTable::hzfs.textStyleId, AcDbMText::kMiddleRight, AcDbMText::kLtoR, angle);
			}
			else if (pGrid->xFontStyle.m_nTextAlignFlags == 1)
			{	//���������
				text_base.x = (grid_rect.topLeft.x + grid_rect.bottomRight.x) / 2.0;
				text_base.y = (grid_rect.topLeft.y + grid_rect.bottomRight.y) / 2.0;
				DimMText(pBlockTableRecord, pGrid->data.sVal, text_base, width, textH,
					TextStyleTable::hzfs.textStyleId, AcDbMText::kMiddleCenter, AcDbMText::kLtoR, angle);
			}
			else if (pGrid->xFontStyle.m_nTextAlignFlags == 4)
			{	//���������
				text_base.x = grid_rect.topLeft.x + pGrid->xFontStyle.m_fLeftDist*scale;
				text_base.y = (grid_rect.topLeft.y + grid_rect.bottomRight.y) / 2.0;
				DimMText(pBlockTableRecord, pGrid->data.sVal, text_base, width, textH,
					TextStyleTable::hzfs.textStyleId, AcDbMText::kMiddleLeft, AcDbMText::kLtoR, angle);
			}
			else if (pGrid->xFontStyle.m_nTextAlignFlags == 2)
			{	//���½Ƕ���
				text_base.x = grid_rect.bottomRight.x - pGrid->xFontStyle.m_fRightDist*scale;
				text_base.y = grid_rect.bottomRight.y + pGrid->xFontStyle.m_fBottomDist*scale;
				DimMText(pBlockTableRecord, pGrid->data.sVal, text_base, width, textH,
					TextStyleTable::hzfs.textStyleId, AcDbMText::kBottomRight, AcDbMText::kLtoR, angle);
			}
			else if (pGrid->xFontStyle.m_nTextAlignFlags == 3)
			{	//���ϽǶ���
				text_base.x = grid_rect.topLeft.x + pGrid->xFontStyle.m_fLeftDist*scale;
				text_base.y = grid_rect.topLeft.y - pGrid->xFontStyle.m_fTopDist*scale;
				DimMText(pBlockTableRecord, pGrid->data.sVal, text_base, width, textH,
					TextStyleTable::hzfs.textStyleId, AcDbMText::kTopLeft, AcDbMText::kLtoR, angle);
			}
		}
	}
	//�����б����(����)
	f3dPoint line_start, line_end;
	for (int i = 0; i <= pTable->nRowsCount; i++)
	{
		pTable->GetRowLineStart(i, line_start);
		pTable->GetRowLineEnd(i, line_end);
		line_start.z = line_end.z = 0;
		for (int j = 0; j < pTable->nColsCount; j++)
		{	//�������е�����Ԫ
			if (i == 0 || i == pTable->nRowsCount)	//��ĩ�в����ж��鵥Ԫ
				break;
			double wide;
			pTable->GetColWide(j, &wide);	//��õ�j�б���
			if (j == 0 && pTable->GetGridAt(i, j)->m_bVGridVirtual)
			{		//���о�Ϊ�鵥Ԫ
				line_start.x += wide;
				line_end.x = line_start.x;
			}
			else if (pTable->GetGridAt(i, j)->m_bVGridVirtual == TRUE)
			{	//��ǰ��Ϊ�鵥Ԫ
				if (!pTable->GetGridAt(i, j - 1)->m_bVGridVirtual)
				{
					CreateAcadLine(pBlockTableRecord, line_start, line_end);
					line_start.x = line_end.x + wide;
					line_end.x = line_start.x;
				}
				else
				{
					line_end.x += wide;
					line_start.x = line_end.x;
				}
			}
			else	//��ǰ�в����鵥Ԫ,�޸�����β����
			{
				if (j == 0)
					line_end.x = line_start.x + wide;
				else
					line_end.x += wide;
			}
		}
		CreateAcadLine(pBlockTableRecord, line_start, line_end);
	}
	// ���ɱ������(����)
	for (int j = 0; j <= pTable->nColsCount; j++)
	{
		pTable->GetColLineStart(j, line_start);
		pTable->GetColLineEnd(j, line_end);
		line_start.z = line_end.z = 0;
		for (int i = 0; i < pTable->nRowsCount; i++)
		{	//�������е�����Ԫ
			if (j == 0 || j == pTable->nColsCount)	//��ĩ�в����ж��鵥Ԫ
				break;
			double high;
			pTable->GetRowHigh(i, &high);
			if (i == 0 && pTable->GetGridAt(i, j)->m_bHGridVirtual)
			{		//���о�Ϊ�鵥Ԫ
				line_start.y -= high;
				line_end.y = line_start.y;
			}
			else if (pTable->GetGridAt(i, j)->m_bHGridVirtual)
			{		//��ǰ�����鵥Ԫ
				if (!pTable->GetGridAt(i - 1, j)->m_bHGridVirtual)
				{
					CreateAcadLine(pBlockTableRecord, line_start, line_end);
					line_start.y = line_end.y - high;
					line_end.y = line_start.y;
				}
				else
				{
					line_end.y -= high;
					line_start.y = line_end.y;
				}
			}
			else	//��ǰ�в����鵥Ԫ,�޸�����β����
			{
				if (i == 0)
					line_end.y = line_start.y - high;
				else
					line_end.y -= high;
			}
		}
		if (fabs(line_start.y - line_end.y) < eps)	//��ĩ���غ�
			continue;
		CreateAcadLine(pBlockTableRecord, line_start, line_end);
	}
	if(pTemRecord==NULL)
		pBlockTableRecord->close();
#ifdef _ARX_2007
	acedCommand(RTSTR, L"undo", RTSTR, L"e", RTNONE);//������ǰundo״̬
#else
	acedCommand(RTSTR, "undo", RTSTR, "e", RTNONE);//������ǰundo״̬
#endif
}
