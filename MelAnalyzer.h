#ifndef MELANALYZER_H_
#define MELANALYZER_H_

//#include "MfccModule.h"
#include "Block.h"
#include "MfccSettings.h"

#include <boost/lockfree/queue.hpp>
#include <boost/thread.hpp>

class MelAnalyzer : public MfccSettings {
public:
	MelAnalyzer(boost::lockfree::queue<Block>* input_arg, boost::lockfree::queue<Block>* output_arg);
	void run();
	void stop();
	~MelAnalyzer();
private:
	void buildFilterBank();
	void doMel();
	bool continueRun;
	boost::thread internalThread_;
	boost::lockfree::queue<Block>* inputStack;
	boost::lockfree::queue<Block>* outputStack;
	double ** filterBank;
};

#endif /* MELANALYZER_H_ */
