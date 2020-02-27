// DefinedView.h: interface for the CDefinedView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFINEDVIEW_H__C52F8086_2951_4978_818C_AC7E9ADC3D0F__INCLUDED_)
#define AFX_DEFINEDVIEW_H__C52F8086_2951_4978_818C_AC7E9ADC3D0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HashTable.h"
#include "lds_part.h"

#define VP_DRAWFRONTPLATE			0x01
#define VP_DRAWFRONTPLATESECTION	0x02
#define VP_NOTDISPLAYFRONT			0xfc
#define VP_DRAWSIDEPLATE			0x04
#define VP_DRAWSIDEPLATESECTION		0x08
#define VP_NOTDISPLAYSIDE			0xf3
#define VP_DRAWSECTIONPLATE			0x10
#define VP_DRAWSECTIONPLATESECTION	0x20
#define VP_NOTDISPLAYSECTION		0xcf
#define VP_DRAWWINGX				0x40
#define VP_DRAWWINGY				0xbf

//绘图辅助 移到lds_part.h
//class CDrawingAssistant

class CDisplayView;
class CViewPart  
{
	IModel *m_pModel;
	CLDSPart *pPart;			//临时使用不存储
	CDrawingAssistant *pAssist;	//临时使用不存储
public:
	CDisplayView *pDisplayView;	//临时使用不存储
	char cFlag;
	int idClassType; 
	long handle;
	//构件为角钢时启用 展开线偏移距离,以楞线为基准 wht 10-12-10
	int nSpreadLineOffsetDist;	
	int iResidePlaneNo;	//所在绘图平面序列号
	//
	void SetBelongModel(IModel *pModel){m_pModel=pModel;}
	IModel* BelongModel(){return m_pModel;}
	CLDSPart *GetPart();
	ITAG *pTagInfo; //临时使用不存储，可省去每次遍历构件链表时重复查找当前构件指针 wht 09-12-12
	void SetTagInfo(ITAG* pTag){pTagInfo=pTag;}
	void SetKey(DWORD key){handle=key;}
	//获得当前绘制标志字符串以及索引值
	void GetCurDrawFlagStr(char *sCurFlag,int &iDrawIndex);
	//获得用来填充绘图标志下拉框的字符串以及标志字符串("XYZSQ")
	int GetDrawFlagCmbStrList(CString &sCmbList,char *sDrawFlag);
	//CDrawingAssistant *GetRotateAxisAndAngle(CTower *pTower,f3dPoint draw_norm,f3dLine &rotate_axis,double &rotate_angle);
	CViewPart()
	{
		pPart=NULL; 
		nSpreadLineOffsetDist=0; 
		handle=idClassType=cFlag=0; 
		iResidePlaneNo=-1;
		pDisplayView=NULL;
		pAssist=NULL;
		m_pModel=NULL;
		pTagInfo=NULL;
	}
	void CopyFrom(const CViewPart& item)
	{
		cFlag=item.cFlag;
		idClassType=item.idClassType; 
		handle=item.handle;
		pPart=NULL;
		nSpreadLineOffsetDist=item.nSpreadLineOffsetDist;	
		iResidePlaneNo=item.iResidePlaneNo;	//所在绘图平面序列号
	}
	CViewPart(long h,int classTypeId)
	{
		handle=h;
		cFlag=0;
		idClassType=classTypeId;
		pPart=NULL;
		iResidePlaneNo=-1;
		pDisplayView=NULL;
		pAssist=NULL;
	}
};

class CLayerFilter  
{
public:
	CLayerFilter();
	virtual ~CLayerFilter(){;}
public:
	char FirstCharSet[16];
	char SecondCharSet[16]; 
	char ThirdCharSet[16];
	void SetFilter(char *firstCh,char *secondCh,char *thirdCh);
	//layer中可能包含'?'等通用匹配字符
	BOOL IsValidLayer(char *layer);
};

class CDisplayView : public CLDSObject
{
	SEGI m_iSeg;	//段号 不存储 wht 10-12-27
	CHashTable<CViewPart*>partsetTable;
	CHashTable<CDrawingAssistant*>assistHashTable;
	CHashListEx<CViewPart> partset;				//显示控件
public:
	struct ITEM_TAG_INFO : public TAG_INFO<CViewPart>{
	public:
		//DRAWFLAG cDrawMode;
		CLDSPart *pCurPart;
	public:
		ITEM_TAG_INFO();
		virtual ~ITEM_TAG_INFO(){DetachObject();}
	};
	//当PART_ITEM已经有挂接对象时执行TagInfo(...), 否则创建新挂接数据对象后返回
	ITEM_TAG_INFO* LoadTagInfo(CViewPart* pItem);
private:
	ATOM_LIST<ITEM_TAG_INFO>listTagInfo;	//临时挂接的附加到构件项{PART_ITEM}上的数据对象链表
	int m_idSubViewType;	//0.普通;1.V面视图
public:
	static const int SUBVIEW_COMMON = 0;	//普通视图
	static const int SUBVIEW_LIFTARM= 1;	//含预拱的横担视图(需处理单线图）
	static const int SUBVIEW_HSECT	= 2;	//横隔面视图（应处理四根杆件截面图及横材上贴合立板的截面图）
	static const int SUBVIEW_VFACE	= 3;	//V面视图(应处理V型腿部斜材的展开面及展开线设定）
	bool IsValidSubViewType(int idSubView);
	bool IntelliUpdate(int idSubView);
public:
	int m_iViewType;	//0:表示图层视图 1:表示定制视图 2:组合构件附带视图，该类视图不保存只用来显示
	char defaultLayer[4];
	char folderName[17];	//用于显示定制视图时分类显示，比如段号名
	char name[51];			//视图名称
	UCS_STRU ucs;
	CParaCS des_cs;
	CLayerFilter filter;	//要显示的集合
	ATOM_LIST<CDrawingAssistant> listAssist;	//绘图辅助列表
	//
	CDisplayView();
	int SubViewType(){return m_idSubViewType;}
	void InitHashTable();
	BOOL IsValidObject(CLDSDbObject *pObj);
	CViewPart *FindViewPart(long h);
	CDrawingAssistant *GetDrawingAssistByNo(int iNo,SEGI iSeg=SEGI());
	CDrawingAssistant *GetDrawingAssistByHandle(long h,SEGI iSeg=SEGI());
	int GetSpreadFaceCount(int iSeg=0);
	BOOL SelectDispSet(NODESET& DispNodeSet,PARTSET& DispPartSet,BLOCKREFSET& DispBlockRefSet,FOUNDATIONSET &DispFoundationSet);			//根据条件选择要显示的集合
	SEGI SetSegI(SEGI segI){m_iSeg=segI;return segI;}
	SEGI GetSegI();
	void BackupTo(CDisplayView& backup);
	void RestoreFrom(CDisplayView& backup);

	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromCdfBuffer(CBuffer &buffer,long version=0);
	void ToCdfBuffer(CBuffer &buffer,long version=0);	
	void ToXmlFile(FILE* fp,DWORD schema);
	//
	CViewPart *AppendViewPart(long h);
	CViewPart *AppendViewPart(const CViewPart &viewPart);
	CViewPart *GetFirstViewPart(){return partset.GetFirst();}
	CViewPart *GetNextViewPart(){return partset.GetNext();}
	int GetViewPartCount(){return partset.GetNodeNum();}
	void EmptyPartSet(){partset.Empty();}
	BOOL DeletCursorViewPart(BOOL bClean=FALSE){return partset.DeleteCursor(bClean);}
	void CleanPartSet(){partset.Clean();}
};
CDisplayView::ITEM_TAG_INFO* TagInfo(CViewPart* pItem);
CLDSPart* TagPart(CViewPart* pItem);

#endif // !defined(AFX_DEFINEDVIEW_H__C52F8086_2951_4978_818C_AC7E9ADC3D0F__INCLUDED_)
