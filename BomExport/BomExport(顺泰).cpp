// BomExport.cpp : 定义 DLL 的初始化例程。
// 山东华安
//TODO 没有对焊法兰

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
	return 7;
}
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"

typedef LIST_NODE<BOMPROFILE_VERTEX>* VERTEX_PTR;
//
static const BYTE WELD_LEN	=1;		//焊缝长度
static const BYTE EDGE_NUM	=2;		//边数
static const BYTE PERIMETER =3;		//周长
static const BYTE FOOT_PLATE=4;		//塔脚板
static const BYTE RECT_PLATE=5;		//矩形板

static int CompareSegiFunc(const SUPERLIST_NODE<BOMPART>& item1,const SUPERLIST_NODE<BOMPART>& item2)
{
	BOMPART* pBomPart1= (BOMPART*)item1.pDataObj;
	BOMPART* pBomPart2= (BOMPART*)item2.pDataObj;
	SEGI seg1(pBomPart1->iSeg),seg2(pBomPart2->iSeg);
	if(seg1.iSeg>seg2.iSeg)
		return 1;
	else if(seg1.iSeg<seg2.iSeg)
		return -1;
	else	//if(seg1.iSeg==seg2.iSeg)
		return ComparePartNoString(pBomPart1->sPartNo,pBomPart2->sPartNo);
}
static void GetAngleSimpleSpec(char *simple_spec,double x_wing_wide,double y_wing_wide,double wide_thick)
{
	char sSpec[200]="";
	char sWide[100]="",sThick[100]="";
	sprintf(sWide,"%s",(char*)CXhChar16(x_wing_wide));
	sprintf(sThick,"%s",(char*)CXhChar16(wide_thick));
	if(IsUnequalAngle(x_wing_wide,y_wing_wide,wide_thick))
	{
		char sWideY[100]="";
		sprintf(sWideY,"%s",(char*)CXhChar16(y_wing_wide));
		sprintf(sSpec,"%sx%s*%s",sWide,sWideY,sThick);
	}
	else 
		sprintf(sSpec,"%s*%s",sWide,sThick);
	if(simple_spec)
		strcpy(simple_spec,sSpec);
}
static CXhChar50 GetSpecStr(BOMPART* pBomPart)
{
	CXhChar50 str;
	char sSpec[30]="";
	if(pBomPart->cPartType==BOMPART::ANGLE)	//标注角钢规格∠
	{
		GetAngleSimpleSpec(sSpec,pBomPart->wide,pBomPart->wingWideY,pBomPart->thick);
		str.Printf("∠%s",sSpec);
	}
	else if(pBomPart->cPartType==BOMPART::PLATE&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_WATER
		&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_FL&&pBomPart->siSubType!=BOMPART::SUB_TYPE_FOOT_FL)
		str.Printf("-%.f",pBomPart->thick);
	else 
		str.Printf("%s",(char*)pBomPart->sSpec);
	return str;
}
static CXhChar50 GetProcessNotes(BOMPART* pBomPart)
{
	CXhChar50 sNotes;
	if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		if(pBomAngle->bHasFootNail)
			sNotes.Copy("带脚钉");
	}
	else if(pBomPart->cPartType==BOMPART::TUBE)
	{
		PART_TUBE* pBomTube=(PART_TUBE*)pBomPart;
		if(pBomTube->bHasFootNail)
			sNotes.Copy("带脚钉");
	}
	return sNotes;
}
static CXhChar16 GetPlateData(BOMPART* pBomPart,BYTE biDataType)
{
	CXhChar16 sValue;
	if(biDataType==WELD_LEN&&pBomPart->bWeldPart&&pBomPart->cPartType==BOMPART::SLOT)
	{	//槽钢脚钉的焊接长度
		sValue.Printf("%.1f",pBomPart->length*2);
		return sValue;
	}
	if(pBomPart->cPartType!=BOMPART::PLATE)
		return sValue; 
	PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
	if(pPlate->fWeldEdgeLen>0 && biDataType==WELD_LEN)
		sValue.Printf("%.1f",pPlate->fWeldEdgeLen);
	else if(biDataType==EDGE_NUM)
		sValue.Printf("%d",pPlate->GetEdgeCount());
	else if(pPlate->fPerimeter>0&&biDataType==PERIMETER)
		sValue.Printf("%.1f",pPlate->fPerimeter);
	else if(pPlate->bTowerFootPlate&&biDataType==FOOT_PLATE)
		sValue.Copy("1");
	else if(pPlate->IsRect()&&biDataType==RECT_PLATE)
		sValue.Copy("1");
	return sValue;
}
static CXhChar100 GetLsHoleStr(BOMPART* pBomPart, BOOL bIncUnit=TRUE)
{
	CXhChar100 sHoleStr="";
	if(pBomPart->nMSumLs<=0)
		return "";
	CHashStrList<int> hashLsNumByD;
	for(BOMBOLT_RECORD* pBomboltRecoed=pBomPart->m_arrBoltRecs.GetFirst();pBomboltRecoed;pBomboltRecoed=pBomPart->m_arrBoltRecs.GetNext())
	{
		int* pNum=hashLsNumByD.GetValue(CXhChar50(pBomboltRecoed->d+pBomboltRecoed->hole_d_increment));
		if(pNum==NULL)
			hashLsNumByD.SetValue(CXhChar16(pBomboltRecoed->d+pBomboltRecoed->hole_d_increment),1);
		else
			(*pNum)++;
	}
	CXhChar50 sCurHoleStr="";
	for(int* pNum=hashLsNumByD.GetFirst();pNum;pNum=hashLsNumByD.GetNext())
	{
		if(sCurHoleStr.GetLength()>0)
			sCurHoleStr.Append(",");
		if(bIncUnit)
			sCurHoleStr.Append(CXhChar16("%smm*%d",(char*)hashLsNumByD.GetCursorKey(),*pNum));
		else
			sCurHoleStr.Append(CXhChar16("%s*%d",(char*)hashLsNumByD.GetCursorKey(),*pNum));
	}
	if(sCurHoleStr.GetLength()>0)
		sHoleStr=sCurHoleStr;
	return sHoleStr;
}
#define      DEGTORAD_COEF    57.2957795130823208768
CXhChar100 GetProcessParam(BOMPART* pBomPart)
{
	int nNum=0;
	CXhChar100 sProcessParam="";
	if(pBomPart->bWeldPart)
		sProcessParam.Append("焊接",',');
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
		int nHuoquCount=pPlate->m_cFaceN-1;
		if(nHuoquCount>0)
		{
			double fHuoquAngle1=fabs(pPlate->huoQuAngleArr[0]);
			double fHuoquAngle2=fabs(pPlate->huoQuAngleArr[1]);
			CXhChar50 sAngle("制弯%d-%0.1f",nHuoquCount,fHuoquAngle1>fHuoquAngle2?fHuoquAngle1:fHuoquAngle2);
			sProcessParam.Append(sAngle,',');
		}
		for(VERTEX_PTR pVertex=pPlate->listVertex.EnumFirst();pVertex;pVertex=pPlate->listVertex.EnumNext())
		{
			if(pVertex->data.m_bRollEdge)
			{
				sProcessParam.Append("卷边",',');
				break;
			}
		}
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		if(pBomAngle->bCutAngle)
			sProcessParam.Append(CXhChar16("切角%d",GetCutAngleSwordNum(pBomAngle)),',');
		//制弯
		int nHuoquType=pBomAngle->GetHuoquType();
		long nHuoquLineCount=pBomPart->GetHuoquLineCount();
		CXhChar16 zhiwanArr[4]={"内曲面","外曲面","内曲筋","外曲筋"};
		if(nHuoquLineCount>0&&nHuoquType>=1&&nHuoquType<=4)
		{
			BOMPROFILE_VERTEX::COORD3D prev_len_vec;
			prev_len_vec.z=1;
			double fMinCosa=1;
			pBomAngle->listPolyAngle.EnumFirst();
			for(LIST_NODE<PART_ANGLE::POLY_ANGLE_INFO> *pNode=pBomAngle->listPolyAngle.EnumNext();pNode;pNode=pBomAngle->listPolyAngle.EnumNext())
			{
				BOMPROFILE_VERTEX::COORD3D len_vec(pNode->data.len_vec);
				//NormalizedVector(len_vec);
				double cosa=prev_len_vec.x*len_vec.x+prev_len_vec.y*len_vec.y+prev_len_vec.z*len_vec.z;
				if(cosa<fMinCosa)
					fMinCosa=cosa;
				prev_len_vec.x=len_vec.x;
				prev_len_vec.y=len_vec.y;
				prev_len_vec.z=len_vec.z;
			}
			double fAngle=acos(fMinCosa)*DEGTORAD_COEF;
			sProcessParam.Append(CXhChar16("%s %d-%0.1f",(char*)zhiwanArr[nHuoquType-1],nHuoquLineCount,fAngle),',');
		}
		//压扁
		if(pBomAngle->nPushFlat>0)		//是否压扁
		{
			int nPushFlat=0;
			if(pBomAngle->nPushFlat&0x01)
				nPushFlat++;
			if(pBomAngle->nPushFlat&0x02)
				nPushFlat++;
			if(pBomAngle->nPushFlat&0x04)
				nPushFlat+=2;
			sProcessParam.Append(CXhChar16("压扁 %d",nPushFlat),',');
		}
		//开合角
		if(pBomAngle->bHeJiao||pBomAngle->bKaiJiao)
		{
			int nCount=0;
			for(LIST_NODE<KAI_HE_JIAO> *pNode=pBomAngle->listKaihe.EnumFirst();pNode;pNode=pBomAngle->listKaihe.EnumNext())
			{
				short siLen=pNode->data.endLength+pNode->data.startLength;
				short siCount=siLen/500;
				if(siLen%500>0)
					siCount+=1;
				nCount+=siCount;
			}
			if(pBomAngle->bKaiJiao)
				sProcessParam.Append(CXhChar16("开角 %d-%0.1f",nCount,pBomAngle->wing_angle-90),',');
			else 
				sProcessParam.Append(CXhChar16("合角 %d-%0.1f",nCount,90-pBomAngle->wing_angle),',');
		}
		if(pBomAngle->bCutBer)			//是否需要铲背
			sProcessParam.Append("铲背",',');
		if(pBomAngle->bCutRoot)		//是否需要清根
			sProcessParam.Append("清根",',');
	}
	else if(pBomPart->cPartType==BOMPART::TUBE)
	{
		PART_TUBE *pTube=(PART_TUBE*)pBomPart;
		BYTE typeS=pTube->startProcess.type,typeE=pTube->endProcess.type;
		if(typeS==typeE)
		{
			if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_CSLOT)
				sProcessParam.Append("一型开口槽2",',');
			else if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_USLOT)
				sProcessParam.Append("U型开口槽2",',');
			else if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TSLOT)
				sProcessParam.Append("T型开口槽2",',');
			else if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_XSLOT)
				sProcessParam.Append("X型开口槽2",',');
			else if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TRANSECT||
					typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_PLANECUT)
				sProcessParam.Append("相贯2",',');
		}
		else
		{
			if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_CSLOT||typeE==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_CSLOT)
				sProcessParam.Append("一型开口槽1",',');
			if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_USLOT||typeE==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_USLOT)
				sProcessParam.Append("U型开口槽1",',');
			if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TSLOT||typeE==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TSLOT)
				sProcessParam.Append("T型开口槽1",',');
			if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_XSLOT||typeE==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_XSLOT)
				sProcessParam.Append("X型开口槽1",',');
			if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TRANSECT)
			{
				if(typeE==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_PLANECUT)
					sProcessParam.Append("相贯2",',');
				else
					sProcessParam.Append("相贯1",',');
			}
			if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_PLANECUT)
			{
				if(typeE==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TRANSECT)
					sProcessParam.Append("相贯2",',');
				else
					sProcessParam.Append("相贯1",',');
			}
		}
	}
	return sProcessParam;
}

TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf,int buf_len,const char* file_name,DWORD dwFlag)
{
	CModelBOM bom;
	CBuffer buffer(buf_len);
	if(buf_len>0)
	{
		buffer.AttachMemory(data_buf,buf_len);
		bom.FromBuffer(buffer,GetSupportDataBufferVersion());
	}
	//计算列数
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
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short) 1));
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("放样原始材料表");
	//添加标题列名
	CStringArray str_arr;
	str_arr.SetSize(20);
	str_arr[0]="段号";str_arr[1]="部件号";str_arr[2]="材质";str_arr[3]="材料";
	str_arr[4]="规格";str_arr[5]="长度";str_arr[6]="宽度";str_arr[7]="单基数量";										
	str_arr[8]="工艺参数";str_arr[9]="备注";str_arr[10]="特殊件号";str_arr[11]="各孔径孔数";								
	str_arr[12]="有无挂线孔/施工孔";str_arr[13]="是否塔脚底板";str_arr[14]="有无条孔";str_arr[15]="板件是否矩形";
	str_arr[16]="焊接边长";str_arr[17]="边数";str_arr[18]="板周长mm";str_arr[19]="构件镀锌面积";
	double col_arr[20]={7.0,7.0,7.0,8.0,7.5,7.0,7.0,7.0,20.0,10.0,7.5,17.0,8.0,6.0,4.0,6.0,7.5,4.5,7.5,11.0};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1","T1",COleVariant((long)3));	//列标题水平居中
	//添加实时数据
	CVariant2dArray map(bom.m_nPartsN,20);
	int index=0;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));	//段号
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//部件号
		CXhChar16 sMat=QuerySteelMatMark(pBomPart->cMaterial);	//材质
		map.SetValueAt(index,2,COleVariant(sMat));
		CExcelOper::SetRangeBackColor(excel_sheet,GetMatColorIndex(sMat),CXhChar16("B%d",index+2),CXhChar16("C%d",index+2));
		//材料
		CXhChar16 sSumTypeName=pBomPart->GetPartSubTypeName();
		if(pBomPart->cPartType==BOMPART::PLATE&&sSumTypeName.GetLength()>0)
			map.SetValueAt(index,3,COleVariant(sSumTypeName));
		else
			map.SetValueAt(index,3,COleVariant(pBomPart->GetPartTypeName()));
		map.SetValueAt(index,4,COleVariant(GetSpecStr(pBomPart)));					//规格
		map.SetValueAt(index,5,COleVariant(CXhChar16("%d",ftol(pBomPart->length))));	//长度
		if(pBomPart->cPartType==BOMPART::PLATE||pBomPart->cPartType==BOMPART::TUBE)
			map.SetValueAt(index,6,COleVariant(CXhChar16("%d",ftol(pBomPart->wide))));	//宽度
		map.SetValueAt(index,7,COleVariant(pBomPart->GetPartNum()));	//单基数量
		map.SetValueAt(index,8,COleVariant(GetProcessParam(pBomPart)));	//工艺参数
		map.SetValueAt(index,9,COleVariant(GetProcessNotes(pBomPart)));	//备注
		map.SetValueAt(index,10,COleVariant(""));	//特殊件号
		map.SetValueAt(index,11,COleVariant(GetLsHoleStr(pBomPart,FALSE)));	//各孔径孔数
		map.SetValueAt(index,12,pBomPart->IsHasSpecFuncHole()?COleVariant(long(1)):COleVariant(""));//有无挂线孔/施工孔
		map.SetValueAt(index,13,COleVariant(GetPlateData(pBomPart,FOOT_PLATE)));	//是否塔脚底板
		map.SetValueAt(index,14,pBomPart->IsHasLinearHole()?COleVariant(long(1)):COleVariant(""));	//有无条孔
		map.SetValueAt(index,15,COleVariant(GetPlateData(pBomPart,RECT_PLATE)));	//板件是否矩形
		map.SetValueAt(index,16,COleVariant(GetPlateData(pBomPart,WELD_LEN)));	//焊接边长
		map.SetValueAt(index,17,COleVariant(GetPlateData(pBomPart,EDGE_NUM)));	//边数
		map.SetValueAt(index,18,COleVariant(GetPlateData(pBomPart,PERIMETER)));	//板周长mm
		map.SetValueAt(index,19,COleVariant(CXhChar16("%0.3f",pBomPart->CalZincCoatedArea())));//镀锌面积
		index++;
	}
	CXhChar16 cell_end("T%d",index+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);	//设置表格内容
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A2",cell_end,COleVariant((long)2));//水平居左
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"M2",CXhChar16("P%d",index+1),COleVariant((long)3));//水平居中
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));	//设置边框和字体大小
	excel_sheet.ReleaseDispatch();
	excel_sheets.ReleaseDispatch();
	return 1;
}