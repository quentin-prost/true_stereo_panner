/*
  ==============================================================================

    stereo_panner.cpp
    Created: 22 Sep 2022 8:35:39pm
    Author:  Quentin Prost

  ==============================================================================
*/

#include "Panner.h"

template <class SampleType> Panner<SampleType>::Panner() {}

template <class SampleType> Panner<SampleType>::~Panner() {}

template <class SampleType> void Panner<SampleType>::set_pan(float pan) {
    
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

template <class SampleType> float Panner<SampleType>::get_pan() {

    switch (m_method) {
        case MONO_PANNER:
            return mono_panner.getPan(m_pan);
        case STEREO_PANNER:
            return stereo_panner.get_pan(m_pan);
            break;
        case BINAURAL_PANNER:
            break;
        default:
            break;
    }
}

template <class SampleType> void Panner<SampleType>::set_pan_method(panMethod method) {
    m_method = method;
};

template <class SampleType> panMethod Panner<SampleType>::get_pan_method() {
    return m_method;
};

template <class SampleType> void Panner<SampleType>::prepare(juce::dsp::ProcessSpec &spec) {
    mono_panner.prepare(spec);
    stereo_panner.prepare(spec);
}

template <class SampleType> void Panner<SampleType>::process(juce::dsp::ProcessContextReplacing<SampleType> &context) {
    
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


