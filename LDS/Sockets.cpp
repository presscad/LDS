#include "StdAfx.h"
#include "Sockets.h"
#include <winsock2.h>

CSockets::CSockets()
{
	m_hSocket = INVALID_SOCKET;
}
CSockets::~CSockets(){}
BOOL CSockets::InitSocketLib(int lowver,int higver)
{
	WORD wVersion =0 ;
	int errret = -1;
	WSADATA wsaData;
	wVersion = MAKEWORD(lowver,higver);
	errret = WSAStartup(wVersion,&wsaData);
	if( LOBYTE( wsaData.wVersion) != SOCKET_LOWEVER ||HIBYTE( wsaData.wVersion) !=SOCKER_HIGVER)
		return FALSE;
	return TRUE;
}
/* ʹ��TCP/IPЭ��
 * strBindIp Ĭ�ϼ����������е�ͨ��
 */
SOCKET CSockets::InitSocket(u_short bindPort,CXhChar16 strBindIp/*=""*/,BOOL bServer/*=TRUE*/)
{
	SOCKET socketId = INVALID_SOCKET;
	//��ʼ��ͨ�Ű汾��
	if(!InitSocketLib(SOCKET_LOWEVER,SOCKER_HIGVER))
		return FUN_LIB_ERR;
	//����һ���ܽ�������ͨ�ŵ��׽���
	socketId = socket(AF_INET,SOCK_STREAM,0);
	if(socketId==SOCKET_ERROR)
		return FUN_SCOKET_ERR;
	SOCKADDR_IN sockStruct;
	sockStruct.sin_family = AF_INET; //ʹ��TCP/IPЭ��
	if(strBindIp.Length()==0 )
		sockStruct.sin_addr.S_un.S_addr = INADDR_ANY;
	else
		sockStruct.sin_addr.S_un.S_addr = inet_addr(strBindIp);
	sockStruct.sin_port = htons(bindPort);
	if(!bServer) //�ͻ���
	{
		if(connect(socketId,(LPSOCKADDR)&sockStruct,sizeof(sockStruct)) == SOCKET_ERROR)
			return FUN_CONNECT_ERR;
		else
			m_hSocket = socketId;
	}
	else //if( bServer)  //�����
	{
		if(bind(socketId,(sockaddr*)&sockStruct,sizeof(sockaddr_in)) == SOCKET_ERROR)
			return FUN_BIND_ERR;
		else if( listen(socketId,SOMAXCONN) == SOCKET_ERROR )
			return FUN_LISTEN_ERR;
		else
			return(m_hSocket=socketId);
	}
	return socketId;
}
SOCKET CSockets::NetAccept(SOCKET s,sockaddr addr/*=NULL*/,int addrLen/*=0*/)
{
	SOCKET accpSocket = INVALID_SOCKET;
	accpSocket = accept(s,(struct sockaddr* )&addr,&addrLen);
	if (accpSocket)
		return FUN_ACCEPT_ERR;
	return accpSocket;
}
int CSockets::NetRecv(SOCKET sock, char *buf, int len)
{
	int res=recv(sock, buf, len, 0);
	if(res==SOCKET_ERROR||res<0)
		return FUN_RECV_ERR;
	return res;
}
int CSockets::NetSend(SOCKET sock, const char *buf, int len)
{
	int res=send( sock, buf, len,0);
	if(res==SOCKET_ERROR||res<0)
		return FUN_SEND_ERR;
	return  res;
}
CXhChar100  CSockets::GetErrCode(int nFunType)
{
	CXhChar100 sErr;
	DWORD dwErrCode=WSAGetLastError();;
	if(nFunType==CSockets::FUN_LIB_ERR)
		sErr.Printf("winsocket��汾��");
	else if(nFunType==CSockets::FUN_SCOKET_ERR)
	{
		if(dwErrCode==WSAEAFNOSUPPORT)
			sErr.Printf("socket()�����룺%d,��֧��ָ���ĵ�ַ�壡",dwErrCode);
		else if(dwErrCode==WSAEINPROGRESS)
			sErr.Printf("socket()�����룺%d,һ���������׽ӿڵ������������У�",dwErrCode);
		else if(dwErrCode==WSAEMFILE)
			sErr.Printf("socket()�����룺%d,�޿����ļ������֣�",dwErrCode);
		else if(dwErrCode==WSAENOBUFS)
			sErr.Printf("socket()�����룺%d,�޿��û��������޷������׽ӿڣ�",dwErrCode);
		else if(dwErrCode==WSAEPROTONOSUPPORT)
			sErr.Printf("socket()�����룺%d,��֧��ָ����Э�飡",dwErrCode);
		else if(dwErrCode==WSAEPROTOTYPE)
			sErr.Printf("socket()�����룺%d,ָ����Э�鲻�����ڱ��׽ӿڣ�",dwErrCode);
		else if(dwErrCode==WSAESOCKTNOSUPPORT)
			sErr.Printf("socket()�����룺%d,����ַ���в�֧�ָ������׽ӿڣ�",dwErrCode);
		else 
			sErr.Printf("socket()�����룺%d",dwErrCode);
	}
	else if(nFunType==CSockets::FUN_SEND_ERR)
	{
		if(dwErrCode==WSAEINTR)
			sErr.Printf("send()�����룺%d��ͨ��һ��WSACancelBlockingCall()��ȡ��һ���������ģ����ã�",dwErrCode);
		else if(dwErrCode==WSAEFAULT)
			sErr.Printf("send()�����룺%d��buf���������û���ַ�ռ��е���Чλ�ã�",dwErrCode);
		else if(dwErrCode==WSAENETRESET)
			sErr.Printf("send()�����룺%d������WINDOWS�׽ӿ�ʵ�ַ��������ӣ��ʸ����ӱ��豻��λ��",dwErrCode);
		else if(dwErrCode==WSAENOBUFS)
			sErr.Printf("send()�����룺%d��WINDOWS�׽ӿ�ʵ�ֱ���һ��������������",dwErrCode);
		else if(dwErrCode==WSAENOTCONN)
			sErr.Printf("send()�����룺%d���׽ӿ�δ�����ӣ�",dwErrCode);
		else if(dwErrCode==WSAEINVAL)
			sErr.Printf("send()�����룺%d���׽ӿ�δ��bind()����",dwErrCode);
		else if(dwErrCode==WSAECONNABORTED)
			sErr.Printf("send()�����룺%d�����ڳ�ʱ������ԭ���������·���жϣ�",dwErrCode);
		else 
			sErr.Printf("send()�����룺%d",dwErrCode);
	}
	else if(nFunType==CSockets::FUN_RECV_ERR)
	{
		if(dwErrCode==WSAESHUTDOWN)
			sErr.Printf("recv()�����룺%d���׽ӿ��ѱ��رգ�",dwErrCode);
		else if(dwErrCode==WSAEINVAL)
			sErr.Printf("recv()�����룺%d���׽ӿ�δ��bind()��������",dwErrCode);
		//else if(dwErrCode==WSAENOTCONN)
			//sErr.Printf("�׽ӿ�δ���ӣ�",dwErrCode);
		else if(dwErrCode==WSANOTINITIALISED)
			sErr.Printf("recv()�����룺%d��û�е���WSAStartup()����ʧ�ܣ�",dwErrCode);
		//else if(dwErrCode==WSAENOBUFS)
			//sErr.Printf("WINDOWS�׽ӿ�ʵ�ֱ���һ��������������";
		else if(dwErrCode==WSAENOTCONN)
			sErr.Printf("recv()�����룺%d���׽ӿ�δ�����ӣ�",dwErrCode);
		else if(dwErrCode==WSAEINVAL)
			sErr.Printf("recv()�����룺%d���׽ӿ�δ��bind()����",dwErrCode);
		else if(dwErrCode==WSAECONNABORTED)
			sErr.Printf("recv()�����룺%d�����ڳ�ʱ������ԭ���������·���жϣ�",dwErrCode);
		else 
			sErr.Printf("recv()�����룺%d",dwErrCode);
	}
	else if(nFunType==CSockets::FUN_CONNECT_ERR)
	{
		if (dwErrCode==WSAEADDRINUSE)
			sErr.Printf("connect()�����룺%d����ַ�Ѿ�ʹ��",dwErrCode);
		else if (dwErrCode==WSAEISCONN)
			sErr.Printf("connect()�����룺%d���׽ӿ���������",dwErrCode);
		else if (dwErrCode==WSAEFAULT)
			sErr.Printf("connect()�����룺%d��namelen��������ȷ",dwErrCode);
		else if (dwErrCode==WSAEINVAL)
			sErr.Printf("connect()�����룺%d���׽ӿ�û��׼������һ��ַ����",dwErrCode);
		else if (dwErrCode==WSAENETUNREACH)
			sErr.Printf("connect()�����룺%d����ǰ�޷��ӱ�������������",dwErrCode);
		else if (dwErrCode==WSAECONNREFUSED)
			sErr.Printf("connect()�����룺%d�����ӳ��Ա�ǿ�ƾܾ�",dwErrCode);
		else if (dwErrCode==WSAEINPROGRESS)
			sErr.Printf("connect()�����룺%d��һ���������׽ӿڵ�������������",dwErrCode);
		else if (dwErrCode==WSAEADDRNOTAVAIL)
			sErr.Printf("connect()�����룺%d���ڱ��ػ������Ҳ�����ָ�ĵ�ַ",dwErrCode);
		else 
			sErr.Printf("connect()�����룺%d",dwErrCode);
	}
	else if(nFunType==CSockets::FUN_LISTEN_ERR)
	{
		if(dwErrCode==WSAEADDRINUSE)
			sErr.Printf("listen()�����룺%d,��ͼ��listen()ȥ����һ������ʹ���еĵ�ַ��",dwErrCode);
		else if(dwErrCode==WSAEINPROGRESS)
			sErr.Printf("listen()�����룺%d,һ���������׽ӿڵ������������У�",dwErrCode);
		else if(dwErrCode==WSAEINVAL)
			sErr.Printf("listen()�����룺%d,���׽ӿ�δ��bind()�������󣬻��ѱ����ӣ�",dwErrCode);
		else if(dwErrCode==WSAEISCONN)
			sErr.Printf("listen()�����룺%d,�׽ӿ��ѱ����ӣ�",dwErrCode);
		else if(dwErrCode==WSAEMFILE)
			sErr.Printf("listen()�����룺%d,�޿����ļ������֣�",dwErrCode);
		else if(dwErrCode==WSAENOTSOCK)
			sErr.Printf("listen()�����룺%d,�����ֲ���һ���׽ӿڣ�",dwErrCode);
		else if(dwErrCode==WSAENOBUFS)
			sErr.Printf("listen()�����룺%d,�޿��û������ռ䣡",dwErrCode);
		else if(dwErrCode==WSAEOPNOTSUPP)
			sErr.Printf("listen()�����룺%d,���׽ӿڲ�����listen()���ã�",dwErrCode);
		else
			sErr.Printf("listen()�����룺%d",dwErrCode);
	}
	else if(nFunType==CSockets::FUN_ACCEPT_ERR)
	{
		if (dwErrCode==WSAEFAULT)
			sErr.Printf("accept()�����룺%d,addrlen����̫С��С��socket�ṹ�Ĵ�С��",dwErrCode);
		else if (dwErrCode==WSAEINTR)
			sErr.Printf("accept()�����룺%d,ͨ��һ��WSACancelBlockingCall()��ȡ��һ���������ģ�����",dwErrCode);
		else if (dwErrCode==WSAEINPROGRESS)
			sErr.Printf("accept()�����룺%d,һ���������׽ӿڵ�������������",dwErrCode);
		else if (dwErrCode==WSAEINVAL)
			sErr.Printf("accept()�����룺%d,��accept()ǰδ����listen()",dwErrCode);
		else if (dwErrCode==WSAEMFILE)
			sErr.Printf("accept()�����룺%d,����accept()ʱ����Ϊ�գ��޿��õ�������",dwErrCode);
		else if (dwErrCode==WSAENOBUFS)
			sErr.Printf("accept()�����룺%d,�޿��û������ռ�",dwErrCode);
		else if (dwErrCode==WSAENOTSOCK)
			sErr.Printf("accept()�����룺%d,�����ֲ���һ���׽ӿ�",dwErrCode);
		else if (dwErrCode==WSAEOPNOTSUPP)
			sErr.Printf("accept()�����룺%d,���׽ӿ����Ͳ�֧���������ӷ���",dwErrCode);
		else if (dwErrCode==WSAEWOULDBLOCK)
			sErr.Printf("accept()�����룺%d,���׽ӿ�Ϊ��������ʽ�������ӿɹ�����",dwErrCode);
		else if (dwErrCode==WSAECONNRESET)
			sErr.Printf("accept()�����룺%d,�������Ӻ���accept����֮ǰ����Զ�̿ͻ��˶Ͽ�����",dwErrCode);
		else
			sErr.Printf("accept()�����룺%d",dwErrCode);
	}
	return sErr;
}
#ifdef __ALFA_TEST_VERSION_
void TestServerSocket()
{
	CSockets s;
	SOCKET soket=s.InitSocket(TRUE,8888,"192.168.2.1");
	//if(soket<0)
		//���� 
	while(TRUE)
	{
		SOCKET acceptSoket=s.NetAccept(soket);
		char str[100]="";
		if(s.NetRecv(acceptSoket,str,sizeof(str))<0)
		{//���� 
			closesocket(soket);
			return;
		}
		if(s.NetRecv(acceptSoket,str,sizeof(str))<0)
		{
			closesocket(soket);
			return;
		}
		CXhChar100 str1="asdffds";
		if(s.NetSend(acceptSoket,str1,sizeof(str1))<0)
		{
			closesocket(soket);
			return;
		}
		closesocket(acceptSoket);
	}
	closesocket(soket);
}
void TestClientSocket()
{
	CSockets socket;
	SOCKET socket1=socket.InitSocket(FALSE,8888,"192.168.2.1");
	if(socket<0)
	{
		//socket.GetErrCode(CSockets::FUN_SCOKET_ERR);
		//����
		return;
	}
	CXhChar100 a ="1234";
	if(socket.NetSend(socket1,a,sizeof(a))<0)
	{
		closesocket(soket);
		return;
	}
	a ="789";
	if(socket.NetSend(socket1,a,sizeof(a))<0)
	{
		closesocket(soket);
		return;
	}
	a="";
	if(socket.NetRecv(socket1,a,sizeof(a))<0)
	{
		closesocket(soket);
		return;
	}
	closesocket(socket.m_Socket);
}
#endif