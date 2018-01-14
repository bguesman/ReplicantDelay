#define PLUG_MFR "BradGuesman"
#define PLUG_NAME "ReplicantDelay"

#define PLUG_CLASS_NAME ReplicantDelay

#define BUNDLE_MFR "BradGuesman"
#define BUNDLE_NAME "ReplicantDelay"

#define PLUG_ENTRY ReplicantDelay_Entry
#define PLUG_VIEW_ENTRY ReplicantDelay_ViewEntry

#define PLUG_ENTRY_STR "ReplicantDelay_Entry"
#define PLUG_VIEW_ENTRY_STR "ReplicantDelay_ViewEntry"

#define VIEW_CLASS ReplicantDelay_View
#define VIEW_CLASS_STR "ReplicantDelay_View"

// Format        0xMAJR.MN.BG - in HEX! so version 10.1.5 would be 0x000A0105
#define PLUG_VER 0x00010000
#define VST3_VER_STR "1.0.0"

// http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm
// 4 chars, single quotes. At least one capital letter
#define PLUG_UNIQUE_ID 'Royb'
// make sure this is not the same as BUNDLE_MFR
#define PLUG_MFR_ID 'Acme'

// ProTools stuff

#if (defined(AAX_API) || defined(RTAS_API)) && !defined(_PIDS_)
  #define _PIDS_
  const int PLUG_TYPE_IDS[2] = {'EFN1', 'EFN2'};
  const int PLUG_TYPE_IDS_AS[2] = {'EFA1', 'EFA2'}; // AudioSuite
#endif

#define PLUG_MFR_PT "BradGuesman\nBradGuesman\nAcme"
#define PLUG_NAME_PT "ReplicantDelay\nROYB"
#define PLUG_TYPE_PT "Delay"
#define PLUG_DOES_AUDIOSUITE 1

/* PLUG_TYPE_PT can be "None", "EQ", "Dynamics", "PitchShift", "Reverb", "Delay", "Modulation", 
"Harmonic" "NoiseReduction" "Dither" "SoundField" "Effect" 
instrument determined by PLUG _IS _INST
*/

#define PLUG_CHANNEL_IO "1-2 2-2" //"1-1 2-2"
//#if (defined(AAX_API) || defined(RTAS_API))
//#define PLUG_CHANNEL_IO "1-1 2-2"
//#else
//#define PLUG_CHANNEL_IO "0-1 0-2"
//#endif

#define PLUG_LATENCY 0
#define PLUG_IS_INST 0
//#define PLUG_IS_INST

// if this is 0 RTAS can't get tempo info
//#define PLUG_DOES_MIDI 0
#define PLUG_DOES_MIDI 1

#define PLUG_DOES_STATE_CHUNKS 1

// Unique IDs for each image resource.
#define BG_ID         101
#define KNOB_ID       102
#define SLIDER_ID     103
#define SINE_WAVE_ID  104
#define TAP_TEMPO_ID  105
#define TEMPO_SYNC_ID  106
#define TEMPO_SYNC_KNOB_ID  107

// Image resource locations for this plug.
#define BG_FN         "resources/img/delay-plugin-background-cyan-layout-9.png"
#define KNOB_FN       "resources/img/cyan_outrun_knob_72px.png"
#define SLIDER_FN     "resources/img/cyan_outrun_slider_peg.png"
#define SINE_WAVE_FN     "resources/img/sine_wave_button_cyan.png"
#define TAP_TEMPO_FN     "resources/img/tap_tempo_button.png"
#define TEMPO_SYNC_FN     "resources/img/quarter_note_button_cyan.png"
#define TEMPO_SYNC_KNOB_FN "resources/img/cyan_outrun_knob_72px_tempoframes.png"
#define SMALL_SLIDER_LEN 298
#define LONG_SLIDER_LEN 314
#define DISTORTION_SLIDER_LEN 219

// GUI default dimensions
#define GUI_WIDTH 700
#define GUI_HEIGHT 640

// on MSVC, you must define SA_API in the resource editor preprocessor macros as well as the c++ ones
#if defined(SA_API)
#include "app_wrapper/app_resource.h"
#endif

// vst3 stuff
#define MFR_URL "www.olilarkin.co.uk"
#define MFR_EMAIL "spam@me.com"
//#define EFFECT_TYPE_VST3 "Fx"
#define EFFECT_TYPE_VST3 "Fx|Delay"

/* "Fx|Analyzer"", "Fx|Delay", "Fx|Distortion", "Fx|Dynamics", "Fx|EQ", "Fx|Filter",
"Fx", "Fx|Instrument", "Fx|InstrumentExternal", "Fx|Spatial", "Fx|Generator",
"Fx|Mastering", "Fx|Modulation", "Fx|PitchShift", "Fx|Restoration", "Fx|Reverb",
"Fx|Surround", "Fx|Tools", "Instrument", "Instrument|Drum", "Instrument|Sampler",
"Instrument|Synth", "Instrument|Synth|Sampler", "Instrument|External", "Spatial",
"Spatial|Fx", "OnlyRT", "OnlyOfflineProcess", "Mono", "Stereo",
"Surround"
*/
