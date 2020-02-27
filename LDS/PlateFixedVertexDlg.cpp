// PlateFixedVertexDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "PlateFixedVertexDlg.h"
#include "DesignVertexParaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CPlateFixedVertexDlg dialog
CPlateFixedVertexDlg::CPlateFixedVertexDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlateFixedVertexDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlateFixedVertexDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("Plane");
	m_arrHeaderLabel.Add("X");
	m_arrHeaderLabel.Add("Y");
#else 
	m_arrHeaderLabel.Add("面号");
	m_arrHeaderLabel.Add("X坐标");
	m_arrHeaderLabel.Add("Y坐标");
#endif
}


void CPlateFixedVertexDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlateFixedVertexDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlateFixedVertexDlg, CDialog)
	//{{AFX_MSG_MAP(CPlateFixedVertexDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PLATE_FIXED_VERTEX, OnDblclkListPlateFixedVertex)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_PLATE_FIXED_VERTEX, OnKeydownListPlateFixedVertex)
	ON_BN_CLICKED(IDC_BN_ADD_VERTICE, OnBnAddVertice)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlateFixedVertexDlg message handlers

BOOL CPlateFixedVertexDlg::OnInitDialog() 
{
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_PLATE_FIXED_VERTEX);
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT
		| LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	for(f3dPoint *pPoint=m_pLjPara->additional_vertex.GetFirst();pPoint;pPoint=m_pLjPara->additional_vertex.GetNext())
	{
		CString tempStr;
		CStringArray str_arr;
		tempStr.Format("%d",pPoint->feature);
		str_arr.Add(tempStr);
		tempStr.Format("%.2f",pPoint->x);
		str_arr.Add(tempStr);
		tempStr.Format("%.2f",pPoint->y);
		str_arr.Add(tempStr);
		AppendRecord(-1,str_arr);
	}
	return CDialog::OnInitDialog();
}

void CPlateFixedVertexDlg::RefreshListViewHeader()
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
		if(i==0)
			lvc.cx = 40;
		else
			lvc.cx = 80;
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}
void CPlateFixedVertexDlg::AppendRecord(int iItem, CStringArray &str_arr)
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

void CPlateFixedVertexDlg::DeleteItem(int iCurSel) 
{
	char svalx[50],svaly[50];
	m_pListCtrl->GetItemText(iCurSel,1,svalx,50);
	m_pListCtrl->GetItemText(iCurSel,2,svaly,50);
	double valx,valy;
	sscanf(svalx,"%lf",&valx);
	sscanf(svaly,"%lf",&valy);
	for(f3dPoint *pPoint=m_pLjPara->additional_vertex.GetFirst();pPoint;pPoint=m_pLjPara->additional_vertex.GetNext())
	{
		if(valx==pPoint->x&&valy==pPoint->y)
		{
			m_pLjPara->additional_vertex.DeleteCursor();
			break;
		}
	}
	m_pListCtrl->DeleteItem(iCurSel);
}

void CPlateFixedVertexDlg::ModifyItem(int iCurSel) 
{
	CDesignVertexParaDlg vertice_dlg;
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_LIST_PLATE_FIXED_VERTEX);
	char sText[50];
	double valx,valy;
	pListCtrl->GetItemText(iCurSel,1,sText,50);
	sscanf(sText,"%lf",&valx);
	pListCtrl->GetItemText(iCurSel,2,sText,50);
	sscanf(sText,"%lf",&valy);
	f3dPoint *pPoint;
	for(pPoint=m_pLjPara->additional_vertex.GetFirst();pPoint;pPoint=m_pLjPara->additional_vertex.GetNext())
	{
		if(pPoint->x==valx&&pPoint->y==valy)
			break;
	}
	if(pPoint)
	{
		vertice_dlg.m_fPosX=valx;
		vertice_dlg.m_fPosY=valy;
		vertice_dlg.m_iFaceI=pPoint->feature-1;
		if(vertice_dlg.DoModal()!=IDOK)
			return;
		pPoint->x=vertice_dlg.m_fPosX;
		pPoint->y=vertice_dlg.m_fPosY;
		pPoint->feature=vertice_dlg.m_iFaceI+1;
		CStringArray str_arr;
		str_arr.SetSize(3);
		str_arr[0].Format("%d",pPoint->feature);
		str_arr[1].Format("%f",pPoint->x);
		str_arr[2].Format("%f",pPoint->y);
		for(int j=0;j<3;j++)
			pListCtrl->SetItemText(iCurSel,j,str_arr[j]);
	}
}

void CPlateFixedVertexDlg::OnDblclkListPlateFixedVertex(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CPlateFixedVertexDlg::OnKeydownListPlateFixedVertex(NMHDR* pNMHDR, LRESULT* pResult) 
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
	}
	*pResult = 0;
}

void CPlateFixedVertexDlg::OnBnAddVertice() 
{
	CDesignVertexParaDlg vertice_dlg;
	if(vertice_dlg.DoModal()!=IDOK)
		return;
	f3dPoint *pPoint;
	for(pPoint=m_pLjPara->additional_vertex.GetFirst();pPoint;pPoint=m_pLjPara->additional_vertex.GetNext())
	{
		if(pPoint->x==vertice_dlg.m_fPosX&&pPoint->y==vertice_dlg.m_fPosY)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("The position has point, the input point is invalid!");
#else 
			AfxMessageBox("此位置已有点,输入轮廓点无效!");
#endif
			return;
		}
	}
	pPoint=m_pLjPara->additional_vertex.append();
	pPoint->x=vertice_dlg.m_fPosX;
	pPoint->y=vertice_dlg.m_fPosY;
	pPoint->feature=vertice_dlg.m_iFaceI+1;
	CStringArray str_arr;
	str_arr.SetSize(3);
	str_arr[0].Format("%d",pPoint->feature);
	str_arr[1].Format("%f",pPoint->x);
	str_arr[2].Format("%f",pPoint->y);
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_LIST_VERTICE);
	AppendRecord(-1,str_arr);
}
#endif