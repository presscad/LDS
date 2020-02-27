//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "stdio.h"
#include "Tower.h"
#include "UniWirePointModel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef __PART_DESIGN_INC_
int HANG_NODE_INFO::Compare(const ICompareClass *pCompareObj)const
{
	HANG_NODE_INFO* pCmprWireNode=(HANG_NODE_INFO*)pCompareObj;
	BYTE cType1=2,cType2=2;
	if (m_xWirePlace.ciWireType=='E'||m_xWirePlace.ciWireType==1)
		cType1=1;
	else if (m_xWirePlace.ciWireType=='J'||m_xWirePlace.ciWireType==3)
		cType1=3;
	if (pCmprWireNode->m_xWirePlace.ciWireType=='E'||pCmprWireNode->m_xWirePlace.ciWireType==1)
		cType2=1;
	else if (pCmprWireNode->m_xWirePlace.ciWireType=='J'||pCmprWireNode->m_xWirePlace.ciWireType==3)
		cType2=3;
	//�������������
	if (m_xWirePlace.iSerial>pCmprWireNode->m_xWirePlace.iSerial)
		return 1;
	else if (m_xWirePlace.iSerial<pCmprWireNode->m_xWirePlace.iSerial)
		return -1;
	else
	{	//���ݹ�����������
		if (cType1>cType2)
			return 1;
		else if (cType1<cType2)
			return -1;
		else
		{	//���ݸ���������
			if (m_xWirePlace.ciPostCode>pCmprWireNode->m_xWirePlace.ciPostCode)
				return 1;
			else if (m_xWirePlace.ciPostCode<pCmprWireNode->m_xWirePlace.ciPostCode)
				return -1;
			else
				return 0;
		}
	}
}
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CNodeRestrict::SetRestrict(WORD wFlag,BOOL bEnable/*=TRUE*/)
{
	WORD w;
	wFlag=wFlag&(X_RESTRICT|Y_RESTRICT|Z_RESTRICT|X_ROT_RESTRICT|Y_ROT_RESTRICT|Z_ROT_RESTRICT);
	if (bEnable)	//����Լ��
	{
		w = wFlag&X_RESTRICT ? X_RESTRICT : 0;
		w|= wFlag&Y_RESTRICT ? Y_RESTRICT : 0;
		w|= wFlag&Z_RESTRICT ? Z_RESTRICT : 0;
		w|= wFlag&X_ROT_RESTRICT ? X_ROT_RESTRICT : 0;
		w|= wFlag&Y_ROT_RESTRICT ? Y_ROT_RESTRICT : 0;
		w|= wFlag&Z_ROT_RESTRICT ? Z_ROT_RESTRICT : 0;
		m_wRestrict|=w;
	}
	else		//ȡ��Լ��
		m_wRestrict&=~wFlag;
}
void CNodeRestrict::SetRestrictWord(WORD wFlag)
{
	m_wRestrict=wFlag;
}

BOOL CNodeRestrict::IsHasRestrict(WORD wFlag)
{
	wFlag=wFlag&(X_RESTRICT|Y_RESTRICT|Z_RESTRICT|X_ROT_RESTRICT|Y_ROT_RESTRICT|Z_ROT_RESTRICT);
	if (m_wRestrict&wFlag)
		return TRUE;
	else
		return FALSE;
}
CXhChar16 CNodeRestrict::ToString(int style/*=0*/)
{
	if (style==0)
	{
		DWORD dwRestrictMark=0;
		if (IsHasRestrict(X_RESTRICT))
			dwRestrictMark+=100000;
		if (IsHasRestrict(Y_RESTRICT))
			dwRestrictMark+=10000;
		if (IsHasRestrict(Z_RESTRICT))
			dwRestrictMark+=1000;
		if (IsHasRestrict(X_ROT_RESTRICT))
			dwRestrictMark+=100;
		if (IsHasRestrict(Y_ROT_RESTRICT))
			dwRestrictMark+=10;
		if (IsHasRestrict(Z_ROT_RESTRICT))
			dwRestrictMark+=1;
		return CXhChar16("%d",dwRestrictMark);
	}
	else
	{
		CXhChar16 restStr;
		DWORD dwRestrictMark=0;
		if (IsHasRestrict(X_RESTRICT))
			restStr.Append('X');
		if (IsHasRestrict(Y_RESTRICT))
			restStr.Append(",Y");
		if (IsHasRestrict(Z_RESTRICT))
			restStr.Append(",Z");
		if (IsHasRestrict(X_ROT_RESTRICT))
			restStr.Append(",Rx");
		if (IsHasRestrict(Y_ROT_RESTRICT))
			restStr.Append(",Ry");
		if (IsHasRestrict(Z_ROT_RESTRICT))
			restStr.Append(",Rz");
		if (restStr[0]==',')
			memmove(restStr,(char*)restStr+1,restStr.GetLength());
		return restStr;
	}
}

WORD CNodeRestrict::GetRestrictWord()
{
	return m_wRestrict;
}
CWireInfo::CWireInfo()
{
	memset(this,0,sizeof(CWireInfo));
	land_high=15;
	m_cFormula=1;	//Ĭ����б�����߷���
	wire.m_fSafeCoef=2.5;
	m_nDaoXianFengLieNum=1;
	m_fAttachmentDiameter=280;
}
WIREPLACE::WIREPLACE()
{
	idNodePlace=0;
	fLoadAllocCoefX=fLoadAllocCoefY=fLoadAllocCoefZ=1.0;
}

IMPLEMENT_GET(CLDSNode,pointI)
{
	return point_i;
}
IMPLEMENT_GET(CLDSNode,xPreliftPos)
{
	if (m_hArcLift>=0)
		return pos;
	if (!m_bInitArcLiftPos)
		Set(true);
	m_bInitArcLiftPos=true;
	return m_xPreliftPosition;
}
IMPLEMENT_GET(CLDSNode,xArcliftPos)
{
	if (m_hArcLift==0)
		return pos;
	if (!m_bInitArcLiftPos)
		Set(true);
	m_bInitArcLiftPos=true;
	return m_xArcLiftPos;
}
IMPLEMENT_GET(CLDSNode,xActivePos)
{
	if (m_hArcLift>0)
		return m_bDisplayBeforeArcLift?pos:m_xArcLiftPos;
	else
		return pos;
}
bool CLDSNode::INTERNAL_TEST_VERSION;	//Ŀǰ�������������ڵ��涯����

CLDSNode::CLDSNode()
{
	//memset(this,0,sizeof(CLDSNode));
	_bVirtualObj=false;
	m_hArcLift=0;
	m_cPosCalType=0;
	m_cPosCalViceType=0;
	attach_offset=0.0;
	attach_scale=0.0;
	arrRelationPole[0]=arrRelationPole[1]=arrRelationNode[0]=arrRelationNode[1]=arrRelationNode[2]=arrRelationNode[3]=0;
	hRelationPlane=0;
	hDatumPoint=0;
	_hFatherPart=0;
	is_visible = TRUE;
	is_undo_modified=TRUE;
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	point_i=0;
	m_hRelaWireLoad=0;
	force_type=AUTO_NODE;
	//pRestrictList = new CNodeRestList;
	dfAddedStruMass=attachMass=0.0;
	point_i=0;
	force_type=AUTO_NODE;
	m_ciWireFractions=1;
	wireplace.data.wDataCode=0;
#endif
	m_bInitArcLiftPos=false;
	m_pxConnNodeInfo=NULL;
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN��������Ա���;
	m_nClassTypeId=CLS_NODE;
	//strcpy(m_sClassName,"CLDSNode");
}
char CLDSNode::get_ciLegQuad()
{
	if (m_ciLegQuad>0&&m_ciLegQuad<5)
		return m_ciLegQuad;
	else if (layer_tag[0]=='L')
	{
		char ciQuad=layer_tag[2]-'0';
		if (ciQuad>0&&ciQuad<5)
			return ciQuad;
		if (pos.x>EPS&&pos.y>EPS)
			return 1;
		else if (pos.x<EPS&&pos.y>EPS)
			return 2;
		else if (pos.x>EPS&&pos.y<EPS)
			return 3;
		else if (pos.x<EPS&&pos.y<EPS)
			return 4;
		else
			return 0;
	}
	else
		return 0;
}
bool CLDSNode::IsAcceptableTypeId(int type_id)
{
	if (type_id==CLS_NODE||type_id==CLS_OBJECT||type_id==CLS_DBOBJECT)
		return true;
	else
		return false;
}

CLDSNode::~CLDSNode()
{
	if (m_pxConnNodeInfo)
		delete m_pxConnNodeInfo;
	m_pxConnNodeInfo=NULL;
}

CLDSNode & CLDSNode::operator=(const CLDSNode &node)
{
	memcpy(this,&node,sizeof(CLDSNode));
	is_undo_modified=TRUE;
	return *this;
}
f3dPoint CLDSNode::Position(bool bArcLiftPos/*=true*/)
{
#ifdef __COMMON_PART_INC_
	if (bArcLiftPos)
	{
		if (m_hArcLift>0x20)
			return m_xArcLiftPos;
		else //if(m_hArcLift<0)//0x20)
			return pos;
	}
	else
#endif
		return xPreliftPos;
}
double CLDSNode::Position(char cCoordType,bool bArcLiftPos/*=true*/)
{
	GEPOINT xActivePoint=pos;
	if (bArcLiftPos)
	{
		if (m_hArcLift>0x20)
			xActivePoint=m_xArcLiftPos;
		else //if(m_hArcLift<0)//0x20)
			xActivePoint=pos;
	}
	else
		xActivePoint=xPreliftPos;

	if (toupper(cCoordType)=='X')
		return xActivePoint.x;
	else if (toupper(cCoordType)=='Y')
		return xActivePoint.y;
	else //if(toupper(cCoordType)=='Z')
		return xActivePoint.z;
}
bool CLDSNode::IsUdfCoordX() const	//X��������Ƿ����û�����
{
	if (_cPosCalType==0||_cPosCalType==1||			//�������κ������ڵ��ָ��X��������ڵ�
		(_cPosCalType==7&&_cPosCalViceType=='X')||	//��(Y)��ƫ�Ƶ�
		(_cPosCalType==8||_cPosCalType==10))			//��׼����ָ��XY��XZ���������
		return true;
	else
		return false;
}
bool CLDSNode::IsUdfCoordY() const	//Y��������Ƿ����û�����
{
	if (_cPosCalType==0||_cPosCalType==2||			//�������κ������ڵ��ָ��Y��������ڵ�
		(_cPosCalType==7&&_cPosCalViceType=='Y')||	//��(Y)��ƫ�Ƶ�
		(_cPosCalType==8||_cPosCalType==9))			//��׼����ָ��XY��YZ���������
		return true;
	else
		return false;
}
bool CLDSNode::IsUdfCoordZ() const	//Z��������Ƿ����û�����
{
	if (_cPosCalType==0||_cPosCalType==3||			//�������κ������ڵ��ָ������������ڵ�
		(_cPosCalType==7&&_cPosCalViceType=='Z')||	//��(��)��ƫ�Ƶ�
		(_cPosCalType==9||_cPosCalType==10))		//��׼����ָ��X(Y)��Z���������
		return true;
	else
		return false;
}
f3dPoint CLDSNode::SetPosition(double x,double y,double z)
{
	pos.Set(x,y,z);
	SetModified();
	return pos;
}
f3dPoint CLDSNode::SetPositionX(double coordx)
{
	pos.x=coordx;
	SetModified();
	return pos;
}
f3dPoint CLDSNode::SetPositionY(double coordy)
{
	pos.y=coordy;
	SetModified();
	return pos;
}

f3dPoint CLDSNode::SetPositionZ(double coordz)
{
	pos.z=coordz;
	SetModified();
	return pos;
}

f3dPoint CLDSNode::SetPosition(const f3dPoint& position)
{
	pos=position;
	SetModified();
	return pos;
}
long CLDSNode::RegisterUdfPropId(char *propStr)		//����û��Զ�������Id
{
	long idProp=GetPropID(propStr);
	if (idProp>0)
		return idProp;
	CLDSNode::AddPropItem(propStr,m_idPropUdf++);
	return GetPropID(propStr);
}
BOOL CLDSNode::FindUdfPropKeyStrByPropId(long id,char* propKeyStr/*=NULL*/,int maxStrLen/*=100*/)
{
	if (id<m_idMinPropUdf)
		return FALSE;
	return GetPropStr(id,propKeyStr,maxStrLen);
}
DWORD CLDSNode::RetrieveWirePlaceKey(char* propKeyStr)	//�õ�����ֵ
{
	CXhChar100 keyStr;keyStr.Copy(propKeyStr);
	char* keySect=strtok(keyStr,"_-");
	if (keySect!=NULL&&stricmp(keySect,"accessaryNodePlace")==0)
	{
		DWORD key=0;
		keySect=strtok(NULL,"_-");
		if (keySect!=NULL)
			key=atoi(keySect);
		return key;
	}
	return 0;
}
int CLDSNode::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CWireInfo wireInfo;	//��ʱ
	char sText[101]="",sProp[101]="";
	if (GetPropID("basicInfo")==0)
		strcpy(sText,"");
	else if (GetPropID("handle")==id)
		sprintf(sText,"0X%X",handle);
	else if (GetPropID("layer")==id)
		strcpy(sText,layer());
	else if (GetPropID("hFatherPart")==id)
		sprintf(sText,"0X%X",hFatherPart);
#if defined(__TSA_)||defined(__LDS_)||defined(__TSA_FILE_)||defined(__LDS_FILE_)
	else if (GetPropID("hangingInfo")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if (m_cHangWireType=='E')
			strcpy(sText,"Earth wire");
		else if (m_cHangWireType=='C')
			strcpy(sText,"Conduct wire");
		else if (m_cHangWireType=='J')//'O')
			strcpy(sText,"Jump wire");
		else
			strcpy(sText,"None");
#else 
		if (m_cHangWireType=='E')
			strcpy(sText,"����");
		else if (m_cHangWireType=='C')
			strcpy(sText,"����");
		else if (m_cHangWireType=='J')//'O')
			strcpy(sText,"����");
		else
			strcpy(sText,"��");
#endif
	}
	else if (GetPropID("hangingDsec")==id)
		strcpy(sText,m_sHangWireDesc);
	else if (GetPropID("hangingStyle")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if (wireplace.ciTensionType==2)//wireInfo.m_bNaiZhang)
			strcpy(sText,"Tension Wire");
		else if (wireplace.ciTensionType==3)
			strcpy(sText,"Overhang Wire");
#else 
		if (wireplace.ciTensionType==2)//wireInfo.m_bNaiZhang)
			strcpy(sText,"���Ź���");
		else if (wireplace.ciTensionType==3)
			strcpy(sText,"��������");
#endif
	}
	else if (GetPropID("voltage")==id)
	{
		sprintf(sText,"%d",wireInfo.m_nVoltGrade);
	}
	else if (GetPropID("splitNumber")==id)
	{
		sprintf(sText,"%d",wireInfo.m_nDaoXianFengLieNum);
	}
	else if (GetPropID("landHigh")==id)
	{
		sprintf(sText,"%f",wireInfo.land_high);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("attachmentSpec")==id)
	{
		strcpy(sText,wireInfo.specAttachment);
	}
	else if (GetPropID("attachmentWeight")==id)
	{
		sprintf(sText,"%f",wireInfo.m_fAttachmentWeight);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("m_fAttachmentDiameter")==id)
	{
		sprintf(sText,"%f",wireInfo.m_fAttachmentDiameter);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("m_fAttachmentLength")==id)
	{
		sprintf(sText,"%f",wireInfo.m_fAttachmentLength);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("anchorLineAngle")==id)
	{
		sprintf(sText,"%f",wireInfo.m_fAchorLineAngle);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("tractionLineAngle")==id)
	{
		sprintf(sText,"%f",wireInfo.m_fTractionLineAngle);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("wireType")==id)
	{
		strcpy(sText,wireInfo.wire.specWireType);
	}
	else if (GetPropID("wireSpec")==id)
	{
		strcpy(sText,wireInfo.wire.specWire);
	}
	else if (GetPropID("wireDiameter")==id)
	{
		sprintf(sText,"%f",wireInfo.wire.m_fDiameter);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("wireArea")==id)
	{
		sprintf(sText,"%f",wireInfo.wire.m_fArea);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("unitWeight")==id)
	{
		sprintf(sText,"%f",wireInfo.wire.m_fUnitWeight);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("forcePermit")==id)
	{
		sprintf(sText,"%f",wireInfo.wire.m_fForcePermit);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("saftyCoef")==id)
	{
		sprintf(sText,"%f",wireInfo.wire.m_fSafeCoef);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("maxTension")==id)
	{
		sprintf(sText,"%f",wireInfo.wire.m_fMaxUseTension);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("elasticityCoef")==id)
	{
		sprintf(sText,"%f",wireInfo.wire.m_fElasticCoef);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("expansionCoef")==id)
	{
		sprintf(sText,"%f",wireInfo.wire.m_fExpansionCoef);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("horiSpace")==id)
	{
		sprintf(sText,"%f",wireInfo.xianlu.horizon_space);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("vertSpace")==id)
	{
		sprintf(sText,"%f",wireInfo.xianlu.vertical_space);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("representSpace")==id)
	{
		sprintf(sText,"%f",wireInfo.xianlu.represent_space);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("HtoL")==id)
	{
		sprintf(sText,"%f",wireInfo.xianlu.HtoL);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("allocHoriCoef")==id)
	{
		sprintf(sText,"%f",wireInfo.xianlu.horizon_space_allotcoef);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("allocVertCoef")==id)
	{
		sprintf(sText,"%f",wireInfo.xianlu.vertical_space_allotcoef);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("xianluAngle")==id)
	{
		sprintf(sText,"%f",wireInfo.xianlu.xianlu_angle);
		SimplifiedNumString(sText);
	}
#endif
	else if (GetPropID("m_bVirtualObj")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if (_bVirtualObj)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
		if (_bVirtualObj)
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if (GetPropID("pos.x")==id)
	{
		sprintf(sText,"%f",pos.x);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("pos.y")==id)
	{
		sprintf(sText,"%f",pos.y);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("pos.z")==id)
	{
		sprintf(sText,"%f",pos.z);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("m_hArcLift")==id)		//Ԥ����
	{
		CLDSArcLift *pLift=BelongModel()->FromArcLiftHandle(abs(m_hArcLift));
		if (pLift)
			_snprintf(sText,100,"%s(%d)",pLift->name,pLift->iNo);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Without Lift Arc");
#else 
			strcpy(sText,"��Ԥ��");
#endif
	}
	else if (GetPropID("ArcLiftDirection")==id)		//Ԥ����
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if (m_hArcLift>0)
			strcpy(sText,"Lift direction");
		else if (m_hArcLift<0)
			strcpy(sText,"Reverse direction");
#else 
		if (m_hArcLift>0)
			strcpy(sText,"����");
		else if (m_hArcLift<0)
			strcpy(sText,"����");
#endif
	}
	else if (GetPropID("m_xArcLiftPos.x")==id)	//Ԥ����λ��
	{
		if (this->m_hArcLift>0)
			sprintf(sText,"%f",m_xArcLiftPos.x);
		else
			sprintf(sText,"%f",m_xPreliftPosition.x);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("m_xArcLiftPos.y")==id)	//Ԥ����λ��
	{
		if (this->m_hArcLift>0)
			sprintf(sText,"%f",m_xArcLiftPos.y);
		else
			sprintf(sText,"%f",m_xPreliftPosition.y);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("m_xArcLiftPos.z")==id)	//Ԥ����λ��
	{
		if (this->m_hArcLift>0)
			sprintf(sText,"%f",m_xArcLiftPos.z);
		else
			sprintf(sText,"%f",m_xPreliftPosition.z);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("iSeg")==id)
		strcpy(sText,(char*)iSeg.ToString());
	else if (GetPropID("cfgword")==id)
		//return MakeWordToString(cfgword,valueStr);
	{
		if (Layer(0)=='L')	//���ȹ���
			return cfgword.GetLegScopeStr(valueStr,50,true);
		else
			return cfgword.GetBodyScopeStr(valueStr);
	}
	else if (GetPropID("m_hBlock")==id)
		strcpy(sText,GetBlockIdName());
	else if (GetPropID("relativeObjs")==id)
	{
		for (RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
		{
			if (strlen(sText)==0)
				_snprintf(sText,99,"0X%X",pRelaObj->hObj);
			else
				_snprintf(sText,99,"%s,0X%X",sText,pRelaObj->hObj);
		}
	}
	else if (GetPropID("shadowInfo")==0)
		strcpy(sText,"");
	else if (GetPropID("m_hMotherObj")==id)
	{
		CLDSNode *pMotherNode=(CLDSNode*)MotherObject(false);
		if (pMotherNode!=this)
			sprintf(sText,"0X%X",pMotherNode->handle);
		else
			strcpy(sText,"0X0");
	}
	else if (GetPropID("m_hBlockRef")==id)
	{
		CBlockParameter blockPara;
		if (GetBlockParameter(blockPara))
			sprintf(sText,"0X%X",blockPara.m_pBlockRef->handle);
		else
			strcpy(sText,"0X0");
	}
	else if (GetPropID("m_hBlock")==id)
		strcpy(sText,GetBlockIdName());
	else if (GetPropID("attachRelation")==id)
		strcpy(sText,"");
	else if (GetPropID("m_cPosCalType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if (m_cPosCalType==0)
			strcpy(sText,"0-No Dependent");
		else if (m_cPosCalType==1)
			strcpy(sText,"1-X Coordinate Invariant");
		else if (m_cPosCalType==2)
			strcpy(sText,"2-Y Coordinate Invariant");
		else if (m_cPosCalType==3)
			strcpy(sText,"3-Z Coordinate Invariant");
		else if (m_cPosCalType==4)
			strcpy(sText,"4-Two Rod Intersection");
		else if (m_cPosCalType==5)
			strcpy(sText,"5-Proportion");
		else if (m_cPosCalType==6)
			strcpy(sText,"6-With One Node Same Height");
		else if (m_cPosCalType==7)
			strcpy(sText,"7-With One Node Offset Component");
		else if (m_cPosCalType==8)
		{
			if (_cPosCalViceType==0)
				strcpy(sText,"8I-Intersection of line and plane");
			else
				strcpy(sText,"8-X&Y Coordinate Invariant");
		}
		else if (m_cPosCalType==9)
			strcpy(sText,"9-Z&Y Coordinate Invariant");
		else if (m_cPosCalType==10)
			strcpy(sText,"10-X&Z Coordinate Invariant");
		else if (m_cPosCalType==11)
			strcpy(sText,"11-Parametric Position");
		else if (m_cPosCalType==12)
			strcpy(sText,"12-Symmetric Creation");
		else if (m_cPosCalType == 13)
			strcpy(sText,"13-Same coord to");
#else 
		if (m_cPosCalType==0)
			strcpy(sText,"0-�������κ������Ľڵ�");
		else if (m_cPosCalType==1)
			strcpy(sText,"1-�˼���ָ��X�����");
		else if (m_cPosCalType==2)
			strcpy(sText,"2-�˼���ָ��Y�����");
		else if (m_cPosCalType==3)
			strcpy(sText,"3-�˼���ָ��Z�����");
		else if (m_cPosCalType==4)
			strcpy(sText,"4-���˼����潻��");
		else if (m_cPosCalType==5)
			strcpy(sText,"5-�˼��ϵı����ȷֵ�");
		else if (m_cPosCalType==6)
			strcpy(sText,"6-�ظ˼���ĳ�ڵ�ȸߵĽڵ�");
		else if (m_cPosCalType==7)
			strcpy(sText,"7-�ظ˼���ĳ�ڵ��ƫ�Ƶ�");
		else if (m_cPosCalType==8)
		{
			if (_cPosCalViceType=='I')
				strcpy(sText,"8I-ָ�����潻��");
			else
				strcpy(sText,"8 -��׼����ָ��XY�����");
		}
		else if (m_cPosCalType==9)
			strcpy(sText,"9 -��׼����ָ��YZ�����");
		else if (m_cPosCalType==10)
			strcpy(sText,"10-��׼����ָ��XZ�����");
		else if (m_cPosCalType==11)
			strcpy(sText,"11-ָ����׼��");
		else if (m_cPosCalType == 12)
			strcpy(sText,"12-�Գ�����");
		else if (m_cPosCalType == 13)
			strcpy(sText,"13-�������");
#endif
	}
	else if (GetPropID("m_cPosCalViceType")==id)
	{
		if (m_cPosCalType==4)		//�ظ˼���ĳ�ڵ����������Ľڵ�
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if (m_cPosCalViceType==1)
				strcpy(sText,"1.X-Z Plane Cross");
			else if (m_cPosCalViceType==2)
				strcpy(sText,"2.Y-Z Plane Cross");
			else if (m_cPosCalViceType==3)
				strcpy(sText,"3.X-Y Plane Cross");
			else
				strcpy(sText,"0.Cross Same Plane");
#else 
			if (m_cPosCalViceType==1)
				strcpy(sText,"1.������X-Zƽ��");
			else if (m_cPosCalViceType==2)
				strcpy(sText,"2.������Y-Zƽ��");
			else if (m_cPosCalViceType==3)
				strcpy(sText,"3.������X-Yƽ��");
			else
				strcpy(sText,"0.���湲��");
#endif
		}
		else if (m_cPosCalType==6)		//�ظ˼���ĳ�ڵ����������Ľڵ�
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if (m_cPosCalViceType=='X')
				strcpy(sText,"X Coordinate");
			else if (m_cPosCalViceType=='Y')
				strcpy(sText,"Y Coordinate");
			else //if(m_cPosCalViceType=='Z')
				strcpy(sText,"Z Coordinate");
#else 
			if (m_cPosCalViceType=='X')
				strcpy(sText,"X����");
			else if (m_cPosCalViceType=='Y')
				strcpy(sText,"Y����");
			else //if(m_cPosCalViceType=='Z')
				strcpy(sText,"Z����");
#endif
		}
		else if (m_cPosCalType==7)	//ƫ�ƽڵ�
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if (m_cPosCalViceType=='X')
				strcpy(sText,"X Axis");
			else if (m_cPosCalViceType=='Y')
				strcpy(sText,"Y Axis");
			else if (m_cPosCalViceType=='Z')
				strcpy(sText,"Z Axis");
			else
				strcpy(sText,"Length");
#else 
			if (m_cPosCalViceType=='X')
				strcpy(sText,"X��");
			else if (m_cPosCalViceType=='Y')
				strcpy(sText,"Y��");
			else if (m_cPosCalViceType=='Z')
				strcpy(sText,"Z��");
			else
				strcpy(sText,"����");
#endif
		}
		else if (m_cPosCalType==8||m_cPosCalType==9||m_cPosCalType==10||m_cPosCalType==12)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if (m_cPosCalViceType=='X')
				strcpy(sText,"X Axis Symmetry");
			else if (m_cPosCalViceType=='Y')
				strcpy(sText,"Y Axis Symmetry");
			else if (m_cPosCalViceType=='Z')
				strcpy(sText,"Z Axis Symmetry");
			else
				strcpy(sText,"No Symmetric Reference");
#else 
			if (m_cPosCalViceType=='X')
				strcpy(sText,"X��Գ�");
			else if (m_cPosCalViceType=='Y')
				strcpy(sText,"Y��Գ�");
			else if (m_cPosCalViceType=='Z')
				strcpy(sText,"Z��Գ�");
			else
				strcpy(sText,"�޶ԳƲ���");
#endif
		}
	}
	else if (GetPropID("attachNode")==id)
		strcpy(sText,"");
	else if (GetPropID("attachNode1")==id)
		sprintf(sText,"0X%X",arrRelationNode[0]);
	else if (GetPropID("attachNode2")==id)
		sprintf(sText,"0X%X",arrRelationNode[1]);
	else if (GetPropID("attachNode3")==id)
		sprintf(sText,"0X%X",arrRelationNode[2]);
	else if (GetPropID("attachNode4")==id)
		sprintf(sText,"0X%X",arrRelationNode[3]);
	else if (GetPropID("attachPole")==id)
		strcpy(sText,"");
	else if (GetPropID("attachPole1")==id)
		sprintf(sText,"0X%X",arrRelationPole[0]);
	else if (GetPropID("attachPole2")==id)
		sprintf(sText,"0X%X",arrRelationPole[1]);
	else if (GetPropID("attach_scale")==id)
	{
		sprintf(sText,"%f",attach_scale);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("attach_offset")==id)
	{
		sprintf(sText,"%f",attach_offset);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("hDatumPoint")==id)
	{
		if (m_cPosCalType!=12)
			sprintf(sText,"0X%X",hDatumPoint);
		else
			sprintf(sText,"0X%X",arrRelationNode[0]);
	}
	else if (GetPropID("hRelationPlane")==id)
		sprintf(sText,"0X%X",hRelationPlane);
	else if (GetPropID("hPlaneRefNode#0")==id)
	{
		CLDSPlane* pPlane=BelongModel()->FromPlaneHandle(this->hRelationPlane);
		if (pPlane)
			sprintf(sText,"0X%X",pPlane->hPointArr[0].handle);//hNodePoint);
		else
			strcpy(sText,"0X0");
	}
	else if (GetPropID("hPlaneRefNode#1")==id)
	{
		CLDSPlane* pPlane=BelongModel()->FromPlaneHandle(this->hRelationPlane);
		if (pPlane)//&&pPlane->hPointArr[1].bSimpleNodePoint)
			sprintf(sText,"0X%X",pPlane->hPointArr[1].handle);//hNodePoint);
		else
			strcpy(sText,"0X0");
	}
	else if (GetPropID("hPlaneRefNode#2")==id)
	{
		CLDSPlane* pPlane=BelongModel()->FromPlaneHandle(this->hRelationPlane);
		if (pPlane)//&&pPlane->hPointArr[2].bSimpleNodePoint)
			sprintf(sText,"0X%X",pPlane->hPointArr[2].handle);//hNodePoint);
		else
			strcpy(sText,"0X0");
	}
#if defined(__TSA_)||defined(__LDS_)||defined(__TSA_FILE_)||defined(__LDS_FILE_)
	else if (GetPropID("point_i")==id)
		sprintf(sText,"%d",point_i);
	else if (GetPropID("force_type")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if (force_type==AUTO_NODE)
			strcpy(sText,"0.Default");
		else if (force_type==FORCE_NODE)
			strcpy(sText,"1.Force Node");
		else if (force_type==SWING_NODE)
			strcpy(sText,"2.Swing Node");
#else 
		if (force_type==AUTO_NODE)
			strcpy(sText,"0.�Զ��ж�");
		else if (force_type==FORCE_NODE)
			strcpy(sText,"1.�����ڵ�");
		else if (force_type==SWING_NODE)
			strcpy(sText,"2.�ڶ��ڵ�");
#endif
	}
	else if (GetPropID("attachForeverLoad_X")==id)
	{
		sprintf(sText,"%f",PermanentLoad.x/1000);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("attachForeverLoad_Y")==id)
	{
		sprintf(sText,"%f",PermanentLoad.y/1000);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("attachForeverLoad_Z")==id)
	{
		sprintf(sText,"%f",PermanentLoad.z/1000);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("permanentFz")==id)
	{
		CWorkStatus *pWorkStatus=_console->GetActiveWorkStatus();
		if (pWorkStatus==NULL)
			strcpy(sText,"0");
		else
		{
			CExternalNodeLoad *pNodeLoad=pWorkStatus->hashNodeLoad.GetValue(handle);
			if (pNodeLoad)
			{
				sprintf(sText,"%f",pNodeLoad->permanentFz/1000);
				SimplifiedNumString(sText);
			}
			else
				strcpy(sText,"0");
		}
	}
	else if (GetPropID("attachVolLoad_X")==id)
	{
		CWorkStatus *pWorkStatus=_console->GetActiveWorkStatus();
		if (pWorkStatus==NULL)
			strcpy(sText,"0");
		else
		{
			CExternalNodeLoad *pNodeLoad=pWorkStatus->hashNodeLoad.GetValue(handle);
			if (pNodeLoad)
			{
				if (pWorkStatus->m_bLoadIncFactor)
					sprintf(sText,"%f",pNodeLoad->designLoad.x/1000);
				else
					sprintf(sText,"%f",pNodeLoad->Fxyz.x/1000);
				SimplifiedNumString(sText);
			}
			else
				strcpy(sText,"0");
		}
	}
	else if (GetPropID("attachVolLoad_Y")==id)
	{
		CWorkStatus *pWorkStatus=_console->GetActiveWorkStatus();
		if (pWorkStatus==NULL)
			strcpy(sText,"0");
		else
		{
			CExternalNodeLoad *pNodeLoad=pWorkStatus->hashNodeLoad.GetValue(handle);
			if (pNodeLoad)
			{
				if (pWorkStatus->m_bLoadIncFactor)
					sprintf(sText,"%f",pNodeLoad->designLoad.y/1000);
				else
					sprintf(sText,"%f",pNodeLoad->Fxyz.y/1000);
				SimplifiedNumString(sText);
			}
			else
				strcpy(sText,"0");
		}
	}
	else if (GetPropID("attachVolLoad_Z")==id)
	{
		CWorkStatus *pWorkStatus=_console->GetActiveWorkStatus();
		if (pWorkStatus==NULL)
			strcpy(sText,"0");
		else
		{
			CExternalNodeLoad *pNodeLoad=pWorkStatus->hashNodeLoad.GetValue(handle);
			if (pNodeLoad)
			{
				if (pWorkStatus->m_bLoadIncFactor)
					sprintf(sText,"%f",pNodeLoad->designLoad.z/1000);
				else
					sprintf(sText,"%f",pNodeLoad->Fxyz.z/1000);
				SimplifiedNumString(sText);
			}
			else
				strcpy(sText,"0");
		}
	}
	else if (GetPropID("restrict_all")==id)
	{
		WORD wRestrict = GetRestrictWord(_console->ActiveModuleHandle());
		if (wRestrict&X_RESTRICT && wRestrict&Y_RESTRICT && wRestrict&Z_RESTRICT&&
			wRestrict&X_ROT_RESTRICT && wRestrict&Y_ROT_RESTRICT && wRestrict&Z_ROT_RESTRICT)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if (GetPropID("restrict_x")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if (IsHasRestrict(X_RESTRICT))
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if (IsHasRestrict(X_RESTRICT))
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if (GetPropID("restrict_y")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if (IsHasRestrict(Y_RESTRICT))
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if (IsHasRestrict(Y_RESTRICT))
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if (GetPropID("restrict_z")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if (IsHasRestrict(Z_RESTRICT))
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if (IsHasRestrict(Z_RESTRICT))
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if (GetPropID("restrict_rot_x")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if (IsHasRestrict(X_ROT_RESTRICT))
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if (IsHasRestrict(X_ROT_RESTRICT))
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if (GetPropID("restrict_rot_y")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if (IsHasRestrict(Y_ROT_RESTRICT))
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if (IsHasRestrict(Y_ROT_RESTRICT))
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if (GetPropID("restrict_rot_z")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if (IsHasRestrict(Z_ROT_RESTRICT))
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if (IsHasRestrict(Z_ROT_RESTRICT))
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	/*else if(GetPropID("m_idNodePlace")==id)
	{
		WIREPLACE* pWirePlace=wires.GetFirst();
		if(pWirePlace)
			strcpy(sText,pWirePlace->idNodePlace.ToString());
		else
			strcpy(sText,"0");
	}
	else if(FindUdfPropKeyStrByPropId(id,sProp,100))
	{
		WIREPLACE* pWirePlate=wires.GetValue(RetrieveWirePlaceKey(sProp));
		if(pWirePlate)
			strcpy(sText,pWirePlate->idNodePlace.ToString());
	}*/
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	if (strlen(sText)>=nMaxStrBufLen)
		Log2File()->Log("0X%X Node's property##d string value out of length",handle,id);
#else
	if (strlen(sText)>=nMaxStrBufLen)
		Log2File()->Log("�ڵ�0X%X����#%d�ַ���������������",handle,id);
#endif
	if (valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return (int)strlen(sText);
}
long CLDSNode::GetPropID(char* propStr)
{
	PROPLIST_ITEM* pItem=CLDSNode::propHashtable.GetValue(propStr);
	if (pItem!=NULL)
		return pItem->id;
	else
	{
		/*CString prompt="û�ҵ���Ϊ\"";
		prompt+=propStr;
		prompt+="\"������ID��";
		AfxMessageBox(prompt);*/
		return 0;
	}
}

BOOL CLDSNode::GetPropStr(long id,char* propStr,int maxStrLen/*=100*/)
{
	PROPLIST_ITEM* pPropItem=hashPropItemById.GetValue(id);
	if (pPropItem&&propStr)
	{
		strncpy(propStr,pPropItem->propKeyStr,maxStrLen);
		return TRUE;
	}
	return FALSE;
}

BOOL CLDSNode::IsPropEqual(CHashSet<CLDSObject*> &selectObjs,long idProp)
{
	if (selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSObject *pFirstObj = selectObjs.GetFirst();
	CLDSNode *pNode=NULL,*pFirstNode = (CLDSNode*)pFirstObj;
	pFirstNode->GetPropValueStr(idProp,valueStr);
	for (pNode=(CLDSNode*)selectObjs.GetNext();pNode;pNode=(CLDSNode*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pNode->GetPropValueStr(idProp,valueStr2);
		if (stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

int CLDSNode::GetPropStatus(long id)
{
	DWORD dwStatus;
	if (CLDSNode::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if (dwStatus==STATUS_EXPAND)
			return 0;	//������
		else if (dwStatus==STATUS_COLLAPSE)
			return 1;	//����
	}
	return -1;	//δ�ҵ������״̬
}

BOOL CLDSNode::SetPropStatus(long id,BOOL bHide)
{
	DWORD dwStatus;
	if (bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSNode::propStatusHashtable.SetValueAt(id,dwStatus);
}
BOOL CLDSNode::GetObjPropReadOnlyDefaultState(long id)	//�õ��������Ե�Ĭ��ֻ��״̬
{
	return GetObjPropReadOnlyDefaultState(id,ShadowMode());
}
BOOL CLDSNode::GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode)	//�õ��������Ե�Ĭ��ֻ��״̬
{
	if (shadow_mode==CLDSObject::SHADOWMODE_COMMON)
	{
		if (GetPropID("layer")==id)
			return FALSE;
		else
			return TRUE;
	}
	else if (shadow_mode==CLDSObject::SHADOWMODE_BLOCK)
	{
		if (GetPropID("layer")==id)
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		if (GetPropID("handle")==id)
			return TRUE;
		else
			return FALSE;
	}
}

CLDSNode *CLDSNode::MotherNode()
{
	CLDSObject *pMotherObj=MotherObject();
	if (pMotherObj==NULL||pMotherObj->GetClassTypeId()!=m_nClassTypeId)
#ifdef AFX_TARG_ENU_ENGLISH
		throw "The projective mother node is error��";
#else 
		throw "Ӱ��ڵ�ĸ���ڵ����";
#endif
	else
		return (CLDSNode*)pMotherObj;
}
static BYTE _localUpdateMovePartsFlag=0x02;	//0x01:�������������¼����;0x02:��ʱ��ֹ��¼�䶯��������
static CHashSet<CLDSLinePart*> _localHashMoveRods;
static CHashSet<CLDSNode*> _localHashModifiedNodes;
//static ARRAY_LIST<CLDSNode*> _localMoveNodesArray;
BYTE CLDSNode::GetMovePartsUpdateFlag() { return _localUpdateMovePartsFlag; }
BYTE CLDSNode::SetMovePartsUpdateFlag(BYTE cbFlag/*=0x01*/) { return _localUpdateMovePartsFlag=cbFlag; }
void CLDSNode::ClearUpdateMovePartsBuff()
{
	if ((_localUpdateMovePartsFlag&0x03)==CLEAR_BUFF_OF_RELA_PARTS)
	{	//��չ���������¼����
		_localHashMoveRods.Empty();
		_localHashModifiedNodes.Empty();
		//_localMoveNodesArray.SetSize(0,50);
		_localUpdateMovePartsFlag-=CLEAR_BUFF_OF_RELA_PARTS;
	}
}
CLDSLinePart* CLDSNode::EnumFirstRelaRod()
{
	return _localHashMoveRods.GetFirst();
}
CLDSLinePart* CLDSNode::EnumNextRelaRod()
{
	return _localHashMoveRods.GetNext();
}
CLDSNode* CLDSNode::EnumFirstRelaNode()
{
	return _localHashModifiedNodes.GetFirst();
}
CLDSNode* CLDSNode::EnumNextRelaNode()
{
	return _localHashModifiedNodes.GetNext();
}
long CNodeMoveDetector::m_nDepth = 0;
bool CNodeMoveDetector::m_bGlobalEnabled=false;	//�Ƿ�ȫ������/���ýڵ��ƶ�̽��
int MAX_DETECT_NODEMOVE_DEPTH = 20;
CNodeMoveDetector::CNodeMoveDetector(CLDSNode* pNode,bool enabled/*=true*/)
{
	if ((m_pNode = pNode) != NULL)
		m_xOldPos = pNode->xActivePos;
	m_bEnableDetector = enabled && m_bGlobalEnabled;
	if (enabled&&m_nDepth == 0)
	{
		IModel* pModel = pNode->BelongModel();
		BOOL pushed = pModel->PushNodeStack();
		for (CLDSNode* pSlaveNode = pModel->EnumNodeFirst(); pSlaveNode; pSlaveNode = pModel->EnumNodeNext())
			pSlaveNode->Unlock();	//Ϊ��������Ƕ�׽ڵ���������״̬
		pModel->PopNodeStack(pushed);
	}
	m_nDepth++;

	if (m_nDepth > MAX_DETECT_NODEMOVE_DEPTH)
	{
		m_bEnableDetector = false;
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,
			"0x%X�ڵ��ѳ�����ǰ�ڵ㶯̬λ�ø��µĹ��������ȣ���ֱ��ִ�е�λ�������!",pNode->handle);
	}
#ifdef __TMP_LOG_PROC_
	else
		logerr.Log("<%3d id='0x%X push stack'>",m_nDepth,pNode->handle);
#endif
}
bool CNodeMoveDetector::AppendPosSlaveNodes(CLDSNode* pMasterNode)
{
	IModel* pModel = pMasterNode->BelongModel();
	BOOL pushed = pModel->PushNodeStack();
	for (CLDSNode* pSlaveNode = pModel->EnumNodeFirst();pSlaveNode;pSlaveNode = pModel->EnumNodeNext())
	{
		if (pSlaveNode->arrRelationNode[0] == pMasterNode->handle || pSlaveNode->arrRelationNode[1] == pMasterNode->handle ||
			pSlaveNode->arrRelationNode[2] == pMasterNode->handle || pSlaveNode->arrRelationNode[3] == pMasterNode->handle)
			pSlaveNode->CalPosition();
	}
	pModel->PopNodeStack(pushed);
	return true;
}
CNodeMoveDetector::~CNodeMoveDetector()
{
	m_nDepth--;	//������Ӧ������������ĩβ���ã�������return �϶���ǰ���ã����stackʵ��
	if (m_pNode == NULL || !m_bEnableDetector)
	{
#ifdef __TMP_LOG_PROC_
		logerr.Log("</%3d id='0x%X'>",m_nDepth+1,m_pNode->handle);
#endif
		return;
	}
	if (CLDSNode::INTERNAL_TEST_VERSION&&(_localUpdateMovePartsFlag&CLDSNode::DISABLE_RECORD_RELA_PARTS)==0)
	{	//�ڲ�汾���ڵ�λ�÷����䶯ʱ,�Զ���λ�˼�
		if (m_pNode->xActivePos.IsEqual(m_xOldPos,EPS2))
		{
#ifdef __TMP_LOG_PROC_
			logerr.Log("</%3d id='0x%X'>",m_nDepth+1,m_pNode->handle);
#endif
			return;	//�ڵ�����û�з����仯
		}
		if (_localHashModifiedNodes.GetValue(m_pNode->handle) != NULL)
		{
#ifdef __TMP_LOG_PROC_
			logerr.Log("</%3d id='0x%X'>",m_nDepth+1,m_pNode->handle);
#endif
			return;	//֮ǰ�Ѵ�����ýڵ��λ�ñ��
		}
		CStackVariant<long> stackdepth(&m_nDepth,m_nDepth + 1);//���е�Ŀ����̽����ʵ�ĵݹ����
		if (!AppendPosSlaveNodes(m_pNode))
		{
#ifdef __TMP_LOG_PROC_
			logerr.Log("</%3d id='0x%X'>",m_nDepth,m_pNode->handle);
#endif
			return;
		}
		IModel* m_pModel=m_pNode->BelongModel();
		CLdsPartListStack stack(m_pModel);
		CSuperSmartPtr<CLDSPart>pPart;
		for (pPart=m_pModel->EnumPartFirst();pPart.IsHasPtr();pPart=m_pModel->EnumPartNext())
		{
			if (!pPart->IsLinePart())
				continue;
			CLDSLinePart* pLinePart=pPart.LinePartPointer();
			if ((pLinePart->pStart&&pLinePart->pStart->handle==m_pNode->handle)||(pLinePart->pEnd &&pLinePart->pEnd->handle==m_pNode->handle))
				_localHashMoveRods.SetValue(pLinePart->handle,pLinePart);
		}
	}
#ifdef __TMP_LOG_PROC_
	logerr.Log("</%3d id='0x%X'>",m_nDepth+1,m_pNode->handle);
#endif
}
//<DEVELOP_PROCESS_MARK nameId="CLDSNode.Set">
BOOL CLDSNode::Set(bool bIgnoreRepeatFlag/*=false*/)
{
	if (IsShadowObject())
	{	//Ӱ��ڵ�
		CLDSNode *pMotherNode=(CLDSNode*)MotherObject();
		if (m_iShadowObjType==1)
		{
			pMotherNode->Set();
			CTower *pTower=(CTower*)BelongModel();
			CParaCS *pAcs=pTower->FromAcsHandle(shadow.COMMON_OBJ.m_hAcs);
			if (pAcs)
			{
				pos=pAcs->CoordTrans(pMotherNode->Position(false));
				m_xArcLiftPos=pAcs->CoordTrans(pMotherNode->Position(true));
				m_xPreliftPosition=pAcs->CoordTrans(pMotherNode->xPreliftPos);
				return TRUE;
			}
			m_xPreliftPosition=pos;
			return FALSE;
		}
		else if (m_iShadowObjType==2||m_iShadowObjType==3)
		{
			CBlockParameter blockPara;
			pos=pMotherNode->Position(false);
			m_xArcLiftPos=pMotherNode->Position(true);
			m_xPreliftPosition=pMotherNode->xPreliftPos;
			if (GetBlockParameter(blockPara))
			{
				UCS_STRU cs=blockPara.m_pBlock->TransToACS(blockPara.m_pBlockRef->GetACS());
				coord_trans(pos,cs,TRUE);
				coord_trans(m_xArcLiftPos,cs,TRUE);
				coord_trans(m_xPreliftPosition,cs,TRUE);
				return TRUE;
			}
		}
		m_xPreliftPosition=pos;
		return FALSE;
	}
	CLDSArcLift lift,*pLift=NULL;
	GEPOINT face_norm;	//ƽ�ƽ��淨��
	bool bInRightSide=true;
	if (abs(m_hArcLift)>0x20)
		pLift=BelongModel()->FromArcLiftHandle(abs(m_hArcLift));
	if (pLift!=NULL)
	{
		if (pLift->m_cRotAxis=='X'&&pos.y>pLift->m_xRotOrg.y)
			bInRightSide=false;
		else if (pLift->m_cRotAxis=='Y'&&pos.x<pLift->m_xRotOrg.x)
			bInRightSide=false;
	}
	char sError[200]="";
	CLDSPoint *pDatumPoint = NULL;
	CSmartPtr<CLDSPlane>pPlane;
	f3dPoint start_pt,end_pt,vec,sec_vec,zhun_start1,zhun_end1,zhun_start2,zhun_end2;
	f3dPoint norm11,norm12,norm21,norm22,pt1,pt2,start1,end1,start2,end2,intpt;
	f3dPoint offset_org,wing_x_vec,wing_y_vec;
	f3dLine line1,line2,zhun_line1,zhun_line2;
	f3dLine lift_line1,lift_line2;//,zhun_line1,zhun_line2;
	f3dLine preliftline1,preliftline2;//,zhun_line1,zhun_line2;
	CLDSNode* pNode;
	CLDSNode* node1;
	CLDSNode* node2;
	int retCode;
	if (!bIgnoreRepeatFlag && !Lock())	//�Ѵ�����Ľڵ㲻����ڶ���
		return FALSE;
	CNodeMoveDetector detector(this);
	if (m_cPosCalType==0)	//���κ���������õ�ǰ��������������� wjh-2019.8.6
		detector.Enable(false);
	m_xPreliftPosition=pos;
	GEPOINT lineVec(0,0,1);
	bool overlappedLog=false;
	switch (m_cPosCalType)
	{
	case 0:	// ���κ������ڵ㲻�ڴ˴���(�ڽǸֵ�λ���ѵ�������)
		if (pLift)
		{
			m_xArcLiftPos=m_hArcLift>0?pLift->LiftPosTrans(pos):pos;
			m_xPreliftPosition=m_hArcLift>0?pos:pLift->ReverseLiftPosTrans(pos);
		}
		else
			m_xArcLiftPos=pos;
		break;
	case 1:	//�˼���X����ֵ�����
		if (arrRelationNode[0]==arrRelationNode[1])
#ifdef AFX_TARG_ENU_ENGLISH
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X Node's dependent nodes are overlapped,position calculation failed!",handle);
#else
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X�ڵ���������������ڵ��ص�,λ�ü���ʧ��!",handle);
#endif
		node1 = BelongModel()->FromNodeHandle(arrRelationNode[0]);
		node2 = BelongModel()->FromNodeHandle(arrRelationNode[1]);
		if (node1==NULL)
		{
			if (log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				log2file->Log("The first dependent node of 0X%X node is missing,operation error!",GetHandle());
#else 
				log2file->Log("0X%X�ڵ�ĵ�һ�������ڵ��Ѷ�ʧ,��������!",GetHandle());
#endif
			return FALSE;
		}
		else if (node2==NULL)
		{
			if (log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				log2file->Log("The second dependent node of 0X%X node is missing,operation error!",GetHandle());
#else 
				log2file->Log("0X%X�ڵ�ĵڶ��������ڵ��Ѷ�ʧ,��������!",GetHandle());
#endif
			return FALSE;
		}
		node1->Set();
		node2->Set();
		start_pt = node1->pos;
		end_pt   = node2->pos;
		Sub_Pnt(vec,end_pt,start_pt);
		if (vec.mod()<EPS&&!overlappedLog)
#ifdef AFX_TARG_ENU_ENGLISH
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X Node's dependent nodes are overlapped,position calculation failed!",handle);
#else
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X�ڵ���������������ڵ��ص�,λ�ü���ʧ��!",handle);
#endif
		normalize(vec);
		if (fabs(vec.x)>EPS)
			pos.Set(pos.x,(pos.x-start_pt.x)*vec.y/vec.x+start_pt.y,
			(pos.x-start_pt.x)*vec.z/vec.x+start_pt.z);
		if (pLift)
		{
			if (m_hArcLift>0)
			{
				face_norm=pLift->LiftVecTrans(f3dPoint(1,0,0),bInRightSide);
				m_xArcLiftPos=pLift->LiftPosTrans(pos);	//��ȫ����ѧԲ����ת
				start_pt=node1->m_xArcLiftPos;
				end_pt=node2->m_xArcLiftPos;
				if (Int3dlf(m_xArcLiftPos,start_pt,end_pt-start_pt,m_xArcLiftPos,face_norm)<1)
					m_xArcLiftPos=pos;	//Ԥ������ʧ��
				m_xPreliftPosition=pos;
			}
			else
			{
				m_xArcLiftPos=pos;
				face_norm=pLift->ReverseLiftVecTrans(f3dPoint(1,0,0),bInRightSide);
				m_xPreliftPosition=pLift->ReverseLiftPosTrans(pos);	//��ȫ����ѧԲ����ת
				start_pt=node1->xPreliftPos;
				end_pt=node2->xPreliftPos;
				if (Int3dlf(m_xPreliftPosition,start_pt,end_pt-start_pt,m_xPreliftPosition,face_norm)<1)
					m_xPreliftPosition=pos;	//Ԥ������ʧ��
			}
		}
		else
			m_xArcLiftPos=pos;
		break;
	case 2:	//�˼���Y����ֵ�����
		if (arrRelationNode[0]==arrRelationNode[1])
#ifdef AFX_TARG_ENU_ENGLISH
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X Node's dependent nodes are overlapped,position calculation failed!",handle);
#else
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X�ڵ���������������ڵ��ص�,λ�ü���ʧ��!",handle);
#endif
		node1 = BelongModel()->FromNodeHandle(arrRelationNode[0]);
		node2 = BelongModel()->FromNodeHandle(arrRelationNode[1]);
		if (node1==NULL)
		{
			if (log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				log2file->Log("The first dependent node of 0X%X node is missing,operation error!",GetHandle());
#else 
				log2file->Log("0X%X�ڵ�ĵ�һ�������ڵ��Ѷ�ʧ,��������!",GetHandle());
#endif
			return FALSE;
		}
		else if (node2==NULL)
		{
			if (log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				log2file->Log("The second dependent node of 0X%X node is missing,operation error!",GetHandle());
#else 
				log2file->Log("0X%X�ڵ�ĵڶ��������ڵ��Ѷ�ʧ,��������!",GetHandle());
#endif
			return FALSE;
		}
		node1->Set();
		node2->Set();
		start_pt = node1->pos;
		end_pt   = node2->pos;
		Sub_Pnt(vec,end_pt,start_pt);
		if (vec.mod()<EPS&&!overlappedLog)
#ifdef AFX_TARG_ENU_ENGLISH
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X Node's dependent nodes are overlapped,position calculation failed!",handle);
#else
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X�ڵ���������������ڵ��ص�,λ�ü���ʧ��!",handle);
#endif
		normalize(vec);
		if (fabs(vec.y)>EPS)
			pos.Set((pos.y-start_pt.y)*vec.x/vec.y+start_pt.x,
				pos.y,(pos.y-start_pt.y)*vec.z/vec.y+start_pt.z);
		if (pLift)
		{
			if (m_hArcLift>0)
			{
				face_norm=pLift->LiftVecTrans(f3dPoint(0,1,0),bInRightSide);
				m_xArcLiftPos=pLift->LiftPosTrans(pos);	//��ȫ����ѧԲ����ת
				start_pt=node1->m_xArcLiftPos;
				end_pt=node2->m_xArcLiftPos;
				if (Int3dlf(m_xArcLiftPos,start_pt,end_pt-start_pt,m_xArcLiftPos,face_norm)<1)
					m_xArcLiftPos=pos;	//Ԥ������ʧ��
				m_xPreliftPosition=pos;
			}
			else
			{
				m_xArcLiftPos=pos;
				face_norm=pLift->ReverseLiftVecTrans(f3dPoint(0,1,0),bInRightSide);
				m_xPreliftPosition=pLift->ReverseLiftPosTrans(pos);	//��ȫ����ѧԲ����ת
				start_pt=node1->xPreliftPos;
				end_pt=node2->xPreliftPos;
				if (Int3dlf(m_xPreliftPosition,start_pt,end_pt-start_pt,m_xPreliftPosition,face_norm)<1)
					m_xPreliftPosition=pos;	//Ԥ������ʧ��
			}
		}
		else
			m_xArcLiftPos=pos;
		break;
	case 3:	//�˼���Z����ֵ�����
		if (arrRelationNode[0]==arrRelationNode[1])
#ifdef AFX_TARG_ENU_ENGLISH
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X Node's dependent nodes are overlapped,position calculation failed!",handle);
#else
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X�ڵ���������������ڵ��ص�,λ�ü���ʧ��!",handle);
#endif
		node1 = BelongModel()->FromNodeHandle(arrRelationNode[0]);
		node2 = BelongModel()->FromNodeHandle(arrRelationNode[1]);
		if (node1==NULL)
		{
			if (log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				log2file->Log("The first dependent node of 0X%X node is missing,operation error!",GetHandle());
#else 
				log2file->Log("0X%X�ڵ�ĵ�һ�������ڵ��Ѷ�ʧ,��������!",GetHandle());
#endif
			return FALSE;
		}
		else if (node2==NULL)
		{
			if (log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				log2file->Log("The second dependent node of 0X%X node is missing,operation error!",GetHandle());
#else 
				log2file->Log("0X%X�ڵ�ĵڶ��������ڵ��Ѷ�ʧ,��������!",GetHandle());
#endif
			return FALSE;
		}
		node1->Set();
		node2->Set();
		start_pt = node1->pos;
		end_pt   = node2->pos;
		Sub_Pnt(vec,end_pt,start_pt);
		if (vec.mod()<EPS&&!overlappedLog)
#ifdef AFX_TARG_ENU_ENGLISH
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X Node's dependent nodes are overlapped,position calculation failed!",handle);
#else
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X�ڵ���������������ڵ��ص�,λ�ü���ʧ��!",handle);
#endif
		normalize(vec);
		if (fabs(vec.z)>EPS)
			pos.Set((pos.z-start_pt.z)*vec.x/vec.z+start_pt.x,(pos.z-start_pt.z)*vec.y/vec.z+start_pt.y,pos.z);
		if (pLift)
		{
			if (m_hArcLift>0)
			{
				face_norm=pLift->LiftVecTrans(f3dPoint(0,0,1),bInRightSide);
				m_xArcLiftPos=pLift->LiftPosTrans(pos);	//��ȫ����ѧԲ����ת
				start_pt=node1->m_xArcLiftPos;
				end_pt=node2->m_xArcLiftPos;
				if (Int3dlf(m_xArcLiftPos,start_pt,end_pt-start_pt,m_xArcLiftPos,face_norm)<1)
					m_xArcLiftPos=pos;	//Ԥ������ʧ��
				m_xPreliftPosition=pos;
			}
			else
			{
				m_xArcLiftPos=pos;
				face_norm=pLift->ReverseLiftVecTrans(f3dPoint(0,0,1),bInRightSide);
				m_xPreliftPosition=pLift->ReverseLiftPosTrans(pos);	//��ȫ����ѧԲ����ת
				start_pt=node1->xPreliftPos;
				end_pt=node2->xPreliftPos;
				if (Int3dlf(m_xPreliftPosition,start_pt,end_pt-start_pt,m_xPreliftPosition,face_norm)<1)
					m_xPreliftPosition=pos;	//Ԥ������ʧ��
			}
		}
		else
			m_xArcLiftPos=pos;
		break;
	case 4:	//���˼����潻��
		pNode=BelongModel()->FromNodeHandle(arrRelationNode[0]);
		if (pNode)
		{
			pNode->Set();//����������,��Ȼ��Ԥ��ʱ,����㲻��ʵʱ���µ�λ
			line1.startPt=pNode->pos;
			lift_line1.startPt=pNode->m_xArcLiftPos;
			preliftline1.startPt=pNode->xPreliftPos;
		}
		pNode=BelongModel()->FromNodeHandle(arrRelationNode[1]);
		if (pNode)
		{
			pNode->Set();//����������,��Ȼ��Ԥ��ʱ,����㲻��ʵʱ���µ�λ
			line1.endPt=pNode->pos;
			lift_line1.endPt=pNode->m_xArcLiftPos;
			preliftline1.endPt=pNode->xPreliftPos;
		}
		pNode=BelongModel()->FromNodeHandle(arrRelationNode[2]);
		if (pNode)
		{
			pNode->Set();//����������,��Ȼ��Ԥ��ʱ,����㲻��ʵʱ���µ�λ
			line2.startPt=pNode->pos;
			lift_line2.startPt=pNode->m_xArcLiftPos;
			preliftline2.startPt=pNode->xPreliftPos;
		}
		pNode=BelongModel()->FromNodeHandle(arrRelationNode[3]);
		if (pNode)
		{
			pNode->Set();//����������,��Ȼ��Ԥ��ʱ,����㲻��ʵʱ���µ�λ
			line2.endPt=pNode->pos;
			lift_line2.endPt=pNode->m_xArcLiftPos;
			preliftline2.endPt=pNode->xPreliftPos;
		}
		if (_cPosCalViceType<=0||_cPosCalViceType>3)
		{	//���θĶ�����,�˸����� -wjh 2013.8.3
			retCode = Int3dll(line1,line2,pos);
			if (retCode==0||retCode==-1)
			{
				if (log2file)
#ifdef AFX_TARG_ENU_ENGLISH
					log2file->Log("The 0X%X node's two dependent cross rod don't connect the interior point of the two line parts,data error!",handle);
#else 
					log2file->Log("�ڵ�0X%X��������������˼����������˼����ڵ�,���ݴ���!",handle);
#endif
				return FALSE;
			}
			else if (pLift)
			{
				if (this->m_hArcLift>0)
				{
					retCode=Int3dll(lift_line1,lift_line2,m_xArcLiftPos);
					m_xPreliftPosition=pos;
				}
				else
				{
					m_xArcLiftPos=pos;
					retCode=Int3dll(preliftline1,preliftline2,m_xPreliftPosition);
				}
				if (retCode==0||retCode==-1)
				{
					if (log2file)
#ifdef AFX_TARG_ENU_ENGLISH
						log2file->Log("The 0X%X node's two dependent cross prelift rod don't connect the interior point of the two rod,data error!",handle);
#else 
						log2file->Log("�ڵ�0X%X��������������˼�Ԥ���󲻽������˼����ڵ�,���ݴ���!",handle);
#endif
					return FALSE;
				}
			}
			else
				m_xArcLiftPos=pos;
		}
		else
		{
			f3dPoint face_norm;
			f3dPoint line_vec2=line2.endPt-line2.startPt;
			if (_cPosCalViceType==1)	//X-Zƽ��
				face_norm=line_vec2^f3dPoint(0,1,0);
			else if (_cPosCalViceType==2)	//Y-Zƽ��
				face_norm=line_vec2^f3dPoint(1,0,0);
			else if (_cPosCalViceType==3)	//X-Yƽ��
				face_norm=line_vec2^f3dPoint(0,0,1);
			retCode=Int3dlf(pos,line1,line2.startPt,face_norm);
			if (retCode!=1)
			{
				if (log2file)
#ifdef AFX_TARG_ENU_ENGLISH
					log2file->Log("The 0X%X node's two dependent cross rod are parallel or their projective plane are unreasonable,data error!",handle);
#else 
					log2file->Log("�ڵ�0X%X��������������˼�ƽ�л�ͶӰ�治����,���ݴ���!",handle);
#endif
				return FALSE;
			}
			else if (pLift)
			{
				if (m_hArcLift>0)
					line_vec2=lift_line2.endPt-lift_line2.startPt;
				else
					line_vec2=preliftline2.endPt-preliftline2.startPt;
				if (_cPosCalViceType==1)	//X-Zƽ��
					face_norm=line_vec2^f3dPoint(0,1,0);
				else if (_cPosCalViceType==2)	//Y-Zƽ��
					face_norm=line_vec2^f3dPoint(1,0,0);
				else if (_cPosCalViceType==3)	//X-Yƽ��
					face_norm=line_vec2^f3dPoint(0,0,1);
				if (m_hArcLift>0)
				{
					retCode=Int3dlf(m_xArcLiftPos,lift_line1,lift_line2.startPt,face_norm);
					this->m_xPreliftPosition=pos;
				}
				else
				{
					m_xArcLiftPos=pos;
					retCode=Int3dlf(m_xPreliftPosition,preliftline1,preliftline2.startPt,face_norm);
				}
				if (retCode==0||retCode==-1)
				{
					if (log2file)
#ifdef AFX_TARG_ENU_ENGLISH
						log2file->Log("The 0X%X node's two dependent cross rod are parallel or their projective plane are unreasonable,data error!",handle);
#else 
						log2file->Log("�ڵ�0X%X��������������˼�ƽ�л�ͶӰ�治����,���ݴ���!",handle);
#endif
					return FALSE;
				}
			}
			else
				m_xArcLiftPos=pos;
		}
		break;
	case 5:	//�˼��ϵı����ȷֵ�
		if (arrRelationNode[0]==arrRelationNode[1])
#ifdef AFX_TARG_ENU_ENGLISH
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X Node's dependent nodes are overlapped,position calculation failed!",handle);
#else
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X�ڵ���������������ڵ��ص�,λ�ü���ʧ��!",handle);
#endif
		node1 = BelongModel()->FromNodeHandle(arrRelationNode[0]);
		node2 = BelongModel()->FromNodeHandle(arrRelationNode[1]);
		if (node1==NULL)
		{
			if (log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				log2file->Log("The first dependent node of 0X%X node is missing,operation error!",GetHandle());
#else 
				log2file->Log("0X%X�ڵ�ĵ�һ�������ڵ��Ѷ�ʧ,��������!",GetHandle());
#endif
			return FALSE;
		}
		else if (node2==NULL)
		{
			if (log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				log2file->Log("The second dependent node of 0X%X node is missing,operation error!",GetHandle());
#else 
				log2file->Log("0X%X�ڵ�ĵڶ��������ڵ��Ѷ�ʧ,��������!",GetHandle());
#endif
			return FALSE;
		}
		node1->Set();
		node2->Set();
		vec=node2->pos- node1->pos;
		if (vec.mod()<EPS&&!overlappedLog)
#ifdef AFX_TARG_ENU_ENGLISH
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X Node's dependent nodes are overlapped,position calculation failed!",handle);
#else
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X�ڵ���������������ڵ��ص�,λ�ü���ʧ��!",handle);
#endif
		pos=node1->pos+vec*attach_scale;
		if (pLift)
		{
			if (m_hArcLift>0)
			{
				vec=node2->m_xArcLiftPos-node1->m_xArcLiftPos;
				m_xArcLiftPos= node1->m_xArcLiftPos+vec*attach_scale;
				m_xPreliftPosition=pos;
			}
			else
			{
				m_xArcLiftPos=pos;
				vec=node2->xPreliftPos-node1->xPreliftPos;
				m_xPreliftPosition= node1->xPreliftPos+vec*attach_scale;
			}
		}
		else
			m_xArcLiftPos=pos;
		break;
	case 6:	//�ظ˼���ĳ�ڵ�ȸߵĽڵ�
		pNode=BelongModel()->FromNodeHandle(arrRelationNode[0]);
		node1=BelongModel()->FromNodeHandle(arrRelationNode[1]);
		node2=BelongModel()->FromNodeHandle(arrRelationNode[2]);
		if (pNode==NULL||node1==NULL||node2==NULL)
		{	//��ʾ�����ڵ��� wht 11-06-14
			if (log2file)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (pNode==NULL)
					log2file->Log("The 0X%X node's dependent (0X%X) node  has been missing,operation error!",GetHandle(),arrRelationNode[0]);
				else if (node1==NULL)
					log2file->Log("The 0X%X node's dependent (0X%X) node  has been missing,operation error!",GetHandle(),arrRelationNode[1]);
				else //if(node2==NULL)
					log2file->Log("The 0X%X node's dependent (0X%X) node  has been missing,operation error!",GetHandle(),arrRelationNode[2]);
#else 
				if (pNode==NULL)
					log2file->Log("0X%X�ڵ�������ڵ�(0X%X)�Ѷ�ʧ,��������!",GetHandle(),arrRelationNode[0]);
				else if (node1==NULL)
					log2file->Log("0X%X�ڵ�������ڵ�(0X%X)�Ѷ�ʧ,��������!",GetHandle(),arrRelationNode[1]);
				else //if(node2==NULL)
					log2file->Log("0X%X�ڵ�������ڵ�(0X%X)�Ѷ�ʧ,��������!",GetHandle(),arrRelationNode[2]);
#endif
			}
			return FALSE;
		}
		pNode->Set();
		node1->Set();
		node2->Set();
		Sub_Pnt(vec,node2->pos,node1->pos);
		normalize(vec);
		if (m_cPosCalViceType=='X')
			pos= node1->pos+vec*(pNode->pos.x-node1->pos.x)*(vec.mod()/vec.x);
		else if (m_cPosCalViceType=='Y')
			pos= node1->pos+vec*(pNode->pos.y-node1->pos.y)*(vec.mod()/vec.y);
		else //if(m_cPosCalViceType==0||m_cPosCalViceType=='Z')
			pos= node1->pos+vec*(pNode->pos.z-node1->pos.z)*(vec.mod()/vec.z);
		if (pLift)
		{
			if (m_hArcLift>0)
			{
				Sub_Pnt(vec,node2->m_xArcLiftPos,node1->m_xArcLiftPos);
				normalize(vec);

				if (m_cPosCalViceType=='X')
					face_norm=pLift->LiftVecTrans(f3dPoint(1,0,0),bInRightSide);
				else if (m_cPosCalViceType=='Y')
					face_norm=pLift->LiftVecTrans(f3dPoint(0,1,0),bInRightSide);
				else //if(m_cPosCalViceType==0||m_cPosCalViceType=='Z')
					face_norm=pLift->LiftVecTrans(f3dPoint(0,0,1),bInRightSide);

				m_xArcLiftPos=pLift->LiftPosTrans(pos);	//��ȫ����ѧԲ����ת
				if (Int3dlf(m_xArcLiftPos,node1->m_xArcLiftPos,vec,m_xArcLiftPos,face_norm)<1)
					m_xArcLiftPos=pos;	//����ʧ��
				m_xPreliftPosition=pos;
			}
			else
			{
				m_xArcLiftPos=pos;
				Sub_Pnt(vec,node2->xPreliftPos,node1->xPreliftPos);
				normalize(vec);

				if (m_cPosCalViceType=='X')
					face_norm=pLift->ReverseLiftVecTrans(f3dPoint(1,0,0),bInRightSide);
				else if (m_cPosCalViceType=='Y')
					face_norm=pLift->ReverseLiftVecTrans(f3dPoint(0,1,0),bInRightSide);
				else //if(m_cPosCalViceType==0||m_cPosCalViceType=='Z')
					face_norm=pLift->ReverseLiftVecTrans(f3dPoint(0,0,1),bInRightSide);

				m_xPreliftPosition=pLift->ReverseLiftPosTrans(pos);	//��ȫ����ѧԲ����ת
				if (Int3dlf(m_xArcLiftPos,node1->m_xArcLiftPos,vec,m_xArcLiftPos,face_norm)<1)
					m_xArcLiftPos=pos;	//����ʧ��
			}
		}
		else
			m_xArcLiftPos=pos;
		break;
	case 7:	//�ظ˼���ĳ�ڵ��ƫ�Ƶ�
		pNode = BelongModel()->FromNodeHandle(arrRelationNode[0]);
		if (arrRelationNode[1]==arrRelationNode[2])
#ifdef AFX_TARG_ENU_ENGLISH
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X Node's dependent nodes are overlapped,position calculation failed!",handle);
#else
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X�ڵ���������������ڵ��ص�,λ�ü���ʧ��!",handle);
#endif
		node1= BelongModel()->FromNodeHandle(arrRelationNode[1]);
		node2= BelongModel()->FromNodeHandle(arrRelationNode[2]);
		if (pNode==NULL||node1==NULL||node2==NULL)
		{	//��ʾ�����ڵ��� wht 11-06-14
			if (log2file)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (pNode==NULL)
					log2file->Log("The 0X%X node's dependent (0X%X) node  has been missing,operation error!",GetHandle(),arrRelationNode[0]);
				else if (node1==NULL)
					log2file->Log("The 0X%X node's dependent (0X%X) node  has been missing,operation error!",GetHandle(),arrRelationNode[1]);
				else //if(node2==NULL)
					log2file->Log("The 0X%X node's dependent (0X%X) node  has been missing,operation error!",GetHandle(),arrRelationNode[2]);
#else 
				if (pNode==NULL)
					log2file->Log("0X%X�ڵ�������ڵ�(0X%X)�Ѷ�ʧ,��������!",GetHandle(),arrRelationNode[0]);
				else if (node1==NULL)
					log2file->Log("0X%X�ڵ�������ڵ�(0X%X)�Ѷ�ʧ,��������!",GetHandle(),arrRelationNode[1]);
				else //if(node2==NULL)
					log2file->Log("0X%X�ڵ�������ڵ�(0X%X)�Ѷ�ʧ,��������!",GetHandle(),arrRelationNode[2]);
#endif
			}
			return FALSE;
		}
		pNode->Set();
		node1->Set();
		node2->Set();
		Sub_Pnt(vec,node2->pos,node1->pos);
		if (vec.mod()<EPS&&!overlappedLog)
#ifdef AFX_TARG_ENU_ENGLISH
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X Node's dependent nodes are overlapped,position calculation failed!",handle);
#else
			overlappedLog=Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X�ڵ���������������ڵ��ص�,λ�ü���ʧ��!",handle);
#endif
		normalize(vec);
		if (m_cPosCalViceType=='X')
		{
			if (fabs(vec.x)<eps)
			{
				if (log2file)
#ifdef AFX_TARG_ENU_ENGLISH
					log2file->Log("X axial offset node 0X%X and its dependent node is paralleled each other ,data error!",GetHandle());
#else 
					log2file->Log("X����ƫ�ƽڵ�0X%X�������������ڵ��໥ˮƽ,���ݴ���",GetHandle());
#endif
				return FALSE;
			}
			Add_Pnt(pos,pNode->pos,vec*attach_offset*(vec.mod()/fabs(vec.x)));
		}
		else if (m_cPosCalViceType=='Y')
		{
			if (fabs(vec.y)<eps)
			{
				if (log2file)
#ifdef AFX_TARG_ENU_ENGLISH
					log2file->Log("Y axial offset node 0X%X and its coordinate dependent node is paralleled each other ,data error!",GetHandle());
#else 
					log2file->Log("Y����ƫ�ƽڵ�0X%X�������������ڵ��໥ˮƽ,���ݴ���",GetHandle());
#endif
				return FALSE;
			}
			Add_Pnt(pos,pNode->pos,vec*attach_offset*(vec.mod()/fabs(vec.y)));
		}
		else if (m_cPosCalViceType=='Z')
		{	//�ظ˼���ĳ�ڵ�Ĵ�ֱƫ�Ƶ�
			if (fabs(vec.z)<eps)
			{
				if (log2file)
#ifdef AFX_TARG_ENU_ENGLISH
					log2file->Log("Z axial offset node 0X%X and its coordinate dependent node is paralleled each other ,data error!",GetHandle());
#else 
					log2file->Log("Z����ƫ�ƽڵ�0X%X�������������ڵ��໥ˮƽ,���ݴ���",GetHandle());
#endif
				return FALSE;
			}
			Add_Pnt(pos,pNode->pos,vec*attach_offset*(vec.mod()/fabs(vec.z)));
		}
		else
			pos= pNode->pos+vec*attach_offset;

		if (pLift)
		{
			if (m_hArcLift>0)
			{
				vec=node2->m_xArcLiftPos-node1->m_xArcLiftPos;
				normalize(vec);
				double cosa=1.0;
				if (m_cPosCalViceType=='X')
					face_norm=pLift->LiftVecTrans(f3dPoint(1,0,0),bInRightSide);
				else if (m_cPosCalViceType=='Y')
					face_norm=pLift->LiftVecTrans(f3dPoint(0,1,0),bInRightSide);
				else if (m_cPosCalViceType=='Z')
					face_norm=pLift->LiftVecTrans(f3dPoint(0,0,1),bInRightSide);
				if (m_cPosCalViceType>='X'&&m_cPosCalViceType<='Z')
					cosa=fabs(vec*face_norm);
				m_xArcLiftPos=pNode->m_xArcLiftPos+vec*(attach_offset/cosa);
				//m_xArcLiftPos=pLift->LiftPosTrans(pos);	//��ȫ����ѧԲ����ת
				if (!face_norm.IsZero()&&Int3dlf(m_xArcLiftPos,node1->m_xArcLiftPos,vec,m_xArcLiftPos,face_norm)<1)
					m_xArcLiftPos=pos;	//����ʧ��
				this->m_xPreliftPosition=pos;
			}
			else
			{
				m_xArcLiftPos=pos;	//����ʧ��
				vec=node2->xPreliftPos-node1->xPreliftPos;
				normalize(vec);
				double cosa=1.0;
				if (m_cPosCalViceType=='X')
					face_norm=pLift->ReverseLiftVecTrans(f3dPoint(1,0,0),bInRightSide);
				else if (m_cPosCalViceType=='Y')
					face_norm=pLift->ReverseLiftVecTrans(f3dPoint(0,1,0),bInRightSide);
				else if (m_cPosCalViceType=='Z')
					face_norm=pLift->ReverseLiftVecTrans(f3dPoint(0,0,1),bInRightSide);
				if (m_cPosCalViceType>='X'&&m_cPosCalViceType<='Z')
					cosa=fabs(vec*face_norm);
				m_xPreliftPosition=pNode->xPreliftPos+vec*(attach_offset/cosa);
				//m_xArcLiftPos=pLift->LiftPosTrans(pos);	//��ȫ����ѧԲ����ת
				if (!face_norm.IsZero()&&Int3dlf(m_xPreliftPosition,node1->xPreliftPos,vec,m_xPreliftPosition,face_norm)<1)
					m_xPreliftPosition=pos;	//����ʧ��
			}
		}
		else
			m_xArcLiftPos=pos;
		break;
	case 8:		//��׼����ָ��XY�����
		pPlane= BelongModel()->FromPlaneHandle(hRelationPlane);
		if (pPlane.IsNULL())
		{
			Log2File()->Log("0X%X�ڵ�Ļ�׼��0X%Xȱʧ���޷�����λ�ü���",handle,hRelationPlane);
			return FALSE;
		}
		pPlane->UpdatePlane(BelongModel());
		lineVec.Set(0,0,1);
		if (m_cPosCalViceType=='X')
			pos.y=-pos.y;
		else if (m_cPosCalViceType=='Y')
			pos.x=-pos.x;
		else if (m_cPosCalViceType=='Z')
		{
			pos.x=-pos.x;
			pos.y=-pos.y;
		}
		else if (m_cPosCalViceType=='I')
		{
			if ((pNode=BelongModel()->FromNodeHandle(arrRelationNode[0]))!=NULL)
				pos=pNode->xOriginalPos;
			if ((pNode=BelongModel()->FromNodeHandle(arrRelationNode[1]))!=NULL)
				lineVec=pNode->xOriginalPos-GEPOINT(pos);
			else
				lineVec.Set(0,1,0);
		}
		Int3dlf(pos,pos,lineVec,pPlane->GetBasePos(),pPlane->GetWorkNorm());
		if (pLift)
		{
			lift.m_ciLiftType=pLift->m_ciLiftType;
			lift.m_cRotAxis=pLift->m_cRotAxis;
			lift.m_fLiftHeight=pLift->m_fLiftHeight;
			lift.m_xRotOrg=pLift->m_xRotOrg;
			lift.m_xDatumLiftPos=pLift->m_xDatumLiftPos;
			if (lift.m_cRotAxis=='Y'&&(m_cPosCalViceType=='Y'||m_cPosCalViceType=='Z'))
			{
				lift.m_xRotOrg.x*=-1;
				lift.m_xDatumLiftPos.x*=-1;
				pLift=&lift;
			}
			else if (lift.m_cRotAxis=='X'&&(m_cPosCalViceType=='X'||m_cPosCalViceType=='Z'))
			{
				lift.m_xRotOrg.y*=-1;
				lift.m_xDatumLiftPos.x*=-1;
				pLift=&lift;
			}
			pPlane->UpdatePlane(BelongModel(),pLift,m_hArcLift<0);
			if (m_hArcLift>0)
			{
				m_xArcLiftPos=pLift->LiftPosTrans(pos);	//��ȫ����ѧԲ����ת
				Int3dlf(m_xArcLiftPos,m_xArcLiftPos,f3dPoint(0,0,1),pPlane->GetBasePos(),pPlane->GetWorkNorm());
				m_xPreliftPosition=pos;
			}
			else
			{
				m_xArcLiftPos=pos;
				m_xPreliftPosition=pLift->ReverseLiftPosTrans(pos);	//��ȫ����ѧԲ����ת
				Int3dlf(m_xPreliftPosition,m_xPreliftPosition,f3dPoint(0,0,1),pPlane->GetBasePos(),pPlane->GetWorkNorm());
			}
		}
		if (m_cPosCalViceType=='X')
			pos.y=-pos.y;
		else if (m_cPosCalViceType=='Y')
			pos.x=-pos.x;
		else if (m_cPosCalViceType=='Z')
		{
			pos.x=-pos.x;
			pos.y=-pos.y;
		}
		if (pLift)
		{
			if (m_hArcLift>0)
			{
				if (m_cPosCalViceType=='X')
					m_xArcLiftPos.y=-m_xArcLiftPos.y;
				else if (m_cPosCalViceType=='Y')
					m_xArcLiftPos.x=-m_xArcLiftPos.x;
				else if (m_cPosCalViceType=='Z')
				{
					m_xArcLiftPos.x=-m_xArcLiftPos.x;
					m_xArcLiftPos.y=-m_xArcLiftPos.y;
				}
			}
			else
			{
				if (m_cPosCalViceType=='X')
					m_xPreliftPosition.y=-m_xPreliftPosition.y;
				else if (m_cPosCalViceType=='Y')
					m_xPreliftPosition.x=-m_xPreliftPosition.x;
				else if (m_cPosCalViceType=='Z')
				{
					m_xPreliftPosition.x=-m_xPreliftPosition.x;
					m_xPreliftPosition.y=-m_xPreliftPosition.y;
				}
			}
		}
		break;
	case 9:	//��׼����ָ��YZ�����
		pPlane= BelongModel()->FromPlaneHandle(hRelationPlane);
		if (pPlane.IsNULL())
		{
			Log2File()->Log("0X%X�ڵ�Ļ�׼��0X%Xȱʧ���޷�����λ�ü���",handle,hRelationPlane);
			return FALSE;
		}
		pPlane->UpdatePlane(BelongModel());
		if (m_cPosCalViceType=='X')
			pos.y=-pos.y;
		else if (m_cPosCalViceType=='Y')
			pos.x=-pos.x;
		else if (m_cPosCalViceType=='Z')
		{
			pos.x=-pos.x;
			pos.y=-pos.y;
		}
		Int3dlf(pos,pos,f3dPoint(1,0,0),pPlane->GetBasePos(),pPlane->GetWorkNorm());
		if (pLift)
		{
			lift.m_ciLiftType=pLift->m_ciLiftType;
			lift.m_cRotAxis=pLift->m_cRotAxis;
			lift.m_fLiftHeight=pLift->m_fLiftHeight;
			lift.m_xRotOrg=pLift->m_xRotOrg;
			lift.m_xDatumLiftPos=pLift->m_xDatumLiftPos;
			if (lift.m_cRotAxis=='Y'&&(m_cPosCalViceType=='Y'||m_cPosCalViceType=='Z'))
			{
				lift.m_xRotOrg.x*=-1;
				lift.m_xDatumLiftPos.x*=-1;
				pLift=&lift;
			}
			else if (lift.m_cRotAxis=='X'&&(m_cPosCalViceType=='X'||m_cPosCalViceType=='Z'))
			{
				lift.m_xRotOrg.y*=-1;
				lift.m_xDatumLiftPos.x*=-1;
				pLift=&lift;
			}
			pPlane->UpdatePlane(BelongModel(),pLift,m_hArcLift<0);
			if (m_hArcLift>0)
			{
				m_xArcLiftPos=pLift->LiftPosTrans(pos);	//��ȫ����ѧԲ����ת
				Int3dlf(m_xArcLiftPos,m_xArcLiftPos,f3dPoint(1,0,0),pPlane->GetBasePos(),pPlane->GetWorkNorm());
				m_xPreliftPosition=pos;
			}
			else
			{
				m_xArcLiftPos=pos;
				m_xPreliftPosition=pLift->ReverseLiftPosTrans(pos);	//��ȫ����ѧԲ����ת
				Int3dlf(m_xPreliftPosition,m_xPreliftPosition,f3dPoint(1,0,0),pPlane->GetBasePos(),pPlane->GetWorkNorm());
			}
		}
		if (m_cPosCalViceType=='X')
			pos.y=-pos.y;
		else if (m_cPosCalViceType=='Y')
			pos.x=-pos.x;
		else if (m_cPosCalViceType=='Z')
		{
			pos.x=-pos.x;
			pos.y=-pos.y;
		}
		if (pLift)
		{
			if (m_hArcLift>0)
			{
				if (m_cPosCalViceType=='X')
					m_xArcLiftPos.y=-m_xArcLiftPos.y;
				else if (m_cPosCalViceType=='Y')
					m_xArcLiftPos.x=-m_xArcLiftPos.x;
				else if (m_cPosCalViceType=='Z')
				{
					m_xArcLiftPos.x=-m_xArcLiftPos.x;
					m_xArcLiftPos.y=-m_xArcLiftPos.y;
				}
			}
			else
			{
				if (m_cPosCalViceType=='X')
					m_xPreliftPosition.y=-m_xPreliftPosition.y;
				else if (m_cPosCalViceType=='Y')
					m_xPreliftPosition.x=-m_xPreliftPosition.x;
				else if (m_cPosCalViceType=='Z')
				{
					m_xPreliftPosition.x=-m_xPreliftPosition.x;
					m_xPreliftPosition.y=-m_xPreliftPosition.y;
				}
			}
		}
		break;
	case 10:	//��׼����ָ��XZ�����
		pPlane= BelongModel()->FromPlaneHandle(hRelationPlane);
		if (pPlane.IsNULL())
		{
			Log2File()->Log("0X%X�ڵ�Ļ�׼��0X%Xȱʧ���޷�����λ�ü���",handle,hRelationPlane);
			return FALSE;
		}
		pPlane->UpdatePlane(BelongModel());
		if (m_cPosCalViceType=='X')
			pos.y=-pos.y;
		else if (m_cPosCalViceType=='Y')
			pos.x=-pos.x;
		else if (m_cPosCalViceType=='Z')
		{
			pos.x=-pos.x;
			pos.y=-pos.y;
		}
		Int3dlf(pos,pos,f3dPoint(0,1,0),pPlane->GetBasePos(),pPlane->GetWorkNorm());
		if (pLift)
		{
			lift.m_ciLiftType=pLift->m_ciLiftType;
			lift.m_cRotAxis=pLift->m_cRotAxis;
			lift.m_fLiftHeight=pLift->m_fLiftHeight;
			lift.m_xRotOrg=pLift->m_xRotOrg;
			lift.m_xDatumLiftPos=pLift->m_xDatumLiftPos;
			if (lift.m_cRotAxis=='Y'&&(m_cPosCalViceType=='Y'||m_cPosCalViceType=='Z'))
			{
				lift.m_xRotOrg.x*=-1;
				lift.m_xDatumLiftPos.x*=-1;
				pLift=&lift;
			}
			else if (lift.m_cRotAxis=='X'&&(m_cPosCalViceType=='X'||m_cPosCalViceType=='Z'))
			{
				lift.m_xRotOrg.y*=-1;
				lift.m_xDatumLiftPos.x*=-1;
				pLift=&lift;
			}
			pPlane->UpdatePlane(BelongModel(),pLift,m_hArcLift<0);
			if (m_hArcLift>0)
			{
				m_xArcLiftPos=pLift->LiftPosTrans(pos);	//��ȫ����ѧԲ����ת
				Int3dlf(m_xArcLiftPos,m_xArcLiftPos,f3dPoint(0,1,0),pPlane->GetBasePos(),pPlane->GetWorkNorm());
				m_xPreliftPosition=pos;
			}
			else
			{
				m_xArcLiftPos=pos;
				m_xPreliftPosition=pLift->ReverseLiftPosTrans(pos);	//��ȫ����ѧԲ����ת
				Int3dlf(m_xPreliftPosition,m_xPreliftPosition,f3dPoint(0,1,0),pPlane->GetBasePos(),pPlane->GetWorkNorm());
			}
		}
		if (m_cPosCalViceType=='X')
			pos.y=-pos.y;
		else if (m_cPosCalViceType=='Y')
			pos.x=-pos.x;
		else if (m_cPosCalViceType=='Z')
		{
			pos.x=-pos.x;
			pos.y=-pos.y;
		}
		if (pLift)
		{
			if (m_hArcLift>0)
			{
				if (m_cPosCalViceType=='X')
					m_xArcLiftPos.y=-m_xArcLiftPos.y;
				else if (m_cPosCalViceType=='Y')
					m_xArcLiftPos.x=-m_xArcLiftPos.x;
				else if (m_cPosCalViceType=='Z')
				{
					m_xArcLiftPos.x=-m_xArcLiftPos.x;
					m_xArcLiftPos.y=-m_xArcLiftPos.y;
				}
			}
			else
			{
				if (m_cPosCalViceType=='X')
					m_xPreliftPosition.y=-m_xPreliftPosition.y;
				else if (m_cPosCalViceType=='Y')
					m_xPreliftPosition.x=-m_xPreliftPosition.x;
				else if (m_cPosCalViceType=='Z')
				{
					m_xPreliftPosition.x=-m_xPreliftPosition.x;
					m_xPreliftPosition.y=-m_xPreliftPosition.y;
				}
			}
		}
		break;
	case 11:	//ָ����׼��
		pDatumPoint=BelongModel()->FromPointHandle(hDatumPoint);
		if (pDatumPoint)
		{
			pDatumPoint->UpdatePos(BelongModel());
			pos=pDatumPoint->Position();
			if (pLift)
			{
				if (m_hArcLift>0)
				{
					pDatumPoint->UpdatePos(BelongModel(),pLift);
					m_xArcLiftPos=pDatumPoint->Position();
					m_xPreliftPosition=pos;
				}
				else
				{
					m_xArcLiftPos=pos;
					pDatumPoint->UpdatePos(BelongModel(),pLift,true);
					m_xPreliftPosition=pDatumPoint->Position();
				}
			}
			else
				m_xArcLiftPos=pos;
		}
		break;
	case 12:
		if ((pNode=BelongModel()->FromNodeHandle(arrRelationNode[0]))==NULL)
			return FALSE;
		pNode->CalPosition();
		if (_cPosCalViceType=='X')
			pos.Set(pNode->xOriginalPos.x,-pNode->xOriginalPos.y,pNode->xOriginalPos.z);
		else if (_cPosCalViceType=='Y')
			pos.Set(-pNode->xOriginalPos.x,pNode->xOriginalPos.y,pNode->xOriginalPos.z);
		else if (_cPosCalViceType=='Z')
			pos.Set(-pNode->xOriginalPos.x,-pNode->xOriginalPos.y,pNode->xOriginalPos.z);
		else
			return FALSE;
		break;
	case 13:
		pNode = arrRelationNode[0] <= 0x20 ? NULL : BelongModel()->FromNodeHandle(arrRelationNode[0]);
		if (pNode)
		{
			pNode->CalPosition();
			pos.x = pNode->xOriginalPos.x;
		}
		pNode = arrRelationNode[1] <= 0x20 ? NULL : BelongModel()->FromNodeHandle(arrRelationNode[1]);
		if (pNode)
		{
			pNode->CalPosition();
			pos.y = pNode->xOriginalPos.y;
		}
		pNode = arrRelationNode[2] <= 0x20 ? NULL : BelongModel()->FromNodeHandle(arrRelationNode[2]);
		if (pNode)
		{
			pNode->CalPosition();
			pos.z = pNode->xOriginalPos.z;
		}
		break;
	default:
		return FALSE;
	}
	SetModified();
	return TRUE;
}
//</DEVELOP_PROCESS_MARK>
bool CLDSNode::IsIntegrityParams()	//�Ƿ�������������������
{
	if (IsShadowObject())
		return true;
	GEPOINT face_norm;	//ƽ�ƽ��淨��
	bool bInRightSide=true;
	char sError[200]="";
	CLDSPoint *pDatumPoint = NULL;
	CLDSPlane* pPlane;
	CLDSNode* pNode;
	CLDSNode* node1;
	CLDSNode* node2;
	switch (m_cPosCalType)
	{
	case 0:	// ���κ������ڵ㲻�ڴ˴���(�ڽǸֵ�λ���ѵ�������)
		return true;
	case 1:	//�˼���X����ֵ�����
	case 2:	//�˼���Y����ֵ�����
	case 3:	//�˼���Z����ֵ�����
		if (arrRelationNode[0]==arrRelationNode[1])
			return false;
		node1 = BelongModel()->FromNodeHandle(arrRelationNode[0]);
		node2 = BelongModel()->FromNodeHandle(arrRelationNode[1]);
		return (node1!=NULL&&node2!=NULL);
		if (arrRelationNode[0]==arrRelationNode[1])
			return false;
		node1 = BelongModel()->FromNodeHandle(arrRelationNode[0]);
		node2 = BelongModel()->FromNodeHandle(arrRelationNode[1]);
		return (node1!=NULL&&node2!=NULL);
	case 4:	//���˼����潻��
		pNode=BelongModel()->FromNodeHandle(arrRelationNode[0]);
		if (pNode==NULL)
			return false;
		pNode=BelongModel()->FromNodeHandle(arrRelationNode[1]);
		if (pNode==NULL)
			return false;
		pNode=BelongModel()->FromNodeHandle(arrRelationNode[2]);
		if (pNode==NULL)
			return false;
		pNode=BelongModel()->FromNodeHandle(arrRelationNode[3]);
		if (pNode==NULL)
			return false;
		return true;
	case 5:	//�˼��ϵı����ȷֵ�
		if (arrRelationNode[0]==arrRelationNode[1])
			return false;
		node1 = BelongModel()->FromNodeHandle(arrRelationNode[0]);
		node2 = BelongModel()->FromNodeHandle(arrRelationNode[1]);
		return (node1!=NULL&&node2!=NULL);
	case 6:	//�ظ˼���ĳ�ڵ�ȸߵĽڵ�
		pNode=BelongModel()->FromNodeHandle(arrRelationNode[0]);
		node1=BelongModel()->FromNodeHandle(arrRelationNode[1]);
		node2=BelongModel()->FromNodeHandle(arrRelationNode[2]);
		return (pNode!=NULL&&node1!=NULL&&node2!=NULL);
	case 7:	//�ظ˼���ĳ�ڵ��ƫ�Ƶ�
		pNode = BelongModel()->FromNodeHandle(arrRelationNode[0]);
		if (arrRelationNode[1]==arrRelationNode[2])
			return false;
		node1= BelongModel()->FromNodeHandle(arrRelationNode[1]);
		node2= BelongModel()->FromNodeHandle(arrRelationNode[2]);
		return (pNode!=NULL&&node1!=NULL&&node2!=NULL);
	case 8:		//��׼����ָ��XY�����
	case 9:		//��׼����ָ��YZ�����
	case 10:	//��׼����ָ��XZ�����
		pPlane= BelongModel()->FromPlaneHandle(hRelationPlane);
		return pPlane!=NULL;
	case 11:	//ָ����׼��
		pDatumPoint=BelongModel()->FromPointHandle(hDatumPoint);
		return pDatumPoint!=NULL;
	default:
		return false;
	}
	return false;
}
bool CLDSNode::ReverseUpdateParams()	//�������귴��������
{
	if (IsShadowObject()||!IsIntegrityParams())
		return false;
	CLDSNode* pNode;
	CLDSNode* node1;
	CLDSNode* node2;
	switch (m_cPosCalType)
	{
	case 5:	//�˼��ϵı����ȷֵ�
		if (arrRelationNode[0]==arrRelationNode[1])
			return false;
		node1 = BelongModel()->FromNodeHandle(arrRelationNode[0]);
		node2 = BelongModel()->FromNodeHandle(arrRelationNode[1]);
		if (node1!=NULL&&node2!=NULL)
		{
			GEPOINT len2start=pos-node1->Position(false);
			GEPOINT lenvec=node2->Position(false)-node1->Position(false);
			double length=lenvec.mod();
			if (length>EPS)
			{
				attach_scale=len2start.mod()/length;
				SetModified();
				return true;
			}
		}
		return false;
	case 7:	//�ظ˼���ĳ�ڵ��ƫ�Ƶ�
		pNode = BelongModel()->FromNodeHandle(arrRelationNode[0]);
		if (arrRelationNode[1]==arrRelationNode[2])
			return false;
		node1= BelongModel()->FromNodeHandle(arrRelationNode[1]);
		node2= BelongModel()->FromNodeHandle(arrRelationNode[2]);
		if (pNode!=NULL&&node1!=NULL&&node2!=NULL)
		{
			GEPOINT len2datum=pos-node1->Position(false);
			GEPOINT lenvec=node2->Position(false)-node1->Position(false);
			if (m_cPosCalViceType=='X')
			{
				if (len2datum*lenvec>0)
					attach_offset= len2datum.x;
				else
					attach_offset=-len2datum.x;
			}
			else if (m_cPosCalViceType=='Y')
			{
				if (len2datum*lenvec>0)
					attach_offset= len2datum.y;
				else
					attach_offset=-len2datum.y;
			}
			else if (m_cPosCalViceType=='Z')
			{
				if (len2datum*lenvec>0)
					attach_offset= len2datum.z;
				else
					attach_offset=-len2datum.z;
			}
			else if (len2datum*lenvec>0)
				attach_offset= len2datum.mod();
			else
				attach_offset=-len2datum.mod();
			SetModified();
			return true;
		}
	default:
		return false;
	}
	return false;
}
static const double COLINEAR_MINCOSA = 0.99;	//max sina=0.02 = 10/500
bool CLDSNode::SearchRelaDependNodeRods(COLINEAR_TOLERANCE* pTolerance/*=NULL*/)	//�Զ����������ڵ�����������ڵ㼰�˼�
{
	if (IsShadowObject()||IsIntegrityParams())
		return false;
	CLDSLinePart *pRod,*pCrossRod1=NULL,*pCrossRod2=NULL,*pColinearRod=NULL;
	CLDSLinePart* pStartRod=NULL,*pEndRod=NULL;
	CLDSNode* pStartTipNode=NULL,*pEndTipNode=NULL;
	LINEPARTSET rodset;
	GEPOINT lineStdRodVec;
	double length=0;
	COLINEAR_TOLERANCE tolerance(1);
	if (pTolerance==NULL)
		pTolerance=&tolerance;
	CLdsPartListStack stack(m_pModel);
	switch (m_cPosCalType)
	{
	case COORD_X_SPECIFIED:
	case COORD_Y_SPECIFIED:
	case COORD_Z_SPECIFIED:
	case COORD_SCALE:	//�˼��ϵı����ȷֵ�
		for (pRod=m_pModel->EnumRodFirst();pRod;pRod=m_pModel->EnumRodNext())
		{
			if (pRod->pStart==NULL||pRod->pEnd==NULL||!pRod->ModuleTogetherWith(this))
				continue;
			f3dPoint vec2start=pos-pRod->pStart->Position(false);
			f3dPoint vec2end=pos-pRod->pEnd->Position(false);
			f3dPoint linevec=pRod->pEnd->Position(false)-pRod->pStart->Position(false);
			double length=linevec.mod();
			linevec/=length;
			double prjlength=vec2start*linevec;
			f3dPoint perpvec=vec2start-prjlength*linevec;
			if (prjlength>EPS&&prjlength<length-EPS&&perpvec.mod2()<1.0)	//1mm�����������ݲ�
			{	//���Ȳ��Ҹ˼��ڵ����
				if (arrRelationNode[0]==0&&arrRelationNode[1]!=pRod->pStart->handle)
					arrRelationNode[0]=pRod->pStart->handle;
				else if (arrRelationNode[0]==0)
					arrRelationNode[0]=pRod->pEnd->handle;
				if (arrRelationNode[1]==0&&arrRelationNode[0]!=pRod->pEnd->handle)
					arrRelationNode[1]=pRod->pEnd->handle;
				else if (arrRelationNode[1]==0)
					arrRelationNode[1]=pRod->pStart->handle;
				if (hFatherPart==0)
					hFatherPart=pRod->handle;
				ReverseUpdateParams();
				CalPosition(true);
				SetModified();
				return true;
			}
			else if (pRod->pStart==this||vec2start.mod2()<=0.04)
				rodset.append(pRod);
			else if (pRod->pEnd==this||vec2end.mod2()<=0.04)
				rodset.append(pRod);
		}
		for (pColinearRod=rodset.GetFirst();pColinearRod;pColinearRod=rodset.GetNext())
		{	//����ģ���еĹ�������
			GEPOINT lineStdVec=pColinearRod->pEnd->Position(false)-pColinearRod->pStart->Position(false);
			normalize(lineStdVec);
			rodset.push_stack();
			for (pRod=rodset.GetNext();pRod;pRod=rodset.GetNext())
			{
				GEPOINT lineStdVec2=pRod->pEnd->Position(false)-pRod->pStart->Position(false);
				normalize(lineStdVec2);
				if (fabs(lineStdVec*lineStdVec2)>COLINEAR_MINCOSA)	//�ҵ����߸˼�
					break;
			}
			rodset.pop_stack();
			if (pRod!=NULL)
				break;
		}
		bool bStartIsStartNode,bEndIsStartNode;
		rodset.Empty();
		if (pColinearRod==NULL||!m_pModel->SearchSortedColinearRods(pColinearRod,rodset,&bStartIsStartNode,&bEndIsStartNode,pTolerance->distance,pTolerance->tangent))
			return false;
		pStartRod=rodset.GetFirst();
		pEndRod=rodset.GetTail();
		pStartTipNode=pStartRod->pStart;
		pEndTipNode=pEndRod->pEnd;
		if (!bStartIsStartNode)
			pStartTipNode=pStartRod->pEnd;
		if (bEndIsStartNode)
			pEndTipNode=pEndRod->pStart;
		if (arrRelationNode[0]==0&&arrRelationNode[1]!=pStartTipNode->handle)
			arrRelationNode[0]=pStartTipNode->handle;
		else if (arrRelationNode[0]==0)
			arrRelationNode[0]=pEndTipNode->handle;
		if (arrRelationNode[1]==0&&arrRelationNode[0]!=pEndTipNode->handle)
			arrRelationNode[1]=pEndTipNode->handle;
		else if (arrRelationNode[1]==0)
			arrRelationNode[1]=pStartTipNode->handle;
		length=DISTANCE(pStartTipNode->xPreliftPos,pEndTipNode->xPreliftPos);
		lineStdRodVec=pEndTipNode->xPreliftPos-pStartTipNode->xPreliftPos;
		if (length>0)
		{
			lineStdRodVec/=length;
			attach_scale=((pos-f3dPoint(pStartTipNode->xPreliftPos))*lineStdRodVec)/length;
		}
		if (arrRelationNode[0]==handle||arrRelationNode[1]==handle)
			return false;
		CalPosition(true);
		SetModified();
		return true;
	case COORD_INTERSECTION:
		for (pRod=m_pModel->EnumRodFirst();pRod;pRod=m_pModel->EnumRodNext())
		{
			if (pRod->pStart==NULL||pRod->pEnd==NULL||!pRod->ModuleTogetherWith(this))
				continue;
			f3dPoint vec2start=pos-pRod->pStart->Position(false);
			f3dPoint vec2end=pos-pRod->pEnd->Position(false);
			f3dPoint linevec=pRod->pEnd->Position(false)-pRod->pStart->Position(false);
			double length=linevec.mod();
			if (length>EPS)	//һ���������Ϊ�㣬����������ݳ���ʱ�����д�����Է�ֹ�׳��쳣 wjh-2017.11.14
				linevec/=length;
			double prjlength=vec2start*linevec;
			f3dPoint perpvec=vec2start-prjlength*linevec;
			if (prjlength>EPS&&prjlength<length-EPS&&perpvec.mod2()<1.0)	//1mm�����������ݲ�
			{	//���Ȳ��Ҹ˼��ڵ����
				if (pCrossRod1==NULL)
				{
					pCrossRod1=pRod;
					lineStdRodVec=linevec;
				}
				else if (pCrossRod2==NULL&&fabs(lineStdRodVec*linevec)<EPS_COS2)
				{
					pCrossRod2=pRod;
					if (hFatherPart==0)
						hFatherPart=pCrossRod1->handle;
					arrRelationNode[0]=pCrossRod1->pStart->handle;
					arrRelationNode[1]=pCrossRod1->pEnd->handle;
					arrRelationNode[2]=pCrossRod2->pStart->handle;
					arrRelationNode[3]=pCrossRod2->pEnd->handle;
					arrRelationPole[0]=pCrossRod1->handle;
					arrRelationPole[1]=pCrossRod2->handle;
					CalPosition(true);
					SetModified();
					return true;
				}
			}
		}
		return false;
		/*case COORD_OFFSET:	//�ظ˼���ĳ�ڵ��ƫ�Ƶ�
			pNode = BelongModel()->FromNodeHandle(arrRelationNode[0]);
			if(arrRelationNode[1]==arrRelationNode[2])
				return false;
			node1= BelongModel()->FromNodeHandle(arrRelationNode[1]);
			node2= BelongModel()->FromNodeHandle(arrRelationNode[2]);
			if(pNode!=NULL&&node1!=NULL&&node2!=NULL)
			{
				GEPOINT len2datum=pos-node1->Position(false);
				GEPOINT lenvec=node2->Position(false)-node1->Position(false);
				if(m_cPosCalViceType=='X')
				{
					if(len2datum*lenvec>0)
						attach_offset= len2datum.x;
					else
						attach_offset=-len2datum.x;
				}
				else if(m_cPosCalViceType=='Y')
				{
					if(len2datum*lenvec>0)
						attach_offset= len2datum.y;
					else
						attach_offset=-len2datum.y;
				}
				else if(m_cPosCalViceType=='Z')
				{
					if(len2datum*lenvec>0)
						attach_offset= len2datum.z;
					else
						attach_offset=-len2datum.z;
				}
				else if(len2datum*lenvec>0)
					attach_offset= len2datum.mod();
				else
					attach_offset=-len2datum.mod();
				SetModified();
				return true;
			}*/
	default:
		return false;
	}
	return false;
}

bool CLDSNode::HasRelativeNode(long hNode)
{
	switch (m_cPosCalType)
	{
	case 1:	//�˼���X����ֵ�����
	case 2:	//�˼���Y����ֵ�����
	case 3:	//�˼���Z����ֵ�����
		if (arrRelationNode[0]==hNode||arrRelationNode[1]==hNode)
			return true;
		break;
	case 4:	//���˼����潻��
		if (arrRelationNode[0]==hNode||arrRelationNode[1]==hNode||
			arrRelationNode[2]==hNode||arrRelationNode[3]==hNode)
			return true;
		break;
	case 5:	//�˼��ϵı����ȷֵ�
		if (arrRelationNode[0]==hNode||arrRelationNode[1]==hNode)
			return true;
		break;
	case 6:	//�ظ˼���ĳ�ڵ�ȸߵĽڵ�
	case 7:	//�ظ˼���ĳ�ڵ��ƫ�Ƶ�
		if (arrRelationNode[0]==hNode||arrRelationNode[1]==hNode||arrRelationNode[2]==hNode)
			return true;
		break;
	default:
		return false;
	}
	return false;
}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
void CLDSNode::GetAttachNodeFatherJg(CLDSNode *pNode1,CLDSNode *pNode2,
	CLDSLineAngle **ppJg1,CLDSLineAngle **ppJg2)
{
	if (pNode1==NULL||pNode2==NULL)
		return;
	CLDSLineAngle *pFatherJg,*pJg;
	pFatherJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	f3dPoint vec=pNode2->pos-pNode1->pos;
	f3dPoint direct=pFatherJg->get_norm_x_wing()+pFatherJg->get_norm_y_wing();
	f3dPoint vice_vec,vice_direct;
	normalize(vec);
	normalize(direct);
	double just_value=-1;	//�жϽǸֳ�����о�

	//���ҵ�һ�������ڵ�ĵ�ǰ�Ǹ�
	LINEPARTSET jgset;
	CLDSPart *pPart;
	BOOL pushed=m_pModel->PushPartStack(); //���������б�ǰӦ��ѹջ wht 12-03-05
	for (pPart=m_pModel->EnumPartFirst(CLS_LINEANGLE);pPart;pPart=m_pModel->EnumPartNext(CLS_LINEANGLE))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
		if (pLinePart->pStart==pNode1||pLinePart->pEnd==pNode1)
			jgset.append(pLinePart);
	}
	m_pModel->PopPartStack(pushed);
	if (pNode1->hFatherPart==hFatherPart||pNode1==pFatherJg->pStart||pNode1==pFatherJg->pEnd)
		*ppJg1=pFatherJg;
	else
	{
		for (pJg=(CLDSLineAngle*)jgset.GetFirst();pJg;pJg=(CLDSLineAngle*)jgset.GetNext())
		{
			vice_vec=pJg->pEnd->pos-pJg->pStart->pos;
			normalize(vice_vec);
			if (fabs(vice_vec*vec)>EPS_COS)
			{
				vice_direct=pJg->get_norm_x_wing()+pJg->get_norm_y_wing();
				normalize(vice_direct);
				double vice_just_value=vice_direct*direct;
				if (*ppJg1&&vice_just_value>just_value)
				{
					*ppJg1=pJg;
					just_value=vice_just_value;
				}
				else if (*ppJg1==NULL)
				{
					just_value=vice_just_value;
					*ppJg1=pJg;
				}
				else
					continue;
			}
		}
	}
	//���ҵڶ����ڵ�ĵ�ǰ�Ǹ�
	pushed=m_pModel->PushPartStack(); //���������б�ǰӦ��ѹջ wht 12-03-05
	for (pPart=m_pModel->EnumPartFirst(CLS_LINEANGLE);pPart;pPart=m_pModel->EnumPartNext(CLS_LINEANGLE))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
		if (pLinePart->pStart==pNode2||pLinePart->pEnd==pNode2)
			jgset.append(pLinePart);
	}
	m_pModel->PopPartStack(pushed);
	if (pNode2->hFatherPart==hFatherPart||pNode2==pFatherJg->pStart||pNode2==pFatherJg->pEnd)
		*ppJg2=pFatherJg;
	else
	{
		just_value=-1;
		for (pJg=(CLDSLineAngle*)jgset.GetFirst();pJg;pJg=(CLDSLineAngle*)jgset.GetNext())
		{
			vice_vec=pJg->pEnd->pos-pJg->pStart->pos;
			normalize(vice_vec);
			if (fabs(vice_vec*vec)>EPS_COS)
			{
				vice_direct=pJg->get_norm_x_wing()+pJg->get_norm_y_wing();
				normalize(vice_direct);
				double vice_just_value=vice_direct*direct;
				if (*ppJg2&&vice_just_value>just_value)
				{
					*ppJg2=pJg;
					just_value=vice_just_value;
				}
				else if (*ppJg2==NULL)
				{
					*ppJg2=pJg;
					just_value=vice_just_value;
				}
				else
					continue;
			}
		}
	}
}

//���ݽǸ���㷴�ƽڵ�λ�ã���ԭʼ�ڵ�λ�ÿ��ܲ�ͬ��
f3dPoint CLDSNode::GetInverseNodePos(CLDSLineAngle *pJg/*=NULL*/)
{
	f3dPoint rev_pnt;
	if (pJg==NULL)
		pJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	if (pJg==NULL)
		return rev_pnt;

	//����㷴��׼�ߵ���Դ˽Ǹֵ�λ��
	f3dPoint ber_pnt = pJg->GetDatumPosBer(this);	//�ڵ�Ϊ�Ǹֶ˽ڵ�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	if (pJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pJg;
		if (pGroupJg->group_style==1)	//T�����
		{
			if (!pGroupJg->m_bEnableTeG)
				getjgzj(pGroupJg->xWingXZhunJu,pGroupJg->GetWidth());
			rev_pnt=ber_pnt+pGroupJg->GetWingVecX()*pGroupJg->xWingXZhunJu.g;
			return rev_pnt;
		}
		else							//��T�����
			return ber_pnt;
	}
#endif
	if (!pJg->m_bEnableTeG)
	{
		getjgzj(pJg->xWingXZhunJu,pJg->GetWidth());
		getjgzj(pJg->xWingYZhunJu,pJg->GetWidth());
	}
	double a,b,c,gama;
	//�������ͶӰ���˽Ǹ���֫ƫ����Ϊ��ʱ�м����ͶӰ��������������޸�Ϊ���´��� WJH-2004/08/06
	//ԭ����Ϊ��a=pJg->xWingYZhunJu.g;b=pJg->xWingXZhunJu.g;
	if (pJg->pStart==this)
	{
		if (pJg->desStartPos.wing_y_offset.gStyle==0)
			a=pJg->xWingYZhunJu.g;
		else if (pJg->desStartPos.wing_y_offset.gStyle==1)
			a=pJg->xWingYZhunJu.g1;
		else if (pJg->desStartPos.wing_y_offset.gStyle==2)
			a=pJg->xWingYZhunJu.g2;
		else if (pJg->desStartPos.wing_y_offset.gStyle==3)
			a=pJg->xWingYZhunJu.g3;
		else if (pJg->desStartPos.wing_y_offset.gStyle==4)
			a=pJg->desStartPos.wing_y_offset.offsetDist;
		if (pJg->desStartPos.wing_x_offset.gStyle==0)
			b=pJg->xWingXZhunJu.g;
		else if (pJg->desStartPos.wing_x_offset.gStyle==1)
			b=pJg->xWingXZhunJu.g1;
		else if (pJg->desStartPos.wing_x_offset.gStyle==2)
			b=pJg->xWingXZhunJu.g2;
		else if (pJg->desStartPos.wing_x_offset.gStyle==3)
			b=pJg->xWingXZhunJu.g3;
		else if (pJg->desStartPos.wing_x_offset.gStyle==4)
			b=pJg->desStartPos.wing_x_offset.offsetDist;
	}
	else if (pJg->pEnd==this)
	{
		if (pJg->desEndPos.wing_y_offset.gStyle==0)
			a=pJg->xWingYZhunJu.g;
		else if (pJg->desEndPos.wing_y_offset.gStyle==1)
			a=pJg->xWingYZhunJu.g1;
		else if (pJg->desEndPos.wing_y_offset.gStyle==2)
			a=pJg->xWingYZhunJu.g2;
		else if (pJg->desEndPos.wing_y_offset.gStyle==3)
			a=pJg->xWingYZhunJu.g3;
		else if (pJg->desEndPos.wing_y_offset.gStyle==4)
			a=pJg->desEndPos.wing_y_offset.offsetDist;
		if (pJg->desEndPos.wing_x_offset.gStyle==0)
			b=pJg->xWingXZhunJu.g;
		else if (pJg->desEndPos.wing_x_offset.gStyle==1)
			b=pJg->xWingXZhunJu.g1;
		else if (pJg->desEndPos.wing_x_offset.gStyle==2)
			b=pJg->xWingXZhunJu.g2;
		else if (pJg->desEndPos.wing_x_offset.gStyle==3)
			b=pJg->xWingXZhunJu.g3;
		else if (pJg->desEndPos.wing_x_offset.gStyle==4)
			b=pJg->desEndPos.wing_x_offset.offsetDist;
	}
	else
	{
		if (xFatherAngleZhunJu.offset_Y_dist_style==0)
			a=pJg->xWingYZhunJu.g;
		else if (xFatherAngleZhunJu.offset_Y_dist_style==1)
			a=pJg->xWingYZhunJu.g1;
		else if (xFatherAngleZhunJu.offset_Y_dist_style==2)
			a=pJg->xWingYZhunJu.g2;
		else if (xFatherAngleZhunJu.offset_Y_dist_style==3)
			a=pJg->xWingYZhunJu.g3;
		else if (xFatherAngleZhunJu.offset_Y_dist_style==4)
			a=xFatherAngleZhunJu.offset_Y_dist;
		if (xFatherAngleZhunJu.offset_X_dist_style==0)
			b=pJg->xWingXZhunJu.g;
		else if (xFatherAngleZhunJu.offset_X_dist_style==1)
			b=pJg->xWingXZhunJu.g1;
		else if (xFatherAngleZhunJu.offset_X_dist_style==2)
			b=pJg->xWingXZhunJu.g2;
		else if (xFatherAngleZhunJu.offset_X_dist_style==3)
			b=pJg->xWingXZhunJu.g3;
		else if (xFatherAngleZhunJu.offset_X_dist_style==4)
			b=xFatherAngleZhunJu.offset_X_dist;
	}
	gama = pJg->GetRadWingAngle();
	c = (b-a*cos(gama))/sin(gama);
	//start_xyz = start_pos-GetWingVecY()*a+norm_y_wing*c;
	//����Ĺ�ʽ������һ�й�ʽ�����Ǹֻ�׼�����λ��ȷ�����������Ƴ�
	rev_pnt=ber_pnt+pJg->GetWingVecY()*a-pJg->get_norm_y_wing()*c;
	return rev_pnt;
}
#endif

void CLDSNode::CopyProperty(CLDSDbObject* pObj)
{
	if (pObj->GetClassTypeId()!=CLS_NODE)
		return;
	CLDSNode *pNode=(CLDSNode*)pObj;
	SetLayer(pNode->layer());

	m_cPosCalType = pNode->m_cPosCalType;
	m_cPosCalViceType=pNode->m_cPosCalViceType;
	attach_scale = pNode->attach_scale;
	attach_offset = pNode->attach_offset;
	iSeg = pNode->iSeg;
	cfgword=pNode->cfgword;
}

CLDSNode* CLDSNode::GetMirNode(MIRMSG mirmsg)
{
	CLDSNode *pMirNode=NULL;
	f3dPoint mir_pos = GetMirPos(pos,mirmsg);
	if (mirmsg.axis_flag==1&&fabs(pos.y-mirmsg.origin.y)<EPS)
		return this;	//X���ԶԳ����
	else if (mirmsg.axis_flag==2&&fabs(pos.x-mirmsg.origin.x)<EPS)
		return this;	//Y���ԶԳ����
	for (RELATIVE_OBJECT *pItem=relativeObjs.GetFirst();pItem;pItem=relativeObjs.GetNext())
	{	//�����ڹ����ڵ��б��в���
		pMirNode=NULL;
		if (pItem->mirInfo.IsSameMirMsg(mirmsg))
		{	//�жϸ��ݹ����ڵ��б��ҵ��ĶԳƽڵ��Ƿ�Ϸ�
			pMirNode=BelongModel()->FromNodeHandle(pItem->hObj);
			if (pMirNode&&mir_pos.IsEqual(pMirNode->pos,EPS))
				return pMirNode;
			break;
		}
	}
	CLDSNode* pAdaptMirNode=NULL;
	for (pMirNode=m_pModel->EnumNodeFirst();pMirNode!=NULL;pMirNode=m_pModel->EnumNodeNext())
	{
		if (mir_pos.IsEqual(pMirNode->pos)&&ModuleTogetherWith(pMirNode))//&&pMirNode->cfgword.And(cfgword))
		{
			if (cfgword.IsEqual(pMirNode->cfgword))
				return pMirNode;	//Ϊ��ֹ�ظ��ڵ�ʱ���ҵ��ĶԳƽڵ㲻����ǡ���ڵ�,���б�����ƥ�� wjh-2016.6.21
			else if (pAdaptMirNode==NULL)
				pAdaptMirNode=pMirNode;
		}
	}
	return pAdaptMirNode;
}
long CLDSNode::GetRelativeMirObj(MIRMSG msg,bool bIncSelf/*=TRUE*/)
{
	RELATIVE_OBJECT *pMirObj=NULL;
	for (pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		CLDSNode* pRotMirNode=NULL;
		if (pMirObj->mirInfo.axis_flag==8)
		{
			BOOL pushed=BelongModel()->PushNodeStack();
			pRotMirNode=BelongModel()->FromNodeHandle(pMirObj->hObj);
			BelongModel()->PopNodeStack(pushed);
		}
		if (fabs(pos.y)<EPS&&(
			(msg.axis_flag==0x04&&pMirObj->mirInfo.axis_flag==0x02)||
			(msg.axis_flag==0x02&&pMirObj->mirInfo.axis_flag==0x04)))
		{	//����X����ʱ��Y��Գƺ�Z��Գ���ͬ
			if (!bIncSelf&&pMirObj->hObj==handle)
				return NULL;
			else
				return pMirObj->hObj;
		}
		else if (fabs(pos.x)<EPS&&(
			(msg.axis_flag==0x04&&pMirObj->mirInfo.axis_flag==0x01)||
			(msg.axis_flag==0x01&&pMirObj->mirInfo.axis_flag==0x04)))
		{	//����Y����ʱ��X��Գƺ�Z��Գ���ͬ
			if (!bIncSelf&&pMirObj->hObj==handle)
				return NULL;
			else
				return pMirObj->hObj;
		}
		else if ((msg.axis_flag==0x01&&pRotMirNode&&fabs(pos.y+pRotMirNode->Position('Y'))<EPS)||	//X��Գ�
			(msg.axis_flag==0x02&&pRotMirNode&&fabs(pos.x+pRotMirNode->Position('X'))<EPS)||	//Y��Գ�
			(msg.axis_flag==0x04&&pRotMirNode&&fabs(pos.x+pRotMirNode->Position('X'))+fabs(pos.y+pRotMirNode->Position('Y'))<EPS))	//Z��Գ�
		{
			if (!bIncSelf&&pMirObj->hObj==handle)
				return NULL;
			else
				return pMirObj->hObj;
		}
		else if (pMirObj->mirInfo.IsSameMirMsg(msg))
		{
			if (!bIncSelf&&pMirObj->hObj==handle)
				return NULL;
			else
				return pMirObj->hObj;
		}
	}
	return 0;
}
void CLDSNode::IndependentShadowPropFromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if (doc_type==0)
		doc_type=PRODUCT_TYPE;
	if (version==0||
		(doc_type==1&&version>=4020006)||	//TMA(V4.2.0.6)
		(doc_type==2&&version>=1020006)||	//LMA(V1.2.0.6)
		(doc_type==3&&version>=2000006)||	//TSA(V2.0.0.6)
		(doc_type==4&&version>=1020006)||	//LDS(V1.2.0.6)
		(doc_type==5&&version>=1020006))	//TDA(V1.2.0.6)
	{
		buffer.ReadDword(&dwPermission);
		buffer.ReadString(layer_tag,4);
		CfgwordFromBuffer(cfgword,buffer,version,doc_type);
		BelongModel()->EvolveCfgword(cfgword,version,doc_type);
		buffer.ReadInteger(&_iSeg.iSeg);
		buffer.ReadPoint(pos);	//�ڵ�����
		m_xArcLiftPos=pos;		//��Ĭ��ֵ
		buffer.ReadDouble(&xFatherAngleZhunJu.offset_X_dist);
		buffer.ReadInteger(&xFatherAngleZhunJu.offset_X_dist_style);
		buffer.ReadDouble(&xFatherAngleZhunJu.offset_Y_dist);
		buffer.ReadInteger(&xFatherAngleZhunJu.offset_Y_dist_style);
		buffer.ReadInteger(&_hFatherPart);			//�ڵ��������Ǹ־��
		buffer.ReadString(sNotes,51);
	}
}

void CLDSNode::IndependentShadowPropToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteDword(dwPermission);
	buffer.WriteString(layer_tag);
	CfgwordToBuffer(cfgword,buffer,version,doc_type);
	buffer.WriteInteger(iSeg);
	buffer.WritePoint(pos);
	buffer.WriteDouble(xFatherAngleZhunJu.offset_X_dist);
	buffer.WriteInteger(xFatherAngleZhunJu.offset_X_dist_style);
	buffer.WriteDouble(xFatherAngleZhunJu.offset_Y_dist);
	buffer.WriteInteger(xFatherAngleZhunJu.offset_Y_dist_style);
	buffer.WriteInteger(hFatherPart);
	buffer.WriteString(sNotes);
}

void CLDSNode::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if (doc_type==0)
		doc_type=PRODUCT_TYPE;
	//��ȡӰ�乹�������͹���������� wht 12-09-14
	if (ShadowParaAndBlockIdFromBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropFromBuffer(buffer,version,doc_type);
		return;
	}
	bool bPrevDeflateVersion=false;	//�ļ�����֮ǰ�İ汾
	if (version>0&&doc_type!=5&&((doc_type==1&&version<4000027)||
		(doc_type==2&&version<1000082)||(doc_type>2&&version<1000009)))
		bPrevDeflateVersion=true;
	buffer.ReadDword(&dwPermission);	//Ȩ��
	CfgwordFromBuffer(cfgword,buffer,version,doc_type);
	BelongModel()->EvolveCfgword(cfgword,version,doc_type);
	if (version==0||
		(version>=5000003&&doc_type==1)||(version>=2000003&&doc_type==2)||
		(version>=1030003&&doc_type==4)||(version>=1030003&&doc_type==5))
		mpcl.FromBuffer(&buffer,version,doc_type);
	buffer.ReadString(layer(),4);		//�ڵ����ڲ���
	buffer.ReadInteger(&_iSeg.iSeg);
	if (version==0||
		(doc_type==1&&version>=5000200)||	//TMA V5.0.2.0
		(doc_type==2&&version>=2000200)||	//LMA V2.0.2.0
		(doc_type==3&&version>=2010100)||	//TSA V2.1.1.0
		(doc_type==4&&version>=1030200)||	//LDS V1.3.2.0
		(doc_type==5&&version>=1030200))	//TDA V1.3.2.0
		buffer.ReadBooleanThin(&_bVirtualObj);
	//�洢�ڵ��λ�����꼰��Ʋ���
	buffer.ReadPoint(pos);	//�ڵ�����
	m_xArcLiftPos=pos;		//��Ĭ��ֵ
#ifdef __COMMON_PART_INC_
	if (doc_type!=3&&(version==0||
		(doc_type==1&&version>=4020001)||	//TMA V4.2.0.1
		(doc_type==2&&version>=1020001)||	//LMA V1.2.0.1
		(doc_type==4&&version>=1020001)||	//LDS V1.2.0.1
		(doc_type==5&&version>=1020001)))	//TDA V1.2.0.1
	{
		buffer.ReadInteger(&m_hArcLift);
		buffer.ReadPoint(m_xArcLiftPos);
	}
#endif
	buffer.ReadDouble(&xFatherAngleZhunJu.offset_X_dist);
	buffer.ReadInteger(&xFatherAngleZhunJu.offset_X_dist_style);
	buffer.ReadDouble(&xFatherAngleZhunJu.offset_Y_dist);
	buffer.ReadInteger(&xFatherAngleZhunJu.offset_Y_dist_style);
	buffer.ReadInteger(&_hFatherPart);			//�ڵ��������Ǹ־��
	int i,n;
#if defined(__TSA_FILE_)||defined(__LDS_FILE_)
	if (doc_type==3||doc_type==4)
	{
		buffer.ReadInteger(&point_i);		//�ڵ���(������Ӧ���ӿ�)
		if (version==0||(version>=2010302&&doc_type==3)||(version>=1030801&&doc_type==4))
		{
			buffer.ReadByte(&m_ciWireFractions);
			buffer.ReadWord(&wireplace.data.wDataCode);
			if (version==0||(version>=1030802&&doc_type==4))//(version>=2010302&&doc_type==3)||
				buffer.ReadString(m_sHangWireDesc,51);
			else	//����wireplace�Զ�����һ���ҵ�����
				UpdateWireDesc();
		}
		else if (version==0||(version>=2010003&&doc_type==3)||(version>=1030003&&doc_type==4))
		{
			//bool suspension=false;
			//buffer.ReadBooleanThin(&suspension);
			wireplace.data.wDataCode=0;
			char ciHangWireType=0;
			buffer.ReadByte(&ciHangWireType);
			wireplace.ciWireType=ciHangWireType;
		}
		else if (version==0||
			(doc_type==3&&version>=2000101)||	//LDS V1.2.1.1
			(doc_type==4&&version>=1020101))
		{
			long hits=0;
			buffer.ReadInteger(&hits);
			for (i=0;i<hits;i++)
			{
				WIREPLACE wireplace;//* pWirePlace=wires.Add(0);
				buffer.ReadInteger((long*)&wireplace.idNodePlace);
				buffer.ReadDouble(&wireplace.fLoadAllocCoefX);
				buffer.ReadDouble(&wireplace.fLoadAllocCoefY);
				buffer.ReadDouble(&wireplace.fLoadAllocCoefZ);
				m_cHangWireType='C';
			}
		}
		int iForceType=0;
		buffer.ReadInteger(&iForceType);	//��������
		if (iForceType==1)
			force_type=FORCE_NODE;
		else if (iForceType==2)
			force_type=SWING_NODE;
		else //if(iForceType==0)
			force_type=AUTO_NODE;
		WORD wRestrict=0;
		if (version>0&&((doc_type==3&&version<2000007)||(doc_type==4&&version<1020007)))
		{
			buffer.ReadInteger(&n);
			//if(pRestrictList)
			//	pRestrictList->Empty();
			for (i=0;i<n;i++)
			{
				WORD w;
				long hModule;
				buffer.ReadInteger(&hModule);
				buffer.ReadWord(&w);
				wRestrict|=w;
				/*CNodeRestrict *pRest;
				for(pRest=pRestrictList->GetFirst();pRest;pRest=pRestrictList->GetNext())
				{
				if(pRest->m_hWorkModule==hModule)
				break;
				}
				if(pRest==NULL)
				pRest=pRestrictList->append();
				pRest->m_hWorkModule=hModule;
				pRest->SetRestrictWord(w);*/
			}
		}
		else
			buffer.ReadWord(&wRestrict);
		restrict.SetRestrictWord(wRestrict);
		if (version>0&&version<1000011)
		{
			buffer.ReadInteger(&n);
			if (n>0)
			{
				CNodeLoad load;
				for (i=0;i<n;i++)
				{
					buffer.ReadInteger(&load.m_hWorkStatus);//m_hWorkModule);
					buffer.ReadInteger(&load.m_hWorkStatus);
					buffer.ReadDouble(&load.iceFz);//Cqi);
					buffer.ReadPoint(load.setupF);//Fxyz);
				}
			}
		}
		buffer.ReadPoint(WindLoad);		//x/y/z���������ֱ��ʾ������������Զ�����ķ����
		buffer.ReadPoint(m_offset);		//�ڵ��λ��λ��
		buffer.ReadPoint(m_rotoffset);		//�ڵ����תλ��
		buffer.ReadPoint(external_force);	//�ýڵ��ϵ���ӵ�Ч������
		buffer.ReadPoint(internal_force);	//����Ԫ�㼯���ýڵ��ϵ�����
		buffer.ReadPoint(internal_moment);	//����Ԫ�㼯���ýڵ��ϵ�������
		buffer.ReadPoint(unbalance_force);	//���ڵ�Ĳ�ƽ����
		buffer.ReadPoint(PermanentLoad);	//�˽ڵ��ϵ����ú���,�����ֵӦ�������ú��ط���ϵ����ЧӦϵ��
		buffer.ReadDouble(&attachMass);	//��������
	}
#endif	

	// ------ vvvvvvv -------- �й�������ϵ�ı��� -----------
	if ((doc_type==1&&(version>0&&version<4000100)) //TMA V4.0.1.0
		||(doc_type==2&&(version>0&&version<1000100)) //LMA V1.0.1.0
		||(doc_type==3&&(version>0&&version<1070300)) //TSA V1.7.3.0
		||(doc_type==4&&(version>0&&version<1000300)))//LDS V1.0.3.0
	{
		short attach_type;	//�ֽ�Ϊ��ȡ�ɸ�ʽ�ļ���
		buffer.ReadWord(&attach_type);
		if (attach_type==4)
			_cPosCalViceType=0;
		if (attach_type==8)
			_cPosCalViceType='Z';
		if (attach_type>=8)
			attach_type-=1;
		_cPosCalType=(BYTE)attach_type;
	}
	else
	{
		buffer.ReadByte(&_cPosCalType);// �ڵ������������ͣ�
		buffer.ReadByte(&_cPosCalViceType);
		if ((doc_type==1&&version>0&&version<4010005)|| //TMA V4.1.0.5
			(doc_type==2&&version>0&&version<1010005)|| //LMA V1.1.0.5
			(doc_type==3&&version>0&&version<1080102)|| //TSA V1.8.1.2
			(doc_type==4&&version>0&&version<1010102)|| //LDS V1.1.1.2
			(doc_type==5&&version>0&&version<1010005))	//TDA V1.1.0.5
		{
			if (_cPosCalType>=8&&_cPosCalType<=10)
				_cPosCalViceType=0;
		}
		if ((doc_type==1&&version>0&&version<4010200)|| //TMA V4.1.2.0
			(doc_type==2&&version>0&&version<1010200)|| //LMA V1.1.2.0
			(doc_type==3&&version>0&&version<2000009)|| //TSA V2.0.0.9
			(doc_type==4&&version>0&&version<1020009)|| //LDS V1.2.0.9
			(doc_type==5&&version>0&&version<1020009))	//TDA V1.2.0.9
		{
			if (_cPosCalType==4)	//֮ǰ����㲻֧�������� -wjh 2013.8.3
				_cPosCalViceType=0;
		}
	}
	buffer.ReadDouble(&_attach_scale);			// �������������(�ԽǸ���ʼ�����ȫ�Ǹֳ�)
	buffer.ReadDouble(&_attach_offset);			// ƫ�ƾ���
	buffer.ReadInteger(&arrRelationNode[0]);	// �������ڵ�������ȷֵ�
	buffer.ReadInteger(&arrRelationNode[1]);	// �������ڵ�������ȷֵ�
	buffer.ReadInteger(&arrRelationNode[2]);	// �������ڵ�������ȷֵ�
	buffer.ReadInteger(&arrRelationNode[3]);	// �������ڵ�������ȷֵ�
	buffer.ReadInteger(&arrRelationPole[0]);	// �������Ǹ��󽻲潻��
	buffer.ReadInteger(&arrRelationPole[1]);	// �������Ǹ��󽻲潻��
	buffer.ReadInteger(&hRelationPlane);
	if (version==0||
		(doc_type==1&&version>=4010100)||	//TMA(V4.1.1.0)
		(doc_type==2&&version>=1010100)||	//LMA(V1.1.1.0)
		(doc_type==3&&version>=2000100)||	//TSA(V2.0.1.0)
		(doc_type==4&&version>=1020100)||	//LDS(V1.2.1.0)
		(doc_type==5&&version>=1020100))	//TDA(V1.2.1.0)
		buffer.ReadInteger(&hDatumPoint);
	else
		hDatumPoint=0;

	buffer.ReadInteger(&n);
	RELATIVE_OBJECT *pRelaObj=NULL;
	relativeObjs.Empty();
	for (i=0;i<n;i++)
	{
		pRelaObj=relativeObjs.Add(buffer.ReadInteger());
		if (bPrevDeflateVersion)
		{
			DWORD dw;
			buffer.ReadDword(&dw);
			pRelaObj->mirInfo.axis_flag=(BYTE)dw;
			buffer.ReadInteger(&dw);
			pRelaObj->mirInfo.array_num=(BYTE)dw;
			buffer.ReadPoint(pRelaObj->mirInfo.origin);
			double ff;
			buffer.ReadDouble(&ff);
			pRelaObj->mirInfo.rotate_angle=(short)ff;
		}
		else
		{
			buffer.ReadByte(&pRelaObj->mirInfo.axis_flag);
			bool bSpecOrigin;
			buffer.ReadBooleanThin(&bSpecOrigin);
			if (bSpecOrigin)
				buffer.ReadPoint(pRelaObj->mirInfo.origin);
			if (pRelaObj->mirInfo.axis_flag&0x08)
			{
				buffer.ReadByte(&pRelaObj->mirInfo.array_num);
				buffer.ReadWord(&pRelaObj->mirInfo.rotate_angle);
			}
		}
	}
	buffer.ReadString(sNotes,51);
}
void CLDSNode::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//����Ӱ�乹�������͹���������� wht 12-09-14
	if (doc_type==0)
		doc_type=PRODUCT_TYPE;
	if (ShadowParaAndBlockIdToBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropToBuffer(buffer,version,doc_type);
		return;
	}
	buffer.WriteDword(dwPermission);	//Ȩ��
	CfgwordToBuffer(cfgword,buffer,version,doc_type);
	if (version==0||
		(version>=5000003&&doc_type==1)||(version>=2000003&&doc_type==2)||
		(version>=1030003&&doc_type==4)||(version>=1030003&&doc_type==5))
		mpcl.ToBuffer(&buffer,version,doc_type);
	buffer.WriteString(layer());		//�ڵ����ڲ���
	buffer.WriteInteger(iSeg);
	if (version==0||
		(doc_type==1&&version>=5000200)||	//TMA V5.0.2.0
		(doc_type==2&&version>=2000200)||	//LMA V2.0.2.0
		(doc_type==3&&version>=2010100)||	//LMA V2.1.1.0
		(doc_type==4&&version>=1030200)||	//LDS V1.3.2.0
		(doc_type==5&&version>=1030200))	//TDA V1.3.2.0
		buffer.WriteBooleanThin(_bVirtualObj);
#ifdef __COMMON_PART_INC_
	if (version==0||
		(doc_type==1&&version>=4020001)||	//TMA V4.2.0.1
		(doc_type==2&&version>=1020001)||	//LMA V1.2.0.1
		//(doc_type==3&&version>=1080001)||	//TSA V1.8.0.1
		(doc_type==4&&version>=1020001)||	//LDS V1.2.0.1
		(doc_type==5&&version>=1020001))	//TDA V1.2.0.1
	{
		buffer.WritePoint(pos);	//ԭʼ�ڵ�����
		buffer.WriteInteger(m_hArcLift);
		buffer.WritePoint(m_xArcLiftPos);	//Ԥ����ڵ�����
	}
	else if (m_hArcLift>0x20)
		buffer.WritePoint(m_xArcLiftPos);	//���Ͱ汾ʱ�洢Ԥ����ڵ�����
	else
		buffer.WritePoint(pos);	//���Ͱ汾ʱ�洢Ԥ����ڵ�����
#endif
	buffer.WriteDouble(xFatherAngleZhunJu.offset_X_dist);
	buffer.WriteInteger(xFatherAngleZhunJu.offset_X_dist_style);
	buffer.WriteDouble(xFatherAngleZhunJu.offset_Y_dist);
	buffer.WriteInteger(xFatherAngleZhunJu.offset_Y_dist_style);
	buffer.WriteInteger(hFatherPart);			//�ڵ��������Ǹ־��
#if defined(__TSA_)||defined(__LDS_)||defined(__TSA_FILE_)||defined(__LDS_FILE_)
	if (doc_type==3||doc_type==4)
	{
		buffer.WriteInteger(point_i);		//�ڵ���(������Ӧ���ӿ�)
		if (version==0||(version>=2010302&&doc_type==3)||(version>=1030801&&doc_type==4))
		{
			buffer.WriteByte(this->m_ciWireFractions);
			buffer.WriteWord(wireplace.data.wDataCode);
			if (version==0||(version>=1030802&&doc_type==4))//(version>=2010302&&doc_type==3)||
			{
				if (m_sHangWireDesc.GetLength()==0)
					UpdateWireDesc();	//����wireplace�Զ�����һ���ҵ�����
				buffer.WriteString(m_sHangWireDesc);
			}
		}
		else if (version==0||(version>=2010003&&doc_type==3)||(version>=1030003&&doc_type==4))
			buffer.WriteByte(m_cHangWireType);
		//buffer.WriteBooleanThin(wires.GetNodeNum()>0);//m_bSuspension
		else if (version==0||
			(doc_type==3&&version>=2000101)||	//LDS V1.2.1.1
			(doc_type==4&&version>=1020101))
		{
			//buffer.PushPositionStack();
			//long hits=0;
			buffer.WriteInteger(0);//accessaryNodePlace.GetNodeNum());
			//for(WIREPLACE* pWirePlace=wires.GetFirst();pWirePlace;pWirePlace=wires.GetNext())
			//{
			//	if(pWirePlace->idNodePlace.Id()==0)
			//		continue;
			//	buffer.WriteInteger(pWirePlace->idNodePlace);
			//	buffer.WriteDouble(pWirePlace->fLoadAllocCoefX);
			//	buffer.WriteDouble(pWirePlace->fLoadAllocCoefY);
			//	buffer.WriteDouble(pWirePlace->fLoadAllocCoefZ);
			//	hits++;
			//}
			//buffer.PopPositionStack();
			//buffer.WriteInteger(hits);
			//buffer.RestoreNowPosition();
		}
		buffer.WriteInteger(force_type);	//��������
		buffer.WriteWord(restrict);
		buffer.WritePoint(WindLoad);		//x/y/z���������ֱ��ʾ������������Զ�����ķ����
		buffer.WritePoint(m_offset);		//�ڵ��λ��λ��
		buffer.WritePoint(m_rotoffset);		//�ڵ����תλ��
		buffer.WritePoint(external_force);	//�ýڵ��ϵ���ӵ�Ч������
		buffer.WritePoint(internal_force);	//����Ԫ�㼯���ýڵ��ϵ�����
		buffer.WritePoint(internal_moment);	//����Ԫ�㼯���ýڵ��ϵ�������
		buffer.WritePoint(unbalance_force);	//���ڵ�Ĳ�ƽ����
		buffer.WritePoint(PermanentLoad);	//�˽ڵ��ϵ����ú���,�����ֵӦ�������ú��ط���ϵ����ЧӦϵ��
		buffer.WriteDouble(attachMass);	//��������
	}
#endif
	// ------ vvvvvvv -------- �й�������ϵ�ı��� -----------
	if (m_hArcLift<0x20||_cPosCalType<8||version==0||
		(doc_type==1&&version>=4020001)||	//TMA V4.2.0.1
		(doc_type==2&&version>=1020001)||	//LMA V1.2.0.1
		//(doc_type==3&&version>=1080001)||	//TSA V1.8.0.1
		(doc_type==4&&version>=1020001)||	//LDS V1.2.0.1
		(doc_type==5&&version>=1020001))	//TDA V1.2.0.1
		buffer.WriteByte(_cPosCalType);
	else
		buffer.WriteByte(0);	//���汾�洢ʱ�����ڵͰ汾��֧�ָ�����>��8�����ͣ�����תΪ���κ�����������
	buffer.WriteByte(_cPosCalViceType);
	buffer.WriteDouble(_attach_scale);		// �������������(�ԽǸ���ʼ�����ȫ�Ǹֳ�)
	buffer.WriteDouble(_attach_offset);		// ƫ�ƾ���
	buffer.WriteInteger(arrRelationNode[0]);	// �������ڵ�������ȷֵ�
	buffer.WriteInteger(arrRelationNode[1]);	// �������ڵ�������ȷֵ�
	buffer.WriteInteger(arrRelationNode[2]);	// �������ڵ�������ȷֵ�
	buffer.WriteInteger(arrRelationNode[3]);	// �������ڵ�������ȷֵ�
	buffer.WriteInteger(arrRelationPole[0]);	// �������Ǹ��󽻲潻��
	buffer.WriteInteger(arrRelationPole[1]);	// �������Ǹ��󽻲潻��
	buffer.WriteInteger(hRelationPlane);
	buffer.WriteInteger(hDatumPoint);

	RELATIVE_OBJECT *pRelaObj=NULL;
	BUFFERPOP stack(&buffer,relativeObjs.GetNodeNum());
	buffer.WriteInteger(relativeObjs.GetNodeNum());
	for (pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		buffer.WriteInteger(pRelaObj->hObj);
		buffer.WriteByte(pRelaObj->mirInfo.axis_flag);
		bool bSpecOrigin=!pRelaObj->mirInfo.origin.IsZero();
		buffer.WriteBooleanThin(bSpecOrigin);
		if (bSpecOrigin)
			buffer.WritePoint(pRelaObj->mirInfo.origin);
		if (pRelaObj->mirInfo.axis_flag&0x08)
		{
			buffer.WriteByte(pRelaObj->mirInfo.array_num);
			buffer.WriteWord(pRelaObj->mirInfo.rotate_angle);
		}
		stack.Increment();
	}
	stack.VerifyAndRestore();
	buffer.WriteString(sNotes);
}
void CLDSNode::FromCdfBuffer(CBuffer &buffer,long version/*=0*/,bool bReadForCheck/*=false*/)
{
	//��ȡӰ�乹�������͹���������� wht 12-09-14
	if (ShadowParaAndBlockIdFromBuffer(buffer))//,version,doc_type))
	{
		IndependentShadowPropFromBuffer(buffer);//,version,doc_type);
		return;
	}
	buffer.ReadDword(&dwPermission);	//Ȩ��
	buffer.Read(cfgword.flag.word,24);
	buffer.ReadString(layer(),4);		//�ڵ����ڲ���
	buffer.ReadInteger(&_iSeg.iSeg);
	if (version==0||version>=1000001)
		buffer.ReadInteger(&point_i);
	buffer.ReadBooleanThin(&_bVirtualObj);
	//�洢�ڵ��λ�����꼰��Ʋ���
	buffer.ReadPoint(pos);	//�ڵ�����
	buffer.ReadInteger(&_hFatherPart);			//�ڵ��������Ǹ־��
	int i,n;
	// ------ vvvvvvv -------- �й�������ϵ�ı��� -----------
	buffer.ReadByte(&_cPosCalType);// �ڵ������������ͣ�
	buffer.ReadByte(&_cPosCalViceType);
	buffer.ReadDouble(&_attach_scale);			// �������������(�ԽǸ���ʼ�����ȫ�Ǹֳ�)
	buffer.ReadDouble(&_attach_offset);			// ƫ�ƾ���
	buffer.ReadInteger(&arrRelationNode[0]);	// �������ڵ�������ȷֵ�
	buffer.ReadInteger(&arrRelationNode[1]);	// �������ڵ�������ȷֵ�
	buffer.ReadInteger(&arrRelationNode[2]);	// �������ڵ�������ȷֵ�
	buffer.ReadInteger(&arrRelationNode[3]);	// �������ڵ�������ȷֵ�
	buffer.ReadInteger(&arrRelationPole[0]);	// �������Ǹ��󽻲潻��
	buffer.ReadInteger(&arrRelationPole[1]);	// �������Ǹ��󽻲潻��
	buffer.ReadInteger(&hRelationPlane);
	buffer.ReadInteger(&hDatumPoint);

	buffer.ReadInteger(&n);
	RELATIVE_OBJECT *pRelaObj=NULL;
	relativeObjs.Empty();
	for (i=0;i<n;i++)
	{
		pRelaObj=relativeObjs.Add(buffer.ReadInteger());
		buffer.ReadByte(&pRelaObj->mirInfo.axis_flag);
		bool bSpecOrigin;
		buffer.ReadBooleanThin(&bSpecOrigin);
		if (bSpecOrigin)
			buffer.ReadPoint(pRelaObj->mirInfo.origin);
		if (pRelaObj->mirInfo.axis_flag&0x08)
		{
			buffer.ReadByte(&pRelaObj->mirInfo.array_num);
			buffer.ReadWord(&pRelaObj->mirInfo.rotate_angle);
		}
	}
}
void CLDSNode::ToCdfBuffer(CBuffer &buffer,long version/*=0*/)
{
	//����Ӱ�乹�������͹���������� wht 12-09-14
	if (ShadowParaAndBlockIdToBuffer(buffer))//,version,doc_type))
	{
		IndependentShadowPropToBuffer(buffer);//,version,doc_type);
		return;
	}
	buffer.WriteDword(dwPermission);	//Ȩ��
	buffer.Write(cfgword.flag.word,24);
	buffer.WriteString(layer());		//�ڵ����ڲ���
	buffer.WriteInteger(iSeg);
	if (version==0||version>=1000001)
		buffer.WriteInteger(point_i);
	buffer.WriteBooleanThin(_bVirtualObj);
	buffer.WritePoint(pos);	//ԭʼ�ڵ�����
	buffer.WriteInteger(hFatherPart);			//�ڵ��������Ǹ־��
	// ------ vvvvvvv -------- �й�������ϵ�ı��� -----------
	buffer.WriteByte(_cPosCalType);	// �ڵ�������������
	buffer.WriteByte(_cPosCalViceType);
	buffer.WriteDouble(_attach_scale);		// �������������(�ԽǸ���ʼ�����ȫ�Ǹֳ�)
	buffer.WriteDouble(_attach_offset);		// ƫ�ƾ���
	buffer.WriteInteger(arrRelationNode[0]);	// �������ڵ�������ȷֵ�
	buffer.WriteInteger(arrRelationNode[1]);	// �������ڵ�������ȷֵ�
	buffer.WriteInteger(arrRelationNode[2]);	// �������ڵ�������ȷֵ�
	buffer.WriteInteger(arrRelationNode[3]);	// �������ڵ�������ȷֵ�
	buffer.WriteInteger(arrRelationPole[0]);	// �������Ǹ��󽻲潻��
	buffer.WriteInteger(arrRelationPole[1]);	// �������Ǹ��󽻲潻��
	buffer.WriteInteger(hRelationPlane);
	buffer.WriteInteger(hDatumPoint);

	RELATIVE_OBJECT *pRelaObj=NULL;
	BUFFERPOP stack(&buffer,relativeObjs.GetNodeNum());
	buffer.WriteInteger(relativeObjs.GetNodeNum());
	for (pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		buffer.WriteInteger(pRelaObj->hObj);
		buffer.WriteByte(pRelaObj->mirInfo.axis_flag);
		bool bSpecOrigin=!pRelaObj->mirInfo.origin.IsZero();
		buffer.WriteBooleanThin(bSpecOrigin);
		if (bSpecOrigin)
			buffer.WritePoint(pRelaObj->mirInfo.origin);
		if (pRelaObj->mirInfo.axis_flag&0x08)
		{
			buffer.WriteByte(pRelaObj->mirInfo.array_num);
			buffer.WriteWord(pRelaObj->mirInfo.rotate_angle);
		}
		stack.Increment();
	}
	stack.VerifyAndRestore();
}

void CLDSNode::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="",sText2[500]="",sText3[500]="";
	fprintf(fp,"<�ڵ� handle=\"0X%X\">\n",handle);
	fprintf(fp,"<Ȩ���� dwPermission=\"%d\"/>\n",dwPermission);
	fprintf(fp,"<��ĺ� cfgword.word=\"%d,%d,%d,%d,%d\"/>\n",cfgword.flag.word[0],cfgword.flag.word[1],cfgword.flag.word[2],cfgword.flag.word[3],cfgword.flag.word[4]);
	fprintf(fp,"<ͼ�� layer=\"%s\"/>\n",layer());
	fprintf(fp,"<�ֶκ� iSeg=\"%d\"/>\n",iSeg);
	sprintf(sText,"%f",pos.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",pos.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",pos.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<���� pos=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
#ifdef __COMMON_PART_INC_
	fprintf(fp,"<Բ��Ԥ�� m_hArcLift=\"%d\"/>\n",m_hArcLift);
	sprintf(sText,"%f",m_xArcLiftPos.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",m_xArcLiftPos.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",m_xArcLiftPos.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<Ԥ�������� m_xArcLiftPos=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
#endif
	sprintf(sText,"%f",xFatherAngleZhunJu.offset_X_dist);
	SimplifiedNumString(sText);
	fprintf(fp,"<�Ի�׼���X֫ƫ���� xFatherAngleZhunJu.offset_X_dist=\"%s\"/>\n",sText);
	if (xFatherAngleZhunJu.offset_X_dist_style==0)
		fprintf(fp,"<X֫ƫ�Ʒ�ʽ xFatherAngleZhunJu.offset_X_dist_style=\"0.��׼׼��\"/>\n");
	else if (xFatherAngleZhunJu.offset_X_dist_style==1)
		fprintf(fp,"<X֫ƫ�Ʒ�ʽ xFatherAngleZhunJu.offset_X_dist_style=\"1.g1\"/>\n");
	else if (xFatherAngleZhunJu.offset_X_dist_style==2)
		fprintf(fp,"<X֫ƫ�Ʒ�ʽ xFatherAngleZhunJu.offset_X_dist_style=\"2.g2\"/>\n");
	else if (xFatherAngleZhunJu.offset_X_dist_style==3)
		fprintf(fp,"<X֫ƫ�Ʒ�ʽ xFatherAngleZhunJu.offset_X_dist_style=\"3.g3\"/>\n");
	else if (xFatherAngleZhunJu.offset_X_dist_style==4)
		fprintf(fp,"<X֫ƫ�Ʒ�ʽ xFatherAngleZhunJu.offset_X_dist_style=\"4.�Զ���\"/>\n");
	sprintf(sText,"%f",xFatherAngleZhunJu.offset_Y_dist);
	SimplifiedNumString(sText);
	fprintf(fp,"<�Ի�׼���Y֫ƫ���� xFatherAngleZhunJu.offset_Y_dist=\"%s\"/>\n",sText);
	if (xFatherAngleZhunJu.offset_Y_dist_style==0)
		fprintf(fp,"<Y֫ƫ�Ʒ�ʽ xFatherAngleZhunJu.offset_Y_dist_style=\"0.��׼׼��\"/>\n");
	else if (xFatherAngleZhunJu.offset_Y_dist_style==1)
		fprintf(fp,"<Y֫ƫ�Ʒ�ʽ xFatherAngleZhunJu.offset_Y_dist_style=\"1.g1\"/>\n");
	else if (xFatherAngleZhunJu.offset_Y_dist_style==2)
		fprintf(fp,"<Y֫ƫ�Ʒ�ʽ xFatherAngleZhunJu.offset_Y_dist_style=\"2.g2\"/>\n");
	else if (xFatherAngleZhunJu.offset_Y_dist_style==3)
		fprintf(fp,"<Y֫ƫ�Ʒ�ʽ xFatherAngleZhunJu.offset_Y_dist_style=\"3.g3\"/>\n");
	else if (xFatherAngleZhunJu.offset_Y_dist_style==4)
		fprintf(fp,"<Y֫ƫ�Ʒ�ʽ xFatherAngleZhunJu.offset_Y_dist_style=\"4.�Զ���\"/>\n");
	fprintf(fp,"<�����Ǹ־�� hFatherPart=\"0X%X\"/>\n",hFatherPart);
#if defined(__TSA_)||defined(__LDS_)||defined(__TSA_FILE_)||defined(__LDS_FILE_)
	fprintf(fp,"<�ڵ��� point_i=\"%d\"/>\n",point_i);
	fprintf(fp,"<�������� force_type=\"%d\"/>\n",force_type);
	fprintf(fp,"<�ڵ�Լ�� val=\"%d\">\n",(WORD)restrict);
	sprintf(sText,"%f",WindLoad.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",WindLoad.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",WindLoad.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<xyz��������ķ���� WindLoad=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",m_offset.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",m_offset.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",m_offset.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<�ڵ��λ��λ�� m_offset=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",m_rotoffset.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",m_rotoffset.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",m_rotoffset.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<�ڵ����תλ�� m_rotoffset=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",external_force.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",external_force.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",external_force.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<��ӵ�Ч������ external_force=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",internal_force.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",internal_force.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",internal_force.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<����Ԫ�㼯���ýڵ��ϵ����� internal_force=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",internal_moment.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",internal_moment.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",internal_moment.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<����Ԫ�㼯���ýڵ��ϵ������� internal_moment=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",unbalance_force.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",unbalance_force.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",unbalance_force.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<���ڵ�Ĳ�ƽ���� unbalance_force=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",PermanentLoad.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",PermanentLoad.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",PermanentLoad.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<���ú��� PermanentLoad=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",attachMass);
	SimplifiedNumString(sText);
	fprintf(fp,"<������ʩ������������ attachMass=\"%s\"/>\n",sText);
#endif
	// ------ vvvvvvv -------- �й�������ϵ�ı��� -----------
	if (m_cPosCalType==0)
		fprintf(fp,"<�ڵ������������� m_cPosCalType=\"0.���κ�����\"/>\n");
	else if (m_cPosCalType==1)
		fprintf(fp,"<�ڵ������������� m_cPosCalType=\"1.�˼���X����ֵ�����\"/>\n");
	else if (m_cPosCalType==2)
		fprintf(fp,"<�ڵ������������� m_cPosCalType=\"2.�˼���Y����ֵ�����\"/>\n");
	else if (m_cPosCalType==3)
		fprintf(fp,"<�ڵ������������� m_cPosCalType=\"3.�˼���Z����ֵ�����\"/>\n");
	else if (m_cPosCalType==4)
		fprintf(fp,"<�ڵ������������� m_cPosCalType=\"4.���˼����潻��\"/>\n");
	else if (m_cPosCalType==5)
		fprintf(fp,"<�ڵ������������� m_cPosCalType=\"5.�˼��ϵı����ȷֵ�\"/>\n");
	else if (m_cPosCalType==6)
		fprintf(fp,"<�ڵ������������� m_cPosCalType=\"6.�ظ˼���ĳ�ڵ����������Ľڵ�\"/>\n");
	else if (m_cPosCalType==7)
		fprintf(fp,"<�ڵ������������� m_cPosCalType=\"7.�ظ˼���ĳ�ڵ��ƫ�Ƶ�\"/>\n");
	else if (m_cPosCalType==8)
		fprintf(fp,"<�ڵ������������� m_cPosCalType=\"8.��׼����XY����ֵ�����\"/>\n");
	else if (m_cPosCalType==9)
		fprintf(fp,"<�ڵ������������� m_cPosCalType=\"9.��׼����YZ����ֵ�����\"/>\n");
	else if (m_cPosCalType==10)
		fprintf(fp,"<�ڵ������������� m_cPosCalType=\"10.��׼����XZ����ֵ�����\"/>\n");
	fprintf(fp,"<����ڵ�����������͵���������������� m_cPosCalViceType=\"%d\"/>\n",m_cPosCalViceType);
	sprintf(sText,"%f",attach_scale);
	SimplifiedNumString(sText);
	fprintf(fp,"<������������� attach_scale=\"%s\"/>\n",sText);
	sprintf(sText,"%f",attach_offset);
	SimplifiedNumString(sText);
	fprintf(fp,"<ƫ�ƾ��� attach_offset=\"%s\"/>\n",sText);
	fprintf(fp,"<�������ڵ�������ȷֵ� arrRelationNode=\"(%d,%d,%d,%d)\"/>\n",arrRelationNode[0],arrRelationNode[1],arrRelationNode[2],arrRelationNode[3]);
	fprintf(fp,"<�������Ǹ��󽻲潻�� arrRelationPole=\"(%d,%d)\"/>\n",arrRelationPole[0],arrRelationPole[1]);
	fprintf(fp,"<��ػ�׼���� hRelationPlane=\"0X%X\"/>\n",hRelationPlane);
	fprintf(fp,"<��ػ�׼���� hDatumPoint=\"0X%X\"/>\n",hDatumPoint);

	fprintf(fp,"<���������б�>\n");
	fprintf(fp,"<���������� relativeObjs.GetNodeNum=\"%d\"/>\n",relativeObjs.GetNodeNum());
	RELATIVE_OBJECT *pRelaObj=NULL;
	for (pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		fprintf(fp,"<�������� hObj=\"OX%X\"",pRelaObj->hObj);
		fprintf(fp," mirInfo.axis_flag=\"%d\"",pRelaObj->mirInfo.axis_flag);
		bool bSpecOrigin=!pRelaObj->mirInfo.origin.IsZero();
		if (bSpecOrigin)
		{
			fprintf(fp," bSpecOrigin=\"true\"");
			sprintf(sText,"%f",pRelaObj->mirInfo.origin.x);
			SimplifiedNumString(sText);
			sprintf(sText2,"%f",pRelaObj->mirInfo.origin.y);
			SimplifiedNumString(sText2);
			sprintf(sText3,"%f",pRelaObj->mirInfo.origin.z);
			SimplifiedNumString(sText3);
			fprintf(fp," mirInfo.origin=\"(%s,%s,%s)\"",sText,sText2,sText3);
		}
		else
			fprintf(fp," bSpecOrigin=\"false\"");
		if (pRelaObj->mirInfo.axis_flag&0x08)
		{
			fprintf(fp," mirInfo.array_num=\"%d\"",pRelaObj->mirInfo.array_num);
			fprintf(fp," mirInfo.rotate_angle=\"%d\"",pRelaObj->mirInfo.rotate_angle);
		}
		fprintf(fp,"/>\n");
	}
	fprintf(fp,"</���������б�>\n");
	fprintf(fp,"<��ע sNotes=\"%s\"/>\n",sNotes);
	fprintf(fp,"</�ڵ�>\n");
}
void CLDSNode::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/,bool bAddSubLevelRelaObj/*=true*/)
{
	if (_console==NULL||m_pModel==NULL||!Lock())	//�ѵ��ù�)
		return;
	_console->AddToObjSetBuf(this);
	if (!bAddRelaObj)
		return;
	bAddRelaObj=bAddSubLevelRelaObj;
	CLDSNode* pNode=NULL;
	CLDSPart* pPart=NULL;
	if ((pPart=hFatherPart>0?m_pModel->FromPartHandle(this->hFatherPart):NULL)!=NULL)
		pPart->AddToConsoleObjBuf(bAddRelaObj);
	if (m_cPosCalType==0)
		return;
	int i;
	for (i=0;i<4;i++)
	{
		if (arrRelationNode[i]>0x20&&(pNode=m_pModel->FromNodeHandle(arrRelationNode[i]))!=NULL)
			pNode->AddToConsoleObjBuf(bAddRelaObj);
	}
	/*
	for(i=0;i<2;i++)
	{
		if(arrRelationPole[i]>0)
		{
			pPart=m_pModel->FromPartHandle(arrRelationPole[i]);
			if(pPart)
				pPart->AddToConsoleObjBuf(bAddRelaObj);
		}
	}*/
	if (hRelationPlane>0x20)
	{
		CLDSPlane* pPlane=m_pModel->FromPlaneHandle(hRelationPlane);
		if (pPlane)
			pPlane->AddToConsoleObjBuf(bAddRelaObj);
	}
	if (hDatumPoint>0x20)
	{
		CLDSPoint *pPoint=m_pModel->FromPointHandle(hDatumPoint);
		if (pPoint)
			pPoint->AddToConsoleObjBuf(bAddRelaObj);
	}
}
bool CLDSNode::ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType/*=0*/)
{
	bool replaced=false;
	if (replacer.hOld==0)	//��0����滻ΪhNew�����Ϊ�����������hNewֵ wjh-2014.12.19
		handle+=replacer.hNew;
	else if (handle==replacer.hOld&&replacer.IsReplaceObjSelfHandle())
		handle=replacer.hNew;
	if (!replacer.SelectPerfectPartHandle(pos))
		return false;
	replaced|=replacer.VerifyAndReplace(&_hFatherPart);
	if (idClassType==0||idClassType==CLS_NODE)
	{
		for (int i=0;i<4;i++)
			replaced|=replacer.VerifyAndReplace(&arrRelationNode[i]);
		for (RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
			replaced|=replacer.VerifyAndReplace(&pRelaObj->hObj);
	}
	if (idClassType==0||idClassType==CLS_LINEANGLE||idClassType==CLS_LINETUBE
		||idClassType==CLS_LINEFLAT||idClassType==CLS_LINESLOT)
	{
		replaced|=replacer.VerifyAndReplace(&arrRelationPole[0]);
		replaced|=replacer.VerifyAndReplace(&arrRelationPole[1]);
	}
	replaced|=replacer.VerifyAndReplace(&hDatumPoint);
	replaced|=replacer.VerifyAndReplace(&hRelationPlane);
	if (replaced)
		SetModified();
	return replaced;
}
long CLDSNode::StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers)
{
	long nInitCount=pListHandlers->Count;
	pListHandlers->Append(HANDLE_ADDR(&handle));
	pListHandlers->Append(HANDLE_ADDR(&_hFatherPart));
	for (int i = 0; i < 4; i++)
	{
		if (arrRelationNode[i] > 0)
			pListHandlers->Append(HANDLE_ADDR(&arrRelationNode[i]));
	}
	if (arrRelationPole[0] > 0)
		pListHandlers->Append(HANDLE_ADDR(&this->arrRelationPole[0]));
	if (arrRelationPole[1] > 0)
		pListHandlers->Append(HANDLE_ADDR(&this->arrRelationPole[1]));
	pListHandlers->Append(HANDLE_ADDR(&hDatumPoint));
	pListHandlers->Append(HANDLE_ADDR(&hRelationPlane));
	for (RELATIVE_OBJECT *pRelaObj = relativeObjs.GetFirst(); pRelaObj; pRelaObj = relativeObjs.GetNext())
		pListHandlers->Append(HANDLE_ADDR(&pRelaObj->hObj));
	return pListHandlers->Count- nInitCount;
}
bool CLDSNode::MigrateFatherRod(long hNewRod)
{
	if (BelongModel()==NULL)
	{
		_hFatherPart=hNewRod;
		return true;
	}

	CSuperSmartPtr<CLDSLinePart> pNewParentRod=BelongModel()->FromRodHandle(hNewRod);
	CSuperSmartPtr<CLDSLinePart> pOldParentRod=BelongModel()->FromRodHandle(_hFatherPart);
	if (pNewParentRod.IsNULL())
	{
		_hFatherPart=hNewRod;
		return false;
	}
	else if (pOldParentRod.IsNULL())
		_hFatherPart=pNewParentRod->handle;
	else if (pOldParentRod->IsAngle()&&pNewParentRod->IsAngle()&&
		(pOldParentRod->pStart==this||pOldParentRod->pEnd==this)&&
		(pNewParentRod->pStart==this||pNewParentRod->pEnd==this))
	{
		if (pOldParentRod->pStart==this&&pNewParentRod->pStart==this&&
			(pOldParentRod.LineAnglePointer()->desStartPos.spatialOperationStyle==16||pOldParentRod.LineAnglePointer()->desStartPos.spatialOperationStyle==17))
		{
			if (pNewParentRod.LineAnglePointer()->desStartPos.IsInDatumJgBer())
			{
				CMultiOffsetPos schema=pNewParentRod.LineAnglePointer()->desStartPos;
				pNewParentRod.LineAnglePointer()->desStartPos=pOldParentRod.LineAnglePointer()->desStartPos;
				pOldParentRod.LineAnglePointer()->desStartPos=schema;
			}
		}
		else if (pOldParentRod->pStart==this&&pNewParentRod->pEnd==this&&
			(pOldParentRod.LineAnglePointer()->desStartPos.spatialOperationStyle==16||pOldParentRod.LineAnglePointer()->desStartPos.spatialOperationStyle==17))
		{
			if (pNewParentRod.LineAnglePointer()->desEndPos.IsInDatumJgBer())
			{
				CMultiOffsetPos schema=pNewParentRod.LineAnglePointer()->desEndPos;
				pNewParentRod.LineAnglePointer()->desEndPos=pOldParentRod.LineAnglePointer()->desStartPos;
				pOldParentRod.LineAnglePointer()->desStartPos=schema;
			}
		}
		else if (pOldParentRod->pEnd==this&&pNewParentRod->pStart==this&&
			(pOldParentRod.LineAnglePointer()->desEndPos.spatialOperationStyle==16||pOldParentRod.LineAnglePointer()->desEndPos.spatialOperationStyle==17))
		{
			if (pNewParentRod.LineAnglePointer()->desStartPos.IsInDatumJgBer())
			{
				CMultiOffsetPos schema=pNewParentRod.LineAnglePointer()->desStartPos;
				pNewParentRod.LineAnglePointer()->desStartPos=pOldParentRod.LineAnglePointer()->desEndPos;
				pOldParentRod.LineAnglePointer()->desEndPos=schema;
			}
		}
		else if (pOldParentRod->pEnd==this&&pNewParentRod->pEnd==this&&
			(pOldParentRod.LineAnglePointer()->desEndPos.spatialOperationStyle==16||pOldParentRod.LineAnglePointer()->desEndPos.spatialOperationStyle==17))
		{
			if (pNewParentRod.LineAnglePointer()->desEndPos.IsInDatumJgBer())
			{
				CMultiOffsetPos schema=pNewParentRod.LineAnglePointer()->desEndPos;
				pNewParentRod.LineAnglePointer()->desEndPos=pOldParentRod.LineAnglePointer()->desEndPos;
				pOldParentRod.LineAnglePointer()->desEndPos=schema;
			}
		}
	}
	_hFatherPart=hNewRod;
	return true;
}
void CLDSNode::UpdateMirNodeFatherPart(MIRMSG mirmsg,CLDSPart *pPart,int update_type/*=0*/)
{
	if (pPart==NULL||(pPart&&!pPart->IsLinePart()&&!pPart->IsArcPart()))
		return;
	int flagArr[3]={ 1,2,4 };
	CLDSNode* pMirNode=NULL;
	for (int i=0;i<3;i++)
	{
		if ((mirmsg.axis_flag&flagArr[i])==0)
			continue;
		MIRMSG msg=mirmsg.GetSubMirItem(i+1);
		pMirNode=GetMirNode(msg);
		if (pMirNode==this)
			continue;	//�ԶԳ�
		CLDSLinePart *pMirLinePart=NULL;
		if (pMirNode)
		{
			if (pPart->IsLinePart())
			{	//ֱ�߸˼�
				CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
				pMirLinePart=(CLDSLinePart*)pLinePart->GetMirPart(msg);
				if (pMirLinePart&&pMirLinePart->IsLinePart())
				{
					if (update_type==0)		//���½ڵ㸸�˼�
						pMirNode->hFatherPart=pMirLinePart->handle;
					else if (update_type==1)	//���½ڵ������˼�1(��Ҫ���ڽ���ڵ�)
						pMirNode->arrRelationPole[0]=pMirLinePart->handle;
					else if (update_type==2)	//���½ڵ������˼�2(��Ҫ���ڽ���ڵ�)
						pMirNode->arrRelationPole[1]=pMirLinePart->handle;
				}
			}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
			else if (pPart->IsArcPart())
			{	//���ι���
				CLDSArcPart *pArcPart=(CLDSArcPart*)pPart;
				CLDSArcPart *pMirArcPart=(CLDSArcPart*)pArcPart->GetMirPart(msg);
				if (pMirArcPart&&pMirArcPart->IsArcPart())
				{
					if (update_type==0)		//���½ڵ㸸�˼�
						pMirNode->hFatherPart=pMirArcPart->handle;
					else if (update_type==1)	//���½ڵ������˼�1(��Ҫ���ڽ���ڵ�)
						pMirNode->arrRelationPole[0]=pMirArcPart->handle;
					else if (update_type==2)	//���½ڵ������˼�2(��Ҫ���ڽ���ڵ�)
						pMirNode->arrRelationPole[1]=pMirArcPart->handle;
				}
			}
#endif
		}
	}
}

void CLDSNode::UpdateMirNodeAttachNode(MIRMSG mirmsg,CLDSNode *pAttachNode,int update_type/*=0*/)
{
	//update_type 0-���������ڵ�1 1-���������ڵ�2 2-���������ڵ�3 3-���������ڵ�4
	int flagArr[3]={ 1,2,4 };
	CLDSNode* pMirNode=NULL;
	for (int i=0;i<3;i++)
	{
		if ((mirmsg.axis_flag&flagArr[i])==0)
			continue;
		MIRMSG msg=mirmsg.GetSubMirItem(i+1);
		pMirNode=GetMirNode(msg);
		CLDSNode* pMirAttachNode=NULL;
		if (pMirNode&&pMirNode!=this)
		{
			pMirAttachNode=pAttachNode->GetMirNode(msg);
			if (pMirAttachNode)
				pMirNode->arrRelationNode[update_type]=pMirAttachNode->handle;
		}
	}
}

void CLDSNode::ClearErrorRelativeObjs()
{
	for (RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		CLDSNode *pMirNode=BelongModel()->FromNodeHandle(pRelaObj->hObj);
		MIRMSG msg=pRelaObj->mirInfo;
		if (pMirNode==NULL)	//ɾ�������õĹ�������
			relativeObjs.DeleteCursor();
		else
		{
			f3dPoint mir_pos=GetMirPos(pos,pRelaObj->mirInfo);
			if (!mir_pos.IsEqual(pMirNode->pos,0.5))
				relativeObjs.DeleteCursor();
		}
	}
	relativeObjs.Clean();
}

MEMORY_BLOCK CLDSNode::MPCL;	//Mirror Property Control List
PROPLIST_TYPE CLDSNode::propMirSync;
bool CLDSNode::IsSyncMirPropWith(CLDSDbObject* pObj,const char* propKeyStr)
{
	if (pObj==NULL||propKeyStr==NULL||pObj->GetClassTypeId()!=CLS_NODE)
		return false;
	CLDSNode* pNode=(CLDSNode*)pObj;
	MEMORY_BLOCK cl=mpcl;
	if (cl.IsZero())
		cl=MPCL;
	MEMORY_BLOCK mir_cl=pNode->mpcl;
	if (mir_cl.IsZero())
		mir_cl=MPCL;
	PROPLIST_ITEM* pItem=propMirSync.GetPropItem(propKeyStr);
	if (pItem!=NULL)
		return cl.GetBitState((WORD)pItem->id-1)&&mir_cl.GetBitState((WORD)pItem->id-1);
	else
	{
		Log2File()->Log("%sͬ���Գ�������ȱʧ",propKeyStr);
		return false;
	}
}
bool CLDSNode::SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr/*=NULL*/,DWORD dwPropFilterFlag/*=0xffffffff*/)//propKeyStr��ʾͬ���Գ�ȫ��λ�öԳ��޹ص�����
{
	if (pDestObj==NULL||pDestObj==this)
		return false;
	CLDSNode* pNode=(CLDSNode*)pDestObj;
	if ((propKeyStr==NULL||strcmp(propKeyStr,"iSeg")==0)&&IsSyncMirPropWith(pDestObj,"iSeg"))
	{	//�ֶκ�
		pNode->iSeg=iSeg;
	}
	if ((propKeyStr==NULL||strcmp(propKeyStr,"cfgword")==0)&&IsSyncMirPropWith(pDestObj,"cfgword"))
	{	//��ĺ�
		pNode->cfgword=cfgword;
	}
	if ((propKeyStr==NULL||strcmp(propKeyStr,"xFatherAngleZhunJu")==0)&&IsSyncMirPropWith(pDestObj,"xFatherAngleZhunJu"))
	{	//׼���趨
		pNode->xFatherAngleZhunJu=xFatherAngleZhunJu;
	}
#ifdef __PART_DESIGN_INC_
	if ((propKeyStr==NULL||strcmp(propKeyStr,"point_i")==0)&&IsSyncMirPropWith(pDestObj,"point_i"))
	{	//���
		//pAngle->desStartPos.sp=desStartPos.jgber_cal_style;
	}
	if ((propKeyStr==NULL||strcmp(propKeyStr,"force_type")==0)&&IsSyncMirPropWith(pDestObj,"force_type"))
	{	//�ڵ㴫���ж�
		pNode->force_type=force_type;
	}
	if ((propKeyStr==NULL||strcmp(propKeyStr,"hangingInfo")==0)&&IsSyncMirPropWith(pDestObj,"hangingInfo"))
	{	//������Ϣ
		pNode->m_ciWireFractions=m_ciWireFractions;
		pNode->m_cHangWireType=m_cHangWireType;
	}
	if ((propKeyStr==NULL||strcmp(propKeyStr,"restrict")==0)&&IsSyncMirPropWith(pDestObj,"restrict"))
	{	//֫������Ϣ
		pNode->restrict=restrict;
		//pAngle->connectStart=connectStart;
		//pAngle->connectEnd=connectEnd;
	}
#endif
	return false;
}
bool CLDSNode::SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr/*=NULL*/,DWORD dwPropFilterFlag/*=0xffffffff*/)
{
	if (pDestObj==NULL||pDestObj==this)
		return false;
	CLDSNode* pNode=(CLDSNode*)pDestObj;
	if ((propKeyStr==NULL||strcmp(propKeyStr,"layer")==0)&&IsSyncMirPropWith(pDestObj,"layer"))
	{	//ͼ����ͬ���Գ�
		MirObjLayer(layer(),pNode->layer(),mirmsg);
	}
	if ((propKeyStr==NULL||strcmp(propKeyStr,"pos")==0)&&IsSyncMirPropWith(pDestObj,"pos"))
	{	//����
		CNodeMoveDetector detector(pNode);
		pNode->SetPosition(mirmsg.MirPoint(pos));
		if (mirmsg.axis_flag==0x01||mirmsg.axis_flag==0x02||mirmsg.axis_flag==0x04)
			pNode->SetArcLift(m_hArcLift);	//������ΰ��������Բ��Ԥ��������ͬ������
		if (pNode->m_cPosCalType!=11)	//λ����Ʋ����ݲ�֧��ָ����׼��ĶԳ�
		{
			pNode->m_cPosCalType=m_cPosCalType;
			pNode->m_cPosCalViceType=m_cPosCalViceType;
			if (mirmsg.axis_flag&8&&ftoi(mirmsg.rotate_angle/90)%2!=0)
			{	//��ת90�ȶԳ�ʱX��Y��������
				if (m_cPosCalType==1)
					pNode->m_cPosCalType=2;	//�˼���Y����ֵ�����
				else if (m_cPosCalType==2)
					pNode->m_cPosCalType=1;	//�˼���X����ֵ�����
				else if (m_cPosCalType==9)
					pNode->m_cPosCalType=10;//��׼����XZ����ֵ�����
				else if (m_cPosCalType==10)
					pNode->m_cPosCalType=9;	//��׼����YZ����ֵ�����
				//����ڵ�
				if (m_cPosCalType==4&&m_cPosCalViceType==1)
					m_cPosCalViceType=2;
				else if (m_cPosCalType==4&&m_cPosCalViceType==2)
					m_cPosCalViceType=1;
				//����������Ľڵ��ƫ�ƽڵ�
				else if ((m_cPosCalType==6||m_cPosCalType==7)&&m_cPosCalViceType=='X')
					m_cPosCalViceType='Y';
				else if ((m_cPosCalType==6||m_cPosCalType==7)&&m_cPosCalViceType=='Y')
					m_cPosCalViceType='X';
			}
			//�Գ������ڵ���
			for (int i=0;i<4;i++)
			{
				CLDSNode* pDependNode=NULL,*pMirDependNode=NULL;
				if (arrRelationNode[i]>0x20)
					pDependNode=BelongModel()->FromNodeHandle(arrRelationNode[i]);
				if (pDependNode)
					pMirDependNode=pDependNode->GetMirNode(mirmsg);
				if (pMirDependNode&&(pNode->arrRelationNode[i]<0x20||pNode->Position(false)!=pMirDependNode->Position(false)))
					pNode->arrRelationNode[i]=pMirDependNode->handle;
			}
			if (m_cPosCalType==4)
			{
				CLDSLinePart* pRod = BelongModel()->FromRodHandle(arrRelationPole[0]);
				if (pRod)//�������Ǹ־��Ϊ0,��ʾ�������Ǹ�,�˳�
				{
					CLDSLinePart* pMirRod = (CLDSLinePart*)pRod->GetMirPart(mirmsg,pos);
					if (pMirRod)
						pNode->arrRelationPole[0] = pMirRod->handle;
				}
				pRod = BelongModel()->FromRodHandle(arrRelationPole[1]);
				if (pRod)//�������Ǹ־��Ϊ0,��ʾ�������Ǹ�,�˳�
				{
					CLDSLinePart* pMirRod = (CLDSLinePart*)pRod->GetMirPart(mirmsg,pos);
					if (pMirRod)
						pNode->arrRelationPole[1] = pMirRod->handle;
				}
			}
			else if (m_cPosCalType==5)
				pNode->attach_scale=attach_scale;
			else if (m_cPosCalType==7)	//����ƫ�Ƶ�ƫ���������ĶԳ�
				pNode->attach_offset=attach_offset;
			else if (m_cPosCalType>=8&&m_cPosCalType<=10||m_cPosCalType==12)//&&dwPropCopyFlag&PROP_POS_COPLANAR_COPY)
			{	//�Գƻ�׼�涨λ�Ķ�λ������Ϣ
				if (m_cPosCalType!=12)
					pNode->hRelationPlane=hRelationPlane;
				if (mirmsg.axis_flag==0x01)	//X��Գ�
				{
					if (m_cPosCalViceType==0)
						pNode->m_cPosCalViceType='X';
					else if (m_cPosCalViceType=='X')
						pNode->m_cPosCalViceType=0;
					else if (m_cPosCalViceType=='Y')
						pNode->m_cPosCalViceType='Z';
					else if (m_cPosCalViceType=='Z')
						pNode->m_cPosCalViceType='Y';
				}
				else if (mirmsg.axis_flag==0x02)	//Y��Գ�
				{
					if (m_cPosCalViceType==0)
						pNode->m_cPosCalViceType='Y';
					else if (m_cPosCalViceType=='Y')
						pNode->m_cPosCalViceType=0;
					else if (m_cPosCalViceType=='X')
						pNode->m_cPosCalViceType='Z';
					else if (m_cPosCalViceType=='Z')
						pNode->m_cPosCalViceType='X';
				}
				else if (mirmsg.axis_flag==0x04)	//Z��Գ�
				{
					if (m_cPosCalViceType==0)
						pNode->m_cPosCalViceType='Z';
					else if (m_cPosCalViceType=='Z')
						pNode->m_cPosCalViceType=0;
					else if (m_cPosCalViceType=='X')
						pNode->m_cPosCalViceType='Y';
					else if (m_cPosCalViceType=='Y')
						pNode->m_cPosCalViceType='X';
				}
			}
			/*
			����ƫ�Ƶ��ƫ�Ʒ������Ի�׼�˼�ʼ���շ���Ϊ��׼�������Ե�һ����׼�㵽
			�ڶ�����׼��Ϊ��׼��������Ҫ�ж�ƫ�Ʒ����Ƿ��򣬹����´���ֹͣʹ�� wjh-2009-11-11
			if(m_cPosCalType==7||m_cPosCalType==8)	//����ƫ�Ƶ�ƫ���������ĶԳ�
			{
				CLDSPart *pPart=BelongModel()->FromPartHandle(hFatherPart);
				CLDSLinePart *pFatherPart=NULL,*pMirFatherPart=NULL;
				if(pPart&&pPart->IsLinePart())
				{
					pFatherPart=(CLDSLinePart*)pPart;
					pMirFatherPart=(CLDSLinePart*)pFatherPart->GetMirPart(msg);
				}
				pMirNode->attach_offset=attach_offset;
				if(pFatherPart&&pMirFatherPart)
				{
					f3dPoint offset_vec=pFatherPart->End()-pFatherPart->Start();
					f3dPoint mir_offset_vec=pMirFatherPart->End()-pMirFatherPart->Start();
					offset_vec=GetMirVector(offset_vec,msg);
					if(offset_vec*mir_offset_vec<0)
						pMirNode->attach_offset*=-1.0;
				}
			}*/
		}
	}
	if ((propKeyStr==NULL||strcmp(propKeyStr,"iSeg")==0)&&IsSyncMirPropWith(pDestObj,"iSeg"))
	{	//�ֶκ�
		pNode->iSeg=iSeg;
	}
	if ((propKeyStr==NULL||strcmp(propKeyStr,"cfgword")==0)&&IsSyncMirPropWith(pDestObj,"cfgword"))
	{	//��ĺ�
		pNode->cfgword=cfgword;
	}
	CLDSLinePart* pFatherRod=NULL,*pMirFatherRod=NULL;
	if ((propKeyStr==NULL||strcmp(propKeyStr,"hFatherPart")==0)&&IsSyncMirPropWith(pDestObj,"hFatherPart"))
	{	//���˼�
		pFatherRod=BelongModel()->FromRodHandle(hFatherPart);
		pMirFatherRod=BelongModel()->FromRodHandle(pNode->hFatherPart);
		if (pFatherRod)
			pMirFatherRod=pFatherRod->GetMirRod(mirmsg);
		if (pMirFatherRod)
			pNode->hFatherPart=pMirFatherRod->handle;
	}
	if ((propKeyStr==NULL||strcmp(propKeyStr,"xFatherAngleZhunJu")==0)&&IsSyncMirPropWith(pDestObj,"xFatherAngleZhunJu"))
	{	//׼���趨
		CLDSLineAngle* pFatherJg	=(pFatherRod!=NULL	 &&pFatherRod->IsAngle())?(CLDSLineAngle*)pFatherRod:NULL;
		CLDSLineAngle* pMirFatherJg	=(pMirFatherRod!=NULL&&pMirFatherRod->IsAngle())?(CLDSLineAngle*)pMirFatherRod:NULL;
		if (pFatherJg==NULL)
			pFatherJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if (pMirFatherJg!=NULL)
			pMirFatherJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pNode->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		bool bReverseWingXnY=false;
		if (pFatherJg!=NULL&&pMirFatherJg!=NULL)
		{
			pFatherJg->GetMirMatchInfo(pMirFatherJg,mirmsg,NULL,&bReverseWingXnY);
			if (!bReverseWingXnY)
			{	//X֫��Գƽڵ㸸�Ǹ�X֫ͬ��
				pNode->xFatherAngleZhunJu.offset_X_dist_style=xFatherAngleZhunJu.offset_X_dist_style;
				pNode->xFatherAngleZhunJu.offset_X_dist=xFatherAngleZhunJu.offset_X_dist;
				pNode->xFatherAngleZhunJu.offset_Y_dist_style=xFatherAngleZhunJu.offset_Y_dist_style;
				pNode->xFatherAngleZhunJu.offset_Y_dist=xFatherAngleZhunJu.offset_Y_dist;
			}
			else //if(fabs(wing_norm_y*mir_wing_norm_x)>fabs(wing_norm_x*mir_wing_norm_x))
			{	//X֫��Գƽڵ㸸�Ǹ�Y֫ͬ��
				pNode->xFatherAngleZhunJu.offset_X_dist_style=xFatherAngleZhunJu.offset_Y_dist_style;
				pNode->xFatherAngleZhunJu.offset_X_dist=xFatherAngleZhunJu.offset_Y_dist;
				pNode->xFatherAngleZhunJu.offset_Y_dist_style=xFatherAngleZhunJu.offset_X_dist_style;
				pNode->xFatherAngleZhunJu.offset_Y_dist=xFatherAngleZhunJu.offset_X_dist;
			}
		}
	}
#ifdef __PART_DESIGN_INC_
	if ((propKeyStr==NULL||strcmp(propKeyStr,"point_i")==0)&&IsSyncMirPropWith(pDestObj,"point_i"))
	{	//���
		//pAngle->desStartPos.sp=desStartPos.jgber_cal_style;
	}
	if ((propKeyStr==NULL||strcmp(propKeyStr,"force_type")==0)&&IsSyncMirPropWith(pDestObj,"force_type"))
	{	//�ڵ㴫���ж�
		pNode->force_type=force_type;
	}
	if ((propKeyStr==NULL||strcmp(propKeyStr,"hangingInfo")==0)&&IsSyncMirPropWith(pDestObj,"hangingInfo"))
	{	//������Ϣ
		pNode->m_cHangWireType=m_cHangWireType;
	}
	if ((propKeyStr==NULL||strcmp(propKeyStr,"restrict")==0)&&IsSyncMirPropWith(pDestObj,"restrict"))
	{	//֫������Ϣ
		pNode->restrict=restrict;
		//pAngle->connectStart=connectStart;
		//pAngle->connectEnd=connectEnd;
	}
#endif
	return true;
}
bool CLDSNode::SyncMirProp(const char* propKeyStr/*=NULL*/,BYTE ciMirAll0X1Y2Z3/*=0*/,DWORD dwPropFilterFlag/*=0xffffffff*/)//propKeyStr��ʾͬ���Գ�ȫ������
{
	//CRelaObjGroup* pRelaGroup=BelongModel()->FindRelaObjGroup(handle);
	//if(pRelaGroup==NULL)
	//	return false;
	//CRelaObjGroup::RELA_OBJECT* pTriggerRela=pRelaGroup->relaObjs.GetValue(handle);
	//if(pTriggerRela==NULL)
	//	return false;
	//for(CRelaObjGroup::RELA_OBJECT* pRela=pRelaGroup->relaObjs.GetFirst();pRela;pRela=pRelaGroup->relaObjs.GetNext())
	MIRMSG xSpecMirInfo(ciMirAll0X1Y2Z3);
	for (RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		if (ciMirAll0X1Y2Z3!=0&&!pRelaObj->mirInfo.IsSameMirMsg(xSpecMirInfo))
			continue;
		BOOL pushed=relativeObjs.push_stack();
		CLDSNode* pNode=BelongModel()->FromNodeHandle(pRelaObj->hObj);
		SyncMirPropTo(pNode,pRelaObj->mirInfo,propKeyStr,dwPropFilterFlag);
		relativeObjs.pop_stack(pushed);
	}
	return true;
}
void CLDSNode::TransToCS(GECS& cs)
{
	m_xArcLiftPos=cs.TransPToCS(m_xArcLiftPos);
	pos=cs.TransPToCS(pos);
	SetModified(TRUE,false);
}
void CLDSNode::Offset(const double* offsetcoords)
{
	GEPOINT offset(offsetcoords);
	m_xArcLiftPos+=offset;
	pos+=offset;
	SetModified(TRUE,false);
}
bool CLDSNode::OffsetPosParam(const double* offsetcoords)
{
	//��Ʋ�������
	GEPOINT offset(offsetcoords);
	//7.�ظ˼���ĳ�ڵ��ƫ�Ƶ�_cPosCalViceType=0��ʾ�ظ˼�����ƫ��,'X'��ʾ��X����,'Y'��ʾ��Y����,'Z'��ʾ��Z����
	if (_cPosCalType<=3)
		return true;
	else if (_cPosCalType!=7)
		return false;
	//7.�ظ˼���ĳ�ڵ��ƫ�Ƶ�
	char cOffsetByAxis=_cPosCalViceType;
	if (cOffsetByAxis<'X'||cOffsetByAxis>'Z')
	{
		CLDSNode* pOffsetByNode1= BelongModel()->FromNodeHandle(arrRelationNode[1]);
		CLDSNode* pOffsetByNode2= BelongModel()->FromNodeHandle(arrRelationNode[2]);
		if (pOffsetByNode1&&pOffsetByNode2)
		{
			GEPOINT offsetvec=pOffsetByNode2->Position(false)-pOffsetByNode1->Position(false);
			if ((fabs(offset.y)+fabs(offset.z))<EPS)
				cOffsetByAxis='X';
			else if (fabs(offset.x)+fabs(offset.z)<EPS)
				cOffsetByAxis='Y';
			else if (fabs(offset.x)+fabs(offset.y)<EPS)
				cOffsetByAxis='Z';
		}
	}
	if (_cPosCalViceType=='X'&&(fabs(offset.y)+fabs(offset.z))<EPS)
		attach_offset+=offset.x;
	else if (_cPosCalViceType=='Y'&&(fabs(offset.x)+fabs(offset.z))<EPS)
		attach_offset+=offset.y;
	else if (_cPosCalViceType=='Z'&&(fabs(offset.x)+fabs(offset.y))<EPS)
		attach_offset+=offset.z;
	else
		return false;
	return true;
}
void CLDSNode::ResetRelativeObjs()
{
	for (RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		CLDSNode *pMirNode=BelongModel()->FromNodeHandle(pRelaObj->hObj);
		MIRMSG msg=pRelaObj->mirInfo;
		if (pMirNode==NULL)
		{	//ɾ�������õĹ�������
			relativeObjs.DeleteCursor();
			continue;
		}
	}
	relativeObjs.Clean();
	SyncMirProp();
}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
CLDSLinePart* SearchReferenceRod(IModel* pModel,CLDSNode* pStart,CLDSNode* pEnd,bool bPreferStart=true)
{
	if (pStart==NULL||pEnd==NULL)
		return NULL;
	f3dPoint line_vec,line_vec2;
	CLDSNode* pDatumNode=pStart;
	if (bPreferStart)
		line_vec=pEnd->Position(false)-pStart->Position(false);
	else
	{
		pDatumNode=pEnd;
		line_vec=pStart->Position(false)-pEnd->Position(false);
	}
	normalize(line_vec);
	//���������б�ǰӦ��ѹջ wht 12-03-05
	CLDSLinePart *pDatumRod;
	BOOL pushed=pModel->PushPartStack();
	for (pDatumRod=pModel->EnumRodFirst();pDatumRod;pDatumRod=pModel->EnumRodNext())
	{
		if (pDatumRod->pStart==NULL||pDatumRod->pEnd==NULL)
			continue;
		if (pDatumRod->pStart==pDatumNode)
			line_vec2=pDatumRod->pEnd->Position(false)-pDatumRod->pStart->Position(false);
		else if (pDatumRod->pEnd==pDatumNode)
			line_vec2=pDatumRod->pStart->Position(false)-pDatumRod->pEnd->Position(false);
		if (pDatumRod->pStart==pDatumNode||pDatumRod->pEnd==pDatumNode)
		{
			normalize(line_vec2);
			if (line_vec*line_vec2>EPS_COS)
				break;
		}
	}
	pModel->PopPartStack(pushed);
	return pDatumRod;
}

BOOL CLDSNode::GetIntersZhun(COORD3D *pxIntersZhun,double *pDist/*=NULL*/)
{
	if (m_cPosCalType!=4)
		return FALSE;	//���ǽ���ڵ�
	f3dPoint vec[2];
	for (int i=0;i<2;i++)
	{
		CLDSLinePart *pRod=(CLDSLinePart*)BelongModel()->FromPartHandle(arrRelationPole[i],CLS_LINEPART);
		if (pRod!=NULL&&pRod->pStart!=NULL&&pRod->pEnd!=NULL)
			vec[i]=pRod->pEnd->pos-pRod->pStart->pos;	//�л�׼�˼��Ի�׼�˼�Ϊ׼���������潻��ʱ����ƫ��ϴ���ܲ�������
		else
		{
			CLDSNode *pStartNode=BelongModel()->FromNodeHandle(arrRelationNode[i*2]);
			CLDSNode *pEndNode=BelongModel()->FromNodeHandle(arrRelationNode[i*2+1]);
			if (pStartNode!=NULL&&pEndNode!=NULL)
				vec[i]=pEndNode->pos-pStartNode->pos;
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (pStartNode==NULL)
					logerr.Log("0X%X cross node's %d cross dependent node is missing!",handle,i*2+1);
				if (pEndNode==NULL)
					logerr.Log("0X%X cross node's %d cross dependent node is missing!",handle,i*2+2);
#else 
				if (pStartNode==NULL)
					logerr.Log("0X%X����ڵ��%d�����������ڵ�ȱʧ!",handle,i*2+1);
				if (pEndNode==NULL)
					logerr.Log("0X%X����ڵ��%d�����������ڵ�ȱʧ!",handle,i*2+2);
#endif
				return FALSE;	//���ǽ���ڵ�
			}
		}
	}
	//֧�ּ��㽻��˼�Ϊ�Ǹ֡��۸ּ�����������ͶӰλ�ã�ԭ����֧��������˼���Ϊ�Ǹֵ���� wht 12-09-26
	f3dPoint norm = vec[0]^vec[1];
	normalize(norm);
	f3dLine zhun_line[2];
	for (int i=0;i<2;i++)
	{
		CLDSLinePart *pRod=(CLDSLinePart*)BelongModel()->FromPartHandle(arrRelationPole[i],CLS_LINEPART);
#ifdef AFX_TARG_ENU_ENGLISH
		if (pRod==NULL)
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X cross node's %d cross dependent rod is missing!",handle,i+1);
		else if (pRod->pStart==NULL||pRod->pEnd==NULL)
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X cross node's %d cross dependent rod's (0X%X) node is missing!",handle,i+1,arrRelationPole[i]);
#else 
		if (pRod==NULL)
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X����ڵ��%d�����������˼�ȱʧ!",handle,i+1);
		else if (pRod->pStart==NULL||pRod->pEnd==NULL)
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X����ڵ��%d�����������˼�(0X%X)�˽ڵ�ȱʧ!",handle,i+1,arrRelationPole[i]);
#endif
		if (pRod==NULL||pRod->pStart==NULL||pRod->pEnd==NULL)
			return FALSE;
		if (!pRod->IsAngle()&&pRod->GetClassTypeId()!=CLS_LINESLOT&&pRod->GetClassTypeId()!=CLS_LINEFLAT&&pRod->GetClassTypeId()!=CLS_LINETUBE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("0X%X cross node's %d cross dependent rod isn't legal type of rod(line angle��U-steel��flat iron or tube)!",handle,i+1);
#else 
			logerr.Log("0X%X����ڵ��%d�����������˼��ǺϷ��˼�����(ֱ���ͽǸ֡��۸֡�������ֹ�)!",handle,i+1);
#endif
			return FALSE;
		}
		if (pRod->pStart==this)
		{	//��ǰ����ڵ�ͬΪ�˼�����ʼ�ڵ�
			CLDSNode* pOthNode=NULL;
			if (pRod->pEnd->handle==arrRelationNode[i*2])
				pOthNode=m_pModel->FromNodeHandle(arrRelationNode[i*2+1]);
			else
				pOthNode=m_pModel->FromNodeHandle(arrRelationNode[i*2]);
			CLDSLinePart* pRod2=SearchReferenceRod(m_pModel,pOthNode,this);
			if (pRod2&&pRod2->pStart==pOthNode)
				zhun_line[i].startPt=pRod2->Start();
			else if (pRod2&&pRod2->pEnd==pOthNode)
				zhun_line[i].startPt=pRod2->End();
			else
				zhun_line[i].startPt=pRod->Start();
			zhun_line[i].endPt=pRod->End();
			int x_wing0_y_wing1;
			f3dPoint wing_vec_offset;
			CLDSLineAngle *pCrossAngle;
			if (pRod2&&pRod2->IsAngle())
			{
				pCrossAngle=(CLDSLineAngle*)pRod2;
				CLDSLineAngle::IsInsideAngle(pCrossAngle,norm,&x_wing0_y_wing1);
				if (x_wing0_y_wing1==0&&pRod2->pStart==pOthNode)
					wing_vec_offset=pCrossAngle->GetStartOffsetByWingY()*pCrossAngle->GetWingVecX();
				else if (x_wing0_y_wing1==0&&pRod2->pEnd==pOthNode)
					wing_vec_offset=pCrossAngle->GetEndOffsetByWingY()*pCrossAngle->GetWingVecX();
				else if (x_wing0_y_wing1==1&&pRod2->pStart==pOthNode)
					wing_vec_offset=pCrossAngle->GetStartOffsetByWingX()*pCrossAngle->GetWingVecY();
				else //if(x_wing0_y_wing1==1&&pRod2->pEnd==pOthNode)
					wing_vec_offset=pCrossAngle->GetEndOffsetByWingX()*pCrossAngle->GetWingVecY();
				zhun_line[i].startPt += wing_vec_offset;
			}
			if (pRod->IsAngle())
			{
				CLDSLineAngle *pCrossAngle=(CLDSLineAngle*)pRod;
				CLDSLineAngle::IsInsideAngle(pCrossAngle,norm,&x_wing0_y_wing1);
				if (x_wing0_y_wing1==0)
					wing_vec_offset=pCrossAngle->GetEndOffsetByWingY()*pCrossAngle->GetWingVecX();
				else //if(x_wing0_y_wing1==1)
					wing_vec_offset=pCrossAngle->GetEndOffsetByWingX()*pCrossAngle->GetWingVecY();
				if (pRod2==NULL)
					zhun_line[i].startPt += wing_vec_offset;
				zhun_line[i].endPt += wing_vec_offset;
			}
		}
		else if (pRod->pEnd==this)
		{	//��ǰ����ڵ�ͬΪ�˼�����ֹ�ڵ�
			CLDSNode* pOthNode=NULL;
			if (pRod->pStart->handle==arrRelationNode[i*2])
				pOthNode=m_pModel->FromNodeHandle(arrRelationNode[i*2+1]);
			else
				pOthNode=m_pModel->FromNodeHandle(arrRelationNode[i*2]);
			CLDSLinePart* pRod2=SearchReferenceRod(m_pModel,pOthNode,this);
			zhun_line[i].startPt=pRod->Start();
			if (pRod2&&pRod2->pStart==pOthNode)
				zhun_line[i].endPt=pRod2->Start();
			else if (pRod2&&pRod2->pEnd==pOthNode)
				zhun_line[i].endPt=pRod2->End();
			else
				zhun_line[i].endPt=pRod->End();
			int x_wing0_y_wing1;
			f3dPoint wing_vec_offset;
			CLDSLineAngle *pCrossAngle;
			if (pRod2&&pRod2->IsAngle())
			{
				pCrossAngle=(CLDSLineAngle*)pRod2;
				CLDSLineAngle::IsInsideAngle(pCrossAngle,norm,&x_wing0_y_wing1);
				if (x_wing0_y_wing1==0&&pRod2->pStart==pOthNode)
					wing_vec_offset=pCrossAngle->GetStartOffsetByWingY()*pCrossAngle->GetWingVecX();
				else if (x_wing0_y_wing1==0&&pRod2->pEnd==pOthNode)
					wing_vec_offset=pCrossAngle->GetEndOffsetByWingY()*pCrossAngle->GetWingVecX();
				else if (x_wing0_y_wing1==1&&pRod2->pStart==pOthNode)
					wing_vec_offset=pCrossAngle->GetStartOffsetByWingX()*pCrossAngle->GetWingVecY();
				else //if(x_wing0_y_wing1==1&&pRod2->pEnd==pOthNode)
					wing_vec_offset=pCrossAngle->GetEndOffsetByWingX()*pCrossAngle->GetWingVecY();
				zhun_line[i].startPt += wing_vec_offset;
			}
			if (pRod->IsAngle())
			{
				CLDSLineAngle *pCrossAngle=(CLDSLineAngle*)pRod;
				CLDSLineAngle::IsInsideAngle(pCrossAngle,norm,&x_wing0_y_wing1);
				if (x_wing0_y_wing1==0)
					wing_vec_offset=pCrossAngle->GetStartOffsetByWingY()*pCrossAngle->GetWingVecX();
				else //if(x_wing0_y_wing1==1)
					wing_vec_offset=pCrossAngle->GetStartOffsetByWingX()*pCrossAngle->GetWingVecY();
				if (pRod2==NULL)
					zhun_line[i].startPt += wing_vec_offset;
				zhun_line[i].endPt += wing_vec_offset;
			}
		}
		else if (pRod->IsAngle())
		{
			JGZJ jgzj;
			f3dPoint wing_vec;
			int x_wing0_y_wing1;
			CLDSLineAngle *pCrossAngle=(CLDSLineAngle*)pRod;
			getjgzj(jgzj,pCrossAngle->GetWidth());
			CLDSLineAngle::IsInsideAngle(pCrossAngle,norm,&x_wing0_y_wing1);
			if (pCrossAngle->m_bEnableTeG)
			{
				if (x_wing0_y_wing1==0)
					jgzj = pCrossAngle->xWingXZhunJu;
				else
					jgzj = pCrossAngle->xWingYZhunJu;
			}
			double datum_dist;
			if (fabs(norm*pCrossAngle->get_norm_x_wing())>fabs(norm*pCrossAngle->get_norm_y_wing()))
			{
				wing_vec = pCrossAngle->GetWingVecX();
				if (pCrossAngle->m_bEnableTeG)
					datum_dist = pCrossAngle->xWingXZhunJu.g;
				else
					datum_dist = jgzj.g;
			}
			else
			{
				wing_vec = pCrossAngle->GetWingVecY();
				if (pCrossAngle->m_bEnableTeG)
					datum_dist = pCrossAngle->xWingYZhunJu.g;
				else
					datum_dist = jgzj.g;
			}
			zhun_line[i].startPt = pCrossAngle->Start()+wing_vec*datum_dist;
			zhun_line[i].endPt = pCrossAngle->End()+wing_vec*datum_dist;
		}
		else
		{	//�۸ֻ����
			zhun_line[i].startPt = pRod->Start();
			zhun_line[i].endPt = pRod->End();
		}
	}
	if (pDist)	// �������˼�֮��ľ��룬�����ж��Ƿ���Ҫ��ӵ��
		*pDist = DistOf3dll(zhun_line[0],zhun_line[1]);
	//Int3dll(zhun_line[0],zhun_line[1],*inters_zhun);
	char cPrjAxis=0;//0:��ʾ������ΪͶӰ��
	if (m_cPosCalViceType==1)
		cPrjAxis='Y';	//1:X-Zƽ��;
	else if (m_cPosCalViceType==2)
		cPrjAxis='X';	//2:Y-Zƽ��;
	else if (m_cPosCalViceType==3)
		cPrjAxis='Z';	//3:X-Yƽ��
	f3dPoint inters_zhun;
	Int3dDifferPlaneLL(zhun_line[0].startPt,zhun_line[0].endPt,zhun_line[1].startPt,zhun_line[1].endPt,inters_zhun,cPrjAxis);
	if (pxIntersZhun)
		pxIntersZhun->Set(inters_zhun.x,inters_zhun.y,inters_zhun.z);
	return TRUE;
}
#endif
int CLDSNode::ValidCheck()
{
	if (!BelongModel()->FromPartHandle(hFatherPart,CLS_LINEPART))
		return -1;	//ȱ�ٽڵ㸸�˼�
	else if ((m_cPosCalType==1||m_cPosCalType==2||m_cPosCalType==3||m_cPosCalType==5)&&
		(BelongModel()->FromNodeHandle(arrRelationNode[0])==NULL||BelongModel()->FromNodeHandle(arrRelationNode[1])==NULL))
		return -2;	//ȱ�������ڵ�
	else if (m_cPosCalType==4)
	{	//����ڵ�
		CLDSPart *pPart=BelongModel()->FromPartHandle(arrRelationPole[0]);
		if (pPart==NULL||!pPart->IsLinePart())
			return -3;	//ȱ�������˼�
		pPart=BelongModel()->FromPartHandle(arrRelationPole[1]);
		if (pPart==NULL||!pPart->IsLinePart())
			return -3;	//ȱ�������˼�
	}
	else if ((m_cPosCalType==6||m_cPosCalType==7)&&(BelongModel()->FromNodeHandle(arrRelationNode[0])==NULL||
		BelongModel()->FromNodeHandle(arrRelationNode[1])==NULL||BelongModel()->FromNodeHandle(arrRelationNode[2])==NULL))
		return -2;	//ȱ�������ڵ�
	else if (m_cPosCalType>=8&&BelongModel()->FromPlaneHandle(hRelationPlane)==NULL)
		return -4;	//ȱ������ƽ��
	else if (m_cPosCalType==11&&BelongModel()->FromPointHandle(hDatumPoint)==NULL)
		return -5;	//ȱ��������׼��
	return 0;
}

#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
BOOL CLDSNode::IsFatherObject()
{
	RELATIVE_OBJECT *pMirObj=NULL;
	int quad=point_i%10;
	if (quad==0)
		return TRUE;
	for (pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		MIRMSG mirInfo=pMirObj->mirInfo;
		if (pMirObj->mirInfo.axis_flag==0x04&&fabs(pos.x)<EPS)	//Z��Գ�ͬʱ����X��Գ�
			mirInfo.axis_flag=0x01;
		else if (pMirObj->mirInfo.axis_flag==0x04&&fabs(pos.y)<EPS)	//Z��Գ�ͬʱ����Y��Գ�
			mirInfo.axis_flag=0x02;
		if (mirInfo.axis_flag==0x01&&quad>1)	//X��Գ�
			return FALSE;	//�и�С��ŵĹ�������
		else if (mirInfo.axis_flag==0x02&&(quad==1||quad==3))	//Y��Գ�
			return FALSE;	//�и�С��ŵĹ�������
		else if (mirInfo.axis_flag==0x04&&(quad==2||quad==3))	//Z��Գ�
			return FALSE;	//�и�С��ŵĹ�������
	}
	return TRUE;
}
int MirPointI(int pointI,MIRMSG msg)
{
	int quad=pointI%10;
	int basenum=pointI-quad;
	if (msg.axis_flag==0x01)		//X��Գ�
		return (quad+2)%4+basenum;	//0<->2 1<->3
	else if (msg.axis_flag==0x02)	//Y��Գ�
	{
		if (quad%2==0)	//ż����1
			return quad+1+basenum;	//0->1 2->3
		else
			return quad-1+basenum;	//1->0 3->2
	}
	else if (msg.axis_flag==0x04)	//Z��Գ�
		return 3-quad+basenum;		//0<->3 1<->2
	return -1;
}
BOOL CLDSNode::GetFatherMirObj(RELATIVE_OBJECT *pRelaObj)
{
	RELATIVE_OBJECT *pMirObj=NULL;
	RELATIVE_OBJECT relaFatherObj;
	relaFatherObj.hObj=handle;
	int quad=point_i%10;
	int quad_mir,quad_min=quad;

	for (pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		if (pMirObj->mirInfo.axis_flag&0x08)	//��ת�Գ�����
			continue;
		if (pMirObj->mirInfo.axis_flag==0x04&&fabs(pos.x)<EPS)	//Z��Գ�ͬʱ����X��Գ�
			quad_mir=MirPointI(quad,MIRMSG(1));
		else if (pMirObj->mirInfo.axis_flag==0x04&&fabs(pos.y)<EPS)	//Z��Գ�ͬʱ����Y��Գ�
			quad_mir=MirPointI(quad,MIRMSG(2));
		else
			quad_mir=MirPointI(quad,pMirObj->mirInfo);
		if (quad_mir>=0&&quad_mir<quad_min)
		{
			quad_min=quad_mir;
			relaFatherObj = *pMirObj;	//�и�С����Ĺ�������
			if (quad_min==0)
				break;	//�϶�����С�ĺ���
		}
	}
	if (relaFatherObj.mirInfo.axis_flag==0)
		return FALSE;
	else if (pRelaObj)
		*pRelaObj=relaFatherObj;
	return TRUE;
}
void CLDSNode::UpdateWireDesc()
{
	CXhChar50 sDesc=CUniWireModel::GetWirePtGimName(wireplace,xOriginalPos);
	if(sDesc.GetLength()>0)
		m_sHangWireDesc.Copy(sDesc);
}
bool CLDSNode::IsHangWire()	//�Ƿ���и������͵ĵ���
{
	return m_cHangWireType=='E'||m_cHangWireType=='C'||m_cHangWireType=='J';
}
BOOL CLDSNode::IsHasRestrict(WORD wRestrict)
{
	/*for(CNodeRestrict *pRest=pRestrictList->GetFirst();pRest;pRest=pRestrictList->GetNext())
	{
		if(pRest->m_hWorkModule==_console->ActiveModuleHandle())
			return pRest->IsHasRestrict(wRestrict);
	}
	return FALSE;*/
	return restrict.IsHasRestrict(wRestrict);
}
WORD CLDSNode::GetRestrictWord(THANDLE hModule)		//���ָ��ģ���е�Լ����
{
	/*for(CNodeRestrict *pRest=pRestrictList->GetFirst();pRest;pRest=pRestrictList->GetNext())
	{
		if(pRest->m_hWorkModule==hModule)
			return pRest->GetRestrictWord();
	}*/
	return restrict;//0;
}
BOOL CLDSNode::IsHasRestrict(THANDLE hModule,WORD wRestrict)
{
	/*for(CNodeRestrict *pRest=pRestrictList->GetFirst();pRest;pRest=pRestrictList->GetNext())
	{
		if(pRest->m_hWorkModule==hModule)
			return pRest->IsHasRestrict(wRestrict);
	}
	return FALSE;*/
	return restrict.IsHasRestrict(wRestrict);
}
void CLDSNode::SetRestrict(WORD wRestrict,BOOL bEnable/*=TRUE*/)
{
	/*CNodeRestrict *pRest;
	for(pRest=pRestrictList->GetFirst();pRest;pRest=pRestrictList->GetNext())
	{
		if(pRest->m_hWorkModule==_console->ActiveModuleHandle())
		{
			pRest->SetRestrict(wRestrict,bEnable);
			return;
		}
	}
	pRest=pRestrictList->append();
	pRest->m_hWorkModule=_console->ActiveModuleHandle();
	pRest->SetRestrict(wRestrict,bEnable);*/
	restrict.SetRestrict(wRestrict,bEnable);
}
void CLDSNode::SetRestrict(THANDLE hModule,WORD wRestrict,BOOL bEnable/*=TRUE*/)
{
	/*CNodeRestrict *pRest;
	for(pRest=pRestrictList->GetFirst();pRest;pRest=pRestrictList->GetNext())
	{
		if(pRest->m_hWorkModule==hModule)
		{
			pRest->SetRestrict(wRestrict,bEnable);
			return;
		}
	}
	pRest=pRestrictList->append();
	pRest->m_hWorkModule=hModule;
	pRest->SetRestrict(wRestrict,bEnable);*/
	restrict.SetRestrict(wRestrict,bEnable);
}
void CLDSNode::SetRestrictWord(THANDLE hModule,WORD wRestrict)//�趨ָ��ģ���е�ָ��Լ��
{
	/*CNodeRestrict *pRest;
	for(pRest=pRestrictList->GetFirst();pRest;pRest=pRestrictList->GetNext())
	{
		if(pRest->m_hWorkModule==hModule)
		{
			pRest->SetRestrictWord(wRestrict);
			return;
		}
	}
	pRest=pRestrictList->append();
	pRest->m_hWorkModule=hModule;
	pRest->SetRestrictWord(wRestrict);*/
	restrict.SetRestrictWord(wRestrict);
}
#endif

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
int CONNECT_PLANE::BuildRodSet(CHashSet<CLDSLinePart*>* pHashRayRods,long hExcludeBaseRod/*=0*/)
{
	UINT i;
	pHashRayRods->Empty();
	CLDSLinePart* pRod=NULL;
	for (i=0;i<xarrBasePlaneRods.Count;i++)
	{
		pRod=xarrBasePlaneRods.At(i);
		if (pRod->handle!=hExcludeBaseRod)
			pHashRayRods->SetValue(pRod->handle,pRod);
	}
	for (i=0;i<xarrBendPlaneRods.Count;i++)
	{
		pRod=xarrBendPlaneRods.At(i);
		if (pRod->handle!=hExcludeBaseRod)
			pHashRayRods->SetValue(pRod->handle,pRod);
	}
	for (i=0;i<xarrElsePlaneRods.Count;i++)
	{
		pRod=xarrElsePlaneRods.At(i);
		if (pRod->handle!=hExcludeBaseRod)
			pHashRayRods->SetValue(pRod->handle,pRod);
	}
	return pHashRayRods->GetNodeNum();
}

CONNECT_NODEINFO::CONNECT_NODEINFO()
{
	Clear();
	//m_hBaseNode=m_hBasePart=m_hBendlineAngle=m_hBendlineAngle2=m_hJointPart=m_iConnectWing=0;
	//m_bTransitConnect=false;
	//m_ciBaseRodConnState=0;
}
void CONNECT_NODEINFO::Clear()
{
	memset(this,0,sizeof(CONNECT_NODEINFO));
	xCommNode.planes.SetSize(4);
}
CONNECT_PLANE* CONNECT_NODEINFO::AddPlane(int *lpRsltPlaneIndex/*=NULL*/)
{
	for (UINT i=0;i<xCommNode.planes.CountOfPreset;i++)
	{
		if (!xCommNode.planes[i].blValid)
		{
			*lpRsltPlaneIndex=i;
			return &xCommNode.planes[i];
		}
	}
	return NULL;
	//��������ſ�û��ʵ�����壬�������׵��³����������������ʱ���ױ��� wjh-2019.10.31
	CONNECT_PLANE xTmpPlane;
	if (lpRsltPlaneIndex)
		*lpRsltPlaneIndex=xCommNode.planes.Count;
	return xCommNode.planes.Append(xTmpPlane);
}
CONNECT_PLANE& CONNECT_NODEINFO::get_xXlegPlane() {
	int idleindex=-1;
	for (UINT i=0;i<xCommNode.planes.CountOfPreset;i++)
	{
		if (xCommNode.planes[i].ciConnectWing==0&&xCommNode.planes[i].xarrBasePlaneRods.Count>0)
			return xCommNode.planes[i];
		else if (xCommNode.planes[i].xarrBasePlaneRods.Count==0)
			idleindex=i;
	}
	if(idleindex>=0)
		return xCommNode.planes[idleindex];
	else
	{
		//logerr.Log("error");
		return xCommNode.planes[0];
	}
}
CONNECT_PLANE& CONNECT_NODEINFO::get_xYlegPlane() { 
	int idleindex=-1;
	for (UINT i=0;i<xCommNode.planes.CountOfPreset;i++)
	{
		if (xCommNode.planes[i].ciConnectWing==1&&xCommNode.planes[i].xarrBasePlaneRods.Count>0)
			return xCommNode.planes[i];
		else if (xCommNode.planes[i].xarrBasePlaneRods.Count==0)
			idleindex=i;
	}
	if(idleindex>=0)
		return xCommNode.planes[idleindex];
	else
	{
		//logerr.Log("error");
		return xCommNode.planes[0];
	}
}
CONNECT_PLANE& CONNECT_NODEINFO::get_xAcrossPlane() { 
	int idleindex=-1;
	for (UINT i=0;i<xCommNode.planes.CountOfPreset;i++)
	{
		if (xCommNode.planes[i].ciConnectWing==2&&xCommNode.planes[i].xarrBasePlaneRods.Count>0)
			return xCommNode.planes[i];
		else if (xCommNode.planes[i].xarrBasePlaneRods.Count==0)
			idleindex=i;
	}
	if(idleindex>=0)
		return xCommNode.planes[idleindex];
	else
	{
		//logerr.Log("error");
		return xCommNode.planes[0];
	}
}
bool CONNECT_NODEINFO::EnumConnPointFirst(CConnectPointInfo* pConnPointInfo)
{
	m_siIterSerial=0;
	while (m_siIterSerial<(int)this->xCommNode.planes.Count) 
	{
		if (GetConnectPointInfo(m_siIterSerial,pConnPointInfo))
			return true;
		m_siIterSerial++;
	}
	return GetConnectPointInfo(CONNPLANE_JOINT,pConnPointInfo);
}
bool CONNECT_NODEINFO::EnumConnPointNext(CConnectPointInfo* pConnPointInfo)
{
	m_siIterSerial++;
	if (m_siIterSerial==this->xCommNode.planes.Count)	//��һ�θձ�������ͨ�ڵ����ӹ����棬����Ӧ���ؽ�ͷ���ӵ�
		return GetConnectPointInfo(CONNPLANE_JOINT,pConnPointInfo);
	else if (m_siIterSerial>(int)this->xCommNode.planes.Count)
		return false;	//��һ�θձ������ͷ���ӵ㣬����Ӧ���ؿ�
	//else//��һ�α�����ǰ��ȻΪ��ͨ�ڵ����ӹ�����
	while (m_siIterSerial<(int)this->xCommNode.planes.Count) 
	{
		if (GetConnectPointInfo(m_siIterSerial,pConnPointInfo))
			return true;
		m_siIterSerial++;
	}
	return GetConnectPointInfo(CONNPLANE_JOINT,pConnPointInfo);
}
bool CONNECT_NODEINFO::GetConnectPointInfo(short siPlaneSerial,CConnectPointInfo* pConnPointInfo)
{
	if (ciTransitConnType>0)
		return false;//������´�˫��������
	pConnPointInfo->Clear();
	pConnPointInfo->source.pSrcNodeInfo=this;
	pConnPointInfo->source.ciConnPlaneIndex=(char)siPlaneSerial;
	if (siPlaneSerial==4||siPlaneSerial==CONNECT_NODEINFO::CONNPLANE_JOINT)
	{	//��ȡ��ͷ
		if ((cbDisableFlag&0x80)>0)
			return false;
		pConnPointInfo->m_ciBaseRodConnState=4;	//4.���߽ضϵ�����
		pConnPointInfo->designType.bFlJoint=xCommNode.blFlJoint;
		pConnPointInfo->designType.bAngleJoint=!xCommNode.blFlJoint;
		pConnPointInfo->m_hBaseNode=xCommNode.hBaseNode;
		pConnPointInfo->m_hBasePart=xCommNode.hBaseRod;
		pConnPointInfo->m_hJointPart=xCommNode.hJointRod;
		return true;
	}
	if (siPlaneSerial<0||siPlaneSerial>2)
		return false;
	CONNECT_PLANE* pConnPlane=xCommNode.planes.GetAt(siPlaneSerial);
	if (pConnPlane==NULL||!pConnPlane->blValid)
		return false;
	pConnPointInfo->m_bTransitConnect=false;
	pConnPointInfo->m_ciBaseRodConnState=xCommNode.ciBaseRodConnState;
	pConnPointInfo->m_hBaseNode=xCommNode.hBaseNode;
	pConnPointInfo->m_hBasePart=xCommNode.hBaseRod;
	pConnPointInfo->m_hJointPart=xCommNode.hJointRod;
	pConnPointInfo->datumplane=pConnPlane->datumplane;
	pConnPointInfo->bendplane =pConnPlane->bendplane;
	pConnPointInfo->bendplane2=pConnPlane->bendplane2;
	pConnPointInfo->designType=pConnPlane->designType;
	pConnPointInfo->vDatumDirect=pConnPlane->vDatumDirect;
	pConnPointInfo->m_iConnectWing=pConnPlane->ciConnectWing;
	pConnPointInfo->m_hBendlineAngle=pConnPlane->hBendlineAngle;
	pConnPointInfo->m_hBendlineAngle2=pConnPlane->hBendlineAngle2;
	pConnPointInfo->linkPartSet.Empty();
	UINT i;
	for (i=0;i<pConnPlane->xarrBasePlaneRods.Count;i++)
		pConnPointInfo->linkPartSet.append(pConnPlane->xarrBasePlaneRods.At(i));
	for (i=0;i<pConnPlane->xarrBendPlaneRods.Count;i++)
		pConnPointInfo->bendPlaneRodSet.append(pConnPlane->xarrBendPlaneRods.At(i));
	for (i=0;i<pConnPlane->xarrElsePlaneRods.Count;i++)
		pConnPointInfo->bendPlaneRodSet2.append(pConnPlane->xarrElsePlaneRods.At(i));
	return true;
}
bool CONNECT_NODEINFO::InitFromConnectPointInfo(CConnectPointInfo* pConnPointInfo,int liToConnPlaneIndex)
{
	//pConnPoint->cfgword=pBaseRod->UnifiedCfgword()&pRayRod->UnifiedCfgword();
	CONNECT_PLANE* pDestPlane=xCommNode.planes.GetAt(liToConnPlaneIndex);
	if (pDestPlane==NULL)
		return false;
	xCommNode.ciBaseRodConnState=pConnPointInfo->m_ciBaseRodConnState;
	xCommNode.hBaseNode=pConnPointInfo->m_hBaseNode;
	xCommNode.hBaseRod=pConnPointInfo->m_hBasePart;
	//xCommNode.hJointRod=pConnPointInfo->m_hJointPart;
	pDestPlane->vDatumDirect=pConnPointInfo->vDatumDirect;
	pDestPlane->datumplane=pConnPointInfo->datumplane;
	pDestPlane->bendplane =pConnPointInfo->bendplane;
	pDestPlane->bendplane2=pConnPointInfo->bendplane2;
	pDestPlane->wnMaxRayLsNum=pConnPointInfo->m_nMaxRayLsNum;
	pDestPlane->ciConnectWing=pConnPointInfo->m_iConnectWing;	//���ӹ̽��ڻ�׼�Ǹ�X֫��
	pDestPlane->hBendlineAngle =pConnPointInfo->m_hBendlineAngle;
	pDestPlane->hBendlineAngle2=pConnPointInfo->m_hBendlineAngle2;
	CLDSLinePart* pRod;
	pDestPlane->xarrBasePlaneRods.Clear();
	pDestPlane->xarrBendPlaneRods.Clear();
	pDestPlane->xarrElsePlaneRods.Clear();
	for (pRod=pConnPointInfo->linkPartSet.GetFirst();pRod;pRod=pConnPointInfo->linkPartSet.GetNext())
		pDestPlane->xarrBasePlaneRods.Append(pRod);
	for (pRod=pConnPointInfo->bendPlaneRodSet.GetFirst();pRod;pRod=pConnPointInfo->bendPlaneRodSet.GetNext())
		pDestPlane->xarrBendPlaneRods.Append(pRod);
	for (pRod=pConnPointInfo->bendPlaneRodSet2.GetFirst();pRod;pRod=pConnPointInfo->bendPlaneRodSet2.GetNext())
		pDestPlane->xarrElsePlaneRods.Append(pRod);
	return true;
}
CConnectPointInfo::CConnectPointInfo()
{
	m_hBaseNode=m_hBasePart=m_hBendlineAngle=m_hBendlineAngle2=m_hJointPart=m_iConnectWing=0;
	m_nMaxRayLsNum=2;
	m_bTransitConnect=false;
	m_ciBaseRodConnState=0;
}
void CConnectPointInfo::Clear()
{
	m_hBaseNode=m_hBasePart=m_hBendlineAngle=m_hBendlineAngle2=m_hJointPart=m_iConnectWing=0;
	m_nMaxRayLsNum=2;
	m_bTransitConnect=false;
	m_ciBaseRodConnState=0;
	linkPartSet.Empty();
	bendPlaneRodSet.Empty();
	bendPlaneRodSet2.Empty();
}
int CConnectPointInfo::BuildRodSet(CHashSet<CLDSLinePart*>* pHashRayRods)
{
	CLDSLinePart* pRod;
	pHashRayRods->Empty();
	for (pRod=linkPartSet.GetFirst();pRod;pRod=linkPartSet.GetNext())
	{
		if (pRod->handle!=m_hBasePart)
			pHashRayRods->SetValue(pRod->handle,pRod);
	}
	for (pRod=bendPlaneRodSet.GetFirst();pRod;pRod=bendPlaneRodSet.GetNext())
		pHashRayRods->SetValue(pRod->handle,pRod);
	for (pRod=bendPlaneRodSet2.GetFirst();pRod;pRod=bendPlaneRodSet2.GetNext())
		pHashRayRods->SetValue(pRod->handle,pRod);
	return pHashRayRods->GetNodeNum();
}
//����ʶ�����ӵ���Ϣ
static CLDSLinePart* Add2Array(CLDSLinePart* pRod,CLDSLinePartPtr* parr,UINT nMaxArrSize=16)
{
	for (UINT i=0;i<nMaxArrSize;i++)
	{
		if (parr[i]==NULL)
			return parr[i]=pRod;
	}
	return NULL;
}
bool CLDSNode::RetrieveSlopeTransitionNode(SLOPE_TRANSITION* pTransition)
{
	if (Layer(0)=='T'||!isdigit(Layer(2)))
		return false;
	if (m_pModel==NULL||!m_pModel->IsTowerModel())
		return false;
	CLdsPartListStack stack(&((CTower*)m_pModel)->Parts);
	CLDSLinePart* pFatherRod=BelongModel()->FromRodHandle(hFatherPart);
	if (pFatherRod==NULL||pFatherRod->pStart==NULL||pFatherRod->pEnd==NULL||!pFatherRod->IsAngle())
		return false;
	GEPOINT vBaseRodVec;
	if (pFatherRod->pStart==this)
		vBaseRodVec=pFatherRod->pEnd->Position(false)-pos;
	else if (pFatherRod->pEnd==this)
		vBaseRodVec=pFatherRod->pStart->Position(false)-pos;
	else
		return false;
	normalize(vBaseRodVec);
	pTransition->pNode=this;
	pTransition->pUpRod=pTransition->pHoriRodX=pTransition->pHoriRodY=NULL;
	pTransition->ClearDownRods();
	if (vBaseRodVec.z<0)		//���˼���Ϊ�ϲ��׼�˼�
		pTransition->pUpRod=pFatherRod;
	else if (vBaseRodVec.z>0)//���˼���Ϊ�²��׼�˼�
	{
		if (Add2Array(pFatherRod,pTransition->xarrDownRods)==NULL)//.Append(pFatherRod);
			logerr.Log("0x%X�ڵ������ӵ��������ĳ����������(16��)!",handle);
	}
	GEPOINT vOtherPrimaryLenVec;
	for (CLDSLinePart* pRod=m_pModel->EnumRodFirst();pRod;pRod=m_pModel->EnumRodNext())
	{
		if (pRod->pStart==NULL||pRod->pEnd==NULL||(pRod->pStart!=this&&pRod->pEnd!=this)||
			(toupper(pRod->Layer(1))!='Z'&&toupper(pRod->Layer(1))!='H'))                     //���Ǻ�� �������� 
			continue;   //������ڵ�û��ϵ�ĸ˼�
		GEPOINT vLenVec;
		if (pRod->pStart==this)
			vLenVec=pRod->pEnd->pos-pRod->pStart->pos;
		else
			vLenVec=pRod->pStart->pos-pRod->pEnd->pos;
		if (fabs(vLenVec.y)+fabs(vLenVec.z)<EPS)
			pTransition->pHoriRodX=pRod;	//X�᷽��ˮƽ�˼�
		else if (fabs(vLenVec.x)+fabs(vLenVec.z)<EPS)
			pTransition->pHoriRodY=pRod;	//Y�᷽��ˮƽ�˼�
		else
		{   //Ѱ���ϸ˼����¸˼�
			normalize(vLenVec);
			if (fabs(vLenVec.z)<0.966)
				continue;	//��������������Z��нǲ�Ӧ����15��(����λʸ����Z����<0.966)
			if (pTransition->pUpRod==pFatherRod)
			{	//λ�ڽڵ��²�����ֱ�ĸ˼�
				if (vLenVec.z - vOtherPrimaryLenVec.z>EPS)
				{
					vOtherPrimaryLenVec = vLenVec;
					pTransition->xarrDownRods[0]=pRod;
					//pTransition->xarrDownRods.ReSize(1);
				}
				else if (fabs(vLenVec.z - vOtherPrimaryLenVec.z) < EPS)
				{
					if (Add2Array(pRod,pTransition->xarrDownRods)==NULL)//.Append(pFatherRod);
						logerr.Log("0x%X�ڵ������ӵ��������ĳ����������(16��)!",handle);
				}
			}
			if (pTransition->pUpRod ==NULL&&vLenVec.z<vOtherPrimaryLenVec.z)
			{	//λ�ڽڵ��ϲ�����ֱ�ĸ˼�
				vOtherPrimaryLenVec=vLenVec;
				pTransition->pUpRod=pRod;
			}
		}
	}
	//�ж����¶����ǹ���;
	if (fabs(vBaseRodVec*vOtherPrimaryLenVec)>EPS_COS)//�ǹ���
	{
		pTransition->pUpRod=NULL;
		pTransition->ClearDownRods();
	}
	if (pTransition->pUpRod==NULL||pTransition->pUpRod->GetClassTypeId()!=CLS_LINEANGLE||
		pTransition->pDownRod==NULL||!pTransition->pDownRod->IsAngle())
		return false;//Ŀǰ�ݲ�֧����ϽǸ���ʽ�ı��µ���� wjh-2016.3.8
	else
		return true;
}
bool CLDSNode::RetrieveFootTransitionNode(SLOPE_TRANSITION* pTransition)
{
	if (Layer(0)!='L'||!isdigit(Layer(2)))
		return false;	//�Ȳ����ص�
	if (m_pModel==NULL||!m_pModel->IsTowerModel())
		return false;
	CLdsPartListStack stack(&((CTower*)m_pModel)->Parts);
	CLDSLineAngle* pFatherJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	if (pFatherJg==NULL||pFatherJg->pStart==NULL||pFatherJg->pEnd==NULL)
		return false;
	if (pFatherJg->pStart!=this && pFatherJg->pEnd!=this)
		return false;
	GEPOINT vBaseRodVec;
	if (pFatherJg->pStart==this)
		vBaseRodVec=pFatherJg->pEnd->Position(false)-pos;
	else if (pFatherJg->pEnd==this)
		vBaseRodVec=pFatherJg->pStart->Position(false)-pos;
	normalize(vBaseRodVec);
	if (vBaseRodVec.z>0)
		return false;	//���²��׼�˼�
	pTransition->pHoriRodX=pTransition->pHoriRodY=NULL;
	pTransition->ClearDownRods();
	pTransition->pNode=this;
	pTransition->pUpRod=pFatherJg;
	for (CLDSLinePart* pRod=m_pModel->EnumRodFirst();pRod;pRod=m_pModel->EnumRodNext())
	{
		if (!pRod->IsAngle())
			continue;
		CLDSLineAngle* pAngle=(CLDSLineAngle*)pRod;
		if (pAngle->handle==pFatherJg->handle||pAngle->group_father_jg_h==pFatherJg->handle)
			continue;
		if (toupper(pRod->Layer(0))!='L'||toupper(pRod->Layer(1))!='Z')
			continue;
		if (!pRod->IsAngle()||pRod->pStart==NULL||pRod->pEnd==NULL||(pRod->pStart!=this&&pRod->pEnd!=this))
			continue;
		GEPOINT vLenVec;
		if (pRod->pStart==this)
			vLenVec=(pRod->pEnd->pos-pRod->pStart->pos).normalized();
		else
			vLenVec=(pRod->pStart->pos-pRod->pEnd->pos).normalized();
		if (fabs(vBaseRodVec*vLenVec)>EPS_COS)
			return false;
	}
	return true;
}
//�˺�����CConnectPointInfo::datumplane.vLineδ��ֵ����Ҫ�ڴ˺�������¸�ֵ wjh-2019.8.31
static bool InitConnectPointBasicInfo(CConnectPointInfo* pConnPoint,CLDSNode* pBaseNode,SmartRodPtr pBaseRod,SmartRodPtr pRayRod)
{
	pConnPoint->cfgword=pBaseRod->UnifiedCfgword()&pRayRod->UnifiedCfgword();
	pConnPoint->linkPartSet.append(pRayRod);
	pConnPoint->m_hBaseNode=pBaseNode->handle;
	pConnPoint->m_hBasePart=pBaseRod->handle;
	GEPOINT vDatumRodVec=pBaseRod->xPosEnd-pBaseRod->xPosStart;
	GEPOINT vRayRodVec=pRayRod->xPosEnd-pRayRod->xPosStart;
	if (pRayRod->pEnd==pBaseNode)
		vRayRodVec*=-1;
	GEPOINT vWorkNorm=vDatumRodVec^vRayRodVec;
	pConnPoint->vDatumDirect=vDatumRodVec^vWorkNorm;
	if (pConnPoint->vDatumDirect*vRayRodVec<0)
		pConnPoint->vDatumDirect*=-1.0;
	pConnPoint->datumplane.vSolid=vWorkNorm;
	if (pRayRod->pStart==pBaseNode)
		pConnPoint->m_nMaxRayLsNum=pRayRod->connectStart.wnConnBoltN;
	else
		pConnPoint->m_nMaxRayLsNum=pRayRod->connectEnd.wnConnBoltN;
	if (pBaseRod->IsAngle())
	{	//��׼�˼�Ϊ�Ǹ�ʱ���жϻ�׼�Ǹ�����֫
		double ddx=pConnPoint->datumplane.vSolid*pBaseRod.pAngle->vxWingNorm;
		double ddy=pConnPoint->datumplane.vSolid*pBaseRod.pAngle->vyWingNorm;
		if (fabs(ddx)>fabs(ddy))
		{
			pConnPoint->m_iConnectWing=0;	//���ӹ̽��ڻ�׼�Ǹ�X֫��
			if (ddx<0)
				pConnPoint->datumplane.vSolid *= -1.0;
		}
		else
		{
			pConnPoint->m_iConnectWing=1;	//���ӹ̽��ڻ�׼�Ǹ�Y֫��
			if (ddy<0)
				pConnPoint->datumplane.vSolid *= -1.0;
		}
	}
	else if (pBaseRod->GetClassTypeId()==CLS_LINETUBE)
	{
		if (pConnPoint->vDatumDirect.IsZero())
			pConnPoint->vDatumDirect=vDatumRodVec^pConnPoint->datumplane.vLine;
		if (pConnPoint->vDatumDirect*vRayRodVec>0)
			pConnPoint->m_iConnectWing=0;	//��׼�˼��Ҳ�
		else
			pConnPoint->m_iConnectWing=1;	//��׼�˼����
	}
	else
		pConnPoint->m_iConnectWing=2;
	normalize(pConnPoint->vDatumDirect);
	return true;
}
CONNECT_NODEINFO* CLDSNode::GetConnectNodeInfo() {
	//if (m_pxConnNodeInfo==NULL)
	//	RetrieveConnectNodeInfo(NULL);
	return m_pxConnNodeInfo; 
}
bool CLDSNode::RetrieveConnectNodeInfo(CONNECT_NODEINFO* pConnNodeInfo,BOOL bDesignBendConnect/*=TRUE*/,CLDSModule *pWorkHeight/*=NULL*/)
{
	CSuperSmartPtr<CLDSLinePart> pLinePart;
	CHashSet<CLDSLinePart*> linkPartSet;	//�����ڵ�ǰ�ڵ��ϵĸ˼�����
	if (pConnNodeInfo==NULL&&m_pxConnNodeInfo==NULL)
		pConnNodeInfo=m_pxConnNodeInfo=new CONNECT_NODEINFO();
	else if (pConnNodeInfo==NULL)
		pConnNodeInfo=m_pxConnNodeInfo;
	pConnNodeInfo->Clear();
	pConnNodeInfo->ciTransitConnType=0;
	if (RetrieveSlopeTransitionNode(&pConnNodeInfo->transit))
	{	//���´����
		pConnNodeInfo->ciTransitConnType=1;
		return true;
	}
	else if (RetrieveFootTransitionNode(&pConnNodeInfo->transit))
	{
		pConnNodeInfo->ciTransitConnType=2;
		pConnNodeInfo->transit.bBaseFoot=true;
		return true;
	}
	//1����ȡ�ڵ㸸�˼�
	if (m_cPosCalType==4)
	{	//���ݽ���ڵ㸸�˼�,У������㴦��ϵĽ����ĸ��˼�
		CLDSLinePart *pLinePart1=(CLDSLinePart*)BelongModel()->FromPartHandle(arrRelationPole[0],CLS_LINEPART);
		CLDSLinePart *pLinePart2=(CLDSLinePart*)BelongModel()->FromPartHandle(arrRelationPole[1],CLS_LINEPART);
		if (pLinePart1&&pLinePart1->pStart&&pLinePart1->pStart->handle!=handle
			&&pLinePart1->pEnd&&pLinePart1->pEnd->handle!=handle)
			hFatherPart=arrRelationPole[0];
		else if (pLinePart2&&pLinePart2->pStart&&pLinePart2->pStart->handle!=handle
			&&pLinePart2->pEnd&&pLinePart2->pEnd->handle!=handle)
			hFatherPart=arrRelationPole[1];
	}
	SmartRodPtr pDatumRod=BelongModel()->FromRodHandle(hFatherPart);
	if (!pDatumRod.blValid)
		return false;	//��׼�˼����Ϸ�
	CLDSLinePart *pCoupleLinePart=NULL;
	if ((!pDatumRod->IsAngle()&&!pDatumRod->IsTube())||pDatumRod->pStart==NULL||pDatumRod->pEnd==NULL)
		return false;	//��ʱֻ֧�ֻ�׼����Ϊ�Ǹ֣���ϽǸ֣��ֹܵ����
	CONNECT_NODEINFO xConnNodeInfo;//SLOPE_TRANSITION transit;
	xConnNodeInfo.xCommNode.hBaseNode=handle;
	xConnNodeInfo.xCommNode.hBaseRod=pDatumRod->handle;
	//2������ȫ���������ڴ˽ڵ��ϵĸ˼���������linkPartSet�����а������˼�����ȥ����
	CLDSModule *pModule=pWorkHeight!=NULL?pWorkHeight:_console->GetActiveModule();
	BOOL pushed=m_pModel->PushPartStack(); //���������б�ǰӦ��ѹջ wht 12-03-05
	for (pLinePart=m_pModel->EnumRodFirst();pLinePart.IsHasPtr();pLinePart=m_pModel->EnumRodNext())
	{	//Ӧʹ��IsSonPart�ж��Ƿ�Ϊ��ǰģ���еĹ�������Ӧʹ��ģ����ĺűȽ� wht 10-08-02
		if (!pLinePart->IsAngle()&&pLinePart->GetClassTypeId()!=CLS_LINETUBE)
			continue;	//��֧�ֽǸ֡��ֹ�
		if (pLinePart->GetClassTypeId()==CLS_LINEANGLE&&pLinePart.pAngle->hGroupAngle>0x20)
			continue;	//��ϽǸֵ��ӽǸ�����ϽǸִ����������
		if (pModule&&!pModule->IsSonPart(pLinePart))
			continue;	//�ǵ�ǰ������߷������� wht 10-07-23
		if (pLinePart->m_bVirtualPart)
			continue;	//�������⸨������ wjh-2019.8.31
		if (pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;	//һ��˽ڵ�Ϊ����Ϊ�̽Ǹ֣��������� wjh-2017.12.18
		if (pLinePart->pStart==this||pLinePart->pEnd==this)
		{
			if (pDatumRod.IsEqualPtr(pLinePart))
				continue;
			linkPartSet.SetValue(pLinePart->handle,pLinePart);
		}
	}
	m_pModel->PopPartStack(pushed);
	UINT i=0,j=0;
	LDSNORMAL datumlinevec,linevec,vRayDatumLineVec;
	GEPOINT base_norm_wing_x,base_norm_wing_y;
	if (pDatumRod->IsAngle())
	{	//��ȡ���ӻ�׼�Ǹ���֫�ķ��߷��򣬼��������ӹ����淨��
		base_norm_wing_x=pDatumRod.pAngle->vxWingNorm;
		base_norm_wing_y=pDatumRod.pAngle->vyWingNorm;
	}
	datumlinevec.vLine=pDatumRod->pEnd->pos-pDatumRod->pStart->pos;
	datumlinevec.vSolid=pDatumRod->End()-pDatumRod->Start();
	normalize(datumlinevec.vLine);
	normalize(datumlinevec.vSolid);
	vRayDatumLineVec=datumlinevec;
	if (pDatumRod->pEnd==this)
	{	//��֤��׼�˼�Ϊ��ǰ�ڵ����߸˼�ʱ��vRayDatumLineVecΪ���߷��� wjh-2019.11.03
		vRayDatumLineVec.vLine *=-1.0;
		vRayDatumLineVec.vSolid*=-1.0;
	}
	if (m_cPosCalType==4&&linkPartSet.GetNodeNum()<=1)
	{	//3.����㵥��˨����
		SmartRodPtr pLinePart1=m_pModel->FromRodHandle(arrRelationPole[0]);
		SmartRodPtr pLinePart2=m_pModel->FromRodHandle(arrRelationPole[1]);
		if (pLinePart1.IsHasPtr()&&pLinePart1->IsAngle()&&pLinePart2.IsHasPtr()&&pLinePart2->IsAngle())
		{
			//pConnectPoint=GetConnectPointInfoList()->append();
			xConnNodeInfo.xCommNode.planes[0].ciConnectWing=2;
			xConnNodeInfo.xCommNode.planes[0].designType.bIntersBolt = TRUE;
			GEPOINT line_vec1=pLinePart1->xPosEnd-pLinePart1->xPosStart;
			GEPOINT line_vec2=pLinePart2->xPosEnd-pLinePart2->xPosStart;
			xConnNodeInfo.xCommNode.planes[0].datumplane.vSolid=line_vec1^line_vec2;
			//����vLine��ʵ�������ȡֵvSolid
			xConnNodeInfo.xCommNode.planes[0].datumplane.vLine=xConnNodeInfo.xCommNode.planes[0].datumplane.vSolid;
			*pConnNodeInfo=xConnNodeInfo;
			pConnNodeInfo->xCommNode.planes[0].xarrBasePlaneRods.Clear();
			pConnNodeInfo->xCommNode.planes[0].xarrBasePlaneRods.Append(pLinePart1);
			pConnNodeInfo->xCommNode.planes[0].xarrBasePlaneRods.Append(pLinePart2);
			return  true;
		}
		else
			return false;
	}

	//4��ͳ�Ʒ��������߸˼����׼�˼��γɵ��������ӵ���Ϣ
	//������Ȳ����ı���Ҫ��ڵ�����ӹ���λ���²�������
	if (pDatumRod->layer(0)!='T'&&pDatumRod->layer(1)=='Z'&&pDatumRod->pEnd==this)
	{
		datumlinevec.vLine *=-1.0;//��֤datum_line_vecΪ���߷���
		datumlinevec.vSolid*=-1.0;
	}
	bool swapCoupleAngle=false;	//�Ƿ��ѽ�pDatumRod��pCoupleLinePart�����˽���
	BYTE cbExistConnPlaneFlag=0;	//0x01:��ʾX֫;0x02��ʾY֫;0x04��ʾ�����
	//������ȡ�Ǹֻ�׼�ĵ���֫�����漰����������� wjh-2019.8.31
	if (pDatumRod->IsAngle())
	{
		bool blAddLegX=false,blAddLegY=false;
		for (pLinePart=linkPartSet.GetFirst();pLinePart.IsHasPtr();pLinePart=linkPartSet.GetNext())
		{
			if (pLinePart->pStart->handle==handle)
			{
				linevec.vLine=pLinePart->pEnd->pos-pLinePart->pStart->pos;
				linevec.vSolid=pLinePart->xPosEnd-pLinePart->xPosStart;
			}
			else
			{
				linevec.vLine=pLinePart->pStart->pos-pLinePart->pEnd->pos;
				linevec.vSolid=pLinePart->xPosStart-pLinePart->xPosEnd;
			}
			normalize(linevec.vLine);
			normalize(linevec.vSolid);
			double justify=fabs(linevec.vLine*datumlinevec.vLine);
			if (justify>0.966)	//�������߻�׼�˼��������׼�˼�����һ���нǣ�>15��)
				continue;	//���׼�Ĺ��ߵĸ˼�
			LDSNORMAL worknorm;
			worknorm.vLine=linevec.vLine^datumlinevec.vLine;
			worknorm.vSolid=linevec.vSolid^datumlinevec.vSolid;
			normalize(worknorm.vLine);
			normalize(worknorm.vSolid);
			double ddx=worknorm.vSolid*base_norm_wing_x;
			double ddy=worknorm.vSolid*base_norm_wing_y;
			const BYTE xarrDualBytes[2]={ 0x01,0x02 };
			if ((!blAddLegX&&fabs(ddx)>0.996)||	//�·���X֫������
				(!blAddLegY&&fabs(ddy)>0.996))	//�·���Y֫������
			{	//X/Y֫���Ӹ˼�����������Ǹ�����֫��нǲ�Ӧ����5��,cos(5��)=0.996
				CConnectPointInfo xConnPoint;//=this->GetConnectPointInfoList()->append();
				InitConnectPointBasicInfo(&xConnPoint,this,pDatumRod,pLinePart);
				xConnPoint.datumplane=worknorm;
				if (fabs(ddx)>0.996)
				{
					blAddLegX=true;
					if (ddx<0)
					{
						xConnPoint.datumplane.vLine*=-1.0;
						xConnPoint.datumplane.vSolid*=-1.0;
					}
					int index=0;
					if (((cbExistConnPlaneFlag&0x02)==0&&fabs(worknorm.vLine.y)<EPS2)||
						 (cbExistConnPlaneFlag&0x01)>0)
						index=1;	//���Ҳ�������ƽ��ʱ��ֵ1
					xConnNodeInfo.InitFromConnectPointInfo(&xConnPoint,index);
					cbExistConnPlaneFlag|=xarrDualBytes[index];
				}
				else //if (fabs(ddy)>0.996)
				{
					blAddLegY=true;
					if (ddy<0)
					{
						xConnPoint.datumplane.vLine*=-1.0;
						xConnPoint.datumplane.vSolid*=-1.0;
					}
					int index=1;
					if (((cbExistConnPlaneFlag&0x01)==0&&fabs(worknorm.vLine.x)<EPS2)||
						 (cbExistConnPlaneFlag&0x02)>0)
						index=0;	//ǰ����������ƽ��ʱ��ֵ0
					xConnNodeInfo.InitFromConnectPointInfo(&xConnPoint,index);
					cbExistConnPlaneFlag|=xarrDualBytes[index];
				}
				linkPartSet.DeleteCursor();	//������Ƴ��ø˼�
			}
			/*else if (min(ddx,ddy)>0.5&&max(ddx,ddy)<0.85)
			{	//��Ҫ�Ҹ����Ӧ����������ɺ�������� wjh-2019.9.30
				linkPartSet.DeleteCursor();	//������Ƴ��ø˼�
				InitConnectPointBasicInfo(&xConnPoint,this,pDatumRod,pLinePart);
				xConnNodeInfo.InitFromConnectPointInfo(&xConnPoint,0);
				xConnNodeInfo.xCommNode.planes[2].datumplane=worknorm;
				xConnNodeInfo.xCommNode.planes[2].xarrBasePlaneRods.Append(pDatumRod);
				cbExistConnPlaneFlag|=0x04;
			}*/
			if (cbExistConnPlaneFlag==0x03)
				break;
		}
	}
	for (pLinePart=linkPartSet.GetFirst();pLinePart.IsHasPtr();pLinePart=linkPartSet.GetNext())
	{
		int base_bolt_num = 0;	//���ӻ�׼�����ϵ���˨��
		if (pLinePart->pStart->handle==handle)
		{
			linevec.vLine=pLinePart->pEnd->pos-pLinePart->pStart->pos;
			linevec.vSolid=pLinePart->End()-pLinePart->Start();
		}
		else
		{
			linevec.vLine=pLinePart->pStart->pos-pLinePart->pEnd->pos;
			linevec.vSolid=pLinePart->Start()-pLinePart->End();
		}
		normalize(linevec.vLine);
		normalize(linevec.vSolid);
		bool blColinearRod=false;
		double justify=linevec.vLine*vRayDatumLineVec.vLine;
		if (justify>EPS_COS)
		{
			if (pDatumRod.pAngle->hGroupAngle==pLinePart->handle)
				pDatumRod=pLinePart;
			else
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%x�˼���0x%x�˼��ص�",pLinePart->handle,pDatumRod->handle);
			continue;
		}
		else if (justify<-EPS_COS)
		{	//��Ҫ��ƽ�ͷ�Ĺ��߸˼�
			//��ӽǸֽ�ͷ�������ӵ�
			xConnNodeInfo.xCommNode.hJointRod=pLinePart->handle;
			xConnNodeInfo.xCommNode.blFlJoint = pDatumRod->IsTube()&&pLinePart->IsTube();
			blColinearRod=true;

			if (pCoupleLinePart==NULL)
				pCoupleLinePart=pLinePart;
			//TODO:��ʱδ�ܴ������߶���Ȼ��Ӵ���������ơ�wjh-2015.12.29
			else if (!pModule->IsSonPart(pCoupleLinePart)&&!pModule->IsSonPart(pLinePart))
				pCoupleLinePart=pLinePart;
			else if (((CLDSLineAngle*)pCoupleLinePart)->group_father_jg_h==pLinePart->handle)
				pCoupleLinePart=pLinePart;	//��������ϽǸ�Ϊ��ͷ�ԽӽǸ�
			if (!swapCoupleAngle&&vRayDatumLineVec.vLine.z<0&&linevec.vLine.z>0)
			{	//���߸˼���ͷ��׼�˼�ȡ�²�ȡ��, wjh-2015.1.17
				CLDSLinePart* pTempRod=pDatumRod;
				pDatumRod=pCoupleLinePart;
				pCoupleLinePart=pTempRod;
				xConnNodeInfo.xCommNode.hBaseRod=pDatumRod->handle;
				xConnNodeInfo.xCommNode.hJointRod=pCoupleLinePart->handle;
				swapCoupleAngle=true;
			}
		}
		LDSNORMAL worknorm;
		worknorm.vLine=linevec.vLine^datumlinevec.vLine;
		worknorm.vSolid=linevec.vSolid^datumlinevec.vSolid;
		normalize(worknorm.vLine);
		normalize(worknorm.vSolid);
		CMultiOffsetPos *pDesPosPara=NULL;
		if (pLinePart->IsAngle())//GetClassTypeId()==CLS_LINEANGLE)
		{
			if (pLinePart->pStart==this)
			{
				pDesPosPara=&pLinePart.LineAnglePointer()->desStartPos;
				base_bolt_num=pLinePart->connectStart.wnConnBoltN;
			}
			else if (pLinePart->pEnd==this)
			{
				pDesPosPara=&pLinePart.LineAnglePointer()->desEndPos;
				base_bolt_num=pLinePart->connectEnd.wnConnBoltN;
			}
		}
		//�޳�δ�����ӹ������ϵĽǸ�
		if (pDatumRod->IsAngle()&&pDesPosPara)
		{	//��׼�˼�Ϊ�Ǹ�
			bool passcheck=false;	//����������ų�����������Ӹ˼�
			if (fabs(worknorm.vSolid*base_norm_wing_x)>EPS_COS2||fabs(worknorm.vSolid*base_norm_wing_y)>=EPS_COS2)
				passcheck=true;	//δ����ƫ����������֫
			else if(pDesPosPara->IsInHuoQuLine(true)&&(pDesPosPara->IsInDatumJgWingX()||
				pDesPosPara->IsInDatumJgWingY()||pDesPosPara->huoqu_pos_auto_0_x_wing1_y_wing2>0))	//δָ����ǰ�Ǹ���һ֫Ϊ��λ����֫
				passcheck=true;	//��ָ����ǰ�Ǹ�ĳһ֫Ϊ�����߶�λ����֫
			else if (blColinearRod)
				passcheck=true;	//���߸˼�
			else
				continue;	//�������ӽǸ�δ���ӵ���ǰ����֫��
		}
		bool blFindResideConnPointPlane=false;
		for (i=0;i<xConnNodeInfo.xCommNode.planes.Count;i++)
		{
			CONNECT_PLANE* pConnPlane=xConnNodeInfo.xCommNode.planes.GetAt(i);
			if (!pConnPlane->blValid)
				continue;
			if (pLinePart->IsAngle()&&pDatumRod->IsAngle()&&pDesPosPara)
			{	//��׼�˼������߸˼���Ϊ�Ǹ�
				if (bDesignBendConnect&&
					!pConnPlane->bendplane.vLine.IsZero()&&linevec.vLine*pConnPlane->vDatumDirect<0&&
					fabs(linevec.vLine*pConnPlane->bendplane.vLine)<=0.01)
				{	//�˼�λ��1#������
					pConnPlane->xarrBendPlaneRods.Append(pLinePart);
					if (base_bolt_num>pConnPlane->wnMaxRayLsNum)
						pConnPlane->wnMaxRayLsNum=base_bolt_num;
					blFindResideConnPointPlane=true;
					continue;	//�˴�break����©��ͬʱ����˫����ƽ��ĸ˼�
				}
				else if (fabs(linevec.vLine*pConnPlane->datumplane.vLine)<=0.01)
				{	//�˼�λ�ڻ�׼��
					pConnPlane->xarrBasePlaneRods.Append(pLinePart);
					if (base_bolt_num>pConnPlane->wnMaxRayLsNum)
						pConnPlane->wnMaxRayLsNum=base_bolt_num;
					blFindResideConnPointPlane=true;
					continue;	//break;�˴�break����©��ͬʱ����˫����ƽ��ĸ˼�
				}
				else if (bDesignBendConnect&&!worknorm.vLine.IsZero()&&pConnPlane->bendplane.vLine.IsZero()&&
					(linevec.vLine*pConnPlane->vDatumDirect)<0)
				{	//�˼�λ������������
					if ((pConnPlane->ciConnectWing==0&&
						fabs(worknorm.vSolid*base_norm_wing_x)>fabs(pConnPlane->datumplane.vSolid*base_norm_wing_x))||(
							pConnPlane->ciConnectWing==1&&
							fabs(worknorm.vSolid*base_norm_wing_y)>fabs(pConnPlane->datumplane.vSolid*base_norm_wing_y)))
					{	//������׼ƽ�������ƽ��
						pConnPlane->bendplane=pConnPlane->datumplane;
						pConnPlane->vDatumDirect*=-1.0;
						pConnPlane->xarrBendPlaneRods.CloneFrom(pConnPlane->xarrBasePlaneRods);
						pConnPlane->xarrBasePlaneRods.Clear();
						pConnPlane->xarrBasePlaneRods.Append(pLinePart);
						pConnPlane->datumplane=worknorm;
					}
					else
					{
						BYTE ciWorkWing=-1;
						//char cDatumAngleWing=pLineAngle->GetLappedWorkWing(TRUE,NULL);
						//pDesPosPara->cDatumAngleWing
						if (pDesPosPara->IsInDatumJgWingX()||(pDesPosPara->IsInHuoQuLine(true)&&pDesPosPara->huoqu_pos_auto_0_x_wing1_y_wing2==1))
							ciWorkWing=0;
						else if (pDesPosPara->IsInDatumJgWingY()||(pDesPosPara->IsInHuoQuLine(true)&&pDesPosPara->huoqu_pos_auto_0_x_wing1_y_wing2==2))
							ciWorkWing=1;
						else if (fabs(worknorm.vSolid*base_norm_wing_x)>EPS_COS2)
							ciWorkWing=0;
						else if (fabs(worknorm.vSolid*base_norm_wing_y)>EPS_COS2)
							ciWorkWing=1;
						if (pConnPlane->vDatumDirect*linevec.vLine>0)
							continue;
						if (pConnPlane->ciConnectWing==ciWorkWing)
						{
							pConnPlane->bendplane=worknorm;
							pConnPlane->xarrBendPlaneRods.Append(pLinePart);
						}
						else	//�뵱ǰ���ӵ㲻ƥ��
							continue;
					}
					blFindResideConnPointPlane=true;
					continue;	//break;�˴�break����©��ͬʱ����˫��ĸ˼�
				}
				else if (fabs(linevec.vLine*pConnPlane->datumplane.vLine)<=0.2)	//ctan(11��)��0.2
				{	//����������������ʱֻ��ͨ����׼�ڵ�ģ��������
					pConnPlane->xarrElsePlaneRods.Append(pLinePart);
					if (base_bolt_num>pConnPlane->wnMaxRayLsNum)
						pConnPlane->wnMaxRayLsNum=base_bolt_num;
					blFindResideConnPointPlane=true;
					continue;	//break;�˴�break����©��ͬʱ����˫��ĸ˼�
				}
				else	//�����Ȳ��ڻ�׼����ƽ�棬�ֲ��ڻ������ϵ����Ӹ˼�
					continue;
			}
			else if (fabs(linevec.vLine*pConnPlane->datumplane.vLine)<=0.01)
			{	//���ڴ��ڲ����˹����빲�����꣬��ʹ����ͼ���Ҳ���ܴ���EPS����ȡ0.01����0.57��) wjh-2016.1.11
				if (pDatumRod->GetClassTypeId()==CLS_LINETUBE)
				{
					if (pConnPlane->vDatumDirect.IsZero())
						pConnPlane->vDatumDirect = datumlinevec.vLine^pConnPlane->datumplane.vLine;
					if (pConnPlane->vDatumDirect*linevec.vLine>0&&pConnPlane->ciConnectWing==1)
						continue;	//�˼����Ҳ࣬��Ƶ����������
					else if (pConnPlane->vDatumDirect*linevec.vLine<0&&pConnPlane->ciConnectWing==0)
						continue;	//�˼�����࣬��Ƶ��������Ҳ�
				}
				if (base_bolt_num>pConnPlane->wnMaxRayLsNum)
					pConnPlane->wnMaxRayLsNum=base_bolt_num;
				pConnPlane->xarrBasePlaneRods.Append(pLinePart);
			}
			else
				continue;
			break;
		}
		if (!blFindResideConnPointPlane&&cbExistConnPlaneFlag<0x07)
		{
			int liPlaneIndex=0;
			CConnectPointInfo xConnPoint;
			InitConnectPointBasicInfo(&xConnPoint,this,pDatumRod,pLinePart);
			CONNECT_PLANE* pConnPlane=xConnNodeInfo.AddPlane(&liPlaneIndex);
			if (pConnPlane==NULL)
			{
				//logerr.Log("error");
				continue;
			}
			xConnNodeInfo.InitFromConnectPointInfo(&xConnPoint,liPlaneIndex);
			pConnPlane->datumplane=worknorm;
			if (pLinePart->pStart==this)
				pConnPlane->wnMaxRayLsNum=pLinePart->connectStart.wnConnBoltN;
			else
				pConnPlane->wnMaxRayLsNum=pLinePart->connectEnd.wnConnBoltN;
			if (pDatumRod->IsAngle())
			{	//��׼�˼�Ϊ�Ǹ�ʱ���жϻ�׼�Ǹ�����֫
				double ddx=worknorm.vSolid*pDatumRod.pAngle->vxWingNorm;
				double ddy=worknorm.vSolid*pDatumRod.pAngle->vyWingNorm;
				pDesPosPara=pLinePart->pStart==this?&pLinePart.pAngle->desStartPos:&pLinePart.pAngle->desEndPos;
				if ((fabs(ddx)>fabs(ddy)&&fabs(ddx)>EPS_COS2)||(pDesPosPara->IsInHuoQuLine(true)&&pDesPosPara->IsInDatumJgWingX()))
				{
					pConnPlane->ciConnectWing=0;	//���ӹ̽��ڻ�׼�Ǹ�X֫��
					cbExistConnPlaneFlag|=0x01;
					if (ddx<0)
						pConnPlane->datumplane.vSolid *= -1.0;
				}
				else if((fabs(ddx)<fabs(ddy)&&fabs(ddy)>EPS_COS2)||(pDesPosPara->IsInHuoQuLine(true)&&pDesPosPara->IsInDatumJgWingY()))
				{
					cbExistConnPlaneFlag|=0x02;
					pConnPlane->ciConnectWing=1;	//���ӹ̽��ڻ�׼�Ǹ�Y֫��
					if (ddy<0)
						pConnPlane->datumplane.vSolid *= -1.0;
				}
				else
				{
					cbExistConnPlaneFlag|=0x04;
					pConnPlane->ciConnectWing=2;	//����湲�ð�
					//linevec.v
				}
				if (pConnPlane->datumplane.vLine*pConnPlane->datumplane.vSolid<0)
					pConnPlane->datumplane.vLine*=-1.0;
			}
			else if (pDatumRod->GetClassTypeId()==CLS_LINETUBE)
			{
				if (pConnPlane->vDatumDirect.IsZero())
					pConnPlane->vDatumDirect=datumlinevec.vLine^worknorm.vLine;
				if (pConnPlane->vDatumDirect*linevec.vLine>0)
					pConnPlane->ciConnectWing=0;	//��׼�˼��Ҳ�
				else
					pConnPlane->ciConnectWing=1;	//��׼�˼����
			}
			else
				pConnPlane->ciConnectWing=2;
		}
	}
	for (i=0;i<xConnNodeInfo.xCommNode.planes.CountOfPreset;i++)
	{
		CONNECT_PLANE* pConnPlane=xConnNodeInfo.xCommNode.planes.GetAt(i);
		if (!pConnPlane->blValid)
			continue;
		if (pConnPlane->xarrBendPlaneRods.Count>0&&pConnPlane->hBendlineAngle==0)
			pConnPlane->hBendlineAngle=pDatumRod->handle;
	}
	//6�������ӵ����������������׼�˼���˨��������������֧�̶ֳȵȽ��г����ж����������ӷ���
	JGZJ jgzj;
	getjgzj(jgzj,pDatumRod->GetWidth());
	GEPOINT vLenToStart=pDatumRod->pStart->xOriginalPos-pos;
	GEPOINT vLenToEnd  =pDatumRod->pEnd->xOriginalPos-pos;
	BYTE ciBaseRodConnState=0;//0.��ʼδʶ��;1.�м������;2.�νӶ�ĩ������;3.�νӶνضϵ�����;4.���߽ضϵ�����
	if (vLenToStart*vLenToEnd<-EPS)
		ciBaseRodConnState=1;	//1.�м����ӵ�
	SmartRodPtr pRod;
	if (pCoupleLinePart!=NULL)
		ciBaseRodConnState=4;
	else if(ciBaseRodConnState==0)
	{
		GEPOINT vPrimaryVec=-vLenToStart;
		if (vPrimaryVec.IsZero())
			vPrimaryVec=-vLenToEnd;
		normalize(vPrimaryVec);	//ʵ����Ӧ��datumlinevecһ�£���datumlinevec��ǰ��ʱ���淶���԰�ȫ����ڴ����� wjh-2019.9.4
		for (pRod=linkPartSet.GetFirst();pRod.blValid;pRod=linkPartSet.GetNext())
		{
			GEPOINT vLenRayVec;
			if (pRod->pStart==this)
				vLenRayVec=pRod->pEnd->xOriginalPos-pos;
			else if (pRod->pEnd==this)
				vLenRayVec=pRod->pStart->xOriginalPos-pos;
			else
				continue;
			normalize(vLenRayVec);
			double justify=vPrimaryVec*vLenRayVec;
			if (justify>=0.5)
			{
				ciBaseRodConnState=3;	//3.�νӶνضϵ�����
				break;
			}
		}
		if (ciBaseRodConnState==0)
			ciBaseRodConnState=2;		//2.�νӶ�ĩ������
	}
	for (i=0;i<xConnNodeInfo.xCommNode.planes.Count;i++)
	{
		CONNECT_PLANE* pConnPlane=xConnNodeInfo.xCommNode.planes.GetAt(i);
		if (!pConnPlane->blValid||pConnPlane->designType.bIntersBolt)
			continue;	//����㵥��˨���
		xConnNodeInfo.xCommNode.ciBaseRodConnState=ciBaseRodConnState;
		if (pConnPlane->xarrBasePlaneRods.Count==1)
		{	//����������
			SmartRodPtr pRayRod=pConnPlane->xarrBasePlaneRods[0];
			if (pDatumRod->IsAngle()&&pRayRod->IsAngle())
			{
				if ((pRayRod->pStart->handle==handle&&pRayRod->connectStart.wnConnBoltN==1)
					||(pRayRod->pEnd->handle==handle&&pRayRod->connectEnd.wnConnBoltN==1))
					pConnPlane->designType.bL_1LB1 = TRUE;//����˨�ް�����
				else if (jgzj.g1>0&&&jgzj.g2>0&&(
					(pRayRod->pStart->handle==xConnNodeInfo.xCommNode.hBaseNode&&pRayRod->connectStart.wnConnBoltN==2)||
					(pRayRod->pEnd->handle==xConnNodeInfo.xCommNode.hBaseNode&&pRayRod->connectEnd.wnConnBoltN==2)))
					pConnPlane->designType.bL_1LB2 = TRUE;//˫��˨�ް�����
				if (pConnPlane->wnMaxRayLsNum>1&&pConnPlane->wnMaxRayLsNum<3)
					pConnPlane->designType.bL_1LE1 = TRUE;//����Ϊ������˨�Ķ�����
				else if (pConnPlane->wnMaxRayLsNum>=3&&jgzj.g1>0&&jgzj.g2>0)
					pConnPlane->designType.bL_1LE2 = TRUE;//����Ϊ˫����˨�Ķ�����
				//pConnectPoint->designType.bL_1LO1=TRUE;//�ǿ϶��Ĳ����ж�
			}
		}
		else if (pConnPlane->xarrBasePlaneRods.Count==2)
		{	//˫���߸˼�����
			SmartRodPtr pRayRodArr[2];
			pRayRodArr[0]=pConnPlane->xarrBasePlaneRods[0];
			pRayRodArr[1]=pConnPlane->xarrBasePlaneRods[1];
			if (pDatumRod->IsAngle()&&pRayRodArr[0]->IsAngle()&&pRayRodArr[1]->IsAngle())
			{
				CConnectInfo *pLsInfoArr[2]={ NULL,NULL };
				CMultiOffsetPos *pDesParaArr[2]={ NULL,NULL };
				for (int i=0;i<2;i++)
				{
					if (pRayRodArr[i]->pStart->handle==handle)
					{
						pLsInfoArr[i]=&pRayRodArr[i]->connectStart;
						pDesParaArr[i]=&pRayRodArr[i].LineAnglePointer()->desStartPos;
					}
					else if (pRayRodArr[i]->pEnd->handle==handle)
					{
						pLsInfoArr[i]=&pRayRodArr[i]->connectEnd;
						pDesParaArr[i]=&pRayRodArr[i].LineAnglePointer()->desEndPos;
					}
					else
						return false;//pConnectPointList;
				}
				//��ͬ�������߸˼�����˨����ڻ�׼�Ǹ�,��Ҫʱ�ɽ��������߽Ǹֶ�ͷ����ƫ�ƿ�һ����϶��
				if (pLsInfoArr[0]->wnConnBoltN<=1&&pLsInfoArr[1]->wnConnBoltN<=1)
					pConnPlane->designType.bL_2LB1 = TRUE;
				//����ͬ�������߸˼�����ڻ�׼�Ǹ�,����һ����˨
				if (pLsInfoArr[0]->wnConnBoltN<=1&&pLsInfoArr[1]->wnConnBoltN<=1)
					pConnPlane->designType.bL_2LB2 = TRUE;//����˨�ް�����
				else if (pLsInfoArr[0]->wnConnBoltN>=1&&pLsInfoArr[1]->wnConnBoltN>=1&&jgzj.g1>0&&jgzj.g2>0)
					pConnPlane->designType.bL_2LB3 = TRUE;//˫��˨�ް�����
				//��ͬ�������߸˼�˫��˨����ڻ�׼�Ǹ�,��Ҫʱ�ɽ��������߽Ǹֶ�ͷ����ƫ�ƿ�һ����϶��
				if (max(pLsInfoArr[0]->wnConnBoltN,pLsInfoArr[1]->wnConnBoltN)==2&&jgzj.g1>0&&jgzj.g2>0)
					pConnPlane->designType.bL_2LB4 = TRUE;//˫��˨�ް�����
				if ((pLsInfoArr[0]->wnConnBoltN>1||pLsInfoArr[1]->wnConnBoltN>1)&&(pLsInfoArr[0]->wnConnBoltN<=3||pLsInfoArr[1]->wnConnBoltN<=3))
				{
					pConnPlane->designType.bL_2LE1 = TRUE;//����Ϊ������˨�Ķ�����
					pConnPlane->designType.bL_2LC1 = TRUE;//����Ϊ������˨�Ļ������
				}
				/*else */if (jgzj.g1>0&&jgzj.g2>0&&//����5����˨ʱҲ�������Ȱ����Ŷ����Ӳ��� wjh-2012.5.29
					(pLsInfoArr[0]->wnConnBoltN>=2||pLsInfoArr[1]->wnConnBoltN>=2)/*&&(pLsInfoArr[0]->wnConnBoltN<5||pLsInfoArr[1]->wnConnBoltN<5)*/)
				{
					pConnPlane->designType.bL_2LE2 = TRUE;//����Ϊ˫����˨�Ķ�����
					pConnPlane->designType.bL_2LC2 = TRUE;//����Ϊ˫����˨�Ļ������
				}
			}
		}
		else
		{	//�����߸˼�����
		}
	}
	*pConnNodeInfo=xConnNodeInfo;
	return  true;
}
bool CLDSNode::RetrieveConnectRods(const double* vzWorkNorm, LINEPARTSET& linkPartSet, bool bIncBendConnect)
{
	GEPOINT vStdPlaneNormal(vzWorkNorm);
	if (vzWorkNorm == NULL || vStdPlaneNormal.IsZero())
		return false;
	CSuperSmartPtr<CLDSLinePart> pDatumRod = m_pModel->FromRodHandle(hFatherPart);
	if (pDatumRod==NULL || pDatumRod->pStart == NULL || pDatumRod->pEnd == NULL)
		return false;	//��׼�˼����Ϸ�
	if (!pDatumRod->IsAngle() && pDatumRod->GetClassTypeId() != CLS_LINETUBE)
		return false;	//��ʱֻ֧�ֻ�׼����Ϊ�Ǹ֣���ϽǸ֣��ֹܵ����
	CSuperSmartPtr<CLDSLinePart> pRod = NULL;
	linkPartSet.append(pDatumRod);
	if (m_cPosCalType == 4)
	{	
		pRod = m_pModel->FromRodHandle(arrRelationPole[0]);
		if (pRod && pRod != pDatumRod)
			linkPartSet.append(pRod);
		pRod = m_pModel->FromRodHandle(arrRelationPole[1]);
		if (pRod && pRod != pDatumRod)
			linkPartSet.append(pRod);
	}
	//����ȫ���������ڴ˽ڵ��ϵ����߸˼���������
	CLDSModule *pModule = NULL;
	if (m_pModel->IsTowerModel())
		pModule = ((CTower*)m_pModel)->GetActiveModule();
	BOOL pushed = m_pModel->PushPartStack();
	for (pRod = m_pModel->EnumRodFirst(); pRod.IsHasPtr(); pRod = m_pModel->EnumRodNext())
	{	
		if(pRod==pDatumRod)
			continue;
		if (!pRod->IsAngle() && pRod->GetClassTypeId() != CLS_LINETUBE)
			continue;	//��֧�ֽǸ֡��ֹ�
		if (pRod->GetClassTypeId() == CLS_LINEANGLE && pRod.pAngle->group_father_jg_h > 0x20)
			continue;	//��ϽǸֵ��ӽǸ�����ϽǸִ����������
		if (pRod->pStart != this && pRod->pEnd != this)
			continue;	//���κ�һ��˽ڵ�Ϊ��ǰ�ڵ�
		if (!pRod->ModuleTogetherWith(this) || (pModule && !pModule->IsSonPart(pRod)))
			continue;	//�ǵ�ǰ������߷�������
		linkPartSet.append(pRod);
	}
	m_pModel->PopPartStack(pushed);
	//�޳�δ�����ӹ������ϵĸ˼�
	GEPOINT datumlinevec = pDatumRod->pEnd->xOriginalPos - pDatumRod->pStart->xOriginalPos;
	normalize(datumlinevec);
	for (pRod = linkPartSet.GetFirst(); pRod.IsHasPtr(); pRod = linkPartSet.GetNext())
	{
		if (pRod == pDatumRod)
			continue;	//������׼�˼�
		if(pRod->pStart==NULL || pRod->pEnd==NULL)
			continue;	//
		GEPOINT line_vec;
		if (pRod->pStart->handle == this->handle)
			line_vec = pRod->pEnd->xOriginalPos - pRod->pStart->xOriginalPos;
		else
			line_vec = pRod->pStart->xOriginalPos - pRod->pEnd->xOriginalPos;
		normalize(line_vec);
		GEPOINT vNormal = line_vec ^ datumlinevec;
		normalize(vNormal);
		if (fabs(vNormal*vStdPlaneNormal) < 0.82)
			linkPartSet.DeleteCursor();
	}
	if (linkPartSet.GetNodeNum() > 1)
		return true;
	return false;
}
#endif
/*
CLDSNode *CLDSNode::MotherNode()
{
	if(m_pMotherNode==NULL||m_pMotherNode==this)
		return this;
	else
		return m_pMotherNode;
}

bool CLDSNode::ShadowDesign()
{
	return true;
}*/
