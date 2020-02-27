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
	//CBuffer::CBuffer(srcBuf,buf_size);����ֱ�ӵ���CBuffer::CBuffer
	AttachMemory(srcBuf,buf_size);

	hashPartIndexPos.Empty();
	mem_cursor = 0;
	//��ȡ�ĸ��ֽڣ��ж��Ƿ�Ϊ�汾��
	ReadInteger(&version);
	DWORD i,count=0;
	if(!(version&0x80000000))
		count=version;	//û�а汾��
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
	//��ȡ�ĸ��ֽڣ��ж��Ƿ�Ϊ�汾��
	CProcessPart::BUFFER_VERSION v;
	ReadInteger(&v.version);
	//version==1000001��ʹ��,�˴��迼�Ǽ���
	//�Ժ�汾�������ֽ�����Ϊ0xFF,��ʾ�а汾��
	if(v.version!=1000001&&v.bytes[3]!=0xFF)
		SeekPosition(begin_pos);	//û�а汾����ת����ʼλ��
	else
		v.bytes[3]=0;
	if(v.version>=1000005)
		Offset(4);//ReadInteger(&m_verSerial.uiVersion);	//����������Ϣ�İ汾��ˮ��>=V1000005
	BYTE cPartType=*(buffer+mem_cursor);
	if(v.version>=1000005)
	{
		mem_cursor+=24;	//��ת�����ӻ�������
		int nAttachBuff=ReadInteger();
		for(int i=0;i<nAttachBuff;i++)
		{
			DWORD idBuff=0,nBuffSize=0;
			idBuff=ReadInteger();
			nBuffSize=ReadInteger();
			if(idBuff==1&&sPartLabel)	//�������ӻ���
			{
				long currPos=this->CursorPosition;
				ReadString(sPartLabel,nMaxBuffLen);
				mem_cursor=currPos;
			}
			Offset(nBuffSize);
		}
	}
	else
	{	//2.����������Ϣ
		UINT uStrLen=0;
		mem_cursor+=24;	//��ת������λ��(CProcessPart::ToPPIBuffer)
		if(sPartLabel)
			ReadString(sPartLabel,nMaxBuffLen);
		else
		{	//�������������Ϣ
			uStrLen=ReadStringLength();
			Offset(uStrLen);
		}
		//�������������Ϣ
		uStrLen=ReadStringLength();
		Offset(uStrLen);
		//����������ע��Ϣ
		uStrLen=ReadStringLength();
		Offset(uStrLen);
		if(v.version>=1000004)
		{	//���������ļ���m_sRelatePartNo������һ���ŵ����
			uStrLen=ReadStringLength();
			Offset(uStrLen);
		}
		//3.������Ϣ
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
