#ifndef __MULTIDISTORTION__
#define __MULTIDISTORTION__

#include "IPlug_include_in_plug_hdr.h"

#define WDL_BESSEL_FILTER_ORDER 8
#define WDL_BESSEL_DENORMAL_AGGRESSIVE
#include "PeakFollower.h"
#include "VAStateVariableFilter.h"
#include "CParamSmooth.h"

class MultiDistortion : public IPlug
{
public:
  MultiDistortion(IPlugInstanceInfo instanceInfo);
  ~MultiDistortion();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  double fastAtan(double x);
  
private:
  
  const double mDC;
  

  double mDrive;
  double mMix;
  int mDistType;
  double mAmount;
  double mClipLevel;
  bool mClipEnabled;
  bool mFat;
  double mMeterLValue;
  double mMeterRValue;
  double mHighPassCutoff;
  double mLowPassCutoff;
  bool mSmoothFilter;
  
  IBitmapControl* mMeterR;
  IBitmapControl* mMeterL;

  IBitmapControl* mDisplay;
  
  PeakFollower* mPeakFollower;
  PeakFollower* mPeakFollower2;
  
  VAStateVariableFilter* mlowPass;
  VAStateVariableFilter* mhighPass;
  VAStateVariableFilter* mlowPeak;
  
  CParamSmooth* mDriveSmoother;
  CParamSmooth* mMixSmoother;
  CParamSmooth* mHPFSmoother;
  CParamSmooth* mLPFSmoother;

  

};

#endif
