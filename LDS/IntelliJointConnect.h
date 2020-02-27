#pragma once
//辅材设计
class INTELLI_JOINT_CONNECT{	//sizeof(BRACE_NODE_CONNECT)=8 Bytes
protected:
	bool m_bRedesigned;
	//bool m_bRowsSpecified;
public:
	bool blPostAngle;		//[输入]主材接头
	bool blRowsSpecified;
	BYTE ciJointStyle;		//[输出]0.外包铁;1.内外包铁;2.内包铁外包钢板;3.内包钢板外包铁;
	struct MAINANGLE {
		BYTE ciBoltD;		//[输入/输出]
		BYTE cnMinBoltN;	//[输入/输出]
		BYTE cnRows;		//[输入/输出]
		BYTE ciAngleThick;	//[输入]
		WORD wiAngleWidth;	//[输入]
	}pairangles[2];	//接头所连接的角钢
	bool get_blRedesigned() const{ return this->m_bRedesigned; }
	__declspec(property(get=get_blRedesigned)) bool blRedesigned;

	INTELLI_JOINT_CONNECT();
	static bool (*FireIntelliDesignJointConnectFunc)(INTELLI_JOINT_CONNECT* pIntelliJointConnect);
	bool DesignJointConnect();
};
typedef bool  (*DefDesignJointConnectFunc)(INTELLI_JOINT_CONNECT* pIntelliJointConnect);
