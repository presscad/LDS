//<LOCALE_TRANSLATE Confirm by hxr/>
// DesignTransectFaceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DesignTransectFaceDlg.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CDesignTransectFaceDlg dialog


CDesignTransectFaceDlg::CDesignTransectFaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignTransectFaceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignTransectFaceDlg)
	m_bFaceToFace = FALSE;
	m_hLinkObj = _T("0");
	m_hViceLinkObj = _T("0");
	//}}AFX_DATA_INIT
	m_bStartEnd=FALSE;
	m_pDatumTube=NULL;
}


void CDesignTransectFaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignTransectFaceDlg)
	DDX_Check(pDX, IDC_CHK_FACE_TO_FACE, m_bFaceToFace);
	DDX_Text(pDX, IDC_E_LINK_OBJ, m_hLinkObj);
	DDX_Text(pDX, IDC_E_VICE_LINK_OBJ, m_hViceLinkObj);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignTransectFaceDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignTransectFaceDlg)
	ON_BN_CLICKED(IDC_CHK_FACE_TO_FACE, OnChkFaceToFace)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignTransectFaceDlg message handlers

void CDesignTransectFaceDlg::OnChkFaceToFace() 
{
	UpdateData();
	if(m_bFaceToFace)
		GetDlgItem(IDC_E_VICE_LINK_OBJ)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_E_VICE_LINK_OBJ)->EnableWindow(TRUE);
}

BOOL CDesignTransectFaceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_pDatumTube==NULL)
		return FALSE;
	if(m_bFaceToFace)
		GetDlgItem(IDC_E_VICE_LINK_OBJ)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_E_VICE_LINK_OBJ)->EnableWindow(TRUE);	
	return TRUE;
}

void CDesignTransectFaceDlg::OnOK() 
{
	UpdateData();	//��������
	CLDSNode *pNode=NULL;
	long hOldLinkTube=0,hOldViceLinkTube=0,hLinkObj=0,hViceLinkObj=0;
	char sText[MAX_TEMP_CHAR_100+1]="";
	TUBEJOINT *pCurrentJoint=NULL;
	f3dPoint cur_tube_vec,link_tube_vec,cutPlanePos;
	//ɾ���ֹܶԽ�ʱ���¶ԽӸֹܵĶ�����
	if(m_bStartEnd)
	{
		pNode = m_pDatumTube->pStart;
		hOldLinkTube = m_pDatumTube->m_tJointStart.hLinkObj;
		hOldViceLinkTube = m_pDatumTube->m_tJointStart.hViceLinkObj;
		cur_tube_vec=m_pDatumTube->End()-m_pDatumTube->Start();
		pCurrentJoint=&m_pDatumTube->m_tJointStart;
		cutPlanePos=m_pDatumTube->Start();
	}
	else
	{
		pNode = m_pDatumTube->pEnd;
		hOldLinkTube = m_pDatumTube->m_tJointEnd.hLinkObj;
		hOldViceLinkTube = m_pDatumTube->m_tJointEnd.hViceLinkObj;
		cur_tube_vec=m_pDatumTube->Start()-m_pDatumTube->End();
		pCurrentJoint=&m_pDatumTube->m_tJointEnd;
		cutPlanePos=m_pDatumTube->End();
	}
	normalize(cur_tube_vec);
	_snprintf(sText,MAX_TEMP_CHAR_100,"%s",m_hLinkObj);
	sscanf(sText,"%X",&hLinkObj);
	_snprintf(sText,MAX_TEMP_CHAR_100,"%s",m_hViceLinkObj);
	sscanf(sText,"%X",&hViceLinkObj);
	CLDSLineTube *pOldTube=(CLDSLineTube*)console.FromPartHandle(hOldLinkTube,CLS_LINETUBE);
	if(pOldTube&&hOldViceLinkTube<0)//�޸�ǰ�ǸֹܶԽ�
	{
		if(!m_bFaceToFace||(hLinkObj==0))	//ɾ���ֹܶԽ�
		{
			if(pOldTube->pStart==pNode)
			{
				pOldTube->m_tJointStart.type=0;
				pOldTube->m_tJointStart.hLinkObj=0;
				pOldTube->m_tJointStart.hViceLinkObj=0;
				pOldTube->m_tJointStart.cutPlanePos.Set();
				pOldTube->m_tJointStart.cutPlaneNorm.Set();
				pOldTube->SetStartOdd(0);
			}
			else if(pOldTube->pEnd==pNode)
			{
				pOldTube->m_tJointEnd.type=0;
				pOldTube->m_tJointEnd.hLinkObj=0;
				pOldTube->m_tJointEnd.hViceLinkObj=0;
				pOldTube->m_tJointEnd.cutPlanePos.Set();
				pOldTube->m_tJointEnd.cutPlaneNorm.Set();
				pOldTube->SetEndOdd(0);
			}
		}
	}
	//�ж�����ĶԽӸֹ��Ƿ����
	CLDSLineTube *pLineTube = (CLDSLineTube*)console.FromPartHandle(hLinkObj,CLS_LINETUBE);
	CLDSLineTube *pViceLineTube = (CLDSLineTube*)console.FromPartHandle(hViceLinkObj,CLS_LINETUBE);
	if((hLinkObj&&!pLineTube)||(m_bFaceToFace&&hViceLinkObj&&!pViceLineTube))	//��������Ϊ0������ĸֹܲ�����
	{
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("wrong input, the tube does not exist, input again!");
#else
		MessageBox("������󲻴��ڸøֹ�,����������!");
#endif
		return;
	}
	if(m_bFaceToFace&&pLineTube)//�Խ�����������˶ԽӸֹܾ��
	{
		if(pNode!=pLineTube->pStart&&pNode!=pLineTube->pEnd)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox("No connection between two tubes, docking cannot be realized, input again!");
#else
			MessageBox("���ֹ������Ӳ���ʵ�ֶԽ�,����������!");
#endif
			return;	
		}
		if(pLineTube->GetDiameter()!=m_pDatumTube->GetDiameter()||pLineTube->GetThick()!=m_pDatumTube->GetThick())
		{	//�ֹܹ��ͬ
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox("Two tubes are of different specification, docking cannot be realized, input again!");
#else
			MessageBox("���ֹܹ��ͬ����ʵ�ֶԽ�,����������!");
#endif
			return;
		}
		hViceLinkObj=-1;	//�Խ����
	}
	//���øֹ������ز���
	if(pCurrentJoint)
	{
		if(hLinkObj<0)
		{
			pCurrentJoint->cutPlaneNorm.Set();
			pCurrentJoint->cutPlanePos.Set();
			pCurrentJoint->type=0;
		}
		pCurrentJoint->type=1;
		pCurrentJoint->hLinkObj=hLinkObj;
		pCurrentJoint->hViceLinkObj=hViceLinkObj;
		m_pDatumTube->m_hPartWeldParent=hLinkObj;
	}
	if(m_bFaceToFace&&pLineTube&&pCurrentJoint)
	{
		//���ԽӸֹܵ���Ӧ������Ϊ�Խ����
		if(pLineTube->pStart==pNode)
		{
			pLineTube->m_tJointStart.type=1;
			pLineTube->m_tJointStart.hLinkObj=m_pDatumTube->handle;
			pLineTube->m_tJointStart.hViceLinkObj=-1;
			link_tube_vec=pLineTube->End()-pLineTube->Start();
		}
		else if(pLineTube->pEnd==pNode)
		{
			pLineTube->m_tJointEnd.type=1;
			pLineTube->m_tJointEnd.hLinkObj=m_pDatumTube->handle;
			pLineTube->m_tJointEnd.hViceLinkObj=-1;
			link_tube_vec=pLineTube->Start()-pLineTube->End();
		}
		normalize(link_tube_vec);
		//���öԽ������и�ƽ��
		f3dPoint mid_vec,vec,cutPlaneNorm;
		mid_vec=cur_tube_vec+link_tube_vec;
		normalize(mid_vec);
		vec=cur_tube_vec^link_tube_vec;
		normalize(vec);
		cutPlaneNorm=mid_vec^vec;
		normalize(cutPlaneNorm);
		//�����һ����С����ͷ
		double angle=cal_angle_of_2vec(cur_tube_vec,cutPlaneNorm);
		double tana=fabs(tan(angle));
		int min_dist=int(pLineTube->GetDiameter()*0.5*tana+1);
		//���õ�ǰ�ֹ��и���
		if(cutPlaneNorm*cur_tube_vec>0)
			cutPlaneNorm*=-1.0;
		pCurrentJoint->cutPlaneNorm=cutPlaneNorm;
		pCurrentJoint->cutPlanePos=cutPlanePos;
		//��������ͷ
		if(m_bStartEnd)
			m_pDatumTube->SetStartOdd(min_dist);
		else
			m_pDatumTube->SetEndOdd(min_dist);
		//���öԽӸֹܵ��и���
		if(pNode==pLineTube->pStart)
		{
			pLineTube->SetStartOdd(min_dist);	//������ͷ
			pLineTube->m_tJointStart.cutPlaneNorm = -1.0*cutPlaneNorm;
			pLineTube->m_tJointStart.cutPlanePos = pLineTube->Start();
		}
		else if(pNode==pLineTube->pEnd)
		{
			pLineTube->SetEndOdd(min_dist);	//������ͷ
			pLineTube->m_tJointEnd.cutPlaneNorm = -1.0*cutPlaneNorm;
			pLineTube->m_tJointEnd.cutPlanePos = pLineTube->End();
		}
	}
	CDialog::OnOK();
}
#endif
