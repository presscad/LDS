#pragma once
//�������
class BRACE_CONNECT_POINT{	//sizeof(BRACE_NODE_CONNECT)=8 Bytes
protected:
	bool m_bRedesigned;
public:
	BYTE cnBraceRayAngles;	//�������ӵĸ�������
	WORD niBaseAngleWidth;	//��ǰ���ӵ��׼�Ǹ�֫��
	struct RAYANGLE {	//sizeof(RAYANGLE)=10 Bytes
	public:	//�������ӵ����ģ���������Ϣ
		short siRayAngle;	//����ڻ�׼����������߼нǣ��ȡ�
		BYTE ciBoltD;	//��˨ֱ��
		BYTE ciBoltN;	//������˨��
		bool blInsideAngle;	//����ڻ�׼�Ĺ���֫����Ϊ��������
	public:	//�������ӵ����ģ��������ƽ��
		char ciTipPosStyle;	//-1:�˼��˳���ͷ����;0.���߲�����˨ֱ������׼�Ǹ�;1.�ѸýǸ����߶˵�����������˨λ��
		//struct ANGLEG {
			//��׼����˨�ľ�
			char ciBaseStyleG;	//<0(-1):���ֲ���;0.׼��g;1.g1;2.g2;3.g3;4.����Ϊ��׼��siOffsetG�����gֵ;;5.��֫���Ϊ��׼��siOffsetG�����gֵ
			short siOffsetG;	//����ciStyleGָ���Ļ�׼g�ߣ���֫��չ���������ƫ����
		//}xDirectBoltG;
		RAYANGLE() {
			ciBoltD=16;
			ciBoltN=1;
			blInsideAngle=false;
			ciTipPosStyle=0;
			ciBaseStyleG=-1;	//���ֲ���
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
