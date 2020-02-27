//<LOCALE_TRANSLATE Confirm by hxr/>
#if !defined(AFX_TOOLKITDLG_H__D5082355_DE9E_4696_BA19_107853B8815F__INCLUDED_)
#define AFX_TOOLKITDLG_H__D5082355_DE9E_4696_BA19_107853B8815F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolKitDlg.h : header file
//

#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "PropertyList.h"
#include "env_def.h"
#include "CallBackDialog.h"
#include "GlobalFunc.h"
/////////////////////////////////////////////////////////////////////////////
// CToolKitDlg dialog

#define VEC_OFFSET_ID_COEF		100000	//ʸ��ƫ��IDϵ��

//Բ��Ԥ��
struct OPER_PRE_ARCLIFT
{
	char rot_axis;		//'X'��'Y'
	double lift_height;	//Ԥ���߶�
	f3dPoint rot_center;//��ת����
	f3dPoint lift_pos;	//Ԥ����׼λ��(��λ��Ԥ��lift_heightֵ)
	f3dPoint pick,result;	//Ԥ����Ԥ��ǰ��λ��
	void Cal();
};
//ƽ̨����(������ƽ̨Ԥ������)
#ifndef __TSA_
struct OPER_BOARDCAL
{
	long hPoleGrad,hPoleHori;		//б���˼�,ˮƽ�˼����
	double vert_high,hori_space;	//�����϶,��ֱ�߶�
	double grad_offset,left_hori_offset,board_len;	//б��ƫ����,ˮƽ��ƫ����,ƽ̨����(������)
	f3dPoint corner_up;				//�Ǹ����Ͻǵ�λ��(����֫���߻���,������)
	f3dPoint corner_down;			//���½ǽǸ����λ��(������)
	//
	BOOL Cal(char *error_info);
};
#endif
struct TOOLKIT_OPER_DATA
{
	//�ռ�������빫ʽ
	struct OPER_VEC_LEN{
		f3dPoint start,end;
		double result;
		double CalLen()
		{
			result = DISTANCE(start,end);
			return result;
		}
	}vec_len;
	//�㵽��ʸ������
	struct OPER_VEC_PtoP{
		f3dPoint start,end,result;
		f3dPoint CalVec()
		{
			result=end-start;
			normalize(result);
			return result;
		}
	}vec_p2p;
	//ʸ���нǲ���
	struct OPER_VEC_ANGLE{
		f3dPoint vec1,vec2;
		double rad_angle;
		double CalAngle()
		{
			f3dPoint vecStart=vec1,vecEnd=vec2;
			normalize(vecStart);
			normalize(vecEnd);
			rad_angle = cal_angle_of_2vec(vecStart,vecEnd);
			return rad_angle;
		}
	}vec_ang;
	//ʸ��ƫ����ʼ��
	f3dPoint offset_origin;
	//������
	f3dPoint result;
	struct OPER_VEC_OFFSET{
		int iNo;
		double offset;
		f3dPoint vec;
	}vec_offset;	
	//����ϵװ��
	struct OPER_CS_TRANS{
		UCS_STRU cs;
		f3dPoint src,dest;
		void CoordTrans(BOOL bUcsToWcs)
		{
			dest=src;
			coord_trans(dest,cs,bUcsToWcs);
		}
	}trans_cs;
	//���㹲��
	struct OPER_3D_FACE3P{
		f3dPoint p[3],face_norm;
		void CopyPlane()
		{
			WritePlaneToClip(p[0],face_norm);
		}
		void CalPlane()
		{
			f3dPoint vec1,vec2;
			vec1=p[1]-p[0];
			normalize(vec1);
			vec2=p[2]-p[0];
			normalize(vec2);
			face_norm=vec1^vec2;
			normalize(face_norm);
		}
	}face3p;
	//��ֱ�߽���
	struct OPER_3D_INT_LL{
		f3dLine line1,line2;
		f3dPoint result;
		BOOL CalIntersPt(char *error_info)
		{
			int nRetCode = Int3dll(line1,line2,result);
			if(nRetCode==-2||nRetCode==1||nRetCode==2)	//ֱ���⽻�㡢ֱ���ڽ��㡢ֱ�߶ζϵ�
				return TRUE;
			else 
			{
				if(error_info)
				{
					if(nRetCode==-1)
#ifdef AFX_TARG_ENU_ENGLISH
						strcpy(error_info,"Straight line end point overlap, the definition is wrong");
					else if(nRetCode==0)
						strcpy(error_info,"Straight line parallel,overlap or different plane");
					else 
						strcpy(error_info,"calculate result abnormal");
#else
						strcpy(error_info,"ֱ�߶˵��غϣ���������");
					else if(nRetCode==0)
						strcpy(error_info,"ֱ��ƽ�С��غϻ�����");
					else 
						strcpy(error_info,"�������쳣");
#endif
				}
				return FALSE;
			}
		}
	}inters_ll;
	//���潻��
	struct OPER_3D_INT_LF{
		f3dLine line;
		f3dPoint face_pos,face_norm;
		f3dPoint result;
		void PastPlane()
		{
			ReadClipPlane(face_pos,face_norm);
		}
		BOOL CalIntersPt(char *error_info)
		{
			int nRetCode = Int3dlf(result,line,face_pos,face_norm);
			if(nRetCode==1)	//�õ���Ч����
				return TRUE;
			else 
			{
				if(error_info)
				{
					if(nRetCode==-1)
#ifdef AFX_TARG_ENU_ENGLISH
						strcpy(error_info,"the definition of line or plane is wrong0");
					else if(nRetCode==0)
						strcpy(error_info,"line and plane are parallel or overlap");
					else 
						strcpy(error_info,"calculate result abnormal");
#else
						strcpy(error_info,"ֱ�߻��涨������0");
					else if(nRetCode==0)
						strcpy(error_info,"������ƽ�л��غ�");
					else 
						strcpy(error_info,"�������쳣");
#endif
				}
				return FALSE;
			}
		}
	}inters_lf;
	//�㵽�ߵľ���
	struct OPER_3D_DIST_PL
	{
		f3dPoint pt;			//ʰȡ��
		f3dPoint line_start;	//�����
		f3dPoint line_end;		//���յ�
		double result;
		double Cal()
		{
			result=DistOf3dPtLine(pt,line_start,line_end);
			return result;
		}
	}dist_pl;
	//�㵽��ľ���
	struct OPER_3D_DIST_PF
	{
		f3dPoint pt;		//ʰȡ��
		f3dPoint face_pos;	//�����
		f3dPoint face_norm;	//�淨��
		double result;
		void PastPlane()
		{
			ReadClipPlane(face_pos,face_norm);
		}
		double Cal()
		{
			f3dPoint prj_pt=pt;
			project_point(prj_pt,face_pos,face_norm);
			result=DISTANCE(prj_pt,pt);
			return result;
		}
	}dist_pf;
	//Բ��Ԥ��
	OPER_PRE_ARCLIFT arc_lift;
#ifndef __TSA_
	//ƽ̨����(������ƽ̨Ԥ������)
	OPER_BOARDCAL board;
#endif
};
class CToolKitDlg : public CCallBackDialog
{
	// Construction
	CBitmap m_xCurPlateBmp;
	int m_iMinAvailable;	//ƫ�Ʒ�����С���ñ��
public:
	TOOLKIT_OPER_DATA m_xOperData;
	ATOM_LIST<TOOLKIT_OPER_DATA::OPER_VEC_OFFSET>listOffset;//ʸ��ƫ��ʱ��ƫ�ƹ���
	CToolKitDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL Create();
	TOOLKIT_OPER_DATA::OPER_VEC_OFFSET *AddVecOffsetItem(f3dPoint vec,double offse_dist);
	f3dPoint CalVecOffsetResult();
	void UpdateSketchMap(CPropTreeItem *pItem);
// Dialog Data
	//{{AFX_DATA(CToolKitDlg)
	enum { IDD = IDD_TOOLKIT_DLG };
	CPropertyList	m_propList;
	CTabCtrl	m_tabCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolKitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InsertPointPropItem(CPropTreeItem *pParentItem,char *sIdProp,char *sPropName,BOOL bHideChild=FALSE);
	void DisplayItemList();
	void RefreshTabCtrl(int iCurSel);
	void ResetDlgSize(int iCurSel);
	void FinishSelectObjOper();

	// Generated message map functions
	//{{AFX_MSG(CToolKitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//���Թ���
	static int m_iCurGroup;				//��¼��������ǰ��ʾ���ID
	static CStringKeyHashTable<long>propHashtable;	//�����ַ�����ϣ��
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	CString GetPropValueStr(long id);				//��������ID�õ�����ֵ
	static void InitToolkitsPropHashtable();
	static long GetPropID(char* propStr);			//�õ�����ID
	static int GetPropStatus(long id);				//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);	//��������״̬
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLKITDLG_H__D5082355_DE9E_4696_BA19_107853B8815F__INCLUDED_)
