#include "MelAnalyzer.h"
#include "MfccSettings.h"
#include "Block.h"
#include <fftw3.h>

#include <boost/lockfree/queue.hpp>
#include <boost/thread.hpp>

MelAnalyzer::MelAnalyzer(boost::lockfree::queue<Block>* input_arg, boost::lockfree::queue<Block>* output_arg) {
	inputStack = input_arg;
	outputStack = output_arg;
	filterBank = new double * [M_NUMCH]; // Reserve space for filterbank
	for (int i = 0; i < M_NUMCH; i++) {
		filterBank[i] = new double[N/2];
	}
	buildFilterBank(); // Build the filterbank
	continueRun = true;
}

void MelAnalyzer::run() {
	continueRun = true;
	internalThread_ = boost::thread(&MelAnalyzer::doMel, this);
}

// Main function

void MelAnalyzer::doMel() {
	double* logEnergy = (double*) fftw_malloc(sizeof(double) * M_NUMCH); // DCT input
	double* cps = (double*) fftw_malloc(sizeof(double) * M_NUMCH); // DCT output
	fftw_plan q = fftw_plan_r2r_1d(M_NUMCH, logEnergy, cps, FFTW_REDFT10, FFTW_MEASURE);
	while (continueRun == true) {
		while (continueRun == true) {
			Block mel_block;
			if (inputStack->pop(mel_block) == false) break;
			// Multiplication with filterbank
			for(int i = 0; i < M_NUMCH; i++) {
				for(int k = 0; k < N/2; k++) {
					mel_block.setMelData(mel_block.getMelData(i) + mel_block.getFFTData(k)*filterBank[i][k], i);
				}
				//std::cout << i << ":" << mel_block.getMelData(i) << "\n";
				logEnergy[i] = log10(mel_block.getMelData(i)); // Take log from mel data -> log energies
				//std::cout << i << ":" << mel_block.getMelData(i) << "\n";
			}

		    fftw_execute(q); // Execute

		    for(int k = 0; k < M_COEFF; k++) {
		        mel_block.setCpsData(cps[k],k); // push cepstrums to block
		    }

			outputStack->push(mel_block); // push block to next line (Pusher)
		}
	sleep(0.5);
	}
}

// Tool functions
//// mel to freq
double melScaleInverse(double f) {
   return 700 * (pow(10, f / 2595.0) - 1);
}

//// freq to mel
double melScale(double f) {
   return 2595 * log10(1 + (f / 700.0));
}

//// Filters center and borders
void MelAnalyzer::buildFilterBank() {
	double * cf;
		//calculate the center frequencies
		cf = new double[M_NUMCH+2]; // Centers (and borders)

		//// Filters center and borders
	   for(int i = 0; i < (M_NUMCH+2); i++) {
		   cf[i] = melScaleInverse(melScale(300)+(i*((melScale(8000)-melScale(300))/(M_NUMCH + 1))));
		   cf[i] = (int)cf[i] *(N+1)/SAMPLE_RATE;
	   }

		//fill in each bank
		for (int i = 1; i < M_NUMCH+1; i++) {
			for (int j = 0; j < N/2; j++) {
				if (j < (int)cf[i-1]) {
					filterBank[i-1][j] = 0;
				}
				else if (j >= (int)cf[i-1] && j < (int)cf[i]) { // upslope
					filterBank[i-1][j] = (double)(j-cf[i-1])/(cf[i]-cf[i-1]);
				}
				else if (j >= (int)cf[i] && j < (int)cf[i+1]) { // downslope
					filterBank[i-1][j] = (double)(cf[i+1]-j)/(cf[i+1]-cf[i]);
				}
				else {
					filterBank[i-1][j] = 0;

				}
			}
		}

		delete cf;

	}

void MelAnalyzer::stop(){
	continueRun = false;
}

MelAnalyzer::~MelAnalyzer() {
	//Cleanup
	for (int i = 0; i < M_NUMCH; i++) {
		delete filterBank[i];
	}
	delete filterBank;
}
