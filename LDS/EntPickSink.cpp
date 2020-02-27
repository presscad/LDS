//<LOCALE_TRANSLATE Confirm by hxr/>
#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "LDSView.h"
#include "GUID.h"
#include "DefBmpParaDlg.h"
#include "env_def.h"

STDMETHODIMP_(ULONG)CLDSView::XEntPickEventSink::AddRef()
{
	return 1;
}
STDMETHODIMP_(ULONG)CLDSView::XEntPickEventSink::Release()
{
	return 0;
}
STDMETHODIMP CLDSView::XEntPickEventSink::QueryInterface(REFIID iid,LPVOID* ppvObj)
{
	METHOD_PROLOGUE_EX_(CLDSView,EntPickEventSink)
	if( IsEqualIID(iid,IID_IUnknown) ||
		IsEqualIID(iid,IID_IDispatch)||
		IsEqualIID(iid,IID_IPickEventSet))
	{
		*ppvObj=this;
		AddRef();
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}
STDMETHODIMP CLDSView::XEntPickEventSink::GetTypeInfoCount(unsigned int *)
{
	return E_NOTIMPL;
}
STDMETHODIMP CLDSView::XEntPickEventSink::GetTypeInfo(unsigned int,LCID,ITypeInfo **)
{
	return E_NOTIMPL;
}
STDMETHODIMP CLDSView::XEntPickEventSink::GetIDsOfNames(REFIID,LPOLESTR*,unsigned int,LCID,DISPID *)
{
	return E_NOTIMPL;
}
STDMETHODIMP CLDSView::XEntPickEventSink::Invoke(DISPID dispid,REFIID,LCID,unsigned short wFlags,DISPPARAMS *pDispParams, VARIANT *pvarResult,EXCEPINFO *pExcepInfo,
												 unsigned int *puArgError)
{
	METHOD_PROLOGUE_EX_(CLDSView,EntPickEventSink)
	if(dispid==0)
	{
		if(pDispParams->cArgs>0)
		{
			THANDLE hGroupPart=pDispParams->rgvarg[0].lVal;
			long ret_id=pThis->EntPickEvent(hGroupPart);
			if(pvarResult)
			{
				pvarResult->lVal=ret_id;
				pvarResult->vt=VT_I4;
			}
			return S_OK;
		}
		else
			return S_FALSE;
	}
	else if(dispid==1)
	{
		if(pDispParams->cArgs>0)
		{
			int left=pDispParams->rgvarg[3].lVal;
			int bottom=pDispParams->rgvarg[2].lVal;
			int right=pDispParams->rgvarg[1].lVal;
			int top=pDispParams->rgvarg[0].lVal;
			pThis->ExportBmpFile(left,bottom,right,top);
			return S_OK;
		}
		else
			return S_FALSE;
	}
	else if(dispid==2)
	{
		if(pDispParams->cArgs>0)
		{
			THANDLE hPart=pDispParams->rgvarg[1].lVal;
			int idClassType=pDispParams->rgvarg[0].lVal;
			CLDSPart *pPart = console.FromPartHandle(hPart,idClassType);

			if(pvarResult)
			{
				pvarResult->lVal = 0;
				pvarResult->vt=VT_I4;
				if(pPart)
				{
					try{
						if(pPart->pSolidBody==NULL)
							pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
					}
					catch(char *sError)
					{
						AfxMessageBox(sError);
					}

					pvarResult->lVal =(long)pPart->pSolidBody;
					pvarResult->vt=VT_I4;
				}
			}
			return S_OK;
		}
		else
			return S_FALSE;
	}
	else
	{
		return S_FALSE;
	}
}

long CLDSView::EntPickEvent(long ID)
{
	/*HINSTANCE hOldResHandle=AfxGetResourceHandle();
	AfxSetResourceHandle(m_hResHandle);
	//...
	long ret_id=ID;
	CLDSGroupLineAngle *pGroupJg;
	if(console.FromPartHandle(ID,CLS_LINEANGLE))
		goto end;	//普通直角钢
	else
		pGroupJg=(CLDSGroupLineAngle*)console.FromPartHandle(ID,CLS_GROUPLINEANGLE);
	if(pGroupJg==NULL)
		goto end;
	ReleaseCapture();	//否则需要两次点击才能对弹出的对话框进行鼠标操作
	if(pGroupJg->group_style==0)	//对角组合
	{
		CSel2CrossGroupJgDlg dlg;
		dlg.DoModal();
		if(dlg.m_iCurOperJg==0)
			goto end;
		else if(dlg.m_iCurOperJg==1)
		{
			ID=pGroupJg->son_jg_h[0];
			goto end;
		}
		else if(dlg.m_iCurOperJg==2)
		{
			ID=pGroupJg->son_jg_h[1];
			goto end;
		}
		else
			goto end;
	}
	else if(pGroupJg->group_style==1)	//T型组合
	{
		CSelTGroupJgDlg dlg;
		dlg.DoModal();
		if(dlg.m_iCurOperJg==0)
			goto end;
		else if(dlg.m_iCurOperJg==1)
		{
			ID=pGroupJg->son_jg_h[0];
			goto end;
		}
		else if(dlg.m_iCurOperJg==2)
		{
			ID=pGroupJg->son_jg_h[1];
			goto end;
		}
		else
			goto end;
	}
	else if(pGroupJg->group_style==2)	//十字交叉组合
	{
		CSel4CrossGroupJgDlg dlg;
		dlg.DoModal();
		if(dlg.m_iCurOperJg==0)
			goto end;
		else if(dlg.m_iCurOperJg==1)
		{
			ID= pGroupJg->son_jg_h[0];
			goto end;
		}
		else if(dlg.m_iCurOperJg==2)
		{
			ID=pGroupJg->son_jg_h[1];
			goto end;
		}
		else if(dlg.m_iCurOperJg==3)
		{
			ID=pGroupJg->son_jg_h[2];
			goto end;
		}
		else if(dlg.m_iCurOperJg==4)
		{
			ID=pGroupJg->son_jg_h[3];
			goto end;
		}
		else
			goto end;
	}
	//...
end:
	AfxSetResourceHandle(hOldResHandle);*/
	return ID;
}
void CLDSView::ExportBmpFile(int left,int bottom,int right,int top)
{
	HINSTANCE hOldResHandle=AfxGetResourceHandle();
	AfxSetResourceHandle(m_hResHandle);
	//...
	char file[MAX_PATH];
	CDefBmpParaDlg bmpparadlg;
	CFileDialog dlg(FALSE,"bmp","LDS",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"24 bitmap file(*.bmp)|*.bmp||");
#else
		"24位位图文件(*.bmp)|*.bmp||");
#endif
	if(dlg.DoModal()!=IDOK)
		goto end;
	bmpparadlg.m_nBmpWidth=right-left;
	bmpparadlg.m_nBmpHeight=top-bottom;
	_snprintf(file,MAX_PATH-1,"%s",dlg.GetFileName());
	if(bmpparadlg.DoModal()==IDOK)
		g_pSolidDraw->ExportBmpFile(file,left,bottom,right,top,bmpparadlg.m_nBmpWidth,bmpparadlg.m_nBmpHeight);
end:
	AfxSetResourceHandle(hOldResHandle);
}