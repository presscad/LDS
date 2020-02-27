#pragma once
//�������
class INTELLI_JOINT_CONNECT{	//sizeof(BRACE_NODE_CONNECT)=8 Bytes
protected:
	bool m_bRedesigned;
	//bool m_bRowsSpecified;
public:
	bool blPostAngle;		//[����]���Ľ�ͷ
	bool blRowsSpecified;
	BYTE ciJointStyle;		//[���]0.�����;1.�������;2.�ڰ�������ְ�;3.�ڰ��ְ������;
	struct MAINANGLE {
		BYTE ciBoltD;		//[����/���]
		BYTE cnMinBoltN;	//[����/���]
		BYTE cnRows;		//[����/���]
		BYTE ciAngleThick;	//[����]
		WORD wiAngleWidth;	//[����]
	}pairangles[2];	//��ͷ�����ӵĽǸ�
	bool get_blRedesigned() const{ return this->m_bRedesigned; }
	__declspec(property(get=get_blRedesigned)) bool blRedesigned;

	INTELLI_JOINT_CONNECT();
	static bool (*FireIntelliDesignJointConnectFunc)(INTELLI_JOINT_CONNECT* pIntelliJointConnect);
	bool DesignJointConnect();
};
typedef bool  (*DefDesignJointConnectFunc)(INTELLI_JOINT_CONNECT* pIntelliJointConnect);
