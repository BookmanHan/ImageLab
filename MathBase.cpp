#include <StdAfx.h>

#include <vector>
#include <complex>
#include <algorithm>
#include <cmath>

#include "ImageBase.h"
#include "MathBase.h"

using namespace std;

const double CMathBase::PI = 3.1415926535897932384;

CMathBase::CMathBase(void)
{

}

CMathBase::~CMathBase(void)
{

}

bool CMathBase::FFT(complex<double>* TD, complex<double>* FD, int r)
{
	long count;
	int bfsize;
	int p;
	double angel;

	complex<double> *W,*X1,*X2,*X;

	count = 1<<r;

	W = new complex<double>[count/2];
	X1 = new complex<double>[count];
	X2 = new complex<double>[count];

	for(int i=0;i<count/2;i++)
	{
		angel = -i * PI * 2 / count;
		W[i] = complex<double>(cos(angel),sin(angel));
	}

	memcpy(X1,TD,sizeof(complex<double>) * count);

	for(int k=0;k<r;k++)
	{
		for(int j=0;j< 1<<k ;j++)
		{
			bfsize = 1 << (r-k);
			
			for(int i=0;i<bfsize / 2;i++)
			{
				p = j * bfsize;
				X2[i + p] = X1 [i + p] + X1[i + p + bfsize/2];
				X2[i + p + bfsize/2] = (X1[i + p] - X1[i + p + bfsize/2]) * W[i * (1 << k)];
			}
		}

		X = X1;
		X1 = X2;
		X2 = X;
	}

	for(int j=0;j < count;j++)
	{
		p = 0;

		for(int i = 0;i < r;i++)
		{
			if (j & (1<<i))
			{
				p += 1 << (r - i -1);
			}
		}

		FD[j] = X1[p];
	}

	delete W;
	delete X1;
	delete X2;

	return true;
}

bool CMathBase::IFFT(complex<double>* FD, complex<double>* TD, int r)
{
	long count;
	int i;
	complex<double> *X;
	
	count = 1 << r;
	X = new complex<double>[count];

	memcpy(X,FD,sizeof(complex<double>) * count);

	for(int i=0;i < count;i++)
	{
		X[i] = complex<double>(X[i].real(),-X[i].imag());
	}

	FFT(X,TD,r);

	for(int i=0;i < count;i++)
	{
		TD[i] = complex<double>(TD[i].real()/count,-TD[i].imag()/count);
	}
	
	delete X;

	return true;
}

bool CMathBase::FFT2(CImageBase* ib, bool bExpand, complex<double>* out, BYTE btFill)
{
	double dTemp;

	long w = 1;
	long h = 1;
	int wp = 0;
	int hp = 0;

	while(w * 2 <= ib->GetWidth())
	{
		w *= 2;
		wp ++;
	}

	while(h * 2 <= ib->GetHeight())
	{
		h *= 2;
		hp ++;
	}

	if ((bExpand) && (w != ib->GetWidth()) && (h != ib->GetHeight()))
	{
		w *= 2;
		wp ++;

		h *= 2;
		hp ++;
	}

	complex<double> *TD = new complex<double>[w*h];
	complex<double> *FD = new complex<double>[w*h];

	for(int i=0;i < h;i++)
	{
		for(int j=0;j<w;j++)
		{
			if (bExpand)
			{
				if ( (j < ib->GetWidth()) && (i < ib->GetHeight()))
				{
					TD[j + w*i] = complex<double>(ib->GetGray(j,i),0);
				}
				else
				{
					TD[j + w*i] = complex<double>(btFill,0);
				}
			}
			else
			{
				TD[j + w*i] = complex<double>(ib->GetGray(j,i),0);
			}
		}
	}

	for(int i=0;i<h;i++)
	{
		FFT(&TD[w*i],&FD[w*i],wp);
	}

	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			TD[i + h*j] = FD[j + w*i];
		}
	}

	for(int i=0;i<w;i++)
	{
		FFT(&TD[i*h],&FD[i*h],hp);
	}

	if (out)
	{
		for(int i=0;i<h;i++)
		{
			for(int j=0;j<w;j++)
			{
				out[i * w + j] = FD[j * h + i];
			}
		}
	}
	
	ib->Resize(h,w);
	
	double dMax = 0;
	double dMin = 1E+010;
	
	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			dTemp = sqrt(FD[j*h + i].real()*FD[j*h + i].real() + FD[j*h +i].imag()*FD[j*h + i].imag()) / 100;
			dTemp = log(1 + dTemp);

			dMax = max(dMax,dTemp);
			dMin = min(dMin,dTemp);
		}
	}

	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			dTemp = sqrt(FD[j*h + i].real()*FD[j*h + i].real() + FD[j*h +i].imag()*FD[j*h + i].imag()) / 100;
			dTemp = log(1 + dTemp);

			dTemp = (dTemp - dMin) / (dMax - dMin) * 255;

			ib->SetGray( (j<w/2?j+w/2:j-w/2),(i<h/2?i+h/2:i-h/2),(BYTE)(dTemp+0.49));
		}
	}

	delete TD;
	delete FD;

	return true;
}

bool CMathBase::IFFT2(CImageBase* ib, complex<double>* in, long lWidth, long lHeight, long lOutW, long lOutH)
{
	double dTemp;
	
	long w = 1;
	long h = 1;
	int wp = 0;
	int hp = 0;

	if (lOutH == 0)
		lOutH = lHeight;
	if (lOutW == 0)
		lOutW = lWidth;

	while(w * 2 <= lWidth)
	{
		w *= 2;
		wp ++;
	}

	while(h * 2 <= lHeight)
	{
		h *= 2;
		hp ++;
	}

	complex<double> *TD = new complex<double>[w * h];
	complex<double> *FD = new complex<double>[w * h];

	ib->Resize(lOutW,lOutH);
	
	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			FD[j + w * i] = in[j + w * i];
		}
	}

	for(int i=0;i<h;i++)
	{
		IFFT(&FD[w*i],&TD[w*i],wp);
	}

	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			FD[i + h*j] = TD[j + w*i];
		}
	}

	for(int i=0;i<w;i++)
	{
		IFFT(&FD[i*h],&TD[i*h],hp);
	}

	double dMax = 0;
	double dMin = 1E+010;

	for(int i=0;i<lOutH;i++)
	{
		for(int j=0;j<lOutW;j++)
		{
			dTemp = sqrt(TD[j*h+i].real() * TD[j*h + i].real() + TD[j*h + i].imag() * TD[j*h +i].imag());

			dMax = max(dMax,dTemp);
			dMin = min(dMin,dTemp);
		}
	}

	for(int i=0;i<lOutH;i++)
	{
		for(int j=0;j<lOutW;j++)
		{
			dTemp = sqrt(TD[j*h+i].real() * TD[j*h + i].real() + TD[j*h + i].imag() * TD[j*h +i].imag());
			
			dTemp = (dTemp - dMin) / (dMax - dMin) * 255;

			ib->SetGray(j,i,BYTE(dTemp + 0.49));
		}
	}

	delete TD;
	delete FD;

	return true;
}

bool CMathBase::DCT(CImageBase *ib,double *out)
{
	long w = ib->GetFreqWidth();
	long h = ib->GetFreqHeight();

	double *TD = new double[w * h];
	double *FD = new double[w * h];

	long wp = int(log((double)w)/log(10.0) + 0.3);
	long hp = int(log((double)h)/log(10.0) + 0.3);

	for(int j=0;j<h;j++)
	{
		for(int i=0;i<w;i++)
		{
			if (i < ib->GetWidth() && j <ib->GetHeight())
				TD[j + i*w] = ib->GetGray(i,j);
			else 
				TD[j + i*w] = 255;
		}
	}

	for(int i=0;i<h;i++)
	{
		DisFCosTran(&TD[w * i],&FD[w * i],wp);
	}

	for(int j=0;j<h;j++)
	{
		for(int i=0;i<w;i++)
		{
			TD[j*h + i] = FD[j + w*i];
		}
	}

	for(int j=0;j<w;j++)
	{
		DisFCosTran(&TD[j*h],&FD[j*h],hp);
	}

	for(int j=0;j<h;j++)
	{
		for(int i=0;i<w;i++)
		{
			double dTemp = fabs(FD[j*h + i]);

			if (dTemp > 255)
				dTemp = 255;

			ib->SetGray(i,j,(BYTE)(dTemp + 0.49));
		}
	}

	if (out != NULL)
	{
		for(int i=0;i<w*h;i++)
			out[i] = FD[i];
	}

	delete TD;
	delete FD;

	return true;
}

bool CMathBase::DisFCosTran(double *TD,double *FD,int power)
{
	long dotCount;
	dotCount = 1 << power;

	complex<double> *temp = new complex<double>[dotCount * 2];
	for(int i=0;i<dotCount;i++)
	{
		temp[i] = complex<double>(TD[i],0);
	}

	complex<double> *out = new complex<double>[dotCount * 2];
	FFT(temp,out,power);

	double dS = 1.0/sqrt(double(power));

	FD[0] = temp[0].real() * dS;
	dS *= sqrt(2.0f);

	for(int i=1;i<dotCount;i++)
	{
		FD[i] = ( out[i].real() * cos(i * PI / (2 * dotCount)) ) + out[i].imag() * sin(i * PI / (2 * dotCount));
	}

	delete out;
	delete temp;

	return true;
}

bool CMathBase::Walsh_Har2(CImageBase *ib,double *out)
{
	long w = ib->GetFreqWidth();
	long h = ib->GetFreqHeight();


	long wp = int(log((double)w)/log(10.0) + 0.3);
	long hp = int(log((double)h)/log(10.0) + 0.3);

	double *TD = new double[h * w];
	double *FD = new double[h * w];

	for(int j=0;j<h;j++)
	{
		for(int i=0;i<w;i++)
		{
			if (i < ib->GetWidth() && j <ib->GetHeight())
				TD[j + i*w] = ib->GetGray(i,j);
			else 
				TD[j + i*w] = 255;
		}
	}

	for(int i=0;i<h;i++)
	{
		Walsh_Har(TD + w*i,FD + w*i,wp);
	}

	for(int j=0;j<h;j++)
	{
		for(int i=0;i<w;i++)
		{
			TD[ j*h + i] = FD[j + w * i];
		}
	}

	for(int i=0;i<w;i++)
	{
		Walsh_Har(TD + h*i,FD + h*i,hp);
	}

	for(int j=0;j<h;j++)
	{
		for(int i=0;i<w;i++)
		{
			double dd = fabs(FD[i * h + j] * 1000);
			if (dd > 255)
				dd = 255;
			ib->SetGray(i,j,BYTE(dd+0.5));
		}
	}

	if (out)
	{
		for(int i = 0;i< w * h ;i++)
		{
			out[i] = FD[i];
		}
	}

	delete TD;
	delete FD;

	return true;
}

bool CMathBase::Walsh_Har(double *TD,double *FD,int power)
{
	long dotCount ;
	dotCount = 1 << power;

	double *d1 = new double[dotCount];
	double *d2 = new double[dotCount];

	memcpy(d1,TD,sizeof(double) * dotCount);

	for(int k=0;k<power;k++)
	{
		for(int j=0;j< (1 << k);j++)
		{
			int bfsize = 1 << (power - k);
			for(int i=0;i<bfsize/2;i++)
			{
				int p = j * bfsize;
				d2[i + p] = d1[i + p] + d1[i + p + bfsize/2];
				d2[i + p + bfsize/2] = d1[i + p] - d1[i + p + bfsize/2];
			}
		}

		double *tmp;
		tmp = d1;
		d1 = d2;
		d2 = tmp;
	}

	return true;
}

bool CMathBase::FFT2(complex<double>* in,complex<double>* out,int wp,int hp)
{
	double dTemp;

	long w = 1;
	long h = 1;

	for(int i=0;i<wp;i++)
		w *= 2;
	for(int i=0;i<hp;i++)
		h *= 2;

	complex<double> *TD = in;
	complex<double> *FD = out;

	for(int i=0;i<h;i++)
	{
		FFT(&TD[w*i],&FD[w*i],wp);
	}

	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			TD[i + h*j] = FD[j + w*i];
		}
	}

	for(int i=0;i<w;i++)
	{
		FFT(&TD[i*h],&FD[i*h],hp);
	}

	return true;
}

bool CMathBase::IFFT2(complex<double>* in,complex<double>* out,int wp,int hp)
{
	double dTemp;

	long w = 1;
	long h = 1;

	for(int i=0;i<wp;i++)
		w *= 2;
	for(int i=0;i<hp;i++)
		h *= 2;

	complex<double> *TD = out;
	complex<double> *FD = in;

	for(int i=0;i<h;i++)
	{
		IFFT(&FD[w*i],&TD[w*i],wp);
	}

	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			FD[i + h*j] = TD[j + w*i];
		}
	}

	for(int i=0;i<w;i++)
	{
		IFFT(&FD[i*h],&TD[i*h],hp);
	}

	return true;
}