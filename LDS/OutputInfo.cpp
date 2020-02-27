//<LOCALE_TRANSLATE Confirm by hxr/>
// ExtendItemInfo.cpp: implementation of the COutputInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "lds.h"
#include "OutputInfo.h"
#include "env_def.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "CheckPartNoDlg.h"
#include "GlobalFunc.h"
#include "ArrayList.h"
#include "EmendBoltGasket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
COutputInfo* COutputInfoAgent::Append(long hRelaObj1,long hRelaObj2,char* format,...)
{
	if(ListAppendFunc==NULL)
		return NULL;
	COutputInfo* pInfo=(*ListAppendFunc)(m_pContext);
	pInfo->m_uType=m_uType;
	pInfo->m_hRelaObj1=hRelaObj1;
	pInfo->m_hRelaObj2=hRelaObj2;
	va_list marker;
	va_start(marker, format);
	char lineText[201];
	memset(lineText,0,201);
	_vsnprintf(lineText,200,format,marker);
	int str_len=strlen(lineText);
	pInfo->m_sDescription.Format("%s",lineText);
	return pInfo;
}
static CTaskOutputList _localListTemporyWarningOutput;
bool ReflectErrorMsgTo(int iWarningType,long hRelaObj[2],const char* errmsg)
{
	if(errmsg==NULL)	//Ϊ�ձ�ʾ��յ�ǰ��ʱ������Ϣ����
		_localListTemporyWarningOutput.Empty();
	else
	{
		COutputInfo* pOutput=_localListTemporyWarningOutput.AppendSimpleOutput();
		pOutput->m_uType=iWarningType;
		pOutput->m_hRelaObj1=hRelaObj[0];
		pOutput->m_hRelaObj2=hRelaObj[1];
		pOutput->m_sDescription=errmsg;
	}
	return true;
}
void StartTaskOutputPage()
{
	CMainFrame* pMainWnd=((CMainFrame*)theApp.m_pMainWnd);
	COutputInfoDlg *pTaskListDlg = pMainWnd->GetOutputInfoPage();
	if(!g_sysPara.dock.pageOutput.bDisplay)
		pMainWnd->ShowDockpageOutput();
	pTaskListDlg->InitOutputInfoList(_localListTemporyWarningOutput);
	char sTitle[50]="";
#ifdef AFX_TARG_ENU_ENGLISH
	if(_localListTemporyWarningOutput.GetNodeNum()>0)
		sprintf(sTitle,"%d task",_localListTemporyWarningOutput.GetNodeNum());
	else 
		sprintf(sTitle,"0 task");
#else 
	if(_localListTemporyWarningOutput.GetNodeNum()>0)
		sprintf(sTitle,"%d������",_localListTemporyWarningOutput.GetNodeNum());
	else 
		sprintf(sTitle,"0������");
#endif
	pTaskListDlg->SetInfoTitle(sTitle);
	pMainWnd->ActivateDockpage(RUNTIME_CLASS(COutputInfoDlg));
}

CTaskLogErrorLife::CTaskLogErrorLife(CLogFile* pLogFile/*=NULL*/)
	: CLogErrorLife(pLogFile,ReflectErrorMsgTo,StartTaskOutputPage){;}

COutputInfo::COutputInfo()
{
	m_idOutputType=1;
	m_uType=(OUTPUT_INFO_TYPE)0;//����
#ifdef __PART_DESIGN_INC_
	m_ciElemDiffType=0;
	memset(compare.strbuf,0,16);
#endif
	m_hRelaObj1=0;				//��������id1
	m_hRelaObj2=0;				//��������id2
	m_sDescription="";			//������Ϣ
	state=false;
	tolerance=0.5;
}

COutputInfo::~COutputInfo()
{

}
bool COutputInfo::CopyFrom(IBasicOutput* pOutput)
{
	if(GetTypeId()!=pOutput->GetTypeId())
		return false;
	COutputInfo* pCopySrc=(COutputInfo*)pOutput;
	m_hRelaObj1=pCopySrc->m_hRelaObj1;		//��������id1
	m_hRelaObj2=pCopySrc->m_hRelaObj2;		//��������id2
	m_uType=pCopySrc->m_uType;
	state  = pCopySrc->state;
	tolerance=pCopySrc->tolerance;		//����У��ʱ��������Χ,Ĭ��0.5mm
	m_sDescription	=pCopySrc->m_sDescription;	//������Ϣ
	collisionPt		=pCopySrc->collisionPt;	//ʵ����ײ��
#ifdef __PART_DESIGN_INC_
	m_ciElemDiffType=pCopySrc->m_ciElemDiffType;
	compare=pCopySrc->compare;
#endif
	return true;
}
SCOPE_STRU GetPartScope(CLDSPart* pPart,SCOPE_STRU *pScope=NULL)
{
	SCOPE_STRU scope;
	if(pScope==NULL)
		pScope=&scope;
	if(pPart->IsLinePart())
	{
		CLDSLinePart* pRod=(CLDSLinePart*)pPart;
		if(pRod->pStart&&pRod->pEnd)
		{
			pScope->VerifyVertex(pRod->pStart->Position(false));
			pScope->VerifyVertex(pRod->pEnd->Position(false));
		}
		else
		{
			pScope->VerifyVertex(pRod->Start());
			pScope->VerifyVertex(pRod->End());
		}
	}
	else if(pPart->pSolidBody&&pPart->pSolidBody->KeyPointNum()>0)
	{
		for(int i=0;i<pPart->pSolidBody->KeyPointNum();i++)
		{
			f3dPoint vertex=pPart->pSolidBody->GetKeyPointAt(i);
			pScope->VerifyVertex(vertex);
		}
	}
	else
		pScope->VerifyVertex(pPart->ucs.origin);
	if(pScope)
		return *pScope;
	else
		return scope;
}
//ѡ�й�������
void COutputInfo::SelectRelationObj()				
{
	g_pSolidDraw->ReleaseSnapStatus();
	BOOL obj1_selected=false,obj2_selected=false;
	if(m_hRelaObj1>0x20)
		obj1_selected=g_pSolidDraw->SetEntSnapStatus(m_hRelaObj1);
	if(m_hRelaObj2>0x20)
		obj2_selected=g_pSolidDraw->SetEntSnapStatus(m_hRelaObj2);
	SCOPE_STRU scope;
	CLDSDbObject *pRelaObj1=NULL;
	CLDSPart *pPart=NULL;
	CLDSModule* pModule=Ta.GetActiveModule();
#ifdef __PART_DESIGN_INC_
	if(m_uType==INFO_COMPARE_REPORT_ELEMS)
	{
		CLDSNode* pStart=Ta.Node.FromHandle(m_hRelaObj1);
		CLDSNode* pEnd=Ta.Node.FromHandle(m_hRelaObj2);
		if(pStart)
			scope.VerifyVertex(pStart->Position(false));
		if(pEnd)
			scope.VerifyVertex(pEnd->Position(false));
		if((pRelaObj1=pStart)==NULL)
			pRelaObj1=pEnd;
		CElemInfo* pElemInfo=Ta.Elems.GetValue(m_hRelaObj1,m_hRelaObj2);
		if(pElemInfo)
		{
			CLDSLinePart* pRod=Ta.FromRodHandle(pElemInfo->m_hParentRod);
			if(pRod&&!pModule->IsSonPart(pRod))
			{
				for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
				{
					if(!pRod->IsLegObj()&&pModule->IsSonPart(pRod))
					{
						Ta.m_hActiveModule=pModule->handle;
						break;
					}
					else if(pRod->cfgword.IsHasBodyNo(pModule->GetBodyNo()))
					{
						Ta.m_hActiveModule=pModule->handle;
						if(!pModule->IsSonPart(pRod))
						{
							char iQuad=pRod->Layer(2)-'1';
							if(iQuad<0||iQuad>3)
								iQuad=0;
							for(int iLeg=1;iLeg<=192;iLeg++)
							{
								if(pRod->cfgword.IsHasNo(iLeg))
								{
									pModule->m_arrActiveQuadLegNo[iQuad]=iLeg;
									break;
								}
							}
						}
						break;
					}
					g_pSolidDraw->BuildDisplayList(NULL);	
				}
			}
			g_pSolidDraw->SetEntSnapStatus(pElemInfo->Id|0x80000000);
		}
	}
	else
#endif
	{
	//if(m_uType!=INFO_EMEND_CROSSRODINFO)
		pPart = Ta.Parts.FromHandle(m_hRelaObj1);	//ѡ�е�һ������ʱ������ת����
	//else
		CLDSPart* pPart2 = Ta.Parts.FromHandle(m_hRelaObj2);	//ѡ�е�һ������ʱ������ת����
	if((pRelaObj1=pPart)==NULL)
		pRelaObj1=pPart2;
	if(pPart||pPart2)
	{
		if(pPart)
			scope=GetPartScope(pPart);
		if(pPart2)
		{
			SCOPE_STRU scope2=GetPartScope(pPart2);
			double ff1=scope.wide()+scope.high()+scope.depth();
			double ff2=scope2.wide()+scope2.high()+scope2.depth();
			if(ff1<ff2)
				scope=scope2;
		}
	}
	else
	{
		CLDSNode *pNode=Ta.Node.FromHandle(m_hRelaObj1);
		if(pNode==NULL)
			return;
		scope.VerifyVertex(pNode->Position(false));
		scope.fMinX-=500;
		scope.fMaxX+=500;
		scope.fMinY-=500;
		scope.fMaxY+=500;
		scope.fMinZ-=500;
		scope.fMaxZ+=500;
	}
	}
	g_pSolidOper->FocusTo(scope,g_sysPara.m_fMaxScaleScr2User);
	f3dPoint center;
	g_pSolidSet->SetOperType(OPER_ROTATE);
	g_pSolidSet->SetZoomStyle(ROT_CENTER);
	scope.centre(&center.x,&center.y,&center.z);
	if(IsCollisionOutputType())
		center=collisionPt;
	g_pSolidSet->SetRotOrg(center);
	g_pSolidDraw->Draw();
	//����������
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	if(obj1_selected||obj2_selected)
		pView->UpdatePropertyPage();
	else if(pRelaObj1)
	{
		pView->UpdatePropertyPage(pRelaObj1);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The object of specified handle is hidden currently!");
#else
		AfxMessageBox("ָ������Ķ���ǰ���ڷ���ʾ״̬��");
#endif
	}
}
bool COutputInfo::IsCollisionOutputType()
{
	if( INFO_CHK_COLLIDE_RODS==m_uType||INFO_CHK_COLLIDE_RODBOLT==m_uType||
		INFO_CHK_COLLIDE_PLATEBOLT==m_uType||INFO_CHK_COLLIDE_BOLTS==m_uType||
		INFO_CHK_COLLIDE_RODPLATE==m_uType||INFO_CHK_COLLIDE_PLATES==m_uType||
		INFO_ONEKEY_CUTANGLE_CHECK==m_uType)
		return true;
	else
		return false;
}
const char* COutputInfo::GetTypeName()
{
	return GetTypeName(m_uType,m_idOutputType);	
}
const char* COutputInfo::GetTypeName(int uType,int m_idOutputType/*=1*/)
{
	const char* sTypeName="";
	if(m_idOutputType==3)
	{	//���(Ȧ)�����޶��������
		if(uType==ERROR_ISOLATED_BOLT)
			return "���������˨";	//�޹��������Ĺ�����˨, ����ʽΪ:���Ի�ȷ�����
		else if(uType==ERROR_VIRTUAL_HOLE_LIKELY||uType==ONE_PART_BOLT)
			return "���������ӵļ���˨";//Ŀǰ��һ�����������ķǽŶ���������ף�����ʽΪ��ȷ����Ϊ������˨��
		else if(uType==ERROR_VIRTUAL_WIREHOLE_LIKELY)
			return "���߿׼���˨";//Ŀǰ��һ�����������ķǽŶ���������ף�����ʽΪ��ȷ����Ϊ������˨��
		else if(uType==ERROR_MISSING_CROSSGASKET)
			return "����㴦���ȱʧ";	//����㴦���ȱʧ������ʽΪ����ӵ����Ȧ
		else if(uType==ERROR_MISSING_ENDGASKET)
			return "��ͷ���(Ȧ)ȱʧ";
		else if(uType==ERROR_GASKET_THICK)
			return "���(Ȧ)��ȴ���";	//�����Ȧ��ȴ��󣬴���ʽΪ�������޶������Ȧ���
		else if(uType==ERROR_PART_POS)
			return "��ͷ����λ�ô���";	//����λ�ô���
		else if(uType==ERROR_BOLTPAD_CUTANGLE)
			return "��ͷ������н�";
		else if(uType==ERROR_GASKET_PROFILE)
			return "�����нǵ��ͳһ������";
		else if(uType==ERROR_REDUNDANT_GASKET)
			return "������";
		else
			return "δ֪�����������";
	}
	//1.��������
	if(uType==INFO_EMEND_SINGULAR_BOLTLO)		//��˨ͨ���޶�
#ifdef AFX_TARG_ENU_ENGLISH
		sTypeName="bolts LO modification";
	else if(uType==INFO_EMEND_CROSSRODINFO)		//update Cross-bracing Members computation information
		sTypeName="update Cross-bracing Members computation information";
	else if(uType==INFO_EMEND_MIRRODINFO)		//modify relative symmetry information
		sTypeName="modify relative symmetry information";
	else if(uType==INFO_EMEND_BOLTGASKET)		//add modify bolt's washers
		sTypeName="add modify bolt's washers";
	else if(uType==INFO_EMEND_LAYER)			//modify object's layer name
		sTypeName="modify object's layer name";
	else if(uType==INFO_EMEND_TUBEENDJOINTLJ)	//check modifying tube end connecting information
		sTypeName="check modifying tube end connecting information";
#else
		sTypeName="��˨ͨ���޶�";
	else if(uType==INFO_EMEND_CROSSRODINFO)		//���½���ļ�����Ϣ
		sTypeName="���½���ļ�����Ϣ";
	else if(uType==INFO_EMEND_MIRRODINFO)		//���������Գ���Ϣ
		sTypeName="���������Գ���Ϣ";
	else if(uType==INFO_EMEND_BOLTGASKET)		//����������˨��Ƭ���Ȧ
		sTypeName="����������˨���(Ȧ)";
	else if(uType==INFO_EMEND_LAYER)			//��������ͼ������
		sTypeName="��������ͼ������";
	else if(uType==INFO_EMEND_TUBEENDJOINTLJ)	//У�������ֹܶ�ͷ������Ϣ
		sTypeName="�����ֹܶ�ͷ������Ϣ";
#endif
	//2.����У��
	//���ǰ���	
	else if(uType==INFO_CHK_MIRROD)				//�ԱȶԳƸ˼��Ĺ����ʣ��Ա���һ�����й������
#ifdef AFX_TARG_ENU_ENGLISH
		sTypeName="symmetric rods information";
	else if(uType==INFO_CHK_MIRROD_LENGTH)		//datum length of symmetric rods
		sTypeName="datum length of symmetric rods";
	else if(uType==INFO_CHK_MIRPLATE)			//symmetric plates check
		sTypeName="symmetric plates information check";
	else if(uType==INFO_CHK_BOLTOVERLAP)		//overlapped bolts check
		sTypeName="overlapped bolts check";
	else if(uType==INFO_CHK_HOLEMISSING)		//hole missing check
		sTypeName="hole missing check";
	else if(uType==INFO_CHK_REDUNDANTBOLT)		//redundant bolts check
		sTypeName="redundant bolts check";
	//bolts legality check
	else if(uType==INFO_CHK_BOLT_ROLLINGSPACE)	//angle rolling margin
		sTypeName="angle rolling margin";
	else if(uType==INFO_CHK_BOLT_CUTANGLESPACE)	//angle cutting margin
		sTypeName="angle cutting margin";
	else if(uType==INFO_CHK_BOLT_ANGLE_ENDSPACE)//end distance of angle bolts
		sTypeName="end distance of angle bolts";
	else if(uType==INFO_CHK_BOLT_ANGLE_FORGEERR)//angle flat leg conflicts with bolt's hole positon
		sTypeName="angle flat leg conflicts with bolt's hole positon";
	else if(uType==INFO_CHK_BOLT_PLATE_ENDSPACE)//connecting bolts' margin
		sTypeName="connecting bolts' margin";
	else if(uType==INFO_CHK_BOLT_HUOQULINE_SPACE)//connecting bolts' margin
		sTypeName="connecting bolts' margin";
	else if(uType==INFO_CHK_BOLT_L0)			//bolts LO range
		sTypeName="bolts LO range";
	else if(uType==INFO_CHK_BOLT_SPEC)			//bolts specification
		sTypeName="bolts specification";
	else if(uType==INFO_CHK_BOLT_GRADE)			//bolt level does not agree with specification library
		sTypeName="bolt level does not agree with specification library";
	//data completeness check,data completeness test(including parts which can not be shown by wrong solid data, plate parts more often)
	else if(uType==INFO_ROD_LOST_STARTNODE)		//nodes of rod's starting end missing
		sTypeName="nodes of rod's starting end missing";
	else if(uType==INFO_ROD_LOST_ENDNODE)		//nodes of rod's ending end missing
		sTypeName="nodes on rod's ending end missing";
	else if(uType==INFO_NODE_LOST_FATHERROD)	//nodes of parent rod missing
		sTypeName="nodes of parent rod missing";
	else if(uType==INFO_NODE_LOST_ATTACHNODE)	//depend node
		sTypeName="node lack of depend node";
	else if(uType==INFO_NODE_LOST_ATTACHROD)	//depend rod
		sTypeName="node lack of depend rod";
	else if(uType==INFO_NODE_LOST_ATTACHPLANE)	//depend plane
		sTypeName="node lack of depend plane";
	else if(uType==INFO_NODE_LOST_ATTACHPOINT)	//depend datum point
		sTypeName="node lack of depend datum point";
	else if(uType==INFO_PART_LOST_MATERIAL)		//part lack of steel material information
		sTypeName="part lack of steel material information";
	else if(uType==INFO_PART_INVALID_MATERIAL)	//part's steel materil information incorrect
		sTypeName="part's steel material information incorrect";
	else if(uType==INFO_ANGLE_LOST_LSLIST)		//angle lack of bolts chain table
		sTypeName="angle lack of bolts chain table";
	else if(uType==INFO_ANGLESTART_LOST_BENDLINE)	//angle starting lack of bending datum angle
		sTypeName="angle starting lack of bending datum angle";
	else if(uType==INFO_ANGLESTART_LOST_PRJLINE)	//angle starting lack of Collinear position datum angle
		sTypeName="angle starting lack of Collinear position datum angle";
	else if(uType==INFO_ANGLEEND_LOST_BENDLINE)	//angle ending lack of bending datum angle
		sTypeName="angle ending lack of bending datum angle";
	else if(uType==INFO_ANGLEEND_LOST_PRJLINE)	//angle ending lack of Collinear position datum angle
		sTypeName="angle ending lack of Collinear position datum angle";
	else if(uType==INFO_ANGLEWINGX_LOST_REF_ROD)//lack of referrence rod during calculating angle leg X normal
		sTypeName="angle leg X normal";
	else if(uType==INFO_ANGLEWINGY_LOST_REF_ROD)//lack of referrence rod during calculating angle leg Y normal
		sTypeName="angle leg Y normal";
	else if(uType==INFO_PLATE_LOST_DATUM_REF_NODE)		//handle of datum node is empty
		sTypeName="handle of datum node is empty";
	else if(uType==INFO_PLATE_LOST_DATUM_POS_DES_PARA)	//datum point parameters
		sTypeName="datum point parameters";
	else if(uType==INFO_PLATE_LOST_NORM_DES_PARA)	//datum plane normal parameters
		sTypeName="datum plane normal parameters";
	else if(uType==INFO_PLATE_LOST_LINK_REF_ROD)	//connecting rods
		sTypeName="plate connecting rod's information";
	else if(uType==INFO_BOLT_LOST_DATUM_REF_NODE)	//bolts datum node lost
		sTypeName="bolts datum node lost";
	else if(uType==INFO_BOLT_LOST_DATUM_REF_PART)	//bolts datum part lost
		sTypeName="bolts datum part lost";
	else if(uType==INFO_BOLT_LOST_NORM_DES_PARA)	//bolts normal design parameter lost 
		sTypeName="bolts normal design parameter";
	//merge overlap nodes and rods
	else if(uType==INFO_EMEND_OVERLAPNODE)			//merge bolts normal design parameters(modification)
		sTypeName="bolts normal design parameter";
	else if(uType==INFO_EMEND_OVERLAPROD)			//merge overlap rods(modification)
		sTypeName="pverlap rods";
	//check legality of part solid data 
	else if(uType==INFO_BOLT_NORM_ZERO)			//bolt's normal is empty
		sTypeName="bolt's normal is empty";
	else if(uType==INFO_PART_LOCATION_ABNORMAL)			//bolt's normal is empty
		sTypeName="Part's location is abnormal!";
	else if(uType==INFO_INVALID_PART_UCS)		//coordinate system is illegal
		sTypeName="coordinate system is illegal";
	else if(uType==INFO_INVALID_PROFILE)		//illegal appearance, the number of solid vertex is too small, all the length,width and thickness are zero
		sTypeName="invalid appearance";
	//
	else if(uType==INFO_CHK_MODELINTEGRITY)		//check the match relationship between modle inner rod's nodes
		sTypeName="check the match relationship between module inner rod's nodes";
	else if(uType==INFO_CHK_BOLTCONNECT)		//connecting tense of rod's bolts 
		sTypeName="connecting tense of rod's bolts ";
	else if(uType==INFO_CHK_MODELSTABILITY)		//System stability check 
		sTypeName="System stability check";
	//3.check Number
	else if(uType==INFO_CHK_NULLPARTNO)			//No-Number parts
		sTypeName="No-Number parts";
	else if(uType==INFO_CHK_PARTTYPE_DIFFER)	//part type are different
		sTypeName="part type are different";
	else if(uType==INFO_CHK_RODINFO)			//basic information of rods of the same No. 
		sTypeName="basic information of rods of the same No. ";
	else if(uType==INFO_CHK_RODBOLTINFO)		//quantity, specifications and location information of the bolts with the same Number
		sTypeName="rod's bolts information";
	else if(uType==INFO_CHK_ANGLECUT)			//angle cut information(including reverse contrast)
		sTypeName="angle cut and leg cut information of angle";
	else if(uType==INFO_CHK_PLATEINFO)			//thickness, steel material, bending planes, rolling edges of the plates with the same Number
		sTypeName="basic information of plate";
	else if(uType==INFO_CHK_PLATEPROFILEANDBOLT)//plate's bolt information, appearance and corresponding hole position, reverse exchangability
		sTypeName="plate bolts and appearance information";
	//4.collision check
	else if(uType==INFO_CHK_COLLIDE_RODS)		//collision between rod and rod(region can be specified)
		sTypeName="collision between rod and rod";
	else if(uType==INFO_CHK_COLLIDE_RODBOLT)	//collision between rod and bolt(region can be specified)
		sTypeName="collision between rod and bolt";
	else if(uType==INFO_CHK_COLLIDE_PLATEBOLT)	//collision between plate and bolt(region can be specified)
		sTypeName="collision between plate and bolt";
	else if(uType==INFO_CHK_COLLIDE_BOLTS)		//collision between bolt and bolt(region can be specified)
		sTypeName="collision between bolt and bolt";
	else if(uType==INFO_CHK_COLLIDE_RODPLATE)	//collision between rod and connecting plate(region can be specified)
		sTypeName="collision between rod and connecting plate";
	else if(uType==INFO_CHK_COLLIDE_PLATES)		//collision between connecting plate and connecting plate(region can be specified)
		sTypeName="collision between connecting plate and connecting plate";
	//5.����ͳ��
	else if(uType==INFO_STAT_BOLTRELAPARTS)		//inquire parts relative to bolts
		sTypeName="inquire parts relative to bolts";
	else if(uType==INFO_STAT_RODLENGTH)			//rod statistics in the length region
		sTypeName="rod statistics in the length region";
	else if(uType==INFO_STAT_RODSIZE)			//specified specification rods statistics
		sTypeName="specified specification rods statistics";
	else if(uType==INFO_ONEKEY_CUTANGLE_CHECK)
		sTypeName="one key cut angle check";
	else if(uType==INFO_COMPARE_REPORT_WARNNING||uType==INFO_COMPARE_REPORT_NODES||
		uType==INFO_COMPARE_REPORT_ELEMS||uType==INFO_COMPARE_REPORT_RODS)
		sTypeName="computation report comparison";
#else
		sTypeName="�ԳƸ˼���Ϣ���";
	else if(uType==INFO_CHK_MIRROD_LENGTH)		//�ԳƸ˼���׼����
		sTypeName="�ԳƸ˼���׼����";
	else if(uType==INFO_CHK_MIRPLATE)			//�ԳƸְ���
		sTypeName="�ԳƸְ���Ϣ���";
	else if(uType==INFO_CHK_BOLTOVERLAP)		//�ص���˨���
		sTypeName="�ص���˨���";
	else if(uType==INFO_CHK_HOLEMISSING)		//©�׼��
		sTypeName="©�׼��";
	else if(uType==INFO_CHK_REDUNDANTBOLT)		//������˨���
		sTypeName="������˨���";
	//��˨�Ϸ���У��
	else if(uType==INFO_CHK_BOLT_ROLLINGSPACE)	//�Ǹ����Ʊ߾�
		sTypeName="�Ǹ����Ʊ߾�";
	else if(uType==INFO_CHK_BOLT_CUTANGLESPACE)	//�Ǹ��нǱ߾�
		sTypeName="�Ǹ��нǱ߾�";
	else if(uType==INFO_CHK_BOLT_ANGLE_ENDSPACE)//�Ǹ���˨�˾�
		sTypeName="�Ǹ���˨�˾�";
	else if(uType==INFO_CHK_BOLT_ANGLE_FORGEERR)//�Ǹ�ѹ������˨��λ��ͻ
		sTypeName="�Ǹ�ѹ��֫����˨��ͻ";
	else if(uType==INFO_CHK_BOLT_PLATE_ENDSPACE)//������˨�߾�
		sTypeName="������˨�߾�";
	else if(uType==INFO_CHK_BOLT_HUOQULINE_SPACE)//������˨�߾�
		sTypeName="������˨�߾�(��������)";
	else if(uType==INFO_CHK_BOLT_L0)			//��˨ͨ��Χ
		sTypeName="��˨ͨ��Χ";
	else if(uType==INFO_CHK_BOLT_SPEC)			//��˨���
		sTypeName="��˨���";
	else if(uType==INFO_CHK_BOLT_GRADE)			//��˨����������Ƿ����
		sTypeName="��˨��������ⲻ��";
	//����������У�ˣ�����������У��(������ʵ�������޷���ʾ�Ĺ���, ��Ϊ�幹��)
	else if(uType==INFO_ROD_LOST_STARTNODE)		//�˼�ʼ�˽ڵ㶪ʧ
		sTypeName="�˼�ʼ�˽ڵ㶪ʧ";
	else if(uType==INFO_ROD_LOST_ENDNODE)		//�˼��ն˽ڵ㶪ʧ
		sTypeName="�˼��ն˽ڵ㶪ʧ";
	else if(uType==INFO_NODE_LOST_FATHERROD)	//�ڵ㸸�˼���ʧ
		sTypeName="�ڵ㸸�˼���ʧ";
	else if(uType==INFO_NODE_LOST_ATTACHNODE)	//�����ڵ�
		sTypeName="�ڵ�ȱ�������ڵ�";
	else if(uType==INFO_NODE_LOST_ATTACHROD)	//�����˼�
		sTypeName="�ڵ�ȱ�������˼�";
	else if(uType==INFO_NODE_LOST_ATTACHPLANE)	//����ƽ��
		sTypeName="�ڵ�ȱ������ƽ��";
	else if(uType==INFO_NODE_LOST_ATTACHPOINT)	//������׼��
		sTypeName="�ڵ�ȱ��������׼��";
	else if(uType==INFO_PART_LOST_MATERIAL)		//����ȱ�ٲ�����Ϣ
		sTypeName="����ȱ�ٲ�����Ϣ";
	else if(uType==INFO_PART_INVALID_MATERIAL)	//����������Ϣ����ȷ
		sTypeName="����������Ϣ����ȷ";
	else if(uType==INFO_ANGLE_LOST_LSLIST)		//�Ǹ�ȱ����˨����
		sTypeName="�Ǹ�ȱ����˨����";
	else if(uType==INFO_ANGLESTART_LOST_BENDLINE)	//�Ǹ�ʼ��ȱ�ٻ�����׼�Ǹ�
		sTypeName="�Ǹ�ʼ��ȱ�ٻ�����׼�Ǹ�";
	else if(uType==INFO_ANGLESTART_LOST_PRJLINE)	//�Ǹ�ʼ��ȱ�ٹ���λ�û�׼�Ǹ�
		sTypeName="�Ǹ�ʼ��ȱ�ٹ���λ�û�׼�Ǹ�";
	else if(uType==INFO_ANGLEEND_LOST_BENDLINE)	//�Ǹ��ն�ȱ�ٻ�����׼�Ǹ�
		sTypeName="�Ǹ��ն�ȱ�ٻ�����׼�Ǹ�";
	else if(uType==INFO_ANGLEEND_LOST_PRJLINE)	//�Ǹ��ն�ȱ�ٹ���λ�û�׼�Ǹ�
		sTypeName="�Ǹ��ն�ȱ�ٹ���λ�û�׼�Ǹ�";
	else if(uType==INFO_ANGLEWINGX_LOST_REF_ROD)//�Ǹ�X֫���߼��������ȱ�ٲ��ո˼�
		sTypeName="�Ǹ�X֫����";
	else if(uType==INFO_ANGLEWINGY_LOST_REF_ROD)//�Ǹ�Y֫���߼��������ȱ�ٲ��ո˼�
		sTypeName="�Ǹ�Y֫����";
	else if(uType==INFO_PLATE_LOST_DATUM_REF_NODE)		//��׼�ڵ���Ϊ��
		sTypeName="��׼�ڵ���Ϊ��";
	else if(uType==INFO_PLATE_LOST_DATUM_POS_DES_PARA)	//��׼�����
		sTypeName="��׼�����";
	else if(uType==INFO_PLATE_LOST_NORM_DES_PARA)	//��׼�淨�߲���
		sTypeName="��׼�淨�߲���";
	else if(uType==INFO_PLATE_LOST_LINK_REF_ROD)	//���Ӹ˼�
		sTypeName="�ְ����Ӹ˼���Ϣ";
	else if(uType==INFO_BOLT_LOST_DATUM_REF_NODE)	//��˨��ʧ��׼�ڵ�
		sTypeName="��˨��ʧ��׼�ڵ�";
	else if(uType==INFO_BOLT_LOST_DATUM_REF_PART)	//��˨��ʧ��׼����
		sTypeName="��˨��ʧ��׼����";
	else if(uType==INFO_BOLT_LOST_NORM_DES_PARA)	//��˨��ʧ������Ʋ���
		sTypeName="��˨��ʧ������Ʋ���";
	//�ϲ��ظ��ڵ㼰�ظ��˼�
	else if(uType==INFO_EMEND_OVERLAPNODE)			//�ϲ��ظ��ڵ�(����)
		sTypeName="�ظ��ڵ�";
	else if(uType==INFO_EMEND_OVERLAPROD)			//�ϲ��ظ��˼�(����)
		sTypeName="�ظ��˼�";
	//����ʵ�����ݺϷ��Լ��
	else if(uType==INFO_BOLT_NORM_ZERO)			//��˨����Ϊ��
		sTypeName="��˨����Ϊ��";
	else if(uType==INFO_PART_LOCATION_ABNORMAL)	//��˨����Ϊ��
		sTypeName="����λ���쳣";
	else if(uType==INFO_INVALID_PART_UCS)		//����ϵ���Ϸ�
		sTypeName="����ϵ���Ϸ�";
	else if(uType==INFO_INVALID_PROFILE)		//�Ƿ�����,ʵ�嶥�������١����Ȼ��Ȼ���Ϊ0
		sTypeName="�Ƿ�����";
	//
	else if(uType==INFO_CHK_MODELINTEGRITY)		//ģ���ڸ˼��ڵ��ƥ���ϵ���
		sTypeName="ģ���ڸ˼��ڵ��ƥ���ϵ���";
	else if(uType==INFO_CHK_BOLTCONNECT)		//�˼�����˨����ǿ��
		sTypeName="�˼�����˨����ǿ��";
	else if(uType==INFO_CHK_MODELSTABILITY)		//��ϵ�ȶ��Լ��
		sTypeName="��ϵ�ȶ��Լ��";
	//3.���У��
	else if(uType==INFO_CHK_NULLPARTNO)			//�ձ�Ź���
		sTypeName="�ձ�Ź���";
	else if(uType==INFO_CHK_PARTTYPE_DIFFER)	//�������Ͳ�ͬ
		sTypeName="�������Ͳ�ͬ";
	else if(uType==INFO_CHK_RODINFO)			//ͬ��Ÿ˼�������Ϣ
		sTypeName="�˼�������Ϣ";
	else if(uType==INFO_CHK_RODBOLTINFO)		//ͬ��Ÿ˼�����˨���������λ����Ϣ
		sTypeName="�˼���˨��Ϣ";
	else if(uType==INFO_CHK_ANGLECUT)			//�Ǹ����н���Ϣ(���ߵ��Ա�)
		sTypeName="�Ǹ�ѹ���н���֫��Ϣ";
	else if(uType==INFO_CHK_PLATEINFO)			//ͬ��Ÿְ��ȡ����ʡ��������������
		sTypeName="�ְ������Ϣ";
	else if(uType==INFO_CHK_PLATEPROFILEANDBOLT)//�ְ���˨��Ϣ�����μ���Կ�λ���ߵ������ԡ�
		sTypeName="�ְ���˨��������Ϣ";
	//4.��ײУ��
	else if(uType==INFO_CHK_COLLIDE_RODS)		//�˼���˼���ײ(��ָ������)
		sTypeName="�˼���˼���ײ";
	else if(uType==INFO_CHK_COLLIDE_RODBOLT)	//�˼�����˨��ײ(��ָ������)
		sTypeName="�˼�����˨��ײ";
	else if(uType==INFO_CHK_COLLIDE_PLATEBOLT)	//�ְ�����˨��ײ(��ָ������)
		sTypeName="�ְ�����˨��ײ";
	else if(uType==INFO_CHK_COLLIDE_BOLTS)		//��˨����˨��ײ(��ָ������)
		sTypeName="��˨����˨��ײ";
	else if(uType==INFO_CHK_COLLIDE_RODPLATE)	//�˼���������ײ(��ָ������)
		sTypeName="�˼���������ײ";
	else if(uType==INFO_CHK_COLLIDE_PLATES)		//������������ײ(��ָ������)
		sTypeName="������������ײ";
	//5.����ͳ��
	else if(uType==INFO_STAT_BOLTRELAPARTS)		//��ѯ��˨��������
		sTypeName="��ѯ��˨��������";
	else if(uType==INFO_STAT_RODLENGTH)			//���������ڵĸ˼�ͳ��
		sTypeName="���������ڵĸ˼�ͳ��";
	else if(uType==INFO_STAT_RODSIZE)			//ָ�����ĸ˼�ͳ��
		sTypeName="ָ�����ĸ˼�ͳ��";
	else if(uType==INFO_ONEKEY_CUTANGLE_CHECK)
		sTypeName="һ���нǼ��";
	else if(uType==INFO_COMPARE_REPORT_WARNNING||uType==INFO_COMPARE_REPORT_NODES||
		uType==INFO_COMPARE_REPORT_ELEMS||uType==INFO_COMPARE_REPORT_RODS)
		sTypeName="���㱨��ȶ�";
#endif
	return sTypeName;
}
CString COutputInfo::GetKeyStr()
{
	CString keyStr;
	keyStr.Format("%d+%ld+%ld",m_uType,m_hRelaObj1,m_hRelaObj2);
	return keyStr;
}
bool ReCalBoltGuigeAndGrade(COutputInfo *pOutputInfo,bool bGrade)
{
	CLDSBolt* pBolt=(CLDSBolt*)Ta.Parts.FromHandle(pOutputInfo->m_hRelaObj1,CLS_BOLT);
	return pBolt->CalGuigeAuto(bGrade);
}
bool COutputInfo::Recheck()
{
	char sError[100]="";
	CLDSPart *pPart=Ta.Parts.FromHandle(m_hRelaObj1);
	CLDSPart *pOtherPart=Ta.Parts.FromHandle(m_hRelaObj2);
	if(m_uType==INFO_CHK_MIRROD || m_uType==INFO_CHK_MIRROD_LENGTH)			//�ԱȶԳƸ˼��Ĺ����ʼ���׼���ȣ��Ա���һ�����й������
	{
		if(pPart&&pOtherPart&&pPart->IsLinePart()&&pOtherPart->IsLinePart())
		{
			DWORD dwStyleFlag=ChkStyleFlag();
			if(!((CLDSLinePart*)pPart)->IsEqualWith((CLDSLinePart*)pOtherPart,0.5,sError,NULL,&dwStyleFlag))
				m_sDescription.Format("%s",sError);
			else
				state=true;
		}
	}
#ifdef __COMMON_PART_INC_
	else if(m_uType==INFO_CHK_MIRPLATE)		//�ԳƸְ���
	{
		if(pPart&&pOtherPart&&pPart->GetClassTypeId()==CLS_PLATE&&pOtherPart->GetClassTypeId()==CLS_PLATE)
		{
			if(!((CLDSPlate*)pPart)->IsEqual((CLDSPlate*)pOtherPart,0.5,sError))
#ifdef AFX_TARG_ENU_ENGLISH
				m_sDescription.Format("0x%X and 0x%X%s",pPart->handle,pOtherPart->handle,sError);
#else
				m_sDescription.Format("0x%X��0x%X%s",pPart->handle,pOtherPart->handle,sError);
#endif
			else
				state=true;
		}
	}
	else if(m_uType==INFO_CHK_BOLTOVERLAP)	//�ص���˨���
	{
		if(pPart&&pOtherPart&&pPart->GetClassTypeId()==CLS_BOLT&&pOtherPart->GetClassTypeId()==CLS_BOLT)
		{
			CLDSBolt *pBolt=(CLDSBolt*)pPart;
			if(DISTANCE(pPart->ucs.origin,pOtherPart->ucs.origin)<pBolt->get_d())
#ifdef AFX_TARG_ENU_ENGLISH
				m_sDescription.Format("0x%X and 0x%X bolts postion is too near��conflict occur��",pPart->handle,pOtherPart->handle);
#else
				m_sDescription.Format("0x%X��0x%X��˨λ�ù�����������ͻ��",pPart->handle,pOtherPart->handle);
#endif
			else 
				state=true;
		}
		else 
		{
			state=true;
			return false;
		}
	}
	else if(m_uType==INFO_CHK_HOLEMISSING)	//©�׼��
	{
		if(pPart&&pPart->GetClassTypeId()==CLS_BOLT)
		{
			CLDSBolt *pBolt=(CLDSBolt*)pPart;
			pBolt->feature=0;
			for(CLDSPart *pTempPart=Ta.Parts.GetFirst();pTempPart;pTempPart=Ta.Parts.GetNext())
			{
				CLsRefList *pLsRefList=pTempPart->GetLsRefList();
				if(pLsRefList==NULL)
					continue;
				if(pLsRefList->FromHandle(pBolt->handle))
					pBolt->feature++;
			}
			if(pBolt->feature==1&&!pBolt->IsFootNail())
#ifdef AFX_TARG_ENU_ENGLISH
				m_sDescription.Format("0x%X bolt connects only one part ��maybe lack of bolt hole��",pBolt->handle);
#else
				m_sDescription.Format("0x%X��˨ֻ������һ������������ȱ����˨�ף�",pBolt->handle);
#endif
			else
				state=true;
		}
		else if(pPart==NULL)
			return false;
	}
	else if(m_uType==INFO_CHK_REDUNDANTBOLT)	//������˨���
	{
		if(pPart&&pPart->GetClassTypeId()==CLS_BOLT)
		{
			CLDSBolt *pBolt=(CLDSBolt*)pPart;
			pBolt->feature=0;
			for(CLDSPart *pTempPart=Ta.Parts.GetFirst();pTempPart;pTempPart=Ta.Parts.GetNext())
			{
				CLsRefList *pLsRefList=pTempPart->GetLsRefList();
				if(pLsRefList==NULL)
					continue;
				if(pLsRefList->FromHandle(pBolt->handle))
					pBolt->feature++;
			}
			if(pBolt->feature==0)
#ifdef AFX_TARG_ENU_ENGLISH
				m_sDescription.Format("0x%X bolt does not connect any part, maybe redundant bolt��",pBolt->handle);
#else
				m_sDescription.Format("0x%X��˨δ�����κι���������Ϊ������˨��",pBolt->handle);
#endif
			else
				state=true;
		}
		else if(pPart==NULL)
			return false;	
	}
#endif
#ifdef __COMMON_PART_INC_
	else if(m_uType==INFO_CHK_NULLPARTNO)		//�ձ�Ź���
	{
		if(pPart&&strlen(pPart->GetPartNo())<=0)
#ifdef AFX_TARG_ENU_ENGLISH
			m_sDescription.Format("0x%X%s part's No. not input correctly, please check!",pPart->handle,pPart->GetPartTypeName());
#else
			m_sDescription.Format("0x%X%s�Ĺ�����δ��ȷ����,���ʵ!",pPart->handle,pPart->GetPartTypeName());
#endif
		else 
			state=true;
	}	
	else if(m_uType==INFO_CHK_BOLT_SPEC||m_uType==INFO_CHK_BOLT_GRADE)		//�ձ�Ź���
		state=ReCalBoltGuigeAndGrade(this,m_uType==INFO_CHK_BOLT_GRADE);
	else if(m_uType==INFO_CHK_RODBOLTINFO||m_uType==INFO_CHK_PLATEPROFILEANDBOLT)
	{
		CLDSPartPtr pPartArr[2];
		pPartArr[0]=Ta.Parts.FromHandle(m_hRelaObj1);
		pPartArr[1]=Ta.Parts.FromHandle(m_hRelaObj2);
		DWORD styleFlag=CHKPARTNO_RODBOLTINFO;
		if(m_uType==INFO_CHK_PLATEPROFILEANDBOLT)
			styleFlag=CHK_PLATE_PROFILEANDBOLTINFO;
		if(pPartArr[0]&&pPartArr[1])
			state=pPartArr[0]->IsEqual(pPartArr[1],TRUE,tolerance,NULL,NULL,&styleFlag);
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("This check type not support now!");
#else
		logerr.Log("�ݲ�֧�ִ����͵�����У��");
#endif
		return false;
	}
#endif
	return state;
}
int COutputInfo::CorrectErrorCmdInfo(AUTOCORRECT_CMDINFO &cmdInfo)
{
	cmdInfo.safeType=-1;
	if(state)	//�ѽ��������Ҫ������
		return -1;
	/*if(m_uType==INFO_CHK_ANGLECUT)
	{
		cmdInfo.safeType=2;
		cmdInfo.nativeName.Copy("ͳһ�н���Ϣ");
	}
	else */if(m_uType==INFO_EMEND_OVERLAPNODE)
	{
		cmdInfo.safeType=0;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdInfo.nativeName.Copy("merge overlapped nodes");
#else
		cmdInfo.nativeName.Copy("�ϲ��ص��ڵ�");
#endif
	}
	else if(m_uType==INFO_EMEND_OVERLAPROD)
	{
		cmdInfo.safeType=0;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdInfo.nativeName.Copy("merge overlapped rods");
#else
		cmdInfo.nativeName.Copy("�ϲ��ص��˼�");
#endif
	}
	else if(m_uType==INFO_CHK_REDUNDANTBOLT)
	{
		cmdInfo.safeType=1;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdInfo.nativeName.Copy("clear redundant bolts");
#else
		cmdInfo.nativeName.Copy("���������˨");
#endif
	}
	else if(m_uType==INFO_CHK_BOLTOVERLAP)
	{
		cmdInfo.safeType=0;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdInfo.nativeName.Copy("merge overlapped bolts");
#else
		cmdInfo.nativeName.Copy("�ϲ��ص���˨");
#endif
	}
	else if(m_uType==INFO_BOLT_NORM_ZERO||m_uType==INFO_PART_LOCATION_ABNORMAL)
	{
		cmdInfo.safeType=0;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdInfo.nativeName.Copy("clear wrong parts");
#else
		cmdInfo.nativeName.Copy("������󹹼�");
#endif
	}
	else if(m_uType==INFO_INVALID_PART_UCS)
	{
		cmdInfo.safeType=0;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdInfo.nativeName.Copy("clear wrong parts");
#else
		cmdInfo.nativeName.Copy("������󹹼�");
#endif
	}
	else if(m_uType==INFO_INVALID_PROFILE)
	{
		cmdInfo.safeType=0;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdInfo.nativeName.Copy("clear wrong parts");
#else
		cmdInfo.nativeName.Copy("������󹹼�");
#endif
	}
	else if(m_uType==INFO_CHK_RODBOLTINFO||m_uType==INFO_CHK_PLATEPROFILEANDBOLT||m_uType==INFO_CHK_MIRPLATE)
	{
		cmdInfo.safeType=1;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdInfo.nativeName.Copy("design again");
#else
		cmdInfo.nativeName.Copy("�������");
#endif
	}
	else if(m_uType==INFO_CHK_BOLT_SPEC||m_uType==INFO_CHK_BOLT_GRADE)
	{
		cmdInfo.safeType=1;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdInfo.nativeName.Copy("modify");
#else
		cmdInfo.nativeName.Copy("����");
#endif
	}
	return cmdInfo.safeType;
}
//ͳһ�Ǹ��н���Ϣ
bool UnifyAngleCutInfo(COutputInfo *pOutputInfo)
{
	return false;
}
//�ϲ��ص��ڵ�
//pNode:�ϲ�����Ҫ�����Ľڵ�;pOthNode:�ϲ�����Ҫɾ���Ľڵ�
bool MergeOverlapNodes(CLDSNode *pReserveNode,CLDSNode *pMergeNode,COutputInfo *pOutputInfo,bool updateRelaRodSolid)
{
	CLDSNode *pNode=pReserveNode, *pOthNode=pMergeNode;	//���д��봿��Ϊ���������Ķ������ wjh-2017.12.16
	if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pOthNode->dwPermission))
	{
		if(pOutputInfo)
#ifdef AFX_TARG_ENU_ENGLISH
			pOutputInfo->m_sDescription.Format("When network coordinates��no permission to modify node 0X%X authority��merge forbidden",pOthNode->handle);
		else
			logerr.Log("When network coordinates,no permission to modify node 0X%X authority,merge forbidden",pOthNode->handle);
#else
			pOutputInfo->m_sDescription.Format("����Эͬʱ����0X%X�ڵ�û��ӵ���޸�Ȩ�ޣ��޷��ϲ�",pOthNode->handle);
		else
			logerr.Log("����Эͬʱ����0X%X�ڵ�û��ӵ���޸�Ȩ�ޣ��޷��ϲ�",pOthNode->handle);
#endif
		return false;	//����Эͬʱ���Դ�
	}
	if(pNode==NULL||pOthNode==NULL)
		return false;	//��������һ���ڵ��ѱ�ɾ����

	if(!theApp.m_bCooperativeWork||pOthNode->dwPermission==pNode->dwPermission)
	{	//���Ժϲ�
		if(pOutputInfo)
#ifdef AFX_TARG_ENU_ENGLISH
			pOutputInfo->m_sDescription.Format("merge node 0X%X and node &0X%X-->node 0X%X\n",pNode->handle,pOthNode->handle,pNode->handle);
		else
			logerr.Log("merge node 0X%X and node &0X%X-->node 0X%X\n",pNode->handle,pOthNode->handle,pNode->handle);
#else
			pOutputInfo->m_sDescription.Format("�ϲ�0X%X�ڵ�&0X%X�ڵ�-->�ڵ�0X%X\n",pNode->handle,pOthNode->handle,pNode->handle);
		else
		{
			CXhChar16 nodestr1,nodestr2;
			nodestr1.Printf("0X%X",pNode->handle);
			if(pNode->pointI>0)
				nodestr1.Append(CXhChar16("{%d}",pNode->pointI));
			nodestr2.Printf("0X%X",pOthNode->handle);
			if(pOthNode->pointI>0)
				nodestr2.Append(CXhChar16("{%d}",pOthNode->pointI));
			logerr.Log("�ϲ�%s�ڵ�&%s�ڵ�-->�ڵ�%s\n",(char*)nodestr1,(char*)nodestr2,(char*)nodestr1);
		}
#endif
		if(pOthNode->hFatherPart!=0&&pNode->hFatherPart==0)
		{	//�����ڵ���ȱʧ���˼�ʱ��Ӧ���ô�ɾ���ڵ�ĸ��˼� wjh-2013.6.6
			pNode->hFatherPart=pOthNode->hFatherPart;
			pNode->SetModified();
		}
		else if(pOthNode->hFatherPart>0&&pNode->hFatherPart>0)
		{
			CLDSLinePart* pFatherRod=Ta.FromRodHandle(pNode->hFatherPart);
			if(pFatherRod==NULL)
			{
				pNode->hFatherPart=pOthNode->hFatherPart;
				pNode->SetModified();
			}
		}
		CLDSLinePart* pRod;
		LINEPARTSET relaRods;
		for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		{
			if(pRod->pStart==pOthNode||pRod->pEnd==pOthNode)
				relaRods.append(pRod);
		}
		Ta.ReplaceReferenceHandle(CHandleReplacer(pOthNode->handle,pNode->handle),CLS_NODE);
		if(updateRelaRodSolid)
		{
			CAngleMoveOperObject moveangle;
			for(pRod=relaRods.GetFirst();pRod;pRod=relaRods.GetNext())
			{
				pRod->ClearFlag();
				pRod->CalPosition();
				pRod->Create3dSolidModel();
				if(pRod->is_visible)
					g_pSolidDraw->NewSolidPart(pRod->GetSolidPartObject());
			}
		}
		pNode->cfgword.AddSpecWord(pOthNode->cfgword);	//��ֹ�ϲ��������ĺ���ĳһ����ȱʧ���
		console.DispNodeSet.DeleteNode(pOthNode->handle);
		console.DeleteNode(pOthNode->handle);
		g_pSolidDraw->DelEnt(pOthNode->handle);
		return true;
	}
	else //if(theApp.m_bCooperativeWork&&pOthNode->dwPermission!=pNode->dwPermission)
	{
		if(pOutputInfo)
#ifdef AFX_TARG_ENU_ENGLISH
			pOutputInfo->m_sDescription.Format("0X%X & 0X%X-->different authority��unable to merge\n",pNode->handle,pOthNode->handle);
		else
			logerr.Log("0X%X & 0X%X-->different authority��unable to merge\n",pNode->handle,pOthNode->handle);
#else
			pOutputInfo->m_sDescription.Format("0X%X & 0X%X-->Ȩ�޲�ͬ���޷��ϲ�\n",pNode->handle,pOthNode->handle);
		else
			logerr.Log("0X%X & 0X%X-->Ȩ�޲�ͬ���޷��ϲ�\n",pNode->handle,pOthNode->handle);
#endif
		return false;
	}
}
bool MergeOverlapNodes(CLDSNode *pReserveNode,CLDSNode *pMergeNode,COutputInfo *pOutputInfo=NULL)
{
	return MergeOverlapNodes(pReserveNode,pMergeNode,pOutputInfo,true);
}
bool MergeOverlapNodes(COutputInfo *pOutputInfo)
{
	CLDSNode *pNode=Ta.Node.FromHandle(pOutputInfo->m_hRelaObj1);
	CLDSNode *pMergeNode=Ta.Node.FromHandle(pOutputInfo->m_hRelaObj2);
	return MergeOverlapNodes(pNode,pMergeNode,pOutputInfo);
}
//�ϲ��ص��˼�
bool MergeOverlapRods(COutputInfo *pOutputInfo)
{
	CLDSLinePart *pPole=(CLDSLinePart*)Ta.Parts.FromHandle(pOutputInfo->m_hRelaObj1,CLS_LINEPART);
	CLDSLinePart *pOthPole=(CLDSLinePart*)Ta.Parts.FromHandle(pOutputInfo->m_hRelaObj2,CLS_LINEPART);
	if(pPole==NULL||pOthPole==NULL)
		return false;
	//��ǰ����Ӧ�ϲ�����׼����
#ifdef AFX_TARG_ENU_ENGLISH
	pOutputInfo->m_sDescription.Format("0X%X rod and 0X%X rod overlap,merge into 0X%X��\n",pPole->handle,pOthPole->handle,pPole->handle);
#else
	pOutputInfo->m_sDescription.Format("0X%X�˼���0X%X�˼����ص�,�ϲ�Ϊ0X%X��\n",pPole->handle,pOthPole->handle,pPole->handle);
#endif
	Ta.ReplaceReferenceHandle(CHandleReplacer(pOthPole->handle,pPole->handle));
	console.DispPartSet.DeleteNode(pOthPole->handle);
	pPole->cfgword.AddSpecWord(pOthPole->cfgword);
	console.DeletePart(pOthPole->handle);
	return true;
}
#ifdef __COMMON_PART_INC_
//�ϲ��ص���˨
bool MergeOverlapBolts(COutputInfo *pOutputInfo)
{
	if(pOutputInfo->SolvedState())
		return true;
	long hFromBolt=max(pOutputInfo->m_hRelaObj1,pOutputInfo->m_hRelaObj2);
	long hDestBolt=min(pOutputInfo->m_hRelaObj1,pOutputInfo->m_hRelaObj2);
	CLDSBolt* pFromBolt=(CLDSBolt*)Ta.Parts.FromHandle(hFromBolt,CLS_BOLT);
	CLDSBolt* pDestBolt=(CLDSBolt*)Ta.Parts.FromHandle(hDestBolt,CLS_BOLT);
	if(pFromBolt==NULL||pDestBolt==NULL)
		return false;
	bool failed=false;
	for(CSuperSmartPtr<CLDSPart>pPart=Ta.Parts.GetFirst();pPart.IsHasPtr();pPart=Ta.Parts.GetNext())
	{
		CLsRefList* pLsRefList=pPart->GetLsRefList();
		if(pLsRefList)
		{
			CLsRef *pLsRef=pLsRefList->FromHandle(hFromBolt);
			bool canMerge=true;
			if(theApp.m_bCooperativeWork&&pLsRef!=NULL)
			{
				if(pPart->IsLinePart())
				{
					if(pLsRef->start1_end2_mid0==0&&!theApp.IsHasModifyPerm(pPart.LinePartPointer()->dwPermission))
					{
#ifdef AFX_TARG_ENU_ENGLISH
						pOutputInfo->m_sDescription.Format("not own the authority to modify 0X%X rod's basic information,unable to merge bolts!",pPart->handle);
#else
						pOutputInfo->m_sDescription.Format("��ӵ�ж�0X%X�˼�������Ϣ���޸�Ȩ��,�޷��ϲ���˨!",pPart->handle);
#endif
						canMerge=false;
					}
					else if(pLsRef->start1_end2_mid0==1&&!theApp.IsHasModifyPerm(pPart.LinePartPointer()->dwStartPermission))
					{
#ifdef AFX_TARG_ENU_ENGLISH
						pOutputInfo->m_sDescription.Format("not own the authority to modify 0X%X rod's starting end,unable to merge bolts!",pPart->handle);
#else
						pOutputInfo->m_sDescription.Format("��ӵ�ж�0X%X�˼�ʼ�˵��޸�Ȩ��,�޷��ϲ���˨!",pPart->handle);
#endif
						canMerge=false;
					}
					else if(pLsRef->start1_end2_mid0==2&&!theApp.IsHasModifyPerm(pPart.LinePartPointer()->dwEndPermission))
					{
#ifdef AFX_TARG_ENU_ENGLISH
						pOutputInfo->m_sDescription.Format("not own the authority to modify 0X%X rod's ending,unable to merge bolts!",pPart->handle);
#else
						pOutputInfo->m_sDescription.Format("��ӵ�ж�0X%X�˼��ն˵��޸�Ȩ��,�޷��ϲ���˨!",pPart->handle);
#endif
						canMerge=false;
					}
				}
				else if(!theApp.IsHasModifyPerm(pPart->dwPermission))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					pOutputInfo->m_sDescription.Format("not own the authority to modify 0X%X parts,unable to merge bolts!",pPart->handle);
#else
					pOutputInfo->m_sDescription.Format("��ӵ�ж�0X%X�������޸�Ȩ��,�޷��ϲ���˨!",pPart->handle);
#endif
					canMerge=false;	//Эͬģʽ�²�ӵ�жԴ˹�����Ȩ��
				}
			}
			if(canMerge)
				pLsRefList->MergeBolts(pFromBolt,pDestBolt);
			else
				failed=true;
#ifdef AFX_TARG_ENU_ENGLISH
			pOutputInfo->m_sDescription.Format("bolt 0X%X & 0X%X -->bolt 0X%X!",hFromBolt,hDestBolt,hDestBolt);
#else
			pOutputInfo->m_sDescription.Format("��˨0X%X & 0X%X -->��˨0X%X!",hFromBolt,hDestBolt,hDestBolt);
#endif
		}
	}

	if(!failed)
	{
		console.DispPartSet.DeleteNode(pFromBolt->handle);
		g_pSolidDraw->DelEnt(pFromBolt->handle);
		console.DeletePart(pFromBolt->handle);
		return true;
	}
	else
		return false;
}
//���������˨
bool RemoveRedundantBolt(COutputInfo *pOutputInfo)
{
	console.DispPartSet.DeleteNode(pOutputInfo->m_hRelaObj1);
	console.DeletePart(pOutputInfo->m_hRelaObj1);
	return true;
}
//������󹹼�
bool RemoveInvalidPart(COutputInfo *pOutputInfo)
{
	console.DispPartSet.DeleteNode(pOutputInfo->m_hRelaObj1);
	console.DeletePart(pOutputInfo->m_hRelaObj1);
	return true;
}
//�������,Ȼ���ټ��
bool RedesignPartAndCheck(COutputInfo *pOutputInfo)
{
	CSuperSmartPtr<CLDSPart> pPartArr[2];
	pPartArr[0]=Ta.Parts.FromHandle(pOutputInfo->m_hRelaObj1);
	pPartArr[1]=Ta.Parts.FromHandle(pOutputInfo->m_hRelaObj2);
	for(int i=0;i<2;i++)
	{
		CSuperSmartPtr<CLDSPart> pPart1=pPartArr[i];
		if(pPart1.IsHasPtr()&&pPart1->GetClassTypeId()==CLS_PLATE)
		{
			pPart1.PlatePointer()->DesignPlate();
			for(CLsRef *pLsRef=pPart1.PlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pPart1.PlatePointer()->GetNextLsRef())
			{
				pLsRef->GetLsPtr()->correct_worknorm();
				pLsRef->GetLsPtr()->correct_pos();
			}
		}
		else if(pPart1.IsHasPtr()&&pPart1->GetClassTypeId()==CLS_PARAMPLATE)
			pPart1.ParamPlatePointer()->DesignPlate();
		else if(pPart1.IsHasPtr()&&pPart1->GetClassTypeId()==CLS_LINEANGLE)
		{
			pPart1.LineAnglePointer()->cal_x_wing_norm();
			pPart1.LineAnglePointer()->cal_y_wing_norm();
			pPart1.LineAnglePointer()->CalPosition();
			for(CLsRef *pLsRef=pPart1.LineAnglePointer()->GetFirstLsRef();pLsRef;pLsRef=pPart1.LineAnglePointer()->GetNextLsRef())
			{
				pLsRef->GetLsPtr()->correct_worknorm();
				pLsRef->GetLsPtr()->correct_pos();
			}
		}
	}
	DWORD styleFlag=pOutputInfo->ChkStyleFlag();
	return pPartArr[0]->IsEqual(pPartArr[1],TRUE,pOutputInfo->tolerance,NULL,NULL,&styleFlag);
}
#endif
DWORD COutputInfo::ChkStyleFlag()
{
	if(m_uType==INFO_CHK_MIRROD)
		return CHK_MIRROD_BASICINFO;
	else if(m_uType==INFO_CHK_MIRROD_LENGTH)
		return CHK_MIRROD_LENGTHINFO;
	else
		return 0;
}
bool COutputInfo::CorrectError()
{
	/*if(m_uType==INFO_CHK_ANGLECUT)
		state=UnifyAngleCutInfo(this);	//ͳһ�Ǹ��н���Ϣ
	else */if(m_uType==INFO_EMEND_OVERLAPNODE)
		state=MergeOverlapNodes(this);	//�ϲ��ص��ڵ�
	else if(m_uType==INFO_EMEND_OVERLAPROD)
		state=MergeOverlapRods(this);	//�ϲ��ص��˼�
#ifdef __COMMON_PART_INC_
	else if(m_uType==INFO_CHK_REDUNDANTBOLT)
		state=RemoveRedundantBolt(this);//���������˨
	else if(m_uType==INFO_CHK_BOLTOVERLAP)
		state=MergeOverlapBolts(this);	//�ϲ��ص���˨
	else if(m_uType==INFO_BOLT_NORM_ZERO||m_uType==INFO_PART_LOCATION_ABNORMAL)
		state=RemoveInvalidPart(this);	//������󹹼�
	else if(m_uType==INFO_INVALID_PART_UCS)
		state=RemoveInvalidPart(this);	//������󹹼�
	else if(m_uType==INFO_INVALID_PROFILE)
		state=RemoveInvalidPart(this);	//������󹹼�
	else if(m_uType==INFO_CHK_RODBOLTINFO||m_uType==INFO_CHK_PLATEPROFILEANDBOLT||m_uType==INFO_CHK_MIRPLATE)
		state=RedesignPartAndCheck(this);//�������,Ȼ���ټ��
	else if(m_uType==INFO_CHK_BOLT_SPEC||m_uType==INFO_CHK_BOLT_GRADE)
		state=ReCalBoltGuigeAndGrade(this,m_uType==INFO_CHK_BOLT_GRADE);
#endif
	return state;
}

//////////////////////////////////////////////////////////////////////////
//CCheckPartNoInfo
//////////////////////////////////////////////////////////////////////////
CCheckPartNoInfo::CCheckPartNoInfo()
{
	m_idOutputType=2;
	m_uType=INFO_CHK_MIRROD;	//����
	m_hRelaObj1=0;				//��������id1
	m_hRelaObj2=0;				//��������id2
	m_sDescription="";			//������Ϣ
	m_sPartNo="";	
}
CCheckPartNoInfo::~CCheckPartNoInfo()
{

}
CCheckPartNoInfo::CCheckPartNoInfo(CString sPartNo,long hObj1,long hObj2,CString sDescription)
{
	m_idOutputType=2;
	m_uType=INFO_CHK_MIRROD;	//����
	//
	m_sPartNo=sPartNo;			//����
	m_hRelaObj1=hObj1;		//��������id1
	m_hRelaObj2=hObj2;		//��������id2
	m_sDescription=sDescription;//������Ϣ
}

//���¼�鵱ǰ���Ź���
bool CCheckPartNoInfo::Recheck()
{
#ifdef __COMMON_PART_INC_
	CLDSPart *pPart1=Ta.Parts.FromHandle(m_hRelaObj1);
	CLDSPart *pPart2=Ta.Parts.FromHandle(m_hRelaObj2);
	if(pPart1==NULL&&pPart2==NULL)
		return false;
	DWORD dwStyleFlag=ChkStyleFlag();
	state = pPart1->IsEqual(pPart2,TRUE,tolerance,NULL,NULL,&dwStyleFlag);
#endif
	/*ATOM_LIST<CCheckPartNoInfo> itemList;
	PartNoCheck(itemList,"*",tolerance,pPart1->sPartNo);*/
	return state;
}

void AppendCheckLabelOutputInfo(CTaskOutputList* pItemList,CCheckPartNoInfo& checklabel)
{
	CCheckPartNoInfo* pCheckLabel=(CCheckPartNoInfo*)pItemList->Add(0,2);
	*pCheckLabel=checklabel;
}
void CCheckPartNoInfo::PartNoCheck(CTaskOutputList &itemList,CString sSegStr,
								   double fScope,char *sPartNo/*=NULL*/)
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	int nSumProgress=0;
	//�ֽ�κ��ַ���
	CHashList<SEGI>segTbl;
	GetSegNoHashTblBySegStr(sSegStr,segTbl);
	//1.����������������ÿ�����Ŷ�Ӧ�Ĺ��������ϣ��,����ΪNULL�ĵĹ���������keyΪ"NULL"��������
	typedef ARRAY_LIST<CLDSPart*> PART_PTR_ARR;
	typedef ARRAY_LIST<CLDSPolyPart*>POLYPART_PTR_ARR;
	CHashStrList<PART_PTR_ARR> partsHasListPartNo;
	CSuperSmartPtr<CLDSPart> pPart;
	for(pPart=Ta.Parts.GetFirst();pPart.IsHasPtr();pPart=Ta.Parts.GetNext())
	{
		if(sPartNo==NULL)
		{
			if(pPart->GetClassTypeId()==CLS_BOLT||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE||
				pPart->GetClassTypeId()==CLS_POLYJOINT||pPart->m_bVirtualPart)
				continue;	//��˨�ޱ�Ų���Ԥ�����ż��
			else if(pPart->IsLinePart()&&(pPart.LinePartPointer()->bHuoQuStart||pPart.LinePartPointer()->bHuoQuEnd))
				continue;	//
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPart->dwPermission))
				continue;	//����Эͬʱ���Դ˽Ǹ�û��ӵ���޸�Ȩ��
			if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pPart->iSeg.iSeg)==NULL)
				continue;	//����Ƿ���ָ���κŷ�Χ��
		}
		else if(stricmp(sPartNo,pPart->GetPartNo())!=0)
			continue;		//���ָ����Ź���
		CString sKey="NULL";//����Ϊ�յĹ�����NULL��Ϊ��ֵ
		if(strlen(pPart->GetPartNo())>0)
			sKey=pPart->GetPartNo();
		PART_PTR_ARR *pPartArr=partsHasListPartNo.GetValue(sKey.GetBuffer(0));
		if(pPartArr==NULL)
			pPartArr=partsHasListPartNo.Add(sKey.GetBuffer(0));
		pPartArr->append(pPart);
		nSumProgress++;
	}
	//2.����ͳ�����乹��������,����ͬ�Ϲ�ϣ��
	POLYPARTLIST polylist;
	Ta.RetrievePolyPartList(polylist);
	CHashStrList<POLYPART_PTR_ARR> polyPartsHasListPartNo;
	CLDSPolyPart *pPolyPart=NULL;
	for(pPolyPart=polylist.GetFirst();pPolyPart;pPolyPart=polylist.GetNext())
	{
		if(sPartNo==NULL)
		{
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPolyPart->dwPermission))
				continue;	//����Эͬʱ���Դ˹���û��ӵ���޸�Ȩ��
			if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pPolyPart->iSeg.iSeg)==NULL)
				continue;	//����Ƿ���ָ��κŷ�Χ��
			CLDSLinePart *pFirstPole=pPolyPart->segset.GetFirst();
			if(pFirstPole->GetClassTypeId()!=CLS_LINEANGLE
				&&pFirstPole->GetClassTypeId()!=CLS_LINETUBE
				&&pFirstPole->GetClassTypeId()!=CLS_LINESLOT
				&&pFirstPole->GetClassTypeId()!=CLS_LINEFLAT)
				continue;
		}
		else if(stricmp(sPartNo,pPolyPart->GetPartNo())!=0)
			continue;		//���ָ����Ź���
		CString sKey="NULL";//����Ϊ�յĹ�����NULL��Ϊ��ֵ
		if(strlen(pPolyPart->GetPartNo())>0)
			sKey=pPolyPart->GetPartNo();
		POLYPART_PTR_ARR *pPolyPartArr=polyPartsHasListPartNo.GetValue(sKey.GetBuffer(0));
		if(pPolyPartArr==NULL)
			pPolyPartArr=polyPartsHasListPartNo.Add(sKey.GetBuffer(0));
		pPolyPartArr->append(pPolyPart);
		nSumProgress++;
	}
	try{
		int i=0;
		CString sError;
		//3.����ͨ�������м��ż��
		for(PART_PTR_ARR *pPartArr=partsHasListPartNo.GetFirst();pPartArr;pPartArr=partsHasListPartNo.GetNext())
		{
			if(stricmp(partsHasListPartNo.GetCursorKey(),"NULL")==0)
			{	//����Ϊ�յĹ���
				for(CLDSPart **ppPart=pPartArr->GetFirst();ppPart;ppPart=pPartArr->GetNext())
				{
					CString sPartName=(*ppPart)->GetPartTypeName();
#ifdef AFX_TARG_ENU_ENGLISH
					sError.Format("%s0x%X part No. not input correctly,please check!",sPartName,(*ppPart)->handle);
#else
					sError.Format("%s0x%X�Ĺ�����δ��ȷ����,���ʵ!",sPartName,(*ppPart)->handle);
#endif
					AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo((char*)(*ppPart)->GetPartNo(),(*ppPart)->handle,0,sError));
					//��ʾ������
					i++;
					DisplayProcess((i*100)/nSumProgress);
				}
			}
			else if(pPartArr->GetSize()>1)
			{	//���ŷǿ���ͬ���Ź���������1ʱ���бȶ�
				CLDSPart **ppFirstPart=pPartArr->GetFirst();
				CString sFirstPartName=(*ppFirstPart)->GetPartTypeName();
				long hFirstPart=(*ppFirstPart)->handle;
				CString sPartNo=(*ppFirstPart)->GetPartNo();
				i++;
				for(CLDSPart **ppPart=pPartArr->GetFirst();ppPart;ppPart=pPartArr->GetNext())
				{
					char error_info[200]="";
					long hPart=(*ppPart)->handle;
					if((*ppFirstPart)->GetClassTypeId()!=(*ppPart)->GetClassTypeId())
					{
						CString sPartName=(*ppPart)->GetPartTypeName();
#ifdef AFX_TARG_ENU_ENGLISH
						sError.Format("%s0x%X and %s0x%X are not of the same type parts��but coded with the same Number��",
#else
						sError.Format("%s0x%X��%s0x%X����ͬһ���͹�������������ͬ�Ĺ����ţ�",
#endif
							sFirstPartName,hFirstPart,sPartName,hPart);
						AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo(sPartNo,hFirstPart,hPart,sError));
					}
					else if(!(*ppFirstPart)->IsEqual(*ppPart,TRUE,fScope,error_info))
					{
#ifdef AFX_TARG_ENU_ENGLISH
						sError.Format("%s0X%X and 0X%X(%s)are coded with the same Number",sFirstPartName,hFirstPart,hPart,error_info);
#else
						sError.Format("%s0X%X��0X%X(%s)��������ͬ�Ĺ�����",sFirstPartName,hFirstPart,hPart,error_info);
#endif
						AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo(sPartNo,hFirstPart,hPart,sError));
					}
					//��ʾ������
					i++;
					DisplayProcess((i*100)/nSumProgress);
				}
			}
		}
		//4.�����乹�����м��ż��
		POLYPART_PTR_ARR *pPolyPartArr=NULL;
		for(pPolyPartArr=polyPartsHasListPartNo.GetFirst();pPolyPartArr;pPolyPartArr=polyPartsHasListPartNo.GetNext())
		{
			if(stricmp(polyPartsHasListPartNo.GetCursorKey(),"NULL")==0)
			{	//����Ϊ�յĹ���
				for(CLDSPolyPart **ppPart=pPolyPartArr->GetFirst();ppPart;ppPart=pPolyPartArr->GetNext())
				{
					CString sPartName=(*ppPart)->GetPartTypeName();
#ifdef AFX_TARG_ENU_ENGLISH
					sError.Format("%s0x%X Part No not input correctly,please check!",sPartName,(*ppPart)->handle);
#else
					sError.Format("%s0x%X�Ĺ�����δ��ȷ����,���ʵ!",sPartName,(*ppPart)->handle);
#endif
					AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo((char*)(*ppPart)->GetPartNo(),(*ppPart)->handle,0,sError));
					//��ʾ������
					i++;
					DisplayProcess((i*100)/nSumProgress);
				}
			}
			else if(pPolyPartArr->GetSize()>1)
			{	//���ŷǿ���ͬ���Ź���������1ʱ���бȶ�
				CLDSPolyPart **ppFirstPart=pPolyPartArr->GetFirst();
				CString sFirstPartName=(*ppFirstPart)->GetPartTypeName();
				long hFirstPart=(*ppFirstPart)->handle;
				CString sPartNo=(*ppFirstPart)->GetPartNo();
				i++;
				if(PART_PTR_ARR *pPartArr=partsHasListPartNo.GetValue(sPartNo.GetBuffer(0)))
				{	//��������������Ƿ�������������ų�ͻ
					CLDSPart **ppTempPart=pPartArr->GetFirst();
					if(ppTempPart&&(*ppTempPart)->GetClassTypeId()!=(*ppFirstPart)->GetClassTypeId())
					{
#ifdef AFX_TARG_ENU_ENGLISH
						sError.Format("0X%X Part and 0X%X part{%s}are not of the same type��but coded with the same Number��",(*ppTempPart)->handle,hFirstPart,sPartNo);
#else
						sError.Format("0X%X������0X%X����{%s}����ͬһ���͹�������������ͬ�Ĺ����ţ�",(*ppTempPart)->handle,hFirstPart,sPartNo);
#endif
						AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo(sPartNo,hFirstPart,(*ppTempPart)->handle,sError));
					}
				}
				for(CLDSPolyPart **ppPart=pPolyPartArr->GetFirst();ppPart;ppPart=pPolyPartArr->GetNext())
				{
					char error_info[200]="";
					long hPart=(*ppPart)->handle;
					if((*ppFirstPart)->GetClassTypeId()!=(*ppPart)->GetClassTypeId())
					{
						CString sPartName=(*ppPart)->GetPartTypeName();
#ifdef AFX_TARG_ENU_ENGLISH
						sError.Format("0X%X Part and 0X%X part{%s} are not of the same type��but coded with the same Number��",
#else
						sError.Format("%s0x%X��%s0x%X����ͬһ���͹�������������ͬ�Ĺ����ţ�",
#endif
							sFirstPartName,hFirstPart,sPartName,hPart);
						AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo(sPartNo,hFirstPart,hPart,sError));
					}
					else if(!(*ppFirstPart)->IsEqual(*ppPart,fScope,error_info))
					{
#ifdef AFX_TARG_ENU_ENGLISH
						sError.Format("%s0X%X and 0X%X(%s) are coded with the same Number",sFirstPartName,hFirstPart,hPart,error_info);
#else
						sError.Format("%s0X%X��0X%X(%s)��������ͬ�Ĺ�����",sFirstPartName,hFirstPart,hPart,error_info);
#endif
						AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo(sPartNo,hFirstPart,hPart,sError));
					}
					//��ʾ������
					i++;
					DisplayProcess((i*100)/nSumProgress);
				}
			}
		}
		DisplayProcess(100);
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
#endif
}
//////////////////////////////////////////////////////////////////////////
//CCheckGasketErrorInfo
//////////////////////////////////////////////////////////////////////////
#include "VertexsLifeObject.h"
CCheckGasketErrorInfo::CCheckGasketErrorInfo()
{
	m_idOutputType=3;
	m_uType=INFO_EMEND_BOLTGASKET;	//����
	m_hRelaObj1=0;				//��������id1
	m_hRelaObj2=0;				//��������id2
	m_sDescription="";			//������Ϣ
	siGasketThick=0;
	memset((long*)relaObjIds.pPresetObjs,0,sizeof(long)*relaObjIds.CountOfPreset);
}
CCheckGasketErrorInfo::~CCheckGasketErrorInfo()
{

}
long CCheckGasketErrorInfo::get_hRelaBolt(){
	return m_hRelaObj1;
}
long CCheckGasketErrorInfo::set_hRelaBolt(long hBolt){return m_hRelaObj1=hBolt;}
long CCheckGasketErrorInfo::get_hRelaGasket(){
	return m_hRelaObj2;
}
long CCheckGasketErrorInfo::set_hRelaGasket(long hGasket){return m_hRelaObj2=hGasket;}
CLDSLineAngle *GetBoltGasketCutAngle(CLDSBolt* pBolt);	//defined in EmendBoltGasket.cpp
bool CCheckGasketErrorInfo::CorrectError()
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	if(m_uType==ERROR_ISOLATED_BOLT)
	{	//���������˨
		return state=console.DeletePart(this->hRelaBolt)!=FALSE;
	}
	else if(m_uType==ERROR_VIRTUAL_HOLE_LIKELY||m_uType==ERROR_VIRTUAL_WIREHOLE_LIKELY)
	{	//Ŀǰ��һ�����������ķǽŶ����������
		CLDSBolt* pBolt=(CLDSBolt*)console.FromPartHandle(this->hRelaBolt);
		if(pBolt)
		{
			state=true;
			pBolt->m_bVirtualPart=true;
			g_pSolidDraw->DelEnt(pBolt->handle);
			pBolt->SetModified(TRUE,FALSE);
			CLDSPart* pRelaPart=console.FromPartHandle(this->m_hRelaObj2);
			if(pRelaPart)
			{	//������������׵�ʵ����ʾ
				pRelaPart->SetSolidModified(TRUE);
				pRelaPart->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pRelaPart->GetSolidPartObject());
				g_pSolidDraw->Draw();
			}
			return true;
		}
		return false;
	}
	else if(m_uType==ERROR_MISSING_CROSSGASKET)
	{	//����㴦���ȱʧ������ʽΪ����ӵ����Ȧ
		CLDSBolt* pBolt=(CLDSBolt*)console.FromPartHandle(this->hRelaBolt);
		int type=CLDSBolt::CalPadPara(pBolt->get_d(),abs(siGasketThick));
		if(type==DIANQUAN_STRU::DIANBAN)
		{
			pBolt->DianQuan.N=0;
			CLDSPlate* pGasketPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
			pGasketPlate->wiType=CLDSPlate::WASHER_PLATE;
			//��ӵ�嶨λ��Ϣ
			pGasketPlate->designInfo.m_hBasePart=anchor.hAnchorPart;
			//��ӵ���������˨����
			ARRAY_LIST<CLDSBolt*> bolts(0,relaObjIds.Count);
			for(UINT i=0;i<this->relaObjIds.Count;i++)
			{
				CLDSBolt* pRelaBolt=(CLDSBolt*)console.FromPartHandle(relaObjIds.At(i),CLS_BOLT);
				if(pRelaBolt)
				{
					pGasketPlate->AppendLsRef(pRelaBolt->GetLsRef());
					bolts.append(pRelaBolt);
				}
			}
			if(bolts.GetSize()>0)
			{
				CLDSBolt::CorrectBoltZnL0Para(bolts.m_pData,bolts.GetSize());
				for(int j=0;j<bolts.GetSize();j++)
				{
					bolts[j]->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(bolts[j]->GetSolidPartObject());
				}
			}
			if(pGasketPlate->DesignPlate())
			{
				pGasketPlate->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pGasketPlate->GetSolidPartObject());
			}
		}
		else if(type==DIANQUAN_STRU::DIANQUAN)
		{	//��ӵ�Ȧ
			pBolt->DianQuan.thick=CLDSPart::library->GetBestMatchWasherThick(pBolt->get_d(),siGasketThick);
			pBolt->DianQuan.N=abs(siGasketThick)/pBolt->DianQuan.thick;
			pBolt->DianQuan.offset=this->anchor.washerOffset;
		}
	}
	else if(m_uType==ERROR_MISSING_ENDGASKET)
	{	//�����Ӵ����ȱʧ������ʽΪ����ӵ����Ȧ
		CLDSBolt* pBolt=(CLDSBolt*)console.FromPartHandle(this->hRelaBolt);
		int type=CLDSBolt::CalPadPara(pBolt->get_d(),abs(siGasketThick),&pBolt->DianQuan);
		if(type==DIANQUAN_STRU::DIANBAN)
		{
			pBolt->DianQuan.N=0;
			CLDSPart* pDatumPart=Ta.Parts.FromHandle(anchor.hAnchorPart);
			CLDSLineAngle* pCutAngle=GetBoltGasketCutAngle(pBolt);
			//��ӵ���������˨����
			CLDSBolt* pRelaBolt;
			BOLTSET boltset;
			for(UINT i=0;i<this->relaObjIds.Count;i++)
			{
				pRelaBolt=(CLDSBolt*)console.FromPartHandle(relaObjIds.At(i),CLS_BOLT);
				if(pRelaBolt)
					boltset.append(pRelaBolt);
			}
			CLDSPlate* pGasketPlate=pView->DesignBoltPad(pDatumPart,pCutAngle,boltset,siGasketThick,anchor.gasketOffset);
			if(pGasketPlate)
			{
				this->state=true;
				for(pRelaBolt=boltset.GetFirst();pRelaBolt;pRelaBolt=boltset.GetNext())
				{
					pRelaBolt->CorrectBoltZnL0Para();
					pRelaBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pRelaBolt->GetSolidPartObject());
				}
				//��ӵ�嶨λ��Ϣ
				/*pGasketPlate->designInfo.m_hBasePart=anchor.hAnchorPart;
				pGasketPlate->Thick=this->siGasketThick;
				if(pGasketPlate->DesignPlate())*/
				pGasketPlate->wiType=CLDSPlate::WASHER_PLATE;
				pGasketPlate->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pGasketPlate->GetSolidPartObject());
			}
		}
		else if(type==DIANQUAN_STRU::DIANQUAN)
		{	//��ӵ�Ȧ
			pBolt->CalPadPara(this->siGasketThick);
			pBolt->DianQuan.offset=this->anchor.washerOffset;
			state=true;
		}
		if(state)
		{
			CLDSBolt::CorrectBoltZnL0Para(&pBolt);
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
	}
	else if(m_uType==ERROR_GASKET_THICK)
	{	//�����Ȧ��ȴ��󣬴���ʽΪ�������޶������Ȧ���
		CLDSBolt* pBolt=(CLDSBolt*)console.FromPartHandle(this->hRelaBolt);
		CLDSPlate* pGasket=hRelaGasket>0?(CLDSPlate*)console.FromPartHandle(hRelaGasket,CLS_PLATE):NULL;
		bool oldstate=state;
		state=true;
		if(pGasket)
			pGasket->Thick=abs(siGasketThick);
		else if(pBolt)
			pBolt->CalPadPara(siGasketThick);
		else
			state=oldstate;
		if(state)
		{
			CLDSBolt::CorrectBoltZnL0Para(&pBolt);
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
	}
	else if(m_uType==ERROR_PART_POS)
	{	//����λ�ô���
		SmartPartPtr pErrPart=console.FromPartHandle(this->relaObjIds[0]);
		if(pErrPart.IsNULL())
			return false;
		if(pErrPart->GetClassTypeId()==CLS_PLATE)
			pErrPart.pPlate->DesignPlate();
		else if(pErrPart->GetClassTypeId()==CLS_PARAMPLATE)
			pErrPart.pParamPlate->DesignPlate();
		else if(pErrPart->IsLinePart())
			pErrPart.pRod->CalPosition();
		else
			return false;
		return true;
	}
	else if(m_uType==ERROR_BOLTPAD_CUTANGLE)
	{	//���ȱ���нǴ���
		CLDSPlate* pGasketPlate=(CLDSPlate*)console.FromPartHandle(this->hRelaGasket,CLS_PLATE);
		CLDSLineAngle* pCutAngle=(CLDSLineAngle*)console.FromPartHandle(this->relaObjIds[0],CLS_LINEANGLE);
		CVertexsLifeObject manageVertexs(pGasketPlate->vertex_list);
		if(pView&&pView->CutBoltPadByAngle(pGasketPlate,pCutAngle))	//�������нǴ���
		{
			return true;
		}
		else	//δ�������θı�
		{
			manageVertexs.DisableBackup();
			return false;
		}
	}
	else if(m_uType==ERROR_GASKET_PROFILE)
	{	//������β����ʣ�����ʽΪ�������������
		PLATESET gaskets;
		CLDSPlate* pPlate;
		for(UINT i=0;i<this->relaObjIds.Count;i++)
		{
			if((pPlate=(CLDSPlate*)console.FromPartHandle(relaObjIds[i],CLS_PLATE))!=NULL)
				gaskets.append(pPlate);
		}
		CHashList<CBoltPadProperty> hashPlateProps;
		ATOM_LIST<SAMESCHEMA_BOLTPAD> sameSchemaBoltPadList;
		for(pPlate=gaskets.GetFirst();pPlate;pPlate=gaskets.GetNext())
		{
			CBoltPadProperty *pPadProp = hashPlateProps.Add(pPlate->handle);
			pPadProp->InitProperty(pPlate);
		}
		CBoltPadProperty datumprop;
		//1 ��ȡ��С���ζ�������
		CBoltPadProperty *pBoltPadProp=NULL,*pDatumProperty=&datumprop;//pSimBoltPad->pDatumProperty;
		CXhChar16 sPartNo;
		for(pPlate=gaskets.GetFirst();pPlate;pPlate=gaskets.GetNext())
		{
			if(pPlate->handle==pDatumProperty->m_pBoltPad->handle)
				continue;
			pBoltPadProp=hashPlateProps.GetValue(pPlate->handle);
			pBoltPadProp->UpdateMinProfileVertex(pDatumProperty);
			if(sPartNo.GetLength()<=0&&strlen(pPlate->Label)>0)
				sPartNo.Copy(pPlate->Label);
		}
		//2 �������ζ�������
		for(pPlate=gaskets.GetFirst();pPlate;pPlate=gaskets.GetNext())
		{
			pBoltPadProp=hashPlateProps.GetValue(pPlate->handle);
			//if(pBoltPadProp==NULL)
			//	continue;
			if(sPartNo.GetLength()>0)
				pPlate->SetPartNo(sPartNo);
			BOOL bRetCode = pBoltPadProp->EmendProfileVertexs(pDatumProperty);
		}
		return true;
	}
	else if(m_uType==ERROR_REDUNDANT_GASKET)
	{	//�����壬����ʽΪ��������
		return console.DeletePart(hRelaGasket)==TRUE;
	}
	return state;
}
// ����ֵ>=0��ʾ֧�ִ�����������
int CCheckGasketErrorInfo::CorrectErrorCmdInfo(AUTOCORRECT_CMDINFO &cmdInfo)
{
	cmdInfo.safeType=-1;
	if(state)	//�ѽ��������Ҫ������
		return -1;
	if(m_uType==ERROR_ISOLATED_BOLT)
	{	//���������˨
		cmdInfo.safeType=1;
		cmdInfo.nativeName.Copy("���");
	}
	else if(m_uType==ERROR_VIRTUAL_HOLE_LIKELY||m_uType==ERROR_VIRTUAL_WIREHOLE_LIKELY)
	{	//Ŀǰ��һ�����������ķǽŶ����������
		cmdInfo.safeType=1;
		cmdInfo.nativeName.Copy("��Ϊ���п�");
	}
	else if(m_uType==ERROR_MISSING_CROSSGASKET)
	{	//����㴦���ȱʧ������ʽΪ����ӵ����Ȧ
		cmdInfo.safeType=0;	//����㴦�������������ϽǸ�Ӱ�������Ҫ��Ϊ����
		cmdInfo.nativeName.Copy("��ӵ����Ȧ");
	}
	else if(m_uType==ERROR_MISSING_ENDGASKET)
	{	//�����Ӵ����ȱʧ������ʽΪ����ӵ����Ȧ
		cmdInfo.safeType=0;
		cmdInfo.nativeName.Copy("��ӵ����Ȧ");
	}
	else if(m_uType==ERROR_GASKET_THICK)
	{	//�����Ȧ��ȴ��󣬴���ʽΪ�������޶������Ȧ���
		cmdInfo.safeType=0;
		cmdInfo.nativeName.Copy("�������(Ȧ)���");
	}
	else if(m_uType==ERROR_PART_POS)
	{	//����λ�ô���
		cmdInfo.safeType=0;
		return -1;
	}
	else if(m_uType==ERROR_BOLTPAD_CUTANGLE)
	{	//���ȱ���нǴ���
		cmdInfo.safeType=0;
		cmdInfo.nativeName.Copy("����н�");
	}
	else if(m_uType==ERROR_GASKET_PROFILE)
	{	//������β����ʣ�����ʽΪ�������������
		cmdInfo.safeType=0;
		cmdInfo.nativeName.Copy("��׼������");
	}
	else if(m_uType==ERROR_REDUNDANT_GASKET)
	{	//�����壬����ʽΪ��������
		cmdInfo.safeType=0;
		cmdInfo.nativeName.Copy("���");
	}
	else
		return -1;
	PRESET_ARRAY4<int> a;
	return cmdInfo.safeType;
}
bool CCheckGasketErrorInfo::Recheck()
{
	return false;
}
bool CCheckGasketErrorInfo::CopyFrom(IBasicOutput* pOutput)
{
	COutputInfo::CopyFrom(pOutput);
	CCheckGasketErrorInfo* pCopyFrom=(CCheckGasketErrorInfo*)pOutput;
	siGasketThick=pCopyFrom->siGasketThick;//����Ȧ���(��ֵ��ʾ����Ի�������˨�����������£�
	anchor=pCopyFrom->anchor;
	if(relaObjIds.Count!=pCopyFrom->relaObjIds.Count)
	{
		relaObjIds.Clear();
		for(UINT i=0;i<pCopyFrom->relaObjIds.Count;i++)
			relaObjIds.Append(pCopyFrom->relaObjIds.At(i)); 
	}
	else
	{
		for(UINT i=0;i<pCopyFrom->relaObjIds.Count;i++)
			relaObjIds.Set(i,pCopyFrom->relaObjIds.At(i)); 
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
CHashStrList<COutputInfo> markInfoList;