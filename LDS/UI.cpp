#include "stdafx.h"
#include "UI.h"
#include "env_def.h"
#include "lds_part.h"

bool UI::blEnableIntermediateUpdateUI =true;
bool UI::blDisableIntermediateMessageBox =false;
void UI::UpdatePartSolid(CLDSPart* pPart,bool blUpdateScr/*=true*/)
{
	pPart->SetModified();
	pPart->Create3dSolidModel();
	if(blUpdateScr)
	{
		g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
		g_pSolidDraw->Draw();
	}
}
