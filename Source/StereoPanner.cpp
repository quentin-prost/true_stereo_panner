/*
  ==============================================================================

    StereoPanner.cpp
    Created: 23 Sep 2022 9:21:58pm
    Author:  Quentin Prost

  ==============================================================================
*/

#include "StereoPanner.h"
#include <algorithm>

template <typename SampleType> StereoPanner<SampleType>::StereoPanner() {
    m_state.gain_ll.setCurrentAndTargetValue(1.0);
    m_state.gain_rr.setCurrentAndTargetValue(1.0);
    m_state.gain_rl.setCurrentAndTargetValue(0.0);
    m_state.gain_lr.setCurrentAndTargetValue(0.0);
    m_state.coef_mid.setCurrentAndTargetValue(0.5);
    m_state.coef_side.setCurrentAndTargetValue(0.5);
    set_pan(0.0f);
    set_rule(STEREO_LINEAR);
    set_width(1.0f);
};

template <typename SampleType> StereoPanner<SampleType>::~StereoPanner() {};

template <typename SampleType> void StereoPanner<SampleType>::prepare(const juce::dsp::ProcessSpec &spec) {
    m_spec.sampleRate = spec.sampleRate;
    m_spec.maximumBlockSize = spec.maximumBlockSize;
    m_spec.numChannels = spec.numChannels;
}

template <typename SampleType> void StereoPanner<SampleType>::reset() {
    m_state.gain_ll.reset(m_spec.maximumBlockSize);
    m_state.gain_rr.reset(m_spec.maximumBlockSize);
    m_state.gain_lr.reset(m_spec.maximumBlockSize);
    m_state.gain_rl.reset(m_spec.maximumBlockSize);
    m_state.coef_mid.reset(m_spec.maximumBlockSize);
    m_state.coef_side.reset(m_spec.maximumBlockSize); // State coefficient will interpolate only over one block
    set_pan(0.0f);
    set_width(1.0f);
}

template <typename SampleType> void StereoPanner<SampleType>::set_width(float width) {
    if (width > 2.0f) width = 2.0f;
    if (width < 0.0f) width = 0.0f;
    float temp = 1.0f / std::max((float)(1.0f + width), (float)2.0f);
    m_state.coef_mid.setTargetValue(static_cast<SampleType>(temp));
    m_state.coef_side.setTargetValue(static_cast<SampleType>(temp * width));
    m_width = width;
}

template <typename SampleType> float StereoPanner<SampleType>::get_width() {
    return m_width;
}

template <typename SampleType> void StereoPanner<SampleType>::set_pan(float pan) {
    SampleType x;
    juce::jlimit(-1.0f, 1.0f, pan);
    
    switch (m_rule) {
        case STEREO_LINEAR:
            x = static_cast<SampleType>(pan/2.0f + 0.5f);
            m_state.gain_ll.setTargetValue(static_cast<SampleType>(1.0f - x));
            m_state.gain_rr.setTargetValue(static_cast<SampleType>(x));
            if (pan < 0.0f) { // Add contribution of right channel into left channel only for negative panning
                m_state.gain_lr.setTargetValue(static_cast<SampleType>(0.0f));
                m_state.gain_rl.setTargetValue(static_cast<SampleType>(-sin(juce::MathConstants<SampleType>::pi/4 * pan)));
            } else { // Add contribuition of left channel into right channel only for positive panning
                m_state.gain_lr.setTargetValue(static_cast<SampleType>(sin(juce::MathConstants<SampleType>::pi/4 * pan)));
                m_state.gain_rl.setTargetValue(static_cast<SampleType>(0.0f));
            }
            break;
        case STEREO_SIN3DB:
            x = static_cast<SampleType>(juce::MathConstants<SampleType>::pi/4 * (1 - pan));
            m_state.gain_ll.setTargetValue(static_cast<SampleType>(std::sin(x)));
            m_state.gain_rr.setTargetValue(static_cast<SampleType>(std::cos(x)));
            if (pan < 0.0f) { // Add contribution of right channel into left channel only for negative panning
                m_state.gain_lr.setTargetValue(static_cast<SampleType>(0.0f));
                m_state.gain_rl.setTargetValue(static_cast<SampleType>(-std::sin(juce::MathConstants<SampleType>::pi/4 * pan)));
            } else { // Add contribuition of left channel into right channel only for positive panning
                m_state.gain_lr.setTargetValue(static_cast<SampleType>(std::sin(juce::MathConstants<SampleType>::pi/4 * pan)));
                m_state.gain_rl.setTargetValue(static_cast<SampleType>(0.0f));
            }
            break;
        default:
            jassertfalse;
            break;
    }
    
    m_pan = pan;
    
}

template <typename SampleType> float StereoPanner<SampleType>::get_pan_target() {
    return m_pan;
}

template <typename SampleType> void StereoPanner<SampleType>::set_rule(stereoPannerRule rule) {
    m_rule = rule;
}

template <typename SampleType> void StereoPanner<SampleType>::process(const juce::dsp::ProcessContextReplacing<SampleType> &context) {
    const auto& input = context.getInputBlock();
    auto& output = context.getOutputBlock();
    
    if (!context.isBypassed) {
        for (auto sample = 0; sample < output.getNumSamples(); sample++) {
            SampleType mid = (input.getSample(0, sample) + input.getSample(1, sample))*m_state.coef_mid.getNextValue();
            SampleType side = (input.getSample(0, sample) - input.getSample(1, sample))*m_state.coef_side.getNextValue();
            
            SampleType outputLeft = (mid + side) * m_state.gain_ll.getNextValue() + (mid - side) * m_state.gain_rl.getNextValue();
            SampleType outputRight = (mid - side) * m_state.gain_rr.getNextValue() + (mid + side) * m_state.gain_lr.getNextValue();
            
            output.setSample(0, sample, outputLeft);
            output.setSample(1, sample, outputRight);
        }
    } else {
        output.copyFrom(context.getInputBlock());
    }
}

template class StereoPanner<float>;
template class StereoPanner<double>;

