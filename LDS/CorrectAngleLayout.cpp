#include "stdafx.h"
#include "CorrectAngleLayout.h"
#include "GlobalFunc.h"

static char GetWorkPlaneLayer(CLDSLineAngle *pAngle)
{
	char cLayer=toupper(pAngle->Layer(2));
	if(cLayer>='1'&&cLayer<='4')
	{
		f3dPoint ptS=pAngle->pStart->Position(false);
		f3dPoint ptE=pAngle->pEnd->Position(false);
		if(fabs(ptS.x-ptE.x)>fabs(ptS.y-ptE.y))
		{	//水平走向
			if(cLayer=='1'||cLayer=='2')
				cLayer='Q';
			else
				cLayer='H';
		}
		else
		{	//垂直走向
			if(cLayer=='2'||cLayer=='4')
				cLayer='Z';
			else
				cLayer='Y';
		}
	}
	return cLayer;
}
//0.撇外角钢|1.撇里角钢|2.捺里角钢|3.捺外角钢
static int CalAngleLayoutStyle(CLDSLineAngle *pAngle,f3dPoint* near_norm=NULL,BOOL bLegXieJg=FALSE)
{
	int iLayoutStyle=-1;
	bool bFromUpToDown=(pAngle->pEnd->Position(false).z>pAngle->pStart->Position(false).z);
	bool bFromLeftToRight=(pAngle->pEnd->Position(false).x>pAngle->pStart->Position(false).x);
	bool bFromBackToFront=(pAngle->pEnd->Position(false).y>pAngle->pStart->Position(false).y);
	char cLayer=toupper(pAngle->Layer(2));
	if(bLegXieJg)
		cLayer=GetWorkPlaneLayer(pAngle);
	switch(cLayer)
	{
	case 'Q':
		if(bLegXieJg)
		{	//里铁摆放
			if(bFromUpToDown)	//角钢自上至下
				iLayoutStyle=bFromLeftToRight?1:2;
			else				//角钢自下至上
				iLayoutStyle=bFromLeftToRight?2:1;
		}
		else
		{
			if(bFromUpToDown)	//角钢自上至下
				iLayoutStyle=bFromLeftToRight?2:0;	//0.撇角钢 2.捺角钢
			else				//角钢自下至上
				iLayoutStyle=bFromLeftToRight?0:2;	//0.撇角钢 2.捺角钢
		}
		break;
	case 'H':
		if(bLegXieJg)
		{
			if(bFromUpToDown)	//角钢自上至下
				iLayoutStyle=bFromLeftToRight?2:1;
			else				//角钢自下至上
				iLayoutStyle=bFromLeftToRight?1:2;
		}
		else
		{
			if(bFromUpToDown)	//角钢自上至下
				iLayoutStyle=bFromLeftToRight?0:2;	//0.撇角钢 2.捺角钢
			else				//角钢自下至上
				iLayoutStyle=bFromLeftToRight?2:0;	//0.撇角钢 2.捺角钢
		}
		break;
	case 'Z':
		if(bLegXieJg)
		{
			if(bFromUpToDown)	//角钢自上至下
				iLayoutStyle=bFromBackToFront?1:2;
			else				//角钢自下至上
				iLayoutStyle=bFromBackToFront?2:1;	//0.撇角钢 2.捺角钢
		}
		else
		{
			if(bFromUpToDown)	//角钢自上至下
				iLayoutStyle=bFromBackToFront?2:0;	//0.撇角钢 2.捺角钢
			else				//角钢自下至上
				iLayoutStyle=bFromBackToFront?0:2;	//0.撇角钢 2.捺角钢
		}
		break;
	case 'Y':
		if(bLegXieJg)
		{
			if(bFromUpToDown)	//角钢自上至下
				iLayoutStyle=bFromBackToFront?2:1;
			else				//角钢自下至上
				iLayoutStyle=bFromBackToFront?1:2;
		}
		else
		{
			if(bFromUpToDown)	//角钢自上至下
				iLayoutStyle=bFromBackToFront?0:2;	//0.撇角钢 2.捺角钢
			else				//角钢自下至上
				iLayoutStyle=bFromBackToFront?2:0;	//0.撇角钢 2.捺角钢
		}
		break;
	case 'T':
		break;
	default:
		break;
	}
	//计算近似工作法向
	if(near_norm)
	{
		if(cLayer=='Q')
			near_norm->y=(iLayoutStyle==0)?-1:1;
		else if(cLayer=='H')
			near_norm->y=(iLayoutStyle==0)?1:-1;
		else if(cLayer=='Z')
			near_norm->x=(iLayoutStyle==0)?1:-1;
		else if(cLayer=='Y')
			near_norm->x=(iLayoutStyle==0)?-1:1;
	}
	return iLayoutStyle;
}
//0.头模板|1.塔身|2.横膈面|3.多间面|4.K型面|5.V型面|6.接腿正侧面
static void ReadStdFacePanels(int iTemplType,ATOM_LIST<CFacePanel> &templ_list)
{
	if(iTemplType==0)
		return;	//暂不提取头模板
	char file_path[MAX_PATH];
	GetSysPath(file_path);
	strcat(file_path,"facepanel.pns");
	IStorage *pRootStg=NULL,*pStg=NULL;
	::StgOpenStorage(_bstr_t(file_path), NULL,STGM_READ|STGM_SHARE_EXCLUSIVE,NULL, 0, &pRootStg);
	if(pRootStg==NULL)
		return;
	CString sTemplName;
	if(iTemplType==1)	//1表示塔身
		sTemplName="塔身标准面";
	else if(iTemplType==2)	//2:表示横隔面;
		sTemplName="横隔标准面";
	else if(iTemplType==3)	//3:多节间面
		sTemplName="多节间面";
	else if(iTemplType==4)	//3:多节间面
		sTemplName="K型标准面";
	else if(iTemplType==5)	//3:多节间面
		sTemplName="V型标准面";
	else if(iTemplType==6)	//6.接腿正侧面
		sTemplName="接腿正侧面";
	pRootStg->OpenStorage(_bstr_t(sTemplName),NULL,STGM_READ|STGM_SHARE_EXCLUSIVE,NULL,0,&pStg);
	if(pStg==NULL)
		pRootStg->CreateStorage(_bstr_t(sTemplName),STGM_READ|STGM_SHARE_EXCLUSIVE|STGM_CREATE,0,0,&pStg);
	IEnumSTATSTG *pFaceEnum=NULL;
	pStg->EnumElements(0,NULL,0,&pFaceEnum);
	STATSTG face_statstg;
	while(pFaceEnum->Next(1, &face_statstg, NULL) == S_OK)
	{
		if(face_statstg.type!=STGTY_STREAM)
			continue;
		CString face_title=(char*)_bstr_t(face_statstg.pwcsName);
		IStream *pStream=NULL;
		HRESULT hr=pStg->OpenStream(face_statstg.pwcsName,NULL,STGM_READ|STGM_SHARE_EXCLUSIVE,0,&pStream);
		if(pStream)
		{
			CFacePanel* pFacePanel=templ_list.append();
			sprintf(pFacePanel->sLabel,"%s",face_title);
			pFacePanel->FromStream(pStream);
		}
		pStream->Release();
	}
	pFaceEnum->Release();
	pStg->Release();
	pRootStg->Release();
}
//////////////////////////////////////////////////////////////////////////
// NODE_RODS
void NODE_RODS::Add(CLDSLineAngle *pAngle,bool bDiaphragm/*=false*/)
{
	if(bDiaphragm)
		angleSetArr[4].append(pAngle);
	else
	{
		char cThird=toupper(pAngle->Layer(2));
		if('Q'==cThird)
			angleSetArr[0].append(pAngle);
		else if('H'==cThird)
			angleSetArr[1].append(pAngle);
		else if('Z'==cThird)
			angleSetArr[2].append(pAngle);
		else if('Y'==cThird)
			angleSetArr[3].append(pAngle);
		else
			angleSetArr[4].append(pAngle);
	}
}
BOOL NODE_RODS::IsCorrected(CCorrectAngleLayout *pCorrectPos){
	if(m_bCorrected)
		return TRUE;
	for(int i=0;i<5;i++)
	{
		for(CLDSLineAngle *pAngle=angleSetArr[i].GetFirst();pAngle;pAngle=angleSetArr[i].GetNext())
		{
			if(!pCorrectPos->IsCorrectedLayoutPos(pAngle->handle))
				return FALSE;
		}
	}
	return m_bCorrected=TRUE;
}
BOOL NODE_RODS::IsCanCorrect(CCorrectAngleLayout *pCorrectPos)
{
	LAYOUT_POS * pState=pCorrectPos->m_hashRodCorrectedState.GetValue(m_pNode->hFatherPart);
	if(pState==NULL||pState->cLayout==0)
		return FALSE;
	else
		return TRUE;
}
BOOL NODE_RODS::CorrectAngleLayoutPos(CCorrectAngleLayout *pCorrectPos,CLDSLineAngle *pAngle,BYTE cLayout,BYTE cBerStyle,
								      f3dPoint near_work_norm,CLDSLineAngle *pDatumAngle1,CLDSLineAngle *pDatumAngle2/*=NULL*/)
{
	if(pDatumAngle1==NULL)
	{	//初始化基准角钢
		if(pAngle->pEnd==m_pNode)
			pDatumAngle1=(CLDSLineAngle*)console.FromPartHandle(pAngle->pEnd->hFatherPart,CLS_LINEANGLE);
		else 
			pDatumAngle1=(CLDSLineAngle*)console.FromPartHandle(pAngle->pStart->hFatherPart,CLS_LINEANGLE);
	}
	if(pCorrectPos==NULL||pAngle==NULL||pDatumAngle1==NULL||near_work_norm.IsZero()||
		(cLayout!=LAYOUT_POS::LAYOUT_INSIDE&&cLayout!=LAYOUT_POS::LAYOUT_OUTSIDE))
		return FALSE;
	LAYOUT_POS *pState=pCorrectPos->m_hashRodCorrectedState.GetValue(pAngle->handle);
	if(pState&&pState->IsFinished())
		return FALSE;
	//设置角钢里外铁
	if(pState==NULL||pState->cLayout==LAYOUT_POS::LAYOUT_NO)
	{
		LAYOUT_POS *pIntersState=NULL;
		ATOM_LIST<DWORD> *pRodList=pCorrectPos->m_hashIntersRodsById.GetValue(pAngle->handle);
		if(pRodList&&pRodList->GetNodeNum()>0)
			pIntersState=pCorrectPos->m_hashRodCorrectedState.GetValue(*pRodList->GetFirst());
		//默认摆放方式为里铁
		f3dPoint near_norm=near_work_norm,len_vec=pAngle->pEnd->ActivePosition()-pAngle->pStart->ActivePosition();
		DESIGN_JGWING_NORM *pOtherWing=&pAngle->des_norm_x;
		DESIGN_JGWING_NORM *pWorkWing=&pAngle->des_norm_y;
		if(pAngle->pEnd->handle==m_pNode->handle)
		{
			pOtherWing=&pAngle->des_norm_y;
			pWorkWing=&pAngle->des_norm_x;
			len_vec=pAngle->pStart->ActivePosition()-pAngle->pEnd->ActivePosition();
		}
		if(pIntersState&&pIntersState->cLayout==cLayout)
			cLayout=3-cLayout;
		else 
		{	//获取搭接角钢摆放方式及当前肢延伸方向
			int x_wing0_y_wing1=0;
			BOOL bInside=CLDSLineAngle::IsInsideAngle(pDatumAngle1,near_work_norm,&x_wing0_y_wing1);
			f3dPoint datum_wing_vec;
			if(x_wing0_y_wing1==1)
				datum_wing_vec=pDatumAngle1->GetWingVecY();
			else 
				datum_wing_vec=pDatumAngle1->GetWingVecX();
			if((bInside&&cLayout==LAYOUT_POS::LAYOUT_INSIDE)||
			   (!bInside&&cLayout==LAYOUT_POS::LAYOUT_OUTSIDE))
			{	//基准角钢或射线角钢同为里铁或同为外铁
				//射线角钢延伸方向与基准角钢工作肢延伸方向应保持一致
				//否则应调整射线角钢里外铁样式
				if(datum_wing_vec*len_vec<0)
					cLayout=3-cLayout;
			}
		}
		if(cLayout==LAYOUT_POS::LAYOUT_OUTSIDE)
		{
			DESIGN_JGWING_NORM *pTemp=pOtherWing;
			pOtherWing=pWorkWing;
			pWorkWing=pTemp;
			near_norm*=-1;
		}
		//设置角钢摆放方式
		pWorkWing->bSpecific=FALSE;
		pWorkWing->bByOtherWing=FALSE;
		pWorkWing->hViceJg=pDatumAngle1->handle;
		pWorkWing->near_norm=near_norm;
		pOtherWing->bSpecific=FALSE;
		pOtherWing->bByOtherWing=TRUE;
		//
		if(pState==NULL)
			pState=pCorrectPos->m_hashRodCorrectedState.Add(pAngle->handle);
		pState->cLayout=cLayout;
		pAngle->ClearFlag();
		pAngle->cal_x_wing_norm();
		pAngle->cal_y_wing_norm();
		//根据cBerStyle判断是否需要左右翻转角钢肢
		BOOL bTurnWing=FALSE;
		f3dPoint wing_vec;
		if(pWorkWing==&pAngle->des_norm_x)
			wing_vec=pAngle->GetWingVecX();
		else
			wing_vec=pAngle->GetWingVecY();
		//保证楞线在远离/靠近角钢的一侧
		char cThirdChar=toupper(pAngle->Layer(2));
		if(0==cBerStyle||1==cBerStyle)
		{
			f3dPoint datum_len_vec=pDatumAngle1->pEnd->ActivePosition()-pDatumAngle1->pStart->ActivePosition();
			normalize(len_vec);
			normalize(datum_len_vec);
			if(len_vec*datum_len_vec<0)
				datum_len_vec*=-1;
			f3dPoint verfiy_vec=len_vec-datum_len_vec;
			double dd=verfiy_vec*wing_vec;
			if((0==cBerStyle&&dd>0)||	//楞线远离基准角钢
			   (1==cBerStyle&&dd<0))	//楞线靠近基准角钢
			   bTurnWing=TRUE;
		}
		else if(2==cBerStyle&&(cThirdChar=='Q'||cThirdChar=='H'||cThirdChar=='Z'||cThirdChar=='Y'))
			bTurnWing=wing_vec.z<0;	//保证不喝水
		if(bTurnWing)
		{	//角钢肢左右翻转
			pOtherWing->bSpecific=FALSE;
			pOtherWing->bByOtherWing=FALSE;
			pOtherWing->hViceJg=pDatumAngle1->handle;
			pOtherWing->near_norm=near_norm;
			pWorkWing->bSpecific=FALSE;
			pWorkWing->bByOtherWing=TRUE;
			pAngle->ClearFlag();
			pAngle->cal_x_wing_norm();
			pAngle->cal_y_wing_norm();
		}
	}
	//角钢位置
	BOOL bCorrectPos=TRUE;
	if(pState)
		bCorrectPos=!pState->bStartPos;
	long hFatherRod=pAngle->pStart->hFatherPart;
	CMultiOffsetPos *pDesPos=&pAngle->desStartPos;
	if(pAngle->pEnd->handle==m_pNode->handle)
	{
		pDesPos=&pAngle->desEndPos;
		hFatherRod=pAngle->pEnd->hFatherPart;
		if(pState)
			bCorrectPos=!pState->bEndPos;
	}
	if(bCorrectPos)
	{	
		int x_wing0_y_wing1=0;
		pDesPos->jgber_cal_style=0;				//参数化定位
		if(pDatumAngle2)
		{	//角钢心线交点
			pDesPos->spatialOperationStyle=0;		//直接指定基准点
			pDesPos->datumPoint.datum_pos_style=3;	//角钢心线交点
			pDesPos->datumPoint.des_para.AXIS_INTERS.hDatum1=pDatumAngle1->handle;
			pDesPos->datumPoint.des_para.AXIS_INTERS.hDatum2=pDatumAngle2->handle;
			pDesPos->datumPoint.des_para.AXIS_INTERS.wing_offset_style1=0;
			pDesPos->datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;
		}
		else
		{	//X\Y肢心线投影
			CLDSLineAngle::IsInsideAngle(pDatumAngle1,near_work_norm,&x_wing0_y_wing1);
			if(x_wing0_y_wing1==0)
				pDesPos->spatialOperationStyle=2;	//向X肢心线投影
			else
				pDesPos->spatialOperationStyle=3;	//向Y肢心线投影
			if(pDatumAngle1->handle!=hFatherRod)	//设置搭接基准杆件
				pDesPos->datum_jg_h=pDatumAngle1->handle;
			else
				pDesPos->datum_jg_h=0;
		}
		CLDSLineAngle::IsInsideAngle(pAngle,near_work_norm,&x_wing0_y_wing1);
		if(x_wing0_y_wing1==0)
		{
			pDesPos->wing_y_offset.gStyle=0;
			pDesPos->wing_x_offset.gStyle=4;
			pDesPos->wing_x_offset.offsetDist=0;
		}
		else 
		{
			pDesPos->wing_x_offset.gStyle=0;
			pDesPos->wing_y_offset.gStyle=4;
			pDesPos->wing_y_offset.offsetDist=0;
		}
		//记录角钢摆放方式
		if(pState==NULL)
			pState=pCorrectPos->m_hashRodCorrectedState.Add(pAngle->handle);
		if(pAngle->pEnd->handle==m_pNode->handle)
			pState->bEndPos=TRUE;
		else
			pState->bStartPos=TRUE;
		pAngle->ClearFlag();
		pAngle->CalPosition(true);
	}
	return TRUE;
}
//仅连接两根射线杆件的节点
BOOL NODE_RODS::Correct2RayRodsNode(CCorrectAngleLayout *pCorrectPos,BOOL bDiaphragm)
{
	int iStart=bDiaphragm?4:0;
	int iEnd=bDiaphragm?5:4;
	for(int i=iStart;i<iEnd;i++)
	{
		f3dPoint near_work_norm[5]={f3dPoint(0,1,0),f3dPoint(0,-1,0),f3dPoint(-1,0,0),f3dPoint(1,0,0),f3dPoint(0,0,-1)};
		if(angleSetArr[i].GetNodeNum()==2)
		{	//一个节点上仅连接两根射线杆件,一里铁一外铁,保证不喝水
			CLDSLineAngle *pFirstAngle=angleSetArr[i].GetFirst(),*pSecAngle=angleSetArr[i].GetNext();
			LAYOUT_POS *pFirstState=pCorrectPos->m_hashRodCorrectedState.GetValue(pFirstAngle->handle);
			LAYOUT_POS *pSecState=pCorrectPos->m_hashRodCorrectedState.GetValue(pSecAngle->handle);
			if(pFirstState&&pFirstState->IsFinished()&&pSecState&&pSecState->IsFinished())
				return TRUE;
			CLDSLineAngle *pDatumAngle=NULL;
			if(pFirstState&&(pFirstState->cLayout==LAYOUT_POS::LAYOUT_INSIDE||
				pFirstState->cLayout==LAYOUT_POS::LAYOUT_OUTSIDE))
			{
				if(pSecAngle->pEnd==m_pNode)
					pDatumAngle=(CLDSLineAngle*)console.FromPartHandle(pSecAngle->pEnd->hFatherPart,CLS_LINEANGLE);
				else
					pDatumAngle=(CLDSLineAngle*)console.FromPartHandle(pSecAngle->pStart->hFatherPart,CLS_LINEANGLE);
				CorrectAngleLayoutPos(pCorrectPos,pSecAngle,3-pFirstState->cLayout,2,near_work_norm[i],pDatumAngle);
			}
			if(pSecState&&(pSecState->cLayout==LAYOUT_POS::LAYOUT_INSIDE||
				pSecState->cLayout==LAYOUT_POS::LAYOUT_OUTSIDE))
			{
				if(pFirstAngle->pEnd==m_pNode)
					pDatumAngle=(CLDSLineAngle*)console.FromPartHandle(pFirstAngle->pEnd->hFatherPart,CLS_LINEANGLE);
				else
					pDatumAngle=(CLDSLineAngle*)console.FromPartHandle(pFirstAngle->pStart->hFatherPart,CLS_LINEANGLE);
				CorrectAngleLayoutPos(pCorrectPos,pFirstAngle,3-pSecState->cLayout,2,near_work_norm[i],pDatumAngle);
			}
		}
	}
	return TRUE;
}
//修正横隔面角钢摆放方式
BOOL NODE_RODS::CorrectDiaphragm(CCorrectAngleLayout *pCorrectPos)
{	//TODO:此函数功能定位不清晰，未来应重新梳理 wjh-2019.11.19
	LAYOUT_POS *pState=pCorrectPos->m_hashRodCorrectedState.GetValue(m_pNode->hFatherPart);
	if(pState==NULL||pState->cLayout==0)
		return FALSE;
	if(pState->cLayout==LAYOUT_POS::LAYOUT_Z)	
	{	//节点父杆件为主材
		CLDSLineAngle *pAngle,*pDiagAngle,*pHorizAngle1=NULL,*pHorizAngle2=NULL;
		JGSET diagonalAngles;	//横隔面上的对角角钢集合
		for(pAngle=angleSetArr[4].GetFirst();pAngle;pAngle=angleSetArr[4].GetNext())
		{
			if(pAngle->pStart==NULL||pAngle->pEnd==NULL)
				continue;
			if(pAngle->Layer(1)!='H')
				diagonalAngles.append(pAngle);
		}
		for(pDiagAngle=diagonalAngles.GetFirst();pDiagAngle;pDiagAngle=diagonalAngles.GetNext())
		{
			pHorizAngle1=pHorizAngle2=NULL;
			for(pAngle=angleSetArr[4].GetFirst();pAngle;pAngle=angleSetArr[4].GetNext())
			{
				if(pAngle->pStart==NULL||pAngle->pEnd==NULL)
					continue;
				if(pAngle->Layer(1)!='H')
					continue;	//非X/Y向隔面水平横材
				if(!pDiagAngle->ModuleTogetherWith(pAngle))
					continue;	//与当前对角角钢不共存
				if(pHorizAngle1==NULL)
					pHorizAngle1=pAngle;
				else if(pHorizAngle2==NULL)
					pHorizAngle2=pAngle;
				else
					break;
			}
			if(pHorizAngle1&&pHorizAngle2)
			{
				GEPOINT horiLenVec1=pHorizAngle1->pEnd->xPreliftPos-pHorizAngle1->pStart->xPreliftPos;
				GEPOINT horiLenVec2=pHorizAngle2->pEnd->xPreliftPos-pHorizAngle2->pStart->xPreliftPos;
				if(fabs(horiLenVec1*horiLenVec2)<EPS2)	//两横材杆件
					CorrectAngleLayoutPos(pCorrectPos,pDiagAngle,LAYOUT_POS::LAYOUT_INSIDE,0,f3dPoint(0,0,-1),pHorizAngle1,pHorizAngle2);
			}
		}
		m_bCorrected=TRUE;
	}
	//始终端搭接基准角钢位置已确定
	for(CLDSLineAngle *pAngle=angleSetArr[4].GetFirst();pAngle;pAngle=angleSetArr[4].GetNext())
	{
		LAYOUT_POS *pState=pCorrectPos->m_hashRodCorrectedState.GetValue(pAngle->handle);
		if(pState&&pState->IsFinished())
			continue;
		CLDSLineAngle *pStartAngle=(CLDSLineAngle*)console.FromPartHandle(pAngle->pStart->hFatherPart,CLS_LINEANGLE);
		CLDSLineAngle *pEndAngle=(CLDSLineAngle*)console.FromPartHandle(pAngle->pEnd->hFatherPart,CLS_LINEANGLE);
		CLDSLineAngle *pDatumAngle=pStartAngle,*pOtherAngle=pEndAngle;
		if(pAngle->pEnd==m_pNode)
		{
			pDatumAngle=pEndAngle;
			pOtherAngle=pStartAngle;
		}
		LAYOUT_POS *pStartState=pCorrectPos->m_hashRodCorrectedState.GetValue(pAngle->pStart->hFatherPart);
		LAYOUT_POS *pEndState=pCorrectPos->m_hashRodCorrectedState.GetValue(pAngle->pEnd->hFatherPart);
		if(pDatumAngle==NULL||pStartState==NULL||pEndState==NULL||!pStartState->IsFinished()||!pEndState->IsFinished())
			continue;
		BYTE cLayout=LAYOUT_POS::LAYOUT_NO;
		if(toupper(pDatumAngle->Layer(1))=='H'&&(pOtherAngle==NULL||toupper(pOtherAngle->Layer(1))=='H'))
			cLayout=LAYOUT_POS::LAYOUT_OUTSIDE;	//搭接杆件为两根横材时默认为外铁
		else if(pStartState->cLayout==pEndState->cLayout)
		{
			if(LAYOUT_POS::LAYOUT_INSIDE==pStartState->cLayout)
				cLayout=LAYOUT_POS::LAYOUT_OUTSIDE;
			else if(LAYOUT_POS::LAYOUT_OUTSIDE==pStartState->cLayout)
				cLayout=LAYOUT_POS::LAYOUT_INSIDE;
		}
		else if(pStartState->cLayout==LAYOUT_POS::LAYOUT_H)
		{
			if(pEndState->cLayout==LAYOUT_POS::LAYOUT_INSIDE)
				cLayout=LAYOUT_POS::LAYOUT_OUTSIDE;
			else 
				cLayout=LAYOUT_POS::LAYOUT_INSIDE;
		}
		else if(pEndState->cLayout==LAYOUT_POS::LAYOUT_H)
		{
			if(pStartState->cLayout==LAYOUT_POS::LAYOUT_INSIDE)
				cLayout=LAYOUT_POS::LAYOUT_OUTSIDE;
			else
				cLayout=LAYOUT_POS::LAYOUT_INSIDE;
		}
		else if(pStartState->cLayout+pEndState->cLayout==3)
			cLayout=LAYOUT_POS::LAYOUT_INSIDE;
		else
			continue;
		if(cLayout==LAYOUT_POS::LAYOUT_INSIDE||cLayout==LAYOUT_POS::LAYOUT_OUTSIDE)
			CorrectAngleLayoutPos(pCorrectPos,pAngle,cLayout,0,f3dPoint(0,0,-1),pDatumAngle);
	}
	return TRUE;
}
BOOL NODE_RODS::CorrecBodyAuxRod(CCorrectAngleLayout *pCorrectPos)
{
	LAYOUT_POS *pState=pCorrectPos->m_hashRodCorrectedState.GetValue(m_pNode->hFatherPart);
	if(pState==NULL||pState->cLayout==0)
		return FALSE;
	if(pState->cLayout==LAYOUT_POS::LAYOUT_Z)	
	{	//节点父杆件为主材,撇外捺里
		for(int i=0;i<4;i++)
		{
			f3dPoint near_work_norm[4]={f3dPoint(0,1,0),f3dPoint(0,-1,0),f3dPoint(-1,0,0),f3dPoint(1,0,0)};
			for(CLDSLineAngle *pAngle=angleSetArr[i].GetFirst();pAngle;pAngle=angleSetArr[i].GetNext())
			{
				LAYOUT_POS *pState=pCorrectPos->m_hashRodCorrectedState.GetValue(pAngle->handle);
				if(pState&&pState->IsFinished())
					continue;
				//TODO:应排除塔身横材，否则易误修正 wjh-2019.11.19
				int iLayoutStyle=CalAngleLayoutStyle(pAngle);
				if(iLayoutStyle==-1)
					continue;
				CLDSLineAngle *pDatumAngle=NULL;
				if(pAngle->pEnd==m_pNode)
					pDatumAngle=(CLDSLineAngle*)console.FromPartHandle(pAngle->pEnd->hFatherPart,CLS_LINEANGLE);
				else 
					pDatumAngle=(CLDSLineAngle*)console.FromPartHandle(pAngle->pStart->hFatherPart,CLS_LINEANGLE);
				if(iLayoutStyle==0)
					CorrectAngleLayoutPos(pCorrectPos,pAngle,LAYOUT_POS::LAYOUT_OUTSIDE,2,near_work_norm[i],pDatumAngle);
				else if(iLayoutStyle==2)
					CorrectAngleLayoutPos(pCorrectPos,pAngle,LAYOUT_POS::LAYOUT_INSIDE,2,near_work_norm[i],pDatumAngle);
			}
		}
		m_bCorrected=TRUE;
	}
	else if(pState->cLayout==LAYOUT_POS::LAYOUT_H||m_pNode->m_cPosCalType==4)
	{
		f3dPoint near_work_norm[5]={f3dPoint(0,1,0),f3dPoint(0,-1,0),f3dPoint(-1,0,0),f3dPoint(1,0,0),f3dPoint(0,0,-1)};
		for(int i=0;i<4;i++)
		{
			if(angleSetArr[i].GetNodeNum()!=1)
				continue;
			CLDSLineAngle *pAngle=angleSetArr[i].GetFirst();
			CLDSNode *pOtherNode=(pAngle->pStart==m_pNode)?pAngle->pEnd:pAngle->pStart;
			if(pOtherNode->m_cPosCalType==4||m_pNode->m_cPosCalType==4)
				CorrectAngleLayoutPos(pCorrectPos,pAngle,LAYOUT_POS::LAYOUT_OUTSIDE,2,near_work_norm[i],NULL);
		}
	}
	Correct2RayRodsNode(pCorrectPos,FALSE);
	return TRUE;
}
BOOL NODE_RODS::RelayoutRayAngles(CCorrectAngleLayout *pCorrectPos,BOOL bDiaphragm)
{	//修正横隔面
	if(bDiaphragm)
		CorrectDiaphragm(pCorrectPos);
	//修正前后左右面辅材摆放位置
	CorrecBodyAuxRod(pCorrectPos);
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
// CCorrectAngleLayoutPos
void CCorrectAngleLayout::InitNodeRodsList(LINEPARTSET &rodSet)
{
	m_hashNodeRodsById.Empty();
	for(CLDSLinePart *pRod=rodSet.GetFirst();pRod;pRod=rodSet.GetNext())
	{
		if(!pRod->IsAngle()||pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		NODE_PTR nodePtrArr[2]={pRod->pStart,pRod->pEnd};
		for(int i=0;i<2;i++)
		{
			NODE_RODS *pNodeRods=m_hashNodeRodsById.GetValue(nodePtrArr[i]->handle);
			if(pNodeRods==NULL)
			{
				pNodeRods=m_hashNodeRodsById.Add(nodePtrArr[i]->handle);
				pNodeRods->m_pNode=nodePtrArr[i];
			}
			pNodeRods->Add((CLDSLineAngle*)pRod);
		}
	}
}
//初始化交叉杆件集合
void CCorrectAngleLayout::InitIntersRodMap(LINEPARTSET &rodSet,PROGRESS_CONTEXT *pDisplayProcess/*=NULL*/)
{
	m_hashIntersRodsById.Empty();
	CHashList<long> classTypeIdHash;	//杆件类型不同也允许定义交叉点的类型哈希表
	classTypeIdHash.SetValue(CLS_LINEANGLE,CLS_LINEANGLE);
	classTypeIdHash.SetValue(CLS_LINESLOT,CLS_LINESLOT);
	classTypeIdHash.SetValue(CLS_LINEFLAT,CLS_LINEFLAT);
	CLDSLinePart *pFirstRod=NULL,*pSecRod=NULL;
	__int64 i=0,n=rodSet.GetNodeNum();
	__int64 summary=n*n;
	for(pFirstRod=rodSet.GetFirst();pFirstRod;pFirstRod=rodSet.GetNext(),i++)
	{
		if(pFirstRod->pStart==NULL||pFirstRod->pEnd==NULL)
			continue;
		if(pDisplayProcess)
		{
			__int64 index=summary-(n-i)*(n-i);
			DisplayProcess(pDisplayProcess->Mapping((double)(index*100/summary)),pDisplayProcess->title);
		}
		SCOPE_STRU scope;
		f3dPoint intersPt;
		f3dPoint startPt=pFirstRod->pStart->ActivePosition();
		f3dPoint endPt=pFirstRod->pEnd->ActivePosition();
		scope.VerifyVertex(startPt);
		scope.VerifyVertex(endPt);
		int nPush=rodSet.push_stack();
		for(pSecRod=rodSet.GetNext();pSecRod;pSecRod=rodSet.GetNext())
		{
			if(pSecRod->pStart==NULL||pSecRod->pEnd==NULL)
				continue;
			if(pFirstRod->GetClassTypeId()!=pSecRod->GetClassTypeId())
			{
				if(classTypeIdHash.GetValue(pFirstRod->GetClassTypeId())==NULL||
					classTypeIdHash.GetValue(pSecRod->GetClassTypeId())==NULL)
					continue;	//两杆件类型不同且不在指定的非同类杆件列表中，不能添加交叉节点
			}
			SCOPE_STRU scope2;
			scope2.VerifyVertex(pSecRod->pStart->xOriginalPos);
			scope2.VerifyVertex(pSecRod->pEnd->xOriginalPos);
			//if(scope.IsIntersWithLine(pSecRod->pStart->xOriginalPos,pSecRod->pEnd->xOriginalPos))
			if(!scope.IsIntersWith(scope2))
				continue;	//粗判两杆件是否相交
			if(Int3dll(startPt,endPt,pSecRod->pStart->ActivePosition(),pSecRod->pEnd->ActivePosition(),intersPt)!=1)
				continue;
			ATOM_LIST<DWORD> *pList=m_hashIntersRodsById.GetValue(pFirstRod->handle);
			if(pList==NULL)
				pList=m_hashIntersRodsById.Add(pFirstRod->handle);
			pList->append(pSecRod->handle);
			pList=NULL;
			pList=m_hashIntersRodsById.GetValue(pSecRod->handle);
			if(pList==NULL)
				pList=m_hashIntersRodsById.Add(pSecRod->handle);
			pList->append(pFirstRod->handle);
		}
		rodSet.pop_stack(nPush);
	}
	if(pDisplayProcess)
		DisplayProcess(pDisplayProcess->ciToProgressNumber,pDisplayProcess->title);
}
//初始化杆件修订状态
void CCorrectAngleLayout::InitParentRodCorrectState(BOOL bInitIncRodX){
	m_hashRodCorrectedState.Empty();
	for(NODE_RODS *pNodeRods=m_hashNodeRodsById.GetFirst();pNodeRods;pNodeRods=m_hashNodeRodsById.GetNext())
	{
		if(pNodeRods==NULL||pNodeRods->m_pNode==NULL)
			continue;
		CLDSLinePart *pRod=console.FromRodHandle(pNodeRods->m_pNode->hFatherPart);
		if(pRod==NULL)
			continue;
		char cSec=toupper(pRod->Layer(1));
		if('Z'==cSec)
			m_hashRodCorrectedState.SetValue(pRod->handle,LAYOUT_POS(0xFF,TRUE,TRUE));
		if('H'==cSec)
			m_hashRodCorrectedState.SetValue(pRod->handle,LAYOUT_POS(0xFE,TRUE,TRUE));
		if(bInitIncRodX&&'X'==cSec)	//初始化杆件修正状态时包括斜材
			m_hashRodCorrectedState.SetValue(pRod->handle,LAYOUT_POS(0xFD,TRUE,TRUE));
	}
}
void CCorrectAngleLayout::AlignFrontSideHsectAnglesPos()
{
	for(NODE_RODS *pNodeRods=m_hashNodeRodsById.GetFirst();pNodeRods;pNodeRods=m_hashNodeRodsById.GetNext())
	{
		LAYOUT_POS *pState=m_hashRodCorrectedState.GetValue(pNodeRods->m_pNode->hFatherPart);
		if(pState&&pState->cLayout==LAYOUT_POS::LAYOUT_Z)	
		{	//节点父杆件为主材
			CLDSLineAngle *pHorizAngle1=NULL,*pHorizAngle2=NULL;
			for(CLDSLineAngle *pAngle=pNodeRods->angleSetArr[4].GetFirst();pAngle;pAngle=pNodeRods->angleSetArr[4].GetNext())
			{
				char cSec=toupper(pAngle->Layer(1));
				if('H'!=cSec)
					continue;
				if(pHorizAngle1==NULL)
					pHorizAngle1=pAngle;
				else if(pHorizAngle2==NULL)
					pHorizAngle2=pAngle;
				else
					break;
			}
			if(pHorizAngle1==NULL||pHorizAngle2==NULL)
				continue;
			double fWidth1=pHorizAngle1->GetWidth();
			double fWidth2=pHorizAngle2->GetWidth();
			if(fabs(fWidth1-fWidth2)>EPS)
			{
				f3dPoint work_norm(0,0,1);
				normalize(work_norm);
				int x_wing0_y_wing1[2]={0};
				CLDSLineAngle::IsInsideAngle(pHorizAngle1,work_norm,&x_wing0_y_wing1[0]);
				CLDSLineAngle::IsInsideAngle(pHorizAngle2,work_norm,&x_wing0_y_wing1[1]);
				CLDSLineAngle *pDatumAngle=pHorizAngle1,*pOtherAngle=pHorizAngle2;
				if(fWidth1<fWidth2)
				{
					pDatumAngle=pHorizAngle2;
					pOtherAngle=pHorizAngle1;
					int temp=x_wing0_y_wing1[0];
					x_wing0_y_wing1[0]=x_wing0_y_wing1[1];
					x_wing0_y_wing1[1]=temp;
				}
				CMultiOffsetPos *pDesPos=&pOtherAngle->desStartPos;
				if(pOtherAngle->pEnd==pNodeRods->m_pNode)
					pDesPos=&pOtherAngle->desEndPos;
				JGZJ jgzj;
				getjgzj(jgzj,pDatumAngle->GetWidth());
				if(x_wing0_y_wing1[1]==0)
				{
					pDesPos->wing_x_offset.gStyle=4;
					pDesPos->wing_x_offset.offsetDist=jgzj.g;
					pDesPos->wing_y_offset.gStyle=4;
					pDesPos->wing_y_offset.offsetDist=0;
				}
				else 
				{
					pDesPos->wing_x_offset.gStyle=4;
					pDesPos->wing_x_offset.offsetDist=0;
					pDesPos->wing_y_offset.gStyle=4;
					pDesPos->wing_y_offset.offsetDist=jgzj.g;
				}
				pOtherAngle->ClearFlag();
				pOtherAngle->cal_x_wing_norm();
				pOtherAngle->cal_y_wing_norm();
				pOtherAngle->CalPosition(true);
			}
		}
	}
}
CCorrectAngleLayout::CCorrectAngleLayout(LINEPARTSET *pRodSet){
	m_pRodSet=pRodSet;
	if(m_pRodSet==NULL)
	{
		m_xRodSet.Empty();
		for(CLDSLinePart *pRod=console.EnumRodFirst();pRod;pRod=console.EnumRodNext())
		{
			if(!pRod->IsAngle())
				continue;
			m_xRodSet.append(pRod);
		}
		m_pRodSet=&m_xRodSet;
	}
}
void CCorrectAngleLayout::AddExcludeAngleLayout(long hAngle,BYTE cbExcludeFlag)
{
	LAYOUT_POS * pState=m_hashRodCorrectedState.Add(hAngle);
	pState->cLayout|=cbExcludeFlag;
}
BOOL CCorrectAngleLayout::IsCorrectedLayoutPos(long handle)
{
	LAYOUT_POS * pState=m_hashRodCorrectedState.GetValue(handle);
	if(pState==NULL||!pState->IsFinished())
		return FALSE;
	else
		return TRUE;
}
BOOL CCorrectAngleLayout::CorrectByFacePanel(ATOM_LIST<CFacePanel>& templ_list,CLDSNode* pKeyNode,bool bWing2Upward/*=false*/)
{
	if(templ_list.GetNodeNum()<=0)
		return FALSE;
	InitNodeRodsList(*m_pRodSet);
	int nNodeNum=m_hashNodeRodsById.GetNodeNum();
	int nAngleNum=m_pRodSet->GetNodeNum();
	CLDSNode* pMirXNode=pKeyNode->GetMirNode(MIRMSG(1));
	CLDSNode* pMirYNode=pKeyNode->GetMirNode(MIRMSG(2));
	CLDSNode* pMirZNode=pKeyNode->GetMirNode(MIRMSG(3));
	CLDSModule* pActiveModule=console.GetActiveModule();
	CFacePanel* pErrorWingMatchPanel=NULL;	//口朝向有误但其它匹配的不完全匹配横隔面(主要考虑口朝向横隔面不全)
	for(CFacePanel* pPanel=templ_list.GetFirst();pPanel;pPanel=templ_list.GetNext())
	{
		if(pPanel->node_list.GetNodeNum()!=nNodeNum || pPanel->pole_list.GetNodeNum()!=nAngleNum)
			continue;
		matchNodeHashList.Empty();
		matchJgHashList.Empty();
		if(!bWing2Upward&&pPanel->m_bWing2Upward)
			continue;	//口朝向不匹配

		//匹配节点
		arrFaceNodeTemp.Empty();
		for(CFacePanelNode *pFaceNode=pPanel->node_list.GetFirst();pFaceNode;pFaceNode=pPanel->node_list.GetNext())
			arrFaceNodeTemp.append(*pFaceNode);
		for(int i=0;i<arrFaceNodeTemp.GetSize();i++)
		{
			if(arrFaceNodeTemp[i].attach_type==0)
			{
				if(stricmp(arrFaceNodeTemp[i].sTitle,"P1")==0)
					matchNodeHashList.SetValue(arrFaceNodeTemp[i].sTitle,pKeyNode);
				else if(stricmp(arrFaceNodeTemp[i].sTitle,"P2")==0)
					matchNodeHashList.SetValue(arrFaceNodeTemp[i].sTitle,pMirYNode);
				else if(stricmp(arrFaceNodeTemp[i].sTitle,"P3")==0)
					matchNodeHashList.SetValue(arrFaceNodeTemp[i].sTitle,pMirZNode);
				else if(stricmp(arrFaceNodeTemp[i].sTitle,"P4")==0)
					matchNodeHashList.SetValue(arrFaceNodeTemp[i].sTitle,pMirXNode);
			}
			else if(arrFaceNodeTemp[i].attach_type==1)
			{	//-杆件上X坐标值不变点
				f3dPoint ptS,ptE,pos;
				CLDSNode** ppNodeS=matchNodeHashList.GetValue(arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[0]].sTitle);
				if(ppNodeS)
					ptS=(*ppNodeS)->Position(false);
				CLDSNode** ppNodeE=matchNodeHashList.GetValue(arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[1]].sTitle);
				if(ppNodeE)
					ptE=(*ppNodeE)->Position(false);
				CLDSNode** ppNode =matchNodeHashList.GetValue(arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[2]].sTitle);
				if(ppNode)
					pos=(*ppNode)->Position(false);
				else
				{
					f3dPoint vec_sum,vec;
					vec_sum.x=arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[1]].pos.x-
						arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[0]].pos.x;
					vec_sum.y=arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[1]].pos.y-
						arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[0]].pos.y;
					vec.x=arrFaceNodeTemp[i].pos.x-arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[0]].pos.x;
					vec.y=arrFaceNodeTemp[i].pos.y-arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[0]].pos.y;
					double scale=vec.mod()/vec_sum.mod();
					pos=ptS+(ptE-ptS)*scale;
				}
				pos.y=ptS.y+(pos.x-ptS.x)*(ptE.y-ptS.y)/(ptE.x-ptS.x);
				pos.z=ptS.z+(pos.x-ptS.x)*(ptE.z-ptS.z)/(ptE.x-ptS.x);
				CLDSNode *pNode=Ta.CheckOverlappedNode(pos,pActiveModule);
				if(pNode)
					matchNodeHashList.SetValue(arrFaceNodeTemp[i].sTitle,pNode);
			}
			else if(arrFaceNodeTemp[i].attach_type==2)
			{	//-杆件上Y坐标值不变点
				f3dPoint ptS,ptE,pos;
				CLDSNode** ppNodeS=matchNodeHashList.GetValue(arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[0]].sTitle);
				if(ppNodeS)
					ptS=(*ppNodeS)->Position(false);
				CLDSNode** ppNodeE=matchNodeHashList.GetValue(arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[1]].sTitle);
				if(ppNodeE)
					ptE=(*ppNodeE)->Position(false);
				CLDSNode** ppNode =matchNodeHashList.GetValue(arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[2]].sTitle);
				if(ppNode)
					pos=(*ppNode)->Position(false);
				else
				{
					f3dPoint vec_sum,vec;
					vec_sum.x=arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[1]].pos.x-
						arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[0]].pos.x;
					vec_sum.y=arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[1]].pos.y-
						arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[0]].pos.y;
					vec.x=arrFaceNodeTemp[i].pos.x-arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[0]].pos.x;
					vec.y=arrFaceNodeTemp[i].pos.y-arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[0]].pos.y;
					double scale=vec.mod()/vec_sum.mod();
					pos=ptS+(ptE-ptS)*scale;
				}
				pos.x=ptS.x+(pos.y-ptS.y)*(ptE.x-ptS.x)/(ptE.y-ptS.y);
				pos.z=ptS.z+(pos.y-ptS.y)*(ptE.z-ptS.z)/(ptE.y-ptS.y);
				CLDSNode *pNode=Ta.CheckOverlappedNode(pos,pActiveModule);
				if(pNode)
					matchNodeHashList.SetValue(arrFaceNodeTemp[i].sTitle,pNode);
			}
			else if(arrFaceNodeTemp[i].attach_type==3)
			{	//-杆件上Z坐标值不变点
				f3dPoint ptS,ptE,pos;
				CLDSNode** ppNodeS=matchNodeHashList.GetValue(arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[0]].sTitle);
				if(ppNodeS)
					ptS=(*ppNodeS)->Position(false);
				CLDSNode** ppNodeE=matchNodeHashList.GetValue(arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[1]].sTitle);
				if(ppNodeE)
					ptE=(*ppNodeE)->Position(false);
				CLDSNode** ppNode =matchNodeHashList.GetValue(arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[2]].sTitle);
				if(ppNode)
					pos=(*ppNode)->Position(false);
				else
				{
					f3dPoint vec_sum,vec;
					vec_sum.x=arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[1]].pos.x-
						arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[0]].pos.x;
					vec_sum.y=arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[1]].pos.y-
						arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[0]].pos.y;
					vec.x=arrFaceNodeTemp[i].pos.x-arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[0]].pos.x;
					vec.y=arrFaceNodeTemp[i].pos.y-arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[0]].pos.y;
					double scale=vec.mod()/vec_sum.mod();
					pos=ptS+(ptE-ptS)*scale;
				}
				pos.x=ptS.x+(pos.z-ptS.z)*(ptE.x-ptS.x)/(ptE.z-ptS.z);
				pos.y=ptS.y+(pos.z-ptS.z)*(ptE.y-ptS.y)/(ptE.z-ptS.z);
				CLDSNode *pNode=Ta.CheckOverlappedNode(pos,pActiveModule);
				if(pNode)
					matchNodeHashList.SetValue(arrFaceNodeTemp[i].sTitle,pNode);
			}
			else if(arrFaceNodeTemp[i].attach_type==4)
			{	//交叉点
				f3dLine line1,line2;
				CLDSNode** ppNodeS1=matchNodeHashList.GetValue(arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[0]].sTitle);
				if(ppNodeS1)
					line1.startPt=(*ppNodeS1)->Position(false);
				CLDSNode** ppNodeE1=matchNodeHashList.GetValue(arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[1]].sTitle);
				if(ppNodeE1)
					line1.endPt=(*ppNodeE1)->Position(false);
				CLDSNode** ppNodeS2=matchNodeHashList.GetValue(arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[2]].sTitle);
				if(ppNodeS2)
					line2.startPt=(*ppNodeS2)->Position(false);
				CLDSNode** ppNodeE2=matchNodeHashList.GetValue(arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[3]].sTitle);
				if(ppNodeE2)
					line2.endPt=(*ppNodeE2)->Position(false);
				f3dPoint inters;
				int retcode=Int3dll(line1,line2,inters);
				CLDSNode *pNode=retcode!=-1?Ta.CheckOverlappedNode(inters,pActiveModule):NULL;
				if(pNode)
					matchNodeHashList.SetValue(arrFaceNodeTemp[i].sTitle,pNode);
			}
			else if(arrFaceNodeTemp[i].attach_type==5)
			{	//杆件上比例等分点
				f3dPoint pt_S,pt_E;
				CLDSNode** ppNodeS=matchNodeHashList.GetValue(arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[0]].sTitle);
				if(ppNodeS)
					pt_S=(*ppNodeS)->Position(false);
				CLDSNode** ppNodeE=matchNodeHashList.GetValue(arrFaceNodeTemp[arrFaceNodeTemp[i].attach_node_index[1]].sTitle);
				if(ppNodeE)
					pt_E=(*ppNodeE)->Position(false);
				double real_scale=arrFaceNodeTemp[i].attach_scale;
				if(arrFaceNodeTemp[i].attach_scale<0)
				{
					real_scale=-arrFaceNodeTemp[i].attach_scale;
					real_scale=0.1*((int)real_scale)/(real_scale-(int)real_scale);
				}
				f3dPoint pos;
				pos.x=pt_S.x+real_scale*(pt_E.x-pt_S.x);
				pos.y=pt_S.y+real_scale*(pt_E.y-pt_S.y);
				pos.z=pt_S.z+real_scale*(pt_E.z-pt_S.z);
				CLDSNode *pNode=Ta.CheckOverlappedNode(pos,pActiveModule);
				if(pNode)
					matchNodeHashList.SetValue(arrFaceNodeTemp[i].sTitle,pNode);
			}
		}
		if(matchNodeHashList.GetNodeNum()!=nNodeNum)
			return FALSE;
		//匹配角钢
		for(CFacePanelPole *pFacePole=pPanel->pole_list.GetFirst();pFacePole;pFacePole=pPanel->pole_list.GetNext())
		{
			f3dPoint pt_S,pt_E;
			CLDSNode **ppNodeS=matchNodeHashList.GetValue(arrFaceNodeTemp[pFacePole->start_i].sTitle);
			if(ppNodeS)
				pt_S=(*ppNodeS)->Position(false);
			CLDSNode **ppNodeE=matchNodeHashList.GetValue(arrFaceNodeTemp[pFacePole->end_i].sTitle);
			if(ppNodeE)
				pt_E=(*ppNodeE)->Position(false);
			CLDSLinePart* pRod=Ta.CheckOverLappedLinePart(pt_S,pt_E,pActiveModule);
			if(pRod && pRod->IsAngle())
				matchJgHashList.SetValue(pFacePole->sTitle,(CLDSLineAngle*)pRod);
		}
		if(matchJgHashList.GetNodeNum()!=nAngleNum)
			return FALSE;
		if(bWing2Upward!=pPanel->m_bWing2Upward)
			pErrorWingMatchPanel=pPanel;	//口朝向不匹配
		else
			return ExecuteLayoutByFacePanel(pPanel);
	}
	if(pErrorWingMatchPanel)
		return ExecuteLayoutByFacePanel(pErrorWingMatchPanel);
	return FALSE;
}
BOOL CCorrectAngleLayout::ExecuteLayoutByFacePanel(CFacePanel *pFacePanel)
{
	if(pFacePanel==NULL||matchJgHashList.GetNodeNum()<=0)
		return FALSE;
	CLDSNode* pP1=*matchNodeHashList.GetValue(arrFaceNodeTemp[0].sTitle);
	CLDSNode* pP2=*matchNodeHashList.GetValue(arrFaceNodeTemp[1].sTitle);
	CLDSNode* pP3=*matchNodeHashList.GetValue(arrFaceNodeTemp[2].sTitle);
	if(pP1==NULL || pP2==NULL ||pP3==NULL)
		return FALSE;
	f3dPoint line_vec1=pP2->Position(false)-pP1->Position(false);
	f3dPoint line_vec2=pP3->Position(false)-pP2->Position(false);
	f3dPoint work_norm=line_vec1^line_vec2;
	if(work_norm.IsZero())
		work_norm.Set(0,1,0);
	else
		normalize(work_norm);
	CLDSLineAngle* pFirJG=NULL;
	for(CFacePanelPole *pFacePole=pFacePanel->pole_list.GetFirst();pFacePole;pFacePole=pFacePanel->pole_list.GetNext())
	{
		CLDSLineAngle* pAngle=*matchJgHashList.GetValue(pFacePole->sTitle);
		if(pFirJG==NULL)
			pFirJG=pAngle;
		if(pAngle->layer(1)=='H')
			continue;	//横财不做调整;
		CLDSNode* pStart=*matchNodeHashList.GetValue(arrFaceNodeTemp[pFacePole->start_i].sTitle);
		CLDSNode* pEnd=*matchNodeHashList.GetValue(arrFaceNodeTemp[pFacePole->end_i].sTitle);
		if((pAngle->pStart->handle!=pStart->handle && pAngle->pEnd->handle!=pEnd->handle)&&
			(pAngle->pStart->handle!=pEnd->handle && pAngle->pEnd->handle!=pStart->handle))
			continue;
		pAngle->pStart=pStart;
		pAngle->pEnd=pEnd;
		pAngle->SetStart(pStart->Position(false));
		pAngle->SetEnd(pEnd->Position(false));
		if(pAngle->IsAuxPole())
			pAngle->connectStart.wnConnBoltN=pAngle->connectEnd.wnConnBoltN=1;	//辅材默认一颗螺栓，以便于自动拆分节点和批量端螺栓设计 wjh-2017.12.17
		pAngle->des_wing_norm.wLayoutStyle=pFacePole->wxLayout;
		pAngle->des_norm_x.hViceJg=pAngle->des_norm_y.hViceJg=0;
		pAngle->des_norm_x.bSpecific=pAngle->des_norm_y.bSpecific=FALSE;
		pAngle->des_norm_x.bByOtherWing=pAngle->des_norm_y.bByOtherWing=0;
		CLDSLineAngle* pWorkPlaneRefRod=NULL,**ppRefRod=matchJgHashList.GetValue(pFacePole->sRefPole);
		if(ppRefRod)
			pWorkPlaneRefRod=*ppRefRod;
		if(pFacePole->wxLayout.ciLayoutStyle==1)
		{
			pAngle->des_norm_x.near_norm=-work_norm;
			if(pWorkPlaneRefRod)
				pAngle->des_norm_x.hViceJg=pWorkPlaneRefRod->handle;
			pAngle->des_norm_y.bByOtherWing=TRUE;
			pAngle->desStartPos.wing_x_offset.gStyle=pAngle->desEndPos.wing_x_offset.gStyle=4;
			pAngle->desStartPos.wing_x_offset.offsetDist=pAngle->desEndPos.wing_x_offset.offsetDist=0;
			pAngle->desStartPos.wing_y_offset.gStyle=pAngle->desEndPos.wing_y_offset.gStyle=0;
		}
		else if(pFacePole->wxLayout.ciLayoutStyle==2)
		{
			pAngle->des_norm_y.near_norm=work_norm;
			if(pWorkPlaneRefRod)
				pAngle->des_norm_y.hViceJg=pWorkPlaneRefRod->handle;
			pAngle->des_norm_x.bByOtherWing=TRUE;
			pAngle->desStartPos.wing_y_offset.gStyle=pAngle->desEndPos.wing_y_offset.gStyle=4;
			pAngle->desStartPos.wing_y_offset.offsetDist=pAngle->desEndPos.wing_y_offset.offsetDist=0;
			pAngle->desStartPos.wing_x_offset.gStyle=pAngle->desEndPos.wing_x_offset.gStyle=0;
		}
		else if(pFacePole->wxLayout.ciLayoutStyle==3)
		{
			pAngle->des_norm_x.near_norm=work_norm;
			if(pWorkPlaneRefRod)
				pAngle->des_norm_x.hViceJg=pWorkPlaneRefRod->handle;
			pAngle->des_norm_y.bByOtherWing=TRUE;
			pAngle->desStartPos.wing_x_offset.gStyle=pAngle->desEndPos.wing_x_offset.gStyle=4;
			pAngle->desStartPos.wing_x_offset.offsetDist=pAngle->desEndPos.wing_x_offset.offsetDist=0;
			pAngle->desStartPos.wing_y_offset.gStyle=pAngle->desEndPos.wing_y_offset.gStyle=0;
		}
		else
		{
			pAngle->des_norm_y.near_norm=-work_norm;
			if(pWorkPlaneRefRod)
				pAngle->des_norm_y.hViceJg=pWorkPlaneRefRod->handle;
			pAngle->des_norm_x.bByOtherWing=TRUE;
			pAngle->desStartPos.wing_y_offset.gStyle=pAngle->desEndPos.wing_y_offset.gStyle=4;
			pAngle->desStartPos.wing_y_offset.offsetDist=pAngle->desEndPos.wing_y_offset.offsetDist=0;
			pAngle->desStartPos.wing_x_offset.gStyle=pAngle->desEndPos.wing_x_offset.gStyle=0;
		}
		pAngle->ClearFlag();
		pAngle->cal_x_wing_norm();
		pAngle->cal_y_wing_norm();
		pAngle->CalPosition(TRUE);
	}
	return TRUE;
}
void CCorrectAngleLayout::Correct(BOOL bDiaphragm/*=FALSE*/,PROGRESS_CONTEXT *pDisplayProcess/*=NULL*/)
{
	PROGRESS_CONTEXT context;
	if(pDisplayProcess)
	{
		DisplayProcess(0,pDisplayProcess->title);//
		context=*pDisplayProcess;
		context.ciStartProgressNumber=0;
		context.ciToProgressNumber=80;
		InitIntersRodMap(*m_pRodSet,&context);
		DisplayProcess(80,pDisplayProcess->title);//
	}
	else
		InitIntersRodMap(*m_pRodSet,NULL);
	InitNodeRodsList(*m_pRodSet);
	InitParentRodCorrectState(!bDiaphragm);
	if(pDisplayProcess)
		DisplayProcess(85,pDisplayProcess->title);//

	if(bDiaphragm)
		AlignFrontSideHsectAnglesPos();
	int nPrevCanCorrectNode=0;
	int iteration=0,i,n=m_hashNodeRodsById.GetNodeNum();
	while(1)
	{
		NODE_RODS *pNodeRods=NULL;
		iteration++;
		i=0;
		for(pNodeRods=m_hashNodeRodsById.GetFirst();pNodeRods;pNodeRods=m_hashNodeRodsById.GetNext(),i++)
		{
			if(pDisplayProcess)
			{
				if(iteration==1)
					DisplayProcess(85+10*i/n,pDisplayProcess->title);//
				else if(iteration>=2)
					DisplayProcess(95+4*i/n,pDisplayProcess->title);//
			}
			if(pNodeRods->IsCorrected(this)||!pNodeRods->IsCanCorrect(this))
				continue;
			if(!pNodeRods->RelayoutRayAngles(this,bDiaphragm))
				continue;
			NODE_PTR mirNodePtrArr[3]={ pNodeRods->m_pNode->GetMirNode(MIRMSG(1)),
				pNodeRods->m_pNode->GetMirNode(MIRMSG(2)),
				pNodeRods->m_pNode->GetMirNode(MIRMSG(3))};
			for(int i=0;i<3;i++)
			{
				if(mirNodePtrArr[i]==NULL)
					continue;
				NODE_RODS *pMirNodeRods=m_hashNodeRodsById.GetValue(mirNodePtrArr[i]->handle);
				if(pMirNodeRods==NULL||pMirNodeRods->IsCorrected(this)||!pMirNodeRods->IsCanCorrect(this))
					continue;
				pMirNodeRods->RelayoutRayAngles(this,bDiaphragm);
			}
		}
		int nCanCorrectNode=0;
		for(pNodeRods=m_hashNodeRodsById.GetFirst();pNodeRods;pNodeRods=m_hashNodeRodsById.GetNext())
		{
			if(pNodeRods->IsCorrected(this))
				continue;
			if(pNodeRods->IsCanCorrect(this))
				nCanCorrectNode++;
		}
		if(nPrevCanCorrectNode==nCanCorrectNode)
			break;	//前一次能处理的节点数与当前能处理的节点数一致时跳出循环
		nPrevCanCorrectNode=nCanCorrectNode;
		if(nCanCorrectNode<=0)
			break;
	}
	if(pDisplayProcess)
		DisplayProcess(100,pDisplayProcess->title);//
}

//////////////////////////////////////////////////////////////////////////
// CCorrectModelAngleLayout
//修正斜材位置
bool CCorrectModelAngleLayout::CorrectDiagonalAnglePos(CLDSLineAngle *pAngle)
{	//图层
	if(pAngle==NULL||pAngle->pStart==NULL||pAngle->pEnd==NULL)
		return false;
	if (pAngle->group_father_jg_h > 0x20)
		return false;	//不处理组合角钢的子角钢
	char layer[4]={};
	strcpy(layer,pAngle->layer());
	toupper(layer[0]);
	toupper(layer[1]);
	toupper(layer[2]);
	if((layer[0]!='S'&&layer[0]!='L')||layer[1]!='X')
		return false;
	CLDSLinePart  *pStartDatumRod=NULL,*pEndDatumRod=NULL;
	if(pAngle->desStartPos.datum_jg_h>0x20&&pAngle->desStartPos.datum_jg_h!=pAngle->handle)
		pStartDatumRod=console.FromRodHandle(pAngle->desStartPos.datum_jg_h);
	if(pStartDatumRod==NULL&&pAngle->pStart->hFatherPart!=pAngle->handle)
		pStartDatumRod=console.FromRodHandle(pAngle->pStart->hFatherPart);
	//
	if(pAngle->desEndPos.datum_jg_h>0x20&&pAngle->desEndPos.datum_jg_h!=pAngle->handle)
		pEndDatumRod=console.FromRodHandle(pAngle->desEndPos.datum_jg_h);
	if(pEndDatumRod==NULL&&pAngle->pEnd->hFatherPart!=pAngle->handle)
		pEndDatumRod=console.FromRodHandle(pAngle->pEnd->hFatherPart);
	if(pStartDatumRod==NULL||pEndDatumRod==NULL)
		return false;
	//塔身斜材一般为撇外捺里
	f3dPoint near_norm;
	BOOL bXieCaiJg=(layer[0]=='L')?TRUE:FALSE;
	int iLayoutStyle=CalAngleLayoutStyle(pAngle,&near_norm,bXieCaiJg);
	if(iLayoutStyle==-1)
		return false;
	DESIGN_JGWING_NORM *pOtherWing=NULL,*pWorkWing=NULL;
	pAngle->des_wing_norm.wLayoutStyle=iLayoutStyle;
	pAngle->des_norm_x.hViceJg=pAngle->des_norm_y.hViceJg=0;
	pAngle->des_norm_x.bSpecific=pAngle->des_norm_y.bSpecific=FALSE;
	pAngle->des_norm_x.bByOtherWing=pAngle->des_norm_y.bByOtherWing=0;
	GEPOINT posstart=(pAngle->pStart!=NULL&&pAngle->pEnd!=NULL)?pAngle->pStart->xOriginalPos:pAngle->xPosStart;
	GEPOINT posend=(pAngle->pStart!=NULL&&pAngle->pEnd!=NULL)?pAngle->pEnd->xOriginalPos:pAngle->xPosEnd;
	bool bFromUpToDown=posend.z>posstart.z;
	if(layer[0]=='S')
	{	//塔身斜材撇外捺里，不喝水
		if(bFromUpToDown)
		{
			pWorkWing=&pAngle->des_norm_x;
			pOtherWing=&pAngle->des_norm_y;
		}
		else 
		{
			pWorkWing=&pAngle->des_norm_y;
			pOtherWing=&pAngle->des_norm_x;
		}
	}
	else //if(layer[0]=='L')
	{	//塔腿斜材,撇捺角钢均设置为里铁
		if((iLayoutStyle==1&&bFromUpToDown)||(iLayoutStyle==2&&!bFromUpToDown))
		{	//撇里
			pWorkWing=&pAngle->des_norm_y;
			pOtherWing=&pAngle->des_norm_x;
		}
		else
		{
			pWorkWing=&pAngle->des_norm_x;
			pOtherWing=&pAngle->des_norm_y;
		}
	}
	pWorkWing->hViceJg=pStartDatumRod->handle;
	pWorkWing->near_norm=near_norm;
	pOtherWing->bByOtherWing=TRUE;
	//设置塔身斜材、横材始终端定位方式
	f3dPoint len_vec=pAngle->pEnd->Position()-pAngle->pStart->Position();
	f3dPoint datum_len_vec;
	if(pStartDatumRod->pStart&&pStartDatumRod->pEnd)
		datum_len_vec=pStartDatumRod->pEnd->Position()-pStartDatumRod->pStart->Position();
	else
		datum_len_vec=pStartDatumRod->End()-pStartDatumRod->Start();
	f3dPoint work_norm=len_vec^datum_len_vec;
	normalize(work_norm);
	ROD_PTR datumRodPtrArr[2]={pStartDatumRod,pEndDatumRod};
	for(int i=0;i<2;i++)
	{
		CMultiOffsetPos *pDesPos=&pAngle->desStartPos;
		if(i==1)
			pDesPos=&pAngle->desEndPos;
		datum_len_vec=datumRodPtrArr[i]->pEnd->Position()-datumRodPtrArr[i]->pStart->Position();
		normalize(datum_len_vec);
		BOOL bFaceOffset=fabs(datum_len_vec.z)>0.2;			//平推
		if(datumRodPtrArr[i]->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//搭接基准角钢为组合角钢
			pDesPos->jgber_cal_style=0;
			pDesPos->len_offset_dist=pDesPos->len_offset_type=0;
			pDesPos->spatialOperationStyle=!bFaceOffset?1:4;	//向楞线投影或平推
			if(bFaceOffset)
				pDesPos->face_offset_norm.Set(0,0,1);
		}
		else if(datumRodPtrArr[i]->GetClassTypeId()==CLS_LINEANGLE)
		{	//搭接基准角钢为单角钢
			CLDSLineAngle *pDatumAngle=(CLDSLineAngle*)datumRodPtrArr[i];
			int x_wing0_y_wing1=0;
			CLDSLineAngle::IsInsideAngle(pDatumAngle,work_norm,&x_wing0_y_wing1);
			pDesPos->jgber_cal_style=0;
			pDesPos->len_offset_dist=pDesPos->len_offset_type=0;
			if(x_wing0_y_wing1==0)
				pDesPos->spatialOperationStyle=!bFaceOffset?2:5;	//向X肢投影或平推
			else if(x_wing0_y_wing1==1)
				pDesPos->spatialOperationStyle=!bFaceOffset?3:6;	//向Y肢投影或平推
			if(bFaceOffset)
				pDesPos->face_offset_norm.Set(0,0,1);
		}
		else
			continue;
		if((&pAngle->des_norm_x)==pWorkWing)
		{	//X肢为工作肢
			pDesPos->wing_x_offset.gStyle=4;
			pDesPos->wing_x_offset.offsetDist=0;
			pDesPos->wing_y_offset.gStyle=0;
		}
		else //if((&pAngle->des_norm_y)==pWorkWing)
		{	//Y肢为工作肢
			pDesPos->wing_y_offset.gStyle=4;
			pDesPos->wing_y_offset.offsetDist=0;
			pDesPos->wing_x_offset.gStyle=0;
		}
	}
	pAngle->ClearFlag();
	pAngle->cal_x_wing_norm();
	pAngle->cal_y_wing_norm();
	pAngle->CalPosition(true);
	return true;
}
//修正横材位置
bool CCorrectModelAngleLayout::CorrectHorizAnglePos(CLDSLineAngle *pAngle,const double* vcNearWorkNormal/*=NULL*/)
{
	if(pAngle==NULL||pAngle->pStart==NULL||pAngle->pEnd==NULL)
		return false;
	//塔身\塔腿横材
	if(pAngle->chLayer1st=='T'||pAngle->chLayer2nd!='H')
		return false;
	GEPOINT vHoriWingNearNormnal(0,0,-1);
	if(vcNearWorkNormal)
		vHoriWingNearNormnal=GEPOINT(vcNearWorkNormal);
	CLDSLinePart  *pStartDatumRod=NULL,*pEndDatumRod=NULL;
	if(pAngle->desStartPos.datum_jg_h>0x20&&pAngle->desStartPos.datum_jg_h!=pAngle->handle)
		pStartDatumRod=console.FromRodHandle(pAngle->desStartPos.datum_jg_h);
	if(pStartDatumRod==NULL&&pAngle->pStart->hFatherPart!=pAngle->handle)
		pStartDatumRod=console.FromRodHandle(pAngle->pStart->hFatherPart);
	//
	if(pAngle->desEndPos.datum_jg_h>0x20&&pAngle->desEndPos.datum_jg_h!=pAngle->handle)
		pEndDatumRod=console.FromRodHandle(pAngle->desEndPos.datum_jg_h);
	if(pEndDatumRod==NULL&&pAngle->pEnd->hFatherPart!=pAngle->handle)
		pEndDatumRod=console.FromRodHandle(pAngle->pEnd->hFatherPart);
	if(pStartDatumRod==NULL||pEndDatumRod==NULL)
		return false;
	
	DESIGN_JGWING_NORM *pOtherWing=NULL,*pWorkWing=NULL;
	//2.横隔面(连接横担位置下横隔面横材喝水)
	f3dPoint startPt=pAngle->pStart->Position();
	f3dPoint endPt=pAngle->pEnd->Position();
	//if(fabs(startPt.z-endPt.z)>EPS)
	//	return false;	判断多余，万一横材角钢因为参数设定有误导致端点位置有偏差不水平时会漏修正 wjh-2019.6.2
	f3dPoint vWorkWingNearNormal, vIdleWingNearNormal(0,0,-1);	//工作连接肢与另一（空闲）肢的肢法线参照方向
	if(fabs(pAngle->pStart->xOriginalPos.y-pAngle->pEnd->xOriginalPos.y)<EPS)
	{	//前后面横材
		vWorkWingNearNormal.Set(0,1,0);
		if(startPt.y<0)
			vWorkWingNearNormal.Set(0,-1,0);
		if( (startPt.x<endPt.x&&vWorkWingNearNormal.y>0)||	//前面
			(startPt.x>endPt.x&&vWorkWingNearNormal.y<0))		//后面
		{	//X肢为当前面(前面或后面)工作肢
			pOtherWing=&pAngle->des_norm_y;
			pWorkWing=&pAngle->des_norm_x;
		}
		else 
		{	//Y肢为当前面(前面或后面)工作肢
			pOtherWing=&pAngle->des_norm_x;
			pWorkWing=&pAngle->des_norm_y;
		}
	}
	else if(fabs(pAngle->pStart->xOriginalPos.x-pAngle->pEnd->xOriginalPos.x)<EPS)
	{	//左右面横材
		vWorkWingNearNormal.Set(1,0,0);
		if(startPt.x<0)
			vWorkWingNearNormal.Set(-1,0,0);
		if( (startPt.y>endPt.y&&vWorkWingNearNormal.x>0)||	//右面
			(startPt.y<endPt.y&&vWorkWingNearNormal.x<0))		//左面
		{	//X肢为当前面(左面或右面)工作肢
			pOtherWing=&pAngle->des_norm_y;
			pWorkWing=&pAngle->des_norm_x;
		}
		else 
		{	//Y肢为当前面(左面或右面)工作肢
			pOtherWing=&pAngle->des_norm_x;
			pWorkWing=&pAngle->des_norm_y;
		}
	}
	else 
		return false;
	if(vHoriWingNearNormnal.z>0)
	{	//连接横担位置下横隔面横材喝水
		DESIGN_JGWING_NORM *pTmpWing=pWorkWing;
		pWorkWing=pOtherWing;
		pOtherWing=pTmpWing;
		vIdleWingNearNormal.Set(0,0,1);
	}
	//设置横材肢法线
	pOtherWing->bSpecific=TRUE;
	pOtherWing->bByOtherWing=FALSE;
	pOtherWing->spec_norm.norm_style=0;
	pOtherWing->spec_norm.vector=vIdleWingNearNormal;
	//
	pWorkWing->bSpecific=FALSE;
	pWorkWing->bByOtherWing=FALSE;
	pWorkWing->hViceJg=pStartDatumRod->handle;
	pWorkWing->near_norm=vWorkWingNearNormal;
	//设置塔身斜材、横材始终端定位方式
	f3dPoint len_vec=pAngle->pEnd->Position()-pAngle->pStart->Position();
	BYTE ciFaceIncrement=fabs(len_vec.z)>1?3:0;
	f3dPoint datum_len_vec;
	if(pStartDatumRod->pStart&&pStartDatumRod->pEnd)
		datum_len_vec=pStartDatumRod->pEnd->Position()-pStartDatumRod->pStart->Position();
	else
		datum_len_vec=pStartDatumRod->End()-pStartDatumRod->Start();
	f3dPoint work_norm=len_vec^datum_len_vec;
	normalize(work_norm);
	ROD_PTR datumRodPtrArr[2]={pStartDatumRod,pEndDatumRod};
	for(int i=0;i<2;i++)
	{
		CMultiOffsetPos *pDesPos=&pAngle->desStartPos;
		if(i==1)
			pDesPos=&pAngle->desEndPos;
		if(datumRodPtrArr[i]->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//搭接基准角钢为组合角钢
			pDesPos->jgber_cal_style=0;
			pDesPos->len_offset_dist=pDesPos->len_offset_type=0;
			pDesPos->spatialOperationStyle=1+ciFaceIncrement;	//向楞线平推
			pDesPos->face_offset_norm.Set(0,0,1);
		}
		else if(datumRodPtrArr[i]->GetClassTypeId()==CLS_LINEANGLE)
		{	//搭接基准角钢为单角钢
			CLDSLineAngle *pDatumAngle=(CLDSLineAngle*)datumRodPtrArr[i];
			int x_wing0_y_wing1=0;
			CLDSLineAngle::IsInsideAngle(pDatumAngle,work_norm,&x_wing0_y_wing1);
			pDesPos->jgber_cal_style=0;
			pDesPos->len_offset_dist=pDesPos->len_offset_type=0;
			if(x_wing0_y_wing1==0)
				pDesPos->spatialOperationStyle=2+ciFaceIncrement;	//向X肢平推
			else if(x_wing0_y_wing1==1)
				pDesPos->spatialOperationStyle=3+ciFaceIncrement;	//向Y肢平推
			pDesPos->face_offset_norm.Set(0,0,1);
		}
		else
			continue;
		if((&pAngle->des_norm_x)==pWorkWing)
		{	//X肢为工作肢
			pDesPos->wing_x_offset.gStyle=4;
			pDesPos->wing_x_offset.offsetDist=0;
			pDesPos->wing_y_offset.gStyle=0;
		}
		else //if((&pAngle->des_norm_y)==pWorkWing)
		{	//Y肢为工作肢
			pDesPos->wing_y_offset.gStyle=4;
			pDesPos->wing_y_offset.offsetDist=0;
			pDesPos->wing_x_offset.gStyle=0;
		}
	}
	pAngle->ClearFlag();
	pAngle->cal_x_wing_norm();
	pAngle->cal_y_wing_norm();
	pAngle->CalPosition(true);
	return true;
}
GEPOINT CCorrectModelAngleLayout::GetNearTrunkHorizAngleNormal(CLDSNode* pNode,
	char ciDetectArmDirection/*='X'*/,double TANGENT_TOLERANCE/*=0.05*/)
{
	GEPOINT vOffset(10,0,0);
	if(ciDetectArmDirection=='Y')
	{
		if(pNode->xOriginalPos.y>0)
			vOffset.Set(0, 10,0);
		else
			vOffset.Set(0,-10,0);
	}
	else
	{
		if(ciDetectArmDirection!='X')
			ciDetectArmDirection='X';	//只能是'X'或'Y'
		if(pNode->xOriginalPos.x<0)
			vOffset.Set(-10,0,0);
	}
	GEPOINT inters,xSectOrg=pNode->xOriginalPos+vOffset;
	double dfMaxCosa2ArmAxis=0;
	SmartRodPtr pOutRod;
	CLdsPartListStack statck(&Ta.Parts);
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(!pRod->IsAngle()||pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		if(toupper(pRod->Layer(0))!='T'||toupper(pRod->Layer(1))!='Z')
			continue;
		char ciIntersPos=0;	//'X':交点们于X轴正向;'x'交点位于X轴负向;'Y':交点位于Y轴正向;'y':交点位于Y轴负向
		GEPOINT xInner,xOuter;
		double scaleOfY2X,scaleOfZ2X,scaleOfX2Y,scaleOfZ2Y;
		if(ciDetectArmDirection=='X')
		{
			if(pRod->pStart==pNode && xSectOrg.x>0&&pRod->pEnd->xOriginalPos.x>xSectOrg.x)
			{
				ciIntersPos='X';
				xInner=pRod->pStart->xOriginalPos;
				xOuter=pRod->pEnd->xOriginalPos;
			}
			else if(pRod->pStart==pNode && xSectOrg.x<0&&pRod->pEnd->xOriginalPos.x<xSectOrg.x)
			{
				ciIntersPos='x';
				xInner=pRod->pStart->xOriginalPos;
				xOuter=pRod->pEnd->xOriginalPos;
			}
			else if(pRod->pEnd==pNode && xSectOrg.x>0&&pRod->pStart->xOriginalPos.x>xSectOrg.x)
			{
				ciIntersPos='X';
				xInner=pRod->pEnd->xOriginalPos;
				xOuter=pRod->pStart->xOriginalPos;
			}
			else if(pRod->pEnd==pNode && xSectOrg.x<0&&pRod->pStart->xOriginalPos.x<xSectOrg.x)
			{
				ciIntersPos='x';
				xInner=pRod->pEnd->xOriginalPos;
				xOuter=pRod->pStart->xOriginalPos;
			}
			else
				continue;
			scaleOfY2X=(xOuter.y-xInner.y)/(xOuter.x-xInner.x);
			scaleOfZ2X=(xOuter.z-xInner.z)/(xOuter.x-xInner.x);
			if(ciIntersPos=='X')
				vOffset.Set( 10, 10*scaleOfY2X, 10*scaleOfZ2X);
			else //if(ciIntersPos=='x')
				vOffset.Set(-10,-10*scaleOfY2X,-10*scaleOfZ2X);
		}
		else //if(ciDetectArmDirection=='Y')
		{
			if(pRod->pStart==pNode && xSectOrg.y>0&&pRod->pEnd->xOriginalPos.y>xSectOrg.y)
			{
				ciIntersPos='Y';
				xInner=pRod->pStart->xOriginalPos;
				xOuter=pRod->pEnd->xOriginalPos;
			}
			else if(pRod->pStart==pNode && xSectOrg.y<0&&pRod->pEnd->xOriginalPos.y<xSectOrg.y)
			{
				ciIntersPos='y';
				xInner=pRod->pStart->xOriginalPos;
				xOuter=pRod->pEnd->xOriginalPos;
			}
			else if(pRod->pEnd==pNode && xSectOrg.y>0&&pRod->pStart->xOriginalPos.y>xSectOrg.y)
			{
				ciIntersPos='Y';
				xInner=pRod->pEnd->xOriginalPos;
				xOuter=pRod->pStart->xOriginalPos;
			}
			else if(pRod->pEnd==pNode && xSectOrg.y<0&&pRod->pStart->xOriginalPos.y<xSectOrg.y)
			{
				ciIntersPos='y';
				xInner=pRod->pEnd->xOriginalPos;
				xOuter=pRod->pStart->xOriginalPos;
			}
			else
				continue;
			scaleOfX2Y=(xOuter.x-xInner.x)/(xOuter.y-xInner.y);
			scaleOfZ2Y=(xOuter.z-xInner.z)/(xOuter.y-xInner.y);
			if(ciIntersPos=='Y')
				vOffset.Set( 10*scaleOfX2Y, 10, 10*scaleOfZ2Y);
			else //if(ciIntersPos=='y')
				vOffset.Set(-10*scaleOfX2Y,-10,-10*scaleOfZ2Y);
		}
		inters=xInner+vOffset;
		GEPOINT vLenStdVec=vOffset;
		normalize(vLenStdVec);
		if(toupper(ciDetectArmDirection)=='X'&&fabs(scaleOfZ2X)<TANGENT_TOLERANCE)
		{	//fabs(scaleOfZ2X)<0.05用于判断是否为水平横担材
			if(fabs(vLenStdVec.x)>dfMaxCosa2ArmAxis)
			{	//找最接近横担延伸轴方向的横担主材杆件
				pOutRod=pRod;
				dfMaxCosa2ArmAxis=fabs(vLenStdVec.x);
			}
			if(pOutRod.IsHasPtr()&&fabs(scaleOfY2X)<0.15&&fabs(scaleOfZ2X)<0.15)
				break;	//tan(10°)=0.176,此处<0.15基本可判定杆件为横担弦杆主材
		}
		else if(toupper(ciDetectArmDirection)=='Y'&&fabs(scaleOfZ2Y)<TANGENT_TOLERANCE)
		{	//fabs(scaleOfZ2Y)<0.05用于判断是否为水平横担材
			if(fabs(vLenStdVec.y)>dfMaxCosa2ArmAxis)
			{	//找最接近横担延伸轴方向的横担主材杆件
				pOutRod=pRod;
				dfMaxCosa2ArmAxis=fabs(vLenStdVec.y);
			}
			if(pOutRod.IsHasPtr()&&fabs(scaleOfX2Y)<0.15&&fabs(scaleOfZ2Y)<0.15)
				break;	//tan(10°)=0.176,此处<0.15基本可判定杆件为横担弦杆主材
		}
	}
	if(pOutRod.IsNULL()||!pOutRod->IsAngle())
		return GEPOINT(0,0,-1);	//未找到横担主材或横担主材非角钢时，塔身横材默认口朝下
	else
	{
		if(fabs(pOutRod.pAngle->vxWingNorm.z)>fabs(pOutRod.pAngle->vyWingNorm.z))
			return pOutRod.pAngle->vxWingNorm;
		else
			return pOutRod.pAngle->vyWingNorm;
	}
}
//修正横隔面角钢位置
void CCorrectModelAngleLayout::CorrectDiaphragm()			
{	//1.提取横材及横隔面(水平且两端连接在主材上)
	CMapList<CLDSLineAngle*> horizAngleSet;
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if( pRod->pStart==NULL||pRod->pEnd==NULL||
			!pRod->IsAngle()||toupper(pRod->Layer(0))=='T')
			continue;
		CLDSLineAngle *pAngle=(CLDSLineAngle*)pRod;
		if (pAngle->blGroupSonAngle)
			continue;	//组合角钢的子角钢随组合角钢重置不单独处理肢朝向，否则会互相冲突 wjh-2019.11.19
		CLDSLinePart *pStartRod=console.FromRodHandle(pAngle->pStart->hFatherPart);
		CLDSLinePart *pEndRod=console.FromRodHandle(pAngle->pEnd->hFatherPart);
		if( pStartRod==NULL||pEndRod==NULL||pStartRod==pAngle||pEndRod==pAngle||
			toupper(pStartRod->Layer(1))!='Z'||toupper(pEndRod->Layer(1))!='Z')
			continue;
		f3dPoint len_vec=pRod->pEnd->xPreliftPos-pRod->pStart->xPreliftPos;
		normalize(len_vec);
		f3dPoint near_norm;
		DESIGN_JGWING_NORM *pHorizWing=NULL,*pWorkWing=NULL;
		//double ddx=len_vec*f3dPoint(1, 0, 0), ddy=len_vec*f3dPoint(0, 1, 0);
		double ddx=len_vec.x, ddy=len_vec.y;
		if(fabs(ddx)<EPS_COS&&fabs(ddy)<EPS_COS)
			continue;
		//调整图层名为横材
		pAngle->layer(1)='H';
		{
			CLdsPartListStack stack(&Ta.Parts);
			//如未找到两侧（接近）水平的横担弦杆主材返回GEPOINT（0，0，-1)，即默认塔身坡面横材吐水 wjh-2019.6.2
			GEPOINT vNearWorkNormal=GetNearTrunkHorizAngleNormal(pAngle->pEnd);
			if(CorrectHorizAnglePos(pAngle, vNearWorkNormal))
				horizAngleSet.SetValue(pAngle->pStart->handle, pAngle->pEnd->handle, pAngle);
		}
	}
	//2.提取横隔面并调整横隔面杆件摆放方位
	ATOM_LIST<CFacePanel> templ_list;
	ReadStdFacePanels(2,templ_list);
	CHashSet<CLDSNode*> nodeSet;
	LINEPARTSET rodSet;
	CCorrectAngleLayout layout(&rodSet);
	for(CLDSLineAngle **ppAngle=horizAngleSet.GetFirst();ppAngle;ppAngle=horizAngleSet.GetNext())
	{
		CLDSLineAngle *pAngle=*ppAngle;
		if(pAngle->pStart==NULL||pAngle->pEnd==NULL)
			continue;
		pAngle->pStart->feature=pAngle->pEnd->feature=0;
		nodeSet.SetValue(pAngle->pStart->handle,pAngle->pStart);
		nodeSet.SetValue(pAngle->pEnd->handle,pAngle->pEnd);
		//避免后续修正代码将横材误当塔射斜材再次错误修正 wjh-2019.11.19
		layout.AddExcludeAngleLayout(pAngle->handle,LAYOUT_POS::LAYOUT_H);
	}
	for(CLDSNode* pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
	{	//以第二象限的节点为基准节点，根据对称点确定横隔面区域
		if(pNode->Position().x>0 || pNode->Position().y<0)	
			continue;
		if(pNode->feature==1)
			continue;
		CLDSNode* pMirX = pNode->GetMirNode(MIRMSG(1));
		CLDSNode* pMirY = pNode->GetMirNode(MIRMSG(2));
		CLDSNode* pMirZ = pNode->GetMirNode(MIRMSG(3));
		if(pMirX==NULL || pMirY==NULL || pMirZ==NULL)		//有三个对称的节点
			continue;
		if( (horizAngleSet.GetValue(pNode->handle,pMirX->handle)==NULL&&horizAngleSet.GetValue(pMirX->handle,pNode->handle)==NULL)||
			(horizAngleSet.GetValue(pNode->handle,pMirY->handle)==NULL&&horizAngleSet.GetValue(pMirY->handle,pNode->handle)==NULL)||
			(horizAngleSet.GetValue(pMirZ->handle,pMirX->handle)==NULL&&horizAngleSet.GetValue(pMirX->handle,pMirZ->handle)==NULL)||
			(horizAngleSet.GetValue(pMirZ->handle,pMirY->handle)==NULL&&horizAngleSet.GetValue(pMirY->handle,pMirZ->handle)==NULL))
			continue;	//对称节点间无连接杆件
		//水平横担连接处的横膈面

		pNode->feature=pMirX->feature=pMirY->feature=pMirZ->feature=1;
		POLYGON selRgn;
		f3dPoint pPoints[4]={pNode->xPreliftPos,pMirY->xPreliftPos,
			pMirZ->xPreliftPos,pMirX->xPreliftPos};
		if(!selRgn.CreatePolygonRgn(pPoints,4))
			continue;
		for(CLDSNode *pTaNode=Ta.Node.GetFirst();pTaNode;pTaNode=Ta.Node.GetNext())
		{
			int retcode=selRgn.PtInRgn2(pTaNode->xPreliftPos);
			if(retcode==1||retcode==2)//点在多边形域面内(包括边界在内)
				pTaNode->feature=1;
		}
		rodSet.Empty();
		for(CLDSLinePart *pRod=console.EnumRodFirst();pRod;pRod=console.EnumRodNext())
		{
			if( pRod->pStart==NULL||pRod->pEnd==NULL||!pRod->IsAngle())
				continue;	//非正常角钢
			CLDSLineAngle *pAngle=(CLDSLineAngle*)pRod;
			if (pAngle->blGroupSonAngle)
				continue;	//组合角钢的子角钢随组合角钢重置不单独处理肢朝向，否则会互相冲突 wjh-2019.11.19
			if(selRgn.LineInRgn2(pRod->pStart->xPreliftPos,pRod->pEnd->xPreliftPos)!=1) //线不在多边形域面内(包括边界在内)则继续
				continue;
			rodSet.append(pRod);
		}
		if(rodSet.GetNodeNum()<=4)
			continue;
		if(!layout.CorrectByFacePanel(templ_list,pNode))
			layout.Correct(TRUE);
		//特殊修正隔面中与主材节点连接的斜材位置
		for(int i=0;i<4;i++)
		{
			CLDSNode* pDesNode=(i==0)?pNode:pNode->GetMirNode(MIRMSG(i));
			CLDSLineAngle** ppHorizJg1=horizAngleSet.GetValue(pDesNode->handle,pDesNode->GetMirNode(MIRMSG(1))->handle);
			if(ppHorizJg1==NULL)
				ppHorizJg1=horizAngleSet.GetValue(pDesNode->GetMirNode(MIRMSG(1))->handle,pDesNode->handle);
			CLDSLineAngle** ppHorizJg2=horizAngleSet.GetValue(pDesNode->handle,pDesNode->GetMirNode(MIRMSG(2))->handle);
			if(ppHorizJg2==NULL)
				ppHorizJg2=horizAngleSet.GetValue(pDesNode->GetMirNode(MIRMSG(2))->handle,pDesNode->handle);
			if(ppHorizJg1==NULL || ppHorizJg2==NULL)
				continue;	//
			for(CLDSLinePart* pRod=rodSet.GetFirst();pRod;pRod=rodSet.GetNext())
			{
				if(pRod->GetClassTypeId()!=CLS_LINEANGLE||pRod->layer(1)=='H')
					continue;
				if(pRod->pStart!=pDesNode&&pRod->pEnd!=pDesNode)
					continue;
				CLDSLineAngle* pJg=(CLDSLineAngle*)pRod;
				CMultiOffsetPos *pDesPos=(pJg->pStart==pDesNode)?(&pJg->desStartPos):(&pJg->desEndPos);
				pDesPos->spatialOperationStyle=0;		//指定基准点
				pDesPos->datumPoint.datum_pos_style=3;	//角钢心线交点
				pDesPos->datumPoint.des_para.AXIS_INTERS.hDatum1=(*ppHorizJg1)->handle;
				pDesPos->datumPoint.des_para.AXIS_INTERS.hDatum2=(*ppHorizJg2)->handle;
				pDesPos->datumPoint.des_para.AXIS_INTERS.wing_offset_style1=0;
				pDesPos->datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;
			}
		}
	}
}
//修正塔身斜材角钢位置
void CCorrectModelAngleLayout::CorrectBodyDiagonalRodPos()
{
	//提取斜材(两端连接在主材上,其中一端可连接在横材中点上)
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if( pRod->pStart==NULL||pRod->pEnd==NULL||
			!pRod->IsAngle()||toupper(pRod->Layer(0))=='T'||
			toupper(pRod->Layer(1))=='H'||toupper(pRod->Layer(1))=='Z')	//跳过横材.主材
			continue;
		CLDSLineAngle *pAngle=(CLDSLineAngle*)pRod;
		if(pAngle->group_father_jg_h>0x20)
			continue;	//跳过组合角钢的子角钢
		CLDSLinePart *pStartRod=console.FromRodHandle(pAngle->pStart->hFatherPart);
		CLDSLinePart *pEndRod=console.FromRodHandle(pAngle->pEnd->hFatherPart);
		if( pStartRod==NULL||pEndRod==NULL||pStartRod==pAngle||pEndRod==pAngle)
			continue;
		char cStartRodLayer1=toupper(pStartRod->Layer(1));
		char cEndRodLayer1=toupper(pEndRod->Layer(1));
		if( (cStartRodLayer1!='Z'&&cEndRodLayer1!='Z')||		//始终端搭接杆件均不是主材
			(cStartRodLayer1!='Z'&&cStartRodLayer1!='H')||		//始端搭接杆件既不是主材也不是斜材
			(cEndRodLayer1!='Z'&&cEndRodLayer1!='H'))			//终端搭接杆件既不是主材也不是斜材
			continue;
		if(cStartRodLayer1=='H')
		{	//始端搭接角钢为横材，pAngle搭接在横材中点时为斜材
			f3dPoint middlePt=0.5*(pStartRod->pStart->Position()+pStartRod->pEnd->Position());
			if(DISTANCE(middlePt,pAngle->pStart->Position())>0)
				continue;	
		}
		if(cEndRodLayer1=='H')
		{	//终端搭接角钢为横材，pAngle搭接在横材中点时为斜材
			f3dPoint middlePt=0.5*(pEndRod->pStart->Position()+pEndRod->pEnd->Position());
			if(DISTANCE(middlePt,pAngle->pEnd->Position())>0)
				continue;
		}
		//调整图层名为斜材
		pAngle->layer(1)='X';
		CorrectDiagonalAnglePos(pAngle);
	}
}
//修正辅材角钢摆放位置
void CCorrectModelAngleLayout::CorrectAuxRodPosition()
{
	LINEPARTSET auxPoleSet;
	//1.提取身部辅材 
	for(CLDSLinePart* pRod=console.EnumRodFirst();pRod;pRod=console.EnumRodNext())
	{
		if( pRod->IsAngle()&&toupper(pRod->layer(1))=='F'&&
			(toupper(pRod->Layer(0))=='S'||toupper(pRod->Layer(0))=='L'||
			 toupper(pRod->Layer(0))=='T'))
			auxPoleSet.append(pRod);
	}
	CCorrectAngleLayout correct(&auxPoleSet);
	PROGRESS_CONTEXT context;
	context.title="重置塔身辅材杆件方位";
	correct.Correct(FALSE,&context);
}
//修正塔腿V面角钢摆放位置
extern CLDSNode* FindExtendNode(CLDSLinePart* pLinePart,BOOL bStart);
void CCorrectModelAngleLayout::CorrectVPanelRodPos()
{
	//腿部基础节点+相连的两V斜材延伸至横隔面的节点=提起归属腿部V面的构件集合
	UCS_STRU ucs;
	//PROGRESS_CONTEXT context;
	//context.title="修正辅材杆件方位";
	DisplayProcess(0,"重置腿部V面辅材方位");
	int i=0,n=Ta.Node.GetNodeNum();
	for(CLDSNode* pNode=Ta.EnumNodeFirst();pNode;pNode=Ta.EnumNodeNext(),i++)
	{
		DisplayProcess(100*i/n,"重置腿部V面辅材方位");
		//腿部基准节点
		if(pNode->IsBodyObj())			
			continue;	//节点位于塔腿
		CLDSLinePart* pFatherPart=(CLDSLinePart*)Ta.FromPartHandle(pNode->hFatherPart);
		if(pFatherPart==NULL||pFatherPart->IsBodyObj() || pFatherPart->Layer(1)!='Z')
			continue;	//节点父杆件属于腿部主材
		//获取与节点连接的两V斜材
		LINEPARTSET linePartSet;
		for(CLDSLinePart* pLinePart=Ta.EnumRodFirst();pLinePart;pLinePart=Ta.EnumRodNext())
		{
			if(pLinePart->IsBodyObj() || pLinePart->Layer(1)!='X')
				continue;	//杆件属于腿部斜材
			if(pLinePart->pStart!=pNode && pLinePart->pEnd!=pNode)
				continue;	//斜材与节点相连
			linePartSet.append(pLinePart);
		}
		if(linePartSet.GetNodeNum()!=2)
			continue;
		//获取两V斜材角钢延伸至横隔面的节点
		f3dPoint vec;
		ARRAY_LIST<CLDSNodePtr> nodeSet;
		CLDSLineAngle *pLeftAngle=NULL,*pRightAngle=NULL;
		ucs.axis_y.Set();
		for(CLDSLinePart* pRod=linePartSet.GetFirst();pRod;pRod=linePartSet.GetNext())
		{
			if(pRod->GetClassTypeId()!=CLS_LINEANGLE)
				continue;
			BOOL bStart=(pRod->pStart==pNode)?TRUE:FALSE;
			if(bStart)
				vec=(pRod->End()-pRod->Start()).normalized();
			else
				vec=(pRod->Start()-pRod->End()).normalized();
			if(ucs.axis_y.IsZero())
			{
				ucs.axis_y=vec;
				pLeftAngle=(CLDSLineAngle*)pRod;
			}
			else
			{
				ucs.axis_z=ucs.axis_y^vec;
				if(ucs.axis_z.z<0)
				{
					pRightAngle=(CLDSLineAngle*)pRod;
					ucs.axis_z*=-1;
				}
				else
				{
					pRightAngle=pLeftAngle;
					pLeftAngle=(CLDSLineAngle*)pRod;
				}
				ucs.axis_y+=vec;
			}
			CLDSNode* pVNode=FindExtendNode(pRod,bStart);
			if(pVNode)
				nodeSet.append(pVNode);
		}
		if(nodeSet.GetSize()!=2)
			continue;
		normalize(ucs.axis_z);
		normalize(ucs.axis_y);
		ucs.axis_x=ucs.axis_y^ucs.axis_z;
		normalize(ucs.axis_x);
		ucs.origin=pNode->xOriginalPos;
		//根据三个节点提取V面展开视图的构件集合
		DYN_ARRAY<f3dPoint> pt_arr(3);
		pt_arr[0]=pNode->xOriginalPos;
		pt_arr[1]=nodeSet[0]->xOriginalPos;
		pt_arr[2]=nodeSet[1]->xOriginalPos;
		POLYGON selRgn;
		if(!selRgn.CreatePolygonRgn(pt_arr,3))
			continue;
		int pushed=Ta.PushNodeStack();
		NODESET selNodeSet;
		PARTSET selPartSet;
		BOOL bTrue=InitDisplaySetByPolyRgn(selRgn,pNode,&selNodeSet,&selPartSet,true);
		Ta.PopNodeStack(pushed);
		if(!bTrue)
			continue;
		//调整腿部大斜材的摆放方式
		CorrectDiagonalAnglePos(pLeftAngle);
		CorrectDiagonalAnglePos(pRightAngle);
		//调整腿部V面角钢的摆放位置
		CPtInLineCheck left(pLeftAngle->pStart->Position(false),pLeftAngle->pEnd->Position(false));
		CPtInLineCheck right(pRightAngle->pStart->Position(false),pRightAngle->pEnd->Position(false));
		for(CLDSPart* pPart=selPartSet.GetFirst();pPart;pPart=selPartSet.GetNext())
		{
			if(pPart->GetClassTypeId()!=CLS_LINEANGLE)
				continue;
			CLDSLineAngle* pRayJg=(CLDSLineAngle*)pPart;
			if(pRayJg->pStart==NULL||pRayJg->pEnd==NULL)
				continue;
			if(pRayJg==pLeftAngle||pRayJg==pRightAngle)
				continue;	//Ｖ斜材杆件
			bool bInLine=false;
			int startOnLeft1Right2=0,endOnLeft1Right2=0;
			left.CheckPointEx(pRayJg->pStart->Position(false),&bInLine);
			if(bInLine)
				startOnLeft1Right2=1;
			right.CheckPointEx(pRayJg->pStart->Position(false),&bInLine);
			if(bInLine)
				startOnLeft1Right2|=2;
			left.CheckPointEx(pRayJg->pEnd->Position(false),&bInLine);
			if(bInLine)
				endOnLeft1Right2=1;
			right.CheckPointEx(pRayJg->pEnd->Position(false),&bInLine);
			if(bInLine)
				endOnLeft1Right2|=2;
			if(startOnLeft1Right2&0x01||endOnLeft1Right2&0x01)
			{
				BOOL bDesStart=(startOnLeft1Right2&0x01)?TRUE:FALSE;
				CLDSNode* pDesNode=bDesStart?pRayJg->pStart:pRayJg->pEnd;
				CMultiOffsetPos *pDesPos=bDesStart?(&pRayJg->desStartPos):(pDesPos=&pRayJg->desEndPos);
				if(pDesPos->IsInDatumJgWingX())
					pDesPos->spatialOperationStyle = 13;
				else //if(pJg->desStartPos.IsInDatumJgWingY())
					pDesPos->spatialOperationStyle = 14;
				if(pDesPos->jgber_cal_style==2&&pDesPos->IsInOffsetZhunLine())
					pDesPos->ciDestLineMode=CMultiOffsetPos::TO_OFFSETZHUN;
				if(pDesNode&&(pDesNode==nodeSet[0]||pDesNode==nodeSet[1]))
				{	//连接在V斜材端点的杆件需进行偏移
					pDesPos->datum_jg_h=pLeftAngle->handle;
					pDesPos->len_offset_dist=200;	//基点偏移量
				}
				pDesPos->huoqu_jg_h=pLeftAngle->handle;
				pDesPos->datum_offset_dist=0;	//
				pDesPos->huoqu_dist =10;
				pDesPos->offset_zhun=0;
			}
			if(startOnLeft1Right2&0x02||endOnLeft1Right2&0x02)
			{
				BOOL bDesStart=(startOnLeft1Right2&0x02)?TRUE:FALSE;
				CLDSNode* pDesNode=bDesStart?pRayJg->pStart:pRayJg->pEnd;
				CMultiOffsetPos *pDesPos=bDesStart?(&pRayJg->desStartPos):(&pRayJg->desEndPos);
				if(pDesPos->IsInDatumJgWingX())
					pDesPos->spatialOperationStyle = 13;
				else //if(pJg->desStartPos.IsInDatumJgWingY())
					pDesPos->spatialOperationStyle = 14;
				if(pDesPos->jgber_cal_style==2&&pDesPos->IsInOffsetZhunLine())
					pDesPos->ciDestLineMode=CMultiOffsetPos::TO_OFFSETZHUN;
				if(pDesNode&&(pDesNode==nodeSet[0]||pDesNode==nodeSet[1]))
				{	//连接在V斜材端点的杆件需进行偏移
					pDesPos->datum_jg_h=pRightAngle->handle;
					pDesPos->len_offset_dist=200;	//基点偏移量
				}
				pDesPos->huoqu_jg_h=pRightAngle->handle;
				pDesPos->datum_offset_dist=0;	//
				pDesPos->huoqu_dist =10;
				pDesPos->offset_zhun=0;
			}
		}
	}
	DisplayProcess(100,"重置腿部V面辅材方位");
}
