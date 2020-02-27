//<LOCALE_TRANSLATE Confirm by hxr/>
// LinkLinePartDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "LinkLinePartDlg.h"
#include "env_def.h"
#include "DesignLjPartPropDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CLinkLinePartDlg dialog
int CLinkLinePartDlg::nScrLocationX = (int)(GetSystemMetrics(SM_CXSCREEN)*0.3);
int CLinkLinePartDlg::nScrLocationY = (int)(GetSystemMetrics(SM_CXSCREEN)*0.2);
CLinkLinePartDlg::CLinkLinePartDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CLinkLinePartDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLinkLinePartDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hPlate=0;
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("handle");
	m_arrHeaderLabel.Add("number");
	m_arrHeaderLabel.Add("plane No.");
	m_arrHeaderLabel.Add("connecting leg");
	m_arrHeaderLabel.Add("connecting end");
	m_arrHeaderLabel.Add("type");
#else
	m_arrHeaderLabel.Add("句柄");
	m_arrHeaderLabel.Add("编号");
	m_arrHeaderLabel.Add("面号");
	m_arrHeaderLabel.Add("连接肢");
	m_arrHeaderLabel.Add("连接端");
	m_arrHeaderLabel.Add("类型");
#endif
}


void CLinkLinePartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinkLinePartDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLinkLinePartDlg, CDialog)
	//{{AFX_MSG_MAP(CLinkLinePartDlg)
	ON_WM_MOVE()
	ON_WM_CREATE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_LINK_LINEPART, OnDblclkListLinkLinepart)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_LINK_LINEPART, OnKeydownListLinkLinepart)
	ON_BN_CLICKED(IDC_BN_ADD_LINEPART, OnBnAddLinePart)
	ON_NOTIFY(NM_CLICK, IDC_LIST_LINK_LINEPART, OnClickListLinePartset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkLinePartDlg message handlers
//From DesignLjParaDlg.cpp
void InitLjParaListCtrlValue(CStringArray& str_arr,CDesignLjPartPara *pLinePartPara);
BOOL CLinkLinePartDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if (m_bInernalStart)	//内部启动
		FinishSelectObjOper(m_hPickObj, m_idEventProp);
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_LINK_LINEPART);
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT
		| LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	CStringArray str_arr;
	for(CDesignLjPartPara *pLinePartPara=m_pLjPara->partList.GetFirst();pLinePartPara;pLinePartPara=m_pLjPara->partList.GetNext())
	{
		InitLjParaListCtrlValue(str_arr,pLinePartPara);
		AppendRecord(-1,str_arr);
	}
	//
	CRect rect;
	GetWindowRect(rect);
	int width = rect.Width();
	int height = rect.Height();
	rect.left = nScrLocationX;
	rect.top = nScrLocationY;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	MoveWindow(rect, TRUE);
	m_bEnableWindowMoveListen = TRUE;
	UpdateData(FALSE);
	return TRUE;
}

void CLinkLinePartDlg::RefreshListViewHeader()
{
	LV_COLUMN lvc;
	int i, NUM_COLUMNS, nColumnCount;
	CHeaderCtrl *pHeader = m_pListCtrl->GetHeaderCtrl();
	nColumnCount = pHeader->GetItemCount();

	// Delete all of the columns.
	for (i=0;i < nColumnCount;i++)
	{
	   m_pListCtrl->DeleteColumn(0);
	}
	if(m_arrHeaderLabel.GetSize()>0)
		NUM_COLUMNS = m_arrHeaderLabel.GetSize();
	else
		NUM_COLUMNS = 0;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for(i = 0; i<NUM_COLUMNS; i++)
	{
		lvc.iSubItem = i;
		lvc.pszText = m_arrHeaderLabel[i].GetBuffer(1);//_gszColumnLabel[i];

#ifdef AFX_TARG_ENU_ENGLISH
		lvc.cx = 80;
#else 
		lvc.cx = 60;
#endif
		if(i==5)
			lvc.cx+=20;
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}
void CLinkLinePartDlg::AppendRecord(int iItem, CStringArray &str_arr)
{
	//insert items
	LV_ITEM lvi;
	if(iItem==-1)
		iItem=m_pListCtrl->GetItemCount();

	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.pszText = str_arr[0].GetBuffer(1);//_gszItem[i][0];
	iItem = m_pListCtrl->InsertItem(&lvi);
	//set item text for additional columns
	int nCount = m_arrHeaderLabel.GetSize();
	nCount = __min(nCount,str_arr.GetSize());
	for(int j = 1; j<nCount; j++)
	{
		str_arr[j].TrimLeft();
		str_arr[j].TrimRight();
		m_pListCtrl->SetItemText(iItem,j,str_arr[j]);
	}
	m_pListCtrl->RedrawItems(iItem,iItem);
}

void CLinkLinePartDlg::DeleteItem(int nCurSel) 
{
	char tem_str[101];
	long hPart=0;
	m_pListCtrl->GetItemText(nCurSel,0,tem_str,100);
	sscanf(tem_str,"%X",&hPart);
	for(CDesignLjPartPara *pLjPara=m_pLjPara->partList.GetFirst(); pLjPara; pLjPara=m_pLjPara->partList.GetNext())
	{
		if(pLjPara->hPart==hPart)
		{
			m_pLjPara->partList.DeleteCursor();
			break;
		}
	}
	m_pListCtrl->DeleteItem(nCurSel);
}

void CLinkLinePartDlg::ModifyItem(int iCurSel) 
{
	char sText[MAX_TEMP_CHAR_50 + 1] = "";
	m_pListCtrl->GetItemText(iCurSel,0,sText,50);
	long hLinePart = 0;
	sscanf(sText,"%X",&hLinePart);
	CDesignLjPartPara *pLinePartPara = m_pLjPara->FromHandle(hLinePart);
	if(pLinePartPara==NULL)
		return;
	CDesignLjPartPropDlg lj_dlg;
	lj_dlg.m_hPlate = m_hPlate;
	lj_dlg.m_pLjPara = m_pLjPara;
	lj_dlg.m_xLjPartPara=*pLinePartPara;
	if(lj_dlg.DoModal()!=IDOK)
		return;
	*pLinePartPara=lj_dlg.m_xLjPartPara;
	CStringArray str_arr;
	InitLjParaListCtrlValue(str_arr,pLinePartPara);
	for(int j=0;j<6;j++)
		m_pListCtrl->SetItemText(iCurSel,j,str_arr[j]);
}
int CLinkLinePartDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bEnableWindowMoveListen = FALSE;
	if (CCallBackDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
void CLinkLinePartDlg::OnMove(int x, int y)
{
	CCallBackDialog::OnMove(x, y);
	if (m_bEnableWindowMoveListen)
	{
		nScrLocationX = x;
		nScrLocationY = y;
	}
}
void CLinkLinePartDlg::OnDblclkListLinkLinepart(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(iCurSel>=0)
			ModifyItem(iCurSel);
	}
	*pResult = 0;
}


void CLinkLinePartDlg::OnKeydownListLinkLinepart(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_F3)
		{
			if(iCurSel>=0)
				ModifyItem(iCurSel);
		}
		else if(pLVKeyDow->wVKey==VK_DELETE)
		{
			if(iCurSel>=0)
				DeleteItem(iCurSel);
		}
		else if(pLVKeyDow->wVKey==VK_UP)
		{	//按一次UP键选中项会上移一位，但是这个操作是在按下UP键时触发的
			//此时还并未移动，所以手动调整选中项的位置
			if(iCurSel>=1)
				iCurSel--;	
			if(iCurSel>=0)
				SelectPart(iCurSel);
		}
		else if(pLVKeyDow->wVKey==VK_DOWN)
		{	//按一次DOWN键选中项会下移一位，但是这个操作是在按下DOWN键时触发的
			//此时还并未移动，所以手动调整选中项的位置
			if(iCurSel<m_pListCtrl->GetItemCount())
				iCurSel++;
			if(iCurSel>=0)
				SelectPart(iCurSel);
		}

	}
	*pResult = 0;
}

void CLinkLinePartDlg::OnBnAddLinePart() 
{
	SelectObject(CLS_LINEPART, 1);
}
BOOL CLinkLinePartDlg::FinishSelectObjOper(long hPickObj, long idEventProp)
{
	if (idEventProp == 1)
	{
		CLDSPlate* pPlate = (CLDSPlate*)console.FromPartHandle(m_hPlate);
		CLDSLinePart* pRod = console.FromRodHandle(hPickObj);
		if (pPlate == NULL || pRod == NULL)
			return FALSE;
		CDesignLjPartPropDlg lj_dlg;
		lj_dlg.m_bNewAddRod = TRUE;
		lj_dlg.m_pLjPara = m_pLjPara;
		lj_dlg.m_xLjPartPara.iFaceNo = 1;
		lj_dlg.m_xLjPartPara.hPart = pRod->handle;
		if (pRod->pStart && pRod->pStart->handle == pPlate->m_hBaseNode)
			lj_dlg.m_xLjPartPara.start0_end1 = 0;
		else if(pRod->pEnd && pRod->pEnd->handle==pPlate->m_hBaseNode)
			lj_dlg.m_xLjPartPara.start0_end1 = 1;
		else
			lj_dlg.m_xLjPartPara.start0_end1 = 2;
		if (pPlate && pRod->IsAngle())
		{
			CLDSLineAngle* pAngle = (CLDSLineAngle*)pRod;
			double ddx = pPlate->ucs.axis_z*pAngle->vxWingNorm;
			double ddy = pPlate->ucs.axis_z*pAngle->vyWingNorm;
			lj_dlg.m_xLjPartPara.angle.cur_wing_x0_y1 = (fabs(ddx) > fabs(ddy)) ? 0 : 1;
		}
		if (lj_dlg.DoModal() != IDOK)
			return FALSE;
		m_pLjPara->partList.SetValue(lj_dlg.m_xLjPartPara.hPart, lj_dlg.m_xLjPartPara);
		for (CLDSBolt* pBolt = lj_dlg.m_xSelLinkLs.GetFirst(); pBolt; pBolt = lj_dlg.m_xSelLinkLs.GetNext())
		{
			if (pRod->FindLsByHandle(pBolt->handle))
				pPlate->AppendLsRef(pBolt->GetLsRef());
		}
		return TRUE;
	}
	return FALSE;
}
void CLinkLinePartDlg::SelectPart(int nCurSel) 
{
	char tem_str[101]="";
	m_pListCtrl->GetItemText(nCurSel,0,tem_str,100);
	long hCurPart=0;
	sscanf(tem_str,"%X",&hCurPart);
	CLDSLinePart* pRod = console.FromRodHandle(hCurPart);
	CLDSPlate* pPlate = (CLDSPlate*)console.FromPartHandle(m_hPlate);
	g_pSolidDraw->ReleaseSnapStatus(); 
	if(pRod)
		g_pSolidDraw->SetEntSnapStatus(hCurPart);
	if (pPlate && pRod)
	{	//选中连接螺栓
		for (CLsRef* pLsRef = pPlate->GetFirstLsRef(); pLsRef; pLsRef = pPlate->GetNextLsRef())
		{
			CLDSBolt* pBolt = pLsRef->GetLsPtr();
			if(pRod->FindLsByHandle(pBolt->handle))
				g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
		}
	}
}
void CLinkLinePartDlg::OnClickListLinePartset(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(iCurSel>=0)
			SelectPart(iCurSel);
	}
	*pResult = 0;
}
#endif
