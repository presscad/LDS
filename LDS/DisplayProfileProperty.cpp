//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "LmaDlg.h"
#include "RelativeObjectDlg.h"
#include "dlg.h"
#include "PropertyListOper.h"

BOOL ModifyProfileVertexItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	PROFILE_VERTEX::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}
//
BOOL ReCalVertexPostiom(PROFILE_VERTEX* pVertex,CLDSGeneralPlate* pBelogPlate)
{
	if(pBelogPlate->GetClassTypeId()!=CLS_PLATE)
		return FALSE;
	CLDSPlate* pPlate=(CLDSPlate*)pBelogPlate;
	CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.FromPartHandle(pPlate->designInfo.m_hBasePart,CLS_LINEANGLE);
	if(pPlate->wiType==CLDSGeneralPlate::WASHER_PLATE&&pAngle&&pPlate->GetLsCount()>1)
	{	//���¼�����ѡ���������ƫ��ֵ
		if(fabs(pVertex->xOffset.x)<=0&&fabs(pVertex->xOffset.y)<=0)
			return FALSE;
		SCOPE_STRU scope;
		f3dPoint ls_pos,pt;
		for(CLsRef* pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
		{
			pt=ls_pos=(*pLsRef)->ucs.origin;
			pPlate->SpreadLsPos(pLsRef,ls_pos);
			scope.VerifyVertex(ls_pos);
		}
		BYTE start0_end1=DISTANCE(pt,pAngle->Start())<DISTANCE(pt,pAngle->End())?0:1;
		char ciWorkWing=fabs(pAngle->vxWingNorm*pPlate->ucs.axis_z)>fabs(pAngle->vyWingNorm*pPlate->ucs.axis_z)?'X':'Y';
		GEPOINT vWingVec=ciWorkWing=='X'?pAngle->GetWingVecX():pAngle->GetWingVecY();
		GEPOINT vLenVec=start0_end1==0?(pAngle->End()-pAngle->Start()):(pAngle->Start()-pAngle->End());
		if(pPlate->cDatumAxis=='Y')
		{
			int signLenY =vLenVec *pPlate->ucs.axis_y<0?-1:1;
			int signWingX=vWingVec*pPlate->ucs.axis_x<0?-1:1;
			if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER||pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER2)
			{
				if((signLenY>0&&pVertex->m_cPosDesignType==1)||(signLenY<0&&pVertex->m_cPosDesignType==3))
					pVertex->xOffset.y*=-1;
				else
					pVertex->xOffset.y*=1;
				if(signWingX>0)
					pVertex->xOffset.x*=1;
				else
					pVertex->xOffset.x*=-1;
			}
			else if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING||pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING2)
			{
				if((signLenY>0&&pVertex->m_cPosDesignType==2)||(signLenY<0&&pVertex->m_cPosDesignType==4))
					pVertex->xOffset.y*=-1;
				else
					pVertex->xOffset.y*=1;
				if(signWingX>0)
					pVertex->xOffset.x*=-1;
				else
					pVertex->xOffset.x*=1;
			}
		}
		else
		{
			int signLenX =vLenVec *pPlate->ucs.axis_x<0?-1:1;
			int signWingY=vWingVec*pPlate->ucs.axis_y<0?-1:1;
			if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER||pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER2)
			{
				if((signLenX>0&&pVertex->m_cPosDesignType==1)||(signLenX<0&&pVertex->m_cPosDesignType==3))
					pVertex->xOffset.x*=-1;
				else
					pVertex->xOffset.x*=1;
				if(signWingY>0)
					pVertex->xOffset.y*=1;
				else
					pVertex->xOffset.y*=-1;
			}
			else if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING||pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING2)
			{
				if((signLenX>0&&pVertex->m_cPosDesignType==2)||(signLenX<0&&pVertex->m_cPosDesignType==4))
					pVertex->xOffset.x*=-1;
				else
					pVertex->xOffset.x*=1;
				if(signWingY>0)
					pVertex->xOffset.y*=-1;
				else
					pVertex->xOffset.y*=1;
			}
		}
		return TRUE;
	}
	CDesignLjPartPara* pDesignLjPartPara=pPlate->GetLjRodPara(pVertex->m_hRod);
	if(pDesignLjPartPara==NULL)
		return FALSE;
	CLDSLinePart* pRod=(CLDSLinePart*)Ta.FromPartHandle(pVertex->m_hRod);
	if(pRod==NULL)
		return FALSE;
	CSuperSmartPtr<CLDSLinePart> pCurLinePart=pRod;
	f3dPoint base_pos,vice_vec;
	if(pDesignLjPartPara->start0_end1!=1)	//ʼ�����ӻ��м�����
	{
		base_pos=pRod->Start();
		vice_vec=pRod->End()-pRod->Start();
	}
	else
	{	
		base_pos=pRod->End();
		vice_vec=pRod->Start()-pRod->End();
	}
	normalize(vice_vec);
	if(pRod->GetClassTypeId()==CLS_LINEANGLE || pRod->GetClassTypeId()==CLS_LINEFLAT)
	{	//���������˼��ǽǸ�|����
		f3dPoint wing_vec,norm,ls_pos,ls_offset_vec;
		f3dLine ber_line;
		if(pRod->IsAngle())
		{
			ber_line.startPt=pRod->Start();
			ber_line.endPt=pRod->End();
			if(pDesignLjPartPara->angle.cur_wing_x0_y1==0)
			{
				wing_vec=pCurLinePart.LineAnglePointer()->GetWingVecX();
				norm=pCurLinePart.LineAnglePointer()->get_norm_x_wing();
			}
			else
			{
				wing_vec=pCurLinePart.LineAnglePointer()->GetWingVecY();
				norm=pCurLinePart.LineAnglePointer()->get_norm_y_wing();
			}
		}
		else
		{
			wing_vec=pCurLinePart.LineFlatPointer()->ucs.axis_y;
			norm=pCurLinePart.LineFlatPointer()->WorkPlaneNorm();
			base_pos-=wing_vec*0.5*pCurLinePart->GetWidth();
			ber_line.startPt-=wing_vec*0.5*pCurLinePart->GetWidth();
			ber_line.endPt-=wing_vec*0.5*pCurLinePart->GetWidth();
		}
		//������˨������ر߾�
		double max_ber_dist=0,min_ber_dist=pCurLinePart->GetWidth(),head_len=0,tail_len=0;;
		for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			if(pBolt&&pCurLinePart->FindLsByHandle(pBolt->handle))
			{
				double dist=0;
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				ls_pos=pBolt->ucs.origin;
				project_point(ls_pos,base_pos,norm);
				SnapPerp(&ls_pos,ber_line,ls_pos,&dist);
				//֫��ƫ�ƾ�
				if(max_ber_dist==0)
					max_ber_dist=min_ber_dist=dist;
				else if(dist>max_ber_dist)
					max_ber_dist=dist;
				else if(dist<min_ber_dist)
					min_ber_dist=dist;
				//�˼�����ƫ�ƾ�
				ls_offset_vec = ls_pos-base_pos;
				if(ls_offset_vec*vice_vec>0)
					dist=ls_offset_vec.mod();
				else
					dist=-ls_offset_vec.mod();
				if(head_len==0)
					head_len=tail_len=dist;
				if(dist>tail_len)
					tail_len=dist;
				if(dist<head_len)
					head_len=dist;
			}
		}
		//�������ζ�������ͳһ���ն������ν��м���
		int face_i=pDesignLjPartPara->iFaceNo;
		double wing_dist=0,ber_dist=0,tail_edge_dist=0,head_edge_dist=0;
		head_edge_dist=head_len-pDesignLjPartPara->end_space;
		tail_edge_dist=tail_len+pDesignLjPartPara->end_space;
		ber_dist=min_ber_dist-pDesignLjPartPara->ber_space;
		wing_dist=max_ber_dist+pDesignLjPartPara->wing_space;
		f3dPoint vv,vertex;
		if(pVertex->m_cPosDesignType==1 || pVertex->m_cPosDesignType==2)
		{
			vv=base_pos+vice_vec*tail_edge_dist;
			if(pVertex->m_cPosDesignType==1)	//���߲ඥ��
				vertex=vv+wing_vec*ber_dist;
			else								//֫��ඥ��
				vertex=vv+wing_vec*wing_dist;
			coord_trans(vertex,pPlate->ucs,FALSE);
			vertex=pPlate->GetSpreadVertex(vertex,face_i);
		}
		if(pVertex->m_cPosDesignType==3 || pVertex->m_cPosDesignType==4)
		{
			vv=base_pos+vice_vec*head_edge_dist;
			if(pVertex->m_cPosDesignType==3)	//����������һ���߲ඥ��
				vertex=vv+wing_vec*ber_dist;
			else								//����������һ֫��ඥ��
				vertex=vv+wing_vec*wing_dist;
			coord_trans(vertex,pPlate->ucs,FALSE);
			vertex=pPlate->GetSpreadVertex(vertex,face_i);
		}
		pVertex->vertex.Set(vertex.x,vertex.y,0);
	}
	else if(pRod->GetClassTypeId()==CLS_LINETUBE)
	{	//���������˼��Ǹֹ�
		
	}
	return TRUE;
}

BOOL ModifyProfileLineProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	SUB_OBJ* pSelObj=pView->selectSubObjs.GetFirst();
	PROFILE_VERTEX* pFirstVertex=(PROFILE_VERTEX*)pSelObj->pSubObj;
	CLDSGeneralPlate* pBelongPlate=(CLDSGeneralPlate*)Ta.Parts.FromHandle(pSelObj->hObj,CLS_PLATE,CLS_PARAMPLATE);
	if(pFirstVertex==NULL)
		return FALSE;
	CPropListOper<PROFILE_VERTEX,CTmaPtrList<SUB_OBJ,SUB_OBJ*> > oper(pPropList,pFirstVertex);	
	CXhChar200 sText;
	CPropTreeItem* pFindItem=NULL,*pSonItem=NULL;
	if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("m_bRollEdge"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else 
		if(valueStr.CompareNoCase("��")==0)
#endif
		{
			pFirstVertex->m_bRollEdge=true;
			if(pBelongPlate->GetClassTypeId()==CLS_PLATE)
				((CLDSPlate*)pBelongPlate)->designInfo.iProfileStyle0123=3;	//���֮�����μ��㷽ʽ�޸�Ϊ����������
			oper.InsertEditPropItem(pItem,"manu_space1","","",-1,TRUE);
			oper.InsertEditPropItem(pItem,"roll_edge_offset_dist","","",-1,TRUE);
			//
			pFirstVertex->m_bWeldEdge=false;
			CPropTreeItem* pFindItem = pPropList->FindItemByPropId(PROFILE_VERTEX::GetPropID("m_bWeldEdge"),NULL);
			if(pFindItem)
			{
				pPropList->SetItemPropValue(pFindItem->m_idProp,"��");
				pPropList->DeleteAllSonItems(pFindItem);
			}
		}
		else 
		{
			pFirstVertex->m_bRollEdge=false;
			if(pBelongPlate->GetClassTypeId()==CLS_PLATE)
				((CLDSPlate*)pBelongPlate)->designInfo.iProfileStyle0123=1;	//ȡ����ߣ����μ��㷽ʽ�޸�Ϊ��������
			pPropList->DeleteAllSonItems(pItem);
			if(pFirstVertex->m_bWeldEdge)
			{
				CPropTreeItem* pFindItem=pPropList->FindItemByPropId(PROFILE_VERTEX::GetPropID("m_bWeldEdge"),NULL);
				oper.InsertEditPropItem(pFindItem,"manu_space","","",-1,TRUE);
				pSonItem=oper.InsertCmbListPropItem(pFindItem,"local_point_vec","","","",-1,TRUE);
				if(pFirstVertex->local_point_vec!=0)
					oper.InsertEditPropItem(pSonItem,"local_point_y","","",-1,TRUE);
			}
		}
	}
	else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("manu_space1"))	//��߸߶�
		pFirstVertex->manu_space=(short)atof(valueStr);
	else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("roll_edge_offset_dist"))
		pFirstVertex->roll_edge_offset_dist=(short)atof(valueStr);
	else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("m_bWeldEdge"))
	{
#ifdef  AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
#endif
		{
			pFirstVertex->m_bRollEdge=false;
			CPropTreeItem* pFindItem = pPropList->FindItemByPropId(PROFILE_VERTEX::GetPropID("m_bRollEdge"),NULL);
			if(pFindItem)
			{
				pPropList->DeleteAllSonItems(pFindItem);
				pPropList->SetItemPropValue(pFindItem->m_idProp,"��");
			}
			//
			pFirstVertex->m_bWeldEdge=true;
			oper.InsertEditPropItem(pItem,"manu_space","","",-1,TRUE);
			pSonItem=oper.InsertCmbListPropItem(pItem,"local_point_vec","","","",-1,TRUE);
			if(pFirstVertex->local_point_vec!=0)
				oper.InsertEditPropItem(pSonItem,"local_point_y","","",-1,TRUE);
		}
		else
		{
			pFirstVertex->m_bWeldEdge=false;
			pPropList->DeleteAllSonItems(pItem);
			if(pFirstVertex->m_bRollEdge)
			{
				CPropTreeItem* pFindItem=pPropList->FindItemByPropId(PROFILE_VERTEX::GetPropID("m_bRollEdge"),NULL);
				oper.InsertEditPropItem(pFindItem,"manu_space1","","",-1,TRUE);
				oper.InsertEditPropItem(pFindItem,"roll_edge_offset_dist","","",-1,TRUE);
			}
		}
	}
	else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("manu_space"))
		pFirstVertex->manu_space=(short)atof(valueStr);
	else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("local_point_vec"))
	{
		pFirstVertex->local_point_vec=valueStr[0]-'0';
		if(pFirstVertex->local_point_vec==0)
			pPropList->DeleteAllSonItems(pItem);
		else
			oper.InsertEditPropItem(pItem,"local_point_y","","",-1,TRUE);
	}
	else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("local_point_y"))
		pFirstVertex->local_point_y=atof(valueStr);
	//����ʵ��
	pBelongPlate->SetModified();
	pBelongPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
	g_pSolidDraw->NewSolidPart(pBelongPlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return TRUE;
}

void UpdateVertexDesignItem(PROFILE_VERTEX* pVertex,CPropertyList* pPropList,CPropTreeItem *pLeaderItem);
void UpdateSelSubObj(SUB_OBJ* pSelObj,CLDSGeneralPlate* pBelongPlate)
{
	if(pSelObj==NULL || pBelongPlate==NULL)
		return;
	PROFILE_VERTEX* pVertex=NULL;
	for(pVertex=pBelongPlate->vertex_list.GetFirst();pVertex;pVertex=pBelongPlate->vertex_list.GetNext())
	{
		if(pVertex->vertex.ID==pSelObj->iSubId)
			break;
	}
	if(pVertex)
	{
		pVertex->SetBelongPlate(pBelongPlate,&console.m_curWCS);
		pSelObj->pSubObj=pVertex;
		//����������Ϊѡ����
		HIBERID hiberid(pBelongPlate->handle,HIBERARCHY(0,0,0,0));
		hiberid.SetHiberarchy(1,0,pVertex->vertex.ID,0);
		IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
		if(pBuddyDrawing==NULL)
			return;
		for(IDbEntity* pEnt=pBuddyDrawing->EnumFirstDbEntity();pEnt;pEnt=pBuddyDrawing->EnumNextDbEntity())
		{
			HIBERID ent_hiberid=pEnt->GetHiberId();
			if(ent_hiberid.IsEqual(hiberid))
			{
				pEnt->SetStateFlag(IDbEntity::SELECTED);
				break;
			}
		}
	}
}
struct PROFILE_VERTEX_INFO{
	PROFILE_VERTEX* pVertex;
	CLDSGeneralPlate* pBelongPlate;
	CDesignLjPartPara *pLjPartPara;
	CLDSGeneralPlate::GASKETEDGESPACE *pBoltPadSpace;
	PROFILE_VERTEX_INFO(SUB_OBJ* pSelObj=NULL)
	{
		pLjPartPara=NULL;
		pBoltPadSpace=NULL;
		if(pSelObj==NULL)
		{
			pVertex=NULL;
			pBelongPlate=NULL;
			pBoltPadSpace=NULL;
			pLjPartPara=NULL;
			return;
		}
		pVertex=(PROFILE_VERTEX*)pSelObj->pSubObj;
		pBelongPlate=(CLDSGeneralPlate*)Ta.Parts.FromHandle(pSelObj->hObj,CLS_PLATE,CLS_PARAMPLATE);;
		if(pBelongPlate&&pBelongPlate->GetClassTypeId()==CLS_PLATE)
		{
			pLjPartPara=((CLDSPlate*)pBelongPlate)->GetLjRodPara(pVertex->m_hRod);
			if(pBelongPlate->wiType==CLDSGeneralPlate::WASHER_PLATE)
				pBoltPadSpace=&pBelongPlate->gasketEdgeSpace;
		}
	}
};
BOOL ModifyProfileVertexProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	SUB_OBJ* pSelObj=pView->selectSubObjs.GetFirst();
	PROFILE_VERTEX* pFirstVertex=(PROFILE_VERTEX*)pSelObj->pSubObj;
	CLDSGeneralPlate* pBelongPlate=(CLDSGeneralPlate*)Ta.Parts.FromHandle(pSelObj->hObj,CLS_PLATE,CLS_PARAMPLATE);
	if(pFirstVertex==NULL||pBelongPlate==NULL)
		return FALSE;
	CPropListOper<PROFILE_VERTEX,SUBOBJSET>oper(pPropList,pFirstVertex);
	CDesignLjPartPara *pLjPartPara=NULL;
	if(pBelongPlate->GetClassTypeId()==CLS_PLATE)
		pLjPartPara=((CLDSPlate*)pBelongPlate)->GetLjRodPara(pFirstVertex->m_hRod);
	int i=0;
	DYN_ARRAY<PROFILE_VERTEX_INFO> selectVertexes(pView->selectSubObjs.Count);
	for(pSelObj=pView->selectSubObjs.GetFirst();pSelObj;pSelObj=pView->selectSubObjs.GetNext(),i++)
		selectVertexes[i]=PROFILE_VERTEX_INFO(pSelObj);
	BOOL bReCalVertexPos=FALSE;
	try
	{
		if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("cVertexType"))
		{
			if(valueStr[0]=='0')	//��׼���
				pFirstVertex->vertex.feature=1;	
			else if(valueStr[0]=='1')	//�ڶ��������
				pFirstVertex->vertex.feature=2;
			else if(valueStr[0]=='2')	//1-2�������
				pFirstVertex->vertex.feature=12;
			else if(valueStr[0]=='3')	//�����������
				pFirstVertex->vertex.feature=3;
			else if(valueStr[0]=='4')	//1-3�������
				pFirstVertex->vertex.feature=13;
			for(SUB_OBJ* pObj=pView->selectSubObjs.GetFirst();pObj;pObj=pView->selectSubObjs.GetNext())
			{
				PROFILE_VERTEX* pVertex=(PROFILE_VERTEX*)pObj->pSubObj;
				pVertex->vertex.feature=pFirstVertex->vertex.feature;
			}
		}
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("m_hSubjectPlate"))
			sscanf(valueStr,"%X",pBelongPlate->handle);
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("m_sSubjectPlatePartNo"))
			pBelongPlate->SetPartNo(valueStr);
		else if (pItem->m_idProp==PROFILE_VERTEX::GetPropID("m_cSubjectPlateMaterial"))
			pBelongPlate->cMaterial=QuerySteelBriefMatMark(valueStr.GetBuffer());
		else if (pItem->m_idProp==PROFILE_VERTEX::GetPropID("m_fSubjectPlateThick"))
			pBelongPlate->Thick=atof(valueStr);
		else if (pItem->m_idProp==PROFILE_VERTEX::GetPropID("m_relaPlate"))
		{
			CRelativeObjectDlg relativeDlg;
			relativeDlg.m_pWorkObj = pBelongPlate;
			if(relativeDlg.DoModal()==IDOK)
			{
				CXhChar200 sText;
				pFirstVertex->GetPropValueStr(pItem->m_idProp,sText);
				pPropList->SetItemPropValue(pItem->m_idProp,sText);
			}
		}
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("m_cPickedDisplayMode"))
		{
			if(pBelongPlate&&valueStr.Compare("ʵ��������")==0)		//ʵ��ģʽ��ʾ�ְ�������
				pBelongPlate->m_cPickedDisplayMode=1;	
			else if(pBelongPlate&&valueStr.Compare("չ��������")==0)		//ʵ��ģʽ��ʾ�ְ�������
				pBelongPlate->m_cPickedDisplayMode=2;	
			else if(pBelongPlate&&valueStr.Compare("����ʾ")==0)//ʵ��ģʽ����ʾ�ְ�������
				pBelongPlate->m_cPickedDisplayMode=FALSE;
			if(pBelongPlate)
			{
				IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
				DISPLAY_TYPE eDispType=DISP_LINE;
				g_pSolidSet->GetDisplayType(&eDispType);
				if(pBelongPlate->m_cPickedDisplayMode&&eDispType==DISP_SOLID)
					CLDSView::RedrawPlateProfileDrawing(pBelongPlate,NULL,true,0x03);
				else if(pBuddyDrawing)
					pBuddyDrawing->DeleteDbEntityByHiberMasterId(pBelongPlate->handle);
				if(eDispType==DISP_SOLID&&!pBelongPlate->m_cPickedDisplayMode)
					g_pSolidDraw->NewSolidPart(pBelongPlate->GetSolidPartObject());
				g_pSolidDraw->Draw();
				return TRUE;
			}
		}
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("cPosDesignType"))
		{
			CXhChar16 value_str("%s",valueStr);
			value_str.Replace('.','\0');
			pFirstVertex->m_cPosDesignType=atoi(value_str);//�ѳ���10,�ʲ�����valueStr[0]-'0';
			for(SUB_OBJ* pObj=pView->selectSubObjs.GetFirst();pObj;pObj=pView->selectSubObjs.GetNext())
			{
				PROFILE_VERTEX* pVertex=(PROFILE_VERTEX*)pObj->pSubObj;
				pVertex->m_cPosDesignType=pFirstVertex->m_cPosDesignType;
			}
			//����������
			UpdateVertexDesignItem(pFirstVertex,pPropList,pItem);
		}
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("hRod"))
		{
			CXhChar16 value_str("%s",valueStr);
			value_str.Replace('.','\0');
			pFirstVertex->m_hRod=HexStrToLong(value_str);//�ѳ���10,�ʲ�����valueStr[0]-'0';
			for(SUB_OBJ* pObj=pView->selectSubObjs.GetFirst();pObj;pObj=pView->selectSubObjs.GetNext())
			{
				PROFILE_VERTEX* pVertex=(PROFILE_VERTEX*)pObj->pSubObj;
				pVertex->m_hRod=pFirstVertex->m_hRod;
			}
		}
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("start0_end1"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(pBelongPlate&&valueStr.Compare("Start")==0)	
				pLjPartPara->start0_end1=1;
			else if(pBelongPlate&&valueStr.Compare("End")==0)	
				pLjPartPara->start0_end1=0;
			else  if(pBelongPlate&&valueStr.Compare("Middle")==0)
#else 
			if(pBelongPlate&&valueStr.Compare("�ն�")==0)	
				pLjPartPara->start0_end1=1;
			else if(pBelongPlate&&valueStr.Compare("ʼ��")==0)	
				pLjPartPara->start0_end1=0;
			else  if(pBelongPlate&&valueStr.Compare("�м�")==0)
#endif
				pLjPartPara->start0_end1=2;		//��ʾ�м����˼
		}
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("wing_x0_y1"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(pBelongPlate&&valueStr.Compare("X Leg")==0)	
				pLjPartPara->angle.cur_wing_x0_y1=0;
			else //if(pBelongPlate&&valueStr.Compare("Y Leg")==0)	
				pLjPartPara->angle.cur_wing_x0_y1=1;
#else
			if(pBelongPlate&&valueStr.Compare("X֫")==0)	
				pLjPartPara->angle.cur_wing_x0_y1=0;
			else //if(pBelongPlate&&valueStr.Compare("Y֫")==0)	
				pLjPartPara->angle.cur_wing_x0_y1=1;
#endif
		}
		else if(pLjPartPara && pItem->m_idProp==PROFILE_VERTEX::GetPropID("wing_space_style"))
		{
			bReCalVertexPos=TRUE;
			BYTE cbSpaceType=valueStr[0]-'0';
			if(cbSpaceType>0)	//��ָ���߾�ʱ,Ĭ������ȡֵ��ͬ
				pLjPartPara->ciAsynDualEdgeWingSpace=0;
			pLjPartPara->angle.cbSpaceFlag.SetWingSpaceStyle(cbSpaceType);
			((CLDSPlate*)pBelongPlate)->UpdateLjRodEdgeSpace(pFirstVertex->m_hRod,0x02);
			//
			pPropList->DeleteAllSonItems(pItem);
			if(pLjPartPara->angle.bTwoEdge&&pLjPartPara->angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_UDF)
				oper.InsertCmbListPropItem(pItem,"wing_space_syncmode","","","",-1,TRUE);
			CPropTreeItem* pSonItem=oper.InsertEditPropItem(pItem,"wing_space","","",-1,TRUE);
			pSonItem->SetReadOnly(pLjPartPara->angle.cbSpaceFlag.StyleOfWingSpace()>0);
		}
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("wing_space"))
		{
			bReCalVertexPos=TRUE;
			for(UINT i=0;i<selectVertexes.Size();i++)
			{
				if(selectVertexes[i].pLjPartPara)
				{
					selectVertexes[i].pLjPartPara->angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_UDF);
					if(pFirstVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING2)
						selectVertexes[i].pLjPartPara->startSideWingSpace=atof(valueStr);
					else
						selectVertexes[i].pLjPartPara->endSideWingSpace=atof(valueStr);
				}
				else if(selectVertexes[i].pBoltPadSpace)
					selectVertexes[i].pBoltPadSpace->wingSpace=atoi(valueStr);
			}
		}
		else if(pLjPartPara && pItem->m_idProp==PROFILE_VERTEX::GetPropID("ber_space_style"))
		{
			bReCalVertexPos=TRUE;
			BYTE cbSpaceType=valueStr[0]-'0';
			if(cbSpaceType>0)	//��ָ���߾�ʱ,Ĭ������ȡֵ��ͬ
				pLjPartPara->ciAsynDualEdgeBerSpace=0;
			pLjPartPara->angle.cbSpaceFlag.SetBerSpaceStyle(cbSpaceType);
			((CLDSPlate*)pBelongPlate)->UpdateLjRodEdgeSpace(pFirstVertex->m_hRod,0x01);
			//
			pPropList->DeleteAllSonItems(pItem);
			if(pLjPartPara->angle.bTwoEdge&&pLjPartPara->angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_UDF)
				oper.InsertCmbListPropItem(pItem,"ber_space_syncmode","","","",-1,TRUE);
			CPropTreeItem* pSonItem=oper.InsertEditPropItem(pItem,"ber_space","","",-1,TRUE);
			pSonItem->SetReadOnly(pLjPartPara->angle.cbSpaceFlag.StyleOfBerSpace()>0);
		}
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("ber_space"))
		{
			bReCalVertexPos=TRUE;
			for(UINT i=0;i<selectVertexes.Size();i++)
			{
				if(selectVertexes[i].pLjPartPara)
				{
					selectVertexes[i].pLjPartPara->angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_UDF);
					if(pFirstVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER2)
						selectVertexes[i].pLjPartPara->startSideBerSpace=atof(valueStr);
					else
						selectVertexes[i].pLjPartPara->endSideBerSpace=atof(valueStr);
				}
				else if(selectVertexes[i].pBoltPadSpace)
					selectVertexes[i].pBoltPadSpace->berSpace=atoi(valueStr);
			}
		}
		else if(pLjPartPara && pItem->m_idProp==PROFILE_VERTEX::GetPropID("end_space_style"))
		{
			bReCalVertexPos=TRUE;
			BYTE cbSpaceType=valueStr[0]-'0';
			if(cbSpaceType>0)	//��ָ���߾�ʱ,Ĭ������ȡֵ��ͬ
				pLjPartPara->ciAsynDualEdgeEndSpace=0;
			pLjPartPara->angle.cbSpaceFlag.SetEndSpaceStyle(cbSpaceType);
			((CLDSPlate*)pBelongPlate)->UpdateLjRodEdgeSpace(pFirstVertex->m_hRod,0x04);
			//
			pPropList->DeleteAllSonItems(pItem);
			if(pLjPartPara->angle.bTwoEdge&&pLjPartPara->angle.cbSpaceFlag.StyleOfEndSpace()==ANGLE_SPACE_FLAG::SPACE_UDF)
				oper.InsertCmbListPropItem(pItem,"end_space_syncmode","","","",-1,TRUE);
			CPropTreeItem* pSonItem=oper.InsertEditPropItem(pItem,"end_space","","",-1,TRUE);
			pSonItem->SetReadOnly(pLjPartPara->angle.cbSpaceFlag.StyleOfEndSpace()>0);
		}
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("end_space"))
		{
			for(UINT i=0;i<selectVertexes.Size();i++)
			{
				if(selectVertexes[i].pLjPartPara)
				{
					if(selectVertexes[i].pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER2||selectVertexes[i].pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING2)
					{
						if(selectVertexes[i].pLjPartPara)
							selectVertexes[i].pLjPartPara->startSideEndSpace=atof(valueStr);
						if(selectVertexes[i].pBoltPadSpace)
							selectVertexes[i].pBoltPadSpace->endSpaceS=atoi(valueStr);
					}
					else //if(pFirstVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER||pFirstVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING)
					{
						if(selectVertexes[i].pLjPartPara)
							selectVertexes[i].pLjPartPara->endSideEndSpace=atof(valueStr);
						if(selectVertexes[i].pBoltPadSpace)
							selectVertexes[i].pBoltPadSpace->endSpaceE=atoi(valueStr);
					}
				}
			}
			bReCalVertexPos=TRUE;
		}
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("end_space_syncmode"))
		{
			for(UINT i=0;i<selectVertexes.Size();i++)
			{
				if(selectVertexes[i].pLjPartPara)
				{
					BYTE ciAsynDualEdgeEndSpace=selectVertexes[i].pLjPartPara->ciAsynDualEdgeEndSpace;
					selectVertexes[i].pLjPartPara->ciAsynDualEdgeEndSpace=1;	//����Ϊ�첽�Ա��ڸ�ֵ
					if(selectVertexes[i].pVertex->m_cPosDesignType==1||selectVertexes[i].pVertex->m_cPosDesignType==2)
					{
						if(ciAsynDualEdgeEndSpace==0&&valueStr[0]>'0')
							selectVertexes[i].pLjPartPara->startSideEndSpace=selectVertexes[i].pLjPartPara->end_space;
						if(ciAsynDualEdgeEndSpace==1&&valueStr[0]!='1')	//�첽��ͬ��
							selectVertexes[i].pLjPartPara->startSideEndSpace=selectVertexes[i].pLjPartPara->end_space;
						//if(ciAsynDualEdgeEndSpace==2&&valueStr[0]=='1')	//ͬ�����첽
						//	selectVertexes[i].pLjPartPara->startSideEndSpace=selectVertexes[i].pLjPartPara->end_space;
					}
					else if(selectVertexes[i].pVertex->m_cPosDesignType==3||selectVertexes[i].pVertex->m_cPosDesignType==4)
					{	//��һ��
						if(ciAsynDualEdgeEndSpace==0&&valueStr[0]>'0')
							selectVertexes[i].pLjPartPara->startSideEndSpace=selectVertexes[i].pLjPartPara->end_space;
						if(ciAsynDualEdgeEndSpace==1&&valueStr[0]!='1')	//�첽��ͬ��
							selectVertexes[i].pLjPartPara->end_space=selectVertexes[i].pLjPartPara->startSideEndSpace;
					}
					selectVertexes[i].pLjPartPara->ciAsynDualEdgeEndSpace=valueStr[0]-'0';
				}
			}
			bReCalVertexPos=TRUE;
		}
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("ber_space_syncmode"))
		{
			for(UINT i=0;i<selectVertexes.Size();i++)
			{
				if(selectVertexes[i].pLjPartPara)
				{
					BYTE ciAsynDualEdgeBerSpace=selectVertexes[i].pLjPartPara->ciAsynDualEdgeBerSpace;
					selectVertexes[i].pLjPartPara->ciAsynDualEdgeBerSpace=1;	//����Ϊ�첽�Ա��ڸ�ֵ
					if(selectVertexes[i].pVertex->m_cPosDesignType==1||selectVertexes[i].pVertex->m_cPosDesignType==2)
					{
						if(ciAsynDualEdgeBerSpace==0&&valueStr[0]>'0')
							selectVertexes[i].pLjPartPara->startSideBerSpace=selectVertexes[i].pLjPartPara->ber_space;
						if(ciAsynDualEdgeBerSpace==1&&valueStr[0]!='1')	//�첽��ͬ��
							selectVertexes[i].pLjPartPara->startSideBerSpace=selectVertexes[i].pLjPartPara->ber_space;
						//if(ciAsynDualEdgeBerSpace==2&&valueStr[0]=='1')	//ͬ�����첽
						//	selectVertexes[i].pLjPartPara->startSideBerSpace=selectVertexes[i].pLjPartPara->ber_space;
					}
					else if(selectVertexes[i].pVertex->m_cPosDesignType==3||selectVertexes[i].pVertex->m_cPosDesignType==4)
					{	//��һ��
						if(ciAsynDualEdgeBerSpace==0&&valueStr[0]>'0')
							selectVertexes[i].pLjPartPara->startSideBerSpace=selectVertexes[i].pLjPartPara->ber_space;
						if(ciAsynDualEdgeBerSpace==1&&valueStr[0]!='1')	//�첽��ͬ��
							selectVertexes[i].pLjPartPara->ber_space=selectVertexes[i].pLjPartPara->startSideBerSpace;
					}
					selectVertexes[i].pLjPartPara->ciAsynDualEdgeBerSpace=valueStr[0]-'0';
				}
			}
			bReCalVertexPos=TRUE;
		}
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("wing_space_syncmode"))
		{
			for(UINT i=0;i<selectVertexes.Size();i++)
			{
				if(selectVertexes[i].pLjPartPara)
				{
					BYTE ciAsynDualEdgeWingSpace=selectVertexes[i].pLjPartPara->ciAsynDualEdgeWingSpace;
					selectVertexes[i].pLjPartPara->ciAsynDualEdgeWingSpace=1;	//����Ϊ�첽�Ա��ڸ�ֵ
					if(selectVertexes[i].pVertex->m_cPosDesignType==1||selectVertexes[i].pVertex->m_cPosDesignType==2)
					{
						if(ciAsynDualEdgeWingSpace==0&&valueStr[0]>'0')
							selectVertexes[i].pLjPartPara->startSideWingSpace=selectVertexes[i].pLjPartPara->wing_space;
						if(ciAsynDualEdgeWingSpace==1&&valueStr[0]!='1')	//�첽��ͬ��
							selectVertexes[i].pLjPartPara->startSideWingSpace=selectVertexes[i].pLjPartPara->wing_space;
						//if(ciAsynDualEdgeBerSpace==2&&valueStr[0]=='1')	//ͬ�����첽
						//	selectVertexes[i].pLjPartPara->startSideWingSpace=selectVertexes[i].pLjPartPara->wing_space;
					}
					else if(selectVertexes[i].pVertex->m_cPosDesignType==3||selectVertexes[i].pVertex->m_cPosDesignType==4)
					{	//��һ��
						if(ciAsynDualEdgeWingSpace==0&&valueStr[0]>'0')
							selectVertexes[i].pLjPartPara->startSideWingSpace=selectVertexes[i].pLjPartPara->wing_space;
						if(ciAsynDualEdgeWingSpace==1&&valueStr[0]!='1')	//�첽��ͬ��
							selectVertexes[i].pLjPartPara->wing_space=selectVertexes[i].pLjPartPara->startSideWingSpace;
					}
					selectVertexes[i].pLjPartPara->ciAsynDualEdgeWingSpace=valueStr[0]-'0';
				}
			}
			bReCalVertexPos=TRUE;
		}
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("fLengCutWidth"))
		{
			pFirstVertex->fLengCutWidth=(float)atof(valueStr);
			((CLDSPlate*)pBelongPlate)->designInfo.iProfileStyle0123=3;
			bReCalVertexPos=TRUE;
		}
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("fWingCutWidth"))
		{
			pFirstVertex->fWingCutWidth=(float)atof(valueStr);
			((CLDSPlate*)pBelongPlate)->designInfo.iProfileStyle0123=3;
			bReCalVertexPos=TRUE;
		}
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("xOffset.x"))
		{
			pFirstVertex->xOffset.x=(float)atof(valueStr);
			((CLDSPlate*)pBelongPlate)->designInfo.iProfileStyle0123=3;
			bReCalVertexPos=TRUE;
		}
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("xOffset.y"))
		{
			bool processed=false;
			if(pBelongPlate->wiType==CLDSPlate::WASHER_PLATE&&pFirstVertex->xOffset.ciOffMode==0)
			{
				processed=true;
				if(pFirstVertex->m_cPosDesignType==1||pFirstVertex->m_cPosDesignType==3)
					pFirstVertex->xOffset.y=-(float)atof(valueStr);
				else if(pFirstVertex->m_cPosDesignType==2||pFirstVertex->m_cPosDesignType==4)
					pFirstVertex->xOffset.y= (float)atof(valueStr);
				else
					processed=false;
			}
			if(!processed)
				pFirstVertex->xOffset.y=(float)atof(valueStr);
			((CLDSPlate*)pBelongPlate)->designInfo.iProfileStyle0123=3;
			bReCalVertexPos=TRUE;
		}
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("vertex.x")||
			pItem->m_idProp==PROFILE_VERTEX::GetPropID("vertex.y"))
		{
			f3dPoint pt=pFirstVertex->vertex;
			if(pFirstVertex->m_pTransCS)
				coord_trans(pt,*(pFirstVertex->m_pTransCS),FALSE);
			if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("vertex.y"))
				pt.y=atof(valueStr);
			else
				pt.x=atof(valueStr);
			if(pFirstVertex->m_cPosDesignType==0)
			{
				if(pFirstVertex->m_pTransCS)
					coord_trans(pt,*(pFirstVertex->m_pTransCS),TRUE);
				pFirstVertex->vertex=pt;
			}
			bReCalVertexPos=TRUE;
		}
		else 
			return ModifyProfileLineProperty(pPropList,pItem,valueStr);
		if(bReCalVertexPos && pBelongPlate->GetClassTypeId()==CLS_PLATE)
		{
			CUndoOperObject undo(&Ta,true);
			ReCalVertexPostiom(pFirstVertex,pBelongPlate);
			CLDSPlate* pPlate=(CLDSPlate*)pBelongPlate;
			pPlate->DesignPlate();
			if(pPlate->wiType==CLDSGeneralPlate::WASHER_PLATE)
				pView->CutBoltPadByAngle(pPlate);
			//������Ƹְ������������µ�ǰѡ����pSelObj����ˢ��ʵ��
			pBelongPlate->SetModified();
			pBelongPlate->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBelongPlate->GetSolidPartObject());
			CLDSView::RedrawPlateProfileDrawing(pBelongPlate,NULL,true,0x03);
			UpdateSelSubObj(pView->selectSubObjs.GetFirst(),pBelongPlate);
			g_pSolidDraw->Draw();
		}
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
	return TRUE;
}
BOOL ProfileVertexButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	SUB_OBJ* pSelObj=pView->selectSubObjs.GetFirst();
	PROFILE_VERTEX* pVertex=(PROFILE_VERTEX*)pSelObj->pSubObj;
	if(pVertex==NULL)
		return FALSE;
	char sText[100]="";
	try
	{
		if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("hRod"))
		{
			static	CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Rod Start Node";
			modifyhdlg.m_sTitle="Start Node:";
#else
			modifyhdlg.m_sCaption="�޸������˼�";
			modifyhdlg.m_sTitle="�����˼�:";
#endif
			modifyhdlg.m_sHandle.Format("0X%X",pVertex->m_hRod);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Please select datum node:");
#else
			modifyhdlg.SetCmdPickPromptStr("��ѡ�������˼�:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_PART);
			modifyhdlg.m_iMirMsg=0;
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
				if(pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"Inputted handle error,can't find related part��",NULL,MB_OK);
#else
					MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",NULL,MB_OK);
#endif
				else
				{
					CUndoOperObject undo(&Ta);
					CString valueStr;
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(PROFILE_VERTEX::GetPropID("hRod"),valueStr);
					g_pSolidDraw->ReleaseSnapStatus();
					for (SUB_OBJ* pObj=pView->selectSubObjs.GetFirst();pObj;pObj=pView->selectSubObjs.GetNext())
					{
						PROFILE_VERTEX* pSelVertex=(PROFILE_VERTEX*)pObj->pSubObj;
						pSelVertex->m_hRod=modifyhdlg.m_nHandle;
						//g_pSolidDraw->SetEntSnapStatus(pObj->iSubId);			������
					}
				}
			}
		}
		else if(pItem->m_idProp==PROFILE_VERTEX::GetPropID("vertex"))
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Vertex Coordinate";
#else 
			vecDlg.m_sCaption = "��������";
#endif
			vecDlg.m_fVectorX = pVertex->vertex.x;
			vecDlg.m_fVectorY = pVertex->vertex.y;
			vecDlg.m_fVectorZ = pVertex->vertex.z;	//��ά���꣬z������ʼ��Ϊ��
			vecDlg.m_bReadOnlyX=TRUE;
			vecDlg.m_bReadOnlyY=TRUE;
			vecDlg.m_bReadOnlyZ=TRUE;
			vecDlg.DoModal();
		}
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
	return TRUE;
}
void UpdateVertexDesignItem(PROFILE_VERTEX* pVertex,CPropertyList* pPropList,CPropTreeItem *pLeaderItem)
{
	if(pVertex==NULL || pVertex->m_pBelongPlate==NULL)
		return;
	if(pVertex->m_pBelongPlate->GetClassTypeId()!=CLS_PLATE)
		return;
	if( ((CLDSPlate*)pVertex->m_pBelongPlate)->designInfo.iProfileStyle0123!=1&&
		((CLDSPlate*)pVertex->m_pBelongPlate)->designInfo.iProfileStyle0123!=3)	//�Ƕ�������
		return;
	CDesignLjPartPara* pCurrLjRod=((CLDSPlate*)pVertex->m_pBelongPlate)->designInfo.partList.GetValue(pVertex->m_hRod);
	CLDSLinePart* pLinePart=(CLDSLinePart*)Ta.FromPartHandle(pVertex->m_hRod,CLS_LINEPART);
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropTreeItem* pPropItem=NULL,*pSonItem=NULL;
	CPropListOper<PROFILE_VERTEX,CTmaPtrList<SUB_OBJ,SUB_OBJ*> >oper(pPropList,pVertex,&pView->selectSubObjs);	
	pPropList->DeleteAllSonItems(pLeaderItem);
	if(pVertex->m_cPosDesignType>=PROFILE_VERTEX::DT_BER&&pVertex->m_cPosDesignType<=PROFILE_VERTEX::DT_WING2)
	{	//�����˼�
		pPropItem=oper.InsertButtonPropItem(pLeaderItem,"hRod","","",-1,TRUE);
		if(pLinePart&&pLinePart->IsAngle()&&pVertex->m_pBelongPlate->wiType!=CLDSGeneralPlate::WASHER_PLATE)	//
		{	
			oper.InsertCmbListPropItem(pPropItem,"start0_end1","","","",-1,TRUE);	//���Ӷ�
			oper.InsertCmbListPropItem(pPropItem,"wing_x0_y1","","","",-1,TRUE);	//����֫
		}
		if(pVertex->m_cPosDesignType==1||pVertex->m_cPosDesignType==3)
		{	//���߲෽ʽ
			if(pCurrLjRod && pVertex->m_pBelongPlate->wiType!=CLDSGeneralPlate::WASHER_PLATE)
			{	//
				pPropItem=oper.InsertCmbListPropItem(pLeaderItem,"ber_space_style","","","",-1,TRUE);
				if(pCurrLjRod->angle.bTwoEdge&&pCurrLjRod->angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_UDF)
					oper.InsertCmbListPropItem(pPropItem,"ber_space_syncmode","","","",-1,TRUE);
				pSonItem=oper.InsertEditPropItem(pPropItem,"ber_space","","",-1,TRUE);
				pSonItem->SetReadOnly(pCurrLjRod->angle.cbSpaceFlag.StyleOfBerSpace()>0);
			}
			else	//��˨���
				oper.InsertEditPropItem(pLeaderItem,"ber_space","","",-1,TRUE);
		}
		else if(pVertex->m_cPosDesignType==2||pVertex->m_cPosDesignType==4)
		{	//֫���෽ʽ
			if(pCurrLjRod && pVertex->m_pBelongPlate->wiType!=CLDSGeneralPlate::WASHER_PLATE)
			{
				pPropItem=oper.InsertCmbListPropItem(pLeaderItem,"wing_space_style","","","",-1,TRUE);
				if(pCurrLjRod->angle.bTwoEdge&&pCurrLjRod->angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_UDF)
					oper.InsertCmbListPropItem(pPropItem,"wing_space_syncmode","","","",-1,TRUE);
				pSonItem=oper.InsertEditPropItem(pPropItem,"wing_space","","",-1,TRUE);
				pSonItem->SetReadOnly(pCurrLjRod->angle.cbSpaceFlag.StyleOfWingSpace()>0);
			}
			else	//��˨���
				oper.InsertEditPropItem(pLeaderItem,"wing_space","","",-1,TRUE);
		}
		//�˾�
		if(pCurrLjRod && pVertex->m_pBelongPlate->wiType!=CLDSGeneralPlate::WASHER_PLATE)
		{	
			pPropItem=oper.InsertCmbListPropItem(pLeaderItem,"end_space_style","","","",-1,TRUE);
			if(pCurrLjRod->angle.bTwoEdge&&pCurrLjRod->angle.cbSpaceFlag.StyleOfEndSpace()==ANGLE_SPACE_FLAG::SPACE_UDF)
				oper.InsertCmbListPropItem(pPropItem,"end_space_syncmode","","","",-1,TRUE);
			pSonItem=oper.InsertEditPropItem(pPropItem,"end_space","","",-1,TRUE);
			pSonItem->SetReadOnly(pCurrLjRod->angle.cbSpaceFlag.StyleOfEndSpace()>0);
			//
			if(pVertex->xOffset.x!=0)
			{
				CPropTreeItem* pPropItem=oper.InsertEditPropItem(pLeaderItem,"xOffset.x","","",-1,TRUE);
				if(pVertex->xOffset.ciOffMode==0)
					pPropItem->m_lpNodeInfo->m_strPropName="�����н�";
			}
			if(pVertex->xOffset.y!=0)	
			{
				CPropTreeItem* pPropItem=oper.InsertEditPropItem(pLeaderItem,"xOffset.y","","",-1,TRUE);
				if(pVertex->xOffset.ciOffMode==0)
					pPropItem->m_lpNodeInfo->m_strPropName="�����н�";
			}
		}
		else	//��˨���
		{
			oper.InsertEditPropItem(pLeaderItem,"end_space","","",-1,TRUE);
			if(pVertex->fLengCutWidth!=0)
				oper.InsertEditPropItem(pLeaderItem,"fLengCutWidth","","",-1,TRUE);
			if(pVertex->fWingCutWidth!=0)	
				oper.InsertEditPropItem(pLeaderItem,"fWingCutWidth","","",-1,TRUE);
		}
	}
}
//��ʾ�ְ嶥������
void CLDSView::DisplayProfileVertexProperty(BOOL bRebuild,CLDSGeneralPlate* pBelongPlate)
{
	SUB_OBJ* pFirstSubObj=selectSubObjs.GetFirst();
	if(pFirstSubObj==NULL || (pFirstSubObj->ciSubType!=IDbEntity::DbPoint&&abs(pFirstSubObj->ciSubType)!=IDbEntity::DbLine))
		return;
	PROFILE_VERTEX* pVertex=(PROFILE_VERTEX*)pFirstSubObj->pSubObj;
	if(pVertex==NULL)
		return;
	if(pBelongPlate==NULL)
		pBelongPlate=(CLDSGeneralPlate*)Ta.Parts.FromHandle(pFirstSubObj->hObj,CLS_PLATE,CLS_PARAMPLATE);
	if(pBelongPlate!=NULL)
	{
		g_pSolidDraw->BatchClearCS(0x20);
		g_pSolidDraw->AddCS(pBelongPlate->handle,pBelongPlate->ucs,NULL,NULL,NULL,NULL,200);
	}
	pVertex->SetBelongPlate(pBelongPlate,&console.m_curWCS);
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList* pPropList=pPropDlg->GetPropertyList();
	//
	const int GROUP_GENERAL=1;
	pPropDlg->m_arrPropGroupLabel.RemoveAll();
	pPropDlg->m_arrPropGroupLabel.SetSize(1);
#ifdef AFX_TARG_ENU_ENGLISH
	pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
#else 
	pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
#endif
	pPropDlg->RefreshTabCtrl(PROFILE_VERTEX::m_iCurDisplayPropGroup);
	//
	pPropList->CleanTree();
	pPropList->m_nObjClassTypeId = CLS_PROFILEVERTEX;
	pPropList->SetModifyValueFunc(ModifyProfileVertexProperty);
	pPropList->SetButtonClickFunc(ProfileVertexButtonClick);
	pPropList->SetModifyStatusFunc(ModifyProfileVertexItemStatus);
	CPropListOper<PROFILE_VERTEX,CTmaPtrList<SUB_OBJ,SUB_OBJ*> >oper(pPropList,pVertex,&selectSubObjs);
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL,*pSonItem=NULL,*pRoot=pPropList->GetRootItem();
	pRoot->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	//�����ְ������Ϣ
	pGroupItem=oper.InsertPropItem(pRoot,"subjectPlateInfo");
	//�����ְ���
	pPropItem=oper.InsertEditPropItem(pGroupItem,"m_hSubjectPlate");
	pPropItem->SetReadOnly();
	//�����ְ����
	oper.InsertEditPropItem(pGroupItem,"m_sSubjectPlatePartNo");
	//�����ְ���� 
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"m_cSubjectPlateMaterial");
	pPropItem->m_lpNodeInfo->m_cmbItems=MakeMaterialMarkSetString();
	//�����ְ��� 
	oper.InsertEditPropItem(pGroupItem,"m_fSubjectPlateThick");
	//�����ְ��������
	oper.InsertButtonPropItem(pGroupItem,"m_relaPlate");
	//��ʾ������
	//oper.InsertCmbListPropItem(pGroupItem,"m_cPickedDisplayMode");
	if(pFirstSubObj->ciSubType==IDbEntity::DbPoint)
	{	//���ζ��������Ϣ
		pGroupItem=oper.InsertPropItem(pRoot,"basicInfo");
		//��������
		oper.InsertCmbListPropItem(pGroupItem,"cVertexType");
		//��������
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"vertex");
		pPropItem->m_bHideChildren = FALSE;
		//����X�������
		pSonItem=oper.InsertEditPropItem(pPropItem,"vertex.x");
		pSonItem->SetReadOnly(pVertex->m_cPosDesignType!=0);
		//����Y�������
		pSonItem=oper.InsertEditPropItem(pPropItem,"vertex.y");
		pSonItem->SetReadOnly(pVertex->m_cPosDesignType!=0);
		//����Z�������
		pSonItem=oper.InsertEditPropItem(pPropItem,"vertex.z");
		pSonItem->SetReadOnly();
		//���ζ��������Ϣ
		pGroupItem=oper.InsertPropItem(pRoot,"vertexDesignInfo");
		//�����������
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"cPosDesignType");
		//
		UpdateVertexDesignItem(pVertex,pPropList,pPropItem);
	}
	else if(abs(pFirstSubObj->ciSubType)==IDbEntity::DbLine)
	{	//�����߹�����Ϣ
		pGroupItem=oper.InsertPropItem(pRoot,"ProfileEdgeInfo");
		//�������
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"m_bRollEdge");
		if(pVertex->m_bRollEdge)
		{
			oper.InsertEditPropItem(pPropItem,"manu_space1");
			oper.InsertEditPropItem(pPropItem,"roll_edge_offset_dist");
		}
		//��������
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"m_bWeldEdge");
		if(pVertex->m_bWeldEdge)
		{
			oper.InsertEditPropItem(pPropItem,"manu_space");
			pSonItem=oper.InsertCmbListPropItem(pPropItem,"local_point_vec");
			if(pVertex->local_point_vec!=0)
				oper.InsertEditPropItem(pSonItem,"local_point_y");
		}
	}
	pPropList->Redraw();
}