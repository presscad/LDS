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
#include "XhCharString.h"
#include "../../Ent/f_ent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib,"../../Ent/Release/f_ent.lib")

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
	return 8;
}
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"

char* SearchChar(char* srcStr,char ch,bool reverseOrder/*=false*/)
{
	if(!reverseOrder)
		return strchr(srcStr,ch);
	else
	{
		int len=strlen(srcStr);
		for(int i=len-1;i>=0;i--)
		{
			if(srcStr[i]==ch)
				return &srcStr[i];
		}
	}
	return NULL;
}

const int COL_COUNT=9;
TOWER_BOM_EXPORTS_API int GetExcelFormat(int* colIndexArr,int *startRowIndex,char *titleStr)
{
	if(colIndexArr==NULL)
		return COL_COUNT;
	else
	{
		if(startRowIndex!=NULL)
			*startRowIndex=2;
		colIndexArr[0]=1;	//件号
		colIndexArr[1]=3;	//材质
		colIndexArr[2]=5;	//长度
		colIndexArr[3]=4;	//规格
		colIndexArr[4]=8;	//件数
		colIndexArr[5]=9;	//单重
		colIndexArr[6]=11;	//备注
		colIndexArr[7]=7;	//宽度
		colIndexArr[8]=0;	//类型
		CXhChar500 sTitle("段名|部件号|材料名称|材质|规格|长(mm)|宽(mm)|厚(mm)|单段数量|单件重量(kg)|总重kg|备注|单件孔数|类型|边数|钻孔|有扩孔|电焊|电焊长度mm|火曲|A面火曲点数|B面火曲点数|A面火曲度数|B面火曲度数|A面火曲类型|B面火曲类型|火曲后弯曲长度|切角|左端头切角刀数|右端头切角刀数|铲背|清根|打扁|打扁类型|开/合角(0/1/2)|开合角点数|开合角长度|割豁|含弧|开槽形式|开槽总数|开槽长度|坡口|坡口米数(米)|坡口角度|打坡口端数|相贯线|相贯形式|相贯线切割长度|A面芯距数|B面芯距数|A面孔经数|B面孔经数|拉长孔数|套对孔(步)数|挂线孔|各孔孔径数|钻孔孔径数|扩孔孔径数|板件是否矩形|周长|面积|");
		if(titleStr!=NULL)
			strcpy(titleStr,sTitle);
		return 9;
	}
}

//////////////////////////////////////////////////////////////////////////
//构件处理
CXhChar16 _QuerySteelMatMark(char cMat)
{
	CXhChar16 sMatMark;
	if('H'==cMat)
		sMatMark.Copy("Q345");
	else if('G'==cMat)
		sMatMark.Copy("Q390");
	else if('P'==cMat)
		sMatMark.Copy("Q420");
	else if('T'==cMat)
		sMatMark.Copy("Q460");
	else if('S'==cMat)
		sMatMark.Copy("Q235");
	return sMatMark;
}

CXhChar100 GetBomPartSpec(BOMPART *pBomPart)
{
	CXhChar100 sSpec,sSpecialPartSpec;
	char cMaterial=pBomPart->cMaterial;
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP)
		{
			cMaterial=(strlen(pBomPart->sPartNo)>=9)?pBomPart->sPartNo.At(8):'0';
			if(cMaterial=='D')
				cMaterial='P';
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
		}
		else if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_U||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_X||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_C)
		{
			cMaterial=(strlen(pBomPart->sPartNo)>=6)?pBomPart->sPartNo.At(5):'0';
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
		}
		else if(pBomPart->siSubType==BOMPART::SUB_TYPE_STEEL_GRATING||
				pBomPart->siSubType==BOMPART::SUB_TYPE_STAY_ROPE||
				pBomPart->siSubType==BOMPART::SUB_TYPE_LADDER)
		{
			//const static BYTE SUB_TYPE_STEEL_GRATING= 18;	//钢格栅
			//const static BYTE SUB_TYPE_STAY_ROPE	= 19;	//拉索构件
			//const static BYTE SUB_TYPE_LADDER		= 20;	//爬梯
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
			cMaterial='0';
		}
	}
	CXhChar16 materialStr=cMaterial!='0'?_QuerySteelMatMark(cMaterial):"";
	if( pBomPart->sPartNo.EqualNoCase("T2105")||
		pBomPart->sPartNo.EqualNoCase("T2605")||
		pBomPart->sPartNo.EqualNoCase("T3605")||
		pBomPart->sPartNo.EqualNoCase("T3980")||
		pBomPart->sPartNo.EqualNoCase("T5855")||
		pBomPart->sPartNo.EqualNoCase("T6355")||
		pBomPart->cPartType==BOMPART::ACCESSORY)
	{
		materialStr="";
		sSpecialPartSpec.Copy(pBomPart->sPartNo);
	}
	if(pBomPart->cPartType==BOMPART::ANGLE)
		sSpec.Printf("∠%.fx%.f",pBomPart->wide,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::TUBE)
		sSpec.Printf("φ%.fx%.f",pBomPart->wide,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if( pBomPart->siSubType!=BOMPART::SUB_TYPE_LADDER&&
			pBomPart->siSubType!=BOMPART::SUB_TYPE_STAY_ROPE&&
			pBomPart->siSubType!=BOMPART::SUB_TYPE_STEEL_GRATING)
			sSpec.Printf("-%.f",pBomPart->thick);
		else 
			sSpec.Printf("%s",(char*)pBomPart->sSpec);
	}
	else if(pBomPart->cPartType==BOMPART::SLOT)
	{
		sSpec.Copy(pBomPart->sSpec);
		sSpec.Replace('C','[');
	}
	else
	{
		sSpec.Copy(pBomPart->sSpec);
		int hits=sSpec.Replace("φ"," ");
		hits+=sSpec.Replace("Φ"," ");
		sSpec.Replace("/"," ");
		sSpec.Replace("\\"," ");
		if(hits==2)
		{
			int D=0,d=0;
			sscanf(sSpec,"%d%d",&D,&d);
			sSpec.Printf("φ%d/%d",D,d);
		}
		else 
			sSpec.Copy(pBomPart->sSpec);
	}
	if(sSpecialPartSpec.GetLength()>0)
		return sSpecialPartSpec;
	else
		return sSpec;
	/*else if(materialStr.EqualNoCase("Q235"))
		return sSpec;
	else
		return CXhChar100("%s%s",(char*)materialStr,(char*)sSpec);*/
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
static bool SimplifiedNumString(char *num_str)
{
	if(num_str==NULL)
		return false;
	int str_len = strlen(num_str);
	char *point = NULL;
	for(int i=str_len-1;i>=0;i--)
	{
		if(num_str[i]=='.')
		{
			point=&num_str[i];
			break;
		}
	}
	if(point==NULL)
		return true;
	char* cursor=&num_str[str_len-1];
	while(cursor>=point)
	{
		if(*cursor=='0'||*cursor=='.')
		{
			*cursor='\0';
			cursor--;
		}
		else
			break;
	}
	if(*num_str=='-'&&*(num_str+1)=='0'&&*(num_str+2)==0)
	{
		*num_str='0';
		*(num_str+1)=0;
	}
	return true;
}
CXhChar200 GetPartNotes(BOMPART *pPart)
{
	if(pPart->siSubType==BOMPART::SUB_TYPE_BOLT_PAD)
		return CXhChar200();
	CXhChar200 sNotes;
	bool bPolyAngle=false;
	bool bDrillHole=false;
	if( (pPart->cMaterial=='S'&&pPart->thick>16)||			//钻孔
		(pPart->cMaterial=='H'&&pPart->thick>14)||
		(pPart->cMaterial=='P'&&pPart->thick>12)||
		pPart->cMaterial=='T'||pPart->IsDrillHole())
		bDrillHole=true;
	if(pPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pAngle=(PART_ANGLE*)pPart;
		//带脚钉
		if(pAngle->bHasFootNail)
			sNotes.Append("带脚钉,");
		//钻孔
		//if(bDrillHole)
		//	sNotes.Append("钻孔,");
		//切角
		if(pAngle->IsCutAngleOrWing())				
			sNotes.Append("切角,");
		//压扁
		if(pAngle->nPushFlat==0x05)			//0x01:始端压扁 0x02:中间压扁 0x04:终端压扁
			sNotes.Append("两端压扁,");
		else if((pAngle->nPushFlat==0x01)||(pAngle->nPushFlat==0x04))
			sNotes.Append("一端压扁,");
		if(pAngle->nPushFlat==0x02)
			sNotes.Append("中间压扁,");
		if(pAngle->bCutRoot)			//清根
			sNotes.Append("清根,");
		if(pAngle->bCutBer)				//铲背
			sNotes.Append("铲背,");
		if(pAngle->bKaiJiao)			//开角
		{
			sNotes.Append("开角");
			CXhChar16 ss("%.1f",pAngle->wing_angle);
			SimplifiedNumString(ss);
			sNotes.Append(ss);
			sNotes.Append(",");
		}
		if(pAngle->bHeJiao)				//合角
		{
			sNotes.Append("合角");
			CXhChar16 ss("%.1f",pAngle->wing_angle);
			SimplifiedNumString(ss);
			sNotes.Append(ss);
			sNotes.Append(",");
		}
		bPolyAngle=(pAngle->listPolyAngle.Count>0);
	}
	else if(pPart->cPartType==BOMPART::PLATE)	
	{	
		PART_PLATE *pPlate=(PART_PLATE*)pPart;
		//钻孔
		/*if(bDrillHole)
			sNotes.Append("钻孔,");
		//坡口
		if(pPlate->bNeedFillet)	
			sNotes.Append("坡口,");*/
		//卷边
		int nRollHeight=0;
		int nRollEdgeCount=0;
		for(LIST_NODE<BOMPROFILE_VERTEX>* pVertex=pPlate->listVertex.EnumFirst();pVertex;pVertex=pPlate->listVertex.EnumNext())
		{
			if(pVertex->data.m_bRollEdge)
			{
				if(nRollHeight==0)
					nRollHeight=abs(pVertex->data.manu_space);
				nRollEdgeCount++;
			}
		}
		if(nRollEdgeCount>0)
			sNotes.Append(CXhChar50("卷边%d,",nRollHeight));
		//割口
		//无孔
		/*int nHoleCount=pPlate->m_arrBoltRecs.GetSize();
		if(nHoleCount<=0)
			sNotes.Append("无孔,");*/
		/*else if(pPart->siSubType==BOMPART::SUB_TYPE_BOLT_PAD)
		{
			CString str;
			CXhChar50 sNote="垫板";
			if(pPart->nM16Ls>0)
			{
				str.Format("%d",pPart->nM16Ls);
				sNote+="Φ17.5*"+str;
			}
			if(pPart->nM20Ls>0)
			{
				str.Format("%d",pPart->nM20Ls);
				sNote+="Φ21.5*"+str;
			}
			if(pPart->nM24Ls>0)
			{
				str.Format("%d",pPart->nM24Ls);
				sNote+="Φ25.5*"+str;
			}
			sNotes.Append(sNote);
			sNotes.Append(" ");
		}*/
	}
	long nHuoquLineCount=pPart->GetHuoquLineCount(FALSE);
	if(nHuoquLineCount>0)		//火曲
	{
		sNotes.Append("火曲");
		for(int i=0;i<nHuoquLineCount;i++)
		{
			if(i==1)
				sNotes.Append(",");
			double fDegree=fabs(pPart->GetHuoquDegree(i));
			if(fDegree<90)
				fDegree=180-fDegree;
			CXhChar16 ss("%.1f",fDegree);
			SimplifiedNumString(ss);
			sNotes.Append(CXhChar16(ss));
		}
		sNotes.Append(",");
	}
	//制弯角钢不标注为焊接
	//if(!bPolyAngle&&pPart->bWeldPart)		//是否电焊件
	//	sNotes.Append("焊于 ");
	CXhChar200 sRealNotes;
	if(sNotes.EndWith(','))
		strncpy(sRealNotes,sNotes,sNotes.GetLength()-1);
	else
		strcpy(sRealNotes,sNotes);
	return sRealNotes;
}
CXhChar100 GetPartFunType(BOMPART *pBomPart)
{
	CXhChar100 sType;
	return sType;
}
CXhChar100 GetTubeSlotType(PART_TUBE *pBomTube,int *pCount=NULL,double *pfSlotType=NULL)
{
	int nSlotCount=0;
	double fSoltLen=0;
	BOOL bCSlot=FALSE,bXSlot=FALSE,bTSlot=FALSE;
	if( pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_CSLOT||
		pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_XSLOT||
		pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TSLOT)
	{
		if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_CSLOT)
		{
			bCSlot=TRUE;
			nSlotCount+=2;
			fSoltLen+=(2*pBomTube->startProcess.L);
		}
		else if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_XSLOT)
		{
			bXSlot=TRUE;
			nSlotCount+=4;
			fSoltLen+=(4*pBomTube->startProcess.L);
		}
		else if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TSLOT)
		{
			bTSlot=TRUE;
			nSlotCount+=3;
			fSoltLen+=(3*pBomTube->startProcess.L);
		}
	}
	if( pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_CSLOT||
		pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_XSLOT||
		pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TSLOT)
	{
		if(pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_CSLOT)
		{
			bCSlot=TRUE;
			nSlotCount+=2;
			fSoltLen+=(2*pBomTube->endProcess.L);
		}
		else if(pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_XSLOT)
		{
			bXSlot=TRUE;
			nSlotCount+=4;
			fSoltLen+=(4*pBomTube->endProcess.L);
		}
		else if(pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TSLOT)
		{
			bTSlot=TRUE;
			nSlotCount+=3;
			fSoltLen+=(3*pBomTube->endProcess.L);
		}
	}
	//中间开槽
	//
	CXhChar100 sType;
	if(bCSlot)
		sType.Append("一字槽");
	if(bXSlot)
	{
		if(sType.GetLength()>0)
			sType.Append("、");
		sType.Append("十字槽");
	}
	if(bTSlot)
	{
		if(sType.GetLength()>0)
			sType.Append("、");
		sType.Append("T字槽");
	}
	if(pCount!=NULL)
		*pCount=nSlotCount;
	if(pfSlotType)
		*pfSlotType=fSoltLen;
	return sType;
}
short CutAngleN(PART_ANGLE *pAngle,BYTE left0_right1_all2)
{
	short nLeftCount=0,nRightCount=0;
	for(int i=0;i<2;i++)
	{	//0.始端 1.终端
		short *pCount=(i==0)?&nLeftCount:&nRightCount;
		if(pAngle->cut_wing[i]>0&&pAngle->cut_wing_para[i][0]>0)
		{	//切肢
			if(pAngle->cut_wing_para[i][1]==0)
				*pCount+=2;		//切空肢(记1刀)
			else 
				*pCount+=3;		//切肢(记3刀)
		}
		if(pAngle->cut_angle[i*2][0]>0)		//X肢切角(记1刀)
			*pCount++;
		if(pAngle->cut_angle[i*2+1][0]>0)	//Y肢切角(记1刀)
			*pCount++;
	}
	if(left0_right1_all2==0)
		return nLeftCount;
	else if(left0_right1_all2==1)
		return nRightCount;
	else
		return (nLeftCount+nRightCount);
}

bool RetrieveArcLine(f3dArcLine &arcline, BOMPROFILE_VERTEX *pPrevVertex,BOMPROFILE_VERTEX *pVertex)
{
	if (pPrevVertex == NULL || pVertex == NULL)
		return false;
	GEPOINT start(pPrevVertex->vertex.x, pPrevVertex->vertex.y, 0);
	GEPOINT end(pVertex->vertex.x, pVertex->vertex.y, 0);
	//0:普通直边1:指定圆弧夹角2:指定圆弧半径3:椭圆弧
	if (start.IsEqual(end))	//防止因两点重合生成圆弧抛出异常
		arcline = f3dArcLine(f3dLine(start,end));
	else if (pPrevVertex->type == 0)	//普通直边
		arcline.CreateMethod2(start, end, f3dPoint(0, 0, 1), 0);
	else if (pPrevVertex->type == 1)	//指定圆弧夹角
	{
		int sign = 1;
		if (pPrevVertex->sector_angle < 0)
			sign *= -1;
		if (pPrevVertex->work_norm.z < 0)
			sign *= -1;
		arcline.CreateMethod2(start, end, f3dPoint(0, 0, sign), fabs(pPrevVertex->sector_angle));
	}
	else if (pPrevVertex->type == 2)	//指定圆弧半径
	{
		int sign = pPrevVertex->radius < 0 ? -1 : 1;
		f3dPoint center = pPrevVertex->center;
		arcline.CreateMethod3(start, end, f3dPoint(0, 0, sign), sign*pPrevVertex->radius, center);
	}
	else if (pPrevVertex->type == 3)	//椭圆弧
	{
		int sign = 1;
		if (pPrevVertex->radius < 0)
			sign = -1;
		if (pPrevVertex->work_norm.z < 0)
			sign *= -1;
		f3dPoint columnNorm = pPrevVertex->column_norm;
		f3dPoint centerTemp = pPrevVertex->center;
		/*if (ucs != NULL)
		{
			coord_trans(centerTemp, *ucs, FALSE);
			vector_trans(columnNorm, *ucs, FALSE);
		}*/
		arcline.CreateEllipse(centerTemp, start, end, columnNorm, f3dPoint(0, 0, sign), fabs(pPrevVertex->radius));
	}
	return true;
}

struct GROOVE_WELD_INFO{
	PART_PLATE *m_pPlate;
public:
	double m_fLen;
	double m_fWeldLen;
	int m_nEdgeCount;
	double m_fAngle;
	int m_nGrooveCount;

	void Init(PART_PLATE *pPlate){
		m_pPlate=pPlate;
		if (pPlate == NULL)
			return;
		if (m_pPlate->thick >= 8 && m_pPlate->thick <= 10)
			m_nGrooveCount = 1;
		else if (m_pPlate->thick >= 12)
			m_nGrooveCount = 2;
		else
			m_nGrooveCount = 0;
		m_fLen = 0;
		m_fWeldLen = 0;
		m_nEdgeCount = 0;
		BOMPROFILE_VERTEX* pPrevVertex=pPlate->listVertex.EnumObjectTail();
		for(BOMPROFILE_VERTEX *pVertex=pPlate->listVertex.EnumObjectFirst();pVertex;pVertex=pPlate->listVertex.EnumObjectNext())
		{
			if (pPrevVertex->m_bWeldEdge)
			{
				f3dArcLine arcLine;
				RetrieveArcLine(arcLine, pPrevVertex, pVertex);
				m_fLen += arcLine.Length();
				m_fWeldLen += arcLine.Length();
				m_nEdgeCount++;
			}
			pPrevVertex=pVertex;
		}
		m_fLen*=m_nGrooveCount;
	}
	GROOVE_WELD_INFO(PART_PLATE *pPlate){
		m_fLen=0;
		m_fWeldLen = 0;
		m_nEdgeCount=0;
		m_fAngle=30;	//30\45\60
		m_nGrooveCount=1;
		m_pPlate=NULL;
		Init(pPlate);
	}
};

struct PART_HOLE_PARA{
	BOMPART *m_pBomPart;
public:
	struct{
		long nGCountA,nGCountB;
		long nHoleDCountA,nHoleDCountB;
		long nHoleCountA,nHoleCountB;
		long nPairCount;	//套对数
	}angle;
	long nWaistHoleCount;	//拉长孔数
	long nHangWireHoleCount;		//挂线孔数
	CXhChar500 StatHoleStr(BYTE zuan0_kuo1_all2){
		CXhChar500 sHoleStr;
		CHashStrList<long> hashCountByHoleD;
		for(BOMBOLT_RECORD *pRec=m_pBomPart->m_arrBoltRecs.GetFirst();pRec;pRec=m_pBomPart->m_arrBoltRecs.GetNext())
		{
			if(zuan0_kuo1_all2==0&&pRec->cFlag!=0x01)
				continue;	//只统计钻孔
			if(zuan0_kuo1_all2==1&&pRec->cFlag!=0x02)
				continue;	//只统计扩孔
			double hole_d=pRec->d+pRec->hole_d_increment;
			CXhChar16 sHoleD("%.1f",hole_d);
			long nCount=0,*pCount=hashCountByHoleD.GetValue(sHoleD);
			if(pCount!=NULL)
				nCount=*pCount;
			nCount++;
			hashCountByHoleD.SetValue(sHoleD,nCount);
		}
		for(long *pCount=hashCountByHoleD.GetFirst();pCount;pCount=hashCountByHoleD.GetNext())
		{
			char *sKey=hashCountByHoleD.GetCursorKey();
			if(sHoleStr.GetLength()>0)
				sHoleStr.Append(",");
			sHoleStr.Append(CXhChar16("%s*%d",sKey,*pCount));
		}
		return sHoleStr;
	}
	void Init(BOMPART *pBomPart){
		m_pBomPart=pBomPart;
		if(m_pBomPart->cPartType==BOMPART::ANGLE)
		{
			angle.nHoleCountA=angle.nHoleCountB=0;
			CHashStrList<ARRAY_LIST<BOMBOLT_RECORD*>> hashRecListXByPosYStr,hashRecListYByPosYStr;
			CHashStrList<ARRAY_LIST<BOMBOLT_RECORD*>> hashRecListByPosXStr;
			CHashStrList<long> hashCountXByHoleD,hashCountYByHoleD;
			for(BOMBOLT_RECORD *pRec=m_pBomPart->m_arrBoltRecs.GetFirst();pRec;pRec=m_pBomPart->m_arrBoltRecs.GetNext())
			{
				CXhChar16 sPosX("%.1f",pRec->x),sPosY("%.1f",pRec->y);
				ARRAY_LIST<BOMBOLT_RECORD*> *pXRecList=hashRecListByPosXStr.GetValue(sPosX);
				if(pXRecList==NULL)
					pXRecList=hashRecListByPosXStr.Add(sPosX);
				pXRecList->append(pRec);
				//
				double hole_d=pRec->d+pRec->hole_d_increment;
				CXhChar16 sHoleD("%.1f",hole_d);
				long nCount=0;
				ARRAY_LIST<BOMBOLT_RECORD*> *pRecList=NULL;
				if(pRec->y<0)
				{
					angle.nHoleCountA++;
					long *pCount=hashCountXByHoleD.GetValue(sHoleD);
					if(pCount!=NULL)
						nCount=*pCount;
					nCount++;
					hashCountXByHoleD.SetValue(sHoleD,nCount);
					//
					pRecList=hashRecListXByPosYStr.GetValue(sPosY);
					if(pRecList==NULL)
						pRecList=hashRecListXByPosYStr.Add(sPosY);
				}
				else
				{
					angle.nHoleCountB++;
					long *pCount=hashCountYByHoleD.GetValue(sHoleD);
					if(pCount!=NULL)
						nCount=*pCount;
					nCount++;
					hashCountYByHoleD.SetValue(sHoleD,nCount);
					//
					pRecList=hashRecListYByPosYStr.GetValue(sPosY);
					if(pRecList==NULL)
						pRecList=hashRecListYByPosYStr.Add(sPosY);
				}
				pRecList->append(pRec);
			}
			angle.nHoleDCountA=hashCountXByHoleD.GetNodeNum();
			angle.nHoleDCountB=hashCountYByHoleD.GetNodeNum();
			angle.nGCountA=hashRecListXByPosYStr.GetNodeNum();
			angle.nGCountB=hashRecListYByPosYStr.GetNodeNum();
			angle.nPairCount=hashRecListByPosXStr.GetNodeNum();
		}
		//统计挂线孔、腰圆孔
		nHangWireHoleCount=0;
		nWaistHoleCount=0;
		for(BOMBOLT_RECORD *pRec=m_pBomPart->m_arrBoltRecs.GetFirst();pRec;pRec=m_pBomPart->m_arrBoltRecs.GetNext())
		{
			if(pRec->waistLen>0)
				nWaistHoleCount++;
			if(pRec->cFuncType==BOMBOLT_RECORD::FUNC_WIREHOLE)
				nHangWireHoleCount++;
		}
	}
	PART_HOLE_PARA(BOMPART *pBomPart){
		Init(pBomPart);
	}
};

struct HUOQU_LINE_INFO {
	BOOL bZhengQu;
	double fHuoquDegree;
	double fHuoquLineLen;
	HUOQU_LINE_INFO() {
		bZhengQu = FALSE;
		fHuoquDegree = fHuoquLineLen = 0;
	}
};
struct PLATE_HUOQUINFO {
	BOOL bZhengQu;
	ARRAY_LIST<HUOQU_LINE_INFO> huoquLineList;
	long GetCount() { return huoquLineList.GetSize(); }
	CXhChar50 GetHuoquType() {
		//火曲角度≥25度标记为热弯，其余为冷弯
		BOOL bHasLengQu = FALSE, bHasReQu = FALSE;
		for (HUOQU_LINE_INFO *pLine = huoquLineList.GetFirst(); pLine; pLine = huoquLineList.GetNext())
		{
			if (pLine->fHuoquDegree >= 25)
				bHasReQu = TRUE;
			else
				bHasLengQu = TRUE;
		}
		CXhChar50 sHuoquType;
		if (bHasLengQu)
			sHuoquType.Append("冷弯");
		if (bHasReQu)
		{
			if (sHuoquType.GetLength() > 0)
				sHuoquType.Append(" ");
			sHuoquType.Append("热曲");
		}
		return sHuoquType;
	}
	double GetMaxHuoquLen() {
		double fMaxLen = 0;
		for (HUOQU_LINE_INFO *pLine = huoquLineList.GetFirst(); pLine; pLine = huoquLineList.GetNext())
		{
			if (pLine->fHuoquLineLen > fMaxLen)
				fMaxLen = pLine->fHuoquLineLen;
		}
		return fMaxLen;
	}
	CXhChar50 GetHuoquDegree() {
		CHashStrList<double> hashDegreeByDegree;
		for (HUOQU_LINE_INFO *pLine = huoquLineList.GetFirst(); pLine; pLine = huoquLineList.GetNext())
		{
			CXhChar16 sKey("%.1f", pLine->fHuoquDegree);
			if (!hashDegreeByDegree.GetValue(sKey))
				hashDegreeByDegree.SetValue(sKey, pLine->fHuoquDegree);
		}
		CXhChar50 sReturn;
		for (double *pDegree = hashDegreeByDegree.GetFirst(); pDegree; pDegree = hashDegreeByDegree.GetNext())
		{
			CXhChar16 sDegree("%.1",*pDegree);
			SimplifiedNumString(sDegree);
			if (sReturn.GetLength() > 0)
				sReturn.Append(" ");
			sReturn.Append(sDegree);
		}
		return sReturn;
	}
	PLATE_HUOQUINFO() {
		bZhengQu = FALSE;
	}

	static void GetPlateHuoQuInfo(PART_PLATE *pPlate, PLATE_HUOQUINFO &huoquInfoA, PLATE_HUOQUINFO &huoquInfoB)
	{
		for (int i = 0; i < pPlate->m_cFaceN; i++)
		{
			HUOQU_LINE_INFO huoqu_line;
			huoqu_line.fHuoquDegree = pPlate->huoQuAngleArr[i];
			huoqu_line.fHuoquLineLen = 0;
			/*for (VERTEX_PTR pVertex = listVertex.EnumFirst(); pVertex; pVertex = listVertex.EnumNext())
			{
				if (pVertex->data.m_bRollEdge)
					nHuoquLineCount++;
			}*/
			if (pPlate->huoquFaceNormArr[i].z > 0)
			{	//正曲
				huoqu_line.bZhengQu = TRUE;
				huoquInfoA.huoquLineList.append(huoqu_line);
			}
			else
			{	//反曲
				huoqu_line.bZhengQu = FALSE;
				huoquInfoB.huoquLineList.append(huoqu_line);
			}
		}

		BOMPROFILE_VERTEX *pPrevVertex = pPlate->listVertex.EnumObjectTail();
		for (BOMPROFILE_VERTEX *pVertex = pPlate->listVertex.EnumObjectFirst(); pVertex; pVertex = pPlate->listVertex.EnumObjectNext())
		{
			if (!pPrevVertex->m_bRollEdge)
				continue;
			HUOQU_LINE_INFO huoqu_line;
			BOMPROFILE_VERTEX::COORD3D startVertex, endVertex;
			startVertex.Set(pPrevVertex->vertex.x, pPrevVertex->vertex.y);
			endVertex.Set(pVertex->vertex.x, pVertex->vertex.y);
			huoqu_line.fHuoquLineLen = DISTANCE(startVertex,endVertex);
			huoqu_line.fHuoquDegree = 90;
			if (pPrevVertex->manu_space > 0)
			{
				huoqu_line.bZhengQu = TRUE;
				huoquInfoA.huoquLineList.append(huoqu_line);
			}
			else
			{
				huoqu_line.bZhengQu = FALSE;
				huoquInfoB.huoquLineList.append(huoqu_line);
			}
		}
	}
};

long CalPlateEdgeCount(PART_PLATE *pPlate)
{
	BOOL bSpecialPlate = FALSE;
	int thick = (int)pPlate->thick;
	if (('S' == pPlate->cMaterial&&thick >= 20) ||
		('H' == pPlate->cMaterial&&thick >= 18) ||
		('P' == pPlate->cMaterial&&thick >= 16))
		bSpecialPlate = TRUE;
	if (bSpecialPlate&&pPlate->GetArcEdgeCount() > 0)
		return 0;
	else
		return pPlate->GetEdgeCount();
}

short GetKaiHeJiaoCount(PART_ANGLE *pAngle)
{
	short siKaiHeJiaoCount = 0;
	//200为1，小于200按1算 
	for (KAI_HE_JIAO *pKaiHeJiao = pAngle->listKaihe.EnumObjectFirst(); pKaiHeJiao; pKaiHeJiao = pAngle->listKaihe.EnumObjectNext())
	{
		short siLen = pKaiHeJiao->startLength + pKaiHeJiao->endLength;
		short siCount = siLen / 200;
		if (siLen % 200 > 0)
			siCount += 1;
		siKaiHeJiaoCount += siCount;
	}
	return siKaiHeJiaoCount;
}

void AddPartTblSheet(CModelBOM &bom,LPDISPATCH pWorkSheet)
{
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
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("ERP接口");
	//CExcelOper::SetRangeColWidth(excel_sheet,12.0,"C1");
	//清空原有内容
	Range excel_usedRange,excel_range;
	excel_usedRange.AttachDispatch(excel_sheet.GetUsedRange());
	excel_usedRange.Select();
	excel_usedRange.ClearContents();
	
	CXhChar16 cell_start,cell_end;
	int index=0;
	CVariant2dArray map(BomArr.GetSize()+2,62);
	//标题行
	map.SetValueAt(index,0,COleVariant("段名"));
	map.SetValueAt(index,1,COleVariant("部件号"));
	map.SetValueAt(index,2,COleVariant("材料名称"));
	map.SetValueAt(index,3,COleVariant("材质"));
	map.SetValueAt(index,4,COleVariant("规格"));
	map.SetValueAt(index,5,COleVariant("长(mm)"));
	map.SetValueAt(index,6,COleVariant("宽(mm)"));
	map.SetValueAt(index,7,COleVariant("厚(mm)"));
	map.SetValueAt(index,8,COleVariant("单段数量"));
	map.SetValueAt(index,9,COleVariant("单件重量(kg)"));
	map.SetValueAt(index,10,COleVariant("总重kg"));
	map.SetValueAt(index,11,COleVariant("备注"));
	map.SetValueAt(index,12,COleVariant("单件孔数"));
	map.SetValueAt(index,13,COleVariant("类型"));
	map.SetValueAt(index,14,COleVariant("边数"));
	map.SetValueAt(index,15,COleVariant("钻孔"));
	map.SetValueAt(index,16,COleVariant("有扩孔"));
	map.SetValueAt(index,17,COleVariant("电焊"));
	map.SetValueAt(index,18,COleVariant("电焊长度mm"));
	map.SetValueAt(index,19,COleVariant("火曲"));
	map.SetValueAt(index,20,COleVariant("A面火曲点数"));
	map.SetValueAt(index,21,COleVariant("B面火曲点数"));
	map.SetValueAt(index,22,COleVariant("A面火曲度数"));
	map.SetValueAt(index,23,COleVariant("B面火曲度数"));
	map.SetValueAt(index,24,COleVariant("A面火曲类型"));
	map.SetValueAt(index,25,COleVariant("B面火曲类型"));
	map.SetValueAt(index,26,COleVariant("火曲后弯曲长度"));
	map.SetValueAt(index,27,COleVariant("切角"));
	map.SetValueAt(index,28,COleVariant("左端头切角刀数"));
	map.SetValueAt(index,29,COleVariant("右端头切角刀数"));
	map.SetValueAt(index,30,COleVariant("铲背"));
	map.SetValueAt(index,31,COleVariant("清根"));
	map.SetValueAt(index,32,COleVariant("打扁"));
	map.SetValueAt(index,33,COleVariant("打扁类型"));
	map.SetValueAt(index,34,COleVariant("开/合角(0/1/2)"));
	map.SetValueAt(index,35,COleVariant("开合角点数"));
	map.SetValueAt(index,36,COleVariant("开合角长度"));
	map.SetValueAt(index,37,COleVariant("割豁"));
	map.SetValueAt(index,38,COleVariant("含弧"));
	map.SetValueAt(index,39,COleVariant("开槽形式"));
	map.SetValueAt(index,40,COleVariant("开槽总数"));
	map.SetValueAt(index,41,COleVariant("开槽长度"));
	map.SetValueAt(index,42,COleVariant("坡口"));
	map.SetValueAt(index,43,COleVariant("坡口米数(米)"));
	map.SetValueAt(index,44,COleVariant("坡口角度"));
	map.SetValueAt(index,45,COleVariant("打坡口端数"));
	map.SetValueAt(index,46,COleVariant("相贯线"));
	map.SetValueAt(index,47,COleVariant("相贯形式"));
	map.SetValueAt(index,48,COleVariant("相贯线切割长度"));
	map.SetValueAt(index,49,COleVariant("A面芯距数"));
	map.SetValueAt(index,50,COleVariant("B面芯距数"));
	map.SetValueAt(index,51,COleVariant("A面孔径数"));
	map.SetValueAt(index,52,COleVariant("B面孔径数"));
	map.SetValueAt(index,53,COleVariant("拉长孔数"));
	map.SetValueAt(index,54,COleVariant("套对孔(步)数"));
	map.SetValueAt(index,55,COleVariant("挂线孔"));
	map.SetValueAt(index,56,COleVariant("各孔孔径孔数"));
	map.SetValueAt(index,57,COleVariant("钻孔孔径孔数"));
	map.SetValueAt(index,58,COleVariant("扩孔孔径孔数"));
	map.SetValueAt(index,59,COleVariant("板件是否矩形"));
	map.SetValueAt(index,60,COleVariant("周长mm"));
	map.SetValueAt(index,61,COleVariant("面积"));

	index++;
	//设置列宽
	CExcelOper::SetRangeColWidth(excel_sheet,5.0,"A1",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,10.0,"B1",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,10.0,"C1",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,6.0,"D1",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,10.0,"E1",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,7.5,"F1","G1");
	CExcelOper::SetRangeColWidth(excel_sheet,12.0,"J1","K1");
	CExcelOper::SetRangeColWidth(excel_sheet,35.0,"L1",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,7.5,"M1","BD1");
	CExcelOper::SetRangeColWidth(excel_sheet,23.5,"BE1",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,12.5,"BF1","BJ1");
	//
	long n=BomArr.GetSize();
	CXhChar16 cell;
	BYTE part_type=0;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		PART_PLATE *pBomPlate=NULL;
		PART_ANGLE *pBomAngle=NULL;
		PART_TUBE *pBomTube=NULL;
		if(pBomPart->cPartType==BOMPART::PLATE)
			pBomPlate=(PART_PLATE*)pBomPart;
		else if(pBomPart->cPartType==BOMPART::ANGLE)
			pBomAngle=(PART_ANGLE*)pBomPart;
		else if(pBomPart->cPartType==BOMPART::TUBE)
			pBomTube=(PART_TUBE*)pBomPart;
		SEGI segI(pBomPart->iSeg);
		CXhChar16 sPartNo(pBomPart->sPartNo);	//pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)
		map.SetValueAt(index,0,COleVariant(segI));
		map.SetValueAt(index,1,COleVariant(sPartNo));
		map.SetValueAt(index,2,COleVariant(pBomPart->GetPartTypeName()));
		char cMat=pBomPart->cMaterial;
		CXhChar16 sMat=QuerySteelMatMark(cMat);
		map.SetValueAt(index,3,COleVariant(sMat));
		map.SetValueAt(index,4,COleVariant(GetBomPartSpec(pBomPart)));	//规格
		/*cell.Printf("D%d",index+1);
		if(cMat=='S')
			CExcelOper::SetFontColor(excel_sheet,cell,NULL,CExcelOper::COLOR_ID_RED);
		else if(cMat=='H')
			CExcelOper::SetFontColor(excel_sheet,cell,NULL,CExcelOper::COLOR_ID_GREEN);*/
		//长度
		double length=pBomPart->length;
		if(bom.m_iPlankWeightStatType==2&&pBomPart->cPartType==BOMPART::PLATE)
			length=((PART_PLATE*)pBomPart)->real_area/pBomPart->wide;	//净面积输出时应根据钢板净面积与宽反推钢板长度
		map.SetValueAt(index,5,COleVariant(CXhChar16("%.f",length)));
		if(pBomPart->cPartType==BOMPART::PLATE)
			map.SetValueAt(index,6,COleVariant(CXhChar16("%.f",pBomPart->wide)));
		if(pBomAngle||pBomTube||pBomPlate)
			map.SetValueAt(index,7,COleVariant(CXhChar16("%.f",pBomPart->thick)));//厚度
		map.SetValueAt(index,8,COleVariant((long)pBomPart->GetPartNum()));	//单基数
		CXhChar16 sWeight("%.2f",pBomPart->fPieceWeight);
		map.SetValueAt(index,9,COleVariant(sWeight));
		double fSumWeight=atof(sWeight)*pBomPart->GetPartNum();
		map.SetValueAt(index,10,COleVariant(CXhChar16("%.2f",fSumWeight)));
		CXhChar200 ss =GetPartNotes(pBomPart);
		strcat(ss,pBomPart->sNotes);
		map.SetValueAt(index,11,COleVariant(ss));
		map.SetValueAt(index,12,COleVariant((long)pBomPart->GetBoltOrHoleNum(0)));
		map.SetValueAt(index,13,COleVariant(GetPartFunType(pBomPart)));
		if(pBomPlate)
		{
			long nEdgeCount = CalPlateEdgeCount(pBomPlate);
			if(nEdgeCount>0)
				map.SetValueAt(index, 14, COleVariant(nEdgeCount));
		}
		if(pBomPart->IsDrillHole())		//钻孔
			map.SetValueAt(index,15,COleVariant("1"));
		if(pBomPart->IsExpandHole())	//扩孔
			map.SetValueAt(index,16,COleVariant("1"));
		if(pBomPart->bWeldPart)			//电焊
			map.SetValueAt(index,17,COleVariant("1"));
		GROOVE_WELD_INFO plateWeldInfo(pBomPlate);
		double fWeldLen = plateWeldInfo.m_fWeldLen;
		if(fWeldLen>0)
			map.SetValueAt(index,18,COleVariant(CXhChar16("%.1f",fWeldLen)));
		int nCount=pBomPart->GetHuoquLineCount();
		if(nCount>0)
			map.SetValueAt(index,19,COleVariant("1"));
		if(pBomPlate)
		{	//A/B火曲点数、A/B面火曲度数、A/B面火曲类型、火曲后弯曲长度
			PLATE_HUOQUINFO plateSideA, plateSideB;
			PLATE_HUOQUINFO::GetPlateHuoQuInfo(pBomPlate, plateSideA, plateSideB);
			//A面
			if(plateSideA.GetCount()>0)
			{
				map.SetValueAt(index,20,COleVariant(plateSideA.GetCount()));
				map.SetValueAt(index,22,COleVariant(plateSideA.GetHuoquDegree()));
				map.SetValueAt(index,24,COleVariant(plateSideA.GetHuoquType()));
			}
			if(plateSideB.GetCount()>0)
			{
				map.SetValueAt(index, 21, COleVariant(plateSideB.GetCount()));
				map.SetValueAt(index, 23, COleVariant(plateSideB.GetHuoquDegree()));
				map.SetValueAt(index, 25, COleVariant(plateSideB.GetHuoquType()));
			}
			double fHuoquMaxLen = max(plateSideA.GetMaxHuoquLen(), plateSideB.GetMaxHuoquLen());
			if(fHuoquMaxLen)
				map.SetValueAt(index,26,COleVariant(CXhChar16("%.1f",fHuoquMaxLen)));	//火曲后弯曲长度
		}
		if(pBomAngle)
		{
			if(CutAngleN(pBomAngle,2)>0)
				map.SetValueAt(index,27,COleVariant("1"));
			else
				map.SetValueAt(index,27,COleVariant(""));
			int nLeftCount=CutAngleN(pBomAngle,0);
			if(nLeftCount>0)
				map.SetValueAt(index,28,COleVariant((long)nLeftCount));
			else
				map.SetValueAt(index,28,COleVariant(""));
			int nRightCount=CutAngleN(pBomAngle,1);
			if(nRightCount>0)
				map.SetValueAt(index,29,COleVariant((long)nRightCount));
			else
				map.SetValueAt(index,29,COleVariant(""));
			if(pBomAngle->bCutBer>0)
				map.SetValueAt(index,30,COleVariant("1"));
			else
				map.SetValueAt(index,30,COleVariant(""));
			if(pBomAngle->bCutRoot>0)
				map.SetValueAt(index,31,COleVariant("1"));
			else
				map.SetValueAt(index,31,COleVariant(""));
			CXhChar16 sPushType;
			if(pBomAngle->nPushFlat==0x05)			//0x01:始端压扁 0x02:中间压扁 0x04:终端压扁
				sPushType.Append("两端压扁,");
			else if((pBomAngle->nPushFlat==0x01)||(pBomAngle->nPushFlat==0x04))
				sPushType.Append("一端压扁,");
			else if(pBomAngle->nPushFlat==0x02)
				sPushType.Append("中间压扁,");
			if(sPushType.GetLength()>0)
			{
				map.SetValueAt(index,32,COleVariant("1"));
				map.SetValueAt(index,33,COleVariant(sPushType));
			}
			else
			{
				map.SetValueAt(index,32,COleVariant(""));
				map.SetValueAt(index,33,COleVariant(""));
			}
			long nKaiHeType=0;
			if(pBomAngle->bKaiJiao)
				nKaiHeType=1;
			if(pBomAngle->bHeJiao)
				nKaiHeType=2;
			map.SetValueAt(index,34,COleVariant(nKaiHeType));
			long nKaiHeCount = GetKaiHeJiaoCount(pBomAngle);
			if (nKaiHeCount > 0)
			{
				map.SetValueAt(index, 35, COleVariant(nKaiHeCount));
				/*double fKaiHeLen = 0;
				for (KAI_HE_JIAO *pKaiHe = pBomAngle->listKaihe.EnumObjectFirst(); pKaiHe; pKaiHe = pBomAngle->listKaihe.EnumObjectNext())
					fKaiHeLen += (pKaiHe->endLength - pKaiHe->startLength);*/
				long nKaiHeLen = nKaiHeCount * 200;
				map.SetValueAt(index, 36, COleVariant(nKaiHeLen));
			}
		}
		if(pBomPlate)
		{
			map.SetValueAt(index,37,COleVariant(""));		//割豁
			if(pBomPlate->GetArcEdgeCount()>0)
				map.SetValueAt(index,38,COleVariant("1"));	//含弧
		}
		if(pBomTube)
		{
			int nSlotCount=0;
			double fSlotLen=0;
			CXhChar100 sSlotType=GetTubeSlotType(pBomTube,&nSlotCount,&fSlotLen);
			map.SetValueAt(index,39,COleVariant(sSlotType));
			if(nSlotCount>0)
			{
				map.SetValueAt(index,40,COleVariant((long)nSlotCount));
				map.SetValueAt(index,41,COleVariant(CXhChar16("%.f",fSlotLen)));
			}
		}
		if(pBomPlate)
		{	//坡口
			if(plateWeldInfo.m_fLen>0)
			{
				map.SetValueAt(index,42,COleVariant("1"));
				map.SetValueAt(index,43,COleVariant(CXhChar16("%.3f",plateWeldInfo.m_fWeldLen)));
				map.SetValueAt(index,44,COleVariant(CXhChar16("%.f",plateWeldInfo.m_fAngle)));
				map.SetValueAt(index,45,COleVariant((long)plateWeldInfo.m_nEdgeCount));
			}
		}
		
		if(pBomTube)
		{	//相贯线
			CXhChar100 sType;
			double fLen=0;
			if( pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_PLANECUT||
				pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TRANSECT)
			{
				fLen+=pBomTube->length;
				if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_PLANECUT)
					sType.Append("管管相贯");
				else
					sType.Append("管板相贯");
			}
			if( pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_PLANECUT||
				pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TRANSECT)
			{
				fLen+=pBomTube->length;
				if(sType.GetLength()>0)
					sType.Append(",");
				if(pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_PLANECUT)
					sType.Append("管管相贯");
				else
					sType.Append("管板相贯");
			}
			if(fLen>0)
			{
				map.SetValueAt(index,46,COleVariant("1"));
				map.SetValueAt(index,47,COleVariant(sType));
				map.SetValueAt(index,48,COleVariant(CXhChar16("%.f",fLen)));
			}
		}
		//
		PART_HOLE_PARA holePara(pBomPart);
		if(pBomAngle)
		{
			if(holePara.angle.nGCountA>0)
				map.SetValueAt(index,49,COleVariant(holePara.angle.nGCountA));
			if(holePara.angle.nGCountB>0)
				map.SetValueAt(index,50,COleVariant(holePara.angle.nGCountB));
			if(holePara.angle.nHoleCountA>0)
				map.SetValueAt(index,51,COleVariant(holePara.angle.nHoleCountA));
			if(holePara.angle.nHoleCountB>0)
				map.SetValueAt(index,52,COleVariant(holePara.angle.nHoleCountB));
			if(holePara.angle.nPairCount>0)
				map.SetValueAt(index,54,COleVariant(holePara.angle.nPairCount));
		}
		if(holePara.nWaistHoleCount>0)
			map.SetValueAt(index,53,COleVariant(holePara.nWaistHoleCount));
		if(holePara.nHangWireHoleCount>0)
			map.SetValueAt(index,55,COleVariant(holePara.nHangWireHoleCount));
		map.SetValueAt(index,56,COleVariant(holePara.StatHoleStr(2)));	//各孔孔径
		map.SetValueAt(index,57,COleVariant(holePara.StatHoleStr(0)));	//钻孔孔径
		map.SetValueAt(index,58,COleVariant(holePara.StatHoleStr(1)));	//扩孔孔径
		//
		if(pBomPlate)
		{
			if(IsRectPlate(pBomPlate))
				map.SetValueAt(index,59,COleVariant("1"));
			map.SetValueAt(index,60,COleVariant(CXhChar16("%.2f",pBomPlate->fPerimeter)));
			map.SetValueAt(index,61,COleVariant(CXhChar16("%.6f",(pBomPlate->real_area*0.000001))));
		}
		index++;
	}
	//设置构件编号格式
	strcpy(cell_start,"B2");
	sprintf(cell_end,"B%d",index+1);
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,cell_start,cell_end,COleVariant("@"));
	//设置段号水平对齐方式 居中对齐 wht 13-03-11
	//strcpy(cell_start,"A1");
	//sprintf(cell_end,"G%d",index+1);
	//CExcelOper::SetRangeHorizontalAlignment(excel_sheet,cell_start,cell_end,COleVariant((long)3));
	//设置表格内容
	strcpy(cell_start,"A1");
	sprintf(cell_end,"BJ%d",index+1);
	CExcelOper::SetRangeValue(excel_sheet,cell_start,cell_end,map.var);
	//设置边框
	//CExcelOper::SetRangeBorders(excel_sheet,cell_start,cell_end,COleVariant(10.5));
	//
	CExcelOper::SetRangeRowHeight(excel_sheet,15.75,cell_start,cell_end);
	CExcelOper::SetFontSize(excel_sheet,cell_start,cell_end,12.0);
	CExcelOper::SetRangeWrapText(excel_sheet,"A1","BJ1",COleVariant((long)TRUE));
	excel_sheet.ReleaseDispatch();
}

//////////////////////////////////////////////////////////////////////////
//
TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf,int buf_len,const char* file_name,DWORD dwFlag)
{	
	CModelBOM bom;
	CBuffer buffer(buf_len);
	if(buf_len>0)
	{
		buffer.AttachMemory(data_buf,buf_len);
		bom.FromBuffer(buffer,GetSupportDataBufferVersion());
	}
	//
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	//
	LPDISPATCH pWorksheet=excel_sheets.GetItem(COleVariant((short) 1));
	AddPartTblSheet(bom,pWorksheet);
	//	
	excel_sheets.ReleaseDispatch();
	return 0;
}