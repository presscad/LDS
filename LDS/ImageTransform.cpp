#include "stdafx.h"
#include "ImageFile.h"
#include "ximajpg.h"
#include "ImageTransform.h"
#include "HashTable.h"
#include "LogFile.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#if defined(__TMA_)||defined(__LMA_)
#define      Pi               3.14159265358979323846
#define      RADTODEG_COEF    0.01745329251994329577
#define      DEGTORAD_COEF    57.2957795130823208768
//////////////////////////////////////////////////////////////////////////
//CSamplePixelLine
CSamplePixelLine::CSamplePixelLine()
{

}
CSamplePixelLine::~CSamplePixelLine()
{

}

bool CSamplePixelLine::AnalyzeLinePixels(bool* pixels,int nNum)
{
	bool prevGrayness = false;	//记录前一象素点颜色
	int maxPixelsOfValidSeg=30;	//有效黑色线段的最多象素数
	int minPixelsOfValidSeg=0;	//有效黑色线段的最少象素数

	m_nValidBlackSeg=0;
	for(int i = 0;i<nNum;i++)
	{
		if((prevGrayness&&!pixels[i]) || (prevGrayness&&pixels[i]&&i==nNum-1))
		{	//前一点为黑点当前点为白点，意味一个黑色线段的结束
			int seg_len=i-shiftSegArr[m_nValidBlackSeg-1][0];
			if(seg_len>maxPixelsOfValidSeg||seg_len<minPixelsOfValidSeg)
				m_nValidBlackSeg--;	//黑色线段象素数太多或太少，为不合理黑色线段，不进行统计回滚
			else
				shiftSegArr[m_nValidBlackSeg-1][1]=i-1;
		}
		else if(!prevGrayness && pixels[i])
		{	//前一点为白点当前点为黑点，意味一个黑色线段的开始
			m_nValidBlackSeg++;
			if(m_nValidBlackSeg>5)
				break;	//系统最多只检测靠前的５个黑色线段
			shiftSegArr[m_nValidBlackSeg-1][0]=i;
			if(m_nValidBlackSeg>5)
			{
				m_nValidBlackSeg=5;
				break;
			}
		}
		prevGrayness=pixels[i];
	}
	return m_nValidBlackSeg>0;
}
bool CSamplePixelLine::
	FindBlackSegInSpecScope(int scope_start,int scope_end,int* seg_start, int* seg_end)
{
	for(int i=0;i<m_nValidBlackSeg;i++)
	{
		if(shiftSegArr[i][1]<scope_start)
			continue;	//当前黑色线段在检测范围之前
		else if(shiftSegArr[i][0]>scope_end)
			continue;	//当前黑色线段在检测范围之后
		else			//当前黑色线段与检测范围有重叠区域
		{
			if(seg_start)
				*seg_start=shiftSegArr[i][0];
			if(seg_end)
				*seg_end=shiftSegArr[i][1];
			return true;
		}
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
//CImageTransForm
CImageTransform::CImageTransform()
{
	m_lpBMIH=NULL;
	m_lpOrgBits=NULL;
	m_lpDesBitstMap=m_lpBits=m_lpGrayBitsMap=NULL;
	m_nWidth=m_nEffByteWidth=m_nHeight=0;
	m_ubGreynessThreshold=85;
	m_lpPixelGrayThreshold=NULL;
	m_nRectColNum=m_nRectRowNum=0;
	m_nRectWidth=m_nRectHeight=0;
}

CImageTransform::~CImageTransform()
{
	if(m_lpBits)
		delete []m_lpBits;
	if(m_lpOrgBits)
		delete []m_lpOrgBits;
	if(m_lpGrayBitsMap)
		delete []m_lpGrayBitsMap;
	if(m_lpBMIH)
		delete []m_lpBMIH;
	if(m_lpPixelGrayThreshold)
		delete []m_lpPixelGrayThreshold;
}
//BMP格式图片读写
bool CImageTransform::ReadBmpFile(FILE* fp)
{
	return false;
}
bool CImageTransform::WriteBmpFile(FILE* fp)
{
	BITMAPINFOHEADER bmpInfoHeader;
	BITMAPFILEHEADER bmpFileHeader;
	unsigned int width=GetWidth();
	unsigned int height=GetHeight();
	unsigned int row, column;
	unsigned int extrabytes, bytesize;
	unsigned char *paddedImage = NULL, *paddedImagePtr, *imagePtr;
	BYTE* image=GetBits();

	/* The .bmp format requires that the image data is aligned on a 4 byte boundary.  For 24 - bit bitmaps,
	   this means that the width of the bitmap  * 3 must be a multiple of 4. This code determines
	   the extra padding needed to meet this requirement. */
	extrabytes = (4 - (width * 3) % 4) % 4;

	// This is the size of the padded bitmap
	bytesize = (width * 3 + extrabytes) * height;

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
	bmpInfoHeader.biBitCount = 24;            // 8 - bit bitmap
	bmpInfoHeader.biCompression = BI_RGB;
	bmpInfoHeader.biSizeImage = bytesize;     // includes padding for 4 byte alignment
	bmpInfoHeader.biXPelsPerMeter = 2952;
	bmpInfoHeader.biYPelsPerMeter = 2952;
	bmpInfoHeader.biClrUsed = 0;
	bmpInfoHeader.biClrImportant = 0;

	// Write bmp file header
	if (fwrite(&bmpFileHeader, 1, sizeof(BITMAPFILEHEADER), fp) < sizeof(BITMAPFILEHEADER)) {
		AfxMessageBox("Error writing bitmap file header");
		return false;
	}

	// Write bmp info header
	if (fwrite(&bmpInfoHeader, 1, sizeof(BITMAPINFOHEADER), fp) < sizeof(BITMAPINFOHEADER)) {
		AfxMessageBox("Error writing bitmap info header");
		return false;
	}

	// Allocate memory for some temporary storage
	paddedImage = (unsigned char *)calloc(sizeof(unsigned char), bytesize);
	if (paddedImage == NULL) {
		AfxMessageBox("Error allocating memory");
		return false;
	}

	/*	This code does three things.  First, it flips the image data upside down, as the .bmp
		format requires an upside down image.  Second, it pads the image data with extrabytes 
		number of bytes so that the width in bytes of the image data that is written to the
		file is a multiple of 4.  Finally, it swaps (r, g, b) for (b, g, r).  This is another
		quirk of the .bmp file format. */
	for (row = 0; row < height; row++) {
		//imagePtr = image + (height - 1 - row) * width * 3;
		imagePtr = image + row * m_nEffByteWidth;
		paddedImagePtr = paddedImage + row * (width * 3 + extrabytes);
		for (column = 0; column < width; column++) {
			*paddedImagePtr = *(imagePtr + 2);
			*(paddedImagePtr + 1) = *(imagePtr + 1);
			*(paddedImagePtr + 2) = *(imagePtr);
			imagePtr += 3;
			paddedImagePtr += 3;
		}
	}

	// Write bmp data
	if (fwrite(paddedImage, 1, bytesize, fp) < bytesize) {
		AfxMessageBox("Error writing bitmap data");
		free(paddedImage);
		return false;
	}

	// Close file
	free(paddedImage);
	return true;
}
//JPG格式图片的读写
bool CImageTransform::ReadJpegFile(FILE* fp)
{
	CImageFileJpeg imageFile;
	if(!imageFile.ReadImageFile(fp))
		return false;
	if(m_lpBits)
		delete []m_lpBits;
	m_nWidth=imageFile.GetWidth();
	m_nEffByteWidth=imageFile.GetEffWidth();
	m_nHeight=imageFile.GetHeight();
	m_uBitcount=imageFile.GetBpp();
	DWORD dwCount=m_nEffByteWidth*m_nHeight;
	m_lpBits=new BYTE[dwCount];
	imageFile.GetBitmapBits(dwCount,m_lpBits);
	//记录原始Bits
	if(m_lpOrgBits)
		delete []m_lpOrgBits;
	m_lpOrgBits=new BYTE[dwCount];
	imageFile.GetBitmapBits(dwCount,m_lpOrgBits);
	//
	dwCount=sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * imageFile.head.biClrUsed;
	m_lpBMIH = (LPBITMAPINFOHEADER) new BYTE[dwCount];
	memcpy(m_lpBMIH,&imageFile.head,sizeof(BITMAPINFOHEADER));
	memcpy(((BYTE*)m_lpBMIH)+sizeof(BITMAPINFOHEADER),imageFile.GetPalette(),sizeof(RGBQUAD) * imageFile.head.biClrUsed);
	InitPixelGrayThreshold();
	ProcessImage();
	//CalGreynessThreshold();	//自动计算灰度图的最佳二值化阈值
	//LocateDrawingRgnRect();//定位草图位置
	return true;
}
bool CImageTransform::WriteJpegFile(FILE* fp)
{
	return false;
}
//PNG格式图片的读写
bool CImageTransform::ReadPngFile(FILE* fp)
{
	return false;
}
bool CImageTransform::WritePngFile(FILE* fp)
{
	return false;
}
//图片的读与写
bool CImageTransform::ReadImageFile(const char* file_path)
{
	if(file_path)
	{
		FILE* fp=fopen(file_path,"rb");
		if(fp==NULL)
			return false;
		char drive[4],dir[MAX_PATH],fname[MAX_PATH],ext[MAX_PATH];
		_splitpath(file_path,drive,dir,fname,ext);
		bool retvalue=false;
		if(stricmp(ext,".bmp")==0)
			retvalue=ReadBmpFile(fp);
		else if(stricmp(ext,".jpg")==0)
			retvalue=ReadJpegFile(fp);
		else if(stricmp(ext,".png")==0)
			retvalue=ReadPngFile(fp);
		fclose(fp);
		return retvalue;
	}
	else
		return false;
}
bool CImageTransform::WriteImageFile(const char* file_path)
{
	if(file_path)
	{
		FILE* fp=fopen(file_path,"wb");
		if(fp==NULL)
			return false;
		char drive[4],dir[MAX_PATH],fname[MAX_PATH],ext[MAX_PATH];
		_splitpath(file_path,drive,dir,fname,ext);
		bool retvalue=false;
		if(stricmp(ext,".bmp")==0)
			retvalue=WriteBmpFile(fp);
		else if(stricmp(ext,".jpg")==0)
			retvalue=WriteJpegFile(fp);
		else if(stricmp(ext,".png")==0)
			retvalue=WritePngFile(fp);
		fclose(fp);
		return retvalue;
	}
	else
		return false;
}
//图像灰度值处理
BYTE CImageTransform::GetPixelGrayness(int x,int y)
{
	int index=y*m_nWidth+x;
	if(index>=m_nHeight*m_nWidth)
		return 0;
	if(m_lpGrayBitsMap)
		return m_lpGrayBitsMap[y*m_nWidth+x];
	else
		return 0;
}
//图像灰度值处理
BYTE CImageTransform::GetPixelGraynessThresold(int x,int y)
{
	int index=y*m_nWidth+x;
	if(index>=m_nHeight*m_nWidth)
		return 0;
	if(m_lpPixelGrayThreshold)
		return m_lpPixelGrayThreshold[y*m_nWidth+x];
	else
		return 0;
}
BYTE CImageTransform::CalGreynessThreshold()
{
	int i,j;
	BYTE minGreyness=255,maxGreyness=0;
	for(i=0;i<m_nWidth;i++)
	{
		for(j=0;j<m_nHeight;j++)
		{
			if(GetPixelGrayness(i,j)>maxGreyness)
				maxGreyness=GetPixelGrayness(i,j);
			if(GetPixelGrayness(i,j)<minGreyness)
				minGreyness=GetPixelGrayness(i,j);
		}
	}
	m_ubGreynessThreshold=(int)((maxGreyness-minGreyness)/2.5)+minGreyness;
	return m_ubGreynessThreshold;
}
bool CImageTransform::IsBlackPixel(int x,int y)
{
	return GetPixelGrayness(x,y)<GetPixelGraynessThresold(x,y);
}
void CImageTransform::CalRectPixelGrayThreshold(int iRow,int iCol,int RECT_WIDTH,int RECT_HEIGHT)
{
	const int RGB_SIZE=256;
	int arrRGBSumPixelNum [RGB_SIZE]={0};
	DWORD dwBitCount=m_nEffByteWidth*m_nHeight;
	for (int iWidth=0;iWidth<RECT_WIDTH;iWidth++)//遍历每个单元(长)
	{
		if((iCol==m_nRectColNum-1)&&(iCol*RECT_WIDTH+iWidth)>m_nWidth)
			continue;	//最后一个单元格宽度可能小于RECT_WIDTH
		for (int iHeight=0;iHeight<RECT_HEIGHT;iHeight++)
		{	//取出rgb数字
			DWORD iPixelStartPos=(iRow*RECT_HEIGHT+iHeight)*m_nEffByteWidth+(iCol*RECT_WIDTH+iWidth)*3;
			if(iPixelStartPos>dwBitCount)
				break;
			BYTE r=m_lpBits[iPixelStartPos+2];
			BYTE g=m_lpBits[iPixelStartPos+1];
			BYTE b=m_lpBits[iPixelStartPos+0];
			int rgbIndex=(b*114+g*587+r*299)/1000;
			arrRGBSumPixelNum[rgbIndex]++;
		}
	}
	int nCursorWidth =5;
	int unitRGBSumByArr[RGB_SIZE]={0};
	for(int i=0;i<RGB_SIZE;i++)
	{
		for(int	m=0;m<nCursorWidth;m++)
		{
			if(i-m>=0)
				unitRGBSumByArr[i]+=arrRGBSumPixelNum[i-m];
			if(i+m<RGB_SIZE)
				unitRGBSumByArr[i]+=arrRGBSumPixelNum[i+m];
		}
		unitRGBSumByArr[i]=(int)(unitRGBSumByArr[i]/nCursorWidth);
	}
	/*logerr.Log("第%d行 第%d列",iRow,iCol);
	if(iRow==0&&(iCol==0||iCol==1))
	{
		for(int k=0;k<RGB_SIZE;k++)
			logerr.Log("%d:%d",k,unitRGBSumByArr[k]);
	}*/
	int iMaxValueIndex=0,nMaxValue=0;
	for (int i=0;i<RGB_SIZE;i++ ) //找出最大
	{
		if(unitRGBSumByArr[i]>nMaxValue)
		{
			iMaxValueIndex=i;
			nMaxValue=unitRGBSumByArr[i];
		}
	}
	iMaxValueIndex-=20;		//暂时以出现频率最高的像素点为基准向前推20,作为单元格灰度阈值
	for (int iWidth=0;iWidth<RECT_WIDTH;iWidth++)//遍历每个单元(长)
	{
		if((iCol==m_nRectColNum-1)&&(iCol*RECT_WIDTH+iWidth)>m_nWidth)
			continue;	//最后一个单元格宽度可能小于RECT_WIDTH
		for (int iHeight=0;iHeight<RECT_HEIGHT;iHeight++)
		{
			int iPixelPos=(iRow*RECT_HEIGHT+iHeight)*m_nWidth+(iCol*RECT_WIDTH+iWidth);
			if(iPixelPos>=m_nWidth*m_nHeight)
				break;
			m_lpPixelGrayThreshold[iPixelPos]=iMaxValueIndex;
		}
	}
}
int CImageTransform::GetRectPixelGrayThreshold(int iRow,int iCol)
{
	int iPixelPos=(iRow*m_nRectHeight+1)*m_nWidth+(iCol*m_nRectWidth+1);
	if(iPixelPos>=m_nHeight*m_nWidth)
		return 0;
	else
		return m_lpPixelGrayThreshold[(iRow*m_nRectHeight+1)*m_nWidth+(iCol*m_nRectWidth+1)];
}
void CImageTransform::SetRectPixelGrayThreshold(int iRow,int iCol,int threshold)
{
	for (int iWidth=0;iWidth<m_nRectWidth;iWidth++)
	{
		if((iCol==m_nRectColNum-1)&&(iCol*m_nRectWidth+iWidth)>m_nWidth)
			continue;	//最后一个单元格宽度可能小于RECT_WIDTH
		for (int iHeight=0;iHeight<m_nRectHeight;iHeight++)
		{
			int iPixelPos=(iRow*m_nRectHeight+1)*m_nWidth+(iCol*m_nRectWidth+1);
			if(iPixelPos>=m_nHeight*m_nWidth)
				break;
			m_lpPixelGrayThreshold[(iRow*m_nRectHeight+iHeight)*m_nWidth+(iCol*m_nRectWidth+iWidth)]=threshold;
		}
	}
	ProcessImage();
}
void CImageTransform::InitPixelGrayThreshold()
{
	CLogErrorLife log5;
	if(m_lpBits==NULL||m_nWidth==0||m_nHeight==0)
		return;
	if(m_lpPixelGrayThreshold!=NULL)
		delete []m_lpPixelGrayThreshold;
	m_lpPixelGrayThreshold = new BYTE[m_nWidth*m_nHeight];
	memset(m_lpPixelGrayThreshold,0,sizeof(BYTE)*m_nWidth*m_nHeight);
	if(m_nWidth<30)	//字符模板不需分区计算
		return CalRectPixelGrayThreshold(0,0,m_nWidth,m_nHeight);
	//1.计算矩形区域宽度和高度
	m_nRectColNum=16,m_nRectRowNum=0;		//宽高等分数
	//1.1 计算矩形区域宽度
	m_nRectWidth=m_nWidth/m_nRectColNum, m_nRectHeight=0;
	int nWidthOdd=m_nWidth%m_nRectWidth;
	while(nWidthOdd<=0.5*m_nRectWidth)
	{
		m_nRectWidth++;
		nWidthOdd=m_nWidth%m_nRectWidth;
	}
	m_nRectColNum = m_nWidth/m_nRectWidth;
	if(nWidthOdd>0)
		m_nRectColNum+=1;
	//1.2 计算矩形高度,保证等分图片高度
	m_nRectHeight=m_nRectWidth;
	if(m_nRectHeight>m_nHeight)
		m_nRectHeight=m_nHeight;
	else 
	{
		int nOdd=m_nHeight%m_nRectHeight;
		while(nOdd!=0 && nOdd<0.5*m_nRectHeight)
		{
			m_nRectHeight--;
			nOdd=m_nHeight%m_nRectHeight;
		}
	}
	m_nRectRowNum = m_nHeight/m_nRectHeight;
	if(m_nHeight%m_nRectHeight>0)
		m_nRectRowNum+=1;
	//2.按矩形计算像素点灰度阈值
	for(int iCol=0;iCol<m_nRectColNum;iCol++)
	{
		for(int iRow=0;iRow<m_nRectRowNum;iRow++)
		{
			CalRectPixelGrayThreshold(iRow,iCol,m_nRectWidth,m_nRectHeight);	
		}
	}
}
//图像进行灰白化处理
bool CImageTransform::ProcessImage()
{
	if(m_lpBits==NULL||m_nWidth==0||m_nHeight==0)
		return false;
	if(m_lpGrayBitsMap)
		delete []m_lpGrayBitsMap;
	m_lpGrayBitsMap = new BYTE[m_nWidth*m_nHeight];
	for(int i=0;i<m_nWidth;i++)
	{
		for(int j=0;j<m_nHeight;j++)
		{
			BYTE r=m_lpOrgBits[j*m_nEffByteWidth+i*3+2];
			BYTE g=m_lpOrgBits[j*m_nEffByteWidth+i*3+1];
			BYTE b=m_lpOrgBits[j*m_nEffByteWidth+i*3+0];
			//m_lpGrayBitsMap[j*m_nWidth+i]=(b*114+g*587+r*299)/1000;
			/*RGB按照300:300:400比例分配
			字体颜色RGB(103,101,114)  综合值106.8
			背景颜色RGB(165,166,171)  综合值150.6
			*/
			m_lpGrayBitsMap[j*m_nWidth+i]=(b*114+g*587+r*299)/1000;
			if(m_lpGrayBitsMap[j*m_nWidth+i]>m_lpPixelGrayThreshold[j*m_nWidth+i])
			{
				m_lpBits[j*m_nEffByteWidth+i*3+2]=255;
				m_lpBits[j*m_nEffByteWidth+i*3+1]=255;
				m_lpBits[j*m_nEffByteWidth+i*3+0]=255;
			}
			else
			{
				m_lpBits[j*m_nEffByteWidth+i*3+2]=0;
				m_lpBits[j*m_nEffByteWidth+i*3+1]=0;
				m_lpBits[j*m_nEffByteWidth+i*3+0]=0;
			}
		}
	}
	return true;
}
//
bool CImageTransform::GapDetect(int start_x,int start_y,int maxGap,bool bScanByRow,int *gaplen)
{
	int i,step=1,absMaxGap=abs(maxGap);
	if(maxGap<0)
		step=-1;
	for(i=0;i<absMaxGap;i++)
	{
		if(bScanByRow)	//行扫描
			start_x+=step;
		else	//列扫描
			start_y+=step;
		if(IsBlackPixel(start_x,start_y))
		{
			*gaplen=i+1;
			return true;
		}
	}
	return false;
}
//
POINT CImageTransform::TransformPointToOriginalField(POINT p)
{
	POINT point;
	double yitax=(double)p.x/m_nDrawingRgnWidth;	//ηx
	double yitay=(double)p.y/m_nDrawingRgnHeight;	//ηy
	point.x=(long)((kesai[0]*p.x+m_leftUp.x)*(1-yitay)+(kesai[1]*p.x+m_leftDown.x)*yitay);
	point.y=(long)((kesai[2]*p.y+m_leftUp.y)*(1-yitax)+(kesai[3]*p.y+m_rightUp.y)*yitax);
	return point;
}
//
const BYTE BIT2BYTE[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
bool CImageTransform::RetrieveSpecifyRegion(double pos_x_coef, double pos_y_coef, double width_coef, double height_coef,BYTE** outBitsMap,long *outWidth,long *effWidth,long *outHeight)
{
	POINT leftdown,p;
	leftdown.x=(int)(pos_x_coef*m_nDrawingRgnWidth);
	leftdown.y=(int)(pos_y_coef*m_nDrawingRgnHeight);
	int width=*outWidth=(int)(width_coef*m_nDrawingRgnWidth);
	int height=*outHeight=(int)(height_coef*m_nDrawingRgnHeight);
	int eff_width;
	width%16>0?eff_width=(width/16)*2+2:eff_width=width/8;
	*effWidth=eff_width;
	*outBitsMap = new BYTE[eff_width*height];
	memset(*outBitsMap,0,eff_width*height);
	for(int j=0;j<height;j++)
	{
		for(int i=0;i<width;i++)
		{
			p.x=i+leftdown.x;
			p.y=height-j-1+leftdown.y;
			p=TransformPointToOriginalField(p);
			if(!IsBlackPixel(p.x,p.y))//i+leftUp.x,GetHeight()-j-leftUp.y-1))
			{
				int iByte=i/8,iBit=i%8;
				(*outBitsMap)[j*eff_width+iByte]|=BIT2BYTE[7-iBit];
			}
		}
	}
	return true;
}
//
bool CImageTransform::CalDrawingRgnRect()
{
	int i,j;
	CSamplePixelLine sampleLineArr[5];
	int half_width=m_nWidth/2;
	bool *pixels=new bool[half_width];
	//选择五条象素取样行线，用来确定边缘线
	for(i=0;i<5;i++)
	{
		int offset=0;
		while(1)
		{
			sampleLineArr[i].m_iPosIndex=(int)((3+i)*m_nHeight/10);
			for(j=0;j<half_width;j++)
			{
				pixels[j]=IsBlackPixel(j,sampleLineArr[i].m_iPosIndex);
			}
			if(sampleLineArr[i].AnalyzeLinePixels(pixels,half_width))
				break;
			else
			{
				offset++;
				if(offset%2>0)	//奇数为负向移位
					sampleLineArr[i].m_iPosIndex-=offset/2+1;
				else			//偶数为正向移位
					sampleLineArr[i].m_iPosIndex+=offset/2;
				if(offset>0.1*m_nHeight)
					break;
			}
		}
	}
	delete []pixels;
	POINT leftUp,leftDown,rightUp,rightDown;
	leftUp.x=leftUp.y=leftDown.x=leftDown.y=0;
	double deviation_coef=tan(10*RADTODEG_COEF);
	for(i=0;i<sampleLineArr[0].m_nValidBlackSeg;i++)
	{
		int seg_start,seg_end;
		int mid=(sampleLineArr[0].shiftSegArr[i][0]+sampleLineArr[0].shiftSegArr[i][1])/2;
		leftUp.x=mid;
		for(j=1;j<5;j++)
		{
			int height=sampleLineArr[j].m_iPosIndex-sampleLineArr[j-1].m_iPosIndex;
			int deviation=(int)(height*deviation_coef);
			if(!sampleLineArr[j].FindBlackSegInSpecScope(mid-deviation,mid+deviation,&seg_start,&seg_end))
				break;	//没找到合适的黑色线段
			mid=(seg_start+seg_end)/2;
		}
		if(j==5)
		{
			leftDown.x=mid;
			break;
		}
	}
	if(i==sampleLineArr[0].m_nValidBlackSeg)
		return false;
	leftUp.y=sampleLineArr[0].m_iPosIndex;
	leftDown.y=sampleLineArr[4].m_iPosIndex;
	bool bBreakLoop=false;
	double fSlope = 1;
	if((leftUp.x-leftDown.x)!=0)
		fSlope = (leftUp.y - leftDown.y)/(leftUp.x-leftDown.x);
	else
		fSlope = 0;
	int gaplen;
	//查找左上顶点
	for(i=1;i<leftUp.y;i++)
	{
		if(IsBlackPixel(leftUp.x,leftUp.y-i))
			continue;
		else if(leftUp.x-1>0&&IsBlackPixel(leftUp.x-1,leftUp.y-i))
			leftUp.x--;
		else if(leftUp.x+1<m_nWidth&&IsBlackPixel(leftUp.x+1,leftUp.y-i))
			leftUp.x++;
		else if(GapDetect(leftUp.x,leftUp.y-i,-5,false,&gaplen))
			i+=gaplen-1;
		else if(leftUp.x-1>0&&GapDetect(leftUp.x-1,leftUp.y-i,-5,false,&gaplen))
		{
			i+=gaplen-1;
			leftUp.x--;
		}
		else if(leftUp.x+1<m_nWidth&&GapDetect(leftUp.x+1,leftUp.y-i,-5,false,&gaplen))
		{
			i+=gaplen-1;
			leftUp.x++;
		}
		else
		{
			leftUp.y -= i-1;
			bBreakLoop=true;
			break;
		}
	}
	if(!bBreakLoop)
		leftUp.y=0;
	//左下顶点
	bBreakLoop=false;
	for(i=1;i<m_nHeight-leftDown.y;i++)
	{
		if(IsBlackPixel(leftDown.x,leftDown.y+i))
			continue;
		else if(leftDown.x-1>0&&IsBlackPixel(leftDown.x-1,leftDown.y+i))
			leftDown.x--;
		else if(leftDown.x+1<m_nWidth&&IsBlackPixel(leftDown.x+1,leftDown.y+i))
			leftDown.x++;
		else if(GapDetect(leftDown.x,leftDown.y+i,5,false,&gaplen))
			i+=gaplen-1;
		else if(leftDown.x-1>0&&GapDetect(leftDown.x-1,leftDown.y+i,5,false,&gaplen))
		{
			i+=gaplen-1;
			leftDown.x--;
		}
		else if(leftDown.x+1<m_nWidth&&GapDetect(leftDown.x+1,leftDown.y+i,5,false,&gaplen))
		{
			i+=gaplen-1;
			leftDown.x++;
		}
		else
		{
			leftDown.y += i-1;
			bBreakLoop=true;
			break;
		}
	}
	if(!bBreakLoop)
		leftDown.y=m_nHeight-1;
	//查找右上顶点
	rightUp = leftUp;
	bBreakLoop=false;
	for(i=1;i<m_nWidth;i++)
	{
		if(IsBlackPixel(rightUp.x+i,rightUp.y))
			continue;
		else if(rightUp.y-1>0&&IsBlackPixel(rightUp.x+i,rightUp.y-1))
			rightUp.y--;
		else if(rightUp.y+1<m_nHeight&&IsBlackPixel(rightUp.x+i,rightUp.y+1))
			rightUp.y++;
		else if(GapDetect(rightUp.x+1,rightUp.y,5,true,&gaplen))
			i+=gaplen-1;
		else if(leftDown.x-1>0&&GapDetect(rightUp.x+i,rightUp.y-1,5,true,&gaplen))
		{
			i+=gaplen-1;
			rightUp.y--;
		}
		else if(leftDown.x+1<m_nWidth&&GapDetect(rightUp.x+i,rightUp.y+1,5,true,&gaplen))
		{
			i+=gaplen-1;
			rightUp.y++;
		}
		else
		{
			rightUp.x += i-1;
			bBreakLoop=true;
			break;
		}
	}
	if(!bBreakLoop)
		rightUp.x=m_nWidth;
	//查找右下定点
	rightDown = leftDown;
	bBreakLoop=false;
	for(i=1;i<m_nWidth;i++)
	{
		if(IsBlackPixel(rightDown.x+i,rightDown.y))
			continue;
		else if(rightDown.y-1>0&&IsBlackPixel(rightDown.x+i,rightDown.y-1))
			rightDown.y--;
		else if(rightDown.y+1<m_nHeight&&IsBlackPixel(rightDown.x+i,rightDown.y+1))
			rightDown.y++;
		else if(GapDetect(rightDown.x+1,rightDown.y+1,5,true,&gaplen))
			i+=gaplen-1;
		else if(leftDown.x-1>0&&GapDetect(rightDown.x+i,rightDown.y+1,5,true,&gaplen))
		{
			i+=gaplen-1;
			rightDown.y--;
		}
		else if(leftDown.x+1<m_nWidth&&GapDetect(rightDown.x+i,rightDown.y+1,5,true,&gaplen))
		{
			i+=gaplen-1;
			rightDown.y++;
		}
		else
		{
			rightDown.x += i-1;
			bBreakLoop=true;
			break;
		}
	}
	if(!bBreakLoop)
		rightDown.x=m_nWidth;
	//
	m_leftUp=leftUp;
	m_leftDown=leftDown;
	m_rightUp=rightUp;
	m_rightDown=rightDown;
	return true;
}
void CImageTransform::LocateDrawingRgnRect()
{
	m_nDrawingRgnWidth=(m_rightUp.x+m_rightDown.x-m_leftUp.x-m_leftDown.x)/2;
	m_nDrawingRgnHeight=(m_leftDown.y+m_rightDown.y-m_leftUp.y-m_rightUp.y)/2;
	kesai[0]=(double)(m_rightUp.x-m_leftUp.x)/m_nDrawingRgnWidth;
	kesai[1]=(double)(m_rightDown.x-m_leftDown.x)/m_nDrawingRgnWidth;
	kesai[2]=(double)(m_leftDown.y-m_leftUp.y)/m_nDrawingRgnHeight;
	kesai[3]=(double)(m_rightDown.y-m_rightUp.y)/m_nDrawingRgnHeight;
}
void CImageTransform::LocateDrawingRgnRect(CPoint leftU,CPoint leftD,CPoint rightU,CPoint rightD)
{
	m_leftUp.x=leftU.x;
	m_leftUp.y=leftU.y;
	m_leftDown.x=leftD.x;
	m_leftDown.y=leftD.y;
	m_rightUp.x=rightU.x;
	m_rightUp.y=rightU.y;
	m_rightDown.x=rightD.x;
	m_rightDown.y=rightD.y;
	m_nDrawingRgnWidth=(m_rightUp.x+m_rightDown.x-m_leftUp.x-m_leftDown.x)/2;
	m_nDrawingRgnHeight=(m_leftDown.y+m_rightDown.y-m_leftUp.y-m_rightUp.y)/2;
	kesai[0]=(double)(m_rightUp.x-m_leftUp.x)/m_nDrawingRgnWidth;
	kesai[1]=(double)(m_rightDown.x-m_leftDown.x)/m_nDrawingRgnWidth;
	kesai[2]=(double)(m_leftDown.y-m_leftUp.y)/m_nDrawingRgnHeight;
	kesai[3]=(double)(m_rightDown.y-m_rightUp.y)/m_nDrawingRgnHeight;
}
int CImageTransform::StretchBlt(HDC hDC,CRect imageShowRect,DWORD rop)
{
	return ::StretchDIBits( hDC,imageShowRect.left,imageShowRect.top,imageShowRect.Width(),imageShowRect.Height(),
		0,0,GetWidth(),GetHeight(),GetBits(),(LPBITMAPINFO)GetBitmapInfoHeader(),DIB_RGB_COLORS,rop);
}
#endif