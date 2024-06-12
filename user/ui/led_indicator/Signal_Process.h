
/**
	*  Signal_Process.h
	*  Created & Modif by Yeremias Yosef Bria on 19 Desemberember 2019.
	*  yeramias.bria@gmail.com
	*  Library Signal Processing (FFT / IIR). 
	*/
	
	
#ifndef __Signal_Process_h
#define __Signal_Process_h

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

/* User Defined Signal Filter Process Mode (FFT a/ IIR) */
#define FFT					(1)		// 0 Jika IIR Mode.

/* User Defined */
#define sampleData			(64)	// Nilai N data harus = 2^n
#define NOISE				(30)

/* membuat tipe data untuk bilangan Complex (Real & Imaginer) */
typedef struct
{
	float Bil_real;
	float Bil_imag;
}ComplexValue;

#if FFT
	extern ComplexValue dataFFT[sampleData+1];
#endif

/* Prototype Function Init. */
float absMagnitude(float bilReal, float bilImag);
ComplexValue perkalianComplex(ComplexValue a, ComplexValue b);
void ComplexTo_5_FreqBand(ComplexValue *data, int *magnitudeBand_out);
void ComplexTo_Magnitude(ComplexValue *data, int *magnitudeBand_out);

void FFT_init(void);
void FFT_analysis(int16_t *data, ComplexValue *dataOut_FFT);

#ifdef __cplusplus
 }
#endif

#endif
