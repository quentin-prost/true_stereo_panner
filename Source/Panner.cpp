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

template <typename SampleType> void Panner<SampleType>::set_stereo_width(float width) {
    stereo_panner.set_width(width);
    m_width = width;
}

template <typename SampleType> void Panner<SampleType>::set_pan_method(panMethod method) {
    m_method = method;
};

template <typename SampleType> void Panner<SampleType>::set_mono_panner_rule(juce::dsp::PannerRule rule) {
    mono_panner.setRule(rule);
}

template <typename SampleType> void Panner<SampleType>::set_stereo_panner_rule(stereoPannerRule rule) {
    stereo_panner.set_rule(rule);
}

template <typename SampleType> void Panner<SampleType>::prepare(juce::dsp::ProcessSpec &spec) {
    mono_panner.prepare(spec);
    stereo_panner.prepare(spec);
    lfo.prepare_lfo(spec);
}

template <typename SampleType> void Panner<SampleType>::process(juce::dsp::ProcessContextReplacing<SampleType> &context) {
    float modulated_pan;
    
    if (lfo.is_active()) {
        modulated_pan = lfo.process_lfo(0.0f);
        juce::jlimit(static_cast<float>(-1.0), static_cast<float>(1.0), static_cast<float>(modulated_pan));
        set_pan(modulated_pan);
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
            jassertfalse;
            break;
    }
};


template class Panner<float>;
template class Panner<double>;
