#include <StdAfx.h>

#include <Windows.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <complex>

#include "ImageBase.h"
#include "MathBase.h"

using namespace std;

#define WIDTHBYTES(bits) (((bits) + 31) / 32 * 4)

CImageBase::CImageBase(void)
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_Data = NULL;
}

CImageBase::~CImageBase(void)
{
	Clear();
}

bool CImageBase::Load(CString strFileName)
{
	if (!Clear())
	{
		return false;
	}

	CFile flContext;
	if (!flContext.Open(strFileName,CFile::modeRead | CFile::shareDenyWrite))
	{
		return false;
	}

	BITMAPFILEHEADER bmfHeader;

	if (!flContext.Read(&bmfHeader,sizeof(BITMAPFILEHEADER)))
	{
		return false;
	}

	if (bmfHeader.bfType != MAKEWORD('B','M'))
	{
		return false;
	}

	BITMAPINFOHEADER bmiHeader;

	if (!flContext.Read(&bmiHeader,sizeof(BITMAPINFOHEADER)))
	{
		return false;
	}

	m_nWidth = bmiHeader.biWidth;
	m_nHeight = bmiHeader.biHeight;

	int nColorTableEntries = (bmfHeader.bfOffBits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER)) / sizeof(RGBQUAD);

	RGBQUAD *dwColor = NULL;
	if (nColorTableEntries > 0)
	{
		dwColor = new RGBQUAD[nColorTableEntries];

		if (!flContext.Read(dwColor,sizeof(RGBQUAD) * nColorTableEntries))
		{
			delete dwColor;
			return false;
		}
	}

	m_pMem = new BYTE[m_nHeight * m_nWidth * 3];
	m_Data = new BYTE*[m_nHeight];

	BYTE *pTempMem = m_pMem;

	if (bmiHeader.biBitCount == 24)
	{
		for(int i=0;i<m_nHeight;i++)
		{
			m_Data[i] = pTempMem;
			pTempMem += m_nWidth * 3;
		}

		for(int i=m_nHeight - 1;i>=0;i--)
		{
			flContext.Read(m_Data[i],WIDTHBYTES(m_nWidth * bmiHeader.biBitCount));
		}
	}
	else if (bmiHeader.biBitCount == 8)
	{
		if (nColorTableEntries != 256)
		{
			Clear();
			return false;
		}

		BYTE *btTemp = new BYTE[WIDTHBYTES(m_nWidth * bmiHeader.biBitCount)];

		for(int i=0;i<m_nHeight;i++)
		{
			m_Data[i] = pTempMem;
			pTempMem += m_nWidth * 3;
		}

		for(int i=m_nHeight - 1;i>=0;i--)
		{
			flContext.Read(btTemp,WIDTHBYTES(m_nWidth * bmiHeader.biBitCount));

			for(int j=0;j<m_nWidth;j++)
			{
				m_Data[i][3*j] = dwColor[btTemp[j]].rgbBlue;
				m_Data[i][3*j + 1] = dwColor[btTemp[j]].rgbGreen;
				m_Data[i][3*j + 2] = dwColor[btTemp[j]].rgbRed;
			}
		}

		delete btTemp;
	}

	delete dwColor;

	return true;
}

bool CImageBase::Clear(void)
{
	if (m_Data == NULL)
		return true;

	delete m_pMem;
	delete m_Data;

	m_nWidth = 0;
	m_nHeight = 0;
	m_Data = NULL;

	return true;
}

bool CImageBase::Display(CDC* pDC)
{
	BITMAPINFOHEADER bmiHeader;
	bmiHeader.biBitCount = 24;
	bmiHeader.biClrImportant = 0;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biCompression = 0;
	bmiHeader.biHeight = m_nHeight;
	bmiHeader.biWidth = m_nWidth;
	bmiHeader.biPlanes = 1;
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiHeader.biSizeImage = m_nWidth * m_nHeight * 3;
	bmiHeader.biXPelsPerMeter = 3780;
	bmiHeader.biYPelsPerMeter = 3780;

	for(int i=0;i<m_nHeight;i++)
	{
		SetDIBitsToDevice(pDC->m_hDC,0,0,m_nWidth,m_nHeight,0,0,m_nHeight - i,1,m_Data[i],(BITMAPINFO*)&bmiHeader,DIB_RGB_COLORS);
	}
	
	return true;
}

bool CImageBase::ModelFrom(CImageBase* that)
{
	Clear();

	m_nHeight = that->m_nHeight;
	m_nWidth = that->m_nWidth;

	m_Data = new BYTE*[m_nHeight];
	m_pMem = new BYTE[m_nWidth * m_nHeight * 3];
	memcpy(m_pMem,that->m_pMem,m_nHeight * m_nWidth * 3);

	BYTE *btTemp = m_pMem;

	for(int i=0;i<m_nHeight;i++)
	{
		m_Data[i] = btTemp;
		btTemp += m_nWidth * 3;
	}

	return true;
}

bool CImageBase::ModalTo(CImageBase* that)
{
	return that->ModelFrom(this);
}

bool CImageBase::InitPixel(COLORREF color)
{
	for(int i=0;i<m_nHeight;i++)
	{
		for(int j=0;j<m_nWidth;j++)
		{
			SetPixel(i,j,color);
		}
	}

	return true;
}

bool CImageBase::InitPixel(BYTE color)
{
	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			SetPixel(i,j,color);
		}
	}

	return true;
}

bool CImageBase::Gray(void)
{
	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			SetGray(i,j,GetGray(i,j));
		}
	}

	return true;
}

bool CImageBase::Resize(int width, int higth)
{
	Clear();

	m_nWidth = width;
	m_nHeight = higth;

	m_pMem = new BYTE[m_nWidth * m_nHeight * 3];
	m_Data = new BYTE*[m_nHeight];

	BYTE *btTemp = m_pMem;

	for(int i=0;i<m_nHeight;i++)
	{
		m_Data[i] = btTemp;
		btTemp += m_nWidth * 3;
	}

	return true;
}

bool CImageBase::Part(CRect rect)
{
	CImageBase ibPre;
	ibPre.ModelFrom(this);

	if (!Resize(rect.Width(),rect.Height()))
		return false;

	for(int i=0;i<rect.Width();i++)
	{
		for(int j=0;j<rect.Height();j++)
		{
			SetPixel(i,j,ibPre.GetPixel(i + rect.left,j + rect.top));
		}
	}

	return true;
}

bool CImageBase::GetHist(double* re)
{
	for(int i=0;i<256;i++)
	{
		re[i] = 0.0;
	}

	for(int i=0;i<m_nHeight;i++)
	{
		for(int j=0;j<m_nWidth;j++)
		{
			re[GetGray(j,i)] ++;
		}
	}

	for(int i=0;i<256;i++)
	{
		re[i] = re[i] / (m_nWidth * m_nHeight);
	}

	return true;
}

bool CImageBase::LinarTrans(double dFa, double dFb)
{
	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			BYTE btRe;
			double dRe;
			
			dRe = GetGray(i,j) * dFa + dFb;

			if (dRe >= 255)
			{
				btRe = 255;
			}
			else if (dRe < 0)
			{
				btRe = 0;
			}
			else
			{
				btRe = (BYTE)(dRe + 0.5);
			}

			SetGray(i,j,btRe);
		}
	}

	return true;
}

bool CImageBase::Reverse(void)
{
	if (LinarTrans(-1,255))
		return true;
	else
		return false;
}

bool CImageBase::LogTrans(double dTimes)
{
	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			BYTE btRe;
			double dRe;

			dRe = dTimes * log(1.0 + GetGray(i,j));

			if (dRe >= 255)
			{
				btRe = 255;
			}
			else if (dRe < 0)
			{
				btRe = 0;
			}
			else 
			{
				btRe = (BYTE)(dRe + 0.5);
			}

			SetGray(i,j,btRe);
		}
	}

	return true;
}

bool CImageBase::LogTransStd(void)
{
	LogTrans(10);

	return true;
}

bool CImageBase::ExpTrans(double dEsp, double dR)
{
	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			BYTE btRe;
			double dRe;

			dRe = pow( GetGray(i,j) + dEsp , dR); 

			if (dRe >= 255)
			{
				btRe = 255;
			}
			else if (dRe < 0)
			{
				btRe = 0;
			}
			else 
			{
				btRe = (BYTE)(dRe + 0.5);
			}

			SetGray(i,j,btRe);
		}
	}

	return true;
}

bool CImageBase::Threshold(double dColor)
{
	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			BYTE btCur = GetGray(i,j);

			if (btCur <= dColor)
			{
				SetGray(i,j,0);
			}
			else
			{
				SetGray(i,j,255);
			}
		}
	}

	return true;
}


bool CImageBase::ParLineTrans(double dX1, double dY1, double dX2, double dY2)
{
	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			BYTE btCur = GetGray(i,j);
			BYTE btRe = 0;

			if (btCur < dX1)
			{
				btRe = (BYTE)(btCur * dY1 / dX1);
			}
			else if (btCur <= dX2 && btCur >= dX1)
			{
				btRe = (BYTE)( (dY2 - dY1) / (dX2 - dX1) * (btCur - dX1) + dY1 + 0.5);
			}
			else
			{
				btRe = (BYTE)( (255 - dY2) / (255 - dX2) * (btCur - dX2) + dY2 + 0.5);
			}

			SetGray(i,j,btRe);
		}
	}

	return true;
}

bool CImageBase::CurveTrans(BYTE * dTrans)
{
	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			SetGray(i,j,dTrans[GetGray(i,j)]);
		}
	}

	return true;
}

bool CImageBase::HistEq(void)
{
	double dHist[256];

	GetHist(dHist);

	BYTE abtTrans[256];

	memset(abtTrans,0,sizeof(BYTE) * 256);

	for(int i=1;i<256;i++)
	{
		dHist[i] = dHist[i] + dHist[i-1];
	}

	for(int i=0;i<256;i++)
	{
		abtTrans[i] = (BYTE)( 255 * dHist[i] + 0.5);
	}

	CurveTrans(abtTrans);

	return true;
}

bool CImageBase::HistSt(double* adHistStd)
{
	int anTrans[256];
	double adHist[256];

	memset(anTrans,-1,sizeof(int) * 256);
	
	GetHist(adHist);

	for(int i=1;i<256;i++)
	{
		adHistStd[i] += adHistStd[i-1];
		adHist[i] += adHist[i-1];
	}

	for(int i=0;i<256;i++)
	{
		anTrans[int(0.49 + 255*adHistStd[i])] = i;
	}

	int cnti = 0,cntj = 0;
	
	while(cnti < 255)
	{
		if (anTrans[cnti+1] != -1)
		{
			cnti ++;
			continue;
		}

		cntj = 1;

		while((anTrans[cnti + cntj]!=-1) && ((cnti+cntj)<=255))
		{
			anTrans[cnti + cntj] = anTrans[cnti];
			cntj++;
		}
	}

	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth ;i++)
		{
			BYTE gray = GetGray(i,j);
			
			BYTE re = anTrans[int(0.49 + adHist[gray])];

			if (re <= 0)
				re = 0;
			else if (re >= 256)
				re = 255;
			
			SetGray(i,j,re);
		}
	}

	return true;
}

bool CImageBase::GetRealRect(CRect* re,BYTE std)
{
	int nLeft = m_nWidth + 1;
	int nRight = -1;
	int nTop = m_nHeight + 1;
	int nBottom = -1;

	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			if (GetGray(i,j) != std)
			{
				if (i < nLeft)
				{
					nLeft = i;
				}
				else if (i > nRight)
				{
					nRight = i;
				}

				if (j < nTop)
				{
					nTop = j;
				}
				else if (j > nBottom)
				{
					nBottom = j;
				}
			}
		}
	}

	return true;
}

bool CImageBase::StretchNearest(CRect rctSrc, CRect rctDes)
{
	CImageBase ibSrc;
	ibSrc.ModelFrom(this);

	if (rctDes.top <= 0)
		rctDes.top = 0;
	if (rctDes.bottom >= m_nHeight)
		rctDes.bottom = m_nHeight -1;
	if (rctDes.left <= 0)
		rctDes.left = 0;
	if (rctDes.right >= m_nWidth)
		rctDes.right = m_nWidth - 1;

	if (rctSrc.top <= 0)
		rctSrc.top = 0;
	if (rctSrc.bottom >= m_nHeight)
		rctSrc.bottom = m_nHeight -1;
	if (rctSrc.left <= 0)
		rctSrc.left = 0;
	if (rctSrc.right >= m_nWidth)
		rctSrc.right = m_nWidth - 1;

	for(int j=rctDes.top;j<rctDes.bottom;j++)
	{
		for(int i=rctDes.left;i<rctDes.right;i++)
		{
			int nNearestI = int (0.49 + rctSrc.left + (i-rctDes.left)/double(rctDes.Width())*double(rctSrc.Width()));
			int nNearestJ = int (0.49 + rctSrc.top + (j-rctDes.top)/double(rctDes.Height())*double(rctSrc.Height()));
			
			SetPixel(i,j,ibSrc.GetPixel(nNearestI,nNearestJ));
		}
	}

	return true;
}

bool CImageBase::StretchBilinar(CRect rctSrc, CRect rctDes)
{
	CImageBase ibSrc;
	ibSrc.ModelFrom(this);

	if (rctDes.top <= 0)
		rctDes.top = 0;
	if (rctDes.bottom >= m_nHeight)
		rctDes.bottom = m_nHeight -1;
	if (rctDes.left <= 0)
		rctDes.left = 0;
	if (rctDes.right >= m_nWidth)
		rctDes.right = m_nWidth - 1;

	if (rctSrc.top <= 0)
		rctSrc.top = 0;
	if (rctSrc.bottom >= m_nHeight)
		rctSrc.bottom = m_nHeight -1;
	if (rctSrc.left <= 0)
		rctSrc.left = 0;
	if (rctSrc.right >= m_nWidth)
		rctSrc.right = m_nWidth - 1;

	for(int j=rctDes.top;j<rctDes.bottom;j++)
	{
		for(int i=rctDes.left;i<rctDes.right;i++)
		{
			double nNearestI = rctSrc.left + (i-rctDes.left)/double(rctDes.Width())*double(rctSrc.Width());
			double nNearestJ = rctSrc.top + (j-rctDes.top)/double(rctDes.Height())*double(rctSrc.Height());

			int a = int(nNearestI);
			int b = int(nNearestJ);
			double x = nNearestI;
			double y = nNearestJ;

			double nxb1 = (x-a)*ibSrc.GetGray(a+1,b+1) + (a+1-x)*ibSrc.GetGray(a,b+1);
			double nxb = (x-a)*ibSrc.GetGray(a+1,b) + (a+1-x)*ibSrc.GetGray(a,b);

			int nRe = int ((b+1-y)*nxb + (y-b)*nxb1);

			if (nRe <= 0)
				nRe = 0;
			else if (nRe >= 255)
				nRe = 255;

			SetGray(i,j,nRe);
		}
	}

	return true;
}

bool CImageBase::Template(int nTempW, int nTempH, int nTempMX, int nTempMY, float* pfTemp, float fCoef)
{
	CImageBase ibDes;
	ibDes.Resize(m_nWidth,m_nHeight);
	ibDes.InitPixel(BYTE(255));;

	for(int j=nTempMY;j<m_nHeight - (nTempH - nTempMY) + 1;j++)
	{
		for(int i=nTempMX;i<m_nWidth - (nTempW - nTempMX) + 1;i++)
		{
			float fRe = 0.0;

			for(int m=0;m<nTempH;m++)
			{
				for(int n=0;n<nTempW;n++)
				{
					fRe += GetGray(i + n - nTempMX,j + m - nTempMY) * pfTemp[m * nTempW + n];
				}
			}

			fRe *= fCoef;

			fRe = (float)fabs(fRe);

			BYTE btRe = 0;

			if (fRe >= 255)
				btRe = 255;
			else if (fRe <= 0)
				btRe = 0;
			else 
				btRe = (BYTE)(fRe);

			ibDes.SetGray(i,j,btRe);
		}
	}

	this->ModelFrom(&ibDes);

	return true;
}

bool CImageBase::SEAvg(void)
{
	static float Temp[] = {1,1,1,
						 1,1,1,
						 1,1,1};

	return Template(3,3,1,1,Temp,float(1.0/9.0));
}

bool CImageBase::SEGuassSmooth(void)
{
	static float Temp[] = {	1,2,1,
							2,4,2,
							1,2,1};

	return Template(3,3,1,1,Temp,float(1.0/16.0));
}

bool CImageBase::SEGuassSmoothBig(void)
{
	return SEGuassSmooth();
}


bool CImageBase::SEMedianFilter(void)
{
	SEOrderFilter(3,3,1,1,-1);

	return true;
}

bool CImageBase::SEOrderFilter(int Width, int Height,int nMidWidth ,int nMidHeight, int nOrder)
{
	CImageBase ibSrc;
	ibSrc.ModelFrom(this);

	if (nOrder == -1)
		nOrder = Width*Height/2;

	for(int j= nMidHeight;j<m_nHeight - (Height - nMidHeight);j++)
	{
		for(int i=nMidWidth;i<m_nWidth - (Width - nMidWidth);i++)
		{
			vector<int> vct;

			for(int jj=j-nMidHeight;jj<j+(Height - nMidHeight);j++)
			{
				for(int ii=i-nMidWidth;ii<i+(Width - nMidWidth);ii++)
				{
					vct.push_back(ibSrc.GetGray(ii,jj));
				}
			}

			sort(vct.begin(),vct.end());

			SetGray(i,j,vct[nOrder+1]);
		}
	}

	return true;
}

bool CImageBase::SELargestFilter(void)
{
	SEOrderFilter(3,3,1,1,1);

	return true;
}

bool CImageBase::SESmallestFilter(void)
{
	SEOrderFilter(3,3,1,1,9);

	return true;
}

bool CImageBase::Add(CImageBase* src1, CImageBase* src2)
{
	if (src1->m_nWidth != src2->m_nWidth)
		return false;
	if (src1->m_nHeight != src2->m_nHeight)
		return false;

	this->Resize(src1->m_nWidth,src1->m_nHeight);

	for(int j=0;j<src1->m_nHeight;j++)
	{
		for(int i=0;i<src2->m_nWidth;i++)
		{
			int btRed = src1->GetPixelByRed(i,j) + src2->GetPixelByRed(i,j);
			int btGreen = src1->GetPixelByGreen(i,j) + src2->GetPixelByGreen(i,j);
			int btBlue = src1->GetPixelByBlue(i,j) + src2->GetPixelByBlue(i,j);

			if (btRed > 255)
				btRed = 255;
			if (btGreen > 255)
				btGreen = 255;
			if (btBlue > 255)
				btBlue = 255;

			this->SetPixel(i,j,RGB(btRed,btGreen,btBlue));
		}
	}

	return true;
}

bool CImageBase::SESobel(void)
{
	static float TempV[] = {1,2,1,0,0,0,-1,-2,-1};
	static float TempH[] = {1,0,-1,2,0,-2,1,0,-1};

	CImageBase ibSrc1;
	CImageBase ibSrc2;

	ibSrc1.ModelFrom(this);
	ibSrc2.ModelFrom(this);

	ibSrc1.Template(3,3,1,1,TempV,1);
	ibSrc2.Template(3,3,1,1,TempH,1);

	this->Add(&ibSrc1,&ibSrc2);

	return true;
}

bool CImageBase::SELog(void)
{
	static float Temp []= {	0,0,-1,0,0,
							0,-1,-2,-1,0,
							-1,-2,-16,-2,-1,
							0,-1,-2,-1,0,
							0,0,-1,0,0
						};

	return Template(5,5,3,3,Temp,1);
}

bool CImageBase::SELaplaceCross(void)
{
	static float Temp[] = 
	{
		0,-1,0,
		-1,4,-1,
		0,-1,0
	};

	return Template(3,3,1,1,Temp,1);
}

bool CImageBase::SELaplaceSquar(void)
{
	static float Temp[] = 
	{
		-1,-1,-1,
		-1,8,-1,
		-1,-1,-1
	};

	return Template(3,3,1,1,Temp,1);
}

bool CImageBase::SERobert(void)
{
	static float TempV[] = {-1,0,0,1};
	static float TempH[] = {0,-1,1,0};

	CImageBase ibSrc1;
	CImageBase ibSrc2;

	ibSrc1.ModelFrom(this);
	ibSrc2.ModelFrom(this);

	ibSrc1.Template(2,2,1,1,TempV,1);
	ibSrc2.Template(2,2,1,1,TempH,1);

	this->Add(&ibSrc1,&ibSrc2);

	return true;
}

bool CImageBase::SEGuassLaplace(void)
{
	return SELog();
}

bool CImageBase::FEFilter(double* dFilter, BYTE btFill)
{
	long w = GetFreqWidth();
	long h = GetFreqHeight();

	complex<double> * cdFreqImg = new complex<double>[w * h];

	CMathBase::FFT2(this,true,cdFreqImg,btFill);

	for(long i=0;i<w*h;i++)
	{
		cdFreqImg[i] = cdFreqImg[i] * dFilter[i];
	}

	CMathBase::IFFT2(this,cdFreqImg,w,h,m_nWidth,m_nHeight);

	delete[] cdFreqImg;

	return true;
}

bool CImageBase::FEGetIdealLPF(double** dFilter, int nR)
{
	long w = GetFreqWidth();
	long h = GetFreqHeight();

	*dFilter = new double[w * h];
	
	double *dt = *dFilter;

	for(int j=0;j<h;j++)
	{
		for(int i=0;i<w;i++)
		{
			if (sqrt( pow((double)(i - w/2),2) + pow( (double)(j - h/2),2)) > nR)
			{
				dt[ (j<h/2?j+h/2:j-h/2)* w + (j<w/2)?j+w/2:j-w/2 ] = 0;
			}
			else
			{
				dt[ (j<h/2?j+h/2:j-h/2)* w + (j<w/2)?j+w/2:j-w/2 ] = 1;
			}
		}
	}

	return true;
}

bool CImageBase::FEIdealLPF(int nR,BYTE btFill)
{
	double *dFilter;

	FEGetIdealLPF(&dFilter,nR);

	return FEFilter(dFilter,btFill);
}

bool CImageBase::FEGetGaussLPF(double** dFilter, double dSigma)
{
	long w = GetFreqWidth();
	long h = GetFreqHeight();

	*dFilter = new double[w * h];
	double *dt = *dFilter;

	for(int j=0;j<h;j++)
	{
		for(int i=0;i<w;i++)
		{
			dt[ (j<h/2?j+h/2:j-h/2)* w + (j<w/2)?j+w/2:j-w/2 ] = exp( -(pow( (double)(i - w/2),2) + pow ( (double)(j - h/2),2 )) / 2.0 / pow(dSigma,2));
		}
	}

	return true;
}

bool CImageBase::FEGaussLPF(double dSigma,BYTE btFill)
{
	double *dFilter;

	FEGetGaussLPF(&dFilter,dSigma);

	return FEFilter(dFilter,btFill);
}

bool CImageBase::FEGetIdealHPF(double** dFilter, int nR)
{
	long w = GetFreqWidth();
	long h = GetFreqHeight();

	*dFilter = new double[w * h];

	double *dt = *dFilter;

	for(int j=0;j<h;j++)
	{
		for(int i=0;i<w;i++)
		{
			if (sqrt( pow((double)(i - w/2),2) + pow( (double)(j - h/2),2)) < nR)
			{
				dt[ (j<h/2?j+h/2:j-h/2)* w + (j<w/2)?j+w/2:j-w/2 ] = 0;
			}
			else
			{
				dt[ (j<h/2?j+h/2:j-h/2)* w + (j<w/2)?j+w/2:j-w/2 ] = 1;
			}
		}
	}

	return true;
}

bool CImageBase::FEIdealHPF(int nR,BYTE btFill)
{
	double *dFilter;

	FEGetIdealHPF(&dFilter,nR);

	return FEFilter(dFilter,btFill);
}

bool CImageBase::FEGetGaussHPF(double** dFilter, double dSigma)
{
	long w = GetFreqWidth();
	long h = GetFreqHeight();

	*dFilter = new double[w * h];
	double *dt = *dFilter;

	for(int j=0;j<h;j++)
	{
		for(int i=0;i<w;i++)
		{
			dt[ (j<h/2?j+h/2:j-h/2)* w + (j<w/2)?j+w/2:j-w/2 ] = 1 - exp( -(pow( (double)(i - w/2),2) + pow ( (double)(j - h/2),2 )) / 2.0 / pow(dSigma,2));
		}
	}

	return true;
}

bool CImageBase::FEGaussHPF(double dSigma,BYTE btFill)
{
	double *dFilter;

	FEGetGaussHPF(&dFilter,dSigma);

	return FEFilter(dFilter,btFill);
}

bool CImageBase::FEGetLaplace(double** dFilter)
{
	long w = GetFreqWidth();
	long h = GetFreqHeight();

	*dFilter = new double[w * h];
	double *dt = *dFilter;

	for(int j=0;j<h;j++)
	{
		for(int i=0;i<w;i++)
		{
			dt[ (j<h/2?j+h/2:j-h/2)* w + (j<w/2)?j+w/2:j-w/2 ] =  -(pow( (double)(i - w/2),2) + pow ( (double)(j - h/2),2 ));
		}
	}

	return true;
}

bool CImageBase::FELaplace(BYTE btFill)
{
	double *dFilter;

	FEGetLaplace(&dFilter);

	return FEFilter(dFilter,btFill);
}

bool CImageBase::Erode(int se[3][3])
{
	bool bMatch;

	CImageBase ibSrc;
	ibSrc.ModelFrom(this);

	for(int j=1;j<m_nHeight - 1;j++)
	{
		for(int i=1;i<m_nWidth - 1;i++)
		{
			bMatch = true;

			for(int k=0;k<3;k++)
			{
				for(int l=0;l<3;l++)
				{
					if (se[k][l] == -1)
					{
						continue;
					}
					else if (se[k][l] == 1)
					{
						if (ibSrc.GetGray(i-k+1,j-l+1) != 0)
						{
							bMatch = false;
							break;
						}
					}
					else if (se[k][l] == 0)
					{
						if (ibSrc.GetGray(i-k+1,j-l+1) != 255)
						{
							bMatch = false;
							break;
						}
					}
				}
			}

			if (bMatch)
				SetGray(i,j,255);
		}
	}

	return true;
}

bool CImageBase::ErodeCross(void)
{
	static int se[3][3] = { {-1,1,-1},{1,1,1},{-1,1,-1} };

	return Erode(se);
}

bool CImageBase::ErodeSquar(void)
{
	static int se[3][3] = { {1,1,1},{1,1,1},{1,1,1} };

	return Erode(se);
}

bool CImageBase::Dilate(int se[3][3])
{
	bool bMatch;

	CImageBase ibSrc;
	ibSrc.ModelFrom(this);

	for(int j=1;j<m_nHeight - 1;j++)
	{
		for(int i=1;i<m_nWidth - 1;i++)
		{
			bMatch = true;

			for(int k=0;k<3;k++)
			{
				for(int l=0;l<3;l++)
				{
					if (se[k][l] == -1)
					{
						continue;
					}
					else if (se[k][l] == 1)
					{
						if (ibSrc.GetGray(i-k+1,j-l+1) != 255)
						{
							bMatch = false;
							break;
						}
					}
					else if (se[k][l] == 0)
					{
						if (ibSrc.GetGray(i-k+1,j-l+1) != 0)
						{
							bMatch = false;
							break;
						}
					}
				}
			}

			if (bMatch)
				SetGray(i,j,255);
		}
	}

	return true;
}

bool CImageBase::DilateCross(void)
{
	static int se[3][3] = { {-1,1,-1},{1,1,1},{-1,1,-1} };

	return Dilate(se);
}

bool CImageBase::DilateSquar(void)
{
	static int se[3][3] = { {1,1,1},{1,1,1},{1,1,1} };

	return Dilate(se);
}

bool CImageBase::Open(int se[3][3])
{
	Erode(se);
	Dilate(se);

	return true;
}

bool CImageBase::Close(int se[3][3])
{
	Dilate(se);
	Erode(se);

	return true;
}

bool CImageBase::OpenCross(void)
{
	static int se[3][3] = { {-1,1,-1},{1,1,1},{-1,1,-1} };

	return Open(se);
}

bool CImageBase::OpenSquar(void)
{
	static int se[3][3] = { {1,1,1},{1,1,1},{1,1,1} };

	return Open(se);
}

bool CImageBase::CloseCross(void)
{
	static int se[3][3] = { {-1,1,-1},{1,1,1},{-1,1,-1} };

	return Close(se);
}

bool CImageBase::CloseSquar(void)
{
	static int se[3][3] = { {1,1,1},{1,1,1},{1,1,1} };

	return Close(se);
}

bool CImageBase::ReduceEdge(int nWidth,BYTE btFill)
{
	for(int i=0;i<nWidth;i++)
	{
		for(int j=0;j<m_nHeight;j++)
		{
			SetGray(i,j,btFill);
		}
	}

	for(int j=0;j<nWidth;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			SetGray(i,j,btFill);
		}
	}

	return true;
}

bool CImageBase::TraceBoundary(void)
{
	CImageBase ibSrc;
	ibSrc.ModelFrom(this);
	ibSrc.ReduceEdge(1);

	CImageBase ibDes;
	ibDes.ModelFrom(this);
	ibDes.InitPixel(BYTE(255));;

	CPoint ptStart;
	CPoint ptCur;

	static int Direction[8][2] = 
	{
		{-1,1},
		{0,1},
		{1,1},
		{1,0},
		{1,-1},
		{0,1},
		{-1,-1},
		{-1,0},
	};

	int nCurDir = 0;

	int xPos;
	int yPos;

	bool bAtStartedPt;

	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			if (ibSrc.GetGray(j,i) == 0)
			{
				ptStart.x = j;
				ptStart.y = i;

				ptCur = ptStart;
				bAtStartedPt = true;

				while( ( (ptCur.x != ptStart.x) || (ptCur.y != ptStart.y)) || bAtStartedPt )
				{
					bAtStartedPt = false;

					xPos = ptCur.x + Direction[nCurDir][0];
					yPos = ptCur.y + Direction[nCurDir][1];

					int nSearchTimes = 1;

					while(ibSrc.GetGray(xPos,yPos) == 255)
					{
						nCurDir ++;
						if (nCurDir >= 8)
							nCurDir -= 8;

						xPos = ptCur.x + Direction[nCurDir][0];
						yPos = ptCur.y + Direction[nCurDir][1];

						if (nSearchTimes >= 8)
						{
							xPos = ptCur.x;
							yPos = ptCur.y;
							
							break;
						}
					}

					ptCur.x = xPos;
					ptStart.y = yPos;

					ibDes.SetGray(ptCur.x,ptCur.y,0);

					nCurDir -= 2;
					if (nCurDir < 0)
						nCurDir += 8;
				}
			}
		}
	}

	this->ModelFrom(&ibDes);

	return false;
}

bool CImageBase::FillRgnM(CPoint ptStart)
{
	int se[3][3] = { {-1,1,-1},{1,1,1},{-1,1,-1} };

	CImageBase ibDes;
	ibDes.InitPixel(BYTE(255));

	CImageBase ibBr;
	ibBr.ModelFrom(this);
	ibBr.BitReverse();

	ibDes.SetGray(ptStart.x,ptStart.y,0);
	CImageBase ibTemp;
	ibTemp.ModelFrom(&ibDes);

	while(true)
	{
		ibDes.DilateCross();
		ibDes.And(&ibBr);

		if ( ibDes.Equal(&ibTemp))
		{
			break;
		}

		ibTemp.ModelFrom(&ibDes);
	}

	this->ModelFrom(&ibDes);

	return true;
}

bool CImageBase::BitReverse(void)
{
	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			SetGray(i,j,255 - GetGray(i,j));
		}
	}

	return true;
}

bool CImageBase::Equal(CImageBase* that)
{
	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			if (GetGray(i,j) != that->GetGray(i,j))
				return false;
		}
	}

	return true;
}

bool CImageBase::And(CImageBase* that)
{
	CImageBase ibDes;
	ibDes.Resize(that->m_nWidth,that->m_nHeight);
	ibDes.InitPixel(BYTE(255));;

	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			ibDes.SetGray(i,j,this->GetGray(i,j) & that->GetGray(i,j));
		}
	}

	this->ModelFrom(&ibDes);

	return true;
}

bool CImageBase::Or(CImageBase* that)
{
	CImageBase ibDes;
	ibDes.Resize(that->m_nWidth,that->m_nHeight);
	ibDes.InitPixel(BYTE(255));;

	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			ibDes.SetGray(i,j,this->GetGray(i,j) | that->GetGray(i,j));
		}
	}

	this->ModelFrom(&ibDes);

	return true;
}

bool CImageBase::GetConnectionRgnM(int nMin, int nMax,bool bFourConn,int *nConRgn)
{
	if (nMin == -1)
		nMin = -1;
	if (nMax == -1)
		nMax = m_nWidth * m_nHeight + 100;

	int se[3][3] = { {1,1,1},{1,1,1},{1,1,1} };

	if (bFourConn)
	{
		se[0][0] = -1;
		se[0][2] = -1;
		se[2][0] = -1;
		se[2][2] = -1;
	}
	else
	{
		;
	}

	CImageBase ibSrc;
	ibSrc.ModelFrom(this);
	ibSrc.ReduceEdge(1,255);

	CImageBase ibDes;
	ibDes.Resize(m_nWidth,m_nHeight);
	ibDes.InitPixel(BYTE(255));;

	CImageBase ibTmp;
	ibTmp.ModelFrom(&ibDes);

	int nConnRgn = 1;

	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			if (GetGray(i,j) == 0)
			{
				ibDes.InitPixel(BYTE(255));;
				ibDes.SetGray(i,j,0);

				while(true)
				{
					ibDes.Dilate(se);
					ibDes.And(&ibSrc);
					
					if (ibDes.Equal(&ibTmp))
					{
						break;
					}

					ibTmp.ModelFrom(&ibDes);
				}

				int nNum = 0;
				for(int n=0;n<m_nHeight;n++)
				{
					for(int m=0;m<m_nWidth;m++)
					{
						BYTE btGray = ibDes.GetGray(m,n);
						if (btGray == 0)
						{
							nNum ++;
						}
					}
				}

				if (nNum <= nMin || nNum >= nMax)
				{
					for(int n=0;n<m_nHeight;n++)
					{
						for(int m=0;m<m_nWidth;m++)
						{
							BYTE btGray = ibDes.GetGray(m,n);
							if (btGray == 0)
							{
								ibSrc.SetGray(m,n,0);
							}
						}
					}
				}
				else
				{
					for(int n=0;n<m_nHeight;n++)
					{
						for(int m=0;m<m_nWidth;m++)
						{
							BYTE btGray = ibDes.GetGray(m,n);
							if (btGray == 0)
							{
								ibSrc.SetGray(m,n,nConnRgn);
							}
						}
					}

					nConnRgn ++;
					if (nConnRgn >= 255)
					{
						return false;
					}
				}
			}
		}
	}

	this->ModelFrom(&ibSrc);

	if (nConRgn)
	{
		*nConRgn = nConnRgn;
	}
	return true;
}

bool CImageBase::Thining()
{
	bool bc1,bc2,bc3,bc4;
	bool neighboor[5][5];

	bool bModified = true;
	while(bModified)
	{
		bModified = false;
		CImageBase ibSrc;
		ibSrc.ModelFrom(this);

		for(int j=2;j<m_nHeight-2;j++)
		{
			for(int i=2;i<m_nWidth-2;i++)
			{
				bc1 = false;
				bc2 = false;
				bc3 = false;
				bc4 = false;

				BYTE btGray = GetGray(i,j);
				if (btGray == 255)
					continue;

				for(int m=0;m<5;m++)
				{
					for(int n=0;n<5;n++)
					{
						neighboor[m][n] = ( GetGray(i+n-2,j+m-2) == 0 );
					}
				}

				int nCount = neighboor[1][1] + 
					neighboor[1][2] + 
					neighboor[2][1] + 
					neighboor[2][3] + 
					neighboor[3][1] + 
					neighboor[3][2] + 
					neighboor[3][3] + 
					neighboor[1][3] ;

				if (nCount >= 2 && nCount <=6)
					bc1 = true;

				nCount = 0;
				if (neighboor[1][2] == false && neighboor[1][1] == true)
					nCount++;
				if (neighboor[1][1] == false && neighboor[2][1] == true)
					nCount++;
				if (neighboor[2][1] == false && neighboor[3][1] == true)
					nCount++;
				if (neighboor[3][1] == false && neighboor[3][2] == true)
					nCount++;
				if (neighboor[3][2] == false && neighboor[3][3] == true)
					nCount++;
				if (neighboor[3][3] == false && neighboor[2][3] == true)
					nCount++;
				if (neighboor[2][3] == false && neighboor[1][3] == true)
					nCount++;
				if (neighboor[1][3] == false && neighboor[1][2] == true)
					nCount++;
				
				if (nCount == 1)
					bc2 = true;

				if (neighboor[1][2]*neighboor[2][1]*neighboor[2][3] == 0)
				{
					bc3 = true;
				}
				else
				{
					nCount = 0;
					if (neighboor[0][2] == false && neighboor[0][1] == true)
						nCount++;
					if (neighboor[0][1] == false && neighboor[1][1] == true)
						nCount++;
					if (neighboor[1][1] == false && neighboor[2][1] == true)
						nCount++;
					if (neighboor[2][1] == false && neighboor[2][2] == true)
						nCount++;
					if (neighboor[2][2] == false && neighboor[2][3] == true)
						nCount++;
					if (neighboor[2][3] == false && neighboor[1][3] == true)
						nCount++;
					if (neighboor[1][3] == false && neighboor[0][3] == true)
						nCount++;
					if (neighboor[0][3] == false && neighboor[0][2] == true)
						nCount++;

					if (nCount != 1)
						bc3 = true;
				}

				if (neighboor[1][2] * neighboor[2][1] * neighboor[3][2] == 0)
				{
					bc4 = true;
				}
				else
				{
					nCount = 0;
					if (neighboor[1][1] == false && neighboor[1][0] == true)
						nCount++;
					if (neighboor[1][0] == false && neighboor[2][0] == true)
						nCount++;
					if (neighboor[2][0] == false && neighboor[3][0] == true)
						nCount++;
					if (neighboor[3][0] == false && neighboor[3][1] == true)
						nCount++;
					if (neighboor[3][1] == false && neighboor[3][2] == true)
						nCount++;
					if (neighboor[3][2] == false && neighboor[2][2] == true)
						nCount++;
					if (neighboor[2][2] == false && neighboor[1][2] == true)
						nCount++;
					if (neighboor[1][2] == false && neighboor[1][1] == true)
						nCount++;

					if (nCount != 1)
						bc4 = true;
				}

				if (bc1 && bc2 && bc3 && bc4)
				{
					ibSrc.SetPixel(i,j,255);
					bModified = true;
				}
				else
				{
					ibSrc.SetPixel(i,j,0);
				}
			}
		}

		this->ModelFrom(&ibSrc);
	}

	return true;
}
bool CImageBase::GetConnectionRgn(int nMin, int nMax,bool bFourCon)
{
	int nConnRgn = 0;

	if (nMin == -1)
		nMin = -1;
	if (nMax == -1)
		nMax = m_nWidth * m_nHeight + 100;

	vector<int> vct;

	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			int nNum = GetConnectionRgn_R(i,j,nConnRgn,bFourCon);
			if (nNum <= nMin || nNum >= nMax)
			{
				vct.push_back(nConnRgn);
			}
			
			nConnRgn ++;
			if (nConnRgn == 255)
			{
				return false;
			}
		}
	}

	GetConnectionRgn_SetBackground(vct);
	GetConnectionRgn_SetTouch(bFourCon);

	return true;
}

bool CImageBase::GetConnectionRgn_SetTouch(bool bfc)
{
	int nOrder[256];
	for(int i=0;i<256;i++)
	{
		nOrder[i] = i;
	}

	if (!bfc)
	{
		for(int j=0;j<m_nHeight;j++)
		{
			for(int i=0;i<m_nWidth;i++)
			{
				for(int m=0;m<3;m++)
				{
					for(int n=0;n<3;n++)
					{
						BYTE cur = GetGray(i,j);
						BYTE cmp = GetGray(i+m-1,j+n-1);
						if (cur > cmp)
						{
							nOrder[cur] = cmp;
						}
						else if (cur <cmp)
						{
							nOrder[cmp] = cur;
						}

					}
				}
			}
		}
	}
	else
	{
		for(int j=0;j<m_nHeight;j++)
		{
			for(int i=0;i<m_nWidth;i++)
			{
				for(int m=0;m<3;m++)
				{
					for(int n=0;n<3;n++)
					{
						if (m!=1 && n!=1)
							continue;

						BYTE cur = GetGray(i,j);
						BYTE cmp = GetGray(i+m-1,j+n-1);
						if (cur > cmp)
						{
							nOrder[cur] = nOrder[cmp];
						}
						else if (cur <cmp)
						{
							nOrder[cmp] = nOrder[cur];
						}
					}
				}
			}
		}
	}

	for(int i=0;i<256;i++)
	{
		for(int j=i+1;j<256;j++)
		{
			if (nOrder[j] = i)
				nOrder[j] = nOrder[i];
		}
	}

	nOrder[0] = 255;
	nOrder[255] = 255;

	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			SetGray(i,j,nOrder[GetGray(i,j)]);
		}
	}

	return true;
}

bool CImageBase::GetConnectionRgn_SetBackground(vector<int> &vct)
{
	int* nOrder = new int[256];
	memset(nOrder,0,sizeof(int));

	for(int i=0;i<(int)vct.size();i++)
	{
		nOrder[vct[i]] = 255;
	}

	int tmp = 0;
	for(int i=0;i<256;i++)
	{
		if (nOrder[i] == 255)
		{
			continue;
		}
		else
		{
			nOrder[i] = tmp;
			tmp++;
		}
	}

	nOrder[255] = 255;
	nOrder[0] = 255;

	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			SetGray(i,j,nOrder[GetGray(i,j)]);
		}
	}

	delete nOrder;

	return true;
}

int CImageBase::GetConnectionRgn_R(int i,int j,int nCon,bool bfc)
{
	if (GetGray(i,j) == 0)
	{
		SetGray(i,j,nCon);
	}

	int nCount = 0;

	if (bfc)
	{
		if (GetGray(i+1,j) == 0)
			nCount += GetConnectionRgn_R(i+1,j,nCon,bfc);
		if (GetGray(i,j+1) == 0)
			nCount += GetConnectionRgn_R(i,j+1,nCon,bfc);
		if (GetGray(i-1,j) == 0)
			nCount += GetConnectionRgn_R(i-1,j,nCon,bfc);
		if (GetGray(i,j-1) == 0)
			nCount += GetConnectionRgn_R(i,j-1,nCon,bfc);
	}
	else
	{
		if (GetGray(i+1,j) == 0)
			nCount += GetConnectionRgn_R(i+1,j,nCon,bfc);
		if (GetGray(i,j+1) == 0)
			nCount += GetConnectionRgn_R(i,j+1,nCon,bfc);
		if (GetGray(i-1,j) == 0)
			nCount += GetConnectionRgn_R(i-1,j,nCon,bfc);
		if (GetGray(i,j-1) == 0)
			nCount += GetConnectionRgn_R(i,j-1,nCon,bfc);
		if (GetGray(i+1,j+1) == 0)
			nCount += GetConnectionRgn_R(i+1,j+1,nCon,bfc);
		if (GetGray(i-1,j+1) == 0)
			nCount += GetConnectionRgn_R(i-1,j+1,nCon,bfc);
		if (GetGray(i-1,j-1) == 0)
			nCount += GetConnectionRgn_R(i-1,j-1,nCon,bfc);
		if (GetGray(i+1,j-1) == 0)
			nCount += GetConnectionRgn_R(i+1,j-1,nCon,bfc);
	}

	return nCount;
}

bool CImageBase::Convex(bool bLimited)
{
	int se1[3][3] = { {1,-1,-1},{1,0,-1},{1,-1,-1} };
	int se2[3][3] = { {1,1,1},{-1,0,-1},{-1,-1,-1} };
	int se3[3][3] = { {-1,-1,1},{-1,0,-1},{-1,-1,1} };
	int se4[3][3] = { {-1,-1,-1},{-1,0,-1},{1,1,1} };

	CImageBase ibDes;
	ibDes.ModelFrom(this);

	CImageBase ibTmp1;
	ibTmp1.ModelFrom(this);

	while(true)
	{
		ibDes.Erode(se1);
		ibDes.Or(&ibTmp1);
		if (ibDes.Equal(&ibTmp1))
			break;
		ibTmp1.ModelFrom(&ibDes);
	}

	CImageBase ibTmp2;
	ibTmp2.ModelFrom(this);
	ibDes.ModelFrom(this);

	while(true)
	{
		ibDes.Erode(se2);
		ibDes.Or(&ibTmp2);
		if (ibDes.Equal(&ibTmp2))
			break;
		ibTmp2.ModelFrom(&ibDes);
	}

	CImageBase ibTmp3;
	ibTmp3.ModelFrom(this);
	ibDes.ModelFrom(this);

	while(true)
	{
		ibDes.Erode(se3);
		ibDes.Or(&ibTmp3);
		if (ibDes.Equal(&ibTmp3))
			break;
		ibTmp3.ModelFrom(&ibDes);
	}

	CImageBase ibTmp4;
	ibTmp4.ModelFrom(this);
	ibDes.ModelFrom(this);

	while(true)
	{
		ibDes.Erode(se4);
		ibDes.Or(&ibTmp4);
		if (ibDes.Equal(&ibTmp4))
			break;
		ibTmp4.ModelFrom(&ibDes);
	}

	this->InitPixel(BYTE(255));;
	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			if (ibTmp1.GetGray(i,j) == 0 || ibTmp2.GetGray(i,j) == 0 || ibTmp3.GetGray(i,j) == 0 || ibTmp4.GetGray(i,j) == 0 )
			{
				this->SetGray(i,j,0);
			}
		}
	}

	CRect rctReal;
	GetRealRect(&rctReal,0);

	if (bLimited)
	{
		for(int j=0;j<m_nHeight;j++)
		{
			for(int i=0;i<m_nWidth;i++)
			{
				if (i < rctReal.top || i > rctReal.bottom || j < rctReal.left || j > rctReal.right)
				{
					this->SetGray(i,j,255);
				}
			}
		}
	}

	return true;
}

bool CImageBase::DilateGray(int se[3][3])
{
	CImageBase ibSrc;
	ibSrc.ModelFrom(this);

	for(int j=1;j<m_nHeight-1;j++)
	{
		for(int i=1;i<m_nWidth - 1;i++)
		{
			BYTE maxval = 0;
			
			for(int m=0;m<3;m++)
			{
				for(int n=0;n<3;n++)
				{
					if (se[m][n] == 1)
					{
						if (ibSrc.GetGray(i + m - 1,j + n - 1) > maxval)
						{
							maxval = ibSrc.GetGray(i + m - 1,j + n - 1);
						}
					}
				}
			}

			this->SetGray(i,j,maxval);
		}
	}

	return true;
}

bool CImageBase::ErodeGray(int se[3][3])
{
	CImageBase ibSrc;
	ibSrc.ModelFrom(this);

	for(int j=1;j<m_nHeight-1;j++)
	{
		for(int i=1;i<m_nWidth - 1;i++)
		{
			BYTE minval = 0;

			for(int m=0;m<3;m++)
			{
				for(int n=0;n<3;n++)
				{
					if (se[m][n] == 1)
					{
						if (ibSrc.GetGray(i + m - 1,j + n - 1) < minval)
						{
							minval = ibSrc.GetGray(i + m - 1,j + n - 1);
						}
					}
				}
			}

			this->SetGray(i,j,minval);
		}
	}

	return true;
}

bool CImageBase::OpenGray(int se[3][3])
{
	ErodeGray(se);
	DilateGray(se);

	return true;
}

bool CImageBase::CloseGray(int se[3][3])
{
	DilateGray(se);
	ErodeGray(se);

	return true;
}

bool CImageBase::Sub(CImageBase *that)
{
	for(int j=0;j<that->m_nHeight;j++)
	{
		for(int i=0;i<that->m_nWidth;i++)
		{
			BYTE btThat = that->GetGray(i,j);
			BYTE btThis = this->GetGray(i,j);

			int cur = btThis - btThat;

			if (cur < 0)
				cur = 0;
			
			this->SetGray(i,j,cur);
		}
	}

	return true;
}

bool CImageBase::TopHat(int se[3][3])
{
	CImageBase ib;
	ib.ModelFrom(this);

	ib.Open(se);

	this->Sub(&ib);

	return true;
}

bool CImageBase::SEPrewitt(void)
{
	static float tmp1[] = { -1,-1,-1,0,0,0,1,1,1 } ;
	static float tmp2[] = { -1,0,1,-1,0,1,-1,0,1 } ;

	CImageBase ib1;
	ib1.ModelFrom(this);
	ib1.Template(3,3,1,1,tmp1,1);

	CImageBase ib2;
	ib2.ModelFrom(this);
	ib2.Template(3,3,1,1,tmp2,1);

	this->InitPixel(BYTE(255));;
	this->Add(&ib1,&ib2);

	return true;
}

bool CImageBase::DCT()
{
	return CMathBase::DCT(this,NULL);
}

bool CImageBase::Walsh_Har()
{
	return CMathBase::Walsh_Har2(this,NULL);
}

bool CImageBase::InverseFilter(double *dTemp,double coef,int xLen,int yLen)
{
	bool bRe = true;

	long w = 1;
	long h = 1;
	long wp = 0;
	long hp = 0;

	while( 2*w <= m_nWidth)
	{
		w *= 2;
		wp ++;
	}

	while( 2*h <= m_nHeight)
	{
		h *= 2;
		hp ++;
	}

	if (w != m_nWidth)
		bRe = false;
	if (h != m_nHeight)
		bRe = false;

	complex<double> *TSrc = new complex<double>[w * h];
	complex<double> *FSrc = new complex<double>[w * h];
	complex<double> *TTemp = new complex<double>[w * h];
	complex<double> *FTemp = new complex<double>[w * h];

	for(int j=0;j<h;j++)
	{
		for(int i=0;i<w;i++)
		{
			if (i < xLen && j < yLen)
			{
				TTemp[i + j * w] = complex<double>(dTemp[i + j * xLen]/coef,0);
			}
			else
			{
				TTemp[i + j * w] = complex<double>(0.0,0.0);
			}

			if ( i < m_nWidth && j < m_nHeight)
			{
				TSrc[i + j * w] = complex<double>(GetGray(i,j),0.0);
			}
			else
			{
				TSrc[i + j * w] = complex<double>(0.0,0.0);
			}
		}
	}

	CMathBase::FFT2(TTemp,FTemp,wp,hp);
	CMathBase::IFFT2(TSrc,FSrc,wp,hp);

	for(int i=0;i< w * h;i++)
	{
		double a = FSrc[i].real();
		double b = FSrc[i].imag();
		double c = FTemp[i].real();
		double d = FTemp[i].imag();

		double re = 0.0;
		double im = 0.0;

		if ( c*c + d*d > 1e-3)
		{
			re = (a * c + b * d) / (c * c + d * d);
			im = (b * c - a * d) / (c * c + d * d);
		}

		FSrc[i] = complex<double>(re,im);
	}

	CMathBase::IFFT2(FSrc,TSrc,wp,hp);
	double MaxNum = 300;

	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			BYTE btGray = (BYTE)(TSrc[i + j * w].real() * 256 / MaxNum + 0.5);

			SetGray(i,j,btGray);
		}
	}

	delete[] TSrc;
	delete[] FSrc;
	delete[] TTemp;
	delete[] FTemp;

	return bRe;
}

bool CImageBase::InverseFilterAvg()
{
	static double Temp[] = 
	{
		1,1,1,1,1,
		1,1,1,1,1,
		1,1,1,1,1,
		1,1,1,1,1,
		1,1,1,1,1,
	};

	return InverseFilter(Temp,double(1.0/25.0),5,5);
}

bool CImageBase::WienerFilter()
{
	long w = 1;
	long h = 1;
	long wp = 0;
	long hp = 0;

	while( 2 * w <= m_nWidth)
	{
		w *= 2;
		wp++;
	}

	while( 2 * h <= m_nHeight)
	{
		h *= 2;
		hp ++;
	}

	complex<double> *TSrc = new complex<double>[w * h];
	complex<double> *TH = new complex<double>[w * h];
	complex<double> *FSrc = new complex<double>[w * h];
	complex<double> *FH = new complex<double>[w * h];
	
	double *Filter = new double[w * h];

	for(int j=0;j<h;j++)
	{
		for(int i=0;i<w;i++)
		{
			if (i <= m_nWidth && j <= m_nHeight)
			{
				double dG= (double)GetGray(i,j);

				TSrc[w * j + i] = complex<double>(dG,0.0);
				FSrc[w * j + i] = complex<double>(0.0,0.0);
			}
			else
			{
				TSrc[w * j + i] = complex<double>(0.0,0.0);
				FSrc[w * j + i] = complex<double>(0.0,0.0);
			}

			if (i < 5 && j < 5)
			{
				TH[w*j + i] = complex<double>(0.04,0.0);
				Filter[w*j + i] = 0.5;
			}
			else
			{
				TH[w*j + i] = complex<double>(0.0,0.0);
				Filter[w*j + i] = 0.05;
			}

			FH[w*j + i] = complex<double>(0.0,0.0);
		}
	}
	
	CMathBase::FFT2(TSrc,FSrc,w,h);
	CMathBase::FFT2(TH,FH,w,h);

	for(int i=0;i< w*h;i++)
	{
		double a = FSrc[i].real();
		double b = FSrc[i].imag();
		double c = FH[i].real();
		double d = FH[i].imag();

		double norm2 = c*c + d*d;
		double tmp = (norm2) / (norm2 + Filter[i]);
		
		double re = (a*c - b*d)/(c*c + d*d);
		double im = (b*c + a*d)/(c*c + d*d);

		FSrc[i] = complex<double>(tmp*re,tmp*im);
	}

	CMathBase::IFFT2(FSrc,TSrc,w,h);

	for(int j=0;j<h;j++)
	{
		for(int i=0;i<w;i++)
		{
			double dG = double(GetGray(i,j));

			double a = TSrc[w*j + i].real();
			double b = TSrc[w*j + i].imag();

			double norm2 = a*a + b*b;
			norm2 = sqrt(norm2) + 40;
			
			if (norm2 > 255)
				norm2 = 255;
			if (norm2 < 0)
				norm2 = 0;

			SetGray(i,j,(BYTE)norm2);
		}
	}

	delete[] TSrc;
	delete[] FSrc;
	delete[] TH;
	delete[] FH;
	delete[] Filter;

	return true;
}

bool CImageBase::MinDBFilter()
{
	long w = 1;
	long h = 1;
	long wp = 0;
	long hp = 0;

	while( 2 * w <= m_nWidth)
	{
		w *= 2;
		wp++;
	}

	while( 2 * h <= m_nHeight)
	{
		h *= 2;
		hp ++;
	}

	complex<double> *TSrc = new complex<double>[w * h];
	complex<double> *TH = new complex<double>[w * h];
	complex<double> *FSrc = new complex<double>[w * h];
	complex<double> *FH = new complex<double>[w * h];
	complex<double> *Filter = new complex<double>[w * h];
	complex<double> *FilterD = new complex<double>[w * h];

	for(int j=0;j<h;j++)
	{
		for(int i=0;i<w;i++)
		{
			Filter[i] = complex<double>(0.0,0.0);
			
			if (i <= m_nWidth && j <= m_nHeight)
			{
				double dG= (double)GetGray(i,j);

				TSrc[w * j + i] = complex<double>(dG,0.0);
				FSrc[w * j + i] = complex<double>(0.0,0.0);
			}
			else
			{
				TSrc[w * j + i] = complex<double>(0.0,0.0);
				FSrc[w * j + i] = complex<double>(0.0,0.0);
			}

			if (i < 5 && j < 5)
			{
				TH[w*j + i] = complex<double>(0.04,0.0);
				Filter[w*j + i] = 0.5;
			}
			else
			{
				TH[w*j + i] = complex<double>(0.0,0.0);
				Filter[w*j + i] = 0.05;
			}

			FH[w*j + i] = complex<double>(0.0,0.0);
		}
	}

	Filter[w*0 + 1] = complex<double>(-1,0.0);
	Filter[w*1 + 0] = complex<double>(-1,0.0);
	Filter[w*1 + 1] = complex<double>(4,0.0);
	Filter[w*1 + 2] = complex<double>(-1,0.0);
	Filter[w*2 + 1] = complex<double>(-1,0.0);

	CMathBase::FFT2(TSrc,FSrc,w,h);
	CMathBase::FFT2(TH,FH,w,h);
	CMathBase::FFT2(Filter,FilterD,w,h);

	double gama = 0.05;
	for(int i=0;i< w*h;i++)
	{
		double a = FSrc[i].real();
		double b = FSrc[i].imag();
		double c = FH[i].real();
		double d = FH[i].imag();
		double e = FilterD[i].real();
		double f = FilterD[i].imag();

		double norm1 = c*c + d*d;
		double norm2 = e*e + f*f;
		double temp = norm1 + norm2 * gama;

		double re = (a*c + b*d)/temp;
		double im = (b*c - a*d)/temp;

		FSrc[i] = complex<double>(re,im);
	}

	CMathBase::IFFT2(FSrc,TSrc,w,h);

	for(int j=0;j<h;j++)
	{
		for(int i=0;i<w;i++)
		{
			double dG = double(GetGray(i,j));

			double a = TSrc[w*j + i].real();
			double b = TSrc[w*j + i].imag();

			double norm2 = a*a + b*b;
			norm2 = sqrt(norm2) + 40;

			if (norm2 > 255)
				norm2 = 255;
			if (norm2 < 0)
				norm2 = 0;

			SetGray(i,j,(BYTE)norm2);
		}
	}

	delete[] TSrc;
	delete[] FSrc;
	delete[] TH;
	delete[] FH;
	delete[] Filter;
	delete[] FilterD;

	return true;
}

bool CImageBase::MaxProFilter()
{
	long w = 1;
	long h = 1;
	long wp = 0;
	long hp = 0;

	while(2 * w <= m_nWidth)
	{
		w *= 2;
		wp ++;
	}

	while(2 * h <= m_nHeight)
	{
		h *= 2;
		hp ++;
	}

	complex<double> *TSrc = new complex<double>[w*h];
	complex<double> *TH = new complex<double>[w*h];
	complex<double> *FSrc = new complex<double>[w*h];
	complex<double> *FH = new complex<double>[w*h];
	complex<double> *GF = new complex<double>[w*h];
	complex<double> *FHnew = new complex<double>[w*h];
	complex<double> *f1 = new complex<double>[w*h];
	complex<double> *f2 = new complex<double>[w*h];

	double *Filter = new double[w * h];
	complex<double> *TNoise = new complex<double>[w*h];
	complex<double> *FNoise = new complex<double>[w*h];

	for(int j=0;j<h;j++)
	{
		for(int i=0;i<w;i++)
		{
			double dG = (double)GetGray(i,j);

			TSrc[w*j + i] = complex<double>(dG,0.0);
			FSrc[w*j + i] = complex<double>(0.0,0.0);

			if (i < 5 && j <5)
			{
				TH[w*j + i] = complex<double>(0.04,0.0);
				Filter[w*j + i] = 0.05;
			}
			else
			{
				TH[w*j + i] = complex<double>(0.0,0.0);
				Filter[w*j + i] = 0.025;
			}

			FH[w*j + i] = complex<double>(0.0,0.0);

			if (i==j)
			{
				TNoise[w*j + i] = complex<double>(10,0.0);
			}
			else
			{
				TNoise[w*j + i] = complex<double>(0.0,0.0);
			}
		}
	}

	CMathBase::FFT2(TSrc,FSrc,w,h);
	GF = FSrc;
	f1 = FSrc;

	CMathBase::FFT2(TH,FH,w,h);
	CMathBase::FFT2(TNoise,FNoise,w,h);

	double gama = 0.05;
	double fain = 0,faif = 0,lamta;

	for(int i=0;i<w*h;i++)
	{
		double a = f1[i].real();
		double b = f1[i].imag();
		double e = FNoise[i].real();
		double f = FNoise[i].imag();

		fain += (e*e + f*f);
		faif += (a*a + b*b);
	}
	
	lamta = fain/faif;

	for(int k=1;k<6;k++)
	{
		for(int i=0;i<h*w;i++)
		{
			double a = f1[i].real();
			double b = f1[i].imag();
			double c = FH[i].real();
			double d = FH[i].imag();
			double e = FNoise[i].real();
			double f = FNoise[i].imag();

			double norm1 = c*c + d*d;
			double norm2 = a*a + b*b;
			//double norm3 = e*e + f*f;
			
			double temp = norm1 + lamta;
			double re = (a*c + b*d)/temp;
			double im = (b*c - a*d)/temp;

			FSrc[i] = complex<double>(re,im);
		}

		f2 = FSrc;
		for(int i=0;i<w*h;i++)
		{
			double a = f1[i].real();
			double b = f1[i].imag();
			double c = f2[i].real();
			double d = f2[i].imag();
			double e = GF[i].real();
			double f = GF[i].imag();

			double norm1 = a*a + b*b;
			double norm2 = c*c + d*d;

			double temp = norm1 + norm2/(w * h);
			temp = w*h*temp;

			double re = (e*c - f*d)/temp;
			double im = (f*c + e*d)/temp;

			FH[i] = complex<double>(re,im);
		}
	}

	CMathBase::IFFT2(FSrc,TSrc,w,h);

	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			double a = TSrc[w*j + i].real();
			double b = TSrc[w*j + i].imag();

			double norm2 = a*a + b*b;
			norm2 = sqrt(norm2) + 10;

			if (norm2 > 255)
				norm2 = 255;
			else if (norm2 < 0)
				norm2 = 0;

			SetGray(i,j,(BYTE)norm2);
		}
	}

	delete[] TSrc;
	delete[] TH;
	delete[] FSrc;
	delete[] FH;
	delete[] TNoise;
	delete[] FNoise;
	delete[] GF;
	delete[] FHnew;
	delete[] f1;
	delete[] f2;

	return true;
}

bool CImageBase::MaxEntrFilter()
{
	return false;
}

bool CImageBase::GeoFilter()
{
	BYTE *bitmap = new BYTE[m_nWidth * m_nHeight];

	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			bitmap[i*m_nWidth + j] = GetGray(j,i);
		}
	}

	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			double xu = (i - 382.491)/599.359;
			double yu = (j - 268.013)/598.340;
			double r = xu*xu + yu*yu;
			double xd=xu+xu*r*(-0.304484-0.040202*r)+2*0.002783*xu*yu-0.000259*(r+2*xu);
			double yd=yu+yu*r*(-0.304484-0.040202*r)+0.002783*(r+2*yu)-2*0.000259*xu*yu;
			double m = xd*599.359 + 382.491;
			double n = yd*598.340 + 268.013;
			int q = int(m);
			int l = int(n);
			int p = int(m-q);
			int g = int(n-l);
			
			bitmap[i + j*m_nWidth] = (1-p)*(1-q)*GetGray(l,g) + (1-p)*q*GetGray(l+1,g) + p*(1-q)*GetGray(l,g+1)+p*q*GetGray(l+1,g+1);
		}
	}

	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			SetGray(i,j,bitmap[i + j*m_nWidth]);
		}
	}

	return true;
}

bool CImageBase::Max(CImageBase *that)
{
	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			BYTE btThis = this->GetGray(i,j);
			BYTE btThat = that->GetGray(i,j);

			if (btThis < btThat)
				SetGray(i,j,btThis);
		}
	}

	return true;
}

bool CImageBase::Min(CImageBase *that)
{
	for(int j=0;j<m_nHeight;j++)
	{
		for(int i=0;i<m_nWidth;i++)
		{
			BYTE btThis = this->GetGray(i,j);
			BYTE btThat = that->GetGray(i,j);

			if (btThis > btThat)
				SetGray(i,j,btThis);
		}
	}

	return true;
}

bool CImageBase::SEKrisch()
{
	CImageBase ibTemp;
	ibTemp.ModelFrom(this);

	static float temp1[] = 
	{	5,5,5,
		-3,0,-3,
		-3,-3,-3
	};

	static float temp2[] = 
	{	-3,5,5,
	-3,0,5,
	-3,-3,-3
	};

	static float temp3[] = 
	{	-3,-3,5,
	-3,0,5,
	-3,-3,5
	};

	static float temp4[] = 
	{	-3,-3,-3,
	-3,0,5,
	-3,5,5
	};

	static float temp5[] = 
	{	-3,-3,-3,
	-3,0,-3,
	5,5,5
	};

	static float temp6[] = 
	{	-3,-3,-3,
	5,0,-3,
	5,5,-3
	};

	static float temp7[] = 
	{	5,-3,-3,
	5,0,-3,
	5,-3,-3
	};

	static float temp8[] = 
	{	5,5,-3,
	5,0,-3,
	-3,-3,-3
	};

	ibTemp.Template(3,3,1,1,temp1,0.5);
	this->Max(&ibTemp);

	ibTemp.Template(3,3,1,1,temp2,0.5);
	this->Max(&ibTemp);

	ibTemp.Template(3,3,1,1,temp3,0.5);
	this->Max(&ibTemp);

	ibTemp.Template(3,3,1,1,temp4,0.5);
	this->Max(&ibTemp);

	ibTemp.Template(3,3,1,1,temp5,0.5);
	this->Max(&ibTemp);

	ibTemp.Template(3,3,1,1,temp6,0.5);
	this->Max(&ibTemp);

	ibTemp.Template(3,3,1,1,temp7,0.5);
	this->Max(&ibTemp);

	ibTemp.Template(3,3,1,1,temp8,0.5);
	this->Max(&ibTemp);

	return true;
}