#include "stdafx.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "SysPara.h"
#include "RxTools.h"
#include "DrawDim.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDrawSysPara sys;
//������˨(��)��ͼ������ʽ��������Ӧ����˨ͼ��(��)�Ļ�ͼ���ϱ���ϵ��
double DRAWING_SCALE::ComplexCoef()
{
	if(fabs(fRealToDraw)<EPS)
		return 1.0;
	if(iBoltSymbolStyle==0)
		return 1.0/fRealToDraw;
	else if(iBoltSymbolStyle==1)
		return 1.0;
	else if(iBoltSymbolStyle==2)
		return (1.0/fRealToDraw)*fBoltSymbolFactor;
	else if(iBoltSymbolStyle==3)
		return (1.0/fRealToDraw)*fBoltDiaFactor;
	else
		return 1.0;
}
CDrawSysPara::CDrawSysPara()
{
	//FillMemory(this,sizeof(CDrawSysPara),0);//ע�ʹ��з��������ǰ���ϣ��m_hashMapFrameList��ֵ
	//��ʼ��m_hashMapFrameList
	MAP_FRAME_SIZE mapFrm;
	mapFrm.style = 0;
	strcpy(mapFrm.sMapFrameName,"A0");
	mapFrm.nLength = 1194;
	mapFrm.nWidth = 868;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin
		=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=10;
	mapFrm.nBottomRightFixLen=280;
	mapFrm.nBottomRightFixWidth=160;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	mapFrm.style = 1;
	strcpy(mapFrm.sMapFrameName,"A0-2");
	mapFrm.nLength = 868;
	mapFrm.nWidth = 1194;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin
		=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=10;
	mapFrm.nBottomRightFixLen=280;
	mapFrm.nBottomRightFixWidth=160;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	mapFrm.style = 2;
	strcpy(mapFrm.sMapFrameName,"A1");
	mapFrm.nLength = 841;
	mapFrm.nWidth = 594;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=5;
	mapFrm.nBottomRightFixLen=130;
	mapFrm.nBottomRightFixWidth=50;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	mapFrm.style = 3;
	strcpy(mapFrm.sMapFrameName,"A1-2");
	mapFrm.nLength = 594;
	mapFrm.nWidth = 841;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=5;
	mapFrm.nBottomRightFixLen=130;
	mapFrm.nBottomRightFixWidth=50;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	mapFrm.style = 4;
	strcpy(mapFrm.sMapFrameName,"A2");
	mapFrm.nLength = 594;
	mapFrm.nWidth = 420;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=5;
	mapFrm.nBottomRightFixLen=130;
	mapFrm.nBottomRightFixWidth=50;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	mapFrm.style = 5;
	strcpy(mapFrm.sMapFrameName,"A2-2");
	mapFrm.nLength = 420;
	mapFrm.nWidth = 594;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=5;
	mapFrm.nBottomRightFixLen=130;
	mapFrm.nBottomRightFixWidth=50;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	mapFrm.style = 6;
	strcpy(mapFrm.sMapFrameName,"A3");
	mapFrm.nLength = 420;
	mapFrm.nWidth = 297;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=5;
	mapFrm.nBottomRightFixLen=130;
	mapFrm.nBottomRightFixWidth=50;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	mapFrm.style = 7;
	strcpy(mapFrm.sMapFrameName,"A3-2");
	mapFrm.nLength = 297;
	mapFrm.nWidth = 420;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=5;
	mapFrm.nBottomRightFixLen=130;
	mapFrm.nBottomRightFixWidth=50;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	mapFrm.style = 8;
	strcpy(mapFrm.sMapFrameName,"A4");
	mapFrm.nLength = 297;
	mapFrm.nWidth = 210;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=5;
	mapFrm.nBottomRightFixLen=130;
	mapFrm.nBottomRightFixWidth=50;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	mapFrm.style = 9;
	strcpy(mapFrm.sMapFrameName,"A4-2");
	mapFrm.nLength = 210;
	mapFrm.nWidth = 297;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=10;
	mapFrm.nBottomRightFixLen=130;
	mapFrm.nBottomRightFixWidth=50;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	int m_iCurMapType=0;        //��ǰ��ͼ����
	int m_iCurMapSegNo=1;		//��ǰ�ֶ�ͼ�ķֶκ�
	int m_iCurBodyNo=1;		//��ǰ��ͼ�ı����
	bDrawMainPoleJointLine=TRUE;
	nJointLineOffset = 100;	//�ֶα���߾������׼�Ǹ־���
	nJointLineLen = 400;	//�ֶα���߳���
	nBriefGuigeN=4;
	//��������
		//��ͼ����
	general_set.LineMapScale = 100;	//����ͼ���� һ��Ϊ1:100 1:200 1:300
	general_set.ForceLineScale = 50;//˾��ͼ(��ѧ����)  һ��Ϊ1:50
	general_set.StruMapScale.fRealToDraw = 20;		//�ṹͼ���� һ��Ϊ1:10 1:20 1:30 1:15
	general_set.StruMapScale.iBoltSymbolStyle=2;	//�ṹͼ��˨ͼ����ʽ,������ΰ���鵱�µ���ͼֽ��ͳһ��Ϊ�ṹͼ����ͼ�� wjh-2015.10.08
	general_set.StruMapScale.fBoltSymbolFactor=20;	//�ṹͼ��˨ͼ����������
	general_set.StruMapScale.fBoltDiaFactor=0.5;	//�ṹͼ��˨ͼ������ֱ������
	general_set.PartMapScale.fRealToDraw = 5;		//����ͼ���� һ��Ϊ1:5 1:10
	general_set.PartMapScale.iBoltSymbolStyle=3;	//����ͼ��˨ͼ����ʽ,������ΰ���鵱�µ���ͼֽ��ͳһ��Ϊ�ṹͼ����ֱ��ͼ�� wjh-2015.10.08
	general_set.PartMapScale.fBoltSymbolFactor=5;	//����ͼ��˨ͼ����������
	general_set.PartMapScale.fBoltDiaFactor=0.5;	//����ͼ��˨ͼ������ֱ������
	general_set.WeldMapScale=general_set.StruMapScale;	//�麸ͼĬ����ṹͼ��ͼ����һ��
	general_set.WeldMapScale.fRealToDraw=general_set.WeldMapScale.fBoltSymbolFactor=10;
	general_set.fWeldLineSymbolHigh	= 0.8;
	general_set.fWeldLineSymbolSpace= 0.8;
	general_set.fHoleAugmentInNC	= 0.0;	//�׾�����ֵ
	general_set.iDimPrecision		= 0;	//0.1mm 1.0.5mm 2.0.1mm
	general_set.iPartBOMStyle		= 0;	//0.��׼��ʽ 1.�ӿ���ʽ Ĭ��Ϊ��׼��ʽ
	general_set.iSumMatBOMStyle		= 0;	//0.��ʽһ 1.��ʽ��
	general_set.cMatCharSeparator	= 0;
		//�������տ���������
	general_set.nPartRowsN = 4;
	general_set.nRowHigh = 165;				//���й���ͼ�߶�
	general_set.nColumnWidth = 250;			//���й���ͼ���
	general_set.bProcessCardTbl = FALSE;	//���տ�ͼ��
	general_set.bCardLayoutBaseRow=FALSE;	//���տ�����Ϊ��׼
	general_set.bHasFrame = FALSE;			//����װ����
	//���ֻ��ƽ���������
	general_set.lCrossarmCr = RGB(255,255,0);  //��ᵣ��ɫ
	general_set.rCrossarmCr = RGB(255,0,0);//�Һᵣ��ɫ
		//�ṹͼ��������
	stru_map.bExportByEnglish		= FALSE;
	stru_map.fMinPartThick			= 12;
	stru_map.fMinWingWide			= 60;
	stru_map.bDimSpecialLsSpace		= 1;
	stru_map.bDimSpecialJgZhun		= 1;
	stru_map.bDimStruJgLen			= 1;
	stru_map.bDimPartGuiGe			= 1;		//��ע������� wht 11-03-29
	stru_map.bDrawCutAngleAndWingInfo = 1;
	stru_map.bOnlyDrawHorizontalPlate = FALSE;	//�������ڵ�ǰ�ṹͼ��ˮƽ��ʾ�ĸְ� wht 10-06-15
	stru_map.bMergeAttachPlatesToBasePlateInBOM=TRUE;
	stru_map.bUsePolyLineDrawEdge	  = TRUE;	//�ö���߻��ƽǸֺ͸ְ��������	 wht 10-06-15
	stru_map.fPolyLineWidth = 0.1;              //����߿��           xc 10-12-17
	stru_map.fPoleAxisLineStartOutSpreadLen=30;	//�˼�����ʼ�����쳤�� wht 10-12-13
	stru_map.fPoleAxisLineEndOutSpreadLen=30;	//�˼������ն����쳤��
	stru_map.bReadAdditionalBuffer=TRUE;		//��ȡ���ӻ��� wht 11-08-03
		//�ṹͼ�ָ�����
	dim_map.fPartGuigeTextSize		= 2.5;	//��������ע���ָ�
	dim_map.fJgOddmentTextSize		= 2.5;	//�Ǹ�����ͷ��ע
	dim_map.fOtherTextSize			= 2.0;	//������ͨ���ָ߶�
	dim_map.fSimPolyTextSize		= 2.0;  //�����α�ע
	dim_map.fPlateThickTextSize	= 2.0;  //����ע
	dim_map.fLsGuigeTextSize		= 2.0;	//��˨����ע���ָ�
	dim_map.fSimTriScale			= 0.2;	//�����α��� �����α���Ĭ��ֵ�޸�Ϊ0.2 wht 11-05-06
	//�麸ͼ��ͼ����
	part_group_map.fMinFontSize		=1.0;			//�麸ͼ��������С�߶�
	part_group_map.fMinDrawThick	=0.5;			//�����С����ֵ,֮ǰĬ��Ϊ2mm,ͨ��ʵ����֤��Ϊ0.5mm����2mmƫ�󲻺ÿ� wjh-2018.7.29
	part_group_map.nKmJgGIncrement	=0;				//��ģ�Ǹ��ľ����ֵ
	part_group_map.bModulatePartGroupLen=TRUE;		//�����麸������
	part_group_map.fPartGroupDistThreshold=200;		//�麸ͼ������ֵ
	part_group_map.fPartGroupDistZoomCoef=0.5;		//�麸ͼ����ϵ��
	part_group_map.bDrawWeldLineSymbol=TRUE;		//�麸ͼ�л��ƺ����߱��
	part_group_map.iPlateLocateDrawStyle=1;			//�麸ͼ���������߷�ʽ���Ƹְ�Ի��
	part_group_map.iLocateSymboleHeight=5;			//�躸ͼ�жԻ���Ǹ߶�
	part_group_map.bDimEndOuterPlaneSpace=TRUE;		//�˼�����浽��������
	part_group_map.bAutoDimPartNo=TRUE;				//�Զ���ע�������
	part_group_map.bAutoMatchView=TRUE;				//�Զ�����麸����ͼ
	part_group_map.bDimAbsoluteDist=TRUE;			//��ע��λ����Գߴ�
	part_group_map.bDimAfterWeldCheckLine=TRUE;		//��ע���Ӻ�����
	part_group_map.bDimBeforeWeldLocatSpace=TRUE;	//��ע����ǰ��λ�ߴ�
	part_group_map.bDimPlatePoleRayAndAngle=FALSE;	//��ע�ְ������Լ��������׼��֮��ļн�
	part_group_map.bSmartFilterRedundantPart=TRUE;	//��ͼ���Զ�ɸѡ���뵱ǰ��ͼ�޹صĶ��๹����������ʾ
	part_group_map.bAngularDimension=TRUE;			//��ע�Ƕ���
	part_group_map.bCreatePartGroupBOM=TRUE;		//������ϸ��
	part_group_map.part_table.bZoomInProcessCard=FALSE;	//���տ��ڽ��б�������
	part_group_map.part_table.bDispPartGroupNo=FALSE;	//�麸�����
	part_group_map.part_table.bDispPartGroupNum=FALSE;	//�麸������
	part_group_map.part_table.bDispPartNo=TRUE;		//�������
	part_group_map.part_table.bDispPartNum=TRUE;	//��������
	part_group_map.part_table.bDispPartGuiGe=TRUE;	//�������
	part_group_map.part_table.bDispPartLen=TRUE;	//��������
	part_group_map.part_table.bDispPartWeight=TRUE;	//���������Լ�ͬһ��Ź�������С�� ����
	part_group_map.part_table.bDispNotes=FALSE;		//��ע
	part_group_map.part_table.bDispInternal=FALSE;	//���켶������
	part_group_map.part_table.bDispAppearance=FALSE;//���켶�����
	//����ͼ��������
	part_map.iExportDeformedProfile=TRUE;	//����ͼ���ǻ�������
	part_map.iExportDeformedProfile=1;		//���ǻ�������
	part_map.bDimDeformedProfileText=TRUE;	//����ͼ���ǻ�������ʱ�Ƿ��ע��������������� wht 10-11-01
	part_map.bDimPlaneAngle=TRUE;			//��ע����������н�
	part_map.iDimAngleType=2;				//0.���;1.�۽�;2.�Զ� wht 10-11-01
	part_map.fMinHuoquAngle=1;				//��С�����Ƕ���ֵ
	part_map.bDimHuoquType=TRUE;			//��ע������������
	part_map.bUseZhengFanQun=FALSE;			//���������� Ĭ��ʹ���������� wht 11-05-07
	part_map.iPartMapStyle=0;				//0��ʾ��ͨ;1��ʾ�㽭ʢ��
		//�Ǹֹ���ͼ
	part_map.angle.iJgZoomSchema=3;			//0.1:1���� 1.ʹ�ù���ͼ���� 2.����ͬ������ 3.����ֱ�����
	part_map.angle.ciJgLenDrawMode=0;		//0.�������� 1.�м伷ѹ
	part_map.angle.bModulateLongJg=TRUE;	//�����Ǹֳ���
	part_map.angle.bOneCardMultiPart=FALSE;	//�Ǹ�����һ��������
	part_map.angle.iJgGDimStyle=2;			//0.ʼ�˱�ע  1.�м��ע 2.�Զ��ж�
	part_map.angle.bMaxExtendAngleLength=TRUE;
	part_map.angle.nMaxBoltNumStartDimG=100;//������ʼ�˱�ע׼��֧�ֵ������˨��
	part_map.angle.iLsSpaceDimStyle=2;		//0.X�᷽��	  1.Y�᷽��  2.�Զ��ж� 3.����ע
	part_map.angle.nMaxBoltNumAlongX=50;	//��X�᷽���ע֧�ֵ������˨����
	part_map.angle.bDimCutAngle=TRUE;		//��ע�Ǹ��н�
	part_map.angle.bDimCutAngleMap=TRUE;	//��ע�Ǹ��н�ʾ��ͼ
	part_map.angle.bDimPushFlatMap=TRUE;	//��עѹ��ʾ��ͼ
	part_map.angle.bJgUseSimpleLsMap=FALSE;	//�Ǹ�ʹ�ü���˨ͼ��
	part_map.angle.bDimLsAbsoluteDist=TRUE;	//��ע��˨���Գߴ�
	part_map.angle.bMergeLsAbsoluteDist=TRUE;//�ϲ����ڵȾ���˨���Գߴ�
	part_map.angle.bDimRibPlatePartNo=TRUE;	//��ע�Ǹ��߰���
	part_map.angle.bDimRibPlateMaterial=TRUE;	//��ע�Ǹ��߰����
	part_map.angle.bDimRibPlateSetUpPos=TRUE;//��ע�Ǹ��߰尲װλ��
	part_map.angle.iCutAngleDimType=0;		//�Ǹ��нǱ�ע��ʽ 
	//���ϽǱ�ע
	part_map.angle.bDimKaiHe=TRUE;			//��ע�Ǹֿ��Ͻ�
	part_map.angle.bDimKaiheAngleMap=TRUE;	//��ע�Ǹֿ��Ͻ�ʾ��ͼ
	part_map.angle.fKaiHeJiaoThreshold=2;	//Ĭ�Ͽ��ϽǱ�ע��ֵΪ2��
	CLDSLineAngle::KAIHEJIAO_ANGLE_THRESHOLD=part_map.angle.fKaiHeJiaoThreshold;	//Ĭ�Ͽ��ϽǱ�ע��ֵΪ2��
	part_map.angle.fKaiHeJiaoMinDeita=20;	//һ��ü�϶Ϊ10~50mm
	part_map.angle.bDimKaiheSumLen=TRUE;
	part_map.angle.bDimKaiheAngle=TRUE;
	part_map.angle.bDimKaiheSegLen=TRUE;
	part_map.angle.bDimKaiheScopeMap=FALSE;
		//�ְ幹��ͼ
	part_map.plate.bDimPlateRayLs=TRUE;		//��ע����˨���߾���
	part_map.plate.bDimPlateBoltAngle=FALSE;//��ע����˨�Ƕ�
	part_map.plate.bDimBoltEdgeSpace=FALSE;	//��ע����˨�߾�
	part_map.plate.bDimDiffRayLsSpace=FALSE;//��ע����������˨���
	part_map.plate.bDimLsToHuoquLine=FALSE;	//��ע��˨�������߼��
	part_map.plate.bDimLsToWeldEdge=FALSE;	//��ע��˨�����ӱ߼��
	part_map.plate.bDimPlateEdgeLen=FALSE;	//��ע�ְ�߳�
	part_map.plate.bDrawInnerEdge=FALSE;    //������������
	part_map.plate.fInnerEdgeOffset=3.0;    //������ƫ�ƾ�
	part_map.plate.bDimArcEdgeR=FALSE;		//��ע�ְ�Բ���߰뾶
	part_map.plate.bDimArcAngle=FALSE;		//��ע�ְ�Բ���߽Ƕ�
	part_map.plate.bAdjustPlateUcs=FALSE;	//�����ְ�����ϵ��֤�����ˮƽ����
	part_map.plate.bDimCoord=FALSE;			//�幹��ͼ���Ƿ��ע����
	part_map.plate.bPlateUseSimpleLsMap=FALSE;		//�ְ�ʹ�ü���˨ͼ��
	part_map.plate.bZoomDimTextSizeByWidth=FALSE;	//���ݸְ峤���н�Сֵ���Ÿְ���ر�ע������߶�
	part_map.plate.fTextSizeZoomCoef=0.03;			//����߶�����֮�������ϵ��
	part_map.plate.bDrawSlopeCutLine = FALSE;		//���촦���¿���
	part_map.plate.bDrawDatumLine=FALSE;			//���Ƹְ�Ļ�׼��
	part_map.plate.iCutArcsType=0;					//�������Ǵ����л���ʽ 0--���� 1--�н� 2--�л�
	part_map.plate.iZhiWanLineType=1;				//������λ�ô���ʽ
	part_map.plate.fInnerOffThreshold=0;			//����������ƫ����ֵ
	part_map.plate.bMarkHuoquPos=FALSE;				//��ǻ���λ��
	part_map.plate.bDrawHuoQuPrjMap=FALSE;			//���ƻ���ʾ��ͼ
	part_map.plate.bDrawHuoQuMarkLine=TRUE;			//���ƻ���������
	part_map.plate.bDisplaypPartInfoTbl=FALSE;      //��ʾ������Ϣ��
	part_map.plate.nPartInfoTblColumnWidth=20;      //�������п�
	part_map.plate.bUseDimStyleConfigFile=FALSE;	//���øְ������ļ�
	part_map.plate.bShowPlateCfgInfoDlg=TRUE;		//��ʾ�ְ����öԻ���
	strcpy(part_map.plate.sPlateConfigFilePath,"");	//�ְ������ļ�
		//�ֹܹ���ͼ
	part_map.tube.fSlotWidthProcessError=0;	//���ۿ�ȼӹ����ֵ
	part_map.tube.fSlotLenProcessError=0;	//���۳��ȼӹ����ֵ
	part_map.tube.fReelTubeThresholdD=300;	//�ְ���ֱ����ֵ

	//ͼֽ�еı�ע����
		//���� �ָ�
	strcpy(dim_map.sFontName,"gbhzfs.shx");
		//���ϱ������ݼ��ṹͼ�ڹ�����Ÿߣ�3.0��gbhzfs.shx
	dim_map.fHeaderTextSize		= 6;	//����ͷ���ָ�
	dim_map.fCommonTextSize		= 4;	//��ͨ����ָ�
	dim_map.fNumericTextSize	= 3;	//����е����ָ�
	dim_map.PartNo.fPartNoTextSize = 3.0;	//����������ָ�
	dim_map.PartNo.fPartNoCirD = 8.0;		//�������Ȧֱ�� 
	dim_map.PartNo.fMainPoleNoOuterCirD = 14.0;		//���ı����Բֱ��
	dim_map.PartNo.fMainPoleNoInnerCirD = 12.0;		//���������Բֱ��
	dim_map.PartNo.fPartNoMargin   = 0.3;	//������ſ�֮��ļ�϶ֵ
	dim_map.PartNo.bDrawRefLine	   = FALSE;	//����������
	dim_map.PartNo.iMirMsg		   = -1;	//�޶Գ�
		//�κű�ע��ز���	wht 11-06-27
	dim_map.fSegICirD		= 10;
	dim_map.fSegITextSize	= 5;

	dim_map.fDimTextSize		= 2.5;	//���ȳߴ��ע�ı���
	dim_map.fDimAngleTextSize	= 2.5;	//�Ƕȳߴ��ע�ı���
	dim_map.fDimArrowSize		= 1.5;	//�ߴ��ע��ͷ��
	strcpy(dim_map.sArrowType,"_OBLIQUE");	//�ߴ��ע��ͷ���� ��ͷ��ʽĬ��Ϊ��б wht 11-05-06
	dim_map.fDimGap				= 0.75;	//�������ע�߼�϶ֵ
		//����ͼ��
	dim_map.fCutawayViewSymbolLen=8;		//����
	dim_map.fCutawayViewSymbolTextSize=4;	//����߶�
	dim_map.fCutawayViewPolyLineWidth=0.3;	//����ͼ����߿��
		//������ɫ
	dim_map.crIndex.crLs	  = 1;		//��˨--��ɫ
	dim_map.crIndex.crOutLine = 7;		//������--�ڰ�
	dim_map.crIndex.crSingleLine= 7;	//����ͼ--�ڰ�
	dim_map.crIndex.crHuoQuLine = 3;	//������--��ɫ
	dim_map.crIndex.crDashLine = 1;		//���̻���--��ɫ
	dim_map.crIndex.crAxisLine = 2;		//(��)����--����
	dim_map.crIndex.crNode = 4;			//�ڵ�--��ɫ
	dim_map.crIndex.crDimSimTri= 1;		//��ע--�ڰ�
	dim_map.crIndex.crDragLine = 3;		//������--��ɫ
	//����ͼֽ�������С�ı����涯�趨
	summary.bZoomAdaptiveWithScale=chief.bZoomAdaptiveWithScale=segment.bZoomAdaptiveWithScale=welding.bZoomAdaptiveWithScale=part.bZoomAdaptiveWithScale=true;
	summary.wiScaleModel2Draw=50;	//��ͼĬ�ϰ�1:50��ͼ����ʱ�趨����߶�
	chief.wiScaleModel2Draw=50;		//˾��ͼĬ�ϰ�1:50��ͼ����ʱ�趨����߶�
	segment.wiScaleModel2Draw=20;	//�ṹͼĬ�ϰ�1:20��ͼ����ʱ�趨����߶�
	welding.wiScaleModel2Draw=10;	//�麸ͼĬ�ϰ�1:10��ͼ����ʱ�趨����߶�
	part.wiScaleModel2Draw=5;		//���ͼ������ͼĬ�ϰ�1:5��ͼ����ʱ�趨����߶�
	//��ʼ��ȫ�ֱ�����ͬʱ����ʼ��Ĭ�ϵ���ͼ�� wht 10-05-07
	//�ṹͼ�пɼ�������ͼ��
	LayerTable::VisibleProfileLayer.layerName.Copy("1");
	LayerTable::VisibleProfileLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::VisibleProfileLayer.lineWeight=AcDb::kLnWt025;
	LayerTable::VisibleProfileLayer.colorIndex=sys.dim_map.crIndex.crOutLine;//��ɫ
	LayerTable::VisibleProfileLayer.layerId=0;
	LayerTable::VisibleProfileLayer.lineTypeId=0;
	//�ṹͼ�в��ɼ�������ͼ��
	LayerTable::UnvisibleProfileLayer.layerName.Copy("2");
	LayerTable::UnvisibleProfileLayer.lineTypeName.Copy("HIDDEN");
	LayerTable::UnvisibleProfileLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::UnvisibleProfileLayer.colorIndex=sys.dim_map.crIndex.crDashLine;	//��ɫ
	LayerTable::UnvisibleProfileLayer.layerId=0;
	LayerTable::UnvisibleProfileLayer.lineTypeId=0;
	//�����ĵ���ͼ����ͼ
	LayerTable::BriefLineLayer.layerName.Copy("3");
	LayerTable::BriefLineLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::BriefLineLayer.lineWeight=AcDb::kLnWt018;
	LayerTable::BriefLineLayer.colorIndex=sys.dim_map.crIndex.crOutLine;//��ɫ
	LayerTable::BriefLineLayer.layerId=0;
	LayerTable::BriefLineLayer.lineTypeId=0;
	//���ĵ���ͼ
	LayerTable::AuxLineLayer.layerName.Copy("4");
	LayerTable::AuxLineLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::AuxLineLayer.lineWeight=AcDb::kLnWt018;
	LayerTable::AuxLineLayer.colorIndex=4;	//��ɫ
	LayerTable::AuxLineLayer.layerId=0;
	LayerTable::AuxLineLayer.lineTypeId=0;
	//��˨ͼ��
	LayerTable::BoltSymbolLayer.layerName.Copy("5");
	LayerTable::BoltSymbolLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::BoltSymbolLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::BoltSymbolLayer.colorIndex=sys.dim_map.crIndex.crLs;
	LayerTable::BoltSymbolLayer.layerId=0;
	LayerTable::BoltSymbolLayer.lineTypeId=0;
	//�����α�ע
	LayerTable::SimPolyLayer.layerName.Copy("6");
	LayerTable::SimPolyLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::SimPolyLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::SimPolyLayer.colorIndex=sys.dim_map.crIndex.crDimSimTri;
	LayerTable::SimPolyLayer.layerId=0;
	LayerTable::SimPolyLayer.lineTypeId=0;
	//�����ߡ��Գ��ߡ���λ����
	LayerTable::AxisLineLayer.layerName.Copy("7");
	LayerTable::AxisLineLayer.lineTypeName.Copy("DASHDOT2X");
	LayerTable::AxisLineLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::AxisLineLayer.colorIndex=sys.dim_map.crIndex.crAxisLine;
	LayerTable::AxisLineLayer.layerId=0;
	LayerTable::AxisLineLayer.lineTypeId=0;
	//�ӽǸ�������
	LayerTable::SonJgAxisLineLayer.layerName.Copy("7A");
	LayerTable::SonJgAxisLineLayer.lineTypeName.Copy("DASHDOT2X");
	LayerTable::SonJgAxisLineLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::SonJgAxisLineLayer.colorIndex=sys.dim_map.crIndex.crAxisLine;
	LayerTable::SonJgAxisLineLayer.layerId=0;
	LayerTable::SonJgAxisLineLayer.lineTypeId=0;
	//����������
	LayerTable::AssistHuoQuLineLayer.layerName.Copy("8");
	LayerTable::AssistHuoQuLineLayer.lineTypeName.Copy("DIVIDE");
	LayerTable::AssistHuoQuLineLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::AssistHuoQuLineLayer.colorIndex=sys.dim_map.crIndex.crHuoQuLine;
	LayerTable::AssistHuoQuLineLayer.layerId=0;
	LayerTable::AssistHuoQuLineLayer.lineTypeId=0;
	//�Ǹֻ������ְ����
	LayerTable::BendLineLayer.layerName.Copy("8");
	LayerTable::BendLineLayer.lineTypeName.Copy("DIVIDE");
	LayerTable::BendLineLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::BendLineLayer.colorIndex=sys.dim_map.crIndex.crHuoQuLine;
	LayerTable::BendLineLayer.layerId=0;
	LayerTable::BendLineLayer.lineTypeId=0;
	//�Ͽ�����
	LayerTable::BreakLineLayer.layerName.Copy("9");
	LayerTable::BreakLineLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::BreakLineLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::BreakLineLayer.colorIndex=sys.dim_map.crIndex.crDragLine;
	LayerTable::BreakLineLayer.layerId=0;
	LayerTable::BreakLineLayer.lineTypeId=0;
	//��˨��
	LayerTable::BoltLineLayer.layerName.Copy("10");
	LayerTable::BoltLineLayer.lineTypeName.Copy("DASHDOT2X");
	LayerTable::BoltLineLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::BoltLineLayer.colorIndex=sys.dim_map.crIndex.crDimSimTri;
	LayerTable::BoltLineLayer.layerId=0;
	LayerTable::BoltLineLayer.lineTypeId=0;
	//�ߴ��ע
	LayerTable::DimSizeLayer.layerName.Copy("11");
	LayerTable::DimSizeLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::DimSizeLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::DimSizeLayer.colorIndex=sys.dim_map.crIndex.crHuoQuLine;
	LayerTable::DimSizeLayer.layerId=0;
	LayerTable::DimSizeLayer.lineTypeId=0;
	//���ֱ�ע
	LayerTable::DimTextLayer.layerName.Copy("12");
	LayerTable::DimTextLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::DimTextLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::DimTextLayer.colorIndex=sys.dim_map.crIndex.crOutLine;
	LayerTable::DimTextLayer.layerId=0;
	LayerTable::DimTextLayer.lineTypeId=0;
	//���������������ͼԪ����ͼ��
	LayerTable::CommonLayer.layerName.Copy("13");
	LayerTable::CommonLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::CommonLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::CommonLayer.colorIndex=sys.dim_map.crIndex.crDragLine;
	LayerTable::CommonLayer.layerId=0;
	LayerTable::CommonLayer.lineTypeId=0;
	//�����������
	LayerTable::DamagedSymbolLine.layerName.Copy("14");
	LayerTable::DamagedSymbolLine.lineTypeName.Copy("ZIGZAG");
	LayerTable::DamagedSymbolLine.lineWeight=AcDb::kLnWt013;
	LayerTable::DamagedSymbolLine.colorIndex=sys.dim_map.crIndex.crDragLine;
	LayerTable::DamagedSymbolLine.layerId=0;
	LayerTable::DamagedSymbolLine.lineTypeId=0;
	//�ͻ����ƻ�����
	strcpy(m_sCompanyName,"");
}


//��ע����л�ȡ���ò��� wht 11-06-10
BOOL CDrawSysPara::GetPublicSysParaFromReg(LPCTSTR lpszSection, LPCTSTR lpszEntry)	
{
	char sValue[MAX_PATH]="";
	char sSubKey[MAX_PATH]="";
	DWORD dwDataType,dwLength=MAX_PATH;
#ifdef __TMA_FILE_
#ifdef AFX_TARG_ENU_ENGLISH
	sprintf(sSubKey,"Software\\Xerofox\\TMA-EN\\%s",lpszSection);
#else
	sprintf(sSubKey,"Software\\Xerofox\\TMA\\%s",lpszSection);
#endif
#elif defined(__LMA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\LMA\\%s",lpszSection);
#elif defined(__TSA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\TSA\\%s",lpszSection);
#elif defined(__TDA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\TDA\\%s",lpszSection);
#else
	sprintf(sSubKey,"Software\\Xerofox\\LDS\\%s",lpszSection);
#endif
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_READ,&hKey)==ERROR_SUCCESS&&hKey)
	{
		if(stricmp(lpszEntry,"KaiHeJiaoThreshold")==0)
		{
			if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&sValue[0],&dwLength)== ERROR_SUCCESS)
				part_map.angle.fKaiHeJiaoThreshold=atof(sValue);
			else 
				return FALSE;
			CLDSLineAngle::KAIHEJIAO_ANGLE_THRESHOLD=part_map.angle.fKaiHeJiaoThreshold;	//Ĭ�Ͽ��ϽǱ�ע��ֵΪ2��
		}
		else if(stricmp(lpszEntry,"KaiHeJiaoMinDeita")==0)
		{
			if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&sValue[0],&dwLength)== ERROR_SUCCESS)
				part_map.angle.fKaiHeJiaoMinDeita=atof(sValue);
			else 
				return FALSE;
		}
		else if(stricmp(lpszEntry,"MatCharPosType")==0)
		{
			DWORD dwValue=0;
			dwLength=sizeof(DWORD);
			if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&dwValue,&dwLength)== ERROR_SUCCESS)
				general_set.iMatCharPosType = (UINT)dwValue;
			else 
				return FALSE;
		}
		else if(stricmp(lpszEntry,"MatSeparator")==0)
		{
			DWORD dwValue=0;
			if(RegQueryValueEx(hKey,lpszEntry, NULL, &dwDataType,(BYTE*)&dwValue, &dwLength)== ERROR_SUCCESS)
				general_set.cMatCharSeparator=(BYTE)dwValue;
			else
				return FALSE;
		}
		else if(stricmp(lpszEntry,"m_iPlankWeightStatType")==0)
		{
			DWORD dwValue=0;
			dwLength=sizeof(DWORD);
			if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&dwValue,&dwLength)== ERROR_SUCCESS)
				CLDSPlate::m_iPlankWeightStatType = (UINT)dwValue;
			else 
				return FALSE;
		}
		else if(stricmp(lpszEntry,"m_iCirclePlankAreaType")==0)
		{
			DWORD dwValue=0;
			dwLength=sizeof(DWORD);
			if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&dwValue,&dwLength)== ERROR_SUCCESS)
				CLDSParamPlate::m_iCirclePlankAreaType = (UINT)dwValue;
			else 
				return FALSE;
		}
		else if(stricmp(lpszEntry,"MinHuoquAngle")==0)
		{
			if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&sValue[0],&dwLength)== ERROR_SUCCESS)
				part_map.fMinHuoquAngle=atof(sValue);
			else 
				return FALSE;
		}
		else if(stricmp(lpszEntry,"InnerOffThreshold")==0)
		{
			if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&sValue[0],&dwLength)== ERROR_SUCCESS)
				part_map.plate.fInnerOffThreshold=atof(sValue);
			else 
				return FALSE;
		}
		RegCloseKey(hKey);
	}
	else 
		return FALSE;
	return TRUE;
}
void CDrawSysPara::WritePublicSysParaToReg(LPCTSTR lpszSection, LPCTSTR lpszEntry)	//���湲�ò�����ע���
{
	char sValue[MAX_PATH]="";
	char sSubKey[MAX_PATH]="";
	DWORD dwLength=0;
#ifdef __TMA_FILE_
#ifdef AFX_TARG_ENU_ENGLISH
	sprintf(sSubKey,"Software\\Xerofox\\TMA-EN\\%s",lpszSection);
#else
	sprintf(sSubKey,"Software\\Xerofox\\TMA\\%s",lpszSection);
#endif
#elif defined(__LMA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\LMA\\%s",lpszSection);
#elif defined(__TSA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\TSA\\%s",lpszSection);
#elif defined(__TDA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\TDA\\%s",lpszSection);
#else
	sprintf(sSubKey,"Software\\Xerofox\\LDS\\%s",lpszSection);
#endif
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_WRITE,&hKey)==ERROR_SUCCESS&&hKey)
	{
		if(stricmp(lpszEntry,"KaiHeJiaoThreshold")==0)
		{
			sprintf(sValue,"%f",part_map.angle.fKaiHeJiaoThreshold);
			dwLength=strlen(sValue);
			RegSetValueEx(hKey,lpszEntry,NULL,REG_SZ,(BYTE*)&sValue[0],dwLength);
		}
		else if(stricmp(lpszEntry,"KaiHeJiaoMinDeita")==0)
		{
			sprintf(sValue,"%f",part_map.angle.fKaiHeJiaoMinDeita);
			dwLength=strlen(sValue);
			RegSetValueEx(hKey,lpszEntry,NULL,REG_SZ,(BYTE*)&sValue[0],dwLength);
		}
		else if(stricmp(lpszEntry,"MatCharPosType")==0)
		{
			dwLength=sizeof(int);
			RegSetValueEx(hKey,lpszEntry,NULL,REG_DWORD,(BYTE*)&general_set.iMatCharPosType,dwLength);
		}
		else if(stricmp(lpszEntry,"MatSeparator")==0)
		{
			dwLength=sizeof(int);
			BYTE biValue=(BYTE)general_set.cMatCharSeparator;
			RegSetValueEx(hKey,lpszEntry,NULL,REG_DWORD,(BYTE*)&biValue,dwLength);
		}
		else if(stricmp(lpszEntry,"m_iPlankWeightStatType")==0)
		{
			dwLength=sizeof(int);
			RegSetValueEx(hKey,lpszEntry,NULL,REG_DWORD,(BYTE*)&CLDSPlate::m_iPlankWeightStatType,dwLength);
		}
		else if(stricmp(lpszEntry,"m_iPlankWeightStatType")==0)
		{
			dwLength=sizeof(int);
			RegSetValueEx(hKey,lpszEntry,NULL,REG_DWORD,(BYTE*)&CLDSParamPlate::m_iCirclePlankAreaType,dwLength);
		}
		else if(stricmp(lpszEntry,"MinHuoquAngle")==0)
		{
			sprintf(sValue,"%f",part_map.fMinHuoquAngle);
			dwLength=strlen(sValue);
			RegSetValueEx(hKey,lpszEntry,NULL,REG_SZ,(BYTE*)&sValue[0],dwLength);
		}
		else if(stricmp(lpszEntry,"InnerOffThreshold")==0)
		{
			sprintf(sValue,"%f",part_map.plate.fInnerOffThreshold);
			dwLength=strlen(sValue);
			RegSetValueEx(hKey,lpszEntry,NULL,REG_SZ,(BYTE*)&sValue[0],dwLength);
		}
		RegCloseKey(hKey);
	}
}
BOOL CDrawSysPara::GetDrawingSysParaFromReg(LPCTSTR lpszSection, LPCTSTR lpszEntry)	
{
	char sValue[MAX_PATH]="";
	char sSubKey[MAX_PATH]="";
	DWORD dwDataType,dwLength=MAX_PATH;
#ifdef __TMA_FILE_
#ifdef AFX_TARG_ENU_ENGLISH
	sprintf(sSubKey,"Software\\Xerofox\\TMA-EN\\%s",lpszSection);
#else
	sprintf(sSubKey,"Software\\Xerofox\\TMA\\%s",lpszSection);
#endif
#elif defined(__LMA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\LMA\\%s",lpszSection);
#elif defined(__TSA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\TSA\\%s",lpszSection);
#elif defined(__TDA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\TDA\\%s",lpszSection);
#else
	sprintf(sSubKey,"Software\\Xerofox\\LDS\\%s",lpszSection);
#endif
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_READ,&hKey)==ERROR_SUCCESS&&hKey)
	{
		if(stricmp(lpszEntry,"CompanyName")==0)
		{
			if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&sValue[0],&dwLength)== ERROR_SUCCESS)
				strcpy(m_sCompanyName,sValue);
			else 
				return FALSE;
		}
		else if(stricmp(lpszEntry,"Critic")==0)
		{
			if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&sValue[0],&dwLength)== ERROR_SUCCESS)
				strcpy(Ta.m_sCritic,sValue);
			else 
				return FALSE;
		}
		RegCloseKey(hKey);
	}
	else 
		return FALSE;
	return TRUE;
}
void CDrawSysPara::WriteDrawingSysParaToReg(LPCTSTR lpszSection, LPCTSTR lpszEntry)
{
	char sValue[MAX_PATH]="";
	char sSubKey[MAX_PATH]="";
	DWORD dwLength=0;
#ifdef __TMA_FILE_
#ifdef AFX_TARG_ENU_ENGLISH
	sprintf(sSubKey,"Software\\Xerofox\\TMA-EN\\%s",lpszSection);
#else
	sprintf(sSubKey,"Software\\Xerofox\\TMA\\%s",lpszSection);
#endif
#elif defined(__LMA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\LMA\\%s",lpszSection);
#elif defined(__TSA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\TSA\\%s",lpszSection);
#elif defined(__TDA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\TDA\\%s",lpszSection);
#else
	sprintf(sSubKey,"Software\\Xerofox\\LDS\\%s",lpszSection);
#endif
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_WRITE,&hKey)==ERROR_SUCCESS&&hKey)
	{
		if(stricmp(lpszEntry,"CompanyName")==0)
		{
			sprintf(sValue,"%s",m_sCompanyName);
			dwLength=strlen(sValue);
			RegSetValueEx(hKey,lpszEntry,NULL,REG_SZ,(BYTE*)&sValue[0],dwLength);
		}
		else if(stricmp(lpszEntry,"Critic")==0)
		{
			sprintf(sValue,"%s",Ta.m_sCritic);
			dwLength=strlen(sValue);
			RegSetValueEx(hKey,lpszEntry,NULL,REG_SZ,(BYTE*)&sValue[0],dwLength);
		}
		RegCloseKey(hKey);
	}
}
void CDrawSysPara::InitDrawingEnv()
{
	CSizeTextDim::dimStyle.textHeight=sys.dim_map.fDimTextSize;
	//CKaiHeAngleDim::dimStyle.textHeight=sys.dim_map.fDimTextSize;
}
BOOL CDrawSysPara::IsDeformed(BOOL bPatternDrawing)
{
	if(sys.part_map.iExportDeformedProfile==1)	//�����ǻ�������
		return TRUE;
	else if(sys.part_map.iExportDeformedProfile==2&&bPatternDrawing)	//����ͼ���ǻ�������
		return TRUE;
	else if(sys.part_map.iExportDeformedProfile==3&&!bPatternDrawing)	//���տ����ǻ�������
		return TRUE;
	else
		return FALSE;
}
CStringKeyHashTable<long>CTextItemProp::propHashtable;	//�����ַ�����ϣ��
const DWORD HASHTABLESIZE = 500;
CTextItemProp textItemProp;
CTextItemProp::CTextItemProp()
{
	_pSysPara=NULL;
	InitTextItemPropHashtable();
	TEXT_PROP textProp;
	textProp.fHigh = 2.0;
	textProp.fWtoH = 0.7;
	sprintf(textProp.sFont,"%s","gbhzfs.shx");
	SetTextItemProp("DimText",textProp);
	SetTextItemProp("DimAngleText",textProp);
	SetTextItemProp("PartNo",textProp);
	textProp.fHigh = 2.0;
	SetTextItemProp("PartGuigeText",textProp);
	SetTextItemProp("JgOddmentText",textProp);
	textProp.fHigh = 2;
	SetTextItemProp("LsGuigeText",textProp);
	SetTextItemProp("PlateThickText",textProp);
	SetTextItemProp("OtherText",textProp);
	textProp.fHigh = 6;
	SetTextItemProp("HeaderText",textProp);
	textProp.fHigh = 4;
	SetTextItemProp("CommonText",textProp);
	textProp.fHigh = 3;
	SetTextItemProp("NumericText",textProp);
	textProp.fHigh = 0.3;
	textProp.fWtoH = 0.7;
	SetTextItemProp("PartNoMargin",textProp);
	textProp.fHigh = 2;
	textProp.fWtoH = 0.3;
	SetTextItemProp("SimPolyText",textProp);
	textProp.fHigh = 4;//�߶�
	textProp.fWtoH = 8;//���
	sprintf(textProp.sFont,"%s","�ڴ���");
	SetTextItemProp("CutawayViewSymbolText",textProp);

	textProp.fHigh = 4;
	textProp.fWtoH = 10;//ԲȦֱ��
	SetTextItemProp("SegCircleD",textProp);
	textProp.fHigh = 3;
	textProp.fWtoH = 14;//ԲȦֱ��
	SetTextItemProp("MainPartCircleD",textProp);
	textProp.fWtoH = 12;//ԲȦֱ��
	SetTextItemProp("MainPartInsertCircleD",textProp);
	textProp.fWtoH = 8;//ԲȦֱ��
	SetTextItemProp("OtherPartCircleD",textProp);
	textProp.fHigh = 4;//б��
	textProp.fWtoH = 2;//ԲȦֱ��
	SetTextItemProp("LsMapM16AndM20",textProp);
	textProp.fHigh = 4.8;//б��
	textProp.fWtoH = 2.4;//ԲȦֱ��
	SetTextItemProp("LsMapM24",textProp);
	textProp.fHigh = 4;//б��
	textProp.fWtoH = 10;//ԲȦֱ��
	SetTextItemProp("Seg",textProp);
}

void CTextItemProp::SetTextSize(char *sType,double high, BYTE class_datum0_sum1_chief2_seg3_part4_weld5/*=0*/)
{
	TEXT_PROP textProp;
	textProp.fHigh = high;
	SetTextItemProp(sType,textProp,class_datum0_sum1_chief2_seg3_part4_weld5);
}

void CTextItemProp::SetTextWtoH(char *sType,double WtoH, BYTE class_datum0_sum1_chief2_seg3_part4_weld5/*=0*/)
{
	TEXT_PROP textProp;
	textProp.fWtoH = WtoH;
	SetTextItemProp(sType,textProp,class_datum0_sum1_chief2_seg3_part4_weld5);
}

void CTextItemProp::SetTextFont(char *sType,char* font, BYTE class_datum0_sum1_chief2_seg3_part4_weld5/*=0*/)
{
	TEXT_PROP textProp;
	sprintf(textProp.sFont,"%s",sType);
	SetTextItemProp(sType,textProp,class_datum0_sum1_chief2_seg3_part4_weld5);
}

bool CTextItemProp::GetSignProp(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5,TEXT_PROP* pFontStyle)
{
	long id = GetPropID(sType);
	TEXT_PROP *pQueryFontStyle=NULL;
	if(class_datum0_sum1_chief2_seg3_part4_weld5!=0)
	{
		long iFirstSign = class_datum0_sum1_chief2_seg3_part4_weld5;
		long combinedId = (iFirstSign<<24)+id;
		pQueryFontStyle=hashListProp.GetValue(combinedId);
	}
	if(pQueryFontStyle==NULL)
		pQueryFontStyle = hashListProp.GetValue(id);
	if(pQueryFontStyle!=NULL)
		*pFontStyle=*pQueryFontStyle;
	else
		return false;
	return true;
}
double CTextItemProp::GetSignProp(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5,
								  int txt_size0_wtoh1/*=0*/)
{
	long id = GetPropID(sType);
	TEXT_PROP *datumTextProp=NULL;
	if(class_datum0_sum1_chief2_seg3_part4_weld5!=0)
	{
		long iFirstSign = class_datum0_sum1_chief2_seg3_part4_weld5;
		long combinedId = (iFirstSign<<24)+id;
		datumTextProp=hashListProp.GetValue(combinedId);
		if(datumTextProp)
		{
			if(txt_size0_wtoh1==0)
				return datumTextProp->fHigh;
			else
				return datumTextProp->fWtoH;
		}
		else
		{
			datumTextProp=hashListProp.GetValue(id);
			if(datumTextProp)
			{
				if(txt_size0_wtoh1==0)
					return datumTextProp->fHigh;
				else
					return datumTextProp->fWtoH;
			}
			else
				return 0;
		}
	}
	else
	{
		datumTextProp = hashListProp.GetValue(id);
		if(datumTextProp)
		{
			if(txt_size0_wtoh1==0)
				return datumTextProp->fHigh;
			else
				return datumTextProp->fWtoH;
		}
		else
			return 0;
	}
}

double CTextItemProp::TextSize(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5/*=0*/)
{
	return GetSignProp(sType,class_datum0_sum1_chief2_seg3_part4_weld5);
}

double CTextItemProp::TextWtoH(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5/*=0*/)
{
	return GetSignProp(sType,class_datum0_sum1_chief2_seg3_part4_weld5,1);
}

char* CTextItemProp::TextFont(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5/*=0*/)
{
	long id = GetPropID(sType);
	TEXT_PROP *datumTextProp;
	if(class_datum0_sum1_chief2_seg3_part4_weld5!=0)
	{
		long iFirstSign = class_datum0_sum1_chief2_seg3_part4_weld5;
		long combinedId = (iFirstSign<<24)+id;
		datumTextProp = hashListProp.GetValue(combinedId);
		if(datumTextProp)	
			return datumTextProp->sFont;
		else
		{
			datumTextProp = hashListProp.GetValue(id);
			if(datumTextProp)
				return datumTextProp->sFont;
			else
				return NULL;
		}
	}
	else
	{
		datumTextProp = hashListProp.GetValue(id);
		if(datumTextProp)
			return datumTextProp->sFont;
		else
			return NULL;
	}
}
void CTextItemProp::SetTextItemProp(char *sType,TEXT_PROP &textProp,
									BYTE class_datum0_sum1_chief2_seg3_part4_weld5/*=0*/)
{
	long id = GetPropID(sType);
	TEXT_PROP* datumTextProp;
	if(class_datum0_sum1_chief2_seg3_part4_weld5!=0)
	{
		long iFirstSign = class_datum0_sum1_chief2_seg3_part4_weld5;
		long combinedId = (iFirstSign<<24)+id;
		datumTextProp = hashListProp.GetValue(combinedId);
		if(!datumTextProp)
		{
			datumTextProp = hashListProp.GetValue(id);
			if(!datumTextProp)
				return;
		}
		if(textProp.fHigh == 0)
			textProp.fHigh = datumTextProp->fHigh;
		if(textProp.fWtoH == 0)
			textProp.fWtoH = datumTextProp->fWtoH;
		if(strlen(textProp.sFont)==0)
			sprintf(textProp.sFont,"%s",datumTextProp->sFont);
		hashListProp.SetValue(combinedId,textProp);
	}
	else
	{
		datumTextProp = hashListProp.GetValue(id);
		if(datumTextProp)
		{
			if(textProp.fHigh == 0)
				textProp.fHigh = datumTextProp->fHigh;
			if(textProp.fWtoH == 0)
				textProp.fWtoH = datumTextProp->fWtoH;
			if(strlen(textProp.sFont)==0)
				sprintf(textProp.sFont,"%s",datumTextProp->sFont);
		}
		hashListProp.SetValue(id,textProp);
	}
}

long CTextItemProp::GetPropID(char* propStr)
{
	long id;
	if(propHashtable.GetValueAt(propStr,id))
		return id;
	else
		return 0;
}
void CTextItemProp::InitTextItemPropHashtable()
{
	int id = 1;
	propHashtable.CreateHashTable(HASHTABLESIZE);
	propHashtable.SetValueAt("DimText",id++);		    //�ߴ��ע����
	propHashtable.SetValueAt("DimAngleText",id++);	//�ߴ��ע�Ƕ�
	propHashtable.SetValueAt("PartNoMargin",id++);      //�ֿ���
	propHashtable.SetValueAt("PartGuigeText",id++);			//������񳤶�
	propHashtable.SetValueAt("SimPolyText",id++);       //������
	propHashtable.SetValueAt("JgOddmentText",id++);		//����ͷ
	propHashtable.SetValueAt("PartNo",id++);			//�������
	propHashtable.SetValueAt("LsGuigeText",id++);       //��˨��������ע
	propHashtable.SetValueAt("PlateThickText",id++);    //�����ʱ�ע
	propHashtable.SetValueAt("HeaderText",id++);     //��ϸ��ͷ
	propHashtable.SetValueAt("CommonText",id++);	 //����
	propHashtable.SetValueAt("NumericText",id++);    //��ϸ������
	propHashtable.SetValueAt("CutawayViewSymbolText",id++); //�������
	propHashtable.SetValueAt("OtherText",id++);	        //��������

	propHashtable.SetValueAt("LsSpaceDimText",id++);	//��˨����ע
	propHashtable.SetValueAt("HuoQuSpaceDimText",id++);	//��������ע
	propHashtable.SetValueAt("SegCircleD",id++);		//�κ�ԲȦֱ��
	propHashtable.SetValueAt("MainPartCircleD",id++);	//���Ĺ�����ԲȦֱ��
	propHashtable.SetValueAt("MainPartInsertCircleD",id++);	//���Ĺ�����ԲȦֱ��
	propHashtable.SetValueAt("OtherPartCircleD",id++);	//����������ԲȦֱ��
	propHashtable.SetValueAt("LsMapM16AndM20",id++);	//��˨ͼ��M16M20
	propHashtable.SetValueAt("LsMapM24",id++);			//��˨ͼ��M24
	propHashtable.SetValueAt("Seg",id++);				//�κ�
}

void CTextItemProp::InitSpecialProp(BYTE class_datum0_sum1_chief2_seg3_part4_weld5)
{
	sys.dim_map.fHeaderTextSize	= TextSize("HeaderText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fCommonTextSize	= TextSize("CommonText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fNumericTextSize = TextSize("NumericText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.PartNo.fPartNoTextSize = TextSize("PartNo",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fDimTextSize = TextSize("DimText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fDimAngleTextSize = TextSize("DimAngleText",class_datum0_sum1_chief2_seg3_part4_weld5);	
	sys.dim_map.fCutawayViewSymbolTextSize = TextSize("CutawayViewSymbolText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fCutawayViewSymbolLen = TextWtoH("CutawayViewSymbolText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.PartNo.fPartNoMargin = TextSize("PartNoMargin",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fPartGuigeTextSize = TextSize("PartGuigeText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fJgOddmentTextSize = TextSize("JgOddmentText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fOtherTextSize = TextSize("OtherText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fSimPolyTextSize = TextSize("SimPolyText",class_datum0_sum1_chief2_seg3_part4_weld5);
	//sys.dim_map.fSimTriScale = TextWtoH("SimPolyText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fPlateThickTextSize = TextSize("PlateThickText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fLsGuigeTextSize = TextSize("LsGuigeText",class_datum0_sum1_chief2_seg3_part4_weld5);
}
/////////////////////////////////////////////////////////////////////
FRAMESIZE IFontStyleConfig::GetLabelFrame()
{
	FRAMESIZE frame;
	CDrawSysPara* pSysPara=(Parent()->pSysPara!=NULL)?Parent()->pSysPara:&sys;
	frame.siFrameStyle=pSysPara->dim_map.PartNo.iPartNoFrameStyle;
	frame.fFrameWidth=pSysPara->dim_map.PartNo.fPartNoCirD;
	frame.fMargin=this->fPartNoMargin;
	frame.fTextHeight=this->fPartNoTextSize;
	return frame;
}
FRAMESIZE IFontStyleConfig::GetLabelFlFrame()
{
	FRAMESIZE frame;
	CDrawSysPara* pSysPara=(Parent()->pSysPara!=NULL)?Parent()->pSysPara:&sys;
	frame.siFrameStyle=pSysPara->dim_map.PartNo.iFlPartNoFrameStyle;
	frame.fFrameWidth=pSysPara->dim_map.PartNo.fPartNoCirD;
	frame.fMargin=this->fPartNoMargin;
	frame.fTextHeight=this->fPartNoTextSize;
	return frame;
}
FRAMESIZE IFontStyleConfig::GetLabelEndPlateFrame()
{
	FRAMESIZE frame;
	CDrawSysPara* pSysPara=(Parent()->pSysPara!=NULL)?Parent()->pSysPara:&sys;
	frame.siFrameStyle=pSysPara->dim_map.PartNo.iStdInsertPlatePartNoFrameStyle;
	frame.fFrameWidth=pSysPara->dim_map.PartNo.fPartNoCirD;
	frame.fMargin=this->fPartNoMargin;
	frame.fTextHeight=this->fPartNoTextSize;
	return frame;
}
//IFontStyleConfig��ע���ָ߶�����
//����ͷ���ָ�
IMPLEMENT_GET(IFontStyleConfig,fHeaderTextSize)
{
	return Parent()->TextSize("HeaderText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fHeaderTextSize)
{
	Parent()->SetTextSize("HeaderText",value,GetDrawingTypeId());
}
//��ͨ�����⺺�ָ�
IMPLEMENT_GET(IFontStyleConfig,fCommonTextSize)
{
	return Parent()->TextSize("CommonText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fCommonTextSize)
{
	Parent()->SetTextSize("CommonText",value,GetDrawingTypeId());
}
//������������ָ�
IMPLEMENT_GET(IFontStyleConfig,fNumericTextSize)
{
	return Parent()->TextSize("NumericText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fNumericTextSize)
{
	Parent()->SetTextSize("NumericText",value,GetDrawingTypeId());
}
//����������ָ�
IMPLEMENT_GET(IFontStyleConfig,fPartNoTextSize)
{
	return Parent()->TextSize("PartNo",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fPartNoTextSize)
{
	Parent()->SetTextSize("PartNo",value,GetDrawingTypeId());
}
//���ȳߴ��ע�ı���
IMPLEMENT_GET(IFontStyleConfig,fDimTextSize)
{
	return Parent()->TextSize("DimText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fDimTextSize)
{
	Parent()->SetTextSize("DimText",value,GetDrawingTypeId());
}
//�Ƕȳߴ��ע�ı���
IMPLEMENT_GET(IFontStyleConfig,fDimAngleTextSize)
{
	return Parent()->TextSize("DimAngleText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fDimAngleTextSize)
{
	Parent()->SetTextSize("DimAngleText",value,GetDrawingTypeId());
}
//����ͼ��
//DECLARE_PROPERTY(double,fCutawayViewSymbolLen);		//����ͼ������
//����ͼ������߶�
IMPLEMENT_GET(IFontStyleConfig,fCutawayViewSymbolTextSize)
{
	return Parent()->TextSize("CutawayViewSymbolText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fCutawayViewSymbolTextSize)
{
	Parent()->SetTextSize("CutawayViewSymbolText",value,GetDrawingTypeId());
}
//�ṹͼ��ע
//����������ſ�֮��ļ�϶ֵ
IMPLEMENT_GET(IFontStyleConfig,fPartNoMargin)
{
	return Parent()->TextSize("PartNoMargin",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fPartNoMargin)
{
	Parent()->SetTextSize("PartNoMargin",value,GetDrawingTypeId());
}
//����������ָ�		2.5
IMPLEMENT_GET(IFontStyleConfig,fPartGuigeTextSize)
{
	return Parent()->TextSize("PartGuigeText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fPartGuigeTextSize)
{
	Parent()->SetTextSize("PartGuigeText",value,GetDrawingTypeId());
}
//�Ǹ�����ͷ��ע���ָ�	2.5
IMPLEMENT_GET(IFontStyleConfig,fJgOddmentTextSize)
{
	return Parent()->TextSize("JgOddmentText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fJgOddmentTextSize)
{
	Parent()->SetTextSize("JgOddmentText",value,GetDrawingTypeId());
}
//����ע		 2.0
IMPLEMENT_GET(IFontStyleConfig,fPlateThickTextSize)
{
	return Parent()->TextSize("PlateThickText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fPlateThickTextSize)
{
	Parent()->SetTextSize("PlateThickText",value,GetDrawingTypeId());
}
//��˨������ָ� 2.0
IMPLEMENT_GET(IFontStyleConfig,fLsGuigeTextSize)
{
	return Parent()->TextSize("LsGuigeText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fLsGuigeTextSize)
{
	Parent()->SetTextSize("LsGuigeText",value,GetDrawingTypeId());
}
//DECLARE_PROPERTY(double,fSimTriScale);		//�����α���
//�����α�ע	 2.0
IMPLEMENT_GET(IFontStyleConfig,fSimPolyTextSize)
{
	return Parent()->TextSize("SimPolyText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fSimPolyTextSize)
{
	Parent()->SetTextSize("SimPolyText",value,GetDrawingTypeId());
}
//������ͨ���ָ߶�
IMPLEMENT_GET(IFontStyleConfig,fOtherTextSize)
{
	return Parent()->TextSize("OtherText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fOtherTextSize)
{
	Parent()->SetTextSize("OtherText",value,GetDrawingTypeId());
}
