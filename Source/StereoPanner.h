/*
  ==============================================================================

    StereoPanner.h
    Created: 23 Sep 2022 9:21:58pm
    Author:  Student

  ==============================================================================
*/

#include <JuceHeader.h>
#pragma once


template <typename SampleType> struct stereo_panner_state {
    SampleType gain_ll;
    SampleType gain_rr;
    SampleType gain_lr;
    SampleType gain_rl;
    SampleType coef_side;
    SampleType coef_mid = static_cast<SampleType>(0.5);
};

template <typename SampleType> class StereoPanner {    
public:
    StereoPanner();
    ~StereoPanner();
    
    void prepare(const juce::dsp::ProcessSpec &spec);
    void set_width(float width);
    float get_width();
    void set_pan(float pan);
    float get_pan();
    void process(const juce::dsp::ProcessContextReplacing<SampleType> &context);
    
private:
    float m_pan, m_width;
    stereo_panner_state<SampleType> m_state;
    juce::dsp::ProcessSpec m_spec;
};
