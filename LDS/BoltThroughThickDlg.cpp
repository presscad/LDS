//<LOCALE_TRANSLATE Confirm by hxr/>
// BoltThroughThickDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "BoltThroughThickDlg.h"
#include "afxdialogex.h"
#include "env_def.h"

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
// CBoltThroughThickDlg �Ի���

IMPLEMENT_DYNAMIC(CBoltThroughThickDlg, CDialogEx)

CBoltThroughThickDlg::CBoltThroughThickDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBoltThroughThickDlg::IDD, pParent)
	, m_sL0(_T(""))
	, m_fL0(0)
	, m_sZOffsetStr(_T(""))
	, m_bUpdateZOffset(TRUE)
{
	m_pBolt=NULL;
}

CBoltThroughThickDlg::~CBoltThroughThickDlg()
{
}

void CBoltThroughThickDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RELATED_PART, m_xListCtrl);
	DDX_Text(pDX, IDC_E_L0_STR, m_sL0);
	DDX_Text(pDX, IDC_S_L0, m_fL0);
	DDX_Text(pDX, IDC_E_Z_OFFSET_STR, m_sZOffsetStr);
	DDX_Check(pDX, IDC_CHK_UPDATE_Z_OFFSET, m_bUpdateZOffset);
}


BEGIN_MESSAGE_MAP(CBoltThroughThickDlg, CDialogEx)
	ON_BN_CLICKED(ID_BTN_UPDATE_L0, &CBoltThroughThickDlg::OnBnClickedBtnUpdateL0)
	ON_EN_CHANGE(IDC_E_L0_STR, &CBoltThroughThickDlg::OnEnChangeEL0Str)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_LIST_RELATED_PART, &CBoltThroughThickDlg::OnNMClickListRelatedPart)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_RELATED_PART, &CBoltThroughThickDlg::OnNMRClickListRelatedPart)
	ON_COMMAND(ID_SET_ACTIVE_ITEM,OnSetToGlobal)
	ON_COMMAND(ID_DELETE_ITEM,OnRevokeGlobal)
END_MESSAGE_MAP()

// CBoltThroughThickDlg ��Ϣ�������
void FindBoltRelatedParts(const CLDSBolt *pBolt,PARTSET &partSet)
{
	partSet.Empty();
	if(pBolt==NULL)
		return;
	for(CLDSPart *pPart=console.EnumPartFirst();pPart;pPart=console.EnumPartNext())
	{
		if(pPart->FindLsByHandle(pBolt->handle))
		{
			if( pPart->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)pPart)->m_iParamType==TYPE_XEND&&
				fabs(pPart->ucs.axis_z*pBolt->get_norm())<EPS_COS2)
				continue;	//����ʮ�ֲ�岻���ڵ�ǰ���������˨
			partSet.append(pPart);
		}
	}
}

BOOL CBoltThroughThickDlg::OnInitDialog()
{
	if(m_pBolt==NULL)
		return FALSE;
	CXhChar500 sTemp;	//GetL0KeyStrĬ���ַ�����Ϊ500���������sTemp���̻�����ڴ���� wjh-2016.5.6
	m_pBolt->GetL0KeyStr(sTemp,500);
	m_sL0=sTemp;
	if(m_bUpdateZOffset)
		m_sZOffsetStr=m_pBolt->des_base_pos.norm_offset.key_str;
	m_fL0=m_pBolt->get_L0();
	GetDlgItem(IDC_E_Z_OFFSET_STR)->EnableWindow(m_bUpdateZOffset);
	if(!CDialogEx::OnInitDialog())
		return FALSE;
	//��ʼ�����������б�
#ifdef AFX_TARG_ENU_ENGLISH
	m_xListCtrl.AddColumnHeader("handle",60);
	m_xListCtrl.AddColumnHeader("part label",60);
	m_xListCtrl.AddColumnHeader("specification",70);
	m_xListCtrl.AddColumnHeader("model",60);
#else
	m_xListCtrl.AddColumnHeader("���",60);
	m_xListCtrl.AddColumnHeader("����",60);
	m_xListCtrl.AddColumnHeader("���",70);
	m_xListCtrl.AddColumnHeader("���",60);
#endif
	m_xListCtrl.InitListCtrl();
	CStringArray str_arr;
	str_arr.SetSize(4);
	for(THICK_ITEM item=m_pBolt->GetFirstL0ThickItem();!item.IsFinalKey();item=m_pBolt->GetNextL0ThickItem())
	{
		CLDSPart *pPart=Ta.Parts.FromHandle(item.val);
		if(item.cItemType==THICK_ITEM::THICK_THROUGH)
		{
			str_arr[0]="-";
			str_arr[1]="-";
			str_arr[2].Format("%d",item.val);
			str_arr[3]="-";
		}
		else if(pPart)
		{
			str_arr[0].Format("0X%X",pPart->handle);
			str_arr[1]=pPart->GetPartNo();
			pPart->GetSpecification(sTemp,FALSE);
			str_arr[2]=sTemp;
			if(item.cItemType==THICK_ITEM::PART_THROUGHnT)
				str_arr[3]="*";	//ȫ��
			else
			{
				CXhChar50 cfg_str;
				if(pPart->IsLegObj())	//���ȹ���
					pPart->cfgword.GetLegScopeStr(cfg_str,50,true);
				else
					pPart->cfgword.GetBodyScopeStr(cfg_str);
				str_arr[3]=cfg_str;
			}
		}
		else
			continue;
		int iItem=m_xListCtrl.InsertItemRecord(-1,str_arr);
		if(pPart)
		{
			m_xListCtrl.SetItemData(iItem,(DWORD)pPart);
			g_pSolidDraw->SetEntSnapStatus(pPart->handle);
		}
	}
	return TRUE;
}

void CBoltThroughThickDlg::OnBnClickedBtnUpdateL0()
{
	UpdateData();
	PARTSET partSet,actset;
	FindBoltRelatedParts(m_pBolt,partSet);
	m_sL0.Empty();
	m_xListCtrl.DeleteAllItems();
	CStringArray str_arr;
	str_arr.SetSize(4);
	CXhChar50 sTemp;
	bool bMultiModule=false;
	CLDSPart* pPart;
	CLDSModule* pModule=Ta.GetActiveModule();
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(!pModule->IsSonPart(pPart))
		{
			bMultiModule=true;
			break;
		}
	}
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		str_arr[0].Format("0X%X",pPart->handle);
		str_arr[1]=pPart->GetPartNo();
		pPart->GetSpecification(sTemp,FALSE);
		str_arr[2]=sTemp;
		BOOL moduleAdapted=pModule->IsSonPart(pPart);
		if(moduleAdapted)
			actset.append(pPart);
		if(bMultiModule&&moduleAdapted)
			str_arr[3]="*";
		else
		{
			CXhChar50 cfg_str;
			if(pPart->IsLegObj())	//���ȹ���
				pPart->cfgword.GetLegScopeStr(cfg_str,50,true);
			else
				pPart->cfgword.GetBodyScopeStr(cfg_str);
			str_arr[3]=cfg_str;
		}
		int iItem=m_xListCtrl.InsertItemRecord(-1,str_arr);
		m_xListCtrl.SetItemData(iItem,(DWORD)pPart);
		g_pSolidDraw->SetEntSnapStatus(pPart->handle);
		//ʹ��CString::Formatʱ�����ֱ�Ӳ����ַ���������Ϊ��������Ļ���
		//��������оͻ�����ڴ��ͻ���������ͻ���޷�Ԥ����(��ʹ��AppendFormat) wht 15-04-13
		if(m_sL0.GetLength()>0)
			m_sL0.Append(",");
		m_sL0.Append(str_arr[0]);
	}
	RefreshL0StrByListCtrl();
	if(m_bUpdateZOffset)
		m_sZOffsetStr=CLDSBolt::EmendZOffsetStr(m_pBolt,actset);
	UpdateData(FALSE);
	OnEnChangeEL0Str();
}

void CBoltThroughThickDlg::OnEnChangeEL0Str()
{
	UpdateData();
	CDesignThickPara thickPara;
	thickPara.key_str.Copy(m_sL0);
	m_fL0=thickPara.Thick(console.GetActiveModel(),true);
	UpdateData(FALSE);
}

void CBoltThroughThickDlg::OnDestroy()
{
	for(int i=0;i<m_xListCtrl.GetItemCount();i++)
	{
		CLDSPart *pPart=(CLDSPart*)m_xListCtrl.GetItemData(i);
		if(pPart)
			g_pSolidDraw->SetEntSnapStatus(pPart->handle,false);
	}
	CDialogEx::OnDestroy();
}

void CBoltThroughThickDlg::OnNMClickListRelatedPart(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nSelected=m_xListCtrl.GetSelectedCount();
	for(int i=0;i<m_xListCtrl.GetItemCount();i++)
	{
		CLDSPart *pPart=(CLDSPart*)m_xListCtrl.GetItemData(i);
		if(pPart==NULL)
			continue;
		if((m_xListCtrl.GetItemState(i,LVNI_SELECTED)&LVNI_SELECTED)||nSelected==0)
			g_pSolidDraw->SetEntSnapStatus(pPart->handle,true);
		else
			g_pSolidDraw->SetEntSnapStatus(pPart->handle,false);
	}	
	*pResult = 0;
}

void CBoltThroughThickDlg::RefreshL0StrByListCtrl()
{
	m_sL0.Empty();
	for(int i=0;i<m_xListCtrl.GetItemCount();i++)
	{
		CLDSPart *pPart=(CLDSPart*)m_xListCtrl.GetItemData(i);
		if(pPart)
		{
			char cScope='X';
			if(m_xListCtrl.GetItemText(i,3).Compare("*")==0)
				cScope='*';
			if(m_sL0.GetLength()>0)
				m_sL0+=(char*)CXhChar50(",0%c%X",cScope,pPart->handle);
			else 
				m_sL0.Format("0%c%X",cScope,pPart->handle);
		}
		else
		{	//��ͨ�ֶ�ָ��ͨ��ֵ
			CString sThick=m_xListCtrl.GetItemText(i,2);
			if(m_sL0.GetLength()>0)
				m_sL0+=(char*)CXhChar50(",%s",sThick);
			else 
				m_sL0.Format("%s",sThick);
		}
	}
	UpdateData(FALSE);
}
void CBoltThroughThickDlg::OnSetToGlobal()
{
	POSITION pos=m_xListCtrl.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iSel=m_xListCtrl.GetNextSelectedItem(pos);
		CLDSPart* pPart=(CLDSPart*)m_xListCtrl.GetItemData(iSel);
		if(pPart)
			m_xListCtrl.SetItemText(iSel,3,"*");
	}
	RefreshL0StrByListCtrl();
}
void CBoltThroughThickDlg::OnRevokeGlobal()
{
	POSITION pos=m_xListCtrl.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iSel=m_xListCtrl.GetNextSelectedItem(pos);
		CLDSPart* pPart=(CLDSPart*)m_xListCtrl.GetItemData(iSel);
		if(pPart)
		{
			CXhChar50 cfg_str;
			if(pPart->IsLegObj())	//���ȹ���
				pPart->cfgword.GetLegScopeStr(cfg_str,50,true);
			else
				pPart->cfgword.GetBodyScopeStr(cfg_str);
			m_xListCtrl.SetItemText(iSel,3,cfg_str);
		}
	}
	RefreshL0StrByListCtrl();
}

void CBoltThroughThickDlg::ContextMenu(CWnd* pWnd, CPoint point) 
{
	CPoint scr_point = point;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
#ifdef AFX_TARG_ENU_ENGLISH
	pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"set as the global");
	pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"cancel the global");
#else
	pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"��Ϊȫ��");
	pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ȡ��ȫ��");
#endif
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
}

void CBoltThroughThickDlg::OnNMRClickListRelatedPart(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int iCurSel = m_xListCtrl.GetSelectedItem();
	if(iCurSel<0)
		return;
	CPoint point;
	GetCursorPos(&point);
	CRect rect;
	m_xListCtrl.GetWindowRect(&rect);
	if(rect.PtInRect(point))
		ContextMenu(this,point);
	*pResult = 0;
}
#endif
