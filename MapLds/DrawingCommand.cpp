//<LOCALE_TRANSLATE BY hxr /> 2015-04-28
// DrawScriptRecord.cpp: implementation of the CDrawScriptRecord class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Drawing.h"
#include "SysPara.h"
#include "DrawTaMap.h"
#include "SortFunc.h"
#include "SegMatSum.h"
#include "DragEntSet.h"
#include "AdsTools.h"
#include "MenuFunc.h"
#include "InputDrawingUcsDlg.h"
#include "JgDrawing.h"
#include "PaperPatternDrawing.h"
#include "GlobalFunc.h"
#include "JgDrawing.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef __SMART_DRAW_INC_
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CHashList<AcDbObjectId> CDrawingCommand::hashHighlightEnts;
CLDSNode CDrawingCommand::DIM_MODELSIZE_PARA::xMidAxisNode;
CDrawingCommand::DIMNODE* CDrawingCommand::DIM_MODELSIZE_PARA::AppendDimNode(CLDSNode* _pNode/*=NULL*/)
{
	if(_pNode==NULL)
		_pNode=&xMidAxisNode;
	return listDimNodes.append(DIMNODE(_pNode));
}
CDrawingCommand::CDrawingCommand()
{
	m_stateCmd=CMD_STATE_FREE;	//Ĭ������״̬Ϊ����״̬
	m_eCmdType=CMD_OTHER;
	m_bExecute=TRUE;
	m_bFlag=FALSE;
	feature=0;
	m_nDrawingSerialNo=0;
	m_paraPart.para.angle.bDrawAxisLine=TRUE;
	m_paraPart.para.angle.extendStyle=0;	//֫�������ʱ����������չ
	m_paraPart.para.bolt.bDrawSideBoltLine=false;
	m_paraPart.para.bolt.bOriginal=TRUE;
	m_paraPart.para.bolt.bDimHoleDiameter=true;
	m_paraPart.para.bolt.dimAngle=-135;
	m_paraPart.para.bolt.dimFarOffset=3;
}

CDrawingCommand::~CDrawingCommand()
{

}

//�����������ͳ�ʼ���������
void CDrawingCommand::InitCommandPara(int eDrawCmdType/*=0*/)	
{
	CString sId="",sCmdText="";
	if(eDrawCmdType>0)
		m_eCmdType=(DRAW_CMD_TYPE)eDrawCmdType;
	if(m_eCmdType==CMD_DRAW_PART)
	{	//���ƹ���
		m_paraPart.pPart=NULL;
		m_paraPart.hPart=0;
		m_paraPart.fPoleAxisLineStartOutSpreadLen=sys.stru_map.fPoleAxisLineStartOutSpreadLen;
		m_paraPart.fPoleAxisLineEndOutSpreadLen=sys.stru_map.fPoleAxisLineEndOutSpreadLen;
		m_paraPart.cDrawFlag='0';
		m_paraPart.para.angle.nXWingDatumAxisG=0;
		m_paraPart.para.angle.nYWingDatumAxisG=0;
		//������˨ʱ��Ҫ�Ĳ���
		m_paraPart.para.bolt.bOriginal=TRUE;	//�Ƿ�������˨
		m_paraPart.para.bolt.front0_side1=0;	//������˨���ǲ�����˨
		m_paraPart.para.bolt.bDrawSideBoltLine=false;
	}
	else if(m_eCmdType==CMD_DRAW_LINEPART)
	{	//���Ƹ˼�����ͼ
		m_paraLinePart.hLinePart=0;
		m_paraLinePart.pLinePart=NULL;
		m_paraLinePart.bSumMap=TRUE;
		m_paraLinePart.bDimByAnalysisMode=FALSE;
		m_paraLinePart.bAdjoinRod=false;	//Ĭ�ϻ�Ϊʵ��
	}
	else if(m_eCmdType==CMD_DRAW_TABLE)
	{	//���
		m_paraBOMTable.nLocatePos=0;	//0.���� 1.���� 2.���� 4.����
		m_paraBOMTable.bSelectPos=TRUE;	//ѡ����λ��
		m_paraBOMTable.bDisplayDlg=FALSE;//��ʾ�Ի���
		m_paraBOMTable.fDimPosX=0;		//���λ��X����		
		m_paraBOMTable.fDimPosY=0;		//���λ��Y����
		m_paraBOMTable.iTableType=0;	//������� 	
		m_paraBOMTable.iFormatTbl=0;
	}
	else if(m_eCmdType==CMD_DIM_PARTSPEC)
	{	//������ϸ
		m_paraPartSpec.hObj=0;
		m_paraPartSpec.pObj=NULL;		//�ű���¼��������
		m_paraPartSpec.fBasePosOffsetX=0;	//���ݵ�λ��X����ƫ����
		m_paraPartSpec.fBasePosOffsetY=0;	//���ݵ�λ��Y����ƫ����
		m_paraPartSpec.fDimPosOffsetX=0;	//��עλ��X����ƫ��ֵ
		m_paraPartSpec.fDimPosOffsetY=0;	//��עλ��Y����ƫ��ֵ
		m_paraPartSpec.fGuigeOffsetX=0;	//�ְ����עλ��X����ƫ��ֵ
		m_paraPartSpec.fGuigeOffsetY=0;	//�ְ����עλ��Y����ƫ��ֵ
		m_paraPartSpec.iDimPosType=0;	//0.�Զ�����λ�� 1.ָ��λ��(����ָ��λ����ԭ��עλ�õ�ƫ��ֵ)  2.ѡ��λ��
		m_paraPartSpec.bOnlyDimPartNo=FALSE;
		m_paraPartSpec.bDrawRefLine=sys.dim_map.PartNo.bDrawRefLine;//�Ƿ����������
		m_paraPartSpec.fPoleDimPosCoef=0.3;					//�˼���עλ��ϵ�� //LTMA 0.3  TAS 0.5
		m_paraPartSpec.iMirMsg=sys.dim_map.PartNo.iMirMsg;	//0:X��Գƣ�1:Y��Գ�;���಻�Գƴ���
	}
	else if(m_eCmdType==CMD_DIM_SEGI)
	{	//��ע�κ�
		m_paraDimSegI.bSelectPos=TRUE;
		m_paraDimSegI.fDimPosX=0;
		m_paraDimSegI.fDimPosY=0;
	}
	else if(m_eCmdType==CMD_DIM_BOLTPAD)
	{	//��˨��Ϣ����˨��Ȧ
		m_paraBoltPad.hBolt=0;
		m_paraBoltPad.pBolt=NULL;		//������˨
		m_paraBoltPad.iDimPosType=0;	//�Զ�����
		m_paraBoltPad.fDimPosOffsetX=0;	//��עλ��X����ƫ��ֵ
		m_paraBoltPad.fDimPosOffsetY=0;	//��עλ��Y����ƫ��ֵ
	}
	else if(m_eCmdType==CMD_DIM_BOLTINFO)
	{	//��˨��Ϣ��ע
		m_paraBoltGroupInfo.iDimPosType=0;
		m_paraBoltGroupInfo.fDimPosOffsetX=0;
		m_paraBoltGroupInfo.fDimPosOffsetY=0;
	}
	else if(m_eCmdType==CMD_DIM_ODDMENT)
	{	//����ͷ
		m_paraOddment.hLinePart=0;
		m_paraOddment.pLinePart=NULL;
		m_paraOddment.iDimPosType=0;	//0.�Զ�����λ�� 1.ָ��λ��
		m_paraOddment.bDimStartOdd=TRUE;
		m_paraOddment.bDimEndOdd=TRUE;
		m_paraOddment.fStartOddPosOffsetX=m_paraOddment.fStartOddBasePosX=0;
		m_paraOddment.fStartOddPosOffsetY=m_paraOddment.fStartOddBasePosY=0;
		m_paraOddment.fEndOddPosOffsetX=m_paraOddment.fEndOddBasePosX=0;
		m_paraOddment.fEndOddPosOffsetY=m_paraOddment.fEndOddBasePosY=0;
		m_paraOddment.fPoleAngle=0;
	}
	else if(m_eCmdType==CMD_DIM_BOLTORFL_INFO)
	{	//˾��ͼ�˼���˨������Ϣ
		m_paraBoltOrFlInfo.hLinePart=0;
		m_paraBoltOrFlInfo.pLinePart=NULL;
		m_paraBoltOrFlInfo.iDimPosType=0;	//0.�Զ�����λ�� 1.ָ��λ��
		m_paraBoltOrFlInfo.bDimStartOdd=TRUE;
		m_paraBoltOrFlInfo.bDimEndOdd=TRUE;
		m_paraBoltOrFlInfo.fStartOddPosOffsetX=m_paraBoltOrFlInfo.fStartOddBasePosX=0;
		m_paraBoltOrFlInfo.fStartOddPosOffsetY=m_paraBoltOrFlInfo.fStartOddBasePosY=0;
		m_paraBoltOrFlInfo.fEndOddPosOffsetX=m_paraBoltOrFlInfo.fEndOddBasePosX=0;
		m_paraBoltOrFlInfo.fEndOddPosOffsetY=m_paraBoltOrFlInfo.fEndOddBasePosY=0;
		m_paraBoltOrFlInfo.fPoleAngle=0;
	}
	else if(m_eCmdType==CMD_DIM_ANGLEJOINT)
	{	//��ע�Ǹֽ�ͷ��Ϣ
		m_paraAngleJoint.pJoint=NULL;
		m_paraAngleJoint.idAngleJoint=0;
		m_paraAngleJoint.iDimPosType=0;
		m_paraAngleJoint.fDimPosOffsetX=0;
		m_paraAngleJoint.fDimPosOffsetY=0;
	}
	else if(m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)
	{	//�Ŷ����
		m_paraDimFootNailSpace.hLinePart=0;
		m_paraDimFootNailSpace.pLinePart=NULL;
		m_paraDimFootNailSpace.fSpace=5;
		m_paraDimFootNailSpace.fDimDistance=10;
	}
	else if(m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
	{	//�����
		m_paraDimFillPlateSpace.hLinePart=0;
		m_paraDimFillPlateSpace.pLinePart=NULL;
		m_paraDimFillPlateSpace.fSpace=5;
		m_paraDimFillPlateSpace.fDimDistance=10;
	}
	else if(m_eCmdType==CMD_DIM_SIMPOLY)
	{	//������
		m_paraSimPoly.nSimPolyCmdType=2;	//2.��ָ��������������
		m_paraSimPoly.fScale=sys.dim_map.fSimTriScale;	//ָ�������α���
		m_paraSimPoly.fPickPtX=0;			//������ʰȡ��X����		
		m_paraSimPoly.fPickPtY=0;			//������ʰȡ��Y����
		m_paraSimPoly.fDimPosOffsetX=0;		//�����α�עλ��X�᷽��ƫ����
		m_paraSimPoly.fDimPosOffsetY=0;		//�����α�עλ��Y�᷽��ƫ����
	}
	else if(m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)
	{	//����Ǹ�׼��
		m_paraDimJgZhun.hLineAngle=0;
		m_paraDimJgZhun.pLineAngle=NULL;	//��Ҫ��ע����׼�ݵĽǸ�
		memset(&m_paraDimJgZhun.zhun_dim,0,sizeof(CZhunXianTextDim));
		m_paraDimJgZhun.fSpace=0;
		m_paraDimJgZhun.fDimDistance=6;
		m_paraDimJgZhun.bInside=TRUE;
	}
	else if(m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
	{	//������˨���
		m_paraDimLsSpace.hPlate=0;
		m_paraDimLsSpace.pPlate=NULL;
		memset(&m_paraDimLsSpace.spaceDim,0,sizeof(m_paraDimLsSpace.spaceDim));
		m_paraDimLsSpace.fDimDistance=6;	
		m_paraDimLsSpace.fSpace=0;
		m_paraDimLsSpace.bInside=TRUE;
	}
	else if(m_eCmdType==CMD_DIM_MODELSIZE)
	{
		m_paraDimModelSize.fMinX=0;
		m_paraDimModelSize.fSpace = 0;
		m_paraDimModelSize.fDimDistance = 0;
	}
	else if(m_eCmdType==CMD_DIM_MAPSIZE)
	{	//ģ�ͳߴ�
		m_paraDimSize.fDimStartX=0;
		m_paraDimSize.fDimStartY=0;
		m_paraDimSize.fDimEndX=0;
		m_paraDimSize.fDimEndY=0;
		m_paraDimSize.fDimPosX=0;
		m_paraDimSize.fDimPosY=0;
		m_paraDimSize.fScale=0;		//�ߴ����
		m_paraDimSize.iDimDirect=0;	//��ע���� 0.X�᷽�� 1.Y�᷽�� 2.���췽��
	}
	else if(m_eCmdType==CMD_DRAW_ASSIST_LINE)
	{	//���Ƹ�����
		m_paraDrawAssistLine.nStyle=0;
		m_paraDrawAssistLine.iNo=-1;
	}
	else if(m_eCmdType==CMD_DRAW_JG_JOINT)
	{	//�Ǹֽ�ͷ����ͼ
		m_paraPart.pPart=NULL;
		m_paraPart.hPart=0;
	}
}

//�������λ����
void CDrawingCommand::UpdateCommandPara(double fOffsetX,double fOffsetY)
{
	if(fabs(fOffsetX)<EPS&&fabs(fOffsetY)<EPS)
		return;
	if(m_eCmdType==CMD_DIM_SEGI)
	{	//��ע�κ�
		m_paraDimSegI.fDimPosX+=fOffsetX;
		m_paraDimSegI.fDimPosY+=fOffsetY;
	}
	/*else if(m_eCmdType==CMD_DIM_ODDMENT)
	{	//��ע����ͷ
		if(m_paraOddment.bDimStartOdd)
		{
			m_paraOddment.fStartOddPosOffsetX+=fOffsetX;
			m_paraOddment.fStartOddPosOffsetY+=fOffsetY;
		}
		if(m_paraOddment.bDimEndOdd)
		{
			m_paraOddment.fEndOddPosOffsetX+=fOffsetX;
			m_paraOddment.fEndOddPosOffsetY+=fOffsetY;
		}
	}*/
	else if(m_eCmdType==CMD_DIM_SIMPOLY)
	{	//����������λ�� wht 11-06-29
		m_paraSimPoly.fPickPtX+=fOffsetX;
		m_paraSimPoly.fPickPtY+=fOffsetY;
		SIMPOLY_VERTEX *pVertex=NULL;
		for(pVertex=m_paraSimPoly.simPolyVertexList.GetFirst();pVertex;
			pVertex=m_paraSimPoly.simPolyVertexList.GetNext())
		{
			pVertex->vertex.x+=fOffsetX;
			pVertex->vertex.y+=fOffsetY;
		}
	}
}

//�õ�������Ϣ ��ʶ�ַ����Լ�������Ϣ
long CDrawingCommand::GetCmdInfo(char *sCmdId,char *sHelp,char *sPartNo/*=NULL*/)
{
	CString sCmdText=_T("");
	long hPart=0;
	if(m_eCmdType==CMD_DRAW_PART)
	{	//���ƹ���
		if(m_paraPart.pPart)
		{
			CLDSPart *pPart=m_paraPart.pPart;
			hPart=m_paraPart.pPart->handle;
			if(pPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(((CLDSLineAngle*)pPart)->m_bFootNail)
#ifdef AFX_TARG_ENU_ENGLISH
					sCmdText.Format("Draw (%s) angle nail",(char*)pPart->GetPartNo());
				else 
					sCmdText.Format("Draw (%s) angle",(char*)pPart->GetPartNo());
#else
					sCmdText.Format("����(%s)�ǸֽŶ�",(char*)pPart->GetPartNo());
				else 
					sCmdText.Format("����(%s)�Ǹ�",(char*)pPart->GetPartNo());
#endif
			}
			else if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
				sCmdText.Format("Draw group angle's bolt line");
			else if(pPart->GetClassTypeId()==CLS_BOLT)
				sCmdText.Format("Draw %s",GetDrawPartName());
			else
				sCmdText.Format("Draw (%s)%s",(char*)pPart->GetPartNo(),GetDrawPartName());
#else
				sCmdText.Format("������ϽǸ�����");
			else if(pPart->GetClassTypeId()==CLS_BOLT)
				sCmdText.Format("����%s",GetDrawPartName());
			else
				sCmdText.Format("����(%s)%s",(char*)pPart->GetPartNo(),GetDrawPartName());
#endif
			if(sPartNo)	//���
				strcpy(sPartNo,pPart->GetPartNo());
			//�����б����н���ʾ������ţ�����ʾ��˨��� 
			//if(m_paraPart.pPart->GetClassTypeId()==CLS_BOLT)
			//	((CLDSBolt*)m_paraPart.pPart)->GetSpecification(sPartNo,FALSE);	//��˨���
		}
	}
	else if(m_eCmdType==CMD_DRAW_TABLE)
	{	//���
		if(m_paraBOMTable.iTableType==1)
#ifdef AFX_TARG_ENU_ENGLISH
			sCmdText.Format("Draw sum material BOM");
		else if(m_paraBOMTable.iTableType==2)
			sCmdText.Format("Draw part BOM");
		else if(m_paraBOMTable.iTableType==3)
			sCmdText.Format("Draw bolt,foot nail and washer BOM");
		else if(m_paraBOMTable.iTableType==4)	
			sCmdText.Format("Weld part group BOM");
		else if(m_paraBOMTable.iTableType==5)	
			sCmdText.Format("Flange BOM");
		else if(m_paraBOMTable.iTableType==6)	
			sCmdText.Format("Simplify spec BOM");
		else if(m_paraBOMTable.iTableType==7)	
			sCmdText.Format("Angle G BOM");
		else 
			sCmdText.Format("Draw table");
#else
			sCmdText.Format("���Ʋ��ϻ��ܱ�");
		else if(m_paraBOMTable.iTableType==2)
			sCmdText.Format("���ƹ�����ϸ��");
		else if(m_paraBOMTable.iTableType==3)
			sCmdText.Format("������˨�Ŷ���Ȧ��ϸ��");
		else if(m_paraBOMTable.iTableType==4)	
			sCmdText.Format("�麸����ϸ��");
		else if(m_paraBOMTable.iTableType==5)	
			sCmdText.Format("������ϸ��");
		else if(m_paraBOMTable.iTableType==6)	
			sCmdText.Format("�򻯹����ձ�");
		else if(m_paraBOMTable.iTableType==7)	
			sCmdText.Format("�Ǹ�׼�ݱ�");
		else 
			sCmdText.Format("���Ʊ��");
#endif
	}
	else if(m_eCmdType==CMD_DIM_PARTSPEC)
	{	//������ϸ
		if(m_paraPartSpec.pObj)
		{
			CLDSDbObject *pObj=m_paraPartSpec.pObj;
			hPart=pObj->handle;
			if(pObj->IsPart())
			{
				if(pObj->GetClassTypeId()!=CLS_BOLT)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					sCmdText.Format("Dimension (%s)%s details",(char*)((CLDSPart*)pObj)->GetPartNo(),GetDrawPartName());
#else
					sCmdText.Format("��ע(%s)%s��ϸ",(char*)((CLDSPart*)pObj)->GetPartNo(),GetDrawPartName());
#endif
					if(sPartNo)	//���
						strcpy(sPartNo,((CLDSPart*)pObj)->GetPartNo());
				}
			}
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)||defined(__TSA_FILE_)
			else if(pObj->IsNode()) //��ʾ�ڵ��� wht 11-06-16
#ifdef AFX_TARG_ENU_ENGLISH
				sCmdText.Format("Dimension (%d)%s serial",((CLDSNode*)pObj)->point_i,GetDrawPartName());
#else
				sCmdText.Format("��ע(%d)%s���",((CLDSNode*)pObj)->point_i,GetDrawPartName());
#endif
#endif
		}
	}
	else if(m_eCmdType==CMD_DIM_BOLTINFO)
	{	//��˨��Ϣ
		CLDSBolt *pBolt=m_paraBoltGroupInfo.lsGroupDim.GetDatumBolt();
		if(pBolt)
		{
			hPart=pBolt->handle;
#ifdef AFX_TARG_ENU_ENGLISH
			sCmdText.Format("Dimension bolt information");
#else
			sCmdText.Format("��ע��˨��Ϣ");
#endif
			//if(sPartNo)
			//	m_paraBoltGroupInfo.lsGroupDim.GetDimText(sPartNo);
		}
	}
	else if(m_eCmdType==CMD_DIM_BOLTPAD)
	{	//��˨��Ȧ
		if(m_paraBoltPad.pBolt)
		{
			hPart=m_paraBoltPad.pBolt->handle;
#ifdef AFX_TARG_ENU_ENGLISH
			sCmdText.Format("Dimension bolt washer information");
#else
			sCmdText.Format("��ע��˨��Ȧ��Ϣ");
#endif
			//if(sPartNo&&m_paraBoltPad.pBolt->DianQuan.N>0&&m_paraBoltPad.pBolt->DianQuan.thick>0)
			//	sprintf(sPartNo,"-%dX%d",m_paraBoltPad.pBolt->DianQuan.thick,m_paraBoltPad.pBolt->DianQuan.N);
		}
	}
	else if(m_eCmdType==CMD_DIM_SEGI)
	{	//��ע�κ�
#ifdef AFX_TARG_ENU_ENGLISH
		sCmdText.Format("Dimension No.%s segment",(char*)m_paraDimSegI.nSegI.ToString());
#else
		sCmdText.Format("��ע��%s�ζκ�",(char*)m_paraDimSegI.nSegI.ToString());
#endif
	}
	else if(m_eCmdType==CMD_DRAW_LINEPART)
	{	//���Ƹ˼�����ͼ
		if(m_paraLinePart.pLinePart)
		{
			hPart=m_paraLinePart.pLinePart->handle;
#ifdef AFX_TARG_ENU_ENGLISH
			sCmdText.Format("Draw (%s)%s",(char*)m_paraLinePart.pLinePart->GetPartNo(),GetDrawPartName());
#else
			sCmdText.Format("����(%s)%s",(char*)m_paraLinePart.pLinePart->GetPartNo(),GetDrawPartName());
#endif
			if(sPartNo)
				strcpy(sPartNo,m_paraLinePart.pLinePart->GetPartNo());
		}
	}
	else if(m_eCmdType==CMD_DIM_ODDMENT)
	{	//����ͷ
		if(m_paraOddment.pLinePart)
		{
			hPart=m_paraOddment.pLinePart->handle;
#ifdef AFX_TARG_ENU_ENGLISH
			sCmdText.Format("Dimension (%s) rod's oddment",(char*)m_paraOddment.pLinePart->GetPartNo());
#else
			sCmdText.Format("��ע(%s)�˼�����ͷ",(char*)m_paraOddment.pLinePart->GetPartNo());
#endif
			if(sPartNo)
				strcpy(sPartNo,m_paraOddment.pLinePart->GetPartNo());
		}
	}
	else if(m_eCmdType==CMD_DIM_BOLTORFL_INFO)
	{	//˾��ͼ�б�ע�˼���˨��Ϣ������Ϣ
		if(m_paraBoltOrFlInfo.pLinePart)
		{
			hPart=m_paraBoltOrFlInfo.pLinePart->handle;
			if(m_paraBoltOrFlInfo.pLinePart->IsTube())
#ifdef AFX_TARG_ENU_ENGLISH
				sCmdText.Format("Dimension (%s) tube flange information",(char*)m_paraBoltOrFlInfo.pLinePart->GetPartNo());
			else 
				sCmdText.Format("Dimension (%s) rod's bolts information",(char*)m_paraBoltOrFlInfo.pLinePart->GetPartNo());
#else
				sCmdText.Format("��ע(%s)�ֹܷ�����Ϣ",(char*)m_paraBoltOrFlInfo.pLinePart->GetPartNo());
			else 
				sCmdText.Format("��ע(%s)�˼���˨��Ϣ",(char*)m_paraBoltOrFlInfo.pLinePart->GetPartNo());
#endif
			if(sPartNo)
				strcpy(sPartNo,m_paraBoltOrFlInfo.pLinePart->GetPartNo());
		}
	}
	else if(m_eCmdType==CMD_DIM_ANGLEJOINT)
	{	//˾��ͼ�б�ע�Ǹֽ�ͷ��Ϣ
		if(m_paraAngleJoint.pJoint)
		{
			hPart=m_paraAngleJoint.pJoint->Id();
#ifdef AFX_TARG_ENU_ENGLISH
			sCmdText.Format("Dimension (%s) angle joint information",(char*)m_paraAngleJoint.pJoint->GetJointNo());
#else
			sCmdText.Format("��ע(%s)�Ǹֽ�ͷ��Ϣ",(char*)m_paraAngleJoint.pJoint->GetJointNo());
#endif
			if(sPartNo)
				strcpy(sPartNo,m_paraAngleJoint.pJoint->GetJointNo());
		}
	}
	else if(m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)
	{	//�Ŷ����
		if(m_paraDimFootNailSpace.pLinePart)
		{
			hPart=m_paraDimFootNailSpace.pLinePart->handle;
#ifdef AFX_TARG_ENU_ENGLISH
			sCmdText.Format("Dimension (%s) rod's foot nail space",(char*)m_paraDimFootNailSpace.pLinePart->GetPartNo());
#else
			sCmdText.Format("��ע(%s)�˼��Ŷ����",(char*)m_paraDimFootNailSpace.pLinePart->GetPartNo());
#endif
			if(sPartNo)
				strcpy(sPartNo,m_paraDimFootNailSpace.pLinePart->GetPartNo());
		}
	}
	else if(m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
	{	//�����
		if(m_paraDimFillPlateSpace.pLinePart)
		{
			hPart=m_paraDimFillPlateSpace.pLinePart->handle;
#ifdef AFX_TARG_ENU_ENGLISH
			sCmdText.Format("Dimension (%s) group angle's fill plate space",(char*)m_paraDimFillPlateSpace.pLinePart->GetPartNo());
#else
			sCmdText.Format("��ע(%s)��ϽǸ������",(char*)m_paraDimFillPlateSpace.pLinePart->GetPartNo());
#endif
			if(sPartNo)
				strcpy(sPartNo,m_paraDimFillPlateSpace.pLinePart->GetPartNo());
		}
	}
	else if(m_eCmdType==CMD_DIM_SIMPOLY)
	{	//������
		if(m_paraSimPoly.nSimPolyCmdType==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sCmdText.Format("Select pick point to add similar polygon");
		else if(m_paraSimPoly.nSimPolyCmdType==1)
			sCmdText.Format("Select outline point to add similar polygon");
		else if(m_paraSimPoly.nSimPolyCmdType==3)
			sCmdText.Format("Dimension similar polygon by outline point");
		else
			sCmdText.Format("Dimension similar polygon");
#else
			sCmdText.Format("ѡ��ʰȡ�����������");
		else if(m_paraSimPoly.nSimPolyCmdType==1)
			sCmdText.Format("ѡ�����������������");
		else if(m_paraSimPoly.nSimPolyCmdType==3)
			sCmdText.Format("ͨ���������ע������");
		else
			sCmdText.Format("��ע������");
#endif
	}
	else if(m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)
	{	//����Ǹ�׼��
		if(m_paraDimJgZhun.pLineAngle)
		{
			hPart=m_paraDimJgZhun.pLineAngle->handle;
#ifdef AFX_TARG_ENU_ENGLISH
			sCmdText.Format("Dimension (%s) angle special G",(char*)m_paraDimJgZhun.pLineAngle->GetPartNo());
#else
			sCmdText.Format("��ע(%s)�Ǹ�����׼��",(char*)m_paraDimJgZhun.pLineAngle->GetPartNo());
#endif
			if(sPartNo)
				strcpy(sPartNo,m_paraDimJgZhun.pLineAngle->GetPartNo());
		}
	}
	else if(m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
	{	//������˨���
		hPart=m_paraDimLsSpace.spaceDim.hStartLs;
#ifdef AFX_TARG_ENU_ENGLISH
		sCmdText.Format("Dimension bolt special space");
#else
		sCmdText.Format("��ע��˨������");
#endif
		if(sPartNo&&m_paraDimLsSpace.pPlate)
			strcpy(sPartNo,m_paraDimLsSpace.pPlate->GetPartNo());
	}
	else if(m_eCmdType==CMD_DIM_MODELSIZE)
	{	//ģ�ͳߴ�
		if(m_paraDimModelSize.iDimType==DIM_NODE_VSPACE)
#ifdef AFX_TARG_ENU_ENGLISH
			sCmdText.Format("Vertical space between nodes");
		else if(m_paraDimModelSize.iDimType==DIM_HENGDAN_VSPACE)
			sCmdText.Format("Vertical space of cross arm's hanging wire point");
		else if(m_paraDimModelSize.iDimType==DIM_SEG_VSPACE)
			sCmdText.Format("Vertical space of segment");
		else if(m_paraDimModelSize.iDimType==DIM_TOWERHIGH)
			sCmdText.Format("Tower Height");
		else if(m_paraDimModelSize.iDimType==DIM_SLOPE_HSPACE)
			sCmdText.Format("Horizontal space of slope");
		else if(m_paraDimModelSize.iDimType==DIM_HENGDAN_HSPACE)
			sCmdText.Format("Horizontal space of cross arm's hanging wire point");
#else
			sCmdText.Format("�ڼ䴹ֱ����");
		else if(m_paraDimModelSize.iDimType==DIM_HENGDAN_VSPACE)
			sCmdText.Format("�ᵣ���ߵ㴹ֱ����");
		else if(m_paraDimModelSize.iDimType==DIM_SEG_VSPACE)
			sCmdText.Format("�ֶδ�ֱ����");
		else if(m_paraDimModelSize.iDimType==DIM_TOWERHIGH)
			sCmdText.Format("����");
		else if(m_paraDimModelSize.iDimType==DIM_SLOPE_HSPACE)
			sCmdText.Format("���¿���");
		else if(m_paraDimModelSize.iDimType==DIM_HENGDAN_HSPACE)
			sCmdText.Format("�ᵣ���ߵ�ˮƽ����");
#endif
	}
	else if(m_eCmdType==CMD_DIM_MAPSIZE)
	{	//ͼ��ߴ�

	}
	else if(m_eCmdType==CMD_DRAW_ASSIST_LINE)
	{	//������
		if(m_paraDrawAssistLine.nStyle==1)
#ifdef AFX_TARG_ENU_ENGLISH
			sCmdText.Format("Draw assist line");
		else if(m_paraDrawAssistLine.nStyle==2)
			sCmdText.Format("Draw assist bending line");
#else
			sCmdText.Format("���Ƹ�������");
		else if(m_paraDrawAssistLine.nStyle==2)
			sCmdText.Format("���Ƹ���������");
#endif
	}
	if(sCmdId&&(m_eCmdType==CMD_DIM_ANGLEJOINT||hPart>0X20))
		sprintf(sCmdId,"0X%X",hPart);
	if(sHelp)
		strcpy(sHelp,sCmdText);
	return hPart;
}

//-1.δ֪ 0.ƽ�̻��� 1.��ֱ���� 2.������� 3.չ������ wht 11-07-22
int CDrawingCommand::GetDrawType(char *sDrawType/*=NULL*/)
{
	if(sDrawType)
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(sDrawType,"Unknown");
#else
		strcpy(sDrawType,"δ֪");
#endif
	if(m_eCmdType!=CMD_DRAW_PART)
		return -1;
	if(m_paraPart.pPart==NULL)
		m_paraPart.pPart=Ta.FromPartHandle(m_paraPart.hPart);
	if(m_paraPart.pPart==NULL)
		return -1;
	int h0v1e2=-1;
	if(m_paraPart.pPart->GetClassTypeId()==CLS_BOLT)
		h0v1e2=m_paraPart.para.bolt.front0_side1;
	else 
		h0v1e2=m_paraPart.cDrawFlag-'0';
	if(sDrawType)
	{
		if(h0v1e2==0)
		{
			if(m_paraPart.pPart->GetClassTypeId()==CLS_BOLT)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sDrawType,"Vertical");
			else
				strcpy(sDrawType,"Horizontal");
#else
				strcpy(sDrawType,"����");
			else
				strcpy(sDrawType,"ˮƽ");
#endif
		}
		else if(h0v1e2==1)
		{
			if(m_paraPart.pPart->GetClassTypeId()==CLS_BOLT)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sDrawType,"Horizontal");
			else
				strcpy(sDrawType,"Vertical");
#else
				strcpy(sDrawType,"ƽ��");
			else
				strcpy(sDrawType,"��ֱ");
#endif
		}
		else if(h0v1e2==2)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sDrawType,"End Plane");
#else
			strcpy(sDrawType,"����");
#endif
	}
	return h0v1e2;
}

CString CDrawingCommand::GetDrawPartName()
{
#ifdef AFX_TARG_ENU_ENGLISH
	CString sPartName="Part";
#else
	CString sPartName="����";
#endif
	CLDSPart *pPart=NULL;
	if(m_eCmdType==CMD_DRAW_PART&&m_paraPart.pPart)
		pPart=m_paraPart.pPart;
	else if(m_eCmdType==CMD_DIM_PARTSPEC&&m_paraPartSpec.pObj)
	{
		if(m_paraPartSpec.pObj->IsPart())
			pPart=(CLDSPart*)m_paraPartSpec.pObj;
		else if(m_paraPartSpec.pObj->IsNode())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sPartName="Node";
#else
			sPartName="�ڵ�";
#endif
			return sPartName;
		}
	}
	else if(m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		sPartName="Foot Nail";
#else
		sPartName="�Ŷ�";
#endif
		return sPartName;
	}
	else if(m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		sPartName="Fill Plate";
#else
		sPartName="���";
#endif
		return sPartName;
	}
	else if(m_eCmdType==CMD_DRAW_ASSIST_LINE)
	{
		sPartName="";
		if(m_paraDrawAssistLine.nStyle==1)
#ifdef AFX_TARG_ENU_ENGLISH
			sPartName="Assist bolt line";
		else if(m_paraDrawAssistLine.nStyle==2)
			sPartName="Assist bending line";
#else
			sPartName="��������";
		else if(m_paraDrawAssistLine.nStyle==2)
			sPartName="����������";
#endif
		return sPartName;
	}
	else if(m_eCmdType==CMD_DIM_ANGLEJOINT&&m_paraAngleJoint.pJoint)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		sPartName="Angle joint";
#else
		sPartName="�Ǹֽ�ͷ";
#endif
		return sPartName;
	}
	else if(m_eCmdType==CMD_DRAW_LINEPART&&m_paraLinePart.pLinePart)
		pPart=m_paraLinePart.pLinePart;
	else if(m_eCmdType==CMD_DIM_ODDMENT&&m_paraOddment.pLinePart)
		pPart=m_paraOddment.pLinePart;
	else if(m_eCmdType==CMD_DIM_BOLTORFL_INFO&&m_paraBoltOrFlInfo.pLinePart)
		pPart=m_paraBoltOrFlInfo.pLinePart;
	else if(m_eCmdType==CMD_DIM_FOOTNAIL_SPACE&&m_paraDimFootNailSpace.pLinePart)
		pPart=m_paraDimFootNailSpace.pLinePart;
	else if(m_eCmdType==CMD_DIM_FILLPLATE_SPACE&&m_paraDimFillPlateSpace.pLinePart)
		pPart=m_paraDimFillPlateSpace.pLinePart;
	else if(m_eCmdType==CMD_DIM_SPECIAL_LSSPACE&&m_paraDimLsSpace.pPlate)
		pPart=m_paraDimLsSpace.pPlate;
	else if(m_eCmdType==CMD_DIM_SPECIAL_JGZHUN&&m_paraDimJgZhun.pLineAngle)
		pPart=m_paraDimJgZhun.pLineAngle;
	else if(m_eCmdType==CMD_DIM_BOLTPAD&&m_paraBoltPad.pBolt)
		pPart=m_paraBoltPad.pBolt;
	else if(m_eCmdType==CMD_DIM_BOLTINFO)
		pPart=m_paraBoltGroupInfo.lsGroupDim.GetDatumBolt();
	if(pPart)
	{
		if(pPart->IsLinePart())
		{
			if(pPart->GetClassTypeId()==CLS_LINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
				sPartName.Format("Angle");
			else if(pPart->GetClassTypeId()==CLS_LINETUBE)
				sPartName.Format("Tube");
			else if(pPart->GetClassTypeId()==CLS_LINESLOT)
				sPartName.Format("Slot");
			else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
				sPartName.Format("Flat");
			else 
				sPartName.Format("Rod");
#else
				sPartName.Format("�Ǹ�");
			else if(pPart->GetClassTypeId()==CLS_LINETUBE)
				sPartName.Format("�ֹ�");
			else if(pPart->GetClassTypeId()==CLS_LINESLOT)
				sPartName.Format("�۸�");
			else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
				sPartName.Format("���");
			else 
				sPartName.Format("�˼�");
#endif
		}
		else if(pPart->IsArcPart())
		{
			if(pPart->GetClassTypeId()==CLS_ARCANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
				sPartName.Format("Arc Angle");
			else if(pPart->GetClassTypeId()==CLS_ARCSLOT)
				sPartName.Format("Arc Slot");
			else if(pPart->GetClassTypeId()==CLS_ARCFLAT)
				sPartName.Format("Arc Flat");
			else 
				sPartName.Format("Arc Rod");
#else
				sPartName.Format("���νǸ�");
			else if(pPart->GetClassTypeId()==CLS_ARCSLOT)
				sPartName.Format("���β۸�");
			else if(pPart->GetClassTypeId()==CLS_ARCFLAT)
				sPartName.Format("���α��");
			else 
				sPartName.Format("���ι���");
#endif
		}
		else if(pPart->GetClassTypeId()==CLS_PLATE)
#ifdef AFX_TARG_ENU_ENGLISH
			sPartName.Format("Plate");
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			sPartName.Format("Parmetric Plate");
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
			if(pParamPlate->m_iParamType==TYPE_FL)
				sPartName.Format("FL");
			else if(pParamPlate->m_iParamType==TYPE_FLD)
				sPartName.Format("Relative Weld FL");
			else if(pParamPlate->m_iParamType==TYPE_FLP)
				sPartName.Format("Plane Weld FL");
			else if(pParamPlate->m_iParamType==TYPE_FLG)
				sPartName.Format("Rigid FL");
			else if(pParamPlate->m_iParamType==TYPE_FLR)
				sPartName.Format("Flexible FL");
			else if(pParamPlate->m_iParamType==TYPE_ROLLEND)
				sPartName.Format("Slot Insert Plate");
			else if(pParamPlate->m_iParamType==TYPE_UEND)
				sPartName.Format("U Insert Plate");
			else if(pParamPlate->m_iParamType==TYPE_XEND)
				sPartName.Format("Cross Insert Plate");
			else if(pParamPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
				sPartName.Format("Water Shielded Plate");
			else if(pParamPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
				sPartName.Format("Angle Rib Plate");
			else if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
				sPartName.Format("Circular Over Plate");
			else if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
				sPartName.Format("Circular Rib Plate");
			else if(pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
				sPartName.Format("Across Linking Plate");
			else if(pParamPlate->m_iParamType==TYPE_ELBOWPLATE)
				sPartName.Format("Elbow Plate");
#else
			sPartName.Format("�ְ�");
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			sPartName.Format("��������");
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
			if(pParamPlate->m_iParamType==TYPE_FL)
				sPartName.Format("����");
			else if(pParamPlate->m_iParamType==TYPE_FLD)
				sPartName.Format("�Ժ�����");
			else if(pParamPlate->m_iParamType==TYPE_FLP)
				sPartName.Format("ƽ������");
			else if(pParamPlate->m_iParamType==TYPE_FLG)
				sPartName.Format("���Է���");
			else if(pParamPlate->m_iParamType==TYPE_FLR)
				sPartName.Format("���Է���");
			else if(pParamPlate->m_iParamType==TYPE_ROLLEND)
				sPartName.Format("���Ͳ��");
			else if(pParamPlate->m_iParamType==TYPE_UEND)
				sPartName.Format("U�Ͳ��");
			else if(pParamPlate->m_iParamType==TYPE_XEND)
				sPartName.Format("ʮ�ֲ��");
			else if(pParamPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
				sPartName.Format("��ˮ��");
			else if(pParamPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
				sPartName.Format("�Ǹ��߰�");
			else if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
				sPartName.Format("������");
			else if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
				sPartName.Format("�����߰�");
			else if(pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
				sPartName.Format("��Խ����");
			else if(pParamPlate->m_iParamType==TYPE_ELBOWPLATE)
				sPartName.Format("���");
#endif
		}
		else if(pPart->GetClassTypeId()==CLS_BOLT)
		{
			if(pPart->m_bVirtualPart)
#ifdef AFX_TARG_ENU_ENGLISH
				sPartName="Bolt Hole";
			else
				sPartName="Bolt";
#else
				sPartName="��˨��";
			else
				sPartName="��˨";
#endif
		}
		else if(pPart->GetClassTypeId()==CLS_SPHERE)
#ifdef AFX_TARG_ENU_ENGLISH
			sPartName.Format("Sphere");
		else 
			sPartName.Format("Other part");
#else
			sPartName.Format("����");
		else 
			sPartName.Format("��������");
#endif
	}
	return sPartName;
}
//�ɻ�ͼ����ϵ��ƽ��ƫ��������õ��µ�����ϵ
BOOL CDrawingCommand::GetDCS(CDrawCoordTrans &dcs)
{
	if(m_pDrawingCom)
	{
		UCS_STRU cs=m_pDrawingCom->GetDCS();
		if(m_pDrawingCom->m_eClassType==DRAW_COM_PARTVIEW&&m_eCmdType==CMD_DRAW_PART)
		{	//����ͼ,ֱ�ӵ�������ԭ��λ�ü��� wht 11-06-29
			cs.origin+=m_pDrawingCom->drawing_offset;
		}
		else if(m_pDrawingCom->m_eClassType==DRAW_COM_STDVIEW&&m_eCmdType==CMD_DRAW_JG_JOINT)
		{
			cs.origin+=m_pDrawingCom->drawing_offset;
		}
		else
		{	//���Ƚ�ƫ��������������Ȼ����ת������ͼ����ϵ�£�����������ԭ��λ��
			dcs.m_fSpreadAngle=0;
			if(m_pDrawingCom->m_pAttachView!=NULL)
			{	//����չ������Ϣ
				long hPart=0;
				if(m_eCmdType==CMD_DRAW_PART)
					hPart=m_paraPart.hPart;
				else if(m_eCmdType==CMD_DRAW_LINEPART)
					hPart=m_paraLinePart.hLinePart;
				CDrawingAssistant *pAssist=m_pDrawingCom->m_pAttachView->GetDrawingAssistByHandle(hPart,GetSegI());
				if(pAssist)
				{
					pAssist->GetRotateAxisAndAngle(cs.axis_z,dcs.m_xSpreadLine,dcs.m_fSpreadAngle);
					dcs.m_xSpreadPlaneNorm=pAssist->m_xFaceNorm.vector;
				}
			}
			f3dPoint origin=m_pDrawingCom->drawing_offset/m_pDrawingCom->m_fScale;
			vector_trans(origin,cs,TRUE);
			cs.origin-=origin;
		}
		dcs.SetCS(cs);
		dcs.m_fDrawScale=1.0/m_pDrawingCom->m_fScale;
		return TRUE;
	}
	return FALSE;
}

//��עģ�ͳߴ�
static int compare_pt(const f3dPoint &pt1,const f3dPoint &pt2)
{
	if(pt1.z>pt2.z)
		return 1;
	else if(pt1.z<pt2.z)
		return -1;
	else 
		return 0;
}
void DimModelSize(CDrawingCommand *pCommand,UCS_STRU dcs,double draw_scale)
{
	int dimNodeCount=pCommand->m_paraDimModelSize.listDimNodes.GetNodeNum();
	if(pCommand->m_eCmdType!=CMD_DIM_MODELSIZE||dimNodeCount<1||(pCommand->m_paraDimModelSize.iDimType!=5&&dimNodeCount<2))
		return;
	if(pCommand->m_paraDimModelSize.iDimType<3
		&&pCommand->m_paraDimModelSize.listDimNodes.GetNodeNum()<=2)
		return; //ֻ�б�ע����ʱ����nodeSet��ֻ���������ݣ���ֹ��ע���ȸ߶�ʱ�ظ�
	//����ͼ��
	GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
	//��עģ�ͳߴ�
	CLDSNode* pNode=NULL;
	double fMinX=pCommand->m_paraDimModelSize.fMinX;
	double fDimDistance=pCommand->m_paraDimModelSize.fDimDistance;
	double fSpace=pCommand->m_paraDimModelSize.fSpace;
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	if(pCommand->m_paraDimModelSize.iDimType==CLineView::DIM_SHANK_SPACE_XY)
	{	//ˮƽ�¶ο��ڳߴ��ע
		CDrawingCommand::DIMNODE* pFromDimNode=pCommand->m_paraDimModelSize.listDimNodes.GetFirst();
		CDrawingCommand::DIMNODE* pToDimNode=pCommand->m_paraDimModelSize.listDimNodes.GetNext();
		f3dPoint start,end;
		if(pFromDimNode->hNode>0x20)
			start=pFromDimNode->pNode->xOriginalPos;
		if(pToDimNode->hNode>0x20)
			end=pToDimNode->pNode->Position(false);
		if(pFromDimNode->hNode<=0x20)
			start.Set(0,0,end.z);
		if(pToDimNode->hNode<=0x20)
			end.Set(0,0,start.z);

		f3dPoint dim_pos((start.x+end.x)*0.5,0,start.z+fSpace+fDimDistance);
		f3dPoint dim_start(start.x,start.y,start.z+fSpace);
		f3dPoint dim_end(end.x,end.y,end.z+fSpace);
		//ת������ͼ����ϵ��
		coord_trans(dim_pos,dcs,FALSE);
		coord_trans(dim_start,dcs,FALSE);
		coord_trans(dim_end,dcs,FALSE);
		//ʹ��ת������ͼ����ϵ�µ������󿪿�ֵ(���������ͼ�޷���ȡ��ȷ�Ŀ���ֵ) wht 11-06-15
		char dim_text[20]="";
		double dist=fabs(dim_start.x-dim_end.x);
		sprintf(dim_text,"%.0f",dist);
		if(pCommand&&pCommand->m_pDrawingCom&&pCommand->m_pDrawingCom->ParentDrawing()
			&&pCommand->m_pDrawingCom->ParentDrawing()->m_nSegI.iSeg>=0)
		{	//�ֶνṹͼ����ͼ���ڳߴ��ע,�Ǹ����ߵ����ߵľ��� wht 11-07-27
			CLDSPart *pPart=NULL;
			CLDSLineAngle *pAngleArr[4]={NULL};	//[0],[1]Ϊ��һ������б�� [2],[3]Ϊ�ڶ�������б��
			CDrawingComponent *pCom=pCommand->m_pDrawingCom;
			for(pPart=pCom->GetFirstPart();pPart;pPart=pCom->GetNextPart())
			{
				if(pPart->GetClassTypeId()!=CLS_LINEANGLE)
					continue;	//�ǽǸ�
				char cType=pPart->Layer(1);
				if(toupper(cType)=='H')
					continue;	//��ͳ�ƺ��
				CLDSLineAngle *pAngle=(CLDSLineAngle*)pPart;
				if(pAngle->pStart==NULL||pAngle->pEnd==NULL)
					continue;
				if(pAngle->pStart==pFromDimNode->pNode||pAngle->pEnd==pFromDimNode->pNode)
				{
					if(toupper(cType)=='Z')
						pAngleArr[0]=pAngle;
					else 
						pAngleArr[1]=pAngle;
				}
				else if(pAngle->pStart==pToDimNode->pNode||pAngle->pEnd==pToDimNode->pNode)
				{
					if(toupper(cType)=='Z')
						pAngleArr[2]=pAngle;
					else 
						pAngleArr[3]=pAngle;
				}
				if(pAngleArr[0]&&pAngleArr[1]&&pAngleArr[2]&&pAngleArr[3])
					break;
			}
			if(pAngleArr[0]&&pAngleArr[1]&&pAngleArr[2]&&pAngleArr[3])
			{
				int i=0;
				f3dLine line_arr[4];
				for(i=0;i<4;i++)
				{
					AcDbEntity *pEnt=NULL;
					if(pAngleArr[i]&&pAngleArr[i]->group_father_jg_h>0x20)	//������ϽǸ��ӽǸ�Ӧ������ϽǸ� wjh-2011.8.27
						pEnt=pCom->FindCADEntByHandle(pAngleArr[i]->group_father_jg_h);
					if(pEnt==NULL)
						pEnt=pCom->FindCADEntByHandle(pAngleArr[i]->handle);
					if(pEnt==NULL)
						break;
					if(pEnt->isKindOf(AcDbLine::desc()))
					{
						AcDbLine *pLine=(AcDbLine*)pEnt;
						Cpy_Pnt(line_arr[i].startPt,pLine->startPoint());
						Cpy_Pnt(line_arr[i].endPt,pLine->endPoint());
					}
					else if(pEnt->isKindOf(AcDbPolyline::desc()))
					{
						AcDbPolyline *pPolyLine=(AcDbPolyline*)pEnt;
						AcGePoint3d start,end;
						pPolyLine->getPointAt(0,start);
						pPolyLine->getPointAt(1,end);
						Cpy_Pnt(line_arr[i].startPt,start);
						Cpy_Pnt(line_arr[i].endPt,end);
					}
					else 
						break;
				}
				if(i==4)
				{
					f3dPoint ptArr[2];
					if(Int3dll(line_arr[0],line_arr[1],ptArr[0])&&Int3dll(line_arr[2],line_arr[3],ptArr[1]))
					{
						double dist1=DISTANCE(ptArr[0],ptArr[1])/draw_scale;
						if(fabs(dist1-dist)<100)	
							sprintf(dim_text,"%.0f",dist1);
					}
				}
			}
		}
		//����������
		dim_pos = dim_pos*draw_scale;
		dim_start = dim_start*draw_scale;
		dim_end = dim_end*draw_scale;
		DimSize(pBlockTableRecord,dim_start,dim_end,dim_pos,
			dim_text,DimStyleTable::dimStyle.dimStyleId,2,sys.fonts.chief.fDimTextSize);
	}
	else if(pCommand->m_paraDimModelSize.iDimType==CLineView::DIM_GUAXIAN_SPACE_XY)
	{	//ˮƽ�ᵣ���ߴ��ע
		char dim_text[20]="";
		CDrawingCommand::DIMNODE* pFirstDimNode=pCommand->m_paraDimModelSize.listDimNodes.GetFirst();
		while(pFirstDimNode!=NULL&&pFirstDimNode->pNode!=NULL&&fabs(pFirstDimNode->pNode->xOriginalPos*dcs.axis_x)<EPS)
			pFirstDimNode=pCommand->m_paraDimModelSize.listDimNodes.GetNext();
		if(pFirstDimNode==NULL)
		{
			pBlockTableRecord->close();
			return;
		}
		f3dPoint end=pFirstDimNode->pNode->xPreliftPos;
		f3dPoint prev_point=end;
		sprintf(dim_text,"%.0f",fabs(end.x));
		f3dPoint dim_start(0,0,end.z+fSpace);
		f3dPoint dim_end(end.x,end.y,end.z+fSpace);
		//ת������ͼ����ϵ��
		coord_trans(dim_start,dcs,FALSE);
		coord_trans(dim_end,dcs,FALSE);
		//����������
		dim_start = dim_start*draw_scale;
		dim_end = dim_end*draw_scale;
		f3dPoint dim_pos((dim_start.x+dim_end.x)*0.5,dim_end.y+dcs.axis_y.z*fDimDistance*draw_scale,0);
		DimSize(pBlockTableRecord,dim_start,dim_end,dim_pos,
			dim_text,DimStyleTable::dimStyle.dimStyleId,2,sys.fonts.chief.fDimTextSize);
		CDrawingCommand::DIMNODE* pDimNode=NULL;
		for(pDimNode=pCommand->m_paraDimModelSize.listDimNodes.GetNext();pDimNode;pDimNode=pCommand->m_paraDimModelSize.listDimNodes.GetNext())
		{
			pNode=pDimNode->pNode;
			end=pNode->xPreliftPos;
			double distance=fabs(dcs.axis_x*(end-prev_point));
			prev_point=end;
			sprintf(dim_text,"%.0f",distance);
			dim_start=dim_end;
			dim_end.Set(end.x,end.y,end.z+fSpace);
			//ת������ͼ����ϵ��
			coord_trans(dim_end,dcs,FALSE);
			dim_end = dim_end*draw_scale;
			dim_pos.Set((dim_start.x+dim_end.x)*0.5,dim_end.y+dcs.axis_y.z*fDimDistance*draw_scale,0);
			DimSize(pBlockTableRecord,dim_start,dim_end,dim_pos,
				dim_text,DimStyleTable::dimStyle.dimStyleId,2,sys.fonts.chief.fDimTextSize);
		}
	}
	else
	{	//��ֱ��ע
		CArray<f3dPoint,f3dPoint&>ptArr;
		for(CDrawingCommand::DIMNODE* pDimNode=pCommand->m_paraDimModelSize.listDimNodes.GetFirst();pDimNode;pDimNode=pCommand->m_paraDimModelSize.listDimNodes.GetNext())
			ptArr.Add(pDimNode->pNode->Position(false));	//��ʼ��λ������
		CQuickSort<f3dPoint>::QuickSort(ptArr.GetData(),(long)ptArr.GetSize(),compare_pt);//����
		for (int i=0;i<ptArr.GetSize()-1;i++)
		{
			double dist = fabs(ptArr[i].z - ptArr[i+1].z);
			if(fabs(dist)<EPS)
				continue;//�ᵣ��Ԥ��ʱ��ɴ�����0�����б�ע
			char dim_text[20];
			sprintf(dim_text,"%.0f",dist);
			f3dPoint dim_pos(fMinX-fDimDistance,0,(ptArr[i].z+ptArr[i+1].z)*0.5);
			f3dPoint dim_start(fMinX-fSpace,ptArr[i].y,ptArr[i].z);
			f3dPoint dim_end(fMinX-fSpace,ptArr[i+1].y,ptArr[i+1].z);
			//ת������ͼ����ϵ��
			coord_trans(dim_pos,dcs,FALSE);
			coord_trans(dim_start,dcs,FALSE);
			coord_trans(dim_end,dcs,FALSE);
			//����������
			dim_pos = dim_pos*draw_scale;
			dim_start = dim_start*draw_scale;
			dim_end = dim_end*draw_scale;
			//��ע�ߴ�
			DimSize(pBlockTableRecord,dim_start,dim_end,dim_pos,
				dim_text,DimStyleTable::dimStyle.dimStyleId,1,sys.fonts.chief.fDimTextSize);
		}
	}
	pBlockTableRecord->close();
}
//From DrawUnit.cpp
void DimSingleLsSpace(AcDbBlockTableRecord *pBlockTableRecord,CLsSpaceDimUnit *pSpaceDim,
					  double fDimDist=10,double fSpace=3,BOOL bInside=TRUE);
//From DimPartSpec.cpp
CDimPartSpec * AddSinglePartSpec(AcDbObjectId entId,double fDimPosCoef,BOOL bDimByAnalysisMode=FALSE,
								 BOOL bDimPoleSpec=TRUE,BOOL bDrawRefLine=FALSE);
BOOL DimSinglePoleOddment(AcDbBlockTableRecord *pBlockTableRecord,CDrawingCommand *pCmd);
//From DimSimPoly.cpp
void AddDimSimPolyCmd(CDrawingComponent *pDrawingCom,ATOM_LIST<CDrawingCommand*> *pCmdList=NULL);
void ExecuteDimSimPolyCmdByPickPt(CDrawingCommand *pCmd);
void ExecuteDimSimPolyCmdByVertextList(CDrawingCommand *pCmd);
//From SumMap.cpp
void DimSegI(CDrawingCommand *pCommand);
//From PaperPatternDrawing.cpp
AcDbObjectId NewBoltCircle(AcDbBlockTableRecord *pBlockTableRecord,f3dCircle Cir,BOOL bFootNail=FALSE);
//��ע�����˼��ϵĽŶ���� From DimSize.cpp
void DimSinglePoleFootNailSpace(AcDbBlockTableRecord *pBlockTableRecord,AcDbObjectId entId,
								double fPara1=10,double fPara2=5,BOOL bSelectBasePt=TRUE,
								double draw_scale=0.05,UCS_STRU *pDrawUCS=NULL);
void DimSingleGroupLineAngleFillPlateSpace(AcDbBlockTableRecord *pBlockTableRecord,AcDbObjectId entId,
								double fPara1=10,double fPara2=5,BOOL bSelectBasePt=TRUE,
								double draw_scale=0.05,UCS_STRU *pDrawUCS=NULL);
//
f2dRect GetProcessCardDrawingRect(int iProcessCardType=0);
static void trans_pos(f3dPoint &pt,f3dPoint origin)
{
	f3dPoint temp=pt;
	pt.x=-temp.y;
	pt.y=temp.x;
	pt+=origin;
}

static void trans_pos(f3dLine &line,f3dPoint origin)
{
	f3dPoint temp=line.startPt;
	line.startPt.x=-temp.y;
	line.startPt.y=temp.x;
	line.startPt+=origin;
	temp=line.endPt;
	line.endPt.x=-temp.y;
	line.endPt.y=temp.x;
	line.endPt+=origin;
}
//���ƽǸ������ͼ
static BOOL DrawLineAngleDrawing(CDrawingCommand *pCommand)
{
	if(pCommand==NULL)
		return FALSE;
	if(pCommand->m_pDrawingCom==NULL)
		return FALSE;
	if(pCommand->m_pDrawingCom->m_eClassType!=DRAW_COM_PARTVIEW
		||pCommand->m_paraPart.pPart==NULL)
		return FALSE;
	if(pCommand->m_paraPart.pPart->GetClassTypeId()!=CLS_LINEANGLE)
		return FALSE;
	CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pCommand->m_paraPart.pPart;
	CLineAngleDrawing xJgDraw;
	xJgDraw.dimStyleFlag.SetFlagDword(CLineAngleDrawing::SpecifyDrawscale); //ָ����ͼ����
	xJgDraw.dimStyleFlag.SetFlagDword(CLineAngleDrawing::WrapAngleDrawing);	//���ư�����ͼ
	xJgDraw.SetLengthScaleCoef(pCommand->m_pDrawingCom->m_fScale);
	xJgDraw.SetVertScaleCoef(pCommand->m_pDrawingCom->m_fScale);
	f2dRect draw_rect=GetProcessCardDrawingRect();
	xJgDraw.CreateLineAngleDrawing(pLineAngle,draw_rect.Width(),
		draw_rect.Height(),sys.fonts.segment.fDimTextSize);
	int i,n;
	f3dLine line;
	f3dCircle Cir;
	n = xJgDraw.GetLineCount();
	UCS_STRU dcs;
	CDrawCoordTrans dct;	//������ͼ�в���Ҫչ���������ʿ�ֱ��������ϵת����wjh-2011.8.23
	if(!pCommand->GetDCS(dct))
		return FALSE;
	dcs=dct.cs;
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	for(i=0;i<n;i++)
	{
		xJgDraw.GetLineAt(line,i);
		trans_pos(line,dcs.origin);
		CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
	}
	for(i=0;i<5;i++)
	{
		xJgDraw.GetZEdge(line,i);
		trans_pos(line,dcs.origin);
		if(line.feature==0)
		{
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
		}
		else
		{
			GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
			CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		}
	}
	n = xJgDraw.GetXLsCount();
	CXhChar16 sM12("M12"),sM16("M16"),sM18("M18"),sM20("M20"),sM22("M22"),sM24("M24");
	if(sys.part_map.angle.bJgUseSimpleLsMap)
	{
		sM12.Copy("M12_Jg");
		sM16.Copy("M16_Jg");
		sM18.Copy("M18_Jg");
		sM20.Copy("M20_Jg");
		sM22.Copy("M22_Jg");
		sM24.Copy("M24_Jg");
	}
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	for(i=0;i<n;i++)
	{
		xJgDraw.GetXLsCircle(Cir,i);
		trans_pos(Cir.centre,dcs.origin);
		if(Cir.radius<EPS)
			continue;	//����˨��������,�������ϵ������
		BOOL bRetCode=FALSE;
		if(fabs(2*Cir.radius-12)<EPS)	//��3mm��˨ֱ������
			bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM12,1.2,0);
		else if(fabs(2*Cir.radius-16)<EPS)
			bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM16,1.2,0);
		else if(fabs(2*Cir.radius-20)<EPS)
			bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM20,1.2,0);
		else if(fabs(2*Cir.radius-22)<EPS)
			bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM22,1.2,0);
		else if(fabs(2*Cir.radius-24)<EPS)
			bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM24,1.2,0);
		else
		{
			CXhChar16 ls_str;
			ls_str.Printf("M%d",ftol(2*Cir.radius));
			bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,ls_str,1.2,0);
		}
		if(!bRetCode&&fabs(Cir.radius)>EPS2)
		{	//������˨
			Cir.radius=1;
			NewBoltCircle(pBlockTableRecord,Cir);
		}
	}
	n = xJgDraw.GetYLsCount();
	for(i=0;i<n;i++)
	{
		xJgDraw.GetYLsCircle(Cir,i);
		trans_pos(Cir.centre,dcs.origin);
		if(Cir.radius<EPS)
			continue;	//����˨��������,�������ϵ������
		BOOL bRetCode=FALSE;
		if(fabs(2*Cir.radius-12)<EPS)
			bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM12,1.2,0);
		else if(fabs(2*Cir.radius-16)<EPS)
			bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM16,1.2,0);
		else if(fabs(2*Cir.radius-20)<EPS)
			bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM20,1.2,0);
		else if(fabs(2*Cir.radius-22)<EPS)
			bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM22,1.2,0);
		else if(fabs(2*Cir.radius-24)<EPS)
			bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM24,1.2,0);
		else
		{
			CXhChar16 ls_str;
			ls_str.Printf("M%d",ftol(2*Cir.radius));
			bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,ls_str,1.2,0);
		}
		if(!bRetCode&&fabs(Cir.radius)>EPS2)
		{	//������˨
			Cir.radius=1;
			NewBoltCircle(pBlockTableRecord,Cir);
		}
	}
	//��ע��˨��Ծ���
	CSizeTextDim jg_dim;
	n = xJgDraw.GetDimCount();
	CXhChar200 sDimText;
	AcDbObjectId dimStyleId;
	dimStyleId = DimStyleTable::dimStyle.dimStyleId;
	GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
	if(sys.general_set.iDimPrecision==0)		//��ȷ��1mm
		CSizeTextDim::PRECISION=1.0;
	else if(sys.general_set.iDimPrecision==1)	//��ȷ��0.5mm
		CSizeTextDim::PRECISION=0.5;
	else if(sys.general_set.iDimPrecision==2)	//��ȷ��0.1mm
		CSizeTextDim::PRECISION=0.1;
	f3dPoint min_pos,max_pos,dim_pos;
	int nXWingDim=0,nYWingDim=0;
	for(i=0;i<n;i++)
	{
		xJgDraw.GetDimAt(i,&jg_dim);
		if(jg_dim.dimPos.y>0)
			nYWingDim++;	//Y֫��ע��
		else 
			nXWingDim++;	//X֫��ע��
	}
	for(i=0;i<n;i++)
	{
		xJgDraw.GetDimAt(i,&jg_dim);
		if(nXWingDim>0)
		{	//���ȱ�עX֫��˨���
			if(jg_dim.dimPos.y>0)
				continue;	//����עY֫��˨���
		}
		if(jg_dim.dist==0)
			continue;
		jg_dim.dimPos+=jg_dim.origin;
		jg_dim.dimStartPos+=jg_dim.origin;
		jg_dim.dimEndPos+=jg_dim.origin;
		trans_pos(jg_dim.dimPos,dcs.origin);
		trans_pos(jg_dim.dimStartPos,dcs.origin);
		trans_pos(jg_dim.dimEndPos,dcs.origin);
		if(nXWingDim>0)
		{	//��עX֫���ʱ��Ҫ������עλ��
			jg_dim.dimPos.x+=sys.fonts.segment.fDimTextSize;
			jg_dim.dimStartPos.x+=sys.fonts.segment.fDimTextSize;
			jg_dim.dimEndPos.x+=sys.fonts.segment.fDimTextSize;
		}
		if(i==0)
		{
			dim_pos=jg_dim.dimPos;
			min_pos.x=max_pos.x=jg_dim.dimStartPos.x;
			min_pos.y=min(jg_dim.dimStartPos.y,jg_dim.dimEndPos.y);
			max_pos.y=max(jg_dim.dimStartPos.y,jg_dim.dimEndPos.y);
		}
		else 
		{
			min_pos.y=min(min_pos.y,jg_dim.dimStartPos.y);
			min_pos.y=min(min_pos.y,jg_dim.dimEndPos.y);
			max_pos.y=max(max_pos.y,jg_dim.dimStartPos.y);
			max_pos.y=max(max_pos.y,jg_dim.dimEndPos.y);
		}
		if(sys.part_map.angle.iLsSpaceDimStyle>=0&&jg_dim.nHits>0)
		{
			DimAngleSize(pBlockTableRecord,
				jg_dim.dimStartPos,jg_dim.dimEndPos,
				jg_dim.dimPos,jg_dim.DimText(),dimStyleId,jg_dim.angle+0.5*Pi);
		}
	}
	if(nXWingDim>0||nYWingDim>0)
	{	//��ע�ܳ���
		int nFlag=1;
		if(nXWingDim==0)
			nFlag=-1;
		dim_pos.y=0.5*(min_pos.y,max_pos.y);
		dim_pos.x+=2*sys.fonts.segment.fDimTextSize*nFlag;
		min_pos.x+=2*sys.fonts.segment.fDimTextSize*nFlag;
		max_pos.x+=2*sys.fonts.segment.fDimTextSize*nFlag;
		CXhChar16 dim_txt("%.f",pLineAngle->GetLength());
		DimAngleSize(pBlockTableRecord,max_pos,min_pos,dim_pos
			,dim_txt,dimStyleId,0.5*Pi);
	}
	CZhunXianTextDim zhun_dim;
	n = xJgDraw.GetZhunDimCount();
	for(i=0;i<n;i++)
	{
		xJgDraw.GetZhunDimAt(i,&zhun_dim);
		zhun_dim.dimStartPos+=zhun_dim.origin;
		zhun_dim.dimEndPos+=zhun_dim.origin;
		zhun_dim.dimPos+=zhun_dim.origin;
		if(i==0)
		{
			dim_pos=zhun_dim.dimPos;
			min_pos.x=max_pos.x=jg_dim.dimStartPos.x;
			min_pos.y=min(fabs(zhun_dim.dimStartPos.y),fabs(zhun_dim.dimEndPos.y));
			max_pos.y=max(fabs(zhun_dim.dimStartPos.y),fabs(zhun_dim.dimEndPos.y));
		}
		else 
		{
			dim_pos.x=max(dim_pos.x,zhun_dim.dimPos.x);
			min_pos.y=min(min_pos.y,fabs(zhun_dim.dimStartPos.y));
			min_pos.y=min(min_pos.y,fabs(zhun_dim.dimEndPos.y));
			max_pos.y=max(max_pos.y,fabs(zhun_dim.dimStartPos.y));
			max_pos.y=max(max_pos.y,fabs(zhun_dim.dimEndPos.y));
		}
		trans_pos(zhun_dim.dimPos,dcs.origin);
		trans_pos(zhun_dim.dimStartPos,dcs.origin);
		trans_pos(zhun_dim.dimEndPos,dcs.origin);
		sDimText.Printf("%.0f",zhun_dim.dist);
		if(zhun_dim.dimStartPos!=zhun_dim.dimEndPos)
		{
			if(zhun_dim.bDimOnLeftSide)	//�²��ľ��ע
				zhun_dim.dimPos.y-=(1+sys.dim_map.fDimGap+sys.fonts.segment.fDimTextSize);
			DimAngleSize(pBlockTableRecord,zhun_dim.dimStartPos,zhun_dim.dimEndPos,
				zhun_dim.dimPos,sDimText,dimStyleId,zhun_dim.angle-0.5*Pi,sys.fonts.segment.fDimTextSize);
		}
		GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
		trans_pos(zhun_dim.lineStart,dcs.origin);
		trans_pos(zhun_dim.lineEnd,dcs.origin);
		CreateAcadLine(pBlockTableRecord,zhun_dim.lineStart,zhun_dim.lineEnd);
		GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
	}
	//��ע�Ǹ�֫��
	/*max_pos.x=min_pos.x=0;
	max_pos.y=min_pos.y-max(max_pos.y,pLineAngle->GetWidth()*pCommand->m_pDrawingCom->m_fScale);
	dim_pos.y=0.5*(min_pos.y+max_pos.y);
	dim_pos.x-=2*sys.fonts.segment.fDimTextSize;
	trans_pos(min_pos,dcs.origin);
	trans_pos(max_pos,dcs.origin);
	trans_pos(dim_pos,dcs.origin);
	CXhChar16 dim_txt("%.f",pLineAngle->GetWidth());
	DimAngleSize(pBlockTableRecord,max_pos,min_pos,dim_pos
			,dim_txt,dimStyleId,0);*/
	pBlockTableRecord->close();
	return TRUE;
}
//���ƽǸֽ�ͷ����ͼ
static BOOL DrawJgJointSideView(CDrawingCommand *pCommand)
{
	if(pCommand==NULL||pCommand->m_pDrawingCom==NULL||pCommand->m_pDrawingCom->m_eClassType!=DRAW_COM_STDVIEW)
		return FALSE;
	if(pCommand->m_paraPart.pPart==NULL||pCommand->m_paraPart.pPart->GetClassTypeId()!=CLS_LINEANGLE)
		return FALSE;
	CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pCommand->m_paraPart.pPart;
	CJointWrapperView xWrapperView;
	xWrapperView.m_pShortJg=pLineAngle;
	xWrapperView.CreateDrawing();
	//������ͼͼԪ
	CDrawCoordTrans dct;	//������ͼ�в���Ҫչ���������ʿ�ֱ��������ϵת����wjh-2011.8.23
	if(!pCommand->GetDCS(dct))
		return FALSE;
	f3dPoint base_pnt=dct.cs.origin;
	double zoomdraw_coef=pCommand->m_pDrawingCom->m_fScale;
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	for(f3dArcLine *pLine=xWrapperView.EnumFirstArc();pLine;pLine=xWrapperView.EnumNextArc())
	{		
		f3dArcLine arcline=*pLine;
		arcline.ZoomScale(zoomdraw_coef);
		arcline.Offset(base_pnt);
		if(arcline.SectorAngle()>0)
			CreateAcadArcLine(pBlockTableRecord,arcline.Center(),arcline.Start(),arcline.SectorAngle(),arcline.WorkNorm());
		else 
			CreateAcadLine(pBlockTableRecord,arcline.Start(),arcline.End());
	}
	GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
	DimStyleTable::dimStyle.InitDimStyle(sys.fonts.segment.fDimTextSize);
	DimStyleTable::dimStyle4.InitDimStyle(sys.fonts.segment.fDimTextSize);
	for(CSizeTextDim* pSizeDim=xWrapperView.EnumFirstSizeDim();pSizeDim;pSizeDim=xWrapperView.EnumNextSizeDim())
	{
		pSizeDim->dimStartPos*=zoomdraw_coef;
		pSizeDim->dimEndPos*=zoomdraw_coef;
		pSizeDim->dimPos*=zoomdraw_coef;
		pSizeDim->dimStartPos+=base_pnt;
		pSizeDim->dimEndPos+=base_pnt;
		pSizeDim->dimPos+=base_pnt;
		AcDbObjectId dimStyleId=DimStyleTable::dimStyle.dimStyleId;
		if(pSizeDim->angle==0)
			dimStyleId=DimStyleTable::dimStyle4.dimStyleId;
		DimAngleSize(pBlockTableRecord,pSizeDim->dimStartPos,pSizeDim->dimEndPos,pSizeDim->dimPos,pSizeDim->DimText(),dimStyleId,pSizeDim->angle);
	}
	//�������ݵ�λ�ñ�ע�������
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	for(CTextFrame* pDataPoint=xWrapperView.EnumFirstTextFrm();pDataPoint;pDataPoint=xWrapperView.EnumNextTextFrm())
	{
		f3dPoint data_pos=pDataPoint->dataPos*zoomdraw_coef+base_pnt;
		f3dPoint dim_pos=pDataPoint->dimPos*zoomdraw_coef+base_pnt;
		f3dPoint ref_vec=(dim_pos-data_pos).normalized();
		double fTextSize=sys.dim_map.PartNo.fPartNoTextSize;
		double radius=0.5*pDataPoint->Width();
		dim_pos+=ref_vec*radius;
		f3dPoint text_pos(dim_pos.x,dim_pos.y+0.5*fTextSize);
		DimText(pBlockTableRecord,text_pos,pDataPoint->Text(),TextStyleTable::hzfs.textStyleId,fTextSize,0,AcDb::kTextCenter,AcDb::kTextTop);
		//�����ı���������
		pDataPoint->dataPos=data_pos;
		pDataPoint->dimPos=dim_pos;
		f3dPoint refEndPos=pDataPoint->GetRefLineEndPos(1.0);
		CreateAcadLine(pBlockTableRecord,data_pos,refEndPos);
		//�����ı���Ŀ���
		if(pDataPoint->Style()==0)
			CreateAcadCircle(pBlockTableRecord,dim_pos,radius);
		else 
		{	
			f3dPoint waist_vec(1,0,0),norm_vec(0,1,0);
			f3dPoint topLeft=dim_pos-waist_vec*0.5*pDataPoint->Width()+norm_vec*0.5*pDataPoint->Height();
			if(pDataPoint->Style()==1)	//��Բ���ο�
				CreateAcadRect(pBlockTableRecord,topLeft,pDataPoint->Width(),pDataPoint->Height(),TRUE,NULL,waist_vec);
			else	//ֱ�Ǿ��ο�
				CreateAcadRect(pBlockTableRecord,topLeft,pDataPoint->Width(),pDataPoint->Height(),FALSE,NULL,waist_vec);
		}
	}
	pBlockTableRecord->close();
	return TRUE;
}
//���ƻ��������ṹͼ
static BOOL DrawPolyPartDrawing(CDrawingCommand *pCommand)
{
	if(pCommand==NULL)
		return FALSE;
	if(pCommand->m_pDrawingCom==NULL)
		return FALSE;
	if(pCommand->m_pDrawingCom->m_eClassType!=DRAW_COM_PARTVIEW
		||pCommand->m_pDrawingCom->m_pAttachView==NULL)	//������ͼΪ��
		return FALSE;
	CPartDrawing *pPartDrawing=(CPartDrawing*)pCommand->m_pDrawingCom;
	if(!pPartDrawing->m_bPolyPart)
		return FALSE;
	CDrawCoordTrans dcs;
	if(!pCommand->GetDCS(dcs))
		return FALSE;
	int index=0;
	f3dPoint vertexArr[10][3];
	CLDSPolyPart *pPolyPart=&(pPartDrawing->polyPart);
	CLDSLinePart *pTailLinePart=pPolyPart->segset.GetTail();
	if(pTailLinePart==NULL||(pTailLinePart&&pTailLinePart->GetClassTypeId()!=CLS_LINEANGLE))
		return FALSE;
	CLDSLineAngle *pLineAngle=NULL,*pPrevLineAngle=NULL,*pTailLineAngle=(CLDSLineAngle*)pTailLinePart;
	int wing_x0_y1=0;
	f3dPoint prev_wing_vec,wing_vec,wing_norm,work_norm=pCommand->m_pDrawingCom->m_pAttachView->ucs.axis_z;
	IsInsideJg(pTailLineAngle,work_norm,&wing_x0_y1);
	for(pLineAngle=(CLDSLineAngle*)pPolyPart->segset.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pPolyPart->segset.GetNext())
	{
		if(wing_x0_y1==0)
			wing_vec=pLineAngle->GetWingVecX();
		else 
			wing_vec=pLineAngle->GetWingVecY();
		//��һ�������ӽǸ���ӷǻ����˵�����������
		if(pPrevLineAngle==NULL)
		{	
			pLineAngle->getUCS(pLineAngle->ucs);
			if(!pLineAngle->bHuoQuStart)
				vertexArr[index][0]=pLineAngle->Start()-pLineAngle->ucs.axis_z*pLineAngle->startOdd();
			else 
				vertexArr[index][0]=pLineAngle->End()+pLineAngle->ucs.axis_z*pLineAngle->endOdd();
			vertexArr[index][1]=vertexArr[index][0]+wing_vec*pLineAngle->GetThick();
			vertexArr[index][2]=vertexArr[index][0]+wing_vec*pLineAngle->GetWidth();
			index++;
			//
			if(wing_x0_y1==0)
				wing_norm=pLineAngle->get_norm_x_wing();
			else 
				wing_norm=pLineAngle->get_norm_y_wing();
			prev_wing_vec=wing_vec;
			pPrevLineAngle=pLineAngle;
			continue;
		}
		//�м�Ļ����ӽǸ���ӻ�����������
		if(pLineAngle->bHuoQuStart)
			vertexArr[index][0]=pLineAngle->Start();
		else 
			vertexArr[index][0]=pLineAngle->End();
		vertexArr[index][1]=vertexArr[index][0]+wing_vec*pLineAngle->GetThick();
		f3dLine prev_wing_line(pPrevLineAngle->Start(),pPrevLineAngle->End());
		f3dLine wing_line(pLineAngle->Start(),pLineAngle->End());
		prev_wing_line.startPt+=prev_wing_vec*pPrevLineAngle->GetWidth();
		prev_wing_line.endPt+=prev_wing_vec*pPrevLineAngle->GetWidth();
		wing_line.startPt+=wing_vec*pLineAngle->GetWidth();
		wing_line.endPt+=wing_vec*pLineAngle->GetWidth();
		//
		project_point(wing_line.startPt,vertexArr[0][0],wing_norm);
		project_point(wing_line.endPt,vertexArr[0][0],wing_norm);
		project_point(prev_wing_line.startPt,vertexArr[0][0],wing_norm);
		project_point(prev_wing_line.endPt,vertexArr[0][0],wing_norm);
		int nRetCode=Int3dll(wing_line,prev_wing_line,vertexArr[index][2]);
		index++;
		//���һ�λ����ӽǸ���ӷǻ�����������
		if(pLineAngle->handle==pTailLinePart->handle)
		{	
			if(!pLineAngle->bHuoQuStart)
				vertexArr[index][0]=pLineAngle->Start()-pLineAngle->ucs.axis_z*pLineAngle->startOdd();
			else 
				vertexArr[index][0]=pLineAngle->End()+pLineAngle->ucs.axis_z*pLineAngle->endOdd();
			vertexArr[index][1]=vertexArr[index][0]+wing_vec*pLineAngle->GetThick();
			vertexArr[index][2]=vertexArr[index][0]+wing_vec*pLineAngle->GetWidth();
			index++;
		}
		prev_wing_vec=wing_vec;
		pPrevLineAngle=pLineAngle;
	}
	int i;
	for(i=0;i<index;i++)
	{	//ת������ͼ����ϵ��
		vertexArr[i][0]=dcs.TransToDraw(vertexArr[i][0]);
		vertexArr[i][1]=dcs.TransToDraw(vertexArr[i][1]);
		vertexArr[i][2]=dcs.TransToDraw(vertexArr[i][2]);
	}
	AcDbBlockTableRecord *pBlockTableRec=GetBlockTableRecord();
	for(i=0;i<index;i++)
	{
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		if(i==0||i==index-1)
			CreateAcadLine(pBlockTableRec,vertexArr[i][0],vertexArr[i][2]);
		else
		{	//���ƻ�����
			GetCurDwg()->setClayer(LayerTable::BendLineLayer.layerId);	
			CreateAcadLine(pBlockTableRec,vertexArr[i][0],vertexArr[i][2]);
		}
		if(i<index-1)
		{
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			CreateAcadLine(pBlockTableRec,vertexArr[i][0],vertexArr[i+1][0]);
			CreateAcadLine(pBlockTableRec,vertexArr[i][2],vertexArr[i+1][2]);
			GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
			CreateAcadLine(pBlockTableRec,vertexArr[i][1],vertexArr[i+1][1]);
		}
	}
	//������˨
	CLsRef *pLsRef=NULL;
	f3dCircle *pCir=NULL;
	CLsListDraw lsDrawList;
	for(pLineAngle=(CLDSLineAngle*)pPolyPart->segset.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pPolyPart->segset.GetNext())
	{
		for(pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
			lsDrawList.AppendLsRef(pLsRef->GetLsPtr(),TRUE,work_norm);
	}
	CLsDrawUnit *pLsDrawUnit=NULL;
	GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
	for(pLsDrawUnit=lsDrawList.GetFirst();pLsDrawUnit;pLsDrawUnit=lsDrawList.GetNext())
	{
		AcDbObjectId entId;
		LSDRAW.DrawBolt(pBlockTableRec,pLsDrawUnit,dcs,&entId);//1.0/pCommand->m_pDrawingCom->m_fScale,&entId);
	}
	//��ע��˨���
	
	pBlockTableRec->close();
	return TRUE;
}

static void DimJgZhun(CDrawingCommand *pCommand)
{
	if(pCommand->m_eCmdType!=CMD_DIM_SPECIAL_JGZHUN)
		return;
	CLDSLineAngle *pJg=pCommand->m_paraDimJgZhun.pLineAngle;
	if(pJg==NULL)
		pJg=(CLDSLineAngle*)Ta.FromPartHandle(pCommand->m_paraDimJgZhun.hLineAngle,CLS_LINEANGLE);
	if(pJg==NULL)
		return;
	CDrawCoordTrans dcs;
	if(!pCommand->GetDCS(dcs))
		return;
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	CZhunXianTextDim zhun_dim=pCommand->m_paraDimJgZhun.zhun_dim;
	//
	double draw_scale=pCommand->m_pDrawingCom->m_fScale;
	AcGePoint3d acad_dimStart,acad_dimEnd,acad_dimPos;
	DimStyleTable::dimStyle3.InitDimStyle(sys.dim_map.fDimTextSize);	//��ע�ָ�
	coord_trans(zhun_dim.lineStart,dcs,FALSE);
	coord_trans(zhun_dim.lineEnd,dcs,FALSE);
	f3dPoint vec=zhun_dim.lineEnd-zhun_dim.lineStart;
	normalize(vec);
	zhun_dim.lineStart = zhun_dim.lineStart*draw_scale;
	zhun_dim.lineEnd = zhun_dim.lineEnd*draw_scale;
	zhun_dim.lineStart-=2*vec;
	zhun_dim.lineEnd+=2*vec;
	//������˨����
	JGZJ jgzj;
	getjgzj(jgzj,pJg->GetWidth());
	long now_g=jgzj.g;
	CViewPart *pViewPart=NULL;
	if(pCommand->m_pDrawingCom&&pCommand->m_pDrawingCom->m_pAttachView
		&&pCommand->m_pDrawingCom->m_pAttachView->m_iViewType==1)
		pViewPart=pCommand->m_pDrawingCom->m_pAttachView->FindViewPart(pJg->handle);
	int x_wing0_y_wing1=0;
	if(pViewPart&&(pViewPart->cFlag=='X'||pViewPart->cFlag=='S'))
		x_wing0_y_wing1=0;	//X֫Ϊ��ǰ����֫
	else if(pViewPart&&(pViewPart->cFlag=='Y'||pViewPart->cFlag=='Q'))
		x_wing0_y_wing1=1;	//Y֫Ϊ��ǰ����֫
	else
		IsInsideJg(pJg,dcs.PrjNorm(),&x_wing0_y_wing1);
	if(pJg->m_bEnableTeG)
	{
		if(x_wing0_y_wing1==0)
			now_g=pJg->xWingXZhunJu.g;
		else 
			now_g=pJg->xWingYZhunJu.g;
	}
	//������˨����
	if(fabs(zhun_dim.dist-now_g)>1)
	{
		GetCurDwg()->setClayer(LayerTable::BoltLineLayer.layerId);
		CreateAcadLine(pBlockTableRecord,zhun_dim.lineStart,zhun_dim.lineEnd);
	}
	//��ע��˨����
	/*Ϊ��֤�������ͼһ�£�ֻҪ������ͱ�ע�����ڴ˽����ж� wjh-2011.8.31
	if(zhun_dim.dist>0
		&&fabs(zhun_dim.dist-jgzj.g)>1
		&&fabs(zhun_dim.dist-jgzj.g1)>1
		&&fabs(zhun_dim.dist-jgzj.g2)>1
		&&fabs(zhun_dim.dist-jgzj.g3)>1)	//����ͼ���1mm*/
	if(zhun_dim.bDimZhunJu)
	{
		char dimText[8]="";
		sprintf(dimText,"%.0f",zhun_dim.dist);
		f3dPoint vec;
		coord_trans(zhun_dim.dimStartPos,dcs,FALSE);
		coord_trans(zhun_dim.dimEndPos,dcs,FALSE);
		coord_trans(zhun_dim.dimPos,dcs,FALSE);
		zhun_dim.dimStartPos = zhun_dim.dimStartPos*draw_scale;
		zhun_dim.dimEndPos = zhun_dim.dimEndPos*draw_scale;
		zhun_dim.dimPos = zhun_dim.dimPos*draw_scale;
		zhun_dim.dimStartPos.z=zhun_dim.dimEndPos.z=zhun_dim.dimEndPos.z=0;
		//
		Sub_Pnt(vec,zhun_dim.dimEndPos,zhun_dim.dimStartPos);
		normalize(vec);
		f3dPoint vec_norm(-vec.y,vec.x);
		AcDbEntity *pEnt=pCommand->m_pDrawingCom->FindCADEntByHandle(pJg->handle);
		if(pEnt&&pEnt->isKindOf(AcDbLine::desc()))
		{
			f3dPoint verfiy_vec;
			AcDbLine *pLine=(AcDbLine*)pEnt;
			f3dPoint start_pt,end_pt;
			Cpy_Pnt(start_pt,pLine->startPoint());
			Cpy_Pnt(end_pt,pLine->endPoint());
			if(DISTANCE(zhun_dim.lineStart,start_pt)<DISTANCE(zhun_dim.lineStart,end_pt))
				Sub_Pnt(verfiy_vec,end_pt,start_pt);
			else
				Sub_Pnt(verfiy_vec,start_pt,end_pt);
			normalize(verfiy_vec);
			if(verfiy_vec*vec_norm)
				vec_norm*=-1.0;
		}
		//zhun_dim.dimPos +=vec_norm*5;
		zhun_dim.dimPos -= vec*4;
		int nFlag=1;
		if(!pCommand->m_paraDimJgZhun.bInside)	//�������Ʊ�ע���� wht 11-06-30
			nFlag=-1;
		f3dPoint dim_start=zhun_dim.dimStartPos+nFlag*vec_norm*pCommand->m_paraDimJgZhun.fSpace;
		f3dPoint dim_end=zhun_dim.dimEndPos+nFlag*vec_norm*pCommand->m_paraDimJgZhun.fSpace;
		f3dPoint dim_pos=zhun_dim.dimPos+nFlag*vec_norm*pCommand->m_paraDimJgZhun.fDimDistance;
		//dimStyle3ר�����ڱ�ע���ֿ������ⲿ���б�ע����� wht 11-07-12
		GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
		DimSize(pBlockTableRecord,dim_start,dim_end,
			dim_pos,dimText,DimStyleTable::dimStyle3.dimStyleId,2,sys.fonts.segment.fDimTextSize);
	}
	pBlockTableRecord->close();
}

//��ע��˨��Ȧ
BOOL IsRightToLeft(double x1,double x2,int nUnderlineLen)
{	//���ҵ���
	if(x1<(x2-0.1*nUnderlineLen))
		return TRUE;
	return FALSE;
}
static void DimBoltPad(CDrawingCommand *pCmd)
{
	if(pCmd==NULL||pCmd->m_pDrawingCom==NULL)
		return;
	if(pCmd->m_eCmdType!=CMD_DIM_BOLTPAD)
		return;
	if(pCmd->m_paraBoltPad.pBolt==NULL)
		pCmd->m_paraBoltPad.pBolt=(CLDSBolt*)Ta.FromPartHandle(pCmd->m_paraBoltPad.hBolt,CLS_BOLT);
	if(pCmd->m_paraBoltPad.pBolt==NULL)
		return;
	if(pCmd->m_paraBoltPad.pBolt->DianQuan.N<=0||pCmd->m_paraBoltPad.pBolt->DianQuan.thick<=0)
		return;
	//
	const int nUnderlineLen=7;	//��Ȧ��ע�»��߳���
	const double fMargin=0.5;	//��ע�������»���֮��ļ�϶
	f3dPoint pad_pos,insert_pos;
	AcDbObjectId *pEntId=NULL;
	pEntId=pCmd->m_pDrawingCom->keyEntIdList.GetValue(pCmd->m_paraBoltPad.hBolt);
	if(pEntId==NULL)
		return;
	AcDbEntity *pEnt=NULL;
	if(acdbOpenObject(pEnt,*pEntId,AcDb::kForRead)!=Acad::eOk)
		return;
	CAcDbObjLife entLife(pEnt);
	if(!pEnt->isKindOf(AcDbPoint::desc()))
		return;
	//
	AcDbPoint *pPadPoint=(AcDbPoint*)pEnt;
	char sPadText[20]="";
	if(!GetPadText(pPadPoint,sPadText)||strlen(sPadText)<=0)
		return;
	GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	//
	AcGePoint3d acad_insert_pos = pPadPoint->position();
	Cpy_Pnt(pad_pos,acad_insert_pos);
	f3dPoint start(pad_pos.x-1,pad_pos.y+1,0),end;
	if(pCmd->m_paraBoltPad.iDimPosType!=0)
		start.Set(pad_pos.x+pCmd->m_paraBoltPad.fDimPosOffsetX,pad_pos.y+pCmd->m_paraBoltPad.fDimPosOffsetY);
	AcDbObjectId line1Id,line2Id,textId;
	end = start;
	BOOL bRightToLeft=IsRightToLeft(start.x,pad_pos.x,nUnderlineLen);
	if(bRightToLeft)
		end.x-=nUnderlineLen;
	else
		end.x+=nUnderlineLen;
	line1Id=CreateAcadLine(pBlockTableRecord,pad_pos,start);
	line2Id=CreateAcadLine(pBlockTableRecord,start,end);
	f3dPoint dim_pos;
	if(bRightToLeft)
	{
		dim_pos.x = end.x+fMargin;
		dim_pos.y = end.y+fMargin;
	}
	else 
	{
		dim_pos.x = start.x + fMargin;
		dim_pos.y = start.y + fMargin;
	}
	textId = DimText(pBlockTableRecord,dim_pos,sPadText,TextStyleTable::hzfs.textStyleId,sys.dim_map.fLsGuigeTextSize,0);
	//
	if(pCmd->m_paraBoltPad.iDimPosType==0)
	{
		int nRetCode=1,nFlag=1;
		int quad_order[4]={0};
		f3dPoint rgn_vert[4];
		if(bRightToLeft)
		{
			quad_order[0]=2;
			quad_order[1]=3;
		}
		else 
		{
			nFlag=-1;
			quad_order[0]=1;
			quad_order[1]=4;
		}
		double fTextSize=sys.dim_map.fLsGuigeTextSize;
		start.Set(pad_pos.x-2*fMargin,pad_pos.y);
		rgn_vert[0].Set(start.x-nFlag*nUnderlineLen,start.y-(2*fMargin+fTextSize));
		rgn_vert[1].Set(start.x-nFlag*nUnderlineLen,start.y+(2*fMargin+fTextSize));
		rgn_vert[2].Set(start.x+nFlag*4*fMargin,start.y+(2*fMargin+fTextSize));
		rgn_vert[3].Set(start.x+nFlag*4*fMargin,start.y-(2*fMargin+fTextSize));
		nRetCode=SearchNoneIntersPos(&start,rgn_vert,NULL);
		//
		if(nRetCode!=0)
		{
			start.Set(pad_pos.x+2*fMargin,pad_pos.y);
			rgn_vert[0].Set(start.x+nFlag*nUnderlineLen,start.y-(2*fMargin+fTextSize));
			rgn_vert[1].Set(start.x+nFlag*nUnderlineLen,start.y+(2*fMargin+fTextSize));
			rgn_vert[2].Set(start.x-nFlag*4*fMargin,start.y+(2*fMargin+fTextSize));
			rgn_vert[3].Set(start.x-nFlag*4*fMargin,start.y-(2*fMargin+fTextSize));
			if(bRightToLeft)
			{
				quad_order[0]=1;
				quad_order[1]=4;
			}
			else 
			{
				quad_order[0]=2;
				quad_order[1]=3;
			}
			nRetCode=SearchNoneIntersPos(&start,rgn_vert,NULL,quad_order);
		}
		if(nRetCode!=0)
		{	//δ�ҵ����ʵ�λ�ã���Ҫ�ֶ�ָ��
			int type, track = 1;//��������϶�����
			AcGePoint3d acad_end,curPos;
			Cpy_Pnt(acad_end,end);
			while(track>0)
			{
				struct resbuf result;
				ads_grread(track,&type,&result);
				curPos.x = result.resval.rpoint[X];
				curPos.y = result.resval.rpoint[Y];
				curPos.z = 0;
				AcDbLine *pLine1=NULL,*pLine2=NULL;
				AcDbText *pDimText=NULL;
				acdbOpenObject(pLine1,line1Id,AcDb::kForWrite);
				acdbOpenObject(pLine2,line2Id,AcDb::kForWrite);
				acdbOpenObject(pDimText,textId,AcDb::kForWrite);
				CAcDbObjLife line1Life(pLine1),line2Life(pLine2),dimTextLife(pDimText);
				if(pLine1&&pLine2&&pDimText)
				{	
					pLine1->setEndPoint(curPos);
					pLine2->setStartPoint(curPos);
					Cpy_Pnt(acad_end,curPos);
					if(IsRightToLeft(curPos.x,pad_pos.x,nUnderlineLen))
						acad_end.x -= nUnderlineLen;
					else 
						acad_end.x += nUnderlineLen;
					pLine2->setEndPoint(acad_end);
					if(IsRightToLeft(curPos.x,pad_pos.x,nUnderlineLen))
					{
						curPos.x = acad_end.x+fMargin;
						curPos.y = acad_end.y+fMargin;
					}
					else 
					{
						curPos.x +=fMargin;
						curPos.y +=fMargin;
					}
					pDimText->setPosition(curPos);
				}
				else 
					break;
				if(type==3)
				{
					Cpy_Pnt(start,curPos);
					break;
				}
			}
		}
		else
		{
			AcDbLine *pLine1=NULL,*pLine2=NULL;
			AcDbText *pDimText=NULL;
			acdbOpenObject(pLine1,line1Id,AcDb::kForWrite);
			acdbOpenObject(pLine2,line2Id,AcDb::kForWrite);
			acdbOpenObject(pDimText,textId,AcDb::kForWrite);
			CAcDbObjLife line1Life(pLine1),line2Life(pLine2),dimTextLife(pDimText);
			if(pLine1&&pLine2&&pDimText)
			{	
				AcGePoint3d acad_end,curPos;
				Cpy_Pnt(curPos,start);
				if(curPos.x<pad_pos.x)
					curPos.x-=nUnderlineLen;
				pLine1->setEndPoint(curPos);
				pLine2->setStartPoint(curPos);
				Cpy_Pnt(acad_end,curPos);
				//if(IsRightToLeft(curPos.x,pad_pos.x,nUnderlineLen))
				if(bRightToLeft)
					acad_end.x -= nUnderlineLen;
				else 
					acad_end.x += nUnderlineLen;
				pLine2->setEndPoint(acad_end);
				//if(IsRightToLeft(curPos.x,pad_pos.x,nUnderlineLen))
				if(bRightToLeft)
				{
					curPos.x = acad_end.x+fMargin;
					curPos.y = acad_end.y+fMargin;
				}
				else 
				{
					curPos.x +=fMargin;
					curPos.y +=fMargin;
				}
				pDimText->setPosition(curPos);
			}
		}
		//��¼��Ȧ��עλ��ƫ����
		pCmd->m_paraBoltPad.iDimPosType=1;	//ָ����Ȧ��עλ��
		pCmd->m_paraBoltPad.fDimPosOffsetX=start.x-pad_pos.x;
		pCmd->m_paraBoltPad.fDimPosOffsetY=start.y-pad_pos.y;
	}
	pBlockTableRecord->close();	
}

//��ע��˨��Ϣ wht 12-03-21
static void DimBoltInfo(CDrawingCommand *pCmd)
{
	DRAGSET.ClearEntSet();
	if(pCmd==NULL||pCmd->m_pDrawingCom==NULL)
		return;
	if(pCmd->m_eCmdType!=CMD_DIM_BOLTINFO)
		return;
	if(pCmd->m_paraBoltGroupInfo.lsGroupDim.GetBoltNum()<=0)
		return;
	CLDSBolt *pDatumBolt=pCmd->m_paraBoltGroupInfo.lsGroupDim.GetDatumBolt();
	if(pDatumBolt==NULL)
		return;
	//�������׼��˨��Ӧ��CAD��˨ʵ��,�����ע��׼��λ��
	f3dPoint base_pos,dim_pos;
	AcDbObjectId *pBoltEntId=NULL,*pRodEntId=NULL;
	pBoltEntId=pCmd->m_pDrawingCom->keyEntIdList.GetValue(pDatumBolt->handle);
	if(pBoltEntId==NULL)
		return;
	AcDbEntity *pEnt=NULL;
	acdbOpenObject(pEnt,*pBoltEntId,AcDb::kForRead);
	if(pEnt==NULL)
		return;
	pEnt->close();
	if(pEnt->isKindOf(AcDbPoint::desc()))
	{
		AcDbPoint *pPoint=(AcDbPoint*)pEnt;	
		Cpy_Pnt(base_pos,pPoint->position());
		Cpy_Pnt(dim_pos,pPoint->position());
	}
	else if(pEnt->isKindOf(AcDbBlockReference::desc()))
	{
		AcDbBlockReference *pBlockRef=(AcDbBlockReference*)pEnt;	
		Cpy_Pnt(base_pos,pBlockRef->position());
		Cpy_Pnt(dim_pos,pBlockRef->position());
	}
	else 
		return;
	//�����ע�Ƕ�
	double rod_angle=0,dim_angle=0;
	CLDSLinePart *pDatumRod=pCmd->m_paraBoltGroupInfo.lsGroupDim.GetDatumRod();
	if(pDatumRod)
		pRodEntId=pCmd->m_pDrawingCom->keyEntIdList.GetValue(pDatumRod->handle);
	AcDbLine *pDatumLine=NULL;
	if(pRodEntId)
	{
		AcDbEntity *pEnt=NULL;
		acdbOpenObject(pEnt,*pRodEntId,AcDb::kForRead);
		if(pEnt==NULL||!pEnt->isKindOf(AcDbLine::desc()))
			return;
		pDatumLine=(AcDbLine*)pEnt;
		f3dPoint rod_vec;	//ʼ��ָ���ն˵ĸ˼�����
		Sub_Pnt(rod_vec,pDatumLine->endPoint(),pDatumLine->startPoint());
		rod_angle=dim_angle=Cal2dLineAng(0,0,rod_vec.x,rod_vec.y);
		if(dim_angle>Pi/2.0&&dim_angle<=1.5*Pi)
			dim_angle-=Pi;
		pEnt->close();
	}
	//��ע����
	char dim_text[100]="";
	pCmd->m_paraBoltGroupInfo.lsGroupDim.GetDimText(dim_text);
	//
	GetCurDwg()->setClayer(LayerTable::DimTextLayer.layerId);
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	//
	if(pCmd->m_paraBoltGroupInfo.iDimPosType==0)
	{	//�Զ�������עλ��
		int nRetCode=1;
		f3dPoint rgn_vert[4];
		double height=sys.dim_map.fDimTextSize;
		double length=strlen(dim_text)*sys.dim_map.fDimTextSize*0.7+0.5;
		//�ж���˨��ע������׼�˼�ʼ�˻����ն� wht 12-05-31
		LSGROUP_POS groupPos=pCmd->m_paraBoltGroupInfo.lsGroupDim.GetLsGroupPos(base_pos,pDatumLine,pCmd->m_pDrawingCom->m_fScale);
		if(pCmd->m_paraBoltGroupInfo.lsGroupDim.IsPlateLsGroup())
		{
			nRetCode=1;
			rgn_vert[0]=dim_pos;
			rgn_vert[1]=GetPtInPolar(dim_pos,dim_angle,length);
			rgn_vert[2]=GetPtInPolar(rgn_vert[1],dim_angle+Pi/2,height);
			rgn_vert[3]=GetPtInPolar(rgn_vert[0],dim_angle+Pi/2,height);
			nRetCode=SearchNoneIntersPos(&dim_pos,rgn_vert,NULL,NULL);
		}
		else if(groupPos==GROUP_START||groupPos==GROUP_END)
		{
			if(groupPos==GROUP_START)
			{	//ʼ�˱�ע
				if(rod_angle>Pi/2.0&&rod_angle<=1.5*Pi)
				{
					dim_pos=GetPtInPolar(dim_pos,rod_angle,length);	//������עλ��
					rgn_vert[0]=dim_pos;
					rgn_vert[0]=GetPtInPolar(rgn_vert[0],rod_angle-Pi,length);
					rgn_vert[1]=GetPtInPolar(rgn_vert[0],rod_angle,length);
					rgn_vert[2]=GetPtInPolar(rgn_vert[1],rod_angle-Pi/2,height);
					rgn_vert[3]=GetPtInPolar(rgn_vert[0],rod_angle-Pi/2,height);
					int quad_order[4]={1,4,0,0};
					nRetCode=SearchNoneIntersPos(&dim_pos,rgn_vert,NULL,quad_order);
				}
				else
				{
					rgn_vert[0]=dim_pos;
					rgn_vert[1]=GetPtInPolar(rgn_vert[0],rod_angle,length);
					rgn_vert[2]=GetPtInPolar(rgn_vert[1],rod_angle+Pi/2,height);
					rgn_vert[3]=GetPtInPolar(rgn_vert[0],rod_angle+Pi/2,height);
					nRetCode=SearchNoneIntersPos(&dim_pos,rgn_vert,NULL,NULL);
				}
			}
			else 
			{	//�ն˱�ע
				if(rod_angle>Pi/2.0&&rod_angle<=1.5*Pi)
				{
					rgn_vert[0]=dim_pos;
					rgn_vert[1]=GetPtInPolar(rgn_vert[0],rod_angle-Pi,length);
					rgn_vert[2]=GetPtInPolar(rgn_vert[1],rod_angle-Pi/2,height);
					rgn_vert[3]=GetPtInPolar(rgn_vert[0],rod_angle-Pi/2,height);
					nRetCode=SearchNoneIntersPos(&dim_pos,rgn_vert,NULL,NULL);
				}
				else
				{
					dim_pos=GetPtInPolar(dim_pos,rod_angle-Pi,length); //������עλ��
					rgn_vert[0]=dim_pos;
					rgn_vert[0]=GetPtInPolar(rgn_vert[0],rod_angle,length);
					rgn_vert[1]=GetPtInPolar(rgn_vert[0],rod_angle-Pi,length);
					rgn_vert[2]=GetPtInPolar(rgn_vert[1],rod_angle+Pi/2,height);
					rgn_vert[3]=GetPtInPolar(rgn_vert[0],rod_angle+Pi/2,height);
					int quad_order[4]={1,4,0,0};
					nRetCode=SearchNoneIntersPos(&dim_pos,rgn_vert,NULL,quad_order);
				}
			}
		}
		else 
		{
			rgn_vert[0]=dim_pos;
			rgn_vert[1]=GetPtInPolar(dim_pos,dim_angle,length);
			rgn_vert[2]=GetPtInPolar(rgn_vert[1],dim_angle+Pi/2,height);
			rgn_vert[3]=GetPtInPolar(rgn_vert[0],dim_angle+Pi/2,height);
			nRetCode=SearchNoneIntersPos(&dim_pos,rgn_vert,NULL,NULL);
		}
		//����ʱ������ʾ��ײ���Ҿ��ο�
		/*GetCurDwg()->setClayer(LayerTable::AuxLineLayer.layerId);
		CreateAcadLine(pBlockTableRecord,rgn_vert[0],rgn_vert[1]);//�±߿���
		GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
		CreateAcadLine(pBlockTableRecord,rgn_vert[2],rgn_vert[1]);//�ұ߿���
		GetCurDwg()->setClayer(LayerTable::BoltLineLayer.layerId);
		CreateAcadLine(pBlockTableRecord,rgn_vert[3],rgn_vert[2]);//�ϱ߿���
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		CreateAcadLine(pBlockTableRecord,rgn_vert[3],rgn_vert[0]);//��߿���*/
			
		if(nRetCode!=0)
		{	//δ�ҵ����ʵ�λ�ã���Ҫ�ֶ�ָ��
			if(pCmd->m_paraBoltGroupInfo.lsGroupDim.IsPlateLsGroup())
				DimMText(pBlockTableRecord,dim_text,dim_pos,0,sys.dim_map.fDimTextSize,TextStyleTable::hzfs.textStyleId,
						 AcDbMText::kMiddleCenter,AcDbMText::kLtoR,dim_angle);
			else
				DimText(pBlockTableRecord,dim_pos,dim_text,TextStyleTable::hzfs.textStyleId,sys.dim_map.fDimTextSize,
						dim_angle,AcDb::kTextLeft,AcDb::kTextBottom);
			ads_point L_T,R_B;
			SCOPE_STRU scope;
			for(int jj=0;jj<4;jj++)
				scope.VerifyVertex(rgn_vert[jj]);
			L_T[X]=scope.fMinX-20;
			L_T[Y]=scope.fMaxY+20;
			L_T[Z]=0;
			R_B[X]=scope.fMaxX+20;
			R_B[Y]=scope.fMinY-20;
			R_B[Z]=0;
		#ifdef _ARX_2007
			acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,RTNONE);
		#else
			acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,RTNONE);
		#endif
#ifdef AFX_TARG_ENU_ENGLISH
			DragEntSet(dim_pos,"\n Please select a right dimension plate:\n");
#else
			DragEntSet(dim_pos,"\n��ѡ��һ�����ʵı�עλ��:\n");
#endif
			//����ָ�ԭ�е���ͼ״̬����ȻacedSSGet��������ʧ��
		#ifdef _ARX_2007
			acedCommand(RTSTR,L"zoom",RTSTR,L"P",RTNONE);
		#else
			acedCommand(RTSTR,"zoom",RTSTR,"P",RTNONE);
		#endif
		}
		else
		{	//��ע��˨��Ϣ
			if(pCmd->m_paraBoltGroupInfo.lsGroupDim.IsPlateLsGroup())
				DimMText(pBlockTableRecord,dim_text,dim_pos,0,sys.dim_map.fDimTextSize,TextStyleTable::hzfs.textStyleId,
						 AcDbMText::kMiddleCenter,AcDbMText::kLtoR,dim_angle);
			else
				DimText(pBlockTableRecord,dim_pos,dim_text,TextStyleTable::hzfs.textStyleId,sys.dim_map.fDimTextSize,
					dim_angle,AcDb::kTextLeft,AcDb::kTextBottom);		
		}
		//��¼��Ȧ��עλ��ƫ����
		pCmd->m_paraBoltGroupInfo.iDimPosType=1;	//ָ����˨��Ϣ��עλ��
		pCmd->m_paraBoltGroupInfo.fDimPosOffsetX=dim_pos.x-base_pos.x;
		pCmd->m_paraBoltGroupInfo.fDimPosOffsetY=dim_pos.y-base_pos.y;
	}
	else //if(pCmd->m_paraBoltGroupInfo.iDimPosType!=0)
	{
		dim_pos.Set(base_pos.x+pCmd->m_paraBoltGroupInfo.fDimPosOffsetX,base_pos.y+pCmd->m_paraBoltGroupInfo.fDimPosOffsetY);
		if(pCmd->m_paraBoltGroupInfo.lsGroupDim.IsPlateLsGroup())
			DimMText(pBlockTableRecord,dim_text,dim_pos,0,sys.dim_map.fDimTextSize,TextStyleTable::hzfs.textStyleId,
					 AcDbMText::kMiddleCenter,AcDbMText::kLtoR,dim_angle);
		else
			DimText(pBlockTableRecord,dim_pos,dim_text,TextStyleTable::hzfs.textStyleId,
					sys.dim_map.fDimTextSize,dim_angle,AcDb::kTextLeft,AcDb::kTextBottom);
	}
	pBlockTableRecord->close();	
}

//
static void DimRodBoltOrFlInfo(CDrawingCommand *pCmd)
{
	if(pCmd==NULL||pCmd->m_eCmdType!=CMD_DIM_BOLTORFL_INFO)
		return;
	if(pCmd->m_paraBoltOrFlInfo.pLinePart==NULL)
		return;
	pCmd->m_paraBoltOrFlInfo.bDimStartOdd=FALSE;
	pCmd->m_paraBoltOrFlInfo.bDimEndOdd=FALSE;
	CXhChar100 sStartInfo,sEndInfo;
	CLDSLinePart *pRod=pCmd->m_paraBoltOrFlInfo.pLinePart;
	if(pRod->IsTube())
	{
		if(pRod->connectStart.m_iConnectType==0&&pRod->connectEnd.m_iConnectType==0&&
			stricmp(pRod->connectStart.flNo,pRod->connectEnd.flNo)==0)
		{
			pCmd->m_paraBoltOrFlInfo.bDimStartOdd=FALSE;
			pCmd->m_paraBoltOrFlInfo.bDimEndOdd=FALSE;
		}
		else 
		{
			if(pRod->connectStart.m_iConnectType==0)
			{
				sStartInfo.Copy(pRod->connectStart.flNo);
				pCmd->m_paraBoltOrFlInfo.bDimStartOdd=TRUE;
			}
			if(pRod->connectEnd.m_iConnectType==0)
			{
				sEndInfo.Copy(pRod->connectEnd.flNo);
				pCmd->m_paraBoltOrFlInfo.bDimEndOdd=TRUE;
			}
		}
	}
	else 
	{
		if(pRod->connectStart.wnSummBoltN>0&&pRod->connectStart.wnSummBoltN==pRod->connectEnd.wnSummBoltN&&
			pRod->connectStart.d==pRod->connectEnd.d)
		{
			pCmd->m_paraBoltOrFlInfo.bDimStartOdd=FALSE;
			pCmd->m_paraBoltOrFlInfo.bDimEndOdd=FALSE;
		}
		else
		{
			if(pRod->connectStart.wnSummBoltN>0)
			{
				sStartInfo.Printf("M%dx%d",pRod->connectStart.d,pRod->connectStart.wnSummBoltN);
				pCmd->m_paraBoltOrFlInfo.bDimStartOdd=TRUE;
			}
			if(pRod->connectEnd.wnSummBoltN>0)
			{
				sEndInfo.Printf("M%dx%d",pRod->connectEnd.d,pRod->connectEnd.wnSummBoltN);
				pCmd->m_paraBoltOrFlInfo.bDimEndOdd=TRUE;
			}
		}
	}
	if(!pCmd->m_paraBoltOrFlInfo.bDimStartOdd&&!pCmd->m_paraBoltOrFlInfo.bDimEndOdd)
		return;
	
	GetCurDwg()->setClayer(LayerTable::DimTextLayer.layerId);
	//��CAD�н�������ͷ��ע����
	if(pCmd->m_paraBoltOrFlInfo.iDimPosType!=0)
	{
		AcDbBlockTableRecord *pBlockTableRec=GetBlockTableRecord();
		if(pCmd->m_paraBoltOrFlInfo.fPoleAngle<0)
		{
			AcDbEntity *pEnt=NULL;
			AcDbObjectId *pEntId=NULL;
			pEntId=pCmd->m_pDrawingCom->keyEntIdList.GetValue(pCmd->m_paraBoltOrFlInfo.hLinePart);
			if(pEntId!=NULL)
			{
				acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
				if(pEnt!=NULL&&pEnt->isKindOf(AcDbLine::desc()))
				{
					f3dPoint start,end;
					AcGePoint3d acad_pnt=((AcDbLine*)pEnt)->startPoint();
					Cpy_Pnt(start,acad_pnt);
					acad_pnt=((AcDbLine*)pEnt)->endPoint();
					Cpy_Pnt(end,acad_pnt);
					pCmd->m_paraBoltOrFlInfo.fPoleAngle=Cal2dLineAng(start.x,start.y,end.x,end.y);
				}
				if(pEnt!=NULL)
					pEnt->close();
			}
		}
		double dim_angle=pCmd->m_paraBoltOrFlInfo.fPoleAngle;
		AcDb::TextHorzMode hStartMode=AcDb::kTextLeft;
		AcDb::TextHorzMode hEndMode=AcDb::kTextRight;
		if(pCmd->m_paraBoltOrFlInfo.fPoleAngle>Pi/2.0&&pCmd->m_paraBoltOrFlInfo.fPoleAngle<=1.5*Pi)
		{
			hStartMode=AcDb::kTextRight;
			hEndMode=AcDb::kTextLeft;
			dim_angle=pCmd->m_paraBoltOrFlInfo.fPoleAngle-Pi;
		}
		double height=sys.dim_map.fPartGuigeTextSize;
		//��ע��ʼ����Ϣ
		if(sStartInfo.GetLength()>0)
		{
			f3dPoint start(pCmd->m_paraBoltOrFlInfo.fStartOddBasePosX+pCmd->m_paraBoltOrFlInfo.fStartOddPosOffsetX,
				pCmd->m_paraBoltOrFlInfo.fStartOddBasePosY+pCmd->m_paraBoltOrFlInfo.fStartOddPosOffsetY);
			DimText(pBlockTableRec,start,sStartInfo,TextStyleTable::hzfs.textStyleId,height,dim_angle,hStartMode,AcDb::kTextVertMid);
		}
		//��ע��ֹ����Ϣ
		if(sEndInfo.GetLength()>0)
		{
			f3dPoint end(pCmd->m_paraBoltOrFlInfo.fEndOddBasePosX+pCmd->m_paraBoltOrFlInfo.fEndOddPosOffsetX,
				pCmd->m_paraBoltOrFlInfo.fEndOddBasePosY+pCmd->m_paraBoltOrFlInfo.fEndOddPosOffsetY);
			DimText(pBlockTableRec,end,sEndInfo,TextStyleTable::hzfs.textStyleId,height,dim_angle,hEndMode,AcDb::kTextVertMid);
		}
		pBlockTableRec->close();
	}
	else 
	{
		AcDbEntity *pEnt=NULL;
		AcDbObjectId *pEntId=NULL;
		pEntId=pCmd->m_pDrawingCom->keyEntIdList.GetValue(pCmd->m_paraBoltOrFlInfo.hLinePart);
		if(pEntId==NULL)
			return;
		acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
		if(pEnt==NULL)
			return;
		double angle=0;
		f3dPoint dim_pos;
		if(pEnt->isKindOf(AcDbLine::desc()))
		{
			AcDbBlockTableRecord *pBlockTableRec=GetBlockTableRecord();
			AcGePoint3d acad_pnt;
			f3dPoint start,end;
			acad_pnt=((AcDbLine*)pEnt)->startPoint();
			Cpy_Pnt(start,acad_pnt);
			acad_pnt=((AcDbLine*)pEnt)->endPoint();
			Cpy_Pnt(end,acad_pnt);
			angle=Cal2dLineAng(start.x,start.y,end.x,end.y);
			//��¼�˼���б��
			pCmd->m_paraBoltOrFlInfo.fPoleAngle=angle;
			double dim_angle=pCmd->m_paraBoltOrFlInfo.fPoleAngle;
			AcDb::TextHorzMode hStartMode=AcDb::kTextLeft;
			AcDb::TextHorzMode hEndMode=AcDb::kTextRight;
			if(pCmd->m_paraBoltOrFlInfo.fPoleAngle>Pi/2.0&&pCmd->m_paraBoltOrFlInfo.fPoleAngle<=1.5*Pi)
			{
				hStartMode=AcDb::kTextRight;
				hEndMode=AcDb::kTextLeft;
				dim_angle=pCmd->m_paraBoltOrFlInfo.fPoleAngle-Pi;
			}
			f3dPoint dimPos,rgn_vert[4];
			double height=sys.dim_map.fPartGuigeTextSize;
			double length;
			//��ע��ʼ����Ϣ
			if(sStartInfo.GetLength()>0)
			{
				length=sStartInfo.GetLength()*height*0.7;
				int quad_order[4]={0};
				if(angle>Pi/2.0&&angle<=1.5*Pi)
				{
					rgn_vert[0]=GetPtInPolar(start,angle+Pi/2,height/2+0.3);
					rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle,length);
					rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle-Pi/2,height+0.6);
					rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle-Pi/2,height+0.6);
					quad_order[0]=1;quad_order[1]=4;
				}
				else
				{
					rgn_vert[0]=GetPtInPolar(start,angle-Pi/2,height/2+0.3);
					rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle,length);
					rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle+Pi/2,height+0.6);
					rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle+Pi/2,height+0.6);
					quad_order[0]=1;quad_order[1]=4;
				}
				if(SearchNoneIntersPos(&start,rgn_vert,NULL,quad_order)!=0)
				{
					DRAGSET.ClearEntSet();
					DimText(pBlockTableRec,start,sStartInfo,TextStyleTable::hzfs.textStyleId,
						height,dim_angle,hStartMode,AcDb::kTextVertMid);
					f3dPoint box_vertex[4];
					ads_point L_T,R_B;
					SCOPE_STRU scope;
					box_vertex[0]=GetPtInPolar(start,angle-Pi/2,20);
					box_vertex[1]=GetPtInPolar(box_vertex[0],angle,20);
					box_vertex[2]=GetPtInPolar(box_vertex[1],angle+Pi/2,40);
					box_vertex[3]=GetPtInPolar(box_vertex[0],angle+Pi/2,40);
					for(int jj=0;jj<4;jj++)
						scope.VerifyVertex(box_vertex[jj]);
					L_T[X]=scope.fMinX;
					L_T[Y]=scope.fMaxY;
					L_T[Z]=0;
					R_B[X]=scope.fMaxX;
					R_B[Y]=scope.fMinY;
					R_B[Z]=0;
				#ifdef _ARX_2007
					acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
				#else
					acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
				#endif
#ifdef AFX_TARG_ENU_ENGLISH
					DragEntSet(start,"\n Please select a right dimension plate:\n");
#else
					DragEntSet(start,"\n��ѡ����ʵı�עλ��:\n");
#endif
					//����ָ�ԭ�е���ͼ״̬����ȻacedSSGet��������ʧ��
				#ifdef _ARX_2007
					acedCommand(RTSTR,L"zoom",RTSTR,L"P",RTNONE);
				#else
					acedCommand(RTSTR,"zoom",RTSTR,"P",RTNONE);
				#endif
				}
				else
				{
					DimText(pBlockTableRec,start,sStartInfo,TextStyleTable::hzfs.textStyleId,
						height,dim_angle,hStartMode,AcDb::kTextVertMid);
				}
				//��¼�˼�ʼ������ͷ��עλ�� wht 11-07-25
				pCmd->m_paraBoltOrFlInfo.fStartOddPosOffsetX=start.x-pCmd->m_paraBoltOrFlInfo.fStartOddBasePosX;
				pCmd->m_paraBoltOrFlInfo.fStartOddPosOffsetY=start.y-pCmd->m_paraBoltOrFlInfo.fStartOddBasePosY;
			}
			
			//��ע��ֹ����Ϣ
			if(sEndInfo.GetLength()>0)
			{
				length=sEndInfo.GetLength()*height*0.7;
				int quad_order[4]={0};
				if(angle>Pi/2.0&&angle<=1.5*Pi)
				{
					rgn_vert[0]=GetPtInPolar(end,angle+Pi/2,height/2+0.3);
					rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle-Pi,length);
					rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle-Pi/2,height+0.6);
					rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle-Pi/2,height+0.6);
					quad_order[0]=1;quad_order[1]=4;
				}
				else
				{
					rgn_vert[0]=GetPtInPolar(end,angle-Pi/2,height/2+0.3);
					rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle-Pi,length);
					rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle+Pi/2,height+0.6);
					rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle+Pi/2,height+0.6);
					quad_order[0]=1;quad_order[1]=4;
				}
				if(SearchNoneIntersPos(&end,rgn_vert,NULL,quad_order)!=0)
				{
					DRAGSET.ClearEntSet();
					DimText(pBlockTableRec,end,sEndInfo,TextStyleTable::hzfs.textStyleId,
						height,dim_angle,hEndMode,AcDb::kTextVertMid);
					f3dPoint box_vertex[4];
					ads_point L_T,R_B;
					SCOPE_STRU scope;
					box_vertex[0]=GetPtInPolar(end,angle-Pi/2,20);
					box_vertex[1]=GetPtInPolar(box_vertex[0],angle,20);
					box_vertex[2]=GetPtInPolar(box_vertex[1],angle+Pi/2,40);
					box_vertex[3]=GetPtInPolar(box_vertex[0],angle+Pi/2,40);
					for(int jj=0;jj<4;jj++)
						scope.VerifyVertex(box_vertex[jj]);
					L_T[X]=scope.fMinX;
					L_T[Y]=scope.fMaxY;
					L_T[Z]=0;
					R_B[X]=scope.fMaxX;
					R_B[Y]=scope.fMinY;
					R_B[Z]=0;
				#ifdef _ARX_2007
					acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
				#else
					acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
				#endif
#ifdef AFX_TARG_ENU_ENGLISH
					DragEntSet(end,"\n Please select a right dimension plate:\n");
#else
					DragEntSet(end,"\n��ѡ����ʵı�עλ��:\n");
#endif
					//����ָ�ԭ�е���ͼ״̬����ȻacedSSGet��������ʧ��
				#ifdef _ARX_2007
					acedCommand(RTSTR,L"zoom",RTSTR,L"P",RTNONE);
				#else
					acedCommand(RTSTR,"zoom",RTSTR,"P",RTNONE);
				#endif
				}
				else
				{
					DimText(pBlockTableRec,end,sEndInfo,TextStyleTable::hzfs.textStyleId,
						height,dim_angle,hEndMode,AcDb::kTextVertMid);
				}
				//��¼�˼��ն�����ͷ��עλ�� wht 11-07-25
				pCmd->m_paraBoltOrFlInfo.fEndOddPosOffsetX=end.x-pCmd->m_paraBoltOrFlInfo.fEndOddBasePosX;
				pCmd->m_paraBoltOrFlInfo.fEndOddPosOffsetY=end.y-pCmd->m_paraBoltOrFlInfo.fEndOddBasePosY;
			}
			pCmd->m_paraBoltOrFlInfo.iDimPosType=1;
			pBlockTableRec->close();
		}
		pEnt->close();
	}
	return;
}
//��ע�Ǹֽ�ͷ��Ϣ
static void DimAngleJoint(CDrawingCommand *pCmd)
{	//�ж��Ƿ�Ǹֽ�ͷ
	if(pCmd==NULL||pCmd->m_eCmdType!=CMD_DIM_ANGLEJOINT)
		return;
	ANGLE_JOINT *pAngleJoint=pCmd->m_paraAngleJoint.pJoint;
	if(pAngleJoint==NULL||pCmd->m_pDrawingCom==NULL||pCmd->m_pDrawingCom->m_eClassType!=DRAW_COM_LINEVIEW)
		return;
	CLineView *pLineView=(CLineView*)pCmd->m_pDrawingCom;
	//1.�ж��Ƿ��Ѿ���ע��������Ѿ���ע���򷵻�
	if(pLineView->ParentDrawing()->IsItemDimmed(CDrawingComponent::TYPE_JOINT,pAngleJoint->Id()))
		return;	//�ý�ͷ�ڵ�ǰ��ͼ���ѻ��ƹ�
	
	//2.ȷ����ע��Ϣ����עλ��
	//2.1 ȷ����ͷ��ע��Ϣ�����߳���
	double fDimTextHeight=sys.dim_map.fPartGuigeTextSize;//���ָ߶�
	CXhChar100 sBoltInfo=pAngleJoint->GetBoltInfo(FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
	CXhChar100 sJointType("Joint type��");
	if(pAngleJoint->cJointType==0)
		sJointType.Append("Inner angle outer plate");
	else if(pAngleJoint->cJointType==1)
		sJointType.Append("Outer angle");
	else if(pAngleJoint->cJointType==2)
		sJointType.Append("Both angle");
#else
	CXhChar100 sJointType("��ͷ���ͣ�");
	if(pAngleJoint->cJointType==0)
		sJointType.Append("�ڰ�������ְ�");
	else if(pAngleJoint->cJointType==1)
		sJointType.Append("�����");
	else if(pAngleJoint->cJointType==2)
		sJointType.Append("�������");
#endif
	CXhChar100 sJointPartInfo("%s %s",(char*)pAngleJoint->GetInnerPartSpec(TRUE),(char*)pAngleJoint->GetOutterPartSpec(TRUE));
	//���߼нǡ��������ԭ��ı��˾��롢��ע�������
	//TODO: 1.2Ϊ�ַ���������ϵ�����˴����㲻׼ȷ��Ľ�
	int nUnderLineLen=(int)(max(max(sBoltInfo.GetLength(),sJointType.GetLength()),sJointPartInfo.GetLength())*1.2);
	//2.2 ȷ����ע��׼�㼰����������
	const double fDimRowMargin=1;
	f3dPoint dimBasePoint;	//��ע��׼��
	f3dPoint dimBendPoint;	//��ע�����
	double   fOrigionAngle;	//��ͷ����ˮƽ��н�
	//2.2.1 ���ݽ�ͷid���ҽ�ͷʾ���ߣ������ͷ��ע��׼��
	AcDbEntity *pEnt=NULL;
	AcDbObjectId *pEntId=NULL;
	pEntId=pLineView->jointEntIdList.GetValue(pCmd->m_paraAngleJoint.idAngleJoint);
	if(pEntId==NULL)
		return;
	acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
	if(pEnt==NULL)
		return;
	if(!pEnt->isKindOf(AcDbPolyline::desc()))
	{
		pEnt->close();
		return;
	}
	AcGePoint3d acad_pnt;
	f3dPoint start,end;
	((AcDbPolyline*)pEnt)->getStartPoint(acad_pnt);
	Cpy_Pnt(start,acad_pnt);
	((AcDbPolyline*)pEnt)->getEndPoint(acad_pnt);
	Cpy_Pnt(end,acad_pnt);
	fOrigionAngle=Cal2dLineAng(start.x,start.y,end.x,end.y);
	Add_Pnt(dimBasePoint,start,end);
	dimBasePoint /=2.0;
	pEnt->close();

	//2.2.2 �����ע�����
	int nRetCode=1;//0���Ѿ��ҵ����߹յ㣨�����е�һ�����յ�ڶ�������㣩1��û���ҵ�
	if(pCmd->m_paraAngleJoint.iDimPosType!=0)
	{	//���ݱ���Ĳ������������
		dimBendPoint.x =dimBasePoint.x+pCmd->m_paraAngleJoint.fDimPosOffsetX;
		dimBendPoint.y =dimBasePoint.y+pCmd->m_paraAngleJoint.fDimPosOffsetY;
	}
	else 
	{	//�Զ����������λ��
		double fDimPosOffsetX=10.0,fDimPosOffsetY=5.0;
		//����������ˮƽ�нǼ����X ��Yƫ�ƾ���
		if(fOrigionAngle>Pi/2.0&&fOrigionAngle<=1.5*Pi)
		{
			pCmd->m_paraAngleJoint.fDimPosOffsetX =fDimPosOffsetX;
			pCmd->m_paraAngleJoint.fDimPosOffsetY =fDimPosOffsetY;
		}
		else
		{
			pCmd->m_paraAngleJoint.fDimPosOffsetX =-fDimPosOffsetX;
			pCmd->m_paraAngleJoint.fDimPosOffsetY =fDimPosOffsetY;
		}
		//�õ������е�һ��ֱ�ߵ�����յ�
		dimBendPoint.x=pCmd->m_paraAngleJoint.fDimPosOffsetX+dimBasePoint.x;
		dimBendPoint.y=pCmd->m_paraAngleJoint.fDimPosOffsetY+dimBasePoint.y;

		//���þ��������жϳ��Ƿ��к��ʵ�λ�ñ�ע
		double fTempAngle=Cal2dLineAng(dimBasePoint.x,dimBasePoint.y,dimBendPoint.x,dimBendPoint.y);
		f3dPoint rgn_vert[4];//�жϾ���������ĸ��ߵ�
		int quad_order[4]={1,4,0,0};
		if(fTempAngle>Pi/2.0&&fTempAngle<=1.5*Pi)
		{
			quad_order[0]=2;
			quad_order[1]=3;
			rgn_vert[0].Set(dimBendPoint.x,dimBendPoint.y+fDimRowMargin+fDimTextHeight,0);
			rgn_vert[1].Set(dimBendPoint.x,dimBendPoint.y-2*(fDimRowMargin+fDimTextHeight),0);
			rgn_vert[2].Set(dimBendPoint.x+nUnderLineLen,dimBendPoint.y-2*(fDimRowMargin+fDimTextHeight),0);
			rgn_vert[3].Set(dimBendPoint.x+nUnderLineLen,dimBendPoint.y+fDimRowMargin+fDimTextHeight,0);
		}
		else 
		{
			rgn_vert[0].Set(dimBendPoint.x-nUnderLineLen,dimBendPoint.y+fDimRowMargin+fDimTextHeight,0);
			rgn_vert[1].Set(dimBendPoint.x-nUnderLineLen,dimBendPoint.y-2*(fDimRowMargin+fDimTextHeight),0);
			rgn_vert[2].Set(dimBendPoint.x,dimBendPoint.y-2*(fDimRowMargin+fDimTextHeight),0);
			rgn_vert[3].Set(dimBendPoint.x,dimBendPoint.y+fDimRowMargin+fDimTextHeight,0);
		}
		nRetCode=SearchNoneIntersPos(&dimBendPoint,rgn_vert,NULL,quad_order);
	}
	//3.���ƽǸֽ�ͷ��ע
	AcDbBlockTableRecord *pBlockTableRec=GetBlockTableRecord();
	AcDbObjectId lineFirstId=CreateAcadLine(pBlockTableRec,dimBasePoint,dimBendPoint);//����б��
	f3dPoint bendLineEndPt=dimBendPoint;
	f3dPoint textDimPos=dimBendPoint;
	BOOL bRightToLeft=IsRightToLeft(dimBendPoint.x,dimBasePoint.x,nUnderLineLen);
	if(bRightToLeft)
	{
		bendLineEndPt.x-=nUnderLineLen;
		textDimPos=bendLineEndPt;
	}
	else
	{
		bendLineEndPt.x+=nUnderLineLen;
		textDimPos=dimBendPoint;
	}
	AcDbObjectId lineSecondId=CreateAcadLine(pBlockTableRec,dimBendPoint,bendLineEndPt);//����ƽ����
	textDimPos.y+=fDimRowMargin;
	AcDbObjectId textBoltId=DimText(pBlockTableRec,textDimPos,sBoltInfo,TextStyleTable::hzfs.textStyleId,fDimTextHeight,0);
	textDimPos.y=dimBendPoint.y-fDimTextHeight-fDimRowMargin;
	AcDbObjectId textJointTypeId=DimText(pBlockTableRec,textDimPos,sJointType,TextStyleTable::hzfs.textStyleId,fDimTextHeight,0);
	textDimPos.y-=(fDimTextHeight+fDimRowMargin);
	AcDbObjectId textJointPartId=DimText(pBlockTableRec,textDimPos,sJointPartInfo,TextStyleTable::hzfs.textStyleId,fDimTextHeight,0);
	pBlockTableRec->close();
	//4.�Զ�����δ�ҵ�����λ��ʱ�ֶ�ָ����עλ��
	if(pCmd->m_paraAngleJoint.iDimPosType==0&&nRetCode!=0)
	{	//δ�ҵ����ʵ�λ�ã���Ҫ�ֶ�ָ��
		int type, track = 1;//��������϶�����
		AcGePoint3d acad_end,curPos,dim_pos;
		Cpy_Pnt(acad_end,end);
		while(track>0)
		{
			struct resbuf result;
			ads_grread(track,&type,&result);
			curPos.x = result.resval.rpoint[X];
			curPos.y = result.resval.rpoint[Y];
			curPos.z = 0;
			AcDbLine *pLine1=NULL,*pLine2=NULL;
			AcDbText *pDimTextBolt=NULL,*pDimTextJointType=NULL,*pDimTextJointPart=NULL;
			acdbOpenObject(pLine1,lineFirstId,AcDb::kForWrite);
			acdbOpenObject(pLine2,lineSecondId,AcDb::kForWrite);
			acdbOpenObject(pDimTextBolt,textBoltId,AcDb::kForWrite);
			acdbOpenObject(pDimTextJointType,textJointTypeId,AcDb::kForWrite);
			acdbOpenObject(pDimTextJointPart,textJointPartId,AcDb::kForWrite);
			CAcDbObjLife line1Life(pLine1),line2Life(pLine2),dimTextStartLife(pDimTextBolt),dimTextMidLife(pDimTextJointType),dimTextEndLife(pDimTextJointPart);
			if(pLine1&&pLine2&&pDimTextBolt&&pDimTextJointType&&pDimTextJointPart)
			{	
				pLine1->setEndPoint(curPos);
				pLine2->setStartPoint(curPos);
				Cpy_Pnt(acad_end,curPos);
				if(IsRightToLeft(curPos.x,dimBasePoint.x,nUnderLineLen))
				{
					acad_end.x -= nUnderLineLen;
					Cpy_Pnt(dim_pos,acad_end);
				}
				else 
				{
					acad_end.x += nUnderLineLen;
					Cpy_Pnt(dim_pos,curPos);
				}
				pLine2->setEndPoint(acad_end);
				//
				dim_pos.y+=fDimRowMargin;
				pDimTextBolt->setPosition(dim_pos);
				dim_pos.y=curPos.y-fDimTextHeight-fDimRowMargin;
				pDimTextJointType->setPosition(dim_pos);
				dim_pos.y-=(fDimTextHeight+fDimRowMargin);
				pDimTextJointPart->setPosition(dim_pos);
				//
				pCmd->m_paraAngleJoint.fDimPosOffsetX=curPos.x-dimBasePoint.x;
				pCmd->m_paraAngleJoint.fDimPosOffsetY=curPos.y-dimBasePoint.y;
			}
			else 
				break;
			if(type==3)
			{
				Cpy_Pnt(start,curPos);
				break;
			}
		}
	}
	//
	if(pCmd->m_paraAngleJoint.iDimPosType==0)
		pCmd->m_paraAngleJoint.iDimPosType=1;
	pLineView->MarkItemDimSpecState(CDrawingComponent::TYPE_JOINT,pAngleJoint->Id());
}
//���Ƹ�����
static void DrawAssistLine(CDrawingCommand *pCmd,CDrawCoordTrans dcs)
{
	if(pCmd==NULL||pCmd->m_pDrawingCom==NULL)
		return;
	if(pCmd->m_eCmdType!=CMD_DRAW_ASSIST_LINE)
		return;
	CDisplayView *pView=pCmd->m_pDrawingCom->m_pAttachView;
	if(pView==NULL)//||pView->m_iViewType!=1)
		return;
	SEGI iSeg;
	if(pView->m_iViewType==0)
		iSeg=pCmd->m_pDrawingCom->ParentDrawing()->m_nSegI;
	CDrawingAssistant *pAssistLine=pView->GetDrawingAssistByNo(pCmd->m_paraDrawAssistLine.iNo,iSeg);
	if(pAssistLine==NULL)
		return;	
	if(pAssistLine->m_nStyle!=pCmd->m_paraDrawAssistLine.nStyle)
		return;	//���Ͳ�һ�³����쳣
	if(pCmd->m_paraDrawAssistLine.nStyle==1)		//��������
		GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
	else if(pCmd->m_paraDrawAssistLine.nStyle==2)	//����������
		GetCurDwg()->setClayer(LayerTable::AssistHuoQuLineLayer.layerId);
	else 
		return;
	f3dPoint startPt,endPt;
	pAssistLine->m_xLineStart.UpdatePos(&Ta);
	pAssistLine->m_xLineEnd.UpdatePos(&Ta);
	startPt=pAssistLine->m_xLineStart.Position();
	endPt=pAssistLine->m_xLineEnd.Position();
	
	if(pAssistLine->iResidePlaneNo>0)
	{	//���㸨���ߵ���ת���߼���ת�Ƕ�
		CDrawingAssistant *pSpreadFace=pView->GetDrawingAssistByNo(pAssistLine->iResidePlaneNo,pAssistLine->m_iSeg);
		if(pSpreadFace)
		{
			pSpreadFace->GetRotateAxisAndAngle(dcs.PrjNorm(),dcs.m_xSpreadLine,dcs.m_fSpreadAngle,&Ta);
			dcs.m_xSpreadPlaneNorm=pSpreadFace->m_xFaceNorm.vector;
		}
	}
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	startPt=dcs.TransToDraw(startPt);
	endPt=dcs.TransToDraw(endPt);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	pBlockTableRecord->close();
}

//��������
int CDrawingCommand::Run()
{
	DRAGSET.InitClearDegree();	//��ʼ����մ���
	int nOldEntCount=DRAGSET.GetEntNum();	//��¼��ק�����е�ʵ�����
	CDrawCoordTrans dcs;
	if(!GetDCS(dcs))
		return 0;
	if(m_pDrawingCom==NULL)
		return 0;
	if(!IsNeedExecute())
		return 0;	//�������Ҫִ��
	DRAGSET.m_pDrawCmdTagInfo=NULL;
	if(m_eCmdType==CMD_DRAW_PART)
	{	//���ƹ���
		CPartDrawing *pPartDrawing=NULL;
		if(m_pDrawingCom->m_eClassType==DRAW_COM_PARTVIEW)
			pPartDrawing=(CPartDrawing*)m_pDrawingCom;
		if((pPartDrawing==NULL||!pPartDrawing->m_bPolyPart)&&m_paraPart.pPart==NULL)
			return FALSE;
		AcDbObjectId entId=0;
		if(pPartDrawing)
		{
			if(pPartDrawing->m_bPolyPart)
				DrawPolyPartDrawing(this);
			else if(m_paraPart.pPart&&m_paraPart.pPart->GetClassTypeId()==CLS_LINEANGLE)
				DrawLineAngleDrawing(this);
			else if(m_paraPart.pPart&&m_paraPart.pPart->GetClassTypeId()==CLS_PLATE)
			{
				BOOL para[10]={FALSE};
				para[9]=TRUE;
				double fOldScale=sys.general_set.PartMapScale.fRealToDraw;	//��¼��ǰ����ͼ��ͼ����
				sys.general_set.PartMapScale.fRealToDraw=1.0/m_pDrawingCom->m_fScale;
				DrawPlankTypePartMap(m_paraPart.pPart,NULL,para,-1,1,FALSE,TRUE,1,1,&pPartDrawing->drawing_offset);
				sys.general_set.PartMapScale.fRealToDraw=fOldScale;			//�ָ�ԭ�й���ͼ��ͼ����
			}
		}
		else
		{
			CViewPart *pViewPart=NULL;
			if(m_pDrawingCom&&m_pDrawingCom->m_pAttachView)
				pViewPart=m_pDrawingCom->m_pAttachView->FindViewPart(m_paraPart.pPart->handle);
			if(m_paraPart.pPart->GetClassTypeId()==CLS_BOLT)	//������˨
			{
				GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
				//��˨λ�ò���
				CLsDrawUnit lsDrawUnit;
				lsDrawUnit.bOriginal=m_paraPart.para.bolt.bOriginal;
				lsDrawUnit.front0_side1=m_paraPart.para.bolt.front0_side1;
				lsDrawUnit.bDrawSideLine=m_paraPart.para.bolt.bDrawSideBoltLine;
				lsDrawUnit.pos=m_paraPart.pPart->ucs.origin;
				lsDrawUnit.pLs=(CLDSBolt*)m_paraPart.pPart;
				if(fabs(dcs.m_fSpreadAngle)>EPS)
				{
					lsDrawUnit.rotate_angle=dcs.m_fSpreadAngle;
					lsDrawUnit.spreadLine=dcs.m_xSpreadLine;
				}
				AcDbObjectId entId=0;
				AcDbBlockTableRecord *pBlockTableRec=GetBlockTableRecord();
				lsDrawUnit.bDimHoleDiameter=m_paraPart.para.bolt.bDimHoleDiameter;
				LSDRAW.DrawBolt(pBlockTableRec,&lsDrawUnit,dcs,&entId);
				if(entId!=0)
					m_pDrawingCom->keyEntIdList.SetValue(m_paraPart.pPart->handle,entId);	//��¼��˨��Ӧ��CADʵ��
				pBlockTableRec->close();
			}
			else	//���ƹ���
			{
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
				if(m_paraPart.pPart->IsLinePart())
				{	//���ø˼�����ʼ�ն����쳤��
					sys.stru_map.fPoleAxisLineStartOutSpreadLen=m_paraPart.fPoleAxisLineStartOutSpreadLen;
					sys.stru_map.fPoleAxisLineEndOutSpreadLen=m_paraPart.fPoleAxisLineEndOutSpreadLen;
					f3dPoint pos=((CLDSLinePart*)m_paraPart.pPart)->Start();
					pos=dcs.TransToDraw(pos);
					m_paraPart.drawStartBasePos.Set(pos.x,pos.y);
					pos=((CLDSLinePart*)m_paraPart.pPart)->End();
					pos=dcs.TransToDraw(pos);
					m_paraPart.drawEndBasePos.Set(pos.x,pos.y);
				}
				m_paraPart.pPart->feature=(long)pViewPart; //������CViewPartָ��
				AcDbBlockTableRecord *pBlockTableRec=GetBlockTableRecord();
				if(m_paraPart.pPart->IsAngle())
					DRAGSET.m_pDrawCmdTagInfo=&drawAngleInfo;
				entId=DrawPart(pBlockTableRec,CDrawPartInfo(m_paraPart.pPart,pViewPart,this),dcs);
				DRAGSET.m_pDrawCmdTagInfo=NULL;
				m_pDrawingCom->keyEntIdList.SetValue(m_paraPart.pPart->handle,entId);
				pBlockTableRec->close();
			}
		}
	}
	else if(m_eCmdType==CMD_DIM_ODDMENT)
	{	//��ע����ͷ
		if(m_paraOddment.pLinePart==NULL)
			return FALSE;
		if(m_paraOddment.pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			return FALSE;	//����ע��ϽǸ�����ͷ
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		AcDbBlockTableRecord *pBlockTableRec=GetBlockTableRecord();
		if(DimSinglePoleOddment(pBlockTableRec,this)&&m_paraOddment.iDimPosType==0)
			m_paraOddment.iDimPosType=1;
		pBlockTableRec->close();
	}
	else if(m_eCmdType==CMD_DIM_BOLTORFL_INFO)
	{	//��ע�˼���˨��Ϣ������Ϣ
		DimRodBoltOrFlInfo(this);
	}
	else if(m_eCmdType==CMD_DIM_ANGLEJOINT)
	{	//��ע�Ǹֽ�ͷ��Ϣ
		DimAngleJoint(this);
	}
	else if(m_eCmdType==CMD_DRAW_LINEPART)
	{	//���Ƹ˼�����ͼ
		if(m_paraLinePart.pLinePart==NULL)
			return FALSE;
		f3dPoint pos=m_paraLinePart.pLinePart->Start();
		if( m_paraLinePart.pLinePart->pStart!=NULL && 
			m_pDrawingCom!=NULL&&m_pDrawingCom->ParentDrawing()!=NULL&&m_pDrawingCom->ParentDrawing()->m_nSegI==-1)	//˾��ͼ
			pos=m_paraLinePart.pLinePart->pStart->xPreliftPos;
		pos=dcs.TransToDraw(pos);
		m_paraLinePart.drawStartBasePos.Set(pos.x,pos.y);
		pos=m_paraLinePart.pLinePart->End();
		if( m_paraLinePart.pLinePart->pEnd!=NULL && 
			m_pDrawingCom!=NULL&&m_pDrawingCom->ParentDrawing()!=NULL&&m_pDrawingCom->ParentDrawing()->m_nSegI==-1)	//˾��ͼ
			pos=m_paraLinePart.pLinePart->pEnd->xPreliftPos;
		pos=dcs.TransToDraw(pos);
		m_paraLinePart.drawEndBasePos.Set(pos.x,pos.y);
		//
		AcDbObjectId entId=0,startId=0,endId=0,startJointId=0,endJointId=0;
		DrawRodLine(m_paraLinePart.pLinePart,dcs,m_paraLinePart.bSumMap,
			m_paraLinePart.bDimByAnalysisMode,&entId,&startId,&endId,&startJointId,&endJointId,this);
		m_pDrawingCom->keyEntIdList.SetValue(m_paraLinePart.pLinePart->handle,entId);
		if(startId!=0)	//ʼ�˽ڵ� wht 11-06-15
			m_pDrawingCom->keyEntIdList.SetValue(m_paraLinePart.pLinePart->pStart->handle,startId);
		if(endId!=0)	//�ն˽ڵ�
			m_pDrawingCom->keyEntIdList.SetValue(m_paraLinePart.pLinePart->pEnd->handle,endId);
		if(m_pDrawingCom->m_eClassType==DRAW_COM_LINEVIEW)
		{
			CLineView *pLineView=(CLineView*)m_pDrawingCom;
			if(startJointId!=0)
				pLineView->jointEntIdList.SetValue(m_paraLinePart.pLinePart->connectStart.idAngleJoint,startJointId);
			if(endJointId!=0)
				pLineView->jointEntIdList.SetValue(m_paraLinePart.pLinePart->connectEnd.idAngleJoint,endJointId);
		}
	}
	else if(m_eCmdType==CMD_DIM_SEGI)
		DimSegI(this);
	else if(m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)	//��ע��˨���
	{
		AcDbBlockTableRecord *pBlockTableRec=GetBlockTableRecord();
		AcDbObjectId *pStartEntId=NULL,*pEndEntId=NULL;
		pStartEntId=m_pDrawingCom->keyEntIdList.GetValue(m_paraDimLsSpace.spaceDim.hStartLs);
		pEndEntId=m_pDrawingCom->keyEntIdList.GetValue(m_paraDimLsSpace.spaceDim.hEndLs);
		if(pStartEntId&&pEndEntId)
		{
			m_paraDimLsSpace.spaceDim.StartLsId=*pStartEntId;
			m_paraDimLsSpace.spaceDim.EndLsId=*pEndEntId;
			GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
			DimSingleLsSpace(pBlockTableRec,&m_paraDimLsSpace.spaceDim,
				m_paraDimLsSpace.fDimDistance,m_paraDimLsSpace.fSpace,m_paraDimLsSpace.bInside);
		}
		pBlockTableRec->close();
	}
	else if(m_eCmdType==CMD_DRAW_TABLE)
	{	//���Ʊ��
		if(m_pDrawingCom&&m_pDrawingCom->m_eClassType==DRAW_COM_BOM
			&&(m_paraBOMTable.iFormatTbl==1||m_paraBOMTable.iFormatTbl==2))
		{	//���������м�¼��iFormatTbl���б����� wht 11-07-14
			CDrawingBOM *pDrawingBOM=(CDrawingBOM*)m_pDrawingCom;
			pDrawingBOM->PrepareTable();	//�������ɱ������
			CFormatTbl *pFormatTbl=NULL;
			if(m_paraBOMTable.iFormatTbl==1)
				pFormatTbl=&pDrawingBOM->xFormatTbl1;
			else 
				pFormatTbl=&pDrawingBOM->xFormatTbl2;
			AcDbBlockTableRecord *pBlockTableRec=GetBlockTableRecord();
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			f2dPoint base_pos;
			f3dPoint inside_lefttop=m_pDrawingCom->GetMapFrmInsideTopLeft();
			base_pos.Set(inside_lefttop.x+m_paraBOMTable.fDimPosX,inside_lefttop.y+m_paraBOMTable.fDimPosY);
			pFormatTbl->SetBasePos(base_pos);
			if(m_paraBOMTable.bSelectPos)
			{
				m_paraBOMTable.fTableWidth=pFormatTbl->GetTableWide();
				m_paraBOMTable.fTableHeight=pFormatTbl->GetTableHeight();
				//���������ɱ�����¼��㵱ǰ���λ��
				m_pDrawingCom->ParentDrawing()->UpdateBOMTableInitPos((CDrawingBOM*)m_pDrawingCom);
				base_pos.Set(inside_lefttop.x+m_paraBOMTable.fDimPosX,inside_lefttop.y+m_paraBOMTable.fDimPosY);
				pFormatTbl->SetBasePos(base_pos);
				//
				m_paraBOMTable.bSelectPos=FALSE;
				if(m_paraBOMTable.bDisplayDlg)
					DrawTable(pBlockTableRec,pFormatTbl,TRUE);
				else 
					DrawTable(pBlockTableRec,pFormatTbl,TRUE,1.0,m_paraBOMTable.nLocatePos);
				//��¼���λ����
				m_paraBOMTable.nLocatePos=pFormatTbl->GetLocateStyle();
				//��¼���λ��
				pFormatTbl->GetBasePos(base_pos);
				m_paraBOMTable.fDimPosX=base_pos.x-inside_lefttop.x;
				m_paraBOMTable.fDimPosY=base_pos.y-inside_lefttop.y;
			}
			else 
			{	//�����������,Ԥ����λ��
				pFormatTbl->SetLocateStyle((TBL_LOCATE_STYLE)m_paraBOMTable.nLocatePos);	
				DrawTable(pBlockTableRec,pFormatTbl,FALSE);
			}
			m_paraBOMTable.fTableWidth=pFormatTbl->GetTableWide();
			m_paraBOMTable.fTableHeight=pFormatTbl->GetTableHeight();
			pBlockTableRec->close();
		}
	}
	else if(m_eCmdType==CMD_DIM_PARTSPEC)
	{	//��ע������ϸ
		if(m_paraPartSpec.pObj==NULL)
			return FALSE;
		if(m_paraPartSpec.pObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
			return FALSE;	//����ע��ϽǸֹ�����ϸ
		AcDbObjectId *pEntId=NULL;
		pEntId=m_pDrawingCom->keyEntIdList.GetValue(m_paraPartSpec.pObj->handle);
		if(pEntId)
		{
			BOOL bAnalysisMode=FALSE;	//�Ƿ�Ϊ��ѧ����ģʽ
			if(m_pDrawingCom->m_eClassType==DRAW_COM_LINEVIEW)	//����ͼ
			{
				CLineView *pLineView=(CLineView*)m_pDrawingCom;
				bAnalysisMode=pLineView->m_bAnalysisMode;
				if(bAnalysisMode)
				{	//��ѧ����ģʽ(˾��ͼ) wht 11-06-14
					sys.fonts.InitSpecialProp(2);
					if(!pLineView->m_bDimNodeNo&&m_paraPartSpec.pObj->IsNode())
						return FALSE;	//����ע�ڵ���
					if(!pLineView->m_bDimPoleGuige&&m_paraPartSpec.pObj->IsPart()
						&&((CLDSPart*)m_paraPartSpec.pObj)->IsLinePart())
						return FALSE;	//����ע�˼����
				}
			}
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			//���ݹ�����Ӧ��CADʵ��ID��ӹ�����ϸ��ע
			dimpartspec.SetBackMirStyle(m_paraPartSpec.iMirMsg);	//���ù�����ϸ��עʱ�ĶԳƷ�ʽ wht 11-03-29	
			CDimPartSpec *pPartSpec=AddSinglePartSpec(*pEntId,m_paraPartSpec.fPoleDimPosCoef,bAnalysisMode);
			if(pPartSpec==NULL)
				return FALSE;
			//���öԳƷ�ʽ
			pPartSpec->iMirMsg=m_paraPartSpec.iMirMsg;
			if(pPartSpec->iMirMsg==1)
			{	//Y��Գ�,���ݵ�ǰ���ż��ԳƽϺñ�עλ���ж�����
				AcDbObjectId *pMirEntId=m_pDrawingCom->keyEntIdList.GetValue(pPartSpec->hMirPart);
				if(pMirEntId)
				{
					CDimPartSpec *pMirPartSpec=AddSinglePartSpec(*pMirEntId,m_paraPartSpec.fPoleDimPosCoef,bAnalysisMode);
					if(pMirPartSpec)
					{
						if(pMirPartSpec->base_pos.x<pPartSpec->base_pos.x)
							pPartSpec->bLeftPart=FALSE;	//��ǰ����Ϊ�Ҳ๹��
						else 
							pPartSpec->bLeftPart=TRUE;	//��ǰ����Ϊ��๹��
					}
				}
			}
			//������������빹����ע����
			pPartSpec->bDrawRefLine=m_paraPartSpec.bDrawRefLine;	//����������
			if(m_paraPartSpec.iDimPosType==0)	//�Ƿ������Զ�����
				pPartSpec->bAutoSearchDimPos=TRUE;
			else 
				pPartSpec->bAutoSearchDimPos=FALSE;
			pPartSpec->bDimPartNoOnly=m_paraPartSpec.bOnlyDimPartNo;
			AcDbBlockTableRecord *pBlockTableRec=GetBlockTableRecord();
			//��ע����Ϊ�������׼���ʱ���㹹�����˼���X��н� wht 12-03-22
			bool insertPlateByRod=false;
			bool flByRod=false;
			if(sys.dim_map.PartNo.iStdInsertPlatePartNoFrameStyle==1||sys.dim_map.PartNo.iStdInsertPlatePartNoFrameStyle==2)
				insertPlateByRod=true;
			if(sys.dim_map.PartNo.iFlPartNoFrameStyle==1||sys.dim_map.PartNo.iFlPartNoFrameStyle==2)
				flByRod=true;
			bool dimSpecByRod=false;
			if(m_paraPartSpec.pObj->GetClassTypeId()==CLS_PARAMPLATE)
			{
				CLDSParamPlate *pPlate=(CLDSParamPlate*)m_paraPartSpec.pObj;
				if(pPlate->IsFL()&&flByRod)
					dimSpecByRod=true;
				else if(pPlate->IsInsertPlate()&&pPlate->m_bStdPart&&insertPlateByRod)
					dimSpecByRod=true;
			}
			bool bDimStartPartSpec=false;
			if(dimSpecByRod)
			{
				//DimPartSpecByDatumRod(pBlockTableRecord,this,pPartSpec);
				CLDSParamPlate *pParamPlate=(CLDSParamPlate*)m_paraPartSpec.pObj;
				CLDSLineTube *pDatumTube=(CLDSLineTube*)Ta.FromPartHandle(pParamPlate->m_hPartWeldParent);
				if(pDatumTube==NULL)
					return FALSE;
				if(pDatumTube->m_tJointStart.hLinkObj==pParamPlate->handle)
					bDimStartPartSpec=true;	//����ʼ�˲�
				AcDbEntity *pRodEnt=NULL;
				AcDbObjectId *pId=m_pDrawingCom->keyEntIdList.GetValue(pParamPlate->m_hPartWeldParent);
				if(pId)
				{	
					acdbOpenObject(pRodEnt,*pId,AcDb::kForRead);
					if(pRodEnt)
						pRodEnt->close();
				}
				if(pRodEnt&&pRodEnt->isKindOf(AcDbLine::desc()))
				{
					AcDbLine *pLine=(AcDbLine*)pRodEnt;
					pPartSpec->rod_angle=Cal2dLineAng(pLine->startPoint().x,pLine->startPoint().y,
						pLine->endPoint().x,pLine->endPoint().y);
					Sub_Pnt(pPartSpec->rod_vec,pLine->endPoint(),pLine->startPoint());
					normalize(pPartSpec->rod_vec);
				}
			}
			if(m_paraPartSpec.iDimPosType==0)
			{	//��������עָ������
				dimpartspec.AnalyseAndDimPartSpec(pBlockTableRec,pPartSpec,bAnalysisMode,bDimStartPartSpec);
				m_paraPartSpec.iDimPosType=1;
				m_paraPartSpec.fDimPosOffsetX=pPartSpec->fDimPosOffsetX;
				m_paraPartSpec.fDimPosOffsetY=pPartSpec->fDimPosOffsetY;
				m_paraPartSpec.fGuigeOffsetX=pPartSpec->fGuigeOffsetX;
				m_paraPartSpec.fGuigeOffsetY=pPartSpec->fGuigeOffsetY;
			}
			else
			{	//�Զ���עָ������
				pPartSpec->fBasePosOffsetX=m_paraPartSpec.fBasePosOffsetX;
				pPartSpec->fBasePosOffsetY=m_paraPartSpec.fBasePosOffsetY;
				pPartSpec->fDimPosOffsetX=m_paraPartSpec.fDimPosOffsetX;
				pPartSpec->fDimPosOffsetY=m_paraPartSpec.fDimPosOffsetY;
				pPartSpec->fGuigeOffsetX=m_paraPartSpec.fGuigeOffsetX;
				pPartSpec->fGuigeOffsetY=m_paraPartSpec.fGuigeOffsetY;
				dimpartspec.AutoDimPartSpec(pBlockTableRec,pPartSpec,bAnalysisMode,m_paraPartSpec.iDimPosType==2,bDimStartPartSpec);
				m_paraPartSpec.iDimPosType=1;
				m_paraPartSpec.fDimPosOffsetX=pPartSpec->fDimPosOffsetX;
				m_paraPartSpec.fDimPosOffsetY=pPartSpec->fDimPosOffsetY;
				m_paraPartSpec.fGuigeOffsetX=pPartSpec->fGuigeOffsetX;
				m_paraPartSpec.fGuigeOffsetY=pPartSpec->fGuigeOffsetY;
			}
			pBlockTableRec->close();
		}
	}
	else if(m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)
	{	//��ע�Ŷ����
		if(m_paraDimFootNailSpace.pLinePart==NULL)
			return FALSE;
		AcDbObjectId *pEntId=NULL;
		pEntId=m_pDrawingCom->keyEntIdList.GetValue(m_paraDimFootNailSpace.pLinePart->handle);
		if(pEntId)
		{
			GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
			AcDbBlockTableRecord *pBlockTableRec=GetBlockTableRecord();
			UCS_STRU cs=dcs.cs;
			DimSinglePoleFootNailSpace(pBlockTableRec,*pEntId,m_paraDimFootNailSpace.fSpace,
				m_paraDimFootNailSpace.fDimDistance,FALSE,m_pDrawingCom->m_fScale,&cs);
			pBlockTableRec->close();
		}
	}
	else if(m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
	{	//��ע�����
		if(m_paraDimFillPlateSpace.pLinePart==NULL)
			return FALSE;
		AcDbObjectId *pEntId=NULL;
		pEntId=m_pDrawingCom->keyEntIdList.GetValue(m_paraDimFillPlateSpace.pLinePart->handle);
		if(pEntId)
		{
			GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
			AcDbBlockTableRecord *pBlockTableRec=GetBlockTableRecord();
			UCS_STRU cs=dcs.cs;
			DimSingleGroupLineAngleFillPlateSpace(pBlockTableRec,*pEntId,m_paraDimFillPlateSpace.fSpace,
				m_paraDimFillPlateSpace.fDimDistance,FALSE,m_pDrawingCom->m_fScale,&cs);
			pBlockTableRec->close();
		}
	}
	else if(m_eCmdType==CMD_DIM_SIMPOLY)
	{
		if(m_paraSimPoly.nSimPolyCmdType==0)
		{	//������������κ���
			AddDimSimPolyCmd(m_pDrawingCom);
			//m_paraSimPoly.nSimPolyCmdType=2;
			m_bExecute=FALSE; //�����ø�����
		}
		else if(m_paraSimPoly.nSimPolyCmdType==2) 
			ExecuteDimSimPolyCmdByPickPt(this);		//ͨ��ʰȡ���ע������
		else if(m_paraSimPoly.nSimPolyCmdType==3)
			ExecuteDimSimPolyCmdByVertextList(this);//ͨ���������ע������
	}
	else if(m_eCmdType==CMD_DIM_MODELSIZE)
		DimModelSize(this,dcs,m_pDrawingCom->m_fScale);
	else if(m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)
		DimJgZhun(this);
	else if(m_eCmdType==CMD_DIM_BOLTPAD)
		DimBoltPad(this);		//��ע��˨��Ȧ
	else if(m_eCmdType==CMD_DIM_BOLTINFO)
		DimBoltInfo(this);		//��ע��˨��Ϣ wht 12-03-21
	else if(m_eCmdType==CMD_DRAW_ASSIST_LINE)
		DrawAssistLine(this,dcs);//���Ƹ�����
	else if(m_eCmdType==CMD_DRAW_JG_JOINT)
		DrawJgJointSideView(this);
	if(DRAGSET.GetClearDegree()>0)
		nOldEntCount=0;	//��ק������ִ������Ĺ����б���չ�
	int i=0;
	//���ʵ��ID�б�
	entIdList.Empty();
	for(i=nOldEntCount;i<DRAGSET.GetEntNum();i++)
	{
		AcDbObjectId ent_id;
		if(DRAGSET.GetEntId(i,ent_id))
			entIdList.append(ent_id);
	}
	return 1;	//ִ����������˳�
}

int CDrawingCommand::Rerun()	//�������и�����
{
	if(entIdList.GetNodeNum()>0)
	{	//����ִ������ǰӦ���ñ�ע״̬�����򲻻�ִ�е�ǰ����
		if(m_eCmdType==CMD_DIM_ANGLEJOINT)
			m_pDrawingCom->MarkItemDimSpecState(CDrawingComponent::TYPE_JOINT,m_paraAngleJoint.idAngleJoint,false);
	}
	EmptyEntList();		//ɾ���������ɵ�ԭ��ʵ��
	int nRetCode=Run();	//��������
	//ˢ�½���
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();
	return nRetCode;
}

void CDrawingCommand::FromBuffer(CBuffer &buffer, long version/*=NULL*/)
{
	BYTE b=0;
	buffer.ReadByte(&b);	//��������
	m_eCmdType=(DRAW_CMD_TYPE)b;
	buffer.ReadByte(&m_stateCmd);	//����״̬
	buffer.ReadInteger(&m_bExecute);//�Ƿ�ִ�и�����
	//�������
	if(m_eCmdType==CMD_DRAW_PART)
	{	//���ƹ���
		buffer.ReadInteger(&m_paraPart.hPart);
		m_paraPart.pPart=Ta.FromPartHandle(m_paraPart.hPart);
		if(m_paraPart.pPart==NULL)
			m_bExecute=false;	//�ù����ѱ�ɾ�������๹���Ժ�Ӧֱ��ɾ������δ������wjh-2011.9.26
		buffer.ReadDouble(&m_paraPart.fPoleAxisLineStartOutSpreadLen);	//�˼�����ʼ�����쳤��
		buffer.ReadDouble(&m_paraPart.fPoleAxisLineEndOutSpreadLen);		//�˼������ն����쳤��
		if(version<1000008) 
		{	//չ�������� 0.��ʾ��չ���� i-1��ʾչ������չ������չ���б��е�ֵ
			int iSpreadType=0;
			buffer.ReadInteger(&iSpreadType);	
		}
		buffer.ReadByte(&m_paraPart.cDrawFlag);
		//�Ǹֻ�׼�ľ�
		buffer.ReadInteger(&m_paraPart.para.angle.nXWingDatumAxisG);
		buffer.ReadInteger(&m_paraPart.para.angle.nYWingDatumAxisG);
		if(version>=1000011)
			buffer.ReadInteger(&m_paraPart.para.angle.bDrawAxisLine);
		if(version>=1000012)
			buffer.ReadWord(&m_paraPart.para.angle.extendStyle);
		else
			m_paraPart.para.angle.bDrawAxisLine=TRUE;
		//��˨��ػ��Ʋ���
		buffer.ReadInteger(&m_paraPart.para.bolt.bOriginal);
		buffer.ReadInteger(&m_paraPart.para.bolt.front0_side1);
		if(version>=1000014)
			buffer.ReadBooleanThin(&m_paraPart.para.bolt.bDrawSideBoltLine);
		if(version>=1000015)
		{
			buffer.ReadBooleanThin(&m_paraPart.para.bolt.bDimHoleDiameter);
			if(m_paraPart.para.bolt.bDimHoleDiameter)
			{
				buffer.ReadWord(&m_paraPart.para.bolt.dimAngle);
				buffer.ReadWord(&m_paraPart.para.bolt.dimFarOffset);
			}
		}
		else
			m_paraPart.para.bolt.bDimHoleDiameter=false;
		if(version<1000008)	//�°汾���ٱ�����˨λ�á���˨���߼�չ������ת�Ƕȣ��˴�ƫ��13*8���ֽ� wht 11-08-08
			buffer.Offset(13*8);
		if(version>=1000016)	//1000016���Ժ�汾���ȡ��������˳��� wht 12-05-25	
			buffer.ReadInteger(&m_nDrawingSerialNo);
	}
	else if(m_eCmdType==CMD_DRAW_LINEPART)
	{	//���Ƹ˼�����ͼ
		buffer.ReadInteger(&m_paraLinePart.hLinePart);
		m_paraLinePart.pLinePart=(CLDSLinePart*)Ta.FromPartHandle(m_paraLinePart.hLinePart,CLS_LINEPART);
		if(m_paraLinePart.pLinePart&&m_paraLinePart.pLinePart->pStart&&m_paraLinePart.pLinePart->pEnd)
		{	//���ñ�־λ����ڵ��ظ�����
			m_paraLinePart.pLinePart->pStart->is_visible=FALSE;
			m_paraLinePart.pLinePart->pEnd->is_visible=FALSE;		
		}
		buffer.ReadInteger(&m_paraLinePart.bSumMap);
		buffer.ReadInteger(&m_paraLinePart.bDimByAnalysisMode);
	}
	else if(m_eCmdType==CMD_DRAW_TABLE)
	{	//���Ʊ��
		buffer.ReadInteger(&m_paraBOMTable.nLocatePos);	//0.���� 1.���� 2.���� 4.����
		buffer.ReadInteger(&m_paraBOMTable.bSelectPos);	//ѡ����λ��
		buffer.ReadInteger(&m_paraBOMTable.bDisplayDlg);//��ʾ�Ի���
		buffer.ReadDouble(&m_paraBOMTable.fDimPosX);	//���λ��X����		
		buffer.ReadDouble(&m_paraBOMTable.fDimPosY);	//���λ��Y����
		buffer.ReadInteger(&m_paraBOMTable.iTableType);	//������� 
	}
	else if(m_eCmdType==CMD_DIM_PARTSPEC)
	{	//��ע������ϸ
		buffer.ReadInteger(&m_paraPartSpec.hObj);
		m_paraPartSpec.pObj=(CLDSDbObject*)Ta.FromHandle(m_paraPartSpec.hObj,TRUE);
		if(version>1000004)
		{	//������ϸ��ע����ƫ���� wht 11-07-25
			buffer.ReadDouble(&m_paraPartSpec.fBasePosOffsetX);	//���ݵ�λ��X����ƫ��ֵ
			buffer.ReadDouble(&m_paraPartSpec.fBasePosOffsetY);	//���ݵ�λ��Y����ƫ��ֵ
		}
		buffer.ReadDouble(&m_paraPartSpec.fDimPosOffsetX);	//��עλ��X����ƫ��ֵ
		buffer.ReadDouble(&m_paraPartSpec.fDimPosOffsetY);	//��עλ��Y����ƫ��ֵ
		buffer.ReadDouble(&m_paraPartSpec.fGuigeOffsetX);	//����עλ��X����ƫ��ֵ
		buffer.ReadDouble(&m_paraPartSpec.fGuigeOffsetY);	//����עλ��Y����ƫ��ֵ
		buffer.ReadInteger(&m_paraPartSpec.iDimPosType);	//0.�Զ�����λ�� 1.ָ��λ��(����ָ��λ����ԭ��עλ�õ�ƫ��ֵ)
		if(version>1000005)
			buffer.ReadInteger(&m_paraPartSpec.bOnlyDimPartNo);	//����ע���
		else 
		{
			BOOL bDimGuiGe=FALSE,bDimJgLength=FALSE;
			buffer.ReadInteger(&bDimGuiGe);		//��ע�������
			buffer.ReadInteger(&bDimJgLength);	//��ע�Ǹֳ���
			if(!bDimGuiGe&&!bDimJgLength)
				m_paraPartSpec.bOnlyDimPartNo=TRUE;
			else 
				m_paraPartSpec.bOnlyDimPartNo=FALSE;
		}
		buffer.ReadInteger(&m_paraPartSpec.bDrawRefLine);	//�Ƿ����������
		buffer.ReadDouble(&m_paraPartSpec.fPoleDimPosCoef);	//�˼���עλ��ϵ�� //LTMA 0.3  TAS 0.5
		buffer.ReadInteger(&m_paraPartSpec.iMirMsg);		//0:X��Գƣ�1:Y��Գ�;���಻�Գƴ���
	}
	else if(m_eCmdType==CMD_DIM_SEGI)
	{	//��ע�κ�
		buffer.ReadInteger(&m_paraDimSegI.bSelectPos);
		buffer.ReadInteger(&(m_paraDimSegI.nSegI.iSeg));
		buffer.ReadDouble(&m_paraDimSegI.fDimPosX);
		buffer.ReadDouble(&m_paraDimSegI.fDimPosY);
	}
	else if(m_eCmdType==CMD_DIM_ODDMENT)
	{	//��ע�˼�����ͷ
		buffer.ReadInteger(&m_paraOddment.hLinePart);
		m_paraOddment.pLinePart=(CLDSLinePart*)Ta.FromPartHandle(m_paraOddment.hLinePart,CLS_LINEPART);
		if(version>1000004)
		{	//����ͷλ�ü��˼���б�Ƕ� wht 11-07-26
			buffer.ReadInteger(&m_paraOddment.iDimPosType);
			buffer.ReadDouble(&m_paraOddment.fPoleAngle);
			buffer.ReadDouble(&m_paraOddment.fStartOddPosOffsetX);
			buffer.ReadDouble(&m_paraOddment.fStartOddPosOffsetY);
			buffer.ReadDouble(&m_paraOddment.fEndOddPosOffsetX);
			buffer.ReadDouble(&m_paraOddment.fEndOddPosOffsetY);
			buffer.ReadInteger(&m_paraOddment.bDimStartOdd);
			buffer.ReadInteger(&m_paraOddment.bDimEndOdd);
		}
	}
	else if(m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)
	{	//��ע�˼��Ŷ����
		buffer.ReadInteger(&m_paraDimFootNailSpace.hLinePart);
		m_paraDimFootNailSpace.pLinePart=(CLDSLinePart*)Ta.FromPartHandle(m_paraDimFootNailSpace.hLinePart,CLS_LINEPART);
		buffer.ReadDouble(&m_paraDimFootNailSpace.fDimDistance);
		buffer.ReadDouble(&m_paraDimFootNailSpace.fSpace);
	}
	else if(m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
	{	//��ע�˼������
		buffer.ReadInteger(&m_paraDimFillPlateSpace.hLinePart);
		m_paraDimFillPlateSpace.pLinePart=(CLDSLinePart*)Ta.FromPartHandle(m_paraDimFillPlateSpace.hLinePart,CLS_LINEPART);
		buffer.ReadDouble(&m_paraDimFillPlateSpace.fDimDistance);
		buffer.ReadDouble(&m_paraDimFillPlateSpace.fSpace);
	}
	else if(m_eCmdType==CMD_DIM_SIMPOLY)
	{	//��ע������
		buffer.ReadInteger(&m_paraSimPoly.nSimPolyCmdType);	//��������������
		buffer.ReadDouble(&m_paraSimPoly.fScale);			//ָ�������α���
		buffer.ReadDouble(&m_paraSimPoly.fPickPtX);			//������ʰȡ��X����		
		buffer.ReadDouble(&m_paraSimPoly.fPickPtY);			//������ʰȡ��Y����
		buffer.ReadDouble(&m_paraSimPoly.fDimPosOffsetX);	//�����α�עλ��X�᷽��ƫ����
		buffer.ReadDouble(&m_paraSimPoly.fDimPosOffsetY);	//�����α�עλ��Y�᷽��ƫ����
		if(version==0||version>=1000024)
			buffer.ReadBooleanThin(&m_paraSimPoly.bDrawRefLine);
		//������������
		long i=0,nn=0;
		buffer.ReadInteger(&nn);
		m_paraSimPoly.simPolyVertexList.Empty();
		for(i=0;i<nn;i++)
		{
			SIMPOLY_VERTEX *pVertex=m_paraSimPoly.simPolyVertexList.append();
			buffer.ReadInteger(&pVertex->hDatumRod);
			buffer.ReadInteger(&pVertex->hViceRod);
			if(version>1000003)
			{	//��ȡ���������� wht 11-06-29
				buffer.ReadDouble(&pVertex->vertex.x);
				buffer.ReadDouble(&pVertex->vertex.y);
			}
		}
	}
	else if(m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
	{	//��ע������˨���
		buffer.ReadInteger(&m_paraDimLsSpace.hPlate);
		m_paraDimLsSpace.pPlate=Ta.FromPartHandle(m_paraDimLsSpace.hPlate);
		buffer.ReadDouble(&m_paraDimLsSpace.fSpace);
		buffer.ReadDouble(&m_paraDimLsSpace.fDimDistance);
		if(version>1000002)
		{	//��˨������ V1000003 wht 11-06-20
			buffer.ReadInteger(&m_paraDimLsSpace.spaceDim.hStartLs);
			buffer.ReadInteger(&m_paraDimLsSpace.spaceDim.hEndLs);
			buffer.ReadDouble(&m_paraDimLsSpace.spaceDim.dist);
			m_paraDimLsSpace.spaceDim.hFatherPlate=m_paraDimLsSpace.hPlate;
		}
		if(version>1000003)
			buffer.ReadInteger(&m_paraDimLsSpace.bInside);
		if(version>=1000020)
		{
			buffer.ReadInteger(&m_paraDimLsSpace.spaceDim.nLsSpaceNum);
			buffer.ReadInteger(&m_paraDimLsSpace.spaceDim.bMergeDimLsSpace);
		}
		if(version>=1000023)
		{	//��ӱ�ע�Ƕ� v1000023 wht 17-08-17
			buffer.ReadInteger(&m_paraDimLsSpace.spaceDim.bCalDimAngle);
			buffer.ReadDouble(&m_paraDimLsSpace.spaceDim.dim_angle);
		}
	}
	else if(m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)
	{	//��ע�Ǹ�����׼��
		buffer.ReadInteger(&m_paraDimJgZhun.hLineAngle);
		m_paraDimJgZhun.pLineAngle=(CLDSLineAngle*)Ta.FromPartHandle(m_paraDimJgZhun.hLineAngle,CLS_LINEANGLE);
		if(version>1000008)
		{	// v1000009����Ǹ�׼�ݲ��� wht 11-08-09
			buffer.ReadInteger(&m_paraDimJgZhun.bInside);
			buffer.ReadDouble(&m_paraDimJgZhun.fSpace);
			buffer.ReadDouble(&m_paraDimJgZhun.fDimDistance);
		}
		else //X��Y����ƫ����,��ɾ���˴�ƫ��2*8���ֽ� wht 11-08-09
		{
			buffer.Offset(2*8);	
			m_paraDimJgZhun.bInside=TRUE;
			m_paraDimJgZhun.fSpace=0;
			m_paraDimJgZhun.fDimDistance=6;
		}
		if(version>1000003)
		{	//��ȡ�Ǹ�׼�ݱ�ע���� wht 11-06-30
			buffer.ReadPoint(m_paraDimJgZhun.zhun_dim.lineStart);
			buffer.ReadPoint(m_paraDimJgZhun.zhun_dim.lineEnd);
			buffer.ReadPoint(m_paraDimJgZhun.zhun_dim.dimStartPos);
			buffer.ReadPoint(m_paraDimJgZhun.zhun_dim.dimEndPos);
			buffer.ReadPoint(m_paraDimJgZhun.zhun_dim.dimPos);
			buffer.ReadDouble(&m_paraDimJgZhun.zhun_dim.dist);
			if(version>=1000011)
				buffer.ReadBoolean(&m_paraDimJgZhun.zhun_dim.bDimZhunJu);
		}
		if(version>1000009)	//��ȡ�Ǹ�׼�ݹ������ wht 11-08-10
			buffer.ReadInteger(&m_paraDimJgZhun.zhun_dim.hRelaBolt);
	}
	else if(m_eCmdType==CMD_DIM_MODELSIZE)
	{	//��עģ�ͳߴ�
		buffer.ReadInteger(&m_paraDimModelSize.bUpDim);
		buffer.ReadDouble(&m_paraDimModelSize.fDimDistance);
		buffer.ReadDouble(&m_paraDimModelSize.fSpace);
		buffer.ReadDouble(&m_paraDimModelSize.fMinX);
		buffer.ReadInteger(&m_paraDimModelSize.iDimType);
		m_paraDimModelSize.listDimNodes.Empty();
		long i=0,nNodeNum=0,hNode=0;
		buffer.ReadInteger(&nNodeNum);
		for(i=0;i<nNodeNum;i++)
		{
			buffer.ReadInteger(&hNode);
			CLDSNode *pNode=hNode>0x20?Ta.FromNodeHandle(hNode):NULL;
			m_paraDimModelSize.AppendDimNode(pNode);
		}
	}
	else if(m_eCmdType==CMD_DIM_MAPSIZE)
	{	//��עͼ��ߴ�
		buffer.ReadDouble(&m_paraDimSize.fDimStartX);
		buffer.ReadDouble(&m_paraDimSize.fDimStartY);
		buffer.ReadDouble(&m_paraDimSize.fDimEndX);
		buffer.ReadDouble(&m_paraDimSize.fDimEndY);
		buffer.ReadDouble(&m_paraDimSize.fDimPosX);
		buffer.ReadDouble(&m_paraDimSize.fDimPosY);
		buffer.ReadDouble(&m_paraDimSize.fScale);		//�ߴ����
		buffer.ReadInteger(&m_paraDimSize.iDimDirect);	//��ע���� 0.X�᷽�� 1.Y�᷽�� 2.���췽��
	}
	else if(m_eCmdType==CMD_DIM_BOLTPAD)
	{	//��ע��Ȧ��Ϣ
		buffer.ReadInteger(&m_paraBoltPad.hBolt);
		m_paraBoltPad.pBolt=(CLDSBolt*)Ta.FromPartHandle(m_paraBoltPad.hBolt);
		buffer.ReadDouble(&m_paraBoltPad.fDimPosOffsetX);//��עλ��X����ƫ��ֵ
		buffer.ReadDouble(&m_paraBoltPad.fDimPosOffsetY);//��עλ��Y����ƫ��ֵ
		if(version>1000006)	//��Ȧ��ע���� wht 11-08-02
			buffer.ReadInteger(&m_paraBoltPad.iDimPosType);
	}
	else if(m_eCmdType==CMD_DIM_BOLTINFO)
	{	//��ע��˨��Ϣ wht 12-03-21
		m_paraBoltGroupInfo.lsGroupDim.FromBuffer(buffer,version);
		buffer.ReadInteger(&m_paraBoltGroupInfo.iDimPosType);
		buffer.ReadDouble(&m_paraBoltGroupInfo.fDimPosOffsetX);
		buffer.ReadDouble(&m_paraBoltGroupInfo.fDimPosOffsetY);
	}
	else if(m_eCmdType==CMD_DRAW_ASSIST_LINE)
	{	//������
		buffer.ReadInteger(&m_paraDrawAssistLine.iNo);
		buffer.ReadInteger(&m_paraDrawAssistLine.nStyle);
	}
	else if(m_eCmdType==CMD_DIM_BOLTORFL_INFO)
	{	//��ע�˼���˨��Ϣ������Ϣ wht 12-06-01
		buffer.ReadInteger(&m_paraBoltOrFlInfo.hLinePart);
		m_paraBoltOrFlInfo.pLinePart=(CLDSLinePart*)Ta.FromPartHandle(m_paraBoltOrFlInfo.hLinePart,CLS_LINEPART);
		buffer.ReadInteger(&m_paraBoltOrFlInfo.iDimPosType);
		buffer.ReadDouble(&m_paraBoltOrFlInfo.fPoleAngle);
		buffer.ReadDouble(&m_paraBoltOrFlInfo.fStartOddPosOffsetX);
		buffer.ReadDouble(&m_paraBoltOrFlInfo.fStartOddPosOffsetY);
		buffer.ReadDouble(&m_paraBoltOrFlInfo.fEndOddPosOffsetX);
		buffer.ReadDouble(&m_paraBoltOrFlInfo.fEndOddPosOffsetY);
		buffer.ReadInteger(&m_paraBoltOrFlInfo.bDimStartOdd);
		buffer.ReadInteger(&m_paraBoltOrFlInfo.bDimEndOdd);
	}
	else if(m_eCmdType==CMD_DIM_ANGLEJOINT)
	{	//��ע�Ǹֽ�ͷ��Ϣ wht 14-07-08
		buffer.ReadInteger(&m_paraAngleJoint.idAngleJoint);
		m_paraAngleJoint.pJoint=Ta.hashAngleJoint.GetValue(m_paraAngleJoint.idAngleJoint);
		buffer.ReadInteger(&m_paraAngleJoint.iDimPosType);
		buffer.ReadDouble(&m_paraAngleJoint.fDimPosOffsetX);
		buffer.ReadDouble(&m_paraAngleJoint.fDimPosOffsetY);
	}
}

void CDrawingCommand::ToBuffer(CBuffer &buffer)
{
	buffer.WriteByte(m_eCmdType);	//��������
	buffer.WriteByte(m_stateCmd);	//����״̬
	buffer.WriteInteger(m_bExecute);//�Ƿ�ִ�и�����
	//�������
	if(m_eCmdType==CMD_DRAW_PART)//����Ӧȷ���Ƿ���Ҫ�Ż�,Ŀǰ���ֺ�ʱ1.642sʱ��ϳ�,wjh-2011.8.6
	{	//���ƹ���
		buffer.WriteInteger(m_paraPart.hPart);
		buffer.WriteDouble(m_paraPart.fPoleAxisLineStartOutSpreadLen);	//�˼�����ʼ�����쳤��
		buffer.WriteDouble(m_paraPart.fPoleAxisLineEndOutSpreadLen);	//�˼������ն����쳤��
		buffer.WriteByte(m_paraPart.cDrawFlag);
		//�Ǹֻ�׼�ľ�
		buffer.WriteInteger(m_paraPart.para.angle.nXWingDatumAxisG);
		buffer.WriteInteger(m_paraPart.para.angle.nYWingDatumAxisG);
		buffer.WriteInteger(m_paraPart.para.angle.bDrawAxisLine);
		buffer.WriteWord(m_paraPart.para.angle.extendStyle);
		//��˨��ػ��Ʋ���
		buffer.WriteInteger(m_paraPart.para.bolt.bOriginal);
		buffer.WriteInteger(m_paraPart.para.bolt.front0_side1);
		buffer.WriteBooleanThin(m_paraPart.para.bolt.bDrawSideBoltLine);
		buffer.WriteBooleanThin(m_paraPart.para.bolt.bDimHoleDiameter);
		if(m_paraPart.para.bolt.bDimHoleDiameter)
		{
			buffer.WriteWord(m_paraPart.para.bolt.dimAngle);
			buffer.WriteWord(m_paraPart.para.bolt.dimFarOffset);
		}
		//1000016���Ժ�汾���湹������˳��� wht 12-05-25	
		buffer.WriteInteger(m_nDrawingSerialNo);
	}
	else if(m_eCmdType==CMD_DRAW_LINEPART)
	{	//���Ƹ˼�����ͼ
		buffer.WriteInteger(m_paraLinePart.hLinePart);
		buffer.WriteInteger(m_paraLinePart.bSumMap);
		buffer.WriteInteger(m_paraLinePart.bDimByAnalysisMode);
	}
	else if(m_eCmdType==CMD_DRAW_TABLE)
	{	//���Ʊ��
		buffer.WriteInteger(m_paraBOMTable.nLocatePos);	//0.���� 1.���� 2.���� 4.����
		buffer.WriteInteger(m_paraBOMTable.bSelectPos);	//ѡ����λ��
		buffer.WriteInteger(m_paraBOMTable.bDisplayDlg);//��ʾ�Ի���
		buffer.WriteDouble(m_paraBOMTable.fDimPosX);	//���λ��X����		
		buffer.WriteDouble(m_paraBOMTable.fDimPosY);	//���λ��Y����
		buffer.WriteInteger(m_paraBOMTable.iTableType);	//������� 
	}
	else if(m_eCmdType==CMD_DIM_PARTSPEC)
	{	//��ע������ϸ
		buffer.WriteInteger(m_paraPartSpec.hObj);
		buffer.WriteDouble(m_paraPartSpec.fBasePosOffsetX);	//���ݵ�λ��X����ƫ��ֵ
		buffer.WriteDouble(m_paraPartSpec.fBasePosOffsetY);	//���ݵ�λ��Y����ƫ��ֵ
		buffer.WriteDouble(m_paraPartSpec.fDimPosOffsetX);	//��עλ��X����ƫ��ֵ
		buffer.WriteDouble(m_paraPartSpec.fDimPosOffsetY);	//��עλ��Y����ƫ��ֵ
		buffer.WriteDouble(m_paraPartSpec.fGuigeOffsetX);	//����עλ��X����ƫ��ֵ
		buffer.WriteDouble(m_paraPartSpec.fGuigeOffsetY);	//����עλ��Y����ƫ��ֵ
		buffer.WriteInteger(m_paraPartSpec.iDimPosType);	//0.�Զ�����λ�� 1.ָ��λ��(����ָ��λ����ԭ��עλ�õ�ƫ��ֵ)
		buffer.WriteInteger(m_paraPartSpec.bOnlyDimPartNo);	//�����Ƹ˼����
		buffer.WriteInteger(m_paraPartSpec.bDrawRefLine);	//�Ƿ����������
		buffer.WriteDouble(m_paraPartSpec.fPoleDimPosCoef);	//�˼���עλ��ϵ�� //LTMA 0.3  TAS 0.5
		buffer.WriteInteger(m_paraPartSpec.iMirMsg);		//0:X��Գƣ�1:Y��Գ�;���಻�Գƴ���
	}
	else if(m_eCmdType==CMD_DIM_SEGI)
	{	//��ע�κ�
		buffer.WriteInteger(m_paraDimSegI.bSelectPos);
		buffer.WriteInteger(m_paraDimSegI.nSegI);
		buffer.WriteDouble(m_paraDimSegI.fDimPosX);
		buffer.WriteDouble(m_paraDimSegI.fDimPosY);
	}
	else if(m_eCmdType==CMD_DIM_ODDMENT)
	{	//��ע�˼�����ͷ
		buffer.WriteInteger(m_paraOddment.hLinePart);
		//����ͷλ�ü��˼���б�Ƕ� wht 11-07-26
		buffer.WriteInteger(m_paraOddment.iDimPosType);
		buffer.WriteDouble(m_paraOddment.fPoleAngle);
		buffer.WriteDouble(m_paraOddment.fStartOddPosOffsetX);
		buffer.WriteDouble(m_paraOddment.fStartOddPosOffsetY);
		buffer.WriteDouble(m_paraOddment.fEndOddPosOffsetX);
		buffer.WriteDouble(m_paraOddment.fEndOddPosOffsetY);
		buffer.WriteInteger(m_paraOddment.bDimStartOdd);
		buffer.WriteInteger(m_paraOddment.bDimEndOdd);
	}
	else if(m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)
	{	//��ע�˼��Ŷ����
		buffer.WriteInteger(m_paraDimFootNailSpace.hLinePart);
		buffer.WriteDouble(m_paraDimFootNailSpace.fDimDistance);
		buffer.WriteDouble(m_paraDimFootNailSpace.fSpace);
	}
	else if(m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
	{	//��ע�˼������
		buffer.WriteInteger(m_paraDimFillPlateSpace.hLinePart);
		buffer.WriteDouble(m_paraDimFillPlateSpace.fDimDistance);
		buffer.WriteDouble(m_paraDimFillPlateSpace.fSpace);
	}
	else if(m_eCmdType==CMD_DIM_SIMPOLY)
	{	//��ע������
		buffer.WriteInteger(m_paraSimPoly.nSimPolyCmdType);	//��������������
		buffer.WriteDouble(m_paraSimPoly.fScale);			//ָ�������α���
		buffer.WriteDouble(m_paraSimPoly.fPickPtX);			//������ʰȡ��X����		
		buffer.WriteDouble(m_paraSimPoly.fPickPtY);			//������ʰȡ��Y����
		buffer.WriteDouble(m_paraSimPoly.fDimPosOffsetX);	//�����α�עλ��X�᷽��ƫ����
		buffer.WriteDouble(m_paraSimPoly.fDimPosOffsetY);	//�����α�עλ��X�᷽��ƫ����
		buffer.WriteBooleanThin(m_paraSimPoly.bDrawRefLine);
		//������������
		long nn=m_paraSimPoly.simPolyVertexList.GetNodeNum();
		buffer.WriteInteger(nn);
		SIMPOLY_VERTEX *pVertex=NULL;
		for(pVertex=m_paraSimPoly.simPolyVertexList.GetFirst();pVertex;
		pVertex=m_paraSimPoly.simPolyVertexList.GetNext())
		{
			buffer.WriteInteger(pVertex->hDatumRod);
			buffer.WriteInteger(pVertex->hViceRod);
			//�������������� wht 11-06-29
			buffer.WriteDouble(pVertex->vertex.x);	
			buffer.WriteDouble(pVertex->vertex.y);
		}
	}
	else if(m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
	{	//��ע������˨���
		buffer.WriteInteger(m_paraDimLsSpace.hPlate);
		buffer.WriteDouble(m_paraDimLsSpace.fSpace);
		buffer.WriteDouble(m_paraDimLsSpace.fDimDistance);
		buffer.WriteInteger(m_paraDimLsSpace.spaceDim.hStartLs);
		buffer.WriteInteger(m_paraDimLsSpace.spaceDim.hEndLs);
		buffer.WriteDouble(m_paraDimLsSpace.spaceDim.dist);
		buffer.WriteInteger(m_paraDimLsSpace.bInside);
		buffer.WriteInteger(m_paraDimLsSpace.spaceDim.nLsSpaceNum);
		buffer.WriteInteger(m_paraDimLsSpace.spaceDim.bMergeDimLsSpace);
		buffer.WriteInteger(m_paraDimLsSpace.spaceDim.bCalDimAngle);
		buffer.WriteDouble(m_paraDimLsSpace.spaceDim.dim_angle);
	}
	else if(m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)
	{	//��ע�Ǹ�����׼��
		buffer.WriteInteger(m_paraDimJgZhun.hLineAngle);
		buffer.WriteInteger(m_paraDimJgZhun.bInside);
		buffer.WriteDouble(m_paraDimJgZhun.fSpace);
		buffer.WriteDouble(m_paraDimJgZhun.fDimDistance);
		//����Ǹ�׼�ݱ�ע���� wht 11-06-30
		buffer.WritePoint(m_paraDimJgZhun.zhun_dim.lineStart);
		buffer.WritePoint(m_paraDimJgZhun.zhun_dim.lineEnd);
		buffer.WritePoint(m_paraDimJgZhun.zhun_dim.dimStartPos);
		buffer.WritePoint(m_paraDimJgZhun.zhun_dim.dimEndPos);
		buffer.WritePoint(m_paraDimJgZhun.zhun_dim.dimPos);
		buffer.WriteDouble(m_paraDimJgZhun.zhun_dim.dist);
		buffer.WriteBoolean(m_paraDimJgZhun.zhun_dim.bDimZhunJu);
		//����Ǹ�׼�ݹ������ wht 11-08-10
		buffer.WriteInteger(m_paraDimJgZhun.zhun_dim.hRelaBolt);
	}
	else if(m_eCmdType==CMD_DIM_MODELSIZE)
	{	//ģ�ͳߴ�
		buffer.WriteInteger(m_paraDimModelSize.bUpDim);
		buffer.WriteDouble(m_paraDimModelSize.fDimDistance);
		buffer.WriteDouble(m_paraDimModelSize.fSpace);
		buffer.WriteDouble(m_paraDimModelSize.fMinX);
		buffer.WriteInteger(m_paraDimModelSize.iDimType);
		buffer.WriteInteger(m_paraDimModelSize.listDimNodes.GetNodeNum());
		for(DIMNODE *pDimNode=m_paraDimModelSize.listDimNodes.GetFirst();pDimNode;pDimNode=m_paraDimModelSize.listDimNodes.GetNext())
			buffer.WriteInteger(pDimNode->hNode);
	}
	else if(m_eCmdType==CMD_DIM_MAPSIZE)
	{	//��עͼ��ߴ�
		buffer.WriteDouble(m_paraDimSize.fDimStartX);
		buffer.WriteDouble(m_paraDimSize.fDimStartY);
		buffer.WriteDouble(m_paraDimSize.fDimEndX);
		buffer.WriteDouble(m_paraDimSize.fDimEndY);
		buffer.WriteDouble(m_paraDimSize.fDimPosX);
		buffer.WriteDouble(m_paraDimSize.fDimPosY);
		buffer.WriteDouble(m_paraDimSize.fScale);		//�ߴ����
		buffer.WriteInteger(m_paraDimSize.iDimDirect);	//��ע���� 0.X�᷽�� 1.Y�᷽�� 2.���췽��
	}
	else if(m_eCmdType==CMD_DIM_BOLTPAD)
	{	//��ע��Ȧ��Ϣ
		buffer.WriteInteger(m_paraBoltPad.hBolt);
		buffer.WriteDouble(m_paraBoltPad.fDimPosOffsetX);//��עλ��X����ƫ��ֵ
		buffer.WriteDouble(m_paraBoltPad.fDimPosOffsetY);//��עλ��Y����ƫ��ֵ
		buffer.WriteInteger(m_paraBoltPad.iDimPosType);	 //��ע����
	}
	else if(m_eCmdType==CMD_DIM_BOLTINFO)
	{	//��ע��˨��Ϣ wht 12-03-21
		m_paraBoltGroupInfo.lsGroupDim.ToBuffer(buffer);
		buffer.WriteInteger(m_paraBoltGroupInfo.iDimPosType);
		buffer.WriteDouble(m_paraBoltGroupInfo.fDimPosOffsetX);
		buffer.WriteDouble(m_paraBoltGroupInfo.fDimPosOffsetY);
	}
	else if(m_eCmdType==CMD_DRAW_ASSIST_LINE)
	{	//������
		buffer.WriteInteger(m_paraDrawAssistLine.iNo);
		buffer.WriteInteger(m_paraDrawAssistLine.nStyle);
	}
	else if(m_eCmdType==CMD_DIM_BOLTORFL_INFO)
	{	//��ע�˼���˨��Ϣ������Ϣ wht 12-06-01
		buffer.WriteInteger(m_paraBoltOrFlInfo.hLinePart);
		buffer.WriteInteger(m_paraBoltOrFlInfo.iDimPosType);
		buffer.WriteDouble(m_paraBoltOrFlInfo.fPoleAngle);
		buffer.WriteDouble(m_paraBoltOrFlInfo.fStartOddPosOffsetX);
		buffer.WriteDouble(m_paraBoltOrFlInfo.fStartOddPosOffsetY);
		buffer.WriteDouble(m_paraBoltOrFlInfo.fEndOddPosOffsetX);
		buffer.WriteDouble(m_paraBoltOrFlInfo.fEndOddPosOffsetY);
		buffer.WriteInteger(m_paraBoltOrFlInfo.bDimStartOdd);
		buffer.WriteInteger(m_paraBoltOrFlInfo.bDimEndOdd);
	}
	else if(m_eCmdType==CMD_DIM_ANGLEJOINT)
	{	//��ע�Ǹֽ�ͷ��Ϣ wht 14-07-08
		buffer.WriteInteger(m_paraAngleJoint.idAngleJoint);
		buffer.WriteInteger(m_paraAngleJoint.iDimPosType);
		buffer.WriteDouble(m_paraAngleJoint.fDimPosOffsetX);
		buffer.WriteDouble(m_paraAngleJoint.fDimPosOffsetY);
	}
}

//ɾ���ɸ��������ɵ�ʵ��
void CDrawingCommand::EmptyEntList()
{
	AcDbObjectId *pEntId=NULL;
	for(pEntId=entIdList.GetFirst();pEntId;pEntId=entIdList.GetNext())
	{	//ɾ���и��������ɵ�ʵ��
		AcDbEntity *pEnt=NULL;
		if(acdbOpenObject(pEnt,*pEntId,AcDb::kForWrite)!=Acad::eOk)
			continue;
		if(pEnt==NULL)
			continue;
		pEnt->erase();
		pEnt->close();
	}
	entIdList.Empty();	//��������Ӧ��ʵ���б�,�������ͳ�Ƶõ���ʵ�弯��ʵ�ʴ��ڵ�ʵ�弯��ƥ�䵼���Ϸ�ʱʧ�� wht 11-08-01
	//���½���
    actrTransactionManager->flushGraphics(); //�رմ򿪵�ʵ���ſɳɹ�ˢ�»���
	acedUpdateDisplay();
}

//��������
void CDrawingCommand::CopyProperty(CDrawingCommand *pCommand)
{
	m_eCmdType=pCommand->m_eCmdType;	//��������
	m_stateCmd=CMD_STATE_FREE;		//����״̬
	m_bExecute=pCommand->m_bExecute;//�Ƿ�ִ�и�����
	m_bFlag=FALSE;
	m_pDrawingCom=pCommand->m_pDrawingCom;
	if(m_eCmdType==CMD_DRAW_PART)
		m_paraPart=pCommand->m_paraPart;
	else if(m_eCmdType==CMD_DRAW_LINEPART)
		m_paraLinePart=pCommand->m_paraLinePart;
	else if(m_eCmdType==CMD_DIM_PARTSPEC)
		m_paraPartSpec=pCommand->m_paraPartSpec;
	else if(m_eCmdType==CMD_DIM_ODDMENT)
	{
		BOOL bDimStartOdd=m_paraOddment.bDimStartOdd;
		BOOL bDimEndOdd=m_paraOddment.bDimEndOdd;
		m_paraOddment=pCommand->m_paraOddment;
		m_paraOddment.bDimStartOdd=bDimStartOdd;	//
		m_paraOddment.bDimEndOdd=bDimEndOdd;		//�ⲿ����Ϣ��Ӧ�ÿ��� wjh-2011.8.17
	}
	else if(m_eCmdType==CMD_DIM_BOLTORFL_INFO)
	{
		BOOL bDimStartOdd=m_paraBoltOrFlInfo.bDimStartOdd;
		BOOL bDimEndOdd=m_paraBoltOrFlInfo.bDimEndOdd;
		m_paraBoltOrFlInfo=pCommand->m_paraBoltOrFlInfo;
		m_paraBoltOrFlInfo.bDimStartOdd=bDimStartOdd;	//
		m_paraBoltOrFlInfo.bDimEndOdd=bDimEndOdd;		//�ⲿ����Ϣ��Ӧ�ÿ��� wjh-2011.8.17
	}
	else if(m_eCmdType==CMD_DIM_ANGLEJOINT)
		m_paraAngleJoint=pCommand->m_paraAngleJoint;
	else if(m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)
		m_paraDimFootNailSpace=pCommand->m_paraDimFootNailSpace;
	else if(m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
		m_paraDimFillPlateSpace=pCommand->m_paraDimFillPlateSpace;
	else if(m_eCmdType==CMD_DIM_BOLTPAD)
		m_paraBoltPad=pCommand->m_paraBoltPad;
	else if(m_eCmdType==CMD_DIM_BOLTINFO)
	{	//������˨��ע��Ϣ wht 12-03-21
		m_paraBoltGroupInfo.iDimPosType=pCommand->m_paraBoltGroupInfo.iDimPosType;
		m_paraBoltGroupInfo.fDimPosOffsetX=pCommand->m_paraBoltGroupInfo.fDimPosOffsetX;
		m_paraBoltGroupInfo.fDimPosOffsetY=pCommand->m_paraBoltGroupInfo.fDimPosOffsetY;
		m_paraBoltGroupInfo.lsGroupDim.CopyProperty(&(pCommand->m_paraBoltGroupInfo.lsGroupDim));
	}
	else if(m_eCmdType==CMD_DIM_SIMPOLY)
	{
		m_paraSimPoly.fDimPosOffsetX=pCommand->m_paraSimPoly.fDimPosOffsetX;
		m_paraSimPoly.fDimPosOffsetY=pCommand->m_paraSimPoly.fDimPosOffsetY;
		m_paraSimPoly.fPickPtX=pCommand->m_paraSimPoly.fPickPtX;
		m_paraSimPoly.fPickPtY=pCommand->m_paraSimPoly.fPickPtY;
		m_paraSimPoly.fScale=pCommand->m_paraSimPoly.fScale;
		m_paraSimPoly.nSimPolyCmdType=pCommand->m_paraSimPoly.nSimPolyCmdType;
		m_paraSimPoly.simPolyVertexList.Empty();
		SIMPOLY_VERTEX *pVertex=NULL;
		for(pVertex=pCommand->m_paraSimPoly.simPolyVertexList.GetFirst();pVertex;
			pVertex=pCommand->m_paraSimPoly.simPolyVertexList.GetNext())
			m_paraSimPoly.simPolyVertexList.append(*pVertex);
	}
	else if(m_eCmdType==CMD_DRAW_TABLE)
		m_paraBOMTable=pCommand->m_paraBOMTable;
	else if(m_eCmdType==CMD_DIM_MODELSIZE)
	{
		m_paraDimModelSize.bUpDim=pCommand->m_paraDimModelSize.bUpDim;
		m_paraDimModelSize.fDimDistance=pCommand->m_paraDimModelSize.fDimDistance;
		m_paraDimModelSize.fMinX=pCommand->m_paraDimModelSize.fMinX;
		m_paraDimModelSize.fSpace=pCommand->m_paraDimModelSize.fSpace;
		m_paraDimModelSize.iDimType=pCommand->m_paraDimModelSize.iDimType;
		m_paraDimModelSize.listDimNodes.Empty();
		for(DIMNODE *pDimNode=pCommand->m_paraDimModelSize.listDimNodes.GetFirst();pDimNode;
			pDimNode=pCommand->m_paraDimModelSize.listDimNodes.GetNext())
			m_paraDimModelSize.listDimNodes.append(*pDimNode);
	}
	else if(m_eCmdType==CMD_DIM_MAPSIZE)
		m_paraDimSize=pCommand->m_paraDimSize;
	else if(m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)
		m_paraDimJgZhun=pCommand->m_paraDimJgZhun;
	else if(m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
		m_paraDimLsSpace=pCommand->m_paraDimLsSpace;
	else if(m_eCmdType==CMD_DIM_SEGI)
		m_paraDimSegI=pCommand->m_paraDimSegI;
	else if(m_eCmdType==CMD_DRAW_ASSIST_LINE)
		m_paraDrawAssistLine=pCommand->m_paraDrawAssistLine;
	//���������ʵ��ID�б�
	entIdList.Empty();
	AcDbObjectId *pObjID=NULL;
	for(pObjID=pCommand->entIdList.GetFirst();pObjID;pObjID=pCommand->entIdList.GetNext())
		entIdList.append(*pObjID);
}

SCOPE_STRU CDrawingCommand::GetCADEntScope()
{
	SCOPE_STRU scope;
	scope.ClearScope();
	scope.fMinX = scope.fMinY = scope.fMinZ = 100000.0;
	scope.fMaxX = scope.fMaxY = scope.fMaxZ =-100000.0;
	AcDbObjectId *pEntId=NULL;
	for(pEntId=entIdList.GetFirst();pEntId;pEntId=entIdList.GetNext())
		VerifyVertexByCADEntId(scope,*pEntId);
	double width=scope.fMaxX-scope.fMinX;
	double height=scope.fMaxY-scope.fMinY;
	if(fabs(fabs(width)-200000)<EPS||fabs(fabs(height)-200000)<EPS)
	{
		scope.fMinX=scope.fMinY=scope.fMinZ=0;
		scope.fMaxX=scope.fMaxY=scope.fMaxZ=0;
	}
	return scope;
}


//�õ��κŶ�Ӧ��ʵ�弯�ϼ���������
BOOL CDrawingCommand::GetEntSetAboutDimSegICmd(ATOM_LIST<AcDbObjectId> &entIdList,
											   BOOL bHighlight,SCOPE_STRU *pScope/*=NULL*/)
{
	if(m_eCmdType!=CMD_DIM_SEGI)
		return FALSE;
	ads_name ent_name;
	CDrawingCommand *pTempCmd=NULL;
	SCOPE_STRU scope;
	int pushed=m_pDrawingCom->cmdList.push_stack();
	for(pTempCmd=m_pDrawingCom->cmdList.GetFirst();pTempCmd;pTempCmd=m_pDrawingCom->cmdList.GetNext())
	{
		if(pTempCmd->m_eCmdType!=CMD_DRAW_LINEPART)
			continue;	//���Ƶ���
		AcDbObjectId *pEntId=NULL;
		for(pEntId=pTempCmd->entIdList.GetFirst();pEntId;pEntId=pTempCmd->entIdList.GetNext())
		{
			AcDbEntity *pEnt=NULL;
			acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
			if(pEnt==NULL)
				continue;
			long handle = GetTaAtomHandle(pEnt);
			pEnt->close();
			CLDSPart* pPart = Ta.FromPartHandle(handle);
			if(pPart&&pPart->iSeg.iSeg==m_paraDimSegI.nSegI.iSeg)
			{
				if(pEnt->isKindOf(AcDbLine::desc()))
				{
					AcDbLine *pLine=(AcDbLine*)pEnt;
					scope.VerifyVertex(f3dPoint(pLine->startPoint().x,pLine->startPoint().y,0));
					scope.VerifyVertex(f3dPoint(pLine->endPoint().x,pLine->endPoint().y,0));
				}
				else if(pEnt->isKindOf(AcDbPoint::desc()))
				{
					AcDbPoint *pPoint=(AcDbPoint*)pEnt;
					scope.VerifyVertex(f3dPoint(pPoint->position().x,pPoint->position().y,0));
				}
				entIdList.append(*pEntId);
				acdbGetAdsName(ent_name,*pEntId);
				if(bHighlight)
					ads_redraw(ent_name,3);//������ʾ
				else 
					ads_redraw(ent_name,4);//ȡ��������ʾ
				//���½���
				actrTransactionManager->flushGraphics();
				acedUpdateDisplay();
			}
		}
	}
	m_pDrawingCom->cmdList.pop_stack(pushed);
	if(pScope)
		*pScope=scope;
	return TRUE;
}

//�����ͼ��Ӧ��ʵ�弯��
int CDrawingCommand::GetEntSet(ads_name &drag_ent_set,BOOL bSelectEnts/*=FALSE*/)
{
	//��ȡ�������ʵ�弯
	AcDbObjectId *pEntId=NULL;
	for(pEntId=entIdList.GetFirst();pEntId;pEntId=entIdList.GetNext())
	{
		ads_name ent_name;
		if(acdbGetAdsName(ent_name,*pEntId)!=Acad::eOk)
			continue;
		if(bSelectEnts)
			ads_redraw(ent_name,3);//������ʾ
		if(acedSSAdd(ent_name,drag_ent_set,drag_ent_set)!=RTNORM)
			continue;
		struct resbuf *pData=acdbEntGet(ent_name);
		acdbEntMod(pData);
		acdbEntUpd(ent_name);
	}
	long ll=0;
	acedSSLength(drag_ent_set,&ll);
	return ll;
}
//���������Ӧ��ʵ�弯��Ϊ������ʾ
void CDrawingCommand::SetEntSetHighlight(int type)
{
	AcDbObjectId *pEntId=NULL;
	if(entIdList.GetNodeNum()==0)
		return;
	long key=0;
	AcDbObjectId entId;
	ads_name ent_name;
	if(type==1||type==2)	//��������ʾʼ������ͷ
	{
		pEntId=(type==1)?entIdList.GetFirst():entIdList.GetTail();
		if(pEntId)
		{
			entId=*pEntId;
			key=(long)pEntId;
		}
		AcDbEntity *pEnt=NULL;
		if(m_eCmdType==CMD_DIM_SPECIAL_LSSPACE&&m_pDrawingCom!=NULL)
		{
			pEnt=m_pDrawingCom->FindCADEntByHandle(type==1?m_paraDimLsSpace.spaceDim.hStartLs:m_paraDimLsSpace.spaceDim.hEndLs);
			if(pEnt)
			{
				entId=pEnt->id();
				key=(long)pEnt;
			}
		}
		if(acdbGetAdsName(ent_name,entId)!=Acad::eOk)
			return;
		ads_redraw(ent_name,3);//������ʾ
		hashHighlightEnts.SetValue(key,entId);
	}
	else
	{
		for(pEntId=entIdList.GetFirst();pEntId;pEntId=entIdList.GetNext())
		{
			if(acdbGetAdsName(ent_name,*pEntId)!=Acad::eOk)
				continue;
			ads_redraw(ent_name,3);//������ʾ
			hashHighlightEnts.SetValue((long)pEntId,*pEntId);
		}
		if(m_eCmdType==CMD_DIM_BOLTINFO&&m_pDrawingCom->m_eClassType==DRAW_COM_STRUVIEW)
		{	//��ǰ����Ϊ��ע��˨��Ϣʱ��ͬʱ������ʾ���������˨ wht 12-03-22
			ATOM_LIST<AcDbObjectId> sonEntIdList;
			((CStruView*)m_pDrawingCom)->GetDimBoltInfoCmdAttachEntSet(this,sonEntIdList);
			for(pEntId=sonEntIdList.GetFirst();pEntId;pEntId=sonEntIdList.GetNext())
			{
				if(acdbGetAdsName(ent_name,*pEntId)!=Acad::eOk)
					return;
				ads_redraw(ent_name,3);//������ʾ
				hashHighlightEnts.SetValue((long)pEntId,*pEntId);
			}
		}
	}
	//���½���
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();
}
void CDrawingCommand::ReleaseHighlightEnts()
{
	for(AcDbObjectId *pEntId=hashHighlightEnts.GetFirst();pEntId;pEntId=hashHighlightEnts.GetNext())
	{
		ads_name ent_name;
		if(acdbGetAdsName(ent_name,*pEntId)!=Acad::eOk)
			continue;
		ads_redraw(ent_name,4);//ȡ��������ʾ
	}
	hashHighlightEnts.Empty();
}
SEGI CDrawingCommand::GetSegI(void){return m_pDrawingCom->ParentDrawing()->m_nSegI;}

void CDrawingCommand::UpdatePtrByHandle()
{
	if(m_eCmdType==CMD_DRAW_PART)			//���ƹ���
		m_paraPart.pPart=Ta.FromPartHandle(m_paraPart.hPart);
	else if(m_eCmdType==CMD_DIM_PARTSPEC)	//������ϸ
		m_paraPartSpec.pObj=Ta.FromPartHandle(m_paraPartSpec.hObj);
	else if(m_eCmdType==CMD_DIM_BOLTPAD)//��˨��Ȧ
		m_paraBoltPad.pBolt=(CLDSBolt*)Ta.FromPartHandle(m_paraBoltPad.hBolt,CLS_BOLT);
	//else if(m_eCmdType==CMD_DIM_BOLTINFO)//��˨��Ϣ����˨��Ȧ
		//m_paraBoltGroupInfo.lsGroupDim.UpdatePtrByHandle();
	else if(m_eCmdType==CMD_DRAW_LINEPART)	//���Ƹ˼�����ͼ
		m_paraLinePart.pLinePart=(CLDSLinePart*)Ta.FromPartHandle(m_paraLinePart.hLinePart,CLS_LINEPART);
	else if(m_eCmdType==CMD_DIM_ODDMENT)		//����ͷ
		m_paraOddment.pLinePart=(CLDSLinePart*)Ta.FromPartHandle(m_paraOddment.hLinePart,CLS_LINEPART);
	else if(m_eCmdType==CMD_DIM_BOLTORFL_INFO)	//��˨��Ϣ������Ϣ
		m_paraBoltOrFlInfo.pLinePart=(CLDSLinePart*)Ta.FromPartHandle(m_paraBoltOrFlInfo.hLinePart,CLS_LINEPART);
	else if(m_eCmdType==CMD_DIM_ANGLEJOINT)		//��ע�Ǹֽ�ͷ��Ϣ
		m_paraAngleJoint.pJoint=Ta.hashAngleJoint.GetValue(m_paraAngleJoint.idAngleJoint);
	else if(m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)//�Ŷ����
		m_paraDimFootNailSpace.pLinePart=(CLDSLinePart*)Ta.FromPartHandle(m_paraDimFootNailSpace.hLinePart,CLS_LINEPART);
	else if(m_eCmdType==CMD_DIM_FILLPLATE_SPACE)//�����
		m_paraDimFillPlateSpace.pLinePart=(CLDSLinePart*)Ta.FromPartHandle(m_paraDimFillPlateSpace.hLinePart,CLS_LINEPART);
	else if(m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)//����Ǹ�׼��
		m_paraDimJgZhun.pLineAngle=(CLDSLineAngle*)Ta.FromPartHandle(m_paraDimJgZhun.hLineAngle,CLS_LINEANGLE);
	else if(m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)//��˨������
		m_paraDimLsSpace.pPlate=Ta.FromPartHandle(m_paraDimLsSpace.hPlate);
	else if(m_eCmdType==CMD_DIM_MODELSIZE)	//ģ�ͳߴ�
	{
		m_paraDimModelSize.listDimNodes.Empty();
		for(DIMNODE* pDimNode=m_paraDimModelSize.listDimNodes.GetFirst();pDimNode;pDimNode=m_paraDimModelSize.listDimNodes.GetNext())
			pDimNode->pNode=pDimNode->hNode>0x20?Ta.FromNodeHandle(pDimNode->hNode):&DIM_MODELSIZE_PARA::xMidAxisNode;
	}
}

//���ù�������˳���
int CDrawingCommand::SetDrawingSerialNo(int serialNo)
{
	m_nDrawingSerialNo=serialNo;
	return m_nDrawingSerialNo;
}
//�õ���������˳���
int CDrawingCommand::GetDrawingSerialNo()
{
	return m_nDrawingSerialNo;
}

//////////////////////////////////////////////////////////////////////////
//DrawingSpreadLine.cpp

CDrawingSpreadLine::CDrawingSpreadLine()
{
	m_iDefType=0;
	m_iCurWing=0;
	m_fOffsetDist=0;
	m_hDatumPole=0;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sLineName.Copy("New spreading line");
#else
	m_sLineName.Copy("�½�չ����");
#endif
	m_lineSpread.startPt.Set();
	m_lineSpread.endPt.Set();
}

CLDSLineAngle *CDrawingSpreadLine::GetDatumPole()
{
	if(m_hDatumPole<=0x20)
		return NULL;
	if(m_pLineAngle==NULL||(m_pLineAngle&&m_pLineAngle->handle!=m_hDatumPole))
		m_pLineAngle=(CLDSLineAngle*)Ta.FromPartHandle(m_hDatumPole,CLS_LINEANGLE);
	return m_pLineAngle;
}

void CDrawingSpreadLine::FromBuffer(CBuffer &buffer, long version/*=NULL*/)
{
	buffer.ReadString(m_sLineName);	//չ��������
	buffer.ReadByte(&m_iDefType);		//0.ָ���˼� 1.ֱ��ָ��
	buffer.ReadInteger(&m_hDatumPole);	//��׼�˼� 
	buffer.ReadByte(&m_iCurWing);		//0.X֫ 1.Y֫
	buffer.ReadDouble(&m_fOffsetDist);	//ƫ�ƾ���
	buffer.ReadPoint(m_lineSpread.startPt);	//չ����ʼ��λ��
	buffer.ReadPoint(m_lineSpread.endPt);		//չ�����ն�λ��
}

void CDrawingSpreadLine::ToBuffer(CBuffer &buffer)
{
	buffer.WriteString(m_sLineName);	//չ��������
	buffer.WriteByte(m_iDefType);		//0.ָ���˼� 1.ֱ��ָ��
	buffer.WriteInteger(m_hDatumPole);	//��׼�˼� 
	buffer.WriteByte(m_iCurWing);		//0.X֫ 1.Y֫
	buffer.WriteDouble(m_fOffsetDist);	//ƫ�ƾ���
	buffer.WritePoint(m_lineSpread.startPt);	//չ����ʼ��λ��
	buffer.WritePoint(m_lineSpread.endPt);		//չ�����ն�λ��
}
#endif
