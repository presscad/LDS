//<LOCALE_TRANSLATE Confirm by hxr/>
// PlateLsRefDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "PlateLsRefDlg.h"
#include "env_def.h"
#include "Tower.h"
#include "LDSView.h"
#include "CfgPartNoDlg.h"
#include "lds_part.h"
#include "TowerPropertyDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifndef __TSA_
/////////////////////////////////////////////////////////////////////////////
// CPlateLsRefDlg dialog

CPlateLsRefDlg *g_pLsRefDlg;
CPlateLsRefDlg::CPlateLsRefDlg(CWnd* pParent /*=NULL*/)
	//: CDialog(CPlateLsRefDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlateLsRefDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bPlateAttachPart = FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("Handle");
	m_arrHeaderLabel.Add("Spec");
	m_arrHeaderLabel.Add("LO");
	m_arrHeaderLabel.Add("Bolt Reference Ray Number");
#else
	m_arrHeaderLabel.Add("���");
	m_arrHeaderLabel.Add("���");
	m_arrHeaderLabel.Add("ͨ��");
	m_arrHeaderLabel.Add("��˨�������ߺ�");
#endif
	m_pWorkPart=NULL;
	m_pWnd=pParent;
	oldSelectPartSet.Empty();
}


void CPlateLsRefDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlateLsRefDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlateLsRefDlg, CDialog)
	//{{AFX_MSG_MAP(CPlateLsRefDlg)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PLATE_LS_REF, OnClickListPlateLsRef)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PLATE_LS_REF, OnDblclkListPlateLsRef)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_PLATE_LS_REF, OnKeydownListPlateLsRef)
	ON_BN_CLICKED(ID_MODIFY_LSREF_RAY_NO, OnModifyLsRefRayNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlateLsRefDlg message handlers

BOOL CPlateLsRefDlg::OnInitDialog() 
{
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_PLATE_LS_REF);
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT
		| LVS_EX_HEADERDRAGDROP);
	if(m_bPlateAttachPart)
	{
		m_arrHeaderLabel.RemoveAll();
#ifdef AFX_TARG_ENU_ENGLISH
		m_arrHeaderLabel.Add("handle");
		m_arrHeaderLabel.Add("offset angle");
#else
		m_arrHeaderLabel.Add("���");
		m_arrHeaderLabel.Add("ƫ�ƽǶ�");
#endif
	}
	oldSelectPartSet.Empty();
	RefreshListViewHeader();
	CLDSParamPlate *pParamPlate=NULL;
	CLDSLineAngle *pLineAngle=NULL;
	m_pListCtrl->DeleteAllItems();
	if(m_pWorkPart&&m_pWorkPart->GetClassTypeId()==CLS_PLATE||m_pWorkPart->GetClassTypeId()==CLS_LINESLOT||
		m_pWorkPart->GetClassTypeId()==CLS_LINEFLAT||m_pWorkPart->IsTube()||m_pWorkPart->IsArcPart())
	{
		CLsRefList *pLsRefList=NULL;
		if(m_pWorkPart->GetClassTypeId()==CLS_PLATE)
			pLsRefList=((CLDSPlate*)m_pWorkPart)->GetLsRefList();
		else if(m_pWorkPart->IsLinePart())
		{
			pLsRefList=((CLDSLinePart*)m_pWorkPart)->GetLsRefList();
			if(m_pWorkPart->GetClassTypeId()==CLS_LINEFLAT)
#ifdef AFX_TARG_ENU_ENGLISH
				SetWindowText("Flat Relative Bolt List");
			else if(m_pWorkPart->GetClassTypeId()==CLS_LINESLOT)
				SetWindowText("Slot Bolt List");
			else //if(m_pWorkPart->GetClassTypeId()==CLS_LINETUBE)
				SetWindowText("Tube Relative Bolt List");
#else
				SetWindowText("��ֹ�����˨�б�");
			else if(m_pWorkPart->GetClassTypeId()==CLS_LINESLOT)
				SetWindowText("�۸ֹ�����˨�б�");
			else //if(m_pWorkPart->GetClassTypeId()==CLS_LINETUBE)
				SetWindowText("�ֹܹ�����˨�б�");
#endif
		}
		else if(m_pWorkPart->IsArcPart())
		{
			pLsRefList=((CLDSArcPart*)m_pWorkPart)->GetLsRefList();
#ifdef AFX_TARG_ENU_ENGLISH
			SetWindowText("Ring Part Relative Bolt List");
#else
			SetWindowText("���ι���������˨�б�");
#endif
		}
		for(CLsRef* pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
		{
			CLDSBolt *pBolt = pLsRef->GetLsPtr();
			CString tempStr;
			CStringArray str_arr;
			tempStr.Format("0X%X",pBolt->handle);
			str_arr.Add(tempStr);
			tempStr.Format("%sM%dX%.0f", pBolt->Grade(),pBolt->get_d(),pBolt->get_L());
			str_arr.Add(tempStr);
			tempStr.Format("%.2f",pBolt->get_L0());
			str_arr.Add(tempStr);
			tempStr.Empty();	//�������ߺ�
			for(int i=0;i<32;i++)
			{
				if(pLsRef->dwRayNo&GetSingleWord(i+1))
				{
					CString sText;
					sText.Format("%d",i+1);
					if(tempStr.GetLength()<=0)
						tempStr=sText;
					else
						tempStr+=","+sText;
				}
			}
			str_arr.Add(tempStr);
			int nItem=AppendRecord(-1,str_arr);
			m_pListCtrl->SetItemData(nItem,pLsRef->dwRayNo);
		}
	}
	else if(m_pWorkPart&&m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		pParamPlate = (CLDSParamPlate*)m_pWorkPart;
		if((pParamPlate->m_iParamType==TYPE_FL||
			pParamPlate->m_iParamType==TYPE_FLR||
			pParamPlate->m_iParamType==TYPE_FLG)&&m_bPlateAttachPart)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			SetWindowText("Flange Relative Rib Plate List");
#else
			SetWindowText("������������б�");
#endif
			if(pParamPlate&&pParamPlate->AttachPart.GetNodeNum()>0)
			{
				for(ATTACHPART_ITEM* pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
				{
					if(pItem->idClsType==CLS_PARAMPLATE)
					{
						CLDSParamPlate *pElbowPlate = (CLDSParamPlate*)console.FromPartHandle(pItem->h);
						if(pElbowPlate)
						{
							CString tempStr;
							CStringArray str_arr;
							tempStr.Format("0X%X",pElbowPlate->handle);
							str_arr.Add(tempStr);
							double Y=0;
							if(pElbowPlate->GetDesignItemValue('Y',&Y))
								tempStr.Format("%.2f",Y);
							str_arr.Add(tempStr);
							AppendRecord(-1,str_arr);
						}
					}
				}
			}
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			SetWindowText("Parametric Plate Relative Bolt List");
#else
			SetWindowText("�������������˨�б�");
#endif
			if(pParamPlate&&pParamPlate->GetLsRefList())
			{
				for(CLsRef* pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{
					CLDSBolt *pBolt = pLsRef->GetLsPtr();
					CString tempStr;
					CStringArray str_arr;
					tempStr.Format("0X%X",pBolt->handle);
					str_arr.Add(tempStr);
					tempStr.Format("%sM%dX%.0f", pBolt->Grade(),pBolt->get_d(),pBolt->get_L());
					str_arr.Add(tempStr);
					tempStr.Format("%.2f",pBolt->get_L0());
					str_arr.Add(tempStr);
					tempStr.Empty();	//�������ߺ�
					for(int i=0;i<32;i++)
					{
						if(pLsRef->dwRayNo&GetSingleWord(i+1))
						{
							CString sText;
							sText.Format("%d",i+1);
							if(tempStr.GetLength()<=0)
								tempStr=sText;
							else
								tempStr+=","+sText;
						}
					}
					str_arr.Add(tempStr);
					int nItem=AppendRecord(-1,str_arr);
					m_pListCtrl->SetItemData(nItem,pLsRef->dwRayNo);
				}
			}
		}
	}
	else if(m_pWorkPart&&m_pWorkPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		pLineAngle = (CLDSLineAngle*)m_pWorkPart;
		if(start1_end2_mid0==0)
#ifdef AFX_TARG_ENU_ENGLISH
			SetWindowText("Angle Middle Relative Bolt List");
		else if(start1_end2_mid0==1)
			SetWindowText("Angle Starting Relative Bolt List");
		else if(start1_end2_mid0==2)
			SetWindowText("Angle Ending Relative Bolt List");
		else 
			SetWindowText("Angle Relative Bolt List");
#else
			SetWindowText("�Ǹ��м������˨�б�");
		else if(start1_end2_mid0==1)
			SetWindowText("�Ǹ�ʼ�˹�����˨�б�");
		else if(start1_end2_mid0==2)
			SetWindowText("�Ǹ��ն˹�����˨�б�");
		else 
			SetWindowText("�Ǹֹ�����˨�б�");
#endif
		if(pLineAngle&&pLineAngle->GetLsRefList())
		{
			for(CLsRef* pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
			{
				if(start1_end2_mid0<3&&pLsRef->start1_end2_mid0!=start1_end2_mid0)
					continue;	//start1_end2_mid0����2��ʾ�Ǹ��ϵ����й�����˨������ֻ��ʾ��Ӧ���ֹ�����˨
				CLDSBolt *pBolt = pLsRef->GetLsPtr();
				CString tempStr;
				CStringArray str_arr;
				tempStr.Format("0X%X",pBolt->handle);
				str_arr.Add(tempStr);
				tempStr.Format("%sM%dX%.0f", pBolt->Grade(),pBolt->get_d(),pBolt->get_L());
				str_arr.Add(tempStr);
				tempStr.Format("%.2f",pBolt->get_L0());
				str_arr.Add(tempStr);
				tempStr.Empty();	//�������ߺ�
				for(int i=0;i<32;i++)
				{
					if(pLsRef->dwRayNo&GetSingleWord(i+1))
					{
						CString sText;
						sText.Format("%d",i+1);
						if(tempStr.GetLength()<=0)
							tempStr=sText;
						else
							tempStr+=","+sText;
					}
				}
				str_arr.Add(tempStr);
				int nItem=AppendRecord(-1,str_arr);
				m_pListCtrl->SetItemData(nItem,pLsRef->dwRayNo);
			}
		}
	}
	else 
		return FALSE;
	//�ƶ����ڵ�����λ��
	CSize sizeMaxScr;
	sizeMaxScr.cx = GetSystemMetrics(SM_CXSCREEN);
	sizeMaxScr.cy = GetSystemMetrics(SM_CYSCREEN);
	CRect rect;
	CWnd::GetWindowRect(rect);
	int width=rect.Width();
	int height=rect.Height();
	rect.left = sizeMaxScr.cx-width;
	rect.right=sizeMaxScr.cx;
	rect.top  =sizeMaxScr.cy-2*height;
	rect.bottom = sizeMaxScr.cy-height;
	MoveWindow(rect, TRUE);

	return CDialog::OnInitDialog();
}

void CPlateLsRefDlg::RefreshListViewHeader()
{
	LV_COLUMN lvc;
	int i, NUM_COLUMNS, nColumnCount;
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_PLATE_LS_REF);
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
		if (i==0)
			lvc.cx = 60;
		else if(i==1)
			lvc.cx = 80;
		else if(i==2)
			lvc.cx = 50;
		else if(i==3)
			lvc.cx = 200;
#else 
		if(i==1)
			lvc.cx = 90;
		else if(i==3)
			lvc.cx = 120;
		else
			lvc.cx = 55;
#endif
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}
int CPlateLsRefDlg::AppendRecord(int iItem, CStringArray &str_arr)
{
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_PLATE_LS_REF);
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

BOOL CPlateLsRefDlg::DeleteItem(int nCurSel) 
{
	/*
	char tem_str[100];
	m_pListCtrl->GetItemText(nCurSel,0,tem_str,100);
	long handle=0;
	sscanf(tem_str,"%X",&handle);
	if(m_pWorkPart&&m_pWorkPart->GetClassTypeId()==CLS_PLATE)
		((CLDSPlate*)m_pWorkPart)->DeleteLsRef(handle);
	else if(m_pWorkPart&&m_pWorkPart->GetClassTypeId()==CLS_LINEFLAT)
		((CLDSLineFlat*)m_pWorkPart)->DeleteLsRef(handle);
	else if(m_pWorkPart&&m_pWorkPart->GetClassTypeId()==CLS_LINESLOT)
		((CLDSLineSlot*)m_pWorkPart)->DeleteLsRef(handle);
	else if(m_pWorkPart&&m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		if(((CLDSParamPlate*)m_pWorkPart)->m_bStdPart)
			return;	//������ɾ����׼�����ϵ���˨
		if(((CLDSParamPlate*)m_pWorkPart)->m_iParamType==TYPE_FL&&m_bPlateAttachPart)	//���
			((CLDSParamPlate*)m_pWorkPart)->DeleteAttachPartItem(handle);
		else
			((CLDSParamPlate*)m_pWorkPart)->DeleteLsRef(handle);
	}
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(handle);
	//g_pSolidOper->KeyDown(VK_DELETE);	//ɾ������
	*/
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_PLATE_LS_REF);
	if(m_pWorkPart&&m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		if(((CLDSParamPlate*)m_pWorkPart)->m_bStdPart&&!m_bPlateAttachPart)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox("Bolt reference of standard part cannot be deleted");
#else
			MessageBox("������ɾ����׼�����ϵ���˨����");
#endif
			return FALSE;	//������ɾ����׼�����ϵ���˨����
		}
	}
	m_pListCtrl->DeleteItem(nCurSel);
	return TRUE;
}

void CPlateLsRefDlg::SelectPart(int nCurSel) 
{
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_PLATE_LS_REF);
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CLDSView *pLDSView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
	char tem_str[101];
	m_pListCtrl->GetItemText(nCurSel,0,tem_str,100);
	long hCurPart=0;
	sscanf(tem_str,"%X",&hCurPart);
	CLDSBolt *pBolt=(CLDSBolt*)console.FromPartHandle(hCurPart,CLS_BOLT);
	if(pBolt==NULL)
		return;
	
	//ѡ����˨�Լ���˨�����ӵĹ���
	PARTSET partSet;//��¼Ҫѡ�еĹ���
	partSet.Empty();
	CLDSPart *pPart;
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{	//������˨�����ӵĹ���
		CLsRefList *pLsRefList=pPart->GetLsRefList();
		if(pLsRefList&&pLsRefList->FromHandle(pBolt->handle))
			partSet.append(pPart);
	}
	partSet.append(pBolt);
	for(pPart=oldSelectPartSet.GetFirst();pPart;pPart=oldSelectPartSet.GetNext())
	{	//�����ϴ�ѡ�е���ǰ����Ҫѡ�еĹ���Ϊδѡ��״̬
		if(partSet.FromHandle(pPart->handle)==NULL)
			g_pSolidDraw->SetEntSnapStatus(pPart->handle,FALSE);
	}
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{	//���õ�ǰ��Ҫѡ�еĹ���Ϊѡ��״̬
		oldSelectPartSet.append(pPart);
		g_pSolidDraw->SetEntSnapStatus(pPart->handle);
	}
	//����������ʾ��������
	pLDSView->selectObjs.Empty();
	pLDSView->selectObjs.SetValue(pBolt->handle,pBolt);
	pPropList->HideInputCtrl();
	//�������Կ�
	pLDSView->DisplayBoltProperty(pPropList->m_nObjClassTypeId!=CLS_BOLT);
	::SetFocus(this->m_hWnd);
	m_pListCtrl->SetFocus();
	m_pListCtrl->SetItemState(nCurSel,LVIS_SELECTED,LVIS_SELECTED);
}

void CPlateLsRefDlg::OnClickListPlateLsRef(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_PLATE_LS_REF);
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(iCurSel>=0)
			SelectPart(iCurSel);
	}
	*pResult = 0;
}

void CPlateLsRefDlg::OnDblclkListPlateLsRef(NMHDR* pNMHDR, LRESULT* pResult)
{
	/*
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_PLATE_LS_REF);
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(iCurSel>=0)
			SelectPart(iCurSel);
	}
	*/
	*pResult = 0;
}

void CPlateLsRefDlg::OnKeydownListPlateLsRef(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_PLATE_LS_REF);
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_UP)
		{	//��һ��UP��ѡ���������һλ����������������ڰ���UP��ʱ������
			//��ʱ����δ�ƶ��������ֶ�����ѡ�����λ��
			if(iCurSel>=1)
				iCurSel--;	
			if(iCurSel>=0)
				SelectPart(iCurSel);
		}
		else if(pLVKeyDow->wVKey==VK_UP||pLVKeyDow->wVKey==VK_DOWN)
		{	//��һ��UP��ѡ���������һλ����������������ڰ���DOWN��ʱ������
			//��ʱ����δ�ƶ��������ֶ�����ѡ�����λ��
			if(iCurSel<m_pListCtrl->GetItemCount())
				iCurSel++;
			if(iCurSel>=0)
				SelectPart(iCurSel);
		}
		else if(pLVKeyDow->wVKey==VK_DELETE)
		{
			while(iCurSel>=0)
			{
				if(!DeleteItem(iCurSel))
					break;
				pos=m_pListCtrl->GetFirstSelectedItemPosition();
				if(pos==NULL)
					break;
				iCurSel=m_pListCtrl->GetNextSelectedItem(pos);
			}
		}
	}
	*pResult = 0;
}

void CPlateLsRefDlg::OnOK() 
{
	UpdateData();
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_PLATE_LS_REF);
	if(!m_bPlateAttachPart)
	{
		CLsRefList *pLsRefList=m_pWorkPart->GetLsRefList();
		if(pLsRefList==NULL)
			return;
		//�����˨���ã��������
		pLsRefList->Empty();
		int i, nCount=m_pListCtrl->GetItemCount();
		for(i=0; i<nCount; i++)
		{
			long hCurPart=0;
			char tem_str[101];
			m_pListCtrl->GetItemText(i,0,tem_str,100);
			sscanf(tem_str,"%X",&hCurPart);
			CLDSBolt *pBolt=(CLDSBolt*)console.FromPartHandle(hCurPart,CLS_BOLT);
			if(pBolt==NULL)
				continue;
			CLsRef *pLsRef=pLsRefList->append(pBolt->GetLsRef());
			pLsRef->dwRayNo=m_pListCtrl->GetItemData(i);
		}
	}
	else if(m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE&&m_bPlateAttachPart)
	{	//�����ϵ����
		;
	}
	DestroyWindow();
	//CDialog::OnOK();
}

void CPlateLsRefDlg::OnModifyLsRefRayNo() 
{
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_PLATE_LS_REF);
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int nCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		char tem_str[101];
		m_pListCtrl->GetItemText(nCurSel,0,tem_str,100);
		long hCurPart=0;
		sscanf(tem_str,"%X",&hCurPart);
		CLsRef *pLsRef=NULL;
		CLsRefList *pLsRefList=m_pWorkPart->GetLsRefList();
		if(pLsRefList==NULL)
			return;
		for(pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
		{
			if(pLsRef->GetLsPtr()->handle==hCurPart)
				break;
		}
		if(pLsRef==NULL)
			return;
		CString ss;
		CCfgPartNoDlg cfgdlg;
		cfgdlg.cfg_style=CFG_LSRAY_NO;
		cfgdlg.cfgword.flag.word[0]=pLsRef->dwRayNo;
		if(cfgdlg.DoModal()==IDOK)
		{
			//pLsRef->dwRayNo = cfgdlg.cfgword.word[0];
			ss.Empty();	//�������ߺ�
			for(int i=0;i<32;i++)
			{
				if(cfgdlg.cfgword.flag.word[0]&GetSingleWord(i+1))
				{
					CString sText;
					sText.Format("%d",i+1);
					if(ss.GetLength()<=0)
						ss=sText;
					else
						ss+=","+sText;
				}
			}
			m_pListCtrl->SetItemText(nCurSel,3,ss);
			m_pListCtrl->SetItemData(nCurSel,cfgdlg.cfgword.flag.word[0]);
		}
	}
}

void CPlateLsRefDlg::OnCancel() 
{
	UpdateData(FALSE);
	DestroyWindow();	
	//CDialog::OnCancel();
}

BOOL CPlateLsRefDlg::Create()
{
  return CDialog::Create(CPlateLsRefDlg::IDD);
}

void CPlateLsRefDlg::InitLsRefDlg(CLDSPart *pWorkPart,int s1_e2_m0/*=0*/,BOOL bPlateAttachPart/*=FALSE*/)
{
	m_pWorkPart=pWorkPart;
	start1_end2_mid0=s1_e2_m0;
	m_bPlateAttachPart=bPlateAttachPart;
	if(GetSafeHwnd()==0)
		Create();
	else
		OnInitDialog();
	UpdateData(FALSE);
}
#endif