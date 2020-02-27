#ifndef __MENU_FUNC_H_
#define __MENU_FUNC_H_

extern int g_nCurMapSegNo;		//��ǰ�ֶ�ͼ�ķֶκ�
extern int g_nCurBodyNo;		//��ǰ��ͼ�Ķ����
extern double g_fCurMapScale;	//��ͼ�����еĵ�ǰ��ͼ����
extern char g_sFileName[100];     //�ڳ����ϻ��ܱ�ʱʹ���ļ���ƥ��
extern char g_sHelpFilePath[MAX_PATH];//�����ļ�·��
//���ݲ���
void ImportTma();		//�����ļ�
void CloseTma();		//�ر��ļ�
void FileProp();		//�ļ�����
//��ͼ����
void SelModuleNo();		//ѡ��ģ�ͺ�
void SumFrontFace();	//����
void SumSideFace();		//����
void FrontMutiLeg();	//��������
void SideMutiLeg();		//���Ȳ���
void Draw3DLine();		//��ʾ��ά����
void Draw3DLineBC();	//���Ʋ���Ҫ�����ά����
void Draw3dLineGW();	//���ƹ���Ҫ�����ά����
void DrawSolidGW();     //���ƹ���Ҫ�����άʵ��
void Export3DFiles();	//��������ʵ���ļ�
void DrawSolid();        //��ʾʵ��
void TidToDwg();		//����TID�ļ�����DWG(����+ʵ��)
void DimSegI();			//��ע�ֶκ�
void FLPartBOM();		//������ϸ��
void SumTaMat();		//���ϻ��ܱ�
void SumTaMatCfg();		//���ò��ϻ��ܱ�
void ConfigInfoBOM();	//������Ϣ��
void JgZhunJuTable();	//�Ǹ�׼���
void DrawSumMapByAnalysisMode();	//��ѧģʽ��ͼ
//�ṹͼ����
void SelSegI();			//ѡ��ֶκ�
void SpreadFaceLine();	//��ѧ�������չ��ͼ
void SegLine();			//����ͼ
void SegStru();			//�ֶνṹͼ
void SpreadFaceStru();	//չ����ṹͼ
void PartGroupSetupDrawing();//�麸װ��ͼ
void PartGroupBOM();	//�麸��������ϸ��
void SegPartBOM();		//�ֶι�����ϸ��
void BoltPadBOM();		//�ֶ���˨��ϸ��
void DrawFootPlanform();//���Ƶ׽Ű�
//����ͼ��
void InsertMapFrm();
//�����ӡ��
void InsertMKRect();
//�����ͷ���帩��ͼ
void InsertWrapperSideView();
//����ͼ�����еļ���
void CheckDwgFilePartNo();
void MoveTop();			//�ƶ�ͼԪ��ǰ̨
void HideLine();		//��ֱ����������
//��ע����
void DimSpaceSize();	//ģ�ͳߴ�
void DimMapSize();		//ͼ��ߴ�	
void DimFootNailSpace();//��ע�Ŷ����
void DimLsInfo();		//��ע��˨��Ϣ
void DimBoltPad();		//��ע��Ȧ��Ϣ
void DimOddment();		//��ע����ͷ
void PartSpecTbl();		//��������	
void DimPartSpec();		//��ע������ϸ
void DimSimPolyCfg();	//�����α�ע����
void DimSimPoly();		//��ע������
//����ͼ
void PartDrawingClassicAngle();	//�������ɵ��ͽǸֹ��տ�
void PartDrawingOneByOne();		//����������ͼ
void BatchPartDrawing();		//�����������ͼ
void SpecialProcessStatCard();	//���⹤��ͳ�ƿ�
void StdPartGroupProcessCard();	//��׼�麸�����տ�
void SetupAidedPlate();			//��װ�����ְ�
void StdPartDrawingByReadLibFile();	//��ȡ���ļ����ɱ�׼�������ͼ
void PolyJgBendDrawing();		//����Ǹ���ӡͼ
//����
void DefTable();			//���Ʊ��
void RecogizePartBomToTxt();	//��ȡ������ϸ��
void RecogizePartBomToBom();
void CalTwoTubeAngle();		//�������ֹܼ�ļн�
void BreakSpecLine();		//�ض�ָ����
void DrawSpecialWeld();		//���ƺ�����
void DrawSpecTubeBreakLine();//���Ƹֹ��ƶ���
void DrawSpecCutawayViewSymbol();//��������ͼ��
void BatchPlotPartDrawing(); //������ӡ
void PolarCsys();			 //������ת��
void LoadTMAMapArx();		 //����TMA��ͼģ�� wht 10-11-16
void LoadLMAMapArx();		 //����LMA��ͼģ��
void LoadLDSMapArx();		 //����LDS��ͼģ��
void LoadTSAMapArx();		 //����TSA��ͼģ��
void SmartDraw();			 //���ܻ�ͼ
//���ò���
void SystemSetting();	//ϵͳ����
void EnvGeneralSet();	//����
void SysSetImport();	//��������
void SysSetExport();	//��������
void JgGuiGeFileImport();	//����Ǹֹ���
void TubeGuiGeFileImport();	//����ֹܹ���
void CInsertPlateLibImport();//������Ͳ���
void UInsertPlateLibImport();//����U�Ͳ���
void XInsertPlateLibImport();//����X�Ͳ���
void PFlLibImport();	//����ƽ��������
void DFlLibImport();	//����Ժ�������
void GFlLibImport();    //������Է�����
void RFlLibImport();    //�������Է�����
void FlCoupleLevelFileImport();//�Խӷ����������ļ�
#ifdef __PART_DESIGN_INC_//defined(__TSA_FILE_)||defined(__LDS_FILE_)
void PartSpecTbl();
//void DimPartSpec();
void DrawLoadGraph();
#endif
//�˵���δ��ʾ������
void PartGroupStru();	//�麸ͼ
void EnvScaleSet();		//���û�ͼ����
void TransformProcessCard(); //ת�����տ�
void SysSetExportDefault(BOOL bPrompt);	//�������ڱ��湤װ�����ְ���ļ�Ŀ¼�������ļ���
//�ְ岼��
void LayoutPlates();
void ExtractAndLayoutPlates();
#ifdef __BP_ONLY_
//���պ�Դ���Ƶ�����������ӡ����ͼ����
void BatchPrintPartProcessCards();
void OptimalSortSet();		//�Ż���������
#elif defined(__CARD_IMG_VIEWER_)
//�ൺ���ͨ���տ���ȡ�ϴ�����
void AppendOrUpdateProcessCards();
void RetrievedProcessCards();
#endif

#endif