/*
  ==============================================================================

    stereo_panner.h
    Created: 22 Sep 2022 8:35:39pm
    Author:  Quentin Prost

  ==============================================================================
*/

#include <JuceHeader.h>

#pragma once

typedef enum {
    BALANCED_KNOB = 0,
    TRUE_STEREO,
    BINAURAL_PAN
} panType;

typedef enum {
    DEFAULT = 0,
    LINEAR,
    BALANCED,
    SIN3DB,
    SIN4P5DB,
    SIN6DB,
    SQUAREROOT3DB,
    SQUAREROOT4P5DB
} panRule;

template <typename SampleType> class Stereo_panner : public juce::dsp::Panner<SampleType> {
    
public:
    Stereo_panner();
    ~Stereo_panner();
    
    void prepare(const juce::dsp::ProcessSpec & spec) override;
    void process(const juce::dsp::ProcessContextNonReplacing<SampleType> &context) override;
    
    void set_pan_type(panType type);
    panType get_pan_type();
    void set_pan_rule(panRule rule);
    panRule get_pan_rule();
    void set_pan(SampleType pan);
    SampleType get_pan();
    
    void process_balanced_know(const juce::dsp::ProcessContextNonReplacing<SampleType> &context);
    void process_true_stereo(const juce::dsp::ProcessContextNonReplacing<SampleType> &context);
    void process_binaural_pan(const juce::dsp::ProcessContextNonReplacing<SampleType> &context);
private:
    SampleType pan;
    float width;
    panType type;
    panRule rule;
};
