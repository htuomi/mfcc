#ifndef BLOCK_H_
#define BLOCK_H_

class Block {
public:
	// Constructors
	Block();
	// Setters
	void setAudioData(double audio_arg, int step);
	void setEmpData (double emp_arg, int step);
	void setWinData(double win_arg, int step);
	void setFFTData (double fft_arg, int step);
	void setMelData(double mel_arg, int step);
	void setCpsData(double cps_arg, int step);
	void setId(int id_arg);
	// Getters
	int getId();
	double getAudioData(int step);
	double getEmpData (int step);
	double getWinData(int step);
	double getFFTData (int step);
	double getMelData(int step);
	double getCpsData(int step);
private:
	int id;
	double audio_data[512];
	double emphasized_data[512];
	double windowed_data[512];
	double fft_data[256];
	double mel_data[26];
	double cps_data[13];
};

#endif /* BLOCK_H_ */
