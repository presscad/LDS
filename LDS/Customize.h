#pragma once
struct CUSTOMIZE
{
	static const int CLIENT_XEROFOX = 1;
	static const int CLIENT_NANTONG_HUACAN = 2;		//江苏南通华灿
	static const int CLIENT_QINGDAO_WUXIAO = 3;		//青岛武晓
	static const int CLIENT_CHONGQIN_SHUNTAI = 4;	//重庆顺泰
	static const int CLIENT_QINGDAO_DONGFANG = 5;	//青岛东方
	static const int CLIENT_GUANGZHOU_ZENGLI = 6;	//广州增立
	static int CURRENT;
public:
	CUSTOMIZE(void){CURRENT=0;}
};

