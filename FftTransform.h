#ifndef FFTTRANSFORM_H_
#define FFTTRANSFORM_H_

//#include "MfccModule.h"
#include "Block.h"
#include "MfccSettings.h"

#include <boost/lockfree/queue.hpp>
#include <boost/thread.hpp>
#include <fftw3.h>

class FftTransform : public MfccSettings {
public:
	FftTransform(boost::lockfree::queue<Block>* input_arg, boost::lockfree::queue<Block>* output_arg);
	void run();
	void stop();
	~FftTransform();
private:
	void doFft();
	boost::thread internalThread_;
	bool continueRun;
    double *in;
    fftw_complex *out;
    fftw_plan plan;
	boost::lockfree::queue<Block>* inputStack;
	boost::lockfree::queue<Block>* outputStack;
};

#endif /* FFTTRANSFORM_H_ */
