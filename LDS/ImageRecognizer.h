#pragma once

#include "list.h"
#include "f_ent_list.h"	//�Ժ�Ӧ��CXhSimpleList��ȫȡ��
#include "HashTable.h"
#include "Buffer.h"
#include "XhCharString.h"
#include "ImageTransform.h"
#include "MemberProperty.h"
#include ".\BOM\BOM.h"

#if defined(__TMA_)||defined(__LMA_)
union XHWCHAR{
	WORD wHz;
	char chars[2];	//����ĸ���»���'_'��Ϊ��д����ʱ��Label()��������ƴд�� wjh-2014.8.19
public:
	XHWCHAR(){wHz=0;}
	operator WORD(){return wHz;}
	operator char(){return chars[0];}
};
static const BYTE DETECT_LEFT	= 0x01;	//X��С����
static const BYTE DETECT_RIGHT	= 0x02;	//X���ӷ���
static const BYTE DETECT_UP		= 0x04;	//Y��С����
static const BYTE DETECT_DOWN	= 0x08;	//Y���ӷ���
static const BYTE DETECT_X0		= 0x10;	//X���ֲ���
static const BYTE DETECT_Y0		= 0x20;	//Y���ֲ���
struct IImage{
	static const BYTE PIXEL_IS_BYTE = 0;//ÿ������ռ1��Byte
	static const BYTE PIXEL_IS_BIT  = 1;//ÿ������ռ1��Bit
protected:
	BYTE* m_lpBitMap;	//new BYTE[m_nEffWidth*m_nHeight]
	int m_nEffWidth;	//ʵ����Ч���
	int m_nWidth,m_nHeight;
public:
	virtual const BYTE* GetImageMap(){return m_lpBitMap;}
	virtual bool IsPixelBitMode()=0;	//True:ÿ������ռһ���е�һ��bitλ;falseռһ��Byte�ֽ�
	virtual bool BlackIsTrue(){return true;}
	virtual bool IsBlackPixel(int i,int j);
	virtual bool SetPixelState(int i,int j,bool black=true);
	virtual int GetImageEffWidth(){return m_nEffWidth;}	//��ȡͼ��ÿ��������ռ�õ�ʵ���ֽ���
	virtual int GetImageWidth(){return m_nWidth;}		//��ȡͼ��ÿ�е���Ч������
	virtual int GetImageHeight(){return m_nHeight;}		//��ȡͼ��ÿ�е���Ч������
};
class DETECT_MAP{
	bool* map;
	int wDetectPixels;
public:
	int xiOrg,yiOrg;
	DETECT_MAP(WORD wDetectWidth,int iOrgX=0,int iOrgY=0)
	{
		wDetectPixels=wDetectWidth;
		int width=wDetectPixels*2+1;
		map=new bool[width*width];
		memset(map,0,sizeof(bool)*width*width);
		xiOrg=iOrgX;
		yiOrg=iOrgY;
	}
	~DETECT_MAP(){delete []map;}
	bool IsDetect(int xi,int yi)
	{
		int width=wDetectPixels*2+1;
		xi+=wDetectPixels-xiOrg;
		yi+=wDetectPixels-yiOrg;
		if(xi<0||xi>=width||yi<0||yi>=width)
			return true;
		else
			return map[yi*width+xi];
	}
	bool SetDetect(int xi,int yi,bool detect=true)
	{
		int width=wDetectPixels*2+1;
		xi+=wDetectPixels-xiOrg;
		yi+=wDetectPixels-yiOrg;
		if(xi<0||xi>=width||yi<0||yi>=width)
			return false;
		else
			return map[yi*width+xi]=detect;
	}
};
struct IImageNoiseDetect : public IImage{
//	static const BYTE DETECT_LEFT	= 0x01;
//	static const BYTE DETECT_RIGHT	= 0x02;
//	static const BYTE DETECT_UP		= 0x04;
//	static const BYTE DETECT_DOWN	= 0x08;
//public:
	virtual bool IsNoisePixel(int i,int j,int minBlackPixels=5,BYTE cDetectFlag=0x0f);
protected:
	virtual int StatNearBlackPixels(int i,int j,DETECT_MAP* pDetectMap,int maxBlackPixels=5,BYTE cDetectFlag=0x0f);
};
struct CBitImage : public IImageNoiseDetect{
	CBitImage(BYTE* lpBitMap=NULL,int width=0,int height=0);
	~CBitImage();
	virtual void InitBitImage(BYTE* lpBitMap=NULL,int width=0,int height=0);
	virtual bool BlackBitIsTrue(){return m_bBlackPixelIsTrue;}
	virtual bool IsPixelBitMode(){return true;}	//True:ÿ������ռһ���е�һ��bitλ;falseռһ��Byte�ֽ�
	virtual bool IsBlackPixel(int i,int j);
	virtual void SetClipRect(const RECT& rc){clip=rc;}
protected:
	RECT clip;	//��Ч�Ĳü�����
	bool m_bBlackPixelIsTrue;	//���ر���λΪTrue��ʾ�ڵ�
	bool m_bExternalData;
};
struct IImageZoomTrans{
protected:
	double minCoefDest2Src;
	double xCoefDest2Src,yCoefDest2Src,xOffsetOrg,yOffsetOrg;
	RECT _src,_dest;
public:
	virtual bool InitImageZoomTransDataByCenter(const RECT& src,const RECT& dest);
	virtual bool InitImageZoomTransData(const RECT& src,const RECT& dest);
	virtual POINT TransFromDestImage(int xiDest,int yjDest);
	virtual bool IsBlackPixelAtDest(int xiDest,int yjDest,IImage* pSrcImage);
};

struct MATCHCOEF{
	double original;	//����ǰԭʼƥ��ϵ��
	double weighting;	//������ƥ��ϵ��
	MATCHCOEF(double originalCoef=0.0,double weightingCoef=0.0);
	int CompareWith(const MATCHCOEF& coef);
};
class CImageChar : public IImageZoomTrans, public IImage
{
public:
	struct ISLAND{
		//bool boundary;
		double x,y,count;
		ISLAND(){Clear();}
		void Clear(){x=y=count=0;}//boundary=false;}
	};	//�ڲ��հ׹µ���������
	int DetectIslands(CXhSimpleList<ISLAND>* listIslands);
protected:
	int m_nPixels;
	//BYTE* m_lpBitMap;	//��m_nPixels����ÿ���ֽڱ�ʾ1������, 0��ʾ�׵�;1��ʾ�ڵ�
	int _nDestValidWidth;	//����ת����Ŀ��ͼ������Ч���
	bool IsDetect(int i,int j);
	bool SetDetect(int xi,int yi,bool detect=true);
	void ClearDetectMark();
	bool StatNearWhitePixels(int i,int j,ISLAND* pIsland,BYTE cDetectFlag=0x0f,CLogFile* pLogFile=NULL);
public:
	bool m_bTemplEnabled;
	XHWCHAR wChar;
	int m_nBlckPixels;
	//RECT rect;	//��ʼ��ʱԭͼ���еĽ�ȡ����(����rect.right��rect.bottom��)
	//int m_nMaxTextH;
	READONLY_PROPERTY(int,m_nMaxTextH);
	GET(m_nMaxTextH){return m_nHeight;}
public:
	READONLY_PROPERTY(int,m_nImageW);
	GET(m_nImageW){return m_nWidth;}
	READONLY_PROPERTY(int,m_nImageH);
	GET(m_nImageH){return m_nHeight;}
	READONLY_PROPERTY(int,m_nDestValidWidth);
	GET(m_nDestValidWidth){return _nDestValidWidth;}
public:
	CImageChar();
	~CImageChar(); 
	virtual bool IsPixelBitMode(){return false;}	//True:ÿ������ռһ���е�һ��bitλ;falseռһ��Byte�ֽ�
	virtual bool IsBlackPixel(int i,int j);
	void CopyImageChar(CImageChar* pChar);
	void InitTemplate(BYTE* lpBits,int eff_width,RECT rect);
	void Localize(BYTE* lpBits,int eff_width,RECT rect);
	void Standardize(WORD wStdWidth,WORD wStdHeight);
	void StandardLocalize(BYTE* lpBits,int eff_width,const RECT& rc,int wStdHeight);
	int CheckFeatures(CXhSimpleList<ISLAND>* pIslands=NULL);	//-1��ȷ��������������0.����ȷ���Ƿ����������1.ȷ����������
	MATCHCOEF CalMatchCoef(CImageChar* pText,char* pciOffsetX=NULL,char* pciOffsetY=NULL);
	void ToBuffer(CBuffer &buffer);
	void FromBuffer(CBuffer &buffer,long version);
};
//////////////////////////////////////////////////////////////////////////
// CImageDataRegion
class CAlphabet;
class CImageCellRegion : public CBitImage
{
public:
	static const int PARTNO		= 1;
	static const int GUIGE		= 2;
	static const int LENGHT		= 3;
	static const int NUM		= 4;
	static const int PIECE_W	= 5;
	static const int SUM_W		= 6;
	static const int NOTE		= 7;
private:
	int m_nIterDepth;
	ATOM_LIST<CImageChar> listChars;
	CAlphabet* m_pModel;
public:
	int m_nDataType;
	CImageCellRegion();
	~CImageCellRegion();
	//
	void SetBelongModel(CAlphabet *pDataModel){m_pModel=pDataModel;}
	CAlphabet* BelongModel() const{return m_pModel;}
	CXhChar50 GetUnitDataValue();
	virtual bool IsPixelBitMode(){return true;}	//True:ÿ������ռһ���е�һ��bitλ;falseռһ��Byte�ֽ�
	//virtual bool IsBlackPixel(int i,int j);
	BOOL IsSplitter(WORD movewnd[5]);
	int IsMatchChar(WORD movewnd[5],WORD iBitStart,WORD iSplitStart,WORD iSplitEnd,WORD& wChar);
	void SplitImageCharSet(WORD iBitStart=0,WORD wGuessCharWidth=0,bool bIgnoreRepeatFlag=false);
#ifdef _DEBUG
	void PrintCharMark();
#endif
};
class CImageDataFile;
class CImageDataRegion : public IImageNoiseDetect
{
public:
	struct PIXELLINE
	{
		int iStart;
		int nPixell;
		PIXELLINE(){iStart=nPixell=0;}
	};
private:
	CImageDataFile *m_pImageData;
	virtual bool IsPixelBitMode(){return true;}	//True:ÿ������ռһ���е�һ��bitλ;falseռһ��Byte�ֽ�
	virtual bool IsBlackPixel(int i,int j){return IsBlackPixel(i,j,NULL,NULL);}
	bool IsBlackPixel(int i,int j,BYTE* lpBits,WORD wBitWidth);

	void CalCharValidRect(RECT data_rect,int& xStart,int& yStart,int& xEnd,int& yEnd);
	int CalMaxPixelsIndex(int start,int end,BOOL bLevel);
	BOOL AnalyzeLinePixels(int& start,int& end,BOOL bLevel);
	void RecognizeLines(ATOM_LIST<int> &lineStartList,BOOL bLevel);
	void RetrieveCellRgn(CImageCellRegion& cell,RECT rect);
public:
	DWORD m_dwKey;
	BYTE* m_lpBits;
	int m_nEffWidth,m_nDataWidth,m_nDataHeight;
	CPoint leftUp,leftDown,rightUp,rightDown;
	double m_fDisplayAllRatio;		//��¼ͼ��ȫ�Ա�����������ʾʱ֧���϶�
	double m_fDisplayRatio;			//��¼ͼ������ϵ��������ͼƬʱʹ��
	int m_nOffsetX,m_nOffsetY;		//��¼ͼƬƫ��λ�ã�����ͼƬʱʹ��
	CSuperHashList<BOMPART> m_hashPartByIndex;
public:
	CImageDataRegion();
	~CImageDataRegion();
	void SetKey(DWORD key){m_dwKey=key;}
	DWORD GetKey(){return m_dwKey;}
	void TransPickPtToOrigPt(int sumHeight);
	BOOL RetrieveImageData();
	CXhChar50 RecognizeDatas(RECT data_rect,int data_type);
	void SetBelongImageData(CImageDataFile *pImageData){m_pImageData=pImageData;}
	CImageDataFile* BelongImageData() const{return m_pImageData;}
	void ParseDataValue(CXhChar50 sText,int data_type,char* sValue1,char* sValue2=NULL,int cls_id=0);
	void ToBuffer(CBuffer &buffer);
	void FromBuffer(CBuffer &buffer,long version);
};
class CImageDataFile
{
private:
	CAlphabet* m_pModel;
	double m_fDisplayAllRatio;		//��¼ͼ��ȫ�Ա�����������ʾʱ֧���϶�
	double m_fDisplayRatio;			//��¼ͼ������ϵ��������ͼƬʱʹ��
	int m_nOffsetX,m_nOffsetY;		//��¼ͼƬƫ��λ�ã�����ͼƬʱʹ��
public:
	char file_path[MAX_PATH];		//�ļ�·��
	CXhChar100 m_sName;
	CImageTransform image_trans;
	CHashListEx<CImageDataRegion> m_hashRegionById;
public:
	CImageDataFile();
	~CImageDataFile();
	//
	bool ReadImageData(const char* file_name);
	void InitDataBits(CImageDataRegion *pRegion);
	void SetBelongModel(CAlphabet *pDataModel){m_pModel=pDataModel;}
	CAlphabet* BelongModel() const{return m_pModel;}
	int StretchBlt(HDC hDC,DWORD rop,CImageDataRegion *pRegion=NULL);
	//
	void InitImageShowPara(RECT showRect,DWORD dwRegionKey=0);
	CSize GetOffsetSize(DWORD dwRegionKey=0);
	void SetOffsetSize(CSize offset,DWORD dwRegionKey=0);
	double GetDisplayRatio(DWORD dwRegionKey=0);
	BOOL IsCanZoom(DWORD dwRegionKey=0);
	double Zoom(short zDelta,DWORD dwRegionKey=0);
	RECT GetImageDisplayRect(DWORD dwRegionKey=0);
	void ToBuffer(CBuffer &buffer);
	void FromBuffer(CBuffer &buffer,long version);
};
class CAlphabetFont
{
public:
	int serialNo;		//ģ�����к�
	CXhChar16 m_sName;	//ģ������
	int m_nTemplateH;	//ģ������߶�
	double m_fHtoW;		//�߿��
	ATOM_LIST<CImageChar> listChars;	//ģ���ַ��б�
	CImageChar *pCharX,*pCharL,*pCharQ,*pCharPoint;
public:
	CAlphabetFont();
	void SetKey(DWORD key){serialNo=key;}
	void InitFromFolder(const char* folder_path);
	void ToBuffer(CBuffer &buffer);
	void FromBuffer(CBuffer &buffer,long version);
};
class CAlphabet
{
	CHashListEx<CAlphabetFont> listFonts;
	CAlphabetFont* m_pActiveFont;
public:
	struct MATCHCHAR
	{
		XHWCHAR wChar;
		CImageChar* pTemplChar;
		char ciOffsetX,ciOffsetY;
		MATCHCOEF matchcoef;
		MATCHCHAR(){ciOffsetX=ciOffsetY=0;pTemplChar=NULL;}
	};
public:
	CAlphabet();
	CAlphabetFont *ActiveFont();
	CAlphabetFont *EnumFirstFont(){return listFonts.GetFirst();}
	CAlphabetFont *EnumNextFont(){return listFonts.GetNext();}
	void ToBuffer(CBuffer &buffer);
	void FromBuffer(CBuffer &buffer,long version);
	void LoadFromFile(const char* file_name);
	void SaveToFile(const char* file_name);
	//��ʼ������
	void InitBrainData(const char* path);
	int GetTemplateH(int template_serial);
	BOOL RecognizeData(CImageChar* pText,double minMatchCoef=0.7,MATCHCOEF* pMachCoef=NULL);
};
#endif