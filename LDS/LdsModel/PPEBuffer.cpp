#include "stdafx.h"
#include "PPEBuffer.h"

CPPEBuffer::CPPEBuffer(char* srcBuf, DWORD buf_size)
{
	AttachPPEBuffer(srcBuf,buf_size);
	m_nBlockSize = buf_size;
	m_bExternalBuf=(srcBuf!=NULL);
	m_bExternalPosStack=true;
	m_pPosStack=NULL;

}

CPPEBuffer::~CPPEBuffer(void)
{
}
bool CPPEBuffer::AttachPPEBuffer(char* srcBuf, DWORD buf_size)
{
	//CBuffer::CBuffer(srcBuf,buf_size);不能直接调用CBuffer::CBuffer
	AttachMemory(srcBuf,buf_size);

	hashPartIndexPos.Empty();
	mem_cursor = 0;
	//读取四个字节，判断是否为版本号
	ReadInteger(&version);
	DWORD i,count=0;
	if(!(version&0x80000000))
		count=version;	//没有版本号
	else
		count=ReadInteger();
	CProcessPart part;
	CXhChar16 label;
	for(i=0;i<count;i++)
	{
		DWORD pos=mem_cursor;
		SkipProcessPartBuffer(label,label.LengthMax);
		hashPartIndexPos.SetValue(label,pos);
	}
	return count>0;
}
DWORD CPPEBuffer::SkipProcessPartBuffer(char* sPartLabel/*=NULL*/,int nMaxBuffLen/*=0*/)
{
	DWORD begin_pos=GetCursorPosition();
	//读取四个字节，判断是否为版本号
	CProcessPart::BUFFER_VERSION v;
	ReadInteger(&v.version);
	//version==1000001已使用,此处需考虑兼容
	//以后版本第三个字节设置为0xFF,表示有版本号
	if(v.version!=1000001&&v.bytes[3]!=0xFF)
		SeekPosition(begin_pos);	//没有版本号跳转至开始位置
	else
		v.bytes[3]=0;
	if(v.version>=1000005)
		Offset(4);//ReadInteger(&m_verSerial.uiVersion);	//构件工艺信息的版本流水号>=V1000005
	BYTE cPartType=*(buffer+mem_cursor);
	if(v.version>=1000005)
	{
		mem_cursor+=24;	//跳转至附加缓存区域
		int nAttachBuff=ReadInteger();
		for(int i=0;i<nAttachBuff;i++)
		{
			DWORD idBuff=0,nBuffSize=0;
			idBuff=ReadInteger();
			nBuffSize=ReadInteger();
			if(idBuff==1&&sPartLabel)	//基础附加缓存
			{
				long currPos=this->CursorPosition;
				ReadString(sPartLabel,nMaxBuffLen);
				mem_cursor=currPos;
			}
			Offset(nBuffSize);
		}
	}
	else
	{	//2.构件附加信息
		UINT uStrLen=0;
		mem_cursor+=24;	//跳转至件号位置(CProcessPart::ToPPIBuffer)
		if(sPartLabel)
			ReadString(sPartLabel,nMaxBuffLen);
		else
		{	//跳过构件编号信息
			uStrLen=ReadStringLength();
			Offset(uStrLen);
		}
		//跳过构件规格信息
		uStrLen=ReadStringLength();
		Offset(uStrLen);
		//跳过构件备注信息
		uStrLen=ReadStringLength();
		Offset(uStrLen);
		if(v.version>=1000004)
		{	//跳过关联的件号m_sRelatePartNo（处理一板多号的情况
			uStrLen=ReadStringLength();
			Offset(uStrLen);
		}
		//3.工艺信息
		DWORD dwProcessBufLen=0;
		if(version>=1000005)
		{
			int i,nAttachBuff=0;
			ReadInteger(&nAttachBuff);
			for(i=0;i<nAttachBuff;i++)
			{
				long idBuff=ReadInteger();
				ReadInteger(&dwProcessBufLen);
				Offset(dwProcessBufLen);
			}
		}
		else
		{
			ReadInteger(&dwProcessBufLen);
			Offset(dwProcessBufLen);
		}
	}
	return mem_cursor-begin_pos;
}
bool CPPEBuffer::GetProcessPart(const char* sPartLabel,CProcessPart* pProcessPart)
{
	long *pPosIndex=hashPartIndexPos.GetValue(sPartLabel);
	if(pPosIndex==NULL)
		return false;
	mem_cursor=*pPosIndex;
	if(pProcessPart)
		pProcessPart->FromPPIBuffer(*this);
	return true;
}
