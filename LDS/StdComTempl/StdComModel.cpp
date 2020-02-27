#include "stdafx.h"
#include "StdComModel.h"
#include "WireStruComTempl.h"
#include "LifeObj.h"
#include "..\env_def.h"
#include "..\GlobalFunc.h"
#include "..\UI.h"
#include "..\MirTaAtom.h"
#include "..\LdsModel\ConnBoltsStrength.h"

////////////////////////////////////////////////////////////////////////////////
CStdComModel::BOLTREF::BOLTREF(CLDSBolt* _pBolt/*=NULL*/,const double* location_coords/*=NULL*/)
{
	pBolt=_pBolt;
	location=location_coords;
}
CHashListEx<CStdComModel::SHAREANGLE> CStdComModel::hashShareAngles;
CStdComModel::CStdComModel(IModel* pModel/*=NULL*/)
{
	m_pModel=pModel;
	acs.InitStdCS();
}


CStdComModel::~CStdComModel(void)
{
}
bool CStdComModel::BuildAcsOfAssmComponent()
{
	return BuildAcsOfAssmComponent(m_pStdTempl,&acs);
}
bool CStdComModel::BuildAcsOfAssmComponent(CStdComTempl* pStdTempl,GECS* pAcs)
{
	pAcs->InitStdCS();
	STDTEMPL_PARAM_ITEM* pParam;
	//��׼���װ������ϵԭ��
	pParam=pStdTempl->listParams.GetValue(KEY4C("acs"));
	if(pParam==NULL)
		return true;
	EXPRESSION_FUNC* pExprFunc=NULL;
	CStdComTemplExpression expr(pStdTempl);
	if(strstr(pParam->exprstr,"CrossRodAcs")!=NULL)
		pExprFunc=expr.FromFunctionName("CrossRodAcs");
	else
		return false;
	char* pszParamStart=strchr(pParam->exprstr,'(');
	if(pszParamStart!=NULL)
		pszParamStart++;
	char* pszParamEnd=strchr(pParam->exprstr,')');
	if(pszParamEnd!=NULL)
	{
		CXhChar16 szParam;
		szParam.NCopy(pszParamStart,pszParamEnd-pszParamStart);
		pExprFunc->Calc(&pParam->value,szParam,&expr);
		pAcs->origin.Set(pParam->value.origin3d[0],pParam->value.origin3d[1],pParam->value.origin3d[2]);
		pAcs->axis_x.Set(pParam->value.axisx3d[0],pParam->value.axisx3d[1],pParam->value.axisx3d[2]);
		pAcs->axis_y.Set(pParam->value.axisy3d[0],pParam->value.axisy3d[1],pParam->value.axisy3d[2]);
		pAcs->axis_z.Set(pParam->value.axisz3d[0],pParam->value.axisz3d[1],pParam->value.axisz3d[2]);
		return true;
	}
	else
		return false;
}
////////////////////////////////////////////////////////////////////////////////
//���ܺ���
CLDSNode* CStdComModel::GetLdsNodeFromId(TMPLOBJID idNode)
{
	long hid;
	CLDSNode* pNode=NULL;
	if((hid=m_pStdTempl->MappingLdsNodeH(idNode))!=NULL)
	{
		if((pNode=m_pModel->FromNodeHandle(hid))!=NULL)
			return pNode;
	}
	if((hid=m_pStdTempl->MappingLdsNodeH(idNode.uiOriginalId))!=NULL)
		pNode=m_pModel->FromNodeHandle(hid);
	if(idNode.ciMirTransType>0&&pNode!=NULL)
	{
		char ciMirTransType=idNode.ciMirTransType;
		if( m_pStdTempl->m_ciBaseMirType=='Y'&&
			(idNode.ciMirTransType=='X'||idNode.ciMirTransType=='Y'))
			ciMirTransType='Y'-idNode.ciMirTransType+'X';
		if(ciMirTransType=='X')
			return pNode->GetMirNode(MIRMSG(1));
		else if(ciMirTransType=='Y')
			return pNode->GetMirNode(MIRMSG(2));
		else if(ciMirTransType=='Z')
			return pNode->GetMirNode(MIRMSG(2));
		else
			return pNode;
	}
	else
		return pNode;
}
CLDSPart* CStdComModel::GetLdsPartFromId(long idPart, BYTE ciPartType /*= 0*/)
{
	CMPSTID xid(abs(idPart));
	long *pHid = NULL;
	if (ciPartType == 0 || ciPartType == 1)
		pHid = m_pStdTempl->hashMatchRodHid.GetValue(xid.sidGroupRod);
	if ((ciPartType == 0 && pHid == NULL) || ciPartType == 2)
		pHid = m_pStdTempl->hashMatchPlates.GetValue(abs(idPart));
	CLDSPart* pPart = (pHid) ? m_pModel->FromPartHandle(*pHid) : NULL;
	if (pPart && pPart->GetClassTypeId() == CLS_GROUPLINEANGLE && xid.sidSerial > 0)
	{
		CLDSGroupLineAngle* pGroupAngle = (CLDSGroupLineAngle*)pPart;
		pPart = m_pModel->FromPartHandle(pGroupAngle->son_jg_h[xid.sidSerial - 1], CLS_LINEANGLE);
	}
	return pPart;
}
CLDSLinePart* CStdComModel::GetLdsRodFromId(long idRod)
{
	CMPSTID xid(abs(idRod));
	long *pHid= m_pStdTempl->hashMatchRodHid.GetValue(xid.sidGroupRod);
	CLDSLinePart* pRod = (pHid) ? m_pModel->FromRodHandle(*pHid) : NULL;
	if (pRod && pRod->GetClassTypeId() == CLS_GROUPLINEANGLE && xid.sidSerial > 0)
	{
		CStdTemplParaRod* pParaRod=m_pStdTempl->listParaRods.GetValue(xid.sidGroupRod);
		int siIndex=xid.sidSerial-1;
		CLDSGroupLineAngle* pGroupAngle = (CLDSGroupLineAngle*)pRod;
		if(pParaRod->bReverseMasterAngle&&xid.sidSerial <= 2)
			pRod = m_pModel->FromRodHandle(pGroupAngle->son_jg_h[1-siIndex]);
		else
			pRod = m_pModel->FromRodHandle(pGroupAngle->son_jg_h[siIndex]);
	}
	if(idRod<0&&pRod!=NULL)
		return pRod->GetMirRod(MIRMSG(1));
	else
		return pRod;
}
CLDSGeneralPlate* CStdComModel::GetLdsPlateFromId(long idPlate)
{
	CLDSGeneralPlate* pPlate=NULL;
	long *pHid=m_pStdTempl->hashMatchPlates.GetValue(abs(idPlate));
	if(pHid)
		pPlate=(CLDSGeneralPlate*)m_pModel->FromPartHandle(*pHid);
	return pPlate;
}
double CStdComModel::GetNormOffset(CStdTemplParaRod* pCurrParaRod,char cAngleTowardIn1Out2,const char* szNormalOffset,BYTE* pStyleG,CParaThick* pParaThick/*=NULL*/)
{	//szNormalOffset�У�'['��ʾ����;']'��ʾ�������޴���ָʾ����ʾ�������
	CParaThick thick;
	BYTE iStyleG=0;
	CLDSLinePart* pRefRod;
	CXhChar50 thickstr=szNormalOffset;
	PRESET_ARRAY4<CXhChar50> xarrThickStr;
	char* key=strtok(thickstr,"|");
	while(key!=NULL)
	{
		xarrThickStr.Append(key);
		key=strtok(NULL,"|");
	}
	for(UINT option=0;option<xarrThickStr.Count;option++)
	{
		char cCondType=0;	//���κ���������
		if((key=strchr(xarrThickStr[option],'['))||(key=strchr(xarrThickStr[option],']')))
			cCondType=*key;
		thickstr=xarrThickStr[option];
		if(cCondType!=0)
		{
			while(*key!=0&&*key!='=')
				key++;
			if(*key=='=')
				thickstr=key+1;
			else
				logerr.Log("error");
		}
		bool passed=(cCondType==0)||(cAngleTowardIn1Out2!=1&&cAngleTowardIn1Out2!=2)||
			(cCondType==']'&&cAngleTowardIn1Out2==1)||(cCondType=='['&&cAngleTowardIn1Out2==2);
		if(!passed)
		{
			iStyleG=4;
			continue;	//��ǰ�趨������������ʵ����������һ��
		}
		key=strtok(thickstr,",");
		JGZJ jgzj;
		while(key!=NULL)
		{
			int sign=1;
			if(*key=='-')
			{
				iStyleG=4;
				sign=-1;
				key++;
			}
			if((*key)=='g'||(*key)=='G')
			{
				iStyleG=0;
				if(pCurrParaRod->pRelaRod!=NULL)
				{
					getjgzj(jgzj,pCurrParaRod->pRelaRod->GetWidth());
					thick.AddThick(jgzj.g*sign);
				}
			}
			else if(*key=='$'||*key=='R')
			{
				key++;
				char* iter=key;
				while(*iter!='#')
					iter++;
				CXhChar16 szRodId;
				szRodId.NCopy(key,iter-key);
				long idRod=atoi(szRodId);
				if((pRefRod=this->GetLdsRodFromId(idRod))!=NULL)
				{//��ȡ������
					key=iter+1;
					if(*key=='g'||*key=='G')
					{
						iStyleG=0;
						getjgzj(jgzj,pRefRod->size_wide);
						thick.AddThick(sign*jgzj.g);
					}
					else if(*key=='t'||*key=='T')
						thick.AddThick(sign*pRefRod->handle,true);
				}
				iStyleG=4;
			}
			else if(*key=='P')
			{
				key++;
				char* iter=key;
				while(*iter!='#')
					iter++;
				CXhChar16 szPlateId;
				szPlateId.NCopy(key,iter-key);
				long idPlate=atoi(szPlateId);
				CStdTemplParaPlate* pParaPlate=m_pStdTempl->listParaPlates.GetValue(idPlate);
				key=iter+1;
				if(pParaPlate && (*key=='t'||*key=='T'))
					thick.AddThick(sign*pParaPlate->_thick);
				iStyleG=4;
			}
			else if(*key!=0)
			{
				if(sign==-1)
					thick.BuildFromStr(key-1);
				else
					thick.BuildFromStr(key);
				iStyleG=4;
			}
			key=strtok(NULL,",");
		}
	}
	//"-$4#g
	if(pParaThick)
		*pParaThick=thick;
	if(pStyleG)
		*pStyleG=iStyleG;
	return thick.Thick(m_pModel);
}
CLDSNode* CStdComModel::AddOrUpdateParaModelNode(CStdTemplParaNode* pParaNode)
{
	long *pHid=NULL;
	CLDSNode* pDatumNode=NULL,*pDependStart=NULL,*pDependEnd=NULL;
	CLDSNode* pLdsNode=pParaNode->pRelaNode;
	bool bCreateNewNode=(pLdsNode==NULL);
	if(pLdsNode==NULL)
		pParaNode->pRelaNode=pLdsNode=m_pModel->AppendNode();
	pParaNode->m_bNeedUpdateRelaInfo=false;
	if(pParaNode->ciPosCalType==CStdTemplParaNode::INDEPENDENT)
	{
		pLdsNode->m_cPosCalType=CLDSNode::COORD_INDEPENDENT;
		if(pParaNode->posx_expression[0]!=0)
			pParaNode->xPosition.x=m_xExpression.SolveExpression(pParaNode->posx_expression);
		if(pParaNode->posy_expression[0]!=0)
			pParaNode->xPosition.y=m_xExpression.SolveExpression(pParaNode->posy_expression);
		if(pParaNode->posz_expression[0]!=0)
			pParaNode->xPosition.z=m_xExpression.SolveExpression(pParaNode->posz_expression);
		pHid=m_pStdTempl->hashMatchRodHid.GetValue(pParaNode->keyLabelFatherRod);
		pLdsNode->hFatherPart=pLdsNode->arrRelationPole[0]=pHid!=NULL?*pHid:0;
		CLDSLinePart* pRod=(pHid!=NULL)?m_pModel->FromRodHandle(*pHid):NULL;
		if(pRod!=NULL)
			pLdsNode->cfgword=pRod->cfgword;
		if(acs.axis_z*m_xNearAxisZ<0)
			pParaNode->xPosition.z*=-1;
		GEPOINT pos=acs.TransPFromCS(pParaNode->xPosition);
		pLdsNode->SetPosition(pos);
	}
	else if(pParaNode->ciPosCalType==CStdTemplParaNode::COORD_X_SPECIFIED)
	{
		pLdsNode->m_cPosCalType=CLDSNode::COORD_X_SPECIFIED;
		if(pParaNode->posx_expression[0]!=0)
			pParaNode->xPosition.x=m_xExpression.SolveExpression(pParaNode->posx_expression);
		if(pParaNode->posy_expression[0]!=0)
			pParaNode->xPosition.y=m_xExpression.SolveExpression(pParaNode->posy_expression);
		if(pParaNode->posz_expression[0]!=0)
			pParaNode->xPosition.z=m_xExpression.SolveExpression(pParaNode->posz_expression);
		pDependStart=GetLdsNodeFromId(pParaNode->attachNodeArr[0]);
		pLdsNode->arrRelationNode[0]=pDependStart!=NULL?pDependStart->handle:0;
		pDependEnd=GetLdsNodeFromId(pParaNode->attachNodeArr[1]);
		pLdsNode->arrRelationNode[1]=pDependEnd!=NULL?pDependEnd->handle:0;
		pHid=m_pStdTempl->hashMatchRodHid.GetValue(pParaNode->keyLabelFatherRod);
		pLdsNode->hFatherPart=pLdsNode->arrRelationPole[0]=pHid!=NULL?*pHid:0;
		GEPOINT pos=acs.TransPFromCS(pParaNode->xPosition);
		pLdsNode->SetPosition(pos);
		if(pLdsNode->arrRelationNode[0]==0||pLdsNode->arrRelationNode[1]==0)
			pParaNode->m_bNeedUpdateRelaInfo=true;
		else
		{
			pLdsNode->cfgword=pDependStart->cfgword;
			pLdsNode->cfgword&=pDependEnd->cfgword;
			pLdsNode->CalPosition();
		}
	}
	else if(pParaNode->ciPosCalType==CStdTemplParaNode::COORD_Y_SPECIFIED)
	{
		pLdsNode->m_cPosCalType=CLDSNode::COORD_Y_SPECIFIED;
		if(pParaNode->posy_expression[0]!=0)
			pParaNode->xPosition.y=m_xExpression.SolveExpression(pParaNode->posy_expression);
		pDependStart=GetLdsNodeFromId(pParaNode->attachNodeArr[0]);
		pLdsNode->arrRelationNode[0]=pDependStart!=NULL?pDependStart->handle:0;
		pDependEnd=GetLdsNodeFromId(pParaNode->attachNodeArr[1]);
		pLdsNode->arrRelationNode[1]=pDependEnd!=NULL?pDependEnd->handle:0;
		pHid=m_pStdTempl->hashMatchRodHid.GetValue(pParaNode->keyLabelFatherRod);
		pLdsNode->hFatherPart=pLdsNode->arrRelationPole[0]=pHid!=NULL?*pHid:0;
		GEPOINT pos=acs.TransPFromCS(pParaNode->xPosition);
		pLdsNode->SetPosition(pos);
		if(pLdsNode->arrRelationNode[0]==0||pLdsNode->arrRelationNode[1]==0)
			pParaNode->m_bNeedUpdateRelaInfo=true;
		else
		{
			pLdsNode->cfgword=pDependStart->cfgword;
			pLdsNode->cfgword&=pDependEnd->cfgword;
			pLdsNode->CalPosition();
		}
	}
	else if(pParaNode->ciPosCalType==CStdTemplParaNode::COORD_Z_SPECIFIED)
	{
		pLdsNode->m_cPosCalType=CLDSNode::COORD_Z_SPECIFIED;
		if(pParaNode->posz_expression[0]!=0)
			pParaNode->xPosition.z=m_xExpression.SolveExpression(pParaNode->posz_expression);
		pDependStart=GetLdsNodeFromId(pParaNode->attachNodeArr[0]);
		pLdsNode->arrRelationNode[0]=pDependStart!=NULL?pDependStart->handle:0;
		pDependEnd=GetLdsNodeFromId(pParaNode->attachNodeArr[1]);
		pLdsNode->arrRelationNode[1]=pDependEnd!=NULL?pDependEnd->handle:0;
		pHid=m_pStdTempl->hashMatchRodHid.GetValue(pParaNode->keyLabelFatherRod);
		pLdsNode->hFatherPart=pLdsNode->arrRelationPole[0]=pHid!=NULL?*pHid:0;
		GEPOINT pos=acs.TransPFromCS(pParaNode->xPosition);
		pLdsNode->SetPosition(pos);
		if(pLdsNode->arrRelationNode[0]==0||pLdsNode->arrRelationNode[1]==0)
			pParaNode->m_bNeedUpdateRelaInfo=true;
		else
		{
			pLdsNode->cfgword=pDependStart->cfgword;
			pLdsNode->cfgword&=pDependEnd->cfgword;
			pLdsNode->CalPosition();
		}
	}
	else if(pParaNode->ciPosCalType==CStdTemplParaNode::SCALE)
	{
		pLdsNode->m_cPosCalType=CLDSNode::COORD_SCALE;
		if(pParaNode->scale_expression[0]!=0)
			pLdsNode->attach_scale=m_xExpression.SolveExpression(pParaNode->scale_expression);
		else
			pLdsNode->attach_scale=pParaNode->attach_scale;
		pDependStart=GetLdsNodeFromId(pParaNode->attachNodeArr[0]);
		pLdsNode->arrRelationNode[0]=pDependStart!=NULL?pDependStart->handle:0;
		pDependEnd=GetLdsNodeFromId(pParaNode->attachNodeArr[1]);
		pLdsNode->arrRelationNode[1]=pDependEnd!=NULL?pDependEnd->handle:0;
		pHid=m_pStdTempl->hashMatchRodHid.GetValue(pParaNode->keyLabelFatherRod);
		pLdsNode->hFatherPart=pLdsNode->arrRelationPole[0]=pHid!=NULL?*pHid:0;
		if(pLdsNode->arrRelationNode[0]==0||pLdsNode->arrRelationNode[1]==0)
			pParaNode->m_bNeedUpdateRelaInfo=true;
		else
		{
			pLdsNode->cfgword=pDependStart->cfgword;
			pLdsNode->cfgword&=pDependEnd->cfgword;
			pLdsNode->CalPosition();
		}
	}
	else if(pParaNode->ciPosCalType==CStdTemplParaNode::OFFSET)
	{
		pLdsNode->m_cPosCalType=CLDSNode::COORD_OFFSET;
		pDatumNode=GetLdsNodeFromId(pParaNode->attachNodeArr[0]);
		pLdsNode->arrRelationNode[0]=pDatumNode!=NULL?pDatumNode->handle:0;
		pDependStart=GetLdsNodeFromId(pParaNode->attachNodeArr[1]);
		pLdsNode->arrRelationNode[1]=pDependStart!=NULL?pDependStart->handle:0;
		pDependEnd=GetLdsNodeFromId(pParaNode->attachNodeArr[2]);
		pLdsNode->arrRelationNode[2]=pDependEnd!=NULL?pDependEnd->handle:0;
		if(pParaNode->offset_expression[0]!=0)
			pLdsNode->attach_offset=m_xExpression.SolveExpression(pParaNode->offset_expression);
		else
			pLdsNode->attach_offset = 0;//pParaNode->attach_scale
		pLdsNode->m_cPosCalViceType=pParaNode->ciOffsetType;

		pHid=m_pStdTempl->hashMatchRodHid.GetValue(pParaNode->keyLabelFatherRod);
		pLdsNode->hFatherPart=pLdsNode->arrRelationPole[0]=pHid!=NULL?*pHid:0;
		if(pLdsNode->arrRelationNode[0]==0||pLdsNode->arrRelationNode[1]==0||pLdsNode->arrRelationNode[2]==0)
			pParaNode->m_bNeedUpdateRelaInfo=true;
		else
		{
			pLdsNode->cfgword=pDependStart->cfgword;
			pLdsNode->cfgword&=pDependEnd->cfgword;
			pLdsNode->CalPosition();
		}
	}
	else if(pParaNode->ciPosCalType==CStdTemplParaNode::INTERSECTION)
	{
		pLdsNode->m_cPosCalType=CLDSNode::COORD_INTERSECTION;
		CLDSNode* pDependNodeS1=GetLdsNodeFromId(pParaNode->attachNodeArr[0]);
		pLdsNode->arrRelationNode[0]=pDependNodeS1!=NULL?pDependNodeS1->handle:0;
		CLDSNode* pDependNodeE1=GetLdsNodeFromId(pParaNode->attachNodeArr[1]);
		pLdsNode->arrRelationNode[1]=pDependNodeE1!=NULL?pDependNodeE1->handle:0;
		CLDSNode* pDependNodeS2=GetLdsNodeFromId(pParaNode->attachNodeArr[2]);
		pLdsNode->arrRelationNode[2]=pDependNodeS2!=NULL?pDependNodeS2->handle:0;
		CLDSNode* pDependNodeE2=GetLdsNodeFromId(pParaNode->attachNodeArr[3]);
		pLdsNode->arrRelationNode[3]=pDependNodeE2!=NULL?pDependNodeE2->handle:0;
		pHid=m_pStdTempl->hashMatchRodHid.GetValue(pParaNode->keyLabelFatherRod);
		pLdsNode->hFatherPart=pLdsNode->arrRelationPole[0]=pHid!=NULL?*pHid:0;
		if(pLdsNode->arrRelationNode[0]==0||pLdsNode->arrRelationNode[1]==0||pLdsNode->arrRelationNode[2]==0||pLdsNode->arrRelationNode[3]==0)
			pParaNode->m_bNeedUpdateRelaInfo=true;
		else
			pLdsNode->CalPosition();
	}
	else if(pParaNode->ciPosCalType==CStdTemplParaNode::PLANE_XZ_SPECIFIED)
	{
		pLdsNode->m_cPosCalType=CLDSNode::PLANE_XZ_SPECIFIED;
		pLdsNode->m_cPosCalViceType=pParaNode->ciMirTransType;
		CLDSNode* pDependNode0=GetLdsNodeFromId(pParaNode->attachNodeArr[0]);
		pLdsNode->arrRelationNode[0]=pDependNode0!=NULL?pDependNode0->handle:0;
		CLDSNode* pDependNode1=GetLdsNodeFromId(pParaNode->attachNodeArr[1]);
		pLdsNode->arrRelationNode[1]=pDependNode1!=NULL?pDependNode1->handle:0;
		CLDSNode* pDependNode2=GetLdsNodeFromId(pParaNode->attachNodeArr[2]);
		pLdsNode->arrRelationNode[2]=pDependNode2!=NULL?pDependNode2->handle:0;
		pHid=m_pStdTempl->hashMatchRodHid.GetValue(pParaNode->keyLabelFatherRod);
		pLdsNode->hFatherPart=pLdsNode->arrRelationPole[0]=pHid!=NULL?*pHid:0;
		CLDSPlane plane,*pPlane;
		plane.SetBelongModel(this->m_pModel);
		plane.hPointArr[0]=HANDLEPOINT(pLdsNode->arrRelationNode[0],1);
		plane.hPointArr[1]=HANDLEPOINT(pLdsNode->arrRelationNode[1],1);
		plane.hPointArr[2]=HANDLEPOINT(pLdsNode->arrRelationNode[2],1);
		plane.UpdatePlane(m_pModel);
		if((pPlane=m_pModel->MatchDatumPlane(plane.xPickPoint,plane.vStdNormal))==NULL)
		{
			pPlane=Ta.AppendPlane();
			memcpy(pPlane->hPointArr,plane.hPointArr,sizeof(HANDLEPOINT)*3);
			pPlane->UpdatePlane(m_pModel);
		}
		if(pPlane)
			pLdsNode->hRelationPlane=pPlane->handle;
		if(pParaNode->posx_expression[0]!=0)
			pParaNode->xPosition.x=m_xExpression.SolveExpression(pParaNode->posx_expression);
		//if(pParaNode->posy_expression[0]!=0)
		//	pParaNode->xPosition.y=m_xExpression.SolveExpression(pParaNode->posy_expression);
		if(pParaNode->posz_expression[0]!=0)
			pParaNode->xPosition.z=m_xExpression.SolveExpression(pParaNode->posz_expression);
		GEPOINT pos=acs.TransPFromCS(pParaNode->xPosition);
		pLdsNode->SetPosition(pos);
		if(pLdsNode->arrRelationNode[0]==0||pLdsNode->arrRelationNode[1]==0||pLdsNode->arrRelationNode[2]==0)
			pParaNode->m_bNeedUpdateRelaInfo=true;
		else
			pLdsNode->CalPosition();
	}
	if(bCreateNewNode&&m_pModel->IsTowerModel())
	{
		CLDSNode* pLegacyNode=NULL;
		for(CLDSNode* pLegacyNode=m_pModel->EnumNodeFirst();pLegacyNode;pLegacyNode=m_pModel->EnumNodeNext())
		{
			if(pLegacyNode==pLdsNode||!pLegacyNode->ModuleTogetherWith(pLdsNode))
				continue;	//�뵱ǰ�ڵ㲻������ͬһģ��
			if(!pLegacyNode->xOriginalPos.IsEqual(pLdsNode->xOriginalPos))
				continue;
			//�Ѵ����ص��ڵ�
			m_pModel->DeleteNode(pLdsNode->handle);
			pParaNode->pRelaNode=pLdsNode=pLegacyNode;
			break;
		}
	}
	return pLdsNode;
}
bool CStdComModel::UpdateAngleWingDirection(CStdTemplParaAngle* pParaAngle)
{
	CSuperSmartPtr<CLDSLineAngle> pSmartAngle=(CLDSLineAngle*)pParaAngle->pRelaRod;
	if (pParaAngle->blShortAngle)
	{
		pSmartAngle->cal_x_wing_norm();
		pSmartAngle->cal_y_wing_norm();
		return false;
	 }
	if(pSmartAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		if(pSmartAngle->size_cSubClassType=='D'&&pParaAngle->ciSectStyle=='T')
			pSmartAngle->size_cSubClassType='T';
		else if(pSmartAngle->size_cSubClassType=='T'&&pParaAngle->ciSectStyle=='D')
			pSmartAngle->size_cSubClassType='D';
		else if(pSmartAngle->size_cSubClassType!=pParaAngle->ciSectStyle)
			logerr.Log("error");
		pSmartAngle.pGroupAngle->jg_space = pParaAngle->wiGroupAngleSpace;
		if(!pParaAngle->bReverseStartEnd)
			pSmartAngle.pGroupAngle->ciMasterIndex=pParaAngle->ciMasterIndex;
		else if(pSmartAngle->size_cSubClassType=='D'||pSmartAngle->size_cSubClassType=='T')
			pSmartAngle.pGroupAngle->ciMasterIndex=1-pParaAngle->ciMasterIndex;
		pSmartAngle.pGroupAngle->ciMasterDirectIndex=0;	//TODO:��ʱ����
	}
	DESIGN_JGWING_NORM* pWorkWing=NULL,*pOtherWing=NULL;
	DESIGN_JGWING_NORM xBakupOtherWing;
	if((!pParaAngle->bReverseWingX_Y&&(pParaAngle->ciAngleLayout==1||pParaAngle->ciAngleLayout==3))||
		(pParaAngle->bReverseWingX_Y&&(pParaAngle->ciAngleLayout==2||pParaAngle->ciAngleLayout==4)))
	{	//����֫ΪX֫
		pWorkWing=&pSmartAngle->des_norm_x;
		pOtherWing=&pSmartAngle->des_norm_y;
	}
	else// if(( !pParaAngle->bReverseWingX_Y&&(pParaAngle->ciAngleLayout==1||pParaAngle->ciAngleLayout==3))||
		//		(pParaAngle->bReverseWingX_Y&&(pParaAngle->ciAngleLayout==2||pParaAngle->ciAngleLayout==4)))
	{	//����֫ΪY֫
		pWorkWing=&pSmartAngle->des_norm_y;
		pOtherWing=&pSmartAngle->des_norm_x;
	}
	GEPOINT vViewNorm(0,1,0);
	if(pParaAngle->normal.ciNormalDirection=='X')
		vViewNorm=acs.TransVFromCS(GEPOINT(1,0,0));
	else if(pParaAngle->normal.ciNormalDirection=='x')
		vViewNorm=acs.TransVFromCS(GEPOINT(-1,0,0));
	else if(pParaAngle->normal.ciNormalDirection=='Y')
		vViewNorm=acs.TransVFromCS(GEPOINT(0,1,0));
	else if(pParaAngle->normal.ciNormalDirection=='y')
		vViewNorm=acs.TransVFromCS(GEPOINT(0,-1,0));
	else if(pParaAngle->normal.ciNormalDirection=='Z')
		vViewNorm=acs.TransVFromCS(GEPOINT(0,0,1));
	else if(pParaAngle->normal.ciNormalDirection=='z')
		vViewNorm=acs.TransVFromCS(GEPOINT(0,0,-1));
	else
		vViewNorm=acs.axis_z;
	if (fabs(pSmartAngle.pAngle->vxWingNorm*vViewNorm)>fabs(pSmartAngle.pAngle->vyWingNorm*vViewNorm))
		xBakupOtherWing=pSmartAngle->des_norm_y;
	else
		xBakupOtherWing=pSmartAngle->des_norm_x;
	if(pParaAngle->ciAngleLayout==1||pParaAngle->ciAngleLayout==4)
		pWorkWing->near_norm=-vViewNorm;
	else
		pWorkWing->near_norm=vViewNorm;
	CLDSLinePart* pViceRod=GetLdsRodFromId(pParaAngle->normal.uidRefRodOfWorkWing);
	if(pViceRod)
		pWorkWing->hViceJg=pViceRod->handle;
	//else
	//	pParaAngle->m_bNeedUpdateRelaInfo=true;
	pWorkWing->bSpecific=FALSE;
	pWorkWing->bByOtherWing=FALSE;
	if (pParaAngle->normal.blSyncOtherAngleLeg)
	{
		pOtherWing->bSpecific=FALSE;
		pOtherWing->bByOtherWing=TRUE;
	}
	else
	{
		*pOtherWing=xBakupOtherWing;//TODO:δ�������X/Y֫������� wjh-2019.9.15
	}
	pSmartAngle->cal_x_wing_norm();
	pSmartAngle->cal_y_wing_norm();
	return true;
}
CLDSLinePart* CStdComModel::AddOrUpdateParaModelRod(CStdTemplParaRod* pParaRod)
{
	CLDSLinePart* pRod=pParaRod->pRelaRod;
	SmartRodPtr pSmartRod=pRod;
	BOOL bNewRod=(pRod==NULL)?TRUE:FALSE;
	if(bNewRod)
		pParaRod->pRelaRod=pRod=(CLDSLinePart*)m_pModel->AppendPart(pParaRod->m_idPartClsType);
	pParaRod->m_bNeedUpdateRelaInfo=false;
	if(strlen(pParaRod->sPreferLayer)>0)
		pRod->SetLayer(pParaRod->sPreferLayer);
	pRod->cMaterial=pParaRod->cMaterial;
	if (pRod->cMaterial==0)
		pRod->cMaterial=CSteelMatLibrary::Q345BriefMark();
	CLDSNode* pNodeS=GetLdsNodeFromId(pParaRod->idStartNode);
	CLDSNode* pNodeE=GetLdsNodeFromId(pParaRod->idEndNode);
	if(pNodeS && pNodeE)
	{
		pRod->SetStart(pNodeS->Position());	
		pRod->SetEnd(pNodeE->Position());
		pRod->pStart=pNodeS;
		pRod->pEnd=pNodeE;
		pRod->iSeg=pNodeS->iSeg;
		pRod->cfgword=pNodeS->cfgword&pNodeE->cfgword;
	}
	else if((pNodeS==NULL||pNodeE==NULL)&&pParaRod->m_idPartClsType==CLS_LINEANGLE)
	{	//��������̽Ǹ�
		CStdTemplParaAngle* pParaAngle=(CStdTemplParaAngle*)pParaRod;
		pParaAngle->blShortAngle=true;
		CLDSLineAngle* pAngle=(CLDSLineAngle*)pRod;
		CStdTemplParaAngle* pBaseParaAngle=(CStdTemplParaAngle*)m_pStdTempl->listParaRods.GetValue(pParaAngle->attachangle.idBaseAngle);
		CStdTemplParaAngle* pEndTipBaseParaAngle=NULL;
		if(pParaAngle->attachangle.idEndTipBaseAngle>0)
			pEndTipBaseParaAngle=(CStdTemplParaAngle*)m_pStdTempl->listParaRods.GetValue(pParaAngle->attachangle.idEndTipBaseAngle);
		if (pEndTipBaseParaAngle==NULL)
			pEndTipBaseParaAngle=pBaseParaAngle;
		CStdTemplParaNode* pBaseParaNode =m_pStdTempl->listParaNodes.GetValue(pParaAngle->attachangle.idBaseNode);
		CLDSLineAngle* pBaseAngle=(CLDSLineAngle*)GetLdsRodFromId(pParaAngle->attachangle.idBaseAngle);
		CLDSNode* pBaseNode=(CLDSNode*)GetLdsNodeFromId(pParaAngle->attachangle.idBaseNode);
		pAngle->cfgword = pBaseAngle->cfgword;
		pAngle->desStartPos.jgber_cal_style=pAngle->desEndPos.jgber_cal_style=0;
		pAngle->desStartPos.spatialOperationStyle=pAngle->desEndPos.spatialOperationStyle=0;
		char ciDatumWing=pParaAngle->attachangle.ciDatumAngleWing;
		if(ciDatumWing=='X'||ciDatumWing=='x'||ciDatumWing=='Y'||ciDatumWing=='y')
		{
			pAngle->desStartPos.datumPoint.datum_pos_style=2;
			if (pBaseNode == NULL)
				logerr.Log("error");
			else
				pAngle->desStartPos.datumPoint.des_para.RODNODE.hNode=pBaseNode->handle;
			if (pBaseAngle == NULL)
			{
				logerr.Log("error");
				return NULL;
			}
			else
				pAngle->desStartPos.datumPoint.des_para.RODNODE.hRod=pBaseAngle->handle;
			pAngle->desStartPos.datumPoint.des_para.RODNODE.offset_wing=pParaAngle->attachangle.ciDatumAngleWing=='X'?0:1;
			if (pBaseParaAngle->bReverseWingX_Y)
				pAngle->desStartPos.datumPoint.des_para.RODNODE.offset_wing=1-pAngle->desStartPos.datumPoint.des_para.RODNODE.offset_wing;
			pAngle->desStartPos.datumPoint.des_para.RODNODE.wing_offset_style=4;
			pAngle->desStartPos.datumPoint.des_para.RODNODE.wing_offset_dist=0;
			pAngle->desEndPos.datumPoint=pAngle->desStartPos.datumPoint;
			if (pParaAngle->attachangle.idEndTipBaseNode!=0&&
				pParaAngle->attachangle.idEndTipBaseNode!=pParaAngle->attachangle.idBaseNode)
			{	//����ϳ��̽Ǹֵ��ն˶�����λ�ڵ�
				CLDSNode* pEndBaseNode=(CLDSNode*)GetLdsNodeFromId(pParaAngle->attachangle.idEndTipBaseNode);
				if (pEndBaseNode)
					pAngle->desEndPos.datumPoint.des_para.RODNODE.hNode=pEndBaseNode->handle;
			}
			pParaAngle->desStart.cTouchLineType=pParaAngle->desEnd.cTouchLineType=4;
			if (pParaAngle->attachangle.ciAngleLayout==2||pParaAngle->attachangle.ciAngleLayout==3)
			{
				pAngle->desEndPos.datumPoint.des_para.RODNODE.wing_offset_style=4;
				pAngle->desEndPos.datumPoint.des_para.RODNODE.wing_offset_dist=pBaseAngle->size_wide;
			}
			else
			{
				pAngle->desStartPos.datumPoint.des_para.RODNODE.len_offset_dist=-pBaseAngle->size_wide;
				pAngle->desEndPos.datumPoint.des_para.RODNODE.len_offset_dist=pBaseAngle->size_wide;
			}
			pAngle->desStartPos.wing_x_offset.gStyle=pAngle->desEndPos.wing_x_offset.gStyle=4;
			pAngle->desStartPos.wing_x_offset.offsetDist=pAngle->desEndPos.wing_x_offset.offsetDist=0;
			pAngle->desStartPos.wing_y_offset=pAngle->desStartPos.wing_x_offset;
			pAngle->desEndPos.wing_y_offset=pAngle->desEndPos.wing_x_offset;
		}
		else if(ciDatumWing=='O'||ciDatumWing=='I')
		{
			pAngle->desStartPos.datumPoint.datum_pos_style=7;	//�������
			if (pBaseAngle == NULL)
			{
				logerr.Log("error");
				return NULL;
			}
			else
				pAngle->desStartPos.datumPoint.des_para.RODEND.hRod=pBaseAngle->handle;
			if (pBaseAngle->pStart==pBaseNode)
				pAngle->desStartPos.datumPoint.des_para.RODEND.direction=0;
			else
				pAngle->desStartPos.datumPoint.des_para.RODEND.direction=1;
			pAngle->desStartPos.datumPoint.des_para.RODNODE.wing_offset_style=4;
			if (ciDatumWing=='O')
				pAngle->desStartPos.datumPoint.des_para.RODEND.wing_offset_dist=0;
			else
				pAngle->desStartPos.datumPoint.des_para.RODEND.wing_offset_dist=-pBaseAngle->size_thick;
			CXhString szNormalOffset(pAngle->desStartPos.datumPoint.des_para.RODEND.norm_offset,51);
			szNormalOffset.Printf("0x%X",pAngle->handle);
			pAngle->desStartPos.datumPoint.des_para.RODEND.len_offset_dist=pParaAngle->attachangle.siLenStartTipPos;
			pAngle->desEndPos.datumPoint=pAngle->desStartPos.datumPoint;

			CLDSLineAngle* pEndTipBaseAngle=NULL;
			if(pEndTipBaseParaAngle&&pEndTipBaseParaAngle!=pBaseParaAngle)
				pEndTipBaseAngle=(CLDSLineAngle*)GetLdsRodFromId(pEndTipBaseParaAngle->Id);
			if (pEndTipBaseAngle==NULL)
				pEndTipBaseAngle=pBaseAngle;
			CLDSNode* pEndBaseNode=NULL;
			if (pParaAngle->attachangle.idEndTipBaseNode!=0&&
				pParaAngle->attachangle.idEndTipBaseNode!=pParaAngle->attachangle.idBaseNode)
			{	//����ϳ��̽Ǹֵ��ն˶�����λ�ڵ�
				pEndBaseNode=(CLDSNode*)GetLdsNodeFromId(pParaAngle->attachangle.idEndTipBaseNode);
			}
			if (pEndBaseNode==NULL)
				pEndBaseNode=pBaseNode;
			pAngle->desEndPos.datumPoint.des_para.RODEND.hRod=pEndTipBaseAngle->handle;
			if(pEndTipBaseAngle->pStart==pEndBaseNode)
				pAngle->desEndPos.datumPoint.des_para.RODEND.direction=0;
			else
				pAngle->desEndPos.datumPoint.des_para.RODEND.direction=1;
			pAngle->desEndPos.datumPoint.des_para.RODEND.wing_offset_style=4;
			if (ciDatumWing=='O')
				pAngle->desEndPos.datumPoint.des_para.RODEND.wing_offset_dist=0;
			else
				pAngle->desEndPos.datumPoint.des_para.RODEND.wing_offset_dist=-pBaseAngle->size_thick;
			pAngle->desEndPos.datumPoint.des_para.RODEND.len_offset_dist=pParaAngle->attachangle.siLenEndTipPos;
			pAngle->desStartPos.wing_x_offset.SetOffset(0,4);
			pAngle->desStartPos.wing_y_offset=pAngle->desStartPos.wing_x_offset;
			pAngle->desEndPos.wing_x_offset=pAngle->desEndPos.wing_y_offset=pAngle->desStartPos.wing_x_offset;
		}
		pAngle->CalPosition();	//��Ϊ��ʱĬ��λ���������ش���ģ�������Ҫ�ڼ���֫����ƫ��ǰ���Ȱ��ջ���λ�ü���һ�����ƽǸֶ˵�λ�� wjh-2019.3.25

		DESIGN_JGWING_NORM* pWorkWing=&pAngle->des_norm_x,*pOtherWing=&pAngle->des_norm_y;
		if (pParaAngle->attachangle.ciAngleLayout==4)
		{
			if(pBaseAngle->pStart==pBaseNode)
			{
				pWorkWing =&pAngle->des_norm_y;
				pOtherWing=&pAngle->des_norm_x;
			}
			pWorkWing->bSpecific=true;
			pWorkWing->hViceJg=pBaseAngle->handle;
			pWorkWing->spec_norm.norm_style=1;
			pWorkWing->spec_norm.hVicePart=pBaseAngle->handle;
			if (pBaseParaAngle->bReverseWingX_Y)
				pWorkWing->spec_norm.norm_wing=1;
			else
				pWorkWing->spec_norm.norm_wing=0;
			pWorkWing->spec_norm.direction=0;
			*pOtherWing=*pWorkWing;
			pOtherWing->spec_norm.norm_wing=1-pWorkWing->spec_norm.norm_wing;
		}
		else
		{
			if (pParaAngle->attachangle.ciFixWorkWing=='Y')
			{
				pWorkWing =&pAngle->des_norm_y;
				pOtherWing=&pAngle->des_norm_x;
				pAngle->desStartPos.wing_x_offset.offsetDist=pAngle->desEndPos.wing_x_offset.offsetDist=-50;
			}
			else
				pAngle->desStartPos.wing_y_offset.offsetDist=pAngle->desEndPos.wing_y_offset.offsetDist=-50;
			pWorkWing->bSpecific=true;
			pWorkWing->hViceJg=pBaseAngle->handle;
			pWorkWing->spec_norm.norm_style=1;
			pWorkWing->spec_norm.hVicePart=pBaseAngle->handle;
			pWorkWing->spec_norm.norm_wing=pAngle->desStartPos.datumPoint.des_para.RODNODE.offset_wing;
			pWorkWing->spec_norm.direction=1;	//�����Ǹ�����֫�����������Ǹ�֫�����෴
			if(pParaAngle->attachangle.ciAssistWing==0)
				pOtherWing->bByOtherWing=true;
			else
			{	//ָ���ǹ���֫�ķ���
				pOtherWing->bSpecific = true;
				pOtherWing->hViceJg = pBaseAngle->handle;
				pOtherWing->spec_norm.norm_style = 1;
				pOtherWing->spec_norm.hVicePart = pBaseAngle->handle;
				char ciAngleWing=pParaAngle->attachangle.ciAssistWing;
				if (ciAngleWing>='a'&&ciAngleWing<='z')
				{
					pOtherWing->spec_norm.direction = 1;
					ciAngleWing=toupper(ciAngleWing);
				}
				else
					pOtherWing->spec_norm.direction = 0;
				pOtherWing->spec_norm.norm_wing =  ciAngleWing== 'X' ? 0 : 1;
				if (pBaseParaAngle->bReverseWingX_Y)
					pOtherWing->spec_norm.norm_wing=1-pOtherWing->spec_norm.norm_wing;
			}
		}
	}
	//��ʼ���˼���λ����
	if(pParaRod->m_idPartClsType==CLS_LINEANGLE)
	{
		CStdTemplParaAngle* pParaAngle=(CStdTemplParaAngle*)pParaRod;
		CLDSLineAngle* pAngle=(CLDSLineAngle*)pRod;
		//if(pAngle->pStart==NULL || pAngle->pEnd==NULL)
		//{
		//	pParaRod->m_bNeedUpdateRelaInfo=true;
		//	return pRod;
		//}
		if(pParaRod->m_sGuige.GetLength()>0)
		{
			double wing_wide,wing_thick;
			restore_JG_guige(pParaRod->m_sGuige,wing_wide,wing_thick);
			pAngle->set_wing_wide(wing_wide);
			pAngle->set_wing_thick(wing_thick);
		}
		pAngle->desStartOdd.m_iOddCalStyle=pParaAngle->wiStartOddCalStyle;
		pAngle->desEndOdd.m_iOddCalStyle=pParaAngle->wiEndOddCalStyle;
		if(pAngle->pStart!=NULL&&pAngle->pEnd!=NULL)
		{	//�Ǹ�֫����
			GEPOINT vViewNorm(0,1,0);
			if(pParaAngle->normal.ciNormalDirection=='X')
				vViewNorm.Set(1,0,0);
			else if(pParaAngle->normal.ciNormalDirection=='x')
				vViewNorm.Set(-1,0,0);
			else if(pParaAngle->normal.ciNormalDirection=='Y')
				vViewNorm.Set(0, 1,0);
			else if(pParaAngle->normal.ciNormalDirection=='y')
				vViewNorm.Set(0,-1,0);
			else if(pParaAngle->normal.ciNormalDirection=='Z')
				vViewNorm.Set(0,0, 1);
			else if(pParaAngle->normal.ciNormalDirection=='z')
				vViewNorm.Set(0,0,-1);
			BYTE ciAngleLayout=pParaAngle->ciAngleLayout;
			if(m_xNearAxisZ*acs.axis_z<0)
			{
				ciAngleLayout=5-pParaAngle->ciAngleLayout;
				if(pParaAngle->normal.ciNormalDirection=='z')
					vViewNorm.Set(0,0, 1);
				else if(pParaAngle->normal.ciNormalDirection=='Z')
					vViewNorm.Set(0,0,-1);
			}
			vViewNorm=acs.TransVFromCS(vViewNorm);
			DESIGN_JGWING_NORM* pWorkWing=NULL,*pOtherWing=NULL;
			if(ciAngleLayout==1||ciAngleLayout==3)
			{	//����֫ΪX֫
				pWorkWing=&pAngle->des_norm_x;
				pOtherWing=&pAngle->des_norm_y;
			}
			else //if(ciAngleLayout==2||ciAngleLayout==4)
			{	//����֫ΪY֫
				pWorkWing=&pAngle->des_norm_y;
				pOtherWing=&pAngle->des_norm_x;
			}
			if(ciAngleLayout==1||ciAngleLayout==4)
				pWorkWing->near_norm=-vViewNorm;
			else
				pWorkWing->near_norm=vViewNorm;
			CLDSLinePart* pViceRod=GetLdsRodFromId(pParaAngle->normal.uidRefRodOfWorkWing);
			if(pViceRod)
				pWorkWing->hViceJg=pViceRod->handle;
			else
				pParaRod->m_bNeedUpdateRelaInfo=true;
			pWorkWing->bSpecific=FALSE;
			pWorkWing->bByOtherWing=FALSE;
			pOtherWing->bSpecific=FALSE;
			pOtherWing->bByOtherWing=TRUE;
		}
		pAngle->cal_x_wing_norm();
		pAngle->cal_y_wing_norm();
		pAngle->CalPosition();
		//���ýǸ���˨
		if(bNewRod && pParaAngle->m_layoutBoltArr.Count>0)
		{
			int nMd=(int)m_xExpression.SolveExpression("Md");
			int nBolt=(int)m_xExpression.SolveExpression("BN");
			int nBoltSpace=(int)m_xExpression.SolveExpression("BS");
			for(UINT iLayout=0;iLayout<pParaAngle->m_layoutBoltArr.Count;iLayout++)
			{
				BOLT_LAYOUT* pLayout=pParaAngle->m_layoutBoltArr.GetAt(iLayout);
				JGZJ jgzj=pAngle->GetAngleZJ(pAngle->size_wide);
				short g[4]={(short)jgzj.g,(short)jgzj.g1,(short)jgzj.g2,(short)jgzj.g3};
				UINT uiBoltDia=(pLayout->ciFlowDirection==BOLT_LAYOUT::FLOW_StoE)?pAngle->connectStart.d:pAngle->connectEnd.d;
				if(nMd>0)	//�ͻ�������Ӧ�����û�ָ����˨ֱ�� wjh-2018.10.2
					uiBoltDia=nMd;
				LSSPACE_STRU LsSpace;
				GetLsSpace(LsSpace,uiBoltDia);
				if(pLayout->cnCount==0)
					pLayout->cnCount=(nBolt>0)?nBolt:2;
				if(nBoltSpace>LsSpace.SingleRowSpace)
					LsSpace.SingleRowSpace=nBoltSpace;
				CXhSimpleList<BOLT_LOCATION> listLocations;
				pLayout->LayoutBolts(g,&listLocations,LsSpace.EndSpace,LsSpace.SingleRowSpace,LsSpace.doubleRowSpace);
				//������˨�������˨�����Ϣ
				BYTE ciWorkWing=pLayout->ciDatumWing-'X';
				if(m_xNearAxisZ*acs.axis_z<0)
					ciWorkWing=1-ciWorkWing;
				for(BOLT_LOCATION* pLocation=listLocations.EnumObjectFirst();pLocation;pLocation=listLocations.EnumObjectNext())
				{
					CLDSBolt* pBolt=(CLDSBolt*)m_pModel->AppendPart(CLS_BOLT);
					pBolt->d=uiBoltDia;
					pBolt->des_base_pos.hPart=pRod->handle;
					double datumpoint_offset_g=0;
					if(pLocation->basepoint.ciPosCalStyle==0)
					{
						pBolt->des_base_pos.datumPoint.datum_pos_style=1;
						pBolt->des_base_pos.datumPoint.des_para.RODEND.hRod=pRod->handle;
						pBolt->des_base_pos.datumPoint.des_para.RODEND.direction=pLocation->ciDirection;
						pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=ciWorkWing;
						pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist=0;
						pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=4;
						pBolt->des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect=true;
					}
					else if(pLocation->basepoint.ciPosCalStyle==1)
					{
						pBolt->des_base_pos.datumPoint.datum_pos_style=2;
						pBolt->des_base_pos.datumPoint.des_para.RODNODE.hRod=pRod->handle;
						pBolt->des_base_pos.datumPoint.des_para.RODNODE.direction=pLocation->ciDirection;
						CLDSNode* pLocateNode=this->GetLdsNodeFromId(pLocation->basepoint.widBaseNodeOrBolt);
						pBolt->des_base_pos.datumPoint.des_para.RODNODE.hNode=pLocateNode->handle;
						pBolt->des_base_pos.datumPoint.des_para.RODNODE.offset_wing=ciWorkWing;
						pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style=0;
						datumpoint_offset_g=g[0];
					}
					pBolt->des_base_pos.len_offset_dist=pLocation->siLenOffset;
					pBolt->des_base_pos.wing_offset_dist=pLocation->siWingOffset-datumpoint_offset_g;
					pBolt->des_base_pos.offset_wing=ciWorkWing;
					pBolt->des_work_norm.norm_style=1;
					pBolt->des_work_norm.hVicePart=pRod->handle;
					pBolt->des_work_norm.norm_wing=ciWorkWing;
					pBolt->des_work_norm.direction=0;
					pBolt->AddL0Thick(pRod->handle,TRUE);
					pBolt->CalGuigeAuto();
					pBolt->correct_worknorm();
					pBolt->correct_pos();
					pAngle->AppendLsRef(pBolt->GetLsRef());
					//
					CLDSLinePart* pOtherRod=GetLdsRodFromId(pLayout->basepoint.widCrossAngle);
					if(pOtherRod)
					{
						pBolt->AddL0Thick(pOtherRod->handle,TRUE);
						pOtherRod->AppendLsRef(pBolt->GetLsRef());
					}
				}
			}
		}
	}
	else if(pParaRod->m_idPartClsType==CLS_LINESLOT)
	{
		CStdTemplParaSlot* pParaSlot=(CStdTemplParaSlot*)pParaRod;
		CLDSLineSlot* pSlot=(CLDSLineSlot*)pRod;
		CLDSLinePart* pBaseRod=GetLdsRodFromId(pParaSlot->wiAttachTube);
		CLDSNode* pBaseNode=GetLdsNodeFromId(pParaSlot->datum_pos.wiAttachNode);
		if(pBaseRod==NULL ||pBaseNode==NULL)
			return FALSE;
		pBaseRod->BuildUCS();
		pSlot->iSeg=pBaseRod->iSeg;
		pSlot->SetLayer(pBaseRod->layer());
		pSlot->cMaterial = pParaSlot->cMaterial;
		_snprintf(pSlot->spec,MAX_CHAR_GUIGE_16,"%s",(char*)pParaSlot->m_sGuige);
		SLOT_PARA_TYPE *pSlotPara=FindSlotType(pParaSlot->m_sGuige);
		if(pSlotPara!=NULL)
		{
			pSlot->size_wide=pSlotPara->b;	//��
			pSlot->size_height=pSlotPara->h;//�����
			pSlot->size_thick=pSlotPara->t;	//��
		}
		pSlot->fLen=pParaSlot->fLen;
		pSlot->hAttachTube=pBaseRod->handle;
		pSlot->desDatumPoint.datum_pos_style=11;
		pSlot->desDatumPoint.des_para.COLUMN_INTERS.hLineTube=pBaseRod->handle;
		//������淨��(ֱ��ָ��)
		if(pParaSlot->datum_pos.ciNormalStyle==0)
			pSlot->desDatumPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm.Set(0,0,-1);
		else
		{
			GEPOINT vzNormal = pBaseRod->xPosEnd - pBaseRod->xPosStart;
			normalize(vzNormal);
			pSlot->desDatumPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm = vzNormal;
		}
		//�������Բ��(����������)
		pSlot->desDatumPoint.des_para.COLUMN_INTERS.cPointAndVectorFlag|=0x02;
		pSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=pBaseNode->handle;
		pSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.len_offset=pParaSlot->datum_pos.len_offset;	
		//�������߷���
		pSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style=pParaSlot->datum_pos.ciRotFromEdgeStyle;
		pSlot->desDatumPoint.des_para.COLUMN_INTERS.RayVector.ray_vec=pParaSlot->datum_pos.ray_vec;
		pSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_angle=pParaSlot->datum_pos.ray_angle;
		//
		pSlot->desStartPos.endPosType=0;
		pSlot->desEndPos.endPosType=0;
		pSlot->CalPosition();
	}
	return pRod;
}
void CStdComModel::LayoutConnRodBolts(CStdTemplParaPlate* pParaPlate, GEPOINT* pWorkPlaneNormal)
{
	CLDSNode* pBaseNode = pParaPlate ? GetLdsNodeFromId(pParaPlate->design.idBaseNode) : NULL;
	CLDSLinePart* pBaseRod = pParaPlate ? GetLdsRodFromId(pParaPlate->design.idBaseRod) : NULL;
	if (pParaPlate == NULL || pBaseNode == NULL || pBaseRod == NULL)
		return;
	long* pHandle = m_pStdTempl->hashMatchPlates.GetValue(pParaPlate->Id);
	CLDSPlate* pPlate = (CLDSPlate*)Ta.FromPartHandle(*pHandle, CLS_PLATE);
	if (pPlate == NULL)
		return;
	//������ʱ����
	CStdTemplParaRod* pParaRod = NULL;
	int nConnRod = (int)pParaPlate->connrods.Count;
	for (int i = 0; i < nConnRod; i++)
	{
		CONNECT_ROD* pConnRod = pParaPlate->connrods.GetAt(i);
		pParaRod = m_pStdTempl->listParaRods.GetValue(pConnRod->sidRod);
		if (pParaRod)
			pParaRod->cnLiveShareBoltN = 0;	//������ʱ����
	}
	for (pParaRod = this->m_pStdTempl->listParaRods.GetFirst(); pParaRod; pParaRod = m_pStdTempl->listParaRods.GetNext())
		pParaRod->cnLiveShareBoltN = 0;	//��ֹһЩ�����Ǹֲ���(���⣩����
	//���ø˼���˨
	CXhSimpleList<BOLTREF> listExistBolts;
	for (int i = 0; i < nConnRod; i++)
	{
		CONNECT_ROD* pConnRod = pParaPlate->connrods.GetAt(i);
		pParaRod = m_pStdTempl->listParaRods.GetValue(pConnRod->sidRod);	//�˼��̱�ʶ����ָ����ϽǸ�
		CLDSLinePart* pRod = GetLdsRodFromId(pConnRod->idRod);	//�˼�����ʶһ��ָ�򵥽Ǹֻ���ϽǸֵ��ӽǸ�
		if (pParaRod == NULL || pRod == NULL)
			continue;
		CLDSGroupLineAngle* pGroupJg = NULL;
		if (pRod->GetClassTypeId() == CLS_GROUPLINEANGLE)
		{	//��ϽǸ�ʱ��ȡ��׼�ӽǸ���Ϊ��˨��λ�˼�
			pGroupJg = (CLDSGroupLineAngle*)pRod;
			pRod = Ta.FromRodHandle(pGroupJg->son_jg_h[pGroupJg->ciMasterIndex]);
		}
		SmartRodPtr pSmartRod = pRod;
		//��ʼ����˨λ����Ϣ������֫+��ƶˣ�
		BYTE ciXnY = 'X' + 'Y';
		char ciConnectWing = pConnRod->ciConnectWing;
		GEPOINT vCurrWorkPlaneNorm = pWorkPlaneNormal[min(pConnRod->ciPlaneNo - 1, 2)];
		if (pConnRod->ciConnectWing == 0 && pRod->IsAngle())
		{	//�Զ��жϽǸֵ�ǰ��������֫
			double ddx = pSmartRod.pAngle->vxWingNorm*vCurrWorkPlaneNorm;
			double ddy = pSmartRod.pAngle->vyWingNorm*vCurrWorkPlaneNorm;
			ciConnectWing = fabs(ddx) > fabs(ddy) ? 'X' : 'Y';
		}
		else if (pParaRod->bReverseWingX_Y)
			ciConnectWing = ciXnY - ciConnectWing;
		bool bCurrTipIsStart = (pConnRod->ciCurrTipSideS0E1 == 0);
		if (pRod->pStart == pBaseNode)
			bCurrTipIsStart = true;
		else if (pRod->pEnd == pBaseNode)
			bCurrTipIsStart = false;
		else if (pParaRod->bReverseStartEnd)
			bCurrTipIsStart = !bCurrTipIsStart;
		//��ʼ��������Ϣ(����+ֱ��)
		UINT nBolt = bCurrTipIsStart ? pRod->wnStartConnBoltN : pRod->wnEndConnBoltN;
		UINT uiBoltDia = bCurrTipIsStart ? pRod->connectStart.d : pRod->connectEnd.d;
		if(pConnRod->ciBoltNCalMode<0)
			nBolt = bCurrTipIsStart ? pRod->connectStart.wnDualShearConnBoltN : pRod->connectEnd.wnDualShearConnBoltN;
		if (pConnRod->ciCurrTipSideS0E1 > 1 || (pConnRod->blCalBoltNByRayRodsOnBaseRod&&
			pParaPlate->design.idBaseRod == pConnRod->sidRod&&pConnRod->dualSideEdge))
		{	//�м�����
			UINT uiKeyBoltNum = 1, uiKeyBoltDia = 16;
			CONNBOLTS basebolts(0, 0, (int)pRod->Thick);
			if (pConnRod->ciCurrTipSideS0E1 == 0)
			{
				uiKeyBoltNum = nBolt = pRod->wnStartConnBoltN;
				uiKeyBoltDia = uiBoltDia = pRod->connectStart.d;
				basebolts.Init(uiBoltDia, nBolt, (int)pRod->Thick, pRod->connectStart.grade);
			}
			else if (pConnRod->ciCurrTipSideS0E1 == 1)
			{
				uiKeyBoltNum = nBolt = pRod->wnEndConnBoltN;
				uiKeyBoltDia = uiBoltDia = pRod->connectEnd.d;
				basebolts.Init(uiBoltDia, nBolt, (int)pRod->Thick, pRod->connectEnd.grade);
			}
			double maxAvForce = pConnRod->ciCurrTipSideS0E1 < 2 ? nBolt * uiBoltDia*uiBoltDia : 0;
			for (int k = 0; k < (int)pParaPlate->connrods.Count; k++)
			{
				CONNECT_ROD* pRayConnRod = pParaPlate->connrods.GetAt(k);
				if (pConnRod == pRayConnRod)
					continue;	//����
				if (pRayConnRod->ciCurrTipSideS0E1 != 0 && pRayConnRod->ciCurrTipSideS0E1 != 1)
					continue;	//���������Ӳ�
				CLDSLinePart* pRayRod = GetLdsRodFromId(pRayConnRod->sidRod);
				if (pRayRod == NULL)
					continue;
				CStdTemplParaRod* pRayParaRod = m_pStdTempl->listParaRods.GetValue(pRayConnRod->sidRod);
				bool bCurrTipIsStart = (pRayConnRod->ciCurrTipSideS0E1 == 0);
				if (pRayParaRod->bReverseStartEnd)
					bCurrTipIsStart = !bCurrTipIsStart;
				UINT nRayBolt = bCurrTipIsStart ? pRayRod->wnStartConnBoltN : pRayRod->wnEndConnBoltN;
				if (pRayConnRod->ciBoltNCalMode==-1)
				{	//���´���������������˨������˫��(�ڳİ��Ǹ�)����Ǹ�������˨�� wjh-2019.12.12
					nRayBolt = bCurrTipIsStart ? pRayRod->wnStartConnBoltN : pRayRod->wnEndConnBoltN;
					nRayBolt = bCurrTipIsStart ? pRayRod->connectStart.wnDualShearConnBoltN:pRayRod->connectEnd.wnDualShearConnBoltN;
				}
				UINT uRayBoltDia = bCurrTipIsStart ? pRayRod->connectStart.d : pRayRod->connectEnd.d;
				char* szBoltGrade = bCurrTipIsStart ? pRayRod->connectStart.grade : pRayRod->connectEnd.grade;
				nRayBolt = max(nRayBolt, pRayConnRod->cnMinBoltN);
				CONNBOLTS raybolts(uRayBoltDia, (int)ceil(nRayBolt*1.5), (WORD)pRayRod->Thick, szBoltGrade);
				if (basebolts.wiBoltD == 0)
					basebolts = raybolts;
				else if (raybolts.dfCtrlAf > basebolts.dfCtrlAf)
				{
					basebolts = raybolts;
					uiKeyBoltNum = nRayBolt;
					uiKeyBoltDia = uRayBoltDia;
				}
			}
			nBolt = basebolts.wnBoltN;// ftoi(1.5*uiKeyBoltNum);
			uiBoltDia = basebolts.wiBoltD;// uiKeyBoltDia;
		}
		if (nBolt <= pConnRod->cnMinBoltN)
			nBolt = pConnRod->cnMinBoltN;
		//δ��Ӧ֧��ϸ��pConnRod�еĸ�����˨�����Ⲽ�ã�Ŀǰ�ݽ�֧�ּ򵥲���
		JGZJ jgzj;
		jgzj.g = long(pRod->size_wide*0.5); jgzj.g1 = jgzj.g2 = jgzj.g3 = 0;
		if (pRod->IsAngle())
			jgzj = pSmartRod.pAngle->GetAngleZJ(pRod->size_wide);
		long g[4] = { jgzj.g,jgzj.g1,jgzj.g2,jgzj.g3 };
		SHAREANGLE* pShareAngle=this->hashShareAngles.GetValue(pRod->handle);
		if(pShareAngle)
		{
			for(UINT j=0;j<pShareAngle->xarrConnBolts.Count;j++)
			{
				CLDSBolt* pBolt=pShareAngle->xarrConnBolts.At(j);
				pBolt->AddL0Thick(pPlate->handle,TRUE);
				pBolt->cfgword.AddSpecWord(pPlate->cfgword);
				if(pBolt)
					pPlate->AppendLsRef(pBolt->GetLsRef());
			}
		}
		else
		{
			UINT count = 0, summarycount = pParaRod->cnLiveShareBoltN;
			for (UINT iLayout = 0; iLayout < pConnRod->layoutbolts.Count; iLayout++)
			{
				BOLT_LAYOUT* pLayout = pConnRod->layoutbolts.GetAt(iLayout);
				if (pLayout == NULL)
					continue;
				BOLT_LAYOUT layout = *pLayout;
				layout.ciBoldMd = (BYTE)uiBoltDia;
				layout.cnMaxSingularLineBolts = pConnRod->cnMaxSingularLineBolts;
				CXhSimpleList<BOLT_LOCATION> listLocations;
				if ((count = layout.cnCount) == 0)
					layout.cnCount = count = nBolt - summarycount;
				if (layout.ciDatumWing == 0)
					layout.ciDatumWing = ciConnectWing;
				else if (pParaRod->bReverseWingX_Y)
					layout.ciDatumWing = 'Y' - layout.ciDatumWing + 'X';
				if (pParaRod->bReverseStartEnd)
					layout.ciFlowDirection = 1 - layout.ciFlowDirection;
				count = LayoutAngleBolts(pConnRod, layout, &listLocations);
				CStdTemplParaRod* pShareBoltParaRod = NULL;
				if (layout.idDirectConnectToAngle1 > 0)
				{
					if ((pShareBoltParaRod = m_pStdTempl->listParaRods.GetValue(layout.idDirectConnectToAngle1)) != NULL)
						pShareBoltParaRod->cnLiveShareBoltN += count;
				}
				if (layout.idDirectConnectToAngle2 > 0)
				{
					if ((pShareBoltParaRod = m_pStdTempl->listParaRods.GetValue(layout.idDirectConnectToAngle2)) != NULL)
						pShareBoltParaRod->cnLiveShareBoltN += count;
				}

				summarycount += count;
				//������˨�������˨�����Ϣ
				BOLT_LOCATION* pLocation = NULL;
				SmartRodPtr pShareBoltAngle;
				for (pLocation = listLocations.EnumObjectFirst(); pLocation; pLocation = listLocations.EnumObjectNext())
				{
					CLDSBolt* pBolt = (CLDSBolt*)m_pModel->AppendPart(CLS_BOLT);
					pBolt->SetLayer(pBaseRod->szLayer);
					pBolt->cfgword = pBaseRod->cfgword;
					pBolt->d = uiBoltDia;
					pBolt->des_base_pos.hPart = pRod->handle;
					double datumpoint_offset_g = 0;
					if (pLocation->basepoint.ciPosCalStyle == 0)
					{
						pBolt->des_base_pos.datumPoint.datum_pos_style = 1;
						pBolt->des_base_pos.datumPoint.des_para.RODEND.hRod = pRod->handle;
						pBolt->des_base_pos.datumPoint.des_para.RODEND.direction = pLocation->ciDirection;
						if (pLocation->ciDatumWing == 'X')
							pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing = 0;
						else if (pLocation->ciDatumWing == 'Y')
							pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing = 1;
						else
							pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing = ciConnectWing - 'X';
						pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist = 0;
						pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style = 4;
						pBolt->des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect = true;
					}
					else if (pLocation->basepoint.ciPosCalStyle == 1 || pLocation->basepoint.ciPosCalStyle == 2)
					{
						pBolt->des_base_pos.datumPoint.datum_pos_style = 2;
						pBolt->des_base_pos.datumPoint.des_para.RODNODE.hRod = pRod->handle;
						pBolt->des_base_pos.datumPoint.des_para.RODNODE.direction = pLocation->ciDirection;
						CLDSNode* pLocateNode = this->GetLdsNodeFromId(pLocation->basepoint.widBaseNodeOrBolt);
						pBolt->des_base_pos.datumPoint.des_para.RODNODE.hNode = pLocateNode->handle;
						if (pLocation->ciDatumWing == 'X')
							pBolt->des_base_pos.datumPoint.des_para.RODNODE.offset_wing = 0;
						else if (pLocation->ciDatumWing == 'Y')
							pBolt->des_base_pos.datumPoint.des_para.RODNODE.offset_wing = 1;
						else
							pBolt->des_base_pos.datumPoint.des_para.RODNODE.offset_wing = ciConnectWing - 'X';
						pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style = 0;
						datumpoint_offset_g = g[0];
					}
					else if (pLocation->basepoint.ciPosCalStyle == 3)
					{
						pBolt->des_base_pos.datumPoint.datum_pos_style = 3;
						CMPSTID xid = pLocation->basepoint.widBaseAngle==0?pConnRod->idRod:pLocation->basepoint.widBaseAngle;
						CStdTemplParaRod* pBaseParaRod=NULL,*pRefCrossParaRod=NULL;
						CLDSLineAngle* pBaseCrossAngle = NULL,*pRefCrossAngle=NULL;
						if ((pBaseParaRod=m_pStdTempl->listParaRods.GetValue(xid.sidGroupRod))!=NULL)
						{
							if (xid!=pConnRod->idRod)
								pShareBoltAngle=pBaseParaRod->pRelaRod;
							if (xid.sidSerial > 0 && pBaseParaRod->pRelaRod->GetClassTypeId() == CLS_GROUPLINEANGLE)
							{
								CLDSGroupLineAngle* pGroupAngle = (CLDSGroupLineAngle*)pBaseParaRod->pRelaRod;
								if (pBaseParaRod->bReverseMasterAngle&&xid.sidSerial <= 2)
									pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pGroupAngle->son_jg_h[2 - xid.sidSerial];
								else
									pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pGroupAngle->son_jg_h[xid.sidSerial - 1];
								pBaseCrossAngle = (CLDSLineAngle*)m_pModel->FromPartHandle(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1,CLS_LINEANGLE);
							}
							else
							{
								pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pBaseParaRod->pRelaRod->handle;
								if (pBaseParaRod->pRelaRod->GetClassTypeId() == CLS_LINEANGLE)
									pBaseCrossAngle = (CLDSLineAngle*)pBaseParaRod->pRelaRod;
							}
						}
						if(pBaseCrossAngle==NULL)
						{
							logerr.Log("error inters bolt refrod#%d",pLocation->basepoint.widBaseAngle);
							continue;
						}
						xid = pLocation->basepoint.widCrossAngle==0?pConnRod->idRod:pLocation->basepoint.widCrossAngle;
						if ((pRefCrossParaRod = m_pStdTempl->listParaRods.GetValue(xid.sidGroupRod))!=NULL)
						{
							if (xid!=pConnRod->idRod)
								pShareBoltAngle=pRefCrossParaRod->pRelaRod;
							if (xid.sidSerial > 0 && pRefCrossParaRod->pRelaRod->GetClassTypeId() == CLS_GROUPLINEANGLE)
							{
								CLDSGroupLineAngle* pGroupAngle = (CLDSGroupLineAngle*)pRefCrossParaRod->pRelaRod;
								if (pRefCrossParaRod->bReverseMasterAngle&&xid.sidSerial <= 2)
									pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pGroupAngle->son_jg_h[2 - xid.sidSerial];
								else
									pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pGroupAngle->son_jg_h[xid.sidSerial - 1];
								pRefCrossAngle = (CLDSLineAngle*)m_pModel->FromPartHandle(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE);
							}
							else if(pRefCrossParaRod->pRelaRod!=NULL)
							{
								pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pRefCrossParaRod->pRelaRod->handle;
								if (pRefCrossParaRod->pRelaRod->GetClassTypeId() == CLS_LINEANGLE)
									pRefCrossAngle = (CLDSLineAngle*)pRefCrossParaRod->pRelaRod;
							}
						}
						if(pRefCrossAngle==NULL)
						{
							logerr.Log("error inters bolt refrod#%d",pLocation->basepoint.widCrossAngle);
							continue;
						}
						pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1 = min(4, pLocation->basepoint.ciStyleG);
						BYTE ciSafeStyleG=pLocation->basepoint.ciStyleG;
						if (ciSafeStyleG==BOLT_ORIGIN::POS_nS||ciSafeStyleG==BOLT_ORIGIN::POS_WaS)
						{
							if(pBaseCrossAngle&&pRefCrossAngle==pRod)
							{
								GEPOINT lenRayVec=pRod->xPosEnd-pRod->xPosStart;
								if (pRod->pEnd==pBaseNode)
									lenRayVec*=-1.0;
								double jdx=vCurrWorkPlaneNorm*pBaseCrossAngle->vxWingNorm;
								double jdy=vCurrWorkPlaneNorm*pBaseCrossAngle->vyWingNorm;
								GEPOINT vIdleWingNorm=(fabs(jdx)>fabs(jdy))?pBaseCrossAngle->vyWingNorm:pBaseCrossAngle->vxWingNorm;
								double justify=vIdleWingNorm*lenRayVec;
								if (justify>0&&ciSafeStyleG==BOLT_ORIGIN::POS_WaS)	//�����ڽǸ����߲�
									ciSafeStyleG=BOLT_ORIGIN::POS_nS;
								else if(justify<0&&ciSafeStyleG==BOLT_ORIGIN::POS_nS)	//�����ڽǸ�֫���
									ciSafeStyleG=BOLT_ORIGIN::POS_WaS;
							}
						}
						pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 =
							BOLT_ORIGIN::CalWidthByStyleG(pBolt->d, pBaseCrossAngle->size_wide, ciSafeStyleG);
						pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2 = min(4, pLocation->basepoint.ciStyleCrossG);
						ciSafeStyleG=pLocation->basepoint.ciStyleCrossG;
						if (ciSafeStyleG==BOLT_ORIGIN::POS_nS||ciSafeStyleG==BOLT_ORIGIN::POS_WaS)
						{
							if(pRefCrossAngle&&pBaseCrossAngle==pRod)
							{
								GEPOINT lenRayVec=pRod->xPosEnd-pRod->xPosStart;
								if (pLocation->ciDirection==1)//(pRod->pEnd==pBaseNode)
									lenRayVec*=-1.0;
								double jdx=vCurrWorkPlaneNorm*pRefCrossAngle->vxWingNorm;
								double jdy=vCurrWorkPlaneNorm*pRefCrossAngle->vyWingNorm;
								GEPOINT vIdleWingNorm=(fabs(jdx)>fabs(jdy))?pRefCrossAngle->vyWingNorm:pRefCrossAngle->vxWingNorm;
								double justify=vIdleWingNorm*lenRayVec;
								if (justify>0&&ciSafeStyleG==BOLT_ORIGIN::POS_WaS)	//�����ڽǸ����߲�
									ciSafeStyleG=BOLT_ORIGIN::POS_nS;
								else if(justify<0&&ciSafeStyleG==BOLT_ORIGIN::POS_nS)	//�����ڽǸ�֫���
									ciSafeStyleG=BOLT_ORIGIN::POS_WaS;
							}
						}
						pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2 =
							BOLT_ORIGIN::CalWidthByStyleG(pBolt->d, pRefCrossAngle->size_wide, ciSafeStyleG);
						if(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1==pRod->handle)
							datumpoint_offset_g = pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1;
						else if(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2==pRod->handle)
							datumpoint_offset_g = pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2;
					}
					pBolt->des_base_pos.len_offset_dist = pLocation->siLenOffset;
					pBolt->des_base_pos.wing_offset_dist = pLocation->siWingOffset - datumpoint_offset_g;
					pBolt->des_base_pos.direction = pLocation->ciDirection;
					pBolt->des_work_norm.norm_style = 1;
					pBolt->des_work_norm.hVicePart = pRod->handle;
					pBolt->des_work_norm.direction = 0;
					if (pLocation->ciDatumWing == 'X')
						pBolt->des_work_norm.norm_wing = pBolt->des_base_pos.offset_wing = 0;
					else if (pLocation->ciDatumWing == 'Y')
						pBolt->des_work_norm.norm_wing = pBolt->des_base_pos.offset_wing = 1;
					else
					{
						pBolt->des_base_pos.offset_wing = ciConnectWing - 'X';
						pBolt->des_work_norm.norm_wing = ciConnectWing - 'X';
					}
					pBolt->correct_worknorm();
					pBolt->correct_pos();
					GEPOINT interspoint;
					int retcode = Int3dlf(interspoint, pBolt->ucs.origin, pBolt->ucs.axis_z, acs.origin, acs.axis_z);
					if (retcode == 1)
					{	//δ����ص�����˨
						BOLTREF* pBoltRef;
						for (pBoltRef = listExistBolts.EnumObjectFirst(); pBoltRef; pBoltRef = listExistBolts.EnumObjectNext())
						{
							if (pBoltRef->location.IsEqual(interspoint, 5))
								break;
						}
						if (pBoltRef)
						{
							Ta.Parts.DeleteCursor();
							pBolt = pBoltRef->pBolt;
						}
						else
							listExistBolts.Append(BOLTREF(pBolt, interspoint));
					}
					pBolt->AddL0Thick(pRod->handle, TRUE);
					pBolt->AddL0Thick(pPlate->handle, TRUE);
					if (layout.idDirectConnectToAngle1 > 0)
					{
						SmartRodPtr pShareRod = GetLdsRodFromId(layout.idDirectConnectToAngle1);
						if (pShareRod.blValid)
						{
							pBolt->AddL0Thick(pShareRod->handle, TRUE);
							pShareRod->AppendLsRef(pBolt->GetLsRef());
						}
					}
					if (layout.idDirectConnectToAngle2 > 0)
					{
						SmartRodPtr pShareRod = GetLdsRodFromId(layout.idDirectConnectToAngle2);
						if (pShareRod.blValid)
						{
							pBolt->AddL0Thick(pShareRod->handle, TRUE);
							pShareRod->AppendLsRef(pBolt->GetLsRef());
						}
					}
					pBolt->CalGuigeAuto();
					if(pGroupJg)
						pGroupJg->AppendLsRef(pBolt->GetLsRef());
					else
						pRod->AppendLsRef(pBolt->GetLsRef());
					if (pShareBoltAngle.blValid&&pShareBoltAngle->IsAngle())
					{	//��ӽ���㶨λ������˨�������߽Ǹ��� wjh-2019.11.25
						if( pShareBoltAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
							pShareBoltAngle->AppendLsRef(pBolt->GetLsRef());	//��ϽǸ��ڻ��Զ��ж���˨�Ƿ����ӽǸ�֫�� wjh-2019.11.25
						else if(pShareBoltAngle->GetClassTypeId()==CLS_LINEANGLE&&
							pShareBoltAngle.pAngle->IsLsInWing(pBolt,NULL,true))
							pShareBoltAngle->AppendLsRef(pBolt->GetLsRef());
					}
					pPlate->AppendLsRef(pBolt->GetLsRef());
				}
			}
			if (pParaRod->idStartNode == 0)
			{	//��ֹ��׼�ĸ�ͷ�������
				if (pRod->pStart == pBaseNode)
				{
					double dfOldOddment = pRod->dfStartOdd;
					CStackVariant<int> stack(&pRod->desStartOdd.m_iOddCalStyle, 1);
					pRod->CalStartOddment();
					pRod->SetStartOdd(max(dfOldOddment, pRod->dfStartOdd));
					if (pRod->desStartOdd.m_hRefPart1 == 0)
						stack.Enable(false);
				}
				else if (pRod->pEnd == pBaseNode)
				{
					double dfOldOddment = pRod->dfEndOdd;
					CStackVariant<int> stack(&pRod->desEndOdd.m_iOddCalStyle, 1);
					pRod->CalEndOddment();
					pRod->SetEndOdd(max(dfOldOddment, pRod->dfEndOdd));
					if (pRod->desEndOdd.m_hRefPart1 == 0)
						stack.Enable(false);
				}
			}
		}
	}
}
UINT CStdComModel::LayoutAngleBolts(CONNECT_ROD* pConnRod, BOLT_LAYOUT& layout, CXhSimpleList<BOLT_LOCATION>* pListLocations)
{
	CStdTemplParaRod* pParaRod = m_pStdTempl->listParaRods.GetValue(pConnRod->sidRod);
	SmartRodPtr pRod = GetLdsRodFromId(pConnRod->idRod);
	short g[4] = { (short)(pRod->size_wide*0.5),0,0,0 };
	if (pRod->IsAngle())
	{
		JGZJ jgzj;
		jgzj = pRod.pAngle->GetAngleZJ(pRod->size_wide);
		g[0] = (short)jgzj.g;
		g[1] = (short)jgzj.g1;
		g[2] = (short)jgzj.g2;
		g[3] = (short)jgzj.g3;
	}
	//UINT count = 0;
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace, layout.ciBoldMd);// ciConnectWing
	if(layout.basepoint.ciPosCalStyle!=BOLT_ORIGIN::LOCA_KEEPSPACE2ROD)
		return layout.LayoutBolts(g, pListLocations, LsSpace.EndSpace, LsSpace.SingleRowSpace, LsSpace.doubleRowSpace);
	//������ָ���ı��������϶����˨�������
	GEPOINT vCurrLenStdVec = pRod->xPosEnd - pRod->xPosStart;
	bool blCurrDesignRayAngleBolts = (pParaRod->idStartNode == 1);
	if (blCurrDesignRayAngleBolts && pParaRod->bReverseStartEnd)
		vCurrLenStdVec *= -1.0;	//��ǰ�������߸˼���˨ʱ�����߷���Ϊ������
	normalize(vCurrLenStdVec);

	SmartRodPtr pRefRod;
	CStdTemplParaRod* pRefParaRod;
	pRefParaRod = m_pStdTempl->listParaRods.GetValue(layout.basepoint.widBaseAngle);
	pRefRod = GetLdsRodFromId(layout.basepoint.widBaseAngle);
	if (pRefParaRod==NULL||!pRefRod.blValid)
		return false;	//ģ�嶨����� wjh-2019.12.2
	GEPOINT vRefLenStdVec = pRefRod->xPosEnd - pRefRod->xPosStart;
	normalize(vRefLenStdVec);
	if ((pRefParaRod->idStartNode==1&& pRefParaRod->bReverseStartEnd)||
		(pRefParaRod->idEndNode==1  &&!pRefParaRod->bReverseStartEnd))
		vRefLenStdVec *= -1.0;	//���ո˼�Ϊ���߸˼�ʱ�������߷���Ϊ������
	double dfCurrRayCosa = vRefLenStdVec * vCurrLenStdVec;
	if(pRefParaRod->idStartNode==0&& dfCurrRayCosa<0)
	{
		dfCurrRayCosa *= -1.0;
		vRefLenStdVec *= -1.0;	//���ո˼�Ϊ��׼�˼�ʱ�����뵱ǰ������˨�����߷��������һ��Ϊ������
	}
	//���㵥����˨��������
	UINT i,uiSideCount = layout.ciLayoutStyle == 0 ? layout.cnCount : layout.cnCount / 2 + layout.cnCount % 2;
	double dfStdStaggerBoltCosa = 2.0*(g[2] - g[1])/LsSpace.doubleRowSpace;
	dfStdStaggerBoltCosa = 1/sqrt(1 + dfStdStaggerBoltCosa * dfStdStaggerBoltCosa);	//˫����˨���ʱ�ı�׼�н�����

	char direction = 0;
	GEPOINT vFlowDirection;	//��˨��������
	if (blCurrDesignRayAngleBolts)	//��ǰ���õ������߸˼�����˨
	{
		direction = pParaRod->bReverseStartEnd ? 1 : 0;
		vFlowDirection = vCurrLenStdVec;
	}
	else
	{	//��ǰ���õ��ǻ�׼���ϵ���˨
		vFlowDirection = (dfCurrRayCosa > 0) ? vCurrLenStdVec : -vCurrLenStdVec;
		direction = dfCurrRayCosa > 0 ? 0 : 1;
	}
	dfCurrRayCosa = fabs(dfCurrRayCosa);
	BOLT_ORIGIN basepoint;
	basepoint.ciPosCalStyle = BOLT_ORIGIN::LOCA_AXISINTERS;
	basepoint.widBaseAngle  = pConnRod->idRod;
	basepoint.widCrossAngle = layout.basepoint.widBaseAngle;
	GEPOINT vWorkNormal = layout.ciDatumWing == 'Y' ? pRod.pAngle->vyWingNorm : pRod.pAngle->vxWingNorm;
	short siInitLenOffset=0, siInitWingOffset = g[0];	//�׿���˨���������ƫ�Ƴ�ʼֵ
	BYTE ciRows = 1;
	bool blHeadBoltAtG1 = true;
	if (uiSideCount > layout.cnMaxSingularLineBolts&&g[2] - g[1] > 0)
	{
		ciRows = 2;	//˫�Ų���
		if (blCurrDesignRayAngleBolts)	//��ǰ������˨�ĸ˼�Ϊ���߸˼�
		{
			if ((layout.ciDatumWing=='X'&&pRod.pAngle->vyWingNorm*vRefLenStdVec> 0)||
				(layout.ciDatumWing=='Y'&&pRod.pAngle->vxWingNorm*vRefLenStdVec> 0))
			{	// ���߲�Ϊ�ڽ�������˨����2����
				blHeadBoltAtG1 = false;
				siInitWingOffset = g[2];
				if (dfCurrRayCosa > dfStdStaggerBoltCosa)
				{	//���߼н�С�ڱ�׼��׽ǣ���λ��˨λ��1���ģ���������ո˼���Ե����
					siInitLenOffset= (short)-LsSpace.doubleRowSpace / 2;
					basepoint.ciStyleG = 1;
				}
				else//���߼нǴ��ڱ�׼��׽ǣ���λ��˨λ��2���ģ��Ҿ�����ո˼���Ե����
					basepoint.ciStyleG = 2;
			}
			else
			{	// ֫���Ϊ�ڽ�������˨����1����
				siInitWingOffset = g[1];
				if (dfCurrRayCosa > dfStdStaggerBoltCosa)
				{	//���߼н�С�ڱ�׼��׽ǣ���λ��˨λ��2���ģ���������ո˼���Ե����
					siInitLenOffset = (short)-LsSpace.doubleRowSpace / 2;
					basepoint.ciStyleG = 2;
				}
				else
				{	//���߼нǴ��ڱ�׼��׽ǣ���λ��˨λ��1���ģ��Ҿ�����ո˼���Ե����
					basepoint.ciStyleG = 1;
				}
			}
		}
		else
		{
			siInitWingOffset = g[1];
			if (dfCurrRayCosa > dfStdStaggerBoltCosa)
			{
				siInitLenOffset = (short)-LsSpace.doubleRowSpace / 2;
				basepoint.ciStyleG = 2;	//���߼н�С�ڱ�׼��׽���2����Ϊ��λ��׼
			}
			else
				basepoint.ciStyleG = 1;	//���߼нǴ��ڱ�׼��׽���1����Ϊ��λ��׼
		}
	}
	else//������˨�����ڱ�׼������
		basepoint.ciStyleG = 0;
	char ciRayAngleWing;
	double ddx = pRefRod.pAngle->vxWingNorm*vWorkNormal;
	double ddy = pRefRod.pAngle->vyWingNorm*vWorkNormal;
	if (fabs(ddx) >= fabs(ddy))
	{
		ciRayAngleWing = ddx > 0 ? 'X' : 'x';
		if (pRefRod.pAngle->vyWingNorm*vFlowDirection > 0)	// ���߲�Ϊ�ڽ���
			basepoint.ciStyleCrossG = BOLT_ORIGIN::POS_nS;
		else
			basepoint.ciStyleCrossG = BOLT_ORIGIN::POS_WaS;
	}
	else
	{
		ciRayAngleWing = ddy > 0 ? 'Y' : 'y';
		if (pRefRod.pAngle->vxWingNorm*vFlowDirection > 0)	// ���߲�Ϊ�ڽ���
			basepoint.ciStyleCrossG = BOLT_ORIGIN::POS_nS;
		else
			basepoint.ciStyleCrossG = BOLT_ORIGIN::POS_WaS;
	}
	BOLT_LOCATION location;
	location.basepoint = basepoint;
	location.siLenOffset = siInitLenOffset;
	location.siWingOffset = siInitWingOffset;
	location.ciDatumWing = layout.ciDatumWing;
	location.ciDirection = direction;
	for (i = 0; i < uiSideCount; i++)
	{
		pListLocations->Append(location);
		if (ciRows == 1)
			location.siLenOffset += (short)LsSpace.SingleRowSpace;
		else
		{
			location.siLenOffset += (short)LsSpace.doubleRowSpace / 2;
			if (blHeadBoltAtG1)
			{
				if (i % 2 == 0)	//g1->g2
					location.siWingOffset += g[2] - g[1];
				else if (i % 2 == 1)//g2->g1
					location.siWingOffset -= g[2] - g[1];
			}
			else
			{
				if (i % 2 == 0)//g2->g1
					location.siWingOffset -= g[2] - g[1];
				else if (i % 2 == 1)	//g1->g2
					location.siWingOffset += g[2] - g[1];
			}
		}
	}
	if (layout.ciLayoutStyle == 1)
	{	//˫�಼��ʱ��һ����˨����
		uiSideCount = layout.cnCount - uiSideCount;
		UINT idCrossAngle = layout.basepoint.widCrossAngle > 0 ? layout.basepoint.widCrossAngle : layout.basepoint.widBaseAngle;
		pRefParaRod = m_pStdTempl->listParaRods.GetValue(idCrossAngle);
		pRefRod = GetLdsRodFromId(idCrossAngle);
		vRefLenStdVec = pRefRod->xPosEnd - pRefRod->xPosStart;
		if (pRefParaRod->bReverseStartEnd)
			vRefLenStdVec *= -1.0;
		normalize(vRefLenStdVec);
		//���㵥����˨��������
		direction = 1-direction;	//0.��ʾ���߷������ն˲ࣻ1.��ʾ���߷�����ʼ�˲�
		vFlowDirection *= -1;
		dfCurrRayCosa = vRefLenStdVec * vFlowDirection;
		basepoint.ciPosCalStyle = BOLT_ORIGIN::LOCA_AXISINTERS;
		basepoint.widCrossAngle = idCrossAngle;
		siInitLenOffset = 0;
		siInitWingOffset = g[0];	//�׿���˨���������ƫ�Ƴ�ʼֵ
		ciRows = 1;
		bool blHeadBoltAtG1 = true;
		if (uiSideCount > layout.cnMaxSingularLineBolts&&g[2] - g[1] > 0)
		{
			ciRows = 2;	//˫�Ų���
			if (dfCurrRayCosa > 0)
			{
				siInitWingOffset = g[1];
				if (fabs(dfCurrRayCosa) > dfStdStaggerBoltCosa)
				{
					siInitLenOffset = (short)-LsSpace.doubleRowSpace / 2;
					basepoint.ciStyleG = 2;	//���߼н�С�ڱ�׼��׽���2����Ϊ��λ��׼
				}
				else
					basepoint.ciStyleG = 1;	//���߼нǴ��ڱ�׼��׽���1����Ϊ��λ��׼
			}
			else
			{
				blHeadBoltAtG1 = false;
				siInitWingOffset = g[2];
				if (fabs(dfCurrRayCosa) < dfStdStaggerBoltCosa)
					basepoint.ciStyleG = 2;	//���߼н�С�ڱ�׼��׽���2����Ϊ��λ��׼
				else
				{
					siInitLenOffset = (short)-LsSpace.doubleRowSpace / 2;
					basepoint.ciStyleG = 1;	//���߼нǴ��ڱ�׼��׽���1����Ϊ��λ��׼
				}
			}
		}
		else//������˨�����ڱ�׼������
			basepoint.ciStyleG = 0;
		double ddx = pRefRod.pAngle->vxWingNorm*vWorkNormal;
		double ddy = pRefRod.pAngle->vyWingNorm*vWorkNormal;
		char ciRayAngleWing;
		if (fabs(ddx) >= fabs(ddy))
		{
			ciRayAngleWing = ddx > 0 ? 'X' : 'x';
			if (pRefRod.pAngle->vyWingNorm*vFlowDirection > 0)	// ���߲�Ϊ�ڽ���
				basepoint.ciStyleCrossG = BOLT_ORIGIN::POS_nS;
			else
				basepoint.ciStyleCrossG = BOLT_ORIGIN::POS_WaS;
		}
		else
		{
			ciRayAngleWing = ddy > 0 ? 'Y' : 'y';
			if (pRefRod.pAngle->vxWingNorm*vFlowDirection > 0)	// ���߲�Ϊ�ڽ���
				basepoint.ciStyleCrossG = BOLT_ORIGIN::POS_nS;
			else
				basepoint.ciStyleCrossG = BOLT_ORIGIN::POS_WaS;
		}
		location.basepoint = basepoint;
		location.siLenOffset = siInitLenOffset;
		location.siWingOffset = siInitWingOffset;
		location.ciDatumWing = layout.ciDatumWing;
		location.ciDirection = direction;
		for (i = 0; i < uiSideCount; i++)
		{
			pListLocations->Append(location);
			if (ciRows == 1)
				location.siLenOffset += (short)LsSpace.SingleRowSpace;
			else
			{
				location.siLenOffset += (short)LsSpace.doubleRowSpace / 2;
				if (blHeadBoltAtG1)
				{
					if (i % 2 == 0)	//g1->g2
						location.siWingOffset += g[2] - g[1];
					else if (i % 2 == 1)//g2->g1
						location.siWingOffset -= g[2] - g[1];
				}
				else
				{
					if (i % 2 == 0)//g2->g1
						location.siWingOffset -= g[2] - g[1];
					else if (i % 2 == 1)	//g1->g2
						location.siWingOffset += g[2] - g[1];
				}
			}
		}
	}
	return layout.cnCount;
}
CLDSGeneralPlate* CStdComModel::AppendParaModelPlate(
	CStdTemplParaPlate* pParaPlate,GECS* pAcs/*=NULL*/,BYTE ciMirMode/*=0*/)
{
	//TODO:δ��Ҳ����õ�ciMirMode��������ģ�����׼ģ���ƥ��Գ������Ŀǰ�����ô� wjh-2019.11.16
	CStackVariant<char> stackprop0(&CLDSBolt::BOLTL0_CALMODE, CLDSBolt::L0CAL_BY_SUMM_THICK);
	CStackVariant<char> stackprop0i(&CLDSBolt::BOLTL0_PREFER_MODE, CLDSBolt::L0CAL_INC_GAP_THICK);
	if(pParaPlate->wiType==CStdTemplParaPlate::TYPE_CONNECT_PLATE||pParaPlate->wiType==CStdTemplParaPlate::TYPE_VIRTUAL_PLATE)
	{
		CLDSNode* pBaseNode=GetLdsNodeFromId(pParaPlate->design.idBaseNode);
		CLDSLinePart* pBaseRod=GetLdsRodFromId(pParaPlate->design.idBaseRod);
		if(pBaseNode==NULL || pBaseRod==NULL)
			return NULL;
		//����ְ����ڹ���֫
		CStdTemplParaRod* pBseParaRod=m_pStdTempl->listParaRods.GetValue(pParaPlate->design.idBaseRod);
		BYTE ciDatumAngleWing=pParaPlate->design.ciDatumAngleWing-'X';
		if(pBaseRod->IsAngle()&&pAcs!=NULL)
		{	//ָ���ְ�װ������ϵʱ�Զ��жϻ�׼�Ǹֵ�ǰ����֫
			CLDSLineAngle* pBaseAngle=(CLDSLineAngle*)pBaseRod;
			double ddx=pBaseAngle->vxWingNorm*pAcs->axis_z;
			double ddy=pBaseAngle->vyWingNorm*pAcs->axis_z;
			ciDatumAngleWing = (fabs(ddx) >= fabs(ddy)) ? 0 : 1;
		}
		else if(pBseParaRod->bReverseWingX_Y)//bReverseStartEnd)
			ciDatumAngleWing=1-ciDatumAngleWing;
		//�������Ӹ˼�������
		int nMaxConnRodThick = 0;
		CStdTemplParaRod* pParaRod = NULL;
		for (int i = 0; i < (int)pParaPlate->connrods.Count; i++)
		{
			CONNECT_ROD* pConnRod = pParaPlate->connrods.GetAt(i);
			pParaRod = m_pStdTempl->listParaRods.GetValue(pConnRod->sidRod);	//�˼��̱�ʶ����ָ����ϽǸ�
			CLDSLinePart* pRod = GetLdsRodFromId(pConnRod->idRod);	//�˼�����ʶһ��ָ�򵥽Ǹֻ���ϽǸֵ��ӽǸ�
			if (pParaRod == NULL || pRod == NULL)
				continue;
			CLDSGroupLineAngle* pGroupAngle = NULL;
			if (pRod->GetClassTypeId() == CLS_GROUPLINEANGLE)
				pGroupAngle = (CLDSGroupLineAngle*)pRod;
			if (pConnRod->sidRod != pParaPlate->design.idBaseRod)
			{	//ȷ����ǰ�˼��ǻ�׼�˼� wjh-2019.9.9
				if (pGroupAngle == NULL || (pGroupAngle->son_jg_h[0] != pRod->handle&&pGroupAngle->son_jg_h[1] != pRod->handle))
					nMaxConnRodThick = max(nMaxConnRodThick, (int)(pRod->Thick + 0.5));
			}
		}
		//��Ӹְ壬��ʼ���ְ���Ʋ���
		CLDSPlate xTmpPlate,*pBasicPlate=NULL;
		xTmpPlate.SetBelongModel(&Ta);
		if (pParaPlate->wiType==CStdTemplParaPlate::TYPE_CONNECT_PLATE)
			pBasicPlate=(CLDSPlate*)Ta.Parts.append(CLS_PLATE);
		else //if(pParaPlate->wiType==CStdTemplParaPlate::TYPE_VIRTUAL_PLATE)
			pBasicPlate=&xTmpPlate;
		pBasicPlate->ucs.axis_x.Set();
		pBasicPlate->ucs.axis_y.Set();
		m_pStdTempl->hashMatchPlates.SetValue(pParaPlate->Id, pBasicPlate->handle);
		pBasicPlate->cfgword = pBaseRod->cfgword;
		pBasicPlate->SetLayer(pBaseRod->szLayer);
		if (pParaPlate->_thick > 0)
			pBasicPlate->Thick = pParaPlate->_thick;
		else
		{	//�������߲����֫��+2~3mm�����
			if (nMaxConnRodThick % 2 > 0)
				nMaxConnRodThick += 1;
			pBasicPlate->Thick = nMaxConnRodThick + 2;
		}
		pBasicPlate->cMaterial=pParaPlate->cMaterial;
		pBasicPlate->designInfo.m_bEnableFlexibleDesign=true;
		pBasicPlate->designInfo.iProfileStyle0123=pParaPlate->design.ciProfileStyle;
		pBasicPlate->designInfo.m_hBaseNode=pBaseNode->handle;
		pBasicPlate->designInfo.m_hBasePart=pBaseRod->handle;
		//�趨�ְ�װ�����
		pBasicPlate->designInfo.origin.datum_pos_style=2;
		pBasicPlate->designInfo.origin.des_para.RODNODE.hNode=pBaseNode->handle;
		pBasicPlate->designInfo.origin.des_para.RODNODE.hRod=pBaseRod->handle;
		pBasicPlate->designInfo.origin.des_para.RODNODE.offset_wing=ciDatumAngleWing;
		pBasicPlate->designInfo.origin.des_para.RODNODE.wing_offset_style=0;
		pBasicPlate->designInfo.origin.UpdatePos(m_pModel);
		//�ݲ�֧��װ�����ƽ��
		//pBasicPlate.pPlate->designInfo.origin.des_para.RODNODE.bFaceOffset
		//pBasicPlate.pPlate->designInfo.origin.des_para.RODNODE.vPlaneNormal
		//�趨�ְ�װ����淨��
		pBasicPlate->designInfo.norm.norm_style=1;
		pBasicPlate->designInfo.norm.direction =0;	//Ĭ�ϳ���
		pBasicPlate->designInfo.norm.hVicePart=pBaseRod->handle;
		pBasicPlate->designInfo.norm.norm_wing=ciDatumAngleWing;
		pBasicPlate->designInfo.norm.UpdateVector(m_pModel);
		GEPOINT xarrWorkPlaneNormal[3];	//[0]:���淨��;[1]:1#�����淨��;[2]:2#�����淨�� wjh-2019.11.12
		xarrWorkPlaneNormal[0]=pBasicPlate->designInfo.norm.vector;
		if (pBaseRod->chLayer1st == 'S'&&pBaseRod->chLayer2nd == 'Z')
		{
			pBasicPlate->ucs.axis_z = pBasicPlate->designInfo.norm.vector;
			if (pBasicPlate->ucs.axis_z.y > 0.8)
				pBasicPlate->chLayer3rd = 'Q';
			else if (pBasicPlate->ucs.axis_z.y < -0.8)
				pBasicPlate->chLayer3rd = 'H';
			else if (pBasicPlate->ucs.axis_z.x > 0.8)
				pBasicPlate->chLayer3rd = 'Y';
			else if (pBasicPlate->ucs.axis_z.x < -0.8)
				pBasicPlate->chLayer3rd = 'Z';
		}
		for (int i = 0; i < 2; i++)
		{
			SmartRodPtr pBendLineAngle = this->GetLdsRodFromId(pParaPlate->design.bends[i].widAngleBendBy);
			if (pBendLineAngle.IsNULL())
				break;
			f3dPoint workView = pAcs ? pAcs->axis_z : xarrWorkPlaneNormal[0];
			BYTE ciS0_E1 = (pBendLineAngle->pStart == pBaseNode) ? 0 : 1;
			double ddx = pBendLineAngle.pAngle->vxWingNorm*workView;
			double ddy = pBendLineAngle.pAngle->vyWingNorm*workView;
			BYTE ciBendEdgeWing = (fabs(ddx) >= fabs(ddy)) ? 0 : 1;
			if (pParaPlate->design.bends[i].ciBendEdgeWing == 0)
				pParaPlate->design.bends[i].ciBendEdgeWing = 'X' + ciBendEdgeWing;
			//��������Ʋ���
			pBasicPlate->face_N = (i == 0) ? 2 : 3;
			pBasicPlate->designInfo.iFaceType = (i == 0) ? 2 : 3;
			if (pParaPlate->design.bends[i].biSelfCalBendline==FALSE)
			{	//���ݻ��������ڽǸ�ָ��������ʼ�ն�λ��
				pBasicPlate->designInfo.m_bSelfCorrectBendline = FALSE;
				pBasicPlate->designInfo.huoquline_start[i].datum_pos_style = 2;
				pBasicPlate->designInfo.huoquline_start[i].des_para.RODNODE.hRod = pBendLineAngle->handle;
				pBasicPlate->designInfo.huoquline_start[i].des_para.RODNODE.hNode = pBaseNode->handle;
				pBasicPlate->designInfo.huoquline_start[i].des_para.RODNODE.offset_wing = ciBendEdgeWing;
				pBasicPlate->designInfo.huoquline_start[i].des_para.RODNODE.wing_offset_style = 4;
				pBasicPlate->designInfo.huoquline_start[i].des_para.RODNODE.wing_offset_dist = -pParaPlate->design.bends[i].siBendlineSpace;
				pBasicPlate->designInfo.huoquline_start[i].des_para.RODNODE.len_offset_dist = -200;
				pBasicPlate->designInfo.huoquline_start[i].CloneParaTo(&pBasicPlate->designInfo.huoquline_end[i]);
				pBasicPlate->designInfo.huoquline_end[i].des_para.RODNODE.len_offset_dist = 200;
			}
			else
			{	//ͨ���������Զ����������,�Ǹ������Ϊ������ʰȡ��
				pBasicPlate->designInfo.m_bSelfCorrectBendline = TRUE;
				pBasicPlate->designInfo.huoquline_start[i].datum_pos_style = 1;
				pBasicPlate->designInfo.huoquline_start[i].des_para.RODEND.hRod = pBendLineAngle->handle;
				pBasicPlate->designInfo.huoquline_start[i].des_para.RODEND.wing_offset_style = 4;
				pBasicPlate->designInfo.huoquline_start[i].des_para.RODEND.wing_offset_dist = 0;
				pBasicPlate->designInfo.huoquline_start[i].des_para.RODEND.direction = ciS0_E1;
			}
			//�����淨����Ʋ���
			BYTE ciBendMode = pParaPlate->design.bends[i].ciBendMode;
			if (ciBendMode == 0)
				;//TODO:δ���ָ�������淨��
			else if (ciBendMode == 1)
				;//TODO:δ���ָ������Ƕ�
			else if (ciBendMode == 2)
			{	//ָ���Ǹ�֫����
				SmartRodPtr pBendNormAngle = this->GetLdsRodFromId(pParaPlate->design.bends[i].normal.idBendPlaneByAngle);
				if (pBendNormAngle.IsNULL())
					return false;
				ddx = pBendNormAngle.pAngle->vxWingNorm*workView;
				ddy = pBendNormAngle.pAngle->vyWingNorm*workView;
				BYTE ciBendNormWing = (fabs(ddx) >= fabs(ddy)) ? 0 : 1;
				if (pParaPlate->design.bends[i].normal.cDatumWingXorY == 0)
					pParaPlate->design.bends[i].normal.cDatumWingXorY = ciBendNormWing;
				//
				pBasicPlate->designInfo.huoqufacenorm[i].norm_style = 1;
				pBasicPlate->designInfo.huoqufacenorm[i].hVicePart = pBendNormAngle->handle;
				pBasicPlate->designInfo.huoqufacenorm[i].norm_wing = ciBendNormWing;
				pBasicPlate->designInfo.huoqufacenorm[i].direction = 0;
				if ((ciBendNormWing == 0 && ddx < 0) || (ciBendNormWing == 1 && ddy < 0))
					pBasicPlate->designInfo.huoqufacenorm[i].direction = 1;
				pBasicPlate->designInfo.huoqufacenorm[i].UpdateVector(m_pModel);
			}
			else if (ciBendMode == 3)
			{	//���˼����淨��
				char ciDirectRefAxisXorY = 0;
				SmartRodPtr pBendPlaneRod1, pBendPlaneRod2;
				pBendPlaneRod1 = this->GetLdsRodFromId(pParaPlate->design.bends[i].normal.widBendPlaneRefRod1);
				pBendPlaneRod2 = this->GetLdsRodFromId(pParaPlate->design.bends[i].normal.widBendPlaneRefRod2);
				ciDirectRefAxisXorY = pParaPlate->design.bends[i].normal.ciDirectRefAxisXorY;
				if (!pBendPlaneRod1.blValid || !pBendPlaneRod2.blValid)
					return false;
				f3dPoint normV = xarrWorkPlaneNormal[0];
				if (ciDirectRefAxisXorY == 'X')
					normV.Set(1, 0, 0);
				else if (ciDirectRefAxisXorY == 'x')
					normV.Set(-1, 0, 0);
				else if (ciDirectRefAxisXorY == 'Y')
					normV.Set(0, 1, 0);
				else if (ciDirectRefAxisXorY == 'y')
					normV.Set(0, -1, 0);
				else if (ciDirectRefAxisXorY == 'Z')
					normV.Set(0, 0, 1);
				else if (ciDirectRefAxisXorY == 'z')
					normV.Set(0, 0, -1);
				//
				pBasicPlate->designInfo.huoqufacenorm[i].norm_style = 2;
				pBasicPlate->designInfo.huoqufacenorm[i].hVicePart = pBendPlaneRod1->handle;
				pBasicPlate->designInfo.huoqufacenorm[i].hCrossPart = pBendPlaneRod2->handle;
				pBasicPlate->designInfo.huoqufacenorm[i].nearVector = normV;
				pBasicPlate->designInfo.huoqufacenorm[i].UpdateVector(m_pModel);
			}
			xarrWorkPlaneNormal[i+1]=pBasicPlate->designInfo.huoqufacenorm[i].vector;
		}
		//������Ӹ˼���Ʋ���
		for(int i=0;i<(int)pParaPlate->connrods.Count;i++)
		{
			CONNECT_ROD* pConnRod=pParaPlate->connrods.GetAt(i);
			pParaRod=m_pStdTempl->listParaRods.GetValue(pConnRod->sidRod);	//�˼��̱�ʶ����ָ����ϽǸ�
			CLDSLinePart* pRod=GetLdsRodFromId(pConnRod->idRod);	//�˼�����ʶһ��ָ�򵥽Ǹֻ���ϽǸֵ��ӽǸ�
			if(pParaRod==NULL || pRod==NULL)
			{
				if(pParaRod->m_bNecessaryRod)
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"�Ҳ���%d��Ӧ��ģ�͸˼�!",pConnRod->sidRod);
				continue;
			}
			SmartRodPtr pSmartRod = pRod;
			char ciConnectWing = pConnRod->ciConnectWing;
			GEPOINT vCurrWorkPlaneNorm = xarrWorkPlaneNormal[min(pConnRod->ciPlaneNo - 1, 2)];
			if (pConnRod->ciConnectWing == 0 && pRod->IsAngle())
			{	//�Զ��жϽǸֵ�ǰ��������֫
				double ddx = pSmartRod.pAngle->vxWingNorm*vCurrWorkPlaneNorm;
				double ddy = pSmartRod.pAngle->vyWingNorm*vCurrWorkPlaneNorm;
				ciConnectWing = fabs(ddx) > fabs(ddy) ? 'X' : 'Y';
			}
			else if (pParaRod->bReverseWingX_Y)
				ciConnectWing = 'X' - ciConnectWing + 'Y';
			BYTE ciLineS0E1 = pConnRod->ciCurrTipSideS0E1;
			if (pRod->pStart == pBaseNode)
				ciLineS0E1 = 0;
			else if (pRod->pEnd == pBaseNode)
				ciLineS0E1 = 1;
			else
				ciLineS0E1 = 2;
			//��ʼ�����Ӹ˼������Ϣ
			CDesignLjPartPara xLjPartPara, *pLjRod = NULL;
			xLjPartPara.iFaceNo = pConnRod->ciPlaneNo;
			xLjPartPara.start0_end1 = ciLineS0E1;
			xLjPartPara.angle.cur_wing_x0_y1 = ciConnectWing - 'X';
			xLjPartPara.angle.bTwoEdge = pConnRod->dualSideEdge;
			if (pConnRod->idRod == pParaPlate->design.idBaseRod)
			{
				xLjPartPara.angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOBER);
				xLjPartPara.angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
			}
			else
			{
				xLjPartPara.angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
				xLjPartPara.angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
			}
			if (pConnRod->cbSpaceFlag > 0)
				xLjPartPara.angle.cbSpaceFlag = pConnRod->cbSpaceFlag;
			else if (pConnRod->dualSideEdge)
			{
				xLjPartPara.angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOBER);
				xLjPartPara.angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
			}
			//������Ӹ˼���Ʋ���
			if (pRod->GetClassTypeId() == CLS_GROUPLINEANGLE && pSmartRod->size_cSubClassType == 'T')
			{	//T����ϽǸ�ϸ�ֵ��ӽǸ�
				CLDSGroupLineAngle* pGroupJg = (CLDSGroupLineAngle*)pRod;
				pGroupJg->jg_space = pBasicPlate->GetThick();
				for (int j = 0; j < 4; j++)
				{
					CLDSLineAngle *pJg = (CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[j], CLS_LINEANGLE);
					if (pJg==NULL)
						continue;
					double ddx = fabs(pJg->vxWingNorm*vCurrWorkPlaneNorm);
					double ddy = fabs(pJg->vyWingNorm*vCurrWorkPlaneNorm);
					pLjRod = pBasicPlate->designInfo.partList.SetValue(pJg->handle, xLjPartPara);
					pLjRod->pCurLinePart = pJg;
					pLjRod->angle.cur_wing_x0_y1 = (ddx > ddy) ? 0 : 1;
					//��������ͷ
					if (pLjRod->start0_end1 < 2)
					{
						DESIGNODDMENT* pOddDes = (pLjRod->start0_end1 == 0) ? &pJg->desStartOdd : &pJg->desEndOdd;
						pOddDes->m_iOddCalStyle = pConnRod->ciOddCalStyle;
						if (pConnRod->ciOddCalStyle == 0)
						{
							pOddDes->m_hRefPart1 = m_pStdTempl->ToLdsHandle(SEGI("R", pConnRod->sidOddRefRod[0]));
							pOddDes->m_hRefPart2 = m_pStdTempl->ToLdsHandle(SEGI("R", pConnRod->sidOddRefRod[1]));
							if (pLjRod->start0_end1 == 0)
								pJg->CalStartOddment();
							else
								pJg->CalEndOddment();
						}
						else if (pConnRod->ciOddCalStyle == 2)
						{
							if (pLjRod->start0_end1 == 0)
								pJg->SetStartOdd(pConnRod->siSpecOddment);
							else
								pJg->SetEndOdd(pConnRod->siSpecOddment);
						}
					}
				}
			}
			else
			{
				pLjRod = pBasicPlate->designInfo.partList.SetValue(pRod->handle, xLjPartPara);
				pLjRod->pCurLinePart = pRod;
				//��������ͷ
				if (pLjRod->start0_end1 < 2)
				{
					DESIGNODDMENT* pOddDes = (pLjRod->start0_end1 == 0) ? &pRod->desStartOdd : &pRod->desEndOdd;
					pOddDes->m_iOddCalStyle = pConnRod->ciOddCalStyle;
					if (pConnRod->ciOddCalStyle == 0)
					{
						pOddDes->m_hRefPart1 = m_pStdTempl->ToLdsHandle(SEGI("R", pConnRod->sidOddRefRod[0]));
						pOddDes->m_hRefPart2 = m_pStdTempl->ToLdsHandle(SEGI("R", pConnRod->sidOddRefRod[1]));
						if (pLjRod->start0_end1 == 0)
							pRod->CalStartOddment();
						else
							pRod->CalEndOddment();
					}
					else if (pConnRod->ciOddCalStyle == 2)
					{
						if (pLjRod->start0_end1 == 0)
							pRod->SetStartOdd(pConnRod->siSpecOddment);
						else
							pRod->SetEndOdd(pConnRod->siSpecOddment);
					}
				}
			}
		}
		//�������Ӹ˼�����˨
		LayoutConnRodBolts(pParaPlate, xarrWorkPlaneNormal);
		//
		if(pParaPlate->wiType==CStdTemplParaPlate::TYPE_CONNECT_PLATE)
			pBasicPlate->DesignPlate();
		stackprop0.PopStack();
		//�ְ���ƺ����¼�����˨λ�ü�ͨ��(CLDSBolt::CalL0()���ݹ�������λ�ü�����˨ͨ��) wxc-2019.7.23
		for (CLsRef *pLsRef = pBasicPlate->GetFirstLsRef(); pLsRef; pLsRef = pBasicPlate->GetNextLsRef())
		{
			CLDSBolt *pBolt = pLsRef->GetLsPtr();
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->CalGuigeAuto();
		}
		return pBasicPlate;
	}
	else if(pParaPlate->wiType==CStdTemplParaPlate::TYPE_W_TUBECIRPLATE)
	{
		CLDSNode* pBaseNode=GetLdsNodeFromId(pParaPlate->design.idBaseNode);
		CLDSLinePart* pBasePart=GetLdsRodFromId(pParaPlate->design.idBaseRod);
		CLDSGeneralPlate* pConnPlate=GetLdsPlateFromId(pParaPlate->design.idBasePlate);
		if(pBaseNode==NULL||pBasePart==NULL||pConnPlate==NULL)
			return NULL;
		double fD=m_xExpression.SolveExpression("PA");
		fD=(fD==0)?60:fD;
		CLDSParamPlate* pCirPlate=(CLDSParamPlate*)Ta.Parts.append(CLS_PARAMPLATE);
		pCirPlate->m_hPartWeldParent=pBasePart->handle;
		pCirPlate->m_iParamType=TYPE_WALKWAYCIRPLATE;
		pCirPlate->thick=pParaPlate->_thick;
		pCirPlate->cMaterial=pParaPlate->cMaterial;
		pCirPlate->SetDesignItemValue('A',pBasePart->handle,NULL,PARAM_POS,1);
		pCirPlate->SetDesignItemValue('B',pBaseNode->handle,NULL,PARAM_POS,1);
		pCirPlate->SetDesignItemValue('C',pConnPlate->handle,NULL,PARAM_POS,1);
		pCirPlate->SetDesignItemValue('D',fD,NULL,PARAM_POS);
		//
		pCirPlate->DesignPlate();
		return pCirPlate;	
	}
	else if(pParaPlate->wiType==CStdTemplParaPlate::TYPE_W_CIRBOLTPLATE)
	{
		CLDSNode* pBaseNode=GetLdsNodeFromId(pParaPlate->design.idBaseNode);
		CLDSLineTube* pBaseTube=(CLDSLineTube*)GetLdsRodFromId(pParaPlate->design.idBaseRod);
		CLDSLineAngle* pLinkJg=(CLDSLineAngle*)GetLdsRodFromId(abs(pParaPlate->design.idBasePlate));
		if(pBaseTube==NULL||pLinkJg==NULL||pBaseNode==NULL)
			return NULL;
		CLDSLineAngle* pAssistJg=(CLDSLineAngle*)GetLdsRodFromId(pParaPlate->design.idAssistPart);
		BOLTSET boltSet;
		for(CLsRef* pLsRef=pLinkJg->GetFirstLsRef();pLsRef;pLsRef=pLinkJg->GetNextLsRef())
		{
			CLDSBolt* pBolt=pLsRef->GetLsPtr();
			CLDSNode* pNode=m_pModel->FromNodeHandle(pBolt->des_base_pos.datumPoint.des_para.RODNODE.hNode);
			if(pNode==NULL)
				continue;
			if((pParaPlate->design.idBasePlate>0&&pNode->handle==pLinkJg->pStart->handle)||
				(pParaPlate->design.idBasePlate<0&&pNode->handle==pLinkJg->pEnd->handle))
				boltSet.append(pBolt);
		}
		if(boltSet.GetNodeNum()<=0)
			return NULL;
		double fW=m_xExpression.SolveExpression("PW");
		fW=(fW==0)?30:fW;
		double fB=m_xExpression.SolveExpression("PB");
		fB=(fB==0)?180:fB;
		CLDSParamPlate* pCirPlate=(CLDSParamPlate*)Ta.Parts.append(CLS_PARAMPLATE);
		pCirPlate->m_hPartWeldParent=pBaseTube->handle;
		pCirPlate->thick=pParaPlate->_thick;
		pCirPlate->cMaterial=pParaPlate->cMaterial;
		pCirPlate->m_iParamType=TYPE_CIRRIBPLATE;
		pCirPlate->SetDesignItemValue('C',pBaseTube->handle,NULL,PARAM_POS,1);
		pCirPlate->SetDesignItemValue('P',pBaseNode->handle,NULL,PARAM_POS,1);
		pCirPlate->SetDesignItemValue('E',pLinkJg->handle,NULL,PARAM_POS,1);
		pCirPlate->SetDesignItemValue('W',fW);
		pCirPlate->SetDesignItemValue('B',fB);
		pCirPlate->SetDesignItemValue(KEY2C("ES"),0);
		if(pAssistJg)
			pCirPlate->SetDesignItemValue('F',pAssistJg->handle,NULL,PARAM_POS,1);
		for(CLDSBolt* pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
		{
			pCirPlate->AppendLsRef(pBolt->GetLsRef());
			pBolt->AddL0Thick(pCirPlate->handle,TRUE,TRUE);
			pBolt->correct_pos();
			pBolt->CalGuigeAuto();
		}
		//
		pCirPlate->DesignPlate();
		return pCirPlate;	
	}
	return NULL;
}
//
CLDSBolt* CStdComModel::AppendParaModelBolt(CStdTemplParaBolt* pParaBolt)
{
	CStdTemplParaRod* pPosParaRod=m_pStdTempl->listParaRods.GetValue(pParaBolt->org.idBasePart);
	CLDSLinePart* pBaseRod=GetLdsRodFromId(pParaBolt->org.idBasePart);
	if(pPosParaRod==NULL||pBaseRod==NULL)
		return NULL;
	CLDSNode* pBaseNode=GetLdsNodeFromId(pParaBolt->org.idBaseNode);
	CLDSLinePart* pCrossRod = GetLdsRodFromId(pParaBolt->org.idCrossRod);
	if ((pParaBolt->org.ciDatumPointCalStyle == 2 && pBaseNode == NULL) ||
		(pParaBolt->org.ciDatumPointCalStyle == 3 && pCrossRod == NULL))
		return NULL;
	//��ʼ����˨������Ϣ
	CLDSBolt* pBolt=(CLDSBolt*)Ta.Parts.append(CLS_BOLT);
	pBolt->m_bVirtualPart=pParaBolt->m_ciHoleType>0;
	if(pParaBolt->m_ciHoleType==CStdTemplParaBolt::HOLE_CONNECT_BOLT)
		pBolt->m_cFuncType=CLDSBolt::FUNC_COMMON;
	else if(pParaBolt->m_ciHoleType>=CStdTemplParaBolt::HOLE_CONDUCT_WIRE&&
		pParaBolt->m_ciHoleType<=CStdTemplParaBolt::HOLE_JUMPER_WIRE)
		pBolt->m_cFuncType=CLDSBolt::FUNC_WIREHOLE;
	else if(pParaBolt->m_ciHoleType==CStdTemplParaBolt::HOLE_GROUND_DIVERSION)
		pBolt->m_cFuncType=CLDSBolt::FUNC_EARTHHOLE;
	else //if(pParaBolt->m_ciHoleType>4)
		pBolt->m_cFuncType=CLDSBolt::FUNC_SETUPHOLE;
	pBolt->m_hFamily=pParaBolt->m_hBoltFamily;
	pBolt->set_d(pParaBolt->m_wiDiameter);
	pBolt->hole_d_increment=pParaBolt->m_fHoleIncrement;
	//��ʼ����˨��λ����
	char ciPosDatumWing=pParaBolt->org.ciDatumWing;
	if(ciPosDatumWing>='X')
		ciPosDatumWing-='X';
	if(pPosParaRod->bReverseWingX_Y)
		ciPosDatumWing=1-ciPosDatumWing;
	//��˨λ���趨
	//TODO:�����ľ�δ��ɴ������ wjh-2019.3.6
	pBolt->des_base_pos.hPart=pBaseRod->handle;
	pBolt->des_base_pos.offset_wing=ciPosDatumWing;
	pBolt->des_base_pos.len_offset_dist = 0;
	pBolt->des_base_pos.wing_offset_dist = 0;
	pBolt->des_base_pos.direction = pParaBolt->org.ciLenDirect;
	if (pParaBolt->org.ciDatumPointCalStyle == 1)
	{	//�Ǹ����
		pBolt->des_base_pos.datumPoint.datum_pos_style = 1;
		pBolt->des_base_pos.datumPoint.des_para.RODEND.hRod = pBaseRod->handle;
		pBolt->des_base_pos.datumPoint.des_para.RODEND.direction = pParaBolt->org.ciLenDirect;
		pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing = ciPosDatumWing;
		pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist = 0;
		pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style = 4;
		pBolt->des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect = true;
	}
	else if (pParaBolt->org.ciDatumPointCalStyle == 2)
	{	//�˼��Ͻڵ�
	pBolt->des_base_pos.datumPoint.datum_pos_style=2;
		pBolt->des_base_pos.datumPoint.des_para.RODNODE.hNode = pBaseNode?pBaseNode->handle:0;
	pBolt->des_base_pos.datumPoint.des_para.RODNODE.hRod=pBaseRod->handle;
		pBolt->des_base_pos.datumPoint.des_para.RODNODE.direction = pParaBolt->org.ciLenDirect;
	pBolt->des_base_pos.datumPoint.des_para.RODNODE.offset_wing=ciPosDatumWing;
	pBolt->des_base_pos.datumPoint.des_para.RODNODE.len_offset_dist=pParaBolt->org.siLenOffset;
		pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style = 0;	//g
	}
	else if (pParaBolt->org.ciDatumPointCalStyle == 3)
	{	//�Ǹ����߽���
		if (pCrossRod->GetClassTypeId() == CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle* pGroupAngle = (CLDSGroupLineAngle*)pCrossRod;
			pCrossRod = Ta.FromRodHandle(pGroupAngle->son_jg_h[pParaBolt->org.idWorkSonJg]);
		}
		pBolt->des_base_pos.datumPoint.datum_pos_style = 3;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pBaseRod->handle;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pCrossRod->handle;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1 = 4;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = 
			CStdTemplParaBolt::CalWingOffDist(pBolt->d, pBaseRod->size_wide, pParaBolt->org.szWingG);
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2 = 4;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2 =
			CStdTemplParaBolt::CalWingOffDist(pBolt->d, pCrossRod->size_wide, pParaBolt->org.szCrossWingG);
	}
	else if (pParaBolt->org.ciDatumPointCalStyle == 4)
	{

	}
	//��˨������㷽ʽ:
	pBolt->des_work_norm.norm_style=pParaBolt->normal.ciNormalCalStyle;
	if (pParaBolt->normal.ciNormalCalStyle==1)
	{
		CLDSLinePart* pNormRod = pParaBolt->normal.widBasePart>0?GetLdsRodFromId(pParaBolt->normal.widBasePart):NULL;
		pBolt->des_work_norm.hVicePart =(pNormRod)?pNormRod->handle:pBaseRod->handle;
		pBolt->des_work_norm.norm_wing=ciPosDatumWing;
		pBolt->des_work_norm.direction=pParaBolt->normal.ciNormalDirect;
	}
	//��˨����������ͨ��
	PARTSET xThroughPartSet;
	xThroughPartSet.append(pBaseRod);
	pBolt->AddL0Thick(pBaseRod->handle,TRUE,TRUE);
	pBaseRod->AppendLsRef(pBolt->GetLsRef());
	if (pCrossRod)
	{
		xThroughPartSet.append(pCrossRod);
		pBolt->AddL0Thick(pCrossRod->handle,TRUE,TRUE);
		pCrossRod->AppendLsRef(pBolt->GetLsRef());
	}
	for(int j=0;j<5;j++)
	{
		if(pParaBolt->through[j].cItemType==0)
			break;
		if (pParaBolt->through[j].cItemType == CStdTemplParaBolt::THROUGH_ITEM::THICK_THROUGH && 
			pParaBolt->through[j].val!=0)
			pBolt->AddL0Thick(pParaBolt->through[j].val);
		else if(pParaBolt->through[j].cItemType==CStdTemplParaBolt::THROUGH_ITEM::PART_THROUGH)
		{
			WORD widPart=pParaBolt->through[j].val;
			CLDSPart* pPart=GetLdsPartFromId(widPart, pParaBolt->through[j].cPartType);
			if(pPart&&pPart!=pBaseRod&&pPart!=pCrossRod)
			{
				xThroughPartSet.append(pPart);
				pBolt->AddL0Thick(pPart->handle,TRUE);
				pPart->AppendLsRef(pBolt->GetLsRef());
			}
		}
	}
	CLDSLineAngle* anglearr[2]={NULL,NULL};
	if(pBaseRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)pBaseRod;
		BYTE ciMasterIndex=pGroupAngle->ciMasterIndex;
		anglearr[0]=(CLDSLineAngle*)m_pModel->FromPartHandle(pGroupAngle->son_jg_h[ciMasterIndex],CLS_LINEANGLE);
		if(pGroupAngle->size_cSubClassType=='T'&&(
			(ciMasterIndex==0&&pBolt->des_base_pos.offset_wing==0)||(ciMasterIndex==1&&pBolt->des_base_pos.offset_wing==1)))
			anglearr[1]=(CLDSLineAngle*)m_pModel->FromPartHandle(pGroupAngle->son_jg_h[1-ciMasterIndex],CLS_LINEANGLE);
	}
	else if(pBaseRod->IsAngle())
		anglearr[0]=(CLDSLineAngle*)pBaseRod;
	if(anglearr[0])
		anglearr[0]->AppendLsRef(pBolt->GetLsRef());
	if(anglearr[1])
		anglearr[1]->AppendLsRef(pBolt->GetLsRef());
	if (pBaseRod->IsAngle())
	{
		pBaseRod->CalStartOddment();
		pBaseRod->CalEndOddment();
	}
	//ͳһ����ͨ��ͷ���ƫ����
	pBolt->des_base_pos.norm_offset.EmptyThick();
	pBolt->des_base_pos.norm_offset.key_str = CLDSBolt::EmendZOffsetStr(pBolt, xThroughPartSet);
	pBolt->correct_worknorm();
	pBolt->correct_pos();
	return pBolt;
}
bool CStdComModel::VerifyMatchRods(CStdComTempl* pStdTempl,UINT* parrRodIds/*=NULL*/,UINT count/*=1*/)
{
	//if(parrRodIds==NULL||count<1)
	//	return false;
	m_pStdTempl=pStdTempl;
	BuildAcsOfAssmComponent();
	//��֤�˼�ʼĩ��ƥ�䷽�򼰽Ǹ�֫�����ƥ�����
	for(CStdTemplParaRod* pParaRod=pStdTempl->listParaRods.GetFirst();pParaRod;pParaRod=pStdTempl->listParaRods.GetNext())
	{
		CStdTemplParaNode *pParaNode,*pStartParaNode=NULL,*pEndParaNode=NULL;
		SmartRodPtr pRod=GetLdsRodFromId(pParaRod->Id);
		if(pRod.IsNULL())
		{
			if(pParaRod->m_bNecessaryRod)
				return false;	//ȱ�ٱ������Ӹ˼�
			continue;
		}
		if(pParaRod->idStartNode!=0)
		{
			pParaNode=pStartParaNode=pStdTempl->listParaNodes.GetValue(pParaRod->idStartNode);
			if(pParaNode!=NULL)
				pParaRod->xStartPoint=pParaNode->xPosition;
			else if((pParaNode=pStdTempl->listParaNodes.GetValue(pParaRod->idStartNode.uiOriginalId))!=NULL)
			{
				pParaRod->xStartPoint=pParaNode->xPosition;
				if(pParaRod->idStartNode.ciMirTransType=='X')
					pParaRod->xStartPoint.y*=-1.0;
			}
			if(pParaRod->idEndNode==0)
			{	//��ʼ�˷��������߸˼�
				if(pRod.IsHasPtr()&&pRod->pStart==pParaNode->pRelaNode)
					pParaRod->xEndPoint=pRod->pEnd!=NULL?pRod->pEnd->xOriginalPos:pRod->xPosEnd;
				else if(pRod.IsHasPtr()&&pRod->pEnd==pParaNode->pRelaNode)
					pParaRod->xEndPoint=pRod->pStart!=NULL?pRod->pStart->xOriginalPos:pRod->xPosStart;
			}
		}
		if(pParaRod->idEndNode!=0)
		{
			pParaNode=pEndParaNode=pStdTempl->listParaNodes.GetValue(pParaRod->idEndNode);
			if(pParaNode!=NULL)
				pParaRod->xEndPoint=pParaNode->xPosition;
			else if((pParaNode=pStdTempl->listParaNodes.GetValue(pParaRod->idEndNode.uiOriginalId))!=NULL)
			{
				pParaRod->xEndPoint=pParaNode->xPosition;
				if(pParaRod->idEndNode.ciMirTransType=='X')
					pParaRod->xEndPoint.y*=-1.0;
			}
			if(pParaRod->idStartNode==0)
			{	//���ն˷��������߸˼�
				if(pRod.IsHasPtr()&&pRod->pStart==pParaNode->pRelaNode)
					pParaRod->xStartPoint=pRod->pEnd!=NULL?pRod->pEnd->xOriginalPos:pRod->xPosEnd;
				else if(pRod.IsHasPtr()&&pRod->pEnd==pParaNode->pRelaNode)
					pParaRod->xStartPoint=pRod->pStart!=NULL?pRod->pStart->xOriginalPos:pRod->xPosStart;
			}
		}
		if(pParaRod->idStartNode==0&&pParaRod->idEndNode==0)
		{
			pParaRod->vVerifyLenStdVec=acs.axis_x;
			if((pRod->xPosEnd-pRod->xPosStart)*acs.axis_x>0)
			{
				pParaRod->xStartPoint=pRod->pStart!=NULL?pRod->pStart->xOriginalPos:pRod->xPosStart;
				pParaRod->xEndPoint  =pRod->pEnd!=NULL?pRod->pEnd->xOriginalPos:pRod->xPosEnd;
			}
			else
			{
				pParaRod->xStartPoint=pRod->pEnd!=NULL?pRod->pEnd->xOriginalPos:pRod->xPosEnd;
				pParaRod->xEndPoint  =pRod->pStart!=NULL?pRod->pStart->xOriginalPos:pRod->xPosStart;
			}
		}
		else
			pParaRod->vVerifyLenStdVec=pParaRod->xEndPoint-pParaRod->xStartPoint;
		normalize(pParaRod->vVerifyLenStdVec);

		pParaRod->pRelaRod=pRod;
		pParaRod->m_bNeedUpdateRelaInfo=false;
		pStdTempl->hashMatchRodHid.SetValue(pParaRod->Id,pRod->handle);
		pParaRod->bReverseStartEnd=(pParaRod->vVerifyLenStdVec*(pRod->xPosEnd-pRod->xPosStart)<0);
		pParaRod->bReverseWingX_Y=pParaRod->bReverseStartEnd;
		GEPOINT vxWorkPlaneNormal=acs.axis_z;
		char ciBelongPlaneNo=0;
		CStdTemplParaAngle* pParaAngle=pRod->GetClassTypeId()==CLS_GROUPLINEANGLE?(CStdTemplParaAngle*)pParaRod:NULL;
		CStdTemplParaPlate* pParaPlate=pParaAngle!=NULL?this->m_pStdTempl->listParaPlates.GetValue(1):NULL;
		CONNECT_ROD* xarrConnRodPtr[2]={NULL,NULL};
		if(pParaPlate!=NULL)//&&pParaAngle!=NULL)
		{
			for(UINT i=0;i<pParaPlate->connrods.Count;i++)
			{
				CONNECT_ROD* pConnRod=pParaPlate->connrods.GetAt(i);
				if(pConnRod->sidRod!=pParaRod->Id)
					continue;
				//if(pConnRod->idRod.sidSerial!=pParaAngle->cbGroupAngle.ciMasterIndex)
				//	continue;
				//TODO:Ŀǰ�������죬δ��Ӧ�ɸ��ݻ�׼���Ӱ幤���澫׼�жϵ�ǰ���Ӹ˼��Ĺ���ƥ��״̬
				//ciBelongPlaneNo=pConnRod->ciPlaneNo;
				if(xarrConnRodPtr[0]==NULL)
				{
					xarrConnRodPtr[0]=pConnRod;
					if(pRod->GetClassTypeId()!=CLS_GROUPLINEANGLE)
						break;
				}
				else //if(xarrConnRodPtr[1]==NULL)
				{
					xarrConnRodPtr[1]=pConnRod;
					break;
				}
			}
		}
		//if(ciWorkWing==0)
		//	ciWorkWing=pParaAngle->ciAngleWorkWing;
		if(!pRod->IsAngle())
		{
			logerr.Log("0x%X�˼����ǽǸ֣���ǰ��׼�ڵ㹹�첻֧��!",pRod->handle);
			return false;
		}
		double dx=vxWorkPlaneNormal*pRod.pAngle->vxWingNorm;
		double dy=vxWorkPlaneNormal*pRod.pAngle->vyWingNorm;
		for(int j=0;j<2&&xarrConnRodPtr[j]!=NULL;j++)
		{
			char ciWorkWing=(xarrConnRodPtr[j]->ciConnectWing!=NULL)?xarrConnRodPtr[j]->ciConnectWing:pParaAngle->ciAngleWorkWing;
			if(ciWorkWing==0)
				ciWorkWing=pParaAngle->ciAngleWorkWing;
			if(pRod->IsAngle())
			{
				if( (fabs(dx)>fabs(dy)&&(ciWorkWing=='y'||ciWorkWing=='Y'))||
					(fabs(dx)<fabs(dy)&&(ciWorkWing=='x'||ciWorkWing=='X')))
					pParaRod->bReverseWingX_Y=true;
				else
					pParaRod->bReverseWingX_Y=false;
				if(pRod->GetClassTypeId()!=CLS_GROUPLINEANGLE)
					break;
				else
				{
					if(pRod.pGroupAngle->ciMasterIndex!=pParaAngle->cbGroupAngle.ciMasterIndex)
						pParaRod->bReverseMasterAngle=true;
					else
						pParaRod->bReverseMasterAngle=false;
				}
			}
			/*if(!pParaRod->bReverseStartEnd)
			{	//����ߵ�ʼĩ��
				if(fabs(dx)>fabs(dy)&&(dx>0&&ciWorkWing=='x'||dx<0&&ciWorkWing=='X'))
				{
					pParaRod->bReverseWingX_Y=false;
					pParaRod->bReverseMasterAngle=(xarrConnRodPtr[j]->idRod.sidSerial!=pParaAngle->cbGroupAngle.ciMasterIndex);
				}
				else if(fabs(dx)<fabs(dy)&&(dy>0&&ciWorkWing=='y'||dy<0&&ciWorkWing=='Y'))
				{
					pParaRod->bReverseWingX_Y=false;
					pParaRod->bReverseMasterAngle=(xarrConnRodPtr[j]->idRod.sidSerial!=pParaAngle->cbGroupAngle.ciMasterIndex);
				}
			}
			else
			{
				if(fabs(dx)>fabs(dy)&&(dx>0&&ciWorkWing=='y'||dx<0&&ciWorkWing=='Y'))
				{
					pParaRod->bReverseWingX_Y=true;
					pParaRod->bReverseMasterAngle=(xarrConnRodPtr[j]->idRod.sidSerial!=pParaAngle->cbGroupAngle.ciMasterIndex);
				}
				else if(fabs(dx)<fabs(dy)&&(dy>0&&ciWorkWing=='x'||dy<0&&ciWorkWing=='X'))
				{
					pParaRod->bReverseWingX_Y=true;
					pParaRod->bReverseMasterAngle=(xarrConnRodPtr[j]->idRod.sidSerial!=pParaAngle->cbGroupAngle.ciMasterIndex);
				}
			}*/
		}
	}
	return true;
}
bool CStdComModel::RetrieveConnPoint(CConnectPointInfo* pConnPoint,
	CLDSNode* pNode,CLDSLinePart* pBaseRod,const double* vzStdWorkPlaneNormal,bool bIncBendConnect/*=true*/)
{
	LINEPARTSET linkPartSet;	//�����ڵ�ǰ�ڵ��ϵĸ˼�����
	CSuperSmartPtr<CLDSLinePart> pRod,pRod1,pRod2;
	GEPOINT vStdPlaneNormal(vzStdWorkPlaneNormal);
	if(vzStdWorkPlaneNormal==NULL||vStdPlaneNormal.IsZero())
		return false;
	//1����ȡ�ڵ㸸�˼�
	if(pNode->m_cPosCalType==4)
	{	//���ݽ���ڵ㸸�˼�,У������㴦��ϵĽ����ĸ��˼�
		pRod1=m_pModel->FromRodHandle(pNode->arrRelationPole[0]);
		pRod2=m_pModel->FromRodHandle(pNode->arrRelationPole[1]);
		if(pRod1.IsHasPtr()&&pRod1->pStart&&pRod1->pStart->handle!=pNode->handle
			&&pRod1->pEnd&&pRod1->pEnd->handle!=pNode->handle)
			pNode->hFatherPart=pNode->arrRelationPole[0];
		else if(pRod2.IsHasPtr()&&pRod2->pStart&&pRod2->pStart->handle!=pNode->handle
			&&pRod2->pEnd&&pRod2->pEnd->handle!=pNode->handle)
			pNode->hFatherPart=pNode->arrRelationPole[1];
	}
	CSuperSmartPtr<CLDSLinePart> pDatumRod=pBaseRod!=NULL?pBaseRod:m_pModel->FromRodHandle(pNode->hFatherPart);
	if(pDatumRod.IsNULL()||pDatumRod->pStart==NULL||pDatumRod->pEnd==NULL)
		return false;	//��׼�˼����Ϸ�
	if(!pDatumRod->IsAngle()&&pDatumRod->GetClassTypeId()!=CLS_LINETUBE)
		return false;	//��ʱֻ֧�ֻ�׼����Ϊ�Ǹ֣���ϽǸ֣��ֹܵ����
	//2������ȫ���������ڴ˽ڵ��ϵĸ˼���������linkPartSet�����а������˼�����ȥ����
	CLDSModule *pModule=NULL;
	if(m_pModel->IsTowerModel())
		pModule=((CTower*)m_pModel)->GetActiveModule();
	BOOL pushed=m_pModel->PushPartStack(); //���������б�ǰӦ��ѹջ wht 12-03-05
	for(pRod=m_pModel->EnumRodFirst();pRod.IsHasPtr();pRod=m_pModel->EnumRodNext())
	{	//Ӧʹ��IsSonPart�ж��Ƿ�Ϊ��ǰģ���еĹ�������Ӧʹ��ģ����ĺűȽ� wht 10-08-02
		if(!pRod->IsAngle()&&pRod->GetClassTypeId()!=CLS_LINETUBE)
			continue;	//��֧�ֽǸ֡��ֹ�
		if(pRod->GetClassTypeId()==CLS_LINEANGLE&&pRod.pAngle->group_father_jg_h>0x20)
			continue;	//��ϽǸֵ��ӽǸ�����ϽǸִ����������
		if(pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;	//�ǳ������ӿ�ܸ˼�
		if(pRod->pStart!=pNode&&pRod->pEnd!=pNode)
			continue;	//���κ�һ��˽ڵ�Ϊ��ǰ�ڵ�
		if(!pRod->ModuleTogetherWith(pNode)||(pModule&&!pModule->IsSonPart(pRod)))
			continue;	//�ǵ�ǰ������߷������� wht 10-07-23
		if(pDatumRod.IsEqualPtr(pRod))
			continue;	//linkPartSet��ֻ�������߸˼�
		linkPartSet.append(pRod);
	}
	m_pModel->PopPartStack(pushed);
	LDSNORMAL datumlinevec,linevec,vRayDatumLineVec;
	f3dPoint base_norm_wing_x,base_norm_wing_y;
	if(pDatumRod->IsAngle())
	{	//��ȡ���ӻ�׼�Ǹ���֫�ķ��߷��򣬼��������ӹ����淨��
		base_norm_wing_x=pDatumRod.LineAnglePointer()->vxWingNorm;
		base_norm_wing_y=pDatumRod.LineAnglePointer()->vyWingNorm;
	}
	datumlinevec.vLine=pDatumRod->pEnd->xOriginalPos-pDatumRod->pStart->xOriginalPos;
	datumlinevec.vSolid=pDatumRod->End()-pDatumRod->Start();
	normalize(datumlinevec.vLine);
	normalize(datumlinevec.vSolid);
	vRayDatumLineVec=datumlinevec;
	if (pDatumRod->pEnd==pNode)
	{	//��֤��׼�˼�Ϊ��ǰ�ڵ����߸˼�ʱ��vRayDatumLineVecΪ���߷��� wjh-2019.11.03
		vRayDatumLineVec.vLine *=-1.0;
		vRayDatumLineVec.vSolid*=-1.0;
	}
	if(pNode->m_cPosCalType==4&&linkPartSet.GetNodeNum()<=1)
	{	//3.����㵥��˨����
		pRod1=m_pModel->FromRodHandle(pNode->arrRelationPole[0]);
		pRod2=m_pModel->FromRodHandle(pNode->arrRelationPole[1]);
		if(pRod1.IsHasPtr()&&pRod1->IsAngle()&&pRod2.IsHasPtr()&&pRod2->IsAngle())
		{
			f3dPoint line_vec1=pRod1->xPosEnd-pRod1->xPosStart;
			f3dPoint line_vec2=pRod2->xPosEnd-pRod2->xPosStart;
			//����vLine��ʵ�������ȡֵvSolid
			GEPOINT vNormal=line_vec1^line_vec2;
			normalize(vNormal);
			if(fabs(vNormal*vStdPlaneNormal)<0.82)	//������Խ�������һ��ͶӰ�߼н�����Ϊ0.8165
				return false;
			pConnPoint->m_hBaseNode=pNode->handle;
			pConnPoint->m_hBasePart=pDatumRod->handle;
			pConnPoint->m_iConnectWing=2;
			pConnPoint->designType.bIntersBolt = TRUE;
			pConnPoint->datumplane.vLine=pConnPoint->datumplane.vSolid=vNormal;
			return true;
		}
		else
			return false;
	}
	//4��ͳ�Ʒ��������߸˼����׼�˼��γɵ��������ӵ���Ϣ
	//������Ȳ����ı���Ҫ��ڵ�����ӹ���λ���²�������
	if(pDatumRod->layer(0)!='T'&&pDatumRod->layer(1)=='Z'&&pDatumRod->pEnd==pNode)
	{
		datumlinevec.vLine *=-1.0;//��֤datum_line_vecΪ���߷���
		datumlinevec.vSolid*=-1.0;
	}
	bool swapCoupleAngle=false;	//�Ƿ��ѽ�pDatumRod��pCoupleLinePart�����˽���
	CLDSLinePart *pCoupleLinePart=NULL;
	for(pRod=linkPartSet.GetFirst();pRod.IsHasPtr();pRod=linkPartSet.GetNext())
	{
		int base_bolt_num = 0;	//���ӻ�׼�����ϵ���˨��
		if(pRod->pStart->handle==pNode->handle)
		{
			linevec.vLine=pRod->pEnd->xOriginalPos-pRod->pStart->xOriginalPos;
			linevec.vSolid=pRod->End()-pRod->Start();
		}
		else
		{
			linevec.vLine=pRod->pStart->xOriginalPos-pRod->pEnd->xOriginalPos;
			linevec.vSolid=pRod->Start()-pRod->End();
		}
		normalize(linevec.vLine);
		normalize(linevec.vSolid);
		if(fabs(linevec.vLine*vRayDatumLineVec.vLine)>EPS_COS)
		{	//��Ҫ��ƽ�ͷ�Ĺ��߸˼�
			if(pCoupleLinePart==NULL)
				pCoupleLinePart=pRod;
			else if(!swapCoupleAngle&&pCoupleLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(((CLDSLineAngle*)pCoupleLinePart)->group_father_jg_h==pRod->handle)
					pCoupleLinePart=pRod;
			}
			else if(swapCoupleAngle&&pDatumRod->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(pDatumRod.GroupAnglePointer()->group_father_jg_h==pRod->handle)
					pDatumRod=pRod;
			}
			if(!swapCoupleAngle&&vRayDatumLineVec.vLine.z<0&&linevec.vLine.z>0)
			{	//���߸˼���ͷ��׼�˼�ȡ�²�ȡ��, wjh-2015.1.17
				CLDSLinePart* pTempRod=pDatumRod;
				pDatumRod=pCoupleLinePart;
				pCoupleLinePart=pTempRod;
				swapCoupleAngle=true;
			}
			continue;	//���߸˼�
		}
		LDSNORMAL worknorm;
		worknorm.vLine=linevec.vLine^datumlinevec.vLine;
		worknorm.vSolid=linevec.vSolid^datumlinevec.vSolid;
		normalize(worknorm.vLine);
		normalize(worknorm.vSolid);
		CMultiOffsetPos *pDesPosPara=NULL;
		if(pRod->IsAngle())//GetClassTypeId()==CLS_LINEANGLE)
		{
			if(pRod->pStart==pNode)
			{
				pDesPosPara=&pRod.LineAnglePointer()->desStartPos;
				base_bolt_num=pRod->connectStart.wnConnBoltN;
			}
			else if(pRod->pEnd==pNode)
			{
				pDesPosPara=&pRod.LineAnglePointer()->desEndPos;
				base_bolt_num=pRod->connectEnd.wnConnBoltN;
			}
		}
		//�޳�δ�����ӹ������ϵĽǸ�
		if(pDatumRod->IsAngle()&&pDesPosPara)
		{	//��׼�˼�Ϊ�Ǹ�
			bool passcheck=false;	//����������ų�����������Ӹ˼�
			if(fabs(worknorm.vSolid*base_norm_wing_x)>EPS_COS2||fabs(worknorm.vSolid*base_norm_wing_y)>=EPS_COS2)
				passcheck=true;	//δ����ƫ����������֫
			else if(pDesPosPara->IsInHuoQuLine()&&(pDesPosPara->IsInDatumJgWingX()||
				pDesPosPara->IsInDatumJgWingY()||pDesPosPara->huoqu_pos_auto_0_x_wing1_y_wing2>0))	//δָ����ǰ�Ǹ���һ֫Ϊ��λ����֫
				passcheck=true;	//��ָ����ǰ�Ǹ�ĳһ֫Ϊ�����߶�λ����֫
			else
				continue;	//�������ӽǸ�δ���ӵ���ǰ����֫��
		}
		if(pConnPoint->datumplane.vLine.IsZero())
		{	//��ʼ�����ӵ��׼ƽ����Ϣ
			if(fabs(vStdPlaneNormal*worknorm.vLine)<0.82)
				continue;	//�ǵ�ǰ����ƽ������Ӹ˼�
			pConnPoint->cfgword=pDatumRod->UnifiedCfgword()&pRod->UnifiedCfgword();
			pConnPoint->linkPartSet.append(pRod);
			pConnPoint->m_hBaseNode=pNode->handle;
			pConnPoint->m_hBasePart=pDatumRod->handle;
			pConnPoint->vDatumDirect=datumlinevec.vLine^worknorm.vLine;
			if(pConnPoint->vDatumDirect*linevec.vLine<0)
				pConnPoint->vDatumDirect*=-1.0;
			pConnPoint->datumplane=worknorm;
			if(pRod->pStart==pNode)
				pConnPoint->m_nMaxRayLsNum=pRod->connectStart.wnConnBoltN;
			else
				pConnPoint->m_nMaxRayLsNum=pRod->connectEnd.wnConnBoltN;
			if(pDatumRod->IsAngle())
			{	//��׼�˼�Ϊ�Ǹ�ʱ���жϻ�׼�Ǹ�����֫
				double ddx=pConnPoint->datumplane.vSolid*pDatumRod.LineAnglePointer()->get_norm_x_wing();
				double ddy=pConnPoint->datumplane.vSolid*pDatumRod.LineAnglePointer()->get_norm_y_wing();
				if(fabs(ddx)>fabs(ddy))
				{
					pConnPoint->m_iConnectWing=0;	//���ӹ̽��ڻ�׼�Ǹ�X֫��
					if (ddx<0)
						pConnPoint->datumplane.vSolid *= -1.0;	
				}	
				else
				{
					pConnPoint->m_iConnectWing=1;	//���ӹ̽��ڻ�׼�Ǹ�Y֫��
					if (ddy<0)
						pConnPoint->datumplane.vSolid *= -1.0;	
				}
				if(pConnPoint->datumplane.vLine*pConnPoint->datumplane.vSolid<0)
					pConnPoint->datumplane.vLine*=-1.0;
			}
			else if(pDatumRod->GetClassTypeId()==CLS_LINETUBE)
			{
				if(pConnPoint->vDatumDirect.IsZero())
					pConnPoint->vDatumDirect=datumlinevec.vLine^pConnPoint->datumplane.vLine;
				if(pConnPoint->vDatumDirect*linevec.vLine>0)
					pConnPoint->m_iConnectWing=0;	//��׼�˼��Ҳ�
				else
					pConnPoint->m_iConnectWing=1;	//��׼�˼����
			}
			else
				pConnPoint->m_iConnectWing=2;
			continue;
		}

		if(pRod->IsAngle()&&pDesPosPara&&pDatumRod->IsAngle())
		{	//��׼�˼������߸˼���Ϊ�Ǹ�
			if( bIncBendConnect&&
				!pConnPoint->bendplane.vLine.IsZero()&&linevec.vLine*pConnPoint->vDatumDirect<0&&
				fabs(linevec.vLine*pConnPoint->bendplane.vLine)<=0.01)
			{
				pConnPoint->bendPlaneRodSet.append(pRod);
				continue;	//�˴�break����©��ͬʱ����˫����ƽ��ĸ˼�
			}
			if(fabs(linevec.vLine*pConnPoint->datumplane.vLine)<=0.01)
			{
				pConnPoint->linkPartSet.append(pRod);
				continue;	//�˴�break����©��ͬʱ������׼����ƽ�������˼�
			}
			else if(bIncBendConnect&&pConnPoint->bendplane.vLine.IsZero()&&!worknorm.vLine.IsZero())
			{
				if((pConnPoint->m_iConnectWing==0&&
					fabs(worknorm.vSolid*base_norm_wing_x)>fabs(pConnPoint->datumplane.vSolid*base_norm_wing_x))||(
					pConnPoint->m_iConnectWing==1&&
					fabs(worknorm.vSolid*base_norm_wing_y)>fabs(pConnPoint->datumplane.vSolid*base_norm_wing_y)))
				{	//������׼ƽ�������ƽ��
					pConnPoint->bendplane=pConnPoint->datumplane;
					pConnPoint->vDatumDirect*=-1.0;
					for(CLDSLinePart* pTmpRod=pConnPoint->linkPartSet.GetFirst();pTmpRod;pTmpRod=pConnPoint->linkPartSet.GetNext())
						pConnPoint->bendPlaneRodSet.append(pTmpRod);
					pConnPoint->linkPartSet.Empty();
					pConnPoint->linkPartSet.append(pRod);
					pConnPoint->datumplane=worknorm;
				}
				else
				{
					BYTE ciWorkWing=-1;
					//char cDatumAngleWing=pLineAngle->GetLappedWorkWing(TRUE,NULL);
					//pDesPosPara->cDatumAngleWing
					if(pDesPosPara->IsInDatumJgWingX()||(pDesPosPara->IsInHuoQuLine()&&pDesPosPara->huoqu_pos_auto_0_x_wing1_y_wing2==1))
						ciWorkWing=0;
					else if(pDesPosPara->IsInDatumJgWingY()||(pDesPosPara->IsInHuoQuLine()&&pDesPosPara->huoqu_pos_auto_0_x_wing1_y_wing2==2))
						ciWorkWing=1;
					else if(fabs(worknorm.vSolid*base_norm_wing_x)>EPS_COS2)
						ciWorkWing=0;
					else if(fabs(worknorm.vSolid*base_norm_wing_y)>EPS_COS2)
						ciWorkWing=1;
					if(pConnPoint->vDatumDirect*linevec.vLine>0)
						continue;
					if(pConnPoint->m_iConnectWing==ciWorkWing)
					{
						pConnPoint->bendplane=worknorm;
						pConnPoint->bendPlaneRodSet.append(pRod);
					}
					else	//�뵱ǰ���ӵ㲻ƥ��
						continue;
				}
				continue;	//break;
			}
			else	//�����Ȳ��ڻ�׼����ƽ�棬�ֲ��ڻ������ϵ����Ӹ˼�
				continue;
			if(base_bolt_num>pConnPoint->m_nMaxRayLsNum)
				pConnPoint->m_nMaxRayLsNum=base_bolt_num;
		}
		else if(fabs(linevec.vLine*pConnPoint->datumplane.vLine)<=0.01)
		{	//���ڴ��ڲ����˹����빲�����꣬��ʹ����ͼ���Ҳ���ܴ���EPS����ȡ0.01����0.57��) wjh-2016.1.11
			if( pDatumRod->GetClassTypeId()==CLS_LINETUBE)
			{
				if(pConnPoint->vDatumDirect.IsZero())
					pConnPoint->vDatumDirect = datumlinevec.vLine^pConnPoint->datumplane.vLine;
				if (pConnPoint->vDatumDirect*linevec.vLine>0&&pConnPoint->m_iConnectWing==1)
					continue;	//�˼����Ҳ࣬��Ƶ����������
				else if(pConnPoint->vDatumDirect*linevec.vLine<0&&pConnPoint->m_iConnectWing==0)
					continue;	//�˼�����࣬��Ƶ��������Ҳ�
			}
			pConnPoint->linkPartSet.append(pRod);
		}
		else
			continue;
	}
	pConnPoint->m_hBasePart=pDatumRod->handle;
	if(pConnPoint->bendPlaneRodSet.GetNodeNum()>0&&pConnPoint->m_hBendlineAngle==0)
		pConnPoint->m_hBendlineAngle=pDatumRod->handle;
	//5����ӽǸֽ�ͷ�������ӵ�
	/*if(pCoupleLinePart)
	{	//�Ǹֽ�ͷ��Խӷ���
		pConnPoint=GetConnectPointInfoList()->append();
		pConnPoint->m_hBaseNode=pNode->handle;
		pConnPoint->m_hBasePart=pDatumRod->handle;
		pConnPoint->linkPartSet.append(pCoupleLinePart);
		pConnPoint->m_hJointPart = pCoupleLinePart->handle;
		pConnPoint->m_iConnectWing=2;
		pConnPoint->cfgword=pDatumRod->UnifiedCfgword()&pCoupleLinePart->UnifiedCfgword();
		if (pCoupleLinePart->GetClassTypeId()==CLS_LINETUBE)
			pConnPoint->designType.bFlJoint = TRUE;
		else
			pConnPoint->designType.bAngleJoint = TRUE;
	}*/
	//��ʼ����׼�˼�������״̬
	GEPOINT vLenToStart = pDatumRod->pStart->xOriginalPos - pNode->xOriginalPos;
	GEPOINT vLenToEnd = pDatumRod->pEnd->xOriginalPos - pNode->xOriginalPos;
	if (vLenToStart*vLenToEnd < -EPS)
		pConnPoint->m_ciBaseRodConnState = 1;	//1.�м����ӵ�
	if (pCoupleLinePart != NULL)
		pConnPoint->m_ciBaseRodConnState = 4;	//4.���߽ضϵ�����
	else if (pConnPoint->m_ciBaseRodConnState == 0)
	{
		GEPOINT vPrimaryVec = -vLenToStart;
		if (vPrimaryVec.IsZero())
			vPrimaryVec = -vLenToEnd;
		normalize(vPrimaryVec);
		for (pRod = linkPartSet.GetFirst(); pRod.blValid; pRod = linkPartSet.GetNext())
		{
			GEPOINT vLenRayVec;
			if (pRod->pStart && pRod->pEnd)
			{
				if (pRod->pStart == pNode)
					vLenRayVec = pRod->pEnd->xOriginalPos - pNode->xOriginalPos;
				else if (pRod->pEnd == pNode)
					vLenRayVec = pRod->pStart->xOriginalPos - pNode->xOriginalPos;
			}
			else
				continue;
			normalize(vLenRayVec);
			double justify = vPrimaryVec * vLenRayVec;
			if (justify >= 0.5)
			{
				pConnPoint->m_ciBaseRodConnState = 3;	//3.�νӶνضϵ�����
				break;
			}
		}
		if (pConnPoint->m_ciBaseRodConnState == 0)
			pConnPoint->m_ciBaseRodConnState = 2;		//2.�νӶ�ĩ������
	}
	//6�������ӵ����������������׼�˼���˨��������������֧�̶ֳȵȽ��г����ж����������ӷ���
	JGZJ jgzj;
	getjgzj(jgzj,pDatumRod->GetWidth());
	//if(pConnPoint->designType.bAngleJoint||pConnPoint->designType.bFlJoint||pConnPoint->designType.bIntersBolt)
	//	return false;	//��ͷ������������㵥��˨���
	if(pConnPoint->linkPartSet.GetNodeNum()==1)
	{	//����������
		CSuperSmartPtr<CLDSLinePart>pRayRod=pConnPoint->linkPartSet.GetFirst();
		if(pDatumRod->IsAngle()&&pRayRod->IsAngle())
		{
			if((pRayRod->pStart->handle==pNode->handle&&pRayRod->connectStart.wnConnBoltN==1)
				||(pRayRod->pEnd->handle==pNode->handle&&pRayRod->connectEnd.wnConnBoltN==1))
				pConnPoint->designType.bL_1LB1 = TRUE;//����˨�ް�����
			else if(jgzj.g1>0&&&jgzj.g2>0&&(
				(pRayRod->pStart->handle==pConnPoint->m_hBaseNode&&pRayRod->connectStart.wnConnBoltN==2)||
				(pRayRod->pEnd->handle==pConnPoint->m_hBaseNode&&pRayRod->connectEnd.wnConnBoltN==2)))
				pConnPoint->designType.bL_1LB2 = TRUE;//˫��˨�ް�����
			if(pConnPoint->m_nMaxRayLsNum>1&&pConnPoint->m_nMaxRayLsNum<3)
				pConnPoint->designType.bL_1LE1 = TRUE;//����Ϊ������˨�Ķ�����
			else if(pConnPoint->m_nMaxRayLsNum>=3&&jgzj.g1>0&&jgzj.g2>0)
				pConnPoint->designType.bL_1LE2 = TRUE;//����Ϊ˫����˨�Ķ�����
			//pConnPoint->designType.bL_1LO1=TRUE;//�ǿ϶��Ĳ����ж�
		}
	}
	else if (pConnPoint->linkPartSet.GetNodeNum()==2)
	{	//˫���߸˼�����
		CSuperSmartPtr<CLDSLinePart>pRayRodArr[2];
		pRayRodArr[0]=pConnPoint->linkPartSet.GetFirst();
		pRayRodArr[1]=pConnPoint->linkPartSet.GetNext();
		if(pDatumRod->IsAngle()&&pRayRodArr[0]->IsAngle()&&pRayRodArr[1]->IsAngle())
		{
			CConnectInfo *pLsInfoArr[2]={NULL,NULL};
			CMultiOffsetPos *pDesParaArr[2]={NULL,NULL};
			for(int i=0;i<2;i++)
			{
				if(pRayRodArr[i]->pStart==pNode)
				{
					pLsInfoArr[i]=&pRayRodArr[i]->connectStart;
					pDesParaArr[i]=&pRayRodArr[i].pAngle->desStartPos;
				}
				else if(pRayRodArr[i]->pEnd==pNode)
				{
					pLsInfoArr[i]=&pRayRodArr[i]->connectEnd;
					pDesParaArr[i]=&pRayRodArr[i].pAngle->desEndPos;
				}
				else 
					return false;
			}
			//��ͬ�������߸˼�����˨����ڻ�׼�Ǹ�,��Ҫʱ�ɽ��������߽Ǹֶ�ͷ����ƫ�ƿ�һ����϶��
			if(pLsInfoArr[0]->wnConnBoltN<=1&&pLsInfoArr[1]->wnConnBoltN<=1)
				pConnPoint->designType.bL_2LB1 = TRUE;
			//����ͬ�������߸˼�����ڻ�׼�Ǹ�,����һ����˨
			if(pLsInfoArr[0]->wnConnBoltN<=1&&pLsInfoArr[1]->wnConnBoltN<=1)
				pConnPoint->designType.bL_2LB2 = TRUE;//����˨�ް�����
			else if(pLsInfoArr[0]->wnConnBoltN>=1&&pLsInfoArr[1]->wnConnBoltN>=1&&jgzj.g1>0&&jgzj.g2>0)
				pConnPoint->designType.bL_2LB3 = TRUE;//˫��˨�ް�����
			//��ͬ�������߸˼�˫��˨����ڻ�׼�Ǹ�,��Ҫʱ�ɽ��������߽Ǹֶ�ͷ����ƫ�ƿ�һ����϶��
			if(max(pLsInfoArr[0]->wnConnBoltN,pLsInfoArr[1]->wnConnBoltN)==2&&jgzj.g1>0&&jgzj.g2>0)
				pConnPoint->designType.bL_2LB4 = TRUE;//˫��˨�ް�����
			if ((pLsInfoArr[0]->wnConnBoltN>1||pLsInfoArr[1]->wnConnBoltN>1)&&(pLsInfoArr[0]->wnConnBoltN<=3||pLsInfoArr[1]->wnConnBoltN<=3))
			{
				pConnPoint->designType.bL_2LE1 = TRUE;//����Ϊ������˨�Ķ�����
				pConnPoint->designType.bL_2LC1 = TRUE;//����Ϊ������˨�Ļ������
			}
			/*else */if(jgzj.g1>0&&jgzj.g2>0&&//����5����˨ʱҲ�������Ȱ����Ŷ����Ӳ��� wjh-2012.5.29
				(pLsInfoArr[0]->wnConnBoltN>=2||pLsInfoArr[1]->wnConnBoltN>=2)/*&&(pLsInfoArr[0]->N<5||pLsInfoArr[1]->N<5)*/)
			{
				pConnPoint->designType.bL_2LE2 = TRUE;//����Ϊ˫����˨�Ķ�����
				pConnPoint->designType.bL_2LC2 = TRUE;//����Ϊ˫����˨�Ļ������
			}
		}
	}
	else
	{	//�����߸˼�����
	}
	/*
	//��ʱ��֧������ ���ð� wht 10-07-23
	if(sharePlatePole.GetNodeNum()>1)
	{	//���ð�
		pConnectPoint=GetConnectPointInfoList()->append();
		pConnectPoint->m_hBaseNode=handle;
		pConnectPoint->m_hBasePart=sharePlatePole.GetFirst()->handle;
		pConnectPoint->work_norm=f3dPoint(0,0,-1);
		normalize(pConnectPoint->work_norm);
		for(pRod=sharePlatePole.GetFirst();pRod.IsHasPtr();pRod=sharePlatePole.GetNext())
		{
			pConnectPoint->linkPartSet.append(pRod->handle);
			pConnectPoint->cfgword&=pRod->Unifiedfgword();
			if(pRod->GetClassTypeId()==CLS_LINEANGLE)
			{
				if((pRod.LineAnglePointer()->get_norm_x_wing()*f3dPoint(0,0,1))>EPS_COS2)
					pConnectPoint->m_iConnectWing=0;
				else
					pConnectPoint->m_iConnectWing=1;
			}
		}
	}*/
	return true;
}
struct WIRE_HOLE{
	short siLenPosition;
	short siAngleG;		//�����ĽǸ��ľ�
	double dfDiameter;
	WIRE_HOLE(){dfDiameter=26;siLenPosition=siAngleG=0;}
};
int SplitWireHolesFromStr(const char* szHolesStr,WIRE_HOLE* holes,UINT niMaxHoles=0)
{
	DELIMITER_STR delimiter(",");
	CXhChar50 xarrWords[8];
	UINT holecount=DELIMITER_STR::ParseWordsFromStr(szHolesStr,",",xarrWords,8);
	if(holecount>niMaxHoles&&niMaxHoles>0)
		holecount=niMaxHoles;
	for(UINT i=0;i<holecount;i++)
	{
		char* pchSpliter=xarrWords[i];
		char* pKey=NULL;
		WIRE_HOLE wirehole;
		if(*pchSpliter=='D'||*pchSpliter=='d')
		{
			pKey=pchSpliter+1;
			bool findFinalSymbol=false;
			while(*pchSpliter>0)
			{
				if(*pchSpliter=='@'||*pchSpliter=='#')
				{
					char chOldSpliter=*pchSpliter;
					*pchSpliter=0;
					wirehole.dfDiameter=atof(pKey);
					*pchSpliter=chOldSpliter;
					findFinalSymbol=true;
					break;
				}
				pchSpliter++;
			}
			if(!findFinalSymbol)
				wirehole.dfDiameter=atof(pKey);
		}
		if(*pchSpliter=='@'||*pchSpliter=='#')
		{
			char chCurrSpliter=*pchSpliter;
			pKey=pchSpliter+1;
			bool findFinalSymbol=false;
			while(*pchSpliter>0)
			{
				if(*pchSpliter=='@'||*pchSpliter=='#')
				{
					char chOldSpliter=*pchSpliter;
					*pchSpliter=0;
					if(chCurrSpliter=='@')
						wirehole.siLenPosition=atoi(pKey);
					else if(chCurrSpliter=='#')
						wirehole.siAngleG=atoi(pKey);
					*pchSpliter=chOldSpliter;
					findFinalSymbol=true;
					break;
				}
				pchSpliter++;
			}
			if(!findFinalSymbol&&chCurrSpliter=='@')
				wirehole.siLenPosition=atoi(pKey);
			else if(!findFinalSymbol&&chCurrSpliter=='#')
				wirehole.siAngleG=atoi(pKey);
			if(*pchSpliter=='@'||*pchSpliter=='#')
			{
				if(*pchSpliter=='@')
					wirehole.siLenPosition=atoi(pchSpliter+1);
				else if(*pchSpliter=='#')
					wirehole.siAngleG=atoi(pchSpliter+1);
			}
		}
		holes[i]=wirehole;
	}
	return holecount;
}
extern void MirDatumPoint(IModel *pTower, CLDSPoint &srcPoint, CLDSPoint &mirPoint, MIRMSG msg);
extern void MirDatumVector(IModel *pTower, CLDSVector &srcVector, CLDSVector &mirVector, MIRMSG msg, f3dPoint *pPickPos=NULL);
extern void MirLinePartOddDesignInfo(CLDSLinePart* pSrcLinePart, CLDSLinePart* pMirLinePart, MIRMSG msg, bool bSrcStart, bool bMirStart);
extern void MirLineAnglePosDesignInfo(CLDSLineAngle* pSrcAngle, CLDSLineAngle* pMirAngle, MIRMSG msg, bool bSrcStart, bool bMirStart, bool bCompatibleWing);
#include "TimerCount.h"
bool CStdComModel::MirrorNewModelParts(CStdComTempl* pStdTempl)
{
	struct MIR_ITEM {
		CLDSPart* m_pSrcPart;
		BYTE m_ciMirFlag;
		//
		MIR_ITEM(CLDSPart* pPart = NULL, BYTE mir_flag = 0) :m_pSrcPart(pPart), m_ciMirFlag(mir_flag) { ; }
	};
	//��ȡ����жԳƵĹ���
	CHashList<MIR_ITEM> hashRodMirItem, hashBoltMirItem, hashPlateMirItem, hashFittingMirItem;
	for(CStdTemplParaRod* pParaRod=pStdTempl->listParaRods.GetFirst();pParaRod;pParaRod=pStdTempl->listParaRods.GetNext())
	{
		SmartRodPtr pRod = GetLdsRodFromId(pParaRod->Id);
		if (pRod.IsNULL() || pParaRod->cbMirCreateFlag == 0)
			continue;	//����Ҫ�Գ�
		SmartRodPtr pMirRod = pRod->GetMirRod(MIRMSG(pParaRod->cbMirCreateFlag));
		if(pMirRod.IsHasPtr())
			continue;
		hashRodMirItem.SetValue(pRod->handle, MIR_ITEM(pRod,pParaRod->cbMirCreateFlag));
	}
	for (CStdTemplParaPlate* pParaPlate=pStdTempl->listParaPlates.GetFirst();pParaPlate;pParaPlate=pStdTempl->listParaPlates.GetNext())
	{
		CLDSGeneralPlate* pPlate = GetLdsPlateFromId(pParaPlate->Id);
		if (pPlate == NULL || pParaPlate->cbMirCreateFlag == 0)
			continue;	//����Ҫ�Գ�
		CLDSGeneralPlate* pMirPlate = (CLDSGeneralPlate*)pPlate->GetMirPart(MIRMSG(pParaPlate->cbMirCreateFlag));
		if(pMirPlate)
			continue;
		hashPlateMirItem.SetValue(pPlate->handle, MIR_ITEM(pPlate, pParaPlate->cbMirCreateFlag));
		//
		for(CLsRef* pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
			hashBoltMirItem.SetValue(pLsRef->pBolt->handle, MIR_ITEM(pLsRef->pBolt, pParaPlate->cbMirCreateFlag));
	}
	for (CStdTemplParaBolt* pParaBolt = pStdTempl->listParaBolts.GetFirst(); pParaBolt; pParaBolt = pStdTempl->listParaBolts.GetNext())
	{
		long *pHid = m_pStdTempl->hashMatchBolts.GetValue(pParaBolt->Id);
		CLDSBolt* pBolt = (pHid) ? (CLDSBolt*)m_pModel->FromPartHandle(*pHid) : NULL;
		if (pBolt == NULL || pParaBolt->cbMirCreateFlag == 0)
			continue;	//����Ҫ�Գ�
		if(hashBoltMirItem.GetValue(pBolt->handle)==NULL)
			hashBoltMirItem.SetValue(pBolt->handle, MIR_ITEM(pBolt, pParaBolt->cbMirCreateFlag));
		//��ӽ��
		pHid = m_pStdTempl->hashMatchFittings.GetValue(pParaBolt->Id);
		CLDSFitting* pFitting = (pHid) ? (CLDSFitting*)m_pModel->FromPartHandle(*pHid) : NULL;
		if (pFitting==NULL)
			continue;
		hashFittingMirItem.SetValue(pFitting->handle, MIR_ITEM(pFitting, pParaBolt->cbMirCreateFlag));
	}
	//�Գƴ���
	MIR_ITEM* pItem = NULL;
	int nPush=m_pModel->PushPartStack();
	for (pItem = hashRodMirItem.GetFirst(); pItem; pItem = hashRodMirItem.GetNext())
		MirTaAtom(pItem->m_pSrcPart, MIRMSG(pItem->m_ciMirFlag), FALSE);
	for (pItem = hashBoltMirItem.GetFirst(); pItem; pItem = hashBoltMirItem.GetNext())
		MirTaAtom(pItem->m_pSrcPart, MIRMSG(pItem->m_ciMirFlag), FALSE);
	for(pItem=hashPlateMirItem.GetFirst();pItem;pItem=hashPlateMirItem.GetNext())
		MirTaAtom(pItem->m_pSrcPart, MIRMSG(pItem->m_ciMirFlag), FALSE);
	for(pItem=hashFittingMirItem.GetFirst();pItem;pItem=hashFittingMirItem.GetNext())
		MirTaAtom(pItem->m_pSrcPart, MIRMSG(pItem->m_ciMirFlag), FALSE);
	//�������ƶԳƹ����Ĺ�����ϵ
	for (pItem = hashRodMirItem.GetFirst(); pItem; pItem = hashRodMirItem.GetNext())
		MirTaAtom(pItem->m_pSrcPart, MIRMSG(pItem->m_ciMirFlag), TRUE);
	for (pItem = hashBoltMirItem.GetFirst(); pItem; pItem = hashBoltMirItem.GetNext())
		MirTaAtom(pItem->m_pSrcPart, MIRMSG(pItem->m_ciMirFlag), TRUE);
	for (pItem = hashPlateMirItem.GetFirst(); pItem; pItem = hashPlateMirItem.GetNext())
	{
		MIRMSG msg(pItem->m_ciMirFlag);
		CLDSPlate* pSrcPlate = (CLDSPlate*)pItem->m_pSrcPart;
		CLDSPlate *pMirPlate = (CLDSPlate*)pSrcPlate->GetMirPart(msg);
		if(pMirPlate==NULL)
			continue;
		long hMirPartWeldParent=0;
		MirPartHandle(&Ta, &hMirPartWeldParent, pSrcPlate->m_hPartWeldParent, msg);
		pMirPlate->m_hPartWeldParent = hMirPartWeldParent;
		//���ƶԳƸְ�Ļ�����Ʋ����͹������߲���
		pMirPlate->designInfo.m_bWeldToDatumJg = pSrcPlate->designInfo.m_bWeldToDatumJg;
		pMirPlate->designInfo.iFaceType = pSrcPlate->designInfo.iFaceType;
		pMirPlate->designInfo.iProfileStyle0123 = pSrcPlate->designInfo.iProfileStyle0123;
		pMirPlate->designInfo.m_bEnableFlexibleDesign = pSrcPlate->designInfo.m_bEnableFlexibleDesign;
		pMirPlate->designInfo.m_bSelfCorrectBendline = pSrcPlate->designInfo.m_bSelfCorrectBendline;
		MirPartHandle(&Ta, &pMirPlate->designInfo.m_hBaseNode, pSrcPlate->designInfo.m_hBaseNode, msg);
		MirPartHandle(&Ta, &pMirPlate->designInfo.m_hBasePart, pSrcPlate->designInfo.m_hBasePart, msg);
		MirDatumPoint(pSrcPlate->BelongModel(), pSrcPlate->designInfo.origin, pMirPlate->designInfo.origin, msg);
		MirDatumVector(pSrcPlate->BelongModel(), pSrcPlate->designInfo.norm, pMirPlate->designInfo.norm, msg);
		//���ƶԳƸְ��ϵ���˨����
		for (CLsRef *pLsRef = pSrcPlate->GetFirstLsRef(); pLsRef; pLsRef = pSrcPlate->GetNextLsRef())
		{
			CLDSBolt *pMirBolt = (CLDSBolt*)pLsRef->GetLsPtr()->GetMirPart(msg);
			if(pMirBolt==NULL)
				continue;
			pMirPlate->AppendLsRef(pMirBolt->GetLsRef());
			pMirBolt->ReplaceReferenceHandle(CHandleReplacer(pSrcPlate->handle, pMirPlate->handle));
			pMirBolt->AddL0Thick(pMirPlate->handle, TRUE, TRUE);
		}
		//���ƶԳƸְ��ϵ����Ӹ˼������Ϣ
		CDesignLjPartPara* pLjPart = NULL, *pMirLjPart = NULL;
		for(pLjPart=pSrcPlate->GetFirstLjPara();pLjPart;pLjPart=pSrcPlate->GetNextLjPara())
		{
			pLjPart->pCurLinePart = (CLDSLinePart*)Ta.Parts.FromHandle(pLjPart->hPart, CLS_LINEPART);
			if (pLjPart->pCurLinePart == NULL)
				continue;
			long hMirLjPart = 0;
			MirPartHandle(pSrcPlate->BelongModel(), &hMirLjPart, pLjPart->hPart, msg);
			CLDSLinePart* pMirLinePart = (CLDSLinePart*)Ta.Parts.FromHandle(hMirLjPart);
			if (pMirLinePart == NULL)
				continue;
			CDesignLjPartPara *pMirLjPart = pMirPlate->designInfo.partList.SetValue(hMirLjPart, *pLjPart);
			pMirLjPart->pCurLinePart = pMirLinePart;	//
			pMirLjPart->m_bAdjustLjEndPos = pLjPart->m_bAdjustLjEndPos;
			pMirLjPart->m_nClassTypeId = pMirLjPart->pCurLinePart->GetClassTypeId();
			pMirLjPart->iFaceNo = pLjPart->iFaceNo;
			pMirLjPart->ber_space = pLjPart->ber_space;
			pMirLjPart->wing_space = pLjPart->wing_space;
			pMirLjPart->end_space = pLjPart->end_space;
			f3dPoint line_vec = pLjPart->pCurLinePart->End() - pLjPart->pCurLinePart->Start();
			f3dPoint mir_line_vec = pMirLjPart->pCurLinePart->End() - pMirLjPart->pCurLinePart->Start();
			if (pLjPart->start0_end1 == 0)
				line_vec *= -1.0;
			f3dPoint mir_vec_tm = GetMirVector(line_vec, msg);
			if (pLjPart->start0_end1 > 1)		//�м�����
				pMirLjPart->start0_end1 = pLjPart->start0_end1;
			else if (mir_vec_tm*mir_line_vec > 0)
				pMirLjPart->start0_end1 = 1;	//�ն�����
			else
				pMirLjPart->start0_end1 = 0;	//ʼ������
			if (pLjPart->start0_end1 == 0 && pMirLjPart->start0_end1 == 0)
				pMirLjPart->pCurLinePart->SetStartOdd(pLjPart->pCurLinePart->startOdd());
			else if (pLjPart->start0_end1 == 0 && pMirLjPart->start0_end1 == 1)
				pMirLjPart->pCurLinePart->SetEndOdd(pLjPart->pCurLinePart->startOdd());
			else if (pLjPart->start0_end1 == 1 && pMirLjPart->start0_end1 == 0)
				pMirLjPart->pCurLinePart->SetStartOdd(pLjPart->pCurLinePart->endOdd());
			else if (pLjPart->start0_end1 == 1 && pMirLjPart->start0_end1 == 1)
				pMirLjPart->pCurLinePart->SetEndOdd(pLjPart->pCurLinePart->endOdd());
			MirLinePartOddDesignInfo(pLjPart->pCurLinePart, pMirLjPart->pCurLinePart, msg,
				pLjPart->start0_end1 == 0, pMirLjPart->start0_end1 == 0);
			if (pMirLinePart->GetClassTypeId() == CLS_LINEANGLE)
			{
				CLDSLineAngle* pSrcJg = (CLDSLineAngle*)pLjPart->pCurLinePart;
				CLDSLineAngle* pMirJg = (CLDSLineAngle*)pMirLinePart;
				f3dPoint verify_norm = pMirPlate->ucs.axis_z , wing_norm;
				if (pMirPlate->designInfo.norm.norm_style == 1)	//�Ǹ�֫����
				{
					CLDSLineAngle* pDatumAngle = (CLDSLineAngle*)Ta.Parts.FromHandle(pMirPlate->designInfo.norm.hVicePart);
					if (pDatumAngle&&pMirPlate->designInfo.norm.norm_wing == 0)	//X֫����
						wing_norm = pDatumAngle->get_norm_x_wing();
					else if (pDatumAngle&&pMirPlate->designInfo.norm.norm_wing == 1)	//Y֫����
						wing_norm = pDatumAngle->get_norm_y_wing();
					if (wing_norm*verify_norm < 0)
						verify_norm *= -1.0;
				}
				double ddx = pMirJg->get_norm_x_wing()*verify_norm;
				double ddy = pMirJg->get_norm_y_wing()*verify_norm;
				pMirLjPart->angle.bEndLjJg = pLjPart->angle.bEndLjJg;
				pMirLjPart->angle.bTwoEdge = pLjPart->angle.bTwoEdge;
				if (fabs(ddx) > fabs(ddy))
					pMirLjPart->angle.cur_wing_x0_y1 = 0;
				else
					pMirLjPart->angle.cur_wing_x0_y1 = 1;
				//�����ԳƽǸ�ʼ�ն�λ��
				if ((pLjPart->start0_end1 != 0 && pLjPart->start0_end1 != 1)|| !pLjPart->m_bAdjustLjEndPos)
					continue;
				MirLineAnglePosDesignInfo(pSrcJg,pMirJg, msg,
					pLjPart->start0_end1 == 0, pMirLjPart->start0_end1 == 0,
					pMirLjPart->angle.cur_wing_x0_y1 == pLjPart->angle.cur_wing_x0_y1);
				CMultiOffsetPos *pPosDesign= (pMirLjPart->start0_end1 == 0)? &pMirJg->desStartPos: &pMirJg->desEndPos;
				if (pMirLjPart->angle.cur_wing_x0_y1 == 0)
				{
					pPosDesign->wing_x_offset.gStyle = 4;
					pPosDesign->wing_x_offset.offsetDist = (ddx > 0) ? 0 : -pMirPlate->GetThick();
				}
				else
				{
					pPosDesign->wing_y_offset.gStyle = 4;
					pPosDesign->wing_y_offset.offsetDist = (ddy > 0) ? 0 : -pMirPlate->GetThick();
				}
			}
		}
	}
	m_pModel->PopPartStack(nPush);
	return true;
}
void CStdComModel::UpdateTemlRodReverseInfo(CStdTemplParaRod* pParaRod, BYTE* pInsideWingX, BYTE* pInsideWingY)
{
	if (pParaRod == NULL || pParaRod->pRelaRod == NULL)
		return;
	GEPOINT ptS = pParaRod->pRelaRod->xPosStart;
	GEPOINT ptE = pParaRod->pRelaRod->xPosEnd;
	pParaRod->bReverseStartEnd = (pParaRod->vVerifyLenStdVec*(ptE - ptS) < 0);
	if (!pParaRod->pRelaRod->IsAngle())
		return;
	CStdTemplParaAngle* pParaJg = (CStdTemplParaAngle*)pParaRod;
	CLDSLineAngle* pLdsJg = (CLDSLineAngle*)pParaRod->pRelaRod;
	if (!pParaJg->blShortAngle)
	{
		GEPOINT vWormViewNorm(0, 0, 1);
		if (pParaJg->normal.ciNormalDirection == 'X')
			vWormViewNorm.Set(1, 0, 0);
		else if (pParaJg->normal.ciNormalDirection == 'x')
			vWormViewNorm.Set(-1, 0, 0);
		else if (pParaJg->normal.ciNormalDirection == 'Y')
			vWormViewNorm.Set(0, 1, 0);
		else if (pParaJg->normal.ciNormalDirection == 'y')
			vWormViewNorm.Set(0, -1, 0);
		else if (pParaJg->normal.ciNormalDirection == 'Z')
			vWormViewNorm.Set(0, 0, 1);
		else if (pParaJg->normal.ciNormalDirection == 'z')
			vWormViewNorm.Set(0, 0, -1);
		vWormViewNorm = acs.TransVFromCS(vWormViewNorm);
		double ddx = pLdsJg->vxWingNorm*vWormViewNorm;
		double ddy = pLdsJg->vyWingNorm*vWormViewNorm;
		BYTE ciInsideWingX = 'X', ciInsideWingY = 'Y';	//'X','Y'��ʾ��֫����������δ��״̬ wjh-2019.12.10
		if (fabs(ddx) > fabs(ddy))
		{
			if (pParaJg->ciAngleWorkWing == 'X' || pParaJg->ciAngleWorkWing == 'x')
				pParaJg->bReverseWingX_Y = false;
			else if (pParaJg->ciAngleWorkWing == 'Y' || pParaJg->ciAngleWorkWing == 'y')
				pParaJg->bReverseWingX_Y = true;
			ciInsideWingX = ddx > 0 ? 1 : 2;	//����
		}
		else if (fabs(ddx) < fabs(ddy))
		{
			if (pParaJg->ciAngleWorkWing == 'X' || pParaJg->ciAngleWorkWing == 'x')
				pParaJg->bReverseWingX_Y = true;
			else if (pParaJg->ciAngleWorkWing == 'Y' || pParaJg->ciAngleWorkWing == 'y')
				pParaJg->bReverseWingX_Y = false;
			ciInsideWingY = ddy > 0 ? 1 : 2;	//����
		}
		if (pParaJg->bReverseWingX_Y)
		{
			char ciInsideTmpWing = ciInsideWingX;
			ciInsideWingX = ciInsideWingY;
			ciInsideWingY = ciInsideTmpWing;
		}
		if (pInsideWingX)
			*pInsideWingX = ciInsideWingX;
		if (pInsideWingY)
			*pInsideWingY = ciInsideWingY;
	}
}
bool CStdComModel::BuildModel(CStdComTempl* pStdTempl,BYTE ciMirMode/*=0*/)
{
	CLogErrorLife life(&logerr);
	CStackVariant<BOOL> anglelocaparamobj(&g_sysPara.m_bAnglePreferSimplePosMode, FALSE);	//�������׿����½�����ʱ��ʱ�ݲ����ü򵥶�λģʽ
	m_pStdTempl=pStdTempl;
	BuildAcsOfAssmComponent();
	m_xExpression.SetAcs(acs);
	m_xExpression.InitExpressionVarList(pStdTempl);
	pStdTempl->EnableIntelliRecogRodNodes(false);
	//1.��Ӷ���Ľڵ�
	CLDSNode* pLdsNode=NULL;
	for(CStdTemplParaNode* pParaNode=pStdTempl->listParaNodes.GetFirst();pParaNode;pParaNode=pStdTempl->listParaNodes.GetNext())
	{
		if (pParaNode->m_bNecessaryNode)
		{	//ģ���п϶��Ѵ��ڵı�Ҫ�ڵ�
			if ((pLdsNode = GetLdsNodeFromId(pParaNode->Id)) == NULL)
			{
				logerr.Log("ȱ��%d#�Źؼ����ƥ��ӳ��ڵ���Ϣ��������ʧ�ܣ�",pParaNode->Id);
				return false;
			}
			pParaNode->xPosition=pLdsNode->xOriginalPos;
			pParaNode->m_bNeedUpdateRelaInfo=false;
			pParaNode->pRelaNode=pLdsNode;
			pStdTempl->SetMappingNode(pParaNode->Id,pLdsNode->handle);
			m_xMapNodeSet.append(pLdsNode);
			continue;	
		}
		else
		{	//�����ڵ�
			if(pParaNode->m_bOptionExterNode||pStdTempl->MappingLdsNodeH(pParaNode->Id)!=NULL)
				continue;	//��������ɽڵ㣬��ģ�����Ѵ��ڸýڵ��ƥ��ڵ�
			pLdsNode=AddOrUpdateParaModelNode(pParaNode);
			pStdTempl->SetMappingNode(pParaNode->Id,pLdsNode);
			m_xMapNodeSet.append(pLdsNode);
		}
	}
	//2.��Ӹ��ӵļӾ��̽Ǹ�
	for (CStdTemplParaRod* pParaRod=pStdTempl->listParaRods.GetFirst();pParaRod;pParaRod=pStdTempl->listParaRods.GetNext())
	{
		CStdTemplParaNode *pStartParaNode=NULL,*pEndParaNode=NULL;
		SmartRodPtr pRod=GetLdsRodFromId(pParaRod->Id);
		if (pRod.IsNULL() && !pParaRod->m_bNecessaryRod)
			continue;
		if (pParaRod->idStartNode!=0)
		{
			pParaNode=pStartParaNode=pStdTempl->listParaNodes.GetValue(pParaRod->idStartNode);
			if (pParaNode!=NULL)
				pParaRod->xStartPoint=pParaNode->xPosition;
			else if ((pParaNode=pStdTempl->listParaNodes.GetValue(pParaRod->idStartNode.uiOriginalId))!=NULL)
			{
				pParaRod->xStartPoint=pParaNode->xPosition;
				if (pParaRod->idStartNode.ciMirTransType=='X')
					pParaRod->xStartPoint.y*=-1.0;
			}
			if (pParaRod->idEndNode==0&&pRod.IsHasPtr())
			{	//��ʼ�˷��������߸˼�
				if (pRod->pStart==pParaNode->pRelaNode)
					pParaRod->xEndPoint=pRod->pEnd!=NULL?pRod->pEnd->xOriginalPos:pRod->xPosEnd;
				else if (pRod->pEnd==pParaNode->pRelaNode)
					pParaRod->xEndPoint=pRod->pStart!=NULL?pRod->pStart->xOriginalPos:pRod->xPosStart;
			}
		}
		if (pParaRod->idEndNode!=0)
		{
			pParaNode=pEndParaNode=pStdTempl->listParaNodes.GetValue(pParaRod->idEndNode);
			if (pParaNode!=NULL)
				pParaRod->xEndPoint=pParaNode->xPosition;
			else if ((pParaNode=pStdTempl->listParaNodes.GetValue(pParaRod->idEndNode.uiOriginalId))!=NULL)
			{
				pParaRod->xEndPoint=pParaNode->xPosition;
				if (pParaRod->idEndNode.ciMirTransType=='X')
					pParaRod->xEndPoint.y*=-1.0;
			}
			if (pParaRod->idStartNode==0&&pRod.IsHasPtr())
			{	//���ն˷��������߸˼�
				if (pRod->pStart==pParaNode->pRelaNode)
					pParaRod->xStartPoint=pRod->pEnd!=NULL?pRod->pEnd->xOriginalPos:pRod->xPosEnd;
				else if (pRod->pEnd==pParaNode->pRelaNode)
					pParaRod->xStartPoint=pRod->pStart!=NULL?pRod->pStart->xOriginalPos:pRod->xPosStart;
			}
		}
		if (pParaRod->idStartNode==0&&pParaRod->idEndNode==0&&pRod.IsHasPtr())
		{
			if (pParaRod->vLocalNearVecS2E.IsEqual(1,0,0))
				pParaRod->vVerifyLenStdVec=acs.axis_x;
			else
				pParaRod->vVerifyLenStdVec=acs.TransVFromCS(pParaRod->vLocalNearVecS2E);
			if ((pRod->xPosEnd-pRod->xPosStart)*acs.axis_x>0)
			{
				pParaRod->xStartPoint=pRod->pStart!=NULL?pRod->pStart->xOriginalPos:pRod->xPosStart;
				pParaRod->xEndPoint  =pRod->pEnd!=NULL?pRod->pEnd->xOriginalPos:pRod->xPosEnd;
			}
			else
			{
				pParaRod->xStartPoint=pRod->pEnd!=NULL?pRod->pEnd->xOriginalPos:pRod->xPosEnd;
				pParaRod->xEndPoint  =pRod->pStart!=NULL?pRod->pStart->xOriginalPos:pRod->xPosStart;
			}
		}
		else if (pRod.IsHasPtr()||(pParaRod->idStartNode!=0&&pParaRod->idEndNode!=0))
		{
			pParaRod->vVerifyLenStdVec=pParaRod->xEndPoint-pParaRod->xStartPoint;
			normalize(pParaRod->vVerifyLenStdVec);
		}
		if (pRod.IsHasPtr())
		{
			pParaRod->pRelaRod=pRod;
			pParaRod->m_bNeedUpdateRelaInfo=false;
			pStdTempl->hashMatchRodHid.SetValue(pParaRod->Id,pRod->handle);
			UpdateTemlRodReverseInfo(pParaRod);
		}
		else //�����Ǹֿ��������нǸֵĳ���λ���й�ϵ����Ӧ�ڸ������нǸַ�λ������� wjh-2019.9.10
		{
			pRod = AddOrUpdateParaModelRod(pParaRod);
			if(pRod.blValid)
			{
				pParaRod->bReverseStartEnd=false;
				pParaRod->bReverseWingX_Y=(m_xNearAxisZ*acs.axis_z<0)?true:false;
				pParaRod->m_bNeedUpdatePosition=true;
				pParaRod->m_bNeedUpdateNormal = true;
				pParaRod->pRelaRod=pRod;
				if(pStartParaNode)
					pRod->pStart=pStartParaNode->pRelaNode;
				if(pEndParaNode)
					pRod->pEnd=pEndParaNode->pRelaNode;
				m_xMapPartSet.append(pRod);
				pStdTempl->hashMatchRodHid.SetValue(pParaRod->Id,pRod->handle);
			}
		}
	}
	//���½ڵ�Ĺ�����Ϣ�����˼�
	for(CStdTemplParaNode* pParaNode=pStdTempl->listParaNodes.GetFirst();pParaNode;pParaNode=pStdTempl->listParaNodes.GetNext())
	{
		if(pParaNode->pRelaNode&&pParaNode->m_bNeedUpdateRelaInfo)
			AddOrUpdateParaModelNode(pParaNode);
		CLDSNode* pNode=GetLdsNodeFromId(pParaNode->Id);
		if(pNode==NULL||pNode->hFatherPart>0x20)
			continue;
		CLDSLinePart* pRod=GetLdsRodFromId(pParaNode->keyLabelFatherRod);
		if(pRod)
			pNode->hFatherPart=pRod->handle;
		if(pNode->hFatherPart<0x20)
		{	//�޸��˼���Ϣ
			CLDSLinePart *pViceFatherPole=NULL;
			for(CLDSPart* pPart=m_xMapPartSet.GetFirst();pPart;pPart=m_xMapPartSet.GetNext())
			{
				if(!pPart->IsLinePart())
					continue;
				pRod=(CLDSLinePart*)pPart;
				if(pRod->pStart==NULL||pRod->pEnd==NULL)
					continue;
				if(pNode->m_cPosCalType==0)
				{
					if(pRod->pStart==pNode)
						pNode->hFatherPart=pRod->handle;
					else if(pRod->pEnd==pNode)
						pNode->hFatherPart=pRod->handle;
					if(pNode->hFatherPart>0X20)
						break;
				}
				else
				{
					f3dLine line(pRod->pStart->Position(false),pRod->pEnd->Position(false));
					int ret=line.PtInLine(pNode->Position(false));
					if(ret==2)		//���ڸ˼�ֱ����
					{
						pNode->hFatherPart=pRod->handle;
						pNode->layer(2)=pRod->layer(2);
						break;
					}
					else if(ret==1&&pViceFatherPole==NULL)	//׼���˼�(�ڵ�Ϊ�˼��˽ڵ�)
						pViceFatherPole=pRod;
				}
			}
			if(pNode->hFatherPart<0x20 && pViceFatherPole)	//δ�ҵ����˼������Ե�һ��׼���˼�Ϊ���˼�
				pNode->hFatherPart=pViceFatherPole->handle;
		}
	}
	//�Զ�����������ְ�ĺ�Ȳ���
	for(CStdTemplParaPlate* pParaPlate=pStdTempl->listParaPlates.GetFirst();pParaPlate;pParaPlate=pStdTempl->listParaPlates.GetNext())
	{
		//if(pParaPlate->_thick>6)
		//	continue;	//ָ�����
		int thick=0;
		for(UINT i=0;i<pParaPlate->connrods.Count;i++)
		{
			CONNECT_ROD* pConnRod=pParaPlate->connrods.GetAt(i);
			if(pConnRod->sidRod==pParaPlate->design.idBaseRod)
				continue;
			CStdTemplParaRod* pParaRod=pStdTempl->listParaRods.GetValue(pConnRod->sidRod);
			if (pParaRod==NULL||pParaRod->pRelaRod==NULL)
			{
				if (pParaRod->m_bNecessaryRod)
					logerr.Log("error");
			}
			else if(pParaRod->pRelaRod->IsAngle()&&pParaRod->pRelaRod->Thick>thick)
			{
				thick=ftoi(pParaRod->pRelaRod->Thick);
				pParaPlate->cMaterial=pParaRod->pRelaRod->cMaterial;
			}
		}
		if(thick>0)
		{
			if(thick==5)
				thick+=1;
			else if(thick>6&&thick%2>0)	//���ȡż��
				thick+=3;
			else
				thick+=2;	//���Ӧ�����߲������ֵ��һ���ȼ�
			pParaPlate->_thick=thick;
		}
	}
	//����֮ǰδ���������Ĺ�����Ϣ
	for(CStdTemplParaRod* pParaRod=pStdTempl->listParaRods.GetFirst();pParaRod;pParaRod=pStdTempl->listParaRods.GetNext())
	{
		if(pParaRod->pRelaRod&&pParaRod->m_bNeedUpdateRelaInfo)
			AddOrUpdateParaModelRod(pParaRod);
		if(pParaRod->pRelaRod==NULL||!pParaRod->pRelaRod->IsAngle())
			continue;
		CStdTemplParaAngle* pParaJg=(CStdTemplParaAngle*)pParaRod;
		if(pParaRod->pRelaRod&&pParaRod->m_bNeedUpdateNormal)	//��Ҫ��ǰ���½Ǹ�֫����֮�����ж������� wjh-2019.9.10
			UpdateAngleWingDirection(pParaJg);
		//�ж�������
		BYTE ciInsideAngleWingX = 'X', ciInsideAngleWingY = 'Y';
		UpdateTemlRodReverseInfo(pParaJg, &ciInsideAngleWingX, &ciInsideAngleWingY);
		//���½Ǹ�ʼ�ն˶�λ����
		CLDSLineAngle* pLdsJg = (CLDSLineAngle*)pParaRod->pRelaRod;
		pLdsJg->ClearFlag();
		if(pParaRod->pRelaRod&&pParaRod->bUpdateStartPos)
		{	//ģ���п϶��Ѵ��ڵı�Ҫ�˼�,�����˼�֫��ƫ����
			CMultiOffsetPos* pCurrDesPos=!pParaRod->bReverseStartEnd?&pLdsJg->desStartPos:&pLdsJg->desEndPos;
			CLDSNode* pCurrTipNode=!pParaRod->bReverseStartEnd?pLdsJg->pStart:pLdsJg->pEnd;
			if(pParaJg->desStart.cTouchLineType<=3)
				pCurrDesPos->jgber_cal_style=0;
			if(pParaJg->desStart.cTouchLineType==3)
			{	//���Ƶ�
				pCurrDesPos->cFaceOffsetDatumLine=pParaJg->desStart.cFaceOffsetDatumLine;
				if(pParaJg->desStart.coordCtrlType==3&&pParaJg->desStart.ciSectionPlane==0)
					pCurrDesPos->spatialOperationStyle=18;
			}
			else if(pParaJg->desStart.cTouchLineType==0||pParaJg->desStart.cTouchLineType==1)
			{	//����
				TMPLOBJID xid=pParaJg->desStart.uidTouchLineRod;
				UINT uidTouchLineRod=xid.uiOriginalId;
				CStdTemplParaRod *pLappedParaRod = pStdTempl->listParaRods.GetValue(uidTouchLineRod);
				CLDSLinePart* pDatumRod=pLappedParaRod->pRelaRod;
				if (xid.ciMirTransType!=0&&pDatumRod!=NULL)
				{
					if(xid.ciMirTransType=='X')
						pDatumRod=pDatumRod->GetMirRod(MIRMSG(1));
					else if(xid.ciMirTransType=='Y')
						pDatumRod=pDatumRod->GetMirRod(MIRMSG(2));
					else if(xid.ciMirTransType=='Z')
						pDatumRod=pDatumRod->GetMirRod(MIRMSG(3));
				}

				BYTE cTouchLine = pParaJg->desStart.cTouchLine;
				if (pLappedParaRod != NULL && pLappedParaRod->bReverseWingX_Y&&cTouchLine < 2)
					cTouchLine = 1 - cTouchLine;
				if( pCurrTipNode!=NULL&&pLappedParaRod!=NULL&&pDatumRod!=NULL&&
					pCurrTipNode->hFatherPart!=pDatumRod->handle)
					pCurrDesPos->datum_jg_h=pDatumRod->handle;
				if(pLappedParaRod->bReverseStartEnd)
					pCurrDesPos->len_offset_dist=-pParaJg->desStart.siDatumLenOffset;
				else
					pCurrDesPos->len_offset_dist= pParaJg->desStart.siDatumLenOffset;
				if(cTouchLine ==0)
				{
					if(pParaJg->desStart.face_offset_norm.IsZero())		//ͶӰ
						pCurrDesPos->spatialOperationStyle=2;	//��X֫����ͶӰ
					else
					{
						pCurrDesPos->spatialOperationStyle=5;	//��X֫����ƽ��
						pCurrDesPos->face_offset_norm=pParaJg->desStart.face_offset_norm;
					}
				}
				else if(cTouchLine ==1)
				{
					if(pParaJg->desStart.face_offset_norm.IsZero())		//ͶӰ
						pCurrDesPos->spatialOperationStyle=3;	//��Y֫����ͶӰ
					else
					{
						pCurrDesPos->spatialOperationStyle=6;	//��Y֫����ƽ��
						pCurrDesPos->face_offset_norm=pParaJg->desStart.face_offset_norm;
					}
				}
				else if(cTouchLine ==2)
				{
					if(pParaJg->desStart.face_offset_norm.IsZero())		//ͶӰ
						pCurrDesPos->spatialOperationStyle=1;	//������ͶӰ
					else
					{
						pCurrDesPos->spatialOperationStyle=4;	//������ƽ��
						pCurrDesPos->face_offset_norm=pParaJg->desStart.face_offset_norm;
					}
				}
				if (pParaJg->desStart.cTouchLineType==1)
				{
					pCurrDesPos->ChangeToDatumType(3);	//��Ϊ������
					if (pParaJg->desStart.uidBendByRod>0)
					{
						CStdTemplParaRod* pBendByRod=pStdTempl->listParaRods.GetValue(pParaJg->desStart.uidBendByRod);
						if (pBendByRod&&pBendByRod->pRelaRod!=NULL)
							pCurrDesPos->huoqu_jg_h=pBendByRod->pRelaRod->handle;
					}
				}
			}
			//
			if(strlen(pParaJg->desStart.szXorCtrlWingOffset)>0||strlen(pParaJg->desStart.szYorOthrWingOffset)>0)
			{
				CMultiOffsetPos* pDesPos=pParaRod->bReverseStartEnd?&pLdsJg->desEndPos:&pLdsJg->desStartPos;
				WING_OFFSET_STYLE* pDesWingXOffset=pParaRod->bReverseWingX_Y?&pDesPos->wing_y_offset:&pDesPos->wing_x_offset;
				WING_OFFSET_STYLE* pDesWingYOffset=pParaRod->bReverseWingX_Y?&pDesPos->wing_x_offset:&pDesPos->wing_y_offset;
				if(strlen(pParaJg->desStart.szXorCtrlWingOffset)>0)
					pDesWingXOffset->offsetDist=GetNormOffset(pParaJg,ciInsideAngleWingX,pParaJg->desStart.szXorCtrlWingOffset,&pDesWingXOffset->gStyle);
				else
					pDesWingXOffset->gStyle=0;
				if(strlen(pParaJg->desStart.szYorOthrWingOffset)>0)
					pDesWingYOffset->offsetDist=GetNormOffset(pParaJg,ciInsideAngleWingY,pParaJg->desStart.szYorOthrWingOffset,&pDesWingYOffset->gStyle);
				else
					pDesWingYOffset->gStyle=0;
			}
		}
		if(pParaRod->pRelaRod&&pParaRod->bUpdateEndPos)
		{	//ģ���п϶��Ѵ��ڵı�Ҫ�˼�,�����˼�֫��ƫ����
			CMultiOffsetPos* pCurrDesPos=!pParaRod->bReverseStartEnd?&pLdsJg->desEndPos:&pLdsJg->desStartPos;
			CLDSNode* pCurrTipNode=!pParaRod->bReverseStartEnd?pLdsJg->pEnd:pLdsJg->pStart;
			if(pParaJg->desEnd.cTouchLineType<=3)
				pCurrDesPos->jgber_cal_style=0;
			if(pParaJg->desEnd.cTouchLineType==3)
			{	//���Ƶ�
				pLdsJg->desEndPos.jgber_cal_style=0;
				pLdsJg->desEndPos.cFaceOffsetDatumLine=pParaJg->desEnd.cFaceOffsetDatumLine;
				if(pParaJg->desEnd.coordCtrlType==3&&pParaJg->desEnd.ciSectionPlane==0)
					pLdsJg->desEndPos.spatialOperationStyle=18;
			}
			else if(pParaJg->desEnd.cTouchLineType==0||pParaJg->desEnd.cTouchLineType==1)
			{	//����
				TMPLOBJID xid=pParaJg->desEnd.uidTouchLineRod;
				UINT uidTouchLineRod=xid.uiOriginalId;
				CStdTemplParaRod *pLappedParaRod = pStdTempl->listParaRods.GetValue(uidTouchLineRod);
				CLDSLinePart* pDatumRod=pLappedParaRod->pRelaRod;
				if (xid.ciMirTransType!=0&&pDatumRod!=NULL)
				{
					if(xid.ciMirTransType=='X')
						pDatumRod=pDatumRod->GetMirRod(MIRMSG(1));
					else if(xid.ciMirTransType=='Y')
						pDatumRod=pDatumRod->GetMirRod(MIRMSG(2));
					else if(xid.ciMirTransType=='Z')
						pDatumRod=pDatumRod->GetMirRod(MIRMSG(3));
				}
				BYTE cTouchLine = pParaJg->desEnd.cTouchLine;
				if (pLappedParaRod != NULL && pLappedParaRod->bReverseWingX_Y&&cTouchLine < 2)
					cTouchLine = 1 - cTouchLine;
				if( pCurrTipNode!=NULL&&pLappedParaRod!=NULL&&pDatumRod!=NULL&&
					pCurrTipNode->hFatherPart!=pDatumRod->handle)
					pCurrDesPos->datum_jg_h=pDatumRod->handle;
				if(pLappedParaRod->bReverseStartEnd)
					pCurrDesPos->len_offset_dist=-pParaJg->desEnd.siDatumLenOffset;
				else
					pCurrDesPos->len_offset_dist= pParaJg->desEnd.siDatumLenOffset;
				if(cTouchLine ==0)
				{
					if(pParaJg->desEnd.face_offset_norm.IsZero())	//ͶӰ
						pLdsJg->desEndPos.spatialOperationStyle=2;	//��X֫����ͶӰ
					else
					{
						pLdsJg->desEndPos.spatialOperationStyle=5;	//��X֫����ƽ��
						pLdsJg->desEndPos.face_offset_norm=pParaJg->desEnd.face_offset_norm;
					}
				}
				else if(cTouchLine ==1)
				{
					if(pParaJg->desEnd.face_offset_norm.IsZero())	//ͶӰ
						pLdsJg->desEndPos.spatialOperationStyle=3;	//��Y֫����ͶӰ
					else
					{
						pLdsJg->desEndPos.spatialOperationStyle=6;	//��Y֫����ƽ��
						pLdsJg->desEndPos.face_offset_norm=pParaJg->desEnd.face_offset_norm;
					}
				}
				else if(cTouchLine ==2)
				{
					if(pParaJg->desEnd.face_offset_norm.IsZero())	//ͶӰ
						pLdsJg->desEndPos.spatialOperationStyle=1;	//������ͶӰ
					else
					{
						pLdsJg->desEndPos.spatialOperationStyle=4;	//������ƽ��
						pLdsJg->desEndPos.face_offset_norm=pParaJg->desEnd.face_offset_norm;
					}
				}
				if (pParaJg->desEnd.cTouchLineType==1)
				{
					pCurrDesPos->ChangeToDatumType(3);	//��Ϊ������
					if (pParaJg->desEnd.uidBendByRod>0)
					{
						CStdTemplParaRod* pBendByRod=pStdTempl->listParaRods.GetValue(pParaJg->desEnd.uidBendByRod);
						if (pBendByRod&&pBendByRod->pRelaRod!=NULL)
							pCurrDesPos->huoqu_jg_h=pBendByRod->pRelaRod->handle;
					}
				}
			}
			if(strlen(pParaJg->desEnd.szXorCtrlWingOffset)>0||strlen(pParaJg->desEnd.szYorOthrWingOffset)>0)
			{
				CMultiOffsetPos* pDesPos=pParaRod->bReverseStartEnd?&pLdsJg->desStartPos:&pLdsJg->desEndPos;
				WING_OFFSET_STYLE* pDesWingXOffset=pParaRod->bReverseWingX_Y?&pDesPos->wing_y_offset:&pDesPos->wing_x_offset;
				WING_OFFSET_STYLE* pDesWingYOffset=pParaRod->bReverseWingX_Y?&pDesPos->wing_x_offset:&pDesPos->wing_y_offset;

				if(strlen(pParaJg->desEnd.szXorCtrlWingOffset)>0)
					pDesWingXOffset->offsetDist=GetNormOffset(pParaJg,ciInsideAngleWingX,pParaJg->desEnd.szXorCtrlWingOffset,&pDesWingXOffset->gStyle);
				else
					pDesWingXOffset->gStyle=0;
				if(strlen(pParaJg->desEnd.szYorOthrWingOffset)>0)
					pDesWingYOffset->offsetDist=GetNormOffset(pParaJg,ciInsideAngleWingY,pParaJg->desEnd.szYorOthrWingOffset,&pDesWingYOffset->gStyle);
				else
					pDesWingYOffset->gStyle=0;
			}
		}
		pLdsJg->ClearFlag();
		//�����ظ�������Ҫ����ͨ���������½Ǹ����շ��� wjh-2019.9.10
		if(pParaRod->m_bNeedUpdateNormal)	//���½Ǹ�֫���� wjh-2019.3.6
			UpdateAngleWingDirection(pParaJg);
		else
		{
			pLdsJg->cal_x_wing_norm();
			pLdsJg->cal_y_wing_norm();
		}
		pLdsJg->IterCalPosition();
		if(pLdsJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			if (pParaJg->wiGroupAngleSpace > 0)	//ģ��ָ����ϼ�϶
				((CLDSGroupLineAngle*)pLdsJg)->jg_space = pParaJg->wiGroupAngleSpace;
			((CLDSGroupLineAngle*)pLdsJg)->UpdateSonJgWingNorm();
			((CLDSGroupLineAngle*)pLdsJg)->UpdateSonJgPos();
		}
		//����Ǹ�����ͷ��������˨λ��
		if(pParaJg->wiStartOddCalStyle<=2)
			pLdsJg->desStartOdd.m_iOddCalStyle=pParaJg->wiStartOddCalStyle;
		if(pParaJg->wiEndOddCalStyle<=2)
			pLdsJg->desEndOdd.m_iOddCalStyle=pParaJg->wiEndOddCalStyle;
		if (pLdsJg->GetClassTypeId() == CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle* pGroupJg = ((CLDSGroupLineAngle*)pLdsJg);
			pGroupJg->m_bUpdateSonAngleOddment = FALSE;
			CLDSLineAngle *pSonJg[4] = { NULL };
			for (int i = 0; i < 4; i++)
			{
				pSonJg[i] = (CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[i], CLS_LINEANGLE);
				if (pSonJg[i])
				{
					pSonJg[i]->desStartOdd.m_iOddCalStyle = pLdsJg->desStartOdd.m_iOddCalStyle;
					pSonJg[i]->desEndOdd.m_iOddCalStyle = pLdsJg->desEndOdd.m_iOddCalStyle;
				}
			}
		}
		pLdsJg->CalStartOddment();
		pLdsJg->CalEndOddment();
		//��ӽǸ���˨
		for(CLsRef* pLsRef=pLdsJg->GetFirstLsRef();pLsRef;pLsRef=pLdsJg->GetNextLsRef())
		{
			CLDSBolt* pBolt=pLsRef->GetLsPtr();
			m_xMapPartSet.append(pBolt);
		}
		if (UI::blEnableIntermediateUpdateUI)
		{
			if (pParaRod->m_bNeedUpdateNormal&&pLdsJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				for (int j=0;j<4;j++)
				{
					CLDSLineAngle* pSonAngle=pLdsJg->son_jg_h[j]==0 ? NULL :
						(CLDSLineAngle*)m_pModel->FromPartHandle(pLdsJg->son_jg_h[j],CLS_LINEANGLE);
					if (pSonAngle==NULL)
						continue;
					pSonAngle->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pSonAngle->GetSolidPartObject());
				}
			}
		}
	}
	//�����ظ�������Ҫ����ͨ���������½Ǹ����շ��� wjh-2019.9.10
	for (CStdTemplParaRod* pParaRod=pStdTempl->listParaRods.GetFirst();pParaRod;pParaRod=pStdTempl->listParaRods.GetNext())
	{
		if (pParaRod->pRelaRod==NULL||!pParaRod->pRelaRod->IsAngle())
			continue;
		CLDSLineAngle* pLdsJg=(CLDSLineAngle*)pParaRod->pRelaRod;
		if (pParaRod->m_bNeedUpdateNormal)
		{
			pLdsJg->cal_x_wing_norm();
			pLdsJg->cal_y_wing_norm();
		 }
		if (pParaRod->bUpdateStartPos || pParaRod->bUpdateEndPos)
			pLdsJg->CalPosition(true);
	}
	//���Ӱ�
	for(CStdTemplParaPlate* pParaPlate=pStdTempl->listParaPlates.GetFirst();pParaPlate;pParaPlate=pStdTempl->listParaPlates.GetNext())
	{
		GECS* pSpecCS=(pStdTempl->ciGroupType==IStdComTempl::STDTMPL_NODEPLATE)?&acs:NULL;
		CLDSGeneralPlate* pPlate=AppendParaModelPlate(pParaPlate,pSpecCS,ciMirMode);
		if(pPlate)
		{
			m_xMapPartSet.append(pPlate);
			pStdTempl->hashMatchPlates.SetValue(pParaPlate->Id,pPlate->handle);
		}
	}
	CStdComTempl::MATCH_CONNECT_POINT* pMatchConnPoint;
	for(pMatchConnPoint=pStdTempl->listStdConnPoints.EnumObjectFirst();pMatchConnPoint;pMatchConnPoint=pStdTempl->listStdConnPoints.EnumObjectNext())
	{
		CStdComTemplConnPoint* pStdConnPoint=(CStdComTemplConnPoint*)globalStdComTmplLibrary.LoadStdComTempl(pMatchConnPoint->idStdConnPoint);
		if(pStdConnPoint==NULL)
			continue;
		CLDSNode* pBaseNode=this->GetLdsNodeFromId(pMatchConnPoint->idNode);
		CLDSLinePart* pBaseRod=this->GetLdsRodFromId(pMatchConnPoint->idBaseRod);
		CLDSLinePart* pRayRod=this->GetLdsRodFromId(pMatchConnPoint->idRayRodOnAxisY);
		pStdConnPoint->SetMappingNode(1,pBaseNode->handle);
		pStdConnPoint->listParaRods.Add(1)->pRelaRod=pBaseRod;
		pStdConnPoint->listParaRods.Add(2)->pRelaRod=pRayRod;
		pStdConnPoint->hashMatchRodHid.Empty();
		pStdConnPoint->hashMatchRodHid.SetValue(1,pBaseRod->handle);
		pStdConnPoint->hashMatchRodHid.SetValue(2,pRayRod->handle);
		pStdConnPoint->SetBelongModel(&Ta);
		CStdComModel tmpmodeler(this->m_pModel);
		GECS tmpcs;
		tmpmodeler.BuildAcsOfAssmComponent(pStdConnPoint,&tmpcs);
		CConnectPointInfo connpoint;
		//if(!pStdConnPoint->IntelliRecogRayRods())
		if(!tmpmodeler.RetrieveConnPoint(&connpoint,pBaseNode,pBaseRod,tmpcs.axis_z,true))
			logerr.Log("error");
		else
		{
			CStdComTempl* pStdNodePlate=globalStdComTmplLibrary.LoadStdComTempl(pMatchConnPoint->idStdNodePlate);
			pStdNodePlate->SetBelongModel(this->m_pModel);
			UINT idStdConnPoint=pStdConnPoint->Id;
			UINT idStdNotePlate = pStdNodePlate->Id;
			CLdsPartListStack stack(m_pModel);
			MATCHCONNPOINT_INFO match;
			if (RetrieveBasicMatchInfo(&connpoint, &match))
			{
				CStackVariant<bool> stackvar(&m_bEnableMirCreate,false);
				int status = DesignStdConnPoint(pStdConnPoint, &connpoint, &match, &idStdNotePlate, m_bEnablePromptUI, pMatchConnPoint->cbMirCreateFlag);
				if (status == 1) 
				{
					stack.EarlyPopStack();
					for (CLDSPart* pNewPart = m_pModel->EnumPartFirst(); pNewPart; pNewPart = m_pModel->EnumPartNext())
						m_xMapPartSet.append(pNewPart);
				}
			}
		}
	}
	//������˨
	CHashSet<CLDSPlate*> hashRelaPlate;
	for(CStdTemplParaBolt* pParaBolt=pStdTempl->listParaBolts.GetFirst();pParaBolt;pParaBolt=pStdTempl->listParaBolts.GetNext())
	{
		CLDSBolt* pBolt = AppendParaModelBolt(pParaBolt);
		if(pBolt==NULL)
			continue;
		m_xMapPartSet.append(pBolt);
		pStdTempl->hashMatchBolts.SetValue(pParaBolt->Id, pBolt->handle);
		//��˨���뵽�ְ��������Ƹְ�
		if (pBolt->m_cFuncType == 0)
		{
			for (int j = 0; j < 5; j++)
			{
				if (pParaBolt->through[j].cItemType == 0)
					break;
				if (pParaBolt->through[j].cItemType == CStdTemplParaBolt::THROUGH_ITEM::PART_THROUGH)
				{
					WORD widPart = pParaBolt->through[j].val;
					CLDSPart* pPart = GetLdsPartFromId(widPart, pParaBolt->through[j].cPartType);
					if(pPart==NULL || pPart->GetClassTypeId()!=CLS_PLATE)
						continue;
					hashRelaPlate.SetValue(pPart->handle,(CLDSPlate*)pPart);
				}
			}
		}
		//��ӵ�/���߿�ʱͬʱ���ɽ��
		if (pStdTempl->IsWireStruTmpl()&& pBolt->m_cFuncType==2)
		{
			CLDSPart* pResidePart1 = NULL, *pResidePart2 = NULL;
			pResidePart1 = GetLdsPartFromId(pParaBolt->through[0].val);
			pResidePart2 = GetLdsPartFromId(pParaBolt->through[1].val);
			if (pResidePart1 && pResidePart1->GetClassTypeId() == CLS_GROUPLINEANGLE)
			{	//��ϽǸ��ϵĹ��߿ף���ȡ�ҿ����������ӽǸ�
				CLDSGroupLineAngle* pGroupAngle = (CLDSGroupLineAngle*)pResidePart1;
				BYTE ciMasterIndex = pGroupAngle->ciMasterIndex;
				pResidePart1 = m_pModel->FromPartHandle(pGroupAngle->son_jg_h[ciMasterIndex]);
				pResidePart2 = m_pModel->FromPartHandle(pGroupAngle->son_jg_h[1-ciMasterIndex]);
			}
			if (pResidePart1==NULL||pResidePart2==NULL)
			{
				logerr.Log("error");
				continue;
			}
			//���������ڵ����ҹҿ׵���������
			CLDSPart *pResidePartL = NULL, *pResidePartR = NULL;
			f3dPoint xBasePt1, xBasePt2, xBaseVec, xBoltPt = pBolt->ucs.origin;
			pResidePart1->GetBoltIntersPos(pBolt, xBasePt1, NULL, CLDSPart::DATUM_PLANE);
			pResidePart2->GetBoltIntersPos(pBolt, xBasePt2, NULL, CLDSPart::DATUM_PLANE);
			////////////////////////////////////////////////////////
			//�����жϹҽӽ�ߵĶ�������
			BYTE ciQuad = 0;
			if (xBoltPt.x >= 0 && xBoltPt.y >= 0)
				ciQuad = 1;
			else if (xBoltPt.x <= 0 && xBoltPt.y >= 0)
				ciQuad = 2;
			else if (xBoltPt.x >= 0 && xBoltPt.y <= 0)
				ciQuad = 3;
			else if (xBoltPt.x <= 0 && xBoltPt.y <= 0)
				ciQuad = 4;
			xBaseVec = (xBasePt2 - xBasePt1).normalized();
			//double ddX = fabs(xBaseVec*f3dPoint(1, 0, 0));
			//double ddY = fabs(xBaseVec*f3dPoint(0, 1, 0));
			if (fabs(xBaseVec.x)>fabs(xBaseVec.y))//ddX > ddY)
			{	//ˮƽ������
				if (ciQuad == 1 || ciQuad == 2)
				{	//ǰ��
					pResidePartL = (xBasePt1.x < xBasePt2.x) ? pResidePart1 : pResidePart2;
					pResidePartR = (xBasePt1.x < xBasePt2.x) ? pResidePart2 : pResidePart1;
				}
				else
				{	//���
					pResidePartL = (xBasePt1.x > xBasePt2.x) ? pResidePart1 : pResidePart2;
					pResidePartR = (xBasePt1.x > xBasePt2.x) ? pResidePart2 : pResidePart1;
				}
			}
			else
			{	//��ֱ������
				if (ciQuad == 1 || ciQuad == 3)
				{	//�Ҳ�
					pResidePartL = (xBasePt1.y > xBasePt2.y) ? pResidePart1 : pResidePart2;
					pResidePartR = (xBasePt1.y > xBasePt2.y) ? pResidePart2 : pResidePart1;
				}
				else
				{	//���
					pResidePartL = (xBasePt1.y < xBasePt2.y) ? pResidePart1 : pResidePart2;
					pResidePartR = (xBasePt1.y < xBasePt2.y) ? pResidePart2 : pResidePart1;
				}
			}
			////////////////////////////////////////////////////////
			CStdComTemplWireStructure* pWireTempl = (CStdComTemplWireStructure*)pStdTempl;
			int ii = pParaBolt->m_iWireGroup;
			if (pResidePartL && pResidePartR && pWireTempl->xarrWireGroups[ii].ciFittingType<2)
			{	//EB|GD���
				CLDSFitting *pFitting = (CLDSFitting*)console.AppendPart(CLS_FITTINGS);
				pFitting->SetLayer(pResidePartL->layer());
				pFitting->m_ciFittingType = pWireTempl->xarrWireGroups[ii].ciFittingType;
				pFitting->sizespec.Copy(pWireTempl->xarrWireGroups[ii].szFittingSizeCode);
				pFitting->sizespecUR.Copy(pWireTempl->xarrWireGroups[ii].szUringSizeCode);
				pFitting->m_iLiveSwingAngle = (short)pWireTempl->xarrWireGroups[ii].sfMasterSwingAngle;
				pFitting->m_iURLiveSwingAngle = (short)pWireTempl->xarrWireGroups[ii].sfSlaveUrSwingAngle;
				pFitting->holes[0].hBolt = pBolt->handle;
				pFitting->holes[0].hResidePart = pResidePartL->handle;
				pFitting->holes[1].hBolt = pBolt->handle;
				pFitting->holes[1].hResidePart = pResidePartR->handle;
				pFitting->DesignFitting();
				//��¼���
				pStdTempl->hashMatchFittings.SetValue(pParaBolt->Id, pFitting->handle);
			}
		}
	}
	//�ְ������˨���ú�������������
	for (CLDSPlate* pPlate = hashRelaPlate.GetFirst(); pPlate; pPlate = hashRelaPlate.GetNext())
		pPlate->DesignPlate();
	//���¼���������˨�����Ϲҿף����µĽǸ�����ͷ�䶯
	for(CStdTemplParaRod* pParaRod=pStdTempl->listParaRods.GetFirst();pParaRod;pParaRod=pStdTempl->listParaRods.GetNext())
	{
		if(pParaRod->pRelaRod==NULL||!pParaRod->pRelaRod->IsAngle())
			continue;
		CLDSLineAngle* pLdsAngle=(CLDSLineAngle*)pParaRod->pRelaRod;
		bool modified=false;
		if(pLdsAngle->desStartOdd.m_iOddCalStyle==1&&pLdsAngle->GetLsCount()>0)
		{
			double dfOldOdd=pLdsAngle->dfStartOdd;
			pLdsAngle->CalStartOddment();
			modified|=fabs(dfOldOdd-pLdsAngle->dfStartOdd)>0.5;
		}
		if(pLdsAngle->desEndOdd.m_iOddCalStyle==1&&pLdsAngle->GetLsCount()>0)
		{
			double dfOldOdd=pLdsAngle->dfEndOdd;
			pLdsAngle->CalEndOddment();
			modified|=fabs(dfOldOdd-pLdsAngle->dfEndOdd)>0.5;
		}
		if(modified)
		{
			if(pLdsAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)pLdsAngle;
				pGroupAngle->UpdateSonJgOddment();
				for(int j=0;j<4;j++)
				{
					if(pGroupAngle->son_jg_h[j]==0)
						break;
					CLDSPart* pSubAngle=this->m_pModel->FromPartHandle(pGroupAngle->son_jg_h[j]);
					pSubAngle->Create3dSolidModel();
				}
			}
			else
				pLdsAngle->Create3dSolidModel();
		}
	}
//	DWORD dwPrevTicks=timer.Start();
	//������Ҫ�ԳƵ�������������˨
	if(m_bEnableMirCreate)
		MirrorNewModelParts(pStdTempl);
//	timer.Relay(9,dwPrevTicks);
//	timer.End();
/*	for (DWORD* pdwTickCount = timer.hashTicks.GetFirst(); pdwTickCount; pdwTickCount = timer.hashTicks.GetNext())
	{
		UINT uiTickIdentity = timer.hashTicks.GetCursorKey();
		logerr.Log("timer#%2d=%.3f\n", uiTickIdentity, (*pdwTickCount)*0.001);
	}
	logerr.Log("Summary time cost =%.3f\n", (timer.GetEndTicks() - timer.GetStartTicks())*0.001);*/
	return true;
}
////////////////////////////////////////////////////////////////////////////////

static bool MapInfoToStdConnPoint(MATCHCONNPOINT_INFO* pMatchInfo, CStdComTemplConnPoint* pStdConnPoint)
{
	CLDSNode* pBaseNode = Ta.FromNodeHandle(pMatchInfo->hBaseNode);
	if ((pMatchInfo->pBaseRod = Ta.FromRodHandle(pMatchInfo->hBaseRod)) == NULL)
		return false;
	CLDSLinePart* pRayRod = Ta.FromRodHandle(pMatchInfo->hRayRodOnAxisY);
	if (pRayRod == NULL)
		return false;
	pStdConnPoint->SetBelongModel(&Ta);
	pStdConnPoint->ClearTmplParams();
	pStdConnPoint->m_ciMirrorMatchMode = pMatchInfo->ciMirrorMatchMode;
	pStdConnPoint->SetMappingNode(1, pMatchInfo->hBaseNode);
	pStdConnPoint->listParaRods.Add(pStdConnPoint->widBaseRod)->pRelaRod = pMatchInfo->pBaseRod;
	pStdConnPoint->listParaRods.Add(2)->pRelaRod = pRayRod;
	//pStdConnPoint->hashMatchRodHid.SetValue(pStdConnPoint->widBaseRod,pMatchInfo->pBaseRod->handle);
	//pStdConnPoint->hashMatchRodHid.SetValue(2,pRayRod->handle);
	for (UINT i = 0; i < pMatchInfo->xarrMatchRodHid.Count; i++)
	{
		pStdConnPoint->rodarr[i].hMapRod = pMatchInfo->xarrMatchRodHid[i];
		pStdConnPoint->hashMatchRodHid.SetValue(pStdConnPoint->rodarr[i].widRod, pMatchInfo->xarrMatchRodHid[i]);
	}
	return true;
}
static bool RetrieveBasicMatchInfo(CConnectPointInfo *pConnectPoint, MATCHCONNPOINT_INFO* pMatchInfo)
{
	pMatchInfo->connrods.Clear();
	pMatchInfo->connrods.ZeroPresetMemory();
	pMatchInfo->hBaseNode = pConnectPoint->m_hBaseNode;
	pMatchInfo->hBaseRod = pConnectPoint->m_hBasePart;
	pMatchInfo->pBaseRod = Ta.FromRodHandle(pConnectPoint->m_hBasePart);
	if (pMatchInfo->pBaseRod == NULL || pMatchInfo->pBaseRod->pStart == NULL || pMatchInfo->pBaseRod->pEnd == NULL)
		return false;
	SmartRodPtr pBaseRod = pMatchInfo->pBaseRod;
	pMatchInfo->lenBaseRodStdVecStoE = pBaseRod->pEnd->xOriginalPos - pBaseRod->pStart->xOriginalPos;
	normalize(pMatchInfo->lenBaseRodStdVecStoE);
	GECS cs;
	cs.axis_z = pConnectPoint->datumplane.vLine;
	normalize(cs.axis_z);
	if (pBaseRod->IsAngle())
	{
		double jx = pBaseRod.pAngle->vxWingNorm*pConnectPoint->datumplane.vSolid;
		double jy = pBaseRod.pAngle->vyWingNorm*pConnectPoint->datumplane.vSolid;
		if ((fabs(jx) > fabs(jy) && jx < 0) || (fabs(jx) < fabs(jy) && jy < 0))
			cs.axis_z *= -1;	//�ù����淨���������׼�Ǹֹ���֫���
	}
	double prjlen = pMatchInfo->lenBaseRodStdVecStoE*cs.axis_z;
	if (fabs(prjlen) > EPS)	//���˼�ʼ->�����췽������ƽ��ͶӰ
		pMatchInfo->lenBaseRodStdVecStoE -= prjlen * cs.axis_z;
	cs.axis_x = pMatchInfo->lenBaseRodStdVecStoE;
	normalize(cs.axis_x);
	cs.axis_y = cs.axis_z^cs.axis_x;
	//��ȡ�ڵ㴦�����Ӹ˼�
	LINEPARTSET rodset;
	if (pConnectPoint->linkPartSet.FromHandle(pBaseRod->handle) == NULL)
	{
		rodset.append(pBaseRod);
		pBaseRod->feature = 1;
	}
	bool blAxisYVerified = false;
	CLDSLinePart* pLinkRod = NULL;
	for (pLinkRod = pConnectPoint->linkPartSet.GetFirst(); pLinkRod; pLinkRod = pConnectPoint->linkPartSet.GetNext())
	{
		rodset.append(pLinkRod);
		pLinkRod->feature = 1;
		if (blAxisYVerified || pLinkRod == pBaseRod)
			continue;
		//����ʹY�������ܱ�֤������һ�����߸˼� wjh-2019.6.14
		GEPOINT lenRayVec;
		if (pLinkRod->pStart->handle == pConnectPoint->m_hBaseNode)
			lenRayVec = pLinkRod->pEnd->xOriginalPos - pLinkRod->pStart->xOriginalPos;
		else if (pLinkRod->pEnd->handle == pConnectPoint->m_hBaseNode)
			lenRayVec = pLinkRod->pStart->xOriginalPos - pLinkRod->pEnd->xOriginalPos;
		else
			continue;
		normalize(lenRayVec);
		double justify = cs.axis_y*lenRayVec;
		if (fabs(justify) < EPS2)
			continue;
		if (justify < -EPS2)
		{
			cs.axis_x *= -1.0;
			cs.axis_y *= -1.0;
		}
		blAxisYVerified = true;
	}
	for (pLinkRod = pConnectPoint->bendPlaneRodSet.GetFirst(); pLinkRod; pLinkRod = pConnectPoint->bendPlaneRodSet.GetNext())
	{
		rodset.append(pLinkRod);
		pLinkRod->feature = 2;
	}
	for (pLinkRod = pConnectPoint->bendPlaneRodSet2.GetFirst(); pLinkRod; pLinkRod = pConnectPoint->bendPlaneRodSet2.GetNext())
	{
		rodset.append(pLinkRod);
		pLinkRod->feature = 2;
	}
	for (pLinkRod = rodset.GetFirst(); pLinkRod; pLinkRod = rodset.GetNext())
	{
		MATCHROD_INFO rodinfo(pLinkRod);
		GEPOINT vRayStdVec = pLinkRod->pEnd->xOriginalPos - pLinkRod->pStart->xOriginalPos;
		//normalize(vRayStdVec);
		if (pLinkRod->pStart->handle == pConnectPoint->m_hBaseNode)
			rodinfo.ciConnTipS0E1Els2 = 0;
		else if (pLinkRod->pEnd->handle == pConnectPoint->m_hBaseNode)
		{
			vRayStdVec *= -1.0;
			rodinfo.ciConnTipS0E1Els2 = 1;
		}
		else//pRod->pStart!=pBaseNode&&pRod->pEnd!=pBaseNode
			rodinfo.ciConnTipS0E1Els2 = 2;	//�ᴩ�˼�
		vRayStdVec = cs.TransVToCS(vRayStdVec);
		rodinfo.siRayDegAngle = ftoi(DEG(vRayStdVec));
		rodinfo.ciPlaneNo = (BYTE)pLinkRod->feature;
		if (rodinfo.ciConnTipS0E1Els2 == 2 && rodinfo.siRayDegAngle > 180)
			rodinfo.siRayDegAngle -= 180;
		pMatchInfo->connrods.Append(rodinfo);
	}
	pMatchInfo->lcs = cs;
	return true;
}
bool CStdComModel::MatchWithStdConnPoint(CConnectPointInfo *pConnectPoint,
	CStdComTemplConnPoint* pStdConnPoint, MATCHCONNPOINT_INFO* pMatchInfo)
{
	char ciViewPlaneNormal = pStdConnPoint->xView.ciViewPlaneNormal;
	if (ciViewPlaneNormal == 'H')
	{	//��ʾ����Ϊˮƽ����'X','x','Y'��'y'���п��ܣ���������ӵ����ж�
		if (fabs(pConnectPoint->datumplane.vLine.x) > fabs(pConnectPoint->datumplane.vLine.y))
			ciViewPlaneNormal = pConnectPoint->datumplane.vLine.x > 0 ? 'X' : 'x';
		else
			ciViewPlaneNormal = pConnectPoint->datumplane.vLine.y > 0 ? 'Y' : 'y';
	}
	GECS cs;	//�Ӿ�ͶӰƽ������ϵ��Z��ָ���˵��۾�������
	if (ciViewPlaneNormal == 'X')
		cs = GECS(GEPOINT(0, 0, 0), GEPOINT(0, -1, 0), GEPOINT(0, 0, -1), GEPOINT(1, 0, 0));
	else if (ciViewPlaneNormal == 'x')
		cs = GECS(GEPOINT(0, 0, 0), GEPOINT(0, 1, 0), GEPOINT(0, 0, -1), GEPOINT(-1, 0, 0));
	else if (ciViewPlaneNormal == 'Y')
		cs = GECS(GEPOINT(0, 0, 0), GEPOINT(1, 0, 0), GEPOINT(0, 0, -1), GEPOINT(0, 1, 0));
	else if (ciViewPlaneNormal == 'y')
		cs = GECS(GEPOINT(0, 0, 0), GEPOINT(-1, 0, 0), GEPOINT(0, 0, -1), GEPOINT(0, -1, 0));
	else if (ciViewPlaneNormal == 'Z')
		cs = GECS(GEPOINT(0, 0, 0), GEPOINT(1, 0, 0), GEPOINT(0, 1, 0), GEPOINT(0, 0, 1));
	else if (ciViewPlaneNormal == 'y')
		cs = GECS(GEPOINT(0, 0, 0), GEPOINT(1, 0, 0), GEPOINT(0, -1, 0), GEPOINT(0, 0, -1));
	else
		return false;
	if (fabs(pConnectPoint->datumplane.vLine*cs.axis_z) < 0.7)
		return false;	//����������ľ��ֽ�������������н�����Ϊ0.57735,ƽ��45������=0.707
	if (pStdConnPoint->cnConnectPlane > 0)
	{
		if (pStdConnPoint->cnConnectPlane == 1 && pConnectPoint->bendPlaneRodSet.Count > 0)
			return false;
		if (pStdConnPoint->cnConnectPlane == 2 &&
			(pConnectPoint->bendPlaneRodSet.Count == 0 || pConnectPoint->bendPlaneRodSet2.Count > 0))
			return false;
		if (pStdConnPoint->cnConnectPlane == 3 && pConnectPoint->bendPlaneRodSet2.Count == 0)
			return false;
	}
	CLDSLinePart* pBaseRod = Ta.FromRodHandle(pConnectPoint->m_hBasePart);
	if (pMatchInfo == NULL || pBaseRod == NULL || pBaseRod->pStart == NULL || pBaseRod->pEnd == NULL)
	{
		logerr.Log("0x%X�ڵ㴦�����ӵ��׼�˼�0x%X��ʧ!", pConnectPoint->m_hBaseNode, pConnectPoint->m_hBasePart);
		return false;
	}
	cs.axis_z = pConnectPoint->datumplane.vLine;
	if (ciViewPlaneNormal == 'X' || ciViewPlaneNormal == 'x' ||
		ciViewPlaneNormal == 'Y' || ciViewPlaneNormal == 'y')
	{
		cs.axis_x = cs.axis_y^cs.axis_z;
		normalize(cs.axis_x);
		normalize(cs.axis_z);
	}
	else if (ciViewPlaneNormal == 'Z' || ciViewPlaneNormal == 'z')
	{
		cs.axis_y = cs.axis_z^cs.axis_x;
		normalize(cs.axis_y);
		normalize(cs.axis_z);
	}
	GEPOINT lenStdVec = pMatchInfo->lenBaseRodStdVecStoE;
	normalize(lenStdVec);
	if (pStdConnPoint->ciBaseRodConnState > 0 && pStdConnPoint->ciBaseRodConnState != pConnectPoint->m_ciBaseRodConnState)
		return false;
	short sign = 1, siBaseDegAngIncrement = 0;
	if (pStdConnPoint->xView.ciBaseAxisXMode > 0)
	{
		if (pStdConnPoint->xView.ciBaseAxisXMode == 1 && pBaseRod->pEnd->handle == pConnectPoint->m_hBaseNode)
			lenStdVec *= -1.0;	//���߷���Ϊ��->ʼ���򣬱�׼���ӵ�Ҫ�����߷���Ϊ��׼
		else if (pStdConnPoint->xView.ciBaseAxisXMode == 2 && pBaseRod->pStart->handle == pConnectPoint->m_hBaseNode)
			lenStdVec *= -1.0;	//���߷���Ϊʼ->�շ���,����׼���ӵ�Ҫ�����߷�����Ϊ��׼
		GEPOINT lenStdVec2 = pMatchInfo->lcs.TransVToCS(lenStdVec);
		siBaseDegAngIncrement = ftoi(DEG(lenStdVec2));	//������ӵ�ԭʼ����ϵX�ᵱǰ��׼�ߵ����߼н�����
		if (lenStdVec2.x < EPS_COS2)
			sign = -1;	//
	}
	//��Ϊ����ֻ��ҪlenStdVec.x,lenStdVec.y����ͶӰ����ʸ���ļнǣ���˴�ʱcs.axis_z�����������᲻һ�²�Ӱ���� wjh-2019.10.6
	lenStdVec = cs.TransVToCS(lenStdVec);
	if (pStdConnPoint->xView.ciBaseAxisXMode == 0 && lenStdVec.y < 0 &&
		pBaseRod->pStart->handle != pConnectPoint->m_hBaseNode&&
		pBaseRod->pEnd->handle != pConnectPoint->m_hBaseNode)
		lenStdVec *= -1.0;	//�м�ᴩ����,ȡС��180����һ��н�Ϊ��׼�н�
	short siDegAng = ftoi(DEG(lenStdVec));
	short siMirTurnDegAng = siDegAng <= 180 ? 180 - siDegAng : 360 - siDegAng;
	DEG xDegAngle(siDegAng), xMirAngle(siMirTurnDegAng);
	if (!xDegAngle.IsInSector(pStdConnPoint->xView.siMinBaseRodAngle, pStdConnPoint->xView.siMaxBaseRodAngle, true) &&
		!xMirAngle.IsInSector(pStdConnPoint->xView.siMinBaseRodAngle, pStdConnPoint->xView.siMaxBaseRodAngle, true))
		return false;	//��������Ҿ���Գƾ��޷�ƥ���׼�˼�
	bool bTooManyRods = false, bNeedNecessaryRod = false;
	UINT i, j, mirturn;
	bool meetconditions = true;
	MATCHROD_INFO* pMatchRod;
	SCHEMA_ROD* pSchemaRod;
	for (mirturn = 0; mirturn < 4; mirturn++)
	{	//mirturn�ֱ���������ӵ��ĶȶԳƷ�λ�Ĳ�ͬ��̬
		meetconditions = true;
		for (j = 0; j < pMatchInfo->connrods.Count; j++)
			pMatchInfo->connrods.At(j).matched = false;
		for (i = 0; meetconditions&&i < pStdConnPoint->rodarr.Count; i++)
		{
			pSchemaRod = pStdConnPoint->rodarr.GetAt(i);
			pSchemaRod->hMapRod = 0;
			for (j = 0; j < pMatchInfo->connrods.Count; j++)
			{
				pMatchRod = pMatchInfo->connrods.GetAt(j);
				if (pMatchRod->matched)
					continue;	//��ƥ���
				DEG xRayDegAngle(pMatchRod->siRayDegAngle*sign + siBaseDegAngIncrement);
				if (mirturn == 1)		//���ҶԳ�ƥ��
					xRayDegAngle = 180 + xRayDegAngle;// pMatchRod->siRayDegAngle;
				else if (mirturn == 2)	//ǰ����Գ�ƥ��
					xRayDegAngle = 360 - xRayDegAngle;//pMatchRod->siRayDegAngle;
				else if (mirturn == 3)	//Z��Գ�ƥ��
					xRayDegAngle = 180 - xRayDegAngle;//pMatchRod->siRayDegAngle;
				if (!xRayDegAngle.IsInSector(pSchemaRod->siMinRayAngle, pSchemaRod->siMaxRayAngle, pSchemaRod->dualSideEdge))
					continue;	//���߼нǲ�ƥ��
				if (pSchemaRod->ciRodType == 'R'&&pMatchRod->ciRodType == 'A')
					continue;	//���������Ͳ�һ��(SchemaRodΪ'A'��MatchRodΪ'R'ʱ���ǵ�������ͷ���������Ϊһ��)
				char szRodSectType[4] = { 0 };
				szRodSectType[0] = pMatchRod->pRod->size_cSubClassType;
				if (!pMatchRod->pRod->IsTube())
					memcpy(szRodSectType, "��", 2);
				else if (!pMatchRod->pRod->IsAngle())
					szRodSectType[0] = 0;
				CLDSLineAngle* pAngle = pMatchRod->pRod->IsAngle() ? (CLDSLineAngle*)pMatchRod->pRod : NULL;
				if (pSchemaRod->xFlag.bFilterSectType)
				{	//�˼����������ж�
					if ((pSchemaRod->szRodSectType[0] == 'L' || pSchemaRod->szRodSectType[0] == 'T' ||
						pSchemaRod->szRodSectType[0] == 'D' || pSchemaRod->szRodSectType[0] == 'X') &&
						pMatchRod->pRod->size_cSubClassType != pSchemaRod->szRodSectType[0])
						continue;
				}
				if (pSchemaRod->xFlag.bFilterConnBoltN)
				{
					if (pAngle->pStart->handle == pConnectPoint->m_hBaseNode&&pAngle->wnStartConnBoltN < pSchemaRod->cnMinBoltN)
						continue;
					else if (pAngle->pEnd->handle == pConnectPoint->m_hBaseNode&&pAngle->wnEndConnBoltN < pSchemaRod->cnMinBoltN)
						continue;
				}
				if (pSchemaRod->xFlag.bFilterInsideAngle&&pSchemaRod->ciAngleWorkWing > 0 && pAngle != NULL)
				{
					bool blInsideAngle = pSchemaRod->ciAngleWorkWing.ciLayoutStyle == 2 || pSchemaRod->ciAngleWorkWing.ciLayoutStyle == 3;
					double ddx = pAngle->vxWingNorm*pConnectPoint->datumplane.vSolid;
					double ddy = pAngle->vyWingNorm*pConnectPoint->datumplane.vSolid;
					double dd = fabs(ddx) > fabs(ddy) ? ddx : ddy;
					if (dd > 0 && !blInsideAngle)	//��ǰ�Ǹ��ж�Ϊ����
						continue;	//��������һ��
					else if (dd < 0 && blInsideAngle)
						continue;
				}
				GEPOINT lenStdVec=pMatchRod->pRod->pEnd->xOriginalPos-pMatchRod->pRod->pStart->xOriginalPos;
				if(pSchemaRod->xFlag.bFilterHoriPrimaryRod&&(fabs(lenStdVec.z)>1||!pMatchRod->pRod->IsPrimaryRod()))
					continue;
				pMatchRod->matched = true;
				pSchemaRod->hMapRod = pMatchRod->pRod->handle;
				break;
			}
			if (pSchemaRod->ciNecessity == 1 && pSchemaRod->hMapRod == 0)
				meetconditions = false;	//ȱ�ٱ�Ҫ�ĸ˼�
		}
		for (j = 0; meetconditions&&j < pMatchInfo->connrods.Count; j++)
			meetconditions = meetconditions && pMatchInfo->connrods.At(j).matched;
		if (meetconditions)
			break;
	}
	if (meetconditions)
	{
		pMatchInfo->ciMirrorMatchMode = mirturn;
		//pMatchInfo->hRayRodOnAxisY=pStdConnPoint->rodarr[1].hMapRod;
		for (i = 0; i < pStdConnPoint->rodarr.Count; i++)
		{
			pSchemaRod = pStdConnPoint->rodarr.GetAt(i);
			pMatchInfo->xarrMatchRodHid.Set(i, pSchemaRod->hMapRod);
		}
		MapInfoToStdConnPoint(pMatchInfo, pStdConnPoint);
	}
	return meetconditions;
}
bool CStdComModel::m_bEnablePromptUI = true;
bool CStdComModel::m_bEnableMirCreate = true;	//������ƹ���������Գ����ɹ���
#include "..\ConnPointTemplPlateDlg.h"
//-1��ʾ�û��ж������������Ӱ�;0.���δ�ڱ�׼�ڵ�����ҵ����ʵı�׼ģ��;1.����Ѹ���ϵͳ�����ƥ���׼ģ������˽ڵ�������� wjh-2019.5.26
int CStdComModel::DesignStdConnPoint(CStdComTemplConnPoint* pStdConnPoint, CConnectPointInfo *pConnectPoint, MATCHCONNPOINT_INFO* pMatchInfo,
	UINT* pidSpecNodePlateTmpl /*= NULL*/, bool bDisplayMultiTmplUI /*= false*/,
	BYTE cbMirCreateFlag /*= 0*/, bool blNeglectNodePlateTmplCheckState /*= true*/)
{
	if (pConnectPoint != NULL && pMatchInfo != NULL && !pMatchInfo->m_bConnPointMatched &&
		!MatchWithStdConnPoint(pConnectPoint, pStdConnPoint, pMatchInfo))
		return -2;	//��Ҫ���б�׼���ӵ�ģ��ƥ�䣬��ƥ��ʧ�ܷ���-2
	long idStdNodePlate = 0, liMatchCount = 0;
	CStdComTempl* pStdNodePlate = NULL;
	CStdComModel modeler(&Ta);
	pStdConnPoint->SetBelongModel(&Ta);
	CStdComTemplConnPoint::CMatchStdPlate* pMatchStdPlate, *pHeadPassCheckStdPlate = NULL;
	for (pMatchStdPlate = pStdConnPoint->hashMaybeTmplPlates.GetFirst(); pMatchStdPlate;
		pMatchStdPlate = pStdConnPoint->hashMaybeTmplPlates.GetNext())
	{
		if (pidSpecNodePlateTmpl != NULL && pMatchStdPlate->idStdNodePlate != *pidSpecNodePlateTmpl)
			continue;
		if ((pStdNodePlate = globalStdComTmplLibrary.LoadStdComTempl(pMatchStdPlate->idStdNodePlate)) == NULL)
			pMatchStdPlate->blTmpPassedVerify = false;
		if (pMatchStdPlate->blTmpPassedVerify = true)//modeler.VerifyMatchRods(pStdNodePlate, NULL))
		{
			if (pHeadPassCheckStdPlate == NULL)
				pHeadPassCheckStdPlate = pMatchStdPlate;
			liMatchCount++;
		}
	}
	if (liMatchCount > 1 && bDisplayMultiTmplUI)
	{	//ͨ�������ڶ���ڵ�ģ����ѡ��ǡ���ı�׼�ڵ�ģ��
		CConnPointTemplPlateDlg selStdNodePlateDlg;
		selStdNodePlateDlg.SetCurrConnPoint(pStdConnPoint);
		if (selStdNodePlateDlg.DoModal() != IDOK)
			return -1;
		if (selStdNodePlateDlg.m_idTemplPlate == 0)
			return false;	//��ͨ�ù������
		pMatchStdPlate = pStdConnPoint->hashMaybeTmplPlates.GetValue(selStdNodePlateDlg.m_idTemplPlate);
	}
	else
		pMatchStdPlate = pHeadPassCheckStdPlate;
	bool blSuccessed = false;
	do {
		if (pMatchStdPlate == NULL || (
			pStdNodePlate = globalStdComTmplLibrary.LoadStdComTempl(pMatchStdPlate->idStdNodePlate)) == NULL)
			return false;
		if (!blNeglectNodePlateTmplCheckState && !pStdNodePlate->blEnableIntelliConnPointDesign)
			return false;
		modeler.VerifyMatchRods(pStdNodePlate, NULL);
		pStdNodePlate->SetBelongModel(&Ta);
		if (!pStdConnPoint->UpdateMatchStdPlateTmpl(pStdNodePlate))
			return false;
		if(pMatchInfo==NULL)
		{
			logerr.Log("error pMatchInfo=null");
			return false;
		}
		if (blSuccessed = modeler.BuildModel(pStdNodePlate, pMatchInfo->ciMirrorMatchMode))
			break;
		if (bDisplayMultiTmplUI)
			return blSuccessed;	//return false;
		else
			pMatchStdPlate = pStdConnPoint->hashMaybeTmplPlates.GetNext();
	} while (!blSuccessed&&pMatchStdPlate != NULL);
	if (pStdNodePlate->m_cbDisableConnPlaneFlag > 0 && pConnectPoint&&pConnectPoint->source.pSrcNodeInfo != NULL)
		pConnectPoint->source.pSrcNodeInfo->cbDisableFlag |= pStdNodePlate->m_cbDisableConnPlaneFlag;
	if (pMatchStdPlate->xarrReflectMapRods.Count > 0)
	{
		for (UINT i = 0; i < pMatchStdPlate->xarrReflectMapRods.Count; i++)
		{
			CStdComTemplConnPoint::MAPPING_ROD* pMapRod = pMatchStdPlate->xarrReflectMapRods.GetAt(i);
			CStdTemplParaRod* pParaRod = pStdNodePlate->listParaRods.GetValue(pMapRod->idNodePlateRod);
			pStdConnPoint->hashMatchRodHid.SetValue(pMapRod->idConnPointRod, pParaRod->pRelaRod->handle);
		}
	}
	if (pStdConnPoint->xSideConnPoint.idSideStdConnPoint > 0)
	{
		CConnectPointInfo xSideConnPointInfo;
		CStdComTemplConnPoint* pCoupleStdConnPoint = (CStdComTemplConnPoint*)globalStdComTmplLibrary.LoadStdComTempl(pStdConnPoint->xSideConnPoint.idSideStdConnPoint);
		if (pConnectPoint->source.pSrcNodeInfo != NULL && pConnectPoint->source.ciConnPlaneIndex <= 1)
		{
			short siSidePlaneIndex = 1 - pConnectPoint->source.ciConnPlaneIndex;
			bool blSuccess = pConnectPoint->source.pSrcNodeInfo->GetConnectPointInfo(siSidePlaneIndex, &xSideConnPointInfo);
			MATCHCONNPOINT_INFO match;
			if (blSuccess&&RetrieveBasicMatchInfo(&xSideConnPointInfo, &match))
				blSuccess = MatchWithStdConnPoint(&xSideConnPointInfo, pCoupleStdConnPoint, &match);
			if (pStdConnPoint->xSideConnPoint.xarrShareRods.Count > 0)
			{
				for (UINT i = 0; i < pStdConnPoint->xSideConnPoint.xarrShareRods.Count; i++)
				{
					CStdComTemplConnPoint::MAPPING_ROD* pMapRod = pStdConnPoint->xSideConnPoint.xarrShareRods.GetAt(i);
					long* phMapRod = pStdConnPoint->hashMatchRodHid.GetValue(pMapRod->idConnPointRod);
					pCoupleStdConnPoint->hashMatchRodHid.SetValue(pMapRod->idCoupleConnPointRod, *phMapRod);
					//CStdTemplParaRod* pParaRod=pCoupleStdConnPoint->listParaRods.GetValue(pMapRod->idCoupleConnPointRod);
					//pParaRod->pRelaRod=Ta.FromRodHandle(*phMapRod);
				}
			}
			UINT* pidSpecNodePlateTmpl = pStdConnPoint->xSideConnPoint.idSideStdNodePlate > 0 ? &pStdConnPoint->xSideConnPoint.idSideStdNodePlate : NULL;
			match.m_bConnPointMatched=true;
			int status = DesignStdConnPoint(pCoupleStdConnPoint, pConnectPoint, &match, pidSpecNodePlateTmpl,
				bDisplayMultiTmplUI, cbMirCreateFlag, blNeglectNodePlateTmplCheckState);
			//if(status>0)
			pConnectPoint->source.pSrcNodeInfo->cbDisableFlag |= 0x02;
		}
	}
	if (cbMirCreateFlag&CStdComTempl::MIR_CREATE_X)
	{
		pStdConnPoint->UpdateMatchStdPlateTmpl(pStdNodePlate, CStdComTempl::MIR_CREATE_X);
		modeler.BuildModel(pStdNodePlate, CStdComTempl::MIR_CREATE_X);
	}
	if (cbMirCreateFlag&CStdComTempl::MIR_CREATE_Y)
	{
		pStdConnPoint->UpdateMatchStdPlateTmpl(pStdNodePlate, CStdComTempl::MIR_CREATE_Y);
		modeler.BuildModel(pStdNodePlate, CStdComTempl::MIR_CREATE_Y);
	}
	if (cbMirCreateFlag&CStdComTempl::MIR_CREATE_Z)
	{
		pStdConnPoint->UpdateMatchStdPlateTmpl(pStdNodePlate, CStdComTempl::MIR_CREATE_Z);
		modeler.BuildModel(pStdNodePlate, CStdComTempl::MIR_CREATE_Z);
	}
	//SmartPartPtr pNewPart=modeler.m_xMapPartSet.GetTail();
	//if(ppRsltPlate!=NULL&&pNewPart.blValid&&pNewPart->IsPlate())
	//	*ppRsltPlate=pNewPart.pBasicPlate;
	return true;
}
//-1��ʾ�û��ж������������Ӱ�;0.���δ�ڱ�׼�ڵ�����ҵ����ʵı�׼ģ��;1.����Ѹ���ϵͳ�����ƥ���׼ģ������˽ڵ�������� wjh-2019.5.26
int CStdComModel::IntelliDesignStdComNodePlate(CConnectPointInfo *pConnectPoint,UINT* parrSpecifyStdTmpl/*=NULL*/,UINT count/*=1*/,BYTE cbMirCreateFlag/*=0*/,bool blNeglectNodePlateTmplCheckState/*=true*/)
{
	CStdComTemplConnPoint* pStdConnPoint;
	MATCHCONNPOINT_INFO match;
	if(RetrieveBasicMatchInfo(pConnectPoint,&match))
	{
		CStdComTempl* pStdComTmpl;
		if(count>0&&parrSpecifyStdTmpl!=NULL&&*parrSpecifyStdTmpl>0)
		{
			for(UINT i=0;i<count;i++)
			{
				if((pStdComTmpl=globalStdComTmplLibrary.LoadStdComTempl(parrSpecifyStdTmpl[i]))!=NULL)
				{
					if(pStdComTmpl->ciGroupType!=IStdComTempl::STDTMPL_CONNPOINT)
						continue;
					pStdConnPoint=(CStdComTemplConnPoint*)pStdComTmpl;
					int status=DesignStdConnPoint(pStdConnPoint,pConnectPoint,&match,NULL,m_bEnablePromptUI,cbMirCreateFlag,blNeglectNodePlateTmplCheckState);
					if (status==-2)
						continue;	//��ǰ��׼�ڵ㲻ƥ��
					return status;
				}
			}
		}
		else
		{
			for(pStdComTmpl=globalStdComTmplLibrary.EnumFirstStdNodeTempl(true,false);pStdComTmpl;
				pStdComTmpl=globalStdComTmplLibrary.EnumNextStdNodeTempl(true,false))
			{
				pStdConnPoint=(CStdComTemplConnPoint*)pStdComTmpl;
				if (!pStdConnPoint->blEnableIntelliConnPointDesign)
					continue;	//�����⹫������ wjh-2019.10.4
				int status=DesignStdConnPoint(pStdConnPoint,pConnectPoint,&match,NULL,m_bEnablePromptUI,cbMirCreateFlag,blNeglectNodePlateTmplCheckState);
				if (status==-2)
					continue;	//��ǰ��׼�ڵ㲻ƥ��
				return status;
			}
		}
	}
	return false;
}
