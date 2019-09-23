#ifndef MFCCSETTINGS_H_
#define MFCCSETTINGS_H_

#include <boost/lockfree/queue.hpp>

class MfccSettings {
public:
	MfccSettings();
protected:
	int N; //Window size in samples
	int N_NEW;
	int N_OVER;
	int M_NUMCH;
	int M_COEFF;
	int SAMPLE_RATE;
	//int F_MIN;
	//int F_MAX;
	//double PI;
};

#endif /* MFCCSETTINGS_H_ */
