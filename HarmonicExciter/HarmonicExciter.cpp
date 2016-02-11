#include <math.h>

#include "HarmonicExciter.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include "IAutoGUI.h"
#include "../../WDL/denormal.h"


const int kNumPrograms = 1;

enum EParams
{
  kType=0,
  kDrive = 1,
  kFreq=2,
  kMix=3,
  kHarmOnly,
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

HarmonicExciter::HarmonicExciter(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mDrive(1.)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  
  GetParam(kType)->InitInt("Type", 1, 1, 3);
  GetParam(kType)->SetShape(1.0);

  GetParam(kDrive)->InitDouble("Drive", 0.0, 0.0, 18.0, 0.01, "dB");
  GetParam(kDrive)->SetShape(1.0);

  GetParam(kFreq)->InitDouble("Frequency", 2200.0, 1750.0, 7000.00, 1.0, "Hz");
  GetParam(kFreq)->SetShape(1.0);

  GetParam(kMix)->InitDouble("Mix", 50.0, 1.0, 99.0, 0.01, "%");
  GetParam(kMix)->SetShape(1.0);
  
  GetParam(kHarmOnly)->InitBool("Harmonics Only", TRUE);
  
  //Initialize Filter
  mHighPass=new VAStateVariableFilter();
  mHighPass->setSampleRate(GetSampleRate());
  mHighPass->setFilterType(SVFHighpass);
  mHighPass->setCutoffFreq(2200.0);
  mHighPass->setQ(0.707);
  
  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
  
}

HarmonicExciter::~HarmonicExciter() {}

void HarmonicExciter::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  int const channelCount = 2;
  
  
  for (int i = 0; i < channelCount; i++) {
    double* input = inputs[i];
    double* output = outputs[i];
    
    
    for (int s = 0; s < nFrames; ++s, ++input, ++output) {
     
      double sample = *input;
      double drySample = sample;
      
      double preGain = DBToAmp(mDrive);
      double postGain = -preGain;
      
      //Drive
      sample*=preGain;
      
      
      
      //High-pass
      sample = mHighPass->processAudioSample(sample, i);
      //sample = highPass.process(sample);


      //Add harmonics
      if (WDL_DENORMAL_OR_ZERO_DOUBLE_AGGRESSIVE(&sample))
        sample = 0.;
      else{
        switch (mType) {

            case 1: //Full wave rectification
            {
              sample=fabs(sample);
            }break;
       
          
            case 2: // Assymetric clipping
            {
              if (sample>0.9) {
                sample = 0.9;
              }
            }break;
          
          case 3: //Soft clipping
          {
            double threshold = 0.95;
            if(sample<threshold)
              break;
            else if(sample>threshold)
              sample = threshold + (sample - threshold) / (1 + pow(((sample - threshold)/(1 - threshold)), 2));
            else if(sample >1)
              sample = (sample + 1)/2;
          }break;
        }
      }
    
      if(!mHarmOnly){
        //Mix
        sample=drySample+ sample*mMix;
      }
      

      
  
      *output=sample;
    }
  }
}



void HarmonicExciter::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void HarmonicExciter::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
      
    case kType:
      mType = GetParam(kType)->Value();
      break;
      
    case kDrive:
      mDrive = GetParam(kDrive)->Value();
      break;

    case kFreq:
      mFreq = GetParam(kFreq)->Value();
      mHighPass->setCutoffFreq(mFreq);
      break;
      
    case kMix:
      mMix = 36*(GetParam(kMix)->Value() / 100.0) - 18;
      mMix=pow(10, mMix/20.0);
      break;
    
    case kHarmOnly:
      mHarmOnly=GetParam(kHarmOnly)->Value();
      break;
      
    default:
      break;
  }
}



