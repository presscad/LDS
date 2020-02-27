#ifndef __LDS_MSG_DEF_H_
#define __LDS_MSG_DEF_H_

#define PERM_TOWER_ADMIN		0x80000000		//��������Ա���
//#define PERM_MODIFYING			0x40000000		//�����޸�Ȩ�޹�����
#define PACK_SIZE				1024			//��Ϣ����С
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
	long msg_length;//��Ϣ����
	long command_id;//�����ʶ
	BYTE src_code;	//Դ�ڵ���
}LDS_MSGHEAD;
typedef struct tagLDS_MSGBUF
{
	//��Ϣͷ
	long msg_length;//��Ϣ����
	long command_id;//�����ʶ
	BYTE src_code;	//Դ�ڵ���

	BYTE *lpBuffer;			//��Ϣ��
}LDS_MSGBUF;

typedef struct tagUser
{
	BOOL m_bInUse;				//���û�λ���Ƿ�������
	long m_nUserId;				//�û�ID
	char m_sUserName[51];		//�û�������
	HANDLE m_hServerListenPipe;	//�����������û�����Ĺܵ�ʵ�����
	HANDLE m_hServerSendPipe;	//���������û�������Ϣ�Ĺܵ�ʵ�����
	time_t m_tUpdateTime;		//���һ�θ���Ȩ�޵�ʱ��
}USER_STRU;

typedef struct tagBRIEF_USER
{
	BOOL m_bInUse;				//���û�λ���Ƿ�������
	long m_nUserId;				//�û�ID
	char m_sUserName[51];		//�û�������
}BRIEF_USER;


//���ڱ��ؼ��ܰ汾TMA���ʼ��ܹ�
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
	//��Ϣͷ
	long msg_length;//��Ϣ����
	long command_id;//�����ʶ
	BYTE src_code;	//Դ�ڵ���
	BYTE *lpBuffer;			//��Ϣ��
}TMADOGSERV_MSGBUF;

typedef struct tagTmaUser
{
	long m_nUserId;				//�û�ID
	char m_sUserName[51];		//�û�������
	char password[17];
	HANDLE m_hClientListenPipe;	//���������û�������Ϣ�Ĺܵ�ʵ�����
	DWORD dwPermFlag;
}TMAUSER_STRU;

#endif