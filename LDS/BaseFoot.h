#pragma once
struct ANCHOR_BOLT_D{
	int d;
	double inner_d;
};
struct CONCRETE_FC{
	char mark[4];
	double fc;
};
struct ALFA_ITEM{
	double b_a;
	double alfa;
};
class CLDSLineAngle;
class CFootTempl;
bool CalFootDesignParams(CLDSLineAngle* pBaseAngle,CLDSLineAngle* pVAngle,CFootTempl* pFootDesign);
