//
//  Oscillator.hpp
//  ReplicantDelay
//
//  Created by Brad Guesman on 12/31/17.
//

#ifndef Oscillator_h
#define Oscillator_h

#include <math.h>

class Oscillator {
public:
    enum OscillatorMode {
        OSCILLATOR_MODE_SINE = 0,
        OSCILLATOR_MODE_SAW,
        OSCILLATOR_MODE_SQUARE,
        OSCILLATOR_MODE_TRIANGLE,
        kNumOscillatorModes
    };
    void setMode(OscillatorMode mode);
    void setFrequency(double frequency);
    void setSampleRate(double sampleRate);
    void generate(double* buffer, int nFrames);
    inline void setMuted(bool muted) { isMuted = muted; }
    inline void updatePhase(int nSamples) {
        mPhase += (nSamples * mPhaseIncrement);
        while (mPhase >= twoPI) {
            mPhase -= twoPI;
        }
        while (mPhase < 0) {
            mPhase += twoPI;
        }
    }
    double nextSample();
    void reset() { mPhase = 0.0; }
    void setPitchMod(double amount);
    Oscillator() :
    mOscillatorMode(OSCILLATOR_MODE_SINE),
    mPI(2*acos(0.0)),
    twoPI(2 * mPI),
    isMuted(true),
    mFrequency(440.0),
    mPhase(0.0),
    mPitchMod(0.0) { updateIncrement(); };
private:
    OscillatorMode mOscillatorMode;
    const double mPI;
    const double twoPI;
    bool isMuted;
    double mFrequency;
    double mPhase;
    static double mSampleRate;
    double mPhaseIncrement;
    double mPitchMod;
    void updateIncrement();
};

#endif /* Oscillator_h */
