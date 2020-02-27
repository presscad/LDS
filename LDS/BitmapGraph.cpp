// BitmapGraph.cpp: implementation of the CGraphBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BitmapGraph.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBitmapGraph::CBitmapGraph()
{
	m_pdcMemory=new CDC;
	m_pOldBitmap=NULL;
}

CBitmapGraph::~CBitmapGraph()
{
	//if(m_pOldBitmap)
	//	m_pdcMemory->SelectObject(m_pOldBitmap);
	delete m_pdcMemory;
}
BOOL CBitmapGraph::InitDC(HDC hdc,int nWidth,int nHeight)
{
	BOOL bRetCode=CreateCompatibleBitmap(CDC::FromHandle(hdc),nWidth,nHeight);
	if(bRetCode)
	{
		m_nWidth=nWidth;
		m_nHeight=nHeight;
	}
	else
		return bRetCode;
	m_pdcMemory->CreateCompatibleDC(CDC::FromHandle(hdc));
	m_pOldBitmap=m_pdcMemory->SelectObject(this);
	m_pdcMemory->SelectClipRgn(NULL);
	m_pdcMemory->IntersectClipRect(&CRect(0,0,nWidth,nHeight));
	m_pdcMemory->SelectStockObject(BLACK_BRUSH);
	
	m_pdcMemory->PatBlt(0,0,nWidth,nHeight,PATCOPY);
	return TRUE;
}

BOOL CBitmapGraph::ReInitDC(HDC hdc,int nWidth,int nHeight)
{
	if(m_pOldBitmap==NULL)
		return FALSE;
	else
		m_pdcMemory->SelectObject(m_pOldBitmap);
	DeleteObject();
	BOOL bRetCode=CreateCompatibleBitmap(CDC::FromHandle(hdc),nWidth,nHeight);
	m_pOldBitmap=m_pdcMemory->SelectObject(this);
	BITMAP bm;
	GetBitmap(&bm);
	m_nWidth=nWidth;
	m_nHeight=nHeight;
	m_pdcMemory->SelectClipRgn(NULL);
	m_pdcMemory->IntersectClipRect(&CRect(0,0,nWidth,nHeight));
	
	m_pdcMemory->PatBlt(0,0,nWidth,nHeight,PATCOPY);
	return TRUE;
}
void CBitmapGraph::Clear()
{
	m_pdcMemory->SelectStockObject(BLACK_BRUSH);
	m_pdcMemory->PatBlt(0,0,m_nWidth,m_nWidth,PATCOPY);
}
