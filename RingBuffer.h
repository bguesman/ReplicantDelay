//
//  RingBuffer.h
//  ReplicantDelay
//
//  Created by Brad Guesman on 1/9/18.
//

#ifndef RingBuffer_h
#define RingBuffer_h

class RingBuffer {
    
public:
    // constructor
    RingBuffer(int size, double rwOffset) :
    bufferSize(size),
    rwOffset(rwOffset) {
        this->bufferStart = new double[bufferSize];
        this->resamplingBuffer = new double[bufferSize];
        this->readPtr = this->bufferStart;
        this->bufferEnd = bufferStart + bufferSize;
        this->writePtr = incrementPointer(readPtr, (int) rwOffset); // will round down
    }
    
    // destructor
    ~RingBuffer() { delete(bufferStart); }
    
    // Reads from buffer, increments by 1. Does not use linear interpolation; in other words, assumes maximum granularity of delay is sample.
    double read();
    
    // Reads from buffer, increments by 1. Uses linear interpolation
    // to account for fractional delay. Used for delay lines without modulation that still take advantage of fractional delay times.
    double readFractional();
    
    // Reads at (potentially fractional) offset, using linear interpolation. Used for delay lines with modulation.
    double readAtOffsetLinearInterpolation(double offset);
    
    // Reads at (potentially fractional) offset, using Hermite interpolation. Used for delay lines with modulation where fewer digital artifacts are desired.
    double readAtOffsetHermiteInterpolation(double offset);
    
    // writes to buffer, increments by 1
    inline void write(double toWrite) {
        *writePtr = toWrite;
        writePtr = incrementPointer(writePtr, 1);
    }
    
    // Sets the rwOffset, resampling in the process. Utilizes linear interpolation to smooth
    // out the resampled waveform.
    void setrwOffset(double newOffset);
    
    // Clears the buffer (i.e. make every entry 0.0)
    inline void clearBuffer() {
        for (int i = 0; i < bufferSize; i++) {
            bufferStart[i] = 0.0;
        }
    }
    
private:
    // pointer to start of memory allocated for the buffer
    double *bufferStart;
    // pointer to the end of memory allocated for the buffer
    double *bufferEnd;
    // the size of the buffer
    int bufferSize;
    // the space between the read and write pointers, potentially
    // fractionals
    double rwOffset;
    // place to read from in the buffer
    double *readPtr;
    // place to write to in the buffer
    double *writePtr;
    // a buffer for handling resampling
    double *resamplingBuffer;
    // increments a buffer pointer, wrapping around if the end of the buffer is reached
    inline double *incrementPointer(double *p, int toAdd) {
        if (p + toAdd > bufferEnd) {
            return bufferStart + (p + toAdd - bufferEnd);
        } else if (p + toAdd < bufferStart) {
            return bufferEnd - (bufferStart - (p + toAdd));
        } else {
            return p + toAdd;
        }
    }
    // for resampling
    void smooth(double *toSmooth, int nFrames, int pointNum);
};

#endif /* RingBuffer_h */
