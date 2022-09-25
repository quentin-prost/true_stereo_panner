/*
  ==============================================================================

    BinauralPanner.h
    Created: 23 Sep 2022 9:22:05pm
    Author:  Student

  ==============================================================================
*/

#include <JuceHeader.h>
#pragma once

template <typename SampleType> struct binaural_panner_state {
    SampleType gain_ll;
    SampleType gain_rr;
    SampleType gain_lr;
    SampleType gain_rl;
};

typedef enum {
    BINAURAL_DEFAULT = 0,
    BINAURAL_LINEAR,
    BINAURAL_SIN3DB
} binauralPannerRule;

template <typename SampleType> class BinauralPanner {
public:
    BinauralPanner();
    ~BinauralPanner();
    
    void set_pan(float pan);
    float get_pan();
    void process(const juce::dsp::ProcessContextNonReplacing<SampleType> &context);

private:
    float pan, width;
    struct binaural_panner_state<SampleType> state;
};
