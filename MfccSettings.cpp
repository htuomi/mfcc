#include "MfccSettings.h"

MfccSettings::MfccSettings() {
	N      = 512; //Window size in samples
	N_NEW  = 312;
	N_OVER = 200;
	M_NUMCH = 28;
	M_COEFF = 13;
	SAMPLE_RATE = 22050.0;
}


