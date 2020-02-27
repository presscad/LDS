// BomExport.cpp : 定义 DLL 的初始化例程。
#include "stdafx.h"
#include "BomExport.h"
#include "ITowerBom.h"
#include "Buffer.h"
#include "list.h"
#include "math.h"
#include "SortFunc.h"
#include "ToolKit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	注意！
//
//		如果此 DLL 动态链接到 MFC
//		DLL，从此 DLL 导出并
//		调入 MFC 的任何函数在函数的最前面
//		都必须添加 AFX_MANAGE_STATE 宏。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CBomExportApp

BEGIN_MESSAGE_MAP(CBomExportApp, CWinApp)
END_MESSAGE_MAP()


// CBomExportApp 构造

CBomExportApp::CBomExportApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CBomExportApp 对象

CBomExportApp theApp;


// CBomExportApp 初始化

BOOL CBomExportApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
TOWER_BOM_EXPORTS_API DWORD GetSupportDataBufferVersion()
{
	return 10;
}

const int COL_COUNT = 6;
//索引号从1开始，0表示没有对应列
TOWER_BOM_EXPORTS_API int GetExcelFormat(int* colIndexArr, int *startRowIndex)
{
	if (colIndexArr == NULL)
		return COL_COUNT;
	else
	{
		if (startRowIndex != NULL)
			*startRowIndex = 6;
		colIndexArr[0] = 2;	//件号
		colIndexArr[1] = 4;	//规格
		colIndexArr[2] = 5;	//材质
		colIndexArr[3] = 8;	//数量
		colIndexArr[4] = 18;	//单重
		colIndexArr[5] = 6;	//长度
		return 6;
	}
}

#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"

typedef LIST_NODE<BOMPROFILE_VERTEX>* VERTEX_PTR;
static int CompareSegiFunc(const SUPERLIST_NODE<BOMPART>& item1, const SUPERLIST_NODE<BOMPART>& item2)
{
	BOMPART* pBomPart1 = (BOMPART*)item1.pDataObj;
	BOMPART* pBomPart2 = (BOMPART*)item2.pDataObj;
	SEGI seg1(pBomPart1->iSeg), seg2(pBomPart2->iSeg);
	if (seg1.iSeg > seg2.iSeg)
		return 1;
	else if (seg1.iSeg < seg2.iSeg)
		return -1;
	else	//if(seg1.iSeg==seg2.iSeg)
		return ComparePartNoString(pBomPart1->sPartNo, pBomPart2->sPartNo);
}
static void GetAngleSimpleSpec(char *simple_spec, double x_wing_wide, double y_wing_wide, double wide_thick)
{
	char sSpec[200] = "";
	char sWide[100] = "", sThick[100] = "";
	sprintf(sWide, "%s", (char*)CXhChar16(x_wing_wide));
	sprintf(sThick, "%s", (char*)CXhChar16(wide_thick));
	if (IsUnequalAngle(x_wing_wide, y_wing_wide, wide_thick))
	{
		char sWideY[100] = "";
		sprintf(sWideY, "%s", (char*)CXhChar16(y_wing_wide));
		sprintf(sSpec, "L%sx%sx%s", sWide, sWideY, sThick);
	}
	else
		sprintf(sSpec, "L%sx%s", sWide, sThick);
	if (simple_spec)
		strcpy(simple_spec, sSpec);
}
static CXhChar50 GetSpecStr(BOMPART* pBomPart, BOOL bNeedPlateW = FALSE)
{
	CXhChar50 sSpec;
	if (pBomPart->cPartType == BOMPART::ANGLE)
		GetAngleSimpleSpec(sSpec, pBomPart->wide, pBomPart->wingWideY, pBomPart->thick);
	else if (pBomPart->cPartType == BOMPART::TUBE)
		sSpec.Printf("Φ%sx%s", (char*)CXhChar16(pBomPart->wide), (char*)CXhChar16(pBomPart->thick));
	else if (pBomPart->cPartType == BOMPART::SLOT)
	{
		sSpec.Copy(pBomPart->sSpec);
		sSpec.Remove('[');
		sSpec.Remove('C');
	}
	else if (pBomPart->cPartType == BOMPART::PLATE)
	{
		if (pBomPart->siSubType == 0)
		{
			if (bNeedPlateW)
				sSpec.Printf("-%.fx%.f", pBomPart->thick, pBomPart->wide);
			else
				sSpec.Printf("-%.f", pBomPart->thick);
		}
		else
			sSpec.Copy("");
	}
	else
		sSpec.Printf("%s", (char*)pBomPart->sSpec);
	return sSpec;
}

static BOOL IsZhiWan(BOMPART* pBomPart)
{
	if (pBomPart->cPartType == BOMPART::PLATE)
	{
		PART_PLATE* pPlate = (PART_PLATE*)pBomPart;
		int nHuoquCount = pPlate->m_cFaceN - 1;
		if (nHuoquCount > 0)
			return TRUE;	//火曲板
		for (VERTEX_PTR pVertex = pPlate->listVertex.EnumFirst(); pVertex; pVertex = pPlate->listVertex.EnumNext())
		{
			if (pVertex->data.m_bRollEdge)
				return TRUE;	//卷边板
		}
	}
	else if (pBomPart->cPartType == BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle = (PART_ANGLE*)pBomPart;
		if (pBomAngle->bHeJiao || pBomAngle->bKaiJiao)
			return TRUE;	//有开合角
		int nHuoquType = pBomAngle->GetHuoquType();
		if (pBomPart->GetHuoquLineCount() > 0 && nHuoquType >= 1 && nHuoquType <= 4)
			return TRUE;	//制弯角钢
	}
	return FALSE;
}
#define      DEGTORAD_COEF    57.2957795130823208768
CXhChar100 GetProcessParam(BOMPART* pBomPart)
{
	int nNum = 0;
	CXhChar100 sProcessParam(pBomPart->sNotes);
	if (pBomPart->cPartType == BOMPART::PLATE)
	{
		PART_PLATE* pPlate = (PART_PLATE*)pBomPart;
		//垫板
		if (pPlate->siSubType == BOMPART::SUB_TYPE_BOLT_PAD&&pPlate->m_arrBoltRecs.GetSize() == 1)
		{	//单孔螺栓垫板
			CXhChar16 sDianBan("垫板%.f", pBomPart->thick);
			if (pPlate->m_arrBoltRecs[0].d == 16)
				sDianBan.Append("A");
			else if (pPlate->m_arrBoltRecs[0].d == 20)
				sDianBan.Append("B");
			else if (pPlate->m_arrBoltRecs[0].d == 24)
				sDianBan.Append("C");
			sProcessParam.Append(sDianBan, ' ');
			//有孔垫板不需要再进行焊接
			if (pPlate->m_arrBoltRecs.GetSize() > 0)
				pBomPart->bWeldPart = FALSE;
		}
		if (pBomPart->bWeldPart)
			sProcessParam.Append("焊接", ' ');
		if (pPlate->bNeedFillet)
			sProcessParam.Append("坡口", ' ');
		//火曲
		int nHuoquCount = pPlate->m_cFaceN - 1;
		if (nHuoquCount > 0)
		{
			double fHuoquAngle1 = fabs(pPlate->huoQuAngleArr[0]);
			double fHuoquAngle2 = fabs(pPlate->huoQuAngleArr[1]);
			CXhChar50 sAngle("火曲%0.1f", fHuoquAngle1 > fHuoquAngle2 ? fHuoquAngle1 : fHuoquAngle2);
			sProcessParam.Append(sAngle, ' ');
		}
		//卷边
		for (VERTEX_PTR pVertex = pPlate->listVertex.EnumFirst(); pVertex; pVertex = pPlate->listVertex.EnumNext())
		{
			if (pVertex->data.m_bRollEdge)
			{
				sProcessParam.Append(CXhChar50("卷边%dmm", pVertex->data.manu_space), ' ');
				break;
			}
		}
	}
	else if (pBomPart->cPartType == BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle = (PART_ANGLE*)pBomPart;
		//焊接
		if (pBomPart->bWeldPart)
			sProcessParam.Append("焊接", ' ');
		//制弯
		int nHuoquType = pBomAngle->GetHuoquType();
		long nHuoquLineCount = pBomPart->GetHuoquLineCount();
		CXhChar16 zhiwanArr[4] = { "内曲面","外曲面","内曲筋","外曲筋" };
		if (nHuoquLineCount > 0 && nHuoquType >= 1 && nHuoquType <= 4)
		{
			BOMPROFILE_VERTEX::COORD3D prev_len_vec;
			prev_len_vec.z = 1;
			double fMinCosa = 1;
			pBomAngle->listPolyAngle.EnumFirst();
			for (LIST_NODE<PART_ANGLE::POLY_ANGLE_INFO> *pNode = pBomAngle->listPolyAngle.EnumNext(); pNode; pNode = pBomAngle->listPolyAngle.EnumNext())
			{
				BOMPROFILE_VERTEX::COORD3D len_vec(pNode->data.len_vec);
				double cosa = prev_len_vec.x*len_vec.x + prev_len_vec.y*len_vec.y + prev_len_vec.z*len_vec.z;
				if (cosa < fMinCosa)
					fMinCosa = cosa;
				prev_len_vec.x = len_vec.x;
				prev_len_vec.y = len_vec.y;
				prev_len_vec.z = len_vec.z;
			}
			double fAngle = acos(fMinCosa)*DEGTORAD_COEF;
			sProcessParam.Append(CXhChar16("%s%0.1f", (char*)zhiwanArr[nHuoquType - 1], fAngle), ' ');
		}
		if (pBomAngle->nPushFlat > 0)		//是否压扁
			sProcessParam.Append(CXhChar16("压扁"), ' ');
		if (pBomAngle->bKaiJiao)
			sProcessParam.Append(CXhChar16("开角%0.1f", pBomAngle->wing_angle - 90), ' ');
		if (pBomAngle->bHeJiao)
			sProcessParam.Append(CXhChar16("合角%0.1f", 90 - pBomAngle->wing_angle), ' ');
		if (pBomAngle->bCutAngle)
			sProcessParam.Append("切角", ' ');
		if (pBomAngle->bCutBer)
			sProcessParam.Append("铲背", ' ');
		if (pBomAngle->bCutRoot)
			sProcessParam.Append("清根", ' ');
		if (pBomAngle->bHasFootNail)
			sProcessParam.Append("带脚钉", ' ');
	}
	return sProcessParam;
}

static CXhChar16 GetPartTypeName(BOMPART *pPart, BOOL bSymbol/*=FALSE*/)
{
	CXhChar16 sPartType;
	if (pPart->cPartType == BOMPART::PLATE)		//钢板
	{
		sPartType = bSymbol ? "-" : "钢板";
		if (pPart->siSubType == BOMPART::SUB_TYPE_PLATE_C)
			sPartType = bSymbol ? "" : "槽型插板";
		if (pPart->siSubType == BOMPART::SUB_TYPE_PLATE_U)
			sPartType = bSymbol ? "" : "U型插板";
		if (pPart->siSubType == BOMPART::SUB_TYPE_PLATE_X)
			sPartType = bSymbol ? "" : "十字插板";
		if (pPart->siSubType == BOMPART::SUB_TYPE_PLATE_FL ||
			pPart->siSubType == BOMPART::SUB_TYPE_PLATE_FLD ||
			pPart->siSubType == BOMPART::SUB_TYPE_PLATE_FLP ||
			pPart->siSubType == BOMPART::SUB_TYPE_PLATE_FLG ||
			pPart->siSubType == BOMPART::SUB_TYPE_PLATE_FLR)
			sPartType = bSymbol ? "" : "法兰";
	}
	else if (pPart->cPartType == BOMPART::ANGLE)	//角钢
		sPartType = bSymbol ? "L" : "角钢";
	else if (pPart->cPartType == BOMPART::TUBE)	//钢管
		sPartType = bSymbol ? "φ" : "钢管";
	else if (pPart->cPartType == BOMPART::SLOT)	//槽钢
		sPartType = bSymbol ? "[" : "槽钢";
	else if (pPart->cPartType == BOMPART::FLAT)	//扁钢
		sPartType = bSymbol ? "-" : "扁钢";
	else if (pPart->cPartType == BOMPART::ROUND)	//圆钢
		sPartType = bSymbol ? "φ" : "圆钢";
	else if (pPart->cPartType == BOMPART::ACCESSORY)
	{
		if (pPart->siSubType == BOMPART::SUB_TYPE_LADDER)
			sPartType = bSymbol ? " " : "爬梯";
		else if (pPart->siSubType == BOMPART::SUB_TYPE_STEEL_GRATING)
			sPartType = bSymbol ? "-" : "钢格栅";
		else if (pPart->siSubType == BOMPART::SUB_TYPE_STAY_ROPE)
			sPartType = bSymbol ? " " : "拉索构件";
		else
			sPartType = bSymbol ? " " : "附件";
	}
	return sPartType;
}
static CXhChar200 GetZuanKongParam(BOMPART *pBomPart)
{
	CHashStrList<int> hashCountByHoleDStr;
	for (BOMBOLT_RECORD* pBoltRec = pBomPart->m_arrBoltRecs.GetFirst(); pBoltRec; pBoltRec = pBomPart->m_arrBoltRecs.GetNext())
	{
		if (pBoltRec->cFuncType== pBoltRec->FUNC_WIREHOLE)
		{
			double hole_d = pBoltRec->hole_d_increment + pBoltRec->d;
			CXhChar16 sHoleD("%.1f", hole_d);
			int *pCount = hashCountByHoleDStr.GetValue(sHoleD);
			if (pCount == NULL)
			{
				pCount = hashCountByHoleDStr.Add(sHoleD);
				*pCount = 1;
			}
			else
				*pCount += 1;
		}
	}
	//数组判断是否是角钢切是打扁
	if (pBomPart->cPartType == BOMPART::ANGLE)	//角钢
	{
		PART_ANGLE *pAngle = (PART_ANGLE*)pBomPart;
		if (pAngle->listPushFlat.GetCount() > 0)
		{
			for (BOMBOLT_RECORD* pBoltRec = pBomPart->m_arrBoltRecs.GetFirst(); pBoltRec; pBoltRec = pBomPart->m_arrBoltRecs.GetNext())
			{
				if (pBoltRec->cFuncType == pBoltRec->FUNC_WIREHOLE)
					continue;
				ANGLE_PUSH_FLAT *pPushFlat = NULL;
				for (pPushFlat= pAngle->listPushFlat.EnumObjectFirst(); pPushFlat; pPushFlat = pAngle->listPushFlat.EnumObjectNext())
				{
					if(pBoltRec->x>pPushFlat->flat_start_pos && pBoltRec->x<pPushFlat->flat_end_pos)
						break;
				}
				if (pPushFlat != NULL)
				{
					double hole_d = pBoltRec->hole_d_increment + pBoltRec->d;
					CXhChar16 sHoleD("%.1f", hole_d);
					int *pCount = hashCountByHoleDStr.GetValue(sHoleD);
					if (pCount == NULL)
					{
						pCount = hashCountByHoleDStr.Add(sHoleD);
						*pCount = 1;
					}
					else
						*pCount += 1;
				}
			}
		}
	}
	CXhChar200 sHoleDStr;
	for (int *pCount = hashCountByHoleDStr.GetFirst(); pCount; pCount = hashCountByHoleDStr.GetNext())
	{
		sHoleDStr.Append(CXhChar16("钻%d*%s", *pCount,hashCountByHoleDStr.GetCursorKey()));
	}
	return sHoleDStr;
}

static CXhChar50 GetCutParaStr(BOMPART *pBomPart)
{
	if (pBomPart == NULL || pBomPart->cPartType != BOMPART::ANGLE)
		return CXhChar50();
	PART_ANGLE *pBomAngle = (PART_ANGLE*)pBomPart;
	CXhChar50 sCutPara;
	short nCutCount = 0;
	WORD wCutAngleLen = 0;
	long len = 0, width = 0, height = 0;
	for (int i = 0; i < 2; i++)
	{	//0.始端 1.终端
		if (pBomAngle->cut_wing[i] > 0 && pBomAngle->cut_wing_para[i][0] > 0)
		{	//切肢
			if (pBomAngle->cut_wing_para[i][1] == 0)
				nCutCount++;		//切空肢(记1刀)
			else
				nCutCount += 3;		//切肢(记3刀)
			wCutAngleLen += pBomAngle->cut_wing_para[i][0];
			len = abs(pBomAngle->cut_wing_para[i][0] - pBomAngle->cut_wing_para[i][2]);
			width = (int)pBomAngle->wide;
			wCutAngleLen += (WORD)sqrt(double(len*len + width * width));
			if (pBomAngle->cut_wing_para[i][1] > 0)
			{	//切肢
				len = pBomAngle->cut_wing_para[i][1];
				width = pBomAngle->cut_wing_para[i][0];
				wCutAngleLen += (WORD)sqrt(double(len*len + width * width));
			}
		}
		if (pBomAngle->cut_angle[i * 2][0] > 0)		//X肢切角(记1刀)
		{
			len=pBomAngle->cut_angle[i * 2][0];
			width = pBomAngle->cut_angle[i * 2][1];
			wCutAngleLen += (WORD)sqrt(double(len*len + width * width));
			nCutCount++;
		}
		if (pBomAngle->cut_angle[i * 2 + 1][0] > 0)	//Y肢切角(记1刀)
		{
			len = pBomAngle->cut_angle[i * 2+1][0];
			width = pBomAngle->cut_angle[i * 2 + 1][1];
			wCutAngleLen += (WORD)sqrt(double(len*len + width * width));
			nCutCount++;
		}
	}
	if (nCutCount > 0 && wCutAngleLen > 0)
		sCutPara.Printf("%d(%dmm)", nCutCount, wCutAngleLen);
	return sCutPara;
}

static CXhChar50 GetKaiHeJiaoManuParam(BOMPART *pBomPart)
{
	if (pBomPart == NULL || pBomPart->cPartType != BOMPART::ANGLE)
		return CXhChar50();
	PART_ANGLE*pAngle = (PART_ANGLE*)pBomPart;
	const int DEGREE_THRESHOLD = 14;
	int nKaiHeCount = 0, nKaiHeSumCount = 0, nKaiHeLen = 0, nSumKaiHeLen = 0;
	for (KAI_HE_JIAO *pKaiHeJiao = pAngle->listKaihe.EnumObjectFirst(); pKaiHeJiao; pKaiHeJiao = pAngle->listKaihe.EnumObjectNext())
	{
		nKaiHeLen = pKaiHeJiao->startLength + pKaiHeJiao->endLength;
		if(nKaiHeLen<=0)
			continue;
		if (pKaiHeJiao->decWingAngle >= DEGREE_THRESHOLD)
			nKaiHeCount = (int)ceil(((float)nKaiHeLen) / 350);
		else
			nKaiHeCount = (int)ceil(((float)nKaiHeLen) / 1200);
		nSumKaiHeLen += nKaiHeLen;
		nKaiHeSumCount += nKaiHeCount;
	}
	CXhChar50 sPara;
	if (nKaiHeSumCount>0&&nSumKaiHeLen>0)
		sPara.Printf("%d(%dmm)", nKaiHeSumCount,nSumKaiHeLen);
	return sPara;
}

// 极限值: ε
#define      LOCAL_EPS_COS          0.999999999
#define      LOCAL_EPS              1e-9
// 工程极限值: ε2 (2%的误差)
#define      LOCAL_EPS_COS2         0.97
#define      LOCAL_EPS2             0.03
#define		 LOCAL_PI				3.14159265358979323846
#define      LOCAL_DEGTORAD_COEF    57.2957795130823208768

static BOMPROFILE_VERTEX::COORD3D NormalizedVector(BOMPROFILE_VERTEX::COORD3D &vec)
{
	double dd = sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
	if (dd > 1e-9)
	{
		vec.x = vec.x / dd;
		vec.y = vec.y / dd;
		vec.z = vec.z / dd;
	}
	else
		vec.Set();
	return vec;
}

static double LocalDotProd(BOMPROFILE_VERTEX::COORD3D v1, BOMPROFILE_VERTEX::COORD3D v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

static double LOCAL_ACOS(double cosa)
{
	if (cosa >= 1)
		return 0;
	else if (cosa <= -1)
		return LOCAL_PI;
	else
		return acos(cosa);
}
static double LOCAL_ACOS(BOMPROFILE_VERTEX::COORD3D v1, BOMPROFILE_VERTEX::COORD3D v2)
{
	v1 = NormalizedVector(v1);
	v2 = NormalizedVector(v2);
	return LOCAL_ACOS(LocalDotProd(v1, v2));
}
static CXhChar50 GetBendGeKouManuParam(BOMPART *pBomPart)
{
	if (pBomPart == NULL)
		return CXhChar50();
	//角钢火曲割口按下面原则:
	// 1）单面外曲角度大于等于15就割口; 
	// 2）单面内曲角度大于等于5度就割口;
	// 3）双面火曲均不考虑割口。
	CXhChar50 sParam;
	if (pBomPart->cPartType == BOMPART::ANGLE)
	{
		PART_ANGLE *pAngle = (PART_ANGLE*)pBomPart;
		int nCount = 0, nSumLen = 0;
		PART_ANGLE::POLY_ANGLE_INFO *pPolyAngle = NULL,*pPrevPolyAngle=pAngle->listPolyAngle.EnumObjectFirst();
		for (pPolyAngle = pAngle->listPolyAngle.EnumObjectNext(); pPolyAngle; pPolyAngle = pAngle->listPolyAngle.EnumObjectNext())
		{
			if (pPrevPolyAngle == NULL)
				continue;
			BOMPROFILE_VERTEX::COORD3D len_vec = pPolyAngle->len_vec;
			NormalizedVector(len_vec);
			BOMPROFILE_VERTEX::COORD3D prev_wing_norm_x(pPrevPolyAngle->norm_x_wing), prev_wing_norm_y(pPrevPolyAngle->norm_y_wing);
			BOMPROFILE_VERTEX::COORD3D wing_norm_x(pPolyAngle->norm_x_wing), wing_norm_y(pPolyAngle->norm_y_wing);
			double angle = 0;
			if (fabs(len_vec.y) > LOCAL_EPS && fabs(len_vec.x) > LOCAL_EPS)
				continue;	//双肢火曲不需要考虑割口
			if (fabs(len_vec.y) > LOCAL_EPS)
			{
				if (len_vec.y > 0)	//内曲
					angle = LOCAL_ACOS(prev_wing_norm_x, wing_norm_x);
				else
					angle = -LOCAL_ACOS(prev_wing_norm_x, wing_norm_x);
			}
			else
			{
				if (len_vec.x > 0)	//内曲
					angle = LOCAL_ACOS(prev_wing_norm_y, wing_norm_y);
				else
					angle = -LOCAL_ACOS(prev_wing_norm_y, wing_norm_y);
			}
			double deg_angle = angle * LOCAL_DEGTORAD_COEF;
			if (angle >= 5)			//内曲大于5度
				nCount++;
			else if (angle <= -15)		//外曲大于15度
				nCount++;
		}
		if(nCount>0)
			sParam.Printf("%d(%.fmm)", nCount,nCount*pAngle->wide);
	}
	else if (pBomPart->cPartType == BOMPART::PLATE)
	{
		PART_PLATE *pPlate = (PART_PLATE*)pBomPart;
		sParam.Printf("1(%.fmm)", pPlate->fCutMouthLen);
	}
	return CXhChar50();
}
static CXhChar50 GetPushFlatManuParam(BOMPART *pBomPart)
{
	if (pBomPart == NULL || pBomPart->cPartType != BOMPART::ANGLE)
		return CXhChar50();
	PART_ANGLE*pAngle = (PART_ANGLE*)pBomPart;
	const int DEGREE_THRESHOLD = 14;
	int nPushFlatSumCount = 0, nPushFlatLen = 0, nSumPushFlatLen = 0;
	for (ANGLE_PUSH_FLAT *pPushFlat = pAngle->listPushFlat.EnumObjectFirst(); pPushFlat; pPushFlat = pAngle->listPushFlat.EnumObjectNext())
	{
		int startLen = min(0, pPushFlat->flat_start_pos);
		int endLen = min(pPushFlat->flat_end_pos, (short)pBomPart->length);
		if (endLen > startLen)
			nPushFlatLen = endLen - startLen;
		else
			continue;
		nPushFlatSumCount++;
		nSumPushFlatLen += nPushFlatLen;
	}
	CXhChar50 sPara;
	if (nPushFlatSumCount > 0 && nSumPushFlatLen > 0)
		sPara.Printf("%d(%dmm)", nPushFlatSumCount, nSumPushFlatLen);
	return sPara;
}
CXhChar50 GetHuoGeManuParam(BOMPART *pBomPart);
static bool IsLevel1Welding(BOMPART *pBomPart)
{	//板与板（角钢）对接焊、割口焊接及挂线孔处焊接 需用√标识出来
	if (pBomPart->siSubType == BOMPART::SUB_TYPE_TUBE_WIRE)
		return true;
	CXhChar50 sHuoGe=GetHuoGeManuParam(pBomPart);
	if (sHuoGe.GetLength() > 0)
		return true;
	//板与角钢对接焊

	return false;
}
//火割原则
//长度（圆弧及无法实现剪切的板外形）
//二维平面下点与线间的关系判断，>0:点在线左侧;<0:点在线右侧;0:点在线上
short SideOfPtLine(const double* pick, const double* lineStart, const double* lineEnd, double tolerance/*=EPS*/)
{
	double dX = pick[0] - lineStart[0];
	double dY = pick[1] - lineStart[1];
	double deltax = lineEnd[0] - lineStart[0];
	double deltay = lineEnd[1] - lineStart[1];
	double justify = deltax * dY - deltay * dX;
	if (justify > tolerance)
		return 1;
	else if (justify < -tolerance)
		return -1;
	return 0;
}
#define      LOCAL_DISTANCE(a,b)    sqrt(((a).x-(b).x)*((a).x-(b).x)+((a).y-(b).y)*((a).y-(b).y))
static CXhChar50 GetHuoGeManuParam(BOMPART *pBomPart)
{	
	if (pBomPart == NULL || pBomPart->cPartType != BOMPART::PLATE)
		return CXhChar50();
	PART_PLATE *pPlate = (PART_PLATE*)pBomPart;
	BOMPROFILE_VERTEX *pVertex = NULL;
	ARRAY_LIST<BOMPROFILE_VERTEX*> vertexPtrList;
	for (pVertex = pPlate->listVertex.EnumObjectFirst(); pVertex; pVertex = pPlate->listVertex.EnumObjectNext())
		vertexPtrList.append(pVertex);
	CHashList<BOMPROFILE_VERTEX*> hashVertexPtrByEdgeIndex;
	int n = vertexPtrList.GetSize();
	int nSumLen = 0;
	for(int i=0;i<vertexPtrList.GetSize();i++)
	{
		BOMPROFILE_VERTEX *pPrevVertex = *(vertexPtrList.GetByIndex((i + n - 1) % n));
		BOMPROFILE_VERTEX *pCurVertex = *(vertexPtrList.GetByIndex(i));
		BOMPROFILE_VERTEX *pNextVertex = *(vertexPtrList.GetByIndex((i + n + 1) % n));

		int iCurEdge = i + 1;
		int iPrevEdge = (i + n - 1) % n + 1;
		if (pCurVertex->type > 1)
		{	//圆弧
			if (hashVertexPtrByEdgeIndex.GetValue(iCurEdge) == NULL)
			{
				nSumLen += (int)pCurVertex->edge_len;
				hashVertexPtrByEdgeIndex.SetValue(iCurEdge, pCurVertex);
			}
		}
		else
		{
			short siRetCode = SideOfPtLine(pCurVertex->vertex, pPrevVertex->vertex, pNextVertex->vertex, LOCAL_EPS);
			if (siRetCode >= 0)
				continue;
			if (hashVertexPtrByEdgeIndex.GetValue(iPrevEdge) == NULL)
			{
				nSumLen += (int)LOCAL_DISTANCE(pPrevVertex->vertex, pCurVertex->vertex);
				hashVertexPtrByEdgeIndex.SetValue(iPrevEdge, pPrevVertex);
			}
			if (hashVertexPtrByEdgeIndex.GetValue(iCurEdge) == NULL)
			{
				nSumLen += (int)LOCAL_DISTANCE(pCurVertex->vertex, pNextVertex->vertex);
				hashVertexPtrByEdgeIndex.SetValue(iCurEdge, pCurVertex);
			}
		}
	}
	if (nSumLen > 0)
		return CXhChar50("%dmm", nSumLen);
	else
		return CXhChar50();
}
static char GetPartMaterialChar(BOMPART *pBomPart)
{
	if (pBomPart == NULL)
		return '0';
	char cMaterial = pBomPart->cMaterial;
	if (pBomPart->cPartType == BOMPART::PLATE)
	{
		if (pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_FLD || pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_FLP)
		{
			cMaterial = (strlen(pBomPart->sPartNo) >= 9) ? pBomPart->sPartNo.At(8) : '0';
			//if(cMaterial=='D')
			//	cMaterial='P';
		}
		else if (pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_U ||
			pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_X ||
			pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_C)
		{
			cMaterial = (strlen(pBomPart->sPartNo) >= 6) ? pBomPart->sPartNo.At(5) : '0';
		}
		else if (pBomPart->siSubType != 0)
		{
			cMaterial = '0';
		}
		char cRealMat = cMaterial;
		if (cRealMat == 'D')
			cRealMat = 'P';
		if (QuerySteelMatMark(cMaterial).GetLength() <= 0)
			cMaterial = '0';
	}
	return cMaterial;
}

CXhChar16 QuerySteelMatMark(char cMat,char cQualityLevel)
{
	CXhChar16 sMatMark;
	if ('H' == cMat)
		sMatMark.Copy("Q345");
	else if ('G' == cMat)
		sMatMark.Copy("Q390");
	else if ('P' == cMat)
		sMatMark.Copy("Q420");
	else if ('T' == cMat)
		sMatMark.Copy("Q460");
	else //if ('S' == cMat)
		sMatMark.Copy("Q235");
	if (cQualityLevel != 0)
		sMatMark.Append(cQualityLevel);
	else
		sMatMark.Append("B");
	return sMatMark;
}

void AddPartSpecTblSheet(CModelBOM &bom, LPDISPATCH pWorkSheet)
{
	CHashList<SEGI> segTable;
	GetSegNoHashTblBySegStr(bom.m_sSegStr, segTable);
	ARRAY_LIST<SUPERLIST_NODE<BOMPART>> BomArr;
	for (SUPERLIST_NODE<BOMPART> *pNode = bom.listParts.EnumFirst(); pNode; pNode = bom.listParts.EnumNext())
	{
		BOMPART* pBomPart = (BOMPART*)pNode->pDataObj;
		if (segTable.GetNodeNum() == 0 || segTable.GetValue(pBomPart->iSeg) != NULL)	//段号过滤
			BomArr.append(*pNode);
	}
	CBubbleSort<SUPERLIST_NODE<BOMPART>>::BubSort(BomArr.m_pData, BomArr.GetSize(), CompareSegiFunc);
	//
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet, FALSE);
	excel_sheet.SetName("放样原始材料表");
	//添加实时数据的列标题
	CExcelOper::SetMainTitle(excel_sheet, "A1", "AA1", "重庆广仁铁塔原始材料表", 14.0);
	CExcelOper::SetMainTitle(excel_sheet, "A2", "A2", "塔型", 14.0);
	CExcelOper::SetMainTitle(excel_sheet, "B2", "D2", "", 14.0);
	CExcelOper::SetMainTitle(excel_sheet, "E2", "E2", "工程名称", 14.0);
	CExcelOper::SetMainTitle(excel_sheet, "F2", "I2", "", 14.0);
	CExcelOper::SetMainTitle(excel_sheet, "X2", "AA2", "螺栓孔数量", 14.0);
	CExcelOper::SetMainTitle(excel_sheet, "AB3", "AC3", "特殊工艺", 14.0);
	CExcelOper::SetMainTitle(excel_sheet, "AB2", "AB2", "基数", 14.0);
	CExcelOper::SetMainTitle(excel_sheet, "AC2", "AC2", "1", 14.0);
	CExcelOper::SetMainTitle(excel_sheet, "K3", "L3", "制弯", 14.0);

	//CExcelOper::AddRowToExcelSheet(excel_sheet, 3, strarr, col_arr, TRUE);

	int COL_COUNT = 29;
	double col_arr[29] = { 8.0,10.0,10.0,18.0,10.0,
		 				   8.0,8.0,8.0,8.0,18.0,
						   9.0,9.0,10.5,6.0,6.0,
						   25.0,25.0,18.0,18.0,25.0,
						   18.0,25.0,20.0,6.0,6.0,
						   6.0,6.0,15.0,15.0 };
	CStringArray str_arr;
	str_arr.SetSize(COL_COUNT);
	str_arr[0] = "段名"; str_arr[1] = "编号"; str_arr[2] = "材质"; str_arr[3] = "规格"; str_arr[4] = "长度";
	str_arr[5] = "单基件数"; str_arr[6] = "总件数"; str_arr[7] = "单根重"; str_arr[8] = "总重"; str_arr[9] = "切角/切肢角（长度）";
	str_arr[10] = "制弯";str_arr[12] = "套管";str_arr[13] = "清根";str_arr[14] = "铲背";str_arr[15] = "开合角";
	str_arr[16] = "火割（含板开槽）";str_arr[17] = "打扁";str_arr[18] = "板面积";str_arr[19] = "表面积（角钢和板）";
	str_arr[20] = "焊接边长度";str_arr[21] = "一级焊";str_arr[22] = "钻孔（挂线角钢、板及打扁角钢端）";str_arr[23] = "M16";
	str_arr[24] = "M20";str_arr[25] = "M24";str_arr[26] = "M_X";str_arr[27] = "特殊工艺";
	CExcelOper::AddRowToExcelSheet(excel_sheet, 3, str_arr, col_arr, TRUE);
	//添加实时数据
	CVariant2dArray map(bom.m_nPartsN + 2, COL_COUNT);
	int index = 0;
	for (SUPERLIST_NODE<BOMPART>*pNode = BomArr.GetFirst(); pNode; pNode = BomArr.GetNext())
	{
		BOMPART* pBomPart = (BOMPART*)pNode->pDataObj;
		CXhChar50 sSpecialSpec;
		SEGI segI = SEGI(pBomPart->iSeg);
		if (pBomPart->cPartType == BOMPART::PLATE)
		{
			if (pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_FLD ||
				pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_FLP ||
				pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_U ||
				pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_X ||
				pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_C)
			{
				sSpecialSpec.Copy(pBomPart->sPartNo);
				if (segI.Prefix().GetLength() > 0 && sSpecialSpec.StartWith(segI.key.prefix[0]))
					pBomPart->iSeg = 0;
			}
		}
		else if (pBomPart->cPartType == BOMPART::ACCESSORY ||
			pBomPart->cPartType == BOMPART::SUB_TYPE_STAY_ROPE)	//错误将设置构件类型
		{
			sSpecialSpec.Copy(pBomPart->sPartNo);
			if (segI.Prefix().GetLength() > 0 && sSpecialSpec.StartWith(segI.key.prefix[0]))
				pBomPart->iSeg = 0;
			//if(pBomPart->siSubType==BOMPART::SUB_TYPE_STEEL_GRATING)
			//	continue;	//钢格栅单独出一张表
		}
		if (pBomPart->iSeg != 0)
			map.SetValueAt(index, 0, COleVariant(SEGI(pBomPart->iSeg).ToString()));//段号
		if (sSpecialSpec.GetLength() <= 0)
			map.SetValueAt(index, 1, COleVariant(pBomPart->GetPartNo()));		  //零件编号
		char cMaterial = GetPartMaterialChar(pBomPart);
		CXhChar16 sMat=QuerySteelMatMark(cMaterial, pBomPart->cQualityLevel);
		map.SetValueAt(index, 2, COleVariant(CXhChar16("%c", cMaterial)));		 //材质
		if (cMaterial == 'P' || cMaterial == 'D')	//Q420字体进行加粗
			CExcelOper::SetBoldFont(excel_sheet, (char*)CXhChar16("C%d", index + 4), NULL);	//字体加粗
		if (cMaterial == 'H')
			CExcelOper::SetRangeBackColor(excel_sheet, CExcelOper::COLOR_ID_YELLOW, CXhChar16("C%d", index + 4));

		if (sSpecialSpec.GetLength() > 0)
			map.SetValueAt(index, 3, COleVariant(sSpecialSpec));					//设计规格
		else
			map.SetValueAt(index, 3, COleVariant(GetSpecStr(pBomPart)));			//设计规格
		if (pBomPart->length > 0)
			map.SetValueAt(index, 4, COleVariant(CXhChar16("%d", ftol(pBomPart->length))));//长度
		map.SetValueAt(index, 5, COleVariant((long)pBomPart->GetPartNum()));//单基数
		map.SetValueAt(index, 6, COleVariant(CXhChar16("=F%d*AC2",index+4)));	//数量
		
		if (pBomPart->cPartType == BOMPART::PLATE)	//钢板考虑净面积重量
		{
			double fWeight = ((PART_PLATE*)pBomPart)->fPieceNetWeight;
			if (fWeight <= 0)
				fWeight = pBomPart->fPieceWeight;
			map.SetValueAt(index, 7, COleVariant(CXhChar16("%.2f", fWeight)));				//单件重量
		}
		else
			map.SetValueAt(index, 7, COleVariant(CXhChar16("%.2f", pBomPart->fPieceWeight)));//单件重量
		map.SetValueAt(index, 8, COleVariant(CXhChar16("=H%d*G%d", index+4,index+4)));		//总重
		map.SetValueAt(index, 9, COleVariant(GetCutParaStr(pBomPart)));						//切角参数
		map.SetValueAt(index, 17, COleVariant(GetBendGeKouManuParam(pBomPart)));
		BOMBOLT_RECORD *pBomAngle = (BOMBOLT_RECORD*)pBomPart;
		PART_PLATE *pBomPlate = (PART_PLATE*)pBomPart;
		PART_ANGLE *pBomAnGIE = (PART_ANGLE*)pBomPart;
		KAI_HE_JIAO*pKHJ = (KAI_HE_JIAO*)pBomPart;

		if (pBomPart->siZhiWan > 0)
			map.SetValueAt(index, 11, COleVariant(pBomPart->siZhiWan));					//制弯刀数
		if (pBomPart->siSubType == BOMPART::SUB_TYPE_TUBE_WIRE)
			map.SetValueAt(index, 12, COleVariant("√"));//套管

		if (pBomAnGIE->bCutRoot)
			map.SetValueAt(index, 13, COleVariant(CXhChar16("%d", ftol(pBomPart->length))));//清根
		if (pBomAnGIE->bCutBer)
			map.SetValueAt(index, 14, COleVariant(CXhChar16("%d", ftol(pBomPart->length))));//铲背
		map.SetValueAt(index, 15, COleVariant(GetKaiHeJiaoManuParam(pBomPart)));
		map.SetValueAt(index, 16, COleVariant(GetHuoGeManuParam(pBomPart)));
		map.SetValueAt(index, 17, COleVariant(GetPushFlatManuParam(pBomPart)));
		if (pBomPlate)
		{
			int area = (int)pBomPlate->real_area;
			if(area>0)
				map.SetValueAt(index, 18, COleVariant(CXhChar100("%d mm2", area)));
		}
		if (pBomPlate || pBomAngle)
		{
			int area = (int)pBomPart->CalZincCoatedArea(FALSE);
			map.SetValueAt(index, 19, COleVariant(CXhChar100("%d mm3", area)));
		}
		int nWeldEdgeLen = (int)pBomPlate->fWeldEdgeLen;
		if(nWeldEdgeLen>0)
			map.SetValueAt(index, 20, COleVariant(CXhChar16("%d mm", nWeldEdgeLen)));
		if(IsLevel1Welding(pBomPart))
			map.SetValueAt(index, 21, COleVariant("√"));//一级焊
		map.SetValueAt(index, 22, COleVariant(GetZuanKongParam(pBomPart)));
		if(pBomPart->nM16Ls>0)
			map.SetValueAt(index, 23, COleVariant(pBomPart->nM16Ls));//M16
		if(pBomPart->nM20Ls>0)
			map.SetValueAt(index, 24, COleVariant(pBomPart->nM20Ls));//M20
		if(pBomPart->nM24Ls>0)
			map.SetValueAt(index, 25, COleVariant(pBomPart->nM24Ls));//M16
		if(pBomPart->nMXLs>0)
			map.SetValueAt(index, 26, COleVariant(pBomPart->nMXLs));//M16

		index++;
	}
	CXhChar16 cell_end("AC%d", index);
	CExcelOper::SetRangeValue(excel_sheet, "A4", cell_end, map.var);				//填充表格内容
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet, "A1", cell_end, COleVariant((long)3));
	CExcelOper::SetRangeBorders(excel_sheet, "A1", cell_end, COleVariant(16.0));	//设置边框和字体大小
	CExcelOper::SetRangeRowHeight(excel_sheet, 20.25, "A1", cell_end);
	excel_sheet.ReleaseDispatch();
}

void AddSteelGratingTblSheet(CModelBOM &bom, LPDISPATCH pWorkSheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet, FALSE);
	excel_sheet.SetName("钢格板");
	excel_sheet.ReleaseDispatch();
	return;
	/*
	CHashList<SEGI> segTable;
	GetSegNoHashTblBySegStr(bom.m_sSegStr,segTable);
	ARRAY_LIST<SUPERLIST_NODE<BOMPART>> BomArr;
	for(SUPERLIST_NODE<BOMPART> *pNode=bom.listParts.EnumFirst();pNode;pNode=bom.listParts.EnumNext())
	{
		BOMPART* pBomPart=(BOMPART* )pNode->pDataObj;
		if(segTable.GetNodeNum()==0||segTable.GetValue(pBomPart->iSeg)!=NULL)	//段号过滤
			BomArr.append(*pNode);
	}
	CBubbleSort<SUPERLIST_NODE<BOMPART>>::BubSort(BomArr.m_pData,BomArr.GetSize(),CompareSegiFunc);
	//

	//添加实时数据的列标题
	int COL_COUNT=6;
	double col_arr[6]={8.0,12.0,10.0,18.0,10.0,8.0};
	//添加实时数据
	CVariant2dArray map(bom.m_nPartsN+2,COL_COUNT);
	int index=0;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		if(pBomPart->cPartType!=BOMPART::ACCESSORY&&pBomPart->siSubType!=BOMPART::SUB_TYPE_STEEL_GRATING)
			continue;
		map.SetValueAt(index,0,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//零件编号
		map.SetValueAt(index,1,COleVariant(GetSpecStr(pBomPart)));						//设计规格
		map.SetValueAt(index,2,COleVariant(CXhChar16("%d*%d",ftol(pBomPart->wide),ftol(pBomPart->length))));
		map.SetValueAt(index,3,COleVariant(CXhChar16("%d",pBomPart->GetPartNum())));	//数量
		map.SetValueAt(index,4,COleVariant(CXhChar16("%.2f",pBomPart->fPieceWeight)));	//单重
		map.SetValueAt(index,5,COleVariant(CXhChar16("%.1f",pBomPart->fPieceWeight*pBomPart->GetPartNum())));	//总重
		index++;
	}
	if(index>0)
	{
		CXhChar16 cell_end("F%d",index);
		CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);				//填充表格内容
		CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
		CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(16.0));	//设置边框和字体大小
		CExcelOper::SetRangeRowHeight(excel_sheet,20.25,"A1",cell_end);
	}
	excel_sheet.ReleaseDispatch();*/
}

TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf, int buf_len, const char* file_name, DWORD dwFlag)
{
	CModelBOM bom;
	CBuffer buffer(buf_len);
	if (buf_len > 0)
	{
		buffer.AttachMemory(data_buf, buf_len);
		bom.FromBuffer(buffer, GetSupportDataBufferVersion());
	}
	//
	LPDISPATCH pWorksheets = CExcelOper::CreateExcelWorksheets(2);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet;
	//1 构件明细表
	pWorksheet = excel_sheets.GetItem(COleVariant((short)1));
	AddPartSpecTblSheet(bom, pWorksheet);
	//2.钢格栅
	//pWorksheet = excel_sheets.GetItem(COleVariant((short)2));
	//AddSteelGratingTblSheet(bom, pWorksheet);
	//excel_sheets.ReleaseDispatch();
	return 0;
}