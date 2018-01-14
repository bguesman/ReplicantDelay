//
//  Oscillator.cpp
//  ReplicantDelay
//
//  Created by Brad Guesman on 12/31/17.
//

#include "Oscillator.h"
double Oscillator::mSampleRate = 44100.0;

void Oscillator::setMode(OscillatorMode mode) {
    mOscillatorMode = mode;
}

void Oscillator::updateIncrement() {
    double pitchModAsFrequency = pow(2.0, fabs(mPitchMod) * 14.0) - 1;
    if (mPitchMod < 0) {
        pitchModAsFrequency = -pitchModAsFrequency;
    }
    double calculatedFrequency = fmin(fmax(mFrequency + pitchModAsFrequency, 0), mSampleRate/2.0);
    mPhaseIncrement = calculatedFrequency * 2 * mPI / mSampleRate;
}

void Oscillator::setFrequency(double frequency) {
    mFrequency = frequency;
    updateIncrement();
}

void Oscillator::setSampleRate(double sampleRate) {
    mSampleRate = sampleRate;
    updateIncrement();
}

double Oscillator::nextSample() {
    double value = 0.0;
    switch (mOscillatorMode) {
        case OSCILLATOR_MODE_SINE:
            value = sin(mPhase);
            break;
        case OSCILLATOR_MODE_SAW:
            value = 1.0 - (2.0 * mPhase / twoPI);
            break;
        case OSCILLATOR_MODE_SQUARE:
            value = (mPhase <= mPI) ? 1.0 : -1.0;
            break;
        case OSCILLATOR_MODE_TRIANGLE:
            value = 2.0 * (fabs(-1.0 + (2.0 * mPhase / twoPI)) - 0.5);
            break;
        default:
            value = 0.0;
    }
    mPhase += mPhaseIncrement;
    while (mPhase >= twoPI) {
        mPhase -= twoPI;
    }
    return value;
}

void Oscillator::setPitchMod(double amount) {
    mPitchMod = amount;
    updateIncrement();
}

// deprecated
void Oscillator::generate(double *buffer, int nFrames) {
    const double twoPI = 2 * mPI;
    switch (mOscillatorMode) {
        case OSCILLATOR_MODE_SINE:
            for (int i = 0; i < nFrames; i++) {
                buffer[i] = sin(mPhase);
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) { // fmod is probably better here
                    mPhase -= twoPI;
                }
            }
            break;
        case OSCILLATOR_MODE_SAW:
            for (int i = 0; i < nFrames; i++) {
                buffer[i] = 1.0 - (2.0 * mPhase / twoPI);
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) { // fmod is probably better here
                    mPhase -= twoPI;
                }
            }
            break;
        case OSCILLATOR_MODE_SQUARE:
            for (int i = 0; i < nFrames; i++) {
                buffer[i] = (mPhase <= mPI) ? 1.0 : -1.0;
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) { // fmod is probably better here
                    mPhase -= twoPI;
                }
            }
            break;
        case OSCILLATOR_MODE_TRIANGLE:
            for (int i = 0; i < nFrames; i++) {
                buffer[i] = 2.0 * (fabs(-1.0 + (2.0 * mPhase / twoPI)) - 0.5);
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) { // fmod is probably better here
                    mPhase -= twoPI;
                }
            }
            break;
    }
}
