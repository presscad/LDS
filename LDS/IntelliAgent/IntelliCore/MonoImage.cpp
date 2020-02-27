#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "MonoImage.h"
#include "LogFile.h"

#if defined(APP_EMBEDDED_MODULE)||defined(APP_EMBEDDED_MODULE_ENCRYPT)
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

int CMonoImage::WriteMonoBmpFile(const char* fileName, unsigned int width,unsigned effic_width, 
							   unsigned int height, unsigned char* image)
{
	BITMAPINFOHEADER bmpInfoHeader;
	BITMAPFILEHEADER bmpFileHeader;
	FILE *filep;
	unsigned int row, column;
	unsigned int extrabytes, bytesize;
	unsigned char *paddedImage = NULL, *paddedImagePtr, *imagePtr;

	/* The .bmp format requires that the image data is aligned on a 4 byte boundary.  For 24 - bit bitmaps,
	   this means that the width of the bitmap  * 3 must be a multiple of 4. This code determines
	   the extra padding needed to meet this requirement. */
   extrabytes = width%8>0?1:0;//(4 - (width * 3) % 4) % 4;
   long bmMonoImageRowBytes=width/8+extrabytes;
   long bmWidthBytes=((bmMonoImageRowBytes+3)/4)*4;

	// This is the size of the padded bitmap
	bytesize = bmWidthBytes*height;//(width * 3 + extrabytes) * height;

	// Fill the bitmap file header structure
	bmpFileHeader.bfType = 'MB';   // Bitmap header
	bmpFileHeader.bfSize = 0;      // This can be 0 for BI_RGB bitmaps
	bmpFileHeader.bfReserved1 = 0;
	bmpFileHeader.bfReserved2 = 0;
	bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// Fill the bitmap info structure
	bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfoHeader.biWidth = width;
	bmpInfoHeader.biHeight = height;
	bmpInfoHeader.biPlanes = 1;
	bmpInfoHeader.biBitCount = 1;            // 1 - bit mono bitmap
	bmpInfoHeader.biCompression = BI_RGB;
	bmpInfoHeader.biSizeImage = bytesize;     // includes padding for 4 byte alignment
	bmpInfoHeader.biXPelsPerMeter = 2952;
	bmpInfoHeader.biYPelsPerMeter = 2952;
	bmpInfoHeader.biClrUsed = 0;
	bmpInfoHeader.biClrImportant = 0;

	// Open file
	if ((filep = fopen(fileName, "wb")) == NULL) {
		logerr.Log("Error opening file");
		//AfxMessageBox("Error opening file");
		return FALSE;
	}

	// Write bmp file header
	if (fwrite(&bmpFileHeader, 1, sizeof(BITMAPFILEHEADER), filep) < sizeof(BITMAPFILEHEADER)) {
		logerr.Log("Error writing bitmap file header");
		//AfxMessageBox("Error writing bitmap file header");
		fclose(filep);
		return FALSE;
	}

	// Write bmp info header
	if (fwrite(&bmpInfoHeader, 1, sizeof(BITMAPINFOHEADER), filep) < sizeof(BITMAPINFOHEADER)) {
		logerr.Log("Error writing bitmap info header");
		//AfxMessageBox("Error writing bitmap info header");
		fclose(filep);
		return FALSE;
	}
	RGBQUAD palette[2];
	palette[0].rgbBlue=palette[0].rgbGreen=palette[0].rgbRed=palette[0].rgbReserved=palette[1].rgbReserved=0;	//��ɫ
	palette[1].rgbBlue=palette[1].rgbGreen=palette[1].rgbRed=255;	//��ɫ
	fwrite(palette,8,1,filep);
	// Allocate memory for some temporary storage
	paddedImage = (unsigned char *)calloc(sizeof(unsigned char), bytesize);
	if (paddedImage == NULL) {
		logerr.Log("Error allocating memory");
		//AfxMessageBox("Error allocating memory");
		fclose(filep);
		return FALSE;
	}

	/*	This code does three things.  First, it flips the image data upside down, as the .bmp
		format requires an upside down image.  Second, it pads the image data with extrabytes 
		number of bytes so that the width in bytes of the image data that is written to the
		file is a multiple of 4.  Finally, it swaps (r, g, b) for (b, g, r).  This is another
		quirk of the .bmp file format. */
	for (row = 0; row < height; row++) {
		//imagePtr = image + (height - 1 - row) * width * 3;
		imagePtr = image + (height-row-1) * effic_width;
		paddedImagePtr = paddedImage + row * bmWidthBytes;
		for (column = 0; column < effic_width; column++) {
			*paddedImagePtr = *imagePtr;
			imagePtr += 1;
			paddedImagePtr += 1;
		}
	}

	// Write bmp data
	if (fwrite(paddedImage, 1, bytesize, filep) < bytesize) {
		logerr.Log("Error writing bitmap data");
		//AfxMessageBox("Error writing bitmap data");
		free(paddedImage);
		fclose(filep);
		return FALSE;
	}

	// Close file
	fclose(filep);
	free(paddedImage);
	return TRUE;
}
int CMonoImage::WriteMonoBmpFile(const char* fileName)
{
	int nEffWidth=(this->m_nWidth+7)/8;
	BYTE_ARRAY imagebits(nEffWidth*m_nHeight,true);
	const BYTE BIT2BYTE[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
	for(int j=0;j<m_nHeight;j++)
	{
		if(BitCount()==1)
		{
			BYTE* rowdata=m_lpBitMap+j*m_nEffWidth;
			BYTE* prow=&imagebits[nEffWidth*j];
			for(int i=0;i<m_nEffWidth;i++)
			{
				if(!BlackBitIsTrue())
					prow[i]=rowdata[i];
				else
					prow[i]=rowdata[i]^0xff;
			}
		}
		else //if(BitCount()==8)
		{
			BYTE* rowdata=m_lpBitMap+j*this->m_nWidth;
			BYTE* prow=&imagebits[nEffWidth*j];
			for(int i=0;i<m_nWidth;i++)
			{
				int ibyte=i/8,ibit=i%8;
				if(rowdata[i]==0)	//�׵�
					*(prow+ibyte)|=BIT2BYTE[7-ibit];
			}
		}
	}
	return WriteMonoBmpFile(fileName,m_nWidth,nEffWidth,m_nHeight,imagebits);
}

const BYTE BIT2BYTE[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
//////////////////////////////////////////////////////////////////////////
CMonoImage::CMonoImage(BYTE* lpBitMap/*=NULL*/,int width/*=0*/,int height/*=0*/,
	bool black_is_true/*=true*/,BYTE ciBitCount/*=1*/)
{
	m_lpBitMap=NULL;
	m_parrRelaObjs=NULL;
	m_bExternalData=false;
	m_ciBitCount=ciBitCount;
	InitBitImage(lpBitMap,width,height,black_is_true);
}
void CMonoImage::InitBitImage(BYTE* lpBitMap/*=NULL*/,int width/*=0*/,int height/*=0*/,bool black_is_true/*=true*/)
{
	if(!m_bExternalData&&m_lpBitMap!=NULL)
		delete []m_lpBitMap;
	m_bBlackPixelIsTrue=black_is_true;
	m_lpBitMap=lpBitMap;
	m_bExternalData=m_lpBitMap!=NULL;
	m_nWidth=width;
	m_nHeight=height;
	long nBitWidth=m_nWidth*m_ciBitCount;
	m_nEffWidth=(nBitWidth+7)/8;
	if(m_lpBitMap==NULL&&m_nWidth>0&&m_nHeight>0)
	{
		int pixels=m_nEffWidth*m_nHeight;
		m_lpBitMap=new BYTE[pixels];
		memset(m_lpBitMap,0,pixels);
	}
}

CMonoImage::~CMonoImage(void)
{
	if(!m_bExternalData&&m_lpBitMap!=NULL)
	{
		delete []m_lpBitMap;
		m_lpBitMap=NULL;
	}
}
void CMonoImage::MirrorByAxisY()
{
	long nBitWidth=m_nWidth*m_ciBitCount;
	m_nEffWidth=(nBitWidth+7)/8;
	BYTE mempool[2048]={0};
	BYTE_ARRAY rowdata(m_nEffWidth,false,m_nEffWidth<2048?mempool:NULL,m_nEffWidth<2048?m_nEffWidth:0);
	BYTE* pRowData=m_lpBitMap;
	BYTE biBitCount=BitCount();
	DYN_ARRAY<PIXEL_RELAOBJ> rowRelaObjs;
	PIXEL_RELAOBJ* pRowRelaObj=NULL;
	if(m_parrRelaObjs!=NULL)
	{
		rowRelaObjs.Resize(m_nWidth);
		pRowRelaObj=m_parrRelaObjs;
	}

	for(int yJ=0;yJ<m_nHeight;yJ++)
	{
		for(int xI=0;xI<m_nEffWidth;xI++)
		{	//��Y�᾵��ͼ��
			if(biBitCount==1)
			{
				BYTE cbImgPixel=pRowData[m_nEffWidth-1-xI];
				rowdata[xI]=0;
				for(int bitI=0;bitI<8;bitI++)
				{
					if((cbImgPixel&BIT2BYTE[bitI])>0)
						rowdata[xI]|=BIT2BYTE[7-bitI];
				}
			}
			else if(biBitCount==8)
				rowdata[xI]=pRowData[m_nWidth-1-xI];
			if(m_parrRelaObjs!=NULL)
				rowRelaObjs[xI]=pRowRelaObj[m_nWidth-1-xI];
		}
		memcpy(pRowData,rowdata,m_nEffWidth);
		memcpy(pRowRelaObj,rowRelaObjs,m_nWidth*sizeof(PIXEL_RELAOBJ));
		pRowData+=m_nEffWidth;
		pRowRelaObj+=m_nWidth;
	}
}
bool CMonoImage::SetPixelRelaObj(int i,int j,PIXEL_RELAOBJ& relaObj)
{
	if(m_parrRelaObjs==NULL)
		return false;
	if( i<0||i>=m_nWidth || j<0||j>=m_nHeight)
		return false;
	int index=j*m_nWidth+i;
	if(m_parrRelaObjs[index].hRod==0||m_parrRelaObjs[index].zDepth<relaObj.zDepth-0.01)
		m_parrRelaObjs[index]=relaObj;
	else if(m_parrRelaObjs[index].zDepth<relaObj.zDepth+0.01&&
		m_parrRelaObjs[index].bTipPixel&&!relaObj.bTipPixel)
		m_parrRelaObjs[index]=relaObj;	//��Z����������̫������£�����ȡֱ���м�����ع�����Ϣ
	return true;
}
bool CMonoImage::SetPixelState(int i,int j,bool black/*=true*/)
{
	if(i<0||j<0||i>=m_nWidth||j>=m_nHeight)
		return false;
	BYTE biBitCount=BitCount();
	if(biBitCount==1)
	{
		bool pixelstate=BlackBitIsTrue()?black:!black;
		int iByte=i/8,iBit=i%8;
		if(pixelstate)
			m_lpBitMap[j*m_nEffWidth+iByte]|=BIT2BYTE[7-iBit];
		else
			m_lpBitMap[j*m_nEffWidth+iByte]&=(0XFF-BIT2BYTE[7-iBit]);
	}
	else if(biBitCount==8)
		m_lpBitMap[j*m_nWidth+i]=BlackBitIsTrue()?black:!black;
	return black;
}
bool CMonoImage::IsBlackPixel(int i,int j)
{
	if( i<0||i>=m_nWidth || j<0||j>=m_nHeight)
		return false;

	BYTE biBitCount=BitCount();
	if(biBitCount==1)
	{
		int iByte=i/8,iBit=i%8;
		int gray=m_lpBitMap[j*m_nEffWidth+iByte]&BIT2BYTE[7-iBit];	//��ȡ�õ�ĻҶ�ֵ
		//�����������Ǻ�ɫ��
		return BlackBitIsTrue() ? (gray==BIT2BYTE[7-iBit]) : (gray!=BIT2BYTE[7-iBit]);
	}
	else
	{
		int gray=m_lpBitMap[j*m_nWidth+i];	//��ȡ�õ�ĻҶ�ֵ
		//�����������Ǻ�ɫ��
		return BlackBitIsTrue() ? (gray&0x01) : (gray&0x01)==0;
	}
}
PIXEL_RELAOBJ* CMonoImage::GetPixelRelaObj(int i,int j)
{
	if(m_parrRelaObjs==NULL||!IsBlackPixel(i,j))
		return false;
	return &m_parrRelaObjs[j*m_nWidth+i];
}
PIXEL_RELAOBJ* CMonoImage::GetPixelRelaObjEx(int& i,int& j,
	BYTE ciRadiusOfSearchNearFrontObj/*=1*/,int* piExcludeXI/*=NULL*/,int* pjExcludeYJ/*=NULL*/)
{
	PIXEL_RELAOBJ *pFrontPixelObj=NULL,*pPixelObj=NULL;
	pFrontPixelObj=pPixelObj=GetPixelRelaObj(i,j);
	if(pFrontPixelObj==NULL)
		return NULL;
	int xI=i,yJ=j;
	for(int x=-ciRadiusOfSearchNearFrontObj;x<=ciRadiusOfSearchNearFrontObj;x++)
	{
		for(int y=-ciRadiusOfSearchNearFrontObj;y<=ciRadiusOfSearchNearFrontObj;y++)
		{
			if(x==0&&y==0)
				continue;
			if(piExcludeXI&&pjExcludeYJ&&i+x==*piExcludeXI&&j+y==*pjExcludeYJ)
				continue;
			if((pPixelObj=GetPixelRelaObj(i+x,j+y))==NULL)
				continue;
			if(pPixelObj->hRod!=pFrontPixelObj->hRod&&pPixelObj->zDepth>pFrontPixelObj->zDepth)
			{
				pFrontPixelObj=pPixelObj;
				xI=x+i;
				yJ=y+j;
			}
		}
	}
	i=xI;
	j=yJ;
	return pFrontPixelObj;
}
#include <math.h>
void CMonoImage::DrawPoint(const double* point,long hRelaNode/*=0*/)
{
	int xI=point[0]>=0?(int)(point[0]+0.5):(int)(point[0]-0.5);
	int yJ=point[1]>=0?(int)(point[1]+0.5):(int)(point[1]-0.5);
	SetPixelState(xI,yJ);	//��֤������ֽ��Խ�����
	PIXEL_RELAOBJ relaObj(hRelaNode,(float)point[2],true);
	relaObj.ciObjType=0;
	SetPixelRelaObj(xI,yJ,relaObj);
}
void CMonoImage::DrawLine(const double* pxStart,const double* pxEnd,long hRelaRod/*=0*/)
{
	double start[3],end[3];
	bool swapping=false;
	if(pxStart[1]>pxEnd[1]+1)
		swapping=true;	//�����������»�
	else if(fabs(pxStart[1]-pxEnd[1])<1&&pxStart[0]>pxEnd[0])
		swapping=true;	//�ӽ�ˮƽʱ�������һ�����أ����ȴ������һ�
	if(swapping)
	{	//��ֹͬһ���߻���ʼ->������->ʼ���������������Ĳ���
		memcpy(start,pxEnd,3*sizeof(double));
		memcpy(end,pxStart,3*sizeof(double));
	}
	else
	{
		memcpy(start,pxStart,3*sizeof(double));
		memcpy(end,pxEnd,3*sizeof(double));
	}
	double sx=start[0];
	double sy=start[1];
	double sz=start[2];
	double ex=end[0];
	double ey=end[1];
	double ez=end[2];
	double dy=end[1]-start[1];
	double dx=end[0]-start[0];
	if(fabs(dy)+fabs(dx)<0.01)
		return;	//������ֱ�ڵ�ǰ��ͼƽ���ֱ��
	int xPrevI=-1,yPrevJ=-1;
	if(fabs(dy)>fabs(dx))
	{	//ֱ����Y�������
		double coefX2Y=(end[0]-start[0])/(end[1]-start[1]);
		double coefZ2Y=(end[2]-start[2])/(end[1]-start[1]);
		//����Բ��������꣬����ᵼ������ͶӰ���ߵĲ�ͬ�߶λ���ʱ������ȫ�ص�
		int syi=(int)(0.5+sy);
		sx+=coefX2Y*(syi-sy);
		sz+=coefZ2Y*(syi-sy);
		sy=syi;
		int stepX=0;
		if(ex>sx+0.0001)
			stepX=1;
		else if(ex<sx-0.0001)
			stepX=-1;
		int stepY=ey>sy?1:-1;
		for(double yfJ=sy;(yfJ<=ey&&ey>sy)||(yfJ>=ey&&ey<sy);yfJ+=stepY)
		{
			int xI=(int)(0.5+sx+coefX2Y*(yfJ-sy));
			int yJ=(int)(0.5+yfJ);
			SetPixelState(xI,yJ);

			double zDepth=sz+coefZ2Y*(yfJ-sy);
			bool tippoint=fabs(yfJ-sy)<=0.000001;	//�˵���˼����м�������غ�ͬZֵʱ����ȡ�м�������Ϣ
			if((yfJ+stepY>ey&&ey>sy)||(yfJ+stepY<ey&&ey<sy))
				tippoint=true;
			PIXEL_RELAOBJ relaObj(hRelaRod,(float)zDepth,tippoint);
			relaObj.ciObjType=1;
			SetPixelRelaObj(xI,yJ,relaObj);

			if(xPrevI>=0&&yPrevJ>=0&&(abs(yJ-yPrevJ)+abs(xI-xPrevI)==2))
			{
				SetPixelState(xI-stepX,yJ);	//��֤������ֽ��Խ�����
				SetPixelRelaObj(xI-stepX,yJ,relaObj);
			}
			xPrevI=xI;
			yPrevJ=yJ;
		}
	}
	else
	{	//ֱ����X�������
		double coefY2X=(end[1]-start[1])/(end[0]-start[0]);
		double coefZ2X=(end[2]-start[2])/(end[0]-start[0]);
		//����Բ��������꣬����ᵼ������ͶӰ���ߵĲ�ͬ�߶λ���ʱ������ȫ�ص�
		int sxi=(int)(0.5+sx);
		sy+=coefY2X*(sxi-sx);
		sz+=coefZ2X*(sxi-sx);
		sx=sxi;
		int stepX=ex>sx?1:-1;
		int stepY=0;
		if(ey>sy+0.0001)
			stepY=1;
		else if(ey<sy-0.0001)
			stepY=-1;
		for(double xfI=sx;(xfI<=ex&&ex>sx)||(xfI>=ex&&ex<sx);xfI+=stepX)
		{
			int xI=(int)(0.5+xfI);
			int yJ=(int)(0.5+sy+coefY2X*(xfI-sx));
			SetPixelState(xI,yJ);

			double zDepth=sz+coefZ2X*(xfI-sx);
			bool tippoint=fabs(xfI-sx)<=0.000001;	//�˵���˼����м�������غ�ͬZֵʱ����ȡ�м�������Ϣ
			if((xfI+stepX>ex&&ex>sx)||(xfI+stepX<ex&&ex<sx))
				tippoint=true;
			PIXEL_RELAOBJ relaObj(hRelaRod,(float)zDepth,tippoint);
			SetPixelRelaObj(xI,yJ,relaObj);

			if(xPrevI>=0&&yPrevJ>=0&&(abs(yJ-yPrevJ)+abs(xI-xPrevI)==2))
			{
				SetPixelState(xI,yJ-stepY);	//��֤������ֽ��Խ�����
				SetPixelRelaObj(xI,yJ-stepY,relaObj);
			}
			xPrevI=xI;
			yPrevJ=yJ;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////
CByteMonoImage::CByteMonoImage(BYTE* lpBitMap/*=NULL*/,int width/*=0*/,int height/*=0*/,bool black_is_true/*=true*/)
	: CMonoImage(lpBitMap,width,height,black_is_true,8)
{
}
CByteMonoImage::~CByteMonoImage()
{
	if(!m_bExternalData&&m_lpBitMap!=NULL)
	{
		delete []m_lpBitMap;
		m_lpBitMap=NULL;
	}
}
bool CByteMonoImage::IsDetect(int i,int j)
{
	return (0x80&m_lpBitMap[j*m_nWidth+i])>0;
}
bool CByteMonoImage::SetDetect(int i,int j,bool detect/*=true*/)
{
	BYTE* pxyPixel=&m_lpBitMap[j*m_nWidth+i];
	if(detect)
		(*pxyPixel)|=0x80;
	else if((*pxyPixel)>=0x80)
		(*pxyPixel)-=0x80;
	return ((*pxyPixel)&0x80)!=0;
}
void CByteMonoImage::ClearDetectMark()
{
	for(int i=0;i<m_nWidth;i++)
		for(int j=0;j<m_nHeight;j++)
			m_lpBitMap[j*m_nWidth+i]&=0x0f;
}
bool CByteMonoImage::PIXEL::get_Black()
{
	return pcbPixel==NULL?false:((*pcbPixel)&0x0f)>0;
}
bool CByteMonoImage::PIXEL::set_Black(bool black)
{
	if(pcbPixel&&black)
		*pcbPixel|=0x01;
	return pcbPixel!=NULL&&black;
}
char CByteMonoImage::PIXEL::set_Connected(char connected)		//�������Ƿ�Ϊ��ͨ���������
{
	if(pcbPixel==NULL)
		return NONEVISIT;
	if(connected==NONEVISIT)
	{
		(*pcbPixel)&=0x0f;
		return connected;
	}
	else if(connected==CONNECTED)
		(*pcbPixel)|=0xc0;	//0xc0=0x80|0x40
	else if(connected==UNDECIDED)
		(*pcbPixel)|=0x80;
	else
		(*pcbPixel)&=0x0f;
	return connected;
}
char CByteMonoImage::PIXEL::get_Connected()
{
	if(pcbPixel==NULL)
		return NONEVISIT;
	BYTE cbFlag=(*pcbPixel);
	if(!(cbFlag&0x80))
		return NONEVISIT;
	if(cbFlag&0x40)
		return CONNECTED;
	else
		return UNDECIDED;
}
//�������Ƿ�Ϊ��ͨ���������
int island_id=1;
//TODO:�����ݹ���ܵ��¶�ջ��� wjh-2018.6.20
//ʵ��֤��listStatPixels����Ԥ�������CXhSimpleList�ڴ����Ч�ʸ��� wjh-2018.4.12
bool CByteMonoImage::StatNearWhitePixels(int i,int j,ISLAND* pIsland,PRESET_ARRAY1600<PIXEL>* listStatPixels,BYTE cDetectFlag/*=0x0f*/)
{
	BYTE* pxyPixel=&m_lpBitMap[j*m_nWidth+i];
	PIXEL pixel(pxyPixel);
	if(i==0||j==0||i==m_nWidth-1||j==m_nHeight-1)
	{
		//һֱδ�����׵���Ϊʲô��Ӻ������д���������ͼ���²�Ϊ��ʱ���룬������û�ã�����ע�͵���wjh-2018.5.22
		//if(j==0)
		//	StatNearWhitePixels(i,1,pIsland,listStatPixels,cDetectFlag);
		if(pixel.Black)
			return true;
		else
		{
			if(listStatPixels)
				listStatPixels->Append(pixel);
			return false;	//��ɫ������߽���ͨ��һ���޷����γɰ�ɫ�µ�
		}
	}
	if(pixel.Black)
		return true;	//��ǰΪ�ڵ�ֹͣͳ������
	if(pixel.Connected==PIXEL::CONNECTED)
		return false;	//�Ѽ�����ȷ��Ϊ��߽���ͨ�İ׵�
	if(pixel.Connected==PIXEL::UNDECIDED)
		return true;	//�Ѽ����Ľڵ�
	pixel.Connected=PIXEL::UNDECIDED;	//*pxyPixel|=0x80;	�����Ѽ����
	if(listStatPixels)
		listStatPixels->Append(pixel);//AttachObject(pixel);
	//if(pLogFile)
	//	pLogFile->Log("(%2d,%2d)=%d",i,j,island_id);
	pIsland->x+=i;		//�ۻ����ļ������ֵ
	pIsland->y+=j;
	if(pIsland->count==0)
		pIsland->maxy=pIsland->miny=j;
	else
	{
		pIsland->maxy=max(pIsland->maxy,j);
		pIsland->miny=min(pIsland->miny,j);
	}
	pIsland->count+=1;
	bool canconinue=true;
	for(int movJ=-1;movJ<=1;movJ++)
	{
		int jj=movJ+j;
		if(jj<0||jj>=m_nHeight)
			continue;
		if(jj<j&&!(cDetectFlag&DETECT_UP)||jj>j&&!(cDetectFlag&DETECT_DOWN)||jj==j&&!(cDetectFlag&DETECT_Y0))
			continue;
		for(int movI=-1;movI<=1;movI++)
		{
			int ii=movI+i;
			//�Ƴ�abs(movJ)==abs(movI)�������Ϊ���ܴ��ڰ׵�µ�����߽��Ϊ�ڵ�Խǰ׵�Ҳ�Խ���� wjh-2018.3.28
			if(ii<0||ii>=m_nWidth||(ii==i&&jj==j)||abs(movJ)==abs(movI))
				continue;
			if(ii<i&&!(cDetectFlag&DETECT_LEFT)||ii>i&&!(cDetectFlag&DETECT_RIGHT)||ii==i&&!(cDetectFlag&DETECT_X0))
				continue;
			if(!(canconinue=StatNearWhitePixels(ii,jj,pIsland,listStatPixels,cDetectFlag)))
				return false;
		}
	}
	return canconinue;
}
int CByteMonoImage::DetectIslands(CXhSimpleList<ISLAND>* listIslands)
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
				PRESET_ARRAY1600<PIXEL> listStatPixels;
				island.Clear();
				//�������DETECT_X0|DETECT_Y0����Ϊ���ܴ��ڰ׵�µ�����߽��Ϊ�ڵ�Խǰ׵�Ҳ�Խ���� wjh-2018.3.28
				if(StatNearWhitePixels(i,j,&island,&listStatPixels,DETECT_UP|DETECT_Y0|DETECT_DOWN|DETECT_LEFT|DETECT_X0|DETECT_RIGHT))//,&logfile))
				{	//�鵽�İ׵㼯��Ϊ�µ�
					listIslands->AttachObject(island);
					//logfile.Log("%d#island finded @%2d-%2d",island_id,i,j);
					island_id++;
					count++;
				}
				else
				{	//�鵽�İ׵㼯��ʵ����߽���ͨ�����趨�߽���
					for(UINT ii=0;ii<listStatPixels.Count;ii++)
					{
						PIXEL* pPixel=&listStatPixels.At(ii);
						pPixel->Connected=PIXEL::CONNECTED;
					}
				}
			}
			else if((m_lpBitMap[rowstarti+i]&0x7f)==0)
			{
				PRESET_ARRAY1600<PIXEL> listStatPixels;
				StatNearWhitePixels(i,j,&island,&listStatPixels,DETECT_UP|DETECT_Y0|DETECT_DOWN|DETECT_LEFT|DETECT_X0|DETECT_RIGHT);//,&logfile);
				for(UINT ii=0;ii<listStatPixels.Count;ii++)
				{
					PIXEL* pPixel=&listStatPixels.At(ii);
					pPixel->Connected=PIXEL::CONNECTED;
				}
				island.Clear();
			}
			//if(blackpixelStarted&&whitepixelStarted&&blackpixelEnded)
		}
		rowstarti+=m_nWidth;
	}
	ClearDetectMark();
	return count;
}