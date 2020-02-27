#include "StdAfx.h"
#include "..\lds\CryptBuffer.h"

//读取文件修正指定字节为指定内容
void CorrectFileSpecByte(CBuffer &buffer,long pos,BYTE b)
{
	if(buffer.SeekPosition(pos))
		buffer.WriteByte(b);	//修改
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
//修复版本号丢失的问题
void CorrectTowerBufferVersion(CBuffer &buffer,char* version)
{
	CBuffer tempBuffer(buffer.GetLength());
	tempBuffer.Write(buffer.GetBufferPtr(),buffer.GetLength());
	buffer.ClearContents();
	buffer.SeekToBegin();
	buffer.WriteString("铁塔制造助手");
	buffer.WriteString(version);
	//
	tempBuffer.SeekToBegin();
	char sTemp[100]="";
	tempBuffer.ReadString(sTemp);	//读取文件类型
	tempBuffer.ReadString(sTemp);	//读取版本号
	buffer.Write(tempBuffer.GetCursorBuffer(),tempBuffer.GetRemnantSize());
}
//修改CTower对应的Buffer存在的错误数据
void CorrectFileError(CString sFilePath)
{
	//打开文件读取模型数据
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
	DecryptBuffer(model_buffer,FALSE);	//解密
	CorrectFileSpecByte(model_buffer,46095,0x0);
	RemoveBytes(model_buffer,46099,30);
	//CorrectFileSpecByte(model_buffer,579085,0x5B);
	//RemoveBytes(model_buffer,11464617,3640);
	//CorrectTowerBufferVersion(model_buffer,"4.1.2.0");
	EncryptBuffer(model_buffer,FALSE);	//加密
	delete []pBuf;
	original_ar.Close();
	original_file.Close();
	//保存文件
	CFile file;
	if(!file.Open(sFilePath, CFile::modeWrite))
		return;
	file.SetLength(0);	//清空文件长度
	CArchive ar(&file,CArchive::store);
	ar<<sDocTypeName;
	sVersion="4.1.1.2";
	ar<<sVersion;
	ar<<user_pipeline_no;
	ar<<cursor_pipeline_no;
	//保存模型数据
	buffer_len=model_buffer.GetLength();
	ar<<buffer_len;
	ar.Write(model_buffer.GetBufferPtr(),buffer_len);
	ar.Flush(); //将缓存数据写入文件中
	ar.Close();
	file.Close();
}

//仅修正文件类型字符串
void ModifyDocTypeName(CString sFilePath,CString sDocTypeName)
{	//打开文件读取模型数据
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
	//保存文件
	CFile file;
	if(!file.Open(sFilePath, CFile::modeWrite))
		return;
	file.SetLength(0);	//清空文件长度
	CArchive ar(&file,CArchive::store);
	ar<<sDocTypeName;
	ar<<sVersion;
	ar<<user_pipeline_no;
	ar<<cursor_pipeline_no;
	//保存模型数据
	buffer_len=model_buffer.GetLength();
	ar<<buffer_len;
	ar.Write(model_buffer.GetBufferPtr(),buffer_len);
	ar.Flush(); //将缓存数据写入文件中
	ar.Close();
	file.Close();
}