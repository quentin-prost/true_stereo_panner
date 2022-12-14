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

template <typename SampleType> void Panner<SampleType>::prepare(juce::dsp::ProcessSpec &spec) {
    m_spec.sampleRate = spec.sampleRate;
    m_spec.maximumBlockSize = spec.maximumBlockSize;
    m_spec.numChannels = spec.numChannels;
    mono_panner.prepare(spec);
    stereo_panner.prepare(spec);
    lfo.prepare_lfo(spec);
}

template <typename SampleType> void Panner<SampleType>::set_stereo_width(float width) {
    stereo_panner.set_width(width);
    m_width = width;
}

template <typename SampleType> void Panner<SampleType>::set_pan_method(panMethod method) {
    DBG(method);
    m_method = method;
    switch (m_method) {
        case MONO_PANNER:
            mono_panner.setPan(m_pan);
            break;
        case STEREO_PANNER:
            stereo_panner.set_pan(m_pan);
            break;
        default:
            break;
    }
};

template <typename SampleType> void Panner<SampleType>::set_mono_panner_rule(juce::dsp::PannerRule rule) {
    m_mono_rule = static_cast<monoPannerRule>(rule);
    mono_panner.setRule(rule);
}

template <typename SampleType> void Panner<SampleType>::set_stereo_panner_rule(stereoPannerRule rule) {
    m_stereo_rule = rule;
    stereo_panner.set_rule(rule);
}

template <typename SampleType> void Panner<SampleType>::process(juce::dsp::ProcessContextReplacing<SampleType> &context) {
    float modulated_pan;
    
    if (lfo.is_active()) {
        modulated_pan = lfo.process_lfo(0.0f);
        juce::jlimit(static_cast<float>(-1.0), static_cast<float>(1.0), static_cast<float>(modulated_pan));
        set_pan(modulated_pan);
        lfo.skip_samples(m_spec.maximumBlockSize); // skip this amount of sample in the phase, as we are not calling the lfo only once per block. Probably some design choice to reconsider, and build a lfo directly inside the mono_panner & stereo_panner and process the lfo every sample.
        //DBG(modulated_pan);
    }
    
    switch (m_method) {
        case MONO_PANNER:
            mono_panner.process(context);
            break;
        case STEREO_PANNER:
            stereo_panner.process(context);
            break;
//        case BINAURAL_PANNER:
//            //process_binaural_pan(context);
        default:
//            jassertfalse;
            break;
    }
};


template class Panner<float>;
template class Panner<double>;
