#pragma once
#include "Expression.h"

class EXPRESSION_FUNC_NODE_X : public EXPRESSION_FUNC
{
	bool Calc(EXPRESSION_VALUE* pRsltValue, const char* param_str, CExpression* pContextExpr)
	{
		CStdComTemplExpression* pStdTemplExpr = (CStdComTemplExpression*)pContextExpr;
		DWORD idNode = (DWORD)pContextExpr->SolveExpression(param_str);
		long hNode = pStdTemplExpr->pStdTempl->MappingLdsNodeH(idNode);//ToLdsHandle(SEGI("N",widNode));
		CLDSNode* pNode = (hNode == 0) ? NULL : Ta.Node.FromHandle(hNode);
		if (pNode == NULL)
			return false;
		if (pRsltValue)
		{
			pRsltValue->ciValType = EXPRESSION_VALUE::VALUETYPE_DOUBLE;
			GEPOINT pos = pStdTemplExpr->acs.TransPToCS(pNode->xOriginalPos);
			pRsltValue->fValue = pos.x;
		}
		return true;
	}
};
class EXPRESSION_FUNC_NODE_Y : public EXPRESSION_FUNC
{
	bool Calc(EXPRESSION_VALUE* pRsltValue, const char* param_str, CExpression* pContextExpr)
	{
		CStdComTemplExpression* pStdTemplExpr = (CStdComTemplExpression*)pContextExpr;
		DWORD idNode = (DWORD)pContextExpr->SolveExpression(param_str);
		long hNode = pStdTemplExpr->pStdTempl->MappingLdsNodeH(idNode);//ToLdsHandle(SEGI("N",widNode));
		CLDSNode* pNode = (hNode == NULL) ? NULL : Ta.Node.FromHandle(hNode);
		if (pNode == NULL)
			return false;
		if (pRsltValue)
		{
			pRsltValue->ciValType = EXPRESSION_VALUE::VALUETYPE_DOUBLE;
			GEPOINT pos = pStdTemplExpr->acs.TransPToCS(pNode->xOriginalPos);
			pRsltValue->fValue = pos.y;
		}
		return true;
	}
};
class EXPRESSION_FUNC_NODE_Z : public EXPRESSION_FUNC
{
	bool Calc(EXPRESSION_VALUE* pRsltValue, const char* param_str, CExpression* pContextExpr)
	{
		CStdComTemplExpression* pStdTemplExpr = (CStdComTemplExpression*)pContextExpr;
		DWORD idNode = (DWORD)pContextExpr->SolveExpression(param_str);
		long hNode = pStdTemplExpr->pStdTempl->MappingLdsNodeH(idNode);//ToLdsHandle(SEGI("N",widNode));
		CLDSNode* pNode = (hNode == NULL) ? NULL : Ta.Node.FromHandle(hNode);
		if (pNode == NULL)
			return false;
		if (pRsltValue)
		{
			pRsltValue->ciValType = EXPRESSION_VALUE::VALUETYPE_DOUBLE;
			GEPOINT pos = pStdTemplExpr->acs.TransPToCS(pNode->xOriginalPos);
			pRsltValue->fValue = pos.z;
		}
		return true;
	}
};
class EXPRESSION_FUNC_ABS : public EXPRESSION_FUNC
{
	bool Calc(EXPRESSION_VALUE* pRsltValue, const char* param_str, CExpression* pContextExpr)
	{
		double lfValue = pContextExpr->SolveExpression(param_str);
		pRsltValue->fValue = lfValue < 0 ? -lfValue : lfValue;
		return true;
	}
};
class EXPRESSION_FUNC_NODE_POS : public EXPRESSION_FUNC
{
	bool Calc(EXPRESSION_VALUE* pRsltValue, const char* param_str, CExpression* pContextExpr)
	{
		CStdComTemplExpression* pStdTemplExpr = (CStdComTemplExpression*)pContextExpr;
		DWORD idNode = (DWORD)pContextExpr->SolveExpression(param_str);
		long hNode = pStdTemplExpr->pStdTempl->MappingLdsNodeH(idNode);//ToLdsHandle(SEGI("N",widNode));
		CLDSNode* pNode = (hNode == NULL) ? NULL : Ta.Node.FromHandle(hNode);
		if (pNode == NULL)
			return false;
		if (pRsltValue)
		{
			pRsltValue->ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;
			pRsltValue->objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_COORD3D;
			pRsltValue->objtype.wiPropValue = 0;
			GEPOINT pos = pStdTemplExpr->acs.TransPToCS(pNode->xArcliftPos);//xOriginalPos);
			pRsltValue->coord3d[0] = pos.x;
			pRsltValue->coord3d[1] = pos.y;
			pRsltValue->coord3d[2] = pos.z;
		}
		return true;
	}
};
class EXPRESSION_FUNC_NODE_on_ROD_POS : public EXPRESSION_FUNC
{
	bool Calc(EXPRESSION_VALUE* pRsltValue, const char* param_str, CExpression* pContextExpr)
	{
		CStdComTemplExpression* pStdTemplExpr = (CStdComTemplExpression*)pContextExpr;
		CXhChar50 copystr = param_str;
		char *pchSpliter = strtok(copystr, ",");
		CXhChar16 itemstr = pchSpliter;
		DWORD idNode = (DWORD)pContextExpr->SolveExpression(itemstr);
		if ((pchSpliter = strtok(NULL, ",")) == NULL)
			return false;
		DWORD idRod = (DWORD)pContextExpr->SolveExpression(pchSpliter);
		long hNode = pStdTemplExpr->pStdTempl->MappingLdsNodeH(idNode);//ToLdsHandle(SEGI("N",widNode));
		long* phRod = pStdTemplExpr->pStdTempl->hashMatchRodHid.GetValue(idRod);//ToLdsHandle(SEGI("N",widNode));
		CLDSNode* pNode = (hNode == NULL) ? NULL : Ta.Node.FromHandle(hNode);
		if (pNode == NULL)
			return false;
		CLDSLinePart* pRod = (phRod == NULL) ? NULL : Ta.FromRodHandle(*phRod);
		if (pRod == NULL)
			return false;
		if (pRsltValue)
		{
			pRsltValue->ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;
			pRsltValue->objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_COORD3D;
			pRsltValue->objtype.wiPropValue = 0;
			GEPOINT xDatumPoint = pRod->GetDatumPos(pNode);
			GEPOINT pos = pStdTemplExpr->acs.TransPToCS(xDatumPoint);//xOriginalPos);
			pRsltValue->coord3d[0] = pos.x;
			pRsltValue->coord3d[1] = pos.y;
			pRsltValue->coord3d[2] = pos.z;
		}
		return true;
	}
};
class EXPRESSION_FUNC_ROD_LENV : public EXPRESSION_FUNC
{
	bool Calc(EXPRESSION_VALUE* pRsltValue, const char* param_str, CExpression* pContextExpr)
	{
		CStdComTemplExpression* pStdTemplExpr = (CStdComTemplExpression*)pContextExpr;
		DWORD idRod = (DWORD)pContextExpr->SolveExpression(param_str);
		long* phRod = pStdTemplExpr->pStdTempl->hashMatchRodHid.GetValue(idRod);//ToLdsHandle(SEGI("N",widNode));
		CLDSLinePart* pRod = (phRod == NULL) ? NULL : Ta.FromRodHandle(*phRod);
		if (pRod == NULL)
			return false;
		if (pRsltValue)
		{
			pRsltValue->ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;
			pRsltValue->objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_COORD3D;
			pRsltValue->objtype.wiPropValue = 0;
			GEPOINT lenv = pRod->xPosEnd - pRod->xPosStart;
			normalize(lenv);
			lenv = pStdTemplExpr->acs.TransPToCS(lenv);
			pRsltValue->coord3d[0] = lenv.x;
			pRsltValue->coord3d[1] = lenv.y;
			pRsltValue->coord3d[2] = lenv.z;
		}
		return true;
	}
};
class EXPRESSION_FUNC_CROSSROD_ACS : public EXPRESSION_FUNC
{
	//param_str示例：CrossRodAcs(1,1,2),CrossRodAcs(1,1,2,P),CrossRodAcs(1,1,2,P,Y/y,R/L)
	//param1:定位节点模板标识;param2:定位基准杆件模板标识;param3:Y轴方向参照杆件模板标识
	//param4:'P'表示单线坐标，无或其它值表示实体坐标
	//param5:'Y'表示在参照杆件的射线方向,'y'表示在参照杆件的反射线方向
	//param6:'R'或非'L'值表示坐标系遵循右手螺旋法则,'L'表示坐标系遵循左手螺旋法则
	bool Calc(EXPRESSION_VALUE* pRsltValue, const char* param_str, CExpression* pContextExpr)
	{
		CStdComTemplExpression* pStdTemplExpr = (CStdComTemplExpression*)pContextExpr;

		CXhChar50 copystr = param_str;
		char *pszKey = strtok(copystr, ",");
		CXhChar16 itemstr = pszKey;
		DWORD idNode, idBaseRod, idRayRod;
		idNode = (DWORD)pContextExpr->SolveExpression(itemstr);
		if ((pszKey = strtok(NULL, ",")) == NULL)
			return false;
		idBaseRod = (DWORD)pContextExpr->SolveExpression(pszKey);
		if ((pszKey = strtok(NULL, ",")) == NULL)
			return false;
		idRayRod = (DWORD)pContextExpr->SolveExpression(pszKey);
		char ciPosType = 0;//0:表示杆件实体端点坐标；'P':表示杆件单线节点坐标
		char ciAxisDirectY = 'Y';	//'y':表示Y轴指向参照杆件phRayRod射线反方向；'Y'或其它:表示参照杆件phRayRod射线正方向
		char ciRotHandAcs = 'R';	//'L':表示坐标系遵循左手螺旋正交坐标系；'R'或其它:表示坐标系遵循右手螺旋正交坐标系
		if ((pszKey = strtok(NULL, ",")) != NULL)
		{
			if (*pszKey == 'P')
				ciPosType = 'P';
			if ((pszKey = strtok(NULL, ",")) != NULL)
			{
				ciAxisDirectY = (*pszKey == 'y') ? 'y' : 'Y';
				if ((pszKey = strtok(NULL, ",")) != NULL)
					ciRotHandAcs = (*pszKey == 'L') ? 'L' : 'R';
			}
		};
		CLDSNode* pBaseNode = NULL;
		CLDSLinePart *pBaseRod = NULL, *pRayRod = NULL;
		long hBaseNode = pStdTemplExpr->pStdTempl->MappingLdsNodeH(idNode);
		long* phBaseRod = pStdTemplExpr->pStdTempl->hashMatchRodHid.GetValue(idBaseRod);
		long* phRayRod = pStdTemplExpr->pStdTempl->hashMatchRodHid.GetValue(idRayRod);
		if ((pBaseNode = Ta.FromNodeHandle(hBaseNode)) == NULL)
			return false;
		if ((pBaseRod = phBaseRod == NULL ? NULL : Ta.FromRodHandle(*phBaseRod)) == NULL)
			return false;
		if ((pRayRod = phRayRod == NULL ? NULL : Ta.FromRodHandle(*phRayRod)) == NULL)
			return false;
		if (pRsltValue)
		{
			pRsltValue->ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;
			pRsltValue->objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_COORD3D;
			pRsltValue->objtype.wiPropValue = 0;
			GECS acs;
			SmartRodPtr pSmartRod = pBaseRod;
			GEPOINT lenv, rayv;
			if (ciPosType == 'P')
			{
				lenv = pBaseRod->pEnd->xOriginalPos - pBaseRod->pStart->xOriginalPos;
				if (pRayRod->pStart->handle == hBaseNode)
					rayv = pRayRod->pEnd->xOriginalPos - pRayRod->pStart->xOriginalPos;
				else //if(pRayRod->pEnd->handle==hBaseNode)
					rayv = pRayRod->pStart->xOriginalPos - pRayRod->pEnd->xOriginalPos;
			}
			else
			{
				lenv = pBaseRod->xPosEnd - pBaseRod->xPosStart;
				if (pRayRod->pStart->handle == hBaseNode)
					rayv = pRayRod->xPosEnd - pRayRod->xPosStart;
				else //if(pRayRod->pEnd->handle==hBaseNode)
					rayv = pRayRod->xPosStart - pRayRod->xPosEnd;
			}
			if (ciAxisDirectY == 'y')
				rayv *= -1.0;	//Y轴在参照杆件的反射线方向
			if (!normalize(lenv))
				return false;
			acs.axis_x = lenv;
			acs.axis_z = lenv ^ rayv;
			if (!normalize(acs.axis_z))
				return false;
			if (pBaseRod->IsAngle())
			{
				double ddx = pSmartRod.pAngle->vxWingNorm*acs.axis_z;
				double ddy = pSmartRod.pAngle->vyWingNorm*acs.axis_z;
				if (fabs(ddx) > fabs(ddy) && ddx < 0)
					acs.axis_z *= -1.0;
				else if (fabs(ddx) < fabs(ddy) && ddy < 0)
					acs.axis_z *= -1.0;
			}
			if (ciRotHandAcs == 'L')	//左手螺旋直角坐标系
				acs.axis_y = acs.axis_x^acs.axis_z;
			else
				acs.axis_y = acs.axis_z^acs.axis_x;
			normalize(acs.axis_y);
			if (acs.axis_y*rayv < 0)
			{
				acs.axis_y *= -1.0;
				acs.axis_x *= -1.0;
			}
			if (pBaseRod->IsAngle())
				acs.origin = pSmartRod.pAngle->GetDatumPosBer(pBaseNode);
			else
				acs.origin = pSmartRod->GetDatumPos(pBaseNode);
			//lenv=pStdTemplExpr->acs.TransPToCS(lenv);
			memcpy(pRsltValue->origin3d, acs.origin, 24);
			memcpy(pRsltValue->axisx3d, acs.axis_x, 24);
			memcpy(pRsltValue->axisy3d, acs.axis_y, 24);
			memcpy(pRsltValue->axisz3d, acs.axis_z, 24);
		}
		return true;
	}
};
class EXPRESSION_FUNC_WORKPLANE_CS : public EXPRESSION_FUNC
{
	//param_str示例：WorkPlaneCS(1,1,'Y','Z')
	//param1:定位节点模板标识;
	//param2:X/Y轴方向(具体X或Y取决于param3)参照角钢模板标识
	//param3:取值'X'或'Y'。'X'表示以param2杆件始->终轴向为X轴正向；'Y'表示以param2杆件始->终轴向为Y轴正向
	//param4:'X','Y','Z'表示近似工作面坐标系的Z轴方向
	bool Calc(EXPRESSION_VALUE* pRsltValue, const char* param_str, CExpression* pContextExpr)
	{
		CStdComTemplExpression* pStdTemplExpr = (CStdComTemplExpression*)pContextExpr;

		CXhChar50 copystr = param_str;
		char *pszKey = strtok(copystr, ",");
		CXhChar16 itemstr = pszKey;
		DWORD idNode, idAxisYRefRod;
		idNode = (DWORD)pContextExpr->SolveExpression(itemstr);
		if ((pszKey = strtok(NULL, ",")) == NULL)
			return false;
		idAxisYRefRod = (DWORD)pContextExpr->SolveExpression(pszKey);
		char ciAxisXorYNearDirect = 'Y';	//'y':表示Y轴指向参照杆件phRayRod射线反方向；'Y'或其它:表示参照杆件phRayRod射线正方向
		char ciAxisZNearDirect = 'Z';	//'y':表示Y轴指向参照杆件phRayRod射线反方向；'Y'或其它:表示参照杆件phRayRod射线正方向
		if ((pszKey = strtok(NULL, ",")) != NULL)
		{
			if ((*pszKey >= 'X'&&*pszKey <= 'Y') || (*pszKey >= 'x'&&*pszKey <= 'y'))
				ciAxisXorYNearDirect = *pszKey;
			if ((pszKey = strtok(NULL, ",")) != NULL)
			{
				if ((*pszKey >= 'X'&&*pszKey <= 'Z') || (*pszKey >= 'x'&&*pszKey <= 'z'))
					ciAxisZNearDirect = *pszKey;
			};
		};
		CLDSNode* pBaseNode = NULL;
		CLDSLinePart *pAxisXorYRefRod = NULL;
		long hBaseNode = pStdTemplExpr->pStdTempl->MappingLdsNodeH(idNode);
		long* phAxisXorYRefRod = pStdTemplExpr->pStdTempl->hashMatchRodHid.GetValue(idAxisYRefRod);
		if ((pBaseNode = Ta.FromNodeHandle(hBaseNode)) == NULL)
			return false;
		if ((pAxisXorYRefRod = (phAxisXorYRefRod == NULL) ? NULL : Ta.FromRodHandle(*phAxisXorYRefRod)) == NULL)
			return false;
		if (!pAxisXorYRefRod->IsAngle())
			return false;
		if (pRsltValue)
		{
			pRsltValue->ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;
			pRsltValue->objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_COORD3D;
			pRsltValue->objtype.wiPropValue = 0;
			GECS acs;
			SmartRodPtr pSmartRod = pAxisXorYRefRod;
			GEPOINT vNearAxisZ(0, 0, 1);
			if (ciAxisZNearDirect == 'X')
				vNearAxisZ.Set(1, 0, 0);
			else if (ciAxisZNearDirect == 'x')
				vNearAxisZ.Set(-1, 0, 0);
			else if (ciAxisZNearDirect == 'Y')
				vNearAxisZ.Set(0, 1, 0);
			else if (ciAxisZNearDirect == 'y')
				vNearAxisZ.Set(0, -1, 0);
			else if (ciAxisZNearDirect == 'Z')
				vNearAxisZ.Set(0, 0, 1);
			else if (ciAxisZNearDirect == 'z')
				vNearAxisZ.Set(0, 0, -1);
			double ddx = pSmartRod.pAngle->vxWingNorm*vNearAxisZ;
			double ddy = pSmartRod.pAngle->vyWingNorm*vNearAxisZ;
			if (fabs(ddx) > fabs(ddy))
				acs.axis_z = ddx > 0 ? pSmartRod.pAngle->vxWingNorm : -pSmartRod.pAngle->vxWingNorm;
			else
				acs.axis_z = ddy > 0 ? pSmartRod.pAngle->vyWingNorm : -pSmartRod.pAngle->vyWingNorm;
			if (ciAxisXorYNearDirect == 'X' || ciAxisXorYNearDirect == 'x')
			{
				if (ciAxisXorYNearDirect == 'X')
					acs.axis_x = pSmartRod->xPosEnd - pSmartRod->xPosStart;
				else if (ciAxisXorYNearDirect == 'x')
					acs.axis_x = pSmartRod->xPosStart - pSmartRod->xPosEnd;
				if (pSmartRod->pEnd->handle == hBaseNode)
					acs.axis_x = pSmartRod->xPosStart - pSmartRod->xPosEnd;
				if (!normalize(acs.axis_x))
					return false;
				acs.axis_y = acs.axis_z^acs.axis_x;
				if (!normalize(acs.axis_y))
					return false;
			}
			else
			{
				if (ciAxisXorYNearDirect == 'Y')
					acs.axis_y = pSmartRod->xPosEnd - pSmartRod->xPosStart;
				else if (ciAxisXorYNearDirect == 'y')
					acs.axis_y = pSmartRod->xPosStart - pSmartRod->xPosEnd;
				if (pSmartRod->pEnd->handle == hBaseNode)
					acs.axis_y = pSmartRod->xPosStart - pSmartRod->xPosEnd;
				if (!normalize(acs.axis_y))
					return false;
				acs.axis_x = acs.axis_y^acs.axis_z;
				if (!normalize(acs.axis_x))
					return false;
			}
			acs.origin = pSmartRod.pAngle->GetDatumPosBer(pBaseNode);
			//lenv=pStdTemplExpr->acs.TransPToCS(lenv);
			memcpy(pRsltValue->origin3d, acs.origin, 24);
			memcpy(pRsltValue->axisx3d, acs.axis_x, 24);
			memcpy(pRsltValue->axisy3d, acs.axis_y, 24);
			memcpy(pRsltValue->axisz3d, acs.axis_z, 24);
		}
		return true;
	}
};