/**
	*  Signal_Process.c
	*  Created & Modif by Yeremias Yosef Bria on 19 Desember 2019.
	*  yeramias.bria@gmail.com
	*  Source Code Signal Processing (FFT / IIR).
	*/

#include "Signal_Process.h"

#define Enable_DC_Blocker (0)
#if Enable_DC_Blocker
  #define Auto_DC_Blocker (1)
#endif

#define abs(a)			((a>=0)?(a):(a*-1))

/* =========================       FFT  Code        =========================== */
#if FFT
#include "math.h"

ComplexValue dataFFT[sampleData+1];


/* Private typedef Config --------------------------------------------------------*/
#define pi2					(float) (6.2831853072)
static uint16_t _nData=0;
static uint8_t _power=0;

const float Kc1[] = {0.0000000000, 0.7071067812, 0.9238795325, 0.9807852804,
										0.9951847267, 0.9987954562, 0.9996988187, 0.9999247018,
										0.9999811753, 0.9999952938, 0.9999988235, 0.9999997059,
										0.9999999265, 0.9999999816, 0.9999999954, 0.9999999989,
										0.9999999997};
const float Kc2[] = {1.0000000000, 0.7071067812, 0.3826834324, 0.1950903220,
										0.0980171403, 0.0490676743, 0.0245412285, 0.0122715383,
										0.0061358846, 0.0030679568, 0.0015339802, 0.0007669903,
										0.0003834952, 0.0001917476, 0.0000958738, 0.0000479369,
										0.0000239684};

uint8_t Exponent(uint16_t value)
{
	/* Calculates the base 2 logarithm of a value */
	uint8_t result = 0;
	while (((value >> result) & 1) != 1) result++;
	return(result);
}

void swap(int16_t *a, int16_t *b)
{
	int temp = *b;
	*b=*a;
	*a=temp;
}

void DC_Blocker(int16_t *data, ComplexValue *ComplexData)
{
#if Enable_DC_Blocker
  #if Auto_DC_Blocker
    int32_t mean = 0;
    for(uint16_t i=0; i<_nData; i++) mean += data[i];
    mean /= _nData;
  #else
    int32_t mean = 512;
  #endif
#endif
	
	/* mengubah data ke bilangan komplex dan block INPUT DC */
	for(uint16_t i=0; i<_nData; i++)
	{
  #if Enable_DC_Blocker
		ComplexData[i].Bil_real = (float)(data[i] - mean);
  #else
    ComplexData[i].Bil_real = (float)(data[i]);
  #endif
		ComplexData[i].Bil_imag = 0.0;
	}
}

float absMagnitude(float bilReal, float bilImag)
{
	// Fungsi ini sama dengan |magnitude|=square(real^2 + imaginer^2);
	// namun di sederhanakn untuk perhitungan waktu yang cepat.
	float temp = abs(bilReal) + abs(bilImag);
	return temp/1.414;		/* sama dengan akar2 */
}

ComplexValue perkalianComplex(ComplexValue a, ComplexValue b)
{
	ComplexValue temp;
	temp.Bil_real=(float)(a.Bil_real*b.Bil_real)-(a.Bil_imag*b.Bil_imag);
	temp.Bil_imag=(float)(a.Bil_real*b.Bil_imag)+(a.Bil_imag*b.Bil_real);
	return temp;
}

__weak void ComplexTo_5_FreqBand(ComplexValue *data, int *magnitudeBand_out)
{
	// 5 band: 156Hz(0-2), 371Hz(2-4), 883Hz(4-10) 2,1kHz(10-23), 5,0kHz(23-54)
	int temp1=0, temp4=0;
	// konversi output bilangan komplex ke data absoluted magnitude
	// data hasil FFT 1/2 dari data sample. 1/2 lagi merupakan mirror.
	for(uint16_t i=1; i<30; i++)
	{
		int temp = absMagnitude(data[i].Bil_real, data[i].Bil_imag);
		if(temp > NOISE)
		{
			temp -= NOISE;
			if(i<2) temp1 += temp;									// 156 Hz					(Bass Filter)
			else if(i>17) temp4 += temp;						// 2,65kHz~5,1kHz	(Treble Filter)
		}
	}
	/* Out Magnitude Fikter freq. Low & High */
	magnitudeBand_out[0] = temp1 / 11;
	magnitudeBand_out[1] = temp4 / 18; // temp4/25/24;
}

__weak void ComplexTo_Magnitude(ComplexValue *data, int *magnitudeBand_out)
{
	// konversi output bilangan komplex ke data absoluted magnitude
	// data hasil FFT 1/2 dari data sample. 1/2 lagi merupakan mirror.
	for(uint16_t i=1; i<_nData/2; i++)
	{
		int temp = absMagnitude(data[i].Bil_real, data[i].Bil_imag);	
		if(temp > NOISE) temp -= NOISE;
		/* Out Magnitude */
		magnitudeBand_out[i] = temp;
	}
}

void FFT_init(void)
{
	for(uint8_t i=1; i<16; i++)
	{
		if(pow(2, i)==sampleData) 		// mencari quadrat sesuai biner 2^n
		{
			_nData = sampleData;
			break;
		}
	}
	_power = Exponent(_nData);
}

void FFT_analysis(int16_t *data, ComplexValue *dataOut_FFT)
{
	uint32_t j = 0;
	uint32_t index = 0;
	
	/* tukar bit data / reverese bits (Data) */
	for (uint16_t i = 0; i < (_nData - 1); i++)
	{
		if (i < j) swap(&data[i], &data[j]);
		uint32_t k = (_nData >> 1);
		while (k <= j)
		{
			j -= k;
			k >>= 1;
		}
		j += k;
	}
	
	DC_Blocker(data, dataOut_FFT);
	
	/* hitung FFT dengan butterfly -> 2 Radix: */
	float c1 = -1.0;
	float c2 = 0.0;
	uint32_t l2 = 1;
	for(uint8_t l = 0; (l < _power); l++) 
	{
		uint32_t l1 = l2;
		l2 <<= 1;
		float u1 = 1.0;
		float u2 = 0.0;
		for(j = 0; j < l1; j++) 
		{
			for(uint16_t i = j; i < _nData; i += l2)
			{
					uint32_t i1 = i + l1;

					float t1 = u1 * dataOut_FFT[i1].Bil_real - u2 * dataOut_FFT[i1].Bil_imag;
					float t2 = u1 * dataOut_FFT[i1].Bil_imag + u2 * dataOut_FFT[i1].Bil_real;

					dataOut_FFT[i1].Bil_real = dataOut_FFT[i].Bil_real - t1;
					dataOut_FFT[i1].Bil_imag = dataOut_FFT[i].Bil_imag - t2;
					dataOut_FFT[i].Bil_real += t1;
					dataOut_FFT[i].Bil_imag += t2;
			 }
			 float z = ((u1 * c1) - (u2 * c2));
			 u2 = ((u1 * c2) + (u2 * c1));
			 u1 = z;
		}
		
		c2 = (Kc2[index]);
		c1 = (Kc1[index]);
		index++;

		c2 = -c2;
	}
}
#endif
/* =========================      end FFT Code    ============================ */

/* ======================== IIR Filter Audio INPUT - Code ===================== */

#if !FFT
#define blockDC

#ifdef blockDC
uint32_t bufADC_sum=0;
uint16_t meanADC=0;
uint16_t countADC_sum=0;
#endif

int v[5]={0,0,0,0,0};			// LPF
int v1[5]={0,0,0,0,0};		// HPF

/*
//Low pass butterworth filter order=1 alpha1=0.075 (FC: 150, Fs: 2kHz)
short stepLPF_orde1(int x)
{
	v[0] = v[1];
	long tmp = (((x * 6343L)	//= (   1.9359960593e-1 * x)
		+ (v[0] * 20080L))				//+(  0.6128007881*v[0])
	  + 16384) >> 15; 				// round and downshift fixed point /32768

	v[1]= (int)tmp;
	return (short)(v[0] + v[1]); // 2^
}
*/

/*
//Low pass butterworth filter order=2 alpha1=0.075 (FC: 150, Fs: 2kHz)
short stepLPF_orde2(short x)
{
	v[0] = v[1];
	v[1] = v[2];
	long tmp = ((((x *  21629L) >>  5)	//= (   4.1253537242e-2 * x)
		+ (v[0] * -8421L)	//+( -0.5139818942*v[0])
		+ (v[1] * 22101L))	//+(  1.3489677453*v[1])
		+8192) >> 14; // round and downshift fixed point /16384

	v[2]= (short)tmp;
	return (short)((v[0] + v[2])+2 * v[1]); // 2^
}*/

/*
//Low pass butterworth filter order=2 alpha1=0.075 (FC: 150, Fs: 2.5kHz)
short stepLPF_orde2(int x)
{
	v[0] = v[1];
	v[1] = v[2];
	long tmp = ((((x *  29213L) >>  6)	//= (   2.7859766117e-2 * x)
		+ (v[0] * -9616L)	//+( -0.5869195081*v[0])
		+ (v[1] * 24174L))	//+(  1.4754804436*v[1])
		+ 8192) >> 14; // round and downshift fixed point /16384

	v[2]= (int)tmp;
	return (short)(((v[0] + v[2])+2 * v[1])); // 2^
} */

/*
//Low pass butterworth filter order=2 alpha1=0.075 (FC: 150, Fs: 2.5kHz)
short stepLPF_orde2(int x)
{
	v[0] = v[1];
	v[1] = v[2];
	long tmp = ((((x *  24186L) >>  5)	//= (   4.6131802093e-2 * x)
		+ (v[0] * -8058L)			//+( -0.4918122372*v[0])
		+ (v[1] * 21419L))		//+(  1.3072850288*v[1])
		+ 8192) >> 14; 				// round and downshift fixed point /16384

	v[2]= (int)tmp;
	return (short)(((v[0] + v[2]) +2 * v[1])); // 2^
}*/

/*
//Low pass butterworth filter order=1 alpha1=0.08 (FC: 200, Fs: 2.5kHz)
short stepLPF_orde2(int x)
{
	v[0] = v[1];
	long tmp = (((x *   6694L)	//= (   2.0430082430e-1 * x)
		+ (v[0] * 19379L))	//+(  0.5913983514*v[0])
		+16384) >> 15; // round and downshift fixed point /32768

	v[1]= (int)tmp;
	return (int)(((v[0] + v[1]))); // 2^
}*/


/*
//Low pass butterworth filter order=1 alpha1=0.2 (FC: 500, Fs: 2.5kHz)
short stepLPF_orde2(int x)
{
	v[0] = v[1];
	long tmp = (((x *  13789L)	//= (   4.2080777984e-1 * x)
		+ (v[0] * 5190L))	//+(  0.1583844403*v[0])
		+ 16384) >> 15; // round and downshift fixed point /32768

	v[1]= (int)tmp;
	return (int)(((v[0] + v[1]))); // 2^
}*/

/*
//High pass butterworth filter order=1 alpha1=0.32 (FC: 800, Fs: 2.5kHz)
short stepHPF_orde2(int x)
{
	v1[0] = v1[1];
	long tmp = (((x *  12721L)	//= (   3.8823675855e-1 * x)
		+ (v1[0] * -7325L))	//+( -0.2235264829*v[0])
		+ 16384) >> 15; // round and downshift fixed point /32768

	v1[1]= (int)tmp;
	return (int)(((v1[1] - v1[0]))); // 2^
}*/

/*
//Low pass butterworth filter order=1 alpha1=0.125 (FC: 500, Fs: 4kHz)
short stepLPF_orde2(int x)
{
	v[0] = v[1];
	long tmp = (((x *   9597L)	//= (   2.9289321881e-1 * x)
		+ (v[0] * 13573L))	//+(  0.4142135624*v[0])
		+ 16384) >> 15; // round and downshift fixed point /32768

	v[1]= (int)tmp;
	return (int)(((v[0] + v[1]))); // 2^
}*/

/*
//Low pass butterworth filter order=1 alpha1=0.125 (FC: 600, Fs: 4kHz)
short stepLPF_orde2(int x)
{
	v[0] = v[1];
	long tmp = (((x *  11060L)	//= (   3.3754015188e-1 * x)
		+ (v[0] * 10647L))	//+(  0.3249196962*v[0])
		+ 16384) >> 15; // round and downshift fixed point /32768

	v[1]= (int)tmp;
	return (int)(((v[0] + v[1]))); // 2^
}*/

/*
//Low pass butterworth filter order=1 alpha1=0.2  (FC: 800, Fs: 4kHz)
short stepLPF_orde2(int x)
{
	v[0] = v[1];
	long tmp = (((x *  13789L)	//= (   4.2080777984e-1 * x)
		+ (v[0] * 5190L))	//+(  0.1583844403*v[0])
		+ 16384) >> 15; // round and downshift fixed point /32768

	v[1]= (int)tmp;
	return (int)(((v[0] + v[1]))); // 2^
}*/

/*
//Low pass butterworth filter order=1 alpha1=0.25  (FC: 1000, Fs: 4kHz)
short stepLPF_orde2(int x)
{
	v[0] = v[1];
	long tmp = (((x *  16384L)	//= (   5.0000000000e-1 * x)
		+ ((v[0] * 0)))	//+(  0.0000000000*v[0])
		+ 16384) >> 15; // round and downshift fixed point /32768

	v[1]= (int)tmp;
	return (int)(((v[0] + v[1]))); // 2^
}*/

//Low pass butterworth filter order=2 alpha1=0.2   (FC: 800, Fs: 4kHz)
short stepLPF_orde2(int x)
{
	v[0] = v[1];
	v[1] = v[2];
	long tmp = (((x *   3384L)	//= (   2.0657208383e-1 * x)
		+ (v[0] * -3209L)	//+( -0.1958157127*v[0])
		+ (v[1] * 6054L))	//+(  0.3695273774*v[1])
		+ 8192) >> 14; // round and downshift fixed point /16384

	v[2]= (int)tmp;
	return (int)(((v[0] + v[2])+2 * v[1])); // 2^
}

/*
//Low pass butterworth filter order=2 alpha1=0.25  (FC: 1000, Fs: 4kHz)
short stepLPF_orde2(int x)
{
	v[0] = v[1];
	v[1] = v[2];
	long tmp = (((x *   4798L)	//= (   2.9289321881e-1 * x)
		+ (v[0] * -2811L)	//+( -0.1715728753*v[0])
		+ ((v[1] * 0)))	//+(  0.0000000000*v[1])
		+ 8192) >> 14; // round and downshift fixed point /16384

	v[2]= (int)tmp;
	return (int)(((v[0] + v[2])+2 * v[1])); // 2^
}*/

/* ======================== End IIR Filter Audio INPUT - Code ===================== */
#endif
