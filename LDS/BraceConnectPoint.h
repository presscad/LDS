#pragma once
//辅材设计
class BRACE_CONNECT_POINT{	//sizeof(BRACE_NODE_CONNECT)=8 Bytes
protected:
	bool m_bRedesigned;
public:
	BYTE cnBraceRayAngles;	//射线连接的辅材数量
	WORD niBaseAngleWidth;	//当前连接点基准角钢肢宽
	struct RAYANGLE {	//sizeof(RAYANGLE)=10 Bytes
	public:	//辅材连接点设计模块的输入信息
		short siRayAngle;	//相对于基准材纵向的射线夹角，度°
		BYTE ciBoltD;	//螺栓直径
		BYTE ciBoltN;	//连接螺栓数
		bool blInsideAngle;	//相对于基准材工作肢法线为里铁连接
	public:	//辅材连接点设计模块的输出设计结果
		char ciTipPosStyle;	//-1:杆件退出负头连接;0.射线材用螺栓直连至基准角钢;1.把该角钢心线端点移至共用螺栓位置
		//struct ANGLEG {
			//基准材螺栓心距
			char ciBaseStyleG;	//<0(-1):保持不变;0.准距g;1.g1;2.g2;3.g3;4.以楞为基准由siOffsetG定义的g值;;5.以肢翼边为基准由siOffsetG定义的g值
			short siOffsetG;	//根据ciStyleG指定的基准g线，沿肢延展方向的心线偏移量
		//}xDirectBoltG;
		RAYANGLE() {
			ciBoltD=16;
			ciBoltN=1;
			blInsideAngle=false;
			ciTipPosStyle=0;
			ciBaseStyleG=-1;	//保持不变
			siOffsetG=0;
		}
	}*xarrRayAngles;
	bool get_blRedesigned() const{ return cnBraceRayAngles>0&&this->m_bRedesigned; }
	__declspec(property(get=get_blRedesigned)) bool blRedesigned;

	BRACE_CONNECT_POINT(WORD niBaseWidth=0,RAYANGLE* pAttachRayAngles=NULL,BYTE cnRayAngels=0);
	static bool (*FireDesignBraceNodeConnectFunc)(BRACE_CONNECT_POINT* pBraceConnPoint);
	bool DesignConnectPoint();
};
typedef bool  (*DefDesignBraceNodeConnectFunc)(BRACE_CONNECT_POINT* pBraceConnPoint);
