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

//��ͼ���� �Ƶ�lds_part.h
//class CDrawingAssistant

class CDisplayView;
class CViewPart  
{
	IModel *m_pModel;
	CLDSPart *pPart;			//��ʱʹ�ò��洢
	CDrawingAssistant *pAssist;	//��ʱʹ�ò��洢
public:
	CDisplayView *pDisplayView;	//��ʱʹ�ò��洢
	char cFlag;
	int idClassType; 
	long handle;
	//����Ϊ�Ǹ�ʱ���� չ����ƫ�ƾ���,������Ϊ��׼ wht 10-12-10
	int nSpreadLineOffsetDist;	
	int iResidePlaneNo;	//���ڻ�ͼƽ�����к�
	//
	void SetBelongModel(IModel *pModel){m_pModel=pModel;}
	IModel* BelongModel(){return m_pModel;}
	CLDSPart *GetPart();
	ITAG *pTagInfo; //��ʱʹ�ò��洢����ʡȥÿ�α�����������ʱ�ظ����ҵ�ǰ����ָ�� wht 09-12-12
	void SetTagInfo(ITAG* pTag){pTagInfo=pTag;}
	void SetKey(DWORD key){handle=key;}
	//��õ�ǰ���Ʊ�־�ַ����Լ�����ֵ
	void GetCurDrawFlagStr(char *sCurFlag,int &iDrawIndex);
	//�����������ͼ��־��������ַ����Լ���־�ַ���("XYZSQ")
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
		iResidePlaneNo=item.iResidePlaneNo;	//���ڻ�ͼƽ�����к�
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
	//layer�п��ܰ���'?'��ͨ��ƥ���ַ�
	BOOL IsValidLayer(char *layer);
};

class CDisplayView : public CLDSObject
{
	SEGI m_iSeg;	//�κ� ���洢 wht 10-12-27
	CHashTable<CViewPart*>partsetTable;
	CHashTable<CDrawingAssistant*>assistHashTable;
	CHashListEx<CViewPart> partset;				//��ʾ�ؼ�
public:
	struct ITEM_TAG_INFO : public TAG_INFO<CViewPart>{
	public:
		//DRAWFLAG cDrawMode;
		CLDSPart *pCurPart;
	public:
		ITEM_TAG_INFO();
		virtual ~ITEM_TAG_INFO(){DetachObject();}
	};
	//��PART_ITEM�Ѿ��йҽӶ���ʱִ��TagInfo(...), ���򴴽��¹ҽ����ݶ���󷵻�
	ITEM_TAG_INFO* LoadTagInfo(CViewPart* pItem);
private:
	ATOM_LIST<ITEM_TAG_INFO>listTagInfo;	//��ʱ�ҽӵĸ��ӵ�������{PART_ITEM}�ϵ����ݶ�������
	int m_idSubViewType;	//0.��ͨ;1.V����ͼ
public:
	static const int SUBVIEW_COMMON = 0;	//��ͨ��ͼ
	static const int SUBVIEW_LIFTARM= 1;	//��Ԥ���ĺᵣ��ͼ(�账����ͼ��
	static const int SUBVIEW_HSECT	= 2;	//�������ͼ��Ӧ�����ĸ��˼�����ͼ���������������Ľ���ͼ��
	static const int SUBVIEW_VFACE	= 3;	//V����ͼ(Ӧ����V���Ȳ�б�ĵ�չ���漰չ�����趨��
	bool IsValidSubViewType(int idSubView);
	bool IntelliUpdate(int idSubView);
public:
	int m_iViewType;	//0:��ʾͼ����ͼ 1:��ʾ������ͼ 2:��Ϲ���������ͼ��������ͼ������ֻ������ʾ
	char defaultLayer[4];
	char folderName[17];	//������ʾ������ͼʱ������ʾ������κ���
	char name[51];			//��ͼ����
	UCS_STRU ucs;
	CParaCS des_cs;
	CLayerFilter filter;	//Ҫ��ʾ�ļ���
	ATOM_LIST<CDrawingAssistant> listAssist;	//��ͼ�����б�
	//
	CDisplayView();
	int SubViewType(){return m_idSubViewType;}
	void InitHashTable();
	BOOL IsValidObject(CLDSDbObject *pObj);
	CViewPart *FindViewPart(long h);
	CDrawingAssistant *GetDrawingAssistByNo(int iNo,SEGI iSeg=SEGI());
	CDrawingAssistant *GetDrawingAssistByHandle(long h,SEGI iSeg=SEGI());
	int GetSpreadFaceCount(int iSeg=0);
	BOOL SelectDispSet(NODESET& DispNodeSet,PARTSET& DispPartSet,BLOCKREFSET& DispBlockRefSet,FOUNDATIONSET &DispFoundationSet);			//��������ѡ��Ҫ��ʾ�ļ���
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
