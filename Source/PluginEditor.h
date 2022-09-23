/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class True_stereo_pannerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    True_stereo_pannerAudioProcessorEditor (True_stereo_pannerAudioProcessor&);
    ~True_stereo_pannerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    True_stereo_pannerAudioProcessor& audioProcessor;
    
    juce::Slider panSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> panAttachment;
    
    juce::ComboBox panRuleChoice;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> panRuleAttachment;
    
    juce::ComboBox panMethodChoice;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> panMethodAttachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (True_stereo_pannerAudioProcessorEditor)
};
