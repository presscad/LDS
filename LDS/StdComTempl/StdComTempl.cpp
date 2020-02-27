#include "stdafx.h"
#include "afxwin.h"
#include "..\LDS.h"
#include "..\env_def.h"
#include "Deg.h"
#include "IStdComTempl.h"
#include "TubeWalkWayTempl.h"
#include "WireStruComTempl.h"
#include "ExpressionFun.h"
#include "NodeStdComTempl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////////
//��̬�������ص�����
static int RecogMirChar(const char* label_str,char* pciMirTransType=NULL)
{
	const char* iter;
	char ciMirTransType=0,nstr=0;
	for (iter=label_str;*iter!=0;iter++,nstr++) { ; }
	if (nstr>=3&&(label_str[nstr-2]=='|'||label_str[nstr-2]=='-')&&label_str[nstr-1]=='X')
		ciMirTransType='X';
	else if (nstr>=3&&(label_str[nstr-2]=='|'||label_str[nstr-2]=='-')&&label_str[nstr-1]=='Y')
		ciMirTransType='Y';
	else if (nstr>=3&&(label_str[nstr-2]=='|'||label_str[nstr-2]=='-')&&label_str[nstr-1]=='Z')
		ciMirTransType='Z';
	if (pciMirTransType)
		*pciMirTransType=ciMirTransType;
	return nstr;
}
static void LoadParaModelNodeInfo(CParaModelNode *pItem, DWORD key) 
{ 
	pItem->keyLabel = key; 
}
static void LoadParaModelRodInfo(CParaModelRod *pItem,DWORD key) 
{
	pItem->keyLabel=key;
	pItem->m_idPartClsType=CLS_LINETUBE;
}
static void LoadRaraModelPlateInfo(CStdTemplParaPlate *pItem, DWORD key) 
{ 
	pItem->keyLabel = key; 
}
static CStdTemplParaRod* CreateNewParaRod(int idClsType, DWORD key, void* pContext)	//����ص�����
{
	CStdTemplParaRod* pRod;
	switch (idClsType)
	{
	case CLS_LINEANGLE:
		pRod = new CStdTemplParaAngle();
		break;
	case CLS_LINESLOT:
		pRod = new CStdTemplParaSlot();
		break;
	case CLS_LINETUBE:
	default:
		pRod = new CStdTemplParaRod();
		break;
	}
	pRod->keyLabel = key;
	return pRod;
}
static BOOL DeleteParaRod(CStdTemplParaRod *pRod)				//����ص�����
{
	if (pRod->IsAngleObject())
		delete (CStdTemplParaAngle*)pRod;
	else if (pRod->m_idPartClsType == CLS_LINETUBE)
		delete (CStdTemplParaSlot*)pRod;
	else
		delete pRod;
	return TRUE;
}
static CStdComTempl* CreateStdComTmpl(int idGroupType,DWORD key,void* pContext)
{
	CStdComTempl* pNewStdComTmpl=NULL;
	TMPLID tmplid=key;
	if (key==0)
		return NULL;
	if (key==IStdComTempl::STDTMPL_STRU_TUBE_WALKWAY)
	{
		pNewStdComTmpl=new CTubeWalkWayStdComTempl();
		pNewStdComTmpl->InitComponentTempl(key);
	}
	else if (key == IStdComTempl::STDTMPL_STRU_TUBE_WORKBENCH ||
			key == IStdComTempl::STDTMPL_STRU_HOOF_PLATES)
	{
		pNewStdComTmpl = new CStdComTempl();
		pNewStdComTmpl->InitComponentTempl(key);
	}
#ifdef __RAPID_ROUGH_3DMODEL_
	else if (tmplid.ciGroup==0x01)
	{
		pNewStdComTmpl=new CStdComTemplWireStructure();
		pNewStdComTmpl->InitComponentTempl(key);
	}
#endif
	else if (tmplid.ciGroup==0x02)
	{	//���ӵ�ģʽ
		pNewStdComTmpl=new CStdComTemplConnPoint();
		pNewStdComTmpl->InitComponentTempl(key);
	}
	else //if(tmplid.ciGroup==0x03)
	{	//���ӵ���͹���
		pNewStdComTmpl=new CStdComTempl();
		pNewStdComTmpl->InitComponentTempl(key);
	}
	if (pNewStdComTmpl)
		pNewStdComTmpl->SetTmplId(key);
	return pNewStdComTmpl;
};
static BOOL DeleteStdComTmpl(CStdComTempl* pStdComTmpl)
{
	if (pStdComTmpl==NULL)
		return false;
	TMPLID id=pStdComTmpl->GetTmplId();
	//if(id.wiGroupType==0x0102)
	if (id == IStdComTempl::STDTMPL_STRU_TUBE_WALKWAY)
		delete (CTubeWalkWayStdComTempl*)pStdComTmpl;
	else if (id == IStdComTempl::STDTMPL_STRU_TUBE_WORKBENCH ||
		id == IStdComTempl::STDTMPL_STRU_HOOF_PLATES)
		delete pStdComTmpl;
#ifdef __RAPID_ROUGH_3DMODEL_
	//hĿǰ��δ����CArmHeadComTempl��CStdComTemplWireStructure wjh-2019.9.6
	else if (pStdComTmpl->IsWireStruTmpl())//id.ciGroup == 0x01)
		delete (CStdComTemplWireStructure*)pStdComTmpl;
#endif
	else if (id.ciGroup==0x02)	//���ӵ�ģʽ
		delete (CStdComTemplConnPoint*)pStdComTmpl;
	else
		delete pStdComTmpl;
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//TMPLOBJID
TMPLOBJID::TMPLOBJID(DWORD _id,char _ciMirTransType) {
	dwId=_id;
	set_ciMirTransType(_ciMirTransType);
}
char TMPLOBJID::set_ciMirTransType(char ciMirType)
{
	if (ciMirType=='X')
	{	//����λ�ֽ���λ1��ʾX��Գ�
		dwId|=0x80000000;
		dwId&=0xff7fffff;
	}
	else if (ciMirType=='Y')
	{	//���θ�λ�ֽ���λ1��ʾY��Գ�
		dwId|=0x00800000;
		dwId&=0x7fffffff;
	}
	else if (ciMirType=='Z')
		dwId|=0x80800000;	//��λ���θ�λ�ֽ���λ1��ʾZ��Գ�
	else
		dwId&=0x7f7fffff;
	return ciMirType;
}
char TMPLOBJID::get_ciMirTransType()
{
	bool bitX=(this->dwId&0x80000000)>0;
	bool bitY=(this->dwId&0x00800000)>0;
	if (bitX&&!bitY)
		return 'X';
	else if (!bitX&&bitY)
		return 'Y';
	else if (bitX&&bitY)
		return 'Z';
	else
		return 0;
}
bool TMPLOBJID::set_bLongStrId(bool bLongStr)
{
	if (bLongStr)
		this->dwId|=0x00008000;
	else
		this->dwId&=0xffff7fff;
	return bLongStr;
}
bool TMPLOBJID::get_bLongStrId()
{
	return (this->dwId&=0x00008000)>0;
}
UINT TMPLOBJID::set_uiLongStrSerial(UINT uiSerial)
{
	this->dwId&=0xfffffff0;
	this->dwId|=(uiSerial&0x0000000f);
	return (uiSerial&0x0000000f);
}
UINT TMPLOBJID::get_uiLongStrSerial()
{
	return this->dwId&0x0000000f;
}
//////////////////////////////////////////////////////////////////////////
//ListTmplIdNode
ListTmplIdNode::ListTmplIdNode(DWORD hashid/*=0*/,const char* szLabel/*=NULL*/)
{
	id=hashid;
	next=NULL;
	if (szLabel==NULL||szLabel[0]==0)
		memset(label,0,8);
	int nstr=0;
	for (const char* iter=szLabel;*iter!=0&&nstr<8;iter++,nstr++)
		label[nstr]=*iter;
}
ListTmplIdNode::~ListTmplIdNode()
{
	if (next!=NULL)
		delete next;
	next=NULL;
}
bool ListTmplIdNode::IsEqual(const char* szLabel)
{
	for (int j=0;j<8;j++)
	{
		if (label[j]==0&&szLabel[j]!=0)
			return false;
		else if (label[j]!=0&&szLabel[j]==0)
			return false;
		if (label[j]!=szLabel[j])
			return false;
		if (label[j]==0&&szLabel[j]==0)
			return true;
		if (label[j]==szLabel[j])
			continue;
	}
	return true;
}
ListTmplIdNode* ListTmplIdNode::AppendListNode(const char* szLabel)
{
	if (IsEqual(szLabel))
		return this;
	if (next!=NULL)
		return next->AppendListNode(szLabel);
	next=new ListTmplIdNode(id,szLabel);
	next->id=id.uiLongStrSerial+1;
	return next;
}
UINT ListTmplIdNode::MappingId(const char* szLabel)
{
	if (IsEqual(szLabel))
		return this->id;
	if (next!=NULL)
		return next->MappingId(szLabel);
	else
		return 0;
}
///////////////////////////////////////////////////////////////////////////
//CTmplIdManager
CTmplIdManager::CTmplIdManager()
{
}
UINT CTmplIdManager::HashId(const char* id_str,int nidstr/*=0*/)
{
	const char* iter;
	if (nidstr==0)
		for (iter=id_str;*iter!=0;iter++,nidstr++) { ; }
	if (nidstr<=4)
		return KEY4C(id_str);
	UINT keyid=0,ktemp=0;
	for (int i=0;i<nidstr;i+=3)
	{
		ktemp=0;
		int ncopy=i+3>nidstr?nidstr-i:3;
		memcpy(&ktemp,id_str+i,ncopy);
		keyid+=ktemp;
	}
	keyid<<=8;
	keyid+=(BYTE)(nidstr<<4);
	keyid&=0x7f7f7fff;
	TMPLOBJID id=keyid;//|=0x00008000;
	id.bLongStrId=true;
	return id;
}

UINT CTmplIdManager::AddLabel2Id(const char* label_str)
{
	CXhChar16 idstr;
	char ciMirTransType=0;
	int nstr=RecogMirChar(label_str,&ciMirTransType);
	int nidstr=nidstr=ciMirTransType==0?nstr:nstr-2;
	idstr.NCopy(label_str,nidstr);
	TMPLOBJID id=(nidstr<=4)?KEY4C(nidstr):HashId(idstr,nidstr);
	id.ciMirTransType=ciMirTransType;
	if (nidstr<=4)
		return id;
	else
	{
		ListTmplIdNode* pIdNode=idcenter.GetValue(id);
		if (pIdNode==NULL)
			pIdNode=idcenter.SetValue(id,ListTmplIdNode(id,idstr));
		else
			pIdNode=pIdNode->AppendListNode(idstr);
		return pIdNode->id;
	}
}
UINT CTmplIdManager::Label2Id(const char* label_str)
{
	CXhChar16 idstr;
	char ciMirTransType=0;
	int nstr=RecogMirChar(label_str,&ciMirTransType);
	int nidstr=nidstr=ciMirTransType==0?nstr:nstr-2;
	idstr.NCopy(label_str,nidstr);
	TMPLOBJID id=(nidstr<=4)?KEY4C(nidstr):HashId(idstr,nidstr);
	id.ciMirTransType=ciMirTransType;
	if (nidstr<=4)
		return id;
	else
	{
		ListTmplIdNode* pIdNode=idcenter.GetValue(id);
		if (pIdNode==NULL)
			return 0;
		else
			return pIdNode->MappingId(idstr);
	}
}
///////////////////////////////////////////////////////////////////////////////////
CStdComTmplLibrary globalStdComTmplLibrary;
#include "..\Global.h"
static struct INIT_GLOBALVAR {
	INIT_GLOBALVAR (){
		globalVars.RegisterGlobalVar(Global::VAR(4,&globalStdComTmplLibrary));
	}
}InitVars;
CStdComTmplLibrary::CStdComTmplLibrary()
{
	m_bTmplLibraryCompiled=false;
	hashStdTmplPool.CreateNewAtom=CreateStdComTmpl;
	hashStdTmplPool.DeleteAtom=DeleteStdComTmpl;
}
bool CStdComTmplLibrary::GlobalInitialze()
{
	CompileTmplLibrary();
	return true;
}
CStdComTempl* CStdComTmplLibrary::LoadStdComTempl(const char* szStdComTemplId)
{
	return NULL;
}
CStdComTempl* CStdComTmplLibrary::LoadStdComTempl(UINT idStdComTmpl)
{
	CStdComTempl* pStdComTmpl=hashStdTmplPool.GetValue(idStdComTmpl);
	if (idStdComTmpl==0)
		return NULL;
	if (pStdComTmpl==NULL)
	{
		TMPLID tmplid(idStdComTmpl);
		pStdComTmpl=hashStdTmplPool.Add(idStdComTmpl,tmplid.wiGroupType);
	}
	return pStdComTmpl;
}
CStdComTempl* CStdComTmplLibrary::EnumFirstStdFrameTempl()
{
	for (CStdComTempl* pStdComTmpl=hashStdTmplPool.GetFirst();pStdComTmpl;pStdComTmpl=hashStdTmplPool.GetNext())
	{
		TMPLID id=pStdComTmpl->GetTmplId();
		if (id.ciGroup!=0)
			continue;
		return pStdComTmpl;
	}
	return NULL;
}
CStdComTempl* CStdComTmplLibrary::EnumNextStdFrameTempl()
{
	for (CStdComTempl* pStdComTmpl=hashStdTmplPool.GetNext();pStdComTmpl;pStdComTmpl=hashStdTmplPool.GetNext())
	{
		TMPLID id=pStdComTmpl->GetTmplId();
		if (id.ciGroup!=0)
			continue;
		return pStdComTmpl;
	}
	return NULL;
}
CStdComTempl* CStdComTmplLibrary::EnumFirstStdStruTempl()
{
	for (CStdComTempl* pStdComTmpl=hashStdTmplPool.GetFirst();pStdComTmpl;pStdComTmpl=hashStdTmplPool.GetNext())
	{
		TMPLID id=pStdComTmpl->GetTmplId();
		if (id.ciGroup!=1)
			continue;
		return pStdComTmpl;
	}
	return NULL;
}
CStdComTempl* CStdComTmplLibrary::EnumNextStdStruTempl()
{
	for (CStdComTempl* pStdComTmpl=hashStdTmplPool.GetNext();pStdComTmpl;pStdComTmpl=hashStdTmplPool.GetNext())
	{
		TMPLID id=pStdComTmpl->GetTmplId();
		if (id.ciGroup!=1)
			continue;
		return pStdComTmpl;
	}
	return NULL;
}
CStdComTempl* CStdComTmplLibrary::EnumFirstStdNodeTempl(bool bIncConnPoint/*=true*/,bool bIncNodePlate/*=true*/)
{
	for (CStdComTempl* pStdComTmpl=hashStdTmplPool.GetFirst();pStdComTmpl;pStdComTmpl=hashStdTmplPool.GetNext())
	{
		TMPLID id=pStdComTmpl->GetTmplId();
		if ((id.ciGroup==2&&bIncConnPoint)||(id.ciGroup==3&&bIncNodePlate))

			return pStdComTmpl;
	}
	return NULL;
}
CStdComTempl* CStdComTmplLibrary::EnumNextStdNodeTempl(bool bIncConnPoint/*=true*/,bool bIncNodePlate/*=true*/)
{
	for (CStdComTempl* pStdComTmpl=hashStdTmplPool.GetNext();pStdComTmpl;pStdComTmpl=hashStdTmplPool.GetNext())
	{
		TMPLID id=pStdComTmpl->GetTmplId();
		if ((id.ciGroup==2&&bIncConnPoint)||(id.ciGroup==3&&bIncNodePlate))

			return pStdComTmpl;
	}
	return NULL;
}
void CStdComTmplLibrary::InitStdConnPointTmpls()
{
	CStdComTempl* pStdComTmpl;
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_HD3))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_HD3);	//������ᵣ�����ӵ㣨hd_3��
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_HD1))
		pStdComTmpl = LoadStdComTempl(STDTEMPL_CONNPOINT_HD1);	//�ᵣ�����֧���ڲ����ӵ㣨hd_1��(ID:4)
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_HD1A))
		pStdComTmpl = LoadStdComTempl(STDTEMPL_CONNPOINT_HD1A);	//�ᵣ�����֧���ڲ����ӵ㣨hd_1��(ID:4)
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_HD2))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_HD2);	//�ᵣ�����֧��������ӵ㣨hd_2��(ID:5)
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_HD7))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_HD7);	//�ᵣ��ͷ(hd_7)
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_HD4))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_HD4);	//�����ڹյ㴦(hd_4)(ID:9)
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_HD5))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_HD5);	//������յ㴦(hd_5)(ID:10)
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_HD6))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_HD6);	//������յ㴦(hd_6)
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_HD8))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_HD8);	//�ᵣ����(hd_8)
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_HD9))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_HD9);	//�ᵣ����(hd_9)
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_HD10))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_HD10);	//�ᵣ����(hd_10)
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_XH_HD3A))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_XH_HD3A);//���ۺᵣ�����ӵ�4��
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_XH_HD12))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_XH_HD12);//���ۺᵣ�����ӵ�6��
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_XH_HD12_X))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_XH_HD12_X);//���ۺᵣ�����ӵ�6��
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_XH_HD13))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_XH_HD13);//����֧�ܺᵣ�����ӵ�
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_XH_HD14))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_XH_HD14);//����֧�ܺᵣ�����ӵ�
	//����K�ڵ�  CRANK ARM		  
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_ZK14_K1))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_ZK14_K1);	//�Ʊ�èͷ�ᵣ��������K�ڵ㴦(ID:6)
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_ZK14_K2))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_ZK14_K2);//�Ʊ�èͷ�ᵣ��������K�ڵ㴦(ID:6)
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_ZK14_K3))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_ZK14_K3);//�Ʊ�èͷ�ᵣ��������K�ڵ㴦(ID:6)
	//���½ڵ�  SLOPE
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_BP1))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_BP1);	//������Ϊһ�齻���
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_BP2))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_BP2);	//(���Ǹ�)������Ϊһ�鵹K��
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_BP3))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_BP3);	//(��ϽǸ�)������Ϊһ�鵹K��
	//��������K�ڵ� TOWER_BODY
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_SZK))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_SZK);		//zk 1,2,3,4,11,12,13,21,22,23,24;//
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_HK1))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_HK1);		//HK ����K��
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_HK2))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_HK2);		//HK ����K��
	//����ڵ�  CROSS
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_JC1))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_JC1);	//���Ǹ����������֧�ż�
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_JC1_K1))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_JC1_K1);	//���Ǹ����������֧�ż�
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_JC1_K2))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_JC1_K2);	//���Ǹ����������֧�ż�
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_JC2))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_JC2);	//��ϽǸ����������֧�ż�
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_JC3))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_JC3);	//���������֧�ż�
	//���߽ڵ�
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_XH_GX1))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_XH_GX1);	//���ߴ����ƽڵ�(ID:12)
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_XH_GX2))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_XH_GX2);	//���ߴ����ƽڵ�(ID:13)
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_XH_GX3))
		pStdComTmpl = LoadStdComTempl(STDTEMPL_CONNPOINT_XH_GX3);	//���ߴ����ƽڵ�
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_XH_GX4))
		pStdComTmpl = LoadStdComTempl(STDTEMPL_CONNPOINT_XH_GX4);	//���ߴ����ƽڵ�
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_XH_GX5))
		pStdComTmpl = LoadStdComTempl(STDTEMPL_CONNPOINT_XH_GX5);	//���ߴ����ƽڵ�
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_XH_DX1))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_XH_DX1);	//����֧�ܶ˽ڵ�(ID:14)
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_XH_DX2))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_XH_DX2);	//����֧�ܶ˽ڵ�(ID:14)
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_XH_DX3))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_XH_DX3);	//����֧�ܶ˽ڵ�(ID:14)
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_XH_DX3A))
		pStdComTmpl = LoadStdComTempl(STDTEMPL_CONNPOINT_XH_DX3A);	//����֧�ܶ˽ڵ�(ID:14)
	//����
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_XH_DM1))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_XH_DM1);	//���ߵ������ӵ�
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_XH_DM2))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_XH_DM2);	//���ߵ������ӵ�
	//����V�ڵ�
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_TV))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_TV);	//TV
	//�����Ľڵ�   AUXILIARY
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_FK1))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_FK1);//˫�˼����������ӽڵ�
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_FK2))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_FK2);//���˼����������ӽڵ�
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_FK3))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_FK3);//�ĸ˼����������ӽڵ�
	if (CStdComTempl::IsStdTmplAuthorized(STDTEMPL_CONNPOINT_FK4))
		pStdComTmpl=LoadStdComTempl(STDTEMPL_CONNPOINT_FK4);//���ȸ��������ӽڵ�
}
CATLOG_TREEITEM* CStdComTmplLibrary::InsertTreeNodeItem(CATLOG_TREEITEM* pParentItem,UINT idStdComTmpl,const char* szTreeItemName)
{
	if (CStdComTempl::IsStdTmplAuthorized(idStdComTmpl))
	{
		CXhChar50 name = szTreeItemName;
		if (!CStdComTempl::IsStdTmplChecked(idStdComTmpl))
			name.InsertBefore('*');
		CATLOG_TREEITEM* pItem = pParentItem->AppendSonItem(name);
		pItem->pCatlog->idRelaObj = idStdComTmpl;	//EB�ҵ�ģ��
		return pItem;
	}
	return NULL;
}
void CStdComTmplLibrary::CompileTmplLibrary()
{
	if(m_bTmplLibraryCompiled)
		return;
	m_bTmplLibraryCompiled=true;
	CATLOG_TREEITEM::pBelongCatlogLibary=&Ta.hashCatlogs;
	Ta.hashCatlogs.Empty();
	//"�˼�ģ��"����
	CATLOG_ITEM *pFrameCatgGroup=Ta.hashCatlogs.Add(1);
	pFrameCatgGroup->ciType=1;
	pFrameCatgGroup->name="�˼�ģ��";
	xFrameCatlog.pCatlog=pFrameCatgGroup;
	{
		CATLOG_TREEITEM* pCatgTreeItem=NULL;
		pCatgTreeItem=xFrameCatlog.AppendSonItem("HYJ-1");
		pCatgTreeItem->pCatlog->idRelaObj=TMPLID(0x0000,1);
		pCatgTreeItem=xFrameCatlog.AppendSonItem("HX3");
		pCatgTreeItem->pCatlog->idRelaObj=TMPLID(0x0000,2);
	}
	//"�ṹ����"����
	CATLOG_ITEM *pStruCatgGroup =Ta.hashCatlogs.Add(2);
	pStruCatgGroup->name="�ṹ����";
	xStruCatlog.pCatlog=pStruCatgGroup;
	xStruCatlog.pCatlog->ciType=2;
	{
		if (CStdComTempl::IsStdTmplAuthorized(IStdComTempl::STDTMPL_STRU_TUBE_WALKWAY))
			InsertTreeNodeItem(&xStruCatlog,IStdComTempl::STDTMPL_STRU_TUBE_WALKWAY,"�����ߵ���");
		if (CStdComTempl::IsStdTmplAuthorized(IStdComTempl::STDTMPL_STRU_TUBE_WORKBENCH))
			InsertTreeNodeItem(&xStruCatlog,IStdComTempl::STDTMPL_STRU_TUBE_WORKBENCH,"����վλƽ̨��");
		if (CStdComTempl::IsStdTmplAuthorized(IStdComTempl::STDTMPL_STRU_HOOF_PLATES))
			InsertTreeNodeItem(&xStruCatlog,IStdComTempl::STDTMPL_STRU_HOOF_PLATES,"�Ǹ�˫�䵥ţ��");
		CATLOG_TREEITEM* pCatgEarthWireNode=xStruCatlog.AppendSonItem("���߹ҵ�");
		{
			InsertTreeNodeItem(pCatgEarthWireNode, IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM1, "����EB�����ҵ�֧��");
			InsertTreeNodeItem(pCatgEarthWireNode, IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM2, "����EB�������Źҵ�֧��");
			InsertTreeNodeItem(pCatgEarthWireNode, IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM3, "�����Ǻᵣ����EB�������Źҵ�֧��");
			if (pCatgEarthWireNode->GetSonItemCount() <= 0)
				xStruCatlog.DeleteCursor();
		}
		CATLOG_TREEITEM* pCatgConductWireNode=xStruCatlog.AppendSonItem("���߶���");
		{
			//ֱ��������ĵ�EB�ҵ�
			InsertTreeNodeItem(pCatgConductWireNode,IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_EB,"ֱ��������ĵ�EB�ҵ�");
			//ֱ���������˫EB�ҵ�
			InsertTreeNodeItem(pCatgConductWireNode,IStdComTempl::STDTMPL_STRU_META_SECT_BTM_2_EB,"ֱ���������˫EB�ҵ�");
			//����������������浥GD�ҵ�
			InsertTreeNodeItem(pCatgConductWireNode,IStdComTempl::STDTMPL_STRU_META_SECT_FRT_1_GD,"��������������浥GD�ҵ�");
			//������������ĵ��浥GD�ҵ�
			InsertTreeNodeItem(pCatgConductWireNode,IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_GD,"����������ĵ��浥GD�ҵ�");
			if (pCatgConductWireNode->GetSonItemCount()<=0)
				xStruCatlog.DeleteCursor();
		}
		pCatgConductWireNode=xStruCatlog.AppendSonItem("���߼�");
		{
			InsertTreeNodeItem(pCatgConductWireNode,IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM1,"�����Ǻᵣ����EB����I��");
			//�ᵣV��EB�����ҵ�
			InsertTreeNodeItem(pCatgConductWireNode,IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM2,"�ᵣV�浼��EB�����ҵ�");
			//��������˫GD�ҵ�
			InsertTreeNodeItem(pCatgConductWireNode,IStdComTempl::STDTMPL_STRU_NZ_FRT_2_GD,"����������˫GD�ҵ�");
			//���ŵ���˫GD�ҵ�
			InsertTreeNodeItem(pCatgConductWireNode,IStdComTempl::STDTMPL_STRU_NZ_BTM_2_GD,"����������˫GD�ҵ�");
			//���ŵ�����GD�ҵ�
			InsertTreeNodeItem(pCatgConductWireNode,IStdComTempl::STDTMPL_STRU_NZ_BTM_3_GD,"������������GD�ҵ�");
			////ֱ����V�����൥EB�ҵ�
			//pCatgGDWireArm=pCatgConductWireNode->AppendSonItem("ֱ����V�����൥EB�ҵ�");
			//pCatgGDWireArm->pCatlog->idRelaObj=IStdComTempl::STDTMPL_STRU_V_OUTER_1_EB;	//GD�ҵ�ģ��
			////ֱ����V������˫EB�ҵ�
			//pCatgGDWireArm=pCatgConductWireNode->AppendSonItem("ֱ����V������˫EB�ҵ�");
			//pCatgGDWireArm->pCatlog->idRelaObj=IStdComTempl::STDTMPL_STRU_V_OUTER_2_EB;	//GD�ҵ�ģ��
			if (pCatgConductWireNode->GetSonItemCount()<=0)
				xStruCatlog.DeleteCursor();
		}
	}
	//"�ڵ㹹��"����
	CATLOG_ITEM *pNodeCatgGroup =Ta.hashCatlogs.Add(3);
	pNodeCatgGroup->ciType=3;
	pNodeCatgGroup->name="�ڵ㹹��";
	xNodeCatlog.pCatlog=pNodeCatgGroup;
	CATLOG_TREEITEM* pEarthWireNode=xNodeCatlog.AppendSonItem("����֧�ܵ�");
	{
		InsertTreeNodeItem(pEarthWireNode,STDTEMPL_CONNPOINT_XH_DX1,"���߼ܶ˵�1#DX1");
		InsertTreeNodeItem(pEarthWireNode,STDTEMPL_CONNPOINT_XH_DX2,"���߼ܶ˵�2#DX2");
		InsertTreeNodeItem(pEarthWireNode,STDTEMPL_CONNPOINT_XH_DX3,"���Ҹ˽���3#DX3");
		InsertTreeNodeItem(pEarthWireNode,STDTEMPL_CONNPOINT_XH_DX3A,"���Ҹ˽���3#DX3A");
		InsertTreeNodeItem(pEarthWireNode,STDTEMPL_CONNPOINT_XH_DM1,"����ҵ����#DM1");
		InsertTreeNodeItem(pEarthWireNode, STDTEMPL_CONNPOINT_XH_DM2, "����ҵ����#DM2");
		InsertTreeNodeItem(pEarthWireNode, STDTEMPL_CONNPOINT_XH_DM3, "�м�ҵ����#DM3");
		InsertTreeNodeItem(pEarthWireNode, STDTEMPL_CONNPOINT_XH_GX1, "���ߵ�1#GX1");
		InsertTreeNodeItem(pEarthWireNode, STDTEMPL_CONNPOINT_XH_GX2, "���ߵ�2#GX2");
		InsertTreeNodeItem(pEarthWireNode, STDTEMPL_CONNPOINT_XH_GX3, "���ߵ�3#GX3");
		InsertTreeNodeItem(pEarthWireNode, STDTEMPL_CONNPOINT_XH_GX4, "���ߵ�4#GX4");
		//InsertTreeNodeItem(pEarthWireNode, STDTEMPL_CONNPOINT_XH_GX5, "���ߵ�5#GX5");
		if(pEarthWireNode->GetSonItemCount() <=0)
			xNodeCatlog.DeleteCursor();
	}
	CATLOG_TREEITEM* pBodySlopeNode=xNodeCatlog.AppendSonItem("���µ�");
	{
		InsertTreeNodeItem(pBodySlopeNode,STDTEMPL_CONNPOINT_BP1,"��K���µ�(���Ǹ�)#BP1");
		InsertTreeNodeItem(pBodySlopeNode,STDTEMPL_CONNPOINT_BP2,"������µ�#BP2");
		InsertTreeNodeItem(pBodySlopeNode,STDTEMPL_CONNPOINT_BP3,"�Ʊ�(èͷ)6�˼�ƿ��#BP3");
		if (pBodySlopeNode->GetSonItemCount() <= 0)
			xNodeCatlog.DeleteCursor();
	}
	CATLOG_TREEITEM* pVertArmKNode = xNodeCatlog.AppendSonItem("����K��"); 
	{
		InsertTreeNodeItem(pVertArmKNode,STDTEMPL_CONNPOINT_ZK14_K1,"����K��#ZK14_K1");
		InsertTreeNodeItem(pVertArmKNode,STDTEMPL_CONNPOINT_ZK14_K2,"����K��#ZK14_K2");
		InsertTreeNodeItem(pVertArmKNode,STDTEMPL_CONNPOINT_ZK14_K3,"����K��#ZK14_K3");
		if (pVertArmKNode->GetSonItemCount() <= 0)
			xNodeCatlog.DeleteCursor();
	}
	CATLOG_TREEITEM* pCrossNode=xNodeCatlog.AppendSonItem("�����");
	{
		InsertTreeNodeItem(pCrossNode,STDTEMPL_CONNPOINT_JC1,"�����(���Ǹ�)#JC1");
		//InsertTreeNodeItem(pCrossNode,STDTEMPL_CONNPOINT_JC1_K1,"�����(���Ǹ�)#JC1_K1");
		//InsertTreeNodeItem(pCrossNode,STDTEMPL_CONNPOINT_JC1_K2,"�����(���Ǹ�)#JC1_K2");
		InsertTreeNodeItem(pCrossNode,STDTEMPL_CONNPOINT_JC2,"�����(��ϽǸ�)#JC2");
		InsertTreeNodeItem(pCrossNode,STDTEMPL_CONNPOINT_JC3,"�����(֧��)#JC3");
		if (pCrossNode->GetSonItemCount() <= 0)
			xNodeCatlog.DeleteCursor();
	}
	CATLOG_TREEITEM* pBodyKNode=xNodeCatlog.AppendSonItem("����K��");
	{
		InsertTreeNodeItem(pBodyKNode,STDTEMPL_CONNPOINT_SZK,"����K��#SZK");
		InsertTreeNodeItem(pBodyKNode,STDTEMPL_CONNPOINT_HK1,"����K��#HK1");
		InsertTreeNodeItem(pBodyKNode,STDTEMPL_CONNPOINT_HK2,"����K��#HK2");
		if (pBodyKNode->GetSonItemCount() <= 0)
			xNodeCatlog.DeleteCursor();
	}
	CATLOG_TREEITEM* pArmNodeSet=xNodeCatlog.AppendSonItem("�ᵣ��");
	{
		InsertTreeNodeItem(pArmNodeSet, STDTEMPL_CONNPOINT_HD1, "�����ڲ�ᵣ��#HD1");
		InsertTreeNodeItem(pArmNodeSet, STDTEMPL_CONNPOINT_HD1A, "�����ڲ�ᵣ��#HD1A");
		InsertTreeNodeItem(pArmNodeSet, STDTEMPL_CONNPOINT_HD2, "�������ᵣ��#HD2");
		InsertTreeNodeItem(pArmNodeSet,STDTEMPL_CONNPOINT_XH_HD3A,"�����ںᵣ��#HD3A");
		InsertTreeNodeItem(pArmNodeSet, STDTEMPL_CONNPOINT_HD3, "��������ᵣ��#HD3B");
		InsertTreeNodeItem(pArmNodeSet, STDTEMPL_CONNPOINT_HD3_G5, "��������ᵣ��#HD3_G5");//wxc new added  wjh-2019.8.25
		InsertTreeNodeItem(pArmNodeSet, STDTEMPL_CONNPOINT_HD4, "�����ڹյ�#HD4");
		InsertTreeNodeItem(pArmNodeSet, STDTEMPL_CONNPOINT_HD5, "������յ�(��)#HD5");
		InsertTreeNodeItem(pArmNodeSet, STDTEMPL_CONNPOINT_HD6, "������յ�(��)#HD6");
		InsertTreeNodeItem(pArmNodeSet, STDTEMPL_CONNPOINT_HD7, "��ͷ�ᵣ��#HD7");
		//HD8,HD9����ͨ��ͨ�ù���ֱ�����ɣ�û̫���Ҫ�����׼�ڵ� wjh-2019.8.22
		//InsertTreeNodeItem(pArmNodeSet, STDTEMPL_CONNPOINT_HD8, "����ᵣ��#HD8");
		//InsertTreeNodeItem(pArmNodeSet, STDTEMPL_CONNPOINT_HD9, "����ᵣ��#HD9");
		InsertTreeNodeItem(pArmNodeSet, STDTEMPL_CONNPOINT_HD10, "����ᵣ��#HD10");
		InsertTreeNodeItem(pArmNodeSet, STDTEMPL_CONNPOINT_XH_HD12,"�ᵣ�����м����ӵ�#HD12");
		InsertTreeNodeItem(pArmNodeSet, STDTEMPL_CONNPOINT_XH_HD13, "����֧�ܺᵣ��(��)#HD13");
		InsertTreeNodeItem(pArmNodeSet, STDTEMPL_CONNPOINT_XH_HD14,"����֧�ܺᵣ��(��)#HD14");
		if (pArmNodeSet->GetSonItemCount() <= 0)
			xNodeCatlog.DeleteCursor();
	}
	CATLOG_TREEITEM* pLegVNode = xNodeCatlog.AppendSonItem("����V��");
	{
		InsertTreeNodeItem(pLegVNode,STDTEMPL_CONNPOINT_TV,"����V��#TV");
		if (pLegVNode->GetSonItemCount() <= 0)
			xNodeCatlog.DeleteCursor();
	}
	CATLOG_TREEITEM* pAuxiliaryNode=xNodeCatlog.AppendSonItem("�����ĵ�");
	{
		InsertTreeNodeItem(pAuxiliaryNode,STDTEMPL_CONNPOINT_FK1,"�����ĵ�(˫�˼�)#FK1");
		InsertTreeNodeItem(pAuxiliaryNode,STDTEMPL_CONNPOINT_FK2,"�����ĵ�(���˼�)#FK2");
		InsertTreeNodeItem(pAuxiliaryNode,STDTEMPL_CONNPOINT_FK3,"�����ĵ�(�ĸ˼�)#FK3");
		InsertTreeNodeItem(pAuxiliaryNode,STDTEMPL_CONNPOINT_FK4,"���ȸ����ĵ�#FK4");
		if (pAuxiliaryNode->GetSonItemCount() <= 0)
			xNodeCatlog.DeleteCursor();
	}
	InitStdConnPointTmpls();
}
////////////////////////////////////////////////////////////////////////////////
//CStdComTemplExpression
static EXPRESSION_FUNC_NODE_X _locale_expr_funcobj_nodex;
static EXPRESSION_FUNC_NODE_Y _locale_expr_funcobj_nodey;
static EXPRESSION_FUNC_NODE_Z _locale_expr_funcobj_nodez;
static EXPRESSION_FUNC_ABS	  _locale_expr_funcobj_abs;
static EXPRESSION_FUNC_NODE_POS _locale_expr_funcobj_nodepos;
static EXPRESSION_FUNC_NODE_on_ROD_POS _locale_expr_funcobj_nodeonrodpos;
static EXPRESSION_FUNC_ROD_LENV _locale_expr_funcobj_rodlenv;
static EXPRESSION_FUNC_CROSSROD_ACS _locale_expr_funcobj_crossrodacs;
EXPRESSION_FUNC* CStdComTemplExpression::FromFunctionName(const char* func_label)
{
	if (stricmp(func_label,"NODE_X")==0)
		return &_locale_expr_funcobj_nodex;
	else if (stricmp(func_label,"NODE_Y")==0)
		return &_locale_expr_funcobj_nodey;
	else if (stricmp(func_label,"NODE_Z")==0)
		return &_locale_expr_funcobj_nodez;
	else if (stricmp(func_label,"ABS")==0)
		return &_locale_expr_funcobj_abs;
	else if (stricmp(func_label,"Node_Pos")==0)
		return &_locale_expr_funcobj_nodepos;
	else if (stricmp(func_label,"RodNode_Pos")==0)
		return &_locale_expr_funcobj_nodeonrodpos;
	else if (stricmp(func_label,"Rod_LenV")==0)
		return &_locale_expr_funcobj_rodlenv;
	else if (stricmp(func_label,"CrossRodAcs")==0)
		return &_locale_expr_funcobj_crossrodacs;
	/*else if(stricmp(func_label,"Angle_WingNormX")==0)
		return &_locale_expr_funcobj_rodlenv;
	else if(stricmp(func_label,"Angle_WingNormY")==0)
		return &_locale_expr_funcobj_rodlenv;*/
	return NULL;
}
CStdComTemplExpression::CStdComTemplExpression(CStdComTempl* pStdTempl/*=NULL*/,GECS* pAcs/*=NULL*/)
{
	if (pAcs==NULL)
		acs.InitStdCS();
	else
		acs=*pAcs;
	if (pStdTempl)
		InitExpressionVarList(pStdTempl);
}
void CStdComTemplExpression::InitExpressionVarList(CStdComTempl* pStdTempl)
{
	m_pContextStdTempl=pStdTempl;
	for (STDTEMPL_PARAM_ITEM* pParam=pStdTempl->listParams.GetFirst();pParam;pParam=pStdTempl->listParams.GetNext())
	{
		EXPRESSION_VAR* pVar=varList.Append();
		StrCopy(pVar->variableStr,pParam->szVarName,9);
		pVar->fValue=pParam->value.lfSafeVal;
	}
}
//////////////////////////////////////////////////////////////////////////
//CStdTemplParaNode
CStdTemplParaNode::CStdTemplParaNode()
{
	pTagInfo=NULL;
	pRelaNode=NULL;
	m_bNeedUpdateRelaInfo=false;
	/*
		0-�������κ������Ľڵ�;
		1-�˼���X����ֵ�����;
		2-�˼���Y����ֵ�����;
		3-�˼���Z����ֵ�����;
		4-���˼����潻��;
		5-�˼��ϵı����ȷֵ�;
	*/
	ciPosCalType=0;
	strcpy(posx_expression,"0");
	strcpy(posy_expression,"0");
	strcpy(posz_expression,"0");
	strcpy(scale_expression,"");
	strcpy(exist_logicexpr,"");
	ciOffsetType=0;	//Ĭ��ƫ�Ƶ�ƫ����Ϊ�ӳ��ȷ���
	attach_scale=0;	//��ֵ��ʾС����ʽ�ı���λ��ֵ,��ֵ��ʾ����.��ĸ
	m_bNecessaryNode=true;
	m_bOptionExterNode=false;
	memset(sPreferLayer,0,3);
	keyRepeatParam.dwItem=0;	//>0ʱ��ʾ�ýڵ�Ϊ�ڼ��е��ظ��ڵ㣬�ظ�����Ϊ"i"����ֵΪ�ظ��ڵ��������Ľڵ��ظ����ߵı�ʶ
	memset(attachNodeArr,0,sizeof(short)*4);
	wiOffsetByPrevNode=0xffff;
}
CStdTemplParaNode::~CStdTemplParaNode() { ; }
void CStdTemplParaNode::ToBuffer(BUFFER_IO *pIO,DWORD dwVersion/*=0*/)
{
	pIO->WriteBooleanThin(m_bNeedUpdateRelaInfo);
	pIO->WriteBooleanThin(m_bNecessaryNode);
	pIO->WriteBooleanThin(m_bOptionExterNode);
	pIO->Write(sPreferLayer,3);
	pIO->WriteDword(keyLabel);
	pIO->WriteDword(keyLabelFatherRod);
	pIO->WriteString(posx_expression, 50);
	pIO->WriteString(posy_expression, 50);
	pIO->WriteString(posz_expression, 50);
	pIO->WriteString(scale_expression, 50);
	pIO->WriteString(offset_expression, 50);
	pIO->WriteString(exist_logicexpr, 20);
	pIO->WriteDword(keyRepeatParam);
	pIO->WriteByte(ciPosCalType);
	pIO->WriteByte(ciOffsetType);
	pIO->WriteDouble(attach_scale);
	pIO->WriteWord(wiOffsetByPrevNode);
	pIO->WritePoint(xPosition);
	for (int i = 0; i < 4; i++)
		pIO->WriteDword(attachNodeArr[i]);
}
void CStdTemplParaNode::FromBuffer(BUFFER_IO *pIO, DWORD dwVersion/*=0*/)
{
	pIO->ReadBooleanThin(&m_bNeedUpdateRelaInfo);
	pIO->ReadBooleanThin(&m_bNecessaryNode);
	pIO->ReadBooleanThin(&m_bOptionExterNode);
	pIO->Read(sPreferLayer, 3);
	pIO->ReadDword(&keyLabel.dwItem);
	pIO->ReadDword(&keyLabelFatherRod.dwItem);
	pIO->ReadString(posx_expression, 50);
	pIO->ReadString(posy_expression, 50);
	pIO->ReadString(posz_expression, 50);
	pIO->ReadString(scale_expression, 50);
	pIO->ReadString(offset_expression, 50);
	pIO->ReadString(exist_logicexpr, 20);
	pIO->ReadDword(&keyRepeatParam.dwItem);
	pIO->ReadByte(&ciPosCalType);
	pIO->ReadByte(&ciOffsetType);
	pIO->ReadDouble(&attach_scale);
	pIO->ReadWord(&wiOffsetByPrevNode);
	pIO->ReadPoint(xPosition);
	for (int i = 0; i < 4; i++)
		pIO->ReadDword(&attachNodeArr[i].dwId);
}
////////////////////////////////////////////////////////////////////////////////
//CStdTemplParaRod
CStdTemplParaRod::CStdTemplParaRod()
{
	m_idPartClsType = CLS_LINEPART;
	m_bNecessaryRod=true;
	m_bShortAngle = false;
	m_cbNeedUpdatePosition=0;
	bReverseStartEnd=m_bNeedUpdateRelaInfo=false;
	m_bNeedUpdatePosition=m_bNeedUpdateNormal=false;
	pRelaRod=NULL;
	siMinRayAngle=siMaxRayAngle=0;
	wiStartOddCalStyle=wiEndOddCalStyle=0x8002;
	bReverseStartEnd=bReverseWingX_Y=bReverseMasterAngle=false;
	vLocalNearVecS2E.Set(1,0,0);
	cnLiveShareBoltN = 0;
}
CStdTemplParaRod::~CStdTemplParaRod() { ; }
bool CStdTemplParaRod::set_bNeedUpdatePosition(bool bUpdatePos)
{
	if (bUpdatePos)
		m_cbNeedUpdatePosition=0x03;
	else
		m_cbNeedUpdatePosition=0;
	return bUpdatePos;
}
bool CStdTemplParaRod::set_bUpdateStartPos(bool bUpdatePos)
{
	if (bUpdatePos)
		m_cbNeedUpdatePosition|=0x01;
	else
		m_cbNeedUpdatePosition&=0x02;
	return bUpdatePos;
}
bool CStdTemplParaRod::get_bUpdateStartPos() { return (m_cbNeedUpdatePosition & 0x01) > 0; }
bool CStdTemplParaRod::set_bUpdateEndPos(bool bUpdatePos)
{
	if (bUpdatePos)
		m_cbNeedUpdatePosition |= 0x02;
	else
		m_cbNeedUpdatePosition &= 0x01;
	return bUpdatePos;
}
bool CStdTemplParaRod::get_bUpdateEndPos() { return (m_cbNeedUpdatePosition & 0x02) > 0; }
bool CStdTemplParaRod::set_bShortAngle(bool _bShortAngle)
{
	CStdTemplParaAngle* pParaAngle=IsAngleObject()?(CStdTemplParaAngle*)this:NULL;
	if (pParaAngle!=NULL&&(m_bShortAngle=_bShortAngle))
	{
		//�̽Ǹֵ�ʼĩ�˶�λ����һ������ǰ��ã�����Ҫ�����Զ��ж�
		pParaAngle->desStart.cTouchLineType=pParaAngle->desEnd.cTouchLineType=4;
		//�̽Ǹֵ�֫����һ������ǰ��ã�����Ҫ�������¼���
		pParaAngle->m_bNeedUpdateNormal=false;
	}
	if (m_bShortAngle)	//�̽Ǹ�Ĭ���趨Ϊ2:���и˼�(ȱ��ʱ�Զ����)
		m_ciNecessaryRod = 2;
	return m_bShortAngle;
}
bool CStdTemplParaRod::get_bShortAngle(){
	if (this->m_idPartClsType==CLS_LINEANGLE&&
		((CStdTemplParaAngle*)this)->attachangle.idBaseAngle>0)
		m_bShortAngle=true;	//������һ���Ƕ̽Ǹ� wjh-2019.9.11
	return m_bShortAngle;
}
char CStdTemplParaRod::set_ciNecessaryRod(char _ciNecessaryRod)
{
	return (m_ciNecessaryRod = _ciNecessaryRod);
}
char CStdTemplParaRod::get_ciNecessaryRod() {
	//if (this->m_idPartClsType==CLS_LINEANGLE&&
	//	((CStdTemplParaAngle*)this)->attachangle.idBaseAngle>0)
	//	m_bShortAngle=true;	//������һ���Ƕ̽Ǹ� wjh-2019.9.11
	return this->m_ciNecessaryRod;
}
char CStdTemplParaRod::get_ciSectStyle() 
{
	BYTE style=ciSectFlag&0x0f;
	if(style==0)
		return 'L';
	else if(style==1)
		return 'D';
	else if(style==2)
		return 'T';
	else if(style==3)
		return 'X';
	return 'L';	
}
char CStdTemplParaRod::set_ciSectStyle(char ciStyle)
{	//0.��ʾ����ԭ�����ͣ�0='L';1='D';2='T';3='X'��ʾָ���������
	if(ciStyle=='L')
		ciStyle=0;
	else if(ciStyle=='D')
		ciStyle=1;
	else if(ciStyle=='T')
		ciStyle=2;
	else if(ciStyle=='X')
		ciStyle=3;
	ciSectFlag&=0xf0;
	ciSectFlag|=ciStyle&0x0f;
	return ciStyle&0x0f;
}
char CStdTemplParaRod::get_ciMasterIndex() { return ciSectFlag >> 4; }
char CStdTemplParaRod::set_ciMasterIndex(char _ciMasterIndex) 
{
	ciSectFlag&=0x0f;
	ciSectFlag|=(_ciMasterIndex<<4);
	return _ciMasterIndex&0x0f;
}
void CStdTemplParaRod::ToBuffer(BUFFER_IO *pIO, DWORD dwVersion/*=0*/)
{
	pIO->WriteDword(keyLabel);
	pIO->WriteByte(cMaterial);
	pIO->WriteString(sPreferLayer,4);
	pIO->WriteInteger(m_idPartClsType);
	pIO->WriteByte(cbMirCreateFlag);
	pIO->WriteString(exist_logicexpr,20);
	pIO->WriteString(m_sGuige);
	pIO->WriteBooleanThin(m_bShortAngle);
	pIO->WriteBooleanThin(m_bNeedUpdateRelaInfo);
	pIO->WriteBooleanThin(m_bNeedUpdateNormal);
	pIO->WriteByte(m_ciNecessaryRod);
	pIO->WriteByte(m_cbNeedUpdatePosition);
	pIO->WritePoint(xStartPoint);
	pIO->WritePoint(xEndPoint);
	pIO->WritePoint(xMidPoint);
	pIO->WritePoint(vLocalNearVecS2E);
	pIO->WritePoint(vVerifyLenStdVec);
	pIO->WriteWord(wiStartOddCalStyle);
	pIO->WriteWord(wiEndOddCalStyle);
	pIO->WriteWord(siMinRayAngle);
	pIO->WriteWord(siMaxRayAngle);
	pIO->WriteDword(keyStartNode);
	pIO->WriteDword(keyEndNode);
	pIO->WriteDword(keyMidNode);
	pIO->WriteByte(normal.ciNormalCalStyle);
	pIO->WriteByte(normal.ciNormalDirection);
	pIO->WriteWord(normal.xAngleLayout);
	pIO->WriteInteger(normal.uidRefRodOfWorkWing);
	pIO->WriteByte(ciSectFlag);
	pIO->WritePoint(eyeSideNorm);
	pIO->WriteInteger(m_bMajorMat);
	pIO->WriteBooleanThin(bReverseStartEnd);
	pIO->WriteBooleanThin(bReverseWingX_Y);
	pIO->WriteBooleanThin(bReverseMasterAngle);
	pIO->WriteByte(cnLiveShareBoltN);
}
void CStdTemplParaRod::FromBuffer(BUFFER_IO *pIO, DWORD dwVersion/*=0*/)
{
	pIO->ReadDword(&keyLabel.dwItem);
	pIO->ReadByte(&cMaterial);
	pIO->ReadString(sPreferLayer, 4);
	pIO->ReadInteger(&m_idPartClsType);
	pIO->ReadByte(&cbMirCreateFlag);
	pIO->ReadString(exist_logicexpr, 20);
	pIO->ReadString(m_sGuige);
	pIO->ReadBooleanThin(&m_bShortAngle);
	pIO->ReadBooleanThin(&m_bNeedUpdateRelaInfo);
	pIO->ReadBooleanThin(&m_bNeedUpdateNormal);
	pIO->ReadByte(&m_ciNecessaryRod);
	pIO->ReadByte(&m_cbNeedUpdatePosition);
	pIO->ReadPoint(xStartPoint);
	pIO->ReadPoint(xEndPoint);
	pIO->ReadPoint(xMidPoint);
	pIO->ReadPoint(vLocalNearVecS2E);
	pIO->ReadPoint(vVerifyLenStdVec);
	pIO->ReadWord(&wiStartOddCalStyle);
	pIO->ReadWord(&wiEndOddCalStyle);
	pIO->ReadWord(&siMinRayAngle);
	pIO->ReadWord(&siMaxRayAngle);
	pIO->ReadDword(&keyStartNode.dwItem);
	pIO->ReadDword(&keyEndNode.dwItem);
	pIO->ReadDword(&keyMidNode.dwItem);
	pIO->ReadByte(&normal.ciNormalCalStyle);
	pIO->ReadByte(&normal.ciNormalDirection);
	pIO->ReadWord((WORD*)&normal.xAngleLayout);
	pIO->ReadInteger(&normal.uidRefRodOfWorkWing);
	pIO->ReadByte(&ciSectFlag);
	pIO->ReadPoint(eyeSideNorm);
	pIO->ReadInteger(&m_bMajorMat);
	pIO->ReadBooleanThin(&bReverseStartEnd);
	pIO->ReadBooleanThin(&bReverseWingX_Y);
	pIO->ReadBooleanThin(&bReverseMasterAngle);
	pIO->ReadByte(&cnLiveShareBoltN);
}
////////////////////////////////////////////////////////////////////////////
//GROUPANGLE_PROP����������ϽǸ���ʵ��Ǹּ��ͬ����ϵ����
bool GROUPANGLE_PROP::set_bUpdateOddSynState(bool bSynState)
{
	if (bSynState)
		_cbValue|=0x02;
	else
		_cbValue&=0xfd;
	return bSynState;
}
bool GROUPANGLE_PROP::get_bUpdateOddSynState()
{
	return (_cbValue&0x02)>0;
}
bool GROUPANGLE_PROP::set_bSynAnglesOdd(bool bSynState)
{
	_cbValue|=0x02;
	if (bSynState)
		_cbValue|=0x01;
	else
		_cbValue&=0xfe;
	return bSynState;
}
bool GROUPANGLE_PROP::get_bSynAnglesOdd()
{
	if ((_cbValue&0x03)==0x03)
		return true;
	else
		return false;
}
bool GROUPANGLE_PROP::set_bAsynAnglesOdd(bool bAsynState)
{
	_cbValue|=0x02;
	if (bAsynState)
		_cbValue&=0xfe;
	else
		_cbValue|=0x01;
	return bAsynState;
}
bool GROUPANGLE_PROP::get_bAsynAnglesOdd()
{
	if ((_cbValue&0x03)==0x02)
		return true;
	else
		return false;
}
BYTE GROUPANGLE_PROP::set_ciMasterIndex(BYTE ciIndex)
{	//��������ϽǸ�ͬ֫���ʵ���ӽǸ�������
	ciIndex&=0x03;
	_cbValue&=0x0f;
	_cbValue+=(ciIndex<<4);
	return ciIndex;
}
BYTE GROUPANGLE_PROP::get_ciMasterIndex()
{
	BYTE ciIndex=_cbValue&0xf0;
	ciIndex>>=4;
	return ciIndex;
}
//////////////////////////////////////////////////////////////////////////
//CStdTemplParaSlot
CStdTemplParaSlot::CStdTemplParaSlot()
{
	m_idPartClsType=CLS_LINESLOT;
	wiAttachTube=0;
	datum_pos.wiAttachNode=0;
	datum_pos.sect_norm.Set(0,0,1);
	datum_pos.len_offset=0;
	datum_pos.ciNormalStyle = 0;
	datum_pos.ciRotFromEdgeStyle=2;
	datum_pos.ray_angle=0;
}
void CStdTemplParaSlot::ToBuffer(BUFFER_IO *pIO, DWORD dwVersion/*=0*/)
{
	CStdTemplParaRod::ToBuffer(pIO, dwVersion);
	pIO->WriteWord(wiAttachTube);
	pIO->WriteDouble(fLen);
	pIO->WritePoint(datum_pos.sect_norm);
	pIO->WritePoint(datum_pos.ray_vec);
	pIO->WriteDouble(datum_pos.len_offset);
	pIO->WriteDouble(datum_pos.ray_angle);
	pIO->WriteWord(datum_pos.wiAttachNode);
	pIO->WriteByte(datum_pos.ciNormalStyle);
	pIO->WriteByte(datum_pos.ciRotFromEdgeStyle);
}
void CStdTemplParaSlot::FromBuffer(BUFFER_IO *pIO, DWORD dwVersion/*=0*/)
{
	CStdTemplParaRod::FromBuffer(pIO,dwVersion);
	pIO->ReadWord(&wiAttachTube);
	pIO->ReadDouble(&fLen);
	pIO->ReadPoint(datum_pos.sect_norm);
	pIO->ReadPoint(datum_pos.ray_vec);
	pIO->ReadDouble(&datum_pos.len_offset);
	pIO->ReadDouble(&datum_pos.ray_angle);
	pIO->ReadWord(&datum_pos.wiAttachNode);
	pIO->ReadByte(&datum_pos.ciNormalStyle);
	pIO->ReadByte(&datum_pos.ciRotFromEdgeStyle);
}
//////////////////////////////////////////////////////////////////////////
//CStdTemplParaBolt
CStdTemplParaBolt::CStdTemplParaBolt()
{
	id = 0;
	m_idPartClsType=CLS_BOLT;
	m_iWireGroup = 0;
	m_ciHoleType = 1;
	m_hBoltFamily = 1;
	m_wiDiameter = 24;
	m_fHoleIncrement = 1.5;
	cbMirCreateFlag = 0;
}
CStdTemplParaBolt::~CStdTemplParaBolt()
{
	
}
int CStdTemplParaBolt::CalWingOffDist(int niMd, double dfWingW, char* szWingG)
{
	JGZJ jgzj;
	getjgzj(jgzj, dfWingW);
	if(stricmp(szWingG,"g")==0)
		return jgzj.g;
	else if (stricmp(szWingG, "g1") == 0)
		return jgzj.g1;
	else if (stricmp(szWingG, "g2") == 0)
		return jgzj.g2;
	else if (stricmp(szWingG, "g3") == 0)
		return jgzj.g3;
	else
	{	//
		
		return jgzj.g;
	}
}
double CStdTemplParaBolt::get_dfHoleDia() 
{
	return this->m_wiDiameter + m_fHoleIncrement; 
}
double CStdTemplParaBolt::set_dfHoleDia(double diameter) 
{
	m_wiDiameter=(WORD)diameter;
	m_fHoleIncrement=(float)diameter-m_wiDiameter;
	if(m_fHoleIncrement>0&&m_fHoleIncrement<1)
	{
		m_fHoleIncrement+=1;
		m_wiDiameter-=1;
	}
	return diameter;
}
void CStdTemplParaBolt::ToBuffer(BUFFER_IO *pIO,DWORD dwVersion/*=0*/) 
{ 
	pIO->WriteInteger(id);
	pIO->WriteByte(m_ciHoleType);
	pIO->WriteInteger(m_hBoltFamily);
	pIO->WriteWord(m_wiDiameter);
	pIO->WriteFloat(m_fHoleIncrement);
	pIO->WriteInteger(m_iWireGroup);
	pIO->WriteByte(cbMirCreateFlag);
	//��˨���㶨λ����
	pIO->WriteByte(org.ciDatumPointCalStyle);
	pIO->WriteInteger(org.idBaseNode);
	pIO->WriteInteger(org.idBasePart);
	pIO->WriteInteger(org.idCrossRod);
	pIO->WriteInteger(org.idWorkSonJg);
	pIO->WriteByte(org.ciDatumWing);
	pIO->WriteByte(org.ciLenDirect);
	pIO->WriteString(org.szWingG,4);
	pIO->WriteString(org.szCrossWingG,4);
	pIO->WriteByte(org.szZvOffset); 
	pIO->WriteWord(org.siLenOffset);
	//��˨�������
	pIO->WriteByte(normal.ciNormalCalStyle);
	pIO->WriteByte(normal.ciResidePlane);
	pIO->WriteByte(normal.ciNormalDirect);
	pIO->WriteWord(normal.widBasePart);
	//��˨ͨ���ַ���
	for (int i = 0; i < 5; i++)
	{
		pIO->WriteByte(through[i].cItemType);
		pIO->WriteWord(through[i].val);
	}
}
void CStdTemplParaBolt::FromBuffer(BUFFER_IO *pIO,DWORD dwVersion/*=0*/) 
{ 
	pIO->ReadInteger(&id);
	pIO->ReadByte(&m_ciHoleType);
	pIO->ReadInteger(&m_hBoltFamily);
	pIO->ReadWord(&m_wiDiameter);
	pIO->ReadFloat(&m_fHoleIncrement);
	pIO->ReadInteger(&m_iWireGroup);
	pIO->ReadByte(&cbMirCreateFlag);
	//��˨���㶨λ����
	pIO->ReadByte(&org.ciDatumPointCalStyle);
	pIO->ReadInteger(&org.idBaseNode);
	pIO->ReadInteger(&org.idBasePart);
	pIO->ReadInteger(&org.idCrossRod);
	pIO->ReadInteger(&org.idWorkSonJg);
	pIO->ReadByte(&org.ciDatumWing);
	pIO->ReadByte(&org.ciLenDirect);
	pIO->ReadString(org.szWingG, 4);
	pIO->ReadString(org.szCrossWingG, 4);
	pIO->ReadByte(&org.szZvOffset);
	pIO->ReadWord(&org.siLenOffset);
	//��˨�������
	pIO->ReadByte(&normal.ciNormalCalStyle);
	pIO->ReadByte(&normal.ciResidePlane);
	pIO->ReadByte(&normal.ciNormalDirect);
	pIO->ReadWord(&normal.widBasePart);
	//��˨ͨ���ַ���
	for (int i = 0; i < 5; i++)
	{
		pIO->ReadByte(&through[i].cItemType);
		pIO->ReadWord(&through[i].val);
	}
}
////////////////////////////////////////////////////////////////////////////
//CStdTemplParaAngle
CStdTemplParaAngle::CStdTemplParaAngle() {
	m_idPartClsType = CLS_LINEANGLE;
	this->wiGroupAngleSpace = 0;
}
CStdTemplParaAngle::ATTACHANGLE::ATTACHANGLE()
{
	idBaseAngle=idEndTipBaseAngle=0;//�̽ǸֵĶ�λ��׼�Ǹ�
	idBaseNode=0;		//�̽Ǹֵ������������λ�ڵ�
	idEndTipBaseNode=0;	//����ϳ��̽Ǹֵ��ն˶�����λ�ڵ�
	ciLayer=0;	//����λ�ò㼶��0.��ʾ�붨λ�Ǹֽ�����Խ���ʾԽ���
	ciDatumAngleWing='X';	//�ְ�Ķ�λ��׼�ǸֵĹ���֫��'X','Y'��
	ciAssistWing = 0;
	ciNormalOffset=0;	//0.��ʾ�Զ��ж�
	//ciAngleLayout=0;	//�ǸֵĲ��÷�ʽ1,2,3,4;<=0��ʾ��ָ��֫�������������֫���߿�
	ciLenDirection=0;//�̽Ǹ�ʼ->�ս��Ʒ�����ӽ��������ᣩX��Y��Z������Сд��ʾ���᷽��
	siLenStartTipPos=-100;
	siLenEndTipPos=100;
}
char CStdTemplParaAngle::ATTACHANGLE::get_ciFixWorkWing() const
{
	if(ciAngleLayout==0)
	{	//������ʽ��0:����˳��;
		if(ciDatumAngleWing=='x'||ciDatumAngleWing=='X')
			return 'Y';
		else //if(ciDatumAngleWing=='y'||ciDatumAngleWing=='Y')
			return 'X';
	}
	else if(ciAngleLayout==1)
	{	//1:������
		return toupper(ciDatumAngleWing);
	}
	else if(ciAngleLayout==2)
	{	//2.��������;
		if(ciDatumAngleWing=='x'||ciDatumAngleWing=='X')
			return 'Y';
		else //if(ciDatumAngleWing=='y'||ciDatumAngleWing=='Y')
			return 'X';
	}
	else if(ciAngleLayout==3)
	{	//3.��������
		return toupper(ciDatumAngleWing);
	}
	else
		return 0;
}
void CStdTemplParaAngle::ToBuffer(BUFFER_IO *pIO, DWORD dwVersion/*=0*/)
{
	CStdTemplParaRod::ToBuffer(pIO, dwVersion);
	//�Ǹֲ���
	pIO->WriteByte(xFlag);
	pIO->WriteWord(siMinRayDegAngle);
	pIO->WriteWord(siMaxRayDegAngle);
	pIO->WriteByte(cbGroupAngle);
	pIO->WriteWord(wiGroupAngleSpace);
	pIO->WriteByte(cnMinBoltN);
	pIO->WriteByte(cnMaxSingularLineBolts);
	pIO->WriteBooleanThin(dualSideEdge);
	//�����Ǹֲ���
	pIO->WriteDword(attachangle.idBaseAngle);
	pIO->WriteDword(attachangle.idBaseNode);
	pIO->WriteDword(attachangle.idEndTipBaseNode);
	pIO->WriteByte(attachangle.ciLayer);
	pIO->WriteByte(attachangle.ciDatumAngleWing);
	pIO->WriteByte(attachangle.ciAssistWing);
	pIO->WriteByte(attachangle.ciNormalOffset);
	pIO->WriteByte(attachangle.ciAngleLayout);
	pIO->WriteByte(attachangle.ciLenDirection);
	pIO->WriteWord(attachangle.siLenStartTipPos);
	pIO->WriteWord(attachangle.siLenEndTipPos);
	//ʼ�˶�λ����
	pIO->WriteByte(desStart.cTouchLine);
	pIO->WriteByte(desStart.cTouchLineType);
	pIO->WriteByte(desStart.cFaceOffsetDatumLine);
	pIO->WriteWord(desStart.siDatumLenOffset);
	pIO->WriteInteger(desStart.uidTouchLineRod);
	pIO->WriteInteger(desStart.uidBendByRod);
	pIO->WriteByte(desStart.coordCtrlType);
	pIO->WriteByte(desStart.cMainCoordCtrlWing);
	pIO->WritePoint(desStart.face_offset_norm);
	pIO->WriteString(desStart.szXorCtrlWingOffset);
	pIO->WriteString(desStart.szYorOthrWingOffset);
	//�ն˶�λ����
	pIO->WriteByte(desEnd.cTouchLine);
	pIO->WriteByte(desEnd.cTouchLineType);
	pIO->WriteByte(desEnd.cFaceOffsetDatumLine);
	pIO->WriteWord(desEnd.siDatumLenOffset);
	pIO->WriteInteger(desEnd.uidTouchLineRod);
	pIO->WriteInteger(desEnd.uidBendByRod);
	pIO->WriteByte(desEnd.coordCtrlType);
	pIO->WriteByte(desEnd.cMainCoordCtrlWing);
	pIO->WritePoint(desEnd.face_offset_norm);
	pIO->WriteString(desEnd.szXorCtrlWingOffset);
	pIO->WriteString(desEnd.szYorOthrWingOffset);
	//��˨���ò���
	pIO->WriteDword(m_layoutBoltArr.Count);
	for (DWORD i = 0; i < m_layoutBoltArr.Count; i++)
		m_layoutBoltArr[i].ToBuffer(pIO, dwVersion);
}
void CStdTemplParaAngle::FromBuffer(BUFFER_IO *pIO, DWORD dwVersion/*=0*/)
{
	CStdTemplParaRod::FromBuffer(pIO, dwVersion);
	//�Ǹֲ���
	pIO->ReadByte(&xFlag._cbValue);
	pIO->ReadWord(&siMinRayDegAngle);
	pIO->ReadWord(&siMaxRayDegAngle);
	pIO->ReadByte(&cbGroupAngle._cbValue);
	pIO->ReadWord(&wiGroupAngleSpace);
	pIO->ReadByte(&cnMinBoltN);
	pIO->ReadByte(&cnMaxSingularLineBolts);
	pIO->ReadBooleanThin(&dualSideEdge);
	//�����Ǹֲ���
	pIO->ReadDword(&attachangle.idBaseAngle);
	pIO->ReadDword(&attachangle.idBaseNode);
	pIO->ReadDword(&attachangle.idEndTipBaseNode);
	pIO->ReadByte(&attachangle.ciLayer);
	pIO->ReadByte(&attachangle.ciDatumAngleWing);
	pIO->ReadByte(&attachangle.ciAssistWing);
	pIO->ReadByte(&attachangle.ciNormalOffset);
	pIO->ReadByte(&attachangle.ciAngleLayout);
	pIO->ReadByte(&attachangle.ciLenDirection);
	pIO->ReadWord(&attachangle.siLenStartTipPos);
	pIO->ReadWord(&attachangle.siLenEndTipPos);
	//ʼ�˶�λ����
	pIO->ReadByte(&desStart.cTouchLine);
	pIO->ReadByte(&desStart.cTouchLineType);
	pIO->ReadByte(&desStart.cFaceOffsetDatumLine);
	pIO->ReadWord(&desStart.siDatumLenOffset);
	pIO->ReadInteger(&desStart.uidTouchLineRod);
	pIO->ReadInteger(&desStart.uidBendByRod);
	pIO->ReadByte(&desStart.coordCtrlType);
	pIO->ReadByte(&desStart.cMainCoordCtrlWing);
	pIO->ReadPoint(desStart.face_offset_norm);
	pIO->ReadString(desStart.szXorCtrlWingOffset);
	pIO->ReadString(desStart.szYorOthrWingOffset);
	//�ն˶�λ����
	pIO->ReadByte(&desEnd.cTouchLine);
	pIO->ReadByte(&desEnd.cTouchLineType);
	pIO->ReadByte(&desEnd.cFaceOffsetDatumLine);
	pIO->ReadWord(&desEnd.siDatumLenOffset);
	pIO->ReadInteger(&desEnd.uidTouchLineRod);
	pIO->ReadInteger(&desEnd.uidBendByRod);
	pIO->ReadByte(&desEnd.coordCtrlType);
	pIO->ReadByte(&desEnd.cMainCoordCtrlWing);
	pIO->ReadPoint(desEnd.face_offset_norm);
	pIO->ReadString(desEnd.szXorCtrlWingOffset);
	pIO->ReadString(desEnd.szYorOthrWingOffset);
	//��˨���ò���
	DWORD nCount = 0;
	pIO->ReadDword(&nCount);
	m_layoutBoltArr.ReSize(nCount);
	for (DWORD i = 0; i < nCount; i++)
		m_layoutBoltArr[i].FromBuffer(pIO, dwVersion);
}
//////////////////////////////////////////////////////////////////////////
//CStdTemplParaPlate
CStdTemplParaPlate::DESIGN_INFO::BENDPLANE::BENDPLANE()
{
	ciBendMode=0;	//0.ָ�������淨��;1.ָ��ָ����Ƕ�
	biSelfCalBendline = FALSE;
	ciBendEdgeWing=0;	//'X','x','Y','y':��д��ʾ�����߲࣬Сд��֫���
	widAngleBendBy=0;	//��һ��������������߶�λ��׼�Ǹ֣���ְ巨���ڲ���Ϊ���������Ϊ������
	siBendlineSpace=10;	//��������Ǹ���Ե�߼�ļ�϶
	normal.vBendplaneNormal.Set(0,0,0);
	//	float fBendDegAngle;	//����ǣ��������߻�׼�Ǹ�ʼ->��Ϊ����ʱ����תΪ��(�Ƕ��ƣ�0��ʾ�Զ�����)
}
CStdTemplParaPlate::CStdTemplParaPlate()
{
	_thick = 6;
	design.bend.normal.ciDirectRefAxisXorY=0;
	design.bend2.normal.ciDirectRefAxisXorY=0;
}
CStdTemplParaPlate::~CStdTemplParaPlate() { ; }
void CStdTemplParaPlate::ToBuffer(BUFFER_IO *pIO, DWORD dwVersion/*=0*/)
{
	//������Ϣ
	pIO->WriteDword(keyLabel);
	pIO->WriteByte(cMaterial);
	pIO->WriteString(sPreferLayer, 4);
	pIO->WriteInteger(m_idPartClsType);
	pIO->WriteString(exist_logicexpr, 20);
	pIO->WriteWord(wiType);
	pIO->WriteInteger(_thick);
	pIO->WriteInteger(_bStdPart);
	pIO->WriteInteger(feature);
	pIO->WriteString(off_expression,50);
	pIO->WriteDword(keyAssistPart);
	pIO->WriteDword(keyAssistPart2);
	pIO->WriteByte(cbMirCreateFlag);
	//��Ʋ���
	pIO->WriteInteger(design.idBaseRod);
	pIO->WriteInteger(design.idBaseNode);
	pIO->WriteInteger(design.idBasePlate);
	pIO->WriteInteger(design.idAssistPart);
	pIO->WriteByte(design.ciDatumAngleWing);
	pIO->WriteByte(design.ciDatumLenAxis);
	pIO->WriteByte(design.ciProfileStyle);
	for (int i = 0; i < 2; i++)
	{
		pIO->WriteByte(design.bends[i].ciBendMode);
		pIO->WriteByte(design.bends[i].ciBendEdgeWing);
		pIO->WriteByte(design.bends[i].biSelfCalBendline);
		pIO->WriteWord(design.bends[i].widAngleBendBy);
		pIO->WriteWord(design.bends[i].siBendlineSpace);
		pIO->WriteDword(design.bends[i].normal.idBendPlaneByAngle);
		pIO->WriteByte(design.bends[i].normal.cDatumWingXorY);
	}
	//���Ӹ˼���Ϣ
	pIO->WriteDword(connrods.Count);
	for (DWORD i = 0; i < connrods.Count; i++)
		connrods[i].ToBuffer(pIO,dwVersion);
}
void CStdTemplParaPlate::FromBuffer(BUFFER_IO *pIO, DWORD dwVersion/*=0*/)
{
	//������Ϣ
	pIO->ReadDword(&keyLabel.dwItem);
	pIO->ReadByte(&cMaterial);
	pIO->ReadString(sPreferLayer, 4);
	pIO->ReadInteger(&m_idPartClsType);
	pIO->ReadString(exist_logicexpr, 20);
	pIO->ReadWord(&wiType);
	pIO->ReadInteger(&_thick);
	pIO->ReadInteger(&_bStdPart);
	pIO->ReadInteger(&feature);
	pIO->ReadString(off_expression, 50);
	pIO->ReadDword(&keyAssistPart.dwItem);
	pIO->ReadDword(&keyAssistPart2.dwItem);
	pIO->ReadByte(&cbMirCreateFlag);
	//��Ʋ���
	pIO->ReadInteger(&design.idBaseRod);
	pIO->ReadInteger(&design.idBaseNode);
	pIO->ReadInteger(&design.idBasePlate);
	pIO->ReadInteger(&design.idAssistPart);
	pIO->ReadByte(&design.ciDatumAngleWing);
	pIO->ReadByte(&design.ciDatumLenAxis);
	pIO->ReadByte(&design.ciProfileStyle);
	for (int i = 0; i < 2; i++)
	{
		pIO->ReadByte(&design.bends[i].ciBendMode);
		pIO->ReadByte(&design.bends[i].ciBendEdgeWing);
		pIO->ReadByte(&design.bends[i].biSelfCalBendline);
		pIO->ReadWord(&design.bends[i].widAngleBendBy);
		pIO->ReadWord(&design.bends[i].siBendlineSpace);
		pIO->ReadDword(&design.bends[i].normal.idBendPlaneByAngle);
		pIO->ReadByte(&design.bends[i].normal.cDatumWingXorY);
	}
	//���Ӹ˼���Ϣ
	DWORD nCount = 0;
	pIO->ReadDword(&nCount);
	connrods.ReSize(nCount);
	for (DWORD i = 0; i < nCount; i++)
		connrods[i].FromBuffer(pIO, dwVersion);
}
//////////////////////////////////////////////////////////////////////////
//BOLT_ORIGIN
int BOLT_ORIGIN::CalWidthByStyleG(int niMd,double dfWidth,BYTE ciStyleG/*=0*/)
{
	JGZJ jgzj;
	getjgzj(jgzj,dfWidth);
	if (ciStyleG==POS_G)
		return jgzj.g;
	else if (ciStyleG==POS_G1)
		return jgzj.g1;
	else if (ciStyleG==POS_G2)
		return jgzj.g2;
	else if (ciStyleG==POS_G3)
		return jgzj.g3;
	else {
		int S=20,S2=20;
		if (niMd<=16)
		{
			S =20;
			S2=25;
		}
		else if (niMd<=20)
		{
			S =25;
			S2=30;
		}
		else if (niMd<=24)
		{
			S =30;
			S2=40;
		}
		else
		{
			S =40;
			S2=50;
		}
		if (ciStyleG==POS_WdS)
			return ftoi(dfWidth)-S2;
		else if (ciStyleG==POS_nS)
			return -S;
		else if (ciStyleG==POS_WaS)
			return ftoi(dfWidth)+S;
		else
			return jgzj.g;
	}
}
//////////////////////////////////////////////////////////////////////////
//BOLT_LOCATION
void BOLT_LOCATION::WriteParamToBolt(CLDSBolt* pDestBolt,CLDSLineAngle* pDatumAngle,CLDSNode* pBaseNode/*=NULL*/)
{
	pDestBolt->des_base_pos.hPart=pDatumAngle->handle;
	double datumpoint_offset_g=0;
	JGZJ jgzj=pDatumAngle->GetZhunJu(ciDatumWing);
	if (basepoint.ciPosCalStyle==0)
	{
		pDestBolt->des_base_pos.datumPoint.datum_pos_style=1;
		pDestBolt->des_base_pos.datumPoint.des_para.RODEND.hRod=pDatumAngle->handle;
		pDestBolt->des_base_pos.datumPoint.des_para.RODEND.direction=ciDirection;
		pDestBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=ciDatumWing-'X';//ciWorkWingX0Y1;
		pDestBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist=0;
		pDestBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=4;
		pDestBolt->des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect=true;
	}
	else if (basepoint.ciPosCalStyle==1)
	{
		pDestBolt->des_base_pos.datumPoint.datum_pos_style=2;
		pDestBolt->des_base_pos.datumPoint.des_para.RODNODE.hRod=pDatumAngle->handle;
		pDestBolt->des_base_pos.datumPoint.des_para.RODNODE.direction=ciDirection;
		pDestBolt->des_base_pos.datumPoint.des_para.RODNODE.hNode=pBaseNode->handle;
		pDestBolt->des_base_pos.datumPoint.des_para.RODNODE.offset_wing=ciDatumWing-'X';//ciWorkWingX0Y1;
		pDestBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style=0;
		datumpoint_offset_g=jgzj.g;
	}
	pDestBolt->des_base_pos.direction=ciDirection;
	pDestBolt->des_base_pos.len_offset_dist=siLenOffset;
	pDestBolt->des_base_pos.wing_offset_dist=siWingOffset-datumpoint_offset_g;
	pDestBolt->des_base_pos.m_biWingOffsetStyle=4;
	if (pDestBolt->des_base_pos.wing_offset_dist==jgzj.g1&&jgzj.g1>0)
		pDestBolt->des_base_pos.m_biWingOffsetStyle=1;
	else if (pDestBolt->des_base_pos.wing_offset_dist==jgzj.g2&&jgzj.g2>0)
		pDestBolt->des_base_pos.m_biWingOffsetStyle=2;
	else if (pDestBolt->des_base_pos.wing_offset_dist==jgzj.g&&jgzj.g>0)
		pDestBolt->des_base_pos.m_biWingOffsetStyle=0;

	pDestBolt->des_base_pos.offset_wing=ciDatumWing-'X';//ciWorkWingX0Y1;
	pDestBolt->des_work_norm.norm_style=1;
	pDestBolt->des_work_norm.hVicePart=pDatumAngle->handle;
	pDestBolt->des_work_norm.norm_wing=pDestBolt->des_base_pos.offset_wing;
}
bool BOLT_LOCATION::TestCalBoltPosition(CLDSLineAngle* pDatumAngle,CLDSNode* pBaseNode/*=NULL*/)
{
	CLDSBolt bolt(&Ta);
	this->WriteParamToBolt(&bolt,pDatumAngle,pBaseNode);
	if (!bolt.correct_pos())
		return false;
	xTempPos=bolt.ucs.origin;
	return true;
}
//////////////////////////////////////////////////////////////////////////
//BOLT_LAYOUT
BOLT_LAYOUT::BOLT_LAYOUT(char _ciLayoutStyle /*= 0*/, char _cnCount /*= 0*/, char _ciRows /*= 0*/)
{
	blPreferBerSide = true;
	ciDatumWing = 0;
	ciTipS0E1 = 0;
	ciFlowDirection = FLOW_StoE;
	ciLayoutStyle = _ciLayoutStyle;
	cnCount = _cnCount;
	ciRows = _ciRows;
	cnMaxSingularLineBolts = 4;
	memset(szLenOffsetOf1stBolt, 0, 8);	//������㣨Ĭ��0��������ձ�ʾĬ��Ϊ��׼�˾�
}
char BOLT_LAYOUT::get_ciSafeDirection()
{
	if (ciFlowDirection==FLOW_StoE)
		return 0;
	else if (ciFlowDirection==FLOW_EtoS)
		return 1;
	else if (ciTipS0E1==1)
		return 1;
	else //if(ciTipS0E1==0)
		return 0;
}
BYTE BOLT_LAYOUT::MAX_SINGULARLINE_BOLTS=5;
UINT BOLT_LAYOUT::LayoutBolts(long giarr[4],IXhList<BOLT_LOCATION>* pListBoltLocations,
	double fEndSpace,double fBaseSpace,double fDoubleRowSpace)
{
	short g[4]={ (short)giarr[0],(short)giarr[1],(short)giarr[2],(short)giarr[3] };
	return LayoutBolts(g,pListBoltLocations,fEndSpace,fBaseSpace,fDoubleRowSpace);
}
UINT BOLT_LAYOUT::LayoutBolts(short g[4],IXhList<BOLT_LOCATION>* pListBoltLocations,double fEndSpace,double fBaseSpace,double fDoubleRowSpace)
{
	int rows=ciRows;
	if (ciRows==0&&g[1]!=g[2])
		rows=cnCount>cnMaxSingularLineBolts?2:1;
	else if (g[1]==0||g[2]==0)
		rows=1;
	BOLT_LOCATION location,*pLocation,*pPrevLocation=NULL;
	short g1ng2=g[1]+g[2];
	location.basepoint=basepoint;
	location.ciDatumWing=this->ciDatumWing;
	location.ciDirection=this->ciSafeDirection;
	pPrevLocation=pLocation=pListBoltLocations->Append(location);
	short siLenOffsetOf1stBolt=0;
	if (stricmp(szLenOffsetOf1stBolt,"LD")==0)
		siLenOffsetOf1stBolt=ftoi(fDoubleRowSpace);
	else if (stricmp(szLenOffsetOf1stBolt,"LS")==0)
		siLenOffsetOf1stBolt=ftoi(fBaseSpace);
	else if (stricmp(szLenOffsetOf1stBolt,"LE")==0)
		siLenOffsetOf1stBolt=ftoi(fEndSpace);
	else if (stricmp(szLenOffsetOf1stBolt,"L0")==0)
	{
		if (ciLayoutStyle!=1)
		{	//(ciLayoutStyle==0||ciLayoutStyle==2)
			siLenOffsetOf1stBolt=ftoi(fEndSpace);
			if (!blPreferBerSide)
				siLenOffsetOf1stBolt+=ftoi(fBaseSpace/2);
		}
		else
			siLenOffsetOf1stBolt=0;
	}
	else
	{
		CExpression expr;
		EXPRESSION_VAR* pVar=expr.varList.Append();
		memcpy(pVar->variableStr,"LS",3);
		pVar->fValue=fBaseSpace;
		pVar=expr.varList.Append();
		memcpy(pVar->variableStr,"LD",3);
		pVar->fValue=fDoubleRowSpace;
		pVar=expr.varList.Append();
		memcpy(pVar->variableStr,"LE",3);
		pVar->fValue=fEndSpace;
		if (szLenOffsetOf1stBolt[0]==0)	//˫��ԳƲ��û���������߽��㶨λʱĬ������˨ƫ����Ϊ0
			siLenOffsetOf1stBolt=(basepoint.ciPosCalStyle==3||this->ciLayoutStyle==1)?0:ftoi(fEndSpace);
		else
			siLenOffsetOf1stBolt=ftoi(expr.SolveExpression(szLenOffsetOf1stBolt));
	}
	if (ciLayoutStyle!=1)
	{	//���಼��(ciLayoutStyle==0||ciLayoutStyle==2)
		pLocation->siLenOffset=siLenOffsetOf1stBolt;
		if (rows==2)
		{
			if (blPreferBerSide)
				pLocation->siWingOffset=g[1];	//g1
			else
				pLocation->siWingOffset=g[2];	//g2
		}
		else
		{
			pLocation->siWingOffset=g[0];
			if (!blPreferBerSide)
				pLocation->siLenOffset+=(short)(fBaseSpace/2);
		}

		for (UINT i=1;i<this->cnCount;i++)
		{
			pLocation=pListBoltLocations->Append(*pPrevLocation);
			if (rows==2)
			{
				int boltspace=ftoi(0.5*fDoubleRowSpace);
				pLocation->siLenOffset+=(short)(boltspace+0.5);
				pLocation->siWingOffset=g1ng2-pPrevLocation->siWingOffset;
			}
			else
				pLocation->siLenOffset+=(short)(fBaseSpace+0.5);
			pPrevLocation=pLocation;
		}
		if (ciLayoutStyle==2&&(location.ciDatumWing=='X'||location.ciDatumWing=='Y'))
		{	//���ý�ͷ������һ֫����˨ wjh-2019.10.8
			pPrevLocation=pLocation=pListBoltLocations->Append(location);
			pLocation->ciDatumWing=location.ciDatumWing=='X'?'Y':'X';
			if (rows==2)
			{
				pLocation->siLenOffset=siLenOffsetOf1stBolt;
				if (!blPreferBerSide)
					pLocation->siWingOffset=g[1];	//g1
				else
					pLocation->siWingOffset=g[2];	//g2
			}
			else
			{
				pLocation->siWingOffset=g[0];
				pLocation->siLenOffset=siLenOffsetOf1stBolt;
				if (blPreferBerSide)
					pLocation->siLenOffset+=ftoi(fBaseSpace/2);
			}

			for (UINT i=1;i<this->cnCount;i++)
			{
				pLocation=pListBoltLocations->Append(*pPrevLocation);
				if (rows==2)
				{
					int boltspace=ftoi(0.5*fDoubleRowSpace);
					pLocation->siLenOffset+=(short)(boltspace+0.5);
					pLocation->siWingOffset=g1ng2-pPrevLocation->siWingOffset;
				}
				else
					pLocation->siLenOffset+=(short)(fBaseSpace+0.5);
				pPrevLocation=pLocation;
			}
		}
	}
	else if (this->ciLayoutStyle==1)
	{	//˫��ԳƲ���
		short length,boltspace=(short)(fBaseSpace+0.5);
		if (rows==2)
		{
			boltspace=ftoi(0.5*fDoubleRowSpace);
			length=ftoi((cnCount/2)*fDoubleRowSpace);
			if (cnCount%2==1)	//�϶���һ����˨��Ϊ����
				pLocation->siLenOffset=siLenOffsetOf1stBolt;
			else
				length-=ftoi(fDoubleRowSpace/2);
			if (blPreferBerSide)
				pPrevLocation->siWingOffset=g[1];	//g1
			else
				pPrevLocation->siWingOffset=g[2];	//g2
		}
		else
		{
			length=ftoi((cnCount-1)*fBaseSpace);
			pPrevLocation->siWingOffset=g[0];
		}
		pPrevLocation->siLenOffset=(short)(-length*0.5)+siLenOffsetOf1stBolt;
		for (UINT i=1;i<cnCount;i++)
		{
			pLocation=pListBoltLocations->Append(*pPrevLocation);
			if (rows==2)
				pLocation->siWingOffset=g1ng2-pPrevLocation->siWingOffset;
			pLocation->siLenOffset+=boltspace;
			pPrevLocation=pLocation;
		}
	}
	return cnCount;
}
void BOLT_LAYOUT::ToBuffer(BUFFER_IO *pIO, DWORD dwVersion /*= 0*/)
{
	pIO->WriteByte(ciBoldMd);
	pIO->WriteByte(cnCount);
	pIO->WriteByte(ciRows);
	pIO->WriteByte(cnMaxSingularLineBolts);
	pIO->WriteInteger(idDirectConnectToAngle1);
	pIO->WriteInteger(idDirectConnectToAngle2);
	pIO->WriteByte(ciLayoutStyle);
	pIO->WriteBooleanThin(blPreferBerSide);
	pIO->WriteByte(ciDatumWing);
	pIO->WriteByte(ciTipS0E1);
	pIO->WriteByte(ciFlowDirection);
	pIO->WriteString(szLenOffsetOf1stBolt,8);
	//���㶨λ��ʽ
	pIO->WriteByte(basepoint.ciPosCalStyle);
	pIO->WriteByte(basepoint.ciPlaneType);
	pIO->WriteByte(basepoint.ciStyleG);
	pIO->WriteByte(basepoint.ciStyleCrossG);
	pIO->WriteDword(basepoint.widBaseNodeOrBolt);
	pIO->WriteInteger(basepoint.widBaseAngle);
	pIO->WriteInteger(basepoint.widCrossAngle);
}
void BOLT_LAYOUT::FromBuffer(BUFFER_IO *pIO, DWORD dwVersion /*= 0*/)
{
	pIO->ReadByte(&ciBoldMd);
	pIO->ReadByte(&cnCount);
	pIO->ReadByte(&ciRows);
	pIO->ReadByte(&cnMaxSingularLineBolts);
	pIO->ReadInteger(&idDirectConnectToAngle1.uidRod);
	pIO->ReadInteger(&idDirectConnectToAngle2.uidRod);
	pIO->ReadByte(&ciLayoutStyle);
	pIO->ReadBooleanThin(&blPreferBerSide);
	pIO->ReadByte(&ciDatumWing);
	pIO->ReadByte(&ciTipS0E1);
	pIO->ReadByte(&ciFlowDirection);
	pIO->ReadString(szLenOffsetOf1stBolt, 8);
	//���㶨λ��ʽ
	pIO->ReadByte(&basepoint.ciPosCalStyle);
	pIO->ReadByte(&basepoint.ciPlaneType);
	pIO->ReadByte(&basepoint.ciStyleG);
	pIO->ReadByte(&basepoint.ciStyleCrossG);
	pIO->ReadDword(&basepoint.widBaseNodeOrBolt);
	pIO->ReadInteger(&basepoint.widBaseAngle.uidRod);
	pIO->ReadInteger(&basepoint.widCrossAngle.uidRod);
}
/////////////////////////////////////////////////////////////////////////
//CONNROD_FLAG
bool CONNROD_FLAG::set_bInheritWorkWing(bool bFilterVal)
{
	_cbValue&=0xfe;
	if (bFilterVal)
		_cbValue|=0x01;
	return bFilterVal;
}
bool CONNROD_FLAG::set_bFilterWorkWing(bool bFilterVal)
{
	_cbValue&=0xfd;
	if (bFilterVal)
		_cbValue|=0x02;
	return bFilterVal;
}
bool CONNROD_FLAG::set_bFilterInsideAngle(bool bFilterVal)
{
	_cbValue&=0xfb;
	if (bFilterVal)
		_cbValue|=0x04;
	return bFilterVal;
}
bool CONNROD_FLAG::set_bFilterSectType(bool bFilterVal)
{
	_cbValue&=0xf7;
	if (bFilterVal)
		_cbValue|=0x08;
	return bFilterVal;
}
bool CONNROD_FLAG::set_bFilterConnBoltN(bool bFilterVal)
{
	_cbValue&=0xef;
	if (bFilterVal)
		_cbValue|=0x10;
	return bFilterVal;
}
//��ǰ�˼�Ϊˮƽ����Ƿ�Ϊɸѡ����
bool CONNROD_FLAG::set_bFilterHoriPrimaryRod(bool bFilterVal)
{
	_cbValue&=0xdf;
	if (bFilterVal)
		_cbValue|=0x20;
	return bFilterVal;
}
/////////////////////////////////////////////////////////////////////////
//CONNECT_ROD
CONNECT_ROD::CONNECT_ROD(short idRod/*=0*/)
{
	sidRod=idRod;
	ciCmpstSerial=0;
	//idCrossBoltAngle=0;
	ciConnectWing='X';
	dynaRod.siRayMinDegAngle=dynaRod.siRayMaxDegAngle=0;
	dynaRod.sidStartNode=dynaRod.sidEndNode=0;
	ciNecessity=0;
	blCheckAngleConnWing=false;
	ciCurrTipSideS0E1=0;
	ciPlaneNo=1;
	cbSpaceFlag=0;	//0x1B
	bEndLjJg=false;
	ciOddCalStyle=0;
	sidOddRefRod[0]=sidOddRefRod[1]=0;
	dualSideEdge=false;
	ciBoltNCalMode=1;//blCalBoltNByRayRodsOnBaseRod=true;	//��Ϊ��׼�Ǹ�ʱ�������߽Ǹ����¼�����˨��
	cnMinBoltN=2;	//����������˨��
	cnMinShareBoltN = 0;
	cnMaxSingularLineBolts=4;
}
void CONNECT_ROD::InitTipSpecOddment(char ciTipS0E1Else2,short siSpecifyOddment/*=0*/)
{
	ciCurrTipSideS0E1=ciTipS0E1Else2;
	ciOddCalStyle=2;	//ָ������ͷֵ
	siSpecOddment=siSpecifyOddment;	//sidOddRefRod[0]=0;
	sidOddRefRod[1]=0;
}
void CONNECT_ROD::InitTipInfo(char ciTipS0E1Else2,char ciOddStyle/*=0*/,
						short sidOddRefRod1/*=1*/,short sidOddRefRod2/*=0*/)
{
	ciCurrTipSideS0E1=ciTipS0E1Else2;
	ciOddCalStyle=ciOddStyle;
	sidOddRefRod[0]=sidOddRefRod1;
	sidOddRefRod[1]=sidOddRefRod2;
}
void CONNECT_ROD::InitBasicInfo(int idRod,bool bNecessity,BYTE ciFaceNo,
	int nMinBoltN,bool bDualSideEdge /*= false*/)
{
	this->idRod.uidRod = idRod;
	ciNecessity = bNecessity ? 1 : 2;
	ciPlaneNo = ciFaceNo;
	cnMinBoltN = nMinBoltN;
	dualSideEdge = bDualSideEdge;
	ciConnectWing = 0;
}
void CONNECT_ROD::ToBuffer(BUFFER_IO *pIO, DWORD dwVersion /*= 0*/)
{
	pIO->WriteInteger(idRod);
	pIO->WriteByte(ciCmpstSerial);
	pIO->WriteByte(ciNecessity);
	pIO->WriteBooleanThin(blCheckAngleConnWing);
	pIO->WriteByte(ciCurrTipSideS0E1);
	pIO->WriteByte(ciConnectWing);
	pIO->WriteByte(ciPlaneNo);
	pIO->WriteBooleanThin(dualSideEdge);
	pIO->WriteByte(cnMinShareBoltN);
	pIO->WriteBooleanThin(bEndLjJg);
	pIO->WriteByte(ciOddCalStyle);
	pIO->WriteWord(sidOddRefRod[0]);
	pIO->WriteWord(sidOddRefRod[1]);
	pIO->WriteWord(dynaRod.sidStartNode);
	pIO->WriteWord(dynaRod.sidEndNode);
	pIO->WriteWord(dynaRod.siRayMinDegAngle);
	pIO->WriteWord(dynaRod.siRayMaxDegAngle);
	pIO->WriteByte(ciBoltNCalMode);
	pIO->WriteByte(cnMinBoltN);
	pIO->WriteByte(cnMaxSingularLineBolts);
	pIO->WriteDword(layoutbolts.Count);
	for (DWORD i = 0; i < layoutbolts.Count; i++)
		layoutbolts[i].ToBuffer(pIO, dwVersion);
}
void CONNECT_ROD::FromBuffer(BUFFER_IO *pIO, DWORD dwVersion /*=0*/)
{
	pIO->ReadInteger(&idRod.uidRod);
	pIO->ReadByte(&ciCmpstSerial);
	pIO->ReadByte(&ciNecessity);
	pIO->ReadBooleanThin(&blCheckAngleConnWing);
	pIO->ReadByte(&ciCurrTipSideS0E1);
	pIO->ReadByte(&ciConnectWing);
	pIO->ReadByte(&ciPlaneNo);
	pIO->ReadBooleanThin(&dualSideEdge);
	pIO->ReadByte(&cnMinShareBoltN);
	pIO->ReadBooleanThin(&bEndLjJg);
	pIO->ReadByte(&ciOddCalStyle);
	pIO->ReadWord(&sidOddRefRod[0]);
	pIO->ReadWord(&sidOddRefRod[1]);
	pIO->ReadWord(&dynaRod.sidStartNode);
	pIO->ReadWord(&dynaRod.sidEndNode);
	pIO->ReadWord(&dynaRod.siRayMinDegAngle);
	pIO->ReadWord(&dynaRod.siRayMaxDegAngle);
	pIO->ReadByte(&ciBoltNCalMode);
	pIO->ReadByte(&cnMinBoltN);
	pIO->ReadByte(&cnMaxSingularLineBolts);
	DWORD nCount = 0;
	pIO->ReadDword(&nCount);
	layoutbolts.ReSize(nCount);
	for (DWORD i = 0; i < nCount; i++)
		layoutbolts[i].FromBuffer(pIO, dwVersion);
}
///////////////////////////////////////////////////////////////////
//STDTEMPL_PARAM_ITEM
STDTEMPL_PARAM_ITEM::STDTEMPL_PARAM_ITEM()
{
	memset(exprstr,0,19);
}
void STDTEMPL_PARAM_ITEM::SetKey(DWORD keyid)
{
	key=keyid;
	szVarName.Copy(key.Label());
}
//////////////////////////////////////////////////////////////////////////
//CStdComTempl
CXhChar500 CStdComTempl::szImgRootPath = "D:\\CimsForTa\\LDS\\Debug\\images\\stdtmpl_images\\";
CStdComTempl::CStdComTempl(void)
{
	m_ciBaseMirType='X';
	m_ciMirrorMatchMode=0;
	m_cbDisableConnPlaneFlag=0;
	listParaRods.CreateNewAtom=CreateNewParaRod;
	listParaRods.DeleteAtom = DeleteParaRod;
	listParaPlates.LoadDefaultObjectInfo=LoadRaraModelPlateInfo;
	m_bTmplInitialized=false;	//��ʼ״̬��δ��ʼ��
	m_bEnableIntelliConnPointDesign=false;
	m_bIntelliRecogRodNodes=true;
	m_pBelongModel=NULL;
	memset(&sketchimg,0,sizeof(BITMAP));
}
CStdComTempl::~CStdComTempl(void)
{
	if (sketchimg.bmBits!=NULL)
		delete[] sketchimg.bmBits;
	sketchimg.bmBits=NULL;
}
void CStdComTempl::ToBuffer(BUFFER_IO *pIO,DWORD dwVersion/*=0*/)
{
	pIO->WriteDword(m_idStdComTmpl);	//ģ��ı�ʶId
	pIO->WriteBooleanThin(m_bTmplInitialized);	//��ʾ��ģ���Ƿ������
	pIO->WriteBooleanThin(m_bEnableIntelliConnPointDesign);	//�Ƿ񾭹�������֤���Կ���ʽӦ�������������������ı�׼�ڵ��ģ�� wjh-2019.4.23
	pIO->WriteBooleanThin(m_bIntelliRecogRodNodes);
	pIO->WriteString(name);
	pIO->WriteString(szSketchImgFile);
	pIO->WriteByte(m_ciMirrorMatchMode);
	pIO->WriteByte(m_ciBaseMirType);
	pIO->WriteByte(m_cbDisableConnPlaneFlag);
	//1.��Ʋ�������
	pIO->WriteDword(hashCategoryItems.Count);
	for (CATLOG_ITEM *pCatgItem=hashCategoryItems.GetFirst();pCatgItem;pCatgItem=hashCategoryItems.GetNext())
	{
		pIO->WriteDword(pCatgItem->Id);
		pIO->WriteDword(pCatgItem->idRelaObj);
		pIO->WriteByte(pCatgItem->ciType);			//0.Ŀ¼��������飬���ڷ������ͬ���ؼ���Ŀ¼��
		pIO->WriteInteger(pCatgItem->idParentCatg);	//��һ��Ŀ¼��
		pIO->WriteString(pCatgItem->name);
	}
	pIO->WriteDword(listParams.Count);
	for (STDTEMPL_PARAM_ITEM *pParamItem=listParams.GetFirst();pParamItem;pParamItem=listParams.GetNext())
	{
		pIO->WriteDword(pParamItem->key);			//�ò����ı�ʶ��ֵ���򲻳���4�������ַ�����
		pIO->WriteDword(pParamItem->uiCategoryId);	//��λΪ1��uiCategoryId&0x80000000>0��ʾ�ò�������ΪĿ¼��
		pIO->WriteString(pParamItem->szVarName);
		pIO->WriteString(pParamItem->sLabel);	//����������ʾ�Ķ����Ʊ�ǩ
		pIO->WriteString(pParamItem->sNotes);	//�����жԸ������ԵĽ���������
		pIO->WriteString(pParamItem->exprstr);	//���ʽ
		//EXPRESSION_VALUE value;
		pIO->WriteByte(pParamItem->value.ciValType);
		pIO->Write(pParamItem->value.szStrBuf,96);
		pIO->WriteWord(pParamItem->value.objtype.wiPropValue);
		pIO->WriteByte(pParamItem->value.objtype.ciObjType);
	}

	//bool m_bIntelliRecogRodNodes;	//�Ƿ����Զ�����ʶ��˼��ڵ㹦��(SetMappingNodeʱ����)
	//2.�����б���������
	//3.�˼���λ����
	//4.ʵ��ģ������
	pIO->WriteDword(listParaNodes.Count);
	for (CStdTemplParaNode* pParaNode=listParaNodes.GetFirst();pParaNode!=NULL;pParaNode=listParaNodes.GetNext())
	{
		pIO->WriteDword(pParaNode->Id);
		pParaNode->ToBuffer(pIO,dwVersion);
	}
	pIO->WriteDword(listParaRods.GetNodeNum());
	for (CStdTemplParaRod* pParaRod=listParaRods.GetFirst();pParaRod!=NULL;pParaRod=listParaRods.GetNext())
	{
		pIO->WriteDword(pParaRod->Id);
		pIO->WriteInteger(pParaRod->m_idPartClsType);
		pParaRod->ToBuffer(pIO,dwVersion);
	}
	pIO->WriteDword(listParaPlates.Count);
	for (CStdTemplParaPlate* pParaPlate=listParaPlates.GetFirst();pParaPlate!=NULL;pParaPlate=listParaPlates.GetNext())
	{
		pIO->WriteDword(pParaPlate->Id);
		pParaPlate->ToBuffer(pIO,dwVersion);
	}
	pIO->WriteDword(listParaBolts.Count);
	for (CStdTemplParaBolt* pParaBolt=listParaBolts.GetFirst();pParaBolt!=NULL;pParaBolt=listParaBolts.GetNext())
	{
		pIO->WriteDword(pParaBolt->Id);
		pParaBolt->ToBuffer(pIO,dwVersion);
	}
	//�ṹ�����й����ı�׼���ӵ�
	pIO->WriteDword(listStdConnPoints.Count);
	for (MATCH_CONNECT_POINT* pMatchConnPoint=listStdConnPoints.EnumObjectFirst();pMatchConnPoint!=NULL;pMatchConnPoint=listStdConnPoints.EnumObjectNext())
	{
		pIO->WriteInteger(pMatchConnPoint->idNode);			//ƥ�����ӵ����ڵĻ�׼�ڵ�
		pIO->WriteInteger(pMatchConnPoint->idStdConnPoint);	//ƥ��ı�׼���ӵ�ģʽ
		pIO->WriteInteger(pMatchConnPoint->idStdNodePlate);	//���������ӵ㴦��Ҫʵ����ƵĽڵ���׼����ģʽ
		pIO->WriteInteger(pMatchConnPoint->idBaseRod);		//���ӵ�Ļ�׼�˼�
		pIO->WriteInteger(pMatchConnPoint->idRayRodOnAxisY);	//���ӵ��Y�������߸˼�
		pIO->WriteByte(pMatchConnPoint->cbMirCreateFlag);			//�Գ�������Ϣ,0x01:X��Գ�;0x02:Y��Գ�;0x04:Z��Գ�
		pIO->WriteDword(pMatchConnPoint->xarrMapConnRods.Count);
		for (UINT i=0;i<pMatchConnPoint->xarrMapConnRods.Count;i++)
			pIO->WriteDword(pMatchConnPoint->xarrMapConnRods.At(i));//���ӵ��漰�����߸˼���Ϊ��ʱ��ʾ�Զ���ȡ���ӵ����߸˼�
	}
}
void CStdComTempl::FromBuffer(BUFFER_IO *pIO,DWORD dwVersion/*=0*/)
{
	pIO->ReadInteger(&m_idStdComTmpl);	//ģ��ı�ʶId
	pIO->ReadBooleanThin(&m_bTmplInitialized);	//��ʾ��ģ���Ƿ������
	pIO->ReadBooleanThin(&m_bEnableIntelliConnPointDesign);	//�Ƿ񾭹�������֤���Կ���ʽӦ�������������������ı�׼�ڵ��ģ�� wjh-2019.4.23
	pIO->ReadBooleanThin(&m_bIntelliRecogRodNodes);
	pIO->ReadString(name);
	pIO->ReadString(szSketchImgFile);
	pIO->ReadByte(&m_ciMirrorMatchMode);
	pIO->ReadByte(&m_ciBaseMirType);
	pIO->ReadByte(&m_cbDisableConnPlaneFlag);
	//1.��Ʋ�������
	DWORD i=0, count=0, id=0, key=0;
	hashCategoryItems.Empty();
	pIO->ReadDword(&count);
	for (i=0;i<count;i++)
	{
		pIO->ReadDword(&key);
		CATLOG_ITEM *pCatgItem=hashCategoryItems.Add(key);
		pCatgItem->idRelaObj=pIO->ReadDword();
		pIO->ReadByte(&pCatgItem->ciType);			//0.Ŀ¼��������飬���ڷ������ͬ���ؼ���Ŀ¼��
		pIO->ReadInteger(&pCatgItem->idParentCatg);	//��һ��Ŀ¼��
		pIO->ReadString(pCatgItem->name);
	}
	listParams.Empty();
	pIO->ReadDword(&count);
	for (i=0;i<count;i++)
	{
		pIO->ReadDword(&key);			//�ò����ı�ʶ��ֵ���򲻳���4�������ַ�����
		STDTEMPL_PARAM_ITEM *pParamItem=listParams.Add(key);
		pIO->ReadInteger(&pParamItem->uiCategoryId);	//��λΪ1��uiCategoryId&0x80000000>0��ʾ�ò�������ΪĿ¼��
		pIO->ReadString(pParamItem->szVarName);
		pIO->ReadString(pParamItem->sLabel);	//����������ʾ�Ķ����Ʊ�ǩ
		pIO->ReadString(pParamItem->sNotes);	//�����жԸ������ԵĽ���������
		pIO->ReadString(pParamItem->exprstr);	//���ʽ
		//EXPRESSION_VALUE value;
		pIO->ReadByte(&pParamItem->value.ciValType);
		pIO->Read(pParamItem->value.szStrBuf,96);
		pIO->ReadWord(&pParamItem->value.objtype.wiPropValue);
		pIO->ReadByte(&pParamItem->value.objtype.ciObjType);
	}

	//bool m_bIntelliRecogRodNodes;	//�Ƿ����Զ�����ʶ��˼��ڵ㹦��(SetMappingNodeʱ����)
	//2.�����б���������(�ⲿ����Ϣ����ģ��ʵ������Ϣ��������ģ���¡ʱ���ݴ�ȡ wjh-2019.9.6)
	hashMatchNodeHid.Empty();
	hashMatchRodHid.Empty();
	hashMatchPlates.Empty();
	//3.�˼���λ����
	//4.ʵ��ģ������
	listParaNodes.Empty();
	pIO->ReadDword(&count);
	for(i=0;i<count;i++)
	{
		pIO->ReadDword(&id);
		CStdTemplParaNode* pParaNode=listParaNodes.Add(id);
		pParaNode->FromBuffer(pIO,dwVersion);
	}
	listParaRods.Empty();
	long idPartClsType = 0;
	pIO->ReadDword(&count);
	for(i=0;i<count;i++)
	{
		pIO->ReadDword(&id);
		pIO->ReadInteger(&idPartClsType);
		CStdTemplParaRod* pParaRod=listParaRods.Add(id,idPartClsType);
		pParaRod->FromBuffer(pIO,dwVersion);
	}
	listParaPlates.Empty();
	pIO->ReadDword(&count);
	for(i=0;i<count;i++)
	{
		pIO->ReadDword(&id);
		CStdTemplParaPlate* pParaPlate=listParaPlates.Add(id);
		pParaPlate->FromBuffer(pIO,dwVersion);
	}
	listParaBolts.Empty();
	pIO->ReadDword(&count);
	for(i=0;i<count;i++)
	{
		pIO->ReadDword(&id);
		CStdTemplParaBolt* pParaBolt=listParaBolts.Add(id);
		pParaBolt->FromBuffer(pIO,dwVersion);
	}
	//�ṹ�����й����ı�׼���ӵ�
	listStdConnPoints.Empty();
	pIO->ReadDword(&count);
	for(i=0;i<count;i++)
	{
		MATCH_CONNECT_POINT matchpoint;
		pIO->ReadInteger(&matchpoint.idNode);			//ƥ�����ӵ����ڵĻ�׼�ڵ�
		pIO->ReadInteger(&matchpoint.idStdConnPoint);	//ƥ��ı�׼���ӵ�ģʽ
		pIO->ReadInteger(&matchpoint.idStdNodePlate);	//���������ӵ㴦��Ҫʵ����ƵĽڵ���׼����ģʽ
		pIO->ReadInteger(&matchpoint.idBaseRod);		//���ӵ�Ļ�׼�˼�
		pIO->ReadInteger(&matchpoint.idRayRodOnAxisY);	//���ӵ��Y�������߸˼�
		pIO->ReadByte(&matchpoint.cbMirCreateFlag);			//�Գ�������Ϣ,0x01:X��Գ�;0x02:Y��Գ�;0x04:Z��Գ�
		MATCH_CONNECT_POINT* pMatchConnPoint=listStdConnPoints.AttachObject(matchpoint);
		DWORD dwRodCount = 0;
		pIO->ReadDword(&dwRodCount);
		pMatchConnPoint->xarrMapConnRods.ReSize(0);
		for (DWORD j = 0; j < dwRodCount; j++)
		{
			pIO->ReadDword(&id);
			pMatchConnPoint->xarrMapConnRods.Append(id);
		}
	}
}
bool CStdComTempl::CloneStdTmplFrom(IStdComTempl* pStdComTmpl)
{
	if (pStdComTmpl->GetTmplId()!=this->m_idStdComTmpl)
		return false;
	char stackpool[4096]={ 0 };
	CBuffer buffer(stackpool,4096);
	buffer.ClearContents();
	pStdComTmpl->ToBuffer(&buffer);
	buffer.SeekToBegin();
	FromBuffer(&buffer);
	return true;
}
bool CStdComTempl::IsStdTmplChecked(UINT idStdComTmpl)
{
	CStdComTempl* pStdTmpl = globalStdComTmplLibrary.LoadStdComTempl(idStdComTmpl);
	if (pStdTmpl == NULL)
		return false;
	if (pStdTmpl->ciGroupType != STDTMPL_CONNPOINT && pStdTmpl->ciGroupType != STDTMPL_NODEPLATE)
		return true;
	else
		return pStdTmpl->blEnableIntelliConnPointDesign;
}
bool CStdComTempl::IsStdTmplAuthorized(UINT idStdComTmpl,UINT uiDogKey/*=0*/,UINT uiClientSerial/*= 0*/)
{
	TMPLID tmplid = idStdComTmpl;
	if (tmplid.ciGroup == STDTMPL_STRUCTURE)
	{
#if defined(__TMA_)
		if (idStdComTmpl == STDTMPL_STRU_HOOF_PLATES ||	//˫�䵥ţ���
			idStdComTmpl == STDTEMPL_CONNPOINT_HD7 || idStdComTmpl == STDTEMPL_CONNPOINT_HD10 ||
			idStdComTmpl == STDTEMPL_CONNPOINT_BP1 || idStdComTmpl == STDTEMPL_CONNPOINT_BP2 ||
			idStdComTmpl == STDTEMPL_CONNPOINT_BP3)
			return true;
		else
			return false;
#elif defined(__LMA_)
		if (idStdComTmpl == STDTMPL_STRU_TUBE_WALKWAY || idStdComTmpl == STDTMPL_STRU_TUBE_WORKBENCH)
			return true;
		else
			return false;
#elif defined(__LDS_)
		return true;
#else
		return false;
#endif
	}
	else if (tmplid.ciGroup == STDTMPL_CONNPOINT)
	{
#if defined(__TMA_)
		if (idStdComTmpl == STDTEMPL_CONNPOINT_HD7 || idStdComTmpl == STDTEMPL_CONNPOINT_HD10 ||
			idStdComTmpl == STDTEMPL_CONNPOINT_BP1 || idStdComTmpl == STDTEMPL_CONNPOINT_BP2 ||
			idStdComTmpl == STDTEMPL_CONNPOINT_BP3)
			return true;
		else
			return false;
#elif defined(__LDS_)
		return true;
#else
		return false;
#endif
	}
	else if (tmplid.ciGroup == STDTMPL_NODEPLATE)
	{
#if defined(__TMA_)
		if (idStdComTmpl == STDTEMPL_NODEPLATE_HD7 ||idStdComTmpl == STDTEMPL_NODEPLATE_HD10 ||
			idStdComTmpl == STDTEMPL_NODEPLATE_BP1 || idStdComTmpl == STDTEMPL_NODEPLATE_BP2 ||
			idStdComTmpl == STDTEMPL_NODEPLATE_BP3)
			return true;
		else
			return false;
#elif defined(__LDS_)
		return true;
#else
		return false;
#endif
	}
	return false;
}
bool CStdComTempl::IsAuthorized(UINT uiDogKey/*= 0*/,UINT uiClientSerial/*= 0*/)
{
	return IsStdTmplAuthorized(this->Id,uiDogKey,uiClientSerial);
}
CXhChar50 CStdComTempl::_InternalGetSketchImgFileSubFolder()
{
	TMPLID xTmplId(m_idStdComTmpl);
	if (xTmplId.ciGroup == STDTMPL_STRUCTURE)
		return "";
	else //if(xTmplId.ciGroup>=IStdComTempl::STDTMPL_CONNPOINT)
		return "Node\\";
}
bool CStdComTempl::GetSketchImage(IMAGE_INFO* pSketchImage)
{
	if (pSketchImage==NULL)
		return false;
	CXhChar50 szSketchImgSubFolder=_InternalGetSketchImgFileSubFolder();
	CXhChar50 szSketchBmpFile("%s%s",(char*)szSketchImgSubFolder,(char*)szSketchImgFile);
	if(szSketchImgFile.Length==0)
	{
		if (m_idStdComTmpl==STDTEMPL_NODEPLATE_XH_DX1)
			szSketchBmpFile="Node\\DX1.bmp";
		else if (m_idStdComTmpl==STDTEMPL_NODEPLATE_XH_DX2)
			szSketchBmpFile="Node\\DX2.bmp";
		else if (m_idStdComTmpl==STDTEMPL_NODEPLATE_XH_DX3)
			szSketchBmpFile="Node\\DX3.bmp";
		else if (m_idStdComTmpl==STDTEMPL_NODEPLATE_XH_GX2)
			szSketchBmpFile="Node\\GX2.bmp";
		else if (m_idStdComTmpl==STDTEMPL_NODEPLATE_XH_DM1)
			szSketchBmpFile="Node\\DM1.bmp";
		else if (m_idStdComTmpl == STDTEMPL_NODEPLATE_BP1)
			szSketchBmpFile = "Node\\BP1.bmp";
		else if (m_idStdComTmpl == STDTEMPL_NODEPLATE_BP2)
			szSketchBmpFile = "Node\\BP2.bmp";
		else if (m_idStdComTmpl == STDTEMPL_NODEPLATE_BP3)
			szSketchBmpFile = "Node\\BP3.bmp";
		else if (m_idStdComTmpl == STDTEMPL_NODEPLATE_HD1)//STDTEMPL_CONNPOINT_HD7
			szSketchBmpFile = "Node\\HD1.bmp";
		else if (m_idStdComTmpl == STDTEMPL_NODEPLATE_HD1A)//STDTEMPL_CONNPOINT_HD7
			szSketchBmpFile = "Node\\HD1A.bmp";
		else if (m_idStdComTmpl == STDTEMPL_NODEPLATE_HD2)//STDTEMPL_CONNPOINT_HD7
			szSketchBmpFile = "Node\\HD2.bmp";
		else if (m_idStdComTmpl == STDTEMPL_NODEPLATE_HD7)//STDTEMPL_CONNPOINT_HD7
			szSketchBmpFile = "Node\\HD7.bmp";
		else if (m_idStdComTmpl == STDTEMPL_NODEPLATE_HD7_T)//STDTEMPL_CONNPOINT_HD7
			szSketchBmpFile = "Node\\HD7T.bmp";
		else if (m_idStdComTmpl== STDTMPL_STRU_TUBE_WALKWAY)
			szSketchBmpFile="WalkWay.bmp";
		else if (m_idStdComTmpl==STDTMPL_STRU_TUBE_WORKBENCH)
			szSketchBmpFile="StandBench.bmp";
		else if (m_idStdComTmpl == STDTMPL_STRU_HOOF_PLATES)
			szSketchBmpFile = "NeckHoofPlates.bmp";
	}
	if (pSketchImage)
	{
		pSketchImage->ciSrcType=1;
		StrCopy(pSketchImage->szImgFile,szSketchBmpFile,51);
	}
	return true;
}
#if defined(_DEBUG)&&defined(__LDS_)&&defined(__TIF_)
#include "ximatif.h"
#endif
#include "ImageFile.h"
bool CStdComTempl::LoadSketchImage()
{
	IMAGE_INFO image;
	if (GetSketchImage(&image))
	{
		if (image.ciSrcType==0)
		{
			CBitmap img;
			//::LoadBitmap(theApp.m_hInstance,MAKEINTRESOURCE(image.idResImage));
			img.LoadBitmap(image.idResImage);
			img.GetBitmap(&sketchimg);
			DWORD dwCount=sketchimg.bmWidthBytes*sketchimg.bmHeight;
			sketchimg.bmBits=new char[dwCount];//TODO:�ڴ�δ�ͷ�
			img.GetBitmapBits(dwCount,sketchimg.bmBits);
			return true;
		}
		else if (image.ciSrcType==1)
		{
			CXhChar500 szImgPath=szImgRootPath;
			szImgPath.Append(image.szImgFile);
#if defined(_DEBUG)&&defined(__LDS_)&&defined(__TIF_)
			if (strstr(image.szImgFile,"tif")!=NULL)
			{
				CImageFileTif tiff;
				tiff.ReadImageFile(szImgPath);
				int nPages=tiff.niPageCount;
				tiff.Decode(0,true);
				tiff.GetBitmap(&sketchimg);
				return true;
			}
			else
#endif
				return sketchimg.LoadBmpFile(szImgPath);
		}
	}
	return false;
}
void CStdComTempl::InitComponentTempl(TEMPL_SOURCE* pTemplSource/*=NULL*/)
{
	TEMPL_SOURCE source;
	if (pTemplSource==NULL)
		pTemplSource=&source;
	if (pTemplSource!=NULL&&pTemplSource->ciSourceType!=0)
		return ;
	CStdTemplParaNode* pParaNode=NULL;
	CStdTemplParaRod* pParaRod=NULL;
	CStdTemplParaAngle* pParaAngle=NULL;
	CStdTemplParaPlate* pParaPlate = NULL;
	if (pTemplSource->idStdComTempl==STDTMPL_STRU_TUBE_WORKBENCH)
	{	//�ֹ����ϻ���վλƽ̨��
		this->szSketchImgFile = "StandBench.bmp";
		//this->m_idStdComTmpl=pTemplSource->idStdComTempl;
	}
	else if (pTemplSource->idStdComTempl== STDTMPL_STRU_HOOF_PLATES)
	{	//��ϽǸ�����˫�䵥��
		this->szSketchImgFile = "NeckHoofPlates.bmp";
		//this->m_idStdComTmpl=pTemplSource->idStdComTempl;
	}
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_HD1)
		InitNodePlateTemp_HD1();
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_HD1A)
		InitNodePlateTemp_HD1A();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_HD2)
		InitNodePlateTemp_HD2();
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_HD3)
		InitNodePlateTemp_HD3();
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_HD3_2)
		InitNodePlateTemp_HD3_2();
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_HD3_G5)
		InitNodePlateTemp_HD3_G5();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_XH_HD3A)
		InitNodePlateTemp_HD3A();
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_HD4)
		InitNodePlateTemp_HD4(true);
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_HD4_2)
		InitNodePlateTemp_HD4(false);
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_HD5)
		InitNodePlateTemp_HD5();
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_HD6)
		InitNodePlateTemp_HD6();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_HD7)
		InitNodePlateTemp_HD7();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_HD7_T)
		InitNodePlateTemp_HD7_T();
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_HD8)
		InitNodePlateTemp_HD8();
	else if (pTemplSource->idStdComTempl== STDTEMPL_NODEPLATE_HD8_YS)
		InitNodePlateTemp_HD8_YS();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_HD9)
		InitNodePlateTemp_HD9();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_HD9_T)
		InitNodePlateTemp_HD9_T();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_HD10)
		InitNodePlateTemp_HD10();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_HD10_T)
		InitNodePlateTemp_HD10_T();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_XH_HD12)
		InitNodePlateTemp_HD12();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_XH_HD12_X)
		InitNodePlateTemp_HD12X();
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_XH_HD13)
		InitNodePlateTemp_HD13();
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_XH_HD14)
		InitNodePlateTemp_HD14();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_ZK14_K1)
		InitNodePlateTemp_ZK14_K1();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_ZK14_K2)
		InitNodePlateTemp_ZK14_K2();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_ZK14_K3)
		InitNodePlateTemp_ZK14_K3();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_ZK14_K1_SIDE)
		InitNodePlateTemp_ZK14_K1_SIDE();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_ZK14_K3_SIDE)
		InitNodePlateTemp_ZK14_K3_SIDE();
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_BP1||
		pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_BP2)
		InitNodePlateTemp_BP1or2(pTemplSource->idStdComTempl);
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_BP3)
		InitNodePlateTemp_BP3();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_JC1_K1)
	{	//02050001
		szSketchImgFile = "JC1_K1.bmp";
		//�ڵ㹹���׼װ������ϵ����
		STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
		pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
		pParam->value.objtype.wiPropValue=0;
		strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8

		CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
		CStdTemplParaRod* pParaRod=this->listParaRods.Add(1);
		pParaRod->m_bNecessaryRod=true;
		pParaRod=this->listParaRods.Add(2);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		pParaRod=this->listParaRods.Add(3);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		CStdTemplParaPlate* pParaPlate=this->listParaPlates.Append();
		pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
		pParaPlate->cbMirCreateFlag=0x01;
		pParaPlate->design.idBaseNode=1;
		pParaPlate->design.idBaseRod=1;
		pParaPlate->design.ciProfileStyle=1;	//��������
		pParaPlate->design.ciDatumLenAxis='Y';
		pParaPlate->design.ciDatumAngleWing='Y';
		pParaPlate->design.bend.widAngleBendBy=0;
		pParaPlate->connrods.Clear();
		//1.����-����ͨ��
		pParaPlate->connrods[0].sidRod=1;
		pParaPlate->connrods[0].dynaRod.siRayMinDegAngle=40;
		pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle=60;
		pParaPlate->connrods[0].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[0].ciCurrTipSideS0E1=2;
		pParaPlate->connrods[0].ciPlaneNo=1;
		pParaPlate->connrods[0].cnMinBoltN=5;
		pParaPlate->connrods[0].dualSideEdge=false;
		pParaPlate->connrods[0].ciConnectWing='Y';
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[0].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[0].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[0].layoutbolts.ReSize(1);
		//2.����б��
		pParaPlate->connrods[1].sidRod=2;
		pParaPlate->connrods[1].dynaRod.siRayMinDegAngle=120;
		pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle=150;
		pParaPlate->connrods[1].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[1].ciCurrTipSideS0E1=1;
		pParaPlate->connrods[1].ciPlaneNo=1;
		pParaPlate->connrods[1].cnMinBoltN=4;
		pParaPlate->connrods[1].dualSideEdge=false;
		pParaPlate->connrods[1].ciConnectWing='Y';
		pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[1].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[1].layoutbolts.ReSize(1);
		//3.����б��
		pParaPlate->connrods[2].sidRod=3;
		pParaPlate->connrods[2].dynaRod.siRayMinDegAngle=120;
		pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle=150;
		pParaPlate->connrods[2].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[2].ciCurrTipSideS0E1=0;
		pParaPlate->connrods[2].ciPlaneNo=1;
		pParaPlate->connrods[2].cnMinBoltN=4;
		pParaPlate->connrods[2].dualSideEdge=false;
		pParaPlate->connrods[2].ciConnectWing='Y';
		pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[2].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[2].layoutbolts.ReSize(1);
		pParaPlate->connrods.ReSize(3);
	}
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_JC1_K2)
	{
		szSketchImgFile = "JC1_K2.bmp";
		//�ڵ㹹���׼װ������ϵ����
		STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
		pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
		pParam->value.objtype.wiPropValue=0;
		strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8

		CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
		CStdTemplParaRod* pParaRod=this->listParaRods.Add(1);
		pParaRod->m_bNecessaryRod=true;
		pParaRod=this->listParaRods.Add(2);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		pParaRod=this->listParaRods.Add(3);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		pParaRod=this->listParaRods.Add(4);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		CStdTemplParaPlate* pParaPlate=this->listParaPlates.Add(1);
		pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
		pParaPlate->cbMirCreateFlag=0x01;
		pParaPlate->design.idBaseNode=1;
		pParaPlate->design.idBaseRod=1;
		pParaPlate->design.ciProfileStyle=1;	//��������
		pParaPlate->design.ciDatumLenAxis='Y';
		pParaPlate->design.ciDatumAngleWing='X';
		pParaPlate->design.bend.widAngleBendBy=0;
		pParaPlate->connrods.Clear();
		//1.����
		pParaPlate->connrods[0].sidRod=1;
		pParaPlate->connrods[0].dynaRod.siRayMinDegAngle=0;
		pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle=10;
		pParaPlate->connrods[0].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[0].ciCurrTipSideS0E1=0;
		pParaPlate->connrods[0].ciOddCalStyle=1;
		pParaPlate->connrods[0].ciPlaneNo=1;
		pParaPlate->connrods[0].cnMinBoltN=4;
		pParaPlate->connrods[0].dualSideEdge=true;
		pParaPlate->connrods[0].ciConnectWing=0;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=0;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseAngle=1;
		pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[0].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[0].layoutbolts[0].ciDatumWing=0;//'Y';
		memcpy(pParaPlate->connrods[0].layoutbolts[0].szLenOffsetOf1stBolt,"80",2);
		pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[0].layoutbolts.ReSize(1);
		//2.����б��
		pParaPlate->connrods[1].sidRod=2;
		pParaPlate->connrods[1].dynaRod.siRayMinDegAngle=120;
		pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle=140;
		pParaPlate->connrods[1].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[1].ciCurrTipSideS0E1=0;
		pParaPlate->connrods[1].ciOddCalStyle=1;
		pParaPlate->connrods[1].ciPlaneNo=1;
		pParaPlate->connrods[1].cnMinBoltN=4;
		pParaPlate->connrods[1].dualSideEdge=true;
		pParaPlate->connrods[1].ciConnectWing=0;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=0;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseAngle=1;
		pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[1].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;//'Y';
		memcpy(pParaPlate->connrods[1].layoutbolts[0].szLenOffsetOf1stBolt,"80",2);
		pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[1].layoutbolts.ReSize(1);
		//3.����б��
		pParaPlate->connrods[2].sidRod=3;
		pParaPlate->connrods[2].dynaRod.siRayMinDegAngle=190;
		pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle=200;
		pParaPlate->connrods[2].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[2].ciCurrTipSideS0E1=0;
		pParaPlate->connrods[2].ciOddCalStyle=1;
		pParaPlate->connrods[2].ciPlaneNo=1;
		pParaPlate->connrods[2].cnMinBoltN=4;
		pParaPlate->connrods[2].dualSideEdge=true;
		pParaPlate->connrods[2].ciConnectWing=0;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=3;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widCrossAngle=1;
		pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[2].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;//'Y';
		memcpy(pParaPlate->connrods[2].layoutbolts[0].szLenOffsetOf1stBolt,"130",3);
		pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[2].layoutbolts.ReSize(1);
		//4.����б��
		pParaPlate->connrods[3].sidRod=4;
		pParaPlate->connrods[3].dynaRod.siRayMinDegAngle=280;
		pParaPlate->connrods[3].dynaRod.siRayMaxDegAngle=290;
		pParaPlate->connrods[3].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[3].ciCurrTipSideS0E1=0;
		pParaPlate->connrods[3].ciOddCalStyle=1;
		pParaPlate->connrods[3].sidOddRefRod[0]=1;
		pParaPlate->connrods[3].sidOddRefRod[1]=3;
		pParaPlate->connrods[3].ciPlaneNo=1;
		pParaPlate->connrods[3].cnMinBoltN=4;
		pParaPlate->connrods[3].dualSideEdge=true;
		pParaPlate->connrods[3].ciConnectWing=0;
		pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle=3;
		pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[3].layoutbolts[0].basepoint.widCrossAngle=1;
		pParaPlate->connrods[3].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[3].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[3].layoutbolts[0].ciDatumWing=0;//'Y';
		memcpy(pParaPlate->connrods[3].layoutbolts[0].szLenOffsetOf1stBolt,"130",3);
		pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[3].layoutbolts.ReSize(1);
		pParaPlate->connrods.ReSize(4);
	}
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_JC3)
	{	//02050001
		szSketchImgFile = "JC3.bmp";
		//�ڵ㹹���׼װ������ϵ����
		STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
		pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
		pParam->value.objtype.wiPropValue=0;
		strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8

		CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
		CStdTemplParaRod* pParaRod=this->listParaRods.Add(1);
		pParaRod->m_bNecessaryRod=true;
		pParaRod=this->listParaRods.Add(2);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		pParaRod=this->listParaRods.Add(3);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		CStdTemplParaPlate* pParaPlate=this->listParaPlates.Append();
		pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
		pParaPlate->cbMirCreateFlag=0x01;
		pParaPlate->design.idBaseNode=1;
		pParaPlate->design.idBaseRod=1;
		pParaPlate->design.ciProfileStyle=1;	//��������
		pParaPlate->design.ciDatumLenAxis='Y';
		pParaPlate->design.ciDatumAngleWing='Y';
		pParaPlate->design.bend.widAngleBendBy=0;
		pParaPlate->connrods.Clear();
		//1.����-����ͨ��
		pParaPlate->connrods[0].sidRod=1;
		pParaPlate->connrods[0].dynaRod.siRayMinDegAngle=40;
		pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle=60;
		pParaPlate->connrods[0].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[0].ciCurrTipSideS0E1=2;
		pParaPlate->connrods[0].ciPlaneNo=1;
		pParaPlate->connrods[0].cnMinBoltN=5;
		pParaPlate->connrods[0].dualSideEdge=false;
		pParaPlate->connrods[0].ciConnectWing='Y';
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[0].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[0].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[0].layoutbolts.ReSize(1);
		//2.����б��
		pParaPlate->connrods[1].sidRod=2;
		pParaPlate->connrods[1].dynaRod.siRayMinDegAngle=120;
		pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle=150;
		pParaPlate->connrods[1].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[1].ciCurrTipSideS0E1=2;
		pParaPlate->connrods[1].ciPlaneNo=1;
		pParaPlate->connrods[1].cnMinBoltN=4;
		pParaPlate->connrods[1].dualSideEdge=false;
		pParaPlate->connrods[1].ciConnectWing='Y';
		pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[1].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[1].layoutbolts.ReSize(1);
		//3.����б��
		pParaPlate->connrods[2].sidRod=3;
		pParaPlate->connrods[2].dynaRod.siRayMinDegAngle=89;
		pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle=91;
		pParaPlate->connrods[2].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[2].ciCurrTipSideS0E1=0;
		pParaPlate->connrods[2].ciPlaneNo=1;
		pParaPlate->connrods[2].cnMinBoltN=4;
		pParaPlate->connrods[2].dualSideEdge=false;
		pParaPlate->connrods[2].ciConnectWing='Y';
		pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[2].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[2].layoutbolts.ReSize(1);
		pParaPlate->connrods.ReSize(3);
	}
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_HK1)
	{	//03040001
		szSketchImgFile = "HK1.bmp";
		//�ڵ㹹���׼װ������ϵ����
		STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
		pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
		pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
		pParam->value.objtype.wiPropValue = 0;
		strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
		name.Copy("HK1");
		this->m_bEnableIntelliConnPointDesign = true;

		CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
		pParaRod = pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
		pParaRod->m_bNecessaryRod = true;
		pParaAngle->ciAngleWorkWing = 'Y';
		pParaRod = pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
		pParaAngle->idStartNode = 1;
		pParaAngle->m_bNecessaryRod = true;
		pParaAngle->ciAngleWorkWing = 'Y';
		pParaAngle->xFlag.bFilterConnBoltN = true;
		//pParaAngle->xFlag.bFilterSectType = true;
		//pParaAngle->xFlag.bFilterWorkWing = true;
		pParaAngle->bUpdateStartPos = true;
		pParaAngle->desStart.cTouchLineType = 0;
		pParaAngle->desStart.cTouchLine = 1;//'Y';
		pParaAngle->desStart.szXorCtrlWingOffset = "g";
		pParaAngle->desStart.szYorOthrWingOffset = "[=-P1#T";
		pParaRod = pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
		pParaAngle->idStartNode = 1;
		pParaAngle->m_bNecessaryRod = true;
		pParaAngle->ciAngleWorkWing = 'X';
		pParaAngle->xFlag.bFilterConnBoltN = true;
		//pParaAngle->xFlag.bFilterSectType = true;
		//pParaAngle->xFlag.bFilterWorkWing = true;
		pParaAngle->bUpdateStartPos = true;
		pParaAngle->desStart.cTouchLineType = 0;
		pParaAngle->desStart.cTouchLine = 1;//'Y';
		pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
		pParaAngle->desStart.szYorOthrWingOffset = "g";
		CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
		pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
		pParaPlate->cbMirCreateFlag=0x01;
		pParaPlate->design.idBaseNode = 1;
		pParaPlate->design.idBaseRod = 1;
		pParaPlate->design.ciProfileStyle = 1;	//��������
		pParaPlate->design.ciDatumLenAxis = 'X';
		pParaPlate->design.ciDatumAngleWing = 'X';
		pParaPlate->design.bend.widAngleBendBy = 0;
		pParaPlate->connrods.Clear();
		//1.ˮƽ���
		pParaPlate->connrods[0].sidRod = 1;
		//pParaPlate->connrods[0].dynaRod.siRayMinDegAngle = 89;
		//pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle = 95;
		pParaPlate->connrods[0].ciNecessity = 1;	//��Ҫ�˼�
		pParaPlate->connrods[0].ciCurrTipSideS0E1 = 2;
		pParaPlate->connrods[0].ciPlaneNo = 1;
		pParaPlate->connrods[0].cnMinBoltN = 4;
		pParaPlate->connrods[0].dualSideEdge = true;
		pParaPlate->connrods[0].ciConnectWing = 'Y';
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[0].layoutbolts[0].cnCount = 0;
		pParaPlate->connrods[0].layoutbolts[0].ciRows = 0;
		pParaPlate->connrods[0].layoutbolts[0].cnMaxSingularLineBolts=5;
		pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;	//˫��ԳƲ���
		pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;//'Y';
		pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[0].layoutbolts.ReSize(1);
		//2.����б��
		pParaPlate->connrods[1].sidRod = 2;
		//pParaPlate->connrods[1].dynaRod.siRayMinDegAngle = 120;
		//pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle = 150;
		pParaPlate->connrods[1].ciNecessity = 1;	//��Ҫ�˼�
		pParaPlate->connrods[1].InitTipInfo(0,0,1);//ciCurrTipSideS0E1 = 1;
		pParaPlate->connrods[1].ciPlaneNo = 1;
		pParaPlate->connrods[1].cnMinBoltN = 2;
		pParaPlate->connrods[1].dualSideEdge = false;
		pParaPlate->connrods[1].ciConnectWing = 0;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[1].layoutbolts[0].cnCount = 0;
		pParaPlate->connrods[1].layoutbolts[0].ciRows = 0;
		pParaPlate->connrods[1].layoutbolts[0].cnMaxSingularLineBolts=4;
		pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
		pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;//'Y';
		pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[1].layoutbolts.ReSize(1);
		//3.����б��
		pParaPlate->connrods[2].sidRod = 3;
		//pParaPlate->connrods[2].dynaRod.siRayMinDegAngle = 200;
		//pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle = 230;
		pParaPlate->connrods[2].ciNecessity = 1;	//��Ҫ�˼�
		pParaPlate->connrods[2].InitTipInfo(0,0,1);//ciCurrTipSideS0E1 = 0;
		pParaPlate->connrods[2].ciPlaneNo = 1;
		pParaPlate->connrods[2].cnMinBoltN = 2;
		pParaPlate->connrods[2].dualSideEdge = false;
		pParaPlate->connrods[2].ciConnectWing = 0;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[2].layoutbolts[0].cnCount = 0;
		pParaPlate->connrods[2].layoutbolts[0].ciRows = 0;
		pParaPlate->connrods[2].layoutbolts[0].cnMaxSingularLineBolts=4;
		pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
		pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;//'Y';
		pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[2].layoutbolts.ReSize(1);
		pParaPlate->connrods.ReSize(3);
	}
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_XH_GX1)
		InitNodePlateTemp_GX1();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_XH_GX2)
		InitNodePlateTemp_GX2();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_XH_GX3)
		InitNodePlateTemp_GX3();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_XH_GX4)
		InitNodePlateTemp_GX4();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_XH_GX5)
		InitNodePlateTemp_GX5();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_XH_DM1)
		InitNodePlateTemp_DM1();
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_XH_DM1A)
		InitNodePlateTemp_DM1A();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_XH_DM2)
		InitNodePlateTemp_DM2();
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_XH_DM3)
		InitNodePlateTemp_DM3();
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_XH_DX1)
	{
		szSketchImgFile = "DX1.bmp";
		//�ڵ㹹���׼װ������ϵ����
		STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
		pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
		pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
		pParam->value.objtype.wiPropValue = 0;
		strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
		this->m_bEnableIntelliConnPointDesign = true;

		pParaNode = this->listParaNodes.Add(1);
		pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
		pParaAngle->idStartNode = 1;
		pParaAngle->m_bNecessaryRod = true;
		pParaAngle->ciAngleWorkWing = 'Y';
		pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
		pParaAngle->idStartNode = 1;
		pParaAngle->m_bNecessaryRod = true;
		pParaAngle->bUpdateStartPos = true;
		pParaAngle->ciAngleWorkWing = 'X';
		pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
		pParaAngle->desStart.szYorOthrWingOffset = "g";
		pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
		pParaAngle->idStartNode = 1;
		pParaAngle->m_bNecessaryRod = true;
		pParaAngle->bUpdateStartPos = true;
		pParaAngle->ciAngleWorkWing = 'X';
		pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
		pParaAngle->desStart.szYorOthrWingOffset = "g";
		//��ͨ���Ӱ�
		pParaPlate = this->listParaPlates.Add(1);
		pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;
		//pParaPlate->cbMirCreateFlag = 0x01;
		pParaPlate->design.idBaseNode = 1;
		pParaPlate->design.idBaseRod = 1;
		pParaPlate->design.ciProfileStyle = 1;	//��������
		pParaPlate->design.ciDatumLenAxis = 'Y';
		pParaPlate->design.ciDatumAngleWing = 'Y';
		pParaPlate->design.bend.widAngleBendBy = 0;
		pParaPlate->connrods.Clear();
		//1.���֧�Ÿ�
		pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 3, true);
		pParaPlate->connrods[0].InitTipInfo(0, 1);
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[0].layoutbolts[0].cnCount = 0;
		pParaPlate->connrods[0].layoutbolts[0].ciRows = 1;
		pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
		pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;
		pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[0].layoutbolts.ReSize(1);
		//2.��ֱ֧�Ÿ�
		pParaPlate->connrods[1].InitBasicInfo(2, true, 1, 1, false);
		pParaPlate->connrods[1].InitTipInfo(0, 0, 1);
		pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = BOLT_ORIGIN::LOCA_RODEND;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseAngle=1;
		pParaPlate->connrods[1].layoutbolts[0].cnCount = 0;
		pParaPlate->connrods[1].layoutbolts[0].ciRows = 1;
		pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
		pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;
		pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[1].layoutbolts.ReSize(1);
		//3.�Ҳ�֧�Ÿ�
		pParaPlate->connrods[2].InitBasicInfo(3, true, 1, 3, false);
		pParaPlate->connrods[2].InitTipInfo(0, 0, 1);
		pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[2].layoutbolts[0].cnCount = 0;
		pParaPlate->connrods[2].layoutbolts[0].ciRows = 1;
		pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
		pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;
		pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[2].layoutbolts.ReSize(1);
		pParaPlate->connrods.ReSize(3);
	}
	/*STDTEMPL_NODEPLATE_XH_DX1A��STDTEMPL_NODEPLATE_XH_DX1Ŀǰ����һ����û��Ҫ�ظ�����
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_XH_DX1A)
	{
		szSketchImgFile = "DX1A.bmp";
		//�ڵ㹹���׼װ������ϵ����
		STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
		pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
		pParam->value.objtype.wiPropValue=0;
		strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
		this->m_bEnableIntelliConnPointDesign=true;

		CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
		CStdTemplParaRod* pParaRod=this->listParaRods.Add(1);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		CStdTemplParaAngle* pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
		pParaAngle->idStartNode=1;
		pParaAngle->idStartNode=1;
		pParaAngle->m_bNecessaryRod=true;
		pParaAngle->bUpdateStartPos=true;
		pParaAngle->desStart.cTouchLineType=0;
		pParaAngle->desStart.cTouchLine=1;//'Y';
		pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";
		pParaAngle->desStart.szYorOthrWingOffset="g";
		pParaRod=this->listParaRods.Add(3);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		CStdTemplParaPlate* pParaPlate=this->listParaPlates.Append();
		pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
		pParaPlate->cbMirCreateFlag=0x01;
		pParaPlate->design.idBaseNode=1;
		pParaPlate->design.idBaseRod=1;
		pParaPlate->design.ciProfileStyle=1;	//��������
		pParaPlate->design.ciDatumLenAxis='Y';
		pParaPlate->design.ciDatumAngleWing='Y';
		pParaPlate->design.bend.widAngleBendBy=0;
		pParaPlate->connrods.Clear();
		//1.���֧�Ÿ�
		pParaPlate->connrods[0].sidRod=1;
		pParaPlate->connrods[0].dynaRod.siRayMinDegAngle=0;
		pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle=5;
		pParaPlate->connrods[0].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[0].ciCurrTipSideS0E1=0;
		pParaPlate->connrods[0].ciOddCalStyle=1;
		pParaPlate->connrods[0].ciPlaneNo=1;
		pParaPlate->connrods[0].cnMinBoltN=3;
		pParaPlate->connrods[0].dualSideEdge=true;
		pParaPlate->connrods[0].ciConnectWing='Y';
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[0].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=1;
		pParaPlate->connrods[0].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[0].layoutbolts.ReSize(1);
		//2.��ֱ֧�Ÿ�
		pParaPlate->connrods[1].sidRod=2;
		pParaPlate->connrods[1].dynaRod.siRayMinDegAngle=25;
		pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle=75;
		pParaPlate->connrods[1].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[1].InitTipInfo(0,1);
		pParaPlate->connrods[1].ciPlaneNo=1;
		pParaPlate->connrods[1].cnMinBoltN=2;
		pParaPlate->connrods[1].dualSideEdge=false;
		pParaPlate->connrods[1].ciConnectWing='Y';
		pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=3;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widCrossBoltAngle=1;
		pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[1].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		memcpy(pParaPlate->connrods[1].layoutbolts[0].szLenOffsetOf1stBolt,"0",2);
		pParaPlate->connrods[1].layoutbolts.ReSize(1);
		//3.�Ҳ�֧�Ÿ�
		pParaPlate->connrods[2].sidRod=3;
		pParaPlate->connrods[2].dynaRod.siRayMinDegAngle=80;
		pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle=130;
		pParaPlate->connrods[2].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[2].ciCurrTipSideS0E1=0;
		pParaPlate->connrods[2].ciOddCalStyle=0;
		pParaPlate->connrods[2].sidOddRefRod[0]=1;
		pParaPlate->connrods[2].ciPlaneNo=1;
		pParaPlate->connrods[2].cnMinBoltN=2;
		pParaPlate->connrods[2].dualSideEdge=false;
		pParaPlate->connrods[2].ciConnectWing='X';
		pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=0;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[2].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[2].layoutbolts.ReSize(1);
		pParaPlate->connrods.ReSize(3);
	}*/
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_XH_DX2)//jdb
	{
		szSketchImgFile = "DX2.bmp";
		this->m_bEnableIntelliConnPointDesign=true;
		//�ڵ㹹���׼װ������ϵ����
		STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
		pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
		pParam->value.objtype.wiPropValue=0;
		strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
		//
		pParaNode=this->listParaNodes.Add(1);
		pParaNode->m_bNecessaryNode = true;
		pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
		pParaAngle->m_bNecessaryRod=true;
		pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
		pParaAngle->idStartNode=1;
		pParaAngle->m_bNecessaryRod=true;
		pParaAngle->bUpdateStartPos=true;
		pParaAngle->ciAngleWorkWing='Y';
		pParaAngle->desStart.szXorCtrlWingOffset="g";
		pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";
		pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
		pParaAngle->idStartNode=1;
		pParaAngle->m_bNecessaryRod=true;
		//��ͨ���Ӱ�
		pParaPlate=this->listParaPlates.Append();
		pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;	
		//pParaPlate->cbMirCreateFlag=0x01;
		pParaPlate->design.idBaseNode=1;
		pParaPlate->design.idBaseRod=1;
		pParaPlate->design.ciProfileStyle=1;	//��������
		pParaPlate->design.ciDatumLenAxis='Y';
		pParaPlate->design.ciDatumAngleWing='Y';
		pParaPlate->design.bend.widAngleBendBy=0;
		pParaPlate->connrods.Clear();
		//1.ˮƽ����ֱ��
		pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 5, true);
		pParaPlate->connrods[0].InitTipInfo(2, 2);
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=1;
		pParaPlate->connrods[0].layoutbolts[0].ciDatumWing=0;
		pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[0].layoutbolts.ReSize(1);
		//2.�ϲ�����
		pParaPlate->connrods[1].InitBasicInfo(2, true, 1, 2, false);
		pParaPlate->connrods[1].InitTipInfo(0, 0, 1);
		pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=0;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;
		pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[1].layoutbolts.ReSize(1);
		//3.�²�֧�Ÿ�
		pParaPlate->connrods[2].InitBasicInfo(3, true, 1, 3, false);
		pParaPlate->connrods[2].InitTipInfo(0, 0, 1);
		pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=0;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;
		pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[2].layoutbolts.ReSize(1);
		pParaPlate->connrods.ReSize(3);
	}
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_XH_DX3)
		InitNodePlateTemp_DX3();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_XH_DX3A)
		InitNodePlateTemp_DX3A();
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_XH_HD14)
	{
		STDTEMPL_PARAM_ITEM* pParam;
		//�ڵ㹹���׼װ������ϵ����
		pParam=this->listParams.Add(KEY4C("acs"));
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
		pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
		pParam->value.objtype.wiPropValue=0;
		strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8

		pParaNode=this->listParaNodes.Add(1);
		pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
		//pParaAngle->idStartNode=1;
		pParaAngle->m_bNecessaryRod=true;
		pParaAngle->ciAngleWorkWing='X';
		pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
		pParaAngle->idStartNode=1;
		pParaAngle->ciAngleWorkWing='Y';
		pParaAngle->m_bNecessaryRod=true;
		pParaAngle->bUpdateStartPos=true;
		pParaAngle->desStart.cTouchLineType=0;
		pParaAngle->desStart.cTouchLine=1;//'Y';
		pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";
		pParaAngle->desStart.szYorOthrWingOffset="g";
		pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
		pParaAngle->idStartNode=1;
		pParaAngle->m_bNecessaryRod=true;
		pParaAngle->ciAngleWorkWing='Y';
		pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
		pParaAngle->idStartNode=1;
		pParaAngle->m_bNecessaryRod=true;
		pParaAngle->ciAngleWorkWing='X';
		CStdTemplParaPlate* pParaPlate=this->listParaPlates.Add(1);
		pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
		pParaPlate->cbMirCreateFlag=0x01;
		pParaPlate->design.idBaseNode=1;
		pParaPlate->design.idBaseRod=1;
		pParaPlate->design.ciProfileStyle=1;	//��������
		pParaPlate->design.ciDatumLenAxis='Y';
		pParaPlate->design.ciDatumAngleWing='X';
		pParaPlate->design.bend.widAngleBendBy=1;
		pParaPlate->design.bend.ciBendMode=3;
		pParaPlate->design.bend.normal.widBendPlaneRefRod1=1;
		pParaPlate->design.bend.normal.widBendPlaneRefRod2=4;
		pParaPlate->connrods.Clear();
		//����ֱͨ��
		pParaPlate->connrods[0].sidRod=1;
		pParaPlate->connrods[0].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[0].ciCurrTipSideS0E1=2;
		pParaPlate->connrods[0].ciPlaneNo=1;
		pParaPlate->connrods[0].cnMinBoltN=5;
		pParaPlate->connrods[0].dualSideEdge=true;
		pParaPlate->connrods[0].ciConnectWing='X';
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[0].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=1;
		pParaPlate->connrods[0].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[0].layoutbolts.ReSize(1);
		//��б֧�Ÿ�
		pParaPlate->connrods[1].sidRod=2;
		pParaPlate->connrods[1].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[1].ciCurrTipSideS0E1=0;
		pParaPlate->connrods[1].ciOddCalStyle=0;
		pParaPlate->connrods[1].sidOddRefRod[0]=1;
		pParaPlate->connrods[1].sidOddRefRod[1]=3;
		pParaPlate->connrods[1].ciPlaneNo=1;
		pParaPlate->connrods[1].cnMinBoltN=2;
		pParaPlate->connrods[1].dualSideEdge=true;
		pParaPlate->connrods[1].ciConnectWing='Y';
		pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=0;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[1].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[1].layoutbolts.ReSize(1);
		//��֧�Ÿ�
		pParaPlate->connrods[2].sidRod=3;
		pParaPlate->connrods[2].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[2].ciCurrTipSideS0E1=0;
		pParaPlate->connrods[2].ciPlaneNo=1;
		pParaPlate->connrods[2].ciOddCalStyle=0;
		pParaPlate->connrods[2].sidOddRefRod[0]=1;
		pParaPlate->connrods[2].sidOddRefRod[1]=2;
		pParaPlate->connrods[2].cnMinBoltN=2;
		pParaPlate->connrods[2].dualSideEdge=true;
		pParaPlate->connrods[2].ciConnectWing='Y';
		pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=0;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[2].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[2].layoutbolts.ReSize(1);
		//�ᵣˮƽ��
		pParaPlate->connrods[3].sidRod=4;
		pParaPlate->connrods[3].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[3].ciCurrTipSideS0E1=0;
		pParaPlate->connrods[3].ciPlaneNo=1;
		pParaPlate->connrods[3].ciOddCalStyle=0;
		pParaPlate->connrods[3].sidOddRefRod[0]=1;
		pParaPlate->connrods[3].cnMinBoltN=2;
		pParaPlate->connrods[3].dualSideEdge=true;
		pParaPlate->connrods[3].ciConnectWing='X';
		pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle=0;
		pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[3].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[3].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[3].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[3].layoutbolts.ReSize(1);
		pParaPlate->connrods.ReSize(4);
	}
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_XH_HD13)
	{
		STDTEMPL_PARAM_ITEM* pParam;
		//�ڵ㹹���׼װ������ϵ����
		pParam=this->listParams.Add(KEY4C("acs"));
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
		pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
		pParam->value.objtype.wiPropValue=0;
		strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8

		CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
		CStdTemplParaRod* pParaRod=this->listParaRods.Add(1);
		pParaRod->m_bNecessaryRod=true;
		pParaRod=this->listParaRods.Add(2);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		pParaRod=this->listParaRods.Add(3);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		pParaRod=this->listParaRods.Add(4);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		pParaRod=this->listParaRods.Add(5);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		CStdTemplParaPlate* pParaPlate=this->listParaPlates.Append();
		pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
		pParaPlate->cbMirCreateFlag=0x01;
		pParaPlate->design.idBaseNode=1;
		pParaPlate->design.idBaseRod=1;
		pParaPlate->design.ciProfileStyle=1;	//��������
		pParaPlate->design.ciDatumLenAxis='Y';
		pParaPlate->design.ciDatumAngleWing='Y';
		pParaPlate->design.bend.widAngleBendBy=0;
		pParaPlate->connrods.Clear();
		//1.����ֱͨ��
		pParaPlate->connrods[0].sidRod=1;
		pParaPlate->connrods[0].dynaRod.siRayMinDegAngle=20;
		pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle=80;
		pParaPlate->connrods[0].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[0].ciCurrTipSideS0E1=1;
		pParaPlate->connrods[0].ciPlaneNo=1;
		pParaPlate->connrods[0].cnMinBoltN=5;
		pParaPlate->connrods[0].dualSideEdge=false;
		pParaPlate->connrods[0].ciConnectWing='Y';
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[0].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=1;
		pParaPlate->connrods[0].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[0].layoutbolts.ReSize(1);
		//2.�¸���֧�Ÿ�
		pParaPlate->connrods[1].sidRod=2;
		pParaPlate->connrods[1].dynaRod.siRayMinDegAngle=155;
		pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle=180;
		pParaPlate->connrods[1].ciNecessity=2;	//��Ҫ�˼�
		pParaPlate->connrods[1].ciCurrTipSideS0E1=1;
		pParaPlate->connrods[1].ciPlaneNo=1;
		pParaPlate->connrods[1].cnMinBoltN=1;
		pParaPlate->connrods[1].dualSideEdge=false;
		pParaPlate->connrods[1].ciConnectWing='Y';
		pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[1].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[1].layoutbolts.ReSize(1);
		//3.�ᵣб����
		pParaPlate->connrods[2].sidRod=3;
		pParaPlate->connrods[2].dynaRod.siRayMinDegAngle=110;
		pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle=150;
		pParaPlate->connrods[2].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[2].ciCurrTipSideS0E1=1;
		pParaPlate->connrods[2].ciPlaneNo=1;
		pParaPlate->connrods[2].cnMinBoltN=3;
		pParaPlate->connrods[2].dualSideEdge=false;
		pParaPlate->connrods[2].ciConnectWing='Y';
		pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[2].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[2].layoutbolts.ReSize(1);
		//4.����б֧�Ÿ�
		pParaPlate->connrods[3].sidRod=4;
		pParaPlate->connrods[3].dynaRod.siRayMinDegAngle=90;
		pParaPlate->connrods[3].dynaRod.siRayMaxDegAngle=130;
		pParaPlate->connrods[3].ciNecessity=2;	//��Ҫ�˼�
		pParaPlate->connrods[3].ciCurrTipSideS0E1=1;
		pParaPlate->connrods[3].ciPlaneNo=1;
		pParaPlate->connrods[3].cnMinBoltN=1;
		pParaPlate->connrods[3].dualSideEdge=false;
		pParaPlate->connrods[3].ciConnectWing='Y';
		pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[3].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[3].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[3].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[3].layoutbolts.ReSize(1);
		//5.ˮƽ��
		pParaPlate->connrods[4].sidRod=5;
		pParaPlate->connrods[4].dynaRod.siRayMinDegAngle=179;
		pParaPlate->connrods[4].dynaRod.siRayMaxDegAngle=181;
		pParaPlate->connrods[4].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[4].ciCurrTipSideS0E1=1;
		pParaPlate->connrods[4].ciPlaneNo=1;
		pParaPlate->connrods[4].cnMinBoltN=3;
		pParaPlate->connrods[4].dualSideEdge=false;
		pParaPlate->connrods[4].ciConnectWing='Y';
		pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[4].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[4].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[4].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[4].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[4].layoutbolts.ReSize(1);
		pParaPlate->connrods.ReSize(5);
	}
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_SZK1)//ZK-03-3
	{
		this->name = "SZK-1";
		this->szSketchImgFile = "SZK-1.bmp";
		this->m_bEnableIntelliConnPointDesign = true;
		//�ڵ㹹���׼װ������ϵ����
		STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
		pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
		pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
		pParam->value.objtype.wiPropValue = 0;
		strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8

		CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
		pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
		pParaAngle->m_bNecessaryRod = true;
		pParaAngle->ciAngleWorkWing = 'X';
		pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
		pParaAngle->idStartNode = 1;
		pParaAngle->m_bNecessaryRod = true;
		pParaAngle->ciAngleWorkWing = 'y';
		pParaAngle->xFlag.bFilterInsideAngle = true;
		pParaAngle->bUpdateStartPos = true;
		pParaAngle->desStart.cTouchLineType = 0;
		pParaAngle->desStart.cTouchLine = 0;//'X';
		pParaAngle->desStart.szXorCtrlWingOffset = "g";
		pParaAngle->desStart.szYorOthrWingOffset = "]=-R1#T|[=-P1#T";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
		pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
		pParaAngle->idStartNode = 1;
		pParaAngle->m_bNecessaryRod = true;
		pParaAngle->ciAngleWorkWing = 'X';
		pParaAngle->xFlag.bFilterInsideAngle = true;
		pParaAngle->bUpdateStartPos = true;
		pParaAngle->desStart.cTouchLineType = 0;
		pParaAngle->desStart.cTouchLine = 0;//'X';
		pParaAngle->desStart.szXorCtrlWingOffset = "]=-R1#T|[=-P1#T";
		pParaAngle->desStart.szYorOthrWingOffset = "g";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
		CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
		pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
		pParaPlate->cbMirCreateFlag=0x01;
		pParaPlate->design.idBaseNode = 1;
		pParaPlate->design.idBaseRod = 1;
		pParaPlate->design.ciProfileStyle = 1;	//��������
		pParaPlate->design.ciDatumLenAxis = 'Y';
		pParaPlate->design.ciDatumAngleWing = 'Y';
		pParaPlate->design.bend.widAngleBendBy = 0;
		pParaPlate->connrods.Clear();
		//1.������
		pParaPlate->connrods[0].sidRod = 1;
		//pParaPlate->connrods[0].dynaRod.siRayMinDegAngle = 89;
		//pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle = 95;
		pParaPlate->connrods[0].ciNecessity = 1;	//��Ҫ�˼�
		pParaPlate->connrods[0].ciCurrTipSideS0E1 = 2;
		pParaPlate->connrods[0].ciPlaneNo = 1;
		pParaPlate->connrods[0].cnMinBoltN = 5;
		pParaPlate->connrods[0].cnMaxSingularLineBolts = 1;	//������˨��
		pParaPlate->connrods[0].dualSideEdge = true;
		pParaPlate->connrods[0].ciConnectWing = 'X';
		//��һ��ֱ������˨λ�����߸˼����׼�Ľ�����
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = BOLT_ORIGIN::LOCA_AXISINTERS;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widCrossAngle=2;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciStyleG = 0;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciStyleCrossG = 0;
		pParaPlate->connrods[0].layoutbolts[0].idDirectConnectToAngle1 = 2;
		pParaPlate->connrods[0].layoutbolts[0].idDirectConnectToAngle2 = 3;
		pParaPlate->connrods[0].layoutbolts[0].cnCount = 1;
		pParaPlate->connrods[0].layoutbolts[0].ciRows = 0;
		pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 0;
		pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;//'Y';
		pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		//������˨λ�����߸˼�2��3�����˫�Ų����ڻ�׼����
		pParaPlate->connrods[0].layoutbolts[1].basepoint.ciPosCalStyle = BOLT_ORIGIN::LOCA_KEEPSPACE2ROD;
		pParaPlate->connrods[0].layoutbolts[1].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[0].layoutbolts[1].basepoint.widBaseAngle=2; //��һ�����ռ�϶���߽Ǹ�
		pParaPlate->connrods[0].layoutbolts[1].basepoint.widCrossAngle=3;//�ڶ������ռ�϶���߽Ǹ�
		pParaPlate->connrods[0].layoutbolts[1].cnCount = 0;
		pParaPlate->connrods[0].layoutbolts[1].ciRows = 0;
		pParaPlate->connrods[0].layoutbolts[1].ciLayoutStyle = 1;
		pParaPlate->connrods[0].layoutbolts[1].ciDatumWing = 0;//'Y';
		pParaPlate->connrods[0].layoutbolts[1].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[0].layoutbolts.ReSize(2);
		//2.��б��
		pParaPlate->connrods[1].sidRod = 2;
		//pParaPlate->connrods[1].dynaRod.siRayMinDegAngle = 120;
		//pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle = 150;
		pParaPlate->connrods[1].ciNecessity = 1;	//��Ҫ�˼�
		pParaPlate->connrods[1].InitTipInfo(0,1);// ciCurrTipSideS0E1 = 0;
		pParaPlate->connrods[1].ciPlaneNo = 1;
		pParaPlate->connrods[1].cnMinBoltN = 3;
		pParaPlate->connrods[1].cnMinShareBoltN = 2;
		pParaPlate->connrods[1].cnMaxSingularLineBolts = 2;
		pParaPlate->connrods[1].dualSideEdge = false;
		pParaPlate->connrods[1].ciConnectWing = 0;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = BOLT_ORIGIN::LOCA_KEEPSPACE2ROD;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseAngle=1;
		pParaPlate->connrods[1].layoutbolts[0].cnCount = 0;
		pParaPlate->connrods[1].layoutbolts[0].ciRows = 0;
		pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
		pParaPlate->connrods[1].layoutbolts[0].cnMaxSingularLineBolts = 1;	//����˫�Ų���
		pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;//'Y';
		pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[1].layoutbolts.ReSize(1);
		//3.��б��
		pParaPlate->connrods[2].sidRod = 3;
		//pParaPlate->connrods[2].dynaRod.siRayMinDegAngle = 200;
		//pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle = 230;
		pParaPlate->connrods[2].ciNecessity = 1;	//��Ҫ�˼�
		pParaPlate->connrods[2].InitTipInfo(0,1);// ciCurrTipSideS0E1 = 0;
		pParaPlate->connrods[2].ciPlaneNo = 1;
		pParaPlate->connrods[2].cnMinBoltN = 3;
		pParaPlate->connrods[2].cnMinShareBoltN = 2;
		pParaPlate->connrods[2].cnMaxSingularLineBolts = 2;
		pParaPlate->connrods[2].dualSideEdge = false;
		pParaPlate->connrods[2].ciConnectWing = 0;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = BOLT_ORIGIN::LOCA_KEEPSPACE2ROD;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseAngle=1;
		pParaPlate->connrods[2].layoutbolts[0].cnCount = 0;
		pParaPlate->connrods[2].layoutbolts[0].ciRows = 0;
		pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
		pParaPlate->connrods[2].layoutbolts[0].cnMaxSingularLineBolts = 1;	//����˫�Ų���
		pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;//'Y';
		pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[2].layoutbolts.ReSize(1);
		pParaPlate->connrods.ReSize(3);
	}
	//else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_ZK1_NW4)//ZK-04
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_SZK2)//ZK-03-2
	{
		this->name = "SZK-2";
		this->szSketchImgFile="SZK-2.bmp";
		this->m_bEnableIntelliConnPointDesign = true;
		//�ڵ㹹���׼װ������ϵ����
		STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
		pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
		pParam->value.objtype.wiPropValue=0;
		strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8

		CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
		pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
		pParaAngle->m_bNecessaryRod = true;
		pParaAngle->ciAngleWorkWing = 'X';
		pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
		pParaAngle->idStartNode = 1;
		pParaAngle->m_bNecessaryRod = true;
		pParaAngle->ciAngleWorkWing = 'y';
		pParaAngle->xFlag.bFilterInsideAngle = true;
		pParaAngle->bUpdateStartPos = true;
		pParaAngle->desStart.cTouchLineType = 0;
		pParaAngle->desStart.cTouchLine = 0;//'X';
		pParaAngle->desStart.szXorCtrlWingOffset = "g";
		pParaAngle->desStart.szYorOthrWingOffset = "[=-P1#T";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
		pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
		pParaAngle->idStartNode = 1;
		pParaAngle->m_bNecessaryRod = true;
		pParaAngle->ciAngleWorkWing = 'X';
		pParaAngle->xFlag.bFilterInsideAngle = true;
		pParaAngle->bUpdateStartPos = true;
		pParaAngle->desStart.cTouchLineType = 0;
		pParaAngle->desStart.cTouchLine = 0;//'X';
		pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
		pParaAngle->desStart.szYorOthrWingOffset = "g";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
		CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
		pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
		pParaPlate->cbMirCreateFlag=0x01;
		pParaPlate->design.idBaseNode = 1;
		pParaPlate->design.idBaseRod = 1;
		pParaPlate->design.ciProfileStyle = 1;	//��������
		pParaPlate->design.ciDatumLenAxis = 'Y';
		pParaPlate->design.ciDatumAngleWing = 'Y';
		pParaPlate->design.bend.widAngleBendBy = 0;
		pParaPlate->connrods.Clear();
		//1.������
		pParaPlate->connrods[0].sidRod = 1;
		//pParaPlate->connrods[0].dynaRod.siRayMinDegAngle = 89;
		//pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle = 95;
		pParaPlate->connrods[0].ciNecessity = 1;	//��Ҫ�˼�
		pParaPlate->connrods[0].ciCurrTipSideS0E1 = 2;
		pParaPlate->connrods[0].ciPlaneNo = 1;
		pParaPlate->connrods[0].cnMinBoltN = 5;
		pParaPlate->connrods[0].cnMaxSingularLineBolts = 1;	//������˨��
		pParaPlate->connrods[0].dualSideEdge = true;
		pParaPlate->connrods[0].ciConnectWing = 'X';
		//��һ��ֱ������˨λ�����߸˼����׼�Ľ�����
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = BOLT_ORIGIN::LOCA_AXISINTERS;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widCrossAngle=3;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciStyleG = 0;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciStyleCrossG = 0;
		pParaPlate->connrods[0].layoutbolts[0].idDirectConnectToAngle1 = 3;
		pParaPlate->connrods[0].layoutbolts[0].cnCount = 1;
		pParaPlate->connrods[0].layoutbolts[0].ciRows = 0;
		pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 0;
		pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;//'Y';
		pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		//������˨λ�����߸˼�2��3�����˫�Ų����ڻ�׼����
		pParaPlate->connrods[0].layoutbolts[1].basepoint.ciPosCalStyle = BOLT_ORIGIN::LOCA_KEEPSPACE2ROD;
		pParaPlate->connrods[0].layoutbolts[1].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[0].layoutbolts[1].basepoint.widBaseAngle=3;
		pParaPlate->connrods[0].layoutbolts[1].cnCount = 0;
		pParaPlate->connrods[0].layoutbolts[1].ciRows = 0;
		pParaPlate->connrods[0].layoutbolts[1].ciLayoutStyle = 1;
		pParaPlate->connrods[0].layoutbolts[1].ciDatumWing = 0;//'Y';
		pParaPlate->connrods[0].layoutbolts[1].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[0].layoutbolts.ReSize(2);
		//2.��б��
		pParaPlate->connrods[1].sidRod = 2;
		//pParaPlate->connrods[1].dynaRod.siRayMinDegAngle = 120;
		//pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle = 150;
		pParaPlate->connrods[1].ciNecessity = 1;	//��Ҫ�˼�
		pParaPlate->connrods[1].InitTipInfo(0,0,1);// ciCurrTipSideS0E1 = 0;
		pParaPlate->connrods[1].ciPlaneNo = 1;
		pParaPlate->connrods[1].cnMinBoltN = 3;
		pParaPlate->connrods[1].cnMinShareBoltN = 2;
		pParaPlate->connrods[1].cnMaxSingularLineBolts = 3;
		pParaPlate->connrods[1].dualSideEdge = false;
		pParaPlate->connrods[1].ciConnectWing = 0;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = BOLT_ORIGIN::LOCA_RODEND;//LOCA_KEEPSPACE2ROD;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseAngle=1;
		pParaPlate->connrods[1].layoutbolts[0].cnCount = 0;
		pParaPlate->connrods[1].layoutbolts[0].ciRows = 0;
		pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
		pParaPlate->connrods[1].layoutbolts[0].cnMaxSingularLineBolts = 3;	//����˫�Ų���
		pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;//'Y';
		pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[1].layoutbolts.ReSize(1);
		//3.��б��
		pParaPlate->connrods[2].sidRod = 3;
		//pParaPlate->connrods[2].dynaRod.siRayMinDegAngle = 200;
		//pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle = 230;
		pParaPlate->connrods[2].ciNecessity = 1;	//��Ҫ�˼�
		pParaPlate->connrods[2].InitTipInfo(0,1);// ciCurrTipSideS0E1 = 0;
		pParaPlate->connrods[2].ciPlaneNo = 1;
		pParaPlate->connrods[2].cnMinBoltN = 3;
		pParaPlate->connrods[2].cnMinShareBoltN = 1;
		pParaPlate->connrods[2].cnMaxSingularLineBolts = 1;
		pParaPlate->connrods[2].dualSideEdge = false;
		pParaPlate->connrods[2].ciConnectWing = 0;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = BOLT_ORIGIN::LOCA_KEEPSPACE2ROD;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseAngle=1;
		pParaPlate->connrods[2].layoutbolts[0].cnCount = 0;
		pParaPlate->connrods[2].layoutbolts[0].ciRows = 0;
		pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
		pParaPlate->connrods[2].layoutbolts[0].cnMaxSingularLineBolts = 1;	//����˫�Ų���
		pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;//'Y';
		pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[2].layoutbolts.ReSize(1);
		pParaPlate->connrods.ReSize(3);
	}
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_SZK5)//ZK-02-1
	{
		this->name = "SZK-5";
		this->szSketchImgFile = "SZK-5.bmp";
		this->m_bEnableIntelliConnPointDesign = true;
		//�ڵ㹹���׼װ������ϵ����
		STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
		pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
		pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
		pParam->value.objtype.wiPropValue = 0;
		strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8

		CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
		CStdTemplParaRod* pParaRod = this->listParaRods.Add(1);
		pParaRod->m_bNecessaryRod = true;
		pParaRod = this->listParaRods.Add(2);
		pParaRod->idStartNode = 1;
		pParaRod->m_bNecessaryRod = true;
		pParaRod = this->listParaRods.Add(3);
		pParaRod->idStartNode = 1;
		pParaRod->m_bNecessaryRod = true;
		CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
		pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
		pParaPlate->cbMirCreateFlag=0x01;
		pParaPlate->design.idBaseNode = 1;
		pParaPlate->design.idBaseRod = 1;
		pParaPlate->design.ciProfileStyle = 1;	//��������
		pParaPlate->design.ciDatumLenAxis = 'Y';
		pParaPlate->design.ciDatumAngleWing = 'Y';
		pParaPlate->design.bend.widAngleBendBy = 0;
		pParaPlate->connrods.Clear();
		//1.������
		pParaPlate->connrods[0].sidRod = 1;
		pParaPlate->connrods[0].dynaRod.siRayMinDegAngle = 89;
		pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle = 95;
		pParaPlate->connrods[0].ciNecessity = 1;	//��Ҫ�˼�
		pParaPlate->connrods[0].ciCurrTipSideS0E1 = 2;
		pParaPlate->connrods[0].ciPlaneNo = 1;
		pParaPlate->connrods[0].cnMinBoltN = 5;
		pParaPlate->connrods[0].dualSideEdge = false;
		pParaPlate->connrods[0].ciConnectWing = 'X';
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[0].layoutbolts[0].cnCount = 0;
		pParaPlate->connrods[0].layoutbolts[0].ciRows = 1;
		pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
		pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;//'Y';
		pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[0].layoutbolts.ReSize(1);
		//2.��б��
		pParaPlate->connrods[1].sidRod = 2;
		pParaPlate->connrods[1].dynaRod.siRayMinDegAngle = 120;
		pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle = 150;
		pParaPlate->connrods[1].ciNecessity = 1;	//��Ҫ�˼�
		pParaPlate->connrods[1].ciCurrTipSideS0E1 = 0;
		pParaPlate->connrods[1].ciPlaneNo = 1;
		pParaPlate->connrods[1].cnMinBoltN = 3;
		pParaPlate->connrods[1].dualSideEdge = false;
		pParaPlate->connrods[1].ciConnectWing = 'Y';
		pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 1;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[1].layoutbolts[0].cnCount = 0;
		pParaPlate->connrods[1].layoutbolts[0].ciRows = 1;
		pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
		pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;//'Y';
		pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[1].layoutbolts.ReSize(1);
		//3.��б��
		pParaPlate->connrods[2].sidRod = 3;
		pParaPlate->connrods[2].dynaRod.siRayMinDegAngle = 200;
		pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle = 230;
		pParaPlate->connrods[2].ciNecessity = 1;	//��Ҫ�˼�
		pParaPlate->connrods[2].ciCurrTipSideS0E1 = 0;
		pParaPlate->connrods[2].ciPlaneNo = 1;
		pParaPlate->connrods[2].cnMinBoltN = 2;
		pParaPlate->connrods[2].dualSideEdge = false;
		pParaPlate->connrods[2].ciConnectWing = 'Y';
		pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 1;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[2].layoutbolts[0].cnCount = 0;
		pParaPlate->connrods[2].layoutbolts[0].ciRows = 1;
		pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
		pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;//'Y';
		pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[2].layoutbolts.ReSize(1);
		pParaPlate->connrods.ReSize(3);
	}
	else if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_SZK6)
	{	//ȫ��ͷ
		this->name = "SZK-6";
		this->szSketchImgFile = "SZK-6.bmp";
		this->m_bEnableIntelliConnPointDesign = true;
		//�ڵ㹹���׼װ������ϵ����
		STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
		pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
		pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
		pParam->value.objtype.wiPropValue = 0;
		strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8

		CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
		pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
		pParaAngle->m_bNecessaryRod = true;
		pParaAngle->ciAngleWorkWing = 'X';
		pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
		pParaAngle->idStartNode = 1;
		pParaAngle->m_bNecessaryRod = true;
		pParaAngle->ciAngleWorkWing = 'Y';
		pParaAngle->bUpdateStartPos = true;
		pParaAngle->desStart.cTouchLineType = 0;
		pParaAngle->desStart.cTouchLine = 0;//'X';
		pParaAngle->desStart.szXorCtrlWingOffset = "g";
		pParaAngle->desStart.szYorOthrWingOffset = "[=-P1#T";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
		pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
		pParaAngle->idStartNode = 1;
		pParaAngle->m_bNecessaryRod = true;
		pParaAngle->ciAngleWorkWing = 'X';
		pParaAngle->bUpdateStartPos = true;
		pParaAngle->desStart.cTouchLineType = 0;
		pParaAngle->desStart.cTouchLine = 0;//'X';
		pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
		pParaAngle->desStart.szYorOthrWingOffset = "g";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
		CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
		pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
		pParaPlate->cbMirCreateFlag=0x01;
		pParaPlate->design.idBaseNode = 1;
		pParaPlate->design.idBaseRod = 1;
		pParaPlate->design.ciProfileStyle = 1;	//��������
		pParaPlate->design.ciDatumLenAxis = 'Y';
		pParaPlate->design.ciDatumAngleWing = 'X';
		pParaPlate->design.bend.widAngleBendBy = 0;
		pParaPlate->connrods.Clear();
		//1.������
		pParaPlate->connrods[0].sidRod = 1;
		//pParaPlate->connrods[0].dynaRod.siRayMinDegAngle = 89;
		//pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle = 95;
		pParaPlate->connrods[0].ciNecessity = 1;	//��Ҫ�˼�
		pParaPlate->connrods[0].ciCurrTipSideS0E1 = 2;
		pParaPlate->connrods[0].ciPlaneNo = 1;
		pParaPlate->connrods[0].cnMinBoltN = 3;
		pParaPlate->connrods[0].dualSideEdge = true;
		pParaPlate->connrods[0].ciConnectWing = 'X';
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[0].layoutbolts[0].cnCount = 0;
		pParaPlate->connrods[0].layoutbolts[0].ciRows = 0;
		pParaPlate->connrods[0].layoutbolts[0].cnMaxSingularLineBolts = 4;
		pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
		pParaPlate->connrods[0].layoutbolts[0].ciDatumWing = 0;//'Y';
		pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[0].layoutbolts.ReSize(1);
		//2.��б��
		pParaPlate->connrods[1].sidRod = 2;
		//pParaPlate->connrods[1].dynaRod.siRayMinDegAngle = 120;
		//pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle = 150;
		pParaPlate->connrods[1].ciNecessity = 1;	//��Ҫ�˼�
		pParaPlate->connrods[1].InitTipInfo(0,0,1);
		pParaPlate->connrods[1].ciPlaneNo = 1;
		pParaPlate->connrods[1].cnMinBoltN = 2;
		pParaPlate->connrods[1].dualSideEdge = false;
		pParaPlate->connrods[1].ciConnectWing = 'Y';
		pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[1].layoutbolts[0].cnCount = 0;
		pParaPlate->connrods[1].layoutbolts[0].ciRows = 0;
		pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
		pParaPlate->connrods[1].layoutbolts[0].ciDatumWing = 0;//'Y';
		pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[1].layoutbolts.ReSize(1);
		//3.��б��
		pParaPlate->connrods[2].sidRod = 3;
		//pParaPlate->connrods[2].dynaRod.siRayMinDegAngle = 200;
		//pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle = 230;
		pParaPlate->connrods[2].ciNecessity = 1;	//��Ҫ�˼�
		pParaPlate->connrods[2].InitTipInfo(0,0,1);
		pParaPlate->connrods[2].ciPlaneNo = 1;
		pParaPlate->connrods[2].cnMinBoltN = 2;
		pParaPlate->connrods[2].dualSideEdge = false;
		pParaPlate->connrods[2].ciConnectWing = 'X';
		pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
		pParaPlate->connrods[2].layoutbolts[0].cnCount = 0;
		pParaPlate->connrods[2].layoutbolts[0].ciRows = 0;
		pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
		pParaPlate->connrods[2].layoutbolts[0].ciDatumWing = 0;//'Y';
		pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[2].layoutbolts.ReSize(1);
		pParaPlate->connrods.ReSize(3);
	}
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_ZK1_N1)//ZK-12-1
	{
		szSketchImgFile = "ZK1_N1.bmp";
		//�ڵ㹹���׼װ������ϵ����
		STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
		pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
		pParam->value.objtype.wiPropValue=0;
		strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8

		CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
		CStdTemplParaRod* pParaRod=this->listParaRods.Add(1);
		pParaRod->m_bNecessaryRod=true;
		pParaRod=this->listParaRods.Add(2);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		pParaRod=this->listParaRods.Add(3);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		CStdTemplParaPlate* pParaPlate=this->listParaPlates.Append();
		pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
		pParaPlate->cbMirCreateFlag=0x01;
		pParaPlate->design.idBaseNode=1;
		pParaPlate->design.idBaseRod=1;
		pParaPlate->design.ciProfileStyle=1;	//��������
		pParaPlate->design.ciDatumLenAxis='Y';
		pParaPlate->design.ciDatumAngleWing='Y';
		pParaPlate->design.bend.widAngleBendBy=0;
		pParaPlate->connrods.Clear();
		//1.������
		pParaPlate->connrods[0].sidRod=1;
		pParaPlate->connrods[0].dynaRod.siRayMinDegAngle=89;
		pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle=95;
		pParaPlate->connrods[0].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[0].ciCurrTipSideS0E1=2;
		pParaPlate->connrods[0].ciPlaneNo=1;
		pParaPlate->connrods[0].cnMinBoltN=3;
		pParaPlate->connrods[0].dualSideEdge=false;
		pParaPlate->connrods[0].ciConnectWing='X';
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[0].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=1;
		pParaPlate->connrods[0].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[0].layoutbolts.ReSize(1);
		//2.��б��
		pParaPlate->connrods[1].sidRod=2;
		pParaPlate->connrods[1].dynaRod.siRayMinDegAngle=120;
		pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle=150;
		pParaPlate->connrods[1].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[1].ciCurrTipSideS0E1=0;
		pParaPlate->connrods[1].ciPlaneNo=1;
		pParaPlate->connrods[1].cnMinBoltN=2;
		pParaPlate->connrods[1].dualSideEdge=false;
		pParaPlate->connrods[1].ciConnectWing='Y';
		pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[1].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[1].layoutbolts.ReSize(1);
		//3.��б��
		pParaPlate->connrods[2].sidRod=3;
		pParaPlate->connrods[2].dynaRod.siRayMinDegAngle=200;
		pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle=230;
		pParaPlate->connrods[2].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[2].ciCurrTipSideS0E1=0;
		pParaPlate->connrods[2].ciPlaneNo=1;
		pParaPlate->connrods[2].cnMinBoltN=1;
		pParaPlate->connrods[2].dualSideEdge=false;
		pParaPlate->connrods[2].ciConnectWing='Y';
		pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[2].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[2].layoutbolts.ReSize(1);
		pParaPlate->connrods.ReSize(3);
	}
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_ZK1_N2)//ZK-12-2
	{
		szSketchImgFile = "ZK1_N2.bmp";
		//�ڵ㹹���׼װ������ϵ����
		STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
		pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
		pParam->value.objtype.wiPropValue=0;
		strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8

		CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
		CStdTemplParaRod* pParaRod=this->listParaRods.Add(1);
		pParaRod->m_bNecessaryRod=true;
		pParaRod=this->listParaRods.Add(2);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		pParaRod=this->listParaRods.Add(3);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		CStdTemplParaPlate* pParaPlate=this->listParaPlates.Append();
		pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
		pParaPlate->cbMirCreateFlag=0x01;
		pParaPlate->design.idBaseNode=1;
		pParaPlate->design.idBaseRod=1;
		pParaPlate->design.ciProfileStyle=1;	//��������
		pParaPlate->design.ciDatumLenAxis='Y';
		pParaPlate->design.ciDatumAngleWing='Y';
		pParaPlate->design.bend.widAngleBendBy=0;
		pParaPlate->connrods.Clear();
		//1.������
		pParaPlate->connrods[0].sidRod=1;
		pParaPlate->connrods[0].dynaRod.siRayMinDegAngle=89;
		pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle=95;
		pParaPlate->connrods[0].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[0].ciCurrTipSideS0E1=2;
		pParaPlate->connrods[0].ciPlaneNo=1;
		pParaPlate->connrods[0].cnMinBoltN=3;
		pParaPlate->connrods[0].dualSideEdge=false;
		pParaPlate->connrods[0].ciConnectWing='X';
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[0].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=1;
		pParaPlate->connrods[0].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[0].layoutbolts.ReSize(1);
		//2.��б��
		pParaPlate->connrods[1].sidRod=2;
		pParaPlate->connrods[1].dynaRod.siRayMinDegAngle=120;
		pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle=150;
		pParaPlate->connrods[1].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[1].ciCurrTipSideS0E1=0;
		pParaPlate->connrods[1].ciPlaneNo=1;
		pParaPlate->connrods[1].cnMinBoltN=1;
		pParaPlate->connrods[1].dualSideEdge=false;
		pParaPlate->connrods[1].ciConnectWing='Y';
		pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[1].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[1].layoutbolts.ReSize(1);
		//3.��б��
		pParaPlate->connrods[2].sidRod=3;
		pParaPlate->connrods[2].dynaRod.siRayMinDegAngle=200;
		pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle=230;
		pParaPlate->connrods[2].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[2].ciCurrTipSideS0E1=0;
		pParaPlate->connrods[2].ciPlaneNo=1;
		pParaPlate->connrods[2].cnMinBoltN=2;
		pParaPlate->connrods[2].dualSideEdge=false;
		pParaPlate->connrods[2].ciConnectWing='Y';
		pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[2].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[2].layoutbolts.ReSize(1);
		pParaPlate->connrods.ReSize(3);
	}
	else if (pTemplSource->idStdComTempl==STDTEMPL_NODEPLATE_ZK1_N3)//ZK-13
	{
		szSketchImgFile = "ZK1_N3.bmp";
		//�ڵ㹹���׼װ������ϵ����
		STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
		pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
		pParam->value.objtype.wiPropValue=0;
		strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8

		CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
		CStdTemplParaRod* pParaRod=this->listParaRods.Add(1);
		pParaRod->m_bNecessaryRod=true;
		pParaRod=this->listParaRods.Add(2);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		pParaRod=this->listParaRods.Add(3);
		pParaRod->idStartNode=1;
		pParaRod->m_bNecessaryRod=true;
		CStdTemplParaPlate* pParaPlate=this->listParaPlates.Append();
		pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
		pParaPlate->cbMirCreateFlag=0x01;
		pParaPlate->design.idBaseNode=1;
		pParaPlate->design.idBaseRod=1;
		pParaPlate->design.ciProfileStyle=1;	//��������
		pParaPlate->design.ciDatumLenAxis='Y';
		pParaPlate->design.ciDatumAngleWing='Y';
		pParaPlate->design.bend.widAngleBendBy=0;
		pParaPlate->connrods.Clear();
		//1.������
		pParaPlate->connrods[0].sidRod=1;
		pParaPlate->connrods[0].dynaRod.siRayMinDegAngle=89;
		pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle=95;
		pParaPlate->connrods[0].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[0].ciCurrTipSideS0E1=2;
		pParaPlate->connrods[0].ciPlaneNo=1;
		pParaPlate->connrods[0].cnMinBoltN=7;
		pParaPlate->connrods[0].dualSideEdge=false;
		pParaPlate->connrods[0].ciConnectWing='X';
		pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[0].layoutbolts[0].ciRows=2;
		pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=1;
		pParaPlate->connrods[0].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[0].layoutbolts.ReSize(1);
		//2.��б��
		pParaPlate->connrods[1].sidRod=2;
		pParaPlate->connrods[1].dynaRod.siRayMinDegAngle=120;
		pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle=150;
		pParaPlate->connrods[1].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[1].ciCurrTipSideS0E1=0;
		pParaPlate->connrods[1].ciPlaneNo=1;
		pParaPlate->connrods[1].cnMinBoltN=3;
		pParaPlate->connrods[1].dualSideEdge=false;
		pParaPlate->connrods[1].ciConnectWing='Y';
		pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[1].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[1].layoutbolts.ReSize(1);
		//3.��б��
		pParaPlate->connrods[2].sidRod=3;
		pParaPlate->connrods[2].dynaRod.siRayMinDegAngle=200;
		pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle=230;
		pParaPlate->connrods[2].ciNecessity=1;	//��Ҫ�˼�
		pParaPlate->connrods[2].ciCurrTipSideS0E1=0;
		pParaPlate->connrods[2].ciPlaneNo=1;
		pParaPlate->connrods[2].cnMinBoltN=2;
		pParaPlate->connrods[2].dualSideEdge=false;
		pParaPlate->connrods[2].ciConnectWing='Y';
		pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=1;
		pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
		pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
		pParaPlate->connrods[2].layoutbolts[0].ciRows=1;
		pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
		pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;//'Y';
		pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
		pParaPlate->connrods[2].layoutbolts.ReSize(1);
		pParaPlate->connrods.ReSize(3);
	}
}
bool CStdComTempl::ClearTmplParams()
{
	for (CStdTemplParaRod* pTmplRod=listParaRods.GetFirst();pTmplRod;pTmplRod=listParaRods.GetNext())
		pTmplRod->pRelaRod=NULL;
	m_ciMirrorMatchMode=0;
	hashMatchRodHid.Empty();
	hashMatchNodeHid.Empty();
	return true;
}
bool CStdComTempl::IsMeetConditions()
{
	return true;
}
bool CStdComTempl::RecogMatchRod(CStdTemplParaRod* pParaRod)
{
	CStdTemplParaNode* pStartParaNode=this->listParaNodes.GetValue(pParaRod->idStartNode);
	CStdTemplParaNode* pEndParaNode  =this->listParaNodes.GetValue(pParaRod->idEndNode);
	if (pStartParaNode == NULL || pEndParaNode == NULL)
		return false;
	CLDSNode *pMapStartNode=NULL,*pMapEndNode=NULL;
	if (pParaRod->idStartNode.ciMirTransType==0)
		pMapStartNode=pStartParaNode->pRelaNode;
	else
	{
		pStartParaNode  =this->listParaNodes.GetValue(pParaRod->idStartNode.uiOriginalId);
		if (pStartParaNode&&pStartParaNode->pRelaNode&&pParaRod->idStartNode.ciMirTransType=='X')
			pMapStartNode=pStartParaNode->pRelaNode->GetMirNode(MIRMSG(1));
		else if (pStartParaNode&&pStartParaNode->pRelaNode&&pParaRod->idStartNode.ciMirTransType=='Y')
			pMapStartNode=pStartParaNode->pRelaNode->GetMirNode(MIRMSG(2));
		else if (pStartParaNode&&pStartParaNode->pRelaNode&&pParaRod->idStartNode.ciMirTransType=='Z')
			pMapStartNode=pStartParaNode->pRelaNode->GetMirNode(MIRMSG(3));
	}
	if (pParaRod->idEndNode.ciMirTransType==0)
		pMapEndNode=pEndParaNode->pRelaNode;
	else
	{
		pEndParaNode  =this->listParaNodes.GetValue(pParaRod->idEndNode.uiOriginalId);
		if (pEndParaNode&&pEndParaNode->pRelaNode&&pParaRod->idEndNode.ciMirTransType=='X')
			pMapEndNode=pEndParaNode->pRelaNode->GetMirNode(MIRMSG(1));
		else if (pEndParaNode&&pEndParaNode->pRelaNode&&pParaRod->idEndNode.ciMirTransType=='Y')
			pMapEndNode=pEndParaNode->pRelaNode->GetMirNode(MIRMSG(2));
		else if (pEndParaNode&&pEndParaNode->pRelaNode&&pParaRod->idEndNode.ciMirTransType=='Z')
			pMapEndNode=pEndParaNode->pRelaNode->GetMirNode(MIRMSG(3));
	}

	if (pMapStartNode==NULL||pMapEndNode==NULL)
		return false;
	pParaRod->xStartPoint=pMapStartNode->xOriginalPos;
	pParaRod->xEndPoint  =pMapEndNode->xOriginalPos;
	bool bStdOcs=!ocs.origin.IsZero()||!ocs.axis_x.IsEqual(1,0,0)||!ocs.axis_y.IsEqual(0,1,0)||!ocs.axis_z.IsEqual(0,0,1);
	GEPOINT xStartPoint=!bStdOcs?ocs.TransPToCS(pParaRod->xStartPoint):pParaRod->xStartPoint;
	GEPOINT xEndPoint  =!bStdOcs?ocs.TransPToCS(pParaRod->xEndPoint):pParaRod->xEndPoint;
	/*
		STDTEMPL::CONNPOINT::MATCHROD rod;
		rod.pModelRod=pModelRod;
		if(pModelRod->pMappingRod->size_idClassType==CLS_LINEANGLE)
			rod.ciRodSectType='L';
		else if(pModelRod->pMappingRod->size_idClassType==CLS_GROUPLINEANGLE)
			rod.ciRodSectType=pModelRod->pMappingRod->size_cSubClassType;
		else if(pModelRod->pMappingRod->size_idClassType==CLS_LINETUBE)
			rod.ciRodSectType='O';
		else
			continue;
		if(pModelRod->pMappingRod->pStart==pBaseNode)
		{
			rod.ciRodType='R';
			rod.siActualRayAngle=(short)(0.5+Cal2dLineAng(xStartPoint.x,xStartPoint.y,xEndPoint.x,xEndPoint.y)*DEGTORAD_COEF);
		}
		else if(pModelRod->pMappingRod->pEnd==pBaseNode)
		{
			rod.ciRodType='R';
			rod.siActualRayAngle=(short)(0.5+Cal2dLineAng(xEndPoint.x,xEndPoint.y,xStartPoint.x,xStartPoint.y)*DEGTORAD_COEF);
		}
		else
		{
			if(pModelRod->pMappingRod->pStart==NULL||pModelRod->pMappingRod->pEnd==NULL)
				continue;
			CPtInLineCheck check(xStartPoint,xEndPoint);
			bool online=false;
			double ratio=check.CheckPointEx(pBaseNode->xOriginalPos,&online,1);
			if(!online||ratio<EPS||ratio>1-EPS)
				return false;
			rod.ciRodType='A';
			rod.siActualRayAngle=(short)(0.5+Cal2dLineAng(xStartPoint.x,xStartPoint.y,xEndPoint.x,xEndPoint.y)*DEGTORAD_COEF);
			if(rod.siActualRayAngle>=90&&rod.siActualRayAngle<180)
				rod.siActualRayAngle+=180;
			else if(rod.siActualRayAngle>=180&&rod.siActualRayAngle<270)
				rod.siActualRayAngle-=180;
		}
		rodarr.Append(rod);
	*/
	//��ȡ�����߽�˼�
	CLdsPartListStack stack(m_pBelongModel);
	CPtInLineCheck check(xStartPoint,xEndPoint);
	for (CLDSLinePart* pRod=m_pBelongModel->EnumRodFirst();pRod;pRod=m_pBelongModel->EnumRodNext())
	{
		if (pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		GEPOINT xActualStartPoint=!bStdOcs?ocs.TransPToCS(pRod->pStart->xOriginalPos):pRod->pStart->xOriginalPos;
		GEPOINT xActualEndPoint  =!bStdOcs?ocs.TransPToCS(pRod->pEnd->xOriginalPos):pRod->pEnd->xOriginalPos;
		if ((pRod->pStart==pMapStartNode&&pRod->pEnd==pMapEndNode)||
			(xStartPoint.IsEqual(xActualStartPoint)&&xEndPoint.IsEqual(xActualEndPoint)))
		{
			pParaRod->pRelaRod=pRod;
			pParaRod->relaRods[0]=CStdTemplParaRod::EDGEROD(pRod,0);
			break;
		}
		else if ((pRod->pStart==pMapEndNode&&pRod->pEnd==pMapStartNode)||
			(xStartPoint.IsEqual(xActualEndPoint)&&xEndPoint.IsEqual(xActualStartPoint)))
		{
			pParaRod->pRelaRod=pRod;
			pParaRod->relaRods[0]=CStdTemplParaRod::EDGEROD(pRod,0,true);
			break;
		}
		bool blOnLine=false;
		double scaleS=check.CheckPointEx(xActualStartPoint,&blOnLine);
		if (!blOnLine)
			continue;
		double scaleE=check.CheckPointEx(xActualEndPoint,&blOnLine);
		if (!blOnLine)
			continue;
		double minScale=min(0,min(scaleS,scaleE));
		double maxScale=max(1,max(scaleS,scaleE));
		if ((maxScale-minScale)*check.Length()>100)//EPS2)
		{
			pParaRod->pRelaRod=pRod;
			pParaRod->relaRods[0]=CStdTemplParaRod::EDGEROD(pRod,0,true);
			break;
		}
	}
#ifdef _DEBUG
	//if(pParaRod->pRelaRod==NULL)
	//{
	//	pLogErr->Log("%d{����δʶ��˼�}",pEdgeRod->widRod);
	//	continue;
	//}
#endif
		//CXhSimpleList<STDTEMPL::EDGEROD> listEdgeRods;
		//RetrieveLiveEdgeRods(pMatchStart,pMatchEnd,ocs,&listEdgeRods);
		//STDTEMPL::MATCHROD* pMatchRod=pLiveArm->hashMatchRods.Add(pEdgeRod->widRod);
		//if(pMatchRod!=NULL)
		//{
		//	pMatchRod->relaRods.Clear();
		//	for(STDTEMPL::EDGEROD* pEdgeRod=listEdgeRods.EnumObjectFirst();pEdgeRod;pEdgeRod=listEdgeRods.EnumObjectNext())
		//		pMatchRod->relaRods.Append(*pEdgeRod);
		//}
	if (pParaRod->pRelaRod!=NULL)
		hashMatchRodHid.SetValue(pParaRod->Id,pParaRod->pRelaRod->handle);
	return pParaRod->pRelaRod!=NULL;
}
int CStdComTempl::RecogMatchRods(UINT idRecogRod/*=0*/)
{
	int hits = 0;
	CStdTemplParaRod* pParaRod = idRecogRod == 0 ? NULL : listParaRods.GetValue(idRecogRod);
	if (pParaRod)
		return RecogMatchRod(pParaRod);
	else if (idRecogRod == 0)
	{
		for (pParaRod = listParaRods.GetFirst(); pParaRod; pParaRod = listParaRods.GetNext())
		{
			if (RecogMatchRod(pParaRod))
				hits++;
		}
	}
	return hits;
}
bool CStdComTempl::SetMappingNode(long idTemplNode,CLDSNode *pNode)
{
	CStdTemplParaNode* pParaNode=this->listParaNodes.GetValue(idTemplNode);
	if (pParaNode)
	{
		pParaNode->pRelaNode=pNode;
		pParaNode->xPosition=pNode->xOriginalPos;
	}
	if (pNode)
		hashMatchNodeHid.SetValue(idTemplNode,pNode->handle);
	return pParaNode!=NULL;
}
long CStdComTempl::SetMappingNode(long idTemplNode,long hLdsNode)
{
	hashMatchNodeHid.SetValue(idTemplNode,hLdsNode);
	return hLdsNode;
}
long CStdComTempl::MappingLdsNodeH(long idTemplNode)
{
	long* pHid=pHid=hashMatchNodeHid.GetValue(abs(idTemplNode));
	if (pHid==NULL)
		return NULL;
	//if(idTemplNode>0)
	return *pHid;
	//else
	//{
	//	//m_pBelongModel->
	//}
	return 0;
}
long CStdComTempl::ToLdsHandle(DWORD idObj)
{
	long* pHid;
	SEGI kid=idObj;
	if (kid.key.prefix[0]=='P')
	{
		if ((pHid=hashMatchNodeHid.GetValue(kid.key.number))!=NULL)
			return *pHid;
	}
	else if (kid.key.prefix[0]=='R')
	{
		if ((pHid=hashMatchRodHid.GetValue(kid.key.number))!=NULL)
			return *pHid;
	}
	else if (kid.key.prefix[0]=='-')
	{
		if ((pHid=hashMatchPlates.GetValue(kid.key.number))!=NULL)
			return *pHid;
	}
	return 0;
}
void CStdComTempl::Empty()
{
	hashMatchNodeHid.Empty();
	hashMatchRodHid.Empty();
	hashMatchPlates.Empty();
	//
	listParaNodes.Empty();
	listParaRods.Empty();
	listParaPlates.Empty();
	listParaBolts.Empty();
}
CStdComTempl::MATCH_CONNECT_POINT::MATCH_CONNECT_POINT()
{
	idNode=0;			//ƥ�����ӵ����ڵĻ�׼�ڵ�
	idStdConnPoint=0;	//ƥ��ı�׼���ӵ�ģʽ
	idStdNodePlate=0;	//���������ӵ㴦��Ҫʵ����ƵĽڵ���׼����ģʽ
	idBaseRod=0;		//���ӵ�Ļ�׼�˼�
	idRayRodOnAxisY=0;	//Y�������߸˼�
	cbMirCreateFlag=0;	//Ĭ���޶Գ�
	xarrMapConnRods.ZeroPresetMemory();	//���ӵ��漰�����߸˼���Ϊ��ʱ��ʾ�Զ���ȡ���ӵ����߸˼�
}
//////////////////////////////////////////////////////////////////////////
//CStdComTemplConnPoint
void InitSchemaRod(SCHEMA_ROD &rod,WORD idRod,short siMinDegAng,short siMaxDegAng,char ciNecessity,char ciAngleLayout,char ciRodType,bool dualSideEdge=false)
{
	rod.widRod=idRod;
	rod.siMinRayAngle=siMinDegAng;
	rod.siMaxRayAngle=siMaxDegAng;
	rod.ciNecessity=ciNecessity;
	rod.ciAngleWorkWing=ciAngleLayout;	//���Զ�����AngleLayout��AngleWorkWing֮���ת��
	rod.ciRodType=ciRodType;
	rod.szRodSectType[0]='L';
	rod.dualSideEdge=(ciRodType=='A'||dualSideEdge);
}
CStdComTemplConnPoint::CStdComTemplConnPoint()
{
	ciBaseRodConnState=0;
	widParentRod=widBaseRod=0;
	ciBaseRodSearchMode=0;
	blSetParentRodOfBaseNode=false;
	m_ciMirrorMatchMode=0;
	cnConnectPlane=0;
	idRayRodOnAxisY=0;
	m_bEnableIntelliConnPointDesign=false;
	AddConnPointAcsParam();//���ӵ�ģʽװ������ϵ����
}
void CStdComTemplConnPoint::AddConnPointAcsParam()
{
	STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"CrossRodAcs(1,1,2,P)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
	listParaNodes.Add(1)->m_bNecessaryNode=true;
	CStdTemplParaRod* pParaRod=listParaRods.Add(1);
	pParaRod->m_bNecessaryRod=true;
	pParaRod=listParaRods.Add(2);
	pParaRod->idStartNode=1;
	pParaRod->m_bNecessaryRod=true;
}
bool CStdComTemplConnPoint::GetSketchImage(IMAGE_INFO* pSketchImage)
{
	CXhChar50 szSketchImgSubFolder=_InternalGetSketchImgFileSubFolder();
	CXhChar50 szSketchBmpFile;
	//�ᵣ��
	if (szSketchImgFile.Length==0)
	{
		if (m_idStdComTmpl==STDTEMPL_CONNPOINT_HD1)
			szSketchBmpFile="Node\\HD1.bmp";
		else if (m_idStdComTmpl == STDTEMPL_CONNPOINT_HD1A)
			szSketchBmpFile = "Node\\HD1A.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_HD2)
			szSketchBmpFile="Node\\HD2.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_HD3)
			szSketchBmpFile="Node\\HD3.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_HD4)
			szSketchBmpFile="Node\\HD4.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_HD5)
			szSketchBmpFile="Node\\HD5.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_HD6)
			szSketchBmpFile="Node\\HD6.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_HD7)
			szSketchBmpFile="Node\\HD7.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_HD8)
			szSketchBmpFile="Node\\HD8.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_HD9)
			szSketchBmpFile="Node\\HD9.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_HD10)
			szSketchBmpFile="Node\\HD10.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_XH_HD3A)
			szSketchBmpFile="Node\\HD3A.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_XH_HD12||m_idStdComTmpl==STDTEMPL_CONNPOINT_XH_HD12_X)
			szSketchBmpFile="Node\\HD12.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_XH_HD13)
			szSketchBmpFile="Node\\HD13.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_XH_HD14)
			szSketchBmpFile="Node\\HD14.bmp";
		//���µ�
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_BP1)
			szSketchBmpFile="Node\\BP1.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_BP2)
			szSketchBmpFile="Node\\BP2.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_BP3)
			szSketchBmpFile="Node\\BP3.bmp";
		//�����
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_JC1||
			m_idStdComTmpl==STDTEMPL_CONNPOINT_JC1_K1||
			m_idStdComTmpl==STDTEMPL_CONNPOINT_JC1_K2)
			szSketchBmpFile="Node\\JC1.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_JC2)
			szSketchBmpFile="Node\\JC2.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_JC3)
			szSketchBmpFile="Node\\JC3.bmp";
		//����K��
		else if (m_idStdComTmpl == STDTEMPL_CONNPOINT_HK1)
			szSketchBmpFile = "Node\\HK-1.bmp";
		else if (m_idStdComTmpl == STDTEMPL_CONNPOINT_HK2)
			szSketchBmpFile = "Node\\HK-2.bmp";
		//����V���
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_TV)
			szSketchBmpFile="Node\\TV.bmp";
		//���߹ҵ�
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_XH_GX1)
			szSketchBmpFile="Node\\GX1.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_XH_GX2)
			szSketchBmpFile="Node\\GX2.bmp";
		else if (m_idStdComTmpl == STDTEMPL_CONNPOINT_XH_GX3)
			szSketchBmpFile = "Node\\GX3.bmp";
		else if (m_idStdComTmpl == STDTEMPL_CONNPOINT_XH_GX4)
			szSketchBmpFile = "Node\\GX4.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_XH_DX1)
			szSketchBmpFile="Node\\DX1.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_XH_DX2)
			szSketchBmpFile="Node\\DX2.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_XH_DX3)
			szSketchBmpFile="Node\\DX3.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_XH_DX3A)
			szSketchBmpFile="Node\\DX3A.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_XH_DM1)
			szSketchBmpFile="Node\\DM1.bmp";
		//�����ĵ�
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_FK1)
			szSketchBmpFile="Node\\FK1.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_FK2)
			szSketchBmpFile="Node\\FK2.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_FK3)
			szSketchBmpFile="Node\\FK3.bmp";
		else if (m_idStdComTmpl==STDTEMPL_CONNPOINT_FK4)
			szSketchBmpFile="Node\\FK4.bmp";
		else
			return false;
	}
	else
		szSketchBmpFile.Printf("%s%s",(char*)szSketchImgSubFolder,(char*)szSketchImgFile);
	if (pSketchImage)
	{
		pSketchImage->ciSrcType=1;
		StrCopy(pSketchImage->szImgFile,szSketchBmpFile,51);
	}
	return true;
}
void CStdComTemplConnPoint::InitComponentTempl(TEMPL_SOURCE* pTemplSource/*=NULL*/)
{
	TEMPL_SOURCE source;
	if (pTemplSource==NULL)
		pTemplSource=&source;
	if (pTemplSource!=NULL&&pTemplSource->ciSourceType!=0)
		return ;
	CStdTemplParaRod* pParaRod=NULL;
	CStdTemplParaAngle* pParaAngle=NULL;
	if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_HD1)
		InitConnPointTempl_HD1();
	else if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_HD1A)
		InitConnPointTempl_HD1A();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_HD2)
		InitConnPointTempl_HD2();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_HD3)
		InitConnPointTempl_HD3();
	else if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_HD3_G5)
		InitConnPointTempl_HD3_G5();
	if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_XH_HD3A)
		InitConnPointTempl_HD3A();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_HD4)
		InitConnPointTempl_HD4();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_HD5)
		InitConnPointTempl_HD5();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_HD6)
		InitConnPointTempl_HD6();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_HD7)
		InitConnPointTempl_HD7();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_HD8)
		InitConnPointTempl_HD8();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_HD9)
		InitConnPointTempl_HD9();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_HD10)
		InitConnPointTempl_HD10();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_XH_HD12)
		InitConnPointTempl_HD12();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_XH_HD12_X)
		InitConnPointTempl_HD12X();
	else if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_XH_HD13)
		InitConnPointTempl_HD13();
	else if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_XH_HD14)
		InitConnPointTempl_HD14();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_ZK14_K1)
		InitConnPointTempl_ZK14_K1();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_ZK14_K2)
		InitConnPointTempl_ZK14_K2();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_ZK14_K3)
		InitConnPointTempl_ZK14_K3();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_ZK14_K1_SIDE)
		InitConnPointTempl_ZK14_K1_SIDE();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_ZK14_K3_SIDE)
		InitConnPointTempl_ZK14_K3_SIDE();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_BP1||
		pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_BP2)
		InitConnPointTempl_BP1or2(pTemplSource->idStdComTempl);
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_BP3)
		InitConnPointTempl_BP3();
	else if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_JC1)
	{
		name = "JC1";
		szSketchImgFile = "JC1.bmp";
		//description = "�Ʊ�èͷ�������ڲཻ���";
	}
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_JC1_K1)
	{	//02050001
		name="JC1";//id 8
		szSketchImgFile = "JC1_K1.bmp";
		description="�Ʊ�èͷ�������ڲཻ���";
		widBaseRod=1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
		widParentRod=1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�

		//1.����-����ͨ��
		InitSchemaRod(rodarr[0],1,40,60,1,2,'A');
		//2.����б��
		InitSchemaRod(rodarr[1],2,120,150,1,2,'R');
		//3.����б��
		InitSchemaRod(rodarr[2],3,120,150,1,3,'R');
		rodarr.ReSize(3);
		CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_JC1_K1);
		pMatchPlate->InitDefaultMatchRods(3);
	}
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_JC1_K2)
	{	//02050001
		name="JC1";//id 8
		szSketchImgFile = "JC1_K2.bmp";
		description="�Ʊ�èͷ�������ڲཻ���";
		this->xView.ciViewPlaneNormal='Y';
		this->xView.siMinBaseRodAngle=25;
		this->xView.siMaxBaseRodAngle=35;
		widBaseRod=1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
		widParentRod=1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�

		//1.����-����ͨ��
		InitSchemaRod(rodarr[0],1,0,2,1,2,'R');
		//2.����б��
		InitSchemaRod(rodarr[1],2,110,130,1,2,'R');
		//3.����б��
		InitSchemaRod(rodarr[2],3,190,200,1,2,'R');
		//4.����б��
		InitSchemaRod(rodarr[3],4,280,290,1,3,'R');
		rodarr.ReSize(4);
		CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_JC1_K2);
		pMatchPlate->InitDefaultMatchRods(4);
	}
	else if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_JC2)
	{
		name = "JC2";
		szSketchImgFile = "JC2.bmp";
	}
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_JC3)
	{	//02050001
		name="JC3";//id 8
		szSketchImgFile="JC3.bmp";
		description="�����(��֧��)";
		widBaseRod=1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
		widParentRod=1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
		//1.����-����ͨ��
		InitSchemaRod(rodarr[0],1,40,60,1,2,'A');
		//2.����б��
		InitSchemaRod(rodarr[1],2,120,150,1,2,'A');
		//3.����б��
		InitSchemaRod(rodarr[2],3,89,91,1,3,'R');
		rodarr.ReSize(3);
		CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_JC3);
		pMatchPlate->InitDefaultMatchRods(3);
	}
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_SZK)
	{
		name="SZK";//id  11
		szSketchImgFile="SZK.bmp";
		description="���Ǹ�����������б��K�ڵ�����";
		widBaseRod=1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
		widParentRod=1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
		this->xView.ciViewPlaneNormal = 'H';
		xView.siMinBaseRodAngle = 80;
		xView.siMaxBaseRodAngle =100;
		rodarr.ZeroPresetMemory();
		this->m_bEnableIntelliConnPointDesign = true;

		//1.��������
		InitSchemaRod(rodarr[0],1,-1,1,1,3,'A',true);
		rodarr[0].xFlag.bFilterInsideAngle=true;
		//2.��б��(������
		InitSchemaRod(rodarr[1],2,20,70,1,'y','R');
		rodarr[1].xFlag.bFilterInsideAngle=true;
		rodarr[1].cnMinBoltN = 2;
		rodarr[1].xFlag.bFilterConnBoltN = true;
		//3.��б�ˣ�������
		InitSchemaRod(rodarr[2],3,110,160,1,'X','R');
		rodarr[2].xFlag.bFilterInsideAngle=true;
		rodarr[2].cnMinBoltN = 2;
		rodarr[2].xFlag.bFilterConnBoltN = true;
		rodarr.ReSize(3);
		CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_SZK1);
		pMatchPlate->InitDefaultMatchRods(3);
		pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_SZK2);
		pMatchPlate->InitDefaultMatchRods(3);
		//pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_SZK5);
		//pMatchPlate->InitDefaultMatchRods(3);
		pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_SZK6);
		pMatchPlate->InitDefaultMatchRods(3);
		/*
		pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_SZK1);
		pMatchPlate->InitDefaultMatchRods(3);
		pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_ZK1_NW4);
		pMatchPlate->InitDefaultMatchRods(3);
		pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_ZK1_N1);
		pMatchPlate->InitDefaultMatchRods(3);
		pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_ZK1_N2);
		pMatchPlate->InitDefaultMatchRods(3);
		pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_ZK1_N3);
		pMatchPlate->InitDefaultMatchRods(3);
		pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_ZK2_T);
		pMatchPlate->InitDefaultMatchRods(3);
		pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_ZK2_S1);
		pMatchPlate->InitDefaultMatchRods(3);
		pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_ZK2_S2);
		pMatchPlate->InitDefaultMatchRods(3);
		pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_ZK2_S3);
		pMatchPlate->InitDefaultMatchRods(3);
		pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_ZK3_S);
		pMatchPlate->InitDefaultMatchRods(3);
		pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_ZK3_T);
		pMatchPlate->InitDefaultMatchRods(3);
		pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_ZK4);
		pMatchPlate->InitDefaultMatchRods(3);*/
	}
	else if (pTemplSource->idStdComTempl== STDTEMPL_CONNPOINT_HK1)
	{
		name="HK-1";//id  11
		szSketchImgFile="HK-1.bmp";
		description="����ˮƽ����м�K�ڵ�֧������";
		widBaseRod=1;	//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
		widParentRod=1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
		this->xView.ciViewPlaneNormal = 'H';
		xView.siMinBaseRodAngle = -1;
		xView.siMaxBaseRodAngle =  1;
		rodarr.ZeroPresetMemory();
		this->m_bEnableIntelliConnPointDesign = true;

		//1.ˮƽ����
		InitSchemaRod(rodarr[0],1,0,5,1,3,'A');
		//2.��֧��б��
		InitSchemaRod(rodarr[1],2,20,70,1,3,'R');
		rodarr[1].cnMinBoltN = 2;
		rodarr[1].xFlag.bFilterConnBoltN = true;
		//3.��֧��б��
		InitSchemaRod(rodarr[2],3,110,160,1,2,'R');
		rodarr[2].cnMinBoltN = 2;
		rodarr[2].xFlag.bFilterConnBoltN = true;
		rodarr.SetSize(3);
		CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HK1);
		pMatchPlate->InitDefaultMatchRods(3);
	}
	else if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_HK2)
	{
		name = "HK-2";//id  11
		szSketchImgFile = "HK-2.bmp";
		description = "����ˮƽ����м����ֽڵ�֧������";
		widBaseRod = 1;	//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
		widParentRod = 1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
		this->xView.ciViewPlaneNormal = 'H';
		xView.siMinBaseRodAngle = -1;
		xView.siMaxBaseRodAngle = 1;
		rodarr.ZeroPresetMemory();
		this->m_bEnableIntelliConnPointDesign = true;

		//1.ˮƽ����
		InitSchemaRod(rodarr[0],1,0,5,1,3,'A');
		//2.����֧��б��
		InitSchemaRod(rodarr[1],2,20,70,1,3,'R');
		//3.����֧��б��
		InitSchemaRod(rodarr[2],3,110,160,1,2,'R');
		//4.����֧��б��
		InitSchemaRod(rodarr[3],4,290,340,1,2,'R');
		//5.����֧��б��
		InitSchemaRod(rodarr[4],5,200,250,1,3,'R');
		rodarr.SetSize(5);
		CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HK2);
		pMatchPlate->InitDefaultMatchRods(5);
	}
	else if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_TV)
	{
		name = "TV";
		szSketchImgFile = "TV.bmp";
	}
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_XH_GX1)
		InitConnPointTempl_GX1();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_XH_GX2)
		InitConnPointTempl_GX2();
	else if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_XH_GX3)
		InitConnPointTempl_GX3();
	else if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_XH_GX4)
		InitConnPointTempl_GX4();
	else if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_XH_GX5)
		InitConnPointTempl_GX5();
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_XH_DM1)
		InitConnPointTempl_DM1();
	else if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_XH_DM2)
		InitConnPointTempl_DM2();
	else if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_XH_DM3)
		InitConnPointTempl_DM3();
	else if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_XH_DX1)
	{	//02060003
		name = "XH_DX1";//ID 14
		szSketchImgFile = "DX1.bmp";
		description = "����֧�ܶ˽ڵ�";
		widBaseRod = 1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
		widParentRod = 1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
		this->ciBaseRodConnState = 2;		//2.�νӶ�ĩ������
		this->xView.ciBaseAxisXMode = 1;	//X��Ϊ��׼�����߷���
		this->xView.ciViewPlaneNormal = 'Y';
		xView.siMinBaseRodAngle = 195;
		xView.siMaxBaseRodAngle = 240;
		this->m_bEnableIntelliConnPointDesign = true;
		rodarr.ZeroPresetMemory();

		//1.���֧�Ÿ�
		InitSchemaRod(rodarr[0], 1, 0, 5, 1, 2, 'R', true);
		//2.��ֱ֧�Ÿ�
		InitSchemaRod(rodarr[1], 2, 25, 75, 1, 3, 'R');
		//3.�Ҳ�֧�Ÿ�
		InitSchemaRod(rodarr[2], 3, 75, 130, 1, 4, 'R');
		rodarr.ReSize(3);

		CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_DX1);
		pMatchPlate->InitDefaultMatchRods(3);
		//pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_DX1A);
		//pMatchPlate->InitDefaultMatchRods(3);
	}
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_XH_DX2)
	{
		name="XH_DX2";//ID 14
		szSketchImgFile = "DX2.bmp";
		description="����֧�ܶ˽ڵ�";
		widBaseRod=1;	//����֧�ܵ����Ǹ�
		widParentRod=1;	//ͬ��
		this->ciBaseRodConnState=1;		//2.�νӶ�ĩ������
		//this->xView.ciBaseAxisXMode=0;//0.X��Ĭ��Ϊ��׼��ʼ->�շ���
		this->xView.ciViewPlaneNormal='Y';
		xView.siMinBaseRodAngle=-5;
		xView.siMaxBaseRodAngle= 5;
		this->m_bEnableIntelliConnPointDesign=true;

		rodarr.ZeroPresetMemory();
		//1.ˮƽ������׼�Ǹ�
		InitSchemaRod(rodarr[0],1,-1,1,1,2,'A',true);
		//2.�ϲ�����
		InitSchemaRod(rodarr[1],2,85,110,1,1,'R');
		//3.�²�֧�Ÿ�
		InitSchemaRod(rodarr[2],3,200,270,1,3,'R');
		rodarr.ReSize(3);

		CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_DX2);
		pMatchPlate->InitDefaultMatchRods(3);
	}
	else if (pTemplSource->idStdComTempl==STDTEMPL_CONNPOINT_XH_DX3)
		InitConnPointTempl_DX3();
	else if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_XH_DX3A)
		InitConnPointTempl_DX3A();
	if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_FK1)
	{
		name = "FK1";
		description = "";
		szSketchImgFile = "FK1.bmp";
	}
	else if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_FK2)
	{
		name = "XH_HD13";
		description = "FK2";
		szSketchImgFile = "FK2.bmp";
	}
	else if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_FK3)
	{
		name = "FK3";
		description = "";
		szSketchImgFile = "FK3.bmp";
	}
	else if (pTemplSource->idStdComTempl == STDTEMPL_CONNPOINT_FK4)
	{
		name = "FK4";
		description = "";
		szSketchImgFile = "FK4.bmp";
	}
}
bool CStdComTemplConnPoint::ClearTmplParams()
{
	CStdTemplParaRod* pTmplRod;
	for (pTmplRod=listParaRods.GetFirst();pTmplRod;pTmplRod=listParaRods.GetNext())
		pTmplRod->pRelaRod=NULL;
	m_ciMirrorMatchMode=0;
	hashMatchRodHid.Empty();
	hashMatchNodeHid.Empty();
	for (UINT i=0;i<this->rodarr.Count;i++)
		rodarr[i].hMapRod=0;
	return true;
}
bool CStdComTemplConnPoint::IntelliRecogRayRods()
{
	SmartRodPtr pRod;
	long hBaseNode =this->MappingLdsNodeH(1);
	if (m_pBelongModel==NULL)
	{
		m_pBelongModel=&Ta;
		logerr.Log("error");
	}
	CLDSNode* pBaseNode=m_pBelongModel->FromNodeHandle(hBaseNode);
	if (pBaseNode==NULL)
		return false;
	GECS acs;
	if (!CStdComModel::BuildAcsOfAssmComponent(this,&acs))
		return false;
	CXhSimpleList<MATCHROD_INFO> connrods;
	for (pRod=m_pBelongModel->EnumRodFirst();pRod.IsHasPtr();pRod=m_pBelongModel->EnumRodNext())
	{
		if (!pBaseNode->ModuleTogetherWith(pRod))
			continue;
		if (pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		if (pRod->pStart!=pBaseNode&&pRod->pEnd!=pBaseNode&&pBaseNode->hFatherPart!=pRod->handle)
			continue;
		GEPOINT vLenStdVec=pRod->pEnd->xOriginalPos-pRod->pStart->xOriginalPos;
		vLenStdVec=acs.TransVToCS(vLenStdVec);
		normalize(vLenStdVec);
		if (fabs(vLenStdVec.z)>0.1)
			continue;	//���ڵ�ǰ�������ڵĸ˼�
		MATCHROD_INFO rodinfo(pRod);
		rodinfo.wiRayDegAngle=ftoi(Cal2dLineAng(0,0,vLenStdVec.x,vLenStdVec.y)*DEGTORAD_COEF);
		if (pRod->pStart==pBaseNode)
			rodinfo.ciConnTipS0E1Els2=0;
		else if (pRod->pEnd==pBaseNode)
		{
			rodinfo.wiRayDegAngle=(rodinfo.wiRayDegAngle+180)%360;
			rodinfo.ciConnTipS0E1Els2=1;
		}
		else //pRod->pStart!=pBaseNode&&pRod->pEnd!=pBaseNode
		{	//�ᴩ�˼�
			if (rodinfo.wiRayDegAngle>=180)
				rodinfo.wiRayDegAngle-=180;
			rodinfo.ciConnTipS0E1Els2=2;
		}
		connrods.Append(rodinfo);
	}
	UINT i;
	bool meetconditions=true;
	MATCHROD_INFO* pMatchRod;
	for (i=0;i<rodarr.Count;i++)
	{
		SCHEMA_ROD* pSchemaRod=rodarr.GetAt(i);
		for (pMatchRod=connrods.EnumObjectFirst();pMatchRod;pMatchRod=connrods.EnumObjectNext())
		{
			if (pMatchRod->matched)
				continue;	//��ƥ���
			pRod=pMatchRod->pRod;
			DEG xDegAngle(pMatchRod->wiRayDegAngle);
			if (!xDegAngle.IsInSector(pSchemaRod->siMinRayAngle,pSchemaRod->siMaxRayAngle))
				continue;	//���߼нǲ�ƥ��
			if ((pSchemaRod->ciRodType=='A'&&pMatchRod->ciConnTipS0E1Els2<2)||
				(pSchemaRod->ciRodType!='A'&&pMatchRod->ciConnTipS0E1Els2>=2))
				continue;	//���������Ͳ�һ��
			//pSchemaRod->szRodSectType	�˼����������ж�
			pMatchRod->matched=true;
			if (pSchemaRod->hMapRod>0&&pSchemaRod->hMapRod!=pMatchRod->pRod->handle)
				logerr.Log("error");
			pSchemaRod->hMapRod=pMatchRod->pRod->handle;
			this->hashMatchRodHid.SetValue(pSchemaRod->widRod,pRod->handle);
			break;
		}
		if (pSchemaRod->ciNecessity==1&&pSchemaRod->hMapRod==0)
			meetconditions=false;	//ȱ�ٱ�Ҫ�ĸ˼�
	}
	//for(pMatchRod=connrods.EnumObjectFirst();pMatchRod;pMatchRod=connrods.EnumObjectNext())
	//{
	//	if(!pMatchRod->matched)
	//		meetconditions=false;	//����˼�
	//}
	return meetconditions;
}
bool CStdComTemplConnPoint::UpdateMatchStdPlateTmpl(CStdComTempl* pStdPlateTmpl,BYTE ciMirMode/*=0*/)
{
	CMatchStdPlate* pMatchPlate=hashMaybeTmplPlates.GetValue(pStdPlateTmpl->Id);
	if (pMatchPlate==NULL)
		return false;
	if (m_pBelongModel==NULL)
		m_pBelongModel=&Ta;
	int mirmode=0;
	pStdPlateTmpl->ClearTmplParams();
	//���½ڵ�幹��ģ��Ļ�׼�ڵ���Ϣ
	long  hBaseNode =this->MappingLdsNodeH(1);	//1
	CLDSNode* pBaseNode=m_pBelongModel->FromNodeHandle(hBaseNode);
	if (pBaseNode==NULL)
	{
		logerr.Log("error,0x%X�ڵ�ʾ�ҵ�",hBaseNode);
		return false;
	}
	if (ciMirMode>=MIR_CREATE_X&&ciMirMode<=MIR_CREATE_Z)
	{
		if (ciMirMode==MIR_CREATE_X)
			mirmode=1;
		else
			mirmode=ciMirMode==MIR_CREATE_Y?2:3;
		if ((pBaseNode=pBaseNode->GetMirNode(MIRMSG(mirmode)))==NULL)
		{
			logerr.Log("error");
			return false;
		}
	}
	pStdPlateTmpl->SetMappingNode(1,pBaseNode);
	//���½ڵ�幹��ģ��Ķ�λ��׼�˼���Ϣ
	long* phBaseRod =this->hashMatchRodHid.GetValue(this->widBaseRod);
	if (phBaseRod==NULL)
		return false;
	UINT i;
	CLDSLinePart* pBaseRod=m_pBelongModel->FromRodHandle(*phBaseRod);
	if (mirmode>0)
	{
		if ((pBaseRod=pBaseRod->GetMirRod(MIRMSG(mirmode)))==NULL)
		{
			logerr.Log("error");
			return false;
		}
	}
	pStdPlateTmpl->hashMatchRodHid.SetValue(1,pBaseRod->handle);
	for (i=0;i<pMatchPlate->xarrMapConnRods.Count;i++)
	{
		CMPSTID xid=pMatchPlate->xarrMapConnRods.At(i);
		if (xid==0)
			continue;
		long *phRayRod=this->hashMatchRodHid.GetValue(xid.sidGroupRod);
		long hRayRod=phRayRod!=NULL?*phRayRod:rodarr.At(i).hMapRod;

		SmartRodPtr pRayRod=hRayRod==NULL?NULL:m_pBelongModel->FromRodHandle(hRayRod);
		if (mirmode>0&&pRayRod.blValid)
		{
			pRayRod=pRayRod->GetMirRod(MIRMSG(mirmode));
			if (!pRayRod.blValid&&rodarr[i].blNecessary)
			{
				logerr.Log("error");
				return false;
			}
			hRayRod=pRayRod->handle;
		}
		pStdPlateTmpl->hashMatchRodHid.SetValue(xid.sidGroupRod,pRayRod.IsNULL()?0:hRayRod);
	}
	GECS acs;
	CStdComModel model(this->m_pBelongModel);
	model.VerifyMatchRods(pStdPlateTmpl);
	for (i=0;i<pMatchPlate->xarrMapConnRods.Count;i++)
	{
		CMPSTID xid=pMatchPlate->xarrMapConnRods.At(i);
		if (xid==0)
			continue;
		long *phRayRod=this->hashMatchRodHid.GetValue(xid.sidGroupRod);
		long hRayRod=phRayRod!=NULL?*phRayRod:rodarr.At(i).hMapRod;
		SmartRodPtr pRayRod=hRayRod==NULL?NULL:m_pBelongModel->FromRodHandle(hRayRod);
		if (mirmode>0&&pRayRod.blValid)
		{
			pRayRod=pRayRod->GetMirRod(MIRMSG(mirmode));
			if (!pRayRod.blValid)
			{
				logerr.Log("error");
				return false;
			}
			hRayRod=pRayRod->handle;
		}
		if (pRayRod.IsNULL())
			pStdPlateTmpl->hashMatchRodHid.SetValue(xid,0);
		else if (pRayRod->GetClassTypeId()==CLS_GROUPLINEANGLE&&xid.sidSerial>0)
		{
			//CStdTemplParaAngle* pParaAngle=(CStdTemplParaAngle*)pStdPlateTmpl->listParaRods.GetValue(xid.sidGroupRod);
			CStdTemplParaRod* pParaAngle=pStdPlateTmpl->listParaRods.GetValue(xid.sidGroupRod);
			pStdPlateTmpl->hashMatchRodHid.SetValue(xid.sidGroupRod,pRayRod->handle);	//������ϽǸ�
			if (pParaAngle->bReverseMasterAngle&&xid.sidSerial==1)
				pStdPlateTmpl->hashMatchRodHid.SetValue(CMPSTID(xid.sidGroupRod,1),pRayRod.pGroupAngle->son_jg_h[1]);
			else if (pParaAngle->bReverseMasterAngle&&xid.sidSerial==2)
				pStdPlateTmpl->hashMatchRodHid.SetValue(CMPSTID(xid.sidGroupRod,2),pRayRod.pGroupAngle->son_jg_h[0]);
			else
			{
				int j=0,n=pRayRod.pGroupAngle->size_cSubClassType=='X'?4:2;
				for (int j=0;j<n;j++)
					pStdPlateTmpl->hashMatchRodHid.SetValue(CMPSTID(xid.sidGroupRod,j+1),pRayRod.pGroupAngle->son_jg_h[j]);
			}
		}
		else
			pStdPlateTmpl->hashMatchRodHid.SetValue(xid,hRayRod);
	}
	pStdPlateTmpl->m_ciMirrorMatchMode=m_ciMirrorMatchMode;
	return true;
}