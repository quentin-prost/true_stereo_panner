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
#include "Lfo.h"
#pragma once

typedef enum {
    MONO_PANNER = 0,
    STEREO_PANNER,
    BINAURAL_PANNER
} panMethod;

typedef enum {
    MONO_LINEAR,
    MONO_BALANCED,
    MONO_SIN3DB,
    MONO_SIN4P5DB,
    MONO_SIN6DB,
    MONO_SQUAREROOT3DB,
    MONO_SQUAREROOT4P5DB
} monoPannerRule;

typedef enum {
    T1_64 = 0, T1_48, T1_32, T1_16, T1_12, T1_8, T1_6, T1_4,
    T1_3, T1_2, T1_1, T2_1, T3_1, T4_1, T6_1, T8_1, T16_1
} sync_rate_t;

template <typename SampleType> class Panner {
public:
    Panner();
    ~Panner();
    
    void prepare(juce::dsp::ProcessSpec &spec);
    void process(juce::dsp::ProcessContextReplacing<SampleType> &context);
    
    void set_pan_method(panMethod type);
    panMethod get_pan_method() { return m_method; };
    void set_pan(float pan);
    void set_stereo_width(float width);
    void set_lfo_active(bool active) { is_lfo_active = active; };
    void set_lfo_rate_hz(float rate) {
        juce::jlimit(0.01f, 20.0f, rate);
        m_lfo_rate = rate;
        lfo.set_lfo_rate(m_lfo_rate);
    };
    
    void set_lfo_amount(float amount) {
        juce::jlimit(0.0f, 1.0f, amount);
        m_lfo_amount = amount;
        lfo.set_lfo_amount(m_lfo_amount);
    }
    
    
    void set_mono_panner_rule(juce::dsp::PannerRule rule);
    monoPannerRule get_mono_panner_rule();
    
    void set_stereo_panner_rule(stereoPannerRule rule);
    stereoPannerRule get_stereo_panner_rule(stereoPannerRule rule);
    
    //void set_binaural_panner_rule(binauralPannerRule rule);
    //binauralPannerRule get_binaural_panner_rule();
    
private:
    float m_pan, m_width;
    panMethod m_method;
    juce::dsp::Panner<SampleType> mono_panner;
    StereoPanner<SampleType> stereo_panner;
    Lfo lfo;
    bool is_lfo_active = false;
    bool lfo_sync = false;
    float m_lfo_amount, m_lfo_rate;
    
        
};
