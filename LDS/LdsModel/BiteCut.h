// BiteCut.h: interface for the CFlatBiteCut class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//ͨ��һ������յ����ߣ�TopFace���߷����������ڿ�����ʱ�����ʱ���Ϊ���и�ࣩ��
//��һ��ָ��ƽ��Ĳ����ҧ�е�һ��ȱ��
struct CFlatBiteCut
{
	struct INTERS_POINT{
		int index;	//�ö����Ӧ��������ţ�����������solid�Ķ����������Ҳ�����Ǿֲ����������������
		GEPOINT position;
		INTERS_POINT(const double* pos_coords=NULL){
			index=-1;
			position=GEPOINT(pos_coords);
		}
	};
public:
	static bool Cut(GEPOINT* xarrCutPolyVertexes,int count,f3dPolyFace *pTopFace, f3dPolyFace *pBtmFace,fBody *pBody);
};
