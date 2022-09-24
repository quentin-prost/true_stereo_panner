/*
  ==============================================================================

    StereoPanner.cpp
    Created: 23 Sep 2022 9:21:58pm
    Author:  Student

  ==============================================================================
*/

#include "StereoPanner.h"

template <class SampleType> StereoPanner<SampleType>::StereoPanner() {
    m_state.gain_ll = static_cast<SampleType>(1.0);
    m_state.gain_rr = static_cast<SampleType>(1.0);
    m_state.gain_lr = static_cast<SampleType>(0.0);
    m_state.gain_rl = static_cast<SampleType>(0.0);
};

template <class SampleType> StereoPanner<SampleType>::~StereoPanner() {};

template <class SampleType> void StereoPanner<SampleType>::prepare(const juce::dsp::ProcessSpec &spec) {
    m_spec.sampleRate = spec.sampleRate;
    m_spec.maximumBlockSize = spec.maximumBlockSize;
    m_spec.numChannels = spec.numChannels;
}

template <class SampleType> void StereoPanner<SampleType>::set_width(float width) {
    
    if (width > 2.0f) width = 2.0f;
    if (width < 0.0f) width = 0.0f;
    m_state.coef_side = static_cast<SampleType>(0.5 * width);
}

template <class SampleType> float StereoPanner<SampleType>::get_width() {
    return m_width;
}

template <class SampleType> void StereoPanner<SampleType>::set_pan(float pan) {
    m_pan = pan;
    if (pan < -1.0f) pan = -1.0f;
    if (pan > 1.0f) pan = 1.0f;
    m_state.gain_ll = static_cast<SampleType>(1.0f - pan);
    m_state.gain_rr = static_cast<SampleType>(pan);
    if (pan < 0.0f) { // Add contribution of right channel into left channel only for negative panning
        m_state.gain_lr = static_cast<SampleType>(0.0f);
        m_state.gain_rl = static_cast<SampleType>(-sin(juce::MathConstants<SampleType>::halfPi * pan));
    } else { // Add contribuition of left channel into right channel only for positive panning 
        m_state.gain_lr = static_cast<SampleType>(-sin(juce::MathConstants<SampleType>::halfPi * pan));
        m_state.gain_rl = static_cast<SampleType>(0.0f);
    }
}

template <class SampleType> float StereoPanner<SampleType>::get_pan() {
    return m_pan;
}

template <class SampleType> void StereoPanner<SampleType>::process(const juce::dsp::ProcessContextReplacing<SampleType> &context) {
    auto input_left = context.getInputBlock().getSingleChannelBlock(0);
    auto input_right = context.getInputBlock().getSingleChannelBlock(1);
    auto output_left = context.getSingleOutputChannel(0);
    auto output_right = context.getSingleOutputChannel(1);
    if (!context.isBypassed) {
        output_left = input_left.multiplyBy(m_state.gain_ll) + input_right.multiplyBy(m_state.gain_lr); // left channel processing
        output_right = input_right.multiplyBy(m_state.gain_rr) + input_left.multiplyBy(m_state.gain_rl); // right channel processing
        
        // Width parameters
        auto mid = (output_left + output_right)*m_state.coef_mid;
        auto side = (output_left - output_right)*m_state.coef_side;
        output_left = (mid + side);
        output_right = (mid - side);
    } else {
        context.getOutputBlock() = context.getInputBlock();
    }
}

