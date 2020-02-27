//<LOCALE_TRANSLATE BY hxr /> 2015-04-25
//����
#include "stdafx.h"
#include "stdarx.h"
#include "MenuFunc.h"
#include "SetMapScaleDlg.h"
#include "DrawSettingSheet.h"
#include "SysPara.h"
#include "database.h"
#include "DecarlToPolarDlg.h"
#include "AcExtensionModule.h"
#include "RxTools.h"
#include "SystemSettingDlg.h"
#include "PlankPartParaExportDlg.h"

CSetMapScaleDlg dlg;
void EnvScaleSet()
{
    // When resource from this ARX app is needed, just
    // instantiate a local CAcModuleResourceOverride
    CAcModuleResourceOverride resOverride;

	//���õ���ͼ���ֶνṹͼ����
	dlg.m_nLineMapScale = ftoi(sys.general_set.LineMapScale);
	dlg.m_nForceLineScale = ftoi(sys.general_set.ForceLineScale);
	dlg.m_nStruMapScale = ftoi(sys.general_set.StruMapScale.fRealToDraw);
	dlg.m_nPartMapScale = ftoi(sys.general_set.PartMapScale.fRealToDraw);
	if(dlg.DoModal()==IDOK)
	{
		sys.general_set.LineMapScale = dlg.m_nLineMapScale;
		sys.general_set.ForceLineScale = dlg.m_nForceLineScale;
		sys.general_set.StruMapScale.fRealToDraw = dlg.m_nStruMapScale;
		sys.general_set.PartMapScale.fRealToDraw = dlg.m_nPartMapScale;
	}
}
void SysSetImportDefault(BOOL bPrompt)
{
	FILE *fp;
	CString sFileName,sLine;
	char file_name[MAX_PATH],line_txt[MAX_PATH],key_word[100];
	int nKey;
	if(bPrompt)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		CFileDialog dlg(TRUE,"set","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"Draw Setting File(*.set)|*.set||");
#else
		CFileDialog dlg(TRUE,"set","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"��ͼ�����ļ�(*.set)|*.set||");
#endif
		if(dlg.DoModal()!=IDOK)
			return;
		sFileName = dlg.GetPathName();
		sprintf(file_name,"%s",sFileName);
	}
	else
	{
		GetAppPath(file_name);
		strcat(file_name,"map.set");
		//����ARXʱ,��ʼ��������ϸ����
		CXhChar100 plate_cfg_file;
		GetAppPath(plate_cfg_file);
		strcat(plate_cfg_file,"Plate.pds");
		strcpy(sys.part_map.plate.sPlateConfigFilePath,plate_cfg_file);
		sys.part_map.plate.bUseDimStyleConfigFile=CPlankPartParaExportDlg::LoadDimStyleFromFile(plate_cfg_file);	
	}
	if((fp=fopen(file_name,"rt"))==NULL)
	{
		//AfxMessageBox("�򲻿�ָ���������ļ�!");//��ʱ���������� wjh-2015.2.3
		return;
	}
	LayerTable::layerHashList.Empty();
	while(!feof(fp))
	{
		if(fgets(line_txt,MAX_PATH,fp)==NULL)
			break;
		char sText[MAX_PATH];
		strcpy(sText,line_txt);
		sLine=sText;
		sLine.Replace('=',' ');
		sprintf(line_txt,"%s",sLine);
		char *skey=strtok((char*)sText,"=,;");
		strncpy(key_word,skey,100);
		BOOL bContinue=FALSE;	//�������if-else wht 11-07-04
		//��������
		if(_stricmp(key_word,"StruBoltSymbolStyle")==0)
		{
			sscanf(line_txt,"%s%d",key_word,&nKey);
			if(nKey>=0&&nKey<4)
				sys.general_set.StruMapScale.iBoltSymbolStyle=nKey;
			else
				sys.general_set.StruMapScale.iBoltSymbolStyle=0;
		}
		else if(_stricmp(key_word,"StruBoltSymbolFactor")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.general_set.StruMapScale.fBoltSymbolFactor);
		else if(_stricmp(key_word,"StruBoltDiaFactor")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.general_set.StruMapScale.fBoltDiaFactor);
		else if(_stricmp(key_word,"PartBoltSymbolStyle")==0)
		{
			sscanf(line_txt,"%s%d",key_word,&nKey);
			if(nKey>=0&&nKey<4)
				sys.general_set.PartMapScale.iBoltSymbolStyle=nKey;
			else
				sys.general_set.PartMapScale.iBoltSymbolStyle=0;
		}
		else if(_stricmp(key_word,"PartBoltSymbolFactor")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.general_set.PartMapScale.fBoltSymbolFactor);
		else if(_stricmp(key_word,"PartBoltDiaFactor")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.general_set.PartMapScale.fBoltDiaFactor);
		else if(_stricmp(key_word,"WeldBoltSymbolStyle")==0)
		{
			sscanf(line_txt,"%s%d",key_word,&nKey);
			if(nKey>=0&&nKey<4)
				sys.general_set.WeldMapScale.iBoltSymbolStyle=nKey;
			else
				sys.general_set.WeldMapScale.iBoltSymbolStyle=0;
		}
		else if(_stricmp(key_word,"WeldBoltSymbolFactor")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.general_set.WeldMapScale.fBoltSymbolFactor);
		else if(_stricmp(key_word,"WeldBoltDiaFactor")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.general_set.WeldMapScale.fBoltDiaFactor);
		else if(_stricmp(key_word,"HoleAugmentInNC")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.general_set.fHoleAugmentInNC);
		else if(_stricmp(key_word,"PartMapStyle")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.iPartMapStyle);
		//
		else if(_stricmp(key_word,"WeldLineSymbolHigh")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.general_set.fWeldLineSymbolHigh);
		else if(_stricmp(key_word,"WeldLineSymbolSpace")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.general_set.fWeldLineSymbolSpace);
		else if(_stricmp(key_word,"DimPrecision")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.general_set.iDimPrecision);
		else if(_stricmp(key_word,"PartBOMStyle")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.general_set.iPartBOMStyle);
		else if(_stricmp(key_word,"SumMatBOMStyle")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.general_set.iSumMatBOMStyle);
		else if(_stricmp(key_word,"bHasFrame")==0)	//����װ����
		{
			sscanf(line_txt,"%s%d",key_word,&nKey);
			if(nKey==0)
				sys.general_set.bHasFrame=FALSE;
			else
				sys.general_set.bHasFrame=TRUE;
		}
		else if(_stricmp(key_word,"LineMapScale")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.general_set.LineMapScale);
		else if(_stricmp(key_word,"ForceLineScale")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.general_set.ForceLineScale);
		else if(_stricmp(key_word,"StruMapScale")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.general_set.StruMapScale.fRealToDraw);
		else if(_stricmp(key_word,"PartMapScale")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.general_set.PartMapScale.fRealToDraw);
		else if(_stricmp(key_word,"WeldMapScale")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.general_set.WeldMapScale.fRealToDraw);
		else if(_stricmp(key_word,"nColumnWidth")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.general_set.nColumnWidth);
		else if(_stricmp(key_word,"RowHigh")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.general_set.nRowHigh);
		else if(_stricmp(key_word,"PartRowsN")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.general_set.nPartRowsN);
		else if(_stricmp(key_word,"CreatePartCard")==0)
		{
			int bCreate;
			sscanf(line_txt,"%s%d",key_word,&bCreate);
			if(bCreate==0)
				sys.general_set.bProcessCardTbl=FALSE;
			else
				sys.general_set.bProcessCardTbl=TRUE;
		}
		else if(_stricmp(key_word,"CardLayoutBaseRow")==0)
		{
			int iRow;
			sscanf(line_txt,"%s%d",key_word,&iRow);
			if(iRow==0)
				sys.general_set.bCardLayoutBaseRow=FALSE;
			else
				sys.general_set.bCardLayoutBaseRow=TRUE;
		}
		else if(_stricmp(key_word,"JgCard")==0)
		{
			skey=strtok(NULL,"=,;");
			sprintf(sys.general_set.m_sJgCard,"%s",skey);
		}
		else if(_stricmp(key_word,"SlotCard")==0)
		{
			skey=strtok(NULL,"=,;");
			sprintf(sys.general_set.m_sSlotCard,"%s",skey);
		}
		else if(_stricmp(key_word,"TubeCard")==0)
		{
			skey=strtok(NULL,"=,;");
			sprintf(sys.general_set.m_sTubeCard,"%s",skey);
		}
		else if(_stricmp(key_word,"PlankCard")==0)
		{
			skey=strtok(NULL,"=,;");
			sprintf(sys.general_set.m_sPlankCard,"%s",skey);
		}
		else if(_stricmp(key_word,"WeldPartGroupCard")==0)
		{
			skey=strtok(NULL,"=,;");
			sprintf(sys.general_set.m_sWeldPartGroupCard,"%s",skey);
		}
		else if(_stricmp(key_word,"ProcessPartStatCard")==0)
		{
			skey=strtok(NULL,"=,;");
			sprintf(sys.general_set.m_sProcessPartStatCard,"%s",skey);
		}
		else if(_stricmp(key_word,"SetupAidedPlate")==0)
		{
			skey=strtok(NULL,"=,;");
			sprintf(sys.general_set.m_sSetupAidedPlate,"%s",skey);
		}
		else if(_stricmp(key_word,"TMAMapArxPath")==0)
		{
			skey=strtok(NULL,"=,;");
			sprintf(sys.general_set.m_sTMAMapArxPath,"%s",skey);
		}
		else if(_stricmp(key_word,"LMAMapArxPath")==0)
		{
			skey=strtok(NULL,"=,;");
			sprintf(sys.general_set.m_sLMAMapArxPath,"%s",skey);
		}
		else if(_stricmp(key_word,"LDSMapArxPath")==0)
		{
			skey=strtok(NULL,"=,;");
			sprintf(sys.general_set.m_sLDSMapArxPath,"%s",skey);
		}
		else if(_stricmp(key_word,"TSAMapArxPath")==0)
		{
			skey=strtok(NULL,"=,;");
			sprintf(sys.general_set.m_sTSAMapArxPath,"%s",skey);
		}
	//�ֶα�־��
		else if(_stricmp(key_word,"DrawMainPoleJointLine")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.bDrawMainPoleJointLine);
		else if(_stricmp(key_word,"JointLineOffset")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.nJointLineOffset);
		else if(_stricmp(key_word,"JointLineLen")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.nJointLineLen);
		//���ֻ��ƽ�
		else if(_stricmp(key_word,"RCrossarmCr")==0)
			sscanf(line_txt,"%s%X",key_word,&sys.general_set.rCrossarmCr);
		else if(_stricmp(key_word,"LCrossarmCr")==0)
			sscanf(line_txt,"%s%X",key_word,&sys.general_set.lCrossarmCr);
	//�ṹͼ����
		else if(_stricmp(key_word,"stru_map.bExportByEnglish")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.stru_map.bExportByEnglish);
		else if(_stricmp(key_word,"MinWingWide")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.stru_map.fMinWingWide);
		else if(_stricmp(key_word,"MinWingThick")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.stru_map.fMinPartThick);
		else if(_stricmp(key_word,"DimSpecialLsSpace")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.stru_map.bDimSpecialLsSpace);
		else if(_stricmp(key_word,"DimSpecialJgZhunJu")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.stru_map.bDimSpecialJgZhun);
		else if(_stricmp(key_word,"DimStruJgLen")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.stru_map.bDimStruJgLen);
		else if(_stricmp(key_word,"DimPartGuiGe")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.stru_map.bDimPartGuiGe);
		else if(_stricmp(key_word,"DrawCutAngleAndWingInfo")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.stru_map.bDrawCutAngleAndWingInfo);
		else if(_stricmp(key_word,"OnlyDrawHorizontalPlate")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.stru_map.bOnlyDrawHorizontalPlate);
		else if(_stricmp(key_word,"UsePolyLineDrawEdge")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.stru_map.bUsePolyLineDrawEdge);
		else if(_stricmp(key_word,"PolyLineWidth")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.stru_map.fPolyLineWidth);
		else if(_stricmp(key_word,"PoleAxisLineStartOutSpreadLen")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.stru_map.fPoleAxisLineStartOutSpreadLen);
		else if(_stricmp(key_word,"PoleAxisLineEndOutSpreadLen")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.stru_map.fPoleAxisLineEndOutSpreadLen);
		else if(_stricmp(key_word,"ReadDrawingScript")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.stru_map.bReadAdditionalBuffer);
		//�麸ͼ��ͼ����
		else if(_stricmp(key_word,"MinFontSize")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.part_group_map.fMinFontSize);
		else if(_stricmp(key_word,"MinDrawThick")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.part_group_map.fMinDrawThick);
		else if(_stricmp(key_word,"KmJgGIncrement")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.nKmJgGIncrement);
		else if(_stricmp(key_word,"ModulatePartGroupLen")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.bModulatePartGroupLen);
		else if(_stricmp(key_word,"PartGroupDistThreshold")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.part_group_map.fPartGroupDistThreshold);
		else if(_stricmp(key_word,"PartGroupDistZoomCoef")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.part_group_map.fPartGroupDistZoomCoef);
		else if(_stricmp(key_word,"bDrawWeldLineSymbol")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.bDrawWeldLineSymbol);
		else if(_stricmp(key_word,"iPlateLocateDrawStyle")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.iPlateLocateDrawStyle);
		else if(_stricmp(key_word,"iLocateSymboleHeight")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.iLocateSymboleHeight);
		else if(_stricmp(key_word,"bDimEndOuterPlaneSpace")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.bDimEndOuterPlaneSpace);
		else if(_stricmp(key_word,"AutoDimPartNo")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.bAutoDimPartNo);
		else if(_stricmp(key_word,"AutoMatchView")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.bAutoMatchView);
		else if(_stricmp(key_word,"DimAbsoluteDist")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.bDimAbsoluteDist);
		else if(_stricmp(key_word,"DimBeforeWeldLocatSpace")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.bDimBeforeWeldLocatSpace);
		else if(_stricmp(key_word,"AngularDimension")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.bAngularDimension);
		else if(_stricmp(key_word,"DimAfterWeldCheckLine")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.bDimAfterWeldCheckLine);
		else if(_stricmp(key_word,"DimPlatePoleRayAndAngle")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.bDimPlatePoleRayAndAngle);
		else if(_stricmp(key_word,"SmartFilterRedundantPart")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.bSmartFilterRedundantPart);
		else if(_stricmp(key_word,"CreatePartGroupBOM")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.bCreatePartGroupBOM);
		else if(_stricmp(key_word,"WeldingFontNeedZoom")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.welding.bZoomAdaptiveWithScale);
		else if(_stricmp(key_word,"WeldingFontScale")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.welding.wiScaleModel2Draw);
			//�麸����ϸ��
		else if(_stricmp(key_word,"DispNotes")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.part_table.bDispNotes);
		else if(_stricmp(key_word,"DispPartGroupNo")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.part_table.bDispPartGroupNo);
		else if(_stricmp(key_word,"DispPartGroupNum")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.part_table.bDispPartGroupNum);
		else if(_stricmp(key_word,"DispPartGuiGe")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.part_table.bDispPartGuiGe);
		else if(_stricmp(key_word,"DispPartLen")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.part_table.bDispPartLen);
		else if(_stricmp(key_word,"DispPartNo")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.part_table.bDispPartNo);
		else if(_stricmp(key_word,"DispPartNum")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.part_table.bDispPartNum);
		else if(_stricmp(key_word,"DispPartWeight")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.part_table.bDispPartWeight);
		else if(_stricmp(key_word,"bDispInternal")==0)	//���켶������
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.part_table.bDispInternal);
		else if(_stricmp(key_word,"bDispAppearance")==0)//���켶�����
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.part_table.bDispAppearance);
		else if(_stricmp(key_word,"bZoomInProcessCard")==0)//���տ��ڽ��б������ţ����
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.part_table.bZoomInProcessCard);
	//����ͼ
		if(_stricmp(key_word,"ExportDeformedProfile")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.iExportDeformedProfile);
		else if(_stricmp(key_word,"DimDeformedProfileText")==0)
		{	//�ְ忼�ǻ�������ʱ�Ƿ��ע��������������� wht 10-11-01
			int bDimDeformedProfileText;
			sscanf(line_txt,"%s%d",key_word,&bDimDeformedProfileText);
			if(bDimDeformedProfileText==0)
				sys.part_map.bDimDeformedProfileText=FALSE;
			else
				sys.part_map.bDimDeformedProfileText=TRUE;
		}
		else if(_stricmp(key_word,"DimHuoquType")==0)
		{
			int bDimHuoquType;
			sscanf(line_txt,"%s%d",key_word,&bDimHuoquType);
			if(bDimHuoquType==0)
				sys.part_map.bDimHuoquType=FALSE;
			else
				sys.part_map.bDimHuoquType=TRUE;
		}
		else if(_stricmp(key_word,"DimPlaneAngle")==0)
		{
			int bDimPlaneAngle;
			sscanf(line_txt,"%s%d",key_word,&bDimPlaneAngle);
			if(bDimPlaneAngle==0)
				sys.part_map.bDimPlaneAngle=FALSE;
			else
				sys.part_map.bDimPlaneAngle=TRUE;
		}
		else if(_stricmp(key_word,"DimAngleType")==0) //0.��� 1.�۽� 2.�Զ�
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.iDimAngleType);
		else if(_stricmp(key_word,"UseZhengFanQun")==0)
		{
			int bUseZhengFanQun;
			sscanf(line_txt,"%s%d",key_word,&bUseZhengFanQun);
			if(bUseZhengFanQun==0)
				sys.part_map.bUseZhengFanQun=FALSE;
			else
				sys.part_map.bUseZhengFanQun=TRUE;
		}
		//�Ǹֹ���ͼ����
		else if(_stricmp(key_word,"JgZoomSchema")==0) //�Ǹ����ŷ���
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.iJgZoomSchema);
		else if(_stricmp(key_word,"JgLenDrawMode")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.ciJgLenDrawMode);
		else if(_stricmp(key_word,"MaxExtendAngleLength")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.bMaxExtendAngleLength);
		else if(_stricmp(key_word,"ModulateLongJg")==0)
		{
			sscanf(line_txt,"%s%d",key_word,&nKey);
			if(nKey==0)
				sys.part_map.angle.bModulateLongJg=FALSE;
			else
				sys.part_map.angle.bModulateLongJg=TRUE;
		}
		else if(_stricmp(key_word,"LsDistThreshold")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.part_map.angle.fLsDistThreshold);
		else if(_stricmp(key_word,"LsDistZoomCoef")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.part_map.angle.fLsDistZoomCoef);
		else if(_stricmp(key_word,"OneCardMultiPart")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.bOneCardMultiPart);
		else if(_stricmp(key_word,"JgGDimStyle")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.iJgGDimStyle);
		else if(_stricmp(key_word,"MaxBoltNumStartDimG")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.nMaxBoltNumStartDimG);
		else if(_stricmp(key_word,"LsSpaceDimStyle")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.iLsSpaceDimStyle);
		else if(_stricmp(key_word,"MaxBoltNumAlongX")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.nMaxBoltNumAlongX);
		else if(_stricmp(key_word,"DimCutAngle")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.bDimCutAngle);
		else if(_stricmp(key_word,"DimCutAngleMap")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.bDimCutAngleMap);
		else if(_stricmp(key_word,"DimPushFlatMap")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.bDimPushFlatMap);
		else if(_stricmp(key_word,"JgUseSimpleLsMap")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.bJgUseSimpleLsMap);
		else if(_stricmp(key_word,"DimLsAbsoluteDist")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.bDimLsAbsoluteDist);
		else if(_stricmp(key_word,"bMergeLsAbsoluteDist")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.bMergeLsAbsoluteDist);
		else if(_stricmp(key_word,"bDimRibPlatePartNo")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.bDimRibPlatePartNo);
		else if(_stricmp(key_word,"bDimRibPlateMaterial")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.bDimRibPlateMaterial);
		else if(_stricmp(key_word,"bDimRibPlateSetUpPos")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.bDimRibPlateSetUpPos);
		else if(_stricmp(key_word,"CutAngleDimType")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.iCutAngleDimType);
		else if(_stricmp(key_word,"DimKaiHe")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.bDimKaiHe);
		else if(_stricmp(key_word,"DimKaiheAngleMap")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.bDimKaiheAngleMap);
		else if(_stricmp(key_word,"KaiHeJiaoThreshold")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.part_map.angle.fKaiHeJiaoThreshold);
		else if(_stricmp(key_word,"KaiHeJiaoMinDeita")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.part_map.angle.fKaiHeJiaoMinDeita);
		else if(_stricmp(key_word,"MatCharPosType")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.general_set.iMatCharPosType);
		else if(_stricmp(key_word,"DimKaiHeSumLen")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.bDimKaiheSumLen);	
		else if(_stricmp(key_word,"DimKaiHeAngle")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.bDimKaiheAngle);
		else if(_stricmp(key_word,"DimKaiHeSegLen")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.bDimKaiheSegLen);
		else if(_stricmp(key_word,"DimKaiHeScopeMap")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.angle.bDimKaiheScopeMap);
		//�ְ幹��ͼ
		else if(_stricmp(key_word,"DimPlateEdgeLen")==0)
		{
			sscanf(line_txt,"%s%d",key_word,&nKey);
			if(nKey==0)
				sys.part_map.plate.bDimPlateEdgeLen=FALSE;
			else
				sys.part_map.plate.bDimPlateEdgeLen=TRUE;
		}
		else if (_stricmp(key_word,"DrawInnerEdge")==0)
		{
			sscanf(line_txt,"%s%d",key_word,&nKey);
			if(nKey==0)
				sys.part_map.plate.bDrawInnerEdge=FALSE;
			else
				sys.part_map.plate.bDrawInnerEdge=TRUE;
		}
		else if (_stricmp(key_word,"InnerEdgeOffset")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.part_map.plate.fInnerEdgeOffset);
		else if(_stricmp(key_word,"DimPlankRayLs")==0)
		{
			sscanf(line_txt,"%s%d",key_word,&nKey);
			if(nKey==0)
				sys.part_map.plate.bDimPlateRayLs=FALSE;
			else
				sys.part_map.plate.bDimPlateRayLs=TRUE;
		}
		else if(_stricmp(key_word,"DimPlankBoltAngle")==0)//��ע����˨�Ƕ�
		{
			sscanf(line_txt,"%s%d",key_word,&nKey);
			if(nKey==0)
				sys.part_map.plate.bDimPlateBoltAngle=FALSE;
			else
				sys.part_map.plate.bDimPlateBoltAngle=TRUE;
		}
		else if(_stricmp(key_word,"DimBoltEdgeSpace")==0)
		{
			sscanf(line_txt,"%s%d",key_word,&nKey);
			if(nKey==0)
				sys.part_map.plate.bDimBoltEdgeSpace=FALSE;
			else
				sys.part_map.plate.bDimBoltEdgeSpace=TRUE;
		}
		else if(_stricmp(key_word,"DimDiffRayLsSpace")==0)
		{
			sscanf(line_txt,"%s%d",key_word,&nKey);
			if(nKey==0)
				sys.part_map.plate.bDimDiffRayLsSpace=FALSE;
			else
				sys.part_map.plate.bDimDiffRayLsSpace=TRUE;
		}
		else if(_stricmp(key_word,"DimLsToHuoquLine")==0)
		{
			sscanf(line_txt,"%s%d",key_word,&nKey);
			if(nKey==0)
				sys.part_map.plate.bDimLsToHuoquLine=FALSE;
			else
				sys.part_map.plate.bDimLsToHuoquLine=TRUE;
		}
		else if(_stricmp(key_word,"DimLsToWeldEdge")==0)
		{
			sscanf(line_txt,"%s%d",key_word,&nKey);
			if(nKey==0)
				sys.part_map.plate.bDimLsToWeldEdge=FALSE;
			else
				sys.part_map.plate.bDimLsToWeldEdge=TRUE;
		}
		else if(_stricmp(key_word,"DimArcEdgeR")==0)
		{
			sscanf(line_txt,"%s%d",key_word,&nKey);
			if(nKey==0)
				sys.part_map.plate.bDimArcEdgeR=FALSE;
			else
				sys.part_map.plate.bDimArcEdgeR=TRUE;
		}
		else if(_stricmp(key_word,"DimArcAngle")==0)
		{
			sscanf(line_txt,"%s%d",key_word,&nKey);
			if(nKey==0)
				sys.part_map.plate.bDimArcAngle=FALSE;
			else
				sys.part_map.plate.bDimArcAngle=TRUE;
		}
		else if(_stricmp(key_word,"AdjustPlateUcs")==0)
		{
			int bAdjustPlateUcs;
			sscanf(line_txt,"%s%d",key_word,&bAdjustPlateUcs);
			if(bAdjustPlateUcs==0)
				sys.part_map.plate.bAdjustPlateUcs=FALSE;
			else
				sys.part_map.plate.bAdjustPlateUcs=TRUE;
		}
		else if(_stricmp(key_word,"DimPlankCoord")==0)
		{
			int bDimCoord;
			sscanf(line_txt,"%s%d",key_word,&bDimCoord);
			if(bDimCoord==0)
				sys.part_map.plate.bDimCoord=FALSE;
			else
				sys.part_map.plate.bDimCoord=TRUE;
		}
		else if(_stricmp(key_word,"PlateUseSimpleLsMap")==0)
		{
			sscanf(line_txt,"%s%d",key_word,&nKey);
			if(nKey==0)
				sys.part_map.plate.bPlateUseSimpleLsMap=FALSE;
			else
				sys.part_map.plate.bPlateUseSimpleLsMap=TRUE;
		}
		else if(_stricmp(key_word,"ZoomDimTextSizeByWidth")==0)
		{
			sscanf(line_txt,"%s%d",key_word,&nKey);
			if(nKey==0)
				sys.part_map.plate.bZoomDimTextSizeByWidth=FALSE;
			else
				sys.part_map.plate.bZoomDimTextSizeByWidth=TRUE;
		}
		else if(_stricmp(key_word,"TextSizeZoomCoef")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.part_map.plate.fTextSizeZoomCoef);
		else if(_stricmp(key_word,"DrawSlopeCutLine")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.plate.bDrawSlopeCutLine);
		else if(_stricmp(key_word,"DrawDatumLine")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.plate.bDrawDatumLine);
		else if(_stricmp(key_word,"CutArcsType")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.plate.iCutArcsType);
		else if(_stricmp(key_word,"ZhiWanLineType")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.plate.iZhiWanLineType);
		else if(_stricmp(key_word,"MartHuoquPos")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.plate.bMarkHuoquPos);
		else if(_stricmp(key_word,"DrawHuoQuPrjMap")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.plate.bDrawHuoQuPrjMap);
		else if(_stricmp(key_word,"DrawHuoQuMarkLine")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.plate.bDrawHuoQuMarkLine);
		else if(_stricmp(key_word,"DisplaypPartInfoTbl")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.plate.bDisplaypPartInfoTbl);
		else if(_stricmp(key_word,"PartInfoTblColumnWidth")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.plate.nPartInfoTblColumnWidth);
		else if(_stricmp(key_word,"ShowPlateCfgInfoDlg")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.plate.bShowPlateCfgInfoDlg);
		//�ֹܹ���ͼ
		else if(_stricmp(key_word,"SlotWidthProcessError")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.part_map.tube.fSlotWidthProcessError);
		else if(_stricmp(key_word,"SlotLenProcessError")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.part_map.tube.fSlotLenProcessError);
		else if(_stricmp(key_word,"ReelTubeThresholdD")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.part_map.tube.fReelTubeThresholdD);

	//ͼֽ��ע
		else if(_stricmp(key_word,"FontName")==0)
		{	//��������
			skey=strtok(NULL,"=,;");
			CString sName=skey; //ȥ������ո�,�س�,�Ʊ��,�س�
			sName.Remove(' ');
			sName.Remove('\r');
			sName.Remove('\n');
			sName.Remove('\t');
			sprintf(sys.dim_map.sFontName,"%s",sName);
		}
		else if(_stricmp(key_word,"ArrowType")==0)
		{	//��������
			skey=strtok(NULL,"=,;");
			CString sArrowType=skey; //ȥ������ո�
			sArrowType.Remove(' ');
			sprintf(sys.dim_map.sArrowType,"%s",sArrowType);	//�ߴ��ע��ͷ����
		}
		else if(_stricmp(key_word,"DimGap")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fDimGap);	//�������ע�߼�϶ֵ wht 10-08-02
		else if(_stricmp(key_word,"TblHeaderTextSize")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fHeaderTextSize);
		else if(_stricmp(key_word,"TblContentTextSize")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fNumericTextSize);
		else if(_stricmp(key_word,"TblCommonTextSize")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fCommonTextSize);
		else if(_stricmp(key_word,"DimArrowSize")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fDimArrowSize);
		else if(_stricmp(key_word,"DimTextSize")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fDimTextSize);
		else if(_stricmp(key_word,"DimAngleTextSize")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fDimAngleTextSize);
		//����ͼ��
		else if(_stricmp(key_word,"CutawayViewSymbolLen")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fCutawayViewSymbolLen);
		else if(_stricmp(key_word,"CutawayViewSymbolTextSize")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fCutawayViewSymbolTextSize);
		else if(_stricmp(key_word,"CutawayViewPolyLineWidth")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fCutawayViewPolyLineWidth);
		//�κű�ע
		else if(_stricmp(key_word,"SegITextSize")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fSegITextSize);
		else if(_stricmp(key_word,"SegICirD")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fSegICirD);
		//�������
		else if(_stricmp(key_word,"PartNoFrameStyle")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.dim_map.PartNo.iPartNoFrameStyle);
		else if(_stricmp(key_word,"StdInsertPlatePartNoFrameStyle")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.dim_map.PartNo.iStdInsertPlatePartNoFrameStyle);
		else if(_stricmp(key_word,"FlPartNoFrameStyle")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.dim_map.PartNo.iFlPartNoFrameStyle);
		else if(_stricmp(key_word,"PartNoTextSize")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.PartNo.fPartNoTextSize);
		else if(_stricmp(key_word,"PartNoCirD")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.PartNo.fPartNoCirD);
		else if(_stricmp(key_word,"MainPoleNoOuterCirD")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.PartNo.fMainPoleNoOuterCirD);
		else if(_stricmp(key_word,"MainPoleNoInterCirD")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.PartNo.fMainPoleNoInnerCirD);
		else if(_stricmp(key_word,"MirMsg")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.dim_map.PartNo.iMirMsg);
		else if(_stricmp(key_word,"PartNoMargin")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.PartNo.fPartNoMargin);
		else if(_stricmp(key_word,"DrawRefLine")==0)
		{
			sscanf(line_txt,"%s%d",key_word,&nKey);
			if(nKey==0)
				sys.dim_map.PartNo.bDrawRefLine=FALSE;
			else
				sys.dim_map.PartNo.bDrawRefLine=TRUE;
		}
		else if(_stricmp(key_word,"SimTriTextSize")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fSimPolyTextSize);
		else if(_stricmp(key_word,"PlateThickTextSize")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fPlateThickTextSize);
		else if(_stricmp(key_word,"JgOddmentTextSize")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fJgOddmentTextSize);
		else if(_stricmp(key_word,"PartGuigeTextSize")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fPartGuigeTextSize);
		else if(_stricmp(key_word,"SimTriScale")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fSimTriScale);
		else if(_stricmp(key_word,"LsGuigeTextSize")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fLsGuigeTextSize);
		else if(_stricmp(key_word,"OtherTextSize")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fOtherTextSize);
		else if(_stricmp(key_word,"VisibleOutline")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.dim_map.crIndex.crOutLine);
		else if(_stricmp(key_word,"DashLine")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.dim_map.crIndex.crDashLine);
		else if(_stricmp(key_word,"AxisLine")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.dim_map.crIndex.crAxisLine);
		else if(_stricmp(key_word,"SingleLine")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.dim_map.crIndex.crSingleLine);
		else if(_stricmp(key_word,"LsHoleLine")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.dim_map.crIndex.crLs);
		else if(_stricmp(key_word,"HuoQuLine")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.dim_map.crIndex.crHuoQuLine);
		else if(_stricmp(key_word,"DimLine")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.dim_map.crIndex.crDimSimTri);
		else if(_stricmp(key_word,"DragLine")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.dim_map.crIndex.crDragLine);
		else 
			bContinue=TRUE;
		if(bContinue)	//�ڴ˴���� if-else wht 11-07-04
		{
			//�ṹͼ�пɼ�������ͼ��
			if(_stricmp(key_word,"VisibleProfileLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::VisibleProfileLayer.layerName);
			//�ṹͼ�в��ɼ�������ͼ��
			else if(_stricmp(key_word,"UnvisibleProfileLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::UnvisibleProfileLayer.layerName);
			//�����ĵ���ͼ����ͼ
			else if(_stricmp(key_word,"BriefLineLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::BriefLineLayer.layerName);
			//���ĵ���ͼ
			else if(_stricmp(key_word,"AuxLineLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::AuxLineLayer.layerName);
			//��˨ͼ��
			else if(_stricmp(key_word,"BoltSymbolLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::BoltSymbolLayer.layerName);
			//�����α�ע
			else if(_stricmp(key_word,"SimPolyLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::SimPolyLayer.layerName);
			//�����ߡ��Գ��ߡ���λ����
			else if(_stricmp(key_word,"AxisLineLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::AxisLineLayer.layerName);
			//�Ǹֻ������ְ����
			else if(_stricmp(key_word,"BendLineLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::BendLineLayer.layerName);
			//�Ͽ�����
			else if(_stricmp(key_word,"BreakLineLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::BreakLineLayer.layerName);
			//��˨��
			else if(_stricmp(key_word,"BoltLineLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::BoltLineLayer.layerName);
			//�ߴ��ע
			else if(_stricmp(key_word,"DimSizeLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::DimSizeLayer.layerName);
			//���ֱ�ע
			else if(_stricmp(key_word,"DimTextLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::DimTextLayer.layerName);
			//���������������ͼԪ����ͼ��
			else if(_stricmp(key_word,"CommonLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::CommonLayer.layerName);
			//����������
			else if(_stricmp(key_word,"AssistHuoQuLineLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
			//�ӽǸ�������
			else if(_stricmp(key_word,"SonJgAxisLineLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::SonJgAxisLineLayer.layerName);
			//ͼ���ϣ��
			else if(_stricmp(key_word,"LayerKey")==0)
			{
				skey=strtok(NULL,"=,;");
				if(strlen(skey)>0)
				{
					TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.Add(skey);
					fgets(line_txt,MAX_PATH,fp);
					skey=strtok(line_txt,";");
					pLayer->layerName.Printf("%s",skey);
					skey=strtok(NULL,";");
					pLayer->lineTypeName.Printf("%s",skey);
					skey=strtok(NULL,";");
					pLayer->lineWeight=(AcDb::LineWeight)atoi(skey);
					skey=strtok(NULL,";");
					pLayer->colorIndex=atoi(skey);
				}
			}
			else if(_stricmp(key_word,"SpecialSetKey")==0)
			{
				long key = 0;
				sscanf(line_txt,"%s%d",key_word,&key);
				TEXT_PROP *pTextProp = sys.fonts.hashListProp.Add(key);
				fgets(line_txt,MAX_PATH,fp);
				char *skey=strtok(line_txt,";");
				pTextProp->fHigh = atof(skey);
				skey=strtok(NULL,";");
				pTextProp->fWtoH = atof(skey);
				skey=strtok(NULL,";");
				sprintf(pTextProp->sFont,"%s",skey);	
			}
			//ͼ���б�
			else if(_stricmp(key_word,"MapFrameSize")==0)
			{
				skey=strtok(NULL,"=,;");
				if(strlen(skey)>0)
				{
					MAP_FRAME_SIZE mapFrame;
					fgets(line_txt,MAX_PATH,fp);
					fgets(line_txt,MAX_PATH,fp);
					skey=strtok(line_txt,";");
					mapFrame.style=atoi(skey);
					skey=strtok(NULL,";");
					strcpy(mapFrame.sMapFrameName,skey);
					skey=strtok(NULL,";");
					mapFrame.nLength=atoi(skey);
					skey=strtok(NULL,";");
					mapFrame.nWidth=atoi(skey);
					skey=strtok(NULL,";");
					mapFrame.nLeftMargin=atoi(skey);
					skey=strtok(NULL,";");
					mapFrame.nRightMargin=atoi(skey);
					skey=strtok(NULL,";");
					mapFrame.nTopMargin=atoi(skey);
					skey=strtok(NULL,";");
					mapFrame.nBottomMargin=atoi(skey);
					skey=strtok(NULL,";");
					mapFrame.nBottomRightFixLen=atoi(skey);
					skey=strtok(NULL,";");
					mapFrame.nBottomRightFixWidth=atoi(skey);
					sys.m_hashMapFrameList.SetValue(mapFrame.sMapFrameName,mapFrame);
				}
			}
		}
	}
	fclose(fp);
	sys.GetPublicSysParaFromReg("Settings","KaiHeJiaoThreshold");
	sys.GetPublicSysParaFromReg("Settings","KaiHeJiaoMinDeita");
	sys.GetPublicSysParaFromReg("Settings","MatCharPosType");
	sys.GetPublicSysParaFromReg("Settings","m_iPlankWeightStatType");
	sys.GetPublicSysParaFromReg("Settings","m_iCirclePlankAreaType");
	sys.GetPublicSysParaFromReg("Settings","MatSeparator");
	sys.GetPublicSysParaFromReg("Settings","MinHuoquAngle");
	sys.GetPublicSysParaFromReg("Settings","InnerOffThreshold");
	sys.InitDrawingEnv();
	sys.GetDrawingSysParaFromReg("Settings","CompanyName");
	sys.GetDrawingSysParaFromReg("Settings","Critic");
}
void SysSetExportDefault(BOOL bPrompt)
{
	FILE *fp;
	CString sFileName;
	char file_name[MAX_PATH];
	if(bPrompt)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		CFileDialog dlg(FALSE,"set","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"Draw Setting File(*.set)|*.set||");
#else
		CFileDialog dlg(FALSE,"set","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"��ͼ�����ļ�(*.set)|*.set||");
#endif
		if(dlg.DoModal()!=IDOK)
			return;
		sFileName = dlg.GetPathName();
		sprintf(file_name,"%s",sFileName);
	}
	else
	{
		GetAppPath(file_name);
		strcat(file_name,"map.set");
	}

	if((fp=fopen(file_name,"wt"))==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Can't open config file specifed!");
#else
		AfxMessageBox("�򲻿�ָ���������ļ�!");
#endif
		return;
	}
	//��������
#ifdef AFX_TARG_ENU_ENGLISH
	fprintf(fp,"General Settings\n");
	fprintf(fp,"StruBoltSymbolStyle=%d ;Bolt's symbol style in structure map\n",sys.general_set.StruMapScale.iBoltSymbolStyle);
	fprintf(fp,"StruBoltSymbolFactor=%f ;Scale factor of bolt's scale symbol in structure map\n",sys.general_set.StruMapScale.fBoltSymbolFactor);
	fprintf(fp,"StruBoltDiaFactor=%f ;Scale factor of bolt's scale diameter symbol in structure map\n",sys.general_set.StruMapScale.fBoltDiaFactor);
	fprintf(fp,"PartBoltSymbolStyle=%d ;Bolt's symbol style in part map\n",sys.general_set.PartMapScale.iBoltSymbolStyle);
	fprintf(fp,"PartBoltSymbolFactor=%f ;Scale factor of bolt's scale symbol in part map\n",sys.general_set.PartMapScale.fBoltSymbolFactor);
	fprintf(fp,"PartBoltDiaFactor=%f ;Scale factor of bolt's scale diameter symbol in part map\n",sys.general_set.PartMapScale.fBoltDiaFactor);
	fprintf(fp,"WeldBoltSymbolStyle=%d ;Bolt's symbol style in weld group map\n",sys.general_set.WeldMapScale.iBoltSymbolStyle);
	fprintf(fp,"WeldBoltSymbolFactor=%f ;Scale factor of bolt's scale symbol in weld group map\n",sys.general_set.WeldMapScale.fBoltSymbolFactor);
	fprintf(fp,"WeldBoltDiaFactor=%f ;Scale factor of bolt's scale diameter symbol in weld group map\n",sys.general_set.WeldMapScale.fBoltDiaFactor);
	fprintf(fp,"HoleAugmentInNC=%f ;Hole diamaeter augment\n",sys.general_set.fHoleAugmentInNC);
	fprintf(fp,"WeldLineSymbolHigh=%f ;Weld line height\n",sys.general_set.fWeldLineSymbolHigh);
	fprintf(fp,"WeldLineSymbolSpace=%f ;Weld line space\n",sys.general_set.fWeldLineSymbolSpace);
	fprintf(fp,"DimPrecision=%d ;Precision\n",sys.general_set.iDimPrecision);
	fprintf(fp,"PartBOMStyle=%d ;Part BOM style\n",sys.general_set.iPartBOMStyle);
	fprintf(fp,"SumMatBOMStyle=%d ;Sum material Bom style\n",sys.general_set.iSumMatBOMStyle);
	fprintf(fp,"DrawMainPoleJointLine=%d ;Draw segment mark line\n",sys.bDrawMainPoleJointLine);
	fprintf(fp,"JointLineOffset=%d ;Offset of segment mark line offset\n",sys.nJointLineOffset);
	fprintf(fp,"JointLineLen=%d ;Length of segment mark line\n",sys.nJointLineLen);
	//�������տ��������в���
	if(sys.general_set.bProcessCardTbl)
		fprintf(fp,"CreatePartCard=1;//Create part card frame or not\n");
	else
		fprintf(fp,"CreatePartCard=0;//Create part card frame or not\n");
	fprintf(fp,"bOneCardMultiPart=%d ;Allow multi part in a angle card\n",sys.part_map.angle.bOneCardMultiPart);
	fprintf(fp,"bHasFrame=%d ;Draw frame\n",sys.general_set.bHasFrame);
	fprintf(fp,"LineMapScale=%f ;Line map scale\n",sys.general_set.LineMapScale);
	fprintf(fp,"ForceLineScale=%f ;Force map scale\n",sys.general_set.ForceLineScale);
	fprintf(fp,"StruMapScale=%f ;Structure map scale\n",sys.general_set.StruMapScale.fRealToDraw);
	fprintf(fp,"PartMapScale=%f ;Part map scale\n",sys.general_set.PartMapScale.fRealToDraw);
	fprintf(fp,"WeldMapScale=%f ;Weld group map scale\n",sys.general_set.WeldMapScale.fRealToDraw);
	fprintf(fp,"nColumnWidth=%d ;Column width of part map\n",sys.general_set.nColumnWidth);
	fprintf(fp,"RowHigh=%d ;Row height of part map\n",sys.general_set.nRowHigh);
	if(sys.general_set.bCardLayoutBaseRow)
		fprintf(fp,"CardLayoutBaseRow=1;//Part map layout base on row\n");
	else
		fprintf(fp,"CardLayoutBaseRow=0;//Part map layout base on column\n");
	fprintf(fp,"PartRowsN=%d ;Max row count each column in part map\n",sys.general_set.nPartRowsN);
	fprintf(fp,"JgCard=%s ;Angle card contents\n",sys.general_set.m_sJgCard);
	fprintf(fp,"SlotCard=%s ;Slot card contents\n",sys.general_set.m_sSlotCard);
	fprintf(fp,"TubeCard=%s ;Tube card contents\n",sys.general_set.m_sTubeCard);
	fprintf(fp,"PlankCard=%s ;Plank card contents\n",sys.general_set.m_sPlankCard);
	fprintf(fp,"WeldPartGroupCard=%s ;Weld part group card contents\n",sys.general_set.m_sWeldPartGroupCard);
	fprintf(fp,"ProcessPartStatCard=%s ;Process part stat. card contents\n",sys.general_set.m_sProcessPartStatCard);
	//��װ�����ְ���ļ�Ŀ¼
	fprintf(fp,"SetupAidedPlate=%s ;Setup aided plate library file contents\n",sys.general_set.m_sSetupAidedPlate);
	//��ͼģ��·������
	fprintf(fp,"TMAMapArxPath=%s ;TMA map arx path\n",sys.general_set.m_sTMAMapArxPath);
	fprintf(fp,"LMAMapArxPath=%s ;LMA map arx path\n",sys.general_set.m_sLMAMapArxPath);
	fprintf(fp,"LDSMapArxPath=%s ;LDS map arx path\n",sys.general_set.m_sLDSMapArxPath);
	fprintf(fp,"TSAMapArxPath=%s ;TSA map arx path\n",sys.general_set.m_sTSAMapArxPath);
	//���ֻ��ƽ���������
	fprintf(fp,"LCrossarmCr=%X ;Left Arm Color\n",sys.general_set.lCrossarmCr);
	fprintf(fp,"RCrossarmCr=%X ;Rignt Arm Color\n",sys.general_set.rCrossarmCr);
	//�ṹͼ����
	fprintf(fp,"Structure map settings\n");
	fprintf(fp,"bExportByEnglish=%d ;Generate construction drawing by english\n",sys.stru_map.bExportByEnglish);
	fprintf(fp,"MinWingWide=%.0f ;Min wing wide\n",sys.stru_map.fMinWingWide);
	fprintf(fp,"MinWingThick=%.0f ;Min wing thick\n",sys.stru_map.fMinPartThick);
	fprintf(fp,"DimSpecialLsSpace=%d ;Special bolt space\n",sys.stru_map.bDimSpecialLsSpace);
	fprintf(fp,"DimSpecialJgZhunJu=%d ;Dimension angle special G\n",sys.stru_map.bDimSpecialJgZhun);
	fprintf(fp,"DimStruJgLen=%d ;Dimension angle length\n",sys.stru_map.bDimStruJgLen);
	fprintf(fp,"DimPartGuiGe=%d ;Dimension part spec\n",sys.stru_map.bDimPartGuiGe);
	fprintf(fp,"DrawCutAngleAndWingInfo=%d ;Draw angle cutting leg information\n",sys.stru_map.bDrawCutAngleAndWingInfo);
	fprintf(fp,"OnlyDrawHorizontalPlate=%d ;Only draw plates which horizontal display in current view\n",sys.stru_map.bOnlyDrawHorizontalPlate);
	fprintf(fp,"UsePolyLineDrawEdge=%d ;Use poly line to draw edge of angle or plate\n",sys.stru_map.bUsePolyLineDrawEdge);
	fprintf(fp,"PolyLineWidth=%.1f ;Poly line width\n",sys.stru_map.fPolyLineWidth);
	fprintf(fp,"PoleAxisLineStartOutSpreadLen=%.0f ;Start of rod's bolt line out spread length\n",sys.stru_map.fPoleAxisLineStartOutSpreadLen);
	fprintf(fp,"PoleAxisLineEndOutSpreadLen=%.0f ;End of rod's bolt line out spread length\n",sys.stru_map.fPoleAxisLineEndOutSpreadLen);
	fprintf(fp,"ReadDrawingScript=%d ;Read additional buffer\n",sys.stru_map.bReadAdditionalBuffer);
	//�ṹͼ�ָ�����
	fprintf(fp,"PlateThickTextSize=%.2f ;Text size of plate's thickness\n",sys.dim_map.fPlateThickTextSize);
	fprintf(fp,"SimTriTextSize=%.2f ;Text size of similar figure\n",sys.dim_map.fSimPolyTextSize);
	fprintf(fp,"OtherTextSize=%.2f ;Other normal text size\n",sys.dim_map.fOtherTextSize);
	fprintf(fp,"JgOddmentTextSize=%.2f ;Text size of angle oddment\n",sys.dim_map.fJgOddmentTextSize);
	fprintf(fp,"PartGuigeTextSize=%.2f ;Text size of part spec\n",sys.dim_map.fPartGuigeTextSize);
	fprintf(fp,"SimTriScale=%.3f ;Zoom scale of similar figure\n",sys.dim_map.fSimTriScale);
	fprintf(fp,"LsGuigeTextSize=%.2f ;Text size of bolt sepc information\n",sys.dim_map.fLsGuigeTextSize);
	//�麸ͼ��ͼ����
	fprintf(fp,"Weld group map drawing settings\n");
	fprintf(fp,"MinFontSize=%.2f ;Min font height in weld group map\n",sys.part_group_map.fMinFontSize);
	fprintf(fp,"MinDrawThick=%.2f ;The thickness of the minimum value\n",sys.part_group_map.fMinDrawThick);
	fprintf(fp,"KmJgGIncrement=%d ;Explorator Angle G Adjust Threshold\n",sys.part_group_map.nKmJgGIncrement);
	fprintf(fp,"ModulatePartGroupLen=%d	;Modulate weld group map displayed length\n",sys.part_group_map.bModulatePartGroupLen);
	fprintf(fp,"PartGroupDistZoomCoef=%.2f	;Zoom coefficient of weld part group\n",sys.part_group_map.fPartGroupDistZoomCoef);
	fprintf(fp,"fPartGroupDistThreshold=%.2f;Zoom threshold of weld part group\n",sys.part_group_map.fPartGroupDistThreshold);
	//�����б�Ҫ����hxr2015-4-25
	fprintf(fp,"bDrawWeldLineSymbol=%d;���ƺ����߱��\n",sys.part_group_map.bDrawWeldLineSymbol);
	fprintf(fp,"iPlateLocateDrawStyle=%d;�麸ͼ�л��Ƹְ�Ի��\n",sys.part_group_map.iPlateLocateDrawStyle);
	fprintf(fp,"iLocateSymboleHeight=%d;�麸ͼ�иְ�Ի��߶�\n",sys.part_group_map.iLocateSymboleHeight);
	fprintf(fp,"bDimEndOuterPlaneSpace=%d;�˼�����浽��������\n",sys.part_group_map.bDimEndOuterPlaneSpace);
	fprintf(fp,"AutoDimPartNo=%d	;�Զ���ע�������\n",sys.part_group_map.bAutoDimPartNo);
	fprintf(fp,"AutoMatchView=%d	;�Զ�����麸����ͼ\n",sys.part_group_map.bAutoMatchView);
	fprintf(fp,"DimAbsoluteDist=%d	;��ע��λ����Գߴ�\n",sys.part_group_map.bDimAbsoluteDist);
	fprintf(fp,"DimBeforeWeldLocatSpace=%d	;��ע����ǰ��λ�ߴ�\n",sys.part_group_map.bDimBeforeWeldLocatSpace);
	fprintf(fp,"DimAfterWeldCheckLine=%d	;��ע���Ӻ�����\n",sys.part_group_map.bDimAfterWeldCheckLine);
	fprintf(fp,"AngularDimension=%d	;��ע�Ƕ���\n",sys.part_group_map.bAngularDimension);
	fprintf(fp,"DimPlatePoleRayAndAngle=%d	;��ע�ְ����ӵĸ˼������Լ��Ƕ�\n",sys.part_group_map.bDimPlatePoleRayAndAngle);
	fprintf(fp,"SmartFilterRedundantPart=%d	;��ͼ���Զ�ɸѡ���뵱ǰ��ͼ�޹صĶ��๹����������ʾ\n",sys.part_group_map.bSmartFilterRedundantPart);
	fprintf(fp,"CreatePartGroupBOM=%d	;�����麸����ϸ��\n",sys.part_group_map.bCreatePartGroupBOM);
	fprintf(fp,"WeldingFontNeedZoom=%d	;�����涯����\n",sys.welding.bZoomAdaptiveWithScale);
	fprintf(fp,"WeldingFontScale=%d	;�����׼���ű���\n",sys.welding.wiScaleModel2Draw);
	//�麸����ϸ��
	fprintf(fp,"DispPartGroupNo=%d	;//�麸�����\n",sys.part_group_map.part_table.bDispPartGroupNo);
	fprintf(fp,"DispPartGroupNum=%d	;//�麸������\n",sys.part_group_map.part_table.bDispPartGroupNum);
	fprintf(fp,"DispPartNo=%d	;//�������\n",sys.part_group_map.part_table.bDispPartNo);
	fprintf(fp,"DispPartNum=%d	;//��������\n",sys.part_group_map.part_table.bDispPartNum);
	fprintf(fp,"DispPartGuiGe=%d	;//�������\n",sys.part_group_map.part_table.bDispPartGuiGe);
	fprintf(fp,"DispPartLen=%d	;//��������\n",sys.part_group_map.part_table.bDispPartLen);
	fprintf(fp,"DispPartWeight=%d	;//��������\n",sys.part_group_map.part_table.bDispPartWeight);
	fprintf(fp,"DispNotes=%d	;//��ע\n",sys.part_group_map.part_table.bDispNotes);
	fprintf(fp,"bDispInternal=%d	;//���켶������\n",sys.part_group_map.part_table.bDispInternal);
	fprintf(fp,"bDispAppearance=%d	;//���켶�����\n",sys.part_group_map.part_table.bDispAppearance);
	fprintf(fp,"bZoomInProcessCard=%d	;//���տ��ڽ��б�������\n",sys.part_group_map.part_table.bZoomInProcessCard);
	//����ͼ
	fprintf(fp,"ExportDeformedProfile=%d	;//����ͼ����������κ��ЧӦ\n",sys.part_map.iExportDeformedProfile);
	fprintf(fp,"DimDeformedProfileText=%d ;���ǻ�������ʱ�Ƿ��ע���������������\n",sys.part_map.bDimDeformedProfileText);
	fprintf(fp,"DimHuoquType=%d ;��������\n",sys.part_map.bDimHuoquType);
	fprintf(fp,"DimPlaneAngle=%d ;��ע������֮��ļн�\n",sys.part_map.bDimPlaneAngle);
	fprintf(fp,"DimAngleType=%d ;�Ƕȱ�ע����\n",sys.part_map.iDimAngleType);
	fprintf(fp,"PartMapStyle=%d ;����ͼ�����ʽ����\n",sys.part_map.iPartMapStyle);
	fprintf(fp,"UseZhengFanQun=%d ;��������������ʹ��������\n",sys.part_map.bUseZhengFanQun);
	//�Ǹֹ���ͼ����
	fprintf(fp,"�Ǹֹ���ͼ����\n");
	fprintf(fp,"ModulateLongJg=%d ;�����Ǹֳ���\n",sys.part_map.angle.bModulateLongJg);
	fprintf(fp,"JgZoomSchema=%d ;//0.1:1���� 1.ʹ�ù���ͼ���� 2.����ͬ������ 3.����ֱ�����\n",sys.part_map.angle.iJgZoomSchema);
	fprintf(fp,"JgLenDrawMode=%d ;//0.�������� 1.�м伷ѹ\n",sys.part_map.angle.ciJgLenDrawMode);
	fprintf(fp,"MaxExtendAngleLength=%d ;//����޶�����Ǹֻ��Ƴ���\n",sys.part_map.angle.bMaxExtendAngleLength);
	fprintf(fp,"LsDistThreshold=%.2f ;�Ǹֳ����Զ�������˨�����ֵ\n",sys.part_map.angle.fLsDistThreshold);
	fprintf(fp,"LsDistZoomCoef=%.2f ;//��˨�������ϵ��\n",sys.part_map.angle.fLsDistZoomCoef);
	fprintf(fp,"OneCardMultiPart=%d ;//�Ǹ�����һ��������\n",sys.part_map.angle.bOneCardMultiPart);
	fprintf(fp,"JgGDimStyle=%d ;//0.ʼ�˱�ע 1.�м��ע 2.����ע\n",sys.part_map.angle.iJgGDimStyle);
	fprintf(fp,"MaxBoltNumStartDimG=%d ;//������ʼ�˱�ע׼��֧�ֵ������˨��\n",sys.part_map.angle.nMaxBoltNumStartDimG);
	fprintf(fp,"LsSpaceDimStyle=%d ;//0.X�᷽��	 1.Y�᷽��  2.�Զ��ж� 3.����ע\n",sys.part_map.angle.iLsSpaceDimStyle);
	fprintf(fp,"MaxBoltNumAlongX=%d ;//��X�᷽���ע֧�ֵ������˨����\n",sys.part_map.angle.nMaxBoltNumAlongX);
	fprintf(fp,"DimCutAngle=%d ;//��ע�Ǹ��н�\n",sys.part_map.angle.bDimCutAngle);
	fprintf(fp,"DimCutAngleMap=%d ;//��ע�Ǹ��н�ʾ��ͼ\n",sys.part_map.angle.bDimCutAngleMap);
	fprintf(fp,"DimPushFlatMap=%d ;//��עѹ��ʾ��ͼ\n",sys.part_map.angle.bDimPushFlatMap);
	fprintf(fp,"JgUseSimpleLsMap=%d ;//�Ǹ�ʹ�ü���˨ͼ��\n",sys.part_map.angle.bJgUseSimpleLsMap);
	fprintf(fp,"DimLsAbsoluteDist=%d ;//��ע��˨���Գߴ�\n",sys.part_map.angle.bDimLsAbsoluteDist);
	fprintf(fp,"bMergeLsAbsoluteDist=%d ;//�ϲ����ڵȾ���˨���Գߴ�\n",sys.part_map.angle.bMergeLsAbsoluteDist);
	fprintf(fp,"DimRibPlatePartNo=%d ;//��ע�Ǹ��߰���\n",sys.part_map.angle.bDimRibPlatePartNo);
	fprintf(fp,"DimRibPlateMaterial=%d ;//��ע�Ǹ��߰����\n",sys.part_map.angle.bDimRibPlateMaterial);
	fprintf(fp,"bDimRibPlateSetUpPos=%d ;//��ע�Ǹ��߰尲װλ��\n",sys.part_map.angle.bDimRibPlateSetUpPos);
	fprintf(fp,"CutAngleDimType=%d ;//�Ǹ��нǱ�ע����\n",sys.part_map.angle.iCutAngleDimType);
	fprintf(fp,"DimKaiHe=%d ;//��ע�Ǹֿ��Ͻ�\n",sys.part_map.angle.bDimKaiHe);
	fprintf(fp,"DimKaiheAngleMap=%d ;//��ע�Ǹֿ��Ͻ�ʾ��ͼ\n",sys.part_map.angle.bDimKaiheAngleMap);
	fprintf(fp,"KaiHeJiaoThreshold=%.2f ;���ϽǱ�ע��ֵ\n",sys.part_map.angle.fKaiHeJiaoThreshold);
	fprintf(fp,"MatCharPosType=%d ;//�����ַ���עλ��\n",sys.general_set.iMatCharPosType);
	fprintf(fp,"DimKaiHeSumLen=%d ;//��ע�Ǹֿ��������ܳ���\n",sys.part_map.angle.bDimKaiheSumLen);
	fprintf(fp,"DimKaiHeAngle=%d ;//��ע�Ǹֿ��ϽǶ���\n",sys.part_map.angle.bDimKaiheAngle);
	fprintf(fp,"DimKaiHeSegLen=%d ;//��ע�Ǹֿ�������ֶγ�\n",sys.part_map.angle.bDimKaiheSegLen);
	fprintf(fp,"DimKaiHeScopeMap=%d ;//��ע�Ǹֿ�������ֶγ�\n",sys.part_map.angle.bDimKaiheScopeMap);
	//�ְ幹��ͼ
	fprintf(fp,"�ְ幹��ͼ����\n");
	fprintf(fp,"DimPlateEdgeLen=%d ;�����Ӱ�������\n",sys.part_map.plate.bDimPlateEdgeLen);
	fprintf(fp,"DrawInnerEdge=%d ;���ڲ�������\n",sys.part_map.plate.bDrawInnerEdge);
	fprintf(fp,"InnerEdgeOffset=%f ;������ƫ�ƾ�\n",sys.part_map.plate.fInnerEdgeOffset);
	fprintf(fp,"DimPlankRayLs=%d ;��ע���Ӱ���˨��϶\n",sys.part_map.plate.bDimPlateRayLs);
	fprintf(fp,"DimPlankBoltAngle=%d ;��ע���Ӱ���˨�Ƕ�\n",sys.part_map.plate.bDimPlateBoltAngle);
	fprintf(fp,"DimBoltEdgeSpace=%d ;��ע���Ӱ���˨�Ƕ�\n",sys.part_map.plate.bDimBoltEdgeSpace);
	fprintf(fp,"DimDiffRayLsSpace=%d ;��ע���Ӱ���˨�Ƕ�\n",sys.part_map.plate.bDimDiffRayLsSpace);
	fprintf(fp,"DimLsToHuoquLine=%d ;��ע���Ӱ���˨�Ƕ�\n",sys.part_map.plate.bDimLsToHuoquLine);
	fprintf(fp,"DimLsToWeldEdge=%d ;��ע���Ӱ���˨�Ƕ�\n",sys.part_map.plate.bDimLsToWeldEdge);
	fprintf(fp,"DimArcEdgeR=%d ;��ע���Ӱ�Բ���߰뾶\n",sys.part_map.plate.bDimArcEdgeR);
	fprintf(fp,"DimArcAngle=%d ;��ע���Ӱ�Բ���߽Ƕ�\n",sys.part_map.plate.bDimArcAngle);
	fprintf(fp,"PlateUseSimpleLsMap=%d ;�Ǹ�ʹ�ü���˨ͼ��\n",sys.part_map.plate.bPlateUseSimpleLsMap);
	fprintf(fp,"AdjustPlateUcs=%d ;�����ְ�����ϵ��֤�����ˮƽ����\n",sys.part_map.plate.bAdjustPlateUcs);
	if(sys.part_map.plate.bDimCoord)
		fprintf(fp,"DimPlankCoord=1;//���ɰ���˨����������\n");
	else
		fprintf(fp,"DimPlankCoord=0;//���ɰ���˨����������\n");
	fprintf(fp,"ZoomDimTextSizeByWidth=%d ;���ݸְ峤���еĽ�Сֵ���Ÿְ���ر�ע����߶�\n",sys.part_map.plate.bZoomDimTextSizeByWidth);
	fprintf(fp,"TextSizeZoomCoef=%f ;����߶�����֮�������ϵ��\n",sys.part_map.plate.fTextSizeZoomCoef);
	fprintf(fp,"DrawSlopeCutLine=%d ;���촦���¿���\n",sys.part_map.plate.bDrawSlopeCutLine);
	fprintf(fp,"DrawDatumLine=%d ;�ְ�Ļ�׼��\n",sys.part_map.plate.bDrawDatumLine);
	fprintf(fp,"CutArcsType=%d :�������ǵ��л���������\n",sys.part_map.plate.iCutArcsType);
	fprintf(fp,"ZhiWanLineType=%d :������λ�õĴ�������\n",sys.part_map.plate.iZhiWanLineType);
	fprintf(fp,"MartHuoquPos=%d :��ǻ���λ��\n",sys.part_map.plate.bMarkHuoquPos);
	fprintf(fp,"DrawHuoQuPrjMap=%d :���ƻ���ʾ��ͼ\n",sys.part_map.plate.bDrawHuoQuPrjMap);
	fprintf(fp,"DrawHuoQuMarkLine=%d :���ƻ���������\n",sys.part_map.plate.bDrawHuoQuMarkLine);
	fprintf(fp,"DisplaypPartInfoTbl=%d :��ʾ����������\n",sys.part_map.plate.bDisplaypPartInfoTbl);
	fprintf(fp,"PartInfoTblColumnWidth=%d :��������п�����\n",sys.part_map.plate.nPartInfoTblColumnWidth);	
	fprintf(fp,"PlateConfigFilePath=%d :Plate config file\n",sys.part_map.plate.sPlateConfigFilePath);
	fprintf(fp,"ShowPlateCfgInfoDlg=%d ;��ʾ�ְ���ϸ�Ի�\n",sys.part_map.plate.bShowPlateCfgInfoDlg);
	//�ֹܹ���ͼ����
	fprintf(fp,"�ֹܹ���ͼ����\n");
	fprintf(fp,"SlotLenProcessError=%f ;�ֹܿ��۳������ֵ\n",sys.part_map.tube.fSlotLenProcessError);
	fprintf(fp,"SlotWidthProcessError=%f ;�ֹܿ��ۿ�����ֵ\n",sys.part_map.tube.fSlotWidthProcessError);
	fprintf(fp,"ReelTubeThresholdD=%f ;�ְ���ֱ����ֵ\n",sys.part_map.tube.fReelTubeThresholdD);

	//��ע����
	fprintf(fp,"��ע����\n");
	fprintf(fp,"FontName=%s	;��������\n",sys.dim_map.sFontName);
	fprintf(fp,"ArrowType=%s ;�ߴ��ע��ͷ����\n",sys.dim_map.sArrowType);
	fprintf(fp,"DimGap=%.2f ;�������ע�߼�϶ֵ\n",sys.dim_map.fDimGap);	
	fprintf(fp,"TblHeaderTextSize=%.2f ;��ͷ�ı���С\n",sys.dim_map.fHeaderTextSize);
	fprintf(fp,"TblContentTextSize=%.2f ;��������ı���С\n",sys.dim_map.fNumericTextSize);
	fprintf(fp,"TblCommonTextSize=%.2f ;����и������ı���С\n",sys.dim_map.fCommonTextSize);
	fprintf(fp,"DimArrowSize=%.2f ;�ߴ��ע��ͷ��С\n",sys.dim_map.fDimArrowSize);
	fprintf(fp,"DimTextSize=%.2f ;���ȳߴ��ע�ı���С\n",sys.dim_map.fDimTextSize);
	fprintf(fp,"DimAngleTextSize=%.2f ;�Ƕȳߴ��ע�ı���С\n",sys.dim_map.fDimAngleTextSize);
	//����ͼ��
	fprintf(fp,"CutawayViewSymbolLen=%.2f ;����ͼ������\n",sys.dim_map.fCutawayViewSymbolLen);
	fprintf(fp,"CutawayViewSymbolTextSize=%.2f ;����ͼ������߶�\n",sys.dim_map.fCutawayViewSymbolTextSize);
	fprintf(fp,"CutawayViewPolyLineWidth=%.2f ;����ͼ������߿��\n",sys.dim_map.fCutawayViewPolyLineWidth);
	//�κű�ע
	fprintf(fp,"SegITextSize=%.2f ;�κű�ע�ı���С\n",sys.dim_map.fSegITextSize);
	fprintf(fp,"SegICirD=%.2f ;�κű�עȦֱ��\n",sys.dim_map.fSegICirD);
	//
	fprintf(fp,"PartNoFrameStyle=%d ;��ſ�����\n",sys.dim_map.PartNo.iPartNoFrameStyle);
	fprintf(fp,"FlPartNoFrameStyle=%d ;������ſ�����\n",sys.dim_map.PartNo.iFlPartNoFrameStyle);
	fprintf(fp,"StdInsertPlatePartNoFrameStyle=%d ;��׼����ſ�����\n",sys.dim_map.PartNo.iStdInsertPlatePartNoFrameStyle);
	fprintf(fp,"PartNoTextSize=%.2f ;��������ı���С\n",sys.dim_map.PartNo.fPartNoTextSize);
	fprintf(fp,"PartNoCirD=%.2f ;�������Ȧֱ��\n",sys.dim_map.PartNo.fPartNoCirD);
	fprintf(fp,"MainPoleNoOuterCirD=%.2f ;���ı����Բֱ��\n",sys.dim_map.PartNo.fMainPoleNoOuterCirD);
	fprintf(fp,"MainPoleNoInterCirD=%.2f ;���ı����Բֱ��\n",sys.dim_map.PartNo.fMainPoleNoInnerCirD);
	fprintf(fp,"MirMsg=%d ;���ı�ű�ע�ԳƷ�ʽ\n",sys.dim_map.PartNo.iMirMsg);
	fprintf(fp,"PartNoMargin=%.2f ;����������ſ�֮��ļ�϶\n",sys.dim_map.PartNo.fPartNoMargin);
	fprintf(fp,"DrawRefLine=%d ;�Ƿ����������\n",sys.dim_map.PartNo.bDrawRefLine);
	//��ͼ����ɫ
	fprintf(fp,"������ɫ����\n");
	fprintf(fp,"VisibleOutline=%d ;�ɼ���������ɫ\n",sys.dim_map.crIndex.crOutLine);
	fprintf(fp,"DashLine=%d ;������ɫ\n",sys.dim_map.crIndex.crDashLine);
	fprintf(fp,"AxisLine=%d ;���ߡ��Գ�����ɫ\n",sys.dim_map.crIndex.crAxisLine);
	fprintf(fp,"SingleLine=%d ;����ͼ����ͼ��ɫ\n",sys.dim_map.crIndex.crSingleLine);
	fprintf(fp,"LsHoleLine=%d ;��˨����ɫ\n",sys.dim_map.crIndex.crLs);
	fprintf(fp,"HuoQuLine=%d ;��������ɫ\n",sys.dim_map.crIndex.crHuoQuLine);
	fprintf(fp,"DimLine=%d ;��ע����ɫ\n",sys.dim_map.crIndex.crDimSimTri);
	fprintf(fp,"DragLine=%d ;��ע����ɫ\n",sys.dim_map.crIndex.crDragLine);

	//ͼ��
	fprintf(fp,"ͼ������\n");
	//�ṹͼ�пɼ�������ͼ��
	if(LayerTable::VisibleProfileLayer.layerName.Length()>0)
		fprintf(fp,"VisibleProfileLayer.layerName=%s ;�ɼ�������ͼ����\n",(char*)LayerTable::VisibleProfileLayer.layerName);
	//�ṹͼ�в��ɼ�������ͼ��
	if(LayerTable::UnvisibleProfileLayer.layerName.Length()>0)
		fprintf(fp,"UnvisibleProfileLayer.layerName=%s ;������ͼ����\n",(char*)LayerTable::UnvisibleProfileLayer.layerName);
	//�����ĵ���ͼ����ͼ
	if(LayerTable::BriefLineLayer.layerName.Length()>0)
		fprintf(fp,"BriefLineLayer.layerName=%s ;�����ĵ���ͼ����\n",(char*)LayerTable::BriefLineLayer.layerName);
	//���ĵ���ͼ
	if(LayerTable::AuxLineLayer.layerName.Length()>0)
		fprintf(fp,"AuxLineLayer.layerName=%s ;���ĵ���ͼ����\n",(char*)LayerTable::AuxLineLayer.layerName);
	//��˨ͼ��
	if(LayerTable::BoltSymbolLayer.layerName.Length()>0)
		fprintf(fp,"BoltSymbolLayer.layerName=%s ;��˨ͼ����\n",(char*)LayerTable::BoltSymbolLayer.layerName);
	//�����α�ע
	if(LayerTable::SimPolyLayer.layerName.Length()>0)
		fprintf(fp,"SimPolyLayer.layerName=%s ;������ͼ����\n",(char*)LayerTable::SimPolyLayer.layerName);
	//�����ߡ��Գ��ߡ���λ����
	if(LayerTable::AxisLineLayer.layerName.Length()>0)
		fprintf(fp,"AxisLineLayer.layerName=%s ;������ͼ����\n",(char*)LayerTable::AxisLineLayer.layerName);
	//�Ǹֻ������ְ����
	if(LayerTable::BendLineLayer.layerName.Length()>0)
		fprintf(fp,"BendLineLayer.layerName=%s ;������ͼ����\n",(char*)LayerTable::BendLineLayer.layerName);
	//�Ͽ�����
	if(LayerTable::BreakLineLayer.layerName.Length()>0)
		fprintf(fp,"BreakLineLayer.layerName=%s ;���ߡ��Ͽ���ͼ����\n",(char*)LayerTable::BreakLineLayer.layerName);
	//��˨��
	if(LayerTable::BoltLineLayer.layerName.Length()>0)
		fprintf(fp,"BoltLineLayer.layerName=%s ;��˨��ͼ����\n",(char*)LayerTable::BoltLineLayer.layerName);
	//�ߴ��ע
	if(LayerTable::DimSizeLayer.layerName.Length()>0)
		fprintf(fp,"DimSizeLayer.layerName=%s ;�ߴ��עͼ����\n",(char*)LayerTable::DimSizeLayer.layerName);
	//���ֱ�ע
	if(LayerTable::DimTextLayer.layerName.Length()>0)
		fprintf(fp,"DimTextLayer.layerName=%s ;���ֱ�עͼ����\n",(char*)LayerTable::DimTextLayer.layerName);
	//���������������ͼԪ����ͼ��
	if(LayerTable::CommonLayer.layerName.Length()>0)
		fprintf(fp,"CommonLayer.layerName=%s ;����ͼ����\n",(char*)LayerTable::CommonLayer.layerName);
	//����������
	if(LayerTable::AssistHuoQuLineLayer.layerName.Length()>0)
		fprintf(fp,"AssistHuoQuLineLayer.layerName=%s ;����������ͼ����\n",(char*)LayerTable::AssistHuoQuLineLayer.layerName);
	//�ӽǸ�������
	if(LayerTable::SonJgAxisLineLayer.layerName.Length()>0)
		fprintf(fp,"SonJgAxisLineLayer.layerName=%s ;�ӽǸ�������ͼ����\n",(char*)LayerTable::SonJgAxisLineLayer.layerName);
	//�ְ����������ʶ��
	if(LayerTable::DamagedSymbolLine.layerName.Length()>0)
		fprintf(fp,"DamagedSymbolLine.layerName=%s ;�ְ����������ʶ��\n",(char*)LayerTable::DamagedSymbolLine.layerName);
#else
	fprintf(fp,"��������\n");
	fprintf(fp,"StruBoltSymbolStyle=%d ;�ṹͼ��˨ͼ����ʽ\n",sys.general_set.StruMapScale.iBoltSymbolStyle);
	fprintf(fp,"StruBoltSymbolFactor=%f ;�ṹͼ��˨����ͼ���ı�������\n",sys.general_set.StruMapScale.fBoltSymbolFactor);
	fprintf(fp,"StruBoltDiaFactor=%f ;�ṹͼ��˨����ֱ��ͼ����������\n",sys.general_set.StruMapScale.fBoltDiaFactor);
	fprintf(fp,"PartBoltSymbolStyle=%d ;����ͼ��˨ͼ����ʽ\n",sys.general_set.PartMapScale.iBoltSymbolStyle);
	fprintf(fp,"PartBoltSymbolFactor=%f ;����ͼ��˨����ͼ���ı�������\n",sys.general_set.PartMapScale.fBoltSymbolFactor);
	fprintf(fp,"PartBoltDiaFactor=%f ;����ͼ��˨����ֱ��ͼ����������\n",sys.general_set.PartMapScale.fBoltDiaFactor);
	fprintf(fp,"WeldBoltSymbolStyle=%d ;�麸ͼ��˨ͼ����ʽ\n",sys.general_set.WeldMapScale.iBoltSymbolStyle);
	fprintf(fp,"WeldBoltSymbolFactor=%f ;�麸ͼ��˨����ͼ���ı�������\n",sys.general_set.WeldMapScale.fBoltSymbolFactor);
	fprintf(fp,"WeldBoltDiaFactor=%f ;�麸ͼ��˨����ֱ��ͼ����������\n",sys.general_set.WeldMapScale.fBoltDiaFactor);
	fprintf(fp,"HoleAugmentInNC=%f ;�׾�����ֵ\n",sys.general_set.fHoleAugmentInNC);
	fprintf(fp,"WeldLineSymbolHigh=%f ;�����߸߶�\n",sys.general_set.fWeldLineSymbolHigh);
	fprintf(fp,"WeldLineSymbolSpace=%f ;�����߼�϶\n",sys.general_set.fWeldLineSymbolSpace);
	fprintf(fp,"DimPrecision=%d ;��ȷ��\n",sys.general_set.iDimPrecision);
	fprintf(fp,"PartBOMStyle=%d ;������ϸ����ʽ\n",sys.general_set.iPartBOMStyle);
	fprintf(fp,"SumMatBOMStyle=%d ;���ϻ��ܱ���ʽ\n",sys.general_set.iSumMatBOMStyle);
	fprintf(fp,"DrawMainPoleJointLine=%d ;���Ʒֶα�־��\n",sys.bDrawMainPoleJointLine);
	fprintf(fp,"JointLineOffset=%d ;�ֶα�־��ƫ����\n",sys.nJointLineOffset);
	fprintf(fp,"JointLineLen=%d ;�ֶα�־�߳���\n",sys.nJointLineLen);
		//�������տ��������в���
	if(sys.general_set.bProcessCardTbl)
		fprintf(fp,"CreatePartCard=1;//�Ƿ����ɹ��տ�ͼ��\n");
	else
		fprintf(fp,"CreatePartCard=0;//�Ƿ����ɹ��տ�ͼ��\n");
	fprintf(fp,"bOneCardMultiPart=%d ;����Ǹֿ�һ��������\n",sys.part_map.angle.bOneCardMultiPart);
	fprintf(fp,"bHasFrame=%d ;����װ����\n",sys.general_set.bHasFrame);
	fprintf(fp,"LineMapScale=%f ;����ͼ��ͼ����\n",sys.general_set.LineMapScale);
	fprintf(fp,"ForceLineScale=%f ;˾��ͼ��ͼ����\n",sys.general_set.ForceLineScale);
	fprintf(fp,"StruMapScale=%f ;�ṹͼ��ͼ����\n",sys.general_set.StruMapScale.fRealToDraw);
	fprintf(fp,"PartMapScale=%f ;����ͼ��ͼ����\n",sys.general_set.PartMapScale.fRealToDraw);
	fprintf(fp,"WeldMapScale=%f ;�麸ͼ��ͼ����\n",sys.general_set.WeldMapScale.fRealToDraw);
	fprintf(fp,"nColumnWidth=%d ;����ͼ�п�\n",sys.general_set.nColumnWidth);
	fprintf(fp,"RowHigh=%d ;����ͼ�п�\n",sys.general_set.nRowHigh);
	if(sys.general_set.bCardLayoutBaseRow)
		fprintf(fp,"CardLayoutBaseRow=1;//����ͼ����Ϊ��׼��������\n");
	else
		fprintf(fp,"CardLayoutBaseRow=0;//����ͼ����Ϊ��׼��������\n");
	fprintf(fp,"PartRowsN=%d ;����ͼÿ���������\n",sys.general_set.nPartRowsN);
	fprintf(fp,"JgCard=%s ;�Ǹֹ��տ�Ŀ¼\n",sys.general_set.m_sJgCard);
	fprintf(fp,"SlotCard=%s ;�۸ֹ��տ�Ŀ¼\n",sys.general_set.m_sSlotCard);
	fprintf(fp,"TubeCard=%s ;�ֹܹ��տ�Ŀ¼\n",sys.general_set.m_sTubeCard);
	fprintf(fp,"PlankCard=%s ;�幤��Ŀ¼\n",sys.general_set.m_sPlankCard);
	fprintf(fp,"WeldPartGroupCard=%s ;���Ӽ�����Ŀ¼\n",sys.general_set.m_sWeldPartGroupCard);
	fprintf(fp,"ProcessPartStatCard=%s ;���ռ�ͳ�ƿ�Ŀ¼\n",sys.general_set.m_sProcessPartStatCard);
		//��װ�����ְ���ļ�Ŀ¼
	fprintf(fp,"SetupAidedPlate=%s ;��װ�����ְ���ļ�Ŀ¼\n",sys.general_set.m_sSetupAidedPlate);
		//��ͼģ��·������
	fprintf(fp,"TMAMapArxPath=%s ;TMA��ͼģ��·��\n",sys.general_set.m_sTMAMapArxPath);
	fprintf(fp,"LMAMapArxPath=%s ;LMA��ͼģ��·��\n",sys.general_set.m_sLMAMapArxPath);
	fprintf(fp,"LDSMapArxPath=%s ;LDS��ͼģ��·��\n",sys.general_set.m_sLDSMapArxPath);
	fprintf(fp,"TSAMapArxPath=%s ;TSA��ͼģ��·��\n",sys.general_set.m_sTSAMapArxPath);
	//���ֻ��ƽ���������
	fprintf(fp,"LCrossarmCr=%X ;��ᵣ��ɫ\n",sys.general_set.lCrossarmCr);
	fprintf(fp,"RCrossarmCr=%X ;�Һᵣ��ɫ\n",sys.general_set.rCrossarmCr);
	//�ṹͼ����
	fprintf(fp,"�ṹͼ����\n");
	fprintf(fp,"MinWingWide=%.0f ;��С֫��ֵ\n",sys.stru_map.fMinWingWide);
	fprintf(fp,"MinWingThick=%.0f ;��С֫��ֵ\n",sys.stru_map.fMinPartThick);
	fprintf(fp,"DimSpecialLsSpace=%d ;������˨���\n",sys.stru_map.bDimSpecialLsSpace);
	fprintf(fp,"DimSpecialJgZhunJu=%d ;��ע�Ǹ�����׼��\n",sys.stru_map.bDimSpecialJgZhun);
	fprintf(fp,"DimStruJgLen=%d ;��ע�Ǹֳ���\n",sys.stru_map.bDimStruJgLen);
	fprintf(fp,"DimPartGuiGe=%d ;��ע�������\n",sys.stru_map.bDimPartGuiGe);
	fprintf(fp,"DrawCutAngleAndWingInfo=%d ;���ƽǸ��н�֫��Ϣ\n",sys.stru_map.bDrawCutAngleAndWingInfo);
	fprintf(fp,"OnlyDrawHorizontalPlate=%d ;�����Ƶ�ǰ��ͼ��ˮƽ��ʾ�ĸְ�\n",sys.stru_map.bOnlyDrawHorizontalPlate);
	fprintf(fp,"UsePolyLineDrawEdge=%d ;ʹ�ö���߻��ƽǸֻ�ְ��������\n",sys.stru_map.bUsePolyLineDrawEdge);
	fprintf(fp,"PolyLineWidth=%.1f ;����߿��ֵ\n",sys.stru_map.fPolyLineWidth);
	fprintf(fp,"PoleAxisLineStartOutSpreadLen=%.0f ;�˼�����ʼ��������\n",sys.stru_map.fPoleAxisLineStartOutSpreadLen);
	fprintf(fp,"PoleAxisLineEndOutSpreadLen=%.0f ;�˼�����ʼ��������\n",sys.stru_map.fPoleAxisLineEndOutSpreadLen);
	fprintf(fp,"ReadDrawingScript=%d ;��ȡ���ӻ���\n",sys.stru_map.bReadAdditionalBuffer);
	//�ṹͼ�ָ�����
	fprintf(fp,"PlateThickTextSize=%.2f ;����ı���С\n",sys.dim_map.fPlateThickTextSize);
	fprintf(fp,"SimTriTextSize=%.2f ;�������ı���С\n",sys.dim_map.fSimPolyTextSize);
	fprintf(fp,"OtherTextSize=%.2f ;������ͨ�ı���С\n",sys.dim_map.fOtherTextSize);
	fprintf(fp,"JgOddmentTextSize=%.2f ;�Ǹ�����ͷ�ı���С\n",sys.dim_map.fJgOddmentTextSize);
	fprintf(fp,"PartGuigeTextSize=%.2f ;��������ı���С\n",sys.dim_map.fPartGuigeTextSize);
	fprintf(fp,"SimTriScale=%.3f ;���������ű���\n",sys.dim_map.fSimTriScale);
	fprintf(fp,"LsGuigeTextSize=%.2f ;��˨�����Ϣ�ı���С\n",sys.dim_map.fLsGuigeTextSize);
	//�麸ͼ��ͼ����
	fprintf(fp,"�麸ͼ��ͼ����\n");
	fprintf(fp,"MinFontSize=%.2f ;�麸ͼ���������С�߶�\n",sys.part_group_map.fMinFontSize);
	fprintf(fp,"MinDrawThick=%.2f ;�����С����ֵ\n",sys.part_group_map.fMinDrawThick);
	fprintf(fp,"KmJgGIncrement=%d ;��ģ�Ǹ��ľ�����ֵ\n",sys.part_group_map.nKmJgGIncrement);
	fprintf(fp,"ModulatePartGroupLen=%d	;�����麸ͼ��ʾ����\n",sys.part_group_map.bModulatePartGroupLen);
	fprintf(fp,"PartGroupDistZoomCoef=%.2f	;�麸������ϵ��\n",sys.part_group_map.fPartGroupDistZoomCoef);
	fprintf(fp,"fPartGroupDistThreshold=%.2f ;�麸��������ֵ\n",sys.part_group_map.fPartGroupDistThreshold);
	fprintf(fp,"bDrawWeldLineSymbol=%d;���ƺ����߱��\n",sys.part_group_map.bDrawWeldLineSymbol);
	fprintf(fp,"iPlateLocateDrawStyle=%d;�麸ͼ�л��Ƹְ�Ի��\n",sys.part_group_map.iPlateLocateDrawStyle);
	fprintf(fp,"iLocateSymboleHeight=%d;�麸ͼ�иְ�Ի��߶�\n",sys.part_group_map.iLocateSymboleHeight);
	fprintf(fp,"bDimEndOuterPlaneSpace=%d;�˼�����浽��������\n",sys.part_group_map.bDimEndOuterPlaneSpace);
	fprintf(fp,"AutoDimPartNo=%d	;�Զ���ע�������\n",sys.part_group_map.bAutoDimPartNo);
	fprintf(fp,"AutoMatchView=%d	;�Զ�����麸����ͼ\n",sys.part_group_map.bAutoMatchView);
	fprintf(fp,"DimAbsoluteDist=%d	;��ע��λ����Գߴ�\n",sys.part_group_map.bDimAbsoluteDist);
	fprintf(fp,"DimBeforeWeldLocatSpace=%d	;��ע����ǰ��λ�ߴ�\n",sys.part_group_map.bDimBeforeWeldLocatSpace);
	fprintf(fp,"DimAfterWeldCheckLine=%d	;��ע���Ӻ�����\n",sys.part_group_map.bDimAfterWeldCheckLine);
	fprintf(fp,"AngularDimension=%d	;��ע�Ƕ���\n",sys.part_group_map.bAngularDimension);
	fprintf(fp,"DimPlatePoleRayAndAngle=%d	;��ע�ְ����ӵĸ˼������Լ��Ƕ�\n",sys.part_group_map.bDimPlatePoleRayAndAngle);
	fprintf(fp,"SmartFilterRedundantPart=%d	;��ͼ���Զ�ɸѡ���뵱ǰ��ͼ�޹صĶ��๹����������ʾ\n",sys.part_group_map.bSmartFilterRedundantPart);
	fprintf(fp,"CreatePartGroupBOM=%d	;�����麸����ϸ��\n",sys.part_group_map.bCreatePartGroupBOM);
	fprintf(fp,"WeldingFontNeedZoom=%d	;�����涯����\n",sys.welding.bZoomAdaptiveWithScale);
	fprintf(fp,"WeldingFontScale=%d	;�����׼���ű���\n",sys.welding.wiScaleModel2Draw);
	//�麸����ϸ��
	fprintf(fp,"DispPartGroupNo=%d	;//�麸�����\n",sys.part_group_map.part_table.bDispPartGroupNo);
	fprintf(fp,"DispPartGroupNum=%d	;//�麸������\n",sys.part_group_map.part_table.bDispPartGroupNum);
	fprintf(fp,"DispPartNo=%d	;//�������\n",sys.part_group_map.part_table.bDispPartNo);
	fprintf(fp,"DispPartNum=%d	;//��������\n",sys.part_group_map.part_table.bDispPartNum);
	fprintf(fp,"DispPartGuiGe=%d	;//�������\n",sys.part_group_map.part_table.bDispPartGuiGe);
	fprintf(fp,"DispPartLen=%d	;//��������\n",sys.part_group_map.part_table.bDispPartLen);
	fprintf(fp,"DispPartWeight=%d	;//��������\n",sys.part_group_map.part_table.bDispPartWeight);
	fprintf(fp,"DispNotes=%d	;//��ע\n",sys.part_group_map.part_table.bDispNotes);
	fprintf(fp,"bDispInternal=%d	;//���켶������\n",sys.part_group_map.part_table.bDispInternal);
	fprintf(fp,"bDispAppearance=%d	;//���켶�����\n",sys.part_group_map.part_table.bDispAppearance);
	fprintf(fp,"bZoomInProcessCard=%d	;//���տ��ڽ��б�������\n",sys.part_group_map.part_table.bZoomInProcessCard);
	//����ͼ
	fprintf(fp,"ExportDeformedProfile=%d	;//����ͼ����������κ��ЧӦ\n",sys.part_map.iExportDeformedProfile);
	fprintf(fp,"DimDeformedProfileText=%d ;���ǻ�������ʱ�Ƿ��ע���������������\n",sys.part_map.bDimDeformedProfileText);
	fprintf(fp,"DimHuoquType=%d ;��������\n",sys.part_map.bDimHuoquType);
	fprintf(fp,"DimPlaneAngle=%d ;��ע������֮��ļн�\n",sys.part_map.bDimPlaneAngle);
	fprintf(fp,"DimAngleType=%d ;�Ƕȱ�ע����\n",sys.part_map.iDimAngleType);
	fprintf(fp,"PartMapStyle=%d ;����ͼ�����ʽ����\n",sys.part_map.iPartMapStyle);
	fprintf(fp,"UseZhengFanQun=%d ;��������������ʹ��������\n",sys.part_map.bUseZhengFanQun);
		//�Ǹֹ���ͼ����
	fprintf(fp,"�Ǹֹ���ͼ����\n");
	fprintf(fp,"ModulateLongJg=%d ;�����Ǹֳ���\n",sys.part_map.angle.bModulateLongJg);
	fprintf(fp,"JgZoomSchema=%d ;//0.1:1���� 1.ʹ�ù���ͼ���� 2.����ͬ������ 3.����ֱ�����\n",sys.part_map.angle.iJgZoomSchema);
	fprintf(fp,"JgLenDrawMode=%d ;//0.�������� 1.�м伷ѹ\n",sys.part_map.angle.ciJgLenDrawMode);
	fprintf(fp,"MaxExtendAngleLength=%d ;//����޶�����Ǹֻ��Ƴ���\n",sys.part_map.angle.bMaxExtendAngleLength);
	fprintf(fp,"LsDistThreshold=%.2f ;�Ǹֳ����Զ�������˨�����ֵ\n",sys.part_map.angle.fLsDistThreshold);
	fprintf(fp,"LsDistZoomCoef=%.2f ;//��˨�������ϵ��\n",sys.part_map.angle.fLsDistZoomCoef);
	fprintf(fp,"OneCardMultiPart=%d ;//�Ǹ�����һ��������\n",sys.part_map.angle.bOneCardMultiPart);
	fprintf(fp,"JgGDimStyle=%d ;//0.ʼ�˱�ע 1.�м��ע 2.����ע\n",sys.part_map.angle.iJgGDimStyle);
	fprintf(fp,"MaxBoltNumStartDimG=%d ;//������ʼ�˱�ע׼��֧�ֵ������˨��\n",sys.part_map.angle.nMaxBoltNumStartDimG);
	fprintf(fp,"LsSpaceDimStyle=%d ;//0.X�᷽��	 1.Y�᷽��  2.�Զ��ж� 3.����ע\n",sys.part_map.angle.iLsSpaceDimStyle);
	fprintf(fp,"MaxBoltNumAlongX=%d ;//��X�᷽���ע֧�ֵ������˨����\n",sys.part_map.angle.nMaxBoltNumAlongX);
	fprintf(fp,"DimCutAngle=%d ;//��ע�Ǹ��н�\n",sys.part_map.angle.bDimCutAngle);
	fprintf(fp,"DimCutAngleMap=%d ;//��ע�Ǹ��н�ʾ��ͼ\n",sys.part_map.angle.bDimCutAngleMap);
	fprintf(fp,"DimPushFlatMap=%d ;//��עѹ��ʾ��ͼ\n",sys.part_map.angle.bDimPushFlatMap);
	fprintf(fp,"JgUseSimpleLsMap=%d ;//�Ǹ�ʹ�ü���˨ͼ��\n",sys.part_map.angle.bJgUseSimpleLsMap);
	fprintf(fp,"DimLsAbsoluteDist=%d ;//��ע��˨���Գߴ�\n",sys.part_map.angle.bDimLsAbsoluteDist);
	fprintf(fp,"bMergeLsAbsoluteDist=%d ;//�ϲ����ڵȾ���˨���Գߴ�\n",sys.part_map.angle.bMergeLsAbsoluteDist);
	fprintf(fp,"DimRibPlatePartNo=%d ;//��ע�Ǹ��߰���\n",sys.part_map.angle.bDimRibPlatePartNo);
	fprintf(fp,"DimRibPlateMaterial=%d ;//��ע�Ǹ��߰����\n",sys.part_map.angle.bDimRibPlateMaterial);
	fprintf(fp,"bDimRibPlateSetUpPos=%d ;//��ע�Ǹ��߰尲װλ��\n",sys.part_map.angle.bDimRibPlateSetUpPos);
	fprintf(fp,"CutAngleDimType=%d ;//�Ǹ��нǱ�ע����\n",sys.part_map.angle.iCutAngleDimType);
	fprintf(fp,"DimKaiHe=%d ;//��ע�Ǹֿ��Ͻ�\n",sys.part_map.angle.bDimKaiHe);
	fprintf(fp,"DimKaiheAngleMap=%d ;//��ע�Ǹֿ��Ͻ�ʾ��ͼ\n",sys.part_map.angle.bDimKaiheAngleMap);
	fprintf(fp,"KaiHeJiaoThreshold=%.2f ;���ϽǱ�ע��ֵ\n",sys.part_map.angle.fKaiHeJiaoThreshold);
	fprintf(fp,"fKaiHeJiaoMinDeita=%g ;���Ͻ���С��ȫ��϶ֵ\n",sys.part_map.angle.fKaiHeJiaoMinDeita);
	fprintf(fp,"MatCharPosType=%d ;//�����ַ���עλ��\n",sys.general_set.iMatCharPosType);
	fprintf(fp,"DimKaiHeSumLen=%d ;//��ע�Ǹֿ��������ܳ���\n",sys.part_map.angle.bDimKaiheSumLen);
	fprintf(fp,"DimKaiHeAngle=%d ;//��ע�Ǹֿ��ϽǶ���\n",sys.part_map.angle.bDimKaiheAngle);
	fprintf(fp,"DimKaiHeSegLen=%d ;//��ע�Ǹֿ�������ֶγ�\n",sys.part_map.angle.bDimKaiheSegLen);
	fprintf(fp,"DimKaiHeScopeMap=%d ;//��ע�Ǹֿ�������ֶγ�\n",sys.part_map.angle.bDimKaiheScopeMap);
	//�ְ幹��ͼ
	fprintf(fp,"�ְ幹��ͼ����\n");
	fprintf(fp,"DimPlateEdgeLen=%d ;�����Ӱ�������\n",sys.part_map.plate.bDimPlateEdgeLen);
	fprintf(fp,"DrawInnerEdge=%d ;���ڲ�������\n",sys.part_map.plate.bDrawInnerEdge);
	fprintf(fp,"InnerEdgeOffset=%f ;������ƫ�ƾ�\n",sys.part_map.plate.fInnerEdgeOffset);
	fprintf(fp,"DimPlankRayLs=%d ;��ע���Ӱ���˨��϶\n",sys.part_map.plate.bDimPlateRayLs);
	fprintf(fp,"DimPlankBoltAngle=%d ;��ע���Ӱ���˨�Ƕ�\n",sys.part_map.plate.bDimPlateBoltAngle);
	fprintf(fp,"DimBoltEdgeSpace=%d ;��ע���Ӱ���˨�߾�\n",sys.part_map.plate.bDimBoltEdgeSpace);
	fprintf(fp,"DimDiffRayLsSpace=%d ;��ע����������˨���\n",sys.part_map.plate.bDimDiffRayLsSpace);
	fprintf(fp,"DimLsToHuoquLine=%d ;��ע����������˨���\n",sys.part_map.plate.bDimLsToHuoquLine);
	fprintf(fp,"DimLsToWeldEdge=%d ;��ע����������˨���\n",sys.part_map.plate.bDimLsToWeldEdge);
	fprintf(fp,"DimArcEdgeR=%d ;��ע���Ӱ�Բ���߰뾶\n",sys.part_map.plate.bDimArcEdgeR);
	fprintf(fp,"DimArcAngle=%d ;��ע���Ӱ�Բ���߽Ƕ�\n",sys.part_map.plate.bDimArcAngle);
	fprintf(fp,"PlateUseSimpleLsMap=%d ;�Ǹ�ʹ�ü���˨ͼ��\n",sys.part_map.plate.bPlateUseSimpleLsMap);
	fprintf(fp,"AdjustPlateUcs=%d ;�����ְ�����ϵ��֤�����ˮƽ����\n",sys.part_map.plate.bAdjustPlateUcs);
	if(sys.part_map.plate.bDimCoord)
		fprintf(fp,"DimPlankCoord=1;//���ɰ���˨����������\n");
	else
		fprintf(fp,"DimPlankCoord=0;//���ɰ���˨����������\n");
	fprintf(fp,"ZoomDimTextSizeByWidth=%d ;���ݸְ峤���еĽ�Сֵ���Ÿְ���ر�ע����߶�\n",sys.part_map.plate.bZoomDimTextSizeByWidth);
	fprintf(fp,"TextSizeZoomCoef=%f ;����߶�����֮�������ϵ��\n",sys.part_map.plate.fTextSizeZoomCoef);
	fprintf(fp,"DrawSlopeCutLine=%d ;���촦���¿���\n",sys.part_map.plate.bDrawSlopeCutLine);
	fprintf(fp,"DrawDatumLine=%d ;�ְ�Ļ�׼��\n",sys.part_map.plate.bDrawDatumLine);
	fprintf(fp,"CutArcsType=%d :�������ǵ��л���������\n",sys.part_map.plate.iCutArcsType);
	fprintf(fp,"ZhiWanLineType=%d :������λ�õĴ�������\n",sys.part_map.plate.iZhiWanLineType);
	fprintf(fp,"MartHuoquPos=%d :��ǻ���λ��\n",sys.part_map.plate.bMarkHuoquPos);
	fprintf(fp,"DrawHuoQuPrjMap=%d :���ƻ���ʾ��ͼ\n",sys.part_map.plate.bDrawHuoQuPrjMap);
	fprintf(fp,"DrawHuoQuMarkLine=%d :���ƻ���������\n",sys.part_map.plate.bDrawHuoQuMarkLine);
	fprintf(fp,"DisplaypPartInfoTbl=%d ;��ʾ����������\n",sys.part_map.plate.bDisplaypPartInfoTbl);
	fprintf(fp,"PartInfoTblColumnWidth=%d ;��������п�����\n",sys.part_map.plate.nPartInfoTblColumnWidth);
	fprintf(fp,"ShowPlateCfgInfoDlg=%d ;��ʾ�ְ���ϸ�Ի�\n",sys.part_map.plate.bShowPlateCfgInfoDlg);
	//�ֹܹ���ͼ����
	fprintf(fp,"�ֹܹ���ͼ����\n");
	fprintf(fp,"SlotLenProcessError=%f ;�ֹܿ��۳������ֵ\n",sys.part_map.tube.fSlotLenProcessError);
	fprintf(fp,"SlotWidthProcessError=%f ;�ֹܿ��ۿ�����ֵ\n",sys.part_map.tube.fSlotWidthProcessError);
	fprintf(fp,"ReelTubeThresholdD=%f ;�ְ���ֱ����ֵ\n",sys.part_map.tube.fReelTubeThresholdD);

	//��ע����
	fprintf(fp,"��ע����\n");
	fprintf(fp,"FontName=%s	;��������\n",sys.dim_map.sFontName);
	fprintf(fp,"ArrowType=%s ;�ߴ��ע��ͷ����\n",sys.dim_map.sArrowType);
	fprintf(fp,"DimGap=%.2f ;�������ע�߼�϶ֵ\n",sys.dim_map.fDimGap);	
	fprintf(fp,"TblHeaderTextSize=%.2f ;��ͷ�ı���С\n",sys.dim_map.fHeaderTextSize);
	fprintf(fp,"TblContentTextSize=%.2f ;��������ı���С\n",sys.dim_map.fNumericTextSize);
	fprintf(fp,"TblCommonTextSize=%.2f ;����и������ı���С\n",sys.dim_map.fCommonTextSize);
	fprintf(fp,"DimArrowSize=%.2f ;�ߴ��ע��ͷ��С\n",sys.dim_map.fDimArrowSize);
	fprintf(fp,"DimTextSize=%.2f ;���ȳߴ��ע�ı���С\n",sys.dim_map.fDimTextSize);
	fprintf(fp,"DimAngleTextSize=%.2f ;�Ƕȳߴ��ע�ı���С\n",sys.dim_map.fDimAngleTextSize);
	//����ͼ��
	fprintf(fp,"CutawayViewSymbolLen=%.2f ;����ͼ������\n",sys.dim_map.fCutawayViewSymbolLen);
	fprintf(fp,"CutawayViewSymbolTextSize=%.2f ;����ͼ������߶�\n",sys.dim_map.fCutawayViewSymbolTextSize);
	fprintf(fp,"CutawayViewPolyLineWidth=%.2f ;����ͼ������߿��\n",sys.dim_map.fCutawayViewPolyLineWidth);
	//�κű�ע
	fprintf(fp,"SegITextSize=%.2f ;�κű�ע�ı���С\n",sys.dim_map.fSegITextSize);
	fprintf(fp,"SegICirD=%.2f ;�κű�עȦֱ��\n",sys.dim_map.fSegICirD);
	//
	fprintf(fp,"PartNoFrameStyle=%d ;��ſ�����\n",sys.dim_map.PartNo.iPartNoFrameStyle);
	fprintf(fp,"FlPartNoFrameStyle=%d ;������ſ�����\n",sys.dim_map.PartNo.iFlPartNoFrameStyle);
	fprintf(fp,"StdInsertPlatePartNoFrameStyle=%d ;��׼����ſ�����\n",sys.dim_map.PartNo.iStdInsertPlatePartNoFrameStyle);
	fprintf(fp,"PartNoTextSize=%.2f ;��������ı���С\n",sys.dim_map.PartNo.fPartNoTextSize);
	fprintf(fp,"PartNoCirD=%.2f ;�������Ȧֱ��\n",sys.dim_map.PartNo.fPartNoCirD);
	fprintf(fp,"MainPoleNoOuterCirD=%.2f ;���ı����Բֱ��\n",sys.dim_map.PartNo.fMainPoleNoOuterCirD);
	fprintf(fp,"MainPoleNoInterCirD=%.2f ;���ı����Բֱ��\n",sys.dim_map.PartNo.fMainPoleNoInnerCirD);
	fprintf(fp,"MirMsg=%d ;���ı�ű�ע�ԳƷ�ʽ\n",sys.dim_map.PartNo.iMirMsg);
	fprintf(fp,"PartNoMargin=%.2f ;����������ſ�֮��ļ�϶\n",sys.dim_map.PartNo.fPartNoMargin);
	fprintf(fp,"DrawRefLine=%d ;�Ƿ����������\n",sys.dim_map.PartNo.bDrawRefLine);
	//��ͼ����ɫ
	fprintf(fp,"������ɫ����\n");
	fprintf(fp,"VisibleOutline=%d ;�ɼ���������ɫ\n",sys.dim_map.crIndex.crOutLine);
	fprintf(fp,"DashLine=%d ;������ɫ\n",sys.dim_map.crIndex.crDashLine);
	fprintf(fp,"AxisLine=%d ;���ߡ��Գ�����ɫ\n",sys.dim_map.crIndex.crAxisLine);
	fprintf(fp,"SingleLine=%d ;����ͼ����ͼ��ɫ\n",sys.dim_map.crIndex.crSingleLine);
	fprintf(fp,"LsHoleLine=%d ;��˨����ɫ\n",sys.dim_map.crIndex.crLs);
	fprintf(fp,"HuoQuLine=%d ;��������ɫ\n",sys.dim_map.crIndex.crHuoQuLine);
	fprintf(fp,"DimLine=%d ;��ע����ɫ\n",sys.dim_map.crIndex.crDimSimTri);
	fprintf(fp,"DragLine=%d ;��ע����ɫ\n",sys.dim_map.crIndex.crDragLine);
	
	//ͼ��
	fprintf(fp,"ͼ������\n");
	//�ṹͼ�пɼ�������ͼ��
	if(LayerTable::VisibleProfileLayer.layerName.GetLength()>0)
		fprintf(fp,"VisibleProfileLayer.layerName=%s ;�ɼ�������ͼ����\n",(char*)LayerTable::VisibleProfileLayer.layerName);
	//�ṹͼ�в��ɼ�������ͼ��
	if(LayerTable::UnvisibleProfileLayer.layerName.GetLength()>0)
		fprintf(fp,"UnvisibleProfileLayer.layerName=%s ;������ͼ����\n",(char*)LayerTable::UnvisibleProfileLayer.layerName);
	//�����ĵ���ͼ����ͼ
	if(LayerTable::BriefLineLayer.layerName.GetLength()>0)
		fprintf(fp,"BriefLineLayer.layerName=%s ;�����ĵ���ͼ����\n",(char*)LayerTable::BriefLineLayer.layerName);
	//���ĵ���ͼ
	if(LayerTable::AuxLineLayer.layerName.GetLength()>0)
		fprintf(fp,"AuxLineLayer.layerName=%s ;���ĵ���ͼ����\n",(char*)LayerTable::AuxLineLayer.layerName);
	//��˨ͼ��
	if(LayerTable::BoltSymbolLayer.layerName.GetLength()>0)
		fprintf(fp,"BoltSymbolLayer.layerName=%s ;��˨ͼ����\n",(char*)LayerTable::BoltSymbolLayer.layerName);
	//�����α�ע
	if(LayerTable::SimPolyLayer.layerName.GetLength()>0)
		fprintf(fp,"SimPolyLayer.layerName=%s ;������ͼ����\n",(char*)LayerTable::SimPolyLayer.layerName);
	//�����ߡ��Գ��ߡ���λ����
	if(LayerTable::AxisLineLayer.layerName.GetLength()>0)
		fprintf(fp,"AxisLineLayer.layerName=%s ;������ͼ����\n",(char*)LayerTable::AxisLineLayer.layerName);
	//�Ǹֻ������ְ����
	if(LayerTable::BendLineLayer.layerName.GetLength()>0)
		fprintf(fp,"BendLineLayer.layerName=%s ;������ͼ����\n",(char*)LayerTable::BendLineLayer.layerName);
	//�Ͽ�����
	if(LayerTable::BreakLineLayer.layerName.GetLength()>0)
		fprintf(fp,"BreakLineLayer.layerName=%s ;���ߡ��Ͽ���ͼ����\n",(char*)LayerTable::BreakLineLayer.layerName);
	//��˨��
	if(LayerTable::BoltLineLayer.layerName.GetLength()>0)
		fprintf(fp,"BoltLineLayer.layerName=%s ;��˨��ͼ����\n",(char*)LayerTable::BoltLineLayer.layerName);
	//�ߴ��ע
	if(LayerTable::DimSizeLayer.layerName.GetLength()>0)
		fprintf(fp,"DimSizeLayer.layerName=%s ;�ߴ��עͼ����\n",(char*)LayerTable::DimSizeLayer.layerName);
	//���ֱ�ע
	if(LayerTable::DimTextLayer.layerName.GetLength()>0)
		fprintf(fp,"DimTextLayer.layerName=%s ;���ֱ�עͼ����\n",(char*)LayerTable::DimTextLayer.layerName);
	//���������������ͼԪ����ͼ��
	if(LayerTable::CommonLayer.layerName.GetLength()>0)
		fprintf(fp,"CommonLayer.layerName=%s ;����ͼ����\n",(char*)LayerTable::CommonLayer.layerName);
	//����������
	if(LayerTable::AssistHuoQuLineLayer.layerName.GetLength()>0)
		fprintf(fp,"AssistHuoQuLineLayer.layerName=%s ;����������ͼ����\n",(char*)LayerTable::AssistHuoQuLineLayer.layerName);
	//�ӽǸ�������
	if(LayerTable::SonJgAxisLineLayer.layerName.GetLength()>0)
		fprintf(fp,"SonJgAxisLineLayer.layerName=%s ;�ӽǸ�������ͼ����\n",(char*)LayerTable::SonJgAxisLineLayer.layerName);
	//�ְ����������ʶ��
	if(LayerTable::DamagedSymbolLine.layerName.GetLength()>0)
		fprintf(fp,"DamagedSymbolLine.layerName=%s ;�ְ����������ʶ��\n",(char*)LayerTable::DamagedSymbolLine.layerName);
#endif
	//ͼ���ϣ��
	fprintf(fp,"ͼ���ϣ��\n");
	for(TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
	{
		fprintf(fp,"LayerKey=%s;ͼ����;������;�߿�;��ɫ\n",LayerTable::layerHashList.GetCursorKey());
		fprintf(fp,"%s;",(char*)pLayer->layerName);
		fprintf(fp,"%s;",(char*)pLayer->lineTypeName);
		fprintf(fp,"%d;",pLayer->lineWeight);
		fprintf(fp,"%d\n",pLayer->colorIndex);
	}
	fprintf(fp,"��ͼ��������\n");
	for(TEXT_PROP* pTextProp = sys.fonts.hashListProp.GetFirst();pTextProp;pTextProp = sys.fonts.hashListProp.GetNext())
	{
		long key = sys.fonts.hashListProp.GetKey(pTextProp);
		if(key==0)
			continue;
		fprintf(fp,"SpecialSetKey=%d;�ָ�;��߱�;����\n",key);
		fprintf(fp,"%.2f;",pTextProp->fHigh);
		fprintf(fp,"%.2f;",pTextProp->fWtoH);
		fprintf(fp,"%s\n",pTextProp->sFont);
	}
	fprintf(fp,"ͼ���б�\n");
	for(MAP_FRAME_SIZE* pMapFrame=sys.m_hashMapFrameList.GetFirst();pMapFrame;pMapFrame=sys.m_hashMapFrameList.GetNext())
	{
		fprintf(fp,"MapFrameSize=%s;\nͼ������;����;���;����;���ҳ�߼�϶;�Ҳ�ҳ�߼�϶;�ϲ�ҳ�߼�϶;�²�ҳ�߼�϶;ͼ�����²�̶�����;ͼ�����²�̶����\n",sys.m_hashMapFrameList.GetCursorKey());
		fprintf(fp,"%d;",pMapFrame->style);
		fprintf(fp,"%s;",(char*)pMapFrame->sMapFrameName);
		fprintf(fp,"%d;",pMapFrame->nLength);
		fprintf(fp,"%d;",pMapFrame->nWidth);
		fprintf(fp,"%d;",pMapFrame->nLeftMargin);
		fprintf(fp,"%d;",pMapFrame->nRightMargin);
		fprintf(fp,"%d;",pMapFrame->nTopMargin);
		fprintf(fp,"%d;",pMapFrame->nBottomMargin);
		fprintf(fp,"%d;",pMapFrame->nBottomRightFixLen);
		fprintf(fp,"%d\n",pMapFrame->nBottomRightFixWidth);
	}
	fclose(fp);
	//����ע������� wht 11-06-13
	sys.WritePublicSysParaToReg("Settings","KaiHeJiaoThreshold");
	sys.WritePublicSysParaToReg("Settings","KaiHeJiaoMinDeita");
	sys.WritePublicSysParaToReg("Settings","MatCharPosType");
	sys.WritePublicSysParaToReg("Settings","MatSeparator");
	sys.WritePublicSysParaToReg("Settings","MinHuoquAngle");
	sys.WritePublicSysParaToReg("Settings","InnerOffThreshold");
}
void SysSetImport()
{
	SysSetImportDefault(TRUE);
	InitDrawingEnv();
}
void SysSetExport()
{
	SysSetExportDefault(TRUE);
}
void EnvGeneralSet()
{
    // When resource from this ARX app is needed, just
    // instantiate a local CAcModuleResourceOverride
    CAcModuleResourceOverride resOverride;
	CSystemSettingDlg sysSettingDlg;
	sysSettingDlg.DoModal();
	/*CDrawSettingSheet DrawSetSheet;
	//��������
		//��ͼ����
	DrawSetSheet.m_StruPage.m_nLineMapScale = sys.general_set.LineMapScale;
	DrawSetSheet.m_StruPage.ForceLineScale = sys.general_set.ForceLineScale;
	DrawSetSheet.m_StruPage.m_nStruMapScale	= sys.general_set.StruMapScale;
	DrawSetSheet.m_StruPage.m_nPartMapScale = sys.general_set.PartMapScale;
		//������������
	DrawSetSheet.m_StruPage.m_bExportDeformedProfile = sys.part_map.bExportDeformedProfile;
	DrawSetSheet.m_StruPage.m_bDimHuoquType	= sys.part_map.bDimHuoquType;
	DrawSetSheet.m_StruPage.m_bDimPlaneAngle = sys.part_map.bDimPlaneAngle;
		//��˨�״�������
	DrawSetSheet.m_StruPage.m_iLsApertureStyle = sys.general_set.iLsApertureStyle;
	DrawSetSheet.m_StruPage.m_fHoleAugment = sys.general_set.fHoleAugmentInNC;
		//�������տ��������в���
	DrawSetSheet.m_StruPage.m_bProcessCardTbl=sys.general_set.bProcessCardTbl;
	DrawSetSheet.m_StruPage.m_bOneCardMultiPart = sys.part_map.angle.bOneCardMultiPart;
	DrawSetSheet.m_StruPage.m_bHasFrame = sys.general_set.bHasFrame;
	DrawSetSheet.m_StruPage.m_bCardLayoutBaseRow = sys.general_set.bCardLayoutBaseRow;
	DrawSetSheet.m_StruPage.m_nColumnWidth	= sys.general_set.nColumnWidth;
	DrawSetSheet.m_StruPage.m_nRowHigh	= sys.general_set.nRowHigh;
	DrawSetSheet.m_StruPage.m_nPartRowsN = sys.general_set.nPartRowsN;
	DrawSetSheet.m_StruPage.m_sAngleCardPath		= sys.general_set.m_sJgCard;
	DrawSetSheet.m_StruPage.m_sPlankCardPath	= sys.general_set.m_sPlankCard;
	DrawSetSheet.m_StruPage.m_sWeldPartGroupCardPath	= sys.general_set.m_sWeldPartGroupCard;
	DrawSetSheet.m_StruPage.m_sProcessPartStatCardPath	= sys.general_set.m_sProcessPartStatCard;
		//�ṹͼ��ͼ����
	DrawSetSheet.m_StruPage.m_fMinWingWide	 = sys.stru_map.fMinWingWide;
	DrawSetSheet.m_StruPage.m_fMinWingThick	 = sys.stru_map.fMinPartThick;
	DrawSetSheet.m_StruPage.m_bDimSpecialLsSpace=sys.stru_map.bDimSpecialLsSpace;
	DrawSetSheet.m_StruPage.m_bDimSpecialJgZhunJu=sys.stru_map.bDimSpecialJgZhun;
	DrawSetSheet.m_StruPage.m_bDimStruJgLen=sys.stru_map.bDimStruJgLen;
	//�麸ͼ��ͼ����
	DrawSetSheet.m_PartGroupPage.m_bDimAbsoluteDist = sys.part_group_map.bDimAbsoluteDist;
	DrawSetSheet.m_PartGroupPage.m_bDimSpaceDist = sys.part_group_map.bDimBeforeWeldLocatSpace;
	DrawSetSheet.m_PartGroupPage.m_bModulatePartGroupLen = sys.part_group_map.bModulatePartGroupLen;
	DrawSetSheet.m_PartGroupPage.m_bAngularDimension = sys.part_group_map.bAngularDimension;
	DrawSetSheet.m_PartGroupPage.m_bDimAfterWeldCheckLine = sys.part_group_map.bDimAfterWeldCheckLine;
	DrawSetSheet.m_PartGroupPage.m_fPartGroupDistThreshold = sys.part_group_map.fPartGroupDistThreshold;
	DrawSetSheet.m_PartGroupPage.m_fPartGroupDistZoomCoef = sys.part_group_map.fPartGroupDistZoomCoef;
	DrawSetSheet.m_PartGroupPage.m_bDimPlatePoleRayAndAngle = sys.part_group_map.bDimPlatePoleRayAndAngle;
	DrawSetSheet.m_PartGroupPage.m_bAutoDimPartNo = sys.part_group_map.bAutoDimPartNo;
	DrawSetSheet.m_PartGroupPage.m_bAutoMatchView = sys.part_group_map.bAutoMatchView;
	//����ͼ��ͼ����
	DrawSetSheet.m_PartPage.m_iPartMapType = sys.part_map.iPartMapStyle;
		//�Ǹֹ���ͼ��ͼ����
	DrawSetSheet.m_PartPage.m_bDimJgGAtLeft		= sys.part_map.angle.iJgGDimStyle;
	DrawSetSheet.m_PartPage.m_bModulateLongJg	= sys.part_map.angle.bModulateLongJg;
	DrawSetSheet.m_PartPage.m_fLsDistThreshold	= sys.part_map.angle.fLsDistThreshold;
	DrawSetSheet.m_PartPage.m_fLsDistZoomCoef	= sys.part_map.angle.fLsDistZoomCoef;
	DrawSetSheet.m_PartPage.m_bDimLsSpace		= sys.part_map.angle.iLsSpaceDimStyle;
	DrawSetSheet.m_PartPage.m_bDimCutAngle		= sys.part_map.angle.bDimCutAngle;
	DrawSetSheet.m_PartPage.m_bDimCutAngleMap	= sys.part_map.angle.bDimCutAngleMap;
	DrawSetSheet.m_PartPage.m_bDimPushFlatMap	= sys.part_map.angle.bDimPushFlatMap;
	DrawSetSheet.m_PartPage.m_bDimKaiHe			= sys.part_map.angle.bDimKaiHe;
	DrawSetSheet.m_PartPage.m_bDimCutAngleMap	= sys.part_map.angle.bDimKaiheAngleMap;
	DrawSetSheet.m_PartPage.m_bJgUseSimpleLsMap	= sys.part_map.angle.bJgUseSimpleLsMap;
	DrawSetSheet.m_PartPage.m_iDimPrecision=sys.general_set.iDimPrecision;
	DrawSetSheet.m_PartPage.m_iDimPrecision=sys.general_set.iDimPrecision;
		//�ְ幹��ͼ��ͼ����
	DrawSetSheet.m_PartPage.m_bDimPlateOutline  = sys.part_map.plate.bDimPlateEdgeLen;
	DrawSetSheet.m_PartPage.m_bDimPlateRayLs	= sys.part_map.plate.bDimPlateRayLs;
	DrawSetSheet.m_PartPage.m_bDimPlateBoltAngle= sys.part_map.plate.bDimPlateBoltAngle;
	DrawSetSheet.m_PartPage.m_bDimCoord			= sys.part_map.plate.bDimCoord;
	DrawSetSheet.m_PartPage.m_bUseZhengFanQun	= sys.part_map.plate.bUseZhengFanQun;
	DrawSetSheet.m_PartPage.m_bPlateUseSimpleLsMap = sys.part_map.plate.bPlateUseSimpleLsMap;
	//����ͼֽ��ע��������
	DrawSetSheet.m_TextDimPage.m_sFontName = sys.dim_map.sFontName;
	DrawSetSheet.m_TextDimPage.m_fCommonTextSize= sys.dim_map.fCommonTextSize;
	DrawSetSheet.m_TextDimPage.m_fDimArrowSize	= sys.dim_map.fDimArrowSize;
	DrawSetSheet.m_TextDimPage.m_fDimTextSize	= sys.dim_map.fDimTextSize;
	DrawSetSheet.m_TextDimPage.m_fDimAngleTextSize	= sys.dim_map.fDimAngleTextSize;
	DrawSetSheet.m_TextDimPage.m_fHeaderTextSize= sys.dim_map.fHeaderTextSize;
	DrawSetSheet.m_TextDimPage.m_fJgOddmentTextSize	= sys.dim_map.fJgOddmentTextSize;
	DrawSetSheet.m_TextDimPage.m_fOtherTextSize = sys.dim_map.fOtherTextSize;
	DrawSetSheet.m_TextDimPage.m_fLsGuigeTextSize	= sys.dim_map.fLsGuigeTextSize;
	DrawSetSheet.m_TextDimPage.m_fNumericTextSize	= sys.dim_map.fNumericTextSize;
	DrawSetSheet.m_TextDimPage.m_fPartGuigeTextSize	= sys.dim_map.fPartGuigeTextSize;
	DrawSetSheet.m_TextDimPage.m_fPartNoTextSize	= sys.dim_map.fPartNoTextSize;
	DrawSetSheet.m_TextDimPage.m_fPartNoCirD		= sys.dim_map.fPartNoCirD;
	DrawSetSheet.m_TextDimPage.m_fSimTriScale		= sys.dim_map.fSimTriScale;
	//��ͼ����ɫ����
	DrawSetSheet.m_DrawPenPage.m_iVisibleOutline= sys.dim_map.crIndex.crOutLine;//��������
	DrawSetSheet.m_DrawPenPage.m_iSingleLine	= sys.dim_map.crIndex.crSingleLine;
	DrawSetSheet.m_DrawPenPage.m_iAxisLine		= sys.dim_map.crIndex.crAxisLine;
	DrawSetSheet.m_DrawPenPage.m_iHuoQuLine		= sys.dim_map.crIndex.crHuoQuLine;
	DrawSetSheet.m_DrawPenPage.m_iDashLine	= sys.dim_map.crIndex.crDashLine;
	DrawSetSheet.m_DrawPenPage.m_iLsHole		= sys.dim_map.crIndex.crLs;
	DrawSetSheet.m_DrawPenPage.m_iDimSimTri		= sys.dim_map.crIndex.crDimSimTri;
	DrawSetSheet.m_DrawPenPage.m_iDragLine		= sys.dim_map.crIndex.crDragLine;
	if(DrawSetSheet.DoModal()==IDOK)
	{
		//��������
			//��ͼ����
		sys.general_set.LineMapScale = DrawSetSheet.m_StruPage.m_nLineMapScale;
		sys.general_set.ForceLineScale = DrawSetSheet.m_StruPage.ForceLineScale;
		sys.general_set.StruMapScale = DrawSetSheet.m_StruPage.m_nStruMapScale;
		sys.general_set.PartMapScale = DrawSetSheet.m_StruPage.m_nPartMapScale;
			//������������
		sys.part_map.bExportDeformedProfile=DrawSetSheet.m_StruPage.m_bExportDeformedProfile;
		sys.part_map.bDimHuoquType		= DrawSetSheet.m_StruPage.m_bDimHuoquType;
		sys.part_map.bDimPlaneAngle	= DrawSetSheet.m_StruPage.m_bDimPlaneAngle;
			//��˨�״�������
		sys.general_set.iLsApertureStyle = DrawSetSheet.m_StruPage.m_iLsApertureStyle;
		sys.general_set.fHoleAugmentInNC = DrawSetSheet.m_StruPage.m_fHoleAugment;
			//�������տ��������в���
		sys.general_set.bCardLayoutBaseRow	= DrawSetSheet.m_StruPage.m_bCardLayoutBaseRow;
		sys.general_set.nPartRowsN = DrawSetSheet.m_StruPage.m_nPartRowsN;
		sys.general_set.nColumnWidth	  = DrawSetSheet.m_StruPage.m_nColumnWidth;
		sys.general_set.nRowHigh		  = DrawSetSheet.m_StruPage.m_nRowHigh;
		sys.general_set.bProcessCardTbl  = DrawSetSheet.m_StruPage.m_bProcessCardTbl;
		sys.part_map.angle.bOneCardMultiPart = DrawSetSheet.m_StruPage.m_bOneCardMultiPart;
		sys.general_set.bHasFrame		   = DrawSetSheet.m_StruPage.m_bHasFrame;
		sprintf(sys.general_set.m_sJgCard,"%s",DrawSetSheet.m_StruPage.m_sAngleCardPath);
		sprintf(sys.general_set.m_sWeldPartGroupCard,"%s",DrawSetSheet.m_StruPage.m_sWeldPartGroupCardPath);
		sprintf(sys.general_set.m_sProcessPartStatCard,"%s",DrawSetSheet.m_StruPage.m_sProcessPartStatCardPath);
		sprintf(sys.general_set.m_sPlankCard,"%s",DrawSetSheet.m_StruPage.m_sPlankCardPath);
			//�ṹͼ��ͼ����
		sys.stru_map.fMinWingWide = DrawSetSheet.m_StruPage.m_fMinWingWide;
		sys.stru_map.fMinPartThick = DrawSetSheet.m_StruPage.m_fMinWingThick;
		sys.stru_map.bDimSpecialLsSpace=DrawSetSheet.m_StruPage.m_bDimSpecialLsSpace;
		sys.stru_map.bDimSpecialJgZhun=DrawSetSheet.m_StruPage.m_bDimSpecialJgZhunJu;
		sys.stru_map.bDimStruJgLen=DrawSetSheet.m_StruPage.m_bDimStruJgLen;
		//�麸ͼ��ͼ����
		sys.part_group_map.bDimAbsoluteDist = DrawSetSheet.m_PartGroupPage.m_bDimAbsoluteDist;
		sys.part_group_map.bDimBeforeWeldLocatSpace = DrawSetSheet.m_PartGroupPage.m_bDimSpaceDist;
		sys.part_group_map.bModulatePartGroupLen = DrawSetSheet.m_PartGroupPage.m_bModulatePartGroupLen;
		sys.part_group_map.fPartGroupDistThreshold = DrawSetSheet.m_PartGroupPage.m_fPartGroupDistThreshold;
		sys.part_group_map.fPartGroupDistZoomCoef = DrawSetSheet.m_PartGroupPage.m_fPartGroupDistZoomCoef;
		sys.part_group_map.bAngularDimension = DrawSetSheet.m_PartGroupPage.m_bAngularDimension;
		sys.part_group_map.bDimAfterWeldCheckLine = DrawSetSheet.m_PartGroupPage.m_bDimAfterWeldCheckLine;
		sys.part_group_map.bDimBeforeWeldLocatSpace = DrawSetSheet.m_PartGroupPage.m_bDimBeforeWeldCheckLine;
		sys.part_group_map.bDimPlatePoleRayAndAngle = DrawSetSheet.m_PartGroupPage.m_bDimPlatePoleRayAndAngle;
		sys.part_group_map.bAutoDimPartNo = DrawSetSheet.m_PartGroupPage.m_bAutoDimPartNo;
		sys.part_group_map.bAutoMatchView = DrawSetSheet.m_PartGroupPage.m_bAutoMatchView;
		//����ͼ��ͼ����
		sys.part_map.iPartMapStyle = DrawSetSheet.m_PartPage.m_iPartMapType;	//����ͼ���� 0.��ͨ 1.�㽭ʢ��
			//�Ǹֹ���ͼ��ͼ����
		sys.part_map.angle.iJgGDimStyle		= DrawSetSheet.m_PartPage.m_bDimJgGAtLeft;
		sys.part_map.angle.bModulateLongJg	= DrawSetSheet.m_PartPage.m_bModulateLongJg;
		sys.part_map.angle.fLsDistThreshold = DrawSetSheet.m_PartPage.m_fLsDistThreshold;
		sys.part_map.angle.fLsDistZoomCoef  = DrawSetSheet.m_PartPage.m_fLsDistZoomCoef;
		sys.part_map.angle.iLsSpaceDimStyle	  = DrawSetSheet.m_PartPage.m_bDimLsSpace;
		sys.part_map.angle.bDimCutAngle	  = DrawSetSheet.m_PartPage.m_bDimCutAngle;
		sys.part_map.angle.bDimCutAngleMap  = DrawSetSheet.m_PartPage.m_bDimCutAngleMap;
		sys.part_map.angle.bDimPushFlatMap  = DrawSetSheet.m_PartPage.m_bDimPushFlatMap;
		sys.part_map.angle.bDimKaiHe		  = DrawSetSheet.m_PartPage.m_bDimKaiHe;
		sys.part_map.angle.bDimKaiheAngleMap = DrawSetSheet.m_PartPage.m_bDimKaiheAngleMap;
		sys.part_map.angle.bJgUseSimpleLsMap = DrawSetSheet.m_PartPage.m_bJgUseSimpleLsMap;
		sys.general_set.iDimPrecision		 =DrawSetSheet.m_PartPage.m_iDimPrecision;
		sys.general_set.iDimPrecision=DrawSetSheet.m_PartPage.m_iDimPrecision;
			//�ְ幹��ͼ��ͼ����
		sys.part_map.plate.bDimPlateEdgeLen	= DrawSetSheet.m_PartPage.m_bDimPlateOutline;
		sys.part_map.plate.bDimPlateRayLs	= DrawSetSheet.m_PartPage.m_bDimPlateRayLs;
		sys.part_map.plate.bDimPlateBoltAngle= DrawSetSheet.m_PartPage.m_bDimPlateBoltAngle;
		sys.part_map.plate.bDimCoord			= DrawSetSheet.m_PartPage.m_bDimCoord;
		sys.part_map.plate.bUseZhengFanQun	= DrawSetSheet.m_PartPage.m_bUseZhengFanQun;
		sys.part_map.plate.bPlateUseSimpleLsMap	= DrawSetSheet.m_PartPage.m_bPlateUseSimpleLsMap;
		//����ͼֽ��ע��������
		sprintf(sys.dim_map.sFontName,"%s",DrawSetSheet.m_TextDimPage.m_sFontName);
		sys.dim_map.fCommonTextSize		= DrawSetSheet.m_TextDimPage.m_fCommonTextSize;
		sys.dim_map.fDimArrowSize		= DrawSetSheet.m_TextDimPage.m_fDimArrowSize;
		sys.dim_map.fDimTextSize		= DrawSetSheet.m_TextDimPage.m_fDimTextSize;
		sys.dim_map.fDimAngleTextSize	= DrawSetSheet.m_TextDimPage.m_fDimAngleTextSize;
		sys.dim_map.fHeaderTextSize		= DrawSetSheet.m_TextDimPage.m_fHeaderTextSize;
		sys.dim_map.fJgOddmentTextSize	= DrawSetSheet.m_TextDimPage.m_fJgOddmentTextSize;
		sys.dim_map.fOtherTextSize = DrawSetSheet.m_TextDimPage.m_fOtherTextSize;
		sys.dim_map.fLsGuigeTextSize	= DrawSetSheet.m_TextDimPage.m_fLsGuigeTextSize;
		sys.dim_map.fNumericTextSize	= DrawSetSheet.m_TextDimPage.m_fNumericTextSize;
		sys.dim_map.fPartGuigeTextSize	= DrawSetSheet.m_TextDimPage.m_fPartGuigeTextSize;
		sys.dim_map.fPartNoTextSize		= DrawSetSheet.m_TextDimPage.m_fPartNoTextSize;
		sys.dim_map.fPartNoCirD			= DrawSetSheet.m_TextDimPage.m_fPartNoCirD;
		sys.dim_map.fSimTriScale		= DrawSetSheet.m_TextDimPage.m_fSimTriScale;
		//��ͼ����ɫ����
		sys.dim_map.crIndex.crOutLine	= DrawSetSheet.m_DrawPenPage.m_iVisibleOutline;//��������
		sys.dim_map.crIndex.crSingleLine= DrawSetSheet.m_DrawPenPage.m_iSingleLine;
		sys.dim_map.crIndex.crAxisLine	= DrawSetSheet.m_DrawPenPage.m_iAxisLine;
		sys.dim_map.crIndex.crHuoQuLine	= DrawSetSheet.m_DrawPenPage.m_iHuoQuLine	;
		sys.dim_map.crIndex.crDashLine	= DrawSetSheet.m_DrawPenPage.m_iDashLine;
		sys.dim_map.crIndex.crLs		= DrawSetSheet.m_DrawPenPage.m_iLsHole;
		sys.dim_map.crIndex.crDimSimTri	= DrawSetSheet.m_DrawPenPage.m_iDimSimTri;
		sys.dim_map.crIndex.crDragLine  = DrawSetSheet.m_DrawPenPage.m_iDragLine;
		SysSetExportDefault(FALSE);
	}*/
}
void JgGuiGeFileImport()
{
	CString sFileName;
#ifdef AFX_TARG_ENU_ENGLISH
	CFileDialog dlg(TRUE,"jgt","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Angle spec file(*.jgt)|*.jgt||");
#else
	CFileDialog dlg(TRUE,"jgt","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"�Ǹֹ���ļ�(*.jgt)|*.jgt||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		JgGuiGeSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import angle spec library file!");
#else
		AfxMessageBox("�ɹ�����Ǹֹ����ļ�!");
#endif
	}
}

void TubeGuiGeFileImport()
{
	CString sFileName;
#ifdef AFX_TARG_ENU_ENGLISH
	CFileDialog dlg(TRUE,"lst","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Tube spec file(*.pgt)|*.pgt||");
#else
	CFileDialog dlg(TRUE,"lst","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"�ֹܹ���ļ�(*.pgt)|*.pgt||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		TubeGuiGeSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import tube spec library file!");
#else
		AfxMessageBox("�ɹ�����ֹܹ����ļ�!");
#endif
	}
}

//������Ͳ���
void CInsertPlateLibImport()
{
	CString sFileName;
#ifdef AFX_TARG_ENU_ENGLISH
	CFileDialog dlg(TRUE,"cipl","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Slot insert plate library file(*.cipl)|*.cipl||");
#else
	CFileDialog dlg(TRUE,"cipl","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"���Ͳ����ļ�(*.cipl)|*.cipl||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		CInsertPlateSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import slot inserting plate library file!");
#else
		AfxMessageBox("�ɹ�������Ͳ����ļ�!");
#endif
	}
}

//����U�Ͳ���
void UInsertPlateLibImport()
{
	CString sFileName;
#ifdef AFX_TARG_ENU_ENGLISH
	CFileDialog dlg(TRUE,"uipl","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"U insert plate library file(*.uipl)|*.uipl||");
#else
	CFileDialog dlg(TRUE,"uipl","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"U�Ͳ����ļ�(*.uipl)|*.uipl||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		UInsertPlateSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import U inserting plate library file!");
#else
		AfxMessageBox("�ɹ�����U�Ͳ����ļ�!");
#endif
	}
}

//����X�Ͳ���
void XInsertPlateLibImport()
{
	CString sFileName;
	CFileDialog dlg(TRUE,"xipl","Tower",
#ifdef AFX_TARG_ENU_ENGLISH
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"X insert plate library file(*.xipl)|*.xipl||");
#else
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"ʮ���Ͳ����ļ�(*.xipl)|*.xipl||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		XInsertPlateSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import X inserting plate library file spec library file!");
#else
		AfxMessageBox("�ɹ�����ʮ���Ͳ����ļ�!");
#endif
	}
}

//����ƽ��������
void PFlLibImport()
{
	CString sFileName;
#ifdef AFX_TARG_ENU_ENGLISH
	CFileDialog dlg(TRUE,"flp","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Plane weld flange library file(*.flp)|*.flp||");
#else
	CFileDialog dlg(TRUE,"flp","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"ƽ���������ļ�(*.flp)|*.flp||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		FlPSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import plane weld flange library file!");
#else
		AfxMessageBox("�ɹ�����ƽ���������ļ�!");
#endif
	}
}

//����Ժ�������
void DFlLibImport()
{
	CString sFileName;
#ifdef AFX_TARG_ENU_ENGLISH
	CFileDialog dlg(TRUE,"fld","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Relative weld flange library file(*.fld)|*.fld||");
#else
	CFileDialog dlg(TRUE,"fld","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"�Ժ��������ļ�(*.fld)|*.fld||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		FlDSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import relative weld flange library file!");
#else
		AfxMessageBox("�ɹ�����Ժ��������ļ�!");
#endif
	}
}
//������Է�����
void GFlLibImport()
{
	CString sFileName;
#ifdef AFX_TARG_ENU_ENGLISH
	CFileDialog dlg(TRUE,"flg","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Rigid flange library file(*.flg)|*.flg||");
#else
	CFileDialog dlg(TRUE,"flg","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"���Է������ļ�(*.flg)|*.flg||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		FlGSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import rigid flange library file!");
#else
		AfxMessageBox("�ɹ�������Է������ļ�!");
#endif
	}
}
//�������Է�����
void RFlLibImport()
{
	CString sFileName;
#ifdef AFX_TARG_ENU_ENGLISH
	CFileDialog dlg(TRUE,"flr","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Flexible flange library file(*.flr)|*.flr||");
#else
	CFileDialog dlg(TRUE,"flr","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"���Է������ļ�(*.flr)|*.flr||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		FlRSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import flexible flange library file!");
#else
		AfxMessageBox("�ɹ��������Է������ļ�!");
#endif
	}
}

//�Խӷ����������ļ�
void FlCoupleLevelFileImport()
{
	CString sFileName;
#ifdef AFX_TARG_ENU_ENGLISH
	CFileDialog dlg(TRUE,"fll","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"File Flange couple level defines(*.fll)|*.fll||");
#else
	CFileDialog dlg(TRUE,"fll","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"�����ԽӼ������ļ�(*.fll)|*.fll||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		FlCoupleLevelSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import file flange couple level defines!");
#else
		AfxMessageBox("�ɹ�����Խӷ����������ļ�!");
#endif
	}

}

void SystemSetting()
{
	CSystemSettingDlg sysSettingDlg;
	sysSettingDlg.DoModal();
}