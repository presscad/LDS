#pragma once

#ifdef __COMPLEX_PART_INC_
#include ".\StdComTempl\StdTemplPropertyList.h"
#include ".\StdComTempl\TubeWalkWayTempl.h"
class CWalkWayPropertyList :public CStdComTemplPropertyList
{
public://protected:
	long m_hBaseTube,m_hBaseNode;	//防止undo时取消节点导致指针失效 wjh-2018.10.8
	long m_hOuterBeamDatumNode,m_hInnerBeamDatumNode;
	long m_hOuterAngle,m_hInnerAngle;
public:
	CTubeWalkWayStdComTempl m_xWalkWayTempl;
	struct WALKWAY_PARAMS{
		BYTE m_ciWalkWayType;		//0.钢板支架走道|1.三角支架走道|2.台型支架走道
		BOOL m_bCreateWalkWay;		//是否生成走道角钢
		double m_fYA;				//基准钢管到走道外侧角钢的横向投影距离
		double m_fZH;				//基准钢管到走道角钢平面的竖向投影距离
		double m_fWidth;			//走道宽度
		double m_fLength;			//走道长度
		double m_fZh;				//斜撑下侧竖向偏移
		double m_fD1;				//斜撑下侧横向偏移
		double m_fD2;				//斜撑上侧端点沿水平杆向内的横向偏移
		double m_fYB;				//竖撑横向偏移距
		double m_fBoltSpace;		//螺栓间距
		int m_nBoltMd;				//螺栓直径
		int m_nBoltNum;				//螺栓数量
		CXhChar16 m_sWalkWayJgSpec;	//走道角钢规格
		CXhChar16 m_sHolderJgSpec;	//支架角钢规格
		char m_cWalkWayJgMat;		//支架角钢材质
		char m_cHolderJgMat;		//支架角钢材质
		char m_ciHolderAngleLayoutStyle;	//横撑角钢朝向：0.朝外;1.朝内
		char m_ciOtherAngleLayoutStyle;		//斜撑角钢朝向：0.朝外;1.朝内
		char m_cPlateMat;			//钢板材质
		int m_nThick;				//钢板厚度
		struct PLATE_PARA{
			double A;	//走道焊接板扇形夹角
			double B;	//螺栓连接板扇形夹角
			double H;	//台口高度
			double W;	//
		}para;
		//
		CLDSLineTube* m_pBaseTube;
		CLDSNode* m_pBaseNode;
		CLDSLineAngle* m_pOuterJg;	//走道外侧角钢
		CLDSLineAngle* m_pInnerJg;	//走道内侧角钢
		double m_fZjOffDist;		//支架位置偏移距离，对应界面<X0>参数
		double m_fZdOffDist;		//走道始端偏移距离，对应界面<X1>参数
	}params;
protected:
	virtual CStdComTempl* _InternalGetStdComTempl(){return &m_xWalkWayTempl;}
	void UpdateTemplParams();
	bool UnifyParaAngleLayoutByAntiAxisX(CStdTemplParaRod* pParaRod);
	GEPOINT CalNearVecX();
public:
	CWalkWayPropertyList(void);
	virtual ~CWalkWayPropertyList(void);
	void ResetParams();
	void CopyFrom(CTubeWalkWayStdComTempl* pWalkWayTempl);
	void CopyTo(CTubeWalkWayStdComTempl* pWalkWayTempl);
	bool VerifyRelaRodAndNodes();
	virtual long InitPropHashtable(long initId=1,long initHashSize=50,long initStatusHashSize=50);	//返回下一空闲Id
	virtual long GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);
	virtual bool ButtonClick(CPropTreeItem* pItem,BOOL *lpRslt);
	virtual bool ModifyValueFunc(CPropTreeItem* pItem,CString& valueStr,BOOL *lpRslt);
	virtual BOOL HelpPromptProperty(CPropTreeItem* pItem);
	virtual bool CallBackCommand(UINT message,WPARAM wParam,BOOL *lpRslt);
	virtual BOOL FinishSelectObjOper(long hPickObj,long idEventProp);
	virtual void DisplayPropertyList(int overwrite0_append1=0,DWORD dwPropGroup=1);
	virtual bool DisplaySlavePropertyList(CPropTreeItem* pItem,DWORD dwRelaInfo=0);
	virtual void WorkPlaneSketch();
	virtual void DelWorkPlaneSketch();
	virtual void UpdateSketchMap();
	virtual bool CreateComTemplInstance();
	virtual bool InitializeUI(bool bInternalStart=false,DWORD dwParam=0);
	virtual bool ConfirmOK();
	virtual bool CancelUI();
	void UpdateAssemblyProp(CPropTreeItem* pParentItem);
	void UpdateWalkWayProp(CPropTreeItem* pParentItem);
	void UpdateHolderProp(CPropTreeItem* pParentItem);
};
#endif
