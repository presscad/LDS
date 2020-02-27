#ifndef __SYS_PARAMETER_
#define __SYS_PARAMETER_
#include "HashTable.h"
#include "XhCharString.h"
#include "MemberProperty.h"
typedef struct
{
	int crNode;
	int crOutLine;		//�ɼ������߼�һ������
	int crDashLine;		//���ɼ������߼��Ǳ���ʵ��
	int crLs;			//��˨���ż���ע
	int crDimSimTri;	//���������μ���ע
	int crSingleLine;	//��ʵ�������ڵ���ͼ
	int crAxisLine;		//�㻮�ߣ������ߣ�
	int crHuoQuLine;	//������
	int crDragLine;		//������
}TOWERATOM_COLOR_INDEX;
//ͼ��ߴ�
typedef struct tagMAP_FRAME_SIZE
{
	BYTE style;				//�������0-9
	CXhChar100 sMapFrameName;//ͼ������ A0 A0-2 A1 A1-2 A3 A3-2 A4 A4-2
	UINT nLength;			//ͼ����
	UINT nWidth;			//ͼ�򳤶�
	short nLeftMargin;		//���ҳ�߼�϶
	short nRightMargin;		//�Ҳ�ҳ�߼�϶
	short nTopMargin;		//�ϲ�ҳ�߼�϶
	short nBottomMargin;	//�²�ҳ�߼�϶
	short nBottomRightFixLen;	//ͼ�����²�̶�����
	short nBottomRightFixWidth;	//ͼ�����²�̶����
	int GetInsideLen(){return nLength-nLeftMargin-nRightMargin;}
	int GetInsideWidth(){return nWidth-nTopMargin-nBottomMargin;}
	int GetTableValidWidth(){return nWidth-nTopMargin-nBottomMargin-nBottomRightFixWidth;}
	tagMAP_FRAME_SIZE()
	{
		style=0;	//A0
		sMapFrameName.Copy("A0");
		nLength=1194;
		nWidth=868;
		nLeftMargin=25;
		nRightMargin=nTopMargin=nBottomMargin=10;
		nBottomRightFixLen=280;
		nBottomRightFixWidth=160;
	}
}MAP_FRAME_SIZE;
struct DRAWING_SCALE
{
	double fRealToDraw;			//������ͼ�����ߣ�ʵ�ʳߴ�/��ͼ�ߴ磬��1:20ʱ��fRealToDraw=20
	//0:Բ�ף�ʵ��Բ�׻���ֱ��=�ݿ�ֱ��*(1/fRealToDraw)
	//1:ͼ��, ʵ��ͼ������ֱ��=ͼ��ԭֱ��
	//2:����ͼ����ʵ��ͼ������ֱ��=ͼ��ԭֱ��*(1/fRealToDraw)*fBoltSymbolFactor
	//3:����ֱ��ͼ����ʵ��ͼ������ֱ��=�ݿ�ֱ��*ͼ��ԭֱ��*(1/fRealToDraw)*fBoltDiaFactor
	int iBoltSymbolStyle;	//��˨(��)��ͼ������ʽ0:Բ��1:ͼ��2:����ͼ��;3:����ֱ��ͼ��;���಻����
	double fBoltSymbolFactor;	//����ͼ��ʱ�ı�������
	double fBoltDiaFactor;		//����ֱ��ͼ��ʱ�ı�������
	DRAWING_SCALE(){
		fRealToDraw=1;		//1:1
		iBoltSymbolStyle=1;	//ͼ��
		fBoltSymbolFactor=1;
		fBoltDiaFactor=0.5;
	}
	//������˨(��)��ͼ������ʽ��������Ӧ����˨ͼ��(��)�Ļ�ͼ���ϱ���ϵ��
	double ComplexCoef();
};
struct TEXT_PROP
{
	double fHigh;
	double fWtoH;
	char sFont[16];
	TEXT_PROP(){memset(this,0,sizeof(TEXT_PROP));}
};

class CTextItemProp;
#ifndef __TEXTFRAMDE_SIZE_DEF_
#define __TEXTFRAMDE_SIZE_DEF_
struct FRAMESIZE{
	short siFrameStyle;	//-1.�Զ��жϱ�ű�עȦ��ʽ;0.ȦȦ;1.��Բ����;2.��ͨ���ο�
	double fMargin;		//�ı���߿�ļ�϶
	double fFrameWidth;	//�߿�Ŀ��
	double fTextHeight;	//����߶�
	//double fFrameHeight;	//�߿�ĸ߶�һ���ɱ߿���ʽ���߾༰�߿��Ⱦ���
};
#endif
struct IFontStyleConfig
{
	virtual int GetDrawingTypeId()=0;
	virtual double FontScale(){return 1.0;}
	virtual CTextItemProp* Parent()=0;
	virtual FRAMESIZE GetLabelFrame();
	virtual FRAMESIZE GetLabelFlFrame();
	virtual FRAMESIZE GetLabelEndPlateFrame();
	__declspec(property(get=GetLabelFrame)) FRAMESIZE labelFrame;
	__declspec(property(get=GetLabelFlFrame)) FRAMESIZE labelFrameFL;
	__declspec(property(get=GetLabelEndPlateFrame)) FRAMESIZE labelFrameEndPlate;
	DECLARE_PROPERTY(double,fHeaderTextSize); 	//����ͷ���ָ�
	DECLARE_PROPERTY(double,fCommonTextSize); 	//��ͨ�����⺺�ָ�
	DECLARE_PROPERTY(double,fNumericTextSize);	//������������ָ�
	DECLARE_PROPERTY(double,fPartNoTextSize);	//����������ָ�
	//���ȱ�ע
	DECLARE_PROPERTY(double,fDimTextSize);		//���ȳߴ��ע�ı���
	DECLARE_PROPERTY(double,fDimAngleTextSize);	//�Ƕȳߴ��ע�ı���
	//����ͼ��
	//DECLARE_PROPERTY(double,fCutawayViewSymbolLen);		//����ͼ������
	DECLARE_PROPERTY(double,fCutawayViewSymbolTextSize);//����ͼ������߶�
	//�ṹͼ��ע
	DECLARE_PROPERTY(double,fPartNoMargin);			//����������ſ�֮��ļ�϶ֵ
	DECLARE_PROPERTY(double,fPartGuigeTextSize);	//����������ָ�		2.5
	DECLARE_PROPERTY(double,fJgOddmentTextSize);	//�Ǹ�����ͷ��ע���ָ�	2.5
	DECLARE_PROPERTY(double,fPlateThickTextSize);	//����ע		 2.0
	DECLARE_PROPERTY(double,fLsGuigeTextSize);	//��˨������ָ� 2.0
	DECLARE_PROPERTY(double,fOtherTextSize);	//������ͨ���ָ߶�
	//DECLARE_PROPERTY(double,fSimTriScale);		//�����α���
	DECLARE_PROPERTY(double,fSimPolyTextSize);	//�����α�ע	 2.0
};
class CDrawSysPara;
class CTextItemProp
{
	CDrawSysPara* _pSysPara;
	void InitTextItemPropHashtable();
	double GetSignProp(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5,int txt_size0_wtoh1=0);
	bool GetSignProp(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5,TEXT_PROP* pFontStyle);
public:
	CHashList<TEXT_PROP>hashListProp;
	static CStringKeyHashTable<long>propHashtable;
	static long GetPropID(char* propStr);	//�õ�����ID
public:
	CTextItemProp();
	void SetParentSysConfig(CDrawSysPara* pSysParaConfig){_pSysPara=pSysParaConfig;}
	READONLY_PROPERTY(CDrawSysPara*,pSysPara);
	GET(pSysPara){return _pSysPara;}
	//cClass:'0'��׼����'1'��ͼ'2'˾��ͼ'3'�ṹͼ'4'����ͼ5'�麸ͼ'
	void SetTextItemProp(char *sType,TEXT_PROP &textProp,BYTE class_datum0_sum1_chief2_seg3_part4_weld5=0);
	void SetTextSize(char *sType,double high, BYTE class_datum0_sum1_chief2_seg3_part4_weld5=0);
	void SetTextWtoH(char *sType,double WtoH, BYTE class_datum0_sum1_chief2_seg3_part4_weld5=0);
	void SetTextFont(char *sType,char* font, BYTE class_datum0_sum1_chief2_seg3_part4_weld5=0);
	double TextSize(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5=0);
	double TextWtoH(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5=0);
	char*  TextFont(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5=0);
	void InitSpecialProp(BYTE class_datum0_sum1_chief2_seg3_part4_weld5);
	static const int BASIC_CONFIG		= 0;	//����������ʽ����
	static const int SUMMARY_DRAWING	= 1;	//��ͼ������ʽ
	static const int CHIEF_DRAWING		= 2;	//˾��ͼ������ʽ
	static const int SEGMENT_DRAWING	= 3;	//�ֶνṹͼ������ʽ
	static const int ANGLE_DRAWING		= 4;	//�Ǹ������ͼ�������տ�)������ʽ
	static const int WELD_DRAWING		= 5;	//�麸ͼ������ʽ
	static const int PLATE_DRAWING		= 6;	//����ͼ�������տ�)������ʽ
	friend struct IFontStyleConfig;
	class BASIC_FONT : public IFontStyleConfig
	{
	public:
		CTextItemProp* Parent(){return ((CTextItemProp*)((BYTE*)this - offsetof(CTextItemProp, basic)));}
		virtual int GetDrawingTypeId(){return BASIC_CONFIG;}
	}basic;

	class SUMMARY_DRAW_FONT : public IFontStyleConfig
	{
	public:
		CTextItemProp* Parent(){return ((CTextItemProp*)((BYTE*)this - offsetof(CTextItemProp, summary)));}
		virtual int GetDrawingTypeId(){return SUMMARY_DRAWING;}
	}summary;	//��ͼ

	class CHIEF_DRAW_FONT : public IFontStyleConfig
	{
	public:
		CTextItemProp* Parent(){return ((CTextItemProp*)((BYTE*)this - offsetof(CTextItemProp, chief)));}
		virtual int GetDrawingTypeId(){return CHIEF_DRAWING;}
	}chief;		//˾��ͼ

	class SEG_DRAW_FONT : public IFontStyleConfig
	{
	public:
		CTextItemProp* Parent(){return ((CTextItemProp*)((BYTE*)this - offsetof(CTextItemProp, segment)));}
		virtual int GetDrawingTypeId(){return SEGMENT_DRAWING;}
	}segment;		//�ֶνṹͼ

	class ANGLE_DRAW_FONT : public IFontStyleConfig
	{
	public:
		double zoomscale;
		ANGLE_DRAW_FONT(){zoomscale=1.0;}
		virtual double FontScale(){return zoomscale;}
		CTextItemProp* Parent(){return ((CTextItemProp*)((BYTE*)this - offsetof(CTextItemProp, angle)));}
		virtual int GetDrawingTypeId(){return ANGLE_DRAWING;}
	}angle;		//�Ǹ����ͼ

	class PLATE_DRAW_FONT : public IFontStyleConfig
	{
	public:
		double zoomscale;
		PLATE_DRAW_FONT(){zoomscale=1.0;}
		virtual double FontScale(){return zoomscale;}
		CTextItemProp* Parent(){return ((CTextItemProp*)((BYTE*)this - offsetof(CTextItemProp, plate)));}
		virtual int GetDrawingTypeId(){return PLATE_DRAWING;}
	}plate;		//����ͼ

	class WELD_DRAW_FONT : public IFontStyleConfig
	{
	public:
		double zoomscale;
		WELD_DRAW_FONT(){zoomscale=1.0;}
		virtual double FontScale(){return zoomscale;}
		CTextItemProp* Parent(){return ((CTextItemProp*)((BYTE*)this - offsetof(CTextItemProp, weld)));}
		virtual int GetDrawingTypeId(){return WELD_DRAWING;}
	}weld;		//�麸ͼ
}; 
class CDrawSysPara
{
public:
	CDrawSysPara();
	CStringKeyHashList<MAP_FRAME_SIZE> m_hashMapFrameList;	//ͼ��ߴ��ϣ��
	int m_iCurMapType;      //��ǰ��ͼ���� 0.��׼���� 1.��ͼ 2.˾��ͼ 3.�ṹͼ 4.����ͼ 5.�麸ͼ
	int m_iCurMapSegNo;		//��ǰ�ֶ�ͼ�ķֶκ�
	int m_iCurBodyNo;		//��ǰ��ͼ�ı����
	double m_fCurMapScale;	//��ͼ�����еĵ�ǰ��ͼ����
	char sFilePath[MAX_PATH];
	int nBriefGuigeN;		//�򻯹����
	//���ķֶα�־�� wht 10-12-22
	BOOL bDrawMainPoleJointLine;//���ķֶα�־��
	int nJointLineOffset;		//�ֶα���߾������׼�Ǹ־���
	int nJointLineLen;			//�ֶα���߳���
	//��������---������ͼģ�鹲�õ�����
	struct GENERAL_SET{
		//��ͼ����
		//����һ�д������iLsApertureStyle֮ǰ�Ĵ��룬��������ظ��� wjh-2014.6.19
		//DRAWING_SCALE StruMapScale,PartMapScale,WeldMapScale;
		//DrawUnit.cpp��Ӧʹ��StruMapScale;PaperPatternDrawing.cpp��Ӧʹ��PartMapScale
		//SegMap.cpp��Ӧʹ��WeldMapScale
		double LineMapScale;		//����ͼ���� һ��Ϊ1:100 1:200 1:300
		double ForceLineScale;		//˾��ͼ(��ѧ����)  һ��Ϊ1:50
		DRAWING_SCALE StruMapScale;		//�ṹͼ���� һ��Ϊ1:10 1:20 1:30 1:15
		DRAWING_SCALE PartMapScale;		//����ͼ���� һ��Ϊ1:5 1:10
		DRAWING_SCALE WeldMapScale;		//�麸ͼ����
		int iMatCharPosType;    //������ŵĲ����ַ�λ��ѡ��ʽ0.����Ҫ 1.�������ǰ 2.������ź�
		char cMatCharSeparator;	//������ŵĲ����ַ��ָ���: ��0  �ո�' '
		double fHoleAugmentInNC;//�׾�����ֵ
		double fWeldLineSymbolHigh;	 //�����߸߶�
		double fWeldLineSymbolSpace; //�����߼�϶
		//��ȷ��
		int iDimPrecision;		//�ߴ羫ȷ��
		int iPartBOMStyle;		//������ϸ����ʽ 0.��׼��ʽ(TMA)��1.�ӿ���ʽ(����) wht 10-11-11
		int iSumMatBOMStyle;	//���ϻ��ܱ���ʽ 0.��ʽһ(���ʴӴ�С,�Ǹֹ��Ӵ�С)��1.��ʽ�� wht 11-05-06
		//���տ�·�������з�ʽ
		BOOL bProcessCardTbl;		//���ɹ��տ�ͼ��
		int nPartRowsN;				//��������ͼÿ������
		int nRowHigh;				//ÿ�й���ͼ�߶�
		int nColumnWidth;			//ÿ�й���ͼ���
		BOOL bCardLayoutBaseRow;	//����Ϊ��׼���й��տ�
		BOOL bHasFrame;				//����װ����
		char m_sJgCard[MAX_PATH];
		char m_sSlotCard[MAX_PATH];	//�۸ֹ��տ� wht 10-09-14
		char m_sTubeCard[MAX_PATH];
		char m_sPlankCard[MAX_PATH];
		char m_sWeldPartGroupCard[MAX_PATH];
		char m_sProcessPartStatCard[MAX_PATH];
		char m_sSetupAidedPlate[MAX_PATH]; //��װ�����ְ���ļ�·��
		//��ͼģ��·��
		char m_sTMAMapArxPath[MAX_PATH];	//TMA
		char m_sLMAMapArxPath[MAX_PATH];	//LMA
		char m_sLDSMapArxPath[MAX_PATH];	//LDS
		char m_sTSAMapArxPath[MAX_PATH];	//TSA
		//���������ֻ��ƽ�����
		COLORREF lCrossarmCr; //��ᵣ��ɫ
		COLORREF rCrossarmCr; //�Һᵣ��ɫ
	}general_set;
	//�ṹͼ����
	struct{
		BOOL bExportByEnglish;
		double fMinPartThick;	//�Ǹ֡��ֹܡ��ְ�ȹ�����С���ƺ�� wht 12-03-24
		double fMinWingWide; 	//�Ǹ���С֫��/�ֹ���С�뾶 	
		BOOL bDimSpecialLsSpace;//��ע������˨���
		BOOL bDimSpecialJgZhun;	//��ע����Ǹ�׼��
		BOOL bDimStruJgLen;		//�ṹͼ�б�ע�Ǹֳ���
		BOOL bDimPartGuiGe;		//�ṹͼ�б�ע������� wht 11-03-29
		BOOL bDrawCutAngleAndWingInfo;	//�����нǡ�֫��Ϣ
		BOOL bOnlyDrawHorizontalPlate;	//�������ڵ�ǰ�ṹͼ��ˮƽ��ʾ�ĸְ� wht 10-06-15
		BOOL bMergeAttachPlatesToBasePlateInBOM;	//�ֶ���ϸ���н������ӹ�����ʮ�ֲ��������ϲ��������ף����У���������� wjh-17.5.26
		BOOL bUsePolyLineDrawEdge;		//�ö���߻��ƽǸֺ͸ְ��������	 wht 10-06-15
		double fPolyLineWidth;          //����߿��                         xc 10-12-17
		double fPoleAxisLineStartOutSpreadLen;	//�˼�����ʼ�����쳤�� wht 10-12-13
		double fPoleAxisLineEndOutSpreadLen;	//�˼������ն����쳤��
		BOOL bReadAdditionalBuffer;		//��ȡ���ӻ��� wht 11-08-03
	}stru_map;
	//�麸ͼ����
	struct WELDDRAWING{
		double  fMinFontSize;			//������С�߶� Ĭ��Ϊ2.5
		double  fMinDrawThick;			//�����С����ֵ
		int		nKmJgGIncrement;		//��ģ�Ǹ��ľ�����ֵ
		BOOL bModulatePartGroupLen;		//�����麸������
		double fPartGroupDistThreshold;	//�麸ͼ������ֵ
		double fPartGroupDistZoomCoef;	//�麸ͼ����ϵ��
		BOOL bDrawWeldLineSymbol;		//�麸ͼ�л��ƺ����߱��
		int  iPlateLocateDrawStyle;		//�麸ͼ�л��Ƹְ�Ի����Ʒ�ʽ��0.������;1.������;2.��������
		int  iLocateSymboleHeight;		//�麸ͼ�жԻ���Ǹ߶�
		BOOL bDimEndOuterPlaneSpace;	//�˼�����浽��������
		BOOL bAutoDimPartNo;			//�Զ���ע�������
		BOOL bAutoMatchView;			//�Զ�����麸����ͼ
		BOOL bDimAbsoluteDist;			//��ע��λ����Գߴ�
		BOOL bDimAfterWeldCheckLine;	//��ע���Ӻ�����
		BOOL bDimBeforeWeldLocatSpace;	//��ע����ǰ��λ�ߴ�
		BOOL bDimPlatePoleRayAndAngle;	//��ע�ְ������Լ��������׼��֮��ļн�
		BOOL bSmartFilterRedundantPart;	//��ͼ���Զ�ɸѡ���뵱ǰ��ͼ�޹صĶ��๹����������ʾ wjh-2014.3.29
		BOOL bAngularDimension;			//��ע�Ƕ���
		BOOL bCreatePartGroupBOM;		//������ϸ��
		//������ϸ��
		struct{
			BOOL bZoomInProcessCard;	//���տ��ڽ��б�������
			BOOL bDispPartGroupNo;		//�麸�����
			BOOL bDispPartGroupNum;		//�麸������
			BOOL bDispPartNo;			//�������
			BOOL bDispPartNum;			//��������
			BOOL bDispPartGuiGe;		//�������
			BOOL bDispPartLen;			//��������
			BOOL bDispPartWeight;		//���������Լ�ͬһ��Ź�������С�� ����
			BOOL bDispNotes;			//��ע
			BOOL bDispInternal;			//��ERP�����[����]���켶��
			BOOL bDispAppearance;		//��ERP�����[���]���켶��
		}part_table;
	}part_group_map;
	//����ͼ���� ��������ͼ�������� �Ǹֹ���ͼ���� �ְ幹��ͼ����
	struct{
		BOOL bExportDeformedProfile;	//����ͼ���ǻ�������
		int  iExportDeformedProfile;	//�������ο��Ƿ�����0.������1.����2.����ͼ���� 3.���տ�����  wh-17-03-27
		BOOL bDimDeformedProfileText;	//���ǻ�������ʱ�Ƿ��ע���� wht 10-11-01
		BOOL bDimPlaneAngle;			//��ע����������н�
		int	 iDimAngleType;				//�����Ƕȱ�ע���� 0.��� 1.�۽� 2.�Զ� wht 10-11-01
		double fMinHuoquAngle;			//������С�����Ƕȣ�С�ڴ˻����Ƕȵ����乹������ע������Ϣ
		BOOL bDimHuoquType;				//��ע������������
		BOOL bUseZhengFanQun;			//���������� Ĭ��ʹ����������  wht 11-05-07
		int  iPartMapStyle;				//0��ʾ��ͨ;1��ʾ�㽭ʢ��
		struct{
			BOOL bModulateLongJg;		//�����Ǹֳ��� ��δʹ�ã���iJgZoomSchema����ñ��� wht 11-05-07
			int iJgZoomSchema;			//�Ǹֻ��Ʒ�����0.1:1���� 1.ʹ�ù���ͼ���� 2.����ͬ������ 3.����ֱ�����
			BYTE ciJgLenDrawMode;		//�Ǹֽ��г���ֱ����ţ���ͼ���ȳ������տ�ʱ�Ĵ���ģʽ��0.��������(����) 1.�м伷ѹ
			BOOL bMaxExtendAngleLength;	//����޶�����Ǹֻ��Ƴ���
			double fLsDistThreshold;	//�Ǹֳ����Զ�������˨�����ֵ(���ڴ˼��ʱ��Ҫ���е���);
			double fLsDistZoomCoef;		//��˨�������ϵ��
			BOOL bOneCardMultiPart;		//�Ǹ�����һ��������
			int  iJgGDimStyle;			//0.ʼ�˱�ע  1.�м��ע 2.�Զ��ж�
			int  nMaxBoltNumStartDimG;	//������ʼ�˱�ע׼��֧�ֵ������˨��
			int  iLsSpaceDimStyle;		//0.X�᷽��	  1.Y�᷽��  2.�Զ��ж� 3.����ע  4.�ޱ�ע����(X�᷽��)  4.����ߴ��ߣ��ޱ�ע����(X�᷽��)��Ҫ���ڽ���(��)�����ʼ���ä������
			int  nMaxBoltNumAlongX;		//��X�᷽���ע֧�ֵ������˨����
			BOOL bDimCutAngle;			//��ע�Ǹ��н�
			BOOL bDimCutAngleMap;		//��ע�Ǹ��н�ʾ��ͼ
			BOOL bDimPushFlatMap;		//��עѹ��ʾ��ͼ
			BOOL bJgUseSimpleLsMap;		//�Ǹ�ʹ�ü���˨ͼ��
			BOOL bDimLsAbsoluteDist;	//��ע��˨���Գߴ�
			BOOL bMergeLsAbsoluteDist;	//�ϲ����ڵȾ���˨���Գߴ� �����������:��ʱҲ��Ҫ�� wjh-2014.6.9
			BOOL bDimRibPlatePartNo;	//��ע�Ǹ��߰���
			BOOL bDimRibPlateMaterial;	//��ע�Ǹ��߰����
			BOOL bDimRibPlateSetUpPos;	//��ע�Ǹ��߰尲װλ��
			//�нǱ�ע��ʽһ
			//�нǱ�ע��ʽ�� B:��ͷ�ߴ� L:֫�߳ߴ� C:��߳ߴ� 
			//BXL �н�  CXL ��֫ BXC �н�  �д��=�н�+��֫
			int	 iCutAngleDimType;		//�нǱ�ע��ʽ 0.��ʽһ  1.��ʽ�� wht 10-11-01
			//
			BOOL bDimKaiHe;				//��ע�Ǹֿ��Ͻ�
			BOOL bDimKaiheAngleMap;		//��ע�Ǹֿ��Ͻ�ʾ��ͼ
			double fKaiHeJiaoThreshold; //���ϽǱ�ע��ֵ(��) wht 11-05-06
			double fKaiHeJiaoMinDeita;	//���Ͻ������С��϶ wjh 2015.2.6
			//�������ϽǱ�ע���� wht 12-03-13
			BOOL bDimKaiheSumLen;		//��ע���������ܳ�
			BOOL bDimKaiheAngle;		//��ע���϶���	
			BOOL bDimKaiheSegLen;		//��ע��������ֶγ�
			BOOL bDimKaiheScopeMap;		//��ע���������ʶ��
		}angle;
		struct{
			BOOL bMarkHuoquPos;			//��ǻ���λ��
			BOOL bDrawHuoQuPrjMap;		//���ƻ���ʾ��ͼ
			BOOL bDrawHuoQuMarkLine;	//���ƻ���������
			BOOL bAdjustPlateUcs;		//�����ְ�����ϵ��֤�����ˮƽ����
			BOOL bDimPlateRayLs;		//��ע����˨���߾���
			BOOL bDimPlateBoltAngle;	//��ע����˨���߽Ƕ�
			BOOL bDimBoltEdgeSpace;		//��ע����˨�߾�
			BOOL bDimDiffRayLsSpace;	//��ע��ͬ������˨���
			BOOL bDimLsToHuoquLine;		//��ע��˨�������߼��
			BOOL bDimLsToWeldEdge;		//��ע��˨�����ӱ߼��
			BOOL bDimPlateEdgeLen;		//��ע�ְ�߳�
			BOOL bDimArcEdgeR;			//��ע�ְ�Բ���߰뾶
			BOOL bDimArcAngle;			//��ע�ְ�Բ���߽Ƕ�
			BOOL bDrawInnerEdge;        //������������
			double fInnerEdgeOffset;    //������ƫ�ƾ�
			BOOL bDimCoord;				//�幹��ͼ���Ƿ��ע����
			BOOL bPlateUseSimpleLsMap;	//�ְ�ʹ�ü���˨ͼ��
			BOOL bZoomDimTextSizeByWidth;//���ݸְ峤���н�Сֵ���Ÿְ���ر�ע������߶�
			double fTextSizeZoomCoef;	//����߶�����֮�������ϵ��
			BOOL bDrawSlopeCutLine;     //���촦���¿���
			BOOL bDrawDatumLine;		//���Ƹְ��׼��(�ֹ�����),�ֹ��Ͻڵ��һ����˨����Ӧ���������˻�׼��
			int  iCutArcsType;          //�������Ǵ����л���ʽ 0--���� 1--�н� 2--�л�
			int	 iZhiWanLineType;		//���������λ�ô���ʽ 0--���(��ǰ����ʽ) 1.�ڲ�(���ڴ���ʽ)
			double fInnerOffThreshold;	//�������������ƫ����ֵ(ƫ��һ�����Ļ�����)
			BOOL bDisplaypPartInfoTbl;  //��ʾ������Ϣ��
			int  nPartInfoTblColumnWidth;//��������п�
			BOOL bUseDimStyleConfigFile;//���øְ��ע��ʽ�����ļ�
			BOOL bShowPlateCfgInfoDlg;	//�Ƿ���ʾ�ְ����öԻ���
			char sPlateConfigFilePath[MAX_PATH];//�ְ������ļ�·��
		}plate;
		struct{
			double fSlotWidthProcessError;	//���ۿ�ȼӹ����ֵ
			double fSlotLenProcessError;	//���۳��ȼӹ����ֵ
			double fReelTubeThresholdD;		//�ְ���ֱ����ֵ ���ڸ�ֱ���ĸֹ�Ϊ�ְ���
		}tube;
	}part_map;
	struct{
		TOWERATOM_COLOR_INDEX crIndex;//������ɫ
		char sFontName[MAX_PATH];	//Ĭ��ʹ������ gbhzfs.shx
		//���ϱ������ݼ��ṹͼ�ڹ�����Ÿߣ�3.0��gbhzfs.shx
		double	fHeaderTextSize;	//����ͷ���ָ�
		double	fCommonTextSize;	//��ͨ�����⺺�ָ�
		double	fNumericTextSize;	//������������ָ�
		//���������ز���
		struct{
			int		iPartNoFrameStyle;	//��ſ����� //0.ԲȦ 1.��Բ���ľ��ο� 2.���ο�	3.�Զ��ж�
			int		iStdInsertPlatePartNoFrameStyle;//��׼����ſ����� wht 12-03-19	 
			int		iFlPartNoFrameStyle;			//������ſ����� 
			double	fPartNoTextSize;	//����������ָ�
			double	fPartNoCirD;		//�������Ȧֱ��
			double	fPartNoMargin;		//����������ſ�֮��ļ�϶ֵ 
			BOOL	bDrawRefLine;		//����������
			double  fMainPoleNoOuterCirD;//���ı����Բֱ��
			double  fMainPoleNoInnerCirD;//���������Բֱ��
			int		iMirMsg;			//0:X��Գƣ�1:Y��Գ�;���಻�Գƴ��� wht 11-03-29
		}PartNo;
		//�κű�ע��ز���	wht 11-06-27
		double fSegICirD;			//�κ�Ȧֱ��
		double fSegITextSize;		//�κ����ָ�
		//���ȱ�ע
		double	fDimTextSize;		//���ȳߴ��ע�ı���
		double	fDimAngleTextSize;	//�Ƕȳߴ��ע�ı���
		double	fDimArrowSize;		//�ߴ��ע��ͷ��
		char	sArrowType[50];		//�ߴ��ע��ͷ���� wht 10-07-27
		double  fDimGap;			//�������ע��֮��ļ�� wht 10-08-02
		//����ͼ��
		double	fCutawayViewSymbolLen;		//����ͼ������
		double	fCutawayViewSymbolTextSize;	//����ͼ������߶�
		double	fCutawayViewPolyLineWidth;	//����ͼ����߿��
		//����������
		double	fSimTriScale;		//�����α���
		double  fSimPolyTextSize;	//�����α�ע	 2.0
		//�����ָ�����
		double	fLsGuigeTextSize;	//��˨������ָ� 2.0
		double  fOtherTextSize;		//������ͨ���ָ߶�
		double	fPartGuigeTextSize;	//����������ָ�		2.5
		double	fJgOddmentTextSize;	//�Ǹ�����ͷ��ע���ָ�	2.5
		double  fPlateThickTextSize;//����ע		 2.0
	}dim_map;	//ͼֽ�ϵı�ע
	double TEXT_X_FACTOR(){return 0.7;}
	BOOL GetPublicSysParaFromReg(LPCTSTR lpszSection, LPCTSTR lpszEntry);	//��ע����л�ȡ���ò��� wht 11-06-10
	void WritePublicSysParaToReg(LPCTSTR lpszSection, LPCTSTR lpszEntry);	//���湲�ò�����ע���
	BOOL IsDeformed(BOOL bPatternDrawing);
	void InitDrawingEnv();
	CTextItemProp fonts;
	struct TEXTSIZECFG{
		bool bZoomAdaptiveWithScale;	//���ͼ�����涯����
		WORD wiScaleModel2Draw;			//��ǰ�趨����߶�����Ӧ�Ļ�׼��ͼ����=ģ�ͳߴ�/ͼֽ�ߴ硡��1:20��Ϊ20
		TEXTSIZECFG(){bZoomAdaptiveWithScale=true;wiScaleModel2Draw=20;}
	}summary,chief,segment,welding,part;
	//
	char m_sCompanyName[101];	//��˾����
	void WriteDrawingSysParaToReg(LPCTSTR lpszSection, LPCTSTR lpszEntry);
	BOOL GetDrawingSysParaFromReg(LPCTSTR lpszSection, LPCTSTR lpszEntry);
};

extern CDrawSysPara sys;
//extern CTextItemProp textItemProp;
#endif