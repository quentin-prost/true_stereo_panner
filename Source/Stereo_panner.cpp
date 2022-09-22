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
