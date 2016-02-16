//
//  PeakFollower.cpp
//  GritDistortion
//
//
#include <math.h>
#include "PeakFollower.h"

const float HALF_LIFE = 0.3;
const float VERY_SMALL_FLOAT = 0.0001;

// halfLife = time in seconds for output to decay to half value after an impulse

PeakFollower::PeakFollower(){};

float PeakFollower::process(double input, double sampleRate){
    static float output = 0.0;
    float scalar = pow( 0.5, 1.0/(HALF_LIFE * sampleRate));
    
    if( input < 0.0 )
        input = -input;  /* Absolute value. */
    
    if ( input >= output )
    {
        /* When we hit a peak, ride the peak to the top. */
        output = input;
    }
    else
    {
        /* Exponential decay of output when signal is low. */
        output = output * scalar;
        /*
         ** When current gets close to 0.0, set current to 0.0 to prevent FP underflow
         ** which can cause a severe performance degradation due to a flood
         ** of interrupts.
         */
        if( output < VERY_SMALL_FLOAT ) output = 0.0;
    }
    return output;
}

float PeakFollower::process2(double input, double sampleRate){
    static float output = 0.0;
    float scalar = pow( 0.5, 1.0/(HALF_LIFE * sampleRate));
    
    if( input < 0.0 )
        input = -input;  /* Absolute value. */
    
    if ( input >= output )
    {
        /* When we hit a peak, ride the peak to the top. */
        output = input;
    }
    else
    {
        /* Exponential decay of output when signal is low. */
        output = output * scalar;
        /*
         ** When current gets close to 0.0, set current to 0.0 to prevent FP underflow
         ** which can cause a severe performance degradation due to a flood
         ** of interrupts.
         */
        if( output < VERY_SMALL_FLOAT ) output = 0.0;
    }
    return output;
}