/*
  ==============================================================================

    stereo_panner.h
    Created: 22 Sep 2022 8:35:39pm
    Author:  Quentin Prost

  ==============================================================================
*/

#include <JuceHeader.h>
#include "StereoPanner.h"
#include "BinauralPanner.h"

#pragma once

typedef enum {
    MONO_PANNER = 0,
    STEREO_PANNER,
    BINAURAL_PANNER
} panMethod;

typedef enum {
    MONO_DEFAULT = 0,
    MONO_LINEAR,
    MONO_BALANCED,
    MONO_SIN3DB,
    MONO_SIN4P5DB,
    MONO_SIN6DB,
    MONO_SQUAREROOT3DB,
    MONO_SQUAREROOT4P5DB
} monoPannerRule;

typedef enum {
    STEREO_DEFAULT = 0,
    STEREO_LINEAR,
    STEREO_SIN3DB
} stereoPannerRule;

typedef enum {
    BINAURAL_DEFAULT = 0,
    BINAURAL_LINEAR,
    BINAURAL_SIN3DB
} binauralPannerRule;


template <typename SampleType> class Panner {
    
public:
    Panner();
    ~Panner();
    
    void process_panner(const juce::dsp::ProcessContextNonReplacing<SampleType> &context);
    
    void set_pan_method(panMethod type);
    panMethod get_pan_method();
    
    void set_pan(float pan);
    float get_pan();
    
    void set_width(float width);
    float get_width();
    
    void set_mono_panner_rule(juce::dsp::PannerRule rule);
    monoPannerRule get_mono_panner_rule();
    
    void set_stereo_panner_rule(stereoPannerRule rule);
    stereoPannerRule get_stereo_panner_rule(stereoPannerRule rule);
    
    void set_binaural_panner_rule(binauralPannerRule rule);
    binauralPannerRule get_binaural_panner_rule();
    
    void prepare(juce::dsp::ProcessSpec &spec);
    void process(juce::dsp::ProcessContextReplacing<SampleType> &context);
private:
    float m_pan, m_width;
    panMethod m_method;
    juce::dsp::Panner<SampleType> mono_panner;
    StereoPanner<SampleType> stereo_panner;
    //BinauralPanner<SampleType> binaural_panner;
    
};
