#include "stdafx.h"
#include "Fitting.h"

/*template <class FITTING_TYPE> FITTING_TYPE* QueryFitting(const char* sSpec)
{
	if(FITTING_TYPE::GetTypeId()==IFitting::FITTING_EB)
		return FittingLibrary<FITTING_EB>()->QueryFittingRecord(sSpec);
	else if(FITTING_TYPE::GetTypeId()==IFitting::FITTING_GD)
		return FittingLibrary<FITTING_GD>()->QueryFittingRecord(sSpec);
	else if(FITTING_TYPE::GetTypeId()==IFitting::FITTING_UR)
		return FittingLibrary<FITTING_UR>()->QueryFittingRecord(sSpec);
	else
		return NULL;
}*/
FITTING_EB* FindEBFitting(const char* sSpec)
{
	return FittingLibrary<FITTING_EB>()->QueryFittingRecord(sSpec);
}
FITTING_GD* FindGDFitting(const char* sSpec)
{
	return FittingLibrary<FITTING_GD>()->QueryFittingRecord(sSpec);
}
FITTING_UR* FindURFitting(const char* sSpec)
{
	return FittingLibrary<FITTING_UR>()->QueryFittingRecord(sSpec);
}
