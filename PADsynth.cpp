/*
	PADsynth implementation as ready-to-use C++ class.
	By: Nasca O. Paul, Tg. Mures, Romania
	This implementation and the algorithm are released under Public Domain
	Feel free to use it into your projects or your products ;-)

	This implementation is tested under GCC/Linux, but it's
	very easy to port to other compiler/OS. */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "PADsynth.h"

PADsynth::PADsynth(int N_, int samplerate_, int number_harmonics_) {
	N = N_;
	samplerate = samplerate_;
	number_harmonics = number_harmonics_;
    A = new float[number_harmonics];
	for (int i = 0; i < number_harmonics; i++) A[i] = 0.0;
	A[1] = 1.0;//default, the first harmonic has the amplitude 1.0

    freq_amp = new float[N / 2];
};

PADsynth::~PADsynth() {
	delete[] A;
	delete[] freq_amp;
};

float PADsynth::relF(int N) {
	return N;
};

void PADsynth::setharmonic(int n, float value) {
	if ((n < 1) || (n >= number_harmonics)) return;
	A[n] = value;
};

float PADsynth::getharmonic(int n) {
	if ((n < 1) || (n >= number_harmonics)) return 0.0;
	return A[n];
};

float PADsynth::profile(float fi, float bwi) {
    float x = fi / bwi;
	x *= x;
	if (x > 14.71280603) return 0.0;//this avoids computing the e^(-x^2) where it's results are very close to zero
	return exp(-x) / bwi;
};

void PADsynth::synth(float f, float bw, float bwscale, float* smp) {
	int i, nh;

	for (i = 0; i < N / 2; i++) freq_amp[i] = 0.0;//default, all the frequency amplitudes are zero

	for (nh = 1; nh < number_harmonics; nh++) {//for each harmonic
        float bw_Hz;//bandwidth of the current harmonic measured in Hz
        float bwi;
        float fi;
        float rF = f * relF(nh);

		bw_Hz = (pow(2.0, bw / 1200.0) - 1.0) * f * pow(relF(nh), bwscale);

		bwi = bw_Hz / (2.0 * samplerate);
		fi = rF / samplerate;
		for (i = 0; i < N / 2; i++) {//here you can optimize, by avoiding to compute the profile for the full frequency (usually it's zero or very close to zero)
            float hprofile;
            hprofile = profile((i / (float)N) - fi, bwi);
			freq_amp[i] += hprofile * A[nh];
		};
	};

    float* freq_real = new float[N / 2];
    float* freq_imaginary = new float[N / 2];

	//Convert the freq_amp array to complex array (real/imaginary) by making the phases random
	for (i = 0; i < N / 2; i++) {
        float phase = RND() * 2.0 * 3.14159265358979;
		freq_real[i] = freq_amp[i] * cos(phase);
		freq_imaginary[i] = freq_amp[i] * sin(phase);
	};
	IFFT(freq_real, freq_imaginary, smp);
	delete[] freq_real;
	delete[] freq_imaginary;

	//normalize the output
    float max = 0.0;
	for (i = 0; i < N; i++) if (fabs(smp[i]) > max) max = fabs(smp[i]);
	if (max < 1e-5) max = 1e-5;
	for (i = 0; i < N; i++) smp[i] /= max * 1.4142;
};

float PADsynth::RND() {
	return (rand() / (RAND_MAX + 1.0));
};
