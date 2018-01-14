//
//  Filter.h
//  ReplicantDelay
//
//  Created by Brad Guesman on 1/1/18.
//

#ifndef Filter_h
#define Filter_h

#include <cmath>

class Filter {
public:
    enum FilterMode {
        FILTER_MODE_LOWPASS = 0,
        FILTER_MODE_HIGHPASS,
        FILTER_MODE_BANDPASS,
        kNumFilterModes
    };
    Filter() :
    cutoff(0.99),
    resonance(0.01),
    cutoffMod(0.0),
    mode(FILTER_MODE_LOWPASS),
    buf0(0.0),
    buf1(0.0),
    buf2(0.0),
    buf3(0.0) {
        calculateFeedbackAmount();
    };
    double process(double inputValue);
    inline void setCutoff(double newCutoff) { cutoff = newCutoff; calculateFeedbackAmount(); }
    inline void setCutoffMod(double newCutoffMod) {
        cutoffMod = newCutoffMod;
        calculateFeedbackAmount();
    }
    inline double getCalculatedCutoff() const {
        return fmax(fmin(cutoff + cutoffMod, 0.99), 0.01);
    };
    inline void setResonance(double newResonance) { resonance = newResonance; calculateFeedbackAmount(); }
    inline void setFilterMode(FilterMode newMode) { mode = newMode; }
    void reset() {
        buf0 = buf1 = buf2 = buf3 = 0.0;
    }
    
private:
    double cutoff;
    double resonance;
    double cutoffMod;
    FilterMode mode;
    double feedbackAmount;
    inline void calculateFeedbackAmount() { feedbackAmount = resonance + resonance/(1.0 - getCalculatedCutoff()); }
    double buf0;
    double buf1;
    double buf2;
    double buf3;
};

#endif /* Filter_h */
