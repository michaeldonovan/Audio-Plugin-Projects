#include "GritDistortion.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

#include <math.h>


const int kNumPrograms = 1;

enum EParams
{
  kDistType=0,
  kDrive,
  kHighPassCutoff,
  kLowPassCutoff,
  kMix,
  kOutputGain,
  kClipEnabled,
  kClipLevel,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,
  
  
  kDriveX = -18,
  kDriveY = -5,
  
  kOutputGainX = 386,
  kOutputGainY = 192,
  
  kClipX = 386,
  kClipY = 50,


  
  kMixX = 289,
  kMixY = 192,
  
  kFilterX = 487,
  kFilterY = 62,
  
  kMeterLX = 429,
  kMeterLY = 19,
  
  kMeterRX = kMeterLX+27,
  kMeterRY = kMeterLY,
  

  
  kButtons_N=4,
  kButtons_W=35,
  kButtons_H=22,
  kButtonsX=302,
  kButtonsY=159,
  
  kButtonLabelOffsetX=21,
  kButtonLabelOffsetY=3,

  
  kDisplayX=282,
  kDisplayY=38,
  kDiplayFrames=4,

  kFilterFrames=32,
  
  kLevelMeterFrames = 64,
  kSmallKnobFrames = 32,
  kBigKnobFrames = 64
};

GritDistortion::GritDistortion(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),  mDistType(1), mDrive(1.), mMix(100.), mLowPassCutoff(20000.), mHighPassCutoff(0.)
{
  TRACE;

  
  
  
  mPeakFollower = new PeakFollower();
  mPeakFollower2 = new PeakFollower();
  
  
  //Initialize Filters
  mlowPass = new VAStateVariableFilter();
  mhighPass = new VAStateVariableFilter();
  mlowPeak = new VAStateVariableFilter();


  mlowPass->setSampleRate(GetSampleRate());
  mhighPass->setSampleRate(GetSampleRate());
  mlowPeak->setSampleRate(GetSampleRate());
  

  mlowPass->setFilterType(SVFLowpass);
  mlowPass->setCutoffFreq(200000);
  mlowPass->setQ(0.8);
  
  mhighPass->setFilterType(SVFHighpass);
  mhighPass->setCutoffFreq(0);
  mhighPass->setQ(0.6);
  
  mlowPeak->setFilterType(SVFBandShelving);
  mlowPeak->setCutoffFreq(85.0);
  mlowPeak->setShelfGain(1.75);
  mlowPeak->setResonance(0.15);
  

  
  //Initialize Smoothers
  mDriveSmoother = new CParamSmooth(5.0, GetSampleRate());
  mHPFSmoother = new CParamSmooth(5.0, GetSampleRate());
  mMixSmoother = new CParamSmooth(5.0, GetSampleRate());
  mOutputGainSmoother = new CParamSmooth(5.0, GetSampleRate());

  
  //Initalize Parameters
  //arguments are: name, defaultVal, minVal, maxVal, step, label
  
  GetParam(kDistType)->InitInt("Type", 1, 1, 4);

  GetParam(kDrive)->InitDouble("Input Gain", 0.0, 0.0, 18.0, 0.0001, "dB");
  GetParam(kDrive)->SetShape(1.0);

  GetParam(kOutputGain)->InitDouble("Output Gain", 0., -36., 0., 0.0001, "dB");
  GetParam(kOutputGain)->SetShape(1.0);
  

  GetParam(kMix)->InitDouble("Mix", 100.0, 0.0, 100.0, 0.000001, "%");
  GetParam(kMix)->SetShape(2.0);
  
  GetParam(kClipLevel)->InitDouble("Clip Level", 0, -18.0, 0.0, 0.000001, "dB");
  GetParam(kClipLevel)->SetShape(1.0);
  
  GetParam(kClipEnabled)->InitBool("Output Clip", 0);
  
  GetParam(kHighPassCutoff)->InitDouble("High Pass", 00.0, 00.0, 1000.0, 0.0001, "Hz");
  GetParam(kHighPassCutoff)->SetShape(2.0);

  GetParam(kLowPassCutoff)->InitDouble("Low Pass", 20000.0, 1000.0, 20000.0, 0.0001, "Hz");
  GetParam(kLowPassCutoff)->SetShape(2.0);
  
  // create graphics context
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  
  pGraphics->AttachBackground(BACKGROUND_ID, BACKGROUND_FN);
  
  //Load Bitmaps
  
  IBitmap bigKnob = pGraphics->LoadIBitmap(BIGKNOB_ID, BIGKNOB_FN, kBigKnobFrames);
  IBitmap smallKnob1 = pGraphics->LoadIBitmap(SMALLKNOB_ID, SMALLKNOB_FN, kSmallKnobFrames);
  IBitmap smallKnob2 = pGraphics->LoadIBitmap(SMALLKNOB_ID, SMALLKNOB_FN, kSmallKnobFrames);
  IBitmap levelMeterR = pGraphics->LoadIBitmap(LEVELMETER_ID, LEVELMETER_FN, kLevelMeterFrames);
  IBitmap levelMeterL = pGraphics->LoadIBitmap(LEVELMETER_ID, LEVELMETER_FN, kLevelMeterFrames);
  IBitmap buttons = pGraphics->LoadIBitmap(BUTTON_ID, BUTTON_FN, 2);
  IBitmap display = pGraphics->LoadIBitmap(DISPLAY_ID, DISPLAY_FN,kDiplayFrames);
  IBitmap LPF = pGraphics->LoadIBitmap(LPF_ID, LPF_FN, kFilterFrames);
  IBitmap HPF = pGraphics->LoadIBitmap(HPF_ID, HPF_FN, kFilterFrames);
  

  
  //Filter Sliders
  pGraphics->AttachControl(new IKnobMultiControl(this, kFilterX, kFilterY, kLowPassCutoff, &LPF));
  pGraphics->AttachControl(new IKnobMultiControl(this, kFilterX, kFilterY+101, kHighPassCutoff, &HPF));
  
  //Type Selector Button
  pGraphics->AttachControl(new IRadioButtonsControl(this, IRECT(kButtonsX, kButtonsY, kButtonsX+(kButtons_W*kButtons_N), kButtonsY+(kButtons_H)),kDistType,kButtons_N,&buttons, kHorizontal));

  
  //Button Labels
  IColor pColor = IColor(200,255,255,205);
  IText text = IText(16,&pColor);
  pGraphics->AttachControl(new ITextControl(this, IRECT((kButtonsX+kButtonLabelOffsetX+4), (kButtonsY+kButtonLabelOffsetY), (kButtonsX+kButtonLabelOffsetX), (kButtonsY+kButtonLabelOffsetY)), &text, "A "));
  
  pGraphics->AttachControl(new ITextControl(this, IRECT((kButtonsX+kButtonLabelOffsetX+4+kButtons_W), (kButtonsY+kButtonLabelOffsetY), (kButtonsX+kButtonLabelOffsetX+kButtons_W+4), (kButtonsY+kButtonLabelOffsetY)), &text, "B "));
  pGraphics->AttachControl(new ITextControl(this, IRECT((kButtonsX+kButtonLabelOffsetX+6+(2*kButtons_W)), (kButtonsY+kButtonLabelOffsetY), (kButtonsX+kButtonLabelOffsetX+(2*kButtons_W)), (kButtonsY+kButtonLabelOffsetY)), &text, "C "));
  pGraphics->AttachControl(new ITextControl(this, IRECT((kButtonsX+kButtonLabelOffsetX+11+(3*kButtons_W)), (kButtonsY+kButtonLabelOffsetY), (kButtonsX+kButtonLabelOffsetX+(3*kButtons_W)), (kButtonsY+kButtonLabelOffsetY)), &text, "D  "));
  
  //Display
  IRECT zr;
  mDisplay = new IBitmapControl(this, kDisplayX, kDisplayY, &display);
  pGraphics->AttachControl(mDisplay);
  mDisplay->SetTargetArea(zr);
  
  //Knobs
  pGraphics->AttachControl(new IKnobMultiControl(this, kOutputGainX, kOutputGainY, kOutputGain, &smallKnob1));
  pGraphics->AttachControl(new IKnobMultiControl(this, kMixX, kMixY, kMix, &smallKnob2));
  pGraphics->AttachControl(new IKnobMultiControl(this, kDriveX, kDriveY, kDrive, &bigKnob));

  
  
  //INPUT METER
  mMeterL = new IBitmapControl(this, kMeterLX, kMeterLY, &levelMeterL);
  pGraphics->AttachControl(mMeterL);
  mMeterL->SetTargetArea(zr);
  
  //OUTPUT METER
  mMeterR = new IBitmapControl(this, kMeterRX, kMeterRY, &levelMeterR);
  pGraphics->AttachControl(mMeterR);
  mMeterR->SetTargetArea(zr);
  
  
  // attach graphics context
  AttachGraphics(pGraphics);
  
  
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
      double preGain = DBToAmp(mDriveSmoother->process(mDrive));
      double gainComp = -preGain;
      
  
      double sample = *input;
      double drySample = sample;
    
      
      //Apply drive
      sample*=preGain;
      
      //Soft Asymmetric Distortion
      if (mDistType==1) {
        double threshold = 0.9;
  
        if(sample>threshold)
          sample = threshold + (sample - threshold) / (1 + pow(((sample - threshold)/(1 - threshold)), 2));
        else if(sample >1)
          sample = (sample + 1)/2;
      }
      
      
      
      // Symmetric Distortion
      if(mDistType==2){
        double amount = 3;
        sample =  fastAtan(sample * amount);
      }
    
    

     //Sine Shaper
      if(mDistType==3){
        double amount = 1.6;
        double z = M_PI * amount/4.0;
        double s = 1/sin(z);
        double b = 1 / amount;
      
        if (sample>b)
          sample = sample + (1-sample)*0.8;
        else if (sample < - b)
          sample = sample + (-1-sample)*0.8;
        else
          sample = sin(z * sample) * s;
        
        sample *= pow(10, -amount/20.0);
      }
      
      
        //FoldBack Distortion
      if (mDistType==4) {
        double threshold = .9;
        if (sample > threshold || sample < - threshold) {
          sample = fabs(fabs(fmod(sample - threshold, threshold * 4)) - threshold * 2) - threshold;
        }
        
      }
      

      //filters
      sample=mlowPass->processAudioSample(sample, i);
      sample=mhighPass->processAudioSample(sample, i);
      
      
      //Apply Gain Compensation
      sample*=gainComp;
  
      
      
      //Mix
      double mix= mMixSmoother->process(mMix);
      sample=mix*sample + (1.0-mix)*drySample;
      
      
      //Apply Post Gain
      sample *= DBToAmp(mOutputGainSmoother->process(mOutputGain));

      
      //Clipping
      if(mClipEnabled)
      {
        if(sample>mClipLevel)
          sample=mClipLevel;
        else if(sample<-1*mClipLevel)
          sample=-1*mClipLevel;
      }
      
      
      
      //Update Meters
      if(GetGUI()){
        if (i==0) {
          mMeterL->SetValueFromPlug(log10(mPeakFollower->process2(*input, GetSampleRate())) +1);

        }
        else{
          mMeterR->SetValueFromPlug(log10(mPeakFollower2->process(sample, GetSampleRate())) +1);
        }
        
      }
      
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
      //mDrive = mDriveSmoother->process(GetParam(kDrive)->Value());
      mDrive=GetParam(kDrive)->Value();
      break;

    case kMix:
      mMix = (GetParam(kMix)->Value()/100.0);
      break;
    
    case kClipLevel:
      mClipLevel=pow(10, GetParam(kClipLevel)->Value()/20);
      break;
      
      
    case kOutputGain:
      mOutputGain = GetParam(kOutputGain)->Value() ;
      break;
      
      
    case kDistType:
      mDistType = GetParam(kDistType)->Value();
      if(GetGUI()) mDisplay->SetValueFromPlug((mDistType-1)/3.0);
      break;
      
    case kLowPassCutoff:
      mLowPassCutoff=GetParam(kLowPassCutoff)->Value();
      mlowPass->setCutoffFreq(mLowPassCutoff);
      break;
      
    case kHighPassCutoff:
      mHighPassCutoff=GetParam(kHighPassCutoff)->Value();
      mhighPass->setCutoffFreq(mHighPassCutoff);
      break;
      
    case kClipEnabled:
      mClipEnabled = GetParam(kClipEnabled)->Value();
      break;
  
    default:
      break;
  }
}

double GritDistortion::fastAtan(double x){
  return (x / (1.0 + 0.28 * (x * x)));
}



