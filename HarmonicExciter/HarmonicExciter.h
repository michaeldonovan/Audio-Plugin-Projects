#ifndef __HARMONICEXCITER__
#define __HARMONICEXCITER__

#include "IPlug_include_in_plug_hdr.h"

#define WDL_BESSEL_FILTER_ORDER 8
#define WDL_BESSEL_DENORMAL_AGGRESSIVE
#include "Biquad.h"

class HarmonicExciter : public IPlug
{
public:
  HarmonicExciter(IPlugInstanceInfo instanceInfo);
  ~HarmonicExciter();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  
private:
  

  Biquad highPass;
  
  double mDrive;
  double mFreq;
  double mMix;
  int mType;
  
};

#endif
