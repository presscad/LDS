//<LOCALE_TRANSLATE/>
// TableGridDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TableGridDlg.h"
#include "DefCard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTableGridDlg dialog


CTableGridDlg::CTableGridDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTableGridDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTableGridDlg)
	m_iContent = 0;
	m_iDataType = 0;
	m_fHighToWide = 0.65;
	m_fTextHigh = 3.0;
	m_sDisplayContent = _T("");
	m_sDisplayPrefix = _T("");
	m_sDisplaySuffix = _T("");
	m_iAlignType = 4;
	//}}AFX_DATA_INIT
	content_id=0;
}


void CTableGridDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTableGridDlg)
	DDX_CBIndex(pDX, IDC_CMB_DATA_CONTENT, m_iContent);
	DDX_CBIndex(pDX, IDC_CMB_DATA_TYPE, m_iDataType);
	//DDX_Text(pDX, IDC_E_RATIO_HTOW, m_fHighToWide);
	DDX_Text(pDX, IDC_E_TEXT_HIGH, m_fTextHigh);
	DDX_Text(pDX, IDC_E_DISPLAY_CONTENT, m_sDisplayContent);
	DDX_Text(pDX, IDC_E_DISPLAY_PREFIX, m_sDisplayPrefix);
	DDX_Text(pDX, IDC_E_DISPLAY_SUFFIX, m_sDisplaySuffix);
	DDX_Radio(pDX, IDC_RDO_ALIGN_TYPE, m_iAlignType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTableGridDlg, CDialog)
	//{{AFX_MSG_MAP(CTableGridDlg)
	ON_CBN_SELCHANGE(IDC_CMB_DATA_CONTENT, OnSelchangeCmbDataContent)
	ON_CBN_SELCHANGE(IDC_CMB_DATA_TYPE, OnSelchangeCmbDataType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTableGridDlg message handlers

void CTableGridDlg::OnSelchangeCmbDataContent() 
{
	CString sText;
	UpdateData();
	GetDlgItem(IDC_CMB_DATA_CONTENT)->GetWindowText(sText);
	GetDlgItem(IDC_E_DISPLAY_CONTENT)->SetWindowText(sText);
}

void CTableGridDlg::OnSelchangeCmbDataType() 
{
	CString sText;
	UpdateData();
	if(m_iDataType<2)	//�����ı���ʵʱ�ı�
		GetDlgItem(IDC_E_DISPLAY_CONTENT)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_E_DISPLAY_CONTENT)->EnableWindow(FALSE);
	if(m_iDataType==3)	//��ͼ����
		GetDlgItem(IDC_CMB_DATA_CONTENT)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_CMB_DATA_CONTENT)->EnableWindow(TRUE);
}

BOOL CTableGridDlg::OnInitDialog() 
{
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_DATA_CONTENT);
#ifdef AFX_TARG_ENU_ENGLISH
	pCMB->SetItemData(pCMB->AddString("code name"),ITEM_TYPE_CODE_NO);
	pCMB->SetItemData(pCMB->AddString("tower type"),ITEM_TYPE_TA_TYPE);
	pCMB->SetItemData(pCMB->AddString("length"),ITEM_TYPE_LENGTH);
	pCMB->SetItemData(pCMB->AddString("part label"),ITEM_TYPE_PART_NO);
	pCMB->SetItemData(pCMB->AddString("map number"),ITEM_TYPE_MAP_NO);
	pCMB->SetItemData(pCMB->AddString("tower number"),ITEM_TYPE_TA_NUM);
	pCMB->SetItemData(pCMB->AddString("cut root"),ITEM_TYPE_CUT_ROOT);
	pCMB->SetItemData(pCMB->AddString("cut back"),ITEM_TYPE_CUT_BER);
	pCMB->SetItemData(pCMB->AddString("cut edge"),ITEM_TYPE_CUT_EDGE);
	pCMB->SetItemData(pCMB->AddString("rolled edge"),ITEM_TYPE_WRAP_EDGE);
	pCMB->SetItemData(pCMB->AddString("weld"),ITEM_TYPE_WELD);
	pCMB->SetItemData(pCMB->AddString("leg open"),ITEM_TYPE_KAIJIAO);
	pCMB->SetItemData(pCMB->AddString("leg close"),ITEM_TYPE_HEJIAO);
	pCMB->SetItemData(pCMB->AddString("wing angle"),ITEM_TYPE_WING_ANGLE);
	pCMB->SetItemData(pCMB->AddString("single number"),ITEM_TYPE_PART_NUM);
	pCMB->SetItemData(pCMB->AddString("piece weight"),ITEM_TYPE_PIECE_WEIGHT);
	pCMB->SetItemData(pCMB->AddString("process number"),ITEM_TYPE_SUM_PART_NUM);
	pCMB->SetItemData(pCMB->AddString("total weight"),ITEM_TYPE_SUM_WEIGHT);
	pCMB->SetItemData(pCMB->AddString("substitute material"),ITEM_TYPE_REPL_MAT);
	pCMB->SetItemData(pCMB->AddString("design material"),ITEM_TYPE_DES_MAT);
	pCMB->SetItemData(pCMB->AddString("substitute specifications"),ITEM_TYPE_REPL_GUIGE);
	pCMB->SetItemData(pCMB->AddString("design specifications"),ITEM_TYPE_DES_GUIGE);
	pCMB->SetItemData(pCMB->AddString("Project Name"),ITEM_TYPE_PRJ_NAME);
	pCMB->SetItemData(pCMB->AddString("first bend"),ITEM_TYPE_HUOQU_FST);
	pCMB->SetItemData(pCMB->AddString("secondary bend"),ITEM_TYPE_HUOQU_SEC);
	pCMB->SetItemData(pCMB->AddString("scale"),ITEM_TYPE_MAP_SCALE);
	pCMB->SetItemData(pCMB->AddString("cut arc"),ITEM_TYPE_CUT_ARC);
	pCMB->SetItemData(pCMB->AddString("fillet"),ITEM_TYPE_FILLET);
	pCMB->SetItemData(pCMB->AddString("foot nail's base material"),ITEM_TYPE_FOOTNAIL_PLATE);

	pCMB->SetItemData(pCMB->AddString("13.5 hole number"),ITEM_TYPE_LSM12_NUM);
	pCMB->SetItemData(pCMB->AddString("17.5 hole number"),ITEM_TYPE_LSM16_NUM);
	pCMB->SetItemData(pCMB->AddString("19.5 hole number"),ITEM_TYPE_LSM18_NUM);
	pCMB->SetItemData(pCMB->AddString("21.5 hole number"),ITEM_TYPE_LSM20_NUM);
	pCMB->SetItemData(pCMB->AddString("23.5 hole number"),ITEM_TYPE_LSM22_NUM);
	pCMB->SetItemData(pCMB->AddString("25.5 hole number"),ITEM_TYPE_LSM24_NUM);
	pCMB->SetItemData(pCMB->AddString("other hole number"),ITEM_TYPE_LSMX_NUM);
	pCMB->SetItemData(pCMB->AddString("total hole number"),ITEM_TYPE_LSSUM_NUM);
	pCMB->SetItemData(pCMB->AddString("table maker"),ITEM_TYPE_TAB_MAKER);
	pCMB->SetItemData(pCMB->AddString("critic"),ITEM_TYPE_CRITIC);
	pCMB->SetItemData(pCMB->AddString("auditor"),ITEM_TYPE_AUDITOR);
	pCMB->SetItemData(pCMB->AddString("note"),ITEM_TYPE_PART_NOTES);
	pCMB->SetItemData(pCMB->AddString("once for all"),ITEM_TYPE_ONCE_OVER);				//һ�����
	pCMB->SetItemData(pCMB->AddString("gas cut"),ITEM_TYPE_GAS_CUTTING);						//����
	pCMB->SetItemData(pCMB->AddString("punching shear"),ITEM_TYPE_PUNCH_SHEAR);			//���
	pCMB->SetItemData(pCMB->AddString("drill hole"),ITEM_TYPE_ZUAN_KONG);				//���
	pCMB->SetItemData(pCMB->AddString("punching hole"),ITEM_TYPE_CHONG_KONG);			//���
	pCMB->SetItemData(pCMB->AddString("cut angle"),ITEM_TYPE_CUT_ANGLE);				//cut angle
	pCMB->SetItemData(pCMB->AddString("starting X leg cut angle"),ITEM_TYPE_CUT_ANGLE_S_X);	//startingXcut angle/leg
	pCMB->SetItemData(pCMB->AddString("starting Y leg cut angle"),ITEM_TYPE_CUT_ANGLE_S_Y);	//startingXcut angle/leg
	pCMB->SetItemData(pCMB->AddString("ending X leg cut angle"),ITEM_TYPE_CUT_ANGLE_E_X);	//endingXcut angle/leg
	pCMB->SetItemData(pCMB->AddString("ending Y leg cut angle"),ITEM_TYPE_CUT_ANGLE_E_Y);	//engingYcut angle/leg
	//add starting and ending push flat wht 10-11-03
	pCMB->SetItemData(pCMB->AddString("push flat"),ITEM_TYPE_PUSH_FLAT);				//push flat
	pCMB->SetItemData(pCMB->AddString("starting X leg push flat"),ITEM_TYPE_PUSH_FLAT_S_X);	//startingXleg push flat
	pCMB->SetItemData(pCMB->AddString("starting X leg push flat"),ITEM_TYPE_PUSH_FLAT_S_Y);	//startingYleg push flat
	pCMB->SetItemData(pCMB->AddString("ending X leg push flat"),ITEM_TYPE_PUSH_FLAT_E_X);	//endingXleg push flat
	pCMB->SetItemData(pCMB->AddString("ending Y leg push flat"),ITEM_TYPE_PUSH_FLAT_E_Y);	//endingYleg push flat
	pCMB->SetItemData(pCMB->AddString("middle X leg push flat"),ITEM_TYPE_PUSH_FLAT_M_X);	
	pCMB->SetItemData(pCMB->AddString("middle Y leg push flat"),ITEM_TYPE_PUSH_FLAT_M_Y);	

	pCMB->SetItemData(pCMB->AddString("13.5 hole symbol"),1001);
	pCMB->SetItemData(pCMB->AddString("17.5 hole symbol"),1002);
	pCMB->SetItemData(pCMB->AddString("19.5 hole symbol"),1005);
	pCMB->SetItemData(pCMB->AddString("21.5 hole symbol"),1003);
	pCMB->SetItemData(pCMB->AddString("23.5 hole symbol"),1006);
	pCMB->SetItemData(pCMB->AddString("25.5 hole symbol"),1004);
	pCMB->SetItemData(pCMB->AddString("date"),ITEM_TYPE_DATE);
	pCMB->SetItemData(pCMB->AddString("cut back icon"),ITEM_TYPE_CUTBERICON);
	pCMB->SetItemData(pCMB->AddString("cut root icon"),ITEM_TYPE_CUTROOTICON);
	pCMB->SetItemData(pCMB->AddString("bend icon"),ITEM_TYPE_BENDICON);
	pCMB->SetItemData(pCMB->AddString("weld icon"),ITEM_TYPE_WELDICON);
	pCMB->SetItemData(pCMB->AddString("open-close icon"),ITEM_TYPE_KAIHE_ANGLE_ICON);
	pCMB->SetItemData(pCMB->AddString("schematic map"),ITEM_TYPE_SKETCH_MAP);
	pCMB->SetItemData(pCMB->AddString("process name"),ITEM_TYPE_PROCESS_NAME);
	pCMB->SetItemData(pCMB->AddString("total number"),ITEM_TYPE_PAGENUM);
	pCMB->SetItemData(pCMB->AddString("No."),ITEM_TYPE_PAGEINDEX);
#else
	pCMB->SetItemData(pCMB->AddString("����"),ITEM_TYPE_CODE_NO);
	pCMB->SetItemData(pCMB->AddString("����"),ITEM_TYPE_TA_TYPE);
	pCMB->SetItemData(pCMB->AddString("����"),ITEM_TYPE_LENGTH);
	pCMB->SetItemData(pCMB->AddString("����"),ITEM_TYPE_PART_NO);
	pCMB->SetItemData(pCMB->AddString("ͼ��"),ITEM_TYPE_MAP_NO);
	pCMB->SetItemData(pCMB->AddString("����"),ITEM_TYPE_TA_NUM);
	pCMB->SetItemData(pCMB->AddString("�ٸ�"),ITEM_TYPE_CUT_ROOT);
	pCMB->SetItemData(pCMB->AddString("����"),ITEM_TYPE_CUT_BER);
	pCMB->SetItemData(pCMB->AddString("����"),ITEM_TYPE_CUT_EDGE);
	pCMB->SetItemData(pCMB->AddString("���"),ITEM_TYPE_WRAP_EDGE);
	pCMB->SetItemData(pCMB->AddString("����"),ITEM_TYPE_WELD);
	pCMB->SetItemData(pCMB->AddString("����"),ITEM_TYPE_KAIJIAO);
	pCMB->SetItemData(pCMB->AddString("�Ͻ�"),ITEM_TYPE_HEJIAO);
	pCMB->SetItemData(pCMB->AddString("��֫�н�"),ITEM_TYPE_WING_ANGLE);
	pCMB->SetItemData(pCMB->AddString("������"),ITEM_TYPE_PART_NUM);
	pCMB->SetItemData(pCMB->AddString("����"),ITEM_TYPE_PIECE_WEIGHT);
	pCMB->SetItemData(pCMB->AddString("�ӹ���"),ITEM_TYPE_SUM_PART_NUM);
	pCMB->SetItemData(pCMB->AddString("����"),ITEM_TYPE_SUM_WEIGHT);
	pCMB->SetItemData(pCMB->AddString("���ò���"),ITEM_TYPE_REPL_MAT);
	pCMB->SetItemData(pCMB->AddString("��Ʋ���"),ITEM_TYPE_DES_MAT);
	pCMB->SetItemData(pCMB->AddString("���ù��"),ITEM_TYPE_REPL_GUIGE);
	pCMB->SetItemData(pCMB->AddString("��ƹ��"),ITEM_TYPE_DES_GUIGE);
	pCMB->SetItemData(pCMB->AddString("��������"),ITEM_TYPE_PRJ_NAME);
	pCMB->SetItemData(pCMB->AddString("�״λ���"),ITEM_TYPE_HUOQU_FST);
	pCMB->SetItemData(pCMB->AddString("���λ���"),ITEM_TYPE_HUOQU_SEC);
	pCMB->SetItemData(pCMB->AddString("����"),ITEM_TYPE_MAP_SCALE);
	pCMB->SetItemData(pCMB->AddString("�л�"),ITEM_TYPE_CUT_ARC);
	pCMB->SetItemData(pCMB->AddString("�¿�"),ITEM_TYPE_FILLET);
	pCMB->SetItemData(pCMB->AddString("�Ŷ��������ϱ�"),ITEM_TYPE_FOOTNAIL_PLATE);

	pCMB->SetItemData(pCMB->AddString("13.5����"),ITEM_TYPE_LSM12_NUM);
	pCMB->SetItemData(pCMB->AddString("17.5����"),ITEM_TYPE_LSM16_NUM);
	pCMB->SetItemData(pCMB->AddString("19.5����"),ITEM_TYPE_LSM18_NUM);
	pCMB->SetItemData(pCMB->AddString("21.5����"),ITEM_TYPE_LSM20_NUM);
	pCMB->SetItemData(pCMB->AddString("23.5����"),ITEM_TYPE_LSM22_NUM);
	pCMB->SetItemData(pCMB->AddString("25.5����"),ITEM_TYPE_LSM24_NUM);
	pCMB->SetItemData(pCMB->AddString("��������"),ITEM_TYPE_LSMX_NUM);
	pCMB->SetItemData(pCMB->AddString("�ܿ���"),ITEM_TYPE_LSSUM_NUM);
	pCMB->SetItemData(pCMB->AddString("�Ʊ���"),ITEM_TYPE_TAB_MAKER);
	pCMB->SetItemData(pCMB->AddString("������"),ITEM_TYPE_CRITIC);
	pCMB->SetItemData(pCMB->AddString("�����"),ITEM_TYPE_AUDITOR);
	pCMB->SetItemData(pCMB->AddString("��ע"),ITEM_TYPE_PART_NOTES);
	pCMB->SetItemData(pCMB->AddString("һ�����"),ITEM_TYPE_ONCE_OVER);			//һ�����
	pCMB->SetItemData(pCMB->AddString("����"),ITEM_TYPE_GAS_CUTTING);			//����
	pCMB->SetItemData(pCMB->AddString("���"),ITEM_TYPE_PUNCH_SHEAR);			//���
	pCMB->SetItemData(pCMB->AddString("���"),ITEM_TYPE_ZUAN_KONG);				//���
	pCMB->SetItemData(pCMB->AddString("���"),ITEM_TYPE_CHONG_KONG);			//���
	pCMB->SetItemData(pCMB->AddString("�н�"),ITEM_TYPE_CUT_ANGLE);				//�н�
	pCMB->SetItemData(pCMB->AddString("ʼ��X֫�н�"),ITEM_TYPE_CUT_ANGLE_S_X);	//ʼ��X�н�/֫
	pCMB->SetItemData(pCMB->AddString("ʼ��Y֫�н�"),ITEM_TYPE_CUT_ANGLE_S_Y);	//ʼ��Y�н�/֫
	pCMB->SetItemData(pCMB->AddString("ĩ��X֫�н�"),ITEM_TYPE_CUT_ANGLE_E_X);	//�ն�X�н�/֫
	pCMB->SetItemData(pCMB->AddString("ĩ��Y֫�н�"),ITEM_TYPE_CUT_ANGLE_E_Y);	//�ն�Y�н�/֫
	//���ʼ�ն�ѹ�� wht 10-11-03
	pCMB->SetItemData(pCMB->AddString("ѹ��"),ITEM_TYPE_PUSH_FLAT);				//ѹ��
	pCMB->SetItemData(pCMB->AddString("ʼ��X֫ѹ��"),ITEM_TYPE_PUSH_FLAT_S_X);	//ʼ��X֫ѹ��
	pCMB->SetItemData(pCMB->AddString("ʼ��Y֫ѹ��"),ITEM_TYPE_PUSH_FLAT_S_Y);	//ʼ��Y֫ѹ��
	pCMB->SetItemData(pCMB->AddString("ĩ��X֫ѹ��"),ITEM_TYPE_PUSH_FLAT_E_X);	//�ն�X֫ѹ��
	pCMB->SetItemData(pCMB->AddString("ĩ��Y֫ѹ��"),ITEM_TYPE_PUSH_FLAT_E_Y);	//�ն�Y֫ѹ��
	pCMB->SetItemData(pCMB->AddString("�м�X֫ѹ��"),ITEM_TYPE_PUSH_FLAT_M_X);	//�м�X֫ѹ��
	pCMB->SetItemData(pCMB->AddString("�м�Y֫ѹ��"),ITEM_TYPE_PUSH_FLAT_M_Y);	//�м�Y֫ѹ��

	pCMB->SetItemData(pCMB->AddString("13.5��ͼ��"),1001);
	pCMB->SetItemData(pCMB->AddString("17.5��ͼ��"),1002);
	pCMB->SetItemData(pCMB->AddString("19.5��ͼ��"),1005);
	pCMB->SetItemData(pCMB->AddString("21.5��ͼ��"),1003);
	pCMB->SetItemData(pCMB->AddString("23.5��ͼ��"),1006);
	pCMB->SetItemData(pCMB->AddString("25.5��ͼ��"),1004);
	pCMB->SetItemData(pCMB->AddString("����"),ITEM_TYPE_DATE);
	pCMB->SetItemData(pCMB->AddString("����ͼ��"),ITEM_TYPE_CUTBERICON);
	pCMB->SetItemData(pCMB->AddString("�ٸ�ͼ��"),ITEM_TYPE_CUTROOTICON);
	pCMB->SetItemData(pCMB->AddString("����ͼ��"),ITEM_TYPE_BENDICON);
	pCMB->SetItemData(pCMB->AddString("����ͼ��"),ITEM_TYPE_WELDICON);
	pCMB->SetItemData(pCMB->AddString("���Ͻ�ͼ��"),ITEM_TYPE_KAIHE_ANGLE_ICON);
	pCMB->SetItemData(pCMB->AddString("ʾ��ͼ"),ITEM_TYPE_SKETCH_MAP);
	pCMB->SetItemData(pCMB->AddString("��������"),ITEM_TYPE_PROCESS_NAME);
	pCMB->SetItemData(pCMB->AddString("��   ҳ"),ITEM_TYPE_PAGENUM);
	pCMB->SetItemData(pCMB->AddString("��   ҳ"),ITEM_TYPE_PAGEINDEX);
#endif
	SetCMBData(content_id);

	CDialog::OnInitDialog();
	if(m_iDataType<2)	//�����ı���ʵʱ�ı�
		GetDlgItem(IDC_E_DISPLAY_CONTENT)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_E_DISPLAY_CONTENT)->EnableWindow(FALSE);
	if(m_iDataType==3)	//��ͼ����
		GetDlgItem(IDC_CMB_DATA_CONTENT)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_CMB_DATA_CONTENT)->EnableWindow(TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTableGridDlg::SetCMBData(DWORD content)
{
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_DATA_CONTENT);
	for(int i=0;i<pCMB->GetCount();i++)
	{
		if(pCMB->GetItemData(i)==content)
		{
			m_iContent = i;
			return TRUE;
		}
	}
	return FALSE;
}
int CTableGridDlg::GetCMBData(int index)
{
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_DATA_CONTENT);
	content_id = pCMB->GetItemData(index);
	return content_id;
}
	

void CTableGridDlg::OnOK() 
{
	GetCMBData(m_iContent);
	
	CDialog::OnOK();
}
