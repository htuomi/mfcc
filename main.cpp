#include <boost/lockfree/queue.hpp>

#include "MfccSettings.h"
#include "BlockMaker.h"
#include "FftTransform.h"
#include "MelAnalyzer.h"
#include "Pusher.h"
#include "Block.h"

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

bool RUNNING = 1;

void invalidInput(char *prog) {
	// Complain of bad commandline argument or no argument at all.
	std::cout<<"Usage: "<< prog <<" <input number>\n";
	std::cout<<"1. OpenAl\n";
	std::cout<<"2. Stdio\n";
	std::cout<<"3. input.wav\n";
}

void stop_handler(int s) {
	printf("received signal %d\n", s);
	RUNNING = 0;
}

int main(int argc, char *argv[]) {
	// Check for correct input
	int input = 0;
	if ( argc != 2 ) {
		invalidInput(argv[0]);
		return 1;
	}
	else {
		// argv[1] should be number between 1-3
		try {
			input = atoi(argv[1]);
		}
		catch (int e) {
			std::cout<<"Invalid input!\n";
			invalidInput(argv[0]);
			return 1;
		}
		if ( input < 1 or input > 3 ) {
			std::cout<<"Invalid input!\n";
			invalidInput(argv[0]);
			return 1;
		}
	}

	std::cout << "Press control+c to stop recognition!\n";
	sleep(5);

    // Information channels between different objects
    boost::lockfree::queue<Block> blocks(0);
    boost::lockfree::queue<Block> ffts(0);
    boost::lockfree::queue<Block> analyzed(0);

    // Objects
    BlockMaker bl(&blocks, input); // Data input handling
    FftTransform ft(&blocks, &ffts); // FFT things
    MelAnalyzer ma(&ffts, &analyzed); // Mel things
    Pusher pu(&analyzed); // Results giveout to VQ

    bl.run();
    ft.run();
    ma.run();
    pu.run();

    // Waiting
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = stop_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    while (RUNNING == true) {
    	sleep(1);
    }

    bl.stop();
    ft.stop();
    ma.stop();
    pu.stop();

    //while (true) {
    //    sleep(60);
    //}
    return 0;
}
