/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "Panner.h"

namespace ParameterID
{
#define PARAMETER_ID(str) const juce::ParameterID str(#str, 1);
    PARAMETER_ID(panValue)
    PARAMETER_ID(panMethod)
    PARAMETER_ID(widthValue)
    PARAMETER_ID(monoPannerRule)
    PARAMETER_ID(stereoPannerRule)
    PARAMETER_ID(binauralPannerRule)
    
}

class True_stereo_pannerAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    True_stereo_pannerAudioProcessor();
    ~True_stereo_pannerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void set_pan_method(panMethod method);
    void set_pan(float pan);
    
    juce::AudioProcessorValueTreeState apvts;
    //juce::LinearSmoothedValue<double> ;
private:
    Panner<float> panner;
    panMethod m_method;
    monoPannerRule m_mono_rule;
    stereoPannerRule m_stereo_rule;
    binauralPannerRule m_binaural_rule;
    float m_pan, m_width;
    
    // Editor //
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (True_stereo_pannerAudioProcessor)
};
