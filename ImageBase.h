#pragma once

#include <vector>

using namespace std;

class CImageBase
{
private:
	int m_nWidth;
	int m_nHeight;
	
	BYTE ** m_Data;
	BYTE * m_pMem;

public:
	CImageBase(void);
	~CImageBase(void);
	bool Load(CString strFileName);
	bool Clear(void);
	bool Display(CDC*  hDC);
	bool ModelFrom(CImageBase* that);
	bool ModalTo(CImageBase* that);

	inline int GetWidth()
	{
		return m_nWidth;
	}

	inline int GetHeight()
	{
		return m_nHeight;
	}

	inline COLORREF GetPixel(int x, int y)
	{
		return RGB(m_Data[y][3*x + 2],m_Data[y][3*x + 1],m_Data[y][3*x]);
	}

	inline BYTE GetPixelByRed(int x,int y)
	{
		return m_Data[y][3*x + 2];
	}

	inline BYTE GetPixelByGreen(int x,int y)
	{
		return m_Data[y][3*x + 1];
	}

	inline BYTE GetPixelByBlue(int x,int y)
	{
		return m_Data[y][3*x];
	}

	inline BYTE GetPixelEx(int x,int y)
	{
		return m_Data[y][3*x];
	}

	inline void SetPixel(int x,int y,COLORREF color)
	{
		m_Data[y][3*x + 2] = GetRValue(color);
		m_Data[y][3*x + 1] = GetGValue(color);
		m_Data[y][3*x] = GetBValue(color);
	}

	inline void SetGray(int x,int y,BYTE color)
	{
		m_Data[y][3*x + 2] = color;
		m_Data[y][3*x + 1] = color;
		m_Data[y][3*x] = color;
	}

	inline BYTE GetGray(int x,int y)
	{
		double ff = 0.30 * GetPixelByRed(x,y) + 0.59 * GetPixelByGreen(x,y) + 0.11 * GetPixelByBlue(x,y);

		return (BYTE)(ff + 0.5);
	}

	bool InitPixel(COLORREF color);
	bool InitPixel(BYTE color);
	bool Gray(void);
	bool Resize(int width, int higth);
	bool Part(CRect rect);
	bool GetHist(double* re);
	bool LinarTrans(double dFa, double dFb);
	bool Reverse(void);
	bool LogTrans(double dTimes);
	bool LogTransStd(void);
	bool ExpTrans(double dEsp, double dR);
	bool Threshold(double dColor);
	bool ParLineTrans(double dX1, double dY1, double dX2, double dY2);
	bool CurveTrans(BYTE * dTrans);
	bool HistEq(void);
	bool HistSt(double* dHist);
	bool GetRealRect(CRect* re,BYTE std);
	bool StretchNearest(CRect rctSrc, CRect rctDes);
	bool StretchBilinar(CRect rctSrc, CRect rctDes);
	bool Template(int nTempW, int nTempH, int nTempMX, int nTempMY, float* pfTemp, float fCoef);
	bool SEAvg(void);
	bool SEGuassSmooth(void);
	bool SEGuassSmoothBig(void);
	bool SEMedianFilter(void);
	bool SEOrderFilter(int Width, int Height,int nMidWidth ,int nMidHeight, int nOrder);
	bool SELargestFilter(void);
	bool SESmallestFilter(void);
	bool Add(CImageBase* src1, CImageBase* src2);
	bool SESobel(void);
	bool SELog(void);
	bool SELaplaceCross(void);
	bool SELaplaceSquar(void);
	bool SERobert(void);
	bool SEGuassLaplace(void);
	bool SEPrewitt(void);
	bool FEFilter(double* dFilter, BYTE btFill);

	inline long GetFreqWidth(bool bExtended = true)
	{
		long w = 1;

		while( 2 * w <= m_nWidth)
		{
			w *= 2;
		}

		if ((w != m_nWidth) && (bExtended))
		{
			w *= 2;
		}

		return w;
	}

	long GetFreqHeight(bool bExtended = true)
	{
		long h = 1;

		while( 2 * h <= m_nHeight)
		{
			h *= 2;
		}

		if ((h != m_nHeight) && bExtended)
		{
			h *= 2;
		}

		return h;
	}

	bool FEGetIdealLPF(double** dFilter, int nR);
	bool FEIdealLPF(int nR,BYTE btFill = 255);
	bool FEGetGaussLPF(double** dFilter, double dSigma);
	bool FEGaussLPF(double dSigma,BYTE btFill);

	bool FEGetIdealHPF(double** dFilter, int nR);
	bool FEIdealHPF(int nR,BYTE btFill = 255);
	bool FEGetGaussHPF(double** dFilter, double dSigma);
	bool FEGaussHPF(double dSigma,BYTE btFill);

	bool FEGetLaplace(double** dFilter);
	bool FELaplace(BYTE btFill);

	bool Erode(int se[3][3]);
	bool ErodeCross(void);
	bool ErodeSquar(void);

	bool Dilate(int se[3][3]);
	bool DilateCross(void);
	bool DilateSquar(void);

	bool Open(int se[3][3]);
	bool OpenCross(void);
	bool OpenSquar(void);

	bool Close(int se[3][3]);
	bool CloseCross(void);
	bool CloseSquar(void);

	bool ReduceEdge(int dWidth = 1,BYTE btFill=255);
	bool TraceBoundary(void);
	bool FillRgnM(CPoint ptStart);

	bool BitReverse(void);
	bool Equal(CImageBase* that);

	bool And(CImageBase* that);
	bool Or(CImageBase* that);
	
	bool GetConnectionRgnM(int nMin, int nMax,bool bFourConnn,int *nConRgn);
	bool Thining();
	bool GetConnectionRgn(int nMin, int nMax,bool b);
	int GetConnectionRgn_R(int i,int j,int nCon,bool bfc);
	bool GetConnectionRgn_SetBackground(vector<int> &vct);
	bool GetConnectionRgn_SetTouch(bool b);

	bool Convex(bool b);

	bool DilateGray(int se[3][3]);
	bool ErodeGray(int se[3][3]);
	bool OpenGray(int se[3][3]);
	bool CloseGray(int se[3][3]);
	bool TopHat(int se[3][3]);

	bool Sub(CImageBase *that);
	
	bool DCT();
	bool Walsh_Har();

	bool InverseFilter(double *dTemp,double coef,int xLen,int yLen);
	bool InverseFilterAvg();

	bool WienerFilter();
	bool MinDBFilter();
	bool MaxProFilter();
	bool MaxEntrFilter();
	bool GeoFilter();
	bool MotionFilter();

	bool Max(CImageBase *that);
	bool Min(CImageBase *that);

	bool SEKrisch();

	//一切阈值分割功能。
	//一切实训得到的经验功能。
};
