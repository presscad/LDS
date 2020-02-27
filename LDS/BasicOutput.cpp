#include "stdafx.h"
#include "BasicOutput.h"
#include "OutputInfo.h"

static IBasicOutput* CreateNewTaskOutput(int idClsType,DWORD key,void* pContext)
{
	if(idClsType==1)
		return new COutputInfo();
	else if(idClsType==2)
		return new CCheckPartNoInfo();
	else if(idClsType==3)
		return new CCheckGasketErrorInfo();
	else
		return NULL;
}
static BOOL DeleteTaskOutput(IBasicOutput* pOutput)
{
	if(pOutput->GetTypeId()==1)
		delete (COutputInfo*)pOutput;
	else if(pOutput->GetTypeId()==2)
		delete (CCheckPartNoInfo*)pOutput;
	else if(pOutput->GetTypeId()==3)
		delete (CCheckGasketErrorInfo*)pOutput;
	else
		return FALSE;
	return true;
}
CTaskOutputList::CTaskOutputList()
{
	this->CreateNewAtom=CreateNewTaskOutput;
	this->DeleteAtom=DeleteTaskOutput;
}
COutputInfo* CTaskOutputList::AppendSimpleOutput()
{
	return (COutputInfo*)Add(0,1);
}
