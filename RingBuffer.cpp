//
//  RingBuffer.cpp
//  ReplicantDelay
//
//  Created by Brad Guesman on 1/9/18.
//

#include "RingBuffer.h"
#include <stdio.h>

double RingBuffer::read() {
    double toRet = *readPtr;
    readPtr = incrementPointer(readPtr, 1);
    return toRet;
}

double RingBuffer::readFractional() {
    double multiplier = rwOffset - (int) rwOffset;
    double toRet = (*readPtr * (1.0 - multiplier)) + (*incrementPointer(readPtr, -1) * multiplier);
    readPtr = incrementPointer(readPtr, 1);
    return toRet;
}

double RingBuffer::readAtOffsetLinearInterpolation(double offset) {
    double multiplier;
    double *lowerIndexReadPtr;
    double *higherIndexReadPtr;
    // Since doubles always round toward zero, we need to handle separate cases for the sign of the offset. The swap between the two offset read pointers between the cases is so that the lower indexed pointer is well... always the one with the lower index. This makes applying the multiplier much simpler.
    if (offset < 0) {
        higherIndexReadPtr = incrementPointer(readPtr, offset);
        lowerIndexReadPtr = incrementPointer(higherIndexReadPtr, -1);
    } else {
        higherIndexReadPtr = incrementPointer(readPtr, offset);
        lowerIndexReadPtr = incrementPointer(higherIndexReadPtr, 1);
    }
    multiplier = rwOffset + offset - (int) (rwOffset + offset);
    readPtr = incrementPointer(readPtr, 1);
    return *lowerIndexReadPtr * multiplier + *higherIndexReadPtr * (1.0 - multiplier);
}

double RingBuffer::readAtOffsetHermiteInterpolation(double offset) {
    double c0, c1, c2, c3, b0, b1, b2, b3, fraction;
    fraction = rwOffset + offset - ((int) (rwOffset + offset));
    b0 = *incrementPointer(readPtr, offset);
    b1 = *incrementPointer(readPtr, offset + 1);
    b2 = *incrementPointer(readPtr, offset + 2);
    b3 = *incrementPointer(readPtr, offset + 3);
    c0 = b1;
    c1 = (1.0/2.0)*(b2-b0);
    c2 = (b0 - (5.0/2.0)*b1) + (2.0*b2 - (1.0/2.0)*b3);
    c3 = (1.0/2.0)*(b3-b0) + (3.0/2.0)*(b1-b2);
    readPtr = incrementPointer(readPtr, 1);
    return ((c3*(fraction)+c2)*(fraction)+c1)*(fraction)+c0;
}

void RingBuffer::setrwOffset(double newOffset) {
    if (newOffset >= bufferSize || newOffset < 1) {
        fprintf(stderr, "error: new offset greater than buffer size\n");
        return;
    }
    int offset;
    double *resamplingPointer;
    for (int i = 0; i < newOffset; i++) {
        offset = rwOffset * ((1.0 * i) / newOffset);
        resamplingPointer = incrementPointer(readPtr, offset);
        resamplingBuffer[i] = *resamplingPointer;
    }
    smooth(&resamplingBuffer[0], newOffset, 1);
    for (int i = 0; i < newOffset; i++) {
        *incrementPointer(readPtr, i) = resamplingBuffer[i];
    }
    writePtr = incrementPointer(readPtr, newOffset);
    rwOffset = newOffset;
}

// meant to be used with small values for pointNum
void RingBuffer::smooth(double *toSmooth, int nFrames, int pointNum) {
    // we'll use a rectangular smoothing algorithm, using 2 * pointNum + 1 points for the smoothing
    double acc = 0.0;
    // for the bulk of the samples
    for (int i = pointNum; i <= nFrames - pointNum; i++) {
        for (int j = i - pointNum; j < i + pointNum; j++) {
            acc = toSmooth[j] + acc;
        }
        toSmooth[i] = (acc * 1.0) / (2.0 * pointNum);
        acc = 0.0;
    }
}
