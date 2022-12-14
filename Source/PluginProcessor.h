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
    PARAMETER_ID(lfoRateHz)
    PARAMETER_ID(lfoRateSync)
    PARAMETER_ID(lfoAmount)
    PARAMETER_ID(lfoActive)
    PARAMETER_ID(lfoSynced)
    PARAMETER_ID(lfoWaveform)
}

template <typename T>
inline static void castParameter(juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& Id, T& dest) {
    dest = dynamic_cast<T>(apvts.getParameter(Id.getParamID()));
    jassert(dest);
}

class True_stereo_pannerAudioProcessor  : public juce::AudioProcessor,
                                          public juce::AudioProcessorValueTreeState::Listener
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
    
    juce::AudioProcessorValueTreeState apvts;
private:
    Panner<float> panner;
    float last_bpm = 0.0f, last_rate_hz;
    sync_rate_t last_rate_sync;
    
    // Play head to retrieve BPM from host
    juce::AudioPlayHead::CurrentPositionInfo* current_position_info;
    
    // Parameters //
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    juce::AudioParameterFloat* pan_param;
    juce::AudioParameterFloat* width_param;
    juce::AudioParameterChoice* pan_method_param;
    juce::AudioParameterChoice* mono_rule_param;
    juce::AudioParameterChoice* stereo_rule_param;
    juce::AudioParameterBool* lfo_active_param;
    juce::AudioParameterBool* lfo_synced_param;
    juce::AudioParameterChoice* lfo_waveform_param;
    juce::AudioParameterFloat* lfo_rate_hz_param;
    juce::AudioParameterChoice* lfo_rate_sync_param;
    juce::AudioParameterFloat* lfo_amount_param;
    
    
    //std::atomic<bool> parametersChanged { false };
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (True_stereo_pannerAudioProcessor)
};
