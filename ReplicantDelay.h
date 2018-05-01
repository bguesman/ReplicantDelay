#ifndef __REPLICANTDELAY__
#define __REPLICANTDELAY__

#include "IPlug_include_in_plug_hdr.h"
#include "Filter.h"
#include "Oscillator.h"
#include "AnalogDistortion.h"
#include "RingBuffer.h"

#define MAX_DELAY_TIME_MS 3000
#define MIN_DELAY_TIME_MS 35
#define MAX_DELAY_MOD_TIME_MS 10
#define BUFFER_SIZE (int) ((MAX_DELAY_TIME_MS / 1000.0) * 44100.0 * 10)
#define DEFAULT_DELAY_TIME 1000
#define MAX_NUM_DELAY_LINES 2
#define LEFT_INDEX 0
#define RIGHT_INDEX 1
#define NUM_SYNC_PARAMS 10

class ReplicantDelay : public IPlug
{
public:
  ReplicantDelay(IPlugInstanceInfo instanceInfo);
  ~ReplicantDelay();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  inline void clearRingBuffers() {
    for (int i = 0; i < NInChannels(); i++) {
      buffers[i]->clearBuffer();
    }
  }

private:
  // parameters
  double delaysInSamples[MAX_NUM_DELAY_LINES];
  double feedback[MAX_NUM_DELAY_LINES];
  double mix[MAX_NUM_DELAY_LINES];
  double delayModInSamples[MAX_NUM_DELAY_LINES];
  long int timesSinceLastTaps[MAX_NUM_DELAY_LINES];
  bool syncParams[NUM_SYNC_PARAMS];
  bool tempoSync[2 * MAX_NUM_DELAY_LINES]; // first are delay, then modulation
  bool tempoSyncGraphicsLoaded; // graphics loaded yet?
  double tempo;
  
  // ring buffers
  RingBuffer **buffers;
  
  // for hi-cut/lo-cut functionality
  Filter *lowpassFilters;
  Filter *hipassFilters;
  
  // for modulation functionality
  Oscillator *modLfos;
  
  // for distortion functionality
  AnalogDistortion *distortions;
  
  // for "analog" meter functionality
  Filter *feedbackFilters;
  
  // functions for constructor
  void CreatePresets();
  void CreateParams();
  void CreateGraphics();
  
  // for tempo sync functionality
  double noteToSamples(int t);
  
  // convert a delay in ms to samples
  inline double msToSamples(double ms) {
    return (ms / 1000.0) * GetSampleRate();
  }
  
  // convert a delay in samples to ms
  inline double samplesToMs(double samples) {
    return (samples * 1000.0) / GetSampleRate();
  }
  
  // update the buffer pointers when the delay is changed
  void updateBufferPointers(double newDelay);
  
  // utility for "reflecting" a quantized delay enum across the middle of the list
  double invertQuantizedDelayEnum(int toInvert);
  
};

#endif
