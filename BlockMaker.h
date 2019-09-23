#ifndef BLOCKMAKER_H_
#define BLOCKMAKER_H_

//#include "MfccModule.h"
#include "Block.h"
#include "MfccSettings.h"
#include <stdint.h>

#include <boost/lockfree/queue.hpp>
#include <boost/thread.hpp>

class BlockMaker : public MfccSettings {
public:
	BlockMaker(boost::lockfree::queue<Block>* blocks_arg, int input_arg);
	void run();
	void stop();
	bool getStatus();
	bool sleeping();
	//~BlockMaker();
private:
	int inputSource;
	bool continueRun;
	bool sleep_status;
	boost::thread internalThread_;
	void openAlInput();
	void stdioInput();
	void wavInput();

	boost::lockfree::queue<Block>* blocks;
	int idNumber;
	double static const PI = 3.141592653589793238463;
};

#endif /* BLOCKMAKER_H_ */
