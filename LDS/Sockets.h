#include "StdAfx.h"
#include "XhCharString.h"
class CSockets
{
public:
	UINT m_hSocket;
	CSockets();
	~CSockets();
	static const int SOCKET_LOWEVER        = 2;
	static const int SOCKER_HIGVER         = 2;
	//
	
	static const int FUN_SEND_ERR       = -2;
	static const int FUN_RECV_ERR       = -3;
	static const int FUN_CONNECT_ERR    = -4;
	static const int FUN_BIND_ERR       = -5;
	static const int FUN_LISTEN_ERR     = -6;
	static const int FUN_LIB_ERR		= -8;
	static const int FUN_ACCEPT_ERR		= -9;
	static const int FUN_SCOKET_ERR		= -10;
	SOCKET InitSocket(u_short bindPort,CXhChar16 strBindIp="",BOOL bServer=TRUE);
	int NetRecv(SOCKET sock, char *buf, int len);
	int NetSend(SOCKET sock, const char *buf, int len);
	SOCKET NetAccept(SOCKET s,sockaddr addr/*=NULL*/,int addrLen=0);
	//
	CXhChar100 GetErrCode(int nFunType);
private:
	BOOL InitSocketLib(int lowver,int higver );
};