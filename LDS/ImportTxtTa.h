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
	int No;			//节点编号
	int cPosCalType;//依附类型
	int attach_no1,attach_no2;	//依附节点编号
	double attach_para;	//依附参数(等分数量或偏移量)
	int iMirMsg;		//对称信息
	f3dPoint pt;		//节点坐标
	int mir_x_no;		//X轴对称节点编号(前后对称)
	int mir_y_no;		//Y轴对称节点编号(左右对称)
	int mir_z_no;		//Z轴对称节点编号(Z轴对称)
	long handle;		//句柄
	int leg_i;			//百位数为本体号个位与十位为接腿号
	int pt_type;		//节点类型(0.自由节点 1.约束节点)
	BOOL bFather;		//是否为父构件
	int father_no;		//父构件编号
};
struct JG_STRU
{
	int  No;			//角钢编号
	int  start_no,end_no;	//起止节点号
	int  iMirMsg;		//对称信息
	int  type;			//角钢类型
	BOOL bForceJg;		//是否为受力材（否则为补助材）
	double wing_wide,wing_thick;		//角钢规格
	char cMaterial;		//材质
	char LsGuiGe[24];	//连接螺栓规格
	double ls_grade;	//螺栓级别
	long ls_d;			//螺栓直径
	int LsN;			//每一端螺栓连接数
	int mir_x_no;		//X轴对称节点编号(前后对称)
	int mir_y_no;		//Y轴对称节点编号(左右对称)
	int mir_z_no;		//Z轴对称节点编号(Z轴对称)
	long handle;		//句柄
	int leg_i;			//百位数为本体号个位与十位为接腿号
	BOOL bFather;		//是否为父构件
	int father_no;		//父构件编号
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
	//根据输入的角钢节点信息来构造所有的角钢节点构件库
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
