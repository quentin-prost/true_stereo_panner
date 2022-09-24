/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
True_stereo_pannerAudioProcessor::True_stereo_pannerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters()), panner()
#endif
{
    set_pan_method(MONO_PANNER);
    set_pan(0.0f);
}

True_stereo_pannerAudioProcessor::~True_stereo_pannerAudioProcessor()
{
}

//==============================================================================
const juce::String True_stereo_pannerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool True_stereo_pannerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool True_stereo_pannerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool True_stereo_pannerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double True_stereo_pannerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int True_stereo_pannerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int True_stereo_pannerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void True_stereo_pannerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String True_stereo_pannerAudioProcessor::getProgramName (int index)
{
    return {};
}

void True_stereo_pannerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void True_stereo_pannerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;
    panner.prepare(spec);
}

void True_stereo_pannerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool True_stereo_pannerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void True_stereo_pannerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto audioBlock = juce::dsp::AudioBlock<float> (buffer);
    auto processContext = juce::dsp::ProcessContextReplacing<float> (audioBlock);
    
    set_pan_method((panMethod)(apvts.getRawParameterValue(ParameterID::panMethod.getParamID())->load()));
    switch (method) {
        case MONO_PANNER:
            panner.set_mono_panner_rule((juce::dsp::PannerRule)(apvts.getRawParameterValue(ParameterID::monoPannerRule.getParamID())->load()));
            break;
        case STEREO_PANNER:
            break;
        default:
            break;
    }
    
    panner.set_pan(apvts.getRawParameterValue(ParameterID::panValue.getParamID())->load());
    panner.process(processContext);
}

//==============================================================================
bool True_stereo_pannerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* True_stereo_pannerAudioProcessor::createEditor()
{
    return new True_stereo_pannerAudioProcessorEditor (*this);
}

//==============================================================================
void True_stereo_pannerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void True_stereo_pannerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new True_stereo_pannerAudioProcessor();
}

void True_stereo_pannerAudioProcessor::set_pan(float pan) {
    panner.set_pan(pan);
}

void True_stereo_pannerAudioProcessor::set_pan_method(panMethod method) {
    panner.set_pan_method(method);
}

juce::AudioProcessorValueTreeState::ParameterLayout True_stereo_pannerAudioProcessor::createParameters() {
    
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    juce::AudioProcessorValueTreeState::ParameterLayout paramsLayout;
    
    params.push_back(std::make_unique<juce::AudioParameterInt> (ParameterID::monoPannerRule, "Mono Panner Rule", 1, 6, 1));
    params.push_back(std::make_unique<juce::AudioParameterFloat> (ParameterID::panValue, "Panning", juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt> (ParameterID::panMethod, "Panning Method", 1, 3, 1));
    paramsLayout.add(params.begin(), params.end());
    
    return paramsLayout;
}
