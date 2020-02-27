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
	for(i=0;i<m_cutAngleSet.GetNodeNum()-1;i++)   //ð������
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
	{	//������ǰ���㽭ʢ��ı귨�����ò�̫���ʣ����ڸ�Ϊ����˳̩�ı귨���о����淶һЩ
		if(pJg->cut_wing[i]==1)		//ʼ��X֫����֫
			xWingCut.Printf("%.0f=%.0f",pJg->cut_wing_para[i][0],pJg->cut_wing_para[i][2]);
		else if(pJg->cut_wing[i]==2)	//ʼ��Y֫����֫
			xWingCut.Printf("%.0f<%.0f",pJg->cut_wing_para[i][1],pJg->cut_wing_para[i][0]);
		else						//ʼ��X֫���н�
			xWingCut.Printf("%.0fX%.0f",pJg->cut_angle[i*2][1],pJg->cut_angle[i*2][0]);
	}
	if((pJg->cut_angle[i*2+1][0]>0&&pJg->cut_angle[i*2+1][1]>0)||pJg->cut_wing[i]>0)
	{
		if(pJg->cut_wing[i]==1)		//ʼ��X֫����֫
			yWingCut.Printf("%.0f<%.0f",pJg->cut_wing_para[i][1],pJg->cut_wing_para[i][0]);
		else if(pJg->cut_wing[i]==2)	//ʼ��Y֫����֫
			yWingCut.Printf("%.0f=%.0f",pJg->cut_wing_para[i][0],pJg->cut_wing_para[i][2]);
		else						//ʼ��Y֫���н�
			yWingCut.Printf("%.0fX%.0f",pJg->cut_angle[i*2+1][1],pJg->cut_angle[i*2+1][0]);
	}
#else
	if(iCutAngleDimType==1)
	{	//�нǱ�ע��ʽ�� B:��ͷ�ߴ� L:֫�߳ߴ� C:��߳ߴ� 
		//BXL �н�  CXL ��֫ BXC �н�  �д��=�н�+��֫
		if((pJg->cut_angle[i*2][0]>0&&pJg->cut_angle[i*2][1]>0)||pJg->cut_wing[i]>0)
		{
			if(pJg->cut_wing[i]==1)		//ʼ��X֫����֫
			{
				xWingCut.Printf("%.0f=%.0f",pJg->cut_wing_para[i][0],pJg->cut_wing_para[i][2]);
				if(pJg->cut_angle[i*2][0]>pJg->cut_wing_para[i][2])
					xWingCut.Append(CXhChar50(",%.0fX%.0f",pJg->cut_angle[i*2][1],pJg->cut_angle[i*2][0]));
			}
			else if(pJg->cut_wing[i]==2)	//ʼ��Y֫����֫
			{
				xWingCut.Printf("%.0f<%.0f",pJg->cut_wing_para[i][1],pJg->cut_wing_para[i][0]);
				if(pJg->cut_angle[i*2][0]>0&&pJg->cut_angle[i*2][1]>0)
					xWingCut.Append(CXhChar50(",%.0fX%.0f",pJg->cut_angle[i*2][1],pJg->cut_angle[i*2][0]));
			}
			else						//ʼ��X֫���н�
				xWingCut.Printf("%.0fX%.0f",pJg->cut_angle[i*2][1],pJg->cut_angle[i*2][0]);
		}
		if((pJg->cut_angle[i*2+1][0]>0&&pJg->cut_angle[i*2+1][1]>0)||pJg->cut_wing[i]>0)
		{
			if(pJg->cut_wing[i]==1)		//ʼ��X֫����֫
			{
				yWingCut.Printf("%.0f<%.0f",pJg->cut_wing_para[i][1],pJg->cut_wing_para[i][0]);
				if(pJg->cut_angle[i*2+1][0]>0&&pJg->cut_angle[i*2+1][1]>0)
					yWingCut.Append(CXhChar50(",%.0fX%.0f",pJg->cut_angle[i*2+1][1],pJg->cut_angle[i*2+1][0]));
			}
			else if(pJg->cut_wing[i]==2)	//ʼ��Y֫����֫
			{
				yWingCut.Printf("%.0f=%.0f",pJg->cut_wing_para[i][0],pJg->cut_wing_para[i][2]);
				if(pJg->cut_angle[i*2+1][0]>pJg->cut_wing_para[i][2])
					yWingCut.Append(CXhChar50(",%.0fX%.0f",pJg->cut_angle[i*2+1][1],pJg->cut_angle[i*2+1][0]));
			}
			else						//ʼ��Y֫���н�
				yWingCut.Printf("%.0fX%.0f",pJg->cut_angle[i*2+1][1],pJg->cut_angle[i*2+1][0]);
		}
	}
	else
	{
		int alfa = 0;
		if(pJg->cut_wing[i]>0)
		{
			CXhChar16 alfa_str;
			if(pJg->cut_wing[i]==1)		//ʼ��X֫����֫
			{
				alfa = ftoi(DEGTORAD_COEF*atan((pJg->cut_wing_para[i][2]-pJg->cut_wing_para[i][0])/pJg->GetWidth()));
				if(pJg->cut_wing_para[i][1]==0)	//X֫����֫
					xWingCut.Printf("CF%.0f",pJg->cut_wing_para[i][0]);
				else	//����֫	
					xWingCut.Printf("BC%.0fX%.0f",pJg->cut_wing_para[i][0],pJg->cut_wing_para[i][1]);
				if(gxDrwLocalizer.ciCustomerSerial==12)
				{	//��ΰ��Ӧ����������֫ʱ�Ƕȱ�עֻ��45�ȡ�60������ wht 18-11-06
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
			else if(pJg->cut_wing[i]==2)	//ʼ��Y֫����֫
			{
				alfa = ftoi(DEGTORAD_COEF*atan((pJg->cut_wing_para[i][2]-pJg->cut_wing_para[i][0])/pJg->GetWidth()));
				if(pJg->cut_wing_para[i][1]==0)	//X֫����֫
					yWingCut.Printf("CF%.0f",pJg->cut_wing_para[i][0]);
				else	//����֫
					yWingCut.Printf("BC%.0fX%.0f",pJg->cut_wing_para[i][0],pJg->cut_wing_para[i][1]);
				if(gxDrwLocalizer.ciCustomerSerial==12)
				{	//��ΰ��Ӧ����������֫ʱ�Ƕȱ�עֻ��45�ȡ�60������ wht 18-11-06
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
		{	//ʼ��X֫���н�
			if(pJg->cut_angle[i*2][0]==pJg->cut_angle[i*2][1]&&pJg->cut_angle[i*2][1]==(pJg->GetWidth()-pJg->GetThick()))
				xWingCut.Printf("CO45%%%%D");	//�нǳ�������ҵ���(֫��-֫��)ʱ��עδCO45�� wht 18-11-09
			else
				xWingCut.Printf("CT%.0fX%.0f",pJg->cut_angle[i*2][0],pJg->cut_angle[i*2][1]);
		}
		if(pJg->cut_angle[i*2+1][0]>0&&pJg->cut_angle[i*2+1][1]>0&&pJg->cut_wing[i]!=2)
		{	//ʼ��Y֫���н�
			if(pJg->cut_angle[i*2+1][0]==pJg->cut_angle[i*2+1][1]&&pJg->cut_angle[i*2+1][1]==(pJg->GetWidth()-pJg->GetThick()))
				yWingCut.Printf("CO45%%%%D");	//�нǳ�������ҵ���(֫��-֫��)ʱ��עδCO45�� wht 18-11-09
			else
				yWingCut.Printf("CT%.0fX%.0f",pJg->cut_angle[i*2+1][0],pJg->cut_angle[i*2+1][1]);
		}
	}
#endif
}

void CCutAngleBriefMap::PrepareCutAngleTbl()
{	//1.��ҳ
	m_pageList.Empty();
	MAP_PAGE *pCurPage=NULL;
	int nCount=0,nCountPrePage=m_cutAngleSet.GetNodeNum();
	if(gxDrwLocalizer.ciCustomerSerial==11)
	{
		nCountPrePage=7;	//�����۵������ȹ̶�Ϊ7����¼һҳ
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
		{	//��ǰҳ����
			nCount=0;
			pCurPage=NULL;
		}
	}
	//2.׼�����
	for(MAP_PAGE *pPage=m_pageList.GetFirst();pPage;pPage=m_pageList.GetNext())
	{
		if(gxDrwLocalizer.ciCustomerSerial==11)
			PrepareCutAngleTbl2(pPage->xCutAngleTbl,pPage->cutAngleSet);	//�����۵���
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
	//1.���ñ�����
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
	xCutAngleTbl.SetGrid(0,0,CXhChar50("Project Name��%s",Ta.m_sPrjName));
	xCutAngleTbl.SetGrid(1,0,CXhChar50("Tower Type Name��%s",Ta.m_sTaType));
	xCutAngleTbl.SetGrid(1,2,"Cut Angle Detailed List");
	xCutAngleTbl.SetGrid(0,3,CXhChar50("File No.��"));
	xCutAngleTbl.SetGrid(1,3,CXhChar50("DateTime��%s",t.Format("%y.%m.%d")));
	//
	xCutAngleTbl.SetGrid(2,0,"Part No.");
	xCutAngleTbl.SetGrid(2,1,"Spec");
	xCutAngleTbl.SetGrid(2,2,"Cut angle shape");
	xCutAngleTbl.SetGrid(2,4,"Notes");
#else
	sG.Copy("�ľ�");
	xCutAngleTbl.SetGrid(0,0,CXhChar50("��������%s",Ta.m_sPrjName));
	xCutAngleTbl.SetGrid(1,0,CXhChar50("��  �ͣ�%s",Ta.m_sTaType));
	xCutAngleTbl.SetGrid(0,2,"�н���֫�굥");
	xCutAngleTbl.SetGrid(0,3,CXhChar50("�ļ���ţ�"));
	xCutAngleTbl.SetGrid(0,4,"CutAngleTblLog");
	xCutAngleTbl.GetGridAt(0,4)->grid_mode=PIC_BLOCK_GRID;
	xCutAngleTbl.SetGrid(1,3,CXhChar50("���ڣ�%s",t.Format("%y.%m.%d")));
	//
	xCutAngleTbl.SetGrid(2,0,"������");
	xCutAngleTbl.SetGrid(2,1,"��  ��");
	xCutAngleTbl.SetGrid(2,2,"�н���֫ʾ��ͼ������ֵ");
	xCutAngleTbl.SetGrid(2,4,"��  ע");
#endif
	//2.���������
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
	xCutAngleTbl.SetGrid(0,0,"������");
	xCutAngleTbl.SetGrid(0,1,"��  ��");
	xCutAngleTbl.SetGrid(0,2,"�н���״");
	xCutAngleTbl.SetGrid(0,3,"�н���֫����");
	xCutAngleTbl.SetGrid(0,5,"��  ע");
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
			spec.Printf("�ľ�%.0f",pJg->GetLsG(pLsRef));
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
{	//1.׼��������ݲ�����
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
		{	//��ҳʱ����ƾ��α߿򣬱���������ӡ
			f2dRect rect;
			pPage->xCutAngleTbl.GetTblRect(&rect);
			rect.topLeft.x-=1.5;
			rect.topLeft.y+=5;
			CreateAcadRect(pBlockTableRecord,f3dPoint(rect.topLeft.x,rect.topLeft.y,0),210,297);
		}
		pBlockTableRecord->close();
		//2.���ƽǸּ�ͼ
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