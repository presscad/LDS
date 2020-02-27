//<LOCALE_TRANSLATE BY hxr /> 2015-04-25
//配置
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

	//设置单线图及分段结构图比例
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
			"绘图设置文件(*.set)|*.set||");
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
		//加载ARX时,初始化样板明细配置
		CXhChar100 plate_cfg_file;
		GetAppPath(plate_cfg_file);
		strcat(plate_cfg_file,"Plate.pds");
		strcpy(sys.part_map.plate.sPlateConfigFilePath,plate_cfg_file);
		sys.part_map.plate.bUseDimStyleConfigFile=CPlankPartParaExportDlg::LoadDimStyleFromFile(plate_cfg_file);	
	}
	if((fp=fopen(file_name,"rt"))==NULL)
	{
		//AfxMessageBox("打不开指定的配置文件!");//此时报错无意义 wjh-2015.2.3
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
		BOOL bContinue=FALSE;	//用来打断if-else wht 11-07-04
		//常规设置
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
		else if(_stricmp(key_word,"bHasFrame")==0)	//绘制装订线
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
	//分段标志线
		else if(_stricmp(key_word,"DrawMainPoleJointLine")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.bDrawMainPoleJointLine);
		else if(_stricmp(key_word,"JointLineOffset")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.nJointLineOffset);
		else if(_stricmp(key_word,"JointLineLen")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.nJointLineLen);
		//数字化移交
		else if(_stricmp(key_word,"RCrossarmCr")==0)
			sscanf(line_txt,"%s%X",key_word,&sys.general_set.rCrossarmCr);
		else if(_stricmp(key_word,"LCrossarmCr")==0)
			sscanf(line_txt,"%s%X",key_word,&sys.general_set.lCrossarmCr);
	//结构图设置
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
		//组焊图绘图设置
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
			//组焊件明细表
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
		else if(_stricmp(key_word,"bDispInternal")==0)	//焊缝级别－内在
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.part_table.bDispInternal);
		else if(_stricmp(key_word,"bDispAppearance")==0)//焊缝级别－外观
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.part_table.bDispAppearance);
		else if(_stricmp(key_word,"bZoomInProcessCard")==0)//工艺卡内进行比例缩放－外观
			sscanf(line_txt,"%s%d",key_word,&sys.part_group_map.part_table.bZoomInProcessCard);
	//构件图
		if(_stricmp(key_word,"ExportDeformedProfile")==0)
			sscanf(line_txt,"%s%d",key_word,&sys.part_map.iExportDeformedProfile);
		else if(_stricmp(key_word,"DimDeformedProfileText")==0)
		{	//钢板考虑火曲变形时是否标注火曲变形相关文字 wht 10-11-01
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
		else if(_stricmp(key_word,"DimAngleType")==0) //0.锐角 1.钝角 2.自动
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
		//角钢构件图参数
		else if(_stricmp(key_word,"JgZoomSchema")==0) //角钢缩放方案
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
		//钢板构件图
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
		else if(_stricmp(key_word,"DimPlankBoltAngle")==0)//标注板螺栓角度
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
		//钢管构件图
		else if(_stricmp(key_word,"SlotWidthProcessError")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.part_map.tube.fSlotWidthProcessError);
		else if(_stricmp(key_word,"SlotLenProcessError")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.part_map.tube.fSlotLenProcessError);
		else if(_stricmp(key_word,"ReelTubeThresholdD")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.part_map.tube.fReelTubeThresholdD);

	//图纸标注
		else if(_stricmp(key_word,"FontName")==0)
		{	//字体名称
			skey=strtok(NULL,"=,;");
			CString sName=skey; //去除多余空格,回车,制表符,回车
			sName.Remove(' ');
			sName.Remove('\r');
			sName.Remove('\n');
			sName.Remove('\t');
			sprintf(sys.dim_map.sFontName,"%s",sName);
		}
		else if(_stricmp(key_word,"ArrowType")==0)
		{	//字体名称
			skey=strtok(NULL,"=,;");
			CString sArrowType=skey; //去除多余空格
			sArrowType.Remove(' ');
			sprintf(sys.dim_map.sArrowType,"%s",sArrowType);	//尺寸标注箭头类型
		}
		else if(_stricmp(key_word,"DimGap")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fDimGap);	//文字与标注线间隙值 wht 10-08-02
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
		//剖视图符
		else if(_stricmp(key_word,"CutawayViewSymbolLen")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fCutawayViewSymbolLen);
		else if(_stricmp(key_word,"CutawayViewSymbolTextSize")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fCutawayViewSymbolTextSize);
		else if(_stricmp(key_word,"CutawayViewPolyLineWidth")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fCutawayViewPolyLineWidth);
		//段号标注
		else if(_stricmp(key_word,"SegITextSize")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fSegITextSize);
		else if(_stricmp(key_word,"SegICirD")==0)
			sscanf(line_txt,"%s%lf",key_word,&sys.dim_map.fSegICirD);
		//构件编号
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
		if(bContinue)	//在此处打断 if-else wht 11-07-04
		{
			//结构图中可见轮廓线图层
			if(_stricmp(key_word,"VisibleProfileLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::VisibleProfileLayer.layerName);
			//结构图中不可见轮廓线图层
			else if(_stricmp(key_word,"UnvisibleProfileLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::UnvisibleProfileLayer.layerName);
			//受力材单线图、总图
			else if(_stricmp(key_word,"BriefLineLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::BriefLineLayer.layerName);
			//辅材单线图
			else if(_stricmp(key_word,"AuxLineLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::AuxLineLayer.layerName);
			//螺栓图符
			else if(_stricmp(key_word,"BoltSymbolLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::BoltSymbolLayer.layerName);
			//相似形标注
			else if(_stricmp(key_word,"SimPolyLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::SimPolyLayer.layerName);
			//中心线、对称线、定位轴线
			else if(_stricmp(key_word,"AxisLineLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::AxisLineLayer.layerName);
			//角钢火曲、钢板火曲
			else if(_stricmp(key_word,"BendLineLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::BendLineLayer.layerName);
			//断开界线
			else if(_stricmp(key_word,"BreakLineLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::BreakLineLayer.layerName);
			//螺栓线
			else if(_stricmp(key_word,"BoltLineLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::BoltLineLayer.layerName);
			//尺寸标注
			else if(_stricmp(key_word,"DimSizeLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::DimSizeLayer.layerName);
			//文字标注
			else if(_stricmp(key_word,"DimTextLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::DimTextLayer.layerName);
			//除以上以外的所有图元所在图层
			else if(_stricmp(key_word,"CommonLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::CommonLayer.layerName);
			//辅助火曲线
			else if(_stricmp(key_word,"AssistHuoQuLineLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
			//子角钢中心线
			else if(_stricmp(key_word,"SonJgAxisLineLayer.layerName")==0)
				sscanf(line_txt,"%s%s",key_word,(char*)LayerTable::SonJgAxisLineLayer.layerName);
			//图层哈希表
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
			//图框列表
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
			"绘图设置文件(*.set)|*.set||");
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
		AfxMessageBox("打不开指定的配置文件!");
#endif
		return;
	}
	//常规设置
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
	//构件工艺卡及其排列参数
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
	//工装辅助钢板库文件目录
	fprintf(fp,"SetupAidedPlate=%s ;Setup aided plate library file contents\n",sys.general_set.m_sSetupAidedPlate);
	//出图模块路径设置
	fprintf(fp,"TMAMapArxPath=%s ;TMA map arx path\n",sys.general_set.m_sTMAMapArxPath);
	fprintf(fp,"LMAMapArxPath=%s ;LMA map arx path\n",sys.general_set.m_sLMAMapArxPath);
	fprintf(fp,"LDSMapArxPath=%s ;LDS map arx path\n",sys.general_set.m_sLDSMapArxPath);
	fprintf(fp,"TSAMapArxPath=%s ;TSA map arx path\n",sys.general_set.m_sTSAMapArxPath);
	//数字化移交属性设置
	fprintf(fp,"LCrossarmCr=%X ;Left Arm Color\n",sys.general_set.lCrossarmCr);
	fprintf(fp,"RCrossarmCr=%X ;Rignt Arm Color\n",sys.general_set.rCrossarmCr);
	//结构图设置
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
	//结构图字高设置
	fprintf(fp,"PlateThickTextSize=%.2f ;Text size of plate's thickness\n",sys.dim_map.fPlateThickTextSize);
	fprintf(fp,"SimTriTextSize=%.2f ;Text size of similar figure\n",sys.dim_map.fSimPolyTextSize);
	fprintf(fp,"OtherTextSize=%.2f ;Other normal text size\n",sys.dim_map.fOtherTextSize);
	fprintf(fp,"JgOddmentTextSize=%.2f ;Text size of angle oddment\n",sys.dim_map.fJgOddmentTextSize);
	fprintf(fp,"PartGuigeTextSize=%.2f ;Text size of part spec\n",sys.dim_map.fPartGuigeTextSize);
	fprintf(fp,"SimTriScale=%.3f ;Zoom scale of similar figure\n",sys.dim_map.fSimTriScale);
	fprintf(fp,"LsGuigeTextSize=%.2f ;Text size of bolt sepc information\n",sys.dim_map.fLsGuigeTextSize);
	//组焊图绘图设置
	fprintf(fp,"Weld group map drawing settings\n");
	fprintf(fp,"MinFontSize=%.2f ;Min font height in weld group map\n",sys.part_group_map.fMinFontSize);
	fprintf(fp,"MinDrawThick=%.2f ;The thickness of the minimum value\n",sys.part_group_map.fMinDrawThick);
	fprintf(fp,"KmJgGIncrement=%d ;Explorator Angle G Adjust Threshold\n",sys.part_group_map.nKmJgGIncrement);
	fprintf(fp,"ModulatePartGroupLen=%d	;Modulate weld group map displayed length\n",sys.part_group_map.bModulatePartGroupLen);
	fprintf(fp,"PartGroupDistZoomCoef=%.2f	;Zoom coefficient of weld part group\n",sys.part_group_map.fPartGroupDistZoomCoef);
	fprintf(fp,"fPartGroupDistThreshold=%.2f;Zoom threshold of weld part group\n",sys.part_group_map.fPartGroupDistThreshold);
	//以下有必要再译hxr2015-4-25
	fprintf(fp,"bDrawWeldLineSymbol=%d;绘制焊缝线标记\n",sys.part_group_map.bDrawWeldLineSymbol);
	fprintf(fp,"iPlateLocateDrawStyle=%d;组焊图中绘制钢板对活点\n",sys.part_group_map.iPlateLocateDrawStyle);
	fprintf(fp,"iLocateSymboleHeight=%d;组焊图中钢板对活点高度\n",sys.part_group_map.iLocateSymboleHeight);
	fprintf(fp,"bDimEndOuterPlaneSpace=%d;杆件外端面到外端面距离\n",sys.part_group_map.bDimEndOuterPlaneSpace);
	fprintf(fp,"AutoDimPartNo=%d	;自动标注构件编号\n",sys.part_group_map.bAutoDimPartNo);
	fprintf(fp,"AutoMatchView=%d	;自动组合组焊件视图\n",sys.part_group_map.bAutoMatchView);
	fprintf(fp,"DimAbsoluteDist=%d	;标注定位点绝对尺寸\n",sys.part_group_map.bDimAbsoluteDist);
	fprintf(fp,"DimBeforeWeldLocatSpace=%d	;标注焊接前定位尺寸\n",sys.part_group_map.bDimBeforeWeldLocatSpace);
	fprintf(fp,"DimAfterWeldCheckLine=%d	;标注焊接后检测线\n",sys.part_group_map.bDimAfterWeldCheckLine);
	fprintf(fp,"AngularDimension=%d	;标注角度线\n",sys.part_group_map.bAngularDimension);
	fprintf(fp,"DimPlatePoleRayAndAngle=%d	;标注钢板连接的杆件射线以及角度\n",sys.part_group_map.bDimPlatePoleRayAndAngle);
	fprintf(fp,"SmartFilterRedundantPart=%d	;视图中自动筛选出与当前视图无关的多余构件跳过不显示\n",sys.part_group_map.bSmartFilterRedundantPart);
	fprintf(fp,"CreatePartGroupBOM=%d	;生成组焊件明细表\n",sys.part_group_map.bCreatePartGroupBOM);
	fprintf(fp,"WeldingFontNeedZoom=%d	;字体随动缩放\n",sys.welding.bZoomAdaptiveWithScale);
	fprintf(fp,"WeldingFontScale=%d	;字体基准缩放比例\n",sys.welding.wiScaleModel2Draw);
	//组焊件明细表
	fprintf(fp,"DispPartGroupNo=%d	;//组焊件编号\n",sys.part_group_map.part_table.bDispPartGroupNo);
	fprintf(fp,"DispPartGroupNum=%d	;//组焊件数量\n",sys.part_group_map.part_table.bDispPartGroupNum);
	fprintf(fp,"DispPartNo=%d	;//构件编号\n",sys.part_group_map.part_table.bDispPartNo);
	fprintf(fp,"DispPartNum=%d	;//构件数量\n",sys.part_group_map.part_table.bDispPartNum);
	fprintf(fp,"DispPartGuiGe=%d	;//构件规格\n",sys.part_group_map.part_table.bDispPartGuiGe);
	fprintf(fp,"DispPartLen=%d	;//构件长度\n",sys.part_group_map.part_table.bDispPartLen);
	fprintf(fp,"DispPartWeight=%d	;//构件重量\n",sys.part_group_map.part_table.bDispPartWeight);
	fprintf(fp,"DispNotes=%d	;//备注\n",sys.part_group_map.part_table.bDispNotes);
	fprintf(fp,"bDispInternal=%d	;//焊缝级别－内在\n",sys.part_group_map.part_table.bDispInternal);
	fprintf(fp,"bDispAppearance=%d	;//焊缝级别－外观\n",sys.part_group_map.part_table.bDispAppearance);
	fprintf(fp,"bZoomInProcessCard=%d	;//工艺卡内进行比例缩放\n",sys.part_group_map.part_table.bZoomInProcessCard);
	//构件图
	fprintf(fp,"ExportDeformedProfile=%d	;//构件图考虑制弯变形后的效应\n",sys.part_map.iExportDeformedProfile);
	fprintf(fp,"DimDeformedProfileText=%d ;考虑火曲变形时是否标注火曲变形相关文字\n",sys.part_map.bDimDeformedProfileText);
	fprintf(fp,"DimHuoquType=%d ;冷弯热弯\n",sys.part_map.bDimHuoquType);
	fprintf(fp,"DimPlaneAngle=%d ;标注火曲面之间的夹角\n",sys.part_map.bDimPlaneAngle);
	fprintf(fp,"DimAngleType=%d ;角度标注类型\n",sys.part_map.iDimAngleType);
	fprintf(fp,"PartMapStyle=%d ;构件图风格样式设置\n",sys.part_map.iPartMapStyle);
	fprintf(fp,"UseZhengFanQun=%d ;启用正反曲否则使用外内曲\n",sys.part_map.bUseZhengFanQun);
	//角钢构件图设置
	fprintf(fp,"角钢构件图设置\n");
	fprintf(fp,"ModulateLongJg=%d ;调整角钢长度\n",sys.part_map.angle.bModulateLongJg);
	fprintf(fp,"JgZoomSchema=%d ;//0.1:1绘制 1.使用构件图比例 2.长宽同比缩放 3.长宽分别缩放\n",sys.part_map.angle.iJgZoomSchema);
	fprintf(fp,"JgLenDrawMode=%d ;//0.向右延伸 1.中间挤压\n",sys.part_map.angle.ciJgLenDrawMode);
	fprintf(fp,"MaxExtendAngleLength=%d ;//最大限度延伸角钢绘制长度\n",sys.part_map.angle.bMaxExtendAngleLength);
	fprintf(fp,"LsDistThreshold=%.2f ;角钢长度自动调整螺栓间距阈值\n",sys.part_map.angle.fLsDistThreshold);
	fprintf(fp,"LsDistZoomCoef=%.2f ;//螺栓间距缩放系数\n",sys.part_map.angle.fLsDistZoomCoef);
	fprintf(fp,"OneCardMultiPart=%d ;//角钢允许一卡多件情况\n",sys.part_map.angle.bOneCardMultiPart);
	fprintf(fp,"JgGDimStyle=%d ;//0.始端标注 1.中间标注 2.不标注\n",sys.part_map.angle.iJgGDimStyle);
	fprintf(fp,"MaxBoltNumStartDimG=%d ;//集中在始端标注准距支持的最大螺栓数\n",sys.part_map.angle.nMaxBoltNumStartDimG);
	fprintf(fp,"LsSpaceDimStyle=%d ;//0.X轴方向	 1.Y轴方向  2.自动判断 3.不标注\n",sys.part_map.angle.iLsSpaceDimStyle);
	fprintf(fp,"MaxBoltNumAlongX=%d ;//沿X轴方向标注支持的最大螺栓个数\n",sys.part_map.angle.nMaxBoltNumAlongX);
	fprintf(fp,"DimCutAngle=%d ;//标注角钢切角\n",sys.part_map.angle.bDimCutAngle);
	fprintf(fp,"DimCutAngleMap=%d ;//标注角钢切角示意图\n",sys.part_map.angle.bDimCutAngleMap);
	fprintf(fp,"DimPushFlatMap=%d ;//标注压扁示意图\n",sys.part_map.angle.bDimPushFlatMap);
	fprintf(fp,"JgUseSimpleLsMap=%d ;//角钢使用简化螺栓图符\n",sys.part_map.angle.bJgUseSimpleLsMap);
	fprintf(fp,"DimLsAbsoluteDist=%d ;//标注螺栓绝对尺寸\n",sys.part_map.angle.bDimLsAbsoluteDist);
	fprintf(fp,"bMergeLsAbsoluteDist=%d ;//合并相邻等距螺栓绝对尺寸\n",sys.part_map.angle.bMergeLsAbsoluteDist);
	fprintf(fp,"DimRibPlatePartNo=%d ;//标注角钢肋板编号\n",sys.part_map.angle.bDimRibPlatePartNo);
	fprintf(fp,"DimRibPlateMaterial=%d ;//标注角钢肋板材质\n",sys.part_map.angle.bDimRibPlateMaterial);
	fprintf(fp,"bDimRibPlateSetUpPos=%d ;//标注角钢肋板安装位置\n",sys.part_map.angle.bDimRibPlateSetUpPos);
	fprintf(fp,"CutAngleDimType=%d ;//角钢切角标注类型\n",sys.part_map.angle.iCutAngleDimType);
	fprintf(fp,"DimKaiHe=%d ;//标注角钢开合角\n",sys.part_map.angle.bDimKaiHe);
	fprintf(fp,"DimKaiheAngleMap=%d ;//标注角钢开合角示意图\n",sys.part_map.angle.bDimKaiheAngleMap);
	fprintf(fp,"KaiHeJiaoThreshold=%.2f ;开合角标注阈值\n",sys.part_map.angle.fKaiHeJiaoThreshold);
	fprintf(fp,"MatCharPosType=%d ;//材质字符标注位置\n",sys.general_set.iMatCharPosType);
	fprintf(fp,"DimKaiHeSumLen=%d ;//标注角钢开合区域总长度\n",sys.part_map.angle.bDimKaiheSumLen);
	fprintf(fp,"DimKaiHeAngle=%d ;//标注角钢开合角度数\n",sys.part_map.angle.bDimKaiheAngle);
	fprintf(fp,"DimKaiHeSegLen=%d ;//标注角钢开合区域分段长\n",sys.part_map.angle.bDimKaiheSegLen);
	fprintf(fp,"DimKaiHeScopeMap=%d ;//标注角钢开合区域分段长\n",sys.part_map.angle.bDimKaiheScopeMap);
	//钢板构件图
	fprintf(fp,"钢板构件图设置\n");
	fprintf(fp,"DimPlateEdgeLen=%d ;标连接板外轮廓\n",sys.part_map.plate.bDimPlateEdgeLen);
	fprintf(fp,"DrawInnerEdge=%d ;绘内侧轮廓边\n",sys.part_map.plate.bDrawInnerEdge);
	fprintf(fp,"InnerEdgeOffset=%f ;轮廓边偏移距\n",sys.part_map.plate.fInnerEdgeOffset);
	fprintf(fp,"DimPlankRayLs=%d ;标注连接板螺栓间隙\n",sys.part_map.plate.bDimPlateRayLs);
	fprintf(fp,"DimPlankBoltAngle=%d ;标注连接板螺栓角度\n",sys.part_map.plate.bDimPlateBoltAngle);
	fprintf(fp,"DimBoltEdgeSpace=%d ;标注连接板螺栓角度\n",sys.part_map.plate.bDimBoltEdgeSpace);
	fprintf(fp,"DimDiffRayLsSpace=%d ;标注连接板螺栓角度\n",sys.part_map.plate.bDimDiffRayLsSpace);
	fprintf(fp,"DimLsToHuoquLine=%d ;标注连接板螺栓角度\n",sys.part_map.plate.bDimLsToHuoquLine);
	fprintf(fp,"DimLsToWeldEdge=%d ;标注连接板螺栓角度\n",sys.part_map.plate.bDimLsToWeldEdge);
	fprintf(fp,"DimArcEdgeR=%d ;标注连接板圆弧边半径\n",sys.part_map.plate.bDimArcEdgeR);
	fprintf(fp,"DimArcAngle=%d ;标注连接板圆弧边角度\n",sys.part_map.plate.bDimArcAngle);
	fprintf(fp,"PlateUseSimpleLsMap=%d ;角钢使用简化螺栓图符\n",sys.part_map.plate.bPlateUseSimpleLsMap);
	fprintf(fp,"AdjustPlateUcs=%d ;调整钢板坐标系保证最长边沿水平方向\n",sys.part_map.plate.bAdjustPlateUcs);
	if(sys.part_map.plate.bDimCoord)
		fprintf(fp,"DimPlankCoord=1;//生成板螺栓、顶点坐标\n");
	else
		fprintf(fp,"DimPlankCoord=0;//生成板螺栓、顶点坐标\n");
	fprintf(fp,"ZoomDimTextSizeByWidth=%d ;根据钢板长宽中的较小值缩放钢板相关标注字体高度\n",sys.part_map.plate.bZoomDimTextSizeByWidth);
	fprintf(fp,"TextSizeZoomCoef=%f ;字体高度与宽度之间的缩放系数\n",sys.part_map.plate.fTextSizeZoomCoef);
	fprintf(fp,"DrawSlopeCutLine=%d ;焊缝处的坡口线\n",sys.part_map.plate.bDrawSlopeCutLine);
	fprintf(fp,"DrawDatumLine=%d ;钢板的基准线\n",sys.part_map.plate.bDrawDatumLine);
	fprintf(fp,"CutArcsType=%d :焊烧死角的切弧处理设置\n",sys.part_map.plate.iCutArcsType);
	fprintf(fp,"ZhiWanLineType=%d :制弯线位置的处理设置\n",sys.part_map.plate.iZhiWanLineType);
	fprintf(fp,"MartHuoquPos=%d :标记火曲位置\n",sys.part_map.plate.bMarkHuoquPos);
	fprintf(fp,"DrawHuoQuPrjMap=%d :绘制火曲示意图\n",sys.part_map.plate.bDrawHuoQuPrjMap);
	fprintf(fp,"DrawHuoQuMarkLine=%d :绘制火曲特征线\n",sys.part_map.plate.bDrawHuoQuMarkLine);
	fprintf(fp,"DisplaypPartInfoTbl=%d :显示构件表设置\n",sys.part_map.plate.bDisplaypPartInfoTbl);
	fprintf(fp,"PartInfoTblColumnWidth=%d :构件表的列宽设置\n",sys.part_map.plate.nPartInfoTblColumnWidth);	
	fprintf(fp,"PlateConfigFilePath=%d :Plate config file\n",sys.part_map.plate.sPlateConfigFilePath);
	fprintf(fp,"ShowPlateCfgInfoDlg=%d ;显示钢板明细对话\n",sys.part_map.plate.bShowPlateCfgInfoDlg);
	//钢管构件图设置
	fprintf(fp,"钢管构件图设置\n");
	fprintf(fp,"SlotLenProcessError=%f ;钢管开槽长度误差值\n",sys.part_map.tube.fSlotLenProcessError);
	fprintf(fp,"SlotWidthProcessError=%f ;钢管开槽宽度误差值\n",sys.part_map.tube.fSlotWidthProcessError);
	fprintf(fp,"ReelTubeThresholdD=%f ;钢板卷管直径阈值\n",sys.part_map.tube.fReelTubeThresholdD);

	//标注设置
	fprintf(fp,"标注设置\n");
	fprintf(fp,"FontName=%s	;字体名称\n",sys.dim_map.sFontName);
	fprintf(fp,"ArrowType=%s ;尺寸标注箭头类型\n",sys.dim_map.sArrowType);
	fprintf(fp,"DimGap=%.2f ;文字与标注线间隙值\n",sys.dim_map.fDimGap);	
	fprintf(fp,"TblHeaderTextSize=%.2f ;表头文本大小\n",sys.dim_map.fHeaderTextSize);
	fprintf(fp,"TblContentTextSize=%.2f ;表格内容文本大小\n",sys.dim_map.fNumericTextSize);
	fprintf(fp,"TblCommonTextSize=%.2f ;表格中副标题文本大小\n",sys.dim_map.fCommonTextSize);
	fprintf(fp,"DimArrowSize=%.2f ;尺寸标注箭头大小\n",sys.dim_map.fDimArrowSize);
	fprintf(fp,"DimTextSize=%.2f ;长度尺寸标注文本大小\n",sys.dim_map.fDimTextSize);
	fprintf(fp,"DimAngleTextSize=%.2f ;角度尺寸标注文本大小\n",sys.dim_map.fDimAngleTextSize);
	//剖视图符
	fprintf(fp,"CutawayViewSymbolLen=%.2f ;剖视图符长度\n",sys.dim_map.fCutawayViewSymbolLen);
	fprintf(fp,"CutawayViewSymbolTextSize=%.2f ;剖视图符字体高度\n",sys.dim_map.fCutawayViewSymbolTextSize);
	fprintf(fp,"CutawayViewPolyLineWidth=%.2f ;剖视图符多段线宽度\n",sys.dim_map.fCutawayViewPolyLineWidth);
	//段号标注
	fprintf(fp,"SegITextSize=%.2f ;段号标注文本大小\n",sys.dim_map.fSegITextSize);
	fprintf(fp,"SegICirD=%.2f ;段号标注圈直径\n",sys.dim_map.fSegICirD);
	//
	fprintf(fp,"PartNoFrameStyle=%d ;编号框类型\n",sys.dim_map.PartNo.iPartNoFrameStyle);
	fprintf(fp,"FlPartNoFrameStyle=%d ;法兰编号框类型\n",sys.dim_map.PartNo.iFlPartNoFrameStyle);
	fprintf(fp,"StdInsertPlatePartNoFrameStyle=%d ;标准插板编号框类型\n",sys.dim_map.PartNo.iStdInsertPlatePartNoFrameStyle);
	fprintf(fp,"PartNoTextSize=%.2f ;构件编号文本大小\n",sys.dim_map.PartNo.fPartNoTextSize);
	fprintf(fp,"PartNoCirD=%.2f ;构件编号圈直径\n",sys.dim_map.PartNo.fPartNoCirD);
	fprintf(fp,"MainPoleNoOuterCirD=%.2f ;主材编号外圆直径\n",sys.dim_map.PartNo.fMainPoleNoOuterCirD);
	fprintf(fp,"MainPoleNoInterCirD=%.2f ;主材编号内圆直径\n",sys.dim_map.PartNo.fMainPoleNoInnerCirD);
	fprintf(fp,"MirMsg=%d ;主材编号标注对称方式\n",sys.dim_map.PartNo.iMirMsg);
	fprintf(fp,"PartNoMargin=%.2f ;构件编号与编号框之间的间隙\n",sys.dim_map.PartNo.fPartNoMargin);
	fprintf(fp,"DrawRefLine=%d ;是否绘制引出线\n",sys.dim_map.PartNo.bDrawRefLine);
	//绘图笔颜色
	fprintf(fp,"画笔颜色设置\n");
	fprintf(fp,"VisibleOutline=%d ;可见轮廓线颜色\n",sys.dim_map.crIndex.crOutLine);
	fprintf(fp,"DashLine=%d ;虚线颜色\n",sys.dim_map.crIndex.crDashLine);
	fprintf(fp,"AxisLine=%d ;轴线、对称线颜色\n",sys.dim_map.crIndex.crAxisLine);
	fprintf(fp,"SingleLine=%d ;单线图、总图颜色\n",sys.dim_map.crIndex.crSingleLine);
	fprintf(fp,"LsHoleLine=%d ;螺栓孔颜色\n",sys.dim_map.crIndex.crLs);
	fprintf(fp,"HuoQuLine=%d ;火曲线颜色\n",sys.dim_map.crIndex.crHuoQuLine);
	fprintf(fp,"DimLine=%d ;标注线颜色\n",sys.dim_map.crIndex.crDimSimTri);
	fprintf(fp,"DragLine=%d ;标注线颜色\n",sys.dim_map.crIndex.crDragLine);

	//图层
	fprintf(fp,"图层设置\n");
	//结构图中可见轮廓线图层
	if(LayerTable::VisibleProfileLayer.layerName.Length()>0)
		fprintf(fp,"VisibleProfileLayer.layerName=%s ;可见轮廓线图层名\n",(char*)LayerTable::VisibleProfileLayer.layerName);
	//结构图中不可见轮廓线图层
	if(LayerTable::UnvisibleProfileLayer.layerName.Length()>0)
		fprintf(fp,"UnvisibleProfileLayer.layerName=%s ;隐藏线图层名\n",(char*)LayerTable::UnvisibleProfileLayer.layerName);
	//受力材单线图、总图
	if(LayerTable::BriefLineLayer.layerName.Length()>0)
		fprintf(fp,"BriefLineLayer.layerName=%s ;受力材单线图层名\n",(char*)LayerTable::BriefLineLayer.layerName);
	//辅材单线图
	if(LayerTable::AuxLineLayer.layerName.Length()>0)
		fprintf(fp,"AuxLineLayer.layerName=%s ;辅材单线图层名\n",(char*)LayerTable::AuxLineLayer.layerName);
	//螺栓图符
	if(LayerTable::BoltSymbolLayer.layerName.Length()>0)
		fprintf(fp,"BoltSymbolLayer.layerName=%s ;螺栓图层名\n",(char*)LayerTable::BoltSymbolLayer.layerName);
	//相似形标注
	if(LayerTable::SimPolyLayer.layerName.Length()>0)
		fprintf(fp,"SimPolyLayer.layerName=%s ;相似形图层名\n",(char*)LayerTable::SimPolyLayer.layerName);
	//中心线、对称线、定位轴线
	if(LayerTable::AxisLineLayer.layerName.Length()>0)
		fprintf(fp,"AxisLineLayer.layerName=%s ;中心线图层名\n",(char*)LayerTable::AxisLineLayer.layerName);
	//角钢火曲、钢板火曲
	if(LayerTable::BendLineLayer.layerName.Length()>0)
		fprintf(fp,"BendLineLayer.layerName=%s ;制弯线图层名\n",(char*)LayerTable::BendLineLayer.layerName);
	//断开界线
	if(LayerTable::BreakLineLayer.layerName.Length()>0)
		fprintf(fp,"BreakLineLayer.layerName=%s ;界线、断开线图层名\n",(char*)LayerTable::BreakLineLayer.layerName);
	//螺栓线
	if(LayerTable::BoltLineLayer.layerName.Length()>0)
		fprintf(fp,"BoltLineLayer.layerName=%s ;螺栓线图层名\n",(char*)LayerTable::BoltLineLayer.layerName);
	//尺寸标注
	if(LayerTable::DimSizeLayer.layerName.Length()>0)
		fprintf(fp,"DimSizeLayer.layerName=%s ;尺寸标注图层名\n",(char*)LayerTable::DimSizeLayer.layerName);
	//文字标注
	if(LayerTable::DimTextLayer.layerName.Length()>0)
		fprintf(fp,"DimTextLayer.layerName=%s ;文字标注图层名\n",(char*)LayerTable::DimTextLayer.layerName);
	//除以上以外的所有图元所在图层
	if(LayerTable::CommonLayer.layerName.Length()>0)
		fprintf(fp,"CommonLayer.layerName=%s ;其余图层名\n",(char*)LayerTable::CommonLayer.layerName);
	//辅助火曲线
	if(LayerTable::AssistHuoQuLineLayer.layerName.Length()>0)
		fprintf(fp,"AssistHuoQuLineLayer.layerName=%s ;辅助火曲线图层名\n",(char*)LayerTable::AssistHuoQuLineLayer.layerName);
	//子角钢中心线
	if(LayerTable::SonJgAxisLineLayer.layerName.Length()>0)
		fprintf(fp,"SonJgAxisLineLayer.layerName=%s ;子角钢中心线图层名\n",(char*)LayerTable::SonJgAxisLineLayer.layerName);
	//钢板轮廓破损标识线
	if(LayerTable::DamagedSymbolLine.layerName.Length()>0)
		fprintf(fp,"DamagedSymbolLine.layerName=%s ;钢板轮廓破损标识线\n",(char*)LayerTable::DamagedSymbolLine.layerName);
#else
	fprintf(fp,"常规设置\n");
	fprintf(fp,"StruBoltSymbolStyle=%d ;结构图螺栓图符样式\n",sys.general_set.StruMapScale.iBoltSymbolStyle);
	fprintf(fp,"StruBoltSymbolFactor=%f ;结构图螺栓比例图符的比例因子\n",sys.general_set.StruMapScale.fBoltSymbolFactor);
	fprintf(fp,"StruBoltDiaFactor=%f ;结构图螺栓比例直径图符比例因子\n",sys.general_set.StruMapScale.fBoltDiaFactor);
	fprintf(fp,"PartBoltSymbolStyle=%d ;构件图螺栓图符样式\n",sys.general_set.PartMapScale.iBoltSymbolStyle);
	fprintf(fp,"PartBoltSymbolFactor=%f ;构件图螺栓比例图符的比例因子\n",sys.general_set.PartMapScale.fBoltSymbolFactor);
	fprintf(fp,"PartBoltDiaFactor=%f ;构件图螺栓比例直径图符比例因子\n",sys.general_set.PartMapScale.fBoltDiaFactor);
	fprintf(fp,"WeldBoltSymbolStyle=%d ;组焊图螺栓图符样式\n",sys.general_set.WeldMapScale.iBoltSymbolStyle);
	fprintf(fp,"WeldBoltSymbolFactor=%f ;组焊图螺栓比例图符的比例因子\n",sys.general_set.WeldMapScale.fBoltSymbolFactor);
	fprintf(fp,"WeldBoltDiaFactor=%f ;组焊图螺栓比例直径图符比例因子\n",sys.general_set.WeldMapScale.fBoltDiaFactor);
	fprintf(fp,"HoleAugmentInNC=%f ;孔径增大值\n",sys.general_set.fHoleAugmentInNC);
	fprintf(fp,"WeldLineSymbolHigh=%f ;焊缝线高度\n",sys.general_set.fWeldLineSymbolHigh);
	fprintf(fp,"WeldLineSymbolSpace=%f ;焊缝线间隙\n",sys.general_set.fWeldLineSymbolSpace);
	fprintf(fp,"DimPrecision=%d ;精确度\n",sys.general_set.iDimPrecision);
	fprintf(fp,"PartBOMStyle=%d ;构件明细表样式\n",sys.general_set.iPartBOMStyle);
	fprintf(fp,"SumMatBOMStyle=%d ;材料汇总表样式\n",sys.general_set.iSumMatBOMStyle);
	fprintf(fp,"DrawMainPoleJointLine=%d ;绘制分段标志线\n",sys.bDrawMainPoleJointLine);
	fprintf(fp,"JointLineOffset=%d ;分段标志线偏移量\n",sys.nJointLineOffset);
	fprintf(fp,"JointLineLen=%d ;分段标志线长度\n",sys.nJointLineLen);
		//构件工艺卡及其排列参数
	if(sys.general_set.bProcessCardTbl)
		fprintf(fp,"CreatePartCard=1;//是否生成工艺卡图框\n");
	else
		fprintf(fp,"CreatePartCard=0;//是否生成工艺卡图框\n");
	fprintf(fp,"bOneCardMultiPart=%d ;允许角钢卡一卡多件情况\n",sys.part_map.angle.bOneCardMultiPart);
	fprintf(fp,"bHasFrame=%d ;绘制装订线\n",sys.general_set.bHasFrame);
	fprintf(fp,"LineMapScale=%f ;单线图绘图比例\n",sys.general_set.LineMapScale);
	fprintf(fp,"ForceLineScale=%f ;司令图绘图比例\n",sys.general_set.ForceLineScale);
	fprintf(fp,"StruMapScale=%f ;结构图绘图比例\n",sys.general_set.StruMapScale.fRealToDraw);
	fprintf(fp,"PartMapScale=%f ;构件图绘图比例\n",sys.general_set.PartMapScale.fRealToDraw);
	fprintf(fp,"WeldMapScale=%f ;组焊图绘图比例\n",sys.general_set.WeldMapScale.fRealToDraw);
	fprintf(fp,"nColumnWidth=%d ;构件图列宽\n",sys.general_set.nColumnWidth);
	fprintf(fp,"RowHigh=%d ;构件图行宽\n",sys.general_set.nRowHigh);
	if(sys.general_set.bCardLayoutBaseRow)
		fprintf(fp,"CardLayoutBaseRow=1;//构件图以行为基准进行排列\n");
	else
		fprintf(fp,"CardLayoutBaseRow=0;//构件图以列为基准进行排列\n");
	fprintf(fp,"PartRowsN=%d ;构件图每列最多行数\n",sys.general_set.nPartRowsN);
	fprintf(fp,"JgCard=%s ;角钢工艺卡目录\n",sys.general_set.m_sJgCard);
	fprintf(fp,"SlotCard=%s ;槽钢工艺卡目录\n",sys.general_set.m_sSlotCard);
	fprintf(fp,"TubeCard=%s ;钢管工艺卡目录\n",sys.general_set.m_sTubeCard);
	fprintf(fp,"PlankCard=%s ;板工卡目录\n",sys.general_set.m_sPlankCard);
	fprintf(fp,"WeldPartGroupCard=%s ;焊接件工卡目录\n",sys.general_set.m_sWeldPartGroupCard);
	fprintf(fp,"ProcessPartStatCard=%s ;工艺件统计卡目录\n",sys.general_set.m_sProcessPartStatCard);
		//工装辅助钢板库文件目录
	fprintf(fp,"SetupAidedPlate=%s ;工装辅助钢板库文件目录\n",sys.general_set.m_sSetupAidedPlate);
		//出图模块路径设置
	fprintf(fp,"TMAMapArxPath=%s ;TMA出图模块路径\n",sys.general_set.m_sTMAMapArxPath);
	fprintf(fp,"LMAMapArxPath=%s ;LMA出图模块路径\n",sys.general_set.m_sLMAMapArxPath);
	fprintf(fp,"LDSMapArxPath=%s ;LDS出图模块路径\n",sys.general_set.m_sLDSMapArxPath);
	fprintf(fp,"TSAMapArxPath=%s ;TSA出图模块路径\n",sys.general_set.m_sTSAMapArxPath);
	//数字化移交属性设置
	fprintf(fp,"LCrossarmCr=%X ;左横担颜色\n",sys.general_set.lCrossarmCr);
	fprintf(fp,"RCrossarmCr=%X ;右横担颜色\n",sys.general_set.rCrossarmCr);
	//结构图设置
	fprintf(fp,"结构图设置\n");
	fprintf(fp,"MinWingWide=%.0f ;最小肢宽值\n",sys.stru_map.fMinWingWide);
	fprintf(fp,"MinWingThick=%.0f ;最小肢厚值\n",sys.stru_map.fMinPartThick);
	fprintf(fp,"DimSpecialLsSpace=%d ;特殊螺栓间距\n",sys.stru_map.bDimSpecialLsSpace);
	fprintf(fp,"DimSpecialJgZhunJu=%d ;标注角钢特殊准据\n",sys.stru_map.bDimSpecialJgZhun);
	fprintf(fp,"DimStruJgLen=%d ;标注角钢长度\n",sys.stru_map.bDimStruJgLen);
	fprintf(fp,"DimPartGuiGe=%d ;标注构件规格\n",sys.stru_map.bDimPartGuiGe);
	fprintf(fp,"DrawCutAngleAndWingInfo=%d ;绘制角钢切角肢信息\n",sys.stru_map.bDrawCutAngleAndWingInfo);
	fprintf(fp,"OnlyDrawHorizontalPlate=%d ;仅绘制当前视图中水平显示的钢板\n",sys.stru_map.bOnlyDrawHorizontalPlate);
	fprintf(fp,"UsePolyLineDrawEdge=%d ;使用多段线绘制角钢或钢板的轮廓边\n",sys.stru_map.bUsePolyLineDrawEdge);
	fprintf(fp,"PolyLineWidth=%.1f ;多段线宽度值\n",sys.stru_map.fPolyLineWidth);
	fprintf(fp,"PoleAxisLineStartOutSpreadLen=%.0f ;杆件心线始端延伸量\n",sys.stru_map.fPoleAxisLineStartOutSpreadLen);
	fprintf(fp,"PoleAxisLineEndOutSpreadLen=%.0f ;杆件心线始端延伸量\n",sys.stru_map.fPoleAxisLineEndOutSpreadLen);
	fprintf(fp,"ReadDrawingScript=%d ;读取附加缓存\n",sys.stru_map.bReadAdditionalBuffer);
	//结构图字高设置
	fprintf(fp,"PlateThickTextSize=%.2f ;板厚文本大小\n",sys.dim_map.fPlateThickTextSize);
	fprintf(fp,"SimTriTextSize=%.2f ;相似形文本大小\n",sys.dim_map.fSimPolyTextSize);
	fprintf(fp,"OtherTextSize=%.2f ;其余普通文本大小\n",sys.dim_map.fOtherTextSize);
	fprintf(fp,"JgOddmentTextSize=%.2f ;角钢正负头文本大小\n",sys.dim_map.fJgOddmentTextSize);
	fprintf(fp,"PartGuigeTextSize=%.2f ;构件规格文本大小\n",sys.dim_map.fPartGuigeTextSize);
	fprintf(fp,"SimTriScale=%.3f ;相似形缩放比例\n",sys.dim_map.fSimTriScale);
	fprintf(fp,"LsGuigeTextSize=%.2f ;螺栓规格信息文本大小\n",sys.dim_map.fLsGuigeTextSize);
	//组焊图绘图设置
	fprintf(fp,"组焊图绘图设置\n");
	fprintf(fp,"MinFontSize=%.2f ;组焊图中字体的最小高度\n",sys.part_group_map.fMinFontSize);
	fprintf(fp,"MinDrawThick=%.2f ;厚度最小绘制值\n",sys.part_group_map.fMinDrawThick);
	fprintf(fp,"KmJgGIncrement=%d ;靠模角钢心距增大值\n",sys.part_group_map.nKmJgGIncrement);
	fprintf(fp,"ModulatePartGroupLen=%d	;调整组焊图显示长度\n",sys.part_group_map.bModulatePartGroupLen);
	fprintf(fp,"PartGroupDistZoomCoef=%.2f	;组焊件缩放系数\n",sys.part_group_map.fPartGroupDistZoomCoef);
	fprintf(fp,"fPartGroupDistThreshold=%.2f ;组焊件缩放阈值\n",sys.part_group_map.fPartGroupDistThreshold);
	fprintf(fp,"bDrawWeldLineSymbol=%d;绘制焊缝线标记\n",sys.part_group_map.bDrawWeldLineSymbol);
	fprintf(fp,"iPlateLocateDrawStyle=%d;组焊图中绘制钢板对活点\n",sys.part_group_map.iPlateLocateDrawStyle);
	fprintf(fp,"iLocateSymboleHeight=%d;组焊图中钢板对活点高度\n",sys.part_group_map.iLocateSymboleHeight);
	fprintf(fp,"bDimEndOuterPlaneSpace=%d;杆件外端面到外端面距离\n",sys.part_group_map.bDimEndOuterPlaneSpace);
	fprintf(fp,"AutoDimPartNo=%d	;自动标注构件编号\n",sys.part_group_map.bAutoDimPartNo);
	fprintf(fp,"AutoMatchView=%d	;自动组合组焊件视图\n",sys.part_group_map.bAutoMatchView);
	fprintf(fp,"DimAbsoluteDist=%d	;标注定位点绝对尺寸\n",sys.part_group_map.bDimAbsoluteDist);
	fprintf(fp,"DimBeforeWeldLocatSpace=%d	;标注焊接前定位尺寸\n",sys.part_group_map.bDimBeforeWeldLocatSpace);
	fprintf(fp,"DimAfterWeldCheckLine=%d	;标注焊接后检测线\n",sys.part_group_map.bDimAfterWeldCheckLine);
	fprintf(fp,"AngularDimension=%d	;标注角度线\n",sys.part_group_map.bAngularDimension);
	fprintf(fp,"DimPlatePoleRayAndAngle=%d	;标注钢板连接的杆件射线以及角度\n",sys.part_group_map.bDimPlatePoleRayAndAngle);
	fprintf(fp,"SmartFilterRedundantPart=%d	;视图中自动筛选出与当前视图无关的多余构件跳过不显示\n",sys.part_group_map.bSmartFilterRedundantPart);
	fprintf(fp,"CreatePartGroupBOM=%d	;生成组焊件明细表\n",sys.part_group_map.bCreatePartGroupBOM);
	fprintf(fp,"WeldingFontNeedZoom=%d	;字体随动缩放\n",sys.welding.bZoomAdaptiveWithScale);
	fprintf(fp,"WeldingFontScale=%d	;字体基准缩放比例\n",sys.welding.wiScaleModel2Draw);
	//组焊件明细表
	fprintf(fp,"DispPartGroupNo=%d	;//组焊件编号\n",sys.part_group_map.part_table.bDispPartGroupNo);
	fprintf(fp,"DispPartGroupNum=%d	;//组焊件数量\n",sys.part_group_map.part_table.bDispPartGroupNum);
	fprintf(fp,"DispPartNo=%d	;//构件编号\n",sys.part_group_map.part_table.bDispPartNo);
	fprintf(fp,"DispPartNum=%d	;//构件数量\n",sys.part_group_map.part_table.bDispPartNum);
	fprintf(fp,"DispPartGuiGe=%d	;//构件规格\n",sys.part_group_map.part_table.bDispPartGuiGe);
	fprintf(fp,"DispPartLen=%d	;//构件长度\n",sys.part_group_map.part_table.bDispPartLen);
	fprintf(fp,"DispPartWeight=%d	;//构件重量\n",sys.part_group_map.part_table.bDispPartWeight);
	fprintf(fp,"DispNotes=%d	;//备注\n",sys.part_group_map.part_table.bDispNotes);
	fprintf(fp,"bDispInternal=%d	;//焊缝级别－内在\n",sys.part_group_map.part_table.bDispInternal);
	fprintf(fp,"bDispAppearance=%d	;//焊缝级别－外观\n",sys.part_group_map.part_table.bDispAppearance);
	fprintf(fp,"bZoomInProcessCard=%d	;//工艺卡内进行比例缩放\n",sys.part_group_map.part_table.bZoomInProcessCard);
	//构件图
	fprintf(fp,"ExportDeformedProfile=%d	;//构件图考虑制弯变形后的效应\n",sys.part_map.iExportDeformedProfile);
	fprintf(fp,"DimDeformedProfileText=%d ;考虑火曲变形时是否标注火曲变形相关文字\n",sys.part_map.bDimDeformedProfileText);
	fprintf(fp,"DimHuoquType=%d ;冷弯热弯\n",sys.part_map.bDimHuoquType);
	fprintf(fp,"DimPlaneAngle=%d ;标注火曲面之间的夹角\n",sys.part_map.bDimPlaneAngle);
	fprintf(fp,"DimAngleType=%d ;角度标注类型\n",sys.part_map.iDimAngleType);
	fprintf(fp,"PartMapStyle=%d ;构件图风格样式设置\n",sys.part_map.iPartMapStyle);
	fprintf(fp,"UseZhengFanQun=%d ;启用正反曲否则使用外内曲\n",sys.part_map.bUseZhengFanQun);
		//角钢构件图设置
	fprintf(fp,"角钢构件图设置\n");
	fprintf(fp,"ModulateLongJg=%d ;调整角钢长度\n",sys.part_map.angle.bModulateLongJg);
	fprintf(fp,"JgZoomSchema=%d ;//0.1:1绘制 1.使用构件图比例 2.长宽同比缩放 3.长宽分别缩放\n",sys.part_map.angle.iJgZoomSchema);
	fprintf(fp,"JgLenDrawMode=%d ;//0.向右延伸 1.中间挤压\n",sys.part_map.angle.ciJgLenDrawMode);
	fprintf(fp,"MaxExtendAngleLength=%d ;//最大限度延伸角钢绘制长度\n",sys.part_map.angle.bMaxExtendAngleLength);
	fprintf(fp,"LsDistThreshold=%.2f ;角钢长度自动调整螺栓间距阈值\n",sys.part_map.angle.fLsDistThreshold);
	fprintf(fp,"LsDistZoomCoef=%.2f ;//螺栓间距缩放系数\n",sys.part_map.angle.fLsDistZoomCoef);
	fprintf(fp,"OneCardMultiPart=%d ;//角钢允许一卡多件情况\n",sys.part_map.angle.bOneCardMultiPart);
	fprintf(fp,"JgGDimStyle=%d ;//0.始端标注 1.中间标注 2.不标注\n",sys.part_map.angle.iJgGDimStyle);
	fprintf(fp,"MaxBoltNumStartDimG=%d ;//集中在始端标注准距支持的最大螺栓数\n",sys.part_map.angle.nMaxBoltNumStartDimG);
	fprintf(fp,"LsSpaceDimStyle=%d ;//0.X轴方向	 1.Y轴方向  2.自动判断 3.不标注\n",sys.part_map.angle.iLsSpaceDimStyle);
	fprintf(fp,"MaxBoltNumAlongX=%d ;//沿X轴方向标注支持的最大螺栓个数\n",sys.part_map.angle.nMaxBoltNumAlongX);
	fprintf(fp,"DimCutAngle=%d ;//标注角钢切角\n",sys.part_map.angle.bDimCutAngle);
	fprintf(fp,"DimCutAngleMap=%d ;//标注角钢切角示意图\n",sys.part_map.angle.bDimCutAngleMap);
	fprintf(fp,"DimPushFlatMap=%d ;//标注压扁示意图\n",sys.part_map.angle.bDimPushFlatMap);
	fprintf(fp,"JgUseSimpleLsMap=%d ;//角钢使用简化螺栓图符\n",sys.part_map.angle.bJgUseSimpleLsMap);
	fprintf(fp,"DimLsAbsoluteDist=%d ;//标注螺栓绝对尺寸\n",sys.part_map.angle.bDimLsAbsoluteDist);
	fprintf(fp,"bMergeLsAbsoluteDist=%d ;//合并相邻等距螺栓绝对尺寸\n",sys.part_map.angle.bMergeLsAbsoluteDist);
	fprintf(fp,"DimRibPlatePartNo=%d ;//标注角钢肋板编号\n",sys.part_map.angle.bDimRibPlatePartNo);
	fprintf(fp,"DimRibPlateMaterial=%d ;//标注角钢肋板材质\n",sys.part_map.angle.bDimRibPlateMaterial);
	fprintf(fp,"bDimRibPlateSetUpPos=%d ;//标注角钢肋板安装位置\n",sys.part_map.angle.bDimRibPlateSetUpPos);
	fprintf(fp,"CutAngleDimType=%d ;//角钢切角标注类型\n",sys.part_map.angle.iCutAngleDimType);
	fprintf(fp,"DimKaiHe=%d ;//标注角钢开合角\n",sys.part_map.angle.bDimKaiHe);
	fprintf(fp,"DimKaiheAngleMap=%d ;//标注角钢开合角示意图\n",sys.part_map.angle.bDimKaiheAngleMap);
	fprintf(fp,"KaiHeJiaoThreshold=%.2f ;开合角标注阈值\n",sys.part_map.angle.fKaiHeJiaoThreshold);
	fprintf(fp,"fKaiHeJiaoMinDeita=%g ;开合角最小安全间隙值\n",sys.part_map.angle.fKaiHeJiaoMinDeita);
	fprintf(fp,"MatCharPosType=%d ;//材质字符标注位置\n",sys.general_set.iMatCharPosType);
	fprintf(fp,"DimKaiHeSumLen=%d ;//标注角钢开合区域总长度\n",sys.part_map.angle.bDimKaiheSumLen);
	fprintf(fp,"DimKaiHeAngle=%d ;//标注角钢开合角度数\n",sys.part_map.angle.bDimKaiheAngle);
	fprintf(fp,"DimKaiHeSegLen=%d ;//标注角钢开合区域分段长\n",sys.part_map.angle.bDimKaiheSegLen);
	fprintf(fp,"DimKaiHeScopeMap=%d ;//标注角钢开合区域分段长\n",sys.part_map.angle.bDimKaiheScopeMap);
	//钢板构件图
	fprintf(fp,"钢板构件图设置\n");
	fprintf(fp,"DimPlateEdgeLen=%d ;标连接板外轮廓\n",sys.part_map.plate.bDimPlateEdgeLen);
	fprintf(fp,"DrawInnerEdge=%d ;绘内侧轮廓边\n",sys.part_map.plate.bDrawInnerEdge);
	fprintf(fp,"InnerEdgeOffset=%f ;轮廓边偏移距\n",sys.part_map.plate.fInnerEdgeOffset);
	fprintf(fp,"DimPlankRayLs=%d ;标注连接板螺栓间隙\n",sys.part_map.plate.bDimPlateRayLs);
	fprintf(fp,"DimPlankBoltAngle=%d ;标注连接板螺栓角度\n",sys.part_map.plate.bDimPlateBoltAngle);
	fprintf(fp,"DimBoltEdgeSpace=%d ;标注连接板螺栓边距\n",sys.part_map.plate.bDimBoltEdgeSpace);
	fprintf(fp,"DimDiffRayLsSpace=%d ;标注相邻射线螺栓间距\n",sys.part_map.plate.bDimDiffRayLsSpace);
	fprintf(fp,"DimLsToHuoquLine=%d ;标注相邻射线螺栓间距\n",sys.part_map.plate.bDimLsToHuoquLine);
	fprintf(fp,"DimLsToWeldEdge=%d ;标注相邻射线螺栓间距\n",sys.part_map.plate.bDimLsToWeldEdge);
	fprintf(fp,"DimArcEdgeR=%d ;标注连接板圆弧边半径\n",sys.part_map.plate.bDimArcEdgeR);
	fprintf(fp,"DimArcAngle=%d ;标注连接板圆弧边角度\n",sys.part_map.plate.bDimArcAngle);
	fprintf(fp,"PlateUseSimpleLsMap=%d ;角钢使用简化螺栓图符\n",sys.part_map.plate.bPlateUseSimpleLsMap);
	fprintf(fp,"AdjustPlateUcs=%d ;调整钢板坐标系保证最长边沿水平方向\n",sys.part_map.plate.bAdjustPlateUcs);
	if(sys.part_map.plate.bDimCoord)
		fprintf(fp,"DimPlankCoord=1;//生成板螺栓、顶点坐标\n");
	else
		fprintf(fp,"DimPlankCoord=0;//生成板螺栓、顶点坐标\n");
	fprintf(fp,"ZoomDimTextSizeByWidth=%d ;根据钢板长宽中的较小值缩放钢板相关标注字体高度\n",sys.part_map.plate.bZoomDimTextSizeByWidth);
	fprintf(fp,"TextSizeZoomCoef=%f ;字体高度与宽度之间的缩放系数\n",sys.part_map.plate.fTextSizeZoomCoef);
	fprintf(fp,"DrawSlopeCutLine=%d ;焊缝处的坡口线\n",sys.part_map.plate.bDrawSlopeCutLine);
	fprintf(fp,"DrawDatumLine=%d ;钢板的基准线\n",sys.part_map.plate.bDrawDatumLine);
	fprintf(fp,"CutArcsType=%d :焊烧死角的切弧处理设置\n",sys.part_map.plate.iCutArcsType);
	fprintf(fp,"ZhiWanLineType=%d :制弯线位置的处理设置\n",sys.part_map.plate.iZhiWanLineType);
	fprintf(fp,"MartHuoquPos=%d :标记火曲位置\n",sys.part_map.plate.bMarkHuoquPos);
	fprintf(fp,"DrawHuoQuPrjMap=%d :绘制火曲示意图\n",sys.part_map.plate.bDrawHuoQuPrjMap);
	fprintf(fp,"DrawHuoQuMarkLine=%d :绘制火曲特征线\n",sys.part_map.plate.bDrawHuoQuMarkLine);
	fprintf(fp,"DisplaypPartInfoTbl=%d ;显示构件表设置\n",sys.part_map.plate.bDisplaypPartInfoTbl);
	fprintf(fp,"PartInfoTblColumnWidth=%d ;构件表的列宽设置\n",sys.part_map.plate.nPartInfoTblColumnWidth);
	fprintf(fp,"ShowPlateCfgInfoDlg=%d ;显示钢板明细对话\n",sys.part_map.plate.bShowPlateCfgInfoDlg);
	//钢管构件图设置
	fprintf(fp,"钢管构件图设置\n");
	fprintf(fp,"SlotLenProcessError=%f ;钢管开槽长度误差值\n",sys.part_map.tube.fSlotLenProcessError);
	fprintf(fp,"SlotWidthProcessError=%f ;钢管开槽宽度误差值\n",sys.part_map.tube.fSlotWidthProcessError);
	fprintf(fp,"ReelTubeThresholdD=%f ;钢板卷管直径阈值\n",sys.part_map.tube.fReelTubeThresholdD);

	//标注设置
	fprintf(fp,"标注设置\n");
	fprintf(fp,"FontName=%s	;字体名称\n",sys.dim_map.sFontName);
	fprintf(fp,"ArrowType=%s ;尺寸标注箭头类型\n",sys.dim_map.sArrowType);
	fprintf(fp,"DimGap=%.2f ;文字与标注线间隙值\n",sys.dim_map.fDimGap);	
	fprintf(fp,"TblHeaderTextSize=%.2f ;表头文本大小\n",sys.dim_map.fHeaderTextSize);
	fprintf(fp,"TblContentTextSize=%.2f ;表格内容文本大小\n",sys.dim_map.fNumericTextSize);
	fprintf(fp,"TblCommonTextSize=%.2f ;表格中副标题文本大小\n",sys.dim_map.fCommonTextSize);
	fprintf(fp,"DimArrowSize=%.2f ;尺寸标注箭头大小\n",sys.dim_map.fDimArrowSize);
	fprintf(fp,"DimTextSize=%.2f ;长度尺寸标注文本大小\n",sys.dim_map.fDimTextSize);
	fprintf(fp,"DimAngleTextSize=%.2f ;角度尺寸标注文本大小\n",sys.dim_map.fDimAngleTextSize);
	//剖视图符
	fprintf(fp,"CutawayViewSymbolLen=%.2f ;剖视图符长度\n",sys.dim_map.fCutawayViewSymbolLen);
	fprintf(fp,"CutawayViewSymbolTextSize=%.2f ;剖视图符字体高度\n",sys.dim_map.fCutawayViewSymbolTextSize);
	fprintf(fp,"CutawayViewPolyLineWidth=%.2f ;剖视图符多段线宽度\n",sys.dim_map.fCutawayViewPolyLineWidth);
	//段号标注
	fprintf(fp,"SegITextSize=%.2f ;段号标注文本大小\n",sys.dim_map.fSegITextSize);
	fprintf(fp,"SegICirD=%.2f ;段号标注圈直径\n",sys.dim_map.fSegICirD);
	//
	fprintf(fp,"PartNoFrameStyle=%d ;编号框类型\n",sys.dim_map.PartNo.iPartNoFrameStyle);
	fprintf(fp,"FlPartNoFrameStyle=%d ;法兰编号框类型\n",sys.dim_map.PartNo.iFlPartNoFrameStyle);
	fprintf(fp,"StdInsertPlatePartNoFrameStyle=%d ;标准插板编号框类型\n",sys.dim_map.PartNo.iStdInsertPlatePartNoFrameStyle);
	fprintf(fp,"PartNoTextSize=%.2f ;构件编号文本大小\n",sys.dim_map.PartNo.fPartNoTextSize);
	fprintf(fp,"PartNoCirD=%.2f ;构件编号圈直径\n",sys.dim_map.PartNo.fPartNoCirD);
	fprintf(fp,"MainPoleNoOuterCirD=%.2f ;主材编号外圆直径\n",sys.dim_map.PartNo.fMainPoleNoOuterCirD);
	fprintf(fp,"MainPoleNoInterCirD=%.2f ;主材编号内圆直径\n",sys.dim_map.PartNo.fMainPoleNoInnerCirD);
	fprintf(fp,"MirMsg=%d ;主材编号标注对称方式\n",sys.dim_map.PartNo.iMirMsg);
	fprintf(fp,"PartNoMargin=%.2f ;构件编号与编号框之间的间隙\n",sys.dim_map.PartNo.fPartNoMargin);
	fprintf(fp,"DrawRefLine=%d ;是否绘制引出线\n",sys.dim_map.PartNo.bDrawRefLine);
	//绘图笔颜色
	fprintf(fp,"画笔颜色设置\n");
	fprintf(fp,"VisibleOutline=%d ;可见轮廓线颜色\n",sys.dim_map.crIndex.crOutLine);
	fprintf(fp,"DashLine=%d ;虚线颜色\n",sys.dim_map.crIndex.crDashLine);
	fprintf(fp,"AxisLine=%d ;轴线、对称线颜色\n",sys.dim_map.crIndex.crAxisLine);
	fprintf(fp,"SingleLine=%d ;单线图、总图颜色\n",sys.dim_map.crIndex.crSingleLine);
	fprintf(fp,"LsHoleLine=%d ;螺栓孔颜色\n",sys.dim_map.crIndex.crLs);
	fprintf(fp,"HuoQuLine=%d ;火曲线颜色\n",sys.dim_map.crIndex.crHuoQuLine);
	fprintf(fp,"DimLine=%d ;标注线颜色\n",sys.dim_map.crIndex.crDimSimTri);
	fprintf(fp,"DragLine=%d ;标注线颜色\n",sys.dim_map.crIndex.crDragLine);
	
	//图层
	fprintf(fp,"图层设置\n");
	//结构图中可见轮廓线图层
	if(LayerTable::VisibleProfileLayer.layerName.GetLength()>0)
		fprintf(fp,"VisibleProfileLayer.layerName=%s ;可见轮廓线图层名\n",(char*)LayerTable::VisibleProfileLayer.layerName);
	//结构图中不可见轮廓线图层
	if(LayerTable::UnvisibleProfileLayer.layerName.GetLength()>0)
		fprintf(fp,"UnvisibleProfileLayer.layerName=%s ;隐藏线图层名\n",(char*)LayerTable::UnvisibleProfileLayer.layerName);
	//受力材单线图、总图
	if(LayerTable::BriefLineLayer.layerName.GetLength()>0)
		fprintf(fp,"BriefLineLayer.layerName=%s ;受力材单线图层名\n",(char*)LayerTable::BriefLineLayer.layerName);
	//辅材单线图
	if(LayerTable::AuxLineLayer.layerName.GetLength()>0)
		fprintf(fp,"AuxLineLayer.layerName=%s ;辅材单线图层名\n",(char*)LayerTable::AuxLineLayer.layerName);
	//螺栓图符
	if(LayerTable::BoltSymbolLayer.layerName.GetLength()>0)
		fprintf(fp,"BoltSymbolLayer.layerName=%s ;螺栓图层名\n",(char*)LayerTable::BoltSymbolLayer.layerName);
	//相似形标注
	if(LayerTable::SimPolyLayer.layerName.GetLength()>0)
		fprintf(fp,"SimPolyLayer.layerName=%s ;相似形图层名\n",(char*)LayerTable::SimPolyLayer.layerName);
	//中心线、对称线、定位轴线
	if(LayerTable::AxisLineLayer.layerName.GetLength()>0)
		fprintf(fp,"AxisLineLayer.layerName=%s ;中心线图层名\n",(char*)LayerTable::AxisLineLayer.layerName);
	//角钢火曲、钢板火曲
	if(LayerTable::BendLineLayer.layerName.GetLength()>0)
		fprintf(fp,"BendLineLayer.layerName=%s ;制弯线图层名\n",(char*)LayerTable::BendLineLayer.layerName);
	//断开界线
	if(LayerTable::BreakLineLayer.layerName.GetLength()>0)
		fprintf(fp,"BreakLineLayer.layerName=%s ;界线、断开线图层名\n",(char*)LayerTable::BreakLineLayer.layerName);
	//螺栓线
	if(LayerTable::BoltLineLayer.layerName.GetLength()>0)
		fprintf(fp,"BoltLineLayer.layerName=%s ;螺栓线图层名\n",(char*)LayerTable::BoltLineLayer.layerName);
	//尺寸标注
	if(LayerTable::DimSizeLayer.layerName.GetLength()>0)
		fprintf(fp,"DimSizeLayer.layerName=%s ;尺寸标注图层名\n",(char*)LayerTable::DimSizeLayer.layerName);
	//文字标注
	if(LayerTable::DimTextLayer.layerName.GetLength()>0)
		fprintf(fp,"DimTextLayer.layerName=%s ;文字标注图层名\n",(char*)LayerTable::DimTextLayer.layerName);
	//除以上以外的所有图元所在图层
	if(LayerTable::CommonLayer.layerName.GetLength()>0)
		fprintf(fp,"CommonLayer.layerName=%s ;其余图层名\n",(char*)LayerTable::CommonLayer.layerName);
	//辅助火曲线
	if(LayerTable::AssistHuoQuLineLayer.layerName.GetLength()>0)
		fprintf(fp,"AssistHuoQuLineLayer.layerName=%s ;辅助火曲线图层名\n",(char*)LayerTable::AssistHuoQuLineLayer.layerName);
	//子角钢中心线
	if(LayerTable::SonJgAxisLineLayer.layerName.GetLength()>0)
		fprintf(fp,"SonJgAxisLineLayer.layerName=%s ;子角钢中心线图层名\n",(char*)LayerTable::SonJgAxisLineLayer.layerName);
	//钢板轮廓破损标识线
	if(LayerTable::DamagedSymbolLine.layerName.GetLength()>0)
		fprintf(fp,"DamagedSymbolLine.layerName=%s ;钢板轮廓破损标识线\n",(char*)LayerTable::DamagedSymbolLine.layerName);
#endif
	//图层哈希表
	fprintf(fp,"图层哈希表\n");
	for(TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
	{
		fprintf(fp,"LayerKey=%s;图层名;线型名;线宽;颜色\n",LayerTable::layerHashList.GetCursorKey());
		fprintf(fp,"%s;",(char*)pLayer->layerName);
		fprintf(fp,"%s;",(char*)pLayer->lineTypeName);
		fprintf(fp,"%d;",pLayer->lineWeight);
		fprintf(fp,"%d\n",pLayer->colorIndex);
	}
	fprintf(fp,"出图特殊设置\n");
	for(TEXT_PROP* pTextProp = sys.fonts.hashListProp.GetFirst();pTextProp;pTextProp = sys.fonts.hashListProp.GetNext())
	{
		long key = sys.fonts.hashListProp.GetKey(pTextProp);
		if(key==0)
			continue;
		fprintf(fp,"SpecialSetKey=%d;字高;宽高比;字体\n",key);
		fprintf(fp,"%.2f;",pTextProp->fHigh);
		fprintf(fp,"%.2f;",pTextProp->fWtoH);
		fprintf(fp,"%s\n",pTextProp->sFont);
	}
	fprintf(fp,"图框列表\n");
	for(MAP_FRAME_SIZE* pMapFrame=sys.m_hashMapFrameList.GetFirst();pMapFrame;pMapFrame=sys.m_hashMapFrameList.GetNext())
	{
		fprintf(fp,"MapFrameSize=%s;\n图框类型;名称;宽度;长度;左侧页边间隙;右侧页边间隙;上侧页边间隙;下侧页边间隙;图框右下侧固定长度;图框右下侧固定宽度\n",sys.m_hashMapFrameList.GetCursorKey());
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
	//更新注册表内容 wht 11-06-13
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
	//常规设置
		//绘图比例
	DrawSetSheet.m_StruPage.m_nLineMapScale = sys.general_set.LineMapScale;
	DrawSetSheet.m_StruPage.ForceLineScale = sys.general_set.ForceLineScale;
	DrawSetSheet.m_StruPage.m_nStruMapScale	= sys.general_set.StruMapScale;
	DrawSetSheet.m_StruPage.m_nPartMapScale = sys.general_set.PartMapScale;
		//构件火曲参数
	DrawSetSheet.m_StruPage.m_bExportDeformedProfile = sys.part_map.bExportDeformedProfile;
	DrawSetSheet.m_StruPage.m_bDimHuoquType	= sys.part_map.bDimHuoquType;
	DrawSetSheet.m_StruPage.m_bDimPlaneAngle = sys.part_map.bDimPlaneAngle;
		//螺栓孔处理及其他
	DrawSetSheet.m_StruPage.m_iLsApertureStyle = sys.general_set.iLsApertureStyle;
	DrawSetSheet.m_StruPage.m_fHoleAugment = sys.general_set.fHoleAugmentInNC;
		//构件工艺卡及其排列参数
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
		//结构图绘图设置
	DrawSetSheet.m_StruPage.m_fMinWingWide	 = sys.stru_map.fMinWingWide;
	DrawSetSheet.m_StruPage.m_fMinWingThick	 = sys.stru_map.fMinPartThick;
	DrawSetSheet.m_StruPage.m_bDimSpecialLsSpace=sys.stru_map.bDimSpecialLsSpace;
	DrawSetSheet.m_StruPage.m_bDimSpecialJgZhunJu=sys.stru_map.bDimSpecialJgZhun;
	DrawSetSheet.m_StruPage.m_bDimStruJgLen=sys.stru_map.bDimStruJgLen;
	//组焊图绘图设置
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
	//构件图绘图设置
	DrawSetSheet.m_PartPage.m_iPartMapType = sys.part_map.iPartMapStyle;
		//角钢构件图绘图设置
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
		//钢板构件图绘图设置
	DrawSetSheet.m_PartPage.m_bDimPlateOutline  = sys.part_map.plate.bDimPlateEdgeLen;
	DrawSetSheet.m_PartPage.m_bDimPlateRayLs	= sys.part_map.plate.bDimPlateRayLs;
	DrawSetSheet.m_PartPage.m_bDimPlateBoltAngle= sys.part_map.plate.bDimPlateBoltAngle;
	DrawSetSheet.m_PartPage.m_bDimCoord			= sys.part_map.plate.bDimCoord;
	DrawSetSheet.m_PartPage.m_bUseZhengFanQun	= sys.part_map.plate.bUseZhengFanQun;
	DrawSetSheet.m_PartPage.m_bPlateUseSimpleLsMap = sys.part_map.plate.bPlateUseSimpleLsMap;
	//工程图纸标注参数设置
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
	//绘图笔颜色设置
	DrawSetSheet.m_DrawPenPage.m_iVisibleOutline= sys.dim_map.crIndex.crOutLine;//外轮廓线
	DrawSetSheet.m_DrawPenPage.m_iSingleLine	= sys.dim_map.crIndex.crSingleLine;
	DrawSetSheet.m_DrawPenPage.m_iAxisLine		= sys.dim_map.crIndex.crAxisLine;
	DrawSetSheet.m_DrawPenPage.m_iHuoQuLine		= sys.dim_map.crIndex.crHuoQuLine;
	DrawSetSheet.m_DrawPenPage.m_iDashLine	= sys.dim_map.crIndex.crDashLine;
	DrawSetSheet.m_DrawPenPage.m_iLsHole		= sys.dim_map.crIndex.crLs;
	DrawSetSheet.m_DrawPenPage.m_iDimSimTri		= sys.dim_map.crIndex.crDimSimTri;
	DrawSetSheet.m_DrawPenPage.m_iDragLine		= sys.dim_map.crIndex.crDragLine;
	if(DrawSetSheet.DoModal()==IDOK)
	{
		//常规设置
			//绘图比例
		sys.general_set.LineMapScale = DrawSetSheet.m_StruPage.m_nLineMapScale;
		sys.general_set.ForceLineScale = DrawSetSheet.m_StruPage.ForceLineScale;
		sys.general_set.StruMapScale = DrawSetSheet.m_StruPage.m_nStruMapScale;
		sys.general_set.PartMapScale = DrawSetSheet.m_StruPage.m_nPartMapScale;
			//构件火曲参数
		sys.part_map.bExportDeformedProfile=DrawSetSheet.m_StruPage.m_bExportDeformedProfile;
		sys.part_map.bDimHuoquType		= DrawSetSheet.m_StruPage.m_bDimHuoquType;
		sys.part_map.bDimPlaneAngle	= DrawSetSheet.m_StruPage.m_bDimPlaneAngle;
			//螺栓孔处理及其他
		sys.general_set.iLsApertureStyle = DrawSetSheet.m_StruPage.m_iLsApertureStyle;
		sys.general_set.fHoleAugmentInNC = DrawSetSheet.m_StruPage.m_fHoleAugment;
			//构件工艺卡及其排列参数
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
			//结构图绘图设置
		sys.stru_map.fMinWingWide = DrawSetSheet.m_StruPage.m_fMinWingWide;
		sys.stru_map.fMinPartThick = DrawSetSheet.m_StruPage.m_fMinWingThick;
		sys.stru_map.bDimSpecialLsSpace=DrawSetSheet.m_StruPage.m_bDimSpecialLsSpace;
		sys.stru_map.bDimSpecialJgZhun=DrawSetSheet.m_StruPage.m_bDimSpecialJgZhunJu;
		sys.stru_map.bDimStruJgLen=DrawSetSheet.m_StruPage.m_bDimStruJgLen;
		//组焊图绘图设置
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
		//构件图绘图设置
		sys.part_map.iPartMapStyle = DrawSetSheet.m_PartPage.m_iPartMapType;	//构件图类型 0.普通 1.浙江盛达
			//角钢构件图绘图设置
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
			//钢板构件图绘图设置
		sys.part_map.plate.bDimPlateEdgeLen	= DrawSetSheet.m_PartPage.m_bDimPlateOutline;
		sys.part_map.plate.bDimPlateRayLs	= DrawSetSheet.m_PartPage.m_bDimPlateRayLs;
		sys.part_map.plate.bDimPlateBoltAngle= DrawSetSheet.m_PartPage.m_bDimPlateBoltAngle;
		sys.part_map.plate.bDimCoord			= DrawSetSheet.m_PartPage.m_bDimCoord;
		sys.part_map.plate.bUseZhengFanQun	= DrawSetSheet.m_PartPage.m_bUseZhengFanQun;
		sys.part_map.plate.bPlateUseSimpleLsMap	= DrawSetSheet.m_PartPage.m_bPlateUseSimpleLsMap;
		//工程图纸标注参数设置
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
		//绘图笔颜色设置
		sys.dim_map.crIndex.crOutLine	= DrawSetSheet.m_DrawPenPage.m_iVisibleOutline;//外轮廓线
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
		"角钢规格文件(*.jgt)|*.jgt||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		JgGuiGeSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import angle spec library file!");
#else
		AfxMessageBox("成功导入角钢规格库文件!");
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
		"钢管规格文件(*.pgt)|*.pgt||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		TubeGuiGeSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import tube spec library file!");
#else
		AfxMessageBox("成功导入钢管规格库文件!");
#endif
	}
}

//导入槽型插板库
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
		"槽型插板库文件(*.cipl)|*.cipl||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		CInsertPlateSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import slot inserting plate library file!");
#else
		AfxMessageBox("成功导入槽型插板库文件!");
#endif
	}
}

//导入U型插板库
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
		"U型插板库文件(*.uipl)|*.uipl||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		UInsertPlateSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import U inserting plate library file!");
#else
		AfxMessageBox("成功导入U型插板库文件!");
#endif
	}
}

//导入X型插板库
void XInsertPlateLibImport()
{
	CString sFileName;
	CFileDialog dlg(TRUE,"xipl","Tower",
#ifdef AFX_TARG_ENU_ENGLISH
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"X insert plate library file(*.xipl)|*.xipl||");
#else
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"十字型插板库文件(*.xipl)|*.xipl||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		XInsertPlateSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import X inserting plate library file spec library file!");
#else
		AfxMessageBox("成功导入十字型插板库文件!");
#endif
	}
}

//导入平焊法兰库
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
		"平焊法兰库文件(*.flp)|*.flp||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		FlPSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import plane weld flange library file!");
#else
		AfxMessageBox("成功导入平焊法兰库文件!");
#endif
	}
}

//导入对焊法兰库
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
		"对焊法兰库文件(*.fld)|*.fld||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		FlDSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import relative weld flange library file!");
#else
		AfxMessageBox("成功导入对焊法兰库文件!");
#endif
	}
}
//导入刚性法兰库
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
		"刚性法兰库文件(*.flg)|*.flg||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		FlGSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import rigid flange library file!");
#else
		AfxMessageBox("成功导入刚性法兰库文件!");
#endif
	}
}
//导入柔性法兰库
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
		"柔性法兰库文件(*.flr)|*.flr||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		FlRSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import flexible flange library file!");
#else
		AfxMessageBox("成功导入柔性法兰库文件!");
#endif
	}
}

//对接法兰级别定义文件
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
		"法兰对接级别定义文件(*.fll)|*.fll||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		FlCoupleLevelSerialize(sFileName,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Success to import file flange couple level defines!");
#else
		AfxMessageBox("成功导入对接法兰级别定义文件!");
#endif
	}

}

void SystemSetting()
{
	CSystemSettingDlg sysSettingDlg;
	sysSettingDlg.DoModal();
}