#include <iostream>
#include<windows.h>
#include<math.h>
#include<complex>
#include<vector>

#include "AudioFile.h"
#include"Screen.h"

#define M_PI 3.14159265358979323846

using namespace std;

void DFT(const vector<double>& input, vector<std::complex<double>>& output, double samplepersec)
{
	int N = input.size();
	std::complex<double> img(0.0, 1.0);

	int frontbandwith = 0; //여기서부터 주파수 측정 시작
	//int jump = 10;

	//for (int i = 0; i< N; ++i)
	for (int i = frontbandwith; i < N + frontbandwith; ++i)
	{
		std::complex<double> tmp(0.0, 0.0);

		for (int j = 0; j < N; ++j)
		{
			tmp += input.at(j) * std::exp(-2.0 * M_PI * i * j * img / samplepersec);
		}

		output.push_back(tmp);
	}
}

void PrintFrequency(vector<double>& output, ScreenManager& screen) {
	const int color = 10;

	const int resolution = 25; // 분해능
	const int fsize = output.size() / resolution;
	const double amp = 0.01f;

	screen.Clear(color);

	//double f[10] = { 0.0f };
	double* f = new double[fsize];
	for (int i = 0; i < fsize; i++) {
		f[i] = 0.0f;
	}

	for (int i = 0; i < output.size(); i++) {
		f[i / resolution] += output[i] * amp;
	}

	const int space = 5; //출력시 띄울 빈칸 수

	for (int i = 0; i < fsize; i++) {
		screen.Add(i*2 + 5, HEIGHT-1, color, '=');
		for (int j = 0; j < f[i]; j++) {
			screen.Add(i*2 + space, HEIGHT - j - 2, color, '*');
		}
	}

	delete[] f;
	screen.Show();
}

int main()
{
	char FILENAME[100];
	cin >> FILENAME;

	AudioFile<double> audioFile;

	audioFile.load(FILENAME);
	
	int sampleRate = audioFile.getSampleRate();
	int bitDepth = audioFile.getBitDepth();

	int numSamples = audioFile.getNumSamplesPerChannel();
	double lengthInSeconds = audioFile.getLengthInSeconds();

	int numChannels = audioFile.getNumChannels();
	bool isMono = audioFile.isMono();
	bool isStereo = audioFile.isStereo();

	
	// or, just use this quick shortcut to print a summary to the console
	//audioFile.printSummary();


	int channel = 0;

	int WindowSize = 1000;

	int Cut = WindowSize / 4;

	vector<double> v;
	vector<std::complex<double>> output;

	vector<double> sqr_output;

	ScreenManager screen;
	
	double currentSample = 0.0f;
	//double multiply = 0.0f;

	for (int i = 0; i < numSamples; i++)
	{
		if ((i + 1) % Cut == 0) {
			for (int j = i; j < (i + WindowSize); j++) {
				currentSample = audioFile.samples[channel][j];
				v.push_back(currentSample);
			}
			DFT(v, output, sampleRate);

			for (int j = 0; j < output.size(); j++) {
				sqr_output.push_back(sqrt(output[j].real() * output[j].real() + output[j].imag() * output[j].imag()));
				//cout << j << ": " << sqr_output[j] << endl;
			}
			PrintFrequency(sqr_output, screen);
			v.clear();
			output.clear();
			sqr_output.clear();
		}
		
		/*
		if (i % WindowSize == 0) {
			DFT(v, output, sampleRate);

			for (int j = 0; j < output.size(); j++) {
				sqr_output.push_back(sqrt(output[j].real() * output[j].real() + output[j].imag() * output[j].imag()));
				//cout << j << ": " << sqr_output[j] << endl;
			}
			PrintFrequency(sqr_output, screen);
			v.clear();
			output.clear();
			sqr_output.clear();
		}
		currentSample = audioFile.samples[channel][i];
		v.push_back(currentSample);
		*/
	}
	

	//
	//multiply = 0.5 * (1 - cos(2 * M_PI * j / 2047)); //hamming window 
	//currentSample *= multiply;
	//

	//API 사용 그래픽 업데이트
	//고속 푸리에 변환 사용
	//윈도우 이용 값 증폭

}

