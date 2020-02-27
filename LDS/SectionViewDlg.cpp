//<LOCALE_TRANSLATE Confirm by hxr/>
// SectionViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "GlobalFunc.h"
#include "SectionViewDlg.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "env_def.h"
#include "PartDrawMarkDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CSectionViewDlg dialog


CSectionViewDlg::CSectionViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSectionViewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSectionViewDlg)
	m_sSectionViewName = _T("");
	m_fAxisX_X = 1.0;
	m_fAxisX_Y = 0.0;
	m_fAxisX_Z = 0.0;
	m_fAxisY_X = 0.0;
	m_fAxisY_Y = 1.0;
	m_fAxisY_Z = 0.0;
	m_fAxisZ_X = 0.0;
	m_fAxisZ_Y = 0.0;
	m_fAxisZ_Z = 1.0;
	m_fOriginPosX = 0.0;
	m_fOriginPosY = 0.0;
	m_fOriginPosZ = 0.0;
	m_bCanModifyAxisX = TRUE;
	m_bCanModifyAxisY = FALSE;
	m_bCanModifyAxisZ = TRUE;
	m_bLockedView = TRUE;	//新建视图时默认为锁定状态
	m_sPartNum = _T("");
	//}}AFX_DATA_INIT
	ucs.axis_x.Set(1.0,0.0,0.0);
	ucs.axis_y.Set(0.0,1.0,0.0);
	ucs.axis_z.Set(0.0,0.0,1.0);
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("part handle");
	m_arrHeaderLabel.Add("part type");
	m_arrHeaderLabel.Add("drawing mark");
#else
	m_arrHeaderLabel.Add("构件句柄");
	m_arrHeaderLabel.Add("构件类型");
	m_arrHeaderLabel.Add("绘图标记");
#endif
	m_nPartNum = 0;
	m_pWorkView = NULL;
	m_pWorkPartGroup = NULL;
	m_pSrcView = NULL;
}


void CSectionViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSectionViewDlg)
	DDX_Text(pDX, IDC_E_SECTIONVIEW_NAME, m_sSectionViewName);
	DDX_Text(pDX, IDC_E_AXIS_X_X, m_fAxisX_X);
	DDX_Text(pDX, IDC_E_AXIS_X_Y, m_fAxisX_Y);
	DDX_Text(pDX, IDC_E_AXIS_X_Z, m_fAxisX_Z);
	DDX_Text(pDX, IDC_E_AXIS_Y_X, m_fAxisY_X);
	DDX_Text(pDX, IDC_E_AXIS_Y_Y, m_fAxisY_Y);
	DDX_Text(pDX, IDC_E_AXIS_Y_Z, m_fAxisY_Z);
	DDX_Text(pDX, IDC_E_AXIS_Z_X, m_fAxisZ_X);
	DDX_Text(pDX, IDC_E_AXIS_Z_Y, m_fAxisZ_Y);
	DDX_Text(pDX, IDC_E_AXIS_Z_Z, m_fAxisZ_Z);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_X, m_fOriginPosX);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_Y, m_fOriginPosY);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_Z, m_fOriginPosZ);
	DDX_Check(pDX, IDC_CHK_ENABLE_AXIS_X, m_bCanModifyAxisX);
	DDX_Check(pDX, IDC_CHK_ENABLE_AXIS_Y, m_bCanModifyAxisY);
	DDX_Check(pDX, IDC_CHK_ENABLE_AXIS_Z, m_bCanModifyAxisZ);
	DDX_Check(pDX, IDC_CHK_LOCKED_VIEW, m_bLockedView);
	DDX_Text(pDX, IDC_S_PART_NUM, m_sPartNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSectionViewDlg, CDialog)
	//{{AFX_MSG_MAP(CSectionViewDlg)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_X, OnBnPasteAxisX)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_Y, OnBnPasteAxisY)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_Z, OnBnPasteAxisZ)
	ON_BN_CLICKED(IDC_BN_PASTE_ORIGIN_POS, OnBnPasteOriginPos)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_X, OnBnCopyAxisX)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_Y, OnBnCopyAxisY)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_Z, OnBnCopyAxisZ)
	ON_BN_CLICKED(IDC_BN_COPY_ORIGIN_POS, OnBnCopyOriginPos)
	ON_BN_CLICKED(IDC_BN_CPY_UCS, OnBnCpyUcs)
	ON_BN_CLICKED(IDC_BN_PASTE_UCS, OnBnPasteUcs)
	ON_BN_CLICKED(IDC_BN_TO_RIGHT_HAND, OnBnToRightHand)
	ON_BN_CLICKED(IDC_CHK_ENABLE_AXIS_X, OnChkEnableAxisX)
	ON_BN_CLICKED(IDC_CHK_ENABLE_AXIS_Y, OnChkEnableAxisY)
	ON_BN_CLICKED(IDC_CHK_ENABLE_AXIS_Z, OnChkEnableAxisZ)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PART, OnClickListPart)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PART, OnDblclkListPart)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_PART, OnKeydownListPart)
	ON_BN_CLICKED(IDC_BN_DRAW_BY_SEL_PART_UCS, OnBnDrawBySelPartUcs)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSectionViewDlg message handlers

BOOL CSectionViewDlg::OnInitDialog() 
{
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_PART);
	if(m_pListCtrl==NULL)
		return FALSE;
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	m_nPartNum = m_pListCtrl->GetItemCount();
	if(m_pWorkView||m_pSrcView)
	{
		CSectionView *pCurView=m_pWorkView;
		if(pCurView==NULL)
			pCurView=m_pSrcView;
		m_bLockedView=pCurView->m_bLocked;	//视图是否锁定
		for(CSectionView::PART_ITEM *pItem=pCurView->SonParts.GetFirst();pItem;pItem=pCurView->SonParts.GetNext())
		{
			CLDSPart *pPart = console.FromPartHandle(pItem->hSonPart);
			CString tempStr;
			CStringArray str_arr;
			if(pPart==NULL)
				continue;
			tempStr.Format("0X%X",pItem->hSonPart);
			str_arr.Add(tempStr);
			if(pPart->GetClassTypeId()==CLS_LINEANGLE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr.Add("Angle");
				if(pItem->cFlag=='X')
					str_arr.Add("X Leg");
				else if(pItem->cFlag=='Y')
					str_arr.Add("Y Leg");
				else if(pItem->cFlag=='Z')
					str_arr.Add("Z Section");
				else
					str_arr.Add("Judge Auto");
#else
				str_arr.Add("角钢");
				if(pItem->cFlag=='X')
					str_arr.Add("X肢");
				else if(pItem->cFlag=='Y')
					str_arr.Add("Y肢");
				else if(pItem->cFlag=='Z')
					str_arr.Add("Z截面");
				else
					str_arr.Add("自动判断");
#endif
			}
			else if(pPart->GetClassTypeId()==CLS_LINETUBE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr.Add("Tube");
				if(pItem->cFlag=='X')
					str_arr.Add("X Front View");
				else if(pItem->cFlag=='Z')
					str_arr.Add("Z Section");
				else
					str_arr.Add("Judge Auto");
#else
				str_arr.Add("钢管");
				if(pItem->cFlag=='X')
					str_arr.Add("X正视");
				else if(pItem->cFlag=='Z')
					str_arr.Add("Z截面");
				else
					str_arr.Add("自动判断");
#endif
			}
			else if(pPart->GetClassTypeId()==CLS_PLATE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr.Add("Plate");
				if(pItem->cFlag=='X')
					str_arr.Add("X Section");
				else if(pItem->cFlag=='Z')
					str_arr.Add("Z Front View");
				else
					str_arr.Add("Judge Auto");
#else
				str_arr.Add("钢板");
				if(pItem->cFlag=='X')
					str_arr.Add("X截面");
				else if(pItem->cFlag=='Z')
					str_arr.Add("Z正视");
				else
					str_arr.Add("自动判断");
#endif
			}
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr.Add("Para Plate");
				if(pItem->cFlag=='X')
					str_arr.Add("X Section");
				else if(pItem->cFlag=='Z')
					str_arr.Add("Z Front View");
				else
					str_arr.Add("Judge Auto");
#else
				str_arr.Add("参数板");
				if(pItem->cFlag=='X')
					str_arr.Add("X截面");
				else if(pItem->cFlag=='Z')
					str_arr.Add("Z正视");
				else
					str_arr.Add("自动判断");
#endif
			}
			else 
			{
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr.Add("Other Parts");
				str_arr.Add("Judge Auto");
#else
				str_arr.Add("其他构件");
				str_arr.Add("自动判断");
#endif
			}
			int iCur=AppendRecord(-1,str_arr);
			m_pListCtrl->SetItemData(iCur,(DWORD)pPart);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		m_sPartNum.Format("%d items total",m_pListCtrl->GetItemCount());
#else
		m_sPartNum.Format("共%d项",m_pListCtrl->GetItemCount());
#endif
	}

	CDialog::OnInitDialog();
	m_bCanModifyAxisX=FALSE;
	m_bCanModifyAxisY=!m_bCanModifyAxisX;
	m_bCanModifyAxisZ=TRUE;
	CDialog::OnInitDialog();
	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(m_bCanModifyAxisX);

	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(m_bCanModifyAxisY);
	UpdateUcs(FALSE);
	return TRUE;
}

void CSectionViewDlg::OnOK() 
{
	UpdateData();
	BOOL bRepeatedName = FALSE;
	//用构件列表中的构件更新视图构件集合
	if(m_pWorkView)
	{	//编辑当前视图
		sprintf(m_pWorkView->sName,"%s",m_sSectionViewName);
		m_pWorkView->ucs = ucs;
		m_pWorkView->m_bLocked = m_bLockedView;
		if(m_pWorkView->SonParts.GetNodeNum()>0)
			m_pWorkView->SonParts.Empty();
		m_nPartNum=m_pListCtrl->GetItemCount();
		for(int i=0;i<m_nPartNum;i++)
		{
			char sText[100];
			CSectionView::PART_ITEM *pPartItem  = NULL;
			m_pListCtrl->GetItemText(i,0,sText,100);
			long h=0;
			sscanf(sText,"%X",&h);
			CLDSPart *pPart = console.FromPartHandle(h);
			if(pPart==NULL)
				continue;
			pPartItem = m_pWorkView->SonParts.Add(pPart->handle);
			m_pListCtrl->GetItemText(i,2,sText,100);
			if(sText[0]>='A'&&sText[0]<='Z')
				pPartItem->cFlag=sText[0];
			else
				pPartItem->cFlag=0;
		}
	}
	CSectionView *pNewView=NULL;
	if(m_pSrcView&&m_pWorkPartGroup)
	{	//克隆并新建视图 wht 10-08-25
		//pNewView = m_pWorkPartGroup->SonViews.append();
		pNewView = m_pWorkPartGroup->SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
		sprintf(pNewView->sName,"%s",m_sSectionViewName);
		pNewView->m_bLocked=m_bLockedView;
		pNewView->ucs = ucs;
		m_nPartNum=m_pListCtrl->GetItemCount();
		for(int i=0;i<m_nPartNum;i++)
		{
			long h=0;
			char sText[100];
			CSectionView::PART_ITEM *pPartItem = NULL;
			m_pListCtrl->GetItemText(i,0,sText,100);
			sscanf(sText,"%X",&h);
			CLDSPart *pPart = console.FromPartHandle(h);
			if(pPart==NULL)
				continue;
			pPartItem = pNewView->SonParts.Add(pPart->handle);
			pPartItem->cFlag = 0;
		}
		//主视图克隆后变为前视图
		if(pNewView->m_iViewType==VIEW_MAIN)
			pNewView->m_iViewType=VIEW_FRONT;
	}
	if(m_pWorkPartGroup && m_pWorkPartGroup->SonViews.GetNodeNum()>0)
	{
		for(CSectionView *pView=m_pWorkPartGroup->SonViews.GetFirst();pView;pView=m_pWorkPartGroup->SonViews.GetNext())
		{
			if(m_pSrcView&&pView==pNewView)
				continue;	//跳过新建视图
			if(m_sSectionViewName.CompareNoCase(pView->sName)==0)
			{
				bRepeatedName = TRUE;
				break;
			}
		}
	}
	UpdateUcs();
	if(ucs.axis_x.IsZero()||ucs.axis_y.IsZero()||ucs.axis_z.IsZero())
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("coordinate system is illegal (the direction of axis can't be zero)");
	else if(!is_ucs_valid(ucs))
		AfxMessageBox("coordinate system is illegal(Not right hand spiral orthogonal coordinate system )");
	else if(bRepeatedName)
		AfxMessageBox("view name is illegal(the same view name has existed)");
#else
		AfxMessageBox("坐标系不合法(坐标轴方向不能为零)");
	else if(!is_ucs_valid(ucs))
		AfxMessageBox("坐标系不合法(不是右手螺旋正交的坐标系)");
	else if(bRepeatedName)
		AfxMessageBox("视图名不合法(已存在相同的视图名)");
#endif
	else
		CDialog::OnOK();
}

void CSectionViewDlg::UpdateUcs(BOOL bFlag)
{
	if(bFlag)	//交互界面向ucs更新
	{
		UpdateData(TRUE);
		ucs.axis_x.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
		ucs.axis_y.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
		ucs.axis_z.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
		ucs.origin.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	}
	else	//ucs向交互界面更新
	{
		m_fAxisX_X = ucs.axis_x.x;
		m_fAxisX_Y = ucs.axis_x.y;
		m_fAxisX_Z = ucs.axis_x.z;

		m_fAxisY_X = ucs.axis_y.x;
		m_fAxisY_Y = ucs.axis_y.y;
		m_fAxisY_Z = ucs.axis_y.z;

		m_fAxisZ_X = ucs.axis_z.x;
		m_fAxisZ_Y = ucs.axis_z.y;
		m_fAxisZ_Z = ucs.axis_z.z;

		m_fOriginPosX = ucs.origin.x;
		m_fOriginPosY = ucs.origin.y;
		m_fOriginPosZ = ucs.origin.z;
		UpdateData(FALSE);
	}
}

void CSectionViewDlg::OnBnPasteAxisX() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisX_X = point.x;
		m_fAxisX_Y = point.y;
		m_fAxisX_Z = point.z;
		ucs.axis_x.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
		UpdateData(FALSE);
	}
}

void CSectionViewDlg::OnBnPasteAxisY() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisY_X = point.x;
		m_fAxisY_Y = point.y;
		m_fAxisY_Z = point.z;
		ucs.axis_y.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
		UpdateData(FALSE);
	}
}

void CSectionViewDlg::OnBnPasteAxisZ() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisZ_X = point.x;
		m_fAxisZ_Y = point.y;
		m_fAxisZ_Z = point.z;
		ucs.axis_z.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
		UpdateData(FALSE);
	}
}

void CSectionViewDlg::OnBnPasteOriginPos() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fOriginPosX = point.x;
		m_fOriginPosY = point.y;
		m_fOriginPosZ = point.z;
		ucs.origin.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
		UpdateData(FALSE);
	}
}

void CSectionViewDlg::OnBnCopyAxisX() 
{
	f3dPoint point;
	point.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
	WritePointToClip(point);
}

void CSectionViewDlg::OnBnCopyAxisY() 
{
	f3dPoint point;
	point.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
	WritePointToClip(point);
}

void CSectionViewDlg::OnBnCopyAxisZ() 
{
	f3dPoint point;
	point.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
	WritePointToClip(point);
}

void CSectionViewDlg::OnBnCopyOriginPos() 
{
	f3dPoint point;
	point.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	WritePointToClip(point);
}

void CSectionViewDlg::OnBnCpyUcs() 
{
	UCS_STRU ucs;
	UpdateData();
	ucs.origin.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	ucs.axis_x.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
	ucs.axis_y.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
	ucs.axis_z.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
	WriteUCSToClip(ucs);
}

void CSectionViewDlg::OnBnPasteUcs() 
{
	UCS_STRU;
	if(ReadClipUCS(ucs))
	{
		m_fAxisX_X = ucs.axis_x.x;
		m_fAxisX_Y = ucs.axis_x.y;
		m_fAxisX_Z = ucs.axis_x.z;

		m_fAxisY_X = ucs.axis_y.x;
		m_fAxisY_Y = ucs.axis_y.y;
		m_fAxisY_Z = ucs.axis_y.z;

		m_fAxisZ_X = ucs.axis_z.x;
		m_fAxisZ_Y = ucs.axis_z.y;
		m_fAxisZ_Z = ucs.axis_z.z;

		m_fOriginPosX = ucs.origin.x;
		m_fOriginPosY = ucs.origin.y;
		m_fOriginPosZ = ucs.origin.z;
		UpdateData(FALSE);
	}
}

void CSectionViewDlg::OnBnToRightHand() 
{
	UpdateUcs();
	if(!m_bCanModifyAxisZ)
	{	//以X-Y轴为基准	 X轴方向与用户输入方向保持一致，由XY计算出Z轴方向然后重新计算Y轴方向
		ucs.axis_z = ucs.axis_x^ucs.axis_y;
		normalize(ucs.axis_z);
		normalize(ucs.axis_x);
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
	}
	else if(!m_bCanModifyAxisY)
	{	//以Z-X轴为基准	 Z轴方向与用户输入方向保持一致
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
		normalize(ucs.axis_y);
		normalize(ucs.axis_x);
		ucs.axis_x = ucs.axis_y^ucs.axis_z;
	}
	else if(!m_bCanModifyAxisX)
	{	//以Y-Z轴为基准	Y轴方向与用户输入方向保持一致
		ucs.axis_x = ucs.axis_y^ucs.axis_z;
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		ucs.axis_z = ucs.axis_x^ucs.axis_y;
	}
	UpdateUcs(FALSE);
}

void CSectionViewDlg::OnChkEnableAxisX() 
{
	UpdateData();
	if(!m_bCanModifyAxisX)
		m_bCanModifyAxisY=m_bCanModifyAxisZ=TRUE;
	else
	{
		m_bCanModifyAxisY = TRUE;
		m_bCanModifyAxisZ = FALSE;
	}
	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_BN_PASTE_AXIS_Z)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_X)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Y)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Z)->EnableWindow(m_bCanModifyAxisZ);
	UpdateData(FALSE);
	Invalidate(FALSE);	//重绘 wht 10-08-24
}

void CSectionViewDlg::OnChkEnableAxisY() 
{
	UpdateData();
	if(!m_bCanModifyAxisY)
		m_bCanModifyAxisX=m_bCanModifyAxisZ=TRUE;
	else
	{
		m_bCanModifyAxisX = TRUE;
		m_bCanModifyAxisZ = FALSE;
	}
	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_BN_PASTE_AXIS_Z)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_X)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Y)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Z)->EnableWindow(m_bCanModifyAxisZ);
	UpdateData(FALSE);
	Invalidate(FALSE);	//重绘 wht 10-08-24
}

void CSectionViewDlg::OnChkEnableAxisZ() 
{
	UpdateData();
	if(!m_bCanModifyAxisZ)
		m_bCanModifyAxisX=m_bCanModifyAxisY=TRUE;
	else
	{
		m_bCanModifyAxisX = TRUE;
		m_bCanModifyAxisY = FALSE;
	}
	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_BN_PASTE_AXIS_Z)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_X)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Y)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Z)->EnableWindow(m_bCanModifyAxisZ);
	UpdateData(FALSE);
	Invalidate(FALSE);	//重绘 wht 10-08-24
}

void CSectionViewDlg::RefreshListViewHeader()
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
		if(i==2)
			lvc.cx = 120;
		else
			lvc.cx = 80;
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}
int CSectionViewDlg::AppendRecord(int iItem, CStringArray &str_arr)
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

void CSectionViewDlg::ModifyItem(int iCurSel)
{
	CPartDrawMarkDlg markDlg;
	char sText[100];
	CLDSPart *pPart=(CLDSPart*)m_pListCtrl->GetItemData(iCurSel);
	SelectPart(iCurSel);
	m_pListCtrl->GetItemText(iCurSel,2,sText,100);
	if(pPart->GetClassTypeId()==CLS_LINEANGLE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		markDlg.m_cmbItemStrList="X Leg|Y Leg|Z Section|Judge Auto";
#else
		markDlg.m_cmbItemStrList="X肢|Y肢|Z截面|自动判断";
#endif
		if(sText[0]=='X')
			markDlg.m_iDrawMark=0;
		else if(sText[0]=='Y')
			markDlg.m_iDrawMark=1;
		else if(sText[0]=='Z')
			markDlg.m_iDrawMark=2;
		else
			markDlg.m_iDrawMark=3;
	}
	else if(pPart->GetClassTypeId()==CLS_LINETUBE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		markDlg.m_cmbItemStrList="X Front View|Z Section|Judge Auto";
#else
		markDlg.m_cmbItemStrList="X正视|Z截面|自动判断";
#endif
		if(sText[0]=='X')
			markDlg.m_iDrawMark=0;
		else if(sText[0]=='Z')
			markDlg.m_iDrawMark=1;
		else
			markDlg.m_iDrawMark=2;
	}
	else if(pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		markDlg.m_cmbItemStrList="X Section|Z Front View|Judge Auto";
#else
		markDlg.m_cmbItemStrList="X截面|Z正视|自动判断";
#endif
		if(sText[0]=='X')
			markDlg.m_iDrawMark=0;
		else if(sText[0]=='Z')
			markDlg.m_iDrawMark=1;
		else
			markDlg.m_iDrawMark=2;
	}

	if(markDlg.DoModal()==IDOK)
	{
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			markDlg.m_cmbItemStrList="X Leg|Y Leg|Z Section|Judge Auto";
			if(markDlg.m_iDrawMark==0)
				m_pListCtrl->SetItemText(iCurSel,2,"X Leg");
			else if(markDlg.m_iDrawMark==1)
				m_pListCtrl->SetItemText(iCurSel,2,"Y Leg");
			else if(markDlg.m_iDrawMark==2)
				m_pListCtrl->SetItemText(iCurSel,2,"Z Leg");
			else
				m_pListCtrl->SetItemText(iCurSel,2,"Judge Auto");
#else
			markDlg.m_cmbItemStrList="X肢|Y肢|Z截面|自动判断";
			if(markDlg.m_iDrawMark==0)
				m_pListCtrl->SetItemText(iCurSel,2,"X肢");
			else if(markDlg.m_iDrawMark==1)
				m_pListCtrl->SetItemText(iCurSel,2,"Y肢");
			else if(markDlg.m_iDrawMark==2)
				m_pListCtrl->SetItemText(iCurSel,2,"Z肢");
			else
				m_pListCtrl->SetItemText(iCurSel,2,"自动判断");
#endif
		}
		else if(pPart->GetClassTypeId()==CLS_LINETUBE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			markDlg.m_cmbItemStrList="X Front View|Z Section|Judge Auto";
			if(markDlg.m_iDrawMark==0)
				m_pListCtrl->SetItemText(iCurSel,2,"X Front View");
			else if(markDlg.m_iDrawMark==1)
				m_pListCtrl->SetItemText(iCurSel,2,"Z Section");
			else
				m_pListCtrl->SetItemText(iCurSel,2,"Judge Auto");
#else
			markDlg.m_cmbItemStrList="X正视|Z截面|自动判断";
			if(markDlg.m_iDrawMark==0)
				m_pListCtrl->SetItemText(iCurSel,2,"X正视");
			else if(markDlg.m_iDrawMark==1)
				m_pListCtrl->SetItemText(iCurSel,2,"Z截面");
			else
				m_pListCtrl->SetItemText(iCurSel,2,"自动判断");
#endif
		}
		else if(pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			markDlg.m_cmbItemStrList="X Section|Z Front View|Judge Auto";
			if(markDlg.m_iDrawMark==0)
				m_pListCtrl->SetItemText(iCurSel,2,"X Section");
			else if(markDlg.m_iDrawMark==1)
				m_pListCtrl->SetItemText(iCurSel,2,"Z Front View");
			else
				m_pListCtrl->SetItemText(iCurSel,2,"Judge Auto");
#else
			markDlg.m_cmbItemStrList="X截面|Z正视|自动判断";
			if(markDlg.m_iDrawMark==0)
				m_pListCtrl->SetItemText(iCurSel,2,"X截面");
			else if(markDlg.m_iDrawMark==1)
				m_pListCtrl->SetItemText(iCurSel,2,"Z正视");
			else
				m_pListCtrl->SetItemText(iCurSel,2,"自动判断");
#endif
		}
	}
}

void CSectionViewDlg::SelectPart(int nCurSel) 
{
	CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	char tem_str[100];
	m_pListCtrl->GetItemText(nCurSel,0,tem_str,100);
	long hCurPart=0;
	sscanf(tem_str,"%X",&hCurPart);
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(hCurPart);
	pLDSView->UpdatePropertyPage();
}

void CSectionViewDlg::OnClickListPart(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CSectionViewDlg::OnDblclkListPart(NMHDR* pNMHDR, LRESULT* pResult) 
{	//组焊图附加视图绘制时暂不支持指定构件绘制面 wht 11-07-18
	/*POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(iCurSel>=0)
			ModifyItem(iCurSel);
	}*/
	*pResult = 0;
}

void CSectionViewDlg::OnKeydownListPart(NMHDR* pNMHDR, LRESULT* pResult) 
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
#ifdef AFX_TARG_ENU_ENGLISH
			m_sPartNum.Format("%d items total",m_pListCtrl->GetItemCount());
#else
			m_sPartNum.Format("共%d项",m_pListCtrl->GetItemCount());
#endif
			UpdateData(FALSE);
		}
	}
	*pResult = 0;
}

void CSectionViewDlg::OnBnDrawBySelPartUcs() 
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos==NULL)
		return;
	char tem_str[100]="", sText[100]="";
	int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
	m_pListCtrl->GetItemText(iCurSel,0,tem_str,100);
	m_pListCtrl->GetItemText(iCurSel,2,sText,100);
	long hPart=0;
	sscanf(tem_str,"%X",&hPart);
	CLDSPart *pPart=console.FromPartHandle(hPart);
	if(pPart)
	{
		ucs.origin=pPart->ucs.origin;
		if(sText[0]=='X')
		{
			ucs.axis_z=pPart->ucs.axis_x;
			ucs.axis_x=pPart->ucs.axis_z;
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			normalize(ucs.axis_y);
			ucs.axis_x=ucs.axis_y^ucs.axis_z;
			normalize(ucs.axis_x);
		}
		else if(sText[0]=='Y')
		{
			ucs.axis_z=pPart->ucs.axis_y;
			ucs.axis_y=pPart->ucs.axis_z;
			ucs.axis_x=ucs.axis_y^ucs.axis_z;
			normalize(ucs.axis_x);
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			normalize(ucs.axis_y);
		}
		else if(sText[0]=='Z')
			ucs=pPart->ucs;
		UpdateUcs(FALSE);
		//else	//自动判断
		UpdateData(FALSE);
	}
}

HBRUSH CSectionViewDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if(pWnd->GetDlgCtrlID()==IDC_S_RED)
		pDC->SetTextColor(RGB(255,0,0));
	else if(pWnd->GetDlgCtrlID()==IDC_S_BLUE)
		pDC->SetTextColor(RGB(0,130,0));
	else if(!m_bCanModifyAxisZ)
	{	//以X-Y轴为基准	 X轴方向与用户输入方向保持一致，由XY计算出Z轴方向然后重新计算Y轴方向
		if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_X)
			pDC->SetTextColor(RGB(255,0,0));	//不可变
		else if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_Y)
			pDC->SetTextColor(RGB(0,130,0));	//可变
	}
	else if(!m_bCanModifyAxisY)
	{	//以Z-X轴为基准	 Z轴方向与用户输入方向保持一致
		if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_Z)
			pDC->SetTextColor(RGB(255,0,0));
		else if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_X)
			pDC->SetTextColor(RGB(0,130,0));
	}
	else if(!m_bCanModifyAxisX)
	{	//以Y-Z轴为基准	Y轴方向与用户输入方向保持一致
		if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_Y)
			pDC->SetTextColor(RGB(255,0,0));
		else if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_Z)
			pDC->SetTextColor(RGB(0,130,0));
	}
	return hbr;
}

#endif