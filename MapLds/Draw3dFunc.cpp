#include "stdafx.h"
#include "stdarx.h"
#include "Draw3dFunc.h"
#include "dbsol3d.h"
#include "dbregion.h"
#include "MenuFunc.h"
#include "Variant.h"
#include "ExcelOper.h"
#include "SysPara.h"

CLogFile xMyErrLog("D:\\warning.log");
//////////////////////////////////////////////////////////////////////////
//�ֲ���̬���ܺ���
static void GetCircleSimuPolyVertex(double radius, GEPOINT vertex_arr[], int slices)
{
	double slice_angle = 2 * Pi / slices;
	double sina = sin(slice_angle);	//��Ƭ������
	double cosa = cos(slice_angle);	//��Ƭ������
	vertex_arr[0].Set(radius);
	for (int i = 1; i < slices; i++)
	{
		vertex_arr[i].x = vertex_arr[i - 1].x*cosa - vertex_arr[i - 1].y*sina;
		vertex_arr[i].y = vertex_arr[i - 1].y*cosa + vertex_arr[i - 1].x*sina;
		vertex_arr[i].z = 0;
	}
}
//���������Բ��������ģ��Բ���Ķ����б�
void CalArcLineVertexList(f3dArcLine arcLine, fPtList &vertex_list)
{
	int smoothness = 16;
	int nSlices = CalArcResolution(arcLine.Radius(), fabs(arcLine.SectorAngle()), 100, 5, smoothness);
	double fSectorAngle = arcLine.SectorAngle() / nSlices;
	for (int i = 1; i < nSlices; i++)
	{
		f3dPoint vertex = arcLine.Start();
		if (arcLine.ColumnNorm().IsZero())	//Բ��
			rotate_point_around_axis(vertex, i*fSectorAngle, arcLine.Center(), arcLine.Center() + arcLine.WorkNorm());
		else	//��Բ��
			vertex = arcLine.PositionInAngle(i*fSectorAngle);
		vertex_list.append(vertex);
	}
	//���Բ���ն�����(nSlices����Ϊ0)
	vertex_list.append(arcLine.End());
}
CXhChar100 VariantToString(VARIANT value)
{
	CXhChar100 sValue;
	if (value.vt == VT_BSTR)
		return sValue.Copy(CString(value.bstrVal));
	else if (value.vt == VT_R8)
		return sValue.Copy(CXhChar100(value.dblVal));
	else if (value.vt == VT_R4)
		return sValue.Copy(CXhChar100(value.fltVal));
	else if (value.vt == VT_INT)
		return sValue.Copy(CXhChar100(value.intVal));
	else
		return sValue;
}
GEPOINT TransPoint_m_To_mm(GEPOINT srcPt)
{
	GEPOINT dstPt;
	dstPt.x = srcPt.x * 1000;
	dstPt.y = srcPt.y * 1000;
	dstPt.z = srcPt.z * 1000;
	return dstPt;
}
//Exce����
static const int LOFTING_EXCEL_COL_COUNT = 12;
static const char* T_NO = "���";
static const char* T_TOWER_NUM = "��λ��";
static const char* T_TOWER_DIAN = "��λ��";
static const char* T_TOWER_TYPE = "����";
static const char* T_LOCATE_HIGH = "��λ��";
static const char* T_WIRE_ANGLE = "��·ת��";
static const char* T_HUGAO_HEIGHT = "���Ƹ�";
static const char* T_BODY_HEIGHTP = "�����";
static const char* T_LEG_LENGTH = "���ȳ���";

#ifdef __RAPID_ROUGH_3DMODEL_
CDrawManager gDrawManager;
//////////////////////////////////////////////////////////////////////////
//CDrawModel
CDrawModel::CDrawModel()
{
	m_bTurnLeft = FALSE;
	m_ciErrorType = 0;
	m_fMaxLegHeight = 0;
	m_uiHeightSerial = 1;
	for (int i = 0; i < 4; i++)
		m_uiLegSerialArr[i] = 1;
}
CDrawModel::~CDrawModel()
{

}

double CDrawModel::CalTowerHeight(void* pInstance /*= NULL*/)
{
	if (pInstance == NULL)
		pInstance = ExtractActiveTaInstance();
	double fHeight = 0;
	if (m_ciModelFlag == TYPE_TID)
		fHeight = ((ITidTowerInstance*)pInstance)->GetInstanceHeight();
	else
	{
		CLDSModule* pModule = (CLDSModule*)pInstance;
		for (CLDSLinePart *pPole = Ta.EnumRodFirst(); pPole; pPole = Ta.EnumRodNext())
		{
			if(!pModule->IsSonPart(pPole))
				continue;
			fHeight=max(fHeight, pPole->xPosStart.z);
			fHeight = max(fHeight, pPole->xPosEnd.z);
		}
	}
	return fHeight;
}
/*����ת������ϵ����ά�������LDSģ������ϵת����GIM����ϵ��
//LDSģ�����꣺			GIMģ�����꣺		 ��LDS-->GIM��ת������ϵ
//	AxisX(1,0,0)		  AxisX(1,0,0)			AxisX(1,0,0)
//	AxisY(0,0,1)		  AxisY(0,0,-1)			AxisY(0,-1,0)
//	AxisZ(0,-1,0)		  AxisZ(0,1,0)			AxisZ(0,0,-1)
//  ԭ���ڶ���			 ԭ���ڵײ�
*/
GECS CDrawModel::BuildTransCS(void* pInstance /*= NULL*/)
{
	double fHeight = CalTowerHeight(pInstance);
	GECS cs;
	cs.origin.Set(0, 0, fHeight);
	cs.axis_x.Set(1, 0, 0);
	cs.axis_y.Set(0, -1, 0);
	cs.axis_z.Set(0, 0, -1);
	return cs;
}

void* CDrawModel::ExtractActiveTaInstance()
{
	if (m_ciModelFlag == TYPE_TID)
	{
		ITidModel* pTidModel = BelongManager()->m_pTidModel;
		if (pTidModel == NULL || !pTidModel->ReadTidFile(m_sFilePath))
		{
			m_ciErrorType = ERROR_READ_FILE;
			return NULL;
		}
		ITidHeightGroup* pHeightGroup = pTidModel->GetHeightGroupAt(m_uiHeightSerial - 1);
		if (pHeightGroup == NULL)
		{
			m_ciErrorType = ERROR_FIND_HUGAO;
			return NULL;
		}
		pHeightGroup->GetName(m_sHuGao, 50);
		ITidTowerInstance* pInstance = NULL;
		pInstance = pHeightGroup->GetTowerInstance(m_uiLegSerialArr[0], m_uiLegSerialArr[1], m_uiLegSerialArr[2], m_uiLegSerialArr[3]);
		if (pInstance == NULL)
		{
			m_ciErrorType = ERROR_FIND_INSTANCE;
			return NULL;
		}
		return pInstance;
	}
	else if (m_ciModelFlag == TYPE_LDS)
	{
		CloseTaFile(FALSE, TRUE);
		OpenTaFile(CString(m_sFilePath), FALSE, TRUE);
		//
		CLDSModule* pModule = NULL;
		for (pModule = Ta.Module.GetFirst(); pModule; pModule = Ta.Module.GetNext())
		{
			if (pModule->idBodyNo == m_uiHeightSerial)
				break;
		}
		if (pModule == NULL)
		{
			m_uiHeightSerial = 0;
			m_ciErrorType = ERROR_FIND_HUGAO;
			return NULL;
		}
		m_sHuGao.Copy(pModule->description);
		Ta.m_hActiveModule = pModule->handle;
		int nMaxLegs = CFGLEG::MaxLegs();
		pModule->m_arrActiveQuadLegNo[0] = (m_uiHeightSerial - 1)*nMaxLegs + m_uiLegSerialArr[0];
		pModule->m_arrActiveQuadLegNo[1] = (m_uiHeightSerial - 1)*nMaxLegs + m_uiLegSerialArr[1];
		pModule->m_arrActiveQuadLegNo[2] = (m_uiHeightSerial - 1)*nMaxLegs + m_uiLegSerialArr[2];
		pModule->m_arrActiveQuadLegNo[3] = (m_uiHeightSerial - 1)*nMaxLegs + m_uiLegSerialArr[3];
		return pModule;
	}
	return NULL;
}
/*���������޵Ķ��չ�ϵ
   ����		  ����ģ��������	��תģ��������
Serial[0]		LegD				LegB
Serial[1]		LegA				LegC
Serial[2]		LegC				LegA
Serial[3]		LegB				LegD
*/
BOOL CDrawModel::ExtractActiveTaInstance(char* sHuGao, double fLegA, double fLegB, double fLegC, double fLegD)
{
	if (m_ciModelFlag == TYPE_TID)
	{
		ITidModel* pTidModel = BelongManager()->m_pTidModel;
		if (pTidModel == NULL || !pTidModel->ReadTidFile(m_sFilePath))
		{
			m_ciErrorType = ERROR_READ_FILE;
			return FALSE;
		}
		ITidHeightGroup* pHeightGroup = NULL;
		for (int i = 0; i < pTidModel->HeightGroupCount(); i++)
		{
			CXhChar50 sName;
			ITidHeightGroup* pTemHuGao = pTidModel->GetHeightGroupAt(i);
			if (pTemHuGao)
				pTemHuGao->GetName(sName, 50);
			if (sName.EqualNoCase(sHuGao))
			{
				pHeightGroup = pTemHuGao;
				break;
			}
		}
		if (pHeightGroup == NULL)
		{
			m_uiHeightSerial = 0;
			m_ciErrorType = ERROR_FIND_HUGAO;
			return FALSE;
		}
		if (fLegA > 0 && fLegB > 0 && fLegC > 0 && fLegD > 0)
		{	//����ģʽ����Ҫת��Ϊ��������
			fLegA -= m_fMaxLegHeight;
			fLegB -= m_fMaxLegHeight;
			fLegC -= m_fMaxLegHeight;
			fLegD -= m_fMaxLegHeight;
		}
		m_uiHeightSerial = pHeightGroup->GetSerialId();
		int iLegASerial = pHeightGroup->GetLegSerial(fLegA);
		int iLegBSerial = pHeightGroup->GetLegSerial(fLegB);
		int iLegCSerial = pHeightGroup->GetLegSerial(fLegC);
		int iLegDSerial = pHeightGroup->GetLegSerial(fLegD);
		if (iLegASerial > 0 && iLegBSerial > 0 && iLegCSerial > 0 && iLegDSerial > 0)
		{
			if (m_bTurnLeft)
			{
				m_uiLegSerialArr[1] = iLegCSerial;
				m_uiLegSerialArr[3] = iLegDSerial;
				m_uiLegSerialArr[2] = iLegASerial;
				m_uiLegSerialArr[0] = iLegBSerial;
			}
			else
			{
				m_uiLegSerialArr[1] = iLegASerial;
				m_uiLegSerialArr[3] = iLegBSerial;
				m_uiLegSerialArr[2] = iLegCSerial;
				m_uiLegSerialArr[0] = iLegDSerial;
			}
		}
		else
		{
			CXhChar50 szError("�������ô���:"), szErrLeg;
			if (iLegASerial == 0)
				szErrLeg.Printf("%gm", fLegA);
			if (iLegBSerial == 0 && fLegB != fLegA)
				szErrLeg.Append(CXhChar16("%gm", fLegB), ',');
			if (iLegCSerial == 0 && fLegC != fLegB && fLegC != fLegA)
				szErrLeg.Append(CXhChar16("%gm", fLegC), ',');
			if (iLegDSerial == 0 && fLegD != fLegC && fLegD != fLegB && fLegD != fLegA)
				szErrLeg.Append(CXhChar16("%gm", fLegD), ',');
			xMyErrLog.Log("�������ô���:%s", (char*)szErrLeg);
			m_uiLegSerialArr[0] = m_uiLegSerialArr[1] = m_uiLegSerialArr[2] = m_uiLegSerialArr[3] = 1;
			m_ciErrorType = ERROR_FIND_LEG;
			return FALSE;
		}
		//
		ITidTowerInstance* pInstance = NULL;
		pInstance = pHeightGroup->GetTowerInstance(m_uiLegSerialArr[0], m_uiLegSerialArr[1], m_uiLegSerialArr[2], m_uiLegSerialArr[3]);
		if (pInstance == NULL)
		{
			m_uiLegSerialArr[0] = m_uiLegSerialArr[1] = m_uiLegSerialArr[2] = m_uiLegSerialArr[3] = 1;
			m_ciErrorType = ERROR_FIND_INSTANCE;
			return FALSE;
		}
	}
	else if (m_ciModelFlag == TYPE_LDS)
	{
		CloseTaFile(FALSE, TRUE);
		OpenTaFile(CString(m_sFilePath), FALSE, TRUE);
		//
		CLDSModule* pModule = NULL;
		for (pModule = Ta.Module.GetFirst(); pModule; pModule = Ta.Module.GetNext())
		{
			if (strcmp(pModule->description, m_sHuGao) == 0)
				break;
		}
		if (pModule == NULL)
		{
			m_uiHeightSerial = 0;
			m_ciErrorType = ERROR_FIND_HUGAO;
			return FALSE;
		}
		if (fLegA > 0 && fLegB > 0 && fLegC > 0 && fLegD > 0)
		{	//����ģʽ����Ҫת��Ϊ��������
			fLegA -= m_fMaxLegHeight;
			fLegB -= m_fMaxLegHeight;
			fLegC -= m_fMaxLegHeight;
			fLegD -= m_fMaxLegHeight;
		}
		CHashStrList<int> hashSubLegSerial;
		m_uiHeightSerial = pModule->GetBodyNo();
		int nMaxLegs = CFGLEG::MaxLegs();
		DWORD dwIncLegItemFlag = pModule->m_dwLegCfgWord.GetBodyLegs(pModule->idBodyNo);
		for (int i = 0; i < 32; i++)
		{
			if (dwIncLegItemFlag % 2 == 0)
				continue;
			dwIncLegItemFlag >>= 1;
			double heightdiff = (i* pModule->uiLevelHeight)*0.001;
			hashSubLegSerial.SetValue(CXhChar50(heightdiff), i + 1);
		}
		//
		int *pLegASerial = hashSubLegSerial.GetValue(CXhChar50(fabs(fLegA)));
		int *pLegBSerial = hashSubLegSerial.GetValue(CXhChar50(fabs(fLegB)));
		int *pLegCSerial = hashSubLegSerial.GetValue(CXhChar50(fabs(fLegC)));
		int *pLegDSerial = hashSubLegSerial.GetValue(CXhChar50(fabs(fLegD)));
		if (pLegASerial&&pLegBSerial&&pLegCSerial&&pLegDSerial)
		{
			if (m_bTurnLeft)
			{
				m_uiLegSerialArr[1] = *pLegCSerial;
				m_uiLegSerialArr[3] = *pLegDSerial;
				m_uiLegSerialArr[2] = *pLegASerial;
				m_uiLegSerialArr[0] = *pLegBSerial;
			}
			else
			{
				m_uiLegSerialArr[1] = *pLegASerial;
				m_uiLegSerialArr[3] = *pLegBSerial;
				m_uiLegSerialArr[2] = *pLegCSerial;
				m_uiLegSerialArr[0] = *pLegDSerial;
			}
		}
		else
		{
			CXhChar50 szError("�������ô���:"), szErrLeg;
			if (pLegASerial == 0)
				szErrLeg.Printf("%gm", fLegA);
			if (pLegBSerial == 0 && fLegB != fLegA)
				szErrLeg.Append(CXhChar16("%gm", fLegB), ',');
			if (pLegCSerial == 0 && fLegC != fLegB && fLegC != fLegA)
				szErrLeg.Append(CXhChar16("%gm", fLegC), ',');
			if (pLegDSerial == 0 && fLegD != fLegC && fLegD != fLegB && fLegD != fLegA)
				szErrLeg.Append(CXhChar16("%gm", fLegD), ',');
			xMyErrLog.Log("�������ô���:%s", (char*)szErrLeg);
			m_uiLegSerialArr[0] = m_uiLegSerialArr[1] = m_uiLegSerialArr[2] = m_uiLegSerialArr[3] = 1;
			m_ciErrorType = ERROR_FIND_LEG;
			return FALSE;
		}
	}
	return TRUE;
}
CXhChar100 CDrawModel::GetFileName(void* pInstance)
{
	CXhChar100 sFileName;
	if (m_sTowerPlaceCode.GetLength() <= 0)
	{
		sFileName.Printf("%s-%s%s", (char*)m_sTowerType, (char*)m_sHuGao, (char*)m_sLegDes);
		sFileName.Append(CXhChar50("-LOD%d",BelongManager()->m_ciInstanceLevel));
	}
	else
	{
		double dfNamedHeight = 0;	//����(V1.4��������,�ɰ淵��0)����λm  wjh-2019.4.25
		double fLegA = 0, fLegB = 0, fLegC = 0, fLegD = 0;
		if (m_ciModelFlag == TYPE_TID)
		{
			ITidTowerInstance* pTidInstance = (ITidTowerInstance*)pInstance;
			dfNamedHeight = pTidInstance->BelongHeightGroup()->GetNamedHeight()*0.001;
			fLegA = pTidInstance->BelongHeightGroup()->GetLegHeightDifference(m_uiLegSerialArr[0]);
			fLegB = pTidInstance->BelongHeightGroup()->GetLegHeightDifference(m_uiLegSerialArr[1]);
			fLegC = pTidInstance->BelongHeightGroup()->GetLegHeightDifference(m_uiLegSerialArr[2]);
			fLegD = pTidInstance->BelongHeightGroup()->GetLegHeightDifference(m_uiLegSerialArr[3]);
		}
		else if (m_ciModelFlag == TYPE_LDS)
		{
			
		}
		CMaxDouble maxValue;
		maxValue.Update(fLegA);
		maxValue.Update(fLegB);
		maxValue.Update(fLegC);
		maxValue.Update(fLegD);
		sFileName = m_sTowerPlaceCode;
		if (maxValue.IsInited())
		{
			double fMaxSubtractLegH = maxValue.number;
			if (dfNamedHeight == 0)
			{
				CXhChar50 sHuGao(m_sHuGao);
				sHuGao.Remove('m');
				sHuGao.Remove('M');
				dfNamedHeight = atof(sHuGao);
			}
			//
			double dfActualHeight = dfNamedHeight + fMaxSubtractLegH;
			sFileName.Append(CXhChar50("%s-%gm-LOD%d", (char*)m_sTowerType, dfActualHeight, BelongManager()->m_ciInstanceLevel), ' ');
		}
		else
			sFileName.Append(CXhChar50("%s-error-LOD%d", (char*)m_sTowerType, BelongManager()->m_ciInstanceLevel), ' ');
	}
	return sFileName;
}

//////////////////////////////////////////////////////////////////////////
//CDrawManager
CDrawManager::CDrawManager()
{
	m_pActiveTa = NULL;
	m_pTidModel = NULL;
	m_ciInstanceLevel = 0;
	m_bUseUnitM = FALSE;
}

CDrawManager::~CDrawManager()
{
	if (m_pTidModel)
		CTidModelFactory::Destroy(m_pTidModel->GetSerialId());
	m_pTidModel = NULL;
}

void CDrawManager::Empty()
{
	m_pActiveTa = NULL;
	m_pTidModel = NULL;
	m_xDrawModelList.Empty();
}

BOOL CDrawManager::ReadTidFile(const char* sFileName)
{
	if (m_pTidModel == NULL)
		m_pTidModel = CTidModelFactory::CreateTidModel();
	CXhChar50 sTowerName;
	_splitpath(sFileName, NULL, NULL, sTowerName, NULL);
	m_pActiveTa = m_xDrawModelList.append();
	m_pActiveTa->SetManager(this);
	m_pActiveTa->m_sTowerType = sTowerName;
	m_pActiveTa->m_ciModelFlag = CDrawModel::TYPE_TID;
	m_pActiveTa->m_sFilePath.Copy(sFileName);
	return TRUE;
}
BOOL CDrawManager::ReadLdsFile(const char* sFileName)
{
	CXhChar50 sTowerName;
	_splitpath(sFileName, NULL, NULL, sTowerName, NULL);
	m_pActiveTa = m_xDrawModelList.append();
	m_pActiveTa->SetManager(this);
	m_pActiveTa->m_sTowerType = sTowerName;
	m_pActiveTa->m_ciModelFlag = CDrawModel::TYPE_LDS;
	m_pActiveTa->m_sFilePath.Copy(sFileName);
	return TRUE;
}

BOOL CDrawManager::ReadExcleFile(const char* sXlsFileName)
{
	CExcelOperObject xExcelObj;
	if (!xExcelObj.OpenExcelFile(sXlsFileName))
		return FALSE;
	LPDISPATCH pWorksheets = xExcelObj.GetWorksheets();
	ASSERT(pWorksheets != NULL);
	Sheets       excel_sheets;	//��������
	excel_sheets.AttachDispatch(pWorksheets);
	int nSheetNum = excel_sheets.GetCount();
	if (nSheetNum != 2)
	{
		excel_sheets.ReleaseDispatch();
		return FALSE;
	}
	CVariant2dArray sheet1ContentMap(1, 1), sheet2ContentMap(1, 1);
	for (int iSheet = 1; iSheet <= nSheetNum; iSheet++)
	{
		LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		_Worksheet  excel_sheet;
		excel_sheet.AttachDispatch(pWorksheet);
		excel_sheet.Select();
		Range excel_usedRange, excel_range;
		excel_usedRange.AttachDispatch(excel_sheet.GetUsedRange());
		excel_range.AttachDispatch(excel_usedRange.GetRows());
		long nRowNum = excel_range.GetCount();
		excel_range.AttachDispatch(excel_usedRange.GetColumns());
		long nColNum = excel_range.GetCount();
		CXhChar16 sCell("%C%d", 'A' + nColNum, nRowNum + 2);
		LPDISPATCH pRange = excel_sheet.GetRange(COleVariant("A1"), COleVariant(sCell));
		excel_range.AttachDispatch(pRange, FALSE);
		if (iSheet == 1)
			sheet1ContentMap.var = excel_range.GetValue();
		else
			sheet2ContentMap.var = excel_range.GetValue();
		excel_usedRange.ReleaseDispatch();
		excel_range.ReleaseDispatch();
		excel_sheet.ReleaseDispatch();
	}
	excel_sheets.ReleaseDispatch();
	//���������ļ���
	VARIANT value;
	CHashStrList<CXhChar100> hashModelFile;
	for (int i = 1; i < sheet2ContentMap.RowsCount(); i++)
	{
		//����
		sheet2ContentMap.GetValueAt(i, 0, value);
		if (value.vt == VT_EMPTY)
			continue;
		CXhChar100 sTowerType = VariantToString(value);
		//�ļ�����
		sheet2ContentMap.GetValueAt(i, 1, value);
		CXhChar100 sFileName = VariantToString(value);
		//�ļ�·��
		sheet2ContentMap.GetValueAt(i, 2, value);
		CXhChar100 sPath = VariantToString(value);
		//
		hashModelFile.SetValue(sTowerType, CXhChar100("%s\\%s", (char*)sPath, (char*)sFileName));
	}
	//�������������
	CHashStrList<DWORD> hashColIndexByColTitle;
	for (int i = 0; i < LOFTING_EXCEL_COL_COUNT; i++)
	{
		sheet1ContentMap.GetValueAt(0, i, value);
		CString itemstr(value.bstrVal);
		if (itemstr.CompareNoCase(T_NO) == 0)
			hashColIndexByColTitle.SetValue(T_NO, i);
		else if (itemstr.CompareNoCase(T_TOWER_NUM) == 0)
			hashColIndexByColTitle.SetValue(T_TOWER_NUM, i);
		else if (itemstr.CompareNoCase(T_TOWER_DIAN) == 0)
			hashColIndexByColTitle.SetValue(T_TOWER_DIAN, i);
		else if (itemstr.CompareNoCase(T_TOWER_TYPE) == 0)
			hashColIndexByColTitle.SetValue(T_TOWER_TYPE, i);
		else if (itemstr.CompareNoCase(T_WIRE_ANGLE) == 0)
			hashColIndexByColTitle.SetValue(T_WIRE_ANGLE, i);
		else if (strstr(itemstr, T_LOCATE_HIGH))
			hashColIndexByColTitle.SetValue(T_LOCATE_HIGH, i);
		else if (strstr(itemstr, T_HUGAO_HEIGHT))
			hashColIndexByColTitle.SetValue(T_HUGAO_HEIGHT, i);
		else if (strstr(itemstr, T_BODY_HEIGHTP))
			hashColIndexByColTitle.SetValue(T_BODY_HEIGHTP, i);
		else if (strstr(itemstr, T_LEG_LENGTH))
			hashColIndexByColTitle.SetValue(T_LEG_LENGTH, i);
	}
	if (hashColIndexByColTitle.GetValue(T_NO) == NULL ||
		hashColIndexByColTitle.GetValue(T_TOWER_NUM) == NULL ||
		hashColIndexByColTitle.GetValue(T_TOWER_DIAN) == NULL ||
		hashColIndexByColTitle.GetValue(T_TOWER_TYPE) == NULL ||
		hashColIndexByColTitle.GetValue(T_HUGAO_HEIGHT) == NULL ||
		hashColIndexByColTitle.GetValue(T_BODY_HEIGHTP) == NULL ||
		hashColIndexByColTitle.GetValue(T_LOCATE_HIGH) == NULL ||
		hashColIndexByColTitle.GetValue(T_LEG_LENGTH) == NULL)
	{
		xMyErrLog.Log("�ļ���ʽ����");
		return FALSE;
	}
	//��ȡ�ļ�����ȡ����
	DisplayProcess(0, "��ȡExel�ļ�,����ģ�ͽ���");
	int nRows = sheet1ContentMap.RowsCount();
	for (int i = 1; i <= nRows; i++)
	{
		DisplayProcess(int(100 * i / nRows), "��ȡExel�ļ�,����ģ�ͽ���");
		//���
		DWORD *pColIndex = hashColIndexByColTitle.GetValue(T_NO);
		sheet1ContentMap.GetValueAt(i, *pColIndex, value);
		if (value.vt == VT_EMPTY)
			continue;
		//��λ��
		pColIndex = hashColIndexByColTitle.GetValue(T_TOWER_NUM);
		sheet1ContentMap.GetValueAt(i, *pColIndex, value);
		CXhChar16 sTowerNum = VariantToString(value);
		//��λ��
		pColIndex = hashColIndexByColTitle.GetValue(T_TOWER_DIAN);
		sheet1ContentMap.GetValueAt(i, *pColIndex, value);
		CXhChar16 sTowerDian = VariantToString(value);
		//����
		pColIndex = hashColIndexByColTitle.GetValue(T_TOWER_TYPE);
		sheet1ContentMap.GetValueAt(i, *pColIndex, value);
		CXhChar50 sTowerType = VariantToString(value);
		//��λ��
		pColIndex = hashColIndexByColTitle.GetValue(T_LOCATE_HIGH);
		sheet1ContentMap.GetValueAt(i, *pColIndex, value);
		CXhChar16 sLocateH = VariantToString(value);
		//��·ת��
		pColIndex = hashColIndexByColTitle.GetValue(T_WIRE_ANGLE);
		sheet1ContentMap.GetValueAt(i, *pColIndex, value);
		CXhChar50 sWireAngle = VariantToString(value);
		//���Ƹ�
		pColIndex = hashColIndexByColTitle.GetValue(T_HUGAO_HEIGHT);
		sheet1ContentMap.GetValueAt(i, *pColIndex, value);
		double fHuGao = atof(VariantToString(value));
		CXhChar50 sHuGao = VariantToString(value);
		sHuGao.Append("m");
		//�����
		pColIndex = hashColIndexByColTitle.GetValue(T_BODY_HEIGHTP);
		sheet1ContentMap.GetValueAt(i, *pColIndex, value);
		double fBodyHeight = atof(VariantToString(value));
		//���ȳ���
		double fLegA = 0, fLegB = 0, fLegC = 0, fLegD = 0;
		pColIndex = hashColIndexByColTitle.GetValue(T_LEG_LENGTH);
		DWORD dwLegIndex = *pColIndex;
		for (int j = 0; j < 4; j++)
		{
			sheet1ContentMap.GetValueAt(i, dwLegIndex, value);
			if (j == 0)
				fLegA = atof(VariantToString(value));
			else if (j == 1)
				fLegB = atof(VariantToString(value));
			else if (j == 2)
				fLegC = atof(VariantToString(value));
			else if (j == 3)
				fLegD = atof(VariantToString(value));
			dwLegIndex++;
		}
		//��ȡģ���ļ�������ȡ����
		m_pActiveTa = m_xDrawModelList.append();
		m_pActiveTa->SetManager(this);
		m_pActiveTa->m_sTowerPlaceCode.Copy(sTowerNum);
		m_pActiveTa->m_sTowerDot.Copy(sTowerDian);
		m_pActiveTa->m_sTowerType.Copy(sTowerType);
		m_pActiveTa->m_sHuGao.Copy(sHuGao);
		m_pActiveTa->m_fLocateH = atof(sLocateH);	//��λ��
		m_pActiveTa->m_fBodyHeight = fBodyHeight;	//�����
		if (strstr(sWireAngle, "��"))
			m_pActiveTa->m_bTurnLeft = TRUE;
		m_pActiveTa->m_fMaxLegHeight = fHuGao - fBodyHeight;
		CXhChar100* pFullFilePath = hashModelFile.GetValue(sTowerType);
		if (pFullFilePath == NULL)
		{	//�Ҳ������Ͷ�Ӧ���ļ�
			m_pActiveTa->m_ciErrorType = CDrawModel::ERROR_NO_FILE;
			continue;
		}
		CXhChar16 extension;
		m_pActiveTa->m_sFilePath.Copy(*pFullFilePath);
		_splitpath(m_pActiveTa->m_sFilePath, NULL, NULL, NULL, extension);
		if (extension.EqualNoCase(".tid"))
		{
			m_pActiveTa->m_ciModelFlag = CDrawModel::TYPE_TID;
			if (m_pTidModel == NULL)
				m_pTidModel = CTidModelFactory::CreateTidModel();
		}
		else if (extension.EqualNoCase(".lds"))
			m_pActiveTa->m_ciModelFlag = CDrawModel::TYPE_LDS;
		m_pActiveTa->ExtractActiveTaInstance(sHuGao, fLegA, fLegB, fLegC, fLegD);
	}
	DisplayProcess(100, "��ȡExel�ļ�,����ģ�ͽ���");
	//��ʾ��ȡ�����Ϣ
	int nSumNum = GetTaModelNum(), nErrNum = 0;
	for (CDrawModel* pTower = EnumFirstTaModel(); pTower; pTower = EnumNextTaModel())
	{
		if (pTower->m_ciErrorType > 0)
			nErrNum++;
	}
	int nRightNum = nSumNum - nErrNum;
	xMyErrLog.Log(CXhChar100("��ȡ��λ����%d������ȡ�ɹ�%d������ȡʧ��%d��", nSumNum, nRightNum, nErrNum));
	//
	for (CDrawModel* pTower = EnumFirstTaModel(); pTower; pTower = EnumNextTaModel())
	{
		if (pTower->m_ciErrorType == 0)
			continue;
		if (pTower->m_ciErrorType == CDrawModel::ERROR_NO_FILE)
			xMyErrLog.Log("����%s-%s�Ҳ�����Ӧ���ļ�!", (char*)pTower->m_sTowerPlaceCode, (char*)pTower->m_sTowerType);
		else if (pTower->m_ciErrorType == CDrawModel::ERROR_READ_FILE)
			xMyErrLog.Log("����%s-%s��Ӧ���ļ���ȡʧ��!", (char*)pTower->m_sTowerPlaceCode, (char*)pTower->m_sTowerType);
		else if (pTower->m_ciErrorType == CDrawModel::ERROR_FIND_HUGAO)
			xMyErrLog.Log("����%s-%s�в�����%s����!", (char*)pTower->m_sTowerPlaceCode, (char*)pTower->m_sTowerType, (char*)pTower->m_sHuGao);
		else if (pTower->m_ciErrorType == CDrawModel::ERROR_FIND_LEG)
			xMyErrLog.Log("����%s-%s��%s������������Ϣ����!", (char*)pTower->m_sTowerPlaceCode, (char*)pTower->m_sTowerType, (char*)pTower->m_sHuGao);
		else if (pTower->m_ciErrorType == CDrawModel::ERROR_FIND_INSTANCE)
			xMyErrLog.Log("����%s-%s�������Ϣ����!", (char*)pTower->m_sTowerPlaceCode, (char*)pTower->m_sTowerType);
	}
	return TRUE;
}

BOOL CDrawManager::ReadTxtFile(const char* sFileName)
{
	FILE* fp = fopen(sFileName, "rt");
	if (fp == NULL)
		return FALSE;
	CDrawModel* pDrawModel = NULL;
	CXhChar100 lineText, sTowerName;
	while (!ferror(fp) && !feof(fp))
	{
		lineText.Empty();
		fgets(lineText, 100, fp);
		if (lineText.GetLength() == 0)
			continue;
		lineText.Replace("��", " ");	//��ȫ�ǿո񻻳ɰ�ǿո��Ա�ͳһ����
		CXhChar100 copyLnText = lineText;
		char* splitter = " =\t\r\n";
		char* skey = strtok(copyLnText, splitter);
		if (skey == NULL || strlen(skey) == 0)
			continue;	//����
		if (strchr(lineText, ',') == NULL)
		{	//��ȡ��������
			sTowerName = lineText;
			sTowerName.Replace('\n', 0);
			continue;
		}
		//��ȡ���߼�������Ϣ
		CXhChar50 sHuGao;
		char cLegMark;
		for (skey = strtok(lineText, ","); skey; skey = strtok(NULL, ","))
		{
			char cLegMode = 0;
			CXhChar50 sValue(skey);
			if (strchr(sValue, '-') != NULL)
				cLegMode = '-';
			else if (strchr(sValue, '+') != NULL)
				cLegMode = '+';
			if (cLegMode == 0)
				sHuGao = sValue;
			else
			{
				sValue.Replace('-', ' ');
				sValue.Replace('+', ' ');
				sscanf(sValue, "%c%s", &cLegMark, skey);
				//
				pDrawModel = m_xDrawModelList.append();
				pDrawModel->SetManager(this);
				pDrawModel->m_ciModelFlag = CDrawModel::TYPE_LDS;
				pDrawModel->m_sTowerType.Copy(sTowerName);
				pDrawModel->m_sHuGao.Copy(sHuGao);
				pDrawModel->m_cLegMark = cLegMark;
				pDrawModel->m_sLegDes.Printf("%c%s", cLegMode, skey);
			}
		}
	}
	fclose(fp);
	//��ʼ��
	CString file_path(sFileName);
	int index = file_path.ReverseFind('\\');	//�������'\\'
	file_path = file_path.Left(index);		//�Ƴ��ļ���
	file_path += "\\";
	CXhChar16 sExtensions;
#ifdef __LMA_FILE_
	sExtensions.Copy(".lma");
#elif defined(__TMA_FILE_)
	sExtensions.Copy(".tma");
#elif defined(__TSA_FILE_)
	sExtensions.Copy(".tsa");
#elif defined(__TDA_FILE_)
	sExtensions.Copy(".tda");
#else
	sExtensions.Copy(".lds");
#endif
	int i=1,nNum = GetTaModelNum();
	DisplayProcess(0, "��ȡtxt�ļ�,����ģ�ͽ���");
	for (pDrawModel = EnumFirstTaModel(); pDrawModel; pDrawModel = EnumNextTaModel(),i++)
	{	//�����������ƣ�����Ӧ�������ļ�
		DisplayProcess(int(100 * i / nNum), "��ȡtxt�ļ�,����ģ�ͽ���");
		pDrawModel->m_sFilePath.Printf("%s%s%s", file_path, (char*)pDrawModel->m_sTowerType, (char*)sExtensions);
		CloseTaFile(FALSE, TRUE);
		OpenTaFile(CString(pDrawModel->m_sFilePath), FALSE, TRUE);
		//
		CLDSModule* pModule = NULL;
		for (pModule = Ta.Module.GetFirst(); pModule; pModule = Ta.Module.GetNext())
		{
			if (strcmp(pModule->description, pDrawModel->m_sHuGao) == 0)
				break;
		}
		if (pModule == NULL)
		{
			pDrawModel->m_ciErrorType = CDrawModel::ERROR_FIND_HUGAO;
			continue;
		}
		//�������
		int nMaxLegs = CFGLEG::MaxLegs();
		pDrawModel->m_uiHeightSerial = pModule->GetBodyNo();
		pDrawModel->m_uiLegSerialArr[0] = pDrawModel->m_cLegMark - '@';
		pDrawModel->m_uiLegSerialArr[1] = pDrawModel->m_cLegMark - '@';
		pDrawModel->m_uiLegSerialArr[2] = pDrawModel->m_cLegMark - '@';
		pDrawModel->m_uiLegSerialArr[3] = pDrawModel->m_cLegMark - '@';
	}
	DisplayProcess(100, "��ȡtxt�ļ�,����ģ�ͽ���");
	//�رյ�ǰ�����ļ�
	CloseTaFile(FALSE, TRUE);
	return TRUE;
}
#endif
//////////////////////////////////////////////////////////////////////////
//�ڹ���ʵ���Ͻ��д�ײ���
void MakeLsHole(CLDSPart *pPart, AcDbObjectId mainSolidId, UCS_STRU* pUcs = NULL, double scale = 1,
					AcDbBlockTableRecord *pTemRecord = NULL, int idCurrPlaneNo = -1)
{
	CXhSimpleList<CLDSPart::HOLE> hole_list;
	pPart->GetHoleSet(&hole_list);
	for (CLDSPart::HOLE* pHole = hole_list.EnumObjectFirst(); pHole; pHole = hole_list.EnumObjectNext())
	{
		if (pHole->pBolt && pHole->pBolt->m_bVirtualPart == FALSE)
			continue;	//����ʾ��������˨��
		int idPlaneNo = pHole->ciResidePlane;
		if (pHole->pBolt && pPart->IsPlate())
			idPlaneNo = ((CLDSGeneralPlate*)pPart)->GetNormFaceI(pHole->pBolt);
		if (idCurrPlaneNo >= 0 && idCurrPlaneNo != idPlaneNo)
			continue;	//��ָ�����ϵĿ�
		long d = 0;
		double L = 0, hold_d = 0;
		f3dPoint orgPt, norm, axisX;
		if (pHole->pBolt)
		{
			pHole->pBolt->GetUCS();
			pPart->GetBoltIntersPos(pHole->pBolt, orgPt, NULL, CLDSPart::BOLTBTM_PLANE);
			norm = pHole->pBolt->ucs.axis_z;
			axisX = pHole->pBolt->ucs.axis_x;
			d = pHole->pBolt->get_d();
			L = pHole->pBolt->get_L();
			hold_d = d + pHole->pBolt->hole_d_increment;
		}
		else
		{
			orgPt = pHole->xLocalPosition;
			coord_trans(orgPt, pPart->ucs, TRUE, TRUE);
			norm = pPart->ucs.axis_z;
			axisX = pPart->ucs.axis_x;
			d = pHole->wBoltD;
			L = pPart->GetThick() + 4;
			hold_d = d + pHole->fHoleMargin;
		}
		double D, H, H2;
		FindLsD_H(d, &D, &H, &H2);
		f3dPoint vertex_arr[4];
		vertex_arr[0] = orgPt - norm * H;
		vertex_arr[1] = orgPt + norm * L;
		vertex_arr[2] = vertex_arr[1] + axisX * hold_d*0.5;
		vertex_arr[3] = vertex_arr[0] + axisX * hold_d*0.5;
		fPtList vertex_list;
		for (int i = 0; i < 4; i++)
		{
			TransPtToCadUcs(vertex_arr[i], pUcs, scale);
			vertex_list.append(vertex_arr[i]);
		}
		//
		SubtractSubSolid(mainSolidId, vertex_list, f3dLine(vertex_arr[0], vertex_arr[1]), 1, RGB(0, 255, 128), pTemRecord, 2 * Pi);
	}
}
//���Ƶ����ʵ�壬�����Բ���������
AcDbObjectId Draw1FacePlate(CLDSGeneralPlate* pPlate, UCS_STRU* pUcs /*= NULL*/, double scale /*= 1*/, 
							COLORREF* pCrSolid /*=NULL*/, AcGeMatrix3d *pMatrix /*=NULL*/)
{
	if (pPlate == NULL)
		return NULL;
	double z_offset = pPlate->GetNormOffset();
	fPtList vertex_list;
	PROFILE_VERTEX *pPrevVertex = pPlate->vertex_list.GetTail();
	for (PROFILE_VERTEX *pVertex = pPlate->vertex_list.GetFirst(); pVertex; pVertex = pPlate->vertex_list.GetNext())
	{
		f3dPoint vertex = pVertex->vertex;
		if (pPrevVertex->type != 0)
		{	//����Բ�����
			int sign = 1;
			f3dArcLine arcLine;
			if (pPrevVertex->type == 1)//ָ��Բ���н�(�Զ�����Բ���нǵ�����������Զ��������ȷֵ)
				arcLine.CreateMethod2(pPrevVertex->vertex, pVertex->vertex, f3dPoint(0, 0, 1), pPrevVertex->sector_angle);
			else if (pPrevVertex->type == 2)	//ָ��R
				arcLine.CreateMethod3(pPrevVertex->vertex, pVertex->vertex, f3dPoint(0, 0, 1), pPrevVertex->radius, pPrevVertex->center);
			else if (pPrevVertex->type == 3)	//��Բ��
			{
				f3dPoint center = pPrevVertex->center;
				f3dPoint col_axis = pPrevVertex->column_norm;
				coord_trans(center, pPlate->ucs, FALSE);
				vector_trans(col_axis, pPlate->ucs, FALSE);
				if (pPrevVertex->radius < 0)
					sign = -1;
				arcLine.CreateEllipse(center, pPrevVertex->vertex, pVertex->vertex, col_axis, f3dPoint(0, 0, sign), sign*pPrevVertex->radius);
			}
			//
			fPtList sub_vertex_list;
			CalArcLineVertexList(arcLine, sub_vertex_list);
			for (f3dPoint *pPt = sub_vertex_list.GetFirst(); pPt; pPt = sub_vertex_list.GetNext())
			{
				f3dPoint temp_vertex(pPt->x, pPt->y, pPt->z + z_offset);
				coord_trans(temp_vertex, pPlate->ucs, TRUE);
				TransPtToCadUcs(temp_vertex, pUcs, scale);
				vertex_list.append(temp_vertex);
			}
		}
		else
		{
			f3dPoint temp_vertex(vertex.x, vertex.y, vertex.z + z_offset);
			coord_trans(temp_vertex, pPlate->ucs, TRUE);
			TransPtToCadUcs(temp_vertex, pUcs, scale);
			vertex_list.append(temp_vertex);
		}
		pPrevVertex = pVertex;
	}
	//��������·������������ʵ��
	f3dLine path;
	path.startPt = pPlate->vertex_list.GetFirst()->vertex;
	path.endPt.Set(path.startPt.x, path.startPt.y, path.startPt.z + pPlate->GetThick());
	coord_trans(path.startPt, pPlate->ucs, TRUE);
	coord_trans(path.endPt, pPlate->ucs, TRUE);
	TransPtToCadUcs(path.startPt, pUcs, scale);
	TransPtToCadUcs(path.endPt, pUcs, scale);
	COLORREF crSolid = RGB(0, 255, 128);
	if (pCrSolid)
		crSolid = *pCrSolid;
	AcDbObjectId mainPlateSolidId;
	AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
	if (pBlockTableRecord == NULL)
		return NULL;
	mainPlateSolidId = CreateSolidByExtrude(pBlockTableRecord, vertex_list, path, crSolid, pPlate->handle);
	MakeLsHole(pPlate, mainPlateSolidId, pUcs, scale, pBlockTableRecord);
	pBlockTableRecord->close();
	//��ʵ��ͼԪλ�ý��е���
	if (pMatrix)
	{
		AcDb3dSolid *p3dObj = NULL;
		acdbOpenObject(p3dObj, mainPlateSolidId, AcDb::kForWrite);
		if (p3dObj)
		{
			p3dObj->transformBy(*pMatrix);
			p3dObj->close();
		}
	}
	return mainPlateSolidId;
}

//����˫���ʵ�壬�����Բ���������
AcDbObjectId Draw2FacePlate(CLDSGeneralPlate* pPlate, UCS_STRU* pUcs /*= NULL*/, double scale /*=1*/, 
							COLORREF* pCrSolid /*=NULL*/, AcGeMatrix3d *pMatrix /*=NULL*/)
{
	if (pPlate == NULL)
		return NULL;
	f3dPoint huoqu_norm = pPlate->GetHuoquFaceNorm(0);
	double z_offset = pPlate->GetNormOffset();
	BOOL bInitPath = TRUE, bInit2FacePath = TRUE;
	f3dLine path[2];
	fPtList vertex_list[2];
	//�����ְ�ڵ㣬����ʵ��
	PROFILE_VERTEX *pPrevVertex = pPlate->vertex_list.GetTail();
	for (PROFILE_VERTEX *pVertex = pPlate->vertex_list.GetFirst(); pVertex; pVertex = pPlate->vertex_list.GetNext())
	{
		f3dPoint vertex = pVertex->vertex;
		vertex.z += z_offset;
		//����Բ�����
		int feature = min(pVertex->vertex.feature, pPrevVertex->vertex.feature);
		if (pPrevVertex->type != 0 && feature <= 2)
		{
			int sign = 1;
			f3dArcLine arcLine;
			if (pPrevVertex->type == 1)//ָ��Բ���н�(�Զ�����Բ���нǵ�����������Զ��������ȷֵ)
				arcLine.CreateMethod2(pPrevVertex->vertex, pVertex->vertex, f3dPoint(0, 0, sign), pPrevVertex->sector_angle);
			else if (pPrevVertex->type == 2)	//ָ��R
				arcLine.CreateMethod3(pPrevVertex->vertex, pVertex->vertex, f3dPoint(0, 0, 1), pVertex->radius, pPrevVertex->center);
			else if (pPrevVertex->type == 3)	//��Բ��
			{
				f3dPoint center = pPrevVertex->center;
				f3dPoint col_axis = pPrevVertex->column_norm;
				coord_trans(center, pPlate->ucs, FALSE);
				vector_trans(col_axis, pPlate->ucs, FALSE);
				if (pPrevVertex->radius < 0)
					sign = -1;
				arcLine.CreateEllipse(center, pPrevVertex->vertex, pVertex->vertex, col_axis, f3dPoint(0, 0, sign), sign*pPrevVertex->radius);
			}
			//
			fPtList sub_vertex_list;
			if (pPrevVertex->vertex.feature > 10)	//ǰһ������Ϊ������ʱ���ڴ˴���Ӹö���
				sub_vertex_list.append(arcLine.Start());
			CalArcLineVertexList(arcLine, sub_vertex_list);
			for (f3dPoint *pPt = sub_vertex_list.GetFirst(); pPt; pPt = sub_vertex_list.GetNext())
			{
				f3dPoint temp_vertex = pPlate->GetRealVertex(*pPt);
				coord_trans(temp_vertex, pPlate->ucs, TRUE);
				TransPtToCadUcs(temp_vertex);
				if (feature == 1)
					vertex_list[0].append(temp_vertex);
				else if (feature == 2)
					vertex_list[1].append(temp_vertex);
			}
		}
		else
		{
			vertex = pPlate->GetRealVertex(vertex);
			coord_trans(vertex, pPlate->ucs, TRUE);
			f3dPoint temp_vertex = vertex;
			if (bInitPath&&feature == 1)
			{
				path[0].startPt = temp_vertex;
				path[0].endPt = temp_vertex + pPlate->ucs.axis_z*pPlate->GetThick();
				bInitPath = FALSE;
			}
			if (bInit2FacePath&&feature == 2)
			{
				path[1].startPt = temp_vertex;
				path[1].endPt = temp_vertex + huoqu_norm * pPlate->GetThick();
				bInit2FacePath = FALSE;
			}
			TransPtToCadUcs(temp_vertex);
			if (feature == 1 || feature > 10)
				vertex_list[0].append(temp_vertex);
			if (feature == 2 || feature > 10)
				vertex_list[1].append(temp_vertex);
		}
		pPrevVertex = pVertex;
	}
	COLORREF crSolid = RGB(0, 255, 128);
	if (pCrSolid)
		crSolid = *pCrSolid;
	//��������·������������ʵ��
	AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
	if (pBlockTableRecord == NULL)
		return NULL;
	AcDbObjectId solidId[2];
	for (int i = 0; i < 2; i++)
	{
		TransPtToCadUcs(path[i].startPt, pUcs, scale);
		TransPtToCadUcs(path[i].endPt, pUcs, scale);
		solidId[i] = CreateSolidByExtrude(pBlockTableRecord, vertex_list[i], path[i], crSolid, pPlate->handle);
		//������˨��
		MakeLsHole(pPlate, solidId[i], pUcs, scale, pBlockTableRecord, i);
	}
	pBlockTableRecord->close();
	//�ϲ�ʵ��
	AcDb3dSolid *p3dObj1 = NULL, *p3dObj2 = NULL;
	acdbOpenObject(p3dObj1, solidId[0], AcDb::kForWrite);
	acdbOpenObject(p3dObj2, solidId[1], AcDb::kForWrite);
	if (p3dObj1 == NULL)
		return NULL;
	if (p3dObj2)
		p3dObj1->booleanOper(AcDb::kBoolUnite, p3dObj2);
	if (p3dObj2)
		p3dObj2->close();
	p3dObj1->close();
	//��ʵ��ͼԪλ�ý��е���
	if (pMatrix)
	{
		acdbOpenObject(p3dObj1, solidId[0], AcDb::kForWrite);
		p3dObj1->transformBy(*pMatrix);
		p3dObj1->close();
	}
	return solidId[0];
}
//////////////////////////////////////////////////////////////////////////
//����TMA�ļ��й���ʵ��
//���ƽǸ�ʵ��
AcDbObjectId DrawSolidAngle(CLDSLineAngle *pLinkAngle, UCS_STRU* pUcs/*=NULL*/, double scale /*=1*/,
					COLORREF* pCrSolid /*= NULL*/, AcGeMatrix3d *pMatrix /*= NULL*/)
{
	if(pLinkAngle==NULL)
		return AcDbObjectId::kNull;
	pLinkAngle->getUCS(pLinkAngle->ucs);
	f3dPoint vertex_arr[8];
	f3dPoint norm_X, norm_Y, wing_x_vec, wing_y_vec, bi_vec;
	//���Y֫�����������ϵ�µķ���
	norm_Y = pLinkAngle->get_norm_y_wing();
	vector_trans(norm_Y, pLinkAngle->ucs, FALSE);
	norm_Y.z = 0;
	normalize(norm_Y);
	//���Y֫�������ϵ�µ����췽��
	wing_y_vec.Set(norm_Y.y, -norm_Y.x);
	wing_x_vec.Set(1, 0, 0);
	norm_X.Set(0, -1, 0);
	bi_vec.Set(1 + wing_y_vec.x, wing_y_vec.y, 0);
	normalize(bi_vec);
	vertex_arr[0].x = 0;
	vertex_arr[0].y = 0;
	vertex_arr[0].z = -pLinkAngle->startOdd();

	vertex_arr[1].x = pLinkAngle->size_wide*(wing_y_vec.x);
	vertex_arr[1].y = pLinkAngle->size_wide*wing_y_vec.y;
	vertex_arr[1].z = vertex_arr[0].z;

	vertex_arr[2].x = pLinkAngle->size_wide*wing_y_vec.x - pLinkAngle->size_thick*norm_Y.x;
	vertex_arr[2].y = pLinkAngle->size_wide*wing_y_vec.y - pLinkAngle->size_thick*norm_Y.y;
	vertex_arr[2].z = vertex_arr[0].z;

	vertex_arr[3].x = pLinkAngle->size_thick*bi_vec.x / bi_vec.y;
	vertex_arr[3].y = pLinkAngle->size_thick;
	vertex_arr[3].z = vertex_arr[0].z;

	vertex_arr[4].x = pLinkAngle->size_wide;
	vertex_arr[4].y = pLinkAngle->size_thick;
	vertex_arr[4].z = vertex_arr[0].z;

	vertex_arr[5].x = pLinkAngle->size_wide;
	vertex_arr[5].y = 0;
	vertex_arr[5].z = vertex_arr[0].z;
	fPtList vertex_list;
	for (int i = 0; i < 6; i++)
	{
		coord_trans(vertex_arr[i], pLinkAngle->ucs, TRUE);
		TransPtToCadUcs(vertex_arr[i], pUcs, scale);
		vertex_list.append(vertex_arr[i]);
	}
	//��¼�����߶���ʼ�����ֹ��
	f3dLine path;
	path.startPt = pLinkAngle->Start() - pLinkAngle->ucs.axis_z*pLinkAngle->startOdd();
	path.endPt = pLinkAngle->End() + pLinkAngle->ucs.axis_z*pLinkAngle->endOdd();
	TransPtToCadUcs(path.startPt, pUcs, scale);
	TransPtToCadUcs(path.endPt, pUcs, scale);
	COLORREF crSolid;
	if (pLinkAngle->pStart == NULL || pLinkAngle->pEnd == NULL)
		crSolid = RGB(90, 90, 90);
	else
		crSolid = RGB(150, 150, 255);
	if (pCrSolid)
		crSolid = *pCrSolid;
	//���ƽǸ�ʵ��
	AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
	if (pBlockTableRecord == NULL)
		return NULL;
	AcDbObjectId solidId;
	solidId = CreateSolidByExtrude(pBlockTableRecord, vertex_list, path, crSolid, pLinkAngle->handle);
	MakeLsHole(pLinkAngle, solidId, pUcs, scale, pBlockTableRecord);	//������˨��
	pBlockTableRecord->close();
	//���ƽǸ�֫��Բ��
	double length = DISTANCE(pLinkAngle->Start(), pLinkAngle->End());
	for (int i = 0; i < 4; i++)
	{
		double fR = 0;
		f3dPoint ptWC, ptBC, ptWR, ptBR, ptOrg, wingNorm;
		if (i == 0 && (fR = pLinkAngle->dfFilletROfSX) > 0)
		{	//ʼ��X֫��Բ��
			wingNorm.Set(0, -1, 0);
			ptWC.Set((pLinkAngle->size_wide - fR)*wing_x_vec.x, (pLinkAngle->size_wide - fR)*wing_x_vec.y, -pLinkAngle->startOdd());
			ptBC.Set(pLinkAngle->size_wide*wing_x_vec.x, pLinkAngle->size_wide*wing_x_vec.y, -pLinkAngle->startOdd() + fR);
			ptWR.Set(ptWC.x, ptWC.y, ptWC.z - pLinkAngle->size_thick);
			ptBR = ptBC + wing_x_vec * pLinkAngle->size_thick;
			ptOrg = ptWR + wing_x_vec * (pLinkAngle->size_thick + fR);
		}
		else if (i == 1 && (fR = pLinkAngle->dfFilletROfSY) > 0)
		{	//ʼ��Y֫��Բ��
			wingNorm = norm_Y;
			ptWC.Set((pLinkAngle->size_wide - fR)*wing_y_vec.x, (pLinkAngle->size_wide - fR)*wing_y_vec.y, -pLinkAngle->startOdd());
			ptBC.Set(pLinkAngle->size_wide*wing_y_vec.x, pLinkAngle->size_wide*wing_y_vec.y, -pLinkAngle->startOdd() + fR);
			ptWR.Set(ptWC.x, ptWC.y, ptWC.z - pLinkAngle->size_thick);
			ptBR = ptBC + wing_y_vec * pLinkAngle->size_thick;
			ptOrg = ptWR + wing_y_vec * (pLinkAngle->size_thick + fR);
		}
		else if (i == 2 && (fR = pLinkAngle->dfFilletROfEX) > 0)
		{	//�ն�X֫��Բ��
			wingNorm.Set(0, -1, 0);
			ptWC.Set((pLinkAngle->size_wide - fR)*wing_x_vec.x, (pLinkAngle->size_wide - fR)*wing_x_vec.y, length + pLinkAngle->endOdd());
			ptBC.Set(pLinkAngle->size_wide*wing_x_vec.x, pLinkAngle->size_wide*wing_x_vec.y, length + pLinkAngle->endOdd() - fR);
			ptWR.Set(ptWC.x, ptWC.y, ptWC.z + pLinkAngle->size_thick);
			ptBR = ptBC + wing_x_vec * pLinkAngle->size_thick;
			ptOrg = ptWR + wing_x_vec * (pLinkAngle->size_thick + fR);
		}
		else if (i == 3 && (fR = pLinkAngle->dfFilletROfEY) > 0)
		{	//�ն�Y֫��Բ��
			wingNorm = norm_Y;
			ptWC.Set((pLinkAngle->size_wide - fR)*wing_y_vec.x, (pLinkAngle->size_wide - fR)*wing_y_vec.y, length + pLinkAngle->endOdd());
			ptBC.Set(pLinkAngle->size_wide*wing_y_vec.x, pLinkAngle->size_wide*wing_y_vec.y, length + pLinkAngle->endOdd() - fR);
			ptWR.Set(ptWC.x, ptWC.y, ptWC.z + pLinkAngle->size_thick);
			ptBR = ptBC + wing_y_vec * pLinkAngle->size_thick;
			ptOrg = ptWR + wing_y_vec * (pLinkAngle->size_thick + fR);
		}
		if (fR <= 0)
			continue;
		//��ȡ������
		f3dPoint ptS(ptWC.x, ptWC.y, ptBC.z);
		f3dPoint ptE = ptS + wingNorm * 10;
		double slice_angle = 0.5*Pi / 15;
		f3dPoint vertexs[19];
		int nNum = 19;
		if (i == 0 || i == 3)
		{
			vertexs[0] = ptBR;
			vertexs[1] = ptOrg;
			vertexs[2] = ptWR;
			vertexs[3] = ptWC;
		}
		else
		{
			vertexs[0] = ptWR;
			vertexs[1] = ptOrg;
			vertexs[2] = ptBR;
			vertexs[3] = ptBC;
		}
		for (int j = 4; j < nNum; j++)
		{
			vertexs[j] = vertexs[j - 1];
			rotate_point_around_axis(vertexs[j], slice_angle, ptS, ptE);
		}
		//��������ϵת��
		fPtList sub_vertex_list;
		for (int j = 0; j < nNum; j++)
		{
			f3dPoint pt = vertexs[j];
			coord_trans(pt, pLinkAngle->ucs, TRUE);
			TransPtToCadUcs(pt, pUcs, scale);
			sub_vertex_list.append(pt);
		}
		path.startPt = ptOrg;
		path.endPt = ptOrg - wingNorm * pLinkAngle->size_thick;
		coord_trans(path.startPt, pLinkAngle->ucs, TRUE);
		coord_trans(path.endPt, pLinkAngle->ucs, TRUE);
		TransPtToCadUcs(path.startPt, pUcs, scale);
		TransPtToCadUcs(path.endPt, pUcs, scale);
		//����ʵ�壬����ʵ������ʵ���󲢼�������ʵ���г�ȥ�ص����֣�
		SubtractSubSolid(solidId, sub_vertex_list, path, 0, crSolid);
	}
	//���ƽǸֿ���
	for (UINT i = 0; i < pLinkAngle->xarrOpenCuts.Count; i++)
	{
		f3dPoint wingNorm, vertexs[4];
		CLDSLineAngle::OPENING_CUT cut = pLinkAngle->xarrOpenCuts[i];
		if (cut.dfLateralBtmXorY < -pLinkAngle->size_thick)
		{	//X֫����
			wingNorm.Set(0, -1, 0);
			vertexs[0].Set(-cut.dfLateralBtmXorY*wing_x_vec.x, -cut.dfLateralBtmXorY*wing_x_vec.y, cut.dfLenPosFrom);
			vertexs[1].Set(-cut.dfLateralBtmXorY*wing_x_vec.x, -cut.dfLateralBtmXorY*wing_x_vec.y, cut.dfLenPosTo);
			vertexs[2].Set(pLinkAngle->size_wide*wing_x_vec.x, pLinkAngle->size_wide*wing_x_vec.y, cut.dfLenPosTo + cut.dfTipDeltaE);
			vertexs[3].Set(pLinkAngle->size_wide*wing_x_vec.x, pLinkAngle->size_wide*wing_x_vec.y, cut.dfLenPosFrom + cut.dfTipDeltaS);
		}
		else if (cut.dfLateralBtmXorY > pLinkAngle->size_thick)
		{	//Y֫����
			wingNorm = norm_Y;
			vertexs[0].Set(cut.dfLateralBtmXorY*wing_y_vec.x, cut.dfLateralBtmXorY*wing_y_vec.y, cut.dfLenPosTo);
			vertexs[1].Set(cut.dfLateralBtmXorY*wing_y_vec.x, cut.dfLateralBtmXorY*wing_y_vec.y, cut.dfLenPosFrom);
			vertexs[2].Set(pLinkAngle->size_wide*wing_y_vec.x, pLinkAngle->size_wide*wing_y_vec.y, cut.dfLenPosFrom + cut.dfTipDeltaS);
			vertexs[3].Set(pLinkAngle->size_wide*wing_y_vec.x, pLinkAngle->size_wide*wing_y_vec.y, cut.dfLenPosTo + cut.dfTipDeltaE);
		}
		else
			continue;
		fPtList sub_vertex_list;
		for (int j = 0; j < 4; j++)
		{
			f3dPoint pt = vertexs[j];
			coord_trans(pt, pLinkAngle->ucs, TRUE);
			TransPtToCadUcs(pt, pUcs, scale);
			sub_vertex_list.append(pt);
		}
		path.startPt = vertexs[0] + wingNorm * 2;
		path.endPt = vertexs[0] - wingNorm * (pLinkAngle->size_thick + 2);
		coord_trans(path.startPt, pLinkAngle->ucs, TRUE);
		coord_trans(path.endPt, pLinkAngle->ucs, TRUE);
		TransPtToCadUcs(path.startPt, pUcs, scale);
		TransPtToCadUcs(path.endPt, pUcs, scale);
		//
		SubtractSubSolid(solidId, sub_vertex_list, path, 0, crSolid);
	}
	//��ʵ��ͼԪλ�ý��е���
	if (pMatrix)
	{
		AcDb3dSolid *p3dObj = NULL;
		acdbOpenObject(p3dObj, solidId, AcDb::kForWrite);
		p3dObj->transformBy(*pMatrix);
		p3dObj->close();
	}
	return solidId;
}
//������ͨ�հ�ʵ��
AcDbObjectId DrawSolidPlate(CLDSPlate *pPlate, UCS_STRU* ucs /*=NULL*/, double scale /*=1*/,
					COLORREF* pCrSolid /*=NULL*/, AcGeMatrix3d *pMatrix /*=NULL*/)
{
	if (pPlate == NULL)
		return AcDbObjectId::kNull;
	double radius = pPlate->m_fInnerRadius;
	if (pPlate->face_N == 1)
		return Draw1FacePlate(pPlate, ucs, scale, pCrSolid, pMatrix);
	else if (pPlate->face_N >= 2)
	{
		int i = 0, j = 0, k = 0;
		f3dPoint pt3d[100], pt3d1[100], pt3d3[100], endPt, endPt1, endPt2, pt1, pt2, pt3, pt4, ptHuoqu1, ptHuoqu2, ptHuoqu3, ptHuoqu4,
			top_point2, top_point3;
		double angle = cal_angle_of_2vec(pPlate->ucs.axis_z, pPlate->huoqufacenorm[0]);
		double angle1 = cal_angle_of_2vec(pPlate->ucs.axis_z, pPlate->huoqufacenorm[1]);
		if (pPlate->face_N > 2 && pPlate->jdb_style != 3)//�����top_point
		{
			top_point2 = pPlate->top_point;
			rotate_point_around_axis(top_point2, angle, pPlate->HuoQuLine[0]);
			coord_trans(top_point2, pPlate->ucs, TRUE);
			TransPtToCadUcs(top_point2, ucs, scale);
			//
			top_point3 = pPlate->top_point;
			rotate_point_around_axis(top_point3, angle1, pPlate->HuoQuLine[1]);
			coord_trans(top_point3, pPlate->ucs, TRUE);
			TransPtToCadUcs(top_point3, ucs, scale);
			//
			f3dPoint top_point = pPlate->top_point;
			coord_trans(top_point, pPlate->ucs, TRUE);
			pt2 = top_point;
			TransPtToCadUcs(pt2, ucs, scale);
			pt4 = pt2;
			//
			ptHuoqu2 = top_point + pPlate->ucs.axis_z*pPlate->GetThick();
			TransPtToCadUcs(ptHuoqu2, ucs, scale);
			ptHuoqu4 = ptHuoqu2;
		}
		//pt3d�����洢���ϵĶ��㣬endpt��¼�������ת��׼���յ㣬pt��huoqu��¼������
		BOOL bFirst = TRUE, bFirst1 = TRUE;
		int nID = 0;
		for (PROFILE_VERTEX* pVertex = pPlate->vertex_list.GetFirst(); pVertex; pVertex = pPlate->vertex_list.GetNext())
		{
			f3dPoint vertex = pVertex->vertex;
			//�ǻ�׼�����Ҫ��ת����Ӧ����
			if (vertex.feature == 2)
				rotate_point_around_axis(vertex, angle, pPlate->HuoQuLine[0]);
			else if (vertex.feature == 3)
				rotate_point_around_axis(vertex, angle1, pPlate->HuoQuLine[1]);
			coord_trans(vertex, pPlate->ucs, TRUE);
			if (vertex.feature == 12)
			{
				if (bFirst)
				{
					pt1 = vertex;
					TransPtToCadUcs(pt1, ucs, scale);
					ptHuoqu1 = vertex + pPlate->ucs.axis_z*pPlate->GetThick();
					TransPtToCadUcs(ptHuoqu1, ucs, scale);
					bFirst = FALSE;
				}
				else
				{
					pt2 = vertex;
					TransPtToCadUcs(pt2, ucs, scale);
					ptHuoqu2 = vertex + pPlate->ucs.axis_z*pPlate->GetThick();
					TransPtToCadUcs(ptHuoqu2, ucs, scale);
				}
			}
			if (vertex.feature == 13)
			{
				if (bFirst1)
				{
					pt3 = vertex;
					TransPtToCadUcs(pt3, ucs, scale);
					ptHuoqu3 = vertex + pPlate->ucs.axis_z*pPlate->GetThick();
					TransPtToCadUcs(ptHuoqu3, ucs, scale);
					bFirst1 = FALSE;
				}
				else
				{
					pt4 = vertex;
					TransPtToCadUcs(pt4, ucs, scale);
					ptHuoqu4 = vertex + pPlate->ucs.axis_z*pPlate->GetThick();
					TransPtToCadUcs(ptHuoqu4, ucs, scale);
				}
			}
			if (vertex.feature != 2 && vertex.feature != 3)
			{
				if (i == 0)
				{
					endPt = vertex + pPlate->ucs.axis_z*pPlate->GetThick();
					TransPtToCadUcs(endPt, ucs, scale);
				}
				f3dPoint vertexTemp = vertex;
				TransPtToCadUcs(vertexTemp, ucs, scale);
				pt3d[i] = vertexTemp;
				i++;
			}
			if (vertex.feature != 1 && vertex.feature != 3 && vertex.feature != 13)
			{
				if (j == 0)
				{
					endPt1 = vertex + pPlate->huoqufacenorm[0] * pPlate->GetThick();
					TransPtToCadUcs(endPt1, ucs, scale);
				}
				f3dPoint vertexTemp = vertex;
				TransPtToCadUcs(vertexTemp, ucs, scale);
				pt3d1[j] = vertexTemp;
				j++;
			}
			if (vertex.feature != 1 && vertex.feature != 2 && vertex.feature != 12)
			{
				if (k == 1)
				{
					endPt2 = vertex + pPlate->huoqufacenorm[1] * pPlate->GetThick();
					TransPtToCadUcs(endPt2, ucs, scale);
				}
				f3dPoint vertexTemp = vertex;
				TransPtToCadUcs(vertexTemp, ucs, scale);
				pt3d3[k] = vertexTemp;
				k++;
			}
		}
		//���ƻ�׼�漰������˨��
		COLORREF crSolid = RGB(0, 255, 128);
		if (pCrSolid)
			crSolid = *pCrSolid;
		if (pPlate->face_N > 2 && pPlate->jdb_style != 3)//���������
		{
			if (pt3d[i - 1] == pt1 || pt3d[i - 1] == pt2
				|| pt3d[0] == pt1 || pt3d[0] == pt2)
			{
				pt3d[i] = pt4;
				i++;
			}
		}
		pt3d[i] = pt3d[0];
		pt3d[i + 1] = endPt;
		//����ʵ��
		AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
		if (pBlockTableRecord == NULL)
			return NULL;
		AcDbObjectId mainObjId, subObj1Id, subObj2Id, subObj3Id, subObj4Id;
		mainObjId = CreateSolidByManner(pBlockTableRecord, pt3d, i, 0, crSolid, pPlate->handle);
		MakeLsHole(pPlate, mainObjId, ucs, scale, pBlockTableRecord, 0);
		//���ƻ����漰������˨��
		if (pPlate->face_N > 2 && pPlate->jdb_style != 3)//���������	
		{
			if (pt3d1[j - 1] == pt1)
			{
				pt3d1[j] = top_point2;
				j++;
			}
		}
		pt3d1[j] = pt3d1[0];
		pt3d1[j + 1] = endPt1;
		subObj1Id = CreateSolidByManner(pBlockTableRecord, pt3d1, j, 0, crSolid, pPlate->handle);
		MakeLsHole(pPlate, subObj1Id, ucs, scale, pBlockTableRecord, 1);
		//���ƹ�����
		f3dPoint pt3d2[6];
		pt3d2[0] = pt1;
		pt3d2[1] = pt2;
		pt3d2[2] = ptHuoqu2;
		pt3d2[3] = ptHuoqu1;
		pt3d2[4] = pt1;
		pt3d2[5] = pt2;
		subObj2Id = CreateSolidByManner(pBlockTableRecord, pt3d2, 4, 1, crSolid, pPlate->handle, angle);
		if (pPlate->face_N == 3)
		{
			if (pPlate->jdb_style != 3)//���������
			{
				if (pt3d3[k - 1] == pt3 || pt3d3[0] == pt3)
				{
					pt3d3[k] = top_point3;
				}
				k++;
			}
			pt3d3[k] = pt3d3[1];
			pt3d3[k + 1] = endPt2;
			subObj3Id = CreateSolidByManner(pBlockTableRecord, pt3d3, k, 0, crSolid, pPlate->handle);
			MakeLsHole(pPlate, subObj3Id, ucs, scale, pBlockTableRecord, 2);
			//
			f3dPoint pt3d4[6];
			pt3d4[0] = pt3;
			pt3d4[1] = pt4;
			pt3d4[2] = ptHuoqu4;
			pt3d4[3] = ptHuoqu3;
			pt3d4[4] = pt3;
			pt3d4[5] = pt4;
			subObj4Id = CreateSolidByManner(pBlockTableRecord, pt3d4, 4, 1, crSolid, pPlate->handle, angle1);
		}
		pBlockTableRecord->close();
		//����ʵ��ϲ�
		AcDb3dSolid *p3dObj = NULL, *p3dObj1 = NULL, *p3dObj2 = NULL, *p3dObj3 = NULL, *p3dObj4 = NULL;
		acdbOpenObject(p3dObj, mainObjId, AcDb::kForWrite);
		if (p3dObj == NULL)
			return NULL;
		acdbOpenObject(p3dObj1, subObj1Id, AcDb::kForWrite);
		acdbOpenObject(p3dObj2, subObj2Id, AcDb::kForWrite);
		acdbOpenObject(p3dObj3, subObj3Id, AcDb::kForWrite);
		acdbOpenObject(p3dObj4, subObj4Id, AcDb::kForWrite);
		if (p3dObj1)
			p3dObj->booleanOper(AcDb::kBoolUnite, p3dObj1);
		if (p3dObj2)
			p3dObj->booleanOper(AcDb::kBoolUnite, p3dObj2);
		if (p3dObj3)
			p3dObj->booleanOper(AcDb::kBoolUnite, p3dObj3);
		if (p3dObj4)
			p3dObj->booleanOper(AcDb::kBoolUnite, p3dObj4);
		if (p3dObj1)
			p3dObj1->close();
		if (p3dObj2)
			p3dObj2->close();
		if (p3dObj3)
			p3dObj3->close();
		if (p3dObj4)
			p3dObj4->close();
		p3dObj->close();
		//��ʵ��ͼԪλ�ý��е���
		if (pMatrix)
		{
			acdbOpenObject(p3dObj, mainObjId, AcDb::kForWrite);
			p3dObj->transformBy(*pMatrix);
			p3dObj->close();
		}
		return mainObjId;
	}
	return NULL;
}
//���Ʋ������հ�ʵ��
AcDbObjectId DrawSolidParamPlate(CLDSParamPlate *pPlate, UCS_STRU* pUcs /*=NULL*/, double scale /*=1*/,
							COLORREF* pCrSolid /*=NULL*/, AcGeMatrix3d *pMatrix/*=NULL*/)
{
	if (pPlate == NULL)
		return AcDbObjectId::kNull;
	int nNum = pPlate->vertex_list.GetNodeNum();
	if (nNum < 3)
		pPlate->DesignPlate();
	if (nNum < 3)
		return AcDbObjectId::kNull;
	f3dPoint vertex_arr[100];
	int i = 0;
	f3dLine path;
	fPtList vertex_list;
	COLORREF crSolid = RGB(0, 255, 128);
	if (pCrSolid)
		crSolid = *pCrSolid;
	if (pPlate->m_iParamType == TYPE_CIRRIBPLATE || pPlate->m_iParamType == TYPE_CIRCOVERPLATE || pPlate->m_iParamType == TYPE_TUBERIBPLATE)
	{	//��Ҫ����ʵ����ʾ wjh-2012.2.19
		PROFILE_VERTEX *pVertex = NULL;
		if (pPlate->IsZhiWan())	//��Ҫ���价���
			return Draw2FacePlate(pPlate, pUcs, scale, pCrSolid, pMatrix);
		else
			return Draw1FacePlate(pPlate, pUcs, scale, pCrSolid, pMatrix);
	}
	else if (pPlate->m_iParamType == TYPE_UEND || pPlate->m_iParamType == TYPE_ROLLEND ||
		pPlate->m_iParamType == TYPE_XEND || pPlate->m_iParamType == TYPE_ELBOWPLATE)
	{
		if (pPlate->m_iParamType == TYPE_UEND)
		{
			double H, L, T, W;
			pPlate->GetDesignItemValue('H', &H);
			pPlate->GetDesignItemValue('L', &L);
			pPlate->GetDesignItemValue('W', &W);
			T = pPlate->GetThick();
			vertex_arr[0] = pPlate->ucs.origin + pPlate->ucs.axis_x*L + pPlate->ucs.axis_y*H*0.5 + pPlate->ucs.axis_z*W*0.5;
			vertex_arr[1] = vertex_arr[0] - pPlate->ucs.axis_y*H;//pPlate->ucs.origin + pPlate->ucs.axis_x*L - pPlate->ucs.axis_y*H*0.5 + pPlate->ucs.axis_z*W*0.5;
			vertex_arr[2] = vertex_arr[1] + pPlate->ucs.axis_z*T;
			vertex_arr[3] = vertex_arr[0] + pPlate->ucs.axis_z*T;
			for (i = 0; i < 4; i++)
			{
				f3dPoint pt = vertex_arr[i];
				TransPtToCadUcs(pt, pUcs, scale);
				vertex_list.append(pt);
			}
			path.startPt = vertex_arr[0] - pPlate->ucs.axis_y*H*0.5;//pPlate->ucs.origin + pPlate->ucs.axis_x*L + pPlate->ucs.axis_z*W*0.5;
			path.endPt = pPlate->ucs.origin + pPlate->ucs.axis_z*W*0.5 + pPlate->ucs.axis_x*(W*0.5 + T);
			TransPtToCadUcs(path.startPt, pUcs, scale);
			TransPtToCadUcs(path.endPt, pUcs, scale);
			f3dPoint plane_norm = pPlate->ucs.axis_x;
			TransPtToCadUcs(plane_norm, pUcs, scale);
			AcDbObjectId solidId;
			AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
			solidId = CreateSolidByExtrude(pBlockTableRecord, vertex_list, plane_norm, path, crSolid);
			pBlockTableRecord->close();
			//
			vertex_list.Empty();
			vertex_arr[0] = pPlate->ucs.origin + pPlate->ucs.axis_x*L + pPlate->ucs.axis_y*H*0.5 - pPlate->ucs.axis_z*W*0.5;
			vertex_arr[1] = vertex_arr[0] - pPlate->ucs.axis_y*H;//pPlate->ucs.origin + pPlate->ucs.axis_x*L - pPlate->ucs.axis_y*H*0.5 - pPlate->ucs.axis_z*W*0.5;
			vertex_arr[2] = vertex_arr[1] - pPlate->ucs.axis_z*T;
			vertex_arr[3] = vertex_arr[0] - pPlate->ucs.axis_z*T;
			for (i = 0; i < 4; i++)
			{
				f3dPoint pt = vertex_arr[i];
				TransPtToCadUcs(pt, pUcs, scale);
				vertex_list.append(pt);
			}
			path.startPt = vertex_arr[0] - pPlate->ucs.axis_y*H*0.5;
			path.endPt = pPlate->ucs.origin - pPlate->ucs.axis_z*W*0.5 + pPlate->ucs.axis_x*(W*0.5 + T);
			TransPtToCadUcs(path.startPt, pUcs, scale);
			TransPtToCadUcs(path.endPt, pUcs, scale);
			AcDbObjectId solidId1;
			AcDbBlockTableRecord *pBlockTableRecord1 = GetBlockTableRecord();
			solidId1 = CreateSolidByExtrude(pBlockTableRecord1, vertex_list, path, crSolid);
			pBlockTableRecord1->close();
			//
			vertex_list.Empty();
			vertex_arr[0] = pPlate->ucs.origin + pPlate->ucs.axis_x*(W*0.5 + T) + pPlate->ucs.axis_y*H*0.5 - pPlate->ucs.axis_z*W*0.5;
			vertex_arr[1] = vertex_arr[0] - pPlate->ucs.axis_y*H;//pPlate->ucs.origin + pPlate->ucs.axis_x*(W*0.5+T) - pPlate->ucs.axis_y*H*0.5 - pPlate->ucs.axis_z*W*0.5;
			vertex_arr[2] = vertex_arr[1] - pPlate->ucs.axis_z*T;
			vertex_arr[3] = vertex_arr[0] - pPlate->ucs.axis_z*T;
			vertex_arr[4] = vertex_arr[0] + pPlate->ucs.axis_z*W*0.5;//pPlate->ucs.origin + pPlate->ucs.axis_x*(W*0.5+T) + pPlate->ucs.axis_y*H*0.5;
			vertex_arr[5] = vertex_arr[4] - pPlate->ucs.axis_y*H;//pPlate->ucs.origin + pPlate->ucs.axis_x*(W*0.5+T) - pPlate->ucs.axis_y*H*0.5;
			for (int i = 0; i < 6; i++)
			{
				f3dPoint pt = vertex_arr[i];
				TransPtToCadUcs(pt, pUcs, scale);
				if (i < 4)
					vertex_list.append(vertex_arr[i]);
			}
			f3dPoint axis_vec = (vertex_arr[4] - vertex_arr[5]).normalized();
			AcDbObjectId solidId2;
			AcDbBlockTableRecord *pBlockTableRecord2 = GetBlockTableRecord();
			solidId2 = CreateSolidByRevolve(pBlockTableRecord2, vertex_list, vertex_arr[5], axis_vec, Pi, crSolid, pPlate->handle);
			pBlockTableRecord2->close();
			//����ʵ��ϲ�
			AcDb3dSolid *pSubPlate1 = NULL, *pSubPlate2 = NULL, *pSubPlate3 = NULL;
			acdbOpenObject(pSubPlate1, solidId, AcDb::kForWrite);
			if (pSubPlate1 == NULL)
				return NULL;
			acdbOpenObject(pSubPlate2, solidId1, AcDb::kForWrite);
			acdbOpenObject(pSubPlate3, solidId2, AcDb::kForWrite);
			if (pSubPlate3)
				pSubPlate1->booleanOper(AcDb::kBoolUnite, pSubPlate3);
			if (pSubPlate2)
				pSubPlate1->booleanOper(AcDb::kBoolUnite, pSubPlate2);
			if (pSubPlate2)
				pSubPlate2->close();
			if (pSubPlate3)
				pSubPlate3->close();
			pSubPlate1->close();
			//��ʵ��ͼԪλ�ý��е���
			if (pMatrix)
			{
				acdbOpenObject(pSubPlate1, solidId, AcDb::kForWrite);
				pSubPlate1->transformBy(*pMatrix);
				pSubPlate1->close();
			}
			return solidId;
		}
		else if (pPlate->m_iParamType == TYPE_ROLLEND)
		{
			double H, L, W, T;
			pPlate->GetDesignItemValue('H', &H);
			pPlate->GetDesignItemValue('L', &L);
			pPlate->GetDesignItemValue('W', &W);
			T = pPlate->GetThick();
			int nVertex = 8;
			if (W > 0)
			{
				vertex_arr[0] = pPlate->ucs.origin + pPlate->ucs.axis_y*H*0.5 + pPlate->ucs.axis_z*pPlate->GetThick()*0.5;
				vertex_arr[1] = vertex_arr[0] - pPlate->ucs.axis_y*H;
				vertex_arr[2] = vertex_arr[1] - pPlate->ucs.axis_z*W;
				vertex_arr[3] = vertex_arr[2] + pPlate->ucs.axis_y*T;
				vertex_arr[4] = vertex_arr[3] + pPlate->ucs.axis_z*(W - T);
				vertex_arr[5] = vertex_arr[4] + pPlate->ucs.axis_y*(H - 2 * T);
				vertex_arr[6] = vertex_arr[5] - pPlate->ucs.axis_z*(W - T);
				vertex_arr[7] = vertex_arr[6] + pPlate->ucs.axis_y*T;
			}
			else
			{
				nVertex = 4;
				vertex_arr[0] = pPlate->ucs.origin + pPlate->ucs.axis_y*H*0.5 + pPlate->ucs.axis_z*pPlate->GetThick()*0.5;
				vertex_arr[1] = vertex_arr[0] - pPlate->ucs.axis_y*H;
				vertex_arr[2] = vertex_arr[1] - pPlate->ucs.axis_z*T;
				vertex_arr[3] = vertex_arr[2] + pPlate->ucs.axis_y*H;
			}
			path.startPt = pPlate->ucs.origin;
			path.endPt = pPlate->ucs.origin + pPlate->ucs.axis_x*L;
			for (i = 0; i < nVertex; i++)
			{
				TransPtToCadUcs(vertex_arr[i], pUcs, scale);
				vertex_list.append(vertex_arr[i]);
			}
		}
		else if (pPlate->m_iParamType == TYPE_XEND)
		{
			double H, L, T;
			pPlate->GetDesignItemValue('H', &H);
			pPlate->GetDesignItemValue('L', &L);
			T = pPlate->GetThick();
			vertex_arr[0] = pPlate->ucs.origin + pPlate->ucs.axis_y*H*0.5;
			vertex_arr[1] = vertex_arr[0] - pPlate->ucs.axis_y*H;//pPlate->ucs.origin - pPlate->ucs.axis_y*H/2;
			vertex_arr[2] = vertex_arr[1] + pPlate->ucs.axis_z*T;
			vertex_arr[3] = vertex_arr[0] + pPlate->ucs.axis_z*T;
			path.startPt = pPlate->ucs.origin;
			path.endPt = pPlate->ucs.origin + pPlate->ucs.axis_x*L;
			for (i = 0; i < 4; i++)
			{
				TransPtToCadUcs(vertex_arr[i], pUcs, scale);
				vertex_list.append(vertex_arr[i]);
			}
		}
		else if (pPlate->m_iParamType == TYPE_ELBOWPLATE)
		{
			for (PROFILE_VERTEX* pVertex = pPlate->vertex_list.GetFirst(); pVertex; pVertex = pPlate->vertex_list.GetNext())
			{
				f3dPoint vertex = pVertex->vertex;
				coord_trans(vertex, pPlate->ucs, TRUE);
				vertex_arr[i] = vertex;
				TransPtToCadUcs(vertex_arr[i], pUcs, scale);
				vertex_list.append(vertex_arr[i]);
				if (i == 0)
				{
					path.startPt = vertex;
					path.endPt = vertex + pPlate->ucs.axis_z*pPlate->GetThick();
				}
				i++;
			}
		}
		TransPtToCadUcs(path.startPt, pUcs, scale);
		TransPtToCadUcs(path.endPt, pUcs, scale);
		AcDbObjectId solidId;
		AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
		solidId = CreateSolidByExtrude(pBlockTableRecord, vertex_list, path, crSolid, pPlate->handle);
		pBlockTableRecord->close();
		//��ʵ��ͼԪλ�ý��е���
		if (pMatrix)
		{
			AcDb3dSolid *pSolid = NULL;
			acdbOpenObject(pSolid, solidId, AcDb::kForWrite);
			pSolid->transformBy(*pMatrix);
			pSolid->close();
		}
		return solidId;
	}
	else if (pPlate->m_iParamType == TYPE_FL ||
		pPlate->m_iParamType == TYPE_FLG ||
		pPlate->m_iParamType == TYPE_FLR ||
		pPlate->m_iParamType == TYPE_WATERSHIELDEDPLATE)
	{
		f3dPoint axis_vec;
		if (pPlate->m_iParamType == TYPE_FL || pPlate->m_iParamType == TYPE_FLG ||
			pPlate->m_iParamType == TYPE_FLR)
		{
			double R, W, T;
			pPlate->GetDesignItemValue('R', &R);
			pPlate->GetDesignItemValue('W', &W);
			T = pPlate->GetThick();
			vertex_arr[0] = pPlate->ucs.origin + pPlate->ucs.axis_y*R;
			vertex_arr[1] = pPlate->ucs.origin + pPlate->ucs.axis_y*W*0.5;
			vertex_arr[2] = vertex_arr[1] + pPlate->ucs.axis_z*T;
			vertex_arr[3] = vertex_arr[0] + pPlate->ucs.axis_z*T;
			vertex_arr[4] = pPlate->ucs.origin;
			vertex_arr[5] = pPlate->ucs.origin + pPlate->ucs.axis_z*T;
			for (int i = 0; i < 6; i++)
			{
				TransPtToCadUcs(vertex_arr[i], pUcs, scale);
				if (i < 4)
					vertex_list.append(vertex_arr[i]);
			}
			axis_vec = vertex_arr[5] - vertex_arr[4];
		}
		else if (pPlate->m_iParamType == TYPE_WATERSHIELDEDPLATE)
		{	//��Ҫ����ʵ����ʾ wjh-2012.2.19
			double W, T;
			pPlate->GetDesignItemValue('W', &W);
			T = pPlate->GetThick();
			vertex_arr[0] = pPlate->ucs.origin;
			vertex_arr[1] = pPlate->ucs.origin + pPlate->ucs.axis_y*W*0.5;
			vertex_arr[2] = vertex_arr[1] + pPlate->ucs.axis_z*T;
			vertex_arr[3] = vertex_arr[0] + pPlate->ucs.axis_z*T;
			vertex_arr[4] = pPlate->ucs.origin;
			vertex_arr[5] = pPlate->ucs.origin + pPlate->ucs.axis_z*T;
			for (int i = 0; i < 6; i++)
			{
				TransPtToCadUcs(vertex_arr[i], pUcs, scale);
				if (i < 4)
					vertex_list.append(vertex_arr[i]);
			}
			axis_vec = vertex_arr[5] - vertex_arr[4];
		}
		normalize(axis_vec);
		AcDbObjectId solidId;
		AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
		solidId = CreateSolidByRevolve(pBlockTableRecord, vertex_list, vertex_arr[5], axis_vec, Pi * 2, crSolid, pPlate->handle);
		pBlockTableRecord->close();
		//��ʵ��ͼԪλ�ý��е���
		if (pMatrix)
		{
			AcDb3dSolid *pSolid = NULL;
			acdbOpenObject(pSolid, solidId, AcDb::kForWrite);
			pSolid->transformBy(*pMatrix);
			pSolid->close();
		}
		return solidId;
	}
	else if (pPlate->m_iParamType == TYPE_FLP || pPlate->m_iParamType == TYPE_FLD)
	{
		f3dPoint axis_vec = pPlate->ucs.axis_z;
		double B, S, N, D, C, H1, H;
		pPlate->GetDesignItemValue('B', &B);
		pPlate->GetDesignItemValue('S', &S);
		pPlate->GetDesignItemValue('N', &N);
		pPlate->GetDesignItemValue('D', &D);
		pPlate->GetDesignItemValue('C', &C);
		pPlate->GetDesignItemValue('O', &H1);
		pPlate->GetDesignItemValue('H', &H);
		vertex_arr[0] = pPlate->ucs.origin + pPlate->ucs.axis_x*B*0.5;
		vertex_arr[1] = pPlate->ucs.origin + pPlate->ucs.axis_x*D*0.5;
		vertex_arr[2] = vertex_arr[1] + pPlate->ucs.axis_z*C;
		vertex_arr[3] = vertex_arr[2] - pPlate->ucs.axis_x*(D - N)*0.5;
		if (pPlate->m_iParamType == TYPE_FLD)
		{
			vertex_arr[6] = vertex_arr[0] + pPlate->ucs.axis_z*H;
			vertex_arr[5] = vertex_arr[6] + pPlate->ucs.axis_x*S;
			vertex_arr[4] = vertex_arr[5] - pPlate->ucs.axis_z*H1;
		}
		else
		{
			vertex_arr[5] = vertex_arr[0] + pPlate->ucs.axis_z*H;
			vertex_arr[4] = vertex_arr[5] + pPlate->ucs.axis_x*S;
		}
		for (int i = 0; i < 7; i++)
		{
			if (i == 6 && pPlate->m_iParamType == TYPE_FLD)
			{
				TransPtToCadUcs(vertex_arr[i], pUcs, scale);
				vertex_list.append(vertex_arr[i]);
			}
			else if (i < 6)
			{
				TransPtToCadUcs(vertex_arr[i], pUcs, scale);
				vertex_list.append(vertex_arr[i]);
			}
		}
		f3dPoint basePt = pPlate->ucs.origin;
		TransPtToCadUcs(basePt, pUcs, scale);
		TransPtToCadUcs(axis_vec, pUcs, 1);
		AcDbObjectId solidId;
		AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
		solidId = CreateSolidByRevolve(pBlockTableRecord, vertex_list, basePt, axis_vec, Pi * 2, crSolid, pPlate->handle);
		pBlockTableRecord->close();
		//��ʵ��ͼԪλ�ý��е���
		if (pMatrix)
		{
			AcDb3dSolid *pSolid = NULL;
			acdbOpenObject(pSolid, solidId, AcDb::kForWrite);
			pSolid->transformBy(*pMatrix);
			pSolid->close();
		}
		return solidId;
	}
	else //if(pPlate->m_iParamType==TYPE_RIBPLATE||pPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
	{	//�߰�(���߰壬���߰壬�Ǹ��߰壬�ְ��߰壬�ֹ��߰壬�ֹܼа�) ʵ����ʾ wht 12-02-20
		return Draw1FacePlate(pPlate, pUcs, scale, pCrSolid, pMatrix);
	}
}
//���Ʋ۸�ʵ��
AcDbObjectId DrawSolidSlot(CLDSLineSlot *pLineSlot, UCS_STRU* pUcs /*=NULL*/, double scale/* =1*/,
					COLORREF* pColor/*=NULL*/, AcGeMatrix3d *pMatrix /*=NULL*/)
{
	if (pLineSlot == NULL)
		return AcDbObjectId::kNull;
	f3dPoint vertex_arr[8];	//��ʼģ�����ζ���
	vertex_arr[0].x = -pLineSlot->size_height / 2;
	vertex_arr[0].y = pLineSlot->size_wide;
	vertex_arr[0].z = -pLineSlot->startOdd();

	vertex_arr[1].x = -pLineSlot->size_height / 2;
	vertex_arr[1].y = 0;
	vertex_arr[1].z = -pLineSlot->startOdd();

	vertex_arr[2].x = pLineSlot->size_height / 2;
	vertex_arr[2].y = 0;
	vertex_arr[2].z = -pLineSlot->startOdd();

	vertex_arr[3].x = pLineSlot->size_height / 2;
	vertex_arr[3].y = pLineSlot->size_wide;
	vertex_arr[3].z = -pLineSlot->startOdd();

	vertex_arr[4].x = pLineSlot->size_height / 2 - pLineSlot->size_thick;
	vertex_arr[4].y = pLineSlot->size_wide;
	vertex_arr[4].z = -pLineSlot->startOdd();

	vertex_arr[5].x = pLineSlot->size_height / 2 - pLineSlot->size_thick;
	vertex_arr[5].y = pLineSlot->size_thick;
	vertex_arr[5].z = -pLineSlot->startOdd();

	vertex_arr[6].x = -pLineSlot->size_height / 2 + pLineSlot->size_thick;
	vertex_arr[6].y = pLineSlot->size_thick;
	vertex_arr[6].z = -pLineSlot->startOdd();

	vertex_arr[7].x = -pLineSlot->size_height / 2 + pLineSlot->size_thick;
	vertex_arr[7].y = pLineSlot->size_wide;
	vertex_arr[7].z = -pLineSlot->startOdd();
	fPtList vertex_list;
	UCS_STRU cs = pLineSlot->BuildUCS();
	for (int i = 0; i < 8; i++)
	{
		coord_trans(vertex_arr[i], cs, TRUE, TRUE);
		TransPtToCadUcs(vertex_arr[i], pUcs, scale);
		vertex_list.append(vertex_arr[i]);
	}
	f3dLine path;
	path.startPt = pLineSlot->Start() - pLineSlot->startOdd()*cs.axis_z;
	path.endPt = path.startPt + pLineSlot->GetLength()*cs.axis_z;
	TransPtToCadUcs(path.startPt, pUcs, scale);
	TransPtToCadUcs(path.endPt, pUcs, scale);
	//ͨ���������ɲ۸�
	COLORREF crSolid;
	if (pLineSlot->pStart == NULL || pLineSlot->pEnd == NULL)
		crSolid = RGB(90, 90, 90);
	else
		crSolid = RGB(150, 150, 255);
	if (pColor)
		crSolid = *pColor;
	AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
	if (pBlockTableRecord == NULL)
		return NULL;
	AcDbObjectId solidId;
	solidId=CreateSolidByExtrude(pBlockTableRecord, vertex_list, cs.axis_z, path, crSolid, pLineSlot->handle);
	pBlockTableRecord->close();
	//��ʵ��ͼԪλ�ý��е���
	if (pMatrix)
	{
		AcDb3dSolid *p3dObj = NULL;
		acdbOpenObject(p3dObj, solidId, AcDb::kForWrite);
		p3dObj->transformBy(*pMatrix);
		p3dObj->close();
	}
	return solidId;
}
//���Ʊ���ʵ��
AcDbObjectId DrawSolidFlat(CLDSLineFlat *pLineFlat, UCS_STRU* pUcs /*=NULL*/, double scale /*=1*/,
					COLORREF* pColor /*=NULL*/, AcGeMatrix3d *pMatrix /*=NULL*/)
{
	if (pLineFlat == NULL)
		return AcDbObjectId::kNull;
	f3dPoint vertex_arr[4];	//��ʼģ�����ζ���
	vertex_arr[0].x = -pLineFlat->size_wide / 2;
	vertex_arr[0].y = pLineFlat->size_thick / 2;
	vertex_arr[0].z = -pLineFlat->startOdd();

	vertex_arr[1].x = pLineFlat->size_wide / 2;
	vertex_arr[1].y = pLineFlat->size_thick / 2;
	vertex_arr[1].z = -pLineFlat->startOdd();

	vertex_arr[2].x = pLineFlat->size_wide / 2;
	vertex_arr[2].y = -pLineFlat->size_thick / 2;
	vertex_arr[2].z = -pLineFlat->startOdd();

	vertex_arr[3].x = -pLineFlat->size_wide / 2;
	vertex_arr[3].y = -pLineFlat->size_thick / 2;
	vertex_arr[3].z = -pLineFlat->startOdd();
	UCS_STRU cs = pLineFlat->BuildUCS();
	fPtList vertex_list;
	for (int i = 0; i < 4; i++)
	{
		coord_trans(vertex_arr[i], cs, TRUE, TRUE);
		TransPtToCadUcs(vertex_arr[i], pUcs, scale);
		vertex_list.append(vertex_arr[i]);
	}
	f3dLine path;
	path.startPt = pLineFlat->Start() - pLineFlat->startOdd()*pLineFlat->ucs.axis_z;
	path.endPt = path.startPt + pLineFlat->GetLength()*cs.axis_z;
	TransPtToCadUcs(path.startPt, pUcs, FALSE);
	TransPtToCadUcs(path.endPt, pUcs, FALSE);
	//ͨ���������ɲ۸�
	COLORREF crSolid;
	if (pLineFlat->pStart == NULL || pLineFlat->pEnd == NULL)
		crSolid = RGB(90, 90, 90);
	else
		crSolid = RGB(150, 150, 255);
	if (pColor)
		crSolid = *pColor;
	AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
	if (pBlockTableRecord == NULL)
		return NULL;
	AcDbObjectId solidId;
	solidId = CreateSolidByExtrude(pBlockTableRecord, vertex_list, cs.axis_z, path, crSolid, pLineFlat->handle);
	pBlockTableRecord->close();
	//��ʵ��ͼԪλ�ý��е���
	if (pMatrix)
	{
		AcDb3dSolid *p3dObj = NULL;
		acdbOpenObject(p3dObj, solidId, AcDb::kForWrite);
		p3dObj->transformBy(*pMatrix);
		p3dObj->close();
	}
	return solidId;
}
//���Ƹֹ�ʵ��
AcDbObjectId DrawSolidTube(CLDSLineTube *pLineTube, UCS_STRU* pUcs /*=NULL*/, double scale /*=1*/,
					COLORREF* pColor /*=NULL*/, AcGeMatrix3d *pMatrix /*=NULL*/)
{
	if (pLineTube == NULL)
		return AcDbObjectId::kNull;
	UCS_STRU cs = pLineTube->BuildUCS();
	f3dLine path;
	path.startPt = pLineTube->Start() - cs.axis_z*pLineTube->startOdd();
	path.endPt = pLineTube->End() + cs.axis_z*pLineTube->endOdd();
	TransPtToCadUcs(path.startPt, pUcs, scale);
	TransPtToCadUcs(path.endPt, pUcs, scale);
	f3dPoint org = path.startPt;
	f3dPoint norm = (path.endPt - path.startPt).normalized();
	COLORREF plank_color(RGB(150, 150, 255));
	if (pColor)
		plank_color = *pColor;
	//��Բ��ʵ��
	AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
	if (pBlockTableRecord == NULL)
		return NULL;
	double fOutR = pLineTube->GetDiameter()*0.5 / scale;
	AcDbObjectId solidId = CreateSolidByExtrude(pBlockTableRecord, fOutR, org, norm, path, plank_color, pLineTube->handle);
	//��Բ��ʵ��
	double fInnR = (pLineTube->GetDiameter()*0.5 - pLineTube->size_thick) / scale;
	AcDbObjectId subSolidId = CreateSolidByExtrude(pBlockTableRecord, fInnR, org, norm, path, plank_color, pLineTube->handle);
	pBlockTableRecord->close();
	//ͨ�״���(��Բ��ʵ���м�ȥ��Բ��ʵ��)
	AcDb3dSolid *p3dMainSolid = NULL, *pSubSolid = NULL;
	acdbOpenObject(p3dMainSolid, solidId, AcDb::kForWrite);
	if (p3dMainSolid == NULL)
		return NULL;
	acdbOpenObject(pSubSolid, subSolidId, AcDb::kForWrite);
	if (pSubSolid)
		p3dMainSolid->booleanOper(AcDb::kBoolSubtract, pSubSolid);
	if (pSubSolid)
		pSubSolid->close();
	p3dMainSolid->close();
	//��ʵ��ͼԪλ�ý��е���
	if (pMatrix)
	{
		acdbOpenObject(p3dMainSolid, solidId, AcDb::kForWrite);
		p3dMainSolid->transformBy(*pMatrix);
		p3dMainSolid->close();
	}
	return solidId;
}
AcDbObjectId DrawSolidFitting(CLDSFitting *pFitting, UCS_STRU* pUcs /*=NULL*/, double scale /*=1*/,
						COLORREF* pCrSolid /*=NULL*/, AcGeMatrix3d *pMatrix /*=NULL*/)
{
	AcDbObjectId solidId[2], newEntId;
	if (pFitting->m_ciFittingType == CLDSFitting::TYPE_EB)
	{
		FITTING_EB *pEBInfo = QueryFitting<FITTING_EB>(pFitting->sizespec);
		if (pEBInfo == NULL)
			return NULL;
		solidId[0] = DrawEBSolid(pEBInfo, pCrSolid);
		FITTING_UR* pURInfo = QueryFitting<FITTING_UR>(pFitting->sizespecUR);
		if (pURInfo)
			solidId[1] = DrawURSolid(pURInfo, pCrSolid);
	}
	else if (pFitting->m_ciFittingType == CLDSFitting::TYPE_GD)
	{
		FITTING_GD *pGDInfo = QueryFitting<FITTING_GD>(pFitting->sizespec);
		if (pGDInfo == NULL)
			return NULL;
		solidId[0] = DrawGDSolid(pGDInfo, pCrSolid);
		FITTING_UR* pURInfo = QueryFitting<FITTING_UR>(pFitting->sizespecUR);
		if (pURInfo)
			solidId[1] = DrawURSolid(pURInfo, pCrSolid);
	}
	else
		return NULL;
	AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
	if (pBlockTableRecord == NULL)
		return NULL;
	for (int i = 0; i < 2; i++)
	{
		if(solidId[i].isNull())
			continue;
		AcDbBlockReference *pBlkRef = new AcDbBlockReference;
		pBlkRef->setBlockTableRecord(solidId[i]);
		//��������ϵ
		f3dPoint pos_orig = pFitting->ucs.origin;
		f3dPoint pos_x = pFitting->ucs.axis_x;
		f3dPoint pos_y = pFitting->ucs.axis_y;
		f3dPoint pos_z = pFitting->ucs.axis_z;
		TransPtToCadUcs(pos_orig, pUcs, scale);
		TransVtToCadUcs(pos_x, pUcs);
		TransVtToCadUcs(pos_y, pUcs);
		TransVtToCadUcs(pos_z, pUcs);
		AcGePoint3d origin(pos_orig.x, pos_orig.y, pos_orig.z);
		if (i == 1)
		{	//����U�ͻ���λ��
			/*origin.x += pos_z.x*L0*(1.0 / scale);
			origin.y += pos_z.y*L0*(1.0 / scale);
			origin.z += pos_z.z*L0*(1.0 / scale);*/
		}
		AcGeVector3d xAxis(pos_x.x, pos_x.y, pos_x.z);
		AcGeVector3d yAxis(pos_y.x, pos_y.y, pos_y.z);
		AcGeVector3d zAxis(pos_z.x, pos_z.y, pos_z.z);
		double fScale = 1.0 / scale;
		AcGeMatrix3d geMat;
		geMat.setCoordSystem(origin, xAxis, yAxis, zAxis);
		pBlkRef->setBlockTransform(geMat);
		AcGeScale3d scaleXYZ(fScale, fScale, fScale);
		pBlkRef->setScaleFactors(fScale);
		if (pMatrix)
			pBlkRef->setBlockTransform(*pMatrix);
		//
		pBlockTableRecord->appendAcDbEntity(newEntId, pBlkRef);
		pBlkRef->close();
	}
	pBlockTableRecord->close();
	return solidId[0];
}
double GetArcLength(double L, double H, double r)
{
	double length;
	length = sqrt((L / 2)*(L / 2) + H * H);
	double angle;
	angle = Pi / 2 - asin(H / length) - asin(r / length);
	return 2 * r*cos(angle);
}

void GetAngleArcVertex(double r, BOOL isLeft, f3dPoint vertex_arr[], int slices)
{
	double startradius;
	double slice_angle = Pi / (3 * (slices - 1));
	if (!isLeft)
		startradius = 0;
	else
		startradius = Pi / 2 + Pi / 6;
	double sina = sin(slice_angle);	//��Ƭ������
	double cosa = cos(slice_angle);	//��Ƭ������
	vertex_arr[0].Set(r*cos(startradius), r*sin(startradius));
	for (int i = 1; i < slices; i++)
	{
		vertex_arr[i].x = vertex_arr[i - 1].x*cosa - vertex_arr[i - 1].y*sina;
		vertex_arr[i].y = vertex_arr[i - 1].y*cosa + vertex_arr[i - 1].x*sina;
		vertex_arr[i].z = 0;
	}
}
AcDbObjectId DrawEBSolid(FITTING_EB* pInfo,COLORREF* pCrSolid /*= NULL*/)
{
	if (pInfo == NULL)
		return NULL;
	short siThroughThick = pInfo->siThroughThick;	//�Ұ���
	double dfBoardL = pInfo->dfBoardL;				//�Ұ�������
	double dfTrunnionD = pInfo->dfTrunnionD;		//����ֱ��
	double dfPointH = pInfo->dfBoardH;
	double dfHoleD = pInfo->dfHoleD;
	double dfPointh = 0.6*pInfo->dfTrunnionD;
	double dfPointhl = pInfo->dfBoardH / 3;
	double dfPointl = GetArcLength(dfBoardL, dfPointH, dfPointhl);
	double dfInnerR = pInfo->dfTrunnionD + 6;
	double dfOuterR = dfInnerR + 2 * dfPointh;
	double dfNutDs = dfTrunnionD / 3 * 5;
	double dfNutDe = dfNutDs / cos(Pi / 6);
	double dfNutH = 0.5*dfNutDs;
	double dfNutTaper = 0.35;
	double dfWasherD = dfNutDe;
	double dfWasherH = 0.1*dfWasherD;
	GEPOINT vertex_arr[32], ver_arr[6];	//��ʼģ�����ζ���
	GetCircleSimuPolyVertex(1.0, vertex_arr, 32);
	GetCircleSimuPolyVertex(1.0, ver_arr, 6);
	AcDbObjectId solidId=SearchBlock(pInfo->szSizeCode);
	if (solidId.isNull())
	{
		//�����¿��¼ָ��
		AcDbBlockTable *pTempBlockTable=NULL;
		GetCurDwg()->getBlockTable(pTempBlockTable, AcDb::kForWrite);
		AcDbBlockTableRecord *pTempBlockTableRecord = new AcDbBlockTableRecord();//�������¼ָ��
#ifdef _ARX_2007
		pTempBlockTableRecord->setName((ACHAR*)_bstr_t(pInfo->szSizeCode));
#else
		pTempBlockTableRecord->setName(pInfo->szSizeCode);
#endif
		pTempBlockTable->add(solidId, pTempBlockTableRecord);
		pTempBlockTable->close();
		//TODO:����EBʵ��
		double fThroughLen = dfBoardL + siThroughThick*3 + dfTrunnionD/3*8;//192
		COLORREF crLs = RGB(91, 91, 91), crBoard = RGB(145, 149, 145);
		if (pCrSolid)
			crBoard = *pCrSolid;
		f3dLine path;
		fPtList vertex_list, vertex_list1;
		//�Ұ�����
		GECS ztcs(GEPOINT(0, 0, -dfPointh / 2), GEPOINT(1, 0, 0), GEPOINT(0, 1, 0), GEPOINT(0, 0, 1));
		//���
		ztcs.origin.y = -dfPointl;
		path.startPt = ztcs.origin;
		path.endPt = ztcs.origin + ztcs.axis_z*dfPointh;

		double section_h = sqrt(pow(dfOuterR / 2, 2) - pow(0.5*dfTrunnionD, 2));//����������ԲͲ��������������
		double dfFilletR = 0.4*dfTrunnionD;	//���ɻ��뾶�Ǹ�����ʾЧ��ȡ�ľ���ֵ
		double fdistance = section_h + dfFilletR * cos(Pi / 6);//��������������ľ���
		f2dPoint xTopLeftVertex(-dfBoardL / 2, fdistance), xTopRightVertex(dfBoardL / 2, fdistance);
		f2dPoint xBoltCenter(0, -dfPointH), pickpoint(0, -dfPointH - 10);
		f2dPoint xLeftTangent, xRightTangent;
		Tang2dpc(xTopLeftVertex, f2dCircle(dfPointhl, xBoltCenter), pickpoint, xLeftTangent);
		Tang2dpc(xTopRightVertex, f2dCircle(dfPointhl, xBoltCenter), pickpoint, xRightTangent);
		vertex_list.Empty();
		vertex_list.append(ztcs.TransPFromCS(GEPOINT(xTopLeftVertex.x, xTopLeftVertex.y)));
		vertex_list.append(ztcs.TransPFromCS(GEPOINT(xLeftTangent.x, xLeftTangent.y)));
		for (int i = 1; i < 32; i++)
		{
			if (i>=16)
			{
				GEPOINT pt = vertex_arr[i] * dfPointl * 0.5;
				pt.y = pt.y + xLeftTangent.y;
				vertex_list.append(ztcs.TransPFromCS(pt));
			}
		}
		vertex_list.append(ztcs.TransPFromCS(GEPOINT(xRightTangent.x, xRightTangent.y)));
		vertex_list.append(ztcs.TransPFromCS(GEPOINT(xTopRightVertex.x, xTopRightVertex.y)));
		AcDbObjectId motherzt = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);
		//���Ȼ�
		ztcs.origin.Set(0, 0, 0);
		ztcs.origin.y = dfPointl- dfOuterR - dfFilletR/2;
		path.startPt = ztcs.origin;
		path.endPt = ztcs.origin + ztcs.axis_x*dfBoardL;
		vertex_list.Empty();
		f3dPoint left_vertext_arr[20], right_vertext_arr[20], vertex_arr1[200];
		int joint_lines = 15;
		GetAngleArcVertex(dfFilletR, TRUE, left_vertext_arr, joint_lines);
		GetAngleArcVertex(dfFilletR, FALSE, right_vertext_arr, joint_lines);
		for (int i = 0; i < joint_lines; i++)
		{
			vertex_arr1[i].Set(0, left_vertext_arr[i].y - fdistance, left_vertext_arr[i].x + dfFilletR + dfPointh*0.5);
			vertex_arr1[i + joint_lines].Set(0, right_vertext_arr[i].y - fdistance, right_vertext_arr[i].x - dfFilletR - dfPointh*0.5);
		}
		for (int i = 0; i < joint_lines*2 ; i++)
			vertex_list.append(ztcs.TransPFromCS(f3dPoint(vertex_arr1[i].x - dfBoardL / 2, vertex_arr1[i].y, vertex_arr1[i].z)));
		AcDbObjectId motherGD = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);
		//���
		ztcs.origin.y = -dfPointl;
		ztcs.origin.z = -dfPointh;

		path.startPt = ztcs.origin;
		path.endPt = ztcs.origin + ztcs.axis_z*dfPointh*3;
		vertex_list.Empty();
		for (int i = 1; i < 32; i++)
		{
			GEPOINT pt = vertex_arr[i] * dfInnerR * 0.5;
			pt.y = pt.y + xLeftTangent.y;
			vertex_list.append(ztcs.TransPFromCS(pt));
		}
		AcDbObjectId motherSolidKong = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);
		AcDbBlockTableRecord *pBlockTableRecord1 = GetBlockTableRecord();
		if (pBlockTableRecord1 == NULL)
			return NULL;
		AcDb3dSolid *p3dMainSolid1 = NULL, *pSubSolid1 = NULL;
		acdbOpenObject(p3dMainSolid1, motherzt, AcDb::kForWrite);
		if (p3dMainSolid1 == NULL)
			return NULL;
		acdbOpenObject(pSubSolid1, motherSolidKong, AcDb::kForWrite);
		if (pSubSolid1)
			p3dMainSolid1->booleanOper(AcDb::kBoolSubtract, pSubSolid1);
		if (pSubSolid1)
			pSubSolid1->close();
		p3dMainSolid1->close();

		//ԲͰ
		GECS pcs(GEPOINT(-dfBoardL / 2, 0, 0), GEPOINT(0, 0, -1), GEPOINT(0, 1, 0), GEPOINT(1, 0, 0));
		//��Բ
		path.startPt = pcs.origin;
		path.endPt = pcs.origin + pcs.axis_z*dfBoardL;
		vertex_list.Empty();
		for (int i = 0; i < 32; i++)
		{
			GEPOINT pt = vertex_arr[i] * dfOuterR * 0.5;
			vertex_list.append(pcs.TransPFromCS(pt));
		}
		AcDbObjectId motherSolid1 = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);
		//��Բ
		path.startPt = pcs.origin;
		path.endPt = pcs.origin + pcs.axis_z*dfBoardL;
		vertex_list.Empty();
		for (int i = 0; i < 32; i++)
		{
			GEPOINT pt = vertex_arr[i] * dfInnerR * 0.5;
			vertex_list.append(pcs.TransPFromCS(pt));
		}
		AcDbObjectId subSolidId = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);

		AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
		if (pBlockTableRecord == NULL)
			return NULL;
		AcDb3dSolid *p3dMainSolid = NULL, *pSubSolid = NULL;
		acdbOpenObject(p3dMainSolid, motherSolid1, AcDb::kForWrite);
		if (p3dMainSolid == NULL)
			return NULL;
		acdbOpenObject(pSubSolid, subSolidId, AcDb::kForWrite);
		if (pSubSolid)
			p3dMainSolid->booleanOper(AcDb::kBoolSubtract, pSubSolid);
		if (pSubSolid)
			pSubSolid->close();
		p3dMainSolid->close();

		GECS cs(GEPOINT(-fThroughLen / 2, 0, 0), GEPOINT(0, 0, -1), GEPOINT(0, 1, 0), GEPOINT(1, 0, 0));

		//�����ݸ�
		path.startPt = cs.origin;
		path.endPt = cs.origin + cs.axis_z*fThroughLen;
		vertex_list.Empty();
		for (int i = 0; i < 32; i++)
		{
			GEPOINT pt = vertex_arr[i] * dfTrunnionD * 0.5;
			vertex_list.append(cs.TransPFromCS(pt));
		}
		
		AcDbObjectId motherSolidId = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);
		//��Ƭ
		cs.origin.x = dfBoardL / 2 + siThroughThick;				//����ĸ��Ƭλ��
		path.startPt = cs.origin;
		path.endPt = cs.origin + cs.axis_z*dfWasherH;
		vertex_list.Empty();
		for (int i = 0; i < 32; i++)
		{
			GEPOINT pt = vertex_arr[i] * dfWasherD * 0.5;
			vertex_list.append(cs.TransPFromCS(pt));
		}
		AcDbObjectId motherSolidup = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);

		cs.origin.x = -dfBoardL / 2 - siThroughThick - dfWasherH;	//����ĸ��Ƭλ��
		path.startPt = cs.origin;
		path.endPt = cs.origin + cs.axis_z*dfWasherH;
		vertex_list.Empty();
		for (int i = 0; i < 32; i++)
		{
			GEPOINT pt = vertex_arr[i] * dfWasherD * 0.5;
			vertex_list.append(cs.TransPFromCS(pt));
		}
		AcDbObjectId motherSoliddown = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);

		//��ĸ
		cs.origin.x = dfBoardL / 2 + siThroughThick + dfWasherH;			//����ĸλ��
		path.startPt = cs.origin;
		path.endPt = cs.origin + cs.axis_z*dfNutH;
		vertex_list.Empty();
		for (int i = 0; i < 6; i++)
		{
			GEPOINT pt = ver_arr[i] * dfNutDs * 0.5;
			vertex_list.append(cs.TransPFromCS(pt));
		}
		AcDbObjectId motherLMup = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);

		cs.origin.x = -dfBoardL / 2 - siThroughThick - dfWasherH - dfNutH;	//����ĸλ��
		path.startPt = cs.origin;
		path.endPt = cs.origin + cs.axis_z*dfNutH;
		vertex_list.Empty();
		for (int i = 0; i < 6; i++)
		{
			GEPOINT pt = ver_arr[i] * dfNutDs * 0.5;
			vertex_list.append(cs.TransPFromCS(pt));
		}
		AcDbObjectId motherLMdown = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);

		//�رտ��¼
		pTempBlockTableRecord->close();
	}
	return solidId;
}

AcDbObjectId DrawGDSolid(FITTING_GD* pInfo, COLORREF* pCrSolid /*= NULL*/)
{
	if (pInfo == NULL)
		return NULL; 
	short siThroughThick = pInfo->siThroughThick;	//Ԥ�����߽Ǹ�֫(ͨ)��
	double dfBoardL = pInfo->dfBoardL;				//���(�Ұ岿��)��ȣ���Բ����
	double dfTrunnionD = pInfo->dfTrunnionD;		//����ֱ��
	double dfPointH = pInfo->dfBoardH;				//��˨�����ĵ���Բ���ĳ�
	double dfHoleD = pInfo->dfHoleD;				//�ҿ�ֱ��(diameter)��
	double dfTrunnionL = pInfo->dfTrunnionL;		//�����ܳ�230
	double dfBoardH2 = pInfo->dfBoardH2;			//�ҿ׵����������߾���H2  
	double dfBoardH1 = pInfo->dfBoardH1;			//���ᵽ��Բ���ĳ�H1
	double siThick = pInfo->siThick;				//�Ұ���b
	double m_fChinChordL = ftoi(pInfo->dfHoleD*1.2);
	double dfNutDs = pInfo->dfTrunnionD / 3 * 4;
	double dfNutDe = dfNutDs / cos(Pi / 6);
	double dfNutH = 0.5*dfNutDs;
	double dfNutTaper = 0.35;
	double dfWasherD = dfNutDe;//��ĸ
	double dfWasherH = 0.1*dfWasherD;
	double dfBaffleD = dfNutDs;
	double dfBaffleH = 0.1*dfBaffleD;

	GEPOINT vertex_arr[32], ver_arr[6];	//��ʼģ�����ζ���
	GetCircleSimuPolyVertex(1.0, vertex_arr, 32);
	GetCircleSimuPolyVertex(1.0, ver_arr, 6);
	AcDbObjectId solidId = SearchBlock(pInfo->szSizeCode);
	if (solidId.isNull())
	{
		//�����¿��¼ָ��
		AcDbBlockTable *pTempBlockTable = NULL;
		GetCurDwg()->getBlockTable(pTempBlockTable, AcDb::kForWrite);
		AcDbBlockTableRecord *pTempBlockTableRecord = new AcDbBlockTableRecord();//�������¼ָ��
#ifdef _ARX_2007
		pTempBlockTableRecord->setName((ACHAR*)_bstr_t(pInfo->szSizeCode));
#else
		pTempBlockTableRecord->setName(pInfo->szSizeCode);
#endif
		pTempBlockTable->add(solidId, pTempBlockTableRecord);
		pTempBlockTable->close();
		//TODO:����GDʵ��
		double fThroughLen = dfBoardL + siThroughThick * 3 + dfTrunnionD / 3 * 8;//192
		COLORREF crLs = RGB(91, 91, 91), crBoard = RGB(145, 149, 145);
		if (pCrSolid)
			crBoard = *pCrSolid;
		f3dLine path;
		fPtList vertex_list;

		GECS ztcs(GEPOINT(dfBoardH2/2, 0, 0), GEPOINT(1, 0, 0), GEPOINT(0, 1, 0), GEPOINT(0, 0, 1));

		ztcs.origin.x = -dfBaffleH/2;
		ztcs.origin.z = -dfHoleD * 0.5;
		path.startPt = ztcs.origin;
		path.endPt = ztcs.origin + ztcs.axis_z*dfHoleD;
		GEPOINT parrVertexes[50], xarrChinArcVertexes[50];
		int slices = 16;
		//���㶥��Բ���߶����
		GEPOINT xUpArcCenter, xChordVecLeft, xChordVecRight, xChordNormal;
		GEPOINT xArcTipLeft(-dfBoardL / 2, dfBaffleD / 2);
		GEPOINT xArcTipRight(dfBoardL / 2, dfBaffleD / 2);
		GEPOINT xArcMidPoint(0, dfBoardH1);
		f3dLine chordline1, chordline2;
		//����
		xChordVecLeft = xArcTipLeft - xArcMidPoint;
		xChordNormal.Set(-xChordVecLeft.y, xChordVecLeft.x);	//��ʱ��ת90��
		chordline1.startPt = (xArcTipLeft + xArcMidPoint)*0.5;
		chordline1.endPt = chordline1.startPt + xChordNormal;
		//����
		xChordVecRight = xArcMidPoint - xArcTipRight;
		xChordNormal.Set(-xChordVecRight.y, xChordVecRight.x);
		chordline2.startPt = (xArcTipRight + xArcMidPoint)*0.5;
		chordline2.endPt = chordline2.startPt + xChordNormal;
		int retcode = Int2dpl(f2dLine(chordline1.startPt, chordline1.endPt), f2dLine(chordline2.startPt, chordline2.endPt),
			xUpArcCenter.x, xUpArcCenter.y);
		if (retcode <= 0)
			return 0;
		//������Բ�ұߵ���������
		//int slices = 16;
		double radius = DISTANCE(xUpArcCenter, xArcMidPoint);
		double sectangle = cal_angle_of_2vec(xArcTipLeft - xUpArcCenter, xArcTipRight - xUpArcCenter);
		double slice_angle = sectangle / slices;//2*asin(Arcl/(4*r))/(slices-1);
		double sina = sin(slice_angle);	//��Ƭ������
		double cosa = cos(slice_angle);	//��Ƭ������
		parrVertexes[0] = xArcTipRight;
		for (int i = 1; i < slices; i++)
		{
			GEPOINT vLocalPoint = parrVertexes[i - 1] - xUpArcCenter;
			parrVertexes[i].x = vLocalPoint.x*cosa - vLocalPoint.y*sina;
			parrVertexes[i].y = vLocalPoint.x*sina + vLocalPoint.y*cosa;
			parrVertexes[i].z = 0;
			parrVertexes[i] = parrVertexes[i] + xUpArcCenter;
			//vertex_list.append(ztcs.TransPFromCS(parrVertexes[i]));
		}
		parrVertexes[slices] = xArcTipLeft;
		//�������Բ���߶����
		GEPOINT xArcTipLeft1, xArcTipRight1, xArcCenter(0, -dfBoardH2);
		f2dCircle circle(dfPointH, xArcCenter.x, xArcCenter.y);
		f2dPoint xBasePoint(-dfBoardL*0.5, -dfBaffleD * 0.5);
		f2dPoint xChinPick(0, -dfBoardH2 - dfPointH), xTangentPoint;
		if (Tang2dpc(xBasePoint, circle, xChinPick, xTangentPoint) < 1)
		{
			logerr.Log("δ�ҵ���๫�е�");
			return 0;
		}
		xArcTipLeft1.Set(xTangentPoint.x, xTangentPoint.y);
		xBasePoint.x *= -1;
		if (Tang2dpc(xBasePoint, circle, xChinPick, xTangentPoint) < 1)
		{
			logerr.Log("δ�ҵ��Ҳ๫�е�");
			return 0;
		}
		xArcTipRight1.Set(xTangentPoint.x, xTangentPoint.y);
		double radius1 = dfPointH;
		GEPOINT vVecStart(xArcTipLeft1.x, xArcTipLeft1.y + dfBoardH2);
		GEPOINT vVecEnd(xArcTipRight1.x, xArcTipRight1.y + dfBoardH2);
		double sectangle1 = cal_angle_of_2vec(vVecStart, vVecEnd);
		double slice_angle1 = sectangle1 / slices;
		double sina1 = sin(slice_angle1);	//��Ƭ������
		double cosa1 = cos(slice_angle1);	//��Ƭ������
		xarrChinArcVertexes[0] = xArcTipLeft1;
		for (int i = 1; i < slices; i++)
		{
			GEPOINT vLocalPoint1 = xarrChinArcVertexes[i - 1] - xArcCenter;
			xarrChinArcVertexes[i].x = vLocalPoint1.x*cosa1 - vLocalPoint1.y*sina1;
			xarrChinArcVertexes[i].y = vLocalPoint1.x*sina1 + vLocalPoint1.y*cosa1;
			xarrChinArcVertexes[i].z = 0;
			xarrChinArcVertexes[i] += xArcCenter;
		}
		xarrChinArcVertexes[slices] = xArcTipRight1;
		vertex_list.Empty();
		for (i = 0; i < 17; i++)
			vertex_list.append(ztcs.TransPFromCS(GEPOINT(parrVertexes[i].x, parrVertexes[i].y, parrVertexes[i].z)));
		vertex_list.append(ztcs.TransPFromCS(GEPOINT(-dfBoardL / 2, -dfBaffleD / 2)));
		//����Բ������ӽ��б�
		for (i = 0; i < 17; i++)
			vertex_list.append(ztcs.TransPFromCS(GEPOINT(xarrChinArcVertexes[i].x, xarrChinArcVertexes[i].y, xarrChinArcVertexes[i].z)));
		vertex_list.append(ztcs.TransPFromCS(GEPOINT(dfBoardL / 2, -dfBaffleD / 2)));

		
		AcDbObjectId motherZT = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);


		//���
		ztcs.origin.Set(0, -dfBoardH2);
		ztcs.origin.z = -dfHoleD;
		path.startPt = ztcs.origin;
		path.endPt = ztcs.origin + ztcs.axis_z*dfHoleD * 3;
		vertex_list.Empty();
		for (int i = 1; i < 32; i++)
		{
			GEPOINT pt = vertex_arr[i] * dfHoleD * 0.5;
			vertex_list.append(ztcs.TransPFromCS(pt));
		}
		AcDbObjectId motherSolidKong = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crLs);
		AcDbBlockTableRecord *pBlockTableRecord1 = GetBlockTableRecord();
		if (pBlockTableRecord1 == NULL)
			return NULL;
		AcDb3dSolid *p3dMainSolid1 = NULL, *pSubSolid1 = NULL;
		acdbOpenObject(p3dMainSolid1, motherZT, AcDb::kForWrite);
		if (p3dMainSolid1 == NULL)
			return NULL;
		acdbOpenObject(pSubSolid1, motherSolidKong, AcDb::kForWrite);
		if (pSubSolid1)
			p3dMainSolid1->booleanOper(AcDb::kBoolSubtract, pSubSolid1);
		if (pSubSolid1)
			pSubSolid1->close();
		p3dMainSolid1->close();



		GECS cs(GEPOINT(-fThroughLen / 2, 0, 0), GEPOINT(0, 0, -1), GEPOINT(0, 1, 0), GEPOINT(1, 0, 0));
		//�ݸ�
		cs.origin.x = dfBoardL * 0.5;
		path.startPt = cs.origin;
		path.endPt = cs.origin + cs.axis_z*(dfTrunnionL - dfBoardL)*0.5;
		vertex_list.Empty();
		for (int i = 0; i < 32; i++)
		{
			GEPOINT pt = vertex_arr[i] * dfTrunnionD * 0.5;
			vertex_list.append(cs.TransPFromCS(pt));
		}
		AcDbObjectId motherLGup = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);
		cs.origin.x = -dfBoardL * 0.5;	//���ݸ�λ��
		path.startPt = cs.origin;
		path.endPt = cs.origin + cs.axis_z*-(dfTrunnionL - dfBoardL)*0.5;
		vertex_list.Empty();
		for (int i = 0; i < 32; i++)
		{
			GEPOINT pt = vertex_arr[i] * dfTrunnionD * 0.5;
			vertex_list.append(cs.TransPFromCS(pt));
		}
		AcDbObjectId motherLGdown = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);

		//���Ӵ�ԲƬ
		cs.origin.x = dfBoardL * 0.5;
		path.startPt = cs.origin;
		path.endPt = cs.origin + cs.axis_z*dfBaffleH;
		vertex_list.Empty();
		for (int i = 0; i < 32; i++)
		{
			GEPOINT pt = vertex_arr[i] * dfBaffleD * 0.5;
			vertex_list.append(cs.TransPFromCS(pt));
		}		
		AcDbObjectId motherYPup = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);
		cs.origin.x = -dfBoardL*0.5 - dfBaffleH;	//�����Ӵ�λ��
		path.startPt = cs.origin;
		path.endPt = cs.origin + cs.axis_z*dfBaffleH;
		vertex_list.Empty();
		for (int i = 0; i < 32; i++)
		{
			GEPOINT pt = vertex_arr[i] * dfBaffleD * 0.5;
			vertex_list.append(cs.TransPFromCS(pt));
		}
		AcDbObjectId motherYPdown = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);
		//��Ƭ
		cs.origin.x = dfBoardL / 2 + siThroughThick;				//����ĸ��Ƭλ��
		path.startPt = cs.origin;
		path.endPt = cs.origin + cs.axis_z*dfWasherH;
		vertex_list.Empty();
		for (int i = 0; i < 32; i++)
		{
			GEPOINT pt = vertex_arr[i] * dfWasherD * 0.5;
			vertex_list.append(cs.TransPFromCS(pt));
		}
		AcDbObjectId motherSolidup = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);

		cs.origin.x = -dfBoardL / 2 - siThroughThick - dfWasherH;	//����ĸ��Ƭλ��
		path.startPt = cs.origin;
		path.endPt = cs.origin + cs.axis_z*dfWasherH;
		vertex_list.Empty();
		for (int i = 0; i < 32; i++)
		{
			GEPOINT pt = vertex_arr[i] * dfWasherD * 0.5;
			vertex_list.append(cs.TransPFromCS(pt));
		}
		AcDbObjectId motherSoliddown = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);

		//��ĸ
		cs.origin.x = dfBoardL / 2 + siThroughThick + dfWasherH;			//����ĸλ��
		path.startPt = cs.origin;
		path.endPt = cs.origin + cs.axis_z*dfNutH;
		vertex_list.Empty();
		for (int i = 0; i < 6; i++)
		{
			GEPOINT pt = ver_arr[i] * dfNutDe * 0.5;
			vertex_list.append(cs.TransPFromCS(pt));
		}
		AcDbObjectId motherLMup = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);

		cs.origin.x = -dfBoardL / 2 - siThroughThick - dfWasherH - dfNutH;	//����ĸλ��
		path.startPt = cs.origin;
		path.endPt = cs.origin + cs.axis_z*dfNutH;
		vertex_list.Empty();
		for (int i = 0; i < 6; i++)
		{
			GEPOINT pt = ver_arr[i] * dfNutDe * 0.5;
			vertex_list.append(cs.TransPFromCS(pt));
		}
		AcDbObjectId motherLMdown = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);

		//�رտ��¼
		pTempBlockTableRecord->close();
	}
	return solidId;
}
AcDbObjectId DrawURSolid(FITTING_UR* pInfo, COLORREF* pCrSolid /*= NULL*/)
{
	if (pInfo == NULL)
		return NULL;
	GEPOINT vertex_arr[32], ver_arr[6];	//��ʼģ�����ζ���
	GetCircleSimuPolyVertex(1.0, vertex_arr, 32);
	GetCircleSimuPolyVertex(1.0, ver_arr, 6);
	AcDbObjectId solidId = SearchBlock(pInfo->szSizeCode);
	if (solidId.isNull())
	{
		//�����¿��¼ָ��
		AcDbBlockTable *pTempBlockTable = NULL;
		GetCurDwg()->getBlockTable(pTempBlockTable, AcDb::kForWrite);
		AcDbBlockTableRecord *pTempBlockTableRecord = new AcDbBlockTableRecord();//�������¼ָ��
#ifdef _ARX_2007
		pTempBlockTableRecord->setName((ACHAR*)_bstr_t(pInfo->szSizeCode));
#else
		pTempBlockTableRecord->setName(pInfo->szSizeCode);
#endif
		pTempBlockTable->add(solidId, pTempBlockTableRecord);
		pTempBlockTable->close();
		//TODO:����URʵ��

		COLORREF crLs = RGB(91, 91, 91), crBoard = RGB(145, 149, 145);
		if (pCrSolid)
			crBoard = *pCrSolid;
		f3dLine path;
		fPtList vertex_list;
		GECS cs(GEPOINT(pInfo->dfBoardSpaceC / 2 + pInfo->dfT, 0, 0), GEPOINT(0, 0, 1), GEPOINT(0, 1, 0), GEPOINT(-1, 0, 0));
		
		double fEdge = 1.5;
		double fShafD = pInfo->dfShaftD - 2 * fEdge;
		double fShefLength = pInfo->dfBoardSpaceC + pInfo->dfT * 2 + 20;//���Ӽ�϶ + ������ + ��˨Ԥ�����ȣ�
		
		path.startPt = cs.origin;
		path.endPt = cs.origin + cs.axis_z*fShefLength;
		vertex_list.Empty();
		for (int i = 0; i < 32; i++)
		{
			GEPOINT pt = vertex_arr[i] * fShafD * 0.5;
			vertex_list.append(cs.TransPFromCS(pt));
		}
		//AcDbObjectId motherzt = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);
		////��ĸ
		double fNutDs = pInfo->dfRingBs + 8;
		double fNutDe = fNutDs * 1.133;
		double fNutH = 15;
		cs.origin.x *= -1.0;
		path.startPt = cs.origin;
		path.endPt = cs.origin + cs.axis_z*fNutH;

		vertex_list.Empty();
		for (int i = 0; i < 6; i++)
		{
			GEPOINT pt = ver_arr[i] * fNutDe * 0.5;
			vertex_list.append(cs.TransPFromCS(pt));
		}
		AcDbObjectId motherztLM = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);

		//��˨ͷ��������
		double fTopDs = pInfo->dfRingBs + 8;
		double fTopDe = fTopDs * 1.0625;
		cs.origin.x *= -1.0;
		cs.origin.x += fNutH;
		path.startPt = cs.origin;
		path.endPt = cs.origin + cs.axis_z*fNutH;

		vertex_list.Empty();
		for (int i = 0; i < 6; i++)
		{
			GEPOINT pt = ver_arr[i] * fTopDe * 0.5;
			vertex_list.append(cs.TransPFromCS(pt));
		}
		//AcDbObjectId motherztLMs = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);
		
		GECS acs(GEPOINT(pInfo->dfBoardSpaceC / 2, 0, 0), GEPOINT(0, 0, -1), GEPOINT(0, 1, 0), GEPOINT(1, 0, 0));
		//����Ҳ����Ӷ���
		
		path.startPt = acs.origin;
		path.endPt = acs.origin + acs.axis_z*pInfo->dfT;
		vertex_list.Empty();
		for (int i = 0; i < 32; i++)
		{
			GEPOINT pt = vertex_arr[i] * pInfo->dfMdEdgeSpace * 2 * 0.5;
			vertex_list.append(acs.TransPFromCS(pt));
		}
		//AcDbObjectId motherztLJBup = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);

		acs.origin.x *= -1.0;
		acs.axis_x *= -1.0;
		acs.axis_z *= -1.0;
		path.startPt = acs.origin;
		path.endPt = acs.origin + acs.axis_z*pInfo->dfT;

		vertex_list.Empty();
		for (int i = 0; i < 32; i++)
		{
			GEPOINT pt = vertex_arr[i] * pInfo->dfMdEdgeSpace * 2 * 0.5;
			vertex_list.append(acs.TransPFromCS(pt));
		}
		//AcDbObjectId motherztLJBdown = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, crBoard);

		//�رտ��¼
		pTempBlockTableRecord->close();
	}
	return solidId;
}
//������˨ʵ��(��ͨ��˨+�ؽ���˨)
AcDbObjectId DrawSolidBolt(CLDSBolt *pBolt, UCS_STRU* pUcs /*=NULL*/, double scale /*=1*/,
					COLORREF* pCrSolid /*=NULL*/, AcGeMatrix3d *pMatrix /*=NULL*/)
{
	LS_HOLE_INFO hole_info;
	hole_info.key = pBolt->handle;
	if (pBolt->GetClassTypeId() == CLS_BOLT)
	{
		hole_info.bolt_d = pBolt->get_d();
		hole_info.bolt_L0 = pBolt->get_L0();
		hole_info.bolt_L = pBolt->get_L();
		hole_info.bTwoCap = pBolt->IsTwoCap();
		return DrawBoltSolid(hole_info, pBolt->ucs, pUcs, scale, pCrSolid, pMatrix);
	}
	else //if (pBolt->GetClassTypeId() == CLS_ANCHORBOLT)
	{
		CLDSAnchorBolt* pAnchorBolt = (CLDSAnchorBolt*)pBolt;
		hole_info.bTwoCap = true;
		hole_info.bolt_d = pAnchorBolt->get_d();
		hole_info.bolt_L0 = pAnchorBolt->baseplate.wiBasePlateThick;
		hole_info.bolt_L = pAnchorBolt->m_fLe;
		hole_info.bolt_LR = pAnchorBolt->m_fLr;
		hole_info.bolt_PT = pAnchorBolt->washer.wiThick;
		hole_info.bolt_PW = pAnchorBolt->washer.wiWidth;
		return DrawSolidAnchorBolt(hole_info, pBolt->ucs, pUcs, scale, pCrSolid, pMatrix);
	}
}
//������ͨ��˨ʵ��
AcDbObjectId DrawBoltSolid(LS_HOLE_INFO& bolt_info, UCS_STRU assemble_ucs, UCS_STRU* pUcs/*=NULL*/, double scale/*=1*/,
					COLORREF* pCrSolid/*=NULL*/, AcGeMatrix3d *pMatrix/*=NULL*/)
{
	int d = bolt_info.bolt_d;;
	double L0 = bolt_info.bolt_L0;
	double L = bolt_info.bolt_L;
	//��˨��Y�����ļ��洢������Ҫʵʱ���� wjh-2018.5.18
	assemble_ucs.axis_y = assemble_ucs.axis_z^assemble_ucs.axis_x;
	CXhChar50 keyBolt, keyCap;
	keyBolt.Printf("Solid_M%dX%.0f", d, L);
	if (!bolt_info.bTwoCap)
		keyCap.Printf("Solid_M%d", d);	//��ñ
	else
		keyCap.Printf("Solid_M%dA", d);//˫ñ
	//
	AcDbObjectId boltSolidId[2], newEntId;
	boltSolidId[0] = SearchBlock(keyBolt);	//���п��¼Id
	boltSolidId[1] = SearchBlock(keyCap);	//���п��¼Id
	if (boltSolidId[0].isNull() || boltSolidId[1].isNull())
	{
		f3dLine path;
		fPtList vertex_list;
		GEPOINT vertex_arr[10];	//��ʼģ�����ζ���
		double D, H, H2, half_D;
		FindLsD_H(d, &D, &H, &H2);
		half_D = D * 0.5;
		//������˨��ɫ
		COLORREF color = RGB(255, 0, 255);
		if (d == 16)
			color = RGB(128, 0, 64);
		else if (d == 20)
			color = RGB(128, 0, 255);
		else if (d == 24)
			color = RGB(46, 0, 91);
		if (pCrSolid)
			color = *pCrSolid;
		AcDb3dSolid *pMotherSolid = NULL, *pSubSolid = NULL;
		GetCircleSimuPolyVertex(1.0, vertex_arr, 6);
		if (boltSolidId[0].isNull())
		{
			//�����¿��¼ָ��
			AcDbBlockTable *pTempBlockTable;
			GetCurDwg()->getBlockTable(pTempBlockTable, AcDb::kForWrite);
			AcDbBlockTableRecord *pTempBlockTableRecord = new AcDbBlockTableRecord();//�������¼ָ��
#ifdef _ARX_2007
			pTempBlockTableRecord->setName((ACHAR*)_bstr_t(keyBolt));
#else
			pTempBlockTableRecord->setName(keyBolt);
#endif
			pTempBlockTable->add(boltSolidId[0], pTempBlockTableRecord);
			pTempBlockTable->close();
			//1.������˨��
			double half_d = d * 0.5;
			path.startPt.Set();
			path.endPt.Set(0, 0, L);
			vertex_list.Empty();
			for (int i = 0; i < 6; i++)
				vertex_list.append(vertex_arr[i] * half_d);
			AcDbObjectId motherSolidId = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, color, bolt_info.key);
			//2.������˨ͷ
			path.startPt.Set();
			path.endPt.Set(0, 0, -H);
			vertex_list.Empty();
			for (int i = 0; i < 6; i++)
				vertex_list.append(vertex_arr[i] * half_D);
			AcDbObjectId subSolidId = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, color, bolt_info.key);
			acdbOpenObject(pMotherSolid, motherSolidId, AcDb::kForWrite);
			acdbOpenObject(pSubSolid, subSolidId, AcDb::kForWrite);
			if (pMotherSolid&&pSubSolid)
				pMotherSolid->booleanOper(AcDb::kBoolUnite, pSubSolid);
			if (pMotherSolid)
				pMotherSolid->close();
			if (pSubSolid)
				pSubSolid->close();
			pTempBlockTableRecord->close();//�رտ��¼
		}
		if (boltSolidId[1].isNull())
		{	//������˨ñ��
			//�����¿��¼ָ��
			AcDbBlockTable *pTempBlockTable;
			GetCurDwg()->getBlockTable(pTempBlockTable, AcDb::kForWrite);
			AcDbBlockTableRecord *pTempBlockTableRecord = new AcDbBlockTableRecord();//�������¼ָ��
#ifdef _ARX_2007
			pTempBlockTableRecord->setName((ACHAR*)_bstr_t(keyCap));
#else
			pTempBlockTableRecord->setName(keyCap);
#endif
			Acad::ErrorStatus errstatus = pTempBlockTable->add(boltSolidId[1], pTempBlockTableRecord);
			pTempBlockTable->close();

			path.startPt.Set();
			double capH = H2;
			if (bolt_info.bTwoCap)
				capH *= 2;
			path.endPt.Set(0, 0, capH);
			vertex_list.Empty();
			for (int i = 0; i < 6; i++)
				vertex_list.append(vertex_arr[i] * half_D);
			CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, color, bolt_info.key);
			pTempBlockTableRecord->close();
		}
	}
	AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
	if (pBlockTableRecord == NULL)
		return NULL;
	for (int i = 0; i < 2; i++)
	{
		AcDbBlockReference *pBlkRef = new AcDbBlockReference;
		pBlkRef->setBlockTableRecord(boltSolidId[i]);
		AcGeMatrix3d geMat;
		f3dPoint pos_orig = assemble_ucs.origin;
		f3dPoint pos_x = assemble_ucs.axis_x;
		f3dPoint pos_y = assemble_ucs.axis_y;
		f3dPoint pos_z = assemble_ucs.axis_z;
		// �淶����˨��װ������ϵ������һ������ʱ������������˨����ʾ��һ�Σ�������˨����ϵ�����ǲ��Ե�
		// ����˴����淶���ᵼ����˨ʵ������ʧ�� wjh-2019.8.17
		normalize(pos_z);
		if(pos_x.IsZero()||fabs(pos_x*pos_z)>EPS_COS)
			pos_x = inters_vec(pos_z);
		pos_x -= (pos_x*pos_z)*pos_z;
		normalize(pos_x);
		pos_y = pos_z^pos_x;

		TransPtToCadUcs(pos_orig, pUcs, scale);
		TransVtToCadUcs(pos_x, pUcs);
		TransVtToCadUcs(pos_y, pUcs);
		TransVtToCadUcs(pos_z, pUcs);
		AcGePoint3d origin(pos_orig.x, pos_orig.y, pos_orig.z);
		if (i == 1)
		{	//��˨ñ
			origin.x += pos_z.x*L0*(1.0 / scale);
			origin.y += pos_z.y*L0*(1.0 / scale);
			origin.z += pos_z.z*L0*(1.0 / scale);
		}
		AcGeVector3d xAxis(pos_x.x, pos_x.y, pos_x.z);
		AcGeVector3d yAxis(pos_y.x, pos_y.y, pos_y.z);
		AcGeVector3d zAxis(pos_z.x, pos_z.y, pos_z.z);
		double fScale = 1.0 / scale;
		geMat.setCoordSystem(origin, xAxis, yAxis, zAxis);
		pBlkRef->setBlockTransform(geMat);
		AcGeScale3d scaleXYZ(fScale, fScale, fScale);
		pBlkRef->setScaleFactors(fScale);
		pBlockTableRecord->appendAcDbEntity(newEntId, pBlkRef);
		pBlkRef->close();
	}
	pBlockTableRecord->close();
	return boltSolidId[0];
}
//���Ƶؽ���˨ʵ��
AcDbObjectId DrawSolidAnchorBolt(LS_HOLE_INFO& bolt_info, UCS_STRU assemble_ucs, UCS_STRU* pUcs/*=NULL*/, double scale/*=1*/,
							COLORREF* pCrSolid/*=NULL*/, AcGeMatrix3d *pMatrix/*=NULL*/)
{
	int d = bolt_info.bolt_d;
	double L0 = bolt_info.bolt_L0;
	double LR = bolt_info.bolt_LR;
	double L = bolt_info.bolt_L;
	double PT = bolt_info.bolt_PT;
	CXhChar50 keyBolt, keyCap, keyPad;
	keyBolt.Printf("Solid_M%d", d);
	keyCap.Printf("Solid_M%dA", d);
	keyPad.Printf("Solid_M%dP", d);
	//
	AcDbObjectId boltSolidId[3], newEntId;
	boltSolidId[0] = SearchBlock(keyBolt);	//�ݸ�
	boltSolidId[1] = SearchBlock(keyPad);		//���
	boltSolidId[2] = SearchBlock(keyCap);		//��ĸ
	if (boltSolidId[0].isNull() || boltSolidId[1].isNull() || boltSolidId[2].isNull())
	{
		f3dLine path;
		fPtList vertex_list;
		GEPOINT vertex_arr[10];	//��ʼģ�����ζ���
		double D, H, H2;
		FindLsD_H(d, &D, &H, &H2);
		//������˨��ɫ
		COLORREF color = RGB(255, 0, 255);
		if (d == 16)
			color = RGB(128, 0, 64);
		else if (d == 20)
			color = RGB(128, 0, 255);
		else if (d == 24)
			color = RGB(46, 0, 91);
		if (pCrSolid)
			color = *pCrSolid;
		AcDb3dSolid *pMotherSolid = NULL, *pSubSolid = NULL;
		GetCircleSimuPolyVertex(1.0, vertex_arr, 6);
		if (boltSolidId[0].isNull())
		{	//�ݸ�ʵ��
			AcDbBlockTable *pTempBlockTable = NULL;
			GetCurDwg()->getBlockTable(pTempBlockTable, AcDb::kForWrite);
			AcDbBlockTableRecord *pTempBlockTableRecord = new AcDbBlockTableRecord();//�������¼ָ��
#ifdef _ARX_2007
			pTempBlockTableRecord->setName((ACHAR*)_bstr_t(keyBolt));
#else
			pTempBlockTableRecord->setName(keyBolt);
#endif
			pTempBlockTable->add(boltSolidId[0], pTempBlockTableRecord);
			pTempBlockTable->close();
			//1.������˨��
			double half_d = d * 0.5;
			path.startPt.Set();
			path.endPt.Set(0, 0, L);
			vertex_list.Empty();
			for (int i = 0; i < 6; i++)
				vertex_list.append(vertex_arr[i] * half_d);
			AcDbObjectId motherSolidId = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, color, bolt_info.key);
			//2.������˨ͷ
			path.startPt.Set();
			path.endPt.Set(0, 0, -LR);
			vertex_list.Empty();
			for (int i = 0; i < 6; i++)
				vertex_list.append(vertex_arr[i] * half_d);
			AcDbObjectId subSolidId = CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, color, bolt_info.key);
			acdbOpenObject(pMotherSolid, motherSolidId, AcDb::kForWrite);
			acdbOpenObject(pSubSolid, subSolidId, AcDb::kForWrite);
			if (pMotherSolid&&pSubSolid)
				pMotherSolid->booleanOper(AcDb::kBoolUnite, pSubSolid);
			if (pMotherSolid)
				pMotherSolid->close();
			if (pSubSolid)
				pSubSolid->close();
			pTempBlockTableRecord->close();//�رտ��¼
		}
		if (boltSolidId[1].isNull())
		{	//������˨ñ��
			AcDbBlockTable *pTempBlockTable = NULL;
			GetCurDwg()->getBlockTable(pTempBlockTable, AcDb::kForWrite);
			AcDbBlockTableRecord *pTempBlockTableRecord = new AcDbBlockTableRecord();//�������¼ָ��
#ifdef _ARX_2007
			pTempBlockTableRecord->setName((ACHAR*)_bstr_t(keyPad));
#else
			pTempBlockTableRecord->setName(keyPad);
#endif
			Acad::ErrorStatus errstatus = pTempBlockTable->add(boltSolidId[1], pTempBlockTableRecord);
			pTempBlockTable->close();
			//
			double half_w = bolt_info.bolt_PW*0.5;
			vertex_list.Empty();
			vertex_list.append(f3dPoint(half_w, half_w, 0));
			vertex_list.append(f3dPoint(-half_w, half_w, 0));
			vertex_list.append(f3dPoint(-half_w, -half_w, 0));
			vertex_list.append(f3dPoint(half_w, -half_w, 0));
			path.startPt.Set();
			path.endPt.Set(0, 0, PT);
			CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, color, bolt_info.key);
			pTempBlockTableRecord->close();
		}
		if (boltSolidId[2].isNull())
		{	//������˨ñ��
			AcDbBlockTable *pTempBlockTable = NULL;
			GetCurDwg()->getBlockTable(pTempBlockTable, AcDb::kForWrite);
			AcDbBlockTableRecord *pTempBlockTableRecord = new AcDbBlockTableRecord();//�������¼ָ��
#ifdef _ARX_2007
			pTempBlockTableRecord->setName((ACHAR*)_bstr_t(keyCap));
#else
			pTempBlockTableRecord->setName(keyCap);
#endif
			Acad::ErrorStatus errstatus = pTempBlockTable->add(boltSolidId[2], pTempBlockTableRecord);
			pTempBlockTable->close();
			//
			double capH = H2 * 2, half_D = D * 0.5;
			path.startPt.Set();
			path.endPt.Set(0, 0, capH);
			vertex_list.Empty();
			for (int i = 0; i < 6; i++)
				vertex_list.append(vertex_arr[i] * half_D);
			CreateSolidByExtrude(pTempBlockTableRecord, vertex_list, path, color, bolt_info.key);
			pTempBlockTableRecord->close();
		}
	}
	AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
	if (pBlockTableRecord == NULL)
		return NULL;
	assemble_ucs.axis_y = assemble_ucs.axis_z^assemble_ucs.axis_x;
	for (int i = 0; i < 3; i++)
	{
		AcDbBlockReference *pBlkRef = new AcDbBlockReference;
		pBlkRef->setBlockTableRecord(boltSolidId[i]);
		AcGeMatrix3d geMat;
		f3dPoint pos_orig = assemble_ucs.origin;
		f3dPoint pos_x = assemble_ucs.axis_x;
		f3dPoint pos_y = assemble_ucs.axis_y;
		f3dPoint pos_z = assemble_ucs.axis_z;
		TransPtToCadUcs(pos_orig, pUcs, scale);
		TransVtToCadUcs(pos_x, pUcs);
		TransVtToCadUcs(pos_y, pUcs);
		TransVtToCadUcs(pos_z, pUcs);
		AcGePoint3d origin(pos_orig.x, pos_orig.y, pos_orig.z);
		if (i == 1)
		{	//���
			origin.x += pos_z.x*L0*(1.0 / scale);
			origin.y += pos_z.y*L0*(1.0 / scale);
			origin.z += pos_z.z*L0*(1.0 / scale);
		}
		if (i == 2)
		{	//��˨ñ
			origin.x += pos_z.x*(L0 + PT)*(1.0 / scale);
			origin.y += pos_z.y*(L0 + PT)*(1.0 / scale);
			origin.z += pos_z.z*(L0 + PT)*(1.0 / scale);
		}

		AcGeVector3d xAxis(pos_x.x, pos_x.y, pos_x.z);
		AcGeVector3d yAxis(pos_y.x, pos_y.y, pos_y.z);
		AcGeVector3d zAxis(pos_z.x, pos_z.y, pos_z.z);
		double fScale = 1.0 / scale;
		geMat.setCoordSystem(origin, xAxis, yAxis, zAxis);
		pBlkRef->setBlockTransform(geMat);
		AcGeScale3d scaleXYZ(fScale, fScale, fScale);
		pBlkRef->setScaleFactors(fScale);
		if (pMatrix)
			pBlkRef->setBlockTransform(*pMatrix);
		//
		pBlockTableRecord->appendAcDbEntity(newEntId, pBlkRef);
		pBlkRef->close();
	}
	pBlockTableRecord->close();
	return boltSolidId[0];
}
void Draw3DLineMap(UCS_STRU ucs, double scale /*= 1*/, BOOL bTurnLeft /*= FALSE*/)
{
	CLDSModule *pModule = Ta.GetActiveModule();
	if (pModule == NULL)
		return;
	PARTSET selRodSet;
	for (CLDSPart* pPart = Ta.EnumPartFirst(); pPart; pPart = Ta.EnumPartNext())
	{
		if (!pPart->IsLinePart() || pPart->m_bVirtualPart)
			continue;
		if (!pModule->IsSonPart(pPart))
			continue;
		CLDSLinePart* pRod = (CLDSLinePart*)pPart;
		if (pRod->pStart == NULL || pRod->pEnd == NULL)
			continue;
		selRodSet.append(pPart);
	}
	return Draw3DLineMap(selRodSet, ucs, scale, bTurnLeft);
}
void Draw3DLineMap(PARTSET& partset, UCS_STRU ucs, double scale /*= 1*/, BOOL bTurnLeft /*= FALSE*/)
{
	AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
	if (pBlockTableRecord == NULL)
		return;
	//��ʼ����ת����
	AcGePoint3d origin(0, 0, 0);
	AcGeVector3d xAxis(-1, 0, 0);
	AcGeVector3d yAxis(0, -1, 0);
	AcGeVector3d zAxis(0, 0, 1);
	AcGeMatrix3d rot_mat;
	rot_mat.setCoordSystem(origin, xAxis, yAxis, zAxis);
	//���Ƹ˼����ߣ�����ǰ���Ƚ�ԭ��������ϵת�����û�����ϵucs��
	for (CLDSPart *pPart = partset.GetFirst(); pPart; pPart = partset.GetNext())
	{
		if(!pPart->IsLinePart() || pPart->m_bVirtualPart)
			continue;
		CLDSLinePart *pLinePart = (CLDSLinePart*)pPart;
		if (pLinePart->pStart == NULL || pLinePart->pEnd == NULL)
			continue;
		//����ACADLINE����
		AcGePoint3d start, end;
		f3dPoint ptOrgS = pLinePart->pStart->Position(false);
		TransPtToCadUcs(ptOrgS, &ucs, scale);
		Cpy_Pnt(start, ptOrgS);
		//�յ�
		f3dPoint ptOrgE= pLinePart->pEnd->Position(false);
		TransPtToCadUcs(ptOrgE, &ucs, scale);
		Cpy_Pnt(end, ptOrgE);
		AcDbObjectId LineId;
		AcDbLine *pLine = new AcDbLine(start, end);//����LINE����
		char cSec = pLinePart->layer(1);
		if (towupper(cSec) == 'F')
			pLine->setColorIndex(4);	//����������ɫ��ʾ
		if (bTurnLeft)
			pLine->transformBy(rot_mat);
		pBlockTableRecord->appendAcDbEntity(LineId, pLine);
		pLine->createExtensionDictionary();
		pLine->close();//���ùرն���ĳ�Ա����
		//
		AcDbObjectId dictObjId, xrecObjId;
		dictObjId = pLine->extensionDictionary();
		AcDbDictionary *pDict=NULL;
		acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
		AcDbXrecord *pXrec = new AcDbXrecord;
#ifdef _ARX_2007
		pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
		pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
		pDict->close();
		struct resbuf *head;
		head = acutBuildList(AcDb::kDxfInt32, pLinePart->handle, 0);
		pXrec->setFromRbChain(*head);
		acutRelRb(head);
		pXrec->close();
	}
	pBlockTableRecord->close();
}
//���Ƹ����ĵ���άʵ��ͼ
void Draw3DSolid(UCS_STRU ucs, BOOL bPrompt, double scale /*= 1*/, BYTE ciModelLevel /*= 3*/, BOOL bTurnLeft /*= FALSE*/)
{
	CLDSModule *pModule = Ta.GetActiveModule();
	if (pModule == NULL)
		return;
	PARTSET selPartSet;
	for (CLDSPart *pPart = Ta.Parts.GetFirst(); pPart; pPart = Ta.Parts.GetNext())
	{
		if (pPart->m_bVirtualPart)
			continue;
		if(pModule->IsSonPart(pPart)||pPart->GetClassTypeId()==CLS_FITTINGS)
			selPartSet.append(pPart);
	}
	Draw3DSolid(selPartSet, ucs, bPrompt, scale, ciModelLevel,bTurnLeft);
	//
	if (Ta.BlockRef.GetNodeNum() > 0)
		DrawBlockSolid(ucs, bPrompt,scale, ciModelLevel,bTurnLeft);
}
void Draw3DSolid(PARTSET& partset, UCS_STRU ucs, BOOL bPrompt, double scale /*=1*/,BYTE ciModelLevel /*= 3*/, BOOL bTurnLeft /*= FALSE*/)
{
	//��ʼ��λ���������
	AcGePoint3d origin(0, 0, 0);
	AcGeVector3d xAxis(-1, 0, 0);
	AcGeVector3d yAxis(0, -1, 0);
	AcGeVector3d zAxis(0, 0, 1);
	AcGeMatrix3d rot_mat;
	rot_mat.setCoordSystem(origin, xAxis, yAxis, zAxis);
	AcGeMatrix3d* pRotMat = bTurnLeft ? (&rot_mat) : NULL;
	//����ʵ��
	int nCurProcess = 0, nPartNum = partset.GetNodeNum();
	if(bPrompt)
		DisplayProcess(0, "ʵ�����ɽ���");
	for (CLDSPart *pPart = partset.GetFirst(); pPart; pPart = partset.GetNext())
	{
		if(bPrompt)
			DisplayProcess(ftoi(100 * nCurProcess / nPartNum), "ʵ�����ɽ���");
		UCS_STRU part_ucs;
		if (pPart->GetClassTypeId() == CLS_LINEANGLE)
			((CLDSLineAngle*)pPart)->getUCS(part_ucs);
		else if (pPart->GetClassTypeId() == CLS_LINETUBE)
			part_ucs = ((CLDSLineTube*)pPart)->BuildUCS();
		else if (pPart->GetClassTypeId() == CLS_LINESLOT)
			part_ucs = ((CLDSLineSlot*)pPart)->BuildUCS();
		else if (pPart->GetClassTypeId() == CLS_LINEFLAT)
			part_ucs = ((CLDSLineFlat*)pPart)->BuildUCS();
		else
			part_ucs = pPart->ucs;
		COLORREF crSolid;
		if (pPart->Layer(0) == 'S' || pPart->Layer(0) == 'L')
			crSolid = RGB(255, 255, 255);
		else if (part_ucs.origin.x > 0)	//�Һᵣ
			crSolid = sys.general_set.rCrossarmCr;
		else	//��ᵣ
			crSolid = sys.general_set.lCrossarmCr;
		//
		if (pPart->GetClassTypeId() == CLS_LINEANGLE)
		{
			CLDSLineAngle *pLinkAngle = (CLDSLineAngle *)pPart;
			DrawSolidAngle(pLinkAngle, &ucs, scale, &crSolid, pRotMat);
		}
		else if (pPart->GetClassTypeId() == CLS_PLATE)
		{
			CLDSPlate *pPlate = (CLDSPlate *)pPart;
			int nNum = pPlate->vertex_list.GetNodeNum();
			if (nNum < 3)
				continue;
			if(ciModelLevel>1)
				DrawSolidPlate(pPlate, &ucs, scale, &crSolid, pRotMat);
		}
		else if (pPart->GetClassTypeId() == CLS_PARAMPLATE)
		{
			CLDSParamPlate *pPlate = (CLDSParamPlate *)pPart;
			if(ciModelLevel>1)
				DrawSolidParamPlate(pPlate, &ucs, scale, &crSolid, pRotMat);
		}
		else if (pPart->GetClassTypeId() == CLS_BOLT)
		{
			CLDSBolt *pBolt = (CLDSBolt *)pPart;
			if(ciModelLevel==3)
				DrawSolidBolt(pBolt, &ucs, scale, &crSolid, pRotMat);
		}
		else if (pPart->GetClassTypeId() == CLS_ANCHORBOLT)
		{	//�ؽ���˨
			CLDSAnchorBolt* pAnchorBolt = (CLDSAnchorBolt*)pPart;
			if(ciModelLevel==3)
				DrawSolidBolt(pAnchorBolt, &ucs, scale, &crSolid, pRotMat);
		}
		else if (pPart->GetClassTypeId() == CLS_LINETUBE)
		{
			CLDSLineTube *pTube = (CLDSLineTube *)pPart;
			DrawSolidTube(pTube, &ucs, scale, &crSolid, pRotMat);
		}
		else if (pPart->GetClassTypeId() == CLS_LINESLOT)
		{
			CLDSLineSlot *pSlot = (CLDSLineSlot *)pPart;
			DrawSolidSlot(pSlot, &ucs, scale, &crSolid, pRotMat);
		}
		else if (pPart->GetClassTypeId() == CLS_LINEFLAT)
		{
			CLDSLineFlat *pFlat = (CLDSLineFlat *)pPart;
			DrawSolidFlat(pFlat, &ucs, scale, &crSolid, pRotMat);
		}
		else if (pPart->GetClassTypeId() == CLS_FITTINGS)
		{
			CLDSFitting *pFitting = (CLDSFitting *)pPart;
			DrawSolidFitting(pFitting, &ucs, scale, &crSolid, pRotMat);
		}
		nCurProcess++;
	}
	if (bPrompt)
		DisplayProcess(100, "ʵ�����ɽ���");
}
//���Ʋ�����άʵ��
void DrawBlockSolid(UCS_STRU ucs, BOOL bPrompt, double scale /*= 1*/, BYTE ciModelLevel /*= 3*/,BOOL bTurnLeft /*= FALSE*/)
{
	//��ʼ��λ���������
	AcGePoint3d origin(0, 0, 0);
	AcGeVector3d xAxis(-1, 0, 0);
	AcGeVector3d yAxis(0, -1, 0);
	AcGeVector3d zAxis(0, 0, 1);
	AcGeMatrix3d rot_mat;
	rot_mat.setCoordSystem(origin, xAxis, yAxis, zAxis);
	AcGeMatrix3d* pRotMat = bTurnLeft ? (&rot_mat) : NULL;
	//���Ʋ��������е�ʵ��
	for (CBlockReference *pBlockRef = Ta.BlockRef.GetFirst(); pBlockRef; pBlockRef = Ta.BlockRef.GetNext())
	{
		CBlockModel *pBlock = Ta.Block.FromHandle(pBlockRef->m_hBlock);
		if (pBlock == NULL)
			continue;
		PARTSET blockPartSet;
		if (pBlock->IsEmbeded())
		{	//Ƕ��ʽ����
			for (CLDSPart *pPart = Ta.EnumPartFirst(); pPart; pPart = Ta.EnumPartNext())
			{
				if (pPart->m_hBlock == pBlock->handle)
					blockPartSet.append(pPart);
			}
		}
		else
		{	//��Ƕ��ʽ����
			for (CLDSPart* pPart = pBlock->Parts.GetFirst(); pPart; pPart = pBlock->Parts.GetNext())
				blockPartSet.append(pPart);
		}
		UCS_STRU cs = pBlock->TransToACS(pBlockRef->GetACS());
		COLORREF crSolid = RGB(255, 255, 255);
		CBuffer clone(4096);
		int nCurProcess = 0;
		int nPartNum = blockPartSet.GetNodeNum();
		if (bPrompt)
			DisplayProcess(nCurProcess, "�����й���ʵ�����ɽ���");
		for (CLDSPart *pPart = blockPartSet.GetFirst(); pPart; pPart = blockPartSet.GetNext(), nCurProcess++)
		{
			if (bPrompt)
				DisplayProcess(ftoi(100 * nCurProcess / nPartNum), "�����й���ʵ�����ɽ���");
			clone.ClearContents();
			pPart->ToBuffer(clone);
			clone.SeekToBegin();
			if (pPart->GetClassTypeId() == CLS_LINEANGLE)
			{
				CLDSLineAngle xLinkAngle;
				xLinkAngle.SetBelongModel(&Ta);
				xLinkAngle.FromBuffer(clone);
				f3dPoint ptS = xLinkAngle.Start();
				f3dPoint ptE = xLinkAngle.End();
				coord_trans(ptS, cs, TRUE, TRUE);
				coord_trans(ptE, cs, TRUE, TRUE);
				coord_trans(xLinkAngle.ucs.origin, cs, TRUE, TRUE);
				vector_trans(xLinkAngle.ucs.axis_x, cs, TRUE, TRUE);
				vector_trans(xLinkAngle.ucs.axis_y, cs, TRUE, TRUE);
				vector_trans(xLinkAngle.ucs.axis_z, cs, TRUE, TRUE);
				xLinkAngle.SetStart(ptS);
				xLinkAngle.SetEnd(ptE);
				DrawSolidAngle(&xLinkAngle, &ucs, scale, &crSolid, pRotMat);
			}
			else if (pPart->GetClassTypeId() == CLS_PLATE)
			{
				CLDSPlate xPlate;
				xPlate.SetBelongModel(&Ta);
				xPlate.FromBuffer(clone);
				coord_trans(xPlate.ucs.origin, cs, TRUE, TRUE);
				vector_trans(xPlate.ucs.axis_x, cs, TRUE, TRUE);
				vector_trans(xPlate.ucs.axis_y, cs, TRUE, TRUE);
				vector_trans(xPlate.ucs.axis_z, cs, TRUE, TRUE);
				if(ciModelLevel>1)
					DrawSolidPlate(&xPlate, &ucs, scale, &crSolid, pRotMat);
			}
			else if (pPart->GetClassTypeId() == CLS_PARAMPLATE)
			{
				CLDSParamPlate xPlate;
				xPlate.SetBelongModel(&Ta);
				xPlate.FromBuffer(clone);
				coord_trans(xPlate.ucs.origin, cs, TRUE, TRUE);
				vector_trans(xPlate.ucs.axis_x, cs, TRUE, TRUE);
				vector_trans(xPlate.ucs.axis_y, cs, TRUE, TRUE);
				vector_trans(xPlate.ucs.axis_z, cs, TRUE, TRUE);
				if(ciModelLevel>1)
					DrawSolidParamPlate(&xPlate, &ucs, scale, &crSolid, pRotMat);
			}
			else if (pPart->GetClassTypeId() == CLS_BOLT)
			{
				CLDSBolt xBolt;
				xBolt.SetBelongModel(&Ta);
				xBolt.FromBuffer(clone);
				coord_trans(xBolt.ucs.origin, cs, TRUE, TRUE);
				vector_trans(xBolt.ucs.axis_x, cs, TRUE, TRUE);
				vector_trans(xBolt.ucs.axis_y, cs, TRUE, TRUE);
				vector_trans(xBolt.ucs.axis_z, cs, TRUE, TRUE);
				if(ciModelLevel==3)
					DrawSolidBolt(&xBolt, &ucs, scale, &crSolid, pRotMat);
			}
			else if (pPart->GetClassTypeId() == CLS_LINETUBE)
			{
				CLDSLineTube xTube;
				xTube.SetBelongModel(&Ta);
				xTube.FromBuffer(clone);
				f3dPoint ptS = xTube.Start();
				f3dPoint ptE = xTube.End();
				coord_trans(ptS, cs, TRUE, TRUE);
				coord_trans(ptE, cs, TRUE, TRUE);
				coord_trans(xTube.ucs.origin, cs, TRUE, TRUE);
				vector_trans(xTube.ucs.axis_x, cs, TRUE, TRUE);
				vector_trans(xTube.ucs.axis_y, cs, TRUE, TRUE);
				vector_trans(xTube.ucs.axis_z, cs, TRUE, TRUE);
				xTube.SetStart(ptS);
				xTube.SetEnd(ptE);
				DrawSolidTube(&xTube, &ucs, scale, &crSolid, pRotMat);
			}
			else if (pPart->GetClassTypeId() == CLS_LINESLOT)
			{
				CLDSLineSlot xSlot;
				xSlot.SetBelongModel(&Ta);
				xSlot.FromBuffer(clone);
				f3dPoint ptS = xSlot.Start();
				f3dPoint ptE = xSlot.End();
				coord_trans(ptS, cs, TRUE, TRUE);
				coord_trans(ptE, cs, TRUE, TRUE);
				coord_trans(xSlot.ucs.origin, cs, TRUE, TRUE);
				vector_trans(xSlot.ucs.axis_x, cs, TRUE, TRUE);
				vector_trans(xSlot.ucs.axis_y, cs, TRUE, TRUE);
				vector_trans(xSlot.ucs.axis_z, cs, TRUE, TRUE);
				xSlot.SetStart(ptS);
				xSlot.SetEnd(ptE);
				DrawSolidSlot(&xSlot, &ucs, scale, &crSolid, pRotMat);
			}
			else if (pPart->GetClassTypeId() == CLS_LINEFLAT)
			{
				CLDSLineFlat xFlat;
				xFlat.SetBelongModel(&Ta);
				xFlat.FromBuffer(clone);
				f3dPoint ptS = xFlat.Start();
				f3dPoint ptE = xFlat.End();
				coord_trans(ptS, cs, TRUE, TRUE);
				coord_trans(ptE, cs, TRUE, TRUE);
				coord_trans(xFlat.ucs.origin, cs, TRUE, TRUE);
				vector_trans(xFlat.ucs.axis_x, cs, TRUE, TRUE);
				vector_trans(xFlat.ucs.axis_y, cs, TRUE, TRUE);
				vector_trans(xFlat.ucs.axis_z, cs, TRUE, TRUE);
				xFlat.SetStart(ptS);
				xFlat.SetEnd(ptE);
				DrawSolidFlat(&xFlat, &ucs, scale, &crSolid, pRotMat);
			}
		}
		if (bPrompt)
			DisplayProcess(100, "�����й���ʵ�����ɽ���");
	}
}
//////////////////////////////////////////////////////////////////////////
//����TID�й���ʵ��
//���ƽǸ���λʵ
AcDbObjectId DrawSolidAngle(CProcessAngle *pJg, UCS_STRU assemble_ucs, UCS_STRU* pUcs/*=NULL*/, double scale/*=1*/,
					COLORREF* pCrSolid/*=NULL*/, AcGeMatrix3d *pMatrix/*=NULL*/)
{
	if (pJg == NULL)
		return AcDbObjectId::kNull;
	f3dPoint vXWingNorm = pJg->GetWingNormX();
	f3dPoint vYWingNorm = pJg->GetWingNormY();
	f3dPoint vXWingVec = pJg->GetWingVecX();
	f3dPoint vYWingVec = pJg->GetWingVecY();
	f3dPoint bi_vec(1 + vYWingVec.x, vYWingVec.y, 0);
	normalize(bi_vec);
	//����������
	f3dPoint vertex_arr[8];
	vertex_arr[0].Set(0, 0, 0);
	vertex_arr[1] = vYWingVec * pJg->m_fWidth;
	vertex_arr[2] = vertex_arr[1] - vYWingNorm * pJg->m_fThick;
	vertex_arr[3].Set(pJg->m_fThick*bi_vec.x / bi_vec.y, pJg->m_fThick);
	vertex_arr[4].Set(pJg->m_fWidth, pJg->m_fThick);
	vertex_arr[5].Set(pJg->m_fWidth, 0, 0);
	fPtList vertex_list;
	for (int i = 0; i < 6; i++)
	{
		vertex_arr[i].z = 0;
		coord_trans(vertex_arr[i], assemble_ucs, TRUE);
		TransPtToCadUcs(vertex_arr[i], pUcs, scale);
		vertex_list.append(vertex_arr[i]);
	}
	//��¼�����߶���ʼ�����ֹ��
	f3dLine path;
	path.startPt = assemble_ucs.origin;
	path.endPt = assemble_ucs.origin + assemble_ucs.axis_z*pJg->m_wLength;
	TransPtToCadUcs(path.startPt, pUcs, scale);
	TransPtToCadUcs(path.endPt, pUcs, scale);
	//
	COLORREF crSolid = RGB(150, 150, 255);
	if (pCrSolid)
		crSolid = *pCrSolid;
	AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
	if (pBlockTableRecord == NULL)
		return NULL;
	AcDbObjectId solidId;
	solidId = CreateSolidByExtrude(pBlockTableRecord, vertex_list, path, crSolid);
	pBlockTableRecord->close();
	//��ʵ��ͼԪλ�ý��е���
	if (pMatrix)
	{
		AcDb3dSolid *p3dObj = NULL;
		acdbOpenObject(p3dObj, solidId, AcDb::kForWrite);
		p3dObj->transformBy(*pMatrix);
		p3dObj->close();
	}
	return solidId;
}
//���Ƹְ���λʵ��
AcDbObjectId DrawSolidPlate(CProcessPlate *pPlate, UCS_STRU assemble_ucs, UCS_STRU* ucs/*=NULL*/, double scale/*=1*/,
					COLORREF* pCrSolid/*=NULL*/, AcGeMatrix3d *pMatrix/*=NULL*/)
{
	if (pPlate == NULL || pPlate->vertex_list.GetNodeNum()<3)
		return AcDbObjectId::kNull;
	COLORREF crSolid = RGB(0, 255, 128);
	if (pCrSolid)
		crSolid = *pCrSolid;
	if (pPlate->m_cFaceN == 1)
	{
		//ͳ�Ƹְ�������
		fPtList vertex_list;
		PROFILE_VER *pPrevPnt = pPlate->vertex_list.GetTail();
		for (PROFILE_VER *pVertex = pPlate->vertex_list.GetFirst(); pVertex; pVertex = pPlate->vertex_list.GetNext())
		{
			f3dPoint vertex = pVertex->vertex;
			if (pPrevPnt->type == 1)
			{	//����ֱ��
				f3dPoint temp_vertex(vertex.x, vertex.y, vertex.z);
				coord_trans(temp_vertex, assemble_ucs, TRUE);
				TransPtToCadUcs(temp_vertex, ucs, scale);
				vertex_list.append(temp_vertex);
			}
			else //if(pPrevVertex->type>1)
			{	//����Բ�����
				f3dArcLine arcLine;
				if (pPrevPnt->type == 2)	//Բ��
					arcLine.CreateMethod2(pPrevPnt->vertex, pVertex->vertex, pPrevPnt->work_norm, pPrevPnt->sector_angle);
				else //if(pPrevPnt->type==3)	//��Բ��
					arcLine.CreateEllipse(pPrevPnt->center, pPrevPnt->vertex, pVertex->vertex, pPrevPnt->column_norm, pPrevPnt->work_norm, pPrevPnt->radius);
				fPtList sub_vertex_list;
				CalArcLineVertexList(arcLine, sub_vertex_list);
				for (f3dPoint *pPt = sub_vertex_list.GetFirst(); pPt; pPt = sub_vertex_list.GetNext())
				{
					f3dPoint temp_vertex(pPt->x, pPt->y, pPt->z);
					coord_trans(temp_vertex, assemble_ucs, TRUE);
					TransPtToCadUcs(temp_vertex, ucs, scale);
					vertex_list.append(temp_vertex);
				}
			}
			pPrevPnt = pVertex;
		}
		//�������췽��
		f3dLine path;
		for (PROFILE_VER *pVertex = pPlate->vertex_list.GetFirst(); pVertex; pVertex = pPlate->vertex_list.GetNext())
			path.startPt += pVertex->vertex;
		path.startPt /= pPlate->vertex_list.GetNodeNum();
		path.endPt.Set(path.startPt.x, path.startPt.y, path.startPt.z + pPlate->m_fThick);
		coord_trans(path.startPt, assemble_ucs, TRUE);
		coord_trans(path.endPt, assemble_ucs, TRUE);
		TransPtToCadUcs(path.startPt, ucs, scale);
		TransPtToCadUcs(path.endPt, ucs, scale);
		//��������ʵ��
		AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
		if (pBlockTableRecord == NULL)
			return NULL;
		AcDbObjectId mainPlateSolidId;
		mainPlateSolidId = CreateSolidByExtrude(pBlockTableRecord, vertex_list, path, crSolid);
		pBlockTableRecord->close();
		//��ʵ��ͼԪλ�ý��е���
		if (pMatrix)
		{
			AcDb3dSolid *p3dObj = NULL;
			acdbOpenObject(p3dObj, mainPlateSolidId, AcDb::kForWrite);
			p3dObj->transformBy(*pMatrix);
			p3dObj->close();
		}
		return mainPlateSolidId;
	}
	else //if (pPlate->m_cFaceN >= 2)
	{
		int i = 0, j = 0, k = 0;
		f3dPoint pt3d[100], pt3d1[100], pt3d3[100], endPt, endPt1, endPt2, pt1, pt2, pt3, pt4, ptHuoqu1, ptHuoqu2, ptHuoqu3, ptHuoqu4,
			top_point2, top_point3;
		double angle = pPlate->GetHuoQuAngle(1);
		double angle1 = pPlate->GetHuoQuAngle(2);
		if (pPlate->m_cFaceN > 2)
		{	//�����top_point
			top_point2 = pPlate->top_point;
			rotate_point_around_axis(top_point2, angle, pPlate->HuoQuLine[0]);
			coord_trans(top_point2, assemble_ucs, TRUE);
			TransPtToCadUcs(top_point2, ucs, scale);
			//
			top_point3 = pPlate->top_point;
			rotate_point_around_axis(top_point3, angle1, pPlate->HuoQuLine[1]);
			coord_trans(top_point3, assemble_ucs, TRUE);
			TransPtToCadUcs(top_point3, ucs, scale);

			f3dPoint top_point = pPlate->top_point;
			coord_trans(top_point, assemble_ucs, TRUE);
			pt2 = top_point;
			TransPtToCadUcs(pt2, ucs, scale);
			pt4 = pt2;
			//
			ptHuoqu2 = top_point + assemble_ucs.axis_z*pPlate->GetThick();
			TransPtToCadUcs(ptHuoqu2, ucs, scale);
			ptHuoqu4 = ptHuoqu2;
		}
		//pt3d�����洢���ϵĶ��㣬endpt��¼�������ת��׼���յ㣬pt��huoqu��¼������
		BOOL bFirst = TRUE, bFirst1 = TRUE;
		int nID = 0;
		for (PROFILE_VER *pVertex = pPlate->vertex_list.GetFirst(); pVertex; pVertex = pPlate->vertex_list.GetNext())
		{
			f3dPoint vertex = pVertex->vertex;
			//�ǻ�׼�����Ҫ��ת����Ӧ����
			if (vertex.feature == 2)
				rotate_point_around_axis(vertex, angle, pPlate->HuoQuLine[0]);
			else if (vertex.feature == 3)
				rotate_point_around_axis(vertex, angle1, pPlate->HuoQuLine[1]);
			coord_trans(vertex, assemble_ucs, TRUE);
			if (vertex.feature == 12)
			{
				if (bFirst)
				{
					pt1 = vertex;
					TransPtToCadUcs(pt1, ucs, scale);
					ptHuoqu1 = vertex + assemble_ucs.axis_z*pPlate->GetThick();
					TransPtToCadUcs(ptHuoqu1, ucs, scale);
					bFirst = FALSE;
				}
				else
				{
					pt2 = vertex;
					TransPtToCadUcs(pt2, ucs, scale);
					ptHuoqu2 = vertex + assemble_ucs.axis_z*pPlate->GetThick();
					TransPtToCadUcs(ptHuoqu2, ucs, scale);
				}
			}
			if (vertex.feature == 13)
			{
				if (bFirst1)
				{
					pt3 = vertex;
					TransPtToCadUcs(pt3, ucs, scale);
					ptHuoqu3 = vertex + assemble_ucs.axis_z*pPlate->GetThick();
					TransPtToCadUcs(ptHuoqu3, ucs, scale);
					bFirst1 = FALSE;
				}
				else
				{
					pt4 = vertex;
					TransPtToCadUcs(pt4, ucs, scale);
					ptHuoqu4 = vertex + assemble_ucs.axis_z*pPlate->GetThick();
					TransPtToCadUcs(ptHuoqu4, ucs, scale);
				}
			}
			if (vertex.feature != 2 && vertex.feature != 3)
			{
				if (i == 0)
				{
					endPt = vertex + assemble_ucs.axis_z*pPlate->GetThick();
					TransPtToCadUcs(endPt, ucs, scale);
				}
				f3dPoint vertexTemp = vertex;
				TransPtToCadUcs(vertexTemp, ucs, scale);
				pt3d[i] = vertexTemp;
				i++;
			}
			if (vertex.feature != 1 && vertex.feature != 3 && vertex.feature != 13)
			{
				if (j == 0)
				{
					endPt1 = vertex + pPlate->HuoQuFaceNorm[0] * pPlate->GetThick();
					TransPtToCadUcs(endPt1, ucs, scale);
				}
				f3dPoint vertexTemp = vertex;
				TransPtToCadUcs(vertexTemp, ucs, scale);
				pt3d1[j] = vertexTemp;
				j++;
			}
			if (vertex.feature != 1 && vertex.feature != 2 && vertex.feature != 12)
			{
				if (k == 1)
				{
					endPt2 = vertex + pPlate->HuoQuFaceNorm[1] * pPlate->GetThick();
					TransPtToCadUcs(endPt2, ucs, scale);
				}
				f3dPoint vertexTemp = vertex;
				TransPtToCadUcs(vertexTemp, ucs, scale);
				pt3d3[k] = vertexTemp;
				k++;
			}
		}
		if (pPlate->m_cFaceN > 2)//���������
		{
			if (pt3d[i - 1] == pt1 || pt3d[i - 1] == pt2 || pt3d[0] == pt1 || pt3d[0] == pt2)
			{
				pt3d[i] = pt4;
				i++;
			}
		}
		pt3d[i] = pt3d[0];
		pt3d[i + 1] = endPt;
		//����ʵ��
		AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
		if (pBlockTableRecord == NULL)
			return NULL;
		AcDb3dSolid *p3dObj = NULL, *p3dObj1 = NULL, *p3dObj2 = NULL, *p3dObj3 = NULL, *p3dObj4 = NULL;
		AcDbObjectId mainObjId, subObj1Id, subObj2Id, subObj3Id, subObj4Id;
		mainObjId = CreateSolidByManner(pBlockTableRecord, pt3d, i, 0, crSolid);
		if (pPlate->m_cFaceN > 2)//���������	
		{
			if (pt3d1[j - 1] == pt1)
			{
				pt3d1[j] = top_point2;
				j++;
			}
		}
		pt3d1[j] = pt3d1[0];
		pt3d1[j + 1] = endPt1;
		subObj1Id = CreateSolidByManner(pBlockTableRecord, pt3d1, j, 0, crSolid);
		//
		f3dPoint pt3d2[6];
		pt3d2[0] = pt1;
		pt3d2[1] = pt2;
		pt3d2[2] = ptHuoqu2;
		pt3d2[3] = ptHuoqu1;
		pt3d2[4] = pt1;
		pt3d2[5] = pt2;
		subObj2Id = CreateSolidByManner(pBlockTableRecord, pt3d2, 4, 1, crSolid, 0, angle);
		if (pPlate->m_cFaceN == 3)
		{
			if (pt3d3[k - 1] == pt3 || pt3d3[0] == pt3)
				pt3d3[k] = top_point3;
			k++;
			pt3d3[k] = pt3d3[1];
			pt3d3[k + 1] = endPt2;
			subObj3Id = CreateSolidByManner(pBlockTableRecord, pt3d3, k, 0, crSolid);
			//
			f3dPoint pt3d4[6];
			pt3d4[0] = pt3;
			pt3d4[1] = pt4;
			pt3d4[2] = ptHuoqu4;
			pt3d4[3] = ptHuoqu3;
			pt3d4[4] = pt3;
			pt3d4[5] = pt4;
			subObj4Id == CreateSolidByManner(pBlockTableRecord, pt3d4, 4, 1, crSolid, 0, angle1);
		}
		pBlockTableRecord->close();
		//����ʵ��ϲ�
		acdbOpenObject(p3dObj, mainObjId, AcDb::kForWrite);
		if (p3dObj == NULL)
			return NULL;
		acdbOpenObject(p3dObj1, subObj1Id, AcDb::kForWrite);
		acdbOpenObject(p3dObj2, subObj2Id, AcDb::kForWrite);
		acdbOpenObject(p3dObj3, subObj3Id, AcDb::kForWrite);
		acdbOpenObject(p3dObj4, subObj4Id, AcDb::kForWrite);
		if (p3dObj1)
			p3dObj->booleanOper(AcDb::kBoolUnite, p3dObj1);
		if (p3dObj2)
			p3dObj->booleanOper(AcDb::kBoolUnite, p3dObj2);
		if (p3dObj3)
			p3dObj->booleanOper(AcDb::kBoolUnite, p3dObj3);
		if (p3dObj4)
			p3dObj->booleanOper(AcDb::kBoolUnite, p3dObj4);
		//
		if (p3dObj1)
			p3dObj1->close();
		if (p3dObj2)
			p3dObj2->close();
		if (p3dObj3)
			p3dObj3->close();
		if (p3dObj4)
			p3dObj4->close();
		p3dObj->close();
		//��ʵ��ͼԪλ�ý��е���
		if (pMatrix)
		{
			acdbOpenObject(p3dObj, mainObjId, AcDb::kForWrite);
			p3dObj->transformBy(*pMatrix);
			p3dObj->close();
		}
		return mainObjId;
	}
}
#ifdef __RAPID_ROUGH_3DMODEL_
//����TID������ά����
void DrawTid3DLine(ITidTowerInstance* pTowerInstance, UCS_STRU ucs, double scale /*= 1*/, BOOL bTurnLeft /*= FALSE*/)
{
	if (pTowerInstance == NULL)
		return;
	AcDbBlockTableRecord *pBlockTableRecord = GetBlockTableRecord();
	if (pBlockTableRecord == NULL)
		return;
	//��ʼ��λ���������
	AcGePoint3d origin(0, 0, 0);
	AcGeVector3d xAxis(-1, 0, 0);
	AcGeVector3d yAxis(0, -1, 0);
	AcGeVector3d zAxis(0, 0, 1);
	AcGeMatrix3d rot_mat;
	rot_mat.setCoordSystem(origin, xAxis, yAxis, zAxis);
	for (ITidAssemblePart* pAssmPart = pTowerInstance->EnumAssemblePartFirst(); pAssmPart; pAssmPart = pTowerInstance->EnumAssemblePartNext())
	{
		ITidPart* pTidPart = pAssmPart->GetPart();
		if (pTidPart->GetPartType() != ITidPart::TYPE_ANGLE)
			continue;
		//��Ӽ���
		TID_COORD3D line_S = pAssmPart->BriefLineStart();
		TID_COORD3D line_E = pAssmPart->BriefLineEnd();
		f3dPoint ptS(line_S.x,line_S.y, line_S.z), ptE(line_E.x, line_E.y, line_E.z);
		TransPtToCadUcs(ptS, &ucs, scale);
		TransPtToCadUcs(ptE, &ucs, scale);
		AcDbLine *pLine = new AcDbLine(AcGePoint3d(ptS.x, ptS.y, ptS.z), AcGePoint3d(ptE.x, ptE.y, ptE.z));
		if (bTurnLeft)
			pLine->transformBy(rot_mat);
		AcDbObjectId LineId;//�����ʶ��
		pBlockTableRecord->appendAcDbEntity(LineId, pLine);
		pLine->createExtensionDictionary();
		pLine->close();//���ùرն���ĳ�Ա����
		//
		AcDbObjectId dictObjId, xrecObjId;
		dictObjId = pLine->extensionDictionary();
		AcDbDictionary *pDict = NULL;
		acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
		AcDbXrecord *pXrec = new AcDbXrecord;
#ifdef _ARX_2007
		pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
		pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
		pDict->close();
		struct resbuf *head;
		head = acutBuildList(AcDb::kDxfInt32, pTidPart->GetSerialId(), 0);
		pXrec->setFromRbChain(*head);
		acutRelRb(head);
		pXrec->close();
	}
	pBlockTableRecord->close();
}
UCS_STRU GetAssembleUcs(TID_CS cs)
{
	UCS_STRU ucs;
	ucs.origin.Set(cs.origin.x, cs.origin.y, cs.origin.z);
	ucs.axis_x.Set(cs.axisX.x, cs.axisX.y, cs.axisX.z);
	ucs.axis_y.Set(cs.axisY.x, cs.axisY.y, cs.axisY.z);
	ucs.axis_z.Set(cs.axisZ.x, cs.axisZ.y, cs.axisZ.z);
	return ucs;
}
void DrawTid3DSolid(ITidTowerInstance* pTowerInstance, UCS_STRU ucs, BOOL bPrompt, double scale /*= 1*/,
						BYTE ciModelLevel /*= 3*/,BOOL bTurnLeft /*= FALSE*/)
{
	if (pTowerInstance == NULL)
		return;
	//��ʼ��λ���������
	AcGePoint3d origin(0, 0, 0);
	AcGeVector3d xAxis(-1, 0, 0);
	AcGeVector3d yAxis(0, -1, 0);
	AcGeVector3d zAxis(0, 0, 1);
	AcGeMatrix3d rot_mat;
	rot_mat.setCoordSystem(origin, xAxis, yAxis, zAxis);
	AcGeMatrix3d* pRotMat = bTurnLeft ? (&rot_mat) : NULL;
	//
	int nPartNum = 0, nBoltNum = 0, nCurProcess = 0;
	for (ITidAssemblePart* pAssmPart = pTowerInstance->EnumAssemblePartFirst(); pAssmPart; pAssmPart = pTowerInstance->EnumAssemblePartNext())
		nPartNum++;
	for (ITidAssembleBolt* pAssmBolt = pTowerInstance->EnumAssembleBoltFirst(); pAssmBolt; pAssmBolt = pTowerInstance->EnumAssembleBoltNext())
		nBoltNum++;
	int nNum = nPartNum + nBoltNum;
	if(bPrompt)
		DisplayProcess(0, "TID��άʵ�����ɽ���");
	for (ITidAssemblePart* pAssmPart = pTowerInstance->EnumAssemblePartFirst(); pAssmPart; pAssmPart = pTowerInstance->EnumAssemblePartNext())
	{
		nCurProcess++;
		if(bPrompt)
			DisplayProcess(ftoi(100 * nCurProcess / nPartNum), "TID��άʵ�����ɽ���");
		//
		ITidPart* pTidPart = pAssmPart->GetPart();
		UCS_STRU assemble_ucs = GetAssembleUcs(pAssmPart->GetAcs());
		UINT buf_size = pTidPart->GetProcessBuffBytes(NULL);
		CHAR_ARRAY buf_str(buf_size);
		pTidPart->GetProcessBuffBytes(buf_str, buf_size);
		CBuffer buffer(buf_str, buf_size);
		if (pTidPart->GetPartType() == ITidPart::TYPE_ANGLE)
		{
			CProcessAngle processJg;
			processJg.m_fWidth = (float)pTidPart->GetWidth();
			processJg.m_fThick = (float)pTidPart->GetThick();
			processJg.m_wLength = pTidPart->GetLength();
			processJg.ReadProcessBuffer(buffer, 1000001, false, CProcessPart::TID_BUFFER);
			DrawSolidAngle(&processJg, assemble_ucs, &ucs, scale,NULL,pRotMat);
		}
		else if (pTidPart->GetPartType() == ITidPart::TYPE_PLATE)
		{
			CProcessPlate processPlate;
			processPlate.m_fThick = (float)pTidPart->GetThick();
			processPlate.ReadProcessBuffer(buffer, 1000001, false, CProcessPart::TID_BUFFER);
			if(ciModelLevel > 1)
				DrawSolidPlate(&processPlate, assemble_ucs, &ucs, scale, NULL, pRotMat);
		}
	}
	if (ciModelLevel == 3)
	{
		for (ITidAssembleBolt* pAssmBolt = pTowerInstance->EnumAssembleBoltFirst(); pAssmBolt; pAssmBolt = pTowerInstance->EnumAssembleBoltNext())
		{
			nCurProcess++;
			if(bPrompt)
				DisplayProcess(ftoi(100 * nCurProcess / nPartNum), "TID��άʵ�����ɽ���");
			//
			UCS_STRU assemble_ucs = GetAssembleUcs(pAssmBolt->GetAcs());
			IBoltSizeSpec* pBolt = pAssmBolt->GetTidBolt();
			LS_HOLE_INFO hole_info;
			hole_info.key = pBolt->GetId();
			hole_info.bolt_d = pBolt->GetDiameter();
			hole_info.bolt_L = pBolt->GetLenValid();
			hole_info.bolt_L0 = pBolt->GetLenNoneThread();
			DrawBoltSolid(hole_info, assemble_ucs, &ucs, scale, NULL, pRotMat);
		}
	}
	if(bPrompt)
		DisplayProcess(100, "TID��άʵ�����ɽ���");
}
#endif