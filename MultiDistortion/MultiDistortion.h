#ifndef __MULTIDISTORTION__
#define __MULTIDISTORTION__

#include "IPlug_include_in_plug_hdr.h"

#define WDL_BESSEL_FILTER_ORDER 8
#define WDL_BESSEL_DENORMAL_AGGRESSIVE
#include "Biquad.h"
#include "PeakFollower.h"

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
  //const int mOversampling;
  
  const double mDC;
  double mDistortedDC;
  
  Biquad lowPeak;
  Biquad highShelf;
  
  double mDrive;
  double mMix;
  int mDistType;
  double mAmount;
  double mClipLevel;
  bool mClipEnabled;
  double mWarm;
  double mMeterValue;
  
  IBitmapControl* mMeterR;
  IBitmapControl* mMeterL;

  PeakFollower* mPeakFollower;
  PeakFollower* mPeakFollower2;

 // Filter mLowCutFilter;
 // Filter mHiCutFilter;
};

#endif
