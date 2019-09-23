#include "MfccSettings.h"
#include "BlockMaker.h"
#include "Block.h"

#include <stdio.h>
#include <stdlib.h>

// Lockfree stack & threading from boost libraries. 
#include <boost/lockfree/stack.hpp>
#include <boost/thread.hpp>

// Openal
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

// Constructors
BlockMaker::BlockMaker(boost::lockfree::queue<Block>* blocks_arg, int input_arg) {
	blocks = blocks_arg;
	inputSource = input_arg;
	continueRun = true;
	idNumber = 0;
	sleep_status = false;
}

void BlockMaker::run() {
	continueRun = true; // Flag for smooth stop
	if (inputSource == 1) {
		internalThread_ = boost::thread(&BlockMaker::openAlInput, this);
	}
	else if (inputSource == 2) {
		internalThread_ = boost::thread(&BlockMaker::stdioInput, this);
	}
	else if (inputSource == 3) {
			internalThread_ = boost::thread(&BlockMaker::wavInput, this);
	}
}

////
//// Different Block generators
////

// Openal

void BlockMaker::openAlInput() {

	int16_t buffer[N_NEW]; // Input buffer

    double over_buffer[N_OVER]; // Temp buffer for overlap
    for (int i = 0; i <= N_OVER; i++) {
    	over_buffer[i] = 0.0;
    }

    double prev_sample = 0.0; // Temp for emphasis

    // prepare system for recording
    ALint sample;
    ALCdevice *device = alcCaptureOpenDevice(NULL, SAMPLE_RATE, AL_FORMAT_MONO16, N_NEW);
    if (alcGetError(device) != AL_NO_ERROR) {
        std::cout << "OpenAL error: " << alcGetError(device) << "\n";
    }
    ALCcontext *ctx;
    ctx = alcCreateContext(device, NULL);
    alcMakeContextCurrent(ctx);
    alcCaptureStart(device);
    // beginning of the recording loop
    while (continueRun == true) {
        while (sample < N_NEW && continueRun == true) { // loop until we get enough samples
            alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALCuint), &sample);
            if (alcGetError(device) != AL_NO_ERROR) {
                std::cout << "OpenAL error: " << alcGetError(device) << "\n";
            }
        }
        alcCaptureSamples(device, (ALCvoid *)buffer, sample);
        sample = 0;

        // Generate new block from temp buffer and input buffer
        Block new_block;
		for(int i = 0; i < N_OVER; i++) {
				new_block.setAudioData(over_buffer[i], i);
				}
		for(int i = N_OVER; i < N-N_OVER; i++) {
				//std::cout << double(buffer[i-N_OVER]/32768.0) << "\n";
				new_block.setAudioData(double(buffer[i-N_OVER]), i); //32768.0
		}
		for(int i = (N-N_OVER); i < N; i++) {
				new_block.setAudioData(double(buffer[i-N_OVER]), i); //32768.0
				over_buffer[i-N_NEW] = double(buffer[i-N_OVER]); //32768.0
		}
		new_block.setId(++idNumber); // Add id
		for(int i = 0; i < N; i++) {
				//std::cout << i << ": "<< new_block.getAudioData(i) << "\n";
			    new_block.setEmpData(new_block.getAudioData(i) - 0.95 * prev_sample, i);
				prev_sample = new_block.getAudioData(i);
				//std::cout << i << ": "<< new_block.getEmpData(i) << "\n";
				double hamm = 0.54 - 0.46 * cos(( 2 * PI * i)/(N - 1));
				new_block.setWinData(new_block.getEmpData(i)*hamm, i);
				std::cout << new_block.getId() << ": "<< i << "b: "<< new_block.getWinData(i) << "\n";
		}
		blocks->push(new_block); // Push to next line (fft)
    }
    // Cleanup
    alcCaptureStop(device);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(ctx);
    alcCaptureCloseDevice(device);
}

//// Stdin

void BlockMaker::stdioInput() {
        int i, nread; // nread == number of samples read
        uint16_t *iobuf; // input buffer
        double over_buffer[N_OVER]; // Overlap buffer
        for (i = 0; i < N_OVER; i++) {
        	over_buffer[i] = 0.0;
        }
        double prev_sample = 0.0;

        iobuf = (uint16_t*) calloc(sizeof(uint16_t), N_NEW); //reserve space
        //fread (iobuf, sizeof(uint16_t), 24, stdin); // Skip header if needed

        // Generate new block from temp buffer and input buffer
        Block new_block;
		while( (nread = fread(iobuf, sizeof(uint16_t), N_NEW, stdin)) == N_NEW ) {
			for(i = 0; i < N_OVER; i++) {
				new_block.setAudioData(over_buffer[i], i);
			}
			for(i = N_OVER; i < N-N_OVER; i++) {
				new_block.setAudioData(double(iobuf[i-N_OVER]/ 32768.0), i);
			}
			for(i = (N-N_OVER); i < N; i++) {
				new_block.setAudioData(double(iobuf[i-N_OVER]/ 32768.0), i);
				over_buffer[i-N_NEW] = double(iobuf[i-N_OVER]/ 32768.0);
			}
			for(int i = 0; i < N; i++) {
				//std::cout << i << ": "<< new_block.getAudioData(i) << "\n";
				new_block.setEmpData(new_block.getAudioData(i) - 0.95 * prev_sample, i);
				prev_sample = new_block.getAudioData(i);
				//std::cout << i << ": "<< new_block.getEmpData(i) << "\n";
				double hamm = 0.54 - 0.46 * cos(( 2 * PI * i)/(N - 1));
				new_block.setWinData(new_block.getEmpData(i)*hamm, i);
				//std::cout << i << ": "<< new_block.getWinData(i) << "\n";
			}
			new_block.setId(++idNumber); // Add id
			blocks->push(new_block); // Push to next line (fft)
		}
		free(iobuf); // Cleanup
}

void BlockMaker::wavInput() {
	//Reads wav from input.wav

	short int buff16[N_NEW]; // Data buffer
    double over_buffer[N_OVER]; // Overlap buffer
    for (int i = 0; i < N_OVER; i++) {
    	over_buffer[i] = 0.0;
    }
    double prev_sample = 0.0;
    int i;

    // variables for wav header
	char ChunkID[4], Format[4], Subchunk1ID[4],Subchunk2ID[4];
	int ChunkSize,Subchunk1Size, SampleRate, ByteRate,Subchunk2Size;
	short AudioFormat, NumChannels, BlockAlign, BitsPerSample;

	// Read the wave file
	FILE *fhandle=fopen("input.wav","rb");
	fread(ChunkID,1,4,fhandle);
	fread(&ChunkSize,4,1,fhandle);
	fread(Format,1,4,fhandle);
	fread(Subchunk1ID,1,4,fhandle);
	fread(&Subchunk1Size,4,1,fhandle);
	fread(&AudioFormat,2,1,fhandle);
	fread(&NumChannels,2,1,fhandle);
	fread(&SampleRate,4,1,fhandle);
	fread(&ByteRate,4,1,fhandle);
	fread(&BlockAlign,2,1,fhandle);
	fread(&BitsPerSample,2,1,fhandle);
	fread(&Subchunk2ID,1,4,fhandle);
	fread(&Subchunk2Size,4,1,fhandle);
	//Data=new int [Subchunk2Size/(BitsPerSample/8)];
	//fread(Data,BitsPerSample/8,Subchunk2Size/(BitsPerSample/8),fhandle);
	int nb; // number of bytes read
	while ((nb = fread(buff16,1,N_NEW*2,fhandle)) > 0 && continueRun == true) { // While there is something to read
        // Generate new block from temp buffer and input buffer
		Block new_block;
		if (nb < N_NEW) {
			continueRun = false;
			for (i = nb; i < N_NEW; i ++) {
				buff16[i] = 0;
			}
		}
		for(i = 0; i < N_OVER; i++) {
			new_block.setAudioData(over_buffer[i], i);
		}
		for(i = N_OVER; i < N-N_OVER; i++) {
			new_block.setAudioData(double(buff16[i-N_OVER]), i); // 32768.0
		}
		for(i = N-N_OVER; i < N; i++) {
			new_block.setAudioData(double(buff16[i-N_OVER]), i);
			over_buffer[i-N_NEW] = double(buff16[i-N_OVER]);

		}
		for(int i = 0; i < N; i++) {
			//std::cout << i << ": "<< new_block.getAudioData(i) << "\n";
			new_block.setEmpData(new_block.getAudioData(i) - 0.95 * prev_sample, i);
			prev_sample = new_block.getAudioData(i);
			//std::cout << i << ": "<< new_block.getEmpData(i) << "\n";
			double hamm = 0.54 - 0.46 * cos(( 2 * PI * i)/(N - 1));
			new_block.setWinData(new_block.getEmpData(i)*hamm, i);
			//std::cout << i << ": "<< new_block.getWinData(i) << "\n";
		}
		new_block.setId(++idNumber);
		blocks->push(new_block);
	}
	sleep_status = true;
	fclose(fhandle); //cleanup
}

bool BlockMaker::sleeping(){
	return this->sleep_status;
}

void BlockMaker::stop(){
	continueRun = false;
}
