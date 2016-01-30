//
//  PeakFollower.hpp
//  MultiDistortion
//
//

#ifndef PeakFollower_hpp
#define PeakFollower_hpp

class PeakFollower {
public:
    PeakFollower();
    ~PeakFollower();
    float process(double input, double sampleRate);
};



#endif /* PeakFollower_h */

