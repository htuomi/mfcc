#include "Pusher.h"
#include "Block.h"
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include  <signal.h>

#include <boost/lockfree/queue.hpp>
#include <boost/lexical_cast.hpp>

Pusher::Pusher(boost::lockfree::queue<Block>* input_arg) {
	inputStack = input_arg;
	continueRun = true;
}

void Pusher::run(){
	continueRun = true;
	VQ.LoadClusters();
	char *out = "out.txt";
	if (VQ.InitOutput(out)==FAILURE) std::cout << "ERROR: Unable make VQ log\n";
	internalThread_ = boost::thread(&Pusher::pushBlock, this);
}

void Pusher::pushBlock() {
    while (continueRun == true) {
		while (continueRun == true) {
			Block ready;
			if (inputStack->pop(ready) == false) break;
			int i = (VQ.Assigned)+1;
			for (int j=1;j < M_COEFF;j++) {
				VQ.Pattern[i][j-1]=ready.getCpsData(j);
			}
			VQ.RunVQ(i, ready.getCpsData(0));
		}
    }
}

void Pusher::stop() {
	continueRun = false;
	VQ.UpdateDB();
}
