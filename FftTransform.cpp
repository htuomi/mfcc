#include "FftTransform.h"
#include "MfccSettings.h"
#include "Block.h"

#include <fftw3.h>
#include <boost/lockfree/queue.hpp>
#include <boost/thread.hpp>

// Constructor
FftTransform::FftTransform(boost::lockfree::queue<Block>* input_arg, boost::lockfree::queue<Block>* output_arg) {
	inputStack = input_arg; // From Blockmaker
	outputStack = output_arg; // To MelAnalyzer

    in = (double*) fftw_malloc(sizeof(double) * N);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (N/2+1));
    plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_MEASURE); //FFTW_PATIENT

    continueRun = true;
}

void FftTransform::run() {
    continueRun = true;
	internalThread_ = boost::thread(&FftTransform::doFft, this);
}

void FftTransform::doFft() {
	// Makes fft transformation to audio data
    while (continueRun == true) {
		while (continueRun == true ) {
		    Block fft_block;
		    if (inputStack->pop(fft_block) == false) break; // Input new block
		    for(int i = 0; i < N; i++) { // Fill in the buffers
		        in[i] = fft_block.getWinData(i);
		    }

		    fftw_execute(plan); // Execute

		    for(int k = 0; k < (N/2); k++) {
		        fft_block.setFFTData(sqrt((out[k][0] * out[k][0]) + (out[k][1] * out[k][1])), k);
		        //std::cout << fft_block.getId() << ": " << k  << "f: "<< fft_block.getFFTData(k) << "\n";
		    }
		    outputStack->push(fft_block);
		}
    	sleep(0.5);
    }
}

void FftTransform::stop() {
	continueRun = false;
}

FftTransform::~FftTransform() {
    free(in); // Cleanup
}
