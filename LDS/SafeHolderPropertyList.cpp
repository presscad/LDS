#include "stdafx.h"
#include "Tower.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "CallBackDialog.h"
#include "SafeHolderPropertyList.h"
#include "PropertyListOper.h"

CTubeSafeHolderPropertyList::CTubeSafeHolderPropertyList(void)
{
	m_pBaseTube=NULL;
	param.m_hBaseTube=0;
	param.m_hBaseNode=0;
	param.m_fLen=60;
	param.m_cMat='H';
	param.m_sSpec="5";
	param.m_nNum=4;
	param.m_fSectAngle=90;
	param.m_xSectNorm.Set(0,0,1);
	param.m_fLenOff=0;
	param.m_ciSectNormalStyle = 0;
	param.m_ciRotFromEdgeStyle = 2;
	param.m_fRayAngle=0;
	param.m_xRayVec.Set(1,0,0);
}
CTubeSafeHolderPropertyList::~CTubeSafeHolderPropertyList(void)
{

}
bool CTubeSafeHolderPropertyList::VerifyRelaRodAndNodes()
{
	CLDSLineTube* pBaseTube=(CLDSLineTube*)m_pBelongModel->FromRodHandle(param.m_hBaseTube);
	if(pBaseTube==NULL)
		return false;
	CLDSNode* pBaseNode=m_pBelongModel->FromNodeHandle(param.m_hBaseNode);
	if(pBaseNode==NULL)
		return false;
	return true;
}
//���Բ���
const DWORD HASHTABLESIZE = 500;
long CTubeSafeHolderPropertyList::InitPropHashtable(long initId/*=1*/,long initHashSize/*=50*/,long initStatusHashSize/*=50*/)
{
	int id = 1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(initStatusHashSize);
	//ģ����Ϣ
	AddPropItem("BasicInfo",PROPLIST_ITEM(id++,"�۸ֵ�������"));
	AddPropItem("para.PartNo",PROPLIST_ITEM(id++,"����","�۸ֱ��"));
	AddPropItem("para.Material",PROPLIST_ITEM(id++,"����","�۸ֲ���"));
	AddPropItem("para.Spec",PROPLIST_ITEM(id++,"���  [","�۸ֹ��"));
	AddPropItem("para.Len",PROPLIST_ITEM(id++,"����","�۸ֳ���"));
	//װ����Ϣ
	AddPropItem("AssemblyInfo",PROPLIST_ITEM(id++,"װ����Ϣ"));
	AddPropItem("VectInfo",PROPLIST_ITEM(id++,"������Ϣ"));
	AddPropItem("para.BaseTube",PROPLIST_ITEM(id++,"��׼�ֹ�"));
	AddPropItem("para.BaseNode",PROPLIST_ITEM(id++,"��λ�ڵ�"));
	AddPropItem("para.ColumnR",PROPLIST_ITEM(id++,"����뾶"));
	AddPropItem("para.LenOff",PROPLIST_ITEM(id++,"������ƫ�ƾ�","��λ����Բ���Զ�λ�ڵ��ظֹ������ƫ�ƾ�"));
	AddPropItem("para.SectOrg",PROPLIST_ITEM(id++,"����Բ��"));
	AddPropItem("para.ciSectNormStyle",PROPLIST_ITEM(id++,"���淽��","","0.ˮƽ��|1.�ֹܴ�ֱ����"));
	AddPropItem("para.ciRotFromEdgeStyle",PROPLIST_ITEM(id++,"�������ʼ��׼��","���õ�һ������ʱ�����õĶ�λ��ʼ��","0.ֱ��ָ��|1.��׼��Ϊ��ʼ|2.������Ϊ��ʼ"));
	AddPropItem("para.RayVec",PROPLIST_ITEM(id++,"��ʼ����","�۸ֲ��õ���ʼ����"));
	AddPropItem("para.RayVec.x",PROPLIST_ITEM(id++,"X","�۸ֲ��õ���ʼ����X"));
	AddPropItem("para.RayVec.y",PROPLIST_ITEM(id++,"Y","�۸ֲ��õ���ʼ����Y"));
	AddPropItem("para.RayVec.z",PROPLIST_ITEM(id++,"Z","�۸ֲ��õ���ʼ����Z"));
	AddPropItem("para.RayRayAngle",PROPLIST_ITEM(id++,"��ʼ���ýǶȣ���"));
	AddPropItem("para.Num",PROPLIST_ITEM(id++,"��������","�Ƹֹ�һ�ܻ����õ���������"));
	AddPropItem("para.SectAngle",PROPLIST_ITEM(id++,"��������н�","����ײ���ת����ʱ������ת����Ƕ�,��λΪ��"));
	return id;
}
long CTubeSafeHolderPropertyList::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if(GetPropID("para.PartNo")==id)
		sText.Copy(param.m_sPartNo);
	else if(GetPropID("para.Material")==id)
		QuerySteelMatMark(param.m_cMat,sText);
	else if(GetPropID("para.Spec")==id)
		sText.Copy(param.m_sSpec);
	else if(GetPropID("para.Len")==id)
	{
		sText.Printf("%f",param.m_fLen);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.BaseTube")==id)
		sText.Printf("0X%X",param.m_hBaseTube);
	else if(GetPropID("para.BaseNode")==id)
		sText.Printf("0X%X",param.m_hBaseNode);
	else if(GetPropID("para.ColumnR")==id)
	{
		CLDSLineTube* pBaseTube=(CLDSLineTube*)m_pBelongModel->FromPartHandle(param.m_hBaseTube,CLS_LINETUBE);
		if(pBaseTube)
			sText.Printf("%f",pBaseTube->GetDiameter()*0.5);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.LenOff")==id)
	{
		sText.Printf("%f",param.m_fLenOff);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.SectOrg")==id)
	{
		GEPOINT xOrg;
		CLDSNode* pBaseNode=(CLDSNode*)m_pBelongModel->FromNodeHandle(param.m_hBaseNode);
		if(pBaseNode)
			xOrg=pBaseNode->xOriginalPos;
		sText.Printf("(%.2f,%.2f,%.2f)",xOrg.x,xOrg.y,xOrg.z);
	}
	else if(GetPropID("para.SectNorm")==id)
		sText.Printf("(%.2f,%.2f,%.2f)",param.m_xSectNorm.x,param.m_xSectNorm.y,param.m_xSectNorm.z);
	else if(GetPropID("para.RayVec")==id)
		sText.Printf("(%.2f,%.2f,%.2f)",param.m_xRayVec.x,param.m_xRayVec.y,param.m_xRayVec.z);
	else if(GetPropID("para.RayVec.x")==id)
	{
		sText.Printf("%f",param.m_xRayVec.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.RayVec.y")==id)
	{
		sText.Printf("%f",param.m_xRayVec.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.RayVec.z")==id)
	{
		sText.Printf("%f",param.m_xRayVec.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.ciSectNormStyle")==id)
	{
		if(param.m_ciSectNormalStyle ==0)
			sText.Copy("0.ˮƽ��");
		else if(param.m_ciSectNormalStyle ==1)
			sText.Copy("1.�ֹܴ�ֱ����");
	}
	else if(GetPropID("para.ciRotFromEdgeStyle")==id)
	{
		if(param.m_ciRotFromEdgeStyle==0)
			sText.Copy("0.ֱ��ָ��");
		else if(param.m_ciRotFromEdgeStyle==1)
			sText.Copy("1.��׼��Ϊ��ʼ");
		else if(param.m_ciRotFromEdgeStyle==2)
			sText.Copy("2.������Ϊ��ʼ");
	}
	else if(GetPropID("para.RayRayAngle")==id)
	{
		sText.Printf("%f",param.m_fRayAngle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.Num")==id)
		sText.Printf("%d",param.m_nNum);
	else if(GetPropID("para.SectAngle")==id)
	{
		sText.Printf("%f",param.m_fSectAngle);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
bool CTubeSafeHolderPropertyList::ButtonClick(CPropTreeItem* pItem,BOOL *lpRslt)
{
	CCallBackDialog* pCallBackDlg=(CCallBackDialog*)GetParent();
	if(pItem->m_idProp==GetPropID("para.BaseTube"))
		pCallBackDlg->SelectObject(CLS_LINETUBE,pItem->m_idProp);
	else if(pItem->m_idProp==GetPropID("para.BaseNode"))
		pCallBackDlg->SelectObject(CLS_NODE,pItem->m_idProp);
	return TRUE;
}
bool CTubeSafeHolderPropertyList::ModifyValueFunc(CPropTreeItem* pItem,CString& valueStr,BOOL *lpRslt)
{
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	CNativePropListManager oper(this);
	if(pItem->m_idProp==GetPropID("para.PartNo"))
		param.m_sPartNo.Copy(tem_str);
	else if(pItem->m_idProp==GetPropID("para.Material"))
		param.m_cMat=QuerySteelBriefMatMark(tem_str);
	else if(pItem->m_idProp==GetPropID("para.Spec"))
		param.m_sSpec.Copy(tem_str);
	else if(pItem->m_idProp==GetPropID("para.Len"))
		param.m_fLen=atof(tem_str);
	else if(pItem->m_idProp==GetPropID("para.BaseTube"))
		param.m_hBaseTube=atol(tem_str);
	else if(pItem->m_idProp==GetPropID("para.BaseNode"))
		param.m_hBaseNode=atol(tem_str);
	else if(pItem->m_idProp==GetPropID("para.LenOff"))
		param.m_fLenOff=atof(tem_str);
	else if (pItem->m_idProp == GetPropID("para.ciSectNormStyle"))
	{
		param.m_ciSectNormalStyle= tem_str[0] - '0';
	}
	else if(pItem->m_idProp==GetPropID("para.ciRotFromEdgeStyle"))
	{
		param.m_ciRotFromEdgeStyle=tem_str[0]-'0';
		if(param.m_ciRotFromEdgeStyle>0)
		{
			m_pBaseTube->BuildUCS();
			param.m_xRayVec=m_pBaseTube->ucs.axis_x;
		}
		//
		DeleteAllSonItems(pItem);
		if(param.m_ciRotFromEdgeStyle==0)
		{
			oper.InsertEditPropItem(pItem,"para.RayVec.x","","",-1,TRUE);
			oper.InsertEditPropItem(pItem,"para.RayVec.y","","",-1,TRUE);
			oper.InsertEditPropItem(pItem,"para.RayVec.z","","",-1,TRUE);
		}
	}
	else if(pItem->m_idProp==GetPropID("para.RayRayAngle"))
		param.m_fRayAngle=atof(tem_str);
	else if(pItem->m_idProp==GetPropID("para.Num"))
	{
		param.m_nNum=atol(tem_str);
		if(param.m_nNum>1)
			param.m_fSectAngle=360.0/param.m_nNum;
		//
		DeleteAllSonItems(pItem);
		if(param.m_nNum>1)
			oper.InsertEditPropItem(pItem,"para.SectAngle","","",-1,TRUE);
	}
	else if(pItem->m_idProp==GetPropID("para.SectAngle"))
		param.m_fSectAngle=atof(tem_str);
	else if(pItem->m_idProp==GetPropID("para.RayVec.x"))
		param.m_xRayVec.x=atof(tem_str);
	else if(pItem->m_idProp==GetPropID("para.RayVec.y"))
		param.m_xRayVec.y=atof(tem_str);	
	else if(pItem->m_idProp==GetPropID("para.RayVec.z"))
		param.m_xRayVec.z=atof(tem_str);
	return true;
}
BOOL CTubeSafeHolderPropertyList::HelpPromptProperty(CPropTreeItem* pItem)
{
	g_pSolidDraw->ReleaseSnapStatus();
	if(pItem->m_idProp==GetPropID("BaseTube"))
		g_pSolidDraw->SetEntSnapStatus(param.m_hBaseTube);
	else if(pItem->m_idProp==GetPropID("BaseNode"))
		g_pSolidDraw->SetEntSnapStatus(param.m_hBaseNode);
	return TRUE;
}
bool CTubeSafeHolderPropertyList::CallBackCommand(UINT message,WPARAM wParam,BOOL *lpRslt)
{
	return TRUE;
}
BOOL CTubeSafeHolderPropertyList::FinishSelectObjOper(long hPickObj,long idEventProp)
{
	CCallBackDialog* pCallBackDlg=(CCallBackDialog*)GetParent();
	if(idEventProp==GetPropID("para.BaseTube"))
	{
		CLDSLineTube* pTube=(CLDSLineTube*)m_pBelongModel->FromPartHandle(hPickObj,CLS_LINETUBE);
		if(pTube!=NULL)
		{
			m_pBaseTube=pTube;
			param.m_hBaseTube=pTube->handle;
		}
	}
	else if(idEventProp==GetPropID("para.BaseNode"))
	{
		CLDSNode* pNode=m_pBelongModel->FromNodeHandle(hPickObj);
		if(pNode)
			param.m_hBaseNode=pNode->handle;
	}
	else
		return false;
	return true;
}
void CTubeSafeHolderPropertyList::DisplayPropertyList(int overwrite0_append1/*=0*/,DWORD dwPropGroup/*=1*/)
{
	CNativePropListManager oper(this);
	CleanTree();
	//������Ϣ
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL,*pRoot=GetRootItem();
	pGroupItem=oper.InsertPropItem(pRoot,"BasicInfo");
	oper.InsertEditPropItem(pGroupItem,"para.PartNo");
	oper.InsertCmbListPropItem(pGroupItem,"para.Material",MakeMaterialMarkSetString());
	oper.InsertCmbListPropItem(pGroupItem,"para.Spec",MakeSlotSetString());
	oper.InsertEditPropItem(pGroupItem,"para.Len");
	//װ����Ϣ
	pGroupItem=oper.InsertPropItem(pRoot,"AssemblyInfo");
	oper.InsertButtonPropItem(pGroupItem,"para.BaseTube");
	oper.InsertButtonPropItem(pGroupItem, "para.BaseNode");
	pPropItem=oper.InsertEditPropItem(pGroupItem,"para.ColumnR");
	pPropItem->SetReadOnly(TRUE);
	oper.InsertEditPropItem(pGroupItem,"para.LenOff");
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"para.ciSectNormStyle");
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"para.ciRotFromEdgeStyle");
	if(param.m_ciRotFromEdgeStyle==0)
	{
		oper.InsertEditPropItem(pPropItem,"para.RayVec.x");
		oper.InsertEditPropItem(pPropItem,"para.RayVec.y");
		oper.InsertEditPropItem(pPropItem,"para.RayVec.z");
	}
	oper.InsertEditPropItem(pGroupItem,"para.RayRayAngle");
	pPropItem=oper.InsertEditPropItem(pGroupItem,"para.Num");
	if(param.m_nNum>1)
		oper.InsertEditPropItem(pPropItem,"para.SectAngle");
	//
	Redraw();
}
bool CTubeSafeHolderPropertyList::DisplaySlavePropertyList(CPropTreeItem* pItem,DWORD dwRelaInfo/*=0*/)
{
	if(this->m_pRelaSubPropList==NULL)
		return false;
	CNativePropListManager oper(m_pRelaSubPropList);
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL,*pRoot=m_pRelaSubPropList->GetRootItem();
	m_pRelaSubPropList->CleanTree();
	pGroupItem=oper.InsertPropItem(pRoot,"VectInfo");
	if(pItem && pItem->m_idProp==GetPropID("para.RayVec"))
	{
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"para.ciRotFromEdgeStyle");
		oper.InsertEditPropItem(pPropItem,"para.RayVec.x");
		oper.InsertEditPropItem(pPropItem,"para.RayVec.y");
		oper.InsertEditPropItem(pPropItem,"para.RayVec.z");
	}
	m_pRelaSubPropList->Redraw();
	return true;
}
bool CTubeSafeHolderPropertyList::CreateComTemplInstance()
{
	CLDSLineTube* pBaseTube=(CLDSLineTube*)m_pBelongModel->FromPartHandle(param.m_hBaseTube,CLS_LINETUBE);
	CLDSNode* pBaseNode=m_pBelongModel->FromNodeHandle(param.m_hBaseNode);
	if(pBaseTube==NULL||pBaseNode==NULL)
	{
		AfxMessageBox("�����ֹܻ������ڵ㲻��Ϊ��!");
		return FALSE;
	}
	if(pBaseNode->hFatherPart!=pBaseTube->handle)
	{
		AfxMessageBox("��׼�ڵ㲻�������ֹ���!");
		return FALSE;
	}
	f3dPoint line_vec=(pBaseTube->End()-pBaseTube->Start()).normalized();
	if(fabs(line_vec*f3dPoint(0,0,1))<0.707)
	{
		AfxMessageBox("��Ʒ���ֱ�ֹ�!");
		return FALSE;
	}
	//������Ӧ���ߵ�����֧�����ģ��
	IStdComTempl::TEMPL_SOURCE source(0,IStdComTempl::STDTMPL_STRU_TUBE_WORKBENCH);
	//m_xSafeHolderTeml.InitComponentTempl(&source);
	CStdTemplParaNode* pParaNode=m_xSafeHolderTeml.listParaNodes.Add(1);				//��׼�ڵ�
	pParaNode->m_bNecessaryNode=true;
	CStdTemplParaRod* pParaRod=m_xSafeHolderTeml.listParaRods.Add(1,CLS_LINETUBE);	//�����ֹ�
	pParaRod->m_bNecessaryRod=true;
	//��Ӳ۸���Ʋ���
	if(param.m_ciRotFromEdgeStyle==2)
		param.m_fRayAngle+=pBaseTube->CalWeldRoadAngle()*DEGTORAD_COEF;
	CStdTemplParaSlot* pParaSlot=NULL;
	for(int i=0;i<param.m_nNum;i++)
	{
		pParaSlot=(CStdTemplParaSlot*)m_xSafeHolderTeml.listParaRods.Add(i+2,CLS_LINESLOT);
		pParaSlot->wiAttachTube=1;
		pParaSlot->fLen=param.m_fLen;
		pParaSlot->m_sGuige=param.m_sSpec;
		pParaSlot->cMaterial=param.m_cMat;
		pParaSlot->datum_pos.wiAttachNode=1;
		pParaSlot->datum_pos.len_offset=param.m_fLenOff;
		pParaSlot->datum_pos.ciNormalStyle = param.m_ciSectNormalStyle;
		pParaSlot->datum_pos.ciRotFromEdgeStyle= param.m_ciRotFromEdgeStyle;
		pParaSlot->datum_pos.ray_angle=param.m_fRayAngle+i*param.m_fSectAngle;
		pParaSlot->datum_pos.ray_vec[0]=param.m_xRayVec.x;
		pParaSlot->datum_pos.ray_vec[1]=param.m_xRayVec.y;
		pParaSlot->datum_pos.ray_vec[2]=param.m_xRayVec.z;
	}
	//ƥ���Ҫ��װ��˼����ڵ�
	m_xSafeHolderTeml.SetMappingNode(1,param.m_hBaseNode);
	m_xSafeHolderTeml.hashMatchRodHid.SetValue(1,param.m_hBaseTube);
	return TRUE;
}
//
bool CTubeSafeHolderPropertyList::InitializeUI(bool bInternalStart/*=false*/,DWORD dwParam/*=0*/)
{
	if(!bInternalStart)
	{
		m_pBaseTube->BuildUCS();
		InitPropHashtable();
		m_xSafeHolderTeml.Empty();
	}
	UpdateSketchMap();
	return true;
}
bool CTubeSafeHolderPropertyList::ConfirmOK()
{
	return true;	
}
bool CTubeSafeHolderPropertyList::CancelUI()
{
	return true;
}
