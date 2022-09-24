/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
True_stereo_pannerAudioProcessorEditor::True_stereo_pannerAudioProcessorEditor (True_stereo_pannerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor(p), panSlider(), monoPannerRuleChoice("panRule")
{
//    panSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
//    panSlider.setRotaryParameters(3*juce::MathConstants<float>::pi/2, juce::MathConstants<float>::pi/2, true);
//    panSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
//    addAndMakeVisible(panSlider);
//
//    panMethodChoice.addItem("Balanced Knob", MONO_PANNER + 1); // +1 because ID cannot be 0.
//    panMethodChoice.addItem("Stereo Panner", STEREO_PANNER + 1);
//    panMethodChoice.addItem("Binaural Panner", BINAURAL_PANNER + 1);
//    addAndMakeVisible(panMethodChoice);
    
//    monoPannerRuleChoice.addItem("Linear", MONO_LINEAR + 1);
//    monoPannerRuleChoice.addItem("Balanced", MONO_BALANCED + 1);
//    monoPannerRuleChoice.addItem("sin3dB", MONO_SIN3DB + 1);
//    monoPannerRuleChoice.addItem("sin4p5db", MONO_SIN4P5DB + 1);
//    monoPannerRuleChoice.addItem("sin6dB", MONO_SIN6DB + 1);
//    monoPannerRuleChoice.addItem("squareroot3dB", MONO_SQUAREROOT3DB + 1);
//    monoPannerRuleChoice.addItem("squareroot4p5db", MONO_SQUAREROOT4P5DB + 1);
//
//    addAndMakeVisible(monoPannerRuleChoice);
    
//    panAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, ParameterID::panValue.getParamID(), panSlider);
//
//    monoPannerRuleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, ParameterID::monoPannerRule.getParamID(), monoPannerRuleChoice);
//
//    panMethodAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, ParameterID::panMethod.getParamID(), panMethodChoice);
//

    setSize (400, 300);
}
True_stereo_pannerAudioProcessorEditor::~True_stereo_pannerAudioProcessorEditor()
{
}

//==============================================================================
void True_stereo_pannerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void True_stereo_pannerAudioProcessorEditor::resized()
{
    panSlider.setBounds(getWidth() / 2 - 100, getHeight() / 2 - 50, 200, 100);
    panMethodChoice.setBounds(panSlider.getX(), panSlider.getY() - 50, 200, 50);
}
