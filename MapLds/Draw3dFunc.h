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
//批量绘制杆塔三位实体模型
//CDrawModel塔型(多呼高多接腿)
class CDrawManager;
extern CDrawManager gDrawManager;
class CDrawModel
{
	CDrawManager* m_pDrawManager;
public:
	CXhChar100 m_sFilePath;
	CXhChar50 m_sTowerPlaceCode;	//塔位号
	CXhChar50 m_sTowerDot;	//塔位点
	CXhChar50 m_sTowerType;	//塔型
	CXhChar50 m_sHuGao;		//胡高
	double m_fLocateH;		//定位高
	double m_fBodyHeight;	//接身高
	double m_fMaxLegHeight;	//最长腿高
	BOOL m_bTurnLeft;		//是否左转，需调整塔型显示坐标系及配腿
	UINT m_uiHeightSerial;
	UINT m_uiLegSerialArr[4];
	//放样文件中的平腿
	char m_cLegMark;
	CXhChar16 m_sLegDes;
	//数据模型
	BYTE m_ciModelFlag;	//0.TID|1.LDS
	static const int TYPE_TID = 0;
	static const int TYPE_LDS = 1;
	//
	BYTE m_ciErrorType;
	static const BYTE ERROR_NO_FILE = 1;	//找不到文件
	static const BYTE ERROR_READ_FILE = 2;	//文件读取失败
	static const BYTE ERROR_FIND_HUGAO = 3;	//找不到呼高
	static const BYTE ERROR_FIND_LEG = 4;	//找不到配腿
	static const BYTE ERROR_FIND_INSTANCE = 5;	//找不到塔例
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
	BYTE m_ciInstanceLevel;	//0.LOD0单线|1.LOD1杆件|2.LOD2杆件+板|3.LOD3放样模型
	BOOL m_bUseUnitM;		//是否启用长度单位米（默认为毫米）
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
//螺栓数据信息
struct LS_HOLE_INFO {
	long key;
	int bolt_d;		//
	double bolt_L0;	//
	double bolt_L;	//
	BOOL bTwoCap;
	double bolt_LR;	//深入地基长度
	double bolt_PT;	//垫板厚
	double bolt_PW;	//垫板宽
	LS_HOLE_INFO() { memset(this, 0, sizeof(LS_HOLE_INFO)); }
};
//////////////////////////////////////////////////////////////////////////
//绘制TMA中构件三维实体
AcDbObjectId Draw1FacePlate(CLDSGeneralPlate* pPlate, UCS_STRU* pUcs = NULL, double scale = 1,
							COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);

AcDbObjectId Draw2FacePlate(CLDSGeneralPlate* pPlate, UCS_STRU* pUcs = NULL, double scale = 1,
							COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);
//绘制普通刚板实体
AcDbObjectId DrawSolidPlate(CLDSPlate *pPlate, UCS_STRU* ucs = NULL, double scale = 1,
						COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);
//绘制参数化刚板实体
AcDbObjectId DrawSolidParamPlate(CLDSParamPlate *pPlate, UCS_STRU* pUcs = NULL, double scale = 1,
						COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);
//绘制角钢实体
AcDbObjectId DrawSolidAngle(CLDSLineAngle *pLinkAngle, UCS_STRU* pUcs = NULL, double scale = 1,
						COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);
//绘制钢管实体
AcDbObjectId DrawSolidTube(CLDSLineTube *pLineTube, UCS_STRU* pUcs = NULL, double scale = 1,
						COLORREF* pColor = NULL, AcGeMatrix3d *pMatrix = NULL);
//绘制扁铁实体
AcDbObjectId DrawSolidFlat(CLDSLineFlat *pLineFlat, UCS_STRU* pUcs = NULL, double scale = 1,
						COLORREF* pColor = NULL, AcGeMatrix3d *pMatrix = NULL);
//绘制槽钢实体
AcDbObjectId DrawSolidSlot(CLDSLineSlot *pLineSlot, UCS_STRU* pUcs = NULL, double scale = 1,
						COLORREF* pColor = NULL, AcGeMatrix3d *pMatrix = NULL);
//绘制金具实体
AcDbObjectId DrawSolidFitting(CLDSFitting *pFitting, UCS_STRU* pUcs = NULL, double scale = 1,
						COLORREF* pColor = NULL, AcGeMatrix3d *pMatrix = NULL);
AcDbObjectId DrawEBSolid(FITTING_EB* pInfo, COLORREF* pCrSolid = NULL);
AcDbObjectId DrawGDSolid(FITTING_GD* pInfo, COLORREF* pCrSolid = NULL);
AcDbObjectId DrawURSolid(FITTING_UR* pInfo, COLORREF* pCrSolid = NULL);
//绘制螺栓实体（普通螺栓+地脚螺栓）
AcDbObjectId DrawSolidBolt(CLDSBolt *pBolt, UCS_STRU* pUcs = NULL, double scale = 1,
						COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);
//绘制普通螺栓三维实体
AcDbObjectId DrawBoltSolid(LS_HOLE_INFO& bolt_info, UCS_STRU assemble_ucs, UCS_STRU* pUcs = NULL, double scale = 1,
						COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);
//绘制地脚螺栓三维实体
AcDbObjectId DrawSolidAnchorBolt(LS_HOLE_INFO& bolt_info, UCS_STRU assemble_ucs, UCS_STRU* pUcs = NULL, double scale = 1,
							COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);
//绘制选中杆件的三维单线图
void Draw3DLineMap(UCS_STRU ucs, double scale = 1, BOOL bTurnLeft = FALSE);
void Draw3DLineMap(PARTSET& partset, UCS_STRU ucs, double scale = 1,BOOL bTurnLeft = FALSE);
//绘制选中杆件的三维实体图
void Draw3DSolid(UCS_STRU ucs, BOOL bPrompt, double scale = 1, BYTE ciModelLevel=3,BOOL bTurnLeft = FALSE);
void Draw3DSolid(PARTSET& partset,UCS_STRU ucs, BOOL bPrompt, double scale = 1, BYTE ciModelLevel = 3,BOOL bTurnLeft = FALSE);
void DrawBlockSolid(UCS_STRU ucs, BOOL bPrompt, double scale = 1, BYTE ciModelLevel = 3,BOOL bTurnLeft=FALSE);
///////////////////////////////////////////////////////////////////////////
//绘制TID中构件实体
//绘制角钢三维实体
AcDbObjectId DrawSolidAngle(CProcessAngle *pJg, UCS_STRU assemble_ucs, UCS_STRU* pUcs = NULL, double scale = 1,
						COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);
//绘制钢板三维实体
AcDbObjectId DrawSolidPlate(CProcessPlate *pPlate, UCS_STRU assemble_ucs, UCS_STRU* ucs = NULL, double scale = 1,
						COLORREF* pCrSolid = NULL, AcGeMatrix3d *pMatrix = NULL);
#ifdef __RAPID_ROUGH_3DMODEL_
//绘制TID塔例三维单线
void DrawTid3DLine(ITidTowerInstance* pTowerInstance,UCS_STRU ucs,double scale = 1,BOOL bTurnLeft=FALSE);
//绘制TID塔例三维实体
void DrawTid3DSolid(ITidTowerInstance* pTowerInstance, UCS_STRU ucs, BOOL bPrompt, double scale = 1, BYTE ciModelLevel = 3,
						BOOL bTurnLeft = FALSE);
#endif