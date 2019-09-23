#ifndef PUSHER_H_
#define PUSHER_H_

#include "vq.h"
#include "Block.h"
#include "MfccSettings.h"

#include <boost/lockfree/queue.hpp>
#include <boost/thread.hpp>

class Pusher : public MfccSettings{
public:
	Pusher(boost::lockfree::queue<Block>* input_arg);
	void run();
	void stop();
private:
	VQsyst VQ;
	void pushBlock();
	bool continueRun;
	boost::thread internalThread_;
	boost::lockfree::queue<Block>* inputStack;
};

#endif /* PUSHER_H_ */
