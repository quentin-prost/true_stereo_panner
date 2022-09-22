/*
  ==============================================================================

    stereo_panner.cpp
    Created: 22 Sep 2022 8:35:39pm
    Author:  Quentin Prost

  ==============================================================================
*/

#include "Stereo_panner.h"

template <class SampleType> Stereo_panner<SampleType>::Stereo_panner() {
    set_pan(0.0);
    set_pan_rule(LINEAR);
    set_pan_type(BALANCED_KNOB);
}

template <class SampleType> Stereo_panner<SampleType>::~Stereo_panner() {}

template <class SampleType> void Stereo_panner<SampleType>::process_panner(const juce::dsp::ProcessContextNonReplacing<SampleType> &context) {
    
    switch (type) {
        case BALANCED_KNOB:
            process(context);
            break;
        case TRUE_STEREO:
            process_true_stereo(context);
            break;
        case BINAURAL_PAN:
            process_binaural_pan(context);
        default:
            jassert("Wrong type panning");
            break;
    }
};

//template <class SampleType> void Stereo_panner<SampleType>::set_pan_type(panType type) {
//
//    switch (type) {
//        case BALANCED_KNOB:
//            setPan(
//            break;
//       case TRUE_STEREO:
//        default:
//            break;
//    }
//};
