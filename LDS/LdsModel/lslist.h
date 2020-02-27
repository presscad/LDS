#ifndef __TDA_LS_REF_LIST_H_
#define __TDA_LS_REF_LIST_H_

#include "macro.h"
#ifdef __COMMON_PART_INC_
typedef struct tagBOLT_RECORD{
	char grade[8];
	long d,num;
	double L;
	long m_hFamily;			// ��˨���ϵ�б�ʶ
	BOOL m_bAntiTheft;	// �Ƿ������˨
	BOOL m_bAntiLoose;	// �Ƿ�����ɵ�
	//BOOL m_bTwoCapLs;		// ˫ñ��˨
  	//BOOL m_bFootNail;		// �Ŷ�(TRUE)����˨(FALSE)
}BOLT_RECORD;
class CLsRefList : public CHashList<CLsRef>
{
//1.˽�г�Ա��������
	BOOL *m_pModified;	//�޸ı�ʶ
//2.���г�Ա��������
public:
	CLsRefList(){m_pModified=NULL;}
	~CLsRefList(){;}

	//THANDLE handle;
	//THANDLE attach_part_handle;	//�����������ľ����Ǹֽڵ��
	//int single1_or_double2;	// ������˨1��˫����˨2
//3.˽�г�Ա��������
private:
//4.���г�Ա��������

public:
	void ResetBoltRefRayNo(CLDSPart *pPart);	//�������pPlate������������ӵ���� wht 11-06-30
	bool MergeBolts(CLDSBolt *pFromBolt,CLDSBolt *pDestBolt);	//��pFromBolt�ϲ���pDestBolt
	int GetLsString(char* ls_str);
	int GetBoltRecordArr(BOLT_RECORD *recArr=NULL);
	void SetModifiedFlagAddress(BOOL *pModified)
	{m_pModified = pModified;}
	int GetLsCount(short d=0);
	int GetLocalLsCount(short start1_end2_mid0=0);	//�õ��ֲ���˨�� ʼ��/�ն�/�м�
	void FromBuffer(CBuffer &buffer,IModel *pTower,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//void Serialize(CArchive& ar);
	//CLsRef* append();		//�ڽڵ������ĩβ��ӽڵ�
	CLsRef* append(CLsRef Ls);//�ڽڵ������ĩβ��ӽڵ�
	//base���ʾ��˨�Ļ���λ��(�������ϵ��),ucs��ʾһ����˨�����ڵ��������ϵ,
	//������ϵ������һ��������˨�Ĺ��������࿿��,iNormX0Y1Z2�ͱ�ʾ
	//����˨���������࿿�����������ϵ��������
	CLsRef* FindLsByUcsPos(f3dPoint base,UCS_STRU ucs,int iNormX0Y1Z2,double epsilon=EPS2);
	CLsRef* FindLsByPos(f3dPoint base,f3dPoint work_norm,double epsilon=EPS2);
	BOOL DeleteNode(THANDLE handle);
	CLsRef* FromHandle(THANDLE);
};
#endif
#endif