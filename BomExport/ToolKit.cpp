#include "stdafx.h"
#include "ToolKit.h"
#include "SortFunc.h"
#include "math.h"

//
BOMBOLT_BOX::BOMBOLT_BOX(const BOMBOLT_BOX &srcBoltBox)
{
	Clone(srcBoltBox);
}
BOMBOLT_BOX& BOMBOLT_BOX::operator=(const BOMBOLT_BOX &srcBoltBox)
{
	Clone(srcBoltBox);
	return *this;
}
void BOMBOLT_BOX::Clone(const BOMBOLT_BOX &srcBoltBox)
{
	BOMBOLT_BOX *pSrcBoltBox=const_cast<BOMBOLT_BOX*>(&srcBoltBox);
	CopyBomBoltProp(pSrcBoltBox->bomBolt,bomBolt);
	bomBolt.AddPart(pSrcBoltBox->bomBolt.GetPartNum());
	m_hashNumBySegI.Empty();
	for(long *pCount=pSrcBoltBox->m_hashNumBySegI.GetFirst();pCount;pCount=pSrcBoltBox->m_hashNumBySegI.GetNext())
		m_hashNumBySegI.SetValue(pSrcBoltBox->m_hashNumBySegI.GetCursorKey(),*pCount);
}

CXhChar16 GetPartSpec(BOMPART *pPart)
{
	CXhChar16 sSpec=pPart->GetSpec();
	return sSpec;
}

int GetCompareGrade(const BOMBOLT& bomBolt)
{
	if(bomBolt.IsDianQuan())	//垫圈
		return 100;
	else if(bomBolt.bFootNail)	//脚钉
	{
		if(bomBolt.bAnitLoose)
			return 6;
		else if (bomBolt.bAntiTheft)
			return 7;
		else 
			return 5;
	}
	else	//普通螺栓
	{
		if(bomBolt.bAntiTheft)
			return 4;
		else if(bomBolt.bAnitLoose)
			return 3;
		else if(bomBolt.bTwoCap)
			return 2;
		else
			return 1;
	}
}

int GetPartCompareGrade(const BOMBOLT& bomBolt)
{
	if(bomBolt.IsDianQuan())
	{
		if(bomBolt.nCommonGasket>0)
			return 6;
		else if(bomBolt.nSpringGasket>0)
			return 5;
		else if(bomBolt.nLockClasp>0)
			return 4;
	}
	else 
	{
		if(bomBolt.nThinNut>0)
			return 3;
		else if(bomBolt.nAntiTheftNut>0)
			return 2;
		else if(bomBolt.nFastenNut>0)
			return 1;
	}
	return 0;
}

int CompareBomBolt(const BOMBOLT& item1,const BOMBOLT& item2)
{
	if(item1.IsDianQuan() && !item2.IsDianQuan())
		return -1;
	else if(!item1.IsDianQuan() && item2.IsDianQuan())
		return 1;

	if(GetCompareGrade(item1)<GetCompareGrade(item2))
		return -1;
	else if(GetCompareGrade(item1)>GetCompareGrade(item2))
		return 1;
	else
	{	//优先级相同按级别排序
		if(item1.grade<item2.grade)
			return -1;
		else if(item1.grade>item2.grade)
			return 1;
		else
		{	//级别相同按直径排序
			if(item1.d<item2.d)
				return -1;
			else if(item1.d>item2.d)
				return 1;
			else
			{	//直径相同按有效长度排序
				if(item1.L<item2.L)
					return -1;
				else if(item1.L>item2.L)
					return 1;
				else 
					return 0;
			}
		}
	}
}

int CompareBomBoltIncSegI(const BOMBOLT& item1,const BOMBOLT& item2)
{	
	if(item1.IsDianQuan() && !item2.IsDianQuan())
		return -1;
	else if(!item1.IsDianQuan() && item2.IsDianQuan())
		return 1;
	//按段号排序
	if(item1.iSeg<item2.iSeg)
		return -1;
	else if(item1.iSeg>item2.iSeg)
		return 1;
	else	//段号相同按螺栓优先级排序
		return CompareBomBolt(item1,item2);
}

typedef BOMBOLT_BOX* BOLTPTR;
int CompareBomBoltBoxPtr(const BOLTPTR& item1,const BOLTPTR& item2)
{
	return CompareBomBolt(item1->bomBolt,item2->bomBolt);
}

void CopyBomBoltProp(BOMBOLT &srcBolt,BOMBOLT &destBolt)
{
	destBolt.bFootNail=srcBolt.bFootNail;
	destBolt.bTwoCap=srcBolt.bTwoCap;
	destBolt.bAntiTheft=srcBolt.bAntiTheft;
	destBolt.bAnitLoose=srcBolt.bAnitLoose;
	destBolt.bUFootNail=srcBolt.bUFootNail;
	destBolt.d=srcBolt.d;
	destBolt.nNoThreadlen=srcBolt.nNoThreadlen;
	destBolt.L=srcBolt.L;
	destBolt.L0=srcBolt.L0;
	destBolt.hFamily=srcBolt.hFamily;
	destBolt.grade=srcBolt.grade;
	destBolt.iSeg=srcBolt.iSeg;
	destBolt.fUnitWeight=srcBolt.fUnitWeight;
	destBolt.sMaterial=srcBolt.sMaterial;
	destBolt.nAntiTheftNut=srcBolt.nAntiTheftNut;
	destBolt.nCommonGasket=srcBolt.nCommonGasket;
	destBolt.nCommonNut=srcBolt.nCommonNut;
	destBolt.nFastenNut=srcBolt.nFastenNut;
	destBolt.nLockClasp=srcBolt.nLockClasp;
	destBolt.nSpringGasket=srcBolt.nSpringGasket;
	destBolt.nThinNut=srcBolt.nThinNut;
}

void GetBomBoltBoxArr(CModelBOM &bom,BOMBOLT_BOX *boltArr,int &arr_size)
{	//重新汇总螺栓不考虑分段
	CHashStrList<BOMBOLT_BOX> hashBoltByCombinedKey; //通过类型
	for (int i=0;i<bom.m_nBoltsN;i++)
	{
		BOMBOLT bolt=bom.m_pBoltRecsArr[i];
		CXhChar50 sKey=bolt.CombinedKey();
		BOMBOLT_BOX* pSumBolt=hashBoltByCombinedKey.GetValue(sKey);
		if(pSumBolt==NULL)
		{
			pSumBolt=hashBoltByCombinedKey.Add(sKey);
			CopyBomBoltProp(bolt,pSumBolt->bomBolt);
		}
		pSumBolt->bomBolt.AddPart(bolt.GetPartNum());
		pSumBolt->m_hashNumBySegI.SetValue(bolt.iSeg,bolt.GetPartNum());
	}
	arr_size=0;
	ARRAY_LIFE<BOMBOLT_BOX> arrayLife;
	BOMBOLT_BOX *tempBoltArr=(BOMBOLT_BOX*)arrayLife.Create(bom.m_nBoltsN);		
	for(BOMBOLT_BOX *pBolt=hashBoltByCombinedKey.GetFirst();pBolt;pBolt=hashBoltByCombinedKey.GetNext())
	{
		tempBoltArr[arr_size]=*pBolt;
		arr_size++;
	}
	if(arr_size>0)
	{
		ARRAY_LIFE<BOLTPTR> boltPtrArrLife;
		BOLTPTR* boltPtrArr=(BOLTPTR*)boltPtrArrLife.Create(arr_size);
		for(int i=0;i<arr_size;i++)
			boltPtrArr[i]=&tempBoltArr[i];
		CBubbleSort<BOLTPTR>::BubSort(boltPtrArr,arr_size,CompareBomBoltBoxPtr);
		for(int i=0;i<arr_size;i++)
			boltArr[i]=*boltPtrArr[i];
	}
}
#define      DEGTORAD_COEF    57.2957795130823208768
//切角：切角后数字为切角刀数，CT、CF、CO为1刀，BC、  AxBxC为2刀，表中所填数字为一张样杆所有切角刀数的总和。\
//		如：切角值为BC+CT+CO为2+1+1 标识为“切角4”
//顺泰	2017-3-29 ccs
int GetCutAngleSwordNum(PART_ANGLE *pBomAngle)
{
	int nNum=0;
	int alfa = 0;
	if(pBomAngle->cut_wing[0]>0)
	{
		CXhChar16 alfa_str;
		if(pBomAngle->cut_wing[0]==1)		//始端X肢有切肢
		{
			alfa = (int)(DEGTORAD_COEF*atan((pBomAngle->cut_wing_para[0][2]-pBomAngle->cut_wing_para[0][0])/pBomAngle->wide));
			if(pBomAngle->cut_wing_para[0][1]==0)	//X肢切整肢
				nNum++;
			else	//背切肢
				nNum=nNum+2;
			if(alfa!=0)
				nNum++;
		}
		else if(pBomAngle->cut_wing[0]==2)	//始端Y肢有切肢
		{

			alfa = (int)(DEGTORAD_COEF*atan((pBomAngle->cut_wing_para[0][2]-pBomAngle->cut_wing_para[0][0])/pBomAngle->wide));
			if(pBomAngle->cut_wing_para[0][1]==0)	//X肢切整肢
				nNum++;
			else	//背切肢
				nNum=nNum+2;
			if(alfa!=0)
				nNum++;
		}
	}
	if(pBomAngle->cut_angle[0][0]>0&&pBomAngle->cut_angle[0][1]>0&&pBomAngle->cut_wing[0]!=1)
	{	//始端X肢有切角
		alfa = int(DEGTORAD_COEF*atan((double)pBomAngle->cut_angle[0][0]/pBomAngle->cut_angle[0][1]));
		if(pBomAngle->cut_angle[0][1]==pBomAngle->wide)
			nNum+=2;
		else
			nNum++;
	}
	if(pBomAngle->cut_angle[1][0]>0&&pBomAngle->cut_angle[1][1]>0&&pBomAngle->cut_wing[0]!=2)
	{	//始端Y肢有切角
		int alfa = int(DEGTORAD_COEF*atan((double)pBomAngle->cut_angle[0][0]/pBomAngle->cut_angle[0][1]));
		if(pBomAngle->cut_angle[1][1]==pBomAngle->wide)
			nNum+=2;
		else
			nNum++;
	}
	if(pBomAngle->cut_wing[1]>0)
	{
		CXhChar16 alfa_str;
		if(pBomAngle->cut_wing[1]==1)		//终端X肢有切肢
		{
			alfa = int(DEGTORAD_COEF*atan((pBomAngle->cut_wing_para[1][2]-pBomAngle->cut_wing_para[1][0])/pBomAngle->wide));
			if(pBomAngle->cut_wing_para[1][1]==0)	//X肢切整肢
				nNum++;
			else	//背切肢
				nNum=nNum+2;
			if(alfa!=0)
				nNum++;
		}
		else if(pBomAngle->cut_wing[1]==2)	//终端Y肢有切肢
		{
			alfa = int(DEGTORAD_COEF*atan((pBomAngle->cut_wing_para[1][2]-pBomAngle->cut_wing_para[1][0])/pBomAngle->wide));
			if(pBomAngle->cut_wing_para[1][1]==0)	//X肢切整肢
				nNum++;
			else	//背切肢
				nNum=nNum+2;
			if(alfa!=0)
				nNum++;
		}
	}
	if(pBomAngle->cut_angle[2][0]>0&&pBomAngle->cut_angle[2][1]>0&&pBomAngle->cut_wing[1]!=1)	
	{	//终端X肢有切角
		alfa = int(DEGTORAD_COEF*atan((double)pBomAngle->cut_angle[2][0]/pBomAngle->cut_angle[2][1]));
		if(pBomAngle->cut_angle[2][1]==pBomAngle->wide)
			nNum+=2;
		else
			nNum++;
	}
	if(pBomAngle->cut_angle[3][0]>0&&pBomAngle->cut_angle[3][1]>0&&pBomAngle->cut_wing[1]!=2)
	{	//终端Y肢有切角
		alfa = int(DEGTORAD_COEF*atan((double)pBomAngle->cut_angle[3][0]/pBomAngle->cut_angle[3][1]));
		if(pBomAngle->cut_angle[3][1]==pBomAngle->wide)
			nNum+=2;
		else
			nNum++;
	}
	return nNum;
}

BOMPROFILE_VERTEX::COORD3D NormalizedVector(BOMPROFILE_VERTEX::COORD3D &vec)
{
	double dd=sqrt(vec.x*vec.x+vec.y*vec.y+vec.z*vec.z);
	if(dd>1e-9)
	{
		vec.x=vec.x/dd; 
		vec.y=vec.y/dd;
		vec.z=vec.z/dd;
	}
	else
		vec.Set();
	return vec;
}
//
short GetKaiHeJiaoN(PART_ANGLE *pAngle)
{
	short siKaiHeJiaoCount=0;
	if(pAngle->bKaiJiao||pAngle->bHeJiao)
	{
		for(LIST_NODE<KAI_HE_JIAO> *pNode=pAngle->listKaihe.EnumFirst();pNode;pNode=pAngle->listKaihe.EnumNext())
			siKaiHeJiaoCount++;

	}
	return siKaiHeJiaoCount;
}
BOOL IsUnequalAngle(double x_wing_wide,double y_wing_wide,double thick)
{	//Y肢宽度大于肢厚且不等于X肢宽度
	if(y_wing_wide>=thick&&fabs(x_wing_wide-y_wing_wide)>0)
		return TRUE;
	return FALSE;
}
//
int GetMatColorIndex(CXhChar16 sMat)
{
	if(strstr(sMat,"Q235"))
		return Q235_COLOR;
	else if(strstr(sMat,"Q345"))
		return Q345_COLOR;
	else if(strstr(sMat,"Q390"))
		return Q390_COLOR;
	else if(strstr(sMat,"Q420"))
		return Q420_COLOR;
	else if(strstr(sMat,"Q460"))
		return Q460_COLOR;
	else
		return OTHER_COLOR;

}

long ftol(double fVal)
{
	return (long)(fVal+0.5);
}
double CalSteelPlankWei(double volume)
{	
	return volume*7.85e-6;
}
double Distance(BOMPROFILE_VERTEX::COORD2D  pos1,BOMPROFILE_VERTEX::COORD2D pos2)
{
	return  sqrt(abs((pos2.x-pos1.x)*(pos2.x-pos1.x))+abs((pos2.y-pos1.y)*(pos2.y-pos1.y)));
}
//
CXhChar16 QuerySteelMatMark(char cMat)
{
	CXhChar16 sMatMark;
	if('H'==cMat)
		sMatMark.Copy("Q345B");
	else if('G'==cMat)
		sMatMark.Copy("Q390B");
	else if('P'==cMat)
		sMatMark.Copy("Q420B");
	else if('T'==cMat)
		sMatMark.Copy("Q460C");
	else if('S'==cMat)
		sMatMark.Copy("Q235B");
	return sMatMark;
}
CXhChar16 GetPartTypeName(BOMPART *pPart)
{
	BYTE cPartType=pPart->cPartType;
	CXhChar16 sPartType;
#ifdef AFX_TARG_ENU_ENGLISH
	if(cPartType==BOMPART::PLATE)		//钢板
	{
		if( pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FL||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLG||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLR)
			sPartType="Flange";
		else
			sPartType="Plate";
	}
	else if(cPartType==BOMPART::ANGLE)	//角钢
		sPartType="Angle";
	else if(cPartType==BOMPART::TUBE)	//钢管
		sPartType="Tube";
	else if(cPartType==BOMPART::SLOT)	//槽钢
		sPartType="U-steel";
	else if(cPartType==BOMPART::FLAT)	//扁钢
		sPartType="Flat Steel";
#else
	if(cPartType==BOMPART::PLATE)		//钢板
	{
		if( pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FL||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLG||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLR)
			sPartType="法兰";
		else if( pPart->siSubType==BOMPART::SUB_TYPE_PLATE_C||	//槽型插板
				 pPart->siSubType==BOMPART::SUB_TYPE_PLATE_U||	//U型插板
				 pPart->siSubType==BOMPART::SUB_TYPE_PLATE_X)	//十字插板
			sPartType="插板";
		else
			sPartType="钢板";
	}
	else if(cPartType==BOMPART::ANGLE)	//角钢
		sPartType="角钢";
	else if(cPartType==BOMPART::TUBE)	//钢管
		sPartType="钢管";
	else if(cPartType==BOMPART::SLOT)	//槽钢
		sPartType="槽钢";
	else if(cPartType==BOMPART::FLAT)	//扁钢
		sPartType="扁钢";
	else if(cPartType==BOMPART::ROUND)	//圆钢
	{
		sPartType="圆钢";
		if(pPart->sPartNo.GetLength()>0)
		{
			if(pPart->sPartNo.At(0)=='T')
				sPartType="爬梯";
			else if(strstr(pPart->sPartNo,"GJ-")||
					strstr(pPart->sPartNo,"NUT-")||
					strstr(pPart->sPartNo,"U-")||
					strstr(pPart->sPartNo,"NX-")||
					strstr(pPart->sPartNo,"JK-"))
				sPartType="拉索构件";
		}
	}
	CXhChar50 sSpec(pPart->sSpec);
	int hits=sSpec.Replace("φ"," ");
	hits+=sSpec.Replace("Φ"," ");
	if(hits>=1)
	{
		if( sSpec.Replace("/"," ")||sSpec.Replace("\\"," ")||
			sSpec.Replace("X"," ")||sSpec.Replace("x"," ")||
			sSpec.Replace("*"," ")||sSpec.Replace("×"," "))
			sPartType="钢管";
		else
			sPartType="圆钢";
	}
#endif
	return sPartType;
}
static bool IsCharInStr(char ch,char* prefix)
{
	if(prefix==NULL)
		return false;
	while(*prefix!='\0'&& ch!=*prefix)
	{
		prefix++;
		continue;
	}
	return (ch==*prefix);
}

int ComparePartNoString(const char *str1,const char *str2,char* materialPrefixCharSet/*=NULL*/)
{	
	//解析两个构件号为段号和流水号
	SEGI seg1,seg2;
	char serial_str1[16],serial_str2[16];
	char postfix1[8]={0},postfix2[8]={0};
	memset(serial_str1,0,16);
	memset(serial_str2,0,16);
	if(!ParsePartNo(str1,&seg1,serial_str1,materialPrefixCharSet)||!ParsePartNo(str2,&seg2,serial_str2,materialPrefixCharSet))
	{
		if(strlen(str1)==strlen(str2))
			return stricmp(str1,str2);
		else
			return strlen(str1)-strlen(str2);
	}
	size_t len1=strlen(serial_str1);
	size_t len2=strlen(serial_str2);
	int index=7;
	while(len1>0&&isdigit((BYTE)serial_str1[len1-1])==0)
	{	//处理流水号后面有多个字符的情况
		if(index>=0)
			postfix1[index]=serial_str1[len1-1];
		//else	//溢出,部分用户会出于出工艺卡原因直接把编号后面添加若干冗余字符(如空格) wjh-2017.6.19
		serial_str1[len1-1]=0;
		len1--;
		index--;
	}
	index=max(index,0);
	memmove(postfix1,&postfix1[index+1],7-index);//去掉前面的空字符
	postfix1[7-index]=0;
	index=7;
	while(len2>0&&isdigit((BYTE)serial_str2[len2-1])==0)
	{	//处理流水号后面有多个字符的情况
		if(index>=0)
			postfix2[index]=serial_str2[len2-1];
		//else	//溢出,部分用户会出于出工艺卡原因直接把编号后面添加若干冗余字符(如空格) wjh-2017.6.19
		serial_str2[len2-1]=0;
		len2--;
		index--;
	}
	index=max(index,0);
	memmove(postfix2,&postfix2[index+1],7-index);//去掉前面的空字符
	postfix2[7-index]=0;
	//先比段号
	if(seg1.Prefix().GetLength()==2||seg2.Prefix().GetLength()==2)
	{
		if(seg1.Prefix().GetLength()>seg2.Prefix().GetLength())
			return 1;
		else if(seg1.Prefix().GetLength()<seg2.Prefix().GetLength())
			return -1;
		int comp_seg=stricmp(seg1.Prefix(),seg2.Prefix());
		if(comp_seg!=0)
			return comp_seg;
	}
	if(seg1.Digital()>seg2.Digital())
		return 1;
	else if(seg1.Digital()<seg2.Digital())
		return -1;
	int comp_seg=stricmp(seg1.Prefix(),seg2.Prefix());
	if(comp_seg!=0)
		return comp_seg;
	//段号相同比流水号
	int comp_serial=0;
	if(strlen(serial_str1)==strlen(serial_str2))
		comp_serial=stricmp(serial_str1,serial_str2);
	else
		comp_serial=atoi(serial_str1)-atoi(serial_str2);
	if(postfix1[0]==0&&postfix2[0]==0)
		return comp_serial;
	else if(comp_serial==0)	//流水号相同比后缀
		return stricmp(postfix1,postfix2);
	else
		return comp_serial;
}

double GetPartZincCoatedArea(BOMPART *pBomPart)
{
	if(pBomPart->cPartType==BOMPART::PLATE||pBomPart->cPartType==BOMPART::ANGLE)
	{
		double fArea=0;
		if(pBomPart->cPartType==BOMPART::PLATE)
		{
			PART_PLATE *pPlate=(PART_PLATE*)pBomPart;
			fArea+=(pPlate->real_area*2);
			fArea+=(pPlate->fPerimeter*pPlate->thick);
		}
		else if(pBomPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE *pAngle=(PART_ANGLE*)pBomPart;
			fArea+=(4*pAngle->wide*pAngle->length)+((2*pAngle->wide*pAngle->thick)-pAngle->thick*pAngle->thick)*2;
		}
		const static double PI=3.1415926;
		for(int i=0;i<pBomPart->m_arrBoltRecs.GetSize();i++)
		{
			double d=pBomPart->m_arrBoltRecs[i].d+pBomPart->m_arrBoltRecs[i].hole_d_increment;
			fArea+=(PI*d*pBomPart->thick-PI*d*d*0.25*2);	//内壁面积减去上下面面积
		}
		fArea*=1e-6;	//mm2转换到m2
		return fArea;
	}
	else
		return 0;
}

int CorrectBomPartSegI(BOMPART *pBomPart)
{
	SEGI segI;
	if(pBomPart->iSeg==-1)
	{
		if(ParsePartNo(pBomPart->sPartNo,&segI,NULL))
			pBomPart->iSeg=segI;
	}
	segI=SEGI(pBomPart->iSeg);
	CXhChar50 sSpecialSpec; 
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if( pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_U||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_X||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_C)
		{
			sSpecialSpec.Copy(pBomPart->sPartNo);
			if(segI.Prefix().GetLength()>0&&sSpecialSpec.StartWith(segI.key.prefix[0]))
				pBomPart->iSeg=0;
		}
	}
	else if(pBomPart->cPartType==BOMPART::ACCESSORY||
		pBomPart->cPartType==BOMPART::SUB_TYPE_STAY_ROPE)	//错误将设置构件类型
	{
		sSpecialSpec.Copy(pBomPart->sPartNo);
		if(segI.Prefix().GetLength()>0&&sSpecialSpec.StartWith(segI.key.prefix[0]))
			pBomPart->iSeg=0;
		//if(pBomPart->siSubType==BOMPART::SUB_TYPE_STEEL_GRATING)
		//	continue;	//钢格栅单独出一张表
	}
	return pBomPart->iSeg;
}

bool IsRectPlate(BOMPART *pBomPart)
{
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
		if(pPlate->GetEdgeCount()==4)
		{	
			LIST_NODE<BOMPROFILE_VERTEX> *pNode1=pPlate->listVertex.EnumFirst(),
				*pNode2=pPlate->listVertex.EnumNext(),*pNode3=pPlate->listVertex.EnumNext(),
				*pNode4=pPlate->listVertex.EnumNext();
			if(Distance(pNode1->data.vertex,pNode2->data.vertex)-Distance(pNode3->data.vertex,pNode4->data.vertex)==0&&
				Distance(pNode2->data.vertex,pNode3->data.vertex)-Distance(pNode1->data.vertex,pNode4->data.vertex)==0&&
				Distance(pNode1->data.vertex,pNode3->data.vertex)-Distance(pNode2->data.vertex,pNode4->data.vertex)==0)
				return true;
			else
				return false;
		}
	}
	return false;
}