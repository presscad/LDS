// LDSDoc.cpp : implementation of the CLDSDoc class
//

#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "SortFunc.h"
#include "Expression.h"
#include "PreProcessor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __PART_DESIGN_INC_
//from AnalyzeProcessor.cpp
int compare_func1(const NODE_PTR& pNode1,const NODE_PTR& pNode2);
//int compare_func2(const ELEM_PTR& pElem1,const ELEM_PTR& pElem2);
//from lds_part.cpp
int compare_eleminfo(const ELEMINFO_PTR& pElemInfo1,const ELEMINFO_PTR& pElemInfo2);
// from Node.cpp
extern int MirPointI(int quad,MIRMSG msg);

struct TDF_NODE{
	long pointI;
	char layer[4];
	char mirstr[4];
	char coordcal_typestr[4];
	CXhChar16 coordpara_str[4];
	long restrict;
	CXhChar16 cfgstr;
	CLDSNode* pMapNode;
	CLDSPoint* pDatumPoint;
	TDF_NODE *pFatherNode,*pNodeMirX,*pNodeMirY,*pNodeMirZ;
	bool IsMirDerived(char cMirType=0)
	{
		if(cMirType=='X'&&pFatherNode&&stricmp(mirstr,"X")==0)
			return true;
		else if(cMirType=='Y'&&pFatherNode&&stricmp(mirstr,"Y")==0)
			return true;
		else if(cMirType=='Z'&&pFatherNode&&stricmp(mirstr,"Z")==0)
			return true;
		else if(cMirType==0&&pFatherNode)
			return true;
		else
			return false;
	}
	TDF_NODE(){memset(this,0,sizeof(TDF_NODE));}
};
struct TDF_DATUM_PLANE{
	long planeI;
	long pointI[3];
	char description[101];
	CLDSPlane* pMapPlane;
	TDF_DATUM_PLANE(){memset(this,0,sizeof(TDF_DATUM_PLANE));}
};
struct TDF_ELEM{
	long JS,JE;	//ʼ�ն˽ڵ��
	char mirstr[4];
	char layer[4];
	SIZESPEC size;
	char cMaterial;
	BYTE start_force_type,end_force_type;
	BYTE start_joint_type,end_joint_type;
	double hole_out;	//������
	bool m_bAutoCalHoleOutCoef;
	WORD wStatSizeId;	//ͳ�ı�ʶ
	CXhChar16 lamdaItems[4];//���֧�����ϸ���趨������һ����С�ᣬһ��ƽ���ᣬ����ͬʱ��ѹ
	CXhChar16 horiRodLabel;	//�����˼���ʶ��
	CXhChar16 uniteRodLabel;//�ϲ��˼���ʶ��
	TDF_ELEM(){memset(this,0,sizeof(TDF_ELEM));}
};
struct NODELOADINFO{
	double xForce;
	double yForce;
	double zForce;
	double permanentFz;
	CWorkStatus* pMapCase;
	NODELOADINFO(){memset(this,0,sizeof(NODELOADINFO));}
};
class TDF_NODELOAD{
public:
	long pointI;
	ARRAY_LIST<NODELOADINFO> nodeloadarr;
	TDF_NODELOAD(){pointI=0;nodeloadarr.Empty();}
	~TDF_NODELOAD(){};
};
struct TDF_WINDLOADSEG
{
	BYTE cType;
	BYTE cMirType;
	BYTE cCalStyle;
	double BetaZ;
	double CqiFront;
	double CqiSide;
	BOOL bAutoCalProfileCoef;
	double fProfileModeCoef;
	BOOL bOrginIsZero;
	f3dPoint origin;
	ARRAY_LIST<long> pontIArr;
	TDF_WINDLOADSEG(){memset(this,0,sizeof(TDF_WINDLOADSEG));}
};
void InitTdfNodePointI(TDF_NODE* pTdfNode,DWORD pointI)
{
	pTdfNode->pointI=pointI;
}
CFGWORD RetrieveCfgWordFromStr(char* cfgstr,bool bLegObj=false)
{
	CFGWORD cfgword;
	CXhChar50 cfg_str(cfgstr);	//��ֹ����ʱ�޸�cfgstr�����Գ����ַ�����ɷ��ʳ�ͻ wjh-2013.4.20
	int len=strlen(cfg_str);
	if(!bLegObj)	//�������
	{
		for(long bodyNo=FindAdaptNo(cfg_str,",","-");bodyNo>0;bodyNo=FindAdaptNo(NULL,",","-"))
		{
			if(bodyNo<=CFGLEG::MaxBodys())
				cfgword.SetBodyLegs(bodyNo);
		}
		return cfgword;
	}
	else if(cfg_str!=NULL&&len>0)
	{
		CXhChar16 body_str;
		char* leg_str=strchr(cfg_str,':');
		if(leg_str!=NULL)
		{	//����ȷ�ı���ŷ���
			*leg_str=0;
			leg_str+=1;
			body_str.Copy(cfg_str);
		}
		else
		{	//����ŷ����ڽ��Ⱥŷ���֮ǰ������һ��
			if(isdigit(cfg_str[0])==0)	//��һ���ַ���Ϊ��ĸ
				leg_str=&cfg_str[0];
			else
			{	//��һ���ַ�Ϊ���������
				body_str[0]=cfg_str[0];
				if(len>1&&isdigit(cfg_str[1])==0)
					leg_str=&cfg_str[1];//�ڶ����ַ�Ϊ������ĸ
				else if(len>2)
				{
					body_str[1]=cfg_str[1];
					leg_str=&cfg_str[2];//�ڶ����ַ�Ϊ������ĸ
				}
				else
					return cfgword;//�����ˣ�һ�㲻��ִ�е��ˡ�false;
			}
		}
		//��ȡ����������Ϣ
		for(int replace=0;replace<(int)strlen(leg_str);replace++)
		{	//�ַ�ת�������֣��Ƚ�����ʶ��
			leg_str[replace]=toupper(leg_str[replace]);
			if(leg_str[replace]>='A')
				leg_str[replace]-=16;//16='A'-'1';
		}
		BYTE legFlag=0,byteConstArr[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
		for(long legNo=FindAdaptNo(leg_str,",","-");legNo>0;legNo=FindAdaptNo(NULL,",","-"))
		{	//��ȡ���Ⱥŷ�Χ��Ϣ
			if(legNo>0&&legNo<=8)
				legFlag|=byteConstArr[legNo-1];
		}
		if(body_str.Length==0)
			body_str.Copy("1");	//ȱ�ٱ�������ݣ�Ĭ�ϰ�1�ű��崦��
		for(long bodyNo=FindAdaptNo(body_str,",","-");bodyNo>0;bodyNo=FindAdaptNo(NULL,",","-"))
		{	//��ȡ�������Ϣ
			if(bodyNo<=CFGLEG::MaxBodys())
				cfgword.SetBodyLegs(bodyNo,legFlag);
		}
	}
	return cfgword;
}
TDF_NODE* TransferMirRelaNode(char cMirType,TDF_NODE* pRelaTdfNode)
{
	if(pRelaTdfNode==NULL)
		return NULL;
	else if(cMirType=='X'&&pRelaTdfNode->pNodeMirX)
		pRelaTdfNode=pRelaTdfNode->pNodeMirX;
	else if(cMirType=='Y'&&pRelaTdfNode->pNodeMirY)
		pRelaTdfNode=pRelaTdfNode->pNodeMirY;
	else if(cMirType=='Z'&&pRelaTdfNode->pNodeMirZ)
		pRelaTdfNode=pRelaTdfNode->pNodeMirZ;
	else if(cMirType=='Z'&&pRelaTdfNode->pNodeMirX)
		pRelaTdfNode=pRelaTdfNode->pNodeMirX;
	else if(cMirType=='Z'&&pRelaTdfNode->pNodeMirY)
		pRelaTdfNode=pRelaTdfNode->pNodeMirY;
	return pRelaTdfNode;
}
TDF_NODE* TransferMirRelaNode(TDF_NODE* pSrcTdfNode,TDF_NODE* pRelaTdfNode)
{
	if(pRelaTdfNode==NULL)
		return NULL;
	else if(pSrcTdfNode->IsMirDerived('X'))
		pRelaTdfNode=TransferMirRelaNode('X',pRelaTdfNode);
	else if(pSrcTdfNode->IsMirDerived('Y'))
		pRelaTdfNode=TransferMirRelaNode('Y',pRelaTdfNode);
	else if(pSrcTdfNode->IsMirDerived('Z'))
		pRelaTdfNode=TransferMirRelaNode('Z',pRelaTdfNode);
	return pRelaTdfNode;
}
void LoadTdfElemLabel(TDF_ELEM* pElem,DWORD key1,DWORD key2)
{
	pElem->JS=key1;
	pElem->JE=key2;
};
void LoadTdfNodeI(TDF_NODELOAD* pLoadNode,DWORD key)
{
	pLoadNode->pointI=key;
};
static CLDSLinePart* CreateMirRod(CLDSLinePart* pRod,char cMirType,CHashList<TDF_NODE>& hashTdfNode)
{
	CLDSLinePart* pMirRod=NULL;
	f3dPoint posStart=pRod->pStart->Position(false);
	f3dPoint posEnd  =pRod->pEnd->Position(false);
	f3dPoint posMirStart,posMirEnd;
	if(cMirType=='X')
	{
		posMirStart.Set(posStart.x,-posStart.y,posStart.z);
		posMirEnd.Set(posEnd.x,-posEnd.y,posEnd.z);
	}
	else if(cMirType=='Y')
	{
		posMirStart.Set(-posStart.x,posStart.y,posStart.z);
		posMirEnd.Set(-posEnd.x,posEnd.y,posEnd.z);
	}
	else if(cMirType=='Z')
	{
		posMirStart.Set(-posStart.x,-posStart.y,posStart.z);
		posMirEnd.Set(-posEnd.x,-posEnd.y,posEnd.z);
	}
	if((posStart==posMirStart&&posEnd==posMirEnd)||(posStart==posMirEnd&&posEnd==posMirStart))
		return pRod;	//�ԶԳ�
	pMirRod=(CLDSLinePart*)Ta.Parts.append(CLS_LINEPART);
	pMirRod->CopyProperty(pRod);
	TDF_NODE* pTdfNode;
	TDF_NODE* pStartTdfNode=hashTdfNode.GetValue(pRod->pStart->point_i);
	TDF_NODE* pEndTdfNode=hashTdfNode.GetValue(pRod->pEnd->point_i);
	pTdfNode=TransferMirRelaNode(cMirType,pStartTdfNode);
	pMirRod->pStart = pTdfNode->pMapNode;
	pTdfNode=TransferMirRelaNode(cMirType,pEndTdfNode);
	pMirRod->pEnd = pTdfNode->pMapNode;
	if(cMirType=='X')
		CalMirAtomLayer(pRod->layer(),pMirRod->layer(),MIRMSG(1));
	else if(cMirType=='Y')
		CalMirAtomLayer(pRod->layer(),pMirRod->layer(),MIRMSG(2));
	else if(cMirType=='Z')
		CalMirAtomLayer(pRod->layer(),pMirRod->layer(),MIRMSG(3));
	pMirRod->SetStart(pMirRod->pStart->Position(false));
	pMirRod->SetEnd(pMirRod->pEnd->Position(false));
	return pMirRod;
}
//���޸�label����
static bool RetrieveElemNodeByLabel(char* label,CHashList<TDF_NODE>& hashTdfNode,long* hStart,long* hEnd)
{
	CXhChar50 labelCopy(label);
	CXhString split;
	TDF_NODE* pTdfNode=NULL;
	char* key=split.StrToKey(labelCopy,"-\n");
	if(key!=NULL)
	{
		pTdfNode=hashTdfNode.GetValue(atoi(key));
		if(pTdfNode&&pTdfNode->pMapNode&&hStart)
			*hStart=pTdfNode->pMapNode->handle;
	}
	else
		return false;
	key=split.StrToKey(NULL,"-\n");
	if(key!=NULL)
	{
		pTdfNode=hashTdfNode.GetValue(atoi(key));
		if(pTdfNode&&pTdfNode->pMapNode&&hStart)
			*hEnd=pTdfNode->pMapNode->handle;
	}
	else
		return false;
	return true;
}
void ImportTdfFile(LPCTSTR lpszPathName) 
{
	FILE *fp = fopen(lpszPathName,"rt");
	if(fp==NULL)
	{
		AfxMessageBox("�򲻿�ָ���ļ���");
		return;
	}
	//
	CXhChar100 lineText;
	enum SECTION_TYPE{SECT_NONE,DESIGN_CODE=1,LOAD_CODE,STEEL,BOLT,ANGLE_SIZE,TUBE_SIZE,BOLT_MATCHING_RULE,MODULE,
		DATUM_PLANES,NODES,MEMBERS,CASES,WIRE_LOAD,WIND_LOAD_PANEL};
	SECTION_TYPE sect_type=SECT_NONE;
	CHashList<TDF_NODE> hashTdfNode;
	CHashList<TDF_DATUM_PLANE> hashTdfPlane;
	CMapList<TDF_ELEM> hashTdfElem;
	CHashList<TDF_NODELOAD>	hashTdfNodeLoad;
	ATOM_LIST<TDF_WINDLOADSEG> TdfWindLoadSegList;
	hashTdfElem.LoadDefaultObjectInfo=LoadTdfElemLabel;
	hashTdfNode.LoadDefaultObjectInfo=InitTdfNodePointI;
	hashTdfNodeLoad.LoadDefaultObjectInfo=LoadTdfNodeI;
	TDF_NODELOAD* pTdfNodeLoad=NULL;
	CExpression expression;
	bool bInitNewSect=false;	//True:��ʼ��һ��;False:��ʾ������ǰ��
	try{
		for(int iRow=1;!feof(fp);iRow++){
			if(!fgets(lineText,100,fp))
				continue;
			if(lineText.Length==100)
			{
				AfxMessageBox(CXhChar50("��%d���ı���Ϣ����100���ַ�,���ʵ��ɾȥ�����ַ��ٵ���!",iRow));
				return;
			}
			lineText.Replace("��"," ");	//��ȫ�ǿո񻻳ɰ�ǿո��Ա�ͳһ����
			lineText.Replace("��","=");
			CXhChar100 copyLnText=lineText;
			char* splitter=" =\t\r\n";
			char* key=strtok(copyLnText,splitter);
			if(key==NULL||strlen(key)==0)
				continue;	//����
			if(stricmp(key,"SECTION")==0)
			{	//һ���½ڵĿ�ʼ
				key=strtok(NULL,splitter);
				if(key==NULL)
					continue;
				if(stricmp(key,"DESIGN-CODE")==0)
				{
					sect_type=DESIGN_CODE;
					key=strtok(NULL,splitter);
					//if(key&&(key[0]>='0'&&key[0]<='2'))
					//	Ta.m_iMemberSizeCheckCode=key[0]-'0';
					if(key)
						sscanf(key,"%X",&Ta.m_iMemberSizeCheckCode);
				}
				else if(stricmp(key,"LOAD-CODE")==0)
				{
					sect_type=LOAD_CODE;
					key=strtok(NULL,splitter);
					if(key&&(key[0]>='0'&&key[0]<='3'))
						Ta.m_iLoadExecuteCode=key[0]-'0';
				}
				else if(stricmp(key,"STEEL")==0)
					sect_type=STEEL;
				else if(stricmp(key,"BOLT")==0)
					sect_type=BOLT;
				else if(stricmp(key,"ANGLE-SIZE")==0)
					sect_type=ANGLE_SIZE;
				else if(stricmp(key,"TUBE-SIZE")==0)
					sect_type=TUBE_SIZE;
				else if(stricmp(key,"BOLT-MATCHING-RULE")==0)
					sect_type=BOLT_MATCHING_RULE;
				else if(stricmp(key,"MODULE")==0)
					sect_type=MODULE;
				else if(stricmp(key,"DATUM-PLANES")==0)
					sect_type=DATUM_PLANES;
				else if(stricmp(key,"NODES")==0)
					sect_type=NODES;
				else if(stricmp(key,"MEMBERS")==0)
					sect_type=MEMBERS;
				else if(stricmp(key,"CASES")==0)
					sect_type=CASES;
				else if(stricmp(key,"WIRE-LOAD")==0)
					sect_type=WIRE_LOAD;
				else if(stricmp(key,"WIND-LOAD-PANEL")==0)
					sect_type=WIND_LOAD_PANEL;
				else
					continue;
				bInitNewSect=true;
				continue;
			}
			CXhChar16 key16(key);
			if(sect_type==DESIGN_CODE)
			{
				if(stricmp(key,"SAFTEY_COEF")==0)
				{
					key=strtok(NULL,splitter);
					if(key!=NULL&&strlen(key)>0)
					{
						Ta.m_siSafetyGrade=3;
						Ta.m_fSafetyCoef=atof(key);
					}
				}
				else if(stricmp(key,"gamaG")==0)
				{
					key=strtok(NULL,splitter);
					if(key!=NULL&&strlen(key)>0)
						Ta.m_fGamaGForDesign=atof(key);
				}
				else if(stricmp(key,"gamaC")==0)
				{	//������ԣ��ϵ��
					CXhChar16 key16(strtok(NULL,splitter));
					key16.Replace(';',0);
					if(key16.Length>0)
						Ta.m_fLimitBearCoef=atof(key16);
				}
				else if(stricmp(key,"ANGLE_LAMDA_LIMIT_OF_LEG_MEMBER")==0)
				{
					key=strtok(NULL,splitter);
					if(key!=NULL&&strlen(key)>0)
						Ta.SelMatPara.m_nMainPoleSlendernessLimit=atoi(key);
				}
				else if(stricmp(key,"ANGLE_LAMDA_LIMIT_OF_COMP_MEMBER")==0)
				{
					key=strtok(NULL,splitter);
					if(key!=NULL&&strlen(key)>0)
						Ta.SelMatPara.m_nXiePoleSlendernessLimit=atoi(key);
				}
				else if(stricmp(key,"ANGLE_LAMDA_LIMIT_OF_BRACE_MEMBER")==0)
				{
					key=strtok(NULL,splitter);
					if(key!=NULL&&strlen(key)>0)
						Ta.SelMatPara.m_nAuxPoleSlendernessLimit=atoi(key);
				}
				else if(stricmp(key,"ANGLE_LAMDA_LIMIT_OF_TENS_MEMBER")==0)
				{
					key=strtok(NULL,splitter);
					if(key!=NULL&&strlen(key)>0)
						Ta.SelMatPara.m_nTensivePoleSlendernessLimit=atoi(key);
				}
				else if(stricmp(key,"TUBE_LAMDA_LIMIT_OF_LEG_MEMBER")==0)
				{
					key=strtok(NULL,splitter);
					if(key!=NULL&&strlen(key)>0)
						Ta.SelMatPara.m_nMainTubeSlendernessLimit=atoi(key);
				}
				else if(stricmp(key,"TUBE_LAMDA_LIMIT_OF_COMP_MEMBER")==0)
				{
					key=strtok(NULL,splitter);
					if(key!=NULL&&strlen(key)>0)
						Ta.SelMatPara.m_nXieTubeSlendernessLimit=atoi(key);
				}
				else if(stricmp(key,"TUBE_LAMDA_LIMIT_OF_BRACE_MEMBER")==0)
				{
					key=strtok(NULL,splitter);
					if(key!=NULL&&strlen(key)>0)
						Ta.SelMatPara.m_nAuxTubeSlendernessLimit=atoi(key);
				}
				else if(stricmp(key,"TUBE_LAMDA_LIMIT_OF_TENS_MEMBER")==0)
				{
					key=strtok(NULL,splitter);
					if(key!=NULL&&strlen(key)>0)
						Ta.SelMatPara.m_nTensiveTubeSlendernessLimit=atoi(key);
				}
				else if(stricmp(key,"LEG_MEMBER_MIN_SIZE")==0)
				{
					key=strtok(NULL,splitter);
					if(key!=NULL&&strlen(key)>0)
					{
						strncpy(Ta.SelMatPara.m_csMinMainAngleSize,key,9);
						Ta.SelMatPara.m_csMinMainAngleSize[9]=0;
					}
				}
				else if(stricmp(key,"FRAME_MEMBER_MIN_SIZE")==0)
				{
					key=strtok(NULL,splitter);
					if(key!=NULL&&strlen(key)>0)
					{
						strncpy(Ta.SelMatPara.m_csMinForceAngleSize,key,9);
						Ta.SelMatPara.m_csMinForceAngleSize[9]=0;
					}
				}
				else if(stricmp(key,"BRACE_MEMBER_MIN_SIZE")==0)
				{
					key=strtok(NULL,splitter);
					if(key!=NULL&&strlen(key)>0)
					{
						strncpy(Ta.SelMatPara.m_csMinAngleSize,key,9);
						Ta.SelMatPara.m_csMinAngleSize[9]=0;
					}
				}
			}
			else if(sect_type==LOAD_CODE)
			{
				if(stricmp(key,"LANDSCAPE")==0)
				{
					key=strtok(NULL,splitter);
					CString landscape(key);
					landscape.Remove('\'');
					if(landscape.GetLength()>0)
						Ta.m_cLandscape=landscape[0];
				}
				else if(stricmp(key,"ICE_DENSITY")==0)
				{
					CXhChar16 key16(strtok(NULL,splitter));
					key16.Replace(';',0);
					if(key16.Length>0)
						Ta.m_fIceDensity=atof(key16);
				}
			}
			else if(sect_type==STEEL)
			{
				if(bInitNewSect)
				{
					bInitNewSect=false;
					CSteelMatLibrary::SetCount(0);
				}

				CSteelMatLibrary::SetCount(CSteelMatLibrary::GetCount()+1);
				STEELMAT mat=CSteelMatLibrary::RecordAt(CSteelMatLibrary::GetCount()-1);
				if(key16.Length>0)
					strncpy(mat.mark,key16,7);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					mat.cBriefMark=key16[0];
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)	//����Ƚ��е�ǿ�ȷ������
					mat.t_lim[0]=atoi(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)	//����Ƚ��е�ǿ�ȷ������
					mat.t_lim[1]=atoi(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)	//����ǿ��
					mat.intensity[0]=atoi(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)	//����ǿ��
					mat.intensity[1]=atoi(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)	//����ǿ��
					mat.intensity[2]=atoi(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)	//���ǿ��
					mat.intensity[4]=atoi(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)	//���ǿ��
					mat.intensity[5]=atoi(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)	//���ǿ��
					mat.intensity[6]=atoi(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)	//�ױڳ�ѹǿ��
					mat.intensity[8]=atoi(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)	//�ױڳ�ѹǿ��
					mat.intensity[9]=atoi(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)	//�ױڳ�ѹǿ��
					mat.intensity[10]=atoi(key16);
				else
					continue;

				CSteelMatLibrary::SetRecordAt(CSteelMatLibrary::GetCount()-1,mat);
			}
			else if(sect_type==BOLT)
			{
				if(bInitNewSect)
				{
					bInitNewSect=false;
					LsTypeCount=1;
				}
				else
					LsTypeCount=min(LsTypeCount+1,5);
				if(key16.Length>0)
					strncpy(ls_table[LsTypeCount-1].grade,key16,4);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					ls_table[LsTypeCount-1].fv=atof(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					ls_table[LsTypeCount-1].f=atof(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					ls_table[LsTypeCount-1].fc=atof(key16);
				else
					continue;
			}
			else if(sect_type==ANGLE_SIZE)
			{
				if(bInitNewSect)
				{
					bInitNewSect=false;
					jgguige_N=1;
				}
				else
					jgguige_N=min(jgguige_N+1,150);
				JG_PARA_TYPE& angle=jgguige_table[jgguige_N-1];
				if(key16.Length>0)
				{
					double fSize=atof(key16);
					angle.wing_wide=(int)fSize;
					angle.wing_thick=ftoi((fSize-(int)fSize)*1000);
				}
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					angle.cType=key16[0];
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					angle.area=atof(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					angle.Ry0=atof(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					angle.Rx=atof(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					angle.r=atof(key16);
				else
					continue;
				angle.theroy_weight=0.001*ftoi(1000*CalSteelPlankWei(angle.area*1e5));
				angle.Ix=0.01*ftoi(100*angle.Rx*angle.Rx*angle.area);
				angle.Iy0=0.01*ftoi(100*angle.Ry0*angle.Ry0*angle.area);
			}
			else if(sect_type==TUBE_SIZE)
			{
				if(bInitNewSect)
				{
					bInitNewSect=false;
					tubeguige_N=1;
				}
				else
					tubeguige_N=min(tubeguige_N+1,200);
				TUBE_PARA_TYPE& tube=tubeguige_table[tubeguige_N-1];
				if(key16.Length>0)
				{
					double fSize=atof(key16);
					tube.D=(int)fSize;
					tube.thick=ftoi((fSize-tube.D)*1000);
				}
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					tube.area=atof(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					tube.r=atof(key16);
				else
					continue;
				tube.theroy_weight=0.001*ftoi(1000*CalSteelPlankWei(tube.area*1e5));
				//TODO:�Ժ�Ҫ�ָ���������ע�ͣ�����ֻ����Ϊ.Net�Ķ�̬��ʾ��Bug wjh-2013-3-29
				//tube.I=tube.r*tube.r*tube.area;
				//tube.W=20*tube.I/tube.D;	//tube.D ��λΪmm��Ӧת��Ϊcm
			}
			else if(sect_type==BOLT_MATCHING_RULE)
			{
				if(bInitNewSect)
				{
					Ta.listSelBoltCond.Empty();
					bInitNewSect=false;
				}
				BOLT_SELCOND cond;
				if(key16.Length>0)
					cond.upWidthLim=atoi(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					strncpy(cond.grade,key16,4);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					cond.d=atoi(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					strncpy(cond.matStr,key16,7);
				else
					continue;
				Ta.listSelBoltCond.append(cond);
			}
			else if(sect_type==MODULE)
			{
				CLDSModule* pModule=NULL;
				CObjNoGroup* pNoGroup=Ta.NoManager.FromGroupID(MODEL_GROUP);
				if(key!=NULL&&strlen(key)>0)
				{
					if(bInitNewSect)
					{
						Ta.Module.Empty();
						pNoGroup->EmptyNo();
					}
					bInitNewSect=false;
					pModule=Ta.Module.append();
					pModule->iNo=pNoGroup->EnumIdleNo();
					pNoGroup->SetNoState(pModule->iNo);
					strncpy(pModule->description,key,50);
					if(pModule->iNo==1)
						Ta.m_hActiveModule=pModule->handle;
				}
				else
					continue;
				key16.Copy(strtok(NULL," ,"));
				if(key16.Length>0)
					pModule->SetBodyNo(atoi(key16));
				else
					continue;
				//Ĭ�ϼ����һ�����ȣ���A��
				pModule->m_arrActiveQuadLegNo[0]=pModule->m_arrActiveQuadLegNo[1]=
					pModule->m_arrActiveQuadLegNo[2]=pModule->m_arrActiveQuadLegNo[3]=pModule->GetBodyNo()*8-7;
				CXhChar200 legStr(strtok(NULL,"'"));
				key16.Copy(strtok(NULL,", '"));
				if(legStr.Length>0)
				{
					pModule->m_dwLegCfgWord=RetrieveCfgWordFromStr(legStr,true);
					if(pModule->GetBodyNo()>1)
					{	//����ֻ�н��Ⱥŷ�Χ���ַ�����Ĭ�ϱ����Ϊ1����1ʱӦ��λ
						pModule->m_dwLegCfgWord.SetBodyLegs(pModule->GetBodyNo(),pModule->m_dwLegCfgWord.GetBodyLegs(1));
						pModule->m_dwLegCfgWord.flag.bytes[0]=0;
					}
				}
				else
					continue;
				//key16.Copy(strtok(NULL,", '"));
				if(key16.Length>0)
					pModule->land_high=atof(key16);
			}
			else if(sect_type==DATUM_PLANES)
			{
				if(bInitNewSect)
				{
					bInitNewSect=false;
					Ta.Plane.Empty();
					Ta.Point.Empty();
					hashTdfPlane.Empty();
				}
				TDF_DATUM_PLANE* pTdfPlane=hashTdfPlane.Add(atoi(key16));
				pTdfPlane->planeI=atoi(key16);
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					pTdfPlane->pointI[0]=atoi(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					pTdfPlane->pointI[1]=atoi(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					pTdfPlane->pointI[2]=atoi(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					strcpy(pTdfPlane->description,key16);
				else continue;
			}
			else if(sect_type==NODES)
			{
				if(bInitNewSect)
				{
					bInitNewSect=false;
					Ta.Node.Empty();
				}
				TDF_NODE* pTdfNode=hashTdfNode.Add(atoi(key16));
				key16.Copy(strtok(NULL,splitter));	//�Գ���Ϣ
				if(key16.Length>0)
					strncpy(pTdfNode->mirstr,key16,3);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));	//ͼ��
				if(key16.Length>0)
					strncpy(pTdfNode->layer,key16,3);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));	//���궨������
				if(key16.Length>0)
					strncpy(pTdfNode->coordcal_typestr,key16,3);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));	//���궨�����1
				if(key16.Length>0)
					pTdfNode->coordpara_str[0]=key16;
				else
					continue;
				key16.Copy(strtok(NULL,splitter));	//���궨�����2
				if(key16.Length>0)
					pTdfNode->coordpara_str[1]=key16;
				else
					continue;
				key16.Copy(strtok(NULL,splitter));	//���궨�����3
				if(key16.Length>0)
					pTdfNode->coordpara_str[2]=key16;
				else
					continue;
				if(stricmp(pTdfNode->coordcal_typestr,"I")==0||strstr(pTdfNode->coordcal_typestr,"Mv")!=NULL)	//�����
				{
					key16.Copy(strtok(NULL,splitter));	//���궨�����4
					if(key16.Length>0)
						pTdfNode->coordpara_str[3]=key16;
					else
						continue;
				}
				key16.Copy(strtok(NULL,splitter));	//Լ��״̬
				if(key16.Length>0)
					pTdfNode->restrict=atoi(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));	//��ĺ�
				if(key16.Length>0)
					pTdfNode->cfgstr=key16;
				else
					continue;
				//���ݶԳƹ�ϵ�����ڵ�
				TDF_NODE* pMirTdfNode;
				pTdfNode->pFatherNode=NULL;
				if(strchr(pTdfNode->mirstr,'X')!=NULL)		//X��Գ�
				{
					int nPointI = MirPointI(pTdfNode->pointI,MIRMSG(1));
					pMirTdfNode=hashTdfNode.GetValue(nPointI);
					if(pMirTdfNode==NULL)
						pMirTdfNode=hashTdfNode.Add(nPointI);
					*pMirTdfNode=*pTdfNode;
					pMirTdfNode->pointI=nPointI;
					pMirTdfNode->pFatherNode=pTdfNode;
					pTdfNode->pNodeMirX = pMirTdfNode;
					pMirTdfNode->pNodeMirX=pTdfNode;
					strcpy(pMirTdfNode->mirstr,"X");
					CalMirAtomLayer(pTdfNode->layer,pMirTdfNode->layer,MIRMSG(1));
				}
				if(strchr(pTdfNode->mirstr,'Y')!=NULL)	//Y��Գ�
				{
					int nPointI = MirPointI(pTdfNode->pointI,MIRMSG(2));
					pMirTdfNode=hashTdfNode.GetValue(nPointI);
					if(pMirTdfNode==NULL)
						pMirTdfNode=hashTdfNode.Add(nPointI);
					*pMirTdfNode=*pTdfNode;
					pMirTdfNode->pointI=nPointI;
					pMirTdfNode->pFatherNode=pTdfNode;
					pTdfNode->pNodeMirY = pMirTdfNode;
					pMirTdfNode->pNodeMirY=pTdfNode;
					strcpy(pMirTdfNode->mirstr,"Y");
					CalMirAtomLayer(pTdfNode->layer,pMirTdfNode->layer,MIRMSG(2));
				}
				if(strchr(pTdfNode->mirstr,'Z')!=NULL)	//Z��Գ�
				{
					int nPointI = MirPointI(pTdfNode->pointI,MIRMSG(3));
					pMirTdfNode=hashTdfNode.GetValue(nPointI);
					if(pMirTdfNode==NULL)
						pMirTdfNode=hashTdfNode.Add(nPointI);
					*pMirTdfNode=*pTdfNode;
					pMirTdfNode->pointI=nPointI;
					pMirTdfNode->pFatherNode=pTdfNode;
					pTdfNode->pNodeMirZ = pMirTdfNode;
					pMirTdfNode->pNodeMirZ=pTdfNode;
					strcpy(pMirTdfNode->mirstr,"Z");
					CalMirAtomLayer(pTdfNode->layer,pMirTdfNode->layer,MIRMSG(3));
				}
				if(pTdfNode->pNodeMirX)
				{
					pTdfNode->pNodeMirX->pNodeMirY=pTdfNode->pNodeMirZ;
					pTdfNode->pNodeMirX->pNodeMirZ=pTdfNode->pNodeMirY;
				}
				if(pTdfNode->pNodeMirY)
				{
					pTdfNode->pNodeMirY->pNodeMirX=pTdfNode->pNodeMirZ;
					pTdfNode->pNodeMirY->pNodeMirZ=pTdfNode->pNodeMirX;
				}
				if(pTdfNode->pNodeMirZ)
				{
					pTdfNode->pNodeMirZ->pNodeMirX=pTdfNode->pNodeMirY;
					pTdfNode->pNodeMirZ->pNodeMirY=pTdfNode->pNodeMirX;
				}
			}
			else if(sect_type==MEMBERS)
			{
				if(bInitNewSect)
				{
					bInitNewSect=false;
					Ta.Elems.Empty();
					Ta.Parts.Empty();
				}
				//1.��ȡ��Ԫ��ʶ
				CXhString label;
				char* labelStart=label.StrToKey(key16,"-");
				char* labelEnd=label.StrToKey(NULL,"-");
				TDF_ELEM* pTdfElem=NULL;
				if(labelStart!=NULL&&labelEnd!=NULL)
					pTdfElem=hashTdfElem.Add(atoi(labelStart),atoi(labelEnd));
				else
					continue;
				//2.��ȡ�Գ���Ϣ
				key16.Copy(strtok(NULL,splitter));	//�Գ���Ϣ�����ɹ����ڵ㣬��������Ϣ
				if(key16.Length>0)
					strncpy(pTdfElem->mirstr,key16,3);
				else
					continue;
				//3.��ȡͼ����Ϣ
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					strncpy(pTdfElem->layer,key16,3);
				else
					continue;
				//4.��ȡ��������Ϣ
				key16.Copy(strtok(NULL,splitter));	//������
				if(key16.Length>0)
				{
					char prefix[3]={0};
					prefix[0]=key16[0];
					if(isdigit(key16[1])==0)	//���ַ�(��������)
						prefix[1]=key16[1];
					if(stricmp(prefix,"G")==0)
						pTdfElem->size.idClassType=CLS_LINETUBE;
					else if(stricmp(prefix,"B")==0)
						pTdfElem->size.idClassType=CLS_LINEFLAT;
					else if(stricmp(prefix,"L")==0)
					{
						pTdfElem->size.idClassType=CLS_LINEANGLE;
						pTdfElem->size.cSubClassType='L';
					}
					else if(stricmp(prefix,"DL")==0||stricmp(prefix,"XL")==0||stricmp(prefix,"TL")==0)
					{
						pTdfElem->size.idClassType=CLS_GROUPLINEANGLE;
						pTdfElem->size.cSubClassType=prefix[0];
					}
					else
					{
						AfxMessageBox("��Ԫ����������󣬲���ʶ��!");
						continue;
					}
					CXhString spec;
					if(prefix[1]==0)
						spec.AttachStringBuffer(&key16[1],16);
					else
						spec.AttachStringBuffer(&key16[2],15);
					if(isdigit(spec[spec.Length-1])==0)
					{	//���ַ�(��������)
						pTdfElem->cMaterial=spec[spec.Length-1];
						spec[spec.Length-1]=0;
					}
					else
						pTdfElem->cMaterial=CSteelMatLibrary::Q235BriefMark();
					char* paraStr=spec.StrToKey(spec,"*Xx");
					if(paraStr==NULL)
						continue;
					else
						pTdfElem->size.wide=atoi(paraStr);
					paraStr=spec.StrToKey(NULL,"*Xx");
					if(paraStr==NULL)
						continue;
					else
						pTdfElem->size.thick=atoi(paraStr);
					paraStr=spec.StrToKey(NULL,"*Xx");
					if(paraStr!=NULL)
						pTdfElem->size.height=atoi(paraStr);
				}
				else
					continue;
				//5.��ȡ�˶�״̬��Ϣ
				key16.Copy(strtok(NULL,splitter));	//����״̬1
				if(key16.Length>0)
				{
					key16.ResizeLength(3,' ',true);
					if(key16[0]==' ')	//0.�½�;1.�ս�;2.ǿ�ƽ½�
						pTdfElem->start_joint_type=JOINT_AUTOJUSTIFY;
					else if(key16[0]=='1')
						pTdfElem->start_joint_type=JOINT_RIGID;
					else //if(key16[0]=='2')
						pTdfElem->start_joint_type=JOINT_HINGE;
					if(key16[1]==' '||key16[1]=='0')	//0.�½�;1.�ս�;2.ǿ�ƽ½�
						pTdfElem->end_joint_type=JOINT_AUTOJUSTIFY;
					else if(key16[1]=='1')
						pTdfElem->end_joint_type=JOINT_RIGID;
					else //if(key16[1]=='2')
						pTdfElem->end_joint_type=JOINT_HINGE;
					if(key16[2]=='0')
					{
						pTdfElem->start_force_type=ECCENTRIC_FORCE;
						pTdfElem->end_force_type=ECCENTRIC_FORCE;
					}
					else if(key16[2]=='1')
					{
						pTdfElem->start_force_type=CENTRIC_FORCE;
						pTdfElem->end_force_type=ECCENTRIC_FORCE;
					}
					else if(key16[2]=='2')
					{
						pTdfElem->start_force_type=ECCENTRIC_FORCE;
						pTdfElem->end_force_type=CENTRIC_FORCE;
					}
					else// if(key16[2]=='3')
					{
						pTdfElem->start_force_type=CENTRIC_FORCE;
						pTdfElem->end_force_type=CENTRIC_FORCE;
					}
				}
				else
					continue;
				//6.������Ϣ
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
				{
					if(isdigit(key16[0])!=0||key16[0]=='.')
					{	//���ַ������֣�Ĭ���Զ����¼��������
						pTdfElem->m_bAutoCalHoleOutCoef=true;
						pTdfElem->hole_out=atof(key16);
					}
					else //if(key16[0]=='S')
					{	//ָ��������
						pTdfElem->m_bAutoCalHoleOutCoef=false;
						pTdfElem->hole_out=atof(&key16[1]);
					}
				}
				else
					continue;
				//7.��ϸ�ȼ�ͳ����Ϣ
				key16.Copy(strtok(NULL," \r\n"));
				char* mark=NULL;
				int lamda_i=0;
				while(key16.Length>0)
				{
					if(key16[0]=='H')	//������
						pTdfElem->horiRodLabel.Copy(&key16[1]);
					else if((mark=strchr(key16,'U'))!=NULL)
					{	//ͳ��
						pTdfElem->uniteRodLabel.Copy(mark+1);	//�ϲ��˼���ʶ
						*mark=0;
						if(key16.Length>0)	//ͳ�ı��
							pTdfElem->wStatSizeId=atoi(key16);
					}
					else
					{
						if(lamda_i<4)
							pTdfElem->lamdaItems[lamda_i]=key16;
						lamda_i++;
						/*char cBothCompType=0;
						if((mark=strstr(key16,"Rx"))==(char*)key16)
						{
						lamdaItems
						}
						else if((mark=strstr(key16,"Ry"))==(char*)key16)
						{
						}
						else if((mark=strstr(key16,"Rc"))==(char*)key16)
						{
						cBothCompType=*(mark+2);
						mark+=3;
						}*/
					}
					key16.Copy(strtok(NULL," \r\n"));
				}
			}
			else if(sect_type==CASES)
			{
				CObjNoGroup* pNoGroup=Ta.NoManager.FromGroupID(WORKSTATUS_GROUP);
				if(bInitNewSect)
				{
					bInitNewSect=false;
					Ta.WorkStatus.Empty();
					pNoGroup->EmptyNo();
				}
				CWorkStatus* pWorkStatus=Ta.WorkStatus.append();
				pWorkStatus->iNo=pNoGroup->EnumIdleNo();
				pNoGroup->SetNoState(pWorkStatus->iNo);
				if(key16.Length>0)
					pWorkStatus->WindLoad.velocity=atof(key16);
				else 
					continue;
				key16.Copy(strtok(NULL,splitter));	
				if(key16.Length>0)
					pWorkStatus->WindLoad.azimuth=atol(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					pWorkStatus->m_cMainLoadType=atoi(key16);
				else
					continue;
				if(pWorkStatus->m_cMainLoadType>=5)
				{
					key16.Copy(strtok(NULL,splitter));
					if(key16.Length>0)
					{
						if(strchr(key16,'X'))
							pWorkStatus->m_cHoriEarthQuakeAxis=0;
						else if(strchr(key16,'Y'))
							pWorkStatus->m_cHoriEarthQuakeAxis=90;
						else
							pWorkStatus->m_cHoriEarthQuakeAxis=atoi(key16);
					}
					else
						continue;
				}
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
				{
					key=strchr(key16,'K');
					if(key)
						key+=1;
					pWorkStatus->SetKesai(atof(key));
				}
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
				{
					key=strchr(key16,'G');
					if(key)
						key+=1;
					pWorkStatus->SetGamaQ(atof(key));
				}
				else 
					continue;
				CXhChar50 key50(strtok(NULL,splitter));
				if(key50.Length>0)
				{
					key=strchr(key50,';');
					if(key)
						key+=1;
					strcpy(pWorkStatus->description,key);
				}
				else
					continue;
			}
			else if(sect_type==WIRE_LOAD)
			{
				if(bInitNewSect)
				{
					bInitNewSect=false;
					hashTdfNodeLoad.Empty();			
				}
				if((key=strchr(key16,'P'))!=NULL)
				{
					key+=1;
					pTdfNodeLoad=hashTdfNodeLoad.Add(atoi(key));
					pTdfNodeLoad->pointI=atoi(key);
					pTdfNodeLoad->nodeloadarr.SetSize(0,Ta.WorkStatus.GetNodeNum());
					key16.Copy(strtok(NULL,splitter));
				}
				else if(pTdfNodeLoad==NULL)
					continue;	//TODO: error
				NODELOADINFO nodeLoadInfo;
				if(key16.Length>0)
					nodeLoadInfo.xForce=atof(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					nodeLoadInfo.yForce=atof(key16);
				else 
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					nodeLoadInfo.zForce=atof(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					nodeLoadInfo.permanentFz=atof(key16);
				else
					continue;
				pTdfNodeLoad->nodeloadarr.append(nodeLoadInfo);
			}
			else if(sect_type==WIND_LOAD_PANEL)
			{
				if(bInitNewSect)
				{
					bInitNewSect=false;
					Ta.WindLoadSeg.Empty();
					TdfWindLoadSegList.Empty();
				}
				TDF_WINDLOADSEG* pTdfWindLoad=TdfWindLoadSegList.append();
				if(key16.Length>0)
				{
					if(key16[0]=='0')
						pTdfWindLoad->cType=0;
					else if(key16[0]=='P')
						pTdfWindLoad->cType=1;
					else if(key16[0]=='T')
						pTdfWindLoad->cType=2;
					pTdfWindLoad->cMirType=key16[1]-'0';
					if(key16[2]=='0')
						pTdfWindLoad->cCalStyle=0;
					else if(key16[2]=='X')
						pTdfWindLoad->cCalStyle=1;
					else if(key16[2]=='Y')
						pTdfWindLoad->cCalStyle=2;			
				}
				else 
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					pTdfWindLoad->BetaZ=atof(key16);
				else 
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					pTdfWindLoad->CqiFront=atof(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
					pTdfWindLoad->CqiSide=atof(key16);
				else
					continue;
				key16.Copy(strtok(NULL,splitter));
				if(key16.Length>0)
				{
					if(strstr(key16,"MuS"))
					{
						strcpy(key,key16);
						key+=3;
						pTdfWindLoad->bAutoCalProfileCoef=0;
						pTdfWindLoad->fProfileModeCoef=atof(key);
						key16.Copy(strtok(NULL,splitter));
					}
					else
						pTdfWindLoad->bAutoCalProfileCoef=1;
					if(strstr(key16,"ORG"))
					{
						key16.Replace("(","");
						key16.Replace(")","");
						strcpy(key,key16);
						key+=3;
						int i=0;
						double fPos[3]={0};
						CXhString coordStr;
						for(char* posStr=coordStr.StrToKey(key,",");posStr;posStr=coordStr.StrToKey(NULL,","))
							fPos[i++]=atof(posStr);
						pTdfWindLoad->origin.Set(fPos[0],fPos[1],fPos[2]);
						pTdfWindLoad->bOrginIsZero=0;
						key16.Copy(strtok(NULL,splitter));
					}
					else
						pTdfWindLoad->bOrginIsZero=1;
					pTdfWindLoad->pontIArr.SetSize(0,8);
					pTdfWindLoad->pontIArr.append(atol(key16));
					key16.Copy(strtok(NULL,splitter));
					while(key16.Length>0 && stricmp(key,key16)!=0)
					{
						strcpy(key,key16);
						pTdfWindLoad->pontIArr.append(atol(key));
						key16.Copy(strtok(NULL,splitter));
					}
				}
				else
					continue;
			}
		}
		TDF_NODE *pTdfNode=NULL,*pRelaTdfNode=NULL;
		//����CLDSNodeԭ�Ͷ����Ի�ȡ�����վ��
		for(pTdfNode=hashTdfNode.GetFirst();pTdfNode;pTdfNode=hashTdfNode.GetNext())
		{
			if(pTdfNode==NULL)
				continue;
			CLDSNode* pNode=Ta.Node.append();
			pTdfNode->pMapNode=pNode;
			pNode->point_i=pTdfNode->pointI;	//�ڵ���
			pNode->SetLayer(pTdfNode->layer);	//ͼ��
		}
		//����CLDSPlane, �����»�׼�涨�����������Ļ�׼������ڵ�
		for(TDF_DATUM_PLANE* pTdfPlane=hashTdfPlane.GetFirst();pTdfPlane;pTdfPlane=hashTdfPlane.GetNext())
		{
			if(pTdfPlane==NULL)
				continue;
			CLDSPlane* pPlane=Ta.Plane.append();
			pTdfPlane->pMapPlane=pPlane;
			pPlane->iNo=pTdfPlane->planeI;
			strcpy(pPlane->description,pTdfPlane->description);
			for(int i=0;i<3;i++)
			{
				pTdfNode=hashTdfNode.GetValue(pTdfPlane->pointI[i]);
				if(pTdfNode)
				{
					if(pTdfNode->pDatumPoint==NULL)
					{
						CLDSPoint* pPoint=Ta.Point.append();
						pPoint->datum_pos_style=8;
						pPoint->des_para.hNode=pTdfNode->pMapNode->handle;
						pTdfNode->pDatumPoint=pPoint;
					}
					pPlane->hPointArr[i]=pTdfNode->pDatumPoint->handle;	
				}
			}		
		}
		//TDF_NODEǨ�ƽڵ�ԭʼ������Ϣ��CLDSNode��
		for(pTdfNode=hashTdfNode.GetFirst();pTdfNode;pTdfNode=hashTdfNode.GetNext())
		{
			if(pTdfNode==NULL)
				continue;
			//�����ڵ�
			int sign_x=1,sign_y=1;
			if(pTdfNode->pFatherNode!=NULL)
			{	//�����ӽڵ�
				if(stricmp(pTdfNode->mirstr,"X")==0)
				{
					if(pTdfNode->coordcal_typestr[0]=='P')
						strcat(pTdfNode->coordcal_typestr,"X");
					sign_y=-1;
				}
				else if(stricmp(pTdfNode->mirstr,"Y")==0)
				{
					if(pTdfNode->coordcal_typestr[0]=='P')
						strcat(pTdfNode->coordcal_typestr,"Y");
					sign_x=-1;
				}
				else if(stricmp(pTdfNode->mirstr,"Z")==0)
				{
					if(pTdfNode->coordcal_typestr[0]=='P')
						strcat(pTdfNode->coordcal_typestr,"Z");
					sign_x=sign_y=-1;
				}
			}
			//��ӶԳƹ����Գ�
			RELATIVE_OBJECT *pRelaObj;
			if(pTdfNode->pNodeMirX&&pTdfNode->pNodeMirX->pMapNode)
			{
				pRelaObj=pTdfNode->pMapNode->relativeObjs.Add(pTdfNode->pNodeMirX->pMapNode->handle);
				pRelaObj->mirInfo.axis_flag=0x01;
			}
			if(pTdfNode->pNodeMirY&&pTdfNode->pNodeMirY->pMapNode)
			{
				pRelaObj=pTdfNode->pMapNode->relativeObjs.Add(pTdfNode->pNodeMirY->pMapNode->handle);
				pRelaObj->mirInfo.axis_flag=0x02;
			}
			if(pTdfNode->pNodeMirZ&&pTdfNode->pNodeMirZ->pMapNode)
			{
				pRelaObj=pTdfNode->pMapNode->relativeObjs.Add(pTdfNode->pNodeMirZ->pMapNode->handle);
				pRelaObj->mirInfo.axis_flag=0x04;
			}

			//����������ͺͲ���
			if(stricmp(pTdfNode->coordcal_typestr,"0")==NULL)	//���κ������ڵ�
			{
				pTdfNode->pMapNode->m_cPosCalType=0;
				pTdfNode->pMapNode->SetPositionX(sign_x*atof(pTdfNode->coordpara_str[0])*1000);
				pTdfNode->pMapNode->SetPositionY(sign_y*atof(pTdfNode->coordpara_str[1])*1000);
				pTdfNode->pMapNode->SetPositionZ(atof(pTdfNode->coordpara_str[2])*1000);
			}
			else if(stricmp(pTdfNode->coordcal_typestr,"X")==0)		//�˼���X���겻���
			{
				pTdfNode->pMapNode->m_cPosCalType=1;
				pTdfNode->pMapNode->SetPositionX(sign_x*atof(pTdfNode->coordpara_str[0])*1000);
				pRelaTdfNode=hashTdfNode.GetValue(atoi(pTdfNode->coordpara_str[1]));
				pRelaTdfNode=TransferMirRelaNode(pTdfNode,pRelaTdfNode);
				if(pRelaTdfNode)
					pTdfNode->pMapNode->arrRelationNode[0]=pRelaTdfNode->pMapNode->handle;
				pRelaTdfNode=hashTdfNode.GetValue(atoi(pTdfNode->coordpara_str[2]));
				pRelaTdfNode=TransferMirRelaNode(pTdfNode,pRelaTdfNode);
				if(pRelaTdfNode)
					pTdfNode->pMapNode->arrRelationNode[1]=pRelaTdfNode->pMapNode->handle;
			}
			else if(stricmp(pTdfNode->coordcal_typestr,"Y")==0)		//�˼���Y���겻���
			{
				pTdfNode->pMapNode->m_cPosCalType=2;
				pTdfNode->pMapNode->SetPositionY(sign_y*atof(pTdfNode->coordpara_str[1])*1000);
				pRelaTdfNode=hashTdfNode.GetValue(atoi(pTdfNode->coordpara_str[0]));
				pRelaTdfNode=TransferMirRelaNode(pTdfNode,pRelaTdfNode);
				if(pRelaTdfNode)
					pTdfNode->pMapNode->arrRelationNode[0]=pRelaTdfNode->pMapNode->handle;
				pRelaTdfNode=hashTdfNode.GetValue(atoi(pTdfNode->coordpara_str[2]));
				pRelaTdfNode=TransferMirRelaNode(pTdfNode,pRelaTdfNode);
				if(pRelaTdfNode)
					pTdfNode->pMapNode->arrRelationNode[1]=pRelaTdfNode->pMapNode->handle;
			}
			else if(stricmp(pTdfNode->coordcal_typestr,"Z")==0)		//�˼���Z���겻���
			{
				pTdfNode->pMapNode->m_cPosCalType=3;
				pTdfNode->pMapNode->SetPositionZ(atof(pTdfNode->coordpara_str[2])*1000);
				pRelaTdfNode=hashTdfNode.GetValue(atoi(pTdfNode->coordpara_str[0]));
				pRelaTdfNode=TransferMirRelaNode(pTdfNode,pRelaTdfNode);
				if(pRelaTdfNode)
					pTdfNode->pMapNode->arrRelationNode[0]=pRelaTdfNode->pMapNode->handle;
				pRelaTdfNode=hashTdfNode.GetValue(atoi(pTdfNode->coordpara_str[1]));
				pRelaTdfNode=TransferMirRelaNode(pTdfNode,pRelaTdfNode);
				if(pRelaTdfNode)
					pTdfNode->pMapNode->arrRelationNode[1]=pRelaTdfNode->pMapNode->handle;
			}
			else if(stricmp(pTdfNode->coordcal_typestr,"I")==0)		//4.�����
			{
				pTdfNode->pMapNode->m_cPosCalType=4;
				for(int i=0;i<4;i++)
				{
					pRelaTdfNode=hashTdfNode.GetValue(atoi(pTdfNode->coordpara_str[i]));
					pRelaTdfNode=TransferMirRelaNode(pTdfNode,pRelaTdfNode);
					if(pRelaTdfNode)
						pTdfNode->pMapNode->arrRelationNode[i]=pRelaTdfNode->pMapNode->handle;
				}
			}
			else if(stricmp(pTdfNode->coordcal_typestr,"DIV")==0)	//5.�����ȷֵ�
			{
				pTdfNode->pMapNode->m_cPosCalType=5;
				for(int i=0;i<2;i++)
				{
					pRelaTdfNode=hashTdfNode.GetValue(atoi(pTdfNode->coordpara_str[i]));
					pRelaTdfNode=TransferMirRelaNode(pTdfNode,pRelaTdfNode);
					if(pRelaTdfNode)
						pTdfNode->pMapNode->arrRelationNode[i]=pRelaTdfNode->pMapNode->handle;
				}
				CExpression expression;
				pTdfNode->pMapNode->attach_scale=expression.SolveExpression(pTdfNode->coordpara_str[2]);
			}
			else if(strstr(pTdfNode->coordcal_typestr,"Mv"))		//7.ƫ�Ƶ�
			{
				pTdfNode->pMapNode->m_cPosCalType=7;
				if(pTdfNode->coordcal_typestr[2]>='X'&&pTdfNode->coordcal_typestr[2]<='Z')
					pTdfNode->pMapNode->m_cPosCalViceType=pTdfNode->coordcal_typestr[2];
				for(int i=0;i<3;i++)
				{
					pRelaTdfNode=hashTdfNode.GetValue(atoi(pTdfNode->coordpara_str[i]));
					pRelaTdfNode=TransferMirRelaNode(pTdfNode,pRelaTdfNode);
					if(pRelaTdfNode)
						pTdfNode->pMapNode->arrRelationNode[i]=pRelaTdfNode->pMapNode->handle;
				}
				pTdfNode->pMapNode->attach_offset=1000*atof(pTdfNode->coordpara_str[3]);
			}
			else if(strstr(pTdfNode->coordcal_typestr,"PZ"))	//8.��׼����XY����ֵ�����
			{
				pTdfNode->pMapNode->m_cPosCalType=8;
				if(pTdfNode->coordcal_typestr[2]>='X')
					pTdfNode->pMapNode->m_cPosCalViceType=pTdfNode->coordcal_typestr[2];
				pTdfNode->pMapNode->SetPositionX(sign_x*atof(pTdfNode->coordpara_str[0])*1000);
				pTdfNode->pMapNode->SetPositionY(sign_y*atof(pTdfNode->coordpara_str[1])*1000);
				TDF_DATUM_PLANE* pRelaTDFPlane=hashTdfPlane.GetValue(atoi(pTdfNode->coordpara_str[2]));
				if(pRelaTDFPlane)
					pTdfNode->pMapNode->hRelationPlane=pRelaTDFPlane->pMapPlane->handle;
			}
			else if(strstr(pTdfNode->coordcal_typestr,"PX"))	//9.��׼����YZ����ֵ�����
			{
				pTdfNode->pMapNode->m_cPosCalType=9;
				if(pTdfNode->coordcal_typestr[2]>='X')
					pTdfNode->pMapNode->m_cPosCalViceType=pTdfNode->coordcal_typestr[2];
				pTdfNode->pMapNode->SetPositionY(sign_y*atof(pTdfNode->coordpara_str[1])*1000);
				pTdfNode->pMapNode->SetPositionZ(atof(pTdfNode->coordpara_str[2])*1000);
				TDF_DATUM_PLANE* pRelaTDFPlane=hashTdfPlane.GetValue(atoi(pTdfNode->coordpara_str[0]));
				if(pRelaTDFPlane)
					pTdfNode->pMapNode->hRelationPlane=pRelaTDFPlane->pMapPlane->handle;
			}
			else if(strstr(pTdfNode->coordcal_typestr,"PY"))	//10.��׼����XZ����ֵ�����
			{
				pTdfNode->pMapNode->m_cPosCalType=10;
				if(pTdfNode->coordcal_typestr[2]>='X')
					pTdfNode->pMapNode->m_cPosCalViceType=pTdfNode->coordcal_typestr[2];
				pTdfNode->pMapNode->SetPositionX(sign_x*atof(pTdfNode->coordpara_str[0])*1000);
				pTdfNode->pMapNode->SetPositionZ(atof(pTdfNode->coordpara_str[2])*1000);
				TDF_DATUM_PLANE* pRelaTDFPlane=hashTdfPlane.GetValue(atoi(pTdfNode->coordpara_str[1]));
				if(pRelaTDFPlane)
					pTdfNode->pMapNode->hRelationPlane=pRelaTDFPlane->pMapPlane->handle;
			}
			//Լ����Ϣ
			//100000:X_RESTRICT;10000:Y_RESTRICT;1000:Z_RESTRICT;
			//100:X_ROT_RESTRICT;10:Y_ROT_RESTRICT;1:Z_ROT_RESTRICT
			if(pTdfNode->restrict%1000000>=100000)
				pTdfNode->pMapNode->SetRestrict(X_RESTRICT);	
			if(pTdfNode->restrict%100000>=10000)
				pTdfNode->pMapNode->SetRestrict(Y_RESTRICT);
			if(pTdfNode->restrict%10000>=1000)
				pTdfNode->pMapNode->SetRestrict(Z_RESTRICT);
			if(pTdfNode->restrict%1000>=100)
				pTdfNode->pMapNode->SetRestrict(X_ROT_RESTRICT);
			if(pTdfNode->restrict%100>=10)
				pTdfNode->pMapNode->SetRestrict(Y_ROT_RESTRICT);
			if(pTdfNode->restrict%10>=1)
				pTdfNode->pMapNode->SetRestrict(Z_ROT_RESTRICT);
			//������ĺ��ַ���
			pTdfNode->pMapNode->cfgword=RetrieveCfgWordFromStr(pTdfNode->cfgstr,toupper(pTdfNode->layer[0])=='L');
		}
		for(pTdfNode=hashTdfNode.GetFirst();pTdfNode;pTdfNode=hashTdfNode.GetNext())
			pTdfNode->pMapNode->Set();
		for(TDF_NODELOAD* pTdfNodeLoad=hashTdfNodeLoad.GetFirst();pTdfNodeLoad;pTdfNodeLoad=hashTdfNodeLoad.GetNext())
		{
			if(pTdfNodeLoad->nodeloadarr.GetSize()!=Ta.WorkStatus.GetNodeNum())
			{
				CXhChar50 sText;
				sText.Printf("�ڵ�%d�µĺ��������빤�����������!",pTdfNodeLoad->pointI);
				AfxMessageBox(sText);
				return;
			}
			CWorkStatus* pWorkStatus=Ta.WorkStatus.GetFirst();
			for(int i=0;i<pTdfNodeLoad->nodeloadarr.GetSize()&&pWorkStatus;i++,pWorkStatus=Ta.WorkStatus.GetNext())
				pTdfNodeLoad->nodeloadarr[i].pMapCase=pWorkStatus;
		}
		for(TDF_NODELOAD* pTdfNodeLoad=hashTdfNodeLoad.GetFirst();pTdfNodeLoad;pTdfNodeLoad=hashTdfNodeLoad.GetNext())
		{
			TDF_NODE* pTdfNode=hashTdfNode.GetValue(pTdfNodeLoad->pointI);
			if(pTdfNode==NULL)
				return;
			for(int i=0;i<pTdfNodeLoad->nodeloadarr.GetSize();i++)
			{
				NODELOADINFO* pNodeLoadInfo=&pTdfNodeLoad->nodeloadarr[i];
				CWorkStatus* pCase=pNodeLoadInfo->pMapCase;
				CExternalNodeLoad nodeLoad;
				f3dPoint Fxyz(pNodeLoadInfo->xForce,pNodeLoadInfo->yForce,pNodeLoadInfo->zForce);
				nodeLoad.Fxyz=Fxyz*1000/pCase->GamaQ()/pCase->Kesai();
				nodeLoad.permanentFz=pNodeLoadInfo->permanentFz*1000;
				if(pCase->m_cMainLoadType!=4)
				{	//��װ���������ǽṹ��Ҫ��ϵ��
					nodeLoad.Fxyz/=Ta.SafetyCoef();
					nodeLoad.permanentFz/=Ta.SafetyCoef();
				}
				pCase->hashNodeLoad.SetValue(pTdfNode->pMapNode->handle,nodeLoad);
			}
		}
		CObjNoGroup* pWindSegGroup=Ta.NoManager.FromGroupID(WINDSEG_GROUP);
		pWindSegGroup->EmptyNo();
		for(TDF_WINDLOADSEG* pTdfWindLoadSeg=TdfWindLoadSegList.GetFirst();pTdfWindLoadSeg;pTdfWindLoadSeg=TdfWindLoadSegList.GetNext())
		{
			CWindSegment* pSeg=Ta.WindLoadSeg.append();
			pSeg->iNo=pWindSegGroup->EnumIdleNo();
			pWindSegGroup->SetNoState(pSeg->iNo);
			pSeg->cType=pTdfWindLoadSeg->cType;
			pSeg->cMirType=pTdfWindLoadSeg->cMirType;
			pSeg->cCalStyle=pTdfWindLoadSeg->cCalStyle;
			pSeg->BetaZ=pTdfWindLoadSeg->BetaZ;
			pSeg->CqiFront=pTdfWindLoadSeg->CqiFront;
			pSeg->CqiSide=pTdfWindLoadSeg->CqiSide;
			pSeg->m_bAutoCalProfileCoef=pTdfWindLoadSeg->bAutoCalProfileCoef;	
			if(!pTdfWindLoadSeg->bAutoCalProfileCoef)
				pSeg->m_fProfileModeCoef=pTdfWindLoadSeg->fProfileModeCoef;
			if(!pTdfWindLoadSeg->bOrginIsZero)
				pSeg->xProjPlaneOrg=pTdfWindLoadSeg->origin;
			for(int i=0;i<pTdfWindLoadSeg->pontIArr.GetSize();i++)
			{
				long nPoint=pTdfWindLoadSeg->pontIArr[i];
				TDF_NODE* pTdfNode=hashTdfNode.GetValue(nPoint);
				if(pTdfNode&&pTdfNode->pMapNode)
				{
					pSeg->nodeArr[i]=pTdfNode->pMapNode->handle;	
					pSeg->nodePtrArr[i]=pTdfNode->pMapNode;	
				}
			}
		}
		//ת����Ԫ��Ϣ
		CElemInfo *pElemInfo;
		CStringKeyHashList<CLDSLinePartPtr>hashRods;
		Ta.Elems.Empty();
		for(TDF_ELEM* pTdfElem=hashTdfElem.GetFirst();pTdfElem;pTdfElem=hashTdfElem.GetNext())
		{
			CLDSLinePart* pLdsRod=NULL,*pMirXRod=NULL,*pMirYRod=NULL,*pMirZRod=NULL;
			CXhChar16 rodLabel(pTdfElem->uniteRodLabel);
			if(rodLabel.Length>0&&hashRods.GetValue(rodLabel))
				pLdsRod=hashRods[rodLabel];
			if(pLdsRod==NULL)
			{
				pLdsRod=(CLDSLinePart*)Ta.Parts.append(CLS_LINEPART);
				long JS=pTdfElem->JS,JE=pTdfElem->JE;
				if(rodLabel.Length>0)
				{
					CXhString label;
					char* key=label.StrToKey(rodLabel,"-");
					if(key!=NULL)
					{
						JS=atoi(key);
						key=label.StrToKey(NULL,"-");
					}
					if(key!=NULL)
						JE=atoi(key);
				}
				hashRods.SetValue(CXhChar16("%d-%d",JS,JE),pLdsRod);
				pTdfNode=hashTdfNode.GetValue(JS);
				if(pTdfNode)
					pLdsRod->pStart=pTdfNode->pMapNode;
				pTdfNode=hashTdfNode.GetValue(JE);
				if(pTdfNode)
					pLdsRod->pEnd=pTdfNode->pMapNode;
				pLdsRod->SetSizeSpec(pTdfElem->size);
				pLdsRod->SetStart(pLdsRod->pStart->Position(false));
				pLdsRod->SetEnd(pLdsRod->pEnd->Position(false));
				pLdsRod->SetLayer(pTdfElem->layer);
				pLdsRod->SetSizeSpec(pTdfElem->size);
				pLdsRod->cMaterial=pTdfElem->cMaterial;
				pLdsRod->cfgword=pLdsRod->pStart->cfgword&pLdsRod->pEnd->cfgword;
				pLdsRod->hole_out=pTdfElem->hole_out;
				if(isdigit(pTdfElem->lamdaItems[0][0])!=0)
					pLdsRod->CalLenCoef.iTypeNo=pTdfElem->lamdaItems[0][0]-'0';
				else
					pLdsRod->CalLenCoef.iTypeNo=0;
				pLdsRod->m_bAutoCalHoleOutCoef=pTdfElem->m_bAutoCalHoleOutCoef;
				//�˼��˽ڵ㸸�˼���ֵ������Ԫ��ȡ����ȷ
				if(pLdsRod->pStart->hFatherPart==0)
					pLdsRod->pStart->hFatherPart=pLdsRod->handle;
				if(pLdsRod->pEnd->hFatherPart==0)
					pLdsRod->pEnd->hFatherPart=pLdsRod->handle;
				if(strchr(pTdfElem->mirstr,'X')!=NULL)
				{
					pMirXRod=CreateMirRod(pLdsRod,'X',hashTdfNode);
					if(pMirXRod!=pLdsRod)
						pLdsRod->relativeObjs.SetValue(pMirXRod->handle,RELATIVE_OBJECT(pMirXRod->handle,MIRMSG(1)));
				}
				if(strchr(pTdfElem->mirstr,'Y')!=NULL)
				{
					pMirYRod=CreateMirRod(pLdsRod,'Y',hashTdfNode);
					if(pMirYRod!=pLdsRod)
						pLdsRod->relativeObjs.SetValue(pMirYRod->handle,RELATIVE_OBJECT(pMirYRod->handle,MIRMSG(2)));
				}
				if(strchr(pTdfElem->mirstr,'Z')!=NULL)
				{
					if(pMirXRod==pLdsRod)
						pMirZRod=pMirYRod;
					else if(pMirYRod==pLdsRod)
						pMirZRod=pMirXRod;
					else
						pMirZRod=CreateMirRod(pLdsRod,'Z',hashTdfNode);
					if(pMirZRod!=pLdsRod)
						pLdsRod->relativeObjs.SetValue(pMirZRod->handle,RELATIVE_OBJECT(pMirZRod->handle,MIRMSG(3)));
				}
				if(pMirXRod&&pMirXRod!=pLdsRod)
				{
					//�˼��˽ڵ㸸�˼���ֵ������Ԫ��ȡ����ȷ
					if(pMirXRod->pStart->hFatherPart==0)
						pMirXRod->pStart->hFatherPart=pMirXRod->handle;
					if(pMirXRod->pEnd->hFatherPart==0)
						pMirXRod->pEnd->hFatherPart=pMirXRod->handle;
					pMirXRod->relativeObjs.SetValue(pLdsRod->handle,RELATIVE_OBJECT(pLdsRod->handle,MIRMSG(1)));
					if(pMirYRod&&pMirYRod!=pLdsRod)
						pMirXRod->relativeObjs.SetValue(pMirYRod->handle,RELATIVE_OBJECT(pMirYRod->handle,MIRMSG(3)));
					if(pMirZRod&&pMirZRod!=pLdsRod)
						pMirXRod->relativeObjs.SetValue(pMirZRod->handle,RELATIVE_OBJECT(pMirZRod->handle,MIRMSG(2)));
				}
				if(pMirYRod&&pMirYRod!=pLdsRod)
				{
					//�˼��˽ڵ㸸�˼���ֵ������Ԫ��ȡ����ȷ
					if(pMirYRod->pStart->hFatherPart==0)
						pMirYRod->pStart->hFatherPart=pMirYRod->handle;
					if(pMirYRod->pEnd->hFatherPart==0)
						pMirYRod->pEnd->hFatherPart=pMirYRod->handle;
					pMirYRod->relativeObjs.SetValue(pLdsRod->handle,RELATIVE_OBJECT(pLdsRod->handle,MIRMSG(2)));
					if(pMirXRod&&pMirXRod!=pLdsRod)
						pMirYRod->relativeObjs.SetValue(pMirXRod->handle,RELATIVE_OBJECT(pMirXRod->handle,MIRMSG(3)));
					if(pMirZRod&&pMirZRod!=pLdsRod)
						pMirYRod->relativeObjs.SetValue(pMirZRod->handle,RELATIVE_OBJECT(pMirZRod->handle,MIRMSG(2)));
				}
				if(pMirZRod&&pMirZRod!=pLdsRod)
				{
					//�˼��˽ڵ㸸�˼���ֵ������Ԫ��ȡ����ȷ
					if(pMirZRod->pStart->hFatherPart==0)
						pMirZRod->pStart->hFatherPart=pMirZRod->handle;
					if(pMirZRod->pEnd->hFatherPart==0)
						pMirZRod->pEnd->hFatherPart=pMirZRod->handle;
					pMirZRod->relativeObjs.SetValue(pLdsRod->handle,RELATIVE_OBJECT(pLdsRod->handle,MIRMSG(3)));
					if(pMirXRod&&pMirXRod!=pLdsRod)
						pMirZRod->relativeObjs.SetValue(pMirXRod->handle,RELATIVE_OBJECT(pMirXRod->handle,MIRMSG(2)));
					if(pMirYRod&&pMirYRod!=pLdsRod)
						pMirZRod->relativeObjs.SetValue(pMirYRod->handle,RELATIVE_OBJECT(pMirYRod->handle,MIRMSG(1)));
				}
			}
			else
			{
				if(strchr(pTdfElem->mirstr,'X')!=NULL)
					pMirXRod=pLdsRod->GetMirRod(MIRMSG(1));
				if(strchr(pTdfElem->mirstr,'Y')!=NULL)
					pMirYRod=pLdsRod->GetMirRod(MIRMSG(2));
				if(strchr(pTdfElem->mirstr,'Z')!=NULL)
					pMirZRod=pLdsRod->GetMirRod(MIRMSG(3));
			}
			TDF_NODE* pTdfNodeStart=hashTdfNode.GetValue(pTdfElem->JS);
			TDF_NODE* pTdfNodeEnd=hashTdfNode.GetValue(pTdfElem->JE);
			if(pTdfNodeStart&&pTdfNodeStart->pMapNode&&pTdfNodeEnd&&pTdfNodeEnd->pMapNode)
			{
				if(pTdfNodeStart->pMapNode->hFatherPart==0)
					pTdfNodeStart->pMapNode->hFatherPart=pLdsRod->handle;
				pElemInfo=Ta.Elems.Add(pTdfNodeStart->pMapNode->handle,pTdfNodeEnd->pMapNode->handle);
			}
			else
				continue;	//error
			pElemInfo->SetBelongModel(&Ta);
			pElemInfo->Id=CElemInfo::GetNewId();
			//Ta.ElemPtrArr[pElemInfo->Id-1]=pElemInfo;
			if(pTdfNodeStart->pMapNode->hFatherPart==0)
				pTdfNodeStart->pMapNode->hFatherPart=pLdsRod->handle;
			if(pTdfNodeEnd->pMapNode->hFatherPart==0)
				pTdfNodeEnd->pMapNode->hFatherPart=pLdsRod->handle;
			pElemInfo->m_hParentRod=pLdsRod->handle;
			if(pElemInfo->m_hStartNode==pLdsRod->pStart->handle)
				pLdsRod->start_force_type=pTdfElem->start_force_type;
			else if(pElemInfo->m_hStartNode==pLdsRod->pEnd->handle)
				pLdsRod->end_force_type=pTdfElem->start_force_type;
			if(pElemInfo->m_hEndNode==pLdsRod->pStart->handle)
				pLdsRod->start_force_type=pTdfElem->end_force_type;
			else if(pElemInfo->m_hEndNode==pLdsRod->pEnd->handle)
				pLdsRod->end_force_type=pTdfElem->end_force_type;
			if(pElemInfo->m_hStartNode==pLdsRod->pStart->handle)
				pLdsRod->start_joint_type=pTdfElem->start_joint_type;
			else if(pElemInfo->m_hStartNode==pLdsRod->pEnd->handle)
				pLdsRod->end_joint_type=pTdfElem->start_joint_type;
			if(pElemInfo->m_hEndNode==pLdsRod->pStart->handle)
				pLdsRod->start_joint_type=pTdfElem->end_joint_type;
			else if(pElemInfo->m_hEndNode==pLdsRod->pEnd->handle)
				pLdsRod->end_joint_type=pTdfElem->end_joint_type;
			pElemInfo->relaElem.bFatherElem=true;
			TDF_NODE* pMirTdfStart,*pMirTdfEnd;
			if(strchr(pTdfElem->mirstr,'X')!=NULL)
			{
				pMirTdfStart=TransferMirRelaNode('X',pTdfNodeStart);
				pMirTdfEnd  =TransferMirRelaNode('X',pTdfNodeEnd);
				if(pMirTdfStart&&pMirTdfEnd)
				{
					pMirXRod->start_force_type=pLdsRod->start_force_type;
					pMirXRod->end_force_type=pLdsRod->end_force_type;
					pMirXRod->start_joint_type=pLdsRod->start_joint_type;
					pMirXRod->end_joint_type=pLdsRod->end_joint_type;
					if(pMirTdfStart->pMapNode->hFatherPart==0)
						pMirTdfStart->pMapNode->hFatherPart=pMirXRod->handle;
					if(pMirTdfEnd->pMapNode->hFatherPart==0)
						pMirTdfEnd->pMapNode->hFatherPart=pMirXRod->handle;
					pElemInfo->relaElem.xElemMirX.pElemInfo=Ta.Elems.Add(pMirTdfStart->pMapNode->handle,pMirTdfEnd->pMapNode->handle);
					pElemInfo->relaElem.xElemMirX.hStartNode=pMirTdfStart->pMapNode->handle;
					pElemInfo->relaElem.xElemMirX.hEndNode  =pMirTdfEnd->pMapNode->handle;
					pElemInfo->relaElem.xElemMirX.pElemInfo->SetBelongModel(&Ta);
					pElemInfo->relaElem.xElemMirX.pElemInfo->relaElem.bFatherElem=false;
					if(hashTdfNode.GetValue(pMirTdfStart->pointI))
						pElemInfo->relaElem.xElemMirX.pElemInfo->m_hStartNode=hashTdfNode[pMirTdfStart->pointI].pMapNode->handle;
					if(hashTdfNode.GetValue(pMirTdfEnd->pointI))
						pElemInfo->relaElem.xElemMirX.pElemInfo->m_hEndNode=hashTdfNode[pMirTdfEnd->pointI].pMapNode->handle;
					pElemInfo->relaElem.xElemMirX.pElemInfo->m_hParentRod=pMirXRod->handle;
				}
			}
			if(strchr(pTdfElem->mirstr,'Y')!=NULL)
			{
				pMirTdfStart=TransferMirRelaNode('Y',pTdfNodeStart);
				pMirTdfEnd  =TransferMirRelaNode('Y',pTdfNodeEnd);
				if(pMirTdfStart&&pMirTdfEnd)
				{
					pMirYRod->start_force_type=pLdsRod->start_force_type;
					pMirYRod->end_force_type=pLdsRod->end_force_type;
					pMirYRod->start_joint_type=pLdsRod->start_joint_type;
					pMirYRod->end_joint_type=pLdsRod->end_joint_type;
					if(pMirTdfStart->pMapNode->hFatherPart==0)
						pMirTdfStart->pMapNode->hFatherPart=pMirYRod->handle;
					if(pMirTdfEnd->pMapNode->hFatherPart==0)
						pMirTdfEnd->pMapNode->hFatherPart=pMirYRod->handle;
					pElemInfo->relaElem.xElemMirY.pElemInfo=Ta.Elems.Add(pMirTdfStart->pMapNode->handle,pMirTdfEnd->pMapNode->handle);
					pElemInfo->relaElem.xElemMirY.hStartNode=pMirTdfStart->pMapNode->handle;
					pElemInfo->relaElem.xElemMirY.hEndNode  =pMirTdfEnd->pMapNode->handle;
					pElemInfo->relaElem.xElemMirY.pElemInfo->SetBelongModel(&Ta);
					pElemInfo->relaElem.xElemMirY.pElemInfo->relaElem.bFatherElem=false;
					if(hashTdfNode.GetValue(pMirTdfStart->pointI))
						pElemInfo->relaElem.xElemMirY.pElemInfo->m_hStartNode=hashTdfNode[pMirTdfStart->pointI].pMapNode->handle;
					if(hashTdfNode.GetValue(pMirTdfEnd->pointI))
						pElemInfo->relaElem.xElemMirY.pElemInfo->m_hEndNode=hashTdfNode[pMirTdfEnd->pointI].pMapNode->handle;
					pElemInfo->relaElem.xElemMirY.pElemInfo->m_hParentRod=pMirYRod->handle;
				}
			}
			if(strchr(pTdfElem->mirstr,'Z')!=NULL)
			{
				pMirTdfStart=TransferMirRelaNode('Z',pTdfNodeStart);
				pMirTdfEnd  =TransferMirRelaNode('Z',pTdfNodeEnd);
				if(pMirTdfStart&&pMirTdfEnd)
				{
					pMirZRod->start_force_type=pLdsRod->start_force_type;
					pMirZRod->end_force_type=pLdsRod->end_force_type;
					pMirZRod->start_joint_type=pLdsRod->start_joint_type;
					pMirZRod->end_joint_type=pLdsRod->end_joint_type;
					if(pMirTdfStart->pMapNode->hFatherPart==0)
						pMirTdfStart->pMapNode->hFatherPart=pMirZRod->handle;
					if(pMirTdfEnd->pMapNode->hFatherPart==0)
						pMirTdfEnd->pMapNode->hFatherPart=pMirZRod->handle;
					pElemInfo->relaElem.xElemMirZ.pElemInfo=Ta.Elems.Add(pMirTdfStart->pMapNode->handle,pMirTdfEnd->pMapNode->handle);
					pElemInfo->relaElem.xElemMirZ.hStartNode=pMirTdfStart->pMapNode->handle;
					pElemInfo->relaElem.xElemMirZ.hEndNode  =pMirTdfEnd->pMapNode->handle;
					pElemInfo->relaElem.xElemMirZ.pElemInfo->SetBelongModel(&Ta);
					pElemInfo->relaElem.xElemMirZ.pElemInfo->relaElem.bFatherElem=false;
					if(hashTdfNode.GetValue(pMirTdfStart->pointI))
						pElemInfo->relaElem.xElemMirZ.pElemInfo->m_hStartNode=hashTdfNode[pMirTdfStart->pointI].pMapNode->handle;
					if(hashTdfNode.GetValue(pMirTdfEnd->pointI))
						pElemInfo->relaElem.xElemMirZ.pElemInfo->m_hEndNode=hashTdfNode[pMirTdfEnd->pointI].pMapNode->handle;
					pElemInfo->relaElem.xElemMirZ.pElemInfo->m_hParentRod=pMirZRod->handle;
				}
			}
			//�ڵ㸸�˼���ֵ������Ԫ��ȡ����ȷ
			if(pLdsRod->pStart==NULL)
				pLdsRod->pStart->hFatherPart=pLdsRod->handle;
			if(pLdsRod->pEnd==NULL)
				pLdsRod->pEnd->hFatherPart=pLdsRod->handle;
			//��ȡ������
			RetrieveElemNodeByLabel(pTdfElem->horiRodLabel,hashTdfNode,&pElemInfo->horiElem.hStart,&pElemInfo->horiElem.hEnd);
			//��ϸ�ȼ�����Ϣ
			if(isdigit(pTdfElem->lamdaItems[0][0])!=0)
				pElemInfo->cCalLenType=pTdfElem->lamdaItems[0][0]-'0';
			else
				pElemInfo->cCalLenType=0;
			for(int i=0;i<4;i++)
			{
				char* key=strchr(pTdfElem->lamdaItems[i],'R');
				if(key==NULL)
					continue;
				CXhChar16 LoStr;
				Lo_CALINFO* pCurCalInfo;
				char cR=*(key+1);
				char c2=*(key+2);
				if(cR=='y'&&(isdigit(c2)||c2=='L'))
				{	//��С��
					LoStr.Copy(key+2);
					pCurCalInfo=&pElemInfo->LoRyo;
				}
				else if(cR=='x'&&(isdigit(c2)||c2=='L'))
				{	//ƽ����
					LoStr.Copy(key+2);
					pCurCalInfo=&pElemInfo->LoRx;
				}
				else
				{	//ͬʱ��ѹ��Ϣ
					for(int j=0;j<3;j++)
					{
						if(pElemInfo->smcItems[j].cSMCType>0)
							continue;
						if(cR=='y')
							pElemInfo->smcItems[j].cSMCType=0x80;
						if(c2=='Y')
						{
							pElemInfo->smcItems[j].cSMCType|=SMC_ITEM::SMC_REFER_MIRY;
							if(pElemInfo->relaElem.xElemMirY.pElemInfo)
							{
								pElemInfo->smcItems[j].relaElem.hStart=pElemInfo->relaElem.xElemMirY.pElemInfo->m_hStartNode;
								pElemInfo->smcItems[j].relaElem.hEnd=pElemInfo->relaElem.xElemMirY.pElemInfo->m_hEndNode;
							}
						}
						else if(c2=='X')
						{
							pElemInfo->smcItems[j].cSMCType|=SMC_ITEM::SMC_REFER_MIRX;
							if(pElemInfo->relaElem.xElemMirX.pElemInfo)
							{
								pElemInfo->smcItems[j].relaElem.hStart=pElemInfo->relaElem.xElemMirX.pElemInfo->m_hStartNode;
								pElemInfo->smcItems[j].relaElem.hEnd=pElemInfo->relaElem.xElemMirX.pElemInfo->m_hEndNode;
							}
						}
						else if(c2=='i')
							pElemInfo->smcItems[j].cSMCType|=SMC_ITEM::SMC_REFER_START;
						else if(c2=='!')
							pElemInfo->smcItems[j].cSMCType|=SMC_ITEM::SMC_REFER_END;
						else if(c2==':')
							pElemInfo->smcItems[j].cSMCType|=SMC_ITEM::SMC_REFER_2ENDS;
						else if(c2=='R')
							pElemInfo->smcItems[j].cSMCType|=SMC_ITEM::SMC_REFER_UDF;
						LoStr.Copy(key+3);
						pCurCalInfo=&pElemInfo->smcItems[j].LoRc;
						key=strchr(LoStr,'|');
						if(key)
						{
							*key=0;
							key++;
							RetrieveElemNodeByLabel(key,hashTdfNode,
								&pElemInfo->smcItems[j].relaElem.hStart,&pElemInfo->smcItems[j].relaElem.hEnd);
						}
						break;
					}
				}
				key=strchr(LoStr,'*');
				if(key!=NULL)
				{
					pCurCalInfo->coef=expression.SolveExpression(key+1);
					*key=0;
				}
				else
					pCurCalInfo->coef=1.0;
				if(LoStr.Equal("L"))
					pCurCalInfo->bCalByElemLen=true;
				else
					RetrieveElemNodeByLabel(LoStr,hashTdfNode,&pCurCalInfo->hStartNode,&pCurCalInfo->hEndNode);
				if(pElemInfo->relaElem.xElemMirX.pElemInfo)
				{
					pElemInfo->relaElem.xElemMirX.pElemInfo->LoRyo=pElemInfo->LoRyo;
					pElemInfo->relaElem.xElemMirX.pElemInfo->LoRx=pElemInfo->LoRx;
					//pElemInfo->relaElem.pElemMirX->LoRxc=pElemInfo->LoRxc;
					for(int k=0;k<3;k++)
						pElemInfo->relaElem.xElemMirX.pElemInfo->smcItems[k]=pElemInfo->smcItems[k];
				}
				if(pElemInfo->relaElem.xElemMirY.pElemInfo)
				{
					pElemInfo->relaElem.xElemMirY.pElemInfo->LoRyo=pElemInfo->LoRyo;
					pElemInfo->relaElem.xElemMirY.pElemInfo->LoRx=pElemInfo->LoRx;
					//pElemInfo->relaElem.pElemMirY->LoRxc=pElemInfo->LoRxc;
					for(int k=0;k<3;k++)
						pElemInfo->relaElem.xElemMirY.pElemInfo->smcItems[k]=pElemInfo->smcItems[k];
				}
				if(pElemInfo->relaElem.xElemMirZ.pElemInfo)
				{
					pElemInfo->relaElem.xElemMirZ.pElemInfo->LoRyo=pElemInfo->LoRyo;
					pElemInfo->relaElem.xElemMirZ.pElemInfo->LoRx=pElemInfo->LoRx;
					//pElemInfo->relaElem.pElemMirZ->LoRxc=pElemInfo->LoRxc;
					for(int k=0;k<3;k++)
						pElemInfo->relaElem.xElemMirZ.pElemInfo->smcItems[k]=pElemInfo->smcItems[k];
				}
			}
		}
		//�����˼��ϵ��м�ڵ㸸�˼�Ϊ�˼���������ᵼ���ɸ˼���ֵ�Ԫʱ��������
		for(CLDSLinePart *pRod=Ta.Parts.GetFirstLinePart();pRod;pRod=Ta.Parts.GetNextLinePart())
		{
			if(pRod->pStart==NULL||pRod->pEnd==NULL)
				continue;
			CPtInLineCheck lineCheck(pRod->pStart->Position(false),pRod->pEnd->Position(false));
			for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{
				if(!pRod->cfgword.And(pNode->cfgword))
					continue;
				double result=lineCheck.CheckPoint(pNode->Position(false));
				if(result>EPS&&result<EPS_COS)
				{
					CLDSLinePart* pFatherRod=NULL;
					BOOL pushed=Ta.Parts.push_stack();
					if(pNode->hFatherPart>0)
						pFatherRod=(CLDSLinePart*)Ta.Parts.FromHandle(pNode->hFatherPart,CLS_LINEPART);
					Ta.Parts.pop_stack(pushed);
					if(pFatherRod==NULL||(pNode==pFatherRod->pStart||pNode==pFatherRod->pEnd))
						pNode->hFatherPart=pRod->handle;	//�ڵ㸸�˼�Ӧ�����Ǹ��˼����м�ڵ�
					else if(pNode->m_cPosCalType==4)	//����ڵ�
					{
						pNode->arrRelationPole[0]=pNode->hFatherPart;
						pNode->arrRelationPole[1]=pRod->handle;
						pRod->CalLenCoef.hRefPole=pNode->hFatherPart;
						if(pFatherRod)
							pFatherRod->CalLenCoef.hRefPole=pRod->handle;
					}
				}
			}
		}
		Ta.ElemPtrArr.SetSize(Ta.Elems.GetNodeNum(),1);
		CElemInfo::ClearInitialGlobalId();
		for(pElemInfo=Ta.Elems.GetFirst();pElemInfo;pElemInfo=Ta.Elems.GetNext())
		{
			pElemInfo->Id=CElemInfo::GetNewId();
			Ta.ElemPtrArr[pElemInfo->Id-1]=pElemInfo;
		}
	}
	catch(char* error)
	{
		AfxMessageBox(error);
	}
}

void ExportTowerDesignTxtFile(LPCTSTR lpszPathName) 
{
	FILE* fp=fopen(lpszPathName,"wt");
	if(fp==NULL)
		return;
	int i;
	CXhChar200 lineText;
	if(strlen(Ta.m_sPrjName)>0)
	{
		lineText.Printf("PROJECT: '%s'",Ta.m_sPrjName);
		fprintf(fp,"%s\n",(char*)lineText);
	}
	if(strlen(Ta.m_sOperator)>0)
	{
		lineText.Printf("DESIGNER: '%s'",Ta.m_sOperator);
		fprintf(fp,"%s\n",(char*)lineText);
	}
	if(strlen(Ta.m_sOperator)>0)
	{
		lineText.Printf("AUDITOR: '%s'",Ta.m_sAuditor);
		fprintf(fp,"%s\n",(char*)lineText);
	}
	// CODE=0, 110kV~750kV�ܿ������·��ƹ淶 GB 50545-2010,�ܿ��͵���·�����ṹ��Ƽ����涨 DL/T 5154-2002
	// CODE=1, ���ʽṹ��ƹ淶 GB 50135-2006;
	// CODE=2, ����ASCE 10-97
	fprintf(fp,"\nSECTION DESIGN-CODE 0x%x\n",Ta.DesignCode()->CodeSerial());
	//�ṹ��Ҫ��ϵ��
	lineText.Printf("  SAFTEY_COEF= %f",Ta.m_fSafetyCoef);
	SimplifiedNumString(lineText);
	fprintf(fp,"%s",(char*)lineText);
	//�Ż�ѡ��ʱ�־ú��ط���ϵ��
	lineText.Printf("\n  gamaG=%.1f;PERMANENT LOAD PARTIAL FACTOR",Ta.m_fGamaGForDesign);
	fprintf(fp,"%s",(char*)lineText);
	//�������������ϵ��
	lineText.Printf("\n  gamaC=%.1f;LOAD CAPACITY COEF",Ta.m_fLimitBearCoef);
	fprintf(fp,"%s",(char*)lineText);
	//�˼�ѡ�ĳ�ϸ������
	fprintf(fp,"\n  ANGLE_LAMDA_LIMIT_OF_LEG_MEMBER=%d",Ta.SelMatPara.m_nMainPoleSlendernessLimit);
	fprintf(fp,"\n  ANGLE_LAMDA_LIMIT_OF_COMP_MEMBER=%d",Ta.SelMatPara.m_nXiePoleSlendernessLimit);
	fprintf(fp,"\n  ANGLE_LAMDA_LIMIT_OF_BRACE_MEMBER=%d",Ta.SelMatPara.m_nAuxPoleSlendernessLimit);
	fprintf(fp,"\n  ANGLE_LAMDA_LIMIT_OF_TENS_MEMBER=%d",Ta.SelMatPara.m_nTensivePoleSlendernessLimit);
	fprintf(fp,"\n  TUBE_LAMDA_LIMIT_OF_LEG_MEMBER=%d",Ta.SelMatPara.m_nMainTubeSlendernessLimit);
	fprintf(fp,"\n  TUBE_LAMDA_LIMIT_OF_COMP_MEMBER=%d",Ta.SelMatPara.m_nXieTubeSlendernessLimit);
	fprintf(fp,"\n  TUBE_LAMDA_LIMIT_OF_BRACE_MEMBER=%d",Ta.SelMatPara.m_nAuxTubeSlendernessLimit);
	fprintf(fp,"\n  TUBE_LAMDA_LIMIT_OF_TENS_MEMBER=%d",Ta.SelMatPara.m_nTensiveTubeSlendernessLimit);
	fprintf(fp,"\n  LEG_MEMBER_MIN_SIZE=%s",Ta.SelMatPara.m_csMinMainAngleSize);
	fprintf(fp,"\n  FRAME_MEMBER_MIN_SIZE=%s",Ta.SelMatPara.m_csMinForceAngleSize);
	fprintf(fp,"\n  BRACE_MEMBER_MIN_SIZE=%s",Ta.SelMatPara.m_csMinAngleSize);
  
	//0:������·�淶
	//1:���ʽṹ�淶
	//2:�����ṹ���ع淶
	//3:�û�ָ��;���ؼ���ִ�й淶������ϵ������������ϵ�����ɱ�������ϵ��������ϵ��ȡֵ����ѭ�Ĺ淶
	fprintf(fp,"\n\nSECTION LOAD-CODE %d\n",Ta.m_iLoadExecuteCode);
	fprintf(fp,"  LANDSCAPE='%C'\n",Ta.m_cLandscape);
	lineText.Printf("  ICE_DENSITY= %f",Ta.m_fIceDensity);
	SimplifiedNumString(lineText);
	lineText.Append(";kg/m3");
	fprintf(fp,"%s",(char*)lineText);

	fprintf(fp,"\n\nSECTION STEEL\n");
	//�ֲı�� �򻯷��� ��1��fy ��2��fy ��3��fy ��1��ft ��2��ft ��3��ft ��1��fc ��2��fc ��3��fc 
	for(i=0;i<CSteelMatLibrary::GetCount();i++)
	{
		STEELMAT mat=CSteelMatLibrary::RecordAt(i);
		double t1=0,t2=mat.t_lim[0]+1,t3=mat.t_lim[1]+1;
		fprintf(fp,"  %s %C %d %d %.0f %.0f %.0f %.0f %.0f %.0f %.0f %.0f %.0f\n",mat.mark,mat.cBriefMark,
			mat.t_lim[0],mat.t_lim[1],//����Ƚ��е�ǿ�ȷ������
			mat.fy(t1),mat.fy(t2),mat.fy(t3),
			mat.f(t1),mat.f(t2),mat.f(t3),
			mat.fc(t1),mat.fc(t2),mat.fc(t3));
	}
	fprintf(fp,"\nSECTION BOLT\n");
	for(i=0;i<LsTypeCount;i++)
		fprintf(fp,"  %s %.0f %.0f %.0f\n",ls_table[i].grade,ls_table[i].fv,ls_table[i].f,ls_table[i].fc);
	fprintf(fp,"\nSECTION ANGLE-SIZE\n");
	for(i=0;i<jgguige_N;i++)
	{
		double size_spec=jgguige_table[i].wing_wide+jgguige_table[i].wing_thick*0.001;
		fprintf(fp,"%.3f %C %.3f %.3f %.3f %.3f %d\n",size_spec,jgguige_table[i].cType,jgguige_table[i].area,
			jgguige_table[i].Ry0,jgguige_table[i].Rx,jgguige_table[i].r,i+1);
	}
	fprintf(fp,"\nSECTION TUBE-SIZE\n");
	for(i=0;i<tubeguige_N;i++)
	{
		double size_spec=tubeguige_table[i].D+tubeguige_table[i].thick*0.001;
		fprintf(fp,"%.3f %.3f %.3f %d\n",size_spec,tubeguige_table[i].area,tubeguige_table[i].r,i+1);
	}
	
	fprintf(fp,"\nSECTION BOLT-MATCHING-RULE\n",Ta.listSelBoltCond.GetNodeNum());
	for(BOLT_SELCOND *pSelCond=Ta.listSelBoltCond.GetFirst();pSelCond;pSelCond=Ta.listSelBoltCond.GetNext())
	{
		fprintf(fp,"%d %s %d %s\n",pSelCond->upWidthLim,pSelCond->grade,pSelCond->d,pSelCond->matStr);
	}
	fprintf(fp,"\nSECTION MODULE\n");
	for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		lineText.Printf("%f",pModule->land_high);
		SimplifiedNumString(lineText);
		char leg_str[200]="";
		CXhChar50 descrip;
		descrip.Copy(pModule->description);
		descrip.ResizeLength(6,' ',true);
		pModule->m_dwLegCfgWord.GetLegScopeStr(leg_str,200);
		fprintf(fp,"%s %d '%s',%s\n",(char*)descrip,pModule->GetBodyNo(),leg_str,(char*)lineText);
	}
	fprintf(fp,"\nSECTION DATUM-PLANES ��׼�涨��\n");
	CHashList<long> hashPlaneNoById;
	for(CLDSPlane* pPlane=Ta.Plane.GetFirst();pPlane;pPlane=Ta.Plane.GetNext())
	{
		CLDSPoint* pDatumPoint1= Ta.Point.FromHandle(pPlane->hPointArr[0]);
		CLDSPoint* pDatumPoint2= Ta.Point.FromHandle(pPlane->hPointArr[1]);
		CLDSPoint* pDatumPoint3= Ta.Point.FromHandle(pPlane->hPointArr[2]);
		if( pDatumPoint1==NULL||pDatumPoint2==NULL||pDatumPoint3==NULL||
			pDatumPoint1->datum_pos_style!=8||pDatumPoint2->datum_pos_style!=8||pDatumPoint3->datum_pos_style!=8)
		{
			//AfxMessageBox("0X%X��׼ƽ��Ķ����׼�����ͷǡ���ڵ�ͬλ������������ʧ��!");
			continue;
		}
		long* pNo=hashPlaneNoById.SetValue(pPlane->handle,hashPlaneNoById.GetNodeNum()+1);
		CLDSNode* pNode1=Ta.Node.FromHandle(pDatumPoint1->des_para.hNode);
		CLDSNode* pNode2=Ta.Node.FromHandle(pDatumPoint2->des_para.hNode);
		CLDSNode* pNode3=Ta.Node.FromHandle(pDatumPoint3->des_para.hNode);
		fprintf(fp,"%s",(char*)CXhChar100("%3d %5d %5d %5d %s\n",*pNo,pNode1->point_i,pNode2->point_i,pNode3->point_i,pPlane->description));
	}

	fprintf(fp,"\nSECTION NODES �ڵ㶨��\n");
	CLDSNode *pNode;
	ARRAY_LIST<NODE_PTR>node_arr;
	node_arr.SetSize(0,Ta.Node.GetNodeNum());
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		node_arr.append(pNode);
	CQuickSort<NODE_PTR>::QuickSort(node_arr.m_pData,node_arr.GetSize(),compare_func1);
	for(i=0;i<node_arr.GetSize();i++)
	{
		CXhChar50 cfg_str,coordtype;
		CXhChar16 symmetry;
		if(node_arr[i]->IsFatherObject())	//���ڵ�
		{
			//�Գ���Ϣ
			long hMirX=node_arr[i]->GetRelativeMirObj(MIRMSG(1),FALSE);
			long hMirY=node_arr[i]->GetRelativeMirObj(MIRMSG(2),FALSE);
			long hMirZ=node_arr[i]->GetRelativeMirObj(MIRMSG(3),FALSE);
			if(hMirX>0x20&&hMirX!=node_arr[i]->handle)
				symmetry.Copy(" X");
			else
				symmetry.Copy("  ");
			if(hMirY>0x20&&hMirY!=node_arr[i]->handle)
				symmetry.Append("Y");
			else
				symmetry.Append(" ");
			if(hMirZ>0x20&&hMirZ!=hMirX&&hMirZ!=hMirY&&hMirZ!=node_arr[i]->handle)
				symmetry.Append("Z");
			else
				symmetry.Append(" ");
			if(symmetry[1]==' ')
			{	//�м䲻���пո�
				symmetry[1]=symmetry[2];
				symmetry[2]=' ';	//�Գ��ַ����м䲻���пո�
			}
			if(symmetry.Equal("    "))
				symmetry.Copy(" N/A");
			//�����������
			NODE_PTR arrRelaNodePtr[4]={NULL};
			long arrPointI[4]={0};
			for(int j=0;j<4;j++)
			{
				arrRelaNodePtr[j]=Ta.Node.FromHandle(node_arr[i]->arrRelationNode[j]);
				if(arrRelaNodePtr[j])
					arrPointI[j]=arrRelaNodePtr[j]->point_i;
			}
			if(node_arr[i]->m_cPosCalType==0)		//0.���κ�����
				coordtype.Printf("  0 %8.3f %8.3f %8.3f", node_arr[i]->Position(false).x*0.001,
					node_arr[i]->Position(false).y*0.001,node_arr[i]->Position(false).z*0.001);
			else if(node_arr[i]->m_cPosCalType==1||(node_arr[i]->m_cPosCalType==6&&node_arr[i]->m_cPosCalViceType=='X'))//1.�˼���X����ֵ�����
				coordtype.Printf("  X %8.3f %8d %8d", node_arr[i]->Position(false).x*0.001,arrPointI[0],arrPointI[1]);
			else if(node_arr[i]->m_cPosCalType==2||(node_arr[i]->m_cPosCalType==6&&node_arr[i]->m_cPosCalViceType=='Y'))//2.�˼���Y����ֵ�����
				coordtype.Printf("  Y %8d %8.3f %8d", arrPointI[0],node_arr[i]->Position(false).y*0.001,arrPointI[1]);
			else if(node_arr[i]->m_cPosCalType==3||(node_arr[i]->m_cPosCalType==6&&node_arr[i]->m_cPosCalViceType=='Z'))//3.�˼���Z����ֵ�����
				coordtype.Printf("  Z %8d %8d %8.3f", arrPointI[0],arrPointI[1],node_arr[i]->Position(false).z*0.001);
			else if(node_arr[i]->m_cPosCalType==4)		//4.�����
				coordtype.Printf("  I %5d %6d %6d %6d",arrPointI[0],arrPointI[1],arrPointI[2],arrPointI[3]);
			else if(node_arr[i]->m_cPosCalType==5)		//5.�����ȷֵ�
			{
				CXhChar16 scaleStr;
				CExpression::ToExpression(node_arr[i]->attach_scale,3,scaleStr,8);
				scaleStr.ResizeLength(8,' ',true);
				coordtype.Printf("DIV %8d %8d %s",arrPointI[0],arrPointI[1],(char*)scaleStr);
			}
			else if(node_arr[i]->m_cPosCalType==7)		//7.ƫ�Ƶ�
			{
				coordtype.Printf("MvL %5d %6d %6d %6.3f",arrPointI[0],arrPointI[1],arrPointI[2],node_arr[i]->attach_offset*0.001);
				if(node_arr[i]->m_cPosCalViceType>='X'&&node_arr[i]->m_cPosCalViceType<='Z')
					coordtype[2]=node_arr[i]->m_cPosCalViceType;	//�ظ˼�����X/Y/Z�������ƫ��
			}
			else if(node_arr[i]->m_cPosCalType>=8)		//8.��׼����ָ�������������
			{
				long iPlaneNo=0;
				if(hashPlaneNoById.GetValue(node_arr[i]->hRelationPlane))
					iPlaneNo=hashPlaneNoById[node_arr[i]->hRelationPlane];
				if(node_arr[i]->m_cPosCalType==8)		//8.��׼����XY����ֵ�����
				{
					if(node_arr[i]->m_cPosCalViceType>='X')
						coordtype.Printf("PZ%C %8.3f %8.3f %8d",node_arr[i]->m_cPosCalViceType,
						node_arr[i]->Position(false).x*0.001,node_arr[i]->Position(false).y*0.001,iPlaneNo);
					else
						coordtype.Printf("PZ  %8.3f %8.3f %8d",
						node_arr[i]->Position(false).x*0.001,node_arr[i]->Position(false).y*0.001,iPlaneNo);
				}
				else if(node_arr[i]->m_cPosCalType==9)		//9.��׼����YZ����ֵ�����
				{
					if(node_arr[i]->m_cPosCalViceType>='X')
						coordtype.Printf("PX%C %8d %8.3f %8.3f",node_arr[i]->m_cPosCalViceType,iPlaneNo,
						node_arr[i]->Position(false).y*0.001,node_arr[i]->Position(false).z*0.001);
					else
						coordtype.Printf("PX  %8d %8.3f %8.3f",iPlaneNo,
						node_arr[i]->Position(false).y*0.001,node_arr[i]->Position(false).z*0.001);
				}
				else if(node_arr[i]->m_cPosCalType==10)		//10.��׼����XZ����ֵ�����
				{
					if(node_arr[i]->m_cPosCalViceType>='X')
						coordtype.Printf("PY%C %8.3f %8d %8.3f",node_arr[i]->m_cPosCalViceType,
						node_arr[i]->Position(false).x*0.001,iPlaneNo,node_arr[i]->Position(false).z*0.001);
					else
						coordtype.Printf("PY  %8.3f %8d %8.3f",
						node_arr[i]->Position(false).x*0.001,iPlaneNo,node_arr[i]->Position(false).z*0.001);
				}
			}
			if(coordtype.Length==0)
				coordtype.Printf("  0 %8.3f %8.3f %8.3f", node_arr[i]->Position(false).x*0.001,
					node_arr[i]->Position(false).y*0.001,node_arr[i]->Position(false).z*0.001);
			//Լ����Ϣ
			//100000:X_RESTRICT;10000:Y_RESTRICT;1000:Z_RESTRICT;
			//100:X_ROT_RESTRICT;10:Y_ROT_RESTRICT;1:Z_ROT_RESTRICT
			DWORD dwRestrictMark=0;
			if(node_arr[i]->IsHasRestrict(X_RESTRICT))
				dwRestrictMark+=100000;
			if(node_arr[i]->IsHasRestrict(Y_RESTRICT))
				dwRestrictMark+=10000;
			if(node_arr[i]->IsHasRestrict(Z_RESTRICT))
				dwRestrictMark+=1000;
			if(node_arr[i]->IsHasRestrict(X_ROT_RESTRICT))
				dwRestrictMark+=100;
			if(node_arr[i]->IsHasRestrict(Y_ROT_RESTRICT))
				dwRestrictMark+=10;
			if(node_arr[i]->IsHasRestrict(Z_ROT_RESTRICT))
				dwRestrictMark+=1;
			if(node_arr[i]->layer(0)=='L')
				node_arr[i]->cfgword.GetLegScopeStr(cfg_str,50,true);
			else
				node_arr[i]->cfgword.GetBodyScopeStr(cfg_str);
			fprintf(fp,"%6d %s %s %s %6d %s\n",node_arr[i]->point_i,(char*)symmetry,node_arr[i]->layer(),(char*)coordtype,dwRestrictMark,(char*)cfg_str);
		}
	}
	fprintf(fp,"\nSECTION MEMBERS\n");
	preprocessor.InitSourceDesignInfo(&Ta);
	CElemInfo* pElemInfo;
	ARRAY_LIST<ELEMINFO_PTR>elemInfoArr;
	elemInfoArr.SetSize(0,1000);
	for(pElemInfo=Ta.Elems.GetFirst();pElemInfo;pElemInfo=Ta.Elems.GetNext())
	{
		if(pElemInfo->relaElem.bFatherElem)
			elemInfoArr.append(pElemInfo);
	}
	CQuickSort<ELEMINFO_PTR>::QuickSort(elemInfoArr.m_pData,elemInfoArr.GetSize(),compare_eleminfo);
	for(i=0;i<elemInfoArr.GetSize();i++)
	{
		pElemInfo=elemInfoArr[i];
		CSuperSmartPtr<CLDSPart> pPart=Ta.Parts.FromHandle(elemInfoArr[i]->m_hParentRod);
		if(pPart==NULL||!pPart->IsLinePart())
			continue;
		CLDSLinePart* pParentRod=pPart.LinePartPointer();
		CLDSNode* pStart=Ta.Node.FromHandle(elemInfoArr[i]->m_hStartNode);
		CLDSNode* pEnd  =Ta.Node.FromHandle(elemInfoArr[i]->m_hEndNode);
		if(pStart==NULL||pEnd==NULL||pParentRod->pStart==NULL||pParentRod->pEnd==NULL)
			continue;
		int endState=0;	//�˶�״��
		if(elemInfoArr[i]->StartForceType(pParentRod)==0)	//0:CENTRIC_FORCE1.ƫ������
			endState+=1;	//��λ��Ϊ1��ʾʼ��Ϊ��������
		if(elemInfoArr[i]->EndForceType(pParentRod)==0)	//0:CENTRIC_FORCE
			endState += 2;	//��λ��Ϊ2��ʾ�ն�Ϊ��������
		if(elemInfoArr[i]->StartJointState(pParentRod)!=0)	//0.�½�;1.�ս�;2.ǿ�ƽ½�
			endState+=elemInfoArr[i]->StartJointState(pParentRod)*100;
		if(elemInfoArr[i]->EndJointState(pParentRod)!=0)	//0.�½�;1.�ս�;2.ǿ�ƽ½�
			endState+=elemInfoArr[i]->EndJointState(pParentRod)*10;
		CXhChar16 spec;	//������
		if(pParentRod->GetClassTypeId()==CLS_LINETUBE||pParentRod->GetClassTypeId()==CLS_LINEFLAT)
		{
			CLDSLineAngle::GetAngleSimpleSpec(spec,pParentRod->size_wide,pParentRod->size_wide,pParentRod->size_thick);
			if(pParentRod->GetClassTypeId()==CLS_LINETUBE)
				spec.InsertBefore(" G");
			else
				spec.InsertBefore(" B");
		}
		else if(pParentRod->IsAngle(TRUE))
			pParentRod->GetSpecification(spec,FALSE);
		if(pParentRod->cMaterial!=CSteelMatLibrary::Q235BriefMark())
			spec.Append(pParentRod->cMaterial);
		spec.ResizeLength(9);	//�̶�9���ַ�����G1829X30P

		CXhChar16 symmetry("   ");
		if(elemInfoArr[i]->relaElem.xElemMirX.pElemInfo)
			symmetry[0]='X';
		if(elemInfoArr[i]->relaElem.xElemMirY.pElemInfo)
			symmetry[1]='Y';
		if(elemInfoArr[i]->relaElem.xElemMirZ.pElemInfo)
			symmetry[2]='Z';
		if(symmetry[1]==' ')
		{
			symmetry[1]=symmetry[2];
			symmetry[2]=' ';	//�Գ��ַ����м䲻���пո�
		}
		if(symmetry.Equal("   "))
			symmetry.Copy("N/A");
		CXhChar16 uniformSizeStr,horiRodStr,elemLabel("%d-%d",pStart->point_i,pEnd->point_i);
		if(pParentRod->m_uStatMatNo>0)
			uniformSizeStr.Printf("%d",pParentRod->m_uStatMatNo);
		if(pParentRod->pStart->handle!=elemInfoArr[i]->m_hStartNode||pParentRod->pEnd->handle!=elemInfoArr[i]->m_hEndNode)
			uniformSizeStr.Append(CXhChar16("U%d-%d",pParentRod->pStart->point_i,pParentRod->pEnd->point_i));
		if(uniformSizeStr.Length>0&&uniformSizeStr.Length<12)
			uniformSizeStr.ResizeLength(12);
		CSuperSmartPtr<CLDSPart>pHoriRod=Ta.Parts.FromHandle(pParentRod->CalLenCoef.hHorizPole);
		if(pHoriRod.IsHasPtr()&&pHoriRod->IsLinePart()&&pHoriRod.LinePartPointer()->pStart&&pHoriRod.LinePartPointer()->pEnd)
			horiRodStr.Printf("H%d-%d",pHoriRod.LinePartPointer()->pStart->point_i,pHoriRod.LinePartPointer()->pEnd->point_i);
		if(horiRodStr.Length>0&&horiRodStr.Length<10)
			horiRodStr.ResizeLength(10);
		if(elemLabel.Length<10)
			elemLabel.ResizeLength(10);
		//  Ry0    0*1.1	��С��(��Ԫ����)
		//  Rx0-1990*1.1	ƽ����(����ڼ�)
		//RcY1970-1990*1/3	���ҶԳ�ͬʱ��ѹ(ָ��)
		//RcX1970-1990*2		ǰ��Գ�ͬʱ��ѹ
		//RcH1970-1990*1.1	�ն˵����ͬʱ��ѹ(�������ˣ� 1230-3120U12  1230-3120H
		//Rci1970-1990*1.1	ʼ�˵����ͬʱ��ѹ
		//Rc!1970-1990*1.1	�ն˵����ͬʱ��ѹ
		//Rc:1970-1990*1.1	���˵����ͬʱ��ѹ
		CXhChar50 lamdaStr,lamdaRyoStr,lamdaRxStr,lamdaRcStr;
		CXhChar100 lamdaAllRcStr;
		int lamdaIndex=0;
		double lamdaArr[2]={0};
		CLDSNode *pStartNode,*pEndNode;
		if(elemInfoArr[i]->LoRyoCalType()==0||elemInfoArr[i]->LoRyoCalType()==1)
		{	//������С��
			if(elemInfoArr[i]->LoRyoCalType()==0)
			{
				pStartNode=Ta.Node.FromHandle(elemInfoArr[i]->m_hStartNode);
				pEndNode=Ta.Node.FromHandle(elemInfoArr[i]->m_hEndNode);
				lamdaRyoStr.Append("RyL");	//��Ԫ����
			}
			else if(elemInfoArr[i]->LoRyoCalType()==1)
			{	//ָ���ڼ�
				pStartNode=Ta.Node.FromHandle(elemInfoArr[i]->LoRyo.hStartNode);
				pEndNode=Ta.Node.FromHandle(elemInfoArr[i]->LoRyo.hEndNode);
				long startPointI=0,endPointI=0;
				if(pStartNode)
					startPointI=pStartNode->point_i;
				if(pEndNode)
					endPointI=pEndNode->point_i;
				lamdaRyoStr.Append(CXhChar16("Ry%d-%d",startPointI,endPointI));
			}
			double gyrationRyo=pParentRod->GetWorkR(1);	//��С���ת�뾶,mm
			if(pStartNode&&pEndNode)
				lamdaArr[lamdaIndex]=DISTANCE(pStartNode->Position(false),pEndNode->Position(false))*elemInfoArr[i]->LoRyo.coef/gyrationRyo;
			lamdaIndex++;
			if(elemInfoArr[i]->LoRyo.coef!=1.0)
			{
				CXhChar16 coefStr;
				CExpression::ToExpression(elemInfoArr[i]->LoRyo.coef,3,coefStr,16);
				lamdaRyoStr.Append('*');
				lamdaRyoStr.Append(coefStr);
			}
		}
		if(elemInfoArr[i]->LoRxCalType()==0||elemInfoArr[i]->LoRxCalType()==1)
		{	//����ƽ����
			if(elemInfoArr[i]->LoRxCalType()==0)
			{
				pStartNode=Ta.Node.FromHandle(elemInfoArr[i]->m_hStartNode);
				pEndNode=Ta.Node.FromHandle(elemInfoArr[i]->m_hEndNode);
				lamdaRxStr.Append("RxL");	//��Ԫ����
			}
			else if(elemInfoArr[i]->LoRxCalType()==1)
			{	//ָ���ڼ�
				pStartNode=Ta.Node.FromHandle(elemInfoArr[i]->LoRx.hStartNode);
				pEndNode=Ta.Node.FromHandle(elemInfoArr[i]->LoRx.hEndNode);
				long startPointI=0,endPointI=0;
				if(pStartNode)
					startPointI=pStartNode->point_i;
				if(pEndNode)
					endPointI=pEndNode->point_i;
				lamdaRxStr.Append(CXhChar16("Rx%d-%d",startPointI,endPointI));
			}
			double gyrationRyo=pParentRod->GetWorkR(1);	//��С���ת�뾶,mm
			if(pStartNode&&pEndNode)
				lamdaArr[lamdaIndex]=DISTANCE(pStartNode->Position(false),pEndNode->Position(false))*elemInfoArr[i]->LoRyo.coef/gyrationRyo;
			lamdaIndex++;
			if(elemInfoArr[i]->LoRx.coef!=1.0)
			{
				CXhChar16 coefStr;
				CExpression::ToExpression(elemInfoArr[i]->LoRx.coef,3,coefStr,16);
				lamdaRxStr.Append('*');
				lamdaRxStr.Append(coefStr);
			}
			if(lamdaIndex>1)
				lamdaRxStr.InsertBefore(' ');
		}
		for(int j=0;j<3;j++)
		{
			int calLenType=elemInfoArr[i]->LoCalType(elemInfoArr[i]->smcItems[j].LoRc);
			if(elemInfoArr[i]->smcItems[j].cSMCType==0||calLenType==2)
				continue;
			char smc_arr[6]={'R','Y','X','i','!',':'};	//R��ʾRefer to
			//bool bSmartJustifySmcType=elemInfoArr[i]->cCalLenType<5;
			if(lamdaAllRcStr.Length>0)
				lamdaAllRcStr.Append(' ');
			if(elemInfoArr[i]->smcItems[j].cSMCType&0x80)	//��С��
				lamdaRcStr.Copy("Ry");
			else
				lamdaRcStr.Copy("Rx");
			if(elemInfoArr[i]->smcItems[j].cSMCType==SMC_ITEM::SMC_REFER_MIRX)
				lamdaRcStr.Append('X');
			else if(elemInfoArr[i]->smcItems[j].cSMCType==SMC_ITEM::SMC_REFER_MIRY)
				lamdaRcStr.Append('Y');
			else if(elemInfoArr[i]->smcItems[j].cSMCType==SMC_ITEM::SMC_REFER_START)
				lamdaRcStr.Append('i');
			else if(elemInfoArr[i]->smcItems[j].cSMCType==SMC_ITEM::SMC_REFER_END)
				lamdaRcStr.Append('!');
			else if(elemInfoArr[i]->smcItems[j].cSMCType==SMC_ITEM::SMC_REFER_2ENDS)
				lamdaRcStr.Append(':');
			else if(elemInfoArr[i]->smcItems[j].cSMCType==SMC_ITEM::SMC_REFER_UDF)
				lamdaRcStr.Append('R');
			else
				continue;
			if(calLenType==0)
				lamdaRcStr.Append("L");	//��Ԫ����
			else if(calLenType==1)
			{	//ָ���ڼ�
				pStartNode=Ta.Node.FromHandle(elemInfoArr[i]->smcItems[j].LoRc.hStartNode);
				pEndNode=Ta.Node.FromHandle(elemInfoArr[i]->smcItems[j].LoRc.hEndNode);
				long startPointI=0,endPointI=0;
				if(pStartNode)
					startPointI=pStartNode->point_i;
				if(pEndNode)
					endPointI=pEndNode->point_i;
				lamdaRcStr.Append(CXhChar16("%d-%d",startPointI,endPointI));
			}
			lamdaIndex++;
			if(elemInfoArr[i]->smcItems[j].LoRc.coef!=1.0)
			{
				CXhChar16 coefStr;
				CExpression::ToExpression(elemInfoArr[i]->smcItems[j].LoRc.coef,3,coefStr,16);
				lamdaRcStr.Append('*');
				lamdaRcStr.Append(coefStr);
			}
			lamdaRcStr.ResizeLength(15);
			lamdaAllRcStr.Append(lamdaRcStr);
			if(lamdaIndex>1)
				lamdaAllRcStr.InsertBefore(' ');
		}
		if(elemInfoArr[i]->cCalLenType!=0)
			lamdaStr[0]='0'+elemInfoArr[i]->cCalLenType;
		else
			lamdaStr[0]=' ';
		if(lamdaRyoStr.Length>0&&lamdaRyoStr.Length<16)
			lamdaRyoStr.ResizeLength(16);
		if(lamdaRxStr.Length>0&&lamdaRxStr.Length<16)
			lamdaRxStr.ResizeLength(16);
		//if(lamdaRcStr.Length>0&&lamdaRcStr.Length<16)
		//	lamdaRcStr.ResizeLength(16);
		if(lamdaRyoStr.Length>0)
			lamdaStr.Append(lamdaRyoStr);
		if(lamdaRxStr.Length>0)
			lamdaStr.Append(lamdaRxStr);
		if(lamdaAllRcStr.Length>0)
			lamdaStr.Append(lamdaAllRcStr);
		double holecut=pParentRod->hole_out;
		CXhChar16 holecutStr("%.2f",pParentRod->hole_out);
		SimplifiedNumString(holecutStr);
		if(pParentRod->m_bAutoCalHoleOutCoef)
			holecutStr.InsertBefore(' ');
		else
			holecutStr.InsertBefore('S');
		holecutStr.ResizeLength(5);
		fprintf(fp,"%s %s %s %s %3d %s %s",(char*)elemLabel,(char*)symmetry,pParentRod->layer(),(char*)spec,
			endState,(char*)holecutStr,(char*)lamdaStr); //��ϸ��
		if(horiRodStr.Length>0)	//����������
			fprintf(fp," %s",(char*)horiRodStr);
		//ͳ�Ĺ����˼�
		if(uniformSizeStr.Length>0)
			fprintf(fp," %s",(char*)uniformSizeStr);
		fprintf(fp,"\n");
	}
	fprintf(fp,"\nSECTION CASES �������\n");
	for(CWorkStatus* pCase=Ta.WorkStatus.GetFirst();pCase;pCase=Ta.WorkStatus.GetNext())
	{
		CXhChar16 azimuthStr(pCase->WindLoad.azimuth);
		CXhChar100 lineText;
		azimuthStr.ResizeLength(4);
		lineText.Printf("%6.3f %s %d",pCase->WindLoad.velocity,(char*)azimuthStr,pCase->m_cMainLoadType);//���ϵ��������ϵ������������
		if(pCase->m_cMainLoadType>=5)	//��������
		{
			if(pCase->m_cHoriEarthQuakeAxis==0||pCase->m_cHoriEarthQuakeAxis=='X')
				lineText.Append("-X");
			else if(pCase->m_cHoriEarthQuakeAxis==90||pCase->m_cHoriEarthQuakeAxis=='Y')
				lineText.Append("-Y");
			else
				lineText.Append(CXhChar16("-%d",pCase->m_cHoriEarthQuakeAxis));
		}
		CXhChar16 kesaiStr(" K"),gamaStr(" G");
		kesaiStr.Append(CXhChar16(pCase->Kesai()));
		gamaStr.Append(CXhChar16(pCase->GamaQ()));
		kesaiStr.ResizeLength(5);
		gamaStr.ResizeLength(5);
		lineText.Append(kesaiStr);
		lineText.Append(gamaStr);
		lineText.Append(CXhChar100(" ;%s\n",pCase->description));
		fprintf(fp,(char*)lineText);
	}
	fprintf(fp,"\nSECTION WIRE-LOAD ����(������)����\n");
	for(i=0;i<node_arr.GetSize();i++)
	{
		pNode=node_arr[i];
		bool bLoadNode=false;
		for(pCase=Ta.WorkStatus.GetFirst();pCase;pCase=Ta.WorkStatus.GetNext())
		{
			if(pCase->hashNodeLoad.GetValue(pNode->handle))
			{
				bLoadNode=true;
				break;
			}
		}
		if(bLoadNode)
		{
			bool bFirstStatus=true;
			for(pCase=Ta.WorkStatus.GetFirst();pCase;pCase=Ta.WorkStatus.GetNext())
			{
				CExternalNodeLoad *pNodeLoad=pCase->hashNodeLoad.GetValue(pNode->handle);
				f3dPoint Fxyz=pNodeLoad->Fxyz*pCase->GamaQ()*pCase->Kesai()/1000;	//N->kN
				double permanentFz=pNodeLoad->permanentFz/1000;						//N->kN
				if(pCase->m_cMainLoadType!=4)
				{	//��װ���������ǽṹ��Ҫ��ϵ��
					Fxyz*=Ta.SafetyCoef();
					permanentFz*=Ta.SafetyCoef();
				}
				CXhChar50 lineText;
				if(bFirstStatus)
				{
					CXhChar16 label("P%d",pNode->point_i);
					label.ResizeLength(6,' ',true);
					if(pNodeLoad)
						lineText.Printf("%s %6.3f %6.3f %6.3f %6.3f\n",(char*)label,Fxyz.x,Fxyz.y,Fxyz.z,permanentFz);
					else
						lineText.Printf("%s  0.000  0.000  0.000  0.000\n",(char*)label);
					bFirstStatus=false;
				}
				else
				{
					if(pNodeLoad)
						lineText.Printf("       %6.3f %6.3f %6.3f %6.3f\n",Fxyz.x,Fxyz.y,Fxyz.z,permanentFz);
					else
						lineText.Printf("        0.000  0.000  0.000  0.000\n");
				}
				fprintf(fp,(char*)lineText);
			}
		}
	}
	fprintf(fp,"\nSECTION WIND-LOAD-PANEL ��ѹ��\n");
	for(CWindSegment* pSeg=Ta.WindLoadSeg.GetFirst();pSeg;pSeg=Ta.WindLoadSeg.GetNext())
	{
		char* windSegTypeStr="0PT";	//0.�˵���������;1.��ڵ�������;2.���������
		char* calStyleStr="0XY";	//0.����ʽ����;1.X��ᵣ��ʽ����;2.Y��ᵣ��ʽ����
		char cMirType=0;	//0:�޶Գ�;1:���ң�Y�ᣩ�Գ�;2:ǰ��Z�ᣩ�Գ�
		if(pSeg->cMirType==1||pSeg->cMirType==2)
			cMirType=pSeg->cMirType;
		fprintf(fp,"%C%d%C %4.2f %4.2f %4.2f",windSegTypeStr[pSeg->cType],cMirType,calStyleStr[pSeg->cCalStyle],
			pSeg->BetaZ,pSeg->CqiFront,pSeg->CqiSide);
		if(!pSeg->m_bAutoCalProfileCoef)//ָ����������ϵ��
			fprintf(fp," MuS%4.2f",pSeg->m_fProfileModeCoef);
		if(!pSeg->xProjPlaneOrg.IsZero())
			fprintf(fp," ORG(%s,%s,%s)",(char*)CXhChar16(pSeg->xProjPlaneOrg.x),
			(char*)CXhChar16(pSeg->xProjPlaneOrg.y),(char*)CXhChar16(pSeg->xProjPlaneOrg.z));
		pSeg->StatValidNodeCount(&Ta);
		for(i=0;i<pSeg->ValidNodeCount();i++)	//�����ڵ�
			fprintf(fp," %5d",pSeg->nodePtrArr[i]->point_i);
		fprintf(fp,"\n");
	}
	fclose(fp);
}
#endif
