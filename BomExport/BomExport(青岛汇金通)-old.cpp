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
		sprintf(sSpec,"%s*%s*%s",sWide,sWideY,sThick);
	}
	else 
		sprintf(sSpec,"%s*%s",sWide,sThick);
	if(simple_spec)
		strcpy(simple_spec,sSpec);
}
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
static int GetProcessNotes(BOMPART* pBomPart,char *notes,bool bIncKaiHeAngle=FALSE)
{
	char ss[300]="";
	long nHuoquLineCount=pBomPart->GetHuoquLineCount();
	if(pBomPart->cPartType==BOMPART::PLATE)
		nHuoquLineCount=((PART_PLATE*)pBomPart)->m_cFaceN-1;
	if(nHuoquLineCount>0)	//是否需要制弯
		sprintf(ss,"%s 制弯",ss);
	if(pBomPart->bWeldPart&&pBomPart->cPartType!=BOMPART::PLATE)
		sprintf(ss,"%s 焊接",ss);
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE *pPlate=(PART_PLATE*)pBomPart;
		if(pPlate->bWeldPart&&pPlate->bNeedFillet)		//是否需要焊接
			sprintf(ss,"%s 焊于		",ss);
		else if(pPlate->bWeldPart)
		if(pPlate->bWeldPart)		//是否需要焊接
			sprintf(ss,"%s 焊接",ss);
		if(pPlate->bNeedFillet)		//需要坡口
			sprintf(ss,"%s 坡口",ss);
		for(LIST_NODE<BOMPROFILE_VERTEX> *pNode=pPlate->listVertex.EnumFirst();pNode;pNode=pPlate->listVertex.EnumNext())
		{
			if(pNode->data.m_bRollEdge)
			{
				sprintf(ss,"%s 卷边%d",ss,abs(pNode->data.manu_space));
				sprintf(ss,"%s 制弯",ss);
				break;
			}
		}
	}
	else if(pBomPart->cPartType==BOMPART::TUBE)
	{
		PART_TUBE *pTube=(PART_TUBE*)pBomPart;
		if( pTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD&&
			pTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			sprintf(ss,"%s 两端坡口",ss);
		else if(pTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD||
			pTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			sprintf(ss,"%s 一端坡口",ss);
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		if(pBomAngle->bCutAngle)		//是否切角
			sprintf(ss,"%s 切角",ss);
		if(pBomAngle->bCutBer)			//是否需要铲背
			sprintf(ss,"%s 铲背",ss);
		if(pBomAngle->bCutRoot)		//是否需要清根
			sprintf(ss,"%s 清根",ss);
		if(pBomAngle->bKaiJiao&&GetKaiHeJiaoN(pBomAngle)>0)		//是否需要开角
		{
			if(bIncKaiHeAngle)	//包括开角度数 wht 11-05-24
				sprintf(ss,"%s 开角(%.1f°)",ss,pBomAngle->wing_angle-90);
			else 
				sprintf(ss,"%s 开角",ss);
		}
		if(pBomAngle->bHeJiao&&GetKaiHeJiaoN(pBomAngle)>0)			//是否需要合角
		{
			if(bIncKaiHeAngle)	//包括合角度数 wht 11-05-24
				sprintf(ss,"%s 合角(%.1f°)",ss,90-pBomAngle->wing_angle);
			else 
				sprintf(ss,"%s 合角",ss);
		}
		if(pBomAngle->nPushFlat>0)		//是否压扁
		{
			if(pBomAngle->nPushFlat==0x02)
				sprintf(ss,"%s 中间压扁",ss);
			else
				sprintf(ss,"%s 压扁",ss);
		}
		if(((PART_ANGLE*)pBomPart)->bHasFootNail)
			sprintf(ss,"%s 带脚钉",ss);
	}
	if(notes)
		strcpy(notes,ss);
	return strlen(ss);
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
	if(bom.m_nBoltsN<=0)
		return 0;
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
	//if(bOrgMat)
		excel_sheet.SetName("放样原始材料表");
	//else
	//	excel_sheet.SetName("铁塔部件明细表");
	//第一行数据项标题行，需要格式输出
	CStringArray str_arr;
	str_arr.SetSize(29);
	str_arr[0]="段号";
	str_arr[1]="部件号";
	str_arr[2]="材质";
	str_arr[3]="材料";
	str_arr[4]="规格";
	str_arr[5]="长度";
	str_arr[6]="宽度";
	str_arr[7]="厚度";
	str_arr[8]="单件数量";
	str_arr[9]="单件重量";	//新增 10-10-12
	str_arr[10]="单件孔数";
	str_arr[11]="备注";
	str_arr[12]="电焊";
	str_arr[13]="火曲";
	str_arr[14]="切角";
	str_arr[15]="铲背";
	str_arr[16]="清根";
	str_arr[17]="打扁";
	str_arr[18]="开合角";
	str_arr[19]="钻孔";
	str_arr[20]="坡口";
	str_arr[21]="割相贯线";
	str_arr[22]="开槽形式";
	str_arr[23]="类型";
	str_arr[24]="边数";
	str_arr[25]="周长";
	str_arr[26]="表面积";
	str_arr[27]="各孔径孔数";
	str_arr[28]="焊接边mm";//新增 10-10-12
	double col_arr[29]={3.0,7.5,3.5,4.5,8.5,
		5.5,4.5,3.0,5.0,5.0,
		5.0,7.5,2.5,2.5,2.5,
		2.5,2.5,2.5,4.0,2.5,
		2.5,4.0,4.0,4.5,2.5,
		3.0,4.0,8.5,5.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	CVariant2dArray map(bom.m_nPartsN+1,29);
	int index =0, i=0;
	for(SUPERLIST_NODE<BOMPART> *pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		i++;
		BOMPART* pBomPart=(BOMPART* )pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));	//段号
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));	//编号
		map.SetValueAt(index,2,COleVariant(QuerySteelMatMark(pBomPart->cMaterial)));//材质
		//材料
		if(pBomPart->cPartType==BOMPART::PLATE)	//钢板
		{
			if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FL)
				map.SetValueAt(index,3,COleVariant("法兰"));
			else if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_WATER)
				map.SetValueAt(index,3,COleVariant("遮水板"));
			else if(pBomPart->siSubType==BOMPART::SUB_TYPE_FOOT_FL)
				map.SetValueAt(index,3,COleVariant("底法兰"));
			else 
				map.SetValueAt(index,3,COleVariant("钢板"));
		}
		else if(pBomPart->cPartType==BOMPART::ANGLE)//角钢
			map.SetValueAt(index,3,COleVariant("角钢"));
		else if(pBomPart->cPartType==BOMPART::TUBE)//钢管
			map.SetValueAt(index,3,COleVariant("钢管"));
		else if(pBomPart->cPartType==BOMPART::SLOT)//槽钢
			map.SetValueAt(index,3,COleVariant("槽钢"));
		else if(pBomPart->cPartType==BOMPART::FLAT)//扁钢
			map.SetValueAt(index,3,COleVariant("扁钢"));
		//规格
		CXhChar100 str;
		char sSpec[30]="";
		if(pBomPart->cPartType==BOMPART::ANGLE)	//标注角钢规格∠
		{
			GetAngleSimpleSpec(sSpec,pBomPart->wide,pBomPart->wingWideY,pBomPart->thick);
			str.Printf("L%s",sSpec);
		}
		else if(pBomPart->cPartType==BOMPART::PLATE&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_WATER
			&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_FL&&pBomPart->siSubType!=BOMPART::SUB_TYPE_FOOT_FL)
			str.Printf("-%.f",pBomPart->thick);
		else 
			str.Printf("%s",(char*)pBomPart->sSpec);
		map.SetValueAt(index,4,COleVariant(str));
		if(bom.m_iPlankWeightStatType==2&&pBomPart->cPartType==BOMPART::PLATE)	//长安提出钢板宽度应根据<净面积/长度>计算所得 wjh-2014.4.09
			map.SetValueAt(index,5,COleVariant(ftol(((PART_PLATE*)pBomPart)->real_area/pBomPart->wide)));		//宽度
		else
			map.SetValueAt(index,5,COleVariant(ftol(pBomPart->length)));	//长度
		map.SetValueAt(index,6,COleVariant(ftol(pBomPart->wide)));		//宽度
		map.SetValueAt(index,7,COleVariant(ftol(pBomPart->thick)));		//厚度
		map.SetValueAt(index,8,COleVariant((long)(pBomPart->GetPartNum())));	//单基数量
		double weight=0;
		if(bom.m_iPlankWeightStatType<=1&&pBomPart->cPartType==BOMPART::PLATE)	//
		{
			PART_PLATE *pPlate=(PART_PLATE*)pBomPart;
			double fMinWeight=CalSteelPlankWei(pPlate->min_area*pPlate->thick);
			weight=ftol(fMinWeight*100)/100.0;
		}
		else
			weight=ftol(pBomPart->fPieceWeight*100)/100.0;
		map.SetValueAt(index,9,COleVariant(CXhChar16("%.2f",weight)));	//单件重量
		map.SetValueAt(index,10,COleVariant((long)pBomPart->nMSumLs));	//单件孔数
		//TODO:备注
		CXhChar100 sNotes;
		GetProcessNotes(pBomPart,sNotes,FALSE);
		sNotes.Append(pBomPart->sNotes);
		map.SetValueAt(index,11,COleVariant(sNotes));			//备注
		if(pBomPart->bWeldPart)		//是否电焊件
			map.SetValueAt(index,12,COleVariant("1"));
		long nHuoquLineCount=pBomPart->GetHuoquLineCount();
		if(nHuoquLineCount>0)		//火曲
			map.SetValueAt(index,13,COleVariant("1"));
		
		if (pBomPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE *pBomAnge = (PART_ANGLE *)pBomPart;
			if(pBomAnge->bCutAngle)		//切角
				map.SetValueAt(index,14,COleVariant("1"));
			if(pBomAnge->bCutBer)		//铲背
				map.SetValueAt(index,15,COleVariant("1"));
			if(pBomAnge->bCutRoot)		//清根
				map.SetValueAt(index,16,COleVariant("1"));
			if(pBomAnge->nPushFlat>0)	//1 表示一端压扁 2 表示两端压扁 
				map.SetValueAt(index,17,COleVariant((long)pBomAnge->nPushFlat));
			if(pBomAnge->bKaiJiao)			//开角
				map.SetValueAt(index,18,COleVariant("1"));
			else if(pBomAnge->bHeJiao)		//合角
				map.SetValueAt(index,18,COleVariant("2"));
			if(pBomAnge->thick>=14)			//钻孔
				map.SetValueAt(index,19,COleVariant("1"));
		}
		if (pBomPart->cPartType==BOMPART::PLATE)
		{
			PART_PLATE* pBomPlate = (PART_PLATE *)pBomPart;
			if(pBomPlate->bNeedFillet)	//坡口
				map.SetValueAt(index,20,COleVariant("1"));
		}
		//类型
		char sTypeName[100]="";
		if(pBomPart->cPartType==BOMPART::TUBE)
		{
			PART_TUBE *pBomTube=(PART_TUBE*)pBomPart;
			if(pBomPart->bWeldPart)
				strcpy(sTypeName,"支管");
			else 
				strcpy(sTypeName,"主管");
			//割相贯线
			long nTransect=0;
			if( pBomTube->startProcess.type==1||pBomTube->startProcess.type==2)
				nTransect++;
			if(pBomTube->endProcess.type==1||pBomTube->endProcess.type==2)
				nTransect++;
			if(nTransect>0)
				map.SetValueAt(index,21,COleVariant("1"));	//割相贯线
			//开槽类型
			long nType=0;
			if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_CSLOT)
				nType=11;	//支管 两头C型插板
			else if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_USLOT)
				nType=22;	//支管 两头U型插板
			else if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TSLOT)
				nType=33;	//支管 两头T型插板
			else if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_XSLOT)
				nType=44;	//支管 两头X型插板
			if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_CSLOT)
				nType=11;	//支管 两头C型插板
			else if(pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_USLOT)
				nType=22;	//支管 两头U型插板
			else if(pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TSLOT)
				nType=33;	//支管 两头T型插板
			else if(pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_XSLOT)
				nType=44;	//支管 两头X型插板
			if(nType>0)
				map.SetValueAt(index,22,COleVariant(nType));
		}
		else if(pBomPart->cPartType==BOMPART::PLATE)
		{
			if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_WATER)
				strcpy(sTypeName,"遮水板");
			else if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FL)
				strcpy(sTypeName,"法兰");
			else if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_C
				||pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_U
				||pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_X)
				strcpy(sTypeName,"插板");
			else if(pBomPart->siSubType==BOMPART::SUB_TYPE_FOOT_FL)
				strcpy(sTypeName,"底法兰");
		}
		map.SetValueAt(index,23,COleVariant(sTypeName));
		if(pBomPart->cPartType==BOMPART::PLATE)
		{
			PART_PLATE * pBomPlate = (PART_PLATE *)pBomPart;
			map.SetValueAt(index,24,COleVariant((long)pBomPlate->GetEdgeCount()));			//边数
			map.SetValueAt(index,25,COleVariant(CXhChar16(pBomPlate->fPerimeter)));			//周长
			//面积
			if(bom.m_iPlankWeightStatType==2)
				map.SetValueAt(index,26,COleVariant(CXhChar16(pBomPlate->real_area)));
			else
				map.SetValueAt(index,26,COleVariant(CXhChar16(pBomPlate->min_area)));

		}
		//孔数孔径
		char sHoleStr[500]="";
		if(pBomPart->GetLsHoleStr(sHoleStr,FALSE)>0)
			map.SetValueAt(index,27,COleVariant(sHoleStr));
		if(pBomPart->cPartType==BOMPART::PLATE)
		{	//焊接边mm
			PART_PLATE* pBomPlate = (PART_PLATE *)pBomPart;
			map.SetValueAt(index,28,COleVariant(CXhChar16(pBomPlate->fWeldEdgeLen)));
		}
		index++;
	}
	char cell_end[15]=" ";
	//设置右对齐
	sprintf(cell_end,"E%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"B2",cell_end,COleVariant((long)2));
	//设置表格内容
	sprintf(cell_end,"AC%d",i+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	return 1;
}
