#pragma once
struct CUSTOMIZE
{
	static const int CLIENT_XEROFOX = 1;
	static const int CLIENT_NANTONG_HUACAN = 2;		//������ͨ����
	static const int CLIENT_QINGDAO_WUXIAO = 3;		//�ൺ����
	static const int CLIENT_CHONGQIN_SHUNTAI = 4;	//����˳̩
	static const int CLIENT_QINGDAO_DONGFANG = 5;	//�ൺ����
	static const int CLIENT_GUANGZHOU_ZENGLI = 6;	//��������
	static int CURRENT;
public:
	CUSTOMIZE(void){CURRENT=0;}
};

