#include "Block.h"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <math.h>

// Constructor(s)
Block::Block() {
	id = 0;
}

// Setters
void Block::setAudioData (double audio_arg, int i) {
    this->audio_data[i] = audio_arg;
}

void Block::setEmpData (double emp_arg, int step) {
    this->emphasized_data[step] = emp_arg;
}

void Block::setWinData(double win_arg, int step) {
    this->windowed_data[step] = win_arg;
}

void Block::setFFTData (double fft_arg, int step) {
    this->fft_data[step] = fft_arg;
}

void Block::setMelData(double mel_arg, int step) {
    this->mel_data[step] = mel_arg;
}

void Block::setCpsData(double cps_arg, int step) {
    this->cps_data[step] = cps_arg;
}

void Block::setId(int id_arg) {
	this->id = id_arg;
}

// Getters. Returns only one double of data -> step as an argument
double Block::getAudioData (int step) {
    return audio_data[step];
}

double Block::getEmpData (int step) {
    return emphasized_data[step];
}

double Block::getWinData(int step) {
    return windowed_data[step];
}

double Block::getFFTData (int step) {
    return fft_data[step];
}

double Block::getMelData(int step) {
    return mel_data[step];
}

double Block::getCpsData(int step) {
    return cps_data[step];
}

int Block::getId() {
	return id;
}
