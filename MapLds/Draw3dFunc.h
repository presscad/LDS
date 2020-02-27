#pragma once
#include "RxTools.h"
#include "ProcessPart.h"
#include "LogFile.h"
#include "..\\StdPartSolid\\Fitting.h"
#ifdef __RAPID_ROUGH_3DMODEL_
#include "TidCplus.h"
#endif

extern CLogFile xMyErrLog;
extern void OpenTaFile(CString sFilePath, BOOL bPrompt, BOOL bReadAdditionalBuffer);
extern void CloseTaFile(BOOL bPrompt, BOOL bClearTa);
extern void DisplayProcess(int percent, const char *sTitle);
//////////////////////////////////////////////////////////////////////////
#ifdef __RAPID_ROUGH_3DMODEL_
//�������Ƹ�����λʵ��ģ��
//CDrawModel����(����߶����)
class CDrawManager;
extern CDrawManager gDrawManager;
class CDrawModel
{
	CDrawManager* m_pDrawManager;
public:
	CXhChar100 m_sFilePath;
	CXhChar50 m_sTowerPlaceCode;	//��λ��
	CXhChar50 m_sTowerDot;	//��λ��
	CXhChar50 m_sTowerType;	//����
	CXhChar50 m_sHuGao;		//����
	double m_fLocateH;		//��λ��
	double m_fBodyHeight;	//�����
	double m_fMaxLegHeight;	//��ȸ�
	BOOL m_bTurnLeft;		//�Ƿ���ת�������������ʾ����ϵ������
	UINT m_uiHeightSerial;
	UINT m_uiLegSerialArr[4];
	//�����ļ��е�ƽ��
	char m_cLegMark;
	CXhChar16 m_sLegDes;
	//����ģ��
	BYTE m_ciModelFlag;	//0.TID|1.LDS
	static const int TYPE_TID = 0;
	static const int TYPE_LDS = 1;
	//
	BYTE m_ciErrorType;
	static const BYTE ERROR_NO_FILE = 1;	//�Ҳ����ļ�
	static const BYTE ERROR_READ_FILE = 2;	//�ļ���ȡʧ��
	static const BYTE ERROR_FIND_HUGAO = 3;	//�Ҳ�������
	static const BYTE ERROR_FIND_LEG = 4;	//�Ҳ�������
	static const BYTE ERROR_FIND_INSTANCE = 5;	//�Ҳ�������
private:
	double CalTowerHeight(void* pInstance = NULL);
public:
	CDrawModel();
	~CDrawModel();
	//
	void SetManager(CDrawManager* pManager) { m_pDrawManager = pManager; }
	CDrawManager* BelongManager() { return m_pDrawManager; }
	void* ExtractActiveTaInstance();
	BOOL ExtractActiveTaInstance(char* sHuGao, double fLegA, double fLegB, double fLegC, double fLegD);
	CXhChar100 GetFileName(void* pInstance);
	GECS BuildTransCS(void* pInstance = NULL);
};
//CDrawManager
class CDrawManager
{
private:
	ATOM_LIST<CDrawModel> m_xDrawModelList;
public:
	CDrawModel* m_pActiveTa;
	ITidModel* m_pTidModel;
	BYTE m_ciInstanceLevel;	//0.LOD0����|1.LOD1�˼�|2.LOD2�˼�+��|3.LOD3����ģ��
	BOOL m_bUseUnitM;		//�Ƿ����ó��ȵ�λ�ף�Ĭ��Ϊ���ף�
public:
	CDrawManager();
	~CDrawManager();
	void Empty();
	//
	CDrawModel* EnumFirstTaModel() { return m_xDrawModelList.GetFirst(); }
	CDrawModel* EnumNextTaModel() { return m_xDrawModelList.GetNext(); }
	int GetTaModelNum() { return m_xDrawModelList.GetNodeNum(); }
	//
	BOOL ReadTidFile(const char* sFileName);
	BOOL ReadLdsFile(const char* sFileName);
	BOOL ReadExcleFile(const char* sFileName);
	BOOL ReadTxtFile(const char* sFileName);
};
#endif
//////////////////////////////////////////////////////////////////////////
//��˨������Ϣ
struct LS_HOLE_INFO {
	long key;
	int bolt_d;		//
	double bolt_L0;	//
	double bolt_L;	//
	BOOL bTwoCap;
	double bolt_LR;	//����ػ�����
	double bolt_PT;	//����
	double bolt_PW;	//����
	LS_HOLE_INFO() { memset(this, 0, sizeof(LS_HOLE_INFO)); }
};
//////////////////////////////////////////////////////////////////////////
//����TMA�й�����άʵ��
AcDbObjectId Draw1FacePlate(CLDSGeneralPlate* pPlate, UCS_STRU* pUcs = NULL, double scale = 1,
							COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);

AcDbObjectId Draw2FacePlate(CLDSGeneralPlate* pPlate, UCS_STRU* pUcs = NULL, double scale = 1,
							COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);
//������ͨ�հ�ʵ��
AcDbObjectId DrawSolidPlate(CLDSPlate *pPlate, UCS_STRU* ucs = NULL, double scale = 1,
						COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);
//���Ʋ������հ�ʵ��
AcDbObjectId DrawSolidParamPlate(CLDSParamPlate *pPlate, UCS_STRU* pUcs = NULL, double scale = 1,
						COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);
//���ƽǸ�ʵ��
AcDbObjectId DrawSolidAngle(CLDSLineAngle *pLinkAngle, UCS_STRU* pUcs = NULL, double scale = 1,
						COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);
//���Ƹֹ�ʵ��
AcDbObjectId DrawSolidTube(CLDSLineTube *pLineTube, UCS_STRU* pUcs = NULL, double scale = 1,
						COLORREF* pColor = NULL, AcGeMatrix3d *pMatrix = NULL);
//���Ʊ���ʵ��
AcDbObjectId DrawSolidFlat(CLDSLineFlat *pLineFlat, UCS_STRU* pUcs = NULL, double scale = 1,
						COLORREF* pColor = NULL, AcGeMatrix3d *pMatrix = NULL);
//���Ʋ۸�ʵ��
AcDbObjectId DrawSolidSlot(CLDSLineSlot *pLineSlot, UCS_STRU* pUcs = NULL, double scale = 1,
						COLORREF* pColor = NULL, AcGeMatrix3d *pMatrix = NULL);
//���ƽ��ʵ��
AcDbObjectId DrawSolidFitting(CLDSFitting *pFitting, UCS_STRU* pUcs = NULL, double scale = 1,
						COLORREF* pColor = NULL, AcGeMatrix3d *pMatrix = NULL);
AcDbObjectId DrawEBSolid(FITTING_EB* pInfo, COLORREF* pCrSolid = NULL);
AcDbObjectId DrawGDSolid(FITTING_GD* pInfo, COLORREF* pCrSolid = NULL);
AcDbObjectId DrawURSolid(FITTING_UR* pInfo, COLORREF* pCrSolid = NULL);
//������˨ʵ�壨��ͨ��˨+�ؽ���˨��
AcDbObjectId DrawSolidBolt(CLDSBolt *pBolt, UCS_STRU* pUcs = NULL, double scale = 1,
						COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);
//������ͨ��˨��άʵ��
AcDbObjectId DrawBoltSolid(LS_HOLE_INFO& bolt_info, UCS_STRU assemble_ucs, UCS_STRU* pUcs = NULL, double scale = 1,
						COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);
//���Ƶؽ���˨��άʵ��
AcDbObjectId DrawSolidAnchorBolt(LS_HOLE_INFO& bolt_info, UCS_STRU assemble_ucs, UCS_STRU* pUcs = NULL, double scale = 1,
							COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);
//����ѡ�и˼�����ά����ͼ
void Draw3DLineMap(UCS_STRU ucs, double scale = 1, BOOL bTurnLeft = FALSE);
void Draw3DLineMap(PARTSET& partset, UCS_STRU ucs, double scale = 1,BOOL bTurnLeft = FALSE);
//����ѡ�и˼�����άʵ��ͼ
void Draw3DSolid(UCS_STRU ucs, BOOL bPrompt, double scale = 1, BYTE ciModelLevel=3,BOOL bTurnLeft = FALSE);
void Draw3DSolid(PARTSET& partset,UCS_STRU ucs, BOOL bPrompt, double scale = 1, BYTE ciModelLevel = 3,BOOL bTurnLeft = FALSE);
void DrawBlockSolid(UCS_STRU ucs, BOOL bPrompt, double scale = 1, BYTE ciModelLevel = 3,BOOL bTurnLeft=FALSE);
///////////////////////////////////////////////////////////////////////////
//����TID�й���ʵ��
//���ƽǸ���άʵ��
AcDbObjectId DrawSolidAngle(CProcessAngle *pJg, UCS_STRU assemble_ucs, UCS_STRU* pUcs = NULL, double scale = 1,
						COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);
//���Ƹְ���άʵ��
AcDbObjectId DrawSolidPlate(CProcessPlate *pPlate, UCS_STRU assemble_ucs, UCS_STRU* ucs = NULL, double scale = 1,
						COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);
#ifdef __RAPID_ROUGH_3DMODEL_
//����TID������ά����
void DrawTid3DLine(ITidTowerInstance* pTowerInstance,UCS_STRU ucs,double scale = 1,BOOL bTurnLeft=FALSE);
//����TID������άʵ��
void DrawTid3DSolid(ITidTowerInstance* pTowerInstance, UCS_STRU ucs, BOOL bPrompt, double scale = 1, BYTE ciModelLevel = 3,
						BOOL bTurnLeft = FALSE);
#endif