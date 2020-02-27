#ifndef __LDS_MSG_DEF_H_
#define __LDS_MSG_DEF_H_

#define PERM_TOWER_ADMIN		0x80000000		//铁塔管理员身份
//#define PERM_MODIFYING			0x40000000		//构件修改权限过程中
#define PACK_SIZE				1024			//信息包大小
enum CSCW_ID{
	//Client-->Server
	REGISTER_LDS_USER	=0x00000001,	//By CorePipe
	LOGOUT_LDS_USER		=0x00000002,	//By CorePipe
	ASK_NEW_HANDLE		=0x00000003,	//By PipeServerListen
	ASK_DELIVER_MSG		=0x00000004,	//By PipeServerListen
	ASK_DATA_CHECK_IN	=0x00000005,	//By PipeServerListen
	ASK_DATA_CHECK_OUT	=0x00000006,	//By PipeServerListen
	ASK_USER_INFO		=0x00000007,
	ASK_PERM_MODIFY_IN	=0x00000008,
	ASK_PERM_MODIFY_OUT	=0x00000009,
	ASK_PRODUCT_VERSION	=0x00000010,
	//Server-->Client
	NOTIFY_MSG			=0x10000001,
	NOTIFY_DATA_MODIFIED=0x10000002,
	NOTIFY_PERM_MODIFIED=0x00000003,
};

typedef struct tagLDS_MSGHEAD
{
	long msg_length;//消息长度
	long command_id;//命令标识
	BYTE src_code;	//源节点编号
}LDS_MSGHEAD;
typedef struct tagLDS_MSGBUF
{
	//消息头
	long msg_length;//消息长度
	long command_id;//命令标识
	BYTE src_code;	//源节点编号

	BYTE *lpBuffer;			//消息体
}LDS_MSGBUF;

typedef struct tagUser
{
	BOOL m_bInUse;				//此用户位置是否有人用
	long m_nUserId;				//用户ID
	char m_sUserName[51];		//用户的名称
	HANDLE m_hServerListenPipe;	//服务器监听用户请求的管道实例句柄
	HANDLE m_hServerSendPipe;	//服务器向用户发送消息的管道实例句柄
	time_t m_tUpdateTime;		//最近一次更新权限的时间
}USER_STRU;

typedef struct tagBRIEF_USER
{
	BOOL m_bInUse;				//此用户位置是否有人用
	long m_nUserId;				//用户ID
	char m_sUserName[51];		//用户的名称
}BRIEF_USER;


//用于本地加密版本TMA访问加密狗
enum TMADOGSERV_CMD{
	//Client-->Server
	LOGIN_TMADOG			=0x00000001,
	LOGOUT_TMADOG			=0x00000002,	//By CorePipe
	ASK_SERVER_VERSION		=0x00000003,
	VERIFY_VERSION			=0x00000004,	//By PipeServerListen
	ASK_ENCRYPT_KEY			=0x00000005,	//By PipeServerListen
	ASK_DECRYPT_KEY			=0x00000006,	//By PipeServerListen
	ASK_FALSE_KEY			=0x00000007,	//By PipeServerListen
	ASK_DOG_PERM_STATE		=0x00000008,
	ASK_EXPIRE_TIMES		=0x00000009,
	ASK_PIPE_LINE_NO		=0x0000000A,
	DECREASE_EXPIRE_TIMES	=0x0000000B,
	MODIFY_USER_PASSWORD	=0x0000000C,
	//Server-->Client
	ASK_LIVE_STATUS		=0x10000001,
};

typedef struct tagTMADOGSERV_MSGBUF
{
	//消息头
	long msg_length;//消息长度
	long command_id;//命令标识
	BYTE src_code;	//源节点编号
	BYTE *lpBuffer;			//消息体
}TMADOGSERV_MSGBUF;

typedef struct tagTmaUser
{
	long m_nUserId;				//用户ID
	char m_sUserName[51];		//用户的名称
	char password[17];
	HANDLE m_hClientListenPipe;	//服务器向用户发送消息的管道实例句柄
	DWORD dwPermFlag;
}TMAUSER_STRU;

#endif