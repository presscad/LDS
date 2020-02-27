#include "stdafx.h"
#include "stdio.h"
#include "..\LDS.h"
#include "..\MainFrm.h"
#include "..\EmendBoltGasket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#if defined(__ALFA_TEST_VERSION_) && defined(__SMART_DRAW_INC_)
void ExportEmendInfoToFile()
{
	//�����������ļ�������ָ���ļ�
	char file_version[20],product_version[20];
	theApp.GetAppVersion(file_version,product_version);
	CBuffer buffer(10000000);
	Ta.ToBuffer(buffer,DogSerialNo(),file_version);
	//�����Խ�����浽ָ���ļ�
	ATOM_LIST<COutputInfo> infoList;
	CEmendBoltGasket emendBoltGasket;
	emendBoltGasket.Execute();
	emendBoltGasket.InitOutputInfoList(&infoList);
	int nNum=infoList.GetNodeNum();
	buffer.WriteInteger(nNum);
	for(COutputInfo* pInfo=infoList.GetFirst();pInfo;pInfo=infoList.GetNext())
	{	
		buffer.WriteInteger(pInfo->m_uType);
		buffer.WriteInteger(pInfo->m_hRelaObj1);
		buffer.WriteInteger(pInfo->m_hRelaObj2);
		buffer.WriteString(pInfo->m_sDescription.GetBuffer());
	}
	FILE* fp=fopen("D:\\bolt_gasket.txt","wb");
	if(fp==NULL)
	{
		AfxMessageBox("�Ҳ���ָ�����ļ�!");
		return;
	}
	fwrite(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
	fclose(fp);	
}

BOOL Test_OutPutInfoList(FILE *fp,ATOM_LIST<COutputInfo> &infoList,ATOM_LIST<COutputInfo> &test_infoList)
{
	if(test_infoList.GetNodeNum()!=infoList.GetNodeNum())
	{
		fprintf(fp,"��������Ϣ��������Խ����Ϣ������ͬ!\n");
		return FALSE;
	}
	int index=0;
	for(COutputInfo* pTestInfo=test_infoList.GetFirst();pTestInfo;pTestInfo=test_infoList.GetNext())
	{
		COutputInfo* pInfo=infoList.GetByIndex(index);
		if(pTestInfo->m_sDescription.Compare(pInfo->m_sDescription)!=0)
		{
			CXhChar100 sText;
			sText.Printf("%s\n",pInfo->m_sDescription.GetBuffer());
			fprintf(fp,sText);
			return FALSE;
		}
		index++;
	}
	return TRUE;
}
BOOL Test_EmendBoltGasket(FILE* pFile)
{
	//��ȡ������Ϣ�Ͳ��������Ϣ
	FILE* fp=fopen("D:\\bolt_gasket.txt","rb");
	if(fp==NULL)
	{
		AfxMessageBox("�Ҳ���ָ�����ļ�!");
		return FALSE;
	}
	fseek(fp,0,SEEK_END);
	long len=ftell(fp);
	CBuffer buffer(len);
	char* bytes=new char[len];
	fseek(fp,0,SEEK_SET);
	fread(bytes,1,len,fp);
	fclose(fp);
	buffer.Write(bytes,len);
	buffer.SeekToBegin();
	Ta.FromBuffer(buffer,4);	//��ȡ����Ϣ
	//��ȡ���Խ����Ϣ
	ATOM_LIST<COutputInfo> test_infoList;
	int nNum=0;
	buffer.ReadInteger(&nNum);
	for(int i=0;i<nNum;i++)
	{
		COutputInfo *pInfo=test_infoList.append();
		int nType=0;
		buffer.ReadInteger(&nType);
		pInfo->m_uType=INFO_EMEND_BOLTGASKET;
		buffer.ReadInteger(&pInfo->m_hRelaObj1);
		buffer.ReadInteger(&pInfo->m_hRelaObj2);
		char sDescription[100];
		buffer.ReadString(sDescription);
		pInfo->m_sDescription.Format("%s",sDescription);
	}
	fclose(fp);
	//�ж�����ͼʱ��ռȥһЩ������������ɵ�Ƭ�ľ����ͨ���������ɵľ����һ��
	((CMainFrame*)AfxGetMainWnd())->GetTowerTreePage()->RefreshTreeCtrl();
	//2��ȡ��Ҫ��ӵ�Ƭ����˨��Ϣ����,�����бȽ�
	fprintf(pFile,"\t�Զ��޸���˨��Ƭ���Խ��\n");
	CEmendBoltGasket emendBoltGasket;
	emendBoltGasket.Execute();
	ATOM_LIST<COutputInfo> infoList;
	emendBoltGasket.InitOutputInfoList(&infoList);
	return Test_OutPutInfoList(pFile,infoList,test_infoList);
}
#endif