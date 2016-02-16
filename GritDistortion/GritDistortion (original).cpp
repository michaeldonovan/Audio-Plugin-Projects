#include "GritDistortion.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include "IAutoGUI.h"

#include <math.h>
//#include "denormal.h"


const int kNumPrograms = 1;

enum EParams
{
  kDrive = 0,
  kCharacter=1,
  kMix=2,
 // kLowCut=3,
 // kHiCut=4,
  kClipLevel=3,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,
  
  kDriveX = 100,
  kDriveY = 100,
  kKnobFrames = 60
};

GritDistortion::GritDistortion(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),  mDC(0.2), mDrive(1.)
{
  TRACE;


  
  mDistortedDC = (1/ mCharacter) * fastAtan(mDC *  mCharacter);

  
  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kDrive)->InitDouble("Drive", 0.0, 0.0, 20.0, 0.01, "dB");
  GetParam(kDrive)->SetShape(1.0);

  GetParam(kMix)->InitDouble("Mix", 100.0, 0.0, 100.0, 0.01, "%");
  GetParam(kMix)->SetShape(1.0);
  
  GetParam(kClipLevel)->InitDouble("Clip Level", 0, -18.0, 0.0, 0.01, "dB");
  GetParam(kClipLevel)->SetShape(1.0);
  
 // GetParam(kLowCut)->InitDouble("Low Cut",  0.99, 0.01, 0.99, 0.001, "HZ");
 // GetParam(kLowCut)->SetShape(2);
  
 // GetParam(kHiCut)->InitDouble("High Cut", 0.99, 0.01, 0.99, 0.001, "HZ");
//  GetParam(kHiCut)->SetShape(2);
  
  GetParam(kCharacter)->InitDouble("Character", 5.0, 1.0, 10.0, 0.01, "%");
  GetParam(kCharacter)->SetShape(1.0);
  

  


  
  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
  
}

GritDistortion::~GritDistortion() {}

void GritDistortion::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  int const channelCount = 2;
  
  for (int i = 0; i < channelCount; i++) {
    double* input = inputs[i];
    double* output = outputs[i];
    
    for (int s = 0; s < nFrames; ++s, ++input, ++output) {
      double preGain = pow(10, mDrive/20.0);
      double postGain = pow(10, -mDrive/40.0);
      
  
      double sample = *input;
      double drySample = sample;
      
        
      //Distort
    
      sample*=preGain;
      sample = (1/ mCharacter) * fastAtan(sample * mCharacter) -mDistortedDC ;
      sample*=postGain;
      //Filter
      //sample = mHiCutFilter.process(sample);
      //sample = mLowCutFilter.process(sample);
      
      
      //Mix
      sample=mMix*sample + (1.0-mMix)*drySample;
      
  
      //Clipping
      if(sample>mClipLevel)
        sample=mClipLevel;
      else if(sample<-1*mClipLevel)
        sample=-1*mClipLevel;
      
      *output=sample;
    }

  }
}


void GritDistortion::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void GritDistortion::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kDrive:
      mDrive = GetParam(kDrive)->Value();
      break;

    case kMix:
      mMix = GetParam(kMix)->Value()/100.0;
      break;
    
    case kClipLevel:
      mClipLevel=pow(10, GetParam(kClipLevel)->Value()/20);
      break;
      
//    case kHiCut:
//      mHiCutFilter.setFilterMode(Filter::FILTER_MODE_LOWPASS);
//      mHiCutFilter.setResonance(.7);
//      mHiCutFilter.setCutoff(GetParam(kHiCut)->Value());
//      break;
//      
//    case kLowCut:
//      mLowCutFilter.setFilterMode(Filter::FILTER_MODE_HIGHPASS);
//      mLowCutFilter.setResonance(.8);
//      mLowCutFilter.setCutoff(GetParam(kLowCut)->Value());
//      break;
      
      
      
    case kCharacter:
      mCharacter = GetParam(kCharacter)->Value();
      break;
      
    default:
      break;
  }
}

double GritDistortion::fastAtan(double x){
  return (x / (1.0 + 0.28 * (x * x)));
}


