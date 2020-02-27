//<LOCALE_TRANSLATE Confirm by hxr/>
// NewSubstationDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "BlockTempl.h"
#include "NewSubstationDlg.h"
#include "afxdialogex.h"


// CNewSubstationDlg �Ի���

IMPLEMENT_DYNAMIC(CNewSubstationDlg, CDialogEx)

CNewSubstationDlg::CNewSubstationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNewSubstationDlg::IDD, pParent)
	, m_iActivePage(0)
{
	m_pModel=NULL;
	m_bWizardDialog=FALSE;
	m_bDesignBeam=TRUE;
	m_bDesignPost=TRUE;
}

CNewSubstationDlg::~CNewSubstationDlg()
{
}

void CNewSubstationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LABEL_GENERAL, m_labelGeneral);
	DDX_Control(pDX, IDC_LABEL_POST, m_labelPost);
	DDX_Control(pDX, IDC_LABEL_BEAM, m_labelBeam);
	DDX_Control(pDX, IDC_LABEL_SEGMENT,m_labelSegment);
}


BEGIN_MESSAGE_MAP(CNewSubstationDlg, CDialogEx)
	ON_STN_CLICKED(IDC_LABEL_POST, &CNewSubstationDlg::OnClickedLabelPost)
	ON_STN_CLICKED(IDC_LABEL_BEAM, &CNewSubstationDlg::OnClickedLabelBeam)
	ON_STN_CLICKED(IDC_LABEL_GENERAL, &CNewSubstationDlg::OnClickedLabelGeneral)
	ON_STN_CLICKED(IDC_LABEL_SEGMENT, &CNewSubstationDlg::OnClickedLabelSegment)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CNewSubstationDlg ��Ϣ�������


BOOL CNewSubstationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	pageGeneral.m_bDesignPost=m_bDesignPost;
	pageGeneral.m_bDesignBeam=m_bDesignBeam;
	m_labelGeneral.SetBackgroundColour(RGB(128,128,128));
	m_labelPost.SetBackgroundColour(RGB(128,128,128));
	m_labelBeam.SetBackgroundColour(RGB(128,128,128));
	m_labelSegment.SetBackgroundColour(RGB(128,128,128));
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CNewSubstationDlg::OnClickedLabelGeneral()
{ 
	pageGeneral.DoModal();
	m_iActivePage=0;
}

void CNewSubstationDlg::OnClickedLabelPost()
{
	pagePost.DoModal();
	m_iActivePage=1;
}

void CNewSubstationDlg::OnClickedLabelBeam()
{
	if(pageGeneral.m_iBeamType==0)
		pageBeam.DoModal();
	else //if(pageGeneral.m_iBeamType==1)
		pageSquareBeam.DoModal();
	m_iActivePage=2;
}

void CNewSubstationDlg::OnClickedLabelSegment()
{
	/*double fSumLen=0;
	fSumLen=pageGeneral.m_fSpanLen;
	pageSegment.m_fLCBeamLen=pageBeam.m_fLeftBeamLength;
	pageSegment.m_fRCBeamLen=pageBeam.m_fRightBeamLength;
	pageSegment.m_fSBeamLen=fSumLen-(pageSegment.m_fLCBeamLen+pageSegment.m_fRCBeamLen);*/
 	pageSegment.DoModal();
	m_iActivePage=3;
}
BOOL CNewSubstationDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(wParam==IDC_LABEL_GENERAL && message==WM_XHYPERLINK_CLICKED)
		OnClickedLabelGeneral();
	else if(wParam==IDC_LABEL_POST && message==WM_XHYPERLINK_CLICKED)
		OnClickedLabelPost();
	else if(wParam==IDC_LABEL_BEAM && message==WM_XHYPERLINK_CLICKED)
		OnClickedLabelBeam();
	else if(wParam==IDC_LABEL_SEGMENT && message==WM_XHYPERLINK_CLICKED)
		OnClickedLabelSegment();
	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
}

void CNewSubstationDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	/*CWnd* pActPage=NULL;
	if(m_iActivePage==0)
		pActPage=&m_labelGeneral;
	else if(m_iActivePage==1)
		pActPage=&m_labelPost;
	else if(m_iActivePage==2)
		pActPage=&m_labelBeam;
	if(pActPage)
	{
		//if (m_Brush.GetSafeHandle())
		//	m_Brush.DeleteObject();
		//m_Brush.CreateSolidBrush(RGB(128,128,128));
		//dc.SelectObject(&m_Brush);
		CRect rect;
		pActPage->GetWindowRect(&rect);
		ScreenToClient(rect);
		//dc.SetBkMode(OBLIQUE);
		dc.FillSolidRect(rect,RGB(128,128,128));
	}*/
	// ��Ϊ��ͼ��Ϣ���� CDialogEx::OnPaint()
}
void CNewSubstationDlg::OnOK()
{
	UpdateData(TRUE);
	if(m_pModel)
	{
		m_pModel->listParams.Empty();
		FillSummarizeParam();
		FillPostParam();
		FillBeamParam();
	}
	m_bDesignPost=pageGeneral.m_bDesignPost;
	m_bDesignBeam=pageGeneral.m_bDesignBeam;
	CDialog::OnOK();
}

//��Ӹ�������
BOOL CNewSubstationDlg::FillSummarizeParam()
{
	DESIGN_PARAM_ITEM* pParam=NULL;
	if(m_pModel)
	{	//����
		pParam=m_pModel->listParams.Add(KEY4C("NUM"));
		pParam->key=KEY4C("NUM");
		pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
		pParam->value.fValue=pageGeneral.m_nSpanNum;
		//���	�˴�Ϊ��ʼ��ֵ���������ʱ���¼���
		pParam=m_pModel->listParams.Add(KEY4C("LEN"));	
		pParam->key=KEY4C("LEN");
		pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
		pParam->value.fValue=24000;
		//�߶�
		pParam=m_pModel->listParams.Add(KEY4C("H"));
		pParam->key=KEY4C("H");
		pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
		pParam->value.fValue=pageGeneral.m_fBeamHeight;
		if(pageGeneral.m_bDesignPost)
		{
			pParam=m_pModel->listParams.Add(KEY4C("PTYP"));	//���������� 0.���ּ�
			pParam->key=KEY4C("PTYP");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=pageGeneral.m_iPostType;
			pParam=m_pModel->listParams.Add(KEY4C("PH"));
			pParam->key=KEY4C("PH");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageGeneral.m_fPostHeight;
			pParam=m_pModel->listParams.Add(KEY4C("PSS"));	//ʼ���Ƿ���֧��	
			pParam->key=KEY4C("PSS");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=pageGeneral.m_bStartSup;
			pParam=m_pModel->listParams.Add(KEY4C("PBS"));		//�ն��Ƿ���֧��
			pParam->key=KEY4C("PBS");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=pageGeneral.m_bEndSup;
		}
		if(pageGeneral.m_bDesignBeam)
		{
			pParam=m_pModel->listParams.Add(KEY4C("BTYP"));	//���ܺ������� 0.������ 1.�ķ��� 
			pParam->key=KEY4C("BTYP");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=pageGeneral.m_iBeamType;
			pParam=m_pModel->listParams.Add(KEY4C("DL"));	
			pParam->key=KEY4C("DL");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageGeneral.m_fDL;
		}
	}
	return TRUE;
}

BOOL CNewSubstationDlg::FillPostParam()
{	//����������
	DESIGN_PARAM_ITEM* pParam=NULL;
	if(m_pModel!=NULL)
	{
		if(pageGeneral.m_iPostType==0)		//���ּ�
		{
			pParam=m_pModel->listParams.Add(KEY4C("PTW"));	//�������ڿ��
			pParam->key=KEY4C("PTW");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pagePost.m_fTopWidth;
#ifdef AFX_TARG_ENU_ENGLISH
			pParam->sLabel.Copy("Shaft bracket open upward");
			pParam->sNotes.Copy("top width of shaft bracket, mm");
#else
			pParam->sLabel.Copy("���ּ��Ͽ���");
			pParam->sNotes.Copy("���ּܶ�����, mm");
#endif
			pParam=m_pModel->listParams.Add(KEY4C("PBW"));	//�ײ����ڿ��
			pParam->key=KEY4C("PBW");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pagePost.m_fBtmWidth;
#ifdef AFX_TARG_ENU_ENGLISH
			pParam->sLabel.Copy("Shaft bracket open downward");
			pParam->sNotes.Copy("bottom width of shaft bracket, mm");
#else
			pParam->sLabel.Copy("���ּ��¸���");
			pParam->sNotes.Copy("���ּܵײ����, mm");
#endif
			pParam=m_pModel->listParams.Add(KEY4C("HV"));	//�����˸߶�
			pParam->key=KEY4C("HV");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pagePost.m_fLinkHigh;
#ifdef AFX_TARG_ENU_ENGLISH
			pParam->sLabel.Copy("Horizon Pole vertical distance");
			pParam->sNotes.Copy("vertical offset value of horizon pole from the top, mm");
#else
			pParam->sLabel.Copy("�����˴���");
			pParam->sNotes.Copy("�����������ּܶ����Ĵ�ֱƫ����, mm");
#endif
			if(pageGeneral.m_bStartSup || pageGeneral.m_bEndSup)
			{
				pParam=m_pModel->listParams.Add(KEY4C("TSW"));	//������ſ��
				pParam->key=KEY4C("TSW");
				pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
				pParam->value.fValue=pagePost.m_fTopSideWidth;
#ifdef AFX_TARG_ENU_ENGLISH
				pParam->sLabel.Copy("Shaft bracket open at the top of the lateral brace");
				pParam->sNotes.Copy("projection distance of shaft bracket between end foundation and support plane,mm");
#else
				pParam->sLabel.Copy("���ּܶ�����Ÿ���");
				pParam->sNotes.Copy("���ּܶ˳Ż�����֧�����ͶӰ����, mm");
#endif
				pParam=m_pModel->listParams.Add(KEY4C("BSD"));	//�ײ���ſ��
				pParam->key=KEY4C("BSD");
				pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
				pParam->value.fValue=pagePost.m_fBtmSideWidth;
#ifdef AFX_TARG_ENU_ENGLISH
				pParam->sLabel.Copy("Shaft bracket open at the bottom of the lateral brace");
				pParam->sNotes.Copy("projection distance of shaft bracket between end foundation and support plane,mm");
#else
				pParam->sLabel.Copy("���ֵײ��ܲ�Ÿ���");
				pParam->sNotes.Copy("���ּܶ˳Ż�����֧�����ͶӰ����, mm");
#endif
			}
		}
		else
		{

		}
	}
	return TRUE;
}

BOOL CNewSubstationDlg::FillBeamParam()
{	//����������
	DESIGN_PARAM_ITEM* pParam=NULL;
	if(m_pModel!=NULL)
	{
		if(pageGeneral.m_iBeamType==0)	//������
		{	//���
			pParam=m_pModel->listParams.Add(KEY4C("LEN"));	
			pParam->key=KEY4C("LEN");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSegment.m_fSpanLen+pageGeneral.m_fDL*2;
			//����ڿ��
			pParam=m_pModel->listParams.Add(KEY4C("LCBW"));
			pParam->key=KEY4C("LCBW");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSegment.m_fWidth1;
			//ֱ�ڿ��
			pParam=m_pModel->listParams.Add(KEY4C("SBW"));
			pParam->key=KEY4C("SBW");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSegment.m_fWidth2;
			//����ڿ��
			pParam=m_pModel->listParams.Add(KEY4C("RCBW"));
			pParam->key=KEY4C("RCBW");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSegment.m_fWidth3;
			//�����������
			pParam=m_pModel->listParams.Add(KEY4C("LCBL"));
			pParam->key=KEY4C("LCBL");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSegment.m_fLeftBeamLength;
			//�Ҳ���������
			pParam=m_pModel->listParams.Add(KEY4C("RCBL"));
			pParam->key=KEY4C("RCBL");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSegment.m_fRightBeamLength;
			//ֱ����೤��
			pParam=m_pModel->listParams.Add(KEY4C("LSBL"));
			pParam->key=KEY4C("LSBL");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSegment.m_fLSBeamLen;
			//ֱ������
			pParam=m_pModel->listParams.Add(KEY4C("SBL"));
			pParam->key=KEY4C("SBL");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSegment.m_fSBeamLen;
			//�����߶�
			pParam=m_pModel->listParams.Add(KEY4C("TBH"));
			pParam->key=KEY4C("TBH");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSegment.m_fBeamHeight;
			//�ڼ�˼�����
			pParam=m_pModel->listParams.Add(KEY4C("PLT"));
			pParam->key=KEY4C("PLT");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=pageSegment.m_iPoleType;
			//��Ӹ��ڼ���Ϣ
			int i=0,num=0;
			ARRAY_LIST<double>* pArray=NULL;
			ARRAY_LIST<CXhChar16>* pLayoutArray=NULL;
			//��������ڼ���
			num=pageSegment.m_xLCBeamSegArr.GetSize();
			pParam=m_pModel->listParams.Add(KEY4C("LCBN"));
			pParam->key=KEY4C("LCBN");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=num;
			//��������ڼ����+ˮƽ����
			pArray=m_pModel->AppendDoubleArray(KEY4C("LCBF"));
			pLayoutArray=((CSubstationTemplate*)m_pModel->m_pTemplate)->hashSectionLayouts.AddNoneKeyObj(KEY4C("LCBR"));
			pLayoutArray->SetSize(0,1);
			pArray->Empty();
			pLayoutArray->Empty();
			for(int i=0;i<num;i++)
			{
				pArray->append(pageSegment.m_xLCBeamSegArr[i].space);
				pLayoutArray->append(pageSegment.m_xLCBeamSegArr[i].LayoutStr());
			}
			//�Ҳ������ڼ����+ˮƽ����
			num=pageSegment.m_xRCBeamSegArr.GetSize();
			pParam=m_pModel->listParams.Add(KEY4C("RCBN"));
			pParam->key=KEY4C("RCBN");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=num;
			//�Ҳ������ڼ���
			pArray=m_pModel->AppendDoubleArray(KEY4C("RCBF"));
			pLayoutArray=((CSubstationTemplate*)m_pModel->m_pTemplate)->hashSectionLayouts.AddNoneKeyObj(KEY4C("RCBR"));
			pArray->Empty();
			pLayoutArray->Empty();
			for(int i=0;i<num;i++)
			{
				pArray->append(pageSegment.m_xRCBeamSegArr[i].space);
				pLayoutArray->append(pageSegment.m_xRCBeamSegArr[i].LayoutStr());
			}
			//���ֱ�����ڼ�����
			num=pageSegment.m_xLSBeamSegArr.GetSize();
			pParam=m_pModel->listParams.Add(KEY4C("LSBN"));
			pParam->key=KEY4C("LSBN");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=num;
			//���ֱ�����ڼ����+����
			pArray=m_pModel->AppendDoubleArray(KEY4C("LSBF"));
			pLayoutArray=((CSubstationTemplate*)m_pModel->m_pTemplate)->hashSectionLayouts.AddNoneKeyObj(KEY4C("LSBR"));
			pArray->Empty();
			pLayoutArray->Empty();
			for(int i=0;i<num;i++)
			{
				pArray->append(pageSegment.m_xLSBeamSegArr[i].space);
				pLayoutArray->append(pageSegment.m_xLSBeamSegArr[i].LayoutStr());
			}
			//���ֱ���Ҳ�ڼ�����
			num=pageSegment.m_xRSBeamSegArr.GetSize();
			pParam=m_pModel->listParams.Add(KEY4C("RSBN"));
			pParam->key=KEY4C("RSBN");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=num;
			//���ֱ���Ҳ�ڼ����+����
			pArray=m_pModel->AppendDoubleArray(KEY4C("RSBF"));
			pLayoutArray=((CSubstationTemplate*)m_pModel->m_pTemplate)->hashSectionLayouts.AddNoneKeyObj(KEY4C("RSBR"));
			pArray->Empty();
			pLayoutArray->Empty();
			for(int i=0;i<num;i++)
			{
				pArray->append(pageSegment.m_xRSBeamSegArr[i].space);
				pLayoutArray->append(pageSegment.m_xRSBeamSegArr[i].LayoutStr());
			}
			//��Ӷ����ڼ���
			num=pageSegment.m_xLTBeamSegArr.GetSize();
			pParam=m_pModel->listParams.Add(KEY4C("LTBN"));
			pParam->key=KEY4C("LTBN");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=num-1;
			//��Ӷ����ڼ����+����
			pArray=m_pModel->AppendDoubleArray(KEY4C("LTBF"));
			pLayoutArray=((CSubstationTemplate*)m_pModel->m_pTemplate)->hashSectionLayouts.AddNoneKeyObj(KEY4C("LTBR"));
			pArray->Empty();
			pLayoutArray->Empty();
			((CSubstationTemplate*)m_pModel->m_pTemplate)->m_sLeftFrontLayout.Copy(pageSegment.m_xLTBeamSegArr[0].LayoutStr());
			for(int i=1;i<num;i++)
			{
				pArray->append(pageSegment.m_xLTBeamSegArr[i].space);
				pLayoutArray->append(pageSegment.m_xLTBeamSegArr[i].LayoutStr());
			}
			//��Ӷ����ڼ���
			num=pageSegment.m_xRTBeamSegArr.GetSize();
			pParam=m_pModel->listParams.Add(KEY4C("RTBN"));
			pParam->key=KEY4C("RTBN");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=num-1;
			//��Ӷ����ڼ����+����
			pArray=m_pModel->AppendDoubleArray(KEY4C("RTBF"));
			pLayoutArray=((CSubstationTemplate*)m_pModel->m_pTemplate)->hashSectionLayouts.AddNoneKeyObj(KEY4C("RTBR"));
			pArray->Empty();
			pLayoutArray->Empty();
			for(int i=0;i<num-1;i++)
			{
				pArray->append(pageSegment.m_xRTBeamSegArr[i].space);
				pLayoutArray->append(pageSegment.m_xRTBeamSegArr[i].LayoutStr());
			}
			((CSubstationTemplate*)m_pModel->m_pTemplate)->m_sRightFrontLayout.Copy(pageSegment.m_xRTBeamSegArr[num-1].LayoutStr());
		}
		else
		{
			pParam=m_pModel->listParams.Add(KEY4C("BW"));	//���������(beam width)
			pParam->key=KEY4C("BW");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSquareBeam.m_fBeamWidth;
#ifdef AFX_TARG_ENU_ENGLISH
			pParam->sLabel.Copy("Rectangular beam width");
			pParam->sNotes.Copy("Rectangular beam width, mm");
#else
			pParam->sLabel.Copy("���������");
			pParam->sNotes.Copy("���������, mm");
#endif
			pParam=m_pModel->listParams.Add(KEY4C("BH"));	//�������߶�(beam height)
			pParam->key=KEY4C("BH");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSquareBeam.m_fBeamHeight;
#ifdef AFX_TARG_ENU_ENGLISH
			pParam->sLabel.Copy("Rectangular beam height");
			pParam->sNotes.Copy("Rectangular beam height, mm");
#else
			pParam->sLabel.Copy("�������߶�");
			pParam->sNotes.Copy("�������߶�, mm");
#endif
			pParam=m_pModel->listParams.Add(KEY4C("BIN"));	//�������Գƽڼ���(beam internode num)
			pParam->key=KEY4C("BIN");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=pageSquareBeam.m_nInternodeNum;
#ifdef AFX_TARG_ENU_ENGLISH
			pParam->sLabel.Copy("Triangle beam symmtrical nodes number");
			pParam->sNotes.Copy("Triangle beam symmtrical nodes number");
#else
			pParam->sLabel.Copy("�������Գƽڼ���");
			pParam->sNotes.Copy("�������Գƽڼ���");
#endif
			pParam=m_pModel->listParams.Add(KEY4C("BRT"));	//�ڼ�˼�����
			pParam->key=KEY4C("BRT");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=pageSquareBeam.m_iPoleType;
#ifdef AFX_TARG_ENU_ENGLISH
			pParam->sLabel.Copy("inter-node rod type");
			pParam->sNotes.Copy("inter-node rod type");
#else
			pParam->sLabel.Copy("�ڼ�˼�����");
			pParam->sNotes.Copy("�ڼ�˼�����");
#endif
		}
	}
	return TRUE;
}

	// modal processing
INT_PTR CNewSubstationDlg::DoModal()
{
	if(!m_bWizardDialog)	//����ͨ����ҳģʽ�����Ի���
		return CDialogEx::DoModal();
	pageGeneral.m_bDesignPost=m_bDesignPost;
	pageGeneral.m_bDesignBeam=m_bDesignBeam;
	if(pageGeneral.DoModal()==IDOK&&FillSummarizeParam())
	{
		m_bDesignPost=pageGeneral.m_bDesignPost;
		m_bDesignBeam=pageGeneral.m_bDesignBeam;
		if(pageGeneral.m_bDesignPost)
		{
			if(pagePost.DoModal()==IDOK)
				FillPostParam();
			else 
				return IDCANCEL;
		}
		if(pageGeneral.m_bDesignBeam)
		{
			if(pageGeneral.m_iBeamType==0&&pageSegment.DoModal()==IDOK)
				FillBeamParam();
			else if(pageGeneral.m_iBeamType==1&&pageSquareBeam.DoModal()==IDOK)
				FillBeamParam();
			else
				return IDCANCEL;
		}
	}
	else
		return IDCANCEL;
	return IDOK;
}