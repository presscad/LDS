//<LOCALE_TRANSLATE Confirm by hxr/>
// PartGroupLocatePointDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "PartGroupLocatePointDlg.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "env_def.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CPartGroupLocatePointDlg dialog


CPartGroupLocatePointDlg::CPartGroupLocatePointDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPartGroupLocatePointDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartGroupLocatePointDlg)
	m_sDatumPart = _T("0X0");
	m_fLocatePointX = 0.0;
	m_fLocatePointY = 0.0;
	m_fLocatePointZ = 0.0;
	m_fStartOffset = 0.0;
	m_fEndOffset = 0.0;
	m_iYOrder = 2;
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("handle");
#else
	m_arrHeaderLabel.Add("句柄");
#endif
	m_arrHeaderLabel.Add("X");
	m_arrHeaderLabel.Add("Y");
	m_arrHeaderLabel.Add("Z");
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("grade");
	m_arrHeaderLabel.Add("start offset");
	m_arrHeaderLabel.Add("end offset");
#else
	m_arrHeaderLabel.Add("级别");
	m_arrHeaderLabel.Add("始端偏移量");
	m_arrHeaderLabel.Add("终端偏移量");
#endif
	m_pWorkView = NULL;
	m_pWorkPartGroup = NULL;
	m_bPauseBreakExit = FALSE;
}


void CPartGroupLocatePointDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartGroupLocatePointDlg)
	DDX_Text(pDX, IDC_E_DATUM_PART, m_sDatumPart);
	DDX_Text(pDX, IDC_E_LOCATE_POINT_X, m_fLocatePointX);
	DDX_Text(pDX, IDC_E_LOCATE_POINT_Y, m_fLocatePointY);
	DDX_Text(pDX, IDC_E_LOCATE_POINT_Z, m_fLocatePointZ);
	DDX_Text(pDX, IDC_E_START_OFFSET, m_fStartOffset);
	DDX_Text(pDX, IDC_E_END_OFFSET, m_fEndOffset);
	DDX_CBIndex(pDX, IDC_CMB_Y_ORDER, m_iYOrder);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPartGroupLocatePointDlg, CDialog)
	//{{AFX_MSG_MAP(CPartGroupLocatePointDlg)
	ON_BN_CLICKED(IDC_BTN_SELECT_PART, OnBtnSelectPart)
	ON_BN_CLICKED(IDC_BTN_MODIFY_LOCATE_POINT, OnBtnModifyLocatePoint)
	ON_BN_CLICKED(IDC_BTN_ADD_LOCATE_POINT, OnBtnAddLocatePoint)
	ON_NOTIFY(NM_CLICK, IDC_LIST_LOCATE_POINT, OnClickListLocatePoint)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_LOCATE_POINT, OnDblclkListLocatePoint)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_LOCATE_POINT, OnKeydownListLocatePoint)
	ON_BN_CLICKED(IDC_BTN_COPY_POINT, OnCopyPoint)
	ON_BN_CLICKED(IDC_BTN_PASTE_POINT, OnPastePoint)
	ON_EN_KILLFOCUS(IDC_E_DATUM_PART, OnKillfocusDatumPart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartGroupLocatePointDlg message handlers
BOOL CPartGroupLocatePointDlg::OnInitDialog() 
{
	m_bPauseBreakExit=FALSE;
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_LOCATE_POINT);
	if(m_pListCtrl)
		m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	if(m_pWorkView==NULL||m_pWorkPartGroup==NULL)
		return FALSE;
	for(CPartGroupLocatePoint *pLocatePoint=m_pWorkView->LocatePoints.GetFirst();pLocatePoint;pLocatePoint=m_pWorkView->LocatePoints.GetNext())
	{
		CLDSPart *pPart = console.FromPartHandle(pLocatePoint->m_hPart);
		CString tempStr;
		CStringArray str_arr;
		if(pPart==NULL)
			continue;
		tempStr.Format("0X%X",pLocatePoint->m_hPart);
		str_arr.Add(tempStr);
		tempStr.Format("%f",pLocatePoint->m_xLocalCoordPos.x);
		SimplifiedNumString(tempStr);
		str_arr.Add(tempStr);
		tempStr.Format("%f",pLocatePoint->m_xLocalCoordPos.y);
		SimplifiedNumString(tempStr);
		str_arr.Add(tempStr);
		tempStr.Format("%f",pLocatePoint->m_xLocalCoordPos.z);
		SimplifiedNumString(tempStr);
		str_arr.Add(tempStr);
		/*if(pLocatePoint->m_iOrderType==0)
			pLocatePoint->m_iOrderType=-1;
		tempStr.Format("%d",pLocatePoint->m_iYOrder);
		str_arr.Add(tempStr);
		tempStr.Format("%f",pLocatePoint->m_fStartOffset);
		SimplifiedNumString(tempStr);
		str_arr.Add(tempStr);
		tempStr.Format("%f",pLocatePoint->m_fEndOffset);
		SimplifiedNumString(tempStr);
		str_arr.Add(tempStr);*/
		int iCur=AppendRecord(-1,str_arr);
		m_pListCtrl->SetItemData(iCur,(DWORD)pPart);
	}
	long handle=0;
	sscanf(m_sDatumPart,"%X",&handle);
	CLDSPart *pPart=console.FromPartHandle(handle,CLS_LINEPART);
	GetDlgItem(IDC_E_START_OFFSET)->EnableWindow(pPart&&pPart->IsLinePart());
	GetDlgItem(IDC_E_END_OFFSET)->EnableWindow(pPart&&pPart->IsLinePart());
	CDialog::OnInitDialog();
	return TRUE;
}

void CPartGroupLocatePointDlg::RefreshListViewHeader()
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
			lvc.cx = 45;
		else if(i<=3)
			lvc.cx = 50;
		else 
			lvc.cx = 80;
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}
int CPartGroupLocatePointDlg::AppendRecord(int iItem, CStringArray &str_arr)
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
	return iItem;
}

void CPartGroupLocatePointDlg::OnBtnSelectPart() 
{
	m_bPauseBreakExit=TRUE;
	OnOK(); 
}

void CPartGroupLocatePointDlg::OnBtnModifyLocatePoint() 
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		UpdateData();
		CString ss;
		int iYOrder=1;
		CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_Y_ORDER);
		if(pCMB)
		{
			pCMB->GetLBText(m_iYOrder,ss);
			iYOrder=atoi(ss);
		}
		if(iCurSel>=0)
		{
			int nCount=m_pListCtrl->GetItemCount();
			for(int i=0;i<nCount;i++)
			{
				char sText[50];
				int iTempYOrder=1;
				CString sDatumPart;
				double x=0,y=0,z=0,startOffset=0,endOffset=0;
				if(i==iCurSel)
					continue;
				m_pListCtrl->GetItemText(i,0,sText,50);
				sDatumPart.Format("%s",sText);
				m_pListCtrl->GetItemText(i,1,sText,50);
				x=atof(sText);
				m_pListCtrl->GetItemText(i,2,sText,50);
				y=atof(sText);
				m_pListCtrl->GetItemText(i,3,sText,50);
				z=atof(sText);
				m_pListCtrl->GetItemText(i,4,sText,50);
				iTempYOrder=atoi(sText);
				m_pListCtrl->GetItemText(i,5,sText,50);
				startOffset=atof(sText);
				m_pListCtrl->GetItemText(i,6,sText,50);
				endOffset=atof(sText);
				if(m_sDatumPart.CompareNoCase(sDatumPart)==0&&iYOrder==iTempYOrder&&
					x==m_fLocatePointX&&y==m_fLocatePointY&&z==m_fLocatePointZ&&
					startOffset==m_fStartOffset&&endOffset==m_fEndOffset)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("Existing the same position locating point,cannot repeat to add!");
#else
					AfxMessageBox("已存在位置相同的定位点，不能重复添加!");
#endif
					return;
				}
			}
			//更新定位点列表
			ss.Empty();
			m_pListCtrl->SetItemText(iCurSel,0,m_sDatumPart);
			ss.Format("%f",m_fLocatePointX);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,1,ss);
			ss.Format("%f",m_fLocatePointY);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,2,ss);
			ss.Format("%f",m_fLocatePointZ);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,3,ss);
			ss.Format("%d",iYOrder);
			m_pListCtrl->SetItemText(iCurSel,4,ss);
			ss.Format("%f",m_fStartOffset);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,5,ss);
			ss.Format("%f",m_fEndOffset);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,6,ss);
		}
	}
}

void CPartGroupLocatePointDlg::OnBtnAddLocatePoint() 
{
	UpdateData();
	long h=0;
	CString tempStr;
	CStringArray str_arr;
	sscanf(m_sDatumPart,"%X",&h);
	CLDSPart *pPart = console.FromPartHandle(h);
	if(pPart==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Input a wrong handle, please add after input again!");
#else
		AfxMessageBox("输入了错误的句柄请重新输入后再添加!");
#endif
		return;
	}
	CString ss;
	int iYOrder=1;
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_Y_ORDER);
	if(pCMB)
	{
		pCMB->GetLBText(m_iYOrder,ss);
		iYOrder=atoi(ss);
	}
	int nCount=m_pListCtrl->GetItemCount();
	for(int i=0;i<nCount;i++)
	{
		char sText[50];
		int iTempYOrder=1;
		CString sDatumPart;
		double x=0,y=0,z=0,startOffset=0,endOffset=0;
		m_pListCtrl->GetItemText(i,0,sText,50);
		sDatumPart.Format("%s",sText);
		m_pListCtrl->GetItemText(i,1,sText,50);
		x=atof(sText);
		m_pListCtrl->GetItemText(i,2,sText,50);
		y=atof(sText);
		m_pListCtrl->GetItemText(i,3,sText,50);
		z=atof(sText);
		m_pListCtrl->GetItemText(i,4,sText,50);
		iTempYOrder=atoi(sText);
		m_pListCtrl->GetItemText(i,5,sText,50);
		startOffset=atof(sText);
		m_pListCtrl->GetItemText(i,6,sText,50);
		endOffset=atof(sText);
		if(m_sDatumPart.CompareNoCase(sDatumPart)==0&&iYOrder==iTempYOrder&&
			x==m_fLocatePointX&&y==m_fLocatePointY&&z==m_fLocatePointZ&&
			startOffset==m_fStartOffset&&endOffset==m_fEndOffset)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Existing the same position locating point,cannot repeat to add!");
#else
			AfxMessageBox("已存在位置相同的定位点，不能重复添加!");
#endif
			return;
		}
	}
	tempStr.Format("0X%X",pPart->handle);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fLocatePointX);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fLocatePointY);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fLocatePointZ);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	tempStr.Format("%d",iYOrder);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fStartOffset);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fEndOffset);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	int iCur=AppendRecord(-1,str_arr);
	m_pListCtrl->SetItemData(iCur,(DWORD)pPart);
}

void CPartGroupLocatePointDlg::OnOK() 
{
	if(m_pWorkView && m_pWorkView->LocatePoints.GetNodeNum()>0)
		m_pWorkView->LocatePoints.Empty();
	int nCount = m_pListCtrl->GetItemCount();
	for(int i=0;i<nCount;i++)
	{
		char sText[100];
		CPartGroupLocatePoint *pLocatePoint=NULL;
		m_pListCtrl->GetItemText(i,0,sText,100);
		long h=0;
		sscanf(sText,"%X",&h);
		CLDSPart *pPart = console.FromPartHandle(h);
		if(pPart==NULL)
			continue;
		pLocatePoint= m_pWorkView->LocatePoints.append();
		pLocatePoint->m_hPart = pPart->handle;
		m_pListCtrl->GetItemText(i,1,sText,100);
		pLocatePoint->m_xLocalCoordPos.x=atof(sText);
		m_pListCtrl->GetItemText(i,2,sText,100);
		pLocatePoint->m_xLocalCoordPos.y=atof(sText);
		m_pListCtrl->GetItemText(i,3,sText,100);
		pLocatePoint->m_xLocalCoordPos.z=atof(sText);
		/*m_pListCtrl->GetItemText(i,4,sText,100);
		pLocatePoint->m_iOrderType=atoi(sText);
		if(pPart->IsLinePart())
		{
			m_pListCtrl->GetItemText(i,5,sText,100);
			pLocatePoint->m_fStartOffset=atof(sText);
			m_pListCtrl->GetItemText(i,6,sText,100);
			pLocatePoint->m_fEndOffset=atof(sText);
		}*/
	}
	CDialog::OnOK();
}

void CPartGroupLocatePointDlg::SelectPart(int nCurSel) 
{
	f3dPoint locate_point;
	double fStartOffset=0,fEndOffset=0;
	CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	char tem_str[100];
	m_pListCtrl->GetItemText(nCurSel,0,tem_str,100);
	long hCurPart=0;
	sscanf(tem_str,"%X",&hCurPart);
	m_pListCtrl->GetItemText(nCurSel,1,tem_str,100);
	locate_point.x=atof(tem_str);
	m_pListCtrl->GetItemText(nCurSel,2,tem_str,100);
	locate_point.y=atof(tem_str);
	m_pListCtrl->GetItemText(nCurSel,3,tem_str,100);
	locate_point.y=atof(tem_str);
	m_pListCtrl->GetItemText(nCurSel,5,tem_str,100);
	fStartOffset=atof(tem_str);
	m_pListCtrl->GetItemText(nCurSel,6,tem_str,100);
	fEndOffset=atof(tem_str);
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(hCurPart);
	pLDSView->UpdatePropertyPage();
	//在屏幕上显示标定点位置
	CLDSPart *pRootPart=console.FromPartHandle(m_pWorkPartGroup->handle);
	if(pRootPart&&pRootPart->GetClassTypeId()==CLS_LINETUBE)
	{
		CLDSLineTube *pDatumTube=(CLDSLineTube*)pRootPart;
		CLDSPart *pPart=console.FromPartHandle(hCurPart);
		if(pPart==NULL)	//显示绝对坐标
			g_pSolidDraw->NewMark(locate_point,5,AtomType::mkRect);
		else
		{	//构件上的相对坐标
			if(pPart->GetClassTypeId()==CLS_LINETUBE&&pPart==pRootPart)
			{
				if(fabs(fStartOffset)>EPS)
					locate_point.z+=fStartOffset;
				if(fabs(fEndOffset)>EPS)
					locate_point.z+=fEndOffset;
			}
			else
				coord_trans(locate_point,pPart->ucs,TRUE);
			coord_trans(locate_point,pRootPart->ucs,FALSE);	//转换到基准钢管坐标系下
			locate_point.x=locate_point.y=0;				
			coord_trans(locate_point,pRootPart->ucs,TRUE);
			g_pSolidDraw->NewMark(locate_point,5,AtomType::mkRect);
		}
		g_pSolidDraw->Draw();
	}
}

void CPartGroupLocatePointDlg::OnClickListLocatePoint(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CPartGroupLocatePointDlg::OnDblclkListLocatePoint(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CPartGroupLocatePointDlg::OnKeydownListLocatePoint(NMHDR* pNMHDR, LRESULT* pResult) 
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
			while(iCurSel>=0)
			{
				m_pListCtrl->DeleteItem(iCurSel);
				pos=m_pListCtrl->GetFirstSelectedItemPosition();
				if(pos==NULL)
					break;
				iCurSel=m_pListCtrl->GetNextSelectedItem(pos);
			}
		}
	}
	*pResult = 0;
}

void CPartGroupLocatePointDlg::ModifyItem(int iCurSel)
{
	char sText[50];
	m_pListCtrl->GetItemText(iCurSel,0,sText,50);
	m_sDatumPart.Format("%s",sText);
	m_pListCtrl->GetItemText(iCurSel,1,sText,50);
	m_fLocatePointX=atof(sText);
	m_pListCtrl->GetItemText(iCurSel,2,sText,50);
	m_fLocatePointY=atof(sText);
	m_pListCtrl->GetItemText(iCurSel,3,sText,50);
	m_fLocatePointZ=atof(sText);
	m_pListCtrl->GetItemText(iCurSel,4,sText,50);
	CString ss=sText;
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_Y_ORDER);
	if(pCMB)
		m_iYOrder=pCMB->SelectString(0,ss);
	m_pListCtrl->GetItemText(iCurSel,5,sText,50);
	m_fStartOffset=atof(sText);
	m_pListCtrl->GetItemText(iCurSel,6,sText,50);
	m_fEndOffset=atof(sText);
	//根据输入的构件句柄，判断是否需要启用始终端偏移量
	long handle=0;
	sscanf(m_sDatumPart,"%X",&handle);
	CLDSPart *pPart=console.FromPartHandle(handle);
	GetDlgItem(IDC_E_START_OFFSET)->EnableWindow(pPart&&pPart->IsLinePart());
	GetDlgItem(IDC_E_END_OFFSET)->EnableWindow(pPart&&pPart->IsLinePart());
	UpdateData(FALSE);
}

void CPartGroupLocatePointDlg::OnCopyPoint() 
{
	UpdateData();
	f3dPoint point;
	point.Set(m_fLocatePointX,m_fLocatePointY,m_fLocatePointZ);
	WritePointToClip(point);
}

void CPartGroupLocatePointDlg::OnPastePoint() 
{
	f3dPoint point;
	ReadClipPoint(point);
	m_fLocatePointX = point.x;
	m_fLocatePointY = point.y;
	m_fLocatePointZ = point.z;
	UpdateData(FALSE);
}

void CPartGroupLocatePointDlg::OnKillfocusDatumPart() 
{
	UpdateData();
	long handle=0;
	sscanf(m_sDatumPart,"%X",&handle);
	CLDSPart *pPart=console.FromPartHandle(handle);
	GetDlgItem(IDC_E_START_OFFSET)->EnableWindow(pPart&&pPart->IsLinePart());
	GetDlgItem(IDC_E_END_OFFSET)->EnableWindow(pPart&&pPart->IsLinePart());
	if(pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Input a wrong handle,fail to find the specified part, please input again!");
#else
		AfxMessageBox("输入了错误的句柄，未找到指定构件请重新输入!");
#endif
	UpdateData(FALSE);
}
#endif