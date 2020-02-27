// ImportTxtTa.h: interface for the CImportTxtTa class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMPORTTXTTA_H__7CCD8B73_14FE_4775_88DC_04CD6F914DE7__INCLUDED_)
#define AFX_IMPORTTXTTA_H__7CCD8B73_14FE_4775_88DC_04CD6F914DE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
struct NODE_STRU
{
	int No;			//�ڵ���
	int cPosCalType;//��������
	int attach_no1,attach_no2;	//�����ڵ���
	double attach_para;	//��������(�ȷ�������ƫ����)
	int iMirMsg;		//�Գ���Ϣ
	f3dPoint pt;		//�ڵ�����
	int mir_x_no;		//X��Գƽڵ���(ǰ��Գ�)
	int mir_y_no;		//Y��Գƽڵ���(���ҶԳ�)
	int mir_z_no;		//Z��Գƽڵ���(Z��Գ�)
	long handle;		//���
	int leg_i;			//��λ��Ϊ����Ÿ�λ��ʮλΪ���Ⱥ�
	int pt_type;		//�ڵ�����(0.���ɽڵ� 1.Լ���ڵ�)
	BOOL bFather;		//�Ƿ�Ϊ������
	int father_no;		//���������
};
struct JG_STRU
{
	int  No;			//�Ǹֱ��
	int  start_no,end_no;	//��ֹ�ڵ��
	int  iMirMsg;		//�Գ���Ϣ
	int  type;			//�Ǹ�����
	BOOL bForceJg;		//�Ƿ�Ϊ�����ģ�����Ϊ�����ģ�
	double wing_wide,wing_thick;		//�Ǹֹ��
	char cMaterial;		//����
	char LsGuiGe[24];	//������˨���
	double ls_grade;	//��˨����
	long ls_d;			//��˨ֱ��
	int LsN;			//ÿһ����˨������
	int mir_x_no;		//X��Գƽڵ���(ǰ��Գ�)
	int mir_y_no;		//Y��Գƽڵ���(���ҶԳ�)
	int mir_z_no;		//Z��Գƽڵ���(Z��Գ�)
	long handle;		//���
	int leg_i;			//��λ��Ϊ����Ÿ�λ��ʮλΪ���Ⱥ�
	BOOL bFather;		//�Ƿ�Ϊ������
	int father_no;		//���������
};
struct BODYLEGINFO_STRU
{
	int Tm1,Tm2,Tm3,MAX,iBody,Var;
};
struct LEG_MAP_BODY_STRU// : public fTowerAtom;
{
	short int iLeg,iBody;
	LEG_MAP_BODY_STRU(){iLeg=iBody=1;}
};


class CImportTxtTa  
{
public:
	CImportTxtTa();
	virtual ~CImportTxtTa();
	void Empty();
public:
	ATOM_LIST<LEG_MAP_BODY_STRU>LegMapBody;
	BODYLEGINFO_STRU *BodyLegInfo;
	int m_nBodyNum,m_nLegNum;
	int node_arr_n,jg_arr_n;
	CArray<NODE_STRU,NODE_STRU&>node_arr;
	CArray<JG_STRU,JG_STRU&>jg_arr;
	CTower *m_pTa;
private:
	CTMANode* GetCanEndExtendNode(double wing_wide,double wing_thick,CTMANode* pStartNode,CTMANode* pEndNode);
	CTMANode* GetCanStartExtendNode(double wing_wide,double wing_thick,CTMANode* pStartNode,CTMANode* pEndNode);
	BOOL ImportJgNode(char *filepath,CProcBarDlg *pProcDlg);
	void ImportJgInfo(char *filepath,CProcBarDlg *pProcDlg);
	//��������ĽǸֽڵ���Ϣ���������еĽǸֽڵ㹹����
	NODE_STRU *FindNodeByNo(int No);
	void MirJg(JG_STRU jg,JG_STRU jg_arr[4]);
	void MirNode(NODE_STRU node,NODE_STRU node_arr[4]);
	JG_STRU *FindJgByStartEndNo(int start_no,int end_no);
	JG_STRU *FindJgByNo(int no);
	CFGWORD GetBodyWord(int iBody);
	CFGWORD CalNodeLegDWORD(CTMANode *pNode);
public:
	CLDSView* m_pView;
	long GetFatherJgHandle(CTMANode *pNode);
	void GetLsInfo(CConnectInfo *ls_group,int start_no, int end_no);
	BOOL ImportTxtFile(CTower *pTa,char *pathname,CView *pView);
};

#endif // !defined(AFX_IMPORTTXTTA_H__7CCD8B73_14FE_4775_88DC_04CD6F914DE7__INCLUDED_)
