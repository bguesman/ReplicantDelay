//
//  AnalogDistortion.cpp
//  ReplicantDelay
//
//  Created by Brad Guesman on 1/7/18.
//

#include "AnalogDistortion.h"

double AnalogDistortion::process(double sample) {
    switch (clippingType) {
        case CLIPPING_TYPE_EXPONENTIAL: {
            if (sample > 0)
                return 1 - exp(-sample);
            else
                return -1 + exp(sample);
        }
        default: {
            return fmin(threshold, sample);
        }
    }
}
