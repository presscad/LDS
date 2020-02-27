// SubstationBeamTemplate.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "SubstationSegmentPage.h"
#include "afxdialogex.h"
#include "supergridctrl.h"

static CString MakeLayerStrArr()
{
	CString layerStr;
	layerStr="/,";
	layerStr+="\\,";
	layerStr+="X,";
	layerStr+="|/,";
	layerStr+="|\\,";
	layerStr+="|X,";
	layerStr+="\\|,";
	layerStr+="/|,";
	layerStr+="X|,";
	layerStr+="|\\|,";
	layerStr+="|/|,";
	layerStr+="|X|";
	return layerStr;
}
//回调函数处理
static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,
	int iSubItem,CString& sTextValue)
{
	CSubstationBeamSegmentPage *pBeamDlg=(CSubstationBeamSegmentPage*)pListCtrl->GetParent();
	if(pBeamDlg==NULL)
		return FALSE;
	CSuperGridCtrl::CTreeItem* pItem=NULL;
	pListCtrl->SetSubItemText(pSelItem,iSubItem,sTextValue);
	int nIndex=pListCtrl->GetSelectedItem();
	int nColCount=pListCtrl->GetHeaderCtrl()->GetItemCount();
	if(nIndex==0 && nColCount==iSubItem+1 && sTextValue.GetLength()>0)
	{	//末尾列填写档距，自动在右侧追加新列
		pSelItem->m_lpNodeInfo->AddSubItemText("");
		pItem=pListCtrl->FindTreeItemById(CSubstationBeamSegmentPage::BOTTOM_PANEL,NULL);
		pItem->m_lpNodeInfo->AddSubItemText("");
		pItem->m_lpNodeInfo->SetControlType(nColCount,GCT_CMB_EDIT);
		pItem->m_lpNodeInfo->SetListItemsStr(nColCount,MakeLayerStrArr());
		pItem=pListCtrl->FindTreeItemById(CSubstationBeamSegmentPage::FRONT_PANEL,NULL);
		pItem->m_lpNodeInfo->AddSubItemText("");
		pItem->m_lpNodeInfo->SetControlType(nColCount,GCT_CMB_EDIT);
		pItem->m_lpNodeInfo->SetListItemsStr(nColCount,MakeLayerStrArr());
		LV_COLUMN lvc;
		lvc.iSubItem = nColCount;
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvc.fmt = LVCFMT_LEFT;
		lvc.cx=40;
		lvc.pszText=CXhChar16("%d",nColCount);
		pListCtrl->InsertColumn(nColCount,&lvc);
		pListCtrl->RedrawItems(0,1);
	}
	if(nIndex==0&&iSubItem>1)
	{	//修改空记录时，布局形式自动按照前一个进行赋值
		CString sLayValue;
		pItem=pListCtrl->FindItemByPropId(CSubstationBeamSegmentPage::BOTTOM_PANEL,NULL);
		sLayValue=pItem->m_lpNodeInfo->GetSubItemText(iSubItem);
		if(sLayValue.GetLength()<=0)
		{
			sLayValue=pItem->m_lpNodeInfo->GetSubItemText(iSubItem-1);
			pListCtrl->SetSubItemText(pItem,iSubItem,sLayValue);
		}
		pItem=pListCtrl->FindTreeItemById(CSubstationBeamSegmentPage::FRONT_PANEL,NULL);
		sLayValue=pItem->m_lpNodeInfo->GetSubItemText(iSubItem);
		if(sLayValue.GetLength()<=0)
		{
			sLayValue=pItem->m_lpNodeInfo->GetSubItemText(iSubItem-1);
			pListCtrl->SetSubItemText(pItem,iSubItem,sLayValue);
		}
	}
	if(pListCtrl->GetSafeHwnd()==pBeamDlg->m_listCtrlLCBeamSeg.GetSafeHwnd())
		pBeamDlg->RefreshRCBeamListCtrl();
	if(pListCtrl->GetSafeHwnd()==pBeamDlg->m_listCtrlLSBeamSeg.GetSafeHwnd())
		pBeamDlg->RefreshRSBeamListCtrl();
	pBeamDlg->RefreshBeamLenEdit();
	return TRUE;
}
static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CSubstationBeamSegmentPage *pBeamDlg=(CSubstationBeamSegmentPage*)pListCtrl->GetParent();
	if(pBeamDlg==NULL)
		return FALSE;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"插入节间");
	pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除节间");
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pBeamDlg);
	return TRUE;
}
static void DrawLine(CDC* pDC,f3dPoint startPt,f3dPoint endPt,UCS_STRU rot_ucs,double scale)
{
	coord_trans(startPt,rot_ucs,FALSE);
	coord_trans(endPt,rot_ucs,FALSE);
	startPt *= scale;
	endPt   *= scale;
	startPt.y+=1;
	endPt.y+=1;
	pDC->MoveTo(ftoi(startPt.x),ftoi(startPt.y));
	pDC->LineTo(ftoi(endPt.x),ftoi(endPt.y));
}
static void DrawBeamSegLines(CDC* pDC,UCS_STRU rot_ucs,CString sLayout,double scale,f3dPoint startFPt,f3dPoint endFPt,f3dPoint startBPt,f3dPoint endBPt,BYTE cbStrokeFlag=0xff)
{
	const BYTE cbShuFlag=0x01;
	const BYTE cbPieFlag=0x02;
	const BYTE cbNaFlag =0x04;

	int nSeg=0;
	for(int i=0;i<sLayout.GetLength();i++)
	{
		if((sLayout[i]=='\\')||(sLayout[i]=='/')||(toupper(sLayout[i])=='X'))
			nSeg++;
	}
	nSeg=max(nSeg,1);
	double iSeg=1;
	f3dPoint preFPt=startFPt,preBPt=startBPt,curFPt,curBPt;
	for(int i=0;i<sLayout.GetLength();i++)
	{
		if((sLayout[i]=='\\')||(sLayout[i]=='/')||(toupper(sLayout[i])=='X'))
		{
			double fScale=iSeg/nSeg;
			curFPt=startFPt+(endFPt-startFPt)*fScale;
			curBPt=startBPt+(endBPt-startBPt)*fScale;
			iSeg++;
		}
		else
		{
			curFPt=preFPt;
			curBPt=preBPt;
		}
		if(cbStrokeFlag&cbPieFlag && (sLayout[i]=='/'|| toupper(sLayout[i])=='X'))
			DrawLine(pDC,preFPt,curBPt,rot_ucs,scale);
		if(cbStrokeFlag&cbNaFlag && (sLayout[i]=='\\'|| toupper(sLayout[i])=='X'))
			DrawLine(pDC,preBPt,curFPt,rot_ucs,scale);
		if(cbStrokeFlag&cbShuFlag && sLayout[i]=='|')
			DrawLine(pDC,curBPt,curFPt,rot_ucs,scale);
		preFPt=curFPt;
		preBPt=curBPt;
	}
}
//////////////////////////////////////////////////////////////////////////
//CSubstationBeamSegmentPage::SECTION
const BYTE CSubstationBeamSegmentPage::SECT_LENGTH	=1;
const BYTE CSubstationBeamSegmentPage::FRONT_PANEL	=2;
const BYTE CSubstationBeamSegmentPage::BOTTOM_PANEL	=3;
CSubstationBeamSegmentPage::SECTION::SECTION(double _space,const char* layout_str)
{
	space=max(1,_space);
	if(layout_str==NULL||*layout_str==0)
		strncpy(sLayout,"|\\",16);
	else
		strncpy(sLayout,layout_str,16);	//布材型式,最后一个字节不为零时,依然有效,故只能用strncpy, 不能用strcpy,strncpy_s
}
static char _INTERNAL_STR_BUF[17]={0};
const char* CSubstationBeamSegmentPage::SECTION::LayoutStr()
{
	if(sLayout[15]==0)
		return sLayout;
	memcpy(_INTERNAL_STR_BUF,sLayout,16);
	return _INTERNAL_STR_BUF;
}

CSubstationBeamSegmentPage::~CSubstationBeamSegmentPage()
{
}
//////////////////////////////////////////////////////////////////////////
// CSubstationBeamTemplateDlg 对话框

IMPLEMENT_DYNAMIC(CSubstationBeamSegmentPage, CDialogEx)

CSubstationBeamSegmentPage::CSubstationBeamSegmentPage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSubstationBeamSegmentPage::IDD, pParent)
{
	m_listCtrlLCBeamSeg.AddColumnHeader("节间数据",70);
	m_listCtrlRCBeamSeg.AddColumnHeader("..",2);
	m_listCtrlLSBeamSeg.AddColumnHeader("节间数据",70);
	m_listCtrlRSBeamSeg.AddColumnHeader("..",2);
	//
	m_nCBeamNum=5;
	m_nSBeamNum=5;
	m_iActiveListCtrl=0;
	m_fSpanLen=24000;
	m_fWidth1=1000;
	m_fWidth2=1600;
	m_fWidth3=1000;
	m_fLeftBeamLength=m_fRightBeamLength=4500;
	m_fBeamHeight=1800;
	m_iPoleType=1;
	//TODO:为什么固死m_fDlWidth=100?
	m_fSBeamLen=m_fSpanLen-m_fLeftBeamLength-m_fRightBeamLength-2*100;
	m_fLSBeamLen=0;
}
void CSubstationBeamSegmentPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_L_CBEAM_SEG, m_listCtrlLCBeamSeg);
	DDX_Control(pDX, IDC_LIST_R_CBEAM_SEG, m_listCtrlRCBeamSeg);
	DDX_Control(pDX, IDC_LIST_L_SBEAM_SEG, m_listCtrlLSBeamSeg);
	DDX_Control(pDX, IDC_LIST_R_SBEAM_SEG, m_listCtrlRSBeamSeg);
	DDX_Text(pDX, IDC_E_WIDTH1, m_fWidth1);
	DDX_Text(pDX, IDC_E_WIDTH2,m_fWidth2);
	DDX_Text(pDX, IDC_E_LEFT_BEAM_LEN,m_fLeftBeamLength);
	DDX_Text(pDX, IDC_E_RIGHT_BEAM_LEN, m_fRightBeamLength);
	DDX_Text(pDX, IDC_E_TOPBEAM_H, m_fBeamHeight);
	DDX_CBIndex(pDX, IDC_CMB_POLE_TYPE, m_iPoleType);
	DDX_Text(pDX, IDC_E_WIDTH3, m_fWidth3);
	DDX_Text(pDX, IDC_E_SBEAM, m_fSBeamLen);
	DDX_Text(pDX, IDC_E_SUBSTATION_LEN, m_fSpanLen);
}


BEGIN_MESSAGE_MAP(CSubstationBeamSegmentPage, CDialogEx)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_COMMAND(ID_NEW_ITEM, OnNewItem)
	ON_COMMAND(ID_DELETE_ITEM, OnDeleteItem)
	ON_EN_CHANGE(IDC_E_WIDTH1, &CSubstationBeamSegmentPage::OnEnChangeEWidth1)
	ON_EN_CHANGE(IDC_E_WIDTH2, &CSubstationBeamSegmentPage::OnEnChangeEWidth2)
	ON_EN_CHANGE(IDC_E_TOPBEAM_H, &CSubstationBeamSegmentPage::OnEnChangeETopbeamH)
	ON_EN_CHANGE(IDC_E_WIDTH3, &CSubstationBeamSegmentPage::OnEnChangeEWidth3)
	ON_CBN_SELCHANGE(IDC_CMB_POLE_TYPE, &CSubstationBeamSegmentPage::OnEnChangeOtherParam)
END_MESSAGE_MAP()


// CSubstationBeamTemplateDlg 消息处理程序


BOOL CSubstationBeamSegmentPage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CListCtrlItemInfo* lpInfo = NULL;
	m_listCtrlLCBeamSeg.SetListItemSplitChar(',');
	m_listCtrlRCBeamSeg.SetListItemSplitChar(',');
	m_listCtrlLSBeamSeg.SetListItemSplitChar(',');
	m_listCtrlRSBeamSeg.SetListItemSplitChar(',');
	//
	m_listCtrlLCBeamSeg.InitListCtrl(NULL);
	m_listCtrlLCBeamSeg.SetModifyValueFunc(FireValueModify);
	m_listCtrlLCBeamSeg.SetContextMenuFunc(FireContextMenu);
	lpInfo=new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("节间距离"),TRUE);
	m_listCtrlLCBeamSeg.InsertRootItem(lpInfo)->m_idProp=SECT_LENGTH;
	lpInfo=new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("正面布材"),TRUE);
	m_listCtrlLCBeamSeg.InsertRootItem(lpInfo)->m_idProp=FRONT_PANEL;
	lpInfo=new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("水平布材"),TRUE);
	m_listCtrlLCBeamSeg.InsertRootItem(lpInfo)->m_idProp=BOTTOM_PANEL;
	//
	m_listCtrlRCBeamSeg.InitListCtrl(NULL);
	m_listCtrlRCBeamSeg.SetModifyValueFunc(FireValueModify);
	m_listCtrlRCBeamSeg.SetContextMenuFunc(FireContextMenu);
	lpInfo=new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("节间距离"),TRUE);
	m_listCtrlRCBeamSeg.InsertRootItem(lpInfo)->m_idProp=SECT_LENGTH;
	lpInfo=new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("正面布材"),TRUE);
	m_listCtrlRCBeamSeg.InsertRootItem(lpInfo)->m_idProp=FRONT_PANEL;
	lpInfo=new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("水平布材"),TRUE);
	m_listCtrlRCBeamSeg.InsertRootItem(lpInfo)->m_idProp=BOTTOM_PANEL;
	//
	m_listCtrlLSBeamSeg.InitListCtrl(NULL);
	m_listCtrlLSBeamSeg.SetModifyValueFunc(FireValueModify);
	m_listCtrlLSBeamSeg.SetContextMenuFunc(FireContextMenu);
	lpInfo=new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("节间距离"),TRUE);
	m_listCtrlLSBeamSeg.InsertRootItem(lpInfo)->m_idProp=SECT_LENGTH;
	lpInfo=new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("正面布材"),TRUE);
	m_listCtrlLSBeamSeg.InsertRootItem(lpInfo)->m_idProp=FRONT_PANEL;
	lpInfo=new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("水平布材"),TRUE);
	m_listCtrlLSBeamSeg.InsertRootItem(lpInfo)->m_idProp=BOTTOM_PANEL;
	//
	m_listCtrlRSBeamSeg.InitListCtrl(NULL);
	m_listCtrlRSBeamSeg.SetModifyValueFunc(FireValueModify);
	m_listCtrlRSBeamSeg.SetContextMenuFunc(FireContextMenu);
	lpInfo=new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T(".."),TRUE);
	m_listCtrlRSBeamSeg.InsertRootItem(lpInfo)->m_idProp=SECT_LENGTH;
	lpInfo=new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T(".."),TRUE);
	m_listCtrlRSBeamSeg.InsertRootItem(lpInfo)->m_idProp=FRONT_PANEL;
	lpInfo=new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T(".."),TRUE);
	m_listCtrlRSBeamSeg.InsertRootItem(lpInfo)->m_idProp=BOTTOM_PANEL;
	//
	for(int i=0;i<4;i++)
		RefreshListCtrl(i);
	RefreshBeamLenEdit();
	//
	CRect rect,clientRect;
	GetClientRect(&clientRect);
	CWnd* pWorkWnd=CWnd::GetDlgItem(IDC_SKETCH_PANEL);
	pWorkWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_xDrawRect=rect;
	m_nRightMargin=clientRect.right-rect.right;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
//
CSuperGridCtrl *CSubstationBeamSegmentPage::GetFocusListCtrl()
{
	CWnd *pWnd=GetFocus();
	HWND hFoucsWnd=NULL;
	if(pWnd)
		hFoucsWnd=pWnd->GetSafeHwnd();
	if(m_listCtrlLCBeamSeg.GetSafeHwnd()==hFoucsWnd)
		return &m_listCtrlLCBeamSeg;
	else if(m_listCtrlRCBeamSeg.GetSafeHwnd()==hFoucsWnd)
		return &m_listCtrlRCBeamSeg;
	else if(m_listCtrlLSBeamSeg.GetSafeHwnd()==hFoucsWnd)
		return &m_listCtrlLSBeamSeg;
	else if(m_listCtrlRSBeamSeg.GetSafeHwnd()==hFoucsWnd)
		return &m_listCtrlRSBeamSeg;
	else
		return NULL;
}
CSuperGridCtrl* CSubstationBeamSegmentPage::GetActiveListCtrl()
{
	if(m_iActiveListCtrl==0)
		return &m_listCtrlLCBeamSeg;
	else if(m_iActiveListCtrl==1)
		return &m_listCtrlRCBeamSeg;
	else if(m_iActiveListCtrl==2)
		return &m_listCtrlLSBeamSeg;
	else if(m_iActiveListCtrl==3)
		return &m_listCtrlRSBeamSeg;
	else
		return NULL;
}
//根据节间数更新列表项
void CSubstationBeamSegmentPage::RefreshListCtrl(int iActiveCtrl/*=0*/)
{
	m_iActiveListCtrl=iActiveCtrl;
	CSuperGridCtrl* pListCtrl=GetActiveListCtrl();
	if(pListCtrl==NULL)
		return;
	double fSBeamLen=m_fSpanLen-(m_fLeftBeamLength+m_fRightBeamLength);
	int nSetDist=0,nSetMent=0;
	if(m_iActiveListCtrl==0||m_iActiveListCtrl==1)
	{
		nSetMent=m_nCBeamNum;
		if(m_iActiveListCtrl==0)
			nSetDist=(int)(m_fLeftBeamLength/nSetMent);
		else
			nSetDist=(int)(m_fRightBeamLength/nSetMent);
	}
	else
	{
		nSetMent=m_nSBeamNum;//*2;
		nSetDist=(int)(fSBeamLen/(nSetMent*2));
	}
	//更新列
	int nColCount=pListCtrl->GetHeaderCtrl()->GetItemCount();
	for(int i=1;i<nColCount;i++)
		pListCtrl->DeleteColumn(1);
	for(int i=1;i<=nSetMent+1;i++)
	{
		LV_COLUMN lvc;
		lvc.iSubItem = i;
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvc.fmt = LVCFMT_LEFT;
		lvc.cx=40;
		lvc.pszText=CXhChar16("%d",i);
		pListCtrl->InsertColumn(i,&lvc);
		pListCtrl->RedrawItems(0,1);
	}
	//更新内容项
	CSuperGridCtrl::CTreeItem* pItem=NULL;
	for(int i=1;i<=nSetMent;i++)
	{
		pItem=pListCtrl->FindTreeItemById(SECT_LENGTH,NULL);
		pListCtrl->SetSubItemText(pItem,i,CXhChar16("%d",nSetDist));
		pItem=pListCtrl->FindTreeItemById(BOTTOM_PANEL,NULL);	//设定底面默认拉花布材型式
		pItem->m_lpNodeInfo->SetControlType(i,GCT_CMB_EDIT);
		pItem->m_lpNodeInfo->SetListItemsStr(i,MakeLayerStrArr());
		if(m_iActiveListCtrl%2==0)
			pListCtrl->SetSubItemText(pItem,i,"X|");	//左侧
		else
			pListCtrl->SetSubItemText(pItem,i,"|X");	//右侧
		pItem=pListCtrl->FindTreeItemById(FRONT_PANEL,NULL);	//设定正面默认拉花布材型式
		pItem->m_lpNodeInfo->SetControlType(i,GCT_CMB_EDIT);
		pItem->m_lpNodeInfo->SetListItemsStr(i,MakeLayerStrArr());
		if(m_iActiveListCtrl==0&&i==1)
			pListCtrl->SetSubItemText(pItem,i,"/|");
		else if(m_iActiveListCtrl==1&&i==nSetMent)
			pListCtrl->SetSubItemText(pItem,i,"|\\");
		else if(m_iActiveListCtrl%2==0)
			pListCtrl->SetSubItemText(pItem,i,"\\|");
		else
			pListCtrl->SetSubItemText(pItem,i,"|/");
	}
	//添加一行空记录
	int iCurCol=nSetMent+1;
	pItem=pListCtrl->FindTreeItemById(SECT_LENGTH,NULL);
	pListCtrl->SetSubItemText(pItem,iCurCol,_T(""));
	pItem=pListCtrl->FindTreeItemById(BOTTOM_PANEL,NULL);
	pItem->m_lpNodeInfo->SetControlType(iCurCol,GCT_CMB_EDIT);
	pItem->m_lpNodeInfo->SetListItemsStr(iCurCol,MakeLayerStrArr());
	pListCtrl->SetSubItemText(pItem,iCurCol,_T(""));
	pItem=pListCtrl->FindTreeItemById(FRONT_PANEL,NULL);
	pItem->m_lpNodeInfo->SetControlType(iCurCol,GCT_CMB_EDIT);
	pItem->m_lpNodeInfo->SetListItemsStr(iCurCol,MakeLayerStrArr());
	pListCtrl->SetSubItemText(pItem,iCurCol,_T(""));
}
void CSubstationBeamSegmentPage::RefreshBeamLenEdit()
{
	((CEdit*)GetDlgItem(IDC_E_LEFT_BEAM_LEN))->SetReadOnly();
	((CEdit*)GetDlgItem(IDC_E_RIGHT_BEAM_LEN))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_E_SBEAM))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_E_SUBSTATION_LEN))->SetReadOnly(TRUE);
	//
	CSuperGridCtrl::CTreeItem* pItem=NULL;
	CString sOffset;
	m_fLeftBeamLength=0;
	pItem=m_listCtrlLCBeamSeg.FindItemByPropId(SECT_LENGTH,NULL);
	for(int i=1;i<m_listCtrlLCBeamSeg.GetHeaderCtrl()->GetItemCount();i++)
	{
		sOffset=pItem->m_lpNodeInfo->GetSubItemText(i);
		double offset=sOffset.GetLength()>0?atof(sOffset):0;
		m_fLeftBeamLength+=offset;
	}
	m_fRightBeamLength=0;
	pItem=m_listCtrlRCBeamSeg.FindItemByPropId(SECT_LENGTH,NULL);
	for(int i=1;i<m_listCtrlRCBeamSeg.GetHeaderCtrl()->GetItemCount();i++)
	{
		sOffset=pItem->m_lpNodeInfo->GetSubItemText(i);
		double offset=sOffset.GetLength()>0?atof(sOffset):0;
		m_fRightBeamLength+=offset;
	}
	m_fSBeamLen=m_fLSBeamLen=0;
	pItem=m_listCtrlLSBeamSeg.FindItemByPropId(SECT_LENGTH,NULL);
	for(int i=1;i<m_listCtrlLSBeamSeg.GetHeaderCtrl()->GetItemCount();i++)
	{
		sOffset=pItem->m_lpNodeInfo->GetSubItemText(i);
		double offset=sOffset.GetLength()>0?atof(sOffset):0;
		m_fSBeamLen+=offset;
		m_fLSBeamLen+=offset;
	}
	pItem=m_listCtrlRSBeamSeg.FindItemByPropId(SECT_LENGTH,NULL);
	for(int i=1;i<m_listCtrlRSBeamSeg.GetHeaderCtrl()->GetItemCount();i++)
	{
		sOffset=pItem->m_lpNodeInfo->GetSubItemText(i);
		double offset=sOffset.GetLength()>0?atof(sOffset):0;
		m_fSBeamLen+=offset;
	}
	m_fSpanLen=m_fLeftBeamLength+m_fRightBeamLength+m_fSBeamLen;
	UpdateData(FALSE);
	Invalidate();
}
//根据左开口梁信息更新右开口梁信息
CXhChar16 MirrorLayoutStr(const char* sLayoutStr)
{
	CXhChar16 layout,layout2;
	char* iter=layout.Copy(sLayoutStr);
	while(*iter!=0){
		if(*iter=='/')
			*iter='\\';
		else if(*iter=='\\')
			*iter='/';
		layout2.InsertBefore(*iter);
		iter++;
	};
	return layout2;
}
void CSubstationBeamSegmentPage::RefreshRCBeamListCtrl()
{
	//更新标题列
	int nColCount=m_listCtrlRCBeamSeg.GetHeaderCtrl()->GetItemCount();
	for(int i=1;i<nColCount;i++)
		m_listCtrlRCBeamSeg.DeleteColumn(1);
	nColCount=m_listCtrlLCBeamSeg.GetHeaderCtrl()->GetItemCount();
	for(int i=1;i<nColCount;i++)
	{
		LV_COLUMN lvc;
		lvc.iSubItem = i;
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvc.fmt = LVCFMT_LEFT;
		lvc.cx=40;
		lvc.pszText=CXhChar16("%d",i);
		m_listCtrlRCBeamSeg.InsertColumn(i,&lvc);
		m_listCtrlRCBeamSeg.RedrawItems(0,1);
	}
	//更新数据项
	CString sValue;
	CSuperGridCtrl::CTreeItem* pLCBeamItem=NULL,*pRCBeamItem=NULL;
	int j=1;
	for(int i=nColCount-1;i>=1;i--,j++)
	{	//节距
		pLCBeamItem=m_listCtrlLCBeamSeg.FindItemByPropId(SECT_LENGTH,NULL);
		sValue=pLCBeamItem->m_lpNodeInfo->GetSubItemText(i);
		pRCBeamItem=m_listCtrlRCBeamSeg.FindItemByPropId(SECT_LENGTH,NULL);
		m_listCtrlRCBeamSeg.SetSubItemText(pRCBeamItem,j,sValue);
		//水平布局形式
		pLCBeamItem=m_listCtrlLCBeamSeg.FindTreeItemById(BOTTOM_PANEL,NULL);
		sValue=pLCBeamItem->m_lpNodeInfo->GetSubItemText(i);
		pRCBeamItem=m_listCtrlRCBeamSeg.FindTreeItemById(BOTTOM_PANEL,NULL);
		pRCBeamItem->m_lpNodeInfo->SetControlType(j,GCT_CMB_EDIT);
		pRCBeamItem->m_lpNodeInfo->SetListItemsStr(j,MakeLayerStrArr());
		m_listCtrlRCBeamSeg.SetSubItemText(pRCBeamItem,j,MirrorLayoutStr(sValue));
		//竖直布局形式
		pLCBeamItem=m_listCtrlLCBeamSeg.FindTreeItemById(FRONT_PANEL,NULL);
		sValue=pLCBeamItem->m_lpNodeInfo->GetSubItemText(i);
		pRCBeamItem=m_listCtrlRCBeamSeg.FindTreeItemById(FRONT_PANEL,NULL);
		pRCBeamItem->m_lpNodeInfo->SetControlType(j,GCT_CMB_EDIT);
		pRCBeamItem->m_lpNodeInfo->SetListItemsStr(j,MakeLayerStrArr());
		m_listCtrlRCBeamSeg.SetSubItemText(pRCBeamItem,j,MirrorLayoutStr(sValue));
	}
}
//根据直梁左侧节间信息更新右侧节间信息
void CSubstationBeamSegmentPage::RefreshRSBeamListCtrl()
{
	//更新标题列
	int nColCount=m_listCtrlRSBeamSeg.GetHeaderCtrl()->GetItemCount();
	for(int i=1;i<nColCount;i++)
		m_listCtrlRSBeamSeg.DeleteColumn(1);
	nColCount=m_listCtrlLSBeamSeg.GetHeaderCtrl()->GetItemCount();
	for(int i=1;i<nColCount;i++)
	{
		LV_COLUMN lvc;
		lvc.iSubItem = i;
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvc.fmt = LVCFMT_LEFT;
		lvc.cx=40;
		lvc.pszText=CXhChar16("%d",i);
		m_listCtrlRSBeamSeg.InsertColumn(i,&lvc);
		m_listCtrlRSBeamSeg.RedrawItems(0,1);
	}
	//更新数据项
	CString sValue;
	CSuperGridCtrl::CTreeItem* pLSBeamItem=NULL,*pRSBeamItem=NULL;
	int j=1;
	for(int i=nColCount-1;i>=1;i--,j++)
	{	//节距
		pLSBeamItem=m_listCtrlLSBeamSeg.FindItemByPropId(SECT_LENGTH,NULL);
		sValue=pLSBeamItem->m_lpNodeInfo->GetSubItemText(i);
		pRSBeamItem=m_listCtrlRSBeamSeg.FindItemByPropId(SECT_LENGTH,NULL);
		m_listCtrlRSBeamSeg.SetSubItemText(pRSBeamItem,j,sValue);
		//水平布局形式
		pLSBeamItem=m_listCtrlLSBeamSeg.FindItemByPropId(BOTTOM_PANEL,NULL);
		sValue=pLSBeamItem->m_lpNodeInfo->GetSubItemText(i);
		pRSBeamItem=m_listCtrlRSBeamSeg.FindItemByPropId(BOTTOM_PANEL,NULL);
		pRSBeamItem->m_lpNodeInfo->SetControlType(j,GCT_CMB_EDIT);
		pRSBeamItem->m_lpNodeInfo->SetListItemsStr(j,MakeLayerStrArr());
		m_listCtrlRSBeamSeg.SetSubItemText(pRSBeamItem,j,MirrorLayoutStr(sValue));
		//竖直布局形式
		pLSBeamItem=m_listCtrlLSBeamSeg.FindItemByPropId(FRONT_PANEL,NULL);
		sValue=pLSBeamItem->m_lpNodeInfo->GetSubItemText(i);
		pRSBeamItem=m_listCtrlRSBeamSeg.FindItemByPropId(FRONT_PANEL,NULL);
		pRSBeamItem->m_lpNodeInfo->SetControlType(j,GCT_CMB_EDIT);
		pRSBeamItem->m_lpNodeInfo->SetListItemsStr(j,MakeLayerStrArr());	
		m_listCtrlRSBeamSeg.SetSubItemText(pRSBeamItem,j,MirrorLayoutStr(sValue));
	}
}
void CSubstationBeamSegmentPage::ExtractBeamSegment()
{
	CSuperGridCtrl::CTreeItem* pDistItem=NULL,*pLinkItem1=NULL,*pLinkItem2=NULL;
	CString sOffset,sLevelLink,sVertiLink;
	//获取左侧开口梁节间信息
	pDistItem =m_listCtrlLCBeamSeg.FindItemByPropId(SECT_LENGTH,NULL);
	pLinkItem1=m_listCtrlLCBeamSeg.FindItemByPropId(BOTTOM_PANEL,NULL);
	pLinkItem2=m_listCtrlLCBeamSeg.FindItemByPropId(FRONT_PANEL ,NULL);
	for(int i=1;i<m_listCtrlLCBeamSeg.GetHeaderCtrl()->GetItemCount();i++)
	{
		sOffset=pDistItem->m_lpNodeInfo->GetSubItemText(i);
		double offset=sOffset.GetLength()>0?atof(sOffset):0;
		sLevelLink=pLinkItem1->m_lpNodeInfo->GetSubItemText(i);
		sVertiLink=pLinkItem2->m_lpNodeInfo->GetSubItemText(i);
		if(offset>0)
		{
			m_xLCBeamSegArr.append(SECTION(offset,sLevelLink));
			m_xLTBeamSegArr.append(SECTION(offset,sVertiLink));
		}
	}
	//获取直口梁左侧节间信息
	pDistItem =m_listCtrlLSBeamSeg.FindItemByPropId(SECT_LENGTH,NULL);
	pLinkItem1=m_listCtrlLSBeamSeg.FindItemByPropId(BOTTOM_PANEL,NULL);
	pLinkItem2=m_listCtrlLSBeamSeg.FindItemByPropId(FRONT_PANEL ,NULL);
	for(int i=1;i<m_listCtrlLSBeamSeg.GetHeaderCtrl()->GetItemCount();i++)
	{
		sOffset=pDistItem->m_lpNodeInfo->GetSubItemText(i);
		double offset=sOffset.GetLength()>0?atof(sOffset):0;
		sLevelLink=pLinkItem1->m_lpNodeInfo->GetSubItemText(i);
		sVertiLink=pLinkItem2->m_lpNodeInfo->GetSubItemText(i);
		if(offset>0)
		{
			m_xLSBeamSegArr.append(SECTION(offset,sLevelLink));
			m_xLTBeamSegArr.append(SECTION(offset,sVertiLink));
		}
	}
	//获取直口梁右侧节间信息
	pDistItem =m_listCtrlRSBeamSeg.FindItemByPropId(SECT_LENGTH,NULL);
	pLinkItem1=m_listCtrlRSBeamSeg.FindItemByPropId(BOTTOM_PANEL,NULL);
	pLinkItem2=m_listCtrlRSBeamSeg.FindItemByPropId(FRONT_PANEL ,NULL);
	for(int i=1;i<m_listCtrlRSBeamSeg.GetHeaderCtrl()->GetItemCount();i++)
	{
		sOffset=pDistItem->m_lpNodeInfo->GetSubItemText(i);
		double offset=sOffset.GetLength()>0?atof(sOffset):0;
		sLevelLink=pLinkItem1->m_lpNodeInfo->GetSubItemText(i);
		sVertiLink=pLinkItem2->m_lpNodeInfo->GetSubItemText(i);
		if(offset>0)
		{
			m_xRSBeamSegArr.append(SECTION(offset,sLevelLink));
			m_xRTBeamSegArr.append(SECTION(offset,sVertiLink));
		}
	}
	//获取右侧开口梁节间信息
	pDistItem =m_listCtrlRCBeamSeg.FindItemByPropId(SECT_LENGTH,NULL);
	pLinkItem1=m_listCtrlRCBeamSeg.FindItemByPropId(BOTTOM_PANEL,NULL);
	pLinkItem2=m_listCtrlRCBeamSeg.FindItemByPropId(FRONT_PANEL ,NULL);
	for(int i=1;i<m_listCtrlRCBeamSeg.GetHeaderCtrl()->GetItemCount();i++)
	{
		sOffset=pDistItem->m_lpNodeInfo->GetSubItemText(i);
		double offset=sOffset.GetLength()>0?atof(sOffset):0;
		sLevelLink=pLinkItem1->m_lpNodeInfo->GetSubItemText(i);
		sVertiLink=pLinkItem2->m_lpNodeInfo->GetSubItemText(i);
		if(offset>0)
		{
			m_xRCBeamSegArr.append(SECTION(offset,sLevelLink));
			m_xRTBeamSegArr.append(SECTION(offset,sVertiLink));
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//消息处理
void CSubstationBeamSegmentPage::OnNewItem()
{
	CSuperGridCtrl* pListCtrl=GetFocusListCtrl();
	if(pListCtrl==NULL)
		return;
	int iCurCol = pListCtrl->GetCurSubItem();
	if(iCurCol==0)	//向后插
	{
		int nColCount=pListCtrl->GetHeaderCtrl()->GetItemCount();
		LV_COLUMN lvc;
		lvc.iSubItem = nColCount;
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvc.fmt = LVCFMT_LEFT;
		lvc.cx=40;
		lvc.pszText=CXhChar16("%d",nColCount);
		pListCtrl->InsertColumn(nColCount,&lvc);
		iCurCol=nColCount;
	}
	else	//向前插
	{
		pListCtrl->_InsertColumn(iCurCol,CXhChar16("%d",iCurCol),40);
		int nColCount=pListCtrl->GetHeaderCtrl()->GetItemCount();
		for(int i=iCurCol+1;i<nColCount;i++)
			pListCtrl->SetColumnHeaderTitle(i,CXhChar16("%d",i));
	}
	pListCtrl->RedrawItems(0,1);
	//插入数据
	CSuperGridCtrl::CTreeItem* pItem=NULL;
	pItem=pListCtrl->FindTreeItemById(SECT_LENGTH,NULL);
	pListCtrl->SetSubItemText(pItem,iCurCol,_T("1500"));
	pItem=pListCtrl->FindTreeItemById(BOTTOM_PANEL,NULL);
	pItem->m_lpNodeInfo->SetControlType(iCurCol,GCT_CMB_EDIT);
	pItem->m_lpNodeInfo->SetListItemsStr(iCurCol,MakeLayerStrArr());
	pListCtrl->SetSubItemText(pItem,iCurCol,_T("X"));
	pItem=pListCtrl->FindTreeItemById(FRONT_PANEL,NULL);
	pItem->m_lpNodeInfo->SetControlType(iCurCol,GCT_CMB_EDIT);
	pItem->m_lpNodeInfo->SetListItemsStr(iCurCol,MakeLayerStrArr());
	pListCtrl->SetSubItemText(pItem,iCurCol,_T("\\|"));
	//
	if(pListCtrl->GetSafeHwnd()==m_listCtrlLCBeamSeg.GetSafeHwnd())
		RefreshRCBeamListCtrl();
	if(pListCtrl->GetSafeHwnd()==m_listCtrlLSBeamSeg.GetSafeHwnd())
		RefreshRSBeamListCtrl();
	RefreshBeamLenEdit();
}
void CSubstationBeamSegmentPage::OnDeleteItem()
{
	CSuperGridCtrl* pListCtrl=GetFocusListCtrl();
	if(pListCtrl==NULL)
		return;
	int iCurCol = pListCtrl->GetCurSubItem();
	if(iCurCol==0)	//不删除标题列
		return;
	int nColCount=pListCtrl->GetHeaderCtrl()->GetItemCount();
	pListCtrl->_DeleteColumn(iCurCol);
	nColCount--;
	for(int i=iCurCol;i<nColCount;i++)
		pListCtrl->SetColumnHeaderTitle(i,CXhChar16("%d",i));
	pListCtrl->RedrawItems(0,1);
	if(pListCtrl->GetSafeHwnd()==m_listCtrlLCBeamSeg.GetSafeHwnd())
		RefreshRCBeamListCtrl();
	if(pListCtrl->GetSafeHwnd()==m_listCtrlLSBeamSeg.GetSafeHwnd())
		RefreshRSBeamListCtrl();
	RefreshBeamLenEdit();
}
void CSubstationBeamSegmentPage::OnOK()
{
	UpdateData(TRUE);
	ExtractBeamSegment();
	CDialog::OnOK();
}
void CSubstationBeamSegmentPage::OnSize(UINT nType, int cx, int cy)
{
	CRect rect;
	int nLeftLen=0,nRightLen=0;
	CWnd *pWnd=NULL,*pListCtrl=NULL;
	pWnd=CWnd::GetDlgItem(IDC_SKETCH_PANEL);
	if(pWnd->GetSafeHwnd()!=NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.right=cx-m_nRightMargin;
		pWnd->MoveWindow(&rect);
		m_xDrawRect=rect;
	}
	pListCtrl=CWnd::GetDlgItem(IDC_LIST_L_CBEAM_SEG);
	if(pListCtrl->GetSafeHwnd()!=NULL)
	{
		pListCtrl->GetWindowRect(&rect);
		ScreenToClient(&rect);
		int nHalfLen=(int)((cx-m_nRightMargin-rect.left)*0.5);
		nLeftLen=nHalfLen+30;
		nRightLen=nHalfLen-30;
		rect.right=rect.left+nLeftLen;
		pListCtrl->MoveWindow(&rect);
	}
	pListCtrl=CWnd::GetDlgItem(IDC_LIST_R_CBEAM_SEG);
	if(pListCtrl->GetSafeHwnd()!=NULL)
	{
		pListCtrl->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.right=cx-m_nRightMargin;
		rect.left=rect.right-nRightLen;
		pListCtrl->MoveWindow(&rect);
		//
		CRect static_rect;
		pWnd=CWnd::GetDlgItem(IDC_S_R_CBEAM);
		pWnd->GetWindowRect(&static_rect);
		ScreenToClient(&static_rect);
		int nSize=static_rect.Width();
		static_rect.left=rect.left;
		static_rect.right=rect.left+nSize;
		pWnd->MoveWindow(&static_rect);
	}
	pListCtrl=CWnd::GetDlgItem(IDC_LIST_L_SBEAM_SEG);
	if(pListCtrl->GetSafeHwnd()!=NULL)
	{
		pListCtrl->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.right=rect.left+nLeftLen;
		pListCtrl->MoveWindow(&rect);
	}
	pListCtrl=CWnd::GetDlgItem(IDC_LIST_R_SBEAM_SEG);
	if(pListCtrl->GetSafeHwnd()!=NULL)
	{
		pListCtrl->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.right=cx-m_nRightMargin;
		rect.left=rect.right-nRightLen;
		pListCtrl->MoveWindow(&rect);
		//
		CRect static_rect;
		pWnd=CWnd::GetDlgItem(IDC_S_R_BEAM);
		pWnd->GetWindowRect(&static_rect);
		ScreenToClient(&static_rect);
		int nSize=static_rect.Width();
		static_rect.left=rect.left;
		static_rect.right=rect.left+nSize;
		pWnd->MoveWindow(&static_rect);
	}
	CDialog::OnSize(nType, cx, cy);
	InvalidateRect(m_xDrawRect);
}
void CSubstationBeamSegmentPage::OnPaint()
{
	CPaintDC dc(this);				//
	CWnd* pSketchPanel=CWnd::GetDlgItem(IDC_SKETCH_PANEL);
	if(pSketchPanel->GetSafeHwnd()!=NULL)
	{
		CDC*  pDC=pSketchPanel->GetDC();
		if(pDC==NULL) 
			return;
		CRect rect;
		pSketchPanel->GetClientRect(&rect);
		//初始化横梁轮廓点及区域，确定绘图坐标系
		double fWidth1=(m_fWidth2-m_fWidth1)*0.5;
		double fWidth2=(m_fWidth2-m_fWidth3)*0.5;
		f3dPoint xLFCBpt,xLBCBpt,xRFCBpt,xRBCBpt;
		f3dPoint xLFSBpt,xLBSBpt,xRFSBpt,xRBSBpt;
		f3dPoint xStartPt,xEndpt,xLTpt,xRTpt;
		xLFCBpt.Set(0,-(m_fWidth1+fWidth1),0);
		xLBCBpt.Set(0,-fWidth1,0);
		xLFSBpt.Set(m_fLeftBeamLength,-m_fWidth2,0);
		xLBSBpt.Set(m_fLeftBeamLength,0,0);
		xRFSBpt.Set(m_fLeftBeamLength+m_fSBeamLen,-m_fWidth2,0);
		xRBSBpt.Set(m_fLeftBeamLength+m_fSBeamLen,0,0);
		xRFCBpt.Set(m_fLeftBeamLength+m_fSBeamLen+m_fRightBeamLength,-(m_fWidth3+fWidth2),0);
		xRBCBpt.Set(m_fLeftBeamLength+m_fSBeamLen+m_fRightBeamLength,-fWidth2,0);
		xStartPt.Set(0,0,0);
		xEndpt.Set(m_fLeftBeamLength+m_fSBeamLen+m_fRightBeamLength,0,0);
		xLTpt.Set(0,m_fBeamHeight,0);
		xRTpt.Set(0,m_fBeamHeight,0);
		//
		SCOPE_STRU scope;
		scope.VerifyVertex(f3dPoint(0,m_fBeamHeight));
		scope.VerifyVertex(f3dPoint(m_fLeftBeamLength+m_fSBeamLen+m_fRightBeamLength,m_fBeamHeight));
		scope.VerifyVertex(f3dPoint(0,-m_fWidth2));
		scope.VerifyVertex(f3dPoint(m_fLeftBeamLength+m_fSBeamLen+m_fRightBeamLength,-m_fWidth2));
		double scale=0.9*min(rect.Width()/scope.wide(),rect.Height()/scope.high());
		UCS_STRU rot_ucs;
		rot_ucs.axis_x.Set(1,0,0);
		rot_ucs.axis_y.Set(0,1,0);
		rot_ucs.axis_z.Set(0,0,-1);
		rot_ucs.origin.Set((scope.fMinX+0.5*scope.wide()-0.5*rect.Width()/scale),(scope.fMinY+0.5*scope.high()+0.5*rect.Height()/scale));
		rot_ucs.axis_y*=-1;
		//绘制曲梁节间
		CString sOffset,sValue,sValue2;
		CSuperGridCtrl::CTreeItem* pDistItem=NULL,*pLinkItem=NULL,*pVertItem=NULL;
		f3dPoint startFPt,endFPt,startBPt,endBPt,vectorF,vectorB;
		f3dPoint startTPt,endTPt,btmStartPt,btmEndPt;
		BOOL bHasCurvedBeam=FALSE;
		if(m_listCtrlLCBeamSeg.GetHeaderCtrl()->GetItemCount()>1)
			bHasCurvedBeam=TRUE;
		CPen bluePen(PS_SOLID,2,RGB(0,0,255)),redPen(PS_SOLID,2,RGB(255,0,0)),greenPen(PS_SOLID,2,RGB(0,255,0));
		//绘制左侧曲梁
		CPen *pOldPen=pDC->SelectObject(&bluePen);
		vectorF=xLFSBpt-xLFCBpt;
		normalize(vectorF);
		vectorB=xLBSBpt-xLBCBpt;
		normalize(vectorB);
		startFPt=xLFCBpt;startBPt=xLBCBpt;
		pDistItem =m_listCtrlLCBeamSeg.FindItemByPropId(SECT_LENGTH,NULL);
		pLinkItem =m_listCtrlLCBeamSeg.FindItemByPropId(BOTTOM_PANEL,NULL);
		pVertItem =m_listCtrlLCBeamSeg.FindItemByPropId(FRONT_PANEL ,NULL);
		if(m_listCtrlLCBeamSeg.GetHeaderCtrl()->GetItemCount()>0)
			xLTpt.x=atof(m_listCtrlLCBeamSeg.GetItemText(0,1));
		if(m_listCtrlRCBeamSeg.GetHeaderCtrl()->GetItemCount()>0)
		{
			double dist=0;
			int j=m_listCtrlRCBeamSeg.GetHeaderCtrl()->GetItemCount()-1;
			do{
				CString stext=m_listCtrlRCBeamSeg.GetItemText(0,j);
				dist=atof(stext);
				xRTpt.x=scope.fMaxX-dist;
				j--;
			}while(dist==0&&j>=0);
		}
		const BYTE cbShuFlag=0x01;
		const BYTE cbPieFlag=0x02;
		const BYTE cbNaFlag =0x04;
		for(int i=1;i<m_listCtrlLCBeamSeg.GetHeaderCtrl()->GetItemCount();i++)
		{
			sOffset=pDistItem->m_lpNodeInfo->GetSubItemText(i);
			sValue=pLinkItem->m_lpNodeInfo->GetSubItemText(i);
			sValue2=pVertItem->m_lpNodeInfo->GetSubItemText(i);
			double offset=sOffset.GetLength()>0?atof(sOffset):0;
			if(offset<=0)
				continue;
			//水平布材
			endFPt=startFPt+vectorF*(offset/vectorF.x);
			endBPt=startBPt+vectorB*(offset/vectorF.x);
			DrawBeamSegLines(pDC,rot_ucs,sValue,scale,startFPt,endFPt,startBPt,endBPt);
			//正面布材
			btmStartPt.Set(startTPt.x,0,0);
			BYTE cbStroke=0xff;
			if(i==1)
			{
				startTPt.Set(offset,xLTpt.y);
				endTPt=startTPt;
				cbStroke=cbPieFlag|cbShuFlag;
				sValue2.TrimLeft('|');
			}
			else
				endTPt.Set(startTPt.x+offset,startTPt.y,0);
			btmEndPt.Set(endTPt.x,0,0);
			DrawBeamSegLines(pDC,rot_ucs,sValue2,scale,btmStartPt,btmEndPt,startTPt,endTPt,cbStroke);
			startTPt=endTPt;
			startFPt=endFPt;
			startBPt=endBPt;
		}
		//绘制直梁左半部
		pDC->SelectObject(&redPen);
		vectorF=xRFSBpt-xLFSBpt;
		normalize(vectorF);
		vectorB=xRBSBpt-xLBSBpt;
		normalize(vectorB);
		startFPt=xLFSBpt;startBPt=xLBSBpt;
		pDistItem =m_listCtrlLSBeamSeg.FindItemByPropId(SECT_LENGTH,NULL);
		pLinkItem =m_listCtrlLSBeamSeg.FindItemByPropId(BOTTOM_PANEL,NULL);
		pVertItem =m_listCtrlLSBeamSeg.FindTreeItemById(FRONT_PANEL,NULL);
		for(int i=1;i<m_listCtrlLSBeamSeg.GetHeaderCtrl()->GetItemCount();i++)
		{
			sOffset=pDistItem->m_lpNodeInfo->GetSubItemText(i);
			sValue=pLinkItem->m_lpNodeInfo->GetSubItemText(i);
			sValue2=pVertItem->m_lpNodeInfo->GetSubItemText(i);
			double offset=sOffset.GetLength()>0?atof(sOffset):0;
			if(offset<=0)
				continue;
			//水平布材
			endFPt=startFPt+vectorF*offset;
			endBPt=startBPt+vectorF*offset;
			DrawBeamSegLines(pDC,rot_ucs,sValue,scale,startFPt,endFPt,startBPt,endBPt);
			startFPt=endFPt;
			startBPt=endBPt;
			//正面布材
			if(i==1&&!bHasCurvedBeam)
				startTPt.Set(xStartPt.x,xLTpt.y);
			endTPt.Set(startTPt.x+offset,startTPt.y,0);
			btmStartPt.Set(startTPt.x,0,0);
			btmEndPt.Set(endTPt.x,0,0);
			DrawBeamSegLines(pDC,rot_ucs,sValue2,scale,btmStartPt,btmEndPt,startTPt,endTPt);
			startTPt=endTPt;
		}
		//绘制直梁右半部
		pDC->SelectObject(&redPen);
		pDistItem =m_listCtrlRSBeamSeg.FindItemByPropId(SECT_LENGTH,NULL);
		pLinkItem =m_listCtrlRSBeamSeg.FindItemByPropId(BOTTOM_PANEL,NULL);
		pVertItem =m_listCtrlRSBeamSeg.FindTreeItemById(FRONT_PANEL,NULL);
		for(int i=1;i<m_listCtrlRSBeamSeg.GetHeaderCtrl()->GetItemCount();i++)
		{
			sOffset=pDistItem->m_lpNodeInfo->GetSubItemText(i);
			sValue=pLinkItem->m_lpNodeInfo->GetSubItemText(i);
			sValue2=pVertItem->m_lpNodeInfo->GetSubItemText(i);
			double offset=sOffset.GetLength()>0?atof(sOffset):0;
			if(offset<=0)
				continue;
			//水平布材
			endFPt=startFPt+vectorF*offset;
			endBPt=startBPt+vectorF*offset;
			DrawBeamSegLines(pDC,rot_ucs,sValue,scale,startFPt,endFPt,startBPt,endBPt);
			startFPt=endFPt;
			startBPt=endBPt;
			//正面布材
			if(i==m_listCtrlRSBeamSeg.GetHeaderCtrl()->GetItemCount()-1&&!bHasCurvedBeam)
			{
				xRTpt.x=xEndpt.x;
				continue;
			}
			endTPt.Set(startTPt.x+offset,startTPt.y,0);
			btmStartPt.Set(startTPt.x,0,0);
			btmEndPt.Set(endTPt.x,0,0);
			DrawBeamSegLines(pDC,rot_ucs,sValue2,scale,btmStartPt,btmEndPt,startTPt,endTPt);
			startTPt=endTPt;
		}
		//绘制右侧曲梁
		pDC->SelectObject(&bluePen);
		vectorF=xRFCBpt-xRFSBpt;
		normalize(vectorF);
		vectorB=xRBCBpt-xRBSBpt;
		normalize(vectorB);
		startFPt=xRFSBpt;
		startBPt=xRBSBpt;
		pDistItem =m_listCtrlRCBeamSeg.FindItemByPropId(SECT_LENGTH,NULL);
		pLinkItem =m_listCtrlRCBeamSeg.FindItemByPropId(BOTTOM_PANEL,NULL);
		pVertItem =m_listCtrlRCBeamSeg.FindTreeItemById(FRONT_PANEL,NULL);
		for(int i=1;i<m_listCtrlRCBeamSeg.GetHeaderCtrl()->GetItemCount();i++)
		{
			sOffset=pDistItem->m_lpNodeInfo->GetSubItemText(i);
			sValue=pLinkItem->m_lpNodeInfo->GetSubItemText(i);
			sValue2=pVertItem->m_lpNodeInfo->GetSubItemText(i);
			double offset=sOffset.GetLength()>0?atof(sOffset):0;
			if(offset<=0)
				continue;
			//水平布材
			endFPt=startFPt+vectorF*offset;
			endBPt=startBPt+vectorB*offset;
			DrawBeamSegLines(pDC,rot_ucs,sValue,scale,startFPt,endFPt,startBPt,endBPt);
			startFPt=endFPt;
			startBPt=endBPt;
			//正面布材
			btmStartPt.Set(startTPt.x,0,0);
			BYTE cbStroke=0xff;
			if(i==m_listCtrlRCBeamSeg.GetHeaderCtrl()->GetItemCount()-1||
				pDistItem->m_lpNodeInfo->GetSubItemText(i+1).GetLength()<=0)
			{
				//xRTpt.x=xRBCBpt.x-offset;
				cbStroke=cbShuFlag|cbNaFlag;
				sValue2.TrimRight('|');
				endTPt.Set(xRBCBpt.x-offset,startTPt.y);
				btmEndPt.Set(xRBCBpt.x,0,0);
			}
			else
			{
				endTPt.Set(startTPt.x+offset,startTPt.y,0);
				btmEndPt.Set(endTPt.x,0,0);
			}
			DrawBeamSegLines(pDC,rot_ucs,sValue2,scale,btmStartPt,btmEndPt,startTPt,endTPt);
			startTPt=endTPt;
		}
		//绘制构架梁轮廓边
		pDC->SelectObject(pOldPen);
		if(bHasCurvedBeam)
		{
			DrawLine(pDC,xLFCBpt,xLFSBpt,rot_ucs,scale);
			DrawLine(pDC,xLBCBpt,xLBSBpt,rot_ucs,scale);
			DrawLine(pDC,xRFSBpt,xRFCBpt,rot_ucs,scale);
			DrawLine(pDC,xRBSBpt,xRBCBpt,rot_ucs,scale);
		}
		DrawLine(pDC,xLFSBpt,xRFSBpt,rot_ucs,scale);
		DrawLine(pDC,xLBSBpt,xRBSBpt,rot_ucs,scale);
		DrawLine(pDC,xStartPt,xEndpt,rot_ucs,scale);
		DrawLine(pDC,xLTpt,xRTpt,rot_ucs,scale);
 	}
}

void CSubstationBeamSegmentPage::OnEnChangeOtherParam()
{
	UpdateData();
}
void CSubstationBeamSegmentPage::OnEnChangeEWidth1()
{
	UpdateData();
	InvalidateRect(m_xDrawRect);
}
void CSubstationBeamSegmentPage::OnEnChangeEWidth2()
{
	UpdateData();
	InvalidateRect(m_xDrawRect);
}
void CSubstationBeamSegmentPage::OnEnChangeEWidth3()
{
	UpdateData();
	InvalidateRect(m_xDrawRect);
}
void CSubstationBeamSegmentPage::OnEnChangeETopbeamH()
{
	UpdateData();
	InvalidateRect(m_xDrawRect);
}
