#include "stdafx.h"
#include "CutAngleBriefMap.h"
#include "ComparePartNoString.h"
#include "FormatTable.h"
#include "DrawDim.h"
#include ".\LdsDrwLocale.h"

//from DrawTable.cpp
extern CTower Ta;
BOOL DrawJgMap(CRodDrawing *pJgDraw,f3dPoint origin,BOOL bDimStyle2=FALSE,AcDbBlockTableRecord *pExterBlockTableRecord=NULL);

//////////////////////////////////////////////////////////////////////////
// CCutAngleBriefMap
bool CCutAngleBriefMap::Init(JGSET &cutanglejgset,BOOL bPromptPos)
{
	if(cutanglejgset.GetNodeNum()<=0)
		return false;	
	m_bPromptPos=bPromptPos;
	m_cutAngleSet.Empty();
	for(CLDSLineAngle *pAngle=cutanglejgset.GetFirst();pAngle;pAngle=cutanglejgset.GetNext())
		m_cutAngleSet.append(pAngle);
	int i=0,j=0;
	for(i=0;i<m_cutAngleSet.GetNodeNum()-1;i++)   //冒泡排序
	{
		BOOL bSorted = TRUE;
		for(j=0;j<m_cutAngleSet.GetNodeNum()-i-1;j++)
		{
			//if(stricmp(cutanglejgset[j]->GetPartNo(),cutanglejgset[j+1]->GetPartNo())>0)
			if(ComparePartNoString(m_cutAngleSet[j]->GetPartNo(),m_cutAngleSet[j+1]->GetPartNo())>0)
			{
				CLDSLineAngle* pJg=m_cutAngleSet[j];
				m_cutAngleSet[j] = m_cutAngleSet[j+1];
				m_cutAngleSet[j+1]=pJg;
				bSorted = FALSE;
			}
		}
		if(bSorted)
			break;
	}
	return true;
}
void CCutAngleBriefMap::GetAngleCutAngleInfo(CLDSLineAngle *pJg,CXhChar50 &xWingCut,CXhChar50 &yWingCut,bool bStart,int iCutAngleDimType)
{
	xWingCut.Empty();
	yWingCut.Empty();
	int i=bStart?0:1;
#ifdef ZHEJIANGSHENGDA
	if((pJg->cut_angle[i*2][0]>0&&pJg->cut_angle[i*2][1]>0)||pJg->cut_wing[i]>0)
	{	//以下以前是浙江盛达的标法，觉得不太合适，现在改为重庆顺泰的标法，感觉更规范一些
		if(pJg->cut_wing[i]==1)		//始端X肢有切肢
			xWingCut.Printf("%.0f=%.0f",pJg->cut_wing_para[i][0],pJg->cut_wing_para[i][2]);
		else if(pJg->cut_wing[i]==2)	//始端Y肢有切肢
			xWingCut.Printf("%.0f<%.0f",pJg->cut_wing_para[i][1],pJg->cut_wing_para[i][0]);
		else						//始端X肢有切角
			xWingCut.Printf("%.0fX%.0f",pJg->cut_angle[i*2][1],pJg->cut_angle[i*2][0]);
	}
	if((pJg->cut_angle[i*2+1][0]>0&&pJg->cut_angle[i*2+1][1]>0)||pJg->cut_wing[i]>0)
	{
		if(pJg->cut_wing[i]==1)		//始端X肢有切肢
			yWingCut.Printf("%.0f<%.0f",pJg->cut_wing_para[i][1],pJg->cut_wing_para[i][0]);
		else if(pJg->cut_wing[i]==2)	//始端Y肢有切肢
			yWingCut.Printf("%.0f=%.0f",pJg->cut_wing_para[i][0],pJg->cut_wing_para[i][2]);
		else						//始端Y肢有切角
			yWingCut.Printf("%.0fX%.0f",pJg->cut_angle[i*2+1][1],pJg->cut_angle[i*2+1][0]);
	}
#else
	if(iCutAngleDimType==1)
	{	//切角标注样式二 B:端头尺寸 L:肢边尺寸 C:筋边尺寸 
		//BXL 切角  CXL 切肢 BXC 切筋  切大角=切筋+切肢
		if((pJg->cut_angle[i*2][0]>0&&pJg->cut_angle[i*2][1]>0)||pJg->cut_wing[i]>0)
		{
			if(pJg->cut_wing[i]==1)		//始端X肢有切肢
			{
				xWingCut.Printf("%.0f=%.0f",pJg->cut_wing_para[i][0],pJg->cut_wing_para[i][2]);
				if(pJg->cut_angle[i*2][0]>pJg->cut_wing_para[i][2])
					xWingCut.Append(CXhChar50(",%.0fX%.0f",pJg->cut_angle[i*2][1],pJg->cut_angle[i*2][0]));
			}
			else if(pJg->cut_wing[i]==2)	//始端Y肢有切肢
			{
				xWingCut.Printf("%.0f<%.0f",pJg->cut_wing_para[i][1],pJg->cut_wing_para[i][0]);
				if(pJg->cut_angle[i*2][0]>0&&pJg->cut_angle[i*2][1]>0)
					xWingCut.Append(CXhChar50(",%.0fX%.0f",pJg->cut_angle[i*2][1],pJg->cut_angle[i*2][0]));
			}
			else						//始端X肢有切角
				xWingCut.Printf("%.0fX%.0f",pJg->cut_angle[i*2][1],pJg->cut_angle[i*2][0]);
		}
		if((pJg->cut_angle[i*2+1][0]>0&&pJg->cut_angle[i*2+1][1]>0)||pJg->cut_wing[i]>0)
		{
			if(pJg->cut_wing[i]==1)		//始端X肢有切肢
			{
				yWingCut.Printf("%.0f<%.0f",pJg->cut_wing_para[i][1],pJg->cut_wing_para[i][0]);
				if(pJg->cut_angle[i*2+1][0]>0&&pJg->cut_angle[i*2+1][1]>0)
					yWingCut.Append(CXhChar50(",%.0fX%.0f",pJg->cut_angle[i*2+1][1],pJg->cut_angle[i*2+1][0]));
			}
			else if(pJg->cut_wing[i]==2)	//始端Y肢有切肢
			{
				yWingCut.Printf("%.0f=%.0f",pJg->cut_wing_para[i][0],pJg->cut_wing_para[i][2]);
				if(pJg->cut_angle[i*2+1][0]>pJg->cut_wing_para[i][2])
					yWingCut.Append(CXhChar50(",%.0fX%.0f",pJg->cut_angle[i*2+1][1],pJg->cut_angle[i*2+1][0]));
			}
			else						//始端Y肢有切角
				yWingCut.Printf("%.0fX%.0f",pJg->cut_angle[i*2+1][1],pJg->cut_angle[i*2+1][0]);
		}
	}
	else
	{
		int alfa = 0;
		if(pJg->cut_wing[i]>0)
		{
			CXhChar16 alfa_str;
			if(pJg->cut_wing[i]==1)		//始端X肢有切肢
			{
				alfa = ftoi(DEGTORAD_COEF*atan((pJg->cut_wing_para[i][2]-pJg->cut_wing_para[i][0])/pJg->GetWidth()));
				if(pJg->cut_wing_para[i][1]==0)	//X肢切整肢
					xWingCut.Printf("CF%.0f",pJg->cut_wing_para[i][0]);
				else	//背切肢	
					xWingCut.Printf("BC%.0fX%.0f",pJg->cut_wing_para[i][0],pJg->cut_wing_para[i][1]);
				if(gxDrwLocalizer.ciCustomerSerial==12)
				{	//刘伟反应宝鸡塔厂切肢时角度标注只有45度、60度两种 wht 18-11-06
					if(alfa<20)
						alfa=0;
					else if(alfa>=20&&alfa<=50)
						alfa=45;
					else if(alfa>50)
						alfa=60;
				}
				if(alfa!=0)
				{
					alfa_str.Printf("+CO%d%%%%D",alfa);
					xWingCut.Append(alfa_str);
				}
			}
			else if(pJg->cut_wing[i]==2)	//始端Y肢有切肢
			{
				alfa = ftoi(DEGTORAD_COEF*atan((pJg->cut_wing_para[i][2]-pJg->cut_wing_para[i][0])/pJg->GetWidth()));
				if(pJg->cut_wing_para[i][1]==0)	//X肢切整肢
					yWingCut.Printf("CF%.0f",pJg->cut_wing_para[i][0]);
				else	//背切肢
					yWingCut.Printf("BC%.0fX%.0f",pJg->cut_wing_para[i][0],pJg->cut_wing_para[i][1]);
				if(gxDrwLocalizer.ciCustomerSerial==12)
				{	//刘伟反应宝鸡塔厂切肢时角度标注只有45度、60度两种 wht 18-11-06
					if(alfa<20)
						alfa=0;
					else if(alfa>=20&&alfa<=50)
						alfa=45;
					else if(alfa>50)
						alfa=60;
				}
				if(alfa!=0)
				{
					alfa_str.Printf("+CO%d%%%%D",alfa);
					yWingCut.Append(alfa_str);
				}
			}
		}
		if(pJg->cut_angle[i*2][0]>0&&pJg->cut_angle[i*2][1]>0&&pJg->cut_wing[i]!=1)
		{	//始端X肢有切角
			if(pJg->cut_angle[i*2][0]==pJg->cut_angle[i*2][1]&&pJg->cut_angle[i*2][1]==(pJg->GetWidth()-pJg->GetThick()))
				xWingCut.Printf("CO45%%%%D");	//切角长宽相等且等于(肢宽-肢厚)时标注未CO45度 wht 18-11-09
			else
				xWingCut.Printf("CT%.0fX%.0f",pJg->cut_angle[i*2][0],pJg->cut_angle[i*2][1]);
		}
		if(pJg->cut_angle[i*2+1][0]>0&&pJg->cut_angle[i*2+1][1]>0&&pJg->cut_wing[i]!=2)
		{	//始端Y肢有切角
			if(pJg->cut_angle[i*2+1][0]==pJg->cut_angle[i*2+1][1]&&pJg->cut_angle[i*2+1][1]==(pJg->GetWidth()-pJg->GetThick()))
				yWingCut.Printf("CO45%%%%D");	//切角长宽相等且等于(肢宽-肢厚)时标注未CO45度 wht 18-11-09
			else
				yWingCut.Printf("CT%.0fX%.0f",pJg->cut_angle[i*2+1][0],pJg->cut_angle[i*2+1][1]);
		}
	}
#endif
}

void CCutAngleBriefMap::PrepareCutAngleTbl()
{	//1.分页
	m_pageList.Empty();
	MAP_PAGE *pCurPage=NULL;
	int nCount=0,nCountPrePage=m_cutAngleSet.GetNodeNum();
	if(gxDrwLocalizer.ciCustomerSerial==11)
	{
		nCountPrePage=7;	//长春聚德龙，先固定为7条记录一页
		m_nRowspann=1;
		m_iBriefMapCol=2;
		m_iStartRow=3;
		m_textHeight=5.5;
		m_bDrawFrame=TRUE;
	}
	for(int i=0;i<m_cutAngleSet.GetNodeNum();i++)
	{
		if(pCurPage==NULL)
			pCurPage=m_pageList.append();
		pCurPage->cutAngleSet.append(m_cutAngleSet[i]);
		nCount++;
		if(nCount==nCountPrePage)
		{	//当前页已满
			nCount=0;
			pCurPage=NULL;
		}
	}
	//2.准备表格
	for(MAP_PAGE *pPage=m_pageList.GetFirst();pPage;pPage=m_pageList.GetNext())
	{
		if(gxDrwLocalizer.ciCustomerSerial==11)
			PrepareCutAngleTbl2(pPage->xCutAngleTbl,pPage->cutAngleSet);	//长春聚德龙
		else
			PrepareCutAngleTbl1(pPage->xCutAngleTbl,pPage->cutAngleSet);
	}
}

void CCutAngleBriefMap::PrepareCutAngleTbl2(CFormatTbl &xCutAngleTbl,JGSET &jgSet)
{
	int i=0,j=0;
	int nRowCount=jgSet.GetNodeNum()+3,nColCount=5;
	xCutAngleTbl.Create(nRowCount,nColCount);
	double colArr[5]={25.5,25.5,82.5,51,22.5};
	for(i=0;i<xCutAngleTbl.GetColumnsCount();i++)
		xCutAngleTbl.SetColumnWide(i,colArr[i]);
	//1.设置标题行
	xCutAngleTbl.SetRowHigh(0,10.5);
	xCutAngleTbl.SetRowHigh(1,10.5);
	xCutAngleTbl.SetRowHigh(2,17.5);
	xCutAngleTbl.GetGridAt(0,1)->m_bHGridVirtual=TRUE;
	xCutAngleTbl.GetGridAt(1,1)->m_bHGridVirtual=TRUE;
	xCutAngleTbl.GetGridAt(1,2)->m_bVGridVirtual=TRUE;
	xCutAngleTbl.GetGridAt(1,4)->m_bVGridVirtual=TRUE;
	for(i=0;i<xCutAngleTbl.GetRowsCount();i++)
	{
		for(j=0;j<xCutAngleTbl.GetColumnsCount();j++)
		{
			xCutAngleTbl.GetGridAt(i,j)->feature = i<2?TEXTALIGN_LEFT_CENTER:TEXTALIGN_MIDDLE_CENTER;
			xCutAngleTbl.GetGridAt(i,j)->textsize=m_textHeight;
		}
		if(i>1)
			xCutAngleTbl.GetGridAt(i,3)->m_bHGridVirtual = TRUE;
		if(i>2)
			xCutAngleTbl.SetRowHigh(i,35);
	}
	xCutAngleTbl.GetGridAt(0,2)->textsize=9;
	xCutAngleTbl.GetGridAt(0,2)->feature=TEXTALIGN_MIDDLE_CENTER;
	CXhChar16 sG;
	CTime t = CTime::GetCurrentTime();
#ifdef AFX_TARG_ENU_ENGLISH
	sG.Copy(centre distance);
	xCutAngleTbl.SetGrid(0,0,CXhChar50("Project Name：%s",Ta.m_sPrjName));
	xCutAngleTbl.SetGrid(1,0,CXhChar50("Tower Type Name：%s",Ta.m_sTaType));
	xCutAngleTbl.SetGrid(1,2,"Cut Angle Detailed List");
	xCutAngleTbl.SetGrid(0,3,CXhChar50("File No.："));
	xCutAngleTbl.SetGrid(1,3,CXhChar50("DateTime：%s",t.Format("%y.%m.%d")));
	//
	xCutAngleTbl.SetGrid(2,0,"Part No.");
	xCutAngleTbl.SetGrid(2,1,"Spec");
	xCutAngleTbl.SetGrid(2,2,"Cut angle shape");
	xCutAngleTbl.SetGrid(2,4,"Notes");
#else
	sG.Copy("心距");
	xCutAngleTbl.SetGrid(0,0,CXhChar50("工程名：%s",Ta.m_sPrjName));
	xCutAngleTbl.SetGrid(1,0,CXhChar50("塔  型：%s",Ta.m_sTaType));
	xCutAngleTbl.SetGrid(0,2,"切角切肢详单");
	xCutAngleTbl.SetGrid(0,3,CXhChar50("文件编号："));
	xCutAngleTbl.SetGrid(0,4,"CutAngleTblLog");
	xCutAngleTbl.GetGridAt(0,4)->grid_mode=PIC_BLOCK_GRID;
	xCutAngleTbl.SetGrid(1,3,CXhChar50("日期：%s",t.Format("%y.%m.%d")));
	//
	xCutAngleTbl.SetGrid(2,0,"零件编号");
	xCutAngleTbl.SetGrid(2,1,"规  格");
	xCutAngleTbl.SetGrid(2,2,"切角切肢示意图及参数值");
	xCutAngleTbl.SetGrid(2,4,"备  注");
#endif
	//2.填充表格数据
	for(i=0;i<jgSet.GetNodeNum();i++)
	{
		CLDSLineAngle *pJg=jgSet[i];
		xCutAngleTbl.SetGrid(i+m_iStartRow,0,GetPartNoIncMat(pJg->GetPartNo(),pJg->cMaterial));
		CXhChar50 spec;
		spec.Printf("L%.0fX%.0f",pJg->GetWidth(),pJg->GetThick());
		xCutAngleTbl.SetGrid(i+m_iStartRow,1,spec);
		CLsRef *pLsRef=pJg->GetFirstLsRef();
		if(pLsRef)
			spec.Printf("%s%.0f",(char*)sG,pJg->GetLsG(pLsRef));
		xCutAngleTbl.SetGrid(i+m_iStartRow,4,spec);
	}
}

void CCutAngleBriefMap::PrepareCutAngleTbl1(CFormatTbl &xCutAngleTbl,JGSET &jgSet)
{
	int i=0,j=0;
	xCutAngleTbl.Create(jgSet.GetNodeNum()*2+1,6);
	for(i=0;i<xCutAngleTbl.GetRowsCount();i++)
	{
		xCutAngleTbl.SetRowHigh(i,10);
		for(j=0;j<xCutAngleTbl.GetColumnsCount();j++)
			xCutAngleTbl.GetGridAt(i,j)->feature = TEXTALIGN_MIDDLE_CENTER;
		if(i>0 && i%2==0)
		{
			xCutAngleTbl.GetGridAt(i,0)->m_bVGridVirtual = TRUE;
			xCutAngleTbl.GetGridAt(i,1)->m_bVGridVirtual = TRUE;
			xCutAngleTbl.GetGridAt(i,2)->m_bVGridVirtual = TRUE;
			xCutAngleTbl.GetGridAt(i,5)->m_bVGridVirtual = TRUE;
		}
	}
	xCutAngleTbl.GetGridAt(0,4)->m_bHGridVirtual = TRUE;
	xCutAngleTbl.SetColumnWide(0,25);
	xCutAngleTbl.SetColumnWide(1,20);
	xCutAngleTbl.SetColumnWide(2,55);
	xCutAngleTbl.SetColumnWide(3,25);
	xCutAngleTbl.SetColumnWide(4,25);
	xCutAngleTbl.SetColumnWide(5,20);
#ifdef AFX_TARG_ENU_ENGLISH
	xCutAngleTbl.SetGrid(0,0,"Part No.");
	xCutAngleTbl.SetGrid(0,1,"Spec");
	xCutAngleTbl.SetGrid(0,2,"Cut angle shape");
	xCutAngleTbl.SetGrid(0,3,"Angle cut para");
	xCutAngleTbl.SetGrid(0,5,"Notes");
#else
	xCutAngleTbl.SetGrid(0,0,"零件编号");
	xCutAngleTbl.SetGrid(0,1,"规  格");
	xCutAngleTbl.SetGrid(0,2,"切角形状");
	xCutAngleTbl.SetGrid(0,3,"切角切肢参数");
	xCutAngleTbl.SetGrid(0,5,"备  注");
#endif
	for(i=0;i<jgSet.GetNodeNum();i++)
	{
		CLDSLineAngle *pJg=jgSet[i];
		xCutAngleTbl.SetGrid(i*2+1,0,GetPartNoIncMat(pJg->GetPartNo(),pJg->cMaterial));
		CXhChar50 spec;
		spec.Printf("L%.0fX%.0f",pJg->GetWidth(),pJg->GetThick());
		xCutAngleTbl.SetGrid(i*2+1,1,spec);
		CLsRef *pLsRef=pJg->GetFirstLsRef();
		if(pLsRef)
		{
	#ifdef AFX_TARG_ENU_ENGLISH
			spec.Printf("centre distance %.0f",pJg->GetLsG(pLsRef));
	#else
			spec.Printf("心距%.0f",pJg->GetLsG(pLsRef));
	#endif
		}
		xCutAngleTbl.SetGrid(i*2+1,5,spec);
		CXhChar50 xWingCut,yWingCut;
		GetAngleCutAngleInfo(pJg,xWingCut,yWingCut,true,sys.part_map.angle.iCutAngleDimType);
		xCutAngleTbl.SetGrid(i*2+2,3,xWingCut);
		xCutAngleTbl.SetGrid(i*2+1,3,yWingCut);
		GetAngleCutAngleInfo(pJg,xWingCut,yWingCut,false,sys.part_map.angle.iCutAngleDimType);
		xCutAngleTbl.SetGrid(i*2+2,4,xWingCut);
		xCutAngleTbl.SetGrid(i*2+1,4,yWingCut);
	}
}
void CCutAngleBriefMap::Draw()
{	//1.准备表格数据并绘制
	PrepareCutAngleTbl();
	int iPageIndex=0;
	f2dPoint initPos;
	int nPaperWidth=210,nPaperHeight=297,TABLE_SPACE=10;
	for(MAP_PAGE *pPage=m_pageList.GetFirst();pPage;pPage=m_pageList.GetNext())
	{
		AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
		if(iPageIndex>0)
		{
			f2dPoint basePos(initPos.x+nPaperWidth+10,initPos.y);
			pPage->xCutAngleTbl.SetBasePos(basePos);
			initPos=basePos;
		}
		DrawTable(pBlockTableRecord,&pPage->xCutAngleTbl,iPageIndex==0?m_bPromptPos:FALSE,1,0);
		if(iPageIndex==0)
			pPage->xCutAngleTbl.GetBasePos(initPos);
		iPageIndex++;
		if(m_bDrawFrame)
		{	//分页时需绘制矩形边框，便于批量打印
			f2dRect rect;
			pPage->xCutAngleTbl.GetTblRect(&rect);
			rect.topLeft.x-=1.5;
			rect.topLeft.y+=5;
			CreateAcadRect(pBlockTableRecord,f3dPoint(rect.topLeft.x,rect.topLeft.y,0),210,297);
		}
		pBlockTableRecord->close();
		//2.绘制角钢简图
		for(int i=0;i<pPage->cutAngleSet.GetNodeNum();i++)
		{
			CLDSLineAngle *pJg=pPage->cutAngleSet[i];
			CLineAngleDrawing xBriefJgDraw;
			f2dRect draw_rect;
			pPage->xCutAngleTbl.GetGridRect(i*m_nRowspann+m_iStartRow,m_iBriefMapCol,draw_rect);
			xBriefJgDraw.dimStyleFlag.Clear();
			xBriefJgDraw.dimStyleFlag.SetFlagDword(CLineAngleDrawing::BriefDrawing);
			xBriefJgDraw.dimStyleFlag.SetFlagDword(CLineAngleDrawing::OnlyDrawEndBolt);
			xBriefJgDraw.dimStyleFlag.SetFlagDword(CLineAngleDrawing::CutAngleDimByText);
			xBriefJgDraw.CreateLineAngleDrawing(pJg,draw_rect.Width(),draw_rect.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);
			if(draw_rect.Width()>EPS2&&draw_rect.Height()>EPS2)
			{
				f3dPoint origin;
				origin.x = draw_rect.topLeft.x;
				origin.y = (draw_rect.topLeft.y+draw_rect.bottomRight.y)/2;
				DrawJgMap(&xBriefJgDraw,origin,TRUE);
			}
		}
	}
}
CCutAngleBriefMap::CCutAngleBriefMap(JGSET &cutAngleSet,BOOL bPropmtPos)
{
	m_bDrawFrame=FALSE;
	m_iBriefMapCol=2;
	m_nRowspann=2;
	m_iStartRow=1;
	m_textHeight=3;
	Init(cutAngleSet,bPropmtPos);
}

void CCutAngleBriefMap::Draw(JGSET &cutAngleSet,BOOL bPropmtPos)
{
	CCutAngleBriefMap map(cutAngleSet,bPropmtPos);
	map.Draw();
}