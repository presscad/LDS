#include "StdAfx.h"
#include "..\lds\CryptBuffer.h"

//��ȡ�ļ�����ָ���ֽ�Ϊָ������
void CorrectFileSpecByte(CBuffer &buffer,long pos,BYTE b)
{
	if(buffer.SeekPosition(pos))
		buffer.WriteByte(b);	//�޸�
}
void RemoveBytes(CBuffer &buffer,DWORD startPos,DWORD removeLen)
{
	if(startPos<0||startPos>buffer.GetLength())
		return;
	CBuffer tempBuffer;
	buffer.SeekToBegin();
	tempBuffer.Write(buffer,buffer.GetLength());

	tempBuffer.SeekToBegin();
	buffer.ClearBuffer();
	buffer.Write(tempBuffer.GetBufferPtr(),startPos);
	tempBuffer.SeekPosition(startPos+removeLen);
	buffer.Write(tempBuffer.GetCursorBuffer(),tempBuffer.GetLength()-tempBuffer.GetCursorPosition());
}
//�޸��汾�Ŷ�ʧ������
void CorrectTowerBufferVersion(CBuffer &buffer,char* version)
{
	CBuffer tempBuffer(buffer.GetLength());
	tempBuffer.Write(buffer.GetBufferPtr(),buffer.GetLength());
	buffer.ClearContents();
	buffer.SeekToBegin();
	buffer.WriteString("������������");
	buffer.WriteString(version);
	//
	tempBuffer.SeekToBegin();
	char sTemp[100]="";
	tempBuffer.ReadString(sTemp);	//��ȡ�ļ�����
	tempBuffer.ReadString(sTemp);	//��ȡ�汾��
	buffer.Write(tempBuffer.GetCursorBuffer(),tempBuffer.GetRemnantSize());
}
//�޸�CTower��Ӧ��Buffer���ڵĴ�������
void CorrectFileError(CString sFilePath)
{
	//���ļ���ȡģ������
	CFile original_file;
	if(!original_file.Open(sFilePath, CFile::modeRead))
		return;
	CString sDocTypeName;
	DWORD user_pipeline_no=0;
	CArchive original_ar(&original_file,CArchive::load);
	original_ar>>sDocTypeName;
	CString sVersion="";
	original_ar>>sVersion;
	original_ar>>user_pipeline_no;
	DWORD cursor_pipeline_no=0;
	original_ar>>cursor_pipeline_no;
	DWORD buffer_len=0;
	original_ar>>buffer_len;
	BYTE *pBuf = new BYTE[buffer_len];
	original_ar.Read(pBuf,buffer_len);
	CBuffer model_buffer;
	model_buffer.Write(pBuf,buffer_len);
	DecryptBuffer(model_buffer,FALSE);	//����
	CorrectFileSpecByte(model_buffer,46095,0x0);
	RemoveBytes(model_buffer,46099,30);
	//CorrectFileSpecByte(model_buffer,579085,0x5B);
	//RemoveBytes(model_buffer,11464617,3640);
	//CorrectTowerBufferVersion(model_buffer,"4.1.2.0");
	EncryptBuffer(model_buffer,FALSE);	//����
	delete []pBuf;
	original_ar.Close();
	original_file.Close();
	//�����ļ�
	CFile file;
	if(!file.Open(sFilePath, CFile::modeWrite))
		return;
	file.SetLength(0);	//����ļ�����
	CArchive ar(&file,CArchive::store);
	ar<<sDocTypeName;
	sVersion="4.1.1.2";
	ar<<sVersion;
	ar<<user_pipeline_no;
	ar<<cursor_pipeline_no;
	//����ģ������
	buffer_len=model_buffer.GetLength();
	ar<<buffer_len;
	ar.Write(model_buffer.GetBufferPtr(),buffer_len);
	ar.Flush(); //����������д���ļ���
	ar.Close();
	file.Close();
}

//�������ļ������ַ���
void ModifyDocTypeName(CString sFilePath,CString sDocTypeName)
{	//���ļ���ȡģ������
	CFile original_file;
	if(!original_file.Open(sFilePath, CFile::modeRead))
		return;
	CString ss;
	CArchive original_ar(&original_file,CArchive::load);
	original_ar>>ss;
	CString sVersion="";
	original_ar>>sVersion;
	DWORD user_pipeline_no=0;
	original_ar>>user_pipeline_no;
	DWORD cursor_pipeline_no=0;
	original_ar>>cursor_pipeline_no;
	DWORD buffer_len=0;
	original_ar>>buffer_len;
	BYTE *pBuf = new BYTE[buffer_len];
	original_ar.Read(pBuf,buffer_len);
	CBuffer model_buffer;
	model_buffer.Write(pBuf,buffer_len);
	delete []pBuf;
	original_ar.Close();
	original_file.Close();
	//�����ļ�
	CFile file;
	if(!file.Open(sFilePath, CFile::modeWrite))
		return;
	file.SetLength(0);	//����ļ�����
	CArchive ar(&file,CArchive::store);
	ar<<sDocTypeName;
	ar<<sVersion;
	ar<<user_pipeline_no;
	ar<<cursor_pipeline_no;
	//����ģ������
	buffer_len=model_buffer.GetLength();
	ar<<buffer_len;
	ar.Write(model_buffer.GetBufferPtr(),buffer_len);
	ar.Flush(); //����������д���ļ���
	ar.Close();
	file.Close();
}