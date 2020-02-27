#include "stdafx.h"
#include "LogFile.h"
#include "ImageRecognizer.h"
#include "PartLib.h"	//QuerySteelBriefMatMark(sMeterial);


#if defined(__TMA_)||defined(__LMA_)
static int f2i(double fVal)
{
	if(fVal>0)
		return (int)(fVal+0.5);
	else
		return (int)(fVal-0.5);
}

const BYTE BIT2BYTE[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
//////////////////////////////////////////////////////////////////////////
//CBitImage
bool IImage::SetPixelState(int i,int j,bool black/*=true*/)
{
	if(i<0||j<0||i>=m_nWidth||j>=m_nHeight)
		return false;
	if(IsPixelBitMode())
	{
		int iByte=i/8,iBit=i%8;
		if(!BlackIsTrue())
			black=!black;
		if(black)
			m_lpBitMap[j*m_nEffWidth+iByte]|=BIT2BYTE[7-iBit];
		else
			m_lpBitMap[j*m_nEffWidth+iByte]&=(0XFF-BIT2BYTE[7-iBit]);
	}
	else
		m_lpBitMap[j*m_nEffWidth+i]=black?1:0;
	return black;
}
bool IImage::IsBlackPixel(int i,int j)
{
	if( i<0||i>=m_nWidth || j<0||j>=m_nHeight)
		return false;
	if(IsPixelBitMode())
	{
		int iByte=i/8,iBit=i%8;
		int gray=m_lpBitMap[j*m_nEffWidth+iByte]&BIT2BYTE[7-iBit];	//获取该点的灰度值
		//如果这个象素是黑色的
		return BlackIsTrue() ? (gray==BIT2BYTE[7-iBit]) : (gray!=BIT2BYTE[7-iBit]);
	}
	else
		return BlackIsTrue() ? m_lpBitMap[j*m_nEffWidth+i]!=0 : m_lpBitMap[j*m_nEffWidth+i]==0;
}
CBitImage::CBitImage(BYTE* lpBitMap,int width,int height)
{
	m_bBlackPixelIsTrue=false;
	InitBitImage(lpBitMap,width,height);
}
void CBitImage::InitBitImage(BYTE* lpBitMap,int width,int height)
{
	m_lpBitMap=lpBitMap;
	m_bExternalData=m_lpBitMap!=NULL;
	m_nWidth=width;
	m_nHeight=height;
	m_nEffWidth=m_nWidth%8>0 ? m_nWidth/8+1 : m_nWidth/8;
	//m_nEffWidth=m_nWidth%16>0?(m_nWidth/16)*2+2:m_nWidth/8;
	if(m_lpBitMap==NULL&&m_nEffWidth>0&&m_nHeight>0)
	{
		int pixels=m_nEffWidth*m_nHeight;
		m_lpBitMap=new BYTE[pixels];
		memset(m_lpBitMap,0,pixels);
	}
	clip.left=clip.top=0;
	clip.right=width;
	clip.bottom=height;
}
bool CBitImage::IsBlackPixel(int i,int j)
{
	if( i<clip.left || i>=clip.right|| j<clip.top|| j>=clip.bottom ||
		i>=m_nWidth || j>=m_nHeight)
		return false;
	return IImage::IsBlackPixel(i,j);
	//int iByte=i/8,iBit=j%8;
	//int gray=m_lpBitMap[j*m_nEffWidth+iByte]&BIT2BYTE[7-iBit];	//获取该点的灰度值
	//如果这个象素是黑色的
	//return m_bBlackPixelIsTrue ? (gray==BIT2BYTE[7-iBit]) : (gray!=BIT2BYTE[7-iBit]);
}
CBitImage::~CBitImage()
{
	if(m_lpBitMap&&!m_bExternalData)
	{
		delete []m_lpBitMap;
		m_lpBitMap=NULL;
	}
}
//////////////////////////////////////////////////////////////////////////
//IImageZoomTrans
bool IImageZoomTrans::InitImageZoomTransData(const RECT& src,const RECT& dest)
{
	_src=src;
	_dest=dest;
	xCoefDest2Src=dest.right-dest.left;
	yCoefDest2Src=dest.bottom-dest.top;
	if(src.right-src.left<=0 || src.bottom-src.top<=0)
		return false;
	xCoefDest2Src/=src.right-src.left;
	yCoefDest2Src/=src.bottom-src.top;
	xOffsetOrg=yOffsetOrg=0.0;
	if(xCoefDest2Src<=yCoefDest2Src)
		yOffsetOrg=0.5*((_dest.bottom-_dest.top)/xCoefDest2Src-(_src.bottom-_src.top));
	else if(xCoefDest2Src>yCoefDest2Src)
		xOffsetOrg=0.5*((_dest.right-_dest.left)/yCoefDest2Src-(_src.right-_src.left));
	minCoefDest2Src=xCoefDest2Src<yCoefDest2Src?xCoefDest2Src:yCoefDest2Src;
	return true;
}
bool IImageZoomTrans::InitImageZoomTransDataByCenter(const RECT& src,const RECT& dest)
{
	UINT wStdWidth =dest.right-dest.left;
	UINT wStdHeight=dest.bottom-dest.top;
	UINT m_nWidth =src.right-src.left;
	UINT m_nHeight=src.bottom-src.top;
	double scaleofX=wStdWidth/(double)m_nWidth;
	double scaleofY=wStdHeight/(double)m_nHeight;
	if(scaleofX>=scaleofY)
	{
		double gap_x=wStdWidth-m_nWidth*scaleofY;
		int offset_x=f2i(gap_x*0.5);
		int offset_rx=f2i(gap_x-offset_x);
		int offset_y=0;
		return InitImageZoomTransData(src,CRect(offset_x,offset_y,wStdWidth-offset_rx,wStdHeight-offset_y));
	}
	else
	{
		int offset_x=0;
		double gap_y=wStdHeight-m_nHeight*scaleofX;
		int offset_y=f2i(gap_y*0.5);
		int offset_ry=f2i(gap_y-offset_y);
		return InitImageZoomTransData(src,CRect(offset_x,offset_y,wStdWidth-offset_x,wStdHeight-offset_ry));
	}
}
POINT IImageZoomTrans::TransFromDestImage(int xiDest,int yjDest)
{
	POINT srcpt;
	srcpt.x=f2i(_src.left-xOffsetOrg+(xiDest-_dest.left)/minCoefDest2Src);
	srcpt.y=f2i(_src.top -yOffsetOrg+(yjDest-_dest.top )/minCoefDest2Src);
	return srcpt;
}
bool IImageZoomTrans::IsBlackPixelAtDest(int xiDest,int yjDest,IImage* pSrcImage)
{
	//double minCoefDest2Src=xCoefDest2Src<yCoefDest2Src?xCoefDest2Src:yCoefDest2Src;
	double xifSrcX=_src.left-xOffsetOrg+(xiDest-_dest.left)/minCoefDest2Src;
	double yjfSrcY=_src.top -yOffsetOrg+(yjDest-_dest.top )/minCoefDest2Src;
	//以后应计算四边形面积内的黑白区域比例来精确计算是否为黑点
	double increment=1/minCoefDest2Src;
	double area=increment*increment,blackarea=0;
	int lowi=(int)floor(xifSrcX),highi=(int)ceil(xifSrcX+increment);
	int lowj=(int)floor(yjfSrcY),highj=(int)ceil(yjfSrcY+increment);
	for(int i=lowi;i<highi;i++)
	{
		if(pSrcImage->IsBlackPixel(i,lowj))
			blackarea-=yjfSrcY-lowj;	//扣除顶部多算黑色区域
		if(pSrcImage->IsBlackPixel(i,highj-1))
			blackarea-=highj-yjfSrcY-increment;	//扣除底部多算黑色区域
		for(int j=lowj;j<highj;j++)
		{
			if(pSrcImage->IsBlackPixel(i,j))
				blackarea+=1.0;
		}
	}
	for(int j=lowj;j<highj;j++)
	{
		if(pSrcImage->IsBlackPixel(lowi,j))
			blackarea-=xifSrcX-lowi;	//扣除左侧多算黑色区域
		if(pSrcImage->IsBlackPixel(highi-1,j))
			blackarea-=highi-xifSrcX-increment;	//扣除右侧多算黑色区域
	}
	if(pSrcImage->IsBlackPixel(lowi,lowj))
		blackarea+=(xifSrcX-lowi)*(yjfSrcY-lowj);
	if(pSrcImage->IsBlackPixel(highi-1,lowj))
		blackarea+=(highi-xifSrcX-increment)*(yjfSrcY-lowj);
	if(pSrcImage->IsBlackPixel(highi-1,highj-1))
		blackarea+=(highi-xifSrcX-increment)*(highj-yjfSrcY-increment);
	if(pSrcImage->IsBlackPixel(lowi,highj-1))
		blackarea+=(xifSrcX-lowi)*(highj-yjfSrcY-increment);
	return blackarea/area>=0.5;
	//int xiSrcX=f2i(xifSrcX);
	//int yjSrcY=f2i(yjfSrcY);
	//return pSrcImage->IsBlackPixel(xiSrcX,yjSrcY);
}
//////////////////////////////////////////////////////////////////////////
//CImageChar
CImageChar::CImageChar()
{
	_nDestValidWidth=0;
	m_nBlckPixels=m_nPixels=0;
	m_lpBitMap=NULL;
	wChar.wHz=0;
	m_bTemplEnabled=true;
}
CImageChar::~CImageChar()
{
	if(m_lpBitMap)
		delete []m_lpBitMap;
}
bool CImageChar::IsBlackPixel(int i,int j)
{
	if(i<0||j<0||i>=m_nWidth||j>=m_nHeight)
		return false;
	return (m_lpBitMap[j*m_nWidth+i]&0x7f)!=0;
}
void CImageChar::CopyImageChar(CImageChar* pChar)
{
	m_nEffWidth=m_nWidth=pChar->GetImageWidth();
	m_nHeight=pChar->GetImageHeight();
	m_nPixels=m_nWidth*m_nHeight;
	m_lpBitMap=new BYTE[m_nPixels];
	memcpy(m_lpBitMap,pChar->GetImageMap(),m_nPixels);
	m_nBlckPixels=pChar->m_nBlckPixels;
	_nDestValidWidth=pChar->m_nDestValidWidth;
	wChar=pChar->wChar;
}
void CImageChar::InitTemplate(BYTE* lpBits,int eff_width,RECT tem_rect)
{
	bool bBlack=false,bLab=false;
	RECT rect;
	rect.left=rect.right=rect.top=rect.bottom=0;
	for(int i=tem_rect.left;i<tem_rect.right;i++)
	{
		bBlack=false;
		for(int j=tem_rect.top;j<tem_rect.bottom;j++)
		{
			int iByte=i/8,iBit=i%8;
			BYTE gray=lpBits[j*eff_width+iByte]&BIT2BYTE[7-iBit];
			if(gray!=BIT2BYTE[7-iBit])	
			{
				bBlack=true;
				if(bLab==false)
				{
					rect.left=i;
					bLab=true;
				}
				else
					break;
			}
		}
		if(bLab==true&&bBlack==false)
		{   
			rect.right=i;
			break;
		}
		if(i==tem_rect.right-1)
		{
			rect.right=tem_rect.right;
			break;
		}
	}
	//
	if(wChar.wHz=='-'||wChar.wHz=='_'||wChar.wHz=='.')
	{	//这三个字符上下位置有特殊要求，不能随便定位
		rect.top=tem_rect.top;
		rect.bottom=tem_rect.bottom;
	}
	else
	{
		bLab=false;
		for(int j=tem_rect.top;j<tem_rect.bottom;j++)
		{
			bBlack=false;
			for(int i=tem_rect.left;i<tem_rect.right;i++)
			{
				int iByte=i/8,iBit=i%8;
				BYTE gray=lpBits[j*eff_width+iByte]&BIT2BYTE[7-iBit];
				if(gray!=BIT2BYTE[7-iBit])	
				{
					bBlack=true;
					if(bLab==false)
					{
						rect.top=j;
						bLab=true;
					}
					else
						break;
				}
			}
			if(bLab==true&&bBlack==false)
			{   
				rect.bottom=j;
				break;
			}
			if(j==tem_rect.bottom-1)
			{
				rect.bottom=tem_rect.bottom;
				break;
			}
		}
	}
	Localize(lpBits,eff_width,rect);
}
void CImageChar::Localize(BYTE* lpBits,int eff_width,RECT rect)
{
	m_nHeight=max(0,rect.bottom-rect.top);
	if(m_lpBitMap)
		delete []m_lpBitMap;
	m_nEffWidth=m_nWidth=(rect.right-rect.left);
	m_nPixels=m_nWidth*m_nHeight;
	m_lpBitMap=new BYTE[m_nPixels];
	memset(m_lpBitMap,0,m_nPixels);
	m_nBlckPixels=0;
	for(int i=rect.left;i<rect.right;i++)
	{
		for(int j=rect.top;j<rect.bottom;j++)
		{
			int iByte=i/8,iBit=i%8;
			int iDestByte=i-rect.left,iDestBit=(i-rect.left)%8;
			BYTE gray=lpBits[j*eff_width+iByte]&BIT2BYTE[7-iBit];
			if(gray!=BIT2BYTE[7-iBit])	
			{
				m_lpBitMap[(j-rect.top)*m_nWidth+iDestByte]=1;
				m_nBlckPixels++;
			}
		}
	}
}
void CImageChar::Standardize(WORD wStdWidth,WORD wStdHeight)
{
	RECT rc;
	rc.left=rc.top=0;
	rc.right=m_nWidth;//rect.right-rect.left;
	rc.bottom=m_nHeight;//rect.bottom-rect.top;
	InitImageZoomTransDataByCenter(rc,CRect(0,0,wStdWidth,wStdHeight));
	BYTE_ARRAY bitsmap(wStdWidth*wStdHeight);
	memset(bitsmap,0,bitsmap.Size());
	m_nBlckPixels=0;
	for(int i=0;i<(int)wStdWidth;i++)
	{
		for(int j=0;j<(int)wStdHeight;j++)
		{
			if(IsBlackPixelAtDest(i,j,this))
			{
				bitsmap[j*wStdWidth+i]=1;
				m_nBlckPixels++;
			}
		}
	}
	if(bitsmap.Size()>(DWORD)m_nPixels)
	{
		m_lpBitMap=new BYTE[bitsmap.Size()];
		m_nPixels=max(m_nPixels,(int)bitsmap.Size());
	}
	m_nEffWidth=m_nWidth=wStdWidth;
	m_nHeight=wStdHeight;
	memcpy(m_lpBitMap,bitsmap,bitsmap.Size());
}
//对字符进行规则化处理
void CImageChar::StandardLocalize(BYTE* lpBits,int eff_width,const RECT& rc,int wStdHeight)
{
	//rect=rc;
	CBitImage bitmap(lpBits,eff_width*8,rc.bottom);
	bitmap.SetClipRect(rc);
	m_nEffWidth=m_nWidth=m_nHeight=wStdHeight;
	int nNewPixels=wStdHeight*wStdHeight;
	if(m_lpBitMap&&m_nPixels!=nNewPixels)
	{
		delete[] m_lpBitMap;
		m_lpBitMap=NULL;
	}
	if(m_lpBitMap==NULL)
		m_lpBitMap=new BYTE[nNewPixels];
	m_nPixels=nNewPixels;
	InitImageZoomTransDataByCenter(rc,CRect(0,0,wStdHeight,wStdHeight));
	m_nBlckPixels=0;
	_nDestValidWidth=0;
	int left=m_nMaxTextH,right=0;
	for(int i=0;i<m_nWidth;i++)
	{
		for(int j=0;j<m_nHeight;j++)
		{
			m_lpBitMap[j*m_nWidth+i]=0;
			//POINT pt=TransFromDestImage(i,j);
			//if(pt.x<rc.left || pt.x>=rc.right || pt.y<rc.top|| pt.y>=rc.bottom)
			//	continue;
			//int iByte=pt.x/8,iBit=pt.x%8;
			//int gray=lpBits[pt.y*eff_width+iByte]&BIT2BYTE[7-iBit];	//获取该点的灰度值
			//if(gray!=BIT2BYTE[7-iBit])	//如果这个象素是黑色的
			if(IsBlackPixelAtDest(i,j,&bitmap))
			{
				left =i<left ? i : left;
				right=i>right? i : right;
				m_lpBitMap[j*m_nMaxTextH+i]=1;
				m_nBlckPixels++;
			}
		}
	}
	_nDestValidWidth=right-left+1;
	return;
	//计算规则化区域
	/*int w=0,w1=0,w2=0;
	w=m_nMaxTextH-(rect.right-rect.left);
	if(w>1)
	{
		w1=w/2;
		w2=w-w1;
	}
	else
		w1=w;
	int h=0,h1=0,h2=0;
	h=m_nMaxTextH-(rect.bottom-rect.top);
	if(h>1)
	{
		h1=h/2;
		h2=h-h1;
	}
	else
		h1=h;
	//初始化像素特征值
	for(int i=0;i<m_nMaxTextH;i++)
	{
		for(int j=0;j<m_nMaxTextH;j++)
		{
			m_lpBitMap[j*m_nMaxTextH+i]=0;
			if(i<w1 || i>w1+(rect.right-rect.left))
				continue;
			if(j<h1 || j>h1+(rect.bottom-rect.top))
				continue;
			int x=rect.left+i-w1;
			int y=rect.top+j-h1;
			int iByte=x/8,iBit=x%8;
			int gray=lpBits[y*eff_width+iByte]&BIT2BYTE[7-iBit];	//获取该点的灰度值
			if(gray!=BIT2BYTE[7-iBit])	//如果这个象素是黑色的
			{
				m_lpBitMap[j*m_nMaxTextH+i]=1;
				m_nBlckPixels++;
			}
		}
	}*/
}
MATCHCOEF::MATCHCOEF(double originalCoef/*=0.0*/,double weightingCoef/*=0.0*/)
{
	original=originalCoef;
	weighting=weightingCoef;
}
int MATCHCOEF::CompareWith(const MATCHCOEF& coef)
{	//比对原则：以修正后为基准，但修正后差别不大时，以修正前为基准
	double scale=coef.weighting/weighting;
	if(scale>1.05)
		return -1;
	else if(scale<0.952381)
		return 1;
	double scale2=coef.original/original;
	if(scale2>1.05)
		return -1;
	else if(scale2<0.952381)
		return 1;
	double scale12=fabs(scale-1)<fabs(scale2-1)?scale2:scale;
	if(scale12>1)
		return -1;
	else
		return 1;
}
bool CImageChar::IsDetect(int i,int j)
{
	return (0x80&m_lpBitMap[j*m_nWidth+i])>0;
}
bool CImageChar::SetDetect(int i,int j,bool detect/*=true*/)
{
	BYTE* pxyPixel=&m_lpBitMap[j*m_nMaxTextH+i];
	if(detect)
		(*pxyPixel)|=0x80;
	else if((*pxyPixel)>=0x80)
		(*pxyPixel)-=0x80;
	return ((*pxyPixel)&0x80)!=0;
}
void CImageChar::ClearDetectMark()
{
	for(int i=0;i<m_nWidth;i++)
		for(int j=0;j<m_nHeight;j++)
			m_lpBitMap[j*m_nWidth+i]&=0x7f;
}
static int island_id=1;
bool CImageChar::StatNearWhitePixels(int i,int j,ISLAND* pIsland,BYTE cDetectFlag/*=0x0f*/,CLogFile* pLogFile/*=NULL*/)
{
	if(i==0||j==0||i==m_nWidth-1||j==m_nHeight-1)
	{
		if(j==0)
			StatNearWhitePixels(i,1,pIsland,cDetectFlag);
		if(pLogFile)
			pLogFile->Log("%d#(%2d,%2d)=false",island_id,i,j);
		return false;	//白色像素与边界连通在一起，无法再形成白色孤岛
	}
	BYTE* pxyPixel=&m_lpBitMap[j*m_nWidth+i];
	if(((*pxyPixel)&0x80)==0x80)
		return true;	//已检测过的节点
	*pxyPixel|=0x80;	//设置已检测标记
	if((*pxyPixel)>0x80)
		return true;	//当前为黑点停止统计搜索
	if(pLogFile)
		pLogFile->Log("(%2d,%2d)=%d",i,j,island_id);
	pIsland->x+=i;		//累积形心计算参数值
	pIsland->y+=j;
	pIsland->count+=1;
	bool canconinue=true;
	for(int jj=j-1;jj<=j+1;jj++)
	{
		if(jj<0||jj>=m_nHeight)
			continue;
		if(jj<j&&!(cDetectFlag&DETECT_UP)||jj>j&&!(cDetectFlag&DETECT_DOWN)||jj==j&&!(cDetectFlag&DETECT_Y0))
			continue;
		for(int ii=i-1;ii<=i+1;ii++)
		{
			if(ii<0||ii>=m_nWidth||(ii==i&&jj==j))
				continue;
			if(ii<i&&!(cDetectFlag&DETECT_LEFT)||ii>i&&!(cDetectFlag&DETECT_RIGHT)||ii==i&&!(cDetectFlag&DETECT_X0))
				continue;
			if(!(canconinue=StatNearWhitePixels(ii,jj,pIsland,cDetectFlag)))
				return false;
		}
	}
	return canconinue;
}
int CImageChar::DetectIslands(CXhSimpleList<ISLAND>* listIslands)
{
	int count=0;
	int rowstarti=0;
	//CLogFile logfile(CXhChar50("c:/%s_detect.txt",wChar.chars));
	//CLogErrorLife life(&logfile);
	ClearDetectMark();
	island_id=1;
	for(int j=0;j<m_nHeight;j++)
	{
		ISLAND island;
		bool blackpixelStarted=false,blackpixelEnded=false;
		bool whitepixelStarted=false;
		for(int i=0;i<m_nWidth;i++)
		{
			if(m_lpBitMap[rowstarti+i]==1)
			{
				blackpixelStarted=true;
				if(whitepixelStarted)
					blackpixelEnded=true;
			}
			else if(blackpixelStarted&&m_lpBitMap[rowstarti+i]==0)
			{
				whitepixelStarted=true;
				if(StatNearWhitePixels(i,j,&island,DETECT_Y0|DETECT_DOWN|DETECT_LEFT|DETECT_X0|DETECT_RIGHT))//,&logfile))
				{
					listIslands->AttachObject(island);
					//logfile.Log("%d#island finded @%2d-%2d",island_id,i,j);
					island_id++;
					island.Clear();
					count++;
				}
			}
			else if((m_lpBitMap[rowstarti+i]&0x7f)==0)
			{
				StatNearWhitePixels(i,j,&island,DETECT_Y0|DETECT_DOWN|DETECT_LEFT|DETECT_X0|DETECT_RIGHT);//,&logfile);
				island.Clear();
			}
			//if(blackpixelStarted&&whitepixelStarted&&blackpixelEnded)
		}
		rowstarti+=m_nWidth;
	}
	return count;
}
int CImageChar::CheckFeatures(CXhSimpleList<ISLAND>* pIslands/*=NULL*/)
{
	CXhSimpleList<ISLAND> islands;
	int count=0;
	ISLAND* pIsland=NULL;
	if(pIslands==NULL)
	{
		pIslands=&islands;
		count=DetectIslands(&islands);
	}
	for(pIsland=pIslands->EnumObjectFirst();pIsland;pIsland=pIslands->EnumObjectNext())
		count++;
	pIsland=pIslands->EnumObjectFirst();
	double core_y=0;
	if(pIsland)
		core_y=pIsland->y/pIsland->count;
	if(wChar.wHz=='0')
	{
		if(count!=1)
			return -1;
		if(core_y<=m_nHeight*0.4&&core_y>=m_nHeight*0.6)
			return -1;
		else if(pIsland->count>4)
			return 1;
	}
	else if(wChar.wHz=='1')
	{
		if(pIsland!=NULL)
			return -1;	//'1'字符不可能有孤岛存在
	}
	else if(wChar.wHz=='2')
	{
		if(pIsland!=NULL)
			return -1;	//'2'字符不可能有孤岛存在
	}
	else if(wChar.wHz=='3')
	{
		if(pIsland!=NULL)
			return -1;	//'3'字符不可能有孤岛存在
	}
	else if(wChar.wHz=='4')
	{
		if(count>1)
			return -1;
		else if(count<=0)
			return 0;	//由于图像转换，可能导致中间的孤岛在转为黑白图时丢失
		else if(core_y>m_nHeight*0.6||core_y<m_nHeight*0.4)
			return -1;	//'4'字符的孤岛应该在中间范围,以示与'6'及'9'等字符区分
		//if(pIsland!=NULL)
		//	return -1;	//'1'字符不可能有孤岛存在
	}
	else if(wChar.wHz=='5')
	{
		if(count>1)
			return -1;	//'5'字符原则上不可能有孤岛存在，但部分情况如305分割字符时，由于0与5粘连在一块可能导致一个错误孤岛，故设为孤岛>2为肯定不合理
	}
	else if(wChar.wHz=='7')
	{
		if(pIsland!=NULL)
			return -1;	//'7'字符不可能有孤岛存在
	}
	//else if(wChar.wHz=='H')
	else if(wChar.wHz=='.')
	{
		int blackpixels=0;
		int leftblackpixels2={0};
		for(int i=0;i<m_nWidth;i++)
		{
			for(int j=0;j<m_nHeight/2;j++)
			{
				if(IsBlackPixel(i,j))
				{
					if(i>2)
						leftblackpixels2+=1;
					blackpixels++;
				}
			}
		}
		if(blackpixels<4&&leftblackpixels2<=2)
			return 1;
	}
	else if(wChar.wHz=='-')
	{
		if(pIsland!=NULL)
			return -1;	//'-'字符不可能有孤岛存在
	}
	else if(wChar.wHz=='.')
	{
		if(pIsland!=NULL)
			return -1;	//'.'字符不可能有孤岛存在
	}
	else if(wChar.wHz=='6')	//下半层有一个白色孤岛
	{
		if(count!=1)
			return -1;
		if(pIsland->count>=2&&core_y>m_nHeight*0.5)
			return 1;
		else
			return -1;
	}
	else if(wChar.wHz=='8')
	{
		if(count!=2)
			return -1;
		ISLAND* pIslandUp=pIsland;
		ISLAND* pIslandDown=pIslands->EnumObjectNext();
		if(pIslandUp==NULL||pIslandDown==NULL)
			return -1;
		if(pIslandUp->y>pIslandDown->y)
		{
			pIslandUp=pIslandDown;
			pIslandDown=pIsland;
		}
		if((pIslandUp->y/pIslandUp->count<m_nHeight*0.5)&&(pIslandDown->y/pIslandDown->count>m_nHeight*0.5))
		{
			if(pIslandUp->count>=2&&pIslandDown->count>=3)
				return 1;
			else
				return 0;
		}
		else
			return -1;
	}
	else if(wChar.wHz=='9')
	{
		if(count!=1)
			return -1;
		if(core_y>2&&core_y<m_nHeight*0.45)
			return 1;
		else
			return -1;
	}
	/*else if(wChar.wHz=='X')
	else if(wChar.wHz=='A')
	else if(wChar.wHz=='B')
	else if(wChar.wHz=='C')*/
	/*else if(wChar.wHz=='Q')
	{
		if(count!=1)
			return -1;
		if(core_y<m_nMaxTextH*0.25||core_y>m_nMaxTextH*0.75)
			return -1;
		else
			return 1;
	}
	else if(wChar.wHz=='P')
	{
		if(count!=1)
			return -1;
		if(core_y>2&&core_y<m_nMaxTextH/2)
			return 1;
		else
			return -1;
	}
	else if(wChar.wHz!=4&&pIsland&&pIsland->count>4)
		return -1;*/
	return 0;
}
//比较两字符是否相同
MATCHCOEF CImageChar::CalMatchCoef(CImageChar* pText,char* pciOffsetX/*=NULL*/,char* pciOffsetY/*=NULL*/)
{
	MATCHCOEF matchcoef;
	int i,j,k,matchCountArr[5]={0};
	if(wChar.wHz=='1'&&pText->m_nDestValidWidth>m_nWidth/2)
		return MATCHCOEF();
	//1.为了区分'1'与'4'字符，'4'字符至少占半个格宽度
	//2.由于个别情况４字符处理灰度->黑白图时导致字符减小，故在原有半格基础上再减1个像素 wjh-2016.5.25
	if(wChar.wHz=='4'&&pText->m_nDestValidWidth<=m_nWidth/2-1)
		return MATCHCOEF();
	for(i=0;i<m_nMaxTextH;i++)
	{
		for(j=0;j<m_nMaxTextH;j++)
		{
			if(!pText->IsBlackPixel(i,j))
				continue;
			bool black_l2,black_l1,black_0,black_r1,black_r2;
			if(wChar.wHz=='-'||wChar.wHz=='_')
			{
				black_l2=j<2?false:IsBlackPixel(i,j-2);
				black_l1=j<1?false:IsBlackPixel(i,j-1);
				black_0=IsBlackPixel(i,j);
				black_r1=(j>=m_nMaxTextH-1)?false:IsBlackPixel(i,j+1);
				black_r2=(j>=m_nMaxTextH-2)?false:IsBlackPixel(i,j+2);
			}
			else
			{
				black_l2=i<2?false:IsBlackPixel(i-2,j);
				black_l1=i<1?false:IsBlackPixel(i-1,j);
				black_0=IsBlackPixel(i,j);
				black_r1=(i>=m_nMaxTextH-1)?false:IsBlackPixel(i+1,j);
				black_r2=(i>=m_nMaxTextH-2)?false:IsBlackPixel(i+2,j);
			}
			if(i>=2&&black_l2)
				matchCountArr[0]++;
			if(i>=1&&black_l1)
				matchCountArr[1]++;
			if(black_0)
				matchCountArr[2]++;
			if(i<m_nMaxTextH-1&&black_r1)
				matchCountArr[3]++;
			if(i<m_nMaxTextH-2&&black_r2)
				matchCountArr[4]++;
		}
	}
	double uMaxMatchNum=0;
	int offset_x=0,offset_y=0;
	for(k=0;k<5;k++)
	{
		if(matchCountArr[k]>uMaxMatchNum)
		{
			uMaxMatchNum=matchCountArr[k];
			if(wChar.wHz=='-'||wChar.wHz=='_')
				offset_y=k-2;
			else
				offset_x=k-2;
			if(pciOffsetX)
				*pciOffsetX=offset_x;
			if(pciOffsetY)
				*pciOffsetY=offset_y;
		}
	}
	double increment=0;
	double bigger=max(m_nBlckPixels,pText->m_nBlckPixels);
	double smaller=min(m_nBlckPixels,pText->m_nBlckPixels);
	if(uMaxMatchNum/smaller>0.6&&bigger/smaller>1.1)
	{
		BYTE_ARRAY bufmap(m_nMaxTextH*m_nMaxTextH);
		if(m_nBlckPixels<pText->m_nBlckPixels)
			memcpy(bufmap,pText->GetImageMap(),bufmap.Size());
		else
			memcpy(bufmap,GetImageMap(),bufmap.Size());
		int count1=0;
		for(i=0;i<m_nWidth;i++)
		{
			for(j=0;j<m_nHeight;j++)
			{
				if(!pText->IsBlackPixel(i,j)||!IsBlackPixel(i+offset_x,j))
					continue;
				count1++;
				int deitax=m_nBlckPixels>pText->m_nBlckPixels?offset_x:0;
				int i2=i+deitax,j2=j;
				if(i+deitax<0)
				{	//借位
					j2-=1;
					i2=m_nWidth+i+deitax;
				}
				else if(i+deitax>=m_nWidth)
				{
					j2+=1;
					i2=i+deitax-m_nWidth;
				}
				BYTE* pCursor=&bufmap[j2*m_nWidth+i2];
				*pCursor=0;
				if(i2>0)
					*(pCursor-1)=0;
				if(i2<m_nWidth-1)
					*(pCursor+1)=0;
				if(j2>0)
					*(pCursor-m_nWidth)=0;
				if(j2<m_nHeight-1)
					*(pCursor+m_nWidth)=0;
			}
		}
		for(j=0;j<m_nHeight;j++)
		{
			BYTE* pCursor=&bufmap[j*m_nWidth];
			for(i=0;i<m_nWidth;i++,pCursor++)
			{
				if(((*pCursor)&0x7f)>0)
					increment+=1;
			}
		}
	}
	else
		increment=bigger-uMaxMatchNum;
	matchcoef.original = 0.382*uMaxMatchNum/smaller+0.618*uMaxMatchNum/bigger;
	matchcoef.weighting= 0.382*uMaxMatchNum/smaller+0.618*(1-increment/bigger);
	return matchcoef;
}
void CImageChar::ToBuffer(CBuffer &buffer)
{
	//buffer.WriteInteger(rect.top);
	//buffer.WriteInteger(rect.left);
	//buffer.WriteInteger(rect.bottom);
	//buffer.WriteInteger(rect.right);
	buffer.WriteWord(wChar.wHz);
	//buffer.WriteInteger(m_nMaxTextH);
	int nBitMapSize=0;
	if(m_lpBitMap==NULL)
		nBitMapSize=m_nMaxTextH*m_nMaxTextH;
	if(nBitMapSize>0)
		buffer.Write(m_lpBitMap,nBitMapSize);
}
void CImageChar::FromBuffer(CBuffer &buffer,long version)
{
	//buffer.ReadInteger(&rect.top);
	//buffer.ReadInteger(&rect.left);
	//buffer.ReadInteger(&rect.bottom);
	//buffer.ReadInteger(&rect.right);
	buffer.ReadWord(&wChar.wHz);
	//buffer.ReadInteger(&m_nMaxTextH);
	int nBitMapSize=buffer.ReadInteger();
	if(nBitMapSize>0)
	{
		if(m_lpBitMap)
			delete []m_lpBitMap;
		m_lpBitMap=new BYTE[nBitMapSize];
		buffer.Write(m_lpBitMap,nBitMapSize);
	}
}
//////////////////////////////////////////////////////////////////////////
//CImageCellRegion
CImageCellRegion::CImageCellRegion()
{
	m_lpBitMap=NULL;
	m_nEffWidth=0;
	m_nWidth=0;
	m_nHeight=0;
	m_nDataType=1;
	m_nIterDepth=0;
	m_bBlackPixelIsTrue=true;
}
CImageCellRegion::~CImageCellRegion()
{
	if(m_lpBitMap)
	{
		delete []m_lpBitMap;
		m_lpBitMap=NULL;
	}
}
int IImageNoiseDetect::StatNearBlackPixels(int i,int j,DETECT_MAP* pDetectMap,int maxBlackPixels/*=5*/,BYTE cDetectFlag/*=0x0f*/)
{
	int blackPixels=IsBlackPixel(i,j)?1:0;
	pDetectMap->SetDetect(i,j);
	if(blackPixels==0)
		return 0;
	if( blackPixels<maxBlackPixels&&(cDetectFlag&DETECT_RIGHT)>0 &&
		!pDetectMap->IsDetect(i+1,j))
		blackPixels+=StatNearBlackPixels(i+1,j,pDetectMap,maxBlackPixels-blackPixels);
	if( blackPixels<maxBlackPixels&&((cDetectFlag&DETECT_RIGHT)>0||(cDetectFlag&DETECT_UP)>0) && 
		!pDetectMap->IsDetect(i+1,j+1))
		blackPixels+=StatNearBlackPixels(i+1,j+1,pDetectMap,maxBlackPixels-blackPixels);
	if( blackPixels<maxBlackPixels&&((cDetectFlag&DETECT_RIGHT)>0||(cDetectFlag&DETECT_DOWN)>0)
		&& !pDetectMap->IsDetect(i+1,j-1))
		blackPixels+=StatNearBlackPixels(i+1,j-1,pDetectMap,maxBlackPixels-blackPixels);
	if( blackPixels<maxBlackPixels&&(cDetectFlag&DETECT_UP)>0 &&
		!pDetectMap->IsDetect(i  ,j+1))
		blackPixels+=StatNearBlackPixels(i  ,j+1,pDetectMap,maxBlackPixels-blackPixels);
	if( blackPixels<maxBlackPixels&&(cDetectFlag&DETECT_DOWN)>0 &&
		!pDetectMap->IsDetect(i  ,j-1))
		blackPixels+=StatNearBlackPixels(i  ,j-1,pDetectMap,maxBlackPixels-blackPixels);
	if( blackPixels<maxBlackPixels&&(cDetectFlag&DETECT_LEFT)>0 && 
		!pDetectMap->IsDetect(i-1,j))
		blackPixels+=StatNearBlackPixels(i-1,j,pDetectMap,maxBlackPixels-blackPixels);
	if( blackPixels<maxBlackPixels&&((cDetectFlag&DETECT_LEFT)>0||(cDetectFlag&DETECT_UP)>0) &&
		!pDetectMap->IsDetect(i-1,j+1))
		blackPixels+=StatNearBlackPixels(i-1,j+1,pDetectMap,maxBlackPixels-blackPixels);
	if( blackPixels<maxBlackPixels&&((cDetectFlag&DETECT_LEFT)>0||(cDetectFlag&DETECT_DOWN)>0) && 
		!pDetectMap->IsDetect(i-1,j-1))
		blackPixels+=StatNearBlackPixels(i-1,j-1,pDetectMap,maxBlackPixels-blackPixels);
	return blackPixels;
}
bool IImageNoiseDetect::IsNoisePixel(int i,int j,int minBlackPixels/*=5*/,BYTE cDetectFlag/*=0x0f*/)
{
	DETECT_MAP detect(minBlackPixels,i,j);
	detect.SetDetect(i,j);
	return StatNearBlackPixels(i,j,&detect,minBlackPixels,cDetectFlag)<minBlackPixels;
}
/*bool CImageCellRegion::IsBlackPixel(int i,int j)
{
	WORD iBytePos=i/8;
	WORD iBitPosInByte=i%8;
	BYTE gray=m_lpBitMap[j*m_nEffWidth+iBytePos]&BIT2BYTE[7-iBitPosInByte];
	if(gray!=BIT2BYTE[7-iBitPosInByte])
		return TRUE;	//黑点
	else
		return FALSE;	//白点
}*/
BOOL CImageCellRegion::IsSplitter(WORD movewnd[5])
{
	if(movewnd[2]==0)
		return TRUE;
	else if(movewnd[2]<2&&
		movewnd[1]>=movewnd[2]&&min(movewnd[0],movewnd[1])>=2&&
		movewnd[3]>=movewnd[2]&&min(movewnd[3],movewnd[4])>=2)
		return TRUE;
	else
		return FALSE;
}
CXhChar50 CImageCellRegion::GetUnitDataValue()
{
	CXhChar50 sValue;
	for(CImageChar* pText=listChars.GetFirst();pText;pText=listChars.GetNext())
	{
		if(pText->wChar.wHz>0)
		{
			if(pText->wChar.wHz<127)
				sValue.Append(pText->wChar.chars[0]);
			else	//汉字
			{
				sValue.Append(pText->wChar.chars[0]);
				sValue.Append(pText->wChar.chars[1]);
			}
		}
		else
			sValue.Append('?');
	}
	return sValue;
}
int CImageCellRegion::IsMatchChar(WORD movewnd[5],WORD iBitStart,WORD iSplitStart,WORD iSplitEnd,WORD& wChar)
{
	MATCHCOEF maxMactchCoef;
	int splitter=0;
	CImageChar character,matchchar;
	for(WORD iSplit=iSplitStart;iSplit<=iSplitEnd;iSplit++)
	{
		//TODO:将来可以进行分断线识别优化　wjh-2016.5.25
		//WORD min1=min(movewnd[0],movewnd[1]);
		//WORD min2=min(movewnd[3],movewnd[4]);
		//if(movewnd[2]<=min1 && movewnd[2]<=min2)
		{
			int stdHeight=BelongModel()->GetTemplateH(1);
			character.StandardLocalize(m_lpBitMap,m_nEffWidth,CRect(iBitStart,0,iSplit-2,m_nHeight),stdHeight);
			MATCHCOEF matchCoef=0;
			if(!BelongModel()->RecognizeData(&character,0.6,&matchCoef))
				continue;
			if(character.wChar.wHz>0&&matchCoef.CompareWith(maxMactchCoef)>0)
			{
				wChar=character.wChar.wHz;
				matchchar.CopyImageChar(&character);
				maxMactchCoef=matchCoef;
				splitter=iSplit;
				//if(maxMactchCoef.weighting>0.9)
				//	break;
			}
		}
	}
	if(wChar>0)
	{
		CImageChar* pText=listChars.append();
		pText->CopyImageChar(&matchchar);
		return splitter;
	}
	else
		return 0;
	return wChar>0?splitter:0;
}
#ifdef _DEBUG
void CImageCellRegion::PrintCharMark()
{
	/*int i,from=0;
	for(CImageChar* pText=listChars.GetFirst();pText;pText=listChars.GetNext())
	{
		for(i=from;i<pText->rect.left;i++)
			logerr.LogString(" ",false);
		logerr.LogString("|",false);	//pText->rect.left
		for(i=pText->rect.left+1;i<pText->rect.right;i++)
			logerr.LogString(".",false);
		logerr.LogString("|",false);	//pText->rect.right
		from=pText->rect.right+1;
	}*/
}
#endif
#include "LifeObj.h"
void CImageCellRegion::SplitImageCharSet(WORD iBitStart/*=0*/,WORD wGuessCharWidth/*=0*/,bool bIgnoreRepeatFlag/*=false*/)
{
	WORD wMinCharWidth=3;
	WORD wMaxCharWidth=m_nHeight*8/10;
	WORD movewnd[5]={0},wSplitPos=0;
	if(bIgnoreRepeatFlag)
		m_nIterDepth=0;
	CDepthCounter<int> visitor(&m_nIterDepth);
	if(m_nIterDepth>15)
	{		//可能会出现Q345-8X358之类的规格字符串，故防死循环检测深度设为15
		logerr.Log("SplitImageCharSet fall into death lock loop");
		return;
	}

	if(iBitStart+wMinCharWidth>=m_nWidth)
		return;
	if(m_nDataType==CImageCellRegion::PARTNO)
	{
	}
	else if(m_nDataType==CImageCellRegion::GUIGE)
	{
		CAlphabetFont* pFont=BelongModel()->ActiveFont();
		if(listChars.GetNodeNum()==0)
		{
			for(CImageChar* pTemplChar=pFont->listChars.GetFirst();pTemplChar;pTemplChar=pFont->listChars.GetNext())
			{
				if(pTemplChar->wChar.wHz>='0'&&pTemplChar->wChar.wHz<='9')
					pTemplChar->m_bTemplEnabled=false;
			}
			pFont->pCharX->m_bTemplEnabled=false;
		}
		else
		{
			CImageChar *pChar,*pTemplChar,*pHeadChar=listChars.GetFirst(),*pLastChar=listChars.GetTail();
			pFont->pCharX->m_bTemplEnabled=true;
			int nRecogCharCount=listChars.GetNodeNum();
			WORD wFai=0xb5a6;	//Φ(GB2312:0xb5a6,46502)φ(GB2312:0xd5a6,54694)
			if(pHeadChar&&pHeadChar->wChar.wHz=='Q'&&nRecogCharCount<=4)
			{
				for(pTemplChar=pFont->listChars.GetFirst();pTemplChar;pTemplChar=pFont->listChars.GetNext())
				{
					if(pTemplChar->wChar.wHz>='0'&&pTemplChar->wChar.wHz<='9')
						pTemplChar->m_bTemplEnabled=true;
					if( nRecogCharCount==1&&pTemplChar->wChar.wHz!='2'&&
						pTemplChar->wChar.wHz!='3'&&pTemplChar->wChar.wHz!='4')
						pTemplChar->m_bTemplEnabled=false;	//第一个字母为'Q'，后续字母只能为'2':Q235,'3':Q345,'4':Q420
					if( nRecogCharCount==2&&pTemplChar->wChar.wHz!='3'&&pTemplChar->wChar.wHz!='4'&&
						pTemplChar->wChar.wHz!='2'&&pTemplChar->wChar.wHz!='6')
						pTemplChar->m_bTemplEnabled=false;	//第一个字母为'Q'，后续字母只能为'3':Q345,'2':Q420,'6':Q460
					if( nRecogCharCount==3&&pTemplChar->wChar.wHz!='0'&&pTemplChar->wChar.wHz!='5')
						pTemplChar->m_bTemplEnabled=false;	//第一个字母为'Q'，后续字母只能为'5':Q345,'0':Q420,Q460
					if( nRecogCharCount==4)
					{
						if(pTemplChar->wChar.wHz!='L'&&pTemplChar->wChar.wHz!='-'&&pTemplChar->wChar.wHz!=wFai)
							pTemplChar->m_bTemplEnabled=false;	//第一个字母为'Q'，后续4th字母只能为'L','-','Φ'
						else
							pTemplChar->m_bTemplEnabled=true;
					}
				}
			}
			else if(pLastChar&&pLastChar->wChar.wHz=='L'||
				pLastChar&&pLastChar->wChar.wHz=='-'||pLastChar&&pLastChar->wChar.wHz==wFai)
			{
				for(pTemplChar=pFont->listChars.GetFirst();pTemplChar;pTemplChar=pFont->listChars.GetNext())
				{
					if(pTemplChar->wChar.wHz>='1'&&pTemplChar->wChar.wHz<='9')
						pTemplChar->m_bTemplEnabled=true;
					else
						pTemplChar->m_bTemplEnabled=false;
				}
			}
			else
			{
				for(pChar=listChars.GetFirst();pChar;pChar=listChars.GetNext())
				{
					if(pChar->wChar.wHz=='Q')
						pFont->pCharQ->m_bTemplEnabled=false;
					else if(pChar->wChar.wHz=='L')
						pFont->pCharQ->m_bTemplEnabled=pFont->pCharL->m_bTemplEnabled=false;
					else if(pChar->wChar.wHz=='X')
						pFont->pCharQ->m_bTemplEnabled=pFont->pCharL->m_bTemplEnabled=pFont->pCharX->m_bTemplEnabled=false;
				}
				for(pTemplChar=pFont->listChars.GetFirst();pTemplChar;pTemplChar=pFont->listChars.GetNext())
				{
					if(pTemplChar->wChar.wHz>='0'&&pTemplChar->wChar.wHz<='9')
						pTemplChar->m_bTemplEnabled=true;
				}
			}
		}
	}
	if(iBitStart+wMinCharWidth<m_nWidth&&iBitStart+wMaxCharWidth>=m_nWidth)
	{	//剩余为一个字符
		CImageChar* pText=listChars.append();
		int stdHeight=BelongModel()->GetTemplateH(1);
		pText->StandardLocalize(m_lpBitMap,m_nEffWidth,CRect(iBitStart,0,m_nWidth,m_nHeight),stdHeight);
		BelongModel()->RecognizeData(pText,0.6);
		return;
	}
	int nSplitMode=0;	//0.空白像素列分割  1.字符匹配方式分割
	while(wSplitPos==0)
	{
		XHWCHAR wChar;
		for(WORD i=wMinCharWidth;i<=wMaxCharWidth;i++)
		{
			WORD xiPixel=i+iBitStart;
			int wColContinueBlackHits=0,wColBlackHits=0;
			bool prevblack=false;
			for(WORD yjPixel=0;yjPixel<m_nHeight;yjPixel++)
			{
				if(IsBlackPixel(xiPixel,yjPixel))
				{
					if(prevblack)
						wColContinueBlackHits++;
					else
					{
						prevblack=true;
						wColContinueBlackHits=1;
					}
				}
				else
				{
					wColBlackHits=max(wColBlackHits,wColContinueBlackHits);
					prevblack=false;
				}
			}
			wColBlackHits=max(wColBlackHits,wColContinueBlackHits);
			if(i<wMinCharWidth+4)
			{
				movewnd[i-wMinCharWidth]=wColBlackHits;
				if(i-wMinCharWidth<2 && wColBlackHits==0)
				{
					wSplitPos=xiPixel;
					break;
				}
			}
			else
			{	//移位一个像素点
				if(i>wMinCharWidth+4)
					memmove(movewnd,&movewnd[1],8);
				movewnd[4]=wColBlackHits;
				if((nSplitMode==0 && IsSplitter(movewnd)) ||
					(nSplitMode==1 && (xiPixel=IsMatchChar(movewnd,iBitStart,xiPixel,iBitStart+wMaxCharWidth,wChar.wHz))))
				{
					wSplitPos=xiPixel-2;
					break;
				}
			}
		}
		if(wChar.wHz>0)
			break;
		else if(wSplitPos!=0)
		{
			CImageChar* pText=listChars.append();
			int stdHeight=BelongModel()->GetTemplateH(1);
			pText->StandardLocalize(m_lpBitMap,m_nEffWidth,CRect(iBitStart,0,wSplitPos,m_nHeight),stdHeight);
			BelongModel()->RecognizeData(pText,0.6);
			break;
		}
		if(nSplitMode==0)
			nSplitMode=1;
		else if(nSplitMode==1)
		{
			wSplitPos=iBitStart+wMaxCharWidth;	//分割失败，像素点向后移动
			nSplitMode=0;
			break;
		}

	}
	//计算下一个字符开始像素
	for(WORD i=wSplitPos;i<=m_nWidth;i++)
	{
		BOOL bBalck=FALSE;
		for(WORD j=0;j<m_nHeight;j++)
		{
			if(IsBlackPixel(i,j))
			{
				bBalck=TRUE;
				break;
			}
		}
		if(bBalck || i==m_nWidth-1)
		{
			iBitStart=i;
			break;
		}
	}
	return SplitImageCharSet(iBitStart,wGuessCharWidth);
}
//////////////////////////////////////////////////////////////////////////
//CImageDataRegion
static BOMPART* CreateBomPart(int idClsType,DWORD key,void* pContext)
{
	BOMPART *pPart=NULL;
	switch (idClsType){
	case BOMPART::PLATE:
		pPart=new PART_PLATE();
		break;
	case BOMPART::ANGLE:
		pPart=new PART_ANGLE();
		break;
	case BOMPART::TUBE:
		pPart=new PART_TUBE();
		break;
	case BOMPART::SLOT:
		pPart=new PART_SLOT();
		break;
	default:
		pPart=new BOMPART();
		break;
	}
	return pPart;
}
static BOOL DeleteBomPart(BOMPART *pPart)
{
	if(pPart==NULL)
		return FALSE;
	switch (pPart->cPartType){
	case BOMPART::PLATE:
		delete (PART_PLATE*)pPart;
		break;
	case BOMPART::ANGLE:
		delete (PART_ANGLE*)pPart;
		break;
	case BOMPART::TUBE:
		delete (PART_TUBE*)pPart;
		break;
	case BOMPART::SLOT:
		delete (PART_SLOT*)pPart;
		break;
	default:
		delete pPart;
		break;
	}
	return TRUE;
}

CImageDataRegion::CImageDataRegion()
{
	m_lpBits=NULL;
	m_nEffWidth=0;
	m_nDataWidth=0;
	m_nDataHeight=0;
	m_hashPartByIndex.CreateNewAtom=CreateBomPart;
	m_hashPartByIndex.DeleteAtom=DeleteBomPart;
	m_dwKey=NULL;
	m_lpBits=NULL;
	m_fDisplayAllRatio=1;
	m_fDisplayRatio=1;
	m_nOffsetX=0;
	m_nOffsetY=0;
	m_lpBits=NULL;
}
CImageDataRegion::~CImageDataRegion()
{
	if(m_lpBits)
		delete []m_lpBits;
}
/*将界面上捕捉的点转换到图片上的点：
	界面上捕捉的点与图片上的点是上下倒置的
	界面上捕捉的：
		leftUp		对应图片左下
		leftDown	对应图片左上
		rightUp		对应图片的右下
		rightDown	对应图片的右上*/
void CImageDataRegion::TransPickPtToOrigPt(int sumHeight)
{
	int x=0,y=0;
	//左侧点转换
	x=(int)leftUp.x;
	leftUp.x=leftDown.x;
	leftDown.x=x;
	leftUp.y=sumHeight-leftUp.y;
	leftDown.y=sumHeight-leftDown.y;
	y=(int)leftUp.y;
	leftUp.y=leftDown.y;
	leftDown.y=y;
	//右侧点转换
	x=(int)rightUp.x;
	rightUp.x=rightDown.x;
	rightDown.x=x;
	rightUp.y=sumHeight-rightUp.y;
	rightDown.y=sumHeight-rightDown.y;
	y=(int)rightUp.y;
	rightUp.y=rightDown.y;
	rightDown.y=y;
}
bool CImageDataRegion::IsBlackPixel(int i,int j,BYTE* lpBits/*=NULL*/,WORD wBitWidth/*=0*/)
{
	WORD iBytePos=i/8;
	WORD iBitPosInByte=i%8;
	BYTE gray=0;
	if(lpBits)
		gray=lpBits[j*wBitWidth+iBytePos]&BIT2BYTE[7-iBitPosInByte];
	else
		gray=m_lpBits[j*m_nEffWidth+iBytePos]&BIT2BYTE[7-iBitPosInByte];
	if(gray!=BIT2BYTE[7-iBitPosInByte])
		return true;	//黑点
	else
		return false;	//白点
}
int CImageDataRegion::CalMaxPixelsIndex(int start,int end,BOOL bLevel)
{
	int nPixelsSum=0;
	if(bLevel)
		nPixelsSum=m_nDataWidth;
	else
		nPixelsSum=m_nDataHeight;
	ATOM_LIST<PIXELLINE> pixellList; 
	for(int i=start;i<=end;i++)
	{
		PIXELLINE* pLixell=pixellList.append();
		pLixell->iStart=i;
		for(int j=0;j<nPixelsSum;j++)
		{
			if(!bLevel && IsBlackPixel(i,j))
				pLixell->nPixell++;
			else if(bLevel && IsBlackPixel(j,i))
				pLixell->nPixell++;	
		}
	}
	int nMaxPixell=0;
	for(PIXELLINE* pLixell=pixellList.GetFirst();pLixell;pLixell=pixellList.GetNext())
	{
		if(pLixell->nPixell>nMaxPixell)
		{
			nMaxPixell=pLixell->nPixell;
			start=pLixell->iStart;
		}
	}
	return start;
}
//
BOOL CImageDataRegion::AnalyzeLinePixels(int& start,int& end,BOOL bLevel/*=TRUE*/)
{
	int nBlackNum=0,nBlackSum=0;
	if(bLevel)
		nBlackSum=m_nDataWidth;
	else
		nBlackSum=m_nDataHeight;
	for(int i=0;i<nBlackSum;i++)
	{
		for(int j=start;j<=end;j++)
		{
			if(bLevel && IsBlackPixel(i,j))
			{
				nBlackNum++;
				break;
			}
			else if(!bLevel && IsBlackPixel(j,i))
			{
				nBlackNum++;
				break;
			}
		}
	}
	double fMatchCoef=(double)nBlackNum/nBlackSum;
	if(fMatchCoef<0.95)
		return FALSE;
	start=CalMaxPixelsIndex(start,end,bLevel);	//计算最优起始点
	return TRUE;
}
//识别图片的中的行列
void CImageDataRegion::RecognizeLines(ATOM_LIST<int> &lineStartList,BOOL bLevel)
{
	int offset=5,start=0,end=0;
	int nLen=0;
	if(bLevel)	//识别行数
		nLen=m_nDataHeight;
	else		//识别列数
		nLen=m_nDataWidth;
	for(int i=0;i<nLen-1;)
	{	
		start=i;
		end=i+offset;
		if(end>=nLen)
			end=nLen-1;
		if(AnalyzeLinePixels(start,end,bLevel))
			lineStartList.append(start);
		i=end;
	}
	if(lineStartList.GetNodeNum()<=0)
		return;
	//对识别出的行列进行检查
	int nNum=0;
	if(lineStartList[0]>10)				//首行/列检查			
		lineStartList.insert(0,-1);
	nNum=lineStartList.GetNodeNum();
	if(nLen-lineStartList[nNum-1]>10)	//末行/列检查
		lineStartList.append(nLen-2);
	for(int i=0;i<lineStartList.GetNodeNum()-1;i++)			//中间行/列检查
	{
		int iCurStart=lineStartList[i];
		int iNextStart=lineStartList[i+1];
		if(iNextStart-iCurStart<10)
		{
			iNextStart=CalMaxPixelsIndex(iCurStart,iNextStart,bLevel);
			lineStartList[i+1]=iNextStart;
			lineStartList.DeleteCursor();
		}
	}
	lineStartList.Clean();
}

//计算文本的最小有效区域,xEnd,yEnd分别为右侧及底部含黑点的边界
void CImageDataRegion::CalCharValidRect(RECT data_rect,int& xStart,int& yStart,int& xEnd,int& yEnd)
{
	//第一次计算文本在矩形框中的空白起始位置及终止位置
	int left=0,right=0,top=0,btm=0;
	for(int i=data_rect.left;i<data_rect.right;i++)
	{
		int nBlack=0;
		for(int j=data_rect.top;j<data_rect.bottom;j++)
		{
			if(IsBlackPixel(i,j))
				nBlack++;
			if(nBlack>7)
				break;
		}
		if(nBlack<=7)
		{
			left=i;
			break;
		}
	}
	for(int i=data_rect.right;i>data_rect.left;i--)
	{
		int nBlack=0;
		for(int j=data_rect.top;j<data_rect.bottom;j++)
		{
			if(IsBlackPixel(i,j))
				nBlack++;
			if(nBlack>7)
				break;
		}
		if(nBlack<=7)
		{
			right=i;
			break;
		}
	}
	//竖直起始位置
	for(int j=data_rect.top;j<data_rect.bottom;j++)
	{
		int nBlack=0;
		for(int i=data_rect.left;i<data_rect.right;i++)
		{
			if(IsBlackPixel(i,j))
				nBlack++;
			if(nBlack>7)
				break;
		}
		if(nBlack<=7)
		{
			top=j;
			break;
		}
	}
	//竖直终止位置
	for(int j=data_rect.bottom;j>data_rect.top;j--)
	{
		int nBlack=0;
		for(int i=data_rect.left;i<data_rect.right;i++)
		{
			if(IsBlackPixel(i,j))
				nBlack++;
			if(nBlack>7)
				break;
		}
		if(nBlack<=7)
		{
			btm=j;
			break;
		}
	}
	//第二次计算文本的最小矩形区域
	for(int i=left;i<=right;i++)
	{
		int nBlack=0,nNextBlack=0,yiBlackPixel=0;
		for(int j=top;j<=btm;j++)
		{
			if(IsBlackPixel(i,j))
			{
				yiBlackPixel=j;
				nBlack++;
			}
			if(nBlack>=1)
				break;
		}
		for(int j=top;j<=btm;j++)
		{
			if(IsBlackPixel(i+1,j))
				nNextBlack++;
			if(nNextBlack>1)
				break;
		}
		if(nBlack>=1 && nNextBlack>1)
		//if(nBlack>1&&!IsNoisePixel(i,yiBlackPixel,5,DETECT_RIGHT))
		{   
			xStart=i;
			break;
		}
	}
	for(int i=right;i>=left;i--)
	{
		int nBlack=0,nPreBlack=0;
		for(int j=top;j<=btm;j++)
		{
			if(IsBlackPixel(i,j))
				nBlack++;
			if(nBlack>=1)
				break;
		}
		for(int j=top;j<=btm;j++)
		{
			if(IsBlackPixel(i-1,j))
				nPreBlack++;
			if(nPreBlack>1)
				break;
		}
		if(nBlack>=1 && nPreBlack>1)
		//if(nBlack>1&&!IsNoisePixel(i,j,5,DETECT_RIGHT))
		{   
			xEnd=i;
			break;
		}
	}
	for(int j=top;j<=btm;j++)
	{
		int nBlack=0,nNextBlack=0;
		for(int i=left;i<=right;i++)
		{	
			if(IsBlackPixel(i,j))
				nBlack++;
			if(nBlack>1)
				break;
		}
		for(int i=left;i<=right;i++)
		{	
			if(IsBlackPixel(i,j+1))
				nNextBlack++;
			if(nNextBlack>1)
				break;
		}
		if(nBlack>1 && nNextBlack>1)
		{
			yStart=j;
			break;
		}
	}
	for(int j=btm;j>=top;j--)
	{
		int nBlack=0,nPreBlack=0;
		for(int i=left;i<=right;i++)
		{	
			if(IsBlackPixel(i,j))
				nBlack++;
			if(nBlack>1)
				break;
		}
		for(int i=left;i<=right;i++)
		{	
			if(IsBlackPixel(i,j-1))
				nPreBlack++;
			if(nPreBlack>1)
				break;
		}
		if(nBlack>1 && nPreBlack>1)
		{
			yEnd=j;
			break;
		}
	}
}
//初始化单元文本区域信息
void CImageDataRegion::RetrieveCellRgn(CImageCellRegion& cell,RECT rect)
{
	cell.InitBitImage(NULL,rect.right-rect.left,rect.bottom-rect.top);
	//unitRgn.m_nWidth=rect.right-rect.left;
	//unitRgn.m_nHeight=rect.bottom-rect.top;
	//unitRgn.m_nWidth%16>0?unitRgn.m_nEffWidth=(unitRgn.m_nWidth/16)*2+2:unitRgn.m_nEffWidth=unitRgn.m_nWidth/8;
	//unitRgn.m_lpBitMap=new BYTE[unitRgn.m_nEffWidth*unitRgn.m_nHeight];
	//memset(unitRgn.m_lpBits,0,unitRgn.m_nEffWidth*unitRgn.m_nHeight);
	for(int i=rect.left;i<rect.right;i++)
	{
		for(int j=rect.top;j<rect.bottom;j++)
		{
			if(IsBlackPixel(i,j))	//默认为白点，只有黑点时需要设定
				cell.SetPixelState(i-rect.left,j-rect.top,true);
			//if(!IsBlackPixel(i,j))
			//{
			//	int x=i-rect.left;
			//	int y=j-rect.top;
			//	int iByte=x/8,iBit=x%8;
			//	unitRgn.m_lpBits[y*unitRgn.m_nEffWidth+iByte]|=BIT2BYTE[7-iBit];
			//}
		}
	}
}
//识别数据内容
int iExportLogFileCell=2;
int iRecognizeCell=-1;
CXhChar50 CImageDataRegion::RecognizeDatas(RECT data_rect,int data_type)
{
	CXhChar50 sValue;
	//计算文本的最小有效区域
	int xStart=0,xEnd=0;
	int yStart=0,yEnd=0;
	iRecognizeCell++;
#ifdef _DEBUG
	{
	int i,j;
	CLogErrorLife life;
	logerr.Log("-----------原始单元格区域-------------\n");
	for(j=data_rect.top;j<=data_rect.bottom;j++)
	{
		for(i=data_rect.left;i<=data_rect.right;i++)
		{
			if(IsBlackPixel(i,j))
				logerr.LogString(".",false);
			else
				logerr.LogString(" ",false);
		}
		logerr.Log("\n");
	}
#endif
	CalCharValidRect(data_rect,xStart,yStart,xEnd,yEnd);
#ifdef _DEBUG
	logerr.Log("-----------有效区域-------------\n");
	for(j=data_rect.top;j<data_rect.bottom;j++)
	{
		for(i=data_rect.left;i<data_rect.right;i++)
		{
			if(j<yStart||j>yEnd||i<xStart||i>xEnd)
				logerr.LogString("B",false);
			else if(IsBlackPixel(i,j))
				logerr.LogString(".",false);
			else
				logerr.LogString(" ",false);
		}
		logerr.Log("\n");
	}
	for(i=data_rect.left;i<xStart;i++)
		logerr.LogString(" ",false);
#endif
	if(xEnd<=xStart || yEnd<=yStart)
	{
#ifdef _DEBUG
	if(iExportLogFileCell!=iRecognizeCell)
		logerr.ClearContents();
#endif
		return sValue;
	}
	data_rect.left=xStart;
	data_rect.right=xEnd+1;
	data_rect.top=yStart;
	data_rect.bottom=yEnd+1;
	//初始化文本单元,拆分识别字符
	CImageCellRegion cell;
	CAlphabet* pAlphabet=this->BelongImageData()->BelongModel();
	CAlphabetFont *pFont=pAlphabet->EnumFirstFont();
	for(CImageChar* pTemplChar=pFont->listChars.GetFirst();pTemplChar;pTemplChar=pFont->listChars.GetNext())
	{
		pTemplChar->m_bTemplEnabled=true;
		if(  data_type==CImageCellRegion::PARTNO&&
			(pTemplChar->wChar.wHz=='Q'||pTemplChar->wChar.wHz=='X'||pTemplChar->wChar.wHz=='L'))
			pTemplChar->m_bTemplEnabled=false;
		else if((data_type==CImageCellRegion::LENGHT||data_type==CImageCellRegion::NUM)&&
			(pTemplChar->wChar.wHz=='Q'||pTemplChar->wChar.wHz=='L'||pTemplChar->wChar.wHz=='-'||
			 pTemplChar->wChar.wHz=='X'))
			pTemplChar->m_bTemplEnabled=false;
		else if(data_type==CImageCellRegion::NUM&&pTemplChar->wChar.wHz=='.')
			pTemplChar->m_bTemplEnabled=false;
	}
	if((data_type==CImageCellRegion::PIECE_W||data_type==CImageCellRegion::SUM_W)&&pFont->pCharPoint)
	{
		pFont->pCharPoint->m_bTemplEnabled=true;
		if(pFont->pCharL)
			pFont->pCharL->m_bTemplEnabled=false;
		if(pFont->pCharQ)
			pFont->pCharQ->m_bTemplEnabled=false;
		if(pFont->pCharX)
			pFont->pCharX->m_bTemplEnabled=false;
	}
	else
		pFont->pCharPoint->m_bTemplEnabled=false;
	//static const int NUM		= 4;
	//static const int PIECE_W	= 5;
	//static const int SUM_W		= 6;
	//static const int NOTE		= 7;
	RetrieveCellRgn(cell,data_rect);
	cell.SetBelongModel(BelongImageData()->BelongModel());
	cell.m_nDataType=data_type;
	cell.SplitImageCharSet(0,0,true);		//拆分识别字符
	if(cell.GetUnitDataValue().Length>0)
		sValue=cell.GetUnitDataValue();
#ifdef _DEBUG
	//cell.PrintCharMark();
	if(iExportLogFileCell!=iRecognizeCell)
		logerr.ClearContents();
	}
#endif
	return sValue;
}
void CImageDataRegion::ParseDataValue(CXhChar50 sText,int data_type,char* sValue1
									,char* sValue2/*=NULL*/,int cls_id/*=0*/)
{
	if(data_type==CImageCellRegion::GUIGE)
	{
		if(strstr(sText,"L"))
			cls_id=BOMPART::ANGLE;
		else if(strstr(sText,"-"))
			cls_id=BOMPART::PLATE;
		else if(strstr(sText,"φ"))
			cls_id=BOMPART::TUBE;
		else
			cls_id=0;
		CXhChar16 sMat;
		if(strstr(sText,"Q"))
		{
			sText.Replace("L"," ");
			sText.Replace("-"," ");
			sText.Replace("φ"," ");
			sscanf(sText,"%s%s",(char*)sMat,sValue1);
		}
		else
		{
			strcpy(sMat,"Q235");
			strcpy(sValue1,sText);
		}	
		if(sValue2)
			strcpy(sValue2,sMat);
	}
	else
		strcpy(sValue1,sText);
}
static void RestoreSpec(const char* spec,int *width,int *thick,char *matStr=NULL)
{
	char sMat[16]="",cMark1=' ',cMark2=' ';
	if(strstr(spec,"Q")==(char*)spec)
	{
		if(strstr(spec,"L"))
			sscanf(spec,"%[^L]%c%d%c%d",sMat,&cMark1,width,&cMark2,thick);
		else if(strstr(spec,"-"))
			sscanf(spec,"%[^-]%c%d",sMat,&cMark1,thick);
	}
	else if(strstr(spec,"L"))
	{
		CXhChar16 sSpec(spec);
		sSpec.Replace("L","");
		sSpec.Replace("*"," ");
		sSpec.Replace("X"," ");
		sSpec.Replace("x"," ");
		sscanf(sSpec,"%d%d",width,thick);
	}
	else if (strstr(spec,"-"))
		sscanf(spec,"%c%d",sMat,thick);
	//else if(spec,"φ")
	//sscanf(spec,"%c%d%c%d",sMat,)
	if(matStr)
		strcpy(matStr,sMat);
}
//提取数据
BOOL CImageDataRegion::RetrieveImageData()
{
	//1、提取数据区域的行数和列数
	//行
	ATOM_LIST<int>	levelStartList;
	RecognizeLines(levelStartList,TRUE);
	if(levelStartList.GetNodeNum()<=0)
	{
		logerr.Log("分行失败");
		return FALSE;
	}
	//列
	ATOM_LIST<int> verticalStartList;
	RecognizeLines(verticalStartList,FALSE);
	if(verticalStartList.GetNodeNum()<=0)
	{
		logerr.Log("分列失败");
		return FALSE;
	}
#ifdef _DEBUG
	iRecognizeCell=-1;
#endif
	//2、将数据区域分成若干个矩形区域
	DYN_ARRAY<RECT> rowCells(verticalStartList.GetNodeNum()-1);
	for(int i=0;i<levelStartList.GetNodeNum()-1;i++)
	{
		//rowCells.Empty();
		//if(i!=3)
		//	continue;
		RECT rect;
		rect.top=levelStartList[i];
		rect.bottom=levelStartList[i+1];
		for(int j=0;j<verticalStartList.GetNodeNum()-1;j++)
		{
			rect.left=verticalStartList[j];
			rect.right=verticalStartList[j+1];
			rowCells[j]=rect;
		}
		//3、识别每个区域的内容
		int cls_id=0;
		CXhChar50 sText;
		CXhChar16 sPartNo,sMaterial,sSpec,sLen,sNum,sWeight,sSumW,sNote;
		for(WORD k=0;k<rowCells.Size();k++)
		{
			rect=rowCells[k];
			sText=RecognizeDatas(rect,k+1);
			if(k==0)		//件号
				ParseDataValue(sText,CImageCellRegion::PARTNO,sPartNo);
			else if(k==1)	//材质|规格
			{
				ParseDataValue(sText,CImageCellRegion::GUIGE,sSpec,sMaterial);
				sSpec=sText;
			}
			else if(k==2)	//长度
				sLen=sText;//ParseDataValue(sText,CImageCellRegion::LENGHT,sLen);
			else if(k==3)	//数量
				ParseDataValue(sText,CImageCellRegion::NUM,sNum);
			else if(k==4)	//单重
				ParseDataValue(sText,CImageCellRegion::PIECE_W,sWeight);
			else if(k==5)	//总重
				ParseDataValue(sText,CImageCellRegion::SUM_W,sSumW);
			else if(k==6)	//备注
				ParseDataValue(sText,CImageCellRegion::NOTE,sNote);	
		}
		//初始化哈希表
		BOMPART* pBomPart=m_hashPartByIndex.Add(0,cls_id);
		pBomPart->AddPart(atoi(sNum));
		pBomPart->sPartNo.Copy(sPartNo);
		pBomPart->sSpec.Copy(sSpec);
		pBomPart->cMaterial=QuerySteelBriefMatMark(sMaterial);
		pBomPart->length=atoi(sLen);
		pBomPart->fPieceWeight=atof(sWeight);
		int nWidth=0,nThick=0;
		RestoreSpec(sSpec,&nWidth,&nThick);
		pBomPart->wide=(float)nWidth;
		pBomPart->thick=(float)nThick;
		pBomPart->feature1=rect.top;
		pBomPart->feature2=rect.bottom;
	}
	return TRUE;
}

void CImageDataRegion::ToBuffer(CBuffer &buffer)
{
	buffer.WriteInteger(leftUp.x);
	buffer.WriteInteger(leftUp.y);
	buffer.WriteInteger(leftDown.x);
	buffer.WriteInteger(leftDown.y);
	buffer.WriteInteger(rightUp.x);
	buffer.WriteInteger(rightUp.y);
	buffer.WriteInteger(rightDown.x);
	buffer.WriteInteger(rightDown.y);
	//图片
	BUFFERPOP stack(&buffer,m_hashPartByIndex.GetNodeNum());
	buffer.WriteInteger(m_hashPartByIndex.GetNodeNum());
	for(BOMPART *pPart=m_hashPartByIndex.GetFirst();pPart;pPart=m_hashPartByIndex.GetNext())
	{
		buffer.WriteInteger(m_hashPartByIndex.GetCursorKey());
		buffer.WriteByte(pPart->cPartType);
		//保存构件信息
		buffer.WriteInteger(pPart->GetPartNum());
		buffer.WriteString(pPart->sPartNo);
		buffer.WriteInteger(pPart->iSeg);
		buffer.WriteByte(pPart->cMaterial);
		buffer.WriteDouble(pPart->wide);
		buffer.WriteDouble(pPart->thick);
		buffer.WriteDouble(pPart->wingWideY);
		buffer.WriteString(pPart->sSpec);
		buffer.WriteDouble(pPart->length);
		buffer.WriteDouble(pPart->fPieceWeight);
		buffer.WriteString(pPart->sNotes);
		buffer.WriteInteger(pPart->feature1);	//构件对应图片行顶部坐标
		buffer.WriteInteger(pPart->feature2);	//构件对应图片行底部坐标
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		logerr.Log("The number record of image part is wrong!");
#else 
		logerr.Log("图片识别构件数量错误!");
#endif
}
void CImageDataRegion::FromBuffer(CBuffer &buffer,long version)
{
	buffer.ReadInteger(&leftUp.x);
	buffer.ReadInteger(&leftUp.y);
	buffer.ReadInteger(&leftDown.x);
	buffer.ReadInteger(&leftDown.y);
	buffer.ReadInteger(&rightUp.x);
	buffer.ReadInteger(&rightUp.y);
	buffer.ReadInteger(&rightDown.x);
	buffer.ReadInteger(&rightDown.y);
	//图片
	m_hashPartByIndex.Empty();
	int n=buffer.ReadInteger();
	for(int i=0;i<n;i++)
	{
		DWORD dwKey=buffer.ReadInteger();
		BYTE cPartType=0;
		buffer.ReadByte(&cPartType);
		BOMPART *pPart=m_hashPartByIndex.Add(dwKey,cPartType);
		pPart->AddPart(buffer.ReadInteger());
		buffer.ReadString(pPart->sPartNo);
		buffer.ReadInteger(&pPart->iSeg);
		buffer.ReadByte(&pPart->cMaterial);
		buffer.ReadDouble(&pPart->wide);
		buffer.ReadDouble(&pPart->thick);
		buffer.ReadDouble(&pPart->wingWideY);
		buffer.ReadString(pPart->sSpec);
		buffer.ReadDouble(&pPart->length);
		buffer.ReadDouble(&pPart->fPieceWeight);
		buffer.ReadString(pPart->sNotes);
		buffer.ReadInteger(&pPart->feature1);	//构件对应图片行顶部坐标
		buffer.ReadInteger(&pPart->feature2);	//构件对应图片行底部坐标
	}
}
//////////////////////////////////////////////////////////////////////////
//CImageDataFile
CImageDataFile::CImageDataFile()
{
}
CImageDataFile::~CImageDataFile()
{
	
}
void CImageDataFile::InitImageShowPara(RECT showRect,DWORD dwRegionKey/*=0*/)
{
	CImageDataRegion *pRegion=m_hashRegionById.GetValue(dwRegionKey);
	int image_width=image_trans.GetWidth(),image_height=image_trans.GetHeight();
	if(pRegion!=NULL)
	{
		image_width=pRegion->m_nDataWidth;
		image_height=pRegion->m_nDataHeight;
	}
	double scale_h=(double)(showRect.right-showRect.left)/image_width;
	double scale_v=(double)(showRect.bottom-showRect.top)/image_height;
	if(pRegion)
	{
		pRegion->m_nOffsetX=pRegion->m_nOffsetY=0;
		pRegion->m_fDisplayAllRatio=min(1,scale_h>scale_v?scale_v:scale_h);
		//pRegion->m_fDisplayRatio=pRegion->m_fDisplayAllRatio;	//默认全显
		pRegion->m_fDisplayRatio=min(1,scale_h>scale_v?scale_h:scale_v);
	}
	else
	{
		m_nOffsetX=m_nOffsetY=0;
		m_fDisplayAllRatio=min(1,scale_h>scale_v?scale_v:scale_h);
		m_fDisplayRatio=min(1,scale_h>scale_v?scale_h:scale_v);
	}
}
//
bool CImageDataFile::ReadImageData(const char* file_name)
{
	strcpy(file_path,file_name);
	return image_trans.ReadImageFile(file_path);
}
//
void CImageDataFile::InitDataBits(CImageDataRegion *pRegion)
{
	if(  pRegion==NULL||
		(pRegion->leftUp.x==0&&pRegion->leftUp.y==0)||
		(pRegion->leftDown.x==0&&pRegion->leftDown.y==0)||
		(pRegion->rightUp.x==0&&pRegion->rightUp.y==0)||
		(pRegion->rightDown.x==0&&pRegion->rightDown.y==0))
	{
		logerr.Log("没有选中数据区域");
		return;
	}
	image_trans.LocateDrawingRgnRect(pRegion->leftUp,pRegion->leftDown,pRegion->rightUp,pRegion->rightDown);
	pRegion->m_nDataWidth=image_trans.GetDrawingWidth()+1;
	pRegion->m_nDataHeight=image_trans.GetDrawingHeight()+1;
	pRegion->m_nDataWidth%16>0?pRegion->m_nEffWidth=(pRegion->m_nDataWidth/16)*2+2:pRegion->m_nEffWidth=pRegion->m_nDataWidth/8;
	if(pRegion->m_lpBits)
		delete []pRegion->m_lpBits;
	pRegion->m_lpBits=new BYTE[pRegion->m_nEffWidth*pRegion->m_nDataHeight];
	memset(pRegion->m_lpBits,0,pRegion->m_nEffWidth*pRegion->m_nDataHeight);
	POINT p;
	for(int i=0;i<pRegion->m_nDataWidth;i++)
	{
		for(int j=0;j<pRegion->m_nDataHeight;j++)
		{
			p.x=i;
			p.y=pRegion->m_nDataHeight-1-j;
			p=image_trans.TransformPointToOriginalField(p);
			BOOL bBlack=image_trans.IsBlackPixel(p.x,p.y);
			if(!bBlack)
			{
				int iByte=i/8,iBit=i%8;
				pRegion->m_lpBits[j*pRegion->m_nEffWidth+iByte]|=BIT2BYTE[7-iBit];
			}
		}
	}
}
int CImageDataFile::StretchBlt(HDC hDC,DWORD rop,CImageDataRegion *pRegion/*=NULL*/)
{
	if(pRegion&&pRegion->m_lpBits)
	{
		CRect imageShowRect;
		imageShowRect.SetRect(0,0,f2i(pRegion->m_nDataWidth*pRegion->m_fDisplayRatio),f2i(pRegion->m_nDataHeight*pRegion->m_fDisplayRatio));
		imageShowRect.OffsetRect(pRegion->m_nOffsetX,pRegion->m_nOffsetY);
		CDC *pDC=CDC::FromHandle(hDC);
		CBitmap bmp;
		bmp.CreateBitmap(pRegion->m_nDataWidth,pRegion->m_nDataHeight,1,1,pRegion->m_lpBits);
		CDC dcMemory;
		dcMemory.CreateCompatibleDC(pDC);
		dcMemory.SelectObject(&bmp);
		BOOL rslt=pDC->StretchBlt(imageShowRect.left,imageShowRect.top,imageShowRect.Width(),imageShowRect.Height(),
			&dcMemory,0,0,pRegion->m_nDataWidth,pRegion->m_nDataHeight,rop);
		dcMemory.DeleteDC();
		return rslt;
	}
	else
	{
		CRect imageShowRect;
		imageShowRect.SetRect(0,0,f2i(image_trans.GetWidth()*m_fDisplayRatio),f2i(image_trans.GetHeight()*m_fDisplayRatio));
		imageShowRect.OffsetRect(m_nOffsetX,m_nOffsetY);
		return image_trans.StretchBlt(hDC,imageShowRect,rop);
	}
}

CSize CImageDataFile::GetOffsetSize(DWORD dwRegionKey/*=0*/)
{
	CImageDataRegion *pRegion=m_hashRegionById.GetValue(dwRegionKey);
	return pRegion!=NULL?CSize(pRegion->m_nOffsetX,pRegion->m_nOffsetY):CSize(m_nOffsetX,m_nOffsetY);
}
void CImageDataFile::SetOffsetSize(CSize offset,DWORD dwRegionKey/*=0*/)
{
	CImageDataRegion *pRegion=m_hashRegionById.GetValue(dwRegionKey);
	if(pRegion)
	{
		pRegion->m_nOffsetX=offset.cx;
		pRegion->m_nOffsetY=offset.cy;
	}
	else
	{
		m_nOffsetX=offset.cx;
		m_nOffsetY=offset.cy;
	}
}
double CImageDataFile::GetDisplayRatio(DWORD dwRegionKey/*=0*/)
{
	CImageDataRegion *pRegion=m_hashRegionById.GetValue(dwRegionKey);
	return pRegion!=NULL?pRegion->m_fDisplayRatio:m_fDisplayRatio;
}
BOOL CImageDataFile::IsCanZoom(DWORD dwRegionKey/*=0*/)
{
	CImageDataRegion *pRegion=m_hashRegionById.GetValue(dwRegionKey);
	if(pRegion)
		return pRegion->m_fDisplayRatio<=min(1,pRegion->m_fDisplayAllRatio);
	else
		return m_fDisplayRatio<=min(1,m_fDisplayAllRatio);
}
double CImageDataFile::Zoom(short zDelta,DWORD dwRegionKey/*=0*/)
{
	CImageDataRegion *pRegion=m_hashRegionById.GetValue(dwRegionKey);
	if(pRegion)
	{
		double fMinDisplayRatio=min(1,pRegion->m_fDisplayAllRatio);
		if(zDelta==0)		//全显
			pRegion->m_fDisplayRatio=pRegion->m_fDisplayAllRatio;
		else if(zDelta>0)	//放大
			pRegion->m_fDisplayRatio*=1.6;
		else if(pRegion->m_fDisplayRatio>fMinDisplayRatio)
		{	//缩小
			pRegion->m_fDisplayRatio=max(fMinDisplayRatio,pRegion->m_fDisplayRatio/1.6);
			if(fabs(pRegion->m_fDisplayRatio-fMinDisplayRatio)<EPS)
				pRegion->m_nOffsetX=pRegion->m_nOffsetY=0;
		}
		return pRegion->m_fDisplayRatio;
	}
	else
	{
		double fMinDisplayRatio=min(1,m_fDisplayAllRatio);
		if(zDelta==0)		//全显
			m_fDisplayRatio=m_fDisplayAllRatio;
		else if(zDelta>0)	//放大
			m_fDisplayRatio*=1.6;
		else if(m_fDisplayRatio>fMinDisplayRatio)
		{	//缩小
			m_fDisplayRatio=max(fMinDisplayRatio,m_fDisplayRatio/1.6);
			if(fabs(m_fDisplayRatio-fMinDisplayRatio)<EPS)
				m_nOffsetX=m_nOffsetY=0;
		}
		return m_fDisplayRatio;
	}	
}
RECT CImageDataFile::GetImageDisplayRect(DWORD dwRegionKey/*=0*/)
{
	CImageDataRegion *pRegion=m_hashRegionById.GetValue(dwRegionKey);
	CRect rect(0,0,0,0);
	if(pRegion)
	{
		rect.left=pRegion->m_nOffsetX;
		rect.top=pRegion->m_nOffsetY;
		rect.right=rect.left+(int)(pRegion->m_nDataWidth*pRegion->m_fDisplayRatio);
		rect.bottom=rect.top+(int)(pRegion->m_nDataHeight*pRegion->m_fDisplayRatio);
	}
	else
	{
		rect.left=m_nOffsetX;
		rect.top=m_nOffsetY;
		rect.right=rect.left+(int)(image_trans.GetWidth()*m_fDisplayRatio);
		rect.bottom=rect.top+(int)(image_trans.GetHeight()*m_fDisplayRatio);
	}
	return rect;
}
void CImageDataFile::ToBuffer(CBuffer &buffer)
{
	buffer.WriteString(m_sName);
	buffer.WriteString(file_path);
	//图片区域
	BUFFERPOP stack(&buffer,m_hashRegionById.GetNodeNum());
	buffer.WriteInteger(m_hashRegionById.GetNodeNum());
	for(CImageDataRegion *pRegion=m_hashRegionById.GetFirst();pRegion;pRegion=m_hashRegionById.GetNext())
	{
		buffer.WriteInteger(pRegion->GetKey());
		pRegion->ToBuffer(buffer);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		logerr.Log("The number record of image region is wrong!");
#else 
		logerr.Log("图片区域数量错误!");
#endif
}
void CImageDataFile::FromBuffer(CBuffer &buffer,long version)
{
	buffer.ReadString(m_sName);
	buffer.ReadString(file_path);
	bool bRetCode=ReadImageData(file_path);
	//图片区域
	m_hashRegionById.Empty();
	int n=buffer.ReadInteger();
	for(int i=0;i<n;i++)
	{
		int key=buffer.ReadInteger();
		CImageDataRegion *pRegion=m_hashRegionById.Add(key);
		pRegion->SetBelongImageData(this);
		pRegion->FromBuffer(buffer,version);
		if(bRetCode)
			InitDataBits(pRegion);
	}
}
//////////////////////////////////////////////////////////////////////////
// CAlphabetFont
CAlphabetFont::CAlphabetFont()
{
	serialNo=0;
	m_nTemplateH=0;
	m_fHtoW=0;
	pCharX=pCharL=pCharQ=NULL;
}
void CAlphabetFont::InitFromFolder(const char* folder_path)
{
	CXhChar100 sName;	
	WIN32_FIND_DATA FindFileData;
	char fn[MAX_PATH];
	sprintf(fn,"%s*.jpg",(char*)folder_path);
	HANDLE hFindFile = FindFirstFile(fn, &FindFileData);
	if (hFindFile ==INVALID_HANDLE_VALUE)
	{
		logerr.Log("未找到字符模板文件*.jpg!\n");
		return;
	}
	float fStdHeight=0,fStdWidth=0;
	BYTE cSampleCounts=0;
	BYTE_ARRAY bitsmap(4*24);
	do{
		CImageTransform image_trans;
		CXhChar200 sFileName("%s%s",(char*)folder_path, FindFileData.cFileName);
		_splitpath(sFileName,NULL,NULL,sName,NULL);
		image_trans.ReadImageFile(sFileName);
		//
		int nEffWidth=0;
		RECT rect;
		rect.left=rect.top=0;
		rect.right=image_trans.GetWidth();
		rect.bottom=image_trans.GetHeight();
		rect.right%16>0?nEffWidth=(rect.right/16)*2+2:nEffWidth=rect.right/8;
		//BYTE* lpBits=new BYTE[nEffWidth*rect.bottom];
		//memset(lpBits,0,nEffWidth*rect.bottom);
		bitsmap.Resize(nEffWidth*rect.bottom);
		bitsmap.Clear();
		POINT p;
		int wMinValidHeight=rect.bottom,wMaxValidHeight=0;
		int wMinValidWidth=rect.right,wMaxValidWidth=0;
		for(int i=0;i<rect.right;i++)
		{
			for(int j=0;j<rect.bottom;j++)
			{
				p.x=i;
				p.y=rect.bottom-1-j;
				if(!image_trans.IsBlackPixel(p.x,p.y))
				{
					int iByte=i/8,iBit=i%8;
					bitsmap[j*nEffWidth+iByte]|=BIT2BYTE[7-iBit];
				}
				else
				{
					wMinValidHeight=min(wMinValidHeight,j);
					wMaxValidHeight=max(wMaxValidHeight,j);
					wMinValidWidth=min(wMinValidWidth,i);
					wMaxValidWidth=max(wMaxValidWidth,i);
				}
			}
		}
		CImageChar* pText=listChars.append();
		memcpy(&pText->wChar.wHz,(char*)sName,2);
		if(pText->wChar.wHz=='Q'||pText->wChar.wHz=='L'||pText->wChar.wHz=='2'||pText->wChar.wHz=='8')
		{
			fStdHeight+=wMaxValidHeight-wMinValidHeight+1;
			fStdWidth +=wMaxValidWidth -wMinValidWidth +1;
			cSampleCounts++;
		}
		pText->InitTemplate(bitsmap,nEffWidth,rect);
		/*if(pText->wChar.wHz=='6'||pText->wChar.wHz=='8'||pText->wChar.wHz=='9')
		{
			CXhSimpleList<CImageChar::ISLAND> islands;
			int count=pText->DetectIslands(&islands);
			count=0;
		}*/
	} while (FindNextFile(hFindFile,&FindFileData));
	WORD wStdWidth =f2i(fStdWidth /cSampleCounts);
	WORD wStdHeight=f2i(fStdHeight/cSampleCounts);
	m_nTemplateH=max(wStdWidth,wStdHeight);
	//规则化字体库中各字体的标准高与宽
	for(CImageChar* pText=listChars.GetFirst();pText;pText=listChars.GetNext())
	{
		/*CXhChar50 charname("C:\\Recog\\template_%C.txt",pText->wChar.chars[0]);
		CLogFile logchar(charname);
		CLogErrorLife life(&logchar);
		int i,j;
		logchar.Log("-----------原始模板区域-------------\n");
		for(j=0;j<pText->GetImageHeight();j++)
		{
			for(i=0;i<pText->GetImageWidth();i++)
			{
				if(pText->IsBlackPixel(i,j))
					logchar.LogString(".",false);
				else
					logchar.LogString("B",false);
			}
			logchar.Log("\n");
		}*/
		if(pText->wChar.wHz=='L')
			pCharL=pText;
		else if(pText->wChar.wHz=='Q')
			pCharQ=pText;
		else if(pText->wChar.wHz=='X')
			pCharX=pText;
		else if(pText->wChar.wHz=='.')
			pCharPoint=pText;
		pText->Standardize(m_nTemplateH,m_nTemplateH);
		/*logchar.Log("-----------单位化模板区域-------------\n");
		for(j=0;j<pText->GetImageHeight();j++)
		{
			for(i=0;i<pText->GetImageWidth();i++)
			{
				if(pText->IsBlackPixel(i,j))
					logchar.LogString(".",false);
				else
					logchar.LogString("B",false);
			}
			logchar.Log("\n");
		}*/
	}
}
void CAlphabetFont::ToBuffer(CBuffer &buffer)
{
	buffer.WriteString(m_sName);
	buffer.WriteInteger(m_nTemplateH);
	buffer.WriteDouble(m_fHtoW);
	BUFFERPOP stack(&buffer,listChars.GetNodeNum());
	buffer.WriteInteger(listChars.GetNodeNum());
	for(CImageChar *pText=listChars.GetFirst();pText;pText=listChars.GetNext())
	{
		pText->ToBuffer(buffer);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		logerr.Log("The number record of char template is wrong!");
#else 
		logerr.Log("字符模板数量错误!");
#endif
}
void CAlphabetFont::FromBuffer(CBuffer &buffer,long version)
{
	buffer.ReadString(m_sName);
	buffer.ReadInteger(&m_nTemplateH);
	buffer.ReadDouble(&m_fHtoW);
	listChars.Empty();
	int n=buffer.ReadInteger();
	for(int i=0;i<n;i++)
	{
		CImageChar *pText=listChars.append();
		pText->FromBuffer(buffer,version);
	}
}
//////////////////////////////////////////////////////////////////////////
// CAlphabet
CAlphabet::CAlphabet()
{
	m_pActiveFont=NULL;
}
CAlphabetFont* CAlphabet::ActiveFont()
{
	if(m_pActiveFont==NULL)
		m_pActiveFont=listFonts.GetFirst();
	return m_pActiveFont;
}

void CAlphabet::ToBuffer(CBuffer &buffer)
{
	BUFFERPOP stack(&buffer,listFonts.GetNodeNum());
	buffer.WriteInteger(listFonts.GetNodeNum());
	for(CAlphabetFont *pSerial=listFonts.GetFirst();pSerial;pSerial=listFonts.GetNext())
	{
		buffer.WriteInteger(pSerial->serialNo);
		pSerial->ToBuffer(buffer);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		logerr.Log("The number record of char template is wrong!");
#else 
		logerr.Log("字符模板数量错误!");
#endif
}
void CAlphabet::FromBuffer(CBuffer &buffer,long version)
{
	listFonts.Empty();
	int n=buffer.ReadInteger();
	for(int i=0;i<n;i++)
	{
		int serial_no=buffer.ReadInteger();
		CAlphabetFont *pSerial=listFonts.Add(serial_no);
		pSerial->FromBuffer(buffer,version);
	}
}
void CAlphabet::LoadFromFile(const char* file_name)
{
	FILE* fp=fopen(file_name,"rb");
	if(fp==NULL)
	{
		logerr.Log("打开模板库文件失败！");
		return;
	}
	long version=1;
	long buf_size=0;
	fread(&version,4,1,fp);
	fread(&buf_size,4,1,fp);	//后续缓存长度
	CBuffer buffer;
	buffer.Write(NULL,buf_size);
	fread(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
	fclose(fp);
	buffer.SeekToBegin();
	FromBuffer(buffer,version);
}
void CAlphabet::SaveToFile(const char* file_name)
{
	FILE *fp=fopen(file_name,"wb");
	if(fp==NULL)
	{
		logerr.Log("打开模板库文件失败！");
		return;
	}
	CBuffer buffer;
	ToBuffer(buffer);
	int version=1;
	fwrite(&version,4,1,fp);
	long buf_size=buffer.GetLength();
	fwrite(&buf_size,4,1,fp);	//后续缓存长度
	fwrite(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
	fclose(fp);
}
//初始化模板字符
void CAlphabet::InitBrainData(const char* path)
{
	listFonts.Empty();
	//m_xTemplateLib.LoadFromFile(CXhChar500("%s\\template\\CharTemplate.tdb",APP_PATH));
	CAlphabetFont *pSerial=listFonts.Add(0);
	pSerial->InitFromFolder(path);
}
int CAlphabet::GetTemplateH(int template_serial)
{
	CAlphabetFont *pSerial=listFonts.GetValue(template_serial);
	if(pSerial)
		return pSerial->m_nTemplateH;
	else
		return 0;
}
//
bool bExport=true;
BOOL CAlphabet::RecognizeData(CImageChar* pText,double minMatchCoef/*=0.7*/,MATCHCOEF* pMachCoef/*=NULL*/)
{
	CXhSimpleList<CImageChar::ISLAND> islands;
	pText->DetectIslands(&islands);
	CXhSimpleList<MATCHCHAR> listMatches;
	for(CAlphabetFont *pFont=EnumFirstFont();pFont;pFont=EnumNextFont())
	{
		int nMinTextW_CHAR_1=pFont->m_nTemplateH/4;
		for(CImageChar* pTemplChar=pFont->listChars.GetFirst();pTemplChar;pTemplChar=pFont->listChars.GetNext())
		{
			if(!pTemplChar->m_bTemplEnabled)
				continue;
			if(pTemplChar->wChar.wHz=='1'&&pText->m_nDestValidWidth<nMinTextW_CHAR_1)
				continue;
			pText->wChar=pTemplChar->wChar;
			int checkfeature=pText->CheckFeatures(&islands);
			if(checkfeature<0)
				continue;	//确定不符合特征
			MATCHCHAR* pMatch=listMatches.AttachObject();
			pMatch->wChar=pTemplChar->wChar;
			pMatch->pTemplChar=pTemplChar;
			//pText->wChar=pTemplChar->wChar;
			//if(checkfeature==1)
			//	pMatch->matchcoef.original=pMatch->matchcoef.weighting=1.0;	//确定符合特征
			//else //if(checkfeature==0)
				pMatch->matchcoef=pTemplChar->CalMatchCoef(pText,&pMatch->ciOffsetX,&pMatch->ciOffsetY);
		}
	}
	//
	MATCHCOEF maxcoef(minMatchCoef,minMatchCoef);
	pText->wChar.wHz=0;
	MATCHCHAR* pLastMatch=NULL;
	for(MATCHCHAR* pMatch=listMatches.EnumObjectFirst();pMatch;pMatch=listMatches.EnumObjectNext())
	//for(MATCHCHAR* pMatch=listMatches.GetFirst();pMatch;pMatch=listMatches.GetNext())
	{
		if(pMatch->matchcoef.CompareWith(maxcoef)>0)
		{
			maxcoef=pMatch->matchcoef;
			pText->wChar=pMatch->wChar;
			pLastMatch=pMatch;
		}
	}
	if(pText->wChar.wHz==0&&ActiveFont()->pCharPoint&&ActiveFont()->pCharPoint->m_bTemplEnabled)
	{
		pText->wChar.wHz='.';
		if(pText->CheckFeatures(NULL)!=1)
			pText->wChar.wHz=0;
	}
	if(bExport&&pText->wChar.wHz>0&&pLastMatch&&pLastMatch->pTemplChar)
	{
		CImageChar* pTempText=pLastMatch->pTemplChar;
		CXhChar50 charname("C:\\Recog\\%C#%.2f.txt",pTempText->wChar.chars[0],pLastMatch->matchcoef.weighting);
		CLogFile logchar(charname);
		CLogErrorLife life(&logchar);
		int i,j;
		for(j=0;j<pTempText->m_nMaxTextH;j++)
		{
			for(i=0;i<pTempText->m_nMaxTextH;i++)
			{
				bool black1=pTempText->IsBlackPixel(i,j);
				bool black2=pText->IsBlackPixel(i-pLastMatch->ciOffsetX,j-pLastMatch->ciOffsetY);
				if(black1&&black2)
					logchar.LogString("*",false);
				else if(black1)
					logchar.LogString("1",false);
				else if(black2)
					logchar.LogString("2",false);
				else
					logchar.LogString(" ",false);
			}
			logchar.LogString("|",false);
			for(i=0;i<pTempText->m_nMaxTextH;i++)
			{
				bool black2=pText->IsBlackPixel(i,j);
				if(black2)
					logchar.LogString("2",false);
				else
					logchar.LogString(" ",false);
			}
			logchar.LogString("|",false);
			for(int i=0;i<pTempText->m_nMaxTextH;i++)
			{
				bool black1=pTempText->IsBlackPixel(i,j);
				if(black1)
					logchar.LogString("1",false);
				else
					logchar.LogString(" ",false);
			}
			logchar.LogString("\n");
		}
		life.EnableShowScreen(false);
	}
	if(pText->wChar.wHz>0&&pMachCoef)
		*pMachCoef=maxcoef;
	return pText->wChar.wHz>0;
}
#endif