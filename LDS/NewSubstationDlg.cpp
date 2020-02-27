//<LOCALE_TRANSLATE Confirm by hxr/>
// NewSubstationDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "BlockTempl.h"
#include "NewSubstationDlg.h"
#include "afxdialogex.h"


// CNewSubstationDlg 对话框

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


// CNewSubstationDlg 消息处理程序


BOOL CNewSubstationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
	pageGeneral.m_bDesignPost=m_bDesignPost;
	pageGeneral.m_bDesignBeam=m_bDesignBeam;
	m_labelGeneral.SetBackgroundColour(RGB(128,128,128));
	m_labelPost.SetBackgroundColour(RGB(128,128,128));
	m_labelBeam.SetBackgroundColour(RGB(128,128,128));
	m_labelSegment.SetBackgroundColour(RGB(128,128,128));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
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
	// TODO: 在此添加专用代码和/或调用基类
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
	// TODO: 在此处添加消息处理程序代码
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
	// 不为绘图消息调用 CDialogEx::OnPaint()
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

//添加概述参数
BOOL CNewSubstationDlg::FillSummarizeParam()
{
	DESIGN_PARAM_ITEM* pParam=NULL;
	if(m_pModel)
	{	//跨数
		pParam=m_pModel->listParams.Add(KEY4C("NUM"));
		pParam->key=KEY4C("NUM");
		pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
		pParam->value.fValue=pageGeneral.m_nSpanNum;
		//跨度	此处为初始化值，在设计梁时重新计算
		pParam=m_pModel->listParams.Add(KEY4C("LEN"));	
		pParam->key=KEY4C("LEN");
		pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
		pParam->value.fValue=24000;
		//高度
		pParam=m_pModel->listParams.Add(KEY4C("H"));
		pParam->key=KEY4C("H");
		pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
		pParam->value.fValue=pageGeneral.m_fBeamHeight;
		if(pageGeneral.m_bDesignPost)
		{
			pParam=m_pModel->listParams.Add(KEY4C("PTYP"));	//构架柱类型 0.人字架
			pParam->key=KEY4C("PTYP");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=pageGeneral.m_iPostType;
			pParam=m_pModel->listParams.Add(KEY4C("PH"));
			pParam->key=KEY4C("PH");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageGeneral.m_fPostHeight;
			pParam=m_pModel->listParams.Add(KEY4C("PSS"));	//始端是否有支撑	
			pParam->key=KEY4C("PSS");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=pageGeneral.m_bStartSup;
			pParam=m_pModel->listParams.Add(KEY4C("PBS"));		//终端是否有支撑
			pParam->key=KEY4C("PBS");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=pageGeneral.m_bEndSup;
		}
		if(pageGeneral.m_bDesignBeam)
		{
			pParam=m_pModel->listParams.Add(KEY4C("BTYP"));	//构架横梁类型 0.三角梁 1.四方梁 
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
{	//构架柱参数
	DESIGN_PARAM_ITEM* pParam=NULL;
	if(m_pModel!=NULL)
	{
		if(pageGeneral.m_iPostType==0)		//人字架
		{
			pParam=m_pModel->listParams.Add(KEY4C("PTW"));	//顶部开口宽度
			pParam->key=KEY4C("PTW");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pagePost.m_fTopWidth;
#ifdef AFX_TARG_ENU_ENGLISH
			pParam->sLabel.Copy("Shaft bracket open upward");
			pParam->sNotes.Copy("top width of shaft bracket, mm");
#else
			pParam->sLabel.Copy("人字架上开口");
			pParam->sNotes.Copy("人字架顶面宽度, mm");
#endif
			pParam=m_pModel->listParams.Add(KEY4C("PBW"));	//底部开口宽度
			pParam->key=KEY4C("PBW");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pagePost.m_fBtmWidth;
#ifdef AFX_TARG_ENU_ENGLISH
			pParam->sLabel.Copy("Shaft bracket open downward");
			pParam->sNotes.Copy("bottom width of shaft bracket, mm");
#else
			pParam->sLabel.Copy("人字架下根开");
			pParam->sNotes.Copy("人字架底部宽度, mm");
#endif
			pParam=m_pModel->listParams.Add(KEY4C("HV"));	//横连杆高度
			pParam->key=KEY4C("HV");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pagePost.m_fLinkHigh;
#ifdef AFX_TARG_ENU_ENGLISH
			pParam->sLabel.Copy("Horizon Pole vertical distance");
			pParam->sNotes.Copy("vertical offset value of horizon pole from the top, mm");
#else
			pParam->sLabel.Copy("横连杆垂距");
			pParam->sNotes.Copy("横连杆自人字架顶部的垂直偏移量, mm");
#endif
			if(pageGeneral.m_bStartSup || pageGeneral.m_bEndSup)
			{
				pParam=m_pModel->listParams.Add(KEY4C("TSW"));	//顶部侧撑宽度
				pParam->key=KEY4C("TSW");
				pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
				pParam->value.fValue=pagePost.m_fTopSideWidth;
#ifdef AFX_TARG_ENU_ENGLISH
				pParam->sLabel.Copy("Shaft bracket open at the top of the lateral brace");
				pParam->sNotes.Copy("projection distance of shaft bracket between end foundation and support plane,mm");
#else
				pParam->sLabel.Copy("人字架顶部侧撑根开");
				pParam->sNotes.Copy("人字架端撑基础到支撑面的投影距离, mm");
#endif
				pParam=m_pModel->listParams.Add(KEY4C("BSD"));	//底部侧撑宽度
				pParam->key=KEY4C("BSD");
				pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
				pParam->value.fValue=pagePost.m_fBtmSideWidth;
#ifdef AFX_TARG_ENU_ENGLISH
				pParam->sLabel.Copy("Shaft bracket open at the bottom of the lateral brace");
				pParam->sNotes.Copy("projection distance of shaft bracket between end foundation and support plane,mm");
#else
				pParam->sLabel.Copy("人字底部架侧撑根开");
				pParam->sNotes.Copy("人字架端撑基础到支撑面的投影距离, mm");
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
{	//构架梁参数
	DESIGN_PARAM_ITEM* pParam=NULL;
	if(m_pModel!=NULL)
	{
		if(pageGeneral.m_iBeamType==0)	//三角梁
		{	//跨度
			pParam=m_pModel->listParams.Add(KEY4C("LEN"));	
			pParam->key=KEY4C("LEN");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSegment.m_fSpanLen+pageGeneral.m_fDL*2;
			//左弯口宽度
			pParam=m_pModel->listParams.Add(KEY4C("LCBW"));
			pParam->key=KEY4C("LCBW");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSegment.m_fWidth1;
			//直口宽度
			pParam=m_pModel->listParams.Add(KEY4C("SBW"));
			pParam->key=KEY4C("SBW");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSegment.m_fWidth2;
			//右弯口宽度
			pParam=m_pModel->listParams.Add(KEY4C("RCBW"));
			pParam->key=KEY4C("RCBW");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSegment.m_fWidth3;
			//左侧曲梁长度
			pParam=m_pModel->listParams.Add(KEY4C("LCBL"));
			pParam->key=KEY4C("LCBL");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSegment.m_fLeftBeamLength;
			//右侧曲梁长度
			pParam=m_pModel->listParams.Add(KEY4C("RCBL"));
			pParam->key=KEY4C("RCBL");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSegment.m_fRightBeamLength;
			//直梁左侧长度
			pParam=m_pModel->listParams.Add(KEY4C("LSBL"));
			pParam->key=KEY4C("LSBL");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSegment.m_fLSBeamLen;
			//直梁长度
			pParam=m_pModel->listParams.Add(KEY4C("SBL"));
			pParam->key=KEY4C("SBL");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSegment.m_fSBeamLen;
			//顶梁高度
			pParam=m_pModel->listParams.Add(KEY4C("TBH"));
			pParam->key=KEY4C("TBH");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSegment.m_fBeamHeight;
			//节间杆件类型
			pParam=m_pModel->listParams.Add(KEY4C("PLT"));
			pParam->key=KEY4C("PLT");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=pageSegment.m_iPoleType;
			//添加各节间信息
			int i=0,num=0;
			ARRAY_LIST<double>* pArray=NULL;
			ARRAY_LIST<CXhChar16>* pLayoutArray=NULL;
			//左侧曲梁节间数
			num=pageSegment.m_xLCBeamSegArr.GetSize();
			pParam=m_pModel->listParams.Add(KEY4C("LCBN"));
			pParam->key=KEY4C("LCBN");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=num;
			//左侧曲梁节间距离+水平布局
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
			//右侧曲梁节间距离+水平布局
			num=pageSegment.m_xRCBeamSegArr.GetSize();
			pParam=m_pModel->listParams.Add(KEY4C("RCBN"));
			pParam->key=KEY4C("RCBN");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=num;
			//右侧曲梁节间数
			pArray=m_pModel->AppendDoubleArray(KEY4C("RCBF"));
			pLayoutArray=((CSubstationTemplate*)m_pModel->m_pTemplate)->hashSectionLayouts.AddNoneKeyObj(KEY4C("RCBR"));
			pArray->Empty();
			pLayoutArray->Empty();
			for(int i=0;i<num;i++)
			{
				pArray->append(pageSegment.m_xRCBeamSegArr[i].space);
				pLayoutArray->append(pageSegment.m_xRCBeamSegArr[i].LayoutStr());
			}
			//添加直梁左侧节间数量
			num=pageSegment.m_xLSBeamSegArr.GetSize();
			pParam=m_pModel->listParams.Add(KEY4C("LSBN"));
			pParam->key=KEY4C("LSBN");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=num;
			//添加直梁左侧节间距离+布局
			pArray=m_pModel->AppendDoubleArray(KEY4C("LSBF"));
			pLayoutArray=((CSubstationTemplate*)m_pModel->m_pTemplate)->hashSectionLayouts.AddNoneKeyObj(KEY4C("LSBR"));
			pArray->Empty();
			pLayoutArray->Empty();
			for(int i=0;i<num;i++)
			{
				pArray->append(pageSegment.m_xLSBeamSegArr[i].space);
				pLayoutArray->append(pageSegment.m_xLSBeamSegArr[i].LayoutStr());
			}
			//添加直梁右侧节间数量
			num=pageSegment.m_xRSBeamSegArr.GetSize();
			pParam=m_pModel->listParams.Add(KEY4C("RSBN"));
			pParam->key=KEY4C("RSBN");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=num;
			//添加直梁右侧节间距离+布局
			pArray=m_pModel->AppendDoubleArray(KEY4C("RSBF"));
			pLayoutArray=((CSubstationTemplate*)m_pModel->m_pTemplate)->hashSectionLayouts.AddNoneKeyObj(KEY4C("RSBR"));
			pArray->Empty();
			pLayoutArray->Empty();
			for(int i=0;i<num;i++)
			{
				pArray->append(pageSegment.m_xRSBeamSegArr[i].space);
				pLayoutArray->append(pageSegment.m_xRSBeamSegArr[i].LayoutStr());
			}
			//添加顶梁节间数
			num=pageSegment.m_xLTBeamSegArr.GetSize();
			pParam=m_pModel->listParams.Add(KEY4C("LTBN"));
			pParam->key=KEY4C("LTBN");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=num-1;
			//添加顶梁节间距离+布局
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
			//添加顶梁节间数
			num=pageSegment.m_xRTBeamSegArr.GetSize();
			pParam=m_pModel->listParams.Add(KEY4C("RTBN"));
			pParam->key=KEY4C("RTBN");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=num-1;
			//添加顶梁节间距离+布局
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
			pParam=m_pModel->listParams.Add(KEY4C("BW"));	//方形梁宽度(beam width)
			pParam->key=KEY4C("BW");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSquareBeam.m_fBeamWidth;
#ifdef AFX_TARG_ENU_ENGLISH
			pParam->sLabel.Copy("Rectangular beam width");
			pParam->sNotes.Copy("Rectangular beam width, mm");
#else
			pParam->sLabel.Copy("方形梁宽度");
			pParam->sNotes.Copy("方形梁宽度, mm");
#endif
			pParam=m_pModel->listParams.Add(KEY4C("BH"));	//方形梁高度(beam height)
			pParam->key=KEY4C("BH");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
			pParam->value.fValue=pageSquareBeam.m_fBeamHeight;
#ifdef AFX_TARG_ENU_ENGLISH
			pParam->sLabel.Copy("Rectangular beam height");
			pParam->sNotes.Copy("Rectangular beam height, mm");
#else
			pParam->sLabel.Copy("方形梁高度");
			pParam->sNotes.Copy("方形梁高度, mm");
#endif
			pParam=m_pModel->listParams.Add(KEY4C("BIN"));	//方形梁对称节间数(beam internode num)
			pParam->key=KEY4C("BIN");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=pageSquareBeam.m_nInternodeNum;
#ifdef AFX_TARG_ENU_ENGLISH
			pParam->sLabel.Copy("Triangle beam symmtrical nodes number");
			pParam->sNotes.Copy("Triangle beam symmtrical nodes number");
#else
			pParam->sLabel.Copy("方形梁对称节间数");
			pParam->sNotes.Copy("方形梁对称节间数");
#endif
			pParam=m_pModel->listParams.Add(KEY4C("BRT"));	//节间杆件类型
			pParam->key=KEY4C("BRT");
			pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
			pParam->value.iType=pageSquareBeam.m_iPoleType;
#ifdef AFX_TARG_ENU_ENGLISH
			pParam->sLabel.Copy("inter-node rod type");
			pParam->sNotes.Copy("inter-node rod type");
#else
			pParam->sLabel.Copy("节间杆件类型");
			pParam->sNotes.Copy("节间杆件类型");
#endif
		}
	}
	return TRUE;
}

	// modal processing
INT_PTR CNewSubstationDlg::DoModal()
{
	if(!m_bWizardDialog)	//以普通属性页模式启动对话框
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