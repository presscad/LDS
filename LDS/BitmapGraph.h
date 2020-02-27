// BitmapGraph.h: interface for the CGraphBitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BITMAPGRAPH_H__DCCBCB01_6B37_46DF_9685_E1FAE810DEB4__INCLUDED_)
#define AFX_BITMAPGRAPH_H__DCCBCB01_6B37_46DF_9685_E1FAE810DEB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBitmapGraph : public CBitmap  
{
	CDC *m_pdcMemory;
	CBitmap *m_pOldBitmap;
	int m_nWidth,m_nHeight;
public:
	CBitmapGraph();
	virtual ~CBitmapGraph();
	BOOL InitDC(
		HDC hdc,        // handle to DC
		int nWidth,     // width of bitmap, in pixels
		int nHeight);	// height of bitmap, in pixels
	BOOL ReInitDC(HDC hdc,int nWidth, int nHeight);
	int Width(){return m_nWidth;}
	int Height(){return m_nHeight;}
	CDC*GetDC(){return m_pdcMemory;}
	void Clear();
};

#endif // !defined(AFX_GRAPHBITMAP_H__DCCBCB01_6B37_46DF_9685_E1FAE810DEB4__INCLUDED_)
