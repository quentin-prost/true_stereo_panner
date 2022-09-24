/*
  ==============================================================================

    StereoPanner.cpp
    Created: 23 Sep 2022 9:21:58pm
    Author:  Student

  ==============================================================================
*/

#include "StereoPanner.h"

template <typename SampleType> StereoPanner<SampleType>::StereoPanner() {
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
    juce::dsp::AudioBlock<SampleType> output = context.getOutputBlock();
    if (!context.isBypassed) {
        for (auto sample = 0; sample < m_spec.maximumBlockSize; sample ++) {
            SampleType outputLeft = context.getInputBlock().getSample(0, sample) * m_state.gain_ll + context.getInputBlock().getSample(1, sample) * m_state.gain_lr;
            SampleType outputRight = context.getInputBlock().getSample(1, sample) * m_state.gain_rr + context.getInputBlock().getSample(0, sample) * m_state.gain_rl;
            
            SampleType mid = (outputLeft + outputRight)*m_state.coef_mid;
            SampleType side = (outputLeft - outputRight)*m_state.coef_side;
            
            output.setSample(0, sample, mid + side);
            output.setSample(1, sample, mid - side);
        }
    } else {
        output.copyFrom(context.getInputBlock());
    }
}

template class StereoPanner<float>;
template class StereoPanner<double>;

