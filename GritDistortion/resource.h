#define PLUG_MFR "Michael Donovan"
#define PLUG_NAME "Grit (Distortion)"

#define PLUG_CLASS_NAME GritDistortion

#define BUNDLE_MFR "michaeldonovan"
#define BUNDLE_NAME "GritDistortion"

#define PLUG_ENTRY GritDistortion_Entry
#define PLUG_VIEW_ENTRY GritDistortion_ViewEntry

#define PLUG_ENTRY_STR "GritDistortion_Entry"
#define PLUG_VIEW_ENTRY_STR "GritDistortion_ViewEntry"

#define VIEW_CLASS GritDistortion_View
#define VIEW_CLASS_STR "GritDistortion_View"

// Format        0xMAJR.MN.BG - in HEX! so version 10.1.5 would be 0x000A0105
#define PLUG_VER 0x000100000
#define VST3_VER_STR "1.0.0"

// http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm
// 4 chars, single quotes. At least one capital letter
#define PLUG_UNIQUE_ID 'y0ti'
// make sure this is not the same as BUNDLE_MFR
#define PLUG_MFR_ID 'Dnvn'

// ProTools stuff

#if (defined(AAX_API) || defined(RTAS_API)) && !defined(_PIDS_)
  #define _PIDS_
  const int PLUG_TYPE_IDS[2] = {'EFN1', 'EFN2'};
  const int PLUG_TYPE_IDS_AS[2] = {'EFA1', 'EFA2'}; // AudioSuite
#endif

#define PLUG_MFR_PT "MichaelDonovan\nMichaelDonovan\nDnvn"
#define PLUG_NAME_PT "GritDistortion\ny0ti"
#define PLUG_TYPE_PT "Effect"
#define PLUG_DOES_AUDIOSUITE 1

/* PLUG_TYPE_PT can be "None", "EQ", "Dynamics", "PitchShift", "Reverb", "Delay", "Modulation", 
"Harmonic" "NoiseReduction" "Dither" "SoundField" "Effect" 
instrument determined by PLUG _IS _INST
*/

#define PLUG_CHANNEL_IO "1-1 2-2"

#define PLUG_LATENCY 0
#define PLUG_IS_INST 0

// if this is 0 RTAS can't get tempo info
#define PLUG_DOES_MIDI 0

#define PLUG_DOES_STATE_CHUNKS 0

// Unique IDs for each image resource.
#define BACKGROUND_ID 101
#define BIGKNOB_ID 102
#define SMALLKNOB_ID 103
#define LEVELMETER_ID 104
#define BUTTON_ID 105
#define DISPLAY_ID 106
#define LPF_ID 107
#define HPF_ID 108

// Image resource locations for this plug.
#define BACKGROUND_FN "resources/img/Background650.png"
#define BIGKNOB_FN "resources/img/BigKnobAction.png"
#define SMALLKNOB_FN "resources/img/SmallKnobAction.png"
#define LEVELMETER_FN "resources/img/MeterAction.png"
#define BUTTON_FN "resources/img/Button.png"
#define DISPLAY_FN "resources/img/Display.png"
#define LPF_FN "resources/img/LPFNotches.png"
#define HPF_FN "resources/img/HPFNotches.png"

// GUI default dimensions
#define GUI_WIDTH 650
#define GUI_HEIGHT 325

// on MSVC, you must define SA_API in the resource editor preprocessor macros as well as the c++ ones
#if defined(SA_API) && !defined(OS_IOS)
#include "app_wrapper/app_resource.h"
#endif

// vst3 stuff
#define MFR_URL "www.github.com/michaeldonovan"
#define MFR_EMAIL "spam@me.com"
#define EFFECT_TYPE_VST3 "Fx|Distortion"

/* "Fx|Analyzer"", "Fx|Delay", "Fx|Distortion", "Fx|Dynamics", "Fx|EQ", "Fx|Filter",
"Fx", "Fx|Instrument", "Fx|InstrumentExternal", "Fx|Spatial", "Fx|Generator",
"Fx|Mastering", "Fx|Modulation", "Fx|PitchShift", "Fx|Restoration", "Fx|Reverb",
"Fx|Surround", "Fx|Tools", "Instrument", "Instrument|Drum", "Instrument|Sampler",
"Instrument|Synth", "Instrument|Synth|Sampler", "Instrument|External", "Spatial",
"Spatial|Fx", "OnlyRT", "OnlyOfflineProcess", "Mono", "Stereo",
"Surround"
*/
