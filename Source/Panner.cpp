/*
  ==============================================================================

    stereo_panner.cpp
    Created: 22 Sep 2022 8:35:39pm
    Author:  Quentin Prost

  ==============================================================================
*/

#include "Panner.h"

template <typename SampleType> Panner<SampleType>::Panner() {}

template <typename SampleType> Panner<SampleType>::~Panner() {}

template <typename SampleType> void Panner<SampleType>::set_pan(float pan) {
    
    switch (m_method) {
        case MONO_PANNER:
            mono_panner.setPan(pan);
            break;
        case STEREO_PANNER:
            stereo_panner.set_pan(pan);
            break;
        case BINAURAL_PANNER:
            break;
        default:
            break;
    }
}

template <typename SampleType> void Panner<SampleType>::set_width(float width) {
    //if (m_method != STEREO_PANNER) return;
    stereo_panner.set_width(width);
    m_width = width;
}

template <typename SampleType> float Panner<SampleType>::get_width() {
    return m_width;
}

template <typename SampleType> void Panner<SampleType>::set_pan_method(panMethod method) {
    m_method = method;
    switch (method) {
        case MONO_PANNER:
            mono_panner.setPan(m_pan);
            break;
        case STEREO_PANNER:
            stereo_panner.set_pan(m_pan);
            stereo_panner.set_width(m_width);
            break;
        default:
            break;
    }
};

template <typename SampleType> panMethod Panner<SampleType>::get_pan_method() {
    return m_method;
};

template <typename SampleType> void Panner<SampleType>::set_mono_panner_rule(juce::dsp::PannerRule rule) {
    mono_panner.setRule(rule);
    mono_panner.setPan(m_pan);
}

template <typename SampleType> void Panner<SampleType>::set_stereo_panner_rule(stereoPannerRule rule) {
    stereo_panner.set_rule(rule);
    stereo_panner.set_pan(m_pan);
}

template <typename SampleType> void Panner<SampleType>::prepare(juce::dsp::ProcessSpec &spec) {
    mono_panner.prepare(spec);
    stereo_panner.prepare(spec);
}

template <typename SampleType> void Panner<SampleType>::process(juce::dsp::ProcessContextReplacing<SampleType> &context) {
    
    switch (m_method) {
        case MONO_PANNER:
            mono_panner.process(context);
            break;
        case STEREO_PANNER:
            stereo_panner.process(context);
            break;
        case BINAURAL_PANNER:
            //process_binaural_pan(context);
        default:
            jassert("Wrong type panning");
            break;
    }
};


template class Panner<float>;
template class Panner<double>;
