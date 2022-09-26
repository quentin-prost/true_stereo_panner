/*
  ==============================================================================

    Lfo.h
    Created: 25 Sep 2022 5:03:35pm
    Author:  Student

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

typedef enum {
    SINE = 0,
    SAW,
    SQUARE,
    TRIANGLE
} waveform_t;

/* create an lfo class derived from the juce oscillator class
   Oscillator shape is based on an user provided function */

class Lfo : juce::dsp::Oscillator<float> {
public:
    
    Lfo() {
        set_waveform(m_waveform);
    };
    ~Lfo() {};
    
    void prepare_lfo(juce::dsp::ProcessSpec& spec);
    void reset_lfo() { reset(); };
    void set_active(bool active);
    void set_waveform(const waveform_t waveform);
    void set_lfo_amount(const float amount);
    void set_lfo_rate(float rate);
    bool is_active() { return m_active; };
    
    float process_lfo(float input);
private:
    float m_amount = 0.0f;
    float m_rate = 1.0f;
    bool m_active = false;
    waveform_t m_waveform = SINE;
};

