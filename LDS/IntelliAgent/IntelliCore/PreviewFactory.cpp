#include "stdafx.h"
#include "Hashtable.h"
#include "PreviewObject.h"


CHashList<CPreviewObject*> hashPreviews;
class CPreviewFactoryLife{
public:
	~CPreviewFactoryLife(){
		for(CPreviewObject** ppEngine=hashPreviews.GetFirst();ppEngine;ppEngine=hashPreviews.GetNext())
		{
			if(*ppEngine==NULL)
				continue;
			CPreviewObject* pEngine=(CPreviewObject*)*ppEngine;
			delete pEngine;
		}
		hashPreviews.Empty();
	}
};
CPreviewFactoryLife life;
IPreview* IPreview::CreatePreview()
{
	long iNo=1;
	do{
		if(hashPreviews.GetValue(iNo)!=NULL)
			iNo++;
		else	//ÕÒµ½Ò»¸ö¿ÕºÅ
			break;
	}while(true);
	CPreviewObject* pEngine = new CPreviewObject(iNo);
	hashPreviews.SetValue(iNo,pEngine);
	return pEngine;
};
IPreview* IPreview::PreviewFromSerial(long serial)
{
	CPreviewObject** ppEngine=hashPreviews.GetValue(serial);
	if(ppEngine&&*ppEngine!=NULL)
		return *ppEngine;
	else
		return NULL;
}
BOOL IPreview::Destroy(long h)
{
	CPreviewObject** ppEngine=hashPreviews.GetValue(h);
	if(ppEngine==NULL||*ppEngine==NULL)
		return false;
	CPreviewObject* pEngine=(CPreviewObject*)*ppEngine;
	delete pEngine;
	return hashPreviews.DeleteNode(h);
}

