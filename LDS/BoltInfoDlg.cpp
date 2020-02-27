// BoltInfoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "BoltInfoDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"

#ifndef AFX_TARG_ENU_ENGLISH
// CBoltInfoDlg �Ի���
 CBoltInfoDlg* g_pBoltInfoDlg;
IMPLEMENT_DYNAMIC(CBoltInfoDlg, CDialogEx)

CBoltInfoDlg::BOLTINFO::BOLTINFO(CLDSBolt* pRelaBolt/*=NULL*/)
{
	if(pRelaBolt==NULL)
		return;
	pBolt=pRelaBolt;
	hBolt=pRelaBolt->handle;
	hAngle=pBolt->des_base_pos.hPart;
	iOffsetWing=pBolt->des_base_pos.offset_wing;
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pRelaBolt->BelongModel()->FromPartHandle(hAngle,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	if(pAngle==NULL)
		fWingOffsetDist=pBolt->des_base_pos.wing_offset_dist;
	else
	{
		fWingOffsetDist=pAngle->GetLsParamG(pBolt);
		CLDSPoint* pPoint=&pBolt->des_base_pos.datumPoint;
		if(fabs(fWingOffsetDist)<eps)//�п��ܲ��ɹ�
			logerr.Log("0X%X��˨���ݲ���",pBolt->handle);
	}
}
CBoltInfoDlg::CBoltInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBoltInfoDlg::IDD, pParent)
{
	m_bGroupSelect=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_boltInfoListCtrl.AddColumnHeader("Bolt Handle",95);
	m_boltInfoListCtrl.AddColumnHeader("G",40);
	m_boltInfoListCtrl.AddColumnHeader("Dependent Wing",110);
	m_boltInfoListCtrl.AddColumnHeader("Angle Handle",100);
#else
	m_boltInfoListCtrl.AddColumnHeader("��˨���",85);
	m_boltInfoListCtrl.AddColumnHeader("׼��",60);
	m_boltInfoListCtrl.AddColumnHeader("����֫",70);
	m_boltInfoListCtrl.AddColumnHeader("�Ǹ־��",85);
#endif 
}

CBoltInfoDlg::~CBoltInfoDlg()
{
}

void  CBoltInfoDlg::ActivateDialog()
{
	GetSelectBoltFrom3D();
	if(GetSafeHwnd()==NULL)
		Create(CBoltInfoDlg::IDD);
	m_boltInfoListCtrl.InitListCtrl(NULL);
	m_boltInfoListCtrl.m_bEnableSortItems=TRUE;
	m_boltInfoListCtrl.SetColumnSortType(2,SORTTYPE_STRING);
	m_boltInfoListCtrl.SetExtendedStyle(m_boltInfoListCtrl.GetExtendedStyle()&~LVS_EX_HEADERDRAGDROP);
	CRect rect;
	//�ƶ����ڵ�����λ��
	CWnd::GetWindowRect(rect);
	int width = rect.Width();
	int height=rect.Height();
	rect.left=950;
	rect.top=80;
	rect.right = rect.left+width;
	rect.bottom = rect.top+height;
	MoveWindow(rect, TRUE);
	ShowWindow(SW_SHOWNORMAL);
	SelectDefaultBolts();
	UpdateData(TRUE);
}
void CBoltInfoDlg::UpdateBoltInfoListCtrl(long hDefaultAngle,int iDefaultWing)
{
	g_pSolidDraw->ReleaseSnapStatus();
	m_boltInfoListCtrl.DeleteAllItems();
	ARRAY_LIST<long> id_arr;
	CStringArray str_arr;
	str_arr.SetSize(4);
	for (BOLTINFO* pBoltInfo=hashBoltInfoByHandle.GetFirst();pBoltInfo;pBoltInfo=hashBoltInfoByHandle.GetNext())
	{
		str_arr[0].Format("0X%X",pBoltInfo->pBolt->handle);
		str_arr[1].Format("%d",ftoi(pBoltInfo->fWingOffsetDist));
		if(pBoltInfo->iOffsetWing==0)
#ifdef  AFX_TARG_ENU_ENGLISH
			str_arr[2]="X Wing";
		else 
			str_arr[2]="Y Wing";
#else
			str_arr[2]="X ֫";
		else 
			str_arr[2]="Y ֫";
#endif
		str_arr[3].Format("0X%X",pBoltInfo->hAngle);
		int iItem=m_boltInfoListCtrl.InsertItemRecord(-1,str_arr); 
		m_boltInfoListCtrl.SetItemData(iItem,DWORD(pBoltInfo));
		if(pBoltInfo->hAngle==hDefaultAngle&&iDefaultWing==pBoltInfo->iOffsetWing)  //Ĭ��ѡ��
		{
			m_boltInfoListCtrl.SetSelectionMark(iItem);
			m_boltInfoListCtrl.SetItemState(iItem,LVIS_SELECTED|LVNI_FOCUSED,LVIS_SELECTED|LVNI_FOCUSED);
			id_arr.append(pBoltInfo->pBolt->handle);
		}
	}
	g_pSolidDraw->SetEntSnapStatus(id_arr.m_pData,id_arr.GetSize());
}

void CBoltInfoDlg::SelectBoltInfos()
{
	UpdateData();
	g_pSolidDraw->ReleaseSnapStatus();
	POSITION pos = m_boltInfoListCtrl.GetFirstSelectedItemPosition();
	ARRAY_LIST<long> id_arr;
	PARTSET xWinghave2RowBoltPartSet;
	PARTSET yWinghave2RowBoltPartSet;
	while (pos)
	{
		int iItem = m_boltInfoListCtrl.GetNextSelectedItem(pos);
		m_boltInfoListCtrl.SetSelectionMark(iItem);
		m_boltInfoListCtrl.SetItemState(iItem,LVIS_SELECTED|LVNI_FOCUSED,LVIS_SELECTED|LVNI_FOCUSED);
		BOLTINFO* pTempBoltInfo=(BOLTINFO*)m_boltInfoListCtrl.GetItemData(iItem);
		if(m_bGroupSelect)
		{   
			if(xWinghave2RowBoltPartSet.FromHandle(pTempBoltInfo->hAngle)==NULL&&pTempBoltInfo->iOffsetWing==0)
				xWinghave2RowBoltPartSet.append((CLDSPart*)Ta.FromHandle(pTempBoltInfo->hAngle));
			else if(yWinghave2RowBoltPartSet.FromHandle(pTempBoltInfo->hAngle)==NULL&&pTempBoltInfo->iOffsetWing==1)
				yWinghave2RowBoltPartSet.append((CLDSPart*)Ta.FromHandle(pTempBoltInfo->hAngle));
			for (int i=0;i<m_boltInfoListCtrl.GetItemCount();i++)
			{
				BOLTINFO* pBoltInfo=(BOLTINFO*)m_boltInfoListCtrl.GetItemData(i);
				if( xWinghave2RowBoltPartSet.FromHandle(pBoltInfo->hAngle)!=NULL&&pBoltInfo->iOffsetWing==0||
					yWinghave2RowBoltPartSet.FromHandle(pBoltInfo->hAngle)&&pBoltInfo->iOffsetWing==1)
				{
					m_boltInfoListCtrl.SetSelectionMark(i);
					m_boltInfoListCtrl.SetItemState(i,LVIS_SELECTED|LVNI_FOCUSED,LVIS_SELECTED|LVNI_FOCUSED);
					id_arr.append(pBoltInfo->pBolt->handle);
				}
			}
		}
		else
			id_arr.append(pTempBoltInfo->pBolt->handle);
	}
	g_pSolidDraw->SetEntSnapStatus(id_arr.m_pData,id_arr.GetSize());
}
void CBoltInfoDlg::SelectDefaultBolts()
{   //����˨���սǸ־������
	int nMostBolt=0;
	CHashListEx<ANGLEINFO> hashAllAngleInfo;
	for(BOLTINFO*pBoltInfo=hashBoltInfoByHandle.GetFirst();pBoltInfo;pBoltInfo=hashBoltInfoByHandle.GetNext())
		hashAllAngleInfo.Add(pBoltInfo->hAngle)->VerifyResideBoltInfo(pBoltInfo);
	long hDefaultAngle=0;
	int  iOffsetWing=0;
	//�ҳ�������˨�ĽǸֺ͹���֫
	for(ANGLEINFO* pAngleInfo=hashAllAngleInfo.GetFirst();pAngleInfo;pAngleInfo=hashAllAngleInfo.GetNext())
	{
		if(pAngleInfo->m_bDoubleRowX&&pAngleInfo->m_nBoltInX>nMostBolt)
		{
			nMostBolt=pAngleInfo->m_nBoltInX;
			hDefaultAngle=pAngleInfo->hAngle;
			iOffsetWing=0;
		}	
		if(pAngleInfo->m_bDoubleRowY&&pAngleInfo->m_nBoltInY>nMostBolt)
		{
			nMostBolt=pAngleInfo->m_nBoltInY;
			hDefaultAngle=pAngleInfo->hAngle;
			iOffsetWing=1;
		}
	}
	UpdateBoltInfoListCtrl(hDefaultAngle,iOffsetWing); 
}

void CBoltInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BOLT_INFO_LIST, (CWnd&)m_boltInfoListCtrl);
	DDX_Check(pDX, IDC_CHK_GROUP_SELECT, m_bGroupSelect);
}

BEGIN_MESSAGE_MAP(CBoltInfoDlg, CDialogEx)
	ON_COMMAND(IDC_BN_SWAP_G,&CBoltInfoDlg::OnBnSwapG)
	ON_NOTIFY(NM_CLICK, IDC_BOLT_INFO_LIST, OnClickBoltInfoList)
END_MESSAGE_MAP()

// CBoltInfoDlg ��Ϣ�������
void CBoltInfoDlg::OnClickBoltInfoList(NMHDR* pNMHDR, LRESULT* pResult)
{
	UpdateData();
	g_pSolidDraw->ReleaseSnapStatus();
	if(m_boltInfoListCtrl.GetSelectedCount()>1)
		SelectBoltInfos();              //��ѡʱ��
	else
	{   //��ѡʱ��
		ARRAY_LIST<long> id_arr;
		POSITION pos = m_boltInfoListCtrl.GetFirstSelectedItemPosition();
	    int iItem = m_boltInfoListCtrl.GetNextSelectedItem(pos);
		if(iItem<0)
			return;
		BOLTINFO*pBoltInfo=(BOLTINFO*)m_boltInfoListCtrl.GetItemData(iItem);
		if(!m_bGroupSelect)
			g_pSolidDraw->SetEntSnapStatus(pBoltInfo->pBolt->handle);
		else
		{
			for (int i=0;i<m_boltInfoListCtrl.GetItemCount();i++)
			{
				BOLTINFO*pTempBoltInfo=(BOLTINFO*)m_boltInfoListCtrl.GetItemData(i);
				if(pTempBoltInfo->hAngle==pBoltInfo->hAngle&&pTempBoltInfo->iOffsetWing==pBoltInfo->iOffsetWing)
				{
					m_boltInfoListCtrl.SetSelectionMark(i);
					m_boltInfoListCtrl.SetItemState(i,LVIS_SELECTED|LVNI_FOCUSED,LVIS_SELECTED|LVNI_FOCUSED);
					id_arr.append(pTempBoltInfo->pBolt->handle);
				}
			}
		}
		g_pSolidDraw->SetEntSnapStatus(id_arr.m_pData,id_arr.GetSize());
	}
}
CBoltInfoDlg::ANGLEINFO::ANGLEINFO()
{
	hAngle=0;
	m_bDoubleRowX=m_bDoubleRowY=false;
	m_nBoltInX=m_nBoltInY=m_siXG1=m_siYG1=0;
}
void CBoltInfoDlg::ANGLEINFO::VerifyResideBoltInfo(BOLTINFO* pBoltInfo)
{
	if(pBoltInfo->iOffsetWing==0)
	{
		if(m_nBoltInX==0)
			m_siXG1=(short)ftoi(pBoltInfo->fWingOffsetDist);
		else if(m_siXG1!=ftoi(pBoltInfo->fWingOffsetDist))
			m_bDoubleRowX=true;
		m_nBoltInX++;
	}
	else //if(pBoltInfo->iOffsetWing==1)
	{
		if(m_nBoltInY==0)
			m_siYG1=(short)ftoi(pBoltInfo->fWingOffsetDist);
		else if(m_siYG1!=ftoi(pBoltInfo->fWingOffsetDist))
			m_bDoubleRowX=true;
		m_nBoltInY++;
	}
}

BOOL UnifyAngleBoltParamG(DESIGN_LS_POS &designLsPos);	//defined in LDSView.cpp
void CBoltInfoDlg::OnBnSwapG()
{ 
	double g1=0,g2=0;
	BOLTSET boltInG1List;
	BOLTSET boltInG2List;
	ATOM_LIST<int>listItemIndicesOfG1;
	ATOM_LIST<int>listItemIndicesOfG2;
	if(m_boltInfoListCtrl.GetSelectedCount()<0)
	{
#ifdef AFX_TARG_ENU_ENGLISH 
		MessageBox("No bolts selected","Misoperation");
#else
		MessageBox("ûѡ����˨","�����");
#endif
		return;
	}
	if(m_boltInfoListCtrl.GetSelectedCount()<1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("Single bolt can not exchange bolt's G","Misoperation");
#else
		MessageBox("����˨û�������ľ���˨","�����");
#endif
		return;
	}
    BOOL  bFirst=TRUE,bDoubleG=FALSE;
	POSITION pos = m_boltInfoListCtrl.GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_boltInfoListCtrl.GetNextSelectedItem(pos);
		BOLTINFO* pBoltInfo=(BOLTINFO*)m_boltInfoListCtrl.GetItemData(nItem);
		if(bFirst)  //��һ��
		{
			bFirst=FALSE;
			g2=g1=pBoltInfo->fWingOffsetDist;
			boltInG1List.append(pBoltInfo->pBolt);
			listItemIndicesOfG1.append(nItem);
		}
		else
		{
			if(fabs(pBoltInfo->fWingOffsetDist-g1)>EPS&&!bDoubleG) //�ڶ���g
			{
				g2=pBoltInfo->fWingOffsetDist;
				boltInG2List.append(pBoltInfo->pBolt);
				listItemIndicesOfG2.append(nItem);
				bDoubleG=TRUE;
			}
			else if(fabs(pBoltInfo->fWingOffsetDist-g1)<EPS) 
			{
				boltInG1List.append(pBoltInfo->pBolt);
				listItemIndicesOfG1.append(nItem);
			}
			else if(fabs(pBoltInfo->fWingOffsetDist-g2)<EPS)
			{
				boltInG2List.append(pBoltInfo->pBolt);
				listItemIndicesOfG2.append(nItem);
			}
			else if(fabs(pBoltInfo->fWingOffsetDist-g2)+fabs(pBoltInfo->fWingOffsetDist-g1)>EPS)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox("Currently only supports two rows bolt swap","Misoperation");
#else
				MessageBox("��Ŀǰֻ֧�ֶ�����˨��","�����");
#endif
				 return ;
			}
		}
	}
	if(boltInG1List.GetNodeNum()==0||boltInG2List.GetNodeNum()==0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("Only one row of angle's bolt","Misoperation");
#else
		MessageBox("ѡ��Ǹ�ֻ��һ����˨","����");
#endif
		return;
	}
	//ˢ�½���
	CString str;
	str.Format("%d",ftoi(g2));
	for (int* pItem=listItemIndicesOfG1.GetFirst();pItem;pItem=listItemIndicesOfG1.GetNext())
	{
		BOLTINFO* pTempbolt=(BOLTINFO*)m_boltInfoListCtrl.GetItemData(*pItem);
		pTempbolt->pBolt->des_base_pos.wing_offset_dist-=(g1-g2);
		pTempbolt->fWingOffsetDist=g2;
		//�ȸ�Ϊ�Զ����ֹ��ָ���ľ����͵����л��ľ�ʧ�ܡ�wjh-2016.11.15
		pTempbolt->pBolt->des_base_pos.m_biWingOffsetStyle=4;
		UnifyAngleBoltParamG(pTempbolt->pBolt->des_base_pos);
		m_boltInfoListCtrl.SetItemData(*pItem,(DWORD)pTempbolt);
		m_boltInfoListCtrl.SetItemText(*pItem,1,str);
		pTempbolt->pBolt->correct_pos();
		pTempbolt->pBolt->SetModified();
		pTempbolt->pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pTempbolt->pBolt->GetSolidPartObject());
	}
	str.Format("%d",ftoi(g1));
	for (int* pItem=listItemIndicesOfG2.GetFirst();pItem;pItem=listItemIndicesOfG2.GetNext())
	{
		BOLTINFO* pTempbolt=(BOLTINFO*)m_boltInfoListCtrl.GetItemData(*pItem);
		pTempbolt->pBolt->des_base_pos.wing_offset_dist+=(g1-g2);
		pTempbolt->fWingOffsetDist=g1;
		//�ȸ�Ϊ�Զ����ֹ��ָ���ľ����͵����л��ľ�ʧ�ܡ�wjh-2016.11.15
		pTempbolt->pBolt->des_base_pos.m_biWingOffsetStyle=4;
		UnifyAngleBoltParamG(pTempbolt->pBolt->des_base_pos);
		m_boltInfoListCtrl.SetItemData(*pItem,(DWORD)pTempbolt);
		m_boltInfoListCtrl.SetItemText(*pItem,1,str);
		pTempbolt->pBolt->correct_pos();
		pTempbolt->pBolt->SetModified();
		pTempbolt->pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pTempbolt->pBolt->GetSolidPartObject());
		m_boltInfoListCtrl.SetItemText(*pItem,1,str);
	}
	g_pSolidDraw->Draw();
}
//��ģ̬�Ի��򲻵���OnClose��������ֻ������OnCancel���� wjh-2016.1.23
void CBoltInfoDlg::OnCancel()
{
	g_pSolidDraw->ReleaseSnapStatus();
	CDialogEx::OnCancel();
	DestroyWindow();
}

void CBoltInfoDlg::GetSelectBoltFrom3D()
{
	CLogErrorLife file;
	hashBoltInfoByHandle.Empty();
	CLDSBolt *pBolt=NULL;
	long *id_arr;
	int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	for(int i=0;i<n;i++)
	{
		pBolt=(CLDSBolt*)console.FromPartHandle(id_arr[i],CLS_BOLT);
		if(pBolt!=NULL)
			hashBoltInfoByHandle.SetValue((DWORD)pBolt->handle,BOLTINFO(pBolt));
	}
}
#endif