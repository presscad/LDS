//<LOCALE_TRANSLATE BY hxr /> 2015-1-9
// ModifyRelativeObjPartNoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "ModifyRelativeObjPartNoDlg.h"
#include "LDSView.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModifyRelativeObjPartNoDlg dialog


CModifyRelativeObjPartNoDlg::CModifyRelativeObjPartNoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModifyRelativeObjPartNoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModifyRelativeObjPartNoDlg)
	m_sCurObjPartNo = _T("");
	//}}AFX_DATA_INIT
	m_pWorkPart = NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	m_listCtrl.AddColumnHeader("Part No",65);
	m_listCtrl.AddColumnHeader("Mirror Style",170);
	m_listCtrl.AddColumnHeader("Angle",52);
#else 
	m_listCtrl.AddColumnHeader("构件编号",90);
	m_listCtrl.AddColumnHeader("对称方式",125);
	m_listCtrl.AddColumnHeader("旋转角度",70);
#endif
	listViewComparer.SetMaxColumnCount(3);
}


void CModifyRelativeObjPartNoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModifyRelativeObjPartNoDlg)
	DDX_Text(pDX, IDC_E_CUR_OBJ_PARTNO, m_sCurObjPartNo);
	DDV_MaxChars(pDX, m_sCurObjPartNo, 15);
	DDX_Control(pDX, IDC_LIST_RELATIVE_OBJECT, m_listCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModifyRelativeObjPartNoDlg, CDialog)
	//{{AFX_MSG_MAP(CModifyRelativeObjPartNoDlg)
	ON_NOTIFY(NM_CLICK, IDC_LIST_RELATIVE_OBJECT, OnClickListRelativeObject)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_RELATIVE_OBJECT, OnKeydownListRelativeObject)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_RELATIVE_OBJECT, OnEndlabeleditList)
	ON_EN_CHANGE(IDC_E_CUR_OBJ_PARTNO, &CModifyRelativeObjPartNoDlg::OnChangeCurObjPartno)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_RELATIVE_OBJECT, &CModifyRelativeObjPartNoDlg::OnColumnClickPartNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModifyRelativeObjPartNoDlg message handlers

void CModifyRelativeObjPartNoDlg::OnKeydownListRelativeObject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_listCtrl.GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_F3)
		{
			if(iCurSel>=0)
				m_listCtrl.EditLabel(iCurSel);
		}
	}
	*pResult = 0;
}

BOOL CModifyRelativeObjPartNoDlg::OnInitDialog() 
{
	if(!CDialog::OnInitDialog())
		return FALSE;

	m_listCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT
		| LVS_EX_HEADERDRAGDROP);
	if(m_pWorkPart==NULL)
		return FALSE;
	if(!m_pWorkPart->IsPart())
		return FALSE;
	m_sCurObjPartNo=m_pWorkPart->GetPartNo();
	m_listCtrl.InitListCtrl();
	SEGI workPartSegI;
	CXhChar16 sWorkPartSerial;
	ParsePartNo(m_sCurObjPartNo,&workPartSegI,sWorkPartSerial);
	for(RELATIVE_OBJECT *pRelaObj=m_pWorkPart->relativeObjs.GetFirst();pRelaObj;pRelaObj=m_pWorkPart->relativeObjs.GetNext())
	{
		CLDSDbObject *pRelativeObj=(CLDSDbObject*)console.FromHandle(pRelaObj->hObj,TRUE);
		if(pRelativeObj==NULL)	//不存在该构件
		{
			m_pWorkPart->relativeObjs.DeleteCursor();
			continue;
		}
		if(!pRelativeObj->IsPart())
			continue;
		CString tempStr="";
		CStringArray str_arr;
		if(pRelativeObj->IsPart())
			tempStr=((CLDSPart*)pRelativeObj)->GetPartNo();
		str_arr.Add(tempStr);
		if((pRelaObj->mirInfo.axis_flag&8)&&(pRelaObj->mirInfo.axis_flag&1))
#ifdef AFX_TARG_ENU_ENGLISH
			tempStr.Format("X Mirror|Rotate Mirror");		//X轴对称后再旋转对称
		else if((pRelaObj->mirInfo.axis_flag&8)&&(pRelaObj->mirInfo.axis_flag&2))
			tempStr.Format("Y Mirror|Rotate Mirror");		//Y轴对称后再旋转对称
		else if((pRelaObj->mirInfo.axis_flag&8)&&(pRelaObj->mirInfo.axis_flag&4))
			tempStr.Format("Z Mirror|Rotate Mirror");		//Z轴对称后再旋转对称
		else if(pRelaObj->mirInfo.axis_flag&1)
			tempStr.Format("X Mirror");
		else if(pRelaObj->mirInfo.axis_flag&2)
			tempStr.Format("Y Mirror");
		else if(pRelaObj->mirInfo.axis_flag&4)
			tempStr.Format("Z Mirror");
		else if(pRelaObj->mirInfo.axis_flag&8)
			tempStr.Format("Rotate Mirror");
#else 
			tempStr.Format("X轴对称|旋转对称");		//X轴对称后再旋转对称
		else if((pRelaObj->mirInfo.axis_flag&8)&&(pRelaObj->mirInfo.axis_flag&2))
			tempStr.Format("Y轴对称|旋转对称");		//Y轴对称后再旋转对称
		else if((pRelaObj->mirInfo.axis_flag&8)&&(pRelaObj->mirInfo.axis_flag&4))
			tempStr.Format("Z轴对称|旋转对称");		//Z轴对称后再旋转对称
		else if(pRelaObj->mirInfo.axis_flag&1)
			tempStr.Format("X轴对称");
		else if(pRelaObj->mirInfo.axis_flag&2)
			tempStr.Format("Y轴对称");
		else if(pRelaObj->mirInfo.axis_flag&4)
			tempStr.Format("Z轴对称");
		else if(pRelaObj->mirInfo.axis_flag&8)
			tempStr.Format("旋转对称");
#endif
		else
			tempStr="";
		str_arr.Add(tempStr);
		if(pRelaObj->mirInfo.axis_flag&8)
			tempStr.Format("%d",pRelaObj->mirInfo.rotate_angle);
		else 
			tempStr="0";
		str_arr.Add(tempStr);
		int iItem = m_listCtrl.InsertItemRecord(-1,str_arr);
		SEGI otherPartSegI;
		CXhChar16 sOtherPartSerial;
		CLDSPart *pOtherPart=(CLDSPart*)pRelativeObj;
		ParsePartNo(pOtherPart->GetPartNo(),&otherPartSegI,sOtherPartSerial);
		//使用pPart->feature属性记录构件流水号差值 wht 16-10-30
		pOtherPart->feature=atoi(sOtherPartSerial)-atoi(sWorkPartSerial);
		m_listCtrl.SetItemData(iItem,(long)pOtherPart);
	}
	m_pWorkPart->relativeObjs.Clean();
	UpdateData(FALSE);
	listViewComparer.m_pListCtrl = &m_listCtrl;
	listViewComparer.m_iColumn = 1;
	listViewComparer.sortOrderAscending[1]= TRUE;
	m_listCtrl.SortItems(CompareItemProc,(DWORD)&listViewComparer);
	return TRUE;
}

void CModifyRelativeObjPartNoDlg::OnOK()
{
	char tem_str[101];
	int i,n=m_listCtrl.GetItemCount();
	for(i=0;i<n;i++)
	{
		m_listCtrl.GetItemText(i,0,tem_str,100);
		CLDSPart *pPart=(CLDSPart*)m_listCtrl.GetItemData(i);
		if(pPart==NULL)
			continue;
		CLDSParamPlate* pParamPlate=pPart->GetClassTypeId()==CLS_PARAMPLATE?(CLDSParamPlate*)pPart:NULL;
		//更新构件编号，同时更新构件段号
		SEGI iSeg;
		if((pParamPlate==NULL||!pParamPlate->m_bStdPart)&&
			CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(tem_str,&iSeg,NULL))
			pPart->iSeg=iSeg;
		pPart->SetPartNo(tem_str);
	}
	CDialog::OnOK();
}

void CModifyRelativeObjPartNoDlg::SelectPart(int iCurSel) 
{	//选中构件
	CLDSView *pLDSView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
	CLDSPart *pPart=(CLDSPart*)m_listCtrl.GetItemData(iCurSel);
	if(pPart==NULL)
		return;
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(pPart->handle);
	pLDSView->UpdatePropertyPage();
}

void CModifyRelativeObjPartNoDlg::OnClickListRelativeObject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_listCtrl.GetNextSelectedItem(pos);
		if(iCurSel>=0)
			m_listCtrl.EditLabel(iCurSel);
		//	SelectPart(iCurSel);
	}
	*pResult = 0;
}

void CModifyRelativeObjPartNoDlg::OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM *pItem = &pDispInfo->item;
	// In case window is being destroyed
    if(!IsWindow(GetSafeHwnd()))
		return;
	if(pItem==NULL||(pItem&&pItem->pszText==NULL))
		return;
    // Only set as modified if (a) it actually was, and (b) ESC was not hit.
    if(pItem->lParam != VK_ESCAPE &&
       m_listCtrl.GetItemText(pItem->iItem, pItem->iSubItem) != pItem->pszText)
	{
		m_listCtrl.UpdateData();
		m_listCtrl.SetItemText(pItem->iItem,pItem->iSubItem,pItem->pszText);

		CXhChar50 sSecondColText;
		m_listCtrl.GetItemText(pItem->iItem,1,sSecondColText,50);
		if(stricmp(sSecondColText,"X轴对称")==0)
		{
			for (int i=0;i<m_listCtrl.GetItemCount();i++)
			{
				m_listCtrl.GetItemText(i,1,sSecondColText,50);
				if(stricmp(sSecondColText,"Y轴对称")==0||stricmp(sSecondColText,"X轴对称|旋转对称")==0||stricmp(sSecondColText,"Y轴对称|旋转对称")==0)
					m_listCtrl.SetItemText(i,0,pItem->pszText);
			}
		}
		else if(stricmp(sSecondColText,"Y轴对称")==0)
		{
			for (int i=0;i<m_listCtrl.GetItemCount();i++)
			{
				m_listCtrl.GetItemText(i,1,sSecondColText,50);
				if(stricmp(sSecondColText,"Y轴对称|旋转对称")==0)
					m_listCtrl.SetItemText(i,0,pItem->pszText);
			}
		}
	}
	*pResult = 0;
}


void CModifyRelativeObjPartNoDlg::OnColumnClickPartNo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	listViewComparer.m_pListCtrl = &m_listCtrl;
	listViewComparer.m_iColumn = pNMLV->iSubItem;
	listViewComparer.sortOrderAscending[pNMLV->iSubItem]=!listViewComparer.sortOrderAscending[pNMLV->iSubItem];
	m_listCtrl.SortItems(CompareItemProc,(DWORD)&listViewComparer);
	*pResult = 0;
}

void CModifyRelativeObjPartNoDlg::OnChangeCurObjPartno()
{
	UpdateData();
	CXhChar50 sFirstColText;
	//计算当前构件段号与流水号
	SEGI curSegI;
	CXhChar16 sCurSerial;
	ParsePartNo(m_sCurObjPartNo,&curSegI,sCurSerial);
	int iCurSerial=atoi(sCurSerial);
	//生成段号字符串(包括分隔符)
	const char* separator=strchr(m_sCurObjPartNo,'-');
	if(separator==NULL)
		separator=strchr(m_sCurObjPartNo,'_');
	CXhChar16 sSeg=curSegI.ToString();
	if(separator)
		sSeg.Printf("%s%c",(char*)curSegI.ToString(),(char)*separator);
	if(curSegI==0)
		sSeg.Empty();
	//更新构件编号
	for (int i=0;i<m_listCtrl.GetItemCount();i++)
	{
		CLDSPart *pPart=(CLDSPart*)m_listCtrl.GetItemData(i);
		if(pPart==NULL)
			continue;
		m_listCtrl.GetItemText(i,0,sFirstColText,50);
		if(strlen(sFirstColText)>0)
		{
			if((iCurSerial+pPart->feature)<10&&curSegI>0)
				sprintf(sFirstColText,"%s0%d",(char*)sSeg,iCurSerial+pPart->feature);
			else
				sprintf(sFirstColText,"%s%d",(char*)sSeg,iCurSerial+pPart->feature);
			m_listCtrl.SetItemText(i,0,sFirstColText);
		}
		else
			m_listCtrl.SetItemText(i,0,m_sCurObjPartNo);
	}
	UpdateData(FALSE);
}
