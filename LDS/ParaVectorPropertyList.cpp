#include "stdafx.h"
#include "LDS.h"
#include "NativePropertyList.h"
#include "Console.h"
#include "env_def.h"
#include "reportvectordlg.h"
#include "ReportPointDlg.h"
#include "CallBackDialog.h"
#include "ScrTools.h"

/////////////////////////////////////////////////////////////////////////////
// CParaVectorPropertyList 的属性栏
void CParaVectorPropertyList::CopyFrom(CLDSVector* pParaVector)
{
	CBuffer buffer;
	pParaVector->ToBuffer(buffer);
	buffer.SeekToBegin();
	m_xVector.FromBuffer(buffer);
	m_xVector.vector=pParaVector->vector;	//CLDSVector::From(To)Buffer中有时不包含方向的计算结果值　wjh-2015.9.9
}
void CParaVectorPropertyList::CopyTo(CLDSVector* pParaVector)
{
	CBuffer buffer;
	m_xVector.ToBuffer(buffer);
	buffer.SeekToBegin();
	pParaVector->FromBuffer(buffer);
	pParaVector->vector=m_xVector.vector;	//CLDSVector::From(To)Buffer中有时不包含方向的计算结果值　wjh-2015.9.9
}
long CParaVectorPropertyList::InitPropHashtable(long initId/*=1*/,long initHashSize/*=50*/,long initStatusHashSize/*=0*/)	//返回下一空闲Id
{
	long id=CNativePropertyList::InitPropHashtable(1,50,8);
	AddPropItem("datumVector",PROPLIST_ITEM(id++,""));
	AddPropItem("datumVector",PROPLIST_ITEM(id++,""));
	AddPropItem("description",PROPLIST_ITEM(id++,""));
	AddPropItem("vector",PROPLIST_ITEM(id++,"火曲法线"));
	AddPropItem("vector.x",PROPLIST_ITEM(id++,"火曲法线X轴方向坐标"));
	AddPropItem("vector.y",PROPLIST_ITEM(id++,"火曲法线Y轴方向坐标"));
	AddPropItem("vector.z",PROPLIST_ITEM(id++,"火曲法线Z轴方向坐标"));
	AddPropItem("norm_style",PROPLIST_ITEM(id++,"法线定位方式"));
	AddPropItem("hVicePart",PROPLIST_ITEM(id++,"基准杆件"));
	AddPropItem("hCrossPart",PROPLIST_ITEM(id++,"交叉参照杆件"));
	AddPropItem("norm_wing",PROPLIST_ITEM(id++,"基准肢"));
	AddPropItem("direction",PROPLIST_ITEM(id++,"朝向"));
	AddPropItem("rot_ang",PROPLIST_ITEM(id++,"旋转角度"));
	AddPropItem("VectorType",PROPLIST_ITEM(id++,"指定方向"));
	AddPropItem("nearVectorType",PROPLIST_ITEM(id++,"近似方向"));
	AddPropItem("nearVector.x",PROPLIST_ITEM(id++,"X分量"));
	AddPropItem("nearVector.y",PROPLIST_ITEM(id++,"Y分量"));
	AddPropItem("nearVector.z",PROPLIST_ITEM(id++,"Z分量"));
	return 0;
}
long CParaVectorPropertyList::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if (GetPropID("vector")==id)
		strcpy(sText,"");
	else if(GetPropID("vector.x")==id)
	{
		sprintf(sText,"%f",m_xVector.vector.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("vector.y")==id)
	{
		sprintf(sText,"%f",m_xVector.vector.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("vector.z")==id)
	{
		sprintf(sText,"%f",m_xVector.vector.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("norm_style")==id)
		strcpy(sText,m_xVector.GetNormStyleDes());	//法向属性
	else if(GetPropID("hVicePart")==id)
		sprintf(sText,"0X%X",m_xVector.hVicePart);
	else if(GetPropID("hCrossPart")==id)
		sprintf(sText,"0X%X",m_xVector.hCrossPart);
	else if(GetPropID("norm_wing")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_xVector.norm_wing==0)
			strcpy(sText,"0.X Leg Normal");
		else
			strcpy(sText,"1.Y Leg Normal");
#else 
		if(m_xVector.norm_wing==0)
			strcpy(sText,"0.X肢法线");
		else
			strcpy(sText,"1.Y肢法线");
#endif
	}
	else if(GetPropID("direction")==id)
		strcpy(sText,m_xVector.GetDirectionDes());
	else if(GetPropID("rot_ang")==id)
	{
		sprintf(sText,"%f",m_xVector.rot_ang);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("VectorType")==id)
		sprintf(sText,"%s",CStdVector::GetVecTypeString(m_xVector.vector));
	else if(GetPropID("nearVectorType")==id)
		sprintf(sText,"%s",CStdVector::GetVecTypeString(m_xVector.nearVector));
	else if(GetPropID("nearVector.x")==id)
	{
		sprintf(sText,"%lf",m_xVector.nearVector.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("nearVector.y")==id)
	{
		sprintf(sText,"%lf",m_xVector.nearVector.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("nearVector.z")==id)
	{
		sprintf(sText,"%lf",m_xVector.nearVector.z);
		SimplifiedNumString(sText);
	}
	else
		return 0;//false;
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
bool CParaVectorPropertyList::CallBackCommand(UINT message,WPARAM wParam,BOOL *lpRslt)
{
	return true;
}
bool CParaVectorPropertyList::ButtonClick(CPropTreeItem* pItem,BOOL *lpRslt)
{
	CXhChar100 sText;
	CCallBackDialog *pCallBackDlg = (CCallBackDialog *)GetParent();
	if(pCallBackDlg==NULL)
		return false;
	if(GetPropID("vector")==pItem->m_idProp)
	{
		CReportVectorDlg vecDlg;
		vecDlg.m_fVectorX=m_xVector.vector.x;
		vecDlg.m_fVectorY=m_xVector.vector.y;
		vecDlg.m_fVectorZ=m_xVector.vector.z;
		vecDlg.m_bReadOnlyX=(m_xVector.norm_style!=0);
		vecDlg.m_bReadOnlyY=(m_xVector.norm_style!=0);
		vecDlg.m_bReadOnlyZ=(m_xVector.norm_style!=0);
		if(vecDlg.DoModal()==IDOK)
		{
			m_xVector.vector.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			normalize(m_xVector.vector);
			long idProp=GetPropID("vector.x");
			if(GetPropValueStr(idProp,sText))
				SetItemPropValue(idProp,sText);
			idProp=GetPropID("vector.y");
			if(GetPropValueStr(idProp,sText))
				SetItemPropValue(idProp,sText);
			idProp=GetPropID("vector.z");
			if(GetPropValueStr(idProp,sText))
				SetItemPropValue(idProp,sText);	
			idProp=GetPropID("VectorType");
			if(GetPropValueStr(idProp,sText))
				SetItemPropValue(idProp,sText);	
		}
	}
	else if(GetPropID("hVicePart")==pItem->m_idProp
		||GetPropID("hCrossPart")==pItem->m_idProp)
	{
		if(m_xVector.norm_style==1)
			pCallBackDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);
		else if(m_xVector.norm_style==2||m_xVector.norm_style==5)
			pCallBackDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);
		else 
			pCallBackDlg->SelectObject(CLS_PART,pItem->m_idProp);
	}
	else 
		return false;
	return true;
}
bool CParaVectorPropertyList::ModifyValueFunc(CPropTreeItem* pItem,CString& valueStr,BOOL *lpRslt)
{
	if(GetPropID("norm_style")==pItem->m_idProp)
	{
		m_xVector.norm_style=valueStr[0]-'0';
		UpdateDatumVectorPropItem();
	}
	else if(GetPropID("vector.x")==pItem->m_idProp)
		m_xVector.vector.x=atof(valueStr);
	else if (GetPropID("vector.y")==pItem->m_idProp)
		m_xVector.vector.y=atof(valueStr);
	else if (GetPropID("vector.z")==pItem->m_idProp)
		m_xVector.vector.z=atof(valueStr);
	else if(GetPropID("hVicePart")==pItem->m_idProp)//基准杆件句柄
		sscanf(valueStr,"%d",&(m_xVector.hVicePart));
	else if(GetPropID("hCrossPart")==pItem->m_idProp)
		sscanf(valueStr,"%d",&(m_xVector.hCrossPart));
	else if(GetPropID("norm_wing")==pItem->m_idProp)
		m_xVector.norm_wing=valueStr[0]-'0';
	else if(GetPropID("direction")==pItem->m_idProp)
	{
		m_xVector.direction=valueStr[0]-'0';
		if(m_xVector.norm_style==5)
		{
			CSuperSmartPtr<CLDSPart>pDatumPart=console.FromPartHandle(m_xVector.hVicePart);
			if(pDatumPart.IsHasPtr()&&pDatumPart->IsTube())
			{
				if(m_xVector.direction<2)
					DeleteItemByPropId(GetPropID("rot_ang"));
				else
				{
					int idProp=GetPropID("rot_ang");
					if(FindItemByPropId(idProp,NULL)==NULL)
					{
						CItemInfo* lpInfo = new CItemInfo();
						lpInfo->m_controlType = PIT_COMBO;
						lpInfo->m_cmbType=CDT_EDIT;
						lpInfo->m_cmbItems = "0|90|180|270";
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName = "Rotation Angle";
						if(m_xVector.direction==2)
							lpInfo->m_strPropHelp="The rotation angle from tube's datum plane around the axial line(°)";
						else 
							lpInfo->m_strPropHelp="The rotation angle from tube's weld plane around the axial line(°)";
#else 
						lpInfo->m_strPropName = "旋转角度";
						if(m_xVector.direction==2)
							lpInfo->m_strPropHelp="自钢管基准线平面法向绕轴线的旋转角度(°)";
						else 
							lpInfo->m_strPropHelp="自钢管焊道线平面法向绕轴线的旋转角度(°)";
#endif
						CPropTreeItem* pSonItem = InsertItem(pItem,lpInfo, -1,TRUE);
						pSonItem->m_idProp = idProp;
						char item_text[100]="";
						if(m_xVector.GetPropValueStr(pSonItem->m_idProp,item_text)>0)
							pSonItem->m_lpNodeInfo->m_strPropValue = item_text;
					}
				}
			}
		}
	}
	else if(GetPropID("rot_ang")==pItem->m_idProp)
		m_xVector.rot_ang = atof(valueStr);
	else if(GetPropID("VectorType")==pItem->m_idProp)
	{
		if(valueStr[0]=='0')	//自定义方向
		{
			SetItemReadOnly(GetPropID("vector.x"),m_xVector.norm_style!=0);
			SetItemReadOnly(GetPropID("vector.y"),m_xVector.norm_style!=0);
			SetItemReadOnly(GetPropID("vector.z"),m_xVector.norm_style!=0);
		}
		else
		{
			SetItemReadOnly(GetPropID("vector.x"),TRUE);
			SetItemReadOnly(GetPropID("vector.y"),TRUE);
			SetItemReadOnly(GetPropID("vector.z"),TRUE);
			f3dPoint vector=CStdVector::GetVector(valueStr[0]-'0');
			m_xVector.vector=vector;
			CString sText;
			sText.Format("%.f",vector.x);
			SetItemPropValue(GetPropID("vector.x"),sText);
			sText.Format("%.f",vector.y);
			SetItemPropValue(GetPropID("vector.y"),sText);
			sText.Format("%.f",vector.z);
			SetItemPropValue(GetPropID("vector.z"),sText);
		}
	}
	else if(GetPropID("nearVectorType")==pItem->m_idProp)
	{
		if(valueStr[0]-'0'==0)	//自定义方向
		{
			CXhChar100 sText;
			CItemInfo *lpInfo;
			CPropTreeItem *pSonItem,*pFindItem=FindItemByPropId(GetPropID("nearVector.x"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "X Component";
				lpInfo->m_strPropHelp = "X Component";
#else 
				lpInfo->m_strPropName = "X分量";
				lpInfo->m_strPropHelp = "X分量";
#endif
				pSonItem = InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =GetPropID("nearVector.x");
				if(m_xVector.GetPropValueStr(pSonItem->m_idProp,sText)>0)
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}

			pFindItem=FindItemByPropId(GetPropID("nearVector.y"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Y Component";
				lpInfo->m_strPropHelp = "Y Component";
#else 
				lpInfo->m_strPropName = "Y分量";
				lpInfo->m_strPropHelp = "Y分量";
#endif
				pSonItem = InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =GetPropID("nearVector.y");
				if(m_xVector.GetPropValueStr(pSonItem->m_idProp,sText)>0)
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}

			pFindItem=FindItemByPropId(GetPropID("nearVector.z"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Z Component";
				lpInfo->m_strPropHelp = "Z Component";
#else 
				lpInfo->m_strPropName = "Z分量";
				lpInfo->m_strPropHelp = "Z分量";
#endif
				pSonItem = InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =GetPropID("nearVector.z");
				if(m_xVector.GetPropValueStr(pSonItem->m_idProp,sText)>0)
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
		else
		{
			DeleteItemByPropId(GetPropID("nearVector.x"));
			DeleteItemByPropId(GetPropID("nearVector.y"));
			DeleteItemByPropId(GetPropID("nearVector.z"));
			m_xVector.nearVector=CStdVector::GetVector(valueStr[0]-'0');
		}
	}
	else if(GetPropID("nearVector.x")==pItem->m_idProp)
		m_xVector.nearVector.x=atof(valueStr);
	else if(GetPropID("nearVector.y")==pItem->m_idProp)
		m_xVector.nearVector.y=atof(valueStr);
	else if(GetPropID("nearVector.z")==pItem->m_idProp)
		m_xVector.nearVector.z=atof(valueStr);
	else
		return false;
	if(m_xVector.norm_style!=0)
	{
		char value_str[100]="";
		m_xVector.UpdateVector(console.GetActiveModel());
		long idProp=GetPropID("vector.x");
		if(GetPropValueStr(idProp,value_str))
			SetItemPropValue(idProp,value_str);
		idProp=GetPropID("vector.y");
		if(GetPropValueStr(idProp,value_str))
			SetItemPropValue(idProp,value_str);
		idProp=GetPropID("vector.z");
		if(GetPropValueStr(idProp,value_str))
			SetItemPropValue(idProp,value_str);
	}
	return true;
}

void CParaVectorPropertyList::DisplayPropertyList(int overwrite0_append1/*=0*/,DWORD dwPropGroup/*=1*/)
{
	CXhChar100 sText;
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL,*pLeafItem=NULL,*pParentItem=GetRootItem();
	if(overwrite0_append1==0)
	{
		CleanTree();
		m_nObjClassTypeId = 0;
	}
	//
	CItemInfo* lpInfo = NULL;
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName="Vector Coordinate";
	lpInfo->m_strPropHelp="Vector Coordinate";
#else 
	lpInfo->m_strPropName="方向坐标";
	lpInfo->m_strPropHelp="方向坐标";
#endif
	pGroupItem = InsertItem(pParentItem,lpInfo,-1);
	pGroupItem->m_idProp = GetPropID("vector");
	if(GetPropValueStr(pGroupItem->m_idProp,sText))
		pGroupItem->m_lpNodeInfo->m_strPropValue=sText;
	pGroupItem->m_bHideChildren=FALSE;
	//X
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp="Normal's x component";
#else 
	lpInfo->m_strPropHelp="法线方向X方向分量";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = GetPropID("vector.x");
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	//Y
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp="Normal's y component";
#else 
	lpInfo->m_strPropHelp="法线方向Y方向分量";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = GetPropID("vector.y");
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	//Z
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp="Normal's z component";
#else 
	lpInfo->m_strPropHelp="法线方向Z方向分量";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = GetPropID("vector.z");
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	//基准点定位方式
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="0.Specify Direction|1.Single Angle Leg Normal|2.Two Rod Cross-bracing Normal|3.Part Work Normal|4.Part Relative Direction|5.Rod Characteristic Direction";
	lpInfo->m_strPropName="Normal Locate Mode";
	lpInfo->m_strPropHelp="Normal locate mode";
#else 
	lpInfo->m_cmbItems="0.指定方向|1.单角钢肢法线|2.两杆件交叉面法线|3.构件工作法线|4.构件上的相对方向|5.杆件上的特征方向";
	lpInfo->m_strPropName="法线定位方式";
	lpInfo->m_strPropHelp="法线定位方式";
#endif
	pGroupItem = InsertItem(pParentItem,lpInfo,-1);
	pGroupItem->m_idProp = GetPropID("norm_style");
	if(GetPropValueStr(pGroupItem->m_idProp,sText))
		pGroupItem->m_lpNodeInfo->m_strPropValue=sText;
	pGroupItem->m_bHideChildren=FALSE;
	//法线属性
	UpdateDatumVectorPropItem();
	
	Redraw();
}
void CParaVectorPropertyList::UpdateDatumVectorPropItem()
{
	CItemInfo* lpInfo=NULL;
	CXhChar100 sText;
	CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL;
	CPropTreeItem* pParentItem=FindItemByPropId(GetPropID("norm_style"),NULL);
	DeleteAllSonItems(pParentItem);
	if(m_xVector.norm_style!=0)
	{
		SetItemReadOnly(GetPropID("vector.x"),TRUE);
		SetItemReadOnly(GetPropID("vector.y"),TRUE);
		SetItemReadOnly(GetPropID("vector.z"),TRUE);
	}
	else 
	{
		int nVectorType=CStdVector::GetVectorType(m_xVector.vector);
		SetItemReadOnly(GetPropID("vector.x"),nVectorType!=0);
		SetItemReadOnly(GetPropID("vector.y"),nVectorType!=0);
		SetItemReadOnly(GetPropID("vector.z"),nVectorType!=0);
	}
	//指定法线
	if(m_xVector.norm_style==0)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specify Direction";
		lpInfo->m_strPropHelp = "Specified Direction";
#else 
		lpInfo->m_strPropName = "指定方向";
		lpInfo->m_strPropHelp = "指定方向";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,TRUE);
		pPropItem->m_idProp =GetPropID("VectorType");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	//单角钢肢法线
	else if(m_xVector.norm_style==1)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Angle";
		lpInfo->m_strPropHelp = "Datum angle's handle";
#else 
		lpInfo->m_strPropName = "基准角钢";
		lpInfo->m_strPropHelp = "基准角钢句柄";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,TRUE);
		pPropItem->m_idProp =GetPropID("hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems ="0.X Leg Normal|1.Y Leg Norma";
		lpInfo->m_strPropName = "Datum Leg";
		lpInfo->m_strPropHelp = "Datum Leg";
#else 
		lpInfo->m_cmbItems ="0.X肢法线|1.Y肢法线";
		lpInfo->m_strPropName = "基准肢";
		lpInfo->m_strPropHelp = "基准肢";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,TRUE);
		pPropItem->m_idProp =GetPropID("norm_wing");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems ="0.Outward|1.Inward";
		lpInfo->m_strPropName = "Direction";
		lpInfo->m_strPropHelp = "Direction";
#else 
		lpInfo->m_cmbItems ="0.朝外|1.朝里";
		lpInfo->m_strPropName = "朝向";
		lpInfo->m_strPropHelp = "朝向";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,TRUE);
		pPropItem->m_idProp =GetPropID("direction");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	else if(m_xVector.norm_style==2)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Rod";
		lpInfo->m_strPropHelp = "Datum rod's handle";
#else 
		lpInfo->m_strPropName = "基准杆件";
		lpInfo->m_strPropHelp = "基准杆件句柄";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,TRUE);
		pPropItem->m_idProp =GetPropID("hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Cross-bracing Reference Rod";
		lpInfo->m_strPropHelp = "Cross-bracing reference rod's handle";
#else 
		lpInfo->m_strPropName = "交叉参照杆件";
		lpInfo->m_strPropHelp = "交叉参照杆件句柄";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp =GetPropID("hCrossPart");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Near Direction";
		lpInfo->m_strPropHelp = "The near direction distinguish whether or not forward normal direction";
#else 
		lpInfo->m_strPropName = "近似方向";
		lpInfo->m_strPropHelp = "近似方向用于区别计算方向为交叉面的正法线还是负法线方向";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,TRUE);
		pPropItem->m_idProp =GetPropID("nearVectorType");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		int nearVectorType=CStdVector::GetVectorType(m_xVector.nearVector);
		if(nearVectorType==0)	//自定义
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Component";
			lpInfo->m_strPropHelp = "X Component";
#else 
			lpInfo->m_strPropName = "X分量";
			lpInfo->m_strPropHelp = "X分量";
#endif
			pSonPropItem = InsertItem(pPropItem,lpInfo, -1,TRUE);
			pSonPropItem->m_idProp =GetPropID("nearVector.x");
			if(GetPropValueStr(pSonPropItem->m_idProp,sText))
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y Component";
			lpInfo->m_strPropHelp = "Y Component";
#else 
			lpInfo->m_strPropName = "Y分量";
			lpInfo->m_strPropHelp = "Y分量";
#endif
			pSonPropItem = InsertItem(pPropItem,lpInfo, -1,TRUE);
			pSonPropItem->m_idProp =GetPropID("nearVector.y");
			if(GetPropValueStr(pSonPropItem->m_idProp,sText))
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z Component";
			lpInfo->m_strPropHelp = "Z Component";
#else 
			lpInfo->m_strPropName = "Z分量";
			lpInfo->m_strPropHelp = "Z分量";
#endif
			pSonPropItem = InsertItem(pPropItem,lpInfo, -1,TRUE);
			pSonPropItem->m_idProp =GetPropID("nearVector.z");
			if(GetPropValueStr(pSonPropItem->m_idProp,sText))
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		}
	}
	else if(m_xVector.norm_style==3)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Part";
		lpInfo->m_strPropHelp = "Datum part";
#else 
		lpInfo->m_strPropName = "基准构件";
		lpInfo->m_strPropHelp = "基准构件";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,TRUE);
		pPropItem->m_idProp =GetPropID("hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems ="0.Forward direction|1.Reverse direction";
		lpInfo->m_strPropName = "Direction";
		lpInfo->m_strPropHelp = "Forward direction is datum part's plus normal ；reverse direction is opposite direction";
#else 
		lpInfo->m_cmbItems ="0.正向|1.反向";
		lpInfo->m_strPropName = "朝向";
		lpInfo->m_strPropHelp = "正向是指基准构件的正工作法线方向；反向是指基准构件的负工作法线方向";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,TRUE);
		pPropItem->m_idProp =GetPropID("direction");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	else if(m_xVector.norm_style==4)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Part";
		lpInfo->m_strPropHelp = "Datum Part";
#else 
		lpInfo->m_strPropName = "基准构件";
		lpInfo->m_strPropHelp = "基准构件";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,TRUE);
		pPropItem->m_idProp =GetPropID("hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Relative Direction";
		lpInfo->m_strPropHelp = "The relative direction of datum part on assemble coordinate system";
#else 
		lpInfo->m_strPropName = "相对方向";
		lpInfo->m_strPropHelp = "基准构件装配坐标系下的相对方向";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,TRUE);
		pPropItem->m_idProp =GetPropID("nearVectorType");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		int nearVectorType=CStdVector::GetVectorType(m_xVector.nearVector);
		if(nearVectorType==0)	//自定义
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Component";
			lpInfo->m_strPropHelp = "X Component";
#else 
			lpInfo->m_strPropName = "X分量";
			lpInfo->m_strPropHelp = "X分量";
#endif
			pSonPropItem = InsertItem(pPropItem,lpInfo, -1,TRUE);
			pSonPropItem->m_idProp =GetPropID("nearVector.x");
			if(GetPropValueStr(pSonPropItem->m_idProp,sText))
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y Component";
			lpInfo->m_strPropHelp = "Y Component";
#else 
			lpInfo->m_strPropName = "Y分量";
			lpInfo->m_strPropHelp = "Y分量";
#endif
			pSonPropItem = InsertItem(pPropItem,lpInfo, -1,TRUE);
			pSonPropItem->m_idProp =GetPropID("nearVector.y");
			if(GetPropValueStr(pSonPropItem->m_idProp,sText))
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z Component";
			lpInfo->m_strPropHelp = "Z Component";
#else 
			lpInfo->m_strPropName = "Z分量";
			lpInfo->m_strPropHelp = "Z分量";
#endif
			pSonPropItem =InsertItem(pPropItem,lpInfo, -1,TRUE);
			pSonPropItem->m_idProp =GetPropID("nearVector.z");
			if(GetPropValueStr(pSonPropItem->m_idProp,sText))
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		}
	}
	else if(m_xVector.norm_style==5)
	{	//5.基准杆件上的特征方向
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Rod";
		lpInfo->m_strPropHelp = "Datum rod's handle";
#else 
		lpInfo->m_strPropName = "基准杆件";
		lpInfo->m_strPropHelp = "基准杆件句柄";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,TRUE);
		pPropItem->m_idProp =GetPropID("hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		//方向类型
		bool bNeedRotAnglePara=false;
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_xVector.hVicePart,CLS_LINEPART);
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		if(pLinePart&&pLinePart->IsAngle())
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems ="0.Start->End|1.End->Start|2.X Leg Normal+|3.X Leg Normal-|4.Y Leg Normal+|5.Y Leg Normal-";
#else 
			lpInfo->m_cmbItems ="0.始->终|1.终->始|2.X肢法线+|3.X肢法线-|4.Y肢法线+|5.Y肢法线-";
#endif
		else if(pLinePart&&pLinePart->IsTube())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems = "0.Start->End|1.End->Start|2.Rotate Along Datum Plane|3.Rotate Along Weld Road Plane";
#else 
			lpInfo->m_cmbItems = "0.始->终|1.终->始|2.自基准面旋转|3.自焊道面旋转";
#endif
			if(m_xVector.direction>=2)
				bNeedRotAnglePara=true;
		}
		else 
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems ="0.Start->End|1.End->Start";
#else 
			lpInfo->m_cmbItems ="0.始->终|1.终->始";
#endif
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Direction Type";
		lpInfo->m_strPropHelp = "0.Rod start->end direction,1.Rod end->start direction,2.Angle X-leg  normal,3.Angle X-leg normal reverse direction,4.Angle Y-leg  normal,5.Angle Y-leg normal reverse direction";
#else 
		lpInfo->m_strPropName = "方向类型";
		lpInfo->m_strPropHelp = "0.杆件始->终方向,1.杆件终->始方向,2.角钢X肢法线方向,3.角钢X肢法线负方向,4.角钢Y肢法线方向,5.角钢Y肢法线负方向";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,TRUE);
		pPropItem->m_idProp =GetPropID("direction");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bNeedRotAnglePara)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbType=CDT_EDIT;
			lpInfo->m_cmbItems = "0|90|180|270";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Rotation Angle";
			if(m_xVector.direction==2)
				lpInfo->m_strPropHelp="The rotation angle from tube's datum plane around the axial line°";
			else 
				lpInfo->m_strPropHelp="The rotation angle from tube's weld plane around the axial line°";
#else 
			lpInfo->m_strPropName = "旋转角度";
			if(m_xVector.direction==2)
				lpInfo->m_strPropHelp="自钢管基准线平面法向绕轴线的旋转角度°";
			else 
				lpInfo->m_strPropHelp="自钢管焊道线平面法向绕轴线的旋转角度°";
#endif
			pSonPropItem = InsertItem(pPropItem,lpInfo, -1,TRUE);
			pSonPropItem->m_idProp = GetPropID("rot_ang");
			if(GetPropValueStr(pSonPropItem->m_idProp,sText))
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		}
	}
}
BOOL CParaVectorPropertyList::FinishSelectObjOper(long hPickObj,long idEventProp)
{
	if(GetPropID("hVicePart")==idEventProp)
	{
		CLDSPart *pPart=NULL;
		if(m_xVector.norm_style==1)
			pPart=console.FromPartHandle(hPickObj,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		else if(m_xVector.norm_style==2||m_xVector.norm_style==5)
			pPart=console.FromPartHandle(hPickObj,CLS_LINEPART);
		else 
			pPart=console.FromPartHandle(hPickObj);
		if(pPart)
		{
			m_xVector.hVicePart=pPart->handle;
			m_xVector.UpdateVector(console.GetActiveModel());
		}
	}
	else if(m_xVector.norm_style==2&&GetPropID("hCrossPart")==idEventProp)
	{
		CLDSPart *pPart=console.FromPartHandle(hPickObj);
		if(pPart)
		{
			m_xVector.hCrossPart=pPart->handle;
			m_xVector.UpdateVector(console.GetActiveModel());
		}
	}
	else 
		return FALSE;
	return TRUE;
}