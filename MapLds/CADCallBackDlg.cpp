//<LOCALE_TRANSLATE BY hxr /> 2015-04-25
// CADCallBackDlg.cpp : implementation file
//

#include "stdafx.h"
#include "stdarx.h"
#include "CADCallBackDlg.h"
#include "dbents.h"
#include "RxTools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCADCallBackDlg dialog

CCADCallBackDlg::CCADCallBackDlg(UINT nIDTemplate,CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
{
	
	//{{AFX_DATA_INIT(CCADCallBackDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nPickEntType=1;	//0.�� 1.����ʵ�� wht 11-06-29
	m_iBreakType=1;
	m_bInernalStart=FALSE;
	m_bPauseBreakExit=FALSE;
	FireCallBackFunc=NULL;
	m_nResultEnt=1;	//Ĭ��ѡ��һ��ʵ��
	m_bFireListItemChange=TRUE;
	m_arrCmdPickPrompt.RemoveAll();
}


void CCADCallBackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCADCallBackDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCADCallBackDlg, CDialog)
	//{{AFX_MSG_MAP(CCADCallBackDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCADCallBackDlg message handlers

#if defined(_WIN64)
INT_PTR CCADCallBackDlg::DoModal()
#else
int CCADCallBackDlg::DoModal() 
#endif
{
	//m_bInernalStart=FALSE;
	int ret=-1;
	ads_name prevPickEnt;
	ads_name_clear(prevPickEnt);
	while(1)
	{
		ret=(int)CDialog::DoModal();
		if(m_bPauseBreakExit)
		{
			if(m_iBreakType==1)
			{	
				//ͨ��ѡ��ʵ���ȡʵ���ID
				ads_name pick_ent;
				ads_point pick_pt,prev_pt;
				resultList.Empty();
				for(int i=0;i<m_nResultEnt;i++)
				{
					AcDbEntity *pEnt=NULL;
					char sPrompt[200]=""; 
					if(i<m_arrCmdPickPrompt.GetSize())	//�ⲿ�趨
						strcpy(sPrompt,m_arrCmdPickPrompt[i]);
					else if(m_nPickEntType==0)	//Ĭ�ϵ���ʾ�ַ���
#ifdef AFX_TARG_ENU_ENGLISH
						sprintf(sPrompt,"\n Please select no. %d outline vertex<Enter Confirm>:",i+1);
					else if(m_nPickEntType==1)	//������չ���Ӧ�Ľڵ�
						sprintf(sPrompt,"\n Please select no. %d node<Enter Confirm>:",i+1);
					else if(m_nPickEntType==2)	//ѡ��˼�����
						sprintf(sPrompt,"\n Please select no. %d rod(bolt line)<right click finish select>:",i+1);
					else 
						sprintf(sPrompt,"\n Please select no.%d object<Enter Confirm>:",i+1);
#else
						sprintf(sPrompt,"\n��ѡ���%d��������<Enterȷ��>:",i+1);
					else if(m_nPickEntType==1)	//������չ���Ӧ�Ľڵ�
						sprintf(sPrompt,"\n��ѡ���%d���ڵ�<Enterȷ��>:",i+1);
					else if(m_nPickEntType==2)	//ѡ��˼�����
						sprintf(sPrompt,"\n��ѡ���%d���˼�(����)<�Ҽ�����ѡ��>:",i+1);
					else 
						sprintf(sPrompt,"\n��ѡ���%d������<Enterȷ��>:",i+1);
#endif
					if(m_nPickEntType==0)
					{
						ads_point temp_pt;
						if(i==0)
						{
						#ifdef _ARX_2007
							if(acedGetPoint(NULL,_bstr_t(sPrompt),temp_pt)==RTNORM)
						#else	
							if(acedGetPoint(NULL,sPrompt,temp_pt)==RTNORM)
						#endif
							{
								CAD_SCREEN_ENT *pCADEnt=resultList.append();
								pCADEnt->m_ptPick.Set(temp_pt[X],temp_pt[Y],temp_pt[Z]);
								prev_pt[X]=temp_pt[X];
								prev_pt[Y]=temp_pt[Y];
								prev_pt[Z]=temp_pt[Z];
							}
							else 
							{
								if(m_nPickEntType==2&&!ads_name_nil(prevPickEnt))	//�ֶ����������
									acedRedraw(prevPickEnt,4);	//ȡ��֮ǰѡ��ͼԪ��ѡ��״̬
								break;
							}
						}
						else 
						{
						#ifdef _ARX_2007
							if(acedGetPoint(prev_pt,_bstr_t(sPrompt),temp_pt)==RTNORM)
						#else	
							if(acedGetPoint(prev_pt,sPrompt,temp_pt)==RTNORM)
						#endif
							{
								CAD_SCREEN_ENT *pCADEnt=resultList.append();
								pCADEnt->m_ptPick.Set(temp_pt[X],temp_pt[Y],temp_pt[Z]);
								prev_pt[X]=temp_pt[X];
								prev_pt[Y]=temp_pt[Y];
								prev_pt[Z]=temp_pt[Z];
							}
							else 
								break;
						}
					}
					else 
					{
						if(m_nPickEntType==1)
						{	//���ݵ��Ӧ�ڵ� wht 11-07-13
					   #ifdef _ARX_2007
							acedCommand(RTSTR,L"PDMODE",RTSTR,L"34",RTNONE);//��ʾ�� X
					   #else
							acedCommand(RTSTR,"REGEN",RTNONE);
					   #endif
						}
						if(m_nPickEntType==2)	//�ֶ����������
						{
							if(!ads_name_nil(prevPickEnt))
								acedRedraw(prevPickEnt,4);	//ȡ��֮ǰѡ��ͼԪ��ѡ��״̬
							ads_name_set(pick_ent,prevPickEnt);
						}
					#ifdef _ARX_2007
						if(ads_entsel(_bstr_t(sPrompt),pick_ent,pick_pt)==RTNORM)
					#else
						if(ads_entsel(sPrompt,pick_ent,pick_pt)==RTNORM)
					#endif
						{
							CAD_SCREEN_ENT *pCADEnt=resultList.append();
							acdbGetObjectId(pCADEnt->m_idEnt,pick_ent);
							acdbOpenObject(pEnt,pCADEnt->m_idEnt,AcDb::kForRead);
							if(pEnt)
							{
								if(pEnt->isKindOf(AcDbPoint::desc()))
								{	//��
									AcDbPoint *pPoint=(AcDbPoint*)pEnt;
									m_ptPick.Set(pPoint->position().x,pPoint->position().y,pPoint->position().z);
									pCADEnt->m_ptPick=m_ptPick;
								}
								else if(pEnt->isKindOf(AcDbLine::desc()))
								{	//ֱ��
									
								}
								long hObj=GetTaAtomHandle(pEnt);
								pCADEnt->m_pObj=Ta.FromHandle(hObj);
								//������ʾʵ�� wht 12-03-22
								ads_name ent_name;
								acdbGetAdsName(ent_name,pCADEnt->m_idEnt);
								ads_redraw(ent_name,3);//������ʾ
								
								pEnt->close();
								if(m_nPickEntType==1)
								{	//���ݵ��Ӧ�ڵ� wht 11-07-13
							   #ifdef _ARX_2007
									acedCommand(RTSTR,L"PDMODE",RTSTR,L"0",RTNONE);//��ʾ�� .
							   #else
									acedCommand(RTSTR,"REGEN",RTNONE);
							   #endif
								}
							}
						}
						else 
						{
							if(m_nPickEntType==1)
							{	//���ݵ��Ӧ�ڵ� wht 11-07-13
						   #ifdef _ARX_2007
							   acedCommand(RTSTR,L"PDMODE",RTSTR,L"0",RTNONE);//��ʾ�� .
						   #else
							   acedCommand(RTSTR,"REGEN",RTNONE);
						   #endif
							}
							break;
						}
					}
				}
			}
			else if(m_iBreakType==3)
			{	//ִ������
				if(FireCallBackFunc)
					FireCallBackFunc(this);
			}
			m_bInernalStart=TRUE;
			m_bPauseBreakExit=FALSE;
		}
		else
			break;
	}
	return ret;
}
