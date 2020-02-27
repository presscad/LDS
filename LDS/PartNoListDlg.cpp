//<LOCALE_TRANSLATE Confirm by hxr/>
// PartNoListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "PartNoListDlg.h"
#include "ModfiyPartNoDlg.h"
#include "GlobalFunc.h"
#include "Env_def.h"
#include "MovePartNoDlg.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "ComparePartNoString.h"
#include "SortFunc.h"
#include "ProcBarDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartNoListDlg dialog

#ifdef __SMART_DRAW_INC_

struct SEG_NO{
	SEGI iSeg;
	long iSerialNo;
};

CPartNoListDlg::CPartNoListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPartNoListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartNoListDlg)
	m_sSegNo = _T("*");
	m_bGroupSel = TRUE;
	//}}AFX_DATA_INIT
	m_partNoList.AddColumnHeader("△");
#ifdef AFX_TARG_ENU_ENGLISH
	m_partNoList.AddColumnHeader("Label");
	m_partNoList.AddColumnHeader("segment number");
	m_partNoList.AddColumnHeader("type");
	m_partNoList.AddColumnHeader("handle");
#else
	m_partNoList.AddColumnHeader("编号");
	m_partNoList.AddColumnHeader("段号");
	m_partNoList.AddColumnHeader("类型");
	m_partNoList.AddColumnHeader("句柄");	
#endif
}
IMPLEMENT_DYNCREATE(CPartNoListDlg, CDialog);
void CPartNoListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartNoListDlg)
	DDX_Control(pDX, IDC_SELECT_BTN, m_btnSelect);
	DDX_Control(pDX, IDC_PARTNO_LIST, m_partNoList);
	DDX_Text(pDX, IDC_E_SEGNO, m_sSegNo);
	DDX_Check(pDX, IDC_CHK_GROUP_SEL, m_bGroupSel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPartNoListDlg, CDialog)
	//{{AFX_MSG_MAP(CPartNoListDlg)
	ON_BN_CLICKED(IDC_BN_SELECT, OnSelectBtn)
	ON_BN_CLICKED(IDC_BN_RETRIEVE_PARTNO, OnRetrievePartNo)
	ON_BN_CLICKED(IDC_BN_SAVE, OnUpdateBtn)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_KEYDOWN, IDC_PARTNO_LIST, OnKeydownPartnoList)
	ON_NOTIFY(NM_RCLICK, IDC_PARTNO_LIST, OnRclickPartnoList)
	ON_NOTIFY(NM_CLICK, IDC_PARTNO_LIST, OnClickPartnoList)
	ON_BN_CLICKED(IDC_BN_IMPORT_PARTNO, OnImportPartNo)
	ON_COMMAND(ID_MOVE,OnMove)
	ON_BN_CLICKED(IDC_BTN_MOVE_TOP, OnBnMoveTop)
	ON_BN_CLICKED(IDC_BTN_UP_MOVE, OnBnUpMove)
	ON_BN_CLICKED(IDC_BTN_DWON_MOVE, OnBnDownMove)
	ON_BN_CLICKED(IDC_BTN_MOVE_BOTTOM, OnBnMoveBottom)
	ON_BN_CLICKED(IDC_BN_EXPORT_PARTNO, OnBnExportPartNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////
//supergridctrl回调函数
static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem,CString& sTextValue)
{
	CPartNoListDlg* pDlg=(CPartNoListDlg*)pListCtrl->GetParent();
	if(pDlg==NULL)
		return FALSE;
	CXhChar100 tem_str;
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",sTextValue);
	PART_NO* pRecord=pDlg->m_PartNoRecordList.GetValue(pSelItem->m_idProp);
	if(pRecord==NULL)
		return FALSE;
	CLDSPart* pPart=Ta.FromPartHandle(pRecord->h);
	pListCtrl->SetItemText(pSelItem->GetIndex(),iSubItem,sTextValue.GetBuffer(1));
	if(iSubItem==0)			//构件编号类型
		pRecord->cNoType=tem_str[0];
	else if(iSubItem==1)	//构件编号
	{
		sprintf(pRecord->sPartNo,"%s",&tem_str);
		if(pPart)
			pPart->SetPartNo(tem_str);
	}
	else if(iSubItem==2)	//段号
	{		
		pRecord->iSeg=SEGI(tem_str);
		if(pPart)
			pPart->iSeg=pRecord->iSeg;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CPartNoListDlg message handlers

BOOL CPartNoListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_partNoList.DeleteAllItems();
	long col_wide_arr[5]={50,58,40,40,58};
	m_partNoList.InitListCtrl(col_wide_arr);
	m_partNoList.ModifyStyle(LVS_SINGLESEL,0);
	m_partNoList.SetExtendedStyle(m_partNoList.GetExtendedStyle()&~LVS_EX_HEADERDRAGDROP);
	m_partNoList.EnableSortItems(false);
	m_partNoList.SetModifyValueFunc(FireValueModify);
	CButton *pBut=(CButton *)GetDlgItem(IDC_BTN_MOVE_TOP);
	char btnname[50]="";
	pBut->GetWindowTextA(btnname,50);
	if(m_curBitmap1.LoadBitmap(IDB_BMP_LABEL_MOVE_TOP_BUTTON))
	{
		pBut->SetBitmap(m_curBitmap1);
	}
	pBut=(CButton *)GetDlgItem(IDC_BTN_MOVE_BOTTOM);
	pBut->GetWindowTextA(btnname,50);
	if(m_curBitmap2.LoadBitmap(IDB_BMP_LABEL_MOVE_BOTTOM_BUTTON))
	{
		pBut->SetBitmap(m_curBitmap2);
	}
	return TRUE;
}
void CPartNoListDlg::OnOK()
{

}	
void CPartNoListDlg::OnCancel()
{

}

void CPartNoListDlg::UpdateNoList(CString str)
{
	CHashList<SEGI>segTbl;
	GetSegNoHashTblBySegStr(str,segTbl);
	m_partNoList.DeleteAllItems();
	int i=0;
	CString itemText;
	CListCtrlItemInfo* lpInfo = NULL;
	CSuperGridCtrl::CTreeItem* pItem=NULL;
	for(PART_NO* pPartNoRecord = m_PartNoRecordList.GetFirst();
	pPartNoRecord;pPartNoRecord = m_PartNoRecordList.GetNext(),i++)
	{
		if (segTbl.GetNodeNum()==0||
			segTbl.GetValue(pPartNoRecord->iSeg.iSeg))
		{
			//编号类型
			lpInfo = new CListCtrlItemInfo();
			itemText.Format("%c",pPartNoRecord->cNoType);
			lpInfo->SetSubItemText(0,itemText,FALSE);
			lpInfo->SetControlType(0,GCT_CMB_EDIT);
			lpInfo->SetListItemsStr(0,"N|R|S");
			//构件编号
			itemText.Format("%s",pPartNoRecord->sPartNo);
			lpInfo->SetSubItemText(1,itemText,FALSE);
			//段号
			itemText.Format("%s",(char*)pPartNoRecord->iSeg.ToString());
			lpInfo->SetSubItemText(2,itemText,TRUE);
			//构件类型
			if (pPartNoRecord->cPartType=='L')
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[3] = "Angle";
			else if(pPartNoRecord->cPartType=='P')
				str_arr[3] = "Tube";
			else if(pPartNoRecord->cPartType=='-')
				str_arr[3] = "Plate";
			else if(pPartNoRecord->cPartType=='F')
				str_arr[3] = "Flange";
			else
				str_arr[3] = "Others";
#else
				itemText = "角钢";
			else if(pPartNoRecord->cPartType=='P')
				itemText = "钢管";
			else if(pPartNoRecord->cPartType=='-')
				itemText = "钢板";
			else if(pPartNoRecord->cPartType=='F')
				itemText = "法兰";
			else
				itemText = "其他";
#endif
			lpInfo->SetSubItemText(3,itemText,TRUE);
			//构件句柄
			itemText.Format("0X%X",pPartNoRecord->h);
			lpInfo->SetSubItemText(4,itemText,TRUE);
			pItem=m_partNoList.InsertRootItem(lpInfo);
			pItem->m_idProp=pPartNoRecord->h;
		}
	}
}

void CPartNoListDlg::OnSelectBtn() 
{
	UpdateData();
	UpdateNoList(m_sSegNo);
	if(m_sSegNo.CollateNoCase(Ta.segstr)!=0)
	{
		_snprintf(Ta.segstr,199,"%s",m_sSegNo);
		CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
		pView->RedrawAll();
		g_pSolidOper->ZoomAll(0.95);
	}
}

void CPartNoListDlg::OnUpdateBtn() 
{
	if (m_PartNoRecordList.GetNodeNum()>0)
		UpdatePartNo();
}

void CPartNoListDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);	
	RECT rectList;
	if (m_partNoList.GetSafeHwnd())
	{
		m_partNoList.GetWindowRect(&rectList);
		ScreenToClient(&rectList);
		if(m_partNoList.GetSafeHwnd())
			m_partNoList.MoveWindow(0,rectList.top,cx,cy-rectList.top);
	}
}

//from outputinfo.cpp
extern SCOPE_STRU GetPartScope(CLDSPart* pPart,SCOPE_STRU *pScope=NULL);
void CPartNoListDlg::SelectMul(int iCurSel)
{
	int iStart=-1,iEnd=-1;
	if(!GetCurSelectedItemIndexScope(iCurSel,iStart,iEnd))
		return;
	m_partNoList.SetFocus();
	//取消所有选中项的选中状态
	m_partNoList.SetSelectionMark(-1);
	m_partNoList.SetItemState(-1,0,LVNI_SELECTED|LVNI_FOCUSED);
	g_pSolidDraw->ReleaseSnapStatus();
	ARRAY_LIST<long> id_arr;
	long minPartId;
	CLDSPart* pMinIdPart=NULL;
	SCOPE_STRU zscope;
	for(int i=iStart;i<=iEnd;i++)
	{
		m_partNoList.SetSelectionMark(i);
		m_partNoList.SetItemState(i,LVIS_SELECTED|LVNI_FOCUSED,LVIS_SELECTED|LVNI_FOCUSED);
		char sText[51]="";
		m_partNoList.GetItemText(i,4,sText,50);
		long handle = 0;
		sscanf(sText,"%X",&handle);
		id_arr.append(handle);
		CLDSPart* pPart=console.FromPartHandle(handle);
		SCOPE_STRU scope=GetPartScope(pPart);
		zscope.VerifyVertex(f3dPoint(scope.fMinX,scope.fMinY,scope.fMinZ));
		zscope.VerifyVertex(f3dPoint(scope.fMaxX,scope.fMaxY,scope.fMaxZ));
		if(pMinIdPart==NULL||minPartId>handle)
		{
			minPartId=handle;
			pMinIdPart=pPart;
		}
	}
	if(pMinIdPart)
	{	//选中句柄最小的构件设置旋转中心
		g_pSolidOper->FocusTo(zscope,g_sysPara.m_fMaxScaleScr2User);
		g_pSolidSet->SetOperType(OPER_ROTATE);
		f3dPoint center;
		SCOPE_STRU scope=GetPartScope(pMinIdPart);
		scope.centre(&center.x,&center.y,&center.z);
		g_pSolidSet->SetRotOrg(center);
		g_pSolidSet->SetZoomStyle(ROT_CENTER);
		g_pSolidDraw->Draw();
	}
	g_pSolidDraw->SetEntSnapStatus(id_arr.m_pData,id_arr.GetSize(),TRUE);
	m_partNoList.EnsureVisible(iStart,FALSE);
	m_partNoList.EnsureVisible(iEnd,FALSE);
	//更新属性栏
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	pView->UpdatePropertyPage();
	m_partNoList.SetFocus();
}
void CPartNoListDlg::OnClickPartnoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(GetKeyState(VK_SHIFT)&0x8000)	//GetKeyState()最高位为1表示当前键处于down的状态，最高位为0当前键处于up状态
	{
		POSITION pos=m_partNoList.GetFirstSelectedItemPosition();
		int iCurSel=0,iFirstSelect=-1,iTailSelect=0;
		PART_NO* pLabelRecord = NULL;
		do{
			iCurSel=m_partNoList.GetNextSelectedItem(pos);
			if(iFirstSelect<0)
				iFirstSelect=iCurSel;
			if(iCurSel>iTailSelect)
				iTailSelect=iCurSel;
		}while(iCurSel>=0);
		iFirstSelect=min(pNMListView->iItem,iFirstSelect);
		iTailSelect=max(pNMListView->iItem,iTailSelect);
		int iStart=-1,iEnd=-1;
		if(!GetCurSelectedItemIndexScope(iFirstSelect,iStart,iEnd))
			return;
		iFirstSelect=min(iStart,iFirstSelect);
		iTailSelect=max(iEnd,iTailSelect);
		if(!GetCurSelectedItemIndexScope(iTailSelect,iStart,iEnd))
			return;
		iFirstSelect=min(iStart,iFirstSelect);
		iTailSelect=max(iEnd,iTailSelect);
		for(int i=iFirstSelect;i<=iTailSelect;i++)
		{
			m_partNoList.SetSelectionMark(i);
			m_partNoList.SetItemState(i,LVIS_SELECTED|LVNI_FOCUSED,LVIS_SELECTED|LVNI_FOCUSED);
		}
		return;							//按下Shift键表示要选中多项
	}
	SelectMul(pNMListView->iItem);
	*pResult = 0;
}
void CPartNoListDlg::OnKeydownPartnoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	int iCurSel=-1;
	POSITION pos = m_partNoList.GetFirstSelectedItemPosition();
	if(pos!=NULL)
		iCurSel = m_partNoList.GetNextSelectedItem(pos);
	int iStart=-1,iEnd=-1;
	if(!GetCurSelectedItemIndexScope(iCurSel,iStart,iEnd))
		return;
	
	if(pLVKeyDow->wVKey==VK_DOWN||pLVKeyDow->wVKey==VK_UP)
	{
		if(pLVKeyDow->wVKey==VK_DOWN)
		{
			if((iEnd+1)>=m_partNoList.GetItemCount())
				SelectMul(0);
			else 
				SelectMul(iEnd); //+1
		}
		else
		{
			if((iStart-1)<0)
				SelectMul(m_partNoList.GetItemCount()-1);
			else 
				SelectMul(iStart-1);
		}
	}
	*pResult = 0;
}

CStringArray partNoArr;
static BOOL FindPartNo(const char *sPartNo=NULL)
{
	CLDSPart *pPart;
	if(sPartNo)
	{
		if(strlen(sPartNo)==0)
			return TRUE;
		for(int i=0;i<partNoArr.GetSize();i++)
		{
			if(partNoArr[i].CompareNoCase(sPartNo)==0)
				return TRUE;	//此编号已被使用
		}
		return FALSE;
	}
	else
		partNoArr.RemoveAll();
	for(pPart=Ta.Parts.GetFirst();pPart!=NULL;pPart=Ta.Parts.GetNext())
	{
		if(strlen(pPart->GetPartNo())==0)
			continue;
		for(int i=0;i<partNoArr.GetSize();i++)
			if(partNoArr[i].CompareNoCase(pPart->GetPartNo())==0)	//有此号
				break;
		if(i==partNoArr.GetSize())
			partNoArr.Add(pPart->GetPartNo());
	}
	for(CSupplementPart *pSuppPart=Ta.SuppPart.GetFirst();pSuppPart;pSuppPart=Ta.SuppPart.GetNext())
	{
		if(strlen(pSuppPart->GetPartNo())==0)
			continue;
		for(int i=0;i<partNoArr.GetSize();i++)
			if(partNoArr[i].CompareNoCase(pSuppPart->GetPartNo())==0)	//有此号
				break;
		if(i==partNoArr.GetSize())
			partNoArr.Add(pSuppPart->GetPartNo());
	}
	return TRUE;
}

void CPartNoListDlg::UpdatePartNo() 
{
	m_PartNoRecordList.Empty();
	for(int i=0;i<m_partNoList.GetItemCount();i++)
	{
		char sText[51];
		m_partNoList.GetItemText(i,4,sText,50);
		long h=HexStrToLong(sText);
		PART_NO* pPartNoRecord = m_PartNoRecordList.Add(h);
		pPartNoRecord->h=h;
		m_partNoList.GetItemText(i,0,sText,50);
		pPartNoRecord->cNoType = sText[0];
		m_partNoList.GetItemText(i,1,sText,50);
		sprintf(pPartNoRecord->sPartNo,"%s",sText);
		m_partNoList.GetItemText(i,2,sText,50);
		pPartNoRecord->iSeg=SEGI(sText);
		m_partNoList.GetItemText(i,3,sText,50);
#ifdef AFX_TARG_ENU_ENGLISH
		if(strcmp(sText,"Angle")==0)
			pPartNoRecord->cPartType='L';
		else if(strcmp(sText,"Tube")==0)
			pPartNoRecord->cPartType='P';
		else if(strcmp(sText,"Plate")==0)
			pPartNoRecord->cPartType='-';
		else if(strcmp(sText,"Flange")==0)
#else
		if(strcmp(sText,"角钢")==0)
			pPartNoRecord->cPartType='L';
		else if(strcmp(sText,"钢管")==0)
			pPartNoRecord->cPartType='P';
		else if(strcmp(sText,"钢板")==0)
			pPartNoRecord->cPartType='-';
		else if(strcmp(sText,"法兰")==0)
#endif
			pPartNoRecord->cPartType='F';
		else
			pPartNoRecord->cPartType='?';
	}
	partNoArr.RemoveAll();
	ATOM_LIST<SEG_NO>seg_list;
	for(CSupplementPart *pSuppPart=Ta.SuppPart.GetFirst();pSuppPart;pSuppPart=Ta.SuppPart.GetNext())
	{	//统计附件构件的编号
		if(!FindPartNo(pSuppPart->GetPartNo()))
			partNoArr.Add(pSuppPart->GetPartNo());
	}
	char sPrevPartNo[MAX_CHAR_GUIGE_16+1]="",sPartNo[MAX_CHAR_GUIGE_16+1]="",sCurrentFLPartNo[MAX_CHAR_GUIGE_16+1]="";
	int fl_serialno=0,elbow_serialno=0;
	for(PART_NO *pPartNoRecord=m_PartNoRecordList.GetFirst();pPartNoRecord;pPartNoRecord=m_PartNoRecordList.GetNext())
	{
		if(pPartNoRecord->cNoType=='S')	//指定构件编号
		{
			if(!FindPartNo(pPartNoRecord->sPartNo))
				partNoArr.Add(pPartNoRecord->sPartNo);
		}
		CSuperSmartPtr<CLDSPart> pPart=console.FromPartHandle(pPartNoRecord->h);
		if(pPartNoRecord->cNoType=='N')	//新号
		{
			int str_len=strlen(pPartNoRecord->sPartNo);
			if(pPartNoRecord->cPartType=='F')
			{
				strcpy(sPartNo,"");
				while(FindPartNo(sPartNo))
				{
					char cFinal=pPartNoRecord->sPartNo[str_len-1];
					if(cFinal!='B')
						fl_serialno++;
					if(cFinal>'9'||cFinal<'0')
						_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"FL%d%C",fl_serialno,cFinal);
					else
						_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"FL%d",fl_serialno);
				}
				strcpy(sPrevPartNo,sPartNo);
			}
			else if(pPartNoRecord->cPartType=='-'&&pPartNoRecord->sPartNo[0]=='F')
			{	//跟法兰一起编号的肘板
				char fl_partno[MAX_PATH]="";
				strcpy(fl_partno,strtok(pPartNoRecord->sPartNo,"-_#"));
				if(strlen(sCurrentFLPartNo)==0)
				{
					strcpy(sCurrentFLPartNo,fl_partno);
					elbow_serialno=1;
				}
				else if(stricmp(sCurrentFLPartNo,fl_partno)==0)
					elbow_serialno++;
				else if(stricmp(sCurrentFLPartNo,fl_partno)!=0)
				{
					strcpy(sCurrentFLPartNo,fl_partno);
					elbow_serialno=1;
				}
				_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"%s-%d",sCurrentFLPartNo,elbow_serialno);
				strcpy(sPrevPartNo,sPartNo);
			}
			else
			{
				for(SEG_NO *pSegNo=seg_list.GetFirst();pSegNo;pSegNo=seg_list.GetNext())
				{
					if(pSegNo->iSeg.iSeg==pPartNoRecord->iSeg.iSeg)
					{
						strcpy(sPartNo,"");
						while(FindPartNo(sPartNo))
						{
							pSegNo->iSerialNo++;
							if(pSegNo->iSerialNo<10)
								_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"%s0%d",(char*)pPartNoRecord->iSeg.ToString(),pSegNo->iSerialNo);
							else if(pSegNo->iSerialNo<100)
								_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"%s%d",(char*)pPartNoRecord->iSeg.ToString(),pSegNo->iSerialNo);
							else
								_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"%s-%d",(char*)pPartNoRecord->iSeg.ToString(),pSegNo->iSerialNo);
						}
						strcpy(sPrevPartNo,sPartNo);
						break;
					}
				}
				if(pSegNo==NULL)
				{
					pSegNo=seg_list.append();
					pSegNo->iSeg=pPartNoRecord->iSeg;
					pSegNo->iSerialNo=0;
					strcpy(sPartNo,"");
					while(FindPartNo(sPartNo))
					{
						pSegNo->iSerialNo++;
						if(pSegNo->iSerialNo<10)
							_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"%s0%d",(char*)pPartNoRecord->iSeg.ToString(),pSegNo->iSerialNo);
						else if(pSegNo->iSerialNo<100)
							_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"%s%d",(char*)pPartNoRecord->iSeg.ToString(),pSegNo->iSerialNo);
						else
							_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"%s-%d",(char*)pPartNoRecord->iSeg.ToString(),pSegNo->iSerialNo);
						//_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"%d",pPartNoRecord->iSeg*100+pSegNo->iSerialNo);
					}
					strcpy(sPrevPartNo,sPartNo);
				}
			}
		}
		else if(pPartNoRecord->cNoType=='R')	//重复上一编号
			strcpy(sPartNo,sPrevPartNo);
		else if(pPartNoRecord->cNoType=='S')	//指定为后面的编号
		{
			strcpy(sPartNo,pPartNoRecord->sPartNo);
			strcpy(sPrevPartNo,sPartNo);
			if(pPartNoRecord->cPartType=='F')
				fl_serialno=RetrievePartSerialNo(sPartNo);
			else if(pPartNoRecord->cPartType=='-'&&pPartNoRecord->sPartNo[0]=='F')
				elbow_serialno=RetrievePartSerialNo(sPartNo);
			else
			{
				for(SEG_NO *pSegNo=seg_list.GetFirst();pSegNo;pSegNo=seg_list.GetNext())
				{
					if(pSegNo->iSeg.iSeg==pPartNoRecord->iSeg.iSeg)
					{
						pSegNo->iSerialNo=RetrievePartSerialNo(sPartNo);
						break;
					}
				}
				if(pSegNo==NULL)
				{
					pSegNo=seg_list.append();
					pSegNo->iSeg=pPartNoRecord->iSeg;
					pSegNo->iSerialNo=RetrievePartSerialNo(sPartNo);
				}
			}
		}
		strcpy(pPartNoRecord->sPartNo,sPartNo);
		if(pPart.IsHasPtr())
			pPart->SetPartNo(sPartNo);
	}
	POLYPARTLIST polylist;
	Ta.RetrievePolyPartList(polylist);
	CLDSPolyPart *pPolyPart=NULL;
	for(pPolyPart=polylist.GetFirst();pPolyPart;pPolyPart=polylist.GetNext())
	{
		CLDSLinePart *pLinePart=NULL,*pFirstLinePart=pPolyPart->segset.GetFirst();
		for(pLinePart=pPolyPart->segset.GetNext();pLinePart;pLinePart=pPolyPart->segset.GetNext())
			pLinePart->SetPartNo(pFirstLinePart->GetPartNo());
	}
	UpdateNoList(m_sSegNo);
}
void CPartNoListDlg::OnRclickPartnoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//OnClickPartnoList(pNMHDR,pResult);
	if(m_partNoList.GetSelectedCount()==0)
		return;
	CPoint point;
	GetCursorPos(&point);
	CRect rect;
	m_partNoList.GetWindowRect(&rect);
	if(rect.PtInRect(point))
		ContextMenu(this,point);
	*pResult = 0;
}
void CPartNoListDlg::ContextMenu(CWnd* pWnd, CPoint point) 
{
	CPoint scr_point = point;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	pMenu->AppendMenu(MF_STRING,ID_MOVE,"移动");
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
}

//根据当前选中项索引得到该编号组的起始索引及结束索引
BOOL CPartNoListDlg::GetCurSelectedItemIndexScope(int iCurSel,int &iStart,int &iEnd)
{
	UpdateData();
	if(iCurSel<0)
		return FALSE;
	if(m_bGroupSel)
	{
		iStart=-1;
		iEnd=-1;
		CString sCurPartNo=m_partNoList.GetItemText(iCurSel,1);
		return GetPartNoGroupScope(sCurPartNo,iStart,iEnd);
	}
	else 
	{
		iStart=iEnd=iCurSel;
		return TRUE;
	}
}
//根据当前编号得到该编号组的起始索引及结束索引
BOOL CPartNoListDlg::GetPartNoGroupScope(CString sCurPartNo,int &iStart,int &iEnd)
{
	if(sCurPartNo.GetLength()<=0)
		return FALSE;
	iStart=-1;
	iEnd=-1;
	for(int i=0;i<m_partNoList.GetItemCount();i++)
	{
		CString sPartNo=m_partNoList.GetItemText(i,1);
		if(sCurPartNo.CompareNoCase(sPartNo)==0)
		{
			if(iStart==-1)
				iStart=i;
			else 
				iEnd=i;
		}
	}
	if(iStart>=0&&iEnd==-1)
		iEnd=iStart;
	if(iStart==-1&&iEnd==-1)
		return FALSE;
	return TRUE;
}

int CPartNoListDlg::GetCurSelByPartHandle(THANDLE hPart)
{
	CString sCurPartHandle;
	sCurPartHandle.Format("0X%X",hPart);
	for(int i=0;i<m_partNoList.GetItemCount();i++)
	{
		CString sPartHandle=m_partNoList.GetItemText(i,4);
		if(sPartHandle.CompareNoCase(sCurPartHandle)==0)
			return i;
	}
	return -1;
}
//工作区中选中构件，只刷新构件编号窗口，不再反射工作区
void CPartNoListDlg::SelectPart(long hPart)
{		
	CLDSPart* pPart=Ta.FromPartHandle(hPart);
	if(pPart==NULL || pPart->GetClassTypeId()==CLS_BOLT)
		return;
	int iStart=-1,iEnd=-1;
	GetPartNoGroupScope((char*)pPart->GetPartNo(),iStart,iEnd);
	m_partNoList.SetSelectionMark(-1);
	m_partNoList.SetItemState(-1,0,LVNI_SELECTED|LVNI_FOCUSED);
	for(int i=iStart;i<=iEnd;i++)
	{
		m_partNoList.SetSelectionMark(i);
		m_partNoList.SetItemState(i,LVIS_SELECTED|LVNI_FOCUSED,LVIS_SELECTED|LVNI_FOCUSED);
	}
}

void CPartNoListDlg::OnMove()
{
	//m_bEnableUpdateSelectedItem = FALSE;
	CMovePartNoDlg dlg;
	if(IDOK==dlg.DoModal())
	{
		int iStart=-1,iEnd=-1;
		if(dlg.GetMoveType()==0)	//按件号移动
			GetPartNoGroupScope(dlg.m_sPartNo,iStart,iEnd);
		else						//按句柄移动
			iStart=iEnd=GetCurSelByPartHandle(dlg.m_hPart);
		if(iStart<0||iEnd<0)
			AfxMessageBox("输入编号不存在!");

		int iInsert=iEnd+1;
		int i,n = m_partNoList.GetSelectedCount();
		ARRAY_LIST<CSuperGridCtrl::CTreeItem*> arraySelItems(0,n);
		POSITION pos=m_partNoList.GetFirstSelectedItemPosition();	
		while(pos!=NULL)
		{
			int iCurSel=m_partNoList.GetNextSelectedItem(pos);
			CSuperGridCtrl::CTreeItem* pItem=m_partNoList.GetTreeItem(iCurSel);
			arraySelItems.append(pItem);
		}
		m_partNoList.MoveTreeItems(arraySelItems.m_pData,arraySelItems.GetSize(),iInsert);
		for(i=0;i<n;i++)
		{
			m_partNoList.SetSelectionMark(i);
			m_partNoList.SetItemState(iInsert+1,LVIS_SELECTED,LVIS_SELECTED);
		}
	}
	//m_bEnableUpdateSelectedItem = TRUE;
}

int CPartNoListDlg::GetSelectedItemList(ARRAY_LIST<CSuperGridCtrl::CTreeItem*>* pListItems,
	int* pStartIndex/*=NULL*/,int* pEndIndex/*=NULL*/)
{
	int n=m_partNoList.GetSelectedCount();
	pListItems->SetSize(0,n);
	POSITION pos=m_partNoList.GetFirstSelectedItemPosition();
	int iCurSel=m_partNoList.GetNextSelectedItem(pos);
	if(pStartIndex)
		*pStartIndex=iCurSel;
	int iTailSel=iCurSel;
	while(iCurSel>=0)
	{
		CSuperGridCtrl::CTreeItem* pItem=m_partNoList.GetRootItem(iCurSel);
		pListItems->append(pItem);
		iTailSel=iCurSel;
		iCurSel=m_partNoList.GetNextSelectedItem(pos);
	}
	if(pEndIndex)
		*pEndIndex=iTailSel;
	return n;
}
void CPartNoListDlg::MoveItemListBefore(ARRAY_LIST<CSuperGridCtrl::CTreeItem*>* pListItems,int iInsertBefore)
{
	int i,n=pListItems->GetSize();
	m_partNoList.MoveTreeItems(pListItems->m_pData,n,iInsertBefore);
	m_partNoList.Redraw();
	for(i=0;i<n;i++)
	{
		CSuperGridCtrl::CTreeItem* pItem=(*pListItems)[i];
		m_partNoList.SetSelectionMark(pItem->GetIndex());
		m_partNoList.SetItemState(pItem->GetIndex(),LVIS_SELECTED|LVNI_FOCUSED,LVIS_SELECTED|LVNI_FOCUSED);
	}
}
void CPartNoListDlg::OnBnMoveTop()
{
	ARRAY_LIST<CSuperGridCtrl::CTreeItem*> itemList;
	int iFirstSelect=0,iTailSelect=0;
	GetSelectedItemList(&itemList,&iFirstSelect,&iTailSelect);
	if(iFirstSelect==0)
		return;
	char sText[51]={0};
	CSuperGridCtrl::CTreeItem* pItem=m_partNoList.GetRootItem(iFirstSelect);
	m_partNoList.GetItemText(iFirstSelect,4,sText,50);//<=0)
	long h=HexStrToLong(sText);
	PART_NO* pLabelRecord = m_PartNoRecordList.GetValue(h);
	//搜索定位当前段的第一个件号记录
	int iCurSel=iFirstSelect-1;
	while(iCurSel>=0){
		CSuperGridCtrl::CTreeItem* pItem=m_partNoList.GetTreeItem(iCurSel);
		if(m_partNoList.GetItemText(iCurSel,4,sText,50)<=0)
			continue;
		long h=HexStrToLong(sText);
		PART_NO* pPrevLabelRecord = m_PartNoRecordList.GetValue(h);
		if(pPrevLabelRecord==NULL||pPrevLabelRecord->iSeg!=pLabelRecord->iSeg)
		{	//段号已发生变化，已移至当前段最前位置
			iCurSel++;
			break;
		}
		else
			iCurSel--;	//当前构件记录为重复前一记录编号（非编号的第一个构件记录）
	};
	if(iCurSel<0)
		iCurSel=0;
	if(iCurSel<iFirstSelect)
		MoveItemListBefore(&itemList,iCurSel);
}

void CPartNoListDlg::OnBnMoveBottom()
{
	ARRAY_LIST<CSuperGridCtrl::CTreeItem*> itemList;
	int iFirstSelect=0,iTailSelect=0;
	GetSelectedItemList(&itemList,&iFirstSelect,&iTailSelect);
	if(iFirstSelect==0)
		return;
	char sText[51]={0};
	CSuperGridCtrl::CTreeItem* pItem=m_partNoList.GetRootItem(iFirstSelect);
	m_partNoList.GetItemText(iTailSelect,4,sText,50);//<=0)
	long h=HexStrToLong(sText);
	PART_NO* pLabelRecord = m_PartNoRecordList.GetValue(h);
	//搜索定位当前段的最后个件号记录
	int iCurSel=iTailSelect+2;
	while(true){
		CSuperGridCtrl::CTreeItem* pItem=m_partNoList.GetTreeItem(iCurSel);
		if(pItem==NULL)
			break;
		m_partNoList.GetItemText(iCurSel,4,sText,50);//<=0)
		long h=HexStrToLong(sText);
		PART_NO* pAfterLabelRecord = m_PartNoRecordList.GetValue(h);
		if(pAfterLabelRecord==NULL||pAfterLabelRecord->iSeg!=pLabelRecord->iSeg)
			break;	//段号已发生变化，已移至当前段最后位置
		else
			iCurSel++;	//当前构件记录为重复前一记录编号（非编号的第一个构件记录）
	};
	if(iCurSel>iTailSelect+1)
		MoveItemListBefore(&itemList,iCurSel);
}

void CPartNoListDlg::OnBnUpMove()
{	//记录项上移
	ARRAY_LIST<CSuperGridCtrl::CTreeItem*> itemList;
	int iFirstSelect=0,iTailSelect=0;
	GetSelectedItemList(&itemList,&iFirstSelect,&iTailSelect);
	if(iFirstSelect==0)
		return;
	char sText[51]={0};
	CSuperGridCtrl::CTreeItem* pItem=m_partNoList.GetRootItem(iFirstSelect);
	m_partNoList.GetItemText(iFirstSelect,4,sText,50);//<=0)
	long h=HexStrToLong(sText);
	PART_NO* pLabelRecord = m_PartNoRecordList.GetValue(h);

	int iCurSel=iFirstSelect-1;
	while(iCurSel>=0){
		CSuperGridCtrl::CTreeItem* pItem=m_partNoList.GetTreeItem(iCurSel);
		if(m_partNoList.GetItemText(iCurSel,4,sText,50)<=0)
			continue;
		long h=HexStrToLong(sText);
		PART_NO* pPrevLabelRecord = m_PartNoRecordList.GetValue(h);
		if(pPrevLabelRecord==NULL||pPrevLabelRecord->iSeg!=pLabelRecord->iSeg)
		{	//段号已发生变化，已移至当前段最前位置
			iCurSel++;
			break;
		}
		else if(pPrevLabelRecord->cNoType=='R')
			iCurSel--;	//当前构件记录为重复前一记录编号（非编号的第一个构件记录）
		else
			break;
	};
	//
	if(iCurSel<iFirstSelect)
		MoveItemListBefore(&itemList,iCurSel);
}

void CPartNoListDlg::OnBnDownMove()
{	//记录项下移
	int n=m_partNoList.GetSelectedCount();
	ARRAY_LIST<CSuperGridCtrl::CTreeItem*> itemList;
	POSITION pos=m_partNoList.GetFirstSelectedItemPosition();
	int iCurSel=m_partNoList.GetNextSelectedItem(pos);
	int iTailSelect=iCurSel;
	char sText[51]={0};
	PART_NO* pLabelRecord = NULL;
	while(iCurSel>=0)
	{
		CSuperGridCtrl::CTreeItem* pItem=m_partNoList.GetRootItem(iCurSel);
		itemList.append(pItem);
		m_partNoList.GetItemText(iCurSel,4,sText,50);//<=0)
		long h=HexStrToLong(sText);
		pLabelRecord = m_PartNoRecordList.GetValue(h);
		iTailSelect=iCurSel;
		iCurSel=m_partNoList.GetNextSelectedItem(pos);
	}
	iCurSel=iTailSelect+2;
	while(true){
		CSuperGridCtrl::CTreeItem* pItem=m_partNoList.GetTreeItem(iCurSel);
		if(pItem==NULL)
			break;
		m_partNoList.GetItemText(iCurSel,4,sText,50);//<=0)
		long h=HexStrToLong(sText);
		PART_NO* pAfterLabelRecord = m_PartNoRecordList.GetValue(h);
		if(pAfterLabelRecord==NULL||pAfterLabelRecord->iSeg!=pLabelRecord->iSeg)
			break;	//段号已发生变化，已移至当前段最后位置
		else if(pAfterLabelRecord->cNoType=='R')
			iCurSel++;	//当前构件记录为重复前一记录编号（非编号的第一个构件记录）
		else
			break;
	};
	//
	if(iCurSel>iTailSelect+1)
		MoveItemListBefore(&itemList,iCurSel);
}

int ComparePartNo(const PART_NO& partNo1,const PART_NO& partNo2)
{
	int i = ComparePartNoString((char*)partNo1.sPartNo,(char*)partNo2.sPartNo);
	if(i>0)
		return 1;
	else if(i<0)
		return -1;
	else
	{
		int order1=0,order2=0;
		if(partNo1.cNoType=='N')
			order1=0;
		else if(partNo1.cNoType=='R')
			order1=1;
		else if(partNo1.cNoType=='S')
			order1=2;
		if(partNo2.cNoType=='N')
			order2=0;
		else if(partNo2.cNoType=='R')
			order2=1;
		else if(partNo2.cNoType=='S')
			order2=2;
		if(order1>order2)
			return 1;
		else if(order1<order2)
			return -1;
		else if(partNo1.h>partNo2.h)
			return 1;
		else
			return -1;
	}
}
void CPartNoListDlg::OnRetrievePartNo() 
{
	UpdateData();
	sprintf(file_name,"%s",NULL);
	m_PartNoRecordList.Empty();
	m_partNoList.DeleteAllItems();
	int n = Ta.Parts.GetNodeNum();
	if(n<=0)		//空塔，不需进行编号
		return;
	CHashStrList<long>hashPartNo;
	CHashList<SEGI>hashSegTbl;
	GetSegNoHashTblBySegStr(m_sSegNo,hashSegTbl);
	CArray<PART_NO,PART_NO>partArr;
	PART_NO part_no;
#ifdef AFX_TARG_ENU_ENGLISH
	PutMessage(this,"processing the tower numbers statistics!");
#else
	PutMessage(this,"正在进行本塔编号统计!");
#endif
	for(CLDSPart *pPart = Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_BOLT||pPart->m_bVirtualPart)
			continue;	//跳过螺栓等不需统计的构件
		if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)pPart)->m_bStdPart)
			continue;	//标准构件不参与编号 
		if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			continue;
		if(strlen(pPart->GetPartNo())==0) //编号为空跳过
			continue;
		if(hashSegTbl.GetNodeNum()>0&&hashSegTbl.GetValue(pPart->iSeg.iSeg)==NULL)
			continue;	//不是当前统计段号
		part_no.h = pPart->handle;
		part_no.iSeg = pPart->iSeg;
		sprintf(part_no.sPartNo,"%s",(char*)pPart->GetPartNo());
		if (pPart->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)pPart)->m_iParamType==TYPE_FL)
			part_no.cPartType = 'F';
		else if(pPart->GetClassTypeId()==CLS_LINEANGLE||pPart->GetClassTypeId()==CLS_POLYPART)
			part_no.cPartType = 'L';
		else if(pPart->GetClassTypeId()==CLS_LINETUBE)
			part_no.cPartType = 'P';
		else if(pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE
			||(pPart->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)pPart)->m_iParamType==TYPE_ELBOWPLATE&&
			toupper(pPart->GetPartNo()[0])=='F'&&toupper(pPart->GetPartNo()[1])=='L'))//法兰肋板也为‘-’
			part_no.cPartType = '-';
		else 
			part_no.cPartType = '?';
		long *handle = hashPartNo.GetValue(pPart->GetPartNo());
		if(handle==NULL)
		{
			long *pPartNo = hashPartNo.Add(pPart->GetPartNo());
			pPartNo = &pPart->handle;
			part_no.cNoType='N';
		}
		else
			part_no.cNoType='R';	
		partArr.Add(part_no);
	}
	n = partArr.GetSize();
	CQuickSort<PART_NO>::QuickSort(partArr.GetData(),n,ComparePartNo);
	char sOldPartNo[50];
	if(n>0)
		strcpy(sOldPartNo,strtok(partArr[0].sPartNo,"-_#"));
	for(int i=0;i<n;i++)
	{
		PART_NO *pPartNoRecord = m_PartNoRecordList.Add(partArr[i].h);
		pPartNoRecord->h = partArr[i].h;
		pPartNoRecord->cNoType = partArr[i].cNoType;
		pPartNoRecord->cPartType = partArr[i].cPartType;
		pPartNoRecord->iSeg = partArr[i].iSeg;
		sprintf(pPartNoRecord->sPartNo,"%s",partArr[i].sPartNo);
		if(i>0)
		{
			char sCulPartNo[50];
			strcpy(sCulPartNo,strtok(partArr[i].sPartNo,"-_#"));
			if(strcmp(sCulPartNo,sOldPartNo)!=0)
			{
				if(sCulPartNo[0]==sOldPartNo[0])
				{
					int iOld,iCul;
					sscanf(&sCulPartNo[strlen(sCulPartNo)-1],"%X",&iCul);
					sscanf(&sOldPartNo[strlen(sOldPartNo)-1],"%X",&iOld);
					if(iCul - iOld>1)
						pPartNoRecord->cNoType = 'S';
				}
				strcpy(sOldPartNo,sCulPartNo);
			}
		}
		
	}
	PutMessage(this,NULL);
	UpdateNoList("*");
}

void CPartNoListDlg::OnImportPartNo() 
{
	m_PartNoRecordList.Empty();
	CFileDialog file_dlg(TRUE,"no","LDS",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"Numbering file(*.no)|*.no||");
#else
		"编号过程文件(*.no)|*.no||");
#endif
	if(file_dlg.DoModal()==IDOK)
	{
		char file_name[MAX_PATH],line_txt[MAX_PATH];
		_snprintf(file_name,MAX_PATH-1,"%s",file_dlg.GetPathName());
		FILE *fp=fopen(file_name,"rt");
		if(fp==NULL)
			return;	//未找到文件
		fgets(line_txt,MAX_PATH,fp);
		fgets(line_txt,MAX_PATH,fp);
		ATOM_LIST<SEG_NO>seg_list;
		partNoArr.RemoveAll();
		for(CSupplementPart *pSuppPart=Ta.SuppPart.GetFirst();pSuppPart;pSuppPart=Ta.SuppPart.GetNext())
		{	//统计附件构件的编号
			if(!FindPartNo(pSuppPart->GetPartNo()))
				partNoArr.Add(pSuppPart->GetPartNo());
		}
		while(!feof(fp))
		{
			if(fgets(line_txt,MAX_PATH,fp)==NULL)
				break;
			char *key=strtok(line_txt,",\t\n");
			long h=HexStrToLong(key);
			//if(key)
			//	sscanf(key,"%X",&h);
			PART_NO *pPartNoRecord=m_PartNoRecordList.Add(h);
			pPartNoRecord->h=h;
			key=strtok(NULL,",\t\n");
			if(key)
				pPartNoRecord->iSeg=SEGI(key);
			key=strtok(NULL,",\t\n");
			if(key)
				pPartNoRecord->cPartType=*key;
			key=strtok(NULL,",\t\n");
			if(key)
				pPartNoRecord->cNoType=*key;
			key=strtok(NULL,",\t\n");
			if(key)
				strcpy(pPartNoRecord->sPartNo,key);
			if(pPartNoRecord->cNoType=='S')	//指定构件编号
				if(!FindPartNo(pPartNoRecord->sPartNo))
					partNoArr.Add(pPartNoRecord->sPartNo);
		}
		fclose(fp);
		fp=fopen(file_name,"wt");
#ifdef AFX_TARG_ENU_ENGLISH
		fprintf(fp,"part's handle,segment No.，part's type,number type,part label.\n'L':angle;'P':tube;'-':plate;'F':flange;'N':new number;'R':repeated the above number;'S':specified as the latter's number\n");
#else
		fprintf(fp,"构件句柄，段号，构件类型，编号类型，构件编号\n'L':角钢;'P':钢管;'-':钢板;'F':法兰;'N':新编号;'R':重复上一编号;'S':指定为后面的编号\n");
#endif
		char sPrevPartNo[MAX_CHAR_GUIGE_16+1]="",sPartNo[MAX_CHAR_GUIGE_16+1]="",sCurrentFLPartNo[MAX_CHAR_GUIGE_16+1]="";
		int fl_serialno=0,elbow_serialno=0;
		int i=0;
		for(PART_NO *pPartNoRecord=m_PartNoRecordList.GetFirst();pPartNoRecord;pPartNoRecord=m_PartNoRecordList.GetNext())
		{
			CSuperSmartPtr<CLDSPart> pPart=console.FromPartHandle(pPartNoRecord->h);
			if(pPartNoRecord->cNoType=='N')	//新号
			{
				int str_len=strlen(pPartNoRecord->sPartNo);
				if(pPartNoRecord->cPartType=='F')
				{
					strcpy(sPartNo,"");
					while(FindPartNo(sPartNo))
					{
						char cFinal=pPartNoRecord->sPartNo[str_len-1];
						if(cFinal!='B')
							fl_serialno++;
						if(cFinal>'9'||cFinal<'0')
							_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"FL%d%C",fl_serialno,cFinal);
						else
							_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"FL%d",fl_serialno);
					}
					strcpy(sPrevPartNo,sPartNo);
				}
				else if(pPartNoRecord->cPartType=='-'&&pPartNoRecord->sPartNo[0]=='F')
				{	//跟法兰一起编号的肘板
					char fl_partno[MAX_PATH]="";
					strcpy(fl_partno,strtok(pPartNoRecord->sPartNo,"-_#"));
					if(strlen(sCurrentFLPartNo)==0)
					{
						strcpy(sCurrentFLPartNo,fl_partno);
						elbow_serialno=1;
					}
					else if(stricmp(sCurrentFLPartNo,fl_partno)==0)
						elbow_serialno++;
					else if(stricmp(sCurrentFLPartNo,fl_partno)!=0)
					{
						strcpy(sCurrentFLPartNo,fl_partno);
						elbow_serialno=1;
					}
					_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"%s-%d",sCurrentFLPartNo,elbow_serialno);
					strcpy(sPrevPartNo,sPartNo);
				}
				else
				{
					SEG_NO *pSegNo;
					for(pSegNo=seg_list.GetFirst();pSegNo;pSegNo=seg_list.GetNext())
					{
						if(pSegNo->iSeg.iSeg==pPartNoRecord->iSeg.iSeg)
						{
							strcpy(sPartNo,"");
							while(FindPartNo(sPartNo))
							{
								pSegNo->iSerialNo++;
								if(pSegNo->iSerialNo<10)
									_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"%s0%d",(char*)pPartNoRecord->iSeg.ToString(),pSegNo->iSerialNo);
								else if(pSegNo->iSerialNo<100)
									_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"%s%d",(char*)pPartNoRecord->iSeg.ToString(),pSegNo->iSerialNo);
								else
									_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"%s-%d",(char*)pPartNoRecord->iSeg.ToString(),pSegNo->iSerialNo);
							}
							strcpy(sPrevPartNo,sPartNo);
							break;
						}
					}
					if(pSegNo==NULL)
					{
						pSegNo=seg_list.append();
						pSegNo->iSeg=pPartNoRecord->iSeg;
						pSegNo->iSerialNo=0;
						strcpy(sPartNo,"");
						while(FindPartNo(sPartNo))
						{
							pSegNo->iSerialNo++;
							if(pSegNo->iSerialNo<10)
								_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"%s0%d",(char*)pPartNoRecord->iSeg.ToString(),pSegNo->iSerialNo);
							else if(pSegNo->iSerialNo<100)
								_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"%s%d",(char*)pPartNoRecord->iSeg.ToString(),pSegNo->iSerialNo);
							else
								_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"%s-%d",(char*)pPartNoRecord->iSeg.ToString(),pSegNo->iSerialNo);
							//_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"%d",pPartNoRecord->iSeg*100+pSegNo->iSerialNo);
						}
						strcpy(sPrevPartNo,sPartNo);
					}
				}
			}
			else if(pPartNoRecord->cNoType=='R')	//重复上一编号
				strcpy(sPartNo,sPrevPartNo);
			else if(pPartNoRecord->cNoType=='S')	//指定为后面的编号
			{
				strcpy(sPartNo,pPartNoRecord->sPartNo);
				strcpy(sPrevPartNo,sPartNo);
				if(pPartNoRecord->cPartType=='F')
					fl_serialno=RetrievePartSerialNo(sPartNo);
				else if(pPartNoRecord->cPartType=='-'&&pPartNoRecord->sPartNo[0]=='F')
					elbow_serialno=RetrievePartSerialNo(sPartNo);
				else
				{
					for(SEG_NO *pSegNo=seg_list.GetFirst();pSegNo;pSegNo=seg_list.GetNext())
					{
						if(pSegNo->iSeg.iSeg==pPartNoRecord->iSeg.iSeg)
						{
							pSegNo->iSerialNo=RetrievePartSerialNo(sPartNo);
							break;
						}
					}
					if(pSegNo==NULL)
					{
						pSegNo=seg_list.append();
						pSegNo->iSeg=pPartNoRecord->iSeg;
						pSegNo->iSerialNo=RetrievePartSerialNo(sPartNo);
					}
				}
			}
			strcpy(pPartNoRecord->sPartNo,sPartNo);
			if(pPart.IsHasPtr())
				pPart->SetPartNo(sPartNo);
			_snprintf(line_txt,MAX_PATH-1,"%4X,%s,%C,%C,%s",pPartNoRecord->h,(char*)pPartNoRecord->iSeg.ToString(),pPartNoRecord->cPartType,pPartNoRecord->cNoType,sPartNo);
			fprintf(fp,"%s\n",line_txt);
		}
		fclose(fp);
		POLYPARTLIST polylist;
		Ta.RetrievePolyPartList(polylist);
		CLDSPolyPart *pPolyPart=NULL;
		for(pPolyPart=polylist.GetFirst();pPolyPart;pPolyPart=polylist.GetNext())
		{
			CLDSLinePart *pLinePart=NULL,*pFirstLinePart=pPolyPart->segset.GetFirst();
			for(pLinePart=pPolyPart->segset.GetNext();pLinePart;pLinePart=pPolyPart->segset.GetNext())
				pLinePart->SetPartNo(pFirstLinePart->GetPartNo());
		}
		UpdateNoList(m_sSegNo);
	}
}

void CPartNoListDlg::OnBnExportPartNo() 
{
	if(m_PartNoRecordList.GetNodeNum()==0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("no data usable!");
#else
		MessageBox("没有可用数据!");
#endif
		return;
	}
	CFileDialog file_dlg(FALSE,"no","LDS",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"Numbering file(*.no)|*.no||");
#else
		"编号过程文件(*.no)|*.no||");
#endif
	if(file_dlg.DoModal()==IDOK)
	{
		char file_name[MAX_PATH];
		_snprintf(file_name,MAX_PATH-1,"%s",file_dlg.GetPathName());
		FILE *fp=fopen(file_name,"wt");
#ifdef AFX_TARG_ENU_ENGLISH
		fprintf(fp,"part's handle,segment No.,part's type,number type,part label.\n'L':angle;'P':tube;'-':plate;'F':flange;'N':new number;'R':repeated the above number;'S':specified as the latter's number\n");
#else
		fprintf(fp,"构件句柄，段号，构件类型，编号类型，构件编号\n'L':角钢;'P':钢管;'-':钢板;'F':法兰; 'N':新编号;'R':重复上一编号;'S':指定为后面的编号\n");
#endif
		for(PART_NO *pPartNoRecord=m_PartNoRecordList.GetFirst();pPartNoRecord;
		pPartNoRecord=m_PartNoRecordList.GetNext())
		{
			char line_txt[MAX_PATH];
			CXhChar16 idstr("0x%X",pPartNoRecord->h);
			idstr.ResizeLength(6,' ',true);
			_snprintf(line_txt,MAX_PATH-1,"%s,%s,%C,%C,%s",(char*)idstr,
				(char*)pPartNoRecord->iSeg.ToString(),pPartNoRecord->cPartType,pPartNoRecord->cNoType,pPartNoRecord->sPartNo);
			fprintf(fp,"%s\n",line_txt);
		}
		fclose(fp);
	}
}

void CPartNoListDlg::EmptyPartNoList()
{
	m_partNoList.DeleteAllItems();
	m_PartNoRecordList.Empty();
	strcpy(file_name,"");
}
#endif