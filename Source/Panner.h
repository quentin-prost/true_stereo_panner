/*
  ==============================================================================

    stereo_panner.h
    Created: 22 Sep 2022 8:35:39pm
    Author:  Quentin Prost

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StereoPanner.h"
#include "BinauralPanner.h"
#include "Lfo.h"
#include "Utilities.h"


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
    
    void set_lfo_synced(bool synced, float bpm) {
        m_lfo_synced = synced;
        if (synced) {
            float rate_hz = get_rate_in_hz(m_rate_sync, m_bpm);
            set_lfo_rate_hz(rate_hz);
        } else {
            set_lfo_rate_hz(m_rate_hz);
        }
        lfo.reset_lfo();
    }
    void set_lfo_rate_synced(sync_rate_t rate, float bpm) {
        m_rate_sync = rate;
        m_bpm = bpm;
        float rate_hz = get_rate_in_hz(rate, m_bpm);
        lfo.set_lfo_rate(rate_hz);
    }
    void set_lfo_rate_hz(float rate) {
        juce::jmin(0.01f, rate);
        m_rate_hz = rate;
        lfo.set_lfo_rate(rate);
    }
    void set_lfo_waveform(waveform_t wave) {
        lfo.set_waveform(wave);
        lfo.reset_lfo();
    }
    void set_lfo_active(bool active) {
        lfo.set_active(active);
        lfo.reset_lfo();
    }
    bool get_lfo_active() {
        return lfo.is_active();
    }

    void set_lfo_amount(float amount) {
        juce::jlimit(0.0f, 1.0f, amount);
        lfo.set_lfo_amount(amount);
    }
    bool get_lfo_synced() {
        return m_lfo_synced;
    }
    void set_mono_panner_rule(juce::dsp::PannerRule rule);
    monoPannerRule get_mono_panner_rule();
    
    void set_stereo_panner_rule(stereoPannerRule rule);
    stereoPannerRule get_stereo_panner_rule(stereoPannerRule rule);
    
    //void set_binaural_panner_rule(binauralPannerRule rule);
    //binauralPannerRule get_binaural_panner_rule();
private:
    
    juce::dsp::ProcessSpec m_spec;
    float m_pan, m_width;
    
    panMethod m_method;
    juce::dsp::Panner<SampleType> mono_panner;
    StereoPanner<SampleType> stereo_panner;
    
    Lfo lfo;
    bool m_lfo_synced;
    float m_rate_hz; // rate in Hertz
    sync_rate_t m_rate_sync; // rate in bpm
    float m_bpm = 120.0f;

};
