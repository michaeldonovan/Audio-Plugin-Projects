#include "MultiDistortion.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include "IAutoGUI.h"

#include <math.h>
//#include "denormal.h"


const int kNumPrograms = 1;

enum EParams
{
  kDistType=0,
  kDrive = 1,
  kAmount=2,
  kWarm = 3,
  kMix=4,
  kClipEnabled=5,
  kClipLevel=6,
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

MultiDistortion::MultiDistortion(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),  mDC(0.25), mDrive(1.)
{
  TRACE;

  
  
  mDistortedDC = (1/ mAmount) * fastAtan(mDC *  mAmount);

  
  //arguments are: name, defaultVal, minVal, maxVal, step, label
  
  GetParam(kDistType)->InitInt("Type", 1, 1, 4);
  GetParam(kAmount)->SetShape(1.0);

  GetParam(kDrive)->InitDouble("Input Gain", 0.0, 0.0, 20.0, 0.01, "dB");
  GetParam(kDrive)->SetShape(1.0);

  GetParam(kAmount)->InitDouble("Saturation", 12.5, 0.01, 100.0, 0.01, "%");
  GetParam(kAmount)->SetShape(1.0);
  
  GetParam(kWarm)->InitDouble("Warm", 0.0, 0.0, 1.0, 0.1);
  GetParam(kWarm)->SetShape(1.0);

  GetParam(kMix)->InitDouble("Mix", 100.0, 0.0, 100.0, 0.01, "%");
  GetParam(kMix)->SetShape(1.0);
  
  GetParam(kClipLevel)->InitDouble("Clip Level", 0, -18.0, 0.0, 0.01, "dB");
  GetParam(kClipLevel)->SetShape(1.0);
  
  GetParam(kClipEnabled)->InitBool("Output Clip", 0);
  //GetParam(kClipEnabled)->SetShape(1.0);



  

  
  
  highShelf.setBiquad(bq_type_highshelf, 6000.0 / GetSampleRate(), 1.0, 0.0);
  lowPeak.setBiquad(bq_type_peak, 90.0 /GetSampleRate(), 1.0, 0.0);

  
  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
  
}

MultiDistortion::~MultiDistortion() {}

void MultiDistortion::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
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
      
      switch (mDistType) {

          case 1: //Soft Saturation
          {
            double threshold = 0.95;
            if(sample<threshold)
            break;
            else if(sample>threshold)
              sample = threshold + (sample - threshold) / (1 + pow(((sample - threshold)/(1 - threshold)), 2));
            else if(sample >1)
            sample = (sample + 1)/2;
          }break;
          
          
          
          case 2: // Assymetric Distortion
          {
            sample = (1/ (mAmount/2.0)) * fastAtan(sample * mAmount) -mDistortedDC ;
            //sample *= pow(10, mAmount/20.0);
          }break;
        
          

          case 3:  //Sine Shaper
          {
            double amount = fabs(mAmount - .3);
            double z = M_PI * amount/4.0;
            double s = 1/sin(z);
            double b = 1 / amount;
          
            if (sample>b)
              sample = 1;
            else if (sample < - b)
              sample = -1;
            else
              sample = sin(z * sample) * s;
            
            sample *= pow(10, -amount/20.0);
          }break;
          
          
          case 4: //FoldBack Distortion
          {
            double threshold = 1.0 - (mAmount/10.0);
            if (sample > threshold || sample < - threshold) {
              sample = fabs(fabs(fmod(sample - threshold, threshold * 4)) - threshold * 2) - threshold;
            }
          }break;
          
          
      }
    
     // Warmth
      sample = lowPeak.process(sample);
      sample = highShelf.process(sample);
      
      
      sample*=postGain;
  
      
      //Mix
      sample=mMix*sample + (1.0-mMix)*drySample;
      
  
      //Clipping
      if(mClipEnabled)
      {
        if(sample>mClipLevel)
          sample=mClipLevel;
        else if(sample<-1*mClipLevel)
          sample=-1*mClipLevel;
      }
  
      *output=sample;
    }
  }
}


void MultiDistortion::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void MultiDistortion::OnParamChange(int paramIdx)
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
      
      
      
    case kAmount:
      mAmount = 4 * (GetParam(kAmount)->Value() / 100) ;
      break;
      
      
    case kDistType:
      mDistType = GetParam(kDistType)->Value();
      break;
      
    case kClipEnabled:
      mClipEnabled = GetParam(kClipEnabled)->Value();
      break;
     
    case kWarm:
      mWarm = GetParam(kWarm)->Value();
      lowPeak.Biquad::setPeakGain(1.5 * mWarm);
      highShelf.Biquad::setPeakGain(-6.0 * mWarm);
      break;
      
    default:
      break;
  }
}

double MultiDistortion::fastAtan(double x){
  return (x / (1.0 + 0.28 * (x * x)));
}



