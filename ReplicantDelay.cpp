#include <sys/time.h>
#include "ReplicantDelay.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include <tr1/functional>
#include "IKnobMultiControlText.h"
#include "IFaderControlText.h"

const int kNumPrograms = 5;
const double parameterStep = 0.01;

enum QuantizedDelayTime {
  SIXTY_FOURTH = 0,
  TRIPLET_THIRTY_SECOND,
  DOTTED_SIXTY_FOURTH,
  THIRTY_SECOND,
  TRIPLET_SIXTEENTH,
  DOTTED_THIRTY_SECOND,
  SIXTEENTH,
  TRIPLET_EIGHTH,
  DOTTED_SIXTEENTH,
  EIGHTH,
  TRIPLET_QUARTER,
  DOTTED_EIGHTH,
  QUARTER,
  TRIPLET_HALF,
  DOTTED_QUARTER,
  HALF,
  TRIPLET_WHOLE,
  DOTTED_HALF,
  WHOLE,
  NUM_QUANTIZED_DELAY_TIMES
};

const double quantizedTimeMultipliers[NUM_QUANTIZED_DELAY_TIMES] =
{
  1.0/64.0,
  1.0/48.0,
  3.0/128.0,
  1.0/32.0,
  1.0/24.0,
  3.0/64.0,
  1.0/16.0,
  1.0/12.0,
  3.0/32.0,
  1.0/8.0,
  1.0/6.0,
  3.0/16.0,
  1.0/4.0,
  1.0/3.0,
  3.0/8.0,
  1.0/2.0,
  2.0/3.0,
  3.0/4.0,
  1.0
};

const char *quantizedDisplayTexts[NUM_QUANTIZED_DELAY_TIMES] =
{
  "1/64",
  "1/32t",
  "1/64d",
  "1/32",
  "1/16t",
  "1/32d",
  "1/16",
  "1/8t",
  "1/16d",
  "1/8",
  "1/4t",
  "1/8d",
  "1/4",
  "1/2t",
  "1/4d",
  "1/2",
  "1/1t",
  "1/2d",
  "1/1"
};

enum EParams
{
  mLeftDelayTimeMs = 0,
  mRightDelayTimeMs,
  mLeftFeedback,
  mRightFeedback,
  mLeftMix,
  mRightMix,
  mLeftModRate,
  mRightModRate,
  mLeftModAmt,
  mRightModAmt,
  mLeftLowCut,
  mRightLowCut,
  mLeftHighCut,
  mRightHighCut,
  mLeftAnalog,
  mRightAnalog,
  mLeftDistortionThreshold,
  mRightDistortionThreshold,
  kRegParamsEnd,
  mDelayTimeSync,
  mFeedbackSync,
  mMixSync,
  mModRateSync,
  mModAmtSync,
  mLowCutSync,
  mHighCutSync,
  mAnalogSync,
  mDistortionThresholdSync,
  kSyncParamsEnd,
  mLeftTapTempo,
  mRightTapTempo,
  kTapTempoParamsEnd,
  mLeftTempoSync,
  mRightTempoSync,
  mLeftModTempoSync,
  mRightModTempoSync,
  mLeftTempoSyncDelayNote,
  mRightTempoSyncDelayNote,
  mLeftTempoSyncModNote,
  mRightTempoSyncModNote,
  kTempoSyncParamsEnd,
  kNumParams
};

typedef struct {
  const char* name;
  const int x;
  const int y;
  const double defaultDoubleVal;
  const bool defaultBoolVal;
  const double minVal;
  const double maxVal;
} parameterProperties_struct;

const parameterProperties_struct parameterProperties[kNumParams] =
{
  {.name="Left Delay", .x=61 - 36, .y=134 - 36, .defaultDoubleVal=DEFAULT_DELAY_TIME, .minVal=MIN_DELAY_TIME_MS, .maxVal=MAX_DELAY_TIME_MS},
  {.name="Right Delay", .x=417 - 36, .y=134 - 36, .defaultDoubleVal=DEFAULT_DELAY_TIME, .minVal=MIN_DELAY_TIME_MS, .maxVal=MAX_DELAY_TIME_MS},
  {.name="Left Feedback", .x=171 - 36, .y=134 - 36, .defaultDoubleVal=0.4, .minVal=0.01, .maxVal=1.5},
  {.name="Right Feedback", .x=527 - 36, .y=134 - 36, .defaultDoubleVal=0.4, .minVal=0.01, .maxVal=1.5},
  {.name="Left Mix", .x=282 - 36, .y=134 - 36, .defaultDoubleVal=0.5, .minVal=0.0, .maxVal=1.0},
  {.name="Right Mix", .x=639 - 36, .y=134 - 36, .defaultDoubleVal=0.5, .minVal=0.0, .maxVal=1.0},
  {.name="Left Mod Rate", .x=61 - 36, .y=384 - 36, .defaultDoubleVal=0.20, .minVal=0.01, .maxVal=10.0},
  {.name="Right Mod Rate", .x=418 - 36, .y=384 - 36, .defaultDoubleVal=0.20, .minVal=0.01, .maxVal=10.0},
  {.name="Left Mod Amount", .x=61 - 36, .y=503 - 36, .defaultDoubleVal=0.0, .minVal=0.0, .maxVal=1.00},
  {.name="Right Mod Amount", .x=418 - 36, .y=503 - 36, .defaultDoubleVal=0.0, .minVal=0.0, .maxVal=1.00},
  {.name="Left Low Cut", .x=31 - 18.5, .y=290 - 18.5, .defaultDoubleVal=0.0, .minVal=0.0, .maxVal=0.99},
  {.name="Right Low Cut", .x=388 - 18.5, .y=290 - 18.5, .defaultDoubleVal=0.0, .minVal=0.0, .maxVal=0.99},
  {.name="Left High Cut", .x=31 - 18.5, .y=228 - 18.5, .defaultDoubleVal=0.99, .minVal=0.0, .maxVal=0.99},
  {.name="Right High Cut", .x=388 - 18.5, .y=228 - 18.5, .defaultDoubleVal=0.99, .minVal=0.0, .maxVal=0.99},
  {.name="Left Analog", .x=31 - 18.5, .y=588 - 18.5, .defaultDoubleVal=0.0, .minVal=0.00, .maxVal=100.0},
  {.name="Right Analog", .x=388 - 18.5, .y=588 - 18.5, .defaultDoubleVal=0.0, .minVal=0.00, .maxVal=100.0},
  {.name="Left Distortion Threshold", .x=246 - 18.5, .y=363 - 18.5, .defaultDoubleVal=0.00, .minVal=0.00, .maxVal=100.0},
  {.name="Right Distortion Threshold", .x=604 - 18.5, .y=363 - 18.5, .defaultDoubleVal=0.00, .minVal=0.00, .maxVal=100.0},
  {.name="Regular Params End Placeholder"},
  {.name="Delay Time Sync", .x=104 - 10, .y=100 - 10, .defaultBoolVal=false},
  {.name="Feedback Sync", .x=214 - 10, .y=100 - 10, .defaultBoolVal=false},
  {.name="Mix Sync", .x=326 - 10, .y=100 - 10, .defaultBoolVal=false},
  {.name="Mod Rate Sync", .x=104 - 10, .y=350 - 10, .defaultBoolVal=false},
  {.name="Mod Amount Sync", .x=104 - 10, .y=471 - 10, .defaultBoolVal=false},
  {.name="Low Cut Sync", .x=326 - 10, .y=291 - 10, .defaultBoolVal=false},
  {.name="High Cut Sync", .x=326 - 10, .y=230 - 10, .defaultBoolVal=false},
  {.name="Analog Sync", .x=326 - 10, .y=589 - 10, .defaultBoolVal=false},
  {.name="Distortion Sync", .x=320 - 10, .y=453 - 10, .defaultBoolVal=false},
  {.name="Sync Params End Placeholder"},
  {.name="Left Tap Tempo", .x=173 - 41, .y=450 - 54, .defaultBoolVal=false},
  {.name="Right Tap Tempo", .x=530 - 41, .y=450 - 54, .defaultBoolVal=false},
  {.name="Tap Tempo Params End Placeholder"},
  {.name="Left Tempo Sync", .x=17 - 10, .y=99 - 10, .defaultBoolVal=false},
  {.name="Right Tempo Sync", .x=374 - 10, .y=99 - 10, .defaultBoolVal=false},
  {.name="Left Mod Tempo Sync", .x=17 - 10, .y=350 - 10, .defaultBoolVal=false},
  {.name="Right Mod Tempo Sync", .x=373 - 10, .y=350 - 10, .defaultBoolVal=false},
  {.name="Left Tempo Sync Delay Note", .x=61 - 36, .y=134 - 36},
  {.name="Right Tempo Sync Delay Note", .x=417 - 36, .y=134 - 36},
  {.name="Left Tempo Sync Mod Note", .x=61 - 36, .y=384 - 36},
  {.name="Right Tempo Sync Mod Note", .x=418 - 36, .y=384 - 36},
  {.name="Tempo Sync Params End Placeholder"},
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT
};

ReplicantDelay::ReplicantDelay(IPlugInstanceInfo instanceInfo) : IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo) {
  TRACE;
  
  // set up the filters
  lowpassFilters = new Filter[NOutChannels()];
  hipassFilters = new Filter[NOutChannels()];
  feedbackFilters = new Filter[NOutChannels()];
  for (int i = 0; i < NOutChannels(); i++) {
    lowpassFilters[i].setFilterMode(Filter::FILTER_MODE_LOWPASS);
    hipassFilters[i].setFilterMode(Filter::FILTER_MODE_HIGHPASS);
    feedbackFilters[i].setFilterMode(Filter::FILTER_MODE_LOWPASS);
  }
  
  // set up the lfo
  modLfos = new Oscillator[NOutChannels()];
  for (int i = 0; i < NOutChannels(); i++) {
    modLfos[i].setMode(Oscillator::OSCILLATOR_MODE_SINE);
  }
  
  // set up distortions
  distortions = new AnalogDistortion[NOutChannels()];
  for (int i = 0; i < NOutChannels(); i++) {
    distortions[i].setClippingType(AnalogDistortion::CLIPPING_TYPE_DIGITAL);
  }
  
  // set up the ring buffers
  buffers = new RingBuffer *[NOutChannels()];
  for (int i = 0; i < NOutChannels(); i++) {
    buffers[i] = new RingBuffer(BUFFER_SIZE, msToSamples(DEFAULT_DELAY_TIME));
  }
  
  // ensure the tempo sync graphics loaded is false
  tempoSyncGraphicsLoaded = false;
  // also ensure all the initialization values of the tempo sync array are false
  for (int i = 0; i < 2 * MAX_NUM_DELAY_LINES; i++) {
    tempoSync[i] = false;
  }
  
  // set a default value for the tempo
  tempo = 120.0;
  
  // do the plugin stuff as usual
  CreateParams();
  CreateGraphics();
  CreatePresets();
  
  // clear the buffer before we begin
  clearRingBuffers();
  
  // set the times since last taps all to the current times
  struct timeval tp;
  gettimeofday(&tp, NULL);
  long int currentTime = tp.tv_sec * 1000 + tp.tv_usec / 1000;
  // figure out the new delay time
  timesSinceLastTaps[0] = currentTime - timesSinceLastTaps[0];
}

ReplicantDelay::~ReplicantDelay() {
  for (int i = 0; i < NOutChannels(); i++) {
    delete(buffers[i]);
  }
  delete(buffers);
  delete(hipassFilters);
  delete(lowpassFilters);
  delete(feedbackFilters);
  delete(modLfos);
}

void ReplicantDelay::ProcessDoubleReplacing(
                                       double** inputs,
                                       double** outputs,
                                       int nFrames)
{
  // Mutex is already locked for us.
  // First, check if the tempo has changed, since there is no function for the
  // host to notify us
  double newTempo = GetTempo();
  if (tempo != newTempo) {
    tempo = newTempo;
    OnParamChange(mLeftDelayTimeMs);
    OnParamChange(mRightDelayTimeMs);
    OnParamChange(mLeftModRate);
    OnParamChange(mRightModRate);
  }
  double *input;
  double *output;
  RingBuffer *buffer;
  double readVal;
  for (int i = 0; i < NOutChannels(); i++) {
    if (IsInChannelConnected(1)) {
      input = inputs[i];
    } else {
      input = inputs[0];
    }
    output = outputs[i];
    buffer = buffers[i];
    for (int n = 0; n < nFrames; n++) {
      readVal = buffer->readAtOffsetHermiteInterpolation(modLfos[i].nextSample() * delayModInSamples[i]);
      output[n] = fmax(-1.0, fmin(1.0, input[n] * (1.0 - mix[i]) + lowpassFilters[i].process(hipassFilters[i].process(distortions[i].process(readVal))) * mix[i]));
      buffer->write(input[n] + feedbackFilters[i].process(readVal * feedback[i]));
    }
  }
}

void ReplicantDelay::updateBufferPointers(double newDelay) {
  for (int i = 0; i < NOutChannels(); i++) {
    buffers[i]->setrwOffset(newDelay);
  }
}

void ReplicantDelay::Reset()
{
  TRACE;
  IMutexLock lock(this);
  // need to readjust these so they stay the same across sample rate changes
  OnParamChange(mLeftDelayTimeMs);
  OnParamChange(mRightDelayTimeMs);
  OnParamChange(mLeftTempoSyncDelayNote);
  OnParamChange(mRightTempoSyncDelayNote);
  OnParamChange(mLeftModRate);
  OnParamChange(mRightModRate);
  OnParamChange(mLeftTempoSyncModNote);
  OnParamChange(mRightTempoSyncModNote);
  clearRingBuffers();
}

void ReplicantDelay::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);
  IParam* param = GetParam(paramIdx);
  int syncIndex = paramIdx / 2; // index of a left parameter's corresponding sync param
  switch (paramIdx) {
    case mLeftDelayTimeMs: {
      if (!tempoSync[LEFT_INDEX]) {
        double newDelay = (int) msToSamples(param->Value());
        buffers[0]->setrwOffset(newDelay);
        delaysInSamples[LEFT_INDEX] = newDelay;
        if (syncParams[syncIndex]) {
          GetParam(mRightDelayTimeMs)->Set(param->Value());
          OnParamChange(mRightDelayTimeMs);
          this->RedrawParamControls();
        }
      }
      break;
    }
    case mRightDelayTimeMs: {
      if (!tempoSync[RIGHT_INDEX]) {
        double newDelay = (int) msToSamples(param->Value());
        buffers[1]->setrwOffset(newDelay);
        delaysInSamples[RIGHT_INDEX] = newDelay;
      }
      break;
    }
    case mLeftFeedback: {
      feedback[LEFT_INDEX] = param->Value();
      if (syncParams[syncIndex]) {
        GetParam(mRightFeedback)->Set(param->Value());
        OnParamChange(mRightFeedback);
        this->RedrawParamControls();
      }
      break;
    }
    case mRightFeedback: {
      feedback[RIGHT_INDEX] = param->Value();
      break;
    }
    case mLeftMix: {
      mix[LEFT_INDEX] = param->Value();
      if (syncParams[syncIndex]) {
        GetParam(mRightMix)->Set(param->Value());
        OnParamChange(mRightMix);
        this->RedrawParamControls();
      }
      break;
    }
    case mRightMix: {
      mix[RIGHT_INDEX] = param->Value();
      break;
    }
    case mLeftLowCut: {
      hipassFilters[LEFT_INDEX].setCutoff(param->Value());
      if (syncParams[syncIndex]) {
        GetParam(mRightLowCut)->Set(param->Value());
        OnParamChange(mRightLowCut);
        this->RedrawParamControls();
      }
      break;
    }
    case mRightLowCut: {
      hipassFilters[RIGHT_INDEX].setCutoff(param->Value());
      break;
    }
    case mLeftHighCut: {
      lowpassFilters[LEFT_INDEX].setCutoff((0.85 / 0.99) * param->Value());
      if (syncParams[syncIndex]) {
        GetParam(mRightHighCut)->Set(param->Value());
        OnParamChange(mRightHighCut);
        this->RedrawParamControls();
      }
      break;
    }
    case mRightHighCut: {
      lowpassFilters[RIGHT_INDEX].setCutoff((0.85 / 0.99) * param->Value());
      break;
    }
    case mLeftModRate: {
      modLfos[LEFT_INDEX].setFrequency(param->Value());
      if (syncParams[syncIndex]) {
        GetParam(mRightModRate)->Set(param->Value());
        OnParamChange(mRightModRate);
        this->RedrawParamControls();
      }
      break;
    }
    case mRightModRate: {
      modLfos[RIGHT_INDEX].setFrequency(param->Value());
      break;
    }
    case mLeftModAmt: {
      delayModInSamples[LEFT_INDEX] = msToSamples(MAX_DELAY_MOD_TIME_MS) * param->Value();
      if (syncParams[syncIndex]) {
        GetParam(mRightModAmt)->Set(param->Value());
        OnParamChange(mRightModAmt);
        this->RedrawParamControls();
      }
      break;
    }
    case mRightModAmt: {
      delayModInSamples[RIGHT_INDEX] = msToSamples(MAX_DELAY_MOD_TIME_MS) * param->Value();
      break;
    }
    case mLeftAnalog: {
      feedbackFilters[LEFT_INDEX].setCutoff(0.99 - ((param->Value() / 100.0) * 0.6));
      if (syncParams[syncIndex]) {
        GetParam(mRightAnalog)->Set(param->Value());
        OnParamChange(mRightAnalog);
        RedrawParamControls();
      }
      break;
    }
    case mRightAnalog: {
      feedbackFilters[RIGHT_INDEX].setCutoff(0.99 - ((param->Value() / 100.0) * 0.6));
      break;
    }
    case mLeftDistortionThreshold: {
      distortions[LEFT_INDEX].setThreshold(1.0 - ((param->Value() / 100.0) * 0.99));
      if (syncParams[syncIndex]) {
        GetParam(mRightDistortionThreshold)->Set(param->Value());
        OnParamChange(mRightDistortionThreshold);
        RedrawParamControls();
      }
      break;
    }
    case mRightDistortionThreshold: {
      distortions[RIGHT_INDEX].setThreshold(1.0 - ((param->Value() / 100.0) * 0.99));
      break;
    }
    case mDelayTimeSync:
    case mFeedbackSync:
    case mMixSync:
    case mModRateSync:
    case mModAmtSync:
    case mLowCutSync:
    case mHighCutSync:
    case mAnalogSync:
    case mDistortionThresholdSync: {
      syncParams[paramIdx - kRegParamsEnd - 1] = param->Bool();
      if (2 * (paramIdx - kRegParamsEnd - 1) == mLeftDelayTimeMs) {
        OnParamChange(mLeftTempoSync);
        if (tempoSync[LEFT_INDEX]) {
          OnParamChange(mLeftTempoSyncDelayNote);
        } else {
          OnParamChange(2 * (paramIdx - kRegParamsEnd - 1));
        }
      } else if (2 * (paramIdx - kRegParamsEnd - 1) == mLeftModRate) {
        OnParamChange(mLeftModTempoSync);
        if (tempoSync[LEFT_INDEX + 2]) {
          OnParamChange(mLeftTempoSyncModNote);
        } else {
          OnParamChange(2 * (paramIdx - kRegParamsEnd - 1));
        }
      } else {
        OnParamChange(2 * (paramIdx - kRegParamsEnd - 1));
      }
      break;
    }
    case mLeftTapTempo:
    case mRightTapTempo: {
      // get the current time
      struct timeval tp;
      gettimeofday(&tp, NULL);
      long int currentTime = tp.tv_sec * 1000 + tp.tv_usec / 1000;
      // figure out the new delay time
      double newDelayTimeMs = currentTime - timesSinceLastTaps[paramIdx - mLeftTapTempo];
      timesSinceLastTaps[paramIdx - mLeftTapTempo] = currentTime;
      if (newDelayTimeMs <= MAX_DELAY_TIME_MS) {
        GetParam(paramIdx - mLeftTapTempo)->Set(newDelayTimeMs);
        OnParamChange(paramIdx - mLeftTapTempo);
      }
      // Potentially redraw the delay knob/(s).
      RedrawParamControls();
      break;
    }
    case mLeftTempoSync: {
      tempoSync[LEFT_INDEX] = param->Bool();
      if (tempoSyncGraphicsLoaded) {
        if (tempoSync[LEFT_INDEX]) {
          OnParamChange(mLeftTempoSyncDelayNote);
          GetGUI()->HideControl(mLeftDelayTimeMs, true);
          GetGUI()->HideControl(mLeftTempoSyncDelayNote, false);
          RedrawParamControls();
        } else {
          OnParamChange(mLeftDelayTimeMs);
          GetGUI()->HideControl(mLeftDelayTimeMs, false);
          GetGUI()->HideControl(mLeftTempoSyncDelayNote, true);
          RedrawParamControls();
        }
        if (syncParams[mLeftDelayTimeMs]) {
          GetParam(mRightTempoSync)->Set(param->Bool());
          OnParamChange(mRightTempoSync);
          RedrawParamControls();
        }
      }
      break;
    }
    case mRightTempoSync: {
      tempoSync[RIGHT_INDEX] = param->Bool();
      if (tempoSyncGraphicsLoaded) {
        if (tempoSync[RIGHT_INDEX]) {
          OnParamChange(mRightTempoSyncDelayNote);
          GetGUI()->HideControl(mRightDelayTimeMs, true);
          GetGUI()->HideControl(mRightTempoSyncDelayNote, false);
          RedrawParamControls();
        } else {
          OnParamChange(mRightDelayTimeMs);
          GetGUI()->HideControl(mRightDelayTimeMs, false);
          GetGUI()->HideControl(mRightTempoSyncDelayNote, true);
          RedrawParamControls();
        }
      }
      break;
    }
    case mLeftModTempoSync: {
      tempoSync[LEFT_INDEX + 2] = param->Bool();
      if (tempoSyncGraphicsLoaded) {
        if (tempoSync[LEFT_INDEX + 2]) {
          OnParamChange(mLeftTempoSyncModNote);
          GetGUI()->HideControl(mLeftModRate, true);
          GetGUI()->HideControl(mLeftTempoSyncModNote, false);
          RedrawParamControls();
        } else {
          OnParamChange(mLeftModRate);
          GetGUI()->HideControl(mLeftModRate, false);
          GetGUI()->HideControl(mLeftTempoSyncModNote, true);
          RedrawParamControls();
        }
        if (syncParams[mLeftModRate / 2]) {
          GetParam(mRightModTempoSync)->Set(param->Bool());
          OnParamChange(mRightModTempoSync);
          RedrawParamControls();
        }
      }
      break;
    }
    case mRightModTempoSync: {
      tempoSync[RIGHT_INDEX + 2] = param->Bool();
      if (tempoSyncGraphicsLoaded) {
        if (tempoSync[RIGHT_INDEX + 2]) {
          OnParamChange(mRightTempoSyncModNote);
          GetGUI()->HideControl(mRightModRate, true);
          GetGUI()->HideControl(mRightTempoSyncModNote, false);
          RedrawParamControls();
        } else {
          OnParamChange(mRightModRate);
          GetGUI()->HideControl(mRightModRate, false);
          GetGUI()->HideControl(mRightTempoSyncModNote, true);
          RedrawParamControls();
        }
      }
      break;
    }
    case mLeftTempoSyncDelayNote: {
      if (tempoSync[LEFT_INDEX]) {
        double newDelay = (int) noteToSamples(param->Int());
        buffers[LEFT_INDEX]->setrwOffset(newDelay);
        delaysInSamples[LEFT_INDEX] = newDelay;
        GetParam(mLeftDelayTimeMs)->Set(samplesToMs(newDelay));
        if (syncParams[0]) {
          GetParam(mRightTempoSyncDelayNote)->Set(param->Int());
          OnParamChange(mRightTempoSyncDelayNote);
          RedrawParamControls();
        }
      }
      break;
    }
    case mRightTempoSyncDelayNote: {
      if (tempoSync[RIGHT_INDEX]) {
        double newDelay = (int) noteToSamples(param->Int());
        buffers[RIGHT_INDEX]->setrwOffset(newDelay);
        delaysInSamples[RIGHT_INDEX] = newDelay;
      }
      break;
    }
    case mLeftTempoSyncModNote: {
      if (tempoSync[LEFT_INDEX + 2]) {
        double newModRate = 1.0 / ((1.0 / GetSampleRate()) * noteToSamples(invertQuantizedDelayEnum(param->Int())));
        modLfos[LEFT_INDEX].setFrequency(newModRate);
        GetParam(mLeftModRate)->Set(samplesToMs(newModRate));
        if (syncParams[mLeftModRate / 2]) {
          GetParam(mRightTempoSyncModNote)->Set(param->Int());
          OnParamChange(mRightTempoSyncModNote);
          RedrawParamControls();
        }
      }
      break;
    }
    case mRightTempoSyncModNote: {
      if (tempoSync[RIGHT_INDEX + 2]) {
        double newModRate = 1.0 / ((1.0 / GetSampleRate()) * noteToSamples(invertQuantizedDelayEnum(param->Int())));
        modLfos[RIGHT_INDEX].setFrequency(newModRate);
      }
      break;
    }
  }
}

void ReplicantDelay::CreatePresets() {
}

void ReplicantDelay::CreateParams() {
  for (int i = 0; i < kNumParams; i++) {
    IParam* param = GetParam(i);
    const parameterProperties_struct& properties = parameterProperties[i];
    switch (i) {
      case mDelayTimeSync:
      case mFeedbackSync:
      case mMixSync:
      case mModRateSync:
      case mModAmtSync:
      case mLowCutSync:
      case mHighCutSync:
      case mAnalogSync:
      case mDistortionThresholdSync:
      case mLeftTapTempo:
      case mRightTapTempo:
      case mLeftTempoSync:
      case mRightTempoSync:
      case mLeftModTempoSync:
      case mRightModTempoSync: {
        param->InitBool(properties.name, properties.defaultBoolVal);
        param->SetIsMeta(true);
        break;
      }
      case mLeftTempoSyncDelayNote:
      case mRightTempoSyncDelayNote:
      case mLeftTempoSyncModNote:
      case mRightTempoSyncModNote: {
        param->InitEnum(properties.name, QUARTER, NUM_QUANTIZED_DELAY_TIMES);
        param->SetIsMeta(true);
        for (int i = 0; i < NUM_QUANTIZED_DELAY_TIMES; i++) {
          param->SetDisplayText(i, quantizedDisplayTexts[i]);
        }
        break;
      }
      default: {
        param->InitDouble(properties.name,
                          properties.defaultDoubleVal,
                          properties.minVal,
                          properties.maxVal,
                          parameterStep);
        param->SetIsMeta(true);
        break;
      }
    }
  }
  // hack: for some reason, accessing these parameters seems to
  // get them initialized properly...
  printf("DELAY TIME LEFT, %f", GetParam(mLeftDelayTimeMs)->Value());
  printf("DELAY TIME RIGHT, %f", GetParam(mRightDelayTimeMs)->Value());
  GetParam(mLeftDelayTimeMs)->SetShape(2.5);
  GetParam(mRightDelayTimeMs)->SetShape(2.5);
  GetParam(mLeftModRate)->SetShape(3);
  GetParam(mRightModRate)->SetShape(3);
  GetParam(mLeftModAmt)->SetShape(2);
  GetParam(mRightModAmt)->SetShape(2);
  GetParam(mLeftHighCut)->SetShape(2);
  GetParam(mRightHighCut)->SetShape(2);
  GetParam(mLeftLowCut)->SetShape(2);
  GetParam(mRightLowCut)->SetShape(2);
  GetParam(mLeftAnalog)->SetShape(1);
  GetParam(mRightAnalog)->SetShape(1);
  GetParam(mLeftDistortionThreshold)->SetShape(0.4);
  GetParam(mRightDistortionThreshold)->SetShape(0.4);
  for (int i = 0; i < kNumParams; i++) {
    OnParamChange(i);
  }
}

void ReplicantDelay::CreateGraphics() {
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(BG_ID, BG_FN);
  IColor *textColor = new IColor(255, 139, 225, 243);
  IColor *analogTextColor = new IColor(255, 252, 71, 122);
  IBitmap knobBitmap = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, 140);
  IText *mText = new IText(11, textColor, "Android 7", IText::kStyleNormal, IText::kAlignCenter, 0, IText::kQualityAntiAliased);
  IText *mAnalogText = new IText(11, analogTextColor, "Android 7", IText::kStyleNormal, IText::kAlignCenter, 0, IText::kQualityAntiAliased);
  for (int i = 0; i < mLeftLowCut; i++) {
    const parameterProperties_struct& properties = parameterProperties[i];
    IControl* control;
    IBitmap* graphic;
    switch (i) {
      default:
        graphic = &knobBitmap;
        control = new IKnobMultiControlText(this, *new IRECT(properties.x, properties.y, properties.x + graphic->W, properties.y + (graphic->H / graphic->N) + 40), i, graphic, mText);
        break;
    }
    pGraphics->AttachControl(control);
  }
  IBitmap sliderBitmap = pGraphics->LoadIBitmap(SLIDER_ID, SLIDER_FN, 1);
  int sliderLength;
  IText *sliderText;
  for (int i = mLeftLowCut; i < mLeftDistortionThreshold; i++) {
    if (i == mLeftLowCut || i == mLeftHighCut || i == mLeftAnalog) {
      sliderLength = SMALL_SLIDER_LEN;
    } else {
      sliderLength = LONG_SLIDER_LEN;
    }
    if (i == mLeftAnalog || i == mRightAnalog) {
      sliderText = mAnalogText;
    } else {
      sliderText = mText;
    }
    pGraphics->AttachControl(new IFaderControlText(this, *new IRECT(parameterProperties[i].x, parameterProperties[i].y, parameterProperties[i].x + sliderLength, parameterProperties[i].y + (sliderBitmap.H / sliderBitmap.N) + 50), sliderLength, i, &sliderBitmap, kHorizontal, false, sliderText, 0, 41));
  }
  for (int i = mLeftDistortionThreshold; i <= mRightDistortionThreshold; i++) {
    pGraphics->AttachControl(new IFaderControlText(this, *new IRECT(parameterProperties[i].x, parameterProperties[i].y, parameterProperties[i].x + sliderBitmap.W + 50, parameterProperties[i].y + sliderLength), DISTORTION_SLIDER_LEN, i, &sliderBitmap, kVertical, false, mText, -21, 195));
  }
  IBitmap syncBitmap = pGraphics->LoadIBitmap(SINE_WAVE_ID, SINE_WAVE_FN, 2);
  for (int i = mDelayTimeSync; i < kSyncParamsEnd; i++) {
    pGraphics->AttachControl(new ISwitchControl(this, parameterProperties[i].x, parameterProperties[i].y, i, &syncBitmap));
  }
  IBitmap tapTempoBitmap = pGraphics->LoadIBitmap(TAP_TEMPO_ID, TAP_TEMPO_FN, 1);
  for (int i = mLeftTapTempo; i < kTapTempoParamsEnd; i++) {
    pGraphics->AttachControl(new ISwitchControl(this, parameterProperties[i].x, parameterProperties[i].y, i, &tapTempoBitmap));
  }
  IBitmap tempoSyncBitmap = pGraphics->LoadIBitmap(TEMPO_SYNC_ID, TEMPO_SYNC_FN, 2);
  for (int i = mLeftTempoSync; i < mLeftTempoSyncDelayNote; i++) {
    pGraphics->AttachControl(new ISwitchControl(this, parameterProperties[i].x, parameterProperties[i].y, i, &tempoSyncBitmap));
  }
  IBitmap syncKnobBitmap = pGraphics->LoadIBitmap(TEMPO_SYNC_KNOB_ID, TEMPO_SYNC_KNOB_FN, 19);
  for (int i = mLeftTempoSyncDelayNote; i <= mRightTempoSyncModNote; i++) {
    pGraphics->AttachControl(new IKnobMultiControlText(this, *new IRECT(parameterProperties[i].x, parameterProperties[i].y, parameterProperties[i].x + syncKnobBitmap.W, parameterProperties[i].y + (syncKnobBitmap.H / syncKnobBitmap.N) + 40), i, &syncKnobBitmap, mText));
    pGraphics->HideControl(i, true);
  }
  AttachGraphics(pGraphics);
  tempoSyncGraphicsLoaded = true;
}

double ReplicantDelay::noteToSamples(int t) {
  int beatsPerMeasure, noteGetsBeat;
  // assume 4/4, so that unreasonable values don't get returned in the event of a failure of GetTimeSig
  beatsPerMeasure = 4;
  noteGetsBeat = 4;
  GetTimeSig(&beatsPerMeasure, &noteGetsBeat);
  if (beatsPerMeasure < 1) {
    beatsPerMeasure = 4;
  }
  if (noteGetsBeat < 1) {
    noteGetsBeat = 4;
  }
  double samplesPerMeasure = msToSamples((beatsPerMeasure / (tempo * 1.0)) * 60.0 * 1000.0);
  double measuresPerBeat = 1.0 / beatsPerMeasure;
  return fmin(samplesPerMeasure * measuresPerBeat * noteGetsBeat * quantizedTimeMultipliers[t], msToSamples(MAX_DELAY_TIME_MS));
}

// utility for "reflecting" a quantized delay enum across the middle of the list
double ReplicantDelay::invertQuantizedDelayEnum(int toInvert) {
  return 2 * TRIPLET_QUARTER - toInvert;
}
