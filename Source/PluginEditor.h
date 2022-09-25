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
    True_stereo_pannerAudioProcessor& audioProcessor;
    
//    juce::Slider panSlider; // Slider object
//    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> panAttachment; // To be able to keep track on the slider value into the apvst of audio processor
//
//    juce::ComboBox monoPannerRuleChoice;
//    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> monoPannerRuleAttachment;
//
//    juce::ComboBox panMethodChoice;
//    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> panMethodAttachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (True_stereo_pannerAudioProcessorEditor)
};
