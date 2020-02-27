#pragma once
#include "f_ent.h"
#include "f_ent_list.h"
#include "HashTable.h"
#include "Hash64Table.h"
#include "XhCharString.h"
#include "Variant.h"
#include "LogFile.h"
#include "ArrayList.h"
#include "StdArx.h"

//���տ���ʶ�����
class CIdentifyManager
{
private:
	static double fMaxX,fMaxY,fMinX,fMinY;
	static f3dPoint partNoPt;
	static double fPartNoFontSize;
	static CXhChar500 sJgCardPath;
	static CHashList<f2dRect> hashRectByItemType;
public:
	CIdentifyManager();
	~CIdentifyManager();
	static bool InitJgCardInfo(const char* sFileName);
	static CXhChar500 GetJgCardPath(){return sJgCardPath;}
	static f2dRect* GetItemRect(long itemType){return hashRectByItemType.GetValue(itemType+1);}
	static f3dPoint GetLeftBtmPt(){return f3dPoint(fMinX,fMinY,0);}
	static f3dPoint GetLeftTopPt(){return f3dPoint(fMinX,fMaxY,0);}
	static f3dPoint GetRightBtmPt(){return f3dPoint(fMaxX,fMinY,0);}
	static f3dPoint GetRightTopPt(){return f3dPoint(fMaxX,fMaxY);}
	static f3dPoint GetPartnoPt(){return partNoPt;}
	static double GetPartNoFontSize(){return fPartNoFontSize;}
};
//
class CAngleProcessInfo
{
private:
	POLYGON region;
	SCOPE_STRU scope;
public:
	static const BYTE TYPE_JG	 = 1;	//�Ǹ�
	static const BYTE TYPE_YG	 = 2;	//Բ��
	static const BYTE TYPE_TUBE	 = 3;	//�ֹ�
	static const BYTE TYPE_FLAT	 = 4;	//����
	static const BYTE TYPE_JIG	 = 5;	//�о�
	static const BYTE TYPE_GGS	 = 6;	//�ָ�դ
	BYTE m_ciType;				//
	CXhChar16 m_sPartNo;		//������
	CXhChar50 m_sSpec;			//���
	char m_cMaterial;			//����
	int m_nWidth;				//���
	int m_nThick;				//���
	int m_nLength;				//����
	int m_nSumNum;				//�ӹ���
	//����
	int m_nM12;					//M12����
	int m_nM16;					//M16����
	int m_nM18;					//M18����
	int m_nM20;					//M20����
	int m_nM22;					//M22����
	int m_nM24;					//M24����
	int m_nSumHoleCount;		//�ܿ���
	//����
	CXhChar100 m_sTowerType;
	//��ע
	CXhChar200 m_sNotes;		
	//�Ǹֹ���
	BOOL m_bCutRoot;			//�ٸ�
	BOOL m_bCutBer;				//����
	BOOL m_bMakBend;			//����
	BOOL m_bPushFlat;			//ѹ��
	BOOL m_bCutEdge;			//����
	BOOL m_bRollEdge;			//���
	BOOL m_bWeld;				//����
	BOOL m_bKaiJiao;			//����
	BOOL m_bHeJiao;				//�Ͻ�
	BOOL m_bCutAngle;			//�н�
	//
	BOOL m_bInBlockRef;			//�Ƿ��ڹ��տ�����
	AcDbObjectId keyId;
	f3dPoint sign_pt,orig_pt;
	double m_fScale2Template;	//��ǰ���տ���ģ��֮��ı���
	CXhChar100 m_sCardPngFile;
	CXhChar500 m_sCardPngFilePath;	//���տ�ͼƬ·��
	BOOL m_bUploadToServer;
	BOOL m_bUpdatePng;
	int m_iBatchNo;	//��¼�ڼ�����ȡ,ͻ����ʾ��ǰ���¹���
public:
	CAngleProcessInfo();
	~CAngleProcessInfo();
	//
	void InitOrig();
	void CreateRgn(ARRAY_LIST<f3dPoint>& vertexList);
	bool PtInAngleRgn(const double* poscoord);
	void InitAngleInfo(f3dPoint data_pos,const char* sValue);
	f2dRect GetAngleDataRect(BYTE data_type);
	f3dPoint GetAngleDataPos(BYTE data_type);
	bool PtInDataRect(BYTE data_type,f3dPoint pt);
	void CopyProperty(CAngleProcessInfo* pJg);
	SCOPE_STRU GetCADEntScope(){return scope;}
	CXhChar50 GetPartName() const;
};
class CBPSModel
{
	CHash64List<CAngleProcessInfo> m_hashJgInfo;
protected:
	CAngleProcessInfo* FindAngleByPt(f3dPoint data_pos);
	CAngleProcessInfo* FindAngleByPartNo(const char* sPartNo);
public:
	int m_idProject;
	int m_idManuTask;
	int m_idTowerType;
	int m_iRetrieveBatchNo;	//��ȡ���κ� ÿ��ȡһ�����κ�+1��������ȡʱ��1��ʼ
public:
	CBPSModel(void);
	~CBPSModel(void);
	//
	void CorrectAngles();
	void RetrieveJgCardRegion(CHashSet<AcDbObjectId>& selPolyLineIdHash);
	BOOL RetrieveAngleInfo(CHashSet<AcDbObjectId>& selTextIdSet);
	void Empty()
	{
		m_iRetrieveBatchNo=1;
		return m_hashJgInfo.Empty();
	}
	CAngleProcessInfo* AppendJgInfo(UINT_PTR hKey)
	{
		CAngleProcessInfo *pJgInfo=m_hashJgInfo.Add(hKey);
		if(pJgInfo)
			pJgInfo->m_iBatchNo=m_iRetrieveBatchNo;
		return pJgInfo;
	}
	CAngleProcessInfo* EnumFirstJg(){return m_hashJgInfo.GetFirst();}
	CAngleProcessInfo* EnumNextJg(){return m_hashJgInfo.GetNext();}
	BOOL DeleteJgInfo(long hKey){return m_hashJgInfo.DeleteNode(hKey);}
	int GetJgNum(){return m_hashJgInfo.GetNodeNum();}
	//
	static char QueryBriefMatMark(const char* sMatMark);
	static CXhChar16 QuerySteelMatMark(char cMat);
	static int QuerySteelMatIndex(char cMat);
	static void RestoreSpec(const char* spec,int *width,int *thick,char *matStr=NULL);
};
extern CBPSModel BPSModel;