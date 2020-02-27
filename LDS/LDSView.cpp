//<LOCALE_TRANSLATE Confirm by hxr/>

//////////////////////////////////////////////////////////////////
//                  �����ź��������Ƽ����޹�˾
// �ļ���        :      LDSView.cpp
// �汾          :		V2.0
// Ŀ�ļ���Ҫ����:		��ӦLDS���󲿷ֹ����������˵�����
// ��������      :      2003.6.05
// �޸�����      :		2005.5.10
// ������        :
////////////////////////////////////////////////////////////////

// LDSView.cpp : implementation of the CLDSView class
//

#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "GUID.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "MainFrm.h"
#include "UserDefMsg.h"
#include "database.h"
#include "dlg.h"
#include "DesignShadowDlg.h"
#include "DesignOuterAttachDlg.h"
#include "InputAnValDlg.h"
#include "LsCheckParaDlg.h"
#include "SetNcDrvDlg.h"
#include "JgNcDataManager.h"
#include "NcJg.h"
#include "NcPlank.h"
#include "direct.h"
#include "GroupEditLayerDlg.h"
#include "PolyStyleDlg.h"
#include "ModifyVertexDlg.h"
#include "PoleAuthPermDlg.h"
#include "PartAuthPermDlg.h"
#include "SolidCollideOptionDlg.h"
#include "SpecZhiWanFaceVecDlg.h"
#include "DefScopeStruDlg.h"
#include "NewFileDlg.h"
#include "DefinedView.h"
#include "image.h"
#include "lds_part.h"
#include "TowerPropertyDlg.h"
#include "CalInt3dllDlg.h"
#include "3dLFDlg.h"
#include "CfgPartNoDlg.h"
#include "NodeDesignPosDlg.h"
#include "Query.h"
#include "LmaDlg.h"
#include "MirTaAtom.h"
#include "OffsetPartDlg.h"
#include "LinePartDivideDlg.h"
#include "PlateLsRefDlg.h"
#include "CompareLinePartPosDlg.h"
#include "AddSetupAidedPlateDlg.h"
#include "DesignJoint.h"
#include "IntelligentConnectParaDlg.h"
#include "LibraryQuery.h"
//#include "XhCharString.h"
#include "SortFunc.h"
//#include "ArrayList.h"
#include "MutiThreadTask.h"
#include "DesignSimpleLadderDlg.h"
#if defined(__LDS_)||defined(__TSA_)
#include "PreProcessor.h"
#include "PostProcessor.h"
#include "PostProcessReport.h"
#include "FLBillDlg.h"
#include "RevisionOperDlg.h"
#endif
#include "SuppPartListDlg.h"
#include "MoveBoltDlg.h"
#include "PartDrawMarkDlg.h"
#include "SetArcLiftDlg.h"
#include ".\Log\XhLog.h"
#include "LogViewerDlg.h"
#include "SolidBody.h"
#include "DefineOffsetPosDlg.h"
#include "ModifyPartNoSegIDlg.h"
#include "UniformNormDLg.h"
#include "LogFile.h"
#include "DesignColumnRayRodDlg.h"
#include "AngleJointsDesignForm.h"
#include "LicFuncDef.h"
#include "SearchPartDlg.h"
#include "HandleReplaceDlg.h"
#include "ReusePartsDlg.h"
#include "BoltInfoDlg.h"
#include "RetrieveSegIDlg.h"
#include "ProcessPart.h"
#include "SelAssotiatedBoltsDlg.h"
#include "PlateNcSetDlg.h"
#include "DrawingToolKits.h"
#include "SnapTypeVerify.h"
#include "KeypointLifeObj.h"
#include "OriginalData.h"
#include "OutputInfo.h"
#include "DataCompareDlg.h"
#include "MsgBox.h"
#ifdef __COMMON_PART_INC_
#include "PPEBuffer.h"
#include "LabelOperDlg.h"
#include "LabelProp.h"
#include "BoltHoleInfo.h"
#include "UI.h"
#endif
#include "ListenCollector.h"
#include "MirOperObject.h"
#include "XhObjHandler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CLDSView

::CEvent LButtonDown,RButtonDown,HaltProc;
static BOOL g_bFileFirstOpen;
//BOOL g_bRedrawSolid;
void InitClassTypeIdSelectPartsTable()
{
	int arrClsTypeIdTableArr[32]={0};
	arrClsTypeIdTableArr[SELECTINDEX_NODE-1]=CLS_NODE;
	//arrClsTypeIdTableArr[SELECTINDEX_LINE-1]=CLS_LINEPART;
	arrClsTypeIdTableArr[SELECTINDEX_LINEANGLE-1]=CLS_LINEANGLE;
	arrClsTypeIdTableArr[SELECTINDEX_LINETUBE-1]=CLS_LINETUBE;
	arrClsTypeIdTableArr[SELECTINDEX_LINEFLAT-1]=CLS_LINEFLAT;
	arrClsTypeIdTableArr[SELECTINDEX_LINESLOT-1]=CLS_LINESLOT;
	arrClsTypeIdTableArr[SELECTINDEX_PLATE-1]=CLS_PLATE;
	arrClsTypeIdTableArr[SELECTINDEX_PARAMPLATE-1]=CLS_PARAMPLATE;
	arrClsTypeIdTableArr[SELECTINDEX_BOLT-1]=CLS_BOLT;
	arrClsTypeIdTableArr[SELECTINDEX_ARCANGLE-1]=CLS_ARCANGLE;
	arrClsTypeIdTableArr[SELECTINDEX_ARCFLAT-1]=CLS_ARCFLAT;
	arrClsTypeIdTableArr[SELECTINDEX_ARCSLOT-1]=CLS_ARCSLOT;
	arrClsTypeIdTableArr[SELECTINDEX_ARCTUBE - 1] = CLS_ARCTUBE;
	arrClsTypeIdTableArr[SELECTINDEX_HOLEWALL - 1] = CLS_HOLEWALL;
	g_pSolidSet->SetClassTypeIdTable(arrClsTypeIdTableArr);
}
bool GetPartsFromScr(GET_SCR_PART_PARA &part_para,BOOL bCaptureLButtonUpMsg/*=FALSE*/)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	DISPLAY_TYPE disp_type;
	int iOrder=0;
	char sText[300]="";
	g_pSolidDraw->ReleaseSnapStatus();
	//�趨��ǰ��Ҫ�ĵ���/ʵ����ʾ��ʽ
	g_pSolidSet->GetDisplayType(&disp_type);
	if(part_para.disp_type!=disp_type)
	{
		g_pSolidSet->SetDisplayType(part_para.disp_type);
		g_pSolidDraw->Draw();
	}
	pCmdLine->FillCmdLine(part_para.cmdStr,"");
	DWORD dwOldSelectPartType=g_pSolidSnap->SetSelectPartsType(part_para.dwSelectPartType);
	while(1)
	{
		CString cmdStr;
		if(!pCmdLine->GetStrFromCmdLine(cmdStr,bCaptureLButtonUpMsg?CCmdLineDlg::LBUTTONUP_AS_RETURN:0))
		{
			if(g_pSolidSnap)
				g_pSolidSnap->SetSelectPartsType(dwOldSelectPartType);
			return false;
		}
		long *id_arr;
		if(part_para.nResultObjs<=1)
		{
			//�����·����ڴ�֮ǰӦ���ͷ������ڴ� wht 11-05-27
			if(part_para.pResultObjsArr)
				delete []part_para.pResultObjsArr;
			part_para.pResultObjsArr=NULL;
			part_para.pResultObjsArr=new CLDSDbObject* [1];
			part_para.pResultObjsArr[0]=NULL;
			if(g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
			{
				if(part_para.cls_id1==CLS_NODE)	//ѡ��ڵ�
					part_para.pResultObjsArr[0]=console.FromNodeHandle(id_arr[0]);
				else
				{
					if(part_para.cls_id1>0&&part_para.cls_id2>0)
						part_para.pResultObjsArr[0]=console.FromPartHandle(id_arr[0],part_para.cls_id1,part_para.cls_id2);
					else if(part_para.cls_id1>0&&part_para.cls_id2==0)
						part_para.pResultObjsArr[0]=console.FromPartHandle(id_arr[0],part_para.cls_id1);
					else if(part_para.cls_id1==0&&part_para.cls_id2>0)
						part_para.pResultObjsArr[0]=console.FromPartHandle(id_arr[0],part_para.cls_id2);
					else 
						part_para.pResultObjsArr[0]=(CLDSDbObject*)console.FromHandle(id_arr[0],TRUE);
				}
			}
			if(part_para.pResultObjsArr[0]==NULL&&part_para.cmdErrorFeedbackStr.GetLength()>0)
			{	//�д���ѡ�����ַ���ʱ��Ҫ����ѡ��
				pCmdLine->FillCmdLine(part_para.cmdErrorFeedbackStr,"");
				g_pSolidDraw->ReleaseSnapStatus();
				continue;
			}
			else
			{
				if(part_para.pResultObjsArr[0]==NULL)
					part_para.nResultObjs=0;
				else
					part_para.nResultObjs=1;
				break;
			}
		}
		else
		{
			long *id_arr1=NULL,arr1_len=0;;
			if(cmdStr.GetLength()>0)
			{
				id_arr1=new long[cmdStr.GetLength()];
				sprintf(sText,"%s",cmdStr);
				for(char* key=strtok(sText,",");key;key=strtok(NULL,","))
				{
					long h;
					sscanf(key,"%X",&h);
					id_arr1[arr1_len]=h;
					arr1_len++;
				}
			}
			long *id_arr;
			int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
			int nArrayLen=max(arr1_len+n,part_para.nResultObjs);
			//�����·����ڴ�֮ǰӦ���ͷ������ڴ� wht 11-05-27
			if(part_para.pResultObjsArr)
				delete []part_para.pResultObjsArr;
			part_para.pResultObjsArr=NULL;
			part_para.pResultObjsArr = new CLDSDbObject*[nArrayLen];
			part_para.nResultObjs=n+arr1_len;	//��¼ʵ��ѡ���˶��ٸ�����
			for(int j=0;j<nArrayLen;j++)
			{
				part_para.pResultObjsArr[j]=NULL;
				if(j<arr1_len)
				{
					if(part_para.cls_id1==CLS_NODE)	//ѡ��ڵ�
						part_para.pResultObjsArr[j]=console.FromNodeHandle(id_arr1[j]);
					else
					{
						if(part_para.cls_id1>0&&part_para.cls_id2>0)
							part_para.pResultObjsArr[j]=console.FromPartHandle(id_arr1[j],part_para.cls_id1,part_para.cls_id2);
						else if(part_para.cls_id1>0&&part_para.cls_id2==0)
							part_para.pResultObjsArr[j]=console.FromPartHandle(id_arr1[j],part_para.cls_id1);
						else if(part_para.cls_id1==0&&part_para.cls_id2>0)
							part_para.pResultObjsArr[j]=console.FromPartHandle(id_arr1[j],part_para.cls_id2);
						else 
							part_para.pResultObjsArr[j]=console.FromPartHandle(id_arr1[j]);
					}
				}
				else if(j<n+arr1_len)
				{
					if(part_para.cls_id1==CLS_NODE)	//ѡ��ڵ�
						part_para.pResultObjsArr[j]=console.FromNodeHandle(id_arr[j-arr1_len]);
					else
					{
						if(part_para.cls_id1>0&&part_para.cls_id2>0)
							part_para.pResultObjsArr[j]=console.FromPartHandle(id_arr[j-arr1_len],part_para.cls_id1,part_para.cls_id2);
						else if(part_para.cls_id1>0&&part_para.cls_id2==0)
							part_para.pResultObjsArr[j]=console.FromPartHandle(id_arr[j-arr1_len],part_para.cls_id1);
						else if(part_para.cls_id1==0&&part_para.cls_id2>0)
							part_para.pResultObjsArr[j]=console.FromPartHandle(id_arr[j-arr1_len],part_para.cls_id2);
						else 
							part_para.pResultObjsArr[j]=console.FromPartHandle(id_arr[j-arr1_len]);
					}
				}
			}
			if(id_arr1)
				delete []id_arr1;
		}
		break;
	}
	g_pSolidSnap->SetSelectPartsType(dwOldSelectPartType);
	//�ָ�ԭ�еĵ���/ʵ����ʾ��ʽ
	if(part_para.disp_type!=disp_type)
	{
		g_pSolidSet->SetDisplayType(disp_type);
		g_pSolidDraw->Draw();
	}
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("����:","");
#endif
	return true;
}
const DWORD HASHTABLESIZE = 500;
const DWORD STATUSHASHTABLESIZE = 50;
int CLDSView::m_iCurDisplayPropGroup=0;
CHashStrList<PROPLIST_ITEM> CLDSView::propHashtable;
CXhChar100 CLDSView::HELPLINK_URL;
CHashSet<PROPLIST_ITEM*>CLDSView::hashPropItemById;
CHashTable<DWORD> CLDSView::propStatusHashtable;
CSysPromptObject CLDSView::prompt;
CLDSView::PROMPT_LIFE::PROMPT_LIFE(bool disableAutoCloseOnUse/*=false*/)
{
	m_bDisableAutoCloseOnUse=disableAutoCloseOnUse;
	m_bAutoClose=true;
}
CLDSView::PROMPT_LIFE::~PROMPT_LIFE()
{
	if(m_bAutoClose)
		CLDSView::prompt.Close();
}
void CLDSView::PROMPT_LIFE::Prompt(const char* promptstr){
	if(m_bDisableAutoCloseOnUse)
		m_bAutoClose=false;
	CLDSView::prompt.Prompt(promptstr);
}
void CLDSView::PROMPT_LIFE::SetPromptPicture(long bitmapId){
	if(m_bDisableAutoCloseOnUse)
		m_bAutoClose=false;
	CLDSView::prompt.SetPromptPicture(bitmapId);
}
void CLDSView::PROMPT_LIFE::SetPromptPicture(const char* bmpfile){
	if(m_bDisableAutoCloseOnUse)
		m_bAutoClose=false;
	CLDSView::prompt.SetPromptPicture(bmpfile);
}
void CLDSView::InitWorkViewPropHashtable()
{
	int id=1;
	CLDSView::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSView::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CLDSView::AddPropItem("VCS",PROPLIST_ITEM(id++,"View Coordinate System"));	//View Coordinate System,����ǰ��ͼ��ʾ����ϵ
	CLDSView::AddPropItem("vcs.origin",PROPLIST_ITEM(id++,"Datum Position"));
	CLDSView::AddPropItem("vcs.origin.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSView::AddPropItem("vcs.origin.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSView::AddPropItem("vcs.origin.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSView::AddPropItem("vcs.axis_x",PROPLIST_ITEM(id++,"X Axis Direction"));
	CLDSView::AddPropItem("vcs.axis_x.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSView::AddPropItem("vcs.axis_x.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSView::AddPropItem("vcs.axis_x.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSView::AddPropItem("vcs.axis_y",PROPLIST_ITEM(id++,"Y Axis Direction"));
	CLDSView::AddPropItem("vcs.axis_y.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSView::AddPropItem("vcs.axis_y.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSView::AddPropItem("vcs.axis_y.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSView::AddPropItem("vcs.axis_z",PROPLIST_ITEM(id++,"Z Axis Direction"));
	CLDSView::AddPropItem("vcs.axis_z.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSView::AddPropItem("vcs.axis_z.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSView::AddPropItem("vcs.axis_z.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSView::AddPropItem("WCS",PROPLIST_ITEM(id++,"Work Coordinate System"));	//Work Coordinate System
	CLDSView::AddPropItem("wcs.origin",PROPLIST_ITEM(id++,"Datum Position"));
	CLDSView::AddPropItem("wcs.origin.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSView::AddPropItem("wcs.origin.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSView::AddPropItem("wcs.origin.z",PROPLIST_ITEM(id++,""));
	CLDSView::AddPropItem("wcs.axis_x",PROPLIST_ITEM(id++,"X Axis Direction"));
	CLDSView::AddPropItem("wcs.axis_x.a",PROPLIST_ITEM(id++,"������нǦ�"));
	CLDSView::AddPropItem("wcs.axis_x.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSView::AddPropItem("wcs.axis_x.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSView::AddPropItem("wcs.axis_x.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSView::AddPropItem("wcs.axis_y",PROPLIST_ITEM(id++,"Y Axis Direction"));
	CLDSView::AddPropItem("wcs.axis_y.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSView::AddPropItem("wcs.axis_y.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSView::AddPropItem("wcs.axis_y.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSView::AddPropItem("wcs.axis_z",PROPLIST_ITEM(id++,""));
	CLDSView::AddPropItem("wcs.axis_z.x",PROPLIST_ITEM(id++,""));
	CLDSView::AddPropItem("wcs.axis_z.y",PROPLIST_ITEM(id++,""));
	CLDSView::AddPropItem("wcs.axis_z.z",PROPLIST_ITEM(id++,""));
#else
	CLDSView::AddPropItem("residePlateInfo",PROPLIST_ITEM(id++,"�����ְ���Ϣ"));	//View Coordinate System,����ǰ��ͼ��ʾ����ϵ
	CLDSView::AddPropItem("m_hSubjectPlate",PROPLIST_ITEM(id++,"�����ְ���"));	//View Coordinate System,����ǰ��ͼ��ʾ����ϵ
	CLDSView::AddPropItem("m_sSubjectPlatePartNo",PROPLIST_ITEM(id++,"�����ְ���"));	//View Coordinate System,����ǰ��ͼ��ʾ����ϵ
	CLDSView::AddPropItem("vcs.origin",PROPLIST_ITEM(id++,"λ��"));
	CLDSView::AddPropItem("vcs.origin.x",PROPLIST_ITEM(id++,"X����"));
	CLDSView::AddPropItem("vcs.origin.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSView::AddPropItem("vcs.origin.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSView::AddPropItem("vcs.axis_x",PROPLIST_ITEM(id++,"X�᷽��"));
	CLDSView::AddPropItem("vcs.axis_x.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSView::AddPropItem("vcs.axis_x.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSView::AddPropItem("vcs.axis_x.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSView::AddPropItem("vcs.axis_y",PROPLIST_ITEM(id++,"Y�᷽��"));
	CLDSView::AddPropItem("vcs.axis_y.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSView::AddPropItem("vcs.axis_y.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSView::AddPropItem("vcs.axis_y.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSView::AddPropItem("vcs.axis_z",PROPLIST_ITEM(id++,"Z�᷽��"));
	CLDSView::AddPropItem("vcs.axis_z.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSView::AddPropItem("vcs.axis_z.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSView::AddPropItem("vcs.axis_z.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSView::AddPropItem("WCS",PROPLIST_ITEM(id++,"��������ϵ"));	//Work Coordinate System
	CLDSView::AddPropItem("wcs.origin",PROPLIST_ITEM(id++,"λ��"));
	CLDSView::AddPropItem("wcs.origin.x",PROPLIST_ITEM(id++,"X����"));
	CLDSView::AddPropItem("wcs.origin.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSView::AddPropItem("wcs.origin.z",PROPLIST_ITEM(id++,""));
	CLDSView::AddPropItem("wcs.axis_x",PROPLIST_ITEM(id++,"X�᷽��"));
	CLDSView::AddPropItem("wcs.axis_x.a",PROPLIST_ITEM(id++,"������нǦ�"));
	CLDSView::AddPropItem("wcs.axis_x.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSView::AddPropItem("wcs.axis_x.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSView::AddPropItem("wcs.axis_x.z",PROPLIST_ITEM(id++,""));
	CLDSView::AddPropItem("wcs.axis_y",PROPLIST_ITEM(id++,"Y�᷽��"));
	CLDSView::AddPropItem("wcs.axis_y.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSView::AddPropItem("wcs.axis_y.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSView::AddPropItem("wcs.axis_y.z",PROPLIST_ITEM(id++,""));
	CLDSView::AddPropItem("wcs.axis_z",PROPLIST_ITEM(id++,""));
	CLDSView::AddPropItem("wcs.axis_z.x",PROPLIST_ITEM(id++,""));
	CLDSView::AddPropItem("wcs.axis_z.y",PROPLIST_ITEM(id++,""));
	CLDSView::AddPropItem("wcs.axis_z.z",PROPLIST_ITEM(id++,""));
#endif
}

CString CLDSView::GetPropValueStr(long id)
{
	CString valueStr;
	CLDSGeneralPlate* pWcsBelongPlate=NULL;
	if(console.m_pWorkPart&&console.m_pWorkPart->IsPlate())
		pWcsBelongPlate=(CLDSGeneralPlate*)console.m_pWorkPart;
	if(GetPropID("m_hSubjectPlate")==id)
	{
		if(pWcsBelongPlate)
			valueStr.Format("0X%X",pWcsBelongPlate->handle);
	}
	else  if(GetPropID("m_sSubjectPlatePartNo")==id)
	{
		if(pWcsBelongPlate)
			valueStr.Format("%s",(char*)pWcsBelongPlate->GetPartNo());
	}
	else if(GetPropID("vcs.origin.x")==id)
	{
		valueStr.Format("%f",console.m_curACS.origin.x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("vcs.origin.y")==id)
	{
		valueStr.Format("%f",console.m_curACS.origin.y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("vcs.origin.z")==id)
	{
		valueStr.Format("%f",console.m_curACS.origin.z);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("vcs.axis_x.x")==id)
	{
		valueStr.Format("%f",console.m_curACS.axis_x.x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("vcs.axis_x.y")==id)
	{
		valueStr.Format("%f",console.m_curACS.axis_x.y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("vcs.axis_x.z")==id)
	{
		valueStr.Format("%f",console.m_curACS.axis_x.z);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("vcs.axis_y.x")==id)
	{
		valueStr.Format("%f",console.m_curACS.axis_y.x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("vcs.axis_y.y")==id)
	{
		valueStr.Format("%f",console.m_curACS.axis_y.y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("vcs.axis_y.z")==id)
	{
		valueStr.Format("%f",console.m_curACS.axis_y.z);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("vcs.axis_z.x")==id)
	{
		valueStr.Format("%f",console.m_curACS.axis_z.x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("vcs.axis_z.y")==id)
	{
		valueStr.Format("%f",console.m_curACS.axis_z.y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("vcs.axis_z.z")==id)
	{
		valueStr.Format("%f",console.m_curACS.axis_z.z);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("wcs.origin.x")==id)
	{
		valueStr.Format("%f",console.m_curWCS.origin.x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("wcs.origin.y")==id)
	{
		valueStr.Format("%f",console.m_curWCS.origin.y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("wcs.origin.z")==id)
	{
		valueStr.Format("%f",console.m_curWCS.origin.z);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("wcs.axis_x.a")==id)
	{
		double alfa=Cal2dLineAng(0,0,console.m_curWCS.axis_x.x,console.m_curWCS.axis_x.y);
		alfa*=DEGTORAD_COEF;
		valueStr.Format("%f",alfa);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("wcs.axis_x.x")==id)
	{
		valueStr.Format("%f",console.m_curWCS.axis_x.x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("wcs.axis_x.y")==id)
	{
		valueStr.Format("%f",console.m_curWCS.axis_x.y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("wcs.axis_x.z")==id)
	{
		valueStr.Format("%f",console.m_curWCS.axis_x.z);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("wcs.axis_y.x")==id)
	{
		valueStr.Format("%f",console.m_curWCS.axis_y.x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("wcs.axis_y.y")==id)
	{
		valueStr.Format("%f",console.m_curWCS.axis_y.y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("wcs.axis_y.z")==id)
	{
		valueStr.Format("%f",console.m_curWCS.axis_y.z);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("wcs.axis_z.x")==id)
	{
		valueStr.Format("%f",console.m_curWCS.axis_z.x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("wcs.axis_z.y")==id)
	{
		valueStr.Format("%f",console.m_curWCS.axis_z.y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("wcs.axis_z.z")==id)
	{
		valueStr.Format("%f",console.m_curWCS.axis_z.z);
		SimplifiedNumString(valueStr);
	}
	else
		return "";
	return valueStr;
}

long CLDSView::GetPropID(char* propStr)
{
	PROPLIST_ITEM *pItem=CLDSView::propHashtable.GetValue(propStr);
	if(pItem!=NULL)
		return pItem->id;
	else
	{
		/*CString prompt="û�ҵ���Ϊ\"";
		prompt+=propStr;
		prompt+="\"������ID��";
		AfxMessageBox(prompt);*/
		return 0;
	}
}

int CLDSView::GetPropStatus(long id)
{
	DWORD dwStatus;
	if(CLDSView::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//������
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//����
	}
	return -1;	//δ�ҵ������״̬
}

BOOL CLDSView::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSView::propStatusHashtable.SetValueAt(id,dwStatus);
}

IMPLEMENT_DYNCREATE(CLDSView, CView)
BEGIN_MESSAGE_MAP(CLDSView, CView)
	//{{AFX_MSG_MAP(CLDSView)
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_SHARE_JDB_DESIGN, OnShareJdbDesign)
	ON_COMMAND(ID_MERGE_LJB, OnMergeLjb)
	ON_COMMAND(ID_CUT_PLATE_BY_PLANE, OnCutPlateByPlane)
	ON_COMMAND(ID_ROTATE_PART_AROUND_AXIS, OnRotatePartAroundAxis)
	ON_COMMAND(ID_CUT_JG, OnCutJg)
	ON_COMMAND(ID_SEG_I_CHECK, OnSegICheck)
	ON_COMMAND(ID_PART_NO_CHECK, OnPartNoCheck)
	ON_COMMAND(ID_LOCATE_PART_BY_NO, OnLocatePartByNo)
	ON_COMMAND(ID_DRAW_TA_IN_ACAD, OnDrawTaInAcad)
	ON_COMMAND(ID_LINEPART_EXTEND_VEC, OnLinePartExtendVec)
	ON_COMMAND(ID_TA_DAO_WEI, OnTaDaoWei)
	ON_COMMAND(ID_EDIT_FIND_PART, OnEditFindPart)
	ON_COMMAND(ID_HIDE_PART, OnHidePart)
	ON_COMMAND(ID_INVERSE_SELECT_OBJS, OnInverseSelectObjs)
	ON_COMMAND(ID_CHECK_PART, OnCheckPartData)
	ON_COMMAND(ID_DISPLAY_PART_WORK_UCS, OnDisplayPartWorkUcs)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_HANDLE_REPLACE, &CLDSView::OnHandleReplace)
	ON_COMMAND(ID_CLEAR_DESIGN_RESULT, OnClearDesignResult)
	ON_COMMAND(ID_EXPLODE_HUOQU, OnExplodeHuoqu)
	ON_COMMAND(ID_HAND_SHORT_JG, OnHandShortJg)
	ON_COMMAND(ID_ONE_KEY_CUT_ANGLE_CHECK,OnOneKeyCutAngleCheck)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_SEND_PART_EDITOR, OnSendPartEditor)
	ON_UPDATE_COMMAND_UI(ID_SEND_PART_EDITOR, OnUpdateSendPartEditor)
	ON_COMMAND(ID_SEND_PPE_EDITOR, OnSendPpeEditor)
	ON_COMMAND(ID_ASSEMBLE_CS, OnAssembleCs)
	ON_COMMAND(ID_WORK_CS, OnWorkCs)
	ON_COMMAND(ID_ATTACH_PART_TBL, OnAttachPartTbl)
	ON_COMMAND(ID_IMPORT_PART, OnImportPart)
	ON_COMMAND(ID_FEATURE_PROP, OnFeatureProp)
	ON_COMMAND(ID_NEW_LS, OnNewLs)
	ON_COMMAND(ID_SPEC_WCS_ORGIN, OnSpecWcsOrgin)
	ON_COMMAND(ID_SPEC_AXIS_X_VERTEX, OnSpecAxisXVertex)
	ON_COMMAND(ID_DEL_PART_FEATURE, OnDelPartFeature)
	ON_COMMAND(ID_MEASURE_VERTEX_DIST, OnMeasureVertexDist)
	ON_COMMAND(ID_INSERT_VERTEX, OnInVertex)
	ON_COMMAND(ID_FOLD_PLANK, OnFoldPlank)
	ON_COMMAND(ID_RE_CAL_HUOQU_POS, OnReCalHuoquPos)
	ON_COMMAND(ID_CLEAR_NAIL, OnClearNail)
	ON_COMMAND(ID_CODE_PART_NO_RULE, OnCodePartNoRule)
	ON_COMMAND(ID_CODE_PART_NO, OnCodePartNo)
	ON_COMMAND(ID_INSERT_JGNODE_TO_VIEW, OnInsertJgnodeToView)
	ON_COMMAND(ID_REMOVE_OBJS_FROM_VIEW, OnRemoveObjsFromView)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_CLEAR_PART_NO_I, OnClearPartNoI)
	ON_COMMAND(ID_CHECK_JG_LS_VALID, OnCheckJgLsValid)
	ON_COMMAND(ID_SET_NC_DRIVER, OnSetNcDriver)
	ON_COMMAND(ID_GEN_JG_NC_FILE, OnGenJgNcFile)
	ON_COMMAND(ID_GEN_PART_PROCESSINFO_FILE,OnGenPartProcessInfoFile)
	ON_COMMAND(ID_DISPLAY_OR_HIDE_ALL_HOLE, OnDisplayOrHideAllHole)
	ON_COMMAND(ID_GEN_PLANK_NC_DATA, OnGenPlankNcData)
	ON_COMMAND(ID_MOVE_DBOBJECT, OnMoveDbObjPosition)
	ON_COMMAND(ID_IMPORT_PLANK_PROFILE, OnImportPlankProfile)
	ON_COMMAND(ID_MAKE_END_BEND, OnMakeEndBend)
	ON_COMMAND(ID_AUTO_CAL_PLANK_PROFILE, OnAutoCalPlankProfile)
	ON_COMMAND(ID_BEND_PLANK, OnBendPlank)
	ON_COMMAND(ID_CONVERT_TO_GROUP_JG, OnConvertToGroupJg)
	ON_COMMAND(ID_ADD_FILL_PLANK, OnAddFillPlank)
	ON_COMMAND(ID_DESIGN_GUAXIAN_HOLE,OnDesignGuaXianHole)
	ON_COMMAND(ID_DESIGN_BUSHTUBE,OnDesignBushTube)
	ON_COMMAND(ID_SPEC_LS_RAY_NO, OnSpecLsRayNo)
	ON_UPDATE_COMMAND_UI(ID_HIDE_PART, OnUpdateHidePart)
	ON_COMMAND(ID_DAOWEI_SELECT_PART, OnDaoweiSelectPart)
	ON_COMMAND(ID_CONVERT_TO_SINGLE_JG, OnConvertToSingleJg)
	ON_COMMAND(ID_AUTO_CORRECT_JG_LAYSTYLE, OnAutoCorrectJgLaystyle)
	ON_COMMAND(ID_SOLID_COLLIDE_CHECK, OnSolidCollideCheck)
	ON_COMMAND(ID_CHECK_PLANK_LS_VALID, OnCheckPlankLsValid)
	ON_COMMAND(ID_ROLL_PLANK_EDGE, OnRollPlankEdge)
	ON_COMMAND(ID_HANDOVER_PERMISSION, OnHandoverPermission)
	ON_COMMAND(ID_RETURN_TO_ASSEMBLE_ENV, OnReturnToAssembleEnv)
	ON_COMMAND(ID_REMOVE_KAIHE_JIAO, OnRemoveKaiheJiao)
	ON_COMMAND(ID_PREV_PART, OnPrevPart)
	ON_COMMAND(ID_NEXT_PART, OnNextPart)
	ON_UPDATE_COMMAND_UI(ID_PREV_PART, OnUpdatePrevPart)
	ON_UPDATE_COMMAND_UI(ID_NEXT_PART, OnUpdateNextPart)
	ON_COMMAND(ID_SPEC_CUT_FACE_CUT_JG, OnSpecCutFaceCutJg)
	ON_COMMAND(ID_ANGLE_FACE_OFFSET_ODD_DIFF, OnAngleFaceOffsetOddDiff)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_NEW_TOWER_MODEL, OnFileNewTowerModel)
	ON_COMMAND(ID_NEW_SUBSTATION_MODEL, OnFileNewSubstation)
	ON_COMMAND(ID_SAVE_AS_NEW_VIEW, OnSaveAsNewView)
	ON_COMMAND(ID_LINK_LINEPART, OnLinkLinePart)
	ON_COMMAND(ID_OPEN_WND_SEL, OnOpenWndSel)
	ON_COMMAND(ID_DEF_CTRL_PT, OnDefCtrlPt)
	ON_COMMAND(ID_DEF_OFFSET_NODE, OnDefOffsetNode)
	ON_COMMAND(ID_DEF_INTERS_NODE, OnDefIntersNode)
	ON_COMMAND(ID_DEF_HORIZON_NODE, OnDefHorizonNode)
	ON_COMMAND(ID_DEF_AXIS_VAL_FIX_NODE, OnDefAxisValFixNode)
	ON_COMMAND(ID_DEF_ZHU_JG_POLY, OnDefZhuJgPoly)
	ON_COMMAND(ID_MAKE_JG_BEND, OnMakeJgBend)
	ON_COMMAND(ID_CAL_1JG_NORM, OnCal1jgNorm)
	ON_COMMAND(ID_GL_OPEN_WINDOW, OnGlOpenWindow)
	ON_UPDATE_COMMAND_UI(ID_GL_OPEN_WINDOW, OnUpdateGlOpenWindow)
	ON_COMMAND(ID_GL_ALL_ZOOM, OnGlAllZoom)
	ON_COMMAND(ID_GL_ZOOM, OnGlZoom)
	ON_COMMAND(ID_GL_PAN, OnGlPan)
	ON_UPDATE_COMMAND_UI(ID_GL_PAN, OnUpdateGlPan)
	ON_UPDATE_COMMAND_UI(ID_GL_ZOOM, OnUpdateGlZoom)
	ON_COMMAND(ID_GL_ROTATED, OnGlRotated)
	ON_UPDATE_COMMAND_UI(ID_GL_ROTATED, OnUpdateGlRotated)
	ON_COMMAND(ID_RESET_VIEW, OnResetView)
	ON_COMMAND(ID_OPER_OTHER,  OnOperOther)
	ON_COMMAND(ID_OPER_CONTROL_CENTER, OnOperControlCenter)
	ON_COMMAND(ID_NODE_CENTER, OnNodeCenter)
	ON_COMMAND(ID_REDRAW_ALL, OnRedrawAll)
	ON_COMMAND(ID_SPREAD_FACE, OnSpreadFace)
	ON_COMMAND(ID_SPREAD_NODE_FACE, OnSpreadNodeFace)
	ON_COMMAND(ID_SELECT_NODE_FACE, OnSelectNodeFace)
	ON_COMMAND(ID_DESIGN_JDB, OnDesignJdb)
	ON_COMMAND(ID_CIRCULAR_PLATE, OnCircularPlate)
	ON_COMMAND(ID_GY_HOLE, OnGyHole)
	ON_COMMAND(ID_XIE_INTERS_PT_DESIGN, OnXieIntersPtDesign)
	ON_COMMAND(ID_SINGLE_ANGLE_ONEBOLT_END_DESIGN, OnSingleXieNodeDesign)
	ON_COMMAND(ID_ALL_SINGLE_XIE_NODE_DESIGN, OnAllSingleXieNodeDesign)
	ON_COMMAND(ID_ONEBOLT_DESIGN, OnOneBoltDesign)
	ON_COMMAND(ID_SPEC_NODE_ONEBOLT_DESIGN, OnSpecNodeOneBoltDesign)
	ON_COMMAND(ID_LAY_JG_END_LS, OnLayJgEndLs)
	ON_COMMAND(ID_COMMON_JG_JOINT, OnCommonJgJoint)
	ON_COMMAND(ID_LAY_FILL_PLANK, OnLayFillPlank)
	ON_COMMAND(ID_LAYOUT_FOOTNAIL, OnLayoutFootnail)
	ON_COMMAND(ID_FOOT_PLANK, OnFootPlank)
	ON_COMMAND(ID_PART_PERM_SWITCH, OnPartPermSwitch)
	ON_UPDATE_COMMAND_UI(ID_PART_PERM_SWITCH, OnUpdatePartPermSwitch)
	ON_COMMAND(ID_REFRESH_BURGLAR_LS, OnRefreshBurglarLs)
	ON_COMMAND(ID_DISPLAY_COORD_AXIS, OnDisplayCoordAxis)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_COORD_AXIS, OnUpdateDisplayCoordAxis)
	ON_COMMAND(ID_EXPORT_BMP_FILE, OnExportBmpFile)
	ON_COMMAND(ID_DISPLAY_FRAMELINE, OnDisplayFrameLine)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_FRAMELINE, OnUpdateDisplayFrameLine)
	ON_COMMAND(ID_DISPLAY_LINE_VEC, OnDisplayLineVec)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_LINE_VEC, OnUpdateDisplayLineVec)
	ON_COMMAND(ID_DISPLAY_LINE_NODE, OnDisplayLineNode)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_LINE_NODE, OnUpdateDisplayLineNode)
	ON_COMMAND(ID_VIEW_JG_KEY_POINT, OnViewJgKeyPoint)
	ON_COMMAND(ID_TOOL_KIT, OnToolKit)
	ON_COMMAND(ID_MEASURE, OnMeasure)
	ON_COMMAND(ID_CAL_POLE_INTERS_PT, OnCalJgWingZhunIntersPt)
	ON_COMMAND(ID_DES_HOOF_PLANK, OnDesHoofPlank)
	ON_COMMAND(ID_CAL_WORK_FACE_NORM, OnCalWorkFaceNorm)
	ON_COMMAND(ID_AUTO_DESIGN_SELECTED_PARTS, OnAutoDesignSelectedParts)
	ON_COMMAND(ID_REVISION_BOLT_L0nZPOS, OnRevisionBoltL0nZpos)
	ON_COMMAND(ID_REVISION_BOLT_SPACE, OnRevisionBoltSpace)
	ON_COMMAND(ID_OBJECT_PROPERTY, OnObjectProperty)
	ON_COMMAND(ID_DEF_DIVIDE_SCALE_NODE, OnDefDivideScaleNode)
	ON_COMMAND(ID_DESIGN_LJ_PLATE, OnDesignLjPlate)
	ON_COMMAND(ID_DESIGN_RIB_PLATE, OnDesignRibPlate)
	ON_COMMAND(ID_DESIGN_OVERLAPPED_PLATE, OnDesignOverlappedPlate)
	ON_COMMAND(ID_BREAK_LINEPART, OnBreakLinePart)
	ON_COMMAND(ID_MIRROR_CREATE, OnMirrorCreate)
	ON_COMMAND(ID_DESIGN_SPHERE, OnDesignSphere)
	ON_UPDATE_COMMAND_UI(ID_MIRROR_CREATE, OnUpdateMirrorCreate)
	ON_COMMAND(ID_RECREATE_SPEC_PART_SOLIDMODEL, OnRecreateSpecPartSolidmodel)
	ON_COMMAND(ID_DESIGN_LINEPART_ODDMENT, OnDesignLinepartOddment)
	ON_COMMAND(ID_DESIGN_ELBOW_PLATE, OnDesignElbowPlate)
	ON_COMMAND(ID_IMPORT_BOLT_REF_TO_PART, OnImportBoltRefToPart)
	//ON_COMMAND(ID_LOOK_BOLT_IN_PART, OnLookBoltInPart)
	ON_COMMAND(ID_IMPORT_LINEPART_TO_PLATE, OnImportLinePartToPlate)
	ON_COMMAND(ID_LOOK_LINEPART_IN_PLATE, OnLookLinePartInPlate)
	ON_COMMAND(ID_RETRIEVE_PLATE_SEG_I,OnRetrievePartSegI)
	ON_COMMAND(ID_CREATE_BOLT_IN_PART, OnCreateBoltInPart)
	ON_COMMAND(ID_ADD_RELATIVE_OBJECT, OnAddRelativeObject)
	ON_UPDATE_COMMAND_UI(ID_ADD_RELATIVE_OBJECT, OnUpdateAddRelativeObject)
	ON_UPDATE_COMMAND_UI(ID_CREATE_BOLT_IN_PART, OnUpdateCreateBolt)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_BOLT_REF_TO_PART, OnUpdateImportBoltRef)
	//ON_UPDATE_COMMAND_UI(ID_LOOK_BOLT_IN_PART, OnUpdateLookBolt)
	ON_COMMAND(ID_COMPOSE_LINEPART, OnComposeLinepart)
	ON_COMMAND(ID_TUBE_BETWEEN_PLATE, OnTubeBetweenPlate)
	ON_COMMAND(ID_EXTEND_PLATE, OnExtendPlate)
	ON_COMMAND(ID_SPEC_WCS_ORGIN_BY_GLOBAL_CS, OnSpecWcsOrginByGlobalCs)
	ON_COMMAND(ID_NEW_ARCPART, OnNewArcPart)
	ON_COMMAND(ID_GROUP_EDIT_PART_LAYER, OnGroupEditPartLayer)
	ON_COMMAND(ID_BOLT_VALID_CHECK, OnBoltValidCheck)
	ON_COMMAND(ID_BOLT_PAD, OnBoltPad)
	ON_COMMAND(ID_INSERT_PLATE_STYLE_TUBE_INTERS, OnInsertPlateStyleTubeInters)
	ON_COMMAND(ID_CONVERT_TO_PLATE, OnConvertToPlate)
	ON_COMMAND(ID_CUT_PLATE_BY_COLUMN_FACE, OnCutPlateByColumnFace)
	ON_COMMAND(ID_DIVIDE_MAIN_LINEPART, OnDivideMainLinePart)
	ON_COMMAND(ID_ADJUST_PLATE_WELD_LEN, OnAdjustPlateWeldLen)
	ON_COMMAND(ID_COMPARE_LINEPART_POS, OnCompareLinepartPos)
	ON_COMMAND(ID_TURN_ANGLE_CLOCKWISE, OnTurnAngleClockwise)
	ON_COMMAND(ID_TURN_ANGLE_ANTICLOCKWISE, OnTurnAngleAnticlockwise)
	ON_COMMAND(ID_LOOK_DESIGN_PARAM, OnLookDesignParam)
	ON_COMMAND(ID_CHECK_BOLT_VALID, OnCheckBoltValid)
	ON_COMMAND(ID_SETUP_AIDED_PLATE, OnSetupAidedPlate)
	ON_COMMAND(ID_AFFIRM_CUR_COMMAND, OnAffirmCurCommand)
	ON_COMMAND(ID_CANCEL_CUR_COMMAND, OnCancelCurCommand)
	ON_COMMAND(ID_DEL_SPECIFY_PART, OnDelSpecifyPart)
	ON_COMMAND(ID_INTELLIGENT_CONNECT_DESIGN, OnIntelligentConnectDesign)
	ON_COMMAND(ID_STDCOMTMPL_REPOSITORY, OnStdComTmplRepository)
	ON_COMMAND(ID_CONNECT_FROM_TEMPL, OnConnectFromTemplate)
	ON_COMMAND(ID_EXPORT_CONNECT_TEMPL, OnExportConnectionTemplate)
	ON_COMMAND(ID_COPY_TO_CLIPBOARD, OnCopyToClipboard)
	ON_COMMAND(ID_CLASSIC_CONNECT_DESIGN, OnClassicConnectDesign)
	ON_COMMAND(ID_TURN_ANGLE_INOUT_SIDE, OnTurnAngleInoutSide)
	ON_COMMAND(ID_TURN_ANGLE_WING, OnTurnAngleWing)
	ON_COMMAND(ID_IMPORT_EXIST_LS, OnImportExistLs)
	ON_COMMAND(ID_DESIGN_SPECIAL_FL, OnDesignSpecialFl)
	ON_COMMAND(ID_DESIGN_SIMPLE_LADDER, OnDesignSimpleLadder)
	ON_COMMAND(ID_DEF_ALL_INTERS_NODE, OnDefAllIntersNode)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_OR_HIDE_ALL_HOLE, OnUpdateDisplayAllHole)
	ON_COMMAND(ID_MAKE_PLATE_BEND, OnMakePlateBend)
	ON_COMMAND(ID_FAST_CIRCOVER_PLATE, OnFastCirCoverPlate)
	ON_COMMAND(ID_FAST_CIRRIB_PLATE, OnFastCirRibPlate)
	ON_COMMAND(ID_FAST_TUBERIB_PLATE, OnFastTubeRibPlate)
	ON_COMMAND(ID_CHECK_RELATIVE_MIR_INFO, OnCheckRelativeMirInfo)
	ON_COMMAND(ID_VALIDATE_CROSSROD_BEARCOMP_INFO, OnValidateCrossrodBearcompInfo)
	ON_COMMAND(ID_SHADOW_CREATE, OnShadowCreate)
	ON_COMMAND(ID_OUTER_ATTACH_ANGLE, OnOuterAttachAngle)
	ON_COMMAND(ID_DESIGN_TUBE_TOWER_FOOT, OnDesignTubeTowerFoot)
	ON_COMMAND(ID_DESIGN_TUBE_TOWER_SLOPE, OnDesignTubeTowerSlope)
	ON_COMMAND(ID_BILL_OF_FL, OnBillOfFL)
	ON_COMMAND(ID_ANGLE_JOINT_DESIGN_CAL, OnAngleJointDesignCal)
	ON_COMMAND(ID_REPLACE_BOLT_DATUMPOINT, OnReplaceBoltDatumpoint)
	ON_COMMAND(ID_ALIGN_BOLT_ALONG_LENVEC, OnAlignBoltAlongLenvec)
	ON_COMMAND(ID_BOLT_PUNCH_HOLE,OnBoltPunchHole)
	ON_COMMAND(ID_SWAP_G,OnSwapBoltG)
	ON_COMMAND(ID_MOVE_BOLT_SET, OnMoveBoltSet)
	ON_COMMAND(ID_UNIFY_BOLT_NORM,OnUnifyBoltNorm)
	ON_COMMAND(ID_PLATE_PROFILE_RECT, OnPlateProfileToRect)
	ON_COMMAND(ID_COPY_PARTSET, OnCopyPartSet)
	ON_COMMAND(ID_SELECT_RELA_BOLTS, OnSelectRelaBolts)
	ON_COMMAND(ID_ADD_RELA_BOLTS_TO_VIEW, OnAddRelaBoltsToView)
	ON_COMMAND(ID_SET_SPREAD_FACE, OnSetPartSpreadFace)
	ON_COMMAND(ID_SET_ARC_LIFT, OnSetArcLift)
	ON_COMMAND(ID_CORRECT_BOLT_L0_PARA, OnCorrectBoltL0Para)
	ON_COMMAND(ID_CONVERT_TO_TUBE, OnConvertToTube)
	ON_COMMAND(ID_OPER_SHANK_SECTION, OnOperShankSection)
	ON_COMMAND(ID_ADD_LEG_AND_BRACE_RODS, OnAddLegAndBraceRods)
	ON_COMMAND(ID_ADD_GTM_BRACE_ELEM_PANELS, OnAddGtmBraceElemPanels)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_SAVE_AS_NEW_BLOCK, OnSaveAsNewBlock)
	ON_COMMAND(ID_EXPLODE_BLOCKREF,OnExplodeBlockRef)
	ON_COMMAND(ID_LOG_VIEWER, OnLogViewer)
	ON_COMMAND(ID_DESIGN_COLUMN_RAY_ROD, OnDesignColumnRayRod)
	ON_COMMAND(ID_START_WINGX_CUT_ANGLE, OnStartWingXCutAngle)
	ON_COMMAND(ID_START_WINGY_CUT_ANGLE, OnStartWingYCutAngle)
	ON_COMMAND(ID_END_WINGX_CUT_ANGLE, OnEndWingXCutAngle)
	ON_COMMAND(ID_END_WINGY_CUT_ANGLE, OnEndWingYCutAngle)
	ON_COMMAND(ID_WINGX_CUT_ANGLE, OnWingXCutAngle)
	ON_COMMAND(ID_WINGY_CUT_ANGLE, OnWingYCutAngle)
	ON_COMMAND(ID_UPGRADE_CTRL_NODE, OnUpgradeCtrlNode)
	ON_COMMAND(ID_REUSE_PARTS,OnReuseParts)
	ON_COMMAND(ID_CREATE_FACEPANEL,OnCreateFacePanel)
	ON_COMMAND(ID_PPE_STARTER,OnPPEStarter)
	ON_WM_HELPINFO()
	ON_WM_DROPFILES()
	ON_COMMAND(ID_DATA_COMPARE,OnDataCompare)
	ON_COMMAND(ID_MECHANICS_CHECK,OnMechanicsCheck)
	ON_COMMAND(ID_OPTIMIZE_PLATE,OnOptimizePlate)
	ON_MESSAGE(WM_OBJECT_SNAPED, ObjectSnappedProcess)
	ON_MESSAGE(WM_OBJECT_SELECTED, ObjectSelectProcess)
	ON_MESSAGE(WM_CMDLINE_AFFIRMED, CmdLineAffirmed)
	ON_MESSAGE(WM_CMDLINE_CANCELED, CmdLineCanceled)
	ON_COMMAND(ID_CAL_PERFECT_SECT_COUNT, &CLDSView::OnCalPerfectSectCount)
	ON_COMMAND(ID_MERGE_NODES, &CLDSView::OnMergeNodes)
	ON_COMMAND(ID_RELAYOUT_SCALE_NODE, &CLDSView::OnRelayoutScaleNodes)
	ON_COMMAND(ID_SYNC_MIR_OBJ_PROP, &CLDSView::OnSyncMirObjProp)
	ON_COMMAND(ID_REMOVE_RELA_OBJECTS, &CLDSView::OnRemoveRelaObjects)
	ON_UPDATE_COMMAND_UI(ID_REMOVE_RELA_OBJECTS, &CLDSView::OnUpdateRemoveRelaObjects)
#ifndef __STATION_
	ON_COMMAND(ID_INTELLI_CORRECT_OBJ_LAYER,&CLDSView::OnIntelliCorrectModelObjsLayer)
#endif
	ON_COMMAND(ID_UNIFY_ANGLE_BOLT_G,OnUnifyAngleBoltsParamG)
	ON_COMMAND(ID_ADD_ASSIST_LINE, &CLDSView::OnAddAssistLine)
	ON_COMMAND(ID_NEXT_VIEW, &CLDSView::OnNextView)
	ON_COMMAND(ID_PREV_VIEW, &CLDSView::OnPrevView)
	ON_UPDATE_COMMAND_UI(ID_PREV_VIEW, &CLDSView::OnUpdatePrevView)
	ON_UPDATE_COMMAND_UI(ID_NEXT_VIEW, &CLDSView::OnUpdateNextView)
	ON_COMMAND(ID_BASE_PLATE, &CLDSView::OnBasePlate)
	ON_COMMAND(ID_ANGLE_SHOE_PLATE, &CLDSView::OnAngleShoePlate)
	ON_COMMAND(ID_REVISION_MODE, &CLDSView::OnRevisionMode)
	ON_UPDATE_COMMAND_UI(ID_REVISION_MODE, &CLDSView::OnUpdateRevisionMode)
	ON_COMMAND(ID_SIZE_DIMENSION, &CLDSView::OnSizeDimension)
	ON_COMMAND_RANGE(ID_PERSPECTIVE_VIEW, ID_DOWNWARD_VIEW, &CLDSView::OnShiftView)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PERSPECTIVE_VIEW, ID_DOWNWARD_VIEW, &CLDSView::OnUpdateShiftView)
	ON_COMMAND(ID_BATCH_INPUT_ROD_LABEL, &CLDSView::OnLabelInput)
	ON_COMMAND(ID_BRANCH_NODES, &CLDSView::OnBranchNodes)
#ifdef __RAPID_ROUGH_3DMODEL_
	ON_COMMAND(ID_INTELLI_RELAYOUT_RODMODEL, &CLDSView::OnIntelliRelayoutRodModel)
	ON_COMMAND(ID_TRUNK_MODEL_TREE, &CLDSView::OnTrunkModelTree)
	ON_COMMAND(ID_SET_FOUNDATION_PARAM, &CLDSView::OnSetFoundationParams)
	ON_COMMAND(ID_GENERATE_ROUGH_3D_MODEL, &CLDSView::OnGenerateRough3DModel)
	ON_COMMAND(ID_UPDATE_BASEFOOT_INFO, &CLDSView::OnUpdateBaseFootInfo)
#endif
#ifdef __COMMON_PART_INC_
	ON_COMMAND(ID_ADD_ANCHOR_BOLTS, &CLDSView::OnAddAnchorBolts)
	ON_COMMAND(ID_CUT_OPEN_BY_FITTING, &CLDSView::OnCutOpenByFitting)
#endif
	//}}AFX_MSG_MAP
	// Standard printing commands
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLDSView construction/destruction

extern BOOL g_bObjectDeleted;
CLDSView::CLDSView()	
{
	m_pSelectBlock=NULL;

	m_curTask = TASK_OTHER;
	m_eViewFlag	  = FRONT_VIEW;
	m_bStartOpenWndSelTest = FALSE;
	m_bEnableOpenWndSelectTest=TRUE;
	m_bInputCharByCmdLine = TRUE;
	UndoViewBuffStack.SetSize(10);
	g_pPromptMsg = new CPromptDlg(this);
	m_curAppType = TA_ASSEMBLE_ENV;
#ifndef AFX_TARG_ENU_ENGLISH
	g_pBoltInfoDlg = new CBoltInfoDlg(this);
#endif
#ifndef __TSA_
	g_pLsRefDlg = new CPlateLsRefDlg(this);
#endif
	g_pSearchPartDlg = new CSearchPartDlg(this);
#if defined(__TMA_)||defined(__LMA_)
	g_pDataCompareDlg= new CDataCompareDlg(this);
#endif
	g_bObjectDeleted = FALSE;
	m_dwCookie=0;
	m_nPrevCommandID=0;
#ifdef __GGG_
	m_pDraw2d=NULL;
#endif
}

CLDSView::~CLDSView()
{
	g_pSolidSet->ExitComponent();	//��ֹ��ʱ��SnapPoint,SnapLine�Ⱥ����ڹرճ���ʱ���ɱ�������״̬����޷��رճ���
	DisconnectSolidSrcComp();
	CDrawSolidFactory::Destroy(m_pDrawSolid->GetSerial());
	g_pSolidDraw=NULL;
	g_pSolidOper=NULL;
	g_pSolidSet=NULL;
	g_pSolidSnap=NULL;
	/*if(g_pSolidDraw)
		g_pSolidDraw->Release();
	g_pSolidDraw=NULL;
	if(g_pSolidSet)
		g_pSolidSet->Release();
	g_pSolidSet=NULL;
	if(g_pSolidSnap)
		g_pSolidSnap->Release();
	g_pSolidSnap=NULL;
	if(g_pSolidOper)
		g_pSolidOper->Release();
	g_pSolidOper=NULL;
	if(solid3dClf)
		solid3dClf->LockServer(FALSE);*/
#ifndef AFX_TARG_ENU_ENGLISH
	delete g_pBoltInfoDlg;
#endif
	delete g_pPromptMsg;
#ifndef __TSA_
	delete g_pLsRefDlg;
#endif
#if defined(__TMA_)||defined(__LMA_)
	delete g_pDataCompareDlg;
#endif
	delete g_pSearchPartDlg;
	delete pFuncStatus;
	/*	delete m_pInputLsDlg;
	delete m_pInputPlankDlg;
	delete m_pDivideLongJg;*/
}

BOOL CLDSView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style|=WS_CLIPSIBLINGS|WS_CLIPCHILDREN;
	
	return CView::PreCreateWindow(cs);
}

BOOL CLDSView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	try{
		return CView::OnCommand(wParam, lParam);
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	catch(CXhChar16 sError)
	{
		AfxMessageBox(sError);
	}
	catch(CXhChar50 sError)
	{
		AfxMessageBox(sError);
	}
	catch(CXhChar100 sError)
	{
		AfxMessageBox(sError);
	}
	return FALSE;
}

BOOL CLDSView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	try{
		return CView::OnWndMsg(message, wParam, lParam, pResult);
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	catch(CXhChar16 sError)
	{
		AfxMessageBox(sError);
	}
	catch(CXhChar50 sError)
	{
		AfxMessageBox(sError);
	}
	catch(CXhChar100 sError)
	{
		AfxMessageBox(sError);
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CLDSView drawing
//�����༭���еĽǸ���ʾ
static double CalAngleInArcPart(f3dPoint boltPos,CLDSArcPart* pArcPart)
{
	if(pArcPart==NULL)
		return 0;
	UCS_STRU ucs;
	pArcPart->getUCS(ucs);
	f3dPoint inters,vec;
	Int3dlf(inters,boltPos,ucs.axis_z,ucs.origin,ucs.axis_z);
	vec=inters-ucs.origin;
	normalize(vec);
	double angle=CalRotateAngle(ucs.axis_z,ucs.axis_x,vec);
	//����Բ������������ʱ�Ѿ�����������ͷӰ�죬�˴�������˨�Ƕ�ʱ�����ٴο�������ͷӰ�� wht 15-03-20
	//angle+=pArcPart->start_oddment/pArcPart->baseline.Radius(); 
	if(angle<0)
		angle+=2*Pi;
	return angle;
}
void DrawRollEdge(PROFILE_VERTEX* pPrevPnt,PROFILE_VERTEX* pRollLineStart,PROFILE_VERTEX* pRollLineEnd,
	              PROFILE_VERTEX* pNextVertex,CLDSGeneralPlate* pPlate)
{
	if(pPrevPnt==NULL||pRollLineStart==NULL||pRollLineEnd==NULL||pNextVertex==NULL||pPlate==NULL)
		return;
	f3dLine huoqu_line,roll_edge_line;
	BOOL bRetainRollLineStart=FALSE,bRetainRollLineEnd=FALSE;
	pPlate->CalRollLineAndHuoquLine(pRollLineStart,pRollLineEnd,pPrevPnt,pNextVertex,huoqu_line,roll_edge_line,
									&bRetainRollLineStart,&bRetainRollLineEnd);
	//����
	f3dLine line;
	if(pRollLineStart->vertex.feature>10)
	{	//���о�ߵ���ʼ��Ϊ������ʱ��������ȥ
		line.startPt=pPrevPnt->vertex;
		line.endPt=pRollLineStart->vertex;
		g_pSolidDraw->NewLine(line);
		//
		line.startPt=pRollLineStart->vertex;
		line.endPt=huoqu_line.startPt;
		g_pSolidDraw->NewLine(line);
	}
	else
	{
		line.startPt=pPrevPnt->vertex;
		if(bRetainRollLineStart)
		{
			line.endPt=pRollLineStart->vertex;
			g_pSolidDraw->NewLine(line);
			g_pSolidDraw->NewPoint(pRollLineStart->vertex);
			line.startPt=pRollLineStart->vertex;
		}
		line.endPt=huoqu_line.startPt;
		g_pSolidDraw->NewLine(line);
	}
	if(pRollLineEnd->vertex.feature>10)
	{	//���о�ߵ���ֹ��Ϊ������ʱ��������ȥ
		line.startPt=huoqu_line.endPt;
		line.endPt=pRollLineEnd->vertex;
		g_pSolidDraw->NewLine(line);
		//
		line.startPt=pRollLineEnd->vertex;
		line.endPt=pNextVertex->vertex;
		g_pSolidDraw->NewLine(line);
	}
	else
	{
		line.startPt=huoqu_line.endPt;
		if(bRetainRollLineEnd)
		{
			line.endPt=pRollLineEnd->vertex;
			g_pSolidDraw->NewLine(line);
			g_pSolidDraw->NewPoint(pRollLineEnd->vertex);
			line.startPt=pRollLineEnd->vertex;
		}
		line.endPt=pNextVertex->vertex;
		g_pSolidDraw->NewLine(line);
	}
	//
	line.startPt=huoqu_line.startPt;
	line.endPt=roll_edge_line.startPt;
	g_pSolidDraw->NewLine(line);
	line.startPt=roll_edge_line.startPt;
	line.endPt=roll_edge_line.endPt;
	g_pSolidDraw->NewLine(line);
	line.startPt=roll_edge_line.endPt;
	line.endPt=huoqu_line.endPt;
	g_pSolidDraw->NewLine(line);
	//
	roll_edge_line.startPt.feature=roll_edge_line.endPt.feature=
		huoqu_line.startPt.feature=huoqu_line.endPt.feature=(long)pRollLineStart;	//���������feature��¼���������ָ�� wht 16-06-06
	g_pSolidDraw->NewPoint(roll_edge_line.startPt);
	g_pSolidDraw->NewPoint(roll_edge_line.endPt);
	g_pSolidDraw->NewPoint(huoqu_line.startPt);
	g_pSolidDraw->NewPoint(huoqu_line.endPt);
	//���ƻ��������ߺ;�߹��������� wxc-2017.3-30
	if(pRollLineStart->vertex.IsEqual(huoqu_line.startPt)&&pRollLineEnd->vertex.IsEqual(huoqu_line.endPt))
	{	//���������о�߹�����Ϣ���������غϣ�ֻ����ƻ����߼���
		line.pen.style=PS_DASHDOTDOT;
		line.pen.crColor=RGB(255,0,0);
		line.startPt=pRollLineStart->vertex;
		line.endPt=pRollLineEnd->vertex;
		g_pSolidDraw->NewLine(line);
	}
	else
	{	//���������о�߹�����Ϣ�������߲��غϣ���ֱ�����������ߺ�������
		line.pen.style=PS_DASHDOT;
		line.startPt=pRollLineStart->vertex;
		line.endPt=pRollLineEnd->vertex;
		g_pSolidDraw->NewLine(line);
		//����������
		line.pen.style=PS_DASHDOTDOT;
		line.pen.crColor=RGB(255,0,0);
		line.startPt=huoqu_line.startPt;
		line.endPt=huoqu_line.endPt;
		line.feature=(long)pRollLineStart;	//��߻�����feature��¼��Ӧ��������ָ�� wht 16-06-06
		g_pSolidDraw->NewLine(line);
		g_pSolidDraw->NewPoint(line.startPt);
		g_pSolidDraw->NewPoint(line.endPt);
	}
}
void ConvertPlateRollEdgeToHuoQu(CLDSPlate *pPlate)
{
	ATOM_LIST<PROFILE_VERTEX> vertexList;
	for(PROFILE_VERTEX *pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		vertexList.append(*pVertex);
	pPlate->vertex_list.Empty();
	f3dLine huoqu_line,roll_edge_line;
	PROFILE_VERTEX* pTailVertex=vertexList.GetTail(),*pPrevVertex=pTailVertex;
	for(PROFILE_VERTEX *pVertex=vertexList.GetFirst();pVertex;pVertex=vertexList.GetNext())
	{
		if(pPlate->face_N!=1&&pPlate->face_N!=2)
			pPlate->vertex_list.append(*pVertex);
		else
		{
			pVertex->vertex.feature=1;
			if (pVertex->m_bRollEdge)
			{
				PROFILE_VERTEX* pRollLineStart=pVertex,*pRollLineEnd=NULL,*pNextVertex=NULL;
				int nPush=vertexList.push_stack();
				pRollLineEnd=(pTailVertex==pVertex)?vertexList.GetFirst():vertexList.GetNext();
				pNextVertex =(pTailVertex==pRollLineEnd)?vertexList.GetFirst(): vertexList.GetNext();
				vertexList.pop_stack(nPush);
				pPlate->CalRollLineAndHuoquLine(pRollLineStart,pRollLineEnd,pPrevVertex,pNextVertex,huoqu_line,roll_edge_line);
				//������������
				int j=pPlate->face_N-1;
				pPlate->huoqufacenorm[j]=huoqu_line.endPt-huoqu_line.startPt;
				if(pVertex->manu_space>0)
					pPlate->huoqufacenorm[j].Set(-pPlate->huoqufacenorm[j].y,pPlate->huoqufacenorm[j].x,0);
				else
					pPlate->huoqufacenorm[j].Set(pPlate->huoqufacenorm[j].y,-pPlate->huoqufacenorm[j].x,0);
				normalize(pPlate->huoqufacenorm[j]);
				vector_trans(pPlate->huoqufacenorm[j],pPlate->ucs,TRUE);
				pPlate->designInfo.huoqufacenorm[j].norm_style=0;
				pPlate->designInfo.huoqufacenorm[j].vector=pPlate->huoqufacenorm[j];
				//����������
				pPlate->HuoQuLine[j].startPt=huoqu_line.startPt;
				pPlate->HuoQuLine[j].endPt=huoqu_line.endPt;
				pPlate->designInfo.huoquline_start[j].datum_pos_style=0;
				pPlate->designInfo.huoquline_start[j].SetPosition(huoqu_line.startPt);
				pPlate->designInfo.huoquline_end[j].datum_pos_style=0;
				pPlate->designInfo.huoquline_end[j].SetPosition(huoqu_line.endPt);
				//
				huoqu_line.startPt.feature=huoqu_line.endPt.feature=10+pPlate->face_N+1;
				roll_edge_line.startPt.feature=roll_edge_line.endPt.feature=pPlate->face_N+1;
				//
				pPlate->vertex_list.append(huoqu_line.startPt);
				pPlate->vertex_list.append(roll_edge_line.startPt);
				pPlate->vertex_list.append(roll_edge_line.endPt);
				pPlate->vertex_list.append(huoqu_line.endPt);
				//
				pPlate->face_N++;
				if(pPlate->face_N==3)
				{
					if(pPrevVertex->m_bRollEdge)//������������߽���
						pPlate->top_point.Set();
					else
						pPlate->jdb_style=1;	//�۵���
				}
			}
			else if(!pPrevVertex->m_bRollEdge)
				pPlate->vertex_list.append(*pVertex);
		}
		pPrevVertex=pVertex;
	}
}
void DrawSolidPart(void* pContext)
{
#ifdef __ANGLE_PART_INC_
	try{
		CMainFrame *pFrame=(CMainFrame*)AfxGetMainWnd();
		if(console.m_pWorkPart==NULL)
			return;	//û�е�ǰ�������
		else
			console.m_pWorkPart=console.m_pWorkPart->MotherPart();
		IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
		pBuddyDrawing->EmptyDbEntities();
		DISPLAY_TYPE disp;
		g_pSolidSet->GetDisplayType(&disp);
		Ta.GeEnts.Empty();
		if(disp!=DISP_LINE)//DISP_SOLID)
		{
			//ʵ����ʾ����
			g_pSolidSet->SetObjectUcs(console.m_curACS);
			if(console.m_pWorkPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)console.m_pWorkPart;
				for(int i=0;i<4;i++)
				{
					CLDSLineAngle *pCommJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
					if(pCommJg&&!pCommJg->m_bVirtualPart)
					{	//���ڶ�Ӧ���ӽǸ����ӽǸֲ�Ϊ���⹹�� wht 11-07-25
						bool bDispHole=pCommJg->m_bDisplayHole;
						pCommJg->SetModified();
						pCommJg->m_bDisplayHole=true;
						pCommJg->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
						pCommJg->m_bDisplayHole=bDispHole;
						CSolidPart solidPart=pCommJg->GetSolidPartObject();
						solidPart.m_bDisplaySingle=FALSE;
						g_pSolidDraw->NewSolidPart(solidPart);
					}
				}
			}
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_LINEANGLE) //
			{
				CLDSLineAngle *pCommJg=(CLDSLineAngle*)console.m_pWorkPart;
				bool bDispLsHole=pCommJg->m_bDisplayHole;
				pCommJg->m_bDisplayHole = true;
				pCommJg->SetModified();
				pCommJg->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				pCommJg->m_bDisplayHole = bDispLsHole;
				CSolidPart solidPart=pCommJg->GetSolidPartObject();
				solidPart.m_bDisplaySingle=FALSE;
				g_pSolidDraw->NewSolidPart(solidPart);
			}
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_LINESLOT)
			{
				CLDSLineSlot *pLineSlot=(CLDSLineSlot*)console.m_pWorkPart;
				bool bDispLsHole=pLineSlot->m_bDisplayHole;
				pLineSlot->m_bDisplayHole = true;
				pLineSlot->SetModified();
				pLineSlot->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				pLineSlot->m_bDisplayHole = bDispLsHole;
				g_pSolidDraw->NewSolidPart(pLineSlot->GetSolidPartObject());
			}
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_LINEFLAT)
			{
				CLDSLineFlat *pLineFlat=(CLDSLineFlat*)console.m_pWorkPart;
				bool bDispLsHole=pLineFlat->m_bDisplayHole;
				pLineFlat->m_bDisplayHole = true;
				pLineFlat->SetModified();
				pLineFlat->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				pLineFlat->m_bDisplayHole = bDispLsHole;
				g_pSolidDraw->NewSolidPart(pLineFlat->GetSolidPartObject());
			}
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE)
			{
				CLDSPlate *pPlank=(CLDSPlate*)console.m_pWorkPart;
				if(pPlank->vertex_list.GetNodeNum()<3)
					return;
				bool bDispLsHole = pPlank->m_bDisplayHole;
				pPlank->m_bDisplayHole = true;
				pPlank->SetModified();
				pPlank->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				pPlank->m_bDisplayHole = bDispLsHole;
				g_pSolidDraw->NewSolidPart(pPlank->GetSolidPartObject());
			}
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				CLDSParamPlate *pParamPlate=(CLDSParamPlate*)console.m_pWorkPart;
				if(pParamPlate->vertex_list.GetNodeNum()<3)
					return;
				bool bDispLsHole = pParamPlate->m_bDisplayHole;
				pParamPlate->m_bDisplayHole = true;
				pParamPlate->SetModified();
				pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				pParamPlate->m_bDisplayHole = bDispLsHole;
				g_pSolidDraw->NewSolidPart(pParamPlate->GetSolidPartObject());
			}
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_SPHERE)
			{
				CLDSSphere *pSphere=(CLDSSphere*)console.m_pWorkPart;
				bool bDispLsHole = pSphere->m_bDisplayHole;
				pSphere->m_bDisplayHole = true;
				pSphere->SetModified();
				pSphere->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pSphere->GetSolidPartObject());
				pSphere->m_bDisplayHole = bDispLsHole;
			}
			else if(console.m_pWorkPart->IsArcPart())
			{
				CLDSArcPart *pArcPart=(CLDSArcPart*)console.m_pWorkPart;
				bool bDispLsHole = pArcPart->m_bDisplayHole;
				pArcPart->m_bDisplayHole = true;
				pArcPart->SetModified();
				pArcPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pArcPart->GetSolidPartObject());
				pArcPart->m_bDisplayHole = bDispLsHole;
			}
		}
		else if(disp==DISP_LINE)
		{	//����ͼ��ʾ����
			f3dPoint point;
			f3dLine line;
			f3dCircle circle;
			f3dPoint f_start,f_end;
			UCS_STRU std_ucs;
			LoadDefaultUCS(&std_ucs);
			g_pSolidSet->SetObjectUcs(std_ucs);
			g_pSolidSet->SetPen(0,PS_SOLID,1);
			if(console.m_pWorkPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSLineAngle *pCommonJg;
				CLDSGroupLineAngle *pGroupJg;
				pGroupJg=(CLDSGroupLineAngle*)console.m_pWorkPart;
				f3dPoint norm=pGroupJg->get_norm_y_wing();
				for(int i=0;i<4;i++)
				{
					int x_wing0_y_wing1;
					BOOL bInsideJg;
					pCommonJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
					if(pCommonJg==NULL||pCommonJg->m_bVirtualPart)
						continue;	//�����ڶ�Ӧ���ӽǸֻ��ӽǸ�Ϊ���⹹�� wht 11-07-25
					bInsideJg=IsInsideJg(pCommonJg,norm,&x_wing0_y_wing1);
					double len=DISTANCE(pCommonJg->Start(),pCommonJg->End());
					double wing_wide,wing_thick;
					wing_wide=pCommonJg->GetWidth();
					wing_thick=pCommonJg->GetThick();
					f3dRect rect;
					rect.pen.style=PS_SOLID;
					f3dLine line;
					BOOL bUpperJg;
					if(bInsideJg)	//����
					{
						if(x_wing0_y_wing1==0)	//X֫Ϊ��ǰ֫	:�²�Ǹ�
						{
							bUpperJg=FALSE;
							rect.topLeft.Set(-pCommonJg->startOdd(),-pGroupJg->jg_space/2,-pGroupJg->jg_space/2);
							rect.bottomRight.Set(len+pCommonJg->endOdd(),-wing_wide-pGroupJg->jg_space/2,-pGroupJg->jg_space/2);
							rect.ID=pCommonJg->handle;
							if(pGroupJg->group_style==1)	//T����ϽǸ�
								rect.topLeft.z=rect.bottomRight.z=0;
							g_pSolidDraw->NewRect(rect);
							line.pen.style=PS_DASH;
							line.startPt.Set(-pCommonJg->startOdd(),-wing_thick-pGroupJg->jg_space/2,-pGroupJg->jg_space/2);
							line.endPt.Set(len+pCommonJg->endOdd(),-wing_thick-pGroupJg->jg_space/2,-pGroupJg->jg_space/2);
							line.ID=pCommonJg->handle;
							if(pGroupJg->group_style==1)
								line.startPt.z=line.endPt.z=0;
							g_pSolidDraw->NewLine(line);
						}
						else					//Y֫Ϊ��ǰ֫	:�ϲ�Ǹ�
						{
							bUpperJg=TRUE;
							rect.topLeft.Set(-pCommonJg->startOdd(),pGroupJg->jg_space/2,-pGroupJg->jg_space/2);
							rect.bottomRight.Set(len+pCommonJg->endOdd(),wing_wide+pGroupJg->jg_space/2,-pGroupJg->jg_space/2);
							rect.ID=pCommonJg->handle;
							if(pGroupJg->group_style==1)	//T����ϽǸ�
								rect.topLeft.z=rect.bottomRight.z=0;
							g_pSolidDraw->NewRect(rect);
							line.pen.style=PS_DASH;
							line.startPt.Set(-pCommonJg->startOdd(),wing_thick+pGroupJg->jg_space/2,-pGroupJg->jg_space/2);
							line.endPt.Set(len+pCommonJg->endOdd(),wing_thick+pGroupJg->jg_space/2,-pGroupJg->jg_space/2);
							line.ID=pCommonJg->handle;
							if(pGroupJg->group_style==1)	//T����ϽǸ�
								line.startPt.z=line.endPt.z=0;
							g_pSolidDraw->NewLine(line);
						}
					}
					else					//����
					{
						if(x_wing0_y_wing1==0)	//X֫Ϊ��ǰ֫	:�ϲ�Ǹ�
						{
							bUpperJg=TRUE;
							rect.topLeft.Set(-pCommonJg->startOdd(),pGroupJg->jg_space/2,pGroupJg->jg_space/2);
							rect.bottomRight.Set(len+pCommonJg->endOdd(),wing_wide+pGroupJg->jg_space/2,pGroupJg->jg_space/2);
							rect.ID=pCommonJg->handle;
							if(pGroupJg->group_style==1)	//T����ϽǸ�
								rect.topLeft.z=rect.bottomRight.z=0;
							g_pSolidDraw->NewRect(rect);
							line.pen.style=PS_SOLID;
							line.startPt.Set(-pCommonJg->startOdd(),wing_thick+pGroupJg->jg_space/2,pGroupJg->jg_space/2);
							line.endPt.Set(len+pCommonJg->endOdd(),wing_thick+pGroupJg->jg_space/2,pGroupJg->jg_space/2);
							line.ID=pCommonJg->handle;
							if(pGroupJg->group_style==1)	//T����ϽǸ�
								line.startPt.z=line.endPt.z=0;
							g_pSolidDraw->NewLine(line);
						}
						else					//Y֫Ϊ��ǰ֫	:�²�Ǹ�
						{
							bUpperJg=FALSE;
							rect.topLeft.Set(-pCommonJg->startOdd(),-pGroupJg->jg_space/2,pGroupJg->jg_space/2);
							rect.bottomRight.Set(len+pCommonJg->endOdd(),-wing_wide-pGroupJg->jg_space/2,pGroupJg->jg_space/2);
							rect.ID=pCommonJg->handle;
							if(pGroupJg->group_style==1)	//T����ϽǸ�
								rect.topLeft.z=rect.bottomRight.z=0;
							g_pSolidDraw->NewRect(rect);
							line.pen.style=PS_SOLID;
							line.startPt.Set(-pCommonJg->startOdd(),-wing_thick-pGroupJg->jg_space/2,pGroupJg->jg_space/2);
							line.endPt.Set(len+pCommonJg->endOdd(),-wing_thick-pGroupJg->jg_space/2,pGroupJg->jg_space/2);
							line.ID=pCommonJg->handle;
							if(pGroupJg->group_style==1)	//T����ϽǸ�
								line.startPt.z=line.endPt.z=0;
							g_pSolidDraw->NewLine(line);
						}
					}
					//��˨��
					UCS_STRU ucs;
					pCommonJg->getUCS(ucs);
					for(CLsRef *pLsRef=pCommonJg->GetFirstLsRef();pLsRef;pLsRef=pCommonJg->GetNextLsRef())
					{
						circle.norm = (*pLsRef)->get_norm();
						circle.centre = (*pLsRef)->ucs.origin;
						if(fabs(pCommonJg->get_norm_y_wing()*circle.norm )<fabs(pCommonJg->get_norm_x_wing()*circle.norm ))
							Int3dlf(circle.centre,circle.centre,circle.norm,ucs.origin,pCommonJg->get_norm_x_wing());
						else
							Int3dlf(circle.centre,circle.centre,circle.norm,ucs.origin,pCommonJg->get_norm_y_wing());
						circle.radius = ((*pLsRef)->get_d()+(*pLsRef)->hole_d_increment)/2;
						circle.ID	  = (*pLsRef)->handle;
						coord_trans(circle.centre,ucs,FALSE);
						double g = pCommonJg->GetLsG(pLsRef);
						if(fabs(circle.norm*pCommonJg->get_norm_x_wing())>
							fabs(circle.norm*pCommonJg->get_norm_y_wing())&&x_wing0_y_wing1==0)
						{	//X֫�ϵ���˨
							if(bUpperJg)
								circle.centre.Set(circle.centre.z,g+pGroupJg->jg_space*0.5,0);
							else
								circle.centre.Set(circle.centre.z,-g-pGroupJg->jg_space*0.5,0);
							circle.norm.Set(0,0,1);
							if((*pLsRef)->IsFootNail())
								circle.pen.width = 2;	//�Ŷ�
							else
								circle.pen.width = 1;
							g_pSolidDraw->NewCircle(circle);
						}
						else if(fabs(circle.norm*pCommonJg->get_norm_x_wing())<
							fabs(circle.norm*pCommonJg->get_norm_y_wing())&&x_wing0_y_wing1==1)
						{	//Y֫�ϵ���˨
							if(bUpperJg)
								circle.centre.Set(circle.centre.z,g+pGroupJg->jg_space*0.5,0);
							else
								circle.centre.Set(circle.centre.z,-g-pGroupJg->jg_space*0.5,0);
							circle.norm.Set(0,0,1);
							if((*pLsRef)->IsFootNail())
								circle.pen.width = 2;	//�Ŷ�
							else
								circle.pen.width = 1;
							g_pSolidDraw->NewCircle(circle);
						}
					}
				}
			}
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				CLDSLineAngle *pCommonJg=(CLDSLineAngle*)console.m_pWorkPart;
				double len=DISTANCE(pCommonJg->Start(),pCommonJg->End());
				g_pSolidDraw->NewPoint(-pCommonJg->startOdd(),0,0);
				g_pSolidDraw->NewPoint(-pCommonJg->startOdd(),pCommonJg->GetThick(),0);
				g_pSolidDraw->NewPoint(-pCommonJg->startOdd(),pCommonJg->GetWidthWingY(),0);
				g_pSolidDraw->NewPoint(-pCommonJg->startOdd(),-pCommonJg->GetThick(),0);
				g_pSolidDraw->NewPoint(-pCommonJg->startOdd(),-pCommonJg->GetWidth(),0);
				
				g_pSolidDraw->NewPoint(len+pCommonJg->endOdd(),0,0);
				g_pSolidDraw->NewPoint(len+pCommonJg->endOdd(),pCommonJg->GetThick(),0);
				g_pSolidDraw->NewPoint(len+pCommonJg->endOdd(),pCommonJg->GetWidthWingY(),0);
				g_pSolidDraw->NewPoint(len+pCommonJg->endOdd(),-pCommonJg->GetThick(),0);
				g_pSolidDraw->NewPoint(len+pCommonJg->endOdd(),-pCommonJg->GetWidth(),0);
				//������
				line.pen.style=PS_SOLID;
				line.startPt.Set(-pCommonJg->startOdd(),-pCommonJg->GetWidth());
				line.endPt.Set(-pCommonJg->startOdd(),pCommonJg->GetWidthWingY());
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pCommonJg->startOdd(),-pCommonJg->GetWidth());
				line.endPt.Set(len+pCommonJg->endOdd(),-pCommonJg->GetWidth());
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pCommonJg->startOdd(),pCommonJg->GetWidthWingY());
				line.endPt.Set(len+pCommonJg->endOdd(),pCommonJg->GetWidthWingY());
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(len+pCommonJg->endOdd(),-pCommonJg->GetWidth());
				line.endPt.Set(len+pCommonJg->endOdd(),pCommonJg->GetWidthWingY());
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pCommonJg->startOdd(),-pCommonJg->GetThick());
				line.endPt.Set(len+pCommonJg->endOdd(),-pCommonJg->GetThick());
				line.pen.style=PS_DASH;
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pCommonJg->startOdd(),pCommonJg->GetThick());
				line.endPt.Set(len+pCommonJg->endOdd(),pCommonJg->GetThick());
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pCommonJg->startOdd(),0);
				line.endPt.Set(len+pCommonJg->endOdd(),0);
				line.pen.style=PS_SOLID;
				line.ID=pCommonJg->handle;	//�Ǹ����߹����Ǹ־��
				g_pSolidDraw->NewLine(line);
				line.ID=0;
				//��˨��
				UCS_STRU ucs;
				pCommonJg->getUCS(ucs);
				for(CLsRef *pLsRef=pCommonJg->GetFirstLsRef();pLsRef;pLsRef=pCommonJg->GetNextLsRef())
				{
					circle.norm = (*pLsRef)->get_norm();
					circle.centre = (*pLsRef)->ucs.origin;
					if(fabs(pCommonJg->get_norm_y_wing()*circle.norm )<fabs(pCommonJg->get_norm_x_wing()*circle.norm ))
						Int3dlf(circle.centre,circle.centre,circle.norm,ucs.origin,ucs.axis_y);
					else
						Int3dlf(circle.centre,circle.centre,circle.norm,ucs.origin,ucs.axis_x);
					circle.radius = ((*pLsRef)->get_d()+(*pLsRef)->hole_d_increment)/2;
					circle.ID	  = (*pLsRef)->handle;
					coord_trans(circle.centre,ucs,FALSE);
					double g = pCommonJg->GetLsG(pLsRef);

					if(fabs(circle.norm*pCommonJg->get_norm_x_wing())>
						fabs(circle.norm*pCommonJg->get_norm_y_wing()))
					{	//X֫�ϵ���˨
						circle.centre.Set(circle.centre.z,-g,0);
					}
					else
					{	//Y֫�ϵ���˨
						circle.centre.Set(circle.centre.z, g,0);
					}
					//�Բ���Ҫ���п��ϽǵĶ̽Ǹֽ�����˨λ������   wxc-2016.10.28
					if((pCommonJg->pStart==NULL || pCommonJg->pEnd==NULL)&&pCommonJg->GetKaiHeWingX0Y1()<0)
						pCommonJg->EmendShortJgLsPosY(pLsRef->GetLsPtr(),circle.centre.y);
					circle.norm.Set(0,0,1);
					if((*pLsRef)->IsFootNail())
						circle.pen.width = 2;	//�Ŷ�
					else
						circle.pen.width = 1;
					g_pSolidDraw->NewCircle(circle);
				}
			}
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_LINESLOT)
			{
				CLDSLineSlot *pLineSlot=(CLDSLineSlot*)console.m_pWorkPart;
				double len=DISTANCE(pLineSlot->Start(),pLineSlot->End());
				g_pSolidDraw->NewPoint(-pLineSlot->startOdd(),pLineSlot->GetHeight()*0.5+pLineSlot->GetWidth(),0);
				g_pSolidDraw->NewPoint(-pLineSlot->startOdd(),pLineSlot->GetHeight()*0.5+pLineSlot->GetThick(),0);
				g_pSolidDraw->NewPoint(-pLineSlot->startOdd(),pLineSlot->GetHeight()*0.5,0);
				g_pSolidDraw->NewPoint(-pLineSlot->startOdd(),pLineSlot->GetHeight()*0.5-pLineSlot->GetThick(),0);
				g_pSolidDraw->NewPoint(-pLineSlot->startOdd(),-pLineSlot->GetHeight()*0.5+pLineSlot->GetThick(),0);
				g_pSolidDraw->NewPoint(-pLineSlot->startOdd(),-pLineSlot->GetHeight()*0.5,0);
				g_pSolidDraw->NewPoint(-pLineSlot->startOdd(),-pLineSlot->GetHeight()*0.5-pLineSlot->GetThick(),0);
				g_pSolidDraw->NewPoint(-pLineSlot->startOdd(),-pLineSlot->GetHeight()*0.5-pLineSlot->GetWidth(),0);
				
				g_pSolidDraw->NewPoint(len+pLineSlot->endOdd(),pLineSlot->GetHeight()*0.5+pLineSlot->GetWidth(),0);
				g_pSolidDraw->NewPoint(len+pLineSlot->endOdd(),pLineSlot->GetHeight()*0.5+pLineSlot->GetThick(),0);
				g_pSolidDraw->NewPoint(len+pLineSlot->endOdd(),pLineSlot->GetHeight()*0.5,0);
				g_pSolidDraw->NewPoint(len+pLineSlot->endOdd(),pLineSlot->GetHeight()*0.5-pLineSlot->GetThick(),0);
				g_pSolidDraw->NewPoint(len+pLineSlot->endOdd(),-pLineSlot->GetHeight()*0.5+pLineSlot->GetThick(),0);
				g_pSolidDraw->NewPoint(len+pLineSlot->endOdd(),-pLineSlot->GetHeight()*0.5,0);
				g_pSolidDraw->NewPoint(len+pLineSlot->endOdd(),-pLineSlot->GetHeight()*0.5-pLineSlot->GetThick(),0);
				g_pSolidDraw->NewPoint(len+pLineSlot->endOdd(),-pLineSlot->GetHeight()*0.5-pLineSlot->GetWidth(),0);
				//������
				line.pen.style=PS_SOLID;
				line.startPt.Set(-pLineSlot->startOdd(),pLineSlot->GetHeight()*0.5+pLineSlot->GetWidth(),0);
				line.endPt.Set(-pLineSlot->startOdd(),-pLineSlot->GetHeight()*0.5-pLineSlot->GetWidth(),0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(len+pLineSlot->endOdd(),pLineSlot->GetHeight()*0.5+pLineSlot->GetWidth(),0);
				line.endPt.Set(len+pLineSlot->endOdd(),-pLineSlot->GetHeight()*0.5-pLineSlot->GetWidth(),0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pLineSlot->startOdd(),pLineSlot->GetHeight()*0.5+pLineSlot->GetWidth(),0);
				line.endPt.Set(len+pLineSlot->endOdd(),pLineSlot->GetHeight()*0.5+pLineSlot->GetWidth(),0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pLineSlot->startOdd(),pLineSlot->GetHeight()*0.5,0);
				line.endPt.Set(len+pLineSlot->endOdd(),pLineSlot->GetHeight()*0.5,0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pLineSlot->startOdd(),-pLineSlot->GetHeight()*0.5,0);
				line.endPt.Set(len+pLineSlot->endOdd(),-pLineSlot->GetHeight()*0.5,0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pLineSlot->startOdd(),-pLineSlot->GetHeight()*0.5-pLineSlot->GetWidth(),0);
				line.endPt.Set(len+pLineSlot->endOdd(),-pLineSlot->GetHeight()*0.5-pLineSlot->GetWidth(),0);
				g_pSolidDraw->NewLine(line);
				line.pen.style=PS_DASH;
				line.startPt.Set(-pLineSlot->startOdd(),pLineSlot->GetHeight()*0.5+pLineSlot->GetThick(),0);
				line.endPt.Set(len+pLineSlot->endOdd(),pLineSlot->GetHeight()*0.5+pLineSlot->GetThick(),0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pLineSlot->startOdd(),pLineSlot->GetHeight()*0.5-pLineSlot->GetThick(),0);
				line.endPt.Set(len+pLineSlot->endOdd(),pLineSlot->GetHeight()*0.5-pLineSlot->GetThick(),0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pLineSlot->startOdd(),-pLineSlot->GetHeight()*0.5+pLineSlot->GetThick(),0);
				line.endPt.Set(len+pLineSlot->endOdd(),-pLineSlot->GetHeight()*0.5+pLineSlot->GetThick(),0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pLineSlot->startOdd(),-pLineSlot->GetHeight()*0.5-pLineSlot->GetThick(),0);
				line.endPt.Set(len+pLineSlot->endOdd(),-pLineSlot->GetHeight()*0.5-pLineSlot->GetThick(),0);
				g_pSolidDraw->NewLine(line);
				//��˨��
				UCS_STRU ucs;
				ucs=pLineSlot->BuildUCS();
				for(CLsRef *pLsRef=pLineSlot->GetFirstLsRef();pLsRef;pLsRef=pLineSlot->GetNextLsRef())
				{
					f3dPoint ls_pos=pLsRef->GetLsPtr()->ucs.origin;
					coord_trans(ls_pos,ucs,FALSE);
					circle.norm.Set(0,0,1);
					circle.centre.Set(ls_pos.z,ls_pos.x,0);
					circle.radius = ((*pLsRef)->get_d()+(*pLsRef)->hole_d_increment)/2;
					circle.ID	  = (*pLsRef)->handle;
					if((*pLsRef)->IsFootNail())
						circle.pen.width = 2;	//�Ŷ�
					else
						circle.pen.width = 1;
					g_pSolidDraw->NewCircle(circle);
				}
			}
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_LINEFLAT)
			{	//���
				CLDSLineFlat *pLineFlat=(CLDSLineFlat*)console.m_pWorkPart;
				double len=DISTANCE(pLineFlat->Start(),pLineFlat->End());
				g_pSolidDraw->NewPoint(-pLineFlat->startOdd(),pLineFlat->GetWidth()*0.5,0);
				g_pSolidDraw->NewPoint(-pLineFlat->startOdd(),-pLineFlat->GetWidth()*0.5,0);
				g_pSolidDraw->NewPoint(len+pLineFlat->endOdd(),pLineFlat->GetWidth()*0.5,0);
				g_pSolidDraw->NewPoint(len+pLineFlat->endOdd(),-pLineFlat->GetWidth()*0.5,0);
				//������
				line.pen.style=PS_SOLID;
				line.startPt.Set(-pLineFlat->startOdd(),pLineFlat->GetWidth()*0.5,0);
				line.endPt.Set(-pLineFlat->startOdd(),-pLineFlat->GetWidth()*0.5,0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(len+pLineFlat->endOdd(),pLineFlat->GetWidth()*0.5,0);
				line.endPt.Set(len+pLineFlat->endOdd(),-pLineFlat->GetWidth()*0.5,0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pLineFlat->startOdd(),pLineFlat->GetWidth()*0.5,0);
				line.endPt.Set(len+pLineFlat->endOdd(),pLineFlat->GetWidth()*0.5,0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pLineFlat->startOdd(),-pLineFlat->GetWidth()*0.5,0);
				line.endPt.Set(len+pLineFlat->endOdd(),-pLineFlat->GetWidth()*0.5,0);
				g_pSolidDraw->NewLine(line);
				//��˨��
				UCS_STRU ucs;
				ucs=pLineFlat->BuildUCS();
				for(CLsRef *pLsRef=pLineFlat->GetFirstLsRef();pLsRef;pLsRef=pLineFlat->GetNextLsRef())
				{
					f3dPoint ls_pos=pLsRef->GetLsPtr()->ucs.origin;
					coord_trans(ls_pos,ucs,FALSE);
					circle.norm.Set(0,0,1);
					circle.centre.Set(ls_pos.x,ls_pos.y,0);
					circle.radius = ((*pLsRef)->get_d()+(*pLsRef)->hole_d_increment)/2;
					circle.ID	  = (*pLsRef)->handle;
					if((*pLsRef)->IsFootNail())
						circle.pen.width = 2;	//�Ŷ�
					else
						circle.pen.width = 1;
					g_pSolidDraw->NewCircle(circle);
				}
			}
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_ARCFLAT)
			{	//���α��
				CLDSArcFlat *pArcFlat=(CLDSArcFlat*)console.m_pWorkPart;
				double len=pArcFlat->baseline.Radius()*pArcFlat->baseline.SectorAngle();
				g_pSolidDraw->NewPoint(-pArcFlat->start_oddment,pArcFlat->wide*0.5,0);
				g_pSolidDraw->NewPoint(-pArcFlat->start_oddment,-pArcFlat->wide*0.5,0);
				g_pSolidDraw->NewPoint(len+pArcFlat->end_oddment,pArcFlat->wide*0.5,0);
				g_pSolidDraw->NewPoint(len+pArcFlat->end_oddment,-pArcFlat->wide*0.5,0);
				//������
				line.pen.style=PS_SOLID;
				line.startPt.Set(-pArcFlat->start_oddment,pArcFlat->wide*0.5,0);
				line.endPt.Set(-pArcFlat->start_oddment,-pArcFlat->wide*0.5,0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(len+pArcFlat->end_oddment,pArcFlat->wide*0.5,0);
				line.endPt.Set(len+pArcFlat->end_oddment,-pArcFlat->wide*0.5,0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pArcFlat->start_oddment,pArcFlat->wide*0.5,0);
				line.endPt.Set(len+pArcFlat->end_oddment,pArcFlat->wide*0.5,0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pArcFlat->start_oddment,-pArcFlat->wide*0.5,0);
				line.endPt.Set(len+pArcFlat->end_oddment,-pArcFlat->wide*0.5,0);
				g_pSolidDraw->NewLine(line);
				
				//��˨��
				UCS_STRU ucs;
				pArcFlat->getUCS(ucs);
				for(CLsRef *pLsRef=pArcFlat->GetFirstLsRef();pLsRef;pLsRef=pArcFlat->GetNextLsRef())
				{
					CLDSBolt* pBolt=pLsRef->GetLsPtr();
					f3dPoint pt,ls_pos;
					double angle=CalAngleInArcPart(pBolt->ucs.origin,pArcFlat);
					ls_pos.x=pArcFlat->baseline.Radius()*angle;
					pt=pBolt->ucs.origin;
					coord_trans(pt,ucs,FALSE);
					ls_pos.y=pt.z-pArcFlat->offset_v-pArcFlat->wide*0.5;
					//
					circle.norm.Set(0,0,1);
					circle.centre.Set(ls_pos.x,ls_pos.y,0);
					circle.radius = (pBolt->get_d()+pBolt->hole_d_increment)/2;
					circle.ID	  = pBolt->handle;
					if(pBolt->IsFootNail())
						circle.pen.width = 2;	//�Ŷ�
					else
						circle.pen.width = 1;
					g_pSolidDraw->NewCircle(circle);
				}
			}
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_ARCSLOT)
			{	//���β۸�
				CLDSArcSlot *pArcSlot=(CLDSArcSlot*)console.m_pWorkPart;
				double len=pArcSlot->baseline.Radius()*pArcSlot->baseline.SectorAngle();
				g_pSolidDraw->NewPoint(-pArcSlot->start_oddment,pArcSlot->height*0.5+pArcSlot->wide,0);
				g_pSolidDraw->NewPoint(-pArcSlot->start_oddment,pArcSlot->height*0.5+pArcSlot->thick,0);
				g_pSolidDraw->NewPoint(-pArcSlot->start_oddment,pArcSlot->height*0.5,0);
				g_pSolidDraw->NewPoint(-pArcSlot->start_oddment,pArcSlot->height*0.5-pArcSlot->thick,0);
				g_pSolidDraw->NewPoint(-pArcSlot->start_oddment,-pArcSlot->height*0.5+pArcSlot->thick,0);
				g_pSolidDraw->NewPoint(-pArcSlot->start_oddment,-pArcSlot->height*0.5,0);
				g_pSolidDraw->NewPoint(-pArcSlot->start_oddment,-pArcSlot->height*0.5-pArcSlot->thick,0);
				g_pSolidDraw->NewPoint(-pArcSlot->start_oddment,-pArcSlot->height*0.5-pArcSlot->wide,0);
				
				g_pSolidDraw->NewPoint(len+pArcSlot->end_oddment,pArcSlot->height*0.5+pArcSlot->wide,0);
				g_pSolidDraw->NewPoint(len+pArcSlot->end_oddment,pArcSlot->height*0.5+pArcSlot->thick,0);
				g_pSolidDraw->NewPoint(len+pArcSlot->end_oddment,pArcSlot->height*0.5,0);
				g_pSolidDraw->NewPoint(len+pArcSlot->end_oddment,pArcSlot->height*0.5-pArcSlot->thick,0);
				g_pSolidDraw->NewPoint(len+pArcSlot->end_oddment,-pArcSlot->height*0.5+pArcSlot->thick,0);
				g_pSolidDraw->NewPoint(len+pArcSlot->end_oddment,-pArcSlot->height*0.5,0);
				g_pSolidDraw->NewPoint(len+pArcSlot->end_oddment,-pArcSlot->height*0.5-pArcSlot->thick,0);
				g_pSolidDraw->NewPoint(len+pArcSlot->end_oddment,-pArcSlot->height*0.5-pArcSlot->wide,0);
				//������
				line.pen.style=PS_SOLID;
				line.startPt.Set(-pArcSlot->start_oddment,pArcSlot->height*0.5+pArcSlot->wide,0);
				line.endPt.Set(-pArcSlot->start_oddment,-pArcSlot->height*0.5-pArcSlot->wide,0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(len+pArcSlot->end_oddment,pArcSlot->height*0.5+pArcSlot->wide,0);
				line.endPt.Set(len+pArcSlot->end_oddment,-pArcSlot->height*0.5-pArcSlot->wide,0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pArcSlot->start_oddment,pArcSlot->height*0.5+pArcSlot->wide,0);
				line.endPt.Set(len+pArcSlot->end_oddment,pArcSlot->height*0.5+pArcSlot->wide,0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pArcSlot->start_oddment,pArcSlot->height*0.5,0);
				line.endPt.Set(len+pArcSlot->end_oddment,pArcSlot->height*0.5,0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pArcSlot->start_oddment,-pArcSlot->height*0.5,0);
				line.endPt.Set(len+pArcSlot->end_oddment,-pArcSlot->height*0.5,0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pArcSlot->start_oddment,-pArcSlot->height*0.5-pArcSlot->wide,0);
				line.endPt.Set(len+pArcSlot->end_oddment,-pArcSlot->height*0.5-pArcSlot->wide,0);
				g_pSolidDraw->NewLine(line);
				line.pen.style=PS_DASH;
				line.startPt.Set(-pArcSlot->start_oddment,pArcSlot->height*0.5+pArcSlot->thick,0);
				line.endPt.Set(len+pArcSlot->end_oddment,pArcSlot->height*0.5+pArcSlot->thick,0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pArcSlot->start_oddment,pArcSlot->height*0.5-pArcSlot->thick,0);
				line.endPt.Set(len+pArcSlot->end_oddment,pArcSlot->height*0.5-pArcSlot->thick,0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pArcSlot->start_oddment,-pArcSlot->height*0.5+pArcSlot->thick,0);
				line.endPt.Set(len+pArcSlot->end_oddment,-pArcSlot->height*0.5+pArcSlot->thick,0);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pArcSlot->start_oddment,-pArcSlot->height*0.5-pArcSlot->thick,0);
				line.endPt.Set(len+pArcSlot->end_oddment,-pArcSlot->height*0.5-pArcSlot->thick,0);
				g_pSolidDraw->NewLine(line);
				//��˨��
				UCS_STRU ucs;
				pArcSlot->getUCS(ucs);
				for(CLsRef *pLsRef=pArcSlot->GetFirstLsRef();pLsRef;pLsRef=pArcSlot->GetNextLsRef())
				{
					//������˨�ڲ۸��ϵ�λ��
					CLDSBolt* pBolt=pLsRef->GetLsPtr();
					f3dPoint ls_pos;
					double angle=CalAngleInArcPart(pBolt->ucs.origin,pArcSlot);
					ls_pos.x=pArcSlot->baseline.Radius()*angle;
					if(pArcSlot->m_siLayoutStyle==0 || pArcSlot->m_siLayoutStyle==2)
					{
						if(fabs(pBolt->ucs.axis_z*ucs.axis_z)>fabs(pBolt->ucs.axis_z*ucs.axis_x)&&
							fabs(pBolt->ucs.axis_z*ucs.axis_z)>fabs(pBolt->ucs.axis_z*ucs.axis_y))
						{	//��˨λ�ڲ۸ֲ��
							f3dPoint inters,pick,intersUp,intersDown;
							pick=ucs.origin+ucs.axis_z*pArcSlot->offset_v;
							Int3dlf(inters,pBolt->ucs.origin,ucs.axis_z,pick,ucs.axis_z);
							double radius=pArcSlot->baseline.Radius();
							double dist=fabs(DISTANCE(inters,ucs.origin)-radius-pArcSlot->offset_h);
							//�ж���˨����һ��
							if(pArcSlot->m_siLayoutStyle==0)
							{
								intersUp=inters;
								pick=pick-ucs.axis_z*pArcSlot->height;
								Int3dlf(intersDown,pBolt->ucs.origin,ucs.axis_z,pick,ucs.axis_z);
							}
							else
							{
								intersDown=inters;
								pick=pick+ucs.axis_z*pArcSlot->height;
								Int3dlf(intersUp,pBolt->ucs.origin,ucs.axis_z,pick,ucs.axis_z);
							}
							if(DISTANCE(intersUp,pBolt->ucs.origin)<DISTANCE(intersDown,pBolt->ucs.origin))
								ls_pos.y=-pArcSlot->height*0.5-dist;
							else
								ls_pos.y=pArcSlot->height*0.5+dist;
						}
						else	//��˨λ�ڲ۸ֵױ�
						{
							f3dPoint pt=pBolt->ucs.origin;
							coord_trans(pt,ucs,FALSE);
							if(pArcSlot->m_siLayoutStyle==0)
								ls_pos.y=pArcSlot->height*0.5-fabs((pt.z-pArcSlot->offset_v));
							else
								ls_pos.y=fabs((pt.z-pArcSlot->offset_v))-pArcSlot->height*0.5;
						}
					}
					else if(pArcSlot->m_siLayoutStyle==1 || pArcSlot->m_siLayoutStyle==3)
					{
						if(fabs(pBolt->ucs.axis_z*ucs.axis_z)>fabs(pBolt->ucs.axis_z*ucs.axis_x)&&
							fabs(pBolt->ucs.axis_z*ucs.axis_z)>fabs(pBolt->ucs.axis_z*ucs.axis_y))
						{	//��˨λ�ڲ۸ֵĵױ�
							f3dPoint inters;
							Int3dlf(inters,pBolt->ucs.origin,ucs.axis_z,ucs.origin,ucs.axis_z);
							double dist=DISTANCE(inters,ucs.origin)-pArcSlot->baseline.Radius()-pArcSlot->offset_h;
							ls_pos.y=dist-pArcSlot->height*0.5;
						}
						else	//��˨λ�ڲ۸ֲ��
						{
							f3dPoint pt,inters,pick,vec;
							pt=pBolt->ucs.origin;
							coord_trans(pt,ucs,FALSE);
							vec.Set(pt.x,pt.y,0);
							//�˴�֮ǰ�����б��� wjh-2019.8.26
							pick=ucs.origin+ucs.axis_z*pArcSlot->offset_v+vec*(pArcSlot->baseline.Radius()+pArcSlot->offset_h);
							Int3dlf(inters,pt,ucs.axis_z,pick,ucs.axis_z);
							f3dPoint intersUp,intersDown;
							if(pArcSlot->m_siLayoutStyle==1)
							{
								intersDown=inters;
								pick=pick+vec*pArcSlot->height;
								Int3dlf(intersUp,pt,ucs.axis_z,pick,ucs.axis_z);
							}
							else
							{
								intersDown=inters;
								pick=pick-vec*pArcSlot->height;
								Int3dlf(intersDown,pt,ucs.axis_z,pick,ucs.axis_z);
							}
							if(DISTANCE(intersUp,pt)>DISTANCE(intersDown,pt))
								ls_pos.y=-fabs(pt.z-pArcSlot->offset_v)-pArcSlot->height*0.5;
							else
								ls_pos.y=fabs(pt.z-pArcSlot->offset_v)+pArcSlot->height*0.5;
						}
					}
					//����˨��
					circle.norm.Set(0,0,1);
					circle.centre.Set(ls_pos.x,ls_pos.y,0);
					circle.radius = (pBolt->get_d()+pBolt->hole_d_increment)/2;
					circle.ID	  = pBolt->handle;
					if(pBolt->IsFootNail())
						circle.pen.width = 2;	//�Ŷ�
					else
						circle.pen.width = 1;
					g_pSolidDraw->NewCircle(circle);
				}
			}
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_ARCANGLE)
			{	//���νǸ�
				CLDSArcAngle *pArcAngle=(CLDSArcAngle*)console.m_pWorkPart;
				double len=pArcAngle->baseline.Radius()*pArcAngle->baseline.SectorAngle();
				g_pSolidDraw->NewPoint(-pArcAngle->start_oddment,0,0);
				g_pSolidDraw->NewPoint(-pArcAngle->start_oddment,pArcAngle->wing_thick,0);
				g_pSolidDraw->NewPoint(-pArcAngle->start_oddment,pArcAngle->wing_wide,0);
				g_pSolidDraw->NewPoint(-pArcAngle->start_oddment,-pArcAngle->wing_thick,0);
				g_pSolidDraw->NewPoint(-pArcAngle->start_oddment,-pArcAngle->wing_wide,0);
				
				g_pSolidDraw->NewPoint(len+pArcAngle->end_oddment,0,0);
				g_pSolidDraw->NewPoint(len+pArcAngle->end_oddment,pArcAngle->wing_thick,0);
				g_pSolidDraw->NewPoint(len+pArcAngle->end_oddment,pArcAngle->wing_wide,0);
				g_pSolidDraw->NewPoint(len+pArcAngle->end_oddment,-pArcAngle->wing_thick,0);
				g_pSolidDraw->NewPoint(len+pArcAngle->end_oddment,-pArcAngle->wing_wide,0);
				//������
				line.pen.style=PS_SOLID;
				line.startPt.Set(-pArcAngle->start_oddment,-pArcAngle->wing_wide);
				line.endPt.Set(-pArcAngle->start_oddment,pArcAngle->wing_wide);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pArcAngle->start_oddment,-pArcAngle->wing_wide);
				line.endPt.Set(len+pArcAngle->end_oddment,-pArcAngle->wing_wide);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pArcAngle->start_oddment,pArcAngle->wing_wide);
				line.endPt.Set(len+pArcAngle->end_oddment,pArcAngle->wing_wide);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(len+pArcAngle->end_oddment,-pArcAngle->wing_wide);
				line.endPt.Set(len+pArcAngle->end_oddment,pArcAngle->wing_wide);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pArcAngle->start_oddment,-pArcAngle->wing_thick);
				line.endPt.Set(len+pArcAngle->end_oddment,-pArcAngle->wing_thick);
				line.pen.style=PS_DASH;
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pArcAngle->start_oddment,pArcAngle->wing_thick);
				line.endPt.Set(len+pArcAngle->end_oddment,pArcAngle->wing_thick);
				g_pSolidDraw->NewLine(line);
				line.startPt.Set(-pArcAngle->start_oddment,0);
				line.endPt.Set(len+pArcAngle->end_oddment,0);
				line.pen.style=PS_SOLID;
				g_pSolidDraw->NewLine(line);
				//��˨��
				UCS_STRU ucs;
				pArcAngle->getUCS(ucs);
				for(CLsRef *pLsRef=pArcAngle->GetFirstLsRef();pLsRef;pLsRef=pArcAngle->GetNextLsRef())
				{
					//������˨�ڽǸ��ϵ�λ��
					CLDSBolt* pBolt=pLsRef->GetLsPtr();
					f3dPoint inters,ls_pos;
					double angle=CalAngleInArcPart(pBolt->ucs.origin,pArcAngle);
					ls_pos.x=pArcAngle->baseline.Radius()*angle;
					if(fabs(pBolt->ucs.axis_z*ucs.axis_z)>fabs(pBolt->ucs.axis_z*ucs.axis_x)&&
						fabs(pBolt->ucs.axis_z*ucs.axis_z)>fabs(pBolt->ucs.axis_z*ucs.axis_y))
					{	//X֫:
						Int3dlf(inters,pBolt->ucs.origin,ucs.axis_z,ucs.origin,ucs.axis_z);
						double dist=DISTANCE(inters,ucs.origin);
						ls_pos.y=-fabs(dist-pArcAngle->baseline.Radius()-pArcAngle->offset_h);
					}
					else	
					{	//Y֫:
						f3dPoint pt=pBolt->ucs.origin;
						coord_trans(pt,ucs,FALSE);
						ls_pos.y=fabs(pt.z-pArcAngle->offset_v);
					}
					//����˨��
					circle.norm.Set(0,0,1);
					circle.centre.Set(ls_pos.x,ls_pos.y,0);
					circle.radius = (pBolt->get_d()+pBolt->hole_d_increment)/2;
					circle.ID	  = pBolt->handle;
					circle.norm.Set(0,0,1);
					if(pBolt->IsFootNail())
						circle.pen.width = 2;	//�Ŷ�
					else
						circle.pen.width = 1;
					g_pSolidDraw->NewCircle(circle);
				}
			}
			//�ڹ����༭������ʾ����������ͼ  ��ʾ�˰������ͼ 
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				CLDSParamPlate *pParamPlate=(CLDSParamPlate *)console.m_pWorkPart;
				int i=0;
				PROFILE_VERTEX *pPrevPnt=pParamPlate->vertex_list.GetTail();
				f3dLine huoquline;
				bool bZhiWanStart=false,bZhiWanEnd=false;
				for(PROFILE_VERTEX *pVertex=pParamPlate->vertex_list.GetFirst();pVertex;pVertex=pParamPlate->vertex_list.GetNext(),i++)
				{
					pVertex->vertex.ID=i+1;
					if(pVertex->vertex.feature>10)
					{
						if(!bZhiWanStart)
						{
							huoquline.startPt=pVertex->vertex;
							bZhiWanStart=true;
						}
						else if(!bZhiWanEnd)
						{
							huoquline.endPt=pVertex->vertex;
							bZhiWanEnd=true;
						}
					}
					SUB_OBJ* pSubObj=Ta.GeEnts.Add(0);//Ta.GeEnts.NewUniqueId());NewUniqueId�ѱ�Ϊ�Զ�����Id
					pSubObj->hObj=pParamPlate->handle;
					pSubObj->ciSubType=IDbEntity::DbPoint;
					pSubObj->pSubObj=pVertex;

					g_pSolidDraw->NewPoint(pVertex->vertex.x,pVertex->vertex.y,0,SUBID(pSubObj->iSubId,SUBID::GROUP_BOUNDARY));
					//������
					line.pen.style=PS_SOLID;
					f3dArcLine arcLine;
					int sign=1;
					int iFace=pPrevPnt->vertex.feature;
					if(pPrevPnt->m_bWeldEdge)
						arcLine.pen.width=line.pen.width=2;	//������
					else
						arcLine.pen.width=line.pen.width=0;
					if(pPrevPnt->type==1)
					{	//Բ��
						if(pPrevPnt->sector_angle<0)
							sign=-1;
						arcLine.CreateMethod2(pPrevPnt->vertex,pVertex->vertex,f3dPoint(0,0,sign),sign*pPrevPnt->sector_angle);
						g_pSolidDraw->NewArcLine(arcLine);
					}
					else if(pPrevPnt->type==2)	//ָ��Բ��R
					{
						arcLine.CreateMethod3(pPrevPnt->vertex,pVertex->vertex,f3dPoint(0,0,sign),pPrevPnt->radius,pPrevPnt->center);
						g_pSolidDraw->NewArcLine(arcLine);
					}
					else if(pPrevPnt->type==3)	//��Բ��
					{
						f3dPoint center = pPrevPnt->center;
						f3dPoint col_axis=pPrevPnt->column_norm;
						coord_trans(center,pParamPlate->ucs,FALSE);
						vector_trans(col_axis,pParamPlate->ucs,FALSE);
						if(pPrevPnt->vertex.feature%10==2&&pVertex->vertex.feature%10==2)
						{	//�������ϵ���Բ��������Ӧ��Բ�ı궨�㼰���淨����תչ�����������ٽ�������ϵת��
							center=pParamPlate->SpreadTransV(center);
							col_axis=pParamPlate->SpreadTransV(col_axis,false);
						}
						if(pPrevPnt->radius<0)
							sign=-1;
						arcLine.CreateEllipse(center,pPrevPnt->vertex,pVertex->vertex,col_axis,f3dPoint(0,0,sign),sign*pPrevPnt->radius);
						g_pSolidDraw->NewArcLine(arcLine);
					}
					else
					{	//ֱ��
						if ((pPrevPnt->vertex.feature == 12 && pVertex->vertex.feature == 13) ||
							(pPrevPnt->vertex.feature == 13 && pVertex->vertex.feature == 12))
						{	//�������߽��о�ߴ�������������⴦��
							pPrevPnt = pVertex;
							bZhiWanStart = bZhiWanEnd = FALSE;
							continue;
						}
						line.startPt.Set(pPrevPnt->vertex.x, pPrevPnt->vertex.y);
						line.endPt.Set(pVertex->vertex.x,pVertex->vertex.y);
						line.startPt.ID=pPrevPnt->vertex.ID;
						line.endPt.ID=pVertex->vertex.ID;
						g_pSolidDraw->NewLine(line);
					}
					pPrevPnt = pVertex;
				}
				if(bZhiWanStart&&bZhiWanEnd)
				{
					huoquline.pen.style=PS_DASHDOTDOT;
					huoquline.pen.crColor=RGB(255,0,0);
					huoquline.pen.width=1;
					huoquline.feature=2;	//�����߻���
					huoquline.ID=1;	//��һ��������
					g_pSolidDraw->NewLine(huoquline);
				}	
				if(pParamPlate->m_iParamType==TYPE_FL||
					pParamPlate->m_iParamType==TYPE_FLG||
					pParamPlate->m_iParamType==TYPE_FLR)
				{
					double r=0;
					pParamPlate->GetDesignItemValue('R',&r);
					//����һ������Բ,�����ж�Բ�����νǶ� wjh-2012.3.30
					circle.norm.Set(0,0,1);
					circle.centre = pParamPlate->ucs.origin;
					coord_trans(circle.centre,pParamPlate->ucs,FALSE);
					circle.radius = r;
					circle.pen.width = 1;
					g_pSolidDraw->NewCircle(circle);
				}
				else if (pParamPlate->m_iParamType == TYPE_PEDALPLATE)
				{	//��ʾ��̤���߲���
					long A = 0, V = 0;
					double W = 0, H = 0;
					pParamPlate->GetDesignItemValue('A', &A);
					pParamPlate->GetDesignItemValue('V', &V);
					pParamPlate->GetDesignItemValue('W', &W);
					pParamPlate->GetDesignItemValue('H', &H);
					CLDSLineAngle* pDatumJg = (CLDSLineAngle*)Ta.FromPartHandle(A, CLS_LINEANGLE);
					double ddx = fabs(pParamPlate->ucs.axis_z*pDatumJg->vxWingNorm);
					double ddy = fabs(pParamPlate->ucs.axis_z*pDatumJg->vyWingNorm);
					GEPOINT spreadVec = (ddx > ddy) ? pDatumJg->GetWingVecX() : pDatumJg->GetWingVecY();
					spreadVec *= (V == 0) ? 1 : -1;
					vector_trans(spreadVec, pParamPlate->ucs, FALSE);
					f3dPoint linePtS, linePtE;
					for (pVertex = pParamPlate->vertex_list.GetFirst(); pVertex; pVertex = pParamPlate->vertex_list.GetNext())
					{
						if (pVertex->vertex.feature == PROFILE_VERTEX::DT_BEND_DATUM_POINT)
							linePtS = pVertex->vertex;
						else if (pVertex->vertex.feature == PROFILE_VERTEX::DT_BEND_EXTEND_POINT)
							linePtE = pVertex->vertex;
					}
					f3dLine roll_line, huoqu_line, roll_edge;
					roll_edge.pen = line.pen;
					huoqu_line.pen.style = PS_DASHDOTDOT;
					huoqu_line.pen.crColor = RGB(255, 0, 0);
					roll_line.pen.style = PS_DASHDOT;
					roll_line.startPt = linePtS;
					roll_line.endPt = linePtE;
					for (int ii = 0; ii < 2; ii++)
					{
						GEPOINT vec = (ii == 0) ? GEPOINT(0, 1, 0) : spreadVec;
						GEPOINT line_vec = (roll_line.endPt - roll_line.startPt).normalized();
						GEPOINT offset_vec(line_vec.y, -line_vec.x);
						normalize(offset_vec);
						if (offset_vec*vec < 0)
							offset_vec *= -1;
						double roll_len = (ii == 0) ? W : H;
						huoqu_line.startPt = roll_line.startPt + offset_vec * pParamPlate->thick;
						huoqu_line.endPt = roll_line.endPt + offset_vec * pParamPlate->thick;
						roll_edge.startPt = roll_line.startPt + offset_vec * (roll_len + pParamPlate->thick);
						roll_edge.endPt = roll_line.endPt + offset_vec * (roll_len + pParamPlate->thick);
						g_pSolidDraw->NewPoint(roll_edge.startPt);
						g_pSolidDraw->NewPoint(roll_edge.endPt);
						g_pSolidDraw->NewPoint(huoqu_line.startPt);
						g_pSolidDraw->NewPoint(huoqu_line.endPt);
						g_pSolidDraw->NewLine(roll_line);
						g_pSolidDraw->NewLine(huoqu_line);
						g_pSolidDraw->NewLine(roll_edge);
						line.startPt = roll_line.startPt;
						line.endPt = roll_edge.startPt;
						g_pSolidDraw->NewLine(line);
						if (ii == 0)
						{	
							line.startPt = roll_line.endPt;
							line.endPt = huoqu_line.endPt;
							g_pSolidDraw->NewLine(line);
							//��¼����ľ����
							roll_line.startPt = huoqu_line.endPt;
							roll_line.endPt = roll_edge.endPt;
						}
						else
						{
							line.startPt = roll_line.endPt;
							line.endPt = roll_edge.endPt;
							g_pSolidDraw->NewLine(line);
						}
					}
				}
				//��ʾ�������Բ wht 11-05-27
				double inner_r=0;	//��Բ�뾶
				f3dPoint col_axis;	//Բ�����߷���
				if(pParamPlate->GetInnerCirRadius(inner_r,col_axis))
				{	//��ʱ����Բ��ʾʵ��,��ͼʱ����Բ����
					circle.norm.Set(0,0,1);
					circle.centre.Set();
					circle.radius = inner_r;
					circle.pen.width = 1;
					g_pSolidDraw->NewCircle(circle);
				}
				//
				line.pen.style=PS_SOLID;
				line.pen.crColor=0;
				line.pen.width=1;
				line.feature=0;//��ֱͨ��
				//��˨��
				CXhSimpleList<CLDSPart::HOLE> hole_list;
				pParamPlate->GetHoleSet(&hole_list);
				for(CLDSPart::HOLE* pHole=hole_list.EnumObjectFirst();pHole;pHole=hole_list.EnumObjectNext())
				{
					if(pHole->pBolt && pParamPlate->m_iParamType==TYPE_XEND)
					{
						CLsRef* pLsRef=pParamPlate->FindLsByHandle(pHole->pBolt->handle);
						if(pLsRef->start1_end2_mid0>0)
						{	
							if(pLsRef->start1_end2_mid0!=11&&pLsRef->start1_end2_mid0!=21)
								continue;	//�������ڲ�巶Χ�ڵ���˨
						}
						//��start1_end2_mid0��ʾ��˨λ��֮ǰ����ʮ�ֲ��
						double L=0;
						pParamPlate->GetDesignItemValue('L',&L);
						if(fabs((*pLsRef)->get_norm()*pParamPlate->ucs.axis_z)<EPS_COS2)
							continue;
						if((*pLsRef)->des_base_pos.len_offset_dist>L)
							continue; //������ʮ�ֲ�巶Χ�����˨��
					}
					//������˨��
					f3dPoint ls_pos=pHole->xLocalPosition;
					if(fabs(ls_pos.x)<eps)
						ls_pos.x = 0;
					if(fabs(ls_pos.y)<eps)
						ls_pos.y = 0;
					if(fabs(ls_pos.z)<eps)
						ls_pos.z = 0;
					circle.norm.Set(0,0,1);
					circle.centre = ls_pos;
					circle.radius = pHole->fHoleD*0.5;
					circle.ID	  = pHole->pBolt?pHole->pBolt->handle:0;
					if(pHole->pBolt && pHole->pBolt->IsFootNail())
						circle.pen.width = 2;	//�Ŷ�
					else
						circle.pen.width = 1;
					g_pSolidDraw->NewCircle(circle);
				}
			}
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE)
			{
				CLDSLineTube *pLineTube=NULL;
				CLDSPlate *pPlate=(CLDSPlate *)console.m_pWorkPart;
				pLineTube=(CLDSLineTube*)console.FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
				f3dPoint tube_len_vec;	//�������˼����췽��
				if(pLineTube)	
				{
					tube_len_vec=pLineTube->End()-pLineTube->Start();
					normalize(tube_len_vec);
					vector_trans(tube_len_vec,pPlate->ucs,FALSE);	//ת����������ϵ��
				}
				if(pPlate->IsPeakPointPlate())
					g_pSolidDraw->NewPoint(pPlate->top_point.x,pPlate->top_point.y,0,pPlate->handle);
				int i=0;
				PROFILE_VERTEX *pPrevPnt=pPlate->vertex_list.GetTail(),*pPrevPrevPnt=pPlate->vertex_list.GetPrev();
				for(PROFILE_VERTEX *pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext(),i++)
				{
					pVertex->vertex.ID=i+1;
					SUB_OBJ* pSubObj=Ta.GeEnts.Add(0);//Ta.GeEnts.NewUniqueId());NewUniqueId�ѱ�Ϊ�Զ�����Id
					pSubObj->hObj=pPlate->handle;
					pSubObj->ciSubType=IDbEntity::DbPoint;
					pSubObj->pSubObj=pVertex;
					g_pSolidDraw->NewPoint(pVertex->vertex.x,pVertex->vertex.y,0,SUBID(pSubObj->iSubId,SUBID::GROUP_BOUNDARY));
					//������
					line.pen.style=PS_SOLID;
					if(pPrevPnt->m_bWeldEdge)
					{
						line.pen.width=2;
						if(pLineTube&&(pPrevPnt->local_point_vec==1||pPrevPnt->local_point_vec==2))
						{	//���Ӹ�����Ϊ�ֹ����жԻ����Ϣʱ����Ҫ��ע�Ի��
							f3dPoint line_vec=pPlate->GetRealVertex(pVertex->vertex)-pPlate->GetRealVertex(pPrevPnt->vertex);
							normalize(line_vec);
							GEPOINT local_point=pPrevPnt->RetrieveWeldLocation(pVertex->vertex,tube_len_vec);
							/*f3dPoint local_point(0,pPrevPnt->local_point_y,0);
							if(fabs(line_vec.y)<EPS)
								local_point.x=pPrevPnt->vertex.x;
							else
								local_point.x=(pPrevPnt->local_point_y-pPrevPnt->vertex.y)*(line_vec.x/line_vec.y)+pPrevPnt->vertex.x;*/
							g_pSolidDraw->NewMark(local_point,5);
							//��ʼ-->�շ����뺸�Ӹ��˼�ʼ-->�շ�����ͬ�ҶԻ�㷽��Ϊʼ-->��
							//���߱���˼��෴�ҶԻ�㷽��Ϊ��-->ʼ
							if((pPrevPnt->local_point_vec==1&&tube_len_vec*line_vec>EPS_COS2)||
								(pPrevPnt->local_point_vec==2&&(-tube_len_vec)*line_vec>EPS_COS2))
							{	
								local_point-=line_vec*50;
								g_pSolidDraw->NewMark(local_point,5);
								local_point+=line_vec*150;
								g_pSolidDraw->NewMark(local_point,5);
							}
							//��ʼ-->�շ����뺸�Ӹ��˼�ʼ-->�շ�����ͬ�ҶԻ�㷽��Ϊ��-->ʼ
							//���߱���˼������෴�ҶԻ�㷽��Ϊʼ-->��
							else if((pPrevPnt->local_point_vec==2&&tube_len_vec*line_vec>EPS_COS2)||
								(pPrevPnt->local_point_vec==1&&(-tube_len_vec)*line_vec>EPS_COS2))
							{	
								local_point+=line_vec*50;
								g_pSolidDraw->NewMark(local_point,5);
								local_point-=line_vec*150;
								g_pSolidDraw->NewMark(local_point,5);
							}
						}
					}
					else
						line.pen.width=0;
					if((pPrevPrevPnt&&pPrevPrevPnt->m_bRollEdge&&pPrevPrevPnt->manu_space!=0)||
						(pPrevPnt&&pPrevPnt->m_bRollEdge&&pPrevPnt->manu_space!=0)||(pVertex->m_bRollEdge&&pVertex->manu_space!=0))
					{	//�о�߹���ʱ���˴����������߹��������������ӵ�ǰ�����������ߣ��������ߣ�wxc-2017.3-30
						pPrevPrevPnt=pPrevPnt;
						pPrevPnt=pVertex;
						continue;
					}
					//pPrevPnt->vertex.feature+pVertex->vertex.feature!=5�����Ը�ǿ(��������ͨ�����XY��Գ����ɵ������)��wjh-2015.9.10
					if(pPlate->face_N<3||pPlate->IsFoldPlate()||(pPrevPnt->vertex.feature+pVertex->vertex.feature!=5))
					{
						f3dArcLine arcLine;
						arcLine.pen=line.pen;
						int sign=1;
						int iFace=pPrevPnt->vertex.feature;
						if(pPrevPnt->type==1)
						{	//Բ��
							if(pPrevPnt->sector_angle<0)
								sign=-1;
							arcLine.CreateMethod2(pPrevPnt->vertex,pVertex->vertex,f3dPoint(0,0,sign),fabs(pPrevPnt->sector_angle));
							g_pSolidDraw->NewArcLine(arcLine);
						}
						else if(pPrevPnt->type==2)	//ָ��Բ��R
						{
							/*double half_len = 0.5*DISTANCE(pPrevPnt->vertex,pVertex->vertex);
							if(pPrevPnt->radius<0)
								sign=-1;
							if(fabs(half_len/pPrevPnt->radius)<=1)
								pPrevPnt->sector_angle = 2*asin(half_len/fabs(pPrevPnt->radius));
							else 
								pPrevPnt->sector_angle = Pi;
							arcLine.CreateMethod2(pPrevPnt->vertex,pVertex->vertex,f3dPoint(0,0,sign),pPrevPnt->sector_angle);*/
							arcLine.CreateMethod3(pPrevPnt->vertex,pVertex->vertex,f3dPoint(0,0,1),pPrevPnt->radius,pPrevPnt->center);
							g_pSolidDraw->NewArcLine(arcLine);
						}
						else if(pPrevPnt->type==3)	//��Բ��
						{
							f3dPoint center = pPrevPnt->center;
							f3dPoint col_axis=pPrevPnt->column_norm;
							coord_trans(center,pPlate->ucs,FALSE);
							vector_trans(col_axis,pPlate->ucs,FALSE);
							if(pPrevPnt->radius<0)
								sign=-1;
							arcLine.CreateEllipse(center,pPrevPnt->vertex,pVertex->vertex,col_axis,f3dPoint(0,0,sign),sign*pPrevPnt->radius);
							g_pSolidDraw->NewArcLine(arcLine);
						}
						else
						{	//ֱ��
							line.startPt.Set(pPrevPnt->vertex.x,pPrevPnt->vertex.y);
							line.endPt.Set(pVertex->vertex.x,pVertex->vertex.y);
							line.startPt.ID=pPrevPnt->vertex.ID;
							line.endPt.ID=pVertex->vertex.ID;
							g_pSolidDraw->NewLine(line);
						}
					}
					else
					{
						line.startPt.Set(pPrevPnt->vertex.x,pPrevPnt->vertex.y);
						line.endPt.Set(pPlate->top_point.x,pPlate->top_point.y);
						line.startPt.ID=pPrevPnt->vertex.ID;
						g_pSolidDraw->NewLine(line);
						line.startPt = line.endPt;
						line.endPt.Set(pVertex->vertex.x,pVertex->vertex.y);
						line.endPt.ID=pVertex->vertex.ID;
						g_pSolidDraw->NewLine(line);
					}
					pPrevPrevPnt=pPrevPnt;
					pPrevPnt = pVertex;
				}
				line.pen.style=PS_DASHDOTDOT;
				line.pen.crColor=RGB(255,0,0);
				line.pen.width=1;
				line.feature=2;	//�����߻���
				PROFILE_VERTEX* pBendLine1Start=NULL,*pBendLine1End=NULL,*pBendLine2Start=NULL,*pBendLine2End=NULL;
				pPlate->GetBendlineVertexies(&pBendLine1Start,&pBendLine1End,&pBendLine2Start,&pBendLine2End);
				for(i=0;i<2;i++)
				{	//��ӻ�����
					PROFILE_VERTEX* pBendLineS=(i==0)?pBendLine1Start:pBendLine2Start;
					PROFILE_VERTEX* pBendLineE=(i==0)?pBendLine1End:pBendLine2End;
					if(pBendLineS==NULL && pBendLineE==NULL)
						continue;
					if(pBendLineS)
						line.startPt=pBendLineS->vertex;
					else
						line.startPt=pPlate->top_point;
					if(pBendLineE)
						line.endPt=pBendLineE->vertex;
					else
						line.endPt=pPlate->top_point;
					if(!line.startPt.IsEqual(line.endPt))
					{
						line.ID=i+1;
						g_pSolidDraw->NewLine(line);
					}
				}
				if(pPlate->m_fInnerRadius>0)
				{
					//����ע�ͣ�pPlate->m_tInnerColumnNormΪ���ʾ��ְ崹ֱ��ָ������ֵ������ת�������׸��²���ʱ wjh-2016.3.22
					//��pPlate->m_tInnerColumnNorm.IsZero()ʱĬ��ΪԲ�� wxc-2017.2.28
					if(!pPlate->m_tInnerColumnNorm.IsZero()&&fabs(pPlate->m_tInnerColumnNorm*pPlate->ucs.axis_z)<EPS_COS)	//Բ��
					{	//��Բ
						//�˴������� 
						f3dArcLine arcLine;
						if(!pPlate->m_tInnerOrigin.IsZero())
						{	//���ֻ����ڻ�����Բλ�ڻ������Բ��ʱ����
							f3dPoint workNorm(0,0,1),columnNorm=pPlate->m_tInnerColumnNorm;
							vector_trans(columnNorm,pPlate->ucs,FALSE);
							f3dPoint minorAxis=columnNorm^workNorm;	
							normalize(minorAxis);	//��Բ���᷽��
							f3dPoint majorAxis(-minorAxis.y,minorAxis.x,minorAxis.z);	
							normalize(majorAxis);	//��Բ���᷽��
							double radiusRatio=fabs(columnNorm*workNorm);
							double minorRadius=pPlate->m_fInnerRadius;				//��Բ�̰��᳤��
							double majorRadius=pPlate->m_fInnerRadius/radiusRatio;	//��Բ�����᳤��
							f3dPoint start=pPlate->m_tInnerOrigin+majorAxis*majorRadius;
							f3dPoint end = pPlate->m_tInnerOrigin+minorAxis*minorRadius;
							int sign=1;
							if(columnNorm*workNorm<EPS)
								sign=-1;
							arcLine.CreateEllipse(pPlate->m_tInnerOrigin,start,end,columnNorm,f3dPoint(0,0,sign),minorRadius);
							g_pSolidDraw->NewArcLine(arcLine);
							start=end;
							end=pPlate->m_tInnerOrigin-majorAxis*majorRadius;
							arcLine.CreateEllipse(pPlate->m_tInnerOrigin,start,end,columnNorm,f3dPoint(0,0,sign),minorRadius);
							g_pSolidDraw->NewArcLine(arcLine);
							start=end;
							end = pPlate->m_tInnerOrigin-minorAxis*minorRadius;
							arcLine.CreateEllipse(pPlate->m_tInnerOrigin,start,end,columnNorm,f3dPoint(0,0,sign),minorRadius);
							g_pSolidDraw->NewArcLine(arcLine);
							start=end;
							end=pPlate->m_tInnerOrigin+majorAxis*majorRadius;
							arcLine.CreateEllipse(pPlate->m_tInnerOrigin,start,end,columnNorm,f3dPoint(0,0,sign),minorRadius);
							g_pSolidDraw->NewArcLine(arcLine);
						}
						else
						{	//���ֻ����ڻ�����Բ���ڻ������Բ��ʱ����
							double radius=pPlate->m_fInnerRadius;
							//����Բ����һ��
							f3dPoint inter_pos,inter_pos1,vertex,axis_x;
							vertex=pPlate->vertex_list.GetFirst()->vertex;
							coord_trans(vertex,pPlate->ucs,TRUE);
							axis_x=vertex-pPlate->ucs.origin;
							normalize(axis_x);
							f3dPoint inner_origin=pPlate->m_tInnerOrigin;
							coord_trans(inner_origin,pPlate->ucs,TRUE);
							Int3dlc(&inter_pos,&inter_pos1,vertex,axis_x,inner_origin,inner_origin+100*pPlate->m_tInnerColumnNorm,radius,radius);
							//�趨��Բ������
							f3dPoint center = pPlate->m_tInnerOrigin;	//pPlate->ucs.origin;
							f3dPoint col_axis=pPlate->m_tInnerColumnNorm;
							coord_trans(inter_pos,pPlate->ucs,FALSE);
							coord_trans(inter_pos1,pPlate->ucs,FALSE);
							//coord_trans(center,pPlate->ucs,FALSE);
							vector_trans(col_axis,pPlate->ucs,FALSE);
							arcLine.CreateEllipse(center,inter_pos,inter_pos1,col_axis,f3dPoint(0,0,1),pPlate->m_fInnerRadius);
							g_pSolidDraw->NewArcLine(arcLine);
							arcLine.CreateEllipse(center,inter_pos1,inter_pos,col_axis,f3dPoint(0,0,1),pPlate->m_fInnerRadius);
							g_pSolidDraw->NewArcLine(arcLine);
						}
					}
					else
					{
						circle.norm.Set(0,0,1);
						circle.centre =	pPlate->m_tInnerOrigin;	//pPlate->ucs.origin;
						//coord_trans(circle.centre,pPlate->ucs,FALSE);
						circle.radius = pPlate->m_fInnerRadius;
						circle.pen.width = 1;
						g_pSolidDraw->NewCircle(circle);
					}
				}
				line.pen.style=PS_SOLID;
				line.pen.crColor=0;
				line.pen.width=1;
				line.feature=0;//��ֱͨ��
				//��˨��
				f3dPoint ls_pos;
				for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
				{
					pPlate->SpreadLsPos(pLsRef,ls_pos);
					if(fabs(ls_pos.x)<eps)
						ls_pos.x = 0;
					if(fabs(ls_pos.y)<eps)
						ls_pos.y = 0;
					if(fabs(ls_pos.z)<eps)
						ls_pos.z = 0;
					circle.norm.Set(0,0,1);
					circle.centre = ls_pos;
					circle.radius = ((*pLsRef)->get_d()+(*pLsRef)->hole_d_increment)/2;
					circle.ID	  = (*pLsRef)->handle;
					if((*pLsRef)->IsFootNail())
						circle.pen.width = 2;	//�Ŷ�
					else
						circle.pen.width = 1;
					g_pSolidDraw->NewCircle(circle);
				}
				//������
				int iCount=0; 
				pPrevPnt=pPlate->vertex_list.GetTail();
				PROFILE_VERTEX* pTailVertex=pPrevPnt;
				for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext(),iCount++)
				{
					if(pVertex->m_bRollEdge&&pVertex->manu_space!=0&&pVertex->type==0)
					{
						PROFILE_VERTEX* pRollLineStart = pVertex;
						int nPush=pPlate->vertex_list.push_stack();
						PROFILE_VERTEX* pRollLineEnd = NULL,*pNextVertex = NULL;
						pRollLineEnd=(pTailVertex==pVertex)?pPlate->vertex_list.GetFirst():pPlate->vertex_list.GetNext();
						pNextVertex =(pTailVertex==pRollLineEnd)?pPlate->vertex_list.GetFirst(): pPlate->vertex_list.GetNext();
						pPlate->vertex_list.pop_stack(nPush);
						DrawRollEdge(pPrevPnt,pRollLineStart,pRollLineEnd,pNextVertex,pPlate);
					}
					pPrevPnt=pVertex;
				}
			}
			/*g_pSolidSet->SetObjectUcs(console.m_curACS);
			DISPLAY_TYPE type;
			g_pSolidSet->GetDisplayType(&type);
			if(type==DISP_SOLID)
			{
				g_pSolidDraw->NewSolidPart(console.m_pWorkPart->GetSolidPartObject());
				g_pSolidDraw->RenderDisplayList();//��ҪĿ����Ϊ��ȫ�Բ���ǰ���Ի�ͼ�ռ��С
			}*/
			CLDSView *pView=pFrame->GetLDSView();
			if(pView)
			{	//���ƹ����༭������ӵ���ʱ�������븨���� wht 16-11-10
				for(GEPOINT *pPt=pView->m_xAssistPointList.GetFirst();pPt;pPt=pView->m_xAssistPointList.GetNext())
					g_pSolidDraw->NewPoint(f3dPoint(*pPt));
				for(GELINE *pLine=pView->m_xAssistLineList.GetFirst();pLine;pLine=pView->m_xAssistLineList.GetNext())
					g_pSolidDraw->NewLine(f3dLine(pLine->start,pLine->end));
			}
		}
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
#endif
}
//from Analysis.cpp
BOOL SelectNodeInPole(CLDSLinePart *pLinePart,NODESET& node_set);
DWORD WINAPI CreatePart3dSolidModel(LPVOID lpParam)
{
#ifdef __ANGLE_PART_INC_
	CMutiThreadTask *pTaskQueue=(CMutiThreadTask*)lpParam;
	CLDSPart *pPart=(CLDSPart*)pTaskQueue->GetHeadTask();
	while (true)
	{
		if(pPart!=NULL)
		{
			pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),
				g_pSolidOper->GetScaleUserToScreen(),g_sysPara.display.nSmoothness);
			if(pTaskQueue->m_pContext)
				((CMutiThreadTask*)pTaskQueue->m_pContext)->AddTask(pPart);
			pTaskQueue->RemoveHead();
			pPart=(CLDSPart*)pTaskQueue->GetHeadTask();
		}
		else if(pTaskQueue->m_bWaitOnIdle)
		{
			Sleep(10);
			pPart=(CLDSPart*)pTaskQueue->GetHeadTask();
		}
		else
			break;
	}
	pTaskQueue->OverRunningState();
#endif
	return 0;
}
DWORD WINAPI DrawPart3dSolidModel(LPVOID lpParam)
{
#ifdef __ANGLE_PART_INC_
	CMutiThreadTask *pTaskQueue=(CMutiThreadTask*)lpParam;
	CLDSPart *pPart=(CLDSPart*)pTaskQueue->GetHeadTask();
	while (true)
	{
		if(pPart!=NULL)
		{
			g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
			pTaskQueue->RemoveHead();
			pPart=(CLDSPart*)pTaskQueue->GetHeadTask();
		}
		else if(pTaskQueue->m_bWaitOnIdle)
		{
			Sleep(10);
			pPart=(CLDSPart*)pTaskQueue->GetHeadTask();
		}
		else
			break;
	}
	pTaskQueue->OverRunningState();
#endif
	return 0;
}
COLORREF CalEfficColor(double efficCoef)
{
	BYTE r=0,g=0;
	double pivot[2]={0.55,0.95};
	double pivotcoef[2]={255/0.55,255/0.95};
	if(efficCoef<=0)
	{
		r=0;
		g=0;
	}
	else if(efficCoef<=pivot[0])
	{
		r=(BYTE)ftoi(pivotcoef[0]*efficCoef);
		g=255;
	}
	else if(efficCoef>pivot[0]&&efficCoef<=pivot[1])
	{
		r=255;
		g=(BYTE)ftoi(205+50-50*(efficCoef-pivot[0])/(pivot[1]-pivot[0]));
	}
	else if(efficCoef>pivot[1]&&efficCoef<=1.0)
	{
		r=255;
		g=(BYTE)ftoi(155+50-50*(efficCoef-pivot[1])/(1.0-pivot[1]));
	}
	else //if(efficCoef>1.0)
		r=255;
	return RGB(r,g,0);
}
static double phasePositionCoef;
BOOL g_bVibrationAnimate=FALSE;
BOOL RetrieveParaPointByAngleHiberarchy(CLDSLineAngle* pAngle,HIBERARCHY hiberarchy,CLDSPoint* pParaPoint)
{
	if(pParaPoint==NULL||pAngle==NULL)
		return FALSE;
	DWORD up1=hiberarchy.HiberUpId(1);
	DWORD up2=hiberarchy.HiberUpId(2);
	DWORD down1=hiberarchy.HiberDownId(1);
	DWORD down2=hiberarchy.HiberDownId(2);
	//up1=0: �ؼ���;down1:�ľ�
	//up1>0: �ؼ���λ��;up1-1Ϊ��ǰ�ؼ��������ľ�
	//		 down1: 0(ʼ��),1(�ն�),����(�ڵ���)
	//		 down2:0Ĭ��(���ݽǸ�״̬���Զ�,����ΪͶӰ),1.��ڵ�ͬX����ƽ��,2.Y����ƽ��,3.Z����ƽ��
	//up2=0: X֫
	//up2=1: Y֫
	//up2=2: �� 
	if(up1==0)	//�ؼ���
		return FALSE;
	if((down1==0||(pAngle->pStart&&down1==pAngle->pStart->handle))&&down2==0)	//ʼ��
	{
		pParaPoint->datum_pos_style=1;
		pParaPoint->des_para.RODEND.direction=0;
	}
	else if((down1==1||(pAngle->pEnd&&down1==pAngle->pEnd->handle))&&down2==0)	//ĩ��
	{
		pParaPoint->datum_pos_style=1;
		pParaPoint->des_para.RODEND.direction=1;
	}
	else
	{
		CLDSNode* pNode=Ta.Node.FromHandle(down1);
		if(pNode==NULL)
			return FALSE;
		pParaPoint->datum_pos_style=2;
	}
	if(pParaPoint->datum_pos_style==1)
	{
		pParaPoint->des_para.RODEND.hRod=pAngle->handle;
		if(up2==1)	//Y֫
			pParaPoint->des_para.RODEND.offset_wing=1;
		else
			pParaPoint->des_para.RODEND.offset_wing=0;
		if(up1==1&&up2!=2)
			pParaPoint->des_para.RODEND.wing_offset_style=0;
		else
		{
			pParaPoint->des_para.RODEND.wing_offset_style=4;
			pParaPoint->des_para.RODEND.wing_offset_dist=up1-1;
		}
		pParaPoint->des_para.RODEND.bIncOddEffect=down1<0x20;
	}
	else if(pParaPoint->datum_pos_style==2)
	{
		pParaPoint->des_para.RODNODE.hRod =pAngle->handle;
		pParaPoint->des_para.RODNODE.hNode=down1;
		if(up2==1)	//Y֫
			pParaPoint->des_para.RODNODE.offset_wing=1;
		else
			pParaPoint->des_para.RODNODE.offset_wing=0;
		if(up1==1&&up2!=2)
			pParaPoint->des_para.RODNODE.wing_offset_style=0;
		else
		{
			pParaPoint->des_para.RODNODE.wing_offset_style=4;
			pParaPoint->des_para.RODNODE.wing_offset_dist=up1-1;
		}
		pParaPoint->des_para.RODNODE.vPlaneNormal.Set(0,0,0);//memset(...,0,sizeof(double)*3);
		if(down2>=1&&down2<=3)
		{
			pParaPoint->des_para.RODNODE.bFaceOffset=TRUE;
			((double*)pParaPoint->des_para.RODNODE.vPlaneNormal)[down2-1]=1.0;
		}
	}
	return true;
};
//cbHighlightFlag,0x01ͶӰ���,0x02ͶӰX֫���ߵ�,0x04ͶӰY֫���ߵ�;0x08ƽ�����;0x10ƽ��X֫���ߵ�;0x20ƽ��Y֫���ߵ�
void AppendAngleSectionFrameDrawing(CLDSLineAngle* pAngle,CLDSNode* pNode,int gWingX,int gWingY,BYTE ciIntersPlane=0,UCS_STRU *pObjectCS=NULL,BYTE cbHighlightFlag=0)
{
	CLDSView::RedrawAngleFrameDrawing(pAngle,0x02,pObjectCS);
	GEPOINT xBer,xWingX,xWingY,xPlaneBer,xPlaneWingX,xPlaneWingY,xOrgPlaneBer,xOrgPlaneWingX,xOrgPlaneWingY;
	IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
	UCS_STRU object_ucs;
	if(pObjectCS!=NULL)
		object_ucs=*pObjectCS;
	else
	{
		CDisplayView *pDispView=console.GetActiveView();
		if(pDispView==NULL)
			return;
		LoadDefaultUCS(&object_ucs);
		object_ucs.origin.Set();
		vector_trans(object_ucs.axis_x,pDispView->ucs,FALSE);
		vector_trans(object_ucs.axis_y,pDispView->ucs,FALSE);
		vector_trans(object_ucs.axis_z,pDispView->ucs,FALSE);
		coord_trans(object_ucs.origin,pDispView->ucs,FALSE);
	}
	//����Ǹ���ָ���ڵ��ͶӰ����ؼ���
	xBer=pAngle->GetDatumPosBer(pNode);
	GEPOINT vWingVecX=pAngle->GetWingVecX();
	GEPOINT vWingVecY=pAngle->GetWingVecY();
	if(gWingX==0)
		gWingX=(int)pAngle->GetNodeWingXG(pNode);
	if(gWingY==0)
		gWingY=(int)pAngle->GetNodeWingYG(pNode);
	xWingX=xBer+vWingVecX*gWingX;
	xWingY=xBer+vWingVecY*gWingY;
	//����Ǹ���ָ���ڵ��ƽ�ƽ���ؼ���
	f3dPoint vPlaneNorm,xPlanePick,vNormal,vxLocalWingNorm=pAngle->vxWingNorm,vyLocalWingNorm=pAngle->vyWingNorm;
	GEPOINT xLocalPlanePick,vLocalPlaneNorm,vLocalLineVec;
	if(ciIntersPlane=='X'||ciIntersPlane=='x')	//ָ��X�������
		vPlaneNorm.Set(1,0,0);
	else if(ciIntersPlane=='Y'||ciIntersPlane=='y')	//ָ��Y�������
		vPlaneNorm.Set(0,1,0);
	else if(ciIntersPlane=='Z'||ciIntersPlane=='z')	//ָ��Y�������
		vPlaneNorm.Set(0,0,1);
	if(pAngle->m_cPosCtrlType==1)		//X-Yƽ��ƽ��
		vNormal.Set(0,0,1);
	else if(pAngle->m_cPosCtrlType==2)	//Y-Zƽ��ƽ��
		vNormal.Set(1,0,0);
	else if(pAngle->m_cPosCtrlType==3)	//X-Zƽ��ƽ��
		vNormal.Set(0,1,0);
	else
		xPlanePick=pNode->Position(true);
	if(pAngle->m_cPosCtrlDatumLineType=='X'&&!vNormal.IsZero())		//X֫���߿���
		xPlanePick=pAngle->GetDatumPosWingXByFaceOffset(pNode,vNormal,NULL,gWingX);
	else if(pAngle->m_cPosCtrlDatumLineType=='Y'&&!vNormal.IsZero())//Y֫���߿���
		xPlanePick=pAngle->GetDatumPosWingYByFaceOffset(pNode,vNormal,NULL,gWingY);
	xLocalPlanePick=xPlanePick;
	vLocalPlaneNorm=vPlaneNorm;
	xPlaneBer=pAngle->GetDatumPosBerByFaceOffset(pNode,vPlaneNorm,&xPlanePick);
	xPlaneWingX=pAngle->GetDatumPosWingXByFaceOffset(pNode,vPlaneNorm,&xPlanePick,gWingX);
	xPlaneWingY=pAngle->GetDatumPosWingYByFaceOffset(pNode,vPlaneNorm,&xPlanePick,gWingY);
	xBer+=(pAngle->vxWingNorm+pAngle->vyWingNorm)*0.1;
	xWingX+=pAngle->vxWingNorm*0.1;
	xWingY+=pAngle->vyWingNorm*0.1;
	xPlaneBer+=(pAngle->vxWingNorm+pAngle->vyWingNorm)*0.1;
	xPlaneWingX+=pAngle->vxWingNorm*0.1;
	xPlaneWingY+=pAngle->vyWingNorm*0.1;
	xOrgPlaneBer=xPlaneBer;
	xOrgPlaneWingX=xPlaneWingX;
	xOrgPlaneWingY=xPlaneWingY;
	coord_trans(xBer,object_ucs,TRUE,TRUE);
	coord_trans(xWingX,object_ucs,TRUE,TRUE);
	coord_trans(xWingY,object_ucs,TRUE,TRUE);
	coord_trans(xPlaneBer,object_ucs,TRUE,TRUE);
	coord_trans(xPlaneWingX,object_ucs,TRUE,TRUE);
	coord_trans(xPlaneWingY,object_ucs,TRUE,TRUE);
	vector_trans(vxLocalWingNorm,object_ucs,TRUE,TRUE);
	vector_trans(vyLocalWingNorm,object_ucs,TRUE,TRUE);
	if(!vPlaneNorm.IsZero())
	{
		coord_trans(xLocalPlanePick,object_ucs,TRUE,TRUE);
		vector_trans(vLocalPlaneNorm,object_ucs,TRUE,TRUE);
		vLocalLineVec=pAngle->End()-pAngle->Start();
		vector_trans(vLocalLineVec,object_ucs,TRUE,TRUE);
	}
	//���ƽǸ���ָ���ڵ㴦��ͶӰ���漰�ؼ���
	PEN_STRU pen,highlightpen;
	pen.style=PS_SOLID;
	pen.width=4;
	pen.crColor=RGB(0,0,0);
	highlightpen=pen;
	highlightpen.crColor=RGB(255,0,0);
	highlightpen.width=6;
	HIBERID hiberid(pAngle->handle,HIBERARCHY(0,0,0,0));
	//up1=0: �ؼ���;down1:�ľ�
	//up1>0: �ؼ���λ��;up1-1Ϊ��ǰ�ؼ��������ľ�
	//		 down1:0(ʼ��),1(�ն�),����(�ڵ���)
	//		 down2:0Ĭ��(���ݽǸ�״̬���Զ�,����ΪͶӰ),1.��ڵ�ͬX����ƽ��,2.Y����ƽ��,3.Z����ƽ��
	//up2=0: X֫
	//up2=1: Y֫
	//up2=2: �� 
	long hNode=pNode->handle;
	//if(pAngle->pStart==pNode)
	//	hNode=0;
	//else if(pAngle->pEnd==pNode)
	//	hNode=1;
	IDbPoint* pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
	hiberid.SetHiberarchy(1,2,hNode,0);
	pKeyPoint->SetHiberId(hiberid);
	if(cbHighlightFlag&0x01||(xPlaneBer==xBer&&cbHighlightFlag&0x08))	//ͶӰ���
		pKeyPoint->SetPen(highlightpen);
	else
		pKeyPoint->SetPen(pen);
	pKeyPoint->SetPosition(xBer);
	//X֫���ߵ�
	pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
	hiberid.SetHiberarchy(1+gWingX,0,hNode,0);
	pKeyPoint->SetHiberId(hiberid);
	if(cbHighlightFlag&0x02||(xPlaneWingX==xWingX&&cbHighlightFlag&0x10))	//ͶӰX֫���ߵ�
		pKeyPoint->SetPen(highlightpen);
	else
		pKeyPoint->SetPen(pen);
	pKeyPoint->SetPosition(xWingX);
	pKeyPoint->SetUpperDirection(pAngle->vxWingNorm);
	pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
	hiberid.SetHiberarchy(1+gWingX,0,hNode,0);
	pKeyPoint->SetHiberId(hiberid);
	if(cbHighlightFlag&0x02||(xPlaneWingX==xWingX&&cbHighlightFlag&0x10))	//ͶӰX֫���ߵ�
		pKeyPoint->SetPen(highlightpen);
	else
		pKeyPoint->SetPen(pen);
	pKeyPoint->SetPosition(f3dPoint(xWingX)-vxLocalWingNorm*(pAngle->Thick+0.2));
	pKeyPoint->SetUpperDirection(-pAngle->vxWingNorm);
	//Y֫���ߵ�
	pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
	hiberid.SetHiberarchy(1+gWingY,1,hNode,0);
	pKeyPoint->SetHiberId(hiberid);
	if(cbHighlightFlag&0x04||(xPlaneWingY==xWingY&&cbHighlightFlag&0x20))	//ͶӰY֫���ߵ�
		pKeyPoint->SetPen(highlightpen);
	else
		pKeyPoint->SetPen(pen);
	pKeyPoint->SetPosition(xWingY);
	pKeyPoint->SetUpperDirection(pAngle->vyWingNorm);
	pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
	hiberid.SetHiberarchy(1+gWingY,1,hNode,0);
	pKeyPoint->SetHiberId(hiberid);
	if(cbHighlightFlag&0x04||(xPlaneWingY==xWingY&&cbHighlightFlag&0x20))	//ͶӰY֫���ߵ�
		pKeyPoint->SetPen(highlightpen);
	else
		pKeyPoint->SetPen(pen);
	pKeyPoint->SetPosition(f3dPoint(xWingY)-vyLocalWingNorm*(pAngle->Thick+0.2));
	pKeyPoint->SetUpperDirection(-pAngle->vyWingNorm);
	//���ƽǸ���ָ���ڵ㴦��ƽ�ƽ��漰�ؼ���
	if(ciIntersPlane=='X'||ciIntersPlane=='x')
		ciIntersPlane=1;
	else if(ciIntersPlane=='Y'||ciIntersPlane=='y')
		ciIntersPlane=2;
	else if(ciIntersPlane=='Z'||ciIntersPlane=='z')
		ciIntersPlane=3;
	else
		return;
	bool diffPlaneInters=false;
	if(xPlaneBer!=xBer)
	{
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1,2,hNode,ciIntersPlane);
		pKeyPoint->SetHiberId(hiberid);
		if(cbHighlightFlag&0x08)	//ƽ�����
			pKeyPoint->SetPen(highlightpen);
		else
			pKeyPoint->SetPen(pen);
		pKeyPoint->SetPosition(xPlaneBer);
		diffPlaneInters=true;
	}
	if(xPlaneWingX!=xWingX)
	{
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1+gWingX,0,hNode,ciIntersPlane);
		pKeyPoint->SetHiberId(hiberid);
		if(cbHighlightFlag&0x10)	//ͶӰX֫���ߵ�
			pKeyPoint->SetPen(highlightpen);
		else
			pKeyPoint->SetPen(pen);
		pKeyPoint->SetPosition(xPlaneWingX);
		pKeyPoint->SetUpperDirection(pAngle->vxWingNorm);
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1+gWingX,0,hNode,ciIntersPlane);
		pKeyPoint->SetHiberId(hiberid);
		if(cbHighlightFlag&0x10)	//ͶӰX֫���ߵ�
			pKeyPoint->SetPen(highlightpen);
		else
			pKeyPoint->SetPen(pen);
		GEPOINT xInternal=xPlaneWingX+vxLocalWingNorm*-(pAngle->Thick+0.2);
		Int3dlf(xInternal,xInternal,vLocalLineVec,xLocalPlanePick,vLocalPlaneNorm);
		pKeyPoint->SetPosition(xInternal);
		pKeyPoint->SetUpperDirection(-pAngle->vxWingNorm);
		diffPlaneInters=true;
	}
	if(xPlaneWingY!=xWingY)
	{
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1+gWingY,1,hNode,ciIntersPlane);
		pKeyPoint->SetHiberId(hiberid);
		if(cbHighlightFlag&0x20)	//ͶӰY֫���ߵ�
			pKeyPoint->SetPen(highlightpen);
		else
			pKeyPoint->SetPen(pen);
		pKeyPoint->SetPosition(xPlaneWingY);
		pKeyPoint->SetUpperDirection(pAngle->vyWingNorm);
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1+gWingY,1,hNode,ciIntersPlane);
		pKeyPoint->SetHiberId(hiberid);
		if(cbHighlightFlag&0x20)	//ͶӰY֫���ߵ�
			pKeyPoint->SetPen(highlightpen);
		else
			pKeyPoint->SetPen(pen);
		GEPOINT xInternal=xPlaneWingY+vyLocalWingNorm*-(pAngle->Thick+0.2);
		Int3dlf(xInternal,xInternal,vLocalLineVec,xLocalPlanePick,vLocalPlaneNorm);
		pKeyPoint->SetPosition(xInternal);
		pKeyPoint->SetUpperDirection(-pAngle->vyWingNorm);
		diffPlaneInters=true;
	}
	if(diffPlaneInters)
	{
		GEPOINT vertexArr[4];
		vertexArr[0]=xOrgPlaneBer;
		vertexArr[1].Set(xOrgPlaneWingX.x*2-xOrgPlaneBer.x,xOrgPlaneWingX.y*2-xOrgPlaneBer.y,xOrgPlaneWingX.z*2-xOrgPlaneBer.z);
		vertexArr[3].Set(xOrgPlaneWingY.x*2-xOrgPlaneBer.x,xOrgPlaneWingY.y*2-xOrgPlaneBer.y,xOrgPlaneWingY.z*2-xOrgPlaneBer.z);
		vertexArr[2]=vertexArr[1]+vertexArr[3]-f3dPoint(xOrgPlaneBer);
		GEPOINT v01=(vertexArr[0]-vertexArr[1])*0.618;
		GEPOINT v03=(vertexArr[0]-vertexArr[3])*0.618;
		vertexArr[0]+=v01+v03;
		vertexArr[1]+=v03;
		vertexArr[3]+=v01;
		double normal[3]={0.0};
		normal[ciIntersPlane-1]=1.0;
		g_pSolidDraw->NewWorkPlaneSketch(pAngle->handle,RGB(50,50,50),vertexArr,4,normal,"",0);
	}
}
void CLDSView::RedrawAngleFrameDrawing(CLDSLineAngle* pAngle,BYTE cbDrawMode/*=0*/,UCS_STRU *pObjectCS/*=NULL*/,bool removeOldEntities/*=true*/)
{
	//cbDrawMode:
	//	0.��ͨ����ʾǰ�����߼������ģʽ
	//	1.У��ģʽ�½Ǹ���Ĭ�Ϲؼ�����ʾģʽ
	//	2.ȫ���ؼ���͸����ʾ������������߼�����
	if(!CLDSLineAngle::m_bDisplaySolidAxisLine)
		return;
	UCS_STRU object_ucs;
	if(pObjectCS!=NULL)
		object_ucs=*pObjectCS;
	else
	{
		CDisplayView *pDispView=console.GetActiveView();
		if(pDispView==NULL)
			return;
		LoadDefaultUCS(&object_ucs);
		object_ucs.origin.Set();
		vector_trans(object_ucs.axis_x,pDispView->ucs,FALSE);
		vector_trans(object_ucs.axis_y,pDispView->ucs,FALSE);
		vector_trans(object_ucs.axis_z,pDispView->ucs,FALSE);
		coord_trans(object_ucs.origin,pDispView->ucs,FALSE);
	}

	IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
	GECS ocs(object_ucs.origin,object_ucs.axis_x,object_ucs.axis_y,object_ucs.axis_z);
	pBuddyDrawing->SetOCS(ocs);
	if(removeOldEntities)
	{
		pBuddyDrawing->DeleteDbEntityByHiberMasterId(pAngle->handle);
		g_pSolidDraw->DelWorkPlaneSketch(pAngle->handle);
	}

	f3dPoint align_vec=pAngle->End()-pAngle->Start();
	double length=align_vec.mod()+pAngle->startOdd()+pAngle->endOdd();
	normalize(align_vec);
	f3dPoint ptS=pAngle->Start()-align_vec*pAngle->startOdd();
	f3dPoint ptE=pAngle->End()+align_vec*pAngle->endOdd();
	f3dPoint wing_x_norm=pAngle->get_norm_x_wing();
	f3dPoint wing_y_norm=pAngle->get_norm_y_wing();
	f3dPoint wing_x_vec=pAngle->GetWingVecX();
	f3dPoint wing_y_vec=pAngle->GetWingVecY();
	f3dPoint wing_x_former_vec=wing_x_vec;
	f3dPoint wing_y_former_vec=wing_y_vec;
	f3dPoint y_extend_vec=wing_y_vec;
	vector_trans(y_extend_vec,pAngle->ucs,FALSE);
	double outterReservedWidth=0;
	if(y_extend_vec.x>0&&pAngle->m_fHeJiaoReservedWidth>0)
	{	//�Ͻ��ұ���һ���ĺϽǿ��
		f3dPoint bi_vec(1+y_extend_vec.x,y_extend_vec.y,0);
		normalize(bi_vec);	//��ƽ�ַ���
		double cos45=0.70710678118655,sin45=cos45;
		wing_x_former_vec.Set(bi_vec.x*cos45-bi_vec.y*-sin45,bi_vec.x*-sin45+bi_vec.y*cos45);
		wing_y_former_vec.Set(-wing_x_former_vec.y,wing_x_former_vec.x);
		vector_trans(wing_x_former_vec,pAngle->ucs,TRUE);
		vector_trans(wing_y_former_vec,pAngle->ucs,TRUE);
		double increment=pAngle->size_thick*tan(0.125*Pi-0.25*pAngle->GetRadWingAngle());
		if(pAngle->m_cHeJiaoReservedWidthType=='I')	//ָ��ֵΪ�ڲౣ�����
			outterReservedWidth=pAngle->m_fHeJiaoReservedWidth+pAngle->size_thick+increment;
		else
			outterReservedWidth=pAngle->m_fHeJiaoReservedWidth;
	}
	//up1=0: �ؼ���;down1:�ľ�
	//up1>0: �ؼ���λ��;up1-1Ϊ��ǰ�ؼ��������ľ�
	//		 down1: 0(ʼ��),1(�ն�),����(�ڵ���)
	//		 down2:0Ĭ��(���ݽǸ�״̬���Զ�,����ΪͶӰ),1.��ڵ�ͬX����ƽ��,2.Y����ƽ��,3.Z����ƽ��
	//up2=0: X֫
	//up2=1: Y֫
	HIBERID hiberid(pAngle->handle,HIBERARCHY(0,0,0,0));
	//���������֫�������
	f3dLine wing_x_thickline(ptS,ptE),wing_y_thickline(ptS,ptE);
	if(outterReservedWidth>0)
	{
		wing_x_thickline.startPt+=wing_x_former_vec*pAngle->size_thick-wing_y_former_vec*0.1;
		wing_x_thickline.endPt  +=wing_x_former_vec*pAngle->size_thick-wing_y_former_vec*0.1;
		wing_y_thickline.startPt+=wing_y_former_vec*pAngle->size_thick-wing_x_former_vec*0.1;
		wing_y_thickline.endPt  +=wing_y_former_vec*pAngle->size_thick-wing_x_former_vec*0.1;
	}
	else
	{
		wing_x_thickline.startPt+=wing_x_vec*pAngle->size_thick+wing_x_norm*0.1;
		wing_x_thickline.endPt  +=wing_x_vec*pAngle->size_thick+wing_x_norm*0.1;
		wing_y_thickline.startPt+=wing_y_vec*pAngle->size_thick+wing_y_norm*0.1;
		wing_y_thickline.endPt  +=wing_y_vec*pAngle->size_thick+wing_y_norm*0.1;
	}
	JGZJ jgzj_x=pAngle->GetZhunJu('X');
	JGZJ jgzj_y=pAngle->GetZhunJu('Y');
	coord_trans(wing_x_thickline.startPt,object_ucs,TRUE,TRUE);
	coord_trans(wing_x_thickline.endPt  ,object_ucs,TRUE,TRUE);
	coord_trans(wing_y_thickline.startPt,object_ucs,TRUE,TRUE);
	coord_trans(wing_y_thickline.endPt  ,object_ucs,TRUE,TRUE);
	PEN_STRU pen,pointpen;
	pen.style=PS_DASH;
	pen.width=1;
	pen.crColor=RGB(64,64,64);
	pointpen.width=4;
	pointpen.crColor=RGB(0,0,0);//255,255,255);

	IDbPoint* pKeyPoint=NULL;
	//������ʾʱ�ĺ������ wjh-2015.2.2
	IDbLine* pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
	pLine->SetHiberId(HIBERID(pAngle->handle));
	pLine->SetPen(pen);
	pLine->SetStartPosition(wing_x_thickline.startPt);
	pLine->SetEndPosition(wing_x_thickline.endPt);
	pLine->SetUpperDirection(wing_x_norm);	//������������
	pLine->SetSelectable(false);
	pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
	pLine->SetHiberId(HIBERID(pAngle->handle));
	pLine->SetPen(pen);
	pLine->SetStartPosition(wing_y_thickline.startPt);
	pLine->SetEndPosition(wing_y_thickline.endPt);
	pLine->SetUpperDirection(wing_y_norm);	//������������
	pLine->SetSelectable(false);
	//���������
	pen.crColor=RGB(0,0,0);
	pen.style=PS_SOLID;
	f3dLine arrow(ptS,ptE);
	arrow.startPt+=(wing_x_norm+wing_y_norm)*0.1;
	arrow.endPt  +=(wing_x_norm+wing_y_norm)*0.1;
	coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
	coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
	pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
	hiberid.SetHiberarchy(0,0,0,0);	//����
	pLine->SetHiberId(hiberid);
	pLine->SetPen(pen);
	pLine->SetStartPosition(arrow.startPt);
	pLine->SetEndPosition(arrow.endPt);
	if(cbDrawMode<0x03)
		pLine->SetSelectable(false);
	if(cbDrawMode>0)
	{
		//ʼ�����
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1,2,0,0);
		pKeyPoint->SetHiberId(hiberid);
		pKeyPoint->SetPen(pointpen);
		pKeyPoint->SetPosition(arrow.startPt);
		if(cbDrawMode<0x03)
			pKeyPoint->SetSelectable(false);
		//�ն����
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1,2,1,0);
		pKeyPoint->SetHiberId(hiberid);
		pKeyPoint->SetPen(pointpen);
		pKeyPoint->SetPosition(arrow.endPt);
		if(cbDrawMode<0x03)
			pKeyPoint->SetSelectable(false);
	}
	//֫����λ�ü���
	if(cbDrawMode>0)
	{
		f3dPoint start_wing_x_tip[2],start_wing_y_tip[2],end_wing_x_tip[2],end_wing_y_tip[2];
		start_wing_x_tip[0]=start_wing_x_tip[1]=ptS+wing_x_former_vec*outterReservedWidth;
		start_wing_y_tip[0]=start_wing_y_tip[1]=ptS+wing_y_former_vec*outterReservedWidth;
		end_wing_x_tip[0]=end_wing_x_tip[1]=ptE+wing_x_former_vec*outterReservedWidth;
		end_wing_y_tip[0]=end_wing_y_tip[1]=ptE+wing_y_former_vec*outterReservedWidth;
		start_wing_x_tip[0]+=wing_x_vec*(pAngle->size_wide-outterReservedWidth)+wing_x_norm*0.1;
		start_wing_x_tip[1] =start_wing_x_tip[0]-wing_x_norm*(pAngle->size_thick+0.2);
		start_wing_y_tip[0]+=wing_y_vec*(pAngle->size_wide-outterReservedWidth)+wing_y_norm*0.1;
		start_wing_y_tip[1] =start_wing_y_tip[0]-wing_y_norm*(pAngle->size_thick+0.2);
		end_wing_x_tip[0]+=wing_x_vec*(pAngle->size_wide-outterReservedWidth)+wing_x_norm*0.1;
		end_wing_x_tip[1] =end_wing_x_tip[0]-wing_x_norm*(pAngle->size_thick+0.2);
		end_wing_y_tip[0]+=wing_y_vec*(pAngle->size_wide-outterReservedWidth)+wing_y_norm*0.1;
		end_wing_y_tip[1] =end_wing_y_tip[0]-wing_y_norm*(pAngle->size_thick+0.2);
		for(int i=0;i<2;i++)
		{
			coord_trans(start_wing_x_tip[i],object_ucs,TRUE,TRUE);
			coord_trans(start_wing_y_tip[i],object_ucs,TRUE,TRUE);
			coord_trans(end_wing_x_tip[i],object_ucs,TRUE,TRUE);
			coord_trans(end_wing_y_tip[i],object_ucs,TRUE,TRUE);
		}
		//ʼ��֫����
		int size_wide=(DWORD)pAngle->size_wide;
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1+size_wide,0,0,0);
		pKeyPoint->SetHiberId(hiberid);
		pKeyPoint->SetPen(pointpen);
		pKeyPoint->SetPosition(start_wing_x_tip[0]);
		if(cbDrawMode<0x03)
		{
			pKeyPoint->SetUpperDirection(wing_x_norm);
			pKeyPoint->SetSelectable(false);
		}
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1+size_wide,0,0,0);
		pKeyPoint->SetHiberId(hiberid);
		pKeyPoint->SetPen(pointpen);
		pKeyPoint->SetPosition(start_wing_x_tip[1]);
		if(cbDrawMode<0x03)
		{
			pKeyPoint->SetUpperDirection(-wing_x_norm);
			pKeyPoint->SetSelectable(false);
		}

		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1+size_wide,1,0,0);
		pKeyPoint->SetHiberId(hiberid);
		pKeyPoint->SetPen(pointpen);
		pKeyPoint->SetPosition(start_wing_y_tip[0]);
		if(cbDrawMode<0x03)
		{
			pKeyPoint->SetUpperDirection(wing_y_norm);
			pKeyPoint->SetSelectable(false);
		}
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1+size_wide,1,0,0);
		pKeyPoint->SetHiberId(hiberid);
		pKeyPoint->SetPen(pointpen);
		pKeyPoint->SetPosition(start_wing_y_tip[1]);
		if(cbDrawMode<0x03)
		{
			pKeyPoint->SetUpperDirection(-wing_y_norm);
			pKeyPoint->SetSelectable(false);
		}
		//�ն�֫����
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1+size_wide,0,1,0);
		pKeyPoint->SetHiberId(hiberid);
		pKeyPoint->SetPen(pointpen);
		pKeyPoint->SetPosition(end_wing_x_tip[0]);
		if(cbDrawMode<0x03)
		{
			pKeyPoint->SetUpperDirection(wing_x_norm);
			pKeyPoint->SetSelectable(false);
		}
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1+size_wide,0,1,0);
		pKeyPoint->SetHiberId(hiberid);
		pKeyPoint->SetPen(pointpen);
		pKeyPoint->SetPosition(end_wing_x_tip[1]);
		if(cbDrawMode<0x03)
		{
			pKeyPoint->SetUpperDirection(-wing_x_norm);
			pKeyPoint->SetSelectable(false);
		}

		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1+size_wide,1,1,0);
		pKeyPoint->SetHiberId(hiberid);
		pKeyPoint->SetPen(pointpen);
		pKeyPoint->SetPosition(end_wing_y_tip[0]);
		if(cbDrawMode<0x03)
		{
			pKeyPoint->SetUpperDirection(wing_y_norm);
			pKeyPoint->SetSelectable(false);
		}
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1+size_wide,1,1,0);
		pKeyPoint->SetHiberId(hiberid);
		pKeyPoint->SetPen(pointpen);
		pKeyPoint->SetPosition(end_wing_y_tip[1]);
		if(cbDrawMode<0x03)
		{
			pKeyPoint->SetUpperDirection(-wing_y_norm);
			pKeyPoint->SetSelectable(false);
		}
	}
	//���X֫�������
	pen.crColor=RGB(255,0,0);
	pen.style=PS_DASHDOT;
	arrow.startPt=ptS;
	arrow.endPt  =ptE;
	if(outterReservedWidth>0)
	{	//�кϽǱ������
		if(jgzj_x.g>outterReservedWidth)
		{	//׼���ڱ��������
			arrow.startPt+=wing_x_former_vec*outterReservedWidth+wing_x_vec*(jgzj_x.g-outterReservedWidth)+wing_x_norm*0.1;
			arrow.endPt  +=wing_x_former_vec*outterReservedWidth+wing_x_vec*(jgzj_x.g-outterReservedWidth)+wing_x_norm*0.1;
		}
		else
		{	//׼���ڱ��������
			arrow.startPt+=wing_x_former_vec*jgzj_x.g-wing_y_former_vec*0.1;
			arrow.endPt  +=wing_x_former_vec*jgzj_x.g-wing_y_former_vec*0.1;
		}
	}
	else
	{
		arrow.startPt+=wing_x_vec*jgzj_x.g+wing_x_norm*0.1;
		arrow.endPt  +=wing_x_vec*jgzj_x.g+wing_x_norm*0.1;
	}
	coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
	coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
	pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
	hiberid.SetHiberarchy(0,0,jgzj_x.g);
	pLine->SetHiberId(hiberid);
	pLine->SetPen(pen);
	pLine->SetStartPosition(arrow.startPt);
	pLine->SetEndPosition(arrow.endPt);
	pLine->SetSelectLevel(2);	//һ��������ܱ�ѡ�У�ֻ����������¿��Ա�ѡ��
	if(cbDrawMode<0x03)
	{
		pLine->SetUpperDirection(wing_x_norm);
	}
	if(cbDrawMode>0)
	{
		//ʼ��X֫��ඥ��
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1,0,0,0);
		pKeyPoint->SetHiberId(hiberid);
		pKeyPoint->SetPen(pointpen);
		pKeyPoint->SetPosition(arrow.startPt);
		if(cbDrawMode<0x03)
		{
			pKeyPoint->SetUpperDirection(wing_x_norm);
			pKeyPoint->SetSelectable(false);
		}
		//�ն�X֫��ඥ��
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1,0,1,0);
		pKeyPoint->SetHiberId(hiberid);
		pKeyPoint->SetPen(pointpen);
		pKeyPoint->SetPosition(arrow.endPt);
		if(cbDrawMode<0x02)
		{
			pKeyPoint->SetUpperDirection(wing_x_norm);
			pKeyPoint->SetSelectable(false);
		}
	}
	//���X֫�ڲ�����
	arrow.startPt=ptS;
	arrow.endPt  =ptE;
	if(outterReservedWidth>0)
	{	//�кϽǱ������
		if(jgzj_x.g>outterReservedWidth)
		{	//׼���ڱ��������
			arrow.startPt+=wing_x_former_vec*outterReservedWidth+wing_x_vec*(jgzj_x.g-outterReservedWidth)-wing_x_norm*(pAngle->size_thick+0.1);
			arrow.endPt  +=wing_x_former_vec*outterReservedWidth+wing_x_vec*(jgzj_x.g-outterReservedWidth)-wing_x_norm*(pAngle->size_thick+0.1);
		}
		else
		{	//׼���ڱ��������
			arrow.startPt+=wing_x_former_vec*jgzj_x.g+wing_y_former_vec*(pAngle->size_thick+0.1);
			arrow.endPt  +=wing_x_former_vec*jgzj_x.g+wing_y_former_vec*(pAngle->size_thick+0.1);
		}
	}
	else
	{
		arrow.startPt+=wing_x_vec*jgzj_x.g-wing_x_norm*(pAngle->size_thick+0.1);
		arrow.endPt  +=wing_x_vec*jgzj_x.g-wing_x_norm*(pAngle->size_thick+0.1);
	}
	coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
	coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
	pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
	hiberid.SetHiberarchy(0,0,jgzj_x.g);
	pLine->SetHiberId(hiberid);
	pLine->SetPen(pen);
	pLine->SetStartPosition(arrow.startPt);
	pLine->SetEndPosition(arrow.endPt);
	pLine->SetSelectLevel(2);	//һ��������ܱ�ѡ�У�ֻ����������¿��Ա�ѡ��
	if(cbDrawMode<0x03)
	{
		pLine->SetUpperDirection(-wing_x_norm);
	}
	if(cbDrawMode>0)
	{
		//ʼ��X֫�ڲඥ��
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1,0,0,0);
		pKeyPoint->SetHiberId(hiberid);
		pKeyPoint->SetPen(pointpen);
		pKeyPoint->SetPosition(arrow.startPt);
		if(cbDrawMode<0x02)
		{
			pKeyPoint->SetUpperDirection(-wing_x_norm);
			pKeyPoint->SetSelectable(false);
		}
		//�ն�X֫�ڲඥ��
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1,0,1,0);
		pKeyPoint->SetHiberId(hiberid);
		pKeyPoint->SetPen(pointpen);
		pKeyPoint->SetPosition(arrow.endPt);
		if(cbDrawMode<0x02)
		{
			pKeyPoint->SetUpperDirection(-wing_x_norm);
			pKeyPoint->SetSelectable(false);
		}
	}
	//���Y֫�������
	pen.crColor=RGB(30,185,30);
	arrow.startPt=ptS;
	arrow.endPt  =ptE;
	if(outterReservedWidth>0)
	{
		if(jgzj_y.g>outterReservedWidth)
		{	//׼���ڱ��������
			arrow.startPt+=wing_y_former_vec*outterReservedWidth+wing_y_vec*(jgzj_y.g-outterReservedWidth)+wing_y_norm*0.1;
			arrow.endPt  +=wing_y_former_vec*outterReservedWidth+wing_y_vec*(jgzj_y.g-outterReservedWidth)+wing_y_norm*0.1;
		}
		else
		{	//׼���ڱ��������
			arrow.startPt+=wing_y_former_vec*jgzj_y.g-wing_x_former_vec*0.1;
			arrow.endPt  +=wing_y_former_vec*jgzj_y.g-wing_x_former_vec*0.1;
		}
	}
	else
	{
		arrow.startPt+=wing_y_vec*jgzj_y.g+wing_y_norm*0.1;
		arrow.endPt  +=wing_y_vec*jgzj_y.g+wing_y_norm*0.1;
	}
	coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
	coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
	pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
	hiberid.SetHiberarchy(0,1,jgzj_y.g);
	pLine->SetHiberId(hiberid);
	pLine->SetPen(pen);
	pLine->SetStartPosition(arrow.startPt);
	pLine->SetEndPosition(arrow.endPt);
	pLine->SetSelectLevel(2);	//һ��������ܱ�ѡ�У�ֻ����������¿��Ա�ѡ��
	if(cbDrawMode<0x03)
	{
		pLine->SetUpperDirection(wing_y_norm);
	}
	if(cbDrawMode>0)
	{
		//ʼ��Y֫��ඥ��
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1,1,0,0);
		pKeyPoint->SetHiberId(hiberid);
		pKeyPoint->SetPen(pointpen);
		pKeyPoint->SetPosition(arrow.startPt);
		if(cbDrawMode<0x02)
		{
			pKeyPoint->SetUpperDirection(wing_y_norm);
			pKeyPoint->SetSelectable(false);
		}
		//�ն�Y֫��ඥ��
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1,1,1,0);
		pKeyPoint->SetHiberId(hiberid);
		pKeyPoint->SetPen(pointpen);
		pKeyPoint->SetPosition(arrow.endPt);
		if(cbDrawMode<0x02)
		{
			pKeyPoint->SetUpperDirection(wing_y_norm);
			pKeyPoint->SetSelectable(false);
		}
	}
	//���Y֫�ڲ�����
	arrow.startPt=ptS;
	arrow.endPt  =ptE;
	if(outterReservedWidth>0)
	{
		if(jgzj_y.g>outterReservedWidth)
		{	//׼���ڱ��������
			arrow.startPt+=wing_y_former_vec*outterReservedWidth+wing_y_vec*(jgzj_y.g-outterReservedWidth)-wing_y_norm*(pAngle->size_thick+0.1);
			arrow.endPt  +=wing_y_former_vec*outterReservedWidth+wing_y_vec*(jgzj_y.g-outterReservedWidth)-wing_y_norm*(pAngle->size_thick+0.1);
		}
		else
		{	//׼���ڱ��������
			arrow.startPt+=wing_y_former_vec*jgzj_y.g+wing_x_former_vec*(pAngle->size_thick+0.1);
			arrow.endPt  +=wing_y_former_vec*jgzj_y.g+wing_x_former_vec*(pAngle->size_thick+0.1);
		}
	}
	else
	{
		arrow.startPt+=wing_y_vec*jgzj_y.g-wing_y_norm*(pAngle->size_thick+0.1);
		arrow.endPt  +=wing_y_vec*jgzj_y.g-wing_y_norm*(pAngle->size_thick+0.1);
	}
	coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
	coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
	pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
	hiberid.SetHiberarchy(0,1,jgzj_y.g);
	pLine->SetHiberId(hiberid);
	pLine->SetPen(pen);
	pLine->SetStartPosition(arrow.startPt);
	pLine->SetEndPosition(arrow.endPt);
	pLine->SetSelectLevel(2);	//һ��������ܱ�ѡ�У�ֻ����������¿��Ա�ѡ��
	if(cbDrawMode<0x03)
	{
		pLine->SetUpperDirection(-wing_y_norm);
	}
	if(cbDrawMode>0)
	{
		//ʼ��Y֫�ڲඥ��
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1,1,0,0);
		pKeyPoint->SetHiberId(hiberid);
		pKeyPoint->SetPen(pointpen);
		pKeyPoint->SetPosition(arrow.startPt);
		if(cbDrawMode<0x02)
		{
			pKeyPoint->SetUpperDirection(-wing_y_norm);
			pKeyPoint->SetSelectable(false);
		}
		//�ն�Y֫�ڲඥ��
		pKeyPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		hiberid.SetHiberarchy(1,1,1,0);
		pKeyPoint->SetHiberId(hiberid);
		pKeyPoint->SetPen(pointpen);
		pKeyPoint->SetPosition(arrow.endPt);
		if(cbDrawMode<0x02)
		{
			pKeyPoint->SetUpperDirection(-wing_y_norm);
			pKeyPoint->SetSelectable(false);
		}
	}
	//ʵ��״̬�µ�ʼ->�շ�����
	if(CLDSLinePart::bDisplayLineVecMark)
	{
		pen.style=PS_SOLID;
		double startspace=min(130,0.1*length);
		f3dPoint dimstartpos_x=pAngle->Start()+align_vec*(startspace+20-pAngle->startOdd());
		f3dPoint dimstartpos_y=pAngle->Start()+align_vec*(startspace+20-pAngle->startOdd());
		if(outterReservedWidth>0)
		{
			if(jgzj_y.g>outterReservedWidth)
			{	//׼���ڱ��������
				dimstartpos_x+=wing_x_former_vec*outterReservedWidth+wing_x_vec*(jgzj_x.g-outterReservedWidth);
				dimstartpos_y+=wing_y_former_vec*outterReservedWidth+wing_y_vec*(jgzj_y.g-outterReservedWidth);
			}
			else
			{	//׼���ڱ��������
				dimstartpos_x+=wing_x_former_vec*jgzj_x.g;
				dimstartpos_y+=wing_y_former_vec*jgzj_y.g;
			}
		}
		else
		{
			dimstartpos_x+=wing_x_vec*jgzj_x.g;
			dimstartpos_y+=wing_y_vec*jgzj_y.g;
		}
		f3dLine arrowx1(dimstartpos_x,dimstartpos_x-align_vec*20+wing_x_vec*10);
		f3dLine arrowx2(dimstartpos_x,dimstartpos_x-align_vec*20-wing_x_vec*10);
		f3dLine arrowy1(dimstartpos_y,dimstartpos_y-align_vec*20+wing_y_vec*10);
		f3dLine arrowy2(dimstartpos_y,dimstartpos_y-align_vec*20-wing_y_vec*10);
		double rgn_length=length-startspace*2-20;
		int slices=min(10,max(1,ftoi(rgn_length/200)));
		double step=max(1,rgn_length/slices);
		for(int i=0;i<=slices;i++)
		{
			pen.crColor=RGB(255,0,0);
			//���X֫����ͷ
			pen.style=PS_SOLID;
			arrow.startPt=arrowx1.startPt+align_vec*(i*step)+wing_x_norm*0.1;
			arrow.endPt  =arrowx1.endPt  +align_vec*(i*step)+wing_x_norm*0.1;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
			pLine->SetHiberId(HIBERID(pAngle->handle));
			pLine->SetPen(pen);
			pLine->SetStartPosition(arrow.startPt);
			pLine->SetEndPosition(arrow.endPt);
			if(cbDrawMode<0x03)
				pLine->SetUpperDirection(wing_x_norm);
			pLine->SetSelectable(false);
			arrow.startPt=arrowx2.startPt+align_vec*(i*step)+wing_x_norm*0.1;
			arrow.endPt  =arrowx2.endPt  +align_vec*(i*step)+wing_x_norm*0.1;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
			pLine->SetHiberId(HIBERID(pAngle->handle));
			pLine->SetPen(pen);
			pLine->SetStartPosition(arrow.startPt);
			pLine->SetEndPosition(arrow.endPt);
			if(cbDrawMode<0x03)
				pLine->SetUpperDirection(wing_x_norm);
			pLine->SetSelectable(false);
			//���X֫�ڲ��ͷ
			arrow.startPt=arrowx1.startPt+align_vec*(i*step)-(pAngle->size_thick+0.1)*wing_x_norm;
			arrow.endPt  =arrowx1.endPt  +align_vec*(i*step)-(pAngle->size_thick+0.1)*wing_x_norm;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
			pLine->SetHiberId(HIBERID(pAngle->handle));
			pLine->SetPen(pen);
			pLine->SetStartPosition(arrow.startPt);
			pLine->SetEndPosition(arrow.endPt);
			if(cbDrawMode<0x03)
				pLine->SetUpperDirection(-wing_x_norm);
			pLine->SetSelectable(false);
			arrow.startPt=arrowx2.startPt+align_vec*(i*step)-(pAngle->size_thick+0.1)*wing_x_norm;
			arrow.endPt  =arrowx2.endPt  +align_vec*(i*step)-(pAngle->size_thick+0.1)*wing_x_norm;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
			pLine->SetHiberId(HIBERID(pAngle->handle));
			pLine->SetPen(pen);
			pLine->SetStartPosition(arrow.startPt);
			pLine->SetEndPosition(arrow.endPt);
			if(cbDrawMode<0x03)
				pLine->SetUpperDirection(-wing_x_norm);
			pLine->SetSelectable(false);
			//���Y֫����ͷ
			pen.crColor=RGB(0,255,0);
			arrow.startPt=arrowy1.startPt+align_vec*(i*step)+wing_y_norm*0.1;
			arrow.endPt  =arrowy1.endPt  +align_vec*(i*step)+wing_y_norm*0.1;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
			pLine->SetHiberId(HIBERID(pAngle->handle));
			pLine->SetPen(pen);
			pLine->SetStartPosition(arrow.startPt);
			pLine->SetEndPosition(arrow.endPt);
			if(cbDrawMode<0x03)
				pLine->SetUpperDirection(wing_y_norm);
			pLine->SetSelectable(false);
			arrow.startPt=arrowy2.startPt+align_vec*(i*step)+wing_y_norm*0.1;
			arrow.endPt  =arrowy2.endPt  +align_vec*(i*step)+wing_y_norm*0.1;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
			pLine->SetHiberId(HIBERID(pAngle->handle));
			pLine->SetPen(pen);
			pLine->SetStartPosition(arrow.startPt);
			pLine->SetEndPosition(arrow.endPt);
			if(cbDrawMode<0x03)
				pLine->SetUpperDirection(wing_y_norm);
			pLine->SetSelectable(false);
			//���Y֫�ڲ��ͷ
			arrow.startPt=arrowy1.startPt+align_vec*(i*step)-(pAngle->size_thick+0.1)*wing_y_norm;
			arrow.endPt  =arrowy1.endPt  +align_vec*(i*step)-(pAngle->size_thick+0.1)*wing_y_norm;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
			pLine->SetHiberId(HIBERID(pAngle->handle));
			pLine->SetPen(pen);
			pLine->SetStartPosition(arrow.startPt);
			pLine->SetEndPosition(arrow.endPt);
			if(cbDrawMode<0x03)
				pLine->SetUpperDirection(-wing_y_norm);
			pLine->SetSelectable(false);
			arrow.startPt=arrowy2.startPt+align_vec*(i*step)-(pAngle->size_thick+0.1)*wing_y_norm;
			arrow.endPt  =arrowy2.endPt  +align_vec*(i*step)-(pAngle->size_thick+0.1)*wing_y_norm;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
			pLine->SetHiberId(HIBERID(pAngle->handle));
			pLine->SetPen(pen);
			pLine->SetStartPosition(arrow.startPt);
			pLine->SetEndPosition(arrow.endPt);
			if(cbDrawMode<0x03)
				pLine->SetUpperDirection(-wing_y_norm);
			pLine->SetSelectable(false);
		}
	}
}
void CLDSView::RedrawFlatFrameDrawing(CLDSLineFlat* pFlat,BYTE cbDrawMode/*=0*/,UCS_STRU *pObjectCS/*=NULL*/,bool removeOldEntities/*=true*/)
{
	//cbDrawMode:
	//	0.��ͨ����ʾǰ�����߼������ģʽ
	//	1.У��ģʽ�½Ǹ���Ĭ�Ϲؼ�����ʾģʽ
	//	2.ȫ���ؼ���͸����ʾ������������߼�����
	if(!CLDSLineAngle::m_bDisplaySolidAxisLine)
		return;
	//���û�ͼ����ϵ
	UCS_STRU object_ucs;
	if(pObjectCS!=NULL)
		object_ucs=*pObjectCS;
	else
	{
		CDisplayView *pDispView=console.GetActiveView();
		if(pDispView==NULL)
			return;
		LoadDefaultUCS(&object_ucs);
		object_ucs.origin.Set();
		vector_trans(object_ucs.axis_x,pDispView->ucs,FALSE);
		vector_trans(object_ucs.axis_y,pDispView->ucs,FALSE);
		vector_trans(object_ucs.axis_z,pDispView->ucs,FALSE);
		coord_trans(object_ucs.origin,pDispView->ucs,FALSE);
	}
	GECS ocs(object_ucs.origin,object_ucs.axis_x,object_ucs.axis_y,object_ucs.axis_z);
	IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
	if(pBuddyDrawing==NULL)
		return;
	pBuddyDrawing->SetOCS(ocs);
	if(removeOldEntities)
	{
		pBuddyDrawing->DeleteDbEntityByHiberMasterId(pFlat->handle);
		g_pSolidDraw->DelWorkPlaneSketch(pFlat->handle);
	}
	//����������
	f3dPoint align_vec=pFlat->End()-pFlat->Start();
	double length=align_vec.mod()+pFlat->startOdd()+pFlat->endOdd();
	normalize(align_vec);
	f3dPoint ptS=pFlat->Start()-align_vec*pFlat->startOdd();
	f3dPoint ptE=pFlat->End()+align_vec*pFlat->endOdd();
	f3dPoint norm=pFlat->ucs.axis_z;
	f3dPoint vecY=pFlat->ucs.axis_y;
	f3dLine arrow;
	//����������
	PEN_STRU pen;
	pen.width=1;
	pen.style=PS_DASHDOT;
	pen.crColor=RGB(0,255,0);
	arrow.startPt=ptS-norm*0.1;
	arrow.endPt  =ptE-norm*0.1;
	coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
	coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
	IDbLine* pLine=AppendDbLine(arrow,pen,HIBERID(pFlat->handle),-norm,true);
	pLine->SetSelectLevel(2);
	//����ڲ�����
	arrow.startPt=ptS+norm*(pFlat->size_thick+0.1);
	arrow.endPt  =ptE+norm*(pFlat->size_thick+0.1);
	coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
	coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
	pLine=AppendDbLine(arrow,pen,HIBERID(pFlat->handle),norm,true);
	pLine->SetSelectLevel(2);
	//ʵ��״̬�µ�ʼ->�շ�����
	if(CLDSLinePart::bDisplayLineVecMark)
	{
		pen.style=PS_SOLID;
		double startspace=min(130,0.1*length);
		f3dPoint dim_pos=ptS+align_vec*(startspace+20-pFlat->startOdd());
		f3dLine arrowx1(dim_pos,dim_pos-align_vec*20+vecY*10);
		f3dLine arrowx2(dim_pos,dim_pos-align_vec*20-vecY*10);
		double rgn_length=length-startspace*2-20;
		int slices=min(10,max(1,ftoi(rgn_length/200)));
		double step=max(1,rgn_length/slices);
		for(int i=0;i<=slices;i++)
		{
			//�������ͷ
			arrow.startPt=arrowx1.startPt+align_vec*(i*step)-norm*0.1;
			arrow.endPt  =arrowx1.endPt  +align_vec*(i*step)-norm*0.1;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			AppendDbLine(arrow,pen,HIBERID(pFlat->handle),-norm);
			arrow.startPt=arrowx2.startPt+align_vec*(i*step)-norm*0.1;
			arrow.endPt  =arrowx2.endPt  +align_vec*(i*step)-norm*0.1;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			AppendDbLine(arrow,pen,HIBERID(pFlat->handle),-norm);
			//����ڲ��ͷ
			arrow.startPt=arrowx1.startPt+align_vec*(i*step)+norm*(pFlat->size_thick+0.1);
			arrow.endPt  =arrowx1.endPt  +align_vec*(i*step)+norm*(pFlat->size_thick+0.1);
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			AppendDbLine(arrow,pen,HIBERID(pFlat->handle),norm);
			arrow.startPt=arrowx2.startPt+align_vec*(i*step)+norm*(pFlat->size_thick+0.1);
			arrow.endPt  =arrowx2.endPt  +align_vec*(i*step)+norm*(pFlat->size_thick+0.1);
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			AppendDbLine(arrow,pen,HIBERID(pFlat->handle),norm);
		}
	}
}
void CLDSView::RedrawSlotFrameDrawing(CLDSLineSlot* pSlot,BYTE cbDrawMode/*=0*/,UCS_STRU *pObjectCS/*=NULL*/,bool removeOldEntities/*=true*/)
{
	//cbDrawMode:
	//	0.��ͨ����ʾǰ�����߼������ģʽ
	//	1.У��ģʽ�½Ǹ���Ĭ�Ϲؼ�����ʾģʽ
	//	2.ȫ���ؼ���͸����ʾ������������߼�����
	if(!CLDSLineAngle::m_bDisplaySolidAxisLine)
		return;
	//���û�ͼ����ϵ
	UCS_STRU object_ucs;
	if(pObjectCS!=NULL)
		object_ucs=*pObjectCS;
	else
	{
		CDisplayView *pDispView=console.GetActiveView();
		if(pDispView==NULL)
			return;
		LoadDefaultUCS(&object_ucs);
		object_ucs.origin.Set();
		vector_trans(object_ucs.axis_x,pDispView->ucs,FALSE);
		vector_trans(object_ucs.axis_y,pDispView->ucs,FALSE);
		vector_trans(object_ucs.axis_z,pDispView->ucs,FALSE);
		coord_trans(object_ucs.origin,pDispView->ucs,FALSE);
	}

	IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
	GECS ocs(object_ucs.origin,object_ucs.axis_x,object_ucs.axis_y,object_ucs.axis_z);
	pBuddyDrawing->SetOCS(ocs);
	if(removeOldEntities)
	{
		pBuddyDrawing->DeleteDbEntityByHiberMasterId(pSlot->handle);
		g_pSolidDraw->DelWorkPlaneSketch(pSlot->handle);
	}
	//����������
	f3dPoint align_vec=pSlot->End()-pSlot->Start();
	double length=align_vec.mod()+pSlot->startOdd()+pSlot->endOdd();
	normalize(align_vec);
	f3dPoint ptS=pSlot->Start()-align_vec*pSlot->startOdd();
	f3dPoint ptE=pSlot->End()+align_vec*pSlot->endOdd();
	f3dPoint wing_x_norm=pSlot->ucs.axis_x;
	f3dPoint wing_y_norm=-1*wing_x_norm;
	f3dPoint wing_vec=pSlot->ucs.axis_y;
	f3dLine ber_line_x,ber_line_y,thick_line,arrow;
	ber_line_x.startPt=ptS+wing_x_norm*pSlot->size_height*0.5;
	ber_line_x.endPt=ptE+wing_x_norm*pSlot->size_height*0.5;
	ber_line_y.startPt=ptS+wing_y_norm*pSlot->size_height*0.5;
	ber_line_y.endPt=ptE+wing_y_norm*pSlot->size_height*0.5;
	//����X֫�������
	PEN_STRU pen;
	pen.width=1;
	pen.style=PS_DASH;
	pen.crColor=RGB(64,64,64);
	thick_line.startPt=ber_line_x.startPt+wing_vec*pSlot->size_thick+wing_x_norm*0.1;
	thick_line.endPt=ber_line_x.endPt+wing_vec*pSlot->size_thick+wing_x_norm*0.1;
	coord_trans(thick_line.startPt,object_ucs,TRUE,TRUE);
	coord_trans(thick_line.endPt  ,object_ucs,TRUE,TRUE);
	AppendDbLine(thick_line,pen,HIBERID(pSlot->handle),wing_x_norm);
	thick_line.startPt=ber_line_x.startPt-wing_x_norm*pSlot->size_thick-wing_vec*0.1;
	thick_line.endPt=ber_line_x.endPt-wing_x_norm*pSlot->size_thick-wing_vec*0.1;
	coord_trans(thick_line.startPt,object_ucs,TRUE,TRUE);
	coord_trans(thick_line.endPt  ,object_ucs,TRUE,TRUE);
	AppendDbLine(thick_line,pen,HIBERID(pSlot->handle),-wing_vec);
	//����Y֫�������
	thick_line.startPt=ber_line_y.startPt+wing_vec*pSlot->size_thick+wing_y_norm*0.1;
	thick_line.endPt=ber_line_y.endPt+wing_vec*pSlot->size_thick+wing_y_norm*0.1;
	coord_trans(thick_line.startPt,object_ucs,TRUE,TRUE);
	coord_trans(thick_line.endPt  ,object_ucs,TRUE,TRUE);
	AppendDbLine(thick_line,pen,HIBERID(pSlot->handle),wing_y_norm);
	thick_line.startPt=ber_line_y.startPt-wing_y_norm*pSlot->size_thick-wing_vec*0.1;
	thick_line.endPt=ber_line_y.endPt-wing_y_norm*pSlot->size_thick-wing_vec*0.1;
	coord_trans(thick_line.startPt,object_ucs,TRUE,TRUE);
	coord_trans(thick_line.endPt  ,object_ucs,TRUE,TRUE);
	AppendDbLine(thick_line,pen,HIBERID(pSlot->handle),-wing_vec);
	//��Ӳ��������(up2=0:����|up2=1:Y֫|up2=2:Y֫)
	HIBERID hiberid(pSlot->handle,HIBERARCHY(0,0,0,0));
	pen.crColor=RGB(255,0,0);
	pen.style=PS_DASHDOT;
	hiberid.SetHiberarchy(0,0,0,0);
	arrow.startPt=ptS-wing_vec*0.1;
	arrow.endPt  =ptE-wing_vec*0.1;
	coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
	coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
	IDbLine* pLine=AppendDbLine(arrow,pen,hiberid,-wing_vec,true);
	pLine->SetSelectLevel(2);
	//��Ӳ��ڲ�����
	arrow.startPt=ptS+wing_vec*(0.1+pSlot->size_thick);
	arrow.endPt  =ptE+wing_vec*(0.1+pSlot->size_thick);
	coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
	coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
	pLine=AppendDbLine(arrow,pen,hiberid,wing_vec,true);
	pLine->SetSelectLevel(2);
	//���X֫�������
	pen.crColor=RGB(0,255,0);
	hiberid.SetHiberarchy(0,1,0,0);
	arrow.startPt=ber_line_x.startPt+wing_vec*pSlot->size_wide*0.5+wing_x_norm*0.1;
	arrow.endPt  =ber_line_x.endPt+wing_vec*pSlot->size_wide*0.5+wing_x_norm*0.1;
	coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
	coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
	pLine=AppendDbLine(arrow,pen,hiberid,wing_x_norm,true);
	pLine->SetSelectLevel(2);
	//���X֫�ڲ�����
	arrow.startPt=ber_line_x.startPt+wing_vec*pSlot->size_wide*0.5-wing_x_norm*(0.1+pSlot->size_thick);
	arrow.endPt  =ber_line_x.endPt+wing_vec*pSlot->size_wide*0.5-wing_x_norm*(0.1+pSlot->size_thick);
	coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
	coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
	pLine=AppendDbLine(arrow,pen,hiberid,-wing_x_norm,true);
	pLine->SetSelectLevel(2);
	//���Y֫�������
	pen.crColor=RGB(0,255,0);
	hiberid.SetHiberarchy(0,2,0,0);
	arrow.startPt=ber_line_y.startPt+wing_vec*pSlot->size_wide*0.5+wing_y_norm*0.1;
	arrow.endPt  =ber_line_y.endPt+wing_vec*pSlot->size_wide*0.5+wing_y_norm*0.1;
	coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
	coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
	pLine=AppendDbLine(arrow,pen,hiberid,wing_y_norm,true);
	pLine->SetSelectLevel(2);
	//���Y֫�ڲ�����
	arrow.startPt=ber_line_y.startPt+wing_vec*pSlot->size_wide*0.5-wing_y_norm*(pSlot->size_thick+0.1);
	arrow.endPt  =ber_line_y.endPt+wing_vec*pSlot->size_wide*0.5-wing_y_norm*(pSlot->size_thick+0.1);
	coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
	coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
	pLine=AppendDbLine(arrow,pen,hiberid,-wing_y_norm,true);
	pLine->SetSelectLevel(2);
	//ʵ��״̬�µ�ʼ->�շ�����
	if(CLDSLinePart::bDisplayLineVecMark)
	{
		pen.style=PS_SOLID;
		double startspace=min(130,0.1*length);
		f3dPoint pt_s=ptS+align_vec*(startspace+20-pSlot->startOdd());
		f3dPoint pt_x=ber_line_x.startPt+align_vec*(startspace+20-pSlot->startOdd())+wing_vec*pSlot->size_wide*0.5;
		f3dPoint pt_y=ber_line_y.startPt+align_vec*(startspace+20-pSlot->startOdd())+wing_vec*pSlot->size_wide*0.5;
		f3dLine arrow_s1(pt_s,pt_s-align_vec*20+wing_x_norm*10);
		f3dLine arrow_s2(pt_s,pt_s-align_vec*20+wing_y_norm*10);
		f3dLine arrow_x1(pt_x,pt_x-align_vec*20+wing_vec*10);
		f3dLine arrow_x2(pt_x,pt_x-align_vec*20-wing_vec*10);
		f3dLine arrow_y1(pt_y,pt_y-align_vec*20+wing_vec*10);
		f3dLine arrow_y2(pt_y,pt_y-align_vec*20-wing_vec*10);
		double rgn_length=length-startspace*2-20;
		int slices=min(10,max(1,ftoi(rgn_length/200)));
		double step=max(1,rgn_length/slices);
		for(int i=0;i<=slices;i++)
		{
			//��Ӳ�����ͷ
			pen.crColor=RGB(255,0,0);
			arrow.startPt=arrow_s1.startPt+align_vec*(i*step)-wing_vec*0.1;
			arrow.endPt  =arrow_s1.endPt  +align_vec*(i*step)-wing_vec*0.1;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			AppendDbLine(arrow,pen,HIBERID(pSlot->handle),-wing_vec);
			arrow.startPt=arrow_s2.startPt+align_vec*(i*step)-wing_vec*0.1;
			arrow.endPt  =arrow_s2.endPt  +align_vec*(i*step)-wing_vec*0.1;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			AppendDbLine(arrow,pen,HIBERID(pSlot->handle),-wing_vec);
			//��Ӳ��ڲ��ͷ
			arrow.startPt=arrow_s1.startPt+align_vec*(i*step)+(pSlot->size_thick+0.1)*wing_vec;
			arrow.endPt  =arrow_s1.endPt  +align_vec*(i*step)+(pSlot->size_thick+0.1)*wing_vec;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			AppendDbLine(arrow,pen,HIBERID(pSlot->handle),wing_vec);
			arrow.startPt=arrow_s2.startPt+align_vec*(i*step)+(pSlot->size_thick+0.1)*wing_vec;
			arrow.endPt  =arrow_s2.endPt  +align_vec*(i*step)+(pSlot->size_thick+0.1)*wing_vec;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			AppendDbLine(arrow,pen,HIBERID(pSlot->handle),wing_vec);
			//���X֫����ͷ
			pen.crColor=RGB(0,255,0);
			arrow.startPt=arrow_x1.startPt+align_vec*(i*step)+wing_x_norm*0.1;
			arrow.endPt  =arrow_x1.endPt  +align_vec*(i*step)+wing_x_norm*0.1;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			AppendDbLine(arrow,pen,HIBERID(pSlot->handle),wing_x_norm);
			arrow.startPt=arrow_x2.startPt+align_vec*(i*step)+wing_x_norm*0.1;
			arrow.endPt  =arrow_x2.endPt  +align_vec*(i*step)+wing_x_norm*0.1;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			AppendDbLine(arrow,pen,HIBERID(pSlot->handle),wing_x_norm);
			//���X֫�ڲ��ͷ
			arrow.startPt=arrow_x1.startPt+align_vec*(i*step)-(pSlot->size_thick+0.1)*wing_x_norm;
			arrow.endPt  =arrow_x1.endPt  +align_vec*(i*step)-(pSlot->size_thick+0.1)*wing_x_norm;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			AppendDbLine(arrow,pen,HIBERID(pSlot->handle),-wing_x_norm);
			arrow.startPt=arrow_x2.startPt+align_vec*(i*step)-(pSlot->size_thick+0.1)*wing_x_norm;
			arrow.endPt  =arrow_x2.endPt  +align_vec*(i*step)-(pSlot->size_thick+0.1)*wing_x_norm;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			AppendDbLine(arrow,pen,HIBERID(pSlot->handle),-wing_x_norm);
			//���Y֫����ͷ
			pen.crColor=RGB(0,255,0);
			arrow.startPt=arrow_y1.startPt+align_vec*(i*step)+wing_y_norm*0.1;
			arrow.endPt  =arrow_y1.endPt  +align_vec*(i*step)+wing_y_norm*0.1;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			AppendDbLine(arrow,pen,HIBERID(pSlot->handle),wing_y_norm);
			arrow.startPt=arrow_y2.startPt+align_vec*(i*step)+wing_y_norm*0.1;
			arrow.endPt  =arrow_y2.endPt  +align_vec*(i*step)+wing_y_norm*0.1;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			AppendDbLine(arrow,pen,HIBERID(pSlot->handle),wing_y_norm);
			//���Y֫�ڲ��ͷ
			arrow.startPt=arrow_y1.startPt+align_vec*(i*step)-(pSlot->size_thick+0.1)*wing_y_norm;
			arrow.endPt  =arrow_y1.endPt  +align_vec*(i*step)-(pSlot->size_thick+0.1)*wing_y_norm;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			AppendDbLine(arrow,pen,HIBERID(pSlot->handle),-wing_y_norm);
			arrow.startPt=arrow_y2.startPt+align_vec*(i*step)-(pSlot->size_thick+0.1)*wing_y_norm;
			arrow.endPt  =arrow_y2.endPt  +align_vec*(i*step)-(pSlot->size_thick+0.1)*wing_y_norm;
			coord_trans(arrow.startPt,object_ucs,TRUE,TRUE);
			coord_trans(arrow.endPt  ,object_ucs,TRUE,TRUE);
			AppendDbLine(arrow,pen,HIBERID(pSlot->handle),-wing_y_norm);
		}
	}
}
IDbLine* CLDSView::AppendDbLine(f3dLine line,PEN_STRU pen,HIBERID hiberId,f3dPoint up_vec,bool bSelect/*=false*/)
{
	IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
	if(pBuddyDrawing==NULL)
		return NULL;
	IDbLine* pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
	pLine->SetHiberId(hiberId);
	pLine->SetPen(pen);
	pLine->SetStartPosition(line.startPt);
	pLine->SetEndPosition(line.endPt);
	pLine->SetUpperDirection(up_vec);
	pLine->SetSelectable(bSelect);
	return pLine;
}
void CLDSView::UpdatePlateProfileDisplayStates()
{
	IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
	for(PLATEEDIT_STATE* pState=CLDSGeneralPlate::m_hashDisplayPlateStates.GetFirst();pState;pState=CLDSGeneralPlate::m_hashDisplayPlateStates.GetNext())
	{
		long hPart=CLDSGeneralPlate::m_hashDisplayPlateStates.GetCursorKey();
		if(selectObjs.GetValue(hPart)==NULL)
		{
			CLDSGeneralPlate::m_hashDisplayPlateStates.DeleteCursor();
			CLDSGeneralPlate* pBasicPlate=(CLDSGeneralPlate*)Ta.Parts.FromHandle(hPart,CLS_PLATE,CLS_PARAMPLATE);
			if(pBasicPlate==NULL)
			{
				CLDSGeneralPlate::m_hashDisplayPlateStates.DeleteCursor();
				if(pBuddyDrawing)
					pBuddyDrawing->DeleteDbEntityByHiberMasterId(hPart);
			}
			else
				RedrawPlateProfileDrawing(pBasicPlate);
		}
	}
}
// cbModeģʽ˵����
void CLDSView::RedrawPlateProfileDrawing(CLDSGeneralPlate* pPlate,UCS_STRU *pObjectCS/*=NULL*/,bool removeOldEntities/*=true*/,BYTE cbMode/*=0x03*/)
{
	if(pPlate==NULL)
		return;
	if(CLDSGeneralPlate::m_ciDisplayVertexType==0)
		return;
	const BYTE KEEP_SOLID = 0x01;
	const BYTE DRAW_TOPVERTEX = 0x02;
	const BYTE DRAW_TOPEDGELINE = 0x02;//0x04;
	cbMode|=0x03;
	IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
	PLATEEDIT_STATE* pPickedState=CLDSGeneralPlate::m_hashDisplayPlateStates.GetValue(pPlate->handle);
	BYTE ciPickedDisplayMode=pPickedState!=NULL?1:pPlate->m_cPickedDisplayMode;
	if(pPlate->m_cPickedDisplayMode>0)
		CLDSGeneralPlate::m_hashDisplayPlateStates.SetValue(pPlate->handle,pPlate->m_cPickedDisplayMode);
	if(!ciPickedDisplayMode)
	{
		if(pBuddyDrawing)
			pBuddyDrawing->DeleteDbEntityByHiberMasterId(pPlate->handle);
		return;
	}
	UCS_STRU object_ucs;
	if(pObjectCS!=NULL)
		object_ucs=*pObjectCS;
	else
	{
		CDisplayView *pDispView=console.GetActiveView();
		if(pDispView==NULL)
			return;
		LoadDefaultUCS(&object_ucs);
		object_ucs.origin.Set();
		vector_trans(object_ucs.axis_x,pDispView->ucs,FALSE);
		vector_trans(object_ucs.axis_y,pDispView->ucs,FALSE);
		vector_trans(object_ucs.axis_z,pDispView->ucs,FALSE);
		coord_trans(object_ucs.origin,pDispView->ucs,FALSE);
	}

	if(removeOldEntities)
		pBuddyDrawing->DeleteDbEntityByHiberMasterId(pPlate->handle);
	if((cbMode&KEEP_SOLID)==0)
		g_pSolidDraw->DelEnt(pPlate->handle);
	int i=0;
	PROFILE_VERTEX *pVertex,*pNextVertex,*pPrevVertex=pPlate->vertex_list.GetTail();
	if(pPrevVertex)
		pPrevVertex->vertex.ID=pPlate->vertex_list.GetNodeNum();
	PEN_STRU pen,huoqulinepen;
	pen.style=PS_SOLID;
	pen.crColor=RGB(255,255,255);
	pen.width=4;
	huoqulinepen.style=PS_DASHDOTDOT;
	huoqulinepen.crColor=RGB(255,0,0);
	huoqulinepen.width=1;
	GEPOINT peakpoint;
	IDbPoint* pPoint;
	IDbLine*  pLine;
	IDbArcline* pArcline=NULL;
	//up1=1: ��������;up1=2:  ������;up1=3: ������
	//down1: ��ű�ʶ;down1:ʼ���ʶ;down1:��ű�ʶ
	HIBERID hiberid(pPlate->handle,HIBERARCHY(0,0,0,0));
	CLDSPlate* pCommPlate=pPlate->GetClassTypeId()==CLS_PLATE?(CLDSPlate*)pPlate:NULL;
	if(pCommPlate&&pCommPlate->IsPeakPointPlate())
	{
		peakpoint=pCommPlate->top_point;
		coord_trans(peakpoint  ,pPlate->ucs,TRUE,TRUE);
		coord_trans(peakpoint,object_ucs,TRUE,TRUE);
		pPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		pPoint->SetPen(pen);
		pPoint->SetPosition(peakpoint);
		hiberid.SetHiberarchy(1,0,0,0);
		pPoint->SetHiberId(hiberid);
	}
	f3dPoint btmvertex,topvertex;
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext(),i++)
	{
		GELINE edge,topedge;
		edge.start=pPrevVertex->vertex;
		edge.end=pVertex->vertex;
		if(ciPickedDisplayMode==1)
		{	//ʵ��������
			edge.start=pPlate->GetRealVertex(pPrevVertex->vertex);
			edge.end=pPlate->GetRealVertex(pVertex->vertex);
		}
		edge.start.z+=pPlate->GetNormOffset();
		edge.end.z+=pPlate->GetNormOffset();
		coord_trans(edge.start,pPlate->ucs,TRUE,TRUE);
		coord_trans(edge.end  ,pPlate->ucs,TRUE,TRUE);
		coord_trans(edge.start,object_ucs,TRUE,TRUE);
		coord_trans(edge.end  ,object_ucs,TRUE,TRUE);
		btmvertex=edge.end;
		//���㶥��Ķ����������
		if(ciPickedDisplayMode>0&&cbMode&DRAW_TOPVERTEX)
		{
			topvertex=pVertex->vertex;
			topvertex.z=pPlate->GetThick();	
			if(topvertex.feature>10)
			{	//���¼������������
				int bendFaceIndex=topvertex.feature%10-1;
				GEPOINT midnorm=pPlate->ucs.axis_z+pPlate->GetHuoquFaceNorm(bendFaceIndex);
				Standarized(midnorm);
				double cosa=abs(pPlate->ucs.axis_z*midnorm);
				vector_trans(midnorm,pPlate->ucs,FALSE,TRUE);
				double offsetlen=cosa>EPS?pPlate->Thick/cosa:pPlate->Thick;
				topvertex=pVertex->vertex+offsetlen*midnorm;
			}
			if(ciPickedDisplayMode==1)
				topvertex=pPlate->GetRealVertex(topvertex);
			topvertex.z+=pPlate->GetNormOffset();
			coord_trans(topvertex,pPlate->ucs,TRUE,TRUE);
			coord_trans(topvertex,object_ucs,TRUE,TRUE);
		}
		if(ciPickedDisplayMode>0&&cbMode&DRAW_TOPEDGELINE)
		{
			f3dPoint prev_vertex=pPrevVertex->vertex;
			f3dPoint curr_vertex=pVertex->vertex;
			prev_vertex.z=curr_vertex.z=pPlate->GetThick();
			//������ʼ��
			if(prev_vertex.feature>10)
			{	//���¼������������
				int bendFaceIndex=prev_vertex.feature%10-1;
				GEPOINT midnorm=pPlate->ucs.axis_z+pPlate->GetHuoquFaceNorm(bendFaceIndex);
				Standarized(midnorm);
				double cosa=abs(pPlate->ucs.axis_z*midnorm);
				vector_trans(midnorm,pPlate->ucs,FALSE,TRUE);
				double offsetlen=cosa>EPS?pPlate->Thick/cosa:pPlate->Thick;
				prev_vertex=pPrevVertex->vertex+offsetlen*midnorm;
			}
			if(ciPickedDisplayMode==1)
				prev_vertex=pPlate->GetRealVertex(prev_vertex);
			prev_vertex.z+=pPlate->GetNormOffset();
			coord_trans(prev_vertex  ,pPlate->ucs,TRUE,TRUE);
			//���㵱ǰ��ֹ��
			if(curr_vertex.feature>10)
			{	//���¼������������
				int bendFaceIndex=curr_vertex.feature%10-1;
				GEPOINT midnorm=pPlate->ucs.axis_z+pPlate->GetHuoquFaceNorm(bendFaceIndex);
				Standarized(midnorm);
				double cosa=abs(pPlate->ucs.axis_z*midnorm);
				vector_trans(midnorm,pPlate->ucs,FALSE,TRUE);
				double offsetlen=cosa>EPS?pPlate->Thick/cosa:pPlate->Thick;
				curr_vertex=pVertex->vertex+offsetlen*midnorm;
			}
			if(ciPickedDisplayMode==1)
				curr_vertex=pPlate->GetRealVertex(curr_vertex);
			curr_vertex.z+=pPlate->GetNormOffset();
			coord_trans(curr_vertex  ,pPlate->ucs,TRUE,TRUE);
			//
			topedge.start=prev_vertex;
			topedge.end=curr_vertex;
			coord_trans(topedge.start,object_ucs,TRUE,TRUE);
			coord_trans(topedge.end  ,object_ucs,TRUE,TRUE);
		}
		//������������
		pVertex->vertex.ID=i+1;
		pPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
		pen.crColor=RGB(255,255,255);
		pen.width=8;
		pPoint->SetPen(pen);
		pPoint->SetPosition(btmvertex);
		hiberid.SetHiberarchy(1,0,pVertex->vertex.ID,0);
		pPoint->SetHiberId(hiberid);
		pPoint->SetUpperDirection(-pPlate->ucs.axis_z);	//������������
		if((cbMode&DRAW_TOPVERTEX)&&topvertex.feature<10)
		{	//�������֮��
			pPoint=(IDbPoint*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbPoint);
			pen.crColor=RGB(255,255,255);
			pPoint->SetPen(pen);
			pPoint->SetPosition(topvertex);
			hiberid.SetHiberarchy(1,0,pVertex->vertex.ID,0);
			pPoint->SetHiberId(hiberid);
			pPoint->SetUpperDirection(pPlate->ucs.axis_z);	//������������
		}
		//g_pSolidDraw->NewPoint(pVertex->vertex.x,pVertex->vertex.y,0,SUBID(pSubObj->iSubId,SUBID::GROUP_BOUNDARY));
		BOOL pushed=pPlate->vertex_list.push_stack();
		pNextVertex=pPlate->vertex_list.GetNext();
		if(pNextVertex==NULL)
			pNextVertex=pPlate->vertex_list.GetFirst();
		pPlate->vertex_list.pop_stack(pushed);
		f3dPoint vNextVec=pNextVertex->vertex-pVertex->vertex;
		f3dPoint vPrevVec=pPrevVertex->vertex-pVertex->vertex;
		normalize(vNextVec);
		normalize(vPrevVec);
		f3dPoint vMidVec=vNextVec+vPrevVec;
		normalize(vMidVec);
		vector_trans(vMidVec,pPlate->ucs,TRUE,TRUE);
		//������Ļ����ķ��߷���(ģ������ϵ)
		GEPOINT dynScrNorm=g_pSolidOper->TransVFromScr(GEPOINT(0,0,-1));	//��Ļ����ϵY������Z����� wjh-2018.1.26
		bool bScrNormalSameToAxisZ=dynScrNorm*pPlate->ucs.axis_z>0;
		f3dPoint draw_pos=bScrNormalSameToAxisZ?topedge.end:edge.end;//pVertex->vertex;�����ʶ��ĸҲӦ��ʾ��ʵ������λ�ã�����չ��λ�� wjh-2017.12.3
		vector_trans(vMidVec,object_ucs,TRUE,TRUE);
		GEPOINT vPlateNorm=bScrNormalSameToAxisZ?pPlate->ucs.axis_z:-pPlate->ucs.axis_z;
		vector_trans(vPlateNorm,object_ucs,TRUE,TRUE);
		draw_pos+=-vMidVec*20+vPlateNorm*pPlate->GetThick();
		char alphabet[2]={'A'+i,0};
		if(CLDSGeneralPlate::m_ciDisplayVertexType==1)
		{
			IDbText* pText=(IDbText*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbText);
			PEN_STRU fontpen;
			fontpen.crColor=RGB(255,0,0);
			fontpen.style=PS_SOLID;
			fontpen.width=3;
			pText->SetPen(fontpen); //Ĭ����������ĸ��ʶ��ʾ��ɫ
			pText->SetTextString(alphabet);
			pText->SetHeight(36);
			pText->SetAlignment(3);
			hiberid.SetHiberarchy(4,0,pVertex->vertex.ID,0);
			pText->SetHiberId(hiberid);
			pText->SetPosition(GEPOINT(draw_pos));
			pText->SetNormal(GEPOINT(0,0,0));
		}
		//g_pSolidDraw->NewText(alphabet,draw_pos,20,f3dPoint(0.0),f3dPoint(0,0,0),3,2);
		//����������
		pen.width=4;
		pen.crColor=RGB(0,127,255);
		hiberid.SetHiberarchy(2,0,pPrevVertex->vertex.ID,0);
		//pPrevPnt->vertex.feature+pVertex->vertex.feature==5�����Ը�ǿ(��������ͨ�����XY��Գ����ɵ������)��wjh-2015.9.10
		if(pCommPlate&&pCommPlate->IsPeakPointPlate()&&pPrevVertex->vertex.feature+pVertex->vertex.feature==5)
		{
			pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
			pLine->SetPen(pen);
			pLine->SetStartPosition(edge.start);
			pLine->SetEndPosition(peakpoint);
			//hiberid.SetHiberarchy(2,0,pPrevVertex->vertex.ID,0);
			pLine->SetHiberId(hiberid);
			if(cbMode&DRAW_TOPEDGELINE)
				pLine->SetUpperDirection(-pPlate->ucs.axis_z);	//������������

			pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
			pLine->SetPen(pen);
			pLine->SetStartPosition(peakpoint);
			pLine->SetEndPosition(edge.end);
			hiberid.SetHiberarchy(2,0,0,0);
			pLine->SetHiberId(hiberid);
			if(cbMode&DRAW_TOPEDGELINE)
				pLine->SetUpperDirection(-pPlate->ucs.axis_z);	//������������
			if(cbMode&DRAW_TOPEDGELINE)
			{
				f3dPoint topPeakPt=pCommPlate->top_point;
				topPeakPt.z=pPlate->GetThick();
				coord_trans(topPeakPt  ,pPlate->ucs,TRUE,TRUE);
				coord_trans(topPeakPt,object_ucs,TRUE,TRUE);
				//
				pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
				pLine->SetPen(pen);
				pLine->SetStartPosition(topedge.start);
				pLine->SetEndPosition(topPeakPt);
				//hiberid.SetHiberarchy(2,0,pPrevVertex->vertex.ID,0);
				pLine->SetHiberId(hiberid);
				pLine->SetUpperDirection(pPlate->ucs.axis_z);	//������������

				pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
				pLine->SetPen(pen);
				pLine->SetStartPosition(topPeakPt);
				pLine->SetEndPosition(topedge.end);
				hiberid.SetHiberarchy(2,0,0,0);
				pLine->SetHiberId(hiberid);
			}
		}
		else//if(pPlate->GetFaceN()<3||pCommPlate->IsFoldPlate()||(pPrevPnt->vertex.feature+pVertex->vertex.feature!=5))
		{
			f3dArcLine arcLine;
			int sign=1;
			int iFace=pPrevVertex->vertex.feature;
			pArcline=NULL;
			if(pPrevVertex->type==1)
			{	//Բ��
				if(pPrevVertex->sector_angle<0)
					sign=-1;
				pArcline=(IDbArcline*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbArcline);
				pArcline->SetPen(pen);
				pArcline->SetHiberId(hiberid);
				pArcline->CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),fabs(pPrevVertex->sector_angle));
			}
			else if(pPrevVertex->type==2)	//ָ��Բ��R
			{
				pArcline=(IDbArcline*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbArcline);
				pArcline->SetPen(pen);
				pArcline->SetHiberId(hiberid);
				double dist=DISTANCE(pPrevVertex->vertex,pVertex->vertex);
				if(dist>1)
					pArcline->CreateMethod3(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,1),pPrevVertex->radius,pPrevVertex->center);
			}
			else if(pPrevVertex->type==3)	//��Բ��
			{
				f3dPoint center = pPrevVertex->center;
				f3dPoint col_axis=pPrevVertex->column_norm;
				coord_trans(center,pPlate->ucs,FALSE);
				vector_trans(col_axis,pPlate->ucs,FALSE);
				if(pPrevVertex->radius<0)
					sign=-1;
				pArcline=(IDbArcline*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbArcline);
				pArcline->SetPen(pen);
				pArcline->SetHiberId(hiberid);
				pArcline->CreateEllipse(center,pPrevVertex->vertex,pVertex->vertex,col_axis,f3dPoint(0,0,sign),sign*pPrevVertex->radius);
			}
			else
			{	//ֱ��
				IDbLine* pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
				pLine->SetPen(pen);
				pLine->SetHiberId(hiberid);
				pLine->SetStartPosition(edge.start);
				pLine->SetEndPosition(edge.end);
				if(cbMode&DRAW_TOPEDGELINE)
				{
					pLine->SetUpperDirection(-pPlate->ucs.axis_z);
					pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
					pLine->SetPen(pen);
					pLine->SetHiberId(hiberid);
					pLine->SetStartPosition(topedge.start);
					pLine->SetEndPosition(topedge.end);
					pLine->SetUpperDirection(pPlate->ucs.axis_z);
				}
			}
			if(pArcline)
			{	//֮ǰ�汾���ΰ嶥�߻���δ��������ϵת�� wjh-2017.4.21
				pArcline->Offset(GEPOINT(0,0,pPlate->GetNormOffset()));
				f3dArcLine toparcline=pArcline->GetArcLine();
				pArcline->CoordTrans(pPlate->ucs);
				pArcline->CoordTrans(object_ucs);
				if(cbMode&DRAW_TOPEDGELINE)
				{
					pArcline->SetUpperDirection(-pPlate->ucs.axis_z);
					toparcline.Offset(f3dPoint(0,0,pPlate->Thick));
					pArcline=NULL;
					if(pPrevVertex->type>0&&pPrevVertex->type<3)
					{
						pArcline=(IDbArcline*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbArcline);
						pArcline->CreateMethod2(toparcline.Start(),toparcline.End(),toparcline.WorkNorm(),toparcline.SectorAngle());
					}
					else if(pPrevVertex->type==3)
					{
						pArcline=(IDbArcline*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbArcline);
						pArcline->CreateEllipse(toparcline.Center(),toparcline.Start(),toparcline.End(),toparcline.ColumnNorm(),toparcline.WorkNorm(),toparcline.Radius());
					}
					if(pArcline)
					{
						pArcline->SetPen(pen);
						pArcline->SetHiberId(hiberid);
						pArcline->SetUpperDirection(pPlate->ucs.axis_z);
						pArcline->CoordTrans(pPlate->ucs);
						pArcline->CoordTrans(object_ucs);
					}
				}
			}
		}
		pPrevVertex=pVertex;
	}
	pen.width=1;	//��ԭ�߿�
	PROFILE_VERTEX* pBendLine1Start=NULL,*pBendLine1End=NULL;
	PROFILE_VERTEX* pBendLine2Start=NULL,*pBendLine2End=NULL;
	pPlate->GetBendlineVertexies(&pBendLine1Start,&pBendLine1End,&pBendLine2Start,&pBendLine2End);
	bool initbendline1=true,initbendline2=true;
	GELINE bendline1,bendline2;
	if(pBendLine1Start)
		bendline1.start=pBendLine1Start->vertex;
	else if(pCommPlate&&pCommPlate->IsPeakPointPlate())
		bendline1.start=pCommPlate->top_point;
	else
		initbendline1=false;
	if(pBendLine1End)
		bendline1.end=pBendLine1End->vertex;
	else if(pCommPlate&&pCommPlate->IsPeakPointPlate())
		bendline1.end=pCommPlate->top_point;
	else
		initbendline1=false;
	if(pBendLine2Start)
		bendline2.start=pBendLine2Start->vertex;
	else if(pCommPlate&&pCommPlate->IsPeakPointPlate())
		bendline2.start=pCommPlate->top_point;
	else
		initbendline2=false;
	if(pBendLine2End)
		bendline2.end=pBendLine2End->vertex;
	else if(pCommPlate&&pCommPlate->IsPeakPointPlate())
		bendline2.end=pCommPlate->top_point;
	else
		initbendline2=false;

	if(initbendline1)
	{	//���ƻ�����ڶ���������
		coord_trans(bendline1.start,pPlate->ucs,TRUE,TRUE);
		coord_trans(bendline1.end  ,pPlate->ucs,TRUE,TRUE);
		coord_trans(bendline1.start,object_ucs,TRUE,TRUE);
		coord_trans(bendline1.end  ,object_ucs,TRUE,TRUE);
		IDbLine* pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
		pLine->SetPen(huoqulinepen);
		hiberid.SetHiberarchy(3,0,1,0);
		pLine->SetHiberId(hiberid);
		pLine->SetStartPosition(bendline1.start);
		pLine->SetEndPosition(bendline1.end);
	}
	if(initbendline2)
	{	//���ƻ����������������
		coord_trans(bendline2.start,pPlate->ucs,TRUE,TRUE);
		coord_trans(bendline2.end  ,pPlate->ucs,TRUE,TRUE);
		coord_trans(bendline2.start,object_ucs,TRUE,TRUE);
		coord_trans(bendline2.end  ,object_ucs,TRUE,TRUE);
		IDbLine* pLine=(IDbLine*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbLine);
		pLine->SetPen(huoqulinepen);
		hiberid.SetHiberarchy(3,0,2,0);
		pLine->SetHiberId(hiberid);
		pLine->SetStartPosition(bendline2.start);
		pLine->SetEndPosition(bendline2.end);
	}
	//������˨��
	ATOM_LIST<BOLTHOLE_INFO> holeList;
	pPlate->GetAllBoltHoles(holeList);
	for(BOLTHOLE_INFO* pHole=holeList.GetFirst();pHole;pHole=holeList.GetNext())
	{
		CLDSBolt* pBolt=pHole->pBolt?pHole->pBolt:NULL;
		if(pBolt&&pBolt->is_visible)
			continue;	//����ʾʵ����˨
		GEPOINT holepos=pHole->centre;
		if(ciPickedDisplayMode==1)	//ʵ��������
			pPlate->GetBoltIntersPos(pBolt,holepos);
		else
			coord_trans(holepos,pPlate->ucs,TRUE,TRUE);
		coord_trans(holepos,object_ucs,TRUE,TRUE);
		IDbCircle* pCir=(IDbCircle*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbCircle);
		pCir->SetPen(pen);
		if(pBolt)
			hiberid.SetHiberarchy(5,0,pBolt->handle,0);
		else
			hiberid.SetHiberarchy(5,0,0,0);
		pCir->SetHiberId(hiberid);
		pCir->SetRadius(pHole->bolt_d*0.5);
		pCir->SetCenter(holepos);
		GEPOINT worknorm=pPlate->ucs.axis_z;
		vector_trans(worknorm,object_ucs,TRUE,TRUE);
		pCir->SetNormal(worknorm);
	}
}
static bool ConvertFromSizeCheckKeyPoint(CLDSPoint* pPoint,CDataCheckModel::KEYPOINT* pKeyPoint)
{
	SmartRodPtr pRod=Ta.FromRodHandle(pKeyPoint->angle.hAngle);
	CLDSNode*  pNode=Ta.Node.FromHandle(pKeyPoint->angle.hNode);
	if(pRod.IsNULL()||pNode==NULL)
		return false;
	if(pKeyPoint->ciPointType>1)
		return false;
	if(pKeyPoint->ciPointType==1)
	{
		SmartPartPtr pDatumPart=pKeyPoint->coord.hPart>0 ? Ta.FromPartHandle(pKeyPoint->coord.hPart) : NULL;
		if(pDatumPart.IsNULL())
		{
			pPoint->datum_pos_style=0;
			pPoint->datum_pos.Set(pKeyPoint->coord.x,pKeyPoint->coord.y,pKeyPoint->coord.z);
			pPoint->UpdatePos();
		}
		else
		{	//ָ���������������
			pPoint->datum_pos_style=9;
			pPoint->des_para.hPart=pKeyPoint->coord.hPart;
			pPoint->datum_pos.Set(pKeyPoint->coord.x,pKeyPoint->coord.y,pKeyPoint->coord.z);
			pPoint->UpdatePos();
		}
		return true;
	}
	pPoint->datum_pos_style=2;
	/*if((pRod->pStart==pNode||pRod->pEnd==pNode)&&pKeyPoint->angle.offset.ciOffsetPlane==0)
	{
		pPoint->datum_pos_style=1;
		pKeyPoint->ciPointType=0;
		pKeyPoint->angle.hAngle=pPoint->des_para.RODEND.hRod;
		SmartRodPtr pRod=Ta.FromRodHandle(pKeyPoint->angle.hAngle);
		if(pPoint->des_para.RODEND.direction==0&&pRod.IsHasPtr()&&pRod->pStart)
			pKeyPoint->angle.hNode=pRod->pStart->handle;
		else if(pPoint->des_para.RODEND.direction==1&&pRod.IsHasPtr()&&pRod->pEnd)
			pKeyPoint->angle.hNode=pRod->pEnd->handle;
		pKeyPoint->angle.offset.ciOffsetWing=pPoint->des_para.RODEND.offset_wing;
		pKeyPoint->angle.offset.wiLenOffset=pPoint->des_para.RODEND.len_offset_dist;
		pKeyPoint->angle.offset.ciStyleOfG=pPoint->des_para.RODEND.wing_offset_style;
		pKeyPoint->angle.offset.wiWingOffset=pPoint->des_para.RODEND.wing_offset_dist;
		pKeyPoint->angle.offset.ciOffsetPlane=0;
	}
	else if(pPoint->datum_pos_style==2)*/
	{
		pPoint->datum_pos_style=2;
		pPoint->des_para.RODNODE.hRod=pKeyPoint->angle.hAngle;
		pPoint->des_para.RODNODE.hNode=pKeyPoint->angle.hNode;
		pPoint->des_para.RODNODE.offset_wing=pKeyPoint->angle.offset.ciOffsetWing;
		pPoint->des_para.RODNODE.len_offset_dist=pKeyPoint->angle.offset.wiLenOffset;
		pPoint->des_para.RODNODE.wing_offset_style=pKeyPoint->angle.offset.ciStyleOfG;
		pPoint->des_para.RODNODE.wing_offset_dist=pKeyPoint->angle.offset.wiWingOffset;
		pPoint->des_para.RODNODE.bFaceOffset=FALSE;
		pPoint->des_para.RODNODE.vPlaneNormal.Set(0,0,0);
		if(pKeyPoint->angle.offset.ciOffsetPlane>0&&pKeyPoint->angle.offset.ciOffsetPlane<=3)
		{
			pPoint->des_para.RODNODE.bFaceOffset=TRUE;
			if(pKeyPoint->angle.offset.ciOffsetPlane==1)
				pPoint->des_para.RODNODE.vPlaneNormal.Set(0,0,1);	//X-Yƽ��
			else if(pKeyPoint->angle.offset.ciOffsetPlane==2)
				pPoint->des_para.RODNODE.vPlaneNormal.Set(1,0,0);	//Y-Zƽ��
			else if(pKeyPoint->angle.offset.ciOffsetPlane==3)
				pPoint->des_para.RODNODE.vPlaneNormal.Set(0,1,0);	//X-Zƽ��
		}
	}
	return true;
}
void CLDSView::AppendSizeCoordCheckItem(void* pCoordSize)
{
	CDataCheckModel::SIZE_DATA* pCoordSizeItem=(CDataCheckModel::SIZE_DATA*)pCoordSize;
	IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
	if(pCoordSizeItem->ciSizeType==0)
	{
		CLDSPoint from,to;
		bool suscessed=true;
		suscessed=suscessed&&ConvertFromSizeCheckKeyPoint(&from,&pCoordSizeItem->from);
		suscessed=suscessed&&ConvertFromSizeCheckKeyPoint(&to,&pCoordSizeItem->to);
		//from.UpdatePos(&Ta);
		to.UpdatePos(&Ta);
		GEPOINT dimFrom =from.Position();
		GEPOINT dimTo	=to.Position();
		for(int i=0;i<3;i++)
		{
			if(i==0&&pCoordSizeItem->cbSizeDimFlag&0x01)
				dimFrom=dimTo.x>0?dimTo-GEPOINT(500,0,0):dimTo+GEPOINT(500,0,0);
			else if(i==1&&pCoordSizeItem->cbSizeDimFlag&0x02)
				dimFrom=dimTo.y>0?dimTo-GEPOINT(0,500,0):dimTo+GEPOINT(0,500,0);
			else if(i==2&&pCoordSizeItem->cbSizeDimFlag&0x04)
				dimFrom=dimTo.z>0?dimTo-GEPOINT(0,0,500):dimTo+GEPOINT(0,0,500);
			else
				continue;
			IDbRadialDimension* pRadialDimension=(IDbRadialDimension*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbRadialDimension);
			pRadialDimension->SetUpperDirection(GEPOINT(0,0,-1));
			pRadialDimension->SetChordPoint(dimTo);
			pRadialDimension->SetCenterPoint(dimFrom);
			pRadialDimension->SetLeaderLength(g_sysPara.display.nRodDesignInfoTextHeight);
			pRadialDimension->SetUpwardOffset(g_sysPara.display.nRodDesignInfoTextHeight);
			double actual=dimTo.x,planing=pCoordSizeItem->dist.coords.distInX;
			if(i==1)
			{
				actual=dimTo.y;
				planing=pCoordSizeItem->dist.coords.distInY;
			}
			else if(i==2)
			{
				actual=dimTo.z;
				planing=pCoordSizeItem->dist.coords.distInZ;
			}
			PEN_STRU pen;
			pen.style=PS_SOLID;
			pen.width=1;
			if(fabs(actual-planing)<pCoordSizeItem->tolerance)
			{
				pen.crColor=RGB(255,255,0);
				if(i<2)
					pRadialDimension->SetDimText(CXhChar16("������%d",ftoi(fabs(actual))));
				else
					pRadialDimension->SetDimText(CXhChar16("Z����%d",ftoi(fabs(actual))));
			}
			else
			{
				pen.crColor=RGB(255,0,0);
				if(i<2)
				{
					if(actual*planing>0)
						pRadialDimension->SetDimText(CXhChar50("������%.1f��(%g)",fabs(actual),fabs(planing)));
					else
						pRadialDimension->SetDimText(CXhChar50("������%.1f��(%g)",actual,planing));
				}
				else
				{
					if(actual*planing>0)
						pRadialDimension->SetDimText(CXhChar50("Z����%.1f��(%g)",fabs(actual),fabs(planing)));
					else
						pRadialDimension->SetDimText(CXhChar50("Z����%.1f��(%g)",actual,planing));
				}
			}
			pRadialDimension->SetTextHeight(g_sysPara.display.nRodDesignInfoTextHeight);
			pRadialDimension->SetArrowSize(g_sysPara.display.nRodDesignInfoTextHeight/2);
			pRadialDimension->SetPen(pen);
			HIBERID hiberid(pCoordSizeItem->Id);
			hiberid.SetHiberarchy(i);
			pRadialDimension->SetHiberId(hiberid);
		}
	}
	else if(pCoordSizeItem->ciSizeType==1)
	{	//�ؼ����ߴ�
		IDbAlignedDimension* pAlignDimension=(IDbAlignedDimension*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbAlignedDimension);
		CLDSPoint from,to;
		bool suscessed=true;
		suscessed=suscessed&&ConvertFromSizeCheckKeyPoint(&from,&pCoordSizeItem->from);
		suscessed=suscessed&&ConvertFromSizeCheckKeyPoint(&to,&pCoordSizeItem->to);
		from.UpdatePos(&Ta);
		to.UpdatePos(&Ta);
		GEPOINT dimLineStart=from.Position();
		GEPOINT dimLineEnd	=to.Position();
		pAlignDimension->SetUpperDirection(GEPOINT(0,0,-1));
		pAlignDimension->SetLine1Point(dimLineStart);
		pAlignDimension->SetLine2Point(dimLineEnd);
		GEPOINT dimpos=0.5*(dimLineStart+dimLineEnd);
		//dimpos+=pCoordSize->dimoffset;
		pAlignDimension->SetDimLinePoint(dimpos);
		pAlignDimension->SetTextHeight(g_sysPara.display.nRodDesignInfoTextHeight);
		pAlignDimension->SetArrowSize(g_sysPara.display.nRodDesignInfoTextHeight/2);
		double actual=DISTANCE(dimLineStart,dimLineEnd);
		PEN_STRU pen;
		pen.width=1;
		pen.style=PS_SOLID;
		if(fabs(actual-pCoordSizeItem->dist.distance)<pCoordSizeItem->tolerance)
		{
			pen.crColor=RGB(255,255,0);
			pAlignDimension->SetDimText(CXhChar16("%d",ftoi(pCoordSizeItem->dist.distance)));
		}
		else
		{
			pen.crColor=RGB(255,0,0);
			pAlignDimension->SetDimText(CXhChar50("%.1f��(%d)",actual,ftoi(pCoordSizeItem->dist.distance)));
		}
		pAlignDimension->SetPen(pen);
		pAlignDimension->SetHiberId(HIBERID(pCoordSizeItem->Id));
	}
	else if(pCoordSizeItem->ciSizeType==4||pCoordSizeItem->ciSizeType==5)
	{	//����׾�(����,����)
		PEN_STRU pen;
		pen.width=1;
		pen.style=PS_SOLID;
		double fNormOffset=0;
		GEPOINT dimLineStart,dimLineEnd,dynScrNorm;
		CSuperSmartPtr<CLDSPart> pDatumPart=Ta.Parts.FromHandle(pCoordSizeItem->dist.holes.hDatumRod);
		CLDSBolt* pStartBolt=(CLDSBolt*)Ta.Parts.FromHandle(pCoordSizeItem->from.hole.hBolt,CLS_BOLT);
		CLDSBolt* pEndBolt=(CLDSBolt*)Ta.Parts.FromHandle(pCoordSizeItem->to.hole.hBolt,CLS_BOLT);
		if(pDatumPart.IsNULL())
		{
			CLDSPart* pStartDatumPart=Ta.Parts.FromHandle(pCoordSizeItem->from.hole.hDatumPart);
			CLDSPart* pEndDatumPart=Ta.Parts.FromHandle(pCoordSizeItem->to.hole.hDatumPart);
			if(pStartDatumPart!=NULL)
				pStartDatumPart->GetBoltIntersPos(pStartBolt,dimLineStart);
			if(pEndDatumPart!=NULL)
				pEndDatumPart->GetBoltIntersPos(pEndBolt,dimLineEnd);
		}
		else if(!pDatumPart->IsAngle())
		{
			pDatumPart->GetBoltIntersPos(pStartBolt,dimLineStart);
			pDatumPart->GetBoltIntersPos(pEndBolt,dimLineEnd);
		}
		else if(pDatumPart->IsAngle())
		{
			//������Ļ����ķ��߷���
			dynScrNorm=g_pSolidOper->TransVFromScr(GEPOINT(0,0,-1));
			normalize(dynScrNorm);
			//������˨�ڵ�ǰ��׼�Ǹֹ���֫�ϵĽ���
			int wingx0_y1=0,_wingx0_y1=0;
			pDatumPart.pAngle->GetBoltIntersPos(pStartBolt,dimLineStart,&wingx0_y1);
			pDatumPart.pAngle->GetBoltIntersPos(pEndBolt,dimLineEnd,&_wingx0_y1);
			if(pStartBolt->Normal*dynScrNorm<0)
				dynScrNorm=-pStartBolt->get_norm();
			else
				dynScrNorm= pStartBolt->get_norm();
			GEPOINT vNormalOffset=f3dPoint(dimLineStart)-f3dPoint(pStartBolt->ucs.origin);
			if(vNormalOffset*dynScrNorm>0)
				fNormOffset=pStartBolt->L-vNormalOffset.mod()+5;
			else
				fNormOffset=pStartBolt->d+vNormalOffset.mod();

			GEPOINT vWingVec,vLenVec;
			if(wingx0_y1==0)
				vWingVec=pDatumPart.pAngle->GetWingVecX();
			else
				vWingVec=pDatumPart.pAngle->GetWingVecY();
			vLenVec=pDatumPart.pAngle->End()-pDatumPart.pAngle->Start();
			normalize(vLenVec);
			GEPOINT vDistVec=dimLineEnd-dimLineStart;
			double fDistG=vDistVec*vWingVec;
			if(pCoordSizeItem->cbSizeDimFlag&0x02)	//���ע������
			{
				GEPOINT xStart=dimLineStart,xEnd=dimLineEnd;
				if(fDistG<0)	//�ն���˨��������
					xStart+=fDistG*vWingVec;
				else	//ʼ����˨��������
					xEnd-=fDistG*vWingVec;
				IDbAlignedDimension* pAlignDimensionL=(IDbAlignedDimension*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbAlignedDimension);
				//pAlignDimensionL->SetUpperDirection(GEPOINT(0,0,-1));
				pAlignDimensionL->SetLine1Point(xStart);
				pAlignDimensionL->SetLine2Point(xEnd);
				GEPOINT dimpos=0.5*(xStart+xEnd);
				//dimpos+=pCoordSize->dimoffset;
				pAlignDimensionL->SetDimLinePoint(dimpos);
				pAlignDimensionL->SetTextHeight(30);//g_sysPara.display.nRodDesignInfoTextHeight);
				pAlignDimensionL->SetArrowSize(15);//g_sysPara.display.nRodDesignInfoTextHeight/2);
				pAlignDimensionL->SetNormal(dynScrNorm);
				pAlignDimensionL->SetNormalOffset(fNormOffset);
				double actual=DISTANCE(xStart,xEnd);
				if(fabs(actual-pCoordSizeItem->dist.holes.distInLength)<pCoordSizeItem->tolerance)
				{
					pen.crColor=RGB(255,255,0);
					pAlignDimensionL->SetDimText(CXhChar16("%d",ftoi(actual)));
				}
				else
				{
					pen.crColor=RGB(255,0,0);
					pAlignDimensionL->SetDimText(CXhChar50("%.1f��(%d)",actual,ftoi(pCoordSizeItem->dist.holes.distInLength)));
				}
				pAlignDimensionL->SetPen(pen);
				HIBERID hiberid(pCoordSizeItem->Id);
				hiberid.SetHiberarchy(1);
				pAlignDimensionL->SetHiberId(hiberid);
			}
			if(pCoordSizeItem->cbSizeDimFlag&0x04)	//���ע������
			{
				GEPOINT xStart=dimLineStart,xEnd=dimLineEnd;
				if(fDistG<0)	//�ն���˨��������
					xEnd=xStart+fDistG*vWingVec;
				else			//ʼ����˨��������
					xStart=f3dPoint(xEnd)-f3dPoint(fDistG*vWingVec);
				IDbAlignedDimension* pAlignDimensionG=(IDbAlignedDimension*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbAlignedDimension);
				//pAlignDimensionG->SetUpperDirection(GEPOINT(0,0,-1));
				pAlignDimensionG->SetLine1Point(xStart);
				pAlignDimensionG->SetLine2Point(xEnd);
				GEPOINT dimpos=0.5*(xStart+xEnd);
				//dimpos+=pCoordSize->dimoffset;
				pAlignDimensionG->SetDimLinePoint(dimpos);
				pAlignDimensionG->SetTextHeight(30);//g_sysPara.display.nRodDesignInfoTextHeight);
				pAlignDimensionG->SetArrowSize(15);	//g_sysPara.display.nRodDesignInfoTextHeight/2);
				pAlignDimensionG->SetNormal(dynScrNorm);
				pAlignDimensionG->SetNormalOffset(fNormOffset);
				double actual=DISTANCE(xStart,xEnd);
				if(fabs(actual-pCoordSizeItem->dist.holes.distInTransverse)<pCoordSizeItem->tolerance)
				{
					pen.crColor=RGB(255,255,0);
					pAlignDimensionG->SetDimText(CXhChar16("%d",ftoi(actual)));
				}
				else
				{
					pen.crColor=RGB(255,0,0);
					pAlignDimensionG->SetDimText(CXhChar50("%.1f��(%d)",actual,ftoi(pCoordSizeItem->dist.holes.distInTransverse)));
				}
				pAlignDimensionG->SetPen(pen);
				HIBERID hiberid(pCoordSizeItem->Id);
				hiberid.SetHiberarchy(2);
				pAlignDimensionG->SetHiberId(hiberid);
			}
		}
		IDbAlignedDimension* pAlignDimensionD=(IDbAlignedDimension*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbAlignedDimension);
		//pAlignDimensionD->SetUpperDirection(GEPOINT(0,0,-1));
		pAlignDimensionD->SetLine1Point(dimLineStart);
		pAlignDimensionD->SetLine2Point(dimLineEnd);
		GEPOINT dimpos=0.5*(dimLineStart+dimLineEnd);
		//dimpos+=pCoordSize->dimoffset;
		pAlignDimensionD->SetDimLinePoint(dimpos);
		pAlignDimensionD->SetTextHeight(30);//g_sysPara.display.nRodDesignInfoTextHeight);
		pAlignDimensionD->SetArrowSize(15);//g_sysPara.display.nRodDesignInfoTextHeight/2);
		pAlignDimensionD->SetUpwardOffset(-50);
		if(!dynScrNorm.IsZero())
		{
			pAlignDimensionD->SetNormal(dynScrNorm);
			pAlignDimensionD->SetNormalOffset(fNormOffset);
		}
		double actual=DISTANCE(dimLineStart,dimLineEnd);
		if(fabs(actual-pCoordSizeItem->dist.distance)<pCoordSizeItem->tolerance)
		{
			pen.crColor=RGB(255,255,0);
			pAlignDimensionD->SetDimText(CXhChar16("%d",ftoi(actual)));
		}
		else
		{
			pen.crColor=RGB(255,0,0);
			pAlignDimensionD->SetDimText(CXhChar50("%.1f��(%d)",actual,ftoi(pCoordSizeItem->dist.distance)));
		}
		pAlignDimensionD->SetPen(pen);
		pAlignDimensionD->SetHiberId(HIBERID(pCoordSizeItem->Id));
	}
}
/*
#include "./StdPartSolid/GDBoard.h"
#include "./StdPartSolid/EBHangingPoint.h"
CGDBoard gdboard;
CEBHangingPoint ebboard;
*/
CSolidBody gdbody,ebbody;
//#include "WireNodeDef.h"
struct ARROWOBJ{
	double dfArrowLength;
	GEPOINT basepoint;
	GEPOINT vStdArrowVec;
	ARROWOBJ(const double* base=NULL,const double* stdarrowvec=NULL,double arrowlength=10){
		basepoint=base;
		vStdArrowVec=stdarrowvec;
		dfArrowLength=arrowlength;
	}
	bool IsOverlapped(ARROWOBJ& arrow){
		if(!basepoint.IsEqual(arrow.basepoint,0.5))
			return false;
		if(vStdArrowVec*arrow.vStdArrowVec<0.9962)	//cos(5��)
			return false;
		if(min(dfArrowLength,arrow.dfArrowLength)<50)
			return false;
		return true;
	}
};
#include "ElectricTower.h"
void DrawSolidTower(void* pContext)
{
	CLDSObject *pCurAtom=NULL;
	CLogErrorLife loglife;
	CHashList<bool> hashDrawNodes;	//��ǰ������ʾ�Ľڵ�
	try
	{
		PEN_STRU pen;
		pen.style = PS_SOLID;
		pen.width = 1;
		//g_gl_ucsΪ��ǰ��ʾ��ͼ����������ڵ�ǰ��ʾ����ϵ�е��������ϵ����
		UCS_STRU object_ucs;
		CDisplayView *pDispView=console.GetActiveView();
		if(pDispView==NULL)
			return;
		PRESET_ARRAY64<ARROWOBJ> xarrArrowes;	//�Ѳ�����ʾ�ļ�ͷ
		DRAWCONTEXT* pDrawContext=(DRAWCONTEXT*)pContext;
		if(pDrawContext&&pDrawContext->ContextClsType()==DRAWCONTEXT::VIEW_ENV)
		{
			g_pSolidSet->SetObjectUcs(((OBJECTCS_CONTEXT*)pContext)->object_ucs);
			object_ucs=((OBJECTCS_CONTEXT*)pContext)->object_ucs;
		}
		else
		{
			LoadDefaultUCS(&object_ucs);
			object_ucs.origin.Set();
			vector_trans(object_ucs.axis_x,pDispView->ucs,FALSE);
			vector_trans(object_ucs.axis_y,pDispView->ucs,FALSE);
			vector_trans(object_ucs.axis_z,pDispView->ucs,FALSE);
			coord_trans(object_ucs.origin,pDispView->ucs,FALSE);
			g_pSolidSet->SetObjectUcs(object_ucs);
		}
		CHashList<SEGI>segTbl;
		GetSegNoHashTblBySegStr(Ta.segstr,segTbl);
		CLDSNode* pNode;
		CLDSPart *pPart;
		CLDSModule *pModule=Ta.GetActiveModule();
		if(pModule==NULL)
			return;
		g_pSolidDraw->BatchClearCS(4);	//���֮ǰ��Ļ����ʾ�Ĺ���װ������ϵ��wjh-2015.9.10
		IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
		if(pBuddyDrawing)
			pBuddyDrawing->EmptyDbEntities();
		//IDbAlignedDimension* pAlignDimension=(IDbAlignedDimension*)pBuddyDrawing->AppendDbEntity(IDbEntity::DbAlignedDimension);
		//pAlignDimension->SetUpperDirection(GEPOINT(0,0,-1));
		//pAlignDimension->SetLine1Point(GEPOINT(-1000,0,0));
		//pAlignDimension->SetLine2Point(GEPOINT( 1000,0,0));
		//pAlignDimension->SetDimLinePoint(GEPOINT( 0,0,-10));
		//pAlignDimension->SetDimText("2000");

		/*gdboard.Create3dSolidModel(&gdbody);
		ebboard.Create3dSolidModel(&ebbody);
		CSolidPart solidpart;
		solidpart.pBody=&gdbody;
		solidpart.m_hPart=1;
		solidpart.m_idClassType=CLS_PLATE;
		g_pSolidDraw->NewSolidPart(solidpart);*/
		DISPLAY_TYPE disp_type;
		g_pSolidSet->GetDisplayType(&disp_type);
		if(disp_type==DISP_LINE&&theApp.m_eEnvType==SOLID_ENV)
			theApp.m_eEnvType=LINE_ENV;
		else if(disp_type==DISP_SOLID&&theApp.m_eEnvType!=SOLID_ENV)
			g_pSolidSet->SetDisplayType(DISP_LINE);
#if defined(__LDS_)||defined(__TSA_)
		CWorkStatus *pStatus=Ta.GetActiveWorkStatus();
#endif
		CFGWORD dwBodyWord(pModule->GetBodyNo(),0),dwActiveQuad1LegWord(pModule->m_arrActiveQuadLegNo[0]),
			dwActiveQuad2LegWord(pModule->m_arrActiveQuadLegNo[1]),dwActiveQuad3LegWord(pModule->m_arrActiveQuadLegNo[2]),dwActiveQuad4LegWord(pModule->m_arrActiveQuadLegNo[3]);
		dwBodyWord.AddBodyLegs(pModule->GetBodyNo());
		if(!console.GetActiveModel()->IsTowerModel())
		{	//��ǰģ�ͷ�TowerModelʱ������ʹ�öκŷ�Χ����ĺŽ��й��� wht 13-04-10
			segTbl.Empty();
			memset(&(dwBodyWord.flag),1,sizeof(CFGWORD));
		}
		DWORD dwPermWord=theApp.GetPermDword();
		//if(g_sysPara.display.bNodePnt)//���ڵ���ͼ����չ��������״̬(��ֱ������DrawSolid�жԽڵ������ʾ���� wjh-2019.7.29)
		{	//Ҫ����ʾ�ڵ�
			if(g_sysPara.iColorSchema==1)
				pen.crColor = g_sysPara.crTsaMode.crNode;
			else if(g_sysPara.iColorSchema==2)
				pen.crColor = g_sysPara.crMaterialMode.crNode;
			else	
				pen.crColor = g_sysPara.crPartMode.crNode;
			g_pSolidSet->SetPointSize(g_sysPara.display.xPointStyle.cbPointSize);
			for(pNode=console.DispNodeSet.GetFirst();pNode;pNode=console.DispNodeSet.GetNext())
			{
				if(!pNode->is_visible)
					continue;
				if(!g_sysPara.display.bDispVirtualPart&&pNode->m_bVirtualObj)	//����ʾ���⸨���ڵ�
					continue;
				else
					pNode->is_visible=FALSE;
				if(theApp.m_bCooperativeWork&&!theApp.m_bDisplayAllPart&&!(pNode->dwPermission&dwPermWord))
					continue;
				if(Ta.segstr[0]=='0')
				{	//��ʾ�κ�Ϊ0�Ĺ����������޸Ķκ�
					if(pNode->iSeg.iSeg!=0)
						continue;
				}
				else
				//if(Ta.segstr[0]!='0'&& pNode->iSeg.iSeg==0) //���˵��κ�Ϊ����κ��ַ�����������Ľڵ�
				//	continue;
				//if(pNode->iSeg.iSeg!=0)
				{	//��ʾͼ����ͼʱ���ܶκ����� ����������ͼ����һ�㶼����ʿɲ��ܶκ�����
					if(pDispView->m_iViewType==0&&segTbl.GetNodeNum()>0&&segTbl.GetValue(pNode->iSeg.iSeg)==NULL)
						continue;
				}
				if(pNode->layer(0)=='L')
				{
					int iPos=CalPointQuadPos(pNode->ActivePosition());
					if(   (iPos==1&&pNode->cfgword.And(dwActiveQuad1LegWord))
						||(iPos==2&&pNode->cfgword.And(dwActiveQuad2LegWord))
						||(iPos==3&&pNode->cfgword.And(dwActiveQuad3LegWord))
						||(iPos==4&&pNode->cfgword.And(dwActiveQuad4LegWord)))
						pNode->is_visible=TRUE;
					else
						continue;
				}
				else if(pNode->cfgword.And(dwBodyWord))	//�����ͷ���˼�
					pNode->is_visible=TRUE;
				else
					continue;
				pCurAtom = pNode;	//��¼������
				//if(g_sysPara.iColorSchema==3)
				{
					int cPosCalType=pNode->m_cPosCalType;
					int cPosCalViceType=pNode->m_cPosCalViceType;
					if(cPosCalType==12)
					{
						CLDSNode* pMirNode=pNode->BelongModel()->FromNodeHandle(pNode->arrRelationNode[0]);
						if(pMirNode)
						{
							cPosCalType=pMirNode->m_cPosCalType;
							cPosCalViceType=pNode->m_cPosCalViceType;
						}
					}
					if( cPosCalType==1||  //�˼���ָ��X�����
						cPosCalType==2|| //�˼���ָ��Y�����
						cPosCalType==3|| //�˼���ָ��Z�����
						cPosCalType==6|| //�ظ˼���ĳ�ڵ�ȸߵĽڵ�  
						cPosCalType==8|| //��׼����ָ��XY����� 
						cPosCalType==9|| //��׼����ָ��YZ�����
						cPosCalType==10) //��׼����ָ��XZ����� 
						pen.crColor=g_sysPara.crPartMode.crAxisValFixNode;
					else if(cPosCalType==7&&(       //�ظ˼���ĳ�ڵ��ƫ�Ƶ�
							cPosCalViceType=='X'||  //X��
							cPosCalViceType=='Y'||  //Y��
							cPosCalViceType=='Z'))  //Z��
						pen.crColor=g_sysPara.crPartMode.crAxisValFixNode;
					else if(cPosCalType==5)       //�˼��ϵı����ȷֵ�
						pen.crColor=g_sysPara.crPartMode.crDivideScaleNode;
					else if(cPosCalType==0)  //�������κ������Ľڵ�
						pen.crColor=g_sysPara.crPartMode.crControlNode;
					else if(cPosCalType==4||cPosCalType==7) //����㼰�ظ˼���ĳ�ڵ��ƫ�Ƶ�
						pen.crColor=g_sysPara.crPartMode.crOffsetNode;
					else	
						pen.crColor = g_sysPara.crPartMode.crNode;
				}
				g_pSolidSet->SetPen(pen);
				hashDrawNodes.SetValue(pNode->handle,true);
#if defined(__LDS_)||defined(__TSA_)
				if(g_bVibrationAnimate)
				{
					f3dPoint point=pNode->Position(false)+pNode->m_offsetAmplitude*phasePositionCoef;
					point.ID = pNode->handle;
					g_pSolidDraw->NewPoint(point);
				}
				else
#endif
				{
					f3dPoint point = pNode->ActivePosition();
					point.ID = pNode->handle;
					g_pSolidDraw->NewPoint(point);
#if defined(__LDS_)||defined(__TSA_)
					if(theApp.IsAnalysisWorkEnv()&&pStatus&&g_sysPara.display.bLoadArrow)
					{	//�к��ش�������Ҫ��ʾ
						f3dPoint load=pNode->PermanentLoad;
						CExternalNodeLoad *pLoad=pStatus->hashNodeLoad.GetValue(pNode->handle);
						if(pLoad)
						{
							if(pStatus->m_bLoadIncFactor)
								load+=pLoad->designLoad;
							else
							{
								load+=pLoad->Fxyz;
								load.z+=pLoad->permanentFz;
								load+=pNode->PermanentLoad;
							}
						}
						if(!load.IsZero())
						{
							f3dPoint vec=load;
							normalize(vec);
							point.pen.crColor=g_sysPara.crTsaMode.crLoadArrow;
							double nArrowLen=load.mod()/1000*g_sysPara.display.nLoadArrowScale;
							g_pSolidDraw->NewArrow(point,f3dPoint(vec),nArrowLen);
							xarrArrowes.Append(ARROWOBJ(point,vec,nArrowLen));
						}
					}
					else if(theApp.IsAnalysisWorkEnv()&&pStatus==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
						logerr.Log("the active condition is not specified, load indicating arrow cannot be shown");
#else
						logerr.Log("δָ����ǰ��������޷���ʾ����ʾ���ͷ");
#endif
				}
#ifndef __RAPID_ROUGH_3DMODEL_
				if(theApp.IsAnalysisWorkEnv())
#endif
				{
					if(g_sysPara.display.bDimNodeLabel&&pNode->point_i>0)
					{
						CXhChar16 labelNodeI("%d",pNode->point_i);
						f3dPoint dimpos=pNode->ActivePosition();
						g_pSolidSet->SetPen(g_sysPara.crTsaMode.crDimText,PS_SOLID,1);
						g_pSolidDraw->NewText(labelNodeI,dimpos,g_sysPara.display.nNodeLabelTextHeight,f3dPoint(0.0),f3dPoint(0,0,0),3,2);
					}
#endif
				}
			}
		}
#if defined(__RAPID_ROUGH_3DMODEL_)&&defined(__LDS_)
		if(g_sysPara.display.bWirePoint)
		{	//��ʾ�ҵ�
			CXhWirePoint* pWirePoint;
			for(pWirePoint=gxWirePointModel.m_hashWirePoints.GetFirst();pWirePoint;pWirePoint=gxWirePointModel.m_hashWirePoints.GetNext())
			{
				CLDSNode* pRelaLoadNode=pWirePoint->m_hRelaNode>0x20?Ta.FromNodeHandle(pWirePoint->m_hRelaNode):NULL;
				if(pRelaLoadNode&&hashDrawNodes.GetValue(pRelaLoadNode->handle)==NULL)
					continue;	//�����ڵ㲻��ʾ
				if(pWirePoint->acs.axis_z.IsZero())
				{	//�趨Ĭ�Ϲ��߷���
					if(pWirePoint->m_xWirePlace.ciTensionType==1)
						pWirePoint->acs.axis_z.Set(0,0,1);	//��������Ĭ��Ǧ������
					else if(pWirePoint->m_xWirePlace.ciWireDirection=='X')
					{
						if(pWirePoint->xPosition.x<0)
							pWirePoint->acs.axis_z.Set(-1,0,0);
						else
							pWirePoint->acs.axis_z.Set( 1,0,0);
					}
					else //if(pWirePoint->m_xWirePlace.ciWireDirection=='Y')
					{
						if(pWirePoint->xPosition.y<0)
							pWirePoint->acs.axis_z.Set(0,-1,0);
						else
							pWirePoint->acs.axis_z.Set(0, 1,0);
					}
				}
				f3dPoint load,vStdArrowVec,basepoint;
				if (!pWirePoint->m_vSketchArrow.IsZero())
					load = pWirePoint->m_vSketchArrow;
				else
				{
					CExternalNodeLoad *pLoad = (pStatus != NULL) ? pStatus->hashNodeLoad.GetValue(pWirePoint->m_hRelaNode) : NULL;
					if (pLoad)
					{
						if (pStatus->m_bLoadIncFactor)
							load += pLoad->designLoad;
						else
						{
							load += pLoad->Fxyz;
							load.z += pLoad->permanentFz;
							load += pNode->PermanentLoad;
						}
					}
				}
				double dfArrowLen;
				if(!load.IsZero())
				{
					vStdArrowVec=load;
					normalize(vStdArrowVec);
					pWirePoint->acs.axis_z=vStdArrowVec;
					dfArrowLen=load.mod()/1000*g_sysPara.display.nLoadArrowScale;
				}
				else
				{
					dfArrowLen=2000;
					vStdArrowVec=pWirePoint->vWirePull;
				}

				g_pSolidSet->SetPen(g_sysPara.crTsaMode.crLoadArrow,PS_SOLID,2);
				pWirePoint->Create3dSolidModel();
				CSolidPart solidpart=pWirePoint->GetSolidPartObject(dfArrowLen);
				g_pSolidDraw->NewSolidPart(solidpart);
				ARROWOBJ arrow(pWirePoint->xPosition,vStdArrowVec,dfArrowLen);
				xarrArrowes.Append(arrow);
				//UINT index=0;
				//for(index=0;index<xarrArrowes.Count;index++)

				//ARROWOBJ arrow(basepoint,vStdArrowVec,nArrowLen);
				/*UINT index=0;
				for(index=0;index<xarrArrowes.Count;index++)
				{
					if(xarrArrowes[index].IsOverlapped(arrow))
						break;
				}
				if(index>=xarrArrowes.Count)
				{
					g_pSolidDraw->NewArrow(basepoint,vStdArrowVec,nArrowLen);
					xarrArrowes.Append(arrow);
				}*/

				f3dPoint align_vec=vStdArrowVec;
				f3dPoint dimpos=pWirePoint->xPosition+align_vec*(dfArrowLen*0.5);
				normalize(align_vec);
				g_pSolidSet->SetPen(RGB(255,255,0),PS_SOLID,2);
				CXhChar50 szWireName="����";
				if(pWirePoint->m_xWirePlace.ciWireType=='J')
					szWireName.Copy("����");
				else if(pWirePoint->m_xWirePlace.ciWireType=='E')
					szWireName.Copy("����");
				if(pWirePoint->m_sGimName.Length==0)
					gxWirePointModel.UpdateWirePointGimName(pWirePoint);
				if(pWirePoint->m_sGimName.Length>0)
					szWireName=pWirePoint->m_sGimName;
				g_pSolidDraw->NewText(szWireName,dimpos,g_sysPara.display.nRodDesignInfoTextHeight*2,vStdArrowVec,f3dPoint(0,0,0),ISolidDraw::TEXT_ALIGN_BTMCENTER,2);
			}
		}
#endif
		if(console.DispPartSet.GetNodeNum()>0)
		{
			//������ɫ
			if(g_sysPara.iColorSchema!=1)
			{
				if(g_sysPara.iColorSchema==2)		//������ɫ����
					pen.crColor = g_sysPara.crMaterialMode.crLine;
				else //if(g_sysPara.iColorSchema==3)//������ɫ����
					pen.crColor = g_sysPara.crPartMode.crLine;
				g_pSolidSet->SetPen(pen);
			}
			PARTSET displayPartSet;
#if defined(__LDS_)||defined(__TSA_)
			CMainFrame* pMainFrm=(CMainFrame*)theApp.m_pMainWnd;
#endif
			for(pPart=console.DispPartSet.GetFirst();pPart!=NULL;pPart=console.DispPartSet.GetNext())
			{
				if(!pPart->is_visible)
					continue;
				else
					pPart->is_visible=FALSE;
				if(theApp.m_bCooperativeWork&&!theApp.m_bDisplayAllPart&&!(pPart->dwPermission&dwPermWord))
					continue;
				if(pPart->IsAggregated())
					continue;
				if(Ta.segstr[0]=='0')
				{	//��ʾ�κ�Ϊ0�Ĺ����������޸Ķκ�
					if(pPart->iSeg.iSeg!=0)
						continue;
				}
				else
				//if(Ta.segstr[0]!='0'&& pPart->iSeg.iSeg==0) //���˵��κ�Ϊ����κ��ַ�����������Ĺ���
				//	continue;
				//if(pPart->iSeg.iSeg!=0)
				{	//��ʾͼ����ͼʱ���ܶκ����� ����������ͼ����һ�㶼����ʿɲ��ܶκ�����
					if(pDispView->m_iViewType==0&&segTbl.GetNodeNum()>0&&segTbl.GetValue(pPart->iSeg.iSeg)==NULL)
						continue;
				}
				if(!g_sysPara.display.bDispVirtualPart&&pPart->m_bVirtualPart)	//����ʾ��Ǹ�
					continue;
				else if(pPart->GetMotherClassTypeId()==CLS_BOLT&&!g_sysPara.display.bSolidLs)
					continue;
				else if(pPart->GetMotherClassTypeId()==CLS_PLATE&&!g_sysPara.display.bSolidLjb)
					continue;
				else if(pPart->GetMotherClassTypeId()==CLS_PARAMPLATE&&!g_sysPara.display.bSolidLjb)
					continue;
				else if(pPart->GetMotherClassTypeId()==CLS_LINETUBE&&!g_sysPara.display.bSolidTube)
					continue;
				else if(pPart->GetMotherClassTypeId()==CLS_LINEANGLE&&!g_sysPara.display.bSolidJg)
					continue;
				else if(pPart->GetMotherClassTypeId()==CLS_LINESLOT&&!g_sysPara.display.bSolidSlot)
					continue;
				else if(pPart->GetMotherClassTypeId()==CLS_SPHERE&&!g_sysPara.display.bSolidSphere)
					continue;
				else if(pPart->GetMotherClassTypeId()==CLS_LINEFLAT&&!g_sysPara.display.bSolidFlat)
					continue;
				else if(pPart->GetMotherClassTypeId()==CLS_ARCANGLE&&!g_sysPara.display.bSolidArcAngle)
					continue;
				else if(pPart->GetMotherClassTypeId()==CLS_ARCSLOT&&!g_sysPara.display.bSolidArcSlot)
					continue;
				else if(pPart->GetMotherClassTypeId()==CLS_ARCFLAT&&!g_sysPara.display.bSolidArcFlat)
					continue;
				else if(pPart->GetMotherClassTypeId()==CLS_ANCHORBOLT&&!g_sysPara.display.bSolidFoundation)
					continue;
				if(pPart->GetMotherClassTypeId()==CLS_POLYJOINT)
					continue;	//����ʵ�幹������ʾ
				if(pPart->layer(0)=='L')
				{
					if(pPart->IsLinePart())
					{
						f3dPoint lineStart,lineEnd;
						CLDSLinePart* pLinePart=(CLDSLinePart*)pPart;
						if(pLinePart->pStart)
							lineStart=pLinePart->pStart->ActivePosition();
						else
							lineStart=pLinePart->Start();
						if(pLinePart->pEnd)
							lineEnd=pLinePart->pEnd->ActivePosition();
						else
							lineEnd=pLinePart->End();
						char cPos=CalPoleQuad(lineStart,lineEnd);
						if(   (cPos=='1'&&pPart->cfgword.And(dwActiveQuad1LegWord))
							||(cPos=='2'&&pPart->cfgword.And(dwActiveQuad2LegWord))
							||(cPos=='3'&&pPart->cfgword.And(dwActiveQuad3LegWord))
							||(cPos=='4'&&pPart->cfgword.And(dwActiveQuad4LegWord)))
							pPart->is_visible=TRUE;
						else
							continue;
					}
					else
					{
						int iPos=CalPointQuadPos(pPart->ucs.origin);
						if(   (iPos==1&&pPart->cfgword.And(dwActiveQuad1LegWord))
							||(iPos==2&&pPart->cfgword.And(dwActiveQuad2LegWord))
							||(iPos==3&&pPart->cfgword.And(dwActiveQuad3LegWord))
							||(iPos==4&&pPart->cfgword.And(dwActiveQuad4LegWord)))
							pPart->is_visible=TRUE;
						else
							continue;
					}
				}
				else if(pPart->cfgword.And(dwBodyWord)||	//�����ͷ���˼�
					pPart->GetClassTypeId()==CLS_FITTINGS)	//��߲�����ĺ�����
					pPart->is_visible=TRUE;
				else
					continue;
#ifdef __ANGLE_PART_INC_
				if(disp_type==DISP_SOLID&&pPart->GetClassTypeId()==CLS_LINEANGLE)
				{
					CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pPart;
					if(pLineAngle->huoqu_handle>0)
					{
						Ta.Parts.push_stack();
						for(CLDSPolyJoint *pPolyJoint=(CLDSPolyJoint*)Ta.Parts.GetFirst(CLS_POLYJOINT);
							pPolyJoint;pPolyJoint=(CLDSPolyJoint*)Ta.Parts.GetNext(CLS_POLYJOINT))
						{
							if(pPolyJoint->pPrevLinePart==pLineAngle)
							{
								displayPartSet.append(pPolyJoint);
								break;
							}
						}
						Ta.Parts.pop_stack();
					}
				}
#endif
				displayPartSet.append(pPart);
			}
			//����������һ������ʵ�弰��ʾĿ����Ϊ���Ǽ��ٲ����ڴ���Ƭ
#ifdef __PART_DESIGN_INC_
			if(pMainFrm->m_bDisplayDeformedTower&&g_sysPara.iShowDeformedMapPlan>0)
			{
				theApp.m_eEnvType=FEMELEM_ENV;
				g_pSolidSet->SetSelectEntsColor(RGB(0,0,255));
				g_pSolidSet->SetSelectEntsPenStyle(PS_SOLID);
			}
			else
			{
				g_pSolidSet->SetSelectEntsColor(RGB(0,255,0));
				g_pSolidSet->SetSelectEntsPenStyle(PS_DASH);
			}
#endif

			if(theApp.m_eEnvType==LINE_ENV||theApp.m_eEnvType==FEMROD_ENV)
			{	//��ͨ����ģʽ
				for(pPart=displayPartSet.GetFirst();pPart;pPart=displayPartSet.GetNext())
				{
					if(!g_sysPara.display.bPoleLine&&pPart->IsLinePart())
						continue;
					if(pPart->IsLinePart())
					{
						CLDSLinePart* pRod=(CLDSLinePart*)pPart;
						if(g_sysPara.iColorSchema==1)
						{	//TSA��ɫ����
#ifdef __PART_DESIGN_INC_
							if(((CLDSLinePart*)pPart)->IsAuxPole())
								pen.crColor = g_sysPara.crTsaMode.crAuxLine;
							else if(((CLDSLinePart*)pPart)->IsPostRod())
								pen.crColor = g_sysPara.crTsaMode.crPostLine;
							else
								pen.crColor = g_sysPara.crTsaMode.crPrimaryLine;
#endif
						}
						else if(g_sysPara.iColorSchema==2)		//������ɫ����
							pen.crColor = g_sysPara.crMaterialMode.crLine;
						else //if(g_sysPara.iColorSchema==3)//������ɫ����
							pen.crColor = g_sysPara.crPartMode.crNode;
						g_pSolidSet->SetPen(pen);
						if(  pRod->pStart!=NULL&&pRod->pEnd!=NULL&&g_sysPara.display.bDimRodDesignInfo&&
							(pRod->GetClassTypeId()!=CLS_LINEANGLE||((CLDSLineAngle*)pRod)->group_father_jg_h<0x20))
						{
							CXhChar50 spec;
							pRod->GetSpecification(spec,FALSE);
							if(pRod->cMaterial!=CSteelMatLibrary::Q235BriefMark())
								spec.Append(pRod->cMaterial);
							WORD wnStartDisplayBoltN=(theApp.m_eEnvType==FEMROD_ENV)?pRod->connectStart.wnSummBoltN:pRod->connectStart.wnConnBoltN;
							WORD wnEndDisplayBoltN  =(theApp.m_eEnvType==FEMROD_ENV)?pRod->connectEnd.wnSummBoltN:pRod->connectEnd.wnConnBoltN;

							if(pRod->connectStart.d==pRod->connectEnd.d)
							{
								spec.Append(CXhChar16(" M%d",pRod->connectStart.d));
								if(wnStartDisplayBoltN==wnEndDisplayBoltN&&wnStartDisplayBoltN>=1)
									spec.Append(CXhChar16("X%d",wnStartDisplayBoltN));
								else if(wnStartDisplayBoltN!=wnEndDisplayBoltN)
									spec.Append(CXhChar16("X%d~%d",wnStartDisplayBoltN,wnEndDisplayBoltN));
							}
							else
								spec.Append(CXhChar16(" M%dX%d~M%dX%d",pRod->connectStart.d,wnStartDisplayBoltN,pRod->connectEnd.d,wnEndDisplayBoltN));

							if(!theApp.m_bAnalysisWorkEnv)		//TSA��ɫ����
							{
								spec.Append(" \'");
								spec.Append(pRod->GetPartNo());
								spec.Append('\'');
							}
							f3dPoint align_vec=pRod->pEnd->Position(false)-pRod->pStart->Position(false);
							f3dPoint dimpos=pRod->pStart->Position(false)+align_vec*0.25;
							normalize(align_vec);
							g_pSolidDraw->NewText(spec,dimpos,g_sysPara.display.nRodDesignInfoTextHeight,align_vec,f3dPoint(0,0,0),7,2);
						}
						if(g_sysPara.iColorSchema==3)//������ɫ����
						{
							pen.crColor = g_sysPara.crPartMode.crLine;
							g_pSolidSet->SetPen(pen);
						}
					}
					pCurAtom = pPart;	//��¼������
					CSolidBody *pSolidBody=pPart->pSolidBody;
					pPart->pSolidBody=NULL;	//�ӿ쵥��ͼ��ʾ�ٶȣ���ΪNULL���ݲ�����ʵ����ʾ��
					g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
					pPart->pSolidBody=pSolidBody;
				}
			}
#ifdef __ANGLE_PART_INC_
			else if(disp_type==DISP_SOLID)
			{	//ʵ����ʾģʽ
				CMutiThreadTask solidModelThread[2],solidDrawThread;
				solidModelThread[0].TaskFunc=solidModelThread[1].TaskFunc=&CreatePart3dSolidModel;
				solidModelThread[0].m_pContext=solidModelThread[1].m_pContext=&solidDrawThread;
				solidDrawThread.TaskFunc = &DrawPart3dSolidModel;
				bool odd=false;
				HOLE_WALL* pHoleWall;
				for(pHoleWall=console.hashHoles.GetFirst();pHoleWall;pHoleWall=console.hashHoles.GetNext())
				{	//����Ӵ˶δ�����ܻ�����֮ǰ��ʾ�Ŀף������޷�������˨��ʵ��
					//ĳЩ����¿��ܻ������˨��ɾ������δ֪ͨ����̨�Ƴ������ױڶ�������������˴����� wjh-2016.3.23
					pHoleWall->pBolt=(CLDSBolt*)console.FromPartHandle(pHoleWall->hBolt,CLS_BOLT);
					if(pHoleWall->is_visible&&pHoleWall->pBolt&&pHoleWall->pBolt->pSolidBody)
						pHoleWall->pBolt->pSolidBody->m_bModified=TRUE;
				}
				console.hashHoles.Empty();
				CHashList<bool>hashVisibleParts;
				for(pPart=displayPartSet.GetFirst();pPart;pPart=displayPartSet.GetNext())
				{
					hashVisibleParts.Add(pPart->handle);
					pCurAtom = pPart;	//��¼������
					/*if(!odd)	//ż��
						solidModelThread[0].AddTask(pPart);
					else
						solidModelThread[1].AddTask(pPart);
					odd=!odd;*/
#ifdef __GGG_
					pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),
						g_pSolidOper->GetScaleUserToScreen(),0);
#else
					pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),
						g_pSolidOper->GetScaleUserToScreen(),g_sysPara.display.nSmoothness);
#endif				
					if(pPart->IsPlate()&&((CLDSGeneralPlate*)pPart)->m_cPickedDisplayMode!=0)
						CLDSView::RedrawPlateProfileDrawing((CLDSGeneralPlate*)pPart);
					g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
					if(pPart->IsLinePart() && CLDSLineAngle::m_bDisplaySolidAxisLine)
					{
						if(pPart->GetClassTypeId()==CLS_LINEANGLE)
							CLDSView::RedrawAngleFrameDrawing((CLDSLineAngle*)pPart,0,&object_ucs,false);
						else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
							CLDSView::RedrawFlatFrameDrawing((CLDSLineFlat*)pPart,0,&object_ucs,false);
						else if(pPart->GetClassTypeId()==CLS_LINESLOT)
							CLDSView::RedrawSlotFrameDrawing((CLDSLineSlot*)pPart,0,&object_ucs,false);
					}
					pPart->GatherHoleWalls(console.hashHoles);
				}
				if(g_sysPara.display.bSolidFoundation)
				{
					for(CLDSFoundation *pFoundation=console.DispFoundationSet.GetFirst();pFoundation;pFoundation= console.DispFoundationSet.GetNext())
					{
						BYTE ciCurrQuadActiveLegSerial=(pModule->m_arrActiveQuadLegNo[pFoundation->ciLegQuad-1]-1)%CFGLEG::MaxLegs()+1;
						if(pFoundation->ciHeightSerial!=pModule->idBodyNo||pFoundation->ciLegSerial!=ciCurrQuadActiveLegSerial)
							continue;
						pFoundation->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pFoundation->GetSolidPartObject());
					}
				}
				for(pHoleWall=console.hashHoles.GetFirst();pHoleWall;pHoleWall=console.hashHoles.GetNext())
				{
					if(pHoleWall->pBolt->is_visible&&hashVisibleParts.GetValue(pHoleWall->pBolt->handle)==NULL)
						pHoleWall->pBolt->is_visible=false;
					pHoleWall->is_visible=(pHoleWall->pBolt->is_visible==FALSE || pHoleWall->pBolt->m_bVirtualPart);
					if(!pHoleWall->is_visible)
						continue;
					pHoleWall->Create3dSolidModel(g_pSolidOper->GetScaleUserToScreen(),g_pSolidOper->GetScaleUserToScreen(),g_sysPara.display.nSmoothness);
					g_pSolidDraw->NewSolidPart(pHoleWall->GetSolidPartObject());
				}
				solidModelThread[0].m_bWaitOnIdle=solidModelThread[1].m_bWaitOnIdle=false;
				solidDrawThread.m_bWaitOnIdle=false;
				WaitForSingleObject(solidModelThread[0].GetThreadHandle(),INFINITE);
				WaitForSingleObject(solidModelThread[1].GetThreadHandle(),INFINITE);
				WaitForSingleObject(solidDrawThread.GetThreadHandle(),INFINITE);
				//��ӳߴ�У����
				if(pBuddyDrawing&&theApp.m_bManuRevisionMode&&PRODUCT_FUNC::IsHasInternalTest())
				{
					for(CDataCheckModel::SIZE_DATA* pCoordSize=xDataCheckModel.EnumFirstCoordSizeItem();pCoordSize;pCoordSize=xDataCheckModel.EnumNextCoordSizeItem())
						CLDSView::AppendSizeCoordCheckItem(pCoordSize);
				}
			}
#endif
#ifdef __PART_DESIGN_INC_
			else if(theApp.m_eEnvType==FEMELEM_ENV)
			{	//��ԪУ�����ģʽ����ʾ���ԣ��Ԥ��ʱֻ��ʾ��Ԫ������ʾ�˼�
				CHashTable<CLDSLinePart*> hashRods;
				hashRods.CreateHashTable(displayPartSet.GetNodeNum());
				for(pPart=displayPartSet.GetFirst();pPart;pPart=displayPartSet.GetNext())
				{
					if(pPart->IsLinePart())
						hashRods.SetValueAt(pPart->handle,(CLDSLinePart*)pPart);
				}
				if(Ta.Elems.GetNodeNum()==0)
					preprocessor.InitSourceDesignInfo(&Ta);
				f3dLine line;
				for(CElemInfo *pElemInfo=Ta.Elems.GetFirst();pElemInfo;pElemInfo=Ta.Elems.GetNext())
				{
					CLDSLinePart *pParentRod=NULL;
					if(!hashRods.GetValueAt(pElemInfo->m_hParentRod,pParentRod))
						continue;
					CLDSNode *pStart=console.FromNodeHandle(pElemInfo->m_hStartNode);
					CLDSNode *pEnd = console.FromNodeHandle(pElemInfo->m_hEndNode);
					if(pStart==NULL||pEnd==NULL)
						continue;
					//�����һ��ʱ������������޷���ѡ�б��ι���������ԭ����λ�ã��ʸ�Ϊһ��
					//����Ϊʲô�ĳɲ�һ�����ݲ���������ǵ����û�ѡ�б��ι�����ɾ��Ե�ʰɣ�wjh-2010-11-09
					line.ID=pElemInfo->Id|0x80000000;	//line.ID=-pLinePart->handle;
					line.startPt=pStart->Position(false);
					line.endPt=pEnd->Position(false);
					CXhChar50 dimspec;
					CElement *pRelaElem=NULL;
					if(pMainFrm->m_bDisplayDeformedTower)
					{	//��Ԫ��������Чϵ��
						//if(postprocessor.hashRodRelaElem.GetValue(pElemInfo->m_hParentRod))
						//	pRelaElem=postprocessor.hashRodRelaElem[pElemInfo->m_hParentRod];
						long hStart=pStart->handle;
						long hEnd=pEnd->handle;
						if(!pElemInfo->relaElem.bFatherElem&&pElemInfo->relaElem.xFatherElem.pElemInfo==NULL)
						{	//Ӧ��xFatherElem.pElemInfo==NULL��xFatherElem.hStartNode,hEndNode>0ʱ���Լ���
							pElemInfo->relaElem.xFatherElem.pElemInfo=
							Ta.Elems.GetValue(pElemInfo->relaElem.xFatherElem.hStartNode,pElemInfo->relaElem.xFatherElem.hEndNode);
						}
						if(!pElemInfo->relaElem.bFatherElem&&pElemInfo->relaElem.xFatherElem.pElemInfo!=NULL)
						{
							CLDSNode* pMirStart=console.FromNodeHandle(pElemInfo->relaElem.xFatherElem.pElemInfo->m_hStartNode);
							CLDSNode* pMirEnd=console.FromNodeHandle(pElemInfo->relaElem.xFatherElem.pElemInfo->m_hEndNode);
							hStart=pMirStart->handle;
							hEnd=pMirEnd->handle;
						}
						ELEM_PTR* ppElem=postreport.ElemPtrs.GetValue(hStart,hEnd);
						if(ppElem)
							pRelaElem=*ppElem;
					}
					if( g_sysPara.display.bDimRodDesignInfo)
					{
						pParentRod->GetSpecification(dimspec,FALSE);
						if(pParentRod->cMaterial!=CSteelMatLibrary::Q235BriefMark())
							dimspec.Append(pParentRod->cMaterial);
						if(pRelaElem&&g_sysPara.iShowDeformedMapPlan==0)	//��Ԫ����ϵ��ͼ
						{
							CXhChar16 caserule;
							if(pRelaElem->design_info.iDangerousLoadCase==2)
							{
								strcpy(caserule,"T");
								CWorkStatus* pStatus=Ta.WorkStatus.FromHandle(pRelaElem->MaxTension.hWorkStatus.Id());
								if(pStatus)
									caserule.Append(CXhChar16("@%d",pStatus->iNo));
							}
							else if(pRelaElem->design_info.iDangerousLoadCase==3)
							{
								strcpy(caserule,"C");
								CWorkStatus* pStatus=Ta.WorkStatus.FromHandle(pRelaElem->MaxCompression.hWorkStatus.Id());
								if(pStatus)
									caserule.Append(CXhChar16("@%d",pStatus->iNo));
							}
							else if(pRelaElem->design_info.iDangerousLoadCase==4)
							{
								strcpy(caserule,"S");
								CWorkStatus* pStatus=Ta.WorkStatus.FromHandle(pRelaElem->MaxCompStable.hWorkStatus.Id());
								if(pStatus)
									caserule.Append(CXhChar16("@%d",pStatus->iNo));
							}
							if(dimspec.GetLength()>0)
								dimspec.Append("    ");
							dimspec.Append(CXhChar50("%.0f%%%s",pRelaElem->design_info.fCoefOfBearLoad*100,(char*)caserule));
						}
					}
					if(pMainFrm->m_bDisplayDeformedTower&&g_sysPara.iShowDeformedMapPlan>0)
					{	//��Ԫ��������Чϵ��
						/*if(pRelaElem)	
							pRelaElem=postreport.hashElems.GetValue(pRelaElem->id);*/
						double coef_effic=0;
						CXhChar16 caserule;
						if(pRelaElem==NULL)
							line.pen.crColor=g_sysPara.CalEfficColor(0);
						else
						{
							if(g_sysPara.iShowDeformedMapPlan==1)		//��ԪЧ��ϵ��ͼ
							{
								coef_effic=pRelaElem->design_info.fCoefEffic;
								if(pRelaElem->design_info.iDangerousRule==1)
#ifdef AFX_TARG_ENU_ENGLISH
									strcpy(caserule,"��");                                       
#else
									strcpy(caserule,"��");
#endif
								else if(pRelaElem->design_info.iDangerousLoadCase==2)
								{
									strcpy(caserule,"T");
									CWorkStatus* pStatus=Ta.WorkStatus.FromHandle(pRelaElem->MaxTension.hWorkStatus.Id());
									if(pStatus)
										caserule.Append(CXhChar16("@%d",pStatus->iNo));
								}
								else if(pRelaElem->design_info.iDangerousLoadCase==3)
								{
									strcpy(caserule,"C");
									CWorkStatus* pStatus=Ta.WorkStatus.FromHandle(pRelaElem->MaxCompression.hWorkStatus.Id());
									if(pStatus)
										caserule.Append(CXhChar16("@%d",pStatus->iNo));
								}
								else if(pRelaElem->design_info.iDangerousLoadCase==4)
								{
									strcpy(caserule,"S");
									CWorkStatus* pStatus=Ta.WorkStatus.FromHandle(pRelaElem->MaxCompStable.hWorkStatus.Id());
									if(pStatus)
										caserule.Append(CXhChar16("@%d",pStatus->iNo));
								}
							}
							else if(g_sysPara.iShowDeformedMapPlan==2)	//��Ԫ����ϵ��ͼ
							{
								coef_effic=pRelaElem->design_info.fCoefOfBearLoad;
								if(pRelaElem->design_info.iDangerousLoadCase==2)
								{
									strcpy(caserule,"T");
									CWorkStatus* pStatus=Ta.WorkStatus.FromHandle(pRelaElem->MaxTension.hWorkStatus.Id());
									if(pStatus)
										caserule.Append(CXhChar16("@%d",pStatus->iNo));
								}
								else if(pRelaElem->design_info.iDangerousLoadCase==3)
								{
									strcpy(caserule,"C");
									CWorkStatus* pStatus=Ta.WorkStatus.FromHandle(pRelaElem->MaxCompression.hWorkStatus.Id());
									if(pStatus)
										caserule.Append(CXhChar16("@%d",pStatus->iNo));
								}
								else if(pRelaElem->design_info.iDangerousLoadCase==4)
								{
									strcpy(caserule,"S");
									CWorkStatus* pStatus=Ta.WorkStatus.FromHandle(pRelaElem->MaxCompStable.hWorkStatus.Id());
									if(pStatus)
										caserule.Append(CXhChar16("@%d",pStatus->iNo));
								}
							}
							else					//��Ԫ����ϵ��ͼ
							{
								STEELMAT *pSteel=QuerySteelMatByBriefMark(pRelaElem->pOrgPole->cMaterial);
								double An=pRelaElem->pOrgPole->GetArea()-pRelaElem->design_info.hole_out*pRelaElem->GetThick();
								if(-pRelaElem->MaxCompression.N>pRelaElem->MaxTension.N)
								{
									coef_effic=-pRelaElem->MaxCompression.N/(An*pSteel->f(pRelaElem->GetThick()));
									strcpy(caserule,"C");
									CWorkStatus* pStatus=Ta.WorkStatus.FromHandle(pRelaElem->MaxCompression.hWorkStatus.Id());
									if(pStatus)
										caserule.Printf("@%d",pStatus->iNo);
								}
								else
								{
									coef_effic= pRelaElem->MaxTension.N/(An*pSteel->f(pRelaElem->GetThick()));
									strcpy(caserule,"T");
									CWorkStatus* pStatus=Ta.WorkStatus.FromHandle(pRelaElem->MaxTension.hWorkStatus.Id());
									if(pStatus)
										caserule.Printf("@%d",pStatus->iNo);
								}
							}
							line.pen.crColor=g_sysPara.CalEfficColor(coef_effic);
						}
						if( g_sysPara.display.bDimRodDesignInfo)
						{
							if(dimspec.GetLength()>0)
								dimspec.Append("    ");
							dimspec.Append(CXhChar16("%.0f%%%s",coef_effic*100,(char*)caserule));
						}
						/*CRodIncIntervals *pRodInterval=preprocessor.result.hashRodIntervals.GetValue(pPart->handle);
						if(pRodInterval)
						{
							for(ELEM_PTR *ppElem=pRodInterval->listElems.GetFirst();ppElem;ppElem=pRodInterval->listElems.GetNext())
							{
								CElement *pElem=*ppElem;
								line.ID=pPart->handle;	//line.ID=-pLinePart->handle;
								line.startPt=pElem->pStart->Position(false);
								line.endPt=pElem->pEnd->Position(false);
								g_pSolidDraw->NewLine(line);
							}
						}*/
					}
					else if(g_sysPara.iColorSchema==1)		//TSA��ɫ����
					{
						if(pParentRod->IsAuxPole())
							line.pen.crColor = g_sysPara.crTsaMode.crAuxLine;
						else if(pParentRod->IsPostRod())
							line.pen.crColor = g_sysPara.crTsaMode.crPostLine;
						else
							line.pen.crColor = g_sysPara.crTsaMode.crPrimaryLine;
					}
					//line.pen.style=PS_DASH;
					if(dimspec.GetLength()>0)
					{
						f3dPoint align_vec=line.endPt-line.startPt;
						f3dPoint dimpos=line.startPt+align_vec*0.35;
						normalize(align_vec);
						g_pSolidSet->SetPen(g_sysPara.crTsaMode.crDimText,PS_SOLID,1);
						g_pSolidDraw->NewText(dimspec,dimpos,g_sysPara.display.nRodDesignInfoTextHeight,align_vec,f3dPoint(0,0,0),7,2);
					}
					g_pSolidDraw->NewLine(line);
				}
			}
			//������ʾ����λ�ƺ�ı���ͼ
			if(g_bVibrationAnimate)
			{	//����ģ������ģʽ
				for(pPart=displayPartSet.GetFirst();pPart;pPart=displayPartSet.GetNext())
				{
					if(!pPart->IsLinePart())
						continue;	//�������ι���
					f3dLine line;
					NODESET nodeset;
					CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
					if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
						continue;	//���Ǳ�׼�ļ���ѧ����ģ���еĸ˼�
					SelectNodeInPole(pLinePart,nodeset);
					CLDSNode *pStartNode=NULL,*pEndNode=NULL;
					pStartNode = nodeset.GetFirst();
					for(pEndNode=nodeset.GetNext();pEndNode;pEndNode=nodeset.GetNext())
					{
						line.pen.crColor=g_sysPara.crTsaMode.crDeformedPoleLine;
						line.ID=-pLinePart->handle;
						f3dPoint offset=pStartNode->m_offsetAmplitude*phasePositionCoef;
						line.startPt=pStartNode->Position(false)+offset;
						offset=pEndNode->m_offsetAmplitude*phasePositionCoef;
						line.endPt=pEndNode->Position(false)+offset;
						line.pen.style=PS_DASH;
						g_pSolidDraw->NewLine(line);
						pStartNode = pEndNode;
					}
				}
			}
			else if(pMainFrm->m_bDisplayDeformedTower&&g_sysPara.iShowDeformedMapPlan==0)
			{	//��ʾ��ѧ����λ�Ʊ���ͼ
				long iStatusNo=0;
				if(pStatus)
					iStatusNo=pStatus->iNo;
				for(pPart=displayPartSet.GetFirst();pPart;pPart=displayPartSet.GetNext())
				{
					if(!pPart->IsLinePart())
						continue;	//�������ι���
					f3dLine line;
					NODESET nodeset;
					CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
					if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
						continue;	//���Ǳ�׼�ļ���ѧ����ģ���еĸ˼�
					SelectNodeInPole(pLinePart,nodeset);
					CLDSNode *pStartNode=NULL,*pEndNode=NULL;
					pStartNode = nodeset.GetFirst();
					for(pEndNode=nodeset.GetNext();pEndNode;pEndNode=nodeset.GetNext())
					{
						if(!pEndNode->is_visible)
							continue;	//�ǵ�ǰ���ȴ��ڲ���ʾ״̬
						line.pen.crColor=g_sysPara.crTsaMode.crDeformedPoleLine;
						//�����һ��ʱ������������޷���ѡ�б��ι���������ԭ����λ�ã��ʸ�Ϊһ��
						//����Ϊʲô�ĳɲ�һ�����ݲ���������ǵ����û�ѡ�б��ι�����ɾ��Ե�ʰɣ�wjh-2010-11-09
						line.ID=pLinePart->handle;	//line.ID=-pLinePart->handle;
						CNodeAnalyzeInfo analInfo(pStartNode->handle,pModule->iNo,pModule->ActiveQuadLegDword(),iStatusNo);
						postreport.hashNodeAnalBuffer.GetObject(analInfo.GetCompositeKey(),&analInfo);
						line.startPt=pStartNode->Position(false)+analInfo.offset;
						analInfo=CNodeAnalyzeInfo(pEndNode->handle,pModule->iNo,pModule->ActiveQuadLegDword(),iStatusNo);
						postreport.hashNodeAnalBuffer.GetObject(analInfo.GetCompositeKey(),&analInfo);
						line.endPt=pEndNode->Position(false)+analInfo.offset;
						line.pen.style=PS_DASH;
						g_pSolidDraw->NewLine(line);
						pStartNode = pEndNode;
					}
				}
			}
#endif
		}
		if(disp_type==DISP_SOLID)
		{	//��ʾ��������ʵ��,��ʱ��ʾ���еĲ������� 
			for(CBlockReference *pBlockRef=console.DispBlockRefSet.GetFirst();pBlockRef;pBlockRef=console.DispBlockRefSet.GetNext())
			{
				if(Ta.segstr[0]=='0')
				{	//��ʾ�κ�Ϊ0�Ĺ����������޸Ķκ�
					if(pBlockRef->iSeg.iSeg!=0)
						continue;
				}
				else
				//if(Ta.segstr[0]!='0'&& pBlockRef->iSeg.iSeg==0) //���˵��κ�Ϊ����κ��ַ�����������Ĳ�������
				//	continue;
				//if(pBlockRef->iSeg.iSeg!=0)
				{	//��ʾͼ����ͼʱ���ܶκ����� ����������ͼ����һ�㶼����ʿɲ��ܶκ�����
					if(pDispView->m_iViewType==0&&segTbl.GetNodeNum()>0&&segTbl.GetValue(pBlockRef->iSeg.iSeg)==NULL)
						continue;
				}
				pBlockRef->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBlockRef->GetSolidPartObject());
			}
		}
	}
	catch(CXhChar100 sError)
	{
		if(pCurAtom)
			sError.Append(CXhChar16("@0X%X",pCurAtom->handle));
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox(AfxGetMainWnd()->GetSafeHwnd(),sError,"error",MB_ICONERROR);
#else
		MessageBox(AfxGetMainWnd()->GetSafeHwnd(),sError,"����",MB_ICONERROR);
#endif
	}
	catch(char *error)
	{
		//HINSTANCE hOldResHandle=AfxGetResourceHandle();
		//AfxSetResourceHandle(m_hResHandle);
		CXhChar100 sError(error);
		if(pCurAtom)
			sError.Append(CXhChar16("@0X%X",pCurAtom->handle));
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox(AfxGetMainWnd()->GetSafeHwnd(),sError,"error",MB_ICONERROR);
#else
		MessageBox(AfxGetMainWnd()->GetSafeHwnd(),sError,"����",MB_ICONERROR);
#endif
		//AfxMessageBox(sError);
		//AfxSetResourceHandle(hOldResHandle);
	}
}
#ifdef __GGG_
void Draw2DTower(CDrawEngine* pDraw)
{
	CLDSView *pView=(CLDSView*)pDraw->dwParam;
}
#endif
void CLDSView::OnDraw(CDC* pDC)
{
	CLDSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
#ifdef __GGG_
	if(theApp.m_eEnvType==DRAWING2D_ENV)
		m_pDraw2d->Draw(pDC);
	else
#endif
		g_pSolidDraw->Draw();
}
void LiveDrawFunc(LIVE_DISPLAY_CONTEXT* pContext)
{
	CDC* pDC = CDC::FromHandle(pContext->hDC);
	pDC->SetBkMode(TRANSPARENT);
#ifdef __PART_DESIGN_INC_
	CMainFrame* pMainFrm=(CMainFrame*)theApp.m_pMainWnd;
	if(pMainFrm->m_bDisplayDeformedTower&&g_sysPara.iShowDeformedMapPlan>0)
	{
		//DWORD n=g_sysPara.efficColorBarScaleArr.Size();
		DWORD n=0;
		for(n=0;n<6;n++)
		{
			if(g_sysPara.crTsaMode.crElemDivArr[n].efficPercent>100)
				break;
		}
		//if(g_sysPara.crTsaMode.crElemDivArr[0].efficPercent>0)
		//	n++;
		g_sysPara.efficColorBarScaleArr.Resize(n);
		for(n=0;n<g_sysPara.efficColorBarScaleArr.Size();n++)
		{
			//if(g_sysPara.crTsaMode.crElemDivArr[0].efficPercent>0)
			//	g_sysPara.efficColorBarScaleArr[n+1]=g_sysPara.crTsaMode.crElemDivArr[n].efficPercent*0.01;
			//else
				g_sysPara.efficColorBarScaleArr[n]=g_sysPara.crTsaMode.crElemDivArr[n].efficPercent*0.01;
		}
		CPoint pos(10,10);
		CPen *pOldPen=NULL;
		const int minspan=30;
		COLORREF warningclr=g_sysPara.CalEfficColor(1.1);	//����100%
		pDC->SetTextColor(RGB(0,0,0));
		pDC->FillSolidRect(pos.x,pos.y,20,5,warningclr);
		CPen warnpen(PS_SOLID,1,warningclr);
		pOldPen=pDC->SelectObject(&warnpen);
		pDC->MoveTo(pos.x+20,pos.y+2);
		pDC->LineTo(pos.x+30,pos.y+2);
		pDC->SelectObject(pOldPen);
#ifdef AFX_TARG_ENU_ENGLISH
		pDC->TextOut(pos.x+30,pos.y-6,"overrun",4);
#else
		pDC->TextOut(pos.x+30,pos.y-6,"����",4);
#endif
		pos.y+=20;
		double prev_effic=1.0;
		for(DWORD i=0;i<=n;i++)
		{
			int index=n-i-1;
			double now_effic;
			if(index>=0)
				now_effic=g_sysPara.efficColorBarScaleArr[index];
			else
				now_effic=0;
			int span=ftoi(100*(prev_effic-now_effic));
			if(span==0)
				continue;
			span=max(span,minspan);
			if(span%2>0)
				span+=1;
			COLORREF rgb=g_sysPara.CalEfficColor(prev_effic);
			CPen pen(PS_SOLID,1,rgb);
			for(int j=0;j<span;j+=2)
			{
				double effic=prev_effic-j*(prev_effic-now_effic)/span;
				rgb = g_sysPara.CalEfficColor(effic);
				pDC->FillSolidRect(pos.x,pos.y+j,20,2,rgb);
			}
			//pDC->SetBkColor(g_sysPara.crTsaMode.crBackground);
			CString scale;
			scale.Format("%.0f%%",100*prev_effic);
			pOldPen=pDC->SelectObject(&pen);
			pDC->MoveTo(pos.x+20,pos.y);
			pDC->LineTo(pos.x+30,pos.y);
			pDC->SelectObject(pOldPen);
			pDC->TextOut(pos.x+30,pos.y,scale);
			if(i==n)
			{
				CPen zeropen(PS_SOLID,1,g_sysPara.CalEfficColor(0.001));
				pOldPen=pDC->SelectObject(&zeropen);
				pDC->MoveTo(pos.x+20,pos.y+span-1);
				pDC->LineTo(pos.x+30,pos.y+span-1);
				pDC->SelectObject(pOldPen);
				scale="��0";
				pDC->TextOut(pos.x+30,pos.y+span-8,scale);
			}
			pos.y+=span;
			prev_effic=now_effic;
		}
		//�������
		CPen blackpen(PS_SOLID,1,RGB(0,0,0));
		pos.y+=20;//minspan;
		pDC->FillSolidRect(pos.x,pos.y,20,5,RGB(0,0,0));
		pOldPen=pDC->SelectObject(&blackpen);
		pDC->MoveTo(pos.x+20,pos.y+2);
		pDC->LineTo(pos.x+30,pos.y+2);
		pDC->SelectObject(pOldPen);
#ifdef AFX_TARG_ENU_ENGLISH
		pDC->TextOut(pos.x+30,pos.y-7,"zero element",4);
#else
		pDC->TextOut(pos.x+30,pos.y-7,"���",4);
#endif
	}
	RECT rc;
	pMainFrm->GetLDSView()->GetClientRect(&rc);
	pDC->SetTextColor(RGB(255,0,0));
	if(theApp.m_eEnvType==FEMELEM_ENV)
#ifdef AFX_TARG_ENU_ENGLISH
		pDC->TextOut(rc.left+1,rc.bottom-15,"element display mode",12);
	else if(theApp.m_eEnvType==FEMROD_ENV)
		pDC->TextOut(rc.left+1,rc.bottom-15,"bar display mode",12); 
#else
		pDC->TextOut(rc.left+1,rc.bottom-15,"��Ԫ��ʾģʽ",12);
	else if(theApp.m_eEnvType==FEMROD_ENV)
		pDC->TextOut(rc.left+1,rc.bottom-15,"�˼���ʾģʽ",12);
#endif
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CLDSView printing

BOOL CLDSView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLDSView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLDSView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CLDSView diagnostics

#ifdef _DEBUG
void CLDSView::AssertValid() const
{
	CView::AssertValid();
}

void CLDSView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLDSDoc* CLDSView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLDSDoc)));
	return (CLDSDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLDSView message handlers
long FireEntPickEvent(long ID)
{
	/*HINSTANCE hOldResHandle=AfxGetResourceHandle();
	AfxSetResourceHandle(m_hResHandle);
	//...
	long ret_id=ID;
	CLDSGroupLineAngle *pGroupJg;
	if(console.FromPartHandle(ID,CLS_LINEANGLE))
		goto end;	//��ֱͨ�Ǹ�
	else
		pGroupJg=(CLDSGroupLineAngle*)console.FromPartHandle(ID,CLS_GROUPLINEANGLE);
	if(pGroupJg==NULL)
		goto end;
	ReleaseCapture();	//������Ҫ���ε�����ܶԵ����ĶԻ������������
	if(pGroupJg->group_style==0)	//�Խ����
	{
		CSel2CrossGroupJgDlg dlg;
		dlg.DoModal();
		if(dlg.m_iCurOperJg==0)
			goto end;
		else if(dlg.m_iCurOperJg==1)
		{
			ID=pGroupJg->son_jg_h[0];
			goto end;
		}
		else if(dlg.m_iCurOperJg==2)
		{
			ID=pGroupJg->son_jg_h[1];
			goto end;
		}
		else
			goto end;
	}
	else if(pGroupJg->group_style==1)	//T�����
	{
		CSelTGroupJgDlg dlg;
		dlg.DoModal();
		if(dlg.m_iCurOperJg==0)
			goto end;
		else if(dlg.m_iCurOperJg==1)
		{
			ID=pGroupJg->son_jg_h[0];
			goto end;
		}
		else if(dlg.m_iCurOperJg==2)
		{
			ID=pGroupJg->son_jg_h[1];
			goto end;
		}
		else
			goto end;
	}
	else if(pGroupJg->group_style==2)	//ʮ�ֽ������
	{
		CSel4CrossGroupJgDlg dlg;
		dlg.DoModal();
		if(dlg.m_iCurOperJg==0)
			goto end;
		else if(dlg.m_iCurOperJg==1)
		{
			ID= pGroupJg->son_jg_h[0];
			goto end;
		}
		else if(dlg.m_iCurOperJg==2)
		{
			ID=pGroupJg->son_jg_h[1];
			goto end;
		}
		else if(dlg.m_iCurOperJg==3)
		{
			ID=pGroupJg->son_jg_h[2];
			goto end;
		}
		else if(dlg.m_iCurOperJg==4)
		{
			ID=pGroupJg->son_jg_h[3];
			goto end;
		}
		else
			goto end;
	}
	//...
end:
	AfxSetResourceHandle(hOldResHandle);*/
	return ID;
}
bool FireEntSnapPosition(long ID,const double* front_pos,const double* back_pos,double* nearest_pos)
{
	CSuperSmartPtr<CLDSPart> pPart=Ta.Parts.FromHandle(ID);
	if(pPart.IsNULL())
	{
		CLDSNode* pNode=Ta.Node.FromHandle(ID);
		if(pNode)
		{
			nearest_pos[0]=pNode->Position('X');
			nearest_pos[1]=pNode->Position('Y');
			nearest_pos[2]=pNode->Position('Z');
		}
		return true;
	}
	f3dPoint front(front_pos),back(back_pos);
	f3dPoint depth_vec=back-front;
	normalize(depth_vec);
	GEPOINT inters;
	if(pPart->IsLinePart())
	{
		GECS cs;
		f3dLine rodline(pPart.LinePartPointer()->Start(),pPart.LinePartPointer()->End());
		DISPLAY_TYPE display_type;
		g_pSolidSet->GetDisplayType(&display_type);
		if(display_type==DISP_LINE&&pPart.LinePartPointer()->pStart!=NULL&&pPart.LinePartPointer()->pEnd!=NULL)
		{
			rodline.startPt=pPart.LinePartPointer()->pStart->Position();
			rodline.endPt=pPart.LinePartPointer()->pEnd->Position();
		}
		cs.origin=rodline.startPt;
		cs.axis_x=rodline.endPt-rodline.startPt;
		cs.axis_z=cs.axis_x^depth_vec;
		normalize(cs.axis_x);
		normalize(cs.axis_z);
		cs.axis_y=cs.axis_z^cs.axis_x;
		front=cs.TransPToCS(front);
		back=cs.TransPToCS(back);
		front.z=back.z=0;
		double length=DISTANCE(rodline.startPt,rodline.endPt);
		int rslt=Int2dll(f2dLine(f2dPoint(0,0),f2dPoint(length,0)),f2dLine(front,back),inters.x,inters.y);
		if(rslt==0||rslt==-1)
			return false;
		inters=cs.TransPFromCS(inters);
	}
	else if(pPart->IsPlate()&&fabs(pPart->ucs.axis_z*depth_vec)>0.717)
	{
		if(Int3dlf(inters,front,depth_vec,
			pPart->ucs.origin+pPart->ucs.axis_z*(pPart.GeneralPlatePointer()->GetNormOffset()+pPart->GetThick()),
			pPart->ucs.axis_z)!=1)
			inters=pPart->ucs.origin;
	}
	else
		inters=pPart->ucs.origin;
	nearest_pos[0]=inters.x;
	nearest_pos[1]=inters.y;
	nearest_pos[2]=inters.z;
	return true;
}
CSolidBody* FireSolidPartBody(long hPart,long idClassType)
{
	CLDSPart *pPart = console.FromPartHandle(hPart,idClassType);
	if(pPart&&!pPart->is_visible&&pPart->GetClassTypeId()==CLS_BOLT&&console.DispPartSet.FromHandle(hPart))
	{
		HOLE_WALL* pHoleWall=console.GetHoleWall((CLDSBolt*)pPart);
		return pHoleWall!=NULL ? pHoleWall->pSolidBody : NULL;
	}
	else if(pPart&&pPart->GetClassTypeId()!=CLS_BOLT)
	{	//��˨ʵ��仯�����Ϊ����,û��Ҫ��ѡ����˨ʱ��������ʵ��
		try{
			if(pPart->pSolidBody==NULL||pPart->IsModified())
			{
				pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				if(pPart->GetClassTypeId()==CLS_LINEANGLE)
					CLDSView::RedrawAngleFrameDrawing((CLDSLineAngle*)pPart);
				else if(pPart->GetClassTypeId()==CLS_PLATE)
					CLDSView::RedrawPlateProfileDrawing((CLDSPlate*)pPart,NULL,true,0x03);
				else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
					CLDSView::RedrawFlatFrameDrawing((CLDSLineFlat*)pPart);
				else if(pPart->GetClassTypeId()==CLS_LINESLOT)
					CLDSView::RedrawSlotFrameDrawing((CLDSLineSlot*)pPart);
			}
		}
		catch(char *sError)
		{
			AfxMessageBox(sError);
		}
		return pPart->pSolidBody;
	}
	else
		return NULL;
}
#include "DefBmpParaDlg.h"
void FireExportBmpFile(long left,long bottom,long right,long top)
{
	char file[MAX_PATH];
	CDefBmpParaDlg bmpparadlg;
	CFileDialog dlg(FALSE,"bmp","LDS",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"24 bits map file(*.bmp)|*.bmp||");
#else
		"24λλͼ�ļ�(*.bmp)|*.bmp||");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	bmpparadlg.m_nBmpWidth=right-left;
	bmpparadlg.m_nBmpHeight=top-bottom;
	//win7֮��dlg.GetFileName�в�����·�����������ɵ��ļ������ﶼ˵�����������ȫ·�����ơ�wjh-2018.1.19
	_snprintf(file,MAX_PATH-1,"%s",dlg.GetPathName());
	if(bmpparadlg.DoModal()==IDOK)
		g_pSolidDraw->ExportBmpFile(file,left,bottom,right,top,bmpparadlg.m_nBmpWidth,bmpparadlg.m_nBmpHeight);
}
int CLDSView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	pFuncStatus = new CSingleLock(&FuncRun);
	/*CLSID clsid;
	LPUNKNOWN pUnk;
	HRESULT hr;
	if((hr = ::CLSIDFromProgID(L"DrawSolid",&clsid))!=NOERROR){
		TRACE(" unable to find program id");
		AfxMessageBox("δ�ҵ�Solid3d���,�����޷���������");
		return -1;
	};
	if((hr = ::CoGetClassObject(clsid, CLSCTX_INPROC_SERVER,NULL,IID_IClassFactory,(void**)&solid3dClf))!=NOERROR){
		TRACE("unable to find clsid");
		AfxMessageBox("δ�ҵ�Solid3d.dll,�����޷���������");
		return -1;
	}
	solid3dClf->LockServer(TRUE);
	solid3dClf->CreateInstance(NULL,IID_IUnknown,(void**)&pUnk);
	pUnk->QueryInterface(IID_ISolidDraw, (void**)&g_pSolidDraw);
	pUnk->QueryInterface(IID_ISolidSet, (void**)&g_pSolidSet);
	pUnk->QueryInterface(IID_ISolidSnap, (void**)&g_pSolidSnap);
	pUnk->QueryInterface(IID_ISolidOper, (void**)&g_pSolidOper);
	g_pSolidSet->Init(m_hWnd);
	solid3dClf->Release();
	pUnk->Release();*/
	m_pDrawSolid=CDrawSolidFactory::CreateDrawEngine();
	g_pSolidDraw=m_pDrawSolid->SolidDraw();
	g_pSolidSet=m_pDrawSolid->SolidSet();
	g_pSolidSnap=m_pDrawSolid->SolidSnap();
	g_pSolidOper=m_pDrawSolid->SolidOper();
	g_pSolidSet->Init(m_hWnd);
	g_pSolidSet->SetBmpScopeFunc(FireExportBmpFile);
	g_pSolidSet->SetSolidBodyCreateFunc(FireSolidPartBody);
	g_pSolidSet->SetEntPickedFunc(FireEntPickEvent);
	g_pSolidSet->SetEntSnapPosFunc(FireEntSnapPosition);
	char fontfile[MAX_PATH];
	strcpy(fontfile,theApp.execute_path);
	strcat(fontfile,"sys\\simplex.shx");
	g_pSolidSet->SetShxFontFile(fontfile);
	strcpy(fontfile,theApp.execute_path);
	strcat(fontfile,"sys\\GBHZFS.shx");
	g_pSolidSet->SetBigFontFile(fontfile);
	ConnectSolidSrcComp();
	m_hResHandle = AfxGetResourceHandle();
#ifdef __GGG_
	m_pDraw2d=CDrawEngine::CreateEngine();
	CRect rc;
	GetClientRect(&rc);
	m_pDraw2d->Init(GetSafeHwnd(),rc);
	m_pDraw2d->SetBkColor(RGB(0,0,0));	//��ɫ����
	m_pDraw2d->dwParam=(DWORD)this;
	m_pDraw2d->SetDrawFunc(Draw2DTower);
#endif
	InitSysCmdList();
	return 0;
}

int ObjectDeleteFunc(int ID, int ent_type);
void CLDSView::OnInitialUpdate() 
{
	CRect rect;
	g_bFileFirstOpen = TRUE;
	FuncRun.SetEvent();//��ʶ��������View���Ա�������ڿ���״̬
	GetClientRect(rect);
	m_nMaxX = rect.right - rect.left;//����ɾ��
	m_nMaxY = rect.bottom - rect.top;//����ɾ��
	//--------------VVVVVVV--------�������ò���---------VVVVVVV----------
    theApp.env.m_sAcadPath = 
		AfxGetApp()->GetProfileString("ACAD","ACAD_PATH",NULL);
	theApp.env.m_sUserName = 
		AfxGetApp()->GetProfileString("Settings","UserName",NULL);
    theApp.env.m_sShortUserName = 
		AfxGetApp()->GetProfileString("Settings","ShortUserName",NULL);
	//
	theApp.env.m_sWorkDir = AfxGetApp()->GetProfileString("Settings","WorkDirectory",NULL);
	theApp.env.m_sJgNcDrvFile = AfxGetApp()->GetProfileString("Settings","JgNcDrvFile",NULL);
	theApp.env.m_sPlankNcDrvFile = AfxGetApp()->GetProfileString("Settings","PlankNcDrvFile",NULL);
	theApp.env.m_iNcGroupType = AfxGetApp()->GetProfileInt("Settings","NcGroupType",NULL);
	
	g_pSolidSet->SetDelObjectFunc(ObjectDeleteFunc);
	g_pSolidSet->SetDisplayFunc(DrawSolidTower);
	g_pSolidSet->SetLiveDisplayFunc(LiveDrawFunc);
	g_pSolidSet->SetSolidAndLineStatus(g_sysPara.display.bSolidLine);
	g_pSolidSet->SetArcSamplingLength(5);
	g_pSolidSet->SetSmoothness(g_sysPara.display.nSmoothness);
	g_pSolidDraw->InitialUpdate();
	g_pSolidSet->SetDisplayLineVecMark(CLDSLinePart::bDisplayLineVecMark);
	if(g_sysPara.m_bLineDepthTest)
		g_pSolidSet->EnableLineModelDepthTest();
	else
		g_pSolidSet->DisableLineModelDepthTest();
	//g_pSolidDraw->AddCS(2,work_ucs);
	//g_pSolidSet->SetDispWorkUCS(TRUE);
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	pCmdLine->m_hMsgProcessWnd = GetSafeHwnd();
	InitClassTypeIdSelectPartsTable();
	DisplaySysSettingProperty(TRUE);
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	if(pPropDlg)
	{	//���¹������������� wht 10-11-17
		selectObjs.Empty();
		pPropDlg->UpdateSelectFilter(selectObjs);
	}
	
	I2dDrawing* pDraw=m_pDrawSolid->Drawing2d();
	IDrawingAssembly* pDrawAssembly=pDraw->InitDrawingAssembly();
	IDrawing* pDrawing=pDrawAssembly->AppendDrawing(1,"SolidCouple");
	char asciifontfile[MAX_PATH],hzfontfile[MAX_PATH];
	strcpy(asciifontfile,theApp.execute_path);
	strcat(asciifontfile,"sys\\simplex.shx");
	strcpy(hzfontfile,theApp.execute_path);
	strcat(hzfontfile,"sys\\GBHZFS.shx");
	pDrawing->Database()->AddTextStyle("standard",asciifontfile,hzfontfile);
	g_pSolidSet->SetSolidBuddyDrawing(pDrawing->GetId());
	
	/*g_pSolidSet->SetDisplayType(DISP_DRAWING);
	pDraw->SetActiveDrawing(pDrawing->GetId());
	IDbLine* pDbLine=(IDbLine*)pDrawing->AppendDbEntity(IDbEntity::DbLine);
	pDbLine->SetStartPosition(GEPOINT(0,0,0));
	pDbLine->SetEndPosition(GEPOINT(1000,1000,0));
	pDraw->RenderDrawing();*/
	CView::OnInitialUpdate();
}

BOOL CLDSView::LockFunc(BOOL bPromptMsg/*=TRUE*/)
{
	//-----vvvvvvv-��ʶ��������״̬Ϊ�棬��ͬһʱ��ֻ����һ����������������---------
	if(pFuncStatus->IsLocked())//�жϺ����Ƿ�������������������������
	{
		if(bPromptMsg)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("One command is running, please press ESC key to cancel it before executing other operations!");
#else
			AfxMessageBox("��һ������������ִ�У��밴ESC��ȡ�����������ִ����������!");
#endif
		return FALSE;
	}
	//������������״̬���ڴ˺��������ڼ䲻����������������������
	else if(!pFuncStatus->Lock(1000))//�����ɹ����ȴ�һ����
		return FALSE;
	else
		return TRUE;
}

void CLDSView::ReleaseFunc()
{
	if(pFuncStatus->IsLocked())
		pFuncStatus->Unlock();	//�򿪺�����������״̬
	FuncRun.SetEvent();		//��ʶ��������View���Ա�������ڿ���״̬
}
//ɾ����ǰѡ�е�δ��ʾʵ��Ĺ���
static void DeleteSelectedNotDisplayedPart(HASHOBJSET &selectObjs)
{
	for(CLDSObject *pDelObj=selectObjs.GetFirst();pDelObj;pDelObj=selectObjs.GetNext())
	{
		if(!pDelObj->IsDbObject())
			continue;
		CLDSDbObject *pDelDbObj=(CLDSDbObject*)pDelObj;
		Ta.RemoveRelativeObjs(pDelDbObj);
		if(pDelDbObj->IsNode())
			Ta.Node.DeleteNode(pDelDbObj->handle);
		else if(pDelDbObj->IsPart())
		{
			Ta.Parts.DeleteNode(pDelDbObj->handle);
			if(console.m_pWorkPart==pDelObj)
				console.m_pWorkPart=NULL;
		}
		
	}
}
//����������ϵ��ɾ���Ĺ����ڽ�������ʾ��ʵ��
static void CleanEntOfRelationDelObj()
{
	for(CLDSNode *pNode=Ta.Node.GetFirst(TRUE);pNode;pNode=Ta.Node.GetNext(TRUE))
	{
		if(Ta.Node.IsCursorDeleted())
			g_pSolidDraw->DelEnt(pNode->handle);
	}
	for(CLDSPart *pPart=Ta.Parts.GetFirst(0,TRUE);pPart;pPart=Ta.Parts.GetNext(0,TRUE))
	{
		if(Ta.Parts.IsCursorDeleted())
			g_pSolidDraw->DelEnt(pPart->handle);
	}
}
static BOOL PreDeletePartHookFunc(CLDSPart* pPart)
{
	if (pPart==NULL||!pPart->IsBolt())
		return true;
#ifndef __DRAWING_CONTEXT_
	collector.hashBoltRelaParts.DeleteNode(pPart->handle);
#endif
	/*����˨������������CTower::DelBoltAndRelativeBoltRef�д����ʴ˺�����ʱ���� wjh-2019.12.10
	CLdsPartListStack stack(&Ta);
	for(CLDSPart* pDatumPart=Ta.Parts.GetFirst();pDatumPart!=NULL;pDatumPart=Ta.Parts.GetNext())
	{
		if (!pDatumPart->IsPlate()&&!pDatumPart->IsAngle())
			continue;
		if (!pDatumPart->ModuleTogetherWith(pPart))
			continue;
		if (pDatumPart->FindLsByHandle(pPart->handle)==NULL)
			continue;
		_localHashDatumParts.SetValue(pDatumPart->handle,pDatumPart);
	}*/
	return true;
}
static int UpdateRelaPlatesAndAngleOddments()
{
	SmartPartPtr pDatumPart;
	int nhits=0;
	for(pDatumPart=collector.hashBoltRelaParts.GetFirst();pDatumPart.IsHasPtr();pDatumPart=collector.hashBoltRelaParts.GetNext())
	{
		if(pDatumPart->GetClassTypeId()==CLS_PLATE)
		{
			pDatumPart.pPlate->DesignPlate();
			pDatumPart.pPlate->SetModified();
			pDatumPart.pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pDatumPart.pPlate->GetSolidPartObject());
			nhits++;
		}
		else if(pDatumPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			pDatumPart.pParamPlate->DesignPlate();
			pDatumPart.pParamPlate->SetModified();
			pDatumPart.pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pDatumPart.pParamPlate->GetSolidPartObject());
			nhits++;
		}
		else if (pDatumPart->IsAngle())
		{
			if (pDatumPart.pAngle->desStartOdd.m_iOddCalStyle==1)
				pDatumPart.pAngle->CalStartOddment();
			else if (pDatumPart.pAngle->desEndOdd.m_iOddCalStyle==1)
				pDatumPart.pAngle->CalEndOddment();
			nhits++;
		}
	}
	return nhits;
}
void CLDSView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CWnd *pCmdLinePage=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	if(m_bInputCharByCmdLine&&nChar!=VK_DELETE)
	{
		if(nChar==VK_SPACE)
			pCmdLinePage->PostMessage(WM_KEYDOWN,VK_RETURN,nFlags);
		else if(nChar!=VK_SHIFT&&nChar!=VK_CONTROL&&nChar!=VK_CAPITAL && 
			nChar!=VK_LEFT&&nChar!=VK_RIGHT&&nChar!=VK_UP&&nChar!=VK_DOWN &&
			nChar!=VK_PRIOR&&nChar!=VK_NEXT&&pCmdLinePage!=NULL)
			pCmdLinePage->PostMessage(WM_KEYDOWN,nChar,nFlags);
	}
	if(nChar==VK_ESCAPE)
	{
		ReleaseFunc();
		OnOperOther();
		if(pCmdLinePage)
			((CCmdLineDlg*)pCmdLinePage)->CancelCmdLine();
	}
	if(nChar==VK_DELETE)
		Ta.BeginUndoListen();
	//��nChar==VK_DELETEʱ����ʵ��ɾ���Ĺ�������,���Դ���ɾ����ǰδ��ʾ���������
	CStackVariant<void*> stackvar((void**)&Ta.Parts.PreDeletePartHookFunc,PreDeletePartHookFunc);
	if (nChar!=VK_DELETE||m_curAppType!=TA_ASSEMBLE_ENV)
		stackvar.PopStack();
	else
		collector.hashBoltRelaParts.Empty();
	int ret=g_pSolidOper->KeyDown(nChar);
	long *id_arr=NULL;
	int hits=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(nChar==VK_DELETE&&m_curAppType==TA_ASSEMBLE_ENV)
	{
		UpdateRelaPlatesAndAngleOddments();
		//ɾ��ѡ�еĲ�������
		CTowerTreeDlg *pTreeView = (CTowerTreeDlg*)((CMainFrame*)AfxGetMainWnd())->GetTowerTreePage();
		for(int i=0;i<hits&&id_arr!=NULL;i++)
		{	
			CBlockReference* pBlockRef = Ta.BlockRef.FromHandle(id_arr[i]);
			if(pBlockRef!=NULL)
				pTreeView->DeleteBlockRefItem(pBlockRef);
		}
		if(ret==0)	//ɾ����ǰѡ�е�δ��ʾʵ��Ĺ���
		{
			CCmdLockObject cmdlock(this,FALSE);
			if(cmdlock.LockSuccessed())
				DeleteSelectedNotDisplayedPart(selectObjs);
		}
		//����������ϵ��ɾ���Ĺ����ڽ�������ʾ��ʵ��
		CleanEntOfRelationDelObj();
		Ta.EndUndoListen();
		Ta.Node.Clean();
		Ta.Parts.Clean();
		UpdatePropertyPage();	//��ֹselectObjs�д��ڶ���ɾ����������� wjh-2015.1.17
	}
	//�����༭���е�ͼԪ��Ϣͨ����������ʾ����ESC��Ҫ����������
	if(nChar==VK_DELETE||nChar==VK_ESCAPE)
	{
		CLDSObject* pObj=NULL;
		if(selectSubObjs.GetNodeNum()>0)
		{	//���ѡ�е���ͼԪ���󣬰�ESC�ͷź�������Ӧ��ʾ����һ��������
			SUB_OBJ* pSubObj=selectSubObjs.GetFirst();
			pObj=console.FromPartHandle(pSubObj->hObj);
		}
		UpdatePropertyPage(pObj);	//���ͷ���ѡ�и˼����ٸ������ԶԻ���
		UpdatePlateProfileDisplayStates();
		g_pSolidDraw->BatchClearCS(4);
		g_sysPara.m_bDispWorkUcs=FALSE;
		g_pSolidDraw->DelWorkPlaneSketch(1);
		g_pSolidDraw->DelWorkPlaneSketch(2);
		/*ɾ��ʱԲ����ɾ������ ��ʱ��֧�ְ�ESCʱȡ�������ߵĲ��� wht 10-09-17
		BOOL bRet=TRUE;
		while(bRet)	//ɾ������IDΪ0���߶� ����ɾ�������������Լ������� wht 10-09-15
			bRet=g_pSolidDraw->DelEnt(0);
		if(!g_sysPara.m_bDispWorkUcs)
		{	//ϵͳ����Ϊ����ʾ��������ϵʱ,����ʾ������ʱӦ��ʱ��ȥ��������ϵ
			g_pSolidSet->SetDispWorkUcs(FALSE);
			Invalidate(FALSE);
		}*/
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CLDSView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	//�������ԶԻ���
	//UpdatePropertyPage();
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

bool InsertStdFacePanel(BOOL bNewCmd=TRUE);
int  CLDSView::StdPanel()
{
	CUndoOperObject undo(&Ta,true);
	g_pSolidDraw->ReleaseSnapStatus();
	if(InsertStdFacePanel(TRUE))
		OnAutoCorrectJgLaystyle();
	CCmdLineDlg* pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	pCmdLine->FinishCmdLine();
	return 0;	//��ʱ��������������ģ������
}
void CLDSView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	BOOL bAltKeyDown=GetKeyState(VK_MENU)&0x8000;
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	CCmdLineDlg *pCmdLine = pMainWnd->GetCmdLinePage();
	CTowerTreeDlg *pTowerTreePage = pMainWnd->GetTowerTreePage();
	CTowerTreeCtrl *pTreeCtrl = pTowerTreePage->GetTreeCtrl();
#ifdef __ANGLE_PART_INC_
	CTowerPartGroupDlg *pPartGroupPage=pMainWnd->GetTowerPartGroupPage();
	CTowerTreeCtrl *pPartGroupTreeCtrl=pPartGroupPage->GetTreeCtrl();
#endif
	if(nFlags&MK_SHIFT)
	{
		selectObjs.Empty();
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->BatchClearCS(4);
	}
	SetCapture();	//��������ڵ�ǰ����
	if(g_pSolidSet->GetOperType()==OPER_OTHER&&nFlags==MK_LBUTTON&&!bAltKeyDown)	//���У���δ��סCtrl��Shift��
	{
		m_ptLBDownPos=point;
		m_bStartOpenWndSelTest = TRUE;	//ֻ�п���ʱ����������ѡ����
	}
	else if(g_pSolidSet->GetOperType()==OPER_OTHER&&nFlags&MK_CONTROL&&bAltKeyDown)	//���У���δ��סCtrl��Shift��
	{
		m_ptLBDownPos=point;
		if(m_curTask=TASK_OPEN_WND_SEL)
			m_bEnableOpenWndSelectTest=TRUE;
		m_bStartOpenWndSelTest = TRUE;	//ֻ�п���ʱ����������ѡ����
		return; //������ʵ��ͼ�²�ѡ��ʵ������¿���ѡ�񹹼�
	}
	g_pSolidOper->LMouseDown(point);
	if(pTreeCtrl)
		pTreeCtrl->m_bLMouseDown = TRUE;
	LButtonDown.SetEvent();
	/*		****������ק����****
	������ק������
	1.��ǰϵͳ�������ƶ�����ת����ͼ����״̬
	2.����������	//����Ctrl��
	3.��ǰ��ѡ�еĹ���
	*/
	long *id_arr,id_num;
	id_num = g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(g_pSolidSet->GetOperType()==OPER_OTHER&&id_num>0&&(nFlags&MK_CONTROL)&&
		!pCmdLine->m_bGettingStrFromCmdLine&&!pFuncStatus->IsLocked())	//&&(nFlags&MK_CONTROL))
	{	
		m_dataSource.Empty();
		FORMATETC fmtetc = {CF_TEXT,NULL,DVASPECT_CONTENT,-1,TYMED_ISTREAM};
		STGMEDIUM stgmed;
		if(CreateStreamOnHGlobal(NULL,TRUE,&stgmed.pstm)==S_OK)
		{
			stgmed.pstm->Write(&id_num,sizeof(long),NULL);
			for(int i=0;i<id_num;i++)
				stgmed.pstm->Write(&id_arr[i],sizeof(long),NULL);
			stgmed.tymed = TYMED_ISTREAM;
			stgmed.pUnkForRelease = NULL; 
			m_dataSource.CacheData(NULL,&stgmed,&fmtetc);
			DROPEFFECT dropEffect=m_dataSource.DoDragDrop(DROPEFFECT_LINK,CRect(10,10,10,10));
			if(dropEffect==DROPEFFECT_LINK)
				g_pSolidDraw->ReleaseSnapStatus();
			if(pTowerTreePage)
				pTreeCtrl->SelectDropTarget(NULL);
#ifdef __ANGLE_PART_INC_
			pPartGroupTreeCtrl->SelectDropTarget(NULL);
#endif
		}
	}
	//�˴���û�д���ã������ʱ��ѡ�й����Ļ������ظ�ˢ�������������
	//��סSHIFT��ѡ��һ������ʱ��ˢ����������������һ��Ϊ����ˢ�£��ڶ���Ϊ����ˢ��
	//�ڽ��еڶ���ˢ��ʱ������Ϊ������ʾ�����Ҳ���ĳЩ����	wht 09-06-03
	//�Ѽ���id_num==0��������Ӧ�ò��������������� wjh 2010-4-27
	/* ��ع��ܴ�������ת��CLDSView::ObjectSnappedProcess�� wjh-2017.12.3
	if(nFlags&MK_SHIFT&&id_num==0)	
	{
		UpdatePropertyPage();
		this->UpdatePlateProfileDisplayStates();
	}
	else*/ 
		if(nFlags==MK_LBUTTON&&bAltKeyDown)
	{
		if(!PRODUCT_FUNC::IsHasStdTemplate())
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Authorization certificate does not contain ��insert standard template�� function");
#else
			AfxMessageBox("��Ȩ֤���в����С������׼ģ�塱���ܵ�ʹ����Ȩ");
#endif
		else
		{
			g_pSolidDraw->ReleaseSnapStatus();
			Command("Panel");
		}
	}
	CView::OnLButtonDown(nFlags, point);
}

void CLDSView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if(g_sysPara.m_bDblClkStartRotateOper)
		g_pSolidSet->SetOperType(OPER_ROTATE);	//Ϊ��ݲ�����˫��ʱ������ת����
	BOOL bAltKeyDown=GetKeyState(VK_MENU)&0x8000;
	if((nFlags&MK_CONTROL)==0)	//ֻ��δ����Control��ʱ��˫���Żἤ��˫��������ת���Ĳ���
		g_pSolidOper->LMouseDoubleClick(point);
	CView::OnLButtonDblClk(nFlags, point);
}

void CLDSView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	g_pSolidOper->LMouseUp(point);
	m_bStartOpenWndSelTest=FALSE;
	if(GetCapture()==this)
	{
		ReleaseCapture();
		//m_bLMouseDown = FALSE;
	}
	
	CView::OnLButtonUp(nFlags, point);
}
static bool CheckMemoryOverflow(CLDSDoc* pDoc)
{
	if(g_sysPara.char0_arr[0].CompareNoCase("STL")!=0)
		return true;
	if(g_sysPara.char1_arr[0].CompareNoCase("PZXFH")!=0&&g_sysPara.char1_arr[0].CompareNoCase("PZXBH")!=0)
		return true;
	if(g_sysPara.char2_arr[0].CompareNoCase("12Q")!=0)
		return true;
	if(g_sysPara.char0_arr[1].CompareNoCase("STL")!=0)
		return true;
	if(g_sysPara.char1_arr[1].CompareNoCase("PZXFH")!=0&&g_sysPara.char1_arr[1].CompareNoCase("PZXBH")!=0)
		return true;
	if(g_sysPara.char2_arr[1].CompareNoCase("34H")!=0)
		return true;
#if defined(__TMA_FILE_)||defined(__TMA_)
	if(pDoc->m_sDocType.CompareNoCase("������������")!=0)
		return true;
#elif defined(__LMA_FILE_)||defined(__LMA_)
	if(pDoc->m_sDocType.CompareNoCase("��������������")!=0)
		return true;
#endif
	int n=Ta.listSelBoltCond.GetNodeNum();
	int i=0;
	for(BOLT_SELCOND *pSelCond=Ta.listSelBoltCond.GetFirst();pSelCond;pSelCond=Ta.listSelBoltCond.GetNext())
		i++;
	if(i!=n)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Error in bolt intelligent selection data!");
#else
		AfxMessageBox("��˨����ѡ���������ݳ���");
#endif
		return true;
	}
	return false;
}
void CLDSView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(CheckMemoryOverflow(GetDocument()))
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("when out of memory is found, please save data in a new file and record all the operations done before, and immediately notice XeroFox Company at 010-68241139");
#else
		AfxMessageBox("�������ڴ�������뼰ʱ���������һ���ļ������¼֮ǰ������еĲ�����������֪ͨ�ź���˾��010-68241139");
#endif
	f3dPoint user_pt;
	f2dPoint port_pt;
	try{
		CSize offset=point-m_ptLBDownPos;
		//m_bEnableOpenWndSelectTest&&
		if(m_bStartOpenWndSelTest&&nFlags&MK_LBUTTON&&(abs(offset.cx)>2||abs(offset.cy)>2))// && m_curTask == TASK_OTHER&&g_pSolidSet->GetOperType()==OPER_OTHER)
		{
			if(g_sysPara.m_bDragOpenWndSel)
			{	//���ڿ�����ק��������ѡ���ʱ��Ч,����ܶ��Ͽͻ���ӳ̫��������Ч�ʷ������� wjh-2015.8.24
				m_curTask=TASK_OPEN_WND_SEL;
				OpenWndSel();
				g_pSolidOper->LMouseDown(m_ptLBDownPos);
			}
			m_bStartOpenWndSelTest=FALSE;
		}
		//point.x=point.y=0;
		port_pt.Set(point.x,point.y);
		g_pSolidOper->MouseMove(point,nFlags);
		CString sMousePos;
		if(!g_pSolidOper->IsHasHighlightPoint(&user_pt))
			g_pSolidOper->ScreenToUser(&user_pt,port_pt);
		UCS_STRU object_ucs;
		g_pSolidSet->GetObjectUcs(object_ucs);
		coord_trans(user_pt,object_ucs,FALSE);
		sMousePos.Format("X=%.2f,Y=%.2f,Z=%.2f",user_pt.x,user_pt.y,user_pt.z);
		if(theApp.m_pMainWnd)
		{	//˫���ļ��򿪳���ʱ,�����theApp.m_pMainWnd��pCmdLineDlgΪNULL����� wjh-2015.4.3
			CMainFrame* pMainWnd = (CMainFrame*)theApp.m_pMainWnd;
			pMainWnd->SetPaneText(1,sMousePos);
			CCmdLineDlg *pCmdLineDlg=pMainWnd->GetCmdLinePage();
			if(pCmdLineDlg&&pCmdLineDlg->IsHasFocus())
				SetFocus();	//��ü�������Ľ���
		}
	}
	catch(char * sError)
	{
		AfxMessageBox(sError);
	}
	
	CView::OnMouseMove(nFlags, point);
}
static OPER_TYPE _LocalCurrentOperTypeBeforeRBtnDown=OPER_OTHER;
void CLDSView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	if(pCmdLine->m_bGettingStrFromCmdLine&&g_pSolidSet->GetOperType()==OPER_OTHER)
		RButtonDown.SetEvent();
	_LocalCurrentOperTypeBeforeRBtnDown=g_pSolidSet->GetOperType();
	g_pSolidOper->RMouseDown(point);
	CView::OnRButtonDown(nFlags, point);
}

void CLDSView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	g_pSolidOper->RMouseUp(point);
	if(!pFuncStatus->IsLocked())
	{	//ֻ�����޵ȴ�ʰȡ����(��������)������ִ��ʱ�ſ��Ҽ���������״̬,�������γɼٽ�������״̬(ʵ�ʻ���ִ��) wjh-2016.10.22
		CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
		if(pCmdLine&&pCmdLine->m_sCmdPrompt.CompareNoCase("command:")!=0)
		{
			pCmdLine->FinishCmdLine();	
			pCmdLine->FillCmdLine("command:","");
#else
		if(pCmdLine&&pCmdLine->m_sCmdPrompt.CompareNoCase("����:")!=0)
		{
			pCmdLine->FinishCmdLine();	
			pCmdLine->FillCmdLine("����:","");
#endif
		}
	}
	CView::OnRButtonUp(nFlags, point);
	_LocalCurrentOperTypeBeforeRBtnDown=g_pSolidSet->GetOperType();
}

void CLDSView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	LOGFONT lf;
	if(afxGlobalData.fontRegular.m_hObject!=NULL)
	{
		afxGlobalData.fontRegular.GetLogFont(&lf);   
		if(lf.lfHeight!=-12)
		{
			lf.lfHeight =-12;   
			lstrcpy(lf.lfFaceName, _T("����"));     // using without style office 2007   
			afxGlobalData.fontRegular.DeleteObject();   
			afxGlobalData.fontRegular.CreateFontIndirect(&lf);  
		}
	}
	
	m_nMaxX = cx;
	m_nMaxY = cy;
	g_pSolidOper->ReSize();
#ifdef __GGG_
	m_pDraw2d->UpdateViewport();
#endif
	/*	�ļ��մ�ʱ����OnUpdate������ִ�й�һ��ZoomAll,��˲�����ִ�����´��� wjh-2013.2.22
	if(g_bFileFirstOpen)
	{
		g_pSolidOper->ZoomAll(0.95);
		g_bFileFirstOpen = FALSE;
	}*/
	
}

void CLDSView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	UndoViewBuffStack.Empty();
	//����UNDO��ջ��С
	m_nObjectSnapedCounts = 0;
	m_setSnapObjects.Empty();
	m_pDoc = GetDocument();
	SCOPE_STRU scope;
	Ta.GetTaSizeScope(scope);
	g_sysPara.fRotateVelocity = scope.depth()/1000;
	g_sysPara.fRotateVelocity = __max(g_sysPara.fRotateVelocity,10);	//���û������ʱ���߲���Ϊ����Ϊ10��
	g_pSolidSet->SetRotRadius(g_sysPara.fRotateVelocity*30);
	if(theApp.m_bAnalysisWorkEnv)
		g_sysPara.iColorSchema=1;
	else
		g_sysPara.iColorSchema=3;
	if(g_sysPara.iColorSchema==1)		//TSA��ɫ����
		g_pSolidSet->SetBkColor(g_sysPara.crTsaMode.crBackground);
	else if(g_sysPara.iColorSchema==2)	//������ɫ����
		g_pSolidSet->SetBkColor(g_sysPara.crMaterialMode.crBackground);
	else								//������ɫ����
		g_pSolidSet->SetBkColor(g_sysPara.crPartMode.crBackground);
	if(theApp.m_pMainWnd!=NULL)
	{
		((CMainFrame*)theApp.m_pMainWnd)->UpdateAllPages();
		//ˢ�¹�����ʾ��
		g_pSolidDraw->BatchClearCS(ISolidDraw::MODEL_CS);
		console.Display(NULL);
		OnOperOther();
		DisplaySysSettingProperty(TRUE);
	}
}

BOOL CLDSView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CLDSView::AutoDaoWei()
{
	long i,ll,hCurrentObj=0,*id_arr=NULL;
	CProcBarDlg *pProcDlg = NULL;
	CUndoOperObject undo(&Ta,true,CUndoOperObject::UNDO_REPOSITION,false);
	if(Ta.CurrentUndoType()!=CUndoOperObject::UNDO_REPOSITION)
		undo.StartUndoListen();	//�����ظ�ѹ�뵽λ�����Undo��¼�������û�����F7������Undo����ʱ�о������� wjh-2015.7.22
	if(g_pSolidSnap->GetLastSelectEnts(id_arr))
	{
		OnDaoweiSelectPart();
		return;
	}
	try
	{
		CLogErrorLife loglife;
		DisplayProcess(0);
		i=0;
		ll = Ta.Node.GetNodeNum();
		for(CLDSNode* pNode=Ta.Node.GetFirst();pNode!=NULL;pNode=Ta.Node.GetNext())
		{
			i++;
			hCurrentObj=pNode->handle;
			pNode->Unlock();
			Ta.Node.push_stack();
			pNode->Set();
			Ta.Node.pop_stack();
			DisplayProcess(i*15/ll);
		}
		i=0;
		ll = Ta.Parts.GetNodeNum();
		if(theApp.m_bAnalysisWorkEnv)	//��ѧ����ʱ���������Բ����㹹����λ��,����������ľ�ʾ��־���ɺ��� wjh-2017.3.3
			logerr.GlobalEnabled(false);
		CLDSPart *pPart=NULL;
		bool hasBoltOrPlates=false;
		for(pPart=Ta.Parts.GetFirst();pPart&&!hasBoltOrPlates;pPart=Ta.Parts.GetNext())
		{
			if(pPart->GetClassTypeId()==CLS_BOLT||pPart->IsPlate())
				hasBoltOrPlates=true;
		}
		CLDSPart::INTERNAL_TEST_VERSION=(theApp.m_bAnalysisWorkEnv||!hasBoltOrPlates)?false:(PRODUCT_FUNC::IsHasInternalTest()==TRUE);
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			i++;
			hCurrentObj=pPart->handle;
			CLdsPartListStack stackParts(&Ta.Parts);
#ifdef __ANGLE_PART_INC_
			if(pPart->IsLinePart())
			{
				((CLDSLinePart*)pPart)->ClearFlag();
				((CLDSLinePart*)pPart)->CalPosition();
				if(pPart->GetClassTypeId()==CLS_LINEANGLE)
				{
					CLDSLineAngle* pLineAngle=(CLDSLineAngle*)pPart;
					pLineAngle->cal_x_wing_norm();
					pLineAngle->cal_y_wing_norm();
				}
				else if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
				{
					CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)pPart;
					pGroupAngle->cal_x_wing_norm();
					pGroupAngle->cal_y_wing_norm();
					pGroupAngle->UpdateSonJgBasicInfo();
					pGroupAngle->UpdateSonJgPos();
					pGroupAngle->UpdateSonJgWingNorm();
				}
				else if(pPart->GetClassTypeId()==CLS_LINETUBE)
				{
					CLDSLineTube *pTube=(CLDSLineTube*)pPart;
					pTube->ClearFlag();
					pTube->CalWorkPlaneNorm();
				}
				else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
				{
					CLDSLineFlat *pFlat=(CLDSLineFlat*)pPart;
					pFlat->ClearFlag();
					pFlat->CalWorkPlaneNorm();
				}
				else if(pPart->GetClassTypeId()==CLS_LINESLOT)
				{
					CLDSLineSlot *pSlot=(CLDSLineSlot*)pPart;
					pSlot->ClearFlag();
					pSlot->CalWorkPlaneNorm();
				}
				else if(pPart->IsArcPart())
				{
					CLDSArcPart* pArcPart=(CLDSArcPart*)pPart;
					pArcPart->DesignArcBaseLine();
				}
			}
			/*else if(pPart->GetClassTypeId()==CLS_BOLT)
			{
				continue;	//Ϊ�˼ӿ쵽λ�ٶȣ��ڴ˲�������˨λ�ü���������Ҫ��ͨ�����������ʵ��
				((CLDSBolt*)pPart)->correct_worknorm();
				((CLDSBolt*)pPart)->correct_pos();
			}
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				continue;	//Ϊ�˼ӿ쵽λ�ٶȣ��ڴ˲������������λ�ü���������Ҫ��ͨ�����������ʵ��
				CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
				if(pParamPlate->m_iParamType==TYPE_FL||pParamPlate->m_iParamType==TYPE_FLD
					||pParamPlate->m_iParamType==TYPE_FLP||pParamPlate->m_iParamType==TYPE_ROLLEND
					||pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_XEND)
				{	
					CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(pParamPlate->m_hPartWeldParent,CLS_LINETUBE);
					if(pLineTube)
					{	//������Ʋ�������
						if(pLineTube->m_tJointEnd.hLinkObj==pParamPlate->handle)
							pLineTube->DesignEndJointUCS(pParamPlate);
						else if(pLineTube->m_tJointStart.hLinkObj==pParamPlate->handle)
							pLineTube->DesignStartJointUCS(pParamPlate);
					}
					ATTACHPART_ITEM *pItem=NULL;
					if(pParamPlate->m_iParamType==TYPE_ROLLEND||pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_XEND)
					{	//��λ���Ͳ���U�Ͳ���ʮ�ֲ�帽�Ӹְ�
						for(pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
						{
							CLDSPart *pAttachPart=console.FromPartHandle(pItem->h,pItem->idClsType);
							if(pAttachPart==NULL)
								continue;
							if(pAttachPart->GetClassTypeId()!=CLS_PLATE)
								continue;
							((CLDSPlate*)pAttachPart)->DesignSetupUcs();
						}
					}
				}
			}*/
#endif
			DisplayProcess(15+i*90/ll);
		}
		CLDSPart::INTERNAL_TEST_VERSION=(PRODUCT_FUNC::IsHasInternalTest()==TRUE);
		RedrawAll();
	}
	//�����쳣���������⣬û�д����м��ѹջ��δ��վ���⣬�����ⲻ���û��������쳣��λ2048�����ϡ�2003/10/18 WJH
	catch(char *sError)
	{
		DisplayProcess(100);
		char sText[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(sText,"%s,current part��0X%X",sError,hCurrentObj);
#else
		sprintf(sText,"%s,��ǰ������0X%X",sError,hCurrentObj);
#endif
		AfxMessageBox(sText);
	}
	if(theApp.m_bAnalysisWorkEnv)
		logerr.GlobalEnabled(true);
	DisplayProcess(100);
}
void CLDSView::OnTaDaoWei() 
{
	m_nPrevCommandID=ID_TA_DAO_WEI;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat reposition";
#else
	m_sPrevCommandName="�ظ�ȫ����λ";
#endif
	AutoDaoWei();
}
	
void CLDSView::OnEditFindPart() 
{
	FindPart();
	//m_nPrevCommandID=ID_EDIT_FIND_PART;
	//m_sPrevCommandName="�ظ����ҹ���";
	//Command("Find");
}

void CLDSView::OpenWndSel() 
{
	//ֻ����Ƥ�����ͼԪ
	g_pSolidSet->SetCreateEnt(FALSE);
	g_pSolidSet->SetDrawType(DRAW_RECTANGLE);
	m_curTask = TASK_OPEN_WND_SEL;
	//ִ�п�������󣬱����ȵ��һ����������������������������ľ�������ճ����һ�� wjh-2013.11.08
	m_bEnableOpenWndSelectTest=FALSE;
}

void CLDSView::OnOpenWndSel() 
{
	m_nPrevCommandID=ID_OPEN_WND_SEL;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat windowing";
#else
	m_sPrevCommandName="�ظ�����ѡ��";
#endif
	OpenWndSel();
}

void CLDSView::OnInverseSelectObjs() 
{
	g_pSolidSnap->InverseSelectEntsStatus();
	g_pSolidDraw->Draw();
}
void CLDSView::OnHidePart() 
{
	m_nPrevCommandID=ID_HIDE_PART;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat hide part";
#else
	m_sPrevCommandName="�ظ���ȥ����";
#endif
	long *id_arr=NULL;
	int i,n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(g_sysPara.display.bPartSketch) 
	{	//ɾ��IDΪ0��ʵ�� wht 10-09-15
		BOOL bRet=TRUE;
		while(bRet)
			bRet=g_pSolidDraw->DelEnt(0);
	}
	//g_pSolidDraw->BuildDisplayList();
	IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
	for(i=0;i<n;i++)
	{
		CLDSDbObject* pObj = (CLDSDbObject*)console.FromHandle(id_arr[i],TRUE);
		if(pObj)
			pObj->is_visible = FALSE;
		BOOL bRet=TRUE;
		while(bRet) //ɾ������IDΪid_arr[i]��ʵ��
			bRet=g_pSolidDraw->DelEnt(id_arr[i]);
		HOLE_WALL* pHoleWall=NULL;
		if(pObj&&pObj->GetClassTypeId()==CLS_BOLT&&(pHoleWall=console.GetHoleWall((CLDSBolt*)pObj))!=NULL)
		{
			pHoleWall->Create3dSolidModel();//g_pSolidOper->GetScaleUserToScreen(),g_pSolidOper->GetScaleUserToScreen(),g_sysPara.display.nSmoothness);
			pHoleWall->is_visible=true;
			g_pSolidDraw->NewSolidPart(pHoleWall->GetSolidPartObject());
		}
		if(pBuddyDrawing)
			pBuddyDrawing->DeleteDbEntityByHiberMasterId(id_arr[i]);
	}
	g_pSolidDraw->Draw();
}

void CLDSView::OnDisplayPartWorkUcs()
{
	m_nPrevCommandID=ID_DISPLAY_PART_WORK_UCS;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat display part assembling coordinate system";
#else
	m_sPrevCommandName="�ظ���ʾ����װ������ϵ";
#endif
	Command("Ucs");
}

void CLDSView::OnUpdateHidePart(CCmdUI* pCmdUI) 
{
	long *id_arr=NULL;
	if(g_pSolidSnap->GetLastSelectEnts(id_arr)<=0)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
	
}

void CLDSView::OnEditSelectAll() 
{
	g_pSolidDraw->SetAllSnapStatus();
	UpdatePropertyPage();
}

void CLDSView::OnClearDesignResult() 
{
	Command("DelSpecClsPart");
	m_pDoc->UpdateAllViews(NULL);
}

void CLDSView::OnHandleReplace()
{
	static CHandleReplaceDlg replacedlg;
	if(replacedlg.DoModal()!=IDOK)
		return;
	long hOld=HexStrToLong(replacedlg.m_hOldObject);
	long hNew=HexStrToLong(replacedlg.m_hNewObject);
	CLDSObject* pOldObj=Ta.FromHandle(hOld);
	CLDSObject* pNewObj=Ta.FromHandle(hNew);
	if(pOldObj!=NULL&&pNewObj!=NULL)
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("New and old handle after replacing both refer to valid data object(Replacement will lead to same handle appear),failed to replace");
#else 
		AfxMessageBox("�滻ǰ����¾ɾ��ֵ��ָ���˺Ϸ������ݶ���(�滻�ᵼ�����������)���滻ʧ��");
#endif
	else if(hNew>0x20)
	{
		CHandleReplacer replacer(hOld,hNew,NULL,false);
		replacer.SetReplaceObjSelfState();
		if(Ta.ReplaceReferenceHandle(replacer))
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox("Handle replaced success");
	}
	else
		AfxMessageBox("Inputed handle is invalid");
#else 
			MessageBox("����滻�ɹ�");
	}
	else
		AfxMessageBox("������ֵΪ���Ϸ����");
#endif
}
void CLDSView::OnReuseParts()
{
	CReusePartsDlg dlg;
	long *id_arr,n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	for(int j=0;j<n;j++)
	{
		CLDSPart *pPart=console.FromPartHandle(id_arr[j]);
		if(pPart!=NULL)
			dlg.m_reusePartList.append(REUSE_PART(pPart));
	}
	if(dlg.DoModal()==IDOK)
	{
		for(REUSE_PART* pReusePart=dlg.m_reusePartList.GetFirst();pReusePart;pReusePart=dlg.m_reusePartList.GetNext())
		{
			CBuffer buff;
			CLDSPart* pPart=NULL;
			if(pReusePart->m_cReuseType==REUSE_PART::REUSE_TYPE_COPY && pReusePart->m_pPart)
			{
				pPart=Ta.Parts.append(pReusePart->m_pPart->GetClassTypeId());
				pReusePart->m_pPart->ToBuffer(buff);
				buff.SeekToBegin();
				pPart->FromBuffer(buff);
			}
			else if(pReusePart->m_cReuseType==REUSE_PART::REUSE_TYPE_SHARE)
			{
				pReusePart->m_pPart->cfgword.AddSpecWord(pReusePart->m_destCfgword);
				continue;
			}
			else if(pReusePart->m_cReuseType==REUSE_PART::REUSE_TYPE_SHADOW)
			{
				pPart=Ta.Parts.append(pReusePart->m_pPart->GetClassTypeId());
				pPart->SetShadowPara(pReusePart->m_pPart,0,1);
				pPart->ShadowDesign();
			}
			pPart->cfgword=pReusePart->m_destCfgword;
			if(pReusePart->m_pPart->pSolidBody)
				pPart->pSolidBody=new CSolidBody(pReusePart->m_pPart->pSolidBody->BufferPtr(),pReusePart->m_pPart->pSolidBody->BufferLength());
			else
				pReusePart->m_pPart->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
		}
	}
	g_pSolidDraw->Draw();
}
//�������䣨������_��϶������Ϊһ�������Ǹ�
void CLDSView::OnMakeJgBend() 
{
#ifdef __ANGLE_PART_INC_
	m_nPrevCommandID=ID_MAKE_JG_BEND;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat bend angle";
#else
	m_sPrevCommandName="�ظ��Ǹ�����";
#endif
	CString cmdStr="";
	CLDSLineAngle *pSelJg,*pSeg;
	CLDSPolyJoint *pPolyJoint=NULL;
	double wing_wide,wing_thick;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	JGSET seg_arr;	//��ʱ�洢�����Ǹֵ��Ӷ�
	//-----vvvvvvv-��ʶ��������״̬Ϊ�棬��ͬһʱ��ֻ����һ����������������---------
	if(pFuncStatus->IsLocked())//�жϺ����Ƿ�������������������������
		return;
	else//������������״̬���ڴ˺��������ڼ䲻����������������������
		if(!pFuncStatus->Lock(1000))//���� �ɹ����ȴ�һ����
			return;
	//-----^^^^^^^-��ʶ��������״̬Ϊ�棬��ͬһʱ��ֻ����һ����������������---------
	//g_pPromptMsg->SetMsg("�밴��ѡ��Ҫ��ϻ����Ǹֵ��ӽǸ�!");
	g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select in order child angles of the bending angle<click the left mouse button to end selection>:","");
#else
	pCmdLine->FillCmdLine("�밴��ѡ��Ҫ��ϻ����Ǹֵ��ӽǸ�<�յ�����������ѡ��>:","");
#endif
	///////////////��׽����/////////////////////////
	int i=0;
	CUndoOperObject undo(&Ta,true);
	try{
		while(1)
		{	
			long *id_arr=NULL;
			//������������Ӧ������̧�����Ϣ��Ч��ͬ���س���ͬ
			if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::LBUTTONUP_AS_RETURN))
				return;
			if(g_pSolidSnap->GetLastSelectEnts(id_arr)>i)
			{
				CLDSPart *pPart = console.FromPartHandle(id_arr[i]);
				if(pPart==NULL)
					break;
				if(pPart->GetClassTypeId()!=CLS_LINEANGLE)
				{
					g_pSolidDraw->SetEntSnapStatus(pPart->handle,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("chose invalid part(maybe chose a bended angle)");
#else
					AfxMessageBox("ѡ���˴���Ĺ�������(������ѡ�����ѻ����ĽǸ�)");
#endif
					continue;
				}
				pSelJg = (CLDSLineAngle*)pPart;
				if(seg_arr.GetNodeNum()==0)
				{
					seg_arr.append(pSelJg);
					wing_wide = pSelJg->GetWidth();
					wing_thick= pSelJg->GetThick();
				}
				else if(seg_arr.GetNodeNum()>0)
				{
					pSeg=seg_arr.GetTail();
					if(wing_wide!=pSelJg->GetWidth()
						||wing_thick!=pSelJg->GetThick()
						||pSeg->cMaterial!=pSelJg->cMaterial)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("The two angles have different size, the selection is invalid!");
#else
						AfxMessageBox("ǰ��ѡ������������ӽǸֹ��ͬ,��ѡ����Ч!");
#endif
						g_pSolidDraw->SetEntSnapStatus(pSelJg->handle,FALSE);
						continue;
					}
					else
					{
						CLDSPolyJoint *pPolyJoint;
						for(pPolyJoint=(CLDSPolyJoint*)Ta.Parts.GetFirst(CLS_POLYJOINT);pPolyJoint;
							pPolyJoint=(CLDSPolyJoint*)Ta.Parts.GetNext(CLS_POLYJOINT))
						{
							if((pPolyJoint->pPrevLinePart==pSeg&&pPolyJoint->pNextLinePart==pSelJg)
								||(pPolyJoint->pPrevLinePart==pSelJg&&pPolyJoint->pNextLinePart==pSeg))
								break;
						}
						if(pPolyJoint)
						{
#ifdef AFX_TARG_ENU_ENGLISH
							AfxMessageBox("The selected angle has been bended, select again!");
#else
							AfxMessageBox("ѡ�����Ѿ������ĽǸ֣�������ѡ��!");
#endif
							g_pSolidDraw->SetEntSnapStatus(pSelJg->handle,FALSE);
							continue;
						}
						seg_arr.append(pSelJg);
					}
				}
				i++;
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(AfxMessageBox("Are you sure that you have selected all bending angles?",MB_YESNO|MB_ICONQUESTION)==IDYES)
#else
				if(AfxMessageBox("��ȷ���Ѿ�ѡ��Ҫ��������нǸ�����?",MB_YESNO|MB_ICONQUESTION)==IDYES)
#endif
					break;
				else
					continue;
			}
		}
		if(seg_arr.GetNodeNum()<2)
		{
			pCmdLine->CancelCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
			throw "The number of angles selected is too few, bending cannot be done";
#else
			throw "ѡ��Ǹ���̫��,�޷�������ϻ���";
#endif
		}
		f3dPoint offset_vec;
		static CHuoQuOffsetDlg huoqu_dlg;
		CLDSLineAngle *pPrevSeg=NULL;
		long huoqu_handle=Ta.GetNewHandle();
		CAngleMoveOperObject oddmodified;
		//pHuoQuJg=(CLDSPolyAngle*)console.AppendPart(CLS_POLYANGLE);
		for(pSeg=seg_arr.GetFirst();pSeg;pSeg=seg_arr.GetNext())
		{
			g_pSolidDraw->ReleaseSnapStatus();
			if(pPrevSeg!=NULL)
			{
				if(pPrevSeg->pEnd)
					g_pSolidDraw->SetEntSnapStatus(pPrevSeg->pEnd->handle);
				g_pSolidDraw->SetEntSnapStatus(pPrevSeg->handle);
				if(huoqu_dlg.DoModal()!=IDOK)
				{
					//pHuoQuJg->ExplodeHuoQu();
					//console.DeletePart(pHuoQuJg->handle);
#ifdef AFX_TARG_ENU_ENGLISH
					throw "angle bending is cancelled";
#else
					throw "�Ǹ����乤����ȡ��";
#endif
				}
				pPrevSeg->huoqu_r = huoqu_dlg.m_fHuoQuR;
				pPrevSeg->huoqu_base_wing_x0_y1_both2 = huoqu_dlg.m_iHuoQuBaseWing;
				if(huoqu_dlg.m_nByStart==0)
				{
					if(pPrevSeg->pStart==pSeg->pStart)
					{	//ʼ�˽Ǹֵ�ʼ������һ�����Ӷ�����
						offset_vec=pPrevSeg->Start()-pPrevSeg->End();
						normalize(offset_vec);
						pPrevSeg->pre_huoqu_start_xyz=pPrevSeg->Start();
						pPrevSeg->SetStart(pPrevSeg->Start()+offset_vec*huoqu_dlg.m_fHuoQuOffset);
						pSeg->pre_huoqu_start_xyz=pSeg->Start();
						pSeg->SetStart(pPrevSeg->Start());
						pPrevSeg->SetStartOdd(huoqu_dlg.m_fHuoQuOffset);//bCorrectLsPosPara=TRUE)������˨λ����Ʋ���
						pPrevSeg->SetStartOdd(0);	//�������ͷ
						pSeg->SetStartOdd(0);		//�������ͷ
						pPrevSeg->LockStartPos();
						pSeg->LockStartPos();
						pPrevSeg->bHuoQuStart = TRUE;
						pSeg->bHuoQuStart = TRUE;
					}
					else if(pPrevSeg->pStart==pSeg->pEnd)
					{
						offset_vec=pPrevSeg->Start()-pPrevSeg->End();
						normalize(offset_vec);
						pPrevSeg->pre_huoqu_start_xyz=pPrevSeg->Start();
						pPrevSeg->SetStart(pPrevSeg->Start()+offset_vec*huoqu_dlg.m_fHuoQuOffset);
						pSeg->pre_huoqu_end_xyz=pSeg->End();
						pSeg->SetEnd(pPrevSeg->Start());
						pPrevSeg->SetStartOdd(huoqu_dlg.m_fHuoQuOffset);//bCorrectLsPosPara=TRUE)	//������˨λ����Ʋ���
						pPrevSeg->SetStartOdd(0);	//�������ͷ
						pSeg->SetEndOdd(0);	//�������ͷ
						pPrevSeg->LockStartPos();
						pSeg->LockEndPos();
						pPrevSeg->bHuoQuStart = TRUE;
						pSeg->bHuoQuEnd = TRUE;
					}
					else if(pPrevSeg->pEnd==pSeg->pStart)
					{
						offset_vec=pPrevSeg->End()-pPrevSeg->Start();
						normalize(offset_vec);
						pPrevSeg->pre_huoqu_end_xyz=pPrevSeg->End();
						pPrevSeg->SetEnd(pPrevSeg->End()+offset_vec*huoqu_dlg.m_fHuoQuOffset);
						pSeg->pre_huoqu_start_xyz=pSeg->Start();
						pSeg->SetStart(pPrevSeg->End());
						pPrevSeg->SetEndOdd(huoqu_dlg.m_fHuoQuOffset);//bCorrectLsPosPara=TRUE)	//������˨λ����Ʋ���
						pPrevSeg->SetEndOdd(0);	//�������ͷ
						pSeg->SetStartOdd(0);		//�������ͷ
						pPrevSeg->LockEndPos();
						pSeg->LockStartPos();
						pPrevSeg->bHuoQuEnd = TRUE;
						pSeg->bHuoQuStart = TRUE;
					}
					else //if(pPrevSeg->pEnd==pSeg->pEnd)
					{	//ʼ�˽Ǹֵ��ն�����һ�����Ӷ�����
						offset_vec=pPrevSeg->End()-pPrevSeg->Start();
						normalize(offset_vec);
						pPrevSeg->pre_huoqu_end_xyz=pPrevSeg->End();
						pPrevSeg->SetEnd(pPrevSeg->End()+offset_vec*huoqu_dlg.m_fHuoQuOffset);
						pSeg->pre_huoqu_end_xyz=pSeg->End();
						pSeg->SetEnd(pPrevSeg->End());
						pPrevSeg->SetEndOdd(huoqu_dlg.m_fHuoQuOffset);//bCorrectLsPosPara=TRUE)	//������˨λ����Ʋ���
						pPrevSeg->SetEndOdd(0);	//�������ͷ
						pSeg->SetEndOdd(0);		//�������ͷ
						pPrevSeg->LockEndPos();
						pSeg->LockEndPos();
						pPrevSeg->bHuoQuEnd = TRUE;
						pSeg->bHuoQuEnd = TRUE;
					}
				}
				else
				{
					if(pPrevSeg->pStart==pSeg->pStart&&pPrevSeg->pStart!=NULL)
					{	//ʼ�˽Ǹֵ�ʼ������һ�����Ӷ�����
						offset_vec=pSeg->Start()-pSeg->End();
						normalize(offset_vec);
						pSeg->pre_huoqu_start_xyz=pSeg->Start();
						pSeg->SetStart(pSeg->Start()+offset_vec*huoqu_dlg.m_fHuoQuOffset);
						pPrevSeg->pre_huoqu_start_xyz=pPrevSeg->Start();
						pPrevSeg->SetStart(pSeg->Start());
						pPrevSeg->SetStartOdd(0);	//�������ͷ
						pSeg->SetStartOdd(huoqu_dlg.m_fHuoQuOffset);//bCorrectLsPosPara=TRUE)	//������˨λ����Ʋ���
						pSeg->SetStartOdd(0);		//�������ͷ
						pPrevSeg->LockStartPos();
						pSeg->LockStartPos();
						pPrevSeg->bHuoQuStart = TRUE;
						pSeg->bHuoQuStart = TRUE;
					}
					else if(pPrevSeg->pEnd==pSeg->pStart&&pPrevSeg->pEnd!=NULL)
					{
						offset_vec=pSeg->Start()-pSeg->End();
						normalize(offset_vec);
						pSeg->pre_huoqu_start_xyz=pSeg->Start();
						pSeg->SetStart(pSeg->Start()+offset_vec*huoqu_dlg.m_fHuoQuOffset);
						pPrevSeg->pre_huoqu_end_xyz=pPrevSeg->End();
						pPrevSeg->SetEnd(pSeg->Start());
						pPrevSeg->SetEndOdd(0);	//�������ͷ
						pSeg->SetStartOdd(huoqu_dlg.m_fHuoQuOffset);//bCorrectLsPosPara=TRUE)	//������˨λ����Ʋ���
						pSeg->SetStartOdd(0);		//�������ͷ
						pPrevSeg->LockEndPos();
						pSeg->LockStartPos();
						pPrevSeg->bHuoQuEnd = TRUE;
						pSeg->bHuoQuStart = TRUE;
					}
					else if(pPrevSeg->pStart==pSeg->pEnd&&pPrevSeg->pStart!=NULL)
					{
						offset_vec=pSeg->End()-pSeg->Start();
						normalize(offset_vec);
						pSeg->pre_huoqu_end_xyz=pSeg->End();
						pSeg->SetEnd(pSeg->End()+offset_vec*huoqu_dlg.m_fHuoQuOffset);
						pPrevSeg->pre_huoqu_start_xyz=pPrevSeg->Start();
						pPrevSeg->SetStart(pSeg->End());
						pPrevSeg->SetStartOdd(0);	//�������ͷ
						pSeg->SetEndOdd(huoqu_dlg.m_fHuoQuOffset);//bCorrectLsPosPara=TRUE)	//������˨λ����Ʋ���
						pSeg->SetEndOdd(0);		//�������ͷ
						pPrevSeg->LockStartPos();
						pSeg->LockEndPos();
						pPrevSeg->bHuoQuStart = TRUE;
						pSeg->bHuoQuEnd = TRUE;
					}
					else //if(pPrevSeg->pEnd==pSeg->pEnd)
					{	//ʼ�˽Ǹֵ��ն�����һ�����Ӷ�����
						offset_vec=pSeg->End()-pSeg->Start();
						normalize(offset_vec);
						pSeg->pre_huoqu_end_xyz=pSeg->End();
						pSeg->SetEnd(pSeg->End()+offset_vec*huoqu_dlg.m_fHuoQuOffset);
						pPrevSeg->pre_huoqu_end_xyz=pPrevSeg->End();
						pPrevSeg->SetEnd(pSeg->End());
						pPrevSeg->SetEndOdd(0);	//�������ͷ
						pSeg->SetEndOdd(huoqu_dlg.m_fHuoQuOffset);//bCorrectLsPosPara=TRUE)	//������˨λ����Ʋ���
						pSeg->SetEndOdd(0);		//�������ͷ
						pPrevSeg->LockEndPos();
						pSeg->LockEndPos();
						pPrevSeg->bHuoQuEnd = TRUE;
						pSeg->bHuoQuEnd = TRUE;
					}
				}
			}
			pSeg->huoqu_handle = huoqu_handle;
			if(pPrevSeg)
			{
				pPolyJoint=(CLDSPolyJoint*)console.AppendPart(CLS_POLYJOINT);
				pPolyJoint->cMaterial=pPrevSeg->cMaterial;
				pPolyJoint->iSeg=pPrevSeg->iSeg;
				pPolyJoint->SetPartNo(pPrevSeg->GetPartNo());
				pPolyJoint->pPrevLinePart=pPrevSeg;
				pPolyJoint->pNextLinePart=pSeg;
				pPolyJoint->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pPolyJoint->GetSolidPartObject());
				pPrevSeg->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pPrevSeg->GetSolidPartObject());
			}
			pPrevSeg=pSeg;
			pSeg->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pSeg->GetSolidPartObject());
		}
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->Draw();
	}
	catch (char* sError)
	{
		AfxMessageBox(sError);
		ReleaseFunc();
		return;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	MessageBox("angle bending is done successfuly");
#else
	MessageBox("�Ǹ�����ɹ�");
#endif
	ReleaseFunc();
	g_pPromptMsg->Destroy();
	m_pDoc->SetModifiedFlag();
#endif
}

void CLDSView::OnMakeEndBend() 
{
#ifdef __ANGLE_PART_INC_
	m_nPrevCommandID=ID_MAKE_END_BEND;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat end bending";
#else
	m_sPrevCommandName="�ظ���ͷ����";
#endif
	CLDSNode *pSelNode=NULL;
	CLDSLineAngle *pBendJg=NULL,*pSpanJg=NULL,*pSegJg=NULL;
	CLDSPolyJoint *pPolyJoint=NULL;
	int wing_x0_y1=0;
	JGSET seg_arr;	//��ʱ�洢�����Ǹֵ��Ӷ�
	//-----vvvvvvv-��ʶ��������״̬Ϊ�棬��ͬһʱ��ֻ����һ����������������---------
	CString cmdStr,errcmdStr;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CCmdLockObject cmdLock(this);
	if(!cmdLock.LockSuccessed())
		return;
	DWORD dwhObj=0,dwExportFlag=0;
	g_pSolidDraw->ReleaseSnapStatus();
	CUndoOperObject undo(&Ta,true);
		try{
	#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("please choose bending nodes first:");
	#else
			cmdStr.Format("������ѡ������ڵ�:");
	#endif
			CSnapTypeVerify verifier(OBJPROVIDER::LINESPACE,SNAP_POINT);
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
			{
			CDisplayNodeAtFrontLife displayNode;
			displayNode.DisplayNodeAtFront();
			while(pSelNode==NULL)
			{
				pCmdLine->FillCmdLine(cmdStr,"");
				if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
				{
					pCmdLine->CancelCmdLine();
					return;
				}
				SELOBJ obj(dwhObj,dwExportFlag);
				pSelNode=console.FromNodeHandle(obj.hRelaObj);
			}
			g_pSolidDraw->SetEntSnapStatus(pSelNode->handle);
			pCmdLine->FinishCmdLine(CXhChar16("�ڵ�0X%X",pSelNode->handle));
			}
			//ѡ������Ǹ�
	#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("please choose the angles whose ends need to be bended��");
	#else
			cmdStr.Format("��ѡ����Ҫ��ͷ����ĽǸ�:");
	#endif	
			verifier.ClearSnapFlag();
			verifier.SetVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINEANGLE));
			pCmdLine->FillCmdLine(cmdStr,"");
			while(true)
			{
				if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
				{
					pCmdLine->CancelCmdLine();
					return;
				}
				SELOBJ obj(dwhObj,dwExportFlag);
				pBendJg=(CLDSLineAngle*)console.FromPartHandle(obj.hRelaObj,CLS_LINEANGLE);
				if(pBendJg==NULL||(pSelNode!=pBendJg->pStart&&pSelNode!=pBendJg->pEnd))
				{
	#ifdef AFX_TARG_ENU_ENGLISH
					errcmdStr.Format("please choose the angles whose ends need to be bended:");
	#else
					errcmdStr.Format("�Ǹ�ѡ������,������ѡ�����ͷ����ĽǸ�:");
	#endif
					pCmdLine->FillCmdLine(errcmdStr,"");
				}
				else
					break;
			}
			g_pSolidDraw->SetEntSnapStatus(pBendJg->handle);
			pCmdLine->FinishCmdLine(CXhChar16("�Ǹ�0X%X",pBendJg->handle));
			//ѡ������߻�׼�Ǹ�
	#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("please choose the angles whose ends need to be bended��");
	#else
			cmdStr.Format("��ѡ������Ǹ�����Խ�Ļ����߻�׼�Ǹ�,��ջ�������,�ֶ�ָ���������������:");
	#endif	
			pCmdLine->FillCmdLine(cmdStr,"");
			f3dPoint span_wing_norm,span_wing_vec,ray_vec,ref_ray_vec;
			UCS_STRU ucs;
			while(true)
			{
				if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
				{
					pCmdLine->CancelCmdLine();
					return;
				}
				SELOBJ obj(dwhObj,dwExportFlag);
				pSpanJg=(CLDSLineAngle*)console.FromPartHandle(obj.hRelaObj,CLS_LINEANGLE);
				if(pSpanJg==NULL)
					break;
				f3dLine line;
				line.startPt=pSpanJg->pStart->Position(false);
				line.endPt = pSpanJg->pEnd->Position(false);
				if(line.PtInLine(pSelNode->Position(false))==0)
				{
	#ifdef AFX_TARG_ENU_ENGLISH
					errcmdStr.Format("The cross angle does not pass through the selected bending nodes, the cross angle is invalid. please choose again��");
	#else
					errcmdStr.Format("����Խ�Ļ�׼�����Ǹ�δͨ����ѡ����ڵ�,������ѡ��:");
	#endif	
					pCmdLine->FillCmdLine(errcmdStr,"");
				}
				else
					break;
			}
			if(pSpanJg)
			{
				g_pSolidDraw->SetEntSnapStatus(pSpanJg->handle);
				pCmdLine->FinishCmdLine(CXhChar16("�Ǹ�0X%X",pSpanJg->handle));
#ifdef AFX_TARG_ENU_ENGLISH
				if(MessageBox("please confirm the cross angle's working leg X(Y)/Y(N)","Message",MB_YESNO)==IDYES)
#else
				if(MessageBox("��ѡ���Խ�ǸֵĻ�׼֫X(Y)/Y(N)Ϊ��Խ��׼֫","��ʾ",MB_YESNO)==IDYES)
#endif
				{
					wing_x0_y1=0;
					span_wing_norm=pSpanJg->get_norm_x_wing();
					span_wing_vec =pSpanJg->GetWingVecX();
				}
				else
				{
					wing_x0_y1=1;
					span_wing_norm=pSpanJg->get_norm_y_wing();
					span_wing_vec =pSpanJg->GetWingVecY();
				}
				pSpanJg->getUCS(ucs,wing_x0_y1);
			}
			else
			{
				pCmdLine->FinishCmdLine();
				CSpecZhiWanFaceVecDlg spec_dlg;
				spec_dlg.m_fNormZ=1.0;
				if(spec_dlg.DoModal())
				{
					span_wing_norm.Set(spec_dlg.m_fNormX,spec_dlg.m_fNormY,spec_dlg.m_fNormZ);
					f3dPoint line_vec(spec_dlg.m_fVectorX,spec_dlg.m_fVectorY,spec_dlg.m_fVectorZ);
					normalize(span_wing_norm);
					//�ų����������������淨�߲����ķ���
					line_vec-=(line_vec*span_wing_norm)*span_wing_norm;
					span_wing_vec=line_vec^span_wing_norm;
					normalize(span_wing_vec);
					if(fabs(span_wing_norm*pBendJg->get_norm_x_wing())>fabs(span_wing_norm*pBendJg->get_norm_y_wing()))
						ucs.origin=pBendJg->GetDatumPosWingX(pSelNode);
					else
						ucs.origin=pBendJg->GetDatumPosWingY(pSelNode);
					ucs.axis_z=span_wing_norm;
					ucs.axis_x=span_wing_vec;
					ucs.axis_y=ucs.axis_z^ucs.axis_x;
				}
				else
#ifdef AFX_TARG_ENU_ENGLISH
					throw "operation is cancelled!";
#else
					throw "����ȡ��!";
#endif
			}
			CInputAnIntegerValDlg huoqudistdlg;
			huoqudistdlg.m_nVal=g_sysPara.HuoQuDist;
#ifdef AFX_TARG_ENU_ENGLISH
			huoqudistdlg.m_sValTitle="bending gap";
#else
			huoqudistdlg.m_sValTitle="������϶";
#endif
			huoqudistdlg.DoModal();
			if(pSelNode==pBendJg->pStart)
				ray_vec=pBendJg->End()-pBendJg->Start();
			else
				ray_vec=pBendJg->Start()-pBendJg->End();
			normalize(ray_vec);
			f3dLine huoquline,cutline[2];
			if(ray_vec*span_wing_vec>0)	//��������֫һ��
			{
				if(pSpanJg)
				{
					huoquline.startPt=pSpanJg->Start()+span_wing_vec*(pSpanJg->GetWidth()+huoqudistdlg.m_nVal);
					huoquline.endPt  =pSpanJg->End()+span_wing_vec*(pSpanJg->GetWidth()+huoqudistdlg.m_nVal);
				}
				else
				{
					huoquline.startPt=ucs.origin+span_wing_vec*huoqudistdlg.m_nVal+ucs.axis_y*1000;
					huoquline.endPt  =ucs.origin+span_wing_vec*huoqudistdlg.m_nVal-ucs.axis_y*1000;
				}
			}
			else	//������������һ��
			{
				if(pSpanJg)
				{
					huoquline.startPt=pSpanJg->xPosStart-span_wing_vec*huoqudistdlg.m_nVal;
					huoquline.endPt  =pSpanJg->xPosEnd-span_wing_vec*huoqudistdlg.m_nVal;
				}
				else
				{
					huoquline.startPt=ucs.origin-span_wing_vec*huoqudistdlg.m_nVal+ucs.axis_y*1000;
					huoquline.endPt  =ucs.origin-span_wing_vec*huoqudistdlg.m_nVal-ucs.axis_y*1000;
				}
			}
			if(fabs(span_wing_norm*pBendJg->get_norm_x_wing())>fabs(span_wing_norm*pBendJg->get_norm_y_wing()))
			{
				if(pSelNode==pBendJg->pStart)
					cutline[0].startPt=pBendJg->Start();
				else
					cutline[0].startPt=pBendJg->End();
				pBendJg->huoqu_base_wing_x0_y1_both2=0;
				cutline[1].startPt = cutline[0].startPt+pBendJg->GetWingVecX()*pBendJg->GetWidth();
			}
			else
			{
				if(pSelNode==pBendJg->pStart)
					cutline[0].startPt=pBendJg->Start();
				else
					cutline[0].startPt=pBendJg->End();
				pBendJg->huoqu_base_wing_x0_y1_both2=1;
				cutline[1].startPt = cutline[0].startPt+pBendJg->GetWingVecY()*pBendJg->GetWidth();
			}
			coord_trans(cutline[0].startPt,ucs,FALSE);
			coord_trans(cutline[1].startPt,ucs,FALSE);
			coord_trans(cutline[0].startPt,ucs,TRUE);
			coord_trans(cutline[0].startPt,ucs,FALSE);
			ref_ray_vec = ray_vec;
			vector_trans(ref_ray_vec,ucs,FALSE);
			if(pSpanJg&&wing_x0_y1==0)	//��Խ�Ǹֵ�X֫Ϊ��Խ֫
				ref_ray_vec.y=cutline[1].startPt.y=cutline[0].startPt.y=0;
			else if(pSpanJg&&wing_x0_y1==1)	//��Խ�Ǹֵ�Y֫Ϊ��Խ֫
				ref_ray_vec.x=cutline[1].startPt.x=cutline[0].startPt.x=0;
			else						//�ֶ�ָ�������������ʸ������
				ref_ray_vec.z=cutline[1].startPt.z=cutline[0].startPt.z=0;
			normalize(ref_ray_vec);	//�ٴε�λ��
			cutline[0].endPt=cutline[0].startPt+ref_ray_vec*200;
			cutline[1].endPt=cutline[1].startPt+ref_ray_vec*200;
			f3dPoint inters_pt[2];
			coord_trans(huoquline.startPt,ucs,FALSE);
			coord_trans(huoquline.endPt,ucs,FALSE);
			if(pSpanJg&&wing_x0_y1==0)
				huoquline.startPt.y=huoquline.endPt.y=cutline[0].startPt.y;
			else if(pSpanJg&&wing_x0_y1==1)
				huoquline.startPt.x=huoquline.endPt.x=cutline[0].startPt.x;
			else
				huoquline.startPt.z=huoquline.endPt.z=cutline[0].startPt.z;
			Int3dpl(cutline[0],huoquline,inters_pt[0]);
			Int3dpl(cutline[1],huoquline,inters_pt[1]);
			double dd1=DISTANCE(cutline[0].startPt,inters_pt[0]);
			double dd2=DISTANCE(cutline[1].startPt,inters_pt[1]);
#ifdef AFX_TARG_ENU_ENGLISH
			huoqudistdlg.m_sValTitle = "bend offset:";
#else
			huoqudistdlg.m_sValTitle = "������ƫ����:";
#endif
			huoqudistdlg.m_nVal = (int)max(dd1,dd2);
			if(huoqudistdlg.DoModal()!=IDOK)
			{
				ReleaseFunc();
				return;
			}
			if(pBendJg->huoqu_handle<0x20)	//��ͨ�Ǹ�
			{
				CLDSLineAngle *pStartSonJg;
				CLDSNode *pHuoQuNode;
				pHuoQuNode=console.AppendNode();
				pHuoQuNode->SetLayer(pBendJg->layer());
				pHuoQuNode->layer(1)='P';
				f3dPoint huoqupt=cutline[0].startPt+ref_ray_vec*huoqudistdlg.m_nVal;
				coord_trans(cutline[0].startPt,ucs,TRUE);
				coord_trans(huoqupt,ucs,TRUE);
				//������������ڵ�λ��
				f3dPoint pos=pSelNode->Position(true);
				coord_trans(pos,ucs,FALSE);
				pos+=ref_ray_vec*huoqudistdlg.m_nVal;
				coord_trans(pos,ucs,TRUE);
				pHuoQuNode->SetPosition(pos);
				
				pStartSonJg=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
				pStartSonJg->CopyProperty(pBendJg);
				pStartSonJg->pStart=pSelNode;
				pStartSonJg->pEnd=pHuoQuNode;
				pStartSonJg->desEndPos.jgber_cal_style=0;	//������ƫ�ƶ�λ
				pStartSonJg->desEndPos.spatialOperationStyle=1;	//���׼�Ǹ�����ͶӰ
				pStartSonJg->desEndPos.wing_x_offset.offsetDist=0;
				pStartSonJg->desEndPos.wing_x_offset.gStyle=4;
				pStartSonJg->desEndPos.wing_y_offset.offsetDist=0;
				pStartSonJg->desEndPos.wing_y_offset.gStyle=4;
				pStartSonJg->SetEnd(huoqupt);
				pStartSonJg->bHuoQuEnd=TRUE;
				pStartSonJg->pre_huoqu_end_xyz=huoqupt;
				pStartSonJg->LockEndPos();
				pHuoQuNode->hFatherPart = pBendJg->handle;
				if(pBendJg->pStart==pSelNode)
				{
					pStartSonJg->desStartPos=pBendJg->desStartPos;
					pStartSonJg->SetStartOdd(pBendJg->startOdd());
					pBendJg->SetStartOdd(0);
					pStartSonJg->SetStart(cutline[0].startPt);
					pStartSonJg->SetEnd(huoqupt);
					pBendJg->SetStart(huoqupt);
					pBendJg->bHuoQuStart=TRUE;
					if(fabs(span_wing_norm*pBendJg->get_norm_x_wing())>fabs(span_wing_norm*pBendJg->get_norm_y_wing()))
					{
						if(span_wing_norm*pBendJg->get_norm_x_wing()>0)
							pStartSonJg->set_norm_x_wing(span_wing_norm,TRUE);
						else
							pStartSonJg->set_norm_x_wing(-span_wing_norm,TRUE);
						pStartSonJg->des_norm_y.bByOtherWing=TRUE;
						pStartSonJg->cal_y_wing_norm();
					}
					else
					{
						if(span_wing_norm*pBendJg->get_norm_y_wing()>0)
							pStartSonJg->set_norm_y_wing(span_wing_norm,TRUE);
						else
							pStartSonJg->set_norm_y_wing(-span_wing_norm,TRUE);
						pStartSonJg->des_norm_x.bByOtherWing=TRUE;
						pStartSonJg->cal_x_wing_norm();
					}
					pBendJg->desStartPos.jgber_cal_style=1;	//ָ���������
					pBendJg->pre_huoqu_start_xyz=pBendJg->Start();
					pBendJg->LockStartPos();
					pBendJg->pStart=pHuoQuNode;
				}
				else
				{
					pStartSonJg->desStartPos=pBendJg->desEndPos;
					pStartSonJg->desStartPos.wing_x_offset.offsetDist=pBendJg->desEndPos.wing_y_offset.offsetDist;
					pStartSonJg->desStartPos.wing_x_offset.gStyle=pBendJg->desEndPos.wing_y_offset.gStyle;
					pStartSonJg->desStartPos.wing_y_offset.offsetDist=pBendJg->desEndPos.wing_x_offset.offsetDist;
					pStartSonJg->desStartPos.wing_y_offset.gStyle=pBendJg->desEndPos.wing_x_offset.gStyle;
					pStartSonJg->SetStartOdd(pBendJg->endOdd());
					pBendJg->SetEndOdd(0);
					pStartSonJg->SetStart(cutline[0].startPt);
					pStartSonJg->SetEnd(huoqupt);
					pBendJg->SetEnd(huoqupt);
					pBendJg->bHuoQuEnd=TRUE;
					if(fabs(span_wing_norm*pBendJg->get_norm_x_wing())>fabs(span_wing_norm*pBendJg->get_norm_y_wing()))
					{
						if(span_wing_norm*pBendJg->get_norm_x_wing()>0)
							pStartSonJg->set_norm_y_wing(span_wing_norm,TRUE);
						else
							pStartSonJg->set_norm_y_wing(-span_wing_norm,TRUE);
						pStartSonJg->des_norm_x.bByOtherWing=TRUE;
						pStartSonJg->cal_x_wing_norm();
					}
					else
					{
						if(span_wing_norm*pBendJg->get_norm_y_wing()>0)
							pStartSonJg->set_norm_x_wing(span_wing_norm,TRUE);
						else
							pStartSonJg->set_norm_x_wing(-span_wing_norm,TRUE);
						pStartSonJg->des_norm_y.bByOtherWing=TRUE;
						pStartSonJg->cal_y_wing_norm();
					}
					pBendJg->desEndPos.jgber_cal_style=1;	//ָ���������
					pBendJg->pre_huoqu_end_xyz=pBendJg->End();
					pBendJg->LockEndPos();
					pBendJg->pEnd=pHuoQuNode;
				}
				pStartSonJg->CalPosition();
				long huoqu_handle=Ta.GetNewHandle();
				pPolyJoint=(CLDSPolyJoint*)console.AppendPart(CLS_POLYJOINT);
				pPolyJoint->cMaterial=pBendJg->cMaterial;
				pPolyJoint->iSeg=pBendJg->iSeg;
				pPolyJoint->SetPartNo(pBendJg->GetPartNo());
				pPolyJoint->pPrevLinePart=pBendJg;
				pPolyJoint->pNextLinePart=pStartSonJg;
				pStartSonJg->pre_huoqu_end_xyz=pStartSonJg->End();
				pStartSonJg->LockEndPos();
				pStartSonJg->huoqu_handle=huoqu_handle;
				pBendJg->huoqu_handle=huoqu_handle;
				CHuoQuOffsetDlg huoqu_dlg;
				huoqu_dlg.m_iHuoQuBaseWing=pBendJg->huoqu_base_wing_x0_y1_both2;
				if(huoqu_dlg.DoModal()!=IDOK)
				{
					//pHuoQuJg->ExplodeHuoQu();
					//console.DeletePart(pHuoQuJg->handle);
#ifdef AFX_TARG_ENU_ENGLISH
					throw "angle bending is cancelled";
#else
					throw "�Ǹ����乤����ȡ��";
#endif
				}
				pBendJg->huoqu_r=huoqu_dlg.m_fHuoQuR;
				pBendJg->huoqu_base_wing_x0_y1_both2 = huoqu_dlg.m_iHuoQuBaseWing;
				NewNode(pHuoQuNode);
				NewLinePart(pStartSonJg);
		}
		else
#ifdef AFX_TARG_ENU_ENGLISH
			throw "the bended angle steel cannot be bended again!";
#else
			throw "�޷��Ի����ӽǸֽ��ж�ͷ����!";
#endif
	}
	catch (CString sError)
	{
		AfxMessageBox(sError);
		ReleaseFunc();
		g_pPromptMsg->Destroy();
		return;
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
		ReleaseFunc();
		g_pPromptMsg->Destroy();
		return;
	}
	RedrawAll();
#ifdef AFX_TARG_ENU_ENGLISH
    MessageBox("the angle end is bend successfully");
#else
	MessageBox("�Ǹֶ�ͷ����ɹ�");
#endif
	ReleaseFunc();
	g_pPromptMsg->Destroy();
	m_pDoc->SetModifiedFlag();
#endif
}

void CLDSView::OnExplodeHuoqu() 
{
#ifdef __ANGLE_PART_INC_
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//-----vvvvvvv-��ʶ��������״̬Ϊ�棬��ͬһʱ��ֻ����һ����������������---------
	if(pFuncStatus->IsLocked())//�жϺ����Ƿ�������������������������
		return;
	else//������������״̬���ڴ˺��������ڼ䲻����������������������
		if(!pFuncStatus->Lock(1000))//�����ɹ����ȴ�һ����
			return;
	//-----^^^^^^^-��ʶ��������״̬Ϊ�棬��ͬһʱ��ֻ����һ����������������---------
	try{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("please select any child member of angle bars:","");
		GET_SCR_PART_PARA scr_part_para(SELECT_LINEPART,CLS_LINEANGLE,CLS_LINETUBE,1);
		scr_part_para.disp_type=DISP_SOLID;
		scr_part_para.cmdStr="please select any child member of bended bar:";
		scr_part_para.cmdErrorFeedbackStr="No proper part is selected, please select any child member of angle bars again:";
#else
		pCmdLine->FillCmdLine("��ѡ������Ǹ��е���һ�������Ӹ˼�:","");
		GET_SCR_PART_PARA scr_part_para(SELECT_LINEPART,CLS_LINEANGLE,CLS_LINETUBE,1);
		scr_part_para.disp_type=DISP_SOLID;
		scr_part_para.cmdStr="��ѡ������Ǹ��е���һ�������Ӹ˼�:";
		scr_part_para.cmdErrorFeedbackStr="û��ѡ�к��ʵĹ�����������ѡ������Ǹ��е���һ�������Ӹ˼�:";
#endif
		if(!GetPartsFromScr(scr_part_para))
		{
			ReleaseFunc();
			return; 
		}
		CUndoOperObject undo(&Ta,true);
		CLDSLinePart *pSelRod=(CLDSLineAngle*)scr_part_para.pResultObjsArr[0];
		if(pSelRod->huoqu_handle>0x20)
		{
			long huoqu_handle=pSelRod->huoqu_handle;
			CSuperSmartPtr<CLDSLinePart>pLinePart;
			for(pLinePart=Ta.Parts.GetFirstLinePart();pLinePart.IsHasPtr();pLinePart=Ta.Parts.GetNextLinePart())
			{
				if(pLinePart->huoqu_handle==huoqu_handle)
				{
					if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pLinePart->dwPermission))
					{
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("No permission to modify��");
#else
						AfxMessageBox("û����Ӧ���޸�Ȩ�ޣ�");
#endif
						ReleaseFunc();
						return;
					}
					else
					{
						pLinePart->UnlockStartPos();
						pLinePart->UnlockEndPos();
						pLinePart->bHuoQuStart = FALSE;
						pLinePart->bHuoQuEnd = FALSE;
						pLinePart->huoqu_handle=0;
						BOOL pushed=Ta.Parts.push_stack();
						for(CLDSPolyJoint *pPolyJoint=(CLDSPolyJoint*)Ta.Parts.GetFirst(CLS_POLYJOINT);pPolyJoint;
						pPolyJoint=(CLDSPolyJoint*)Ta.Parts.GetNext())
						{
							if(pLinePart.IsEqualPtr(pPolyJoint->pPrevLinePart)||pLinePart.IsEqualPtr(pPolyJoint->pNextLinePart))
							{
								g_pSolidDraw->DelEnt(pPolyJoint->handle);
								console.DispPartSet.DeleteNode(pPolyJoint->handle);
								Ta.Parts.DeleteCursor();
							}
						}
						Ta.Parts.pop_stack(pushed);
					}
				}
			}
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox("the bending part exploded successfully!");
#else
			MessageBox("�ɹ�����û�������!");
#endif
		}
		else if(pSelRod->bHuoQuStart||pSelRod->bHuoQuEnd)
			pSelRod->bHuoQuStart=pSelRod->bHuoQuEnd=FALSE;
		else
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("the selected part is not bending angle!");
#else
			AfxMessageBox("ѡ���˵Ĺ������ǻ����ӽǸ�!");
#endif
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	ReleaseFunc();
	m_pDoc->SetModifiedFlag();
#endif
}

#ifdef __ANGLE_PART_INC_
void CLDSView::ShiftApplication(int app_type)
{
	m_xAssistLineList.Empty();	//��ո�����
	m_xAssistPointList.Empty();	//��ո�����
	if(app_type==TA_ASSEMBLE_ENV)
	{
		if(m_curAppType!=TA_ASSEMBLE_ENV)
			g_pSolidOper->PopEnv();
		DISPLAY_TYPE disp_type;
		g_pSolidSet->GetDisplayType(&disp_type);
		if(disp_type==DISP_SOLID)	//���֮ǰ��������ʵ����ʾ״̬�������Ϊʵ����ʾ����
			theApp.m_eEnvType=SOLID_ENV;
		g_pSolidSet->SetDisplayFunc(DrawSolidTower);
		m_curAppType = TA_ASSEMBLE_ENV;
		if(console.m_pWorkPart)
			console.m_pWorkPart->SetModified();
		g_pSolidDraw->BuildDisplayList();
		LoadDefaultUCS(&console.m_curWCS);
		console.m_curWCS.origin.Set();
		g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,console.m_curWCS);
		//g_pSolidSet->SetDispWorkUcs(FALSE);
		//g_pSolidDraw->BatchClearCS(2);
		if(g_sysPara.iColorSchema==1)		//TSA��ɫ����
			g_pSolidSet->SetBkColor(g_sysPara.crTsaMode.crBackground);
		else if(g_sysPara.iColorSchema==2)	//������ɫ����
			g_pSolidSet->SetBkColor(g_sysPara.crMaterialMode.crBackground);
		else								//������ɫ����
			g_pSolidSet->SetBkColor(g_sysPara.crPartMode.crBackground);
		g_pSolidSet->SetSolidAndLineStatus(g_sysPara.display.bSolidLine);
	}
	else
	{
		g_pSolidDraw->ReleaseSnapStatus();
		LoadDefaultUCS(&console.m_curWCS);
		console.m_curWCS.origin.Set();
		CalDefaultAssembleCS(console.m_pWorkPart);
		g_pSolidSet->SetDisplayFunc(DrawSolidPart);
		m_curAppType = TA_PART_EDITOR;
		g_pSolidSet->SetDisplayType(DISP_LINE);
		g_pSolidSet->SetBkSceneMode(0);
		g_pSolidDraw->BuildDisplayList();
		g_pSolidOper->ReSetView();
		g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,console.m_curWCS);
		//g_pSolidSet->SetDispWorkUcs(TRUE);
		g_pSolidSet->SetBkColor(g_sysPara.crPartMode.crPartBackground);
		g_pSolidSet->SetSolidAndLineStatus(FALSE);
		g_pSolidOper->ZoomAll(0.95);
	}
	OnOperOther();
	Invalidate(FALSE);
}
#endif
BOOL IsCanCutPlateByColumnFace(CLDSDbObject *pDbObj);	//From NewPlate.cpp
int BuildSelDbObjSet(DBOBJSET& dbobjset,long *id_arr=NULL,int n=0)
{
	CLDSDbObject *pSelObject;
	CHashSet<CLDSDbObject*> hashDbObjSet;
	if(id_arr==NULL)
		n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	long liInitCount = dbobjset.Count;
	for(int i=0;i<n;i++)
	{
		if (id_arr[i] > 0)
		{
			if ((pSelObject = (CLDSDbObject*)console.FromHandle(id_arr[i], TRUE)) != NULL)
				hashDbObjSet.SetValue(pSelObject->handle, pSelObject);
		}
		else //if(m_curAppType==TA_PART_EDITOR)
		{
			OBJECT_HANDLER handler;
			g_pSolidDraw->GetSolidObject(id_arr[i],&handler);
			if(handler.hiberid.HiberUpId(1)==1)
			{
				CLDSPart* pPart=console.FromPartHandle(handler.hiberid.masterId);
				if(pPart&&pPart->IsBolt())
					hashDbObjSet.SetValue(pPart->handle,pPart);
			}
		}
	}
	for(pSelObject=hashDbObjSet.GetFirst();pSelObject;pSelObject=hashDbObjSet.GetNext())
		dbobjset.append(pSelObject);
	return dbobjset.Count-liInitCount;
}
void CLDSView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	if(pFuncStatus->IsLocked())
		return;	//��ǰ������������״̬��ִ���Ҽ��˵��ļ�
	if(_LocalCurrentOperTypeBeforeRBtnDown!=OPER_OTHER||(m_curTask!=TASK_OTHER&&m_curTask!=TASK_DEF_MAP_SEG_I))
	{//����״̬��ʾ��ݲ˵�
		OnOperOther();
		return;
	}
	CMenu popMenu;
	BOOL bDeleteCirPlateMenu=TRUE;	//ɾ�������˵�
	BOOL bDeleteCutAngleMenu=TRUE;	//ɾ���Ǹ�45���нǲ˵�
	BOOL bDeleteCutPlateByColumnFaceMenu=TRUE;	//ɾ��Բ�����и�ְ�˵� wht 16-10-08
	if(m_curAppType==TA_PART_EDITOR)
	{
		popMenu.LoadMenu(IDR_PART_EDIT_POP_MENU);
		if(console.m_pWorkPart==NULL)
		{
			popMenu.GetSubMenu(0)->DeleteMenu(ID_ASSEMBLE_CS,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_IMPORT_EXIST_LS,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_NEW_LS,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_MOVE_DBOBJECT,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_IMPORT_PLANK_PROFILE,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_COPY_FEATURE,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_PASTE_FEATURE,MF_BYCOMMAND);
		}
		else if(console.m_pWorkPart->GetClassTypeId()!=CLS_PLATE)
		{	//�ǰ�
			popMenu.GetSubMenu(0)->DeleteMenu(ID_IMPORT_PLANK_PROFILE,MF_BYCOMMAND);
		}
	}
	else
	{
		long *id_arr=NULL;
		long i=0,n = g_pSolidSnap->GetLastSelectEnts(id_arr);
		DWORD dwSelPartsTypeFlag=0;
		SEGI iSeg;	//ͳ��ѡ��ͼԪ�Ķκ�
		DYN_ARRAY<int>countStatArr(20);
		memset((int*)countStatArr,0,sizeof(int)*countStatArr.Size());
		DBOBJSET objset;
		n=BuildSelDbObjSet(objset,id_arr,n);
		//for(int i=0;i<n;i++)
		for(CLDSDbObject *pSelObject=objset.GetFirst();pSelObject!=NULL;pSelObject=objset.GetNext())
		{
			//CLDSDbObject *pSelObject=(CLDSDbObject*)console.FromHandle(id_arr[i],TRUE);
			//if(pSelObject==NULL)
			//	continue;
			if(iSeg.iSeg==0&&pSelObject->iSeg.iSeg>0&&pSelObject->IsPart())
				iSeg=pSelObject->iSeg;
			else if(iSeg.iSeg>0&&pSelObject->iSeg.iSeg>0&&pSelObject->iSeg.iSeg!=iSeg.iSeg&&pSelObject->IsPart())
				iSeg=SEGI(-1);	//ѡ�й�������ͬһ������
			if(pSelObject->GetClassTypeId()==CLS_NODE)
				dwSelPartsTypeFlag|=GetSingleWord(SELECTINDEX_NODE);
			else if(pSelObject->GetClassTypeId()==CLS_LINEPART)
				dwSelPartsTypeFlag|=GetSingleWord(SELECTINDEX_LINE);
#ifdef __ANGLE_PART_INC_
			else if(pSelObject->GetClassTypeId()==CLS_LINEANGLE)
			{
				countStatArr[SELECTINDEX_LINEANGLE]++;
				dwSelPartsTypeFlag|=GetSingleWord(SELECTINDEX_LINEANGLE);
			}
			else if(pSelObject->GetClassTypeId()==CLS_GROUPLINEANGLE)
				dwSelPartsTypeFlag|=GetSingleWord(SELECTINDEX_GROUPLINEANGLE);
			else if(pSelObject->GetClassTypeId()==CLS_LINEFLAT)
				dwSelPartsTypeFlag|=GetSingleWord(SELECTINDEX_LINEFLAT);
			else if(pSelObject->GetClassTypeId()==CLS_LINESLOT)
				dwSelPartsTypeFlag|=GetSingleWord(SELECTINDEX_LINESLOT);
			else if(pSelObject->GetClassTypeId()==CLS_LINETUBE)
				dwSelPartsTypeFlag|=GetSingleWord(SELECTINDEX_LINETUBE);
			else if(pSelObject->GetClassTypeId()==CLS_ARCANGLE)
				dwSelPartsTypeFlag|=GetSingleWord(SELECTINDEX_ARCANGLE);
			else if(pSelObject->GetClassTypeId()==CLS_ARCSLOT)
				dwSelPartsTypeFlag|=GetSingleWord(SELECTINDEX_ARCSLOT);
			else if(pSelObject->GetClassTypeId()==CLS_ARCFLAT)
				dwSelPartsTypeFlag|=GetSingleWord(SELECTINDEX_ARCFLAT);
			else if(pSelObject->GetClassTypeId()==CLS_ARCTUBE)
				dwSelPartsTypeFlag|=GetSingleWord(SELECTINDEX_ARCTUBE);
			else if(pSelObject->GetClassTypeId()==CLS_PLATE)
				dwSelPartsTypeFlag|=GetSingleWord(SELECTINDEX_PLATE);
			else if(pSelObject->GetClassTypeId()==CLS_PARAMPLATE)
			{
				dwSelPartsTypeFlag|=GetSingleWord(SELECTINDEX_PARAMPLATE);
				if(((CLDSParamPlate*)pSelObject)->m_bStdPart)
					dwSelPartsTypeFlag|=GetSingleWord(SELECTINDEX_STDPART);
			}
			else if(pSelObject->GetClassTypeId()==CLS_BOLT)
			{
				countStatArr[SELECTINDEX_BOLT]++;
				dwSelPartsTypeFlag|=GetSingleWord(SELECTINDEX_BOLT);
			}
			else if(pSelObject->GetClassTypeId()==CLS_BLOCKREF)
				dwSelPartsTypeFlag|=GetSingleWord(SELECTINDEX_BLOCKREF);
			if(bDeleteCutPlateByColumnFaceMenu&&IsCanCutPlateByColumnFace(pSelObject))
				bDeleteCutPlateByColumnFaceMenu=FALSE;
#endif
		}
		popMenu.LoadMenu(IDR_ASSEMBLE_ENV_POP_MENU);
		if(theApp.m_bAnalysisWorkEnv)
		{	//��ѧ����ģʽ
			popMenu.GetSubMenu(0)->DeleteMenu(ID_SEND_PART_EDITOR,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_SEND_PPE_EDITOR,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_MOVE_DBOBJECT,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_SPEC_NODE_ONEBOLT_DESIGN,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_RETRIEVE_PLATE_SEG_I,MF_BYCOMMAND);
			if(!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_NODE)))	//����ģʽ��ƶ���ֻ����ڵ�λ�� wjh-2015.5.27
				popMenu.GetSubMenu(0)->DeleteMenu(ID_AUTO_DESIGN_SELECTED_PARTS,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_DESIGN_COLUMN_RAY_ROD,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_INTELLIGENT_CONNECT_DESIGN,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_CREATE_BOLT_IN_PART,MF_BYCOMMAND);
			//popMenu.GetSubMenu(0)->DeleteMenu(ID_LOOK_BOLT_IN_PART,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_IMPORT_BOLT_REF_TO_PART,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_CONVERT_TO_GROUP_JG,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_CONVERT_TO_SINGLE_JG,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_CONVERT_TO_TUBE,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_AUTO_CORRECT_JG_LAYSTYLE,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_REMOVE_KAIHE_JIAO,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_CUT_JG,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_GEN_JG_NC_FILE,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_GEN_PART_PROCESSINFO_FILE,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_SET_ARC_LIFT,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_COMPARE_LINEPART_POS,MF_BYCOMMAND);
		}
		else
		{
			popMenu.GetSubMenu(0)->DeleteMenu(ID_CAL_PERFECT_SECT_COUNT,MF_BYCOMMAND);
			if(n<=0)
				popMenu.GetSubMenu(0)->DeleteMenu(ID_AUTO_DESIGN_SELECTED_PARTS,MF_BYCOMMAND);
		}
		if(n<=0)
		{
			popMenu.GetSubMenu(0)->DeleteMenu(ID_RETRIEVE_PLATE_SEG_I,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_HIDE_PART,MF_BYCOMMAND);
		}
		if(n<=0||!theApp.m_bCooperativeWork)
			popMenu.GetSubMenu(0)->DeleteMenu(ID_HANDOVER_PERMISSION,MF_BYCOMMAND);
#ifdef __ANGLE_PART_INC_
		if(n<=0 || !PRODUCT_FUNC::IsHasBlockSetup())
			popMenu.GetSubMenu(0)->DeleteMenu(ID_SAVE_AS_NEW_BLOCK,MF_BYCOMMAND);
		if(n!=1||!(dwSelPartsTypeFlag&(GetSingleWord(SELECTINDEX_BLOCKREF))) || !PRODUCT_FUNC::IsHasBlockSetup())
			popMenu.GetSubMenu(0)->DeleteMenu(ID_EXPLODE_BLOCKREF,MF_BYCOMMAND);
#else
		popMenu.GetSubMenu(0)->DeleteMenu(ID_SAVE_AS_NEW_BLOCK,MF_BYCOMMAND);
		popMenu.GetSubMenu(0)->DeleteMenu(ID_EXPLODE_BLOCKREF,MF_BYCOMMAND);
#endif
#ifdef __ANGLE_PART_INC_
		if(console.m_pWorkPart&&m_curAppType==TA_ASSEMBLE_ENV&&g_sysPara.display.bPartSketch)
		{	//��ʾ����������ʱ��ѡ�е�������֧���ڸ˼���Բ�������Լ��ְ���������˨ wht 10-09-09
			if(!console.m_pWorkPart->IsArcPart()&&!console.m_pWorkPart->IsLinePart()
				&&console.m_pWorkPart->GetClassTypeId()!=CLS_PLATE&&console.m_pWorkPart->GetClassTypeId()!=CLS_PARAMPLATE)
				popMenu.GetSubMenu(0)->DeleteMenu(ID_CREATE_BOLT_IN_PART,MF_BYCOMMAND);
		}
		else
		{
			if(n!=1||!(dwSelPartsTypeFlag&(GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE))))
				popMenu.GetSubMenu(0)->DeleteMenu(ID_CREATE_BOLT_IN_PART,MF_BYCOMMAND);
		}
		bool bEnabledSendPpeEditorMenu=false;
		if( n==0||((n==1||n==2)&&(dwSelPartsTypeFlag==GetSingleWord(SELECTINDEX_PLATE)||
								  dwSelPartsTypeFlag==GetSingleWord(SELECTINDEX_PARAMPLATE)||
								  dwSelPartsTypeFlag==GetSingleWord(SELECTINDEX_LINEANGLE))))
			bEnabledSendPpeEditorMenu=true;
		if(!PRODUCT_FUNC::IsHasPPE()||!bEnabledSendPpeEditorMenu)
			popMenu.GetSubMenu(0)->DeleteMenu(ID_SEND_PPE_EDITOR,MF_BYCOMMAND);
		if(n!=1||!(dwSelPartsTypeFlag&(GetSingleWord(SELECTINDEX_PLATE)
			|GetSingleWord(SELECTINDEX_PARAMPLATE)|GetSingleWord(SELECTINDEX_LINETUBE)
			|GetSingleWord(SELECTINDEX_LINEANGLE)|GetSingleWord(SELECTINDEX_LINESLOT)|GetSingleWord(SELECTINDEX_LINEFLAT)
			|GetSingleWord(SELECTINDEX_ARCANGLE)|GetSingleWord(SELECTINDEX_ARCFLAT)|GetSingleWord(SELECTINDEX_ARCSLOT))))
		{
			popMenu.GetSubMenu(0)->DeleteMenu(ID_IMPORT_BOLT_REF_TO_PART,MF_BYCOMMAND);
			//popMenu.GetSubMenu(0)->DeleteMenu(ID_LOOK_BOLT_IN_PART,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_SEND_PART_EDITOR,MF_BYCOMMAND);
		}
		if(n!=1||!(dwSelPartsTypeFlag&(GetSingleWord(SELECTINDEX_PLATE))))
			popMenu.GetSubMenu(0)->DeleteMenu(ID_IMPORT_LINEPART_TO_PLATE,MF_BYCOMMAND);
		if(n<=0 || !PRODUCT_FUNC::IsHasShadowObject())
			popMenu.GetSubMenu(0)->DeleteMenu(ID_SHADOW_CREATE,MF_BYCOMMAND);
		//��ʱ�����Ž�߸������
		popMenu.GetSubMenu(0)->DeleteMenu(ID_CUT_OPEN_BY_FITTING,MF_BYCOMMAND);
		if(n==1&&(dwSelPartsTypeFlag&(GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE))))
		{
			CLDSGeneralPlate *pBasePlate=(CLDSGeneralPlate*)Ta.FromPartHandle(id_arr[0],CLS_PLATE,CLS_PARAMPLATE);
			if(pBasePlate==NULL||!pBasePlate->IsMaybeBasePlate())
				popMenu.GetSubMenu(0)->DeleteMenu(ID_ADD_ANCHOR_BOLTS,MF_BYCOMMAND);
		}
		else
			popMenu.GetSubMenu(0)->DeleteMenu(ID_ADD_ANCHOR_BOLTS,MF_BYCOMMAND);
#ifndef __COMMON_PART_INC_
		popMenu.GetSubMenu(0)->DeleteMenu(ID_SHADOW_CREATE,MF_BYCOMMAND);
#endif
		if ((n!=1&&n!=2) || !(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_LINEANGLE)))
			popMenu.GetSubMenu(0)->DeleteMenu(ID_OUTER_ATTACH_ANGLE,MF_BYCOMMAND);
		if(n!=1||dwSelPartsTypeFlag!=GetSingleWord(SELECTINDEX_BOLT))	//ÿ��ֻ�ܶ���һ����˨
			popMenu.GetSubMenu(0)->DeleteMenu(ID_ALIGN_BOLT_ALONG_LENVEC,MF_BYCOMMAND);
		if(!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_BOLT)))
		{
			popMenu.GetSubMenu(0)->DeleteMenu(ID_REVISION_BOLT_L0nZPOS,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_REVISION_BOLT_SPACE,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_REPLACE_BOLT_DATUMPOINT,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_MOVE_BOLT_SET,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_BOLT_PUNCH_HOLE,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_SWAP_G,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_UNIFY_ANGLE_BOLT_G,MF_BYCOMMAND);
		}
		else
		{
			if(n<=1)	//������˨�ľ�������Ҫ������˨
				popMenu.GetSubMenu(0)->DeleteMenu(ID_SWAP_G,MF_BYCOMMAND);
#ifndef __SMART_DRAW_INC_	//������˨��׼�϶��һ��ֻ���ڻ�ͼʱȡ�����߽Ǹ���˨���������˨��ļ�ࡡwjh-2018.1.22
			popMenu.GetSubMenu(0)->DeleteMenu(ID_REVISION_BOLT_SPACE,MF_BYCOMMAND);
#endif
		}
		if(n<=0)
		{
			popMenu.GetSubMenu(0)->DeleteMenu(ID_DAOWEI_SELECT_PART,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_MIRROR_CREATE,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_SYNC_MIR_OBJ_PROP,MF_BYCOMMAND);
		}
#endif
		if(n<=0)
			popMenu.GetSubMenu(0)->DeleteMenu(ID_ADD_RELATIVE_OBJECT,MF_BYCOMMAND);
#ifdef __ANGLE_PART_INC_
#if defined(__LDS_)||defined(__TDA_)
		if(!VerifyValidFunction(LICFUNC::FUNC_IDENTITY_PROCESS_EXPORT)||
			theApp.m_bCooperativeWork||!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_LINEANGLE)))
#else
		if(theApp.m_bCooperativeWork||!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_LINEANGLE)))
#endif
			popMenu.GetSubMenu(0)->DeleteMenu(ID_GEN_JG_NC_FILE,MF_BYCOMMAND);
#if defined(__LDS_)||defined(__TDA_)
		if(!VerifyValidFunction(LICFUNC::FUNC_IDENTITY_PROCESS_EXPORT)||
			theApp.m_bCooperativeWork||!(dwSelPartsTypeFlag&(GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE))))
#else
		if( theApp.m_bCooperativeWork||!(dwSelPartsTypeFlag&(GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE))))
#endif
			popMenu.GetSubMenu(0)->DeleteMenu(ID_GEN_PLANK_NC_DATA,MF_BYCOMMAND);

#if defined(__LDS_)||defined(__TDA_)
		if(!VerifyValidFunction(LICFUNC::FUNC_IDENTITY_PROCESS_EXPORT)||
			theApp.m_bCooperativeWork||(!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_LINEANGLE))&&
			!(dwSelPartsTypeFlag&(GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE)))))
#else
		if(theApp.m_bCooperativeWork||(!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_LINEANGLE))&&
			!(dwSelPartsTypeFlag&(GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE)))))
#endif
			popMenu.GetSubMenu(0)->DeleteMenu(ID_GEN_PART_PROCESSINFO_FILE,MF_BYCOMMAND);

		if(!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_PLATE)))
			popMenu.GetSubMenu(0)->DeleteMenu(ID_PLATE_PROFILE_RECT,MF_BYCOMMAND);
		if(!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_PARAMPLATE)))	//ѡ�й������в�������ʱӦ��ʾת��Ϊ��ͨ��(��ѡʱ����ѡ����˨) 
			popMenu.GetSubMenu(0)->DeleteMenu(ID_CONVERT_TO_PLATE,MF_BYCOMMAND);
		if(!(dwSelPartsTypeFlag&(GetSingleWord(SELECTINDEX_LINEANGLE)|GetSingleWord(SELECTINDEX_GROUPLINEANGLE))))
		{
			popMenu.GetSubMenu(0)->DeleteMenu(ID_AUTO_CORRECT_JG_LAYSTYLE,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_CONVERT_TO_GROUP_JG,MF_BYCOMMAND);
			//popMenu.GetSubMenu(0)->DeleteMenu(ID_AUTO_CORRECT_1JG_LS,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_REMOVE_KAIHE_JIAO,MF_BYCOMMAND);
		}
		if( !(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_GROUPLINEANGLE))&&
			!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_LINETUBE)))
			popMenu.GetSubMenu(0)->DeleteMenu(ID_CONVERT_TO_SINGLE_JG,MF_BYCOMMAND);
		if(//n!=2||countStatArr[SELECTINDEX_LINEANGLE]!=2||
			!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_LINEANGLE)))
			popMenu.GetSubMenu(0)->DeleteMenu(ID_CUT_JG,MF_BYCOMMAND);
		if(!theApp.m_bAnalysisWorkEnv && n>0 && dwSelPartsTypeFlag==GetSingleWord(SELECTINDEX_LINEANGLE))
			bDeleteCutAngleMenu=FALSE;	//�˴��ж��Ƿ���Ҫɾ���Ǹ�45���нǲ˵�����������ɾ������ wht 13-12-03
#endif
#ifndef __COMMON_PART_INC_
		popMenu.GetSubMenu(0)->DeleteMenu(ID_SET_ARC_LIFT,MF_BYCOMMAND);
#else
		if(!PRODUCT_FUNC::IsHasAutoArcLift())
			popMenu.GetSubMenu(0)->DeleteMenu(ID_SET_ARC_LIFT,MF_BYCOMMAND);
#endif
		//ת��Ϊ�ֹ��������TDA������ wht 12-04-28
#ifdef __COMPLEX_PART_INC_
		if(!(dwSelPartsTypeFlag&(GetSingleWord(SELECTINDEX_LINEANGLE)|
			GetSingleWord(SELECTINDEX_LINESLOT)|GetSingleWord(SELECTINDEX_LINEFLAT))))
#endif
			popMenu.GetSubMenu(0)->DeleteMenu(ID_CONVERT_TO_TUBE,MF_BYCOMMAND);
#ifdef __TMA_
		popMenu.GetSubMenu(0)->DeleteMenu(ID_CUT_PLATE_BY_COLUMN_FACE,MF_BYCOMMAND);
#else
		if(bDeleteCutPlateByColumnFaceMenu)
			popMenu.GetSubMenu(0)->DeleteMenu(ID_CUT_PLATE_BY_COLUMN_FACE,MF_BYCOMMAND);
#endif
#ifdef __ANGLE_PART_INC_
		if(n!=1||dwSelPartsTypeFlag!=GetSingleWord(SELECTINDEX_PLATE))
		{
			popMenu.GetSubMenu(0)->DeleteMenu(ID_EXTEND_PLATE,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_CUT_PLATE_BY_PLANE,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_MAKE_PLATE_BEND,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_ROLL_PLANK_EDGE,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_COPY_TO_CLIPBOARD,MF_BYCOMMAND);
			//popMenu.GetSubMenu(0)->DeleteMenu(ID_MERGE_LJB,MF_BYCOMMAND);
		}
		if(n!=1||dwSelPartsTypeFlag==GetSingleWord(SELECTINDEX_NODE))
			popMenu.GetSubMenu(0)->DeleteMenu(ID_DISPLAY_PART_WORK_UCS,MF_BYCOMMAND);
#endif
		if(n!=1||g_sysPara.dock.pageProp.bDisplay)
			popMenu.GetSubMenu(0)->DeleteMenu(ID_OBJECT_PROPERTY,MF_BYCOMMAND);
		if(!PRODUCT_FUNC::IsHasColumnRayRod() || n!=1||!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_LINETUBE)))
			popMenu.GetSubMenu(0)->DeleteMenu(ID_DESIGN_COLUMN_RAY_ROD,MF_BYCOMMAND);
		if(dwSelPartsTypeFlag==0||dwSelPartsTypeFlag==GetSingleWord(SELECTINDEX_NODE))
			popMenu.GetSubMenu(0)->DeleteMenu(ID_RECREATE_SPEC_PART_SOLIDMODEL,MF_BYCOMMAND);
		if(n<=0)
			popMenu.GetSubMenu(0)->DeleteMenu(ID_REMOVE_OBJS_FROM_VIEW,MF_BYCOMMAND);
#ifdef __PART_DESIGN_INC_
		if(!theApp.m_bAnalysisWorkEnv||(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_NODE))==0)
#endif
			popMenu.GetSubMenu(0)->DeleteMenu(ID_UPGRADE_CTRL_NODE,MF_BYCOMMAND);
#ifndef __TSA_	//TSA������˨����˵���
		if(n<=0||theApp.m_bAnalysisWorkEnv||
			!(dwSelPartsTypeFlag&(GetSingleWord(SELECTINDEX_LINEANGLE)|GetSingleWord(SELECTINDEX_LINETUBE)|GetSingleWord(SELECTINDEX_PLATE)|
			GetSingleWord(SELECTINDEX_PARAMPLATE))))
#endif
		{
			popMenu.GetSubMenu(0)->DeleteMenu(ID_SELECT_RELA_BOLTS,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_ADD_RELA_BOLTS_TO_VIEW,MF_BYCOMMAND);
		}
#ifndef __TSA_	//TSA�С����û�ͼƽ�桿�˵���
		if(n<=0||theApp.m_bAnalysisWorkEnv||console.GetActiveView()->GetSpreadFaceCount(iSeg.iSeg)<=0||
			dwSelPartsTypeFlag==GetSingleWord(SELECTINDEX_NODE))
#endif
			popMenu.GetSubMenu(0)->DeleteMenu(ID_SET_SPREAD_FACE,MF_BYCOMMAND);
		if(n>0)//||console.GetActiveView()->m_iViewType!=1)��ʱչ����ͼ(��Ϊͼ����ͼ���о������õ������� wjh-2015.5.17
			popMenu.GetSubMenu(0)->DeleteMenu(ID_INSERT_JGNODE_TO_VIEW,MF_BYCOMMAND);

		if(n<=1)
			popMenu.GetSubMenu(0)->DeleteMenu(ID_GROUP_EDIT_PART_LAYER,MF_BYCOMMAND);
		if(n!=1||!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_LINE)||dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_LINEANGLE)))
			popMenu.GetSubMenu(0)->DeleteMenu(ID_CAL_PERFECT_SECT_COUNT,MF_BYCOMMAND);
		if(n==1&&dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_STDPART))
		{	//��׼������������(������˨����������˨ͨ��)
			//�����ڱ�׼�����ϳ�ʼ©��˨ʱ���޷�����˨�����׼�����������˨ͨ������,������ʱ�ſ� wjh-2014.11.11
			//popMenu.GetSubMenu(0)->DeleteMenu(ID_IMPORT_BOLT_REF_TO_PART,MF_BYCOMMAND);
			//�Ѵ��Ҽ��˵��и����Ƴ�ID_CREATE_BOLT_IN_PART��wjh-2018.1.29
			//popMenu.GetSubMenu(0)->DeleteMenu(ID_CREATE_BOLT_IN_PART,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_SEND_PART_EDITOR,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_SEND_PPE_EDITOR,MF_BYCOMMAND);
		}
		//�ֹܲ�֧�ַ��͵������༭�� wht 12-03-19
		if(n==1&&dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_LINETUBE))
		{
			popMenu.GetSubMenu(0)->DeleteMenu(ID_SEND_PART_EDITOR,MF_BYCOMMAND);
			popMenu.GetSubMenu(0)->DeleteMenu(ID_SEND_PPE_EDITOR,MF_BYCOMMAND);
		}
		//������ƽ�ƽڵ㡡wjh-2016.8.21
		//if(n!=1||dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_NODE))
		if(n==0)
			popMenu.GetSubMenu(0)->DeleteMenu(ID_MOVE_DBOBJECT,MF_BYCOMMAND);
		else	//��ID_MOVE_DBOBJECT<�ƶ�λ��>�˵�ʱ��<�ƶ���˨>������������� wjh-2016.8.22
			popMenu.GetSubMenu(0)->DeleteMenu(ID_MOVE_BOLT_SET,MF_BYCOMMAND);
#ifndef __PART_DESIGN_INC_
		popMenu.GetSubMenu(0)->DeleteMenu(ID_EXPORT_SPEC_PART_EXCEL_FILE,MF_BYCOMMAND);
#else
		if(!theApp.m_bAnalysisWorkEnv||n<2)
			popMenu.GetSubMenu(0)->DeleteMenu(ID_EXPORT_SPEC_PART_EXCEL_FILE,MF_BYCOMMAND);
#endif
		if(n<2||!(n>=2&&dwSelPartsTypeFlag==GetSingleWord(SELECTINDEX_PLATE)))
			popMenu.GetSubMenu(0)->DeleteMenu(ID_ADJUST_PLATE_WELD_LEN,MF_BYCOMMAND);
		if(n!=2||!(n==2&&(dwSelPartsTypeFlag==GetSingleWord(SELECTINDEX_LINEANGLE)||
			dwSelPartsTypeFlag==GetSingleWord(SELECTINDEX_LINETUBE))))
			popMenu.GetSubMenu(0)->DeleteMenu(ID_COMPARE_LINEPART_POS,MF_BYCOMMAND);
		if(n<1||!(n>=1&&(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_PARAMPLATE))))
			popMenu.GetSubMenu(0)->DeleteMenu(ID_LOOK_DESIGN_PARAM,MF_BYCOMMAND);
		if(n<1||!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_NODE)))
			popMenu.GetSubMenu(0)->DeleteMenu(ID_SPEC_NODE_ONEBOLT_DESIGN,MF_BYCOMMAND);
		//�����������
#ifdef __ANGLE_PART_INC_//__LDS_
		if(theApp.m_bAnalysisWorkEnv||(n>0&&!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_NODE))))
#endif
			popMenu.GetSubMenu(0)->DeleteMenu(ID_INTELLIGENT_CONNECT_DESIGN,MF_BYCOMMAND);
#ifdef __SMART_DRAW_INC_//__LDS_
		if(theApp.m_bAnalysisWorkEnv||!PRODUCT_FUNC::IsHasInternalTest()||
			(n>0&&!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_NODE))))
#endif
			popMenu.GetSubMenu(0)->DeleteMenu(ID_CONNECT_FROM_TEMPL,MF_BYCOMMAND);
#ifdef __SMART_DRAW_INC_//__LDS_
		if(theApp.m_bAnalysisWorkEnv||!PRODUCT_FUNC::IsHasInternalTest()||n==0||
			!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_PLATE)))
#endif
			popMenu.GetSubMenu(0)->DeleteMenu(ID_EXPORT_CONNECT_TEMPL,MF_BYCOMMAND);
		if(n==0||dwSelPartsTypeFlag!=GetSingleWord(SELECTINDEX_NODE))
			popMenu.GetSubMenu(0)->DeleteMenu(ID_MERGE_NODES,MF_BYCOMMAND);
		if(n==0||!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_NODE)))
			popMenu.GetSubMenu(0)->DeleteMenu(ID_RELAYOUT_SCALE_NODE,MF_BYCOMMAND);
		/*BOOL bCanCopyPlate=TRUE;
		if(n==1&&dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_PLATE))
		{	//��ʱֻ֧����ϽǸ�����Լ�δ�������߸˼��ĸְ�ĸ���ճ��
			CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(id_arr[0],CLS_PLATE);
			CLDSGroupLineAngle *pGroupLineAngle=NULL;
			if(pPlate&&pPlate->designInfo.m_hBasePart>0X20)
				pGroupLineAngle=(CLDSGroupLineAngle*)console.FromPartHandle(pPlate->designInfo.m_hBasePart,CLS_GROUPLINEANGLE);
			int nPole=1;
			if(pGroupLineAngle&&pGroupLineAngle->group_style<2)
				nPole=2;
			else if(pGroupLineAngle&&pGroupLineAngle->group_style==2)
				nPole=4;
			if(pPlate&&pPlate->designInfo.partList.GetNodeNum()<=nPole)
				bCanCopyPlate=TRUE;
		}*/
		if(n!=1||!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_PLATE)))
			popMenu.GetSubMenu(0)->DeleteMenu(ID_COPY_PLATE_TO_BUFFER,MF_BYCOMMAND);
		//���Զ�ȡ�����������ݣ���鱣��������Ƿ�Ϊ�ְ�����
		char sName[50]="";
		console.clipper_board.SeekToBegin();
		if(console.clipper_board.ReadString(sName,50)>=50)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("the key string length exceeds the clipboard limit. Memory overflow is found, please save files and close the program promptly ");
#else
			AfxMessageBox("���а��趨�ؼ��ַ����������ȼ��ޣ��ѳ����ڴ���������������̣��رճ���");
#endif
		if(!(dwSelPartsTypeFlag&GetSingleWord(SELECTINDEX_NODE))||(stricmp(sName,"Plate")!=0&&stricmp(sName,"ClonePlate")!=0))
			popMenu.GetSubMenu(0)->DeleteMenu(ID_PASTE_PLATE_FROM_BUFFER,MF_BYCOMMAND);
		//��ʾ�ϴ�ִ�еĲ��� Ӧ�÷������ȥ���£�������ܻᱻɾ����
		int nRepeatedCommand=2;	//�����ɾ����һ������ظ�����λ�ڵ�3λ ɾ����λ�ڵ�һλ
		popMenu.GetSubMenu(0)->DeleteMenu(ID_AFFIRM_CUR_COMMAND,MF_BYCOMMAND);	//ȷ�� ��ʱδ�����Ҽ�
		nRepeatedCommand=1;
		if(pCmdLine&&!pCmdLine->m_bGettingStrFromCmdLine&&pFuncStatus&&!pFuncStatus->IsLocked())
		{
			popMenu.GetSubMenu(0)->DeleteMenu(ID_CANCEL_CUR_COMMAND,MF_BYCOMMAND);	//ȡ��
			nRepeatedCommand=0;
		}
		if(m_nPrevCommandID>0)
			popMenu.GetSubMenu(0)->ModifyMenu(nRepeatedCommand,MF_BYPOSITION,m_nPrevCommandID,m_sPrevCommandName);
		else 
			popMenu.GetSubMenu(0)->DeleteMenu(ID_REPEATED_COMMAND,MF_BYCOMMAND);
		//�˴��ж��Ƿ���Ҫɾ�������˵���ɾ������������� wht 11-03-22
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
		if(dwSelPartsTypeFlag==GetSingleWord(SELECTINDEX_PLATE)&&(n==1||n==2))
		{	//ѡ��һ�������ְ�
			CLDSPlate *pPlate1=NULL,*pPlate2=NULL;
			pPlate1=(CLDSPlate*)console.FromPartHandle(id_arr[0],CLS_PLATE);
			if(n==2)
				pPlate2=(CLDSPlate*)console.FromPartHandle(id_arr[1],CLS_PLATE);
			if(pPlate1)
			{
				CLDSNode *pDatumNode=console.FromNodeHandle(pPlate1->designInfo.m_hBaseNode);
				CLDSLineTube *pDatumTube=(CLDSLineTube*)console.FromPartHandle(pPlate1->designInfo.m_hBasePart,CLS_LINETUBE);
				if(pDatumNode&&pDatumTube)
				{
					if(n==1)
						bDeleteCirPlateMenu=FALSE;
					else if(n==2&&pPlate2&&pDatumNode->handle==pPlate2->designInfo.m_hBaseNode
						&&pDatumTube->handle==pPlate2->designInfo.m_hBasePart)
						bDeleteCirPlateMenu=FALSE;
				}
			}
		} 
#endif
	}
	int nMenuItem=popMenu.GetSubMenu(0)->GetMenuItemCount();
	BOOL bPrevSeparator=FALSE;
	//ɾ�������˵� wht 11-03-22
	int j;
	for(j=0;j<nMenuItem;j++)
	{
		CMenu *pCirPlateMenu=popMenu.GetSubMenu(0)->GetSubMenu(j);
		if(pCirPlateMenu)
		{
			if((pCirPlateMenu->GetMenuItemID(0)==ID_FAST_CIRCOVER_PLATE&&bDeleteCirPlateMenu)||
				(pCirPlateMenu->GetMenuItemID(0)==ID_START_WINGX_CUT_ANGLE&&bDeleteCutAngleMenu))
				popMenu.GetSubMenu(0)->DeleteMenu(j,MF_BYPOSITION);
		}
	}
	//ɾ������ķָ���
	for(j=0;j<nMenuItem;j++)
	{
		if(popMenu.GetSubMenu(0)->GetMenuItemID(j)==ID_SEPARATOR)
		{
			if(bPrevSeparator||j==0||j==nMenuItem-1)
			{
				popMenu.GetSubMenu(0)->DeleteMenu(j,MF_BYPOSITION);
				j--;
			}
			else
				bPrevSeparator=TRUE;
		}
		else
			bPrevSeparator=FALSE;
	}
	//popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,this);
	//ʹ��theApp.GetContextMenuManager()->ShowPopupMenu��ʾ�Ҽ��˵���֧����ʾͼ�� wht 15-03-01
	theApp.GetContextMenuManager()->ShowPopupMenu(popMenu.GetSubMenu(0)->m_hMenu,point.x,point.y,this);
}

void CLDSView::OnBillOfFL() 
{
#if defined(__LDS_)||defined(__TSA_)
	CFLBillDlg fl_bill;
	fl_bill.DoModal();
#endif	
}

#ifdef __PART_DESIGN_INC_
static CAngleJointsDesignForm anglejointform;
void CLDSView::OnAngleJointDesignCal()
{
	CLogErrorLife loglife;
	if(Ta.hashAngleJoint.GetNodeNum()==0)
	{
		ATOM_LIST<CJointCouple> listCouple;
		//���ɽǸֽ�ͷƥ���
		CJointCouple::StatJointLinkPart(&listCouple,CLS_LINEANGLE);
		CJointCouple *pCouple;
		//ȷ���������������ѧ������Ԫ
		for(CElement* pElem=postprocessor.hashElems.GetFirst();pElem;pElem=postprocessor.hashElems.GetNext())
		{
			if(pElem->pStart!=pElem->pOrgPole->pStart&&pElem->pEnd!=pElem->pOrgPole->pEnd)
				continue;
			if(pElem->pOrgPole->size_idClassType!=CLS_LINETUBE)
				continue;
			for(pCouple=listCouple.GetFirst();pCouple;pCouple=listCouple.GetNext())
			{
				if(pCouple->pLinkRod==pElem->pOrgPole)
				{
					if( (pElem->pStart==pElem->pOrgPole->pStart&&pElem->pStart==pCouple->pNode)||
						(pElem->pEnd==pElem->pOrgPole->pEnd&&pElem->pEnd==pCouple->pNode))
					{
						pCouple->N=pElem->MaxTension.N;
						pCouple->M=0;	//�ݲ��������
					}
				}
			}
		}
		CAngleJointDesigner::GenAngleJointSet(&listCouple,&Ta.hashAngleJoint);
	}
	anglejointform.hashAngleJoints.Empty();
	for(ANGLE_JOINT* pJoint=Ta.hashAngleJoint.GetFirst();pJoint;pJoint=Ta.hashAngleJoint.GetNext())
	{
		ANGLE_JOINT* pNewJoint=anglejointform.hashAngleJoints.Add(pJoint->Id());
		*pNewJoint=*pJoint;
		pNewJoint->pTagInfo=NULL;
	}
	anglejointform.DisplayWindow();
}
#else
void CLDSView::OnAngleJointDesignCal(){;}
#endif

void CLDSView::OnAttachPartTbl() 
{
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
	//CSuppPartTblDlg dlg;
	CSuppPartListDlg dlg;
	dlg.DoModal();
#endif
}

void CLDSView::OnImportPart() 
{
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
	CFindPartDlg dlg;
	if(dlg.DoModal()!=IDOK)
		return;
	CLDSDbObject *pLDSObject = (CLDSDbObject*)console.FromHandle(dlg.m_nPartHandle,TRUE);
	if(pLDSObject==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("Your specified part is not found");
#else
		MessageBox("δ��������ָ������Ĺ���");
#endif
	else
	{
		if(pLDSObject->GetClassTypeId()==CLS_NODE)
			return;
		CMainFrame* pMainFrm=(CMainFrame*)AfxGetMainWnd();
		if(console.m_pWorkPart&&console.m_pWorkPart!=pLDSObject)
			pMainFrm->prev_part.push(console.m_pWorkPart->handle);	//����һ����ѹ���ջ
		console.m_pWorkPart = (CLDSPart*)pLDSObject;
		UpdatePropertyPage(console.m_pWorkPart);	//����������
		CalDefaultAssembleCS(console.m_pWorkPart);
		LoadDefaultUCS(&console.m_curWCS);
		console.m_curWCS.origin.Set();
		g_pSolidDraw->BuildDisplayList();
		g_pSolidOper->ReSetView();
		g_pSolidOper->ZoomAll(0.95);
		Invalidate(FALSE);
	}
#endif
}

void CLDSView::OnPrevView()
{
	UndoView();
}

void CLDSView::OnNextView()
{
	RedoView();
}

void CLDSView::OnUpdatePrevView(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(CanUndoView());
}

void CLDSView::OnUpdateNextView(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(CanRedoView());
}

void CLDSView::OnSendPartEditor() 
{
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
	long *id_arr,id_num=g_pSolidSnap->GetLastSelectEnts(id_arr);
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSDbObject *pObj=NULL;
	GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECT_ALL));
	scr_part_para.disp_type=DISP_SOLID;
	if(id_num==1)
		pObj=console.FromPartHandle(id_arr[0]);
	if(pObj==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		scr_part_para.cmdStr="please select the part to be sent to part editor:";
		scr_part_para.cmdErrorFeedbackStr="invalid part or more parts selected, please select again:";
#else
		scr_part_para.cmdStr="��ѡ��Ҫ���͹����༭���Ĺ���:";
		scr_part_para.cmdErrorFeedbackStr="ѡ���˴���Ĺ�����ѡ���˶��������������ѡ��:";
#endif
		if(!GetPartsFromScr(scr_part_para))
			return; 
		pObj=scr_part_para.pResultObjsArr[0];
		if(pObj==NULL)
			return;
		if(!pObj->IsPart())
			return;
	}
	console.m_pWorkPart = (CLDSPart*)pObj;
	if(m_curAppType==TA_ASSEMBLE_ENV)
		g_pSolidOper->PushEnv();
	((CMainFrame*)AfxGetMainWnd())->ShiftToPartEditor();
	ShiftApplication(TA_PART_EDITOR);
	g_pSolidOper->ZoomAll(0.95);
#ifdef AFX_TARG_ENU_ENGLISH
    pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("����:","");
#endif
#endif
}
void CLDSView::OnSendPpeEditor() 
{
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
	long *id_arr,id_num=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(id_num==0||id_num==1||id_num==2)
	{
		PARTSET partSet;
		if(id_num==2)
		{
			CLDSPart *pFirstPart=console.FromPartHandle(id_arr[0],CLS_PLATE,CLS_PARAMPLATE,CLS_LINEANGLE);
			CLDSPart *pSecPart=console.FromPartHandle(id_arr[1],CLS_PLATE,CLS_PARAMPLATE,CLS_LINEANGLE);
			if(pFirstPart&&pSecPart&&pFirstPart->GetClassTypeId()==pSecPart->GetClassTypeId())
			{
				partSet.append(pFirstPart);
				partSet.append(pSecPart);
			}
		}
		else if(id_num==0)
		{
			CHashList<SEGI>segTbl;
			GetSegNoHashTblBySegStr(Ta.segstr,segTbl);
			for(CLDSPart *pPart=console.DispPartSet.GetFirst();pPart;pPart=console.DispPartSet.GetNext())
			{
				if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)		//������ϽǸ֣������»�׼�ӽǸֲ���ʾ��
					continue;
				if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pPart->iSeg)==NULL)
					continue;
				partSet.append(pPart);
			}
		}
		else //if(id_num==1)
		{
			CLDSPart *pPart=console.FromPartHandle(id_arr[0],CLS_PLATE,CLS_PARAMPLATE,CLS_LINEANGLE);
			if(pPart)
				partSet.append(pPart);
		}	
		PPEStarter(&partSet,id_num==0&&partSet.GetNodeNum()>0);
		return;
	}
#endif
}
void CLDSView::OnUpdateSendPartEditor(CCmdUI* pCmdUI)
{
	long *id_arr=NULL;
	int n=g_pSolidSnap->GetLastSelectEnts(id_arr);
}
void CLDSView::ClearSelectRodNail(LINEPARTSET& xLinePartSet)
{
	if(xLinePartSet.GetNodeNum()<=0)
		return;
	//���ѡ�и˼��ϵ���˨�Ŷ�
	CUndoOperObject undo(&Ta);
	THANDLE ls_handle;
	CStack<THANDLE> del_ls_stack;
	for(CLDSLinePart* pRod=xLinePartSet.GetFirst();pRod;pRod=xLinePartSet.GetNext())
	{
		for(CLsRef* pLsRef=pRod->GetFirstLsRef();pLsRef;pLsRef=pRod->GetNextLsRef())
		{
			CLDSBolt* pBolt=pLsRef->GetLsPtr();
			if(!pBolt->IsFootNail())
				continue;
			pBolt->restore_oldL();
			pBolt->m_dwFlag.SetFlagDword(CLDSBolt::FOOT_NAIL,FALSE);
			if(pBolt->get_oldL()<EPS)
			{
				del_ls_stack.push(pBolt->handle);
				CLDSLineSlot *pLineSlot=(CLDSLineSlot*)console.FromPartHandle(pBolt->des_base_pos.hPart,CLS_LINESLOT);
				if(pLineSlot==NULL)
					continue;
				if(pLineSlot->pStart)
					console.DeleteNode(pLineSlot->pStart->handle);
				if(pLineSlot->pEnd)
					console.DeleteNode(pLineSlot->pEnd->handle);
				console.DeletePart(pLineSlot->handle);
			}
			else
			{
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			}
		}
	}
	while(del_ls_stack.pop(ls_handle))
		Ta.DelBoltAndRelativeBoltRef(ls_handle);
	//���ѡ�и˼��ϵĽǸֽŶ�
	for(CLDSLineAngle *pLineAngle=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pLineAngle;pLineAngle=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
	{
		if(pLineAngle->m_bFootNail && xLinePartSet.FromHandle(pLineAngle->m_hPartWeldParent))
			console.DeletePart(pLineAngle->handle);
	}
	Ta.Parts.Clean();
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	MessageBox("Remove cleging bolts successfully!");
#else
	MessageBox("�ɹ�����Ŷ�!");
#endif
}
void CLDSView::OnClearNail() 
{
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
	m_nPrevCommandID=ID_CLEAR_NAIL;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat to remove cleging bolts";
#else
	m_sPrevCommandName="�ظ�����Ŷ�";
#endif
	LINEPARTSET xSelectRodSet;
	long *id_arr=NULL,nn=g_pSolidSnap->GetLastSelectEnts(id_arr);
	for(int i=0;i<nn;i++)
	{
		CLDSLinePart *pRod=(CLDSLinePart*)console.FromPartHandle(id_arr[i],CLS_LINEANGLE,CLS_GROUPLINEANGLE,CLS_LINETUBE);
		if(pRod && pRod->IsHasFootNail())
			xSelectRodSet.append(pRod);
	}
	if(xSelectRodSet.GetNodeNum()>0)
		return ClearSelectRodNail(xSelectRodSet);
	//
	CLDSBolt *pLs;
	CStack<THANDLE>del_ls_stack;
	CUndoOperObject undo(&Ta);
	for(pLs=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pLs;pLs=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
	{
		if(!pLs->IsFootNail())
			continue;
		pLs->restore_oldL();
		pLs->m_dwFlag.SetFlagDword(CLDSBolt::FOOT_NAIL,FALSE);
		if(pLs->liInitL<=0)
		{	
			del_ls_stack.push(pLs->handle);
			CLDSLineSlot *pLineSlot=(CLDSLineSlot*)console.FromPartHandle(pLs->des_base_pos.hPart,CLS_LINESLOT);
			if(!pLineSlot)
				continue;
			if(pLineSlot->pStart)
			{	
				console.DispNodeSet.DeleteNode(pLineSlot->pStart->handle);
				console.DeleteNode(pLineSlot->pStart->handle);
			}
			if(pLineSlot->pEnd)
			{	
				console.DispNodeSet.DeleteNode(pLineSlot->pEnd->handle);
				console.DeleteNode(pLineSlot->pEnd->handle);
			}
			console.DispPartSet.DeleteNode(pLineSlot->handle);
			console.DeletePart(pLineSlot->handle);
		}
	}
	THANDLE ls_handle;
	while(del_ls_stack.pop(ls_handle))
		Ta.DelBoltAndRelativeBoltRef(ls_handle);
	for(CLDSLineAngle *pLineAngle=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pLineAngle;pLineAngle=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
	{	//����ǸֽŶ�
		if(pLineAngle->m_bFootNail)
		{
			console.DispPartSet.DeleteNode(pLineAngle->handle);
			Ta.Parts.DeleteCursor();
		}
	}
	this->RedrawAll();
	//m_pDoc->UpdateAllViews(NULL);
#ifdef AFX_TARG_ENU_ENGLISH
    MessageBox("Remove cleging bolts successfully!");
#else
	MessageBox("�ɹ�����Ŷ�!");
#endif
#endif
}

/*void CLDSView::OnFormatBrush()
{
	long *arrH;
	int n = g_pSolidSnap->GetLastSelectEnts(arrH);
	if(n>1)
	{
		AfxMessageBox("����ͬʱѡ�ж������!");
		return;
	}
	else if(n<1)
	{
		AfxMessageBox("û��ѡ���κι���!");
		return;
	}
	else
	{
		CLDSDbObject *pDbObject=console.FromHandle(arrH[0]);
		if(pDbObject->GetClassTypeId() == CLS_NODE)
		{
			CLDSNode *pBaseNode = (CLDSNode *)pDbObject;
			f3dPoint* pPoint;
			while (1)
			{
				int ret = g_pSolidSnap->SnapPoint(pPoint);
				if(ret<1)
					return;
				else
				{
					g_pSolidDraw->ReleaseSnapStatus();
					g_pSolidDraw->SetEntSnapStatus(pPoint->ID);
					CLDSNode* pNode=console.FromNodeHandle(pPoint->ID);
					if(g_sysPara.brush.bCfgword)
						pNode->cfgword=pBaseNode->cfgword;
					if(g_sysPara.brush.bLayer)
						pNode->SetLayer(pBaseNode->layer());
					if(g_sysPara.brush.bSeg)
						pNode->iSeg = pBaseNode->iSeg;
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
		}
		else if (pDbObject->IsPart())
		{
			CLDSLinePart *pBaseLinePart = (CLDSLinePart *)pDbObject;
			f3dLine line;
			while (1)
			{
				int ret = g_pSolidSnap->SnapLine(line);
				if(ret<1)
					return;
				else
				{
					g_pSolidDraw->ReleaseSnapStatus();
					g_pSolidDraw->SetEntSnapStatus(line.ID);
					CLDSLinePart* pLinePart=(CLDSLinePart*)console.FromPartHandle(line.ID,CLS_LINEPART);
					if(g_sysPara.brush.bCfgword)
						pLinePart->cfgword=pBaseLinePart->cfgword;
					if(g_sysPara.brush.bLayer)
						pLinePart->SetLayer(pBaseLinePart->layer());
					if(g_sysPara.brush.bSeg)
						pLinePart->iSeg=pBaseLinePart->iSeg;
					if(g_sysPara.brush.bGuige)
					{
						pLinePart->size.wide=pBaseLinePart->size.wide;
						pLinePart->size.thick=pBaseLinePart->size.thick;
					}
					if(g_sysPara.brush.bMaterial)
						pLinePart->cMaterial = pBaseLinePart->cMaterial;
					if(g_sysPara.brush.bColour)
						pLinePart->crMaterial = pBaseLinePart->crMaterial;
					if(g_sysPara.brush.bVirtualPart)
						pLinePart->m_bVirtualPart = pBaseLinePart->m_bVirtualPart;
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
		}
	}
}
*/
//ͨ��ʵ�������������Ҫ��һЩ��ͼ�е�����һ���������Ӹ˼����ݲ���ȥ����wjh-2015.5.17
void CLDSView::OnInsertJgnodeToView() 
{
	CFindPartDlg dlg;
	dlg.m_bDisplayPropertyVisible = FALSE;
	if(dlg.DoModal()==IDOK)
	{
		CDisplayView *pDispView=console.GetActiveView();
		CLDSNode* pNode = console.FromNodeHandle(dlg.m_nPartHandle);
		if(pNode)
		{
			if(console.DispNodeSet.FromHandle(pNode->handle)==NULL)
			{
				console.DispNodeSet.append(pNode);
				if(pDispView->m_iViewType==1)
					pDispView->AppendViewPart(CViewPart(pNode->handle,CLS_NODE));
			}
		}
		else
		{
			CLDSPart* pPart = console.FromPartHandle(dlg.m_nPartHandle);
			if(pPart)
			{
				if(console.DispPartSet.FromHandle(pPart->handle)==NULL)
				{
					console.DispPartSet.append(pPart);
					if(pDispView->m_iViewType==1)
						pDispView->AppendViewPart(CViewPart(pPart->handle,pPart->GetClassTypeId()));
				}
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Didn't find a right type part!");
#else 
				AfxMessageBox("δ���ҵ��������͵Ĺ���!");
#endif
				return;
			}
		}
		RedrawAll();
	}
	
}

void CLDSView::OnRemoveObjsFromView() 
{
	CFindPartDlg dlg;
	dlg.m_bDisplayPropertyVisible = FALSE;
	long *id_arr=NULL;
	long n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	CDisplayView *pDisplayView=console.GetActiveView();
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
	CSectionView *pSectionView=NULL;
	if(pDisplayView->m_iViewType==2)
	{
		for(CLDSPartGroup *pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
		{
			if(stricmp(pPartGroup->sGroupName,pDisplayView->folderName)!=0)
				continue;
			for(CSectionView *pSectView=pPartGroup->SonViews.GetFirst();pSectView;pSectView=pPartGroup->SonViews.GetNext())
			{
				if((DWORD)pSectView==(DWORD)pDisplayView->feature)
				{
					pSectionView=pSectView;
					break;
				}
			}
			if(pSectionView!=NULL)
				break;
		}
	}
#endif
	IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
	for(int i=0;i<n;i++)
	{
		CLDSObject* pObj = console.FromHandle(id_arr[i]);
		if(pObj==NULL)
			continue;
		if(pObj->GetClassTypeId()==CLS_NODE)
			console.DispNodeSet.DeleteNode(id_arr[i]);
		else
			console.DispPartSet.DeleteNode(id_arr[i]);
		BOOL bRet=TRUE;
		while(bRet) //ɾ������IDΪid_arr[i]��ʵ��
			bRet=g_pSolidDraw->DelEnt(id_arr[i]);
		HOLE_WALL* pHoleWall=NULL;
		if(pObj&&pObj->GetClassTypeId()==CLS_BOLT&&(pHoleWall=console.GetHoleWall((CLDSBolt*)pObj))!=NULL)
		{
			pHoleWall->Create3dSolidModel();//g_pSolidOper->GetScaleUserToScreen(),g_pSolidOper->GetScaleUserToScreen(),g_sysPara.display.nSmoothness);
			pHoleWall->is_visible=true;
			g_pSolidDraw->NewSolidPart(pHoleWall->GetSolidPartObject());
		}

		if(pBuddyDrawing && pObj->GetClassTypeId()==CLS_LINEANGLE)
			pBuddyDrawing->DeleteDbEntityByHiberMasterId(pObj->handle);
		if(pDisplayView->m_iViewType!=0)	//������ͼ
		{
			for(CViewPart *pViewPart=pDisplayView->GetFirstViewPart();pViewPart;pViewPart=pDisplayView->GetNextViewPart())
			{
				if(pViewPart->handle==id_arr[i])
				{
					pDisplayView->DeletCursorViewPart(TRUE);
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
					if(pSectionView)
					{
						for(CSectionView::PART_ITEM *pItem=pSectionView->SonParts.GetFirst();pItem;pItem=pSectionView->SonParts.GetNext())
						{
							if(pItem->hSonPart==id_arr[i])
							{
								pSectionView->SonParts.DeleteCursor();
								break;
							}
						}
					}
#endif
					break;
				}
			}
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
			if(pSectionView)
				pSectionView->SonParts.Clean();
#endif
		}
	}
	g_pSolidDraw->Draw();
}

BOOL CLDSView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	ZOOM_CENTER_STYLE zoom_style;
	g_pSolidSet->GetZoomStyle(&zoom_style);
	g_pSolidSet->SetZoomStyle(MOUSE_CENTER);
	if(zDelta>0)
		g_pSolidOper->Scale(1.6);
	else
		g_pSolidOper->Scale(1/1.6);
	g_pSolidOper->RefreshScope();
	g_pSolidSet->SetZoomStyle(zoom_style);
	
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CLDSView::OnSetNcDriver() 
{
	CSetNcDrvDlg dlg;
	dlg.m_sJgNcDrv = theApp.env.m_sJgNcDrvFile;
	dlg.m_sOutPutPath = theApp.env.m_sWorkDir;
	dlg.m_sPartNoPrefix = theApp.env.m_sPartNoPrefix;
	dlg.m_iNcGroupType = theApp.env.m_iNcGroupType;
	if(dlg.DoModal()==IDOK)
	{
		theApp.env.m_sJgNcDrvFile = dlg.m_sJgNcDrv;
		theApp.env.m_sWorkDir = dlg.m_sOutPutPath;
		theApp.env.m_sPartNoPrefix = dlg.m_sPartNoPrefix;
		theApp.env.m_iNcGroupType = dlg.m_iNcGroupType;
		AfxGetApp()->WriteProfileString("Settings","JgNcDrvFile",theApp.env.m_sJgNcDrvFile);
		AfxGetApp()->WriteProfileString("Settings","WorkDirectory",theApp.env.m_sWorkDir);
		AfxGetApp()->WriteProfileInt("Settings","NcGroupType",theApp.env.m_iNcGroupType);
	}
}

void CLDSView::OnGenPartProcessInfoFile() 
{
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
	m_nPrevCommandID=ID_GEN_PART_PROCESSINFO_FILE;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat generating angle neutral file";
#else
	m_sPrevCommandName="�ظ����ɽǸ������ļ�";
#endif
	//1.����ѡ�й����еĽǸֹ���(�����Ǹֺ�����Ǹ�)�Ͱ幹��
	long *id_arr=NULL;
	int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	CSuperSmartPtr<CLDSPart> pPart;
	PARTSET partset;
	POLYPARTLIST polylist;
	Ta.RetrievePolyPartList(polylist);
	for(int i=0;i<n;i++)
	{
		pPart=console.FromPartHandle(id_arr[i]);
		if(pPart.IsNULL())
			continue;
		if(pPart->m_bVirtualPart)
			continue;
		else if(pPart->IsPlate())	//�ְ�
			partset.append(pPart);
		else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{	//�Ǹ�	
			if(!pPart.LineAnglePointer()->bHuoQuStart&&!pPart.LineAnglePointer()->bHuoQuEnd)
				partset.append(pPart);
			else	//�����ӽǸ�
			{
				for(CLDSPolyPart *pPolyPart=polylist.GetFirst();pPolyPart;pPolyPart=polylist.GetNext())
				{
					if(pPart.IsEqualPtr(pPolyPart->segset.GetFirst()))	//�׻����ӽǸ���Ϊ�����Ǹֽ���ͳ��
					{
						partset.append(pPolyPart);
						break;
					}
				}
			}
		}
	}
	if(partset.GetNodeNum()<=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Usable angles or plates not chosen, please choose again and then regenerate into neutral file!");
#else
		AfxMessageBox("δѡ����ýǸֻ�ְ壬������ѡ��������������ļ�!");
#endif
		return;
	}
	//2.���ɹ�����Ϣ�����ļ�
	//2.1 ��ȡ�����ļ����·��
	char sFolderPath[MAX_PATH];
	strcpy(sFolderPath,theApp.env.m_sWorkDir);
	if(strlen(sFolderPath)<=0)
		strcpy(sFolderPath,"C:\\");
	CString title = m_pDoc->GetTitle();
	title = title.SpanExcluding(".");
	if(sFolderPath[strlen(sFolderPath)-1]=='\\'||sFolderPath[strlen(sFolderPath)-1]=='/')
		strcat(sFolderPath,title);
	else
	{
		strcat(sFolderPath,"\\");
		strcat(sFolderPath,title);
	}
	strcat(sFolderPath,"\\");
	MakeDirectory(sFolderPath);
	//2.2 ���������ļ�
	//char sFilePath[MAX_PATH]="";
	for(pPart=partset.GetFirst();pPart.IsHasPtr();pPart=partset.GetNext())
	{
		CBuffer buffer;
		pPart->ToPPIBuffer(buffer);
		FILE *fp;
		CXhChar100 sSpec;
		pPart->GetSpecification(sSpec,FALSE);
		CString sSpecIncMat;
		sSpecIncMat.Format("%s%c",(char*)sSpec,pPart->cMaterial);
		CXhChar200 filename("%s\\%s",sFolderPath,theApp.env.m_sPartNoPrefix);
		if(g_sysPara.m_nMatCharPosType==1)//���ǰ��Ӳ����ַ�
			filename.Append(pPart->cMaterial);
		filename.Append(pPart->GetPartNo());
		if(g_sysPara.m_nMatCharPosType==2)//��ź���Ӳ����ַ�
			filename.Append(pPart->cMaterial);
		filename.Append(CXhChar100("#%s.ppi",sSpecIncMat.Trim()));
		//sprintf(sFilePath,"%s#%s.ppi",sFolderPath,theApp.env.m_sPartNoPrefix,(char*)pPart->GetPartNo(),sSpecIncMat.Trim());
		if((fp=fopen(filename,"wb"))==NULL)
			continue;
		long file_len=buffer.GetLength();
		fwrite(&file_len,sizeof(long),1,fp);
		fwrite(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
		fclose(fp);
	}
	CString ss;
#ifdef AFX_TARG_ENU_ENGLISH
    ss.Format("component process information nertral file has been generated into {%s}!",sFolderPath);
#else
	ss.Format("����������Ϣ�����ļ������ɵ�{%s}��!",sFolderPath);
#endif
	if (MsgBox(GetSafeHwnd(), ss, "�����ļ�", MB_OKCANCEL) == IDOK)
	{	//���ļ���
		ShellExecute(NULL, "open", NULL, NULL, sFolderPath, SW_SHOW);
	}
#endif
}

typedef struct tagWIDTH_RANGE{
	static const BYTE TYPE_EQUAL	= 0;	//ָ��֫��,��:40
	static const BYTE TYPE_RANGE	= 1;	//ָ����Χ,��:40~125
	static const BYTE TYPE_PLUS		= 2;	//���ڵ���ָ��֫��,��:125+
	static const BYTE TYPE_SUBTRACT	= 3;	//С�ڵ���ָ��֫��,��:125-
	BYTE cType;	//0.ָ��֫�� 1.ָ��֫��Χ 2.���ڵ���ָ��֫�� 3.С�ڵ���ָ��֫��
	long nMinWidth;
	long nMaxWidth;
	tagWIDTH_RANGE(){cType=0;nMinWidth=0xFFFFFFFF;nMaxWidth=0xFFFFFFFF;}
	bool InRange(long width){
		if((cType==TYPE_EQUAL&&nMinWidth==width)||
		   (cType==TYPE_RANGE&&width>=nMinWidth&&width<=nMaxWidth)||
		   (cType==TYPE_PLUS&&width>=nMaxWidth)||
		   (cType==TYPE_SUBTRACT&&width<=nMinWidth))
			return true;
		else
			return false;
	}
	CXhChar200 GetRangeFolderName(){
		CXhChar200 sFolder;
		if(cType==TYPE_EQUAL)
			sFolder.Printf("%d",nMinWidth);
		else if(cType==TYPE_RANGE)
			sFolder.Printf("%d-%d",nMinWidth,nMaxWidth);
#ifdef AFX_TARG_ENU_ENGLISH
		else if(cType==TYPE_PLUS)
			sFolder.Printf("GreaterThanOrEqualTo%d",nMaxWidth);
		else if(cType==TYPE_SUBTRACT)
			sFolder.Printf("LessThanOrEqualTo%d",nMinWidth);
#else
		else if(cType==TYPE_PLUS)
			sFolder.Printf("���ڵ���%d",nMaxWidth);
		else if(cType==TYPE_SUBTRACT)
			sFolder.Printf("С�ڵ���%d",nMinWidth);
#endif
		return sFolder;
	}
}WIDTH_RANGE;
bool ParseWidthRangeStr(const char* sWidthRange,ARRAY_LIST<WIDTH_RANGE> &widthRangeList)
{
	widthRangeList.Empty();
	if(sWidthRange==NULL)
		return false;
	int nLen=strlen(sWidthRange);
	if(nLen<=0)
		return false;
	CXhChar500 sRange(sWidthRange);
	char *sKey=strtok(sRange,",��");
	while(sKey)
	{
		bool bQuanJiaoChar=false;
		char *delimiter=strchr(sKey,'~');
		if(delimiter==NULL)
		{
			delimiter=strchr(sKey,'��');
			bQuanJiaoChar=true;
		}
		char *flagPlus=strchr(sKey,'+');
		char *flagSubtract=strchr(sKey,'-');
		WIDTH_RANGE *pRange=widthRangeList.append();
		if(delimiter)
		{
			if(bQuanJiaoChar)	//ȫ�Ƿָ���
				*(delimiter-1)=' ';
			*delimiter=' ';
			pRange->cType=WIDTH_RANGE::TYPE_RANGE;
			sscanf(sKey,"%d%d",&pRange->nMinWidth,&pRange->nMaxWidth);
		}
		else if(flagPlus)
		{
			*flagPlus=' ';
			pRange->cType=WIDTH_RANGE::TYPE_PLUS;
			pRange->nMinWidth=pRange->nMaxWidth=atoi(sKey);
		}
		else if(flagSubtract)
		{
			*flagSubtract=' ';
			pRange->cType=WIDTH_RANGE::TYPE_SUBTRACT;
			pRange->nMinWidth=pRange->nMaxWidth=atoi(sKey);
		}
		else
		{
			pRange->cType=WIDTH_RANGE::TYPE_EQUAL;
			pRange->nMinWidth=pRange->nMaxWidth=atoi(sKey);
		}
		sKey=strtok(NULL,",��");
	}
	return true;
}
void Test_ParseWidthRangeStr()
{
	CLogErrorLife logLife;
	ARRAY_LIST<WIDTH_RANGE> widthRangeList;
	ParseWidthRangeStr("0+",widthRangeList);
	WIDTH_RANGE *pRange=widthRangeList.GetFirst();
	if(pRange->cType!=WIDTH_RANGE::TYPE_PLUS||pRange->nMaxWidth!=0)
		logerr.Log("0+��������");
	//
	ParseWidthRangeStr("125-",widthRangeList);
	pRange=widthRangeList.GetFirst();
	if(pRange->cType!=WIDTH_RANGE::TYPE_SUBTRACT||pRange->nMinWidth!=125)
		logerr.Log("125-��������");
	//
	ParseWidthRangeStr("125",widthRangeList);
	pRange=widthRangeList.GetFirst();
	if(pRange->cType!=WIDTH_RANGE::TYPE_EQUAL||pRange->nMinWidth!=125)
		logerr.Log("125��������");
	//
	ParseWidthRangeStr("70��125",widthRangeList);
	pRange=widthRangeList.GetFirst();
	if(pRange->cType!=WIDTH_RANGE::TYPE_RANGE||pRange->nMinWidth!=70||pRange->nMaxWidth!=125)
		logerr.Log("70��125��������");
	//
	ParseWidthRangeStr("70~125",widthRangeList);
	pRange=widthRangeList.GetFirst();
	if(pRange->cType!=WIDTH_RANGE::TYPE_RANGE||pRange->nMinWidth!=70||pRange->nMaxWidth!=125)
		logerr.Log("70~125��������");
	//
	ParseWidthRangeStr("0��40,70~100,110,125+",widthRangeList);
	pRange=widthRangeList.GetFirst();
	if(pRange->cType!=WIDTH_RANGE::TYPE_RANGE||pRange->nMinWidth!=0||pRange->nMaxWidth!=40)
		logerr.Log("0��40,70~100,110,125+��������");
	pRange=widthRangeList.GetNext();
	if(pRange->cType!=WIDTH_RANGE::TYPE_RANGE||pRange->nMinWidth!=70||pRange->nMaxWidth!=100)
		logerr.Log("0��40,70~100,110,125+��������");
	pRange=widthRangeList.GetNext();
	if(pRange->cType!=WIDTH_RANGE::TYPE_EQUAL||pRange->nMinWidth!=110||pRange->nMaxWidth!=110)
		logerr.Log("0��40,70~100,110,125+��������");
	pRange=widthRangeList.GetNext();
	if(pRange->cType!=WIDTH_RANGE::TYPE_PLUS||pRange->nMaxWidth!=125)
		logerr.Log("0��40,70~100,110,125+��������");
}

void CLDSView::OnGenJgNcFile() 
{
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
	m_nPrevCommandID=ID_GEN_JG_NC_FILE;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat generate NC data of angle steel";
#else
	m_sPrevCommandName="�ظ����ɽǸ�NC����";
#endif
	CNcJg NcJg;
	int n;
	long *id_arr=NULL;
	n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	char DRV_PATH[MAX_PATH],fpath[MAX_PATH],bak_fpath[MAX_PATH];
	sprintf(DRV_PATH,"%s",theApp.env.m_sJgNcDrvFile);
	//1.��ʾ�û�����֫���ٽ�ֵ,����������ַ�����ȡ�Ǹ��ٽ�֫���б�
	static CInputAnStringValDlg MinWingWideDlg;
	MinWingWideDlg.m_sItemValue="0+";
#ifdef AFX_TARG_ENU_ENGLISH
    MinWingWideDlg.m_sItemTitle="range of angle leg width:";
	MinWingWideDlg.m_sTip="40+:width��40\r\n40-:width��40\r\n40 :width=40\r\n40~100:40��width��100\r\n connect multiple ranges with a comma";
#else
	if(!NcJg.NcManager.InitJgNcDriver(DRV_PATH))
		return;	//��ʼ��ʧ��,Ӣ�İ�NC��������Ҫ��ʼ���������� wjh-2015.7.23
	MinWingWideDlg.m_sTip="40+:֫���40\r\n40-:֫���40\r\n40 :֫��=40\r\n40~100:40��֫���100\r\n�ö������Ӷ����ȷ�Χ";
	MinWingWideDlg.m_sItemTitle="�Ǹ�֫��Χ;";
#endif
	if(MinWingWideDlg.DoModal()!=IDOK)
		return;
	CLogErrorLife loglife(&logerr);
	CXhChar500 sWidthRange("%s",MinWingWideDlg.m_sItemValue);
	ARRAY_LIST<WIDTH_RANGE> widthRangeList;
	ParseWidthRangeStr(sWidthRange,widthRangeList);
	//2.��ȡ��ǰ�ļ�����
	strcpy(fpath,theApp.env.m_sWorkDir);
	CString title = m_pDoc->GetTitle();
	title = title.SpanExcluding(".");
	title.Remove(' ');	//���ļ��������Ƴ��ո� wht 18-12-29
	if(fpath[strlen(fpath)-1]=='\\'||fpath[strlen(fpath)-1]=='/')
		strcat(fpath,title);
	else
	{
		strcat(fpath,"\\");
		strcat(fpath,title);
	}
	strcat(fpath,"\\");
	//3.��ȡѡ�е����нǸ�
	CSuperSmartPtr<CLDSPart> pPart;
	PARTSET partset,angleSet;
	POLYPARTLIST polylist;
	Ta.RetrievePolyPartList(polylist);
	int i;
	CHashSet<CLDSPart*>hashAddPolyParts;
	for(i=0;i<n;i++)
	{
		pPart=console.FromPartHandle(id_arr[i]);
		if(pPart.IsNULL()|| pPart->m_bVirtualPart)
			continue;
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			if(!pPart.LineAnglePointer()->bHuoQuStart&&!pPart.LineAnglePointer()->bHuoQuEnd)
				angleSet.append(pPart);
			else	//�����ӽǸ�
			{
				for(CLDSPolyPart *pPolyPart=polylist.GetFirst();pPolyPart;pPolyPart=polylist.GetNext())
				{
					CLDSPart* pSegPart,*pHeadSegPart = pPolyPart->segset.GetFirst();
					for(pSegPart= pHeadSegPart;pSegPart!=NULL;pSegPart=pPolyPart->segset.GetNext())
					{
						if (pPart.IsEqualPtr(pSegPart))	//�׻����ӽǸ���Ϊ�����Ǹֽ���ͳ��
						{
							if (hashAddPolyParts.GetValue(pHeadSegPart->handle) == NULL)
							{
								angleSet.append(pPolyPart);
								hashAddPolyParts.SetValue(pHeadSegPart->handle, pHeadSegPart);
							}
							break;
						}
					}
				}
			}
		}
	}
	//4.��������ĽǸ��ٽ�ֵ���ɽǸ�NC����(��ʼֵ��֫����ֵֹ)
	char sOrgPath[MAX_PATH];
	strcpy(sOrgPath,fpath);
	for(WIDTH_RANGE *pRange=widthRangeList.GetFirst();pRange;pRange=widthRangeList.GetNext())
	{
		strcpy(fpath,sOrgPath);
		CXhChar200 sFolder=pRange->GetRangeFolderName();
		if(sFolder.GetLength()>0)
		{
			strcat(fpath,sFolder);
			strcat(fpath,"\\");
		}
		partset.Empty();
		for(pPart=angleSet.GetFirst();pPart.IsHasPtr();pPart=angleSet.GetNext())
		{
			if(!pRange->InRange((int)pPart->GetWidth()))
				continue;
			partset.append(pPart);
		}
		MakeDirectory(fpath);
		strcpy(bak_fpath,fpath);
		FILE *dir_fp=NULL;
		for(CNCDirectoryFile *pDirFile=NcJg.NcManager.directory.GetFirst();pDirFile;pDirFile=NcJg.NcManager.directory.GetNext())
		{	//��Ա�����ˮ��ѹ������������,�������ļ��⻹��һ��Ŀ¼�ļ�,�˴������������ɴ���Ŀ¼�ļ�
			if(pDirFile->contents.GetNodeNum()>0)
			{	//��Ҫ����Ŀ¼
				strcat(fpath,pDirFile->DIR_FILE_NAME);
				if(pDirFile->m_bDirectoryPrintByASCII)
					dir_fp=fopen(fpath,"wt");
				else
					dir_fp=fopen(fpath,"wb");
				if(dir_fp)
				{
					BOOL bak_format=NcJg.NcManager.m_bPrintByASCII;
					NcJg.NcManager.m_bPrintByASCII = pDirFile->m_bDirectoryPrintByASCII;
					strncpy(NcJg.sPrefix,theApp.env.m_sPartNoPrefix,19);
					CHashStrTable<long>partnosTable;
					partnosTable.CreateHashTable(partset.GetNodeNum());
					for(pPart=partset.GetFirst();pPart.IsHasPtr();pPart=partset.GetNext())
					{
						NcJg.InitJg(pPart);
						long key=0;
						if(partnosTable.GetValueAt(pPart->GetPartNo(),key))
							continue;	//�����ɹ�
						else
							partnosTable.SetValueAt(pPart->GetPartNo(),1);
						for(CVariant *pVar=pDirFile->contents.GetFirst();pVar;pVar=pDirFile->contents.GetNext())
							NcJg.PrintLn(dir_fp,pVar->sVal);
					}
					NcJg.NcManager.m_bPrintByASCII = bak_format;
					fclose(dir_fp);
				}
			}
		}
		CXhChar200 filename;
		for(pPart=partset.GetFirst();pPart.IsHasPtr();pPart=partset.GetNext())
		{
			strcpy(fpath,bak_fpath);
			//����NC�ļ����鷽ʽ������ļ�·��
			CXhChar16 sMat,sSpec;
			QuerySteelMatMark(pPart->cMaterial,sMat);
			CLDSLineAngle *pAngle=NULL;
			if(pPart->IsPolyPart())
				pAngle=(CLDSLineAngle*)((CLDSPolyPart*)pPart.PartPointer())->segset.GetFirst();
			else
				pAngle=pPart.LineAnglePointer();
			pAngle->GetAngleSimpleSpec(sSpec);
			if(theApp.env.m_iNcGroupType==1||theApp.env.m_iNcGroupType==3)
			{
				strcat(fpath,sSpec);
				strcat(fpath,"\\");
			}
			if(theApp.env.m_iNcGroupType==2||theApp.env.m_iNcGroupType==3)
			{
				strcat(fpath,sMat);
				strcat(fpath,"\\");
			}
			if(theApp.env.m_iNcGroupType>0)
				MakeDirectory(fpath);
			//����ļ�����
			char sPrefix[21]={0};
			_snprintf(sPrefix,20,"%s",theApp.env.m_sPartNoPrefix);
			size_t nPrefixLen=strlen(sPrefix);
			if(nPrefixLen>0&&sPrefix[nPrefixLen-1]!='-'&&nPrefixLen<20)
				strcat(sPrefix,"-");
			strcat(fpath,sPrefix);
			filename.Copy(fpath);
			if(g_sysPara.m_nMatCharPosType==1)//���ǰ��Ӳ����ַ�
			{
				if(pPart->cMaterial!=CSteelMatLibrary::Q235BriefMark())
					filename.Append(pPart->cMaterial);
			}
			filename.Append(pPart->GetPartNo());
			if(g_sysPara.m_nMatCharPosType==2)//��ź���Ӳ����ַ�
			{
				if(pPart->cMaterial!=CSteelMatLibrary::Q235BriefMark())
					filename.Append(pPart->cMaterial);
			}
			NcJg.InitJg(pPart);
			NcJg.GenNCFile(filename,sPrefix);
		}
	}
	if(logerr.IsHasContents())
	{	//��ǰ�����ʾ��־����Ŀ
		logerr.ShowToScreen();
		loglife.EnableShowScreen(false);
	}
	CString ss;
#ifdef AFX_TARG_ENU_ENGLISH
    ss.Format("the NC data of angle has generated into %s!",sOrgPath);
#else
	ss.Format("�Ǹ�NC���������ɵ�%s��!",sOrgPath);
#endif
	if(MsgBox(GetSafeHwnd(),ss,"�Ǹ�NC����",MB_OKCANCEL)==IDOK)
	{	//���ļ���
		ShellExecute(NULL,"open",NULL,NULL,sOrgPath,SW_SHOW);
	}
#endif
}

void CLDSView::OnGenPlankNcData() 
{
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
	m_nPrevCommandID=ID_GEN_PLANK_NC_DATA;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat to generate NC data of steel plate";
#else
	m_sPrevCommandName="�ظ����ɰ�NC����";
#endif
	CLDSPlate *pPlate;
	CNcPlank NcPlank;
	int i,n;
	long *id_arr=NULL;
	n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	PARTSET plateSet;
	for(i=0;i<n;i++)
	{
		CLDSPart *pPart = console.FromPartHandle(id_arr[i]);
		if(pPart&&pPart->IsPlate()&&strlen(pPart->GetPartNo())>0)
			plateSet.append(pPart);
	}
	if(plateSet.GetNodeNum()<=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("usable plates not chosen, please select plates and then generate NC data!");
#else
		AfxMessageBox("δѡ����øְ壬��ѡ��ְ��������NC����!");
#endif
		return;
	}
	BOOL bExportDeformedProfile = FALSE;//���ǻ�������
#ifdef AFX_TARG_ENU_ENGLISH
	if (AfxMessageBox("Is bending deformation considered or not?", MB_YESNO) == IDYES)
#else
	if (AfxMessageBox("�Ƿ��ǻ�������?", MB_YESNO) == IDYES)
#endif
		bExportDeformedProfile = TRUE;
	char fpath[MAX_PATH];
	strcpy(fpath,theApp.env.m_sWorkDir);
	CString title = m_pDoc->GetTitle();
	title = title.SpanExcluding(".");
	title = title.Trim();
	if(fpath[strlen(fpath)-1]=='\\'||fpath[strlen(fpath)-1]=='/')
		strcat(fpath,title);
	else
	{
		strcat(fpath,"\\");
		strcat(fpath,title);
	}
	strcat(fpath,"\\");
	MakeDirectory(fpath);
	//
	char sOrgPath[MAX_PATH];
	strcpy(sOrgPath,fpath);
	FILE *dir_fp=NULL;
	CXhChar200 filename;
	for(pPlate=(CLDSPlate*)plateSet.GetFirst();pPlate;pPlate=(CLDSPlate*)plateSet.GetNext())
	{
		strcpy(filename,fpath);
		if(g_sysPara.m_nMatCharPosType==1)//���ǰ��Ӳ����ַ�
		{
			if(pPlate->cMaterial!=CSteelMatLibrary::Q235BriefMark())
				filename.Append(pPlate->cMaterial);
		}
		filename.Append(pPlate->GetPartNo());
		if(g_sysPara.m_nMatCharPosType==2)//��ź���Ӳ����ַ�
		{
			if(pPlate->cMaterial!=CSteelMatLibrary::Q235BriefMark())
				filename.Append(pPlate->cMaterial);
		}
		NcPlank.GenNCFile(pPlate,filename,bExportDeformedProfile);
	}
	CString ss;
#ifdef AFX_TARG_ENU_ENGLISH
    ss.Format("NC data of steel plate has generated inot %s!",(char*)sOrgPath);
#else
	ss.Format("��NC���������ɵ�%s��!",(char*)sOrgPath);
#endif
	if(MsgBox(GetSafeHwnd(),ss,"��NC����",MB_OKCANCEL)==IDOK)
	{	//���ļ���
		ShellExecute(NULL,"open",NULL,NULL,sOrgPath,SW_SHOW);
	}
#endif
}

void CLDSView::DelSpecPart(long h)
{
	CLDSObject *pObj=console.FromHandle(h);
	if(pObj==console.m_pWorkPart)
		console.m_pWorkPart=NULL;
	if(pObj==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("the part doesn't exist,delete failed!");
#else
		AfxMessageBox("�˹����Ѳ����ڣ�ɾ��ʧ�ܣ�");
#endif
		return;
	}
	if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pObj->dwPermission))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("No permission to modify the part!");
#else
		AfxMessageBox("û����Ӧ�������޸�Ȩ�ޣ�");
#endif
		return;
	}
	if(pObj->GetClassTypeId()==CLS_NODE)
	{
		console.DispNodeSet.DeleteNode(h);
		console.DeleteNode(h);
	}
	if(pObj->GetClassTypeId()==CLS_LINETUBE||pObj->GetClassTypeId()==CLS_LINEANGLE||
		pObj->GetClassTypeId()==CLS_LINESLOT||pObj->GetClassTypeId()==CLS_LINEFLAT||
		pObj->GetClassTypeId()==CLS_PLATE||pObj->GetClassTypeId()==CLS_ARCANGLE||
		pObj->GetClassTypeId()==CLS_ARCSLOT||pObj->GetClassTypeId()==CLS_ARCFLAT)
	{
		console.DispPartSet.DeleteNode(h);
		console.DeletePart(h);
	}
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(pObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pObj;
		console.DeletePart(pGroupJg->son_jg_h[0]);
		console.DeletePart(pGroupJg->son_jg_h[1]);
		console.DeletePart(pGroupJg->son_jg_h[2]);
		console.DeletePart(pGroupJg->son_jg_h[3]);
		console.DeletePart(pGroupJg->handle);
	}
#endif
	else if(pObj->GetClassTypeId()==CLS_POLYJOINT)
		console.DeletePart(h);
	else if(pObj->GetClassTypeId()==CLS_GEPLANE)
		Ta.Plane.DeleteNode(h);
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(pObj->GetClassTypeId()==CLS_JOINT)
	{
		CJoint *pJoint=(CJoint*)pObj;
		CLDSPart *pPart;
		for(pPart=pJoint->GetFirstPart();pPart;pPart=pJoint->GetNextPart())
		{
			if(pPart->GetClassTypeId()==CLS_LINEANGLE)
				console.DeletePart(pPart->handle);
		}
		for(pPart=pJoint->GetFirstPart();pPart;pPart=pJoint->GetNextPart())
			console.DeletePart(pPart->handle);
		console.DeletePart(h);
	}
	else if(pObj->GetClassTypeId()==CLS_BOLT)
		Ta.DelBoltAndRelativeBoltRef(h);
	else if(pObj->GetClassTypeId()==CLS_PARAMPLATE)
		Ta.DelTubeEndLj(h);	//ɾ���ֹܶ�����
	else if(Ta.SuppPart.FromHandle(h))
		Ta.SuppPart.DeleteNode(h);
#endif
	else
#ifdef AFX_TARG_ENU_ENGLISH
	    AfxMessageBox("the specified part is not found!");
	RedrawAll();
	MessageBox("the specified part is deleted!");
#else
		AfxMessageBox("û���ҵ�ָ������!");
	RedrawAll();
	MessageBox("ָ�������ѱ�ɾ��!");
#endif
}
void CLDSView::NotifyObjStateModified(long* id_arr,long count/*=1*/,UINT message/*=1*/)
{
	for(long i=0;i<count;i++)
	{
		selectObjs.DeleteNode(id_arr[i]);
		m_setSnapObjects.DeleteNode(id_arr[i]);
	}
}

void CLDSView::OnDefZhuJgPoly() 
{
	m_nPrevCommandID=ID_DEF_ZHU_JG_POLY;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat to normalize 4-leg structure";
#else
	m_sPrevCommandName="�ظ����������ı���";
#endif
	m_curTask=TASK_DEF_POLY_STYLE;
	//ֻ����Ƥ�����ͼԪ
	g_pSolidSet->SetCreateEnt(FALSE);
	g_pSolidSet->SetDrawType(DRAW_LINE);
	m_curTask = TASK_DEF_POLY_STYLE;
}
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
#include "IntelliAgent/IntelliAgent.h"
static BOOL IntelliRetrieve4PostAngles(const double* vPlaneNorm,const double* vPlanePick,ARRAY_LIST<MAIN_ANGLE>& angles,MAIN_ANGLE* pAngle4Arr)
{
	int i;
	GEPOINT vNorm(vPlaneNorm),vPick(vPlanePick);
	if(angles.GetSize()<4)
		return FALSE;
	for(i=0;i<angles.GetSize();i++)
	{
		if(Int3dlf(angles[i].vertex,f3dLine(angles[i].Start(),angles[i].End()),vPick,vPlaneNorm)!=1)
			return FALSE;
	}
	//�������´��룬�����Զ�ʶ�������νǵ㣨ÿ���ǵ����һ���Ǹ֣���
	//1�����ֲ�����ϵ����X-Yƽ����ѡ�����½ǵĵ�Ϊ��ʼ����̽���
	GECS cs;
	cs.origin=angles[0].vertex;
	cs.axis_x=angles[1].vertex-cs.origin;
	cs.axis_z=GEPOINT(vPlaneNorm);//������CalFaceNorm����Ϊ��������ǰ�ĸ��Ǹֽ���ܿ�����ͬһƽ���ϡ�wjh-2015.12.31
	if(cs.axis_z.IsZero())
		return FALSE;
	normalize(cs.axis_x);
	normalize(cs.axis_z);
	cs.axis_y=cs.axis_z^cs.axis_x;
	DYN_ARRAY<GEPOINT> localVertexArr(angles.GetSize());
	int currCornerI=0;
	for (i=0;i<angles.GetSize();i++)
	{	//�����½Ƕ���Ϊ��һ���ǵ�
		localVertexArr[i]=cs.TransPToCS(angles[i].vertex);
		if(i>0&&localVertexArr[currCornerI].x-localVertexArr[i].x>eps)
			currCornerI=i;
		else if(i>0&&fabs(localVertexArr[currCornerI].x-localVertexArr[i].x)<eps
			&&(localVertexArr[currCornerI].y-localVertexArr[i].y>eps))
			currCornerI=i;
	}
	//2.ȥ��������������ϵ��м乲�ߵ�
	CHashList<int> hashCornerAnglesI;
	hashCornerAnglesI.SetValue(currCornerI+1,currCornerI);
	GEPOINT *pStartVertex=&localVertexArr[currCornerI],*pEndVertex=NULL;

	while(true)
	{	//���Ѱ�ҵ�localVertexArr.Size()���Ƕ���Ǹ�
		BOOL bFindCornerAngle=TRUE;
		for (i=0;i<(int)localVertexArr.Size();i++)
		{	//̽���i�����Ƿ�Ϊ�Ƕ���
			if(i==currCornerI)
				continue;
			pEndVertex = &localVertexArr[i];
			f3dPoint line_vec(pEndVertex->x-pStartVertex->x,pEndVertex->y-pStartVertex->y);
			normalize(line_vec);
			bFindCornerAngle=TRUE;
			for (UINT j=1;j<localVertexArr.Size();j++)
			{
				GEPOINT *pVertex=&localVertexArr[(i+j)%localVertexArr.Size()];
				if(pVertex==pStartVertex||pVertex==pEndVertex)
					continue;
				double dd=DistOf2dPtLine(f2dPoint(pVertex->x,pVertex->y),
					f2dPoint(pStartVertex->x,pStartVertex->y),f2dPoint(pEndVertex->x,pEndVertex->y));
				if(fabs(dd)<=10)	//������EPS��Ϊ�ݲ��Ϊ�����TTA�����ļ���Ϊ���ܱ�֤�ϸ�������һ����� wjh-2017.5.4
				{//�������ߵĵ�
					f3dPoint vec2start(pVertex->x-pStartVertex->x,pVertex->y-pStartVertex->y);
					f3dPoint vec2end(pVertex->x-pEndVertex->x,pVertex->y-pEndVertex->y);
					if(line_vec*vec2start>0&&line_vec*vec2end>0)
					{	//���ڶ����뵱ǰ�㹲�ߣ�������������
						bFindCornerAngle=FALSE;
						break;
					}
				}
				else if(dd<-eps)  //�����ߵ����
				{
					bFindCornerAngle=FALSE;
					break;
				}
			}
			if(bFindCornerAngle)
			{
				currCornerI=i;
				break;
			}
		}
		if(!bFindCornerAngle||hashCornerAnglesI.GetValue(currCornerI+1)!=NULL)
			break;	//���Ҳ�����Ч��͹�ǵ���ҵ���֮ǰ�ĽǶ���(������ѷ��)
		else
		{
			hashCornerAnglesI.SetValue(currCornerI+1,currCornerI);
			pStartVertex=pEndVertex;
		}
	}
	//3.���ʣ��ǵ�������4����FALSE
	if(hashCornerAnglesI.GetNodeNum()!=4)
		return FALSE;
	else if(pAngle4Arr!=NULL)
	{	//4.����ǵ���Ϊ4�����4���ǵ�����Ӧ�ĽǸ�(MAIN_ANGLE)��ֵ��pAngle4Arr��
		pAngle4Arr[0]=angles[*hashCornerAnglesI.GetFirst()];
		for (i=1;i<4;i++)
			pAngle4Arr[i]=angles[*hashCornerAnglesI.GetNext()];
	}
	return TRUE;
}
BOOL CLDSView::FinishDefPoly(f3dPoint *vertex_arr,int nCount)
{
	static CPolyStyleDlg dlg;
	long *id_arr,nSelect=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(nSelect<4)
		return FALSE;
	if(dlg.DoModal()!=IDOK)
		return FALSE;
	CLogErrorLife life;
	MAIN_ANGLE angleArr[4],actualAngleArr[4];
	int nHits=0;
	ARRAY_LIST<MAIN_ANGLE> angles(0,4);
	for(int i=0;i<nSelect;i++)
	{
		CLDSLineAngle *pAngle=(CLDSLineAngle*)console.FromPartHandle(id_arr[i],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pAngle==NULL)
			continue;
		if(pAngle->Layer(1)=='Z')
		{
			if(nHits<4)
				angleArr[nHits].pAngle=pAngle;
			nHits++;
		}
		MAIN_ANGLE* pPostAngle=angles.append();
		pPostAngle->pAngle=pAngle;
	}
	//1.������淨�߼�ԭ�㣬�����������ϵ
	f3dPoint face_norm=(vertex_arr[1]-vertex_arr[0])^(vertex_arr[2]-vertex_arr[0]);
	normalize(face_norm);
	if(!IntelliRetrieve4PostAngles(face_norm,vertex_arr[0],angles,actualAngleArr))
	{	//�������ضϽǸּ����������������ǣ�����������ɸ��б���Ľ�����
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The count of leg angles is not 4��data errors!");
#else
		AfxMessageBox("δ����ȡ���ĸ�ǡ�������ĽǸ֣����ݴ���!");
#endif
		return FALSE;
	}
	if(nHits==4)
	{	//nHits!=4ʱ������������м��㴦�����
#ifndef AFX_TARG_ENU_ENGLISH
		for(i=0;i<4;i++)
		{
			for(int j=0;j<4;j++)
			{
				if(angleArr[i].pAngle==actualAngleArr[j].pAngle)
					break;
			}
			if(j==4)
			{
				logerr.Log("ϵͳ�Զ���ȡ�����ı��ν���Ľ����Ǹֿ���δ������ģ���������������ĸ���ǡ��Ϊ�ĸ��������ı��μ����������!");
				break;
			}
		}
#endif
		for(i=0;i<4;i++)
			Int3dlf(angleArr[i].vertex,f3dLine(angleArr[i].Start(),angleArr[i].End()),vertex_arr[0],face_norm);
	}
	else
		memcpy(angleArr,actualAngleArr,sizeof(MAIN_ANGLE)*4);

	UCS_STRU ucs;
	ucs.origin = (angleArr[0].vertex+angleArr[1].vertex+angleArr[2].vertex+angleArr[3].vertex)*0.25;
	ucs.axis_z=face_norm;
	if(dlg.m_iPolyStyle==1||dlg.m_iPolyStyle==2)
	{
		CModifyVertexDlg norm_dlg;
#ifdef AFX_TARG_ENU_ENGLISH
		norm_dlg.m_sTitle="input view direction";
#else
		norm_dlg.m_sTitle="��������ʸ��";
#endif
		norm_dlg.m_fVertexPosX=0;
		norm_dlg.m_fVertexPosY=-1.0;
		norm_dlg.m_fVertexPosZ=0;
		if(norm_dlg.DoModal()==IDOK)
			ucs.axis_y.Set(norm_dlg.m_fVertexPosX,norm_dlg.m_fVertexPosY,norm_dlg.m_fVertexPosZ);
		else
			return FALSE;
	}
	else
		ucs.axis_y=inters_vec(face_norm);
	ucs.axis_x=ucs.axis_y^face_norm;
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	normalize(ucs.axis_x);
	normalize(ucs.axis_y);
	//2.��4�����İ���ʱ�뷽������
	for(i=0;i<4;i++)
	{
		f3dPoint vertex=angleArr[i].vertex;
		coord_trans(vertex,ucs,FALSE);
		angleArr[i].fAngle=Cal2dLineAng(0,0,vertex.x,vertex.y);
	}
	CQuickSort<MAIN_ANGLE>::QuickSortClassic(angleArr,4);
	//3.�������ĽǸַ��߲���
	CUndoOperObject undo(&Ta,true); 
	f3dPoint pre_vec,next_vec,angle_vec;
	for(i=0;i<4;i++)
	{	
		angle_vec=angleArr[i].End()-angleArr[i].Start();
		normalize(angle_vec);
		MAIN_ANGLE *pPreAngle=&(angleArr[(i-1+4)%4]);
		MAIN_ANGLE *pNextAngle=&(angleArr[(i+1)%4]);
		pre_vec=angleArr[i].vertex-pPreAngle->vertex;
		normalize(pre_vec);
		next_vec=pNextAngle->vertex-angleArr[i].vertex;
		normalize(next_vec);
		//
		DESIGN_JGWING_NORM *pPreDesNorm=&(angleArr[i].pAngle->des_norm_x);
		DESIGN_JGWING_NORM *pNextDesNorm=&(angleArr[i].pAngle->des_norm_y);
		if( ((dlg.m_iPolyStyle==0||dlg.m_iPolyStyle==3)&&angle_vec*face_norm>0)||	//0.����	 3.����
			((dlg.m_iPolyStyle==1||dlg.m_iPolyStyle==2)&&angle_vec*face_norm<0))	//1.��ʽ	 2.��ʽ
		{
			pPreDesNorm=&(angleArr[i].pAngle->des_norm_y);
			pNextDesNorm=&(angleArr[i].pAngle->des_norm_x);
		}
		pPreDesNorm->bByOtherWing=FALSE;
		pPreDesNorm->bSpecific=FALSE;
		pNextDesNorm->bByOtherWing=FALSE;
		pNextDesNorm->bSpecific=FALSE;
		//���ýǸ�֫���߲��ո˼�
		pNextDesNorm->hViceJg=pNextAngle->pAngle->handle;
		pPreDesNorm->hViceJg=pPreAngle->pAngle->handle;
		//���ýǸ�֫���߽��Ʒ���
		if(dlg.m_iPolyStyle==0)
		{
			pNextDesNorm->near_norm=next_vec^face_norm;
			pPreDesNorm->near_norm=pre_vec^face_norm;
		}
		if(dlg.m_iPolyStyle==3)
		{
			pNextDesNorm->near_norm=face_norm^next_vec;
			pPreDesNorm->near_norm=face_norm^pre_vec;
		}
		else if(dlg.m_iPolyStyle==1)
		{
			if(i==0||i==2)
			{
				pNextDesNorm->near_norm=face_norm^next_vec;
				pPreDesNorm->near_norm=pre_vec^face_norm;
			}
			else 
			{
				pNextDesNorm->near_norm=next_vec^face_norm;
				pPreDesNorm->near_norm=face_norm^pre_vec;
			}
		}
		else if(dlg.m_iPolyStyle==2)
		{
			if(i==0||i==2)
			{
				pNextDesNorm->near_norm=next_vec^face_norm;
				pPreDesNorm->near_norm=face_norm^pre_vec;
			}
			else
			{
				pNextDesNorm->near_norm=face_norm^next_vec;
				pPreDesNorm->near_norm=pre_vec^face_norm;
			}
		}
	}
	//4.���¼���Ǹ�֫���߷���
	for(i=0;i<4;i++)
	{
		angleArr[i].pAngle->cal_x_wing_norm();
		angleArr[i].pAngle->cal_y_wing_norm();
		if(angleArr[i].pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
			((CLDSGroupLineAngle*)angleArr[i].pAngle)->UpdateSonJgWingNorm();
		angleArr[i].pAngle->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(angleArr[i].pAngle->GetSolidPartObject());
		RedrawAngleFrameDrawing(angleArr[i].pAngle);
	}
	g_pSolidDraw->Draw();
	return TRUE;
}
#endif
//��DrawSolid���Դ����������
void CLDSView::ConnectSolidSrcComp()
{
	if(m_dwCookie>0)	//�ѽ�������
		return;
	/*LPCONNECTIONPOINTCONTAINER pConnPtCont;
	if(SUCCEEDED(g_pSolidSet->QueryInterface(IID_IConnectionPointContainer,(LPVOID*)&pConnPtCont)))
	{
		ASSERT(pConnPtCont!=NULL);
		LPCONNECTIONPOINT pConnPt=NULL;
		DWORD dwCookie=0;
		if(SUCCEEDED(pConnPtCont->FindConnectionPoint(IID_IPickEventSet,&pConnPt)))
		{
			ASSERT(pConnPt!=NULL);
			pConnPt->Advise(&m_xEntPickEventSink,&dwCookie);
			pConnPt->Release();
		}
		pConnPtCont->Release();
		m_dwCookie=dwCookie;
	}*/
}
//����DrawSolid���Դ�������
void CLDSView::DisconnectSolidSrcComp()
{
	if(m_dwCookie<=0)	//�ѳ�������
		return;
	/*LPCONNECTIONPOINTCONTAINER pConnPtCont;
	if(SUCCEEDED(g_pSolidSet->QueryInterface(IID_IConnectionPointContainer,(LPVOID*)&pConnPtCont)))
	{
		ASSERT(pConnPtCont!=NULL);
		LPCONNECTIONPOINT pConnPt=NULL;
		if(SUCCEEDED(pConnPtCont->FindConnectionPoint(IID_IPickEventSet,&pConnPt)))
		{
			ASSERT(pConnPt!=NULL);
			pConnPt->Unadvise(m_dwCookie);
			pConnPt->Release();
		}
		pConnPtCont->Release();
		m_dwCookie=0;
	}*/
}

void CLDSView::OnLayFillPlank() 
{
	m_nPrevCommandID=ID_LAY_FILL_PLANK;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat to layout filling board";
#else
	m_sPrevCommandName="�ظ���岼��";
#endif
	g_pSolidSet->SetOperType(OPER_OSNAP);
	g_pSolidSnap->SetSnapType(SNAP_ALL);
	m_curTask = TASK_ADD_FILL_PLANK;
	m_nObjectSnapedCounts=0;
	m_setSnapObjects.Empty();
	//�л���������ʾ״̬
	g_pSolidSet->SetDisplayType(DISP_LINE);
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
    g_pPromptMsg->SetMsg("please select the angle steel to be filled or its nodes!");
#else
	g_pPromptMsg->SetMsg("��ѡ����Ҫ������ĽǸֻ����ϵĽڵ�!");
#endif
}
void CLDSView::OnDaoweiSelectPart() 
{
	long *id_arr=NULL;
	long nn=g_pSolidSnap->GetLastSelectEnts(id_arr);
	CProcBarDlg *pProcDlg = new CProcBarDlg(AfxGetApp()->m_pMainWnd);
	pProcDlg->Create();
	CLogErrorLife loglife;
	CUndoOperObject undo(&Ta,true,CUndoOperObject::UNDO_REPOSITION,false);
	if(Ta.CurrentUndoType()!=CUndoOperObject::UNDO_REPOSITION)
		undo.StartUndoListen();	//�����ظ�ѹ�뵽λ�����Undo��¼�������û�����F7������Undo����ʱ�о������� wjh-2015.7.22
	bool hasBoltOrPlates=false;
	for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart&&!hasBoltOrPlates;pPart=Ta.Parts.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_BOLT||pPart->IsPlate())
		{
			hasBoltOrPlates=true;
			break;
		}
	}
	CLDSPart::INTERNAL_TEST_VERSION=(theApp.m_bAnalysisWorkEnv||!hasBoltOrPlates)?false:(PRODUCT_FUNC::IsHasInternalTest()==TRUE);
	CNodeMoveOperObject movenodes;
	if(nn>50)
		movenodes.Enable(false);
	try
	{
		CHashSet<CLDSGroupLineAngle*> hashRelaParentGroupAngles;
		for(int i=0;i<nn;i++)
		{
			CLDSDbObject *pObj=(CLDSDbObject*)console.FromHandle(id_arr[i],TRUE);
			if(pObj==NULL)
				continue;
			CAngleMoveOperObject moveangle(NULL,true, nn > 50);
			moveangle.Enable(false);
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
			if(pObj->GetClassTypeId()==CLS_LINEANGLE || pObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSLineAngle *pCommJg=(CLDSLineAngle*)pObj;
				if (pCommJg->group_father_jg_h > 0x20 && hashRelaParentGroupAngles.GetValue(pCommJg->group_father_jg_h) == NULL)
				{
					CLDSGroupLineAngle* pGroupAngle = (CLDSGroupLineAngle*)Ta.FromPartHandle(pCommJg->group_father_jg_h, CLS_GROUPLINEANGLE);
					if (pGroupAngle)
						pObj = pCommJg = pGroupAngle;
				}
				if (nn <= 50)
					moveangle.Enable(true);
				pCommJg->ClearFlag();
				pCommJg->cal_x_wing_norm();
				pCommJg->cal_y_wing_norm();
				pCommJg->CalPosition();
				if(pObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
				{
					((CLDSGroupLineAngle*)pCommJg)->UpdateSonJgWingNorm();
					((CLDSGroupLineAngle*)pCommJg)->UpdateSonJgPos();
					hashRelaParentGroupAngles.SetValue(pObj->handle, (CLDSGroupLineAngle*)pObj);
				}
				else
					RedrawAngleFrameDrawing(pCommJg);
			}
			else if(pObj->GetClassTypeId()==CLS_LINETUBE)
			{
				CLDSLineTube *pTube=(CLDSLineTube*)pObj;
				pTube->ClearFlag();
				pTube->CalWorkPlaneNorm();
				pTube->CalWeldRoadAngle();
				pTube->CalPosition();
			}
			else if(pObj->GetClassTypeId()==CLS_LINESLOT)
			{
				CLDSLineSlot *pSlot=(CLDSLineSlot*)pObj;
				pSlot->ClearFlag();
				pSlot->CalWorkPlaneNorm();
				pSlot->CalPosition();
				RedrawSlotFrameDrawing(pSlot);
			}
			else if(pObj->GetClassTypeId()==CLS_LINEFLAT)
			{
				CLDSLineFlat *pFlat=(CLDSLineFlat*)pObj;
				pFlat->ClearFlag();
				pFlat->CalWorkPlaneNorm();
				pFlat->CalPosition();
				RedrawFlatFrameDrawing(pFlat);
			}
#endif
			else if(pObj->GetClassTypeId()==CLS_NODE)
			{
				((CLDSNode*)pObj)->Unlock();
				GEPOINT oldpos=((CLDSNode*)pObj)->ActivePosition();
				((CLDSNode*)pObj)->Set();
				GEPOINT pos=((CLDSNode*)pObj)->ActivePosition();
				if(!oldpos.IsEqual(pos.x,pos.y,pos.z,EPS2))
					g_pSolidDraw->ModifyDbPoint(pObj->handle,pos);
			}
			else if(pObj->IsPart()&&((CLDSPart*)pObj)->IsArcPart())
			{
				CLDSArcPart* pArcPart=(CLDSArcPart*)pObj;
				pArcPart->DesignArcBaseLine();
			}
			/*else if(pObj->GetClassTypeId()==CLS_BOLT)
			{
				CLDSBolt *pBolt=(CLDSBolt*)pObj;
				pBolt->correct_pos();
				pBolt->correct_worknorm();
			}*/
			if(pObj->IsPart())
			{
				CLDSPart* pPart=(CLDSPart*)pObj;
				if (pPart->GetClassTypeId() == CLS_LINEANGLE || pPart->GetClassTypeId() == CLS_GROUPLINEANGLE)
				{
					moveangle.Terminate(true, (CLDSLineAngle*)pPart);
					if (pPart->GetClassTypeId() == CLS_GROUPLINEANGLE)
					{
						CLDSGroupLineAngle* pGroupAngle = (CLDSGroupLineAngle*)pPart;
						int count = pGroupAngle->wSectStyle < 2 ? 2 : 4;
						for (int index = 0; index < count; index++)
						{
							CLDSLineAngle*  pAngle = pGroupAngle->son_jg_h[index] <= 0x20 ? NULL : (CLDSLineAngle*)Ta.FromPartHandle(pGroupAngle->son_jg_h[index], CLS_LINEANGLE);
							if (pAngle == NULL)
								continue;
							pAngle->Create3dSolidModel();
							g_pSolidDraw->NewSolidPart(pAngle->GetSolidPartObject());
						}
					}
				}
				else
				{
					pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
					g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
				}
			}
			pProcDlg->Refresh((100*i)/nn);
		}
		pProcDlg->DestroyWindow();
		delete pProcDlg;
		g_pSolidDraw->Draw();
		CLDSPart::INTERNAL_TEST_VERSION=(PRODUCT_FUNC::IsHasInternalTest()==TRUE);
	}
	//�����쳣���������⣬û�д����м��ѹջ��δ��վ���⣬�����ⲻ���û��������쳣��λ2048�����ϡ�2003/10/18 WJH
	catch(char *sError)
	{
		AfxMessageBox(sError);
		if(pProcDlg!=NULL)
			delete pProcDlg;
	}
}
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
// defined in LinkJg.cpp wjh-2016.6.10
bool InitHorizonCrossAnglePosition(CLDSLineAngle* pAngle);
bool CorrectSingleAngleLayout(CLDSLineAngle* pJg,bool updatePartSolid=true)
{
	if(pJg->pStart==NULL||pJg->pEnd==NULL)
		return false;
	if(InitHorizonCrossAnglePosition(pJg))	//�Զ������ߵ��Խǽڵ�ĽǸְڷ�λ��
	{
		int startWorkWing=pJg->GetStartWorkWing();
		int endWorkWing=pJg->GetEndWorkWing();
		CLDSLinePart* pDatumRod=NULL;
		if( pJg->desStartPos.spatialOperationStyle==0&&pJg->desStartPos.datumPoint.datum_pos_style==3)
			pDatumRod=Ta.FromRodHandle(pJg->desStartPos.datumPoint.des_para.AXIS_INTERS.hDatum1);
		if( pDatumRod==NULL&&
			pJg->desEndPos.spatialOperationStyle==0&&pJg->desEndPos.datumPoint.datum_pos_style==3)
			pDatumRod=Ta.FromRodHandle(pJg->desEndPos.datumPoint.des_para.AXIS_INTERS.hDatum1);
		if(pDatumRod)
		{	//������
			GEPOINT rodvec=pJg->End()-pJg->Start();
			GEPOINT datumvec=pDatumRod->End()-pDatumRod->Start();
			GEPOINT worknorm=rodvec^datumvec;
			normalize(worknorm);
			int workwing=startWorkWing<2?startWorkWing:endWorkWing;
			if(workwing==0&&!pJg->des_norm_x.IsByOtherWingActual()&&fabs(worknorm*pJg->vxWingNorm)<0.98)
				pJg->des_norm_x.hViceJg=pDatumRod->handle;
			else if(workwing==1&&!pJg->des_norm_y.IsByOtherWingActual()&&fabs(worknorm*pJg->vyWingNorm)<0.98)
				pJg->des_norm_y.hViceJg=pDatumRod->handle;
		}
		if(startWorkWing==0)
		{
			pJg->desStartPos.wing_x_offset.gStyle=4;
			pJg->desStartPos.wing_x_offset.offsetDist=0;
			pJg->desStartPos.wing_y_offset.gStyle=0;
		}
		else
		{
			pJg->desStartPos.wing_y_offset.gStyle=4;
			pJg->desStartPos.wing_y_offset.offsetDist=0;
			pJg->desStartPos.wing_x_offset.gStyle=0;
		}
		if(endWorkWing==0)
		{
			pJg->desEndPos.wing_x_offset.gStyle=4;
			pJg->desEndPos.wing_x_offset.offsetDist=0;
			pJg->desEndPos.wing_y_offset.gStyle=0;
		}
		else
		{
			pJg->desEndPos.wing_y_offset.gStyle=4;
			pJg->desEndPos.wing_y_offset.offsetDist=0;
			pJg->desEndPos.wing_x_offset.gStyle=0;
		}
	}
	pJg->ClearFlag();
	pJg->UnlockStart();
	pJg->UnlockEnd();
	int wing_x0_y1,father_wing_x0_y1;
	bool wingxnorm_corrected=false,wingynorm_corrected=false;
	CLDSLinePart *pRefRodWingX=NULL,*pRefRodWingY=NULL;;
	if(pJg->des_norm_x.hViceJg>0)
		pRefRodWingX=Ta.FromRodHandle(pJg->des_norm_x.hViceJg);
	if(pJg->des_norm_y.hViceJg>0)
		pRefRodWingY=Ta.FromRodHandle(pJg->des_norm_y.hViceJg);
	if((!pJg->des_norm_x.bSpecific&&pRefRodWingX!=NULL)||pJg->des_norm_x.bByOtherWing||pJg->des_norm_x.spec_norm.norm_style>0)
		wingxnorm_corrected=true;	//X֫���߼��㷽ʽ��Ч
	if((!pJg->des_norm_y.bSpecific&&pRefRodWingY!=NULL)||pJg->des_norm_y.bByOtherWing||pJg->des_norm_y.spec_norm.norm_style>0)
		wingynorm_corrected=true;	//Y֫���߼��㷽ʽ��Ч
	if(pJg->des_norm_x.bByOtherWing&&pJg->des_norm_y.bByOtherWing)
		wingxnorm_corrected=wingynorm_corrected=false;	//��֫���߻�����������Ϊ��Ч
	int style=CStdVector::GetVectorType(pJg->des_norm_x.spec_norm.vector);
	if(wingxnorm_corrected&&pJg->des_norm_x.bSpecific&&(style<1||style>6))
		wingxnorm_corrected=false;	//ָ�����߷Ǳ�׼ʸ������
	style=CStdVector::GetVectorType(pJg->des_norm_y.spec_norm.vector);
	if(wingynorm_corrected&&pJg->des_norm_y.bSpecific&&(style<1||style>6))
		wingynorm_corrected=false;	//ָ�����߷Ǳ�׼ʸ������
	f3dPoint lenStdVec=pJg->End()-pJg->Start();
	GEPOINT lenBriefLineStdVec=lenStdVec;
	if(pJg->pStart&&pJg->pEnd)
		lenBriefLineStdVec=pJg->pEnd->xPreliftPos-pJg->pStart->xPreliftPos;
	if(!wingxnorm_corrected||!wingynorm_corrected)
	{
		normalize(lenStdVec);	//��Ҫ���ü���֫����
		normalize(lenBriefLineStdVec);
	}

	//if(!pJg->des_norm_x.bSpecific&&)
	CLDSLineAngle *pFatherJg=NULL;
	CLDSLinePart  *pFatherRod=NULL;
	if(pJg->desStartPos.datum_jg_h>0x20&&pJg->desStartPos.datum_jg_h!=pJg->handle)
		pFatherRod=console.FromRodHandle(pJg->desStartPos.datum_jg_h);
	else if(pJg->pStart->hFatherPart!=pJg->handle)
		pFatherRod=console.FromRodHandle(pJg->pStart->hFatherPart);
	else if(pJg->desEndPos.datum_jg_h>0x20&&pJg->desEndPos.datum_jg_h!=pJg->handle)
		pFatherRod=console.FromRodHandle(pJg->desEndPos.datum_jg_h);
	else if(pJg->pEnd->hFatherPart!=pJg->handle)
		pFatherRod=console.FromRodHandle(pJg->pEnd->hFatherPart);
	f3dPoint lenRefStdVec;
	if(pFatherRod&&pFatherRod!=pJg)
	{
		if(pFatherRod->IsAngle())
			pFatherJg=(CLDSLineAngle*)pFatherRod;
		lenRefStdVec=pFatherRod->End()-pFatherRod->Start();
		normalize(lenRefStdVec);
		if(!pFatherRod->ModuleTogetherWith(pJg->pStart))
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"the model flag of 0x%X bar's start node is not identical with parent bars, may lead to leg placement correct mistakes!",pJg->handle);
#else
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X�˼���ʼ�˽ڵ��븸�˼�����ĺŲ���ȫ��ͬ,���ܵ���֫�ڷ�λ����������!",pJg->handle);
#endif
		CalAngleLayoutStyle(pJg,pFatherRod,&wing_x0_y1,&father_wing_x0_y1);
		//�ղ���ģ���еĺ�ĽǸֺ��п��ܽǸ�δ��λ���λ�ô��ڽϴ�ƫ���������ʴ˴���ȡ�������������Ա������ģ��ʱ���� wjh-2018.2.11
		if(pJg->Layer(0)!='T'&&pJg->Layer(1)=='H'&&(fabs(lenBriefLineStdVec.x)<EPS2||fabs(lenBriefLineStdVec.y)<EPS2))
		{	//�Զ������������������ĵĿ��Ͻ�
			if(wing_x0_y1==0)
			{
				if(pJg->des_norm_x.bByOtherWing||pJg->des_norm_x.bSpecific)
				{
					pJg->des_norm_x.hViceJg=pFatherRod->handle;
					pJg->des_norm_x.bByOtherWing=FALSE;
					pJg->des_norm_x.bSpecific=FALSE;
					pJg->des_norm_x.near_norm=pJg->get_norm_x_wing();
					pJg->cal_x_wing_norm(true);
					wingxnorm_corrected=true;
				}
				if(pJg->des_norm_y.bByOtherWing||pJg->des_norm_y.bSpecific)
				{
					CLDSLinePart* pMirRod=pJg->GetMirRod(MIRMSG(3));
					if(pMirRod&&pMirRod->handle!=pJg->handle)
					{
						GEPOINT vNormal=CalFaceNorm(pJg->Start(),pJg->End(),pMirRod->Start(),pMirRod->End());
						if(fabs(vNormal*pJg->vxWingNorm)<EPS_COS2)
						{	//��ֹģ���аѺ�����ڲ���Сб������Ϊ��ģ�������֫���߶�����Ϊˮƽ�����������wjh-2018.1.30
							pJg->des_norm_y.hViceJg=pMirRod->handle;
							pJg->des_norm_y.bByOtherWing=FALSE;
							pJg->des_norm_y.bSpecific=FALSE;
							pJg->des_norm_y.near_norm=pJg->get_norm_y_wing();
							wingynorm_corrected=true;
						}
					}
				}
			}
			else if(wing_x0_y1==1)
			{
				if(pJg->des_norm_y.bByOtherWing||pJg->des_norm_y.bSpecific)
				{
					pJg->des_norm_y.hViceJg=pFatherRod->handle;
					pJg->des_norm_y.bByOtherWing=FALSE;
					pJg->des_norm_y.bSpecific=FALSE;
					pJg->des_norm_y.near_norm=pJg->get_norm_y_wing();
					pJg->cal_y_wing_norm(true);
					wingynorm_corrected=true;
				}
				if(pJg->des_norm_x.bByOtherWing||pJg->des_norm_x.bSpecific)
				{
					CLDSLinePart* pMirRod=pJg->GetMirRod(MIRMSG(3));
					if(pMirRod&&pMirRod->handle!=pJg->handle)
					{	//֫���߲��ո˼�һ�������ǽǸ����� wjh-2015.3.3
						GEPOINT vNormal=CalFaceNorm(pJg->Start(),pJg->End(),pMirRod->Start(),pMirRod->End());
						if(fabs(vNormal*pJg->vyWingNorm)<EPS_COS2)
						{	//��ֹģ���аѺ�����ڲ���Сб������Ϊ��ģ�������֫���߶�����Ϊˮƽ�����������wjh-2018.1.30
							pJg->des_norm_x.hViceJg=pMirRod->handle;
							pJg->des_norm_x.bByOtherWing=FALSE;
							pJg->des_norm_x.bSpecific=FALSE;
							pJg->des_norm_x.near_norm=pJg->get_norm_x_wing();
							wingxnorm_corrected=true;
						}
					}
				}
			}
		}
	}
	if(pFatherRod&&pFatherRod!=pJg&&pJg->desStartPos.jgber_cal_style!=1&&!pJg->IsStartPosLocked()&&
		((pJg->desStartPos.spatialOperationStyle>0&&pJg->desStartPos.spatialOperationStyle<7)||pJg->desStartPos.spatialOperationStyle==18))
	{
		BOOL bFaceOffset=FALSE;
		if(pJg->desStartPos.IsFaceOffset())//wjh -2011-6-26������׼ȷ
			bFaceOffset=TRUE;
		if(pJg->desStartPos.IsInDatumJgBer()&&pJg->desStartPos.wing_x_offset.gStyle==4&&pJg->desStartPos.wing_y_offset.gStyle==4)
			return false;	//���������ϣ�����֫����ƫ�����û�ָ��ʱ������
		normalize(lenRefStdVec);	//��Ҫ���ü���֫����
		if(wing_x0_y1==0)
		{
			if(!wingxnorm_corrected)
			{
				if(fabs(lenStdVec*lenRefStdVec)<EPS_COS2)
				{
					pJg->des_norm_x.hViceJg=pFatherRod->handle;
					pJg->des_norm_x.bSpecific=FALSE;
					wingxnorm_corrected=true;
				}
				else if(wingynorm_corrected&&!pJg->des_norm_y.bByOtherWing)
				{
					pJg->des_norm_x.bByOtherWing=TRUE;
					wingxnorm_corrected=true;
				}
				else
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT," X leg resetting of 0X%X angle steel failed!",pJg->handle);
#else
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X�Ǹֵ�X֫��������ʧ��!",pJg->handle);
#endif
			}
			if(!wingynorm_corrected)
			{
				if(!pJg->des_norm_x.bByOtherWing)
				{
					pJg->des_norm_y.bByOtherWing=TRUE;
					wingynorm_corrected=true;
				}
				else
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"Y leg resetting of 0X%X angle steel failed!",pJg->handle);
#else
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X�Ǹֵ�Y֫��������ʧ��!",pJg->handle);
#endif
			}
		}
		else
		{
			if(!wingynorm_corrected)
			{
				if(fabs(lenStdVec*lenRefStdVec)<EPS_COS2)
				{
					pJg->des_norm_y.hViceJg=pFatherRod->handle;
					pJg->des_norm_y.bSpecific=FALSE;
					wingynorm_corrected=true;
				}
				else if(wingxnorm_corrected&&!pJg->des_norm_x.bByOtherWing)
				{
					pJg->des_norm_y.bByOtherWing=TRUE;
					wingynorm_corrected=true;
				}
				else
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"Y leg resetting of 0X%X angle steel failed!",pJg->handle);
#else
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X�Ǹֵ�Y֫��������ʧ��!",pJg->handle);
#endif
			}
			if(!wingxnorm_corrected)
			{
				if(!pJg->des_norm_y.bByOtherWing)
				{
					pJg->des_norm_x.bByOtherWing=TRUE;
					wingxnorm_corrected=true;
				}
				else
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"X leg resetting of 0X%X angle steel failed!",pJg->handle);
#else
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X�Ǹֵ�X֫��������ʧ��!",pJg->handle);
#endif
			}
		}
		if(!(pJg->desStartPos.datum_jg_h==pJg->handle||pJg->pStart->hFatherPart==pJg->handle))
		{
			if(wing_x0_y1==0)
			{
				pJg->desStartPos.wing_x_offset.gStyle=4;
				pJg->desStartPos.wing_x_offset.offsetDist=0;
				pJg->desStartPos.wing_y_offset.gStyle=0;
			}
			else
			{
				pJg->desStartPos.wing_x_offset.gStyle=0;
				pJg->desStartPos.wing_y_offset.gStyle=4;
				pJg->desStartPos.wing_y_offset.offsetDist=0;
			}
			if(pFatherJg==NULL)	//���˼�Ϊ�ֹ�ʱӦ�Զ�����Ϊ��������
				pJg->desStartPos.spatialOperationStyle=1;
			else
			{
				if(pFatherJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
				{
					if(((CLDSGroupLineAngle*)pFatherJg)->group_style!=1)
						pJg->desStartPos.spatialOperationStyle=1;
					else if(father_wing_x0_y1==0)	//T�����
						pJg->desStartPos.spatialOperationStyle=2;	//����X֫
					else
						pJg->desStartPos.spatialOperationStyle=1;	//������
				}
				else if(pJg->desStartPos.spatialOperationStyle!=1&&father_wing_x0_y1==0)
					pJg->desStartPos.spatialOperationStyle=2;
				else if(pJg->desStartPos.spatialOperationStyle!=1&&father_wing_x0_y1==1)
					pJg->desStartPos.spatialOperationStyle=3;
				normalize(pJg->desStartPos.face_offset_norm);
				if(bFaceOffset&&fabs(pJg->desStartPos.face_offset_norm*lenRefStdVec)>0.707)	//0.707=cos(45��),���Զ��幤�̾���ֵ wjh-2015.1.27
					pJg->desStartPos.ChangeToDatumType(2);
				//һЩб����������������ƫ�ƽڵ�ĸ��ģ������Ϊƽ�ƻ�����ܶ����⣬�����Ǹ��Ǹ������Ӷ�λ��׼���Զ���λʱ������ǿ�и�ΪͶӰ��wjh-2018.1.17
				else if(pFatherJg->m_cPosCtrlDatumLineType!='X'&&pFatherJg->m_cPosCtrlDatumLineType!='Y'&&
					pJg->layer(1)!='Z'&&pFatherJg->layer(0)!='T'&&pFatherJg->layer(1)=='Z'&&	//���˼�Ϊ������Ȳ�����
					fabs(pJg->desStartPos.face_offset_norm*lenRefStdVec)>0.707)
					pJg->desStartPos.ChangeToDatumType(2);
			}
		}
	}
	if(pJg->desEndPos.datum_jg_h>0x20)
		pFatherRod=console.FromRodHandle(pJg->desEndPos.datum_jg_h);
	else
		pFatherRod=console.FromRodHandle(pJg->pEnd->hFatherPart);
	if(pFatherRod!=NULL&&pFatherRod->IsAngle())
		pFatherJg=(CLDSLineAngle*)pFatherRod;
	else
		pFatherJg=NULL;
	if(pFatherRod&&pFatherRod!=pJg&&pJg->desEndPos.jgber_cal_style!=1&&!pJg->IsEndPosLocked()&&
		((pJg->desEndPos.spatialOperationStyle>0&&pJg->desEndPos.spatialOperationStyle<7)||pJg->desEndPos.spatialOperationStyle==18))
	{
		lenRefStdVec=pFatherRod->End()-pFatherRod->Start();
		normalize(lenRefStdVec);
		BOOL bFaceOffset=FALSE;

		if(pJg->desEndPos.IsFaceOffset())
			bFaceOffset=TRUE;
		if(pFatherJg!=NULL)
		{
			double ddxx=fabs(pJg->get_norm_x_wing()*pFatherJg->get_norm_x_wing());
			double ddxy=fabs(pJg->get_norm_x_wing()*pFatherJg->get_norm_y_wing());
			double ddyx=fabs(pJg->get_norm_y_wing()*pFatherJg->get_norm_x_wing());
			double ddyy=fabs(pJg->get_norm_y_wing()*pFatherJg->get_norm_y_wing());
			if(IsMaxInAll(ddxx,ddxy,ddyx,ddyy))
			{
				wing_x0_y1=0;
				father_wing_x0_y1=0;
			}
			else if(IsMaxInAll(ddxy,ddxx,ddyx,ddyy))
			{
				wing_x0_y1=0;
				father_wing_x0_y1=1;
			}
			else if(IsMaxInAll(ddyx,ddxx,ddxy,ddyy))
			{
				wing_x0_y1=1;
				father_wing_x0_y1=0;
			}
			else// if(IsMaxInAll(ddyy,ddxx,ddxy,ddyx))
			{
				wing_x0_y1=1;
				father_wing_x0_y1=1;
			}
		}
		if(!wingxnorm_corrected||!wingynorm_corrected)
			normalize(lenRefStdVec);	//��Ҫ���ü���֫����
		if(wing_x0_y1==0)
		{
			if(!wingxnorm_corrected)
			{
				if(fabs(lenStdVec*lenRefStdVec)<0.9)
				{
					pJg->des_norm_x.hViceJg=pFatherRod->handle;
					pJg->des_norm_x.bSpecific=FALSE;
					wingxnorm_corrected=true;
				}
				else if(wingynorm_corrected&&!pJg->des_norm_y.bByOtherWing)
				{
					pJg->des_norm_x.bByOtherWing=TRUE;
					pJg->des_norm_x.bSpecific=FALSE;
					wingxnorm_corrected=true;
				}
				else
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"X leg resetting of 0X%X angle steel failed!",pJg->handle);
#else
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X�Ǹֵ�X֫��������ʧ��!",pJg->handle);
#endif
			}
			if(!wingynorm_corrected)
			{
				if(pJg->des_norm_y.bSpecific||!pJg->des_norm_x.bByOtherWing)
				{
					pJg->des_norm_y.bSpecific=FALSE;
					pJg->des_norm_y.bByOtherWing=TRUE;
					wingynorm_corrected=true;
				}
				else
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"Y leg resetting of 0X%X angle steel failed!",pJg->handle);
#else
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X�Ǹֵ�Y֫��������ʧ��!",pJg->handle);
#endif
			}
		}
		else
		{
			if(!wingynorm_corrected)
			{
				if(fabs(lenStdVec*lenRefStdVec)<0.9)
				{
					pJg->des_norm_y.hViceJg=pFatherRod->handle;
					pJg->des_norm_y.bSpecific=FALSE;
					wingynorm_corrected=true;
				}
				else if(wingxnorm_corrected&&!pJg->des_norm_x.bByOtherWing)
				{
					pJg->des_norm_y.bByOtherWing=TRUE;
					pJg->des_norm_y.bSpecific=FALSE;
					wingynorm_corrected=true;
				}
				else
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"Y leg resetting of 0X%X angle steel failed!",pJg->handle);
#else
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X�Ǹֵ�Y֫��������ʧ��!",pJg->handle);
#endif
			}
			if(!wingxnorm_corrected)
			{
				if(!pJg->des_norm_y.bByOtherWing)
				{
					pJg->des_norm_x.bByOtherWing=TRUE;
					pJg->des_norm_x.bSpecific=FALSE;
					wingxnorm_corrected=true;
				}
				else
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"X leg resetting of 0X%X angle steel failed!",pJg->handle);
#else
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X�Ǹֵ�X֫��������ʧ��!",pJg->handle);
#endif
			}
		}
		if(!(pJg->desEndPos.datum_jg_h==pJg->handle||pJg->pEnd->hFatherPart==pJg->handle))
		{
			if(wing_x0_y1==0)
			{
				pJg->desEndPos.wing_x_offset.gStyle=4;
				pJg->desEndPos.wing_x_offset.offsetDist=0;
				pJg->desEndPos.wing_y_offset.gStyle=0;
			}
			else
			{
				pJg->desEndPos.wing_x_offset.gStyle=0;
				pJg->desEndPos.wing_y_offset.gStyle=4;
				pJg->desEndPos.wing_y_offset.offsetDist=0;
			}
			if(pFatherJg==NULL)	//���˼�Ϊ�ֹ�ʱӦ�Զ�����Ϊ��������
				pJg->desEndPos.spatialOperationStyle=1;
			else
			{
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
				if(pFatherJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
				{
					if(((CLDSGroupLineAngle*)pFatherJg)->group_style!=1)
						pJg->desEndPos.spatialOperationStyle=1;
					else if(father_wing_x0_y1==0)	//T�����
						pJg->desEndPos.spatialOperationStyle=2;	//����X֫
					else
						pJg->desEndPos.spatialOperationStyle=1;	//������
				}
#endif
				else if(pJg->desEndPos.spatialOperationStyle!=1&&father_wing_x0_y1==0)
					pJg->desEndPos.spatialOperationStyle=2;
				else if(pJg->desEndPos.spatialOperationStyle!=1&&father_wing_x0_y1==1)
					pJg->desEndPos.spatialOperationStyle=3;
				if(bFaceOffset&&fabs(pJg->desEndPos.face_offset_norm*lenRefStdVec)>0.707)	//0.707=cos(45��),���Զ��幤�̾���ֵ wjh-2015.1.27
					pJg->desEndPos.ChangeToDatumType(2);
				//һЩб����������������ƫ�ƽڵ�ĸ��ģ������Ϊƽ�ƻ�����ܶ����⣬�����Ǹ��Ǹ������Ӷ�λ��׼���Զ���λʱ������ǿ�и�ΪͶӰ��wjh-2018.1.17
				else if(pFatherJg->m_cPosCtrlDatumLineType!='X'&&pFatherJg->m_cPosCtrlDatumLineType!='Y'&&
					pJg->layer(1)!='Z'&&pFatherJg->layer(0)!='T'&&pFatherJg->layer(1)=='Z'&&	//���˼�Ϊ������Ȳ�����
					fabs(pJg->desEndPos.face_offset_norm*lenRefStdVec)>0.707)
					pJg->desEndPos.ChangeToDatumType(2);
			}
		}
	}
	pJg->CalPosition();
	pJg->cal_x_wing_norm();
	pJg->cal_y_wing_norm();
	pJg->SetModified();
	if(updatePartSolid)
	{
		pJg->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pJg->GetSolidPartObject());
		CLDSView::RedrawAngleFrameDrawing(pJg);
	}
	return true;
}
void CorrectJgLaystyle(long *id_arr,long n)
{
	CProcBarDlg *pProcDlg = new CProcBarDlg(AfxGetApp()->m_pMainWnd);
	pProcDlg->Create();
	CLogErrorLife life;
	CUndoOperObject undo(&Ta,true);
	try
	{
		for(int i=0;i<n;i++)
		{
			CLDSDbObject *pObj=(CLDSDbObject*)console.FromHandle(id_arr[i],TRUE);
			if(pObj==NULL)
				continue;
			else if(pObj->Layer(0)!='T'&&pObj->Layer(1)=='Z')
			{
				if(pObj->GetClassTypeId()==CLS_LINEANGLE || pObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X angle is leg rod or head part, do not meet the conditions for angle layout correction!",pObj->handle);
#else
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X�Ǹ�Ϊ���Ļ�ͷ���˼��������ϽǸְڷ���ʽ��ִ������",pObj->handle);
#endif
				continue;	//�����Ȳ����Ĳ��������ڷ�λ��
			}
			else if(pObj->GetClassTypeId()==CLS_LINEANGLE || pObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CorrectSingleAngleLayout((CLDSLineAngle*)pObj);
				pProcDlg->Refresh((100*i)/n);
			}
			else
				continue;
		}
		pProcDlg->DestroyWindow();
		if(pProcDlg!=NULL)
			delete pProcDlg;
	}
	//�����쳣���������⣬û�д����м��ѹջ��δ��վ���⣬�����ⲻ���û��������쳣��λ2048�����ϡ�2003/10/18 WJH
	catch(char *sError)
	{
		Ta.EndUndoListen();
		AfxMessageBox(sError);
		pProcDlg->DestroyWindow();
		if(pProcDlg!=NULL)
			delete pProcDlg;
	}
}
#endif
void CLDSView::OnAutoCorrectJgLaystyle() 
{
#ifdef __ANGLE_PART_INC_
	long *id_arr=NULL;
	long nn=g_pSolidSnap->GetLastSelectEnts(id_arr);
	CorrectJgLaystyle(id_arr,nn);
	g_pSolidDraw->Draw();
#endif
}

#ifdef __ANGLE_PART_INC_
void CLDSView::SolidCollideCheck(SCOPE_STRU *pScope/*=NULL*/)
{
	int nInvalidHits = 0;
	int n;
	static CSolidCollideOptionDlg solid_option_dlg;
	if(solid_option_dlg.DoModal()!=IDOK)
		return;
	if(!(solid_option_dlg.m_bJgVsJg||solid_option_dlg.m_bJgVsLjb||solid_option_dlg.m_bJgVsLs||
		solid_option_dlg.m_bLjbVsLjb||solid_option_dlg.m_bLjbVsLs||solid_option_dlg.m_bLsVsLs))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("No item is to be tested!");
#else
		AfxMessageBox("û��Ҫ������Ŀ!");
#endif
		return;	
	}
	FILE *fp;
	fp = fopen("SolidCollide.txt","wt");
	g_pSolidDraw->ReleaseSnapStatus();
	CProcBarDlg *pProcDlg = NULL;
	pProcDlg = new CProcBarDlg(AfxGetApp()->m_pMainWnd);
	pProcDlg->Create();
	BOOL g_bBakDisplayAllHole=g_sysPara.bDisplayAllHole;
	g_sysPara.bDisplayAllHole=FALSE;	//ʵ����ʱ���ڲ���ʾ��˨�׵�״̬�£��ٶȿ�һЩ����
	
	int i=0;
	SCOPE_STRU scope;
#ifdef AFX_TARG_ENU_ENGLISH
    pProcDlg->SetTitle(CString("Preparing work before entity collision detection is ongoing!"));
#else
	pProcDlg->SetTitle(CString("���ڽ���ʵ����ײ���ǰ��׼������!"));
#endif
	n=Ta.Parts.GetNodeNum();
	CLDSPart *pPart;
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{	//�������ɲ�����˨�׵�ʵ��
		bool oldholestyle=pPart->m_bDisplayHole;
		if(pPart->GetClassTypeId()!=CLS_BOLT)
			pPart->m_bDisplayHole=false;
		pPart->SetModified();
		pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		pPart->m_bDisplayHole=oldholestyle;
		i++;
		pProcDlg->Refresh((100*i)/n);
	}
	pProcDlg->Refresh(100);
	CLDSBolt *pLs=NULL;
	//�����໥���ӵĹ�������ж�(�ֹ����,������)��δ����... �ֹܼа弰����ְ��
	//1.ͬ�˽ڵ�˼���ײ���
	if(solid_option_dlg.m_bJgVsJg)
	{
		i=0;
		n=Ta.Node.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
		pProcDlg->SetTitle(CString("the same end nodes bar collision detection progress!"));
#else
		pProcDlg->SetTitle(CString("ͬ�˽ڵ�˼���ײ������!"));
#endif
		pProcDlg->Refresh(0);
		CLDSPart *pPart=NULL,*pOthPart=NULL;
		CLDSLinePart *pLinePart=NULL,*pOthLinePart=NULL;
		for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			i++;
			for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
			{
				if(!pPart->IsLinePart())
					continue;
				pLinePart=(CLDSLinePart*)pPart;
				if(pLinePart->pStart!=pNode&&pLinePart->pEnd!=pNode)
					continue;	//�˽ڵ㲻Ϊ��ǰ�˼��Ķ˽ڵ�
				if(pLinePart->handle==pNode->hFatherPart)
					continue;	//�ڵ㸸�˼�����Ԥ�Ƚ�
				if(pLinePart->m_bVirtualPart)
					continue;	//����ָ˼�����˼���������ײ���
				if(pScope && !pScope->IsIntersWithLine(pLinePart->Start(),pLinePart->End()))
					continue;	//����ָ���ռ������ڵĸ˼�
				Ta.Parts.push_stack();
				for(pOthPart=Ta.Parts.GetNext();pOthPart;pOthPart=Ta.Parts.GetNext())
				{
					if(!pOthPart->IsLinePart())
						continue;
					pOthLinePart=(CLDSLinePart*)pOthPart;
					if(pOthLinePart->pStart!=pNode&&pOthLinePart->pEnd!=pNode)
						continue;	//�˽ڵ㲻Ϊ��ǰ�ǸֵĶ˽ڵ�
					if(pOthLinePart->handle==pNode->hFatherPart)
						continue;	//�ڵ㸸�Ǹֲ���Ԥ�Ƚ�
					if(pOthLinePart->m_bVirtualPart)
						continue;	//����ֽǸֺ���Ǹֲ�������ײ���
					if(!pLinePart->cfgword.And(pOthLinePart->cfgword))
						continue;	//����ͬһ��ģ���в��ؼ����ײ
					if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&pOthLinePart->GetClassTypeId()==CLS_LINEANGLE)
					{
						if(pLinePart->huoqu_handle>0X20&&pLinePart->huoqu_handle==pOthLinePart->huoqu_handle)//pJg->sub_type&TA_JG_HUOQUSEG&&pOthJg->sub_type&TA_JG_HUOQUSEG&&
							continue;	//ͬһ�������Ǹ��ϵĻ����ӽǸּ䲻�ж�
						//���������������޳��������Ǹ���ײ
						if(((CLDSLineAngle*)pLinePart)->pStart==pNode&&((CLDSLineAngle*)pLinePart)->desStartPos.datum_jg_h==((CLDSLineAngle*)pOthLinePart)->handle)
							continue;
						else if(((CLDSLineAngle*)pLinePart)->pEnd==pNode&&((CLDSLineAngle*)pLinePart)->desEndPos.datum_jg_h==((CLDSLineAngle*)pOthLinePart)->handle)
							continue;
						else if(((CLDSLineAngle*)pOthLinePart)->pStart==pNode&&((CLDSLineAngle*)pOthLinePart)->desStartPos.datum_jg_h==((CLDSLineAngle*)pLinePart)->handle)
							continue;
						else if(((CLDSLineAngle*)pOthLinePart)->pEnd==pNode&&((CLDSLineAngle*)pOthLinePart)->desEndPos.datum_jg_h==((CLDSLineAngle*)pLinePart)->handle)
							continue;
					}
					if(pScope && !pScope->IsIntersWithLine(pOthLinePart->Start(),pOthLinePart->End()))
						continue;	//����ָ���ռ������ڵĸ˼�
					if(pLinePart->m_hPartWeldParent==pOthLinePart->handle||pOthLinePart->m_hPartWeldParent==pLinePart->handle)
						continue;	//���˼�������һ�� ������ײ
					if(pLinePart->cfgword.And(pOthLinePart->cfgword))
					{	//���������ͬһ������
						if(pLinePart->pSolidBody->IsIntWith(pOthLinePart->pSolidBody))
						{	//���˼�����ײ
#ifdef AFX_TARG_ENU_ENGLISH
							CString sName="part",sOthName="part";
							if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
								sName="Angle";
							else if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
								sName="Tube";
							else if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
								sName="Slot";
							else if(pLinePart->GetClassTypeId()==CLS_LINEFLAT)
								sName="Flat";
							if(pOthLinePart->GetClassTypeId()==CLS_LINEANGLE)
								sOthName="Angle";
							else if(pOthLinePart->GetClassTypeId()==CLS_LINETUBE)
								sOthName="Tube";
							else if(pOthLinePart->GetClassTypeId()==CLS_LINESLOT)
								sOthName="Slot";
							else if(pOthLinePart->GetClassTypeId()==CLS_LINEFLAT)
								sOthName="Flat";
							fprintf(fp,"collision may occur between %d#0X%X%s and 0X%X%s\n",nInvalidHits+1,pLinePart->handle,sName,pOthLinePart->handle,sOthName);
#else
							CString sName="�˼�",sOthName="�˼�";
							if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
								sName="�Ǹ�";
							else if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
								sName="�ֹ�";
							else if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
								sName="�۸�";
							else if(pLinePart->GetClassTypeId()==CLS_LINEFLAT)
								sName="���";
							if(pOthLinePart->GetClassTypeId()==CLS_LINEANGLE)
								sOthName="�Ǹ�";
							else if(pOthLinePart->GetClassTypeId()==CLS_LINETUBE)
								sOthName="�ֹ�";
							else if(pOthLinePart->GetClassTypeId()==CLS_LINESLOT)
								sOthName="�۸�";
							else if(pOthLinePart->GetClassTypeId()==CLS_LINEFLAT)
								sOthName="���";
							fprintf(fp,"%d#0X%X%s��0X%X%s����ܷ�����ײ��\n",nInvalidHits+1,pLinePart->handle,sName,pOthLinePart->handle,sOthName);
#endif
							g_pSolidDraw->SetEntSnapStatus(pLinePart->handle);
							g_pSolidDraw->SetEntSnapStatus(pOthLinePart->handle);
							nInvalidHits++;
						}
					}
				}
				Ta.Parts.pop_stack();
			}
			pProcDlg->Refresh((100*i)/n);
		}
	}
	//2.�˼���˨��ײ���	//��...
	f3dPoint ls_pos;
	AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
	if(solid_option_dlg.m_bJgVsLs)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pProcDlg->SetTitle("progress is detected by the collision of bars and bolts!");
#else
		pProcDlg->SetTitle("�˼�����˨��ײ�����ȣ�");
#endif
		pProcDlg->Refresh(0);
		i=0; 
		n=Ta.Parts.GetNodeNum();
		CLDSPart *pPart=NULL;
		CLDSLinePart *pLinePart=NULL;
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			i++;
			f3dLine axis_line;
			if(!pPart->IsLinePart())
			{
				pProcDlg->Refresh((100*i)/n);
				continue;
			}
			pLinePart=(CLDSLinePart*)pPart;
			if(pLinePart->m_bVirtualPart)
				continue;	//����ֽǸֲ�������ײ���
			if(pScope && !pScope->IsIntersWithLine(pLinePart->Start(),pLinePart->End()))
				continue;	//����ָ���ռ������ڵĸ˼�
			if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{
				CLDSLineAngle *pJg=(CLDSLineAngle*)pLinePart;
				axis_line.startPt=pJg->Start()+pJg->GetWingVecX()*pJg->GetWidth()/2.0+pJg->GetWingVecY()*pJg->GetWidth()/2.0;
				axis_line.endPt=pJg->End()+pJg->GetWingVecX()*pJg->GetWidth()/2.0+pJg->GetWingVecY()*pJg->GetWidth()/2.0;
			}
			scope.ClearScope();
			int vertex_n=pLinePart->pSolidBody->KeyPointNum();
			//for(f3dPoint *pPt=pLinePart->pBody->vertex.GetFirst();pPt;pPt=pLinePart->pBody->vertex.GetNext())
			for(int vi=0;vi<vertex_n;vi++)
			{
				f3dPoint vertex=pLinePart->pSolidBody->GetKeyPointAt(vi);
				scope.VerifyVertex(vertex);
			}
			Ta.Parts.push_stack();
			for(pLs=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pLs;pLs=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
			{
				if(pLs->m_bVirtualPart)
					continue;	//��ͳ����˨��������ײ���
				ls_pos=pLs->ucs.origin;
				if(pScope && !pScope->IsIncludePoint(pLs->ucs.origin))
					continue;	//����ָ���ռ������ڵ���˨
				if(ls_pos.x<scope.fMinX-pLs->get_L())
					continue;
				else if(ls_pos.x>=scope.fMaxX+pLs->get_L())
					continue;
				else if(ls_pos.y<=scope.fMinY-pLs->get_L())
					continue;
				else if(ls_pos.y>=scope.fMaxY+pLs->get_L())
					continue;
				else if(ls_pos.z<=scope.fMinZ-pLs->get_L())
					continue;
				else if(ls_pos.z>=scope.fMaxZ+pLs->get_L())
					continue;
				if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
				{
					if(DistOf3dPtLine(ls_pos,axis_line.startPt,axis_line.endPt)>((CLDSLineAngle*)pLinePart)->GetWidth()+pLs->get_L())
						continue;
				}
				if(pLinePart->FindLsByHandle(pLs->handle))
					continue;	//��˨�ڸ˼���
				if(!pLinePart->cfgword.And(pLs->cfgword))
					continue;	//����ͬһ��ģ���в��ؼ����ײ
				if(pLinePart->pSolidBody->IsIntWith(pLs->pSolidBody))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					CString sName="Rod";
					if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
						sName="Angle";
					else if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
						sName="Tube";
					else if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
						sName="Slot";
					else if(pLinePart->GetClassTypeId()==CLS_LINEFLAT)
						sName="Flat";
					fprintf(fp, "collision may occur between %d#0X%X%s and 0X%X. \n",nInvalidHits+1,pLinePart->handle,sName,pLs->handle);
#else
					CString sName="�˼�";
					if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
						sName="�Ǹ�";
					else if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
						sName="�ֹ�";
					else if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
						sName="�۸�";
					else if(pLinePart->GetClassTypeId()==CLS_LINEFLAT)
						sName="���";
					fprintf(fp,"%d#0X%X%s��0X%X��˨���ܷ�����ײ��\n",nInvalidHits+1,pLinePart->handle,sName,pLs->handle);
#endif
					g_pSolidDraw->SetEntSnapStatus(pLinePart->handle);
					nInvalidHits++;
				}
			}
			Ta.Parts.pop_stack();
			pProcDlg->Refresh((100*i)/n);
		}
		pProcDlg->Refresh(100);
	}
	//3.����˨��ײ������
	if(solid_option_dlg.m_bLjbVsLs)
	{
		n=Ta.Parts.GetNodeNum(CLS_PLATE)+Ta.Parts.GetNodeNum(CLS_PARAMPLATE);
		i=0;
#ifdef AFX_TARG_ENU_ENGLISH
		pProcDlg->SetTitle("Progress is detected by the collision of steel plate and bolts!");
#else
		pProcDlg->SetTitle("������˨��ײ�����ȣ�");
#endif
		pProcDlg->Refresh(0);
		CSuperSmartPtr<CLDSPart>pPlate=NULL;
		for(pPlate=Ta.Parts.GetFirst();pPlate.IsHasPtr();pPlate=Ta.Parts.GetNext())
		{
			if(pPlate->GetClassTypeId()!=CLS_PLATE&&pPlate->GetClassTypeId()!=CLS_PARAMPLATE)
				continue;
			i++;
			if(pScope && !pScope->IsIncludePoint(pPlate->ucs.origin))
				continue;	//���㲻��ָ���ռ������ڵı߰�
			if(pPlate->GetClassTypeId()==CLS_PLATE)
			{
				if(pPlate.PlatePointer()->vertex_list.GetNodeNum()<3)
					continue;	//�ް�
			}
			else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
			{
				if(pPlate.ParamPlatePointer()->vertex_list.GetNodeNum()<3)
					continue;	//�ް�
			}
			scope.ClearScope();
			int vertex_n=pPlate->pSolidBody->KeyPointNum();
			//for(f3dPoint *pPt=pPlate->pBody->vertex.GetFirst();pPt;pPt=pPlate->pBody->vertex.GetNext())
			for(int vi=0;vi<vertex_n;vi++)
			{
				f3dPoint vertex=pPlate->pSolidBody->GetKeyPointAt(vi);
				scope.VerifyVertex(vertex);
			}
			Ta.Parts.push_stack();
			for(pLs=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pLs;pLs=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
			{
				if(pLs->m_bVirtualPart)
					continue;	//��ͳ����˨��������ײ���
				if(pScope && !pScope->IsIncludePoint(pLs->ucs.origin))
					continue;	//����ָ���ռ������ڵ���˨
				ls_pos=pLs->ucs.origin;
				if(ls_pos.x<scope.fMinX-pLs->get_L())
					continue;
				else if(ls_pos.x>=scope.fMaxX+pLs->get_L())
					continue;
				else if(ls_pos.y<=scope.fMinY-pLs->get_L())
					continue;
				else if(ls_pos.y>=scope.fMaxY+pLs->get_L())
					continue;
				else if(ls_pos.z<=scope.fMinZ-pLs->get_L())
					continue;
				else if(ls_pos.z>=scope.fMaxZ+pLs->get_L())
					continue;
				if(pPlate->GetClassTypeId()==CLS_PLATE)
				{
					if(pPlate.PlatePointer()->FindLsByHandle(pLs->handle))
						continue;
				}
				else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
				{
					if(pPlate.ParamPlatePointer()->FindLsByHandle(pLs->handle))
						continue;
				}
				if(!pPlate->cfgword.And(pLs->cfgword))
					continue;	//����ͬһ��ģ���в��ؼ����ײ
				if(pPlate->pSolidBody->IsIntWith(pLs->pSolidBody))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					CString sName="Plate";
					if(pPlate->GetClassTypeId()==CLS_PLATE)
						sName="Collecting Plate";
					else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
						sName="Param Plate";
					fprintf(fp,"collision may occur between %d#0X%X%s and 0X%X bolts. \n",nInvalidHits+1,pPlate->handle,sName,pLs->handle);
#else
					CString sName="��";
					if(pPlate->GetClassTypeId()==CLS_PLATE)
						sName="���Ӱ�";
					else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
						sName="��������";
					fprintf(fp,"%d#0X%X%s��0X%X��˨���ܷ�����ײ��\n",nInvalidHits+1,pPlate->handle,sName,pLs->handle);
#endif
					g_pSolidDraw->SetEntSnapStatus(pPlate->handle);
					nInvalidHits++;
				}
			}
			Ta.Parts.pop_stack();
			pProcDlg->Refresh((100*i)/n);
		}
		pProcDlg->Refresh(100);
	}
	//4.��˨����˨��ײ������
	if(solid_option_dlg.m_bLsVsLs)
	{
		n=Ta.Parts.GetNodeNum(CLS_BOLT);
		long nn=n*(n-1)/2;
		long ii=0;
		i=0;
#ifdef AFX_TARG_ENU_ENGLISH
		pProcDlg->SetTitle("collision progress between bolts and bolts!");
#else
		pProcDlg->SetTitle("��˨����˨��ײ�����ȣ�");
#endif
		pProcDlg->Refresh(0);
		for(pLs=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pLs;pLs=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
		{
			i++;
			if(pLs->m_bVirtualPart)
				continue;	//��ͳ����˨��������ײ���
			if(pScope && !pScope->IsIncludePoint(pLs->ucs.origin))
				continue;	//����ָ���ռ������ڵ���˨
			Ta.Parts.push_stack();
			for(CLDSBolt *pOthLs=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT);pOthLs;pOthLs=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
			{
				ii++;
				if(pLs->m_bVirtualPart)
					continue;	//��ͳ����˨��������ײ���
				if(pScope && !pScope->IsIncludePoint(pOthLs->ucs.origin))
					continue;	//����ָ���ռ������ڵ���˨
				//����
				if(fabs(pLs->get_norm()*pOthLs->get_norm())>EPS_COS2)
				{
					if(DISTANCE(pLs->ucs.origin,pOthLs->ucs.origin)>0.5*(pLs->get_d()+pOthLs->get_d()))
						continue;
				}
				else if(DISTANCE(pLs->ucs.origin,pOthLs->ucs.origin)>pLs->get_L()+pOthLs->get_L()+0.5*(pLs->get_d()+pOthLs->get_d()))
					continue;	//̫Զ�޼���Ҫ���϶�����
				if(!pLs->cfgword.And(pOthLs->cfgword))
					continue;	//����ͬһ��ģ���в��ؼ����ײ
				if(pLs->pSolidBody->IsIntWith(pOthLs->pSolidBody))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					fprintf(fp,"collision may occur between %d#0X%X bolts and 0X%X bolts.\n",nInvalidHits+1,pLs->handle,pOthLs->handle);
#else
					fprintf(fp,"%d#0X%X��˨��0X%X��˨���ܷ�����ײ��\n",nInvalidHits+1,pLs->handle,pOthLs->handle);
#endif
					nInvalidHits++;
				}
				pProcDlg->Refresh((100*ii)/nn);
			}
			Ta.Parts.pop_stack();
		}
	}
	//5.�˼������ײ������
	if(solid_option_dlg.m_bJgVsLjb)
	{
		n=Ta.Parts.GetNodeNum(CLS_PLATE)+Ta.Parts.GetNodeNum(CLS_PARAMPLATE);;
		i=0;
#ifdef AFX_TARG_ENU_ENGLISH
		pProcDlg->SetTitle("Collision progress between bars and plate!");
#else
		pProcDlg->SetTitle("�˼������ײ�����ȣ�");
#endif
		pProcDlg->Refresh(0);
		CSuperSmartPtr<CLDSPart>pPlate=NULL,pLinePart=NULL;
		for(pPlate=Ta.Parts.GetFirst();pPlate.IsHasPtr();pPlate=Ta.Parts.GetNext())
		{
			if(pPlate->GetClassTypeId()!=CLS_PLATE&&pPlate->GetClassTypeId()!=CLS_PARAMPLATE)
				continue;
			i++;
			if(pScope && !pScope->IsIncludePoint(pPlate->ucs.origin))
				continue;	//���㲻��ָ���ռ������ڵı߰�
			if(pPlate->GetClassTypeId()==CLS_PLATE)
			{
				if(pPlate.PlatePointer()->vertex_list.GetNodeNum()<3)
					continue;	//�ް�
			}
			else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
			{
				if(pPlate.ParamPlatePointer()->vertex_list.GetNodeNum()<3)
					continue;	//�ް�
			}
			SCOPE_STRU plate_scope;
			int vertex_n=pPlate->pSolidBody->KeyPointNum();
			//for(f3dPoint *pPt=pPlate->pBody->vertex.GetFirst();pPt;pPt=pPlate->pBody->vertex.GetNext())
			for(int vi=0;vi<vertex_n;vi++)
			{
				f3dPoint vertex=pPlate->pSolidBody->GetKeyPointAt(vi);
				plate_scope.VerifyVertex(vertex);
			}
			Ta.Parts.push_stack();
			for(pLinePart=Ta.Parts.GetFirst();pLinePart.IsHasPtr();pLinePart=Ta.Parts.GetNext())
			{
				if(!pLinePart->IsLinePart())
					continue;
				if(pLinePart->m_bVirtualPart)
					continue;	//����ֽǸֲ�������ײ���
				if(pScope && !pScope->IsIntersWithLine(pLinePart.LinePartPointer()->Start(),pLinePart.LinePartPointer()->End()))
					continue;	//������㲻��ָ���ռ������ڵĽǸ�
				if(!pPlate->cfgword.And(pLinePart->cfgword))
					continue;	//����ͬһ��ģ���в��ؼ����ײ
				BOOL bNotChk=FALSE;
				if(pPlate->GetClassTypeId()==CLS_PLATE&&pPlate.PlatePointer()->jdb_style==3)//�ֹܼа�
				{
					for(CDesignLjPartPara *pLjPartPara=pPlate.PlatePointer()->designInfo.partList.GetFirst();
						pLjPartPara;pLjPartPara=pPlate.PlatePointer()->designInfo.partList.GetNext())
					{
						if(pLjPartPara->hPart==pLinePart->handle)
						{
							bNotChk=TRUE; 
							break;	
						}
					}
				}	
				if(bNotChk)	//�ֹܼа������������Ϊ������ײ
					continue;
				if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
				{	//��������׼�ֹܵ���ײΪ������ײ
					long h=0;
					if(pPlate.ParamPlatePointer()->m_iParamType==TYPE_ELBOWPLATE)
						pPlate.ParamPlatePointer()->GetDesignItemValue('K',&h);
					if(pLinePart->handle==abs(h))
						continue;
				}
				bNotChk=FALSE;	//��ͬ�ǻ������ӵĽǸֺͰ�䲻�����ײ
				if(pPlate->GetClassTypeId()==CLS_PLATE)
				{
					for(CLsRef *pLsRef=pPlate.PlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pPlate.PlatePointer()->GetNextLsRef())
					{	
						if(pLinePart.LinePartPointer()->FindLsByHandle((*pLsRef)->handle))
						{
							bNotChk=TRUE;
							break;
						}
					}
				}
				else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
				{
					for(CLsRef *pLsRef=pPlate.ParamPlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pPlate.ParamPlatePointer()->GetNextLsRef())
					{	
						if(pLinePart.LinePartPointer()->FindLsByHandle((*pLsRef)->handle))
						{
							bNotChk=TRUE;
							break;
						}
					}
				}
				if(bNotChk)
					continue;	//����˼�������
				if(pPlate->m_hPartWeldParent==pLinePart->handle||pLinePart->m_hPartWeldParent==pPlate->handle)
					continue;	//����˼�������һ��
				SCOPE_STRU pole_scope;
				int vertex_n=pLinePart->pSolidBody->KeyPointNum();
				//for(f3dPoint *pPt=pLinePart->pBody->vertex.GetFirst();pPt;pPt=pLinePart->pBody->vertex.GetNext())
				for(int vi=0;vi<vertex_n;vi++)
				{
					f3dPoint vertex=pLinePart->pSolidBody->GetKeyPointAt(vi);
					pole_scope.VerifyVertex(vertex);
				}
				if(!plate_scope.IsIntersWith(pole_scope))
					continue;
				if(pPlate->pSolidBody->IsIntWith(pLinePart->pSolidBody))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					CString sName="bars",sOthName="plate";
					if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
						sName="Angle";
					else if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
						sName="Tube";
					else if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
						sName="Slot";
					else if(pLinePart->GetClassTypeId()==CLS_LINEFLAT)
						sName="Flat";
					if(pPlate->GetClassTypeId()==CLS_PLATE)
						sOthName="Collecting Plate";
					else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
						sOthName="Param Plate";
					fprintf(fp,"collision may occur between %d#0X%X%s and 0X%X%s .\n",nInvalidHits+1,pLinePart->handle,sName,pPlate->handle,sOthName);
#else
					CString sName="�˼�",sOthName="��";
					if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
						sName="�Ǹ�";
					else if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
						sName="�ֹ�";
					else if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
						sName="�۸�";
					else if(pLinePart->GetClassTypeId()==CLS_LINEFLAT)
						sName="���";
					if(pPlate->GetClassTypeId()==CLS_PLATE)
						sOthName="���Ӱ�";
					else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
						sOthName="������";
					fprintf(fp,"%d#0X%X%s��0X%X%s���ܷ�����ײ��\n",nInvalidHits+1,pLinePart->handle,sName,pPlate->handle,sOthName);
#endif
					g_pSolidDraw->SetEntSnapStatus(pPlate->handle);
					g_pSolidDraw->SetEntSnapStatus(pLinePart->handle);
					nInvalidHits++;
				}
			}
			Ta.Parts.pop_stack();
			pProcDlg->Refresh((100*i)/n);
		}
		pProcDlg->Refresh(100);
	}
	//6.�������ײ������
	if(solid_option_dlg.m_bLjbVsLjb)
	{
		n=Ta.Parts.GetNodeNum();
		i=0;
#ifdef AFX_TARG_ENU_ENGLISH
		pProcDlg->SetTitle("Collision progress between plate and plate!");
#else
		pProcDlg->SetTitle("�������ײ�����ȣ�");
#endif
		pProcDlg->Refresh(0);
		CSuperSmartPtr<CLDSPart>pPlate=NULL,pOthPlate=NULL;
		for(pPlate=Ta.Parts.GetFirst();pPlate.IsHasPtr();pPlate=Ta.Parts.GetNext())
		{
			i++;
			pProcDlg->Refresh((100*i)/n);
			if(pPlate->GetClassTypeId()!=CLS_PLATE&&pPlate->GetClassTypeId()!=CLS_PARAMPLATE)
				continue;
			if(pScope && !pScope->IsIncludePoint(pPlate->ucs.origin))
				continue;	//���㲻��ָ���ռ������ڵı߰�
			if(pPlate->GetClassTypeId()==CLS_PLATE)
			{
				if(pPlate.PlatePointer()->vertex_list.GetNodeNum()<3)
					continue;	//�ް�
			}
			else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
			{
				if(pPlate.ParamPlatePointer()->vertex_list.GetNodeNum()<3)
					continue;	//�ް�
			}
			SCOPE_STRU scope; 
			int vertex_n=pPlate->pSolidBody->KeyPointNum();
			//for(f3dPoint *pPt=pPlate->pBody->vertex.GetFirst();pPt;pPt=pPlate->pBody->vertex.GetNext())
			for(int vi=0;vi<vertex_n;vi++)
			{
				f3dPoint vertex=pPlate->pSolidBody->GetKeyPointAt(vi);
				scope.VerifyVertex(vertex);
			}
			Ta.Parts.push_stack();
			for(pOthPlate=Ta.Parts.GetNext();pOthPlate.IsHasPtr();pOthPlate=Ta.Parts.GetNext())
			{
				if(pOthPlate->GetClassTypeId()!=CLS_PLATE&&pOthPlate->GetClassTypeId()!=CLS_PARAMPLATE)
					continue;
				if(!pPlate->cfgword.And(pOthPlate->cfgword))
					continue;	//����ͬһ��ģ���в��ؼ����ײ
				else if(pPlate->m_hPartWeldParent==pOthPlate->handle||
					pPlate->m_hPartWeldParent==pOthPlate->m_hPartWeldParent||
					pPlate->handle==pOthPlate->m_hPartWeldParent)
					continue;	//ͬһ������ϼ��ڵİ岻���
				//�������������׼�ְ����ײΪ������ײ
				if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
				{
					long h=0,h1=0;
					if(pPlate.ParamPlatePointer()->m_iParamType==TYPE_CIRCOVERPLATE)
					{
						pPlate.ParamPlatePointer()->GetDesignItemValue('F',&h);
						pPlate.ParamPlatePointer()->GetDesignItemValue('G',&h1);
					}
					else if(pPlate.ParamPlatePointer()->m_iParamType==TYPE_CIRRIBPLATE||
						pPlate.ParamPlatePointer()->m_iParamType==TYPE_TUBERIBPLATE)
					{
						pPlate.ParamPlatePointer()->GetDesignItemValue('E',&h);		//��һ���׼�ְ���B��ΪE wht 11-05-13
						pPlate.ParamPlatePointer()->GetDesignItemValue('F',&h1);	//�ڶ����׼�ְ���C��ΪF wht 11-05-13
					}
					else if(pPlate.ParamPlatePointer()->m_iParamType==TYPE_ELBOWPLATE)
						pPlate.ParamPlatePointer()->GetDesignItemValue('L',&h);
					if(pOthPlate->handle==h||pOthPlate->handle==h1)
						continue;
				}
				if(pOthPlate->GetClassTypeId()==CLS_PARAMPLATE)
				{
					long h=0,h1=0;
					if(pOthPlate.ParamPlatePointer()->m_iParamType==TYPE_CIRCOVERPLATE)
					{
						pOthPlate.ParamPlatePointer()->GetDesignItemValue('F',&h);
						pOthPlate.ParamPlatePointer()->GetDesignItemValue('G',&h1);
					}
					else if(pOthPlate.ParamPlatePointer()->m_iParamType==TYPE_CIRRIBPLATE||
						pOthPlate.ParamPlatePointer()->m_iParamType==TYPE_TUBERIBPLATE)
					{
						pOthPlate.ParamPlatePointer()->GetDesignItemValue('E',&h);	//��һ���׼�ְ���B��ΪE wht 11-05-13
						pOthPlate.ParamPlatePointer()->GetDesignItemValue('F',&h1);	//�ڶ����׼�ְ���C��ΪF wht 11-05-13
					}
					else if(pOthPlate.ParamPlatePointer()->m_iParamType==TYPE_ELBOWPLATE)
						pOthPlate.ParamPlatePointer()->GetDesignItemValue('L',&h);
					if(pPlate->handle==h||pPlate->handle==h1)
						continue;
				} 
				//����˨�໥���ӵİ�Ϊ������ײ
				if(pOthPlate->GetClassTypeId()==CLS_PLATE)
				{
					if(pOthPlate.PlatePointer()->vertex_list.GetNodeNum()<3)
						continue;	//�ް�
					BOOL bNotChk=FALSE;	
					if(pPlate->GetClassTypeId()==CLS_PLATE)
					{
						for(CLsRef *pLsRef=pPlate.PlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pPlate.PlatePointer()->GetNextLsRef())
						{	
							if(pOthPlate.PlatePointer()->FindLsByHandle((*pLsRef)->handle))
							{
								bNotChk=TRUE;
								break;
							}
						}
					}
					else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
					{
						for(CLsRef *pLsRef=pPlate.ParamPlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pPlate.ParamPlatePointer()->GetNextLsRef())
						{	
							if(pOthPlate.PlatePointer()->FindLsByHandle((*pLsRef)->handle))
							{
								bNotChk=TRUE;
								break;
							}
						}
					} 
					if(bNotChk)
						continue;
				}
				else if(pOthPlate->GetClassTypeId()==CLS_PARAMPLATE)
				{
					if(pOthPlate.ParamPlatePointer()->vertex_list.GetNodeNum()<3)
						continue;	//�ް�
					BOOL bNotChk=FALSE;	
					if(pPlate->GetClassTypeId()==CLS_PLATE)
					{
						for(CLsRef *pLsRef=pPlate.PlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pPlate.PlatePointer()->GetNextLsRef())
						{	
							if(pOthPlate.ParamPlatePointer()->FindLsByHandle((*pLsRef)->handle))
							{
								bNotChk=TRUE;
								break;
							}
						}
					}
					else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
					{
						for(CLsRef *pLsRef=pPlate.ParamPlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pPlate.ParamPlatePointer()->GetNextLsRef())
						{	
							if(pOthPlate.ParamPlatePointer()->FindLsByHandle((*pLsRef)->handle))
							{
								bNotChk=TRUE;
								break;
							}
						}
					} 
					if(bNotChk)
						continue;
				}
				if(pScope && !pScope->IsIncludePoint(pOthPlate->ucs.origin))
					continue;	//���㲻��ָ���ռ������ڵı߰�
				SCOPE_STRU oth_scope;
				int vertex_n=pOthPlate->pSolidBody->KeyPointNum();
				//for(f3dPoint *pPt=pOthPlate->pBody->vertex.GetFirst();pPt;pPt=pOthPlate->pBody->vertex.GetNext())
				for(int vi=0;vi<vertex_n;vi++)
				{
					f3dPoint vertex=pOthPlate->pSolidBody->GetKeyPointAt(vi);
					oth_scope.VerifyVertex(vertex);
				}
				if(!scope.IsIntersWith(oth_scope))
					continue;	//���в������ཻ
				if(pOthPlate->m_hPartWeldParent==pPlate->handle||pOthPlate->handle==pPlate->m_hPartWeldParent)
					continue;	//���庸����һ��
				if(pPlate->pSolidBody->IsIntWith(pOthPlate->pSolidBody))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					CString sName="Plate",sOthName="plate";
					if(pPlate->GetClassTypeId()==CLS_PLATE)
						sName="Collecting Plate";
					else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
						sName="Param Plate";
					if(pOthPlate->GetClassTypeId()==CLS_PLATE)
						sOthName="Collecting Plate";
					else if(pOthPlate->GetClassTypeId()==CLS_PARAMPLATE)
						sOthName="Param Plate";
					fprintf(fp,"collision may occur between %d#0X%X{%s}%s and 0X%X{%s}%s .\n",nInvalidHits+1,pPlate->handle,(char*)pPlate->GetPartNo(),sName,pOthPlate->handle,(char*)pOthPlate->GetPartNo(),sOthName);
#else
					CString sName="��",sOthName="��";
					if(pPlate->GetClassTypeId()==CLS_PLATE)
						sName="���Ӱ�";
					else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
						sName="������";
					if(pOthPlate->GetClassTypeId()==CLS_PLATE)
						sOthName="���Ӱ�";
					else if(pOthPlate->GetClassTypeId()==CLS_PARAMPLATE)
						sOthName="������";
					fprintf(fp,"%d#0X%X{%s}%s��0X%X{%s}%s���ܷ�����ײ��\n",nInvalidHits+1,pPlate->handle,(char*)pPlate->GetPartNo(),sName,pOthPlate->handle,(char*)pOthPlate->GetPartNo(),sOthName);
#endif
					g_pSolidDraw->SetEntSnapStatus(pPlate->handle);
					g_pSolidDraw->SetEntSnapStatus(pOthPlate->handle);
					nInvalidHits++;
				}
			}
			Ta.Parts.pop_stack();
			pProcDlg->Refresh((100*i)/n);
		}
	}
	g_sysPara.bDisplayAllHole=g_bBakDisplayAllHole;
	if(pProcDlg!=NULL)
		delete pProcDlg;
	fclose(fp);
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->m_bDisplayHole&&g_sysPara.bDisplayAllHole)
			pPart->SetModified();
	}
	if(nInvalidHits>0)
		WinExec("notepad.exe SolidCollide.txt",SW_SHOW);
	else
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("solid model collision passed!");
#else
		MessageBox("ͨ������ʵ����ײ���!");
#endif
}
#endif
/* Ŀǰ������ѻ�����[����]->[��ײУ��]�˵�ȡ�� wjh-2015.3.7
void CLDSView::OnSolidCollideCheckScope() 
{
#ifdef __ANGLE_PART_INC_
	m_nPrevCommandID=ID_SOLID_COLLIDE_CHECK_SCOPE;
	m_sPrevCommandName="�ظ�ָ������ʵ����ײ���";
	static CDefScopeStruDlg scope_dlg;
	long *id_arr=NULL;
	long nn=g_pSolidSnap->GetLastSelectEnts(id_arr);
	SCOPE_STRU scope;
	scope.ClearScope();
	BOOL bFindObj = FALSE;
	for(int i=0;i<nn;i++)
	{
		CLDSDbObject* pObj = (CLDSDbObject*)console.FromHandle(id_arr[i],TRUE);
		if(pObj->GetClassTypeId()==CLS_NODE)
		{
			CLDSNode *pNode = (CLDSNode*)pObj;
			scope.VerifyVertex(pNode->ActivePosition());
			bFindObj = TRUE;
		}
		else if(pObj->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLDSLineAngle *pCommJg = (CLDSLineAngle*)pObj;
			scope.VerifyVertex(pCommJg->Start());
			scope.VerifyVertex(pCommJg->End());
			bFindObj = TRUE;
		}
	}
	if(bFindObj)
	{
		scope_dlg.m_fMinX = ftoi(scope.fMinX/100)*100;
		scope_dlg.m_fMinY = ftoi(scope.fMinY/100)*100;
		scope_dlg.m_fMinZ = ftoi(scope.fMinZ/100)*100;
		scope_dlg.m_fMaxX = ftoi(scope.fMaxX/100)*100;
		scope_dlg.m_fMaxY = ftoi(scope.fMaxY/100)*100;
		scope_dlg.m_fMaxZ = ftoi(scope.fMaxZ/100)*100;
	}
	
	if(scope_dlg.DoModal()==IDOK)
	{
		scope.fMinX = scope_dlg.m_fMinX;
		scope.fMinY = scope_dlg.m_fMinY;
		scope.fMinZ = scope_dlg.m_fMinZ;
		scope.fMaxX = scope_dlg.m_fMaxX;
		scope.fMaxY = scope_dlg.m_fMaxY;
		scope.fMaxZ = scope_dlg.m_fMaxZ;
		SolidCollideCheck(&scope);
	}
#endif
}
*/
void CLDSView::OnSolidCollideCheck() 
{
#ifdef __ANGLE_PART_INC_
	m_nPrevCommandID=ID_SOLID_COLLIDE_CHECK;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat solid model collision check";
#else
	m_sPrevCommandName="�ظ�ʵ����ײ���";
#endif
	SolidCollideCheck();
#endif
}

void CLDSView::OnHandoverPermission() 
{
	THANDLE *id_arr;
	CLDSDbObject *pObj;
	CBuffer buffer;
	CPoleAuthPermDlg pole_perm;
	CPartAuthPermDlg part_perm;
	int i,n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	BOOL bHasLinePart=FALSE;
	/*
		һ�����������Ѿ��޸ģ����޸�Ȩ��ǰ����ǿ����ǰ�����ݵ�����������������һ�ڵ������ɣ�������һ�˼���
		Ȼ�󽫽ڵ�Ȩ��ת������û�����ʱ��ǰ�����޴˽ڵ�Ȩ�ޣ�����ʱ�˽ڵ�Ҳ����浽Эͬ���ģ���˾ͳ�Ϊ������
	*/
	if(Ta.IsDataModified())
	{
		BeginWaitCursor();
		GetDocument()->SaveDataFile();
		Ta.SetDocModifiedFlag(FALSE);
		GetDocument()->SetModifiedFlag(FALSE);
		EndWaitCursor();
	}
	if(n>0)
	{
		for(i=0;i<n;i++)
		{
			pObj=(CLDSDbObject*)console.FromHandle(id_arr[i],TRUE);
			if(i==0)
			{
				part_perm.m_dwPermission=pObj->dwPermission;
				if(n>1||theApp.IsHasModifyPerm(pObj->dwPermission))
					part_perm.m_bEnableModify=TRUE;
				else
					part_perm.m_bEnableModify=FALSE;
			}
			if(!pObj->IsPart())
			{	//�ڵ�
				if(theApp.IsHasModifyPerm(pObj->dwPermission))
					part_perm.m_bEnableModify=TRUE;
				else
					part_perm.m_bEnableModify=FALSE;
				part_perm.m_dwPermission=pObj->dwPermission;
			}
			else
			{
				CLDSPart *pPart=(CLDSPart*)pObj;
				if(pPart->IsLinePart())
				{
					CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
					bHasLinePart=TRUE;
					if(n==1)
					{
						if(theApp.IsHasModifyPerm(pLinePart->dwStartPermission))
							pole_perm.m_bEnableStartModify=TRUE;
						else
							pole_perm.m_bEnableStartModify=FALSE;
						if(theApp.IsHasModifyPerm(pLinePart->dwPermission))
							pole_perm.m_bEnableBasicModify=TRUE;
						else
							pole_perm.m_bEnableBasicModify=FALSE;
						if(theApp.IsHasModifyPerm(pLinePart->dwEndPermission))
							pole_perm.m_bEnableEndModify=TRUE;
						else
							pole_perm.m_bEnableEndModify=FALSE;
					}
					pole_perm.m_dwStartPermission=pLinePart->dwStartPermission;
					pole_perm.m_dwBasicPermission=pLinePart->dwPermission;
					pole_perm.m_dwEndPermission=pLinePart->dwEndPermission;
					break;
				}
				else
				{
					if(theApp.IsHasModifyPerm(pObj->dwPermission))
						part_perm.m_bEnableModify=TRUE;
					else
						part_perm.m_bEnableModify=FALSE;
					part_perm.m_dwPermission=pObj->dwPermission;
				}
			}
		}
	}
	else
	{
		CFindPartDlg find_part;
		if(find_part.DoModal()!=IDOK)
			return;
		pObj=(CLDSDbObject*)console.FromHandle(find_part.m_nPartHandle,TRUE);
		if(pObj==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("the specified part is not found!");
#else
			AfxMessageBox("�Ҳ���ָ��������");
#endif
			return;
		}
		if(!pObj->IsPart())
		{	//�ڵ�
			if(theApp.IsHasModifyPerm(pObj->dwPermission))
				part_perm.m_bEnableModify=TRUE;
			else
				part_perm.m_bEnableModify=FALSE;
			part_perm.m_dwPermission=pObj->dwPermission;
		}
		else
		{
			CLDSPart *pPart=(CLDSPart*)pObj;
			if(pPart->IsLinePart())
			{
				bHasLinePart=TRUE;
				CLDSLinePart* pLinePart=(CLDSLinePart*)pObj;
				if(theApp.IsHasModifyPerm(pLinePart->dwStartPermission))
					pole_perm.m_bEnableStartModify=TRUE;
				else
					pole_perm.m_bEnableStartModify=FALSE;
				if(theApp.IsHasModifyPerm(pLinePart->dwPermission))
					pole_perm.m_bEnableBasicModify=TRUE;
				else
					pole_perm.m_bEnableBasicModify=FALSE;
				if(theApp.IsHasModifyPerm(pLinePart->dwEndPermission))
					pole_perm.m_bEnableEndModify=TRUE;
				else
					pole_perm.m_bEnableEndModify=FALSE;
				pole_perm.m_dwStartPermission=pLinePart->dwStartPermission;
				pole_perm.m_dwBasicPermission=pLinePart->dwPermission;
				pole_perm.m_dwEndPermission=pLinePart->dwEndPermission;
			}
			else
			{
				if(theApp.IsHasModifyPerm(pObj->dwPermission))
					part_perm.m_bEnableModify=TRUE;
				else
					part_perm.m_bEnableModify=FALSE;
				part_perm.m_dwPermission=pObj->dwPermission;
			}
		}
	}
	buffer.WriteInteger(0);
	if(bHasLinePart)
	{
		theApp.GetUserInfo(pole_perm.user_arr);
		if(pole_perm.DoModal()==IDOK)
		{
			if(n>0)
			{
				for(i=0;i<n;i++)
				{
					pObj=(CLDSDbObject*)console.FromHandle(id_arr[i],TRUE);
					/*if(pObj->GetClassTypeId()==CLS_LINEANGLE||pObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
					{
						CLDSLineAngle *pCommJg=(CLDSLineAngle*)pObj;
						if(theApp.IsHasModifyPerm(pCommJg->dwPermission|pCommJg->dwStartPermission|pCommJg->dwEndPermission))
						{
							if(theApp.IsHasModifyPerm(pCommJg->dwStartPermission))
								pCommJg->dwStartPermission=pole_perm.m_dwStartPermission;
							if(theApp.IsHasModifyPerm(pCommJg->dwPermission))
								pCommJg->dwPermission=pole_perm.m_dwBasicPermission;
							if(theApp.IsHasModifyPerm(pCommJg->dwEndPermission))
								pCommJg->dwEndPermission=pole_perm.m_dwEndPermission;
							buffer.WriteInteger(pCommJg->handle);
							buffer.WriteDword(pCommJg->dwPermission);
							buffer.WriteDword(pCommJg->dwStartPermission);
							buffer.WriteDword(pCommJg->dwEndPermission);
							if(pCommJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
								((CLDSGroupLineAngle*)pCommJg)->UpdateSonJgPerm();
							for(int j=0;j<4;j++)
							{
								CLDSLineAngle *pTmJg=(CLDSLineAngle*)console.FromPartHandle(((CLDSGroupLineAngle*)pCommJg)->son_jg_h[j],CLS_LINEANGLE);
								if(pTmJg)
								{
									buffer.WriteInteger(pTmJg->handle);
									buffer.WriteDword(pTmJg->dwPermission);
									buffer.WriteDword(pTmJg->dwStartPermission);
									buffer.WriteDword(pTmJg->dwEndPermission);
								}
							}
						}
						else if(theApp.IsHasModifyPerm(pObj->dwPermission))
						{
							pObj->dwPermission=pole_perm.m_dwBasicPermission;
							buffer.WriteInteger(pObj->handle);
							buffer.WriteDword(pObj->dwPermission);
							buffer.WriteDword(pObj->dwPermission);
							buffer.WriteDword(pObj->dwPermission);
						}
					}*/
					if(pObj->GetClassTypeId()==CLS_LINEANGLE||pObj->GetClassTypeId()==CLS_GROUPLINEANGLE||
						pObj->GetClassTypeId()==CLS_LINETUBE||pObj->GetClassTypeId()==CLS_LINEFLAT||pObj->GetClassTypeId()==CLS_LINESLOT)
					{
						CLDSLinePart *pLinePart=(CLDSLinePart*)pObj;
						if(theApp.IsHasModifyPerm(pLinePart->dwPermission|pLinePart->dwStartPermission|pLinePart->dwEndPermission))
						{
							if(theApp.IsHasModifyPerm(pLinePart->dwStartPermission))
								pLinePart->dwStartPermission=pole_perm.m_dwStartPermission;
							if(theApp.IsHasModifyPerm(pLinePart->dwPermission))
								pLinePart->dwPermission=pole_perm.m_dwBasicPermission;
							if(theApp.IsHasModifyPerm(pLinePart->dwEndPermission))
								pLinePart->dwEndPermission=pole_perm.m_dwEndPermission;
							buffer.WriteInteger(pLinePart->handle);
							buffer.WriteDword(pLinePart->dwPermission);
							buffer.WriteDword(pLinePart->dwStartPermission);
							buffer.WriteDword(pLinePart->dwEndPermission);
#ifdef __ANGLE_PART_INC_
							if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
							{
								CLDSGroupLineAngle* pGroupLineAngle=(CLDSGroupLineAngle*)pLinePart;
								pGroupLineAngle->UpdateSonJgPerm();
								for(int j=0;j<4;j++)
								{
									CLDSLineAngle *pTmJg=(CLDSLineAngle*)console.FromPartHandle(pGroupLineAngle->son_jg_h[j],CLS_LINEANGLE);
									if(pTmJg)
									{
										buffer.WriteInteger(pTmJg->handle);
										buffer.WriteDword(pTmJg->dwPermission);
										buffer.WriteDword(pTmJg->dwStartPermission);
										buffer.WriteDword(pTmJg->dwEndPermission);
									}
								}
							}
#endif
						}
						else if(theApp.IsHasModifyPerm(pObj->dwPermission))
						{
							pObj->dwPermission=pole_perm.m_dwBasicPermission;
							buffer.WriteInteger(pObj->handle);
							buffer.WriteDword(pObj->dwPermission);
							buffer.WriteDword(pObj->dwPermission);
							buffer.WriteDword(pObj->dwPermission);
						}
					}
					else if(theApp.IsHasModifyPerm(pObj->dwPermission))
					{
						pObj->dwPermission=pole_perm.m_dwBasicPermission;
						buffer.WriteInteger(pObj->handle);
						buffer.WriteDword(pObj->dwPermission);
						buffer.WriteDword(pObj->dwPermission);
						buffer.WriteDword(pObj->dwPermission);
					}
				}
			}
			else
			{
				/*if(pObj->GetClassTypeId()==CLS_LINEANGLE||pObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
				{
					CLDSLineAngle *pCommJg=(CLDSLineAngle*)pObj;
					if(theApp.IsHasModifyPerm(pCommJg->dwPermission|pCommJg->dwStartPermission|pCommJg->dwEndPermission))
					{
						if(theApp.IsHasModifyPerm(pCommJg->dwStartPermission))
							pCommJg->dwStartPermission=pole_perm.m_dwStartPermission;
						if(theApp.IsHasModifyPerm(pCommJg->dwPermission))
							pCommJg->dwPermission=pole_perm.m_dwBasicPermission;
						if(theApp.IsHasModifyPerm(pCommJg->dwEndPermission))
							pCommJg->dwEndPermission=pole_perm.m_dwEndPermission;
						buffer.WriteInteger(pCommJg->handle);
						buffer.WriteDword(pCommJg->dwPermission);
						buffer.WriteDword(pCommJg->dwStartPermission);
						buffer.WriteDword(pCommJg->dwEndPermission);
						if(pCommJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
							((CLDSGroupLineAngle*)pCommJg)->UpdateSonJgPerm();
						for(int j=0;j<4;j++)
						{
							CLDSLineAngle *pTmJg=(CLDSLineAngle*)console.FromPartHandle(((CLDSGroupLineAngle*)pCommJg)->son_jg_h[j],CLS_LINEANGLE);
							if(pTmJg)
							{
								buffer.WriteInteger(pTmJg->handle);
								buffer.WriteDword(pTmJg->dwPermission);
								buffer.WriteDword(pTmJg->dwStartPermission);
								buffer.WriteDword(pTmJg->dwEndPermission);
							}
						}
					}
				}*/
				if(pObj->GetClassTypeId()==CLS_LINEANGLE||pObj->GetClassTypeId()==CLS_GROUPLINEANGLE||
						pObj->GetClassTypeId()==CLS_LINETUBE||pObj->GetClassTypeId()==CLS_LINEFLAT||pObj->GetClassTypeId()==CLS_LINESLOT)
				{
					CLDSLinePart *pLinePart=(CLDSLinePart*)pObj;
					if(theApp.IsHasModifyPerm(pLinePart->dwPermission|pLinePart->dwStartPermission|pLinePart->dwEndPermission))
					{
						if(theApp.IsHasModifyPerm(pLinePart->dwStartPermission))
							pLinePart->dwStartPermission=pole_perm.m_dwStartPermission;
						if(theApp.IsHasModifyPerm(pLinePart->dwPermission))
							pLinePart->dwPermission=pole_perm.m_dwBasicPermission;
						if(theApp.IsHasModifyPerm(pLinePart->dwEndPermission))
							pLinePart->dwEndPermission=pole_perm.m_dwEndPermission;
						buffer.WriteInteger(pLinePart->handle);
						buffer.WriteDword(pLinePart->dwPermission);
						buffer.WriteDword(pLinePart->dwStartPermission);
						buffer.WriteDword(pLinePart->dwEndPermission);
#ifdef __ANGLE_PART_INC_
						if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
						{
							CLDSGroupLineAngle* pGroupLineAngle=(CLDSGroupLineAngle*)pLinePart;
							pGroupLineAngle->UpdateSonJgPerm();
							for(int j=0;j<4;j++)
							{
								CLDSLineAngle *pTmJg=(CLDSLineAngle*)console.FromPartHandle(pGroupLineAngle->son_jg_h[j],CLS_LINEANGLE);
								if(pTmJg)
								{
									buffer.WriteInteger(pTmJg->handle);
									buffer.WriteDword(pTmJg->dwPermission);
									buffer.WriteDword(pTmJg->dwStartPermission);
									buffer.WriteDword(pTmJg->dwEndPermission);
								}
							}
						}
#endif
					}
					else if(theApp.IsHasModifyPerm(pObj->dwPermission))
					{
						pObj->dwPermission=pole_perm.m_dwBasicPermission;
						buffer.WriteInteger(pObj->handle);
						buffer.WriteDword(pObj->dwPermission);
						buffer.WriteDword(pObj->dwPermission);
						buffer.WriteDword(pObj->dwPermission);
					}
				}
				else if(theApp.IsHasModifyPerm(pObj->dwPermission))
				{
					pObj->dwPermission=pole_perm.m_dwBasicPermission;
					buffer.WriteInteger(pObj->handle);
					buffer.WriteDword(pObj->dwPermission);
					buffer.WriteDword(pObj->dwPermission);
					buffer.WriteDword(pObj->dwPermission);
				}
			}
		}
		else
			return;
	}
	else
	{
		theApp.GetUserInfo(part_perm.user_arr);
		if(part_perm.DoModal()==IDOK)
		{
			if(n>0)
			{
				for(i=0;i<n;i++)
				{
					pObj=(CLDSDbObject*)console.FromHandle(id_arr[i],TRUE);
					if(pObj&&theApp.IsHasModifyPerm(pObj->dwPermission))
					{
						pObj->dwPermission=part_perm.m_dwPermission;
						buffer.WriteInteger(pObj->handle);
						buffer.WriteDword(pObj->dwPermission);
						buffer.WriteDword(pObj->dwPermission);
						buffer.WriteDword(pObj->dwPermission);
					}
				}
			}
			else if(theApp.IsHasModifyPerm(pObj->dwPermission))
			{
				pObj->dwPermission=part_perm.m_dwPermission;
				buffer.WriteInteger(pObj->handle);
				buffer.WriteDword(pObj->dwPermission);
				buffer.WriteDword(pObj->dwPermission);
				buffer.WriteDword(pObj->dwPermission);
			}
		}
		else
			return;
	}
	
	DWORD dwWritten;
	LDS_MSGHEAD msg_head;
	msg_head.command_id=ASK_PERM_MODIFY_IN;
	msg_head.src_code=(BYTE)theApp.m_nUserId;
	msg_head.msg_length=0;
	WriteFile(theApp.m_hPipeServerListen,&msg_head,9,&dwWritten,NULL);
	buffer.SeekToBegin();//���¼��㲢д�����Ȩ�޵Ĺ���
	buffer.WriteInteger((buffer.GetLength()-4)/16);
	BYTE memory_pool[PACK_SIZE];
	//д����Ϣ�����ݲ�����
	buffer.SeekToBegin();
	while(buffer.GetRemnantSize()>PACK_SIZE-1)
	{
		buffer.Read(&memory_pool,PACK_SIZE-1);
		memory_pool[PACK_SIZE-1]=1;	//�������
		WriteFile(theApp.m_hPipeServerListen,memory_pool,PACK_SIZE,&dwWritten,NULL);
	}
	buffer.Read(memory_pool,buffer.GetRemnantSize());
	memory_pool[PACK_SIZE-1]=0;		//���ݽ���
	WriteFile(theApp.m_hPipeServerListen,memory_pool,PACK_SIZE,&dwWritten,NULL);
}

void CLDSView::OnCalJgWingZhunIntersPt() 
{
#ifdef __ANGLE_PART_INC_
	m_nPrevCommandID=ID_CAL_POLE_INTERS_PT;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat the rods center line intersection point";
#else
	m_sPrevCommandName="�ظ�����˼����߽���";
#endif
	m_curTask = TASK_CAL_2POLE_INTERS_PT;
	g_pSolidSet->SetOperType(OPER_OSNAP);
	g_pSolidSnap->SetSnapType(SNAP_LINE);
	m_nObjectSnapedCounts=0;
	m_setSnapObjects.Empty();
#ifdef AFX_TARG_ENU_ENGLISH
    g_pPromptMsg->SetMsg("Please choose two rods��and the first choice for datum rod!");
#else
	g_pPromptMsg->SetMsg("������ѡ���������������߽���ĸ˼�����ѡ��Ϊ��׼�˼�!");
#endif
#endif
}

void CLDSView::OnReturnToAssembleEnv() 
{
#ifdef __ANGLE_PART_INC_
	((CMainFrame*)AfxGetMainWnd())->ExitPartApp();
	ShiftApplication(TA_ASSEMBLE_ENV);
#endif
}

void CLDSView::OnRemoveKaiheJiao() 
{
#ifdef __ANGLE_PART_INC_
	m_nPrevCommandID=ID_REMOVE_KAIHE_JIAO;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat to remove angle open-close";
#else
	m_sPrevCommandName="�ظ�ȥ���Ǹֿ��Ͻ�";
#endif
	long n,*id_arr=NULL;
	CLDSDbObject *pObj;
	n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	BOOL bUpdate=FALSE;
	Ta.BeginUndoListen();
	for(int i=0;i<n;i++)
	{
		pObj = (CLDSDbObject*)console.FromHandle(id_arr[i],TRUE);
		if(pObj==NULL)
			continue;
		if(pObj->GetClassTypeId()==CLS_LINEANGLE||pObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSLineAngle *pCommJg=(CLDSLineAngle*)pObj;
			pCommJg->RemoveKaiHeJiao();
			bUpdate=TRUE;
			if(pCommJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pCommJg;
				pGroupJg->UpdateSonJgWingNorm();
			}
		}
	}
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
	if(bUpdate)
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("Angle open-close remove successfully!");
	else
		AfxMessageBox("No selected suitable angle to remove open-close!");
#else
		MessageBox("�Ǹֿ��Ͻ��Ƴ��ɹ���");
	else
		AfxMessageBox("û��ѡ�к��ʵĽǸֽ����Ƴ����Ͻǣ�");
#endif
#endif
}

void CLDSView::OnAngleFaceOffsetOddDiff() 
{
#ifdef __ANGLE_PART_INC_
	m_nPrevCommandID=ID_ANGLE_FACE_OFFSET_ODD_DIFF;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat to calculate angle planar location ends";
#else
	m_sPrevCommandName="�ظ��Ǹ�ƽ������ͷ����";
#endif
	g_pSolidSet->SetOperType(OPER_OSNAP);
	g_pSolidSnap->SetSnapType(SNAP_LINE);
	m_curTask = TASK_CAL_ANGLE_FACEOFFSET_ODD_DIFF;
	m_nObjectSnapedCounts=0;
	m_setSnapObjects.Empty();
#ifdef AFX_TARG_ENU_ENGLISH
    g_pPromptMsg->SetMsg("Please select the angle to be calculated the planar location X&Y leg center line ends!");
#else
	g_pPromptMsg->SetMsg("��ѡ��Ҫ��Ҫ����Ǹ�ƽ����֫��������ͷ���ĽǸ�!");
#endif
#endif
}

void CLDSView::OnSaveAsNewView() //����Ϊһ����ͼ, ����Ϊ��ǰ���ͼ
{
	CTowerTreeDlg *pTreeView = (CTowerTreeDlg*)((CMainFrame*)AfxGetMainWnd())->GetTowerTreePage();
	CTreeCtrl* pTreeCtrl = pTreeView->GetTreeCtrl();
	HTREEITEM hRootItem = pTreeCtrl->GetRootItem();
	HTREEITEM hModelItem = pTreeCtrl->GetChildItem(hRootItem);
	HTREEITEM hViewGroupItem = pTreeCtrl->GetChildItem(hModelItem);
	HTREEITEM hCurItem = pTreeCtrl->GetChildItem(hViewGroupItem);
	hCurItem = pTreeCtrl->GetNextSiblingItem(hCurItem);
	pTreeView->GetTreeCtrl()->SelectItem(hCurItem);
	pTreeView->NewItem();
}

void CLDSView::SaveCursorView()
{
	CDisplayView *pView=console.GetActiveView();
	if(pView==NULL||pView->m_iViewType!=1)
		return;
	CViewPart *pViewPart;
	CHashTable<CViewPart*>hashViews(pView->GetViewPartCount());
	for(pViewPart=pView->GetFirstViewPart();pViewPart;pViewPart=pView->GetNextViewPart())
	{
		hashViews.SetValueAt(pViewPart->handle,pViewPart);
		pViewPart->handle=-pViewPart->handle;	//��Ϊ�Ƴ����
	}
	for(CLDSPart *pPart = console.DispPartSet.GetFirst(); pPart; pPart = console.DispPartSet.GetNext())
	{
		//����ʵ���о���ȥ����Ҳ��Ҫ���,��������ʾ���п�ʱ,���ص���˨�ᱻ������Ƴ���ǰ��ͼ
		//if(!pPart->is_visible)
		//	continue;		//��ȥ���������
		if(hashViews.GetValueAt(pPart->handle,pViewPart))
		{
			pViewPart->handle=pPart->handle;
			pViewPart->idClassType=pPart->GetClassTypeId();
		}
		else	//����ӹ���
		{
			pViewPart=pView->AppendViewPart(pPart->handle);	//����Ƴ����
			pViewPart->idClassType=pPart->GetClassTypeId();
		}
	}
	for(CLDSNode *pNode = console.DispNodeSet.GetFirst(); pNode; pNode = console.DispNodeSet.GetNext())
	{
		//if(!pNode->is_visible)
		//	continue;		//��ȥ���������
		if(hashViews.GetValueAt(pNode->handle,pViewPart))
		{
			pViewPart->handle=pNode->handle;
			pViewPart->idClassType=pNode->GetClassTypeId();
		}
		else //����ӹ���
		{
			pViewPart=pView->AppendViewPart(pNode->handle);
			pViewPart->idClassType=pNode->GetClassTypeId();
		}
	}
	for(pViewPart=pView->GetFirstViewPart();pViewPart;pViewPart=pView->GetNextViewPart())
	{
		if(pViewPart->handle<0)	//���Ƴ�����
			pView->DeletCursorViewPart();
	}
	pView->CleanPartSet();
	g_pSolidDraw->BuildDisplayList();
}

void CLDSView::OnFileNew() 
{
	Command("NewTower");
}

void CLDSView::OnFileNewTowerModel()
{
	Command("NewTower");
}

void CLDSView::OnFileNewSubstation()
{
	Command("NewSubstation");
}

void CLDSView::UpdatePropertyPage(CLDSObject *pObj/*=NULL*/,BOOL bFilterDisplay/*=FALSE*/,HWND hFocusWnd/*=NULL*/)
{
	long *id_arr,id_num;
	SUB_OBJ* pSubObj;
	prompt.Close();
	if(!g_sysPara.dock.pageProp.bDisplay)
		return;
	CLogErrorLife life;
	//TODO: �˴�������ҪΪ���ѡ�а�ʱ��ʾ�������ߺͶ�����ţ����ǻᵼ�¹����༭����ѡ�����ʱ��ʧ�ְ�50|100�ȱ�� wjh 2013.11.30
	if(m_curAppType==TA_ASSEMBLE_ENV)
		g_pSolidDraw->DestroyAllMark();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	int iDisplayType = -1;		//-1-��������һ�� ����-1Ϊ�������Ͳ�һ��(0-CLDSDbObject 1-CLDSPart 2-CLDSLinePart 3-CLDSArcPart)
	bool bHasFemElem=false;		//ӵ�е�Ԫʱ���ų���������������
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	if(pPropList->IsLocked())
		return;	//��������������״̬�������
	pPropList->HideInputCtrl();
	//ʹ���Կ�ʧȥ���� ����ˢ��ʱ�ᱨ��
	if(hFocusWnd!=NULL)
	{	//�ⲿָ�����㴰�ھ��(��˨���á��������ҵȷ�ģ̬�Ի����ڵ���ʱʹ��) wht 15-04-16
		HWND hCurFocusWnd=::GetFocus();
		CWnd *pWnd=FromHandle(hFocusWnd),*pCurFocusWnd=FromHandle(hCurFocusWnd);
		if(pCurFocusWnd==NULL||(pWnd&&!pWnd->IsChild(pCurFocusWnd)))
			::SetFocus(hFocusWnd);	//��ǰ���㴰��ΪhFocusWnd��hFocusWnd�Ӵ���ʱ�������ظ����ý��㴰��
	}
	else
		::SetFocus(m_hWnd);
	try{
	pPropDlg->SetPropHelpToTextMode();	//������������ʾ��Ϊ�ı�ģʽ wht 15-03-09
	selectObjs.Empty();
	selectSubObjs.Empty();
	hashSelectObjs.Empty();
	if(bFilterDisplay)
	{	//�޸Ĺ����������ڴ˴����¹������� wht 10-11-12
		CLDSObject *pPrevObj=NULL;
		g_pSolidDraw->ReleaseSnapStatus();	//��������ѡ�й���Ϊδѡ��״̬
		for(pObj=selectObjsBackup.GetFirst();pObj;pObj=selectObjsBackup.GetNext())
		{
			if((pObj->GetClassTypeId()==CLS_PARAMPLATE||pObj->GetClassTypeId()==CLS_PLATE)
				&&(int)pPropDlg->m_curClsTypeId==-1)	
				selectObjs.SetValue(pObj->handle,pObj);	//ѡ�иְ������Ҫ�����ͨ�ְ��Լ��������ְ�
			else if(pObj->GetClassTypeId()==pPropDlg->m_curClsTypeId||(int)pPropDlg->m_curClsTypeId==0)
				selectObjs.SetValue(pObj->handle,pObj);
			else if(pObj->IsDbObject()&&((CLDSDbObject*)pObj)->IsPart()&&pPropDlg->m_curClsTypeId==CLS_PART)
				selectObjs.SetValue(pObj->handle,pObj);	//����
			else if(pObj->IsDbObject()&&((CLDSDbObject*)pObj)->IsPart()
				&&((CLDSPart*)pObj)->IsArcPart()&&pPropDlg->m_curClsTypeId==CLS_ARCPART)
				selectObjs.SetValue(pObj->handle,pObj);	//���θ˼�
			else if(pObj->IsDbObject()&&((CLDSDbObject*)pObj)->IsPart()
				&&((CLDSPart*)pObj)->IsLinePart()&&pPropDlg->m_curClsTypeId==CLS_LINEPART)
				selectObjs.SetValue(pObj->handle,pObj);	//�˼�
			else 
				continue;	//���˵���ǰ����Ҫ��ʾ�Ķ���
			//������Ҫ��ʾ���ԵĹ���Ϊѡ��״̬
			g_pSolidDraw->SetEntSnapStatus(pObj->handle,TRUE);
			//��ǰ��Ҫ��ʾ�Ķ����д������Ͳ�һ�µĶ���ʱ����ʾ�乲������
			if(pPrevObj&&pPrevObj->GetClassTypeId()!=pObj->GetClassTypeId()) 
			{
				if(!pObj->IsDbObject())
				{
					selectObjs.Empty();
					selectObjsBackup.Empty();
					break;
				}
				iDisplayType = 0;	//��ʾ��������
			}
			pPrevObj=pObj;
		}
	}
	else
	{
		selectObjsBackup.Empty();
		if(pObj==NULL)
		{
			id_num = g_pSolidSnap->GetLastSelectEnts(id_arr);	
			CLDSObject *pPrevObj=NULL;
			Ta.ReBuildObjsHashTable();
			for(int i=0;i<id_num;i++)
			{
				if(id_arr[i]>0|| m_curAppType == TA_PART_EDITOR)
				{	//m_curAppType == TA_PART_EDITORΪ��ʱ�������༭���а���������ʾ��������δ��Ӧ��SUB_OBJ��PROP_OBJ������һ�� wjh-2019.7.22
					CLDSObject *pObj=NULL;
					SUBID subid=SUBID(id_arr[i]);
					BYTE cGroup=(BYTE)subid.Group();
					if(cGroup==SUBID::GROUP_DEFAULT) //Ĭ��ģʽ
					{
						if(id_arr[i]>0)	//������ɹ���
							pObj=console.FromHandle(id_arr[i]);
#ifdef __PART_DESIGN_INC_
						else if(id_arr[i]!=0)
						{	//��ֳ����ĵ�Ԫ
							long id=id_arr[i]^0x80000000;	//���ʵ�ʵ�Ԫ
							if(id-1>=0&&id-1<Ta.ElemPtrArr.GetSize())
								pObj=Ta.ElemPtrArr[id-1];
						}
#endif
						if(pObj==NULL)
						{
							CBlockReference* pAssmBlock=Ta.BlockRef.FromHandle(id_arr[i]);
							if(pAssmBlock!=NULL)
							{
								selectObjs.Empty();
								selectObjs.SetValue(pAssmBlock->handle,pAssmBlock);
								DisplayBlockReferenceProperty(pPropList->m_nObjClassTypeId!=CLS_BLOCKREF);
							}
							else
								DisplaySysSettingProperty(pPropList->m_nObjClassTypeId!=0);
							return;
						}
						if(pPrevObj&&pPrevObj->GetClassTypeId()!=pObj->GetClassTypeId()) //�������Ͳ�һ��ʱ����ʾ�乲������
						{
							if(!pObj->IsDbObject())
							{
								//selectObjs.Empty(); //wjh 2012.5.26
								//selectObjsBackup.Empty();
								//break;
							}
							iDisplayType = 0;	//��ʾ��������
						}
						if(pObj->GetClassTypeId()==CLS_FEMELEMENT)
							bHasFemElem=true;	//�������޵�Ԫ
						selectObjs.SetValue(pObj->Id,pObj);
						selectObjsBackup.append(pObj);
						pPrevObj=pObj;
					}
					else if(cGroup==SUBID::GROUP_BOUNDARY)	//���������α߿�
					{
						pSubObj=Ta.GeEnts.GetValue(subid.Id());
						selectSubObjs.append(pSubObj);
					}
				}
				else
				{
					XHOBJ_HANDLER handler;
					g_pSolidDraw->GetSolidObject(id_arr[i],&handler);
					if(handler.ciXhObjType==PROP_OBJ::TYPE_HOLEWALL)
					{
						CLDSPart* pPart=console.FromPartHandle(handler.hiberid.masterId);
						if(pPart&&pPart->IsBolt())
						{
							PROP_OBJ* pPropObj=hashSelectObjs.Add(id_arr[i]);
							pPropObj->ciObjType=PROP_OBJ::TYPE_HOLEWALL;
							pPropObj->pBolt=(CLDSBolt*)pPart;
							pPropObj->hResidePart=handler.hBoltResidePart;
						}
					}
					else if(handler.ciXhObjType==PROP_OBJ::TYPE_WIREPOINTARROW)
					{	//�ҵ��ͷ����
						CXhWirePoint* pWirePoint=gxWirePointModel.m_hashWirePoints.GetValue(handler.hiberid.masterId);
						if(pWirePoint)
						{
							PROP_OBJ* pPropObj=hashSelectObjs.Add(id_arr[i]);
							pPropObj->ciObjType=PROP_OBJ::TYPE_WIREPOINTARROW;
							pPropObj->obj.subobj.hObj=pWirePoint->Id;
						}
					}
				}
			}
		}
		else
		{
			selectObjs.SetValue(pObj->Id,pObj);
			selectObjsBackup.append(pObj);
		}
	}
	if(iDisplayType==0)	//�������Ͳ�һ�µ�����½����ж�
	{
		for(CLDSObject *pObj=selectObjs.GetFirst();pObj;pObj=selectObjs.GetNext())
		{
			if(bHasFemElem&&pObj->GetClassTypeId()!=CLS_FEMELEMENT)
			{
				selectObjs.DeleteCursor();
				continue;
			}
			if(!bHasFemElem)
			{
				if(!((CLDSDbObject*)pObj)->IsPart())
				{
					iDisplayType = 0;
					break;
				}
				if(!((CLDSPart*)pObj)->IsLinePart()&&!((CLDSPart*)pObj)->IsArcPart())
					iDisplayType = 1;	//CLDSPart
				if(iDisplayType!=1)
				{	//ֱ�߹������߻��ι���
					if(!((CLDSPart*)pObj)->IsArcPart())
					{
						if(iDisplayType==3)
							iDisplayType=1;
						else
							iDisplayType=2;	//CLDSLinePart
					}
					else //if(!((CLDSPart*)pObj)->IsLinePart())
					{
						if(iDisplayType==2)
							iDisplayType=1;
						else
							iDisplayType=3;	//CLDSArcPart
					}
				}
			}
		}
	}
	if(selectObjs.GetNodeNum()==0 && selectSubObjs.GetNodeNum()==0)
	{
		if(hashSelectObjs.GetNodeNum()>0)
		{
			PROP_OBJ* pPropObj=hashSelectObjs.GetFirst();
			for(PROP_OBJ* pIterPropObj=hashSelectObjs.GetFirst();pIterPropObj!=NULL;pIterPropObj=hashSelectObjs.GetNext())
			{
				if(pIterPropObj->ciObjType!=PROP_OBJ::TYPE_HOLEWALL)
					continue;
				if (pIterPropObj->pBolt)	//ѡ��ʵ����˨�ױڣ�������ʾ��˨����
				{
					selectObjs.SetValue(pIterPropObj->pBolt->handle, pIterPropObj->pBolt);
					if (pPropObj->ciObjType != PROP_OBJ::TYPE_HOLEWALL)
						pPropObj = pIterPropObj;
				}
			}
			if(pPropObj->ciObjType==PROP_OBJ::TYPE_HOLEWALL)
			{
				if (pPropObj->pBolt)
				{	//ѡ��ʵ����˨�ױڣ�������ʾ��˨����
					selectObjs.SetValue(pPropObj->pBolt->handle, pPropObj->pBolt);
					DisplayBoltProperty(pPropList->m_nObjClassTypeId != CLS_BOLT);
				}
				else
					DisplayHoleWallProperty(TRUE);
			}
			else if(pPropObj->ciObjType==PROP_OBJ::TYPE_WIREPOINTARROW)
				DisplayWirePointProperty(TRUE);
		}
		else
		{
			if(m_curAppType==TA_PART_EDITOR)
				DisplayWorkViewProperty();
			else
				DisplaySysSettingProperty(pPropList->m_nObjClassTypeId!=0);
		}
	}
	else if(selectSubObjs.GetNodeNum()>0)
	{
		pSubObj=selectSubObjs.GetFirst();
		if(pSubObj&&pSubObj->ciSubType==IDbEntity::DbPoint)	//��ʾ��������
			DisplayProfileVertexProperty(pPropList->m_nObjClassTypeId!=CLS_PROFILEVERTEX);
	} 
	else 
	{
		if(iDisplayType==2)	//CLDSLinePart
			DisplayLinePartProperty(pPropList->m_nObjClassTypeId!=CLS_LINEPART);
#ifdef __PART_DESIGN_INC_
		else if(bHasFemElem)
			DisplayFemElementProperty(pPropList->m_nObjClassTypeId!=CLS_FEMELEMENT);
#endif
		else if(iDisplayType==0)		//CLDSDbObject
			DisplayDbObjectProperty(pPropList->m_nObjClassTypeId!=CLS_DBOBJECT);
#ifndef __TSA_
		else if(iDisplayType==1)	//CLDSPart
			DisplayPartProperty(pPropList->m_nObjClassTypeId!=CLS_PART);
		else if(iDisplayType==3)	//CLDSArcPart
			DisplayArcPartProperty(pPropList->m_nObjClassTypeId!=CLS_ARCPART);
#endif
		else if(iDisplayType==-1)
		{
			CLDSObject *pObj=selectObjs.GetFirst();
			if(pObj->GetClassTypeId()==CLS_NODE)
				DisplayNodeProperty(pPropList->m_nObjClassTypeId!=CLS_NODE);
			else if(pObj->GetClassTypeId()==CLS_LINEPART||(((CLDSPart*)pObj)->IsLinePart()&&theApp.m_bAnalysisWorkEnv))
				DisplayLinePartProperty(pPropList->m_nObjClassTypeId!=CLS_LINEPART);
			else if(pObj->GetClassTypeId()==CLS_LINETUBE)
				DisplayTubeProperty(pPropList->m_nObjClassTypeId!=CLS_LINETUBE);
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
#ifdef __GGG_
			else if(pObj->GetClassTypeId()==CLS_CONETUBE)
				DisplayConeTubeProperty(pPropList->m_nObjClassTypeId!=CLS_CONETUBE);
			else if(pObj->GetClassTypeId()==CLS_SLOTARM)
				DisplaySlotArmProperty(pPropList->m_nObjClassTypeId!=CLS_SLOTARM);
#endif
#endif
#ifndef __TSA_
			else if(pObj->GetClassTypeId()==CLS_LINEANGLE||pObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
				DisplayLineAngleProperty(pPropList->m_nObjClassTypeId!=pObj->GetClassTypeId());
			else if(pObj->GetClassTypeId()==CLS_PLATE)
				DisplayPlateProperty(pPropList->m_nObjClassTypeId!=CLS_PLATE);
			else if(pObj->GetClassTypeId()==CLS_SPHERE)
				DisplaySphereProperty(pPropList->m_nObjClassTypeId!=CLS_SPHERE);
			else if(pObj->GetClassTypeId()==CLS_PARAMPLATE)
				DisplayParamPlateProperty(pPropList->m_nObjClassTypeId!=CLS_PARAMPLATE);
			else if(pObj->GetClassTypeId()==CLS_BOLT)
				DisplayBoltProperty(pPropList->m_nObjClassTypeId!=CLS_BOLT);
			else if(pObj->GetClassTypeId()==CLS_LINESLOT)
				DisplayLineSlotProperty(pPropList->m_nObjClassTypeId!=CLS_LINESLOT);
			else if(pObj->GetClassTypeId()==CLS_LINEFLAT)
				DisplayLineFlatProperty(pPropList->m_nObjClassTypeId!=CLS_LINEFLAT);
#ifdef __H_SHAPE_STEEL_
			else if(pObj->GetClassTypeId()==CLS_LINEHSHAPE)
				DisplayLineHShapeProperty(pPropList->m_nObjClassTypeId!=CLS_LINEHSHAPE);
#endif
			else if(pObj->GetClassTypeId()==CLS_ARCANGLE)
				DisplayArcAngleProperty(pPropList->m_nObjClassTypeId!=CLS_ARCANGLE);
			else if(pObj->GetClassTypeId()==CLS_ARCSLOT)
				DisplayArcSlotProperty(pPropList->m_nObjClassTypeId!=CLS_ARCSLOT);
			else if(pObj->GetClassTypeId()==CLS_ARCFLAT)
				DisplayArcFlatProperty(pPropList->m_nObjClassTypeId!=CLS_ARCFLAT);
			else if(pObj->GetClassTypeId()==CLS_POLYJOINT)
				DisplayPolyJointProperty(pPropList->m_nObjClassTypeId!=CLS_POLYJOINT);
			else if(pObj->GetClassTypeId()==CLS_BLOCKREF)
				DisplayBlockReferenceProperty(pPropList->m_nObjClassTypeId!=CLS_BLOCKREF);
			else if(pObj->GetClassTypeId()==CLS_FITTINGS)
				DisplayFittingProperty(pPropList->m_nObjClassTypeId!=CLS_FITTINGS);
			else if(pObj->GetClassTypeId()==CLS_FOUNDATION)
				DisplayFoundationProperty(pPropList->m_nObjClassTypeId!=CLS_FOUNDATION);
			else if(pObj->GetClassTypeId()==CLS_ANCHORBOLT)
				DisplayAnchorBoltProperty(pPropList->m_nObjClassTypeId!=CLS_ANCHORBOLT);
#endif
			//else
			//	DisplaySysSettingProperty(pPropList->m_nObjClassTypeId!=0);
		}
	}
	if(!bFilterDisplay)
	{	//����ǰѡ�й������ಢ�����������еĹ��������� wht 10-11-12
		HASHOBJSET objSet;
		for(pObj=selectObjsBackup.GetFirst();pObj;pObj=selectObjsBackup.GetNext())
			objSet.SetValue(pObj->handle,pObj);	
		pPropDlg->UpdateSelectFilter(objSet);
	}
	}
	catch(char* error)
	{	//����������ʾ���Թ����п��ܻ���ڶ�ʧ��׼����������������׳��쳣 wjh-2015.6.5
		AfxMessageBox(error);
	}
}

void CLDSView::OnDisplayCoordAxis() 
{
	theApp.env.m_bDisplayWorldUcs = !theApp.env.m_bDisplayWorldUcs;
	g_pSolidSet->SetDispObjectUcs(theApp.env.m_bDisplayWorldUcs);
	Invalidate(FALSE);
}

void CLDSView::OnUpdateDisplayCoordAxis(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(theApp.env.m_bDisplayWorldUcs);
}
void CLDSView::OnDisplayFrameLine()
{
	bool display=!(g_sysPara.display.bNodePnt||CLDSLinePart::bDisplayLineVecMark);
	g_sysPara.display.bNodePnt=CLDSLinePart::bDisplayLineVecMark=display;
	g_sysPara.display.bSolidLine=g_sysPara.display.bNodePnt;
	g_pSolidSet->SetDisplayLinePoints(g_sysPara.display.bNodePnt);
	g_pSolidSet->SetDisplayLineVecMark(CLDSLinePart::bDisplayLineVecMark);
	g_pSolidSet->SetSolidAndLineStatus(g_sysPara.display.bSolidLine);
	g_pSolidDraw->Draw();
}
void CLDSView::OnUpdateDisplayFrameLine(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(g_sysPara.display.bNodePnt||CLDSLinePart::bDisplayLineVecMark);	//theApp.env.m_bDisplayLineVecMark);
}
void CLDSView::OnDisplayLineNode()
{
	g_sysPara.display.bNodePnt=!g_sysPara.display.bNodePnt;
	g_sysPara.display.bSolidLine=g_sysPara.display.bNodePnt;
	g_pSolidSet->SetDisplayLinePoints(g_sysPara.display.bNodePnt);
	g_pSolidSet->SetSolidAndLineStatus(g_sysPara.display.bSolidLine);
	g_pSolidDraw->Draw();
}
void CLDSView::OnUpdateDisplayLineNode(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(g_sysPara.display.bNodePnt);
}

void CLDSView::OnDisplayLineVec() 
{
	//theApp.env.m_bDisplayLineVecMark = !theApp.env.m_bDisplayLineVecMark;
	//g_pSolidSet->SetDisplayLineVecMark(theApp.env.m_bDisplayLineVecMark);
	CLDSLinePart::bDisplayLineVecMark = !CLDSLinePart::bDisplayLineVecMark;
	g_pSolidSet->SetDisplayLineVecMark(CLDSLinePart::bDisplayLineVecMark);
	Invalidate(FALSE);
}

void CLDSView::OnExportBmpFile() 
{
	g_pSolidSet->SetOperType(OPER_SNAPBMP);
}

void CLDSView::OnUpdateDisplayLineVec(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(CLDSLinePart::bDisplayLineVecMark);	//theApp.env.m_bDisplayLineVecMark);
}
static BOOL IsHasBoltPad(CLDSBolt* pBolt)
{
	CLDSPlate* pBoltPad=NULL;
	for(pBoltPad=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pBoltPad;pBoltPad=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
	{
		if(pBoltPad->FindLsByHandle(pBolt->handle)==NULL)
			continue;
		if(pBoltPad->wiType==CLDSGeneralPlate::WASHER_PLATE)
			return TRUE;
	}
	return FALSE;
}
void CLDSView::OnRevisionBoltSpace()
{
	PARTSET boltSet;
	long *id_arr=NULL,nn=g_pSolidSnap->GetLastSelectEnts(id_arr);
	for(int i=0;i<nn;i++)
	{
		CLDSBolt *pBolt=(CLDSBolt*)Ta.Parts.FromHandle(id_arr[i],CLS_BOLT);
		if(pBolt)
			boltSet.append(pBolt);
	}
	if(boltSet.GetNodeNum()<=0)
		return;
	//ѡ���������˨�˾�Ľ���Ǹ�
	CCmdLineDlg *pCmdLine =((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSLineAngle* pCrossJg=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select a cross line-angle��or Enter:","");
#else
	pCmdLine->FillCmdLine("��ѡ���������˨�Ľ���Ǹ֣��򰴻س������������ж�:","");
#endif
	DWORD dwhObj=0,dwExportFlag=0;
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINEANGLE));
	verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
	while(1)
	{
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
		{
			pCmdLine->CancelCmdLine();
			return;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		pCrossJg=(CLDSLineAngle*)console.FromPartHandle(obj.hRelaObj,CLS_LINEANGLE);
		if(pCrossJg)
			break;
		if(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
			break;
	}
	if(pCrossJg)
	{
		g_pSolidDraw->SetEntSnapStatus(pCrossJg->handle);
		pCmdLine->FinishCmdLine(CXhChar16("0X%X",pCrossJg->handle));
	}
	//���ݽ���Ǹ�������˨�ļ��
	CUndoOperObject undo(&Ta,true);
	for(CLDSBolt* pBolt=(CLDSBolt*)boltSet.GetFirst();pBolt;pBolt=(CLDSBolt*)boltSet.GetNext())
	{
		if(pBolt->des_base_pos.datumPoint.datum_pos_style!=3)
		{	//�ǽǸ����߽������˨���õ���λ��
			boltSet.DeleteCursor();
			continue;
		}
		CLDSLineAngle* pDaumJg=(CLDSLineAngle*)console.FromPartHandle(pBolt->des_base_pos.hPart);
		if(pCrossJg==NULL)
		{	//����ѡ�񽻲�˼�
			long hCrossJg=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1;
			if(hCrossJg==pBolt->des_base_pos.hPart)
				hCrossJg=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2;
			pCrossJg=(CLDSLineAngle*)console.FromPartHandle(hCrossJg);
		}
		if(pCrossJg==NULL || pDaumJg==NULL)
		{
			boltSet.DeleteCursor();
			continue;
		}
		//ȷ����˨��׼�϶
		BOOL bHasBoltPad=IsHasBoltPad(pBolt);
		int nLsJgSpace=0;
		if(pBolt->get_d()<=12)
			nLsJgSpace=bHasBoltPad?g_sysPara.intelligentConnect.m_n12LsOutterSpaceHasWasher:g_sysPara.intelligentConnect.m_n12LsOutterSpace;
		else if(pBolt->get_d()<=16)//M20
			nLsJgSpace=bHasBoltPad?g_sysPara.intelligentConnect.m_n16LsOutterSpaceHasWasher:g_sysPara.intelligentConnect.m_n16LsOutterSpace;
		else if(pBolt->get_d()<=20)
			nLsJgSpace=bHasBoltPad?g_sysPara.intelligentConnect.m_n20LsOutterSpaceHasWasher:g_sysPara.intelligentConnect.m_n20LsOutterSpace;
		else
			nLsJgSpace=bHasBoltPad?g_sysPara.intelligentConnect.m_n24LsOutterSpaceHasWasher:g_sysPara.intelligentConnect.m_n24LsOutterSpace;
		//���ݽ���Ǹ�����˨λ�ã���ʼ��λ�ù�ϵ
		f3dPoint work_norm,cross_line_vec,cross_wing_vec;
		work_norm=pBolt->get_norm();
		cross_line_vec=pCrossJg->End()-pCrossJg->Start();
		normalize(cross_line_vec);
		if(fabs(work_norm*pCrossJg->get_norm_x_wing())>fabs(work_norm*pCrossJg->get_norm_y_wing()))
			cross_wing_vec=pCrossJg->GetWingVecX();
		else
			cross_wing_vec=pCrossJg->GetWingVecY();
		UCS_STRU ucs;
		ucs.origin=pCrossJg->Start();
		ucs.axis_x=cross_line_vec;
		ucs.axis_y=cross_wing_vec;
		ucs.axis_z=ucs.axis_x^ucs.axis_y;
		//
		double dd=0;
		f3dPoint pos=pBolt->ucs.origin;
		coord_trans(pos,ucs,FALSE);
		if(pos.y>pCrossJg->GetWidth())	//��˨λ��֫���
			dd=pos.y-pCrossJg->GetWidth();
		else if(pos.y<0)		//��˨λ�����߲�
			dd=-pos.y;
		else if(pCrossJg->FindLsByHandle(pBolt->handle)!=NULL)
		{	//��˨�ڽ���Ǹ���
			boltSet.DeleteCursor();
			continue;
		}
		else if(pos.y<pCrossJg->GetWidth()/2)
			dd=pos.y*-1.0;	//��Ϊ��˨Ӧ�����߲�ƫ��
		else //if(pos.y>=pCrossJg->GetWidth()/2)
			dd=pos.y-pCrossJg->GetWidth();
		if(dd>=nLsJgSpace)	
		{	//������׼�϶���ڽ���Ǹ���
			boltSet.DeleteCursor();
			continue;
		}
		//���㲹����
		f3dPoint line_vec;
		if(pBolt->des_base_pos.direction==0)
			line_vec=pDaumJg->End()-pDaumJg->Start();
		else
			line_vec=pDaumJg->Start()-pDaumJg->End();
		normalize(line_vec);
		double cosa=fabs(line_vec*cross_wing_vec);
		double sina=sqrt(1-cosa*cosa);
		double add_offset=(nLsJgSpace-dd)/cosa;
		double round_offset=fto_halfi(add_offset/10)*10;	//Բ���������ֵ
		if(round_offset<add_offset)
			pBolt->des_base_pos.len_offset_dist+=ftoi(round_offset)+5;
		else
			pBolt->des_base_pos.len_offset_dist+=ftoi(round_offset);	
		pBolt->correct_pos();
		pBolt->CalGuigeAuto();
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	boltSet.Clean();
	g_pSolidDraw->ReleaseSnapStatus();
	if(boltSet.GetNodeNum()==0)
	{
		AfxMessageBox("ѡ����˨������Ҫ��!");
		return;
	}
	//�޸���˨λ�ú���˨���ڰ���Ҫ�������
	for(CLDSBolt* pBolt=(CLDSBolt*)boltSet.GetFirst();pBolt;pBolt=(CLDSBolt*)boltSet.GetNext())
	{
		for(CLDSPlate* pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
		{
			if(pPlate->FindLsByHandle(pBolt->handle))
			{	//�����������˨��صĸְ�
				pPlate->SetModified();
				pPlate->DesignPlate();
				pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
			}
		}
	}
	g_pSolidDraw->Draw();
}
void FindBoltRelatedParts(const CLDSBolt *pBolt,PARTSET &partSet); //defined in BoltThroughThickDlg.cpp
void CLDSView::OnRevisionBoltL0nZpos()
{
#ifdef __ANGLE_PART_INC_
	m_nPrevCommandID=ID_REVISION_BOLT_L0nZPOS;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat <Revision Bolt's Z Axis Position>";
#else 
	m_sPrevCommandName="�ظ�<У����˨Z��λ��>";
#endif
	CLogErrorLife loglife(&logerr);
	try{
		CUndoOperObject undo(&Ta,true);
		long *id_arr=NULL;
		long i,nn=g_pSolidSnap->GetLastSelectEnts(id_arr);
		CLDSModule* pModule=Ta.GetActiveModule();
		int hits=0;
		for(i=0;i<nn;i++)
		{
			CLDSBolt *pBolt=(CLDSBolt*)Ta.Parts.FromHandle(id_arr[i],CLS_BOLT);
			if(pBolt==NULL)
				continue;
			PARTSET partSet,actset;
			FindBoltRelatedParts(pBolt,partSet);
			CXhChar50 sTemp;
			bool bMultiModule=false;
			CLDSPart* pPart;
			for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
			{
				if(!pModule->IsSonPart(pPart))
				{
					bMultiModule=true;
					break;
				}
			}
			CXhChar500 strL0;
			for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
			{
				CXhChar16 hstr("0X%X",pPart->handle);
				BOOL moduleAdapted=pModule->IsSonPart(pPart);
				if(moduleAdapted)
					actset.append(pPart);
				if(bMultiModule&&moduleAdapted)
					hstr.At(1)='*';
				//ʹ��CString::Formatʱ�����ֱ�Ӳ����ַ���������Ϊ��������Ļ���
				//��������оͻ�����ڴ��ͻ���������ͻ���޷�Ԥ����(��ʹ��AppendFormat) wht 15-04-13
				if(strL0.GetLength()>0)
					strL0.Append(',');
				strL0.Append(hstr);
			}
			if(strlen(pBolt->statSegStr)==0)
				pBolt->SetL0KeyStr(strL0);	//������ȡͨ���ַ���
			else
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X Bolt's statistical segment range setted��through length revision is blocked!",pBolt->handle);
#else
				logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X��˨�趨�˶κ�ͳ�Ʒ�Χ����ֹ����˨ͨ����޶�",pBolt->handle);
#endif
			//��������������˨����ƫ�Ʋ�����wjh-2015.4.11
			pBolt->des_base_pos.norm_offset.key_str=CLDSBolt::EmendZOffsetStr(pBolt,actset);
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->CalGuigeAuto();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			hits++;
		}
		if(hits>0)
		{
			g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox("Through thickness and position of bolt revision successful!");
#else 
			MessageBox("��˨ͨ��λ��У���ɹ�!");
#endif
		}
	}
	//�����쳣���������⣬û�д����м��ѹջ��δ��վ���⣬�����ⲻ���û��������쳣��λ2048�����ϡ�2003/10/18 WJH
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
#endif
}
void CLDSView::GetBoltActiveRelaPartSet(CLDSBolt* pBolt,PARTSET& partset,CLDSModule* pModule)
{
	if(pModule==NULL)
		pModule=Ta.GetActiveModule();
	CHashList<THICK_ITEMTAG> listItems;
	bool hasMultiModule=false;
	THICK_ITEMTAG* pItemTag=NULL;
	for(THICK_ITEM item=pBolt->GetFirstL0ThickItem();!item.IsFinalKey();item=pBolt->GetNextL0ThickItem())
	{
		pItemTag=item.IsHandle()?listItems.SetValue((item.val),THICK_ITEMTAG(item)):NULL;
		if(pItemTag==NULL)
			continue;
		pItemTag->pPart=Ta.Parts.FromHandle(item.val);
		if(pItemTag->cItemType==3)
			hasMultiModule=true;
	}
	for(pItemTag=listItems.GetFirst();pItemTag;pItemTag=listItems.GetNext())
	{
		if(pItemTag->pPart==NULL)
			continue;
		if(  pItemTag->cItemType==3||
			(!hasMultiModule&&pModule->IsSonPart(pItemTag->pPart)))
			partset.append(pItemTag->pPart);
	}
}
void CLDSView::OnAutoDesignSelectedParts() 
{	//�������ָ������
#ifdef __ANGLE_PART_INC_
	m_nPrevCommandID=ID_AUTO_DESIGN_SELECTED_PARTS;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat to redesign the specified object";
#else
	m_sPrevCommandName="�ظ��������ָ������";
#endif
	CLogErrorLife loglife;
	try{
		Ta.BeginUndoListen();
		long hInvisibleObj=0;
		long *id_arr=&hInvisibleObj;
		long i,nn=g_pSolidSnap->GetLastSelectEnts(id_arr);
		if(nn==0&&selectObjs.GetNodeNum()>0)
		{
			nn=1;
			hInvisibleObj=selectObjs.GetFirst()->handle;
			id_arr=&hInvisibleObj;
		}
		CAngleMoveOperObject oddmodified(NULL,true, nn > 50);
		//��һ�д����Ѹ���ϵͳ�������������� wjh-2019.8.25
		//CStackVariant<char> stackBoltL0CalVariable(&CLDSBolt::BOLTL0_CALMODE, CLDSBolt::L0CAL_INC_GAP_THICK);
		PARTSET angleset;
		for(i=0;i<nn;i++)
		{	//�Ƚ��ֹܡ��Ǹ֡��������۸ּ����ι��������������
			CLDSNode* pNode=console.FromNodeHandle(id_arr[i]);
			if(pNode)
			{
				pNode->Set();
				continue;
			}
			oddmodified.Enable(false);
			CLDSPart *pPart = console.FromPartHandle(id_arr[i]);
			if(pPart==NULL)
				continue;
			else if(pPart->IsShadowObject())
				pPart->ShadowDesign();
			else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				if (nn<50)
					oddmodified.Enable(true);
				CLDSLineAngle *pAngle = (CLDSLineAngle*)pPart;
				pAngle->ClearFlag();
				pAngle->CalPosition();
				pAngle->cal_x_wing_norm();
				pAngle->cal_y_wing_norm();
				pAngle->SetModified();
				if(pAngle->desStartOdd.m_iOddCalStyle==0)
					pAngle->CalStartOddment();
				if(pAngle->desEndOdd.m_iOddCalStyle==0)
					pAngle->CalEndOddment();
				if(pAngle->desStartOdd.m_iOddCalStyle!=0||pAngle->desEndOdd.m_iOddCalStyle!=0)
					angleset.append(pAngle);
				if(pAngle->desStartOdd.m_iOddCalStyle==0||pAngle->desEndOdd.m_iOddCalStyle==0)
					pAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				g_pSolidDraw->NewSolidPart(pAngle->GetSolidPartObject());
				RedrawAngleFrameDrawing(pAngle);
				//���½Ǹ�����˨�ױڵ�ʵ����ʾ
				for (CLsRef* pLsRef = pAngle->GetFirstLsRef(); pLsRef; pLsRef = pAngle->GetNextLsRef())
				{
					HOLE_WALL* pHoleWall = console.GetHoleWall(pLsRef->GetLsPtr(), pAngle->handle);
					if (pHoleWall)
					{
						pHoleWall->UpdateSolid();
						g_pSolidDraw->NewSolidPart(pHoleWall->GetSolidPartObject());
					}
				}
				id_arr[i]=0; //����ƹ��Ĺ����Ƴ���������
			}
			else if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				if (nn<50)
					oddmodified.Enable(true);
				CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)pPart;
				pGroupAngle->CalPosition(true);
				pGroupAngle->cal_x_wing_norm();
				pGroupAngle->cal_y_wing_norm();
				pGroupAngle->UpdateSonJgBasicInfo();
				pGroupAngle->UpdateSonJgPos();
				pGroupAngle->UpdateSonJgWingNorm();
				if(pGroupAngle->desStartOdd.m_iOddCalStyle==0)
					pGroupAngle->CalStartOddment();
				if(pGroupAngle->desEndOdd.m_iOddCalStyle==0)
					pGroupAngle->CalEndOddment();
				for(int j=0;j<4;j++)
				{
					CLDSLineAngle* pSonAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pGroupAngle->son_jg_h[j],CLS_LINEANGLE);
					if(pSonAngle)
					{
						pSonAngle->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pSonAngle->GetSolidPartObject());
					}
				}
			}
			else if(pPart->GetClassTypeId()==CLS_LINETUBE)
			{
				CLDSLineTube *pTube = (CLDSLineTube*)pPart;
				pTube->ClearFlag();
				pTube->CalWorkPlaneNorm();
				pTube->CalPosition();
				pTube->DesignStartJoint();
				pTube->DesignEndJoint();
				pTube->CalStartOddment();
				pTube->CalEndOddment();
				pTube->SetModified();
				pTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pTube->GetSolidPartObject());
				id_arr[i]=0;
			}
			else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
			{
				CLDSLineFlat *pFlat = (CLDSLineFlat*)pPart;
				pFlat->ClearFlag();
				pFlat->CalPosition();
				pFlat->CalWorkPlaneNorm();
				pFlat->CalStartOddment();
				pFlat->CalEndOddment();
				pFlat->SetModified();
				pFlat->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				g_pSolidDraw->NewSolidPart(pFlat->GetSolidPartObject());
				RedrawFlatFrameDrawing(pFlat);
				id_arr[i]=0;
			}
			else if(pPart->GetClassTypeId()==CLS_LINESLOT)
			{
				CLDSLineSlot *pSlot = (CLDSLineSlot*)pPart;
				pSlot->ClearFlag();
				pSlot->CalPosition();
				pSlot->CalWorkPlaneNorm();
				pSlot->CalStartOddment();
				pSlot->CalEndOddment();
				pSlot->SetModified();
				pSlot->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				g_pSolidDraw->NewSolidPart(pSlot->GetSolidPartObject());
				RedrawSlotFrameDrawing(pSlot);
				id_arr[i]=0;
			}
			else if(pPart->IsArcPart())
			{
				CLDSArcPart* pArcPart=(CLDSArcPart*)pPart;
				pArcPart->DesignArcBaseLine();
				pArcPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pArcPart->GetSolidPartObject());
				id_arr[i]=0;
			}
		}
		for(i=0;i<nn;i++)
		{	//�Ƚ�����壬�����壬�����߰���Ĳ����������������
			CLDSPart *pPart=console.FromPartHandle(id_arr[i]);
			if(pPart==NULL)
				continue;
			if(pPart->GetClassTypeId()!=CLS_PARAMPLATE)
				continue;
			CLDSParamPlate *pParamPlate = (CLDSParamPlate*)pPart;
			if(pParamPlate->m_iParamType==TYPE_ELBOWPLATE||pParamPlate->m_iParamType==TYPE_CIRRIBPLATE
				||pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
				continue;
			//��U�Ͱ���߰帽�����ԭ������ת��Ϊ������꣬������������Ժ���ת��Ϊ��������
			if(pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND)
			{
				for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
				{
					CLDSPart *pPart=console.FromPartHandle(pItem->h);
					if(pPart==NULL)
						continue;
					if(pPart->GetClassTypeId()==CLS_PLATE)
						coord_trans(pPart->ucs.origin,pParamPlate->ucs,FALSE);
				}
			}
			//������Ʋ�������
			CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(pParamPlate->m_hPartWeldParent,CLS_LINETUBE);
			if(pLineTube)
			{
				if(pLineTube->m_tJointEnd.hLinkObj==pParamPlate->handle)
					pLineTube->DesignEndJointUCS(pParamPlate);
				else if(pLineTube->m_tJointStart.hLinkObj==pParamPlate->handle)
					pLineTube->DesignStartJointUCS(pParamPlate);
			}
			pParamPlate->SetModified();
			pParamPlate->DesignPlate();
			pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pParamPlate->GetSolidPartObject());
			id_arr[i]=0;
			//������Ʋ�����ʱ���丽������˨�������
			for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
			{
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				pBolt->CalGuigeAuto();
				if(!pBolt->m_bVirtualPart)
				{	//��������˨����Ҫ��ʾ��˨ʵ�� wht 11-07-08
					pBolt->SetModified();
					pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}
			//�������U�Ͱ�;�߰�ʱ���丽���ķ�ͷ��ͼӾ���һ���������
			if(pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND||
				pParamPlate->m_iParamType==TYPE_XEND)
			{
				for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
				{
					CLDSPart *pPlate=console.FromPartHandle(pItem->h,CLS_PLATE,CLS_PARAMPLATE);
					if(!pPlate)
						continue;
					if(pPlate->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pPlate)->jdb_style==5)
						((CLDSPlate*)pPlate)->DesignSetupUcs();
					else
						coord_trans(pPlate->ucs.origin,pParamPlate->ucs,TRUE);
					pPlate->SetModified();
					pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
					g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
				}
			}
		}
		for(i=0;i<nn;i++)
		{	//�Ƚ��ְ������������
			CLDSPart *pPart=console.FromPartHandle(id_arr[i]);
			if(pPart==NULL)
				continue;
			SmartPartPtr pMotherPart=pPart->MotherPart();
			if(pMotherPart.IsNULL())
				pMotherPart=pPart;
			if(pMotherPart->GetClassTypeId()==CLS_PLATE)
			{
				CLDSPlate *pPlate = (CLDSPlate*)pMotherPart.pPlate;
				if(pPlate->jdb_style!=4&&pPlate->jdb_style!=5&&!pPlate->designInfo.m_bEnableFlexibleDesign)
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.Log("0X%X Plate is not enabled flexible design!",pPlate->handle);
#else
					logerr.Log("0X%X�ְ�δ�����������!",pPlate->handle);
#endif
				
				if(pPlate->jdb_style==5)	//����ڻ�׼����λ�ò���İ�
					pPlate->DesignSetupUcs();
				else
				{
					//ע����һ�д��룬�书����ͨ������DesignPlate()�е�DesignSetupUcs����λ��ʵ�֡�wjh-2015.9.10
					//pPlate->DesignSetupUcs();	//���Ӵ��лᵼ��������Ʋ����°�λ�ã�wjh-2011-6-30
					pPlate->AlignWeldLine();	//�Զ����������ĺ�����(ֻӰ�쵱ǰ�庸����)
					pPlate->DesignPlate();
				}
				if(pPlate->jdb_style==4)//��˨���������н�
					CutBoltPadByAngle(pPlate);
				pPlate->SetModified();
				pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				RedrawPlateProfileDrawing(pPlate,NULL,true,0x03);
				g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
				if(!pMotherPart.IsEqualPtr(pPart))
				{
					pPart->ShadowDesign();
					pPart->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
				}
				//���¸ְ�����˨�ױڵ�ʵ����ʾ
				for (CLsRef* pLsRef = pPlate->GetFirstLsRef(); pLsRef; pLsRef = pPlate->GetNextLsRef())
				{
					HOLE_WALL* pHoleWall = console.GetHoleWall(pLsRef->GetLsPtr(), pPlate->handle);
					if (pHoleWall)
					{
						pHoleWall->UpdateSolid();
						g_pSolidDraw->NewSolidPart(pHoleWall->GetSolidPartObject());
					}
				}
				id_arr[i]=0;
			}
		}
		CLDSModule* pModule=Ta.GetActiveModule();
		for(i=0;i<nn;i++)
		{
			CLDSPart *pPart = NULL;
			CLDSPart *pHoleResidePart=NULL;
			if(id_arr[i]>0)
				pPart = console.FromPartHandle(id_arr[i]);
			else
			{
				OBJECT_HANDLER handler;
				g_pSolidDraw->GetSolidObject(id_arr[i], &handler);
				if (handler.hiberid.HiberUpId(1) == 1)	//ѡ����˨��ʱ��ͬѡ����˨
				{
					pPart = console.FromPartHandle(handler.hiberid.masterId, CLS_BOLT);
					long hResidePart = handler.hiberid.HiberUpId(2);
					pHoleResidePart = console.FromPartHandle(hResidePart);
				}
				else if (handler.hiberid.HiberUpId(1) == 2)
					continue;//�ҵ��ͷ����
			}
			if(pPart==NULL)
				continue;
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				CLDSParamPlate *pPlate = (CLDSParamPlate*)pPart;
				if(pPlate->m_bStdPart)
					pPlate->DesignStdPartUcs();
				pPlate->SetModified();
				pPlate->DesignPlate();
				pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
			}
			else if(pPart->GetClassTypeId()==CLS_BOLT)
			{
				CLDSBolt *pBolt=(CLDSBolt*)pPart;
				pBolt->correct_worknorm();
				//��������������˨����ƫ�Ʋ�����wjh-2015.4.11
				PARTSET partset;
				GetBoltActiveRelaPartSet(pBolt,partset,pModule);
				pBolt->des_base_pos.norm_offset.key_str=CLDSBolt::EmendZOffsetStr(pBolt,partset);
				pBolt->correct_pos();
				pBolt->CalGuigeAuto();
				pBolt->SetModified();
				if(pBolt->is_visible)
				{	//������˨��ʾ״̬
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
				if(!pBolt->is_visible||pHoleResidePart!=NULL)
				{	//���ڿױ���ʾ״̬
					long hResidePart = pHoleResidePart != NULL ? pHoleResidePart->handle : 0;
					HOLE_WALL* pHoleWall = console.GetHoleWall(pBolt, hResidePart);
					if(pHoleWall)
					{
						pHoleWall->UpdateSolid();
						g_pSolidDraw->NewSolidPart(pHoleWall->GetSolidPartObject());
					}
					if (pHoleResidePart)
					{
						pHoleResidePart->SetSolidModified();
						pHoleResidePart->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pHoleResidePart->GetSolidPartObject());
					}
				}
			}
		}
		//���½�߹ҵ�
		for (i=0;i<nn;i++)
		{
			CLDSPart *pPart = id_arr[i]>0?console.FromPartHandle(id_arr[i]):NULL;
			if (pPart!=NULL&&pPart->GetClassTypeId()==CLS_FITTINGS)
			{
				CLDSFitting* pFitting=(CLDSFitting*)pPart;
				pFitting->DesignFitting();
				pFitting->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pFitting->GetSolidPartObject());
			}
		}
		for(CLDSLineAngle* pAngle=(CLDSLineAngle*)angleset.GetFirst();pAngle;pAngle=(CLDSLineAngle*)angleset.GetNext())
		{
			if(pAngle->desStartOdd.m_iOddCalStyle!=0)
				pAngle->CalStartOddment();
			if(pAngle->desEndOdd.m_iOddCalStyle!=0)
				pAngle->CalEndOddment();
			pAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pAngle->GetSolidPartObject());
		}
		Ta.EndUndoListen();
		g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("Redesign is finished!", theApp.m_pszAppName);
#else
		MessageBox("���������ϣ�",theApp.m_pszAppName);//����һЩδ֪�쳣����AfxGetAppName���󣬹��ڴ�ָ��AppName wjh-2019.6.23
#endif
	}
	//�����쳣���������⣬û�д����м��ѹջ��δ��վ���⣬�����ⲻ���û��������쳣��λ2048�����ϡ�2003/10/18 WJH
	catch(char *sError)
	{
		Ta.EndUndoListen();
		AfxMessageBox(sError);
		//if(pProcDlg!=NULL)
		//	delete pProcDlg;
	}
#endif
}

void CLDSView::OnObjectProperty() 
{
	CMainFrame* pFrame=(CMainFrame*)AfxGetMainWnd();
	g_sysPara.dock.pageProp.bDisplay=TRUE;
	pFrame->ModifyDockpageStatus(RUNTIME_CLASS(CTowerPropertyDlg),g_sysPara.dock.pageProp.bDisplay);
	UpdatePropertyPage();
}

int CLDSView::RenderMode()
{	//��ͼ��ʾ����
#ifdef __ANGLE_PART_INC_
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	DISPLAY_TYPE disp;
	g_pSolidSet->GetDisplayType(&disp);
	//�л���ͼ��ʾ����
	if(disp==DISP_LINE)
	{
		theApp.m_ePrevEnvType=theApp.m_eEnvType;
		theApp.m_eEnvType=SOLID_ENV;
		if(theApp.m_bAnalysisWorkEnv)
			theApp.m_bAnalysisWorkEnv=FALSE;
		if(g_sysPara.iColorSchema==1)		//TSA��ɫ����
			g_pSolidSet->SetBkColor(g_sysPara.crTsaMode.crBackground);
		else if(g_sysPara.iColorSchema==2)	//������ɫ����
			g_pSolidSet->SetBkColor(g_sysPara.crMaterialMode.crBackground);
		else								//������ɫ����
			g_pSolidSet->SetBkColor(g_sysPara.crPartMode.crBackground);
		LdsWorkEnvShift(SOLID_ENV);
		if(!pCmdLine->m_bGettingStrFromCmdLine&&!IsCmdLocked())
		{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Render view display type<solid model map(S)>:","");
#else
		pCmdLine->FillCmdLine("Render ��ͼ��ʾ����<ʵ��ͼ(S)>:","");
#endif
		pCmdLine->FinishCmdLine();
		}
		if(console.m_pWorkPart&&m_curAppType==TA_ASSEMBLE_ENV&&g_sysPara.display.bPartSketch)
		{	//�л���ʵ����ʾ״̬ʱѡ�е�ǰ�������� wht 10-09-03
			g_pSolidDraw->SetEntSnapStatus(console.m_pWorkPart->handle,TRUE);
			DrawPartLineMap(console.m_pWorkPart);
		}
	}
	else if(disp==DISP_SOLID)
	{
		theApp.m_ePrevEnvType=theApp.m_eEnvType;
		theApp.m_eEnvType=LINE_ENV;
		if(!pCmdLine->m_bGettingStrFromCmdLine&&!IsCmdLocked())
		{
#ifdef AFX_TARG_ENU_ENGLISH
    pCmdLine->FillCmdLine("Render view display type<line map(L)>:","");
#else
		pCmdLine->FillCmdLine("Render ��ͼ��ʾ����<����ͼ(L)>:","");
#endif
		pCmdLine->FinishCmdLine();
		}
		if(m_curAppType==TA_ASSEMBLE_ENV)
		{
			if(g_sysPara.iColorSchema==1)		//TSA��ɫ����
				g_pSolidSet->SetBkColor(g_sysPara.crTsaMode.crBackground);
			else if(g_sysPara.iColorSchema==2)	//������ɫ����
				g_pSolidSet->SetBkColor(g_sysPara.crMaterialMode.crBackground);
			else								//������ɫ����
				g_pSolidSet->SetBkColor(g_sysPara.crPartMode.crBackground);	
		}		
		else
			g_pSolidSet->SetBkColor(g_sysPara.crPartMode.crPartBackground);
		g_pSolidSet->SetDisplayType(DISP_LINE);
		if(m_curAppType==TA_PART_EDITOR)
			g_pSolidDraw->BuildDisplayList();
	}
	g_pSolidDraw->Draw();
	Invalidate(FALSE);
	if(!pCmdLine->m_bGettingStrFromCmdLine&&!IsCmdLocked())
	{
#ifdef AFX_TARG_ENU_ENGLISH
    pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("����:","");
#endif
	}
#endif
	return 0;
}

void CLDSView::OnUpdateMirrorCreate(CCmdUI* pCmdUI) 
{
	long *id_arr=NULL;
	if(g_pSolidSnap->GetLastSelectEnts(id_arr)>0)
		pCmdUI->Enable();
	else
		pCmdUI->Enable(FALSE);
}

typedef CLDSDbObject* DBOBJPTR;
static BOOL IsIncludeHandle(CLDSPoint *pDatumPoint,long hMainObj)
{
	if(pDatumPoint&&hMainObj>0x20)
	{
		int datum_pos_style=pDatumPoint->datum_pos_style;
		CLDSPoint::DES_PARA des_para=pDatumPoint->des_para;
		if(des_para.hNode==hMainObj
			||(datum_pos_style==1&&des_para.RODEND.hRod==hMainObj)
			||(datum_pos_style==2&&(des_para.RODNODE.hRod ==hMainObj||des_para.RODNODE.hNode==hMainObj))
			||(datum_pos_style==3&&(des_para.AXIS_INTERS.hDatum1==hMainObj||des_para.AXIS_INTERS.hDatum2==hMainObj))	
			||(datum_pos_style==4&&(des_para.AXIS_INTERS.hDatum1==hMainObj||des_para.AXIS_INTERS.hDatum2==hMainObj))
			||(datum_pos_style==5&&des_para.DATUMLINE.hDatumLine==hMainObj)
			||(datum_pos_style==6&&des_para.DATUMPLANE.hDatumPlane==hMainObj)
			||(datum_pos_style==7&&des_para.RODEND.hRod==hMainObj)
			||(datum_pos_style==8&&des_para.hNode==hMainObj)
			||(datum_pos_style==9&&des_para.hPart==hMainObj))
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}
static BOOL IsIncludeHandle(CLDSVector *pDatumVector,long hMainObj)
{
	if(pDatumVector&&hMainObj>0x20)
	{
		if(pDatumVector->hCrossPart==hMainObj
			||pDatumVector->hVicePart==hMainObj)
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}
//1:pObj1������pObj2 pObj1��pObj2֮������
//0:pObj1��pObj2��������ϵ
//-1:pObj2������pObj1 pObj2��pObj1֮������
int CompareObjAttachRelation(const DBOBJPTR &pObj1,const DBOBJPTR &pObj2)
{
	if(pObj1==NULL||pObj2==NULL)
		return 0;
	//����ѥ�����������Ҫ�ȶԳ���˨�ף���Ӧ����˨������������Գ�֮ǰ wjh-2015.6.30
	if(pObj1->GetClassTypeId()==CLS_BOLT && pObj2->GetClassTypeId()!=CLS_BOLT)
		return -1;
	else if(pObj1->GetClassTypeId()!=CLS_BOLT && pObj2->GetClassTypeId()==CLS_BOLT)
		return  1;
	else if (pObj1->GetClassTypeId() == CLS_FITTINGS && pObj2->GetClassTypeId() != CLS_FITTINGS)
		return  1;	//��߿���Գ�
	else if (pObj1->GetClassTypeId() == CLS_POLYJOINT && pObj2->GetClassTypeId() != CLS_POLYJOINT)
		return  1;	//�����ͷ�������Գ� wjh-2016.11.8
	for(int k=0;k<2;k++)
	{
		int nRetCode=1;
		long hMainObj=0;
		CLDSDbObject *pCurObj=NULL,*pMainObj=NULL;
		if(k==0)
		{	//�ж�pObj1�Ƿ�������pObj2
			hMainObj=pObj2->handle;
			pCurObj=pObj1;
			pMainObj=pObj2;
			nRetCode=1;	//pObj1������pObj2 pObj1��pObj2֮������
		}
		else 
		{	//�ж�pObj2�Ƿ�������pObj1
			hMainObj=pObj1->handle;
			pMainObj=pObj1;
			pCurObj=pObj2;
			nRetCode=-1;//pObj2������pObj1 pObj2��pObj1֮������
		}
		for(RELATIVE_OBJECT *pRelaObj=pCurObj->relativeObjs.GetFirst();pRelaObj;pRelaObj=pCurObj->relativeObjs.GetNext())
		{
			if(pRelaObj->hObj==hMainObj)
				return nRetCode; 
		}
		if(pCurObj->GetClassTypeId()==CLS_NODE)
		{
			CLDSNode *pNode=(CLDSNode*)pCurObj;
			if(pNode->hFatherPart==hMainObj)
				return nRetCode;
			for(int i=0;i<4;i++)
			{
				if(pNode->arrRelationNode[0]==hMainObj)
					return nRetCode;
			}
			if(pMainObj&&pMainObj->GetClassTypeId()==CLS_LINEANGLE)
			{	//�ڽ���ڵ㴦��Ϻ�������ϵ���ܳ���"����"���� ��ʱ���ж�������˼�Ϊ�Ǹֵ���� wht 10-03-02 
				if(pNode->arrRelationPole[0]==hMainObj||pNode->arrRelationPole[1]==hMainObj)
					return nRetCode;
			}
		}
		else if(pCurObj->IsPart())
		{	
			CLDSPart *pPart=(CLDSPart*)pCurObj;
			if(pPart->m_hPartWeldParent==hMainObj)
				return nRetCode;
			if(pPart->IsLinePart())
			{
				CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
				//���ն˽ڵ�
				if((pLinePart->pStart&&pLinePart->pStart->handle==hMainObj)
					||(pLinePart->pEnd&&pLinePart->pEnd->handle==hMainObj))
					return nRetCode;
				//����ͷ
				if(pLinePart->desStartOdd.m_hRefPart1==hMainObj||pLinePart->desStartOdd.m_hRefPart2==hMainObj
					||pLinePart->desEndOdd.m_hRefPart1==hMainObj||pLinePart->desEndOdd.m_hRefPart2==hMainObj)
					return nRetCode;
#ifdef __ANGLE_PART_INC_
				if(pPart->GetClassTypeId()==CLS_LINEANGLE
					||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
				{
					CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pPart;
					//ʼ��λ��
					if(pLineAngle->des_norm_x.hViceJg==hMainObj||pLineAngle->des_norm_y.hViceJg==hMainObj
						||pLineAngle->desStartPos.datum_jg_h==hMainObj||pLineAngle->desStartPos.huoqu_jg_h==hMainObj
						||pLineAngle->desStartPos.datumLineStart.des_para.RODEND.hRod==hMainObj
						||pLineAngle->desStartPos.datumLineEnd.des_para.RODEND.hRod==hMainObj)
						return nRetCode;
					//��׼��
					if(IsIncludeHandle(&pLineAngle->desStartPos.datumPoint,hMainObj))
						return nRetCode;
					//�ն�λ��
					if(pLineAngle->desStartPos.datum_jg_h==hMainObj||pLineAngle->desStartPos.huoqu_jg_h==hMainObj
						||pLineAngle->desStartPos.datumLineStart.des_para.RODEND.hRod==hMainObj
						||pLineAngle->desStartPos.datumLineEnd.des_para.RODEND.hRod==hMainObj)
						return nRetCode;
					//��׼��
					if(IsIncludeHandle(&pLineAngle->desEndPos.datumPoint,hMainObj))
						return nRetCode;
					if(pLineAngle->group_father_jg_h==hMainObj)
						return nRetCode;
					if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
					{
						for(int i=0;i<4;i++)
						{
							if(pLineAngle->son_jg_h[i]==hMainObj)
								return nRetCode;
						}
					}
				}
				else if(pCurObj->GetClassTypeId()==CLS_LINETUBE
					||pCurObj->GetClassTypeId()==CLS_LINESLOT
					||pCurObj->GetClassTypeId()==CLS_LINEFLAT)
				{
					CTubeEndPosPara *pCurStartPosPara=NULL,*pCurEndPosPara=NULL;
					if(pCurObj->GetClassTypeId()==CLS_LINETUBE)
					{
						CLDSLineTube *pLineTube=(CLDSLineTube*)pCurObj;
						if(pLineTube->hWorkNormRefPart==hMainObj
							||pLineTube->hTransTube==hMainObj
							||pLineTube->m_tJointStart.hLinkObj==hMainObj
							||pLineTube->m_tJointStart.hViceLinkObj==hMainObj
							||pLineTube->m_tJointEnd.hLinkObj==hMainObj
							||pLineTube->m_tJointEnd.hViceLinkObj==hMainObj)
							return nRetCode;
						pCurStartPosPara=&pLineTube->desStartPos;
						pCurEndPosPara=&pLineTube->desEndPos;
					}
					else if(pCurObj->GetClassTypeId()==CLS_LINESLOT)
					{
						pCurStartPosPara=&((CLDSLineSlot*)pCurObj)->desStartPos;
						pCurEndPosPara=&((CLDSLineSlot*)pCurObj)->desEndPos;
					}
					else if(pCurObj->GetClassTypeId()==CLS_LINEFLAT)
					{
						pCurStartPosPara=&((CLDSLineFlat*)pCurObj)->desStartPos;
						pCurEndPosPara=&((CLDSLineFlat*)pCurObj)->desEndPos;
					}
					if((pCurStartPosPara&&(pCurStartPosPara->hDatumPart==hMainObj||pCurStartPosPara->hDatumStartPart==hMainObj||pCurStartPosPara->hDatumEndPart==hMainObj))
						||(pCurEndPosPara&&(pCurEndPosPara->hDatumPart==hMainObj||pCurEndPosPara->hDatumStartPart==hMainObj||pCurEndPosPara->hDatumEndPart==hMainObj)))
						return nRetCode;
				}
#endif
			}
			/*else if(pPart->IsArcPart())
			{
				
			}*/
#ifdef __ANGLE_PART_INC_
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				DESIGN_PLATE_ITEM *pItem=NULL;
				CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
				for(pItem=pParamPlate->params.GetFirst();pItem;pItem=pParamPlate->params.GetNext())
				{
					if(pItem->dataType==1&&pItem->value.hPart==hMainObj)
						return nRetCode;
				}		
			}
			else if(pPart->GetClassTypeId()==CLS_PLATE)
			{
				CLDSPlate *pPlate=(CLDSPlate*)pPart;
				if(pPlate->designInfo.m_hBaseNode==hMainObj
					||pPlate->designInfo.m_hBasePart==hMainObj)
					return nRetCode;
				//��׼��
				if(IsIncludeHandle(&pPlate->designInfo.origin,hMainObj))
					return nRetCode;
				int i=0;
				for(i=0;i<2;i++)
				{	
					if(IsIncludeHandle(&pPlate->designInfo.huoquline_start[i],hMainObj))
						return nRetCode;
					if(IsIncludeHandle(&pPlate->designInfo.huoquline_end[i],hMainObj))
						return nRetCode;
					if(IsIncludeHandle(&pPlate->designInfo.huoqufacenorm[i],hMainObj))
						return nRetCode;
				}
				if(IsIncludeHandle(&pPlate->designInfo.norm,hMainObj))
					return nRetCode;
				CDesignLjPartPara *pLjPara=NULL;
				for(pLjPara=pPlate->designInfo.partList.GetFirst();pLjPara;pLjPara=pPlate->designInfo.partList.GetNext())
				{
					if(pLjPara->hPart==hMainObj)
						return nRetCode;
				}
			}
			else if(pPart->GetClassTypeId()==CLS_BOLT)
			{
				CLDSBolt *pBolt=(CLDSBolt*)pPart;
				if(pBolt->des_base_pos.hPart==hMainObj)
					return nRetCode;
				if(IsIncludeHandle(&pBolt->des_base_pos.datumPoint,hMainObj))
					return nRetCode;
				if(IsIncludeHandle(&pBolt->des_work_norm,hMainObj))
					return nRetCode;
				//ƫ�����ַ���
				THICK_ITEM item;
				for(item=pBolt->des_base_pos.norm_offset.GetFirstItem();item.val!=0;
				item=pBolt->des_base_pos.norm_offset.GetNextItem())
				{
					if(item.IsHandle()&&abs(item.val)==hMainObj)
						return nRetCode;
				}
				for(item=pBolt->GetL0ThickPara()->GetFirstItem();item.val!=0;
				item=pBolt->GetL0ThickPara()->GetNextItem())
				{
					if(item.IsHandle()&&abs(item.val)==hMainObj)
						return nRetCode;
				}
			}
			else if(pPart->GetClassTypeId()==CLS_SPHERE)
			{
				CLDSSphere *pSphere=(CLDSSphere*)pPart;
				if(pSphere->hDatumNode==hMainObj)
					return nRetCode;
			}
#endif
		}
	}
#ifdef __COMMON_PART_INC_ //ifndef __TSA_
	//���������ڹ�����˨
	if(pObj1->IsPart()&&pObj2->IsPart())
	{
		CLsRefList *pLsRefList1=((CLDSPart*)pObj1)->GetLsRefList();
		CLsRefList *pLsRefList2=((CLDSPart*)pObj2)->GetLsRefList();
		if(pLsRefList1&&pLsRefList2)
		{	
			CLsRef *pLsRef=NULL;
			for(pLsRef=pLsRefList1->GetFirst();pLsRef;pLsRef=pLsRefList1->GetNext())
			{
				if(pLsRefList2->FromHandle(pLsRef->GetLsPtr()->handle))
					break;
			}
			if(pLsRef&&pLsRef->GetLsPtr())
			{	//���ڹ�����˨��Ӧ����������˨��λ����(��Ϊ�Գ����ɹ���ʱ��ͬʱ�Գ����ɹ����ϵ���˨) wht 12-03-19
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				if(pBolt->des_base_pos.hPart==pObj1->handle
					||IsIncludeHandle(&pBolt->des_base_pos.datumPoint,pObj1->handle))
					return -1;
				else if(pBolt->des_base_pos.hPart==pObj2->handle
					||IsIncludeHandle(&pBolt->des_base_pos.datumPoint,pObj2->handle))
					return 1;
			}
		}
	}
#endif
	return 0;
}
void CLDSView::OnMirrorCreate() 
{
	long *id_arr=NULL;
	int i=0,n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	static CMirMsgDlg mirdlg;
	mirdlg.m_bUseMirrorSymmetry=TRUE;
	if(mirdlg.DoModal()!=IDOK)
		return;
	CLogErrorLife loglife;
	int nError=0,hits=0;
	CUndoOperObject undo(&Ta,true);
	CMirOperObject mirror;
	//���ݹ��������ͽ�������
	CLDSDbObject *pObj;
	CHashSet<DBOBJPTR>hashSrcObjs;
	for (i=0;i<n;i++)
	{
		PROP_OBJ* pPropObj=NULL;
		if (id_arr[i]<0)
		{
			XHOBJ_HANDLER handler;
			g_pSolidDraw->GetSolidObject(id_arr[i],&handler);
			if (handler.ciXhObjType==PROP_OBJ::TYPE_HOLEWALL)
			{
				CLDSPart* pPart=console.FromPartHandle(handler.hiberid.masterId);
				if (pPart&&pPart->IsBolt())
				{	//��׽����˨�ױ�
					pPropObj=hashSelectObjs.Add(id_arr[i]);
					pPropObj->ciObjType=PROP_OBJ::TYPE_HOLEWALL;
					pObj=pPropObj->pBolt=(CLDSBolt*)pPart;
					pPropObj->hResidePart=handler.hBoltResidePart;
				}
				else
					continue;
			}
			else if (handler.ciXhObjType==PROP_OBJ::TYPE_WIREPOINTARROW)	//�ҵ��ͷ�������Գƴ���
				continue;	//�ҵ��ͷ�������Գƴ���
		}
		else
			pObj=(CLDSDbObject*)console.FromHandle(id_arr[i],TRUE);
		if(pObj==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox(CXhChar50("0X%X part not found in data model",id_arr[i]));
#else
			AfxMessageBox(CXhChar50("������ģ����δ�ҵ�0X%X����",id_arr[i]));
#endif
			return;
		}
		hashSrcObjs.SetValue(pObj->handle,pObj);
		//TODO:���´��뻹������,���¸��汾����ʱ������ wjh-2016.11.8
		if(pObj->IsPart()&&((CLDSPart*)pObj)->IsLinePart())
		{
			CLDSLinePart* pRod=(CLDSLinePart*)pObj;
			if(pRod->huoqu_handle>0x20)
			{
				CLDSPolyPart* pPolyPart=(CLDSPolyPart*)Ta.Parts.FromHandle(pRod->huoqu_handle,CLS_POLYPART);
				if(pPolyPart)
				{
					hashSrcObjs.SetValue(pPolyPart->handle,pPolyPart);
					for(pRod=pPolyPart->segset.GetFirst();pRod;pRod=pPolyPart->segset.GetNext())
						hashSrcObjs.SetValue(pRod->handle,pRod);
				}
				BYTE detect=0x03;	//0x01:��ʼ��̽��;0x02:���ն�̽��
				CLDSLinePart* pPrevRod=pRod,*pNextRod=pRod;
				for(CLDSPolyJoint* pJoint=(CLDSPolyJoint*)Ta.Parts.GetFirst(CLS_POLYJOINT);pJoint;pJoint=(CLDSPolyJoint*)Ta.Parts.GetNext(CLS_POLYJOINT))
				{
					if(pJoint->pPrevLinePart==pRod)
					{
						hashSrcObjs.SetValue(pJoint->handle,pJoint);
						if(pJoint->pNextLinePart)
							hashSrcObjs.SetValue(pJoint->pNextLinePart->handle,pJoint->pNextLinePart);
					}
					if(pJoint->pNextLinePart==pRod)
					{
						hashSrcObjs.SetValue(pJoint->handle,pJoint);
						if(pJoint->pPrevLinePart)
							hashSrcObjs.SetValue(pJoint->pPrevLinePart->handle,pJoint->pPrevLinePart);
					}
				}
			}
		}
	}
	n=hashSrcObjs.GetNodeNum();
	if(n==0)	//��Чѡ�й�����Ϊ��
		return;
	DBOBJPTR *dbObjPtrArr = new DBOBJPTR[n];
	for(pObj=hashSrcObjs.GetFirst(),hits=0;pObj;pObj=hashSrcObjs.GetNext(),hits++)
		dbObjPtrArr[hits] = pObj;
	CBubbleSort<DBOBJPTR>::BubSort(dbObjPtrArr,n,CompareObjAttachRelation);
	char sErrorInfo[200]="";
	//������������е���Ч���
	for(i=0;i<n;i++)
	{
		for(RELATIVE_OBJECT *pRelaObj=dbObjPtrArr[i]->relativeObjs.GetFirst();pRelaObj;pRelaObj=dbObjPtrArr[i]->relativeObjs.GetNext())
		{
			CLDSObject *pObj=dbObjPtrArr[i]->BelongModel()->FromHandle(pRelaObj->hObj,TRUE);
			if(dbObjPtrArr[i]->BelongModel()->FromHandle(pRelaObj->hObj,TRUE)==NULL)	
			{
				dbObjPtrArr[i]->relativeObjs.DeleteCursor();
				dbObjPtrArr[i]->SetModified(TRUE,FALSE);
			}
		}
		dbObjPtrArr[i]->relativeObjs.Clean();
	}
	BOOL hJustNewMirPart=Ta.handle;	//С�ڴ˾���Ĺ�����ԭ�й���,���ڵ��ڴ˾���Ĺ������¶Գ����ɵĹ��� 
	for(i=0;i<n;i++)
	{
		if(dbObjPtrArr[i]==NULL)
			continue;
		if(dbObjPtrArr[i]->GetClassTypeId()!=CLS_POLYJOINT)
			MirTaAtom(dbObjPtrArr[i],mirdlg.mirmsg,FALSE,sErrorInfo);
		else if(!MirTaAtom(dbObjPtrArr[i],mirdlg.mirmsg,TRUE,sErrorInfo))
		{
			if(sErrorInfo&&strlen(sErrorInfo)>0)
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("0X%X Fail to mirror the part!%s",dbObjPtrArr[i]->handle,sErrorInfo);
			else
				logerr.Log("0X%X Fail to mirror the part!",dbObjPtrArr[i]->handle);
#else
				logerr.Log("0X%X�����Գ�ʧ��!%s",dbObjPtrArr[i]->handle,sErrorInfo);
			else
				logerr.Log("0X%X�����Գ�ʧ��!",dbObjPtrArr[i]->handle);
#endif
			nError++;
		}
	}	
/*
	�ڶ๹��һ��Գ�ʱ��ǰһ������������Ϣ������õ���һ�������Գ�ǰһ����ʱ�����ں�һ�����ĶԳƹ���
	��δ���ɣ��ᵼ��ĳЩ������Ϣ�еľ�����ԶԳƶ�ʧ����˶������˺�����Ҫ������ȫ���Գƹ��������ɺ�
	����һЩ���ֵ���ԵĶԳƹ�����
*/
	for(i=0;i<n;i++)
	{
		if(dbObjPtrArr[i]==NULL)
			continue;
		if(dbObjPtrArr[i]->GetClassTypeId()!=CLS_POLYJOINT)
		{
			dbObjPtrArr[i]->feature=hJustNewMirPart;	//С�ڴ��¹����жϾ���Ĺ���Ϊԭ�й������Գ�λ�ü�������Ϣ	wjh 2011.8.12
			if(!MirTaAtom(dbObjPtrArr[i],mirdlg.mirmsg,TRUE,sErrorInfo))
			{
				if(sErrorInfo&&strlen(sErrorInfo)>0)
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.Log("0X%X Fail to mirror the part!%s",dbObjPtrArr[i]->handle,sErrorInfo);
				else
					logerr.Log("0X%X Fail to mirror the part!",dbObjPtrArr[i]->handle);
#else
					logerr.Log("0X%X�����Գ�ʧ��!%s",dbObjPtrArr[i]->handle,sErrorInfo);
				else
					logerr.Log("0X%X�����Գ�ʧ��!",dbObjPtrArr[i]->handle);
#endif
				nError++;
			}
		}
	}
	delete []dbObjPtrArr;
	g_pSolidDraw->Draw();
	mirror.Finished();
	if(nError==0)
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("Mirror successfully!");
#else
		MessageBox("�ԳƳɹ�!");
#endif
}

void CLDSView::OnSyncMirObjProp()
{
	long *id_arr=NULL;
	int i=0,n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	int hits=0;
	CLogErrorLife errorcycle;
	CUndoOperObject undo(&Ta);
	for(i=0;i<n;i++)
	{
		CLDSNode* pNode=Ta.Node.FromHandle(id_arr[i]);
		if(pNode && pNode->SyncMirProp())
		{
			hits++;
			continue;
		}
		CLDSPart* pPart=Ta.Parts.FromHandle(id_arr[i]);
		CAngleMoveOperObject anglemove;
		if(pPart==NULL||pPart->GetClassTypeId()!=CLS_LINEANGLE)
			anglemove.Enable(false);
		if(pPart && pPart->SyncMirProp())
			hits++;
		if(pPart!=NULL&&pPart->GetClassTypeId()==CLS_LINEANGLE)
			anglemove.Terminate(true,(CLDSLineAngle*)pPart);
	}
	if(hits>0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox(CXhChar50("%d data objects associated group are synchronized!",hits));
#else
		MessageBox(CXhChar50("��ͬ��%d�����ݶ��������!",hits));
#endif
		RedrawAll();
	}
}

void CLDSView::OnRecreateSpecPartSolidmodel() 
{	//�������ɹ���
	m_nPrevCommandID=ID_RECREATE_SPEC_PART_SOLIDMODEL;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat to generate part";
#else
	m_sPrevCommandName="�ظ��������ɹ���";
#endif
	long *id_arr=NULL;
	int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	try{
		for(int i=0;i<n;i++)
		{
			CLDSPart *pPart=console.FromPartHandle(id_arr[i]);
			if(pPart)
			{
				pPart->SetModified();
				pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
			}
			else if(console.GetActiveModel()==&Ta)
			{
				CBlockReference *pBlkRef=Ta.BlockRef.FromHandle(id_arr[i]);
				if(pBlkRef)
				{
					pBlkRef->SetModified();
					pBlkRef->Create3dSolidModel(g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
					g_pSolidDraw->NewSolidPart(pBlkRef->GetSolidPartObject());
				}
			}
		}
		g_pSolidDraw->Draw();
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
}

void CLDSView::OnDesignLinepartOddment() 
{	//�趨����ͷ
	m_nPrevCommandID=ID_DESIGN_LINEPART_ODDMENT;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat to set ends";
#else
	m_sPrevCommandName="�ظ��趨����ͷ";
#endif
	Command("DesignOddment");
}

#ifdef __ANGLE_PART_INC_
int CLDSView::DesignOddment()
{
	CString cmdStr;
	CSuperSmartPtr<CLDSPart> pDatumPart,pRayPart;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	g_pSolidDraw->ReleaseSnapStatus();
	f3dLine line;
#ifdef AFX_TARG_ENU_ENGLISH
    pCmdLine->FillCmdLine("DesignOddment Please select a rod or a sphere as the check datum for collision calculation of ends:","");
#else
	pCmdLine->FillCmdLine("DesignOddment ��ѡ��һ���˼���һ��������Ϊ��ײ��������ͷ�������׼:","");
#endif
	while(1)
	{
		if(pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			long *id_arr;
			if(g_pSolidSnap->GetLastSelectEnts(id_arr)>0)
			{
				pDatumPart=console.FromPartHandle(id_arr[0]);
				if(pDatumPart.IsHasPtr()&&(pDatumPart->IsLinePart()||pDatumPart->GetClassTypeId()==CLS_SPHERE))
					break;
				else
				{
					pDatumPart=NULL;
					g_pSolidDraw->ReleaseSnapStatus();
					pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("Not selected the appropriate part��please select again:","");
#else
					pCmdLine->FillCmdLine("û��ѡ�к��ʵĹ�����������ѡ��:","");
#endif
					continue;
				}
			}
		}
		else
		{
			g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
			return 0;
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
    pCmdLine->FillCmdLine("Please select a rod to be calculated ends, Enter key to confirm:","");
#else
	pCmdLine->FillCmdLine("��ѡ����Ҫ��������ͷ�ĸ˼�,�س���ȷ��:","");
#endif
	g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_LINEANGLE)|GetSingleWord(SELECTINDEX_LINETUBE)|
						GetSingleWord(SELECTINDEX_LINEFLAT)|GetSingleWord(SELECTINDEX_LINESLOT));
	OpenWndSel();	//����ѡ����Ҫ��������ͷ�ĸ˼�
	LINEPARTSET raypartset;
	if(pCmdLine->GetStrFromCmdLine(cmdStr))
	{	
		long *id_arr;
		int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
		for(int i=0;i<n;i++)
		{
			pRayPart=console.FromPartHandle(id_arr[i]);
			if(pRayPart.IsHasPtr()&&pRayPart->IsLinePart()&&!pRayPart.IsEqualPtr(pDatumPart))
				raypartset.append(pRayPart.LinePartPointer());
		}
	}
	else
	{
		pCmdLine->CancelCmdLine();
		return 0;
	}
	g_pSolidDraw->ReleaseSnapStatus();
	OnOperOther();	//ȡ����������״̬
	g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	CInputAnIntegerValDlg weldspaceDlg;
#ifdef AFX_TARG_ENU_ENGLISH
    weldspaceDlg.m_sValTitle="Collision clearance:";//"datum material weld height:";
#else
	weldspaceDlg.m_sValTitle="��ײ��϶:";//"��׼�ĺ���߶�:";
#endif
	weldspaceDlg.m_nVal=0;
	weldspaceDlg.DoModal();
	if(pDatumPart->IsLinePart())
	{
		line.startPt=pDatumPart.LinePartPointer()->pStart->Position(false);
		line.endPt  =pDatumPart.LinePartPointer()->pEnd->Position(false);
	}
	Ta.BeginUndoListen();
	for(pRayPart=raypartset.GetFirst();pRayPart.IsHasPtr();pRayPart=raypartset.GetNext())
	{
		int start0_end1=-1;
		if(pDatumPart->IsLinePart())
		{
			if(pRayPart.LinePartPointer()->pStart&&pRayPart.LinePartPointer()->pStart->hFatherPart==pDatumPart->handle)
				start0_end1=0;
			else if(pRayPart.LinePartPointer()->pEnd&&pRayPart.LinePartPointer()->pEnd->hFatherPart==pDatumPart->handle)
				start0_end1=1;
			else if(pRayPart.LinePartPointer()->pStart&&line.PtInLine(pRayPart.LinePartPointer()->pStart->Position(false))!=0)
				start0_end1=0;
			else if(pRayPart.LinePartPointer()->pEnd&&line.PtInLine(pRayPart.LinePartPointer()->pEnd->Position(false))!=0)
				start0_end1=1;
			else
			{
				g_pSolidDraw->SetEntSnapStatus(pRayPart->handle);
#ifdef AFX_TARG_ENU_ENGLISH
				if(MessageBox("Will the starting of rod be the design side of current ends(Y/N)?","LDS",MB_YESNO)==IDYES)
#else
				if(MessageBox("�˼���ʼ��Ϊ��ǰ����ͷ��ƶ���(Y/N)?","LDS",MB_YESNO)==IDYES)
#endif
					start0_end1=0;
				else
					start0_end1=1;
				g_pSolidDraw->ReleaseSnapStatus();
			}
		}
		else if(pDatumPart->GetClassTypeId()==CLS_SPHERE)
		{
			if(pRayPart.LinePartPointer()->pStart&&pRayPart.LinePartPointer()->pStart->handle==pDatumPart.SpherePointer()->hDatumNode)
				start0_end1=0;
			else if(pRayPart.LinePartPointer()->pEnd&&pRayPart.LinePartPointer()->pEnd->handle==pDatumPart.SpherePointer()->hDatumNode)
				start0_end1=1;
			else
			{
				g_pSolidDraw->SetEntSnapStatus(pRayPart->handle);
#ifdef AFX_TARG_ENU_ENGLISH
				if(MessageBox("Will the starting of rod be the design side of current ends(Y/N)?","LDS",MB_YESNO)==IDYES)
#else
				if(MessageBox("�˼���ʼ��Ϊ��ǰ����ͷ��ƶ���(Y/N)?","LDS",MB_YESNO)==IDYES)
#endif
					start0_end1=0;
				else
					start0_end1=1;
				g_pSolidDraw->ReleaseSnapStatus();
			}
		}
		if(start0_end1==0)
		{
			 if(pRayPart.LinePartPointer()->desStartOdd.m_hRefPart1!=pDatumPart->handle && 
				 pRayPart.LinePartPointer()->desStartOdd.m_hRefPart2!=pDatumPart->handle)
			 {
				 CLDSPart *pCollidePart=console.FromPartHandle(pRayPart.LinePartPointer()->desStartOdd.m_hRefPart1);
				 if(pCollidePart==NULL)
					 pRayPart.LinePartPointer()->desStartOdd.m_hRefPart1=pDatumPart->handle;
				 else
				 {
					 pCollidePart=console.FromPartHandle(pRayPart.LinePartPointer()->desStartOdd.m_hRefPart2);
					 if(pCollidePart==NULL)
						 pRayPart.LinePartPointer()->desStartOdd.m_hRefPart2=pDatumPart->handle;
				 }
			 }
			pRayPart.LinePartPointer()->desStartOdd.m_fCollideDist=weldspaceDlg.m_nVal;//+g_sysPara.VertexDist;
			pRayPart.LinePartPointer()->CalStartOddment();
		}
		else
		{
			if(pRayPart.LinePartPointer()->desEndOdd.m_hRefPart1!=pDatumPart->handle && 
				 pRayPart.LinePartPointer()->desEndOdd.m_hRefPart2!=pDatumPart->handle)
			 {
				 CLDSPart *pCollidePart=console.FromPartHandle(pRayPart.LinePartPointer()->desEndOdd.m_hRefPart1);
				 if(pCollidePart==NULL)
					 pRayPart.LinePartPointer()->desEndOdd.m_hRefPart1=pDatumPart->handle;
				 else
				 {
					 pCollidePart=console.FromPartHandle(pRayPart.LinePartPointer()->desEndOdd.m_hRefPart2);
					 if(pCollidePart==NULL)
						 pRayPart.LinePartPointer()->desEndOdd.m_hRefPart2=pDatumPart->handle;
				 }
			 }
			pRayPart.LinePartPointer()->desEndOdd.m_fCollideDist=weldspaceDlg.m_nVal;//+g_sysPara.VertexDist;
			pRayPart.LinePartPointer()->CalEndOddment();
		}
		pRayPart->SetModified();
		pRayPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pRayPart->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();	
	m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	Ta.EndUndoListen();

#ifdef AFX_TARG_ENU_ENGLISH
    pCmdLine->FillCmdLine("Command:","");
#else
	pCmdLine->FillCmdLine("����:","");
#endif
	//m_bCmdLineIdle=TRUE;
	Command("DesignOddment");
	return 0;
}
#endif

void CLDSView::OnUpdateImportBoltRef(CCmdUI* pCmdUI) 
{	
#ifdef __ANGLE_PART_INC_
	long *id_arr=NULL;
	if(g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
	{
		CLDSPart *pPart = console.FromPartHandle(id_arr[0]);
		if(!pPart)
			return;
		if(pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE||
			pPart->IsArcPart()||pPart->IsLinePart())
			pCmdUI->Enable();
		else
			pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(FALSE);	
#endif
}
/*
void CLDSView::OnUpdateLookBolt(CCmdUI* pCmdUI) 
{	
#ifdef __ANGLE_PART_INC_
	long *id_arr=NULL;
	if(g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
	{
		CLDSPart *pPart = console.FromPartHandle(id_arr[0]);
		if(!pPart)
			return;
		if(pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE||
			pPart->IsArcPart()||pPart->IsLinePart())
			pCmdUI->Enable();
		else
			pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(FALSE);
#endif
}
*/
void CLDSView::OnUpdateCreateBolt(CCmdUI* pCmdUI) 
{	
#ifdef __ANGLE_PART_INC_
	if(console.m_pWorkPart&&m_curAppType==TA_ASSEMBLE_ENV)
	{	//ѡ�е�������֧���ڸ˼���Բ�������Լ��ְ���������˨ wht 10-09-09
		if(!console.m_pWorkPart->IsArcPart()&&!console.m_pWorkPart->IsLinePart()
			&&console.m_pWorkPart->GetClassTypeId()!=CLS_PLATE&&console.m_pWorkPart->GetClassTypeId()!=CLS_PARAMPLATE)
			pCmdUI->Enable(FALSE);
		else
			pCmdUI->Enable();
	}
	else
	{
		long *id_arr=NULL;
		if(g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
		{
			CLDSPart *pPart = console.FromPartHandle(id_arr[0]);
			if(!pPart)
				return;
			if(pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE)
				pCmdUI->Enable();
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
#endif
}

void CLDSView::OnComposeLinepart() 
{
	m_nPrevCommandID=ID_COMPOSE_LINEPART;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat to combine rod";
#else
	m_sPrevCommandName="�ظ��ϲ��˼�";
#endif
	Command("ComposeLinePart");
}

CMirMsgDlg mir_dlg;
BOOL FinishComposLinePart(LINEPARTSET &partset,CLDSNode *pStart,CLDSNode *pEnd,BOOL bMechanicalPersist=FALSE,BOOL bFullMode=TRUE)
{
	CUndoOperObject undo(&Ta,true);
	CSuperSmartPtr<CLDSLinePart> pLastLinePart, pFinalLinePart;
	pFinalLinePart=partset.GetFirst();
	pLastLinePart=partset.GetTail();

	NODESET middlenodes;
	CLDSNode* pBreakLinkNode=NULL;	//��ǰ��Ԫ����һ��Ԫ�����Ӷ˽ڵ��ĩβ�ڵ�
	CLDSNode* pCurrLinkNode=pStart;	//��ǰ��Ԫ����һ��Ԫ�����Ӷ˽ڵ��ĩβ�ڵ�
	for(CLDSLinePart* pRod=partset.GetFirst();pRod;pRod=partset.GetNext())
	{
		if(pRod->pStart==pCurrLinkNode)
			pCurrLinkNode=pRod->pEnd;
		else if(pRod->pEnd==pCurrLinkNode)
			pCurrLinkNode=pRod->pStart;
		if(pRod!=pLastLinePart)	//�ϲ��˼�����м�ڵ�
			middlenodes.append(pCurrLinkNode);
	}
#ifdef __PART_DESIGN_INC_
	CLDSLinePart* pFirstRod=partset.GetFirst();
	if(pFirstRod)
	{
		int iFirstMechanicalType=1;	//��ͨ������
		if(pFirstRod->Layer(1)=='Z')
			iFirstMechanicalType=0;
		else if(pFirstRod->Layer(1)=='F')
			iFirstMechanicalType=2;	//����֧�Ųģ�����Ԫ����ʱ��������
		LINEPARTSET subrodset;
		if(pFirstRod->pStart==pStart)
			pCurrLinkNode=pFirstRod->pEnd;
		else if(pFirstRod->pEnd==pStart)
			pCurrLinkNode=pFirstRod->pStart;
		CLDSLinePart* pLastValidRod=pFirstRod;
		for(CLDSLinePart* pRod=partset.GetNext();pRod;pRod=partset.GetNext())
		{
			if(pFirstRod->IsLegObj()!=pRod->IsLegObj() || subrodset.GetNodeNum()>0)
			{
				if(bMechanicalPersist)
				{
					if(subrodset.GetNodeNum()==0)
						pBreakLinkNode=pCurrLinkNode;
					subrodset.append(pRod);
					partset.DeleteCursor();
					continue;
				}
				else
					logerr.Log("0X%X�˼���0X%X�˼�һ�����Ȳ��˼�����һ�������˼����ϲ����ܵ�����ѧ��ƽ���ı仯��",pFirstRod->handle,pRod->handle);
			}
			int iMechanicalType=1;	//��ͨ������
			if(pRod->Layer(1)=='Z')
				iMechanicalType=0;
			else if(pRod->Layer(1)=='F')
				iMechanicalType=2;	//����֧�Ųģ�����Ԫ����ʱ��������
			if(iFirstMechanicalType!=iMechanicalType || subrodset.GetNodeNum()>0)
			{
				if(bMechanicalPersist)
				{
					if(subrodset.GetNodeNum()==0)
						pBreakLinkNode=pCurrLinkNode;
					subrodset.append(pRod);
					partset.DeleteCursor();
					continue;
				}
				else
					logerr.Log("0X%X�˼���0X%X�˼����ڲ�ͬ�������ͣ����ġ������ġ�����֧�Ųģ��ĸ˼����ϲ����ܵ�����ѧ�������ı仯��",pFirstRod->handle,pRod->handle);
			}
			if(pFirstRod->pStart==pCurrLinkNode)
				pCurrLinkNode=pFirstRod->pEnd;
			else if(pFirstRod->pEnd==pCurrLinkNode)
				pCurrLinkNode=pFirstRod->pStart;
			pLastValidRod=pRod;
		}
		BOOL rslt=FALSE;
		if(subrodset.GetNodeNum()>1)
			rslt=FinishComposLinePart(subrodset,pBreakLinkNode,pEnd,bMechanicalPersist,bFullMode);
		if(partset.GetNodeNum()<2)
			return rslt;
	}
#endif
#ifdef __ANGLE_PART_INC_
	if(pFinalLinePart->GetClassTypeId()==CLS_LINEANGLE||pFinalLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		if(pStart==pFinalLinePart->pEnd)
		{
			pFinalLinePart.LineAnglePointer()->connectStart = pFinalLinePart.LineAnglePointer()->connectEnd;
			pFinalLinePart.LineAnglePointer()->desStartPos=pFinalLinePart.LineAnglePointer()->desEndPos;
			DESIGN_JGWING_NORM des_wing_norm=pFinalLinePart.LineAnglePointer()->des_norm_x;
			pFinalLinePart.LineAnglePointer()->des_norm_x=pFinalLinePart.LineAnglePointer()->des_norm_y;
			pFinalLinePart.LineAnglePointer()->des_norm_y=des_wing_norm;
			int style;
			double dist;
			style=pFinalLinePart.LineAnglePointer()->desStartPos.wing_x_offset.gStyle;
			dist=pFinalLinePart.LineAnglePointer()->desStartPos.wing_x_offset.offsetDist;
			pFinalLinePart.LineAnglePointer()->desStartPos.wing_x_offset.gStyle=
				pFinalLinePart.LineAnglePointer()->desStartPos.wing_y_offset.gStyle;
			pFinalLinePart.LineAnglePointer()->desStartPos.wing_x_offset.offsetDist=
				pFinalLinePart.LineAnglePointer()->desStartPos.wing_y_offset.offsetDist;
			pFinalLinePart.LineAnglePointer()->desStartPos.wing_y_offset.gStyle=style;
			pFinalLinePart.LineAnglePointer()->desStartPos.wing_y_offset.offsetDist=dist;
			pFinalLinePart.LineAnglePointer()->desStartOdd=pFinalLinePart.LineAnglePointer()->desEndOdd;
		}
		if(pEnd==pLastLinePart->pStart)
		{
			pFinalLinePart.LineAnglePointer()->connectEnd = pLastLinePart.LineAnglePointer()->connectStart;
			pFinalLinePart.LineAnglePointer()->desEndPos=pLastLinePart.LineAnglePointer()->desStartPos;
			pFinalLinePart.LineAnglePointer()->desEndOdd=pLastLinePart.LineAnglePointer()->desStartOdd;
		}
		else // if(pEnd==pLastLinePart->pEnd)
		{
			pFinalLinePart.LineAnglePointer()->connectEnd = pLastLinePart.LineAnglePointer()->connectEnd;
			pFinalLinePart.LineAnglePointer()->desEndPos=pLastLinePart.LineAnglePointer()->desEndPos;
			pFinalLinePart.LineAnglePointer()->desEndOdd=pLastLinePart.LineAnglePointer()->desEndOdd;
		}
	}
	else if(pFinalLinePart->GetClassTypeId()==CLS_LINETUBE)
	{
		if(pStart==pFinalLinePart->pEnd)
		{
			pFinalLinePart.LineTubePointer()->desStartPos=pFinalLinePart.LineTubePointer()->desEndPos;
			pFinalLinePart.LineTubePointer()->m_tJointStart=pFinalLinePart.LineTubePointer()->m_tJointEnd;
			pFinalLinePart.LineTubePointer()->desStartOdd=pFinalLinePart.LineTubePointer()->desEndOdd;
		}
		if(pEnd==pLastLinePart->pStart)
		{
			pFinalLinePart.LineTubePointer()->desStartPos=pLastLinePart.LineTubePointer()->desStartPos;
			pFinalLinePart.LineTubePointer()->m_tJointStart=pLastLinePart.LineTubePointer()->m_tJointStart;
			pFinalLinePart.LineTubePointer()->desStartOdd=pLastLinePart.LineTubePointer()->desStartOdd;
		}
		else // if(pEnd==pLastLinePart->pEnd)
		{
			pFinalLinePart.LineTubePointer()->desStartPos=pLastLinePart.LineTubePointer()->desEndPos;
			pFinalLinePart.LineTubePointer()->m_tJointStart=pLastLinePart.LineTubePointer()->m_tJointEnd;
			pFinalLinePart.LineTubePointer()->desStartOdd=pLastLinePart.LineTubePointer()->desEndOdd;
		}
		pFinalLinePart.LineTubePointer()->m_bHasWeldRoad=pLastLinePart.LineTubePointer()->m_bHasWeldRoad;
	}
	pFinalLinePart->pStart=pStart;
	pFinalLinePart->pEnd=pEnd;
	pFinalLinePart->SetStart(pStart->ActivePosition());
	pFinalLinePart->SetEnd(pEnd->ActivePosition());
	if(bFullMode)
	{	//��ģʽ�ϲ�ʱ��Ҫ�����ø˼�λ�ã������������д��󱨾� wjh-2015.12.28
		pFinalLinePart->CalPosition();
		pFinalLinePart->CalStartOddment();
		pFinalLinePart->CalEndOddment();
	}
	for(CLDSNode* pNode=middlenodes.GetFirst();pNode;pNode=middlenodes.GetNext())
		pNode->hFatherPart=pFinalLinePart->handle;
	for(CSuperSmartPtr<CLDSLinePart> pSelLinePart=partset.GetFirst();pSelLinePart.IsHasPtr();pSelLinePart=partset.GetNext())
	{
		//�˼���ʼ/�ն˽ڵ㸸�˼�Ϊ�˼�����򸸸˼�������ʱ����ʼ/�ն˽ڵ㸸�˼�Ϊ�ϲ���ĸ˼� wht 11-04-25
		if(console.FromPartHandle(pSelLinePart->pStart->hFatherPart,CLS_LINEPART)==NULL//���˼�������
			||pSelLinePart->pStart->hFatherPart==pSelLinePart->handle)				//�ڵ㸸�˼�Ϊ�˼�����
		{
			pSelLinePart->pStart->hFatherPart = pFinalLinePart->handle;
			pSelLinePart->pStart->SetModified();
		}
		if(console.FromPartHandle(pSelLinePart->pEnd->hFatherPart,CLS_LINEPART)==NULL	//���˼�������
			||pSelLinePart->pEnd->hFatherPart==pSelLinePart->handle)				//�ڵ㸸�˼�Ϊ�˼�����
		{
			pSelLinePart->pEnd->hFatherPart=pFinalLinePart->handle;
			pSelLinePart->pEnd->SetModified();
		}
		CLDSGroupLineAngle* pSelGroupAngle=NULL;
		if(pSelLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//ɾ����ϽǸ�ǰ���Ƴ���ϽǸֵ��ӽǸ�
			pSelGroupAngle=pSelLinePart.GroupAnglePointer();
			for(int j=0;j<4;j++)
			{
				if(pSelGroupAngle->son_jg_h[j]<=0)
					continue;
				console.DispPartSet.DeleteNode(pSelGroupAngle->son_jg_h[j]);
				g_pSolidDraw->DelEnt(pSelGroupAngle->son_jg_h[j]);
			}
		}
		console.DispPartSet.DeleteNode(pSelLinePart->handle);
		g_pSolidDraw->DelEnt(pSelLinePart->handle);
		if(!pSelLinePart.IsEqualPtr(pFinalLinePart))
		{
			CLsRefList *pSelLsRefList=pSelLinePart->GetLsRefList();
			CLsRefList *pFinalLsRefList=pFinalLinePart->GetLsRefList();
			for (CLsRef *pLsRef=pSelLsRefList->GetFirst();pLsRef;pLsRef=pSelLsRefList->GetNext())
				pFinalLsRefList->append(*pLsRef);
			//�жϺϲ��˼����Ƿ�ʼ�ն˷�ת��XY֫�Ƿ�ת
			bool bReverseSAndE=false,bReverseXAndY=false;
			f3dPoint sel_line_vec,final_line_vec;
			sel_line_vec=pSelLinePart->End()-pSelLinePart->Start();
			//normalize(sel_line_vec);
			final_line_vec=pFinalLinePart->End()-pFinalLinePart->Start();
			//normalize(final_line_vec);
			if(sel_line_vec*final_line_vec<0)
				bReverseSAndE=true;
			if(pSelLinePart->IsAngle())
			{
				f3dPoint sel_norm_x,sel_norm_y,final_norm_x;
				sel_norm_x=pSelLinePart.LineAnglePointer()->get_norm_x_wing();
				sel_norm_y=pSelLinePart.LineAnglePointer()->get_norm_y_wing();
				final_norm_x=pFinalLinePart.LineAnglePointer()->get_norm_x_wing();
				if(fabs(sel_norm_y*final_norm_x)>EPS_COS2 && fabs(sel_norm_x*final_norm_x)<EPS2)
					bReverseXAndY=true;
			}
			ANGLEREPLACER_TAG angle_tag;
			CHandleReplacer handleReplacer(pSelLinePart->handle,pFinalLinePart->handle,0,false,bReverseSAndE,bReverseXAndY);
			handleReplacer.pTag=bFullMode?&angle_tag:NULL;
			Ta.ReplaceReferenceHandle(handleReplacer);
			if(pSelGroupAngle)
			{	//ɾ����ϽǸ�ǰ���Ƴ���ϽǸֵ��ӽǸ�
				CLDSGroupLineAngle* pFinalGroupAngle=pFinalLinePart.GroupAnglePointer();
				for(int j=0;j<4;j++)
				{
					if(pSelGroupAngle->son_jg_h[j]<=0)
						continue;
					console.DeletePart(pSelGroupAngle->son_jg_h[j]);
					int mapping_index=j;
					if(pSelGroupAngle->size_cSubClassType=='T'&&bReverseSAndE&&j<2)
						mapping_index=1-j;	//T����ϽǸ���Ҫ���⴦��
					CHandleReplacer handleReplacer(pSelGroupAngle->son_jg_h[j],pFinalLinePart.GroupAnglePointer()->son_jg_h[mapping_index],0,false,bReverseSAndE,bReverseXAndY);
					handleReplacer.pTag=bFullMode?&angle_tag:NULL;
					Ta.ReplaceReferenceHandle(handleReplacer);
				}
			}
			console.DeletePart(pSelLinePart->handle);
		}
	}
	NewLinePart(pFinalLinePart,bFullMode!=FALSE);
	if(!bFullMode)
		return TRUE;	//��ģʽ�ϲ�ʱ��Ҫ����������ʵ��
	if(pFinalLinePart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
	{
		pFinalLinePart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pFinalLinePart->GetSolidPartObject());
	}
	else if(pFinalLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		pFinalLinePart.GroupAnglePointer()->UpdateSonJgPos();
		for(int j=0;j<4;j++)
		{
			if(pFinalLinePart.GroupAnglePointer()->son_jg_h[j]<=0)
				continue;
			CLDSLineAngle* pSonAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pFinalLinePart.GroupAnglePointer()->son_jg_h[j],CLS_LINEANGLE);
			if(pSonAngle!=NULL)
			{
				pSonAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole);
				g_pSolidDraw->NewSolidPart(pSonAngle->GetSolidPartObject());
			}
		}
	}
#endif
	return TRUE;
}

#ifdef __ANGLE_PART_INC_
BOOL CLDSView::ComposeLinePart() 
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr;
#ifdef AFX_TARG_ENU_ENGLISH
    pCmdLine->FillCmdLine("ComposeLinePart Combine rod, Please select the child rods to be combined in turn:","");
#else
	pCmdLine->FillCmdLine("ComposeLinePart �ϲ��˼� ������ѡ��Ҫ�ϲ����Ӹ˼�:","");
#endif
	CLogErrorLife loglife;
	//�л���������ʾ״̬
	g_pSolidSet->SetDisplayType(DISP_LINE);
	Invalidate(FALSE);
	CLDSNode *pStart,*pEnd,*pMirNode=NULL;
	CSuperSmartPtr<CLDSLinePart> pSelLinePart,pFinalLinePart,pMirLinePart,pPrevLinePart;
	LINEPARTSET partset;
	f3dPoint start,end,last_vec,vec;
	THANDLE hLastStartNode,hLastEndNode=0;
	f3dLine line_3d;
	try{
		while(1)
		{
			int ret = g_pSolidSnap->SnapLine(line_3d);
			if(ret<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			else if(ret>0)
			{
				CLDSPart *pPart=console.FromPartHandle(line_3d.ID);
				if(pPart&&pPart->IsLinePart())
					pSelLinePart = (CLDSLinePart*)pPart;
				else
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("Select an illegal rod, please select again!");
#else
					AfxMessageBox("ѡ����һ���Ƿ��˼�,������ѡ��!");
#endif
					continue;
				}
				if(pPrevLinePart.IsHasPtr())
				{
					if(pPrevLinePart.IsEqualPtr(pSelLinePart))
					{
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("Select the same bar repeatedly, please select again!");
#else
						AfxMessageBox("�ظ�ѡ����ͬһ���˼�,������ѡ��!");
#endif
						continue;
					}
					//TODO:һ����ʵ��Ǹ֣���һ������ѧ�����½Ǹ֣�δʵ�廯�����ͻ��޷��ϲ���wjh-2016.6.1
					else if(pPrevLinePart->GetClassTypeId()!=pSelLinePart->GetClassTypeId())
					{
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("Rod types are different, fail to merge rods, please select again!");
#else
						AfxMessageBox("�˼����Ͳ�ͬ,���ܺϲ���Ϊһ���˼�,������ѡ��!");
#endif
						continue;
					}
					else if(pPrevLinePart->cMaterial!=pSelLinePart->cMaterial)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("Rod materials are different, fail to merge rods, please select again!");
#else
						AfxMessageBox("�˼����ʲ�ͬ,���ܺϲ���Ϊһ���˼�,������ѡ��!");
#endif
						continue;
					}
					else if(pSelLinePart->GetClassTypeId()==CLS_LINEANGLE)
					{
						if(pPrevLinePart.LineAnglePointer()->GetWidth()!=pSelLinePart.LineAnglePointer()->GetWidth()||
							pPrevLinePart.LineAnglePointer()->GetThick()!=pSelLinePart.LineAnglePointer()->GetThick())
						{	
#ifdef AFX_TARG_ENU_ENGLISH
							AfxMessageBox("Angle specifications are different,fail to merge");
#else
							AfxMessageBox("�Ǹֹ��ͬ���������Ϊͬһ���Ǹ�");
#endif
							continue;
						}
					}
					else if(pSelLinePart->GetClassTypeId()==CLS_LINETUBE)
					{
						if(pPrevLinePart.LineTubePointer()->GetDiameter()!=pSelLinePart.LineTubePointer()->GetDiameter()||
							pPrevLinePart.LineTubePointer()->GetThick()!=pSelLinePart.LineTubePointer()->GetThick())
						{	
#ifdef AFX_TARG_ENU_ENGLISH
							AfxMessageBox("Tube specifications are different,fail to merge");
#else
							AfxMessageBox("�ֹܹ��ͬ���������Ϊͬһ���ֹ�");
#endif
							continue;
						}
					}
				}
				if(pSelLinePart->pStart->handle==hLastEndNode)
				{
					hLastStartNode = pSelLinePart->pStart->handle;
					hLastEndNode = pSelLinePart->pEnd->handle;
					pEnd = pSelLinePart->pEnd;
				}
				else if(pSelLinePart->pEnd->handle==hLastEndNode)
				{
					hLastStartNode = pSelLinePart->pEnd->handle;
					hLastEndNode = pSelLinePart->pStart->handle;
					pEnd = pSelLinePart->pStart;
				}
				else if(pSelLinePart->pStart->handle==hLastStartNode&&partset.GetNodeNum()==1)
				{
					pStart = pPrevLinePart->pEnd;
					hLastStartNode = pSelLinePart->pStart->handle;
					hLastEndNode = pSelLinePart->pEnd->handle;
					pEnd = pSelLinePart->pEnd;
				}
				else if(pSelLinePart->pEnd->handle==hLastStartNode&&partset.GetNodeNum()==1)
				{
					pStart = pPrevLinePart->pEnd;
					hLastStartNode = pSelLinePart->pEnd->handle;
					hLastEndNode = pSelLinePart->pStart->handle;
					pEnd = pSelLinePart->pStart;
				}
				else if(hLastEndNode==0)	//��һ���Ǹ�
				{
					pStart = pSelLinePart->pStart;
					pEnd = pSelLinePart->pEnd;
					hLastStartNode = pSelLinePart->pStart->handle;
					hLastEndNode	  = pSelLinePart->pEnd->handle;
					pFinalLinePart = pSelLinePart;
				}
				else //if(hLastEndNode>0)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("There is no connection between the selected rod and the previous one ��please select again!");
#else
					AfxMessageBox("��ѡ�˼�����һ�˼�û�����ӣ�������ѡ��!");
#endif
					continue;
				}
				partset.append(pSelLinePart);
				pPrevLinePart = pSelLinePart;
				g_pSolidDraw->SetEntSnapStatus(pSelLinePart->handle,TRUE);
			}
			else	//�˼�ѡ�����
				break;
		}
		if(partset.GetNodeNum()<2||mir_dlg.DoModal()!=IDOK)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		//FinishComposLinePart(partset,pStart,pEnd);
		BYTE flagArr[3]={1,2,4};
		CLDSLinePart *pLinePart=NULL;
		LINEPARTSET mirPartSet;
		CUndoOperObject undo(&Ta,true);
		MIRMSG mirmsg;
		for(int i=1;i<5;i++)
		{
			mirPartSet.Empty();
			CLDSNode *pMirStart,*pMirEnd;
			if(i==4)
			{
				pMirStart=pStart;
				pMirEnd=pEnd;
				for(pLinePart=partset.GetFirst();pLinePart;pLinePart=partset.GetNext())
					mirPartSet.append(pLinePart);
				FinishComposLinePart(mirPartSet,pMirStart,pMirEnd);
			}
			else if(mir_dlg.mirmsg.axis_flag&flagArr[i-1])
			{
				mirmsg.axis_flag = flagArr[i-1];
				pMirStart=pStart->GetMirNode(mirmsg);
				pMirEnd=pEnd->GetMirNode(mirmsg);
				if(pMirStart==NULL||pMirEnd==NULL)
					continue;
				for(pLinePart=partset.GetFirst();pLinePart;pLinePart=partset.GetNext())
				{
					CSuperSmartPtr<CLDSLinePart> pMirLinePart=(CLDSLinePart*)pLinePart->GetMirPart(mirmsg);
					if(pMirLinePart.IsNULL())
						break;
					mirPartSet.append(pMirLinePart);
				}
				if(pLinePart)
					continue;
				FinishComposLinePart(mirPartSet,pMirStart,pMirEnd);
			}
			else
				continue;	//û�д˷���Գ�
			
			if(mirPartSet.GetNodeNum()>0&&(mir_dlg.mirmsg.axis_flag&8))
			{
				mirmsg.axis_flag = mir_dlg.mirmsg.axis_flag;//8;	//��ת�Գ�
				mirmsg.array_num = mir_dlg.mirmsg.array_num;
				mirmsg.rotate_angle = mir_dlg.mirmsg.rotate_angle;
				mirmsg.origin = mir_dlg.mirmsg.origin;
				CLDSNode *pRotateMirStart=pStart->GetMirNode(mirmsg);
				CLDSNode *pRotateMirEnd=pEnd->GetMirNode(mirmsg);
				if(pRotateMirStart==NULL||pRotateMirEnd==NULL)
					continue;
				LINEPARTSET sidemirPartSet;
				for(pLinePart=partset.GetFirst();pLinePart;pLinePart=partset.GetNext())
				{
					CSuperSmartPtr<CLDSLinePart> pSideMirLinePart=(CLDSLinePart*)pLinePart->GetMirPart(mirmsg);
					if(pSideMirLinePart.IsNULL())
						break;
					sidemirPartSet.append(pSideMirLinePart);
				}
				if(pLinePart)
					continue;
				FinishComposLinePart(sidemirPartSet,pRotateMirStart,pRotateMirEnd);
			}	
		}
		/*if(partset.GetNodeNum()>0&&(mir_dlg.mirmsg.axis_flag&8))
		{
			mirmsg.axis_flag = 8;	//��ת�Գ�
			mirmsg.array_num = mir_dlg.mirmsg.array_num;
			mirmsg.rotate_angle = mir_dlg.mirmsg.rotate_angle;
			mirmsg.origin = mir_dlg.mirmsg.origin;
			mirPartSet.Empty();
			CLDSNode *pRotateMirStart=pStart->GetMirNode(mirmsg);
			CLDSNode *pRotateMirEnd=pEnd->GetMirNode(mirmsg);
			LINEPARTSET sidemirPartSet;
			if(pRotateMirStart!=NULL&&pRotateMirEnd!=NULL)
			{
				for(pLinePart=partset.GetFirst();pLinePart;pLinePart=partset.GetNext())
				{
					CSuperSmartPtr<CLDSLinePart> pMirLinePart=(CLDSLinePart*)pLinePart->GetMirPart(mirmsg);
					if(pMirLinePart.IsNULL())
						break;
					sidemirPartSet.append(pMirLinePart);
				}
				if(pLinePart==NULL)
					FinishComposLinePart(sidemirPartSet,pRotateMirStart,pRotateMirEnd);
			}
		}*/	
	}
	catch(char* sError)
	{
		Ta.EndUndoListen();
		AfxMessageBox(sError);
		pCmdLine->CancelCmdLine();
		return FALSE;
	}
	RedrawAll();
	m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	ComposeLinePart();
	return TRUE;
}
#endif
extern int CompareRodPointI(CLDSNode* pStart,CLDSNode* pEnd,CLDSNode* pStart2,CLDSNode* pEnd2);
void CLDSView::CheckRelativeMirInfo(CLDSDbObject *pObject,int* bad_hits,CTaskOutputList* pInfoList)
{
	RELATIVE_OBJECT* pRelaObj;
	if(pObject->GetClassTypeId()==CLS_NODE)
	{
		CLDSNode *pNode=(CLDSNode*)pObject;
		for(pRelaObj=pNode->relativeObjs.GetFirst();pRelaObj;pRelaObj=pNode->relativeObjs.GetNext())
		{
			if(pRelaObj->hObj==pNode->handle)	//�ԶԳ�
			{
				pNode->relativeObjs.DeleteCursor();
				continue;
			}
			CLDSNode *pMirNode=Ta.Node.FromHandle(pRelaObj->hObj);
			int bad_mir=0;
			if(pMirNode==NULL)
				bad_mir=-1;
			else if(pRelaObj->mirInfo.axis_flag==0)
				bad_mir=-4;
			else if( pRelaObj->mirInfo.axis_flag==0x01&&		//X��Գ�
				(fabs(pNode->Position(false).x-pMirNode->Position(false).x)>EPS||fabs(pNode->Position(false).y+pMirNode->Position(false).y)>EPS||fabs(pNode->Position(false).z-pMirNode->Position(false).z)>EPS))
				bad_mir=1;
			else if(pRelaObj->mirInfo.axis_flag==0x02&&	//Y��Գ�
				(fabs(pNode->Position(false).x+pMirNode->Position(false).x)>EPS||fabs(pNode->Position(false).y-pMirNode->Position(false).y)>EPS||fabs(pNode->Position(false).z-pMirNode->Position(false).z)>EPS))
				bad_mir=2;
			else if(pRelaObj->mirInfo.axis_flag==0x04&&	//Z��Գ�
				(fabs(pNode->Position(false).x+pMirNode->Position(false).x)>EPS||fabs(pNode->Position(false).y+pMirNode->Position(false).y)>EPS||fabs(pNode->Position(false).z-pMirNode->Position(false).z)>EPS))
				bad_mir=3;
			else
			{
				f3dPoint mirpos=pRelaObj->mirInfo.MirPoint(pNode->Position(false));
				if(!mirpos.IsEqual(pMirNode->Position(false)))
					bad_mir=-3;
			}
			if(bad_mir!=0)
			{
				if(pInfoList!=NULL)
				{
					++(*bad_hits);
					COutputInfo *pInfo=pInfoList->AppendSimpleOutput();
					pInfo->m_uType=INFO_EMEND_MIRRODINFO;
					pInfo->m_hRelaObj1=pNode->handle;
					pInfo->m_hRelaObj2=pRelaObj->hObj;
#ifdef AFX_TARG_ENU_ENGLISH
					if(bad_mir==-1)
						pInfo->m_sDescription.Format("Can't find 0X%X node's related object 0X%X,record is cleared!",pNode->handle,pRelaObj->hObj);
					else if(bad_mir==-3)
						pInfo->m_sDescription.Format("Actual of 0X%X node and its related object 0X%X isn't symmetric,record is cleared!\n",pNode->handle,pRelaObj->hObj);
					else if(bad_mir==-4)
						pInfo->m_sDescription.Format("Lack of symmetric information between 0X%X node and its related object 0X%X,record is cleared!\n",pNode->handle,pRelaObj->hObj);
					else if(bad_mir==1)
						pInfo->m_sDescription.Format("0X%X node with its X axis symmetric associated objects 0X%X isn't about X axial symmetry,record is cleared!",pNode->handle,pRelaObj->hObj);
					else if(bad_mir==2)
						pInfo->m_sDescription.Format("0X%X node with its Y axis symmetric associated objects 0X%X isn't about Y axial symmetry,record is cleared!",pNode->handle,pRelaObj->hObj);
					else if(bad_mir==3)
						pInfo->m_sDescription.Format("0X%X node with its Z axis symmetric associated objects 0X%X isn't about Z axial symmetry,record is cleared!",pNode->handle,pRelaObj->hObj);
#else 
					if(bad_mir==-1)
						pInfo->m_sDescription.Format("0X%X�ڵ�ĶԳƹ�������0X%X�Ҳ���,��¼�������",pNode->handle,pRelaObj->hObj);
					else if(bad_mir==-3)
						pInfo->m_sDescription.Format("0X%X�ڵ����Գƹ�������0X%Xʵ��λ�ò��Գ�,��¼�������\n",pNode->handle,pRelaObj->hObj);
					else if(bad_mir==-4)
						pInfo->m_sDescription.Format("0X%X�ڵ�����������0X%Xȱ�ٹ����Գ���Ϣ,��¼�������\n",pNode->handle,pRelaObj->hObj);
					else if(bad_mir==1)
						pInfo->m_sDescription.Format("0X%X�ڵ��X��Գƹ�������0X%X��X��Գ�,��¼�������",pNode->handle,pRelaObj->hObj);
					else if(bad_mir==2)
						pInfo->m_sDescription.Format("0X%X�ڵ��Y��Գƹ�������0X%X��Y��Գ�,��¼�������",pNode->handle,pRelaObj->hObj);
					else if(bad_mir==3)
						pInfo->m_sDescription.Format("0X%X�ڵ��Z��Գƹ�������0X%X��Z��Գ�,��¼�������",pNode->handle,pRelaObj->hObj);
#endif
				}
				pNode->relativeObjs.DeleteCursor();
			}
		}
	}
	else
	{
		CSuperSmartPtr<CLDSPart> pPart=(CLDSPart*)pObject;
		for(pRelaObj=pPart->relativeObjs.GetFirst();pRelaObj;pRelaObj=pPart->relativeObjs.GetNext())
		{
			if(pRelaObj->hObj==pPart->handle)	//�ԶԳ�
			{
				pPart->relativeObjs.DeleteCursor();
				continue;
			}
			CSuperSmartPtr<CLDSPart> pMirPart=Ta.Parts.FromHandle(pRelaObj->hObj,pPart->GetClassTypeId());
			bool bRodPart=false;
			if(pPart->IsLinePart()&&pPart.LinePartPointer()->pStart!=NULL&&pPart.LinePartPointer()->pEnd!=NULL&&
				pMirPart.IsHasPtr()&&pMirPart.LinePartPointer()->pStart!=NULL&&pMirPart.LinePartPointer()->pEnd!=NULL)
				bRodPart=true;
			int bad_mir=0;
			if(pMirPart.IsNULL())
				bad_mir=-1;
			else if(pRelaObj->mirInfo.axis_flag==0)
				bad_mir=-4;
			else if(pRelaObj->mirInfo.axis_flag>=0x08&&bRodPart)
			{
				f3dPoint mirstart=pRelaObj->mirInfo.MirPoint(pPart.LinePartPointer()->pStart->Position(false));
				f3dPoint mirend=pRelaObj->mirInfo.MirPoint(pPart.LinePartPointer()->pEnd->Position(false));
				f3dPoint mirstart2=pMirPart.LinePartPointer()->pStart->Position(false);
				f3dPoint mirend2=pMirPart.LinePartPointer()->pEnd->Position(false);
				bool b1=mirstart.IsEqual(mirstart2);
				bool b2=mirend.IsEqual(mirend2);
				bool b3=mirstart.IsEqual(mirend2);
				bool b4=mirend.IsEqual(mirstart2);
				if(!((b1&&b2)||(b3&&b4)))
					bad_mir=-3;	//ʵ��λ�ò��Գ�
			}
			else if(!pMirPart->IsLinePart())
				bad_mir=-2;
			else if( pRelaObj->mirInfo.axis_flag==0x01&&bRodPart&&		//X��Գ�
				((fabs(pPart.LinePartPointer()->pStart->Position(false).x-pMirPart.LinePartPointer()->pStart->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).x-pMirPart.LinePartPointer()->pEnd->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).z-pMirPart.LinePartPointer()->pStart->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).z-pMirPart.LinePartPointer()->pEnd->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).y+pMirPart.LinePartPointer()->pStart->Position(false).y)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).y+pMirPart.LinePartPointer()->pEnd->Position(false).y)>EPS)&&

				(fabs(pPart.LinePartPointer()->pStart->Position(false).x-pMirPart.LinePartPointer()->pEnd->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).x-pMirPart.LinePartPointer()->pStart->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).z-pMirPart.LinePartPointer()->pEnd->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).z-pMirPart.LinePartPointer()->pStart->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).y+pMirPart.LinePartPointer()->pEnd->Position(false).y)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).y+pMirPart.LinePartPointer()->pStart->Position(false).y)>EPS)))
				bad_mir=1;
			else if(pRelaObj->mirInfo.axis_flag==0x02&&bRodPart&&	//Y��Գ�
				((fabs(pPart.LinePartPointer()->pStart->Position(false).x+pMirPart.LinePartPointer()->pStart->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).x+pMirPart.LinePartPointer()->pEnd->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).z-pMirPart.LinePartPointer()->pStart->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).z-pMirPart.LinePartPointer()->pEnd->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).y-pMirPart.LinePartPointer()->pStart->Position(false).y)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).y-pMirPart.LinePartPointer()->pEnd->Position(false).y)>EPS)&&

				(fabs(pPart.LinePartPointer()->pStart->Position(false).x+pMirPart.LinePartPointer()->pEnd->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).x+pMirPart.LinePartPointer()->pStart->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).z-pMirPart.LinePartPointer()->pEnd->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).z-pMirPart.LinePartPointer()->pStart->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).y-pMirPart.LinePartPointer()->pEnd->Position(false).y)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).y-pMirPart.LinePartPointer()->pStart->Position(false).y)>EPS)))
				bad_mir=2;
			else if(pRelaObj->mirInfo.axis_flag==0x04&&bRodPart&&	//Z��Գ�
				((fabs(pPart.LinePartPointer()->pStart->Position(false).x+pMirPart.LinePartPointer()->pStart->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).x+pMirPart.LinePartPointer()->pEnd->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).z-pMirPart.LinePartPointer()->pStart->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).z-pMirPart.LinePartPointer()->pEnd->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).y+pMirPart.LinePartPointer()->pStart->Position(false).y)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).y+pMirPart.LinePartPointer()->pEnd->Position(false).y)>EPS)&&

				(fabs(pPart.LinePartPointer()->pStart->Position(false).x+pMirPart.LinePartPointer()->pEnd->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).x+pMirPart.LinePartPointer()->pStart->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).z-pMirPart.LinePartPointer()->pEnd->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).z-pMirPart.LinePartPointer()->pStart->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).y+pMirPart.LinePartPointer()->pEnd->Position(false).y)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).y+pMirPart.LinePartPointer()->pStart->Position(false).y)>EPS)))
				bad_mir=3;
			else if(pPart->cMaterial!=pMirPart->cMaterial)
			{
				if(CompareRodPointI(pPart.LinePartPointer()->pStart,pPart.LinePartPointer()->pEnd,
					pMirPart.LinePartPointer()->pStart,pMirPart.LinePartPointer()->pEnd)<=0)
					pMirPart->cMaterial=pPart->cMaterial;
				else
					pPart->cMaterial=pMirPart->cMaterial;
				bad_mir=4;
			}
#ifdef __PART_DESIGN_INC_//fined(__TSA_)||defined(__LDS_)
			else if(pPart.LinePartPointer()->m_iElemType!=pMirPart.LinePartPointer()->m_iElemType)
			{
				if(CompareRodPointI(pPart.LinePartPointer()->pStart,pPart.LinePartPointer()->pEnd,
					pMirPart.LinePartPointer()->pStart,pMirPart.LinePartPointer()->pEnd)<=0)
					pMirPart.LinePartPointer()->m_iElemType=pPart.LinePartPointer()->m_iElemType;
				else
					pPart.LinePartPointer()->m_iElemType=pMirPart.LinePartPointer()->m_iElemType;
				bad_mir=5;
			}
			else if(pPart.LinePartPointer()->start_joint_type!=pMirPart.LinePartPointer()->start_joint_type)
			{
				if(CompareRodPointI(pPart.LinePartPointer()->pStart,pPart.LinePartPointer()->pEnd,
					pMirPart.LinePartPointer()->pStart,pMirPart.LinePartPointer()->pEnd)<=0)
					pMirPart.LinePartPointer()->start_joint_type=pPart.LinePartPointer()->start_joint_type;
				else
					pPart.LinePartPointer()->start_joint_type=pMirPart.LinePartPointer()->start_joint_type;
				bad_mir=6;
			}
			else if(pPart.LinePartPointer()->end_joint_type!=pMirPart.LinePartPointer()->end_joint_type)
			{
				if(CompareRodPointI(pPart.LinePartPointer()->pStart,pPart.LinePartPointer()->pEnd,
					pMirPart.LinePartPointer()->pStart,pMirPart.LinePartPointer()->pEnd)<=0)
					pMirPart.LinePartPointer()->end_joint_type=pPart.LinePartPointer()->end_joint_type;
				else
					pPart.LinePartPointer()->end_joint_type=pMirPart.LinePartPointer()->end_joint_type;
				bad_mir=7;
			}
			else if(pPart.LinePartPointer()->start_force_type!=pMirPart.LinePartPointer()->start_force_type)
			{
				if(CompareRodPointI(pPart.LinePartPointer()->pStart,pPart.LinePartPointer()->pEnd,
					pMirPart.LinePartPointer()->pStart,pMirPart.LinePartPointer()->pEnd)<=0)
					pMirPart.LinePartPointer()->start_force_type=pPart.LinePartPointer()->start_force_type;
				else
					pPart.LinePartPointer()->start_force_type=pMirPart.LinePartPointer()->start_force_type;
				bad_mir=8;
			}
			else if(pPart.LinePartPointer()->end_force_type!=pMirPart.LinePartPointer()->end_force_type)
			{
				if(CompareRodPointI(pPart.LinePartPointer()->pStart,pPart.LinePartPointer()->pEnd,
					pMirPart.LinePartPointer()->pStart,pMirPart.LinePartPointer()->pEnd)<=0)
					pMirPart.LinePartPointer()->end_force_type=pPart.LinePartPointer()->end_force_type;
				else
					pPart.LinePartPointer()->end_force_type=pMirPart.LinePartPointer()->end_force_type;
				bad_mir=9;
			}
#endif
			if(bad_mir!=0&&pInfoList!=NULL)
			{
				++(*bad_hits);
				COutputInfo *pInfo=pInfoList->AppendSimpleOutput();
				pInfo->m_uType=INFO_EMEND_MIRRODINFO;
				pInfo->m_hRelaObj1=pPart->handle;
				pInfo->m_hRelaObj2=pRelaObj->hObj;
#ifdef AFX_TARG_ENU_ENGLISH
				if(bad_mir==-1)
					pInfo->m_sDescription.Format("Can't find 0X%X rod's related object 0X%X,record is cleared��\n",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==-2)
					pInfo->m_sDescription.Format("0X%X rod's related object 0X%X isn't a rod ,record is cleared��\n",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==-3)
					pInfo->m_sDescription.Format("Actual of 0X%X rod and its related object 0X%X isn't symmetric,record is cleared!\n",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==-4)
					pInfo->m_sDescription.Format("Lack of symmetric information between 0X%X rod and its related object 0X%X,record is cleared!\n",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==1)
					pInfo->m_sDescription.Format("0X%X node with its X axis symmetric associated objects 0X%X isn't about x axial symmetry,record is cleared��",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==2)
					pInfo->m_sDescription.Format("0X%X node with its Y axis symmetric associated objects 0X%X isn't about x axial symmetry,record is cleared��",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==3)
					pInfo->m_sDescription.Format("0X%X node with its Z axis symmetric associated objects 0X%X isn't about x axial symmetry,record is cleared��",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==4)
					pInfo->m_sDescription.Format("the material of 0X%X rod and associated object 0X%X  is different,it has been revised��please check it",pPart->handle,pRelaObj->hObj);
#ifdef __PART_DESIGN_INC_//defined(__TSA_)||defined(__LDS_)
				else if(bad_mir==5)
					pInfo->m_sDescription.Format("The unit type of 0X%X rod and associated object 0X%X is different,it has been revised��please check it",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==6)
					pInfo->m_sDescription.Format("The start restrictive types of 0X%X rod and associated object 0X%X is different,it has been revised��please check it",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==7)
					pInfo->m_sDescription.Format("The end restrictive types of 0X%X rod and associated object 0X%X is different,it has been revised��please check it",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==8)
					pInfo->m_sDescription.Format("The start stress type of 0X%X rod and associated object 0X%X is different,it has been revised��please check it",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==9)
					pInfo->m_sDescription.Format("The end stress type of 0X%X rod and associated object 0X%X is different,it has been revised��please check it",pPart->handle,pRelaObj->hObj);
#endif
#else 
				if(bad_mir==-1)
					pInfo->m_sDescription.Format("0X%X�˼��ĶԳƹ�������0X%X�Ҳ���,��¼�������\n",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==-2)
					pInfo->m_sDescription.Format("0X%X�˼��ĶԳƹ�������0X%X���Ǹ˼�,��¼�������\n",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==-3)
					pInfo->m_sDescription.Format("0X%X�˼�����Գƹ�������0X%Xʵ��λ�ò��Գ�,��¼�������\n",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==-4)
					pInfo->m_sDescription.Format("0X%X�˼������������0X%Xȱ�ٹ����Գ���Ϣ,��¼�������\n",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==1)
					pInfo->m_sDescription.Format("0X%X�˼���X��Գƹ�������0X%X��X��Գ�,��¼�������",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==2)
					pInfo->m_sDescription.Format("0X%X�˼���Y��Գƹ�������0X%X��X��Գ�,��¼�������",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==3)
					pInfo->m_sDescription.Format("0X%X�˼���Z��Գƹ�������0X%X��X��Գ�,��¼�������",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==4)
					pInfo->m_sDescription.Format("0X%X�˼�����Գƹ�������0X%X���ʲ�ͬ,�����������ʵ",pPart->handle,pRelaObj->hObj);
#ifdef __PART_DESIGN_INC_//defined(__TSA_)||defined(__LDS_)
				else if(bad_mir==5)
					pInfo->m_sDescription.Format("0X%X�˼�����Գƹ�������0X%X��Ԫ���Ͳ�ͬ,�����������ʵ",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==6)
					pInfo->m_sDescription.Format("0X%X�˼�����Գƹ�������0X%Xʼ������Լ�����Ͳ�ͬ,�����������ʵ��",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==7)
					pInfo->m_sDescription.Format("0X%X�˼�����Գƹ�������0X%X�ն�����Լ�����Ͳ�ͬ,�����������ʵ",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==8)
					pInfo->m_sDescription.Format("0X%X�˼�����Գƹ�������0X%Xʼ���������Ͳ�ͬ,�����������ʵ",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==9)
					pInfo->m_sDescription.Format("0X%X�˼�����Գƹ�������0X%X�ն��������Ͳ�ͬ,�����������ʵ",pPart->handle,pRelaObj->hObj);
#endif
#endif
			}
			if(bad_mir<=3&&bad_mir!=0)
				pPart->relativeObjs.DeleteCursor();
		}
		pPart->relativeObjs.Clean();
	}
}
BOOL CLDSView::EmendMirRodInfo(CLDSDbObject* pObject)
{
	RELATIVE_OBJECT relaObj;
	if(pObject->GetClassTypeId()==CLS_NODE)
	{
		CLDSNode *pNode=(CLDSNode*)pObject;
		CLDSNode *pXMirNode=pNode->GetMirNode(MIRMSG(1));
		CLDSNode *pYMirNode=pNode->GetMirNode(MIRMSG(2));
		CLDSNode *pZMirNode=pNode->GetMirNode(MIRMSG(3));
		if(pXMirNode==pNode)
			pXMirNode=NULL;
		if(pYMirNode==pNode)
			pYMirNode=NULL;
		if(pZMirNode==pNode)
			pZMirNode=NULL;
		//�������ͬ�εĽڵ�
		if (pXMirNode&&pXMirNode->iSeg != pNode->iSeg)
			pXMirNode = NULL;	
		if (pYMirNode&&pYMirNode->iSeg != pNode->iSeg)
			pYMirNode = NULL;
		if (pZMirNode&&pZMirNode->iSeg != pNode->iSeg)
			pZMirNode = NULL;
		//���ڵ������ϵ
		if(pXMirNode&&pXMirNode!=pNode)
		{
			relaObj.hObj=pXMirNode->handle;
			relaObj.mirInfo.axis_flag=0x01;
			pNode->AppendRelativeObj(relaObj);
			relaObj.hObj=pNode->handle;
			pXMirNode->AppendRelativeObj(relaObj);
			if(pYMirNode&&pYMirNode!=pXMirNode)
			{
				relaObj.hObj=pYMirNode->handle;
				relaObj.mirInfo.axis_flag=0x04;
				pXMirNode->AppendRelativeObj(relaObj);
				relaObj.hObj=pXMirNode->handle;
				pYMirNode->AppendRelativeObj(relaObj);
			}
			if(pZMirNode&&pZMirNode!=pXMirNode)
			{
				relaObj.hObj=pZMirNode->handle;
				relaObj.mirInfo.axis_flag=0x02;
				pXMirNode->AppendRelativeObj(relaObj);
				relaObj.hObj=pXMirNode->handle;
				pZMirNode->AppendRelativeObj(relaObj);
			}
		}
		if(pYMirNode&&pYMirNode!=pNode)
		{
			relaObj.hObj=pYMirNode->handle;
			relaObj.mirInfo.axis_flag=0x02;
			pNode->AppendRelativeObj(relaObj);
			relaObj.hObj=pNode->handle;
			pYMirNode->AppendRelativeObj(relaObj);
			if(pZMirNode&&pYMirNode!=pZMirNode)
			{
				relaObj.hObj=pZMirNode->handle;
				relaObj.mirInfo.axis_flag=0x01;
				pYMirNode->AppendRelativeObj(relaObj);
				relaObj.hObj=pYMirNode->handle;
				pZMirNode->AppendRelativeObj(relaObj);
			}
		}
		if(pZMirNode&&pZMirNode!=pNode)
		{
			relaObj.hObj=pZMirNode->handle;
			relaObj.mirInfo.axis_flag=0x04;
			pNode->AppendRelativeObj(relaObj);
			relaObj.hObj=pNode->handle;
			pZMirNode->AppendRelativeObj(relaObj);
		}
	}
	else
	{
		CLDSPart* pPart=(CLDSPart*)pObject;
		if(!pPart->IsLinePart())
			return FALSE;
		CLDSLinePart *pRod=(CLDSLinePart*)pPart;
		CLDSLinePart *pXMirRod=(CLDSLinePart*)pRod->GetMirRod(MIRMSG(1));
		CLDSLinePart *pYMirRod=(CLDSLinePart*)pRod->GetMirRod(MIRMSG(2));
		CLDSLinePart *pZMirRod=(CLDSLinePart*)pRod->GetMirRod(MIRMSG(3));
		if(pXMirRod==pRod)
			pXMirRod=NULL;
		if(pYMirRod==pRod)
			pYMirRod=NULL;
		if(pZMirRod==pRod)
			pZMirRod=NULL;
		//�������ͬ�εĸ˼�
		if (pXMirRod&&pXMirRod->iSeg != pRod->iSeg)
			pXMirRod = NULL;
		if (pYMirRod&&pYMirRod->iSeg != pRod->iSeg)
			pYMirRod = NULL;
		if (pZMirRod&&pZMirRod->iSeg != pRod->iSeg)
			pZMirRod = NULL;
		//���˼��Ĺ�����ϵ
		//���Ӷκ�ƥ���ж�����,�Է��ᵣ���Ҳ��ֶԳ� wjh-2019.7.22
		if(pXMirRod&&pXMirRod!=pRod&&pXMirRod->iSeg==pRod->iSeg)
		{
			relaObj.hObj=pXMirRod->handle;
			relaObj.mirInfo.axis_flag=0x01;
			pRod->AppendRelativeObj(relaObj);
			relaObj.hObj=pRod->handle;
			pXMirRod->AppendRelativeObj(relaObj);
			if(pYMirRod&&pXMirRod!=pYMirRod)
			{
				relaObj.hObj=pYMirRod->handle;
				relaObj.mirInfo.axis_flag=0x04;
				pXMirRod->AppendRelativeObj(relaObj);
				relaObj.hObj=pXMirRod->handle;
				pYMirRod->AppendRelativeObj(relaObj);
			}
			if(pZMirRod&&pXMirRod!=pZMirRod)
			{
				relaObj.hObj=pZMirRod->handle;
				relaObj.mirInfo.axis_flag=0x02;
				pXMirRod->AppendRelativeObj(relaObj);
				relaObj.hObj=pXMirRod->handle;
				pZMirRod->AppendRelativeObj(relaObj);
			}
		}
		if(pYMirRod&&pYMirRod!=pRod&&pYMirRod->iSeg==pRod->iSeg)
		{
			relaObj.hObj=pYMirRod->handle;
			relaObj.mirInfo.axis_flag=0x02;
			pRod->AppendRelativeObj(relaObj);
			relaObj.hObj=pRod->handle;
			pYMirRod->AppendRelativeObj(relaObj);
			if(pZMirRod&&pYMirRod!=pZMirRod)
			{
				relaObj.hObj=pZMirRod->handle;
				relaObj.mirInfo.axis_flag=0x01;
				pYMirRod->AppendRelativeObj(relaObj);
				relaObj.hObj=pYMirRod->handle;
				pZMirRod->AppendRelativeObj(relaObj);
			}
		}
		if(pZMirRod&&pZMirRod!=pRod&&pZMirRod->iSeg==pRod->iSeg)
		{
			relaObj.hObj=pZMirRod->handle;
			relaObj.mirInfo.axis_flag=0x04;
			pRod->AppendRelativeObj(relaObj);
			relaObj.hObj=pRod->handle;
			pZMirRod->AppendRelativeObj(relaObj);
		}
		//������Գƣ����Ӿ������ʱ��񡢲��ʵ�����ƥ�䣬�Է�������������ͬ wjh-2019.7.22
		MIRMSG mirmsg;
		mirmsg.axis_flag = 8;
		mirmsg.array_num = 1;
		mirmsg.rotate_angle = 90;
		RELATIVE_OBJECT *pRelaObj = NULL;
		int flagArr[4] = { 1,2,4,16 };
		CLDSLinePart* srcRodArr[4] = { pRod, pXMirRod,pYMirRod,pZMirRod };
		for (int i = 0; i < 4; i++)
		{
			if(srcRodArr[i]==NULL)
				continue;
			CLDSLinePart *pMirAngle=srcRodArr[i]->GetMirRod(mirmsg);
			if (pMirAngle&&(pMirAngle->cMaterial == srcRodArr[i]->cMaterial&&
				pMirAngle->size_wide == srcRodArr[i]->size_wide&&pMirAngle->size_thick == srcRodArr[i]->size_thick))
			{
				BOOL bInList = FALSE;
				for (pRelaObj = srcRodArr[0]->relativeObjs.GetFirst(); pRelaObj; pRelaObj = srcRodArr[0]->relativeObjs.GetNext())
				{	//�����ظ���ӹ�������
					if (pRelaObj->hObj == pMirAngle->handle)
					{
						bInList = TRUE;
						break;
					}
				}
				if (bInList)
					continue;
				pRelaObj = srcRodArr[0]->relativeObjs.Add(pMirAngle->handle);
				pRelaObj->mirInfo.rotate_angle = 90;
				pRelaObj->mirInfo.axis_flag = mirmsg.axis_flag;
				if (i>0)
					pRelaObj->mirInfo.axis_flag += flagArr[i - 1];
			}
		}
	}
	return TRUE;
}
//��ȡ���¹�������
BOOL CLDSView::FinishRetrieveRelaMirObject(CLDSDbObject* pSrcObject)
{
	CheckRelativeMirInfo(pSrcObject);
	EmendMirRodInfo(pSrcObject);
	return TRUE;
}
static MIRMSG GetMirMsg(CLDSNode *pSrcNode,CLDSNode *pRelativeNode)
{
	MIRMSG mirmsg;
	if(fabs(pSrcNode->Position(false).y+pRelativeNode->Position(false).y)<EPS&&fabs(pSrcNode->Position(false).x-pRelativeNode->Position(false).x)<EPS)
		mirmsg.axis_flag = 1;
	else if(fabs(pSrcNode->Position(false).y-pRelativeNode->Position(false).y)<EPS&&fabs(pSrcNode->Position(false).x+pRelativeNode->Position(false).x)<EPS)
		mirmsg.axis_flag = 2;
	else if(fabs(pSrcNode->Position(false).y+pRelativeNode->Position(false).y)<EPS&&fabs(pSrcNode->Position(false).x+pRelativeNode->Position(false).x)<EPS)
		mirmsg.axis_flag = 4;
	if(mirmsg.axis_flag>0)
		return mirmsg;	//��ȷ��Ϊĳһ��Գƽڵ�
	if(pSrcNode->Position(false).z!=pRelativeNode->Position(false).z)
		return mirmsg;	//����ͬһZƽ����
	double src_len2=pSrcNode->Position(false).x*pSrcNode->Position(false).x+pSrcNode->Position(false).y*pSrcNode->Position(false).y;
	double len2=pRelativeNode->Position(false).x*pRelativeNode->Position(false).x+pRelativeNode->Position(false).y*pRelativeNode->Position(false).y;
	if(fabs(len2-src_len2)>EPS)
		return mirmsg;	//���������߾��벻ͬ
	short angleX=0,angleY=0,angleZ=0;
	CLDSNode *pXMirNode=NULL,*pYMirNode=NULL,*pZMirNode=NULL;
	pXMirNode = pSrcNode->GetMirNode(MIRMSG(1));
	pYMirNode = pSrcNode->GetMirNode(MIRMSG(2));
	pZMirNode = pSrcNode->GetMirNode(MIRMSG(3));
	if(pXMirNode)
		angleX = (short)ftoi(CalRotateAngle(f3dPoint(0,0,1),f3dPoint(pXMirNode->Position(false).x,pXMirNode->Position(false).y),
		f3dPoint(pRelativeNode->Position(false).x,pRelativeNode->Position(false).y))*DEGTORAD_COEF);
	if(pYMirNode)
		angleY = (short)ftoi(CalRotateAngle(f3dPoint(0,0,1),f3dPoint(pYMirNode->Position(false).x,pYMirNode->Position(false).y),
		f3dPoint(pRelativeNode->Position(false).x,pRelativeNode->Position(false).y))*DEGTORAD_COEF);
	if(pZMirNode)
		angleZ = (short)ftoi(CalRotateAngle(f3dPoint(0,0,1),f3dPoint(pZMirNode->Position(false).x,pZMirNode->Position(false).y),
		f3dPoint(pRelativeNode->Position(false).x,pRelativeNode->Position(false).y))*DEGTORAD_COEF);
	mirmsg.rotate_angle = (short)ftoi(CalRotateAngle(f3dPoint(0,0,1),f3dPoint(pSrcNode->Position(false).x,pSrcNode->Position(false).y),
		f3dPoint(pRelativeNode->Position(false).x,pRelativeNode->Position(false).y))*DEGTORAD_COEF);
	
	if(abs(angleX)==90)
	{	//X��Գƺ�����ת�Գ�
		mirmsg.axis_flag=8|1;
		mirmsg.rotate_angle=angleX;
	}
	else if(abs(angleY)==90)
	{	//Y��Գƺ�����ת�Գ�
		mirmsg.axis_flag=8|2;
		mirmsg.rotate_angle=angleY;
	}
	else if(abs(angleZ)==90)
	{	//Z��Գƺ�����ת�Գ�
		mirmsg.axis_flag=8|4;
		mirmsg.rotate_angle=angleZ;
	}
	else
		mirmsg.axis_flag = 8;
		
	return mirmsg;
}

void CLDSView::OnAddRelativeObject() 
{
	CLDSDbObject *pSrcObj=NULL,*pRelativeObj=NULL;
	long *id_arr=NULL;
	int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(n>1)
	{	//�Զ�����������ӹ�������ʱ�����Զ���ȡ��ʽ���д���
		for(int i=0;i<n;i++)
		{
			pSrcObj=(CLDSDbObject*)console.FromHandle(id_arr[i],TRUE);
			if(pSrcObj)
				FinishRetrieveRelaMirObject(pSrcObj);
		}
		g_pSolidDraw->ReleaseSnapStatus();
		MessageBox("�Զ���ӹ����������");
		return;
	}
	//�Ե���ѡ�ж�����ӹ������󣬿����Զ���ȡ���������Ӷ�Ӧ�Ĺ�������
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	pSrcObj=(CLDSDbObject*)console.FromHandle(id_arr[0],TRUE);
	if(pSrcObj==NULL)
		return;
	CXhChar50 sClassName;
	if(pSrcObj->GetClassTypeId()==CLS_NODE)
		sClassName.Copy("�ڵ�");
	else if(pSrcObj->GetClassTypeId()==CLS_LINEANGLE)
		sClassName.Copy("�Ǹ�");
	else if(pSrcObj->GetClassTypeId()==CLS_LINEFLAT)
		sClassName.Copy("����");
	else if(pSrcObj->GetClassTypeId()==CLS_LINESLOT)
		sClassName.Copy("�۸�");
	else if(pSrcObj->GetClassTypeId()==CLS_PLATE)
		sClassName.Copy("�ְ�");
	else if(pSrcObj->GetClassTypeId()==CLS_PARAMPLATE)
		sClassName.Copy("��������");
	else
		sClassName.Copy("����");
	//
	CMirMsgDlg mirmsgDlg;
	CString cmdStr,cmdValue;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("AddRelaObjs Please select the relative nodes to be added<Enter to retrieve automatic>:");
#else
	cmdStr.Format("AddRelaObjs ��ѡ��ǰ%s�Ĺ�������<Enter���Զ����>:",(char*)sClassName);
#endif
	DWORD dwhObj=0,dwExportFlag=0;
	CSnapTypeVerify verifier(0,0);
	if(pSrcObj->GetClassTypeId()==CLS_NODE)
		verifier.SetVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
	else
	{
		verifier.AddVerifyType(OBJPROVIDER::SOLIDSPACE,pSrcObj->GetClassTypeId());
		if(pSrcObj->IsPart()&&((CLDSPart*)pSrcObj)->IsLinePart())
			verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
	}
	BOOL bAutoRetrieve=FALSE;
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
			return ;
		SELOBJ obj(dwhObj,dwExportFlag);
		if(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
		{	//�Զ���ӹ�������
			bAutoRetrieve=TRUE;
			break;
		}
		//�ֶ���ӹ����ڵ�
		dwhObj=obj.hRelaObj;
		if(obj.provider==OBJPROVIDER::SOLIDSPACE)
			dwhObj=obj.hObj;
		if(pSrcObj->GetClassTypeId()==CLS_NODE)
			pRelativeObj=console.FromNodeHandle(dwhObj);
		else 
			pRelativeObj=console.FromPartHandle(dwhObj);
		if(pRelativeObj && pRelativeObj->GetClassTypeId()==pSrcObj->GetClassTypeId())
		{
			g_pSolidDraw->SetEntSnapStatus(pRelativeObj->handle);
			if(pRelativeObj->GetClassTypeId()==CLS_NODE)
			{
				MIRMSG mirmsg = GetMirMsg((CLDSNode*)pSrcObj,(CLDSNode*)pRelativeObj);
				mirmsgDlg.mirmsg = mirmsg;
			}
			if(IDOK!=mirmsgDlg.DoModal())
			{
				g_pSolidDraw->ReleaseSnapStatus();
				pCmdLine->CancelCmdLine();
				return;	
			}
			BOOL bInList = FALSE;
			RELATIVE_OBJECT *pRelaObj;
			for(pRelaObj=pSrcObj->relativeObjs.GetFirst();pRelaObj;pRelaObj=pSrcObj->relativeObjs.GetNext())
			{	//�����ظ����ͬһ������
				if(pRelaObj->hObj==pRelativeObj->handle)
				{
					bInList = TRUE;
					break;
				}
			}
			if(!bInList)
			{
				RELATIVE_OBJECT relaObj(pRelativeObj->handle,mirmsgDlg.mirmsg);
				pSrcObj->AppendRelativeObj(relaObj);
			}
			//����׼������ӵ�(���������)���������б���
			bInList = FALSE;
			for(pRelaObj=pRelativeObj->relativeObjs.GetFirst();pRelaObj;pRelaObj=pRelativeObj->relativeObjs.GetNext())
			{	//�����ظ����ͬһ������
				if(pRelaObj->hObj==pSrcObj->handle)
				{
					bInList = TRUE;
					break;
				}
			}
			if(!bInList)
			{
				RELATIVE_OBJECT relaObj(pSrcObj->handle,mirmsgDlg.mirmsg);
				if(mirmsgDlg.mirmsg.axis_flag==8) //��ת�Գ�
					relaObj.mirInfo.axis_flag=4|8;
				else if(mirmsgDlg.mirmsg.axis_flag==12) //Z��|��ת�Գ�
					relaObj.mirInfo.axis_flag=8;
				pRelativeObj->AppendRelativeObj(relaObj);
			}
			//�����ֶ�ѡ���������
			pCmdLine->FinishCmdLine(CXhChar16("0x%X",pRelativeObj->handle));
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("AddRelaObjs Please continue to select the relative %s to be added<Finish ESC>:",(char*)sClassName);
#else
			cmdStr.Format("AddRelaObjs �����ѡ��ǰ%s�Ĺ�������<ESC������>:",(char*)sClassName);
#endif
			pCmdLine->FillCmdLine(cmdStr,"");
		}
	}
	if(bAutoRetrieve)
	{	//�Զ���ӹ�������
		FinishRetrieveRelaMirObject(pSrcObj);
		pCmdLine->FinishCmdLine("�Զ����");
	}
	g_pSolidDraw->ReleaseSnapStatus();
	pCmdLine->CancelCmdLine();
}

void CLDSView::OnUpdateAddRelativeObject(CCmdUI* pCmdUI) 
{
	/*
	long *id_arr=NULL;
	int n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(n==1)
		pCmdUI->Enable();
	else
		pCmdUI->Enable(FALSE);
	*/
}

void CLDSView::OnUpdateMirCreateTransectFace(CCmdUI* pCmdUI) 
{
	long *id_arr=NULL;
	int n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(n==1)
	{
		CLDSPart *pPart = console.FromPartHandle(id_arr[0]);
		if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE)
			pCmdUI->Enable();
		else
			pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(FALSE);
}

int CLDSView::CalDistOfSolid()
{
#ifdef __ANGLE_PART_INC_
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr,promptStr;
	CSuperSmartPtr<CLDSPart> pDatumPart,pPart,pTempPart;
	GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECT_ALL));
	scr_part_para.disp_type=DISP_SOLID;
#ifdef AFX_TARG_ENU_ENGLISH
    scr_part_para.cmdStr="DIST Please select a datum part:";
	scr_part_para.cmdErrorFeedbackStr="Select a wrong part��please select a datum part again:";
#else
	scr_part_para.cmdStr="DIST ��ѡ��һ����׼����:";
	scr_part_para.cmdErrorFeedbackStr="ѡ���˴���Ĺ�����������ѡ��һ����׼����:";
#endif
	while(1)
	{
		if(!GetPartsFromScr(scr_part_para))
			return 0; 
		if(scr_part_para.pResultObjsArr[0]==NULL)
			return 0;
		if(!scr_part_para.pResultObjsArr[0]->IsPart())
			return 0;
		pDatumPart=(CLDSPart*)scr_part_para.pResultObjsArr[0];
		BOOL bErrorPart=FALSE;
		if(pDatumPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			if(pDatumPart.ParamPlatePointer()->m_iParamType!=TYPE_ROLLEND
				&&pDatumPart.ParamPlatePointer()->m_iParamType!=TYPE_ROLLEND
				&&pDatumPart.ParamPlatePointer()->m_iParamType!=TYPE_XEND)
				bErrorPart=TRUE;
		}
		if(bErrorPart)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			scr_part_para.cmdStr="Select a wrong part��please select a datum part again:";
#else
			scr_part_para.cmdStr="ѡ���˴���Ĺ�����������ѡ��һ����׼����:";
#endif
			continue;
		}
		else
			break;
	}
#ifdef AFX_TARG_ENU_ENGLISH
    scr_part_para.cmdStr="Please select a part to be measured:";
	scr_part_para.cmdErrorFeedbackStr="Select a wrong part��please select a part to be measured again:";
#else
	scr_part_para.cmdStr="��ѡ��һ��Ҫ�����Ĺ���:";
	scr_part_para.cmdErrorFeedbackStr="ѡ���˴���Ĺ�����������ѡ��һ��Ҫ�����Ĺ���:";
#endif
	while(1)
	{
		if(!GetPartsFromScr(scr_part_para))
			return 0; 
		if(scr_part_para.pResultObjsArr[0]&&scr_part_para.pResultObjsArr[0]->IsPart())
			pPart=(CLDSPart*)scr_part_para.pResultObjsArr[0];
		if(pPart.IsNULL())
			continue;
		if(pPart->pSolidBody->IsIntWith(pDatumPart->pSolidBody))
		{	
			g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox("Fail to calculate because of the collision of two solid model!");
			pCmdLine->FillCmdLine("Command:","");
#else
			MessageBox("������ʵ�巢����ײ�޷�����!");
			pCmdLine->FillCmdLine("����:","");
#endif
			return 0;
		}	
		break;
	}
 	CString sDist;
	f3dPoint perp;
	f3dLine dist_line;
	dist_line.pen.width=2;
	dist_line.pen.crColor=RGB(255,0,0);
	double dist_min=1000000,dist=0;
	
	if(pDatumPart->GetClassTypeId()==CLS_BOLT&&pPart->GetClassTypeId()==CLS_BOLT)
	{	//����һ������Ϊ��˨ ������һ��������Ϊ�ֹ�
		dist_line.startPt=pDatumPart.BoltPointer()->ucs.origin;
		dist_line.endPt=pPart.BoltPointer()->ucs.origin;
		g_pSolidDraw->NewLine(dist_line);
		dist_min=DISTANCE(pDatumPart.BoltPointer()->ucs.origin,pPart.BoltPointer()->ucs.origin);
#ifdef AFX_TARG_ENU_ENGLISH
		sDist.Format("The shortest distance between the two bolts=%.1fmm",dist_min);
#else
		sDist.Format("����˨�����̾���=%.1fmm",dist_min);
#endif
		MessageBox(sDist);
		g_pSolidDraw->ReleaseSnapStatus();
		Command("DIST");
	}
	else if(pDatumPart->GetClassTypeId()==CLS_LINETUBE||pPart->GetClassTypeId()==CLS_LINETUBE)
	{	//����һ������Ϊ�ֹ�
		if(pPart->GetClassTypeId()==CLS_LINETUBE
			&&pDatumPart->GetClassTypeId()!=CLS_LINETUBE)
		{	//��֤��׼����Ϊ�ֹ�
			pTempPart=pDatumPart;
			pDatumPart=pPart;
			pPart=pTempPart;
		}
		f3dPoint start=pDatumPart.LineTubePointer()->Start();
		f3dPoint end=pDatumPart.LineTubePointer()->End();
		if(pPart->GetClassTypeId()==CLS_BOLT)
		{
			SnapPerp(&perp,start,end,pPart->ucs.origin,&dist_min);
			dist_line.startPt=pPart->ucs.origin;
			dist_line.endPt=perp;
		}
		else 
		{
			int vertex_n=pPart->pSolidBody->KeyPointNum();
			//for(f3dPoint *pPt=pPart->pBody->vertex.GetFirst();pPt;pPt=pPart->pBody->vertex.GetNext())
			for(int vi=0;vi<vertex_n;vi++)
			{
				f3dPoint vertex=pPart->pSolidBody->GetKeyPointAt(vi);
				SnapPerp(&perp,start,end,vertex,&dist);
				if(dist<dist_min)
				{
					dist_min=dist;
					dist_line.startPt=vertex;
					dist_line.endPt=perp;
				}
			}
		}
		dist_min-=(pDatumPart.LineTubePointer()->GetDiameter()*0.5);
		//���Ƶ�ǰ��ʾ������߶�
		f3dPoint line_vec=dist_line.startPt-dist_line.endPt;
		normalize(line_vec);
		dist_line.endPt+=line_vec*(pDatumPart.LineTubePointer()->GetDiameter()*0.5);
		g_pSolidDraw->NewLine(dist_line);
#ifdef AFX_TARG_ENU_ENGLISH
		sDist.Format("The shortest distance between the two parts=%.1fmm",dist_min);
#else
		sDist.Format("�����������̾���=%.1fmm",dist_min);
#endif
		MessageBox(sDist);
		g_pSolidDraw->ReleaseSnapStatus();
		Command("DIST");	
	}
	else if(pDatumPart->GetClassTypeId()==CLS_LINEANGLE||pPart->GetClassTypeId()==CLS_LINEANGLE)
	{	//����һ������Ϊ�Ǹ� ������һ��������Ϊ�ֹܺ���˨
		if(pPart->GetClassTypeId()==CLS_LINEANGLE
			&&pDatumPart->GetClassTypeId()!=CLS_LINEANGLE)
		{	//��֤��׼����Ϊ�Ǹ�
			pTempPart=pDatumPart;
			pDatumPart=pPart;
			pPart=pTempPart;
		}
		double wing_wide=pDatumPart.LineAnglePointer()->GetWidth();
		f3dPoint start=pDatumPart.LineAnglePointer()->Start();
		f3dPoint end=pDatumPart.LineAnglePointer()->End();
		f3dPoint len_vec;
		Sub_Pnt(len_vec,end,start);
		normalize(len_vec);
		Sub_Pnt(start,start,len_vec*pDatumPart.LineAnglePointer()->startOdd());
		Add_Pnt(end,end,len_vec*pDatumPart.LineAnglePointer()->endOdd());
		int vertex_n=pPart->pSolidBody->KeyPointNum();
		//for(f3dPoint *pPt=pPart->pBody->vertex.GetFirst();pPt;pPt=pPart->pBody->vertex.GetNext())
		for(int vi=0;vi<vertex_n;vi++)
		{
			f3dPoint vertex=pPart->pSolidBody->GetKeyPointAt(vi);
			f3dLine datum_line;
			for(int i=0;i<7;i++)
			{
				if(i==0)
				{
					datum_line.startPt=start;
					datum_line.endPt=end;
				}
				else if(i==1||i==2)
				{
					f3dPoint wing_vec=pDatumPart.LineAnglePointer()->GetWingVecX();
					if(i==2)
						wing_vec=pDatumPart.LineAnglePointer()->GetWingVecY();
					datum_line.startPt=start+wing_vec*wing_wide;
					datum_line.endPt=end+wing_vec*wing_wide;
				}
				else if(i==3||i==4)
				{
					f3dPoint wing_vec=pDatumPart.LineAnglePointer()->GetWingVecX();	
					if(i==4)
						wing_vec=pDatumPart.LineAnglePointer()->GetWingVecY();
					datum_line.startPt=start;
					datum_line.endPt=start+wing_vec*wing_wide;
				}
				else if(i==5||i==6)
				{
					f3dPoint wing_vec=pDatumPart.LineAnglePointer()->GetWingVecX();	
					if(i==6)
						wing_vec=pDatumPart.LineAnglePointer()->GetWingVecY();
					datum_line.startPt=end;
					datum_line.endPt=end+wing_vec*wing_wide;
				}
				if(pPart->GetClassTypeId()==CLS_BOLT)
					SnapPerp(&perp,datum_line,pPart->ucs.origin,&dist);
				else
					SnapPerp(&perp,datum_line,vertex,&dist);
				int nRetCode=datum_line.PtInLine(perp);
				if(dist<dist_min&&nRetCode>0)
				{
					dist_min=dist;
					dist_line.startPt=vertex;
					dist_line.endPt=perp;
				}
			}
			if(pPart->GetClassTypeId()==CLS_BOLT)
				break;	//��˨
		}
		//�ڲ��������Ǹ�֮�����̾���ʱ�û�����ѡ���˻�׼���� ������Ҫ�������¼���
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			double wing_wide=pPart.LineAnglePointer()->GetWidth();
			f3dPoint start=pPart.LineAnglePointer()->Start();
			f3dPoint end=pPart.LineAnglePointer()->End();
			Sub_Pnt(len_vec,end,start);
			normalize(len_vec);
			Sub_Pnt(start,start,len_vec*pPart.LineAnglePointer()->startOdd());
			Add_Pnt(end,end,len_vec*pPart.LineAnglePointer()->endOdd());
			int vertex_n=pDatumPart->pSolidBody->KeyPointNum();
			//for(f3dPoint *pPt=pDatumPart->pBody->vertex.GetFirst();pPt;pPt=pDatumPart->pBody->vertex.GetNext())
			for(int vi=0;vi<vertex_n;vi++)
			{
				f3dPoint vertex=pDatumPart->pSolidBody->GetKeyPointAt(vi);
				f3dLine datum_line;
				for(int i=0;i<7;i++)
				{
					if(i==0)
					{	//����
						datum_line.startPt=start;
						datum_line.endPt=end;
					}
					else if(i==1||i==2)
					{	//
						f3dPoint wing_vec=pPart.LineAnglePointer()->GetWingVecX();	
						if(i==2)
							wing_vec=pPart.LineAnglePointer()->GetWingVecY();
						datum_line.startPt=start+wing_vec*wing_wide;
						datum_line.endPt=end+wing_vec*wing_wide;
					}
					else if(i==3||i==4)
					{
						f3dPoint wing_vec=pPart.LineAnglePointer()->GetWingVecX();	
						if(i==4)
							wing_vec=pPart.LineAnglePointer()->GetWingVecY();
						datum_line.startPt=start;
						datum_line.endPt=start+wing_vec*wing_wide;
					}
					else if(i==5||i==6)
					{
						f3dPoint wing_vec=pPart.LineAnglePointer()->GetWingVecX();	
						if(i==6)
							wing_vec=pPart.LineAnglePointer()->GetWingVecY();
						datum_line.startPt=end;
						datum_line.endPt=end+wing_vec*wing_wide;
					}
					SnapPerp(&perp,datum_line,vertex,&dist);
					int nRetCode=datum_line.PtInLine(perp);
					if(dist<dist_min&&nRetCode>0)
					{
						dist_min=dist;
						dist_line.startPt=vertex;
						dist_line.endPt=perp;
					}
				}
			}
		}
		g_pSolidDraw->NewLine(dist_line);
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
			sDist.Format("The shortest distance between the two angles=%.1fmm",dist_min);
		else
			sDist.Format("The shortest distance between the two parts=%.1fmm",dist_min);
#else
			sDist.Format("���Ǹּ����̾���=%.1fmm",dist_min);
		else
			sDist.Format("�����������̾���=%.1fmm",dist_min);
#endif
		MessageBox(sDist);
		g_pSolidDraw->ReleaseSnapStatus();
		Command("DIST");
	}
	else
		return 0;
#endif
	return 0;
}

//ʶ���ж�pRod1��pRod2�����Ӷˣ��й�ͬ�˽ڵ�ʱ����ֵΪ0,���򷵻ؾ�������Ķ˵����
double RodJunctionDiscern(CLDSLinePart* pRod1,CLDSLinePart* pRod2,char* cJunction,bool bOnlySameNode=false);
//�������˼���ļн�
int CLDSView::CalAngle()
{
#ifdef __ANGLE_PART_INC_
	//ѡȡ���˼�
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr;
	DWORD dwhObj=0,dwExportFlag=0;
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,SELECT_LINEPART|GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE));
	verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prPoint);
	CSuperSmartPtr<CLDSPart> pFirstPart,pSecondPart;
	for(int i=0;i<2;i++)
	{
		if(i==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("ANGLE Please select the first part(rod or plate):","");
#else
			pCmdLine->FillCmdLine("ANGLE ��ѡ���һ������(�˼���ְ�):","");
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("ANGLE Please select the second part(rod or plate):","");
#else
			pCmdLine->FillCmdLine("ANGLE ��ѡ��ڶ�������(�˼���ְ�):","");
#endif
		}
		CLDSPart* pSelPart=NULL;
		while(1)
		{
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			dwhObj=obj.hRelaObj;
			if(i==0)
				pFirstPart= pSelPart=console.FromPartHandle(dwhObj);
			else
				pSecondPart= pSelPart=console.FromPartHandle(dwhObj);
			if(pSelPart)
				break;
		}
		g_pSolidDraw->SetEntSnapStatus(pSelPart->handle);
		if(pSelPart->IsLinePart())
			pCmdLine->FinishCmdLine(CXhChar16("�˼�0x%X",pSelPart->handle));
		else if(pSelPart->IsPlate())
			pCmdLine->FinishCmdLine(CXhChar16("�ְ�0x%X",pSelPart->handle));
		else
			pCmdLine->FinishCmdLine(CXhChar16("����0x%X",pSelPart->handle));
	}
	if(pFirstPart.IsNULL() || pSecondPart.IsNULL())
		return 0;
	g_pSolidDraw->SetEntSnapStatus(pFirstPart->handle);
	g_pSolidDraw->SetEntSnapStatus(pSecondPart->handle);
	//����ѡȡ�˼��Ĺ�ϵ����ʼ���˼��ķ���
	f3dPoint fir_vec,sec_vec;
	if(pFirstPart->IsLinePart()||pSecondPart->IsLinePart())
	{
		char junction=0;
		const int JUNCTION_CLEARANCE=50;
		if(RodJunctionDiscern(pFirstPart.pRod,pSecondPart.pRod,&junction)<JUNCTION_CLEARANCE)
		{
			if(junction&0x02)
				fir_vec=pFirstPart.pRod->Start()-pFirstPart.pRod->End();
			else
				fir_vec=pFirstPart.pRod->End()-pFirstPart.pRod->Start();
			if(junction&0x08)
				sec_vec=pSecondPart.pRod->Start()-pSecondPart.pRod->End();
			else
				sec_vec=pSecondPart.pRod->End()-pSecondPart.pRod->Start();
		}
		else
		{
			fir_vec=pFirstPart.pRod->End()-pFirstPart.pRod->Start();
			sec_vec=pSecondPart.pRod->End()-pSecondPart.pRod->Start();
		}
	}
	else if(pFirstPart->IsPlate()&&pSecondPart->IsPlate())
	{
		fir_vec=pFirstPart->ucs.axis_z;
		sec_vec=pSecondPart->ucs.axis_z;
		if( pFirstPart->GetClassTypeId()==CLS_PLATE&&pSecondPart->GetClassTypeId()==CLS_PLATE&&
			pFirstPart.pPlate->designInfo.m_hBasePart==pSecondPart.pPlate->designInfo.m_hBasePart)
		{
			CSuperSmartPtr<CLDSPart> pDatumPart=Ta.Parts.FromHandle(pFirstPart.pPlate->designInfo.m_hBasePart);
			if(pDatumPart->IsTube())
			{
				fir_vec=pDatumPart.pTube->GetPlateVertSpreadVec(pFirstPart.pPlate);
				sec_vec=pDatumPart.pTube->GetPlateVertSpreadVec(pSecondPart.pPlate);
			}
		}
	}
	//����нǲ�������
	normalize(fir_vec);
	normalize(sec_vec);
	double rad_angle=cal_angle_of_2vec(fir_vec,sec_vec);
	rad_angle*=DEGTORAD_COEF;
	CString sValue;
	#ifdef AFX_TARG_ENU_ENGLISH
		sValue.Format("The angle between the two linepart=%.1f��",rad_angle);
	#else
		sValue.Format("��������ļн�=%.1f��",rad_angle);
	#endif
	MessageBox(sValue);
	g_pSolidDraw->ReleaseSnapStatus();
#endif
	return 0;
}

void CLDSView::OnGroupEditPartLayer() 
{
	long *id_arr=NULL;
	long i,n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(n<=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("There is not any modified object!");
#else
		AfxMessageBox("û���κ��޸Ķ���!");
#endif
		return;
	}
	CGroupEditLayerDlg dlg;
	CLDSDbObject* pObj;
#ifdef __PART_DESIGN_INC_
	if(id_arr[0]<0)
	{	//��ֳ����ĵ�Ԫ
		long id=id_arr[0]^0x80000000;	//���ʵ�ʵ�Ԫ
		if(id-1>=0&&id-1<Ta.ElemPtrArr.GetSize())
		{
			CElemInfo* pElemInfo=Ta.ElemPtrArr[id-1];
			pObj=console.FromRodHandle(pElemInfo->m_hParentRod);
		}
	}
	else
#endif
	pObj = (CLDSDbObject*)console.FromHandle(id_arr[0],TRUE);
	if(pObj==NULL)
		return;
	IModel* pTower =pObj->BelongModel();
	strcpy(dlg.layer,pObj->layer());
	if(dlg.DoModal()!=IDOK)
		return;
	CHashList<long> hashObjHs;
	for(i=0;i<n;i++)
		hashObjHs.Append(id_arr[i],id_arr[i]);
	for(i=0;i<n;i++)
	{
#ifdef __PART_DESIGN_INC_
		if(id_arr[i]<0)
		{	//��ֳ����ĵ�Ԫ
			long id=id_arr[i]^0x80000000;	//���ʵ�ʵ�Ԫ
			if(id-1>=0&&id-1<Ta.ElemPtrArr.GetSize())
			{
				CElemInfo* pElemInfo=Ta.ElemPtrArr[id-1];
				pObj=console.FromRodHandle(pElemInfo->m_hParentRod);
			}
		}
		else
#endif
			pObj = (CLDSDbObject*)console.FromHandle(id_arr[i],TRUE);
		if(pObj==NULL)
			continue;
		if(pObj->GetClassTypeId()==CLS_NODE)
		{
			if(dlg.m_bFirstChar)
				pObj->layer(0)=dlg.layer[0];
			if(dlg.m_bThirdChar)
				pObj->layer(2)=dlg.layer[2];
		}
		else //if(pObj->part_type==TA_JG)
		{
			if(dlg.m_bFirstChar)
				pObj->layer(0)=dlg.layer[0];
			if(dlg.m_bSecChar)
				pObj->layer(1)=dlg.layer[1];
			if(dlg.m_bThirdChar)
				pObj->layer(2)=dlg.layer[2];
			//((CObCommonJg*)pObj)->NotifyAll(REASON_MODIFIED);
		}
		for(RELATIVE_OBJECT *pRelaObj=pObj->relativeObjs.GetFirst();pRelaObj;pRelaObj=pObj->relativeObjs.GetNext())
		{	//ֻͬ��ͼ�����ĵ�ǰָ���ַ�
			if(hashObjHs.GetValue(pRelaObj->hObj))
				continue;	//�ù�������Ҳ�ڵ�ǰ�ֶ�ָ�����󼯺��С�wjh-2015.3.28
			CLDSDbObject* pRelaDbObj=(CLDSDbObject*)pObj->BelongModel()->FromHandle(pRelaObj->hObj,TRUE);
			if(pRelaDbObj==NULL)
				continue;
			char layerStr[4]={0};
			strncpy(layerStr,pRelaDbObj->layer(),3);
			pObj->SyncMirPropTo(pRelaDbObj,pRelaObj->mirInfo,"layer");
			if(dlg.m_bFirstChar)
				layerStr[0]=pRelaDbObj->Layer(0);
			if(dlg.m_bSecChar)
				layerStr[1]=pRelaDbObj->Layer(1);
			if(dlg.m_bThirdChar)
				layerStr[2]=pRelaDbObj->Layer(2);
			pRelaDbObj->SetLayer(layerStr);
		}
	}
}

#ifdef __ANGLE_PART_INC_
CLDSPlate* CLDSView::ConvertParamPlateToPlate(CLDSParamPlate *pParamPlate)
{
	CLDSPlate* pPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
	if(!pParamPlate->ConvertToPlate(pPlate))
	{	//�����쳣ʱӦ������������
		console.DeletePart(pPlate->handle);
		return NULL;
	}
	long hOldParamHandle=pParamPlate->handle;
	Ta.ReplaceReferenceHandle(CHandleReplacer(pParamPlate->handle,pPlate->handle));
	pParamPlate->handle=hOldParamHandle;	//Ta.ReplaceReferenceHandle���滻��ԭ���ľ��
	console.DispPartSet.DeleteNode(pParamPlate->handle);
	g_pSolidDraw->DelEnt(pParamPlate->handle);
	console.DeletePart(pParamPlate->handle);
	console.DispPartSet.append(pPlate);
	pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return pPlate;
}
#endif

void CLDSView::OnConvertToPlate() 
{	//����������ת��Ϊ��ͨ�ְ�
#ifdef __ANGLE_PART_INC_
	m_nPrevCommandID=ID_CONVERT_TO_PLATE;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat to converte to ordinary plate";
#else
	m_sPrevCommandName="�ظ�ת��Ϊ��ͨ�ְ�";
#endif
	long *id_arr=NULL;
	long n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	int nHits=0;
	CLogErrorLife life;
	CUndoOperObject undo(&Ta,true);
	try{
		Ta.BeginUndoListen();
		for(int i=0;i<n;i++)
		{
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)console.FromPartHandle(id_arr[i],CLS_PARAMPLATE);
			if(pParamPlate==NULL)
				continue;
			if(pParamPlate->m_bStdPart)
				continue;	//��׼��������ת��Ϊ��ͨ�ְ�
			if(pParamPlate->GetClassTypeId()!=CLS_PARAMPLATE)
				continue;
			CLDSPlate *pPlate=ConvertParamPlateToPlate(pParamPlate);
			if(pPlate!=NULL)
				nHits++;
		}
		Ta.EndUndoListen();
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	g_pSolidDraw->Draw();
	if(nHits>0)
	{
		g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox(CXhChar50("%d parametric plates have been successfully converted to ordinary plate!",nHits));
#else
		MessageBox(CXhChar50("�ѳɹ�ת��%d���������Ϊ��ͨ�ְ�!",nHits));
#endif
	}
#endif
}

void CLDSView::OnMoveDbObjPosition() 
{
	m_nPrevCommandID=ID_MOVE_DBOBJECT;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat to modify the part position";
#else
	m_sPrevCommandName="�ظ��ƶ����ݶ���λ��";
#endif
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr,cmdValue;
	char cOffsetCoordRule='G';

	long *id_arr=NULL;
	COffsetPartDlg dlg;
	CLDSDbObject* pDbObj;
	CLDSPart *pPart=NULL;
	int i,selObjClsType=0,nSelObjs=g_pSolidSnap->GetLastSelectEnts(id_arr);
	DBOBJSET objset;
	for(i=0;i<nSelObjs;i++)
	{
		pDbObj=(CLDSDbObject*)console.FromHandle(id_arr[i],TRUE);
		if(pDbObj==NULL)
			continue;
		objset.append(pDbObj);
		if(selObjClsType==0)
			selObjClsType=pDbObj->GetClassTypeId();
		else if(selObjClsType==pDbObj->GetClassTypeId())
			continue;
		else if(selObjClsType==CLS_DBOBJECT)
			break;
		else if(selObjClsType>=CLS_PART&&pDbObj->IsNode()||selObjClsType==CLS_NODE&&pDbObj->IsPart())
			selObjClsType=CLS_DBOBJECT;
		else if(selObjClsType!=pDbObj->GetClassTypeId()&&pDbObj->IsPart())
			selObjClsType=CLS_PART;
	}
	if(selObjClsType==0)
		return;

	if(selObjClsType==CLS_BOLT)
	{	//���������ƶ���˨����
		OnMoveBoltSet();
		return;
	}
	else if(selObjClsType>=CLS_PART)
	{
		if(nSelObjs==1)
		{	//���þ��޸Ĺ���λ�ô���
			ModifyPartPos((CLDSPart*)objset.GetFirst());
			return;
		}
		/*
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("Is welding Angle and base-plate<0.NO|1.YES>[0]:");
#else 
		cmdStr.Format("��ѡ�����ݹ����ľֲ�����(L), ����ȫ��ģ������(G)�����ƶ�[G]:");
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		while(1)
		{
			if(!pCmdLine->GetStrFromCmdLine(cmdValue,CCmdLineDlg::KEYWORD_AS_RETURN,"L|G"))
			{
				pCmdLine->CancelCmdLine();
				return;
			}
			if(cmdValue.GetLength()>0&&(toupper(cmdValue[0])=='L'||toupper(cmdValue[0])=='G'))
				cOffsetCoordRule=toupper(cmdValue[0]);
			break;
		}
		*/
	}
	else
		cOffsetCoordRule='G';
	//if(selObjClsType==CLS_NODE)
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Input the offset value(��x,��y,��z):");
#else 
	cmdStr.Format("��������ƶ��ĸ���ƫ����(��x,��y,��z):");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	CXhChar16 offsetStrX="0",offsetStrY="0",offsetStrZ="0";
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdValue))
		{
			pCmdLine->CancelCmdLine();
			return;
		}
		if(cmdValue.GetLength()==0)
			continue;
		CXhChar200 offsetStr=cmdValue;
		char* key=strtok(offsetStr,", ");
		if(key!=NULL)
			offsetStrX.Copy(key);
		else
			break;
		key=strtok(NULL,", ");
		if(key!=NULL)
			offsetStrY.Copy(key);
		else
			break;
		key=strtok(NULL,", ");
		if(key!=NULL)
			offsetStrZ.Copy(key);
		break;
	}
	bool movenode=false;
	GEPOINT offset(atof(offsetStrX),atof(offsetStrY),atof(offsetStrZ));
	for(pDbObj=objset.GetFirst();pDbObj;pDbObj=objset.GetNext())
	{
		movenode|=pDbObj->IsNode();
		CLDSPart* pPart=pDbObj->IsPart()?(CLDSPart*)pDbObj:NULL;
		if(cOffsetCoordRule!='L')	//=='G'
			pDbObj->Offset(offset);
		//else if(pDbObj->GetClassTypeId()==cOffsetCoordRule=='L')
		//{
		//	if(pPart&&pPart->IsAngle())
		//	{
		//		CLDSLineAngle* pAngle=(CLDSLineAngle*)pAngle->
		//	}
		//}
		if(pPart)
		{
			pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
		}
	}
	if(movenode)
		g_pSolidDraw->BuildDisplayList(this);
	else
		g_pSolidDraw->Draw();
}
void CLDSView::ModifyPartPos(CLDSPart* pPart)
{
#ifdef __ANGLE_PART_INC_
	long *id_arr=NULL;
	COffsetPartDlg dlg;
	if(pPart==NULL)
		return;
	if(pPart->IsLinePart())
		dlg.part_type = 1;	//�˼�
	else
		dlg.part_type = 0;	//��������
	UCS_STRU object_ucs,ucs,part_ucs;
	ucs = pPart->ucs;
	g_pSolidSet->GetObjectUcs(object_ucs);
	coord_trans(ucs.origin,object_ucs,TRUE);
	vector_trans(ucs.axis_x,object_ucs,TRUE);
	vector_trans(ucs.axis_y,object_ucs,TRUE);
	vector_trans(ucs.axis_z,object_ucs,TRUE);
	g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,ucs);
	//g_pSolidSet->SetDispWorkUcs();
	g_pSolidDraw->Draw();
	if(dlg.DoModal()==IDOK)
	{
		Ta.BeginUndoListen();
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{	//�Ǹ�
			CLDSLineAngle* pJg=(CLDSLineAngle*)pPart;
			if(dlg.m_bAbsoluteUcs)
			{	//���������£������Ǹ�λ��
				f3dPoint start,end;
				start = pJg->Start();
				end = pJg->End();
				start+=f3dPoint(1,0,0)*dlg.m_fBaseOffsetX+f3dPoint(0,1,0)*dlg.m_fBaseOffsetY+f3dPoint(0,0,1)*dlg.m_fBaseOffsetZ;
				end  +=f3dPoint(1,0,0)*dlg.m_fEndOffsetX+f3dPoint(0,1,0)*dlg.m_fEndOffsetY+f3dPoint(0,0,1)*dlg.m_fEndOffsetZ;
				pJg->SetStart(start);
				pJg->SetEnd(end);
			}
			if(dlg.m_bRelativeUcs)
			{	//�������ϵ�£����������ƫ��ֵ���ϵ��Ǹֵ�֫����ƫ�ƶ�λ������
				f3dPoint x_wing_norm,y_wing_norm;
				x_wing_norm=pJg->get_norm_x_wing();
				y_wing_norm=pJg->get_norm_y_wing();
				if(fabs(x_wing_norm*y_wing_norm)>EPS2)
				{
					logerr.Log("�ݲ�֧�ַ�ֱ�ǽǸֵ���λ��!");
					return;
				}
				GEPOINT start=pJg->Start()+ucs.axis_x*dlg.m_fBaseOffsetX+ucs.axis_y*dlg.m_fBaseOffsetY+ucs.axis_z*dlg.m_fBaseOffsetZ;
				GEPOINT end=pJg->End()+ucs.axis_x*dlg.m_fEndOffsetX+ucs.axis_y*dlg.m_fEndOffsetY+ucs.axis_z*dlg.m_fEndOffsetZ;
				pJg->SetStart(start);
				pJg->SetEnd(end);
				for(int i=0;i<2;i++)
				{
					BYTE gStyleX=0,gStyleY=0;
					double fDistX=0,fDistY=0; 
					if(i==0)
					{
						gStyleX=pJg->desStartPos.wing_x_offset.gStyle;
						fDistX =pJg->desStartPos.wing_x_offset.offsetDist;
						gStyleY=pJg->desStartPos.wing_y_offset.gStyle;
						fDistY =pJg->desStartPos.wing_y_offset.offsetDist;
					}
					else
					{
						gStyleX=pJg->desEndPos.wing_x_offset.gStyle;
						fDistX =pJg->desEndPos.wing_x_offset.offsetDist;
						gStyleY=pJg->desEndPos.wing_y_offset.gStyle;
						fDistY =pJg->desEndPos.wing_y_offset.offsetDist;
					}
					if(gStyleX==0)
						fDistX=pJg->xWingYZhunJu.g;
					else if(gStyleX==1)
						fDistX=pJg->xWingYZhunJu.g1;
					else if(gStyleX==2)
						fDistX=pJg->xWingYZhunJu.g2;
					else if(gStyleX==3)
						fDistX=pJg->xWingYZhunJu.g3;	
					if(gStyleY==0)
						fDistY=pJg->xWingXZhunJu.g;
					else if(gStyleY==1)
						fDistY=pJg->xWingXZhunJu.g1;
					else if(gStyleY==2)
						fDistY=pJg->xWingXZhunJu.g2;
					else if(gStyleY==3)
						fDistY=pJg->xWingXZhunJu.g3;
					//����ʼ�ն˵�֫����ƫ����
					if(i==0)
					{	//X֫����(��ͬ�ڽǸ�Y�ᷴ����)ƫ��
						pJg->desStartPos.wing_x_offset.gStyle=4;
						pJg->desStartPos.wing_x_offset.offsetDist=fDistX-dlg.m_fBaseOffsetY;
						//Y֫����(��ͬ�ڽǸ�X�ᷴ����)ƫ��
						pJg->desStartPos.wing_y_offset.gStyle=4;
						pJg->desStartPos.wing_y_offset.offsetDist=fDistY-dlg.m_fBaseOffsetX;
					}
					else
					{	//X֫����(��ͬ�ڽǸ�Y�ᷴ����)ƫ��
						pJg->desEndPos.wing_x_offset.gStyle=4;
						pJg->desEndPos.wing_x_offset.offsetDist=fDistX-dlg.m_fEndOffsetY;//m_fBaseOffsetY;
						//Y֫����(��ͬ�ڽǸ�X�ᷴ����)ƫ��
						pJg->desEndPos.wing_y_offset.gStyle=4;
						pJg->desEndPos.wing_y_offset.offsetDist=fDistY-dlg.m_fEndOffsetX;//m_fBaseOffsetX;
					}
				}
				pJg->CalPosition(true);	//����Ǹ�λ��
			}
		}
		else if(pPart->IsLinePart())
		{	//�˼�
			CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
			if(pLinePart->GetClassTypeId()==CLS_LINEANGLE||pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
				((CLDSLineAngle*)pLinePart)->getUCS(ucs);
			else if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
				ucs=((CLDSLineTube*)pLinePart)->BuildUCS();
			else if(pLinePart->GetClassTypeId()==CLS_LINEFLAT)
				ucs=((CLDSLineFlat*)pLinePart)->BuildUCS();
			else if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
				ucs=((CLDSLineSlot*)pLinePart)->BuildUCS();
			f3dPoint start,end;
			start = pLinePart->Start();
			end = pLinePart->End();
			if(dlg.m_bRelativeUcs)
			{
				start+=ucs.axis_x*dlg.m_fBaseOffsetX+ucs.axis_y*dlg.m_fBaseOffsetY+ucs.axis_z*dlg.m_fBaseOffsetZ;
				end  +=ucs.axis_x*dlg.m_fEndOffsetX+ucs.axis_y*dlg.m_fEndOffsetY+ucs.axis_z*dlg.m_fEndOffsetZ;
			}
			else if(dlg.m_bAbsoluteUcs)
			{
				start+=f3dPoint(1,0,0)*dlg.m_fBaseOffsetX+f3dPoint(0,1,0)*dlg.m_fBaseOffsetY+f3dPoint(0,0,1)*dlg.m_fBaseOffsetZ;
				end  +=f3dPoint(1,0,0)*dlg.m_fEndOffsetX+f3dPoint(0,1,0)*dlg.m_fEndOffsetY+f3dPoint(0,0,1)*dlg.m_fEndOffsetZ;
			}
			pLinePart->SetStart(start);
			pLinePart->SetEnd(end);
		}
		else 
		{	//��������
			ucs =pPart->ucs;
			if(dlg.m_bRelativeUcs)
				pPart->ucs.origin+=ucs.axis_x*dlg.m_fBaseOffsetX+ucs.axis_y*dlg.m_fBaseOffsetY+ucs.axis_z*dlg.m_fBaseOffsetZ;
			else if(dlg.m_bAbsoluteUcs)
				pPart->ucs.origin+=f3dPoint(1,0,0)*dlg.m_fBaseOffsetX+f3dPoint(0,1,0)*dlg.m_fBaseOffsetY+f3dPoint(0,0,1)*dlg.m_fBaseOffsetZ;
		}
		//g_pSolidSet->SetDispWorkUcs(FALSE);
		g_pSolidDraw->BatchClearCS(2);
		pPart->SetModified();
		pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
		g_pSolidDraw->Draw();
		Ta.EndUndoListen();
	}	
#endif
}

//�������Ӱ庸�쳤��
void CLDSView::OnAdjustPlateWeldLen() 
{
#ifdef __ANGLE_PART_INC_
	m_nPrevCommandID=ID_ADJUST_PLATE_WELD_LEN;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat to adjust the weld length of the connecting plate";
#else
	m_sPrevCommandName="�ظ��������Ӱ庸�쳤��";
#endif
	CString cmdStr;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSPlate *pDatumPlate=NULL;
	PARTSET partset;
	partset.Empty();
	long *id_arr,i=0,nCount=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(nCount>1)
	{
		//�ҵ�һ��Ǹֹܼа�İ���Ϊ��׼�ְ�
		for(i=0;i<nCount;i++)
		{
			CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(id_arr[i],CLS_PLATE);
			if(pPlate==NULL)
				break;
			if(pPlate&&pPlate->jdb_style!=3)
			{
				pDatumPlate=pPlate;
				partset.append(pPlate);
				break;
			}
		}
		for(i=0;i<nCount;i++)
		{	
			CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(id_arr[i],CLS_PLATE);
			if(pPlate==NULL||pDatumPlate==NULL)
				break;
			if(pPlate==pDatumPlate)
				continue;	//��׼�ֹ������
			else if(pDatumPlate&&pPlate->designInfo.m_hBaseNode==pDatumPlate->designInfo.m_hBaseNode &&
				pPlate->designInfo.m_hBasePart==pDatumPlate->designInfo.m_hBasePart)
				partset.append(pPlate);//��֤��ѡ���Ӱ�Ļ�׼�ڵ�ͻ�׼�˼�����ͬ
			else if(pDatumPlate&&pPlate->jdb_style==3)
			{	//��������ֹܼа�����
				CLDSLinePart *pDatumLinePart1=NULL, *pDatumLinePart2=NULL;
				pDatumLinePart1=(CLDSLinePart*)console.FromPartHandle(pPlate->designInfo.partList.GetFirst()->hPart,CLS_LINEPART);
				pDatumLinePart2=(CLDSLinePart*)console.FromPartHandle(pPlate->designInfo.partList.GetNext()->hPart,CLS_LINEPART);
				if(pDatumLinePart1==NULL||pDatumLinePart2==NULL)
					break;
				long hBasePart=pDatumPlate->designInfo.m_hBasePart;
				long hBaseNode=pDatumPlate->designInfo.m_hBaseNode;
				if((pDatumLinePart1->handle==hBasePart||pDatumLinePart2->handle==hBasePart)&&
					(pDatumLinePart1->pStart->handle==hBaseNode||pDatumLinePart1->pEnd->handle==hBaseNode||
					pDatumLinePart2->pStart->handle==hBaseNode||pDatumLinePart2->pEnd->handle==hBaseNode))
					partset.append(pPlate);
				else
					break;
			}
			else
				break;
		}
	}
	if(pDatumPlate==NULL||(pDatumPlate&&partset.GetNodeNum()<2))
	{
		pDatumPlate=NULL;
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select the plate to be adjusted the weld length:","");
#else
		pCmdLine->FillCmdLine("��ѡ����Ҫ�������쳤�ȵĸְ�:","");
#endif
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_PLATE));
		Invalidate(FALSE);
		OpenWndSel();	//����ѡ��ְ�
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
		{	
			pCmdLine->CancelCmdLine();	
			return;
		}
		int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
		for(i=0;i<n;i++)
		{	//��һ��Ǹֹܼа�ĸְ���Ϊ��׼�ְ�
			CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(id_arr[i],CLS_PLATE);
			if(pPlate==NULL)
				break;
			if(pPlate&&pPlate->jdb_style!=3)
			{
				pDatumPlate=pPlate;
				partset.append(pPlate);
				break;
			}
		}
		for(i=0;i<n;i++)
		{	
			CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(id_arr[i],CLS_PLATE);
			if(pPlate==NULL||pDatumPlate==NULL)
				break;
			if(pPlate==pDatumPlate)
				continue;	//��׼�ֹ������
			else if(pDatumPlate&&pPlate->designInfo.m_hBaseNode==pDatumPlate->designInfo.m_hBaseNode &&
				pPlate->designInfo.m_hBasePart==pDatumPlate->designInfo.m_hBasePart)
				partset.append(pPlate);//��֤��ѡ���Ӱ�Ļ�׼�ڵ�ͻ�׼�˼�����ͬ
			else if(pDatumPlate&&pPlate->jdb_style==3)
			{	//��������ֹܼа�����
				CLDSLinePart *pDatumLinePart1=NULL, *pDatumLinePart2=NULL;
				pDatumLinePart1=(CLDSLinePart*)console.FromPartHandle(pPlate->designInfo.partList.GetFirst()->hPart,CLS_LINEPART);
				pDatumLinePart2=(CLDSLinePart*)console.FromPartHandle(pPlate->designInfo.partList.GetNext()->hPart,CLS_LINEPART);
				if(pDatumLinePart1==NULL||pDatumLinePart2==NULL)
					break;
				long hBasePart=pDatumPlate->designInfo.m_hBasePart;
				long hBaseNode=pDatumPlate->designInfo.m_hBaseNode;
				if((pDatumLinePart1->handle==hBasePart||pDatumLinePart2->handle==hBasePart)&&
					(pDatumLinePart1->pStart->handle==hBaseNode||pDatumLinePart1->pEnd->handle==hBaseNode||
					pDatumLinePart2->pStart->handle==hBaseNode||pDatumLinePart2->pEnd->handle==hBaseNode))
					partset.append(pPlate);
				else
					break;
			}
			else
				break;
		}
		g_pSolidDraw->ReleaseSnapStatus();
		OnOperOther();	//ȡ����������״̬
	}
	if(partset.GetNodeNum()>1&&pDatumPlate)
	{
		int up0_down1_both2=-1;
		pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Adjust the connection plate weld line align up(U)/align down(D)/align up and down(B), up and down relative to the extension direction of datum rod<B>:","");
#else
		pCmdLine->FillCmdLine("�������Ӱ庸�����϶���(U)/�¶���(D)/���¶���(B),��������ڻ�׼�˼����췽�����<B>:","");
#endif
		while(1)
		{
			if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				return;
			if(cmdStr.GetLength()==0||cmdStr.CompareNoCase("B")==0)
				up0_down1_both2=2;
			else if(cmdStr.CompareNoCase("U")==0)
				up0_down1_both2=0;
			else if(cmdStr.CompareNoCase("D")==0)
				up0_down1_both2=1;
			else if(cmdStr.GetLength()>0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("Need the keyword of option","");
				pCmdLine->FinishCmdLine();
				pCmdLine->FillCmdLine("Adjust the connection plate weld line align up(U)/align down(D)/align up and down(B), up and down relative to the extension direction of datum rod<B>:","");
#else
				pCmdLine->FillCmdLine("��Ҫѡ��ؼ���","");
				pCmdLine->FinishCmdLine();
				pCmdLine->FillCmdLine("�������Ӱ庸�����϶���(U)/�¶���(D)/���¶���(B),��������ڻ�׼�˼����췽�����<B>:","");
#endif
				continue;
			}
			break;
		}
		CLDSLinePart *pBaseLinePart=(CLDSLinePart*)console.FromPartHandle(pDatumPlate->designInfo.m_hBasePart,CLS_LINEPART);
		CLDSNode *pBaseNode=console.FromNodeHandle(pDatumPlate->designInfo.m_hBaseNode);
		if(pBaseLinePart==NULL||pBaseNode==NULL)
		{
			pCmdLine->CancelCmdLine();
			return;
		}
		f3dLine line(pBaseLinePart->Start(),pBaseLinePart->End());
		f3dPoint origin,perp,len_vec=line.endPt-line.startPt;
		normalize(len_vec);
		SnapPerp(&origin,line.startPt,line.endPt,pBaseNode->Position(true));
		double up_len_dist=0,down_len_dist=0;
		CLDSPart *pPart=NULL;
		Ta.BeginUndoListen();
		for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			f3dPoint vertice;
			BOOL bFirst=TRUE;
			CLDSPlate *pTmPlate=(CLDSPlate*)pPart;
			double up_weld_len=0,down_weld_len=0;
			for(PROFILE_VERTEX *pVertex=pTmPlate->vertex_list.GetFirst();pVertex;pVertex=pTmPlate->vertex_list.GetNext())
			{	//����ÿ���ְ�ĺ�����ʼ�߶�/��ֹ�߶�
				vertice=pVertex->vertex;
				coord_trans(vertice,pTmPlate->ucs,TRUE);
				SnapPerp(&perp,line.startPt,line.endPt,vertice);
				f3dPoint vec=perp-origin;
				double dd=vec*len_vec;
				if(bFirst)
				{
					up_weld_len=down_weld_len=dd;
					bFirst=FALSE;
				}
				else if(dd>down_weld_len)
					down_weld_len=dd;
				else if(dd<up_weld_len)
					up_weld_len=dd;
			}
			//Ϊÿ���ְ�ĺ�����ʼ/��ֹ�߶ȸ���ֵ����ΪĬ��ֵΪ0��
			//������ֵ��ֻ�����ϻ��¶���ʱδ������λ�ûᷢ���仯 wht 09-05-31
			pTmPlate->designInfo.weld_start_high=up_weld_len;
			pTmPlate->designInfo.weld_end_high=down_weld_len;
			if(down_weld_len>down_len_dist)
				down_len_dist=down_weld_len;
			if(up_weld_len<up_len_dist)
				up_len_dist=up_weld_len;
		}
		for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			CLDSPlate *pTmPlate=(CLDSPlate*)pPart;
			pTmPlate->designInfo.iWeldLineStyle=1;
			if(up0_down1_both2==0||up0_down1_both2==2)
				pTmPlate->designInfo.weld_start_high=up_len_dist;
			if(up0_down1_both2==1||up0_down1_both2==2)
				pTmPlate->designInfo.weld_end_high=down_len_dist;
			pTmPlate->DesignPlate();
			pTmPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pTmPlate->GetSolidPartObject());
		}
		g_pSolidDraw->Draw();
		Ta.EndUndoListen();
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The selected weld length of connection plate has be adjusted successfully!");
	}
	else
		pCmdLine->FillCmdLine("Select a wrong part, or the datum nodes or datum rods of selected connection plates are different,can not adjusted the weld line:","");
	pCmdLine->FinishCmdLine();
	pCmdLine->FillCmdLine("Command:","");
#else
		AfxMessageBox("�ѳɹ�����ѡ�����Ӱ�ĺ��쳤�ȣ�");
	}
	else
		pCmdLine->FillCmdLine("ѡ���˴���Ĺ���������ѡ���Ӱ�Ļ�׼�ڵ���׼�˼�����ͬ,�޷����������߳���:","");
	pCmdLine->FinishCmdLine();
	pCmdLine->FillCmdLine("����:","");
#endif
#endif
}


void CLDSView::OnCompareLinepartPos() 
{
	m_nPrevCommandID=ID_COMPARE_LINEPART_POS;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat to compare the rod position";
#else
	m_sPrevCommandName="�ظ��Աȸ˼�λ��";
#endif
	CString cmdStr;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSLinePart *pLinePart1=NULL,*pLinePart2=NULL;
	long *id_arr,nCount=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(nCount==2)
	{
		pLinePart1=(CLDSLinePart*)console.FromPartHandle(id_arr[0],CLS_LINEPART);
		pLinePart2=(CLDSLinePart*)console.FromPartHandle(id_arr[1],CLS_LINEPART);
		if(pLinePart1==NULL||pLinePart2==NULL||
			(pLinePart1&&pLinePart2&&pLinePart1->GetClassTypeId()!=pLinePart2->GetClassTypeId()))
		{
			pLinePart1=NULL;
			pLinePart2=NULL;
		}
	}
	if(pLinePart1==NULL||pLinePart2==NULL)
	{
		while(1)
		{
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidSet->SetDisplayType(DISP_SOLID);
			g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_LINEANGLE|SELECTINDEX_LINETUBE));
			Invalidate(FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please select two rods with the similar datum:","");
#else
			pCmdLine->FillCmdLine("��ѡ������ͬ���׼�˼�:","");
#endif
			if(!pCmdLine->GetStrFromCmdLine(cmdStr))
			{	
				pCmdLine->CancelCmdLine();	
				return;
			}
			int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
			if(n==2)
			{
				pLinePart1=(CLDSLinePart*)console.FromPartHandle(id_arr[0],CLS_LINEPART);
				pLinePart2=(CLDSLinePart*)console.FromPartHandle(id_arr[1],CLS_LINEPART);
				if(pLinePart1&&pLinePart2&&(pLinePart1->GetClassTypeId()==pLinePart2->GetClassTypeId()))
					break;
				else
				{
					pLinePart1=NULL;
					pLinePart2=NULL;
					continue;
				}
			}
		}
	}
	if(pLinePart1&&pLinePart2&&(pLinePart1->GetClassTypeId()==pLinePart2->GetClassTypeId()))
	{
		CCompareLinePartPosDlg compare_dlg;
		compare_dlg.m_pDatumLinePart1=pLinePart1;
		compare_dlg.m_pDatumLinePart2=pLinePart2;
		compare_dlg.DoModal();
		/*
		BOOL bRetOk=TRUE;
		while(1)
		{
			bRetOk=compare_dlg.DoModal();
			if(compare_dlg.m_bPauseBreakExit1||compare_dlg.m_bPauseBreakExit2)
			{
				CString cmdStr;
				CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
				if(compare_dlg.m_bPauseBreakExit1)
					pCmdLine->FillCmdLine("��ѡ���һ����׼�˼���<�س���>ȷ��:","");
				else if(compare_dlg.m_bPauseBreakExit2)
					pCmdLine->FillCmdLine("��ѡ��ڶ�����׼�˼���<�س���>ȷ��:","");
				DWORD dwOldSelectTypeFlag=g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_LINEANGLE));
				if(pCmdLine->GetStrFromCmdLine(cmdStr))
				{
					long *id_arr=NULL;
					if(g_pSolidSnap->GetLastSelectEnts(id_arr)>0)
					{
						CLDSLinePart *pLinePart=NULL;
						if(compare_dlg.m_bPauseBreakExit1)
						{
							pLinePart=(CLDSLinePart*)console.FromPartHandle(id_arr[0]);
							compare_dlg.m_sLinePartHandle1.Format("0X%X",id_arr[0]);
						}
						else if(compare_dlg.m_bPauseBreakExit2)
						{
							pLinePart=(CLDSLinePart*)console.FromPartHandle(id_arr[0])
							compare_dlg.m_sLinePartHandle2.Format("0X%X",id_arr[0]);
						}
					}
				}
				g_pSolidSnap->SetSelectPartsType(dwOldSelectTypeFlag);
			}
			else
				break;
		}*/
	}
}

#ifdef __ANGLE_PART_INC_
//�õ���Ҫ���з�ת�ĽǸּ���
static void GetTurnJgSetFromScreen(JGSET *pJgSet)
{
	CLDSLineAngle *pCurAngle = NULL, *pTmpAngle = NULL;
	long *id_arr, i, n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	CHashSet<CLDSLineAngle*> hashAngleSet;
	for (i = 0; i < n; i++)
	{
		pCurAngle = (CLDSLineAngle*)console.FromPartHandle(id_arr[i], CLS_LINEANGLE, CLS_GROUPLINEANGLE);
		if (pCurAngle == NULL)
			continue;
		CLDSGroupLineAngle *pGroupAngle = NULL;
		if (pCurAngle->group_father_jg_h > 0x20)
			pGroupAngle = (CLDSGroupLineAngle*)console.FromPartHandle(pCurAngle->group_father_jg_h, CLS_GROUPLINEANGLE);
		if (pGroupAngle)
		{	//��ѡ����ϽǸ��ӽǸ֣���ת��ϽǸ�
			if (hashAngleSet.GetValue(pGroupAngle->handle))
				continue;	//�����
			else
				hashAngleSet.SetValue(pGroupAngle->handle, pGroupAngle);
			for (pTmpAngle = pJgSet->GetFirst(); pTmpAngle; pTmpAngle = pJgSet->GetNext())
			{
				if (pGroupAngle->handle == pTmpAngle->handle)
					break;
			}
			if (pTmpAngle == NULL)
				pJgSet->append(pGroupAngle);
		}
		else
			pJgSet->append(pCurAngle);
	}
}
void SyncUpdateSlavePartsWithAngleTurn(CLDSLineAngle* pAngle,char cLegX2Leg,char cLegY2Leg)
{
	CLdsPartListStack stack(&Ta);
	for (CLDSPlate* pPlate = (CLDSPlate*)Ta.EnumPartFirst(CLS_PLATE); pPlate; pPlate = (CLDSPlate*)Ta.EnumPartNext(CLS_PLATE))
	{
		CDesignLjPartPara* pRodPara=pPlate->designInfo.partList.GetValue(pAngle->handle);
		if (pRodPara!=NULL&&(pRodPara->angle.cur_wing_x0_y1==0||pRodPara->angle.cur_wing_x0_y1==1))
			pRodPara->angle.cur_wing_x0_y1=1-pRodPara->angle.cur_wing_x0_y1;	//�ߵ���ǰ���ӽǸֲ�����X/Y֫
	}
	for(CLsRef* pLsRef=pAngle->GetFirstLsRef();pLsRef;pLsRef=pAngle->GetNextLsRef())
	{
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		if(pBolt->des_base_pos.hPart==pAngle->handle)
			pBolt->des_base_pos.offset_wing=1-pBolt->des_base_pos.offset_wing;
		if(pBolt->des_base_pos.datumPoint.datum_pos_style==1&&pBolt->des_base_pos.datumPoint.des_para.RODEND.hRod==pAngle->handle)
			pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=1-pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing;
		if(pBolt->des_base_pos.datumPoint.datum_pos_style==2&&pBolt->des_base_pos.datumPoint.des_para.RODNODE.hRod==pAngle->handle)
			pBolt->des_base_pos.datumPoint.des_para.RODNODE.offset_wing=1-pBolt->des_base_pos.datumPoint.des_para.RODNODE.offset_wing;
		if (pBolt->des_work_norm.norm_style==1&&pBolt->des_work_norm.hVicePart==pAngle->handle)
			pBolt->des_work_norm.norm_wing=1-pBolt->des_work_norm.norm_wing;
		GEPOINT worknorm=pBolt->ucs.axis_z;
		pBolt->correct_worknorm();
		if(worknorm*pBolt->ucs.axis_z<0)
		{
			pBolt->des_work_norm.ReverseDirection();
			pBolt->ucs.axis_z*=-1.0;
		}
		pBolt->correct_pos();
		if (UI::blEnableIntermediateUpdateUI)
		{
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
	}
}
//turn_style 
//0.��������ת	��֫��Ϊ����һ֫   ���򲻱�
//				����֫��Ϊ����һ֫ ������
//1.�����淭ת  ��֫��Ϊ����һ֫   ������
//				����֫��Ϊ����һ֫ ���򲻱�
static bool TurnLineAngle(CLDSLineAngle* pLineAngle,int work_wing_x0_y1,int ciTurnStyle0or1)
{
	try{
		GEPOINT vxWingNorm=pLineAngle->vxWingNorm;
		GEPOINT vyWingNorm=pLineAngle->vyWingNorm;
		if(!pLineAngle->TurnLineAngle(work_wing_x0_y1,ciTurnStyle0or1))
			return false;
		char cLegXTo='Y',cLegYTo='X';
		if (vxWingNorm*pLineAngle->vyWingNorm<0)
			cLegXTo='y';
		if (vyWingNorm*pLineAngle->vyWingNorm<0)
			cLegYTo='x';
		SyncUpdateSlavePartsWithAngleTurn(pLineAngle,'Y','X');
		if(pLineAngle->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLDSView::RedrawAngleFrameDrawing(pLineAngle);
			pLineAngle->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
		}
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
		return false;
	}
	g_pSolidDraw->Draw();
	return true;
}

BOOL CLDSView::FinishTurnAngle(CLDSLineAngle *pLineAngle,int nTurnType)
{
	CLDSGroupLineAngle *pGroupAngle = NULL;
	if (pLineAngle->group_father_jg_h > 0x20)
		pGroupAngle = (CLDSGroupLineAngle*)console.FromPartHandle(pLineAngle->group_father_jg_h, CLS_GROUPLINEANGLE);
	else if (pLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		pGroupAngle = (CLDSGroupLineAngle*)pLineAngle;
	if (pGroupAngle)	//��ѡ����ϽǸ��ӽǸ֣���ת��ϽǸ�
	{
		if (pGroupAngle->ciMasterDirectIndex>0)
			pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[pGroupAngle->ciMasterDirectIndex-1],CLS_LINEANGLE);
		if (pLineAngle==NULL)
			pLineAngle=pGroupAngle;
	}
	Ta.BeginUndoListen();
	int work_wing_x0_y1=0;
	if(pLineAngle->des_norm_x.bByOtherWing)
		work_wing_x0_y1=1;
	else if(pLineAngle->des_norm_y.bByOtherWing)
		work_wing_x0_y1=0;
	else if(m_eViewFlag==PERSPECTIVE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(MessageBox("CXhChar100(Please select the work leg of angle 0X%X, Yes is X(red)/ No is Y(green).",pLineAngle->handle),NULL,MB_YESNO)==IDYES)
#else
		if(MessageBox(CXhChar100("��ѡ��Ǹ�0X%X�Ĺ���֫,��ΪX(��)/��ΪY(��).",pLineAngle->handle),NULL,MB_YESNO)==IDYES)
#endif
			work_wing_x0_y1=0;
		else
			work_wing_x0_y1=1;
	}
	if(m_eViewFlag!=PERSPECTIVE)
	{	//��͸��ͼʱӦУ�鵱ǰ����֫��������ͼ�����Ƿ�һ��,�Է����� wjh-2015.3.14
		CDisplayView* pCurrView=console.GetActiveView();
		f3dPoint wing_norm;
		if(work_wing_x0_y1==0)
			wing_norm=pLineAngle->get_norm_x_wing();
		else if(work_wing_x0_y1==1)
			wing_norm=pLineAngle->get_norm_y_wing();
		if(pCurrView!=NULL && fabs(pCurrView->ucs.axis_z*wing_norm)<0.5)
			work_wing_x0_y1=1-work_wing_x0_y1;
	}
	//��������ת��
	pLineAngle->SetDirectMasterAngle();
	TurnLineAngle(pLineAngle,work_wing_x0_y1,nTurnType);
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
	return TRUE;
}

void CLDSView::OnTurnAngleClockwise()
{
	m_nPrevCommandID = ID_TURN_ANGLE_CLOCKWISE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName = "Repeat to clockwise-turn angle";
#else
	m_sPrevCommandName = "�ظ��Ǹ�ʼ->��˳ʱ����ת";
#endif
	CString cmdStr;
	CLDSLineAngle *pTmpAngle = NULL;
	CLDSLineAngle *pAngle = NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//��Ҫ������ת�ĽǸ�
	JGSET angleSet;
	GetTurnJgSetFromScreen(&angleSet);
	if (angleSet.GetNodeNum() > 0)
	{	//������ת
		for (pTmpAngle = angleSet.GetFirst(); pTmpAngle; pTmpAngle = angleSet.GetNext())
			FinishTurnAngleClockwise(pTmpAngle);
	}
	else //if(angleSet.GetNodeNum() == 0)
	{	//������ת
		m_setSnapObjects.Empty();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("please select clockwise-turn angle:", "");
#else
		pCmdLine->FillCmdLine("��ѡ����Ҫ����ʼ->��˳ʱ����ת�ĽǸ�:", "");
#endif
		g_pSolidSet->SetOperType(OPER_OSNAP);
		g_pSolidSnap->SetSnapType(SNAP_LINE);
		m_curTask = TASK_TURN_ANGLE_CLOCKWISE;
	}
}

void CLDSView::OnTurnAngleAnticlockwise()
{
	m_nPrevCommandID = ID_TURN_ANGLE_ANTICLOCKWISE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName = "Repeat to overturn angle";
#else
	m_sPrevCommandName = "�ظ��Ǹ�ʼ->����ʱ����ת";
#endif
	CString cmdStr;
	CLDSLineAngle *pTmpAngle = NULL;
	CLDSLineAngle *pAngle = NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//��Ҫ������ת�ĽǸ�
	JGSET angleSet;
	GetTurnJgSetFromScreen(&angleSet);
	if (angleSet.GetNodeNum() > 0)
	{	//������ת
		for (pTmpAngle = angleSet.GetFirst(); pTmpAngle; pTmpAngle = angleSet.GetNext())
			FinishTurnAngleAnticlockwise(pTmpAngle);
	}
	else //if(angleSet.GetNodeNum() == 0)
	{	//������ת
		m_setSnapObjects.Empty();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("please select anticlockwise-turn angle:", "");
#else
		pCmdLine->FillCmdLine("��ѡ����Ҫ����ʼ->����ʱ����ת�ĽǸ�:", "");
#endif
		g_pSolidSet->SetOperType(OPER_OSNAP);
		g_pSolidSnap->SetSnapType(SNAP_LINE);
		m_curTask = TASK_TURN_ANGLE_ANTICLOCKWISE;
	}
}

BOOL CLDSView::FinishTurnAngleClockwise(CLDSLineAngle *pLineAngle)
{
	CLDSGroupLineAngle *pGroupAngle = NULL;
	if (pLineAngle->group_father_jg_h > 0x20)
		pGroupAngle = (CLDSGroupLineAngle*)console.FromPartHandle(pLineAngle->group_father_jg_h,CLS_GROUPLINEANGLE);
	else if (pLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		pGroupAngle = (CLDSGroupLineAngle*)pLineAngle;
	if (pGroupAngle)	//��ѡ����ϽǸ��ӽǸ֣���ת��ϽǸ�
	{
		if (pGroupAngle->ciMasterDirectIndex>0)
			pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[pGroupAngle->ciMasterDirectIndex-1],CLS_LINEANGLE);
		if (pLineAngle==NULL)
			pLineAngle=pGroupAngle;
	}
	Ta.BeginUndoListen();
	//˳ʱ��תһ������,x->y,-y->x
	pLineAngle->des_norm_y.ReverseDirection();
	DESIGN_JGWING_NORM tempwingnorm=pLineAngle->des_norm_x;
	pLineAngle->des_norm_x = pLineAngle->des_norm_y;
	pLineAngle->des_norm_y = tempwingnorm;

	int iTm=pLineAngle->desStartPos.wing_x_offset.gStyle;
	pLineAngle->desStartPos.wing_x_offset.gStyle=pLineAngle->desStartPos.wing_y_offset.gStyle;
	pLineAngle->desStartPos.wing_y_offset.gStyle=iTm;
	double fTm=pLineAngle->desStartPos.wing_x_offset.offsetDist;
	pLineAngle->desStartPos.wing_x_offset.offsetDist=pLineAngle->desStartPos.wing_y_offset.offsetDist;
	pLineAngle->desStartPos.wing_y_offset.offsetDist=fTm;
	iTm=pLineAngle->desEndPos.wing_x_offset.gStyle;
	pLineAngle->desEndPos.wing_x_offset.gStyle=pLineAngle->desEndPos.wing_y_offset.gStyle;
	pLineAngle->desEndPos.wing_y_offset.gStyle=iTm;
	fTm=pLineAngle->desEndPos.wing_x_offset.offsetDist;
	pLineAngle->desEndPos.wing_x_offset.offsetDist=pLineAngle->desEndPos.wing_y_offset.offsetDist;
	pLineAngle->desEndPos.wing_y_offset.offsetDist=fTm;
	GEPOINT vxWingNorm=pLineAngle->vxWingNorm;
	GEPOINT vyWingNorm=pLineAngle->vyWingNorm;
	pLineAngle->cal_x_wing_norm();
	pLineAngle->cal_y_wing_norm();
	pLineAngle->UnlockStart();
	pLineAngle->UnlockEnd();
	pLineAngle->CalPosition();
	pLineAngle->UpdateRelateRayJgPos();
	char cLegXTo='Y',cLegYTo='X';
	if (vxWingNorm*pLineAngle->vyWingNorm<0)
		cLegXTo='y';
	if (vyWingNorm*pLineAngle->vyWingNorm<0)
		cLegYTo='x';
	if(pGroupAngle!=NULL)
	{
		CLDSLineAngle* pSonAngle;
		pGroupAngle->UpdateSonJgWingNorm();
		pGroupAngle->UpdateSonJgPos();
		for (int i = 0; i < 4; i++)
		{
			if ((pSonAngle = (CLDSLineAngle*)Ta.FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE)) == NULL)
				break;
			SyncUpdateSlavePartsWithAngleTurn(pSonAngle,cLegXTo,cLegYTo);
			if (UI::blEnableIntermediateUpdateUI)
			{
				CLDSView::RedrawAngleFrameDrawing(pSonAngle);
				pSonAngle->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pSonAngle->GetSolidPartObject());
			}
		}
	}
	else
		SyncUpdateSlavePartsWithAngleTurn(pLineAngle,cLegXTo,cLegYTo);
	if(pLineAngle->GetClassTypeId()==CLS_LINEANGLE&&UI::blEnableIntermediateUpdateUI)
	{
		CLDSView::RedrawAngleFrameDrawing(pLineAngle);
		pLineAngle->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
	return TRUE;
}

BOOL CLDSView::FinishTurnAngleAnticlockwise(CLDSLineAngle *pLineAngle)
{
	CLDSGroupLineAngle *pGroupAngle = NULL;
	if (pLineAngle->group_father_jg_h > 0x20)
		pGroupAngle = (CLDSGroupLineAngle*)console.FromPartHandle(pLineAngle->group_father_jg_h, CLS_GROUPLINEANGLE);
	else if (pLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		pGroupAngle = (CLDSGroupLineAngle*)pLineAngle;
	if (pGroupAngle)	//��ѡ����ϽǸ��ӽǸ֣���ת��ϽǸ�
	{
		if (pGroupAngle->ciMasterDirectIndex>0)
			pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[pGroupAngle->ciMasterDirectIndex-1],CLS_LINEANGLE);
		if (pLineAngle==NULL)
			pLineAngle=pGroupAngle;
	}
	Ta.BeginUndoListen();
	//��ʱ��תһ������,-x->y,y->x
	pLineAngle->des_norm_x.ReverseDirection();
	DESIGN_JGWING_NORM tempwingnorm=pLineAngle->des_norm_x;
	pLineAngle->des_norm_x = pLineAngle->des_norm_y;
	pLineAngle->des_norm_y = tempwingnorm;

	int iTm=pLineAngle->desStartPos.wing_x_offset.gStyle;
	pLineAngle->desStartPos.wing_x_offset.gStyle=pLineAngle->desStartPos.wing_y_offset.gStyle;
	pLineAngle->desStartPos.wing_y_offset.gStyle=iTm;
	double fTm=pLineAngle->desStartPos.wing_x_offset.offsetDist;
	pLineAngle->desStartPos.wing_x_offset.offsetDist=pLineAngle->desStartPos.wing_y_offset.offsetDist;
	pLineAngle->desStartPos.wing_y_offset.offsetDist=fTm;
	iTm=pLineAngle->desEndPos.wing_x_offset.gStyle;
	pLineAngle->desEndPos.wing_x_offset.gStyle=pLineAngle->desEndPos.wing_y_offset.gStyle;
	pLineAngle->desEndPos.wing_y_offset.gStyle=iTm;
	fTm=pLineAngle->desEndPos.wing_x_offset.offsetDist;
	pLineAngle->desEndPos.wing_x_offset.offsetDist=pLineAngle->desEndPos.wing_y_offset.offsetDist;
	pLineAngle->desEndPos.wing_y_offset.offsetDist=fTm;

	GEPOINT vxWingNorm=pLineAngle->vxWingNorm;
	GEPOINT vyWingNorm=pLineAngle->vyWingNorm;
	pLineAngle->cal_x_wing_norm();
	pLineAngle->cal_y_wing_norm();
	pLineAngle->UnlockStart();
	pLineAngle->UnlockEnd();
	pLineAngle->CalPosition();
	pLineAngle->UpdateRelateRayJgPos();
	char cLegXTo='Y',cLegYTo='X';
	if (vxWingNorm*pLineAngle->vyWingNorm<0)
		cLegXTo='y';
	if (vyWingNorm*pLineAngle->vyWingNorm<0)
		cLegYTo='x';
	if(pGroupAngle!=NULL)
	{
		CLDSLineAngle* pSonAngle;
		pGroupAngle->UpdateSonJgWingNorm();
		pGroupAngle->UpdateSonJgPos();
		for (int i = 0; i < 4; i++)
		{
			if ((pSonAngle = (CLDSLineAngle*)Ta.FromPartHandle(pGroupAngle->son_jg_h[i], CLS_LINEANGLE)) == NULL)
				break;
			SyncUpdateSlavePartsWithAngleTurn(pSonAngle,cLegXTo,cLegYTo);
			if (UI::blEnableIntermediateUpdateUI)
			{
				CLDSView::RedrawAngleFrameDrawing(pSonAngle);
				pSonAngle->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pSonAngle->GetSolidPartObject());
			}
		}
	}
	else
		SyncUpdateSlavePartsWithAngleTurn(pLineAngle,cLegXTo,cLegYTo);
	if(pLineAngle->GetClassTypeId()==CLS_LINEANGLE&&UI::blEnableIntermediateUpdateUI)
	{
		CLDSView::RedrawAngleFrameDrawing(pLineAngle);
		pLineAngle->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
	return TRUE;
}

void CLDSView::OnSetupAidedPlate() 
{
	m_nPrevCommandID=ID_SETUP_AIDED_PLATE;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat tooling auxiliary plate";
#else
	m_sPrevCommandName="�ظ���װ�����ְ�";
#endif
	CAddSetupAidedPlateDlg plate_dlg;
	BOOL bRetOk=TRUE;
	CString cmdStr;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	while(1)
	{
		bRetOk=plate_dlg.DoModal();
		if(plate_dlg.m_bPauseBreakExit)
		{
			GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECTINDEX_PLATE),0,0,2);
			scr_part_para.disp_type=DISP_SOLID;
#ifdef AFX_TARG_ENU_ENGLISH
			scr_part_para.cmdStr="Please select the beginning plate and ending plate of auxiliary plate, the <enter> to confirm:";
			scr_part_para.cmdErrorFeedbackStr="There is not any appropriate selected part, please select the beginning plate and ending plate of auxiliary plate again, the <enter> to confirm:";
#else
			scr_part_para.cmdStr="��ѡ�����ְ�ʼ�˶�λ�ְ��Լ��ն˶�λ�ְ�,<�س���>ȷ��:";
			scr_part_para.cmdErrorFeedbackStr="û��ѡ�к��ʵĹ���,������ѡ�����ְ�ʼ�˶�λ�ְ��Լ��ն˶�λ�ְ�,<�س���>ȷ��:";
#endif
			if(!GetPartsFromScr(scr_part_para,FALSE))
				return ;
			if(scr_part_para.pResultObjsArr[0]&&scr_part_para.pResultObjsArr[1])
			{
				if(scr_part_para.pResultObjsArr[0]->GetClassTypeId()==CLS_PLATE&&scr_part_para.pResultObjsArr[0]->GetClassTypeId()==CLS_PLATE)
				{
					CLDSPlate *pStartPlate=(CLDSPlate*)scr_part_para.pResultObjsArr[0];
					CLDSPlate *pEndPlate=(CLDSPlate*)scr_part_para.pResultObjsArr[1];
					CLDSLineTube *pLineTube=NULL;
					if(pStartPlate&&pEndPlate&&pStartPlate->m_hPartWeldParent==pEndPlate->m_hPartWeldParent)
						pLineTube=(CLDSLineTube*)console.FromPartHandle(pStartPlate->m_hPartWeldParent,CLS_LINETUBE);
					if(pLineTube&&pStartPlate->designInfo.m_hBaseNode==pEndPlate->designInfo.m_hBaseNode)
					{
						plate_dlg.m_sStartPlateHandle.Format("0X%X",scr_part_para.pResultObjsArr[0]->handle);
						plate_dlg.m_sEndPlateHandle.Format("0X%X",scr_part_para.pResultObjsArr[1]->handle);
						plate_dlg.m_fRadius=pLineTube->GetDiameter()*0.5;
						plate_dlg.m_fAngle=cal_angle_of_2vec(pStartPlate->ucs.axis_x,pEndPlate->ucs.axis_x)*DEGTORAD_COEF;
						plate_dlg.m_sPlatePartNo.Format("%s",(char*)pStartPlate->GetPartNo());
						plate_dlg.m_fStartPlateHalfT=pStartPlate->GetThick()*0.5;
						plate_dlg.m_fEndPlateHalfT=pEndPlate->GetThick()*0.5;
						//��ʱ��ʾ������
						CLDSPlate* pStartPlate = (CLDSPlate*)console.FromPartHandle(scr_part_para.pResultObjsArr[0]->handle,CLS_PLATE);
						CLDSPlate* pEndPlate = (CLDSPlate*)console.FromPartHandle(scr_part_para.pResultObjsArr[1]->handle,CLS_PLATE);
						CLDSLineTube* pBaseLine = (CLDSLineTube*)console.FromPartHandle(pStartPlate->m_hPartWeldParent);
						plate_dlg.DisplayTmpModel(pStartPlate,pEndPlate,pBaseLine,0);
					}
					else
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("Select a wrong part!");
				}
				else
					AfxMessageBox("Select a wrong part!");
			}
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("Command:","");
#else
						AfxMessageBox("ѡ���˴���Ĺ�����");
				}
				else
					AfxMessageBox("ѡ���˴���Ĺ�����");
			}
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("����:","");
#endif
		}
		else
			break;
	}
	if(!bRetOk)
	{
		pCmdLine->CancelCmdLine();
		return;
	}
}
#else
void CLDSView::OnSetupAidedPlate(){;}
#endif

void CLDSView::OnAffirmCurCommand() 
{	//ȡ����ǰ����ִ�е�����
	CCmdLineDlg *pCmdLinePage=(CCmdLineDlg*)((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//if(pCmdLinePage)
	//	pCmdLinePage->SetCaptureLButtonUpMsg(TRUE);
}

void CLDSView::OnCancelCurCommand() 
{	//ȡ����ǰ����ִ�е�����
	CCmdLineDlg *pCmdLinePage=(CCmdLineDlg*)((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	ReleaseFunc();
	OnOperOther();
	if(pCmdLinePage)
	{
		pCmdLinePage->CancelCmdLine();
		pCmdLinePage->m_bForceExitCommand=TRUE; //ǿ���˳� wht 11-03-19
	}
}

void CLDSView::VibrationAnimate(double phaseCoef)
{
	phasePositionCoef=phaseCoef;
	g_bVibrationAnimate=TRUE;
	g_pSolidDraw->BuildDisplayList();
	g_pSolidDraw->Draw();
	g_bVibrationAnimate=FALSE;
	g_pSolidDraw->Draw();
}

void CLDSView::OnTurnAngleInoutSide() 
{
#ifdef __ANGLE_PART_INC_
	m_nPrevCommandID=ID_TURN_ANGLE_INOUT_SIDE;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat to overturn angle";
#else
	m_sPrevCommandName="�ظ��Ǹ���������ת";
#endif
	CString cmdStr;
	CLDSLineAngle *pTmpAngle=NULL;
	CLDSLineAngle *pAngle=NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//��Ҫ������ת�ĽǸ�
	JGSET angleSet;
	GetTurnJgSetFromScreen(&angleSet);
	if(angleSet.GetNodeNum()>0)
	{	//������ת
		for(pTmpAngle=angleSet.GetFirst();pTmpAngle;pTmpAngle=angleSet.GetNext())
			FinishTurnAngle(pTmpAngle,0);
	}
	else //if(angleSet.GetNodeNum() == 0)
	{	//������ת
		m_setSnapObjects.Empty();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select the angle to be overturned:","");
#else
		pCmdLine->FillCmdLine("��ѡ����Ҫ������������ת�ĽǸ�:","");
#endif
		g_pSolidSet->SetOperType(OPER_OSNAP);
		g_pSolidSnap->SetSnapType(SNAP_LINE);
		m_curTask = TASK_TURN_ANGLE_INOUT_SIDE;
	}
#endif
}

void CLDSView::OnTurnAngleWing() 
{
#ifdef __ANGLE_PART_INC_
	m_nPrevCommandID=ID_TURN_ANGLE_WING;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat to reverse angle direction";
#else
	m_sPrevCommandName="�ظ��Ǹ�����֫��ת";
#endif
	CString cmdStr;
	CLDSLineAngle *pTmpAngle=NULL;
	CLDSLineAngle *pAngle=NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//��Ҫ������ת�ĽǸ�
	JGSET angleSet;
	GetTurnJgSetFromScreen(&angleSet);
	if(angleSet.GetNodeNum()>0)
	{	//������ת
		for(pTmpAngle=angleSet.GetFirst();pTmpAngle;pTmpAngle=angleSet.GetNext())
			FinishTurnAngle(pTmpAngle,1);
	}
	else //if(angleSet.GetNodeNum() == 0)
	{	//������ת
		m_setSnapObjects.Empty();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select the angle to be reversed direction:","");
#else
		pCmdLine->FillCmdLine("��ѡ����Ҫ����������֫��ת�ĽǸ�:","");
#endif
		g_pSolidSet->SetOperType(OPER_OSNAP);
		g_pSolidSnap->SetSnapType(SNAP_LINE);
		m_curTask = TASK_TURN_ANGLE_WING;
	}
#endif
}

#ifdef __COMPLEX_PART_INC_
int CLDSView::DesignSimpleLadder()
{
	CDesignSimpleLadderDlg dlg;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//�趨��ǰ��Ҫ�ĵ���/ʵ����ʾ��ʽ
	DISPLAY_TYPE disp_type;
	g_pSolidSet->GetDisplayType(&disp_type);
	if(DISP_LINE!=disp_type)
	{	//�л���������ʾ״̬
		g_pSolidSet->SetDisplayType(DISP_LINE);
		g_pSolidDraw->Draw();
	}
#ifdef AFX_TARG_ENU_ENGLISH
    pCmdLine->FillCmdLine("Please the datum tube:","");
#else
	pCmdLine->FillCmdLine("��ѡ���׼�ֹ�:","");
#endif
	while(1)
	{
		f3dLine line;
		int ret = g_pSolidSnap->SnapLine(line);
		if(ret<0)
		{
			pCmdLine->CancelCmdLine();	
			return 0;
		}
		else if(ret>0)
		{
			CLDSLineTube *pBaseLineTube=(CLDSLineTube*)console.FromPartHandle(line.ID,CLS_LINETUBE);
			if(pBaseLineTube)
			{
				dlg.LineTubePara.pBaseLineTube = pBaseLineTube;
				g_pSolidDraw->SetEntSnapStatus(line.ID);
				break;
			}
		}
		pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("There is not any appropriate selected part��please select the datum again:","");
#else
		pCmdLine->FillCmdLine("û��ѡ�к��ʵĹ�����������ѡ���׼�˼�:","");
#endif
	}
	//�ָ�ԭ�еĵ���/ʵ����ʾ��ʽ
	g_pSolidSet->SetDisplayType(disp_type);
	g_pSolidDraw->Draw();
	dlg.DoModal();
	return 0;
}
int CLDSView::MigrateSlotNailParams()
{
	//if(doc_type==2&&nVersion<2000300)//LMA V2.0.3.0֮ǰ�汾 ˢ�½Ŷ��۸ֵ�����Ʋ���
	int hits=0;
	for(CLDSLineSlot* pSlot=(CLDSLineSlot*)Ta.Parts.GetFirst(CLS_LINESLOT);pSlot;pSlot=(CLDSLineSlot*)Ta.Parts.GetNext(CLS_LINESLOT))
	{
		f3dPoint location;
		CLDSLineTube* pBaseTube=NULL;
		if(pSlot->hAttachTube>0x20)
		{
			pBaseTube=(CLDSLineTube*)Ta.Parts.FromHandle(pSlot->hAttachTube,CLS_LINETUBE);
			location=pSlot->desDatumPoint.Position();
		}
		else 
			location=(pSlot->Start()+pSlot->End())*0.5;
		if(pBaseTube==NULL&&pSlot->m_hPartWeldParent>0x20&&pSlot->hAttachTube==0)
			pBaseTube=(CLDSLineTube*)Ta.Parts.FromHandle(pSlot->m_hPartWeldParent,CLS_LINETUBE);
		f3dPoint lenStdTubeVec,lenStdSlotVec;
		if(pBaseTube!=NULL&&pSlot->hAttachTube==0&&pSlot->pStart==NULL&&pSlot->pEnd==NULL)
		{
			lenStdTubeVec=pBaseTube->End()-pBaseTube->Start();
			normalize(lenStdTubeVec);
			lenStdSlotVec=pSlot->End()-pSlot->Start();
			normalize(lenStdSlotVec);
			if(fabs(lenStdTubeVec*lenStdSlotVec)>EPS_COS2)
				pSlot->hAttachTube=pBaseTube->handle;
		}
		if(pBaseTube==NULL||pSlot->hAttachTube<=0x20||pBaseTube->pStart==NULL||pBaseTube->pEnd==NULL)
			continue;
		if(pSlot->desDatumPoint.datum_pos_style==11)
			continue;
		//���²۸ֻ��������ж�Ϊ�ֹ��ϵĽŶ�������Ӧ��Ĭ�ϲ���ֵ
		pSlot->desDatumPoint.datum_pos_style=11;	//���涨λ��
		pSlot->desDatumPoint.des_para.COLUMN_INTERS.hLineTube=pSlot->hAttachTube;
		CLDSNode* pDatumNode=pBaseTube->pStart;
		if(pBaseTube->pEnd->Position('Z')>pBaseTube->pStart->Position('Z'))
			pDatumNode=pBaseTube->pEnd;
		f3dPoint datumStartPos=pBaseTube->GetDatumPos(pDatumNode);
		f3dPoint verifyVec=location-datumStartPos;
		double len_offset=verifyVec*lenStdTubeVec;
		pSlot->desDatumPoint.des_para.COLUMN_INTERS.cPointAndVectorFlag|=0x02;	//��������λ����Բ�ĵ�
		pSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.len_offset=fabs(len_offset);
		if(len_offset>0)
			pSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=pDatumNode->handle;
		else
			pSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=-pDatumNode->handle;
		if(pBaseTube->Layer(0)!='T'&&pBaseTube->Layer(1)=='Z')	//���������ϽŶ��Ժ�����Ϊ��ת��ʼ��
			pSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style=3;
		else
			pSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style=2;
		pBaseTube->BuildUCS();
		f3dPoint startEdgeVec=pBaseTube->ucs.axis_x;
		double weldRoadAngle=0;	//������
		if(pSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style==3)
			weldRoadAngle=pBaseTube->CalWeldRoadAngle();
		verifyVec-=(verifyVec*lenStdTubeVec)*lenStdTubeVec;
		normalize(verifyVec);
		vector_trans(verifyVec,pBaseTube->ucs,FALSE,TRUE);
		double rotAngle=Cal2dLineAng(0,0,verifyVec.x,verifyVec.y);
		pSlot->desDatumPoint.des_para.COLUMN_INTERS.RayVector.ray_vec=verifyVec;
		pSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_angle=(rotAngle-weldRoadAngle)*DEGTORAD_COEF;
		pSlot->fLen=pSlot->GetLength();
		hits++;
	}
	MessageBox(CXhChar50("����%d���ֹ��ϵĽŶ��۸ֵ�����ɲ�����������ȡ!",hits));
	return hits;
}
#endif

void CLDSView::LdsWorkEnvShift(WORKENV_TYPE env_type)
{
	static int iColorSchema;
	CLogErrorLife loglife;
	if(env_type==SOLID_ENV && m_curAppType == TA_ASSEMBLE_ENV &&
		PRODUCT_FUNC::IsHasInternalTest())	//����ɫ����
		g_pSolidSet->SetBkSceneMode(1);
	else
		g_pSolidSet->SetBkSceneMode(0);
#ifdef __PART_DESIGN_INC_
	if(env_type==FEMELEM_ENV)
	{
		CWaitCursor wait;
		if(!CheckNodeI(NULL,NULL))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Did not pass the nodes number check, please check after the nodes are numbered again!");
#else
			AfxMessageBox("δͨ���ڵ��ż��,�����½��нڵ��ź��ٽ��в�����");
#endif
			return;
		}
		preprocessor.InitSourceDesignInfo(&Ta);
	}
#ifdef __GGG_
	else if(env_type==DRAWING2D_ENV)
		m_pDraw2d->BuildDisplayBuffer();
#endif
	if( (theApp.m_ePrevEnvType==LINE_ENV||theApp.m_ePrevEnvType==SOLID_ENV)&&
		(env_type==FEMROD_ENV||env_type==FEMELEM_ENV))
	{	//��ʵ����ʾ�������͸��µ�����������ʱ����
#ifdef __ANGLE_PART_INC_
		for(CLDSLinePart* pLinePart=Ta.Parts.GetFirstLinePart();pLinePart;pLinePart=Ta.Parts.GetNextLinePart())
		{
			if( pLinePart->size_idClassType==CLS_LINEANGLE||pLinePart->size_idClassType==CLS_GROUPLINEANGLE||
				pLinePart->size_idClassType==CLS_LINETUBE)
			pLinePart->size_idClassType=pLinePart->GetClassTypeId();	//���µ������ݲ�֧��ת��
			if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pLinePart;
				if(pGroupAngle->group_style==0)
					pLinePart->size_cSubClassType='D';
				else if(pGroupAngle->group_style==1)
					pLinePart->size_cSubClassType='T';
				else if(pGroupAngle->group_style==2)
					pLinePart->size_cSubClassType='X';
			}
		}
#endif
		if(env_type==FEMELEM_ENV)
		{
			CMainFrame *pMainFrm=(CMainFrame*)theApp.m_pMainWnd;
			//CRevisionOperDlg *pRevision=(CRevisionOperDlg*)pMainFrm->GetDockPage(RUNTIME_CLASS(CRevisionOperDlg));
			CRevisionOperDlg *pRevision=pMainFrm->GetRevisionOperPage();
			pRevision->UpdateRevision(postreport);
		}
		if(g_sysPara.iColorSchema>1)
			iColorSchema=g_sysPara.iColorSchema;
		g_sysPara.iColorSchema = 1;
		g_pSolidSet->SetBkColor(globalLibrary.GetColor(0,0,0));
		g_pSolidSet->SetDisplayType(DISP_LINE);
		g_pSolidDraw->BuildDisplayList();
		UpdatePropertyPage();
	}
	else if(env_type==FEMROD_ENV||env_type==FEMELEM_ENV)
		g_pSolidDraw->BuildDisplayList();
#endif
#ifdef __ANGLE_PART_INC_
	if(env_type==LINE_ENV||env_type==SOLID_ENV)
	{
		theApp.m_bAnalysisWorkEnv=FALSE;
		//������ɫ������ֵ������ڴ˴�����ΪCorrectLayoutJgStyle�л��漰�Ǹ�ʵ���ʵ����
		if(iColorSchema!=0)
			g_sysPara.iColorSchema=iColorSchema;
		else
			g_sysPara.iColorSchema=3;	//����ģʽ
#ifdef __SMART_DRAW_INC_
		//�������ѧ�������ͼ����ת��ʱ��Ӧ�����е�CLDSLinePartת������Ӧ���������ͬʱ��Ӧ��
		//CLDSLinePart::size.idClassType��m_nClassTypeId��һ�µĶ�����size.idClassTypeΪ��׼����ת��
		//����������ĽǸּ�����Ӧ����Ľ��Ǹֵ�֫������������ж�
		CLDSPart *pPart;
		CLDSLinePart *pLinePart;
		LINEPARTSET newpoleset;
		CArray<long,long>id_arr,disppartid_arr;
		for(pPart=console.DispPartSet.GetFirst();pPart;pPart=console.DispPartSet.GetNext())
			disppartid_arr.Add(pPart->handle);
		for(pLinePart=Ta.Parts.GetFirstLinePart();pLinePart;pLinePart=Ta.Parts.GetNextLinePart())
		{
			CSuperSmartPtr<CLDSLinePart>pNewLinePart;
			Ta.Parts.push_stack();
			//��ȡ��ǰ��ϽǸֵ�����
			bool group_modified=false;
			if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pLinePart;
				/*char cSubClassType=' ';
				if(pGroupAngle->group_style==0)
					cSubClassType='D';
				else if(pGroupAngle->group_style==1)
					cSubClassType='T';
				else if(pGroupAngle->group_style==2)
					cSubClassType='X';*/
				bool needUpdate=(pGroupAngle->son_jg_h[0]==0||pGroupAngle->son_jg_h[1]==0);
				if(!needUpdate&&pGroupAngle->size_cSubClassType=='X'&&(
					pGroupAngle->son_jg_h[2]==0||pGroupAngle->son_jg_h[3]==0))
					needUpdate=true;
				if(needUpdate)
					pGroupAngle->AppendSonJg();
				pGroupAngle->UpdateSonJgBasicInfo();	//��ֹ��ǰʵ����ʱ�ӽǸֹ����������ϽǸֹ��һ�� wjh-2011.8.18
				if(needUpdate)
				{
					pGroupAngle->UpdateSonJgWingNorm();
					pGroupAngle->UpdateSonJgPos();
				}
				group_modified=pGroupAngle->IsNeedUpdateGroupStyle();
			}
			if((pLinePart->GetClassTypeId()!=pLinePart->size_idClassType&&pLinePart->size_idClassType!=0)			//ת����������
				||(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE&&group_modified))	//ת����ϽǸ�����
			{	//�ݲ�����ʾ�û��Զ���Ĺ���
				long idOldClassType=pLinePart->GetClassTypeId();
				pNewLinePart=(CLDSLinePart*)Ta.Parts.InstantiateCursorLinePart();
				if(pNewLinePart.IsHasPtr())
				{
					pNewLinePart->feature=idOldClassType;
					newpoleset.append(pNewLinePart);
				}
			}
			Ta.Parts.pop_stack();
		}
		id_arr.SetSize(0,2);
		for(pLinePart=newpoleset.GetFirst();pLinePart;pLinePart=newpoleset.GetNext())
		{
			if(pLinePart->GetClassTypeId()==CLS_LINEANGLE||pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pLinePart;
				//if(pLineAngle->feature==CLS_LINEPART)	
				//�����Ǹֹ�����ת����ϽǸ�����,��ҪҪ�����ж�֫���߷���λ��
				if(pLineAngle->GetClassTypeId()!=pLineAngle->feature	//��������
					||pLineAngle->feature==CLS_GROUPLINEANGLE)			//ת����ϽǸ�����
				{
					Ta.SmartCalAngleWingNorm(pLineAngle);
					if(pLineAngle->feature==CLS_LINEPART)	
						id_arr.Add(pLineAngle->handle);	//����������Ҫ�Զ������Ǹְڷ�λ��
				}
				if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
				{
					CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pLinePart;
					pGroupAngle->UpdateSonJgBasicInfo();
					pGroupAngle->UpdateSonJgWingNorm();
					pGroupAngle->UpdateSonJgPos();
				}
			}
			else if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{
				CLDSLineTube *pTube=(CLDSLineTube*)pLinePart;
				f3dPoint ref_vec,line_vec=pTube->pEnd->Position(false)-pTube->pStart->Position(false);
				normalize(line_vec);
				CSuperSmartPtr<CLDSPart>pFatherPart;
				pTube->CalPosition();
				if(pTube->layer(0)!='T'&&pTube->layer(1)=='Z')
					pFatherPart=pTube->GetMirPart(MIRMSG(2));
				if(pFatherPart.IsHasPtr()&&pFatherPart->IsLinePart())
					ref_vec=pFatherPart.LinePartPointer()->pEnd->Position(false)-pFatherPart.LinePartPointer()->pStart->Position(false);
				normalize(ref_vec);
				if(fabs(ref_vec*line_vec)<EPS_COS)
					pTube->hWorkNormRefPart=pFatherPart->handle;
				else
				{
					if(pTube->pStart->hFatherPart!=pTube->handle)
						pFatherPart=console.FromPartHandle(pTube->pStart->hFatherPart);
					if(pFatherPart.IsHasPtr()&&pFatherPart->IsLinePart())
						ref_vec=pFatherPart.LinePartPointer()->pEnd->Position(false)-pFatherPart.LinePartPointer()->pStart->Position(false);
					normalize(ref_vec);
					if(fabs(ref_vec*line_vec)<EPS_COS)
						pTube->hWorkNormRefPart=pFatherPart->handle;
					else if(pTube->pEnd->hFatherPart!=pTube->handle)
					{
						pFatherPart=console.FromPartHandle(pTube->pEnd->hFatherPart);
						if(pFatherPart.IsHasPtr()&&pFatherPart->IsLinePart())
						{
							ref_vec=pFatherPart.LinePartPointer()->pEnd->Position(false)-pFatherPart.LinePartPointer()->pStart->Position(false);
							normalize(ref_vec);
							if(fabs(ref_vec*line_vec)<EPS_COS)
								pTube->hWorkNormRefPart=pFatherPart->handle;
						}
					}
				}
				if(pTube->hWorkNormRefPart>0x20)
					pTube->CalWorkPlaneNorm();
			}
		}
		if(id_arr.GetSize()>0)
			CorrectJgLaystyle(id_arr.GetData(),id_arr.GetSize());
		console.DispPartSet.Empty();
		for(int i=0;i<disppartid_arr.GetSize();i++)
		{
			if ((pPart = console.FromPartHandle(disppartid_arr[i])) == NULL)
			{
				logerr.Log("��ʾ�б��е�0x%X�����Ѳ�����!", disppartid_arr[i]);
				continue;
			}
			console.DispPartSet.append(pPart);
			if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{	//����ϽǸ��ӽǸ���ӵ���ʾ���� 
				CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pPart;
				for(int j=0;j<4;j++)
				{
					if(pGroupAngle->son_jg_h[j]>0x20)
					{
						CLDSPart* pSonAngle=Ta.Parts.FromHandle(pGroupAngle->son_jg_h[j]);
						if (pSonAngle != NULL&&console.DispPartSet.FromHandle(pSonAngle->handle)==NULL)
							console.DispPartSet.append(pSonAngle);
						else
							logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"��ʾ�б��е�0x%X��ϽǸֵ��ӽǸ�0x%X�Ѳ�����!", pGroupAngle->handle,pGroupAngle->son_jg_h[j]);
					}
				}
			}
		}
#endif
		g_pSolidSet->SetBkColor(globalLibrary.GetColor(0,0,0));
		if(env_type==LINE_ENV)
		{
			g_pSolidSet->SetDisplayType(DISP_LINE);
			g_pSolidDraw->BuildDisplayList();
		}
		else
		{
			g_pSolidSet->SetDisplayType(DISP_SOLID);
			g_pSolidDraw->BuildDisplayList();
		}
	}
#endif
	Invalidate(false);
}

void CLDSView::OnDesignSimpleLadder() 
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
{
	m_nPrevCommandID=ID_DESIGN_SIMPLE_LADDER;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat simple tube ladder";
#else
	m_sPrevCommandName="�ظ��ֹܼ�������";
#endif
	Command("PT");
}
#else
{;}
#endif

void CLDSView::OnDefAllIntersNode() 
{
	DefAllIntersNode();
}

void CLDSView::OnUpdateDisplayAllHole(CCmdUI* pCmdUI) 
{
	CMenu *pMenu=pCmdUI->m_pMenu;
	if(pMenu)
	{
		if(g_sysPara.bDisplayAllHole)
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->ModifyMenu(ID_DISPLAY_OR_HIDE_ALL_HOLE,MF_BYCOMMAND,ID_DISPLAY_OR_HIDE_ALL_HOLE,"Hide all holes(&D)\tF3");
		else
			pMenu->ModifyMenu(ID_DISPLAY_OR_HIDE_ALL_HOLE,MF_BYCOMMAND,ID_DISPLAY_OR_HIDE_ALL_HOLE,"Show all holes(&D)\tF3");
#else
			pMenu->ModifyMenu(ID_DISPLAY_OR_HIDE_ALL_HOLE,MF_BYCOMMAND,ID_DISPLAY_OR_HIDE_ALL_HOLE,"�������п�(&D)\tF3");
		else
			pMenu->ModifyMenu(ID_DISPLAY_OR_HIDE_ALL_HOLE,MF_BYCOMMAND,ID_DISPLAY_OR_HIDE_ALL_HOLE,"��ʾ���п�(&D)\tF3");
#endif
	}
}

/*�ú������ܾ���ʵ������DataCenter.cpp�е�ExecuteEmendMirRodInfo�С�wjh-2015.3.2
void CLDSView::OnAddMirRelativeInfo() 
{
	RELATIVE_OBJECT relaObj;
	int hits=0,sum=Ta.Node.GetNodeNum()+Ta.Parts.GetNodeNum();
	for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		hits++;
		BOOL bPush=Ta.Node.push_stack();
		CLDSNode *pXMirNode=pNode->GetMirNode(MIRMSG(1),false);
		CLDSNode *pYMirNode=pNode->GetMirNode(MIRMSG(2),false);
		CLDSNode *pZMirNode=pNode->GetMirNode(MIRMSG(3),false);
		if(pXMirNode==pNode)
			pXMirNode=NULL;
		if(pYMirNode==pNode)
			pYMirNode=NULL;
		if(pZMirNode==pNode)
			pZMirNode=NULL;
		if(pXMirNode&&pXMirNode!=pNode)
		{
			relaObj.hObj=pXMirNode->handle;
			relaObj.mirInfo.axis_flag=0x01;
			pNode->AppendRelativeObj(relaObj);
			relaObj.hObj=pNode->handle;
			pXMirNode->AppendRelativeObj(relaObj);
			if(pYMirNode&&pYMirNode!=pXMirNode)
			{
				relaObj.hObj=pYMirNode->handle;
				relaObj.mirInfo.axis_flag=0x04;
				pXMirNode->AppendRelativeObj(relaObj);
				relaObj.hObj=pXMirNode->handle;
				pYMirNode->AppendRelativeObj(relaObj);
			}
			if(pZMirNode&&pZMirNode!=pXMirNode)
			{
				relaObj.hObj=pZMirNode->handle;
				relaObj.mirInfo.axis_flag=0x02;
				pXMirNode->AppendRelativeObj(relaObj);
				relaObj.hObj=pXMirNode->handle;
				pZMirNode->AppendRelativeObj(relaObj);
			}
		}
		if(pYMirNode&&pYMirNode!=pNode)
		{
			relaObj.hObj=pYMirNode->handle;
			relaObj.mirInfo.axis_flag=0x02;
			pNode->AppendRelativeObj(relaObj);
			relaObj.hObj=pNode->handle;
			pYMirNode->AppendRelativeObj(relaObj);
			if(pZMirNode&&pYMirNode!=pZMirNode)
			{
				relaObj.hObj=pZMirNode->handle;
				relaObj.mirInfo.axis_flag=0x01;
				pYMirNode->AppendRelativeObj(relaObj);
				relaObj.hObj=pYMirNode->handle;
				pZMirNode->AppendRelativeObj(relaObj);
			}
		}
		if(pZMirNode&&pZMirNode!=pNode)
		{
			relaObj.hObj=pZMirNode->handle;
			relaObj.mirInfo.axis_flag=0x04;
			pNode->AppendRelativeObj(relaObj);
			relaObj.hObj=pNode->handle;
			pZMirNode->AppendRelativeObj(relaObj);
		}
		if(bPush)
			Ta.Node.pop_stack();
		DisplayProcess(hits*100/sum);
	}
	for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		hits++;
		BOOL pushed=Ta.Parts.push_stack();
		if(!pPart->IsLinePart())
			continue;
		CLDSLinePart *pRod=(CLDSLinePart*)pPart;
		CLDSLinePart *pXMirRod=(CLDSLinePart*)pRod->GetMirPart(MIRMSG(1));
		CLDSLinePart *pYMirRod=(CLDSLinePart*)pRod->GetMirPart(MIRMSG(2));
		CLDSLinePart *pZMirRod=(CLDSLinePart*)pRod->GetMirPart(MIRMSG(3));
		if(pXMirRod==pRod)
			pXMirRod=NULL;
		if(pYMirRod==pRod)
			pYMirRod=NULL;
		if(pZMirRod==pRod)
			pZMirRod=NULL;
		if(pXMirRod&&pXMirRod!=pRod)
		{
			relaObj.hObj=pXMirRod->handle;
			relaObj.mirInfo.axis_flag=0x01;
			pRod->AppendRelativeObj(relaObj);
			relaObj.hObj=pRod->handle;
			pXMirRod->AppendRelativeObj(relaObj);
			if(pYMirRod&&pXMirRod!=pYMirRod)
			{
				relaObj.hObj=pYMirRod->handle;
				relaObj.mirInfo.axis_flag=0x04;
				pXMirRod->AppendRelativeObj(relaObj);
				relaObj.hObj=pXMirRod->handle;
				pYMirRod->AppendRelativeObj(relaObj);
			}
			if(pZMirRod&&pXMirRod!=pZMirRod)
			{
				relaObj.hObj=pZMirRod->handle;
				relaObj.mirInfo.axis_flag=0x02;
				pXMirRod->AppendRelativeObj(relaObj);
				relaObj.hObj=pXMirRod->handle;
				pZMirRod->AppendRelativeObj(relaObj);
			}
		}
		if(pYMirRod&&pYMirRod!=pRod)
		{
			relaObj.hObj=pYMirRod->handle;
			relaObj.mirInfo.axis_flag=0x02;
			pRod->AppendRelativeObj(relaObj);
			relaObj.hObj=pRod->handle;
			pYMirRod->AppendRelativeObj(relaObj);
			if(pZMirRod&&pYMirRod!=pZMirRod)
			{
				relaObj.hObj=pZMirRod->handle;
				relaObj.mirInfo.axis_flag=0x01;
				pYMirRod->AppendRelativeObj(relaObj);
				relaObj.hObj=pYMirRod->handle;
				pZMirRod->AppendRelativeObj(relaObj);
			}
		}
		if(pZMirRod&&pZMirRod!=pRod)
		{
			relaObj.hObj=pZMirRod->handle;
			relaObj.mirInfo.axis_flag=0x04;
			pRod->AppendRelativeObj(relaObj);
			relaObj.hObj=pRod->handle;
			pZMirRod->AppendRelativeObj(relaObj);
		}
		Ta.Parts.pop_stack(pushed);
		hits++;
		DisplayProcess(hits*100/sum);
	}
	DisplayProcess(100);
}
*/
static int CompareRodPointI(CLDSNode* pStart,CLDSNode* pEnd,CLDSNode* pStart2,CLDSNode* pEnd2)
{
	int startI=0,endI=0,startI2=0,endI2=0;
#ifdef __PART_DESIGN_INC_//defined(__TSA_)||defined(__LDS_)
	if(pStart)
		startI=pStart->point_i;
	if(pEnd)
		endI=pEnd->point_i;
	if(pStart2)
		startI2=pStart2->point_i;
	if(pEnd2)
		endI2=pEnd2->point_i;
#else
	if(pStart)
		startI=pStart->handle;
	if(pEnd)
		endI=pEnd->handle;
	if(pStart2)
		startI2=pStart2->handle;
	if(pEnd2)
		endI2=pEnd2->handle;
#endif
	long start_i=min(startI,endI);
	long end_i=max(startI,endI);
	long start_i2=min(startI2,endI2);
	long end_i2=max(startI2,endI2);
	if(start_i<start_i2)
		return -1;
	else if(start_i>start_i2)
		return 1;
	else if(end_i<end_i2)
		return -1;
	else if(end_i>end_i2)
		return 1;
	else
		return 0;
}
void CLDSView::OnCheckRelativeMirInfo() 
{
	RELATIVE_OBJECT* pRelaObj;
	int bad_hits=0;
	int hits=0,sum=Ta.Node.GetNodeNum()+Ta.Parts.GetNodeNum();
	FILE* fp=fopen("badmir.txt","wt");
	for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		hits++;
		BOOL bPush=Ta.Node.push_stack();
		for(pRelaObj=pNode->relativeObjs.GetFirst();pRelaObj;pRelaObj=pNode->relativeObjs.GetNext())
		{
			if(pRelaObj->hObj==pNode->handle)	//�ԶԳ�
			{
				pNode->relativeObjs.DeleteCursor();
				continue;
			}
			CLDSNode *pMirNode=console.FromNodeHandle(pRelaObj->hObj);
			int bad_mir=0;
			if(pMirNode==NULL)
				bad_mir=-1;
			else if( pRelaObj->mirInfo.axis_flag==0x01&&		//X��Գ�
				(fabs(pNode->Position(false).x-pMirNode->Position(false).x)>EPS||fabs(pNode->Position(false).y+pMirNode->Position(false).y)>EPS||fabs(pNode->Position(false).z-pMirNode->Position(false).z)>EPS))
					bad_mir=1;
			else if(pRelaObj->mirInfo.axis_flag==0x02&&	//Y��Գ�
				(fabs(pNode->Position(false).x+pMirNode->Position(false).x)>EPS||fabs(pNode->Position(false).y-pMirNode->Position(false).y)>EPS||fabs(pNode->Position(false).z-pMirNode->Position(false).z)>EPS))
					bad_mir=2;
			else if(pRelaObj->mirInfo.axis_flag==0x04&&	//Z��Գ�
				(fabs(pNode->Position(false).x+pMirNode->Position(false).x)>EPS||fabs(pNode->Position(false).y+pMirNode->Position(false).y)>EPS||fabs(pNode->Position(false).z-pMirNode->Position(false).z)>EPS))
					bad_mir=3;
			if(bad_mir!=0)
			{
				bad_hits++;
				if(bad_mir<0)
#ifdef AFX_TARG_ENU_ENGLISH
					fprintf(fp,"The symmetrical relative object 0X%X of 0X%X node can not be found, record has been cleared!\n",pNode->handle,pRelaObj->hObj);
				else if(bad_mir==1)
					fprintf(fp,"The X axis symmetrical relative object 0X%X of 0X%X node is X axis asymmetric,record has been cleared!\n",pNode->handle,pRelaObj->hObj);
				else if(bad_mir==2)
					fprintf(fp,"The Y axis symmetrical relative object 0X%X of 0X%X node is X axis asymmetric,record has been cleared!\n",pNode->handle,pRelaObj->hObj);
				else if(bad_mir==3)
					fprintf(fp,"The Z axis symmetrical relative object 0X%X of 0X%X node is X axis asymmetric,record has been cleared!\n",pNode->handle,pRelaObj->hObj);
#else
					fprintf(fp,"0X%X�ڵ�ĶԳƹ�������0X%X�Ҳ���,��¼�������\n",pNode->handle,pRelaObj->hObj);
				else if(bad_mir==1)
					fprintf(fp,"0X%X�ڵ��X��Գƹ�������0X%X��X��Գ�,��¼�������\n",pNode->handle,pRelaObj->hObj);
				else if(bad_mir==2)
					fprintf(fp,"0X%X�ڵ��Y��Գƹ�������0X%X��X��Գ�,��¼�������\n",pNode->handle,pRelaObj->hObj);
				else if(bad_mir==3)
					fprintf(fp,"0X%X�ڵ��Z��Գƹ�������0X%X��X��Գ�,��¼�������\n",pNode->handle,pRelaObj->hObj);
#endif
				pNode->relativeObjs.DeleteCursor();
			}
		}
		if(bPush)
			Ta.Node.pop_stack();
		DisplayProcess(hits*100/sum);
	}
	for(CSuperSmartPtr<CLDSPart> pPart=Ta.Parts.GetFirst();pPart.IsHasPtr();pPart=Ta.Parts.GetNext())
	{
		hits++;
		BOOL pushed=Ta.Parts.push_stack();
		if(!pPart->IsLinePart())
			continue;
		for(pRelaObj=pPart->relativeObjs.GetFirst();pRelaObj;pRelaObj=pPart->relativeObjs.GetNext())
		{
			if(pRelaObj->hObj==pPart->handle)	//�ԶԳ�
			{
				pPart->relativeObjs.DeleteCursor();
				continue;
			}
			if(pRelaObj->mirInfo.axis_flag>=0x08)
				continue;	//��ת�ԳƲ�����
			CSuperSmartPtr<CLDSPart> pMirPart=console.FromPartHandle(pRelaObj->hObj);
			int bad_mir=0;
			if(pMirPart.IsNULL())
				bad_mir=-1;
			else if(!pMirPart->IsLinePart())
				bad_mir=-2;
			else if( pRelaObj->mirInfo.axis_flag==0x01&&		//X��Գ�
				((fabs(pPart.LinePartPointer()->pStart->Position(false).x-pMirPart.LinePartPointer()->pStart->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).x-pMirPart.LinePartPointer()->pEnd->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).z-pMirPart.LinePartPointer()->pStart->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).z-pMirPart.LinePartPointer()->pEnd->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).y+pMirPart.LinePartPointer()->pStart->Position(false).y)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).y+pMirPart.LinePartPointer()->pEnd->Position(false).y)>EPS)&&

				(fabs(pPart.LinePartPointer()->pStart->Position(false).x-pMirPart.LinePartPointer()->pEnd->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).x-pMirPart.LinePartPointer()->pStart->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).z-pMirPart.LinePartPointer()->pEnd->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).z-pMirPart.LinePartPointer()->pStart->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).y+pMirPart.LinePartPointer()->pEnd->Position(false).y)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).y+pMirPart.LinePartPointer()->pStart->Position(false).y)>EPS)))
					bad_mir=1;
			else if(pRelaObj->mirInfo.axis_flag==0x02&&	//Y��Գ�
				((fabs(pPart.LinePartPointer()->pStart->Position(false).x+pMirPart.LinePartPointer()->pStart->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).x+pMirPart.LinePartPointer()->pEnd->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).z-pMirPart.LinePartPointer()->pStart->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).z-pMirPart.LinePartPointer()->pEnd->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).y-pMirPart.LinePartPointer()->pStart->Position(false).y)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).y-pMirPart.LinePartPointer()->pEnd->Position(false).y)>EPS)&&

				(fabs(pPart.LinePartPointer()->pStart->Position(false).x+pMirPart.LinePartPointer()->pEnd->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).x+pMirPart.LinePartPointer()->pStart->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).z-pMirPart.LinePartPointer()->pEnd->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).z-pMirPart.LinePartPointer()->pStart->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).y-pMirPart.LinePartPointer()->pEnd->Position(false).y)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).y-pMirPart.LinePartPointer()->pStart->Position(false).y)>EPS)))
					bad_mir=2;
			else if(pRelaObj->mirInfo.axis_flag==0x04&&	//Z��Գ�
				((fabs(pPart.LinePartPointer()->pStart->Position(false).x+pMirPart.LinePartPointer()->pStart->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).x+pMirPart.LinePartPointer()->pEnd->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).z-pMirPart.LinePartPointer()->pStart->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).z-pMirPart.LinePartPointer()->pEnd->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).y+pMirPart.LinePartPointer()->pStart->Position(false).y)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).y+pMirPart.LinePartPointer()->pEnd->Position(false).y)>EPS)&&

				(fabs(pPart.LinePartPointer()->pStart->Position(false).x+pMirPart.LinePartPointer()->pEnd->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).x+pMirPart.LinePartPointer()->pStart->Position(false).x)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).z-pMirPart.LinePartPointer()->pEnd->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).z-pMirPart.LinePartPointer()->pStart->Position(false).z)>EPS||
				fabs(pPart.LinePartPointer()->pStart->Position(false).y+pMirPart.LinePartPointer()->pEnd->Position(false).y)>EPS||
				fabs(pPart.LinePartPointer()->pEnd->Position(false).y+pMirPart.LinePartPointer()->pStart->Position(false).y)>EPS)))
					bad_mir=3;
			else if(pPart->cMaterial!=pMirPart->cMaterial)
			{
				if(CompareRodPointI(pPart.LinePartPointer()->pStart,pPart.LinePartPointer()->pEnd,
					pMirPart.LinePartPointer()->pStart,pMirPart.LinePartPointer()->pEnd)<=0)
					pMirPart->cMaterial=pPart->cMaterial;
				else
					pPart->cMaterial=pMirPart->cMaterial;
				bad_mir=4;
			}
#ifdef __PART_DESIGN_INC_//fined(__TSA_)||defined(__LDS_)
			else if(pPart.LinePartPointer()->m_iElemType!=pMirPart.LinePartPointer()->m_iElemType)
			{
				if(CompareRodPointI(pPart.LinePartPointer()->pStart,pPart.LinePartPointer()->pEnd,
					pMirPart.LinePartPointer()->pStart,pMirPart.LinePartPointer()->pEnd)<=0)
					pMirPart.LinePartPointer()->m_iElemType=pPart.LinePartPointer()->m_iElemType;
				else
					pPart.LinePartPointer()->m_iElemType=pMirPart.LinePartPointer()->m_iElemType;
				bad_mir=5;
			}
			else if(pPart.LinePartPointer()->start_joint_type!=pMirPart.LinePartPointer()->start_joint_type)
			{
				if(CompareRodPointI(pPart.LinePartPointer()->pStart,pPart.LinePartPointer()->pEnd,
					pMirPart.LinePartPointer()->pStart,pMirPart.LinePartPointer()->pEnd)<=0)
					pMirPart.LinePartPointer()->start_joint_type=pPart.LinePartPointer()->start_joint_type;
				else
					pPart.LinePartPointer()->start_joint_type=pMirPart.LinePartPointer()->start_joint_type;
				bad_mir=6;
			}
			else if(pPart.LinePartPointer()->end_joint_type!=pMirPart.LinePartPointer()->end_joint_type)
			{
				if(CompareRodPointI(pPart.LinePartPointer()->pStart,pPart.LinePartPointer()->pEnd,
					pMirPart.LinePartPointer()->pStart,pMirPart.LinePartPointer()->pEnd)<=0)
					pMirPart.LinePartPointer()->end_joint_type=pPart.LinePartPointer()->end_joint_type;
				else
					pPart.LinePartPointer()->end_joint_type=pMirPart.LinePartPointer()->end_joint_type;
				bad_mir=7;
			}
			else if(pPart.LinePartPointer()->start_force_type!=pMirPart.LinePartPointer()->start_force_type)
			{
				if(CompareRodPointI(pPart.LinePartPointer()->pStart,pPart.LinePartPointer()->pEnd,
					pMirPart.LinePartPointer()->pStart,pMirPart.LinePartPointer()->pEnd)<=0)
					pMirPart.LinePartPointer()->start_force_type=pPart.LinePartPointer()->start_force_type;
				else
					pPart.LinePartPointer()->start_force_type=pMirPart.LinePartPointer()->start_force_type;
				bad_mir=8;
			}
			else if(pPart.LinePartPointer()->end_force_type!=pMirPart.LinePartPointer()->end_force_type)
			{
				if(CompareRodPointI(pPart.LinePartPointer()->pStart,pPart.LinePartPointer()->pEnd,
					pMirPart.LinePartPointer()->pStart,pMirPart.LinePartPointer()->pEnd)<=0)
					pMirPart.LinePartPointer()->end_force_type=pPart.LinePartPointer()->end_force_type;
				else
					pPart.LinePartPointer()->end_force_type=pMirPart.LinePartPointer()->end_force_type;
				bad_mir=9;
			}
#endif
			if(bad_mir!=0)
			{
				bad_hits++;
				if(bad_mir==-1)
#ifdef AFX_TARG_ENU_ENGLISH
					fprintf(fp,"The symmetrical relative objects 0X%X of 0X%X part can not be found, record has been cleared!\n",pRelaObj->hObj,pPart->handle);
				else if(bad_mir==-1)
					fprintf(fp,"The X axis symmetrical relative object 0X%X of 0X%X rod is not a rod, record has been cleared!\n",pRelaObj->hObj,pPart->handle);
				else if(bad_mir==1)
					fprintf(fp,"The X axis symmetrical relative object 0X%X of 0X%X rod is X axis asymmetric,record has been cleared!\n",pRelaObj->hObj,pPart->handle);
				else if(bad_mir==2)
					fprintf(fp,"The Y axis symmetrical relative object 0X%X of 0X%X rod is X axis asymmetric,record has been cleared!\n",pRelaObj->hObj,pPart->handle);
				else if(bad_mir==3)
					fprintf(fp,"The Z axis symmetrical relative object 0X%X of 0X%X rod is X axis asymmetric,record has been cleared!\n",pRelaObj->hObj,pPart->handle);
				else if(bad_mir==4)
					fprintf(fp,"The material of 0X%X rod is different from the one of relative object 0X%X, it has been corrected,please check!\n",pRelaObj->hObj,pPart->handle);
#else 
					fprintf(fp,"0X%X�˼��ĶԳƹ�������0X%X�Ҳ���,��¼�������\n",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==-1)
					fprintf(fp,"0X%X�˼��ĶԳƹ�������0X%X���Ǹ˼�,��¼�������\n",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==1)
					fprintf(fp,"0X%X�˼���X��Գƹ�������0X%X��X��Գ�,��¼�������\n",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==2)
					fprintf(fp,"0X%X�˼���Y��Գƹ�������0X%X��X��Գ�,��¼�������\n",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==3)
					fprintf(fp,"0X%X�˼���Z��Գƹ�������0X%X��X��Գ�,��¼�������\n",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==4)
					fprintf(fp,"0X%X�˼�����Գƹ�������0X%X���ʲ�ͬ,�����������ʵ\n",pPart->handle,pRelaObj->hObj);
#endif
#ifdef __PART_DESIGN_INC_//defined(__TSA_)||defined(__LDS_)
				else if(bad_mir==5)
					fprintf(fp,"0X%X�˼�����Գƹ�������0X%X��Ԫ���Ͳ�ͬ,�����������ʵ\n",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==6)
					fprintf(fp,"0X%X�˼�����Գƹ�������0X%Xʼ������Լ�����Ͳ�ͬ,�����������ʵ��\n",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==7)
					fprintf(fp,"0X%X�˼�����Գƹ�������0X%X�ն�����Լ�����Ͳ�ͬ,�����������ʵ\n",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==8)
					fprintf(fp,"0X%X�˼�����Գƹ�������0X%Xʼ���������Ͳ�ͬ,�����������ʵ\n",pPart->handle,pRelaObj->hObj);
				else if(bad_mir==9)
					fprintf(fp,"0X%X�˼�����Գƹ�������0X%X�ն��������Ͳ�ͬ,�����������ʵ\n",pPart->handle,pRelaObj->hObj);
#endif
				if(bad_mir<=3)
					pPart->relativeObjs.DeleteCursor();
			}
		}
		Ta.Parts.pop_stack(pushed);
		hits++;
		DisplayProcess(hits*100/sum);
	}
	DisplayProcess(100);
	fclose(fp);
	if(bad_hits>0)
		WinExec("notepad.exe badmir.txt",SW_SHOW);
	else
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("Check by symmetrical relative information of rod node!");
#else
		MessageBox("ͨ���˼��ڵ�Գƹ�����ϢУ��!");
#endif
}

void CLDSView::OnValidateCrossrodBearcompInfo() 
{
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__TSA_)
	for(CLDSLinePart *pLinePart=Ta.Parts.GetFirstLinePart();pLinePart;pLinePart=Ta.Parts.GetNextLinePart())
	{
		if( pLinePart->layer(1)=='Z')
			continue;
		if(pLinePart->CalLenCoef.iTypeNo!=0)
			continue;	//ֻ�������ܼ��㳤ϸ�����
		BOOL pushed=Ta.Parts.push_stack();
		f3dPoint inters_pos;
		CLDSLinePart *pRefLinePart;
		if(pLinePart->CalLenCoef.hRefPole>=0x20)
		{
			pRefLinePart=(CLDSLinePart*)console.FromPartHandle(pLinePart->CalLenCoef.hRefPole);
			if(Int3dll(pLinePart->pStart->Position(false),pLinePart->pEnd->Position(false),
				pRefLinePart->pStart->Position(false),pRefLinePart->pEnd->Position(false),inters_pos)!=1)	//����Ч�����
				pLinePart->CalLenCoef.hRefPole=0;
		}
		if(pLinePart->CalLenCoef.hRefPole<=0x20)
		{
			bool bHasIntersPt=false;
			pRefLinePart=(CLDSLinePart*)pLinePart->GetMirPart(MIRMSG(1));
			if(pRefLinePart!=NULL&&
				Int3dll(pLinePart->pStart->Position(false),pLinePart->pEnd->Position(false),
				pRefLinePart->pStart->Position(false),pRefLinePart->pEnd->Position(false),inters_pos)==1)	//���ڽ���
			{
				bHasIntersPt=true;
				pLinePart->CalLenCoef.hRefPole=pRefLinePart->handle;
			}
			else
				pRefLinePart=(CLDSLinePart*)pLinePart->GetMirPart(MIRMSG(2));
			if(!bHasIntersPt&&pRefLinePart!=NULL&&
				Int3dll(pLinePart->pStart->Position(false),pLinePart->pEnd->Position(false),
				pRefLinePart->pStart->Position(false),pRefLinePart->pEnd->Position(false),inters_pos)==1)	//���ڽ���
			{
				bHasIntersPt=true;
				pLinePart->CalLenCoef.hRefPole=pRefLinePart->handle;
			}
			else
				pRefLinePart=(CLDSLinePart*)pLinePart->GetMirPart(MIRMSG(3));
			if(!bHasIntersPt&&pRefLinePart!=NULL&&
				Int3dll(pLinePart->pStart->Position(false),pLinePart->pEnd->Position(false),
				pRefLinePart->pStart->Position(false),pRefLinePart->pEnd->Position(false),inters_pos)==1)	//���ڽ���
			{
				bHasIntersPt=true;
				pLinePart->CalLenCoef.hRefPole=pRefLinePart->handle;
			}
		}
		Ta.Parts.pop_stack(pushed);
	}
#ifdef AFX_TARG_ENU_ENGLISH
    MessageBox("The information of the rod pressured in the same time for the cross-bracing members has been updated and checked!");
#else
	MessageBox("�����ͬʱ��ѹ�˼���Ϣ������֤��ϣ�");
#endif
#endif
}
#ifdef __COMMON_PART_INC_
static void AddBoltToRelaPartLsRefList(CLDSBolt *pBolt,long hRelationPart)
{
	CLDSPart *pRelationPart=(CLDSPart*)console.FromHandle(hRelationPart,TRUE);
	CLsRefList *pLsRefList=NULL;
	if (pRelationPart!=NULL)
		pLsRefList=pRelationPart->GetLsRefList();
	if(pLsRefList->FromHandle(pBolt->handle)==NULL)
		pLsRefList->append(pBolt->GetLsRef());
}
void ShadowCreate(ATOM_LIST<int> &idList,CParaCS::PARAMETRIC_INFO param,int iShadowCreateStyle,int nOffsetNum,long hRelationPart=0)
{
	BYTE mirflag[3]={0x01,0x02,0x04};
	ATOM_LIST<int> id_list;
	for(int *pId=idList.GetFirst();pId;pId=idList.GetNext())
	{
		CLDSDbObject *pObj=(CLDSDbObject*)console.FromHandle(*pId,TRUE);
		if(pObj==NULL||!pObj->IsPart())
			continue;
		SmartPartPtr pPart=(CLDSPart*)pObj;
		if( pObj->GetClassTypeId()==CLS_BOLT||pPart->IsLinePart()||//pObj->GetClassTypeId()==CLS_LINEANGLE||
			pObj->GetClassTypeId()==CLS_PLATE||pObj->GetClassTypeId()==CLS_PARAMPLATE)
		{
			if (pObj->GetClassTypeId()==CLS_PLATE||pObj->GetClassTypeId()==CLS_PARAMPLATE)
			{
				id_list.Empty();
				CLsRefList *pLsRefList=((CLDSPart*)pObj)->GetLsRefList();
				if (pLsRefList!=NULL)
				{
					for (CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
						id_list.append((pLsRef->GetLsPtr())->handle);
				}
			}
			CLDSPart *pPart=(CLDSPart*)pObj;
			if (iShadowCreateStyle==0)
			{	//�ԳƱ任����Ӱ�乹��
				for(int j=0;j<3;j++)
				{
					CLDSPart *pShadowPart=NULL;
					if(param.m_xMirInfo.axis_flag&mirflag[j])
					{
 						pShadowPart=console.AppendPart(pObj->GetClassTypeId());
						MIRMSG miritem=param.m_xMirInfo.GetSubMirItem(j+1);
						CalMirAtomLayer(pPart->layer(),pShadowPart->layer(),miritem);
						if (hRelationPart!=0&&pObj->GetClassTypeId()==CLS_BOLT)
							pShadowPart->SetShadowPara(pPart,hRelationPart,CLDSObject::SHADOW_ISOLATED_RELA_LOCATION);
						else
						{
							if(param.m_xMirInfo.origin.IsZero())
								pShadowPart->SetShadowPara(pPart,j+1);	//����X/Y/Z��Գƹ̶�idΪ1,2,3
							else
							{
								CParaCS *pAssembleCS=Ta.AppendAcs();
								CParaCS::PARAMETRIC_INFO paramcs=param;
								paramcs.m_xMirInfo=miritem;
								pAssembleCS->SetParamInfo(paramcs);
								pShadowPart->SetShadowPara(pPart,pAssembleCS->id);	//����X/Y/Z��Գƹ̶�idΪ1,2,3
							}
						}
						pShadowPart->ShadowDesign();
						pShadowPart->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pShadowPart->GetSolidPartObject());
						if ((pShadowPart->GetClassTypeId()==CLS_PLATE||pShadowPart->GetClassTypeId()==CLS_PARAMPLATE)&&id_list.GetNodeNum()>0)
						{
							CParaCS::PARAMETRIC_INFO paramcs=param;
							paramcs.m_xMirInfo.axis_flag=mirflag[j];
							ShadowCreate(id_list,paramcs,iShadowCreateStyle,nOffsetNum,pShadowPart->handle);
						}
						if (pObj->GetClassTypeId()==CLS_BOLT&&hRelationPart!=0)
							AddBoltToRelaPartLsRefList((CLDSBolt*)pShadowPart,hRelationPart);
						if(param.m_xMirInfo.axis_flag&0x08)	//����ת����
						{
							short init_angle=param.m_xMirInfo.rotate_angle;
							miritem=param.m_xMirInfo;
							miritem.axis_flag=8;
							for(int k=0;k<param.m_xMirInfo.array_num;k++)
							{
								CLDSPart *pRotatePart=console.AppendPart(pObj->GetClassTypeId());
								miritem.rotate_angle=(k+1)*init_angle;
								CParaCS *pAssembleCS=Ta.AppendAcs();
								CParaCS::PARAMETRIC_INFO paramcs=param;
								paramcs.m_xMirInfo=miritem;
								pAssembleCS->SetParamInfo(paramcs);
								CalMirAtomLayer(pShadowPart->layer(),pRotatePart->layer(),miritem);
								if (hRelationPart!=0&&pObj->GetClassTypeId()==CLS_BOLT)
									pRotatePart->SetShadowPara(pShadowPart,hRelationPart,CLDSObject::SHADOW_ISOLATED_RELA_LOCATION);
								else
									pRotatePart->SetShadowPara(pShadowPart,pAssembleCS->id);
								pRotatePart->ShadowDesign();
								pRotatePart->Create3dSolidModel();
								g_pSolidDraw->NewSolidPart(pRotatePart->GetSolidPartObject());
								if ((pRotatePart->GetClassTypeId()==CLS_PLATE||pRotatePart->GetClassTypeId()==CLS_PARAMPLATE)&&id_list.GetNodeNum()>0)
								{
									ATOM_LIST<int> idArr;
									CLsRefList *pLsRefList=pShadowPart->GetLsRefList();
									if (pLsRefList!=NULL)
									{
										for (CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
											idArr.append((pLsRef->GetLsPtr())->handle);
									}
									paramcs.m_xMirInfo.array_num=1;
									ShadowCreate(idArr,paramcs,iShadowCreateStyle,nOffsetNum,pRotatePart->handle);
								}
								if (pObj->GetClassTypeId()==CLS_BOLT&&hRelationPart!=0)
									AddBoltToRelaPartLsRefList((CLDSBolt*)pRotatePart,hRelationPart);
							}
						}
					}
				}
				//����ԭʼ��
				if(param.m_xMirInfo.axis_flag&0x08)	//����ת����
				{
					short init_angle=param.m_xMirInfo.rotate_angle;
					MIRMSG miritem=param.m_xMirInfo;
					miritem.axis_flag=8;
					for(int k=0;k<param.m_xMirInfo.array_num;k++)
					{
						CLDSPart *pRotatePart=console.AppendPart(pObj->GetClassTypeId());
						miritem.rotate_angle=(k+1)*init_angle;
						CParaCS *pAssembleCS=Ta.AppendAcs();
						CParaCS::PARAMETRIC_INFO paramcs=param;
						paramcs.m_xMirInfo=miritem;
						pAssembleCS->SetParamInfo(paramcs);
						CalMirAtomLayer(pPart->layer(),pRotatePart->layer(),miritem);
						if(hRelationPart!=0&&pObj->GetClassTypeId()==CLS_BOLT)
							pRotatePart->SetShadowPara(pPart,hRelationPart,CLDSObject::SHADOW_ISOLATED_RELA_LOCATION);
						else
							pRotatePart->SetShadowPara(pPart,pAssembleCS->id);
						pRotatePart->ShadowDesign();
						pRotatePart->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pRotatePart->GetSolidPartObject());
						if ((pRotatePart->GetClassTypeId()==CLS_PLATE||pRotatePart->GetClassTypeId()==CLS_PARAMPLATE)&&id_list.GetNodeNum()>0)
						{
							paramcs.m_xMirInfo.array_num=1;
							ShadowCreate(id_list,paramcs,iShadowCreateStyle,nOffsetNum,pRotatePart->handle);
						}
						if (pObj->GetClassTypeId()==CLS_BOLT&&hRelationPart!=0)
							AddBoltToRelaPartLsRefList((CLDSBolt*)pRotatePart,hRelationPart);
					}
				}
			}
			else if (iShadowCreateStyle==1)
			{	//����ƽ������Ӱ�乹��
				double init_offset=param.m_xMirInfo.origin.x;
				for (int j=1;j<=nOffsetNum;j++)
				{
					CLDSPart *pShadowPart=NULL;
 					pShadowPart=console.AppendPart(pObj->GetClassTypeId());
					CParaCS *pAssembleCS=Ta.AppendAcs();
					CParaCS::PARAMETRIC_INFO paramcs=pAssembleCS->GetParamInfo();
					paramcs.m_cDefStyle=param.m_cDefStyle;
					paramcs.m_xMirInfo=param.m_xMirInfo;
					paramcs.desAxisX=param.desAxisX;
					paramcs.m_xMirInfo.origin.x=j*init_offset;	//origin.xΪƫ����
					pAssembleCS->SetParamInfo(paramcs);
					CalMirAtomLayer(pPart->layer(),pShadowPart->layer(),param.m_xMirInfo);
					if(hRelationPart!=0&&pObj->GetClassTypeId()==CLS_BOLT)
						pShadowPart->SetShadowPara(pPart,hRelationPart,CLDSObject::SHADOW_ISOLATED_RELA_LOCATION);
					else
						pShadowPart->SetShadowPara(pPart,pAssembleCS->id);
					pShadowPart->ShadowDesign();
					pShadowPart->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pShadowPart->GetSolidPartObject());
					if ((pShadowPart->GetClassTypeId()==CLS_PLATE||pShadowPart->GetClassTypeId()==CLS_PARAMPLATE)&&id_list.GetNodeNum()>0)
						ShadowCreate(id_list,param,iShadowCreateStyle,1,pShadowPart->handle);
					if (pObj->GetClassTypeId()==CLS_BOLT&&hRelationPart!=0)
						AddBoltToRelaPartLsRefList((CLDSBolt*)pShadowPart,hRelationPart);
				}
			}
			else if (iShadowCreateStyle==2)
			{	//ָ����λ�Զ��������װ������ϵ����Ӱ�乹��
				CLDSPart *pShadowPart=NULL;
				pShadowPart=console.AppendPart(pObj->GetClassTypeId());
				CParaCS *pAssembleCS=Ta.AppendAcs();
				pAssembleCS->SetParamInfo(param);
				pShadowPart->SetLayer(pPart->layer());
				if(hRelationPart!=0&&pObj->GetClassTypeId()==CLS_BOLT)
					pShadowPart->SetShadowPara(pPart,hRelationPart,CLDSObject::SHADOW_ISOLATED_RELA_LOCATION);
				else
					pShadowPart->SetShadowPara(pPart,pAssembleCS->id);
				pShadowPart->ShadowDesign();
				pShadowPart->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pShadowPart->GetSolidPartObject());
				if ((pShadowPart->GetClassTypeId()==CLS_PLATE||pShadowPart->GetClassTypeId()==CLS_PARAMPLATE)&&id_list.GetNodeNum()>0)
					ShadowCreate(id_list,param,iShadowCreateStyle,nOffsetNum,pShadowPart->handle);
				if (pObj->GetClassTypeId()==CLS_BOLT&&hRelationPart!=0)
					AddBoltToRelaPartLsRefList((CLDSBolt*)pShadowPart,hRelationPart);
			}
			else
			{	//ָ���澵��Գ�����
				CLDSPart *pShadowPart=NULL;
				pShadowPart=console.AppendPart(pObj->GetClassTypeId());
				CParaCS *pAssembleCS=Ta.AppendAcs();
				pAssembleCS->SetParamInfo(param);
				CalMirAtomLayer(pPart->layer(),pShadowPart->layer(),param.m_xMirInfo);
				if(hRelationPart!=0&&pObj->GetClassTypeId()==CLS_BOLT)
					pShadowPart->SetShadowPara(pPart,hRelationPart,CLDSObject::SHADOW_ISOLATED_RELA_LOCATION);
				else
					pShadowPart->SetShadowPara(pPart,pAssembleCS->id);
				pShadowPart->ShadowDesign();
				pShadowPart->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pShadowPart->GetSolidPartObject());
				if ((pShadowPart->GetClassTypeId()==CLS_PLATE||pShadowPart->GetClassTypeId()==CLS_PARAMPLATE)&&id_list.GetNodeNum()>0)
					ShadowCreate(id_list,param,iShadowCreateStyle,nOffsetNum,pShadowPart->handle);
				if (pObj->GetClassTypeId()==CLS_BOLT&&hRelationPart!=0)
					AddBoltToRelaPartLsRefList((CLDSBolt*)pShadowPart,hRelationPart);
			}
		}
	}
}
#endif
void CLDSView::OnShadowCreate()
{
#ifdef __COMMON_PART_INC_
	if(!PRODUCT_FUNC::IsHasShadowObject())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Current authorization does not support the maping object!");
#else
		AfxMessageBox("��ǰ��Ȩ��֧��Ӱ�����!");
#endif
		return;
	}
 	long *id_arr=NULL;
	int i=0,n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(n<=0)
		return;
	ATOM_LIST<int> idList;
	PARTSET plateSet;
	//��ȡ������˨���õĸְ�
	for (i=0;i<n;i++)
	{
		CLDSDbObject *pObj=(CLDSDbObject*)console.FromHandle(id_arr[i],TRUE);
		if(pObj==NULL)
			continue;
		if (pObj->GetClassTypeId()==CLS_PLATE||pObj->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLsRefList *pLsRefList=((CLDSPart*)pObj)->GetLsRefList();
			if (pLsRefList!=NULL)
				plateSet.append((CLDSPart*)pObj);
		}
	}
	//���˸ְ��������˨
	for (i=0;i<n;i++)
	{
		CLDSDbObject *pObj=(CLDSDbObject*)console.FromHandle(id_arr[i],TRUE);
		if (pObj == NULL)
			continue;
		if (pObj->GetClassTypeId()!=CLS_BOLT)
			idList.append(id_arr[i]);
		else
		{
			if (plateSet.GetNodeNum()>0)
			{
				CLDSPart *pPart;
				for(pPart=plateSet.GetFirst();pPart;pPart=plateSet.GetNext())
				{
					CLsRefList *pLsRefList=pPart->GetLsRefList();
					CLsRef *pLsRef;
					for (pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
					{
						if (id_arr[i]==(pLsRef->GetLsPtr())->handle)
							break;
					}
					if (pLsRef!=NULL)
						break;
				}
				if (pPart==NULL)
					idList.append(id_arr[i]);
			}
			else
				idList.append(id_arr[i]);
		}
	}
	CDesignShadowDlg designdlg;
	if(designdlg.DoModal()!=IDOK)
		return;
	CUndoOperObject undo(&Ta,true);
	CParaCS::PARAMETRIC_INFO param=designdlg.des_cs.GetParamInfo();
	ShadowCreate(idList,param,designdlg.m_iShadowCreateStyle,designdlg.m_nOffsetNum);
	g_pSolidDraw->Draw();
#endif
}

#if defined(__TSA_)|| defined(__TSA_FILE_)
void CLDSView::OnDesignTubeTowerSlope() {;}
void CLDSView::OnDesignTubeTowerFoot(){;}
#endif

void CLDSView::OnReplaceBoltDatumpoint() 
{
#ifndef __TSA_
	m_nPrevCommandID=ID_REPLACE_BOLT_DATUMPOINT;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Change the datum position of bolt";
#else
	m_sPrevCommandName="�任��˨����";
#endif
	Command("ReplaceBoltDP");
#endif
}

void CLDSView::OnAlignBoltAlongLenvec()
{
#ifndef __TSA_
	m_nPrevCommandID=ID_ALIGN_BOLT_ALONG_LENVEC;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Align bolts along longitude";
#else
	m_sPrevCommandName="���������˨";
#endif
	Command("AlignBolts");
#endif
}
//<DEVELOP_PROCESS_MARK nameId="CLDSView.OnBoltPunchHole">
static BOOL InitPolygonPoints(ATOM_LIST<PROFILE_VERTEX> &vertex_list,ATOM_LIST<f3dPoint> &point_list)
{
	if(vertex_list.GetNodeNum()<3)
		return FALSE;
	f3dPoint vertex,center,offset_vec;
	double sector_angle=0.0,radius=0.0,angle=0.0;
	int nSlices;
	PROFILE_VERTEX* pPreVertex=vertex_list.GetTail();
	for(PROFILE_VERTEX* pCurVertex=vertex_list.GetFirst();pCurVertex;pCurVertex=vertex_list.GetNext())
	{
		BOOL bRepeat=FALSE;
		vertex=pPreVertex->vertex;
		for(f3dPoint* pPt=point_list.GetFirst();pPt;pPt=point_list.GetNext())
		{	//ȥ���ظ��Ķ���
			if(vertex.IsEqual(*pPt,eps2))
			{
				bRepeat=TRUE;
				break;
			}
		}
		if(!bRepeat)
			point_list.append(vertex);
		int sign=1;
		int index=vertex_list.GetCurrentIndex();
		if(pPreVertex->type==1)
		{	//ָ��Բ���Ƕ�
			if(pPreVertex->sector_angle<0)
				sign=-1;
			sector_angle=fabs(pPreVertex->sector_angle);
			radius=(DISTANCE(vertex,pCurVertex->vertex)*0.5)/sin(0.5*sector_angle);
			nSlices = CalArcResolution(radius,sector_angle,1.0,5.0,18);
			offset_vec=pCurVertex->vertex-vertex;
			normalize(offset_vec);
			angle=(Pi-sector_angle)*0.5;
			RotateVectorAroundVector(offset_vec,angle,f3dPoint(0,0,sign));
			center=pPreVertex->vertex+offset_vec*radius;
			center.z=0;
			f3dLine axis_line;
			axis_line.startPt.Set(center.x,center.y,0);
			axis_line.endPt.Set(center.x,center.y,sign);
			for(int i=1;i<nSlices;i++)
			{
				f3dPoint nextPt=vertex;
				rotate_point_around_axis(nextPt,sector_angle/nSlices,axis_line);
				point_list.append(nextPt);
				vertex=nextPt;
			}
		}
		else if(pPreVertex->type==2 || pPreVertex->type==3)
		{	//ָ���뾶
			if(pPreVertex->radius<0)
				sign=-1;
			radius=fabs(pPreVertex->radius);
			sector_angle=2*ACOS(DISTANCE(vertex,pCurVertex->vertex)*0.5/radius);
			nSlices = CalArcResolution(radius,sector_angle,1.0,5.0,18);
			offset_vec=pCurVertex->vertex-vertex;
			normalize(offset_vec);
			angle=(Pi-sector_angle)*0.5;
			RotateVectorAroundVector(offset_vec,angle,f3dPoint(0,0,sign));
			center=pPreVertex->vertex+offset_vec*radius;
			center.z=0;
			f3dLine axis_line;
			axis_line.startPt.Set(center.x,center.y,0);
			axis_line.endPt.Set(center.x,center.y,sign);
			for(int i=1;i<nSlices;i++)
			{
				f3dPoint nextPt=vertex;
				rotate_point_around_axis(nextPt,sector_angle/nSlices,axis_line);
				point_list.append(nextPt);
				vertex=nextPt;
			}
		}
		pPreVertex=pCurVertex;
	}
	return TRUE;
}
static void FindPartSetByBolt(CLDSBolt* pBolt,PARTSET& partset,int distScopeTolerFrom = -2, int distScopeTolerTo=2)
{
	if(pBolt==NULL)
		return;
	for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_BOLT)	//����������
			continue;
		if(!pPart->cfgword.And(pBolt->cfgword))	//����ͬһģ����
			continue;	
		if(pPart->GetLsRefList()==NULL||pPart->GetLsRefList()->FromHandle(pBolt->handle))	//���д���˨����
			continue;
		f3dPoint inters,bolt_norm,face_pick,face_norm;
		bolt_norm=pBolt->ucs.axis_z;
		if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSParamPlate* pParamPlate=(CLDSParamPlate*)pPart;
			//��˨�������������ķ����Ƿ���
			face_pick=pParamPlate->ucs.origin;
			face_norm=pParamPlate->ucs.axis_z;
			double dd=fabs(bolt_norm*face_norm);
			if(dd<EPS_COS2)
				continue;
			//��ȡ��˨�ڲ��������ϵ�ͶӰ�㣬�ж�ͶӰ���Ƿ��ڲ����������Ч������
			if(Int3dlf(inters,pBolt->ucs.origin,bolt_norm,face_pick,face_norm)!=1)
				continue;
			ATOM_LIST<f3dPoint> point_list;
			if(!InitPolygonPoints(pParamPlate->vertex_list,point_list))
				continue;
			int nPtNum=point_list.GetNodeNum();
			f3dPoint *pointArr= new f3dPoint[nPtNum];
			int i=0;
			for(f3dPoint* pPt=point_list.GetFirst();pPt;pPt=point_list.GetNext())
			{	
				f3dPoint vertex=*pPt;
				coord_trans(vertex,pParamPlate->ucs,TRUE);
				pointArr[i++]=vertex;
			}
			fPolygonRgn polyRgn;
			if(polyRgn.CreatePolygonRgn(pointArr,nPtNum))
				continue;
			if(polyRgn.PtInRgn(inters)!=1)
				continue;
			//������˨ԭ�㵽ͶӰ��ľ��룬����˨����Ч���Ƚ��бȽϣ��ж��Ƿ����
			f3dPoint vec=inters-pBolt->ucs.origin;
			normalize(vec);
			double fDistToPlane=DISTANCE(pBolt->ucs.origin,inters);
			if(vec*bolt_norm<0 && fDistToPlane>distScopeTolerFrom)
				continue;
			else if(vec*bolt_norm>0 && bolt_norm*face_norm>0 && fDistToPlane-pBolt->get_L()>distScopeTolerTo)
				continue;
			else if(vec*bolt_norm>0 && bolt_norm*face_norm<0 && fDistToPlane-pParamPlate->GetThick()-pBolt->get_L()>distScopeTolerTo)
				continue;
		}
		else if (pPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate* pPlate=(CLDSPlate*)pPart;
			double dd1=0,dd2=0,dd3=0;
			dd1=fabs(bolt_norm*pPlate->ucs.axis_z);
			if(pPlate->face_N>=2)
				dd2=fabs(bolt_norm*pPlate->huoqufacenorm[0]);
			if(pPlate->face_N>=3)
				dd3=fabs(bolt_norm*pPlate->huoqufacenorm[1]);
			//�ж���˨�������ķ����Ƿ���
			int iFace=0;
			if(dd1>dd2 && dd1>dd3 && dd1>EPS_COS2)
			{
				face_norm=pPlate->ucs.axis_z;
				face_pick=pPlate->ucs.origin;
				iFace=1;
			}
			else if(dd2>dd1 && dd2>dd3 && dd2>EPS_COS2)
			{
				face_norm=pPlate->huoqufacenorm[0];
				face_pick=pPlate->HuoQuLine[0].startPt;
				coord_trans(face_pick,pPlate->ucs,TRUE);
				iFace=2;
			}
			else if(dd3>dd1 && dd3>dd2 && dd3>EPS_COS2)
			{
				face_norm=pPlate->huoqufacenorm[1];
				face_pick=pPlate->HuoQuLine[1].startPt;
				coord_trans(face_pick,pPlate->ucs,TRUE);
				iFace=3;
			}
			else
				continue;
			//����˨�ڰ��ϵ�ͶӰ�㣬�ж�ͶӰ���Ƿ��ڰ����Ч������
			if(Int3dlf(inters,pBolt->ucs.origin,bolt_norm,face_pick,face_norm)!=1)
				continue;
			fPolygonRgn polyRgn;
			ATOM_LIST<f3dPoint> point_list;
			if(pPlate->face_N==1)
			{	//�����
				if(!InitPolygonPoints(pPlate->vertex_list,point_list))
					continue;
			}
			else 
			{	//�����(һ�㲻����Բ��)
				for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
				{
					f3dPoint vertex=pPlate->GetRealVertex(pVertex->vertex);
					if(iFace==1 && (pVertex->vertex.feature==1 || pVertex->vertex.feature==12 || pVertex->vertex.feature==13))
						point_list.append(vertex);
					else if(iFace==2&&(pVertex->vertex.feature==2 || pVertex->vertex.feature==12))
						point_list.append(vertex);
					else if(iFace==3 &&(pVertex->vertex.feature==3 || pVertex->vertex.feature==13))
						point_list.append(vertex);
				}
			}
			int nPtNum=point_list.GetNodeNum();
			f3dPoint *pPoints=new f3dPoint[nPtNum];
			int i=0;
			for(f3dPoint* pPt=point_list.GetFirst();pPt;pPt=point_list.GetNext())
			{
				f3dPoint vertex=*pPt;
				coord_trans(vertex,pPlate->ucs,TRUE);
				pPoints[i++]=vertex;
			}
			if(!polyRgn.CreatePolygonRgn(pPoints,nPtNum))
				continue;
			if(polyRgn.PtInRgn(inters)!=1)
				continue;
			//������˨ԭ�㵽ͶӰ��ľ��룬����˨����Ч���Ƚ��бȽϣ��ж��Ƿ����
			f3dPoint vec=inters-pBolt->ucs.origin;
			normalize(vec);
			double fDistToPlane=DISTANCE(pBolt->ucs.origin,inters);
			if(vec*bolt_norm<0 && fDistToPlane>distScopeTolerFrom)
				continue;
			else if(vec*bolt_norm>0 && bolt_norm*face_norm>0 && fDistToPlane-pBolt->get_L()>distScopeTolerTo)
				continue;
			else if(vec*bolt_norm>0 && bolt_norm*face_norm<0 && fDistToPlane-pPlate->GetThick()-pBolt->get_L()>distScopeTolerFrom)
				continue;
		}
		else if(pPart->IsAngle())
		{
			CLDSLineAngle* pAngle=(CLDSLineAngle*)pPart;
			int iResideWingX0Y1=0;
			f3dPoint xIntersPoint;
			if(!pAngle->IsLsInWing(pBolt,&iResideWingX0Y1,false,xIntersPoint))
				continue;
			GEPOINT xLenVec = xIntersPoint - pBolt->ucs.origin;
			double dfLenPos = xLenVec * pBolt->ucs.axis_z;
			if (dfLenPos<-distScopeTolerFrom || dfLenPos - pBolt->L>distScopeTolerTo)
				continue;

			//�ж���˨������Ǹֵ�֫�����Ƿ���
			/*BOOL bInWingX=FALSE;
			double dd1=bolt_norm*pAngle->get_norm_x_wing();
			double dd2=bolt_norm*pAngle->get_norm_y_wing();
			if(fabs(dd1)>fabs(dd2) && fabs(dd1)>EPS_COS2)	//X֫
				bInWingX=TRUE;
			else if(fabs(dd2)>fabs(dd1) && fabs(dd2)>EPS_COS2)	//Y֫
				bInWingX=FALSE;
			else
				continue;
			//����˨�ڽǸ��ϵ�ͶӰ�㣬�ж�ͶӰ���Ƿ��ڽǸֵ���Ч֫����
			face_pick=pAngle->Start();
			f3dPoint wing_vec;
			if(bInWingX)
			{
				wing_vec=pAngle->GetWingVecX();
				face_norm=pAngle->get_norm_x_wing();
			}
			else
			{
				wing_vec=pAngle->GetWingVecY();
				face_norm=pAngle->get_norm_y_wing();
			}
			if(Int3dlf(inters,pBolt->ucs.origin,bolt_norm,face_pick,face_norm)!=1)
				continue;
			fPolygonRgn polyRgn;
			f3dPoint pointArr[4];
			pointArr[0]=pAngle->Start();
			pointArr[1]=pAngle->Start()+wing_vec*pAngle->GetWidth();
			pointArr[2]=pAngle->End()+wing_vec*pAngle->GetWidth();
			pointArr[3]=pAngle->End();
			if(!polyRgn.CreatePolygonRgn(pointArr,4))
				continue;
			if(polyRgn.PtInRgn(inters)!=1)
				continue;
			//������˨ԭ�㵽ͶӰ��ľ��룬����˨����Ч���Ƚ��бȽϣ��ж��Ƿ����
			f3dPoint vec=inters-pBolt->ucs.origin;
			normalize(vec);
			double fDistToPlane=DISTANCE(pBolt->ucs.origin,inters);
			if(vec*bolt_norm<0 && fDistToPlane>dist)
				continue;
			else if(vec*bolt_norm>0 && bolt_norm*face_norm>0 && fDistToPlane-pAngle->GetThick()-pBolt->get_L()>dist)
				continue;
			else if(vec*bolt_norm>0 && bolt_norm*face_norm<0 && fDistToPlane-pBolt->get_L()>dist)
				continue;*/
		}
		else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
		{
			CLDSLineFlat *pLineFlat=(CLDSLineFlat*)pPart;
			pLineFlat->BuildUCS();
			face_pick = pLineFlat->ucs.origin;
			face_norm = pLineFlat->WorkPlaneNorm();
			double dd1=fabs(face_norm*bolt_norm);
			if(dd1<EPS_COS2)
				continue;
			//����˨�ڱ����ϵ�ͶӰ�㣬�ж�ͶӰ���Ƿ��ڱ�������Ч������
			if(Int3dlf(inters,pBolt->ucs.origin,bolt_norm,face_pick,face_norm)!=1)
				continue;
			fPolygonRgn polyRgn;
			f3dPoint pointArr[4];
			pointArr[0]=pLineFlat->xPosStart-pLineFlat->ucs.axis_y*pLineFlat->GetWidth()*0.5;
			pointArr[1]=pLineFlat->xPosStart+pLineFlat->ucs.axis_y*pLineFlat->GetWidth()*0.5;
			pointArr[2]=pLineFlat->xPosEnd+pLineFlat->ucs.axis_y*pLineFlat->GetWidth()*0.5;
			pointArr[3]=pLineFlat->xPosEnd-pLineFlat->ucs.axis_y*pLineFlat->GetWidth()*0.5;
			if(polyRgn.CreatePolygonRgn(pointArr,4))
				continue;
			if(polyRgn.PtInRgn(inters)!=1)
				continue;
			//������˨ԭ�㵽ͶӰ��ľ��룬����˨����Ч���Ƚ��бȽϣ��ж��Ƿ����
			f3dPoint vec=inters-pBolt->ucs.origin;
			normalize(vec);
			double fDistToPlane=DISTANCE(pBolt->ucs.origin,inters);
			if(vec*bolt_norm<0 && fDistToPlane>distScopeTolerFrom)
				continue;
			else if(vec*bolt_norm>0 && bolt_norm*face_norm>0 && fDistToPlane-pBolt->get_L()>distScopeTolerTo)
				continue;
			else if(vec*bolt_norm>0 && bolt_norm*face_norm<0 && fDistToPlane-pLineFlat->GetThick()-pBolt->get_L()>distScopeTolerTo)
				continue;
		}
		//else if(pPart->GetClassTypeId()==CLS_LINESLOT)
		//{	//�ݲ�����
		//}
		else
			continue;
		partset.append(pPart);
	}
}
void CLDSView::OnBoltPunchHole()
{
#ifndef __TSA_
	long* id_arr=NULL;
	long nn=g_pSolidSnap->GetLastSelectEnts(id_arr);
	DBOBJSET objset;
	long hits=0,nSelPartCount=BuildSelDbObjSet(objset,id_arr,nn);
	CHashList<CLDSPart*> hashParts;
	logerr.ClearContents();
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CUndoOperObject undo(&Ta,true);
	SmartPartPtr pSelPart;
	SmartPartPtr pOperBolt = (CLDSPart*)objset.GetFirst();
	static int distScopeFrom = -10, distScopeTo = 10;
	if (nSelPartCount ==1 &&pOperBolt->IsBolt())
	{
		CString cmdStr;
		//#ifdef AFX_TARG_ENU_ENGLISH
		//	cmdStr.Format("please choose bending nodes first:");
		//#else
		cmdStr.Format("�����ε�ѡҪ���׵Ĺ�����ESC��ֹ)�������봩�׷�Χ������˨���巽�򣩻س��Զ�ʶ�𴩿׹������� <%d,%d>:", distScopeFrom, distScopeTo);
		//#endif
		DWORD dwhObj = 0, dwExportFlag = 0;
		CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE, GetSingleWord(SELECTINDEX_LINEANGLE));
		verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE, GetSingleWord(SELECTINDEX_LINEFLAT));
		verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE, GetSingleWord(SELECTINDEX_PLATE));
		verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE, GetSingleWord(SELECTINDEX_PARAMPLATE));
		while (!pSelPart.blValid)
		{
			pCmdLine->FillCmdLine(cmdStr, "");
			if (g_pSolidSnap->SnapObject(&dwhObj, &dwExportFlag, &verifier) < 0)
			{
				pCmdLine->CancelCmdLine();
				return;
			}
			SELOBJ obj(dwhObj, dwExportFlag);
			if(obj.hRelaObj==0 && obj.ciTriggerType == SELOBJ::TRIGGER_KEYRETURN)
			{
				CString cmdValueStr;
				pCmdLine->PeekStrFromCmdLine(cmdValueStr);
				CXhChar50 xarrWords[2];
				UINT count=DELIMITER_STR::ParseWordsFromStr(cmdValueStr,", ",xarrWords,2);
				if (count == 2)
				{
					distScopeFrom = atoi(xarrWords[0]);
					distScopeTo = atoi(xarrWords[1]);
					break;	//���س���ո�����
				}
			}
			pSelPart = console.FromPartHandle(obj.hRelaObj);
			if (pSelPart.blValid)
			{
				g_pSolidDraw->SetEntSnapStatus(pSelPart->handle);
				pCmdLine->FinishCmdLine(CXhChar16("����0X%X", pSelPart->handle));
				if (AppendBoltRefToPart(pSelPart, pOperBolt.pBolt))	//����˨���뵽����������
				{
					hashParts.Add(pSelPart->handle);
					hits++;
				}
			}
		}
	}
	//for(int i=0;i<nn;i++)
	for(pOperBolt = (CLDSPart*)objset.GetFirst();pOperBolt.blValid;pOperBolt=(CLDSPart*)objset.GetNext())
	{
		//if(pBolt==NULL)
		if (!pOperBolt->IsBolt())
			continue;
		CLDSBolt* pBolt = pOperBolt.pBolt;// (CLDSBolt*)console.FromPartHandle(id_arr[i], CLS_BOLT);
		PARTSET partset;
		FindPartSetByBolt(pBolt,partset,distScopeFrom,distScopeTo);	//������˨�����Ĺ�������
		if(partset.GetNodeNum()==0)
			continue;
		for(CLDSPart* pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			if(pPart==NULL ||pPart->GetClassTypeId()==CLS_BOLT)
				continue;
			if(AppendBoltRefToPart(pPart,pBolt))	//����˨���뵽����������
			{
				hashParts.Add(pPart->handle);
				hits++;
			}
		}
	}
	if(hits>0)
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox(CXhChar50("%d parts have gotten %d bolt perforation!",hashParts.GetNodeNum(),hits));
	else
		AfxMessageBox("There are not suitable parts for bolt perforation!");
#else
		MessageBox(CXhChar50("%d���������%d����˨����!",hashParts.GetNodeNum(),hits));
	else
		AfxMessageBox("�޺��ʹ���������˨����!");
#endif
	if(logerr.IsHasContents())
		logerr.ShowToScreen();
#endif
}
//</DEVELOP_PROCESS_MARK>
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
// From ToolKit.cpp
void CreateOffsetLsSimplified(BOLTSET &boltSet,double lenoffset,double wingOffset,int normOffset);
#endif
void CLDSView::OnMoveBoltSet() 
{
#ifndef __TSA_
	long *id_arr=NULL;
	long nn=g_pSolidSnap->GetLastSelectEnts(id_arr);
	long hBasePart=0;
	CMoveBoltDlg dlg;
	dlg.m_bDisplayNewBoltCheckBox=true;
	if(dlg.DoModal()==IDOK)
	{
		if(dlg.IsCreateNewBolt())
		{
			BOLTSET boltSet;
			for(int i=0;i<nn;i++)
			{
				CLDSBolt *pBolt=(CLDSBolt*)console.FromPartHandle(id_arr[i],CLS_BOLT);
				if(pBolt)
					boltSet.append(pBolt);
			}
			CreateOffsetLsSimplified(boltSet,dlg.m_fLenOffsetDist,dlg.m_fWingOffsetDist,dlg.m_nNormOffset);
			return;
		}
		for(int i=0;i<nn;i++)
		{
			CLDSBolt *pBolt=(CLDSBolt*)console.FromPartHandle(id_arr[i],CLS_BOLT);
			if(pBolt)
			{
				pBolt->des_base_pos.len_offset_dist+=dlg.m_fLenOffsetDist;
				pBolt->des_base_pos.wing_offset_dist+=dlg.m_fWingOffsetDist;
				//��Ϊ������˨λ�õ�֫��ƫ��ʱ����Ҫ��֫��ƫ�Ʒ�ʽ����Ϊ4.�Զ��壬������֫��ƫ������������ wxc 17-04-20
				if(fabs(dlg.m_fWingOffsetDist)>0)
					pBolt->des_base_pos.m_biWingOffsetStyle=4;
				if(abs(dlg.m_nNormOffset)>0)
					pBolt->des_base_pos.norm_offset.AddThick(dlg.m_nNormOffset);
				pBolt->correct_pos();
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			}
		}
		g_pSolidDraw->Draw();
	}
#endif	
}

bool CLDSView::IsNeedReverseBoltDirection(CLDSBolt* pBolt,CLDSPart* pDatumPart/*=NULL*/)
{
	if(pBolt->m_bVirtualPart||pBolt->IsFootNail())
		return false;
	CLDSLineAngle* pDatumAngle=(CLDSLineAngle*)Ta.FromPartHandle(pBolt->des_base_pos.hPart,CLS_LINEANGLE);
	if(pDatumAngle!=NULL&&pDatumAngle->group_father_jg_h>0x20&&pDatumAngle->Layer(0)!='T'&&pDatumAngle->Layer(1)=='Z')
	{
		char arrQuadSymbol[4]={'D','A','C','B'};
		char ciPostQuad=pDatumAngle->Layer(2)-'1';
		GECS cs;
		cs.axis_z=pDatumAngle->End()-pDatumAngle->Start();
		normalize(cs.axis_z);
		if(cs.axis_z.z>0)
			cs.axis_z*=-1.0;
		Int3dlf(cs.origin,pDatumAngle->Start(),cs.axis_z,pBolt->ucs.origin,cs.axis_z);
		cs.axis_x=GEPOINT(pBolt->ucs.axis_z)-cs.axis_z*(cs.axis_z*pBolt->ucs.axis_z);
		cs.axis_y=cs.axis_z^cs.axis_x;
		GEPOINT distvec=GEPOINT(pBolt->ucs.origin)-cs.origin;
		return distvec*cs.axis_y<0;	//�ൺ��������ΰ˵��ҵ��Ҫ��֤������ϽǸ��ϵ���˨����˳ʱ�뷽�򴩲壨�������¿����Ľ����ڣ� wjh-2018.1.15
	}
	if(fabs(pBolt->ucs.axis_z.z)<0.15)	//�ӽ�ˮƽ������˨
	{	//ˮƽ������������
		if (pBolt->Layer(2)=='Q'&&fabs(pBolt->ucs.axis_z.y)>0.5)
			return pBolt->ucs.axis_z.y<0;
		else if (pBolt->Layer(2)=='H'&&fabs(pBolt->ucs.axis_z.y)>0.5)
			return pBolt->ucs.axis_z.y>0;
		else if (pBolt->Layer(2)=='Z'&&fabs(pBolt->ucs.axis_z.x)>0.5)
			return pBolt->ucs.axis_z.x>0;
		else if (pBolt->Layer(2)=='Y'&&fabs(pBolt->ucs.axis_z.x)>0.5)
			return pBolt->ucs.axis_z.x<0;
		else if (pDatumAngle!=NULL&&pDatumAngle->chLayer2nd=='Z')
		{	//���ģ�����ͷ�Ҹ����ģ�����˨��һ�ɴ�������� wjh-2019.9.18
			double jdx=pBolt->ucs.axis_z*pDatumAngle->vxWingNorm;
			double jdy=pBolt->ucs.axis_z*pDatumAngle->vyWingNorm;
			if (fabs(jdx)>fabs(jdy)&&jdx<0)
				return true;
			else if (fabs(jdx)<fabs(jdy)&&jdy<0)
				return true;
		}
		else if(fabs(pBolt->ucs.axis_z.y)<EPS2)	//X���򴩲��ˮƽ��˨
			return pBolt->ucs.origin.x*pBolt->ucs.axis_z.x<0;
		else if(fabs(pBolt->ucs.axis_z.x)<EPS2)	//Y���򴩲��ˮƽ��˨
			return pBolt->ucs.origin.y*pBolt->ucs.axis_z.y<0;
	}
	else if (pBolt->ucs.axis_z.z>=0.15)
		return true;	//�ǳ�����˨��һ����ֱ���򴩲�ʱ��һ���������ϴ� wjh-2019.9.18
	return false;
}
bool CLDSView::ReverseBoltDirection(CLDSBolt* pBolt,CLDSModule* pModule/*=NULL*/)
{
	if(!pBolt->des_work_norm.ReverseDirection(false))
		return false;
	//������˨���߷���
	pBolt->correct_worknorm();
	//���µ�����˨����ƫ��
	PARTSET partset;
	GetBoltActiveRelaPartSet(pBolt,partset,pModule);
	pBolt->des_base_pos.norm_offset.key_str=CLDSBolt::EmendZOffsetStr(pBolt,partset);
	//���¼�����˨λ��
	pBolt->correct_pos();
	pBolt->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	return true;
}
void CLDSView::OnUnifyBoltNorm()
{
#ifdef __ANGLE_PART_INC_
	m_nPrevCommandID=ID_UNIFY_BOLT_NORM;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat <Unify Bolts' Norm>";
#else 
	m_sPrevCommandName="�ظ�<ͳһ��˨����>";
#endif
	//CUniformNormDLg updateBoltNormDlg;
	//if(updateBoltNormDlg.DoModal()!=IDOK)
	//	return;
	try{
		CLDSModule* pModule=Ta.GetActiveModule();
		CUndoOperObject undo(&Ta);

		for(CLDSBolt* pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
		{
			if(IsNeedReverseBoltDirection(pBolt))
				ReverseBoltDirection(pBolt,pModule);
		}
		/*
		long *id_arr=NULL;
		long nn=g_pSolidSnap->GetLastSelectEnts(id_arr);
		for(int i=0;i<nn;i++)
		{
			CLDSBolt* pBolt=(CLDSBolt*)console.FromPartHandle(id_arr[i],CLS_BOLT);
			if(pBolt==NULL || pBolt->ucs.axis_z*updateBoltNormDlg.norm>=0)
				continue;	//����Ҫ��˨����
			if(!pBolt->des_work_norm.ReverseDirection(false))
				continue;
			//������˨���߷���
			pBolt->correct_worknorm();
			//���µ�����˨����ƫ��
			PARTSET partset;
			GetBoltActiveRelaPartSet(pBolt,partset,pModule);
			pBolt->des_base_pos.norm_offset.key_str=CLDSBolt::EmendZOffsetStr(pBolt,partset);
			//���¼�����˨λ��
			pBolt->correct_pos();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
		*/

		g_pSolidDraw->Draw();
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
#endif
}
void CLDSView::OnPlateProfileToRect() 
{
#ifndef __TSA_
	long *id_arr=NULL;
	long nn=g_pSolidSnap->GetLastSelectEnts(id_arr);
	bool bModifyPlateProfile=false;
	CUndoOperObject undo(&Ta);
	for(int i=0;i<nn;i++)
	{
		CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(id_arr[i],CLS_PLATE);
		if(pPlate)
		{
			pPlate->CalStdProfile();
			SOLIDMODEL_DISPLAY_PARA display=GetCurrSolidModelDisplayPara();
			pPlate->designInfo.iProfileStyle0123=3;
			pPlate->Create3dSolidModel(display.bDisplayHole,display.scale_of_user2scr,display.sample_len,display.smoothness);
			g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
			CLDSView::RedrawPlateProfileDrawing(pPlate);
			//UpdatePropertyPage();		//���ô˺����ᵼ��id_arr���ݱ仯,�˴�����Ҫ����������
			bModifyPlateProfile=true;
		}
	}
	if(bModifyPlateProfile)
		g_pSolidDraw->Draw();
#endif	
}
void CLDSView::OnCopyPartSet() 
{
	long* id_arr,n;
	CLDSNode* pNode=NULL;
	n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	CLDSPart* pPart=NULL;
	console.clipper_board.ClearBuffer();
	if(n==0)
		return;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();

	if((pNode=Ta.Node.FromHandle(id_arr[0]))!=NULL)
	{
		NODESET nodeset;
		nodeset.append(pNode);
		for(int i=1;i<n;i++)
		{
			if((pNode=Ta.Node.FromHandle(id_arr[i]))!=NULL)
				nodeset.append(pNode);
		}
		CopyNodes(nodeset);//CopyNodesToBuffer();
		return;
	}
	else if((pPart=Ta.Parts.FromHandle(id_arr[0]))!=NULL)
	{
		BOLTSET boltset;
		PARTSET partset;
		partset.append(pPart);
		if(pPart->GetClassTypeId()==CLS_BOLT)
			boltset.append((CLDSBolt*)pPart);
		for(int i=1;i<n;i++)
		{
			if((pPart=Ta.Parts.FromHandle(id_arr[i]))!=NULL)
			{
				partset.append(pPart);
				if(boltset.GetNodeNum()>0&&pPart->GetClassTypeId()==CLS_BOLT)
					boltset.append((CLDSBolt*)pPart);
			}
		}
#ifdef __COMMON_PART_INC_
		if(boltset.GetNodeNum()>0)
		{	//������������˨
			CopyBoltSimplified(&boltset);
			return;
		}
		else
			CopyOrdinaryParts(partset);
#endif
	}
	else
		return;
	pCmdLine->FillCmdLine("���ƶ���(Ctrl+C):","");
	pCmdLine->FinishCmdLine();
}
//�˺�����֪�к��ã�ԭΪ[����]��<����˼����������ֵ>������Ϊ��ʱ��� wjh-2012.3.11
/*void CLDSView::OnClearRodDesignForce() 
{
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__TSA_)
	for(CLDSLinePart *pLinePart=Ta.Parts.GetFirstLinePart();pLinePart;pLinePart=Ta.Parts.GetNextLinePart())
	{
		pLinePart->connectStart.maxCompression=pLinePart->connectStart.maxTension=pLinePart->connectStart.maxMoment=0;
		pLinePart->connectEnd.maxCompression=pLinePart->connectEnd.maxTension=pLinePart->connectEnd.maxMoment=0;
	}
	MessageBox("���и˼��Ķ�ͷ���������ֵ�������㣡");
#endif
}*/
void CLDSView::OnSelectRelaBolts() 
{
#ifdef __COMMON_PART_INC_//ifndef __TSA_
	long *id_arr;
	long nSel=g_pSolidSnap->GetLastSelectEnts(id_arr);
	g_pSolidDraw->ReleaseSnapStatus();
	ARRAY_LIST<long> ordinaryBolts(0,8),footnailBolts(0,8),baseSlots(0,8);
	for(int i=0;i<nSel;i++)
	{
		CSuperSmartPtr<CLDSPart> pPart=console.FromPartHandle(id_arr[i]);
		selectObjs.DeleteNode(id_arr[i]);
		CLsRefList *pLsRefList=pPart->GetLsRefList();
		if(pLsRefList==NULL)
			continue;
		for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			if(pBolt->IsFootNail())
				footnailBolts.append(pBolt->handle);
			else //��ͨ��˨
				ordinaryBolts.append(pBolt->handle);
		}
		if(pPart->GetClassTypeId()==CLS_LINETUBE)
		{	//ѡ��ֹ��ϵĽŶ�
			for(CLDSLineSlot* pSlot=(CLDSLineSlot*)Ta.Parts.GetFirst(CLS_LINESLOT);pSlot;pSlot=(CLDSLineSlot*)Ta.Parts.GetNext(CLS_LINESLOT))
			{
				if(pSlot->IsBaseSlot()&&pSlot->hAttachTube==pPart->handle)
				{
					baseSlots.append(pSlot->handle);
					for(CLsRef *pLsRef=pSlot->GetFirstLsRef();pLsRef;pLsRef=pSlot->GetNextLsRef())
						footnailBolts.append(pLsRef->GetLsPtr()->handle);
				}
			}
		}
	}
	if (footnailBolts.GetSize()>0&&ordinaryBolts.GetSize()>0)
	{
		CSelAssotiatedBoltsDlg dlg;
		dlg.DoModal();
		if(dlg.m_bFootnailBolt)
		{
			g_pSolidDraw->SetEntSnapStatus(footnailBolts.m_pData,footnailBolts.GetSize());
			if(baseSlots.GetSize()>0)
				g_pSolidDraw->SetEntSnapStatus(baseSlots.m_pData,baseSlots.GetSize());
		}
		if (dlg.m_bOrdinaryBolt)
			g_pSolidDraw->SetEntSnapStatus(ordinaryBolts.m_pData,ordinaryBolts.GetSize());
	}
	else if (footnailBolts.GetSize()>0)
	{
		g_pSolidDraw->SetEntSnapStatus(footnailBolts.m_pData,footnailBolts.GetSize());
		if(baseSlots.GetSize()>0)
			g_pSolidDraw->SetEntSnapStatus(baseSlots.m_pData,baseSlots.GetSize());
	}
	else if (ordinaryBolts.GetSize()>0)
		g_pSolidDraw->SetEntSnapStatus(ordinaryBolts.m_pData,ordinaryBolts.GetSize());
	g_pSolidDraw->Draw();
	UpdatePropertyPage();
#endif
}
void CLDSView::OnAddRelaBoltsToView() 
{
#ifdef __COMMON_PART_INC_//ifndef __TSA_
	long *id_arr;
	long nSel=g_pSolidSnap->GetLastSelectEnts(id_arr);
	CDisplayView *pActView=console.GetActiveView();
	for(int i=0;i<nSel;i++)
	{
		CSuperSmartPtr<CLDSPart> pPart=console.FromPartHandle(id_arr[i]);
		CLsRefList *pLsRefList=pPart->GetLsRefList();
		if(pLsRefList==NULL)
			continue;
		for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			if(console.DispPartSet.FromHandle(pBolt->handle)==NULL)
			{
				console.DispPartSet.append(pBolt);
				CViewPart viewPart;
				viewPart.handle=pBolt->handle;
				viewPart.idClassType=CLS_BOLT;
				pActView->AppendViewPart(viewPart);
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			}
		}
	}
	g_pSolidDraw->Draw();
#endif
}

void CLDSView::OnSetPartSpreadFace()
{
#ifdef __COMMON_PART_INC_//ifndef __TSA_
	long *id_arr;
	long nSel=g_pSolidSnap->GetLastSelectEnts(id_arr);
	CDisplayView *pActiveView=console.GetActiveView();
	if(pActiveView==NULL||nSel<=0||(pActiveView->m_iViewType!=0&&pActiveView->m_iViewType!=1))
		return;
	pActiveView->InitHashTable();
	CViewPart *pFirstViewPart=pActiveView->FindViewPart(id_arr[0]);
	CLDSPart *pPart=console.FromPartHandle(id_arr[0]);
	if(pFirstViewPart==NULL&&pPart!=NULL)
	{
		pFirstViewPart=pActiveView->AppendViewPart(id_arr[0]);
		pFirstViewPart->idClassType=pPart->GetClassTypeId();
	}
	else if(pPart==NULL)
		return;
	CPartDrawMarkDlg markDlg;
	markDlg.m_pViewPart=pFirstViewPart;
	markDlg.m_pRelaView=pActiveView;
	markDlg.m_bOnlySetSpreadFace=TRUE;
	if(markDlg.DoModal()!=IDOK)
		return;
	for(int i=1;i<nSel;i++)
	{
		CViewPart *pViewPart=pActiveView->FindViewPart(id_arr[i]);
		pPart=console.FromPartHandle(id_arr[i]);
		if(pPart==NULL)
			continue;
		if(pViewPart==NULL)
		{
			pViewPart=pActiveView->AppendViewPart(id_arr[i]);
			pViewPart->idClassType=pPart->GetClassTypeId();
		}
		pViewPart->iResidePlaneNo=pFirstViewPart->iResidePlaneNo;
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{	//���ýǸֻ�ͼƽ��ʱ,ͬʱ���ýǸֹ�����˨�Ļ�ͼƽ��
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pPart;
			for(CLsRef *pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
			{
				pViewPart=pActiveView->FindViewPart((*pLsRef)->handle);
				if(pViewPart)
					pViewPart->iResidePlaneNo=pFirstViewPart->iResidePlaneNo;
			}
		}
	}
#endif
}

void CLDSView::OnSetArcLift()
{
#ifdef __COMMON_PART_INC_
	long *id_arr;
	long nSel=g_pSolidSnap->GetLastSelectEnts(id_arr);
	long hArcLift=0;
	bool selectObjsBeforehand=nSel>0;	//Ԥ��ѡ�д�Ԥ������
	if(nSel==0)	//ͨ����������Ԥ����Թ�������Ԥ������
	{
		//ѡ��ѡ���׼�ڵ�
		CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
		CLDSNode* pSelNode=NULL;
		f3dPoint* pPoint;
		f3dPoint rodOrg,datumListPos;
		DISPLAY_TYPE disp_type;
		g_pSolidSet->GetDisplayType(&disp_type);
		if(DISP_LINE!=disp_type)
		{	//�л���������ʾ״̬
			g_pSolidSet->SetDisplayType(DISP_LINE);
			g_pSolidDraw->Draw();
		}
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select lift arc rotate basic node:","");
#else
		pCmdLine->FillCmdLine("��ѡ��Ԥ����ת��׼�ڵ�:","");
#endif
		while(1)
		{
			g_pSolidSnap->SetSnapType(SNAP_POINT);
			int ret =g_pSolidSnap->SnapPoint(pPoint);
			if(ret<0)
			{
				pCmdLine->CancelCmdLine();	
				return;
			}
			else if(ret>0)
			{
				pSelNode=console.FromNodeHandle(pPoint->ID);
				if(pSelNode)
				{
					rodOrg.Set(pSelNode->Position().x,pSelNode->Position().y,pSelNode->Position().z);
					g_pSolidDraw->SetEntSnapStatus(pPoint->ID,TRUE);
					break;
				}
			}
			pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("The suitable node has not been selected��please select again:","");
		}
		//select lift arc rotate basic node
		pCmdLine->FillCmdLine("Please select lift arc rotate basic node:","");
#else
			pCmdLine->FillCmdLine("û��ѡ�к��ʽڵ㣬������ѡ��:","");
		}
		//ѡ��Ԥ���߶Ȼ�׼�ڵ�
		pCmdLine->FillCmdLine("��ѡ��Ԥ���߶Ȼ�׼�ڵ�:","");
#endif
		while(1)
		{
			g_pSolidSnap->SetSnapType(SNAP_POINT);
			int ret =g_pSolidSnap->SnapPoint(pPoint);
			if(ret<0)
			{
				pCmdLine->CancelCmdLine();	
				return;
			}
			else if(ret>0)
			{
				pSelNode=console.FromNodeHandle(pPoint->ID);
				if(pSelNode)
				{
					datumListPos.Set(pSelNode->Position().x,pSelNode->Position().y,pSelNode->Position().z);
					g_pSolidDraw->SetEntSnapStatus(pPoint->ID,TRUE);
					break;
				}
			}
			pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("The suitable node has not been selected��please select again:","");
		}
		//Fill lift arc information:
		pCmdLine->FillCmdLine("Fill lift arc information:","");
#else
			pCmdLine->FillCmdLine("û��ѡ�к��ʵĽڵ㣬������ѡ��:","");
		}
		//���Ԥ����Ϣ:
		pCmdLine->FillCmdLine("���Ԥ����Ϣ:","");
#endif
		CTowerTreeDlg *pTreeView = (CTowerTreeDlg*)((CMainFrame*)AfxGetMainWnd())->GetTowerTreePage();
		hArcLift=pTreeView->InsertArcLiftItem(NULL,&rodOrg,&datumListPos);//����Ԥ����
		if(hArcLift==0)
		{
			pCmdLine->CancelCmdLine();
			return ;
		}
		//ѡȡ������Χ����Ԥ��
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select a range of lift arc:","");
#else
		pCmdLine->FillCmdLine("��ѡ��Ԥ����Χ:","");
#endif
		g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_NODE)|GetSingleWord(SELECTINDEX_LINE));
		OpenWndSel();	//����ѡ��
		CString cmdStr;
		if(pCmdLine->GetStrFromCmdLine(cmdStr))
			nSel=g_pSolidSnap->GetLastSelectEnts(id_arr);
		else
		{
			pCmdLine->CancelCmdLine();
			return;
		}
		if(nSel==0)
			return;
		pCmdLine->FinishCmdLine();
	}
	CLDSNode *pNode=NULL;
	CLDSLinePart *pRod=NULL;
	CSuperSmartPtr<CLDSDbObject>pNodeOrRod;
	pNodeOrRod=console.FromNodeHandle(id_arr[0]);
	if(hArcLift==0)
	{	//Ԥ����Ϊ��ʱ���г�ʼ��
		if(pNodeOrRod.IsNULL())
			pNodeOrRod=console.FromPartHandle(id_arr[0],CLS_LINEPART);
		if(pNodeOrRod.IsHasPtr())
		{
			if(pNodeOrRod->IsNode())
				hArcLift=pNodeOrRod.NodePointer()->ArcLiftHandle();
			if(pNodeOrRod->IsLinePart())
				hArcLift=pNodeOrRod.LinePartPointer()->ArcLiftHandle();
		}
		CLDSArcLift *pArcLift=Ta.ArcLift.GetFirst();
		if(hArcLift>0)
			pArcLift=Ta.ArcLift.FromHandle(hArcLift);
		else if(pArcLift)
			hArcLift=pArcLift->handle;
	}
	if(Ta.ArcLift.GetNodeNum()>1||selectObjsBeforehand)
	{	//�ж���Ԥ����ɽ���ѡ�񣬷���ֱ����Ԥ��,����Ԥ��ָ����Ԥ������ʱҲӦ��ʾ������������ɻ����������� wjh-2013.6.23
		CSetArcLiftDlg arcliftdlg;
		arcliftdlg.m_hArcLift=hArcLift;
		if(arcliftdlg.DoModal()!=IDOK)
			return;
		hArcLift=arcliftdlg.m_hArcLift;
	}
	if(pNodeOrRod.IsHasPtr()&&pNodeOrRod->IsNode())
		pNodeOrRod.NodePointer()->SetArcLift(hArcLift);
	else if(pNodeOrRod.IsHasPtr()&&pNodeOrRod->IsLinePart())
		pNodeOrRod.LinePartPointer()->SetArcLift(hArcLift);
	CHashSet<CLDSNode*> hashRelaNodes,hashSelNodes;
	for(int i=1;i<nSel;i++)
	{
		pNodeOrRod=console.FromNodeHandle(id_arr[i]);
		if(pNodeOrRod.IsHasPtr())
		{
			pNodeOrRod.NodePointer()->SetArcLift(hArcLift);
			pNodeOrRod.NodePointer()->Set();
			hashSelNodes.SetValue(pNodeOrRod->handle,pNodeOrRod.pNode);
		}
		else
		{
			pNodeOrRod=console.FromPartHandle(id_arr[i],CLS_LINEPART);
			if(pNodeOrRod.IsHasPtr())
			{
				pNodeOrRod.LinePartPointer()->SetArcLift(hArcLift);
				pNodeOrRod.LinePartPointer()->CalPosition();
				if(pNodeOrRod.pRod->pStart)
					hashRelaNodes.SetValue(pNodeOrRod.pRod->pStart->handle,pNodeOrRod.pRod->pStart);
				if(pNodeOrRod.pRod->pEnd)
					hashRelaNodes.SetValue(pNodeOrRod.pRod->pEnd->handle,pNodeOrRod.pRod->pEnd);
			}
		}
	}
	//�Զ��趨ʵ��ģʽ��δ��ѡ�еĽڵ�(���˼�Ϊ��ǰԤ���˼��Ľڵ�) wjh-2017.5.18
	for(pNode=hashRelaNodes.GetFirst();pNode;pNode=hashRelaNodes.GetNext())
	{
		if(hashSelNodes.GetValue(pNode->handle))
			continue;
		pNode->SetArcLift(hArcLift);
		pNode->Set();
	}
#endif
}
void CLDSView::OnCorrectBoltL0Para() 
{
#ifdef __COMMON_PART_INC_//ifndef __TSA_
	Ta.BeginUndoListen();
	long hCurBolt=0;
	int nInvalidHits=0;
	try{
		CLDSBolt *pBolt;
		CSuperSmartPtr<CLDSPart>pPart;
		CHashTable<double>hashL0,hashL;
		hashL0.CreateHashTable(Ta.Parts.GetNodeNum(CLS_BOLT));
		hashL.CreateHashTable(Ta.Parts.GetNodeNum(CLS_BOLT));
		CStackVariant<char> stackBoltL0CalVariable(&CLDSBolt::BOLTL0_CALMODE, CLDSBolt::L0CAL_INC_GAP_THICK);
		for(pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
		{
			hashL0.SetValueAt(pBolt->handle,pBolt->get_L0());
			hashL.SetValueAt(pBolt->handle,pBolt->get_L());
			if(strlen(pBolt->statSegStr)==0)
			{
				pBolt->feature=1;	//��Ҫ����
				pBolt->EmptyL0DesignPara();
			}
			else
				pBolt->feature=0;	//������ֻ���˨����Ԥ����
		}
		for(pPart=Ta.Parts.GetFirst();pPart.IsHasPtr();pPart=Ta.Parts.GetNext())
		{
			CLsRefList *pLsRefList=pPart->GetLsRefList();
			if(pLsRefList)
			{
				for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
				{
					if(pLsRef->GetLsPtr()->feature==0)
						continue;
					hCurBolt=pLsRef->GetLsPtr()->handle;
					pLsRef->GetLsPtr()->AddL0Thick(pPart->handle,TRUE);
				}
			}
		}
		FILE *fp = fopen("LsL0.txt","wt");
		for(pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
		{
			if(pBolt->m_bVirtualPart||pBolt->feature==0)
				continue;	//������˨��Ϊ����ã���ʵ��ͳ�����壬���Ҳ��������Ҫwjh-2011.9.3
			BOOL pushed=Ta.Parts.push_stack();
			BOOL bRet=pBolt->CalGuigeAuto();
			double L=0,L0=0;
			hashL0.GetValueAt(pBolt->handle,L0);
			hashL.GetValueAt(pBolt->handle,L);
			if(L0!=pBolt->get_L0()||L!=pBolt->get_L())
			{
				if(pBolt->IsFootNail())
#ifdef AFX_TARG_ENU_ENGLISH
					fprintf(fp,"%4d#0X%X climbing bolt",nInvalidHits+1,pBolt->handle);
				else
					fprintf(fp,"%4d#0X%X bolt",nInvalidHits+1,pBolt->handle);
				if(L0!=pBolt->get_L0())
					fprintf(fp,"L0 correction:%d->%d��",ftoi(L0),ftoi(pBolt->get_L0()));
				else
					fprintf(fp,"L0:%d��",ftoi(L0));
#else
					fprintf(fp,"%4d#0X%X�Ŷ�",nInvalidHits+1,pBolt->handle);
				else
					fprintf(fp,"%4d#0X%X��˨",nInvalidHits+1,pBolt->handle);
				if(L0!=pBolt->get_L0())
					fprintf(fp,"ͨ������:%.f->%.f��",L0,pBolt->L0);
				else
					fprintf(fp,"ͨ��:%.f��",L0);
#endif
				if(bRet)
				{
					if(L!=pBolt->get_L())
#ifdef AFX_TARG_ENU_ENGLISH
						fprintf(fp,"Specification has been corrected successfully:M%dX%d->M%dX%d!\n",pBolt->get_d(),ftoi(L),pBolt->get_d(),ftoi(pBolt->get_L()));
					else
						fprintf(fp,"Specification:M%dX%d!\n",pBolt->get_d(),ftoi(L));
				}
				else
					fprintf(fp,"Fail to correct the bolt specification,the suitable bolt specification for L0 is not found(keep the old specificationM%dX%d)!\n",pBolt->get_d(),ftoi(pBolt->get_L()));
#else
						fprintf(fp,"���ɹ�����:M%dX%d->M%dX%d!\n",pBolt->get_d(),ftoi(L),pBolt->get_d(),ftoi(pBolt->get_L()));
					else
						fprintf(fp,"���:M%dX%d!\n",pBolt->get_d(),ftoi(L));
				}
				else
					fprintf(fp,"��˨�������ʧ�ܣ�û���ҵ�����ͨ��ֵ����˨���(����ԭ���M%dX%d)!\n",pBolt->get_d(),ftoi(pBolt->get_L()));
#endif
				nInvalidHits++;
			}
			Ta.Parts.pop_stack(pushed);
		}
		fclose(fp);
	}
	catch(char* sError)
	{
		CString error;
#ifdef AFX_TARG_ENU_ENGLISH
		error.Format("%s error bolt handle is:0X%X",sError,hCurBolt);
#else
		error.Format("%s������˨���Ϊ:0X%X",sError,hCurBolt);
#endif
		AfxMessageBox(error);
	}
	Ta.EndUndoListen();
	if(nInvalidHits==0)
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("There is not any L0 needed to correct!");
#else
		MessageBox("����˨ͨ����Ҫ������");
#endif
	else
		WinExec("notepad.exe LsL0.txt",SW_SHOW);
#endif

}

void CLDSView::OnSaveAsNewBlock()
{
	if(!PRODUCT_FUNC::IsHasBlockSetup())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Current authorization does not support the block assembling!"); 
#else
		AfxMessageBox("��ǰ��Ȩ��֧�ֲ���װ��!");
#endif
		return;
	}
	CBlockModel *pBlock=Ta.Block.append();
#ifdef AFX_TARG_ENU_ENGLISH
    strcpy(pBlock->name,"New block");
#else
	strcpy(pBlock->name,"�²���");
#endif
	CString szBlkName=pBlock->name;
	if(Input("���벿������",&szBlkName,'s')>0)
		CXhString(pBlock->name,51)=szBlkName;
	console.FillBlock(pBlock);
	CTowerTreeDlg *pTreeView = (CTowerTreeDlg*)((CMainFrame*)AfxGetMainWnd())->GetTowerTreePage();
	CTreeCtrl *pTreeCtrl = pTreeView->GetTreeCtrl();
	HTREEITEM hBlockItem=pTreeView->InsertBlockItem(pBlock);
	pTreeView->RefreshModelItem(hBlockItem,pBlock);
	pTreeCtrl->SelectItem(hBlockItem);
	//��ʾ��������
	CLDSView *pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
	pLdsView->m_pSelectBlock=pBlock;
	pLdsView->DisplayBlockModelProperty(TRUE);
}

void CLDSView::ExplodeBlockRef(CBlockReference *pBlockRef)
{
	if(!PRODUCT_FUNC::IsHasBlockSetup())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Current authorization does not support block assembling!");
#else
		AfxMessageBox("��ǰ��Ȩ��֧�ֲ���װ��!");
#endif
		return;
	}
	if(pBlockRef==NULL)
		return;
	CWaitCursor wait;
	BOOL bRetCode=FALSE;
	if(pBlockRef->IsExploded())
	{	//ִ��ȡ��ը��
		bRetCode=pBlockRef->Exploded(FALSE);
		if(!bRetCode)
			return;
		for(CLDSPart *pPart=console.EnumPartFirst();pPart;pPart=console.EnumPartNext())
		{
			if(!pPart->IsMemberOfBlockRef(pBlockRef))
				continue;
			BOOL pushed=console.PushPartStack();
			g_pSolidDraw->DelEnt(pPart->handle);
			CLDSObject* pMotherObj=pPart->MotherObject(false);
			if(pPart->IsRelaUsed()&&pMotherObj!=NULL&&pMotherObj!=pPart)
			{
				pPart->SetAggregateState();
				pBlockRef->hashReserveBolts.SetValue(pMotherObj->handle,(CLDSBolt*)pPart);
			}
			else
				console.DeletePart(pPart->handle);
			console.PopPartStack(pushed);
		}
		for(CLDSNode* pNode=console.EnumNodeFirst();pNode;pNode=console.EnumNodeNext())
		{
			if(!pNode->IsMemberOfBlockRef(pBlockRef))
				continue;
			g_pSolidDraw->DelEnt(pNode->handle);
			console.DeleteNode(pNode->handle);
		}
		pBlockRef->SetModified();
		pBlockRef->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBlockRef->GetSolidPartObject());
		g_pSolidDraw->Draw();
	}
	else 
	{
		bRetCode=pBlockRef->Exploded(TRUE);
		if(!bRetCode)
			return;
		for(CLDSPart *pPart=console.EnumPartFirst();pPart;pPart=console.EnumPartNext())
		{
			if(!pPart->IsMemberOfBlockRef(pBlockRef))
				continue;
			pPart->SetModified();
			pPart->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
		}
		for(CLDSNode* pNode=console.EnumNodeFirst();pNode;pNode=console.EnumNodeNext())
		{
			if(!pNode->IsMemberOfBlockRef(pBlockRef))
				continue;
			f3dPoint node_pos = pNode->ActivePosition();
			node_pos.ID = pNode->handle;
			node_pos.pen.crColor = g_sysPara.crPartMode.crNode;
			g_pSolidDraw->NewPoint(node_pos);
		}
		g_pSolidDraw->DelEnt(pBlockRef->handle);
		g_pSolidDraw->Draw();
	}
	//�����ǰ��������ʾ�����������ԣ����²�����������
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	if(pPropList->m_nObjClassTypeId == CLS_BLOCKREF&&bRetCode)
	{
		CXhChar100 value_str;
		long idProp=CBlockReference::GetPropID("m_bExploded");
		if(pBlockRef->GetPropValueStr(idProp,value_str)>0)
			pPropList->SetItemPropValue(idProp,CString(value_str));
	}
}
static void  RetrievePartsSegIFromPartNo(PARTSET &partSet)
{
	CRetrieveSegIDlg dlg;
	for (CLDSPart* pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
		dlg.m_partSet.append(pPart);
	if(dlg.DoModal()==IDOK)
	{
		;
	}
}
bool CLDSView::IntegrateBlockRef(CBlockReference *pBlockRef)
{
	if(!PRODUCT_FUNC::IsHasBlockSetup())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Current authorization does not support block assembling merge!");
#else
		AfxMessageBox("��ǰ��Ȩ��֧�ֲ�����װ���ں�!");
#endif
		return false;
	}
	else if(theApp.m_bCooperativeWork)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The block assembling merge function is not supported at the cooperative work mode!");
#else
		AfxMessageBox("��ǰ��֧����Эͬ��ҵģʽ�²�����װ���ںϹ���!");
#endif
		return false;
	}
	if(pBlockRef==NULL)
		return false;
	CWaitCursor wait;
	BOOL bRetCode=FALSE;
	CLDSNode* pNode;
	CLDSPart* pPart;
	CLogErrorLife life;
	CUndoOperObject undo(&Ta);
	ARRAY_LIST<long> removeNodesIdArr(0,1000),removePartsIdArr(0,1000);
	if(pBlockRef->IsExploded())
	{	//ִ��ȡ��ը��
		for(pPart=console.EnumPartFirst();pPart;pPart=console.EnumPartNext())
		{
			if(!pPart->IsMemberOfBlockRef(pBlockRef))
				continue;
			removePartsIdArr.append(pPart->handle);
			console.DispPartSet.DeleteNode(pPart->handle);
			g_pSolidDraw->DelEnt(pPart->handle);
		}
		for(pNode=console.EnumNodeFirst();pNode;pNode=console.EnumNodeNext())
		{
			if(!pNode->IsMemberOfBlockRef(pBlockRef))
				continue;
			removeNodesIdArr.append(pNode->handle);
			console.DispPartSet.DeleteNode(pNode->handle);
			g_pSolidDraw->DelEnt(pNode->handle);
		}
		pBlockRef->SetModified();
	}
	else 
		g_pSolidDraw->DelEnt(pBlockRef->handle);
	Ta.Node.DeleteNodeArr(removeNodesIdArr.m_pData,removeNodesIdArr.GetSize());
	Ta.Parts.DeleteNodeArr(removePartsIdArr.m_pData,removePartsIdArr.GetSize(),TRUE);
	Ta.Node.GetTail();	//�޴��д��룬���ǡ��Ta.Node.cursor==NULLʱ��ѹջʧ��
	BOOL pushnodes=Ta.Node.push_stack();
	Ta.Parts.GetTail();	//�޴��д��룬���ǡ��Ta.Parts.cursor==NULLʱ��ѹջʧ��
	BOOL bIsNullTa=Ta.Parts.GetNodeNum()==0;
	BOOL pushparts=Ta.Parts.push_stack();
	pBlockRef->IntegrateToTowerModel();
	Ta.Node.pop_stack(pushnodes);
	for(pNode=Ta.Node.GetNext();pNode;pNode=Ta.Node.GetNext())
		NewNode(pNode);
	Ta.Parts.pop_stack(pushparts);
	PARTSET partSet;
	for(pPart=bIsNullTa?Ta.Parts.GetFirst():Ta.Parts.GetNext();pPart;pPart=Ta.Parts.GetNext())
	{
		partSet.append(pPart);
		if(pPart->IsLinePart())
			NewLinePart((CLDSLinePart*)pPart);
		pPart->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
	}
	if(partSet.GetNodeNum()>0)
		RetrievePartsSegIFromPartNo(partSet);
	g_pSolidDraw->Draw();
	UpdatePropertyPage();
	console.DispBlockRefSet.DeleteNode(pBlockRef->handle);
	Ta.BlockRef.DeleteNode(pBlockRef->handle);
	return true;
}

void CLDSView::OnExplodeBlockRef()
{
	long* id_arr;
	int id_num = g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(id_num==1)
	{
		CBlockReference* pBlockRef=Ta.BlockRef.FromHandle(id_arr[0]);
		ExplodeBlockRef(pBlockRef);
		//����ʵ��õ�������ͼ�еĶ�Ӧ����
		CTowerTreeDlg *pTreeView = (CTowerTreeDlg*)((CMainFrame*)AfxGetMainWnd())->GetTowerTreePage();
		CTreeCtrl* pTreeCtrl = pTreeView->GetTreeCtrl();
		HTREEITEM hRootItem = pTreeCtrl->GetRootItem();
		HTREEITEM hModelItem = pTreeCtrl->GetChildItem(hRootItem);
		HTREEITEM hSonItem = pTreeCtrl->GetChildItem(hModelItem),hBlockRefenceItem=NULL;
		TREEITEM_INFO *pItemInfo=NULL;
		while(hSonItem)
		{
			pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hSonItem);
			if(pItemInfo&&pItemInfo->itemType==BLOCKREF_GROUP)
				break;
			hSonItem=pTreeCtrl->GetNextSiblingItem(hSonItem);
		}
		if(pTreeCtrl->ItemHasChildren(hSonItem))
		{
			hBlockRefenceItem=pTreeCtrl->GetChildItem(hSonItem);
			while(hBlockRefenceItem)
			{
				pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hBlockRefenceItem);
				CBlockReference* pBlockRefence = (CBlockReference*)pItemInfo->dwRefData;
				if(pBlockRefence->handle== pBlockRef->handle)
					break;
				hBlockRefenceItem = pTreeCtrl->GetNextSiblingItem(hBlockRefenceItem);
			}
		}
		if(hBlockRefenceItem==NULL)
			return;
		if(pBlockRef->IsExploded())
			pTreeCtrl->SetItemImage(hBlockRefenceItem,PRJ_IMG_EXPLODED_BLOCKREF_CASE,PRJ_IMG_EXPLODED_BLOCKREF_CASE);
		else
			pTreeCtrl->SetItemImage(hBlockRefenceItem,PRJ_IMG_UNEXPLODED_BLOCKREF_CASE,PRJ_IMG_UNEXPLODED_BLOCKREF_CASE);
	}
}
void CLDSView::OnLogViewer()
{
#ifdef __PART_DESIGN_INC_
	if(!g_sysPara.dock.pageLogger.bDisplay)
		((CMainFrame*)AfxGetMainWnd())->ModifyDockpageStatus(RUNTIME_CLASS(CLogViewerDlg),TRUE);
	g_sysPara.dock.pageLogger.bDisplay=TRUE;
	((CMainFrame*)AfxGetMainWnd())->ActivateDockpage(RUNTIME_CLASS(CLogViewerDlg));
	CLogViewerDlg *pLogViewerDlg=((CMainFrame*)AfxGetMainWnd())->GetLogViewerPage();
	pLogViewerDlg->RefreshLogVersionList();
#endif
}

//�����������߸˼�
void CLDSView::OnDesignColumnRayRod()
{
	m_nPrevCommandID=ID_DESIGN_COLUMN_RAY_ROD;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat to generate cylindrical ray rod";
#else
	m_sPrevCommandName="�ظ������������߸˼�";
#endif
	Command("DesignColumnRayRod");
}
CDesignColumnRayRodDlg desRayRodDlg;
int CLDSView::DesignColumnRayRod()
{
	CString cmdStr;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CUndoOperObject undo(&Ta);
	long *id_arr=NULL;
	int n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	//1��ѡȡ��׼�ֹ�
	if(n==0)
	{
		GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECT_ALL),CLS_LINETUBE);
		scr_part_para.disp_type=DISP_SOLID;
#ifdef AFX_TARG_ENU_ENGLISH
		scr_part_para.cmdStr="DesignColumnRayRod Please select locate tube: (Can press the <Enter> key directly if there is no)";
#else
		scr_part_para.cmdStr="DesignColumnRayRod ��ѡ��λ��λ�ֹ�: (��û�п�ֱ�Ӱ��س���)";
#endif
		scr_part_para.cmdErrorFeedbackStr="";	//����Ϊ��
		while(1)
		{	//������������Ӧ������̧�����Ϣ��ͬ���س���Ч����ͬ
			if(!GetPartsFromScr(scr_part_para))//,TRUE))
				return FALSE;
			CLDSDbObject *pObj=scr_part_para.pResultObjsArr[0];
			if(pObj)
			{
				if(pObj->GetClassTypeId()==CLS_LINETUBE)
					desRayRodDlg.m_pBaseLineTube=(CLDSLineTube*)pObj;
				else
				{
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("No tube is selected��please select locate tube again: (Can press the <Enter> key directly if there is no)","");
#else
					pCmdLine->FillCmdLine("û��ѡ�иֹܣ�������ѡ��λ�ֹ�: (��û�п�ֱ�Ӱ��س���)","");
#endif
					pCmdLine->FinishCmdLine();
					continue;
				}
			}
			break;
		}
	}
	else
		desRayRodDlg.m_pBaseLineTube=(CLDSLineTube*)console.FromPartHandle(id_arr[0],CLS_LINETUBE);
	//2.�������߸˼�
	if(desRayRodDlg.DoModal()==IDOK)
		desRayRodDlg.CreateRayRod();
	return TRUE;
}

//<DEVELOP_PROCESS_MARK nameId="CLDSView::CutAngle">
//�Ǹ�45���н�
static void CutAngle(int start0_end1_both2,int wing_x0_y1_both2)
{
	CUndoOperObject undo(&Ta,true);
	long *id_arr=NULL;
	long n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(n<=0)
		return;
	
	for(int i=0;i<n;i++)
	{
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(id_arr[i],CLS_LINEANGLE);
		if(pLineAngle==NULL)
			continue;
		double CutAngleCoefR=0;
		if(g_sysPara.m_b45CutAngleNeedCoefR)
			CutAngleCoefR=pLineAngle->GetCutAngleCoefR();//ϵ��
		double r = FindJg_r(pLineAngle->GetWidth());//�뾶
		double fCutAngle=pLineAngle->GetWidth()-pLineAngle->GetThick()-CutAngleCoefR*r;
		//�Ǹ�45���нǣ���ǰ֫������֫ʱ������֫������ȡ����(ȡ�нǳ�����֫���߳����еĽϴ�ֵ) wht 16-10-08
		if(start0_end1_both2==0||start0_end1_both2==2)
		{	//ʼ���н�
			if(wing_x0_y1_both2==0||wing_x0_y1_both2==2)
			{	
				if(pLineAngle->cut_wing[0]==1)	//ʼ��X֫��֫
					pLineAngle->cut_wing_para[0][2]=max(fCutAngle,pLineAngle->cut_wing_para[0][2]);
				else //X֫�н�
					pLineAngle->cut_angle[0][0]=pLineAngle->cut_angle[0][1]=fCutAngle;
			}
			if(wing_x0_y1_both2==1||wing_x0_y1_both2==2)
			{	
				if(pLineAngle->cut_wing[0]==2)	//ʼ��Y֫��֫
					pLineAngle->cut_wing_para[0][2]=max(fCutAngle,pLineAngle->cut_wing_para[0][2]);
				else //Y֫�н�
					pLineAngle->cut_angle[1][0]=pLineAngle->cut_angle[1][1]=fCutAngle;
			}
			pLineAngle->m_bUserSpecStartCutAngle=TRUE;
			pLineAngle->SyncMirProp("oddStartOnSameLabel",0,LABEL_PROP::ANGLE_CUT_START);
		}
		if(start0_end1_both2==1||start0_end1_both2==2)
		{	//�ն��н�
			if(wing_x0_y1_both2==0||wing_x0_y1_both2==2)
			{	
				if(pLineAngle->cut_wing[1]==1)	//�ն�X֫��֫
					pLineAngle->cut_wing_para[1][2]=max(fCutAngle,pLineAngle->cut_wing_para[1][2]);
				else //X֫�н�
					pLineAngle->cut_angle[2][0]=pLineAngle->cut_angle[2][1]=fCutAngle;
			}
			if(wing_x0_y1_both2==1||wing_x0_y1_both2==2)
			{	
				if(pLineAngle->cut_wing[1]==2)	//�ն�Y֫��֫
					pLineAngle->cut_wing_para[1][2]=max(fCutAngle,pLineAngle->cut_wing_para[1][2]);
				else //Y֫�н�
					pLineAngle->cut_angle[3][0]=pLineAngle->cut_angle[3][1]=fCutAngle;
			}
			pLineAngle->m_bUserSpecEndCutAngle=TRUE;
			pLineAngle->SyncMirProp("oddEndOnSameLabel",0,LABEL_PROP::ANGLE_CUT_END);
		}
		pLineAngle->SetModified();
		pLineAngle->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
}
void CLDSView::OnStartWingXCutAngle()
{
	CutAngle(0,0);	//ʼ��X֫�н�
}
void CLDSView::OnStartWingYCutAngle()
{
	CutAngle(0,1);	//ʼ��Y֫�н�
}
void CLDSView::OnEndWingXCutAngle()
{
	CutAngle(1,0);	//�ն�X֫�н�
}
void CLDSView::OnEndWingYCutAngle()
{
	CutAngle(1,1);	//�ն�Y֫�н�
}
void CLDSView::OnWingXCutAngle()
{
	CutAngle(2,0);	//����X֫�н�
}
void CLDSView::OnWingYCutAngle()
{
	CutAngle(2,1);	//����Y֫�н�
}
//��Ҫ����ֱ�����Ϊ���¶�����ʱ�м����µ��ת�� wjh-2017.5.18
void CLDSView::OnUpgradeCtrlNode()
{
#ifdef __PART_DESIGN_INC_
	long* id_arr=NULL;
	int count=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(count<0)
		return;
	CLogErrorLife life;
	CUndoOperObject undo(&Ta);
	int hits=0;
	for(int i=0;i<count;i++)
	{
		CLDSNode* pNode=Ta.Node.FromHandle(id_arr[i]);
		if(pNode->m_cPosCalType==0||!Ta.UpgradeToCtrlNode(pNode))
			continue;
		hits++;
		for(RELATIVE_OBJECT* pRela=pNode->relativeObjs.GetFirst();pRela;pRela=pNode->relativeObjs.GetNext())
		{
			CLDSNode* pRelaNode=Ta.Node.FromHandle(pRela->hObj);
			if(pRelaNode->m_cPosCalType==0||Ta.UpgradeToCtrlNode(pRelaNode))
				hits++;
		}
	}
	if(hits>0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox(CXhChar50("%d node control node have been converted ",hits));
#else
		MessageBox(CXhChar50("�������%d�����Ƶ�ת��",hits));
#endif
		UpdatePropertyPage();
	}
#endif
}
//</DEVELOP_PROCESS_MARK>
#include "CalPerfectSectsDlg.h"
void CLDSView::OnCalPerfectSectCount()
{
#ifdef __PART_DESIGN_INC_
	long* id_arr=NULL;
	if(g_pSolidSnap->GetLastSelectEnts(id_arr)<0)
		return;
	CLDSLinePart* pRod=(CLDSLinePart*)Ta.Parts.FromHandle(id_arr[0],CLS_LINEPART,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	if(pRod==NULL||pRod->pStart==NULL||pRod->pEnd==NULL)
		return;
	CCalPerfectSectsDlg sectsdlg;
	sectsdlg.m_pRod=pRod;
	if(sectsdlg.DoModal()!=IDOK)
		return;
	//
#endif
}

void CLDSView::OnUpdateRemoveRelaObjects(CCmdUI *pCmdUI)
{
	long* id_arr=NULL;
	pCmdUI->Enable(g_pSolidSnap->GetLastSelectEnts(id_arr)>0);
}

void CLDSView::OnRemoveRelaObjects()
{
	long* id_arr=NULL,hits=0;
	long count=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(count<=0)
		return;
	for(long i=0;i<count;i++)
	{
		CLDSDbObject* pObj=Ta.Node.FromHandle(id_arr[i]);
		if(pObj==NULL)
			pObj=Ta.Parts.FromHandle(id_arr[i]);
		if(pObj==NULL)
			continue;
		for(RELATIVE_OBJECT* pRelaObj=pObj->relativeObjs.GetFirst();pRelaObj;pRelaObj=pObj->relativeObjs.GetNext())
		{
			CLDSDbObject* pRelaObject=Ta.Node.FromHandle(pRelaObj->hObj);
			if(pRelaObject==NULL)
				pRelaObject=Ta.Parts.FromHandle(pRelaObj->hObj);
			if(pRelaObject)
				pRelaObject->relativeObjs.DeleteNode(pObj->handle);
		}
		if(pObj->relativeObjs.GetNodeNum()>0)
			hits++;
		pObj->relativeObjs.Empty();
	}
#ifdef AFX_TARG_ENU_ENGLISH
	MessageBox(CXhChar16("A total of %d objects symmetric information removed success!",hits));
#else
	MessageBox(CXhChar16("����%d������Ĺ����Գ���Ϣ�Ƴ��ɹ���",hits));
#endif
}
//���Ʊ�׼ģ�壨����FacePanel�ӳ���
BOOL WriteToFacePanelClient(HANDLE hPipeWrite)
{
	if( hPipeWrite == INVALID_HANDLE_VALUE )
	{
		AfxMessageBox("��ȡ�ܵ������Ч\n");
		return FALSE;
	}
	BYTE cProductType=PRODUCT_TMA;
#ifdef __TMA_
	cProductType=PRODUCT_TMA;
#elif defined(__LMA_)
	cProductType=PRODUCT_LMA;
#elif defined(__TSA_)
	cProductType=PRODUCT_TSA;
#elif defined(__LDS_)
	cProductType=PRODUCT_LDS;
#elif defined(__TDA_)
	cProductType=PRODUCT_TDA;
#endif
	CBuffer buffer(1000000);	//1Mb
	buffer.WriteByte(cProductType);				//��Ʒ����
	buffer.WriteDword(theApp.m_version[0]);		//�汾��Ϣ
	buffer.WriteDword(theApp.m_version[1]);
	buffer.WriteString(APP_PATH,MAX_PATH);		//֤��·��
	//�������ܵ���д������
	return buffer.WriteToPipe(hPipeWrite,1024);
}

BOOL CreateFacePanelProcess( HANDLE hClientPipeRead, HANDLE hClientPipeWrite )
{
	TCHAR cmd_str[MAX_PATH]="";
	GetSysPath(cmd_str);
	strcat(cmd_str,"FacePanel.exe -child");

	STARTUPINFO startInfo;
	memset( &startInfo, 0 , sizeof( STARTUPINFO ) );

	startInfo.cb= sizeof( STARTUPINFO );
	startInfo.dwFlags |= STARTF_USESTDHANDLES;
	startInfo.hStdError= 0;//hPipeWrite;
	startInfo.hStdInput= hClientPipeRead;
	startInfo.hStdOutput= hClientPipeWrite;

	PROCESS_INFORMATION processInfo;
	memset( &processInfo, 0, sizeof(PROCESS_INFORMATION) );

	BOOL b=CreateProcess( NULL,cmd_str,
		NULL,NULL, TRUE,CREATE_NEW_CONSOLE, NULL, NULL,&startInfo,&processInfo );
	DWORD er=GetLastError();
	return b;
}
void CLDSView::OnCreateFacePanel()
{
	if(!PRODUCT_FUNC::IsHasStdTemplate())
		return;	//��Ȩ֤���в����С������׼ģ�塱���ܵ�ʹ����Ȩ
	//1��������һ���ܵ�: ���ڷ���������ͻ��˷�������
	SECURITY_ATTRIBUTES attrib;
	attrib.nLength = sizeof( SECURITY_ATTRIBUTES );
	attrib.bInheritHandle= true;
	attrib.lpSecurityDescriptor = NULL;
	HANDLE hPipeClientRead=NULL, hPipeSrcWrite=NULL;
	if( !CreatePipe( &hPipeClientRead, &hPipeSrcWrite, &attrib, 0 ) )
		return ;
	//��Ϊ��ʱ����������ɼ̳У��ʿɸ���һ�����ɼ̳е�д�������ԭ����д����ص�
	HANDLE hPipeSrcWriteDup=NULL;
	if( !DuplicateHandle( GetCurrentProcess(), hPipeSrcWrite, GetCurrentProcess(), &hPipeSrcWriteDup, 0, false, DUPLICATE_SAME_ACCESS ) )
		return ;
	CloseHandle( hPipeSrcWrite );
	//2�������ڶ����ܵ������ڿͻ�����������˷�������
	HANDLE hPipeClientWrite=NULL, hPipeSrcRead=NULL;
	if( !CreatePipe( &hPipeSrcRead, &hPipeClientWrite, &attrib, 0) )
		return ;
	//���Ʋ��ɼ̳еĶ����������ԭ�еĶ�����ص�
	HANDLE hPipeSrcReadDup=NULL;
	if( !DuplicateHandle( GetCurrentProcess(), hPipeSrcRead, GetCurrentProcess(), &hPipeSrcReadDup, 0, false, DUPLICATE_SAME_ACCESS ) )
		return ;
	CloseHandle( hPipeSrcRead );
	//4�������ӽ���,
	if( !CreateFacePanelProcess( hPipeClientRead, hPipeClientWrite ) )
	{
		AfxMessageBox("��FacePanel.exeʧ��!");
		return ;
	}
	WriteToFacePanelClient(hPipeSrcWriteDup);
}

static CProcessPart* CreatePart(int idPartClsType,const char* key,void* pContext)
{
	CProcessPart* pPart=NULL;
	if(idPartClsType==CProcessPart::TYPE_LINEANGLE)
		pPart=new CProcessAngle();
	else if(idPartClsType==CProcessPart::TYPE_PLATE)
		pPart=new CProcessPlate();
	else
		pPart=new CProcessPart();	
	return pPart;
}
static BOOL DestroyPart(CProcessPart* pPart)
{
	if(pPart->m_cPartType==CProcessPart::TYPE_LINEANGLE)
		delete (CProcessAngle*)pPart;
	else if(pPart->m_cPartType==CProcessPart::TYPE_PLATE)
		delete (CProcessPlate*)pPart;
	else
		delete (CProcessPart*)pPart;
	return TRUE;
}

void ReadProcessPartsFromAttachBuffer(CBuffer* pAttachBuff,CSuperHashStrList<CProcessPart> &hashPartByPartNo)
{
	hashPartByPartNo.CreateNewAtom=CreatePart;
	hashPartByPartNo.DeleteAtom=DestroyPart;
	hashPartByPartNo.Empty();
	CBuffer *pPPEModelBuffer=pAttachBuff!=NULL?pAttachBuff:Ta.AttachBuffer.GetValue(CTower::PPE_MODEL);
	if(pPPEModelBuffer==NULL||pPPEModelBuffer->GetLength()<=0)
		return;
	pPPEModelBuffer->SeekToBegin();
	int nPart=pPPEModelBuffer->ReadInteger();
	CProcessPart* pProcessPart=NULL;
	CXhChar16 sPartNo;
	for(int i=0;i<nPart;i++)
	{
		BYTE cPartType=CProcessPart::RetrievedPartTypeAndLabelFromBuffer(*pPPEModelBuffer,sPartNo);
		CProcessPart *pProcessPart=hashPartByPartNo.Add(sPartNo,cPartType);
		pProcessPart->FromPPIBuffer(*pPPEModelBuffer);
	}
}

BOOL UpdateProcessPartsToAttachBuffer(CHashStrList<CProcessPart*> &hashPartPtrByLabel)
{
	CSuperHashStrList<CProcessPart> hashLegacyPartByPartNo;
	CAttachBuffer *pLegacyBuffer=Ta.AttachBuffer.Add(CTower::PPE_MODEL);
	ReadProcessPartsFromAttachBuffer(pLegacyBuffer,hashLegacyPartByPartNo);
	CProcessPart *pNewPart=NULL;
	for(CProcessPart** ppNewPart=hashPartPtrByLabel.GetFirst();ppNewPart;ppNewPart=hashPartPtrByLabel.GetNext())
	{
		pNewPart=*ppNewPart;
		CProcessPart* pLegacyPart=hashLegacyPartByPartNo.GetValue(pNewPart->GetPartNo());
		CBuffer buf(2048);
		if(pLegacyPart==NULL)
		{
			pLegacyPart=hashLegacyPartByPartNo.Add(pNewPart->GetPartNo(),pNewPart->m_cPartType);
			pLegacyPart->ClonePart(pNewPart);
		}
		else if(!pLegacyPart->IsEqual(pNewPart))
		{
			CProcessPart::BUFFER_VERSION serial=pLegacyPart->m_verSerial;
			pLegacyPart->ClonePart(pNewPart);
			serial.revision++;
			pLegacyPart->m_verSerial=serial;
		}
	}
	pLegacyBuffer->ClearContents();
	pLegacyBuffer->SetBlockSize(0x5000000);
	int nPart=hashLegacyPartByPartNo.GetNodeNum();
	pLegacyBuffer->WriteInteger(nPart);
	for(pNewPart=hashLegacyPartByPartNo.GetFirst();pNewPart;pNewPart=hashLegacyPartByPartNo.GetNext())
		pNewPart->ToPPIBuffer(*pLegacyBuffer);
	return TRUE;
}

bool PartToPPIBufferIncAttachInfo(CBuffer &buffer,CLDSPart *pPart,CProcessPart *pProcessPart)
{
	if(pPart==NULL||(!pPart->IsAngle()&&!pPart->IsPlate()))
		return false;
	buffer.ClearContents();
	if(pProcessPart&&pProcessPart->m_dwInheritPropFlag==0xFFFFFFFF)	//CProcessPart::PATTERN_OVERWRITE
		pProcessPart->ToPPIBuffer(buffer);
	else if(pProcessPart==NULL||pProcessPart->m_dwInheritPropFlag==0)
		pPart->ToPPIBuffer(buffer);	//��ȫ�̳�����ȫ������ά����ģ��
	else
	{
		pPart->ToPPIBuffer(buffer);
		buffer.SeekToBegin();
		CProcessPlate xPlate;
		CProcessAngle xAngle;
		CProcessPart* pNewCopyPart=NULL;
		if(pProcessPart->IsPlate())
		{
			pNewCopyPart=&xPlate;
			xPlate.FromPPIBuffer(buffer);
			if(pProcessPart->m_dwInheritPropFlag&0x0040)
			{	//0x40==CProcessPart::PATTERN_MKPOS
				xPlate.mcsFlg.wFlag=((CProcessPlate*)pProcessPart)->mcsFlg.wFlag;
				xPlate.mkpos=pProcessPart->mkpos;
			}
		}
		if(pProcessPart->IsAngle())
		{
			pNewCopyPart=&xAngle;
			xAngle.FromPPIBuffer(buffer);
		}
		if(pProcessPart->m_dwInheritPropFlag&0x0001)
		{	//0x40==CProcessPart::PATTERN_MATERIAL
			pNewCopyPart->cMaterial=pProcessPart->cMaterial;
		}
		else if(pProcessPart->m_dwInheritPropFlag&0x0002)
		{	//0x40==CProcessPart::PATTERN_SIZE
			pNewCopyPart->m_fThick=pProcessPart->m_fThick;
			if(pProcessPart->m_cPartType==CProcessPart::TYPE_LINEANGLE)
				pNewCopyPart->m_fWidth=pProcessPart->m_fWidth;
		}
		else if(pProcessPart->m_dwInheritPropFlag&CProcessPart::PATTERN_LENGTH)
		{	//0x40==CProcessPart::PATTERN_LENGTH
			pNewCopyPart->m_wLength=pProcessPart->m_wLength;
		}
		//else if(pProcessPart->m_dwInheritPropFlag&CProcessPart::PATTERN_COUNT)
		//{	//0x40==CProcessPart::PATTERN_COUNT
		//	pNewCopyPart->m_nDanJiNum=pProcessPart->m_nDanJiNum;
		//}
		buffer.ClearContents();
		pNewCopyPart->ToPPIBuffer(buffer);
	}
	return true;
}
//���ݹ����б��ȡPPEModel����������ģʽ
int WritePPEModelBuffer(CBuffer &buffer,PARTSET *pPartSet,bool bPartNoIncHandle=false)
{
	buffer.ClearContents();
	if(pPartSet==NULL||pPartSet->GetNodeNum()==0)
		return 0;
	buffer.WriteString("�������Ϲ���ϵͳ");
	buffer.WriteString("1.0.0.1");
	//������Ϣ
	buffer.WriteString(Ta.m_sCompanyName);		//��Ƶ�λ
	buffer.WriteString(Ta.m_sPrjCode);			//���̱��
	buffer.WriteString(Ta.m_sPrjName);			//��������
	buffer.WriteString(Ta.m_sTaType);			//����
	buffer.WriteString(Ta.m_sTaAlias);			//����
	buffer.WriteString(Ta.m_sTaStampNo);		//��ӡ��
	buffer.WriteString(Ta.m_sOperator);			//����Ա���Ʊ��ˣ�
	buffer.WriteString(Ta.m_sAuditor);			//�����
	buffer.WriteString(Ta.m_sCritic);			//������
	//�����б�
	BUFFERPOP stack(&buffer,pPartSet->GetNodeNum());
	buffer.WriteDword(pPartSet->GetNodeNum());
	CBuffer partBuffer;
	CSuperHashStrList<CProcessPart> hashPartByPartNo;
	ReadProcessPartsFromAttachBuffer(NULL,hashPartByPartNo);
	for(CLDSPart *pPart=pPartSet->GetFirst();pPart;pPart=pPartSet->GetNext())
	{
		if(!pPart->IsAngle()&&!pPart->IsPlate())
			continue;
		CXhChar16 sOldPartNo=pPart->GetPartNo();
		if(bPartNoIncHandle)
			pPart->SetPartNo(CXhChar16("%s#0x%X",(char*)sOldPartNo,pPart->handle));
		CProcessPart *pProcessPart=hashPartByPartNo.GetValue(pPart->GetPartNo());
		PartToPPIBufferIncAttachInfo(partBuffer,pPart,pProcessPart);
		buffer.Write(partBuffer.GetBufferPtr(),partBuffer.GetLength());
		if(bPartNoIncHandle)
			pPart->SetPartNo(sOldPartNo);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
		CLDSObject::Log2File()->Log("�����������ִ���");
	return buffer.GetLength();
}

bool GetLocalFileVersion(const char* filepathname, char* pszVersion, UINT uiMaxStrBufLen)
{
	VS_FIXEDFILEINFO *pVsInfo;
	unsigned int iFileInfoSize = sizeof(VS_FIXEDFILEINFO);
	DWORD dwVerInfoSize = GetFileVersionInfoSize(filepathname, NULL);
	if (dwVerInfoSize == 0)
		return 0;
	CHAR_ARRAY bufpool(dwVerInfoSize + 1);
	CXhString szVersion(pszVersion, uiMaxStrBufLen);
	if (GetFileVersionInfo(filepathname, 0, dwVerInfoSize, (char*)bufpool))
	{
		if (VerQueryValue(bufpool, "\\", (void **)&pVsInfo, &iFileInfoSize))
		{
			szVersion.Printf("%d.%d.%d.%d", HIWORD(pVsInfo->dwProductVersionMS), LOWORD(pVsInfo->dwProductVersionMS), HIWORD(pVsInfo->dwProductVersionLS), LOWORD(pVsInfo->dwProductVersionLS));
			return true;
		}
	}
	return false;
}
static bool GetPPEClientVersion(char* pszVersion, UINT uiMaxStrBufLen)
{
	TCHAR szFileName[MAX_PATH] = "E:\\Structure\\PPE\\Debug\\";
	GetSysPath(szFileName);
	strcat(szFileName, "PPE.exe");
	return GetLocalFileVersion(szFileName, pszVersion, uiMaxStrBufLen);
}

//���Ʊ�׼ģ�壨����PPE�ӳ���
BOOL WriteToPPEClient(HANDLE hPipeWrite,PARTSET *pPartSet,bool bWaitReturn)
{
	if( hPipeWrite == INVALID_HANDLE_VALUE )
	{
		AfxMessageBox("��ȡ�ܵ������Ч\n");
		return FALSE;
	}
	BYTE cProductType=PRODUCT_TMA;
#ifdef __TMA_
	cProductType=PRODUCT_TMA;
#elif defined(__LMA_)
	cProductType=PRODUCT_LMA;
#elif defined(__LDS_)
	cProductType=PRODUCT_LDS;
#elif defined(__TDA_)
	cProductType=PRODUCT_TDA;
/*#elif defined(__TSA_)
	cProductType=PRODUCT_TSA;
*/
#endif
	CXhChar50 szVersion;
	GetPPEClientVersion(szVersion, 51);
	CBuffer buffer(1000);	//1Mb
	//1.д����ܹ�֤����Ϣ
	buffer.WriteByte(cProductType);				//��Ʒ����
	buffer.WriteDword(theApp.m_version[0]);		//�汾��Ϣ
	buffer.WriteDword(theApp.m_version[1]);
	CXhChar200 szLicFilePath = APP_PATH;
	if (compareVersion(szVersion, "1.2.0.1") >= 0)
	{
		UINT uiProcessAuthNumer=GetCurrentProcessSerialNumber();
		szLicFilePath.Append(CXhChar16("%d", uiProcessAuthNumer), '|');
	}
	buffer.WriteString(szLicFilePath,201);		//֤��·��
	//2.д���ӳ�������ģʽ
	//2.1 ��֤����������ֻ�иְ�ͽǸ��๹��
	if(pPartSet)
	{
		for(CLDSPart *pPart=pPartSet->GetFirst();pPart;pPart=pPartSet->GetNext())
		{
			if(!pPart->IsPlate()&&!pPart->IsAngle())
				pPartSet->DeleteCursor(TRUE);
		}
	}
	//2.2 ���ݹ��������ж�����ģʽ
	BYTE mode=0;	//0.������Ϊ�ӽ�������;1.�������޷���;2.����������ģʽ;3.�๹���޷���;4.�๹������ģʽ;5.���������αȶ�ģʽ
	if(pPartSet==NULL||pPartSet->GetNodeNum()==0)
		mode=0;
	else if(pPartSet->GetNodeNum()==1)
		mode=bWaitReturn?2:1;
	else if(pPartSet->GetNodeNum()==2&&pPartSet->GetFirst()->GetClassTypeId()==pPartSet->GetNext()->GetClassTypeId())
		mode=5;
	else 
		mode=bWaitReturn?7:3;	//TMA����PPEʱʹ��ģʽ7(����ģʽ4��洢����������Ϣģʽ) wht 16-03-18
	buffer.WriteByte(mode);
	//3.д�빤�չ�����
	CBuffer ppeModeBuf;
	if(mode==3||mode==7)
	{	//�๹������ģʽ��ͬ��Ź���������һ������
		CHashStrList<long> hashHandleByPartNo;
		for(CLDSPart *pPart=pPartSet->GetFirst();pPart;pPart=pPartSet->GetNext())
		{
			if(strlen(pPart->GetPartNo())>0&&hashHandleByPartNo.GetValue(pPart->GetPartNo())==NULL)
				hashHandleByPartNo.SetValue(pPart->GetPartNo(),pPart->handle);
			else
				pPartSet->DeleteCursor(TRUE);
		}
	}
	WritePPEModelBuffer(ppeModeBuf,pPartSet,mode==5);
	buffer.WriteInteger(ppeModeBuf.GetLength());
	if(ppeModeBuf.GetLength()>0)
		buffer.Write(ppeModeBuf.GetBufferPtr(),ppeModeBuf.GetLength());
	//�������ܵ���д������
	return buffer.WriteToPipe(hPipeWrite,1024);
}

static HANDLE _hLocalStartPPEProcess=NULL;
BOOL CreatePPEProcess( HANDLE hClientPipeRead, HANDLE hClientPipeWrite,BOOL bNeedResponse)
{
	TCHAR cmd_str[MAX_PATH]="E:\\Structure\\PPE\\Debug\\";
	GetSysPath(cmd_str);
	strcat(cmd_str,"PPE.exe -child");

	STARTUPINFO startInfo;
	memset( &startInfo, 0 , sizeof( STARTUPINFO ) );

	startInfo.cb= sizeof( STARTUPINFO );
	startInfo.dwFlags |= STARTF_USESTDHANDLES;
	startInfo.hStdError= 0;//hPipeWrite;
	startInfo.hStdInput= hClientPipeRead;
	startInfo.hStdOutput= hClientPipeWrite;

	PROCESS_INFORMATION processInfo;
	memset( &processInfo, 0, sizeof(PROCESS_INFORMATION) );

	BOOL b=CreateProcess( NULL,cmd_str,
		NULL,NULL, TRUE,CREATE_NEW_CONSOLE, NULL, NULL,&startInfo,&processInfo );
	DWORD er=GetLastError();
	if(b&&bNeedResponse)
		_hLocalStartPPEProcess=processInfo.hProcess;
	return b;
}
void CLDSView::OnPPEStarter()
{
	PPEStarter();
}
int ReadFromClient(HANDLE hPipeRead,CBuffer& partBuffer)
{
	if( hPipeRead == INVALID_HANDLE_VALUE)
	{
		logerr.Log("��ȡ�ܵ������Ч\n");
		return -1;
	}
	//�������ܵ��ж�ȡ���ݣ���ʾ�������ظ�������
	CBuffer buffer(10000);	//10Kb
	if(!buffer.ReadFromPipe(hPipeRead,1024))
		return -1;
	if(buffer.GetLength()<=0)
		return -1;
	buffer.SeekToBegin();
	//1.��ȡ��־λ
	BYTE continueFlag=0;
	buffer.ReadByte(&continueFlag);
	if(continueFlag==0)
		return 0;//FALSE;
	//2.��ȡ���ӻ���
	long partBufferLen=0;
	buffer.ReadInteger(&partBufferLen);
	if(partBufferLen>0)
	{
		partBuffer.SetBlockSize(partBufferLen);
		partBuffer.Write(buffer.GetCursorBuffer(),partBufferLen);
		partBuffer.SeekToBegin();
	}
	return 1;//TRUE;
}

static DWORD WINAPI ReadPpeResonseData(LPVOID lpParameter)
{	//��ȡ����ֵ
	HANDLE hPipeSrcReadDup=(HANDLE)lpParameter;
	CBuffer ppemodelbuf;
	if(ReadFromClient(hPipeSrcReadDup,ppemodelbuf)==1)
	{	//���¸��ӻ�������(����������ģʽ�������й����������ݸ�������) wjh-2017.1.17
		CSuperHashStrList<CProcessPart> hashNewPartByPartNo,hashLegacyPartByPartNo;
		ReadProcessPartsFromAttachBuffer(&ppemodelbuf,hashNewPartByPartNo);
		CAttachBuffer *pLegacyBuffer=Ta.AttachBuffer.Add(CTower::PPE_MODEL);
		ReadProcessPartsFromAttachBuffer(pLegacyBuffer,hashLegacyPartByPartNo);
		for(CProcessPart* pNewPart=hashNewPartByPartNo.GetFirst();pNewPart;pNewPart=hashNewPartByPartNo.GetNext())
		{
			CProcessPart* pLegacyPart=hashLegacyPartByPartNo.GetValue(pNewPart->GetPartNo());
			CBuffer buf(2048);
			if(pLegacyPart==NULL)
			{
				pLegacyPart=hashLegacyPartByPartNo.Add(pNewPart->GetPartNo(),pNewPart->m_cPartType);
				pLegacyPart->ClonePart(pNewPart);
			}
			else if(!pLegacyPart->IsEqual(pNewPart))
			{
				CProcessPart::BUFFER_VERSION serial=pLegacyPart->m_verSerial;
				pLegacyPart->ClonePart(pNewPart);
				serial.revision++;
				pLegacyPart->m_verSerial=serial;
			}
		}
		pLegacyBuffer->ClearContents();
		pLegacyBuffer->SetBlockSize(ppemodelbuf.GetLength());
		int nPart=hashLegacyPartByPartNo.GetNodeNum();
		pLegacyBuffer->WriteInteger(nPart);
		for(pNewPart=hashLegacyPartByPartNo.GetFirst();pNewPart;pNewPart=hashLegacyPartByPartNo.GetNext())
			pNewPart->ToPPIBuffer(*pLegacyBuffer);
		/*
#ifdef __CONNECT_REMOTE_SERVER_
		if(theApp.m_bNetModelWork)
		{
			CRemoteServerDlg *pRemoteServerPage = ((CMainFrame*)theApp.m_pMainWnd)->GetRemoteServerPage();
			pRemoteServerPage->RefreshTreeCtrl();
		}
#endif
		*/
	}
	_hLocalStartPPEProcess=NULL;
	return TRUE;
}
void CLDSView::PPEStarter(PARTSET *pPartSet/*=NULL*/,bool bNeedResponse/*=false*/)
{
	if(!PRODUCT_FUNC::IsHasPPE())
		return;	//��Ȩ֤���в����С����չ����������ܵ�ʹ����Ȩ
	if(_hLocalStartPPEProcess!=NULL&&bNeedResponse)
	{
		AfxMessageBox("��ǰģʽ�£������ظ���������PPE���չ������ӳ���");
		return;
	}
	//1��������һ���ܵ�: ���ڷ���������ͻ��˷�������
	SECURITY_ATTRIBUTES attrib;
	attrib.nLength = sizeof( SECURITY_ATTRIBUTES );
	attrib.bInheritHandle= true;
	attrib.lpSecurityDescriptor = NULL;
	HANDLE hPipeClientRead=NULL, hPipeSrcWrite=NULL;
	if( !CreatePipe( &hPipeClientRead, &hPipeSrcWrite, &attrib, 0 ) )
		return ;
	//��Ϊ��ʱ����������ɼ̳У��ʿɸ���һ�����ɼ̳е�д�������ԭ����д����ص�
	HANDLE hPipeSrcWriteDup=NULL;
	if( !DuplicateHandle( GetCurrentProcess(), hPipeSrcWrite, GetCurrentProcess(), &hPipeSrcWriteDup, 0, false, DUPLICATE_SAME_ACCESS ) )
		return ;
	CloseHandle( hPipeSrcWrite );
	//2�������ڶ����ܵ������ڿͻ�����������˷�������
	HANDLE hPipeClientWrite=NULL, hPipeSrcRead=NULL;
	if( !CreatePipe( &hPipeSrcRead, &hPipeClientWrite, &attrib, 0) )
		return ;
	//���Ʋ��ɼ̳еĶ����������ԭ�еĶ�����ص�
	HANDLE hPipeSrcReadDup=NULL;
	if( !DuplicateHandle( GetCurrentProcess(), hPipeSrcRead, GetCurrentProcess(), &hPipeSrcReadDup, 0, false, DUPLICATE_SAME_ACCESS ) )
		return ;
	CloseHandle( hPipeSrcRead );
	//3�������ӽ���,
	if( !CreatePPEProcess( hPipeClientRead, hPipeClientWrite,bNeedResponse ) )
	{

		AfxMessageBox("��PPE.exeʧ��!");
		return ;
	}
	//4��ͨ���ܵ���ͻ���ͨѶ
	BOOL bRetCode=WriteToPPEClient(hPipeSrcWriteDup,pPartSet,bNeedResponse);
	if(!bRetCode)
	{
		if(bNeedResponse)
			_hLocalStartPPEProcess=NULL;
		return;
	}
	if(bNeedResponse)
	{
		DWORD ThreadId;
		HANDLE ThreadHandle=CreateThread(NULL,0,ReadPpeResonseData,hPipeSrcReadDup,0,&ThreadId);
		CloseHandle(ThreadHandle);
	}
}
void CLDSView::OnUnifyAngleBoltsParamG(){UnifyAngleBoltsParamG();}
BOOL UnifyAngleBoltParamG(DESIGN_LS_POS &designLsPos);
void CLDSView::UnifyAngleBoltsParamG()
{
	CString cmdStr;
	CLDSBolt *pBolt=NULL;
	long *id_arr;
	int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	CUndoOperObject undo(&Ta);
	for(int i=0;i<n;i++)
	{
		if((pBolt=(CLDSBolt*)Ta.Parts.FromHandle(id_arr[i],CLS_BOLT))==NULL)
			continue;
		if(UnifyAngleBoltParamG(pBolt->des_base_pos))
			pBolt->SetModified(TRUE,FALSE);
	}
}
//UnifyAngleBoltParamG����void CLDSView::UnifyAngleBoltsParamG()֮�󣬷���VSS����Ա� wht 16-10-09
BOOL UnifyAngleBoltParamG(DESIGN_LS_POS &designLsPos)
{
	CLDSPoint* pPoint=&designLsPos.datumPoint;
	if(pPoint->datum_pos_style==3)
	{
		CLDSLineAngle* pAngle1=(CLDSLineAngle*)Ta.Parts.FromHandle(pPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEANGLE);
		CLDSLineAngle* pAngle2=(CLDSLineAngle*)Ta.Parts.FromHandle(pPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE);
		if(pAngle1&&pAngle2)
		{
			GEPOINT lineVec1=pAngle1->End()-pAngle1->Start();
			GEPOINT lineVec2=pAngle2->End()-pAngle2->Start();
			GEPOINT norm=lineVec1^lineVec2;
			normalize(norm);
			JGZJ jgzj1=pAngle1->GetZhunJu('X');
			JGZJ jgzj2=pAngle2->GetZhunJu('X');
			if(fabs(norm*pAngle1->vxWingNorm)<fabs(norm*pAngle1->vyWingNorm))
				jgzj1=pAngle1->GetZhunJu('Y');
			if(fabs(norm*pAngle2->vxWingNorm)<fabs(norm*pAngle2->vyWingNorm))
				jgzj2=pAngle2->GetZhunJu('Y');
			if(pPoint->des_para.AXIS_INTERS.wing_offset_style1==4&&pPoint->des_para.AXIS_INTERS.wing_offset_dist1==jgzj1.g)
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=0;
			else if(pPoint->des_para.AXIS_INTERS.wing_offset_style1==4&&pPoint->des_para.AXIS_INTERS.wing_offset_dist1==jgzj1.g1&&jgzj1.g1>0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=1;
			else if(pPoint->des_para.AXIS_INTERS.wing_offset_style1==4&&pPoint->des_para.AXIS_INTERS.wing_offset_dist1==jgzj1.g2&&jgzj1.g2>0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=2;
			else if(pPoint->des_para.AXIS_INTERS.wing_offset_style1==4&&pPoint->des_para.AXIS_INTERS.wing_offset_dist1==jgzj1.g3&&jgzj1.g3>0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=3;
			else if(pPoint->des_para.AXIS_INTERS.wing_offset_style1>0&&pPoint->des_para.AXIS_INTERS.wing_offset_dist1==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=4;
			if(pPoint->des_para.AXIS_INTERS.wing_offset_style2==4&&pPoint->des_para.AXIS_INTERS.wing_offset_dist2==jgzj2.g)
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=0;
			else if(pPoint->des_para.AXIS_INTERS.wing_offset_style2==4&&pPoint->des_para.AXIS_INTERS.wing_offset_dist2==jgzj2.g1&&jgzj2.g1>0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=1;
			else if(pPoint->des_para.AXIS_INTERS.wing_offset_style2==4&&pPoint->des_para.AXIS_INTERS.wing_offset_dist2==jgzj2.g2&&jgzj2.g2>0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=2;
			else if(pPoint->des_para.AXIS_INTERS.wing_offset_style2==4&&pPoint->des_para.AXIS_INTERS.wing_offset_dist2==jgzj2.g3&&jgzj2.g3>0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=3;
			else if(pPoint->des_para.AXIS_INTERS.wing_offset_style2==4&&pPoint->des_para.AXIS_INTERS.wing_offset_dist2==jgzj2.g)
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=0;
			else if(pPoint->des_para.AXIS_INTERS.wing_offset_style2>0&&pPoint->des_para.AXIS_INTERS.wing_offset_dist2==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=4;
		}
		else
			return FALSE;
	}
	else if(pPoint->datum_pos_style==1||pPoint->datum_pos_style==2)
	{
		long hAngle=pPoint->des_para.RODEND.hRod;
		short direction = pPoint->des_para.RODEND.direction;
		short offset_wing=pPoint->des_para.RODEND.offset_wing;	//0.X֫;1.Y֫
		short wing_offset_style=pPoint->des_para.RODEND.wing_offset_style;
		double wing_offset_dist=pPoint->des_para.RODEND.wing_offset_dist;
		CLDSNode* pDatumNode=NULL;
		if(pPoint->datum_pos_style==2)
		{
			hAngle=pPoint->des_para.RODNODE.hRod ;
			pDatumNode=Ta.Node.FromHandle(pPoint->des_para.RODNODE.hNode);
			direction = pPoint->des_para.RODNODE.direction;
			offset_wing=pPoint->des_para.RODNODE.offset_wing;	//0.X֫;1.Y֫
			wing_offset_style=pPoint->des_para.RODNODE.wing_offset_style;
			wing_offset_dist=pPoint->des_para.RODNODE.wing_offset_dist;
		}
		if((wing_offset_dist!=0||wing_offset_style<4) && offset_wing!=designLsPos.offset_wing)
			return FALSE;	//��׼�㶨λ֫����˨��λ֫��һ��
		CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(hAngle,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pAngle==NULL)
			return FALSE;
		if(pPoint->datum_pos_style==1&&direction==0)
			pDatumNode=pAngle->pStart;
		else if(pPoint->datum_pos_style==1&&direction==1)
			pDatumNode=pAngle->pEnd;
		if(wing_offset_style==0)
		{	//��׼׼��
			if(offset_wing==0)
				wing_offset_dist=pAngle->GetNodeWingXG(pDatumNode);
			else
				wing_offset_dist=pAngle->GetNodeWingYG(pDatumNode);
		}
		else if(wing_offset_style!=4)
		{
			if(offset_wing==0)
				wing_offset_dist=pAngle->GetWingXG(wing_offset_style);
			else
				wing_offset_dist=pAngle->GetWingYG(wing_offset_style);
		}

		if(pPoint->datum_pos_style==1)
		{
			pPoint->des_para.RODEND.wing_offset_dist=0;
			pPoint->des_para.RODEND.wing_offset_style=4;
		}
		else //if(pPoint->datum_pos_style==2)
		{
			pPoint->des_para.RODNODE.wing_offset_dist=0;
			pPoint->des_para.RODNODE.wing_offset_style=4;
		}
		JGZJ jgzj;
		if(offset_wing==0)
			jgzj=pAngle->GetZhunJu('X');
		else
			jgzj=pAngle->GetZhunJu('Y');
		if(pPoint->datum_pos_style==2&&pPoint->des_para.RODNODE.bFaceOffset)
		{	//�˼��ڵ㶨λƽ��ʱƫ����Ӧͳһ����׼����(ƫ������designLsPos��ʱ�ᵼ��ƽ����ͶӰû������) wht 17-02-20
			pPoint->des_para.RODNODE.wing_offset_dist=wing_offset_dist;
			if(pPoint->des_para.RODNODE.wing_offset_dist==jgzj.g)
				pPoint->des_para.RODNODE.wing_offset_style=0;
			else if(pPoint->des_para.RODNODE.wing_offset_dist==jgzj.g1&&pPoint->des_para.RODNODE.wing_offset_dist>0)
				pPoint->des_para.RODNODE.wing_offset_style=1;
			else if(pPoint->des_para.RODNODE.wing_offset_dist==jgzj.g2&&pPoint->des_para.RODNODE.wing_offset_dist>0)
				pPoint->des_para.RODNODE.wing_offset_style=2;
			else if(pPoint->des_para.RODNODE.wing_offset_dist==jgzj.g3&&pPoint->des_para.RODNODE.wing_offset_dist>0)
				pPoint->des_para.RODNODE.wing_offset_style=3;
			else
				pPoint->des_para.RODNODE.wing_offset_style=4;
		}
		else
		{
		designLsPos.wing_offset_dist=wing_offset_dist+designLsPos.wing_offset_dist;
		if(designLsPos.wing_offset_dist==jgzj.g)
			designLsPos.m_biWingOffsetStyle=0;
		else if(designLsPos.wing_offset_dist==jgzj.g1&&designLsPos.wing_offset_dist>0)
			designLsPos.m_biWingOffsetStyle=1;
		else if(designLsPos.wing_offset_dist==jgzj.g2&&designLsPos.wing_offset_dist>0)
			designLsPos.m_biWingOffsetStyle=2;
		else if(designLsPos.wing_offset_dist==jgzj.g3&&designLsPos.wing_offset_dist>0)
			designLsPos.m_biWingOffsetStyle=3;
		else
			designLsPos.m_biWingOffsetStyle=4;
		}
	}
	return TRUE;
}

void CLDSView::OnSwapBoltG()
{
#ifndef AFX_TARG_ENU_ENGLISH
	g_pBoltInfoDlg->ActivateDialog();
#endif
}
void CLDSView::OnRetrievePartSegI()
{
	long *id_arr=NULL, nCount=0;
	nCount=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(nCount<1)
		return;
	PARTSET partSet;
	for(int i=0;i<nCount;i++)
	{
		CLDSPart *pPart = console.FromPartHandle(id_arr[i]);
		if(pPart==NULL)
			continue;
		else	
			partSet.append(pPart);
	}
	RetrievePartsSegIFromPartNo(partSet);
}
//
#include "DataCompareDlg.h"
void CLDSView::OnDataCompare()
{
#if defined(__TMA_)||defined(__LMA_)
	if( PRODUCT_FUNC::IsHasInternalTest()||(
		GetLicVersion()>=1000003&&VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM)))
	{
		g_pDataCompareDlg->InitDataCmpDlg();
	}
#endif
}

void CLDSView::OnRevisionMode()
{
	if(PRODUCT_FUNC::IsHasInternalTest())
		theApp.m_bManuRevisionMode=!theApp.m_bManuRevisionMode;
	else
		theApp.m_bManuRevisionMode=false;
}


void CLDSView::OnUpdateRevisionMode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theApp.m_bManuRevisionMode);
}


void CLDSView::OnSizeDimension()
{
	// TODO: �ڴ���������������
}
#include "LocalFeatureDef.h"
#include "EmbedLabelInputPanel.h"
int LabelInputInCmdPanel(CCmdLineDlg *pCmdLine,PARTSET* pPartSet)
{
	CLogErrorLife life(&logerr);
	CEmbedLabelInputPanel xCmdPanel;
	xCmdPanel.SetPartSet(pPartSet);
	pCmdLine->AttachCmdOptionPanel(&xCmdPanel);
	CString cmdPromptStr="ȷ�ϼ��ż������Ϣ��[ȷ�ϲ�¼����һ��(N)/�ж��˳�(C)]<N>:";
	pCmdLine->FillCmdLine(cmdPromptStr,"");
	while(1)
	{
		CString cmdStr;
		char cDefaultType='N';
		if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"N|C"))
		{
			pCmdLine->m_bGettingStrFromCmdLine=FALSE;
			//g_pSolidSnap->SetSnapType(SNAP_ALL);
			pCmdLine->DetachCmdOptionPanel();
			return -1;
		}
		pCmdLine->m_bGettingStrFromCmdLine=FALSE;
		if(cmdStr.CompareNoCase("C")==0)
			break;
		//else if(cmdStr.CompareNoCase("N")==0)
		//	cDefaultType='N';
		pCmdLine->FillCmdLine(cmdPromptStr, "");
		if (xCmdPanel.ConfirmAndNextLabelGroup())
			break;
	}
	pCmdLine->m_bGettingStrFromCmdLine = FALSE;
	pCmdLine->DetachCmdOptionPanel();
	g_pSolidDraw->ReleaseSnapStatus();
	pCmdLine->FinishCmdLine();
	return 0;
}
int CLDSView::LabelInput() 
{
#if defined(__TMA_)||defined(__LMA_)
	if (!VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM)&&::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)==0)
		return -1;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	pCmdLine->FinishCmdLine();
	long *id_arr=NULL;
	long i,n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	CLDSPart* pPart;
	PARTSET partset;
	for(i=0;i<n;i++)
	{
		if((pPart=Ta.Parts.FromHandle(id_arr[i]))==NULL)
			continue;
		if(pPart->IsLinePart())//||pPart->IsPlate())
			partset.append(pPart);
	}
	if(partset.GetNodeNum()==0)
	{
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		DISPLAY_TYPE disp_type;
		g_pSolidSet->GetDisplayType(&disp_type);
		if(disp_type!=DISP_SOLID)
		{
			g_pSolidSet->SetDisplayType(DISP_SOLID);
			g_pSolidDraw->Draw();
		}
		GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECTINDEX_LINEANGLE)|GetSingleWord(SELECTINDEX_LINETUBE),CLS_LINEPART,0,2);
		scr_part_para.disp_type=DISP_SOLID;
		scr_part_para.nResultObjs=2;	//����2�����ѡ
#ifdef AFX_TARG_ENU_ENGLISH
		scr_part_para.cmdStr="please select the part that you want to input label:";
		scr_part_para.cmdErrorFeedbackStr="invalid part or more parts selected, please select again:";
#else
		scr_part_para.cmdStr="��ѡ����Ҫ��ŵĹ���:";
		scr_part_para.cmdErrorFeedbackStr="ѡ���˴���Ĺ�����ѡ���˶��������������ѡ��:";
#endif
		if(!GetPartsFromScr(scr_part_para))
			return 0; 
		OperOther();
		for(int i=0;i<scr_part_para.nResultObjs;i++)
		{
			CLDSDbObject *pObj=scr_part_para.pResultObjsArr[i];
			if(pObj&&pObj->IsLinePart())
				partset.append((CLDSPart*)pObj);
		}
	}
	if(partset.GetNodeNum()>0)
		return LabelInputInCmdPanel(pCmdLine,&partset);
	/*{
		CLabelOperDlg labeldlg;
		labeldlg.SetPartSet(&partset);
		labeldlg.DoModal();
	}
	*/
#endif
	return 0;
}
void CLDSView::OnLabelInput()
{
	Command("Label");
}
#include "EmendBoltGasket.h"
int CLDSView::WasherRevision()
{
	if(!PRODUCT_FUNC::IsHasInternalTest())
		return -1;
	//���Ե�������޶�
	DATACENTERCMD_CONTEXT context;
	CTaskOutputList infoList;
	context.m_pOutputInfoList=&infoList;
	context.m_pMainWnd=theApp.m_pMainWnd;
	context.nMaxWarnings=1000;//cmdRoutineChk.nMaxWarnings=dlg.m_nMaxWarnings;
	CLogErrorLife loglife(&logerr);
	CWaitCursor wait;
	CEmendBoltGasket emendBoltGasket(&Ta);
	emendBoltGasket.Execute(&context);
	emendBoltGasket.InitOutputInfoList(&infoList);
	((CMainFrame*)theApp.m_pMainWnd)->ShowTaskInfoList(infoList);

	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	pCmdLine->FinishCmdLine();
	return 0;
}
void CLDSView::OnWasherRevision()
{
	Command("Washer");
}

#ifdef __COMMON_PART_INC_
int CLDSView::AddAnchorBolts()
{
	long *id_arr=NULL;
	if(g_pSolidSnap->GetLastSelectEnts(id_arr)!=1)
		return -1;
	SmartPartPtr pBasePart = Ta.FromPartHandle(id_arr[0]);
	if(!pBasePart.blValid||!pBasePart->IsPlate())
		return -1;
	CUndoOperObject undo(&Ta);
	CLsRef* pLsRef;
	if(!pBasePart.pBasicPlate->IsMaybeBasePlate())
	{
		AfxMessageBox("��ǰ�ְ��ϴ��ڷǵؽ���˨��");
		return -1;	//�ǵ�����
	}
	//��ȡ��ǰ���������ڵĺ���-����-���޺���Ϣ
	BYTE ciHeightSerial=1,ciLegSerial=1,ciLegQuad=1;
	/*if(!pBasePart->IsLegObj())
	{
		AfxMessageBox("ѡ�е����岻�����ȹ���,ͼ�����д���");
		return -1;
	}*/
	int i,count=0;
	for(i=0;i<24;i++)
	{
		if(!pBasePart->cfgword.IsHasBodyNo(i+1))
			continue;
		ciHeightSerial=i+1;
		break;
	}
	CLogErrorLife life(&logerr);
	BYTE xarrLegSerials[24]={0};
	if((count=pBasePart->cfgword.GetExistLegSerials(xarrLegSerials,24))>1)
		logerr.Log("��ǰ��ѡ�ְ���ض���(��)�ȵ�ר�������壡");
	if(count>0)
		ciLegSerial=xarrLegSerials[0];
	ciLegQuad=(BYTE)CalPointQuadPos(pBasePart->ucs.origin);
	//��ӻ���¶�Ӧ�Ļ�����Ϣ
	GEPOINT xSummBasePoint,xAnchorLocation;
	bool blAddFirstFoundation=Ta.Foundations.GetNodeNum()==0;
	CLDSFoundation* pFoundation=Ta.AddFoundation(ciHeightSerial,ciLegSerial,pBasePart->ciLegQuad);
	pFoundation->CopyModuleInstanceInfo(pBasePart);
	pFoundation->vWorkNormal=pBasePart.pBasicPlate->ucs.axis_z;
	pFoundation->vProjAxisX=pBasePart.pBasicPlate->ucs.axis_x;
	GECS lcs(pFoundation->location,pFoundation->vProjAxisX,GEPOINT(0,1,0),pFoundation->vWorkNormal);
	lcs.axis_y=lcs.axis_z^lcs.axis_x;
	normalize(lcs.axis_y);
	normalize(lcs.axis_z);
	lcs.axis_x=lcs.axis_y^lcs.axis_z;
	lcs.origin=pBasePart->ucs.origin;	//���Ե����嶨λԭ����Ϊ������������ϵԭ���ʼֵ
	//���ݵ������ϵ�������˨��ӵؽ���˨��ͬʱ����ؽ���˨�����ĵ㣬�Ա���Ϊ���������ն�λ����
	WORD wiExistBoltD=0,wiExistBoltHoleD=0;
	CLDSAnchorBolt* pAnchorBolt;
	CLDSFoundation::LOCATION* pLocation;
	
	for(pLsRef=pBasePart.pBasicPlate->GetFirstLsRef(),count=0;pLsRef;pLsRef=pBasePart.pBasicPlate->GetNextLsRef())
	{
		if(!pBasePart.pBasicPlate->GetBoltIntersPos(pLsRef->GetLsPtr(),xAnchorLocation))
			continue;
		count++;
		xSummBasePoint+=xAnchorLocation;
		xAnchorLocation=lcs.TransPToCS(xAnchorLocation);
		pLocation=pFoundation->anchors.Append(CLDSFoundation::LOCATION(ftoi(xAnchorLocation.x),ftoi(xAnchorLocation.y),pLsRef->GetLsPtr()->handle));
		pAnchorBolt=Ta.AppendAnchorBolt();
		pAnchorBolt->m_hRelaVirtualBolt=pLsRef->GetLsPtr()->handle;
		pLocation->hAnchorBolt=pAnchorBolt->handle;
		pAnchorBolt->ucs.origin=lcs.TransPFromCS(GEPOINT(pLocation->siPosX,pLocation->siPosY));
		if(wiExistBoltD==0)
		{
			wiExistBoltD=pLsRef->pBolt->d;
			if(blAddFirstFoundation)
			{
				Ta.m_xFoundation.wiBoltD=wiExistBoltD;
				wiExistBoltHoleD=ftoi(pLsRef->pBolt->d+pLsRef->pBolt->hole_d_increment);
			}
			else if(Ta.m_xFoundation.wiBoltD!=wiExistBoltD)
			{
				logerr.Log("�������ϵ�����ؽ���˨��ģ�������趨�ĵؽ���˨ֱ������ͬM%d(��ǰ)��M%d(ģ��)",wiExistBoltD, Ta.m_xFoundation.wiBoltD);
				if (fabs(Ta.m_xFoundation.wiBasePlateHoleD - (pLsRef->pBolt->d + pLsRef->pBolt->hole_d_increment))<2)
				{
					wiExistBoltD = Ta.m_xFoundation.wiBoltD;
					wiExistBoltHoleD = Ta.m_xFoundation.wiBasePlateHoleD;
				}
			}
		}
		else if(wiExistBoltD!=pLsRef->pBolt->d)
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"ͬһ�������ϵ�����ؽ���˨�趨ֱ������ͬM%d(��ǰ)��M%d(֮ǰ)", pLsRef->pBolt->d,wiExistBoltD);

		pAnchorBolt->d=pLsRef->pBolt->d;
		pAnchorBolt->ucs=pLsRef->pBolt->ucs;
		pAnchorBolt->ucs.origin.z=lcs.origin.z;	//�䲻�����޷�����б���ţ������Դ�����˨���㲻�ڵ������ wjh-2020.1.8
		if(pAnchorBolt->ucs.axis_z.z>0)
		{	//�ؽ���˨һ������
			pAnchorBolt->ucs.axis_z*=-1.0;
			pAnchorBolt->ucs.axis_y*=-1.0;
		}
		pAnchorBolt->SetLayer(CXhChar16("LB%d",ciLegQuad));
		pAnchorBolt->ciLegQuad=ciLegQuad;
		pAnchorBolt->CopyModuleInstanceInfo(pBasePart);
		int i,count = ANCHOR_BOLT::GetCount();
		double dfTolerance = 100;
		const ANCHOR_BOLT* pAnchorBoltInfo=NULL, *pAdaptAnchorInfo = NULL;
		for (i = 0; i < count; i++)
		{
			const ANCHOR_BOLT* pAnchorBoltInfo = ANCHOR_BOLT::RecordAt(i);// pAnchorBolt->d);
			if (abs(pAnchorBoltInfo->wiBasePlateHoleD - wiExistBoltHoleD)< dfTolerance)
			{
				dfTolerance = fabs((double)pAnchorBoltInfo->wiBasePlateHoleD - wiExistBoltHoleD);
				pAdaptAnchorInfo = pAnchorBoltInfo;
			}
		}
		pAnchorBoltInfo = pAdaptAnchorInfo;
		if (pAnchorBoltInfo)
		{ 
			wiExistBoltD = pAnchorBolt->d = pAnchorBoltInfo->d;
			pAnchorBolt->SetAnchorInfo(pAnchorBoltInfo);
		}
		if(pAnchorBoltInfo==NULL)
		{
			pAnchorBolt->InitDefaultAnchorInfo();
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"ϵͳ������δ�ҵ�M%d���ĵؽ���˨��Ϣ", pAnchorBolt->d);
		}

		pAnchorBolt->baseplate.wiBasePlateThick=ftoi(pBasePart->GetThick());
		pAnchorBolt->m_fLr=200;//wiHeight>200?(float)wiHeight-200:200;
	}
	//if(count!=4&&count!=8)
	pFoundation->location=xSummBasePoint/count;
	GEPOINT vOrgOffset=lcs.TransPToCS(pFoundation->location);
	lcs.origin=pFoundation->location;
	if(count>8)
	{
		logerr.Log("�����ʱ��֧��8�����ڵؽ���˨�Ļ�����ʾ��Ŀǰ�ؽ���˨����%d��",count);
		count=8;
	}
	else
	{
		for (i=0;i<count;i++)
		{
			CLDSFoundation::LOCATION* pLocation=pFoundation->anchors.GetAt(i);
			pLocation->siPosX-=ftoi(vOrgOffset.x);
			pLocation->siPosY-=ftoi(vOrgOffset.y);
			if ((pAnchorBolt=(CLDSAnchorBolt*)Ta.FromPartHandle(pLocation->hAnchorBolt))!=NULL)
			{
				pAnchorBolt->ucs.origin=lcs.TransPFromCS(GEPOINT(pLocation->siPosX,pLocation->siPosY));
				pAnchorBolt->SetModified();
				pAnchorBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pAnchorBolt->GetSolidPartObject());
			}
		}
	}
	//pFoundation->LayoutAnchorBolts(Ta.m_xFoundation.biLayoutType,Ta.m_xFoundation.wiBoltSapce);
	//pFoundation->AddOrUpdateAnchorBolts(Ta.m_xFoundation.wiBasePlateThick);
	if (blAddFirstFoundation)
	{
		CLDSNode* pBaseNode=pBasePart.pBasicPlate->GetBaseNode();
		if (pBaseNode)
		{
			GEPOINT zorg=lcs.TransPToCS(pBaseNode->xOriginalPos);
			double tolerance=pBasePart->GetThick()*0.25;
			if (zorg.z<=tolerance)	//1.������ƽ��
				Ta.m_xFoundation.biBasePlateType=1;
			else if (zorg.z>=pBasePart->GetThick()-tolerance)//0.������ƽ�� 
				Ta.m_xFoundation.biBasePlateType=0;
			else if (fabs(zorg.z-pBasePart->GetThick()/2)<tolerance)
				Ta.m_xFoundation.biBasePlateType=2;//2.�����м�
			else
				logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON,"�׽Ű����������ڵ����λ�ò����ϳ���");
		}
		Ta.m_xFoundation.wiBasePlateWidth=(WORD)pBasePart->GetWidth();
		Ta.m_xFoundation.wiBasePlateThick=ftoi(pBasePart->GetThick());
		Ta.m_xFoundation.wiBoltD=wiExistBoltD;
		if (pFoundation->anchors.Count==4)
		{
			CLDSFoundation::LOCATION *pFirstLocation=pFoundation->anchors.GetAt(0);
			for (int j=1;j<4;j++)
			{
				if (pFirstLocation->siPosX==pFoundation->anchors[j].siPosX)
					Ta.m_xFoundation.wiBoltSapce=ftoi(pFirstLocation->siPosY-pFoundation->anchors[j].siPosY);
				else if (pFirstLocation->siPosY==pFoundation->anchors[j].siPosY)
					Ta.m_xFoundation.wiBoltSapce=ftoi(pFirstLocation->siPosX-pFoundation->anchors[j].siPosX);
				else
					continue;
				break;
			}
			//Ta.m_xFoundation.S2;	//�ݻ������S2����ָʲô
		}
		Ta.m_xFoundation.wiBasePlateHoleD=wiExistBoltHoleD;
		Ta.m_xFoundation.wiFoundationWidth=Ta.m_xFoundation.wiBasePlateWidth+200;
		if (pFoundation->anchors.Count==4)
			Ta.m_xFoundation.biLayoutType=FOUNDATION_PARA::LAYOUT_4BOLT;
		else if (pFoundation->anchors.Count==8)
			Ta.m_xFoundation.biLayoutType=FOUNDATION_PARA::LAYOUT_8BOLT;
		else	//���ɶ���ĵؽ���˨������ֹ��������ŵ�����
			Ta.m_xFoundation.biLayoutType=FOUNDATION_PARA::LAYOUT_NBOLT;
	}
	pFoundation->ciAnchorBoltD=(BYTE)Ta.m_xFoundation.wiBoltD;
	pFoundation->wiWidth=(WORD)pBasePart->GetWidth();//Ta.m_xFoundation.wiFoundationWidth;
	pFoundation->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pFoundation->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return 0;
}
int CLDSView::CutOpenByFitting()
{
	return 0;
}
void CLDSView::OnAddAnchorBolts()
{
	AddAnchorBolts();
}

void CLDSView::OnCutOpenByFitting()
{
	CutOpenByFitting();
}
#endif