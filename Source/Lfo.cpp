/*
  ==============================================================================

    Lfo.cpp
    Created: 25 Sep 2022 5:43:59pm
    Author:  Quentin Prost

  ==============================================================================
*/

#include "Lfo.h"


void Lfo::prepare_lfo(juce::dsp::ProcessSpec& spec)
{
    set_waveform(SINE);
    set_lfo_rate(1.0f * spec.sampleRate / spec.maximumBlockSize);
    frequency.reset(spec.sampleRate, 0.001);
    prepare(spec);
}

void Lfo::set_waveform(waveform_t waveform) {
    
    switch (waveform) {
        case SINE:
            initialise([](float x) {return std::sin (x); }, 0);
            break;
        case SAW:
            initialise([](float x) {return x / juce::MathConstants<float>::pi; }, 0 );
            break;
        case SQUARE:
            initialise([](float x) {return x < 0.0f ? -1.0f : 1.0f; });
            break;
        case TRIANGLE:
            initialise([](float x) {return x < 0.0f ? juce::MathConstants<float>::halfPi * x + 1 : - juce::MathConstants<float>::halfPi * x + 1; });
            break;
        default:
            jassertfalse;
            break;
    }
    m_waveform = waveform;
    reset();
}

void Lfo::set_active(bool active) {
    m_active = active;
}

void Lfo::set_lfo_amount(float amount) {
    juce::jlimit(0.0f, 1.0f, amount);
    m_amount = amount;
}

void Lfo::set_lfo_rate(float rate) {
    setFrequency(rate);
    m_rate = rate;
}

float Lfo::process_lfo(float input) {
    float output = processSample(input);
    output *= m_amount;
    return output;
}


