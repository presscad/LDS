#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "env_def.h"
#include "NewTowerDataOper.h"

BOOL CNewTowerDataOper::m_bSwitchMode=FALSE;	//是否切换模式
//
CNewTowerDataOper::CNewTowerDataOper(BYTE ciRunUiMode/*=1*/)
{
	m_ciRunUiMode=ciRunUiMode;
	m_ciMultiLegSchema=1;
	m_nPoly=4;
}
void CNewTowerDataOper::TransShankDefToFrameDef()
{
	m_xFrameDef.Empty();
	int nNum=m_xShankDef.SlopeNum();
	m_xFrameDef.m_xOpenArr.SetSize(0,nNum+1);
	CFrameDefine::OPEN_SIZE xSize;
	CShankLeg* pLeg=NULL;
	for(int i=0;i<nNum;i++)
	{
		CShankSlope* pSlope=m_xShankDef.SlopeAtIndex(i);
		xSize.m_fFW=pSlope->m_fTopFrontWidth;
		xSize.m_fSW=pSlope->m_fTopSideWidth;
		xSize.m_fVH=m_xShankDef.SlopeTopCoordZ(i);
		m_xFrameDef.m_xOpenArr.append(xSize);
		if(i==nNum-1 && (pLeg=pSlope->GetDatumLeg()))
		{	//最后一个坡段，提取接腿信息
			xSize.m_fFW=pLeg->m_fFrontWidth;
			xSize.m_fSW=pLeg->m_fSideWidth;
			xSize.m_fVH=m_xShankDef.SlopeBottomCoordZ(i,pLeg);
			m_xFrameDef.m_xOpenArr.append(xSize);
		}
	}
}
void CNewTowerDataOper::TransFrameDefToShankDef()
{
	m_xShankDef.Empty();
	int nNum=m_xFrameDef.m_xOpenArr.GetSize();
	if(nNum<3)
	{
		logerr.Log("数据太少，不做转换!");
		return;
	}
	CShankSlope* pPreSlope=NULL,*pSlope=NULL;
	for(int i=0;i<nNum;i++)
	{
		CFrameDefine::OPEN_SIZE xSize=m_xFrameDef.m_xOpenArr[i];
		if(i==nNum-1)
		{	//最后一段做为塔腿处理,接在前一个坡段下
			if(pPreSlope)
			{
				CShankModule* pModule=pPreSlope->AppendModule();
				CShankLeg* pLeg=pModule->AppendLeg();
				pLeg->m_fFrontWidth=xSize.m_fFW;
				pLeg->m_fSideWidth=xSize.m_fSW;
				pLeg->m_fHeight=xSize.m_fVH-m_xFrameDef.m_xOpenArr[i-1].m_fVH;
			}
		}
		else
		{
			pSlope=m_xShankDef.AppendSlope();
			pSlope->m_fTopFrontWidth=xSize.m_fFW;
			pSlope->m_fTopSideWidth=xSize.m_fSW;
			if(i==0)
				pSlope->m_fTopCoordZ=xSize.m_fVH;
			else if(pPreSlope)
			{	//相连两坡段通过添加Segment进行连接，接在前一个坡段下
				CShankSegment* pSeg=pPreSlope->AppendSegment();
				pSeg->m_fHeight=xSize.m_fVH-m_xFrameDef.m_xOpenArr[i-1].m_fVH;
			}
			pPreSlope=pSlope;
		}
	}
}
BOOL CNewTowerDataOper::RunNewTowerUi()
{
	int ret=-1;
	while(1)
	{
		if(OLD_UI==m_ciRunUiMode)
		{	//经典操作界面
			//初始化对话框数据
			CNewFileDlg  file_dlg;
			if(m_xFrameDef.m_xOpenArr.GetSize()>0)
			{
				if(m_nPoly==8)
					file_dlg.m_iSideNum=4;
				else if(m_nPoly>=3)
					file_dlg.m_iSideNum=m_nPoly-3;
				file_dlg.m_xFrameDef.CloneFrameDef(&m_xFrameDef);
			}
			//
			ret=file_dlg.DoModal();
			if(file_dlg.m_xFrameDef.GetSizeNum()>0)
			{	//提取对话框信息
				if(file_dlg.m_iSideNum<4)
					m_nPoly=3+file_dlg.m_iSideNum;
				else //if(file_dlg.m_iSideNum==4)
					m_nPoly=8;
				m_ciMultiLegSchema=1;	//默认8腿
				m_xFrameDef.CloneFrameDef(&file_dlg.m_xFrameDef);
			}
			if(ret==IDOK)
				break;
			if(m_bSwitchMode)
			{	//切换界面，更新数据
				m_bSwitchMode=FALSE;
				m_ciRunUiMode=NEW_UI;
				TransFrameDefToShankDef();
			}
			else
				return FALSE;
		}
		else if(NEW_UI==m_ciRunUiMode)
		{	//标准操作界面
			//初始化对话框数据
			CNewTowerDlg tower_dlg;
			if(m_xShankDef.SlopeNum()>0)
			{
				if(m_nPoly==8)
					tower_dlg.m_iSideNum=4;
				else if(m_nPoly>=3)
					tower_dlg.m_iSideNum=m_nPoly-3;
				tower_dlg.m_xShankDef.CloneShankDef(&m_xShankDef);
			}
			//
			ret=tower_dlg.DoModal();
			if(tower_dlg.m_xShankDef.SlopeNum()>0)
			{	//提取对话框数据
				if(tower_dlg.m_iSideNum<4)
					m_nPoly=3+tower_dlg.m_iSideNum;
				else //if(tower_dlg.m_iSideNum==4)
					m_nPoly=8;
				m_ciMultiLegSchema=tower_dlg.m_iMultiLegSchema+1;
				m_xFrameDef.m_iMainPartType=tower_dlg.m_iMainPartType;
				m_xShankDef.CloneShankDef(&tower_dlg.m_xShankDef);
			}
			if(ret==IDOK)
				break;
			if(m_bSwitchMode)
			{	//切换界面，更新数据
				m_bSwitchMode=FALSE;
				m_ciRunUiMode=OLD_UI;
				TransShankDefToFrameDef();
			}
			else
				return FALSE;
		}
	}
	if(ret==IDOK)
	{
		NewTowerModel();
		return TRUE;
	}
	return FALSE;
}
//根据CShankDefine数据生成塔型
bool CorrectSingleAngleLayout(CLDSLineAngle* pJg,bool updatePartSolid=true);
void CNewTowerDataOper::NewTowerModel()
{
	CFGLEG::SetSchema(m_ciMultiLegSchema);
	BOOL bPreferAngleSimplePosMode=g_sysPara.m_bAnglePreferSimplePosMode;
	g_sysPara.m_bAnglePreferSimplePosMode=FALSE;	//出于稳妥考虑新建塔身时暂时暂不启用简单定位模式
	CShankDefineToTowerModel modeler;
	if(m_ciRunUiMode==NEW_UI)
		modeler.UpdateTowerModel(&Ta,&m_xShankDef,m_nPoly);
	else if(m_ciRunUiMode==OLD_UI)
		modeler.BuildLegacyFrameTowerModel(&Ta,&m_xFrameDef,m_nPoly);
	g_sysPara.m_bAnglePreferSimplePosMode=bPreferAngleSimplePosMode;
	//应自动执行角钢摆放位置修正
	for(CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pAngle;pAngle=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
	{
		if(pAngle->layer(0)!='T'&&pAngle->layer(1)=='Z')
			continue;	//塔身或腿部主材不修正
		CorrectSingleAngleLayout(pAngle);
	}
}
