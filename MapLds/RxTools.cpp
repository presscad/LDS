//<LOCALE_TRANSLATE/>
#include "stdafx.h"
#include "stdarx.h"
#include "adesk.h"
#include "rxdefs.h"
#include "dbxrecrd.h"
#include "Tower.h"
#include "RxTools.h"
#include "winreg.h"
#include "SysPara.h"
#include "TblDef.h"
#include "DragEntSet.h"
#include "DrawTaMap.h"
#include "dbsol3d.h"
#include "dbregion.h"
#include "DrawUnit.h"
#include "PlankDrawing.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//ACAD RGB��ACI��Ӧ���� ��ʱʹ������ acdbGetRGB ������
typedef struct tagAcadRGB
{
	BYTE R,G,B;
}ACAD_RGB;
static ACAD_RGB AcadRGB[256]=
{
	0,0,0,
	255,0,0,
	255,255,0,
	0,255,0,
	0,255,255,
	0,0,255,
	255,0,255,
	255,255,255,
	128,128,128,
	192,192,192,
	255,0,0,
	255,127,127,
	165,0,0,
	165,82,82,
	127,0,0,
	127,63,63,
	76,0,0,
	76,38,38,
	38,0,0,
	38,19,19,
	255,63,0,
	255,159,127,
	165,41,0,
	165,103,82,
	127,31,0,
	127,79,63,
	76,19,0,
	76,47,38,
	38,9,0,
	38,23,19,
	255,127,0,
	255,191,127,
	165,82,0,
	165,124,82,
	127,63,0,
	127,95,63,
	76,38,0,
	76,57,38,
	38,19,0,
	38,28,19,
	255,191,0,
	255,223,127,
	165,124,0,
	165,145,82,
	127,95,0,
	127,111,63,
	76,57,0,
	76,66,38,
	38,28,0,
	38,33,19,
	255,255,0,
	255,255,127,
	165,165,0,
	165,165,82,
	127,127,0,
	127,127,63,
	76,76,0,
	76,76,38,
	38,38,0,
	38,38,19,
	191,255,0,
	223,255,127,
	124,165,0,
	145,165,82,
	95,127,0,
	111,127,63,
	57,76,0,
	66,76,38,
	28,38,0,
	33,38,19,
	127,255,0,
	191,255,127,
	82,165,0,
	124,165,82,
	63,127,0,
	95,127,63,
	38,76,0,
	57,76,38,
	19,38,0,
	28,38,19,
	63,255,0,
	159,255,127,
	41,165,0,
	103,165,82,
	31,127,0,
	79,127,63,
	19,76,0,
	47,76,38,
	9,38,0,
	23,38,19,
	0,255,0,
	127,255,127,
	0,165,0,
	82,165,82,
	0,127,0,
	63,127,63,
	0,76,0,
	38,76,38,
	0,38,0,
	19,38,19,
	0,255,63,
	127,255,159,
	0,165,41,
	82,165,103,
	0,127,31,
	63,127,79,
	0,76,19,
	38,76,47,
	0,38,9,
	19,38,23,
	0,255,127,
	127,255,191,
	0,165,82,
	82,165,124,
	0,127,63,
	63,127,95,
	0,76,38,
	38,76,57,
	0,38,19,
	19,38,28,
	0,255,191,
	127,255,223,
	0,165,124,
	82,165,145,
	0,127,95,
	63,127,111,
	0,76,57,
	38,76,66,
	0,38,28,
	19,38,33,
	0,255,255,
	127,255,255,
	0,165,165,
	82,165,165,
	0,127,127,
	63,127,127,
	0,76,76,
	38,76,76,
	0,38,38,
	19,38,38,
	0,191,255,
	127,223,255,
	0,124,165,
	82,145,165,
	0,95,127,
	63,111,127,
	0,57,76,
	38,66,76,
	0,28,38,
	19,33,38,
	0,127,255,
	127,191,255,
	0,82,165,
	82,124,165,
	0,63,127,
	63,95,127,
	0,38,76,
	38,57,76,
	0,19,38,
	19,28,38,
	0,63,255,
	127,159,255,
	0,41,165,
	82,103,165,
	0,31,127,
	63,79,127,
	0,19,76,
	38,47,76,
	0,9,38,
	19,23,38,
	0,0,255,
	127,127,255,
	0,0,165,
	82,82,165,
	0,0,127,
	63,63,127,
	0,0,76,
	38,38,76,
	0,0,38,
	19,19,38,
	63,0,255,
	159,127,255,
	41,0,165,
	103,82,165,
	31,0,127,
	79,63,127,
	19,0,76,
	47,38,76,
	9,0,38,
	23,19,38,
	127,0,255,
	191,127,255,
	82,0,165,
	124,82,165,
	63,0,127,
	95,63,127,
	38,0,76,
	57,38,76,
	19,0,38,
	28,19,38,
	191,0,255,
	223,127,255,
	124,0,165,
	145,82,165,
	95,0,127,
	111,63,127,
	57,0,76,
	66,38,76,
	28,0,38,
	33,19,38,
	255,0,255,
	255,127,255,
	165,0,165,
	165,82,165,
	127,0,127,
	127,63,127,
	76,0,76,
	6,38,76,
	38,0,38,
	38,19,38,
	255,0,191,
	255,127,223,
	165,0,124,
	165,82,145,
	127,0,95,
	127,63,111,
	76,0,57,
	76,38,66,
	38,0,28,
	38,19,33,
	255,0,127,
	255,127,191,
	165,0,82,
	165,82,124,
	127,0,63,
	127,63,95,
	76,0,38,
	76,38,57,
	38,0,19,
	38,19,28,
	255,0,63,
	255,127,159,
	165,0,41,
	165,82,103,
	127,0,31,
	127,63,79,
	76,0,19,
	76,38,47,
	38,0,9,
	38,19,23,
	0,0,0,
	51,51,51,
	91,91,91,
	132,132,132,
	173,173,173,
	214,214,214,
};

//��RGB�õ�cad��ɫ����ֵ 
int GetNearestACI(COLORREF color)
{
	long min_dist = 2147483647L;
	long dist = 0;
	int min_index = 0;
	long red=GetRValue(color);
	long green=GetGValue(color);
	long blue=GetBValue(color);
	for(int i=1;i<256;i++)
	{
		/*long aci_rgb,r,g,b;
		aci_rgb=acdbGetRGB(i);
		r=GetRValue(aci_rgb);
		g=GetGValue(aci_rgb);
		b=GetBValue(aci_rgb);
		dist=abs(r-red)+abs(g-green)+abs(b-blue);*/
		dist=abs(AcadRGB[i].R-red)+abs(AcadRGB[i].G-green)+abs(AcadRGB[i].B-blue);
		if(dist<min_dist) 
		{
			min_index = i;
			min_dist = dist;
		}
	}
	return min_index;
}
//��CAD����ɫ�õ�RGB
COLORREF GetColorFromIndex(int color_index)
{
	/*if(colorIndex < 0 || colorIndex > 255)
	{
		ads_alert("�������ɫ�Ų���0~255֮��!");
		return 0;
	}
	BYTE R, G, B;
#ifdef ARX_2002_dll
	R = lpszRGBData[colorIndex*3+0];
	G = lpszRGBData[colorIndex*3+1];
	B = lpszRGBData[colorIndex*3+2];
#else
	long zhi = acdbGetRGB(colorIndex);
	WORD LOW = LOWORD(zhi);
	WORD HIG = HIWORD(zhi);
	R = LOBYTE(LOW);
	G = HIBYTE(LOW);
	B = LOBYTE(HIG);
#endif
	return RGB(R,G,B);*/
	if(color_index<1||color_index>255)
		return RGB(255,0,0);
	else
		return RGB(AcadRGB[color_index].R,AcadRGB[color_index].G,AcadRGB[color_index].B);
}
void OpenDoc(void *pData)
{
	AcApDocument* pDoc = acDocManager->mdiActiveDocument();
	if (acDocManager->isApplicationContext())
	{
#ifdef _ARX_2007
		acDocManager->appContextOpenDocument(_bstr_t((char*)pData));
#else
		acDocManager->appContextOpenDocument((char*)pData);
#endif
		//���ｫ�ĵ��л���ԭ�����ĵ����Ա���ĵ�����ִ�д���
		acDocManager->activateDocument(pDoc);
	}
	else
#ifdef _ARX_2007
		acutPrintf(_bstr_t("\nERROR To Open Doc!\n"));
#else
		acutPrintf("\nERROR To Open Doc!\n");
#endif
}
AcApDocument* OpenDwgFile(const char* sDwgFile)
{
	if (strlen(sDwgFile) <= 0)
		return FALSE;
	//���ļ�
#ifdef _ARX_2007
	acDocManager->executeInApplicationContext(OpenDoc, (void*)(ACHAR*)_bstr_t(sDwgFile));
#else
	acDocManager->executeInApplicationContext(OpenDoc, (char*)CXhChar500(sDwgFile));
#endif
	//��ȡ�����ĵ�
	CXhChar200 file_path;
	AcApDocument *pDoc = NULL;
	AcApDocumentIterator *pIter = acDocManager->newAcApDocumentIterator();
	for (; !pIter->done(); pIter->step())
	{
		pDoc = pIter->document();
#ifdef _ARX_2007
		file_path.Copy(_bstr_t(pDoc->fileName()));
#else
		file_path.Copy(pDoc->fileName());
#endif
		if (strstr(file_path, sDwgFile))
			break;
	}
	if (strstr(file_path, sDwgFile))
		return pDoc;
	return NULL;
}
AcDbDatabase *GetCurDwg()
{
	return acdbHostApplicationServices()->workingDatabase();
}
CNewDatabaseLife::CNewDatabaseLife()
{
	pOldDb=acdbHostApplicationServices()->workingDatabase();
	newDb.setLtscale(20);
	newDb.setExtmin(AcGePoint3d(-300,600,0));
	newDb.setExtmax(AcGePoint3d(300,-600,0));
#ifdef _ARX_2007
	newDb.setWorldUcsBaseOrigin(AcGePoint3d(0,0,0),AcDb::kNonOrthoView);
#else
	newDb.setWorldUcsBaseOrigin(AcGePoint3d(0,0,0),AcDb::OrthographicView::kNonOrthoView);
#endif
	newDb.disableUndoRecording(Adesk::kTrue);
	acdbHostApplicationServices()->setWorkingDatabase(&newDb);
}
CNewDatabaseLife::~CNewDatabaseLife()
{
	newDb.closeInput();
	acdbHostApplicationServices()->setWorkingDatabase(pOldDb);
}
BOOL CNewDatabaseLife::SaveAs(const char* sFullPath)
{
#ifdef _ARX_2007
	//if (newDb.saveAs("D:\\body_L.dwg",false,AcDb::AcDbDwgVersion::kDHL_1015)!=Acad::eOk)//sFullPath))
	if(newDb.saveAs(_bstr_t(sFullPath),false,AcDb::kDHL_1015)!=Acad::eOk)
#else
	if(newDb.saveAs(sFullPath)!=Acad::eOk)
#endif
		return FALSE;
	else
		return TRUE;
}
void GetSlotDBFilePath(char* slotguige_path)
{
	char path[MAX_PATH]="";
	char sSubKey[MAX_PATH]="";
	DWORD dwDataType,dwLength=MAX_PATH;
#ifdef __TMA_FILE_
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(sSubKey,"Software\\Xerofox\\TMA-EN\\Settings");
#else
	strcpy(sSubKey,"Software\\Xerofox\\TMA\\Settings");
#endif
#elif defined(__LMA_FILE_)
	strcpy(sSubKey,"Software\\Xerofox\\LMA\\Settings");
#elif defined(__TSA_FILE_)
	strcpy(sSubKey,"Software\\Xerofox\\TSA\\Settings");
#elif defined(__TDA_FILE_)
	strcpy(sSubKey,"Software\\Xerofox\\TDA\\Settings");
#else
	strcpy(sSubKey,"Software\\Xerofox\\LDS\\Settings");
#endif
	HKEY hKey;
	RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_READ,&hKey);
	if(RegQueryValueEx(hKey,_T("SlotGuiGeTblPath"),NULL,&dwDataType,(BYTE*)&path[0],&dwLength)== ERROR_SUCCESS)
		strcpy(slotguige_path,path);
	else 
		strcpy(slotguige_path,"");
	RegCloseKey(hKey);
}
void GetJgGuiGeDBFilePath(char* jgguige_path)
{
	char path[MAX_PATH]="";
	char sSubKey[MAX_PATH]="";
	DWORD dwDataType,dwLength=MAX_PATH;
#ifdef __TMA_FILE_
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(sSubKey,"Software\\Xerofox\\TMA-EN\\Settings");
#else
	strcpy(sSubKey,"Software\\Xerofox\\TMA\\Settings");
#endif
#elif defined(__LMA_FILE_)
	strcpy(sSubKey,"Software\\Xerofox\\LMA\\Settings");
#elif defined(__TSA_FILE_)
	strcpy(sSubKey,"Software\\Xerofox\\TSA\\Settings");
#elif defined(__TDA_FILE_)
	strcpy(sSubKey,"Software\\Xerofox\\TDA\\Settings");
#else
	strcpy(sSubKey,"Software\\Xerofox\\LDS\\Settings");
#endif
	HKEY hKey;
	RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_READ,&hKey);
	if(RegQueryValueEx(hKey,_T("JgGuiGeTblPath"),NULL,&dwDataType,(BYTE*)&path[0],&dwLength)== ERROR_SUCCESS)
		strcpy(jgguige_path,path);
	else 
		strcpy(jgguige_path,"");
	RegCloseKey(hKey);
}

void GetTubeGuiGeDBFilePath(char* tubeguige_path)
{
	char path[MAX_PATH]="";
	char sSubKey[MAX_PATH]="";
	DWORD dwDataType,dwLength=MAX_PATH;
#ifdef __TMA_FILE_
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(sSubKey,"Software\\Xerofox\\TMA-EN\\Settings");
#else
	strcpy(sSubKey,"Software\\Xerofox\\TMA\\Settings");
#endif
#elif defined(__LMA_FILE_)
	strcpy(sSubKey,"Software\\Xerofox\\LMA\\Settings");
#elif defined(__TSA_FILE_)
	strcpy(sSubKey,"Software\\Xerofox\\TSA\\Settings");
#elif defined(__TDA_FILE_)
	strcpy(sSubKey,"Software\\Xerofox\\TDA\\Settings");
#else
	strcpy(sSubKey,"Software\\Xerofox\\LDS\\Settings");
#endif
	HKEY hKey;
	RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_READ,&hKey);
	if(RegQueryValueEx(hKey,_T("TubeGuiGeTblPath"),NULL,&dwDataType,(BYTE*)&path[0],&dwLength)== ERROR_SUCCESS)
		strcpy(tubeguige_path,path);
	else 
		strcpy(tubeguige_path,"");
	RegCloseKey(hKey);
}

void GetFlatGuiGeDBFilePath(char* flatguige_path)
{
	char path[MAX_PATH]="";
	char sSubKey[MAX_PATH]="";
	DWORD dwDataType,dwLength=MAX_PATH;
#ifdef __TMA_FILE_
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(sSubKey,"Software\\Xerofox\\TMA-EN\\Settings");
#else
	strcpy(sSubKey,"Software\\Xerofox\\TMA\\Settings");
#endif
#elif defined(__LMA_FILE_)
	strcpy(sSubKey,"Software\\Xerofox\\LMA\\Settings");
#elif defined(__TSA_FILE_)
	strcpy(sSubKey,"Software\\Xerofox\\TSA\\Settings");
#elif defined(__TDA_FILE_)
	strcpy(sSubKey,"Software\\Xerofox\\TDA\\Settings");
#else
	strcpy(sSubKey,"Software\\Xerofox\\LDS\\Settings");
#endif
	HKEY hKey;
	RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_READ,&hKey);
	if(RegQueryValueEx(hKey,_T("FlatGuiGeTblPath"),NULL,&dwDataType,(BYTE*)&path[0],&dwLength)== ERROR_SUCCESS)
		strcpy(flatguige_path,path);
	else 
		strcpy(flatguige_path,"");
	RegCloseKey(hKey);
}

void GetAppPath(char* app_path)
{
	/*FILE* fp;
	char path[MAX_PATH];
	char system_path[MAX_PATH];
	GetSystemDirectory(system_path,MAX_PATH);
	strcat(system_path,"\\Tower.sys");
	fp = fopen(system_path,"rt");
	if(fp==NULL)
	{
		AfxMessageBox("ϵͳ�ļ���ȡ����");
		return;
	}
	fgets(path,MAX_PATH,fp);
	if(path[strlen(path)-1]=='\n')
		path[strlen(path)-1]='\0';
	fclose(fp);*/
	char path[MAX_PATH];
	char sSubKey[MAX_PATH]="";
	DWORD dwDataType,dwLength=MAX_PATH;
#ifdef __TMA_FILE_
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(sSubKey,"Software\\Xerofox\\TMA-EN\\SETUP");
#else
	strcpy(sSubKey,"Software\\Xerofox\\TMA\\SETUP");
#endif
#elif defined(__LMA_FILE_)
	strcpy(sSubKey,"Software\\Xerofox\\LMA\\SETUP");
#elif defined(__TSA_FILE_)
	strcpy(sSubKey,"Software\\Xerofox\\TSA\\SETUP");
#elif defined(__TDA_FILE_)
	strcpy(sSubKey,"Software\\Xerofox\\TDA\\SETUP");
#else
	strcpy(sSubKey,"Software\\Xerofox\\LDS\\SETUP");
#endif
	HKEY hKey;
	RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_READ,&hKey);
	if(RegQueryValueEx(hKey,_T("APP_PATH"),NULL,&dwDataType,(BYTE*)&path[0],&dwLength)== ERROR_SUCCESS)
		strcpy(app_path,path);
	else 
	{
		RegCloseKey(hKey);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("There is an error in reading application program path!");
#else
		AfxMessageBox("Ӧ�ó���·����ȡ����");
#endif
		return;
	}
	RegCloseKey(hKey);
}
void GetSysPath(char* sys_path)
{
	/*FILE* fp;
	char path[MAX_PATH];
	char system_path[MAX_PATH];
	GetSystemDirectory(system_path,MAX_PATH);
	strcat(system_path,"\\Tower.sys");
	fp = fopen(system_path,"rt");
	if(fp==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("There is an error in reading system file!");
#else
		AfxMessageBox("ϵͳ�ļ���ȡ����");
#endif
		exit(0);
	}
	fgets(path,MAX_PATH,fp);	//��һ���ǻ�Ŀ¼
	fgets(path,MAX_PATH,fp);	//�ڶ�����ϵͳĿ¼
	if(path[strlen(path)-1]=='\n')
		path[strlen(path)-1]='\0';
	fclose(fp);*/
	
	char path[MAX_PATH];
	char sSubKey[MAX_PATH]="";
	DWORD dwDataType,dwLength=MAX_PATH;
#ifdef __TMA_FILE_
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(sSubKey,"Software\\Xerofox\\TMA-EN\\SETUP");
#else
	strcpy(sSubKey,"Software\\Xerofox\\TMA\\SETUP");
#endif
#elif defined(__LMA_FILE_)
	strcpy(sSubKey,"Software\\Xerofox\\LMA\\SETUP");
#elif defined(__TSA_FILE_)
	strcpy(sSubKey,"Software\\Xerofox\\TSA\\SETUP");
#elif defined(__TDA_FILE_)
	strcpy(sSubKey,"Software\\Xerofox\\TDA\\SETUP");
#else
	strcpy(sSubKey,"Software\\Xerofox\\LDS\\SETUP");
#endif
	HKEY hKey;
	RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_READ,&hKey);
	if(RegQueryValueEx(hKey,_T("SYS_PATH"),NULL,&dwDataType,(BYTE*)&path[0],&dwLength)== ERROR_SUCCESS)
		strcpy(sys_path,path);
	else 
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("There is an error in reading system file!");
#else
		AfxMessageBox("ϵͳ�ļ���ȡ����");
#endif
		exit(0);
	}
}
ACADDRAW_CONTEXT::ACADDRAW_CONTEXT(AcDbBlockTableRecord* pBlockTableRecord,AcDbObjectId layerId/*=NULL*/)
{
	_pBlockTableRecord=pBlockTableRecord;
	currLayerId=layerId;
}
void TextStyle(AcDbTextStyleTableRecord *pTextStyleTblRec,char *style_name)
{
#ifdef _ARX_2007
	//����������ʽ���¼��
	pTextStyleTblRec->setName((ACHAR*)_bstr_t(style_name));
	pTextStyleTblRec->setFileName(L"simplex");
	//���ô������ļ���
	//pTextStyleTblRec->setBigFontFileName(L"gbhzfs.shx");
	if(strchr(sys.dim_map.sFontName,'.'))	//���������ļ�(�����ļ���׺����Ϊ.shx)
		pTextStyleTblRec->setBigFontFileName((ACHAR*)_bstr_t(sys.dim_map.sFontName));
	else									//����ϵͳ�Դ����� wht 10-08-23
		pTextStyleTblRec->setFont((ACHAR*)_bstr_t(sys.dim_map.sFontName),FALSE,FALSE,0,0);
#else
	//����������ʽ���¼��
	pTextStyleTblRec->setName(style_name);
	pTextStyleTblRec->setFileName("simplex");
	//���ô������ļ���
#ifndef _MAPTMA_GRX
	//pTextStyleTblRec->setBigFontFileName("gbhzfs.shx");
	
	if(strchr(sys.dim_map.sFontName,'.'))	//���������ļ�(�����ļ���׺����Ϊ.shx)
		pTextStyleTblRec->setBigFontFileName(sys.dim_map.sFontName);
	else									//����ϵͳ�Դ����� wht 10-08-23
		pTextStyleTblRec->setFont(sys.dim_map.sFontName,FALSE,FALSE,0,0);
#else
	pTextStyleTblRec->setBigFontFileName("CHINA.shx");
#endif
#endif
	//���������Ч������ȱ���
	pTextStyleTblRec->setXScale(0.7);
}
//���������ʽ���л�û��ָ����ʽ,�򴴽���������ʽ��¼
//����������ʽ��¼ID,�������ָ����ʽ,�򷵻�������ʽ��ID

//Ӧ����������ʽ����,�����ʹ��ͬһ��������ʽ��¼��
//ÿ���޸������¼�����壬ÿ������������ٴ��϶�ʱ
//�ᰴ����洢����ʽID������Ӧ��������ʽ�����϶�ʱ�����С�����仯 
//DimTextʱ��������������߶ȹʲ��������� wht 10-01-09
void GetTextStyleId(AcDbObjectId &TextStyleId,char *textStyle/*=NULL*/,bool init_style/*=false*/)
{
	char style[100];
	AcDbTextStyleTable *pTextStyleTbl;//����������ʽ��ָ��
	AcDbTextStyleTableRecord *pTextStyleTblRec;//����������ʽ��¼ָ��
	//��д��ʽ��������ʽ�����ָ����������ʽ���ָ��
	GetCurDwg()->getTextStyleTable(pTextStyleTbl,AcDb::kForWrite);
	//����������ʽ���¼
	//���������ʽ���л�û��ָ����ʽ,�򴴽���������ʽ
	//��������ʽID,�������ָ����ʽ,�򷵻�������ʽ��ID
	if(textStyle==NULL)
		strcpy(style,"Standard");
	else
		strcpy(style,textStyle);
#ifdef _ARX_2007
	if(pTextStyleTbl->getAt((ACHAR*)_bstr_t(style),TextStyleId)!=Acad::eOk)
	{
		Acad::ErrorStatus rescode;
		pTextStyleTblRec =new AcDbTextStyleTableRecord ;
		TextStyle(pTextStyleTblRec,_bstr_t(style));
		//���������С
		pTextStyleTblRec->setTextSize(0);
		//��������ʽ����д��������ʽ���¼
		rescode =pTextStyleTbl->add(TextStyleId,pTextStyleTblRec);
		//�ر�������ʽ���¼
		pTextStyleTbl->close();
		//�ر�������ʽ��
		pTextStyleTblRec->close();
		if(rescode==Acad::eOutOfMemory)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("memory overflow");
		else if(rescode==Acad::eDuplicateRecordName)
			AfxMessageBox("duplicate record name");
		else if(rescode==Acad::eNoDatabase)
			AfxMessageBox("no database");
#else
			AfxMessageBox("�ڴ����");
		else if(rescode==Acad::eDuplicateRecordName)
			AfxMessageBox("��ʽ����");
		else if(rescode==Acad::eNoDatabase)
			AfxMessageBox("û�����ݿ�");
#endif
	}
	else
	{
		pTextStyleTbl->close();
		acdbOpenObject(pTextStyleTblRec,TextStyleId,AcDb::kForWrite);
		if(init_style)
			TextStyle(pTextStyleTblRec,_bstr_t(style));
		pTextStyleTblRec->setTextSize(0);
		//�ر�������ʽ��
		pTextStyleTblRec->close();
	}
#else
	if(pTextStyleTbl->getAt(style,TextStyleId)!=Acad::eOk)
	{
		Acad::ErrorStatus rescode;
		pTextStyleTblRec =new AcDbTextStyleTableRecord ;
		TextStyle(pTextStyleTblRec,style);
		//���������С
		pTextStyleTblRec->setTextSize(0);
		//��������ʽ����д��������ʽ���¼
		rescode =pTextStyleTbl->add(TextStyleId,pTextStyleTblRec);
		//�ر�������ʽ���¼
		pTextStyleTbl->close();
		//�ر�������ʽ��
		pTextStyleTblRec->close();
		if(rescode==Acad::eOutOfMemory)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("memory overflow");
		else if(rescode==Acad::eDuplicateRecordName)
			AfxMessageBox("duplicate record name");
		else if(rescode==Acad::eNoDatabase)
			AfxMessageBox("no database");
#else
			AfxMessageBox("�ڴ����");
		else if(rescode==Acad::eDuplicateRecordName)
			AfxMessageBox("��ʽ����");
		else if(rescode==Acad::eNoDatabase)
			AfxMessageBox("û�����ݿ�");
#endif
	}
	else
	{
		pTextStyleTbl->close();
		acdbOpenObject(pTextStyleTblRec,TextStyleId,AcDb::kForWrite);
		if(init_style)
			TextStyle(pTextStyleTblRec,style);
		pTextStyleTblRec->setTextSize(0);
		//�ر�������ʽ��
		pTextStyleTblRec->close();
	}
#endif
	GetCurDwg()->setTextstyle(TextStyleId);
}

/*DIMBLKϵͳ���� 
""				ʵ�ıպ�
"_DOT"			�� 
"_DOTSMALL"		С��
"_DOTBLANK"		���ĵ�
"_ORIGIN"		ԭ����
"_ORIGIN2"		ԭ���� 2
"_OPEN"			��
"_OPEN90"		ֱ��
"_OPEN30"		30 �Ƚ�
"_CLOSED"		�պ�
"_SMALL"		����С��
"_NONE"			��
"_OBLIQUE"		��б
"_BOXFILLED"	ʵ�Ŀ�
"_BOXBLANK"		��
"_CLOSEDBLANK"	���ıպ�
"_DATUMFILLED"	ʵ�Ļ�׼������
"_DATUMBLANK"	��׼������
"_INTEGRAL"		�������
"_ARCHTICK"		�������
*/
//���óߴ������������
void setDimVar(AcDbDimStyleTableRecord *pDimStyleTblRec, double txt_height/*=2.5*/,
			   BOOL bInside/*=TRUE*/,BYTE cVerticalPosType/*=1*/)
{	
	pDimStyleTblRec ->setDimasz(sys.dim_map.fDimArrowSize);	//��ͷ����Ϊ
#ifdef _ARX_2007
	//pDimStyleTblRec ->setDimblk(L"_ARCHTICK");	//��ͷ����ʽ"/"
	//pDimStyleTblRec ->setDimblk1(L"_ARCHTICK");	//��ͷ����ʽ"/"
	//pDimStyleTblRec ->setDimblk2(L"_ARCHTICK");	//��ͷ����ʽ"/"
	//�û������ü�ͷ���� wht 10-08-02
	pDimStyleTblRec ->setDimblk((ACHAR*)_bstr_t(sys.dim_map.sArrowType));	//��ͷ����ʽ"/"
	pDimStyleTblRec ->setDimblk1((ACHAR*)_bstr_t(sys.dim_map.sArrowType));	//��ͷ����ʽ"/"
	pDimStyleTblRec ->setDimblk2((ACHAR*)_bstr_t(sys.dim_map.sArrowType));	//��ͷ����ʽ"/"
#else
	//pDimStyleTblRec ->setDimblk("_ARCHTICK");	//��ͷ����ʽ"/"
	//pDimStyleTblRec ->setDimblk1("_ARCHTICK");	//��ͷ����ʽ"/"
	//pDimStyleTblRec ->setDimblk2("_ARCHTICK");	//��ͷ����ʽ"/"
	pDimStyleTblRec ->setDimblk(sys.dim_map.sArrowType);	//��ͷ����ʽ"/"
	pDimStyleTblRec ->setDimblk1(sys.dim_map.sArrowType);	//��ͷ����ʽ"/"
	pDimStyleTblRec ->setDimblk2(sys.dim_map.sArrowType);	//��ͷ����ʽ"/"
#endif
	//pDimStyleTblRec ->setDimexe(2);	//�ߴ�����ڱ�ע�Ϸ���������
	//pDimStyleTblRec ->setDimgap(1);	//��ע�������ּ��Ϊ1
	pDimStyleTblRec ->setDimtmove(2);	//0.�ߴ����������ƶ�;1.�ƶ�����ʱ�������;2.�ƶ�����ʱ���������
	pDimStyleTblRec ->setDimexe(0.5*txt_height);	//�ߴ�����ڱ�ע�Ϸ���������Ӧ����Ϊ��ע�ָߵ�һ�� wht 10-01-10
	double fDimGap=max(sys.dim_map.fDimGap,0.25*txt_height); //wht 10-08-02
	pDimStyleTblRec ->setDimgap(fDimGap);			//��ע�������ּ��ӦΪ0.5�ߴ���������	wht 10-01-10
	pDimStyleTblRec ->setDimexo(0);	//�ߴ�������ƫ����Ϊ0
	pDimStyleTblRec ->setDimdli(6);	//��ע�߼��Ϊ6
	pDimStyleTblRec ->setDimtih(0);	//�ߴ�����ڲ�ĵ�����ˮƽ����
	pDimStyleTblRec ->setDimtoh(0);	//�������ˮƽ����
	pDimStyleTblRec ->setDimtxt(txt_height);	//�ߴ����ָ߶�Ϊ
	pDimStyleTblRec ->setDimtxsty(TextStyleTable::hzfs.textStyleId);	//�趨������ʽ
	pDimStyleTblRec ->setDimtad(cVerticalPosType);	//0.���� 1.����λ�ڱ�ע���Ϸ� 2.�ⲿ 3.JIS 4.�·�
	pDimStyleTblRec ->setDimdec(2);	//��עС��λ��
	pDimStyleTblRec ->setDimtdec(1);//��ע����λ��
	if(bInside)	//���ݴ�����������Ƿ񽫳ߴ��ע�����ڳߴ���֮�� wht 11-07-12
		pDimStyleTblRec ->setDimtix(Adesk::kTrue);	//ǿ�ƽ��ߴ���ڳߴ�����ڲ�
	else 
		pDimStyleTblRec ->setDimtix(Adesk::kFalse);	//��ǿ�ƽ��ߴ���ڳߴ�����ڲ�
	pDimStyleTblRec ->setDimtofl(1);//ǿ���ڳߴ�����ڻ���ע��
}
void setDimVar2(AcDbDimStyleTableRecord *pDimStyleTblRec, double txt_height)
{	
	pDimStyleTblRec ->setDimasz(sys.dim_map.fDimArrowSize);	//��ͷ����Ϊ
#ifdef _ARX_2007
	//pDimStyleTblRec ->setDimblk(L"_ARCHTICK");	//��ͷ����ʽ"/"
	//pDimStyleTblRec ->setDimblk1(L"_ARCHTICK");	//��ͷ����ʽ"/"
	//pDimStyleTblRec ->setDimblk2(L"_ARCHTICK");	//��ͷ����ʽ"/"
	//�û������ü�ͷ���� wht 10-08-02
	pDimStyleTblRec ->setDimblk((ACHAR*)_bstr_t(sys.dim_map.sArrowType));	//��ͷ����ʽ"/"
	pDimStyleTblRec ->setDimblk1((ACHAR*)_bstr_t(sys.dim_map.sArrowType));	//��ͷ����ʽ"/"
	pDimStyleTblRec ->setDimblk2((ACHAR*)_bstr_t(sys.dim_map.sArrowType));	//��ͷ����ʽ"/"
#else
	//pDimStyleTblRec ->setDimblk("_ARCHTICK");	//��ͷ����ʽ"/"
	//pDimStyleTblRec ->setDimblk1("_ARCHTICK");	//��ͷ����ʽ"/"
	//pDimStyleTblRec ->setDimblk2("_ARCHTICK");	//��ͷ����ʽ"/"
	pDimStyleTblRec ->setDimblk(sys.dim_map.sArrowType);	//��ͷ����ʽ"/"
	pDimStyleTblRec ->setDimblk1(sys.dim_map.sArrowType);	//��ͷ����ʽ"/"
	pDimStyleTblRec ->setDimblk2(sys.dim_map.sArrowType);	//��ͷ����ʽ"/"
#endif
	pDimStyleTblRec ->setDimtmove(2);	//0.�ߴ����������ƶ�;1.�ƶ�����ʱ�������;2.�ƶ�����ʱ���������
	pDimStyleTblRec ->setDimexe(0);	//�ߴ�����ڱ�ע�Ϸ���������
	pDimStyleTblRec ->setDimexo(0);	//�ߴ�������ƫ����Ϊ0
	pDimStyleTblRec ->setDimdli(2);	//��ע�߼��Ϊ6
	pDimStyleTblRec ->setDimtad(1);	//����λ�ڱ�ע���Ϸ�
	pDimStyleTblRec ->setDimtih(0);	//�ߴ�����ڲ�ĵ�����ˮƽ����
	pDimStyleTblRec ->setDimtoh(0);	//�������ˮƽ����
	pDimStyleTblRec ->setDimtxt(txt_height);	//�ߴ����ָ߶�Ϊ
	pDimStyleTblRec ->setDimtxsty(TextStyleTable::hzfs.textStyleId);	//�趨������ʽ
	pDimStyleTblRec ->setDimgap(1);	//��ע�������ּ��Ϊ1
	pDimStyleTblRec ->setDimdec(2);	//��עС��λ��
	pDimStyleTblRec ->setDimtdec(1);//��ע����λ��
	pDimStyleTblRec ->setDimtix(Adesk::kTrue);	//ǿ�ƽ��ߴ���ڳߴ�����ڲ�
	pDimStyleTblRec ->setDimtofl(1);//ǿ���ڳߴ�����ڻ���ע��
}
//���óߴ������������
void setDimAngleVar(AcDbDimStyleTableRecord *pDimStyleTblRec, double txt_height)
{	
	pDimStyleTblRec ->setDimasz(sys.dim_map.fDimArrowSize);	//��ͷ����Ϊ
#ifdef _ARX_2007
	pDimStyleTblRec ->setDimblk(L"");	//��ͷ����ʽ"/"_OPEN30
	pDimStyleTblRec ->setDimblk1(L"");	//��ͷ����ʽ"/"
	pDimStyleTblRec ->setDimblk2(L"");	//��ͷ����ʽ"/"
#else
	pDimStyleTblRec ->setDimblk("");	//��ͷ����ʽ"/"
	pDimStyleTblRec ->setDimblk1("");	//��ͷ����ʽ"/"
	pDimStyleTblRec ->setDimblk2("");	//��ͷ����ʽ"/"
#endif
	//pDimStyleTblRec ->setDimexe(2);	//�ߴ�����ڱ�ע�Ϸ���������
	//pDimStyleTblRec ->setDimgap(1);	//��ע�������ּ��Ϊ1
	pDimStyleTblRec ->setDimexe(0.5*txt_height);	//�ߴ�����ڱ�ע�Ϸ���������Ӧ����Ϊ��ע�ָߵ�һ�� wht 10-01-10
	pDimStyleTblRec ->setDimgap(0.25*txt_height);	//��ע�������ּ��ӦΪ0.5�ߴ���������	wht 10-01-10
	pDimStyleTblRec ->setDimexo(0);	//�ߴ�������ƫ����Ϊ0
	pDimStyleTblRec ->setDimdli(6);	//��ע�߼��Ϊ6
	pDimStyleTblRec ->setDimtad(2);	//����λ�ڱ�ע���Ϸ�0:����;1:�Ϸ�;2:�ⲿ
	pDimStyleTblRec ->setDimtih(0);	//�ߴ�����ڲ�ĵ�����ˮƽ����
	pDimStyleTblRec ->setDimtoh(0);	//�������ˮƽ����
	pDimStyleTblRec ->setDimtxt(txt_height);	//�ߴ����ָ߶�Ϊ
	pDimStyleTblRec ->setDimtxsty(TextStyleTable::hzfs.textStyleId);	//�趨������ʽ
	pDimStyleTblRec ->setDimadec(1);	//��עС��λ��
	pDimStyleTblRec ->setDimtdec(1);//��ע����λ��
	pDimStyleTblRec ->setDimtix(Adesk::kTrue);	//ǿ�ƽ��ߴ���ڳߴ�����ڲ�
	pDimStyleTblRec ->setDimtofl(1);//ǿ���ڳߴ�����ڻ���ע��
}
TMADRAWING_DIM_STYLE GetDimStyle(char *dimstyle/*=NULL*/,double txt_height/*=2.0*/,BOOL bInside/*=TRUE*/,BYTE cVerticalPosType/*=1*/)
{
	TMADRAWING_DIM_STYLE dimStyle;
	dimStyle.dimStyleName.Copy(dimstyle);
	dimStyle.arrowSize = sys.dim_map.fDimArrowSize;
	dimStyle.Gap = 0.25*txt_height;
	AcDbDimStyleTable *pDimStyleTbl;//����ߴ��עָ��
	AcDbDimStyleTableRecord *pDimStyleTblRec;
	if(dimstyle==NULL)
		dimStyle.dimStyleName.Copy("TOWERBZ");
	else
		dimStyle.dimStyleName.Copy(dimstyle);
	//��д��ʽ�򿪳ߴ���ʽ��
	GetCurDwg()->getDimStyleTable(pDimStyleTbl,AcDb::kForWrite);
#ifdef _ARX_2007
	if(pDimStyleTbl->getAt((ACHAR*)_bstr_t((char*)dimStyle.dimStyleName),dimStyle.dimStyleId)!=Acad::eOk)
	{
		pDimStyleTblRec =new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName((ACHAR*)_bstr_t(dimStyle.dimStyleName));
		//��ߴ��ע��ʽ����д��ߴ��ע��ʽ���¼
		pDimStyleTbl->add( dimStyle.dimStyleId,pDimStyleTblRec);
#else
	if(pDimStyleTbl->getAt((char*)dimStyle.dimStyleName,dimStyle.dimStyleId)!=Acad::eOk)
	{
		pDimStyleTblRec =new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName(dimStyle.dimStyleName);
		//��ߴ��ע��ʽ����д��ߴ��ע��ʽ���¼
		pDimStyleTbl->add( dimStyle.dimStyleId,pDimStyleTblRec);
#endif
	}
	else
		acdbOpenObject(pDimStyleTblRec,dimStyle.dimStyleId,AcDb::kForWrite);
	pDimStyleTbl->close();
	setDimVar(pDimStyleTblRec,txt_height,bInside,cVerticalPosType);
	pDimStyleTblRec->close();
	return dimStyle;
}
TMADRAWING_DIM_STYLE GetDimStyle2(char *dimstyle/*=NULL*/,double txt_height/*=2.0*/)
{
	TMADRAWING_DIM_STYLE dimStyle;
	dimStyle.arrowSize = sys.dim_map.fDimArrowSize;
	dimStyle.Gap = 0.25*txt_height;
	if(dimstyle==NULL)
		dimStyle.dimStyleName.Copy("TOWERBZ2");
	else
		dimStyle.dimStyleName.Copy(dimstyle);
	//��д��ʽ�򿪳ߴ���ʽ��
	AcDbDimStyleTable *pDimStyleTbl;//����ߴ��עָ��
	AcDbDimStyleTableRecord *pDimStyleTblRec;
	GetCurDwg()->getDimStyleTable(pDimStyleTbl,AcDb::kForWrite);
#ifdef _ARX_2007
	if(pDimStyleTbl->getAt((ACHAR*)_bstr_t((char*)dimStyle.dimStyleName),dimStyle.dimStyleId)!=Acad::eOk)
	{
		pDimStyleTblRec =new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName((ACHAR*)_bstr_t(dimStyle.dimStyleName));
#else
	if(pDimStyleTbl->getAt((char*)dimStyle.dimStyleName,dimStyle.dimStyleId)!=Acad::eOk)
	{
		pDimStyleTblRec =new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName(dimStyle.dimStyleName);
		//��ߴ��ע��ʽ����д��ߴ��ע��ʽ���¼
		pDimStyleTbl->add( dimStyle.dimStyleId,pDimStyleTblRec);
#endif
	}
	else
		acdbOpenObject(pDimStyleTblRec,dimStyle.dimStyleId,AcDb::kForWrite);
	setDimVar2(pDimStyleTblRec,txt_height);
	pDimStyleTbl->close();
	pDimStyleTblRec->close();
	return dimStyle;
}
TMADRAWING_DIM_STYLE GetDimAngleStyle(char *dimstyle/*=NULL*/,double txt_height/*=2.0*/)
{
	TMADRAWING_DIM_STYLE dimStyle;
	dimStyle.arrowSize = sys.dim_map.fDimArrowSize;
	dimStyle.Gap = 0.25*txt_height;
	if(dimstyle==NULL)
		dimStyle.dimStyleName.Copy("TOWERBZ3");
	else
		dimStyle.dimStyleName.Copy(dimstyle);
	//��д��ʽ�򿪳ߴ���ʽ��
	AcDbDimStyleTable *pDimStyleTbl;//����ߴ��עָ��
	AcDbDimStyleTableRecord *pDimStyleTblRec;
	GetCurDwg()->getDimStyleTable(pDimStyleTbl,AcDb::kForWrite);
#ifdef _ARX_2007
	if(pDimStyleTbl->getAt((ACHAR*)_bstr_t((char*)dimStyle.dimStyleName),dimStyle.dimStyleId)!=Acad::eOk)
	{
		pDimStyleTblRec =new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName((ACHAR*)_bstr_t((char*)dimStyle.dimStyleName));
#else
	if(pDimStyleTbl->getAt(dimStyle.dimStyleName,dimStyle.dimStyleId)!=Acad::eOk)
	{
		pDimStyleTblRec =new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName(dimStyle.dimStyleName);
		//��ߴ��ע��ʽ����д��ߴ��ע��ʽ���¼
		pDimStyleTbl->add( dimStyle.dimStyleId,pDimStyleTblRec);
#endif
	}
	else
		acdbOpenObject(pDimStyleTblRec,dimStyle.dimStyleId,AcDb::kForWrite);
	setDimAngleVar(pDimStyleTblRec,txt_height);
	//pDimStyleTblRec->setDimfit(1);	//���ð뾶��ע��ʽ
	pDimStyleTbl->close();
	pDimStyleTblRec->close();
	return dimStyle;
}

TMADRAWING_DIM_STYLE GetDimRadiusStyle(char *dimstyle/*=NULL*/,double txt_height/*=2.0*/)
{
	TMADRAWING_DIM_STYLE dimStyle;
	dimStyle.arrowSize = sys.dim_map.fDimArrowSize;
	dimStyle.Gap = 0.25*txt_height;
	if(dimstyle==NULL)
		dimStyle.dimStyleName.Copy("TOWERBZ4");
	else
		dimStyle.dimStyleName.Copy(dimstyle);
	//��д��ʽ�򿪳ߴ���ʽ��
	AcDbDimStyleTable *pDimStyleTbl;//����ߴ��עָ��
	AcDbDimStyleTableRecord *pDimStyleTblRec;
	GetCurDwg()->getDimStyleTable(pDimStyleTbl,AcDb::kForWrite);
#ifdef _ARX_2007
	if(pDimStyleTbl->getAt((ACHAR*)_bstr_t((char*)dimStyle.dimStyleName),dimStyle.dimStyleId)!=Acad::eOk)
	{
		pDimStyleTblRec =new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName((ACHAR*)_bstr_t((char*)dimStyle.dimStyleName));
#else
	if(pDimStyleTbl->getAt(dimStyle.dimStyleName,dimStyle.dimStyleId)!=Acad::eOk)
	{
		pDimStyleTblRec =new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName(dimStyle.dimStyleName);
		//��ߴ��ע��ʽ����д��ߴ��ע��ʽ���¼
		pDimStyleTbl->add( dimStyle.dimStyleId,pDimStyleTblRec);
#endif
	}
	else
		acdbOpenObject(pDimStyleTblRec,dimStyle.dimStyleId,AcDb::kForWrite);
	setDimAngleVar(pDimStyleTblRec,txt_height);
	pDimStyleTbl->close();
	pDimStyleTblRec->close();
	return dimStyle;
}

TMADRAWING_DIM_STYLE::TMADRAWING_DIM_STYLE()
{
	arrowSize=gap=0;
	m_fInnerSetTextSize=0;
}
void TMADRAWING_DIM_STYLE::InitDimStyle(double text_height/*=0*/,double arrow_size/*=0*/,double text_gap/*=0*/)
{
	//��д��ʽ�򿪳ߴ���ʽ��
	AcDbDimStyleTableRecord *pDimStyleTblRec=NULL;
	acdbOpenObject(pDimStyleTblRec,dimStyleId,AcDb::kForWrite);
	if(pDimStyleTblRec)
	{
		if(arrow_size==0)
			arrowSize=sys.dim_map.fDimArrowSize;
		else
			arrowSize=arrow_size;
		m_fInnerSetTextSize=text_height;
		if(m_fInnerSetTextSize==0)
			m_fInnerSetTextSize=sys.dim_map.fDimTextSize;
		if(text_gap==0)
			gap=0.25*m_fInnerSetTextSize;
		else
			gap=text_gap;
		pDimStyleTblRec ->setDimasz(arrowSize);	//��ͷ����Ϊ
		pDimStyleTblRec ->setDimtxt(m_fInnerSetTextSize);	//�ߴ����ָ߶�Ϊ
		pDimStyleTblRec ->setDimexe(0.5*m_fInnerSetTextSize);	//�ߴ�����ڱ�ע�Ϸ���������(�ߴ���߷�Χ)Ӧ����Ϊ��ע�ָߵ�һ�� wht 10-01-10
		pDimStyleTblRec ->setDimgap(gap);	//��ע�������ּ��ӦΪ0.5�ߴ���������	wht 10-01-10
		pDimStyleTblRec->close();
	}
}
double TMADRAWING_DIM_STYLE::get_Gap(){return gap;}
double TMADRAWING_DIM_STYLE::set_Gap(double dimgap){return gap=dimgap;}
double TMADRAWING_DIM_STYLE::CalGap(double text_height)
{
	return max(sys.dim_map.fDimGap,0.25*text_height);
}
//double TMADRAWING_DIM_STYLE::set_Gap(double dimgap)
//{
//}
//��ʼ����ͼ����
CStringKeyHashList<TMADRAWING_LAYER> LayerTable::layerHashList;	//ͼ���ϣ����ͼ����Ϊ��ֵ
TMADRAWING_LAYER LayerTable::VisibleProfileLayer;	//�ṹͼ�пɼ�������ͼ��
TMADRAWING_LAYER LayerTable::BriefLineLayer;		//�����ĵ���ͼ
TMADRAWING_LAYER LayerTable::AuxLineLayer;			//���ĵ���ͼ
TMADRAWING_LAYER LayerTable::UnvisibleProfileLayer;	//�ṹͼ�в��ɼ�������ͼ��
TMADRAWING_LAYER LayerTable::AxisLineLayer;			//�����ߡ��Գ��ߡ���λ����
TMADRAWING_LAYER LayerTable::SonJgAxisLineLayer;	//�ӽǸ�����ͼ��
TMADRAWING_LAYER LayerTable::BreakLineLayer;		//�Ͽ�����
TMADRAWING_LAYER LayerTable::BendLineLayer;			//�Ǹֻ������ְ����
TMADRAWING_LAYER LayerTable::AssistHuoQuLineLayer;	//����������
TMADRAWING_LAYER LayerTable::SimPolyLayer;			//�����α�ע
TMADRAWING_LAYER LayerTable::BoltSymbolLayer;		//��˨ͼ��
TMADRAWING_LAYER LayerTable::BoltLineLayer;			//��˨��
TMADRAWING_LAYER LayerTable::DimSizeLayer;			//�ߴ��ע wht 11-07-04
TMADRAWING_LAYER LayerTable::DimTextLayer;			//���ֱ�ע
TMADRAWING_LAYER LayerTable::CommonLayer;			//���������������ͼԪ����ͼ��
TMADRAWING_LAYER LayerTable::DamagedSymbolLine;		//�ְ����������ʶ��
TMADRAWING_TEXT_STYLE TextStyleTable::hzfs;			//���ַ�������
TMADRAWING_DIM_STYLE DimStyleTable::dimStyle;
TMADRAWING_DIM_STYLE DimStyleTable::dimStyle2;
TMADRAWING_DIM_STYLE DimStyleTable::dimStyle3;
TMADRAWING_DIM_STYLE DimStyleTable::dimStyle4;		//���ַ��ڳߴ������ wht 15-04-28
TMADRAWING_DIM_STYLE DimStyleTable::dimAngleStyle;
TMADRAWING_DIM_STYLE DimStyleTable::dimRadiusStyle;

bool TMADRAWING_LAYER::CopyProperty(TMADRAWING_LAYER *pLayer)
{
	if(pLayer==NULL)
		return false;
	layerName.Copy(pLayer->layerName);
	layerId=pLayer->layerId;
	colorIndex=pLayer->colorIndex;
	lineWeight=pLayer->lineWeight;
	lineTypeId=pLayer->lineTypeId;
	lineTypeName.Copy(pLayer->lineTypeName);
	return true;
}
void InitDrawingEnv()
{
	AcDbObjectId layerId;
	GetCurDwg()->setLtscale(20);
	/*
	//��ʼ��Ĭ�ϵ���ͼ��
		//�ṹͼ�пɼ�������ͼ��
	LayerTable::VisibleProfileLayer.layerName.Copy("0.�ɼ�������");
	LayerTable::VisibleProfileLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::VisibleProfileLayer.lineWeight=AcDb::kLnWt025;
	LayerTable::VisibleProfileLayer.colorIndex=sys.dim_map.crIndex.crOutLine;//��ɫ
	LayerTable::VisibleProfileLayer.layerId=0;
	LayerTable::VisibleProfileLayer.lineTypeId=0;
		//�ṹͼ�в��ɼ�������ͼ��
	LayerTable::UnvisibleProfileLayer.layerName.Copy("1.������");
	LayerTable::UnvisibleProfileLayer.lineTypeName.Copy("HIDDEN");
	LayerTable::UnvisibleProfileLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::UnvisibleProfileLayer.colorIndex=sys.dim_map.crIndex.crDashLine;	//��ɫ
	LayerTable::UnvisibleProfileLayer.layerId=0;
	LayerTable::UnvisibleProfileLayer.lineTypeId=0;
	//�����ĵ���ͼ����ͼ
	LayerTable::BriefLineLayer.layerName.Copy("2.�����ĵ���");
	LayerTable::BriefLineLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::BriefLineLayer.lineWeight=AcDb::kLnWt018;
	LayerTable::BriefLineLayer.colorIndex=sys.dim_map.crIndex.crOutLine;//��ɫ
	LayerTable::BriefLineLayer.layerId=0;
	LayerTable::BriefLineLayer.lineTypeId=0;
	//���ĵ���ͼ
	LayerTable::AuxLineLayer.layerName.Copy("3.���ĵ���");
	LayerTable::AuxLineLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::AuxLineLayer.lineWeight=AcDb::kLnWt018;
	LayerTable::AuxLineLayer.colorIndex=4;	//��ɫ
	LayerTable::AuxLineLayer.layerId=0;
	LayerTable::AuxLineLayer.lineTypeId=0;
		//��˨ͼ��
	LayerTable::BoltSymbolLayer.layerName.Copy("4.��˨");
	LayerTable::BoltSymbolLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::BoltSymbolLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::BoltSymbolLayer.colorIndex=sys.dim_map.crIndex.crLs;
	LayerTable::BoltSymbolLayer.layerId=0;
	LayerTable::BoltSymbolLayer.lineTypeId=0;
		//�����α�ע
	LayerTable::SimPolyLayer.layerName.Copy("5.������");
	LayerTable::SimPolyLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::SimPolyLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::SimPolyLayer.colorIndex=sys.dim_map.crIndex.crDimSimTri;
	LayerTable::SimPolyLayer.layerId=0;
	LayerTable::SimPolyLayer.lineTypeId=0;
		//�����ߡ��Գ��ߡ���λ����
	LayerTable::AxisLineLayer.layerName.Copy("6.������");
	LayerTable::AxisLineLayer.lineTypeName.Copy("DASHDOT2X");
	LayerTable::AxisLineLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::AxisLineLayer.colorIndex=sys.dim_map.crIndex.crAxisLine;	//��ɫ
	LayerTable::AxisLineLayer.layerId=0;
	LayerTable::AxisLineLayer.lineTypeId=0;
		//�Ǹֻ������ְ����
	LayerTable::BendLineLayer.layerName.Copy("7.������");
	LayerTable::BendLineLayer.lineTypeName.Copy("DIVIDE");
	LayerTable::BendLineLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::BendLineLayer.colorIndex=sys.dim_map.crIndex.crHuoQuLine;
	LayerTable::BendLineLayer.layerId=0;
	LayerTable::BendLineLayer.lineTypeId=0;
	//�Ͽ�����
	LayerTable::BreakLineLayer.layerName.Copy("8.���ߡ��Ͽ���");
	LayerTable::BreakLineLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::BreakLineLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::BreakLineLayer.colorIndex=sys.dim_map.crIndex.crDragLine;
	LayerTable::BreakLineLayer.layerId=0;
	LayerTable::BreakLineLayer.lineTypeId=0;
	//���������������ͼԪ����ͼ��
	LayerTable::CommonLayer.layerName.Copy("9.����");
	LayerTable::CommonLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::CommonLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::CommonLayer.colorIndex=sys.dim_map.crIndex.crDragLine;
	LayerTable::CommonLayer.layerId=0;
	LayerTable::CommonLayer.lineTypeId=0;*/
	//ͼ���ϣ��
	if(LayerTable::layerHashList.GetNodeNum()==0)
	{	//��֮ǰ�����������ߣ����Կ�������Ϊ1�� wjh-2016.6.19
		CreateNewLayer(LayerTable::VisibleProfileLayer.layerName,LayerTable::VisibleProfileLayer.lineTypeName,
			LayerTable::VisibleProfileLayer.lineWeight,LayerTable::VisibleProfileLayer.colorIndex,
			LayerTable::VisibleProfileLayer.layerId,LayerTable::VisibleProfileLayer.lineTypeId);	//1
		CreateNewLayer(LayerTable::UnvisibleProfileLayer.layerName,LayerTable::UnvisibleProfileLayer.lineTypeName,
			LayerTable::UnvisibleProfileLayer.lineWeight,LayerTable::UnvisibleProfileLayer.colorIndex,
			LayerTable::UnvisibleProfileLayer.layerId,LayerTable::UnvisibleProfileLayer.lineTypeId);//2
		CreateNewLayer(LayerTable::BriefLineLayer.layerName,LayerTable::BriefLineLayer.lineTypeName,
			LayerTable::BriefLineLayer.lineWeight,LayerTable::BriefLineLayer.colorIndex,
			LayerTable::BriefLineLayer.layerId,LayerTable::BriefLineLayer.lineTypeId);				//3	
		CreateNewLayer(LayerTable::AuxLineLayer.layerName,LayerTable::AuxLineLayer.lineTypeName,
			LayerTable::AuxLineLayer.lineWeight,LayerTable::AuxLineLayer.colorIndex,
			LayerTable::AuxLineLayer.layerId,LayerTable::AuxLineLayer.lineTypeId);					//4
		CreateNewLayer(LayerTable::BoltSymbolLayer.layerName,LayerTable::BoltSymbolLayer.lineTypeName,
			LayerTable::BoltSymbolLayer.lineWeight,LayerTable::BoltSymbolLayer.colorIndex,
			LayerTable::BoltSymbolLayer.layerId,LayerTable::BoltSymbolLayer.lineTypeId);			//5
		CreateNewLayer(LayerTable::SimPolyLayer.layerName,LayerTable::SimPolyLayer.lineTypeName,
			LayerTable::SimPolyLayer.lineWeight,LayerTable::SimPolyLayer.colorIndex,
			LayerTable::SimPolyLayer.layerId,LayerTable::SimPolyLayer.lineTypeId);					//6
		CreateNewLayer(LayerTable::AxisLineLayer.layerName,LayerTable::AxisLineLayer.lineTypeName,
			LayerTable::AxisLineLayer.lineWeight,LayerTable::AxisLineLayer.colorIndex,
			LayerTable::AxisLineLayer.layerId,LayerTable::AxisLineLayer.lineTypeId);				//7
		CreateNewLayer(LayerTable::BendLineLayer.layerName,LayerTable::BendLineLayer.lineTypeName,
			LayerTable::BendLineLayer.lineWeight,LayerTable::BendLineLayer.colorIndex,
			LayerTable::BendLineLayer.layerId,LayerTable::BendLineLayer.lineTypeId);				//8
		CreateNewLayer(LayerTable::BreakLineLayer.layerName,LayerTable::BreakLineLayer.lineTypeName,
			LayerTable::BreakLineLayer.lineWeight,LayerTable::BreakLineLayer.colorIndex,
			LayerTable::BreakLineLayer.layerId,LayerTable::BreakLineLayer.lineTypeId);				//9
		CreateNewLayer(LayerTable::BoltLineLayer.layerName,LayerTable::BoltLineLayer.lineTypeName,
			LayerTable::BoltLineLayer.lineWeight,LayerTable::BoltLineLayer.colorIndex,
			LayerTable::BoltLineLayer.layerId,LayerTable::BoltLineLayer.lineTypeId);				//10
		CreateNewLayer(LayerTable::DimSizeLayer.layerName,LayerTable::DimSizeLayer.lineTypeName,
			LayerTable::DimSizeLayer.lineWeight,LayerTable::DimSizeLayer.colorIndex,
			LayerTable::DimSizeLayer.layerId,LayerTable::DimSizeLayer.lineTypeId);					//11
		CreateNewLayer(LayerTable::DimTextLayer.layerName,LayerTable::DimTextLayer.lineTypeName,
			LayerTable::DimTextLayer.lineWeight,LayerTable::DimTextLayer.colorIndex,
			LayerTable::DimTextLayer.layerId,LayerTable::DimTextLayer.lineTypeId);					//12
		CreateNewLayer(LayerTable::CommonLayer.layerName,LayerTable::CommonLayer.lineTypeName,
			LayerTable::CommonLayer.lineWeight,LayerTable::CommonLayer.colorIndex,
			LayerTable::CommonLayer.layerId,LayerTable::CommonLayer.lineTypeId);					//13
		CreateNewLayer(LayerTable::DamagedSymbolLine.layerName,LayerTable::DamagedSymbolLine.lineTypeName,
			LayerTable::DamagedSymbolLine.lineWeight,LayerTable::DamagedSymbolLine.colorIndex,
			LayerTable::DamagedSymbolLine.layerId,LayerTable::DamagedSymbolLine.lineTypeId);					//14
		CreateNewLayer(LayerTable::SonJgAxisLineLayer.layerName,LayerTable::SonJgAxisLineLayer.lineTypeName,
			LayerTable::SonJgAxisLineLayer.lineWeight,LayerTable::SonJgAxisLineLayer.colorIndex,
			LayerTable::SonJgAxisLineLayer.layerId,LayerTable::SonJgAxisLineLayer.lineTypeId);		//7A
		LayerTable::AssistHuoQuLineLayer.CopyProperty(&LayerTable::BendLineLayer);	//8
		/*
		CreateNewLayer(LayerTable::SonJgAxisLineLayer.layerName,LayerTable::SonJgAxisLineLayer.lineTypeName,
			LayerTable::SonJgAxisLineLayer.lineWeight,LayerTable::SonJgAxisLineLayer.colorIndex,
			LayerTable::SonJgAxisLineLayer.layerId,LayerTable::SonJgAxisLineLayer.lineTypeId);		//14
		CreateNewLayer(LayerTable::AssistHuoQuLineLayer.layerName,LayerTable::AssistHuoQuLineLayer.lineTypeName,
			LayerTable::AssistHuoQuLineLayer.lineWeight,LayerTable::AssistHuoQuLineLayer.colorIndex,
			LayerTable::AssistHuoQuLineLayer.layerId,LayerTable::AssistHuoQuLineLayer.lineTypeId);	//15
		*/
		LayerTable::layerHashList.SetValue(LayerTable::VisibleProfileLayer.layerName,LayerTable::VisibleProfileLayer);
		LayerTable::layerHashList.SetValue(LayerTable::UnvisibleProfileLayer.layerName,LayerTable::UnvisibleProfileLayer);
		LayerTable::layerHashList.SetValue(LayerTable::BriefLineLayer.layerName,LayerTable::BriefLineLayer);
		LayerTable::layerHashList.SetValue(LayerTable::AuxLineLayer.layerName,LayerTable::AuxLineLayer);
		LayerTable::layerHashList.SetValue(LayerTable::BoltSymbolLayer.layerName,LayerTable::BoltSymbolLayer);
		LayerTable::layerHashList.SetValue(LayerTable::SimPolyLayer.layerName,LayerTable::SimPolyLayer);
		LayerTable::layerHashList.SetValue(LayerTable::AxisLineLayer.layerName,LayerTable::AxisLineLayer);
		LayerTable::layerHashList.SetValue(LayerTable::BendLineLayer.layerName,LayerTable::BendLineLayer);
		LayerTable::layerHashList.SetValue(LayerTable::BreakLineLayer.layerName,LayerTable::BreakLineLayer);
		LayerTable::layerHashList.SetValue(LayerTable::BoltLineLayer.layerName,LayerTable::BoltLineLayer);
		LayerTable::layerHashList.SetValue(LayerTable::DimSizeLayer.layerName,LayerTable::DimSizeLayer);
		LayerTable::layerHashList.SetValue(LayerTable::DimTextLayer.layerName,LayerTable::DimTextLayer);
		LayerTable::layerHashList.SetValue(LayerTable::CommonLayer.layerName,LayerTable::CommonLayer);
		LayerTable::layerHashList.SetValue(LayerTable::DamagedSymbolLine.layerName,LayerTable::DamagedSymbolLine);
		LayerTable::layerHashList.SetValue(LayerTable::SonJgAxisLineLayer.layerName,LayerTable::SonJgAxisLineLayer);
		//LayerTable::layerHashList.SetValue(LayerTable::AssistHuoQuLineLayer.layerName,LayerTable::AssistHuoQuLineLayer);
	}
	else 
	{
		if(LayerTable::layerHashList.GetValue("14")==NULL)
		{
			TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.Add("14");
			*pLayer=LayerTable::DamagedSymbolLine;
		}
		TMADRAWING_LAYER* pLayer=NULL;
		for(pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
		{
			int n=LayerTable::layerHashList.GetNodeNum();
			CreateNewLayer(pLayer->layerName,pLayer->lineTypeName,pLayer->lineWeight,pLayer->colorIndex,
				pLayer->layerId,pLayer->lineTypeId);
		}
		//�ɼ�����
		CXhChar16 sLayerName;
		sLayerName.Copy(LayerTable::VisibleProfileLayer.layerName);
		LayerTable::VisibleProfileLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//���ɼ�����
		sLayerName.Copy(LayerTable::UnvisibleProfileLayer.layerName);
		LayerTable::UnvisibleProfileLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//��������ͼ
		sLayerName.Copy(LayerTable::AuxLineLayer.layerName);
		LayerTable::AuxLineLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//������
		sLayerName.Copy(LayerTable::AxisLineLayer.layerName);
		LayerTable::AxisLineLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//�Ǹֻ���
		sLayerName.Copy(LayerTable::BendLineLayer.layerName);
		LayerTable::BendLineLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//��˨ͼ
		sLayerName.Copy(LayerTable::BoltSymbolLayer.layerName);
		LayerTable::BoltSymbolLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//�Ͽ�����
		sLayerName.Copy(LayerTable::BreakLineLayer.layerName);
		LayerTable::BreakLineLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//�����ĵ���ͼ
		sLayerName.Copy(LayerTable::BriefLineLayer.layerName);
		LayerTable::BriefLineLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//�ߴ��ע
		sLayerName.Copy(LayerTable::DimSizeLayer.layerName);
		LayerTable::DimSizeLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//���ֱ�ע
		sLayerName.Copy(LayerTable::DimTextLayer.layerName);
		LayerTable::DimTextLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//�����ͱ�ע
		sLayerName.Copy(LayerTable::SimPolyLayer.layerName);
		LayerTable::SimPolyLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//��˨��
		sLayerName.Copy(LayerTable::BoltLineLayer.layerName);
		LayerTable::BoltLineLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//����
		sLayerName.Copy(LayerTable::CommonLayer.layerName);
		LayerTable::CommonLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//�ְ����������ʶ��
		sLayerName.Copy(LayerTable::DamagedSymbolLine.layerName);
		LayerTable::DamagedSymbolLine.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//����������
		sLayerName.Copy(LayerTable::AssistHuoQuLineLayer.layerName);
		LayerTable::AssistHuoQuLineLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//�ӽǸ�������
		sLayerName.Copy(LayerTable::SonJgAxisLineLayer.layerName);
		LayerTable::SonJgAxisLineLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
	}

	/*CreateNewLayer("0","CONTINUOUS",AcDb::kLnWt025,sys.dim_map.crIndex.crOutLine,layerId);//0
	CreateNewLayer("1","HIDDEN",AcDb::kLnWt018,sys.dim_map.crIndex.crDashLine,layerId);		//1
	CreateNewLayer("2","CONTINUOUS",AcDb::kLnWt018,sys.dim_map.crIndex.crLs,layerId);		//4
	CreateNewLayer("3","CONTINUOUS",AcDb::kLnWt018,sys.dim_map.crIndex.crDimSimTri,layerId);//5
	CreateNewLayer("4","DASHDOT2X",AcDb::kLnWt013,sys.dim_map.crIndex.crAxisLine,layerId);	//6
	CreateNewLayer("5","DIVIDE",AcDb::kLnWt013,sys.dim_map.crIndex.crHuoQuLine,layerId);	//7
	CreateNewLayer("6","CONTINUOUS",AcDb::kLnWt013,sys.dim_map.crIndex.crDragLine,layerId);	//8*/
	TextStyleTable::hzfs.bigFontName.Copy(sys.dim_map.sFontName);
	TextStyleTable::hzfs.textStyleName.Copy("standard");
	GetTextStyleId(TextStyleTable::hzfs.textStyleId,"standard",true);	//��ʼ���ı���ʽ
	DimStyleTable::dimStyle = GetDimStyle("standard",sys.dim_map.fDimTextSize);			//Ĭ�ϱ�ע��ʽ����ע�ߴ�����ڳߴ���֮��
	DimStyleTable::dimStyle2 = GetDimStyle2("dimStyle2",sys.dim_map.fDimTextSize);		//���ڻ��ƽǸּ�ͼ��Сͼ
	DimStyleTable::dimStyle3 = GetDimStyle("dimStyle3",sys.dim_map.fDimTextSize,FALSE);//�����ߴ��עǿ�Ʒ��ڳߴ���֮�䣬��Ҫ���ڱ�ע�Ǹ������� wht 11-07-12	
	DimStyleTable::dimStyle4 = GetDimStyle("dimStyle4",sys.dim_map.fDimTextSize,TRUE,2);//���ַ��ڳߴ������ wht 15-04-28
	DimStyleTable::dimAngleStyle = GetDimAngleStyle("dimAngleStyle",sys.dim_map.fDimAngleTextSize);
	DimStyleTable::dimRadiusStyle = GetDimRadiusStyle("dimRadiusStyle",sys.dim_map.fDimTextSize);
}

void CreateAcadRect(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint topLeft,double rect_width,
					double rect_height,BOOL bHasBulge/*=FALSE*/,THANDLE handle/*=NULL*/
					,f3dPoint waist_vec/*=f3dPoint(0,0,0)*/,double line_width/*=0*/)
{
	int i=0;
	AcDbObjectId LineId;//�����ʶ��
	f3dPoint rectVertex[4];
	AcGePoint2d acad_vertex[4];
	double offset_dist=0;
	if(bHasBulge) //���Ǿ���
		offset_dist=(0.5-CTextFrame::BULGERECT_EXTEND_COEF)*rect_height;
	if(offset_dist>rect_width*0.5)	//���Ǿ�������ܳ�����ȵ�һ��
		offset_dist=rect_width*0.25;	
	if(waist_vec.IsZero())
	{
		rectVertex[0].Set(topLeft.x+offset_dist,topLeft.y,0);
		rectVertex[1].Set(topLeft.x+offset_dist,topLeft.y-rect_height);
		rectVertex[2].Set(topLeft.x+rect_width-offset_dist,topLeft.y-rect_height);
		rectVertex[3].Set(topLeft.x+rect_width-offset_dist,topLeft.y);
	}
	else
	{	//��Բ����
		f3dPoint vec(-waist_vec.y,waist_vec.x);	//vec��ֱ����Բ����
		rectVertex[0]=topLeft+waist_vec*offset_dist;
		rectVertex[1]=topLeft+waist_vec*offset_dist-vec*rect_height;
		rectVertex[2]=topLeft+waist_vec*(rect_width-offset_dist)-vec*rect_height;
		rectVertex[3]=topLeft+waist_vec*(rect_width-offset_dist);
	}
	AcDbPolyline *pPolyLine=new AcDbPolyline(4);//����LINE����
	pPolyLine->setClosed(true);
	for(i=0;i<4;i++)
	{
		acad_vertex[i].x=rectVertex[i].x;
		acad_vertex[i].y=rectVertex[i].y;
		if(bHasBulge&&(i==1||i==3))
			pPolyLine->addVertexAt(0,acad_vertex[i],-1,line_width,line_width);
		else
			pPolyLine->addVertexAt(0,acad_vertex[i],0,line_width,line_width);
	}
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,LineId,pPolyLine))//��ʵ��д�����¼
	{
		if(handle!=NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId,xrecObjId;
			pPolyLine->createExtensionDictionary();
			dictObjId = pPolyLine->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();
			
			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		pPolyLine->close();//���ùرն���ĳ�Ա����
	}
	ads_name ent_name;
	acdbGetAdsName(ent_name,LineId);
	acdbEntUpd(ent_name);
	/* ���´���ʵ�����������д����ظ���
	 * ֻ�е�pBlockTableRecord->database()!=GetCurDwg()��ͬʱ�Ų�ͬ������ʱʵ���ǲ������ wjh-2014.8.9
	ads_name line_name;
	acdbEntLast(line_name);
	acdbEntUpd(line_name);
	struct resbuf *rb = acdbEntGet(line_name);
	acdbEntMod(rb);
	acutRelRb(rb);*/
}

AcDbObjectId CreateAcadPolyLine(ACADDRAW_CONTEXT context,f3dPoint start,
								f3dPoint end,double line_width,THANDLE handle/*=NULL*/)
{
	AcDbObjectId LineId=0;//�����ʶ��
	AcGePoint2d acad_start,acad_end;
	acad_start.x=start.x;
	acad_start.y=start.y;
	acad_end.x=end.x;
	acad_end.y=end.y;
	AcDbBlockTableRecord *pBlockTableRecord=context;
	AcDbPolyline *pPolyLine=new AcDbPolyline(2);//����LINE����
	pPolyLine->addVertexAt(0,acad_start,0,line_width,line_width);
	pPolyLine->addVertexAt(0,acad_end,0,line_width,line_width);
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,LineId,pPolyLine))//��ʵ��д�����¼
	{
		if(handle!=NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId,xrecObjId;
			pPolyLine->createExtensionDictionary();
			dictObjId = pPolyLine->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();
			
			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		pPolyLine->close();//���ùرն���ĳ�Ա����
	}
	ads_name ent_name;
	acdbGetAdsName(ent_name,LineId);
	acdbEntUpd(ent_name);
	/*ads_name line_name;
	acdbEntLast(line_name);
	acdbEntUpd(line_name);
	struct resbuf *rb = acdbEntGet(line_name);
	acdbEntMod(rb);
	acutRelRb(rb);*/
	return LineId;
}

AcDbObjectId CreateAcadPolyLine(ACADDRAW_CONTEXT context,f3dPoint* vertex_arr,int vertex_num,double line_width,THANDLE handle/*=NULL*/)
{
	AcDbBlockTableRecord *pBlockTableRecord=context;
	AcDbObjectId LineId=0;//�����ʶ��
	AcDbPolyline *pPolyLine=new AcDbPolyline(vertex_num);//����LINE����
	AcGePoint2d acad_pnt;
	for(int i=0;i<vertex_num;i++)
	{
		acad_pnt.x=vertex_arr[i].x;
		acad_pnt.y=vertex_arr[i].y;
		pPolyLine->addVertexAt(i,acad_pnt,0,line_width,line_width);
	}
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,LineId,pPolyLine))//��ʵ��д�����¼
	{
		if(handle!=NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId,xrecObjId;
			pPolyLine->createExtensionDictionary();
			dictObjId = pPolyLine->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();
			
			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		pPolyLine->close();//���ùرն���ĳ�Ա����
	}
	ads_name ent_name;
	acdbGetAdsName(ent_name,LineId);
	acdbEntUpd(ent_name);
	/*ads_name line_name;
	acdbEntLast(line_name);
	acdbEntUpd(line_name);
	struct resbuf *rb = acdbEntGet(line_name);
	acdbEntMod(rb);
	acutRelRb(rb);*/
	return LineId;
}

//handle���ڼ�¼��ֱ�����������ľ��,style����ù����Ļ��Ʒ�ʽ
//���ڽǸֹ�����style:0.����Ϣ;'X',��ʾX֫��������;-'x',��ʾX֫��������;'Y'��ʾY֫��������;'y'��ʾY֫��������;'Z'��ʾ����ͼ
AcDbObjectId CreateAcadLine(ACADDRAW_CONTEXT context,
							f3dPoint start, f3dPoint end,long handle/*=NULL*/,long style/*=0*/)
{
	AcDbObjectId LineId=0;//�����ʶ��
	AcGePoint3d acad_start,acad_end;
	start.z = end.z = 0;	//Z������㣬����ƽ��ͼ����ͬһZƽ���ϡ�WJH-2006/06/27
	if(start==end)
		return LineId;	//ʼ�յ��غ�
	Cpy_Pnt(acad_start,start);
	Cpy_Pnt(acad_end,end);
	AcDbBlockTableRecord *pBlockTableRecord=context;
	AcDbLine *pLine=new AcDbLine(acad_start,acad_end);//����LINE����
	if(context.currLayerId.isValid())
		pLine->setLayer(context.currLayerId);
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,LineId,pLine))//��ʵ��д�����¼
	{
		if(handle!=NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId,xrecObjId;
			pLine->createExtensionDictionary();
			dictObjId = pLine->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();
			
			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle,AcDb::kDxfInt32, style,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		pLine->close();//���ùرն���ĳ�Ա����
	}
	ads_name ent_name;
	acdbGetAdsName(ent_name,LineId);
	acdbEntUpd(ent_name);
	/* ���´���ʵ�����������д����ظ���
	 * ֻ�е�pBlockTableRecord->database()!=GetCurDwg()��ͬʱ�Ų�ͬ������ʱʵ���ǲ������ wjh-2014.8.9
	ads_name line_name;
	acdbEntLast(line_name);
	acdbEntUpd(line_name);
	struct resbuf *rb = acdbEntGet(line_name);
	acdbEntMod(rb);
	acutRelRb(rb);*/
	return LineId;
}

//�����Բ����
AcDbObjectId CreateAcadPolyArcLine(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint start,f3dPoint end,
								   double sector_angle,double line_width,THANDLE handle/*=NULL*/)
{
	AcDbObjectId LineId;//�����ʶ��
	AcGePoint2d acad_end,acad_start;
	acad_end.x=end.x;
	acad_end.y=end.y;
	acad_start.x=start.x;
	acad_start.y=start.y;
	double fBulge=tan(sector_angle/4.0);	//���������ϵ��
	AcDbPolyline *pPolyLine=new AcDbPolyline(2);//����LINE����
	pPolyLine->addVertexAt(0,acad_start,fBulge,line_width,line_width);
	pPolyLine->addVertexAt(1,acad_end,0,line_width,line_width);
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,LineId,pPolyLine))//��ʵ��д�����¼
	{
		if(handle!=NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId,xrecObjId;
			pPolyLine->createExtensionDictionary();
			dictObjId = pPolyLine->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();
			
			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		pPolyLine->close();//���ùرն���ĳ�Ա����
	}
	/*ads_name ent_name;
	acdbGetAdsName(ent_name,LineId);
	acdbEntUpd(ent_name);
	ads_name line_name;
	acdbEntLast(line_name);
	acdbEntUpd(line_name);
	struct resbuf *rb = acdbEntGet(line_name);
	acdbEntMod(rb);
	acutRelRb(rb);*/
	return LineId;
}

//�����Բ����
AcDbObjectId CreateAcadPolyArcLine(AcDbBlockTableRecord *pBlockTableRecord,f3dArcLine arcLine,
								   double line_width,THANDLE handle/*=NULL*/)
{
	AcDbObjectId LineId;//�����ʶ��
	AcGePoint2d acad_end,acad_start;
	acad_end.x=arcLine.End().x;
	acad_end.y=arcLine.End().y;
	acad_start.x=arcLine.Start().x;
	acad_start.y=arcLine.Start().y;
	double sector_angle=arcLine.SectorAngle();
	if(arcLine.WorkNorm().z<0)	
		sector_angle*=-1.0;	//˳ʱ��Բ�� wht 11-07-09
	double fBulge=tan(sector_angle/4.0);	//���������ϵ��
	AcDbPolyline *pPolyLine=new AcDbPolyline(2);//����LINE����
	pPolyLine->addVertexAt(0,acad_start,fBulge,line_width,line_width);
	pPolyLine->addVertexAt(1,acad_end,0,line_width,line_width);
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,LineId,pPolyLine))//��ʵ��д�����¼
	{
		if(handle!=NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId,xrecObjId;
			pPolyLine->createExtensionDictionary();
			dictObjId = pPolyLine->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();
			
			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		pPolyLine->close();//���ùرն���ĳ�Ա����
	}
	/*ads_name ent_name;
	acdbGetAdsName(ent_name,LineId);
	acdbEntUpd(ent_name);
	ads_name line_name;
	acdbEntLast(line_name);
	acdbEntUpd(line_name);
	struct resbuf *rb = acdbEntGet(line_name);
	acdbEntMod(rb);
	acutRelRb(rb);*/
	return LineId;
}

AcDbObjectId CreateAcadArcLine(AcDbBlockTableRecord *pBlockTableRecord,
							   f3dPoint center,f3dPoint startPoint,f3dPoint endPoint,THANDLE handle/*=NULL*/)
{	//����Բ����㣬�յ��Լ��뾶����Բ��
	center.z=startPoint.z=endPoint.z=0;
	f3dPoint start_line_vec=startPoint-center;
	f3dPoint end_line_vec=endPoint-center;
	double startAngle = Cal2dLineAng(f2dPoint(0,0),f2dPoint(start_line_vec.x,start_line_vec.y));
	double endAngle = Cal2dLineAng(f2dPoint(0,0),f2dPoint(end_line_vec.x,end_line_vec.y));
	double sectorAngle=endAngle-startAngle;
	return CreateAcadArcLine(pBlockTableRecord,center,startPoint,sectorAngle,f3dPoint(0,0,1),handle);
}

AcDbObjectId CreateAcadArcLine(AcDbBlockTableRecord *pBlockTableRecord,
							   f3dPoint center,f3dPoint startPoint,double sectorAngle,THANDLE handle/*=NULL*/)
{
	return CreateAcadArcLine(pBlockTableRecord,center,startPoint,sectorAngle,f3dPoint(0,0,1),handle);
}

AcDbObjectId CreateAcadArcLine(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint center,
							   f3dPoint startPoint,double sectorAngle,f3dPoint worknorm,THANDLE handle/*=NULL*/)
{
	//���������3������Բ����Բ�ģ��뾶����ʼ�Ƕȣ���ֹ�Ƕ�
	double radius,startAngle,endAngle;
	f2dLine lineStart,lineEnd;
	center.z=startPoint.z=0;
	//��ʼ�Ƕȣ���ֹ�Ƕ�
	f3dPoint start_line_vec=startPoint-center;
	radius=start_line_vec.mod();
	startAngle = Cal2dLineAng(f2dPoint(0,0),f2dPoint(start_line_vec.x,start_line_vec.y));
	if(worknorm.z<0)
		startAngle=Pi-startAngle;
	if(startAngle<0)
		startAngle+=2*Pi;
	endAngle = startAngle+sectorAngle;

	AcDbObjectId ArcId;//�����ʶ��
	AcGePoint3d acad_center;
	AcGeVector3d normal;
	normal.set(worknorm.x,worknorm.y,worknorm.z);
	Cpy_Pnt(acad_center,center);
	AcDbArc *pArc=new AcDbArc(acad_center,normal,radius,startAngle,endAngle);//����ARC����
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,ArcId,pArc))//��ʵ��д�����¼
	{
		if(handle!=NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId,xrecObjId;
			pArc->createExtensionDictionary();
			dictObjId = pArc->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();
			
			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		pArc->close();//���ùرն���ĳ�Ա����
	}
	/*ads_name ent_name;
	acdbGetAdsName(ent_name,ArcId);
	acdbEntUpd(ent_name);
	ads_name arc_name;
	acdbEntLast(arc_name);
	acdbEntUpd(arc_name);
	struct resbuf *rb = acdbEntGet(arc_name);
	acdbEntMod(rb);
	acutRelRb(rb);*/
	return ArcId;
}

AcDbObjectId CreateAcadCircle(ACADDRAW_CONTEXT context,
							  f3dPoint centre, double radius,THANDLE handle/*=NULL*/)
{
	AcDbBlockTableRecord *pBlockTableRecord=context;
	AcDbObjectId CircleId;//�����ʶ��
	AcGePoint3d acad_centre;
	AcGeVector3d norm(0,0,1);
	centre.z = 0;	//Z������㣬����ƽ��ͼ����ͬһZƽ���ϡ�WJH-2006/06/27
	Cpy_Pnt(acad_centre,centre);
	AcDbCircle *pCircle=new AcDbCircle(acad_centre,norm,radius);//����CIRCLE����
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,CircleId,pCircle))//��ʵ��д�����¼
	{
		if(handle!=NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId,xrecObjId;
			pCircle->createExtensionDictionary();
			dictObjId = pCircle->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();
			
			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle,AcDb::kDxfInt32, STEELTYPE_BOLT,0);
			pXrec->setFromRbChain(*head);
			ads_relrb(head);
			pXrec->close();
		}
		pCircle->close();//���ùرն���ĳ�Ա����
	}
	ads_name ent_name;
	acdbGetAdsName(ent_name,CircleId);
	acdbEntUpd(ent_name);
	/*ads_name circle_name;
	acdbEntLast(circle_name);
	acdbEntUpd(circle_name);
	struct resbuf *rb = acdbEntGet(circle_name);
	acdbEntMod(rb);
	acutRelRb(rb);*/
	return CircleId;
}

void CreateAcadEllipseLine(AcDbBlockTableRecord *pBlockTableRecord,f3dArcLine arcline,THANDLE handle/*=NULL*/)
{
	f3dPoint majorAxis,minorAxis=arcline.ColumnNorm()^arcline.WorkNorm();
	majorAxis.Set(-minorAxis.y,minorAxis.x,minorAxis.z);
	normalize(majorAxis);
	double radiusRatio=fabs(arcline.ColumnNorm()*arcline.WorkNorm());
	f3dPoint start_vec,end_vec;
	double start_angle=0,end_angle=0;
	start_vec=arcline.Start()-arcline.Center();
	normalize(start_vec);
	if(arcline.SectorAngle()<Pi+Pi-EPS)
	{
		end_vec=arcline.End()-arcline.Center();
		normalize(end_vec);
		double start_cosa=start_vec*majorAxis;
		double end_cosa=end_vec*majorAxis;
		//������ʼ�������᷽����ͬ����� wht 10-08-31
		start_angle=ACOS(start_cosa);
		end_angle=ACOS(end_cosa);
		start_vec=majorAxis^start_vec;
		end_vec=majorAxis^end_vec;
		if(start_vec*arcline.WorkNorm()<0)
			start_angle=2*Pi-start_angle;
		if(end_vec*arcline.WorkNorm()<0)
			end_angle=2*Pi-end_angle;
	}
	else
	{
		end_vec=start_vec;
		start_angle=0;
		end_angle=Pi+Pi;
	}
	majorAxis*=arcline.Radius()/radiusRatio;
	CreateAcadEllipseLine(pBlockTableRecord,arcline.Center(),arcline.WorkNorm(),majorAxis,radiusRatio,start_angle,end_angle,handle);
}
void CreateAcadEllipseLine(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint center,
				   f3dPoint normal,f3dPoint majorAxis,double radiusRatio,double startAngle,double endAngle,THANDLE handle/*=NULL*/)
{
	//���������3������Բ����Բ�ģ��뾶����ʼ�Ƕȣ���ֹ�Ƕ�
	center.z=0;

	AcDbObjectId ArcId;//�����ʶ��
	AcGePoint3d acad_center;
	AcGeVector3d acad_normal,acad_majorAxis;
	acad_normal.set(normal.x,normal.y,normal.z);
	acad_majorAxis.set(majorAxis.x,majorAxis.y,majorAxis.z);
	Cpy_Pnt(acad_center,center);
	AcDbEllipse *pEllipse=new AcDbEllipse(acad_center,acad_normal,acad_majorAxis,radiusRatio,startAngle,endAngle);//����ARC����
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,ArcId,pEllipse))//��ʵ��д�����¼
	{
		if(handle!=NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId,xrecObjId;
			pEllipse->createExtensionDictionary();
			dictObjId = pEllipse->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();
			
			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		pEllipse->close();//���ùرն���ĳ�Ա����
	}
	/*ads_name ent_name;
	acdbGetAdsName(ent_name,ArcId);
	acdbEntUpd(ent_name);
	ads_name arc_name;
	acdbEntLast(arc_name);
	acdbEntUpd(arc_name);
	struct resbuf *rb = acdbEntGet(arc_name);
	acdbEntMod(rb);
	acutRelRb(rb);*/
}

void CreateAcadSpline(AcDbBlockTableRecord *pBlockTableRecord,
				   f3dPoint startPoint,f3dPoint midPoint,f3dPoint endPoint)
{	
	AcGePoint3dArray fitPoints;
	AcGePoint3d start(startPoint.x,startPoint.y,startPoint.z),end(endPoint.x,endPoint.y,endPoint.z),mid(midPoint.x,midPoint.y,midPoint.z);
	fitPoints.append(start);
	fitPoints.append(mid);
	fitPoints.append(end);
	AcDbObjectId splineId;//�����ʶ��
	AcDbSpline *pSpline=new AcDbSpline(fitPoints,4,0);//����SPline�������߶���
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,splineId,pSpline))//��ʵ��д�����¼
		pSpline->close();//���ùرն���ĳ�Ա����
}

void CreateAcadSpline(AcDbBlockTableRecord *pBlockTableRecord,AcGePoint3dArray fitPoints)
{
	AcDbObjectId splineId;//�����ʶ��
	AcDbSpline *pSpline=new AcDbSpline(fitPoints,4,0);	//����SPline�������߶���
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,splineId,pSpline))//��ʵ��д�����¼
		pSpline->close();//���ùرն���ĳ�Ա����
}
void CreateAcadSectionLine(AcDbBlockTableRecord *pBlockTableRecord,	//����һ������
					f3dPoint startPoint, f3dPoint endPoint, double scale)
{
	double acad_radius,line_len,offset_len,startAngle,endAngle;
	startPoint.z = endPoint.z = 0;	//Z������㣬����ƽ��ͼ����ͬһZƽ���ϡ�
	line_len = DISTANCE(endPoint,startPoint);
	acad_radius = scale*line_len;
	offset_len = sqrt(SQR(acad_radius)-SQR(0.25*line_len));
	f3dPoint midPoint,start_mid,end_mid,start_line_vec,end_line_vec,center[4];
	midPoint = (startPoint+endPoint)/2.0;
	start_mid=(startPoint+midPoint)/2.0;
	end_mid=(endPoint+midPoint)/2.0;
	start_line_vec=startPoint-midPoint;
	end_line_vec=endPoint-midPoint;
	start_line_vec.Set(-start_line_vec.y,start_line_vec.x);
	end_line_vec.Set(-end_line_vec.y,end_line_vec.x);
	normalize(start_line_vec);
	normalize(end_line_vec);
	center[0] = start_mid + offset_len*start_line_vec;
	center[1] = end_mid + offset_len*end_line_vec;
	center[2] = start_mid - offset_len*start_line_vec;
	center[3] = end_mid - offset_len*end_line_vec;
	//��ʼ�Ƕȣ���ֹ�Ƕ�
	f3dPoint start_vec, end_vec;
	AcDbObjectId ArcId;//�����ʶ��
	AcGePoint3d acad_center;
	for(int i=0;i<4;i++)
	{
		if(i%2==0)
			start_vec = startPoint - center[i];
		else
			start_vec = endPoint - center[i];
		end_vec = midPoint - center[i];
		startAngle = endAngle = Cal2dLineAng(f2dPoint(0,0),f2dPoint(start_vec.x,start_vec.y));
		if(Cal2dLineAng(f2dPoint(0,0),f2dPoint(end_vec.x,end_vec.y))>endAngle)
			endAngle = Cal2dLineAng(f2dPoint(0,0),f2dPoint(end_vec.x,end_vec.y));
		else
			startAngle = Cal2dLineAng(f2dPoint(0,0),f2dPoint(end_vec.x,end_vec.y));
		Cpy_Pnt(acad_center,center[i]);
		AcDbArc *pArc=new AcDbArc(acad_center,acad_radius,startAngle,endAngle);//����ARC����
		if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,ArcId,pArc))//��ʵ��д�����¼
			pArc->close();//���ùرն���ĳ�Ա����
	}
}

//------------װ��ָ������------------------------------------------------//
//�˺����Ժ����޸ģ��Դ�ע����ж�ȡ�����ļ�·�� WJH--01.03.26
bool SetEntLineType(AcDbEntity* pEnt, char* line_type)
{
	if(!load_linetype(line_type))
		return false;
#ifdef _ARX_2007
	else if(pEnt->setLinetype((ACHAR*)_bstr_t(line_type))==Acad::eOk)
#else
	else if(pEnt->setLinetype(line_type)==Acad::eOk)
#endif
		return true;
	else
		return false;
}
bool load_linetype(char *LineType)
{
	AcDbLinetypeTable *pLinetypeTbl;//�������ͱ�ָ��
	AcDbObjectId LineTypeId;//�������͵�ID��
	//�Զ���ʽ�����ͱ���������ͱ�ָ��
	char line_path[MAX_PATH],sys_path[MAX_PATH];
	GetSysPath(sys_path);
	strcpy(line_path,sys_path);
	strcat(line_path ,"Tower.lin");
	//������ͱ������е�ǰ������getAt()�����ID�ţ�����װ��
	GetCurDwg()->getLinetypeTable(pLinetypeTbl,AcDb::kForWrite);
#ifdef _ARX_2007
	if((pLinetypeTbl->getAt((ACHAR*)_bstr_t(LineType),LineTypeId))!=Acad::eOk)
#else
	if((pLinetypeTbl->getAt(LineType,LineTypeId))!=Acad::eOk)
#endif
	{
		pLinetypeTbl->close();//�ر����ͱ�
#ifdef _ARX_2007
		Acad::ErrorStatus error=GetCurDwg()->loadLineTypeFile((ACHAR*)_bstr_t(LineType),(ACHAR*)_bstr_t(line_path));
#else
		Acad::ErrorStatus error=GetCurDwg()->loadLineTypeFile(LineType,line_path);
#endif

		if( error==Acad::eOk)
			return true;
#ifdef _ARX_2007
		else if( GetCurDwg()->loadLineTypeFile((ACHAR*)_bstr_t(LineType),L"acad.lin")==Acad::eOk)
			return true;
		else if( GetCurDwg()->loadLineTypeFile((ACHAR*)_bstr_t(LineType),L"acadiso.lin")==Acad::eOk)
			return true;
#else
		else if( GetCurDwg()->loadLineTypeFile(LineType,"acad.lin")==Acad::eOk)
			return true;
		else if( GetCurDwg()->loadLineTypeFile(LineType,"acadiso.lin")==Acad::eOk)
			return true;
#endif
		else
			return false;
	}
	else
		pLinetypeTbl->close();	//�ر����ͱ�
	return true;
}
//------------------------- �����²�   2000.830 jwz  -------------------------//
bool SetEntLayer(AcDbEntity* pEnt, char *layer)
{
	AcDbObjectId layerId;
    AcDbLayerTable *pLayerTable;
    GetCurDwg()->getLayerTable(pLayerTable,AcDb::kForRead);
#ifdef _ARX_2007
	if(pLayerTable->getAt((ACHAR*)_bstr_t(layer), layerId)==Acad::eOk)
#else
	if(pLayerTable->getAt(layer, layerId)==Acad::eOk)
#endif
	{
		pLayerTable->close();
		pEnt->setLayer(layerId);
		return true;
	}
	else
	{
		pLayerTable->close();
		return false;
	}
}
bool SetCurLayer(AcDbObjectId& layerId,AcDbEntity* pAcadDbEnt/*=NULL*/)
{
	if(layerId.isValid())
	{
		if(pAcadDbEnt!=NULL)
			return pAcadDbEnt->setLayer(layerId)==Acad::eOk;
		else
			return GetCurDwg()->setClayer(layerId)==Acad::eOk;
	}
	else
		return false;
}
bool SetCurLayer(const char *cur_layer)
{
	if(cur_layer==NULL)
		return false;
	else if(strlen(cur_layer)<1)
		return false;
	AcDbObjectId layerId;
    AcDbLayerTable *pLayerTable;
    GetCurDwg()->getLayerTable(pLayerTable,AcDb::kForRead);
#ifdef _ARX_2007
	if(pLayerTable->getAt((ACHAR*)_bstr_t(cur_layer), layerId)==Acad::eOk)
#else
	if(pLayerTable->getAt(cur_layer, layerId)==Acad::eOk)
#endif
	{
		pLayerTable->close();
		GetCurDwg()->setClayer(layerId);
		return true;
	}
	else
	{
		pLayerTable->close();
		return false;
	}
	
}
bool CreateNewLayer(char *newlayer, char* line_type,AcDb::LineWeight line_thick,
					int color_i,AcDbObjectId &layerId,AcDbObjectId &lineTypeId)
{
	char layer[MAX_PATH];
	if(newlayer==NULL)
		return false;
	else
	{
		strcpy(layer,newlayer);
		if(strlen(newlayer)<1)
			return false;
	}
    AcDbLayerTable *pLayerTable;
	AcDbLayerTableRecord *pLayerTableRecord;
    GetCurDwg()->getLayerTable(pLayerTable,AcDb::kForWrite);
#ifdef _ARX_2007
	if(pLayerTable->getAt((ACHAR*)_bstr_t(layer), layerId)==Acad::eOk)
#else
	if(pLayerTable->getAt(layer, layerId)==Acad::eOk)
#endif
	{
		pLayerTable->close();
		acdbOpenObject(pLayerTableRecord, layerId,AcDb::kForWrite);
		pLayerTableRecord->setIsFrozen(0);// layer to THAWED
		pLayerTableRecord->close();
		//��ȡͼ���Ӧ������Id
		AcDbLinetypeTable *pLinetypeTbl;
		GetCurDwg()->getLinetypeTable(pLinetypeTbl,AcDb::kForRead);
		if(pLinetypeTbl)
		{
#ifdef _ARX_2007
			pLinetypeTbl->getAt((ACHAR*)_bstr_t(line_type), lineTypeId);
#else
			pLinetypeTbl->getAt(line_type, lineTypeId);
#endif
			pLinetypeTbl->close();
		}
		return true;
	}
	
	pLayerTableRecord = new AcDbLayerTableRecord;
#ifdef _ARX_2007
	pLayerTableRecord->setName((ACHAR*)_bstr_t(layer));
#else
	pLayerTableRecord->setName(layer);
#endif
	pLayerTableRecord->setIsFrozen(0);// layer to THAWED
	pLayerTableRecord->setIsOff(0);   // layer to ON
	pLayerTableRecord->setVPDFLT(0);  // viewport default
	pLayerTableRecord->setIsLocked(0);// un-locked
	pLayerTableRecord->setLineWeight(line_thick);
	AcCmColor color;
	color.setColorIndex(color_i);
	pLayerTableRecord->setColor(color);
	pLayerTable->add(layerId,pLayerTableRecord);
	pLayerTableRecord->close();
	pLayerTable->close();
	load_linetype(line_type);
	ads_name ent_name;
	if(acdbGetAdsName(ent_name,layerId)==Acad::eOk)
	{
#ifndef _MAPTMA_GRX
		struct resbuf *pData=acdbEntGet(ent_name);
		acdbEntMod(pData);
#endif
		acdbEntUpd(ent_name);
	}

	AcDbLinetypeTable *pLinetypeTbl;
	acdbOpenObject(pLayerTableRecord, layerId,AcDb::kForWrite);
	GetCurDwg()->getLinetypeTable(pLinetypeTbl,
		AcDb::kForRead);
#ifdef _ARX_2007
	pLinetypeTbl->getAt((ACHAR*)_bstr_t(line_type), lineTypeId);
#else
	pLinetypeTbl->getAt(line_type, lineTypeId);
#endif

	pLayerTableRecord->setLinetypeObjectId(lineTypeId);
	pLinetypeTbl->close();
	pLayerTableRecord->close();
	return true;
}

//�޸�ָ��ͼ������
bool ModifySpecialLayer(TMADRAWING_LAYER cur_layer,AcDbObjectId &lineTypeId)
{
	AcDbLayerTableRecord *pLayerTableRecord;
	if(acdbOpenObject(pLayerTableRecord,cur_layer.layerId,AcDb::kForWrite)==Acad::eOk)
	{
		pLayerTableRecord->setIsFrozen(0);// layer to THAWED
		//����
		load_linetype(cur_layer.lineTypeName);
		AcDbLinetypeTable *pLinetypeTbl;
		GetCurDwg()->getLinetypeTable(pLinetypeTbl,AcDb::kForRead);
#ifdef _ARX_2007
		pLinetypeTbl->getAt((ACHAR*)_bstr_t(cur_layer.lineTypeName),lineTypeId);
#else
		pLinetypeTbl->getAt(cur_layer.lineTypeName,lineTypeId);
#endif
		pLayerTableRecord->setLinetypeObjectId(lineTypeId);
		pLinetypeTbl->close();
		//�߿�
		pLayerTableRecord->setLineWeight(cur_layer.lineWeight);
		//��ɫ
		AcCmColor color;
		color.setColorIndex(cur_layer.colorIndex);
		pLayerTableRecord->setColor(color);
		//ͼ����
		char layer[MAX_PATH];
		strcpy(layer,cur_layer.layerName);
#ifdef _ARX_2007
		pLayerTableRecord->setName((ACHAR*)_bstr_t(layer));
#else
		pLayerTableRecord->setName(layer);
#endif
		pLayerTableRecord->close();
		return true;
	}
	return false;
}

//------------����������ʵ�������������Ӿ����дʵ��ʱ��ӵģ�----//
THANDLE GetTaAtomHandle(AcDbEntity* pEnt,long* drawStyle/*=NULL*/)
{
	THANDLE handle = 0;
	resbuf *pRb;
	AcDbXrecord *pXrec;
	AcDbDictionary *pDict;
	AcDbObjectId dictObjId, xrecObjId;
	if(pEnt==NULL)
		return 0;
	dictObjId = pEnt->extensionDictionary();
	if(dictObjId != AcDbObjectId::kNull)
	{
		acdbOpenObject(pDict, dictObjId, AcDb::kForRead);
		CAcDbObjLife dictLife(pDict);
#ifdef _ARX_2007
		if(pDict->getAt(L"TOWER_XREC",(AcDbObject* &)pXrec, AcDb::kForRead)==Acad::eOk)
#else
		if(pDict->getAt("TOWER_XREC",(AcDbObject* &)pXrec, AcDb::kForRead)==Acad::eOk)
#endif
		{
			pXrec->rbChain(&pRb);
			pXrec->close();
			if(pRb!=NULL)
			{
				handle = pRb->resval.rlong;
				if(drawStyle&&pRb->rbnext&&pRb->rbnext->restype==AcDb::kDxfInt32)
					*drawStyle=pRb->rbnext->resval.rlong;
				acutRelRb(pRb);
				return  handle;
			}
		}
		//pDict->close();
	}
	return 0;
}
//��ñ��Ԫ����
BOOL GetGridKey(AcDbEntity* pPoint, GRID_DATA_STRU *grid_data)
{
	THANDLE handle = 0;
	resbuf *pRb,*pNextRb;
	AcDbXrecord *pXrec;
	AcDbDictionary *pDict;
	AcDbObjectId dictObjId, xrecObjId;
	f2dRect rect;
	
	dictObjId = pPoint->extensionDictionary();
	//AcGePoint3d acad_pnt=pPoint->position();
	if(dictObjId != AcDbObjectId::kNull)
	{
		acdbOpenObject(pDict, dictObjId, AcDb::kForRead);
		CAcDbObjLife dictLife(pDict);
#ifdef _ARX_2007
		if(pDict->getAt(L"TOWER_XREC",(AcDbObject* &)pXrec, AcDb::kForRead)==Acad::eOk)
#else
		if(pDict->getAt("TOWER_XREC",(AcDbObject* &)pXrec, AcDb::kForRead)==Acad::eOk)
#endif
		{
			pXrec->rbChain(&pRb);
			pXrec->close();
			if(pRb==NULL)
				return FALSE;
			grid_data->data_type = pRb->resval.rlong;	//��������
			pNextRb=pRb->rbnext;
			if(pNextRb==NULL)
			{
				ads_relrb(pRb);
				return FALSE;
			}
			grid_data->type_id = pNextRb->resval.rlong;	//����ID
			pNextRb=pNextRb->rbnext;
			if(pNextRb==NULL)
			{
				ads_relrb(pRb);
				return FALSE;
			}
			grid_data->fTextHigh = pNextRb->resval.rreal;//���ָ߶�
			pNextRb=pNextRb->rbnext;
			if(pNextRb==NULL)
			{
				ads_relrb(pRb);
				return FALSE;
			}
			grid_data->fHighToWide = pNextRb->resval.rreal;//���ֿ�߱�
			pNextRb=pNextRb->rbnext;
			if(pNextRb==NULL)
			{
				ads_relrb(pRb);
				return FALSE;
			}
			grid_data->min_x = pNextRb->resval.rreal;
			pNextRb=pNextRb->rbnext;
			if(pNextRb==NULL)
			{
				ads_relrb(pRb);
				return FALSE;
			}
			grid_data->min_y = pNextRb->resval.rreal;
			pNextRb=pNextRb->rbnext;
			if(pNextRb==NULL)
			{
				ads_relrb(pRb);
				return FALSE;
			}
			grid_data->max_x = pNextRb->resval.rreal;
			pNextRb=pNextRb->rbnext;
			if(pNextRb==NULL)
			{
				ads_relrb(pRb);
				return FALSE;
			}
			grid_data->max_y = pNextRb->resval.rreal;
			pNextRb=pNextRb->rbnext;
			if(pNextRb)
				grid_data->align_type=pNextRb->resval.rlong;
			else
			{
				grid_data->align_type=4+1;	//Ĭ��Ϊ����
				ads_relrb(pRb);
				return TRUE;	//���뷽ʽΪ��ѡ�Ϊ����֮ǰ�ľɹ��տ��˴�����TRUE wht 18-09-22
			}
			pNextRb=pNextRb->rbnext;
			if(pNextRb==NULL)
			{
				ads_relrb(pRb);
				return TRUE;	//ǰ׺Ϊ��ѡ�Ϊ����֮ǰ�ľɹ��տ��˴�����TRUE wht 18-09-22
			}
			if(pNextRb->resval.rstring!=NULL)
			#ifdef _ARX_2007
				strcpy(grid_data->sContentPrefix,_bstr_t(pNextRb->resval.rstring));
			#else
				strcpy(grid_data->sContentPrefix,pNextRb->resval.rstring);
			#endif
			pNextRb=pNextRb->rbnext;
			if(pNextRb==NULL)
			{
				ads_relrb(pRb);
				return TRUE;	//��׺Ϊ��ѡ�Ϊ����֮ǰ�ľɹ��տ��˴�����TRUE wht 18-09-22
			}
			if(pNextRb->resval.rstring!=NULL)
			#ifdef _ARX_2007
				strcpy(grid_data->sContentSuffix,_bstr_t(pNextRb->resval.rstring));
			#else
				strcpy(grid_data->sContentSuffix,pNextRb->resval.rstring);
			#endif
			ads_relrb(pRb);
			return TRUE;
		}
		//pDict->close();
	}
	return FALSE;
}
//------------����������ʵ�������������Ӿ����дʵ��ʱ��ӵģ�----//
BOOL GetPadText(AcDbPoint* pPadPoint, char *sPadText)
{
	THANDLE handle = 0;
	resbuf *pRb;
	AcDbXrecord *pXrec;
	AcDbDictionary *pDict;
	AcDbObjectId dictObjId, xrecObjId;
	
	dictObjId = pPadPoint->extensionDictionary();
	if(dictObjId != AcDbObjectId::kNull)
	{
		acdbOpenObject(pDict, dictObjId, AcDb::kForRead);
		CAcDbObjLife dictLife(pDict);
#ifdef _ARX_2007
		if(pDict->getAt(L"TOWER_XREC",(AcDbObject* &)pXrec, AcDb::kForRead)==Acad::eOk)
#else
		if(pDict->getAt("TOWER_XREC",(AcDbObject* &)pXrec, AcDb::kForRead)==Acad::eOk)
#endif
		{
			pXrec->rbChain(&pRb);
			pXrec->close();
			for(;pRb!=NULL;pRb=pRb->rbnext)
			{
				if(pRb->restype==AcDb::kDxfText)
				{
#ifdef _ARX_2007
					strcpy(sPadText,_bstr_t(pRb->resval.rstring));
#else
					strcpy(sPadText,pRb->resval.rstring);
#endif
					acutRelRb(pRb);
					if(stricmp(sPadText,"")==0)
						return FALSE;
					else
						return  TRUE;
				}
				else
					continue;
			}
		}
		//pDict->close();
	}
	return FALSE;
}
//-------------------------���Ƶ���ͼ�еĵ���-------------------------//
AcDbObjectId 
DrawLine(f3dPoint start,f3dPoint end, THANDLE Handle/*=NULL*/)//�Ǹ־��
{
	AcDbXrecord *pXrec = new AcDbXrecord;
	AcDbObjectId dictObjId, xrecObjId;
	AcDbDictionary *pDict;
	//��õ�ǰͼ�εĿ��ָ��
	AcDbBlockTable *pBlockTable;
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
	//��õ�ǰͼ�ο���¼ָ��
	AcDbBlockTableRecord *pBlockTableRecord;//�������¼ָ��
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);//��д��ʽ��ģ�Ϳռ䣬��ÿ���¼ָ��
	pBlockTable->close();//�رտ��
	//����LINE���󡣲�д�����ݿ�
	AcGePoint3d acad_start,acad_end;
	start.z = end.z = 0;	//Z������㣬����ƽ��ͼ����ͬһZƽ���ϡ�WJH-2006/06/27
	Cpy_Pnt(acad_start,start);
	Cpy_Pnt(acad_end,end);
	AcDbLine *pLine=new AcDbLine(acad_start,acad_end);//����LINE����
	AcDbObjectId LineId;//�����ʶ��
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,LineId,pLine))//��ʵ��д�����¼
		pBlockTableRecord->close();//�رտ���¼

	if(Handle>=0x20)
	{
		pLine->createExtensionDictionary();
		dictObjId = pLine->extensionDictionary();
		acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
#ifdef _ARX_2007
		pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
		pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
		pDict->close();
		
		struct resbuf *head;
		head = acutBuildList(AcDb::kDxfInt32, Handle,AcDb::kDxfInt32, STEELTYPE_ANGLE,0);
		pXrec->setFromRbChain(*head);
		pXrec->close();
		acutRelRb(head);
	}
	pLine->close();//���ùرն���ĳ�Ա����
	ads_name ent_name;
	acdbGetAdsName(ent_name,LineId);
	acdbEntUpd(ent_name);
	return LineId;//���ض����ʶ
}
//------------------------- ��Բ-------------------------//
AcDbObjectId DrawCircle(double x0,double y0,double r)
{
	AcGePoint3d center(x0,y0,0);//����Բ��
	AcGeVector3d normal(0.0,0.0,1.0);//���巨��ʸ��

	AcDbBlockTable *pBlockTable;//������ָ��
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);//��ȡ��ǰͼ�εĿ��ָ��
	AcDbBlockTableRecord *pBlockTableRecord;//�������¼ָ��
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,
					   AcDb::kForWrite);//��õ�ǰͼ�εĿ��ָ��
	pBlockTable->close();//�رտ��

	//���´���Ϊ����circle���󣬲�д�����ݿ�
	AcDbCircle *pCirc=new AcDbCircle(center,normal,r);//����Circle����
	AcDbObjectId CircleId;//�����ʶ��
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,CircleId,pCirc))//��ʵ��д�����¼
	{
		pBlockTableRecord->close();//�رտ���¼
		pCirc->close();//���ùرն���ĳ�Ա����
	}
	ads_name ent_name;
	acdbGetAdsName(ent_name,CircleId);
	acdbEntUpd(ent_name);
	AcDbEntity *pEntity;//
	acdbOpenObject(pEntity,CircleId,AcDb::kForWrite);

	pEntity->close();//�رն���
	return CircleId;//���ض����ʶ
}


//------------------------- �����ע   2000.828 jwz  -------------------------//
AcDbObjectId MarkText(double x0,double y0,char *str,
					      double height/*=3*/,
					      double rotation/*=0*/)
{
	AcGePoint3d startPt(x0,y0,0);//���������
	
	AcDbBlockTable *pBlockTable;//������ָ��
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);//��ȡ��ǰͼ�εĿ��ָ��
	AcDbBlockTableRecord *pBlockTableRecord;//�������¼ָ��
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,
					   AcDb::kForWrite);//��õ�ǰͼ�εĿ��ָ��
	//AcDbBlockTableRecord *pBlockTableRecord;//�������¼ָ��
	pBlockTable->close();//�رտ��

	//���´���Ϊ����Text���󣬲�д�����ݿ�
#ifdef _ARX_2007
	AcDbText *pText=new AcDbText(startPt,(ACHAR*)_bstr_t(str),AcDbObjectId::kNull,height,rotation);//����Text����
#else
	AcDbText *pText=new AcDbText(startPt,str,AcDbObjectId::kNull,height,rotation);//����Text����
#endif
	pText->setWidthFactor(0.7);
	AcDbObjectId TextId;//�����ʶ��
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,TextId,pText))//��ʵ��д�����¼
		pBlockTableRecord->close();//�رտ���¼
	pText->close();//���ùرն���ĳ�Ա����
	ads_name ent_name;
	acdbGetAdsName(ent_name,TextId);
	acdbEntUpd(ent_name);
	AcDbEntity *pEntity;//
	acdbOpenObject(pEntity,TextId,AcDb::kForWrite);
	
	pEntity->close();//�رն���
	return TextId;//���ض����ʶ
}
//-------------------------����ߴ��ע ��ֱ ˮƽ ��б  2000.927 jwz  -------------------------//
AcDbObjectId 
DimSize(AcDbBlockTableRecord *pBlockTableRecord, 
		f3dPoint start,f3dPoint end,//��ע��ʼ/��ֹ��
		f3dPoint dimpos,const char *dimText,//�ߴ���λ�ü�����
		AcDbObjectId dimStyleId,	//=NULL��ע��ʽ��¼ID��
		int dimtype,double txt_height/*=0*//*,BOOL bInside*/)	//=0 ˮƽ =1 ��ֱ =2��б
{
	AcDbEntity *pDim;
	AcDbRotatedDimension* pRotateDim=NULL;
	AcDbAlignedDimension* pAlignedDim=NULL;
	//double dd = DISTANCE(start,end);
	//ȥ��bInside����,dimStyle3ר������[��ǿ�ƽ��ߴ���ڳߴ���֮��]�ı�ע
	//���ʹ��ͬһ��ע��ʽ�����޸ĸ�����ʱ��Ӱ������ʹ�ø���ʽ�ı�ע  wht 11-07-12
	/*
	if(!bInside)
	{	//ֻ����Ҫ���ñ�ע��ʽʱ����Ҫ����dimStyleId������Ӧ�ı�ע��ʽ��ÿ�ζ��򿪻�Ӱ������ٶ� wht 11-07-07
		AcDbDimStyleTableRecord *pDimStyleRec=NULL;
		acdbOpenObject(pDimStyleRec,dimStyleId,AcDb::kForWrite);
		if(pDimStyleRec)
		{
			double fTxtSize=pDimStyleRec->dimtxt();
			pDimStyleRec->setDimtix(Adesk::kFalse);	//ǿ�ƽ��ߴ���ڳߴ�����ڲ�
			pDimStyleRec->close();
		}
	}
	*/
	GetCurDwg()->setDimstyle(dimStyleId);
	AcGePoint3d acad_start,acad_end,acad_dimpos;
	start.z = end.z = dimpos.z = 0;	//Z������㣬����ƽ��ͼ����ͬһZƽ���ϡ�WJH-2006/06/27
	Cpy_Pnt(acad_start,start);
	Cpy_Pnt(acad_end,end);
	Cpy_Pnt(acad_dimpos,dimpos);
		
	//����ע�Ͷ���
#ifdef _ARX_2007
	if(dimtype==0)	//ˮƽ��ע
		pDim=pRotateDim=new AcDbRotatedDimension(0,acad_start,acad_end,acad_dimpos,(ACHAR*)_bstr_t(dimText),dimStyleId);
	else if(dimtype==1)	//��ֱ��ע
		pDim=pRotateDim=new AcDbRotatedDimension(Pi/2,acad_start,acad_end,acad_dimpos,(ACHAR*)_bstr_t(dimText),dimStyleId);
	else// if (dimtype==2)
		pDim=pAlignedDim=new AcDbAlignedDimension(acad_start,acad_end,acad_dimpos,(ACHAR*)_bstr_t(dimText),dimStyleId);
#else
	if(dimtype==0)	//ˮƽ��ע
		pDim=pRotateDim=new AcDbRotatedDimension(0,acad_start,acad_end,acad_dimpos,dimText,dimStyleId);
	else if(dimtype==1)	//��ֱ��ע
		pDim=pRotateDim=new AcDbRotatedDimension(Pi/2,acad_start,acad_end,acad_dimpos,dimText,dimStyleId);
	else// if (dimtype==2)
		pDim=pAlignedDim=new AcDbAlignedDimension(acad_start,acad_end,acad_dimpos,dimText,dimStyleId);
#endif
	if(pRotateDim&&txt_height>0)
		pRotateDim->setDimtxt(txt_height);
	else if(pAlignedDim&&txt_height>0)
		pAlignedDim->setDimtxt(txt_height);

	AcDbObjectId DimId;//�����ʶ��
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,DimId,pDim))//��ʵ��д�����¼
		pDim->close();//�رն���
	ads_name ent_name;
	acdbGetAdsName(ent_name,DimId);
	acdbEntUpd(ent_name);
	return DimId;
}
//����ת�Ǳ�ע
AcDbObjectId DimAngleSize(AcDbBlockTableRecord *pBlockTableRecord,
		f3dPoint start,f3dPoint end,//��ע��ʼ/��ֹ��
		f3dPoint dimpos,const char *dimText,//�ߴ���λ�ü�����
		AcDbObjectId dimStyleId,	//��ע��ʽ��¼ID��
		double angle,double txt_height/*=0,BOOL bInside*/)
{
	AcGePoint3d acad_start,acad_end,acad_dimpos;
	//double dd = DISTANCE(start,end);
	//ȥ��bInside����,dimStyle3ר������[��ǿ�ƽ��ߴ���ڳߴ���֮��]�ı�ע
	//���ʹ��ͬһ��ע��ʽ�����޸ĸ�����ʱ��Ӱ������ʹ�ø���ʽ�ı�ע  wht 11-07-12
	/*
	if(!bInside)
	{	//ֻ����Ҫ���ñ�ע��ʽʱ����Ҫ����dimStyleId������Ӧ�ı�ע��ʽ��ÿ�ζ��򿪻�Ӱ������ٶ� wht 11-07-07
		AcDbDimStyleTableRecord *pDimStyleRec=NULL;
		acdbOpenObject(pDimStyleRec,dimStyleId,AcDb::kForWrite);
		if(pDimStyleRec)
		{	
			pDimStyleRec->setDimtix(Adesk::kFalse);	//ǿ�ƽ��ߴ���ڳߴ�����ڲ�
			pDimStyleRec->close();
		}
	}*/
	start.z = end.z = dimpos.z = 0;	//Z������㣬����ƽ��ͼ����ͬһZƽ���ϡ�WJH-2006/06/27
	Cpy_Pnt(acad_start,start);
	Cpy_Pnt(acad_end,end);
	Cpy_Pnt(acad_dimpos,dimpos);
	//GetCurDwg()->setDimstyle(dimStyleId);
	//����ע�Ͷ���
#ifdef _ARX_2007
	AcDbRotatedDimension *pDim=new AcDbRotatedDimension(angle,acad_start,acad_end,acad_dimpos,(ACHAR*)_bstr_t(dimText),dimStyleId);
#else
	AcDbRotatedDimension *pDim=new AcDbRotatedDimension(angle,acad_start,acad_end,acad_dimpos,dimText,dimStyleId);
#endif
	if(txt_height>0)
		pDim->setDimtxt(txt_height);
	AcDbObjectId DimId;//�����ʶ��
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,DimId,pDim))//��ʵ��д�����¼
	{
		pDim->close();//�رն���
		ads_name ent_name;
		acdbGetAdsName(ent_name,DimId);
		acdbEntUpd(ent_name);
	}
	return DimId;
}
//���ɶ����ע����
AcDbObjectId DimAlignedSize(AcDbBlockTableRecord *pBlockTableRecord,
		AcDbObjectId dimStyleId,		//��ע��ʽ��¼ID��
		f3dPoint start,f3dPoint end,	//��ע��ʼ/��ֹ��
		f3dPoint dimpos,char *dimText,double txt_height/*=0*/)	//�ߴ���λ�ü�����
{
	AcGePoint3d acad_start,acad_end,acad_dimpos;
	AcDbAlignedDimension *pDim;
	start.z = end.z = dimpos.z = 0;	//Z������㣬����ƽ��ͼ����ͬһZƽ���ϡ�WJH-2006/06/27
	Cpy_Pnt(acad_start,start);
	Cpy_Pnt(acad_end,end);
	Cpy_Pnt(acad_dimpos,dimpos);
	//GetCurDwg()->setDimstyle(dimStyleId);
	//����ע�Ͷ���
#ifdef _ARX_2007
	pDim=new AcDbAlignedDimension(acad_start,acad_end,acad_dimpos,(ACHAR*)_bstr_t(dimText),dimStyleId);
#else
	pDim=new AcDbAlignedDimension(acad_start,acad_end,acad_dimpos,dimText,dimStyleId);
#endif
	if(txt_height>0)
		pDim->setDimtxt(txt_height);
	AcDbObjectId DimId;//�����ʶ��
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,DimId,pDim))//��ʵ��д�����¼
		pDim->close();//�رն���	
	ads_name ent_name;
	acdbGetAdsName(ent_name,DimId);
	acdbEntUpd(ent_name);

	return DimId;
}
AcDbObjectId
DimAngle(AcDbBlockTableRecord *pBlockTableRecord,
		AcDbObjectId dimStyleId,		//��ע��ʽ��¼ID��
		f3dPoint vertex,				//���㶨�ǵĽǶ���
		f3dPoint start,f3dPoint end,	//��ע��ʼ/��ֹ��
		f3dPoint dimpos,char *dimText,	//�ߴ���λ�ü�����
		double txt_height/*=0*/,double arrowsize/*=0*/)
{
	AcGePoint3d acad_vertex,acad_start,acad_end,acad_dimpos;
	AcDb2LineAngularDimension *pDim;
	start.z = end.z = dimpos.z = 0;	//Z������㣬����ƽ��ͼ����ͬһZƽ���ϡ�WJH-2006/06/27
	Cpy_Pnt(acad_vertex,vertex);
	Cpy_Pnt(acad_start,start);
	Cpy_Pnt(acad_end,end);
	Cpy_Pnt(acad_dimpos,dimpos);
	//GetCurDwg()->setDimstyle(dimStyleId);
	//����ע�Ͷ���
#ifdef _ARX_2007
	//pDim=new AcDb3PointAngularDimension(acad_vertex,acad_start,acad_end,
	//								acad_dimpos,_bstr_t(dimText),dimStyleId);
	if(dimText)
		pDim=new AcDb2LineAngularDimension(acad_vertex,acad_start,acad_vertex,acad_end,
										acad_dimpos,(ACHAR*)_bstr_t(dimText),dimStyleId);
	else
		pDim=new AcDb2LineAngularDimension(acad_vertex,acad_start,acad_vertex,acad_end,
										acad_dimpos,NULL,dimStyleId);
#else
	//pDim=new AcDb3PointAngularDimension(acad_vertex,acad_start,acad_end,
	//								acad_dimpos,dimText,dimStyleId);
	pDim=new AcDb2LineAngularDimension(acad_vertex,acad_start,acad_vertex,acad_end,
									acad_dimpos,dimText,dimStyleId);
#endif
	AcDbObjectId DimId;//�����ʶ��
	if(txt_height>0)
		pDim->setDimtxt(txt_height);
	if(arrowsize>0)
		pDim->setDimasz(arrowsize);
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,DimId,pDim))//��ʵ��д�����¼
		pDim->close();//�رն���
	ads_name ent_name;
	acdbGetAdsName(ent_name,DimId);
	acdbEntUpd(ent_name);

	return DimId;
}
AcDbObjectId 
DimDiameter(AcDbBlockTableRecord *pBlockTableRecord, 
			double* chordPos,
			double* farChordPos,	//��ע��ʼ/��ֹ��
			double leaderLen,
			char *dimText,
			AcDbObjectId dimStyleId,double txt_height/*=0*/)//��ע��ʽ��¼ID��
{
	AcDbDiametricDimension *pDim;
	AcDbDimStyleTableRecord *pDimStyleRec=NULL;
	//GetCurDwg()->setDimstyle(dimStyleId);
	AcGePoint3d acad_start,acad_end,acad_dimpos;
	GEPOINT chordPoint(chordPos[0],chordPos[1]),farChordPoint(farChordPos[0],farChordPos[1]);
	//chordPoint.z = farChordPoint.z = 0;	//Z������㣬����ƽ��ͼ����ͬһZƽ���ϡ�WJH-2006/06/27
	Cpy_Pnt(acad_start,chordPoint);
	Cpy_Pnt(acad_end,farChordPoint);
		
	//����ע�Ͷ���
#ifdef _ARX_2007
	pDim=new AcDbDiametricDimension(acad_start,acad_end,leaderLen,(ACHAR*)_bstr_t(dimText),dimStyleId);
#else
	pDim=new AcDbDiametricDimension(acad_start,acad_end,leaderLen,dimText,dimStyleId);
#endif
	if(txt_height>0)
		pDim->setDimtxt(txt_height);
	AcDbObjectId DimId;//�����ʶ��
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,DimId,pDim))//��ʵ��д�����¼
		pDim->close();//�رն���
	ads_name ent_name;
	acdbGetAdsName(ent_name,DimId);
	acdbEntUpd(ent_name);
	return DimId;
}
AcDbObjectId DimRadial(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint center,
						f3dPoint chordPoint,double leaderLen,char *dimText,
						AcDbObjectId dimStyleId,double txt_height/*=0*/)//��ע��ʽ��¼ID��
{
	AcDbRadialDimension *pDim;
	AcDbDimStyleTableRecord *pDimStyleRec=NULL;
	//GetCurDwg()->setDimstyle(dimStyleId);
	AcGePoint3d acad_start,acad_end,acad_dimpos;
	center.z = chordPoint.z = 0;	//Z������㣬����ƽ��ͼ����ͬһZƽ���ϡ�WJH-2006/06/27
	Cpy_Pnt(acad_start,center);
	Cpy_Pnt(acad_end,chordPoint);
		
	//����ע�Ͷ���
#ifdef _ARX_2007
	pDim=new AcDbRadialDimension(acad_start,acad_end,leaderLen,(ACHAR*)_bstr_t(dimText),dimStyleId);
#else
	pDim=new AcDbRadialDimension(acad_start,acad_end,leaderLen,dimText,dimStyleId);
#endif

	if(txt_height>0)
		pDim->setDimtxt(txt_height);
	AcDbObjectId DimId;//�����ʶ��
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,DimId,pDim))//��ʵ��д�����¼
		pDim->close();//�رն���
	ads_name ent_name;
	acdbGetAdsName(ent_name,DimId);
	acdbEntUpd(ent_name);
	return DimId;
}

double DistOf2NodeByHandle(THANDLE handle1, THANDLE handle2,int style)
{
	CLDSNode *pNode1,*pNode2;
	pNode1 = Ta.FromNodeHandle(handle1);
	pNode2 = Ta.FromNodeHandle(handle2);
	if(pNode1==NULL||pNode2==NULL)
		return 0;
	f3dPoint vec;
	vec = pNode1->Position(false)-pNode2->Position(false);
	if(style == 0)		//X���ע
		return fabs(vec.x);
	else if(style == 1)	//Y���ע
		return fabs(vec.y);
	else  if(style == 2)//�����ע
		return vec.mod();
	else// if(style==3)	//��ֱ��ע
		return fabs(vec.z);
}
static AcDbBlockTableRecord *fpBlockTableRecord=NULL;
AcDbBlockTableRecord *GetBlockTableRecord()
{
	//��õ�ǰͼ�εĿ��ָ��
	AcDbBlockTable *pBlockTable;
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
	//��õ�ǰͼ�ο���¼ָ��
	AcDbBlockTableRecord *pBlockTableRecord;//�������¼ָ��
	//��д��ʽ��ģ�Ϳռ䣬��ÿ���¼ָ��
	if(pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite)!=Acad::eOk)
		pBlockTableRecord=fpBlockTableRecord;	//TODO:���ܴ�δ�ر�
	else
		fpBlockTableRecord=pBlockTableRecord;
	pBlockTable->close();//�رտ��
	return pBlockTableRecord;
}

//У�����ֵ���ת����Ϊͷ���ϣ�radiusAngleModeΪTrueʱ��angleΪ������; False:�Ƕ��ơ�wjh-2014.3.26
double ReviseTextAngleToHeadUpward(double angle,bool radiusAngleMode/*=true*/)
{
	if(radiusAngleMode)
	{
		double Pi2=Pi+Pi;
		//ת����0~360�ȷ�Χ��
		while(angle<0)
			angle+=Pi2;
		while(angle>=Pi2)
			angle-=Pi2;

		double half_Pi=Pi*0.5;
		if(angle>half_Pi&&angle<Pi)
			angle+=Pi;
		else if(angle>=Pi&&angle<=Pi+half_Pi)
			angle-=Pi;
	}
	else
	{
		//ת����0~360�ȷ�Χ��
		while(angle<0)
			angle+=360;
		if(angle>360)
			angle-=360;

		if(angle>90&&angle<180)
			angle+=180;
		else if(angle>=180&&angle<=270)
			angle-=180;
	}
	return angle;
}
AcDbObjectId 
DimText(AcDbBlockTableRecord *pBlockTableRecord,
		f3dPoint base,//��ע��ʼ/��ֹ��
		const char *dimText,//�ߴ���λ�ü�����
		AcDbObjectId textStyleId,	//=NULL��ע��ʽ��¼ID��
		double height,double rotation,
		AcDb::TextHorzMode hMode,AcDb::TextVertMode vMode,
		AcDbObjectId layerId/*=AcDbObjectId::kNull*/)	//=NULLͼ��ID��
{
	AcGePoint3d acad_base;
	AcDbText *pText;
	base.z = 0;	//Z������㣬����ƽ��ͼ����ͬһZƽ���ϡ�WJH-2006/06/27
	if(layerId.isValid())
		GetCurDwg()->setClayer(layerId);
	GetCurDwg()->setTextstyle(textStyleId);
	Cpy_Pnt(acad_base,base);
	//����ע�Ͷ���
#ifdef _ARX_2007
	pText=new AcDbText(acad_base,(ACHAR*)_bstr_t(dimText),textStyleId,height,rotation);
#else
	pText=new AcDbText(acad_base,dimText,textStyleId,height,rotation);
#endif

	pText->setHorizontalMode(hMode);
	pText->setVerticalMode(vMode);
	if(hMode!=AcDb::kTextLeft||vMode!=AcDb::kTextBase)
		pText->setAlignmentPoint(acad_base);
	pText->setHeight(height);
	pText->setWidthFactor(0.7);
	AcDbObjectId TextId;//�����ʶ��
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,TextId,pText))//��ʵ��д�����¼
		pText->close();//�رն���
	ads_name ent_name;
	acdbGetAdsName(ent_name,TextId);
	acdbEntUpd(ent_name);
	return TextId;
}
double TestDrawTextLength(const char* dimtext,double height,AcDbObjectId textStyleId)
{
	AcDbMText mtxt;
#ifdef _ARX_2007
	mtxt.setContents((ACHAR*)_bstr_t(dimtext));				//�������ֱ�ע����
#else
	mtxt.setContents(dimtext);				//�������ֱ�ע����
#endif
	mtxt.setWidth(strlen(dimtext)*height);					//ÿ�����ֵ������
	mtxt.setTextHeight(height);
	mtxt.setTextStyle(textStyleId);		//���ֲ����
	return mtxt.actualWidth();
}
AcDbObjectId 
DimMText(AcDbBlockTableRecord *pBlockTableRecord,//����¼ָ��
		char *dimText,//�ߴ���λ�ü�����
		f3dPoint base,//��ע��ʼ/��ֹ��
		double width,	//ÿ�е�����
		double textheight,//���ָ�
		AcDbObjectId textStyleId,	//=NULL��ע��ʽ��¼ID��
		AcDbMText::AttachmentPoint align_type,	//���ֶ��뷽ʽ
		AcDbMText::FlowDirection flowDirection,double angle)
{
	AcDbMText *pText;
	AcGePoint3d acad_base;
	//����ע�Ͷ���
	base.z = 0;	//Z������㣬����ƽ��ͼ����ͬһZƽ���ϡ�WJH-2006/06/27
	Cpy_Pnt(acad_base,base);
	pText=new AcDbMText();
#ifdef _ARX_2007
	pText->setContents((ACHAR*)_bstr_t(dimText));				//�������ֱ�ע����
#else
	pText->setContents(dimText);				//�������ֱ�ע����
#endif
	pText->setLocation(acad_base);				//���ò����
	pText->setWidth(width);					//ÿ�����ֵ������
	pText->setTextHeight(textheight);
	pText->setTextStyle(textStyleId);		//���ֲ����
	pText->setAttachment(align_type);		//���ֶ��뷽ʽ
	pText->setFlowDirection(flowDirection);	//������������
	pText->setRotation(angle);	//������������
	
	AcDbObjectId TextId;//�����ʶ��
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,TextId,pText))//��ʵ��д�����¼
	{
		pText->close();//�رն���
		ads_name ent_name;
		acdbGetAdsName(ent_name,TextId);
		acdbEntUpd(ent_name);
	}

	return TextId;
}
AcDbObjectId SearchBlock(const char *blkname)
{
	AcDbBlockTable *pBT;
	AcDbObjectId blockId;
	AcDbBlockTableRecord *pBTRecord;
	GetCurDwg()->getBlockTable(pBT,AcDb::kForRead);
#ifdef _ARX_2007
	if(pBT->getAt((ACHAR*)_bstr_t(blkname),pBTRecord,AcDb::kForRead)!=Acad::eOk)
#else
	if(pBT->getAt(blkname,pBTRecord,AcDb::kForRead)!=Acad::eOk)
#endif
	{
		pBT->close();
		return NULL;
	}
	else
	{
		pBT->close();
		blockId = pBTRecord->objectId();
		pBTRecord->close();
		return blockId;
	}
}
#ifdef __COMMON_PART_INC_
AcDbObjectId AddEntityToDbs(AcDbEntity *pEntity,AcDbBlockTableRecord *pTemRecord/*=NULL*/)
{
	if(pEntity==NULL)
		return NULL;
	AcDbObjectId entityId;
	if(pTemRecord!=NULL)
		pTemRecord->appendAcDbEntity(entityId, pEntity);
	else
	{
		AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
		pBlockTableRecord->appendAcDbEntity(entityId, pEntity);
		pBlockTableRecord->close();
	}
	pEntity->close();
	return entityId;
}
AcDbObjectId CreateSolidByManner(AcDbBlockTableRecord *pBlockTableRecord, f3dPoint pt3d[], int n, int nManner, COLORREF crSolid,
	THANDLE handle/*=NULL*/, double angle/*=2*Pi*/)
{
	int i = 0;
	int nIndex = GetNearestACI(crSolid);
	AcGePoint3dArray pt3dTemp;
	f3dPoint vec_start = (pt3d[1] - pt3d[0]).normalized();
	f3dPoint vec_end, plane_norm;
	for (i = 2; i < n; i++)
	{
		vec_end = (pt3d[i] - pt3d[0]).normalized();
		plane_norm = vec_start ^ vec_end;
		if (plane_norm.IsZero())
			continue;
		else
			break;
	}
	int j = i;
	pt3dTemp.setLogicalLength(n);
	for (i = 0; i < n; i++)
	{
		if (i > 1 && i != j)
			project_point(pt3d[i], pt3d[0], plane_norm);
		pt3dTemp[i].set(pt3d[i].x, pt3d[i].y, pt3d[i].z);
	}
	AcDbVoidPtrArray lines, regions;
	for (i = 0; i < n; i++)
	{
		AcDbLine *poly = new AcDbLine(pt3dTemp[i], pt3dTemp[(i + 1) % n]);
		lines.append((void *)poly);
	}
	AcDb3dPolyline *poly = new AcDb3dPolyline(AcDb::k3dSimplePoly, pt3dTemp, Adesk::kTrue);
	lines.append((void *)poly);
	if (Acad::eOk != AcDbRegion::createFromCurves(lines, regions))
	{
		char p[100] = "";
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log(" part with handle 0X%x may be designed wrongly!", handle);
#else
		logerr.Log("���Ϊ0X%x�Ĺ��������������!", handle);
#endif
		return NULL;
	}
	AcDbRegion *pRegion = AcDbRegion::cast((AcRxObject*)regions[0]);
	AcDbObjectId solidId;
	AcDb3dSolid *p3dObj = new AcDb3dSolid;
	if (nManner == 0)//���췽ʽ����ʵ��
	{
		AcGePoint3d start(pt3d[n].x, pt3d[n].y, pt3d[n].z);
		AcGePoint3d end(pt3d[n + 1].x, pt3d[n + 1].y, pt3d[n + 1].z);
		AcDbLine *pPath = new AcDbLine(end, start);
		AcDbObjectId pathId = AddEntityToDbs(pPath, pBlockTableRecord);
		if (Acad::eOk != p3dObj->extrudeAlongPath(pRegion, pPath))
			return NULL;
		p3dObj->setColorIndex(nIndex);
		solidId = AddEntityToDbs(p3dObj, pBlockTableRecord);
		//ɾ��������
		AcDbEntity *pEnt = NULL;
		acdbOpenObject(pEnt, pathId, AcDb::kForWrite);
		if (pEnt)
		{
			pEnt->erase(Adesk::kTrue);
			pEnt->close();
		}
	}
	else if (nManner == 1)//��ת��ʽ����ʵ��
	{
		f3dPoint dir_vec = (pt3d[n + 1] - pt3d[n]).normalized();
		AcGePoint3d axisPoint(pt3d[n].x, pt3d[n].y, pt3d[n].z);
		AcGeVector3d axisDir(dir_vec.x, dir_vec.y, dir_vec.z);
		if (Acad::eOk != p3dObj->revolve(pRegion, axisPoint, axisDir, angle))
			return NULL;
		p3dObj->setColorIndex(nIndex);
		solidId = AddEntityToDbs(p3dObj, pBlockTableRecord);
	}
	if (p3dObj)
		p3dObj->close();
	return solidId;
}
#endif
BOOL InsertBlock(AcDbBlockTableRecord *pBlockTableRecord,
						f3dPoint insert_pos,char *blkname,
						double scale,double angle,
						long handle/*=0*/,AcDbObjectId *pOutEntId/*=NULL*/)
{
	AcDbDatabase blkDb(Adesk::kFalse);//����յ����ݿ�
	AcDbObjectId newEntId,blockId;
	AcGePoint3d acad_insert_pos;
	AcDbBlockReference *pBlkRef = new AcDbBlockReference;
	blockId = SearchBlock(blkname);
	Cpy_Pnt(acad_insert_pos,insert_pos);
	if(blockId==NULL)
	{
		char filename[MAX_PATH];
		char temstr[MAX_PATH],sys_path[MAX_PATH];
		GetSysPath(sys_path);
		strcpy(filename,sys_path);
		//TODO:��������ȷ�Ϻ����Ƿ���ڵĴ��룬FindFirstFile,FindNextFile wjh-2014.5.13
		sprintf(temstr,"%s.dwg",blkname);
		long ll=(int)strlen(temstr);
		strcat(filename,temstr);
#ifdef _ARX_2007
		if(blkDb.readDwgFile((ACHAR*)_bstr_t(filename))==Acad::eOk)
#else
		if(blkDb.readDwgFile(filename)==Acad::eOk)
#endif
		{
			AcDbDatabase *pTempDb;
			blkDb.wblock(pTempDb);
#ifdef _ARX_2007
			GetCurDwg()->insert(blockId,_bstr_t(blkname),pTempDb);
#else
			GetCurDwg()->insert(blockId,blkname,pTempDb);
#endif
			delete pTempDb;
		}
		else
		{
			pBlkRef->close();
			return FALSE;
		}
	}
	AcGeScale3d scaleXYZ(scale,scale,scale);
	pBlkRef->setBlockTableRecord(blockId);
	pBlkRef->setPosition(acad_insert_pos);
	pBlkRef->setRotation(angle);
	pBlkRef->setScaleFactors(scaleXYZ);
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,newEntId,pBlkRef))
	{
		if(handle!=NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId,xrecObjId;
			pBlkRef->createExtensionDictionary();
			dictObjId = pBlkRef->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();
			
			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle,AcDb::kDxfInt32, STEELTYPE_BOLT,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		pBlkRef->close();	
	}
	ads_name ent_name;
	acdbGetAdsName(ent_name,newEntId);
	acdbEntUpd(ent_name);
	if(pOutEntId)	//��˨��Ӧ��ʵ��ID wht 11-06-20
		*pOutEntId=newEntId;
	return TRUE;
}
void NewDwgFile(char *filename)
{
	// ����һ�����ݿ�,�õ�ָ��������ݿ��ָ��pDb
	AcDbDatabase Db;
#ifdef _ARX_2007
	Db.saveAs((ACHAR*)_bstr_t(filename));
#else
	Db.saveAs(filename);
#endif
	//acdbHostApplicationServices()->setWorkingDatabase(pDb);
}

//���Ƹֹ��ƶ���
void DrawTubeBreakLine(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint startPt,
					   f3dPoint endPt,double fBreakLen/*=1.4*/,double fLenCoefThreshold/*=8*/)
{
	double len=0.0;
	f3dPoint line_vec,line_norm,midPt,midPt1,midPt2;
	line_vec=endPt-startPt;
	len=line_vec.mod();
	normalize(line_vec);
	line_norm.Set(-2*line_vec.y,2*line_vec.x);
	double half_symbol_width=0.5*fBreakLen;
	//double half_symbol_width=0.7;		//�ƶϷ��Ŵ�С,ԭΪ5
	//half_symbol_width*=(1.0/sys.general_set.StruMapScale);	//ʵ�ʻ�ͼʱ���ֿ����ϻ�ͼ�������ƶ��߷���̫С wjh-2011.10.8
	GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
	if(len<fLenCoefThreshold*fBreakLen)
	{	//�϶̵��ƶ���ֻ����1���ƶϷ���
		midPt=startPt+0.5*len*line_vec;
		startPt-=2*line_vec;
		endPt+=2*line_vec;
		f3dPoint vertex_arr[6];
		vertex_arr[0]=startPt;
		vertex_arr[1]=midPt-half_symbol_width*line_vec;
		vertex_arr[2]=midPt+half_symbol_width*line_norm;
		vertex_arr[3]=midPt-half_symbol_width*line_norm;
		vertex_arr[4]=midPt+half_symbol_width*line_vec;
		vertex_arr[5]=endPt;
		CreateAcadPolyLine(pBlockTableRecord,vertex_arr,6,0);
	}
	else
	{
		midPt1=startPt+0.25*len*line_vec;
		midPt2=startPt+0.75*len*line_vec;
		startPt-=2*line_vec;
		endPt+=2*line_vec;
		f3dPoint vertex_arr[10];
		vertex_arr[0]=startPt;
		vertex_arr[1]=midPt1-half_symbol_width*line_vec;
		vertex_arr[2]=midPt1+half_symbol_width*line_norm;
		vertex_arr[3]=midPt1-half_symbol_width*line_norm;
		vertex_arr[4]=midPt1+half_symbol_width*line_vec;
		vertex_arr[5]=midPt2-half_symbol_width*line_vec;
		vertex_arr[6]=midPt2+half_symbol_width*line_norm;
		vertex_arr[7]=midPt2-half_symbol_width*line_norm;
		vertex_arr[8]=midPt2+half_symbol_width*line_vec;
		vertex_arr[9]=endPt;
		CreateAcadPolyLine(pBlockTableRecord,vertex_arr,10,0);
		/*CreateAcadLine(pBlockTableRecord,startPt,midPt1-half_symbol_width*line_vec);
		CreateAcadLine(pBlockTableRecord,midPt1-half_symbol_width*line_vec,midPt1+half_symbol_width*line_norm);
		CreateAcadLine(pBlockTableRecord,midPt1+half_symbol_width*line_norm,midPt1-half_symbol_width*line_norm);
		CreateAcadLine(pBlockTableRecord,midPt1-half_symbol_width*line_norm,midPt1+half_symbol_width*line_vec);
		CreateAcadLine(pBlockTableRecord,midPt1+half_symbol_width*line_vec,midPt2-half_symbol_width*line_vec);
		CreateAcadLine(pBlockTableRecord,midPt2-half_symbol_width*line_vec,midPt2+half_symbol_width*line_norm);
		CreateAcadLine(pBlockTableRecord,midPt2+half_symbol_width*line_norm,midPt2-half_symbol_width*line_norm);
		CreateAcadLine(pBlockTableRecord,midPt2-half_symbol_width*line_norm,midPt2+half_symbol_width*line_vec);
		CreateAcadLine(pBlockTableRecord,midPt2+half_symbol_width*line_vec,endPt);*/
	}
}

//���ƺ���---��������յ�����Ϊ��ͼ�����µ����꣬widthΪ�����ǵĿ��(mm),���������ʰȡ��pickPt�ж�����һ����ƺ����� wht 12-08-31
void DrawWeldLineSymbol(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint start,
						f3dPoint end,f3dPoint pickPt,double width/*=0.8*/,double height/*=0.7*/)	
{	
	start.z = end.z = 0;
	f3dPoint line_vec = end-start;
	double length = line_vec.mod();
	normalize(line_vec);
	f3dPoint dest_vec;
	dest_vec.Set(line_vec.y,-line_vec.x);
	f3dPoint verify_vec=pickPt-start;
	normalize(verify_vec);
	if(verify_vec*dest_vec<0)
		dest_vec *=-1.0;
	f3dPoint weldEnd, weldStart = start+line_vec*width;
	GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
	for(int i=0; i<ftoi(length/width)-1;i++)
	{
		weldEnd = weldStart+dest_vec*height;
		//���ƺ�����
		CreateAcadLine(pBlockTableRecord,weldStart,weldEnd);
		weldStart += line_vec*width;
	}
}
//���ƺ���---��ֱ�����������ߣ���������յ�����Ϊ��ͼ�����µ����꣬widthΪ�����ǵĿ��(mm)
void DrawWeldLineSymbol(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint start,
						f3dPoint end,double width/*=0.8*/,double height/*=0.7*/,BOOL bClockwise/*=FALSE*/)	
{	
	start.z = end.z = 0;
	f3dPoint line_vec = end-start;
	double length = line_vec.mod();
	normalize(line_vec);
	f3dPoint dest_vec;
	if(bClockwise)	//˳ʱ����ת90�� wht 11-07-22
		dest_vec.Set(line_vec.y,-line_vec.x);
	else			//��ʱ����ת90��
		dest_vec.Set(-line_vec.y,line_vec.x);
	f3dPoint weldEnd, weldStart = start+line_vec*width;
	GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
	for(int i=0; i<ftoi(length/width)-1;i++)
	{
		weldEnd = weldStart+dest_vec*height;
		//���ƺ�����
		CreateAcadLine(pBlockTableRecord,weldStart,weldEnd);
		weldStart += line_vec*width;
	}
}
//���ƺ�����--��Բ������������
void DrawWeldLineSymbol(AcDbBlockTableRecord *pBlockTableRecord,f3dArcLine arcline,double width/*=0.8*/,double height/*=0.7*/)
{	
	double radius=fabs(arcline.Radius());
	double slice_angle=width/radius;
	f3dPoint center = arcline.Center();
	f3dPoint dest_vec;
	f3dPoint weldStart,weldEnd;
	GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
	double angle=slice_angle;
	for(;;)
	{
		if(angle>arcline.SectorAngle())
			break;
		weldStart = arcline.PositionInAngle(angle);
		dest_vec=weldStart-center;
		normalize(dest_vec);
		//���ƺ�����
		CreateAcadLine(pBlockTableRecord,weldStart,weldStart+dest_vec*height);
		angle+=slice_angle;
	}
}
//������ģ���еĵ�����ת����CAD��ͼģ������ϵ
void TransPtToCadUcs(f3dPoint& pt,UCS_STRU* pUcs/*=NULL*/,double scale/*=1*/)
{
	if(pUcs)
	{
		coord_trans(pt,*pUcs,FALSE);
		pt=pt*(1.0/scale);
	}
	else
	{
		double y=pt.z;
		pt.z=pt.y;
		pt.y=-y;
		pt = pt * (1.0 / scale);
	}
}
void TransVtToCadUcs(f3dPoint& vect, UCS_STRU* pUcs/*=NULL*/)
{
	if (pUcs)
		vector_trans(vect, *pUcs, FALSE);
	else
	{
		double y = vect.z;
		vect.z = vect.y;
		vect.y = -y;
	}
}
//��ʵ���м�ȥ������ʵ��
void SubtractSubSolid(AcDbObjectId mainSolidId,fPtList &vertex_list,f3dLine path,int ciType,COLORREF crSolid,
							AcDbBlockTableRecord *pTemRecord/*=NULL*/,double angle/*=2*Pi*/)
{
	AcDb3dSolid *pSubSolid=NULL,*p3dMainSolid = NULL;
	acdbOpenObject(p3dMainSolid,mainSolidId,AcDb::kForWrite);
	if(p3dMainSolid==NULL)
		return;
	AcDbObjectId subSolidId;
	if(pTemRecord)
	{
		if(ciType==0)
		{
			f3dPoint plane_norm=(path.endPt-path.startPt).normalized();
			subSolidId=CreateSolidByExtrude(pTemRecord,vertex_list,plane_norm,path,crSolid);
		}
		else
		{
			f3dPoint axisP=path.startPt;
			f3dPoint axisV=(path.endPt-path.startPt).normalized();
			subSolidId=CreateSolidByRevolve(pTemRecord,vertex_list,axisP,axisV,angle,crSolid);
		}
		acdbOpenObject(pSubSolid,subSolidId,AcDb::kForWrite);
		if(pSubSolid)
		{
			p3dMainSolid->booleanOper(AcDb::kBoolSubtract,pSubSolid);
			pSubSolid->close();
		}
	}
	else
	{
		AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
		if(ciType==0)
		{
			f3dPoint plane_norm=(path.endPt-path.startPt).normalized();
			subSolidId=CreateSolidByExtrude(pBlockTableRecord,vertex_list,plane_norm,path,crSolid);
		}
		else if(ciType==1)
		{
			f3dPoint axisP=path.startPt;
			f3dPoint axisV=(path.endPt-path.startPt).normalized();
			subSolidId=CreateSolidByRevolve(pBlockTableRecord,vertex_list,axisP,axisV,angle,crSolid);
		}
		acdbOpenObject(pSubSolid,subSolidId,AcDb::kForWrite);
		if(pSubSolid)
		{
			p3dMainSolid->booleanOper(AcDb::kBoolSubtract,pSubSolid);
			pSubSolid->close();
		}
		pBlockTableRecord->close();
	}
	if(p3dMainSolid)
		p3dMainSolid->close();
}
//ͨ����������ʵ��(�����)
AcDbObjectId CreateSolidByExtrude(AcDbBlockTableRecord *pBlockTableRecord,fPtList &vertex_list,
								  f3dLine path,COLORREF crSolid,THANDLE handle/*=NULL*/)
{
	f3dPoint plane_norm=path.endPt-path.startPt;
	normalize(plane_norm);
	return CreateSolidByExtrude(pBlockTableRecord,vertex_list,plane_norm,path,crSolid,handle);
}
AcDbObjectId CreateSolidByExtrude(AcDbBlockTableRecord *pBlockTableRecord,fPtList &vertex_list,f3dPoint plane_norm,
								  f3dLine path,COLORREF crSolid,THANDLE handle/*=NULL*/)
{
	int nIndex = GetNearestACI(crSolid);
	//����ʵ�����
	AcDb3dSolid *p3dObj = new AcDb3dSolid;
	//������������
	AcGePoint3dArray pt3dTemp;
	pt3dTemp.setLogicalLength(vertex_list.GetNodeNum());
	int i=0,n=vertex_list.GetNodeNum();
	pt3dTemp.setLogicalLength(n);
	f3dPoint *pPlaneBasePoint=vertex_list.GetFirst();
	pt3dTemp[0].set(pPlaneBasePoint->x,pPlaneBasePoint->y,pPlaneBasePoint->z);
	i=1;
	for (f3dPoint *pPoint=vertex_list.GetNext();pPoint;pPoint=vertex_list.GetNext())
	{
		project_point(*pPoint,*pPlaneBasePoint,plane_norm);
		pt3dTemp[i].set(pPoint->x,pPoint->y,pPoint->z);
		i++;
	}
	AcDb3dPolyline *poly = new AcDb3dPolyline(AcDb::k3dSimplePoly,pt3dTemp,Adesk::kTrue);
	AcDbVoidPtrArray lines,regions;
	lines.append((void *)poly);
	if (Acad::eOk != AcDbRegion::createFromCurves(lines,regions))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("the design of handle 0X%x part may be wrong!",handle);
#else
		logerr.Log("���Ϊ0X%x�Ĺ��������������!",handle);
#endif
		return NULL;
	}
	poly->close();
	AcDbRegion *pRegion = AcDbRegion::cast((AcRxObject*)regions[0]);
	//����·��
	AcDbLine *pPath = new AcDbLine(AcGePoint3d(path.endPt.x,path.endPt.y,path.endPt.z),
		AcGePoint3d(path.startPt.x,path.startPt.y,path.startPt.z));
	//������������ɫ
	pPath->setColorIndex(nIndex);
	//������·��д�����¼
	AcDbObjectId pathId,solidId;
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pathId, pPath))
		pPath->close();
	//��������ʵ��
	if (Acad::eOk != p3dObj->extrudeAlongPath(pRegion,pPath))
		return NULL;
	//����ʵ����ɫ
	p3dObj->setColorIndex(nIndex);
	//��ʵ��д�����¼
	if(pBlockTableRecord->appendAcDbEntity(solidId,p3dObj)==Acad::eOk)
	{
		if(handle!=NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId,xrecObjId;
			p3dObj->createExtensionDictionary();
			dictObjId = p3dObj->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();
			
			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		p3dObj->close();//���ùرն���ĳ�Ա����
	}
	//ɾ��������
	AcDbEntity *pEnt=NULL;
	acdbOpenObject(pEnt,pathId,AcDb::kForWrite);
	if(pEnt)
	{
		pEnt->erase(Adesk::kTrue);
		pEnt->close();
	}
	return solidId;
}
//ͨ����������ʵ��(Բ����)
AcDbObjectId CreateSolidByExtrude(AcDbBlockTableRecord *pBlockTableRecord,double fRadius,f3dPoint center,f3dPoint norm,
									f3dLine path,COLORREF crSolid,THANDLE handle/*=NULL*/)
{
	int nIndex = GetNearestACI(crSolid);
	AcGePoint3d org(center.x,center.y,center.z);
	AcGeVector3d nrm(norm.x,norm.y,norm.z);
	AcDbCircle *pCirc = new AcDbCircle(org,nrm,fRadius);
	AcDbVoidPtrArray lines,regions;
	lines.append((void *)pCirc);
	if(Acad::eOk != AcDbRegion::createFromCurves(lines,regions))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("part with handle 0X%x may be designed wrongly!",handle);
#else
		logerr.Log("���Ϊ0X%x�Ĺ��������������!",handle);
#endif
		return NULL;
	}
	pCirc->close();
	//����·��
	AcGePoint3d lineStart,lineEnd;
	Cpy_Pnt(lineStart,path.startPt);
	Cpy_Pnt(lineEnd,path.endPt);
	AcDbLine *pPath = new AcDbLine(lineEnd,lineStart);
	pPath->setColorIndex(nIndex);	//������������ɫ
	//������·��д�����¼
	AcDbObjectId pathId,solidId;
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pathId, pPath))
		pPath->close();
	//��������ʵ��
	AcDbRegion *pRegion = AcDbRegion::cast((AcRxObject*)regions[0]);
	AcDb3dSolid *p3dObj = new AcDb3dSolid;
	if(Acad::eOk != p3dObj->extrudeAlongPath(pRegion,pPath))
		return NULL;
	p3dObj->setColorIndex(nIndex);
	if(pBlockTableRecord->appendAcDbEntity(solidId,p3dObj)==Acad::eOk)
	{
		if(handle!=NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId,xrecObjId;
			p3dObj->createExtensionDictionary();
			dictObjId = p3dObj->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();

			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		p3dObj->close();//���ùرն���ĳ�Ա����
	}
	//ɾ��������
	AcDbEntity *pEnt=NULL;
	acdbOpenObject(pEnt,pathId,AcDb::kForWrite);
	if(pEnt)
	{
		pEnt->erase(Adesk::kTrue);
		pEnt->close();
	}
	return solidId;
}
//ͨ����ת����ʵ��
AcDbObjectId CreateSolidByRevolve(AcDbBlockTableRecord *pBlockTableRecord,fPtList &vertex_list,f3dPoint axisPoint,
								  f3dPoint axisDir,double angle,COLORREF crSolid,THANDLE handle/*=NULL*/)
{
	//����ʵ�����
	AcDb3dSolid *p3dObj = new AcDb3dSolid;
	//��������ת��
	AcGePoint3dArray pt3dTemp;
	int i=0;
	f3dPoint vec_start=vertex_list[1]-vertex_list[0];
	f3dPoint vec_end,plane_norm;
	int n = vertex_list.GetNodeNum();
	normalize(vec_start);
	/*for(i=2;i<n;i++)
	{
		vec_end=vertex_list[i]-vertex_list[0];
		plane_norm=vec_start^vec_end;
		if(plane_norm.IsZero())
			continue;
		else
			break;
	}*/
	vec_end = vertex_list[2]-vertex_list[0];
	plane_norm = vec_start^vec_end;
	pt3dTemp.setLogicalLength(n);
	for (i=0;i<n;i++)
	{
		if(i>2)
			project_point(vertex_list[i],vertex_list[0],plane_norm);
		pt3dTemp[i].set(vertex_list[i].x,vertex_list[i].y,vertex_list[i].z);
	}
	AcDb3dPolyline *poly = new AcDb3dPolyline(AcDb::k3dSimplePoly,pt3dTemp,Adesk::kTrue);
	AcDbVoidPtrArray lines,regions;
	lines.append((void *)poly);
	if (Acad::eOk != AcDbRegion::createFromCurves(lines,regions))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("the design of handle 0X%x part may be wrong!",handle);
#else
		logerr.Log("���Ϊ0X%x�Ĺ��������������!",handle);
#endif
		return NULL;
	}
	AcDbRegion *pRegion = AcDbRegion::cast((AcRxObject*)regions[0]);
	//��ת����ʵ��
	normalize(axisDir);
	if (Acad::eOk != p3dObj->revolve(pRegion,AcGePoint3d(axisPoint.x,axisPoint.y,axisPoint.z),
		AcGeVector3d(axisDir.x,axisDir.y,axisDir.z),angle))	
		return NULL;
	//����ʵ����ɫ
	int nIndex = GetNearestACI(crSolid);
	p3dObj->setColorIndex(nIndex);
	//��ʵ��д�����¼
	AcDbObjectId solidId;
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,solidId,p3dObj))
	{
		if(handle!=NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId,xrecObjId;
			p3dObj->createExtensionDictionary();
			dictObjId = p3dObj->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();
			
			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		p3dObj->close();//���ùرն���ĳ�Ա����
	}
/*	ads_name ent_name;
	acdbGetAdsName(ent_name,solidId);
	acdbEntUpd(ent_name);
	ads_name solid_name;
	acdbEntLast(solid_name);
	acdbEntUpd(solid_name);
	struct resbuf *rb = acdbEntGet(solid_name);
	acdbEntMod(rb);
	acutRelRb(rb);*/
	return solidId;
}

void ZoomAcadView(SCOPE_STRU scope,double extendLen/*=100*/,double scale/*=1.0*/)
{
	if((scope.fMaxX-scope.fMinX)<=0||(scope.fMaxY-scope.fMinY)<=0)
		return;
	ads_point L_T,R_B;
	L_T[X]=scope.fMinX-extendLen;
	L_T[Y]=scope.fMaxY+extendLen;
	L_T[Z]=0;
	R_B[X]=scope.fMaxX+extendLen;
	R_B[Y]=scope.fMinY-extendLen;
	R_B[Z]=0;
	ZoomAcadView(AcGePoint2d(L_T[X],L_T[Y]),AcGePoint2d(R_B[X],R_B[Y]));
}
void ZoomAcadView(f2dRect rect,double extendLen/*=100*/,double scale/*=1.0*/)
{
	if(rect.Width()<=0||rect.Height()<=0)
		return;
	ads_point L_T,R_B;
	L_T[X]=rect.topLeft.x-extendLen;
	L_T[Y]=rect.topLeft.y+extendLen;
	L_T[Z]=0;
	R_B[X]=rect.bottomRight.x+2*extendLen;
	R_B[Y]=rect.bottomRight.y-extendLen;
	R_B[Z]=0;
	ZoomAcadView(AcGePoint2d(L_T[X],L_T[Y]),AcGePoint2d(R_B[X],R_B[Y]));
}
/******************************************************************
������:			ZoomAcadView
����:           ������ͼ���൱��Zoom Window���   
�������:		Pt1 -- ��ͼ���Ͻǵ�
				Pt2 -- ��ͼ���½ǵ�
				scale -- ��չ����,һ��Ϊ1.0
*****************************************************************/
void ZoomAcadView(AcGePoint2d Pt1,AcGePoint2d Pt2,double scale/*=1.0*/)
{
	AcGePoint2d CenterPt;
	//��X�����Y�����غϣ���Ϊ���⣬������SetAcadView����
	if ((fabs(Pt1.x-Pt2.x)<EPS)||(fabs(Pt1.y-Pt2.y)<EPS))
		return;
	//ȷ�����������ֱ�Ϊ���ϽǺ����½�
	if (Pt1.x>Pt2.x) 
	{
		double tmp;
		tmp = Pt1.x;
		Pt1.x = Pt2.x;
		Pt2.x = tmp;
	}
	if (Pt2.y>Pt1.y) 
	{
		double tmp;
		tmp = Pt1.y;
		Pt1.y = Pt2.y;
		Pt2.y = tmp;
	}
	//��ȡ��ǰDwgView�ĳߴ�
	CRect CADrect;
	acedGetAcadDwgView()->GetClientRect(&CADrect);
	double width,height,ratio;
	ratio = (double)(CADrect.right-CADrect.left)/(double)(CADrect.bottom-CADrect.top);
	if (fabs(ratio)<EPS)
		return;
	if ((Pt2.x-Pt1.x)/(Pt1.y-Pt2.y) > ratio) 
	{
		width = Pt2.x-Pt1.x;
		height = width/ratio;
	}
	else
	{
		height = Pt1.y-Pt2.y;
		width = height * ratio;
	}
	//���õ�ǰ��ͼ���ĵ�
	CenterPt.x = (Pt1.x+Pt2.x)/2;
	CenterPt.y = (Pt1.y+Pt2.y)/2;
	//�ı䵱ǰ��ͼ
	AcDbViewTableRecord pVwRec;
	pVwRec.setCenterPoint(CenterPt);
	pVwRec.setWidth(width * scale);
	pVwRec.setHeight(height * scale); 
	acedSetCurrentView( &pVwRec, NULL );
}

//Ϊָ����CAD���������չ��¼,���ر�pObj wht 11-05-26
AcDbObjectId InsertExtensionRecord(AcDbObject *pObj,long handle,int cls_id/*=0*/)
{
	if(handle<0x20||pObj==NULL)
		return 0;
	//������չ��¼
	AcDbObjectId dictObjId,xrecObjId;
	pObj->createExtensionDictionary();
	dictObjId = pObj->extensionDictionary();
	AcDbDictionary *pDict;
	acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
	AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
	pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
	pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
	pDict->close();
	
	struct resbuf *head;
	head = acutBuildList(AcDb::kDxfInt32, handle,AcDb::kDxfInt32, cls_id,0);
	pXrec->setFromRbChain(*head);
	acutRelRb(head);
	pXrec->close();
	return pObj->objectId();
}
//����ָ�����������չ��¼ wht 11-05-26
AcDbObjectId InsertExtensionRecord(AcDbBlockTableRecord *pBlockTableRecord,
								   f3dPoint point,long handle,int cls_id/*=0*/)
{
	if(handle<0x20)
		return 0;
	AcDbObjectId pointId;
	AcDbPoint *pPoint=new AcDbPoint(AcGePoint3d(point.x,point.y,point.z));
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
		InsertExtensionRecord(pPoint,handle,cls_id);
	pPoint->close();
	return pointId;
}

//ͼ�����,�������谴˳ʱ�����ʱ�뷽������ wht 11-07-21
AcDbObjectId CreateAcadHatch(AcDbBlockTableRecord *pBlockTableRecord,fPtList &vertexList,char *sHatchStyle,
							 double scale,double angle/*=0*/,COLORREF color/*=RGB(255,255,255)*/)
{
	if(vertexList.GetNodeNum()<=0)
		return 0;
	AcDbHatch *pHatch=new AcDbHatch();
	pHatch->setNormal(AcGeVector3d(0,0,1));	//���÷���
	pHatch->setElevation(0.0);				//���û�ͼ���
	pHatch->setAssociative(Adesk::kFalse);
	pHatch->setPatternScale(scale);			//�������ű���
#ifdef _ARX_2007
	pHatch->setPattern(AcDbHatch::kPreDefined,(ACHAR*)_bstr_t(sHatchStyle));	//�������ͼ��
#else
	pHatch->setPattern(AcDbHatch::kPreDefined,sHatchStyle);				//�������ͼ��
#endif
	pHatch->setHatchStyle(AcDbHatch::kNormal);	//���������ʽ
	//
	AcGePoint2dArray ptArr;
	AcGeDoubleArray ptBulgesArr;
	ptArr.setLogicalLength(vertexList.GetNodeNum()+1);
	ptBulgesArr.setLogicalLength(vertexList.GetNodeNum()+1);
	int i=0;
	f3dPoint *pPoint=NULL,*pFirstPt=NULL;
	for(pPoint=vertexList.GetFirst();pPoint;pPoint=vertexList.GetNext())
	{
		if(pFirstPt==NULL)
			pFirstPt=pPoint;
		ptBulgesArr[i]=0;
		ptArr[i].set(pPoint->x,pPoint->y);
		i++;
	}
	if(pFirstPt)
	{	//������ĩβ�ٴ���ӵ�һ���������γɷ������
		ptBulgesArr[i]=0;
		ptArr[i].set(pFirstPt->x,pFirstPt->y);
	}
	pHatch->appendLoop(AcDbHatch::kDefault,ptArr,ptBulgesArr);
	pHatch->evaluateHatch();	//���趨����������ʽ���
	pHatch->setPatternAngle(angle);			//�������ͼ����ת�Ƕ�
	int color_index=GetNearestACI(color);
	pHatch->setColorIndex(color_index);	//������������ɫ����
	AcDbObjectId HatchId;//�����ʶ��
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,HatchId,pHatch))//��ʵ��д�����¼
	{
		pHatch->close();//�رն���
		ads_name ent_name;
		acdbGetAdsName(ent_name,HatchId);
		acdbEntUpd(ent_name);
	}
	return HatchId;
}

//����CADʵ��ID����scope wht 11-08-01
//�����ı����������ֵ��׼�ڻ�ȡBOM������ʱ����Ҫ���˵���ֱ����ͼԪ wjh-2019.5.26
BOOL VerifyVertexByCADEntId(SCOPE_STRU &scope,AcDbObjectId entId,bool blFilterOnlyBomTblLine/*=false*/)
{
	AcDbEntity *pEnt=NULL;
	acdbOpenObject(pEnt,entId,AcDb::kForRead);
	if(pEnt==NULL)
		return FALSE;
	if(pEnt->isKindOf(AcDbLine::desc()))
	{
		AcDbLine *pLine=(AcDbLine*)pEnt;
		scope.VerifyVertex(f3dPoint(pLine->startPoint().x,pLine->startPoint().y,0));
		scope.VerifyVertex(f3dPoint(pLine->endPoint().x,pLine->endPoint().y,0));
	}
	else if(pEnt->isKindOf(AcDbPolyline::desc()))
	{
		AcDbPolyline *pPolyLine=(AcDbPolyline*)pEnt;
		int i=0,nVertex=pPolyLine->numVerts();
		for(i=0;i<nVertex;i++)
		{
			AcGePoint3d point;
			pPolyLine->getPointAt(i,point);
			scope.VerifyVertex(f3dPoint(point.x,point.y,0));
		}
	}
	else if(pEnt->isKindOf(AcDbPoint::desc())&&!blFilterOnlyBomTblLine)
	{	//��һ���������£�Ӧ������������չ����Χ wht 11-06-16
		AcDbPoint *pPoint=(AcDbPoint*)pEnt;
		f3dPoint base_pt;
		Cpy_Pnt(base_pt,pPoint->position());
		scope.VerifyVertex(f3dPoint(base_pt.x-5,base_pt.y,0));
		scope.VerifyVertex(f3dPoint(base_pt.x+5,base_pt.y,0));
		scope.VerifyVertex(f3dPoint(base_pt.x,base_pt.y-5,0));
		scope.VerifyVertex(f3dPoint(base_pt.x,base_pt.y+5,0));
	}
	else if(pEnt->isKindOf(AcDbCircle::desc())&&!blFilterOnlyBomTblLine)
	{
		AcDbCircle *pCir=(AcDbCircle*)pEnt;
		AcGePoint3d point=pCir->center();
		scope.VerifyVertex(f3dPoint(point.x+pCir->radius(),point.y,0));
		scope.VerifyVertex(f3dPoint(point.x-pCir->radius(),point.y,0));
		scope.VerifyVertex(f3dPoint(point.x,point.y+pCir->radius(),0));
		scope.VerifyVertex(f3dPoint(point.x,point.y-pCir->radius(),0));
	}
	else if(pEnt->isKindOf(AcDbText::desc())&&!blFilterOnlyBomTblLine)
	{	//��һ���������£�Ӧ������������չ����Χ wht 11-06-16
		AcDbText *pText=(AcDbText*)pEnt;
		f3dPoint base_pt; 
		Cpy_Pnt(base_pt,pText->position());
		scope.VerifyVertex(f3dPoint(base_pt.x-5,base_pt.y,0));
		scope.VerifyVertex(f3dPoint(base_pt.x+5,base_pt.y,0));
		scope.VerifyVertex(f3dPoint(base_pt.x,base_pt.y-5,0));
		scope.VerifyVertex(f3dPoint(base_pt.x,base_pt.y+5,0));
	}
	else if(pEnt->isKindOf(AcDbMText::desc())&&!blFilterOnlyBomTblLine)
	{	//ʵ�����´���Ҳ��׼ȷ��ֻ�����ڵ�ǰ�����������ڴ������ţ���һ��Ҳ����Ҫ̫��ȷ,������Ҫ���pMText->actualWidth()��������о�ȷ�Ľ� wjh-2017.8.26
		AcDbMText *pMText=(AcDbMText*)pEnt;
		f3dPoint base_pt; 
		Cpy_Pnt(base_pt,pMText->location());
		scope.VerifyVertex(f3dPoint(base_pt.x-5,base_pt.y,0));
		scope.VerifyVertex(f3dPoint(base_pt.x+5,base_pt.y,0));
		scope.VerifyVertex(f3dPoint(base_pt.x,base_pt.y-5,0));
		scope.VerifyVertex(f3dPoint(base_pt.x,base_pt.y+5,0));
	}
	else if(pEnt->isKindOf(AcDbRotatedDimension::desc())&&!blFilterOnlyBomTblLine)
	{
		AcDbRotatedDimension *pDim=(AcDbRotatedDimension*)pEnt;
		AcGePoint3d point=pDim->xLine1Point();
		scope.VerifyVertex(f3dPoint(point.x,point.y,0));
		point=pDim->xLine2Point();
		scope.VerifyVertex(f3dPoint(point.x,point.y,0));
		point=pDim->dimLinePoint();
		scope.VerifyVertex(f3dPoint(point.x,point.y,0));
	}
	else if(pEnt->isKindOf(AcDbAlignedDimension::desc())&&!blFilterOnlyBomTblLine)
	{
		AcDbAlignedDimension *pDim=(AcDbAlignedDimension*)pEnt;
		AcGePoint3d point=pDim->xLine1Point();
		scope.VerifyVertex(f3dPoint(point.x,point.y,0));
		point=pDim->xLine2Point();
		scope.VerifyVertex(f3dPoint(point.x,point.y,0));
		point=pDim->dimLinePoint();
		scope.VerifyVertex(f3dPoint(point.x,point.y,0));
	}
	pEnt->close();
	return TRUE;
}

CXhChar200 CreateGuidStr()
{
	GUID guid; 
	CXhChar200 szGUID; 
	if(S_OK==::CoCreateGuid(&guid)) 
	{
		szGUID.Printf("%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X" 
			,guid.Data1,guid.Data2,guid.Data3,guid.Data4[0],guid.Data4[1] ,guid.Data4[2],
			guid.Data4[3],guid.Data4[4],guid.Data4[5],guid.Data4[6],guid.Data4[7]); 
	}
	return szGUID;
}