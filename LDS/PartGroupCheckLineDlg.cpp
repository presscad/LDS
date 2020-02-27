//<LOCALE_TRANSLATE Confirm by hxr/>
// PartGroupCheckLineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "PartGroupCheckLineDlg.h"
#include "LDSView.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CPartGroupCheckLineDlg dialog


CPartGroupCheckLineDlg::CPartGroupCheckLineDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPartGroupCheckLineDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartGroupCheckLineDlg)
	m_fEndPointX = 0.0;
	m_fEndPointY = 0.0;
	m_fEndPointZ = 0.0;
	m_sStartPartHandle = _T("0X0");
	m_sEndPartHandle = _T("0X0");
	m_fStartPointX = 0.0;
	m_fStartPointY = 0.0;
	m_fStartPointZ = 0.0;
	m_bBeforeWeld = FALSE;
	m_sPrompt = _T("");
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("starting parts");
	m_arrHeaderLabel.Add("ending parts");
	m_arrHeaderLabel.Add("type");
	m_arrHeaderLabel.Add("starting point X");
	m_arrHeaderLabel.Add("starting point Y");
	m_arrHeaderLabel.Add("starting point Z");
	m_arrHeaderLabel.Add("ending point X");
	m_arrHeaderLabel.Add("ending point Y");
	m_arrHeaderLabel.Add("ending point Z");
#else
	m_arrHeaderLabel.Add("起始构件");
	m_arrHeaderLabel.Add("终止构件");
	m_arrHeaderLabel.Add("类型");
	m_arrHeaderLabel.Add("起始点X");
	m_arrHeaderLabel.Add("起始点Y");
	m_arrHeaderLabel.Add("起始点Z");
	m_arrHeaderLabel.Add("终止点X");
	m_arrHeaderLabel.Add("终止点Y");
	m_arrHeaderLabel.Add("终止点Z");
#endif
	m_pWorkView = NULL;
	m_pWorkPartGroup = NULL;
	m_bPauseBreakExit = FALSE;
}


void CPartGroupCheckLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartGroupCheckLineDlg)
	DDX_Text(pDX, IDC_E_END_POINT_X, m_fEndPointX);
	DDX_Text(pDX, IDC_E_END_POINT_Y, m_fEndPointY);
	DDX_Text(pDX, IDC_E_END_POINT_Z, m_fEndPointZ);
	DDX_Text(pDX, IDC_E_START_PART, m_sStartPartHandle);
	DDX_Text(pDX, IDC_E_END_PART, m_sEndPartHandle);
	DDX_Text(pDX, IDC_E_START_POINT_X, m_fStartPointX);
	DDX_Text(pDX, IDC_E_START_POINT_Y, m_fStartPointY);
	DDX_Text(pDX, IDC_E_START_POINT_Z, m_fStartPointZ);
	DDX_Check(pDX, IDC_CHK_BEFORE_WELD, m_bBeforeWeld);
	DDX_Text(pDX, IDC_S_PROMPT, m_sPrompt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPartGroupCheckLineDlg, CDialog)
	//{{AFX_MSG_MAP(CPartGroupCheckLineDlg)
	ON_BN_CLICKED(IDC_BTN_PASTE_START_POINT, OnPasteStartPoint)
	ON_BN_CLICKED(IDC_BTN_SELECT_PART, OnSelectPart)
	ON_BN_CLICKED(IDC_BTN_PASTE_END_POINT, OnPasteEndPoint)
	ON_BN_CLICKED(IDC_BTN_ADD_CHECK_LINE, OnAddCheckLine)
	ON_BN_CLICKED(IDC_BTN_COPY_END_POINT, OnCopyEndPoint)
	ON_BN_CLICKED(IDC_BTN_COPY_START_POINT, OnCopyStartPoint)
	ON_BN_CLICKED(IDC_BTN_MODIFY_CHECK_LINE, OnModifyCheckLine)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CHECK_LINE, OnClickListCheckLine)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CHECK_LINE, OnDblclkListCheckLine)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_CHECK_LINE, OnKeydownListCheckLine)
	ON_BN_CLICKED(IDC_CHK_BEFORE_WELD, OnBeforeWeld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartGroupCheckLineDlg message handlers

BOOL CPartGroupCheckLineDlg::OnInitDialog() 
{
	m_bPauseBreakExit=FALSE;
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_CHECK_LINE);
	if(m_pListCtrl)
		m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	if(m_pWorkView==NULL||m_pWorkPartGroup==NULL)
		return FALSE;
	for(CCheckLineDimension *pCheckLine=m_pWorkView->CheckLines.GetFirst();pCheckLine;pCheckLine=m_pWorkView->CheckLines.GetNext())
	{
		CLDSPart *pStartPart = console.FromPartHandle(pCheckLine->m_hStartPart);
		CLDSPart *pEndPart = console.FromPartHandle(pCheckLine->m_hEndPart);
		if((pCheckLine->m_bSpecStartLocatePart&&pStartPart==NULL)||(pCheckLine->m_bSpecEndLocatePart&&pEndPart==NULL))
			continue;
		CString tempStr;
		CStringArray str_arr;
		tempStr.Format("0X%X",pCheckLine->m_hStartPart);
		str_arr.Add(tempStr);
		tempStr.Format("0X%X",pCheckLine->m_hEndPart);
		str_arr.Add(tempStr);
		if(pCheckLine->m_bBeforeWeld)
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr.Add("Before Welding");
		else
			str_arr.Add("After Welding");
#else
			str_arr.Add("焊接前");
		else
			str_arr.Add("焊接后");
#endif
		tempStr.Format("%f",pCheckLine->m_xStartLocalCoordPos.x);
		SimplifiedNumString(tempStr);
		str_arr.Add(tempStr);
		tempStr.Format("%f",pCheckLine->m_xStartLocalCoordPos.y);
		SimplifiedNumString(tempStr);
		str_arr.Add(tempStr);
		tempStr.Format("%f",pCheckLine->m_xStartLocalCoordPos.z);
		SimplifiedNumString(tempStr);
		str_arr.Add(tempStr);
		tempStr.Format("%f",pCheckLine->m_xEndLocalCoordPos.x);
		SimplifiedNumString(tempStr);
		str_arr.Add(tempStr);
		tempStr.Format("%f",pCheckLine->m_xEndLocalCoordPos.y);
		SimplifiedNumString(tempStr);
		str_arr.Add(tempStr);
		tempStr.Format("%f",pCheckLine->m_xEndLocalCoordPos.z);
		SimplifiedNumString(tempStr);
		str_arr.Add(tempStr);
		AppendRecord(-1,str_arr);
	}
	if(m_bBeforeWeld)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sPrompt="==>dimension the vertical distance between bolts and plate and tube";
	else
		m_sPrompt="==>dimension the straight-line distance between two parts";
#else
		m_sPrompt="==>标注螺栓到钢板、钢管的垂直距离";
	else
		m_sPrompt="==>标注两构件之间的直线距离";
#endif
	GetDlgItem(IDC_E_START_POINT_X)->EnableWindow(!m_bBeforeWeld);
	GetDlgItem(IDC_E_START_POINT_Y)->EnableWindow(!m_bBeforeWeld);
	GetDlgItem(IDC_E_START_POINT_Z)->EnableWindow(!m_bBeforeWeld);
	GetDlgItem(IDC_E_END_POINT_X)->EnableWindow(!m_bBeforeWeld);
	GetDlgItem(IDC_E_END_POINT_Y)->EnableWindow(!m_bBeforeWeld);
	GetDlgItem(IDC_E_END_POINT_Z)->EnableWindow(!m_bBeforeWeld);
	CDialog::OnInitDialog();
	return TRUE;
}

void CPartGroupCheckLineDlg::RefreshListViewHeader()
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
		lvc.cx = 70;
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}

int CPartGroupCheckLineDlg::AppendRecord(int iItem, CStringArray &str_arr)
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

void CPartGroupCheckLineDlg::OnSelectPart() 
{
	UpdateData();
	m_bPauseBreakExit=TRUE;
	OnOK();
}

void CPartGroupCheckLineDlg::OnAddCheckLine() 
{
	UpdateData();
	long hStart=0, hEnd=0;
	CString tempStr;
	CStringArray str_arr;
	sscanf(m_sStartPartHandle,"%X",&hStart);
	sscanf(m_sEndPartHandle,"%X",&hEnd);
	CLDSPart *pStartPart = console.FromPartHandle(hStart);
	CLDSPart *pEndPart = console.FromPartHandle(hEnd);
	if(pStartPart==NULL||pEndPart==NULL)
	{
		m_sStartPartHandle="0X0";
		m_sEndPartHandle="0X0";
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("wrong handle input, please input again and then add!");
#else
		AfxMessageBox("输入了错误的句柄请重新输入后再添加!");
#endif
		return;
	}
	if(((pStartPart->GetClassTypeId()!=CLS_BOLT&&pEndPart->GetClassTypeId()!=CLS_BOLT)
		||(pStartPart->GetClassTypeId()==CLS_BOLT&&pEndPart->GetClassTypeId()==CLS_BOLT))
		&&m_bBeforeWeld)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("checking line only supports the dimension between bolts and tube or plate before welding!");
#else
		AfxMessageBox("焊接前检测线仅支持螺栓到钢管或钢板的尺寸标注!");
#endif
		return;
	}
	int nCount=m_pListCtrl->GetItemCount();
	for(int i=0;i<nCount;i++)
	{
		BOOL bBeforeWeld=FALSE;
		char sText[50];
		CString sStartPart,sEndPart;
		f3dPoint start,end;
		m_pListCtrl->GetItemText(i,0,sText,50);
		sStartPart.Format("%s",sText);
		m_pListCtrl->GetItemText(i,1,sText,50);
		sEndPart.Format("%s",sText);
		m_pListCtrl->GetItemText(i,2,sText,50);
#ifdef AFX_TARG_ENU_ENGLISH
		if(strcmp(sText,"Before Welding")==0)
#else
		if(strcmp(sText,"焊接前")==0)
#endif
			bBeforeWeld=TRUE;
		m_pListCtrl->GetItemText(i,3,sText,50);
		start.x=atof(sText);
		m_pListCtrl->GetItemText(i,4,sText,50);
		start.y=atof(sText);
		m_pListCtrl->GetItemText(i,5,sText,50);
		start.z=atof(sText);
		m_pListCtrl->GetItemText(i,6,sText,50);
		end.x=atof(sText);
		m_pListCtrl->GetItemText(i,7,sText,50);
		end.y=atof(sText);
		m_pListCtrl->GetItemText(i,8,sText,50);
		end.z=atof(sText);
		if(m_sStartPartHandle.CompareNoCase(sStartPart)==0&&
			m_sEndPartHandle.CompareNoCase(sEndPart)==0&&(bBeforeWeld||(!bBeforeWeld&&
			start.x==m_fStartPointX&&start.y==m_fStartPointY&&start.z==m_fStartPointZ&&
			end.x==m_fEndPointX&&end.y==m_fEndPointY&&end.z==m_fEndPointZ)))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("the same checking line has existed, no permission to repeat adding!");
#else
			AfxMessageBox("已存在相同的检测线，不能重复添加!");
#endif
			return;
		}
	}
	if(pStartPart)
		tempStr.Format("0X%X",pStartPart->handle);
	else
		tempStr.Format("0X%X",0);
	str_arr.Add(tempStr);
	if(pEndPart)
		tempStr.Format("0X%X",pEndPart->handle);
	else
		tempStr.Format("0X%X",0);
	str_arr.Add(tempStr);
	if(m_bBeforeWeld)
#ifdef AFX_TARG_ENU_ENGLISH
		str_arr.Add("Before Welding");
	else 
		str_arr.Add("After Welding");
#else
		str_arr.Add("焊接前");
	else 
		str_arr.Add("焊接后");
#endif
	tempStr.Format("%f",m_fStartPointX);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fStartPointY);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fStartPointZ);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fEndPointX);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fEndPointY);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fEndPointZ);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	AppendRecord(-1,str_arr);
}

void CPartGroupCheckLineDlg::OnModifyCheckLine() 
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		UpdateData();
		if(iCurSel>=0)
		{
			long hStart=0, hEnd=0;
			sscanf(m_sStartPartHandle,"%X",&hStart);
			sscanf(m_sEndPartHandle,"%X",&hEnd);
			CLDSPart *pStartPart = console.FromPartHandle(hStart);
			CLDSPart *pEndPart = console.FromPartHandle(hEnd);
			if(pStartPart==NULL||pEndPart==NULL)
			{
				m_sStartPartHandle="0X0";
				m_sEndPartHandle="0X0";
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("wrong handle input, please input again and then add!");
#else
				AfxMessageBox("输入了错误的句柄请重新输入后再添加!");
#endif
				return;
			}
			if(((pStartPart->GetClassTypeId()!=CLS_BOLT&&pEndPart->GetClassTypeId()!=CLS_BOLT)
				||(pStartPart->GetClassTypeId()==CLS_BOLT&&pEndPart->GetClassTypeId()==CLS_BOLT))
				&&m_bBeforeWeld)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("checking line only supports the dimension between bolts and tube or plate before welding!");
#else
				AfxMessageBox("焊接前检测线仅支持螺栓到钢管或钢板的尺寸标注!");
#endif
				return;
			}
			BOOL bBeforeWeld=FALSE;
			int nCount=m_pListCtrl->GetItemCount();
			for(int i=0;i<nCount;i++)
			{
				char sText[50];
				CString sStartPart,sEndPart;
				f3dPoint start,end;
				if(i==iCurSel)
					continue;
				m_pListCtrl->GetItemText(i,0,sText,50);
				sStartPart.Format("%s",sText);
				m_pListCtrl->GetItemText(i,1,sText,50);
				sEndPart.Format("%s",sText);
				m_pListCtrl->GetItemText(i,2,sText,50);
#ifdef AFX_TARG_ENU_ENGLISH
				if(strcmp(sText,"Before Welding")==0)
#else
				if(strcmp(sText,"焊接前")==0)
#endif
					bBeforeWeld=TRUE;
				m_pListCtrl->GetItemText(i,3,sText,50);
				start.x=atof(sText);
				m_pListCtrl->GetItemText(i,4,sText,50);
				start.y=atof(sText);
				m_pListCtrl->GetItemText(i,5,sText,50);
				start.z=atof(sText);
				m_pListCtrl->GetItemText(i,6,sText,50);
				end.x=atof(sText);
				m_pListCtrl->GetItemText(i,7,sText,50);
				end.y=atof(sText);
				m_pListCtrl->GetItemText(i,8,sText,50);
				end.z=atof(sText);
				if(m_sStartPartHandle.CompareNoCase(sStartPart)==0&&
					m_sEndPartHandle.CompareNoCase(sEndPart)==0&&
					start.x==m_fStartPointX&&start.y==m_fStartPointY&&start.z==m_fStartPointZ&&
					end.x==m_fEndPointX&&end.y==m_fEndPointY&&end.z==m_fEndPointZ)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("the same checking line has existed, no permission to repeat adding!");
#else
					AfxMessageBox("已存在相同的检测线，不能重复添加!");
#endif
					return;
				}
			}
			//更新定位点列表
			CString ss;
			m_pListCtrl->SetItemText(iCurSel,0,m_sStartPartHandle);
			m_pListCtrl->SetItemText(iCurSel,1,m_sEndPartHandle);
			if(m_bBeforeWeld)
#ifdef AFX_TARG_ENU_ENGLISH
				ss.Format("Before Welding");
			else 
				ss.Format("After Welding");
#else
				ss.Format("焊接前");
			else 
				ss.Format("焊接后");
#endif
			m_pListCtrl->SetItemText(iCurSel,2,ss);
			ss.Format("%f",m_fStartPointX);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,3,ss);
			ss.Format("%f",m_fStartPointY);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,4,ss);
			ss.Format("%f",m_fStartPointZ);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,5,ss);
			ss.Format("%f",m_fEndPointX);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,6,ss);
			ss.Format("%f",m_fEndPointY);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,7,ss);
			ss.Format("%f",m_fEndPointZ);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,8,ss);
		}
	}
}


void CPartGroupCheckLineDlg::OnOK() 
{
	if(m_pWorkView && m_pWorkView->CheckLines.GetNodeNum()>0)
		m_pWorkView->CheckLines.Empty();
	int nCount = m_pListCtrl->GetItemCount();
	for(int i=0;i<nCount;i++)
	{
		char sText[100];
		long hStart=0,hEnd=0;
		bool bSpecStartPart=FALSE,bSpecEndPart=FALSE;
		CCheckLineDimension *pCheckLine=NULL;
		m_pListCtrl->GetItemText(i,0,sText,100);
		sscanf(sText,"%X",&hStart);
		m_pListCtrl->GetItemText(i,1,sText,100);
		sscanf(sText,"%X",&hEnd);
		CLDSPart *pStartPart = console.FromPartHandle(hStart);
		CLDSPart *pEndPart = console.FromPartHandle(hEnd);
		pCheckLine= m_pWorkView->CheckLines.append();
		if(pStartPart)
		{
			pCheckLine->m_bSpecStartLocatePart=TRUE;
			pCheckLine->m_hStartPart = pStartPart->handle;
		}
		else
		{
			pCheckLine->m_bSpecStartLocatePart=FALSE;
			pCheckLine->m_hStartPart=0;
		}
		if(pEndPart)
		{
			pCheckLine->m_bSpecEndLocatePart=TRUE;
			pCheckLine->m_hEndPart=pEndPart->handle;
		}
		else
		{
			pCheckLine->m_bSpecEndLocatePart=FALSE;
			pCheckLine->m_hEndPart=0;
		}
		m_pListCtrl->GetItemText(i,2,sText,50);
#ifdef AFX_TARG_ENU_ENGLISH
		if(strcmp(sText,"Before Welding")==0)
#else
		if(strcmp(sText,"焊接前")==0)
#endif
			pCheckLine->m_bBeforeWeld=TRUE;
		else
			pCheckLine->m_bBeforeWeld=FALSE;
		m_pListCtrl->GetItemText(i,3,sText,100);
		pCheckLine->m_xStartLocalCoordPos.x=atof(sText);
		m_pListCtrl->GetItemText(i,4,sText,100);
		pCheckLine->m_xStartLocalCoordPos.y=atof(sText);
		m_pListCtrl->GetItemText(i,5,sText,100);
		pCheckLine->m_xStartLocalCoordPos.z=atof(sText);
		m_pListCtrl->GetItemText(i,6,sText,100);
		pCheckLine->m_xEndLocalCoordPos.x=atof(sText);
		m_pListCtrl->GetItemText(i,7,sText,100);
		pCheckLine->m_xEndLocalCoordPos.y=atof(sText);
		m_pListCtrl->GetItemText(i,8,sText,100);
		pCheckLine->m_xEndLocalCoordPos.z=atof(sText);
	}
	CDialog::OnOK();
}

void CPartGroupCheckLineDlg::SelectPart(int nCurSel) 
{
	CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	char tem_str[100];
	long hStartPart=0,hEndPart=0;
	m_pListCtrl->GetItemText(nCurSel,0,tem_str,100);
	sscanf(tem_str,"%X",&hStartPart);
	m_pListCtrl->GetItemText(nCurSel,1,tem_str,100);
	sscanf(tem_str,"%X",&hEndPart);
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(hStartPart);
	g_pSolidDraw->SetEntSnapStatus(hEndPart);
	//pLDSView->UpdatePropertyPage();
	CLDSPart *pStartPart=NULL, *pEndPart=NULL;
	pStartPart=console.FromPartHandle(hStartPart);
	pEndPart=console.FromPartHandle(hEndPart);
	f3dPoint startPos, endPos;
	m_pListCtrl->GetItemText(nCurSel,2,tem_str,100);
#ifdef AFX_TARG_ENU_ENGLISH
	if(strcmp(tem_str,"After Welding")==0)
#else
	if(strcmp(tem_str,"焊接后")==0)
#endif
	{	//焊接后检测线
		m_pListCtrl->GetItemText(nCurSel,3,tem_str,100);
		startPos.x=atof(tem_str);
		m_pListCtrl->GetItemText(nCurSel,4,tem_str,100);
		startPos.y=atof(tem_str);
		m_pListCtrl->GetItemText(nCurSel,5,tem_str,100);
		startPos.z=atof(tem_str);
		m_pListCtrl->GetItemText(nCurSel,6,tem_str,100);
		endPos.x=atof(tem_str);
		m_pListCtrl->GetItemText(nCurSel,7,tem_str,100);
		endPos.y=atof(tem_str);
		m_pListCtrl->GetItemText(nCurSel,8,tem_str,100);
		endPos.z=atof(tem_str);
		if(pStartPart)
		{
			if(pStartPart->GetClassTypeId()==CLS_BOLT)
				((CLDSBolt*)pStartPart)->GetUCS();
			else if(pStartPart->GetClassTypeId()==CLS_LINETUBE)
				((CLDSLineTube*)pStartPart)->BuildUCS();
			else if(pStartPart->GetClassTypeId()==CLS_LINESLOT)
				((CLDSLineSlot*)pStartPart)->BuildUCS();
			coord_trans(startPos,pStartPart->ucs,TRUE);
		}
		if(pEndPart)
		{
			if(pEndPart->GetClassTypeId()==CLS_BOLT)
				((CLDSBolt*)pEndPart)->GetUCS();
			else if(pEndPart->GetClassTypeId()==CLS_LINETUBE)
				((CLDSLineTube*)pEndPart)->BuildUCS();
			else if(pEndPart->GetClassTypeId()==CLS_LINESLOT)
				((CLDSLineSlot*)pEndPart)->BuildUCS();
			coord_trans(endPos,pEndPart->ucs,TRUE);
		}
	}
	else
	{	//焊接前检测线
		if(pStartPart==NULL&&pEndPart==NULL)
			return;
		CLDSBolt *pBolt=NULL;
		CLDSPart *pOthPart=NULL;
		if(pStartPart->GetClassTypeId()==CLS_BOLT)
		{
			pBolt=(CLDSBolt*)pStartPart;
			pOthPart=pEndPart;
		}
		else if(pEndPart->GetClassTypeId()==CLS_BOLT)
		{
			pBolt=(CLDSBolt*)pEndPart;
			pOthPart=pStartPart;
		}
		if(pBolt&&(pOthPart->GetClassTypeId()==CLS_PLATE||pOthPart->GetClassTypeId()==CLS_LINETUBE
			||pOthPart->GetClassTypeId()==CLS_PARAMPLATE))
		{
			f3dPoint perp,vec;
			startPos=pBolt->ucs.origin;
			if(pOthPart->GetClassTypeId()==CLS_LINETUBE)
			{
				CLDSLineTube *pLineTube=(CLDSLineTube*)pOthPart;
				SnapPerp(&perp,pLineTube->Start(),pLineTube->End(),startPos);
				vec=startPos-perp;
				normalize(vec);
				endPos=perp+vec*pLineTube->GetDiameter()*0.5;
			}
			else if(pOthPart->GetClassTypeId()==CLS_PLATE)
			{
				CLDSPlate *pPlate=(CLDSPlate*)pOthPart;
				vec=startPos-pPlate->ucs.origin;
				normalize(vec);
				f3dPoint basePt;
				if(vec*pPlate->ucs.axis_z>0)	//螺栓位于板正方向一侧
					basePt=pPlate->ucs.origin+pPlate->ucs.axis_z*(pPlate->GetThick()+pPlate->m_fNormOffset);
				else 
					basePt=pPlate->ucs.origin+pPlate->ucs.axis_z*pPlate->m_fNormOffset;
				endPos=startPos;
				project_point(endPos,basePt,pPlate->ucs.axis_z);
			}
		}
		else
			return;
	}
	f3dLine line;
	line.startPt=startPos;
	line.endPt=endPos;
	line.pen.crColor=RGB(255,0,0);
	line.pen.width=2;
	line.pen.style=PS_SOLID;
	g_pSolidDraw->NewLine(line);
}

void CPartGroupCheckLineDlg::OnClickListCheckLine(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CPartGroupCheckLineDlg::OnDblclkListCheckLine(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CPartGroupCheckLineDlg::OnKeydownListCheckLine(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CPartGroupCheckLineDlg::ModifyItem(int iCurSel)
{
	char sText[50];
	m_pListCtrl->GetItemText(iCurSel,0,sText,50);
	m_sStartPartHandle.Format("%s",sText);
	m_pListCtrl->GetItemText(iCurSel,1,sText,50);
	m_sEndPartHandle.Format("%s",sText);
	m_pListCtrl->GetItemText(iCurSel,2,sText,50);
#ifdef AFX_TARG_ENU_ENGLISH
	if(strcmp(sText,"Before Welding")==0)
#else
	if(strcmp(sText,"焊接前")==0)
#endif
		m_bBeforeWeld=TRUE;
	else 
		m_bBeforeWeld=FALSE;
	//更新界面
	if(m_bBeforeWeld)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sPrompt="==>mark the vertical distance between bolts and plates and tubes";
	else
		m_sPrompt="==>mark the straight-line distance between two parts";
#else
		m_sPrompt="==>标注螺栓到钢板、钢管的垂直距离";
	else
		m_sPrompt="==>标注两构件之间的直线距离";
#endif
	GetDlgItem(IDC_E_START_POINT_X)->EnableWindow(!m_bBeforeWeld);
	GetDlgItem(IDC_E_START_POINT_Y)->EnableWindow(!m_bBeforeWeld);
	GetDlgItem(IDC_E_START_POINT_Z)->EnableWindow(!m_bBeforeWeld);
	GetDlgItem(IDC_E_END_POINT_X)->EnableWindow(!m_bBeforeWeld);
	GetDlgItem(IDC_E_END_POINT_Y)->EnableWindow(!m_bBeforeWeld);
	GetDlgItem(IDC_E_END_POINT_Z)->EnableWindow(!m_bBeforeWeld);
	m_pListCtrl->GetItemText(iCurSel,3,sText,50);
	m_fStartPointX=atof(sText);
	m_pListCtrl->GetItemText(iCurSel,4,sText,50);
	m_fStartPointY=atof(sText);
	m_pListCtrl->GetItemText(iCurSel,5,sText,50);
	m_fStartPointZ=atof(sText);
	m_pListCtrl->GetItemText(iCurSel,6,sText,50);
	m_fEndPointX=atof(sText);
	m_pListCtrl->GetItemText(iCurSel,7,sText,50);
	m_fEndPointY=atof(sText);
	m_pListCtrl->GetItemText(iCurSel,8,sText,50);
	m_fEndPointZ=atof(sText);
	UpdateData(FALSE);
}

void CPartGroupCheckLineDlg::OnCopyStartPoint() 
{
	UpdateData();
	f3dPoint point;
	point.Set(m_fStartPointX,m_fStartPointY,m_fStartPointZ);
	WritePointToClip(point);
}

void CPartGroupCheckLineDlg::OnCopyEndPoint() 
{
	UpdateData();
	f3dPoint point;
	point.Set(m_fEndPointX,m_fEndPointY,m_fEndPointZ);
	WritePointToClip(point);
}

void CPartGroupCheckLineDlg::OnPasteStartPoint() 
{
	f3dPoint point;
	ReadClipPoint(point);
	m_fStartPointX = point.x;
	m_fStartPointY = point.y;
	m_fStartPointZ = point.z;
	UpdateData(FALSE);
}

void CPartGroupCheckLineDlg::OnPasteEndPoint() 
{
	f3dPoint point;
	ReadClipPoint(point);
	m_fEndPointX = point.x;
	m_fEndPointY = point.y;
	m_fEndPointZ = point.z;
	UpdateData(FALSE);	
}

void CPartGroupCheckLineDlg::OnBeforeWeld() 
{
	UpdateData();
	if(m_bBeforeWeld)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sPrompt="==>mark the vertical distance between bolts and plates and tubes";
	else
		m_sPrompt="==>mark the straight-line distance between two parts";
#else
		m_sPrompt="==>标注螺栓到钢板、钢管的垂直距离";
	else
		m_sPrompt="==>标注两构件之间的直线距离";
#endif
	GetDlgItem(IDC_E_START_POINT_X)->EnableWindow(!m_bBeforeWeld);
	GetDlgItem(IDC_E_START_POINT_Y)->EnableWindow(!m_bBeforeWeld);
	GetDlgItem(IDC_E_START_POINT_Z)->EnableWindow(!m_bBeforeWeld);
	GetDlgItem(IDC_E_END_POINT_X)->EnableWindow(!m_bBeforeWeld);
	GetDlgItem(IDC_E_END_POINT_Y)->EnableWindow(!m_bBeforeWeld);
	GetDlgItem(IDC_E_END_POINT_Z)->EnableWindow(!m_bBeforeWeld);
	UpdateData(FALSE);
}
#endif