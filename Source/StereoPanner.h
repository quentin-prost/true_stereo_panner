/*
  ==============================================================================

    StereoPanner.h
    Created: 23 Sep 2022 9:21:58pm
    Author:  Student

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

template <typename SampleType> struct stereo_panner_state {
    SampleType gain_ll;
    SampleType gain_rr;
    SampleType gain_lr;
    SampleType gain_rl;
    SampleType coef_side = static_cast<SampleType>(0.5);
    SampleType coef_mid = static_cast<SampleType>(0.5);
};

typedef enum {
    STEREO_LINEAR,
    STEREO_SIN3DB
} stereoPannerRule;

template <typename SampleType> class StereoPanner {    
public:
    StereoPanner();
    ~StereoPanner();
    
    void prepare(const juce::dsp::ProcessSpec &spec);
    void set_width(float width);
    float get_width();
    void set_pan(float pan);
    float get_pan();
    void set_rule(stereoPannerRule rule);
    stereoPannerRule get_rule();
    void process(const juce::dsp::ProcessContextReplacing<SampleType> &context);
    
private:
    float m_pan = 0.0f, m_width = 1.0f;
    stereoPannerRule m_rule = STEREO_LINEAR;
    stereo_panner_state<SampleType> m_state;
    juce::dsp::ProcessSpec m_spec;
};
