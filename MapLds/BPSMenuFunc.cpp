#include "StdAfx.h"
#if defined(__BP_ONLY_)||defined(__CARD_IMG_VIEWER_)
#include "StdArx.h"
#include "MenuFunc.h"
#include "BPSModel.h"
#include "OptimalSortDlg.h"
#include "LocalFeatureDef.h"
#include "XhLicAgent.h"
#include "RxTools.h"
#include "SysPara.h"
#include "UploadProcessCardDlg.h"
#include "ReConnServerDlg.h"
#include "objptr_list.h"
#include "ProcBarDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

struct ACAD_TEXT{
	AcDbObjectId entId;
	CXhChar100 sText;
	GEPOINT text_pos;
	double fTextHeight;
	CAngleProcessInfo *pAngleInfo;
	ACAD_TEXT(){pAngleInfo=NULL;}
	void Init(AcDbObjectId id,const char* text,GEPOINT &pos,double text_height){
		entId=id;
		sText.Copy(text);
		text_pos=pos;
		fTextHeight=text_height;
	}
};
struct ACAD_RECT{
	AcDbObjectId entId;
	POLYGON region;
	double fWidth;
	CAngleProcessInfo *pAngleInfo;
	ACAD_RECT(){pAngleInfo=NULL; fWidth=0;}
	void Init(AcDbPolyline *pPolyLine){
		if(pPolyLine==NULL)
			return;
		SCOPE_STRU scope;
		ARRAY_LIST<f3dPoint> vertex_list;
		entId=pPolyLine->id();
		int nNum=pPolyLine->numVerts();
		for(int iVertIndex = 0;iVertIndex<nNum;iVertIndex++)
		{
			AcGePoint3d location;
			pPolyLine->getPointAt(iVertIndex,location);
			vertex_list.append(f3dPoint(location.x,location.y,location.z));
			scope.VerifyVertex(f3dPoint(location.x,location.y,location.z));
		}
		region.CreatePolygonRgn(vertex_list.m_pData,vertex_list.GetSize());
		fWidth=scope.wide();
	}
};

/*CProcBarDlg *pProcDlg=NULL;
void DisplayProcess(int percent,const char *sTitle)
{
	if(percent>=100)
	{
		if(pProcDlg!=NULL)
		{
			pProcDlg->DestroyWindow();
			delete pProcDlg;
			pProcDlg=NULL;
		}
		return;
	}
	else if(pProcDlg==NULL)
		pProcDlg=new CProcBarDlg(NULL);
	if(pProcDlg->GetSafeHwnd()==NULL)
		pProcDlg->Create();
	static int prevPercent;
	if(percent!=0&&percent==prevPercent)
		return;	//���ϴν���һ�²���Ҫ����
	else
		prevPercent=percent;
	if(sTitle)
		pProcDlg->SetTitle(CString(sTitle));
	else
#ifdef AFX_TARG_ENU_ENGLISH
		pProcDlg->SetTitle("Process");
#else 
		pProcDlg->SetTitle("����");
#endif
	pProcDlg->Refresh(percent);
}*/

void DisplayProcess(int percent,const char *sTitle);
//������ȡ�Ǹ�
BOOL InitBpsModel(BOOL bEmptyModel)
{
	//if(!ValidateLocalizeFeature(FEATURE::BATCH_INTELLI_PRINT_PROCESSCARD))
	//	return FALSE;
	//1.ʶ��Ǹֹ��տ�ģ��
	CXhChar100 APP_PATH,sJgCardPath="D:\\BP�Ǹֹ��տ�.dwg";
	//GetAppPath(APP_PATH);
	//sJgCardPath.Printf("%sBP�Ǹֹ��տ�.dwg",(char*)APP_PATH);
	//if(strlen(sys.general_set.m_sJgCard)>0)
	//	sJgCardPath.Copy(sys.general_set.m_sJgCard);
	if(!CIdentifyManager::InitJgCardInfo(sJgCardPath))
	{
		AfxMessageBox(CXhChar100("�Ǹֹ��տ�ģ���ȡʧ��:%s!",(char*)sJgCardPath));
		return FALSE;
	}
	//2.ѡȡDWG�нǸֹ��տ�����ȡ�Ǹ���Ϣ
	if(bEmptyModel)
		BPSModel.Empty();
	int retCode=0;
	ads_name ent_sel_set;
#ifdef _ARX_2007
	if(bEmptyModel)	//���ģ����Ĭ��ѡ������ʵ�壬�����޸Ļ��߲鿴ʱ����Ҫѡ������ʵ�� wht 17-12-19
		retCode=acedSSGet(L"ALL",NULL,NULL,NULL,ent_sel_set);
	retCode=acedSSGet(L"",NULL,NULL,NULL,ent_sel_set);
#else
	if(bEmptyModel)
		retCode=acedSSGet("ALL",NULL,NULL,NULL,ent_sel_set);
	retCode=acedSSGet("",NULL,NULL,NULL,ent_sel_set);
#endif
	if(retCode==RTCAN)
	{	//�û���ESCȡ��
		acedSSFree(ent_sel_set);
		return FALSE;
	}
	CHashSet<AcDbObjectId> textIdHash;
	CHashSet<AcDbObjectId> polyLineIdHash;
	long i=0,ll;
	acedSSLength(ent_sel_set,&ll);
	if(ll<=0)
		return FALSE;
	//1.��CADʵ�����ͳ��
	CHashList<ACAD_TEXT> hashTextByEntId;		//����
	CXhPtrSet<ACAD_TEXT> partNoTextList;		//����
	CHashList<ACAD_RECT> hashRectByEntId;		//���ο�
	CHashList<ACAD_TEXT> hashAngleCardBlockRefByEntId;	//������
	DisplayProcess(0,"��������");
	for(i=0;i<ll;i++)
	{
		ads_name entname;
		acedSSName(ent_sel_set,i,entname);
		AcDbObjectId entId;
		acdbGetObjectId(entId,entname);
		AcDbEntity *pEnt=NULL;
		acdbOpenAcDbEntity(pEnt,entId,AcDb::kForRead);
		if(pEnt==NULL)
			continue;
		pEnt->close();
		f3dPoint entPt;
		BOOL bInBlockRef=TRUE;
		if(pEnt->isKindOf(AcDbText::desc()))
		{	//�Ǹֹ��տ��ǿ飬����"����"������ȡ�Ǹ���Ϣ
			AcDbText* pText=(AcDbText*)pEnt;
#ifdef _ARX_2007
			CXhChar50 sText(_bstr_t(pText->textString()));
#else
			CXhChar50 sText(pText->textString());
#endif
			ACAD_TEXT *pAcadText=hashTextByEntId.Add(entId.handle());
			pAcadText->Init(entId,sText,GEPOINT(pText->position().x,pText->position().y,0),pText->height());
			if(strstr(sText,"��")==NULL || strstr(sText,"��")==NULL)
				continue;
			partNoTextList.append(pAcadText);
		}
		else if(pEnt->isKindOf(AcDbMText::desc()))
		{	//�Ǹֹ��տ��ǿ飬����"����"������ȡ�Ǹ���Ϣ
			AcDbMText* pMText=(AcDbMText*)pEnt;
			ACAD_TEXT *pAcadText=hashTextByEntId.Add(entId.handle());
#ifdef _ARX_2007
			CXhChar50 sText(_bstr_t(pMText->contents()));
			pAcadText->Init(entId,sText,GEPOINT(pMText->location().x,pMText->location().y,0),pMText->height());
#else
			CXhChar50 sText(pMText->contents());
			pAcadText->Init(entId,sText,GEPOINT(pMText->location().x,pMText->location().y,0),pMText->textHeight());
#endif
			if(strstr(sText,"��")==NULL || strstr(sText,"��")==NULL)
				continue;
			partNoTextList.append(pAcadText);
		}
		else if(pEnt->isKindOf(AcDbBlockReference::desc()))
		{	//���ݽǸֹ��տ�����ȡ�Ǹ���Ϣ
			AcDbBlockTableRecord *pTempBlockTableRecord=NULL;
			AcDbBlockReference* pReference=(AcDbBlockReference*)pEnt;
			AcDbObjectId blockId=pReference->blockTableRecord();
			acdbOpenObject(pTempBlockTableRecord,blockId,AcDb::kForRead);
			if(pTempBlockTableRecord==NULL)
				continue;
			pTempBlockTableRecord->close();
			CXhChar50 sName;
#ifdef _ARX_2007
			ACHAR* sValue=new ACHAR[50];
			pTempBlockTableRecord->getName(sValue);
			sName.Copy((char*)_bstr_t(sValue));
			delete[] sValue;
#else
			char *sValue=new char[50];
			pTempBlockTableRecord->getName(sValue);
			sName.Copy(sValue);
			delete[] sValue;
#endif
			if(strcmp(sName,"JgCard")!=0)
				continue;
			ACAD_TEXT *pAcadText=hashAngleCardBlockRefByEntId.Add(entId.handle());
			pAcadText->Init(entId,sName,GEPOINT(pReference->position().x,pReference->position().y,0),0);
		}
		else if(pEnt->isKindOf(AcDbPolyline::desc()))
		{
			ACAD_RECT *pRect=hashRectByEntId.Add(entId.handle());
			pRect->Init((AcDbPolyline*)pEnt);
		}
		DisplayProcess((100*i)/ll,"��������");
	}
	acedSSFree(ent_sel_set);
	DisplayProcess(100,"��������");

	if(hashAngleCardBlockRefByEntId.GetNodeNum()<=0&&partNoTextList.GetNodeNum()<=0)
		return FALSE;
	//2.��ʼ���Ǹ���Ϣ
	int nNewJgCount=0;
	CHashStrList<CAngleProcessInfo*> hashJgPtrByPartLabel;
	for(CAngleProcessInfo *pJgInfo=BPSModel.EnumFirstJg();pJgInfo;pJgInfo=BPSModel.EnumNextJg())
		hashJgPtrByPartLabel.SetValue(pJgInfo->m_sPartNo,pJgInfo);
	//2.1 ���ݿ�����ʶ��Ǹ�
	i=0;
	int nSumCount=hashAngleCardBlockRefByEntId.GetNodeNum();
	DisplayProcess(0,"��ȡ�Ǹֹ��տ�");
	ARRAY_LIST<f3dPoint> vertex_list;
	for(ACAD_TEXT *pBlockRef=hashAngleCardBlockRefByEntId.GetFirst();pBlockRef;pBlockRef=hashAngleCardBlockRefByEntId.GetNext())
	{
		i++;
		DisplayProcess((100*i)/nSumCount,"��ȡ�Ǹֹ��տ�");
		nNewJgCount++;
#ifdef _WIN64
		CAngleProcessInfo* pJgInfo=BPSModel.AppendJgInfo(pBlockRef->entId.asOldId());
#else
		CAngleProcessInfo* pJgInfo=BPSModel.AppendJgInfo((Adesk::UInt32)pBlockRef->entId.handle());
#endif
		pJgInfo->keyId=pBlockRef->entId;
		pJgInfo->m_bInBlockRef=TRUE;
		pJgInfo->sign_pt=pBlockRef->text_pos;	//�Ǹֵ�ʶ���ǵ㣨�����Լ��ű�ע����б�ǣ�ͼ�����ͼ��ԭ����б�ǣ�
		pJgInfo->InitOrig();					//��ʼ���˽Ǹֹ��տ��빤�տ�ģ��Ķ�Ӧ��ϵ
		pJgInfo->m_bUpdatePng=TRUE;
		vertex_list.Empty();
		vertex_list.append(CIdentifyManager::GetLeftBtmPt()+pJgInfo->orig_pt);
		vertex_list.append(CIdentifyManager::GetLeftTopPt()+pJgInfo->orig_pt);
		vertex_list.append(CIdentifyManager::GetRightTopPt()+pJgInfo->orig_pt);
		vertex_list.append(CIdentifyManager::GetRightBtmPt()+pJgInfo->orig_pt);
		pJgInfo->CreateRgn(vertex_list);
		//
		for(ACAD_TEXT *pText=hashTextByEntId.GetFirst();pText;pText=hashTextByEntId.GetNext())
		{
			if(pText->pAngleInfo!=NULL||!pJgInfo->PtInAngleRgn(pText->text_pos))
				continue;
			pText->pAngleInfo=pJgInfo;
			pJgInfo->InitAngleInfo(pText->text_pos,pText->sText);
		}
		//�ڶ��ν�����ȡʱ,ͬһ�������տ�ͼ�����仯����ɾ��֮ǰ�Ĺ��� wht 17-12-18
		CAngleProcessInfo **ppOldJgInfo=hashJgPtrByPartLabel.GetValue(pJgInfo->m_sPartNo);
		if(ppOldJgInfo!=NULL&&pJgInfo->keyId!=(*ppOldJgInfo)->keyId)
#ifdef _WIN64
			BPSModel.DeleteJgInfo((*ppOldJgInfo)->keyId.asOldId());	//���¹��տ�����ʱ���������entId�����仯��Ҫɾ��ԭ�нǸ� wht 17-12-18
#else
			BPSModel.DeleteJgInfo((Adesk::UInt32)(*ppOldJgInfo)->keyId.handle());	//���¹��տ�����ʱ���������entId�����仯��Ҫɾ��ԭ�нǸ� wht 17-12-18
#endif
	}
	DisplayProcess(100,"��ȡ�Ǹֹ��տ�");
	//2.2 ����Ǹ����������������һ�£��ٸ��ݼ���ʶ��Ǹ�
	i=0;
	DisplayProcess(0,"��ȡ�Ǹֹ��տ�");
	nSumCount=partNoTextList.GetNodeNum();
	for(ACAD_TEXT *pText=partNoTextList.GetFirst();pText;pText=partNoTextList.GetNext())
	{
		i++;
		DisplayProcess((100*i)/nSumCount,"��ȡ�Ǹֹ��տ�");
		if(pText->pAngleInfo!=NULL)
			continue;
		CAngleProcessInfo **ppOldJgInfo=hashJgPtrByPartLabel.GetValue(pText->sText);
		nNewJgCount++;
#ifdef _WIN64
		if(ppOldJgInfo!=NULL&&pText->entId!=(*ppOldJgInfo)->keyId)
			BPSModel.DeleteJgInfo((*ppOldJgInfo)->keyId.asOldId());	//���¹��տ�����ʱ���������entId�����仯��Ҫɾ��ԭ�нǸ� wht 17-12-18
		CAngleProcessInfo* pJgInfo=BPSModel.AppendJgInfo(pText->entId.asOldId());
#else
		if(ppOldJgInfo!=NULL&&pText->entId!=(*ppOldJgInfo)->keyId)
			BPSModel.DeleteJgInfo((Adesk::UInt32)(*ppOldJgInfo)->keyId.handle());	//���¹��տ�����ʱ���������entId�����仯��Ҫɾ��ԭ�нǸ� wht 17-12-18
		CAngleProcessInfo* pJgInfo=BPSModel.AppendJgInfo((Adesk::UInt32)pText->entId.handle());
#endif
		pJgInfo->keyId=pText->entId;
		pJgInfo->m_bInBlockRef=FALSE;
		pJgInfo->sign_pt=pText->text_pos;	//�Ǹֵ�ʶ���ǵ㣨�����Լ��ű�ע����б�ǣ�ͼ�����ͼ��ԭ����б�ǣ�
		if(CIdentifyManager::GetPartNoFontSize()>0&&pText->fTextHeight>0)
			pJgInfo->m_fScale2Template=pText->fTextHeight/CIdentifyManager::GetPartNoFontSize();
		pJgInfo->InitOrig();				//��ʼ���˽Ǹֹ��տ��빤�տ�ģ��Ķ�Ӧ��ϵ
		pJgInfo->m_bUpdatePng=TRUE;
		//����PolyLine��ʼ�����տ�����
		double fMaxWidth=0;
		ARRAY_LIST<f3dPoint> vertexList;
		for(ACAD_RECT *pRect=hashRectByEntId.GetFirst();pRect;pRect=hashRectByEntId.GetNext())
		{
			if(pRect->region.PtInRgn(pJgInfo->sign_pt)==1&&pRect->fWidth>fMaxWidth)
			{
				pRect->pAngleInfo=pJgInfo;
				fMaxWidth=pRect->fWidth;
				vertexList.Empty();
				for(int i=0;i<pRect->region.GetVertexCount();i++)
					vertexList.append(pRect->region.GetVertexAt(i));
				pJgInfo->CreateRgn(vertexList);
				break;
			}
		}
		//��ʼ���Ǹ���Ϣ
		for(ACAD_TEXT *pText=hashTextByEntId.GetFirst();pText;pText=hashTextByEntId.GetNext())
		{
			if(pText->pAngleInfo!=NULL||!pJgInfo->PtInAngleRgn(pText->text_pos))
				continue;
			pText->pAngleInfo=pJgInfo;
			pJgInfo->InitAngleInfo(pText->text_pos,pText->sText);
		}
	}
	DisplayProcess(100,"��ȡ�Ǹֹ��տ�");
	if(nNewJgCount>0)
		BPSModel.m_iRetrieveBatchNo++;
	if(BPSModel.GetJgNum()<=0)
	{
#if _ARX_2007
		acutPrintf(L"\n��ȡ�Ǹ�DWG�ļ�ʧ��!");
#else
		acutPrintf("\n��ȡ�Ǹ�DWG�ļ�ʧ��!");
#endif
	}
	BPSModel.CorrectAngles();
	return TRUE;
}
#include "GlobalFunc.h"
CXhChar500 GetTempFileFolderPath(bool bEmpty)
{	//��ȡ��ʱ�ļ�·��
	CXhChar500 sSysPath;
	GetSysPath(sSysPath);
	CXhChar500 sTemp("%s\\PngTemp\\",(char*)sSysPath);
	//ȷ��·������
	CFileFind fileFind;
	BOOL bContinue=fileFind.FindFile(CXhChar500("%s*.*",(char*)sTemp));
	if(!bContinue)
		MakeDirectory(sTemp);
	else
	{
		while(bContinue)
		{
			bContinue=fileFind.FindNextFile();

			CString ss1 = fileFind.GetFileName();
			CString ss2 = fileFind.GetFilePath();
			if(fileFind.IsDots()||fileFind.IsDirectory())
				continue;
			DeleteFile(fileFind.GetFilePath());
		}
	}
	//ɾ����ʱ�ļ����е���������
	return sTemp;
}

bool PrintProcessCardToPNG(bool bEmptyPngFiles)
{	//�жϴ�ӡ���Ƿ����
	AcApLayoutManager *pLayMan = NULL;
	pLayMan = (AcApLayoutManager *) acdbHostApplicationServices()->layoutManager();
	AcDbLayout *pLayout = pLayMan->findLayoutNamed(pLayMan->findActiveLayout(TRUE),TRUE);
	if(pLayout==NULL)
		return false;
	pLayout->close();
	AcDbPlotSettings* pPlotSetting = (AcDbPlotSettings*)pLayout;
	CXhChar500 sPlotCfgName;
	Acad::ErrorStatus retCode;
#ifdef _ARX_2007
	const ACHAR* sValue;
	retCode=pPlotSetting->getPlotCfgName(sValue);
	sPlotCfgName.Copy((char*)_bstr_t(sValue));
#else
	char *sValue;
	retCode=pPlotSetting->getPlotCfgName(sValue);
	sPlotCfgName.Copy(sValue);
#endif
	if(retCode!=Acad::eOk)//||stricmp(sPlotCfgName,"PublishToWeb PNG.pc3")!=0) 
	{//��ȡ����豸����
#ifdef _ARX_2007
		acutPrintf(L"\n��ǰ�����ӡ�豸������,�����Ƚ���ҳ������!");
#else
		acutPrintf("\n��ǰ�����ӡ�豸������,�����Ƚ���ҳ������!");
#endif
		return false;
	}
	//����������ӡ
	CXhChar500 sTempPath=GetTempFileFolderPath(bEmptyPngFiles),sFilePath;
	SetCurrentDirectory(sTempPath);	//���õ�ǰ����·��
	CFileFind fileFind;
	for(CAngleProcessInfo *pInfo=BPSModel.EnumFirstJg();pInfo;pInfo=BPSModel.EnumNextJg())
	{
		if(!pInfo->m_bUpdatePng)
			continue;
		SCOPE_STRU scope=pInfo->GetCADEntScope();
		ads_point L_T,R_B;
		L_T[0]=scope.fMinX;
		L_T[1]=scope.fMaxY;
		L_T[2]=0;
		R_B[0]=scope.fMaxX;
		R_B[1]=scope.fMinY;
		R_B[2]=0;
		CString sLabel=pInfo->m_sPartNo;
		sLabel.Remove(' ');
		sFilePath.Printf("%s.png",sLabel);
		int nRetCode=0;
		pInfo->m_sCardPngFile.Copy(sFilePath);
		pInfo->m_sCardPngFilePath.Printf("%s\\%s",(char*)sTempPath,(char*)sFilePath);
		pInfo->m_bUpdatePng=FALSE;
#ifdef _ARX_2007
		acedCommand(RTSTR,L"CMDECHO",RTLONG,0,RTNONE);	//�����������Ƿ��������
		nRetCode=acedCommand(RTSTR,L"-plot",RTSTR,L"y",	//�Ƿ���Ҫ��ϸ��ӡ����[��(Y)/��(N)]
			RTSTR,L"",							//������
			RTSTR,L"PublishToWeb PNG.pc3",		//����豸������
			RTSTR,L"Sun Hi-Res (1600.00 x 1280.00 ����)",//ͼֽ�ߴ�:<A4>
			RTSTR,L"L",							//ͼ�η���:<����|����>
			RTSTR,L"N",							//�Ƿ����ӡ
			RTSTR,L"w",							//��ӡ����:<ָ������>
			RTPOINT,L_T,						//���Ͻ�
			RTPOINT,R_B,						//���½�
			RTSTR,L"",							//��ӡ����:<����>
			RTSTR,L"C",							//��ӡƫ�ƣ�<����>
			RTSTR,L"N",							//�Ƿ���ʽ��ӡ
			RTSTR,L".",							//��ӡ��ʽ����
			RTSTR,L"Y",							//��ӡ�߿�
			RTSTR,L"A",							//��ɫ��ӡ����
			RTSTR,(ACHAR*)_bstr_t(sFilePath),	//��ӡ���ļ�	//�˴��������·��ʱ��ʧ�ܣ���Ҫ����SetCurrentDirectory֮�󣬴˴�ֱ�������ļ��� wht 17-10-23
			RTSTR,L"N",							//�Ƿ񱣴��ҳ�����õ��޸�
			RTSTR,L"Y",							//�Ƿ������ӡ
			RTNONE);
#else
		acedCommand(RTSTR,"CMDECHO",RTLONG,0,RTNONE);		//�����������Ƿ��������
		nRetCode=acedCommand(RTSTR,"-plot",RTSTR,"y",	//�Ƿ���Ҫ��ϸ��ӡ����[��(Y)/��(N)]
			RTSTR,"",							//������
			RTSTR,"PublishToWeb PNG.pc3",		//����豸������
			RTSTR,"Sun Hi-Res (1600.00 x 1280.00 ����)",//ͼֽ�ߴ�:<A4>
			RTSTR,"L",							//ͼ�η���:<����|����>
			RTSTR,"N",							//�Ƿ����ӡ
			RTSTR,"w",							//��ӡ����:<ָ������>
			RTPOINT,L_T,						//���Ͻ�
			RTPOINT,R_B,						//���½�
			RTSTR,"",							//��ӡ����:<����>
			RTSTR,"C",							//��ӡƫ�ƣ�<����>
			RTSTR,"N",							//�Ƿ���ʽ��ӡ
			RTSTR,".",							//��ӡ��ʽ����
			RTSTR,"Y",							//��ӡ�߿�
			RTSTR,"A",							//��ɫ��ӡ����
			RTSTR,(char*)sFilePath,					//��ӡ���ļ�
			RTSTR,"N",							//�Ƿ񱣴��ҳ�����õ��޸�
			RTSTR,"Y",							//�Ƿ������ӡ
			RTNONE);
#endif
	}
	return true;
}

BOOL WriteOrReadInfoFromReg(LPCTSTR lpszSection, LPCTSTR lpszEntry,BOOL bWrite,char *sValue)	
{
	char sSubKey[MAX_PATH]="";
	DWORD dwDataType,dwLength=MAX_PATH;
	sprintf(sSubKey,"Software\\Xerofox\\BPS\\%s",lpszSection);
	HKEY hKey;
	int nRetCode=RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_READ|KEY_WRITE,&hKey);
	if(hKey==NULL&&bWrite)
	{
		DWORD dw;
		HKEY hSoftwareKey;
		RegOpenKeyEx(HKEY_CURRENT_USER,"Software",0,KEY_WRITE,&hSoftwareKey);
		if(hSoftwareKey!=NULL)
		{
			RegCreateKeyEx(hSoftwareKey, CXhChar100("Xerofox\\BPS\\%s",lpszSection), 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL, &hKey, &dw);
			RegCloseKey(hSoftwareKey);
		}
	}
	BOOL bRetCode = FALSE;
	if(hKey!=NULL)
	{
		if(bWrite)
		{
			dwLength=strlen(sValue);
			nRetCode=RegSetValueEx(hKey,lpszEntry,NULL,REG_SZ,(BYTE*)&sValue[0],dwLength);
			bRetCode=(nRetCode==ERROR_SUCCESS);
		}
		else 
		{
			nRetCode=RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&sValue[0],&dwLength); 
			bRetCode=(nRetCode==ERROR_SUCCESS);
		}
		RegCloseKey(hKey);
	}
	return bRetCode;
}

#ifdef __CONNECT_REMOTE_SERVER_
#include "TMS.h"
BOOL LogonTMSServer()
{
	CReConnServerDlg logdlg;
	logdlg.m_bInputPassword=TRUE;
	CXhChar200 sServerName,sUserName,sPassword;
	WriteOrReadInfoFromReg("Port","ServerName",FALSE,sServerName);
	WriteOrReadInfoFromReg("Port","UserName",FALSE,sUserName);
	WriteOrReadInfoFromReg("Port","Password",FALSE,sPassword);
	logdlg.m_sServerName=sServerName;
	logdlg.m_sUserName=sUserName;
	logdlg.m_sPassword=sPassword;
	CAcModuleResourceOverride resOverride;
	if(logdlg.DoModal()==IDOK)
	{
		WriteOrReadInfoFromReg("Port","ServerName",TRUE,logdlg.m_sServerName.GetBuffer());
		WriteOrReadInfoFromReg("Port","UserName",TRUE,logdlg.m_sUserName.GetBuffer());
		WriteOrReadInfoFromReg("Port","Password",TRUE,logdlg.m_sPassword.GetBuffer());
		if(logdlg.m_sServerName.GetLength()>0)
			TMS.SetServerUrl(logdlg.m_sServerName.Trim());
		BOOL bRetCode=TMS.LoginUser(logdlg.m_sUserName,logdlg.m_sPassword);
		return bRetCode;
	}
	else 
		return FALSE;
}

void AppendOrUpdateProcessCards()
{
	if(TMS.m_pServer==NULL)
	{
		if(!LogonTMSServer())
			AfxMessageBox("��¼������ʧ��!");
	}
	if(InitBpsModel(FALSE))
		PrintProcessCardToPNG(false);
	CUploadProcessCardDlg dlg;
	CAcModuleResourceOverride resOverride;
	dlg.DoModal();
}
void RetrievedProcessCards()
{
	if(TMS.m_pServer==NULL)
	{
		if(!LogonTMSServer())
			AfxMessageBox("��¼������ʧ��!");
	}
	//1.��DWG����ȡ���տ���Ϣ
	if(InitBpsModel(TRUE))
	{	//2.�����տ���Ϣ����ΪͼƬ�ļ�
		PrintProcessCardToPNG(true);
	}
	CUploadProcessCardDlg dlg;
	CAcModuleResourceOverride resOverride;
	dlg.DoModal();
}
#endif
#ifdef __BP_ONLY_
void BatchPrintPartProcessCards()
{
	//1.��DWG����ȡ�Ǹֹ��տ�
	InitBpsModel(TRUE);
	//2.����������ӡ
	OptimalSortSet();	
}

//�Ż���������
void OptimalSortSet()
{
	if(!ValidateLocalizeFeature(FEATURE::BATCH_INTELLI_PRINT_PROCESSCARD))
		return;
	COptimalSortDlg dlg;
	CAcModuleResourceOverride resOverride;
	if(dlg.DoModal()==IDOK)
	{
		//�жϴ�ӡ���Ƿ����
		AcApLayoutManager *pLayMan = NULL;
		pLayMan = (AcApLayoutManager *) acdbHostApplicationServices()->layoutManager();
		AcDbLayout *pLayout = pLayMan->findLayoutNamed(pLayMan->findActiveLayout(TRUE),TRUE);
		if(pLayout==NULL)
			return;
		pLayout->close();
		AcDbPlotSettings* pPlotSetting = (AcDbPlotSettings*)pLayout;
		CXhChar500 sPlotCfgName;
		Acad::ErrorStatus retCode;
#ifdef _ARX_2007
		const ACHAR* sValue;
		retCode=pPlotSetting->getPlotCfgName(sValue);
		sPlotCfgName.Copy((char*)_bstr_t(sValue));
#else
		char *sValue;
		retCode=pPlotSetting->getPlotCfgName(sValue);
		sPlotCfgName.Copy(sValue);
#endif
		if(retCode!=Acad::eOk||stricmp(sPlotCfgName,"��")==0) 
		{//��ȡ����豸����
#ifdef _ARX_2007
			acutPrintf(L"\n��ǰ�����ӡ�豸������,�����Ƚ���ҳ������!");
#else
			acutPrintf("\n��ǰ�����ӡ�豸������,�����Ƚ���ҳ������!");
#endif
			return;
		}
		//����������ӡ
		for(SCOPE_STRU* pScopy=dlg.m_xPrintScopyList.GetFirst();pScopy;pScopy=dlg.m_xPrintScopyList.GetNext())
		{
			ads_point L_T,R_B;
			L_T[0]=pScopy->fMinX;
			L_T[1]=pScopy->fMaxY;
			L_T[2]=0;
			R_B[0]=pScopy->fMaxX;
			R_B[1]=pScopy->fMinY;
			R_B[2]=0;
#ifdef _ARX_2007
			acedCommand(RTSTR,L"CMDECHO",RTLONG,0,RTNONE);		//�����������Ƿ��������
			acedCommand(RTSTR,L"-plot",RTSTR,L"y",	//�Ƿ���Ҫ��ϸ��ӡ����[��(Y)/��(N)]
				RTSTR,L"",							//������
				RTSTR,L"",							//����豸������
				RTSTR,L"",							//ͼֽ�ߴ�:<A4>
				RTSTR,L"",							//ͼֽ��λ:<����>
				RTSTR,L"",							//ͼ�η���:<����|����>
				RTSTR,L"",							//�Ƿ����ӡ
				RTSTR,L"w",							//��ӡ����:<ָ������>
				RTPOINT,L_T,RTPOINT,R_B,			//���Ͻǡ����½�
				RTSTR,L"",							//��ӡ����:<����>
				RTSTR,L"",							//��ӡƫ�ƣ�<����>
				RTSTR,L"",							//�Ƿ���ʽ��ӡ
				RTSTR,L"",							//��ӡ��ʽ����
				RTSTR,L"",							//��ӡ�߿�
				RTSTR,L"",							//��ɫ��ӡ����
				RTSTR,L"",							//��ӡ���ļ�
				RTSTR,L"",							//�Ƿ񱣴�ҳ�����ø���
				RTSTR,L"",							//�Ƿ������ӡ
				RTNONE);
#else
			acedCommand(RTSTR,"CMDECHO",RTLONG,0,RTNONE);		//�����������Ƿ��������
			acedCommand(RTSTR,"-plot",RTSTR,"y",	//�Ƿ���Ҫ��ϸ��ӡ����[��(Y)/��(N)]
				RTSTR,"",							//������
				RTSTR,"",							//����豸������
				RTSTR,"",							//ͼֽ�ߴ�:<A4>
				RTSTR,"",							//ͼֽ��λ:<����>
				RTSTR,"",							//ͼ�η���:<����|����>
				RTSTR,"",							//�Ƿ����ӡ
				RTSTR,"w",							//��ӡ����:<ָ������>
				RTPOINT,L_T,RTPOINT,R_B,			//���Ͻǡ����½�
				RTSTR,"",							//��ӡ����:<����>
				RTSTR,"",							//��ӡƫ�ƣ�<����>
				RTSTR,"",							//�Ƿ���ʽ��ӡ
				RTSTR,"",							//��ӡ��ʽ����
				RTSTR,"",							//��ӡ�߿�
				RTSTR,"",							//��ɫ��ӡ����
				RTSTR,"",							//��ӡ���ļ�
				RTSTR,"",							//�Ƿ񱣴�ҳ�����ø���
				RTSTR,"",							//�Ƿ������ӡ
				RTNONE);
#endif
		}
	}
}
#endif
#endif