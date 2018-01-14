//
//  AnalogDistortion.h
//  ReplicantDelay
//
//  Created by Brad Guesman on 1/7/18.
//

#ifndef AnalogDistortion_h
#define AnalogDistortion_h

#include <cmath>

class AnalogDistortion {
public:
    enum ClippingType {
        CLIPPING_TYPE_DIGITAL,
        CLIPPING_TYPE_EXPONENTIAL
    };
    AnalogDistortion() :
    threshold(1.0),
    clippingType(CLIPPING_TYPE_DIGITAL) {};
    double process(double sample);
    inline void setThreshold(double newThreshold) { threshold = newThreshold; }
    inline void setClippingType(ClippingType newClippingType) { clippingType = newClippingType; }
private:
    double threshold;
    ClippingType clippingType;
};

#endif /* AnalogDistortion_h */
