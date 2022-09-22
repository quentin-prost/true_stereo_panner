/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
True_stereo_pannerAudioProcessorEditor::True_stereo_pannerAudioProcessorEditor (True_stereo_pannerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), panSlider(), panRuleChoice("panRule")
{
    panSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    panSlider.setRotaryParameters(-3*juce::MathConstants<float>::pi/4, 3*juce::MathConstants<float>::pi/4, true);
    panSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(panSlider);
    
    panRuleChoice.addItem("Linear", LINEAR + 1);
    panRuleChoice.addItem("Balanced", BALANCED + 1);
    panRuleChoice.addItem("sin3dB", SIN3DB + 1);
    panRuleChoice.addItem("sin4p5db", SIN4P5DB + 1);
    panRuleChoice.addItem("sin6dB", SIN6DB + 1);
    panRuleChoice.addItem("squareroot3dB", SQUAREROOT3DB + 1);
    panRuleChoice.addItem("squareroot4p5db", SQUAREROOT4P5DB + 1);
    
    addAndMakeVisible(panRuleChoice);
    
    panAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "pan", panSlider);
    
    panRuleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "panrule", panRuleChoice);
    
    
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
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void True_stereo_pannerAudioProcessorEditor::resized()
{
    panSlider.setBounds(getWidth() / 2 - 100, getHeight() / 2 - 50, 200, 100);
    panRuleChoice.setBounds(panSlider.getX(), panSlider.getY() - 50, 200, 50);
}
