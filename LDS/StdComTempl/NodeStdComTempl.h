#pragma once
//单线：02 //实体：03
//连接点一级分组：
//1.横担连接点
//2.曲臂K点
//3.变坡连接点
//4.塔身K点
//5.交叉节点
//6.挂线连接点
//7.塔腿V面连接点
//8.辅助材连接点
//连接点二级分组(后四位):首字母ASCII码的16进制+Index
//B  42//D 44//F 46//G 47//H  48//J 4A//T 54//Z 5A

//单线
//横担节点  CORSS_ARM
static const DWORD STDTEMPL_CONNPOINT_HD1			= 0x02010001;//水平横担与地线支架内侧连接点（基准水平材非断）
static const DWORD STDTEMPL_CONNPOINT_HD1A			= 0x02010011;//水平横担与地线支架内侧连接点（基准水平材断开）
static const DWORD STDTEMPL_CONNPOINT_HD2			= 0x02010002;//水平横担与地线支架外侧连接点(基准水平材)
static const DWORD STDTEMPL_CONNPOINT_HD3			= 0x02010003;//酒杯塔上曲臂外侧与横担外连接点(基准水平材)
static const DWORD STDTEMPL_CONNPOINT_HD3_G5		= 0x02010004;//酒杯塔上曲臂外侧与横担外连接点(基准水平杆件非断)
static const DWORD STDTEMPL_CONNPOINT_HD4			= 0x02010005;//猫头塔曲臂内拐点处
static const DWORD STDTEMPL_CONNPOINT_HD5			= 0x02010006;//猫头塔曲臂外拐点处(上)
static const DWORD STDTEMPL_CONNPOINT_HD6			= 0x02010007;//猫头塔曲臂外拐点处(下)
static const DWORD STDTEMPL_CONNPOINT_HD7			= 0x02010008;//塔身与水平横担下部连接点
static const DWORD STDTEMPL_CONNPOINT_HD8			= 0x02010009;//塔身与水平横担上部连接点
static const DWORD STDTEMPL_CONNPOINT_HD9			= 0x0201000a;//塔头与水平横担连接点
static const DWORD STDTEMPL_CONNPOINT_HD10			= 0x0201000b;//横担塔身(hd_10)
static const DWORD STDTEMPL_CONNPOINT_XH_HD3A		= 0x0201000c;//曲臂横担内连接点4~7根
static const DWORD STDTEMPL_CONNPOINT_XH_HD12		= 0x0201000d;//曲臂横担内连接点6根
static const DWORD STDTEMPL_CONNPOINT_XH_HD12_X		= 0x0201000e;//曲臂横担内连接点6根
static const DWORD STDTEMPL_CONNPOINT_XH_HD13		= 0x0201000f;//地线支架横担上连接点
static const DWORD STDTEMPL_CONNPOINT_XH_HD14		= 0x02010010;//地线支架横担下连接点
//static const DWORD STDTEMPL_CONNPOINT_HD1A		= 0x02010011;//水平横担与地线支架内侧连接点（基准水平材断开）
//底面
static const DWORD STDTEMPL_CONNPOINT_XH_DM1		=0x02090001;//地线底面连接点
static const DWORD STDTEMPL_CONNPOINT_XH_DM2		=0x02090002;//地线底面连接点
static const DWORD STDTEMPL_CONNPOINT_XH_DM3		=0x02090003;//地线底面连接点
//曲臂K节点  CRANK ARM		  
static const DWORD STDTEMPL_CONNPOINT_ZK14_K1		=0x02020001; //采用外包角钢(制弯角钢)连接
static const DWORD STDTEMPL_CONNPOINT_ZK14_K2		=0x02020002; //上下曲臂不截断采用钢板正侧面互焊
static const DWORD STDTEMPL_CONNPOINT_ZK14_K3		=0x02020003;//酒杯猫头上下曲臂K节点处变坡
static const DWORD STDTEMPL_CONNPOINT_ZK14_K1_SIDE	=0x02020004;//酒杯上下曲臂K节点侧面（单面）
static const DWORD STDTEMPL_CONNPOINT_ZK14_K3_SIDE	=0x02020005;//酒杯上下曲臂K节点侧面（双面变坡）
//变坡节点  SLOPE
static const DWORD STDTEMPL_CONNPOINT_BP1			=0x02030001;//单角钢塔身变坡处连接点
static const DWORD STDTEMPL_CONNPOINT_BP2			=0x02030002;//单角钢猫头酒杯塔头变坡处连接点
static const DWORD STDTEMPL_CONNPOINT_BP3			=0x02030003;//组合角钢塔身变坡处连接点
static const DWORD STDTEMPL_CONNPOINT_BP4			=0x02030004;//组合角钢猫头酒杯塔头变坡处连接点
//塔身主材K节点 TOWER_BODY
static const DWORD STDTEMPL_CONNPOINT_SZK			=0x02040001;//zk 1,2,3,4,11,12,13,21,22,23,24;//
static const DWORD STDTEMPL_CONNPOINT_ZK_K			=0x02040002;//zk 1,2,3,4,11,12,13,21,22,23,24;//
static const DWORD STDTEMPL_CONNPOINT_HK1			=0x02040003;//HK1 隔面K点
static const DWORD STDTEMPL_CONNPOINT_HK2			=0x02040004;//HK2 隔面K点
//交叉节点  CROSS
static const DWORD STDTEMPL_CONNPOINT_JC1			=0x02050001;//单角钢塔身交叉点无支撑件
static const DWORD STDTEMPL_CONNPOINT_JC1_K1		=0x02050002;//单角钢塔身交叉点无支撑件
static const DWORD STDTEMPL_CONNPOINT_JC1_K2		=0x02050003;//单角钢塔身交叉点无支撑件
static const DWORD STDTEMPL_CONNPOINT_JC2			=0x02050004;//组合角钢塔身交叉点无支撑件
static const DWORD STDTEMPL_CONNPOINT_JC3			=0x02050005;//塔身交叉点有支撑件
//挂线节点
static const DWORD STDTEMPL_CONNPOINT_XH_GX1		=0x02060001;//挂线处控制节点(ID:12)
static const DWORD STDTEMPL_CONNPOINT_XH_GX2		=0x02060002;//挂线处控制节点(ID:13)
static const DWORD STDTEMPL_CONNPOINT_XH_GX3		=0x02060007;//挂线处控制节点
static const DWORD STDTEMPL_CONNPOINT_XH_GX4		=0x02060008;//挂线处控制节点
static const DWORD STDTEMPL_CONNPOINT_XH_GX5		=0x02060009;//挂线处控制节点
static const DWORD STDTEMPL_CONNPOINT_XH_DX1		=0x02060003;//地线之架端节点(ID:14)
static const DWORD STDTEMPL_CONNPOINT_XH_DX2		=0x02060004;//地线之架端节点(ID:14)
static const DWORD STDTEMPL_CONNPOINT_XH_DX3		=0x02060005;//地线之架内侧弦杆与横杆相交节点(ID:14)
static const DWORD STDTEMPL_CONNPOINT_XH_DX3A		=0x02060006;//地线之架内侧弦杆(共线截断)与横杆相交节点
//static const DWORD STDTEMPL_CONNPOINT_XH_GX3		=0x02060007;//挂线处控制节点
//static const DWORD STDTEMPL_CONNPOINT_XH_GX4		=0x02060008;//挂线处控制节点
//static const DWORD STDTEMPL_CONNPOINT_XH_GX5		=0x02060009;//挂线处控制节点
//塔腿V节点
static const DWORD STDTEMPL_CONNPOINT_TV			=0x02070001;//TV
//辅助材节点   AUXILIARY
static const DWORD STDTEMPL_CONNPOINT_FK1			=0x02080001;//双杆件辅助材连接节点
static const DWORD STDTEMPL_CONNPOINT_FK2			=0x02080002;//三杆件辅助材连接节点
static const DWORD STDTEMPL_CONNPOINT_FK3			=0x02080003;//四杆件辅助材连接节点
static const DWORD STDTEMPL_CONNPOINT_FK4			=0x02080004;//塔腿辅助材连接节点

//S  十字型//T T型角钢//K 开断//N  肢向朝内//NW 肢向一内一外//G 杆件数量//B 螺栓数量
//实体板
//横担节点
static const DWORD STDTEMPL_NODEPLATE_HD1			=0x03014801;//水平横担与地线支架内侧连接点(双面板水平横杆不断开)
static const DWORD STDTEMPL_NODEPLATE_HD1A			=0x03014818;//水平横担与地线支架内侧连接点(水平横杆断开)
static const DWORD STDTEMPL_NODEPLATE_HD2			=0x03014802;//水平横担与地线支架外侧连接点(双面板)
static const DWORD STDTEMPL_NODEPLATE_HD2_2			=0x03014803;//水平横担与地线支架外侧连接点(三面板)
static const DWORD STDTEMPL_NODEPLATE_HD3			=0x03014804;//酒杯塔上曲臂外侧与横担外连接点(双面板)
static const DWORD STDTEMPL_NODEPLATE_HD3_2			=0x03014805;//酒杯塔上曲臂外侧与横担外连接点(三面板)
static const DWORD STDTEMPL_NODEPLATE_HD3_G5		=0x03014806;//酒杯塔曲臂与横担外连接点水平杆件非断(双面板)
static const DWORD STDTEMPL_NODEPLATE_HD4			=0x03014807;//猫头塔曲臂内拐点处
static const DWORD STDTEMPL_NODEPLATE_HD4_2			=0x03014817;//猫头塔曲臂内拐点处
static const DWORD STDTEMPL_NODEPLATE_HD5			=0x03014808;//猫头塔曲臂外拐点处(上)
static const DWORD STDTEMPL_NODEPLATE_HD6			=0x03014809;//猫头塔曲臂外拐点处(下)
static const DWORD STDTEMPL_NODEPLATE_HD7			=0x0301480a;//塔身与水平横担下部连接点
static const DWORD STDTEMPL_NODEPLATE_HD8			=0x0301480b;//塔身与水平横担上部连接点
static const DWORD STDTEMPL_NODEPLATE_HD9			=0x0301480c;//塔头与水平横担连接点
static const DWORD STDTEMPL_NODEPLATE_HD10			=0x0301480d;//横担塔身(hd_10)
static const DWORD STDTEMPL_NODEPLATE_XH_HD3A		=0x0301480e;//曲臂横担内连接点4根
static const DWORD STDTEMPL_NODEPLATE_XH_HD12		=0x0301480f;//曲臂横担内连接点6根
static const DWORD STDTEMPL_NODEPLATE_XH_HD12_X		=0x03014810;//曲臂横担内连接点6根
static const DWORD STDTEMPL_NODEPLATE_XH_HD13		=0x03014811;//地线支架横担上连接点
static const DWORD STDTEMPL_NODEPLATE_XH_HD14		=0x03014812;//地线支架横担下连接点
static const DWORD STDTEMPL_NODEPLATE_HD8_YS		=0x03014813;//横担塔身(hd_7)
static const DWORD STDTEMPL_NODEPLATE_HD7_T			=0x03014814;//横担塔头(T角钢)
static const DWORD STDTEMPL_NODEPLATE_HD9_T			=0x03014815;//横担塔身(T角钢)
static const DWORD STDTEMPL_NODEPLATE_HD10_T		=0x03014816;//横担塔身(T角钢)
//static const DWORD STDTEMPL_NODEPLATE_HD4_2		=0x03014817;//猫头塔曲臂内拐点处
//static const DWORD STDTEMPL_NODEPLATE_HD1A		=0x03014818;//水平横担与地线支架内侧连接点(水平横杆断开)
//底面
static const DWORD STDTEMPL_NODEPLATE_XH_DM1		=0x03090001;//地线底面连接点
static const DWORD STDTEMPL_NODEPLATE_XH_DM1A		=0x03090004;//地线底面连接点
static const DWORD STDTEMPL_NODEPLATE_XH_DM2		=0x03090002;//挂线杆件底面连接点
static const DWORD STDTEMPL_NODEPLATE_XH_DM3		=0x03090003;//挂线杆件底面连接点
//曲臂K节点  CRANK ARM		  
static const DWORD STDTEMPL_NODEPLATE_ZK14_K1		=0x03025A01; //采用外包角钢(制弯角钢)连接
static const DWORD STDTEMPL_NODEPLATE_ZK14_K2		=0x03025A02; //上下曲臂不截断采用钢板正侧面互焊
static const DWORD STDTEMPL_NODEPLATE_ZK14_K3		=0x03025A03;//酒杯猫头上下曲臂K节点处变坡
static const DWORD STDTEMPL_NODEPLATE_ZK14_K1_SIDE	=0x03025A04;//酒杯上下曲臂K节点侧面（单面）
static const DWORD STDTEMPL_NODEPLATE_ZK14_K3_SIDE	=0x03025A05;//酒杯上下曲臂K节点侧面（双面变坡）
//变坡节点
static const DWORD STDTEMPL_NODEPLATE_BP1			=0x03034201;//单角钢塔身变坡处连接点
static const DWORD STDTEMPL_NODEPLATE_BP2			=0x03034202;//单角钢猫头酒杯塔头变坡处连接点
static const DWORD STDTEMPL_NODEPLATE_BP3			=0x03034203;//组合角钢塔身变坡处连接点
static const DWORD STDTEMPL_NODEPLATE_BP4			=0x03034203;//组合角钢猫头酒杯塔头变坡处连接点
//塔身主材K节点
//塔身  Z  5A
static const DWORD STDTEMPL_NODEPLATE_SZK1			=0x03045A01;//主斜材均为单角钢
static const DWORD STDTEMPL_NODEPLATE_SZK2			=0x03045A02;//主斜材均为单角钢
static const DWORD STDTEMPL_NODEPLATE_SZK5			=0x03045A03;//主斜材均为单角钢(ID:11)	
static const DWORD STDTEMPL_NODEPLATE_SZK6			=0x03045A04;//主斜材均为单角钢
static const DWORD STDTEMPL_NODEPLATE_ZK1_N1		=0x03045A05;//主斜材均为单角钢
static const DWORD STDTEMPL_NODEPLATE_ZK1_N2		=0x03045A06;//主斜材均为单角钢
static const DWORD STDTEMPL_NODEPLATE_ZK1_N3		=0x03045A07;//主斜材均为单角钢
static const DWORD STDTEMPL_NODEPLATE_ZK2_S1		=0x03045A08;//主材组合角钢，斜材单角钢
static const DWORD STDTEMPL_NODEPLATE_ZK2_S2		=0x03045A09;//主材组合角钢，斜材单角钢
static const DWORD STDTEMPL_NODEPLATE_ZK2_S3		=0x03045A0A;//主材组合角钢，斜材单角钢
static const DWORD STDTEMPL_NODEPLATE_ZK2_T			=0x03045A0B;//主材组合角钢，斜材单角钢
static const DWORD STDTEMPL_NODEPLATE_ZK3_S			=0x03045A0C;//主斜材均为组合角钢
static const DWORD STDTEMPL_NODEPLATE_ZK3_T			=0x03045A0D;//主斜材均为组合角钢
static const DWORD STDTEMPL_NODEPLATE_ZK4			=0x03045A0E;//主斜材均为钢管
//隔面  H  48
static const DWORD STDTEMPL_NODEPLATE_HK1			=0x03044801;//塔身水平横材八字支撑板(两射线）
static const DWORD STDTEMPL_NODEPLATE_HK2			=0x03044802;//塔身水平横材米字扣撑板（四射线）
static const DWORD STDTEMPL_NODEPLATE_HK3			=0x03044803;//塔身及非公用塔腿隔面，T型组合角钢。hk3
//交叉节点
static const DWORD STDTEMPL_NODEPLATE_JC1_K1		=0x03054A01;//塔身交叉点：单角钢，无支撑件
static const DWORD STDTEMPL_NODEPLATE_JC1_K2		=0x03054A02;//塔身交叉点：单角钢，无支撑件
static const DWORD STDTEMPL_NODEPLATE_JC2			=0x03054A03;//塔身交叉点：单角钢，有支撑件	jc 06,07
static const DWORD STDTEMPL_NODEPLATE_JC3			=0x03054A04;//塔身交叉点：T型组合角钢		jc 08
//塔腿V节点
static const DWORD STDTEMPL_NODEPLATE_TV1_G2		=0x03075401;//双杆件单角钢
static const DWORD STDTEMPL_NODEPLATE_TV1_G3		=0x03075402;//三杆件单角钢
static const DWORD STDTEMPL_NODEPLATE_TV1_G4		=0x03075403;//四杆件单角钢
static const DWORD STDTEMPL_NODEPLATE_TV2_B1		=0x03075404;//单螺栓T型角钢
static const DWORD STDTEMPL_NODEPLATE_TV2_B2		=0x03075405;//双螺栓T型角钢
static const DWORD STDTEMPL_NODEPLATE_TV2_B3		=0x03075406;//三螺栓T型角钢
//挂线节点
//挂线  G  47
static const DWORD STDTEMPL_NODEPLATE_XH_GX1		=0x03064701;//挂线处控制节点(ID:12)
static const DWORD STDTEMPL_NODEPLATE_XH_GX2		=0x03064702;//挂线处控制节点(ID:13)
static const DWORD STDTEMPL_NODEPLATE_XH_GX3		=0x03064703;//挂线处控制节点
static const DWORD STDTEMPL_NODEPLATE_XH_GX4		=0x03064704;//挂线处控制节点
static const DWORD STDTEMPL_NODEPLATE_XH_GX5		=0x03064705;//挂线处控制节点
//地线  D  44
static const DWORD STDTEMPL_NODEPLATE_XH_DX1		=0x03064401;//地线架端上节点(ID:14)
static const DWORD STDTEMPL_NODEPLATE_XH_DX1A		=0x03064402;//地线架端上节点(ID:14)
static const DWORD STDTEMPL_NODEPLATE_XH_DX2		=0x03064403;//地线架端下节点(ID:14)
static const DWORD STDTEMPL_NODEPLATE_XH_DX3		=0x03064404;//地线架端上节点(ID:14)
static const DWORD STDTEMPL_NODEPLATE_XH_DX3A		=0x03064405;//地线架端上节点(ID:14)
//辅助材节点  
static const DWORD STDTEMPL_NODEPLATE_FK1			=0x03084601;//双杆件辅助材连接节点
static const DWORD STDTEMPL_NODEPLATE_FK2			=0x03084602;//三杆件辅助材连接节点
static const DWORD STDTEMPL_NODEPLATE_FK3			=0x03084603;//四杆件辅助材连接节点
static const DWORD STDTEMPL_NODEPLATE_FK4			=0x03084604;//塔腿辅助材连接节点






