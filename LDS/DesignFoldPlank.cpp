//<LOCALE_TRANSLATE Confirm by hxr/>
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LDS.h"
#include "tower.h"
#include "stdafx.h"
#include "DesignJdb.h"
#include "JdbDlg.h"
#include "DesignJdb.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "dlg.h"
#include "PromptDlg.h"
#include "ScrTools.h"
#include "PlateBasicParaDlg.h"
#include "DefPlaneDlg.h"
#include "env_def.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

BOOL CDesignJdb::DesignFoldPlank(CLDSPlate *pPlank)
{
	CLDSBolt *pLs,*pMirBaseJgLs;
	f3dPoint ls_pos,mir_ls_pos;
	int i,j;
	short x_wing0_y_wing1;
	f3dPoint view_vec,base_line_vec,line_vec,cross_vec,inters,pick[2],vertex[2][4];
	f3dLine line;
	CJdbJgJointDlg oddment_dlg;

	m_pPlank = pPlank;
	m_pBaseNode=console.FromNodeHandle(m_pPlank->designInfo.m_hBaseNode);
	m_pPlank->designInfo.partList.Empty();
	m_pPlank->designInfo.additional_vertex.Empty();
	m_pPlank->designInfo.iFaceType=4;

	oddment_dlg.m_bEnable2RowsLs = FALSE;
	if(m_nViewFlag==LEFT_VIEW||m_nViewFlag==RIGHT_VIEW)
		view_vec.Set(0,-1,0);
	else //if(m_nViewFlag==FRONT_VIEW||m_nViewFlag==BACK_VIEW)
		view_vec.Set(1,0,0);

	CPlateBasicParaDlg share_dlg;
	share_dlg.m_bEnableWeld=FALSE;	//无焊接
	if(share_dlg.DoModal()!=IDOK)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("drop out halfway,design fail!");
#else
		AfxMessageBox("中途退出,设计失败!");
#endif
		return FALSE;
	}
	else
	{
		m_pPlank->Thick=share_dlg.m_nPlankThick;
		m_pPlank->cMaterial=CSteelMatLibrary::RecordAt(share_dlg.m_iMaterial).cBriefMark;
		m_pPlank->SetPartNo(share_dlg.m_sPartNo.GetBuffer(0));
	}
	//建立第一面(中间面)的相对坐标系
	CDefPlaneDlg planedlg;
	planedlg.m_fBaseX=m_pPlank->ucs.origin.x;
	planedlg.m_fBaseY=m_pPlank->ucs.origin.y;
	planedlg.m_fBaseZ=m_pPlank->ucs.origin.z;
	if(planedlg.DoModal()!=IDOK)
#ifdef AFX_TARG_ENU_ENGLISH
		throw "drop out halfway,design fail!";
#else
		throw "中途退出,设计失败!";
#endif
	m_pPlank->ucs.origin.Set(planedlg.m_fBaseX,planedlg.m_fBaseY,planedlg.m_fBaseZ);
	m_pPlank->ucs.axis_z.Set(planedlg.m_fNormX,planedlg.m_fNormY,planedlg.m_fNormZ);
	m_pPlank->designInfo.origin.SetPosition(m_pPlank->ucs.origin);
	m_pPlank->designInfo.origin.datum_pos_style=0;
	m_pPlank->designInfo.norm.vector=m_pPlank->ucs.axis_z;
	m_pPlank->designInfo.norm.norm_style=0;
	if(normalize(m_pPlank->ucs.axis_z)==0)
#ifdef AFX_TARG_ENU_ENGLISH
		throw "Normal value is illegal, design fail!";
	if(normalize(m_pPlank->ucs.axis_x)==0)
		throw "Two datum points coincide, design fail!";
#else
		throw "法线值不合法,设计失败!";
	if(normalize(m_pPlank->ucs.axis_x)==0)
		throw "两基准点重合,设计失败!";
#endif
	if(fabs(m_pPlank->ucs.axis_x*m_pPlank->ucs.axis_z)>EPS_COS)
		m_pPlank->ucs.axis_y=inters_vec(m_pPlank->ucs.axis_z);	//重新定位X轴向
	m_pPlank->ucs.axis_x=m_pPlank->ucs.axis_y^m_pPlank->ucs.axis_z;
	m_pPlank->ucs.axis_y=m_pPlank->ucs.axis_z^m_pPlank->ucs.axis_x;
	normalize(m_pPlank->ucs.axis_x);
	normalize(m_pPlank->ucs.axis_y);
	for(i=0;i<2;i++)
	{
		CLDSLineAngle *pBaseJg,*pMirBaseJg;
		if(i==0)
		{
			pBaseJg = face2_jgset.GetFirst();
			pMirBaseJg =face2_jgset.GetNext();
		}
		else
		{
			pBaseJg = face3_jgset.GetFirst();
			pMirBaseJg =face3_jgset.GetNext();
		}
		pBaseJg->SetModified();
		pMirBaseJg->SetModified();

		m_pPlank->designInfo.huoqufacenorm[i].norm_style=2;
		m_pPlank->designInfo.huoqufacenorm[i].hVicePart=pBaseJg->handle;
		m_pPlank->designInfo.huoqufacenorm[i].hCrossPart=pMirBaseJg->handle;
		if(m_nViewFlag==LEFT_VIEW||m_nViewFlag==RIGHT_VIEW)
			m_pPlank->designInfo.huoqufacenorm[i].nearVector.Set(0,1,0);
		else //if(m_nViewFlag==FRONT_VIEW||m_nViewFlag==BACK_VIEW)
			m_pPlank->designInfo.huoqufacenorm[i].nearVector.Set(-1,0,0);
		if(i>0)
			m_pPlank->designInfo.huoqufacenorm[i].nearVector*=-1.0;
		m_pPlank->designInfo.huoqufacenorm[i].UpdateVector(m_pPlank->BelongModel());
		m_pPlank->huoqufacenorm[i]=m_pPlank->designInfo.huoqufacenorm[i].vector;
		//建立火曲面法线及火曲线
		line.startPt = pBaseJg->Start();
		line.endPt	 = pBaseJg->End();
		line_vec = line.endPt-line.startPt;
		normalize(line_vec);
		if(line.startPt.z>line.endPt.z)
			line_vec*=-1.0;
		Int3dlf(inters,line,m_pPlank->ucs.origin,m_pPlank->ucs.axis_z);
		m_pPlank->HuoQuLine[i].startPt = inters;
		m_pPlank->designInfo.huoquline_start[i].SetPosition(inters);
		m_pPlank->designInfo.huoquline_start[i].datum_pos_style=0;
		if(m_nViewFlag==RIGHT_VIEW||m_nViewFlag==BACK_VIEW)
			inters.x*=-1.0;
		else
			inters.y*=-1.0;
		m_pPlank->HuoQuLine[i].endPt = inters;
		m_pPlank->designInfo.huoquline_end[i].SetPosition(inters);
		m_pPlank->designInfo.huoquline_end[i].datum_pos_style=0;
		coord_trans(m_pPlank->HuoQuLine[i].startPt,m_pPlank->ucs,FALSE);
		coord_trans(m_pPlank->HuoQuLine[i].endPt,m_pPlank->ucs,FALSE);
		m_pPlank->HuoQuLine[i].startPt.z = m_pPlank->HuoQuLine[i].endPt.z = 0;
		m_pPlank->HuoQuLine[i].startPt.feature = 10+i+2;
		m_pPlank->HuoQuLine[i].endPt.feature = 10+i+2;
		//计算正负头
		f3dPoint face_pick(m_pPlank->ucs.origin.x,m_pPlank->ucs.origin.y,m_pPlank->ucs.origin.z+g_sysPara.HuoQuDist);
		Int3dlf(inters,line,face_pick,m_pPlank->ucs.axis_z);
		f3dPoint min_z_wing_vec;
		int add_oddment=0;
		if(pBaseJg->GetWingVecX().z<pBaseJg->GetWingVecY().z)
			min_z_wing_vec = pBaseJg->GetWingVecX();
		else
			min_z_wing_vec = pBaseJg->GetWingVecY();
		add_oddment = (int)__max(0,-min_z_wing_vec.z*pBaseJg->GetWidth());
		if(pBaseJg->Start().z>pBaseJg->End().z)
		{
			f3dPoint odd_vec=inters-pBaseJg->End();
			oddment_dlg.m_fOddment = -(int)(odd_vec*line_vec)-add_oddment;
			pBaseJg->SetEndOdd(oddment_dlg.m_fOddment);
			pMirBaseJg->SetEndOdd(oddment_dlg.m_fOddment);
			oddment_dlg.m_nLsN = pBaseJg->connectEnd.wnConnBoltN;
			oddment_dlg.m_sLsGuiGe.Format("%d",pBaseJg->connectEnd.d);
		}
		else
		{
			f3dPoint odd_vec=inters-pBaseJg->Start();
			oddment_dlg.m_fOddment = -(int)(odd_vec*line_vec)-add_oddment;
			pBaseJg->SetStartOdd(oddment_dlg.m_fOddment);
			pMirBaseJg->SetStartOdd(oddment_dlg.m_fOddment);
			oddment_dlg.m_nLsN = pBaseJg->connectStart.wnConnBoltN;
			oddment_dlg.m_sLsGuiGe.Format("%d",pBaseJg->connectStart.d);
		}
		oddment_dlg.m_sRayJgGuiGe.Format("%.0fX%.0f",pBaseJg->GetWidth(),pBaseJg->GetThick());
		IsInsideJg(pBaseJg,&x_wing0_y_wing1);
		if(pBaseJg->m_bEnableTeG)
		{
			if(x_wing0_y_wing1==0)
				oddment_dlg.jgzj = pBaseJg->xWingXZhunJu;
			else
				oddment_dlg.jgzj = pBaseJg->xWingYZhunJu;
		}
		else
			getjgzj(oddment_dlg.jgzj,pBaseJg->GetWidth());
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pBaseJg->handle);
		if(oddment_dlg.DoModal()==IDOK)
		{
			int ls_d;
			sscanf(oddment_dlg.m_sLsGuiGe,"%d",&ls_d);
			if(pBaseJg->Start().z>pBaseJg->End().z)
			{
				pBaseJg->SetEndOdd(oddment_dlg.m_fOddment);
				pMirBaseJg->SetEndOdd(oddment_dlg.m_fOddment);
			}
			else
			{
				pBaseJg->SetStartOdd(oddment_dlg.m_fOddment);
				pMirBaseJg->SetStartOdd(oddment_dlg.m_fOddment);
			}
			f3dPoint jg_wing_vec;
			if(fabs(m_pPlank->huoqufacenorm[i]*pBaseJg->get_norm_x_wing())>fabs(m_pPlank->huoqufacenorm[i]*pBaseJg->get_norm_y_wing()))
				jg_wing_vec = pBaseJg->GetWingVecX();
			else
				jg_wing_vec = pBaseJg->GetWingVecY();
			for(j=0;j<oddment_dlg.m_nLsN;j++)
			{
				pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
				pLs->iSeg=pPlank->iSeg;
				pLs->set_d(ls_d);
				pLs->cfgword=pBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
				pLs->AddL0Thick(pPlank->handle,TRUE);
				pLs->AddL0Thick(pBaseJg->handle,TRUE);
				pLs->CalGuigeAuto();
				pLs->set_norm(m_pPlank->huoqufacenorm[i]);
				pLs->des_base_pos.datumPoint.datum_pos_style=1;
				pLs->des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
				pLs->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=4;
				pLs->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist=0;
				pLs->des_base_pos.hPart=pLs->des_base_pos.datumPoint.des_para.RODEND.hRod=pBaseJg->handle;
				pLs->des_base_pos.wing_offset_dist=oddment_dlg.jgzj.g;
				_snprintf(pLs->des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"-0X%X",pBaseJg->handle);
				pLs->des_work_norm.norm_style=1;
				if(fabs(m_pPlank->huoqufacenorm[i]*pBaseJg->get_norm_x_wing())>fabs(m_pPlank->huoqufacenorm[i]*pBaseJg->get_norm_y_wing()))
				{	//X肢上的螺栓
						//沿X肢进行螺栓位置偏移
					pLs->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pLs->des_base_pos.offset_wing=0;
					pLs->des_work_norm.hVicePart=pBaseJg->handle;
					pLs->des_work_norm.direction=0;		//朝外
					pLs->des_work_norm.norm_wing=0;
				}
				else
				{	//Y肢上的螺栓
					pLs->des_work_norm.hVicePart=pBaseJg->handle;
					pLs->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pLs->des_base_pos.offset_wing=1;	//沿Y肢进行螺栓位置偏移
					pLs->des_work_norm.direction=0;		//朝外
					pLs->des_work_norm.norm_wing=1;
				}
				if(pBaseJg->Start().z>pBaseJg->End().z)
					pLs->des_base_pos.datumPoint.des_para.RODEND.direction=pLs->des_base_pos.direction=1;
				else
					pLs->des_base_pos.datumPoint.des_para.RODEND.direction=pLs->des_base_pos.direction=0;
				if(oddment_dlg.m_iLsLayOutStyle==0)
					pLs->des_base_pos.len_offset_dist=oddment_dlg.m_nLsEndSpace+j*oddment_dlg.m_nLsSpace;
				else
					pLs->des_base_pos.len_offset_dist=oddment_dlg.m_nLsEndSpace+(j+0.5)*oddment_dlg.m_nLsSpace;
				pLs->correct_worknorm();
				pLs->correct_pos();
				CLsRef *pLsRef=m_pPlank->AppendLsRef(pLs->GetLsRef(),FALSE);
				pLsRef->dwRayNo=GetSingleWord(i*2);
				if(pBaseJg->Start().z>pBaseJg->End().z)
					pBaseJg->AppendEndLsRef(pLs->GetLsRef(),FALSE);
				else
					pBaseJg->AppendStartLsRef(pLs->GetLsRef(),FALSE);

				pMirBaseJgLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
				pMirBaseJgLs->iSeg=pPlank->iSeg;
				pMirBaseJgLs->CopyProperty(pLs);
				pMirBaseJgLs->EmptyL0DesignPara();
				pMirBaseJgLs->cfgword=pMirBaseJgLs->cfgword;	//调整螺栓配材号与基准构件配材号一致
				pMirBaseJgLs->AddL0Thick(pMirBaseJg->handle,TRUE);
				pMirBaseJgLs->AddL0Thick(m_pPlank->handle,TRUE);
				pMirBaseJgLs->CalGuigeAuto();
				pMirBaseJgLs->des_base_pos=pLs->des_base_pos;
				pMirBaseJgLs->des_work_norm=pLs->des_work_norm;
				pMirBaseJgLs->des_base_pos.hPart=pMirBaseJgLs->des_base_pos.datumPoint.des_para.RODEND.hRod=pMirBaseJg->handle;
				pMirBaseJgLs->des_base_pos.offset_wing=1-pMirBaseJgLs->des_base_pos.offset_wing;
				pMirBaseJgLs->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pMirBaseJgLs->des_base_pos.offset_wing;
				pMirBaseJgLs->des_work_norm.hVicePart=pMirBaseJg->handle;
				pMirBaseJgLs->des_work_norm.norm_wing=1-pMirBaseJgLs->des_work_norm.norm_wing;
				pLsRef=m_pPlank->AppendLsRef(pMirBaseJgLs->GetLsRef(),FALSE);
				pLsRef->dwRayNo=GetSingleWord(i*2+1);
				if(pBaseJg->Start().z>pBaseJg->End().z)
					pMirBaseJg->AppendEndLsRef(pMirBaseJgLs->GetLsRef(),FALSE);
				else
					pMirBaseJg->AppendStartLsRef(pMirBaseJgLs->GetLsRef(),FALSE);
				pMirBaseJgLs->correct_worknorm();
				pMirBaseJgLs->correct_pos();
			}
			SnapPerp(&vertex[i][0],line,pLs->ucs.origin);
			vertex[i][0] += line_vec*oddment_dlg.m_nLsEndSpace;
			vertex[i][1] = vertex[i][0]+jg_wing_vec*pBaseJg->GetWidth();
			if(m_nViewFlag==LEFT_VIEW||m_nViewFlag==RIGHT_VIEW)
			{
				vertex[i][2].Set(-vertex[i][1].x,vertex[i][1].y,vertex[i][1].z);
				vertex[i][3].Set(-vertex[i][0].x,vertex[i][0].y,vertex[i][0].z);
			}
			else //if(m_nViewFlag==FRONT_VIEW||m_nViewFlag==BACK_VIEW)
			{
				vertex[i][2].Set(vertex[i][1].x,-vertex[i][1].y,vertex[i][1].z);
				vertex[i][3].Set(vertex[i][0].x,-vertex[i][0].y,vertex[i][0].z);
			}
			for(j=0;j<4;j++)
			{
				coord_trans(vertex[i][j],m_pPlank->ucs,FALSE);
				vertex[i][j] = MutiFacePntToDatumFace(vertex[i][j],i+2);
				vertex[i][j].feature = i+2;
				vertex[i][j].z = 0;
			}
		}
		else
			return FALSE;
		g_pSolidDraw->SetEntSnapStatus(pBaseJg->handle,FALSE);
	}
	m_pPlank->vertex_list.append(vertex[0][1]);
	m_pPlank->vertex_list.append(vertex[0][0]);
	m_pPlank->vertex_list.append(m_pPlank->HuoQuLine[0].startPt);
	m_pPlank->vertex_list.append(m_pPlank->HuoQuLine[1].startPt);
	m_pPlank->vertex_list.append(vertex[1][0]);
	m_pPlank->vertex_list.append(vertex[1][1]);
	m_pPlank->vertex_list.append(vertex[1][2]);
	m_pPlank->vertex_list.append(vertex[1][3]);
	m_pPlank->vertex_list.append(m_pPlank->HuoQuLine[1].endPt);
	m_pPlank->vertex_list.append(m_pPlank->HuoQuLine[0].endPt);
	m_pPlank->vertex_list.append(vertex[0][3]);
	m_pPlank->vertex_list.append(vertex[0][2]);

	g_pSolidDraw->ReleaseSnapStatus();

	return TRUE;
}
#endif