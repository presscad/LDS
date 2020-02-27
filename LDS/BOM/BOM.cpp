//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "BOM.h"
#include "math.h"
#include "LogFile.h"
//#include "f_ent.h"
//#include "f_alg_fun.h"

// ����ֵ: ��
#define      LOCAL_EPS_COS          0.999999999
#define      LOCAL_EPS              1e-9
// ���̼���ֵ: ��2 (2%�����)
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
	return LOCAL_ACOS(LocalDotProd(v1,v2));
}

typedef LIST_NODE<BOMPROFILE_VERTEX>* VERTEX_PTR;
/////////////////////////////////////////////////////////////////////////////
// COORD3D 
BOMPROFILE_VERTEX::COORD3D::COORD3D(double* coord, int dimension/*=3*/)
{
	if(coord!=NULL)
	{
		x=dimension>0?coord[0]:0;
		y=dimension>1?coord[1]:0;
		z=dimension>2?coord[2]:0;
	}
	else
		x=y=z=0;
}
//////////////////////////////////////////////////////////////////////////
// COORD2D
BOMPROFILE_VERTEX::COORD2D::COORD2D(double* coord, int dimension/*=2*/)
{
	if(coord!=NULL)
	{
		x=dimension>0?coord[0]:0;
		y=dimension>1?coord[1]:0;
	}
	else
		x=y=0;
}
//////////////////////////////////////////////////////////////////////////
// BOMPROFILE_VERTEX
BOMPROFILE_VERTEX::BOMPROFILE_VERTEX(const double* coord2d, int dimension/*=2*/)
{
	double x=0,y=0;
	if(coord2d!=NULL)
	{
		x=dimension>0?coord2d[0]:0;
		y=dimension>1?coord2d[1]:0;
	}
	BOMPROFILE_VERTEX::BOMPROFILE_VERTEX(x,y);
	//type=0;	//ֱ��
	//manu_space=0;
	//sector_angle=radius=0;
	//m_bWeldEdge=m_bRollEdge=false;
	//roll_edge_offset_dist=0;
	//local_point_y=0;
	//local_point_vec=0;
}
BOMPROFILE_VERTEX::BOMPROFILE_VERTEX(double x,double y)
{
	type=0;	//ֱ��
	vertex.x=x;
	vertex.y=y;
	manu_space=0;
	sector_angle=radius=0;
	m_bWeldEdge=m_bRollEdge=false;
	roll_edge_offset_dist=0;
	edge_len = 0;
	//local_point_y=0;
	//local_point_vec=0;
}
void BOMPROFILE_VERTEX::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.ReadByte(&type);
	buffer.ReadBooleanThin(&m_bWeldEdge);
	buffer.ReadBooleanThin(&m_bRollEdge);
	buffer.ReadWord(&roll_edge_offset_dist);
	buffer.ReadDouble(&radius);
	buffer.ReadDouble(&sector_angle);
	buffer.ReadWord(&manu_space);
	buffer.ReadDouble(&vertex.x);
	buffer.ReadDouble(&vertex.y);
	buffer.ReadPoint(center);
	buffer.ReadPoint(column_norm);
	buffer.ReadPoint(work_norm);
	buffer.ReadByte(&cStateFlag);
	buffer.ReadWord(&height);
	buffer.ReadWord(&length);
	buffer.ReadWord(&lenpos);
	if(version==NULL||version>=9)
		buffer.ReadString(sWeldMotherPart,16);
	if (version == NULL || version >= 12)
		buffer.ReadDouble(&edge_len);
}
void BOMPROFILE_VERTEX::ToBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.WriteByte(type);
	buffer.WriteBooleanThin(m_bWeldEdge);
	buffer.WriteBooleanThin(m_bRollEdge);
	buffer.WriteWord(roll_edge_offset_dist);
	buffer.WriteDouble(radius);
	buffer.WriteDouble(sector_angle);
	buffer.WriteWord(manu_space);
	buffer.WriteDouble(vertex.x);
	buffer.WriteDouble(vertex.y);
	buffer.WritePoint(center);
	buffer.WritePoint(column_norm);
	buffer.WritePoint(work_norm);
	buffer.WriteByte(cStateFlag);
	buffer.WriteWord(height);
	buffer.WriteWord(length);
	buffer.WriteWord(lenpos);
	if(version==NULL||version>=9)
		buffer.WriteString(sWeldMotherPart);
	if (version == NULL || version >= 12)
		buffer.WriteDouble(edge_len);
}
/////////////////////////////////////////////////////////////////////////////
// BOMBOLT
void BOMBOLT::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.ReadBooleanThin(&bFootNail);
	buffer.ReadBooleanThin(&bTwoCap);
	buffer.ReadWord(&d);
	buffer.ReadInteger(&nNoThreadlen);
	if(version==NULL||version>=3)
	{
		buffer.ReadBooleanThin(&bAntiTheft);
		buffer.ReadBooleanThin(&bAnitLoose);
	}
	if (version == NULL || version >= 11)
	{
		buffer.ReadByte(&nCommonNut);	//��ͨ��ĸ
		buffer.ReadByte(&nFastenNut);	//�۽���ĸ
		buffer.ReadByte(&nAntiTheftNut);//������ĸ
		buffer.ReadByte(&nThinNut);		//����ĸ
		buffer.ReadByte(&nCommonGasket);//ƽ��Ȧ
		buffer.ReadByte(&nSpringGasket);//���ɵ�Ȧ
		buffer.ReadByte(&nLockClasp);	//���ɿ�
		buffer.ReadString(sMaterial);	//����
	}
	buffer.ReadWord(&L);
	buffer.ReadWord(&L0);
	buffer.ReadWord(&hFamily);
	buffer.ReadFloat(&grade);
	buffer.ReadInteger(&iSeg);
	buffer.ReadFloat(&fUnitWeight);
	buffer.ReadInteger(&nPart);
}
void BOMBOLT::ToBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.WriteBooleanThin(bFootNail);
	buffer.WriteBooleanThin(bTwoCap);
	buffer.WriteWord(d);
	buffer.WriteInteger(nNoThreadlen);
	if(version==NULL||version>=3)
	{
		buffer.WriteBooleanThin(bAntiTheft);
		buffer.WriteBooleanThin(bAnitLoose);
	}
	if (version == NULL || version >= 11)
	{
		buffer.WriteByte(nCommonNut);	//��ͨ��ĸ
		buffer.WriteByte(nFastenNut);	//�۽���ĸ
		buffer.WriteByte(nAntiTheftNut);//������ĸ
		buffer.WriteByte(nThinNut);		//����ĸ
		buffer.WriteByte(nCommonGasket);//ƽ��Ȧ
		buffer.WriteByte(nSpringGasket);//���ɵ�Ȧ
		buffer.WriteByte(nLockClasp);	//���ɿ�
		buffer.WriteString(sMaterial);	//����
	}
	buffer.WriteWord(L);
	buffer.WriteWord(L0);
	buffer.WriteWord(hFamily);
	buffer.WriteFloat(grade);
	buffer.WriteInteger(iSeg);
	buffer.WriteFloat(fUnitWeight);
	buffer.WriteInteger(nPart);
}
CXhChar50 BOMBOLT::CombinedKey(bool bIncLsSegI/*=false*/)
{
	CXhChar50 key;
	CXhChar16 itemstr;
	//1.���ӹ��ϵ��
	key.Printf("%d-",hFamily==0?1:hFamily);
	if(hFamily>=0)	//ֻ����˨���ڼ������⣬��Ȧ�޼������
		itemstr.Printf("%.1f",grade);
	else
		itemstr.Append(sMaterial);
	//2.������˨������Ϣ
	key.Append(itemstr);
	//3.������˨�����Ϣ
	if(hFamily>=0)
		key.Append(bTwoCap?'m':'M');//Сдm��ʾ˫ñ
	itemstr.Printf("%dX%d",d,L);
	key.Append(itemstr);
	//4.���ӷֶ�ͳ����˨��Ϣ
	if(bIncLsSegI)
		key.Append(CXhChar16("-%d",iSeg));
	//5.���ӷ��������ɱ�ʶ
	if(hFamily>=0)
	{
		if(bAntiTheft)
			key.Append("-T");//T��ʾ����
		if(bAnitLoose)
			key.Append("-L");//L��ʾ����
	}
	return key;
}
//////////////////////////////////////////////////////////////////////////
// BOMBOLT_RECORD
void BOMBOLT_RECORD::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.ReadFloat(&x);
	buffer.ReadFloat(&y);
	buffer.ReadWord(&d);
	buffer.ReadFloat(&hole_d_increment);
	buffer.ReadWord(&waistLen);
	buffer.ReadDword(&m_dwFlag);
	if(version==NULL||version>=4)
		buffer.ReadByte(&cFuncType);
	if(version==NULL||version>=8)
		buffer.ReadByte(&cFlag);
}
void BOMBOLT_RECORD::ToBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.WriteFloat(x);
	buffer.WriteFloat(y);
	buffer.WriteWord(d);
	buffer.WriteFloat(hole_d_increment);
	buffer.WriteWord(waistLen);
	buffer.WriteDword(m_dwFlag);
	if(version==NULL||version>=4)
		buffer.WriteByte(cFuncType);
	if(version==NULL||version>=8)
		buffer.WriteByte(cFlag);
}
//////////////////////////////////////////////////////////////////////////
// BOMPART
BOMPART::BOMPART()
{
	nPart=0;
	cPartType=BOMPART::OTHER;
	siSubType=0;
	iSeg=0;
	cMaterial='S';
	cQualityLevel = 0;
	wide=thick=wingWideY=length=fPieceWeight=fSumWeight=0;
	siZhiWan=nM16Ls=nM20Ls=nM24Ls=nMXLs=nMSumLs=0;
	strcpy(sNotes,"");
	bWeldPart=bLegPart=FALSE;
	feature1=feature2=0;
	bWeldPart=bLegPart=FALSE;
}
short BOMPART::GetHuoquLineCount(BOOL bIncRollEdge/*=TRUE*/)
{
	return siZhiWan;
}
char* BOMPART::GetPartNo(int nMatCharPosType/*=0*/,char cMatSeparator/*=0*/, bool bIncQualityLevel /*= false*/)
{
	CXhChar16 szMaterial("%c", cMaterial);
	if (cQualityLevel != 0 && bIncQualityLevel)
		szMaterial.Append(cQualityLevel);
	CXhChar50 sPartNoIncMat;
	if (nMatCharPosType == 1 && cMaterial != 'S')
	{
		if (cMatSeparator == 0)		//�޼��
			sprintf(sPartNoIncMat, "%s%s", (char*)szMaterial, (char*)sPartNo);
		else if (cMatSeparator == ' ')	//�ո�
			sprintf(sPartNoIncMat, "%s %s", (char*)szMaterial, (char*)sPartNo);
		else
			sprintf(sPartNoIncMat, "%s%c%s", (char*)szMaterial, cMatSeparator, (char*)sPartNo);
	}
	else if (nMatCharPosType == 2 && cMaterial != 'S')
	{
		if (cMatSeparator == 0)		//�޼��
			sprintf(sPartNoIncMat, "%s%s", (char*)sPartNo, (char*)szMaterial);
		else if (cMatSeparator == ' ')	//�ո�
			sprintf(sPartNoIncMat, "%s %s", (char*)sPartNo, (char*)szMaterial);
		else
			sprintf(sPartNoIncMat, "%s%c%s", (char*)sPartNo, cMatSeparator, (char*)szMaterial);
	}
	else
		sprintf(sPartNoIncMat, "%s", (char*)sPartNo);
	return sPartNoIncMat;
}
CXhChar16 BOMPART::GetPartTypeName(BOOL bSymbol/*=FALSE*/)
{
	CXhChar16 sPartType;
#ifdef AFX_TARG_ENU_ENGLISH
	if(cPartType==BOMPART::PLATE)		//�ְ�
		sPartType=bSymbol?"-":"Plate";
	else if(cPartType==BOMPART::ANGLE)	//�Ǹ�
		sPartType=bSymbol?"��""Angle";
	else if(cPartType==BOMPART::TUBE)	//�ֹ�
		sPartType=bSymbol?"��""Tube";
	else if(cPartType==BOMPART::SLOT)	//�۸�
		sPartType="U-steel";
	else if(cPartType==BOMPART::FLAT)	//���
		sPartType="Flat Steel";
	else if(cPartType==BOMPART::ROUND)	//Բ��
		sPartType="Round Steel";
#else
	if(cPartType==BOMPART::PLATE)		//�ְ�
		sPartType=bSymbol?"-":"�ְ�";
	else if(cPartType==BOMPART::ANGLE)	//�Ǹ�
		sPartType=bSymbol?"��":"�Ǹ�";
	else if(cPartType==BOMPART::TUBE)	//�ֹ�
		sPartType=bSymbol?"��":"�ֹ�";
	else if(cPartType==BOMPART::SLOT)	//�۸�
		sPartType="�۸�";
	else if(cPartType==BOMPART::FLAT)	//���
		sPartType="���";
	else if(cPartType==BOMPART::ROUND)	//Բ��
		sPartType="Բ��";
	else if(cPartType==BOMPART::ACCESSORY)
	{
		if(siSubType==SUB_TYPE_LADDER)
			sPartType="����";
		else if(siSubType==SUB_TYPE_STEEL_GRATING)
			sPartType="�ָ�դ";
		else if(siSubType==SUB_TYPE_STAY_ROPE)
			sPartType="��������";
		else
			sPartType="����";
	}
#endif
	return sPartType;
}
CXhChar16 BOMPART::GetPartSubTypeName()
{
	CXhChar16 sSubType;
#ifdef AFX_TARG_ENU_ENGLISH
	if(siSubType==BOMPART::SUB_TYPE_TUBE_MAIN)
		sSubType="Main Tube";
	else if(siSubType==BOMPART::SUB_TYPE_TUBE_BRANCH)
		sSubType="Branch Tube";
	else if(siSubType==BOMPART::SUB_TYPE_PLATE_FL)
		sSubType="Flange";
	else if(siSubType==BOMPART::SUB_TYPE_PLATE_C
		||siSubType==BOMPART::SUB_TYPE_PLATE_U
		||siSubType==BOMPART::SUB_TYPE_PLATE_X)
		sSubType="Plug Plate";
	else if(siSubType==BOMPART::SUB_TYPE_PLATE_WATER)
		sSubType="Flashing Board";
	else if(siSubType==BOMPART::SUB_TYPE_FOOT_FL)
		sSubType="Bottom Flange";
	else if(siSubType==BOMPART::SUB_TYPE_ROD_Z)
		sSubType="Leg Members";
	else if(siSubType==BOMPART::SUB_TYPE_ROD_F)		
		sSubType="Ancillary Pole";
	else if(siSubType==BOMPART::SUB_TYPE_ANGLE_SHORT)
		sSubType="Short Angle";
#else 
	if(siSubType==BOMPART::SUB_TYPE_TUBE_MAIN)
		sSubType="����";
	else if(siSubType==BOMPART::SUB_TYPE_TUBE_BRANCH)
		sSubType="֧��";
	else if(siSubType==BOMPART::SUB_TYPE_PLATE_FL||
			siSubType==BOMPART::SUB_TYPE_PLATE_FLP||
			siSubType==BOMPART::SUB_TYPE_PLATE_FLG||
			siSubType==BOMPART::SUB_TYPE_PLATE_FLR)
		sSubType="����";
	else if(siSubType==BOMPART::SUB_TYPE_PLATE_FLD)
		sSubType="�߾�����";
	else if(siSubType==BOMPART::SUB_TYPE_PLATE_C
		||siSubType==BOMPART::SUB_TYPE_PLATE_U
		||siSubType==BOMPART::SUB_TYPE_PLATE_X)
		sSubType="���";
	else if(siSubType==BOMPART::SUB_TYPE_PLATE_WATER)
		sSubType="��ˮ��";
	else if(siSubType==BOMPART::SUB_TYPE_FOOT_FL)
		sSubType="�׷���";
	else if(siSubType==BOMPART::SUB_TYPE_ROD_Z)
		sSubType="����";
	else if(siSubType==BOMPART::SUB_TYPE_ROD_F)		
		sSubType="����";
	else if(siSubType==BOMPART::SUB_TYPE_ANGLE_SHORT)
		sSubType="����";
#endif
	return sSubType;
}
//��˨���ַ���
typedef struct tagHOLE_INFO
{
	int d;					//��˨ֱ��
	double hole_d_increment;//������
	int nCount;				//��ֱ����˨����
	tagHOLE_INFO(){d=nCount=0; hole_d_increment=0;}
}HOLE_INFO;
int BOMPART::GetLsHoleStr(char *hole_str, BOOL bIncUnit/*=TRUE*/)
{
	char sHoleStr[500]="";
	if(nMSumLs<=0)
	{
		if(hole_str)
			strcpy(hole_str,"");
		return 0;
	}
	int nHoleCount=0;
	CXhSimpleList<HOLE_INFO> holeList;
	LIST_NODE<HOLE_INFO> *pNode=NULL;
	for(int i=0;i<m_arrBoltRecs.GetSize();i++)
	{
		for(pNode=holeList.EnumFirst();pNode;pNode=holeList.EnumNext())
		{
			if(fabs(pNode->data.d+pNode->data.hole_d_increment-m_arrBoltRecs[i].d-m_arrBoltRecs[i].hole_d_increment)<0.0000009)
			{
				pNode->data.nCount++;
				break;
			}
		}
		if(pNode==NULL)
		{
			pNode=holeList.AttachNode();
			pNode->data.d=m_arrBoltRecs[i].d;
			pNode->data.hole_d_increment=m_arrBoltRecs[i].hole_d_increment;
			pNode->data.nCount=1;
			nHoleCount++;
		}
	}
	int nCount=0;
	for(pNode=holeList.EnumFirst();pNode;pNode=holeList.EnumNext())
	{
		CXhChar50 sCurHole,sD(pNode->data.d+pNode->data.hole_d_increment);
		if(bIncUnit)
			sCurHole.Printf("%smm*%d",(char*)sD,pNode->data.nCount);
		else 
			sCurHole.Printf("%s*%d",(char*)sD,pNode->data.nCount);
		strcat(sHoleStr,sCurHole);
		if(nCount<nHoleCount-1)
			strcat(sHoleStr,",");
		nCount++;
	}
	if(hole_str)
		strcpy(hole_str,sHoleStr);
	return strlen(sHoleStr);
}
int BOMPART::GetBoltOrHoleNum(double min_d,double max_d/*=-1*/,BOOL bHoleD/*=TRUE*/)
{
	if(min_d==-1)
		return 0;
	if(max_d==-1)
		max_d=min_d;
	int nLsNum=0;
	float TOLERANCE=0.1f;
	for(int i=0;i<m_arrBoltRecs.GetSize();i++)
	{
		double fHoleD=m_arrBoltRecs[i].d;
		if(bHoleD)
			fHoleD+=m_arrBoltRecs[i].hole_d_increment;
		//���ָ��ֱ������˨����
		if(min_d==0||(fHoleD>min_d-TOLERANCE&&fHoleD<max_d+TOLERANCE))
			nLsNum++;
	}
	return nLsNum;
}
//�Ƿ�����Բ��
BOOL BOMPART::IsHasLinearHole()
{
	if(m_arrBoltRecs.GetSize()<=0)
		return FALSE;
	for(BOMBOLT_RECORD* pBolt=m_arrBoltRecs.GetFirst();pBolt;pBolt=m_arrBoltRecs.GetNext())
	{
		if(pBolt->waistLen>0)
			return TRUE;
	}
	return FALSE;
}
//�Ƿ������⹦�ÿ�
BOOL BOMPART::IsHasSpecFuncHole()
{
	if(m_arrBoltRecs.GetSize()<=0)
		return FALSE;
	for(BOMBOLT_RECORD* pBolt=m_arrBoltRecs.GetFirst();pBolt;pBolt=m_arrBoltRecs.GetNext())
	{
		if(pBolt->cFuncType!=BOMBOLT_RECORD::FUNC_COMMON
			&&pBolt->cFuncType!=BOMBOLT_RECORD::FUNC_FOOTNAIL
			&&pBolt->cFuncType!=BOMBOLT_RECORD::FUNC_FOOTNAIL2)
			return TRUE;
	}
	return FALSE;
}
//�Ƿ������
BOOL BOMPART::IsDrillHole()
{
	if(m_arrBoltRecs.GetSize()<=0)
		return FALSE;
	for(BOMBOLT_RECORD* pBolt=m_arrBoltRecs.GetFirst();pBolt;pBolt=m_arrBoltRecs.GetNext())
	{
		if(pBolt->cFlag==1)
			return TRUE;
	}
	return FALSE;
}
BOOL BOMPART::IsExpandHole()
{
	if(m_arrBoltRecs.GetSize()<=0)
		return FALSE;
	for(BOMBOLT_RECORD* pBolt=m_arrBoltRecs.GetFirst();pBolt;pBolt=m_arrBoltRecs.GetNext())
	{
		if(pBolt->cFlag&0x02)
			return TRUE;
	}
	return FALSE;
}
void BOMPART::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.ReadInteger(&nPart);
	buffer.ReadByte(&cPartType);
	buffer.ReadString(sPartNo);
	buffer.ReadInteger(&iSeg);
	buffer.ReadWord(&siSubType);
	if (version == NULL || version >= 13)
		buffer.ReadString(sMaterial);
	buffer.ReadByte(&cMaterial);
	buffer.ReadDouble(&wide);
	buffer.ReadDouble(&thick);
	buffer.ReadDouble(&wingWideY);
	buffer.ReadString(sSpec);
	buffer.ReadDouble(&length);
	buffer.ReadDouble(&fPieceWeight);
	if(version==NULL||version>=10)
		buffer.ReadDouble(&fSumWeight);
	buffer.ReadWord(&siZhiWan);
	buffer.ReadInteger(&bWeldPart);
	buffer.ReadInteger(&bLegPart);
	buffer.ReadWord(&nM16Ls);
	buffer.ReadWord(&nM20Ls);
	buffer.ReadWord(&nM24Ls);
	buffer.ReadWord(&nMXLs);
	buffer.ReadWord(&nMSumLs);
	buffer.ReadString(sNotes);
	//��˨��Ϣ
	int nBoltRecCount=0;
	buffer.ReadInteger(&nBoltRecCount);
	for(int i=0;i<nBoltRecCount;i++)
	{
		BOMBOLT_RECORD *pRec=m_arrBoltRecs.append();
		pRec->FromBuffer(buffer,version);
	}
}
void BOMPART::ToBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.WriteInteger(nPart);
	buffer.WriteByte(cPartType);
	buffer.WriteString(sPartNo);
	buffer.WriteInteger(iSeg);
	buffer.WriteWord(siSubType);
	if (version == NULL || version >= 13)
		buffer.WriteString(sMaterial);
	buffer.WriteByte(cMaterial);
	buffer.WriteDouble(wide);
	buffer.WriteDouble(thick);
	buffer.WriteDouble(wingWideY);
	buffer.WriteString(sSpec);
	buffer.WriteDouble(length);
	buffer.WriteDouble(fPieceWeight);
	if(version==NULL||version>=10)
		buffer.WriteDouble(fSumWeight);
	buffer.WriteWord(siZhiWan);
	buffer.WriteInteger(bWeldPart);
	buffer.WriteInteger(bLegPart);
	buffer.WriteWord(nM16Ls);
	buffer.WriteWord(nM20Ls);
	buffer.WriteWord(nM24Ls);
	buffer.WriteWord(nMXLs);
	buffer.WriteWord(nMSumLs);
	buffer.WriteString(sNotes);
	//��˨��Ϣ
	buffer.WriteInteger(m_arrBoltRecs.GetSize());
	for(int i=0;i<m_arrBoltRecs.GetSize();i++)
		m_arrBoltRecs[i].ToBuffer(buffer,version);
}
//////////////////////////////////////////////////////////////////////////
// PART_PLATE
PART_PLATE::PART_PLATE()
{
	cPartType=BOMPART::PLATE;
	m_cFaceN=1;
	bNeedFillet=FALSE;
	m_iPlateType=0;
	huoQuAngleArr[0]=huoQuAngleArr[1]=0;
	fPieceNetWeight=min_area=real_area=fPerimeter=0;
	fWeldLineLen=fWeldEdgeLen=0;
	fFlInnerD=fFlOutterD=fFlBoltLayoutD=0;
	fCutMouthLen = 0;
}
double PART_PLATE::CalZincCoatedArea(BOOL bSwitchToM/*=TRUE*/)
{
	double fArea=real_area*2;
	fArea+=(fPerimeter*thick);
	const double PAI = 3.14159265358979323846;
	for(int i=0;i<m_arrBoltRecs.GetSize();i++)
	{
		double hole_d=m_arrBoltRecs[i].d+m_arrBoltRecs[i].hole_d_increment;
		double hole_r=hole_d*0.5;
		double hole_area= PAI *hole_d*thick- PAI *hole_r*hole_r*2;	//�ڱ������ȥ���������
		fArea+=hole_area;	
	}
	if(bSwitchToM)
		fArea*=1e-6;	//mm2ת����m2
	return fArea;
}
long PART_PLATE::GetEdgeCount()
{
	long nVertexCount=0;
	for(VERTEX_PTR pVertex=listVertex.EnumFirst();pVertex;pVertex=listVertex.EnumNext())
		nVertexCount++;
	return nVertexCount;
}
short PART_PLATE::CutAngleN()
{
	if(siSubType==SUB_TYPE_BOLT_PAD&&GetEdgeCount()>4)
		return 1;	//��˨����������4����Ҫ�н�
	else
		return 0;
}
long PART_PLATE::GetArcEdgeCount()
{
	long nVertexCount=0;
	for(VERTEX_PTR pVertex=listVertex.EnumFirst();pVertex;pVertex=listVertex.EnumNext())
	{
		if(pVertex->data.type>0)
			nVertexCount++;
	}
	return nVertexCount;
}
BOOL PART_PLATE::IsFL()
{
	return  SUB_TYPE_FOOT_FL==siSubType||SUB_TYPE_PLATE_FL==siSubType||SUB_TYPE_PLATE_FLD==siSubType||
			SUB_TYPE_PLATE_FLP==siSubType||SUB_TYPE_PLATE_FLG==siSubType||SUB_TYPE_PLATE_FLR==siSubType;
}
BOOL PART_PLATE::IsRect()
{
	if(siSubType==SUB_TYPE_BOLT_PAD)	//��������˨���
		return FALSE;
	
	int nNum=GetEdgeCount();
	ARRAY_LIST<BOMPROFILE_VERTEX::COORD3D> ptArr;
	ptArr.SetSize(nNum);
	for(VERTEX_PTR pVertex=listVertex.EnumFirst();pVertex;pVertex=listVertex.EnumNext())
		ptArr.append(BOMPROFILE_VERTEX::COORD3D(pVertex->data.vertex));
	BOOL bRigntAngle=TRUE;
	BOMPROFILE_VERTEX::COORD3D vec1,vec2;
	for(int i=0;i<ptArr.GetSize();i++)
	{
		vec1.x = ptArr[(i - 1 + nNum) % nNum].x - ptArr[i].x;
		vec1.y = ptArr[(i - 1 + nNum) % nNum].y - ptArr[i].y;
		vec1.z = ptArr[(i - 1 + nNum) % nNum].z - ptArr[i].z;
		vec1 = NormalizedVector(vec1);
		vec2.x = ptArr[(i + 1) % nNum].x - ptArr[i].x;
		vec2.y = ptArr[(i + 1) % nNum].y - ptArr[i].y;
		vec2.z = ptArr[(i + 1) % nNum].z - ptArr[i].z;
		vec2 = NormalizedVector(vec2);
		if(fabs(LocalDotProd(vec1,vec2))> LOCAL_EPS_COS)
			continue;
		if(fabs(LocalDotProd(vec1, vec2))> LOCAL_EPS2)
			bRigntAngle=FALSE;
	}
	return bRigntAngle;
}
short PART_PLATE::GetHuoquLineCount(BOOL bIncRollEdge/*=TRUE*/)
{
	short nHuoquLineCount=max(0,m_cFaceN-1);
	if(bIncRollEdge)
	{
		for(VERTEX_PTR pVertex=listVertex.EnumFirst();pVertex;pVertex=listVertex.EnumNext())
		{
			if(pVertex->data.m_bRollEdge)
				nHuoquLineCount++;
		}
	}
	return nHuoquLineCount;
}
double PART_PLATE::GetHuoquDegree(int index)
{
	if(index<0||index>1)
		return 0;
	else
		return huoQuAngleArr[index];
}
CXhChar16 PART_PLATE::GetSpec()
{
	CXhChar16 ss("%s",(char*)sSpec);
	if(cPartType==BOMPART::PLATE&&siSubType!=BOMPART::SUB_TYPE_PLATE_WATER
		&&siSubType!=BOMPART::SUB_TYPE_PLATE_FL&&siSubType!=BOMPART::SUB_TYPE_FOOT_FL
		&&siSubType!=BOMPART::SUB_TYPE_PLATE_FLD&&siSubType!=BOMPART::SUB_TYPE_PLATE_FLP
		&&siSubType!=BOMPART::SUB_TYPE_PLATE_FLG&&siSubType!=BOMPART::SUB_TYPE_PLATE_FLR)
		ss.Printf("-%.f",thick);
	return ss;
}
void PART_PLATE::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	BOMPART::FromBuffer(buffer,version);
	//
	buffer.ReadByte(&m_cFaceN);
	buffer.ReadInteger(&bNeedFillet);
	if(version==NULL||version>=6)
	{
		buffer.ReadInteger(&m_iPlateType);
		buffer.ReadDouble(&huoQuAngleArr[0]);
		buffer.ReadDouble(&huoQuAngleArr[1]);
	}
	buffer.ReadDouble(&fPieceNetWeight);
	buffer.ReadDouble(&min_area);
	buffer.ReadDouble(&real_area);
	buffer.ReadDouble(&fPerimeter);
	buffer.ReadDouble(&fWeldEdgeLen);
	if(version==NULL||version>=2)
	{
		buffer.ReadDouble(&fFlInnerD);
		buffer.ReadDouble(&fFlOutterD);
		buffer.ReadDouble(&fFlBoltLayoutD);
	}
	if(version==NULL||version>=8)
		buffer.ReadDouble(&fWeldLineLen);	//���쳤��
	if (version == NULL || version >= 12)
		buffer.ReadDouble(&fCutMouthLen);	//��ڳ���
	//��������Ϣ
	int nVertexCount=0;
	buffer.ReadInteger(&nVertexCount);
	for(int i=0;i<nVertexCount;i++)
	{
		VERTEX_PTR pVertex=listVertex.AttachNode();
		pVertex->data.FromBuffer(buffer,version);
	}
}
void PART_PLATE::ToBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	BOMPART::ToBuffer(buffer,version);
	//
	buffer.WriteByte(m_cFaceN);
	buffer.WriteInteger(bNeedFillet);
	if(version==NULL||version>=6)
	{
		buffer.WriteInteger(m_iPlateType);
		buffer.WriteDouble(huoQuAngleArr[0]);
		buffer.WriteDouble(huoQuAngleArr[1]);
	}
	buffer.WriteDouble(fPieceNetWeight);
	buffer.WriteDouble(min_area);
	buffer.WriteDouble(real_area);
	buffer.WriteDouble(fPerimeter);
	buffer.WriteDouble(fWeldEdgeLen);
	if(version==NULL||version>=2)
	{
		buffer.WriteDouble(fFlInnerD);
		buffer.WriteDouble(fFlOutterD);
		buffer.WriteDouble(fFlBoltLayoutD);
	}
	if(version==NULL||version>=8)
		buffer.WriteDouble(fWeldLineLen);
	if (version == NULL || version >= 12)
		buffer.WriteDouble(fCutMouthLen);
	//��������Ϣ
	int level=buffer.PushPositionStack();
	buffer.ClearLevelCount(level);
	buffer.WriteInteger(0);	//���������
	for(VERTEX_PTR pVertex=listVertex.EnumFirst();pVertex;pVertex=listVertex.EnumNext())
	{
		pVertex->data.ToBuffer(buffer,version);
		buffer.IncrementLevelCount(level);
	}
	buffer.PopPositionStack();
	buffer.WriteInteger(buffer.LevelCount(level));
	buffer.RestoreNowPosition();
}
//////////////////////////////////////////////////////////////////////////
// PART_ANGLE
PART_ANGLE::PART_ANGLE()
{
	cPartType=BOMPART::ANGLE;
	fWeldLineLen=wing_angle=0;	//fWeldLineLen�ں����ʱ����֡�double->floatת����ʾ��Ϣ wjh-2018.1.15
	bHasFootNail=bCutAngle=bCutBer=bCutRoot=bKaiJiao=bHeJiao=FALSE;
	nPushFlat=0;
	memset(cut_wing,0,sizeof(int)*2);
	memset(cut_wing_para,0,sizeof(int)*6);
	memset(cut_angle,0,sizeof(int)*8);
}
bool PART_ANGLE::IsCutAngleOrWing()
{
	double cut_angle_value= cut_angle[0][0]+cut_angle[0][1]+
				cut_angle[1][0]+cut_angle[1][1]+
				cut_angle[2][0]+cut_angle[2][1]+
				cut_angle[3][0]+cut_angle[3][1];
	if(cut_wing[0]>0||cut_wing[1]>0||cut_angle_value>0)
		return true;
	else
		return false;
}
bool PART_ANGLE::IsStartCutAngleOrWing()
{
	double cut_angle_value= cut_angle[0][0]+cut_angle[0][1]+
		cut_angle[1][0]+cut_angle[1][1];
	if(cut_wing[0]>0||cut_angle_value>0)
		return true;
	else
		return false;
}
bool PART_ANGLE::IsEndCutAngleOrWing()
{
	double cut_angle_value= cut_angle[2][0]+cut_angle[2][1]+
		cut_angle[3][0]+cut_angle[3][1];
	if(cut_wing[1]>0||cut_angle_value>0)
		return true;
	else
		return false;
}
//0.������ 1.���� 2.���� 3.������ 4.������
BYTE PART_ANGLE::GetHuoquType(int *huoqu_wing_x0_y1_all2/*=NULL*/)
{
	if(listPolyAngle.EnumFirst()==NULL)
		return 0;
	LIST_NODE<POLY_ANGLE_INFO> *pNode=listPolyAngle.EnumNext();
	if(pNode==NULL)
		return 0;
	BYTE cType=0;
	BOMPROFILE_VERTEX::COORD3D len_vec=pNode->data.len_vec;
	if(len_vec.x>0&&len_vec.y>0)
	{	//����
		if( (len_vec.x<LOCAL_EPS2&&len_vec.y>LOCAL_EPS2)||
			(len_vec.y<LOCAL_EPS2&&len_vec.x>LOCAL_EPS2))
			cType=1;//����
		else
			cType=3;//������
	}
	else if(len_vec.x<0&&len_vec.y<0)
		cType=4;	//������
	else 
		cType=2;	//����
	return cType;
}

double PART_ANGLE::GetHuoquDegree(int index)
{
	if(listPolyAngle.Count<0||index>(int)listPolyAngle.Count-1)
		return 0;
	typedef POLY_ANGLE_INFO* ANGLE_PTR;
	ANGLE_PTR ptrArr[10]={NULL};
	int i=0;
	POLY_ANGLE_INFO *pPolyAngle=NULL,*pPrevPolyAngle=NULL;
	for(pPolyAngle=listPolyAngle.MoveFirst();pPolyAngle;pPolyAngle=listPolyAngle.MoveNext())
	{
		ptrArr[i]=pPolyAngle;
		i++;
		if(i==10)
			break;
	}
	if(index>9)
		return 0;
	pPrevPolyAngle=ptrArr[index];
	pPolyAngle=ptrArr[index+1];
	BOMPROFILE_VERTEX::COORD3D len_vec=pPolyAngle->len_vec;
	NormalizedVector(len_vec);
	BOMPROFILE_VERTEX::COORD3D prev_wing_norm_x(pPrevPolyAngle->norm_x_wing),prev_wing_norm_y(pPrevPolyAngle->norm_y_wing);
	BOMPROFILE_VERTEX::COORD3D wing_norm_x(pPolyAngle->norm_x_wing),wing_norm_y(pPolyAngle->norm_y_wing);
	double angle=0;
	if(fabs(len_vec.y)> LOCAL_EPS)
	{
		if(len_vec.y>0)	//����
			angle = LOCAL_ACOS(prev_wing_norm_x,wing_norm_x);
		else
			angle =-LOCAL_ACOS(prev_wing_norm_x,wing_norm_x);
	}
	else 
	{
		if(len_vec.x>0)	//����
			angle = LOCAL_ACOS(prev_wing_norm_y, wing_norm_y);
		else
			angle =-LOCAL_ACOS(prev_wing_norm_y, wing_norm_y);
	}
	angle*=LOCAL_DEGTORAD_COEF;
	return angle;
}
short PART_ANGLE::CutAngleN()
{
	short nCutCount=0;
	for(int i=0;i<2;i++)
	{	//0.ʼ�� 1.�ն�
		if(cut_wing[i]>0&&cut_wing_para[i][0]>0)
		{	//��֫
			if(cut_wing_para[i][1]==0)
				nCutCount++;		//�п�֫(��1��)
			else 
				nCutCount+=3;		//��֫(��3��)
		}
		if(cut_angle[i*2][0]>0)		//X֫�н�(��1��)
			nCutCount++;
		if(cut_angle[i*2+1][0]>0)	//Y֫�н�(��1��)
			nCutCount++;
	}
	return nCutCount;
}
short PART_ANGLE::KaiHeJiaoN()
{
	short siKaiHeJiaoCount=0;
	//ͳ�ƽǸֿ��Ͻ�λ�õĸ���:��1��500mm����Ϊ1��500��1000mm��Ϊ2��1000 ��1500mm��Ϊ3��	//��
	//1��440mm����Ϊ1�� 441��540��Ϊ2�� 541��1000��Ϊ3��1001��1500��Ϊ4��1501��2000��Ϊ5��2001��2500��Ϊ6��2501��3000��Ϊ7�� 
	//���ݷɽ��±�׼ wht 15-12-25
	for(LIST_NODE<KAI_HE_JIAO> *pNode=listKaihe.EnumFirst();pNode;pNode=listKaihe.EnumNext())
	{
		short siLen=pNode->data.startLength+pNode->data.endLength;
		if(siLen>=1&&siLen<=440)
			siKaiHeJiaoCount+=1;
		else if(siLen>=441&&siLen<=540)
			siKaiHeJiaoCount+=2;
		else if(siLen>=541&&siLen<=1000)
			siKaiHeJiaoCount+=3;
		else if(siLen>1000)
		{
			short siOtherLen=siLen-1000;
			siKaiHeJiaoCount=3;
			short siCount=siOtherLen/500;
			if(siOtherLen%500>0)
				siCount+=1;
			siKaiHeJiaoCount+=siCount;
		}
	}
	return siKaiHeJiaoCount;
}
CXhChar16 PART_ANGLE::GetSpec()
{
	CXhChar16 ss;
	if(wingWideY>0&&wingWideY!=wide)
		ss.Printf("��%.fx%.fx%.f",wide,wingWideY,thick);
	else 
		ss.Printf("��%.fx%.f",wide,thick);
	return ss;
}
double PART_ANGLE::CalZincCoatedArea(BOOL bSwitchToM/*=TRUE*/)
{
	double fArea=4*wide*length;
	fArea+=((2*wide*thick)-thick*thick)*2;
	for(int i=0;i<m_arrBoltRecs.GetSize();i++)
	{
		double hole_d=m_arrBoltRecs[i].d+m_arrBoltRecs[i].hole_d_increment;
		double hole_r=hole_d*0.5;
		double hole_area= LOCAL_PI *hole_d*thick- LOCAL_PI *hole_r*hole_r*2;	//�ڱ������ȥ���������
		fArea+=hole_area;	
	}
	if(bSwitchToM)
		fArea*=1e-6;	//mm2ת����m2
	return fArea;
}
void PART_ANGLE::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	BOMPART::FromBuffer(buffer,version);
	//
	buffer.ReadFloat(&wing_angle);
	buffer.ReadInteger(&bCutAngle);
	buffer.ReadInteger(&bCutBer);
	buffer.ReadInteger(&bCutRoot);
	buffer.ReadInteger(&bKaiJiao);
	buffer.ReadInteger(&bHeJiao);
	if(version==NULL||version>=5)
		buffer.ReadInteger(&bHasFootNail);
	buffer.ReadInteger(&nPushFlat);
	buffer.ReadInteger(&cut_wing[0]);
	buffer.ReadInteger(&cut_wing[1]);
	buffer.Read(cut_wing_para,6*sizeof(int));
	buffer.Read(cut_angle,8*sizeof(int));
	if(version==NULL||version>=8)
		buffer.ReadDouble(&fWeldLineLen);	//���쳤��
	//���Ͻ�
	int nKaiHeJiao=0;
	buffer.ReadInteger(&nKaiHeJiao);
	listKaihe.DeleteList();
	for(int i=0;i<nKaiHeJiao;i++)
	{
		LIST_NODE<KAI_HE_JIAO> *pNode=listKaihe.AttachNode();
		buffer.ReadFloat(&pNode->data.decWingAngle);
		buffer.ReadWord(&pNode->data.position);
		buffer.ReadWord(&pNode->data.startLength);
		buffer.ReadWord(&pNode->data.endLength);
	}
	if(version==NULL||version>=2)
	{	//����Ǹ��ӽǸ�
		int nPolyAngle=buffer.ReadInteger();
		listPolyAngle.DeleteList();
		for(int i=0;i<nPolyAngle;i++)
		{
			LIST_NODE<POLY_ANGLE_INFO> *pNode=listPolyAngle.AttachNode();
			buffer.ReadInteger(&pNode->data.nIndex);
			buffer.ReadPoint(pNode->data.len_vec);
			buffer.ReadPoint(pNode->data.norm_x_wing);
			buffer.ReadPoint(pNode->data.norm_y_wing);
		}
	}
	if (version == NULL || version >= 12)
	{	//����Ǹ��ӽǸ�
		int nPushFlatCount = buffer.ReadInteger();
		listPolyAngle.DeleteList();
		for (int i = 0; i < nPushFlatCount; i++)
		{
			LIST_NODE<POLY_ANGLE_INFO> *pNode = listPolyAngle.AttachNode();
			buffer.ReadInteger(&pNode->data.nIndex);
			buffer.ReadPoint(pNode->data.len_vec);
			buffer.ReadPoint(pNode->data.norm_x_wing);
			buffer.ReadPoint(pNode->data.norm_y_wing);
		}
	}
}
void PART_ANGLE::ToBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	BOMPART::ToBuffer(buffer,version);
	//
	buffer.WriteFloat(wing_angle);
	buffer.WriteInteger(bCutAngle);
	buffer.WriteInteger(bCutBer);
	buffer.WriteInteger(bCutRoot);
	buffer.WriteInteger(bKaiJiao);
	buffer.WriteInteger(bHeJiao);
	if(version==NULL||version>=5)
		buffer.WriteInteger(bHasFootNail);
	buffer.WriteInteger(nPushFlat);
	buffer.WriteInteger(cut_wing[0]);
	buffer.WriteInteger(cut_wing[1]);
	buffer.Write(cut_wing_para,6*sizeof(int));
	buffer.Write(cut_angle,8*sizeof(int));
	if(version==NULL||version>=8)
		buffer.WriteDouble(fWeldLineLen);
	//���Ͻ�
	int level=buffer.PushPositionStack();
	buffer.ClearLevelCount(level);
	buffer.WriteInteger(0);	//���������
	for(LIST_NODE<KAI_HE_JIAO> *pNode=listKaihe.EnumFirst();pNode;pNode=listKaihe.EnumNext())
	{
		buffer.WriteFloat(pNode->data.decWingAngle);
		buffer.WriteWord(pNode->data.position);
		buffer.WriteWord(pNode->data.startLength);
		buffer.WriteWord(pNode->data.endLength);
		buffer.IncrementLevelCount(level);
	}
	buffer.PopPositionStack();
	buffer.WriteInteger(buffer.LevelCount(level));
	buffer.RestoreNowPosition();
	if(version==NULL||version>=2)
	{	//����Ǹ��ӽǸ�
		BUFFERPOP stack(&buffer,0);
		buffer.WriteInteger(0);
		for (LIST_NODE<POLY_ANGLE_INFO> *pNode=listPolyAngle.EnumFirst();pNode;pNode=listPolyAngle.EnumNext())
		{
			buffer.WriteInteger(pNode->data.nIndex);
			buffer.WritePoint(pNode->data.len_vec);
			buffer.WritePoint(pNode->data.norm_x_wing);
			buffer.WritePoint(pNode->data.norm_y_wing);
			stack.Increment();
		}
		stack.VerifyAndRestore();
	}
	if (version == NULL || version >= 12)
	{
		BUFFERPOP stack(&buffer, 0);
		buffer.WriteInteger(0);
		for (ANGLE_PUSH_FLAT *pPushFlat = listPushFlat.EnumObjectFirst(); pPushFlat; pPushFlat = listPushFlat.EnumObjectNext())
		{
			buffer.WriteWord(pPushFlat->flat_start_pos);
			buffer.WriteWord(pPushFlat->flat_end_pos);
			buffer.WriteWord(pPushFlat->push_flat_x1_y2);
			stack.Increment();
		}
		stack.VerifyAndRestore();
	}
}
//////////////////////////////////////////////////////////////////////////
// PART_TUBE
PART_TUBE::PART_TUBE()
{
	cPartType=BOMPART::TUBE;
	bHasBranchTube=FALSE;
	bHasNodePlate=FALSE;
	bHasFootNail=FALSE;
	bHasWeldRoad=FALSE;

}
double PART_TUBE::CalZincCoatedArea(BOOL bSwitchToM/*=TRUE*/)
{
	double fOuterD=wide;
	double fOuterR=fOuterD*0.5;
	double fInnerD=wide-thick*2;
	double fInnerR=fInnerD*0.5;
	double fArea=LOCAL_PI*(fOuterD+fInnerD)*length;
	fArea+=2*(LOCAL_PI*fOuterD*fOuterD-LOCAL_PI*fInnerR*fInnerR);
	if(bSwitchToM)
		fArea*=1e-6;	//mm2ת����m2
	return fArea;
}
CXhChar16 PART_TUBE::GetPartTypeName(BOOL bSymbol/*=FALSE*/)
{
	CXhChar16 sPartType;
	if(bSymbol)
		sPartType.Copy("��");
	else
	{
		if(bHasWeldRoad)
			sPartType.Copy("ֱ��ֹ�");
		else
			sPartType.Copy("�޷�ֹ�");
	}
	return sPartType;
}
void PART_TUBE::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	BOMPART::FromBuffer(buffer,version);
	if(version==NULL||version>=2)
	{	//ʼ�˹�����Ϣ
		buffer.ReadByte(&startProcess.type);
		buffer.ReadDouble(&startProcess.L);
		buffer.ReadDouble(&startProcess.A);
		buffer.ReadDouble(&startProcess.B);
		buffer.ReadDouble(&startProcess.fNormOffset);
		buffer.ReadString(startProcess.sSpec);
		//�ն˹�����Ϣ
		buffer.ReadByte(&endProcess.type);
		buffer.ReadDouble(&endProcess.L);
		buffer.ReadDouble(&endProcess.A);
		buffer.ReadDouble(&endProcess.B);
		buffer.ReadDouble(&endProcess.fNormOffset);
		buffer.ReadString(endProcess.sSpec);
	}
	if(version==NULL||version>=2)
	{
		buffer.ReadInteger(&bHasNodePlate);
		buffer.ReadInteger(&bHasBranchTube);
	}
	if(version==NULL||version>=7)
	{
		buffer.ReadInteger(&bHasFootNail);
		buffer.ReadInteger(&bHasWeldRoad);
	}
}
void PART_TUBE::ToBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	BOMPART::ToBuffer(buffer,version);
	if(version==NULL||version>=2)
	{	//ʼ�˹�����Ϣ
		buffer.WriteByte(startProcess.type);
		buffer.WriteDouble(startProcess.L);
		buffer.WriteDouble(startProcess.A);
		buffer.WriteDouble(startProcess.B);
		buffer.WriteDouble(startProcess.fNormOffset);
		buffer.WriteString(startProcess.sSpec);
		//�ն˹�����Ϣ
		buffer.WriteByte(endProcess.type);
		buffer.WriteDouble(endProcess.L);
		buffer.WriteDouble(endProcess.A);
		buffer.WriteDouble(endProcess.B);
		buffer.WriteDouble(endProcess.fNormOffset);
		buffer.WriteString(endProcess.sSpec);
	}
	if(version==NULL||version>=2)
	{
		buffer.WriteInteger(bHasNodePlate);
		buffer.WriteInteger(bHasBranchTube);
	}
	if(version==NULL||version>=7)
	{
		buffer.WriteInteger(bHasFootNail);
		buffer.WriteInteger(bHasWeldRoad);
	}
}
//��ȡ���ڲ�����
//nRuleType �������ͱ�Ź���
//nRuleType==0 ���ţ�LRM(������) 
//				Mλ: (�ֹ��м����ӷ�ʽ) 0.������ 1.�ڵ�� 2.���ֹ� 3.�ڵ������ֹ�
//			  L,Rλ: (�ֹ����˿��۷�ʽ) 1.���Ͳ�� 2.U�Ͳ�� 3.T�Ͳ�� 4.X�Ͳ�� 5.FL 6.��� 7.ƽ���и�
//nRuleType==1 ���ţ�LMR(������) 
//				Mλ: (�ֹ��м����ӷ�ʽ) 0.������ 1.�ڵ�� 2.���ֹ�
//			  L,Rλ: (�ֹ����˿��۷�ʽ) 1.���Ͳ�� 2.U�Ͳ�� 3.T�Ͳ�� 4.X�Ͳ�� 5.FL
int PART_TUBE::GetNotchType(char *type,int nRuleType)
{
	int nType=-1;
	//ƽ���и� ����������ˮƽ���������η�����Խ����
	if(startProcess.type>0&&startProcess.type<8
		&&endProcess.type>0&&endProcess.type<8)
	{	//���˿���
		if(startProcess.type==endProcess.type)
		{	//���˿�������һ��
			if(startProcess.type==TUBE_PROCESS::PROCESSTYPE_CSLOT)
				nType=11;	//֧�� ��ͷC�Ͳ��
			else if(startProcess.type==TUBE_PROCESS::PROCESSTYPE_USLOT)
				nType=22;	//֧�� ��ͷU�Ͳ��
			else if(startProcess.type==TUBE_PROCESS::PROCESSTYPE_TSLOT)
				nType=33;	//֧�� ��ͷT�Ͳ��
			else if(startProcess.type==TUBE_PROCESS::PROCESSTYPE_XSLOT)
				nType=44;	//֧�� ��ͷX�Ͳ��
			else if(startProcess.IsFL())
				nType=55;	//֧�� ��ͷ����
			else if(nRuleType==0)
			{	//LRM
				if(startProcess.type==TUBE_PROCESS::PROCESSTYPE_TRANSECT)
					nType=66;	//֧�� ��ͷ���
				else if(startProcess.type==TUBE_PROCESS::PROCESSTYPE_PLANECUT)
					nType=77;	//֧�� ��ͷƽ���и�
			}
		}
		else if(startProcess.type==TUBE_PROCESS::PROCESSTYPE_CSLOT||endProcess.type==TUBE_PROCESS::PROCESSTYPE_CSLOT)
		{	//�������Ӳ�ͬ һ��Ϊ���Ͳ��
			TUBE_PROCESS *pAnotherProcess=&endProcess;
			if(endProcess.type==TUBE_PROCESS::PROCESSTYPE_CSLOT)
				pAnotherProcess=&startProcess;
			if(pAnotherProcess->type==TUBE_PROCESS::PROCESSTYPE_USLOT)
				nType=12;	//֧�� һ��C�Ͳ��һ��U�Ͳ��
			else if(pAnotherProcess->type==TUBE_PROCESS::PROCESSTYPE_TSLOT)
				nType=13;	//֧�� һ��C�Ͳ��һ��T�Ͳ��
			else if(pAnotherProcess->type==TUBE_PROCESS::PROCESSTYPE_XSLOT)
				nType=14;	//֧�� һ��C�Ͳ��һ��X�Ͳ��
			else if(pAnotherProcess->IsFL())
				nType=15;	//֧�� һ��C�Ͳ��һ�˷���
			else if(nRuleType==0)
			{	//LRM
				if(pAnotherProcess->type==TUBE_PROCESS::PROCESSTYPE_TRANSECT)
					nType=16;	//֧�� һ��C�Ͳ��һ�����
				else if(pAnotherProcess->type==TUBE_PROCESS::PROCESSTYPE_PLANECUT)
					nType=17;	//֧�� һ��C�Ͳ��һ��ƽ���и�
			}
		}
		else if(startProcess.type==TUBE_PROCESS::PROCESSTYPE_USLOT||endProcess.type==TUBE_PROCESS::PROCESSTYPE_USLOT)
		{	//�������Ӳ�ͬ һ��ΪU�Ͳ��
			TUBE_PROCESS *pAnotherProcess=&endProcess;
			if(endProcess.type==TUBE_PROCESS::PROCESSTYPE_USLOT)
				pAnotherProcess=&startProcess;
			if(pAnotherProcess->type==TUBE_PROCESS::PROCESSTYPE_TSLOT)
				nType=23;	//֧�� һ��U�Ͳ��һ��T�Ͳ��
			else if(pAnotherProcess->type==TUBE_PROCESS::PROCESSTYPE_XSLOT)
				nType=24;	//֧�� һ��U�Ͳ��һ��X�Ͳ��
			else if(pAnotherProcess->IsFL())
				nType=25;	//֧�� һ��U�Ͳ��һ�˷���
			else if(nRuleType==0)
			{	//LRM
				if(pAnotherProcess->type==TUBE_PROCESS::PROCESSTYPE_TRANSECT)
					nType=26;	//֧�� һ��U�Ͳ��һ�����
				else if(pAnotherProcess->type==TUBE_PROCESS::PROCESSTYPE_PLANECUT)
					nType=27;	//֧�� һ��U�Ͳ��һ��ƽ���и�
			}
		}
		else if(startProcess.type==TUBE_PROCESS::PROCESSTYPE_TSLOT||endProcess.type==TUBE_PROCESS::PROCESSTYPE_TSLOT)
		{	//�������Ӳ�ͬ һ��ΪT�Ͳ��
			TUBE_PROCESS *pAnotherProcess=&endProcess;
			if(endProcess.type==TUBE_PROCESS::PROCESSTYPE_TSLOT)
				pAnotherProcess=&startProcess;
			if(pAnotherProcess->type==TUBE_PROCESS::PROCESSTYPE_XSLOT)
				nType=34;	//֧�� һ��T�Ͳ��һ��X�Ͳ��
			else if(pAnotherProcess->IsFL())
				nType=35;	//֧�� һ��T�Ͳ��һ�˷���
			else if(nRuleType==0)
			{	//LRM
				if(pAnotherProcess->type==TUBE_PROCESS::PROCESSTYPE_TRANSECT)
					nType=36;	//֧�� һ��T�Ͳ��һ�����
				else if(pAnotherProcess->type==TUBE_PROCESS::PROCESSTYPE_PLANECUT)
					nType=37;	//֧�� һ��T�Ͳ��һ��ƽ���и�
			}
		}
		else if(startProcess.type==TUBE_PROCESS::PROCESSTYPE_XSLOT||endProcess.type==TUBE_PROCESS::PROCESSTYPE_XSLOT)
		{	//�������Ӳ�ͬ һ��ΪX�Ͳ��
			TUBE_PROCESS *pAnotherProcess=&endProcess;
			if(endProcess.type==TUBE_PROCESS::PROCESSTYPE_USLOT)
				pAnotherProcess=&startProcess;
			if(pAnotherProcess->IsFL())
				nType=45;	//֧�� һ��X�Ͳ��һ�˷���
			else if(nRuleType==0)
			{	//LRM
				if(pAnotherProcess->type==TUBE_PROCESS::PROCESSTYPE_TRANSECT)
					nType=46;	//֧�� һ��X�Ͳ��һ�����
				else if(pAnotherProcess->type==TUBE_PROCESS::PROCESSTYPE_PLANECUT)
					nType=47;	//֧�� һ��X�Ͳ��һ��ƽ���и�
			}
		}
		else if(startProcess.IsFL()||endProcess.IsFL())
		{
			TUBE_PROCESS *pAnotherProcess=&endProcess;
			if(endProcess.IsFL())
				pAnotherProcess=&startProcess;
			if(nRuleType==0)
			{	//LRM
				if(pAnotherProcess->type==TUBE_PROCESS::PROCESSTYPE_TRANSECT)
					nType=56;	//֧�� һ�˷���һ�����
				else if(pAnotherProcess->type==TUBE_PROCESS::PROCESSTYPE_PLANECUT)
					nType=57;	//֧�� һ�˷���һ��ƽ���и�
			}
		}
		else if(startProcess.type==TUBE_PROCESS::PROCESSTYPE_TRANSECT||endProcess.type==TUBE_PROCESS::PROCESSTYPE_TRANSECT)
		{
			TUBE_PROCESS *pAnotherProcess=&endProcess;
			if(endProcess.type==TUBE_PROCESS::PROCESSTYPE_TRANSECT)
				pAnotherProcess=&startProcess;
			if(nRuleType==0)
			{	//LRM
				if(pAnotherProcess->type==TUBE_PROCESS::PROCESSTYPE_PLANECUT)
					nType=67;	//֧�� һ�����һ��ƽ���и�
			}
		}
		if(nType>0)
		{
			if(nRuleType==0)
			{	//LRM
				if(bHasBranchTube&&bHasNodePlate)
					nType=nType*10+3;	//֧�� ��ͷ���(����)�м������Լ��ڵ��
				else if(bHasBranchTube)
					nType=nType*10+2;	//֧�� ��ͷ���(����)�м�����
				else if(bHasNodePlate)
					nType=nType*10+1;	//֧�� ��ͷ���(����)�м�ڵ��
			}
			else if(nRuleType==1)
			{
				int nGeWei=0,nShiWei=0;
				nGeWei=nType%10;
				nShiWei=nType-nGeWei;
				if(bHasNodePlate)
					nType=(nShiWei+1)*10+nGeWei;	//֧�� ��ͷ���(����)�м�ڵ��
				else if(bHasBranchTube)
					nType=(nShiWei+2)*10+nGeWei;	//֧�� ��ͷ���(����)�м�����
				else 
					nType=nShiWei*10+nGeWei;

			}
		}
	}
	//8		����Ǹֺ���
	//9		���ֺ���
	//A		���ݺ���
	//B		��������
	if(type)
		sprintf(type,"%d",nType);
	return nType;
}
//////////////////////////////////////////////////////////////////////////
// PART_SLOT
PART_SLOT::PART_SLOT()
{
	cPartType=BOMPART::SLOT;
}
void PART_SLOT::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	BOMPART::FromBuffer(buffer,version);
}
void PART_SLOT::ToBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	BOMPART::ToBuffer(buffer,version);
}
//////////////////////////////////////////////////////////////////////////
// BOM_WELDPART
void BOM_WELDPART::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.ReadInteger(&iSeg);
	buffer.ReadString(sPartNo);
	buffer.ReadString(sName);
	buffer.ReadInteger(&nWeldPartNum);
	buffer.ReadByte(&cWeldPartType);
	listPartRef.DeleteList();
	int nPartRef=buffer.ReadInteger();
	for(int i=0;i<nPartRef;i++)
	{
		LIST_NODE<BOM_PARTREF> *pNode=listPartRef.AttachNode();
		pNode->data.FromBuffer(buffer,version);
	}
}
void BOM_WELDPART::ToBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.WriteInteger(iSeg);
	buffer.WriteString(sPartNo);
	buffer.WriteString(sName);
	buffer.WriteInteger(nWeldPartNum);
	buffer.WriteByte(cWeldPartType);
	BUFFERPOP stack(&buffer,0);
	buffer.WriteInteger(0);
	for (LIST_NODE<BOM_PARTREF> *pNode=listPartRef.EnumFirst();pNode;pNode=listPartRef.EnumNext())
	{
		pNode->data.ToBuffer(buffer,version);
		stack.Increment();
	}
	stack.VerifyAndRestore();
}
//////////////////////////////////////////////////////////////////////////
// BOM_PARTREF
void BOM_PARTREF::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.ReadString(sPartNo);
	buffer.ReadInteger(&nPartNum);
}
void BOM_PARTREF::ToBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.WriteString(sPartNo);
	buffer.WriteInteger(nPartNum);
}
//////////////////////////////////////////////////////////////////////////
// CModelBOM
static BOMPART* CreateBomPart(int idClsType)
{
	BOMPART *pPart=NULL;
	switch (idClsType){
	case BOMPART::PLATE:
		pPart=new PART_PLATE();
		break;
	case BOMPART::ANGLE:
		pPart=new PART_ANGLE();
		break;
	case BOMPART::TUBE:
		pPart=new PART_TUBE;
		break;
	case BOMPART::SLOT:
		pPart=new PART_SLOT;
		break;
	default:
		pPart=new BOMPART();
		break;
	}
	return pPart;
}
static BOOL DeleteBomPart(BOMPART *pPart)
{
	if(pPart==NULL)
		return FALSE;
	switch (pPart->cPartType){
	case BOMPART::PLATE:
		delete (PART_PLATE*)pPart;
		break;
	case BOMPART::ANGLE:
		delete (PART_ANGLE*)pPart;
		break;
	case BOMPART::TUBE:
		delete (PART_TUBE*)pPart;
		break;
	case BOMPART::SLOT:
		delete (PART_SLOT*)pPart;
		break;
	default:
		delete pPart;
		break;
	}
	return TRUE;
}
CModelBOM::CModelBOM()
{
	m_iCirclePlankAreaType=2;
	m_iPlankWeightStatType=1;
	m_nTaNum=1;
	m_nPartsN=m_nBoltsN=m_nWeldPartsN=0;
	m_biMatCharPosType=0;
	m_cMatCharSeparator=0;
	m_pBoltRecsArr=NULL;
	listParts.CreateNewObject=CreateBomPart;
	listParts.DeleteObject=DeleteBomPart;
}
CModelBOM::~CModelBOM()
{
	if(m_pBoltRecsArr)
		delete m_pBoltRecsArr;
	m_pBoltRecsArr=NULL;
}
//Version:  V4 ��˨���������
//			V5 �����ְ�������㷽ʽ�����ͻ�����Ϣ wht 17-02-24
//			V6 ����������Ų����ַ�������
//			V7 ֧�ֶκŷ�Χ
//			V8 ��Ӻ��쳤�ȡ���˨cFlag
//			V9 ������֧�ֺ���ĸ����
//			V10 ֧������������أ��洢ͼֽ��ȡ�����أ� wht 18-12-21
//			V11 ��˨�������ĸ����Ȧ����ϸ���� wht 19-04-10
//		    V12 ��Ӹְ��ں��ӳ��ȣ�bTowerFootPlate��չΪm_iPlateType
//              ����ѹ���б�֧�ֶ��ѹ�⣩   wht 19-06-27
//		        �ְ������������ӱ߳�����	   wht 19-06-28
//			V13 ���������б�������ַ��������ڼ���Q355��������� wht 19-08-25
bool CModelBOM::ToBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.ClearContents();
	buffer.WriteString(m_sTowerTypeName);
	if(version==NULL||version>=5)
	{	
		buffer.WriteString(m_sTaAlias);		//����
		buffer.WriteString(m_sPrjCode);		//���̱��
		buffer.WriteString(m_sCompanyName);	//��Ƶ�λ
		buffer.WriteString(m_sPrjName);		//��������
		buffer.WriteString(m_sTaStampNo);	//��ӡ��
		buffer.WriteString(m_sOperator);	//����Ա���Ʊ��ˣ�
		buffer.WriteString(m_sAuditor);		//�����
		buffer.WriteString(m_sCritic);		//������
		buffer.WriteInteger(m_nTaNum);		//������
		//�ְ�������㷽ʽ
		buffer.WriteInteger(m_iPlankWeightStatType);
		buffer.WriteInteger(m_iCirclePlankAreaType);
	}
	if(version==NULL||version>=6)
	{
		buffer.WriteByte(m_biMatCharPosType);
		buffer.WriteByte((BYTE)m_cMatCharSeparator);
	}
	if(version==NULL||version>=7)
	{
		buffer.WriteString(m_sSegStr);
	}
	BUFFERPOP stack(&buffer,m_nPartsN);
	buffer.WriteInteger(m_nPartsN);
	for(SUPERLIST_NODE<BOMPART> *pNode=listParts.EnumFirst();pNode;pNode=listParts.EnumNext())
	{
		buffer.WriteByte(pNode->pDataObj->cPartType);
		pNode->pDataObj->ToBuffer(buffer,version);
		stack.Increment();
	}
	stack.VerifyAndRestore();

	buffer.WriteInteger(m_nBoltsN);
	for(int i=0;i<m_nBoltsN;i++)
		m_pBoltRecsArr[i].ToBuffer(buffer,version);
	if(version==NULL||version>=2)
	{
		stack.Initialize(&buffer,m_nWeldPartsN);
		buffer.WriteInteger(m_nWeldPartsN);
		for(LIST_NODE<BOM_WELDPART> *pNode=listWeldParts.EnumFirst();pNode;pNode=listWeldParts.EnumNext())
		{
			pNode->data.ToBuffer(buffer,version);
			stack.Increment();
		}
		stack.VerifyAndRestore();
	}
	return true;
}
bool CModelBOM::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.SeekToBegin();
	buffer.ReadString(m_sTowerTypeName);
	if(version>=5)
	{	
		buffer.ReadString(m_sTaAlias);		//����
		buffer.ReadString(m_sPrjCode);		//���̱��
		buffer.ReadString(m_sCompanyName);	//��Ƶ�λ
		buffer.ReadString(m_sPrjName);		//��������
		buffer.ReadString(m_sTaStampNo);	//��ӡ��
		buffer.ReadString(m_sOperator);		//����Ա���Ʊ��ˣ�
		buffer.ReadString(m_sAuditor);		//�����
		buffer.ReadString(m_sCritic);		//������
		buffer.ReadInteger(&m_nTaNum);		//������
		//�ְ�������㷽ʽ
		buffer.ReadInteger(&m_iPlankWeightStatType);
		buffer.ReadInteger(&m_iCirclePlankAreaType);
	}
	if(version>=6)
	{
		buffer.ReadByte(&m_biMatCharPosType);
		buffer.ReadByte(&m_cMatCharSeparator);
	}
	if(version>=7)
	{
		buffer.ReadString(m_sSegStr);
	}
	buffer.ReadInteger(&m_nPartsN);
	listParts.DeleteList();
	BYTE cPartType=0;
	for(int i=0;i<m_nPartsN;i++)
	{
		buffer.ReadByte(&cPartType);
		SUPERLIST_NODE<BOMPART> *pNode=listParts.AttachNode(cPartType);
		pNode->pDataObj->FromBuffer(buffer,version);
	}
	buffer.ReadInteger(&m_nBoltsN);
	if(m_nBoltsN>0)
	{
		if(m_pBoltRecsArr)
			delete m_pBoltRecsArr;
		m_pBoltRecsArr=new BOMBOLT[m_nBoltsN];
		for(int i=0;i<m_nBoltsN;i++)
			m_pBoltRecsArr[i].FromBuffer(buffer,version);
	}
	if(version==NULL||version>=2)
	{
		buffer.ReadInteger(&m_nWeldPartsN);
		for(int i=0;i<m_nWeldPartsN;i++)
		{
			LIST_NODE<BOM_WELDPART> *pNode=listWeldParts.AttachNode();
			pNode->data.FromBuffer(buffer,version);
		}
	}
	return true;
}
