#ifndef __STEREOIMAGER__
#define __STEREOIMAGER__

#include "IPlug_include_in_plug_hdr.h"
#include "CParamSmooth.h"
class StereoImager : public IPlug
{
public:
  StereoImager(IPlugInstanceInfo instanceInfo);
  ~StereoImager();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  CParamSmooth* mWidthSmoother;
  double mStereoWidth;
};

#endif
