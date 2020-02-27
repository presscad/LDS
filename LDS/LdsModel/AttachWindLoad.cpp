// AttachWindLoad.cpp: implementation of the CAttachWindLoad class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tower.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAttachWindLoad::CAttachWindLoad()
{
	handle=iNo=0;
	m_fAttachWeight=0;
	m_fProfileCoef=m_fShieldArea=m_fShieldCoef=m_fBetaZ=m_fBetaZForFoundation=1.0;
	strcpy(name,"");
	nodeList=new ATOM_LIST<long>;
}

CAttachWindLoad::~CAttachWindLoad()
{
	delete nodeList;
}

void CAttachWindLoad::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	buffer.ReadInteger(&iNo);
	buffer.ReadString(name,50);
	buffer.ReadDouble(&m_fAttachWeight);
	buffer.ReadDouble(&m_fProfileCoef);
	buffer.ReadDouble(&m_fBetaZ);
	if(version==0||(doc_type==3&&version>=1070309)||(doc_type==4&&version>=1000309))
		buffer.ReadDouble(&m_fBetaZForFoundation);
	buffer.ReadDouble(&m_fShieldArea);
	buffer.ReadDouble(&m_fShieldCoef);
	long i,n,h;
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		nodeList->append(h);
	}
}
void CAttachWindLoad::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteInteger(iNo);
	buffer.WriteString(name);
	buffer.WriteDouble(m_fAttachWeight);
	buffer.WriteDouble(m_fProfileCoef);
	buffer.WriteDouble(m_fBetaZ);
	buffer.WriteDouble(m_fBetaZForFoundation);
	buffer.WriteDouble(m_fShieldArea);
	buffer.WriteDouble(m_fShieldCoef);
	BUFFERPOP stack(&buffer,nodeList->GetNodeNum());
	buffer.WriteInteger(nodeList->GetNodeNum());
	for(long* pH=nodeList->GetFirst();pH;pH=nodeList->GetNext())
	{
		buffer.WriteInteger(*pH);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of node is wrong!");
#else
		CLDSObject::Log2File()->Log("附加风荷载节点数出现错误!");
#endif

}
void CAttachWindLoad::ToXmlFile(FILE* fp,DWORD schema)
{
	char sText[500]="";
	fprintf(fp,"<附加风荷载信息 No=\"%d\" name=\"%s\">\n",iNo,name);
	sprintf(sText,"%f",m_fAttachWeight);
	SimplifiedNumString(sText);
	fprintf(fp,"<附加重量 AttachWeight=\"%s\"/>\n",sText);
	sprintf(sText,"%f",m_fProfileCoef);
	SimplifiedNumString(sText);
	fprintf(fp,"<体型系数 ProfileCoef=\"%s\"/>\n",sText);
	sprintf(sText,"%f",m_fBetaZ);
	SimplifiedNumString(sText);
	fprintf(fp,"<风振系数 BetaZ=\"%s\"/>\n",sText);
	sprintf(sText,"%f",m_fBetaZForFoundation);
	SimplifiedNumString(sText);
	fprintf(fp,"<基础风振系数 BetaZForFoundation=\"%s\"/>\n",sText);
	sprintf(sText,"%f",m_fShieldArea);
	SimplifiedNumString(sText);
	fprintf(fp,"<挡风面积 shieldArea=\"%s\"/>\n",sText);
	sprintf(sText,"%f",m_fShieldCoef);
	SimplifiedNumString(sText);
	fprintf(fp,"<挡风系数 shieldCoef=\"%s\"/>\n",sText);
	fprintf(fp,"<荷载节点数量 nodeNum=\"%d\"/>\n",nodeList->GetNodeNum());
	for(long* pH=nodeList->GetFirst();pH;pH=nodeList->GetNext())
		fprintf(fp,"<荷载节点信息 id=\"%d\"/>\n",*pH);
	fprintf(fp,"</附加风荷载信息>\n");
}
