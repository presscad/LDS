// ConeTubeParts.h: interface for the CConeTubeParts class.
//
//////////////////////////////////////////////////////////////////////
#include "Tower.h"
#include "lds_part.h"
#ifdef __GGG_
class CLDSConeTubeFl : public CLDSParamPlate
{
public:
	CLDSConeTubeFl();
	CLDSConeTubeFl(double upRadius,double downRadius,double thick,int faceNum,double outsideR);
	~CLDSConeTubeFl();
	//
	void SetBasePt(f3dPoint pt);
	f3dPoint GetBasePt(){return m_basePt;};
	void SetUpRadius(double nR);
	double GetUpRadius(){return m_fUpRadius;};
	void SetDownRadius(double nR);
	double GetDownRadius(){return m_fDownRadius;};
	void SetThick(double nT);
	double GetThick(){return m_fThick;};
	void SetFaceNum(int nNum);
	int GetFaceNum(){return m_nFaceNum;};
	void SetOutsideR(double nR);
	double GetOutsideR(){return m_fOutsideR;};
	void Create3dSolidModel(UCS_STRU ucs);
private:
	double m_fUpRadius;
	double m_fDownRadius;
	double m_fThick;
	int m_nFaceNum;
	double m_fOutsideR;//Õ‚√Ê‘≤∞Îæ∂
	f3dPoint m_basePt;
};
#endif

