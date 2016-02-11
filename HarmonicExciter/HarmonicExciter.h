#ifndef __HARMONICEXCITER__
#define __HARMONICEXCITER__

#include "IPlug_include_in_plug_hdr.h"
#include "VAStateVariableFilter.h"

class HarmonicExciter : public IPlug
{
public:
  HarmonicExciter(IPlugInstanceInfo instanceInfo);
  ~HarmonicExciter();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  
private:
  
  VAStateVariableFilter* mHighPass;
  double mDrive;
  double mFreq;
  double mMix;
  int mType;
  bool mHarmOnly;
  
};

#endif
