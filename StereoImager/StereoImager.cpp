#include "StereoImager.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
  kStereoWidth = 0,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kStereoWidthX = 36 ,
  kStereoWidthY = 19,
  kKnobFrames = 65
};

StereoImager::StereoImager(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mStereoWidth(1.)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kStereoWidth)->InitDouble("Width", 100., 0., 200.0, 0.01, "%");
  GetParam(kStereoWidth)->SetShape(1.);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(BACKGROUND_ID, BACKGROUND_FN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kStereoWidthX, kStereoWidthY, kStereoWidth, &knob));

  AttachGraphics(pGraphics);

  mWidthSmoother = new CParamSmooth(5.0, GetSampleRate());
  
  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

StereoImager::~StereoImager() {}

void StereoImager::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];

  for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
  {
    double sampleLeft, sampleRight, width_coef, mid, side;
    
    sampleLeft = *in1;
    sampleRight = *in2;
    
    
    width_coef = mWidthSmoother->process(mStereoWidth*.5);
    
    mid = (sampleLeft  + sampleRight)*0.5;
    side = (sampleRight - sampleLeft )*width_coef;
    
    sampleLeft  = mid-side;
    sampleRight = mid+side;
    
    
    *out1 = sampleLeft;
    *out2 = sampleRight;
  }
}

void StereoImager::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void StereoImager::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kStereoWidth:
      mStereoWidth = GetParam(kStereoWidth)->Value() / 100.;
      break;

    default:
      break;
  }
}
