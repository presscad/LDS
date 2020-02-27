//<LOCALE_TRANSLATE/>
// HuoQuJgDrawing.cpp: implementation of the CPolyAngleDrawing class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "SysPara.h"
#include "SortFunc.h"
#include "JgDrawing.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifndef __TSA_FILE_
//判断角钢火曲类型 冷弯/热弯  0---未找到火曲类型,1---冷弯 2---热弯
static int ConfirmLineAngleHuoquType(int wing_wide,double fHuoquAngle,BOOL bXYBoth)
{
	BOOL bOutcurve=TRUE;	//外曲
	if(fHuoquAngle>0)		//火曲角度大于零表示内曲 小于零表示外曲
		bOutcurve=FALSE;
	fHuoquAngle=fabs(fHuoquAngle);
	if(bXYBoth)
	{	//两肢同时弯曲
		if(bOutcurve)
		{	//外曲
			if((fHuoquAngle-6.000001)<0)		//fHuoquAngle<=6
			{
				if(wing_wide>=30&&wing_wide<=140)
					return 1;//冷弯
				if(wing_wide>=160&&wing_wide<=200)
					return 2;//热弯
			}
			else if((fHuoquAngle-45.000001)>0)	//fHuoquAngle>45
			{
				if(wing_wide>=30&&wing_wide<=200)
					return 2;	//热弯
			}
			else	//6<fHuoquAngle<=45
			{
				if(wing_wide>=30&&wing_wide<=200)
					return 2;	//热弯
			}
		}
		else
		{	//内曲
			if((fHuoquAngle-5.000001)<0)		//fHuoquAngle<=5
			{
				if(wing_wide>=30&&wing_wide<=100)
					return 1;//冷弯
				if(wing_wide>=110&&wing_wide<=200)
					return 2;//热弯
			}
			else if((fHuoquAngle-23.000001)>0)	//fHuoquAngle>23
			{
				if(wing_wide>=30&&wing_wide<=200)
					return 2;	//热弯
			}
			else	//5<fHuoquAngle<=23
			{
				if(wing_wide>=30&&wing_wide<=200)
					return 2;	//热弯
			}
		}
	}
	else
	{	//单肢同时弯曲
		if(bOutcurve)
		{	//外曲
			if((fHuoquAngle-7.000001)<0)		//fHuoquAngle<=7
			{
				if(wing_wide>=30&&wing_wide<=140)
					return 1;//冷弯
				if(wing_wide>=160&&wing_wide<=200)
					return 2;//热弯
			}
			else	//fHuoquAngle>7
			{
				if(wing_wide>=30&&wing_wide<=200)
					return 2;	//热弯
			}
		}
		else
		{	//内曲
			if((fHuoquAngle-10.000001)<0)		//fHuoquAngle<=10
			{
				if(wing_wide>=30&&wing_wide<=80)
					return 1;//冷弯
				if(wing_wide>=90&&wing_wide<=200)
					return 2;//热弯
			}
			else	//10<fHuoquAngle
			{
				if(wing_wide>=30&&wing_wide<=200)
					return 2;	//热弯
			}
		}
	}
	return 0;
}

//内包角钢铲背半径
static double GetInsideJgCutBerR(CLDSLineAngle *pMainLineAngle)
{
	if(pMainLineAngle==NULL)
		return 0;
	int wing_wide=(int)pMainLineAngle->GetWidth();
	if(wing_wide==45)
		return 5;
	else if(wing_wide==50)
		return 5.5;
	else if(wing_wide==56)
		return 6;
	else if(wing_wide==63)
		return 7;
	else if(wing_wide==70)
		return 8;
	else if(wing_wide==75||wing_wide==80)
		return 9;
	else if(wing_wide==90)
		return 10;
	else if(wing_wide==100||wing_wide==110)
		return 12;
	else if(wing_wide==125||wing_wide==140)
		return 14;
	else if(wing_wide==160||wing_wide==180)
		return 16;
	else if(wing_wide==200)
		return 18;
	else if(wing_wide==250)
		return 26;
	else 
		return 0;
}

//将源角钢信息复制到目标角钢中
static void CopyLineAngle(CLDSLineAngle *pDestLineAngle,CLDSLineAngle *pSrcLineAngle,BOOL bReverseStartEnd)
{
	if(pSrcLineAngle==NULL||pDestLineAngle==NULL)
		return;
	int i=0,j=0;
	pDestLineAngle->EmptyLsRef();
	if(bReverseStartEnd)
	{
		if(pSrcLineAngle->huoqu_base_wing_x0_y1_both2==0)
			pDestLineAngle->huoqu_base_wing_x0_y1_both2 = 1;
		else if(pSrcLineAngle->huoqu_base_wing_x0_y1_both2==1)
			pDestLineAngle->huoqu_base_wing_x0_y1_both2 = 0;
		else
			pDestLineAngle->huoqu_base_wing_x0_y1_both2 = 2;
		if(pSrcLineAngle->kaihe_base_wing_x0_y1==0)
			pDestLineAngle->kaihe_base_wing_x0_y1 = 1;
		else if(pSrcLineAngle->kaihe_base_wing_x0_y1==1)
			pDestLineAngle->kaihe_base_wing_x0_y1 = 0;
		else
			pDestLineAngle->kaihe_base_wing_x0_y1 = 2;
		pDestLineAngle->SetStart(pSrcLineAngle->End());
		pDestLineAngle->SetEnd(pSrcLineAngle->Start());
		pDestLineAngle->SetStartOdd(pSrcLineAngle->endOdd());
		pDestLineAngle->SetEndOdd(pSrcLineAngle->startOdd());
		pDestLineAngle->set_norm_x_wing(pSrcLineAngle->get_norm_y_wing());
		pDestLineAngle->set_norm_y_wing(pSrcLineAngle->get_norm_x_wing());
		for(CLsRef *pLsRef=pSrcLineAngle->GetFirstLsRef();pLsRef;pLsRef=pSrcLineAngle->GetNextLsRef())
			pDestLineAngle->AppendMidLsRef(*pLsRef);
		pDestLineAngle->set_wing_wide(pSrcLineAngle->GetWidth());
		pDestLineAngle->set_wing_thick(pSrcLineAngle->GetThick());
		pDestLineAngle->pStart = pSrcLineAngle->pEnd;
		pDestLineAngle->pEnd = pSrcLineAngle->pStart;
		pDestLineAngle->cMaterial = pSrcLineAngle->cMaterial;
		pDestLineAngle->iSeg = pSrcLineAngle->iSeg;
		pDestLineAngle->SetPartNo(pSrcLineAngle->GetPartNo());
		pDestLineAngle->handle = pSrcLineAngle->handle;
		pDestLineAngle->huoqu_handle = pSrcLineAngle->huoqu_handle;
		pDestLineAngle->huoqu_r = pSrcLineAngle->huoqu_r;
		pDestLineAngle->m_bCutRoot = pSrcLineAngle->m_bCutRoot;
		pDestLineAngle->m_bCutBer = pSrcLineAngle->m_bCutBer;
		pDestLineAngle->m_bWeldPart = pSrcLineAngle->m_bWeldPart;
		pDestLineAngle->cfgword = pSrcLineAngle->cfgword;
		pDestLineAngle->SetLayer(pSrcLineAngle->layer());
		//切肢切角
		pDestLineAngle->cut_wing[0] = (3-pSrcLineAngle->cut_wing[1])%3;
		pDestLineAngle->cut_wing[1] = (3-pSrcLineAngle->cut_wing[0])%3;
		/*cut_wing[0]表示始端切角cut_wing[1]表示终端切角
		...[][0]表示角钢楞线切取长度
		...[][1]表示另一肢切取宽度
		...[][2]表示肢棱线切取长度
		*/
		pDestLineAngle->cut_wing_para[0][0] = pSrcLineAngle->cut_wing_para[1][0];
		pDestLineAngle->cut_wing_para[0][1] = pSrcLineAngle->cut_wing_para[1][1];
		pDestLineAngle->cut_wing_para[0][2] = pSrcLineAngle->cut_wing_para[1][2];
		pDestLineAngle->cut_wing_para[1][0] = pSrcLineAngle->cut_wing_para[0][0];
		pDestLineAngle->cut_wing_para[1][1] = pSrcLineAngle->cut_wing_para[0][1];
		pDestLineAngle->cut_wing_para[1][2] = pSrcLineAngle->cut_wing_para[0][2];
		
		for(j=0;j<4;j++)
		{
			pDestLineAngle->cut_angle[j][0] = pSrcLineAngle->cut_angle[3-j][0];
			pDestLineAngle->cut_angle[j][1] = pSrcLineAngle->cut_angle[3-j][1];
		}
	}
	else
	{
		pDestLineAngle->huoqu_base_wing_x0_y1_both2=pSrcLineAngle->huoqu_base_wing_x0_y1_both2;
		pDestLineAngle->SetStart(pSrcLineAngle->Start());
		pDestLineAngle->SetEnd(pSrcLineAngle->End());
		pDestLineAngle->SetStartOdd(pSrcLineAngle->startOdd());
		pDestLineAngle->SetEndOdd(pSrcLineAngle->endOdd());
		pDestLineAngle->set_norm_x_wing(pSrcLineAngle->get_norm_x_wing());
		pDestLineAngle->set_norm_y_wing(pSrcLineAngle->get_norm_y_wing());
		for(CLsRef *pLsRef=pSrcLineAngle->GetFirstLsRef();pLsRef;pLsRef=pSrcLineAngle->GetNextLsRef())
			pDestLineAngle->AppendMidLsRef(*pLsRef);
		pDestLineAngle->set_wing_wide(pSrcLineAngle->GetWidth());
		pDestLineAngle->set_wing_thick(pSrcLineAngle->GetThick());
		pDestLineAngle->pStart = pSrcLineAngle->pStart;
		pDestLineAngle->pEnd = pSrcLineAngle->pEnd;
		pDestLineAngle->cMaterial = pSrcLineAngle->cMaterial;
		pDestLineAngle->iSeg = pSrcLineAngle->iSeg;
		pDestLineAngle->SetPartNo(pSrcLineAngle->GetPartNo());
		pDestLineAngle->handle = pSrcLineAngle->handle;
		pDestLineAngle->huoqu_handle = pSrcLineAngle->huoqu_handle;
		pDestLineAngle->huoqu_r = pSrcLineAngle->huoqu_r;
		pDestLineAngle->m_bCutRoot = pSrcLineAngle->m_bCutRoot;
		pDestLineAngle->m_bCutBer = pSrcLineAngle->m_bCutBer;
		pDestLineAngle->m_bWeldPart = pSrcLineAngle->m_bWeldPart;
		pDestLineAngle->cfgword = pSrcLineAngle->cfgword;
		pDestLineAngle->SetLayer(pSrcLineAngle->layer());
		//切肢切角
		pDestLineAngle->cut_wing[0] = pSrcLineAngle->cut_wing[0];
		pDestLineAngle->cut_wing[1] = pSrcLineAngle->cut_wing[1];
		/*cut_wing[0]表示始端切角cut_wing[1]表示终端切角
		...[][0]表示角钢楞线切取长度
		...[][1]表示另一肢切取宽度
		...[][2]表示肢棱线切取长度
		*/
		for(j=0;j<3;j++)
		{
			pDestLineAngle->cut_wing_para[0][j] = pSrcLineAngle->cut_wing_para[0][j];
			pDestLineAngle->cut_wing_para[1][j] = pSrcLineAngle->cut_wing_para[1][j];
		}
		//标识所切角钢肢,0表示无切肢,1表示X肢,2表示Y肢
		// --------- 1.3. 切角 -----------
		/*
		cut_angle[0]表示起点X肢
		cut_angle[1]表示起点Y肢
		cut_angle[2]表示终点X肢
		cut_angle[3]表示终点Y肢
		...[][0]表示平行楞线方向切取量  ...[][1]表示垂直楞线方向切取量
		*/
		for(j=0;j<4;j++)
		{
			pDestLineAngle->cut_angle[j][0] = pSrcLineAngle->cut_angle[j][0];
			pDestLineAngle->cut_angle[j][1] = pSrcLineAngle->cut_angle[j][1];
		}
	}
}

//得到火曲工艺字符串
static int GetHuoQuProcessStr(CTextOnlyDrawing& textDim,double wing_wide,double huoqu_angle,
							  double min_huoqu_angle,char cWing)
{
	textDim.dimText.Empty();
	//正反曲、外内曲
#ifdef AFX_TARG_ENU_ENGLISH
	char sWaiQu[16]="outer blending",sNeiQu[16]="inner blending "; //outer inner blending
	if(sys.part_map.bUseZhengFanQun)
	{	//front reverse blending
		strcpy(sWaiQu,"front blending");
		strcpy(sNeiQu,"reverse blending");
#else
	char sWaiQu[16]="外曲",sNeiQu[16]="内曲"; //内外曲
	if(sys.part_map.bUseZhengFanQun)
	{	//正反曲
		strcpy(sWaiQu,"正曲");
		strcpy(sNeiQu,"反曲");
#endif
	}
	if(fabs(huoqu_angle)>min_huoqu_angle)
	{
		if(sys.part_map.bDimPlaneAngle)
		{	//标注两火曲面夹角
			if(huoqu_angle>0)
#ifdef AFX_TARG_ENU_ENGLISH
				textDim.dimText.Printf("%c leg %s%.1f degree",cWing,sNeiQu,fabs(huoqu_angle));
			else
				textDim.dimText.Printf("%c leg %s%.1f degree",cWing,sWaiQu,fabs(huoqu_angle));
#else
				textDim.dimText.Printf("%c肢%s%.1f度",cWing,sNeiQu,fabs(huoqu_angle));
			else
				textDim.dimText.Printf("%c肢%s%.1f度",cWing,sWaiQu,fabs(huoqu_angle));
#endif
			//
			CXhChar50 sPlaneAngle;
			double fPlaneAngle=180-fabs(huoqu_angle);
#ifdef AFX_TARG_ENU_ENGLISH
			sPlaneAngle.Printf(",two plane angle %.1f degree",fPlaneAngle);
#else
			sPlaneAngle.Printf(",两面夹角%.1f度",fPlaneAngle);
#endif
			textDim.dimText.Append((char*)sPlaneAngle);
		}
		else
		{	//不标注两面夹角时根据角度标注类型标注火曲角度 wht 10-11-02
			if(sys.part_map.iDimAngleType==0)
			{	//标注锐角
				if(huoqu_angle>0)
				{	//大于0表示为内曲
					if(fabs(huoqu_angle)<90)	//锐角
#ifdef AFX_TARG_ENU_ENGLISH
						textDim.dimText.Printf("%c leg %s%.1f degree",cWing,sNeiQu,fabs(huoqu_angle));
					else						//converts obtuse angle to acute angle
						textDim.dimText.Printf("%c leg %s%.1f degree",cWing,sNeiQu,-fabs(huoqu_angle));
#else
						textDim.dimText.Printf("%c肢%s%.1f度",cWing,sNeiQu,fabs(huoqu_angle));
					else						//将钝角转换为锐角
						textDim.dimText.Printf("%c肢%s%.1f度",cWing,sNeiQu,-fabs(huoqu_angle));
#endif
				}
				else 
				{	//小于0表示为外曲
					if(fabs(huoqu_angle)<90)	//锐角
#ifdef AFX_TARG_ENU_ENGLISH
						textDim.dimText.Printf("%c leg %s%.1f degree",cWing,sWaiQu,fabs(huoqu_angle));
					else						//converts obtuse angle to acute angle
						textDim.dimText.Printf("%c leg %s%.1f degree",cWing,sWaiQu,-fabs(huoqu_angle));
#else
						textDim.dimText.Printf("%c肢%s%.1f度",cWing,sWaiQu,fabs(huoqu_angle));
					else						//将钝角转换为锐角
						textDim.dimText.Printf("%c肢%s%.1f度",cWing,sWaiQu,-fabs(huoqu_angle));
#endif
				}
			}
			else if(sys.part_map.iDimAngleType==1)
			{	//标注钝角
				if(huoqu_angle>0)
				{	//大于0表示为内曲
					if(fabs(huoqu_angle)>90)	//钝角
#ifdef AFX_TARG_ENU_ENGLISH
						textDim.dimText.Printf("%c leg %s%.1f degree",cWing,sNeiQu,fabs(huoqu_angle));
					else						//converts acute angle to obtuse angle
						textDim.dimText.Printf("%c leg %s%.1f degree",cWing,sNeiQu,-fabs(huoqu_angle));
#else
						textDim.dimText.Printf("%c肢%s%.1f度",cWing,sNeiQu,fabs(huoqu_angle));
					else						//将锐角转换为钝角
						textDim.dimText.Printf("%c肢%s%.1f度",cWing,sNeiQu,-fabs(huoqu_angle));
#endif
				}
				else 
				{	//小于0表示为外曲
					if(fabs(huoqu_angle)>90)	//钝角
#ifdef AFX_TARG_ENU_ENGLISH
						textDim.dimText.Printf("%c leg %s%.1f degree",cWing,sWaiQu,fabs(huoqu_angle));
					else						//converts acute angle to obtuse angle
						textDim.dimText.Printf("%c leg %s%.1f degree",cWing,sWaiQu,180-fabs(huoqu_angle));
#else
						textDim.dimText.Printf("%c肢%s%.1f度",cWing,sWaiQu,fabs(huoqu_angle));
					else						//将锐角转换为钝角
						textDim.dimText.Printf("%c肢%s%.1f度",cWing,sWaiQu,180-fabs(huoqu_angle));
#endif
				}
			}
			else
			{	//自动选择
				if(huoqu_angle>0)
#ifdef AFX_TARG_ENU_ENGLISH
					textDim.dimText.Printf("%c leg %s%.1f degree",cWing,sNeiQu,fabs(huoqu_angle));
				else
					textDim.dimText.Printf("%c leg %s%.1f degree",cWing,sWaiQu,fabs(huoqu_angle));
#else
					textDim.dimText.Printf("%c肢%s%.1f度",cWing,sNeiQu,fabs(huoqu_angle));
				else
					textDim.dimText.Printf("%c肢%s%.1f度",cWing,sWaiQu,fabs(huoqu_angle));
#endif
			}
		}
		if(sys.part_map.bDimHuoquType)
		{	//标注火曲类型
			CXhChar16 sHuoquType;
			int nRet=ConfirmLineAngleHuoquType((int)wing_wide,huoqu_angle,FALSE);
			if(nRet==2)
#ifdef AFX_TARG_ENU_ENGLISH
				sHuoquType.Copy("(hot blending)");
			else if(nRet==1)
				sHuoquType.Copy("(cold blending)");
#else
				sHuoquType.Copy("(热弯)");
			else if(nRet==1)
				sHuoquType.Copy("(冷弯)");
#endif
			textDim.dimText.Append((char*)sHuoquType);
		}
	}
	return textDim.dimText.GetLength();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void UpdateBoltList(CLDSLineAngle *pLineAngle,UCS_STRU angleDrawUcs,ATOM_LIST<BOLT_INFO> &bolt_list);
void UpdateRibPlateList(CLDSLineAngle *pLineAngle,UCS_STRU angleDrawUcs,ATOM_LIST<RIB_PLATE_INFO> &rib_plate_list);
BOOL CPolyAngleDrawing::CreatePolyAngleDrawing(CLDSPolyPart *pHuoQuJg,double max_drawing_length,double max_drawing_height,
									   double text_height/*=2.5*/,double w_to_h/*=0.7*/,double min_space/*=3.5*/)
{
	if(pHuoQuJg==NULL)
		return FALSE;
	drawPara.fDimTextSize=text_height;
	WtoH=w_to_h;
	minDrawingSpace=min_space;			//孔距最小绘制值
	maxDrawingLength=max_drawing_length;//绘图区最大可用宽度
	maxDrawingHeight=max_drawing_height;//绘图区最大可用高度
	CLDSLineAngle *pFirstAngle=NULL;
	int nBolt = 0;
	m_blScatterBoltLocaTolerance=false;
	for(CLDSLinePart *pLinePart=pHuoQuJg->segset.GetFirst();pLinePart;pLinePart=pHuoQuJg->segset.GetNext())
	{
		if (pLinePart->IsAngle())
			m_blScatterBoltLocaTolerance|=((CLDSLineAngle*)pLinePart)->m_blScatterBoltLocaTolerance;
		if(sPartNo.GetLength()==0)
			sPartNo.Copy(pLinePart->GetPartNo());
		if(pLinePart->GetClassTypeId()!=CLS_LINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "The type of sub part in bending angle is not angle,type error!";
#else
			throw "制弯角钢中的子构件类型不是角钢，类型错误!";
#endif
		nBolt+=pLinePart->GetLsCount();
		if(pFirstAngle==NULL)
		{
			pFirstAngle=(CLDSLineAngle*)pLinePart;
			wingWide=pHuoQuJg->GetWidth();	//角钢肢宽
			wingThick=pHuoQuJg->GetThick();	//角钢肢宽
		}
	}
	length=pHuoQuJg->GetLength();		//角钢长度
	if(sys.part_map.angle.iJgGDimStyle==0)	//始端标注
		dimStyleFlag.SetFlagDword(CAngleDrawing::DimAxisGTextAtStart);
	else if(sys.part_map.angle.iJgGDimStyle==1)	//中间标注
		dimStyleFlag.SetFlagDword(CAngleDrawing::DimAxisGTextAtMiddle);
	else
	{
		if(nBolt<sys.part_map.angle.nMaxBoltNumStartDimG)
			dimStyleFlag.SetFlagDword(CAngleDrawing::DimAxisGTextAtStart);
		else
			dimStyleFlag.SetFlagDword(CAngleDrawing::DimAxisGTextAtMiddle);
	}
	if(sys.part_map.angle.iLsSpaceDimStyle==0)	//X轴向标注孔距
		dimStyleFlag.SetFlagDword(CAngleDrawing::DimSpaceTextAlongX);
	else if(sys.part_map.angle.iLsSpaceDimStyle==1)	//Y轴向标注孔距
		dimStyleFlag.SetFlagDword(CAngleDrawing::DimSpaceTextAlongY);
	else
	{
		if(nBolt<sys.part_map.angle.nMaxBoltNumAlongX)
			dimStyleFlag.SetFlagDword(CAngleDrawing::DimSpaceTextAlongX);
		else
			dimStyleFlag.SetFlagDword(CAngleDrawing::DimSpaceTextAlongY);
	}
	//设置是否需要调整角钢长度 wht 11-04-20
	//if(sys.part_map.angle.bModulateLongJg)
	//	dimStyleFlag.SetFlagDword(CAngleDrawing::ModulateLongJg);
	//设置角钢缩放参数 wht 11-05-07
	if(sys.part_map.angle.iJgZoomSchema==1)
		dimStyleFlag.SetFlagDword(CAngleDrawing::SpecifyDrawscale);	//指定角钢缩放比例，默认为构件图比例
	else if(sys.part_map.angle.iJgZoomSchema==2||sys.part_map.angle.iJgZoomSchema==3)
	{
		dimStyleFlag.SetFlagDword(CAngleDrawing::ModulateLongJg);	//根据绘图区域调整角钢长度 
		if(sys.part_map.angle.iJgZoomSchema==2)
			dimStyleFlag.SetFlagDword(CAngleDrawing::UseMinDrawscale);//长宽同比缩放，使用两比例中较小的缩放比例 
	}
	int i=0,j=0;
	BOOL bFirstSeg = TRUE;
	CArray<UCS_STRU,UCS_STRU&>angleUcsArr;
	
	double realStartPosX=0;
	segArr.SetSize(pHuoQuJg->segset.GetNodeNum());
	angleUcsArr.SetSize(segArr.GetSize());
	CLDSLineAngle *pFirstSeg = (CLDSLineAngle*)pHuoQuJg->segset.GetFirst();
	CLDSLineAngle *pSecondSeg = (CLDSLineAngle*)pHuoQuJg->segset.GetNext();
	CLDSLineAngle *pTailSeg = (CLDSLineAngle*)pHuoQuJg->segset.GetTail();
	CLDSLineAngle* pSeg=NULL,*pPrevSeg =NULL;
	m_nWingAxisGX=m_nWingAxisGY=0;	//清零各类螺栓计数
	//生成首尾衔接的临时角钢段数组
	for(pSeg=(CLDSLineAngle*)pHuoQuJg->segset.GetFirst();pSeg;pSeg=(CLDSLineAngle*)pHuoQuJg->segset.GetNext(),i++)
	{
		BOOL bPrevSegNeedReverseStartEnd=FALSE,bCurSegNeedReverseStartEnd=FALSE;
		if(pPrevSeg==NULL)	//首段火曲子角钢
		{
			if( (pSeg->pStart==pSecondSeg->pStart&&DISTANCE(pSeg->Start(),pSecondSeg->Start())<2.0)||
				(pSeg->pStart==pSecondSeg->pEnd&&DISTANCE(pSeg->Start(),pSecondSeg->End())<2.0))
				bCurSegNeedReverseStartEnd=TRUE;
		}
		else
		{
			if(pPrevSeg->pStart==pSeg->pStart&&DISTANCE(pPrevSeg->Start(),pSeg->Start())<2.0)
			{
				bPrevSegNeedReverseStartEnd=TRUE;
				bCurSegNeedReverseStartEnd=FALSE;
			}
			else if( (pPrevSeg->pStart==pSeg->pEnd&&DISTANCE(pPrevSeg->Start(),pSeg->End())<2.0))
			{
				bPrevSegNeedReverseStartEnd=TRUE;
				bCurSegNeedReverseStartEnd=TRUE;
			}
			else if(pPrevSeg->pEnd==pSeg->pStart&&DISTANCE(pPrevSeg->End(),pSeg->Start())<2.0)
			{
				bPrevSegNeedReverseStartEnd=FALSE;
				bCurSegNeedReverseStartEnd=FALSE;
			}
			else if(pPrevSeg->pEnd==pSeg->pEnd&&DISTANCE(pPrevSeg->End(),pSeg->End())<2.0)
			{
				bPrevSegNeedReverseStartEnd=FALSE;
				bCurSegNeedReverseStartEnd=TRUE;
			}
		}
		//修正制弯点前一角钢段正负头 用于计算火曲变形处长度缩短值
		if(pPrevSeg)
		{
			if(bPrevSegNeedReverseStartEnd)	//前一角钢段始端连接当前制弯点
				pPrevSeg->SetStartOdd(0);
			else	//前一角钢段终端连接当前制弯点
				pPrevSeg->SetEndOdd(0);
			//修正制弯点后一角钢(当前角钢段)正负头 用于计算火曲变形处长度缩短值
			if(bCurSegNeedReverseStartEnd)	//后一角钢段终端连接当前制弯点
				pSeg->SetEndOdd(0);
			else	//后一角钢段始端连接当前制弯点
				pSeg->SetStartOdd(0);
			segArr[i - 1].SetEndOdd(0);	//segArr[i-1]中角钢已调整好始末端顺序 wjh-2019.6.10
		}
		CopyLineAngle(&segArr[i],pSeg,bCurSegNeedReverseStartEnd);
		//以当前段角钢为基准将制弯角钢整体拉直后,制弯角钢的的相对坐标系
		segArr[i].getUCS(angleUcsArr[i],segArr[i].huoqu_base_wing_x0_y1_both2);
		if(pPrevSeg==NULL)
			angleUcsArr[i].origin-=angleUcsArr[i].axis_z*segArr[i].startOdd();	//考虑正负头影响
		else
			angleUcsArr[i].origin-=angleUcsArr[i].axis_z*realStartPosX;
		realStartPosX += segArr[i].GetLength();
		pPrevSeg=pSeg;
	}
	realStartPosX=0;
	LsCirX.SetSize(0,pHuoQuJg->GetLsCount()*2/3);
	LsCirY.SetSize(0,pHuoQuJg->GetLsCount()*2/3);
	for(i=0;i<segArr.GetSize();i++)
	{
		CLDSLineAngle *pLineAngle=&segArr[i];
		pLineAngle->SetBelongModel(pHuoQuJg->BelongModel());
		ATOM_LIST<BOLT_INFO> bolt_list;
		UpdateBoltList(pLineAngle,angleUcsArr[i],bolt_list);
		ATOM_LIST<RIB_PLATE_INFO> rib_plate_list;
		if(drawPara.bDimRibPlateSetUpPos)
			UpdateRibPlateList(pLineAngle,angleUcsArr[i],rib_plate_list);
		StatBoltsInfo(bolt_list);//统计角钢上的螺栓
		realStartPosX+=segArr[i].GetLength();
		if(i<segArr.GetSize()-1)
		{
			LsMark lsCirMark, *pNowMark;
			lsCirMark.dimCircle.centre=lsCirMark.realPos=realStartPosX;
			lsCirMark.dimCircle.radius=0;
			lsCirMark.wingX0Y1=0;
			pNowMark=LsCirX.append(lsCirMark);
			LsCenterArr.append(pNowMark);//center);
			lsCirMark.wingX0Y1=1;
			pNowMark=LsCirY.append(lsCirMark);
			//f3dPoint center(realStartPosX,0,0);
			//center.feature=-1;	//楞线制弯临界点
			LsCenterArr.append(pNowMark);//center);
		}
	}
	CreateLsSpaceDimList();
	CreateLsFuncTextDim();		//特殊螺栓孔的文件标记
	CalDrawingScaleCoef(false,false);	//分析建立角钢外形绘制缩放方案
	for(i=0;i<segArr.GetSize();i++)
	{
		CLDSLineAngle *pLineAngle=&segArr[i];
		ATOM_LIST<RIB_PLATE_INFO> rib_plate_list;
		if(drawPara.bDimRibPlateSetUpPos)
			UpdateRibPlateList(pLineAngle,angleUcsArr[i],rib_plate_list);
		CreateJgRibPlateEdge(rib_plate_list);//标注角钢肋板
		if(i==0)		//首段火曲子角钢
			CreateJgProfile(pLineAngle->cut_wing_para,pLineAngle->cut_wing,pLineAngle->cut_angle,pLineAngle->GetWidth(),pLineAngle->GetWidthWingY(),pLineAngle->GetThick(),TRUE,FALSE);
		else if(i==segArr.GetSize()-1)	//末段火曲子角钢
			CreateJgProfile(pLineAngle->cut_wing_para,pLineAngle->cut_wing,pLineAngle->cut_angle,pLineAngle->GetWidth(),pLineAngle->GetWidthWingY(),pLineAngle->GetThick(),FALSE,TRUE);
	}
	CreateJgLsCircleList();
	for(i=0;i<segArr.GetSize();i++)
		ProcessKaiHeAngle(&segArr[i],angleUcsArr[i],TRUE);			//处理开合角
	//处理火曲点带来的一些特殊绘制(如火曲线,火曲角度标注等)
	realStartPosX=segArr[0].GetLength();
	for(i=1;i<segArr.GetSize();i++)
	{
		f3dPoint cur_huoqu_pos=segArr[i].Start();
		f3dPoint prev_wing_norm_x=segArr[i-1].get_norm_x_wing();
		f3dPoint prev_wing_norm_y=segArr[i-1].get_norm_y_wing();
		f3dPoint vec_start=segArr[i-1].Start()-segArr[i-1].End();
		f3dPoint vec_end=segArr[i].End()-segArr[i].Start();
		normalize(vec_start);
		normalize(vec_end);
		double vec_angle=ACOS(vec_start,vec_end),angle = 0;
		vec_angle*=DEGTORAD_COEF;
		double startPosLenX=TransCoordX(realStartPosX);

		f3dLine line;
		CTextOnlyDrawing pure_txt_dim;
		pure_txt_dim.textHeight=sys.dim_map.fDimTextSize;
		f3dPoint huoqu_line_pick,huoqu_line_vec;
		UCS_STRU prev_ucs=angleUcsArr[i-1];
		coord_trans(cur_huoqu_pos,prev_ucs,FALSE);
		vector_trans(vec_end,prev_ucs,FALSE);
		if(segArr[i-1].huoqu_base_wing_x0_y1_both2==0||segArr[i-1].huoqu_base_wing_x0_y1_both2==2)
		{
			f3dPoint wing_norm_x=segArr[i].get_norm_x_wing();
			if(vec_end.y>0)	//内曲
				angle = ACOS(prev_wing_norm_x*wing_norm_x);
			else
				angle =-ACOS(prev_wing_norm_x*wing_norm_x);
			if(segArr[i-1].huoqu_base_wing_x0_y1_both2==2)
				huoqu_line_pick.Set(wingWide,0,realStartPosX);
			else
			{
				huoqu_line_vec=wing_norm_x^prev_wing_norm_x;
				normalize(huoqu_line_vec);
				vector_trans(huoqu_line_vec,prev_ucs,FALSE);
				huoqu_line_pick=cur_huoqu_pos+huoqu_line_vec*(wingWide/huoqu_line_vec.x);
			}
			angle*=DEGTORAD_COEF;
			//得到X肢火曲工艺字符串
			GetHuoQuProcessStr(pure_txt_dim,segArr[i].GetWidth(),angle,sys.part_map.fMinHuoquAngle,'X');
			if(pure_txt_dim.dimText.GetLength()>0)
			{	//火曲工艺文字标注（坐标在此不进行缩放，在RelayoutDrawing()进行缩放）
				if(segArr[i-1].huoqu_base_wing_x0_y1_both2==2&&sys.part_map.bDimPlaneAngle)
				{	//两肢火曲且标注双面夹角时，需要标注楞线夹角
#ifdef AFX_TARG_ENU_ENGLISH
					pure_txt_dim.dimText.Append(CXhChar50(",ridge angle %.1f degree",vec_angle));
#else
					pure_txt_dim.dimText.Append(CXhChar50(",楞线夹角%.1f度",vec_angle));
#endif
				}
				pure_txt_dim.dimPos.Set(realStartPosX,-wingThick);	//文字标注坐标
				pure_txt_dim.bInXWing=TRUE;
				pure_txt_dim.angle=0;
				pure_txt_dim.iOrder=0;
				pure_txt_dim.bNeedConvertDistToText=false;
				huoqu_txt_arr.append(pure_txt_dim);
				//火曲线位置文字标注
				pure_txt_dim.bNeedConvertDistToText=true;
				pure_txt_dim.dist=huoqu_line_pick.z;
				pure_txt_dim.dimPos.Set(huoqu_line_pick.z,-wingWide);
				if(startPosLenX-pure_txt_dim.dimPos.x>0)
				{
					if(startPosLenX-pure_txt_dim.dimPos.x<pure_txt_dim.GetHeight())
						pure_txt_dim.dimPos.x=startPosLenX-pure_txt_dim.GetHeight()-0.1;
				}
				else if(startPosLenX-pure_txt_dim.dimPos.x<0)
				{
					if(pure_txt_dim.dimPos.x-startPosLenX<pure_txt_dim.GetHeight())
						pure_txt_dim.dimPos.x=startPosLenX+pure_txt_dim.GetHeight()+0.1;
				}
				pure_txt_dim.bInXWing=TRUE;
				pure_txt_dim.angle=Pi/2;
				pure_txt_dim.iOrder=-3; //-3表示螺栓绝对尺寸 wht 11-05-07
				pure_txt_arr.append(pure_txt_dim);
				//制弯线绘制
				HUOQU_LINE xHuoquLine;
				xHuoquLine.huoqu_type=angle>0?0:1;		//X肢火曲
				xHuoquLine.huoqu_angle=fabs(angle);
				xHuoquLine.startPt.Set(startPosLenX+leftGDimMargin,0,0);
				xHuoquLine.endPt.Set(TransCoordX(huoqu_line_pick.z)+leftGDimMargin,TransCoordY(-wingWide),0);
				huoqu_line.Add(xHuoquLine);
			}
		}
		if(segArr[i-1].huoqu_base_wing_x0_y1_both2==1||segArr[i-1].huoqu_base_wing_x0_y1_both2==2)
		{
			f3dPoint wing_norm_y=segArr[i].get_norm_y_wing();
			if(vec_end.x>0)	//内曲
				angle = ACOS(prev_wing_norm_y*wing_norm_y);
			else
				angle =-ACOS(prev_wing_norm_y*wing_norm_y);
			if(segArr[i-1].huoqu_base_wing_x0_y1_both2==2)
				huoqu_line_pick.Set(0,wingWide,realStartPosX);
			else
			{
				huoqu_line_vec=wing_norm_y^prev_wing_norm_y;
				normalize(huoqu_line_vec);
				vector_trans(huoqu_line_vec,prev_ucs,FALSE);
				huoqu_line_pick=cur_huoqu_pos+huoqu_line_vec*(wingWide/huoqu_line_vec.y);
			}

			angle*=DEGTORAD_COEF;
			//得到Y肢火曲工艺字符串
			GetHuoQuProcessStr(pure_txt_dim,wingWide,angle,sys.part_map.fMinHuoquAngle,'Y');
			if(pure_txt_dim.dimText.GetLength()>0)
			{	//火曲工艺文字标注（坐标在此不进行缩放，在RelayoutDrawing()进行缩放）
				if(segArr[i-1].huoqu_base_wing_x0_y1_both2==2&&sys.part_map.bDimPlaneAngle)
				{	//两肢火曲且标注双面夹角时，需要标注楞线夹角
#ifdef AFX_TARG_ENU_ENGLISH
					pure_txt_dim.dimText.Append(CXhChar50(",ridge angle %.1f degree",vec_angle));
#else
					pure_txt_dim.dimText.Append(CXhChar50(",楞线夹角%.1f度",vec_angle));
#endif
				}
				pure_txt_dim.dimPos.Set(realStartPosX,wingThick);
				pure_txt_dim.bInXWing=FALSE;
				pure_txt_dim.angle=0;
				pure_txt_dim.iOrder=0;
				pure_txt_dim.bNeedConvertDistToText=false;
				huoqu_txt_arr.append(pure_txt_dim);
				//火曲线位置文字标注
				pure_txt_dim.bNeedConvertDistToText=true;
				pure_txt_dim.dist=huoqu_line_pick.z;
				pure_txt_dim.dimPos.Set(huoqu_line_pick.z,wingWide);
				if(startPosLenX-pure_txt_dim.dimPos.x>0)
				{
					if(startPosLenX-pure_txt_dim.dimPos.x<pure_txt_dim.GetHeight())
						pure_txt_dim.dimPos.x=startPosLenX-pure_txt_dim.GetHeight()-0.1;
				}
				else if(startPosLenX-pure_txt_dim.dimPos.x<0)
				{
					if(pure_txt_dim.dimPos.x-startPosLenX<pure_txt_dim.GetHeight())
						pure_txt_dim.dimPos.x=startPosLenX+pure_txt_dim.GetHeight()+0.1;
				}
				pure_txt_dim.bInXWing=FALSE;
				pure_txt_dim.angle=Pi/2;
				pure_txt_dim.iOrder=3; //3表示螺栓绝对尺寸 wht 11-05-07
				pure_txt_arr.append(pure_txt_dim);
				//制弯线绘制
				HUOQU_LINE xHuoquLine;
				xHuoquLine.huoqu_type=angle>0?2:3;		//X肢火曲
				xHuoquLine.huoqu_angle=fabs(angle);
				xHuoquLine.startPt.Set(startPosLenX+leftGDimMargin,0,0);
				xHuoquLine.endPt.Set(TransCoordX(huoqu_line_pick.z)+leftGDimMargin,TransCoordY(wingWide),0);
				huoqu_line.Add(xHuoquLine);
			}
		}
		realStartPosX+=segArr[i].GetLength();
	}
	RelayoutDrawing();
	return TRUE;
}

BOOL CPolyAngleDrawing::GetHuoQuLine(f3dLine &line,int index)
{
	if(index>=0&&index<huoqu_line.GetSize())
	{
		line.startPt = huoqu_line[index].startPt;
		line.endPt = huoqu_line[index].endPt;
		return TRUE;
	}
	else
		return FALSE;
}
SCOPE_STRU CPolyAngleDrawing::RelayoutDrawing()
{
	SCOPE_STRU dimRegion=CAngleDrawing::RelayoutDrawing();
	//火曲信息标注
	double fTextHeight=2;
	for(int i=0;i<huoqu_txt_arr.GetSize();i++)
	{
		huoqu_txt_arr[i].dimPos.x=TransCoordX(huoqu_txt_arr[i].dimPos.x);
		huoqu_txt_arr[i].dimPos.x+=leftGDimMargin;
		huoqu_txt_arr[i].alignType=MiddleLeft;
		f3dPoint dimPos=huoqu_txt_arr[i].dimPos;
		fTextHeight=huoqu_txt_arr[i].textHeight;
		if(huoqu_txt_arr[i].dimPos.y<0)
			huoqu_txt_arr[i].dimPos.y=dimRegion.fMinY-fTextHeight*2;
		else
			huoqu_txt_arr[i].dimPos.y=dimRegion.fMaxY+fTextHeight;
		//火曲信息标记字符引线
		f3dLine line(dimPos,huoqu_txt_arr[i].dimPos);
		if(line.endPt.y<0)
			line.endPt.y+=fTextHeight;
		lineArr.Add(line);
	}
	//调整标注矩形区大小
	SCOPE_STRU usedDimRegion;
	usedDimRegion.fMinY=dimRegion.fMinY-fTextHeight*2.5;
	usedDimRegion.fMaxY=dimRegion.fMaxY+fTextHeight*2.5;
	return usedDimRegion;
}
BOOL CPolyAngleDrawing::CreatePolyAngleBendDrawing(CLDSPolyPart *pHuoQuJg)
{
	if(pHuoQuJg==NULL||pHuoQuJg->segset.GetNodeNum()!=2)
		return FALSE;
	//生成首尾衔接的临时角钢段数组
	segArr.SetSize(pHuoQuJg->segset.GetNodeNum());
	CLDSLineAngle *pFirSeg=(CLDSLineAngle*)pHuoQuJg->segset.GetFirst();
	CLDSLineAngle *pSecSeg=(CLDSLineAngle*)pHuoQuJg->segset.GetNext();
	if(pFirSeg==NULL||pSecSeg==NULL)
		return FALSE;
	CLDSLineAngle* pPrevSeg =NULL;
	int i=0;
	m_blScatterBoltLocaTolerance=false;
	for(CLDSLineAngle* pSeg=(CLDSLineAngle*)pHuoQuJg->segset.GetFirst();pSeg;pSeg=(CLDSLineAngle*)pHuoQuJg->segset.GetNext(),i++)
	{
		m_blScatterBoltLocaTolerance|=pSeg->m_blScatterBoltLocaTolerance;
		BOOL bPrevSegNeedReverseStartEnd=FALSE,bCurSegNeedReverseStartEnd=FALSE;
		if(pPrevSeg==NULL)	//首段火曲子角钢
		{
			if((pSeg->pStart==pSecSeg->pStart&&DISTANCE(pSeg->Start(),pSecSeg->Start())<2.0)||
				(pSeg->pStart==pSecSeg->pEnd&&DISTANCE(pSeg->Start(),pSecSeg->End())<2.0))
				bCurSegNeedReverseStartEnd=TRUE;
		}
		else
		{
			if(pPrevSeg->pStart==pSeg->pStart&&DISTANCE(pPrevSeg->Start(),pSeg->Start())<2.0)
			{
				bPrevSegNeedReverseStartEnd=TRUE;
				bCurSegNeedReverseStartEnd=FALSE;
			}
			else if( (pPrevSeg->pStart==pSeg->pEnd&&DISTANCE(pPrevSeg->Start(),pSeg->End())<2.0))
			{
				bPrevSegNeedReverseStartEnd=TRUE;
				bCurSegNeedReverseStartEnd=TRUE;
			}
			else if(pPrevSeg->pEnd==pSeg->pStart&&DISTANCE(pPrevSeg->End(),pSeg->Start())<2.0)
			{
				bPrevSegNeedReverseStartEnd=FALSE;
				bCurSegNeedReverseStartEnd=FALSE;
			}
			else if(pPrevSeg->pEnd==pSeg->pEnd&&DISTANCE(pPrevSeg->End(),pSeg->End())<2.0)
			{
				bPrevSegNeedReverseStartEnd=FALSE;
				bCurSegNeedReverseStartEnd=TRUE;
			}
		}
		//修正制弯点前一角钢段正负头 用于计算火曲变形处长度缩短值
		if(pPrevSeg)
		{
			if(bPrevSegNeedReverseStartEnd)	//前一角钢段始端连接当前制弯点
				pPrevSeg->SetStartOdd(0);
			else	//前一角钢段终端连接当前制弯点
				pPrevSeg->SetEndOdd(0);
			if(bCurSegNeedReverseStartEnd)	//后一角钢段终端连接当前制弯点
				pSeg->SetEndOdd(0);
			else	//后一角钢段始端连接当前制弯点
				pSeg->SetStartOdd(0);
			segArr[i - 1].SetEndOdd(0);	//segArr[i-1]中角钢已调整好始末端顺序 wjh-2019.6.10
		}
		CopyLineAngle(&segArr[i],pSeg,bCurSegNeedReverseStartEnd);
		pPrevSeg=pSeg;
	}
	//分别绘制制弯角钢X肢|Y肢所在的面
	double length=pHuoQuJg->GetLength();
	double fWingW=pFirSeg->GetWidth();
	double fWingT=pFirSeg->GetThick();
	for(int j=0;j<2;j++)
	{
		f3dPoint line_vec1=(segArr[0].End()-segArr[0].Start()).normalized();
		f3dPoint line_vec2=(segArr[1].End()-segArr[1].Start()).normalized();
		f3dPoint wing_vec1=(j==0)?segArr[0].GetWingVecX():segArr[0].GetWingVecY();
		f3dPoint wing_vec2=(j==0)?segArr[1].GetWingVecX():segArr[1].GetWingVecY();
		UCS_STRU draw_ucs;
		draw_ucs.origin=segArr[0].Start()-line_vec1*segArr[0].startOdd();
		draw_ucs.axis_x=line_vec1;
		draw_ucs.axis_y=wing_vec1;
		draw_ucs.axis_z=draw_ucs.axis_x^draw_ucs.axis_y;
		vector_trans(line_vec1,draw_ucs,FALSE);
		vector_trans(wing_vec1,draw_ucs,FALSE);
		vector_trans(line_vec2,draw_ucs,FALSE);
		vector_trans(wing_vec2,draw_ucs,FALSE);
		line_vec1.z=wing_vec1.z=line_vec2.z=wing_vec2.z=0;
		//计算轮廓点
		f3dPoint ptBS,ptBM,ptBE,ptWS,ptWM,ptWE,ptTS,ptTM,ptTE;
		ptBS.Set(0,0,0);
		if(j==1)
			ptBS.Set(0,-fWingW*2,0);
		ptBM=ptBS+line_vec1*segArr[0].GetLength();
		ptBE=ptBM+line_vec2*segArr[1].GetLength();
		ptWS=ptBS+wing_vec1*fWingW;
		ptWE=ptBE+wing_vec2*fWingW;
		if(Int3dll(ptWS,ptWS+line_vec1*1000,ptWE,ptWE-line_vec2*1000,ptWM)<=0)
			ptWM=ptWS+line_vec1*segArr[0].GetLength();
		ptTS=ptBS+wing_vec1*fWingT;
		ptTE=ptBE+wing_vec2*fWingT;
		if(Int3dll(ptTS,ptTS+line_vec1*1000,ptTE,ptTE-line_vec2*1000,ptTM)<=0)
			ptTM=ptTS+line_vec1*segArr[0].GetLength();
		//添加轮廓边
		AppendLine(ptBS,ptBM);
		AppendLine(ptBM,ptBE);
		AppendLine(ptWS,ptWM);
		AppendLine(ptWM,ptWE);
		AppendLine(ptBS,ptWS);
		AppendLine(ptBE,ptWE);
		AppendLine(ptBM,ptWM,PS_DASH);
		AppendLine(ptTS,ptTM,PS_DASH);
		AppendLine(ptTM,ptTE,PS_DASH);
		//添加螺栓孔
		LsMark mark,*pLsMark=NULL;;
		CMaxDouble maxDist;
		CMinDouble minDist;
		for(int i=0;i<2;i++)
		{
			CLDSLineAngle* pSegJg=&segArr[i];
			pSegJg->SetBelongModel(pHuoQuJg->BelongModel());
			UCS_STRU angleDrawUcs;
			pSegJg->getUCS(angleDrawUcs);	
			angleDrawUcs.origin-=angleDrawUcs.axis_z*pSegJg->startOdd();
			f3dPoint org=(i==0)?ptBS:ptBM;
			f3dPoint vecH=(i==0)?line_vec1:line_vec2;
			f3dPoint vecV=(i==0)?wing_vec1:wing_vec2;
			for(CLsRef *pLsRef=pSegJg->GetFirstLsRef();pLsRef;pLsRef=pSegJg->GetNextLsRef())
			{
				CLDSBolt* pLs=(CLDSBolt*)pLsRef->GetLsPtr();
				mark.dimCircle.radius = pLs->get_d()*0.5;
				mark.dimCircle.feature=ftoi(pLs->hole_d_increment*10);
				f3dPoint lsPt=pLs->ucs.origin;
				double g=pSegJg->GetLsG(pLsRef);
				double ddx=pSegJg->vxWingNorm*pLs->get_norm();
				double ddy=pSegJg->vyWingNorm*pLs->get_norm();
				if((fabs(ddx)>fabs(ddy)&&j!=0)||(fabs(ddx)<fabs(ddy)&&j!=1))
					continue;
				f3dPoint wingNorm=(j==0)?pSegJg->vxWingNorm:pSegJg->vyWingNorm;
				Int3dlf(lsPt,lsPt,pLs->get_norm(),angleDrawUcs.origin,wingNorm);
				coord_trans(lsPt,angleDrawUcs,FALSE);
				mark.dimCircle.centre=org+vecH*lsPt.z+vecV*g;
				if(j==0)	//X肢
					pLsMark=LsCirX.append(mark);
				if(j==1)	//Y肢
					pLsMark=LsCirY.append(mark);
				if(i==0)
					maxDist.Update(mark.dimCircle.centre.x,pLsMark);
				else
					minDist.Update(mark.dimCircle.centre.x,pLsMark);
			}
			//标注螺栓到火曲线件的间距
			pLsMark=NULL;
			if(i==0&&maxDist.m_pRelaObj)
				pLsMark=(LsMark*)maxDist.m_pRelaObj;
			else if(i==1&&minDist.m_pRelaObj)
				pLsMark=(LsMark*)minDist.m_pRelaObj;
			if(pLsMark==NULL)
				continue;
			f3dPoint perp,pick=pLsMark->dimCircle.centre;
			double dist=0;
			SnapPerp(&perp,ptBM,ptWM,pick,&dist);
			CSizeTextDim sizeDim;
			sizeDim.dimStartPos=pick;
			sizeDim.dimEndPos=perp;
			sizeDim.dimPos=(sizeDim.dimStartPos+sizeDim.dimEndPos)*0.5;
			sizeDim.dist=dist;
			dim_arr.append(sizeDim);
		}
		//添加火曲角度标注
		if(j==0&&(segArr[0].huoqu_base_wing_x0_y1_both2==0||segArr[0].huoqu_base_wing_x0_y1_both2==2))
		{

		}
		if(j==1&&(segArr[0].huoqu_base_wing_x0_y1_both2==1||segArr[0].huoqu_base_wing_x0_y1_both2==2))
		{

		}
		//添加标注文件
		CTextOnlyDrawing pure_txt_dim;
		pure_txt_dim.textHeight=sys.dim_map.fDimTextSize;
		if(j==0)
			pure_txt_dim.dimText.Printf("%s#X肢拓印视图",(char*)pHuoQuJg->GetPartNo());
		else
			pure_txt_dim.dimText.Printf("%s#Y肢拓印视图",(char*)pHuoQuJg->GetPartNo());
		pure_txt_dim.dimPos=(ptBS+ptBM)*0.5;
		pure_txt_dim.dimPos.y-=pure_txt_dim.textHeight*2;
		huoqu_txt_arr.append(pure_txt_dim);
	}
	return TRUE;
}
#endif