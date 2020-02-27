// SelCirPlateTypePage.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "CirPlateTypePage.h"

// CCirPlateTypePage 对话框
#ifdef __COMPLEX_PART_INC_

IMPLEMENT_DYNAMIC(CCirPlateTypePage, CDialogEx)

CCirPlateTypePage::CCirPlateTypePage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCirPlateTypePage::IDD, pParent)
{
	m_indexCurrSelPlateType=0;
}

CCirPlateTypePage::~CCirPlateTypePage()
{
}
#include "DesignCircularPlateDlg.h"
int CCirPlateTypePage::set_idCirPlateType(int idPlateType)
{
	if(idPlateType==CDesignCircularPlateDlg::TYPE_COVERPLATE2)
		return m_indexCurrSelPlateType=0;
	else if(idPlateType==CDesignCircularPlateDlg::TYPE_COVERPLATE3)
		return m_indexCurrSelPlateType=1;
	else if(idPlateType==CDesignCircularPlateDlg::TYPE_COVERPLATE4)
		return m_indexCurrSelPlateType=2;
	else if(idPlateType==CDesignCircularPlateDlg::TYPE_3WL_2)
		return m_indexCurrSelPlateType=3;
	else if(idPlateType==CDesignCircularPlateDlg::TYPE_3WA_1)
		return m_indexCurrSelPlateType=4;
	else if(idPlateType==CDesignCircularPlateDlg::TYPE_3WA_2)
		return m_indexCurrSelPlateType=5;
	else if(idPlateType==CDesignCircularPlateDlg::TYPE_2WA)
		return m_indexCurrSelPlateType=6;
	else if(idPlateType==CDesignCircularPlateDlg::TYPE_2WL_1)
		return m_indexCurrSelPlateType=7;
	else if(idPlateType==CDesignCircularPlateDlg::TYPE_2WL_2)
		return m_indexCurrSelPlateType=8;
	else if(idPlateType==CDesignCircularPlateDlg::TYPE_COMMON_PLATE)
		return m_indexCurrSelPlateType=9;
	else if(idPlateType==CDesignCircularPlateDlg::TYPE_SHIELD_WATER)
		return m_indexCurrSelPlateType=10;
	else if(idPlateType==CDesignCircularPlateDlg::TYPE_CIR_BASE)
		return m_indexCurrSelPlateType=11;
	else if(idPlateType==CDesignCircularPlateDlg::TYPE_SPECIAL_FL)
		return m_indexCurrSelPlateType=12;
	return 0;	//
}
int CCirPlateTypePage::get_idCirPlateType()
{
	if(m_indexCurrSelPlateType==0)
		return CDesignCircularPlateDlg::TYPE_COVERPLATE2;
	else if(m_indexCurrSelPlateType==1)
		return CDesignCircularPlateDlg::TYPE_COVERPLATE3;
	else if(m_indexCurrSelPlateType==2)
		return CDesignCircularPlateDlg::TYPE_COVERPLATE4;
	else if(m_indexCurrSelPlateType==3)
		return CDesignCircularPlateDlg::TYPE_3WL_2;
	else if(m_indexCurrSelPlateType==4)
		return CDesignCircularPlateDlg::TYPE_3WA_1;
	else if(m_indexCurrSelPlateType==5)
		return CDesignCircularPlateDlg::TYPE_3WA_2;
	else if(m_indexCurrSelPlateType==6)
		return CDesignCircularPlateDlg::TYPE_2WA;
	else if(m_indexCurrSelPlateType==7)
		return CDesignCircularPlateDlg::TYPE_2WL_1;
	else if(m_indexCurrSelPlateType==8)
		return CDesignCircularPlateDlg::TYPE_2WL_2;
	else if(m_indexCurrSelPlateType==9)
		return CDesignCircularPlateDlg::TYPE_COMMON_PLATE;
	else if(m_indexCurrSelPlateType==10)
		return CDesignCircularPlateDlg::TYPE_SHIELD_WATER;
	else if(m_indexCurrSelPlateType==11)
		return CDesignCircularPlateDlg::TYPE_CIR_BASE;
	else if(m_indexCurrSelPlateType==12)
		return CDesignCircularPlateDlg::TYPE_SPECIAL_FL;
	else
		return 0;
}

void CCirPlateTypePage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCirPlateTypePage, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST_TYPE_IMAGE, &CCirPlateTypePage::OnClickListTypeImage)
END_MESSAGE_MAP()


// CCirPlateTypePage 消息处理程序


BOOL CCirPlateTypePage::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
	UINT m_uIconWidth=40,m_uIconHeight=40;
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_TYPE_IMAGE);
	m_pListCtrl->SetExtendedStyle( LVS_ICON);// | LVS_EX_REGIONAL );
	m_pListCtrl->SetBkColor(RGB(255,255,255));
	m_pListCtrl->SetIconSpacing(m_uIconWidth+5,m_uIconWidth+25);
	imageList.DeleteImageList();
	imageList.Create(m_uIconWidth, m_uIconHeight, ILC_COLOR4,   2, 4);
	m_pListCtrl->SetImageList(&imageList,LVSIL_NORMAL);
	UINT i,uidResImages[13];
	uidResImages[0]= IDB_BMP_CIRCULAR_PLATE_TYPE1;	//TYPE_COVERPLATE2
	uidResImages[1]= IDB_BMP_CIRCULAR_PLATE_TYPE5;	//TYPE_COVERPLATE3
	uidResImages[2]= IDB_BMP_CIRCULAR_PLATE_TYPE13;	//TYPE_COVERPLATE4
	uidResImages[3]= IDB_BMP_CIRCULAR_PLATE_TYPE2;	//TYPE_3WL_2
	uidResImages[4]= IDB_BMP_CIRCULAR_PLATE_TYPE4;	//TYPE_3WA_1
	uidResImages[5]= IDB_BMP_CIRCULAR_PLATE_TYPE3;	//TYPE_3WA_2
	uidResImages[6]= IDB_BMP_CIRCULAR_PLATE_TYPE6;	//TYPE_2WA
	uidResImages[7]= IDB_BMP_CIRCULAR_PLATE_TYPE7;	//TYPE_2WL_1
	uidResImages[8]= IDB_BMP_CIRCULAR_PLATE_TYPE8;	//TYPE_2WL_2
	uidResImages[9]= IDB_BMP_CIRCULAR_PLATE_TYPE9;	//TYPE_COMMON_PLATE
	uidResImages[10]=IDB_BMP_CIRCULAR_PLATE_TYPE10;	//TYPE_SHIELD_WATER
	uidResImages[11]=IDB_BMP_CIRCULAR_PLATE_TYPE11;	//TYPE_CIR_BASE
	uidResImages[12]=IDB_BMP_CIRCULAR_PLATE_TYPE12;	//TYPE_SPECIAL_FL
	for(i=0;i<13;i++)
	{
		CBitmap bitmap;
		bitmap.LoadBitmap(uidResImages[i]);
		int index=imageList.Add(&bitmap,RGB(0,0,0));
		CXhChar16 label("TYP%d",i+1);
		m_pListCtrl->InsertItem(m_pListCtrl->GetItemCount(),label,index);
	}
	if(m_indexCurrSelPlateType>=0&&m_indexCurrSelPlateType<m_pListCtrl->GetItemCount())
	{	//设置上一次选中项
		m_pListCtrl->SetSelectionMark(m_indexCurrSelPlateType);
		m_pListCtrl->SetItemState(m_indexCurrSelPlateType,LVIS_SELECTED|LVNI_FOCUSED,LVIS_SELECTED|LVNI_FOCUSED);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CCirPlateTypePage::OnOK()
{
	POSITION pos=m_pListCtrl->GetFirstSelectedItemPosition();
	m_indexCurrSelPlateType=(pos!=NULL)?m_pListCtrl->GetNextSelectedItem(pos):-1;

	CDialogEx::OnOK();
}


void CCirPlateTypePage::OnClickListTypeImage(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	//*pResult = 0;
	POSITION pos=m_pListCtrl->GetFirstSelectedItemPosition();
	if((m_indexCurrSelPlateType=(pos!=NULL)?m_pListCtrl->GetNextSelectedItem(pos):-1)>=0)
		CDialogEx::OnOK();
}
#endif