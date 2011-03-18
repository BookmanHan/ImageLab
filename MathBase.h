#pragma once

#include <complex>

using namespace std;

class CMathBase
{
public:
	CMathBase(void);
	~CMathBase(void);

	static const double PI;

	static bool FFT(complex<double>* TD, complex<double>* FD, int r);
	static bool IFFT(complex<double>* FD, complex<double>* TD, int r);
	static bool FFT2(CImageBase* ib, bool bExpand, complex<double>* out, BYTE btFill);
	static bool IFFT2(CImageBase* ib, complex<double>* in, long lWidth, long lHeight, long lOutW, long lOutH);
	
	static bool DCT(CImageBase *ib,double *out);
	static bool DisFCosTran(double *TD,double *FD,int power);

	static bool Walsh_Har2(CImageBase *ib,double *out);
	static bool Walsh_Har(double *TD,double *FD,int power);

	static bool Dis_KL();
	static bool SVD();

	static bool FFT2(complex<double>* in,complex<double>* out,int wp,int hp);
	static bool IFFT2(complex<double>* in,complex<double>* out,int wp,int hp);

};
